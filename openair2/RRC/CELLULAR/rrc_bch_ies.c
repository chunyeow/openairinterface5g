/***************************************************************************
                          rrc_bch_ies.c  -  description
                             -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform encoding/decoding of information elements used for broadcast
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
//#include "rrc_rg_bch_variables.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_bch.h"

/**************************************************************/
/*  MIB_ValueTag                                              */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_MIB_ValueTag (PERParms * pParms, MIB_ValueTag value){
  int status = P_SUCCESS;
  status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 7);
#ifdef DEBUG_RRC_BROADCAST_DETAILS
  //msg("[RRC_PER]encode MIB_ValueTag status %d \n",status);
#endif
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_MIB_ValueTag (PERParms * pParms, MIB_ValueTag * pvalue){
  int status = P_SUCCESS;
  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 7);
#ifdef DEBUG_RRC_BROADCAST_DETAILS
  //msg("[RRC_PER]decode MIB_ValueTag status %d \n",status);
#endif
  return status;
}
#endif

/**************************************************************/
/*  SIB_Type                                                  */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SIB_Type (PERParms * pParms, SIB_Type value){
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConsUnsigned (pParms, (unsigned int) value, 0, 31);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SIB_Type (PERParms * pParms, SIB_Type * pvalue){
  int status = P_SUCCESS;
  status = rrc_PERDec_ConsUnsigned (pParms, (unsigned int *) pvalue, 0, 31);
  return status;
}
#endif

/**************************************************************/
/*  SegCount                                                  */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SegCount (PERParms * pParms, SegCount value){
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, 1, 16);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SegCount (PERParms * pParms, SegCount * pvalue){
  int status = P_SUCCESS;

  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 1, 16);
  return status;
}
#endif

/**************************************************************/
/*  SegmentIndex                                              */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SegmentIndex (PERParms * pParms, SegmentIndex value){
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, 1, 15);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SegmentIndex (PERParms * pParms, SegmentIndex * pvalue){
  int status = P_SUCCESS;

  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 1, 15);
  return status;
}
#endif

/**************************************************************/
/*  SIB_Data_fixed                                            */
/**************************************************************/
// size constraint lower= 222 upper= 222
#ifdef NODE_RG
int rrc_PEREnc_SIB_Data_fixed (PERParms * pParms, SIB_Data_fixed value){
  int status = P_SUCCESS;

  // 1st step, encode with octets
  int  length;

  length = (pParms->data_size - pParms->data_offset >= LSIBfixed) ? LSIBfixed : pParms->data_size - pParms->data_offset;
  status = rrc_PEREnc_OctetString (pParms, length, (char *) pParms->data);
  //status = rrc_PEREnc_BitString (pParms, value.numbits, value.data);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SIB_Data_fixed (PERParms * pParms, SIB_Data_fixed * pvalue){
  int status = P_SUCCESS;

//   status = rrc_PERDec_BitString (pParms,
//                        &pvalue->numbits,
//                        pvalue->data,
//                        sizeof(pvalue->data));
  return status;
}
#endif

/**************************************************************/
/*  CompleteSIB_sib_Data_fixed                                */
/**************************************************************/
   // size constraint lower= 226 upper= 226  -- 24*8=224
#ifdef NODE_RG
int rrc_PEREnc_CompleteSIB_sib_Data_fixed (PERParms * pParms, CompleteSIB_sib_Data_fixed value){
  // size constraint lower= 226 upper= 226  -- 24*8=224
  int status = P_SUCCESS;
  int  max = LSIBcompl;      //cf structure definition
  int  length;
 //
  status = rrc_PEREnc_BitString (pParms, value.numbits, value.data);
  if (status != P_SUCCESS)
    return status;
  // add padding bits - 1st step = maxsize - length of segment.

  length = max - rrc_get_per_length (pParms->data);
  status = rrc_PEREnc_padding (pParms, length);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_CompleteSIB_sib_Data_fixed (PERParms * pParms, CompleteSIB_sib_Data_fixed * pvalue){
  int status = P_SUCCESS;

//   status = rrc_PERDec_BitString (pParms, &pvalue->numbits, pvalue->data, sizeof(pvalue->data));
  return status;
}
#endif

/**************************************************************/
/*  SFN_Prime                                                 */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SFN_Prime (PERParms * pParms, SFN_Prime value){
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 2047);
  //if (status != P_SUCCESS) return status;
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SFN_Prime (PERParms * pParms, SFN_Prime * pvalue){
  int status = P_SUCCESS;
  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 2047);
  return status;
}
#endif

/**************************************************************/
/*  MCC                                                       */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_MCC (PERParms * pParms, RCELL_MCC * pvalue){
  //size constraints =  3, 3
  int status = P_SUCCESS;
  int  i;
  /* encode length determinant */
  status = rrc_PEREnc_Length (pParms, pvalue->numDigits);
  if (status < 0)
    return status;

  /* encode elements */
  for (i = 0; i < pvalue->numDigits; i++) {
    status = rrc_PEREnc_Digit (pParms, pvalue->elem[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_MCC (PERParms * pParms, RCELL_MCC * pvalue){
  //size constraints =  3, 3
  int status = P_SUCCESS;
  int  i;

  /* decode length determinant */
  status = rrc_PERDec_Length (pParms, &pvalue->numDigits);
  if (status != P_SUCCESS)
    return status;

  /* decode elements */
  for (i = 0; i < pvalue->numDigits; i++) {
    status = rrc_PERDec_Digit (pParms, &pvalue->elem[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif

/**************************************************************/
/*  MNC                                                       */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_MNC (PERParms * pParms, MNC * pvalue){
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConsUnsigned (pParms, *pvalue, 1, 65000);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_MNC (PERParms * pParms, MNC * pvalue){
  int status = P_SUCCESS;

  status = rrc_PERDec_ConsUnsigned (pParms, pvalue, 1, 65000);

  return status;
}
#endif


/**************************************************************/
/*  PLMN_Identity                                             */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_PLMN_Identity (PERParms * pParms, RCELL_PLMN_Identity * pvalue){
  int status = P_SUCCESS;

  /* encode mcc */
  status = rrc_PEREnc_MCC (pParms, &pvalue->mcc);
  if (status != P_SUCCESS)
    return status;

  /* encode mnc */
  status = rrc_PEREnc_MNC (pParms, &pvalue->mnc);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_PLMN_Identity (PERParms * pParms, RCELL_PLMN_Identity * pvalue){
  int status = P_SUCCESS;

  /* decode mcc */
  status = rrc_PERDec_MCC (pParms, &pvalue->mcc);
  if (status != P_SUCCESS)
    return status;

  /* decode mnc */
  status = rrc_PERDec_MNC (pParms, &pvalue->mnc);

  return status;
}
#endif

/**************************************************************/
/*  PLMN_ValueTag                                             */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_PLMN_ValueTag (PERParms * pParms, PLMN_ValueTag value){
  int status = P_SUCCESS;
  status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 255);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_PLMN_ValueTag (PERParms * pParms, PLMN_ValueTag * pvalue){
  int status = P_SUCCESS;
  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 255);

  return status;
}
#endif

/**************************************************************/
/*  CellValueTag                                              */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_CellValueTag (PERParms * pParms, CellValueTag value){
  int status = P_SUCCESS;
  status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 3);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_CellValueTag (PERParms * pParms, CellValueTag * pvalue){
  int status = P_SUCCESS;
  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 3);

  return status;
}
#endif

/**************************************************************/
/*  SIBSb_TypeAndTag                                          */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SIBSb_TypeAndTag (PERParms * pParms, SIBSb_TypeAndTag * pvalue){
  int status = P_SUCCESS;

  /* Encode choice index value */
  status = rrc_PEREnc_ConsUnsigned (pParms, pvalue->type - 1, 0, 31);
  if (status != P_SUCCESS)
    return status;

  /* Encode root element data value */
  switch (pvalue->type) {
        /* sysInfoType1 */
      case 1:
        status = rrc_PEREnc_PLMN_ValueTag (pParms, pvalue->type_tag.sysInfoType1);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType2 */
      case 2:
        status = rrc_PEREnc_CellValueTag (pParms, pvalue->type_tag.sysInfoType2);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType5 */
      case 5:
        status = rrc_PEREnc_CellValueTag (pParms, pvalue->type_tag.sysInfoType5);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType11 */
      case 11:
        status = rrc_PEREnc_CellValueTag (pParms, pvalue->type_tag.sysInfoType11);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType14 */
      case 18:
        /* NULL */
        break;
        /* sysInfoType18 */
      case 28:
        status = rrc_PEREnc_CellValueTag (pParms, pvalue->type_tag.sysInfoType18);
        if (status != P_SUCCESS)
          return status;
        break;
      default:
        return P_INVVALUE;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SIBSb_TypeAndTag (PERParms * pParms, SIBSb_TypeAndTag * pvalue){
  int status = P_SUCCESS;
  unsigned int    sib_type;

  status = rrc_PERDec_ConsUnsigned (pParms, &sib_type, 0, 31);
  //if (status != P_SUCCESS) return status;
  //else
  pvalue->type = sib_type + 1;

  switch (sib_type) {
        /* sysInfoType1 */
      case 0:
        status = rrc_PERDec_PLMN_ValueTag (pParms, &pvalue->type_tag.sysInfoType1);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType2 */
      case 1:
        status = rrc_PERDec_CellValueTag (pParms, &pvalue->type_tag.sysInfoType2);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType5 */
      case 4:
        status = rrc_PERDec_CellValueTag (pParms, &pvalue->type_tag.sysInfoType5);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType11 */
      case 10:
        status = rrc_PERDec_CellValueTag (pParms, &pvalue->type_tag.sysInfoType11);
        if (status != P_SUCCESS)
          return status;
        break;
        /* sysInfoType14 */
      case 17:
        /* NULL */
        break;
        /* sysInfoType18 */
      case 27:
        status = rrc_PERDec_CellValueTag (pParms, &pvalue->type_tag.sysInfoType18);
        if (status != P_SUCCESS)
          return status;
        break;
      default:
        return P_INVVALUE;
  }
  return status;
}
#endif

/**************************************************************/
/*  Scheduling_sib_Pos                                        */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_Scheduling_sib_Pos (PERParms * pParms, Scheduling_sib_Pos * pvalue){
  int status = P_SUCCESS;
  int  max_position; //3->4095

  /* encode repetition period */
  status = rrc_PEREnc_ConsUnsigned (pParms, pvalue->sib_Rep, 0, 11);
  if (status != P_SUCCESS)
    return status;

  /* encode position */
  max_position = (2 << (pvalue->sib_Rep - 1)) - 1;
  status = rrc_PEREnc_ConsUnsigned (pParms, pvalue->sib_Pos, 0, max_position);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_Scheduling_sib_Pos (PERParms * pParms, Scheduling_sib_Pos * pvalue){
  int status = P_SUCCESS;
  int  max_position;

  /* decode repetition period */
  status = rrc_PERDec_ConsUnsigned (pParms, &pvalue->sib_Rep, 0, 11);
  if (status != P_SUCCESS)
    return status;

  /* decode position */
  max_position = (2 << (pvalue->sib_Rep - 1)) - 1;
  status = rrc_PERDec_ConsUnsigned (pParms, &pvalue->sib_Pos, 0, max_position);

  return status;
}
#endif

/**************************************************************/
/*  SchedulingInformation                                     */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SchedulingInformation (PERParms * pParms, SchedulingInformation * pvalue){
  int status = P_SUCCESS;

  /* encode segCount */
  status = rrc_PEREnc_SegCount (pParms, pvalue->segCount);
  if (status != P_SUCCESS)
    return status;

  /* encode sib_Pos */
  status = rrc_PEREnc_Scheduling_sib_Pos (pParms, &pvalue->scheduling_sib_rep);
  if (status != P_SUCCESS)
    return status;

//   /* encode sib_PosOffsetInfo */
//      status = rrc_PEREnc_SibOFF_List (pParms, &pvalue->sib_PosOffsetInfo);
//      if (status != P_SUCCESS) return status;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SchedulingInformation (PERParms * pParms, SchedulingInformation * pvalue){
  int status = P_SUCCESS;

  /* decode segCount */
  status = rrc_PERDec_SegCount (pParms, &pvalue->segCount);
  if (status != P_SUCCESS)
    return status;

  /* decode sib_Pos */
  status = rrc_PERDec_Scheduling_sib_Pos (pParms, &pvalue->scheduling_sib_rep);
  if (status != P_SUCCESS)
    return status;

//   /* decode sib_PosOffsetInfo */
//      status = rrc_PERDec_SibOFF_List (pParms, &pvalue->sib_PosOffsetInfo);
//      if (status != P_SUCCESS) return status;

  return status;
}
#endif

/**************************************************************/
/*  SchedulingInformationSIBSb                                */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_SchedulingInformationSIBSb (PERParms * pParms, SchedulingInformationSIBSb * pvalue){
  int status = P_SUCCESS;

  /* encode sibSb_Type */
  status = rrc_PEREnc_SIBSb_TypeAndTag (pParms, &pvalue->sibSb_Type);
  if (status != P_SUCCESS)
    return status;

  /* encode scheduling */
  status = rrc_PEREnc_SchedulingInformation (pParms, &pvalue->scheduling);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_SchedulingInformationSIBSb (PERParms * pParms, SchedulingInformationSIBSb * pvalue){
  int status = P_SUCCESS;

  /* decode sibSb_Type */
  status = rrc_PERDec_SIBSb_TypeAndTag (pParms, &pvalue->sibSb_Type);
  if (status != P_SUCCESS)
    return status;

  /* decode scheduling */
  status = rrc_PERDec_SchedulingInformation (pParms, &pvalue->scheduling);

  return status;
}
#endif

/**************************************************************/
/*  SIBSb_ReferenceList                                       */
/**************************************************************/
//size constraints =  1, 32  --supportedSIBs
#ifdef NODE_RG
int
rrc_PEREnc_SIBSb_ReferenceList (PERParms * pParms, SIBSb_ReferenceList * pvalue)
{
  int status = P_SUCCESS;
  int  i;

  /* encode length determinant */
  status = rrc_PEREnc_Length (pParms, pvalue->numSIB);
  if (status < 0)
    return status;

  /* encode elements */
  for (i = 0; i < pvalue->numSIB; i++) {
    if (protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[i].scheduling.segCount > 0)
      status = rrc_PEREnc_SchedulingInformationSIBSb (pParms, &pvalue->sib_ref[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_SIBSb_ReferenceList (PERParms * pParms, SIBSb_ReferenceList * pvalue)
{
  int status = P_SUCCESS;
  int  i;
  int  sib_t;

  /* decode length determinant */
  status = rrc_PERDec_Length (pParms, &pvalue->numSIB);
  if (status != P_SUCCESS)
    return status;

  /* decode elements */
  for (i = 0; i < pvalue->numSIB; i++) {
    if (pParms->buff_size == 0)
      break;                    // No more SIBs
    sib_t = (int) *(((char *) pParms->buffer) + pParms->buff_index) + 1;
    if (protocol_ms->rrc.ue_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[i].sibSb_Type.type == sib_t)
      status = rrc_PERDec_SchedulingInformationSIBSb (pParms, &pvalue->sib_ref[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif

/**************************************************************/
/*  NAS_SystemInformation                                     */
/**************************************************************/
//size constraints =  1, 8 -- maxSIB1NAS -- (200 04/2003)
#ifdef NODE_RG
int
rrc_PEREnc_NAS_SystemInformation (PERParms * pParms, NAS_SystemInformation value)
{
  int status = P_SUCCESS;
  if (value.numocts >= 0 && value.numocts <= maxSIB1NAS) {
    status = rrc_PEREnc_VarOctetString (pParms, value.numocts, value.data);
    if (status != P_SUCCESS)
      return status;
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}
#endif

//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_NAS_SystemInformation (PERParms * pParms, NAS_SystemInformation * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_VarOctetString (pParms, &pvalue->numocts, pvalue->data);
  return status;
}
#endif

/**************************************************************/
/*  UE_TimersAndConstants                                     */
/**************************************************************/
//size constraints =  1, 5 --8
#ifdef NODE_RG
int
rrc_PEREnc_UE_TimersAndConstants (PERParms * pParms, RCELL_UE_TimersAndConstants value)
{
  int status = P_SUCCESS;
  if (value.numocts >= 0 && value.numocts <= 8) {
    status = rrc_PEREnc_VarOctetString (pParms, value.numocts, value.data);
    if (status != P_SUCCESS)
      return status;
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_UE_TimersAndConstants (PERParms * pParms, RCELL_UE_TimersAndConstants * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_VarOctetString (pParms, &pvalue->numocts, pvalue->data);

  return status;
}
#endif

/**************************************************************/
/*  IP_Address                                             */
/**************************************************************/
//size = 16 octets
#ifdef NODE_RG
int
rrc_PEREnc_IP_Address (PERParms * pParms, IP_Address * pvalue)
{
  int status = P_SUCCESS;

  //status = rrc_PEREnc_BitString (pParms, value.numbits, value.data);
  status = rrc_PEREnc_OctetString (pParms, 16, (char *) pvalue);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_IP_Address (PERParms * pParms, IP_Address * pvalue)
{
  int status = P_SUCCESS;
  unsigned int    numBytes;

  //status = rrc_PERDec_BitString (pParms,&pvalue->numbits,pvalue->data, sizeof(pvalue->data));
  status = rrc_PERDec_OctetString (pParms, &numBytes, (char *) pvalue, 16);
  return status;
}
#endif

/**************************************************************/
/*  PRACH_SCCPCH_SIList                                     */
/**************************************************************/
//size constraints =  1, 5 --100
#ifdef NODE_RG
int
rrc_PEREnc_PRACH_SCCPCH_SIList (PERParms * pParms, PRACH_SCCPCH_SIList value)
{
  int status = P_SUCCESS;
  if (value.numocts >= 0 && value.numocts <= (maxBlock - 8)) {
    status = rrc_PEREnc_VarOctetString (pParms, value.numocts, value.data);
    if (status != P_SUCCESS)
      return status;
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_PRACH_SCCPCH_SIList (PERParms * pParms, PRACH_SCCPCH_SIList * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_VarOctetString (pParms, &pvalue->numocts, pvalue->data);

  return status;
}
#endif

/**************************************************************/
/*  PrimaryCCPCH_TX_Power                                     */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_PrimaryCCPCH_TX_Power (PERParms * pParms, PrimaryCCPCH_TX_Power value)
{
  int status = P_SUCCESS;

  //  status = rrc_PEREnc_ConstrInteger(pParms, value, 6, 43); specs, but modified for indoor MW, DN april 05
  status = rrc_PEREnc_ConstrInteger (pParms, value, 6, 43);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_PrimaryCCPCH_TX_Power (PERParms * pParms, PrimaryCCPCH_TX_Power * pvalue)
{
  int status = P_SUCCESS;

  //status = rrc_PERDec_ConstrInteger(pParms, pvalue, 6, 43);  specs, but modified for indoor MW, DN april 05
  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 6, 43);

  return status;
}
#endif

/**************************************************************/
/*  Alpha                                                     */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_Alpha (PERParms * pParms, Alpha value)
{
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 8);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_Alpha (PERParms * pParms, Alpha * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 8);

  return status;
}
#endif

/**************************************************************/
/*  ConstantValueTdd                                          */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_ConstantValueTdd (PERParms * pParms, ConstantValueTdd value)
{
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, -35, 10);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_ConstantValueTdd (PERParms * pParms, ConstantValueTdd * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_ConstrInteger (pParms, pvalue, -35, 10);

  return status;
}
#endif

/**************************************************************/
/*  OpenLoopPowerControl_TDD                                  */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_OpenLoopPowerControl_TDD (PERParms * pParms, OpenLoopPowerControl_TDD * pvalue)
{
  int status = P_SUCCESS;

  /* encode primaryCCPCH_TX_Power */
  status = rrc_PEREnc_PrimaryCCPCH_TX_Power (pParms, pvalue->primaryCCPCH_TX_Power);
  if (status != P_SUCCESS)
    return status;

  /* encode alpha */
  status = rrc_PEREnc_Alpha (pParms, pvalue->alpha);
  if (status != P_SUCCESS)
    return status;

  /* encode prach_ConstantValue */
  status = rrc_PEREnc_ConstantValueTdd (pParms, pvalue->prach_ConstantValue);
  if (status != P_SUCCESS)
    return status;

  /* encode dpch_ConstantValue */
  status = rrc_PEREnc_ConstantValueTdd (pParms, pvalue->dpch_ConstantValue);
//   if (status != P_SUCCESS) return status;
//
//   /* encode pusch_ConstantValue */
//   status = rrc_PEREnc_ConstantValueTdd (pParms, pvalue->pusch_ConstantValue);
//   if (status != P_SUCCESS) return status;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_OpenLoopPowerControl_TDD (PERParms * pParms, OpenLoopPowerControl_TDD * pvalue)
{
  int status = P_SUCCESS;

  /* decode primaryCCPCH_TX_Power */
  status = rrc_PERDec_PrimaryCCPCH_TX_Power (pParms, &pvalue->primaryCCPCH_TX_Power);
  if (status != P_SUCCESS)
    return status;

  /* decode alpha */
  status = rrc_PERDec_Alpha (pParms, &pvalue->alpha);
  if (status != P_SUCCESS)
    return status;

  /* decode prach_ConstantValue */
  status = rrc_PERDec_ConstantValueTdd (pParms, &pvalue->prach_ConstantValue);
  if (status != P_SUCCESS)
    return status;

  /* decode dpch_ConstantValue */
  status = rrc_PERDec_ConstantValueTdd (pParms, &pvalue->dpch_ConstantValue);
//   if (status != P_SUCCESS) return status;
//
//   /* decode pusch_ConstantValue */
//   status = rrc_PERDec_ConstantValueTdd (pParms, &pvalue->pusch_ConstantValue);
//   if (status != P_SUCCESS) return status;

  return status;
}
#endif

/**************************************************************/
/*  TimeslotNumber                                            */
/**************************************************************/
////  value >= 0 && value <= 14
#ifdef NODE_RG
int
rrc_PEREnc_TimeslotNumber (PERParms * pParms, TimeslotNumber value)
{
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConsUnsigned (pParms, value, 0, 14);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_TimeslotNumber (PERParms * pParms, TimeslotNumber * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_ConsUnsigned (pParms, (int *) pvalue, 0, 14);

  return status;
}
#endif

/**************************************************************/
/*  TDD_UL_Interference                                       */
/**************************************************************/
//  value >= -110 && value <= -52
#ifdef NODE_RG
int
rrc_PEREnc_TDD_UL_Interference (PERParms * pParms, TDD_UL_Interference value)
{
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConstrInteger (pParms, value, -110, -52);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_TDD_UL_Interference (PERParms * pParms, TDD_UL_Interference * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_ConstrInteger (pParms, (int *) pvalue, -110, -52);

  return status;
}
#endif

/**************************************************************/
/*  IndividualTS_Interference                                 */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_IndividualTS_Interference (PERParms * pParms, IndividualTS_Interference * pvalue)
{
  int status = P_SUCCESS;

  /* encode timeslot */
  status = rrc_PEREnc_TimeslotNumber (pParms, pvalue->timeslot);
  if (status != P_SUCCESS)
    return status;

  /* encode ul_TimeslotInterference */
  status = rrc_PEREnc_TDD_UL_Interference (pParms, pvalue->ul_TimeslotInterference);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_IndividualTS_Interference (PERParms * pParms, IndividualTS_Interference * pvalue)
{
  int status = P_SUCCESS;

  /* decode timeslot */
  status = rrc_PERDec_TimeslotNumber (pParms, &pvalue->timeslot);
  if (status != P_SUCCESS)
    return status;

  /* decode ul_TimeslotInterference */
  status = rrc_PERDec_TDD_UL_Interference (pParms, &pvalue->ul_TimeslotInterference);

  return status;
}
#endif

/**************************************************************/
/*  IndividualTS_InterferenceList                             */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_IndividualTS_InterferenceList (PERParms * pParms, IndividualTS_InterferenceList * pvalue)
{
  // size constraint =  1, 15
  int status = P_SUCCESS;
  int  i;

  /* encode length determinant */
  status = rrc_PEREnc_Length (pParms, pvalue->numSlots);
  if (status < 0)
    return status;

  /* encode valued data */
  for (i = 0; i < pvalue->numSlots; i++) {
    status = rrc_PEREnc_IndividualTS_Interference (pParms, &pvalue->data[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_IndividualTS_InterferenceList (PERParms * pParms, IndividualTS_InterferenceList * pvalue)
{
  // size constraint =  1, 15
  int status = P_SUCCESS;
  int  i;

  /* decode length determinant */
  status = rrc_PERDec_Length (pParms, (int *) &pvalue->numSlots);
  if (status != P_SUCCESS)
    return status;

  /* decode valued data */
  for (i = 0; i < pvalue->numSlots; i++) {
    status = rrc_PERDec_IndividualTS_Interference (pParms, &pvalue->data[i]);
    if (status != P_SUCCESS)
      return status;
  }
  return status;
}
#endif

/**************************************************************/
/*  ExpirationTimeFactor                                      */
/**************************************************************/
#ifdef NODE_RG
int
rrc_PEREnc_ExpirationTimeFactor (PERParms * pParms, ExpirationTimeFactor value)
{
  int status = P_SUCCESS;

  status = rrc_PEREnc_ConsUnsigned (pParms, value, 1, 8);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_ExpirationTimeFactor (PERParms * pParms, ExpirationTimeFactor * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_ConsUnsigned (pParms, pvalue, 1, 8);

  return status;
}
#endif


/**************************************************************/
/*  IdentitiesOfNeighbourCells                                */
/**************************************************************/
//size constraints =  1, 8 -- maxSIBNAS --
#ifdef NODE_RG
int
rrc_PEREnc_IdentitiesOfNeighbourCells (PERParms * pParms, IdentitiesOfNeighbourCells value)
{
  int status = P_SUCCESS;
  if (value.numocts >= 0 && value.numocts <= maxSIBNAS) {
    status = rrc_PEREnc_VarOctetString (pParms, value.numocts, value.data);
    if (status != P_SUCCESS)
      return status;
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}
#endif

//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_IdentitiesOfNeighbourCells (PERParms * pParms, IdentitiesOfNeighbourCells * pvalue)
{
  int status = P_SUCCESS;

  status = rrc_PERDec_VarOctetString (pParms, &pvalue->numocts, pvalue->data);
  return status;
}
#endif

/**************************************************************/
/*  CodeGroupsOfNeighbourCells                                */
/**************************************************************/
//size = (maxCells) octets
#ifdef NODE_RG
int
rrc_PEREnc_CodeGroupsOfNeighbourCells (PERParms * pParms, CodeGroupsOfNeighbourCells * pvalue)
{
  int status = P_SUCCESS;

  //status = rrc_PEREnc_BitString (pParms, value.numbits, value.data);
  status = rrc_PEREnc_OctetString (pParms, maxCells, (char *) pvalue);
  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int
rrc_PERDec_CodeGroupsOfNeighbourCells (PERParms * pParms, CodeGroupsOfNeighbourCells * pvalue)
{
  int status = P_SUCCESS;
  unsigned int    numBytes;

  //status = rrc_PERDec_BitString (pParms,&pvalue->numbits,pvalue->data, sizeof(pvalue->data));
  status = rrc_PERDec_OctetString (pParms, &numBytes, (char *) pvalue, maxCells);
  return status;
}
#endif
