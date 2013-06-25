/***************************************************************************
                          rrc_rg_bch_init.c  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to call initial encoding of SIBs
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
#include "rrc_proto_int.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_bch.h"

//-----------------------------------------------------------------------------
void rrc_rg_init_bch (void){
//-----------------------------------------------------------------------------
  protocol_bs->rrc.rg_bch_blocks.sib_list[0] = systemInformationBlockType1;
  protocol_bs->rrc.rg_bch_blocks.sib_list[1] = systemInformationBlockType2;
  protocol_bs->rrc.rg_bch_blocks.sib_list[2] = systemInformationBlockType5;
  protocol_bs->rrc.rg_bch_blocks.sib_list[3] = systemInformationBlockType11;
  protocol_bs->rrc.rg_bch_blocks.sib_list[4] = systemInformationBlockType14;
  protocol_bs->rrc.rg_bch_blocks.sib_list[5] = systemInformationBlockType18;
  protocol_bs->rrc.rg_bch_blocks.SIB1_oneTimeShot = FALSE;
  protocol_bs->rrc.rg_bch_blocks.SIB14_timeout = (2 << SIB14_REP) * SIB14_TFACTOR;
}

//-----------------------------------------------------------------------------
void rrc_fill_mib (void){
//-----------------------------------------------------------------------------
  MasterInformationBlock *pmib;

  pmib = &(protocol_bs->rrc.rg_bch_blocks.currMIB);
  pmib->mib_ValueTag = 0;

  // hard coded in first step
  pmib->plmn_Identity.mcc.numDigits = 3;
  pmib->plmn_Identity.mcc.elem[0] = 1;
  pmib->plmn_Identity.mcc.elem[1] = 4;
  pmib->plmn_Identity.mcc.elem[2] = 9;
  pmib->plmn_Identity.mnc = protocol_bs->rrc.rg_cell_id;
//  pmib->plmn_Identity.mnc.numDigits=2;
//  pmib->plmn_Identity.mnc.elem[0]=5;
//  pmib->plmn_Identity.mnc.elem[1]=8;

  // initialize with default value
  pmib->sibSb_ReferenceList.numSIB = supportedSIBs;     // number SIBs supported
  pmib->sibSb_ReferenceList.sib_ref[0].sibSb_Type.type = SIBSb_sysInfoType1;
  pmib->sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1 = 0;
  pmib->sibSb_ReferenceList.sib_ref[0].scheduling.scheduling_sib_rep.sib_Rep = SIB1_REP;
  pmib->sibSb_ReferenceList.sib_ref[0].scheduling.scheduling_sib_rep.sib_Pos = SIB1_POS;
  pmib->sibSb_ReferenceList.sib_ref[1].sibSb_Type.type = SIBSb_sysInfoType2;
  pmib->sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2 = 0;
  pmib->sibSb_ReferenceList.sib_ref[1].scheduling.scheduling_sib_rep.sib_Rep = SIB2_REP;
  pmib->sibSb_ReferenceList.sib_ref[1].scheduling.scheduling_sib_rep.sib_Pos = SIB2_POS;
  pmib->sibSb_ReferenceList.sib_ref[2].sibSb_Type.type = SIBSb_sysInfoType5;
  pmib->sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5 = 0;
  pmib->sibSb_ReferenceList.sib_ref[2].scheduling.scheduling_sib_rep.sib_Rep = SIB5_REP;
  pmib->sibSb_ReferenceList.sib_ref[2].scheduling.scheduling_sib_rep.sib_Pos = SIB5_POS;
  pmib->sibSb_ReferenceList.sib_ref[3].sibSb_Type.type = SIBSb_sysInfoType11;
  pmib->sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11 = 0;
  pmib->sibSb_ReferenceList.sib_ref[3].scheduling.scheduling_sib_rep.sib_Rep = SIB11_REP;
  pmib->sibSb_ReferenceList.sib_ref[3].scheduling.scheduling_sib_rep.sib_Pos = SIB11_POS;
  pmib->sibSb_ReferenceList.sib_ref[4].sibSb_Type.type = SIBSb_sysInfoType14;
  //no value_tag for SIB14
  pmib->sibSb_ReferenceList.sib_ref[4].scheduling.scheduling_sib_rep.sib_Rep = SIB14_REP;
  pmib->sibSb_ReferenceList.sib_ref[4].scheduling.scheduling_sib_rep.sib_Pos = SIB14_POS;
  pmib->sibSb_ReferenceList.sib_ref[5].sibSb_Type.type = SIBSb_sysInfoType18;
  pmib->sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18 = 0;
  pmib->sibSb_ReferenceList.sib_ref[5].scheduling.scheduling_sib_rep.sib_Rep = SIB18_REP;
  pmib->sibSb_ReferenceList.sib_ref[5].scheduling.scheduling_sib_rep.sib_Pos = SIB18_POS;

}

//-----------------------------------------------------------------------------
void rrc_init_mib (void){
//-----------------------------------------------------------------------------
  int value_tag;
  // Prepare
  int status = P_SUCCESS;
  MasterInformationBlock *pMIB = &(protocol_bs->rrc.rg_bch_blocks.currMIB);
  PERParms *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
  char *pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currMIB);

  rrc_new_per_parms (pParms, pBuffer);
  // Update value_tag
  value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.mib_ValueTag;
  value_tag = (++value_tag) % 8;
  protocol_bs->rrc.rg_bch_blocks.currMIB.mib_ValueTag = value_tag;

  // Encode MIB
  status = rrc_PEREnc_MasterInformationBlock (pParms, pMIB);

#ifdef DEBUG_RRC_BROADCAST
  msg ("\n[RRC_BCH] Init MIB result - status : %d\n", status);
  rrc_print_per_parms (pParms);
#endif
}

//-----------------------------------------------------------------------------
void rrc_fill_sib1 (void){
//-----------------------------------------------------------------------------
  // hard coded in first step
  char            bufferNAS[] = "NoData in SIB1";
  char            bufferTimer[] = "KLMNOPQR";

  protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.numocts = strlen (bufferNAS);
  memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data, bufferNAS, strlen (bufferNAS));
//     protocol_bs->rrc.rg_bch_blocks.currSIB1.subnet_NAS_SysInfo.data[1]= "a";

  protocol_bs->rrc.rg_bch_blocks.currSIB1.ue_TimersAndConstants.numocts = 8;
  memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB1.ue_TimersAndConstants.data, bufferTimer, 8);
}

//-----------------------------------------------------------------------------
void rrc_update_SIB1_period (int period){
//-----------------------------------------------------------------------------
  MasterInformationBlock *pmib = &(protocol_bs->rrc.rg_bch_blocks.currMIB);

  if (period == 0) {
    protocol_bs->rrc.rg_bch_blocks.SIB1_oneTimeShot = TRUE;
  } else {
    if (period < 192) {
      pmib->sibSb_ReferenceList.sib_ref[0].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep128;
    } else {
      if (period < 320) {
        pmib->sibSb_ReferenceList.sib_ref[0].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep256;
      } else {
        pmib->sibSb_ReferenceList.sib_ref[0].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep512;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void rrc_init_sib1 (void){
//-----------------------------------------------------------------------------
  int value_tag;
  // Prepare
  int status = P_SUCCESS;
  SysInfoType1 *pSIB1 = &(protocol_bs->rrc.rg_bch_blocks.currSIB1);
  PERParms *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
  char *pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB1);

  rrc_new_per_parms (pParms, pBuffer);
  // Encode
  status = rrc_PEREnc_SysInfoType1 (pParms, pSIB1);
#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB1 result - status : %d\n", status);
  rrc_print_per_parms (pParms);
#endif
  // Update value_tag and MIB
  value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1;
  value_tag = (++value_tag) % 256;
  protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].sibSb_Type.type_tag.sysInfoType1 = value_tag;
  protocol_bs->rrc.rg_bch_blocks.encodedSIB1_lgth = pParms->buff_size;
  if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB1_lgth % LSIBfixed) == 0)
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB1_lgth / LSIBfixed);
  else
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[0].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB1_lgth / LSIBfixed) + 1;

  rrc_init_mib ();
}

//-----------------------------------------------------------------------------
void rrc_fill_sib2 (void){
//-----------------------------------------------------------------------------
//  // hard coded in first step
//  int i;
//  char IPAddr[]  = "1234123412341234";
//  char a;
//
//  for (i=0;i<16;i++){
//     a= IPAddr[i];
//     protocol_bs->rrc.rg_bch_blocks.currSIB2.net_IP_addr.data[i]= a;
//  }
}

//-----------------------------------------------------------------------------
void rrc_init_sib2 (void){
//-----------------------------------------------------------------------------
// int value_tag;
//  // Prepare
  int             status = P_SUCCESS;
// SysInfoType2* pSIB2 = &(protocol_bs->rrc.rg_bch_blocks.currSIB2);
// PERParms* pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
// char * pBuffer = (char *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB2);
// rrc_new_per_parms (pParms,  pBuffer);
//  // Encode
// status = rrc_PEREnc_SysInfoType2 (pParms, pSIB2);
//
#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB2 result (removed) - status : %d\n\n", status);
// rrc_print_per_parms(pParms);
#endif
//  // Update value_tag and MIB
// value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2;
// value_tag = (++ value_tag)%4;
// protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].sibSb_Type.type_tag.sysInfoType2
//           = value_tag;
// protocol_bs->rrc.rg_bch_blocks.encodedSIB2_lgth = pParms->buff_size;
// if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB2_lgth % LSIBfixed) ==0)
//    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].scheduling.segCount
//        =  (protocol_bs->rrc.rg_bch_blocks.encodedSIB2_lgth / LSIBfixed);
// else
//    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[1].scheduling.segCount
//        =  (protocol_bs->rrc.rg_bch_blocks.encodedSIB2_lgth / LSIBfixed) + 1;
// rrc_init_mib();
}

//-----------------------------------------------------------------------------
void rrc_fill_sib5 (void){
//-----------------------------------------------------------------------------
  /*
     u8 rachfach_conf[]="I'm not thinking about retiring at this point, so my focus is on coming back and figuring out how to get back here. \
     I don't think anybody's going to question why I'm coming back next year.";
     //size 195
   */
  //   u8 rachfach_conf[]="I'm not thinking about retiring at this point.";
     //size 45 - TEMP : SIB5 content to be revised
  int config_length;
  char* config_ptr=NULL;
  u8 error_message[] = "ERROR-SIB5";

  // Fill SCCPCH-PRACH config
  //protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts=strlen(rachfach_conf);
  //protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts = sizeof (SCCPCH_SYSTEM_INFO) + sizeof (PRACH_SYSTEM_INFO);
  rrc_rg_get_common_config_SIB(&config_length, &config_ptr);
  #ifdef DEBUG_RRC_DETAILS
  //msg ("\n[RRC-RG] rrc_fill_sib5 config pointer  %p \n", config_ptr);
  #endif

  protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts = config_length;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC] Fill SIB5 : config length = %d\n", protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts);
  #endif

  if (protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts <= maxBlock - 8) {
    // For test only
    // memcpy(protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.data,rachfach_conf,
    //           protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts);
    //memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.data, (char *) &(rrm_config->sccpch), sizeof (SCCPCH_SYSTEM_INFO) + sizeof (PRACH_SYSTEM_INFO));
     memcpy(protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.data, config_ptr, config_length);

  } else {
    msg ("[RRC] Fill SIB5 : config length too long \n\n");
    protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts = strlen (error_message);
    memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.data, error_message, protocol_bs->rrc.rg_bch_blocks.currSIB5.prach_sCCPCH_SIList.numocts);
  }

  // Fill OpenLoopPowerControl_TDD with initial values
  protocol_bs->rrc.rg_bch_blocks.currSIB5.openLoopPowerControl_TDD.primaryCCPCH_TX_Power 
  // = rrm_config->outer_loop_vars.PCCPCH_POWER;
      = 25;
  protocol_bs->rrc.rg_bch_blocks.currSIB5.openLoopPowerControl_TDD.alpha 
  // = rrm_config->outer_loop_vars.alpha;
     = 4;
  protocol_bs->rrc.rg_bch_blocks.currSIB5.openLoopPowerControl_TDD.prach_ConstantValue 
  // = rrm_config->outer_loop_vars.PRACH_CNST;
     = -13;
  protocol_bs->rrc.rg_bch_blocks.currSIB5.openLoopPowerControl_TDD.dpch_ConstantValue 
  // = rrm_config->outer_loop_vars.DPCH_CNST;
     = 5;
}

//-----------------------------------------------------------------------------
void rrc_init_sib5 (void){
//-----------------------------------------------------------------------------
  int             value_tag;
  // Prepare
  int             status = P_SUCCESS;
  SysInfoType5   *pSIB5 = &(protocol_bs->rrc.rg_bch_blocks.currSIB5);
  PERParms       *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
  char           *pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB5);
  rrc_new_per_parms (pParms, pBuffer);
  // Encode
  status = rrc_PEREnc_SysInfoType5 (pParms, pSIB5);

#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB5 result - status : %d\n", status);
  rrc_print_per_parms (pParms);
#endif
  if (status != 0){
    msg ("\n\n[RRC_BCH] Init SIB5 result - WCDMA_ERROR_RRC_NASTY_BCH_CONFIG : %d\n\n", status);
    //wcdma_handle_error (WCDMA_ERROR_RRC_NASTY_BCH_CONFIG);
  }
  // Update value_tag and MIB
  value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5;
  value_tag = (++value_tag) % 4;
  protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].sibSb_Type.type_tag.sysInfoType5 = value_tag;
  protocol_bs->rrc.rg_bch_blocks.encodedSIB5_lgth = pParms->buff_size;
  if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB5_lgth % LSIBfixed) == 0)
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB5_lgth / LSIBfixed);
  else
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[2].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB5_lgth / LSIBfixed) + 1;

  rrc_init_mib ();
}

//-----------------------------------------------------------------------------
void rrc_fill_sib11 (void){
//-----------------------------------------------------------------------------
//  // hard coded in first step
// u8 rachfach_conf[]= "6666666666777777777700000000008888888888999999999912121212121212121212343434343434343434345555555555";
//
//  protocol_bs->rrc.rg_bch_blocks.currSIB11.prach_sCCPCH_SIList.numocts=98;
////  for (i=0;i<35;i++)
////     protocol_bs->rrc.rg_bch_blocks.currSIB11.ue_TimersAndConstants.data[i]=bufferTimer[i];
//  memcpy(protocol_bs->rrc.rg_bch_blocks.currSIB11.prach_sCCPCH_SIList.data,rachfach_conf,
//           protocol_bs->rrc.rg_bch_blocks.currSIB11.prach_sCCPCH_SIList.numocts);
}

//-----------------------------------------------------------------------------
void rrc_init_sib11 (void){
//-----------------------------------------------------------------------------
// int value_tag;
  // Prepare
  int             status = P_SUCCESS;
// SysInfoType11* pSIB11 = &(protocol_bs->rrc.rg_bch_blocks.currSIB11);
// PERParms* pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
// char * pBuffer = (char *) & (protocol_bs->rrc.rg_bch_blocks.encoded_currSIB11);
// rrc_new_per_parms (pParms,  pBuffer);
//  // Encode
// status = rrc_PEREnc_SysInfoType11 (pParms, pSIB11);

#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB11 result (not implemented yet)- status : %d\n\n", status);
  //rrc_print_per_parms(pParms);
#endif
  // Update value_tag and MIB
// value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11;
// value_tag = (++ value_tag)%4;
// protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].sibSb_Type.type_tag.sysInfoType11
//           = value_tag;
// protocol_bs->rrc.rg_bch_blocks.encodedSIB11_lgth = pParms->buff_size;
// if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB11_lgth % LSIBfixed) ==0)
//    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].scheduling.segCount
//        =  (protocol_bs->rrc.rg_bch_blocks.encodedSIB11_lgth / LSIBfixed);
// else
//    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[3].scheduling.segCount
//        =  (protocol_bs->rrc.rg_bch_blocks.encodedSIB11_lgth / LSIBfixed) + 1;
// rrc_init_mib();
}

//-----------------------------------------------------------------------------
void rrc_fill_sib14 (void){
//-----------------------------------------------------------------------------
  int             i = 0;
  SysInfoType14  *psib14 = &(protocol_bs->rrc.rg_bch_blocks.currSIB14);

  psib14->expirationTimeFactor = SIB14_TFACTOR;
  psib14->individualTS_InterferenceList.numSlots = 15;
  for (i = 0; i < psib14->individualTS_InterferenceList.numSlots; i++) {
    psib14->individualTS_InterferenceList.data[i].timeslot = i;
  }
/*  **** REMOVED FOR OPENAIR ****
  for (i = 0; i < psib14->individualTS_InterferenceList.numSlots; i++) {
//    psib14->individualTS_InterferenceList.data[i].ul_TimeslotInterference = rrm_config->outer_loop_vars.IBTS[i];
    psib14->individualTS_InterferenceList.data[i].ul_TimeslotInterference = (5*i)-110;
  }
//msg(" *****Trace fill sib14 rrm_config->outer_loop_vars.IBTS[1]= %d; \n",rrm_config->outer_loop_vars.IBTS[1]);
*/
//For test
  psib14->individualTS_InterferenceList.data[0].ul_TimeslotInterference=-110;
  psib14->individualTS_InterferenceList.data[1].ul_TimeslotInterference=-105;
  psib14->individualTS_InterferenceList.data[2].ul_TimeslotInterference=-100;
  psib14->individualTS_InterferenceList.data[3].ul_TimeslotInterference=-95;
  psib14->individualTS_InterferenceList.data[4].ul_TimeslotInterference=-90;
  psib14->individualTS_InterferenceList.data[5].ul_TimeslotInterference=-85;
  psib14->individualTS_InterferenceList.data[6].ul_TimeslotInterference=-80;
  psib14->individualTS_InterferenceList.data[7].ul_TimeslotInterference=-75;
  psib14->individualTS_InterferenceList.data[8].ul_TimeslotInterference=-70;
  psib14->individualTS_InterferenceList.data[9].ul_TimeslotInterference=-65;
  psib14->individualTS_InterferenceList.data[10].ul_TimeslotInterference=-60;
  psib14->individualTS_InterferenceList.data[11].ul_TimeslotInterference=-55;
  psib14->individualTS_InterferenceList.data[12].ul_TimeslotInterference=-100;
  psib14->individualTS_InterferenceList.data[13].ul_TimeslotInterference=-75;
  psib14->individualTS_InterferenceList.data[14].ul_TimeslotInterference=-52;
}

//-----------------------------------------------------------------------------
void rrc_init_sib14 (void){
//-----------------------------------------------------------------------------
  // Prepare
  int             status = P_SUCCESS;
  SysInfoType14  *pSIB14 = &(protocol_bs->rrc.rg_bch_blocks.currSIB14);
  PERParms       *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
  char           *pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB14);
  rrc_new_per_parms (pParms, pBuffer);
  // Encode
  status = rrc_PEREnc_SysInfoType14 (pParms, pSIB14);

#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB14 result - status : %d\n", status);
  rrc_print_per_parms (pParms);
#endif
  // value_tag not updated - controlled by time-out
  protocol_bs->rrc.rg_bch_blocks.encodedSIB14_lgth = pParms->buff_size;
  if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB14_lgth % LSIBfixed) == 0)
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[4].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB14_lgth / LSIBfixed);
  else
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[4].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB14_lgth / LSIBfixed) + 1;
  rrc_init_mib ();
}

//-----------------------------------------------------------------------------
void rrc_fill_sib18 (void){
//-----------------------------------------------------------------------------
// warning SIB18 currently a short SIB ==> length < 110 bytes
/*
   u8 cells_list[]="This is the initial setting of cell list = dummy.";
   //size 48
*/
  u8              cells_list[] = "NoData in SIB18";
  u8              error_message[] = "ERROR-SIB18";

  protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts = strlen (cells_list);
  if (protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts <= 110) {
    memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data, cells_list, protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts);
  } else {
    protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts = strlen (error_message);
    memcpy (protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.data, error_message, protocol_bs->rrc.rg_bch_blocks.currSIB18.cellIdentities.numocts);
  }

  //CodeGroups currently hard-coded for 2 cells
  protocol_bs->rrc.rg_bch_blocks.currSIB18.cellCodegroups.data[0] = 0;
  protocol_bs->rrc.rg_bch_blocks.currSIB18.cellCodegroups.data[1] = 14;

}


//-----------------------------------------------------------------------------
void rrc_init_sib18 (void){
//-----------------------------------------------------------------------------
  int             value_tag;
  // Prepare
  int             status = P_SUCCESS;

  SysInfoType18  *pSIB18 = &(protocol_bs->rrc.rg_bch_blocks.currSIB18);
  PERParms       *pParms = &(protocol_bs->rrc.rg_bch_blocks.perParms);
  char           *pBuffer = (char *) &(protocol_bs->rrc.rg_bch_blocks.encoded_currSIB18);
  rrc_new_per_parms (pParms, pBuffer);
  // Encode
  status = rrc_PEREnc_SysInfoType18 (pParms, pSIB18);
#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] Init SIB18 result - status : %d\n", status);
  rrc_print_per_parms (pParms);
#endif
  // Update value_tag and MIB
  value_tag = protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18;
  value_tag = (++value_tag) % 4;
  protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].sibSb_Type.type_tag.sysInfoType18 = value_tag;
  protocol_bs->rrc.rg_bch_blocks.encodedSIB18_lgth = pParms->buff_size;
  if ((protocol_bs->rrc.rg_bch_blocks.encodedSIB18_lgth % LSIBfixed) == 0)
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB18_lgth / LSIBfixed);
  else
    protocol_bs->rrc.rg_bch_blocks.currMIB.sibSb_ReferenceList.sib_ref[5].scheduling.segCount = (protocol_bs->rrc.rg_bch_blocks.encodedSIB18_lgth / LSIBfixed) + 1;
  rrc_init_mib ();
}


//-----------------------------------------------------------------------------
void rrc_update_SIB18_period (int period){
//-----------------------------------------------------------------------------
  MasterInformationBlock *pmib = &(protocol_bs->rrc.rg_bch_blocks.currMIB);

  if (period < 150) {
    pmib->sibSb_ReferenceList.sib_ref[5].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep128;
  } else {
    if (period < 320) {
      pmib->sibSb_ReferenceList.sib_ref[5].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep256;
    } else {
      pmib->sibSb_ReferenceList.sib_ref[5].scheduling.scheduling_sib_rep.sib_Rep = SchedulingInformation_scheduling_rep512;
    }
  }
}


//-----------------------------------------------------------------------------
void rrc_init_blocks (void){
//-----------------------------------------------------------------------------
  // read configuration file and create MIB/SIBs data
  rrc_fill_mib ();
  rrc_fill_sib1 ();
  rrc_fill_sib2 ();
  rrc_fill_sib5 ();
  rrc_fill_sib11 ();
  rrc_fill_sib14 ();
  rrc_fill_sib18 ();

  // encode MIB/SIBs
  rrc_init_sib1 ();
  rrc_init_sib2 ();
  rrc_init_sib5 ();
  rrc_init_sib11 ();
  rrc_init_sib14 ();
  rrc_init_sib18 ();
//  rrc_init_mib();
  protocol_bs->rrc.rg_bch_blocks.curr_block_type = NO_BLOCK;
  protocol_bs->rrc.rg_bch_blocks.next_block_type = NO_BLOCK;
}
