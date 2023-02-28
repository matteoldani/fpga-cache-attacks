/**
 * @file aes.h
 * @author Matteo Oldani
 * @version 0.1
 * @date 2023-01-18
 * 
 * This code is extracted from the T-Table implementation provided by 
 * https://github.com/gpg/libgcrypt 
 * 
 * Header file for the vulnerable AES implementation used as a POC for cache-based
 * attacks
 * 
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N_ROUNDS 10

#ifndef XMEMCPY
#define XMEMCPY memcpy
#endif

#define STORE32H(x, y)                          \
do { uint32_t ttt = __builtin_bswap32 ((x));     \
      XMEMCPY ((y), &ttt, 4); } while(0)

#define LOAD32H(x, y)                           \
do { XMEMCPY (&(x), (y), 4);                    \
      (x) = __builtin_bswap32 ((x)); } while(0)

// extract a byte
#define LTC_BYTE(x, n) ((unsigned char)((x) >> (8 * (n))))

void encrypt(uint8_t * key, uint8_t * ptxt, uint8_t * ctxt);


static const uint32_t te0[256] __attribute__((aligned(8))) = {

	0xc66363a5, 0xf87c7c84, 0xee777799, 0xf67b7b8d, 0xfff2f20d, 0xd66b6bbd, 0xde6f6fb1, 0x91c5c554,

	0x60303050, 0x02010103, 0xce6767a9, 0x562b2b7d, 0xe7fefe19, 0xb5d7d762, 0x4dababe6, 0xec76769a,

	0x8fcaca45, 0x1f82829d, 0x89c9c940, 0xfa7d7d87, 0xeffafa15, 0xb25959eb, 0x8e4747c9, 0xfbf0f00b,

	0x41adadec, 0xb3d4d467, 0x5fa2a2fd, 0x45afafea, 0x239c9cbf, 0x53a4a4f7, 0xe4727296, 0x9bc0c05b,

	0x75b7b7c2, 0xe1fdfd1c, 0x3d9393ae, 0x4c26266a, 0x6c36365a, 0x7e3f3f41, 0xf5f7f702, 0x83cccc4f,

	0x6834345c, 0x51a5a5f4, 0xd1e5e534, 0xf9f1f108, 0xe2717193, 0xabd8d873, 0x62313153, 0x2a15153f,

	0x0804040c, 0x95c7c752, 0x46232365, 0x9dc3c35e, 0x30181828, 0x379696a1, 0x0a05050f, 0x2f9a9ab5,

	0x0e070709, 0x24121236, 0x1b80809b, 0xdfe2e23d, 0xcdebeb26, 0x4e272769, 0x7fb2b2cd, 0xea75759f,

	0x1209091b, 0x1d83839e, 0x582c2c74, 0x341a1a2e, 0x361b1b2d, 0xdc6e6eb2, 0xb45a5aee, 0x5ba0a0fb,

	0xa45252f6, 0x763b3b4d, 0xb7d6d661, 0x7db3b3ce, 0x5229297b, 0xdde3e33e, 0x5e2f2f71, 0x13848497,

	0xa65353f5, 0xb9d1d168, 0x00000000, 0xc1eded2c, 0x40202060, 0xe3fcfc1f, 0x79b1b1c8, 0xb65b5bed,

	0xd46a6abe, 0x8dcbcb46, 0x67bebed9, 0x7239394b, 0x944a4ade, 0x984c4cd4, 0xb05858e8, 0x85cfcf4a,

	0xbbd0d06b, 0xc5efef2a, 0x4faaaae5, 0xedfbfb16, 0x864343c5, 0x9a4d4dd7, 0x66333355, 0x11858594,

	0x8a4545cf, 0xe9f9f910, 0x04020206, 0xfe7f7f81, 0xa05050f0, 0x783c3c44, 0x259f9fba, 0x4ba8a8e3,

	0xa25151f3, 0x5da3a3fe, 0x804040c0, 0x058f8f8a, 0x3f9292ad, 0x219d9dbc, 0x70383848, 0xf1f5f504,

	0x63bcbcdf, 0x77b6b6c1, 0xafdada75, 0x42212163, 0x20101030, 0xe5ffff1a, 0xfdf3f30e, 0xbfd2d26d,

	0x81cdcd4c, 0x180c0c14, 0x26131335, 0xc3ecec2f, 0xbe5f5fe1, 0x359797a2, 0x884444cc, 0x2e171739,

	0x93c4c457, 0x55a7a7f2, 0xfc7e7e82, 0x7a3d3d47, 0xc86464ac, 0xba5d5de7, 0x3219192b, 0xe6737395,

	0xc06060a0, 0x19818198, 0x9e4f4fd1, 0xa3dcdc7f, 0x44222266, 0x542a2a7e, 0x3b9090ab, 0x0b888883,

	0x8c4646ca, 0xc7eeee29, 0x6bb8b8d3, 0x2814143c, 0xa7dede79, 0xbc5e5ee2, 0x160b0b1d, 0xaddbdb76,

	0xdbe0e03b, 0x64323256, 0x743a3a4e, 0x140a0a1e, 0x924949db, 0x0c06060a, 0x4824246c, 0xb85c5ce4,

	0x9fc2c25d, 0xbdd3d36e, 0x43acacef, 0xc46262a6, 0x399191a8, 0x319595a4, 0xd3e4e437, 0xf279798b,

	0xd5e7e732, 0x8bc8c843, 0x6e373759, 0xda6d6db7, 0x018d8d8c, 0xb1d5d564, 0x9c4e4ed2, 0x49a9a9e0,

	0xd86c6cb4, 0xac5656fa, 0xf3f4f407, 0xcfeaea25, 0xca6565af, 0xf47a7a8e, 0x47aeaee9, 0x10080818,

	0x6fbabad5, 0xf0787888, 0x4a25256f, 0x5c2e2e72, 0x381c1c24, 0x57a6a6f1, 0x73b4b4c7, 0x97c6c651,

	0xcbe8e823, 0xa1dddd7c, 0xe874749c, 0x3e1f1f21, 0x964b4bdd, 0x61bdbddc, 0x0d8b8b86, 0x0f8a8a85,

	0xe0707090, 0x7c3e3e42, 0x71b5b5c4, 0xcc6666aa, 0x904848d8, 0x06030305, 0xf7f6f601, 0x1c0e0e12,

	0xc26161a3, 0x6a35355f, 0xae5757f9, 0x69b9b9d0, 0x17868691, 0x99c1c158, 0x3a1d1d27, 0x279e9eb9,

	0xd9e1e138, 0xebf8f813, 0x2b9898b3, 0x22111133, 0xd26969bb, 0xa9d9d970, 0x078e8e89, 0x339494a7,

	0x2d9b9bb6, 0x3c1e1e22, 0x15878792, 0xc9e9e920, 0x87cece49, 0xaa5555ff, 0x50282878, 0xa5dfdf7a,

	0x038c8c8f, 0x59a1a1f8, 0x09898980, 0x1a0d0d17, 0x65bfbfda, 0xd7e6e631, 0x844242c6, 0xd06868b8,

	0x824141c3, 0x299999b0, 0x5a2d2d77, 0x1e0f0f11, 0x7bb0b0cb, 0xa85454fc, 0x6dbbbbd6, 0x2c16163a,

};

static const uint32_t te1[256] __attribute__((aligned(64))) = {

	0xa5c66363, 0x84f87c7c, 0x99ee7777, 0x8df67b7b, 0x0dfff2f2, 0xbdd66b6b, 0xb1de6f6f, 0x5491c5c5,

	0x50603030, 0x03020101, 0xa9ce6767, 0x7d562b2b, 0x19e7fefe, 0x62b5d7d7, 0xe64dabab, 0x9aec7676,

	0x458fcaca, 0x9d1f8282, 0x4089c9c9, 0x87fa7d7d, 0x15effafa, 0xebb25959, 0xc98e4747, 0x0bfbf0f0,

	0xec41adad, 0x67b3d4d4, 0xfd5fa2a2, 0xea45afaf, 0xbf239c9c, 0xf753a4a4, 0x96e47272, 0x5b9bc0c0,

	0xc275b7b7, 0x1ce1fdfd, 0xae3d9393, 0x6a4c2626, 0x5a6c3636, 0x417e3f3f, 0x02f5f7f7, 0x4f83cccc,

	0x5c683434, 0xf451a5a5, 0x34d1e5e5, 0x08f9f1f1, 0x93e27171, 0x73abd8d8, 0x53623131, 0x3f2a1515,

	0x0c080404, 0x5295c7c7, 0x65462323, 0x5e9dc3c3, 0x28301818, 0xa1379696, 0x0f0a0505, 0xb52f9a9a,

	0x090e0707, 0x36241212, 0x9b1b8080, 0x3ddfe2e2, 0x26cdebeb, 0x694e2727, 0xcd7fb2b2, 0x9fea7575,

	0x1b120909, 0x9e1d8383, 0x74582c2c, 0x2e341a1a, 0x2d361b1b, 0xb2dc6e6e, 0xeeb45a5a, 0xfb5ba0a0,

	0xf6a45252, 0x4d763b3b, 0x61b7d6d6, 0xce7db3b3, 0x7b522929, 0x3edde3e3, 0x715e2f2f, 0x97138484,

	0xf5a65353, 0x68b9d1d1, 0x00000000, 0x2cc1eded, 0x60402020, 0x1fe3fcfc, 0xc879b1b1, 0xedb65b5b,

	0xbed46a6a, 0x468dcbcb, 0xd967bebe, 0x4b723939, 0xde944a4a, 0xd4984c4c, 0xe8b05858, 0x4a85cfcf,

	0x6bbbd0d0, 0x2ac5efef, 0xe54faaaa, 0x16edfbfb, 0xc5864343, 0xd79a4d4d, 0x55663333, 0x94118585,

	0xcf8a4545, 0x10e9f9f9, 0x06040202, 0x81fe7f7f, 0xf0a05050, 0x44783c3c, 0xba259f9f, 0xe34ba8a8,

	0xf3a25151, 0xfe5da3a3, 0xc0804040, 0x8a058f8f, 0xad3f9292, 0xbc219d9d, 0x48703838, 0x04f1f5f5,

	0xdf63bcbc, 0xc177b6b6, 0x75afdada, 0x63422121, 0x30201010, 0x1ae5ffff, 0x0efdf3f3, 0x6dbfd2d2,

	0x4c81cdcd, 0x14180c0c, 0x35261313, 0x2fc3ecec, 0xe1be5f5f, 0xa2359797, 0xcc884444, 0x392e1717,

	0x5793c4c4, 0xf255a7a7, 0x82fc7e7e, 0x477a3d3d, 0xacc86464, 0xe7ba5d5d, 0x2b321919, 0x95e67373,

	0xa0c06060, 0x98198181, 0xd19e4f4f, 0x7fa3dcdc, 0x66442222, 0x7e542a2a, 0xab3b9090, 0x830b8888,

	0xca8c4646, 0x29c7eeee, 0xd36bb8b8, 0x3c281414, 0x79a7dede, 0xe2bc5e5e, 0x1d160b0b, 0x76addbdb,

	0x3bdbe0e0, 0x56643232, 0x4e743a3a, 0x1e140a0a, 0xdb924949, 0x0a0c0606, 0x6c482424, 0xe4b85c5c,

	0x5d9fc2c2, 0x6ebdd3d3, 0xef43acac, 0xa6c46262, 0xa8399191, 0xa4319595, 0x37d3e4e4, 0x8bf27979,

	0x32d5e7e7, 0x438bc8c8, 0x596e3737, 0xb7da6d6d, 0x8c018d8d, 0x64b1d5d5, 0xd29c4e4e, 0xe049a9a9,

	0xb4d86c6c, 0xfaac5656, 0x07f3f4f4, 0x25cfeaea, 0xafca6565, 0x8ef47a7a, 0xe947aeae, 0x18100808,

	0xd56fbaba, 0x88f07878, 0x6f4a2525, 0x725c2e2e, 0x24381c1c, 0xf157a6a6, 0xc773b4b4, 0x5197c6c6,

	0x23cbe8e8, 0x7ca1dddd, 0x9ce87474, 0x213e1f1f, 0xdd964b4b, 0xdc61bdbd, 0x860d8b8b, 0x850f8a8a,

	0x90e07070, 0x427c3e3e, 0xc471b5b5, 0xaacc6666, 0xd8904848, 0x05060303, 0x01f7f6f6, 0x121c0e0e,

	0xa3c26161, 0x5f6a3535, 0xf9ae5757, 0xd069b9b9, 0x91178686, 0x5899c1c1, 0x273a1d1d, 0xb9279e9e,

	0x38d9e1e1, 0x13ebf8f8, 0xb32b9898, 0x33221111, 0xbbd26969, 0x70a9d9d9, 0x89078e8e, 0xa7339494,

	0xb62d9b9b, 0x223c1e1e, 0x92158787, 0x20c9e9e9, 0x4987cece, 0xffaa5555, 0x78502828, 0x7aa5dfdf,

	0x8f038c8c, 0xf859a1a1, 0x80098989, 0x171a0d0d, 0xda65bfbf, 0x31d7e6e6, 0xc6844242, 0xb8d06868,

	0xc3824141, 0xb0299999, 0x775a2d2d, 0x111e0f0f, 0xcb7bb0b0, 0xfca85454, 0xd66dbbbb, 0x3a2c1616,

};

static const uint32_t te2[256]  __attribute__((aligned(64))) = {

	0x63a5c663, 0x7c84f87c, 0x7799ee77, 0x7b8df67b, 0xf20dfff2, 0x6bbdd66b, 0x6fb1de6f, 0xc55491c5,

	0x30506030, 0x01030201, 0x67a9ce67, 0x2b7d562b, 0xfe19e7fe, 0xd762b5d7, 0xabe64dab, 0x769aec76,

	0xca458fca, 0x829d1f82, 0xc94089c9, 0x7d87fa7d, 0xfa15effa, 0x59ebb259, 0x47c98e47, 0xf00bfbf0,

	0xadec41ad, 0xd467b3d4, 0xa2fd5fa2, 0xafea45af, 0x9cbf239c, 0xa4f753a4, 0x7296e472, 0xc05b9bc0,

	0xb7c275b7, 0xfd1ce1fd, 0x93ae3d93, 0x266a4c26, 0x365a6c36, 0x3f417e3f, 0xf702f5f7, 0xcc4f83cc,

	0x345c6834, 0xa5f451a5, 0xe534d1e5, 0xf108f9f1, 0x7193e271, 0xd873abd8, 0x31536231, 0x153f2a15,

	0x040c0804, 0xc75295c7, 0x23654623, 0xc35e9dc3, 0x18283018, 0x96a13796, 0x050f0a05, 0x9ab52f9a,

	0x07090e07, 0x12362412, 0x809b1b80, 0xe23ddfe2, 0xeb26cdeb, 0x27694e27, 0xb2cd7fb2, 0x759fea75,

	0x091b1209, 0x839e1d83, 0x2c74582c, 0x1a2e341a, 0x1b2d361b, 0x6eb2dc6e, 0x5aeeb45a, 0xa0fb5ba0,

	0x52f6a452, 0x3b4d763b, 0xd661b7d6, 0xb3ce7db3, 0x297b5229, 0xe33edde3, 0x2f715e2f, 0x84971384,

	0x53f5a653, 0xd168b9d1, 0x00000000, 0xed2cc1ed, 0x20604020, 0xfc1fe3fc, 0xb1c879b1, 0x5bedb65b,

	0x6abed46a, 0xcb468dcb, 0xbed967be, 0x394b7239, 0x4ade944a, 0x4cd4984c, 0x58e8b058, 0xcf4a85cf,

	0xd06bbbd0, 0xef2ac5ef, 0xaae54faa, 0xfb16edfb, 0x43c58643, 0x4dd79a4d, 0x33556633, 0x85941185,

	0x45cf8a45, 0xf910e9f9, 0x02060402, 0x7f81fe7f, 0x50f0a050, 0x3c44783c, 0x9fba259f, 0xa8e34ba8,

	0x51f3a251, 0xa3fe5da3, 0x40c08040, 0x8f8a058f, 0x92ad3f92, 0x9dbc219d, 0x38487038, 0xf504f1f5,

	0xbcdf63bc, 0xb6c177b6, 0xda75afda, 0x21634221, 0x10302010, 0xff1ae5ff, 0xf30efdf3, 0xd26dbfd2,

	0xcd4c81cd, 0x0c14180c, 0x13352613, 0xec2fc3ec, 0x5fe1be5f, 0x97a23597, 0x44cc8844, 0x17392e17,

	0xc45793c4, 0xa7f255a7, 0x7e82fc7e, 0x3d477a3d, 0x64acc864, 0x5de7ba5d, 0x192b3219, 0x7395e673,

	0x60a0c060, 0x81981981, 0x4fd19e4f, 0xdc7fa3dc, 0x22664422, 0x2a7e542a, 0x90ab3b90, 0x88830b88,

	0x46ca8c46, 0xee29c7ee, 0xb8d36bb8, 0x143c2814, 0xde79a7de, 0x5ee2bc5e, 0x0b1d160b, 0xdb76addb,

	0xe03bdbe0, 0x32566432, 0x3a4e743a, 0x0a1e140a, 0x49db9249, 0x060a0c06, 0x246c4824, 0x5ce4b85c,

	0xc25d9fc2, 0xd36ebdd3, 0xacef43ac, 0x62a6c462, 0x91a83991, 0x95a43195, 0xe437d3e4, 0x798bf279,

	0xe732d5e7, 0xc8438bc8, 0x37596e37, 0x6db7da6d, 0x8d8c018d, 0xd564b1d5, 0x4ed29c4e, 0xa9e049a9,

	0x6cb4d86c, 0x56faac56, 0xf407f3f4, 0xea25cfea, 0x65afca65, 0x7a8ef47a, 0xaee947ae, 0x08181008,

	0xbad56fba, 0x7888f078, 0x256f4a25, 0x2e725c2e, 0x1c24381c, 0xa6f157a6, 0xb4c773b4, 0xc65197c6,

	0xe823cbe8, 0xdd7ca1dd, 0x749ce874, 0x1f213e1f, 0x4bdd964b, 0xbddc61bd, 0x8b860d8b, 0x8a850f8a,

	0x7090e070, 0x3e427c3e, 0xb5c471b5, 0x66aacc66, 0x48d89048, 0x03050603, 0xf601f7f6, 0x0e121c0e,

	0x61a3c261, 0x355f6a35, 0x57f9ae57, 0xb9d069b9, 0x86911786, 0xc15899c1, 0x1d273a1d, 0x9eb9279e,

	0xe138d9e1, 0xf813ebf8, 0x98b32b98, 0x11332211, 0x69bbd269, 0xd970a9d9, 0x8e89078e, 0x94a73394,

	0x9bb62d9b, 0x1e223c1e, 0x87921587, 0xe920c9e9, 0xce4987ce, 0x55ffaa55, 0x28785028, 0xdf7aa5df,

	0x8c8f038c, 0xa1f859a1, 0x89800989, 0x0d171a0d, 0xbfda65bf, 0xe631d7e6, 0x42c68442, 0x68b8d068,

	0x41c38241, 0x99b02999, 0x2d775a2d, 0x0f111e0f, 0xb0cb7bb0, 0x54fca854, 0xbbd66dbb, 0x163a2c16,

};

static const uint32_t te3[256]  __attribute__((aligned(64))) = {

	0x6363a5c6, 0x7c7c84f8, 0x777799ee, 0x7b7b8df6, 0xf2f20dff, 0x6b6bbdd6, 0x6f6fb1de, 0xc5c55491,

	0x30305060, 0x01010302, 0x6767a9ce, 0x2b2b7d56, 0xfefe19e7, 0xd7d762b5, 0xababe64d, 0x76769aec,

	0xcaca458f, 0x82829d1f, 0xc9c94089, 0x7d7d87fa, 0xfafa15ef, 0x5959ebb2, 0x4747c98e, 0xf0f00bfb,

	0xadadec41, 0xd4d467b3, 0xa2a2fd5f, 0xafafea45, 0x9c9cbf23, 0xa4a4f753, 0x727296e4, 0xc0c05b9b,

	0xb7b7c275, 0xfdfd1ce1, 0x9393ae3d, 0x26266a4c, 0x36365a6c, 0x3f3f417e, 0xf7f702f5, 0xcccc4f83,

	0x34345c68, 0xa5a5f451, 0xe5e534d1, 0xf1f108f9, 0x717193e2, 0xd8d873ab, 0x31315362, 0x15153f2a,

	0x04040c08, 0xc7c75295, 0x23236546, 0xc3c35e9d, 0x18182830, 0x9696a137, 0x05050f0a, 0x9a9ab52f,

	0x0707090e, 0x12123624, 0x80809b1b, 0xe2e23ddf, 0xebeb26cd, 0x2727694e, 0xb2b2cd7f, 0x75759fea,

	0x09091b12, 0x83839e1d, 0x2c2c7458, 0x1a1a2e34, 0x1b1b2d36, 0x6e6eb2dc, 0x5a5aeeb4, 0xa0a0fb5b,

	0x5252f6a4, 0x3b3b4d76, 0xd6d661b7, 0xb3b3ce7d, 0x29297b52, 0xe3e33edd, 0x2f2f715e, 0x84849713,

	0x5353f5a6, 0xd1d168b9, 0x00000000, 0xeded2cc1, 0x20206040, 0xfcfc1fe3, 0xb1b1c879, 0x5b5bedb6,

	0x6a6abed4, 0xcbcb468d, 0xbebed967, 0x39394b72, 0x4a4ade94, 0x4c4cd498, 0x5858e8b0, 0xcfcf4a85,

	0xd0d06bbb, 0xefef2ac5, 0xaaaae54f, 0xfbfb16ed, 0x4343c586, 0x4d4dd79a, 0x33335566, 0x85859411,

	0x4545cf8a, 0xf9f910e9, 0x02020604, 0x7f7f81fe, 0x5050f0a0, 0x3c3c4478, 0x9f9fba25, 0xa8a8e34b,

	0x5151f3a2, 0xa3a3fe5d, 0x4040c080, 0x8f8f8a05, 0x9292ad3f, 0x9d9dbc21, 0x38384870, 0xf5f504f1,

	0xbcbcdf63, 0xb6b6c177, 0xdada75af, 0x21216342, 0x10103020, 0xffff1ae5, 0xf3f30efd, 0xd2d26dbf,

	0xcdcd4c81, 0x0c0c1418, 0x13133526, 0xecec2fc3, 0x5f5fe1be, 0x9797a235, 0x4444cc88, 0x1717392e,

	0xc4c45793, 0xa7a7f255, 0x7e7e82fc, 0x3d3d477a, 0x6464acc8, 0x5d5de7ba, 0x19192b32, 0x737395e6,

	0x6060a0c0, 0x81819819, 0x4f4fd19e, 0xdcdc7fa3, 0x22226644, 0x2a2a7e54, 0x9090ab3b, 0x8888830b,

	0x4646ca8c, 0xeeee29c7, 0xb8b8d36b, 0x14143c28, 0xdede79a7, 0x5e5ee2bc, 0x0b0b1d16, 0xdbdb76ad,

	0xe0e03bdb, 0x32325664, 0x3a3a4e74, 0x0a0a1e14, 0x4949db92, 0x06060a0c, 0x24246c48, 0x5c5ce4b8,

	0xc2c25d9f, 0xd3d36ebd, 0xacacef43, 0x6262a6c4, 0x9191a839, 0x9595a431, 0xe4e437d3, 0x79798bf2,

	0xe7e732d5, 0xc8c8438b, 0x3737596e, 0x6d6db7da, 0x8d8d8c01, 0xd5d564b1, 0x4e4ed29c, 0xa9a9e049,

	0x6c6cb4d8, 0x5656faac, 0xf4f407f3, 0xeaea25cf, 0x6565afca, 0x7a7a8ef4, 0xaeaee947, 0x08081810,

	0xbabad56f, 0x787888f0, 0x25256f4a, 0x2e2e725c, 0x1c1c2438, 0xa6a6f157, 0xb4b4c773, 0xc6c65197,

	0xe8e823cb, 0xdddd7ca1, 0x74749ce8, 0x1f1f213e, 0x4b4bdd96, 0xbdbddc61, 0x8b8b860d, 0x8a8a850f,

	0x707090e0, 0x3e3e427c, 0xb5b5c471, 0x6666aacc, 0x4848d890, 0x03030506, 0xf6f601f7, 0x0e0e121c,

	0x6161a3c2, 0x35355f6a, 0x5757f9ae, 0xb9b9d069, 0x86869117, 0xc1c15899, 0x1d1d273a, 0x9e9eb927,

	0xe1e138d9, 0xf8f813eb, 0x9898b32b, 0x11113322, 0x6969bbd2, 0xd9d970a9, 0x8e8e8907, 0x9494a733,

	0x9b9bb62d, 0x1e1e223c, 0x87879215, 0xe9e920c9, 0xcece4987, 0x5555ffaa, 0x28287850, 0xdfdf7aa5,

	0x8c8c8f03, 0xa1a1f859, 0x89898009, 0x0d0d171a, 0xbfbfda65, 0xe6e631d7, 0x4242c684, 0x6868b8d0,

	0x4141c382, 0x9999b029, 0x2d2d775a, 0x0f0f111e, 0xb0b0cb7b, 0x5454fca8, 0xbbbbd66d, 0x16163a2c,

};

static const uint32_t rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000, /* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};

static const uint32_t Te4_0[] = {
	0x00000063UL, 0x0000007cUL, 0x00000077UL, 0x0000007bUL, 0x000000f2UL, 0x0000006bUL, 0x0000006fUL, 0x000000c5UL,
	0x00000030UL, 0x00000001UL, 0x00000067UL, 0x0000002bUL, 0x000000feUL, 0x000000d7UL, 0x000000abUL, 0x00000076UL,
	0x000000caUL, 0x00000082UL, 0x000000c9UL, 0x0000007dUL, 0x000000faUL, 0x00000059UL, 0x00000047UL, 0x000000f0UL,
	0x000000adUL, 0x000000d4UL, 0x000000a2UL, 0x000000afUL, 0x0000009cUL, 0x000000a4UL, 0x00000072UL, 0x000000c0UL,
	0x000000b7UL, 0x000000fdUL, 0x00000093UL, 0x00000026UL, 0x00000036UL, 0x0000003fUL, 0x000000f7UL, 0x000000ccUL,
	0x00000034UL, 0x000000a5UL, 0x000000e5UL, 0x000000f1UL, 0x00000071UL, 0x000000d8UL, 0x00000031UL, 0x00000015UL,
	0x00000004UL, 0x000000c7UL, 0x00000023UL, 0x000000c3UL, 0x00000018UL, 0x00000096UL, 0x00000005UL, 0x0000009aUL,
	0x00000007UL, 0x00000012UL, 0x00000080UL, 0x000000e2UL, 0x000000ebUL, 0x00000027UL, 0x000000b2UL, 0x00000075UL,
	0x00000009UL, 0x00000083UL, 0x0000002cUL, 0x0000001aUL, 0x0000001bUL, 0x0000006eUL, 0x0000005aUL, 0x000000a0UL,
	0x00000052UL, 0x0000003bUL, 0x000000d6UL, 0x000000b3UL, 0x00000029UL, 0x000000e3UL, 0x0000002fUL, 0x00000084UL,
	0x00000053UL, 0x000000d1UL, 0x00000000UL, 0x000000edUL, 0x00000020UL, 0x000000fcUL, 0x000000b1UL, 0x0000005bUL,
	0x0000006aUL, 0x000000cbUL, 0x000000beUL, 0x00000039UL, 0x0000004aUL, 0x0000004cUL, 0x00000058UL, 0x000000cfUL,
	0x000000d0UL, 0x000000efUL, 0x000000aaUL, 0x000000fbUL, 0x00000043UL, 0x0000004dUL, 0x00000033UL, 0x00000085UL,
	0x00000045UL, 0x000000f9UL, 0x00000002UL, 0x0000007fUL, 0x00000050UL, 0x0000003cUL, 0x0000009fUL, 0x000000a8UL,
	0x00000051UL, 0x000000a3UL, 0x00000040UL, 0x0000008fUL, 0x00000092UL, 0x0000009dUL, 0x00000038UL, 0x000000f5UL,
	0x000000bcUL, 0x000000b6UL, 0x000000daUL, 0x00000021UL, 0x00000010UL, 0x000000ffUL, 0x000000f3UL, 0x000000d2UL,
	0x000000cdUL, 0x0000000cUL, 0x00000013UL, 0x000000ecUL, 0x0000005fUL, 0x00000097UL, 0x00000044UL, 0x00000017UL,
	0x000000c4UL, 0x000000a7UL, 0x0000007eUL, 0x0000003dUL, 0x00000064UL, 0x0000005dUL, 0x00000019UL, 0x00000073UL,
	0x00000060UL, 0x00000081UL, 0x0000004fUL, 0x000000dcUL, 0x00000022UL, 0x0000002aUL, 0x00000090UL, 0x00000088UL,
	0x00000046UL, 0x000000eeUL, 0x000000b8UL, 0x00000014UL, 0x000000deUL, 0x0000005eUL, 0x0000000bUL, 0x000000dbUL,
	0x000000e0UL, 0x00000032UL, 0x0000003aUL, 0x0000000aUL, 0x00000049UL, 0x00000006UL, 0x00000024UL, 0x0000005cUL,
	0x000000c2UL, 0x000000d3UL, 0x000000acUL, 0x00000062UL, 0x00000091UL, 0x00000095UL, 0x000000e4UL, 0x00000079UL,
	0x000000e7UL, 0x000000c8UL, 0x00000037UL, 0x0000006dUL, 0x0000008dUL, 0x000000d5UL, 0x0000004eUL, 0x000000a9UL,
	0x0000006cUL, 0x00000056UL, 0x000000f4UL, 0x000000eaUL, 0x00000065UL, 0x0000007aUL, 0x000000aeUL, 0x00000008UL,
	0x000000baUL, 0x00000078UL, 0x00000025UL, 0x0000002eUL, 0x0000001cUL, 0x000000a6UL, 0x000000b4UL, 0x000000c6UL,
	0x000000e8UL, 0x000000ddUL, 0x00000074UL, 0x0000001fUL, 0x0000004bUL, 0x000000bdUL, 0x0000008bUL, 0x0000008aUL,
	0x00000070UL, 0x0000003eUL, 0x000000b5UL, 0x00000066UL, 0x00000048UL, 0x00000003UL, 0x000000f6UL, 0x0000000eUL,
	0x00000061UL, 0x00000035UL, 0x00000057UL, 0x000000b9UL, 0x00000086UL, 0x000000c1UL, 0x0000001dUL, 0x0000009eUL,
	0x000000e1UL, 0x000000f8UL, 0x00000098UL, 0x00000011UL, 0x00000069UL, 0x000000d9UL, 0x0000008eUL, 0x00000094UL,
	0x0000009bUL, 0x0000001eUL, 0x00000087UL, 0x000000e9UL, 0x000000ceUL, 0x00000055UL, 0x00000028UL, 0x000000dfUL,
	0x0000008cUL, 0x000000a1UL, 0x00000089UL, 0x0000000dUL, 0x000000bfUL, 0x000000e6UL, 0x00000042UL, 0x00000068UL,
	0x00000041UL, 0x00000099UL, 0x0000002dUL, 0x0000000fUL, 0x000000b0UL, 0x00000054UL, 0x000000bbUL, 0x00000016UL
};

static const uint32_t Te4_1[] = {
	0x00006300UL, 0x00007c00UL, 0x00007700UL, 0x00007b00UL, 0x0000f200UL, 0x00006b00UL, 0x00006f00UL, 0x0000c500UL,
	0x00003000UL, 0x00000100UL, 0x00006700UL, 0x00002b00UL, 0x0000fe00UL, 0x0000d700UL, 0x0000ab00UL, 0x00007600UL,
	0x0000ca00UL, 0x00008200UL, 0x0000c900UL, 0x00007d00UL, 0x0000fa00UL, 0x00005900UL, 0x00004700UL, 0x0000f000UL,
	0x0000ad00UL, 0x0000d400UL, 0x0000a200UL, 0x0000af00UL, 0x00009c00UL, 0x0000a400UL, 0x00007200UL, 0x0000c000UL,
	0x0000b700UL, 0x0000fd00UL, 0x00009300UL, 0x00002600UL, 0x00003600UL, 0x00003f00UL, 0x0000f700UL, 0x0000cc00UL,
	0x00003400UL, 0x0000a500UL, 0x0000e500UL, 0x0000f100UL, 0x00007100UL, 0x0000d800UL, 0x00003100UL, 0x00001500UL,
	0x00000400UL, 0x0000c700UL, 0x00002300UL, 0x0000c300UL, 0x00001800UL, 0x00009600UL, 0x00000500UL, 0x00009a00UL,
	0x00000700UL, 0x00001200UL, 0x00008000UL, 0x0000e200UL, 0x0000eb00UL, 0x00002700UL, 0x0000b200UL, 0x00007500UL,
	0x00000900UL, 0x00008300UL, 0x00002c00UL, 0x00001a00UL, 0x00001b00UL, 0x00006e00UL, 0x00005a00UL, 0x0000a000UL,
	0x00005200UL, 0x00003b00UL, 0x0000d600UL, 0x0000b300UL, 0x00002900UL, 0x0000e300UL, 0x00002f00UL, 0x00008400UL,
	0x00005300UL, 0x0000d100UL, 0x00000000UL, 0x0000ed00UL, 0x00002000UL, 0x0000fc00UL, 0x0000b100UL, 0x00005b00UL,
	0x00006a00UL, 0x0000cb00UL, 0x0000be00UL, 0x00003900UL, 0x00004a00UL, 0x00004c00UL, 0x00005800UL, 0x0000cf00UL,
	0x0000d000UL, 0x0000ef00UL, 0x0000aa00UL, 0x0000fb00UL, 0x00004300UL, 0x00004d00UL, 0x00003300UL, 0x00008500UL,
	0x00004500UL, 0x0000f900UL, 0x00000200UL, 0x00007f00UL, 0x00005000UL, 0x00003c00UL, 0x00009f00UL, 0x0000a800UL,
	0x00005100UL, 0x0000a300UL, 0x00004000UL, 0x00008f00UL, 0x00009200UL, 0x00009d00UL, 0x00003800UL, 0x0000f500UL,
	0x0000bc00UL, 0x0000b600UL, 0x0000da00UL, 0x00002100UL, 0x00001000UL, 0x0000ff00UL, 0x0000f300UL, 0x0000d200UL,
	0x0000cd00UL, 0x00000c00UL, 0x00001300UL, 0x0000ec00UL, 0x00005f00UL, 0x00009700UL, 0x00004400UL, 0x00001700UL,
	0x0000c400UL, 0x0000a700UL, 0x00007e00UL, 0x00003d00UL, 0x00006400UL, 0x00005d00UL, 0x00001900UL, 0x00007300UL,
	0x00006000UL, 0x00008100UL, 0x00004f00UL, 0x0000dc00UL, 0x00002200UL, 0x00002a00UL, 0x00009000UL, 0x00008800UL,
	0x00004600UL, 0x0000ee00UL, 0x0000b800UL, 0x00001400UL, 0x0000de00UL, 0x00005e00UL, 0x00000b00UL, 0x0000db00UL,
	0x0000e000UL, 0x00003200UL, 0x00003a00UL, 0x00000a00UL, 0x00004900UL, 0x00000600UL, 0x00002400UL, 0x00005c00UL,
	0x0000c200UL, 0x0000d300UL, 0x0000ac00UL, 0x00006200UL, 0x00009100UL, 0x00009500UL, 0x0000e400UL, 0x00007900UL,
	0x0000e700UL, 0x0000c800UL, 0x00003700UL, 0x00006d00UL, 0x00008d00UL, 0x0000d500UL, 0x00004e00UL, 0x0000a900UL,
	0x00006c00UL, 0x00005600UL, 0x0000f400UL, 0x0000ea00UL, 0x00006500UL, 0x00007a00UL, 0x0000ae00UL, 0x00000800UL,
	0x0000ba00UL, 0x00007800UL, 0x00002500UL, 0x00002e00UL, 0x00001c00UL, 0x0000a600UL, 0x0000b400UL, 0x0000c600UL,
	0x0000e800UL, 0x0000dd00UL, 0x00007400UL, 0x00001f00UL, 0x00004b00UL, 0x0000bd00UL, 0x00008b00UL, 0x00008a00UL,
	0x00007000UL, 0x00003e00UL, 0x0000b500UL, 0x00006600UL, 0x00004800UL, 0x00000300UL, 0x0000f600UL, 0x00000e00UL,
	0x00006100UL, 0x00003500UL, 0x00005700UL, 0x0000b900UL, 0x00008600UL, 0x0000c100UL, 0x00001d00UL, 0x00009e00UL,
	0x0000e100UL, 0x0000f800UL, 0x00009800UL, 0x00001100UL, 0x00006900UL, 0x0000d900UL, 0x00008e00UL, 0x00009400UL,
	0x00009b00UL, 0x00001e00UL, 0x00008700UL, 0x0000e900UL, 0x0000ce00UL, 0x00005500UL, 0x00002800UL, 0x0000df00UL,
	0x00008c00UL, 0x0000a100UL, 0x00008900UL, 0x00000d00UL, 0x0000bf00UL, 0x0000e600UL, 0x00004200UL, 0x00006800UL,
	0x00004100UL, 0x00009900UL, 0x00002d00UL, 0x00000f00UL, 0x0000b000UL, 0x00005400UL, 0x0000bb00UL, 0x00001600UL
};

static const uint32_t Te4_2[] = {
	0x00630000UL, 0x007c0000UL, 0x00770000UL, 0x007b0000UL, 0x00f20000UL, 0x006b0000UL, 0x006f0000UL, 0x00c50000UL,
	0x00300000UL, 0x00010000UL, 0x00670000UL, 0x002b0000UL, 0x00fe0000UL, 0x00d70000UL, 0x00ab0000UL, 0x00760000UL,
	0x00ca0000UL, 0x00820000UL, 0x00c90000UL, 0x007d0000UL, 0x00fa0000UL, 0x00590000UL, 0x00470000UL, 0x00f00000UL,
	0x00ad0000UL, 0x00d40000UL, 0x00a20000UL, 0x00af0000UL, 0x009c0000UL, 0x00a40000UL, 0x00720000UL, 0x00c00000UL,
	0x00b70000UL, 0x00fd0000UL, 0x00930000UL, 0x00260000UL, 0x00360000UL, 0x003f0000UL, 0x00f70000UL, 0x00cc0000UL,
	0x00340000UL, 0x00a50000UL, 0x00e50000UL, 0x00f10000UL, 0x00710000UL, 0x00d80000UL, 0x00310000UL, 0x00150000UL,
	0x00040000UL, 0x00c70000UL, 0x00230000UL, 0x00c30000UL, 0x00180000UL, 0x00960000UL, 0x00050000UL, 0x009a0000UL,
	0x00070000UL, 0x00120000UL, 0x00800000UL, 0x00e20000UL, 0x00eb0000UL, 0x00270000UL, 0x00b20000UL, 0x00750000UL,
	0x00090000UL, 0x00830000UL, 0x002c0000UL, 0x001a0000UL, 0x001b0000UL, 0x006e0000UL, 0x005a0000UL, 0x00a00000UL,
	0x00520000UL, 0x003b0000UL, 0x00d60000UL, 0x00b30000UL, 0x00290000UL, 0x00e30000UL, 0x002f0000UL, 0x00840000UL,
	0x00530000UL, 0x00d10000UL, 0x00000000UL, 0x00ed0000UL, 0x00200000UL, 0x00fc0000UL, 0x00b10000UL, 0x005b0000UL,
	0x006a0000UL, 0x00cb0000UL, 0x00be0000UL, 0x00390000UL, 0x004a0000UL, 0x004c0000UL, 0x00580000UL, 0x00cf0000UL,
	0x00d00000UL, 0x00ef0000UL, 0x00aa0000UL, 0x00fb0000UL, 0x00430000UL, 0x004d0000UL, 0x00330000UL, 0x00850000UL,
	0x00450000UL, 0x00f90000UL, 0x00020000UL, 0x007f0000UL, 0x00500000UL, 0x003c0000UL, 0x009f0000UL, 0x00a80000UL,
	0x00510000UL, 0x00a30000UL, 0x00400000UL, 0x008f0000UL, 0x00920000UL, 0x009d0000UL, 0x00380000UL, 0x00f50000UL,
	0x00bc0000UL, 0x00b60000UL, 0x00da0000UL, 0x00210000UL, 0x00100000UL, 0x00ff0000UL, 0x00f30000UL, 0x00d20000UL,
	0x00cd0000UL, 0x000c0000UL, 0x00130000UL, 0x00ec0000UL, 0x005f0000UL, 0x00970000UL, 0x00440000UL, 0x00170000UL,
	0x00c40000UL, 0x00a70000UL, 0x007e0000UL, 0x003d0000UL, 0x00640000UL, 0x005d0000UL, 0x00190000UL, 0x00730000UL,
	0x00600000UL, 0x00810000UL, 0x004f0000UL, 0x00dc0000UL, 0x00220000UL, 0x002a0000UL, 0x00900000UL, 0x00880000UL,
	0x00460000UL, 0x00ee0000UL, 0x00b80000UL, 0x00140000UL, 0x00de0000UL, 0x005e0000UL, 0x000b0000UL, 0x00db0000UL,
	0x00e00000UL, 0x00320000UL, 0x003a0000UL, 0x000a0000UL, 0x00490000UL, 0x00060000UL, 0x00240000UL, 0x005c0000UL,
	0x00c20000UL, 0x00d30000UL, 0x00ac0000UL, 0x00620000UL, 0x00910000UL, 0x00950000UL, 0x00e40000UL, 0x00790000UL,
	0x00e70000UL, 0x00c80000UL, 0x00370000UL, 0x006d0000UL, 0x008d0000UL, 0x00d50000UL, 0x004e0000UL, 0x00a90000UL,
	0x006c0000UL, 0x00560000UL, 0x00f40000UL, 0x00ea0000UL, 0x00650000UL, 0x007a0000UL, 0x00ae0000UL, 0x00080000UL,
	0x00ba0000UL, 0x00780000UL, 0x00250000UL, 0x002e0000UL, 0x001c0000UL, 0x00a60000UL, 0x00b40000UL, 0x00c60000UL,
	0x00e80000UL, 0x00dd0000UL, 0x00740000UL, 0x001f0000UL, 0x004b0000UL, 0x00bd0000UL, 0x008b0000UL, 0x008a0000UL,
	0x00700000UL, 0x003e0000UL, 0x00b50000UL, 0x00660000UL, 0x00480000UL, 0x00030000UL, 0x00f60000UL, 0x000e0000UL,
	0x00610000UL, 0x00350000UL, 0x00570000UL, 0x00b90000UL, 0x00860000UL, 0x00c10000UL, 0x001d0000UL, 0x009e0000UL,
	0x00e10000UL, 0x00f80000UL, 0x00980000UL, 0x00110000UL, 0x00690000UL, 0x00d90000UL, 0x008e0000UL, 0x00940000UL,
	0x009b0000UL, 0x001e0000UL, 0x00870000UL, 0x00e90000UL, 0x00ce0000UL, 0x00550000UL, 0x00280000UL, 0x00df0000UL,
	0x008c0000UL, 0x00a10000UL, 0x00890000UL, 0x000d0000UL, 0x00bf0000UL, 0x00e60000UL, 0x00420000UL, 0x00680000UL,
	0x00410000UL, 0x00990000UL, 0x002d0000UL, 0x000f0000UL, 0x00b00000UL, 0x00540000UL, 0x00bb0000UL, 0x00160000UL
};

static const uint32_t Te4_3[] = {
	0x63000000UL, 0x7c000000UL, 0x77000000UL, 0x7b000000UL, 0xf2000000UL, 0x6b000000UL, 0x6f000000UL, 0xc5000000UL,
	0x30000000UL, 0x01000000UL, 0x67000000UL, 0x2b000000UL, 0xfe000000UL, 0xd7000000UL, 0xab000000UL, 0x76000000UL,
	0xca000000UL, 0x82000000UL, 0xc9000000UL, 0x7d000000UL, 0xfa000000UL, 0x59000000UL, 0x47000000UL, 0xf0000000UL,
	0xad000000UL, 0xd4000000UL, 0xa2000000UL, 0xaf000000UL, 0x9c000000UL, 0xa4000000UL, 0x72000000UL, 0xc0000000UL,
	0xb7000000UL, 0xfd000000UL, 0x93000000UL, 0x26000000UL, 0x36000000UL, 0x3f000000UL, 0xf7000000UL, 0xcc000000UL,
	0x34000000UL, 0xa5000000UL, 0xe5000000UL, 0xf1000000UL, 0x71000000UL, 0xd8000000UL, 0x31000000UL, 0x15000000UL,
	0x04000000UL, 0xc7000000UL, 0x23000000UL, 0xc3000000UL, 0x18000000UL, 0x96000000UL, 0x05000000UL, 0x9a000000UL,
	0x07000000UL, 0x12000000UL, 0x80000000UL, 0xe2000000UL, 0xeb000000UL, 0x27000000UL, 0xb2000000UL, 0x75000000UL,
	0x09000000UL, 0x83000000UL, 0x2c000000UL, 0x1a000000UL, 0x1b000000UL, 0x6e000000UL, 0x5a000000UL, 0xa0000000UL,
	0x52000000UL, 0x3b000000UL, 0xd6000000UL, 0xb3000000UL, 0x29000000UL, 0xe3000000UL, 0x2f000000UL, 0x84000000UL,
	0x53000000UL, 0xd1000000UL, 0x00000000UL, 0xed000000UL, 0x20000000UL, 0xfc000000UL, 0xb1000000UL, 0x5b000000UL,
	0x6a000000UL, 0xcb000000UL, 0xbe000000UL, 0x39000000UL, 0x4a000000UL, 0x4c000000UL, 0x58000000UL, 0xcf000000UL,
	0xd0000000UL, 0xef000000UL, 0xaa000000UL, 0xfb000000UL, 0x43000000UL, 0x4d000000UL, 0x33000000UL, 0x85000000UL,
	0x45000000UL, 0xf9000000UL, 0x02000000UL, 0x7f000000UL, 0x50000000UL, 0x3c000000UL, 0x9f000000UL, 0xa8000000UL,
	0x51000000UL, 0xa3000000UL, 0x40000000UL, 0x8f000000UL, 0x92000000UL, 0x9d000000UL, 0x38000000UL, 0xf5000000UL,
	0xbc000000UL, 0xb6000000UL, 0xda000000UL, 0x21000000UL, 0x10000000UL, 0xff000000UL, 0xf3000000UL, 0xd2000000UL,
	0xcd000000UL, 0x0c000000UL, 0x13000000UL, 0xec000000UL, 0x5f000000UL, 0x97000000UL, 0x44000000UL, 0x17000000UL,
	0xc4000000UL, 0xa7000000UL, 0x7e000000UL, 0x3d000000UL, 0x64000000UL, 0x5d000000UL, 0x19000000UL, 0x73000000UL,
	0x60000000UL, 0x81000000UL, 0x4f000000UL, 0xdc000000UL, 0x22000000UL, 0x2a000000UL, 0x90000000UL, 0x88000000UL,
	0x46000000UL, 0xee000000UL, 0xb8000000UL, 0x14000000UL, 0xde000000UL, 0x5e000000UL, 0x0b000000UL, 0xdb000000UL,
	0xe0000000UL, 0x32000000UL, 0x3a000000UL, 0x0a000000UL, 0x49000000UL, 0x06000000UL, 0x24000000UL, 0x5c000000UL,
	0xc2000000UL, 0xd3000000UL, 0xac000000UL, 0x62000000UL, 0x91000000UL, 0x95000000UL, 0xe4000000UL, 0x79000000UL,
	0xe7000000UL, 0xc8000000UL, 0x37000000UL, 0x6d000000UL, 0x8d000000UL, 0xd5000000UL, 0x4e000000UL, 0xa9000000UL,
	0x6c000000UL, 0x56000000UL, 0xf4000000UL, 0xea000000UL, 0x65000000UL, 0x7a000000UL, 0xae000000UL, 0x08000000UL,
	0xba000000UL, 0x78000000UL, 0x25000000UL, 0x2e000000UL, 0x1c000000UL, 0xa6000000UL, 0xb4000000UL, 0xc6000000UL,
	0xe8000000UL, 0xdd000000UL, 0x74000000UL, 0x1f000000UL, 0x4b000000UL, 0xbd000000UL, 0x8b000000UL, 0x8a000000UL,
	0x70000000UL, 0x3e000000UL, 0xb5000000UL, 0x66000000UL, 0x48000000UL, 0x03000000UL, 0xf6000000UL, 0x0e000000UL,
	0x61000000UL, 0x35000000UL, 0x57000000UL, 0xb9000000UL, 0x86000000UL, 0xc1000000UL, 0x1d000000UL, 0x9e000000UL,
	0xe1000000UL, 0xf8000000UL, 0x98000000UL, 0x11000000UL, 0x69000000UL, 0xd9000000UL, 0x8e000000UL, 0x94000000UL,
	0x9b000000UL, 0x1e000000UL, 0x87000000UL, 0xe9000000UL, 0xce000000UL, 0x55000000UL, 0x28000000UL, 0xdf000000UL,
	0x8c000000UL, 0xa1000000UL, 0x89000000UL, 0x0d000000UL, 0xbf000000UL, 0xe6000000UL, 0x42000000UL, 0x68000000UL,
	0x41000000UL, 0x99000000UL, 0x2d000000UL, 0x0f000000UL, 0xb0000000UL, 0x54000000UL, 0xbb000000UL, 0x16000000UL
};
