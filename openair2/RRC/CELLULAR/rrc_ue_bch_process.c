/***************************************************************************
                          rrc_ue_bch_process.c  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform de-scheduling of SIBs
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
#include "rrc_bch_mib.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_bch.h"
#include "rrc_proto_int.h"
#include "rrc_proto_intf.h"

//#define DEBUG_RRC_BROADCAST
//-----------------------------------------------------------------------------
void rrc_ue_bch_init (void){
//-----------------------------------------------------------------------------
  MasterInformationBlock *pmib = &(protocol_ms->rrc.ue_bch_blocks.currMIB);
  pmib->sibSb_ReferenceList.sib_ref[0].sibSb_Type.type = SIBSb_sysInfoType1;
  pmib->sibSb_ReferenceList.sib_ref[1].sibSb_Type.type = SIBSb_sysInfoType2;
  pmib->sibSb_ReferenceList.sib_ref[2].sibSb_Type.type = SIBSb_sysInfoType5;
  pmib->sibSb_ReferenceList.sib_ref[3].sibSb_Type.type = SIBSb_sysInfoType11;
  pmib->sibSb_ReferenceList.sib_ref[4].sibSb_Type.type = SIBSb_sysInfoType14;
  pmib->sibSb_ReferenceList.sib_ref[5].sibSb_Type.type = SIBSb_sysInfoType18;

  protocol_ms->rrc.ue_bch_blocks.SIB14_timeout = TRUE;
  protocol_ms->rrc.ue_bch_blocks.SIB14_timeout_value = (2 << SIB14_REP) * SIB14_TFACTOR;
  protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.mib_vt = 15;        //0x0f >8
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_MIB (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  //Temp - test MIB value tag directly in MIB - TBD later 8.1.1.7.3 [25.331]
  if ((int) protocol_ms->rrc.ue_bch_blocks.encoded_currMIB[1] != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.mib_vt) {
    // skip segment decoding, 1st step works on bytes only
    status = rrc_PERDec_MasterInformationBlock (pParms, &(protocol_ms->rrc.ue_bch_blocks.currMIB));
    #ifdef DEBUG_RRC_BROADCAST
     msg ("[RRC_BCH-UE] Decode MIB status %d.\n", status);
    #endif
    if (status == P_SUCCESS) {
      #ifdef DEBUG_RRC_BROADCAST
       msg ("[RRC_BCH-UE] Process information from MIB.\n");
       msg ("[RRC_BCH-UE] MIB value tag: %d\n", protocol_ms->rrc.ue_bch_blocks.currMIB.mib_ValueTag);
       msg ("[RRC_BCH-UE] PLMN Identity: MCC %d%d%d , MNC %d\n",
            protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mcc.elem[0],
            protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mcc.elem[1],
            protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mcc.elem[2],
            protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc);
       // protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc.elem[0],
       // protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc.elem[1],
       // protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc.elem[2]);
       msg ("[RRC_BCH-UE] Value tags: SIB1 %d , SIB2 %d , SIB5 %d, SIB11 %d, SIB18 %d.\n",
            protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1,
            protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2,
            protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5,
            protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11,
            protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18);
      #endif
      protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.mib_vt = protocol_ms->rrc.ue_bch_blocks.currMIB.mib_ValueTag;
      //  if first base, store cell_id from RG - Otherwise, check this is the right cell (PLMN)
      if (protocol_ms->rrc.cell_id == 0)
        protocol_ms->rrc.cell_id = protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc;
      else if (protocol_ms->rrc.cell_id != protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc)
        msg ("\n\n[RRC_BCH-UE] Decode MIB Error - Broadcast received from another RG - cell  %d.\n\n", protocol_ms->rrc.ue_bch_blocks.currMIB.plmn_Identity.mnc);
      #ifdef DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH-UE] Decode MIB status - Cell_id %d.\n", protocol_ms->rrc.cell_id);
      #endif
      // check sib_ref information
    }
  } else {
    #ifdef DEBUG_RRC_BROADCAST_DETAILS
     msg ("[RRC_BCH-UE] Ignore information from MIB %d , %d.\n", (int) protocol_ms->rrc.ue_bch_blocks.encoded_currMIB[1], protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.mib_vt);
     // rrc_print_buffer (protocol_ms->rrc.ue_bch_blocks.encoded_currMIB,20);
    #endif
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB1 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  //if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1
  //                != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib1_vt){
  //skip segment decoding, 1st step works on bytes only
  status = rrc_PERDec_SysInfoType1 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB1));
  #ifdef DEBUG_RRC_BROADCAST
   msg ("[RRC_BCH-UE] Decode SIB1 status %d.\n", status);
  #endif
  if (status == P_SUCCESS) {
    #ifdef DEBUG_RRC_BROADCAST
    msg ("[RRC_BCH-UE] Process information from System Information Block 1.\n");
    msg ("[RRC_BCH-UE] NAS System Information.\n");
    rrc_print_buffer ((char *) &protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.data, protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.numocts);
    msg ("[RRC_BCH-UE] UE timers and counters.\n");
    rrc_print_buffer ((char *) &protocol_ms->rrc.ue_bch_blocks.currSIB1.ue_TimersAndConstants.data, protocol_ms->rrc.ue_bch_blocks.currSIB1.ue_TimersAndConstants.numocts);
    #endif
    // save values of timers and counters
    // send NAS data to NAS
    if (protocol_ms->rrc.ue_wait_establish_req == 1) {
      rrc_ue_broadcast_encode_nas_sib1 ();
      rrc_ue_write_FIFO (protocol_ms->rrc.NASMessageToXmit);
    } else {
      protocol_ms->rrc.ue_wait_establish_req = 2;
    }
    protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib1_vt = protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1;
  }
  //   }
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB2 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2 != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib2_vt) {
    // skip segment decoding, 1st step works on bytes only
    status = rrc_PERDec_SysInfoType2 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB2));
    if (status != P_SUCCESS) {
      #ifdef DEBUG_RRC_BROADCAST
       msg ("[RRC_BCH-UE] Decode SIB2 status %d.\n", status);
      #endif
    } else {
      #ifdef DEBUG_RRC_BROADCAST
       msg ("[RRC_BCH-UE] Process information from System Information Block 2.\n");
       msg ("[RRC_BCH-UE] IP address.\n");
       rrc_print_buffer ((char *) &protocol_ms->rrc.ue_bch_blocks.currSIB2.net_IP_addr.data, 16);
      #endif
      // save IP address of RG
      protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib2_vt = protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2;
    }
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB5 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5 != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib5_vt) {
    // skip segment decoding, 1st step works on bytes only
    status = rrc_PERDec_SysInfoType5 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB5));
    #ifdef DEBUG_RRC_STATE
     msg ("[RRC_BCH-UE] Decode SIB5 status %d.\n", status);
    #endif
    if (status == P_SUCCESS) {
      #ifdef  DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH-UE] Process information from System Information Block 5.\n");
      msg ("[RRC_BCH-UE] FACH RACH configuration Information.");
      rrc_print_buffer ((char *) &protocol_ms->rrc.ue_bch_blocks.currSIB5.prach_sCCPCH_SIList.data, protocol_ms->rrc.ue_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts);
      msg ("[RRC_BCH-UE] Config data: %s \n", (char *) &protocol_ms->rrc.ue_bch_blocks.currSIB5.prach_sCCPCH_SIList.data);
      #endif
/*  **** REMOVED FOR OPENAIR ****
      // save configuration
      memcpy ((char *) &(rrm_config->sccpch), (char *) protocol_ms->rrc.ue_bch_blocks.currSIB5.prach_sCCPCH_SIList.data, protocol_ms->rrc.ue_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts);
        if ((rrm_config->prach.rach_trch.tf[0].bs == 0) || (rrm_config->sccpch.fach_trch.tf[0].bs == 0))  // bad config DN april 05
        wcdma_handle_error (WCDMA_ERROR_RRC_NASTY_BCH_CONFIG);
      #ifdef  DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH-UE] config RACH-FACH BS RACH= %d FACH=%d \n ", rrm_config->prach.rach_trch.tf[0].bs, rrm_config->sccpch.fach_trch.tf[0].bs);
      msg ("[RRC_BCH-UE] OpenLoopPowerControl: BCH Tx power %d, alpha %d, prach %d, dpch %d .\n",
           protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.primaryCCPCH_TX_Power,
           protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.alpha,
           protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.prach_ConstantValue, protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.dpch_ConstantValue);
      #endif
      // save outer loop power control parameters
      rrm_config->outer_loop_vars.PCCPCH_POWER = protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.primaryCCPCH_TX_Power;
      rrm_config->pccpch.bch_cctrch.BCH_Power = rrm_config->outer_loop_vars.PCCPCH_POWER;
      rrm_config->outer_loop_vars.alpha = protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.alpha;
      rrm_config->outer_loop_vars.PRACH_CNST = protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.prach_ConstantValue;
      rrm_config->outer_loop_vars.DPCH_CNST = protocol_ms->rrc.ue_bch_blocks.currSIB5.openLoopPowerControl_TDD.dpch_ConstantValue;
*/

      //  w NAS : Forward broadcast to NAS -
      #ifdef DEBUG_RRC_STATE
      msg ("[RRC_BCH-UE] DEBUG SIB5 ue_wait_establish_req = %d.\n", protocol_ms->rrc.ue_wait_establish_req );
      #endif
      if (protocol_ms->rrc.ue_wait_establish_req == 0) {
        msg ("\n\n [RRC]-FSM can start - SIB5 with Common Configuration received - \n\n");
        protocol_ms->rrc.ue_wait_establish_req = 1;
        protocol_ms->rrc.ue_broadcast_counter = 1;
      } else {
        if (protocol_ms->rrc.ue_wait_establish_req == 2) {
          rrc_ue_broadcast_encode_nas_sib1 ();
          rrc_ue_write_FIFO (protocol_ms->rrc.NASMessageToXmit);
          protocol_ms->rrc.ue_wait_establish_req = 1;
        }
      }
      protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib5_vt = protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5;
    }
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB11 (PERParms * pParms){
//-----------------------------------------------------------------------------
/*
  int status = P_SUCCESS;
  if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11
                   != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib11_vt){
    // skip segment decoding, 1st step works on bytes only
    status = rrc_PERDec_SysInfoType11 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB11));
    if (status != P_SUCCESS){
      #ifdef DEBUG_RRC_BROADCAST
       msg("[RRC_BCH-UE] Decode SIB11 status %d.\n", status);
      #endif
    }else{
      #ifdef DEBUG_RRC_BROADCAST
      msg("[RRC_BCH-UE] Process information from System Information Block 11.\n");
      #endif
      // save data
      // update value tag
      protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib11_vt =
          protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11;
    }
  }
*/
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB14 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i, numslots;

  // skip segment decoding, 1st step works on bytes only
  status = rrc_PERDec_SysInfoType14 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB14));
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] Decode SIB14 status %d.\n", status);
  #endif
  if (status == P_SUCCESS) {
    if (protocol_ms->rrc.ue_bch_blocks.SIB14_timeout == TRUE) {
      numslots = protocol_ms->rrc.ue_bch_blocks.currSIB14.individualTS_InterferenceList.numSlots;
      #ifdef DEBUG_RRC_BROADCAST_DETAILS
      msg ("[RRC_BCH-UE] Process information from System Information Block 14.\n");
      msg ("[RRC_BCH-UE] Expiration Time factor : %d,  Number of slots: %d \n", protocol_ms->rrc.ue_bch_blocks.currSIB14.expirationTimeFactor, numslots);
      for (i = 0; i < numslots; i++) {
        msg ("[RRC_BCH-UE] Slot : %d,  Interference: %d \n",
             protocol_ms->rrc.ue_bch_blocks.currSIB14.individualTS_InterferenceList.data[i].timeslot,
             protocol_ms->rrc.ue_bch_blocks.currSIB14.individualTS_InterferenceList.data[i].ul_TimeslotInterference);
      }
      #endif
/*  **** REMOVED FOR OPENAIR ****
      // save data
      for (i = 0; i < numslots; i++) {
        rrm_config->outer_loop_vars.IBTS[i] = protocol_ms->rrc.ue_bch_blocks.currSIB14.individualTS_InterferenceList.data[i].ul_TimeslotInterference;
      }
*/
      //  no update of value tag for SIB14
      protocol_ms->rrc.ue_bch_blocks.SIB14_timeout = FALSE;
    }
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_bch_process_SIB18 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18 != protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib18_vt) {
    // skip segment decoding, 1st step works on bytes only
    status = rrc_PERDec_SysInfoType18 (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSIB18));
    #ifdef DEBUG_RRC_BROADCAST
     msg ("[RRC_BCH-UE] Decode SIB18 status %d.\n", status);
    #endif
    if (status == P_SUCCESS) {
      #ifdef DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH-UE] Process information from System Information Block 18.\n");
      msg ("[RRC_BCH-UE] Neighbour cells list.");
      rrc_print_buffer ((char *) &protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.data, protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.numocts);
      msg ("[RRC_BCH-UE] data <<%s>>\n", (char *) &protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.data);
      msg ("[RRC_BCH-UE] Code groups, cell 1 %d, cell 2 %d. \n",
         protocol_ms->rrc.ue_bch_blocks.currSIB18.cellCodegroups.data[0], protocol_ms->rrc.ue_bch_blocks.currSIB18.cellCodegroups.data[1]);
      #endif
      rrc_ue_read_neighboring_cells_info ();
      rrc_ue_broadcast_encode_nas_sib18 ();
      rrc_ue_write_FIFO (protocol_ms->rrc.NASMessageToXmit);
      // update value tag
      protocol_ms->rrc.ue_bch_blocks.bch_ue_vts.sib18_vt = protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18;
    }
  }
}
