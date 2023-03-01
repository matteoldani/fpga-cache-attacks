/**
 * Semester project Fall 2022 
 * @file axi_master_reader_acp.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xset_mem.h>

#define BASE_ADDR 0x00010000

volatile unsigned char * memory = (unsigned char *)BASE_ADDR;

XSet_mem xset_mem;


int main()
{
    init_platform();

    xil_printf("Starting the AXI master interface\n\r");
    int err = XSet_mem_Initialize(&xset_mem, XPAR_SET_MEM_0_DEVICE_ID);
    if (err != XST_SUCCESS){
    	xil_printf("An error occurred: %d\n\r", err);
    }

    // set the memory to 0
    for(int i=0; i<256; i++){
    	memory[i] = 0;
    }

    // set the address of the master interface
    uintptr_t base_addr = (uintptr_t)memory;
    XSet_mem_Set_addr(&xset_mem, BASE_ADDR);
    xil_printf("The address value is: %p\n\r", base_addr);

    // setting the value to be written in memory
    XSet_mem_Set_value_r(&xset_mem, 46);

    XSet_mem_Start(&xset_mem);

    // waiting the completatoin of the set_mem IP
    while(!XSet_mem_IsDone(&xset_mem));

    err = XSet_mem_Get_return(&xset_mem);
    xil_printf("The result from the AXI Master was: %d\n\r", err);

    xil_printf("Checking the memory\n\n\r");
    for(int i=0;i<256;i++){
    	xil_printf("Value of memory[%d]: %d\n\r", i, memory[i]);
    }

    print("Successfully ran memory setter application");
    cleanup_platform();
    return 0;
}
