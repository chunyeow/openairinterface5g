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
/*! \file MIH_C_primitive_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding of MIH Link messages.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_PRIMITIVE_CODEC_H__
#    define __MIH_C_PRIMITIVE_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef mih_c_primitive_codec_C
#            define private_mih_c_primitive_codec(x)    x
#            define protected_mih_c_primitive_codec(x)  x
#            define public_mih_c_primitive_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_mih_c_primitive_codec(x)
#                define protected_mih_c_primitive_codec(x)  extern x
#                define public_mih_c_primitive_codec(x)     extern x
#            else
#                define private_mih_c_primitive_codec(x)
#                define protected_mih_c_primitive_codec(x)
#                define public_mih_c_primitive_codec(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------
#define MIH_PRIMITIVE_DECODE_OK             0
#define MIH_PRIMITIVE_DECODE_TOO_SHORT     -1
#define MIH_PRIMITIVE_DECODE_FAILURE       -2
#define MIH_PRIMITIVE_DECODE_BAD_PARAMETER -3
#define MIH_PRIMITIVE_ENCODE_OK             0
#define MIH_PRIMITIVE_ENCODE_TOO_SHORT     -1
#define MIH_PRIMITIVE_ENCODE_FAILURE       -2
#define MIH_PRIMITIVE_ENCODE_BAD_PARAMETER -3
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Capability_Discover_request (Bit_Buffer_t* bbP, MIH_C_Link_Capability_Discover_request_t     *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Capability_Discover_request2String (MIH_C_Link_Capability_Discover_request_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Event_Subscribe_request     (Bit_Buffer_t* bbP, MIH_C_Link_Event_Subscribe_request_t         *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Event_Subscribe_request2String     (MIH_C_Link_Event_Subscribe_request_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Event_Unsubscribe_request   (Bit_Buffer_t* bbP, MIH_C_Link_Event_Unsubscribe_request_t       *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Event_Unsubscribe_request2String   (MIH_C_Link_Event_Unsubscribe_request_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Get_Parameters_request      (Bit_Buffer_t* bbP, MIH_C_Link_Get_Parameters_request_t          *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Get_Parameters_request2String      (MIH_C_Link_Get_Parameters_request_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Configure_Thresholds_request(Bit_Buffer_t* bbP, MIH_C_Link_Configure_Thresholds_request_t    *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Configure_Thresholds_request2String(MIH_C_Link_Configure_Thresholds_request_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Decode_Link_Action_request              (Bit_Buffer_t* bbP, MIH_C_Link_Action_request_t                  *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Action_request2String              (MIH_C_Link_Action_request_t *primitiveP, char* bufP);)

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Register_indication         (Bit_Buffer_t* bbP, MIH_C_Link_Register_indication_t             *primitiveP);)
#endif
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Detected_indication         (Bit_Buffer_t* bbP, MIH_C_Link_Detected_indication_t             *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Up_indication               (Bit_Buffer_t* bbP, MIH_C_Link_Up_indication_t                   *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Parameters_Report_indication(Bit_Buffer_t* bbP, MIH_C_Link_Parameters_Report_indication_t    *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Going_Down_indication       (Bit_Buffer_t* bbP, MIH_C_Link_Going_Down_indication_t           *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Down_indication             (Bit_Buffer_t* bbP, MIH_C_Link_Down_indication_t                 *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Register_indication2String         (MIH_C_Link_Register_indication_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Capability_Discover_confirm (Bit_Buffer_t* bbP, MIH_C_Link_Capability_Discover_confirm_t     *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Link_Capability_Discover_confirm2String (MIH_C_Link_Capability_Discover_confirm_t *primitiveP, char* bufP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Event_Subscribe_confirm     (Bit_Buffer_t* bbP, MIH_C_Link_Event_Subscribe_confirm_t         *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Event_Unsubscribe_confirm   (Bit_Buffer_t* bbP, MIH_C_Link_Event_Unsubscribe_confirm_t       *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Configure_Thresholds_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Configure_Thresholds_confirm_t    *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Get_Parameters_confirm      (Bit_Buffer_t* bbP, MIH_C_Link_Get_Parameters_confirm_t          *primitiveP);)
protected_mih_c_primitive_codec(int MIH_C_Link_Primitive_Encode_Link_Action_confirm              (Bit_Buffer_t* bbP, MIH_C_Link_Action_confirm_t                  *primitiveP);)
#endif
