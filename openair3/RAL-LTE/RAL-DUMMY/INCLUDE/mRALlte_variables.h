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

#ifndef __RAL_LTE_VAR_H__
#define __RAL_LTE_VAR_H__
//-------------------------------------------------------------------

#include "rrc_d_types.h"
#include "commonDS.h"
#include "mRALlte_constants.h"
#include "MIH_C.h"

/***************************************************************************
     CONSTANTS
 ***************************************************************************/
#define RAL_DUMMY
//#define RAL_REALTIME
//#define MRALU_SIMU_LINKDOWN
//#define DEBUG_MRALU_MEASURES
//#define UPDATE


#define RALU_MT_EVENT_MAP MIH_EVENT_Link_AdvertisementIndication + MIH_EVENT_Link_Parameters_ReportIndication + MIH_EVENT_Link_UpIndication + MIH_EVENT_Link_DownIndication

#define MIHLink_MAX_LENGTH 500
#define MAX_NUMBER_BS			3  /* Max number of base stations */
#define CONF_UNKNOWN_CELL_ID 0
#define UMTS_INTF_NAME "graal0"

//Attachment status
#define UNKNOWN         9
#define CONNECTED	      1
#define DISCONNECTED    0
//max number RB supported  (27 in Spec)
#define RAL_MAX_RB 5

// constant for integrating measures according to the formula
// y[n] = (1-LAMBDA)*u[n-1] + LAMBDA*u[n]
#define LAMBDA 8
#define MEAS_MAX_RSSI 110

/***************************************************************************
     VARIABLES
 ***************************************************************************/
struct ral_lte_priv {
   // only to call ralu_verifyPendingConnection
   uint8_t pending_req_flag;
// network parameters
   uint16_t cell_id;
   uint16_t nas_state;
   int state;
   uint32_t curr_signal_level;
   uint32_t ipv6_l2id[2];
//measures
   uint8_t  req_num_bs;
   uint16_t req_cell_id[MAX_NUMBER_BS];
   uint32_t req_prov_id[MAX_NUMBER_BS];
   uint16_t req_order_index[MAX_NUMBER_BS];
   int num_measures;
   uint16_t meas_cell_id[MAX_NUMBER_BS];
   uint32_t last_meas_level[MAX_NUMBER_BS];
   uint32_t integrated_meas_level[MAX_NUMBER_BS];
   uint32_t prev_integrated_meas_level[MAX_NUMBER_BS];
   uint32_t provider_id[MAX_NUMBER_BS];
//Radio Bearers
   uint16_t num_rb;
   uint16_t rbId[RAL_MAX_RB];
   uint16_t QoSclass[RAL_MAX_RB];
   uint16_t dscp[RAL_MAX_RB];
// statistics
   uint32_t rx_packets;
   uint32_t tx_packets;
   uint32_t rx_bytes;
   uint32_t tx_bytes;
   uint32_t rx_errors;
   uint32_t tx_errors;
   uint32_t rx_dropped;
   uint32_t tx_dropped;
   char buffer[800];  // For ioctl with NAS driver

   // MIH-INTERFACE data
   // Initialised, then read-only, supported actions
   MIH_C_LINK_AC_TYPE_T            mih_supported_action_list;
   // action currently processed
   MIH_C_LINK_AC_TYPE_T            pending_req_action;
   // actions requested by MIH-H
   MIH_C_LINK_AC_TYPE_T            req_action_list;

   MIH_C_STATUS_T                  pending_req_status;
   MIH_C_LINK_AC_RESULT_T          pending_req_ac_result;
   MIH_C_TRANSACTION_ID_T          pending_req_transaction_id;
   // set unset bits by MIH_C_Message_Link_Event_Subscribe_request MIH_C_Message_Link_Event_Unsubscribe_request
   MIH_C_LINK_EVENT_LIST_T         mih_subscribe_req_event_list;
   // Initialised, then read-only
   MIH_C_LINK_EVENT_LIST_T         mih_supported_link_event_list;
   // Initialised, then read-only
   MIH_C_LINK_CMD_LIST_T           mih_supported_link_command_list;
   LIST(MIH_C_LINK_CFG_PARAM, mih_link_cfg_param_thresholds);
   // to tell what are the configured thresholds in mih_link_cfg_param_thresholds_list
   MIH_C_BOOLEAN_T                 active_mih_link_cfg_param_threshold[MIH_C_LINK_CFG_PARAM_LIST_LENGTH];
   MIH_C_BOOLEAN_T                 link_to_be_detected;
};

//-----------------------------------------------------------------------------
extern struct ral_lte_priv *ralpriv;

#endif

