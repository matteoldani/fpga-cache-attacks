#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_testmem.h"
#include "mmu_control.h"

//#define MEM_TEST_START_ADDRESS XPAR_DDR_MEM_BASEADDR
#define MEM_TEST_START_ADDRESS 0x20000000
#define MEM_TEST_END_ADDRESS 0x2FFFFFFF

#define MEMORY_TEST_PATTERN 0xA1B2C3D4

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


int main()
{
    init_platform();
    xil_printf("\e[1;1H\e[2J");
    xil_printf(ANSI_COLOR_CYAN); xil_printf("########## MEMEORY TESTER ##########\n\r"); xil_printf(ANSI_COLOR_RESET);

    xil_printf("Testing increasing size of malloc\n\r");
    int i=0;
    for(; i<20; i++){
    	volatile char * array = malloc(sizeof(char) * (1ULL << i));
    	xil_printf("Allocation of array with size: 2^%d\n\r", i);
    	xil_printf("Array base point: 0x%p\n\r", array);
    	xil_printf("Array end  point: 0x%p\n\r", &array[(1ULL << i)- 1]);

    	if(array != 0){
    		xil_printf("The array is valid, touching all the memory");
    		volatile int sum = 0;
    		for(int j=0; j<(1ULL << i); j++){
    				array[j] = j;
    				sum += array[j];
    		}

    		xil_printf(" %d\n\r", sum);

    	}else{
    		xil_printf("Memory is invalid\n\r");
    		break;
    	}
    	free(array);
    }

    xil_printf("Trying to allocate more memory of the max size\n\r");
    volatile char * array2;
    do{
    	array2 = malloc(sizeof(char) * (1ULL << i));
    	xil_printf("Allocation of array with size: 2^%d\n\r", i);
    	xil_printf("Array2 starting point: 0x%p\n\r", array2);
    	i--;
    }while(array2 == 0);

    xil_printf(ANSI_COLOR_MAGENTA"@@@@@@@@@@ TEST DONE @@@@@@@@@@\n\r"ANSI_COLOR_RESET);

    cleanup_platform();
    return 0;
}
