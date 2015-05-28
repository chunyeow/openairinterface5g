/*-------------------------------------------------------------------
 * Example algorithms f1, f1*, f2, f3, f4, f5, f5*
 *-------------------------------------------------------------------
 *
 * A sample implementation of the example 3GPP authentication and
 * key agreement functions f1, f1*, f2, f3, f4, f5 and f5*. This is
 * a byte-oriented implementation of the functions, and of the block
 * cipher kernel function Rijndael.
 *
 * This has been coded for clarity, not necessarily for efficiency.
 *
 * The functions f2, f3, f4 and f5 share the same inputs and have
 * been coded together as a single function. f1, f1* and f5* are
 * all coded separately.
 *
 *-----------------------------------------------------------------*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auc.h"
#include "hss_config.h"

extern hss_config_t hss_config;

/*--------- Operator Variant Algorithm Configuration Field --------*/


/*--------------------------- prototypes --------------------------*/


/*-------------------------------------------------------------------
 *
 *-------------------------------------------------------------------
 *-----------------------------------------------------------------*/
void generate_autn(const uint8_t const sqn[6], const uint8_t const ak[6], const uint8_t const amf[2], const uint8_t const mac_a[8], uint8_t autn[16])
{
  int i;

  for (i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }

  memcpy(&autn[6], amf, 2);
  memcpy(&autn[8], mac_a, 8);
}

/*-------------------------------------------------------------------
 * Algorithm f1
 *-------------------------------------------------------------------
 *
 * Computes network authentication code MAC-A from key K, random
 * challenge RAND, sequence number SQN and authentication management
 * field AMF.
 *
 *-----------------------------------------------------------------*/
void f1 ( const uint8_t const opc[16], const uint8_t const k[16], const uint8_t const _rand[16], const uint8_t const sqn[6], const uint8_t const amf[2],
          uint8_t mac_a[8] )
{
  uint8_t temp[16];
  uint8_t in1[16];
  uint8_t out1[16];
  uint8_t rijndaelInput[16];
  uint8_t i;
  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = _rand[i] ^ opc[i];

  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++) {
    in1[i] = sqn[i];
    in1[i+8] = sqn[i];
  }

  for (i=0; i<2; i++) {
    in1[i+6] = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes) */
  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ opc[i];

  /* XOR on the value temp computed before */
  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];

  RijndaelEncrypt( rijndaelInput, out1 );

  for (i=0; i<16; i++)
    out1[i] ^= opc[i];

  for (i=0; i<8; i++)
    mac_a[i] = out1[i];

  return;
} /* end of function f1 */

/*-------------------------------------------------------------------
 * Algorithms f2-f5
 *-------------------------------------------------------------------
 *
 * Takes key K and random challenge RAND, and returns response RES,
 * confidentiality key CK, integrity key IK and anonymity key AK.
 *
 *-----------------------------------------------------------------*/
void f2345 ( const uint8_t const opc[16], const uint8_t const k[16], const uint8_t const _rand[16],
             uint8_t res[8], uint8_t ck[16], uint8_t ik[16], uint8_t ak[6] )
{
  uint8_t temp[16];
  uint8_t out[16];
  uint8_t rijndaelInput[16];
  uint8_t i;
  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = _rand[i] ^ opc[i];

  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT2: XOR OPc and TEMP, *
   * rotate by r2=0, and XOR on the constant c2 (which *
   * is all zeroes except that the last bit is 1). */
  for (i=0; i<16; i++)
    rijndaelInput[i] = temp[i] ^ opc[i];

  rijndaelInput[15] ^= 1;
  RijndaelEncrypt( rijndaelInput, out );

  for (i=0; i<16; i++)
    out[i] ^= opc[i];

  for (i=0; i<8; i++)
    res[i] = out[i+8];

  for (i=0; i<6; i++)
    ak[i] = out[i];

  /* To obtain output block OUT3: XOR OPc and TEMP, *
   * rotate by r3=32, and XOR on the constant c3 (which *
   * is all zeroes except that the next to last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+12) % 16] = temp[i] ^ opc[i];

  rijndaelInput[15] ^= 2;
  RijndaelEncrypt( rijndaelInput, out );

  for (i=0; i<16; i++)
    out[i] ^= opc[i];

  for (i=0; i<16; i++)
    ck[i] = out[i];

  /* To obtain output block OUT4: XOR OPc and TEMP, *
   * rotate by r4=64, and XOR on the constant c4 (which *
   * is all zeroes except that the 2nd from last bit is 1). */
  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = temp[i] ^ opc[i];

  rijndaelInput[15] ^= 4;
  RijndaelEncrypt( rijndaelInput, out );

  for (i=0; i<16; i++)
    out[i] ^= opc[i];

  for (i=0; i<16; i++)
    ik[i] = out[i];

  return;
} /* end of function f2345 */

/*-------------------------------------------------------------------
 * Algorithm f1*
 *-------------------------------------------------------------------
 *
 * Computes resynch authentication code MAC-S from key K, random
 * challenge RAND, sequence number SQN and authentication management
 * field AMF.
 *
 *-----------------------------------------------------------------*/
void f1star( const uint8_t const opc[16], const uint8_t const k[16], const uint8_t const _rand[16], const uint8_t const sqn[6], const uint8_t const amf[2],
             uint8_t mac_s[8] )
{
  uint8_t temp[16];
  uint8_t in1[16];
  uint8_t out1[16];
  uint8_t rijndaelInput[16];
  uint8_t i;
  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = _rand[i] ^ opc[i];

  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++) {
    in1[i] = sqn[i];
    in1[i+8] = sqn[i];
  }

  for (i=0; i<2; i++) {
    in1[i+6] = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes) */
  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ opc[i];

  /* XOR on the value temp computed before */
  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];

  RijndaelEncrypt( rijndaelInput, out1 );

  for (i=0; i<16; i++)
    out1[i] ^= opc[i];

  for (i=0; i<8; i++)
    mac_s[i] = out1[i+8];

  return;
} /* end of function f1star */

/*-------------------------------------------------------------------
 * Algorithm f5*
 *-------------------------------------------------------------------
 *
 * Takes key K and random challenge RAND, and returns resynch
 * anonymity key AK.
 *
 *-----------------------------------------------------------------*/
void f5star( const uint8_t const opc[16], const uint8_t const k[16], const uint8_t const _rand[16],
             uint8_t ak[6] )
{
  uint8_t temp[16];
  uint8_t out[16];
  uint8_t rijndaelInput[16];
  uint8_t i;

  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = _rand[i] ^ opc[i];

  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT5: XOR OPc and TEMP, *
   * rotate by r5=96, and XOR on the constant c5 (which *
   * is all zeroes except that the 3rd from last bit is 1). */
  for (i=0; i<16; i++)
    rijndaelInput[(i+4) % 16] = temp[i] ^ opc[i];

  rijndaelInput[15] ^= 8;
  RijndaelEncrypt( rijndaelInput, out );

  for (i=0; i<16; i++)
    out[i] ^= opc[i];

  for (i=0; i<6; i++)
    ak[i] = out[i];

  return;
} /* end of function f5star */

/*-------------------------------------------------------------------
 * Function to compute OPc from OP and K.
 *-----------------------------------------------------------------*/
void ComputeOPc( const uint8_t const kP[16], const uint8_t const opP[16], uint8_t opcP[16] )
{
  uint8_t i;

  RijndaelKeySchedule( kP );
  printf("Compute opc:\n\tK:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
		  kP[0],kP[1],kP[2],kP[3],kP[4],kP[5],kP[6],kP[7],
		  kP[8],kP[9],kP[10],kP[11],kP[12],kP[13],kP[14],kP[15]);
  RijndaelEncrypt( opP, opcP );
  printf("\tIn:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n\tRinj:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
		  opP[0],opP[1],opP[2],opP[3],opP[4],opP[5],opP[6],opP[7],
		  opP[8],opP[9],opP[10],opP[11],opP[12],opP[13],opP[14],opP[15],
	      opcP[0],opcP[1],opcP[2],opcP[3],opcP[4],opcP[5],opcP[6],opcP[7],
	      opcP[8],opcP[9],opcP[10],opcP[11],opcP[12],opcP[13],opcP[14],opcP[15] );

  for (i=0; i<16; i++)
	  opcP[i] ^= opP[i];
  printf("\tOut:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
	         opcP[0],opcP[1],opcP[2],opcP[3],opcP[4],opcP[5],opcP[6],opcP[7],
	         opcP[8],opcP[9],opcP[10],opcP[11],opcP[12],opcP[13],opcP[14],opcP[15] );

  return;
} /* end of function ComputeOPc */
