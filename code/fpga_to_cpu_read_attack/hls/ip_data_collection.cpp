/**
 * Semester project Fall 2022
 * @file ip_data_collection.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the custom IP component used for the data
 * collection. Given the address of the beginning of a T-Table it 
 * will time and evict the whole table (256 entries of 4 bytes each).
 * 
 */

#include "ap_int.h"
#include "ap_utils.h"


int reader_timer_evicter(
		volatile unsigned char * array, volatile unsigned * c_results,
		volatile unsigned char * c_valid, volatile unsigned * counter,
		volatile unsigned char * enable){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE ap_none port=c_valid
	#pragma HLS INTERFACE ap_none port=counter

	#pragma HLS INTERFACE mode=m_axi bundle=ENABLER port=enable offset=off
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=c_results depth=1024 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array depth=32768 offset=slave


	volatile int sum = 0;
	volatile unsigned char value;

	volatile unsigned evictions = 256;
	volatile unsigned stride = 4;


	for(unsigned e=0; e<evictions; e++){
		// timing array
		value = array[e*stride];
		for(volatile int j=0; j<10;j++);
		while((*c_valid) == 0);
		sum += *counter;
		c_results[e] = *counter;
		for(volatile int j=0; j<10;j++);
	}

	*enable = 1;
	c_results[1023] = sum;
	c_results[1022] = sum/2;
	for(unsigned e=0; e<evictions; e++){
		array[e*stride] = 46;
	}
	*enable = 0;

	// Fake writes to propagate the enable
	c_results[1023] = sum;
	c_results[1022] = sum/2;

	return sum;

}
