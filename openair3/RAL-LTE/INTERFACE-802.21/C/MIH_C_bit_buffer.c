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
#define MRALLTEINT_BIT_BUFFER_C
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
//-----------------------------------------------------------------------------
#include "MIH_C_bit_buffer.h"
//-----------------------------------------------------------------------------
Bit_Buffer_t* new_BitBuffer_0(void)
//-----------------------------------------------------------------------------
{
  Bit_Buffer_t* bb               = malloc(sizeof(Bit_Buffer_t));
  bb->m_buffer_allocated_by_this = BIT_BUFFER_FALSE;
  bb->m_buffer                   = 0;
  bb->m_byte_position            = 0;
  bb->m_bit_mod_8_position       = 0;
  bb->m_capacity                 = 0;
  bb->m_limit                    = 0;
  return bb;
}
//-----------------------------------------------------------------------------
Bit_Buffer_t* new_BitBuffer_1(unsigned int capacityP)
//-----------------------------------------------------------------------------
{
  Bit_Buffer_t* bb = malloc(sizeof(Bit_Buffer_t));

  bb->m_buffer_allocated_by_this = BIT_BUFFER_TRUE;
  bb->m_buffer                   = malloc(capacityP);
  memset(bb->m_buffer,  0, capacityP);
  bb->m_byte_position            = 0;
  bb->m_bit_mod_8_position       = 0;
  bb->m_capacity                 = capacityP;
  bb->m_limit                    = 0;
  return bb;
}

//-----------------------------------------------------------------------------
Bit_Buffer_t* new_BitBuffer_2(unsigned char* bufferP, unsigned int capacityP)
//-----------------------------------------------------------------------------
{
  Bit_Buffer_t* bb               = malloc(sizeof(Bit_Buffer_t));
  bb->m_buffer_allocated_by_this = BIT_BUFFER_FALSE;
  bb->m_buffer                   = bufferP;
  bb->m_byte_position            = 0;
  bb->m_bit_mod_8_position       = 0;
  bb->m_limit                    = 0;
  bb->m_capacity                 = capacityP;
  return bb;
}

//-----------------------------------------------------------------------------
void BitBuffer_wrap(Bit_Buffer_t* bbP, unsigned char* bufferP, unsigned int capacityP)
//-----------------------------------------------------------------------------
{
  if ((bbP->m_buffer != 0) && (bbP->m_buffer != bufferP)) {
      free(bbP->m_buffer);
  }
  bbP->m_buffer_allocated_by_this = BIT_BUFFER_FALSE;
  bbP->m_buffer                   = bufferP;
  bbP->m_byte_position            = 0;
  bbP->m_bit_mod_8_position       = 0;
  bbP->m_limit                    = capacityP;
  bbP->m_capacity                 = capacityP;
}
//-----------------------------------------------------------------------------
u_int32_t BitBuffer_read32(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (u_int32_t)(BitBuffer_read(bbP, 32));
}
//-----------------------------------------------------------------------------
u_int32_t BitBuffer_read32littleendian(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (u_int32_t)(ntohl(BitBuffer_read(bbP,32)));
}
//-----------------------------------------------------------------------------
u_int16_t BitBuffer_read16(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (u_int16_t)(BitBuffer_read(bbP,16));
}
//-----------------------------------------------------------------------------
u_int16_t BitBuffer_read16littleendian(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (u_int16_t)(ntohs((u_int16_t)(BitBuffer_read(bbP, 16))));
}
//-----------------------------------------------------------------------------
unsigned char BitBuffer_read8(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{

  return (u_int8_t)(BitBuffer_read(bbP,8));
}
//-----------------------------------------------------------------------------
u_int8_t BitBuffer_readBool(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (u_int8_t)(BitBuffer_read(bbP,1));
}
//-----------------------------------------------------------------------------
u_int32_t BitBuffer_read(Bit_Buffer_t* bbP, const unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
    u_int64_t             data_l       = 0;
    u_int32_t             mask         = 0xFFFFFFFFL;
    u_int32_t             bit_index_l  = bbP->m_bit_mod_8_position;
    int32_t               bit_length_l = nb_bitsP;
    u_int32_t             byte_index_l = bbP->m_byte_position;
    u_int32_t             rotate_l     = 0;
    //cout << dec << "BitBuffer_read(" << nb_bitsP << ")"<< endl;
    if (BitBuffer_isCheckReadOverflowOK(bbP, nb_bitsP)) {
        if (nb_bitsP > (sizeof(unsigned long)*8)) {
            printf("[MIH_C] ERROR BitBuffer_read nb bits too large - must be <= sizeof(long int)*8");
        } else {
            do {
                data_l          = data_l << 8;
                data_l          = data_l | (unsigned long long int)(bbP->m_buffer[byte_index_l++]);
                //cout << hex << "data_l= " << data_l << endl;
                            bit_length_l   -= (8- bit_index_l);
                bit_index_l     = 0;
                rotate_l       += 8;
            } while (bit_length_l > 0);

            //cout << dec << "data_l rotated >> by  " << rotate_l - bbP->m_bit_mod_8_position - nb_bitsP << " bits " << endl;
            //cout << hex << "data_l masked  >> by  " <<  (mask >> (sizeof(unsigned long)*8 - nb_bitsP)) << endl;
            data_l = (data_l >> (rotate_l - bbP->m_bit_mod_8_position - nb_bitsP)) &
                        (mask >> (sizeof(unsigned long)*8 - nb_bitsP));
            bbP->m_bit_mod_8_position = (bbP->m_bit_mod_8_position + nb_bitsP) % 8;
            if (bbP->m_bit_mod_8_position == 0) {
                bbP->m_byte_position = byte_index_l;
            } else {
                bbP->m_byte_position = byte_index_l - 1;
            }
        }
        return (u_int32_t)(data_l);
    }
    return (u_int32_t)(data_l);
}
//-----------------------------------------------------------------------------
u_int32_t BitBuffer_readlittleendian(Bit_Buffer_t* bbP, const unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
    if (nb_bitsP > 24) {
        return ntohl(BitBuffer_read(bbP,nb_bitsP));
    } else if (nb_bitsP > 16) {
        return ntohl(BitBuffer_read(bbP,nb_bitsP) << 8);
    } else if (nb_bitsP > 8) {
        return ntohl(BitBuffer_read(bbP,nb_bitsP) << 16);
    } else  {
        return ntohl(BitBuffer_read(bbP,nb_bitsP) << 24);
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_readMem(Bit_Buffer_t* bbP, u_int8_t* destP, unsigned int nb_bytesP)
//-----------------------------------------------------------------------------
{
    assert (bbP->m_bit_mod_8_position == 0); // TO DO

    if (BitBuffer_isCheckReadOverflowOK(bbP,nb_bytesP *8)) {
      memcpy(destP,  &bbP->m_buffer[bbP->m_byte_position], nb_bytesP);
      bbP->m_byte_position += nb_bytesP;
   }
}

//-----------------------------------------------------------------------------
void BitBuffer_write32(Bit_Buffer_t* bbP, u_int32_t valueP)
//-----------------------------------------------------------------------------
{
    if (BitBuffer_isCheckWriteOverflowOK(bbP, sizeof(u_int32_t)*8)) {
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 24));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 16));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
        BitBuffer_write8(bbP,(u_int8_t)(valueP));
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_write32littleendian(Bit_Buffer_t* bbP, u_int32_t valueP)
//-----------------------------------------------------------------------------
{
    if (BitBuffer_isCheckWriteOverflowOK(bbP,sizeof(u_int32_t)*8)) {
        BitBuffer_write8(bbP,(u_int8_t)(valueP));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 16));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 24));
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_write16littleendian(Bit_Buffer_t* bbP, u_int16_t valueP)
//-----------------------------------------------------------------------------
{
    if (BitBuffer_isCheckWriteOverflowOK(bbP,sizeof(u_int16_t)*8)) {
        BitBuffer_write8(bbP,(u_int8_t)(valueP));
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_write16(Bit_Buffer_t* bbP, u_int16_t valueP)
//-----------------------------------------------------------------------------
{
    if (BitBuffer_isCheckWriteOverflowOK(bbP,sizeof(u_int16_t)*8)) {
        BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
        BitBuffer_write8(bbP,(u_int8_t)(valueP));
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_write8(Bit_Buffer_t* bbP, u_int8_t valueP)
//-----------------------------------------------------------------------------
{
    if (BitBuffer_isCheckWriteOverflowOK(bbP,sizeof(u_int8_t)*8)) {
        if (bbP->m_bit_mod_8_position == 0) {
            bbP->m_buffer[bbP->m_byte_position++] = valueP;
        } else {
            bbP->m_buffer[bbP->m_byte_position] = (valueP >> bbP->m_bit_mod_8_position) | bbP->m_buffer[bbP->m_byte_position];
            bbP->m_byte_position++;
            bbP->m_buffer[bbP->m_byte_position] =  valueP << (8 - bbP->m_bit_mod_8_position);
        }
    }
}
//-----------------------------------------------------------------------------
void BitBuffer_writeBool(Bit_Buffer_t* bbP, u_int8_t valueP)
//-----------------------------------------------------------------------------
{
   if (BitBuffer_isCheckWriteOverflowOK(bbP,1)) {
       if (valueP) {
           BitBuffer_write8b(bbP,(u_int8_t)(0x01), 1);
       } else {
           BitBuffer_write8b(bbP,(u_int8_t)(0x00), 1);
       }
   }
}
//-----------------------------------------------------------------------------
void BitBuffer_writeMem(Bit_Buffer_t* bbP, u_int8_t* startP, unsigned int nb_bytesP)
//-----------------------------------------------------------------------------
{
    assert (bbP->m_bit_mod_8_position == 0); // TO DO

    if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bytesP *8)) {
      memcpy(&bbP->m_buffer[bbP->m_byte_position], startP,  nb_bytesP);
      bbP->m_byte_position += nb_bytesP;
   }
}
//-----------------------------------------------------------------------------
void BitBuffer_write8b(Bit_Buffer_t* bbP, u_int8_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   assert(nb_bitsP <= 8);
   if (nb_bitsP == 0) return;
   if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bitsP)) {
        valueP = valueP & ((u_int8_t)(0xFF) >> (8 - nb_bitsP));
    //std::cout << "BitBuffer_write char " << (u_int32_t)(valueP) << " " << nb_bitsP  << " bits" << std::endl;
    if (nb_bitsP > (sizeof(unsigned char)*8)) {
        printf("[MIH_C] ERROR BitBuffer_write nb bits too large - must be <= sizeof(char)*8");
    } else {
        if (bbP->m_bit_mod_8_position == 0) {
            if (nb_bitsP == (sizeof(unsigned char)*8)) {
                BitBuffer_write8(bbP, valueP);
            } else {
                bbP->m_buffer[bbP->m_byte_position] = valueP << (8 - nb_bitsP);
                bbP->m_bit_mod_8_position += nb_bitsP;
            }
        } else {
            bbP->m_buffer[bbP->m_byte_position] = ((valueP << (8 - nb_bitsP)) >> bbP->m_bit_mod_8_position) | bbP->m_buffer[bbP->m_byte_position];
            if ((bbP->m_bit_mod_8_position  + nb_bitsP)>= (sizeof(unsigned char)*8)) {
                bbP->m_byte_position++;
                bbP->m_buffer[bbP->m_byte_position] =  valueP << (16 - bbP->m_bit_mod_8_position - nb_bitsP);
            }
            bbP->m_bit_mod_8_position = (nb_bitsP + bbP->m_bit_mod_8_position) % (sizeof(unsigned char)*8);
        }
    }
  }
}
//-----------------------------------------------------------------------------
void BitBuffer_writelittleendian(Bit_Buffer_t* bbP, u_int8_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   BitBuffer_write8b(bbP,valueP, nb_bitsP);
}
//-----------------------------------------------------------------------------
void BitBuffer_write16b(Bit_Buffer_t* bbP, u_int16_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   assert(nb_bitsP <= 16);
   if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bitsP)) {
        valueP = valueP & ((u_int16_t)(0xFFFF) >> (16 - nb_bitsP));
    if (nb_bitsP > (sizeof (u_int16_t)*8)) {
        printf("[MIH_C] ERROR BitBuffer_write nb bits too large - must be <= sizeof (short)*8");
    } else {
        if (nb_bitsP > 8) {
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (8 - (16-nb_bitsP))), nb_bitsP - 8);
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 8), nb_bitsP - 8);
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
        } else if (nb_bitsP > 0) {
            BitBuffer_write8b(bbP,(u_int8_t)(valueP), nb_bitsP);
        }
    }
  }
}
//-----------------------------------------------------------------------------
void BitBuffer_writelittleendian16b(Bit_Buffer_t* bbP, u_int16_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   assert(nb_bitsP <= 16);
   if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bitsP)) {
        valueP = valueP & ((u_int16_t)(0xFFFF) >> (16 - nb_bitsP));
    if (nb_bitsP > (sizeof (u_int16_t)*8)) {
        printf("[MIH_C] ERROR BitBuffer_write nb bits too large - must be <= sizeof (short)*8");
    } else {
        if (nb_bitsP > 8) {
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (8 - (16-nb_bitsP))), nb_bitsP - 8);
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 8), nb_bitsP - 8);
        } else if (nb_bitsP > 0) {
            BitBuffer_write8b(bbP,(u_int8_t)(valueP), nb_bitsP);
        }
    }
  }
}
//-----------------------------------------------------------------------------
void BitBuffer_write32b(Bit_Buffer_t* bbP, u_int32_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   assert(nb_bitsP <= 32);
   if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bitsP)) {
        valueP = valueP & ((u_int32_t)(0xFFFFFFFF) >> (32 - nb_bitsP));
    if (nb_bitsP > (sizeof (u_int32_t)*8)) {
        printf("[MIH_C] ERROR BitBuffer_write nb bits too large - must be <= sizeof (int)*8");
    } else {
        if (nb_bitsP > 24) {
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (24 - (32-nb_bitsP))), nb_bitsP - 24);
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 24), nb_bitsP - 24);
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 16));
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
        } else if (nb_bitsP > 16) {
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (16 - (24-nb_bitsP))), nb_bitsP - 16);
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 16), nb_bitsP - 16);
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
        } else if (nb_bitsP > 8) {
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (8 - (16-nb_bitsP))), nb_bitsP - 8);
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 8), nb_bitsP - 8);
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
        } else if (nb_bitsP > 0) {
            BitBuffer_write8b(bbP,(u_int8_t)(valueP), nb_bitsP);
        }
    }
  }
}
//-----------------------------------------------------------------------------
void BitBuffer_writelittleendian32b(Bit_Buffer_t* bbP, u_int32_t valueP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
   assert(nb_bitsP <= 32);
   if (BitBuffer_isCheckWriteOverflowOK(bbP,nb_bitsP)) {
        valueP = valueP & ((u_int32_t)(0xFFFFFFFF) >> (32 - nb_bitsP));
    if (nb_bitsP > (sizeof (u_int32_t)*8)) {
        printf("[MIH_C] ERROR BitBuffer_write nb bits too large - must be <= sizeof (int)*8");
    } else {
        if (nb_bitsP > 24) {
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 16));
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 24), nb_bitsP - 24);
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (24 - (32-nb_bitsP))), nb_bitsP - 24);
        } else if (nb_bitsP > 16) {
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
            BitBuffer_write8(bbP,(u_int8_t)(valueP >> 8));
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 16), nb_bitsP - 16);
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (16 - (24-nb_bitsP))), nb_bitsP - 16);
        } else if (nb_bitsP > 8) {
            BitBuffer_write8(bbP,(u_int8_t)(valueP));
            BitBuffer_write8b(bbP,(u_int8_t)(valueP >> 8), nb_bitsP - 8);
            //BitBuffer_write8b(bbP,(u_int8_t)(valueP >> (8 - (16-nb_bitsP))), nb_bitsP - 8);
        } else if (nb_bitsP > 0) {
            BitBuffer_write8b(bbP,(u_int8_t)(valueP), nb_bitsP);
        }
    }
  }
}
//-----------------------------------------------------------------------------
void   BitBuffer_rewind(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  bbP->m_byte_position = 0;
  bbP->m_bit_mod_8_position  = 0;
}
//-----------------------------------------------------------------------------
void   BitBuffer_rewind_to(Bit_Buffer_t* bbP, unsigned int byte_positionP)
//-----------------------------------------------------------------------------
{
  bbP->m_byte_position = byte_positionP;
  bbP->m_bit_mod_8_position  = 0;
}
//-----------------------------------------------------------------------------
void   BitBuffer_write_shift_last_n_bytes_right(Bit_Buffer_t* bbP, unsigned int nb_bytes_to_shiftP, unsigned int hole_sizeP)
//-----------------------------------------------------------------------------
{
    unsigned int index = 0;
    if (nb_bytes_to_shiftP >= bbP->m_byte_position) return;
    if ((bbP->m_byte_position + hole_sizeP) > bbP->m_capacity) return;
    while (index < nb_bytes_to_shiftP) {
        bbP->m_buffer[bbP->m_byte_position -1 - index + hole_sizeP] = bbP->m_buffer[bbP->m_byte_position -1 - index];
        index += 1;
    }
}
//-----------------------------------------------------------------------------
void   BitBuffer_reset(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  BitBuffer_rewind(bbP);
  bbP->m_limit  = 0;
  if (bbP->m_buffer != 0) {
    memset(bbP->m_buffer,  0, bbP->m_capacity);
  }
}
//-----------------------------------------------------------------------------
unsigned char*   BitBuffer_getNextFreePosition(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return &(bbP->m_buffer[bbP->m_byte_position]);
}
//-----------------------------------------------------------------------------
unsigned int   BitBuffer_getPosition(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return (bbP->m_byte_position + (bbP->m_bit_mod_8_position +7)/8);
}
//-----------------------------------------------------------------------------
void   BitBuffer_addLimitOffset(Bit_Buffer_t* bbP, unsigned int offsetP)
//-----------------------------------------------------------------------------
{
  bbP->m_limit += offsetP;
}
//-----------------------------------------------------------------------------
unsigned int BitBuffer_getNumFreeBytes(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  return bbP->m_capacity - (bbP->m_byte_position + (bbP->m_bit_mod_8_position +7)/8);
}
//-----------------------------------------------------------------------------
u_int8_t  BitBuffer_isCheckWriteOverflowOK(Bit_Buffer_t* bbP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
  if ((bbP->m_byte_position + (bbP->m_bit_mod_8_position + nb_bitsP+7)/8) < bbP->m_capacity) {
    return BIT_BUFFER_TRUE;
  } else {
    //throw std::out_of_range("Exception in class BitBuffer cannot write more, no more space available");
    return BIT_BUFFER_FALSE;
  }
}
//-----------------------------------------------------------------------------
u_int8_t  BitBuffer_isCheckReadOverflowOK(Bit_Buffer_t* bbP, unsigned int nb_bitsP)
//-----------------------------------------------------------------------------
{
  if ((bbP->m_byte_position + (bbP->m_bit_mod_8_position + nb_bitsP)/8) <= bbP->m_limit) {
    return BIT_BUFFER_TRUE;
  } else {
    //throw std::out_of_range("Exception in class BitBuffer cannot read more - end of buffer reached");
    return BIT_BUFFER_FALSE;
  }
}
//-----------------------------------------------------------------------------
void free_BitBuffer(Bit_Buffer_t* bbP)
//-----------------------------------------------------------------------------
{
  //std::cout << "~BitBuffer()" << std::endl;
  if (bbP->m_buffer_allocated_by_this == BIT_BUFFER_TRUE) {
    free(bbP->m_buffer);
  }
  free(bbP);
}
