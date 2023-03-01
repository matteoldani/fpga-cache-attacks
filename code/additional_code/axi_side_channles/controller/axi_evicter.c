/**
 * Semester project Fall 2022
 * @file axi_evicter.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xreader_timer_evicter.h>
#include "cycle.h"
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

void encrypt(uint8_t * key, uint8_t * ptxt, uint8_t * ctxt){

    uint32_t enc_key[60];

    key_expansion(key, enc_key);
    encrypt_block(enc_key, ptxt, ctxt);

    // printf("The encrypted block is: ");

    // for(int i=0; i<16; i++){
    //     printf("%d ", ctxt[i]);
    // }
    // printf("\n");
}

static uint32_t setup_mix(uint32_t temp){
   return (Te4_3[LTC_BYTE(temp, 2)]) ^
          (Te4_2[LTC_BYTE(temp, 1)]) ^
          (Te4_1[LTC_BYTE(temp, 0)]) ^
          (Te4_0[LTC_BYTE(temp, 3)]);
}

void key_expansion(uint8_t * key, uint32_t * enc_key){
    LOAD32H(enc_key[0], key     );
    LOAD32H(enc_key[1], key +  4);
    LOAD32H(enc_key[2], key +  8);
    LOAD32H(enc_key[3], key + 12);

    int i=0;

    for (;;) {
        uint32_t temp  = enc_key[3];
        enc_key[4] = enc_key[0] ^ setup_mix(temp) ^ rcon[i];
        enc_key[5] = enc_key[1] ^ enc_key[4];
        enc_key[6] = enc_key[2] ^ enc_key[5];
        enc_key[7] = enc_key[3] ^ enc_key[6];
        if (++i == 10) {
            break;
        }
        enc_key += 4;
    }
}

void encrypt_block(uint32_t * enc_key, uint8_t * ptxt, uint8_t * ctxt){


    // State initialization
    uint32_t s0, s1, s2, s3;
    uint32_t t0, t1, t2, t3;

    LOAD32H(s0, ptxt      ); s0 ^= enc_key[0];
    LOAD32H(s1, ptxt  +  4); s1 ^= enc_key[1];
    LOAD32H(s2, ptxt  +  8); s2 ^= enc_key[2];
    LOAD32H(s3, ptxt  + 12); s3 ^= enc_key[3];

    for(int i=1; i<N_ROUNDS; i++){
        t0 =
            te0[LTC_BYTE(s0, 3)] ^ te1[LTC_BYTE(s1, 2)] ^
            te2[LTC_BYTE(s2, 1)] ^ te3[LTC_BYTE(s3, 0)] ^
            enc_key[4*i + 0];
        //xil_printf("LTC_BYTE: %d\t", LTC_BYTE(s0, 3));

        t1 =
            te0[LTC_BYTE(s1, 3)] ^ te1[LTC_BYTE(s2, 2)] ^
            te2[LTC_BYTE(s3, 1)] ^ te3[LTC_BYTE(s0, 0)] ^
            enc_key[4*i + 1];
        //xil_printf("LTC_BYTE: %d\t", LTC_BYTE(s1, 3));
        t2 =
            te0[LTC_BYTE(s2, 3)] ^ te1[LTC_BYTE(s3, 2)] ^
            te2[LTC_BYTE(s0, 1)] ^ te3[LTC_BYTE(s1, 0)] ^
            enc_key[4*i + 2];
        //xil_printf("LTC_BYTE: %d\t", LTC_BYTE(s2, 3));
        t3 =
            te0[LTC_BYTE(s3, 3)] ^ te1[LTC_BYTE(s0, 2)] ^
            te2[LTC_BYTE(s1, 1)] ^ te3[LTC_BYTE(s2, 0)] ^
            enc_key[4*i + 3];
        //xil_printf("LTC_BYTE: %d\n\r", LTC_BYTE(s3, 3));


        s0 = t0;
        s1 = t1;
        s2 = t2;
        s3 = t3;

    }

    /*
     * apply last round and
     * map cipher state to byte array block:
     */
    s0 =
        (Te4_3[LTC_BYTE(t0, 3)]) ^ (Te4_2[LTC_BYTE(t1, 2)]) ^
        (Te4_1[LTC_BYTE(t2, 1)]) ^ (Te4_0[LTC_BYTE(t3, 0)]) ^
        enc_key[4*N_ROUNDS + 0];
    STORE32H(s0, ctxt);
    s1 =
        (Te4_3[LTC_BYTE(t1, 3)]) ^ (Te4_2[LTC_BYTE(t2, 2)]) ^
        (Te4_1[LTC_BYTE(t3, 1)]) ^ (Te4_0[LTC_BYTE(t0, 0)]) ^
        enc_key[4*N_ROUNDS + 1];
    STORE32H(s1, ctxt+4);
    s2 =
        (Te4_3[LTC_BYTE(t2, 3)]) ^ (Te4_2[LTC_BYTE(t3, 2)]) ^
        (Te4_1[LTC_BYTE(t0, 1)]) ^ (Te4_0[LTC_BYTE(t1, 0)]) ^
        enc_key[4*N_ROUNDS + 2];
    STORE32H(s2, ctxt+8);
    s3 =
        (Te4_3[LTC_BYTE(t3, 3)]) ^ (Te4_2[LTC_BYTE(t0, 2)]) ^
        (Te4_1[LTC_BYTE(t1, 1)]) ^ (Te4_0[LTC_BYTE(t2, 0)]) ^
        enc_key[4*N_ROUNDS + 3];
    STORE32H(s3, ctxt+12);
}

//volatile unsigned char array[8*4096];
volatile unsigned int results[1024];
unsigned int attack_results[256*256];

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

    ticks t1, t2;
	volatile char sum = 0;
	volatile uint32_t * array = (te0);

	unsigned char key[16]  = {0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51};
//	unsigned char key[16]  = {15,0,73,214,97,20,3,111,139,202,70,62,103,249,4,46};
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

	for(int i=0; i<10; i++){
	for(int a=0; a<256; a++){
		//loop over all the possible positions

		// reset the plain text
		for(int i=0; i<16; i++){
			ptxt[i] = 0;
		}

		for(int b=0; b<256; b++){
			xil_printf("%d %d %d\r", i, a, b);
			//loop over all the possible plain texts
			ptxt[0] = b;
			rand_ptxt(ptxt, 0);

			encrypt(key, ptxt, ctxt);

//			xil_printf(ANSI_COLOR_RED);
//			xil_printf("################ Te0 ################\n\n\r");
//			xil_printf(ANSI_COLOR_RESET);
			results[0] = number_of_eviction;
			results[1] = STRIDE * 4;
			results[2] = 0; // time after eviction

			XReader_timer_evicter_Start(&evicter);

			while(!XReader_timer_evicter_IsDone(&evicter));

			// I just need to time the correspongind value of the 256
//			for(int i=0; i<256;i++){
//				if(results[i] == 12){
//					attack_results[i*n + b] +=1;
//				}
//			}

			if(results[a] == 12){
				attack_results[a*n + b] += 1;
			}

//			xil_printf("Timing results array[i]: \n\r");
//			for(int i=0; i<number_of_eviction; i=i+64){
//				for(int j=i; j<i+64; j++){
//					xil_printf_colored(results[j]);
//				}
//				xil_printf("\n\r");
//			}



		}

	}
	}

	for(int i=0; i<256; i++){
		for(int j=0; j<256; j++){
			xil_printf("%d ", attack_results[i*n + j]);
		}
		xil_printf("\n\r");
	}

    cleanup_platform();
    return 0;
}
