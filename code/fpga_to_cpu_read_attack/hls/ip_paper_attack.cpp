/**
 * Semester project Fall 2022
 * @file ip_data_collection.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the custom IP component used for the data
 * collection. Given the address of the beginning of a T-Table, the offset
 * at which the table should accesses and how many addresses should be
 * timed/evicted, it will time and evict that set of addresses.
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

	int evictions = c_results[0];
	int stride = c_results[1];
	int time_after_eviction = c_results[2];
	int timing_offset = evictions;
	int value_offset = evictions*2;


	for(int e=0; e<evictions; e++){
		// timing array
		value = array[e*stride];
		while((*c_valid) == 0);
		sum += *counter;
		c_results[e] = *counter;
	}

	*enable = 1;
	for(int e=0; e<evictions; e++){
		// evicting array
		array[e*stride] = 46;
	}
	*enable = 0;

	// Fake writes to propagate the enable
	c_results[1023] = sum;
	c_results[1022] = sum/2;
	for(int e=0; e<evictions; e++){
		if(time_after_eviction == 1){
			// timing array 0 again
			value = array[e*stride];
			while((*c_valid) == 0);
			// end of fakes
			c_results[value_offset + e] = value;
			c_results[timing_offset + e] = *counter;
		}
	}


	return sum;

}

