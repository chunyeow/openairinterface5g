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
/*! \file mRALlte_mih_msg.h
 * \brief This file defines the prototypes of the functions for coding and decoding of MIH Link messages.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MRALLTE_MIH_MSG_H__
#    define __MRALLTE_MIH_MSG_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_MIH_MSG_C
#            define private_mRALlte_mih_msg(x)    x
#            define protected_mRALlte_mih_msg(x)  x
#            define public_mRALlte_mih_msg(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_mih_msg(x)
#                define protected_mRALlte_mih_msg(x)  extern x
#                define public_mRALlte_mih_msg(x)     extern x
#            else
#                define private_mRALlte_mih_msg(x)
#                define protected_mRALlte_mih_msg(x)
#                define public_mRALlte_mih_msg(x)     extern x
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
#include "mRALlte_constants.h"
#include "mRALlte_get.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#define MSG_CODEC_RECV_BUFFER_SIZE    16400
#define MSG_CODEC_SEND_BUFFER_SIZE    16400
//-----------------------------------------------------------------------------
protected_mRALlte_mih_msg(char*              g_mihf_remote_port;)
protected_mRALlte_mih_msg(char*              g_mihf_ip_address;)
protected_mRALlte_mih_msg(char*              g_ral_ip_address;)
protected_mRALlte_mih_msg(char*              g_ral_listening_port_for_mihf;)
protected_mRALlte_mih_msg(char*              g_link_id;)
protected_mRALlte_mih_msg(char*              g_mihf_id;)
protected_mRALlte_mih_msg(int                g_sockd_mihf;)
protected_mRALlte_mih_msg(char               g_msg_print_buffer[8192];)

//-----------------------------------------------------------------------------
protected_mRALlte_mih_msg(u_int8_t g_msg_codec_recv_buffer[MSG_CODEC_RECV_BUFFER_SIZE];)
protected_mRALlte_mih_msg(u_int8_t g_msg_codec_send_buffer[MSG_CODEC_SEND_BUFFER_SIZE];)
//-----------------------------------------------------------------------------
public_mRALlte_mih_msg(     int mRALlte_send_to_mih(u_int8_t  *bufferP, size_t lenP);)

protected_mRALlte_mih_msg(  void mRALlte_print_buffer                   (char * bufferP, int lengthP);)
protected_mRALlte_mih_msg(  int  mRALlte_mihf_connect                   (void);)
protected_mRALlte_mih_msg(  void mRALlte_send_link_register_indication  (MIH_C_TRANSACTION_ID_T       *transaction_idP);)
protected_mRALlte_mih_msg(  void mRALlte_send_link_detected_indication  (MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                                                              MIH_C_LINK_DET_INFO_T        *link_detected_infoP);)

protected_mRALlte_mih_msg(  void mRALlte_send_link_up_indication(MIH_C_TRANSACTION_ID_T    *transaction_idP,
                                                                      MIH_C_LINK_TUPLE_ID_T     *link_identifierP,
                                                                      MIH_C_LINK_ADDR_T         *old_access_routerP,
                                                                      MIH_C_LINK_ADDR_T         *new_access_routerP,
                                                                      MIH_C_IP_RENEWAL_FLAG_T   *ip_renewal_flagP,
                                                                      MIH_C_IP_MOB_MGMT_T       *mobility_management_supportP);)

protected_mRALlte_mih_msg(  void mRALlte_send_link_parameters_report_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                                                     MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                                                                     MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_listP);)

protected_mRALlte_mih_msg(  void mRALlte_send_link_going_down_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                                              MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                                                              MIH_C_UNSIGNED_INT2_T       *time_intervalP,
                                                                              MIH_C_LINK_GD_REASON_T      *link_going_down_reasonP);)

protected_mRALlte_mih_msg(  void mRALlte_send_link_down_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                                        MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                                                        MIH_C_LINK_ADDR_T           *old_access_routerP,
                                                                        MIH_C_LINK_DN_REASON_T      *reason_codeP);)

protected_mRALlte_mih_msg(  void mRALlte_send_link_action_confirm(MIH_C_TRANSACTION_ID_T     *transaction_idP,
                                                                       MIH_C_STATUS_T             *statusP,
                                                                       MIH_C_LINK_SCAN_RSP_LIST_T *scan_response_setP,
                                                                       MIH_C_LINK_AC_RESULT_T     *link_action_resultP);)

protected_mRALlte_mih_msg(  void mRALte_send_capability_discover_confirm(MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                                                              MIH_C_STATUS_T               *statusP,
                                                                              MIH_C_LINK_EVENT_LIST_T      *supported_link_event_listP,
                                                                              MIH_C_LINK_CMD_LIST_T        *supported_link_command_listP);)

protected_mRALlte_mih_msg(  void mRALte_send_event_subscribe_confirm    (MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                                                              MIH_C_STATUS_T               *statusP,
                                                                              MIH_C_LINK_EVENT_LIST_T      *response_link_event_listP);)

protected_mRALlte_mih_msg(  void mRALte_send_event_unsubscribe_confirm  (MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                                                              MIH_C_STATUS_T               *statusP,
                                                                              MIH_C_LINK_EVENT_LIST_T      *response_link_event_listP);)

protected_mRALlte_mih_msg(  void mRALte_send_configure_thresholds_confirm(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                                              MIH_C_STATUS_T               *statusP,
                                                                              MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_listP);)

protected_mRALlte_mih_msg(  void mRALte_send_get_parameters_confirm     (MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                                                              MIH_C_STATUS_T               *statusP,
                                                                              MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_listP,
                                                                              MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_listP,
                                                                              MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_listP);)

private_mRALlte_mih_msg(    int  mRALlte_mih_link_msg_decode            (Bit_Buffer_t* bbP, MIH_C_Message_Wrapper_t *message_wrapperP);)
protected_mRALlte_mih_msg(  int  mRALlte_mih_link_process_message       (void);)
#endif
