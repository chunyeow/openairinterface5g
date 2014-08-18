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
/*! \file MIH_C_header_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding of MIH header messages.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_HEADER_CODEC_H__
#    define __MIH_C_HEADER_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_HEADER_CODEC_C
#            define private_header_codec(x)    x
#            define protected_header_codec(x)  x
#            define public_header_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_header_codec(x)
#                define protected_header_codec(x)  extern x
#                define public_header_codec(x)     extern x
#            else
#                define private_header_codec(x)
#                define protected_header_codec(x)
#                define public_header_codec(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
//-----------------------------------------------------------------------------
#include "MIH_C_bit_buffer.h"
#include "MIH_C_Types.h"
//-----------------------------------------------------------------------------
#define MIH_HEADER_ENCODE_OK             0
#define MIH_HEADER_DECODE_OK             0
#define MIH_HEADER_ENCODE_TOO_SHORT     -1
#define MIH_HEADER_DECODE_TOO_SHORT     -1
#define MIH_HEADER_ENCODE_FAILURE       -2
#define MIH_HEADER_DECODE_FAILURE       -2
#define MIH_HEADER_ENCODE_BAD_PARAMETER -3
#define MIH_HEADER_DECODE_BAD_PARAMETER -3

TYPEDEF_INTEGER1(MIH_C_VERSION);
TYPEDEF_INTEGER1(MIH_C_ACK_REQ);
TYPEDEF_INTEGER1(MIH_C_ACK_RSP);
TYPEDEF_INTEGER1(MIH_C_UIR);
TYPEDEF_INTEGER1(MIH_C_M);
TYPEDEF_INTEGER1(MIH_C_FN);
TYPEDEF_INTEGER1(MIH_C_SID);
TYPEDEF_INTEGER1(MIH_C_OPCODE);
TYPEDEF_INTEGER2(MIH_C_AID);
TYPEDEF_INTEGER2(MIH_C_TRANSACTION_ID);
TYPEDEF_INTEGER2(MIH_C_VARIABLE_PAYLOAD_LENGTH);
TYPEDEF_INTEGER1(MIH_C_TLV_TYPE);


/*! \struct  MIH_C_HEADER
* \brief Structure defining the header of messages exchanged between MIH entities.
*/
typedef struct MIH_C_HEADER {
    MIH_C_VERSION_T                 version;
    MIH_C_ACK_REQ_T                 ack_req;
    MIH_C_ACK_RSP_T                 ack_rsp;
    MIH_C_UIR_T                     uir;
    MIH_C_M_T                       more_fragment;
    MIH_C_FN_T                      fragment_number;
    MIH_C_SID_T                     service_identifier;
    MIH_C_OPCODE_T                  operation_code;
    MIH_C_AID_T                     action_identifier;
    MIH_C_TRANSACTION_ID_T          transaction_id;
    MIH_C_VARIABLE_PAYLOAD_LENGTH_T payload_length;
} MIH_C_HEADER_T;
//-----------------------------------------------------------------------------
public_header_codec(int MIH_C_HEADER2String(MIH_C_HEADER_T* headerP, char* bufP);)
public_header_codec(int MIH_C_Link_Header_Encode(Bit_Buffer_t* message_serializedP, MIH_C_HEADER_T* headerP);)
public_header_codec(int MIH_C_Link_Header_Decode(Bit_Buffer_t* message_serializedP, MIH_C_HEADER_T* headerP);)
//-----------------------------------------------------------------------------
#endif
