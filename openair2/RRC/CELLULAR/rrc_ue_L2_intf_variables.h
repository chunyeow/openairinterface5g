/***************************************************************************
                          rrc_ue_L2_intf_variables.h  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Some additional definitions for UE broadcast
 ***************************************************************************/
#ifndef __RRC_UE_L2INTF_H__
#define __RRC_UE_L2INTF_H__

#include "SystemInformationBlockType2.h"
#include "SystemInformation.h"
#include "BCCH-DL-SCH-Message.h"
#include "TDD-Config.h"

// RRC LTE standard asn1 compliant block
struct rrc_ue_bch_asn1 {
  u8 sizeof_SIB2;
  u8 SIB23[128];
  //BCCH_DL_SCH_Message_t siblock1;
  BCCH_DL_SCH_Message_t systemInformation;
  //SystemInformationBlockType1_t *sib1;
  SystemInformationBlockType2_t *sib2;
  //SystemInformationBlockType3_t *sib3;
  TDD_Config_t tdd_Config;
};

/* Version RRC LITE !!!! RG
struct rrc_rg_srb_drb_asn1{
  struct SRB_ToAddMod             *SRB1_config[NB_CNX_eNB];
  struct SRB_ToAddMod             *SRB2_config[NB_CNX_eNB];
  struct DRB_ToAddMod             *DRB_config[NB_CNX_eNB][8];
  u8                               DRB_active[NB_CNX_eNB][8];
  struct PhysicalConfigDedicated  *physicalConfigDedicated[NB_CNX_eNB];
  struct SPS_Config               *sps_Config[NB_CNX_eNB];
  MAC_MainConfig_t                *mac_MainConfig[NB_CNX_eNB];
  MeasGapConfig_t                 *measGapConfig[NB_CNX_eNB];
};
*/

struct rrc_srb_drb_asn1{
/*  struct SRB_ToAddMod             *SRB1_config;
  struct SRB_ToAddMod             *SRB2_config;
  struct DRB_ToAddMod             *DRB1_config;
  struct DRB_ToAddMod             *DRB2_config;*/
  int DRB1_active;
  int DRB2_active;
  SRB_ToAddModList_t   *SRB_configList[NUMBER_OF_UE_MAX];
  DRB_ToAddModList_t   *DRB_configList[NUMBER_OF_UE_MAX];

  struct PhysicalConfigDedicated  *physicalConfigDedicated;
  //struct SPS_Config               *sps_Config[NB_CNX_eNB];
  MAC_MainConfig_t                *mac_MainConfig;
  MeasGapConfig_t                 *measGapConfig;
  /* Logical channel config */
  LogicalChannelConfig_t     *SRB1_logicalChannelConfig;
  LogicalChannelConfig_t     *SRB2_logicalChannelConfig;
};




#endif
