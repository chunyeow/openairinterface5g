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
/*------- Insert your value of OP here -------*/
extern uint8_t opc[16];
extern uint8_t op[16];

/*--------------------------- prototypes --------------------------*/
void ComputeOPc( u8 opP[16] );

void SetOP(char *opP)
{
    int ret = sscanf(opP,
                 "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                 (unsigned int*)&op[0],(unsigned int*)&op[1],
                 (unsigned int*)&op[2],(unsigned int*)&op[3],
                 (unsigned int*)&op[4],(unsigned int*)&op[5],
                 (unsigned int*)&op[6],(unsigned int*)&op[7],
                 (unsigned int*)&op[8],(unsigned int*)&op[9],
                 (unsigned int*)&op[10],(unsigned int*)&op[11],
                 (unsigned int*)&op[12],(unsigned int*)&op[13],
                 (unsigned int*)&op[14],(unsigned int*)&op[15]);
    if (ret != 16) {
      fprintf(stderr,
              "Error in operator key\n");
      abort();
    }
    printf("SetOP: OP : %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
		  op[0],op[1],op[2],op[3],op[4],op[5],op[6],op[7],
		  op[8],op[9],op[10],op[11],op[12],op[13],op[14],op[15]);
}

void generate_autn(const u8 const sqn[6], const u8 const ak[6], const u8 const amf[2], const u8 const mac_a[8], u8 autn[16])
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
void f1 ( const u8 const k[16], const u8 const _rand[16], const u8 const sqn[6], const u8 const amf[2],
          u8 mac_a[8] )
{
  u8 temp[16];
  u8 in1[16];
  u8 out1[16];
  u8 rijndaelInput[16];
  u8 i;
  RijndaelKeySchedule( k );
  if (hss_config.valid_opc == 0) {
	SetOP(hss_config.operator_key);
    ComputeOPc( opc );
  }

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
void f2345 ( const u8 const k[16], const u8 const _rand[16],
             u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] )
{
  u8 temp[16];
  u8 out[16];
  u8 rijndaelInput[16];
  u8 i;
  RijndaelKeySchedule( k );
  if (hss_config.valid_opc == 0) {
    SetOP(hss_config.operator_key);
    ComputeOPc( opc );
  }

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
void f1star( const u8 const k[16], const u8 const _rand[16], const u8 const sqn[6], const u8 const amf[2],
             u8 mac_s[8] )
{
  u8 temp[16];
  u8 in1[16];
  u8 out1[16];
  u8 rijndaelInput[16];
  u8 i;
  RijndaelKeySchedule( k );
  if (hss_config.valid_opc == 0) {
	SetOP(hss_config.operator_key);
    ComputeOPc( opc );
  } else {
	  printf("Using opc:  %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
		         opc[0],opc[1],opc[2],opc[3],opc[4],opc[5],opc[6],opc[7],
		         opc[8],opc[9],opc[10],opc[11],opc[12],opc[13],opc[14],opc[15] );
  }

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
void f5star( const u8 const k[16], const u8 const _rand[16],
             u8 ak[6] )
{
  u8 temp[16];
  u8 out[16];
  u8 rijndaelInput[16];
  u8 i;

  RijndaelKeySchedule( k );
  if (hss_config.valid_opc == 0) {
	SetOP(hss_config.operator_key);
    ComputeOPc(opc);
  } else {
	  printf("Using OPc: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
			  opc[0],opc[1],opc[2],opc[3],opc[4],opc[5],opc[6],opc[7],
			  opc[8],opc[9],opc[10],opc[11],opc[12],opc[13],opc[14],opc[15]);
  }

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
 * Function to compute OPc from OP and K. Assumes key schedule has
 * already been performed.
 *-----------------------------------------------------------------*/
void ComputeOPc( u8 opcP[16] )
{
  u8 i;

  RijndaelEncrypt( op, opcP );
  printf("Compute opc:\n\tIn:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n\tRinj:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
		  op[0],op[1],op[2],op[3],op[4],op[5],op[6],op[7],
		  op[8],op[9],op[10],op[11],op[12],op[13],op[14],op[15],
	      opcP[0],opcP[1],opcP[2],opcP[3],opcP[4],opcP[5],opcP[6],opcP[7],
	      opcP[8],opcP[9],opcP[10],opcP[11],opcP[12],opcP[13],opcP[14],opcP[15] );

  for (i=0; i<16; i++)
	  opcP[i] ^= op[i];
  printf("\tOut:\t%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
	         opcP[0],opcP[1],opcP[2],opcP[3],opcP[4],opcP[5],opcP[6],opcP[7],
	         opcP[8],opcP[9],opcP[10],opcP[11],opcP[12],opcP[13],opcP[14],opcP[15] );

  return;
} /* end of function ComputeOPc */
