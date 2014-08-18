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
/*! \file lteRALue_mih_msg.h
 * \brief This file defines the prototypes of the functions for lteRALue_mih_msg.c
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __LTERALUE_MIH_MSG_H__
#    define __LTERALUE_MIH_MSG_H__
//-----------------------------------------------------------------------------
#        ifdef LTERALUE_MIH_MSG_C
#            define private_mRAL_mih_msg(x)    x
#            define protected_mRAL_mih_msg(x)  x
#            define public_mRAL_mih_msg(x)     x
#        else
#            ifdef LTE_RAL_UE
#                define private_mRAL_mih_msg(x)
#                define protected_mRAL_mih_msg(x)  extern x
#                define public_mRAL_mih_msg(x)     extern x
#            else
#                define private_mRAL_mih_msg(x)
#                define protected_mRAL_mih_msg(x)
#                define public_mRAL_mih_msg(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "lteRALue.h"

#define MSG_CODEC_RECV_BUFFER_SIZE    16400
#define MSG_CODEC_SEND_BUFFER_SIZE    16400
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
public_mRAL_mih_msg(     int mRAL_send_to_mih(ral_ue_instance_t instanceP, u_int8_t  *bufferP, size_t lenP);)

protected_mRAL_mih_msg(  int  mRAL_mihf_connect                   (ral_ue_instance_t instanceP);)

protected_mRAL_mih_msg(  void MIH_C_3GPP_ADDR_load_3gpp_str_address(ral_ue_instance_t instanceP, \
        MIH_C_3GPP_ADDR_T* _3gpp_addrP, \
        u_int8_t* strP);)

protected_mRAL_mih_msg(  void mRAL_send_link_register_indication  (\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP);)

protected_mRAL_mih_msg(  void mRAL_send_link_detected_indication  (\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_LINK_DET_INFO_T        *link_detected_infoP);)

protected_mRAL_mih_msg(  void mRAL_send_link_up_indication(\
        ral_ue_instance_t          instanceP,\
        MIH_C_TRANSACTION_ID_T    *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T     *link_identifierP,\
        MIH_C_LINK_ADDR_T         *old_access_routerP,\
        MIH_C_LINK_ADDR_T         *new_access_routerP,\
        MIH_C_IP_RENEWAL_FLAG_T   *ip_renewal_flagP,\
        MIH_C_IP_MOB_MGMT_T       *mobility_management_supportP);)

protected_mRAL_mih_msg(  void mRAL_send_link_parameters_report_indication(\
        ral_ue_instance_t            instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_listP);)

protected_mRAL_mih_msg(  void mRAL_send_link_going_down_indication(\
        ral_ue_instance_t            instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_UNSIGNED_INT2_T       *time_intervalP,\
        MIH_C_LINK_GD_REASON_T      *link_going_down_reasonP);)

protected_mRAL_mih_msg(  void mRAL_send_link_down_indication(\
        ral_ue_instance_t            instanceP,\
        MIH_C_TRANSACTION_ID_T      *transaction_idP,\
        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,\
        MIH_C_LINK_ADDR_T           *old_access_routerP,\
        MIH_C_LINK_DN_REASON_T      *reason_codeP);)

protected_mRAL_mih_msg(  void mRAL_send_link_action_confirm(\
        ral_ue_instance_t           instanceP,\
        MIH_C_TRANSACTION_ID_T     *transaction_idP,\
        MIH_C_STATUS_T             *statusP,\
        MIH_C_LINK_SCAN_RSP_LIST_T *scan_response_setP,\
        MIH_C_LINK_AC_RESULT_T     *link_action_resultP);)

protected_mRAL_mih_msg(  void mRAL_send_capability_discover_confirm(\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_EVENT_LIST_T      *supported_link_event_listP,\
        MIH_C_LINK_CMD_LIST_T        *supported_link_command_listP);)

protected_mRAL_mih_msg(  void mRAL_send_event_subscribe_confirm    (\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_EVENT_LIST_T      *response_link_event_listP);)

protected_mRAL_mih_msg(  void mRAL_send_event_unsubscribe_confirm  (\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_EVENT_LIST_T      *response_link_event_listP);)

protected_mRAL_mih_msg(  void mRAL_send_configure_thresholds_confirm(\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_listP);)

protected_mRAL_mih_msg(  void mRAL_send_get_parameters_confirm     (\
        ral_ue_instance_t             instanceP,\
        MIH_C_TRANSACTION_ID_T       *transaction_idP,\
        MIH_C_STATUS_T               *statusP,\
        MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_listP,\
        MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_listP,\
        MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_listP);)

private_mRAL_mih_msg(    int  mRAL_mih_link_msg_decode            (\
        ral_ue_instance_t instanceP,\
        Bit_Buffer_t* bbP, MIH_C_Message_Wrapper_t *message_wrapperP);)

protected_mRAL_mih_msg(  int  mRAL_mih_link_process_message       (ral_ue_instance_t instanceP);)
#endif
