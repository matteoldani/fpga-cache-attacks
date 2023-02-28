/**
 * @file evicter.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements a first test to prove the eviction 
 * abilities of the Attacker IP
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xreader_timer_evicter.h>

int main()
{

	XReader_timer_evicter evicter;
	volatile unsigned char array[8*4096];
	volatile unsigned int results[1024];

    init_platform();

    Xil_DCacheFlush();
    dsb();
    isb();

    xil_printf("\e[1;1H\e[2J");

    int number_of_eviction = 8;
    results[0] = number_of_eviction;

    for(int i=0; i<number_of_eviction; i++){
    	array[i] = i;
    }

	for(int i=0; i<number_of_eviction; i++){
		xil_printf("Initial Value array[%d]:\t%d \n\r", i, array[i]);
	}
	xil_printf("\n\r");

	XReader_timer_evicter_Initialize(&evicter, 0);
	XReader_timer_evicter_Set_array_r(&evicter, (UINTPTR) array);
	XReader_timer_evicter_Set_c_results(&evicter, (UINTPTR) results);

	XReader_timer_evicter_Start(&evicter);

	while(!XReader_timer_evicter_IsDone(&evicter));

	for(int i=0; i<number_of_eviction; i++){
		xil_printf("Timing results array[%d]: \t%d <-- L1 \n\r", i, results[i]);
	}
	xil_printf("\n\r");
	for(int i=0; i<number_of_eviction; i++){
		xil_printf("Timing results array[%d]: \t%d <-- L2 \n\r", i, results[i+number_of_eviction]);
	}
	xil_printf("\n\r");
	for(int i=0; i<number_of_eviction; i++){
		xil_printf("Value array[%d]: \t%d <-- No change \n\r", i, results[i+number_of_eviction*2]);
	}

    cleanup_platform();
    return 0;
}
