/**
 * Semester project Fall 2022
 * @file test_axi_signals.c
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
#include <xaxi_mem_tester.h>

XAxi_mem_tester mem_tester;

int main()
{
    init_platform();

    volatile unsigned char array[1024];
    array[0] = 0;
    array[1] = 1;
    array[2] = 2;

    xil_printf("\e[1;1H\e[2J");
    xil_printf("Starting the mem tester application \n\r");
    xil_printf("Base address of array: 0x%p\n\r", array);
    xil_printf("Initial array[0] = %d\n\r", array[0]);
    xil_printf("Initial array[1] = %d\n\r", array[1]);
    xil_printf("Initial array[2] = %d\n\r", array[2]);

    XAxi_mem_tester_Initialize(&mem_tester, XPAR_AXI_MEM_TESTER_0_DEVICE_ID);
    XAxi_mem_tester_Set_array_r(&mem_tester, (UINTPTR)array);

    XAxi_mem_tester_Start(&mem_tester);

    while(!XAxi_mem_tester_IsDone(&mem_tester));

    int return_value = XAxi_mem_tester_Get_return(&mem_tester);

    xil_printf("Final array[0] = %d\n\r", array[0]);
    xil_printf("Final array[1] = %d\n\r", array[1]);
    xil_printf("Final array[2] = %d\n\r", array[2]);
    xil_printf("Return value: %d\n\r", return_value);

    xil_printf("Array address at 1000: 0x%p\n\r", &array[1000]);

    cleanup_platform();
    return 0;
}
