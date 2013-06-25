/*******************************************************************************
 *
 * Eurecom OpenAirInterface 3
 * Copyright(c) 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/
/*! \file MIH_C_F1_basic_data_types_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding basic data types defined in Std 802.21-2008 Table F1.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_F1_BASIC_DATA_TYPES F1 Basic data types codec
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_F1_BASIC_DATA_TYPES_CODEC_H__
#    define __MIH_C_F1_BASIC_DATA_TYPES_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
#            define private_F1_codec(x)    x
#            define protected_F1_codec(x)  x
#            define public_F1_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_F1_codec(x)
#                define protected_F1_codec(x)  extern x
#                define public_F1_codec(x)     extern x
#            else
#                define private_F1_codec(x)
#                define protected_F1_codec(x)
#                define public_F1_codec(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------
typedef struct MIH_C_F1_Generic_List {
    u_int16_t length;
    u_int8_t  val[2];
}MIH_C_F1_Generic_List_t;
//-----------------------------------------------------------------------------
typedef struct MIH_C_F1_Generic_Octet {
    u_int8_t  val[2];
}MIH_C_F1_Generic_Octet_t;
//-----------------------------------------------------------------------------
public_F1_codec( inline void MIH_C_BITMAP8_encode      (Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP16_encode     (Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP24_encode     (Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP32_encode     (Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP64_encode     (Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP128_encode    (Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP256_encode    (Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP);)
public_F1_codec( inline void MIH_C_CHOICE_encode       (Bit_Buffer_t* bbP, MIH_C_CHOICE_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER1_encode     (Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER2_encode     (Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER4_encode     (Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER8_encode     (Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP);)
public_F1_codec( inline void MIH_C_NULL_encode         (Bit_Buffer_t* bbP, MIH_C_NULL_T* dataP);)
public_F1_codec( inline void MIH_C_OCTET_encode        (Bit_Buffer_t* bbP, MIH_C_F1_Generic_Octet_t *dataP, int lengthP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT1_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT2_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT4_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT8_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP);)
public_F1_codec( inline void MIH_C_LIST_LENGTH_encode  (Bit_Buffer_t* bbP, u_int16_t lengthP);)
//-----------------------------------------------------------------------------
public_F1_codec( inline void MIH_C_BITMAP8_decode       (Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP16_decode      (Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP24_decode      (Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP32_decode      (Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP64_decode      (Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP128_decode     (Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP);)
public_F1_codec( inline void MIH_C_BITMAP256_decode     (Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP);)
public_F1_codec( inline void MIH_C_CHOICE_decode        (Bit_Buffer_t* bbP, MIH_C_CHOICE_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER1_decode      (Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER2_decode      (Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER4_decode      (Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP);)
public_F1_codec( inline void MIH_C_INTEGER8_decode      (Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP);)
public_F1_codec( inline void MIH_C_NULL_decode          (Bit_Buffer_t* bbP);)
public_F1_codec( inline void MIH_C_OCTET_decode         (Bit_Buffer_t* bbP, MIH_C_F1_Generic_Octet_t *dataP, int lengthP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT1_decode (Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT2_decode (Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT4_decode (Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP);)
public_F1_codec( inline void MIH_C_UNSIGNED_INT8_decode (Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP);)
public_F1_codec( inline void MIH_C_LIST_LENGTH_decode   (Bit_Buffer_t* bbP, u_int16_t *lengthP);)
//-----------------------------------------------------------------------------
public_F1_codec( unsigned int MIH_C_BITMAP82String(MIH_C_BITMAP8_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_BITMAP162String(MIH_C_BITMAP16_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_BITMAP242String(MIH_C_BITMAP24_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_UNSIGNED_INT12String(MIH_C_UNSIGNED_INT1_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_UNSIGNED_INT22String(MIH_C_UNSIGNED_INT2_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_UNSIGNED_INT42String(MIH_C_UNSIGNED_INT4_T* dataP, char* bufP);)
public_F1_codec( unsigned int MIH_C_UNSIGNED_INT82String(MIH_C_UNSIGNED_INT8_T* dataP, char* bufP);)
//-----------------------------------------------------------------------------
public_F1_codec(inline u_int16_t MIH_C_LIST_LENGTH_get_encode_num_bytes(u_int16_t lengthP);)

#endif
/** @}*/
