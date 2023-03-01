/**
 * Semester project Fall 2022
 * @file aes.c
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code is extracted from the T-Table implementation provided by 
 * https://github.com/gpg/libgcrypt 
 * 
 */

#include "aes.h"

void encrypt(uint8_t * key, uint8_t * ptxt, uint8_t * ctxt){

    uint32_t enc_key[60]; 

    key_expansion(key, enc_key);
    encrypt_block(enc_key, ptxt, ctxt);
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
        t1 = 
            te0[LTC_BYTE(s1, 3)] ^ te1[LTC_BYTE(s2, 2)] ^ 
            te2[LTC_BYTE(s3, 1)] ^ te3[LTC_BYTE(s0, 0)] ^ 
            enc_key[4*i + 1];
        t2 = 
            te0[LTC_BYTE(s2, 3)] ^ te1[LTC_BYTE(s3, 2)] ^ 
            te2[LTC_BYTE(s0, 1)] ^ te3[LTC_BYTE(s1, 0)] ^ 
            enc_key[4*i + 2];
        t3 = 
            te0[LTC_BYTE(s3, 3)] ^ te1[LTC_BYTE(s0, 2)] ^ 
            te2[LTC_BYTE(s1, 1)] ^ te3[LTC_BYTE(s2, 0)] ^ 
            enc_key[4*i + 3];


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
