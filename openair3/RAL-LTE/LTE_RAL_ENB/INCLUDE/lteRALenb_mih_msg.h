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
/*! \file lteRALenb_mih_msg.h
 * \brief
 * \author GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __LTE_RAL_ENB_MIH_MSG_H__
#define __LTE_RAL_ENB_MIH_MSG_H__
//-----------------------------------------------------------------------------
#        ifdef LTE_RAL_ENB_MIH_MSG_C
#            define private_lteralenb_mih_msg(x)    x
#            define protected_lteralenb_mih_msg(x)  x
#            define public_lteralenb_mih_msg(x)     x
#        else
#            ifdef LTE_RAL_ENB
#                define private_lteralenb_mih_msg(x)
#                define protected_lteralenb_mih_msg(x)  extern x
#                define public_lteralenb_mih_msg(x)     extern x
#            else
#                define private_lteralenb_mih_msg(x)
#                define protected_lteralenb_mih_msg(x)
#                define public_lteralenb_mih_msg(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "lteRALenb.h"

#include "MIH_C_Types.h"
#include "MIH_C_header_codec.h"
#include "MIH_C_Link_Primitives.h"

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/
protected_lteralenb_mih_msg(void eRAL_MIH_C_3GPP_ADDR_load_3gpp_str_address(ral_enb_instance_t instanceP, MIH_C_3GPP_ADDR_T* _3gpp_addr_pP, u_int8_t* str_pP));

protected_lteralenb_mih_msg(int eRAL_mihf_connect(ral_enb_instance_t instanceP);)

protected_lteralenb_mih_msg(int eRAL_mih_link_process_message(ral_enb_instance_t instanceP);)
private_lteralenb_mih_msg(void eRAL_print_buffer(const u_int8_t * bufferP, int lenP);)
/*
 * --------------------------------------------------------------------------
 * MIH service management messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_REGISTER_INDICATION_ID */
protected_lteralenb_mih_msg(void eRAL_send_link_register_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T *transaction_idP);)

/* MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM_ID */
private_lteralenb_mih_msg(void eRAL_send_capability_discover_confirm(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T  *tidP,\
        MIH_C_STATUS_T          *statusP,\
        MIH_C_LINK_EVENT_LIST_T *link_evt_listP,\
        MIH_C_LINK_CMD_LIST_T   *link_cmd_listP);)

/* MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_CONFIRM_ID */
protected_lteralenb_mih_msg(void eRAL_send_event_subscribe_confirm(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T  *transaction_idP,\
        MIH_C_STATUS_T          *statusP,\
        MIH_C_LINK_EVENT_LIST_T *link_event_listP);)

/* MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_CONFIRM_ID */
protected_lteralenb_mih_msg(void eRAL_send_event_unsubscribe_confirm(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T  *transaction_idP,\
        MIH_C_STATUS_T          *statusP,\
        MIH_C_LINK_EVENT_LIST_T *link_event_listP);)

/*
 * --------------------------------------------------------------------------
 * MIH event messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_DETECTED_INDICATION_ID */
protected_lteralenb_mih_msg(void eRAL_send_link_detected_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T  *transaction_idP,\
        MIH_C_LINK_DET_INFO_T   *link_detected_infoP);)

/* MIH_C_MESSAGE_LINK_UP_INDICATION_ID */
protected_lteralenb_mih_msg(void eRAL_send_link_up_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T  *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T   *link_identifierP,\
        MIH_C_LINK_ADDR_T       *old_access_routerP,\
        MIH_C_LINK_ADDR_T       *new_access_routerP,\
        MIH_C_IP_RENEWAL_FLAG_T *ip_renewal_flagP,\
        MIH_C_IP_MOB_MGMT_T     *mobility_mngtP);)

/* MIH_C_MESSAGE_LINK_DOWN_INDICATION_ID */
private_lteralenb_mih_msg(void eRAL_send_link_down_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_LINK_ADDR_T           *old_access_routerP,\
        MIH_C_LINK_DN_REASON_T      *reason_codeP);)

/* MIH_C_MESSAGE_LINK_PARAMETERS_REPORT_INDICATION_ID */
protected_lteralenb_mih_msg(void eRAL_send_link_parameters_report_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_listP);)

/* MIH_C_MESSAGE_LINK_GOING_DOWN_INDICATION_ID */
private_lteralenb_mih_msg(void eRAL_send_link_going_down_indication(\
        ral_enb_instance_t instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_UNSIGNED_INT2_T       *time_intervalP,\
        MIH_C_LINK_GD_REASON_T      *link_going_down_reasonP);)

/* MIH_C_MESSAGE_LINK_HANDOVER_IMMINENT_INDICATION_ID */

/* MIH_C_MESSAGE_LINK_HANDOVER_COMPLETE_INDICATION_ID */

/* MIH_C_MESSAGE_LINK_PDU_TRANSMIT_STATUS_INDICATION_ID */

/*
 * --------------------------------------------------------------------------
 * MIH command messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_GET_PARAMETERS_CONFIRM_ID */
protected_lteralenb_mih_msg(void eRAL_send_get_parameters_confirm(\
        ral_enb_instance_t instanceP,
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_listP,\
        MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_listP,\
        MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_listP);)

/* MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_CONFIRM_ID */
protected_lteralenb_mih_msg(void eRAL_send_configure_thresholds_confirm(\
        ral_enb_instance_t instanceP,
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_listP);)

/* MIH_C_MESSAGE_LINK_ACTION_CONFIRM_ID */
protected_lteralenb_mih_msg(void eRAL_send_link_action_confirm(\
        ral_enb_instance_t instanceP,
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_STATUS_T              *statusP,\
        MIH_C_LINK_SCAN_RSP_LIST_T  *scan_response_setP,\
        MIH_C_LINK_AC_RESULT_T      *link_action_resultP);)

/*
 * --------------------------------------------------------------------------
 * MIH information messages
 * --------------------------------------------------------------------------
 */
protected_lteralenb_mih_msg(int eRAL_mihf_connect       (ral_enb_instance_t instanceP);)
private_lteralenb_mih_msg(  int eRAL_send_to_mih        (ral_enb_instance_t instanceP, const u_int8_t *bufferP, int lenP);)
private_lteralenb_mih_msg(  int eRAL_mih_link_msg_decode(ral_enb_instance_t instanceP, Bit_Buffer_t* bbP, MIH_C_Message_Wrapper_t *message_wrapperP);)
/****************************************************************************/

#endif
