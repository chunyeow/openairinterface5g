#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <nettle/nettle-meta.h>
#include <nettle/aes.h>
#include <nettle/ctr.h>

#include "assertions.h"
#include "conversions.h"
#include "secu_defs.h"
#include "snow3g.h"
#include "test_util.h"

// #define SECU_DEBUG
void small_test() {

	/* Test Set 1
	 * input : Key and IV
	 * output : z1 and z2
	*/
	printf("Set of tests for Snow3G\n");

	uint32_t k[4]; /*Key */
	uint32_t IV[4];
	uint32_t ks[2];
	uint32_t zf1 ; /*output 1*/
	uint32_t zf2 ; /*output 2*/
	int success = 0;
	int fail = 0;
    snow_3g_context_t snow_3g_context;

    memset(&snow_3g_context, 0, sizeof(snow_3g_context));
	printf("---------------------------------- TEST SET 1 ----------------------------------\n");

	printf ("key = 2B D6 45 9F 82 C5 B3 00 95 2C 49 10 48 81 FF 48 \n");
	printf ("IV = EA 02 47 14 AD 5C 4D 84 DF 1F 9B 25 1C 0B F4 5F \n");

	zf1 = 0xabee9704;
	zf2 = 0x7ac31373;

	k[0] = 0x2BD6459F ;
	k[1] = 0x82C5B300 ;
	k[2] = 0x952C4910 ;
	k[3] = 0x4881FF48 ;

	IV[0] = 0xEA024714 ;
	IV[1] = 0xAD5C4D84 ;
	IV[2] = 0xDF1F9B25 ;
	IV[3] = 0x1C0BF45F ;

	printf("Initialisation Mode \n");
    snow3g_initialize(k, IV, &snow_3g_context);

	printf("Keystream Mode \n");
    snow3g_generate_key_stream(2,(uint32_t*)ks, &snow_3g_context);
	if (ks[0] == zf1 && ks[1] == zf2){
	printf(" z1 = 0x%x\n",ks[0]);
	printf(" z2 = 0x%x\n",ks[1]);
	printf("TEST 1 OK ! \n");
	success = success +1 ;
	}
	else {
	printf("TEST 1 Failed ! \n");
	fail = fail + 1;
	}
	/*---------------------------TEST 2--------------------------------*/
    memset(&snow_3g_context, 0, sizeof(snow_3g_context));
	printf("---------------------------------- TEST SET 2 ----------------------------------\n");

	printf ("key = 8C E3 3E 2C C3 C0 B5 FC 1F 3D E8 A6 DC 66 B1 F3 \n");
	printf ("IV = D3 C5 D5 92 32 7F B1 1C DE 55 19 88 CE B2 F9 B7 \n");

	zf1 = 0xeff8a342;
	zf2 = 0xf751480f;

	k[0] = 0x8CE33E2C ;
	k[1] = 0xC3C0B5FC ;
	k[2] = 0x1F3DE8A6 ;
	k[3] = 0xDC66B1F3 ;

	IV[0] = 0xD3C5D592 ;
	IV[1] = 0x327FB11C ;
	IV[2] = 0xDE551988 ;
	IV[3] = 0xCEB2F9B7 ;

	printf("Initialisation Mode \n");
	snow3g_initialize(k, IV, &snow_3g_context);
	printf("Keystream Mode \n");
	snow3g_generate_key_stream(2,ks, &snow_3g_context);
	if (ks[0] == zf1 && ks[1] == zf2){
	printf(" z1 = 0x%x\n",ks[0]);
	printf(" z2 = 0x%x\n",ks[1]);
	printf("TEST 2 OK ! \n");
	success = success +1 ;
	}
	else {
	printf("TEST 2 Failed ! \n");
	fail = fail + 1;
	}
	/*---------------------------TEST 3--------------------------------*/

	printf("---------------------------------- TEST SET 3 ----------------------------------\n");

	printf ("key = 40 35 C6 68 0A F8 C6 D1 A8 FF 86 67 B1 71 40 13\n");
	printf ("IV = 62 A5 40 98 1B A6 F9 B7 45 92 B0 E7 86 90 F7 1B \n");

	zf1 = 0xa8c874a9;
	zf2 = 0x7ae7c4f8;

	k[0] = 0x4035C668 ;
	k[1] = 0x0AF8C6D1 ;
	k[2] = 0xA8FF8667 ;
	k[3] = 0xB1714013 ;

	IV[0] = 0x62A54098 ;
	IV[1] = 0x1BA6F9B7 ;
	IV[2] = 0x4592B0E7  ;
	IV[3] = 0x8690F71B ;

	printf("Initialisation Mode \n");
	snow3g_initialize(k, IV, &snow_3g_context);
	printf("Keystream Mode \n");
	snow3g_generate_key_stream(2,ks, &snow_3g_context);
	if (ks[0] == zf1 && ks[1] == zf2){
	printf(" z1 = 0x%x\n",ks[0]);
	printf(" z2 = 0x%x\n",ks[1]);
	printf("TEST 3 OK ! \n");
	success = success +1 ;
	}
	else {
	printf("TEST 3 Failed ! \n");
	fail = fail + 1;
	}

	/*---------------------------TEST 4--------------------------------*/

	printf("---------------------------------- TEST SET 4 ----------------------------------\n");

	printf ("key = 0D ED 72 63 10 9C F9 2E 33 52 25 5A 14 0E 0F 76\n");
	printf ("IV = 6B 68 07 9A 41 A7 C4 C9 1B EF D7 9F 7F DC C2 33 \n");

	zf1 = 0xd712c05c;
	zf2 = 0xa937c2a6;
	uint32_t zf3 = 0xeb7eaae3 ;
	uint32_t zf2500 = 0x9c0db3aa;
	uint32_t ks2[2500];

	k[0] = 0x0DED7263 ;
	k[1] = 0x109CF92E ;
	k[2] = 0x3352255A ;
	k[3] = 0x140E0F76 ;

	IV[0] = 0x6B68079A ;
	IV[1] = 0x41A7C4C9 ;
	IV[2] = 0x1BEFD79F  ;
	IV[3] = 0x7FDCC233 ;

	printf("Initialisation Mode \n");
	snow3g_initialize(k, IV, &snow_3g_context);
	printf("Keystream Mode \n");
	snow3g_generate_key_stream(2500,ks2, &snow_3g_context);
	if (ks2[0] == zf1 && ks2[1] == zf2 && ks2[2] == zf3 && ks2[2499] == zf2500){
	printf(" z1 = 0x%x\n",ks2[0]);
	printf(" z2 = 0x%x\n",ks2[1]);
	printf(" z3 = 0x%x\n",ks2[2]);
	printf(" ...\n");
	printf(" z2500 = 0x%x\n",ks2[2499]);
	printf("TEST 4 OK ! \n");
	success = success +1 ;
	}
	else {
	printf("TEST 4 Failed ! \n");
	fail = fail + 1;
	}

	printf("\n");
	printf("\n");
	printf("4 tests run, %d succeded, %d failed. \n",success,fail);

}

int nas_stream_encrypt_eea1(nas_stream_cipher_t *stream_cipher, uint8_t **out)
{
    snow_3g_context_t snow_3g_context;
	int       n ;
	int       i           = 0;
    uint32_t  zero_bit    = 0;
    uint32_t  byte_length;
    uint32_t *KS;
	uint32_t  K[4],IV[4];

    DevAssert(stream_cipher != NULL);
    DevAssert(stream_cipher->key != NULL);
    DevAssert(stream_cipher->key_length == 16);
    DevAssert(out != NULL);

    n = ( stream_cipher->blength + 31 ) / 32;
    zero_bit = stream_cipher->blength & 0x7;
    byte_length = stream_cipher->blength >> 3;

    memset(&snow_3g_context, 0, sizeof(snow_3g_context));
	/*Initialisation*/
	/* Load the confidentiality key for SNOW 3G initialization as in section
	3.4. */
	memcpy(K+3,stream_cipher->key+0,4); /*K[3] = key[0]; we assume
	K[3]=key[0]||key[1]||...||key[31] , with key[0] the
	* most important bit of key*/
	memcpy(K+2,stream_cipher->key+4,4); /*K[2] = key[1];*/
	memcpy(K+1,stream_cipher->key+8,4); /*K[1] = key[2];*/
	memcpy(K+0,stream_cipher->key+12,4); /*K[0] = key[3]; we assume
	K[0]=key[96]||key[97]||...||key[127] , with key[127] the
	* least important bit of key*/
	K[3] = hton_int32(K[3]);
	K[2] = hton_int32(K[2]);
	K[1] = hton_int32(K[1]);
	K[0] = hton_int32(K[0]);
	/* Prepare the initialization vector (IV) for SNOW 3G initialization as in
	section 3.4. */
	IV[3] = stream_cipher->count;
	IV[2] = ((((uint32_t)stream_cipher->bearer) << 3) | ((((uint32_t)stream_cipher->direction) & 0x1) << 2)) << 24;
	IV[1] = IV[3];
	IV[0] = IV[2];

    printf ("K:\n");
	hexprint(K, 16);
    printf ("K[0]:%08X\n",K[0]);
    printf ("K[1]:%08X\n",K[1]);
    printf ("K[2]:%08X\n",K[2]);
    printf ("K[3]:%08X\n",K[3]);

    printf ("IV:\n");
	hexprint(IV, 16);
    printf ("IV[0]:%08X\n",IV[0]);
    printf ("IV[1]:%08X\n",IV[1]);
    printf ("IV[2]:%08X\n",IV[2]);
    printf ("IV[3]:%08X\n",IV[3]);
	/* Run SNOW 3G algorithm to generate sequence of key stream bits KS*/
    snow3g_initialize(K, IV, &snow_3g_context);
	KS = (uint32_t *)malloc(4*n);
    snow3g_generate_key_stream(n,(uint32_t*)KS, &snow_3g_context);

    if (zero_bit > 0) {
		printf ("changing KS[%u]: %08X to %08X remove %d bits\n",
				n - 1,
				KS[n - 1],
				KS[n - 1] & (uint32_t)(0xFFFFFFFF << (8 - zero_bit)),
				8 - zero_bit);
    	KS[n - 1] = KS[n - 1] & (uint32_t)(0xFFFFFFFF << (8 - zero_bit));
    }
    for (i=0;i<n;i++) {
		KS[i] = hton_int32(KS[i]);
	}

    printf ("KS:\n");
	hexprint(KS, 4*n);
	for (i=0;i<n;i++) {
		printf ("KS[%u]:%08X\n",i,KS[i]);
	}
	/* Exclusive-OR the input data with keystream to generate the output bit
	stream */
	for (i=0;i<n*4;i++) {
		stream_cipher->message[i] ^= *(((uint8_t*)KS)+i);
	}
    if (zero_bit > 0) {
    	int ceil_index = (stream_cipher->blength+7) >> 3;
		printf ("changing stream_cipher->message[%u]: %08X to %08X remove %d bits\n",
				ceil_index - 1,
				stream_cipher->message[ceil_index - 1],
				stream_cipher->message[ceil_index - 1] & (uint8_t)(0xFF << (8 - zero_bit)),
				8 - zero_bit);
    	stream_cipher->message[ceil_index - 1] = stream_cipher->message[ceil_index - 1] & (uint8_t)(0xFF << (8 - zero_bit));
    }
    free(KS);
    *out = stream_cipher->message;

/*    uint32_t          local_count;
    uint32_t          zero_bit     = 0;
    uint32_t          m_length;

    snow_3g_context_t snow_3g_context;
    uint32_t  k[4];
    uint32_t  IV[4];
    uint32_t *z    = NULL;
    uint32_t *OBS  = NULL;
    uint64_t  L;
    uint8_t  *KS8  = NULL;
    int       i;

    DevAssert(stream_cipher != NULL);
    DevAssert(stream_cipher->key != NULL);
    DevAssert(stream_cipher->key_length == 16);
    DevAssert(out != NULL);

    memset(&snow_3g_context, 0, sizeof(snow_3g_context));

    zero_bit = stream_cipher->blength & 0x7;

    m_length = stream_cipher->blength >> 3;
    if (zero_bit > 0)
        m_length += 1;

    local_count = hton_int32(stream_cipher->count);

#warning "endianess"
    // SPEC says: K3 = CK[0]  || CK[1]  || CK[2]  || ... || CK[31]
    // SPEC says: K2 = CK[32] || CK[33] || CK[34] || ... || CK[63]
    // SPEC says: K1 = CK[64] || CK[65] || CK[66] || ... || CK[95]
    // SPEC says: K0 = CK[96] || CK[97] || CK[98] || ... || CK[127]

    k[3] = ((uint32_t*)&stream_cipher->key[0])[0]; k[3] = hton_int32(k[3]);
    k[2] = ((uint32_t*)&stream_cipher->key[4])[0]; k[2] = hton_int32(k[2]);
    k[1] = ((uint32_t*)&stream_cipher->key[8])[0]; k[1] = hton_int32(k[1]);
    k[0] = ((uint32_t*)&stream_cipher->key[12])[0];k[0] = hton_int32(k[0]);

    // SPEC says: IV3 = COUNT-C[0] || COUNT-C[1] || COUNT-C[2] || ... || COUNT-C[31]
    // SPEC says: IV2 = BEARER[0] || BEARER[1] || ... || BEARER[4] || DIRECTION[0] || 0 || ... || 0
    // SPEC says: IV1 = COUNT-C[0] || COUNT-C[1] || COUNT-C[2] || ... || COUNT-C[31]
    // SPEC says: IV0 = BEARER[0] || BEARER[1] || ... || BEARER[4] || DIRECTION[0] || 0 || ... || 0

    IV[3] = stream_cipher->count;// local_count; // NOT SURE endianess
    IV[2] = (((uint32_t)(stream_cipher->bearer & 0x1F)) << 27) | (((uint32_t)(stream_cipher->direction & 0x01)) << 26);
    IV[3] = stream_cipher->count;// local_count; // NOT SURE endianess
    IV[0] = (((uint32_t)(stream_cipher->bearer & 0x1F)) << 27) | (((uint32_t)(stream_cipher->direction & 0x01)) << 26);

    // SPEC says: SNOW 3G is initialised as described in document [5].

    snow3g_initialize(k, IV, &snow_3g_context);

    // 3.5. Keystream Generation
    //
    //  Set L = LENGTH / 32.
    //
    //  SNOW 3G is run as described in document [5] to produce the keystream consisting of the 32-
    //  bit words z1 ... zL. The word produced first is z1, the next word z2 and so on.
    //
    //  The sequence of keystream bits is KS[0] ... KS[LENGTH-1], where KS[0] is the most
    //  significant bit and KS[31] is the least significant bit of z1, KS[32] is the most significant bit of
    //  z2 and so on.
    L   = (stream_cipher->blength + 31)/32;
    z   = malloc(L*sizeof(*z));
    OBS = z;
    DevAssert(z != NULL);
    snow3g_generate_key_stream(L, z, &snow_3g_context);
    KS8 = (uint8_t*)z;
    // 3.6.Encryption/Decryption
    //
    // Encryption/decryption operations are identical operations and are performed by the exclusive-
    // OR of the input data (IBS) with the generated keystream (KS).
    //
    // For each integer i with 0 ≤ i ≤ LENGTH-1 we define:
    //
    //     OBS[i] = IBS[i] ⊕ KS[i].
    for (i = 0; i < m_length; i++) {
    	OBS[i] = OBS[i] ^ KS8[i];
    }
    *out = OBS;
    */
    return 0;
}
