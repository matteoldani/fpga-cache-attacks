/**
 * Semester project Fall 2022
 * @file core.cpp
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code implements AES (T-table), inspired by the code provided by
 * https://github.com/gpg/libgcrypt on an FPGA
 * 
 */

#include<stdint.h>
#include <string.h>
#define STORE32H(x, y)                          \
do { uint32_t ttt = __builtin_bswap32 ((x));     \
      memcpy ((y), &ttt, 4); } while(0)

#define LOAD32H(x, y)                           \
do { memcpy (&(x), (y), 4);                    \
      (x) = __builtin_bswap32 ((x)); } while(0)

#define N_ROUNDS 10

// extract a byte
#define LTC_BYTE(x, n) ((unsigned char)((x) >> (8 * (n))))

static uint32_t setup_mix(uint32_t temp, uint32_t *Te4_0, uint32_t *Te4_1, uint32_t *Te4_2, uint32_t *Te4_3){
   return (Te4_3[LTC_BYTE(temp, 2)]) ^
          (Te4_2[LTC_BYTE(temp, 1)]) ^
          (Te4_1[LTC_BYTE(temp, 0)]) ^
          (Te4_0[LTC_BYTE(temp, 3)]);
}

void key_expansion(uint8_t * key, uint32_t * enc_key, uint32_t *rcon,
		uint32_t *Te4_0, uint32_t *Te4_1, uint32_t *Te4_2, uint32_t *Te4_3){
    LOAD32H(enc_key[0], key     );
    LOAD32H(enc_key[1], key +  4);
    LOAD32H(enc_key[2], key +  8);
    LOAD32H(enc_key[3], key + 12);

    int i=0;

    for (;;) {
#pragma HLS LATENCY

        uint32_t temp  = enc_key[3];
        enc_key[4] = enc_key[0] ^ setup_mix(temp, Te4_0, Te4_1, Te4_2, Te4_3) ^ rcon[i];
        enc_key[5] = enc_key[1] ^ enc_key[4];
        enc_key[6] = enc_key[2] ^ enc_key[5];
        enc_key[7] = enc_key[3] ^ enc_key[6];
        if (++i == 10) {
            break;
        }
        enc_key += 4;
    }
}

void encrypt_block(
		uint32_t * enc_key, uint8_t * ptxt, uint8_t * ctxt,
		uint32_t *te0, uint32_t *te1, uint32_t *te2, uint32_t *te3,
		uint32_t *Te4_0, uint32_t *Te4_1, uint32_t *Te4_2, uint32_t *Te4_3){


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

int encrypt(
		uint8_t * key, uint8_t * ptxt, uint8_t * ctxt,
		uint32_t *te0, uint32_t *te1, uint32_t *te2, uint32_t *te3, uint32_t *rcon,
		uint32_t *Te4_0, uint32_t *Te4_1, uint32_t *Te4_2, uint32_t *Te4_3){

	#pragma HLS INTERFACE mode=s_axilite port=return

	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=key offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=Te4_0 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=te3 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=te1 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=Te4_3 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=Te4_1 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=te2 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=rcon offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=Te4_2 offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=ctxt offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=ptxt offset=slave
	#pragma HLS INTERFACE mode=m_axi bundle=MASTER port=te0 offset=slave

    uint32_t enc_key[60];
    key_expansion(key, enc_key, rcon, Te4_0, Te4_1, Te4_2, Te4_3);
    encrypt_block(enc_key, ptxt, ctxt, te0, te1, te2, te3, Te4_0, Te4_1, Te4_2, Te4_3);

    return 0;

}
