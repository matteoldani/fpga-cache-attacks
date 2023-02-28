/**
 * @file core.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the Optimized Attacker IP for the write side channel 
 * attack. For the eviction it does not rely on the MUX as in the r
 * ead side channel. Instead, it will read the value at a certain 
 * address and then write the same value to it.
 * 
 */

#include "ap_int.h"
#include "ap_utils.h"

int write_attack(volatile unsigned char *array, volatile unsigned *c_results,
		volatile unsigned char *c_valid, volatile unsigned *counter){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE ap_none port=c_valid
	#pragma HLS INTERFACE ap_none port=counter

	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array depth=4096 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=c_results depth=1024 offset=slave

	volatile int sum = 0;
	volatile unsigned char values[512];

	unsigned evictions     = c_results[1020];
	unsigned stride        = c_results[1021];
	unsigned access_offset = c_results[1022];
	unsigned inner_stride  = c_results[1023];


	for(volatile int i=0; i<evictions+1; i++){
		values[access_offset + (i*inner_stride)] = array[access_offset*4 + (i*stride)];
	}

	for(volatile int j=0; j<100; j++);

	for(volatile int i=0; i<evictions+1; i++){
		array[access_offset*4 + (i*stride)] = values[access_offset + (i*inner_stride)];
		for(volatile int j=0; j<10; j++){
			sum += *c_valid;
		}
		while((*c_valid) == 0);
		values[access_offset + (i*inner_stride)] = *counter;
	}

	for(volatile int i=1; i<evictions+1; i++){
		c_results[access_offset + ((i-1)*inner_stride)] = values[access_offset + (i*inner_stride)];
	}

	return 0;
}
