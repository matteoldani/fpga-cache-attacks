/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xset_mem.h>

volatile unsigned char * memory = 0x00010000;

XSet_mem xset_mem;


int main()
{
    init_platform();

    xil_printf("Starting the AXI master interface\n\r");
    Xil_DCacheDisable();
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
    XSet_mem_Set_addr(&xset_mem, 0x00010000);
    xil_printf("The address value is: %p\n\r", base_addr);

    // setting the value to be written in memory
    XSet_mem_Set_value_r(&xset_mem, 42);

    XSet_mem_Start(&xset_mem);

    // waiting the completatoin of the set_mem IP
    while(!XSet_mem_IsDone(&xset_mem));

    err = XSet_mem_Get_return(&xset_mem);
    xil_printf("The result from the AXI Master was: %d\n\r", err);

    xil_printf("Checking the memory\n\n\r");
    for(int i=0;i<256;i++){
    	xil_printf("Value of memory[%d]: %d\n\r", i, memory[i]);
    }

    for(int i=0; i<0x0003ffff; i++){
    	if(memory[i] == 42){
    		xil_printf("Memory is 42 at: %p", &memory[i]);
    	}
    }

    print("Successfully ran memory setter application");
    cleanup_platform();
    return 0;
}
