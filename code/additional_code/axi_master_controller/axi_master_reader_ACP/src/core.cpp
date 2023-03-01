/**
 * Semester project Fall 2022 
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 */

int set_mem(unsigned char value, unsigned char * addr){

	#pragma HLS INTERFACE mode=s_axilite bundle=CTRL_BUS port=return
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER depth=256 port=addr offset=slave
	#pragma HLS INTERFACE mode=s_axilite bundle=CTRL_BUS port=value

	for(int i=0; i<256; i++){
		addr[i] = value;
	}

	return 0;
}
