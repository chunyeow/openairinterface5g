/***************************************************************************
                          rrc_rg_variables.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Some additional definitions for RG variables
 ***************************************************************************/
#ifndef __RRC_RG_VARIABLES_H__
#define __RRC_RG_VARIABLES_H__

#include "rrc_platform_types.h"
#include "rrc_msg_class.h"
#include "rrc_mbms_constant.h"

struct rrc_rg_msg_infos {
  int  msg_Id;
  int  msg_length;
  void *msg_ptr;
  mem_block_t *mem_block_ptr;
};

struct rrc_rg_trans {
  int  msg_type;
  int  transaction_Id;
};

struct rrc_rg_rb_information {
  u16  rb_identity;
  u16  rb_started;
  u16  qos_class;
  u16  dscp_code;
  u16  sap_id;
  u8   rbsetup_status;
};

//Structure for measurements in UE
struct rrc_rg_mt_meas_cmd {
  u16  identity;
  u16  command;
  u16  xfer_mode;
  u16  trigger;
  u16  type;
  u16  if_coeff;
  u8   if_cellIdentity_ri;
  u8   if_timeslotISCP_ri;
  u8   if_BCH_RSCP_ri;
  u8   if_pathloss_ri;
  u16  if_validity;
  u16  tv_validity;
  u8   tv_payload_ri;
  u8   tv_average_ri;
  u8   tv_variance_ri;
  u16  q_dl_trch_bler_ri;
  u16  q_sir_TFCSid[MAXMEASTFCS];
  u16  int_quantity;
  u16  int_coeff;
  u8   int_rep_ue_TransmittedPower;
  u8   int_rep_appliedTA;
  u16  criteria_type;
  u16  rep_amount;
  u16  rep_interval;
};

struct rrc_rg_mt_meas_rep {
  u16 identity;
  u16 meas_results_type;
  u16 if_num_cells;
  u16 if_cell_id[maxCells];
  u16 if_cell_parms_id[maxCells];
  u16 if_BCH_RSCP[maxCells];
  u16 if_pathloss[maxCells];
  u16 if_slot_iscp[maxCells][maxTS];
  u16 tv_num_rbs;
  u16 tv_rbid[MAXMEASRBS];
  u16 tv_rb_payload[MAXMEASRBS];
  u16 tv_rb_average[MAXMEASRBS];
  u16 tv_rb_variance[MAXMEASRBS];
  u16 q_num_TrCH;
  u16 q_dl_TrCH_id[MAXMEASTRCH];
  u16 q_dl_TrCH_BLER[MAXMEASTRCH];
  u16 q_num_tfcs;
  u16 q_tfcs_id[MAXMEASTFCS];
  u16 q_sir[MAXMEASTFCS][maxTS];
  u16 int_xmit_power[maxTS];
  u16 int_timing_advance;
};

struct Mobile_Node {
  int mt_id;
  int state;
  char IMEI[14];
  int local_connection_ref;
  int release_cause;
  int u_rnti;
  u8 establishment_cause;
  u8 prot_error_indicator; // boolean
  int  conn_complete_timer;
  struct rrc_rg_msg_infos rg_msg_infos;
  struct rrc_rg_trans xmit_trans[MAXTRANS];
  //Added MSG2
  u8 am_RLC_ErrorIndicationRb2_3or4;
  u8 am_RLC_ErrorIndicationRb5orAbove;
  u8 cellUpdateCause;
  u16 ul_nas_message_lgth;
  u8  *ul_nas_message_ptr;
  u16 dl_nas_message_lgth;
  mem_block_t  *dl_nas_message_ptr;
  u16 paging_message_lgth;
  mem_block_t  *paging_message_ptr;
//     int  rb_id;
//     u8  qos_classes[MAXURAB];
//     u8  dscp_codes[MAXURAB];
  struct rrc_rg_rb_information rg_established_rbs[maxRB];
  u16 num_rb;
  int established_background;
  u16 requested_rbId;
  u16 requested_MTrbId;
  u16 requested_QoSclass;
  u16 requested_dscp;
  u16 requested_sapid;
// u8  rbsetup_status;
// Control blocks for measures
  int rrc_rg_last_measurement;
  struct rrc_rg_mt_meas_cmd rg_meas_cmd;
  struct rrc_rg_mt_meas_rep rg_meas_rep[MAXMEASTYPES];
// UE Capability Information
  u8 ind_accessStratumRelease;
  u8 ind_eurecomKernelRelease; //An adaptation for Eurecom TD-CDMA
// List of services for MBMS
  o3 act_serviceIdentity [maxMBMSServices]; /** OctetString(3), @see MBMS_ServiceIdentity */	
  u8 act_numService;
};

struct rrc_rg_nas_neighbor_list {
  char  sub_type;
  int  num_rg;
  char *ng_list;      // transparent to RRC
};

//Structure for measurements in BS
struct rrc_rg_bs_meas_cmd {
  u16 identity;
  u16 command;
  u16 UE_Id;
  u16 trigger;
  u16 type;
  u16 criteria_type;
  u16 rep_amount;
  u16 rep_interval;
};

struct rrc_rg_bs_meas_rep {
  int meas_counter;
  int meas_interval;
  int meas_start;
  u16 identity;
  u16 meas_results_type;
  u16 tv_num_rbs;
  u16 tv_rbid[MAXMEASRBS];
  u16 tv_rb_payload[MAXMEASRBS];
  u16 tv_rb_average[MAXMEASRBS];
  u16 tv_rb_variance[MAXMEASRBS];
  u16 q_num_TrCH;
  u16 q_dl_TrCH_id[MAXMEASTRCH];
  u16 q_dl_TrCH_BLER[MAXMEASTRCH];
  u16 q_num_tfcs;
  u16 q_tfcs_id[MAXMEASTFCS];
  u16 q_sir[MAXMEASTFCS][maxTS];
  s32 int_xmit_power[numANTENNAS][maxTS];
  s32 int_rscp[maxTS][MAXCH];
  s32 int_rssi_rf[numANTENNAS][maxTS];
  u16 int_spare[NUMSPARE];
};

  //BS Measures
struct rrc_rg_meas_blocks {
  struct rrc_rg_bs_meas_cmd bs_meas_cmd[2 * maxUsers + 1];
  struct rrc_rg_bs_meas_rep bs_meas_rep[2 * maxUsers + 1];
  int rrc_rg_bs_meas_next[2 * maxUsers + 1];
};

#endif
