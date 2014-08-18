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
