/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>

#include "auc.h"

typedef uint8_t  u8;
typedef uint32_t u32;

/*-------------------- Rijndael round subkeys ---------------------*/
u8 roundKeys[11][4][4];
/*--------------------- Rijndael S box table ----------------------*/
u8 S[256] = {
    99,124,119,123,242,107,111,197, 48, 1,103, 43,254,215,171,118,
    202,130,201,125,250, 89, 71,240,173,212,162,175,156,164,114,192,
    183,253,147, 38, 54, 63,247,204, 52,165,229,241,113,216, 49, 21,
    4,199, 35,195, 24,150, 5,154, 7, 18,128,226,235, 39,178,117,
    9,131, 44, 26, 27,110, 90,160, 82, 59,214,179, 41,227, 47,132,
    83,209, 0,237, 32,252,177, 91,106,203,190, 57, 74, 76, 88,207,
    208,239,170,251, 67, 77, 51,133, 69,249, 2,127, 80, 60,159,168,
    81,163, 64,143,146,157, 56,245,188,182,218, 33, 16,255,243,210,
    205, 12, 19,236, 95,151, 68, 23,196,167,126, 61,100, 93, 25,115,
    96,129, 79,220, 34, 42,144,136, 70,238,184, 20,222, 94, 11,219,
    224, 50, 58, 10, 73, 6, 36, 92,194,211,172, 98,145,149,228,121,
    231,200, 55,109,141,213, 78,169,108, 86,244,234,101,122,174, 8,
    186,120, 37, 46, 28,166,180,198,232,221,116, 31, 75,189,139,138,
    112, 62,181,102, 72, 3,246, 14, 97, 53, 87,185,134,193, 29,158,
    225,248,152, 17,105,217,142,148,155, 30,135,233,206, 85, 40,223,
    140,161,137, 13,191,230, 66,104, 65,153, 45, 15,176, 84,187, 22,
};
/*------- This array does the multiplication by x in GF(2^8) ------*/
u8 Xtime[256] = {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
    32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62,
    64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94,
    96, 98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,
    128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,
    160,162,164,166,168,170,172,174,176,178,180,182,184,186,188,190,
    192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,
    224,226,228,230,232,234,236,238,240,242,244,246,248,250,252,254,
    27, 25, 31, 29, 19, 17, 23, 21, 11, 9, 15, 13, 3, 1, 7, 5,
    59, 57, 63, 61, 51, 49, 55, 53, 43, 41, 47, 45, 35, 33, 39, 37,
    91, 89, 95, 93, 83, 81, 87, 85, 75, 73, 79, 77, 67, 65, 71, 69,
    123,121,127,125,115,113,119,117,107,105,111,109, 99, 97,103,101,
    155,153,159,157,147,145,151,149,139,137,143,141,131,129,135,133,
    187,185,191,189,179,177,183,181,171,169,175,173,163,161,167,165,
    219,217,223,221,211,209,215,213,203,201,207,205,195,193,199,197,
    251,249,255,253,243,241,247,245,235,233,239,237,227,225,231,229
};
/*-------------------------------------------------------------------
 * Rijndael key schedule function. Takes 16-byte key and creates
 * all Rijndael's internal subkeys ready for encryption.
 *-----------------------------------------------------------------*/
void RijndaelKeySchedule( u8 key[16] )
{
    u8 roundConst;
    int i, j;
    /* first round key equals key */
    for (i=0; i<16; i++)
        roundKeys[0][i & 0x03][i>>2] = key[i];
    roundConst = 1;
    /* now calculate round keys */
    for (i=1; i<11; i++)
    {
        roundKeys[i][0][0] = S[roundKeys[i-1][1][3]]
        ^ roundKeys[i-1][0][0] ^ roundConst;
        roundKeys[i][1][0] = S[roundKeys[i-1][2][3]]
        ^ roundKeys[i-1][1][0];
        roundKeys[i][2][0] = S[roundKeys[i-1][3][3]]
        ^ roundKeys[i-1][2][0];
        roundKeys[i][3][0] = S[roundKeys[i-1][0][3]]
        ^ roundKeys[i-1][3][0];
        for (j=0; j<4; j++)
        {
            roundKeys[i][j][1] = roundKeys[i-1][j][1] ^ roundKeys[i][j][0];
            roundKeys[i][j][2] = roundKeys[i-1][j][2] ^ roundKeys[i][j][1];
            roundKeys[i][j][3] = roundKeys[i-1][j][3] ^ roundKeys[i][j][2];
        }
        /* update round constant */
        roundConst = Xtime[roundConst];
    }
    return;
} /* end of function RijndaelKeySchedule */
/* Round key addition function */
void KeyAdd(u8 state[4][4], u8 roundKeys[11][4][4], int round)
{
    int i, j;
    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
            state[i][j] ^= roundKeys[round][i][j];
        return;
}
/* Byte substitution transformation */
int ByteSub(u8 state[4][4])
{
    int i, j;
    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
            state[i][j] = S[state[i][j]];
        return 0;
}
/* Row shift transformation */
void ShiftRow(u8 state[4][4])
{
    u8 temp;
    /* left rotate row 1 by 1 */
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;
    /* left rotate row 2 by 2 */
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    /* left rotate row 3 by 3 */
    temp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = temp;
    return;
}
/* MixColumn transformation*/
void MixColumn(u8 state[4][4])
{
    u8 temp, tmp, tmp0;
    int i;
    /* do one column at a time */
    for (i=0; i<4;i++)
    {
        temp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i];
        tmp0 = state[0][i];
        /* Xtime array does multiply by x in GF2^8 */
        tmp = Xtime[state[0][i] ^ state[1][i]];
        state[0][i] ^= temp ^ tmp;
        tmp = Xtime[state[1][i] ^ state[2][i]];
        state[1][i] ^= temp ^ tmp;
        tmp = Xtime[state[2][i] ^ state[3][i]];
        state[2][i] ^= temp ^ tmp;
        tmp = Xtime[state[3][i] ^ tmp0];
        state[3][i] ^= temp ^ tmp;
    }
    return;
}
/*-------------------------------------------------------------------
 * Rijndael encryption function. Takes 16-byte input and creates
 * 16-byte output (using round keys already derived from 16-byte
 * key).
 *-----------------------------------------------------------------*/
void RijndaelEncrypt( u8 input[16], u8 output[16] )
{
    u8 state[4][4];
    int i, r;
    /* initialise state array from input byte string */
    for (i=0; i<16; i++)
        state[i & 0x3][i>>2] = input[i];
    /* add first round_key */
    KeyAdd(state, roundKeys, 0);
    /* do lots of full rounds */
    for (r=1; r<=9; r++)
    {
        ByteSub(state);
        ShiftRow(state);
        MixColumn(state);
        KeyAdd(state, roundKeys, r);
    }
    /* final round */
    ByteSub(state);
    ShiftRow(state);
    KeyAdd(state, roundKeys, r);
    /* produce output byte string from state array */
    for (i=0; i<16; i++)
    {
        output[i] = state[i & 0x3][i>>2];
    }
    return;
} /* end of function RijndaelEncrypt */
