/**
 * Semester project Fall 2022
 * @file core.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ap_int.h"
#include "ap_utils.h"


#define OFFSET 256
#define N 8

int side_channel_finder(volatile unsigned char * array){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array depth=4096 offset=slave


	volatile int sum = 0;
	volatile unsigned char value;

	// write the array for the first time which should be in cache
	for(int i =0; i<N; i++){
		array[i*OFFSET] = 46;

		// randomly wait to not issues writes in a stride
		for(volatile int j=0; j<10; j++);
	}

	return value;

}
