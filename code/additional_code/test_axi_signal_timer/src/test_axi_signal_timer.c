#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xaxi_mem_reader.h>

XAxi_mem_reader mem_reader;

int main()
{
	volatile unsigned char value;
    init_platform();

    xil_printf("\e[1;1H\e[2J");

    XAxi_mem_reader_Initialize(&mem_reader, XPAR_AXI_MEM_READER_0_DEVICE_ID);
    XAxi_mem_reader_Set_array_r(&mem_reader, (UINTPTR) (&value));

    value = 11;
    XAxi_mem_reader_Start(&mem_reader);

    xil_printf("The value is: %d\n\r", value);
    while(!XAxi_mem_reader_IsDone(&mem_reader));
    xil_printf("The value is: %d\n\r", value);
    unsigned char ret  = XAxi_mem_reader_Get_return(&mem_reader);
    xil_printf("The retrun value is: %d\n\r", ret);
    cleanup_platform();
    return 0;
}
