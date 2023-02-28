#include "ap_int.h"
#include "ap_utils.h"

int evict_normal_write(volatile unsigned char * array, volatile unsigned * config, volatile unsigned char * enable){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=config depth=1024 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=array depth=32768 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=ENABLER port=enable offset=off


	volatile int sum = 0;
	volatile unsigned char value;

	int writes = config[0];
	int stride = config[1];
	int mode = config[2];
	int write_after_enable = config[3];
	int write_before_enable = config[4];
	int interleaving_cycle = config[5];

	switch(mode){
	case 0:
		// read+write to check if is in cache and then possibly evict
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
			array[i*stride] = value;
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		// second read to check if is in cache again
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		break;
	case 1:
		// read+write+read again right away
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
			array[i*stride] = value;
			for(volatile int j=0; j<interleaving_cycle; j++);
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		break;

	case 2:
		// old eviction method. Time then evict then time
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		//eviction

		*enable = 1;
		if(write_before_enable){
			sum = 15;
			config[1023] = sum;
			config[1022] = sum/2;
		}
		for(int i=0; i<writes; i++){
			array[i*stride] = 46;
			for(volatile int j=0; j<interleaving_cycle; j++);
		}
		*enable = 0;
		if(write_after_enable){
			sum = 46;
			config[1023] = sum;
			config[1022] = sum/2;
		}

		// time again
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		break;

	case 3:
		// this is experimentary
		// read+evict+read on the single value
		for(int i=0; i<writes; i++){
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);

			// start the eviction process
			*enable = 1;
			if(write_before_enable){
				sum = 15;
				config[1023] = sum;
				config[1022] = sum/2;
			}
			array[i*stride] = value;
			for(volatile int j=0; j<interleaving_cycle; j++);
			*enable = 0;
			if(write_after_enable){
				sum = 46;
				config[1023] = sum;
				config[1022] = sum/2;
			}
			for(volatile int j=0; j<interleaving_cycle; j++);
			value = array[i*stride];
			for(volatile int j=0; j<interleaving_cycle; j++);
		}

		break;

	default:
		// this should not appen
		return -1;
	}

	return 0;

}
