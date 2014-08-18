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
/*! \file eRALlte_mih_msg.h
 * \brief
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __RAL_LTE_MIH_MSG_H__
#define __RAL_LTE_MIH_MSG_H__

#include <sys/types.h>

#include "MIH_C_Types.h"
#include "MIH_C_header_codec.h"
#include "MIH_C_Link_Primitives.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

char* g_mihf_ip_address;
char* g_mihf_remote_port;
int   g_sockd_mihf;

char* g_ral_ip_address;
char* g_ral_listening_port_for_mihf;

char* g_link_id;
char* g_mihf_id;

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

int eRALlte_mihf_connect(void);

int eRALlte_mih_link_process_message(void);

/*
 * --------------------------------------------------------------------------
 * MIH service management messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_REGISTER_INDICATION_ID */
void eRALlte_send_link_register_indication(
	MIH_C_TRANSACTION_ID_T *transaction_idP);

/* MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM_ID */
void eRALlte_send_capability_discover_confirm(
	MIH_C_TRANSACTION_ID_T  *tidP,
	MIH_C_STATUS_T          *statusP,
	MIH_C_LINK_EVENT_LIST_T *link_evt_listP,
	MIH_C_LINK_CMD_LIST_T   *link_cmd_listP);

/* MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_CONFIRM_ID */
void eRALlte_send_event_subscribe_confirm(
	MIH_C_TRANSACTION_ID_T  *transaction_idP,
	MIH_C_STATUS_T          *statusP,
	MIH_C_LINK_EVENT_LIST_T *link_event_listP);

/* MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_CONFIRM_ID */
void eRALlte_send_event_unsubscribe_confirm(
	MIH_C_TRANSACTION_ID_T  *transaction_idP,
	MIH_C_STATUS_T          *statusP,
	MIH_C_LINK_EVENT_LIST_T *link_event_listP);

/*
 * --------------------------------------------------------------------------
 * MIH event messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_DETECTED_INDICATION_ID */
void eRALlte_send_link_detected_indication(
	MIH_C_TRANSACTION_ID_T  *transaction_idP,
	MIH_C_LINK_DET_INFO_T   *link_detected_infoP);

/* MIH_C_MESSAGE_LINK_UP_INDICATION_ID */
void eRALlte_send_link_up_indication(
	MIH_C_TRANSACTION_ID_T  *transaction_idP,
	MIH_C_LINK_TUPLE_ID_T   *link_identifierP,
	MIH_C_LINK_ADDR_T       *old_access_routerP,
	MIH_C_LINK_ADDR_T       *new_access_routerP,
	MIH_C_IP_RENEWAL_FLAG_T *ip_renewal_flagP,
	MIH_C_IP_MOB_MGMT_T     *mobility_mngtP);

/* MIH_C_MESSAGE_LINK_DOWN_INDICATION_ID */
void eRALlte_send_link_down_indication(
	MIH_C_TRANSACTION_ID_T      *transaction_idP,
	MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
	MIH_C_LINK_ADDR_T           *old_access_routerP,
	MIH_C_LINK_DN_REASON_T      *reason_codeP);

/* MIH_C_MESSAGE_LINK_PARAMETERS_REPORT_INDICATION_ID */
void eRALlte_send_link_parameters_report_indication(
	MIH_C_TRANSACTION_ID_T      *transaction_idP,
	MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
	MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_listP);

/* MIH_C_MESSAGE_LINK_GOING_DOWN_INDICATION_ID */
void eRALlte_send_link_going_down_indication(
	MIH_C_TRANSACTION_ID_T      *transaction_idP,
	MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
	MIH_C_UNSIGNED_INT2_T       *time_intervalP,
	MIH_C_LINK_GD_REASON_T      *link_going_down_reasonP);

/* MIH_C_MESSAGE_LINK_HANDOVER_IMMINENT_INDICATION_ID */

/* MIH_C_MESSAGE_LINK_HANDOVER_COMPLETE_INDICATION_ID */

/* MIH_C_MESSAGE_LINK_PDU_TRANSMIT_STATUS_INDICATION_ID */

/*
 * --------------------------------------------------------------------------
 * MIH command messages
 * --------------------------------------------------------------------------
 */

/* MIH_C_MESSAGE_LINK_GET_PARAMETERS_CONFIRM_ID */
void eRALlte_send_get_parameters_confirm(
	MIH_C_TRANSACTION_ID_T       *transaction_idP,
	MIH_C_STATUS_T               *statusP,
	MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_listP,
	MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_listP,
	MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_listP);

/* MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_CONFIRM_ID */
void eRALlte_send_configure_thresholds_confirm(
	MIH_C_TRANSACTION_ID_T       *transaction_idP,
	MIH_C_STATUS_T               *statusP,
	MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_listP);

/* MIH_C_MESSAGE_LINK_ACTION_CONFIRM_ID */
void eRALlte_send_link_action_confirm(
	MIH_C_TRANSACTION_ID_T      *transaction_idP,
	MIH_C_STATUS_T              *statusP,
	MIH_C_LINK_SCAN_RSP_LIST_T  *scan_response_setP,
	MIH_C_LINK_AC_RESULT_T      *link_action_resultP);

/*
 * --------------------------------------------------------------------------
 * MIH information messages
 * --------------------------------------------------------------------------
 */

/****************************************************************************/

#endif
