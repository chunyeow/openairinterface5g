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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#define MIH_C_INTERFACE
#define MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
#include "MIH_C_F1_basic_data_types_codec.h"

//-----------------------------------------------------------------------------
unsigned int MIH_C_BITMAP82String(MIH_C_BITMAP8_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%02X", *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP8_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP) {
    printf("[MIH_C] %s: %02X\n", __FUNCTION__, *dataP);
    BitBuffer_write8(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP16_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP) {
    printf("[MIH_C] %s: %04X\n", __FUNCTION__, *dataP);
    BitBuffer_write16(bbP, *dataP);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_BITMAP162String(MIH_C_BITMAP16_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%04X", *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP24_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP) {
//-----------------------------------------------------------------------------
    printf("[MIH_C] %s: %02X%02X%02X\n", __FUNCTION__, dataP->val[0],dataP->val[1], dataP->val[2]);
    MIH_C_BITMAP8_encode(bbP, &dataP->val[0]);
    MIH_C_BITMAP8_encode(bbP, &dataP->val[1]);
    MIH_C_BITMAP8_encode(bbP, &dataP->val[2]);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_BITMAP242String(MIH_C_BITMAP24_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%02X%02X%02X", dataP->val[0], dataP->val[1], dataP->val[2]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP32_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP) {
    printf("[MIH_C] %s: %08X\n", __FUNCTION__, *dataP);
    BitBuffer_write32(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP64_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP) {
//-----------------------------------------------------------------------------
    printf("[MIH_C] %s: %16X\n", __FUNCTION__, *dataP);
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)(*dataP>>32));
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)*dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP128_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP) {
//-----------------------------------------------------------------------------
    printf("[MIH_C] %s: %16X%16X\n", __FUNCTION__, dataP->val[0], dataP->val[1]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[0]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[1]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP256_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP) {
//-----------------------------------------------------------------------------
    printf("[MIH_C] %s: %16X%16X%16X%16X\n", __FUNCTION__, dataP->val[0], dataP->val[1], dataP->val[2], dataP->val[3]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[0]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[1]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[2]);
    MIH_C_BITMAP64_encode(bbP, &dataP->val[3]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_CHOICE_encode(Bit_Buffer_t* bbP, MIH_C_CHOICE_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write8(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER1_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write8(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER2_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write16(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER4_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write32(bbP, *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER8_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)(*dataP>>32));
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)*dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_NULL_encode(Bit_Buffer_t* bbP, MIH_C_NULL_T* dataP) {;}
//-----------------------------------------------------------------------------
inline void MIH_C_OCTET_encode(Bit_Buffer_t* bbP, MIH_C_F1_Generic_Octet_t *dataP, int lengthP) {
//-----------------------------------------------------------------------------
    BitBuffer_writeMem(bbP, dataP->val, lengthP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT1_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP) {
    printf("[MIH_C] %s: %02X\n", __FUNCTION__, *dataP);
    BitBuffer_write8(bbP, *dataP);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_UNSIGNED_INT12String(MIH_C_UNSIGNED_INT1_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%02X", *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT2_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write16(bbP, *dataP);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_UNSIGNED_INT22String(MIH_C_UNSIGNED_INT2_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%04X", *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT4_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write32(bbP, *dataP);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_UNSIGNED_INT42String(MIH_C_UNSIGNED_INT4_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    return sprintf(bufP, "0x%08X", *dataP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT8_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP) {
//-----------------------------------------------------------------------------
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)(*dataP>>32));
    BitBuffer_write32(bbP, (MIH_C_UNSIGNED_INT4_T)*dataP);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_UNSIGNED_INT82String(MIH_C_UNSIGNED_INT8_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index = sprintf(bufP, "0x%08x", (MIH_C_UNSIGNED_INT4_T)(*dataP>>32));
    buffer_index = sprintf(&bufP[buffer_index], "%08x", (MIH_C_UNSIGNED_INT4_T)(*dataP));
    return buffer_index;\
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP8_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP) {*dataP = BitBuffer_read(bbP, 8);}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP16_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP) {*dataP = BitBuffer_read(bbP, 16);}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP24_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP) {
//-----------------------------------------------------------------------------
    MIH_C_BITMAP8_decode(bbP, &dataP->val[0]);
    MIH_C_BITMAP8_decode(bbP, &dataP->val[1]);
    MIH_C_BITMAP8_decode(bbP, &dataP->val[2]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP32_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP) {*dataP = BitBuffer_read(bbP, 32);}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP64_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP) {
//-----------------------------------------------------------------------------
    *dataP = BitBuffer_read(bbP, 32);
    *dataP = *dataP << 32;
    *dataP = *dataP | BitBuffer_read(bbP, 32);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP128_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP) {
//-----------------------------------------------------------------------------
    MIH_C_BITMAP64_decode(bbP, &dataP->val[0]);
    MIH_C_BITMAP64_decode(bbP, &dataP->val[1]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_BITMAP256_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP) {
//-----------------------------------------------------------------------------
    MIH_C_BITMAP64_decode(bbP, &dataP->val[0]);
    MIH_C_BITMAP64_decode(bbP, &dataP->val[1]);
    MIH_C_BITMAP64_decode(bbP, &dataP->val[2]);
    MIH_C_BITMAP64_decode(bbP, &dataP->val[3]);
}
//-----------------------------------------------------------------------------
inline void MIH_C_CHOICE_decode(Bit_Buffer_t* bbP, MIH_C_CHOICE_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_CHOICE_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER1_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_INTEGER1_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER2_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_INTEGER2_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER4_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_INTEGER4_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_INTEGER8_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP) {
//-----------------------------------------------------------------------------
    *dataP = BitBuffer_read(bbP, sizeof(MIH_C_INTEGER4_T)*8);
    *dataP = *dataP << (sizeof(MIH_C_INTEGER4_T)*8);
    *dataP = *dataP | BitBuffer_read(bbP, sizeof(MIH_C_INTEGER4_T)*8);
}
//-----------------------------------------------------------------------------
inline void MIH_C_NULL_decode(Bit_Buffer_t* bbP) {;}
//-----------------------------------------------------------------------------
inline void MIH_C_OCTET_decode(Bit_Buffer_t* bbP, MIH_C_F1_Generic_Octet_t *dataP, int lengthP) {
//-----------------------------------------------------------------------------
    BitBuffer_readMem(bbP, dataP->val, lengthP);
}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT1_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT1_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT2_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT2_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT4_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP) {*dataP = BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT4_T)*8);}
//-----------------------------------------------------------------------------
inline void MIH_C_UNSIGNED_INT8_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP) {
//-----------------------------------------------------------------------------
    *dataP = BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT4_T)*8);
    *dataP = *dataP << 32;
    *dataP = *dataP | BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT4_T)*8);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LIST_LENGTH_encode(Bit_Buffer_t* bbP, u_int16_t lengthP) {
//-----------------------------------------------------------------------------
    unsigned int num_more_bytes;
    int          length;
    if (lengthP <= 128) {
        #ifdef DEBUG_TRACE_DETAILS
        printf("[MIH_C] %s: Length %02X\n", __FUNCTION__, lengthP);
        #endif
        BitBuffer_write8(bbP, (MIH_C_UNSIGNED_INT1_T)lengthP);
    } else {
        num_more_bytes = (lengthP - 128) / 255;
        if (((lengthP - 128) % 255) != 0) {
            num_more_bytes += 1;
        }
        printf("[MIH_C] %s: Length %02X on %d more bytes\n", __FUNCTION__, lengthP, num_more_bytes);
        BitBuffer_write8(bbP, (MIH_C_UNSIGNED_INT1_T)(128 + num_more_bytes));
        length = lengthP - 128;
        while (length > 0) {
            if (length >= 255) {
                BitBuffer_write8(bbP, (MIH_C_UNSIGNED_INT1_T)(255));
                length = length - 255;
            } else {
                BitBuffer_write8(bbP, (MIH_C_UNSIGNED_INT1_T)(length));
                length = 0;
            }
        }
    }
}
//-----------------------------------------------------------------------------
inline u_int16_t MIH_C_LIST_LENGTH_get_encode_num_bytes(u_int16_t lengthP) {
//-----------------------------------------------------------------------------
    unsigned int num_more_bytes;
    if (lengthP <= 128) {
        return sizeof (MIH_C_UNSIGNED_INT1_T);
    } else {
        num_more_bytes = (lengthP - 128) / 255;
        if (((lengthP - 128) % 255) != 0) {
            num_more_bytes += 1;
        }
        printf("[MIH_C] %s: Length %02X on %d more bytes\n", __FUNCTION__, lengthP, num_more_bytes);
        return num_more_bytes + sizeof (MIH_C_UNSIGNED_INT1_T);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LIST_LENGTH_decode(Bit_Buffer_t* bbP, u_int16_t *lengthP) {
//-----------------------------------------------------------------------------
    unsigned int              num_more_bytes;
    MIH_C_UNSIGNED_INT1_T     byte1;

    byte1 = BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT1_T)*8);
    if (byte1 >= 128) {
        num_more_bytes = byte1 - 128;
        *lengthP = 128;
        while (num_more_bytes > 0) {
            *lengthP = *lengthP + BitBuffer_read(bbP, sizeof(MIH_C_UNSIGNED_INT1_T)*8);
            num_more_bytes = num_more_bytes - 1;
        }
        #ifdef DEBUG_TRACE_DETAILS
        printf("[MIH_C] %s: Length %d \n", __FUNCTION__, *lengthP);
        #endif
    } else {
        *lengthP = byte1;
        #ifdef DEBUG_TRACE_DETAILS
        printf("[MIH_C] %s: Length %d \n", __FUNCTION__, *lengthP);
        #endif
    }
}


