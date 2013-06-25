/***************************************************************************
                          rrc_ue_bch_read.c  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform decoding of SIBs
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

//-----------------------------------------------------------------------------
void rrc_ue_read_Seg10 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned int  SIB_type;

  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] Get information from Complete SIB.\n");
  #endif
  /* decode sib_Type */
  status = rrc_PERDec_SIB_Type (pParms, &SIB_type);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] SI-BCH, SIB_type %d, status %d\n", SIB_type, status);
  #endif
  protocol_ms->rrc.ue_bch_blocks.curr_block_type = SIB_type;
  switch (SIB_type) {
    case masterInformationBlock:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currMIB), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_MIB (pParms);
      break;
    case systemInformationBlockType1:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB1), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_SIB1 (pParms);
      break;
    case systemInformationBlockType2:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB2), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_SIB2 (pParms);
      break;
    case systemInformationBlockType5:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB5), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_SIB5 (pParms);
      break;
    case systemInformationBlockType14:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB14), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_SIB14 (pParms);
      break;
    case systemInformationBlockType18:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB18), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      rrc_ue_bch_process_SIB18 (pParms);
      break;
    default:
      msg ("[RRC_BCH-UE] Seg10, unknown SIB_type \n");
      break;
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_read_Seg2 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned int SIB_type;
  int SEG_count;

  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] Get information from First Segment.\n");
  #endif
  /* decode sib_Type */
  status = rrc_PERDec_SIB_Type (pParms, &SIB_type);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] SI-BCH, SIB_type %d , status %d\n", SIB_type, status);
  #endif
  protocol_ms->rrc.ue_bch_blocks.curr_block_type = SIB_type;

  /* decode seg_Count */
  status = rrc_PERDec_SegCount (pParms, &SEG_count);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   //msg("[RRC_BCH-UE] SI-BCH, SEG_count, status %d \n", SEG_count, status);
  #endif
  protocol_ms->rrc.ue_bch_blocks.curr_segment_count = SEG_count;
  protocol_ms->rrc.ue_bch_blocks.curr_segment_index = 0;        //1st segment

  /* save received block */
  switch (SIB_type) {
    case masterInformationBlock:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currMIB), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType1:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB1), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType5:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB5), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType14:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB14), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType18:
      memcpy (&(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB18), ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    default:
      msg ("[RRC_BCH-UE] Seg2, unknown SIB_type \n");
      break;
  }
  protocol_ms->rrc.ue_bch_blocks.curr_block_index = pParms->buff_size;
  protocol_ms->rrc.ue_bch_blocks.curr_block_length = pParms->buff_size;
}

//-----------------------------------------------------------------------------
void rrc_ue_read_Seg3 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned int SIB_type;
  int SEG_index;

  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] Get information from Subsequent Segment.\n");
  #endif
  /* decode sib_Type */
  status = rrc_PERDec_SIB_Type (pParms, &SIB_type);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] SI-BCH, SIB_type %d , status %d\n", SIB_type, status);
  #endif

  /* decode seg_Index */
  status = rrc_PERDec_SegmentIndex (pParms, &SEG_index);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   //msg("[RRC_BCH-UE] SI-BCH, SEG_index %d , status %d \n", SEG_index, status);
  #endif
  // check valid segment
  if ((SIB_type != protocol_ms->rrc.ue_bch_blocks.curr_block_type) ||
      (SEG_index != protocol_ms->rrc.ue_bch_blocks.curr_segment_index + 1) || (SEG_index >= (protocol_ms->rrc.ue_bch_blocks.curr_segment_count - 1))) {
    protocol_ms->rrc.ue_bch_blocks.curr_block_type = NO_BLOCK;
    protocol_ms->rrc.ue_bch_blocks.curr_block_index = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_block_length = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_segment_index = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_segment_count = 0;
    msg ("[RRC_BCH-UE] Invalid subsequent segment received SIB Type: %d , SEG_index :%d\n", SIB_type, SEG_index);
    return;
  }
  protocol_ms->rrc.ue_bch_blocks.curr_segment_index += 1;       //inc segment index

  /* save received block */
  switch (SIB_type) {
    case masterInformationBlock:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currMIB) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType1:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB1) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType5:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB5) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType14:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB14) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType18:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB18) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    default:
      msg ("[RRC_BCH-UE] Seg3, unknown SIB_type \n");
      break;
  }
  protocol_ms->rrc.ue_bch_blocks.curr_block_index += pParms->buff_size;
  protocol_ms->rrc.ue_bch_blocks.curr_block_length += pParms->buff_size;
}

//-----------------------------------------------------------------------------
void rrc_ue_read_Seg11 (PERParms * pParms){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned int SIB_type;
  int SEG_index;

  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] Get information from Last Segment.\n");
  #endif
  /* decode sib_Type */
  status = rrc_PERDec_SIB_Type (pParms, &SIB_type);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
   msg ("[RRC_BCH-UE] SI-BCH, SIB_type %d, status %d.\n", SIB_type, status);
  #endif

  /* decode seg_Index */
  status = rrc_PERDec_SegmentIndex (pParms, &SEG_index);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
  //msg("[RRC_BCH-UE] SI-BCH, SEG_index %d, status %d \n", SEG_index, status);
  #endif
  // check valid segment
  if ((SIB_type != protocol_ms->rrc.ue_bch_blocks.curr_block_type) ||
      (SEG_index != protocol_ms->rrc.ue_bch_blocks.curr_segment_index + 1) || (SEG_index != (protocol_ms->rrc.ue_bch_blocks.curr_segment_count - 1))) {
    protocol_ms->rrc.ue_bch_blocks.curr_block_type = NO_BLOCK;
    protocol_ms->rrc.ue_bch_blocks.curr_block_index = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_block_length = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_segment_index = 0;
    protocol_ms->rrc.ue_bch_blocks.curr_segment_count = 0;
    msg ("[RRC_BCH-UE] Invalid last segment received SIB Type: %d , SEG_index :%d\n", SIB_type, SEG_index);
    return;
  }

  /* save received block */
  switch (SIB_type) {
    case masterInformationBlock:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currMIB) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType1:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB1) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType5:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB5) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType14:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB14) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    case systemInformationBlockType18:
      memcpy ((char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB18) + protocol_ms->rrc.ue_bch_blocks.curr_block_index, ((char *) pParms->buffer) + pParms->buff_index, pParms->buff_size);
      break;
    default:
      msg ("[RRC_BCH-UE] Seg11, unknown SIB_type \n");
      break;
  }

  protocol_ms->rrc.ue_bch_blocks.curr_block_index += pParms->buff_size;
  protocol_ms->rrc.ue_bch_blocks.curr_block_length += pParms->buff_size;

  switch (SIB_type) {
      /* MIB  */
    case masterInformationBlock:
      rrc_ue_bch_process_MIB (pParms);
      break;
      /* SIB1  */
    case systemInformationBlockType1:
      rrc_ue_bch_process_SIB1 (pParms);
      break;
      /* SIB5  */
    case systemInformationBlockType5:
      rrc_ue_bch_process_SIB5 (pParms);
      break;
      /* SIB14  */
    case systemInformationBlockType14:
      rrc_ue_bch_process_SIB14 (pParms);
      break;
      /* SIB18  */
    case systemInformationBlockType18:
      rrc_ue_bch_process_SIB18 (pParms);
      break;
    default:
      msg ("[RRC_BCH-UE] Seg11, unknown SIB_type \n");
      break;
  }
  protocol_ms->rrc.ue_bch_blocks.curr_block_type = NO_BLOCK;
  protocol_ms->rrc.ue_bch_blocks.curr_block_index = 0;
  protocol_ms->rrc.ue_bch_blocks.curr_block_length = 0;
  protocol_ms->rrc.ue_bch_blocks.curr_segment_index = 0;
  protocol_ms->rrc.ue_bch_blocks.curr_segment_count = 0;
}

//-----------------------------------------------------------------------------
int rrc_ue_get_SIBCH_info (PERParms * pParms){
//-----------------------------------------------------------------------------
  char *pBuffer;
  int message_length;
  unsigned int payload_type;
  int status = P_SUCCESS;

  #ifdef DEBUG_RRC_BROADCAST_DETAILS
  //  msg("[RRC_BCH-UE] Get main information from SI-BCH \n");
  #endif
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIBCH);
  rrc_new_per_parms (pParms, pBuffer);

  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;       // idem

  /* decode sfn_Prime */
  status = rrc_PERDec_SFN_Prime (pParms, &protocol_ms->rrc.ue_bch_blocks.currSI_BCH.sfn_Prime);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
  if (status != P_SUCCESS)
    msg ("[RRC_BCH-UE] Decode SFN_Prime, status %d.\n", status);
  #endif

  /* decode payload_type */
  status = rrc_PERDec_ConsUnsigned (pParms, &payload_type, 0, 15);
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
  if (status != P_SUCCESS)
    msg ("[RRC_BCH-UE] Decode payload_type, status %d.\n", status);
  #endif
  protocol_ms->rrc.ue_bch_blocks.currSI_BCH.payload.type = payload_type + 1;

  switch (protocol_ms->rrc.ue_bch_blocks.currSI_BCH.payload.type) {
    case BCH_completeSIB:
      rrc_ue_read_Seg10 (pParms);
      break;
    case BCH_firstSegment:
      rrc_ue_read_Seg2 (pParms);
      break;
    case BCH_subsequentSegment:
      rrc_ue_read_Seg3 (pParms);
      break;
    case BCH_lastSegment:
      rrc_ue_read_Seg11 (pParms);
      break;
    case BCH_noSegment:
      #ifdef DEBUG_RRC_BROADCAST_DETAILS
      msg ("[RRC_BCH-UE] payload type is noSegment, SFN %d.\n", protocol_ms->rrc.ue_bch_blocks.currSI_BCH.sfn_Prime);
      #endif
      // Nothing else to do
      break;
    default:
      //log error message
      msg ("[RRC_BCH-UE] rrc_ue_get_SIBCH_info : unknown payload type %d.\n", protocol_ms->rrc.ue_bch_blocks.currSI_BCH.payload.type);
      break;
  }
  return status;
}

//-----------------------------------------------------------------------------
void rrc_ue_read_next_segment (void){
//-----------------------------------------------------------------------------
//  int status = P_SUCCESS;
  PERParms *pParms = &(protocol_ms->rrc.ue_bch_blocks.perParms);

  // Get main info and decode SI_BCH
  rrc_ue_get_SIBCH_info (pParms);
  //status = rrc_PERDec_SI_BCH (pParms, &(protocol_ms->rrc.ue_bch_blocks.currSI_BCH));
  #ifdef DEBUG_RRC_BROADCAST_DETAILS
  // msg("[RRC_BCH] Decode Broadcast  - status : %d\n", status);
  #endif
}
