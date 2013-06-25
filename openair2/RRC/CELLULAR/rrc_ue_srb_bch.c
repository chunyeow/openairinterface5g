/***************************************************************************
                          rrc_ue_srb_bch.c  -  description
                             -------------------
    begin             : Someday 2001
    copyright         : (C) 2001, 2010 by Eurecom
    created by	      : Lionel.Gauthier@eurecom.fr
    modified by       : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
 Handling of Signalling Radio Bearers
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_nas_sap.h"
#include "rrc_messages.h"

#include "rrc_proto_bch.h"
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------
// #include "rrc_contrib_l1_console_proto_extern.h"
// #include "rrc_pdcp_control_proto_extern.h"
// #include "rrc_rlc_control_proto_extern.h"
// #include "lists_proto_extern.h"
// #include "umts_sched_struct.h"

//-----------------------------------------------------------------------------
void rrc_broadcast_rx (char *Sdu){
//-----------------------------------------------------------------------------
  char data_size;

  if ((protocol_ms->rrc.current_SFN & 0x0001) == 0) {
    data_size = Sdu[0];
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
    rrc_print_buffer (Sdu, data_size);
    #endif
    memcpy ((char *) protocol_ms->rrc.ue_bch_blocks.encoded_currSIBCH, &Sdu[0], data_size);

    // decode
    rrc_ue_read_next_segment ();
  }
}

//-----------------------------------------------------------------------------
void *rrc_mt_ccch_rx_idle_mode (void *unusedP, mem_block_t * data_indicationP, int rb_idP){
//-----------------------------------------------------------------------------
  u16 tb_size_in_bytes;

  //tb_size_in_bytes  = (((struct rlc_indication*)(data_indicationP->data))->primitive.um_ind.data_size + 7) >> 3;
//   tb_size_in_bytes = ((struct rlc_indication *) (data_indicationP->data))->primitive.um_ind.data_size;

  msg ("[SRB0] RX %d bytes\n", tb_size_in_bytes);
//   rrc_ue_srb0_decode (&(data_indicationP->data[sizeof (struct rlc_indication)]), tb_size_in_bytes);
  return NULL;
}

// //-----------------------------------------------------------------------------
// void *rrc_mt_mcch_rx (void *unusedP, mem_block_t * data_indicationP, int rb_idP){
// //-----------------------------------------------------------------------------
//   u16 tb_size_in_bytes;
// //   tb_size_in_bytes = (((struct rlc_indication *) (data_indicationP->data))->primitive.um_ind.data_size + 7) >> 3;
// 
//   //msg ("[MCCH]  RX %d bytes\n", tb_size_in_bytes);
//   //msg ("[MCCH]  RX String : %s\n", &(data_indicationP->data[sizeof (struct rlc_indication)]));
// //   rrc_ue_mcch_decode (data_indicationP, sizeof (struct rlc_indication));
//   free_mem_block (data_indicationP);
//   return NULL;
// }

/*
//-----------------------------------------------------------------------------
void *rrc_srb_rx (void *unusedP, mem_block_t * sduP, u8 rb_idP){
//-----------------------------------------------------------------------------
  int srb_id;
  int UE_Id;

  srb_id = rb_idP % maxRB;
  UE_Id = (rb_idP - srb_id) / maxRB;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-SRB-UE] RB%d RX in frame %d\n", rb_idP, protocol_ms->rrc.current_SFN);
  msg ("[RRC-SRB-UE] SRB%d received, UE_Id %d\n", srb_id, UE_Id);
  //  msg ("[RRC-SRB] UE SRB%d RX : %hx , in frame %d\n", srb_id, &sduP->data[sizeof (struct rlc_indication)], frame);
  #endif

  switch (srb_id) {
    case RRC_SRB0_ID:
//       rrc_ue_srb0_decode (sduP, sizeof (struct rlc_indication));
//       break;
    case RRC_SRB1_ID:
//       rrc_ue_srb1_decode (sduP, sizeof (struct rlc_indication));
//       break;
    case RRC_SRB2_ID:
//       rrc_ue_srb2_decode (sduP, sizeof (struct rlc_indication));
//       break;
    case RRC_SRB3_ID:
//       rrc_ue_srb3_decode (sduP, sizeof (struct rlc_indication));
//       break;
    default:
      msg ("[RRC-SRB-UE] Invalid call to srb Receive\n");
      break;
  }

  free_mem_block (sduP);
  return NULL;
}
*/

//-----------------------------------------------------------------------------
int rrc_ue_srb_rx (char* sduP, int srb_id,  int UE_Id){
//-----------------------------------------------------------------------------
  //int srb_id, rb_id;
  //int UE_Id;
  int sdu_offset=0;

  #ifdef DEBUG_RRC_DETAILS
  //msg ("\n[RRC][SRB-UE] CALL to rrc_ue_srb_rx\n");
  #endif
  //rb_id = ch_idP - RRC_LCHAN_SRB0_ID;
  //rb_id = ch_idP;
  //srb_id = rb_id % maxRB;
  //UE_Id = (rb_id - srb_id) / maxRB;

  #ifdef DEBUG_RRC_STATE
  if (srb_id != RRC_MCCH_ID)
    msg ("[RRC][SRB-UE] SRB%d for UE %d RX in frame %d\n", srb_id, UE_Id, protocol_ms->rrc.current_SFN);
  //msg ("[RRC][SRB-UE] RB %d, SRB%d received [SRB5=MCCH], UE_Id %d\n", rb_id, srb_id, UE_Id);
  //msg ("[RRC-SRB-UE] [UE-DUMMIES] frame received: %s\n", (char*)&sduP[sdu_offset]);
  #endif

  switch (srb_id) {
    case RRC_SRB0_ID:
//       rrc_ue_srb0_decode (sduP, sizeof (struct rlc_indication));
      rrc_ue_srb0_decode (sduP, sdu_offset);
      break;
    case RRC_SRB1_ID:
//       rrc_ue_srb1_decode (sduP, sizeof (struct rlc_indication));
      rrc_ue_srb1_decode (sduP, sdu_offset);
      break;
    case RRC_SRB2_ID:
//       rrc_ue_srb2_decode (sduP, sizeof (struct rlc_indication));
      rrc_ue_srb2_decode (sduP, sdu_offset);
      break;
    case RRC_SRB3_ID:
//       rrc_ue_srb3_decode (sduP, sizeof (struct rlc_indication));
      rrc_ue_srb3_decode (sduP, sdu_offset);
      break;
    case RRC_MCCH_ID:
      //rrc_ue_mcch_decode (data_indicationP, sizeof (struct rlc_indication));
      rrc_ue_mcch_decode (sduP, sdu_offset);
      break;
    default:
      msg ("[RRC][SRB-UE] Invalid call to rrc_ue_srb_rx, SRB Receive\n");
      break;
  }

  return 0;
}


//-----------------------------------------------------------------------------
void* rrc_ue_srb_confirm (u32 muiP, u8 rb_idP, u8 statusP){
//-----------------------------------------------------------------------------
  int srb_id;
  int UE_Id;

  srb_id = rb_idP % maxRB;
  UE_Id = (rb_idP - srb_id) / maxRB;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-SRB-UE] RB%d RX SDU CONFIRM MUI %d, status %d\n", rb_idP, muiP, statusP);
  msg ("[RRC-SRB-UE] SRB_Id %d UE_Id %d\n", srb_id, UE_Id);
  #endif
  /* Temp OpenAir
  if (srb_id == RRC_SRB2_ID) {
    if (statusP == RLC_TX_CONFIRM_SUCCESSFULL) {
      rrc_ue_fsm_control (RRC_RLC_SUCCESS);
    } else {
      rrc_ue_fsm_control (RRC_RLC_FAILURE);
    }
  }
  */
  rrc_ue_fsm_control (RRC_RLC_SUCCESS);
  return NULL;
}
