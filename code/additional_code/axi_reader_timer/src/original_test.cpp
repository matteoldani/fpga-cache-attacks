/**
 * Semester project Fall 2022
 * @file original_test.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ap_int.h"
#include "ap_utils.h"

unsigned char reader_timer(volatile unsigned char * array, unsigned * c_results,
		volatile unsigned char * c_valid, volatile unsigned * counter){

	#pragma HLS INTERFACE mode=s_axilite port=return
	#pragma HLS INTERFACE ap_none port=c_valid
	#pragma HLS INTERFACE ap_none port=counter
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=c_results offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array offset=slave

	int counter_offset = 512;

	// Reading in cache L1 (Address already read on the controller side)
	for(int i=0;i<10;i++){
		c_results[i] = array[0];
		while((*c_valid) == 0);
		c_results[i+counter_offset] = *counter;
	}


	// evicting and then reading
	for(int i=10; i<20;i++){
		array[0] = 100;
		c_results[i] = array[0];
		while((*c_valid) == 0);
		c_results[i+counter_offset] = *counter;
	}

	// evicting with different value and then reading
	for(int i=20; i<30;i++){
		array[0] = i;
		c_results[i] = array[0];
		while((*c_valid) == 0);
		c_results[i+counter_offset] = *counter;
	}

	// reading address not in cache, then it should be in L2
	for(int i=30; i<40;i++){
		c_results[i]= array[512];
		while((*c_valid) == 0);
		c_results[i+counter_offset] = *counter;
	}

	return 0;

}
