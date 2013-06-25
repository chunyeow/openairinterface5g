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
/*! \file MIH_C_L2_type_values_for_tlv_encoding.h
 * \brief This file defines the prototypes of the functions for coding and decoding Data type for MIHF identification defined in Std 802.21-2008 Table L2.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_L2_TYPE_VALUES_FOR_TLV_ENCODING L2 Type values for TLV encoding.
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_L2_TYPE_VALUES_FOR_TLV_ENCODING_H__
#    define __MIH_C_L2_TYPE_VALUES_FOR_TLV_ENCODING_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_L2_TYPE_VALUES_FOR_TLV_ENCODING_C
#            define private_L2_codec(x)    x
#            define protected_L2_codec(x)  x
#            define public_L2_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_L2_codec(x)
#                define protected_L2_codec(x)  extern x
#                define public_L2_codec(x)     extern x
#            else
#                define private_L2_codec(x)
#                define protected_L2_codec(x)
#                define public_L2_codec(x)     extern x
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

public_L2_codec(unsigned int MIH_C_TLV2String(MIH_C_TLV_TYPE_T *typeP, char* bufP);)
/** @}*/
#endif
