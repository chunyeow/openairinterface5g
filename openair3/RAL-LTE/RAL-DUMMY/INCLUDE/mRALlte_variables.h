/********************************************************************
                          mRALlte_variables.h  -
                          -------------------
    copyright            : (C) 2005 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
 ********************************************************************
  Data structure with Dummy A21_MT_RAL_UMTS / UMTS-TDD protocol parameters
 *******************************************************************/

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
   u8 pending_req_flag;
// network parameters
   u16 cell_id;
   u16 nas_state;
   int state;
   u32 curr_signal_level;
   u32 ipv6_l2id[2];
//measures
   u8  req_num_bs;
   u16 req_cell_id[MAX_NUMBER_BS];
   u32 req_prov_id[MAX_NUMBER_BS];
   u16 req_order_index[MAX_NUMBER_BS];
   int num_measures;
   u16 meas_cell_id[MAX_NUMBER_BS];
   u32 last_meas_level[MAX_NUMBER_BS];
   u32 integrated_meas_level[MAX_NUMBER_BS];
   u32 prev_integrated_meas_level[MAX_NUMBER_BS];
   u32 provider_id[MAX_NUMBER_BS];
//Radio Bearers
   u16 num_rb;
   u16 rbId[RAL_MAX_RB];
   u16 QoSclass[RAL_MAX_RB];
   u16 dscp[RAL_MAX_RB];
// statistics
   u32 rx_packets;
   u32 tx_packets;
   u32 rx_bytes;
   u32 tx_bytes;
   u32 rx_errors;
   u32 tx_errors;
   u32 rx_dropped;
   u32 tx_dropped;
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

