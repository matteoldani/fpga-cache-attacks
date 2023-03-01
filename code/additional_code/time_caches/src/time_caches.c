/**
 * Semester project Fall 2022
 * @file time_caches.c
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
#include "cycle.h"
#include "xtime_l.h"
#include "xpseudo_asm_gcc.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define STRIDE 5000
#define N 4
#define TIMER 2 // 0 is the original one, 1 is the new one, 2 is the asm one

static __inline__ unsigned time_read_mem(unsigned char * addr){

	unsigned cc;

	// Resetting the timer
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 2" :: "r"(1<<31)); /* stop the cc */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(5));     /* initialize */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1<<31)); /* start the cc */

	// reading the value inside a register
	__asm__ __volatile__ ("ldr r9, [%0]" :: "r"(addr) : "r9");
	// store the value inside another register otherwise the read is not issues and the time is fized at 9
	__asm__ __volatile__ ("mov r8, r9" : : : "r9");

	// time the access
	__asm__ __volatile__ ("mrc p15, 0, r10, c9, c13, 0");
	__asm__ __volatile__ ("mov %0, r10" : "=r"(cc) : : "r9");


	return cc;

}


int main()
{
    init_platform();

    register ticks t1, t2;
    unsigned char array[4096*8];

    xil_printf("\e[1;1H\e[2J");
    xil_printf("\e[1;1H\e[2J");
    xil_printf(MAGENTA"Running timer tests with timer: ");
    switch(TIMER){
    case 0: xil_printf("ORIGINAL\n\r"RESET); break;
    case 1: xil_printf("IMPROVED\n\r"RESET); break;
    case 2: xil_printf("ASM\n\r"RESET); break;
    default: xil_printf("timer not recognized, exiting\n\r"); return -1;
    }

    volatile int sum;

    xil_printf(YELLOW"Bringing data into L1 \n\r"RESET);

    for(int i=0; i<N; i++){
		t1 = getticks();
		sum = array[i*STRIDE];
		t2 = getticks();
    }

    xil_printf(CYAN"\n\rData is in L1 \n\r"RESET);
    for(int i=0; i<N; i++){

    	switch(TIMER){
    	case 0:
    		t1 = getticks();
			sum = array[i*STRIDE];
			t2 = getticks();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, elapsed(t2, t1));
			break;
    	case 1:
    		start_timer();
			sum = array[i*STRIDE];
			t2 = stop_timer();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
			break;
    	case 2:
    		t2 = time_read_mem(&array[i*STRIDE]);
    		xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
    		break;
    	default:
    		break;
    	}


    }

    xil_printf(YELLOW"\n\rInvalidating L1 cache lines\n\r"RESET);
	for(int i=0; i<N; i++){
		Xil_L1DCacheInvalidateLine(&array[i*STRIDE]);
		dsb();
		isb();
	}

    xil_printf(CYAN"\n\rData in L2 with all cache enables\n\r"RESET);
    for(int i=0; i<N; i++){

    	switch(TIMER){
    	case 0:
    		t1 = getticks();
			sum = array[i*STRIDE];
			t2 = getticks();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, elapsed(t2, t1));
			break;
    	case 1:
    		start_timer();
			sum = array[i*STRIDE];
			t2 = stop_timer();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
			break;
    	case 2:
    		t2 = time_read_mem(&array[i*STRIDE]);
    		xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
    		break;
    	default:
    		break;
    	}


    }


    xil_printf(YELLOW"\n\rFlushing L1 cache\n\r"RESET);

    xil_printf(CYAN"\n\rData in L2 with all cache enables\n\r"RESET);
    for(int i=0; i<N; i++){
    	Xil_L1DCacheFlush();
    	dsb();
    	isb();
    	switch(TIMER){
    	case 0:
    		t1 = getticks();
			sum = array[i*STRIDE];
			t2 = getticks();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, elapsed(t2, t1));
			break;
    	case 1:
    		start_timer();
			sum = array[i*STRIDE];
			t2 = stop_timer();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
			break;
    	case 2:
    		t2 = time_read_mem(&array[i*STRIDE]);
    		xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
    		break;
    	default:
    		break;
    	}


    }




    Xil_L1DCacheDisable();
    for(int i=0; i<N; i++){
		t1 = getticks();
		array[i*STRIDE] = 10;
		t2 = getticks();
	}

    xil_printf(CYAN"\n\rL1 Data cache disabled\n\r"RESET);
    for(int i=0; i<N; i++){

    	switch(TIMER){
    	case 0:
    		t1 = getticks();
			sum = array[i*STRIDE];
			t2 = getticks();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, elapsed(t2, t1));
			break;
    	case 1:
    		start_timer();
			sum = array[i*STRIDE];
			t2 = stop_timer();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
			break;
    	case 2:
    		t2 = time_read_mem(&array[i*STRIDE]);
    		xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
    		break;
    	default:
    		break;
    	}
    }

    Xil_L2CacheDisable();
	xil_printf(CYAN"\n\rL1d and L2 caches disabled\n\r"RESET);
    for(int i=0; i<N; i++){

    	switch(TIMER){
    	case 0:
    		t1 = getticks();
			sum = array[i*STRIDE];
			t2 = getticks();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, elapsed(t2, t1));
			break;
    	case 1:
    		start_timer();
			sum = array[i*STRIDE];
			t2 = stop_timer();

			xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
			break;
    	case 2:
    		t2 = time_read_mem(&array[i*STRIDE]);
    		xil_printf("Time to read array[%d*%d]: %u\n\r", i, STRIDE, t2);
    		break;
    	default:
    		break;
    	}
    }


    cleanup_platform();
    return 0;
}
