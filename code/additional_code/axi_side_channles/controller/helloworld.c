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
#include <xside_channel_finder.h>



int main()
{
    init_platform();
    print("Starting side channel finder\n\r");
    XSide_channel_finder finder;
    unsigned char array[4096];

    // touching the relevant values to bring them into cache
    array[0] = 46;
    array[256] = 46;
    array[512] = 46;
//    array[768] = 46;
    array[1024] = 46;
	array[1280] = 46;
	array[1536] = 46;
	array[1792] = 46;

//	Xil_L1DCacheFlush();
//	isb();
//	dsb();
//
//
//	array[0] = 46;
//	array[256] = 46;
//	array[512] = 46;
//	array[768] = 46;

//	Xil_L1DCacheInvalidate(&array[256]);
//	Xil_L1DCacheInvalidate(&array[768]);
//	Xil_L1DCacheInvalidate(&array[1280]);
//	Xil_L1DCacheInvalidate(&array[1792]);
    // configuring and starting the fpga

    XSide_channel_finder_Initialize(&finder, 0);
    XSide_channel_finder_Set_array_r(&finder, (UINTPTR)array);
    XSide_channel_finder_Start(&finder);

    while(!XSide_channel_finder_IsDone(&finder));


    print("Successfully ran Hello World application\n\r");
    cleanup_platform();
    return 0;
}
