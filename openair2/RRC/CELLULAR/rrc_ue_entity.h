/********************************************************************
                          rrc_ue_entity.h  -
                          -------------------
    copyright            : (C) 2001, 2010 by Eurecom
    created by		 : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 ********************************************************************
  Data structure with RRC UE protocol parameters
 *******************************************************************/

#ifndef __RRC_UE_ENTITY_H__
#define __RRC_UE_ENTITY_H__
//-------------------------------------------------------------------
#include "rrc_platform_types.h"
#include "rrc_constant.h"
#include "rrc_ue_bch_variables.h"
#include "rrm_config_structs.h"
#include "rrc_ue_variables.h"
#include "rrc_ue_mbms_variables.h"
#include "rrc_ue_L2_intf_variables.h"

/********************************************************************
*                        Protocol variables                         *
********************************************************************/

struct rrc_ue_entity {
//-------------------------------------------------------------------
// implementation variables
  int protocol_state;       //values in rrc_constant.h
  int ue_initial_id;
  u32 local_connection_ref;
  u16 cell_id;
  int current_SFN;

  struct rrc_neighbor_cells rg_ngbr_list;
  char IMEI[14];
  u8  establishment_cause;
  u8  release_cause;
//  u8  failure_cause;        // not mw
//  u8  failure_indicator;    // boolean - not mw
  u32 next_MUI;     // for RLC AM
  int idata_xfer;   // flag for Initial data transfer

  int last_message_sent;
//Added MSG1
  u8  prot_error_indicator; // boolean
  u16 nextActivationTime;
  int next_state;
  struct rrc_ue_msg_infos ue_msg_infos;
  struct rrc_ue_trans accepted_trans[MAXTRANS];
  struct rrc_ue_trans rejected_trans[MAXTRANS];
  struct rrc_ue_trans rcved_trans;      //transaction id received in last message
  u16 c_rnti;
  u16 u_rnti;
//Added MSG2
  u8  am_RLC_ErrorIndicationRb2_3or4;
  u8  am_RLC_ErrorIndicationRb5orAbove;
  u8  rlc_Re_establishIndicatorRb2_3or4;
  u8  rlc_Re_establishIndicatorRb5orAbove;
  u8  cellUpdateCause;
  u8  rejectWaitTime;
  u16 ul_nas_message_lgth;
  mem_block_t *ul_nas_message_ptr;
  u16 dl_nas_message_lgth;
  u8 *dl_nas_message_ptr;
  u16 paging_message_lgth;
  u8 *paging_message_ptr;
  struct rrc_ue_rb_information ue_established_rbs[maxRB];
  u16 num_rb;
//   u8  qos_classes[MAXURAB];
//   u8  dscp_codes[MAXURAB];
  u16 requested_rbId;
  u16 requested_QoSclass;
  u16 requested_dscp;
  u16 requested_sapid;

// platform configuration
  int rrc_currently_updating;
  RRM_VARS saved_configuration;

// link to L1
  u8  rrc_ue_synch_ind;
  u8  rrc_ue_cctrch_synch[maxCCTrCH];
  u8  rrc_ue_cctrch[maxCCTrCH];
  u8  rrc_ue_outsynch_ind;
  u8  rrc_ue_cctrch_outsynch[maxCCTrCH];

  /* ** OAI compatible part - begin **/
  // Control block for srb-drb asn1-compliant
  struct rrc_srb_drb_asn1 ue_rb_asn1;

  // Control block for Broadcast asn1-compliant
  struct rrc_ue_bch_asn1 ue_bch_asn1;
  int mod_id;
  /* ** OAI compatible part - end **/
 
// Control block for Broadcast
  struct rrc_ue_bch_blocks ue_bch_blocks;

// Control blocks for measures
  int rrc_ue_last_measurement;
  int rrc_ue_meas_to_activate;
  struct rrc_ue_meas_cmd ue_meas_cmd[MAXMEASTYPES];
  struct rrc_ue_meas_rep ue_meas_rep[MAXMEASTYPES];

// Control block for MBMS
  rrc_ue_mbms_variables mbms;

// pointer to next message to transmit to NAS
  //   this is actually the start of a linked list
  mem_block_t *NASMessageToXmit;
  int ip_rx_irq;

// RT-fifo descriptors
  int rrc_ue_GC_fifo;
  int rrc_ue_NT_fifo;
  int rrc_ue_DCIn_fifo;
  int rrc_ue_DCOut_fifo;

//internal parameters
  unsigned int ue_broadcast_counter; // temp for sending broadcast to NAS
  int ue_wait_establish_req;  // possible values = 0 (init), 1 (SIB5 OK), 2 (SIB5 Not OK, SIB1 OK)
  int L2_ccch_status;  // possible values = 0 (OFF), 1 (IN_PROGRESS), 2 (ON)
  char ccch_buffer[100];
  int ccch_buffer_size;
  int rrc_ue_t300_target;
  int rrc_ue_t300_retry;
  int rrc_ue_ackSimu_flag;
  unsigned int rrc_ue_ackSimu_mui;
  unsigned int rrc_ue_ackSimu_srbid;
  //list2_t rrc_timers;
};

#endif
