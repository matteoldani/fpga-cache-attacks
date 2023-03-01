/**
 * Semester project Fall 2022
 * @file helloworld.c
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
