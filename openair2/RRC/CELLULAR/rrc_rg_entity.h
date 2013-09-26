/***************************************************************************
                          rrc_rg_entity.h  -
                          -------------------
    begin                : Someday 2001
    copyright            : (C) 2001, 2010 by Eurecom
    created by           : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Data structure with RRC RG protocol parameters
 ***************************************************************************/

#ifndef __RRC_RG_ENTITY_H__
#define __RRC_RG_ENTITY_H__
//-----------------------------------------------------------------------------
#include "rrc_platform_types.h"
#include "rrc_constant.h"
#include "rrc_rg_bch_variables.h"
#include "rrc_rg_variables.h"
#include "rrc_rg_rrm_variables.h"
#include "rrc_rg_mbms_variables.h"
#include "rrc_rg_L2_intf_variables.h"
//-----------------------------------------------------------------------------
#include "rrm_config_structs.h"
// #include "rrc_rg_data.h"
// #include "rrc_rg_rrm_intf.h"

/******************************************************************************
*                        Protocol variables                                   *
******************************************************************************/
struct rrc_rg_entity {
//-----------------------------------------------------------------------------
  int current_SFN;

  u8  protocol_state[maxUsers];
  // Table to be changed into a list??
  struct Mobile_Node Mobile_List[maxUsers];
  u8  establishment_cause;  // For NAS - A Revoir
//  u8  release_cause;
  u32 next_MUI;     // for RLC AM

  //External missing components simulation (debug)
  int simulator;
  int sim_counter;

  // platform configuration
  RRM_VARS  saved_configuration;
  int curr_transaction_id;
  //rrc_md_qos_table md_qos_table;

  int rrc_currently_updating; // RRC is updating rb configuration
  int rrc_UE_updating;
  int rrc_multicast_bearer;
  u8  curr_config_action;
  int curr_rb_failure_code;

  // link to L1
  int rrc_rg_pending_UE;
  u8  rrc_rg_synch_ind;
  u8  rrc_rg_cctrch_synch[maxCCTrCH_rg];
  u8  rrc_rg_outsynch_ind;
  u8  rrc_rg_cctrch_outsynch[maxCCTrCH_rg];
  int rrc_rg_cctrch_crnti[maxCCTrCH_rg];

  /* ** OAI compatible part - begin **/
  // Control block for Broadcast asn1-compliant
  struct rrc_rg_bch_asn1 rg_bch_asn1;

  // Control block for srb-drb asn1-compliant
  struct rrc_srb_drb_asn1 rg_rb_asn1;
  int mod_id;
  int ccch_current_UE_Id; //incremented each time a new UE sends ConnReq through CCH
  /* ** OAI compatible part - end **/

  // Control block for Broadcast
  struct rrc_rg_bch_blocks rg_bch_blocks;
  int rg_broadcast_counter;

  // pointer to next message to transmit to NAS
  //   this is actually the start of a linked list
  mem_block_t  *NASMessageToXmit;
  int ip_rx_irq;
  // RG configuration
  u16 rg_cell_id;
  int num_cells;
  char ccch_buffer[100];
  int ccch_buffer_size;

  // RG measures
  struct rrc_rg_meas_blocks rg_meas_blocks;

  // Control block for MBMS
  struct rrc_rg_mbms_variables mbms;

  // RT-fifo descriptors
  int rrc_rg_GC_fifo;
  int rrc_rg_NT_fifo;
  int rrc_rg_DCIn_fifo[maxUsers];
  int rrc_rg_DCOut_fifo[maxUsers];

  // Control Block RRM interface
  struct rrc_rg_rrm_variables rc_rrm;
  //Added for Medieval demo 3 - MW
  int eNB_measures_flag;
  // hard-coded values for testing demo3 (set in main)
  int conf_rlcBufferOccupancy[maxUsers];
  int conf_scheduledPRB[maxUsers];
  int conf_totalDataVolume[maxUsers];
  // storage of last values measured
  int num_connected_UEs;
  int current_rlcBufferOccupancy[maxUsers];
  int current_scheduledPRB[maxUsers];
  int current_totalDataVolume[maxUsers];
  int current_totalNumPRBs;
};
#endif
