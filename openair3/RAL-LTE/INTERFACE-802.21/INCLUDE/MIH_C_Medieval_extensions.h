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
/*! \file MIH_C_Medieval_extensions.h
 * \brief This file defines the prototypes of the functions for coding and decoding data types defined by the Medieval project.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_MEDIEVAL_EXTENSIONS Extensions to 802.21 defined by MEDIVAL IST Project.
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */
#ifndef __MIH_C_MEDIEVAL_EXTENSIONS_H__
#    define __MIH_C_MEDIEVAL_EXTENSIONS_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_MEDIEVAL_EXTENSIONS_C
#            define private_Medieval_extensions(x)    x
#            define protected_Medieval_extensions(x)  x
#            define public_Medieval_extensions(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_Medieval_extensions(x)
#                define protected_Medieval_extensions(x)  extern x
#                define public_Medieval_extensions(x)     extern x
#            else
#                define private_Medieval_extensions(x)
#                define protected_Medieval_extensions(x)
#                define public_Medieval_extensions(x)     extern x
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
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
public_Medieval_extensions(unsigned int        MIH_C_PROTO2String   (MIH_C_PROTO_T    *protoP,char* bufP);)
public_Medieval_extensions(unsigned int        MIH_C_IP_TUPLE2String(MIH_C_IP_TUPLE_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_IP_TUPLE_encode(Bit_Buffer_t* bbP, MIH_C_IP_TUPLE_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_IP_TUPLE_decode(Bit_Buffer_t* bbP, MIH_C_IP_TUPLE_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_FLOW_ID2String(MIH_C_FLOW_ID_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_FLOW_ID_encode(Bit_Buffer_t* bbP, MIH_C_FLOW_ID_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_FLOW_ID_decode(Bit_Buffer_t* bbP, MIH_C_FLOW_ID_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_MARK2String(MIH_C_MARK_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_MARK_encode(Bit_Buffer_t* bbP, MIH_C_MARK_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_MARK_decode(Bit_Buffer_t* bbP, MIH_C_MARK_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_QOS2String(MIH_C_QOS_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_QOS_encode(Bit_Buffer_t* bbP, MIH_C_QOS_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_QOS_decode(Bit_Buffer_t* bbP, MIH_C_QOS_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_RESOURCE_DESC2String(MIH_C_RESOURCE_DESC_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_RESOURCE_DESC_encode(Bit_Buffer_t* bbP, MIH_C_RESOURCE_DESC_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_RESOURCE_DESC_decode(Bit_Buffer_t* bbP, MIH_C_RESOURCE_DESC_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_FLOW_ATTRIBUTE2String(MIH_C_FLOW_ATTRIBUTE_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_FLOW_ATTRIBUTE_encode(Bit_Buffer_t* bbP, MIH_C_FLOW_ATTRIBUTE_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_FLOW_ATTRIBUTE_decode(Bit_Buffer_t* bbP, MIH_C_FLOW_ATTRIBUTE_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_LINK_AC_PARAM2String(MIH_C_LINK_AC_PARAM_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_LINK_AC_PARAM_encode(Bit_Buffer_t* bbP, MIH_C_LINK_AC_PARAM_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_LINK_AC_PARAM_decode(Bit_Buffer_t* bbP, MIH_C_LINK_AC_PARAM_T *dataP);)
public_Medieval_extensions(unsigned int        MIH_C_LINK_ACTION2String(MIH_C_LINK_ACTION_T *dataP, char* bufP);)
public_Medieval_extensions(inline void         MIH_C_LINK_ACTION_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_LINK_ACTION_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP);)
public_Medieval_extensions(inline void         MIH_C_LINK_ACTION_short_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP);)

#endif
#endif
/** @}*/


