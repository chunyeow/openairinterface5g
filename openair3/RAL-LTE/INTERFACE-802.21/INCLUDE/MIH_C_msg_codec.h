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
/*! \file MIH_C_msg_codec.h
 * \brief This file defines the prototypes of the functions for coding and decoding of MIH Link messages.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_MSG_CODEC_H__
#    define __MIH_C_MSG_CODEC_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_MSG_CODEC_C
#            define private_mih_c_msg_codec(x)    x
#            define protected_mih_c_msg_codec(x)  x
#            define public_mih_c_msg_codec(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_mih_c_msg_codec(x)
#                define protected_mih_c_msg_codec(x)  extern x
#                define public_mih_c_msg_codec(x)     extern x
#            else
#                define private_mih_c_msg_codec(x)
#                define protected_mih_c_msg_codec(x)
#                define public_mih_c_msg_codec(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <libgen.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------
#define MIH_MESSAGE_DECODE_OK             0
#define MIH_MESSAGE_DECODE_TOO_SHORT     -1
#define MIH_MESSAGE_DECODE_FAILURE       -2
#define MIH_MESSAGE_DECODE_BAD_PARAMETER -3
//-----------------------------------------------------------------------------
#define MSG_MIH_HEADER_SIZE_IN_BYTES  8
//-----------------------------------------------------------------------------
typedef u_int16_t MIH_C_Message_Id_t;

typedef struct MIH_C_Message_Wrapper {
    MIH_C_Message_Id_t         message_id;
    union {
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
        MIH_C_Message_Link_Register_indication_t                   link_register_indication;
#endif
        MIH_C_Message_Link_Detected_indication_t                   link_detected_indication;
        MIH_C_Message_Link_Up_indication_t                         link_up_indication;
        MIH_C_Message_Link_Down_indication_t                       link_down_indication;
        MIH_C_Message_Link_Parameters_Report_indication_t          link_parameters_report_indication;
        MIH_C_Message_Link_Going_Down_indication_t                 link_going_down_indication;
        MIH_C_Message_Link_Handover_Imminent_indication_t          link_handover_imminent_indication;
        MIH_C_Message_Link_Handover_Complete_indication_t          link_handover_complete_indication;
        MIH_C_Message_Link_PDU_Transmit_Status_indication_t        link_pdu_transmit_status_indication;
        MIH_C_Message_Link_Capability_Discover_request_t           link_capability_discover_request;
        MIH_C_Message_Link_Capability_Discover_confirm_t           link_capability_discover_confirm;
        MIH_C_Message_Link_Event_Subscribe_request_t               link_event_subscribe_request;
        MIH_C_Message_Link_Event_Subscribe_confirm_t               link_event_subscribe_confirm;
        MIH_C_Message_Link_Event_Unsubscribe_request_t             link_event_unsubscribe_request;
        MIH_C_Message_Link_Event_Unsubscribe_confirm_t             link_event_unsubscribe_confirm;
        MIH_C_Message_Link_Get_Parameters_request_t                link_get_parameters_request;
        MIH_C_Message_Link_Get_Parameters_confirm_t                link_get_parameters_confirm;
        MIH_C_Message_Link_Configure_Thresholds_request_t          link_configure_thresholds_request;
        MIH_C_Message_Link_Configure_Thresholds_confirm_t          link_configure_thresholds_confirm;
        MIH_C_Message_Link_Action_request_t                        link_action_request;
        MIH_C_Message_Link_Action_confirm_t                        link_action_confirm;
    } _union_message;
} MIH_C_Message_Wrapper_t;
//-----------------------------------------------------------------------------
private_mih_c_msg_codec(MIH_C_TRANSACTION_ID_T g_transaction_id_generator;)
//-----------------------------------------------------------------------------
public_mih_c_msg_codec(  MIH_C_TRANSACTION_ID_T MIH_C_get_new_transaction_id(void);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Capability_Discover_request(  Bit_Buffer_t* bbP, MIH_C_Message_Link_Capability_Discover_request_t  *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Capability_Discover_request2String(  MIH_C_Message_Link_Capability_Discover_request_t *messageP, char* bufP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Event_Subscribe_request(      Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Subscribe_request_t      *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Event_Subscribe_request2String(      MIH_C_Message_Link_Event_Subscribe_request_t *messageP, char* bufP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Event_Unsubscribe_request(    Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Unsubscribe_request_t    *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Event_Unsubscribe_request2String(    MIH_C_Message_Link_Event_Unsubscribe_request_t *messageP, char* bufP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Get_Parameters_request(       Bit_Buffer_t* bbP, MIH_C_Message_Link_Get_Parameters_request_t       *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Get_Parameters_request2String(       MIH_C_Message_Link_Get_Parameters_request_t *messageP, char* bufP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Configure_Thresholds_request( Bit_Buffer_t* bbP, MIH_C_Message_Link_Configure_Thresholds_request_t *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Configure_Thresholds_request2String( MIH_C_Message_Link_Configure_Thresholds_request_t *messageP, char* bufP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Decode_Link_Action_request(               Bit_Buffer_t* bbP, MIH_C_Message_Link_Action_request_t               *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Link_Action_request2String(               MIH_C_Message_Link_Action_request_t *messageP, char* bufP);)

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Register_indication(          Bit_Buffer_t* bbP, MIH_C_Message_Link_Register_indication_t          *messageP);)
#endif
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Detected_indication(          Bit_Buffer_t* bbP, MIH_C_Message_Link_Detected_indication_t          *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Up_indication(                Bit_Buffer_t* bbP, MIH_C_Message_Link_Up_indication_t                *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Parameters_Report_indication( Bit_Buffer_t* bbP, MIH_C_Message_Link_Parameters_Report_indication_t *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Going_Down_indication(        Bit_Buffer_t* bbP, MIH_C_Message_Link_Going_Down_indication_t        *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Down_indication(              Bit_Buffer_t* bbP, MIH_C_Message_Link_Down_indication_t              *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Link_Action_confirm(               Bit_Buffer_t* bbP, MIH_C_Message_Link_Action_confirm_t               *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Capability_Discover_confirm(       Bit_Buffer_t* bbP, MIH_C_Message_Link_Capability_Discover_confirm_t  *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Event_Subscribe_confirm(           Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Subscribe_confirm_t      *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Event_Unsubscribe_confirm(         Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Unsubscribe_confirm_t    *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Configure_Thresholds_confirm(      Bit_Buffer_t* bbP, MIH_C_Message_Link_Configure_Thresholds_confirm_t *messageP);)
public_mih_c_msg_codec(  int MIH_C_Link_Message_Encode_Get_Parameters_confirm(            Bit_Buffer_t* bbP, MIH_C_Message_Link_Get_Parameters_confirm_t       *messageP);)
#endif
