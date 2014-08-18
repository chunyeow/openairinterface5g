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
/*! \file MIH_C_bit_buffer.h
 * \brief This file defines the prototypes of the functions for coding and decoding of bit fields.
 * \author GAUTHIER Lionel
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_BIT_BUFFER_H__
#    define __MIH_C_BIT_BUFFER_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_BIT_BUFFER_C
#            define private_bit_buffer(x)    x
#            define protected_bit_buffer(x)  x
#            define public_bit_buffer(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_bit_buffer(x)
#                define protected_bit_buffer(x)  extern x
#                define public_bit_buffer(x)     extern x
#            else
#                define private_bit_buffer(x)
#                define protected_bit_buffer(x)
#                define public_bit_buffer(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <sys/types.h>
//-----------------------------------------------------------------------------
#define BIT_BUFFER_FALSE  0
#define BIT_BUFFER_TRUE   1
typedef struct  BitBuffer {
  unsigned int    m_buffer_allocated_by_this;
  u_int8_t       *m_buffer;
  // size allocated for buffer
  unsigned int    m_capacity;

  // amount of bytes written
  unsigned int    m_limit;
  unsigned int    m_byte_position;
  unsigned int    m_bit_mod_8_position;

} Bit_Buffer_t;

//-----------------------------------------------------------------------------
public_bit_buffer(Bit_Buffer_t* new_BitBuffer_0(void);)

public_bit_buffer(Bit_Buffer_t* new_BitBuffer_1(unsigned int capacityP);)

public_bit_buffer(Bit_Buffer_t* new_BitBuffer_2(unsigned char* bufferP, unsigned int capacityP);)

public_bit_buffer(void BitBuffer_wrap(Bit_Buffer_t* bbP, unsigned char* bufferP, unsigned int capacityP);)

public_bit_buffer(u_int32_t BitBuffer_read32(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int32_t BitBuffer_read32littleendian(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int16_t BitBuffer_read16(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int16_t BitBuffer_read16littleendian(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int8_t BitBuffer_read8(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int8_t BitBuffer_readBool(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int32_t BitBuffer_read(Bit_Buffer_t* bbP, const unsigned int nb_bitsP);)

public_bit_buffer(u_int32_t BitBuffer_readlittleendian(Bit_Buffer_t* bbP, const unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_readMem(Bit_Buffer_t* bbP, u_int8_t* destP, unsigned int nb_bytesP);)


public_bit_buffer(void BitBuffer_write32(Bit_Buffer_t* bbP, u_int32_t valueP);)

public_bit_buffer(void BitBuffer_write32littleendian(Bit_Buffer_t* bbP, u_int32_t valueP);)

public_bit_buffer(void BitBuffer_write16littleendian(Bit_Buffer_t* bbP, u_int16_t valueP);)

public_bit_buffer(void BitBuffer_write16(Bit_Buffer_t* bbP, u_int16_t valueP);)

public_bit_buffer(void BitBuffer_write8(Bit_Buffer_t* bbP, u_int8_t valueP);)

public_bit_buffer(void BitBuffer_writeBool(Bit_Buffer_t* bbP, u_int8_t valueP);)

public_bit_buffer(void BitBuffer_writeMem(Bit_Buffer_t* bbP, u_int8_t* startP, unsigned int nb_bytesP);)

public_bit_buffer(void BitBuffer_write8b(Bit_Buffer_t* bbP, u_int8_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_writelittleendian(Bit_Buffer_t* bbP, u_int8_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_write16b(Bit_Buffer_t* bbP, u_int16_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_writelittleendian16b(Bit_Buffer_t* bbP, u_int16_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_write32b(Bit_Buffer_t* bbP, u_int32_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void BitBuffer_writelittleendian32b(Bit_Buffer_t* bbP, u_int32_t valueP, unsigned int nb_bitsP);)

public_bit_buffer(void   BitBuffer_rewind(Bit_Buffer_t* bbP);)

public_bit_buffer(void   BitBuffer_rewind_to(Bit_Buffer_t* bbP, unsigned int byte_positionP);)

public_bit_buffer(void   BitBuffer_write_shift_last_n_bytes_right(Bit_Buffer_t* bbP, unsigned int nb_bytes_to_shiftP, unsigned int hole_sizeP);)

public_bit_buffer(void   BitBuffer_reset(Bit_Buffer_t* bbP);)

public_bit_buffer(unsigned char*   BitBuffer_getNextFreePosition(Bit_Buffer_t* bbP);)

public_bit_buffer(unsigned int   BitBuffer_getPosition(Bit_Buffer_t* bbP);)

public_bit_buffer(void   BitBuffer_addLimitOffset(Bit_Buffer_t* bbP, unsigned int offsetP);)

public_bit_buffer(unsigned int BitBuffer_getNumFreeBytes(Bit_Buffer_t* bbP);)

public_bit_buffer(u_int8_t  BitBuffer_isCheckWriteOverflowOK(Bit_Buffer_t* bbP, unsigned int nb_bitsP);)

public_bit_buffer(u_int8_t  BitBuffer_isCheckReadOverflowOK(Bit_Buffer_t* bbP, unsigned int nb_bitsP);)

public_bit_buffer(void free_BitBuffer(Bit_Buffer_t* bbP);)

#endif
