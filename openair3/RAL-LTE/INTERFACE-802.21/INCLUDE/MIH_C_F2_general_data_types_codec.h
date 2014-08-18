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
/*! \file MIH_C_F2_general_data_types_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding general data types defined in Std 802.21-2008 Table F2.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_F2_GENERAL_DATA_TYPES_CODEC F2 General data types codec
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_F2_GENERAL_DATA_TYPES_CODEC_H__
#    define __MIH_C_F2_GENERAL_DATA_TYPES_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
#            define private_F2_codec(x)    x
#            define protected_F2_codec(x)  x
#            define public_F2_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_F2_codec(x)
#                define protected_F2_codec(x)  extern x
#                define public_F2_codec(x)     extern x
#            else
#                define private_F2_codec(x)
#                define protected_F2_codec(x)
#                define public_F2_codec(x)     extern x
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
public_F2_codec( unsigned int MIH_C_BOOLEAN2String(MIH_C_BOOLEAN_T* dataP, char* bufP);)
public_F2_codec( unsigned int MIH_C_STATUS2String(MIH_C_STATUS_T *statusP, char* bufP);)

#endif
/** @}*/
