/***************************************************************************
                          rrc_ue_variables.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Some additional definitions for UE variables
 ***************************************************************************/
#ifndef __RRC_UE_VARIABLES_H__
#define __RRC_UE_VARIABLES_H__

#include "rrc_msg_class.h"

struct rrc_ue_msg_infos {
  int msg_Id;
  int msg_length;
  void        *msg_ptr;
  mem_block_t *mem_block_ptr;
};

struct rrc_ue_rb_information {
  u16 rb_identity;
  u16 rb_started;
  u16 qos_class;
  u16 dscp_code;
  u16 sap_id;
};

struct rrc_ue_trans {
  int msg_type;
  int transaction_Id;
};

struct rrc_cell_info {
  u16 cell_id;
  int pccpch_code_group;
};

struct rrc_neighbor_cells {
  int num_cells;
  struct rrc_cell_info cell_info[maxCells];
};

struct rrc_ue_nas_neighbor_list {
  char  sub_type;
  int   num_rg;
  char *ng_list;      // transparent to RRC
};

struct rrc_ue_meas_cmd {
  u16 identity;
  u16 command;
  u16 xfer_mode;
  u16 trigger;
  u16 type;
  u16 if_coeff;
  u8  if_cellIdentity_ri;
  u8  if_timeslotISCP_ri;
  u8  if_BCH_RSCP_ri;
  u8  if_pathloss_ri;
  u16 if_validity;
  u16 tv_validity;
  u8  tv_payload_ri;
  u8  tv_average_ri;
  u8  tv_variance_ri;
  u16 q_dl_trch_bler_ri;
  u16 q_num_tfcs;
  u16 q_sir_TFCSid[MAXMEASTFCS];
  u16 int_quantity;
  u16 int_coeff;
  u8  int_rep_ue_TransmittedPower;
  u8  int_rep_appliedTA;
  u16 criteria_type;
  u16 rep_amount;
  u16 rep_interval;
};

struct rrc_ue_meas_rep {
  int meas_counter;
  int meas_interval;
  int meas_start;
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


#endif
