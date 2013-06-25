/***************************************************************************
                          rrc_rg_bch_write.c  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform encoding of SIBs
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
#include "rrc_bch_mib.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_bch.h"


//-----------------------------------------------------------------------------
void rrc_rg_schedule_bch (void){
//-----------------------------------------------------------------------------
  int i;
  int sib_rep;

  MasterInformationBlock *pmib = &(protocol_bs->rrc.rg_bch_blocks.currMIB);
  //set frame number
  protocol_bs->rrc.rg_bch_blocks.currSI_BCH.sfn_Prime = protocol_bs->rrc.current_SFN % 2048;   //a revoir
  // clean encoded memory
  for (i = 0; i < maxSI; i++) {
    protocol_bs->rrc.rg_bch_blocks.encoded_currSIBCH[i] = 0;
  }
  // check which block is next
  // MIB
//   msg("[RRC_DEBUG] current frame : %d, pos : %d\n", protocol_bs->rrc.current_SFN, (protocol_bs->rrc.current_SFN %(2<<MIB_REP))/2);
  if ((protocol_bs->rrc.current_SFN % (2 << MIB_REP)) / 2 == MIB_POS) {
    protocol_bs->rrc.rg_bch_blocks.next_block_type = masterInformationBlock;
  } else {
    if (((protocol_bs->rrc.current_SFN % (2 << MIB_REP)) / 2 == LONG_SIB_POS)
        && (protocol_bs->rrc.rg_bch_blocks.SIB1_oneTimeShot == TRUE)) {
      protocol_bs->rrc.rg_bch_blocks.next_block_type = systemInformationBlockType1;
      protocol_bs->rrc.rg_bch_blocks.SIB1_oneTimeShot = FALSE;
    } else {
      for (i = 0; i < supportedSIBs; i++) {
        // SIBs -- if SIB_REP>0 and frame%SIB_REP==SIP_POS
        sib_rep = pmib->sibSb_ReferenceList.sib_ref[i].scheduling.scheduling_sib_rep.sib_Rep;
        #ifdef DEBUG_RRC_BROADCAST_DETAILS
         //msg("[RRC_DEBUG] current frame : %d, SIB# %d, rep %d,  pos : %d\n", protocol_bs->rrc.current_SFN,i,sib_rep, (protocol_bs->rrc.current_SFN % (2<<sib_rep))/2);
        #endif
        if ((sib_rep > 0) && ((protocol_bs->rrc.current_SFN % (2 << sib_rep)) / 2 == pmib->sibSb_ReferenceList.sib_ref[i].scheduling.scheduling_sib_rep.sib_Pos)) {
          protocol_bs->rrc.rg_bch_blocks.next_block_type = protocol_bs->rrc.rg_bch_blocks.sib_list[i];
          break;
        }
      }
      if (i == supportedSIBs) {
        protocol_bs->rrc.rg_bch_blocks.next_block_type = NO_BLOCK;
      }
    }
  }

#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH] current frame : %d, SFN : %d\n", protocol_bs->rrc.current_SFN, protocol_bs->rrc.rg_bch_blocks.currSI_BCH.sfn_Prime);
  msg ("[RRC_BCH-RG] next block : %d\n", protocol_bs->rrc.rg_bch_blocks.next_block_type);
#endif
}

//-----------------------------------------------------------------------------
void rrc_rg_segment_block (void){
//-----------------------------------------------------------------------------
  int block_length;

  //check if there is no pending block
  if (protocol_bs->rrc.rg_bch_blocks.curr_block_type == NO_BLOCK) {
    //get next block scheduled and find its length
    switch (protocol_bs->rrc.rg_bch_blocks.next_block_type) {
        case NO_BLOCK:
          block_length = 0;
          break;
        case masterInformationBlock:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currMIB);
          break;
        case systemInformationBlockType1:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1);
          break;
        case systemInformationBlockType2:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIB2);
          break;
        case systemInformationBlockType5:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5);
          break;
        case systemInformationBlockType14:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14);
          break;
        case systemInformationBlockType18:
          block_length = rrc_get_per_length ((ENCODEDBLOCK *) protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18);
          break;
        default:
          block_length = 10000; //value over max
    }
    protocol_bs->rrc.rg_bch_blocks.curr_block_length = block_length;
    // check if segmentation is needed
    if (block_length <= LSIBcompl) {    //complete segment
      protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type = BCH_completeSIB;
      if (block_length == 0)
        protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type = BCH_noSegment;
    } else {                    //first segment of block
      protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type = BCH_firstSegment;
    }
    protocol_bs->rrc.rg_bch_blocks.curr_block_type = protocol_bs->rrc.rg_bch_blocks.next_block_type;
    protocol_bs->rrc.rg_bch_blocks.next_block_type = NO_BLOCK;
  } else {
    // there is a pending block - check if continuing or final
    if ((protocol_bs->rrc.rg_bch_blocks.curr_block_length - protocol_bs->rrc.rg_bch_blocks.curr_block_index) <= LSIBfixed)      //fixed segment size
      protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type = BCH_lastSegment;
    else
      protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type = BCH_subsequentSegment;
  }
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_Seg1 (void){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH-RG] Fill NoSegment SIB with requested info [None] \n");
#endif
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_Seg10 (PERParms * pParms){
//-----------------------------------------------------------------------------
  CompleteSIB    *pSegment;

#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH-RG] Fill Complete SIB. \n");
#endif
  // Temp malloc for test
  //pSegment = malloc(sizeof(CompleteSIB));
  protocol_bs->rrc.rg_bch_blocks.tSegment = get_free_mem_block (sizeof (CompleteSIB));
  pSegment = (CompleteSIB *) protocol_bs->rrc.rg_bch_blocks.tSegment->data;
  //
  protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.segment.completeSIB = pSegment;
  pSegment->sib_Type = protocol_bs->rrc.rg_bch_blocks.curr_block_type;
  pSegment->sib_Data_fixed.numbits = 0;
  switch (protocol_bs->rrc.rg_bch_blocks.curr_block_type) {
      case masterInformationBlock:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currMIB);
        break;
      case systemInformationBlockType1:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1);
        break;
      case systemInformationBlockType2:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB2);
        break;
      case systemInformationBlockType5:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5);
        break;
      case systemInformationBlockType14:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14);
        break;
      case systemInformationBlockType18:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18);
        break;
      default:
        pParms->data = 0;
        //log error message
#ifdef DEBUG_RRC_BROADCAST
        msg ("[RRC_BCH-RG] Error rrc_rg_fill_Seg10 - switch default \n");
#endif
  }
  protocol_bs->rrc.rg_bch_blocks.curr_block_type = NO_BLOCK;
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_Seg2 (PERParms * pParms){
//-----------------------------------------------------------------------------
  FirstSegment   *pSegment;

#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH-RG] Fill First Segment. \n");
#endif
  // Temp malloc for test
  //pSegment = malloc(sizeof(FirstSegment));
  protocol_bs->rrc.rg_bch_blocks.tSegment = get_free_mem_block (sizeof (FirstSegment));
  pSegment = (FirstSegment *) protocol_bs->rrc.rg_bch_blocks.tSegment->data;
  //
  protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.segment.firstSegment = pSegment;
  pSegment->sib_Type = protocol_bs->rrc.rg_bch_blocks.curr_block_type;
  if ((protocol_bs->rrc.rg_bch_blocks.curr_block_length % LSIBfixed) == 0)
    pSegment->seg_Count = (protocol_bs->rrc.rg_bch_blocks.curr_block_length / LSIBfixed);
  else
    pSegment->seg_Count = (protocol_bs->rrc.rg_bch_blocks.curr_block_length / LSIBfixed) + 1;
  protocol_bs->rrc.rg_bch_blocks.curr_segment_index = 0;
  pParms->data_size = protocol_bs->rrc.rg_bch_blocks.curr_block_length;
  pParms->data_offset = 0;
  pSegment->sib_Data_fixed.numbits = 0;
  switch (protocol_bs->rrc.rg_bch_blocks.curr_block_type) {
      case masterInformationBlock:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currMIB);
        break;
      case systemInformationBlockType1:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1);
        break;
        // SIB2 is never segmented
      case systemInformationBlockType5:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5);
        break;
      case systemInformationBlockType14:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14);
        break;
      case systemInformationBlockType18:
        pParms->data = (ENCODEDBLOCK *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18);
        break;
      default:
        pParms->data = 0;
#ifdef DEBUG_RRC_BROADCAST
        msg ("[RRC_BCH-RG] Error rrc_rg_fill_Seg2 - switch default \n");
#endif
  }
  protocol_bs->rrc.rg_bch_blocks.curr_block_index += LSIBfixed;
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_Seg3 (PERParms * pParms){
//-----------------------------------------------------------------------------
  SubsequentSegment *pSegment;
  char *pTemp = NULL;

#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH-RG] Fill Subsequent Segment. \n");
#endif
  // Temp malloc for test
  //pSegment = malloc(sizeof(SubsequentSegment));
  protocol_bs->rrc.rg_bch_blocks.tSegment = get_free_mem_block (sizeof (SubsequentSegment));
  pSegment = (SubsequentSegment *) protocol_bs->rrc.rg_bch_blocks.tSegment->data;
  //
  protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.segment.subsequentSegment = pSegment;
  pSegment->sib_Type = protocol_bs->rrc.rg_bch_blocks.curr_block_type;
  protocol_bs->rrc.rg_bch_blocks.curr_segment_index += 1;
  pSegment->segmentIndex = protocol_bs->rrc.rg_bch_blocks.curr_segment_index;
  pSegment->sib_Data_fixed.numbits = 0;
  pParms->data_offset = protocol_bs->rrc.rg_bch_blocks.curr_block_index;
  switch (protocol_bs->rrc.rg_bch_blocks.curr_block_type) {
      case masterInformationBlock:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currMIB)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType1:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
        // SIB2 is never segmented
      case systemInformationBlockType5:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType14:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType18:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      default:
        pParms->data = 0;
#ifdef DEBUG_RRC_BROADCAST
        msg ("[RRC_BCH-RG] Error rrc_rg_fill_Seg3 - switch default \n");
#endif
  }
  pParms->data = (ENCODEDBLOCK *) pTemp;

  protocol_bs->rrc.rg_bch_blocks.curr_block_index += LSIBfixed;
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_Seg11 (PERParms * pParms){
//-----------------------------------------------------------------------------
  LastSegment    *pSegment;
  char *pTemp = NULL;

#ifdef DEBUG_RRC_BROADCAST_DETAILS
  msg ("[RRC_BCH-RG] Fill Last Segment. \n");
#endif
  // Temp malloc for test
  //pSegment = malloc(sizeof(LastSegment));
  protocol_bs->rrc.rg_bch_blocks.tSegment = get_free_mem_block (sizeof (LastSegment));
  pSegment = (LastSegment *) protocol_bs->rrc.rg_bch_blocks.tSegment->data;
  //
  protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.segment.lastSegment = pSegment;
  pSegment->sib_Type = protocol_bs->rrc.rg_bch_blocks.curr_block_type;
  protocol_bs->rrc.rg_bch_blocks.curr_segment_index += 1;
  pSegment->segmentIndex = protocol_bs->rrc.rg_bch_blocks.curr_segment_index;
  pSegment->sib_Data_fixed.numbits = 0;
  pParms->data_offset = protocol_bs->rrc.rg_bch_blocks.curr_block_index;
  switch (protocol_bs->rrc.rg_bch_blocks.curr_block_type) {
      case masterInformationBlock:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currMIB)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType1:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
        // SIB2 is never segmented
      case systemInformationBlockType5:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType14:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      case systemInformationBlockType18:
        pTemp = (char *) (&(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18)) + protocol_bs->rrc.rg_bch_blocks.curr_block_index;
        break;
      default:
        pParms->data = 0;
#ifdef DEBUG_RRC_BROADCAST
       msg ("[RRC_BCH-RG] Error rrc_rg_fill_Seg11 - switch default \n");
#endif
  }
  pParms->data = (ENCODEDBLOCK *) pTemp;
  protocol_bs->rrc.rg_bch_blocks.curr_block_type = NO_BLOCK;
  protocol_bs->rrc.rg_bch_blocks.curr_block_index = 0;
}

//-----------------------------------------------------------------------------
void rrc_rg_fill_SIBCH (PERParms * pParms){
//-----------------------------------------------------------------------------
  char *pBuffer;

#ifdef DEBUG_RRC_BROADCAST
//  msg("[RRC_BCH-RG] Fill blocks with requested info \n");
#endif
  pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currSIBCH);
  rrc_new_per_parms (pParms, pBuffer);

  switch (protocol_bs->rrc.rg_bch_blocks.currSI_BCH.payload.type) {
      case BCH_completeSIB:
        rrc_rg_fill_Seg10 (pParms);
        break;
      case BCH_firstSegment:
        rrc_rg_fill_Seg2 (pParms);
        break;
      case BCH_subsequentSegment:
        rrc_rg_fill_Seg3 (pParms);
        break;
      case BCH_lastSegment:
        rrc_rg_fill_Seg11 (pParms);
        break;
      case BCH_noSegment:
        rrc_rg_fill_Seg1 ();
        break;
      default:
        //log error message
        msg ("[RRC_BCH-RG] Error rrc_rg_fill_SIBCH - switch default \n");
  }
}

//-----------------------------------------------------------------------------
void rrc_rg_prepare_next_segment (void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  PERParms       *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);

  // schedule next block
  rrc_rg_schedule_bch ();

  // segment
  rrc_rg_segment_block ();

  // fill structures with valid parameters
  rrc_rg_fill_SIBCH (pParms);
#ifdef DEBUG_RRC_BROADCAST
  //msg("[RRC_BCH-RG] Current block index %d \n", protocol_bs->rrc.rg_bch_blocks.curr_block_index);
#endif

  // Encode
  status = rrc_PEREnc_SI_BCH (pParms, &(protocol_bs->rrc.rg_bch_blocks.currSI_BCH));
  // Add length
  rrc_set_per_length (pParms);

#ifdef DEBUG_RRC_BROADCAST_DETAILS
// msg("\n[RRC_BCH-RG] Next Broadcast preparation - status : %d\n", status);
  rrc_print_per_parms (pParms);
// msg("[RRC_BCH-RG] End of buffer \n\n");
#endif

}
