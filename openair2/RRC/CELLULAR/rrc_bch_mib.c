/***************************************************************************
                          rrc_bch_mib.c  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform encoding of MIB and SIBs CCC
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#ifdef NODE_MT
#include "rrc_ue_vars.h"
#endif
#ifdef NODE_RG
#include "rrc_rg_vars.h"
#endif
#include "rrc_bch_mib.h"

#include "rrc_proto_bch.h"

/**************************************************************/
/*  SystemInformation_BCH                                     */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SI_BCH (PERParms * pParms, SystemInformation_BCH * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode sfn_Prime */
  status = rrc_PEREnc_SFN_Prime (pParms, pvalue->sfn_Prime);
  if (status != P_SUCCESS)
    return status;

  /* encode payload */
  status = rrc_PEREnc_SI_BCH_payload (pParms, &pvalue->payload);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SI_BCH (PERParms * pParms, SystemInformation_BCH * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

//   /* decode sfn_Prime */
  // Done in rrc_ue_get_SIBCH_info()
//   /* decode payload */
//   status = rrc_PERDec_SI_BCH_payload (pParms, &pvalue->payload);
////   if (status != P_SUCCESS) return status;

  return status;
}
#endif

/**************************************************************/
/*  FirstSegment - Segment Type = 2                           */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_FirstSegment (PERParms * pParms, FirstSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode sib_Type */
  status = rrc_PEREnc_SIB_Type (pParms, pvalue->sib_Type);
  if (status != P_SUCCESS)
    return status;

  /* encode seg_Count */
  status = rrc_PEREnc_SegCount (pParms, pvalue->seg_Count);
  if (status != P_SUCCESS)
    return status;

  /* encode sib_Data_fixed */
  status = rrc_PEREnc_SIB_Data_fixed (pParms, pvalue->sib_Data_fixed);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_FirstSegment (PERParms * pParms, FirstSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* decode sib_Type */
  // done in rrc_ue_read_Seg2();

  /* decode seg_Count */
  //done in rrc_ue_read_Seg2();
//
//   /* decode sib_Data_fixed */
//   status = rrc_PERDec_SIB_Data_fixed (pParms, &pvalue->sib_Data_fixed);

  return status;
}
#endif

/**************************************************************/
/*  SubsequentSegment - Segment Type = 3                      */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SubsequentSegment (PERParms * pParms, SubsequentSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode sib_Type */
  status = rrc_PEREnc_SIB_Type (pParms, pvalue->sib_Type);
  if (status != P_SUCCESS)
    return status;

  /* encode segmentIndex */
  status = rrc_PEREnc_SegmentIndex (pParms, pvalue->segmentIndex);
  if (status != P_SUCCESS)
    return status;

  /* encode sib_Data_fixed */
  status = rrc_PEREnc_SIB_Data_fixed (pParms, pvalue->sib_Data_fixed);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SubsequentSegment (PERParms * pParms, SubsequentSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* decode sib_Type */
  // done in rrc_ue_read_Seg3();
//
//   /* decode segmentIndex */
//   status = rrc_PERDec_SegmentIndex (pParms, &pvalue->segmentIndex);
//   //if (status != P_SUCCESS) return status;
//
//   /* decode sib_Data_fixed */
//   status = rrc_PERDec_SIB_Data_fixed (pParms, &pvalue->sib_Data_fixed);
//   //if (status != P_SUCCESS) return status;
  return status;
}
#endif

/**************************************************************/
/*  CompleteSIB - Segment Type = 10                           */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_CompleteSIB (PERParms * pParms, CompleteSIB * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode sib_Type */
  status = rrc_PEREnc_SIB_Type (pParms, pvalue->sib_Type);
  if (status != P_SUCCESS)
    return status;

  /* encode sib_Data_fixed */
  status = rrc_PEREnc_CompleteSIB_sib_Data_fixed (pParms, pvalue->sib_Data_fixed);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_CompleteSIB (PERParms * pParms, CompleteSIB * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* decode sib_Type */
  // done in rrc_ue_read_Seg10();

  /* decode sib_Data_fixed */
//   status = rrc_PERDec_CompleteSIB_sib_Data_fixed (ctxt_p, &pvalue->sib_Data_fixed);
  //if (status != P_SUCCESS) return status;
  return status;
}
#endif

/**************************************************************/
/*  LastSegment - Segment Type = 11                           */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_LastSegment (PERParms * pParms, LastSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode sib_Type */
  status = rrc_PEREnc_SIB_Type (pParms, pvalue->sib_Type);
  if (status != P_SUCCESS)
    return status;

  /* encode segmentIndex */
  status = rrc_PEREnc_SegmentIndex (pParms, pvalue->segmentIndex);
  if (status != P_SUCCESS)
    return status;

  /* encode sib_Data_fixed */
  status = rrc_PEREnc_SIB_Data_fixed (pParms, pvalue->sib_Data_fixed);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_LastSegment (PERParms * pParms, LastSegment * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* decode sib_Type */
  // done in rrc_ue_read_Seg11();

  /* decode segmentIndex */
  // done in rrc_ue_read_Seg11();

//   /* decode sib_Data_fixed */
//   status = rrc_PERDec_SIB_Data_fixed (pParms, &pvalue->sib_Data_fixed);
//   //if (status != P_SUCCESS) return status;
  return status;
}
#endif

/**************************************************************/
/*  SystemInformation_BCH_payload                             */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SI_BCH_payload (PERParms * pParms, SystemInformation_BCH_payload * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* Encode choice index value */
  status = rrc_PEREnc_ConsUnsigned (pParms, pvalue->type - 1, 0, 15);
  if (status != P_SUCCESS)
    return status;

  /* Encode root element data value */
  switch (pvalue->type) {
        /* noSegment */
      case 1:
        /* NULL */
        break;
        /* firstSegment */
      case 2:
        status = rrc_PEREnc_FirstSegment (pParms, pvalue->segment.firstSegment);
        if (status != P_SUCCESS)
          return status;
        break;
        /* subsequentSegment */
      case 3:
        status = rrc_PEREnc_SubsequentSegment (pParms, pvalue->segment.subsequentSegment);
        if (status != P_SUCCESS)
          return status;
        break;
//      /* lastSegmentShort */
//      case 4:
//         status = rrc_PEREnc_LastSegmentShort (pParms, pvalue->segment.lastSegmentShort);
//         if (status != P_SUCCESS) return status;
//         break;
//      /* completeSIB_List */
//      case 8:
//         status = rrc_PEREnc_CompleteSIB_List (pParms, pvalue->segment.completeSIB_List);
//         if (status != P_SUCCESS) return status;
//         break;
        /* completeSIB */
      case 10:
        status = rrc_PEREnc_CompleteSIB (pParms, pvalue->segment.completeSIB);
        if (status != P_SUCCESS)
          return status;
        break;
        /* lastSegment */
      case 11:
        status = rrc_PEREnc_LastSegment (pParms, pvalue->segment.lastSegment);
        if (status != P_SUCCESS)
          return status;
        break;
      default:
        return P_INVVALUE;
  }
  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SI_BCH_payload (PERParms * pParms, SystemInformation_BCH_payload * pvalue){
//-----------------------------------------------------------------------------
  int status = P_INVPROC;

  return status;
}
#endif

/**************************************************************/
/*  MasterInformationBlock                                    */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MasterInformationBlock (PERParms * pParms, MasterInformationBlock * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  /* encode mib_ValueTag */
  status = rrc_PEREnc_MIB_ValueTag (pParms, pvalue->mib_ValueTag);
  if (status != P_SUCCESS)
    return status;
  /* encode plmn_Type */
  status = rrc_PEREnc_PLMN_Identity (pParms, &pvalue->plmn_Identity);
  if (status != P_SUCCESS)
    return status;
  /* encode sibSb_ReferenceList */
  status = rrc_PEREnc_SIBSb_ReferenceList (pParms, &pvalue->sibSb_ReferenceList);
  if (status != P_SUCCESS)
    return status;
  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MasterInformationBlock (PERParms * pParms, MasterInformationBlock * pvalue){
//-----------------------------------------------------------------------------
  char *pBuffer;
  int message_length;
  int status = P_SUCCESS;

//   /* read block length - this value is unused in 1st step */
//   pBuffer = ((char *) pParms->buffer ) + pParms->buff_index;
//   message_length = rrc_get_per_length ((ENCODEDBLOCK*)pBuffer);
//   pParms->buff_size = message_length - 1;  // idem
//   pParms->buff_index +=1;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currMIB);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;


  /* decode mib_ValueTag */
  status = rrc_PERDec_MIB_ValueTag (pParms, &pvalue->mib_ValueTag);
  if (status != P_SUCCESS)
    return status;

  /* decode plmn_Type */
  status = rrc_PERDec_PLMN_Identity (pParms, &pvalue->plmn_Identity);
  if (status != P_SUCCESS)
    return status;

  /* decode sibSb_ReferenceList */
  status = rrc_PERDec_SIBSb_ReferenceList (pParms, &pvalue->sibSb_ReferenceList);

  return status;
}
#endif

/**************************************************************/
/*  SysInfoType1                                              */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SysInfoType1 (PERParms * pParms, SysInfoType1 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode subnet_NAS_SysInfo */
  status = rrc_PEREnc_NAS_SystemInformation (pParms, pvalue->subnet_NAS_SysInfo);
  if (status != P_SUCCESS)
    return status;

  /* encode ue_TimersAndConstants */
  status = rrc_PEREnc_UE_TimersAndConstants (pParms, pvalue->ue_TimersAndConstants);
  if (status != P_SUCCESS)
    return status;

  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SysInfoType1 (PERParms * pParms, SysInfoType1 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  char *pBuffer;
  int message_length;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB1);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;

  /* decode NAS_SystemInformation */
  status = rrc_PERDec_NAS_SystemInformation (pParms, &pvalue->subnet_NAS_SysInfo);
  if (status != P_SUCCESS)
    return status;

  /* decode ue_TimersAndConstants */
  status = rrc_PERDec_UE_TimersAndConstants (pParms, &pvalue->ue_TimersAndConstants);

  return status;
}
#endif

/**************************************************************/
/*  SysInfoType2                                              */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SysInfoType2 (PERParms * pParms, SysInfoType2 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode IP_Address */
  status = rrc_PEREnc_IP_Address (pParms, &pvalue->net_IP_addr);
  if (status != P_SUCCESS)
    return status;

  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SysInfoType2 (PERParms * pParms, SysInfoType2 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  char *pBuffer;
  int message_length;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB2);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;

  /* decode IP_Address */
  status = rrc_PERDec_IP_Address (pParms, &pvalue->net_IP_addr);

  return status;
}
#endif

/**************************************************************/
/*  SysInfoType5                                              */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SysInfoType5 (PERParms * pParms, SysInfoType5 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode PRACH_SCCPCH_SIList */
  status = rrc_PEREnc_PRACH_SCCPCH_SIList (pParms, pvalue->prach_sCCPCH_SIList);
  if (status != P_SUCCESS)
    return status;

  /* encode OpenLoopPowerControl_TDD */
  status = rrc_PEREnc_OpenLoopPowerControl_TDD (pParms, &pvalue->openLoopPowerControl_TDD);
  if (status != P_SUCCESS)
    return status;

  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SysInfoType5 (PERParms * pParms, SysInfoType5 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  char *pBuffer = NULL;
  int message_length = 0;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB5);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;

  /* decode PRACH_SCCPCH_SIList */
  status = rrc_PERDec_PRACH_SCCPCH_SIList (pParms, &pvalue->prach_sCCPCH_SIList);
  if (status != P_SUCCESS)
    return status;

  /* decode OpenLoopPowerControl_TDD */
  status = rrc_PERDec_OpenLoopPowerControl_TDD (pParms, &pvalue->openLoopPowerControl_TDD);

  return status;
}
#endif

/**************************************************************/
/*  SysInfoType14                                              */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SysInfoType14 (PERParms * pParms, SysInfoType14 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode individualTS_InterferenceList */
  status = rrc_PEREnc_IndividualTS_InterferenceList (pParms, &pvalue->individualTS_InterferenceList);
  if (status != P_SUCCESS)
    return status;

  /* encode expirationTimeFactor */
  status = rrc_PEREnc_ExpirationTimeFactor (pParms, pvalue->expirationTimeFactor);

  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SysInfoType14 (PERParms * pParms, SysInfoType14 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  char *pBuffer = NULL;
  int message_length = 0;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB14);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;

  /* decode individualTS_InterferenceList */
  status = rrc_PERDec_IndividualTS_InterferenceList (pParms, &pvalue->individualTS_InterferenceList);
  if (status != P_SUCCESS)
    return status;

  /* decode expirationTimeFactor */
  status = rrc_PERDec_ExpirationTimeFactor (pParms, &pvalue->expirationTimeFactor);

  return status;
}
#endif

/**************************************************************/
/*  SysInfoType18                                              */
/**************************************************************/
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_SysInfoType18 (PERParms * pParms, SysInfoType18 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  /* encode IdentitiesOfNeighbourCells */
  status = rrc_PEREnc_IdentitiesOfNeighbourCells (pParms, pvalue->cellIdentities);
  if (status != P_SUCCESS)
    return status;

  /* encode CodeGroupsOfNeighbourCells */
  status = rrc_PEREnc_CodeGroupsOfNeighbourCells (pParms, &pvalue->cellCodegroups);
  if (status != P_SUCCESS)
    return status;

  // Add length
  rrc_set_per_length (pParms);

  return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_SysInfoType18 (PERParms * pParms, SysInfoType18 * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  char *pBuffer;
  int message_length;

  /* read block length - this value is unused in 1st step */
  pBuffer = (char *) &(protocol_ms->rrc.ue_bch_blocks.encoded_currSIB18);
  message_length = rrc_get_per_length ((ENCODEDBLOCK *) pBuffer);

  pParms->buffer = (ENCODEDBLOCK *) pBuffer;
  pParms->buff_size = message_length - 1;       // idem
  pParms->buff_index = 1;

  /* decode IdentitiesOfNeighbourCells */
  status = rrc_PERDec_IdentitiesOfNeighbourCells (pParms, &pvalue->cellIdentities);
  if (status != P_SUCCESS)
    return status;

  /* decode CodeGroupsOfNeighbourCells */
  status = rrc_PERDec_CodeGroupsOfNeighbourCells (pParms, &pvalue->cellCodegroups);

  return status;
}
#endif
