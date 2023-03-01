/**
 * Semester project Fall 2022
 * @file axi_reader_timer.c
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
#include <xreader_timer.h>

XReader_timer reader_timer;

volatile unsigned char value[1024];
volatile unsigned int c_result[1024];

int main()
{
    init_platform();

    Xil_DCacheFlush();
//    Xil_L2CacheDisable();
    Xil_L1DCacheDisable();
    Xil_L1ICacheDisable();
    dsb();
    isb();

    xil_printf("\e[1;1H\e[2J");
    xil_printf("Starting the reader with timer\n\n\r");
    xil_printf("This program will place 45 in value[0], time the access to it.\n\r"
    		"Then access value[512] and time its access too.\n\r"
    		"Finally it will write 100 in value[0] and time again\n\n\r");


    volatile unsigned char ret_val = 0;

    value[0] = 100;

    for(int i=0; i<4; i++){
    	c_result[i] = 0;
    }

    xil_printf("Initial Value[0]:\t%d\n\r", value[0]);


    // setup the reader
    XReader_timer_Initialize(&reader_timer, XPAR_READER_TIMER_0_DEVICE_ID);
    XReader_timer_Set_array_r(&reader_timer, (UINTPTR) value);
    XReader_timer_Set_c_results(&reader_timer, (UINTPTR) c_result);

    // starting the reader
    XReader_timer_Start(&reader_timer);

    while(!XReader_timer_IsDone(&reader_timer));

    xil_printf("Timing results for value[0], is should be in L1 cache\n\r");
    for(int i=0; i<10; i++){
    	xil_printf("Final value[0]:   %d,\ttime: %d\taddress: 0x%p\n\r", c_result[i], c_result[512+i], &c_result[i]);
    }

    xil_printf("Timing results for value[0] with eviction, it should be in L2 cache\n\r");
    for(int i=10; i<20; i++){
        xil_printf("Final value[0]:   %d,\ttime: %d\taddress: 0x%p\n\r", c_result[i], c_result[512+i], &c_result[i]);
    }

    xil_printf("Timing results for value[0] with eviction with different value, it should be in L2 cache\n\r");
    for(int i=20; i<30; i++){
        xil_printf("Final value[0]:   %d,\ttime: %d\taddress: 0x%p\n\r", c_result[i], c_result[512+i], &c_result[i]);
    }

    xil_printf("Timing results for value[512], it should be out of cache\n\r");
    for(int i=30; i<40; i++){
        xil_printf("Final value[512]: %d,\ttime: %d\taddress: 0x%p\n\r", c_result[i], c_result[512+i], &c_result[i]);
    }


    cleanup_platform();
    return 0;
}
