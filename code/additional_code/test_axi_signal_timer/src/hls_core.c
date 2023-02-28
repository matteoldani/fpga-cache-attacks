int axi_mem_reader(unsigned char * array){

	#pragma HLS INTERFACE mode=m_axi bundle=MASTER depth=1 port=array offset=slave
	#pragma HLS INTERFACE mode=s_axilite port=return

	volatile unsigned value = *array;

	return value;
}
