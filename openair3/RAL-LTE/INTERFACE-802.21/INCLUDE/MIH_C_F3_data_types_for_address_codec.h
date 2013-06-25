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
