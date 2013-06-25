/***************************************************************************
                          rrc_rg_srb_bch.c  -  description
                             -------------------
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
#include "rrc_rg_vars.h"
#include "rrc_nas_sap.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_bch.h"
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int rrc_broadcast_tx (char *buffer){
//-----------------------------------------------------------------------------
  int retcode;

  if ((protocol_bs->rrc.current_SFN & 0x0001) == 0) {
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
      msg("[RRC] RG BROADCAST PREPARE SIB \n");
    #endif
    rrc_rg_prepare_next_segment();
    // send it
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
      msg("[RRC] RG BROADCAST TRANSMIT SIB \n");
    #endif
    memcpy (buffer, (char *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIBCH, maxSI);
    // send buffer on BCCH - As of 08/07/2010, this call is actually a NOP, since 
    // broadcast is actually retrieved through the rrc_L2_data_req_rx function
    retcode = rrc_rg_send_to_srb_rlc (0, RRC_BCCH_ID, buffer, maxSI);
    // free temp memory
    switch (protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type) {
      case BCH_noSegment:
        break;
      case BCH_completeSIB:
      case BCH_firstSegment:
      case BCH_subsequentSegment:
      case BCH_lastSegment:
        free_mem_block (protocol_bs->rrc.rg_bch_blocks.tSegment);
        break;
      default:
        msg("[RRC] rrc_broadcast_tx : unknown payload type %d\n", protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type);
        break;
    }
  }
  return maxSI;
}

//-----------------------------------------------------------------------------
void rrc_rg_mcch_tx (u8 * dataP, u16 lengthP){
//-----------------------------------------------------------------------------
  int  retcode, UE_Id =0;

  //TEMP - UE_Id hard coded because MCCH is actually a common channel
  if (lengthP > 0) {
    retcode = rrc_rg_send_to_srb_rlc (UE_Id, RRC_MCCH_ID, dataP, lengthP);
    #ifdef DEBUG_RRC_MBMS
      msg ("[RRC-RG][MBMS][FSM-OUT] Message %s sent on MCCH, ret code= %d.\n", dataP, retcode);
    #endif
  }
}

//-----------------------------------------------------------------------------
int rrc_rg_srb_rx (char* sduP, int srb_id,  int UE_Id){
//-----------------------------------------------------------------------------
  //int srb_id, rb_id = 0;
  //int UE_Id;
  int sdu_offset = 0; // not used anymore

  //rb_id = ch_idP;
  //srb_id = rb_id % maxRB;
  //UE_Id = (rb_id - srb_id) / maxRB;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][SRB-RG] SRB%d for UE %d RX in frame %d\n", srb_id, UE_Id, protocol_bs->rrc.current_SFN);
  #endif

    #ifdef DEBUG_RRC_STATE
    //msg ("[RRC][SRB-RG] RB %d, SRB%d received, UE_Id %d\n", rb_id, srb_id, UE_Id);
    //msg ("[RRC][SRB-RG] frame received: %s\n", (char*)&sduP[sdu_offset]);
    #endif
    switch (srb_id) {
      case RRC_SRB0_ID:
        //rrc_rg_srb0_decode (Rcved_sdu, sizeof (struct rlc_indication));
        rrc_rg_srb0_decode (sduP, sdu_offset);
       break;
      case RRC_SRB1_ID:
        //rrc_rg_srb1_decode (UE_Id, sduP, sizeof (struct rlc_indication));
        rrc_rg_srb1_decode (UE_Id, sduP, sdu_offset);
        break;
      case RRC_SRB2_ID:
        //rrc_rg_srb2_decode (UE_Id, sduP, sizeof (struct rlc_indication));
        rrc_rg_srb2_decode (UE_Id, sduP, sdu_offset);
        break;
      case RRC_SRB3_ID:
        //rrc_rg_srb3_decode (UE_Id, sduP, sizeof (struct rlc_indication));
        rrc_rg_srb3_decode (UE_Id, sduP, sdu_offset);
        break;
      default:
        msg ("[RRC] [SRB-RG] Invalid call to rrc_rg_srb_rx : SRB Receive\n");
        break;
    }

  return 0;
}

//module_id_t , rb_id_t , mui_t
//-----------------------------------------------------------------------------
//void* rrc_srb_confirm (u32 muiP, u8 rb_idP, u8 statusP){
void* rrc_rg_srb_confirm (u32 muiP, u8 rb_idP, u8 statusP){
//-----------------------------------------------------------------------------
  int srb_id;
  int UE_Id;

  srb_id = rb_idP % maxRB;
  UE_Id = (rb_idP - srb_id) / maxRB;
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC-SRB] RB%d RX SDU CONFIRM MUI %d, status %d\n", rb_idP, muiP, statusP);
   msg ("[RRC-SRB] SRB_Id %d UE_Id %d\n", srb_id, UE_Id);
  #endif
  /*  Temp OpenAir
  //  if (srb_id == RRC_SRB2_ID){
  if (statusP == RLC_TX_CONFIRM_SUCCESSFULL) {
    rrc_rg_fsm_control (UE_Id, RG_RLC_SUCCESS);
  } else {
    rrc_rg_fsm_control (UE_Id, RG_RLC_FAILURE);
  }
  //  }
  */
  rrc_rg_fsm_control (UE_Id, RG_RLC_SUCCESS);

  return NULL;
}
