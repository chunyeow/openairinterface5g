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

/*--------- Operator Variant Algorithm Configuration Field --------*/
/*------- Insert your value of OP here -------*/
u8 OP[16]= {
	    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
	};

/*--------------------------- prototypes --------------------------*/
void ComputeOPc( u8 op_c[16] );

void SetOPc(u8 op_c[16])
{
    memcpy(OP, op_c, 16);
}

void generate_autn(u8 sqn[6], u8 ak[6], u8 amf[2], u8 mac_a[8], u8 autn[16])
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
void f1 ( u8 k[16], u8 _rand[16], u8 sqn[6], u8 amf[2],
          u8 mac_a[8] )
{
    u8 op_c[16];
    u8 temp[16];
    u8 in1[16];
    u8 out1[16];
    u8 rijndaelInput[16];
    u8 i;
    RijndaelKeySchedule( k );
    ComputeOPc( op_c );
    for (i=0; i<16; i++)
        rijndaelInput[i] = _rand[i] ^ op_c[i];
    RijndaelEncrypt( rijndaelInput, temp );
    for (i=0; i<6; i++)
    {
        in1[i] = sqn[i];
        in1[i+8] = sqn[i];
    }
    for (i=0; i<2; i++)
    {
        in1[i+6] = amf[i];
        in1[i+14] = amf[i];
    }
    /* XOR op_c and in1, rotate by r1=64, and XOR *
     * on the constant c1 (which is all zeroes) */
    for (i=0; i<16; i++)
        rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];
    /* XOR on the value temp computed before */
    for (i=0; i<16; i++)
        rijndaelInput[i] ^= temp[i];
    RijndaelEncrypt( rijndaelInput, out1 );
    for (i=0; i<16; i++)
        out1[i] ^= op_c[i];
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
void f2345 ( u8 k[16], u8 _rand[16],
             u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] )
{
    u8 op_c[16];
    u8 temp[16];
    u8 out[16];
    u8 rijndaelInput[16];
    u8 i;
    RijndaelKeySchedule( k );
    ComputeOPc( op_c );
    for (i=0; i<16; i++)
        rijndaelInput[i] = _rand[i] ^ op_c[i];
    RijndaelEncrypt( rijndaelInput, temp );
    /* To obtain output block OUT2: XOR OPc and TEMP, *
     * rotate by r2=0, and XOR on the constant c2 (which *
     * is all zeroes except that the last bit is 1). */
    for (i=0; i<16; i++)
        rijndaelInput[i] = temp[i] ^ op_c[i];
    rijndaelInput[15] ^= 1;
    RijndaelEncrypt( rijndaelInput, out );
    for (i=0; i<16; i++)
        out[i] ^= op_c[i];
    for (i=0; i<8; i++)
        res[i] = out[i+8];
    for (i=0; i<6; i++)
        ak[i] = out[i];
    /* To obtain output block OUT3: XOR OPc and TEMP, *
     * rotate by r3=32, and XOR on the constant c3 (which *
     * is all zeroes except that the next to last bit is 1). */

    for (i=0; i<16; i++)
        rijndaelInput[(i+12) % 16] = temp[i] ^ op_c[i];
    rijndaelInput[15] ^= 2;
    RijndaelEncrypt( rijndaelInput, out );
    for (i=0; i<16; i++)
        out[i] ^= op_c[i];
    for (i=0; i<16; i++)
        ck[i] = out[i];
    /* To obtain output block OUT4: XOR OPc and TEMP, *
     * rotate by r4=64, and XOR on the constant c4 (which *
     * is all zeroes except that the 2nd from last bit is 1). */
    for (i=0; i<16; i++)
        rijndaelInput[(i+8) % 16] = temp[i] ^ op_c[i];
    rijndaelInput[15] ^= 4;
    RijndaelEncrypt( rijndaelInput, out );
    for (i=0; i<16; i++)
        out[i] ^= op_c[i];
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
void f1star( u8 k[16], u8 _rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_s[8] )
{
    u8 op_c[16];
    u8 temp[16];
    u8 in1[16];
    u8 out1[16];
    u8 rijndaelInput[16];
    u8 i;
    RijndaelKeySchedule( k );
    ComputeOPc( op_c );
    for (i=0; i<16; i++)
        rijndaelInput[i] = _rand[i] ^ op_c[i];
    RijndaelEncrypt( rijndaelInput, temp );
    for (i=0; i<6; i++)
    {
        in1[i] = sqn[i];
        in1[i+8] = sqn[i];
    }
    for (i=0; i<2; i++)
    {
        in1[i+6] = amf[i];
        in1[i+14] = amf[i];
    }
    /* XOR op_c and in1, rotate by r1=64, and XOR *
     * on the constant c1 (which is all zeroes) */
    for (i=0; i<16; i++)
        rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];
    /* XOR on the value temp computed before */
    for (i=0; i<16; i++)
        rijndaelInput[i] ^= temp[i];
    RijndaelEncrypt( rijndaelInput, out1 );
    for (i=0; i<16; i++)
        out1[i] ^= op_c[i];
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
void f5star( u8 k[16], u8 _rand[16],
             u8 ak[6] )
{
    u8 op_c[16];
    u8 temp[16];
    u8 out[16];
    u8 rijndaelInput[16];
    u8 i;
    RijndaelKeySchedule( k );
    ComputeOPc( op_c );
    for (i=0; i<16; i++)
        rijndaelInput[i] = _rand[i] ^ op_c[i];
    RijndaelEncrypt( rijndaelInput, temp );
    /* To obtain output block OUT5: XOR OPc and TEMP, *
     * rotate by r5=96, and XOR on the constant c5 (which *
     * is all zeroes except that the 3rd from last bit is 1). */
    for (i=0; i<16; i++)
        rijndaelInput[(i+4) % 16] = temp[i] ^ op_c[i];
    rijndaelInput[15] ^= 8;
    RijndaelEncrypt( rijndaelInput, out );
    for (i=0; i<16; i++)
        out[i] ^= op_c[i];
    for (i=0; i<6; i++)
        ak[i] = out[i];
    return;
} /* end of function f5star */

/*-------------------------------------------------------------------
 * Function to compute OPc from OP and K. Assumes key schedule has
 * already been performed.
 *-----------------------------------------------------------------*/
void ComputeOPc( u8 op_c[16] )
{
    u8 i;
    printf("ComputeOPc: OP : %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X ",
    		OP[0],OP[1],OP[2],OP[3],OP[4],OP[5],OP[6],OP[7],
    		OP[8],OP[9],OP[10],OP[11],OP[12],OP[13],OP[14],OP[15]);

    RijndaelEncrypt( OP, op_c );
    for (i=0; i<16; i++)
        op_c[i] ^= OP[i];
    return;
} /* end of function ComputeOPc */
