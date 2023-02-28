int axi_mem_tester(unsigned char * array){

	#pragma HLS TOP name=axi_mem_tester
	#pragma HLS INTERFACE mode=s_axilite bundle=CTL_BUS port=return
	#pragma HLS INTERFACE mode=m_axi bundle=M_ADDR depth=1024 port=array offset=slave

	// I have to make a read

	unsigned char value1 = array[0];
	unsigned char value2 = array[1];
	unsigned char value3 = array[2];

	// I have to make a write
	array[0] = value1*2;
	array[1] = value2*2;
	array[2] = value3*2;

	value1 = array[512];
	value2 = array[513];
	value3 = array[1000];

	return value1 + value2 + value3;

}
