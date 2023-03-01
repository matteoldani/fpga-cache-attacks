/**
 * Semester project Fall 2022
 * @file core.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the Attacker IP for the write side channel 
 * attack. For the eviction it does not rely on the MUX as in the r
 * ead side channel. Instead, it will read the value at a certain 
 * address and then write the same value to it.
 * 
 */

#include "ap_int.h"
#include "ap_utils.h"

/**
 * Give an address, this function will access it while enabling the evicter. Evicted will be
 * disabled before the function returns
 */


int side_channel_finder(volatile unsigned char * array, volatile unsigned * c_results,
		volatile unsigned char * c_valid, volatile unsigned * counter,
		volatile unsigned char * enable){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE ap_none port=c_valid
	#pragma HLS INTERFACE ap_none port=counter

	#pragma HLS INTERFACE mode=m_axi bundle=ENABLER port=enable offset=off
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array depth=4096 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=c_results depth=1024 offset=slave



	volatile int sum = 0;
	volatile unsigned char value_write= 15;
	volatile unsigned char value_evict = 46;

	unsigned evictions = c_results[256];
	unsigned stride = c_results[257];
	unsigned timing_offset = c_results[258];

	// time the array without the eviction itself
	// to not change the value, read and then write

	// write the array for the first time which should be in cache
	volatile unsigned char te_value = array[(timing_offset)*stride];
	// randomly wait to not mess writes with reads
	// maybe not necessary
	for(volatile int j=0; j<10; j++);

	// write and time the write
	array[(timing_offset)*stride] = te_value;
	while((*c_valid) == 0);
	c_results[(timing_offset)] = *counter;

	// randomly wait to not issues writes in a stride
	for(volatile int j=0; j<10; j++);


	return sum;

}
