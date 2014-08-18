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
/*! \file MIH_C_F3_data_types_for_address_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding data types for addresses defined in Std 802.21-2008 Table F3.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC F3 Data types for address codec
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC_H__
#    define __MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC_C
#            define private_F3_codec(x)    x
#            define protected_F3_codec(x)  x
#            define public_F3_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_F3_codec(x)
#                define protected_F3_codec(x)  extern x
#                define public_F3_codec(x)     extern x
#            else
#                define private_F3_codec(x)
#                define protected_F3_codec(x)
#                define public_F3_codec(x)     extern x
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
public_F3_codec(unsigned int  MIH_C_3GPP_2G_CELL_ID2String(MIH_C_3GPP_2G_CELL_ID_T *dataP, char* bufP);)
public_F3_codec(inline void MIH_C_3GPP_2G_CELL_ID_encode     (Bit_Buffer_t* bbP, MIH_C_3GPP_2G_CELL_ID_T *dataP);)
public_F3_codec(inline void MIH_C_3GPP_2G_CELL_ID_decode     (Bit_Buffer_t* bbP, MIH_C_3GPP_2G_CELL_ID_T *dataP);)
public_F3_codec(unsigned int  MIH_C_3GPP_3G_CELL_ID2String(MIH_C_3GPP_3G_CELL_ID_T *dataP, char* bufP);)
public_F3_codec(inline void MIH_C_3GPP_3G_CELL_ID_encode     (Bit_Buffer_t* bbP, MIH_C_3GPP_3G_CELL_ID_T *dataP);)
public_F3_codec(inline void MIH_C_3GPP_3G_CELL_ID_decode     (Bit_Buffer_t* bbP, MIH_C_3GPP_3G_CELL_ID_T *dataP);)
public_F3_codec(unsigned int MIH_C_LINK_ADDR2String(MIH_C_LINK_ADDR_T *dataP, char* bufP);)
public_F3_codec(inline void MIH_C_LINK_ADDR_encode           (Bit_Buffer_t* bbP, MIH_C_LINK_ADDR_T       *dataP);)
public_F3_codec(inline void MIH_C_LINK_ADDR_decode           (Bit_Buffer_t* bbP, MIH_C_LINK_ADDR_T       *dataP);)
#endif
/** @}*/
