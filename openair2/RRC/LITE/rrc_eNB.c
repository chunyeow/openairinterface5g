/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/


/*! \file rrc_eNB.c
 * \brief rrc procedures for eNB
 * \author Raymond Knopp and Navid Nikaein
 * \date 2011
 * \version 1.0
 * \company Eurecom
 * \email: raymond.knopp@eurecom.fr and navid.nikaein@eurecom.fr
 */

#include "defs.h"
#include "extern.h"
#include "assertions.h"
#include "asn1_conversions.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
#include "UTIL/LOG/log.h"
#include "COMMON/mac_rrc_primitives.h"
#include "RRC/LITE/MESSAGES/asn1_msg.h"
#include "RRCConnectionRequest.h"
#include "UL-CCCH-Message.h"
#include "DL-CCCH-Message.h"
#include "UL-DCCH-Message.h"
#include "DL-DCCH-Message.h"
#include "TDD-Config.h"
#include "HandoverCommand.h"
#include "rlc.h"
#include "SIMULATION/ETH_TRANSPORT/extern.h"

//#ifdef Rel10
#include "MeasResults.h"
//#endif

#ifdef USER_MODE
# include "RRC/NAS/nas_config.h"
# include "RRC/NAS/rb_config.h"
# include "OCG.h"
# include "OCG_extern.h"
#endif

#if defined(ENABLE_SECURITY)
# include "UTIL/OSA/osa_defs.h"
#endif

#if defined(ENABLE_USE_MME)
# include "rrc_eNB_S1AP.h"
# if defined(ENABLE_ITTI)
# else
#   include "../../S1AP/s1ap_eNB.h"
# endif
#endif

#include "pdcp.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

//#define XER_PRINT

#ifdef PHY_EMUL
extern EMULATION_VARS *Emul_vars;
#endif
extern eNB_MAC_INST *eNB_mac_inst;
extern UE_MAC_INST *UE_mac_inst;
#ifdef BIGPHYSAREA
extern void *bigphys_malloc (int);
#endif

extern uint16_t two_tier_hexagonal_cellIds[7];
extern inline unsigned int taus (void);

/* TS 36.331: RRC-TransactionIdentifier ::= INTEGER (0..3) */
static const uint8_t RRC_TRANSACTION_IDENTIFIER_NUMBER = 4;

mui_t rrc_eNB_mui = 0;

/*------------------------------------------------------------------------------*/
static void init_SI (u8 Mod_id
#if defined(ENABLE_ITTI)
                   , RrcConfigurationReq *configuration
#endif
                    ) {
  u8 SIwindowsize = 1;
  u16 SIperiod = 8;
#ifdef Rel10
  int i;
#endif
  /*
     uint32_t mib=0;
     int i;
     int N_RB_DL,phich_resource;


     do_MIB(mac_xface->lte_frame_parms,0x321,&mib);

     for (i=0;i<1024;i+=4)
     do_MIB(mac_xface->lte_frame_parms,i,&mib);

     N_RB_DL=6;
     while (N_RB_DL != 0) {
     phich_resource = 1;
     while (phich_resource != 0) {
     for (i=0;i<2;i++) {
     mac_xface->lte_frame_parms->N_RB_DL = N_RB_DL;
     mac_xface->lte_frame_parms->phich_config_common.phich_duration=i;
     mac_xface->lte_frame_parms->phich_config_common.phich_resource = phich_resource;
     do_MIB(mac_xface->lte_frame_parms,0,&mib);
     }
     if (phich_resource == 1)
     phich_resource = 3;
     else if (phich_resource == 3)
     phich_resource = 6;
     else if (phich_resource == 6)
     phich_resource = 12;
     else if (phich_resource == 12)
     phich_resource = 0;
     }
     if (N_RB_DL == 6)
     N_RB_DL = 15;
     else if (N_RB_DL == 15)
     N_RB_DL = 25;
     else if (N_RB_DL == 25)
     N_RB_DL = 50;
     else if (N_RB_DL == 50)
     N_RB_DL = 75;
     else if (N_RB_DL == 75)
     N_RB_DL = 100;
     else if (N_RB_DL == 100)
     N_RB_DL = 0;
     }
     exit(-1);
   */

  eNB_rrc_inst[Mod_id].sizeof_SIB1 = 0;
  eNB_rrc_inst[Mod_id].sizeof_SIB23 = 0;

  eNB_rrc_inst[Mod_id].SIB1 = (u8 *) malloc16 (32);

  /*
     printf ("before SIB1 init : Nid_cell %d\n", mac_xface->lte_frame_parms->Nid_cell);
     printf ("before SIB1 init : frame_type %d,tdd_config %d\n",
     mac_xface->lte_frame_parms->frame_type,
     mac_xface->lte_frame_parms->tdd_config);
   */

  if (eNB_rrc_inst[Mod_id].SIB1)
    eNB_rrc_inst[Mod_id].sizeof_SIB1 = do_SIB1 (mac_xface->lte_frame_parms,
                                                (uint8_t *)
                                                eNB_rrc_inst[Mod_id].SIB1,
                                                &eNB_rrc_inst[Mod_id].
                                                siblock1,
                                                &eNB_rrc_inst[Mod_id].sib1
#if defined(ENABLE_ITTI)
                                              , configuration
#endif
                                                );
  else
    {
      LOG_E (RRC, "[eNB %d] init_SI: FATAL, no memory for SIB1 allocated\n", Mod_id);
      mac_xface->macphy_exit ("[RRC][init_SI] FATAL, no memory for SIB1 allocated");
    }
  /*
     printf ("after SIB1 init : Nid_cell %d\n", mac_xface->lte_frame_parms->Nid_cell);
     printf ("after SIB1 init : frame_type %d,tdd_config %d\n",
     mac_xface->lte_frame_parms->frame_type,
     mac_xface->lte_frame_parms->tdd_config);
   */
    if (eNB_rrc_inst[Mod_id].sizeof_SIB1 == 255)
        mac_xface->macphy_exit ("[RRC][init_SI] FATAL, eNB_rrc_inst[Mod_id].sizeof_SIB1 == 255");

    eNB_rrc_inst[Mod_id].SIB23 = (u8 *) malloc16 (64);
    if (eNB_rrc_inst[Mod_id].SIB23) {
        eNB_rrc_inst[Mod_id].sizeof_SIB23 = do_SIB23 (Mod_id,
                                                      mac_xface->
                                                      lte_frame_parms,
                                                      eNB_rrc_inst[Mod_id].SIB23,
                                                      &eNB_rrc_inst[Mod_id].systemInformation,
                                                      &eNB_rrc_inst[Mod_id].sib2,
                                                      &eNB_rrc_inst[Mod_id].sib3
#ifdef Rel10
                                                      ,
                                                      &eNB_rrc_inst[Mod_id].sib13,
                                                      eNB_rrc_inst[Mod_id].MBMS_flag
#endif
                                                      );
      /*
         eNB_rrc_inst[Mod_id].sizeof_SIB23 = do_SIB2_AT4(Mod_id,
         eNB_rrc_inst[Mod_id].SIB23,
         &eNB_rrc_inst[Mod_id].systemInformation,
         &eNB_rrc_inst[Mod_id].sib2);
       */
      if (eNB_rrc_inst[Mod_id].sizeof_SIB23 == 255)
          mac_xface->macphy_exit ("[RRC][init_SI] FATAL, eNB_rrc_inst[Mod_id].sizeof_SIB23 == 255");

      LOG_T (RRC, "[eNB %d] SIB2/3 Contents (partial)\n", Mod_id);
      LOG_T (RRC, "[eNB %d] pusch_config_common.n_SB = %ld\n", Mod_id,
             eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.pusch_ConfigBasic.n_SB);
      LOG_T (RRC, "[eNB %d] pusch_config_common.hoppingMode = %ld\n", Mod_id,
             eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.pusch_ConfigBasic.hoppingMode);
      LOG_T (RRC, "[eNB %d] pusch_config_common.pusch_HoppingOffset = %ld\n",
             Mod_id,
             eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.pusch_ConfigBasic.pusch_HoppingOffset);
      LOG_T (RRC, "[eNB %d] pusch_config_common.enable64QAM = %d\n", Mod_id,
             (int) eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.pusch_ConfigBasic.enable64QAM);
      LOG_T (RRC, "[eNB %d] pusch_config_common.groupHoppingEnabled = %d\n",
             Mod_id,
             (int) eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupHoppingEnabled);
      LOG_T (RRC, "[eNB %d] pusch_config_common.groupAssignmentPUSCH = %ld\n",
             Mod_id,
             eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.
             groupAssignmentPUSCH);
      LOG_T (RRC,
             "[eNB %d] pusch_config_common.sequenceHoppingEnabled = %d\n",
             Mod_id,
             (int) eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.
             sequenceHoppingEnabled);
      LOG_T (RRC, "[eNB %d] pusch_config_common.cyclicShift  = %ld\n", Mod_id,
             eNB_rrc_inst[Mod_id].sib2->radioResourceConfigCommon.
             pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.cyclicShift);

#ifdef Rel10
    if (eNB_rrc_inst[Mod_id].MBMS_flag > 0) {
        for (i=0; 
             i< eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.count;
             i++){
            // SIB 2
            //   LOG_D(RRC, "[eNB %d] mbsfn_SubframeConfigList.list.count = %ld\n", Mod_id, eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.count);
            LOG_D (RRC, "[eNB %d] SIB13 contents for MBSFN subframe allocation %d/%d(partial)\n",
                   Mod_id,i,eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.count);
            LOG_D (RRC, "[eNB %d] mbsfn_Subframe_pattern is  = %x\n", Mod_id,
                   eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.array[i]->subframeAllocation.choice.oneFrame.buf[0]>>0);
            LOG_D (RRC, "[eNB %d] radioframe_allocation_period  = %ld (just index number, not the real value)\n", Mod_id, eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.array[i]->radioframeAllocationPeriod);        // need to display the real value, using array of char (like in dumping SIB2)
            LOG_D (RRC, "[eNB %d] radioframe_allocation_offset  = %ld\n",
                   Mod_id,
                   eNB_rrc_inst[Mod_id].sib2->mbsfn_SubframeConfigList->list.array[i]->radioframeAllocationOffset);
        }
        //   SIB13
        for (i=0; 
             i<  eNB_rrc_inst[Mod_id].sib13->mbsfn_AreaInfoList_r9.list.count;
             i++){
            LOG_D (RRC,"[eNB %d] SIB13 contents for MBSFN sync area %d/2 (partial)\n",
                   Mod_id, i,
                   eNB_rrc_inst[Mod_id].sib13->mbsfn_AreaInfoList_r9.list.count);
            LOG_D (RRC,"[eNB %d] MCCH Repetition Period: %d (just index number, not real value)\n",
                   Mod_id,
                   eNB_rrc_inst[Mod_id].sib13->mbsfn_AreaInfoList_r9.list.array[i]->mcch_Config_r9.mcch_RepetitionPeriod_r9);
            LOG_D (RRC, "[eNB %d] MCCH Offset: %d\n", Mod_id,
                   eNB_rrc_inst[Mod_id].sib13->mbsfn_AreaInfoList_r9.list.array[i]->mcch_Config_r9.mcch_Offset_r9);
        }
    }
#endif

    LOG_D (RRC, "[MSC_MSG][FRAME unknown][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (SIB1.tdd & SIB2 params) --->][MAC_UE][MOD %02d][]\n",
           Mod_id, Mod_id);

    rrc_mac_config_req (Mod_id, 1, 0, 0,
                          (RadioResourceConfigCommonSIB_t *) &
                          eNB_rrc_inst[Mod_id].sib2->
                          radioResourceConfigCommon,
                          (struct PhysicalConfigDedicated *) NULL,
                          (MeasObjectToAddMod_t **) NULL,
                          (MAC_MainConfig_t *) NULL, 0,
                          (struct LogicalChannelConfig *) NULL,
                          (MeasGapConfig_t *) NULL,
                          eNB_rrc_inst[Mod_id].sib1->tdd_Config,
                          NULL,
                          &SIwindowsize, &SIperiod,
                          eNB_rrc_inst[Mod_id].sib2->freqInfo.ul_CarrierFreq,
                          eNB_rrc_inst[Mod_id].sib2->freqInfo.ul_Bandwidth,
                          &eNB_rrc_inst[Mod_id].sib2->freqInfo.
                          additionalSpectrumEmission,
                          (MBSFN_SubframeConfigList_t *) eNB_rrc_inst[Mod_id].
                          sib2->mbsfn_SubframeConfigList
#ifdef Rel10
                          ,
                          eNB_rrc_inst[Mod_id].MBMS_flag,
                          (MBSFN_AreaInfoList_r9_t *) & eNB_rrc_inst[Mod_id].
                          sib13->mbsfn_AreaInfoList_r9,
                          (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                          ,0,  //eNB_rrc_inst[Mod_id].num_active_cba_groups,
                          0     //eNB_rrc_inst[Mod_id].cba_rnti[0]
#endif
        );
    }
  else
    {
      LOG_E (RRC, "[eNB] init_SI: FATAL, no memory for SIB2/3 allocated\n");
      mac_xface->macphy_exit ("[RRC][init_SI] FATAL, no memory for SIB2/3 allocated");
    }
}

#ifdef Rel10
/*------------------------------------------------------------------------------*/
static void init_MCCH (u8 Mod_id) {

  int sync_area=0;
  // initialize RRC_eNB_INST MCCH entry
  eNB_rrc_inst[Mod_id].MCCH_MESSAGE=malloc(eNB_rrc_inst[Mod_id].num_mbsfn_sync_area*sizeof(uint32_t*));
  for (sync_area = 0;
       sync_area < eNB_rrc_inst[Mod_id].num_mbsfn_sync_area;
       sync_area++){ 
  
    eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[sync_area] = 0;
    eNB_rrc_inst[Mod_id].MCCH_MESSAGE[sync_area] = (u8 *) malloc16 (32);
    
    if (eNB_rrc_inst[Mod_id].MCCH_MESSAGE[sync_area] == NULL) {
      LOG_E(RRC,"[eNB %d][MAIN] init_MCCH: FATAL, no memory for MCCH MESSAGE allocated \n", Mod_id);
      mac_xface->macphy_exit("[RRC][init_MCCH] not enough memory\n");
    } else {
      eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[sync_area] = do_MBSFNAreaConfig (mac_xface->lte_frame_parms,
										sync_area,
										(uint8_t *) eNB_rrc_inst[Mod_id].MCCH_MESSAGE[sync_area],
										&eNB_rrc_inst[Mod_id].mcch,
										&eNB_rrc_inst[Mod_id].mcch_message);

      LOG_I(RRC,"mcch message pointer %p for sync area %d \n", eNB_rrc_inst[Mod_id].MCCH_MESSAGE[sync_area],sync_area);
      LOG_D (RRC, "[eNB %d] MCCH_MESSAGE  contents for Sync Area %d (partial)\n", Mod_id,sync_area);
      LOG_D (RRC, "[eNB %d] CommonSF_AllocPeriod_r9 %d\n", Mod_id,
             eNB_rrc_inst[Mod_id].mcch_message->commonSF_AllocPeriod_r9);
      LOG_D (RRC,
             "[eNB %d] CommonSF_Alloc_r9.list.count (number of MBSFN Subframe Pattern) %d\n",
             Mod_id,
             eNB_rrc_inst[Mod_id].mcch_message->commonSF_Alloc_r9.list.count);
      LOG_D (RRC, "[eNB %d] MBSFN Subframe Pattern: %02x (in hex)\n",
             Mod_id,
             eNB_rrc_inst[Mod_id].mcch_message->commonSF_Alloc_r9.list.
             array[0]->subframeAllocation.choice.oneFrame.buf[0]);


      if (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[sync_area] == 255)
          mac_xface->macphy_exit ("[RRC][init_MCCH] eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[sync_area] == 255");
      else 
          eNB_rrc_inst[Mod_id].MCCH_MESS[sync_area].Active = 1;
    }
  }
  //Set the eNB_rrc_inst[Mod_id].MCCH_MESS.Active to 1 (allow to  transfer MCCH message RRC->MAC in function mac_rrc_data_req)
 

  // ??Configure MCCH logical channel
  // call mac_config_req with appropriate structure from ASN.1 description

  //  LOG_I(RRC, "DUY: serviceID is %d\n",eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->tmgi_r9.serviceId_r9.buf[2]);
  //  LOG_I(RRC, "DUY: session ID is %d\n",eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->sessionId_r9->buf[0]);
  rrc_mac_config_req (Mod_id, 1, 0, 0,
                      (RadioResourceConfigCommonSIB_t *) NULL,
                      (struct PhysicalConfigDedicated *) NULL,
                      (MeasObjectToAddMod_t **) NULL,
                      (MAC_MainConfig_t *) NULL,
                      0,
                      (struct LogicalChannelConfig *) NULL,
                      (MeasGapConfig_t *) NULL,
                      (TDD_Config_t *) NULL,
		      NULL,
                      (u8 *) NULL,
                      (u16 *) NULL,
                      NULL, NULL, NULL, (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                      ,
                      0,
                      (MBSFN_AreaInfoList_r9_t *) NULL,
                      (PMCH_InfoList_r9_t *) & (eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9)
#endif
#ifdef CBA
                      , 0, 0
#endif
    );

  //LOG_I(RRC,"DUY: lcid after rrc_mac_config_req is %02d\n",eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->logicalChannelIdentity_r9);

}

/*------------------------------------------------------------------------------*/
static void init_MBMS (u8 Mod_id, u32 frame) {
  // init the configuration for MTCH 

  if (eNB_rrc_inst[Mod_id].MBMS_flag > 0) {

    LOG_D(RRC,"[eNB %d] Frame %d : Radio Bearer config request for MBMS\n", Mod_id, frame); //check the lcid
      // Configuring PDCP and RLC for MBMS Radio Bearer

      rrc_pdcp_config_asn1_req (Mod_id, 0, frame, 1,
				NULL,      // SRB_ToAddModList
                                NULL,   // DRB_ToAddModList
                                (DRB_ToReleaseList_t *) NULL,
				0, // security mode
				NULL, // key rrc encryption
				NULL, // key rrc integrity
				NULL // key encryption
#ifdef Rel10
                                ,
                                &(eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9)
#endif
			     );
    
    rrc_rlc_config_asn1_req(Mod_id, frame, 1, 0,
			    NULL,// SRB_ToAddModList
			    NULL,// DRB_ToAddModList
			    NULL,// DRB_ToReleaseList
			    &(eNB_rrc_inst[Mod_id].mcch_message->pmch_InfoList_r9));
    
    //rrc_mac_config_req();
    
  }  
  
}
#endif

/*------------------------------------------------------------------------------*/
static void rrc_lite_eNB_init_security(u8 Mod_id, u8 UE_index)
{
#if defined(ENABLE_SECURITY)
    char ascii_buffer[65];
    uint8_t i;

    memset(eNB_rrc_inst[Mod_id].kenb[UE_index], UE_index, 32);

    for (i = 0; i < 32; i++) {
        sprintf(&ascii_buffer[2 * i], "%02X", eNB_rrc_inst[Mod_id].kenb[UE_index][i]);
    }
    ascii_buffer[2 * i] = '\0';

    LOG_T(RRC, "[OSA][eNB %d][UE %d] kenb    = %s\n", Mod_id, UE_index, ascii_buffer);
#endif
}

/*------------------------------------------------------------------------------*/
uint8_t rrc_eNB_get_next_transaction_identifier(uint8_t Mod_id)
{
    static uint8_t rrc_transaction_identifier[NUMBER_OF_eNB_MAX];

    rrc_transaction_identifier[Mod_id] = (rrc_transaction_identifier[Mod_id] + 1) % RRC_TRANSACTION_IDENTIFIER_NUMBER;

    return rrc_transaction_identifier[Mod_id];
}

/*------------------------------------------------------------------------------*/
/* Functions to handle UE index in eNB UE list */

static uint8_t rrc_eNB_get_next_free_UE_index (uint8_t Mod_id, uint64_t UE_identity)
{
  uint8_t i, first_index = UE_INDEX_INVALID, reg = 0;

  DevCheck(Mod_id < NB_eNB_INST, Mod_id, NB_eNB_INST, 0);

  for (i = 0; i < NUMBER_OF_UE_MAX; i++) {
    if ((first_index == UE_INDEX_INVALID) && (eNB_rrc_inst[Mod_id].Info.UE_list[i] == 0)) {
      first_index = i; // save first free position
    }

    if (eNB_rrc_inst[Mod_id].Info.UE_list[i] == UE_identity) {
      // UE_identity already registered
      reg = 1;
    }
  }

  if (reg == 0) {
    LOG_I(RRC, "[eNB %d] Adding UE %d with identity "PRIu64"\n", Mod_id, first_index, UE_identity);
    return (first_index);
  }
  else {
    return (UE_INDEX_INVALID);
  }
}

void rrc_eNB_free_UE_index (uint8_t Mod_id, uint8_t UE_id)
{
  DevCheck(Mod_id < NB_eNB_INST, Mod_id, UE_id, NB_eNB_INST);
  DevCheck(UE_id < NUMBER_OF_UE_MAX, Mod_id, UE_id, NUMBER_OF_UE_MAX);

  LOG_I (RRC, "[eNB %d] Removing UE %d rv 0x%" PRIx64 "\n", Mod_id, UE_id, eNB_rrc_inst[Mod_id].Info.UE_list[UE_id]);
  eNB_rrc_inst[Mod_id].Info.UE[UE_id].Status = RRC_IDLE;
  eNB_rrc_inst[Mod_id].Info.UE_list[UE_id] = 0;
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_RRCConnectionSetupComplete (u8 Mod_id,
                                                 u32 frame,
                                                 u8 UE_index,
                                                 RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete)
{
  LOG_I (RRC, "[eNB %d][RAPROC] Frame %d : Logical Channel UL-DCCH, ""processing RRCConnectionSetupComplete from UE %d\n",
         Mod_id, frame, UE_index);

#if defined(ENABLE_USE_MME)
  if (EPC_MODE_ENABLED == 1)
  {
    // Forward message to S1AP layer
    rrc_eNB_send_S1AP_NAS_FIRST_REQ(Mod_id, UE_index, rrcConnectionSetupComplete);
  }
  else
#endif
  {
    // RRC loop back (no S1AP), send SecurityModeCommand to UE
    rrc_eNB_generate_SecurityModeCommand (Mod_id, frame, UE_index);
    // rrc_eNB_generate_UECapabilityEnquiry(Mod_id,frame,UE_index);
  }
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_SecurityModeCommand (u8 Mod_id, u32 frame, u16 UE_index)
{
  uint8_t buffer[100];
  uint8_t size;

  size = do_SecurityModeCommand(Mod_id, buffer, UE_index, rrc_eNB_get_next_transaction_identifier(Mod_id),
      eNB_rrc_inst[Mod_id].ciphering_algorithm[UE_index], eNB_rrc_inst[Mod_id].integrity_algorithm[UE_index]);

  LOG_I (RRC,
         "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate SecurityModeCommand (bytes %d, UE id %d)\n",
         Mod_id, frame, size, UE_index);


  LOG_D (RRC,
         "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (securityModeCommand to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
         frame, Mod_id, size, UE_index, rrc_eNB_mui, Mod_id, DCCH);
  //rrc_rlc_data_req(Mod_id,frame, 1,(UE_index*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
  pdcp_rrc_data_req (Mod_id, UE_index, frame, 1, DCCH,
                     rrc_eNB_mui++, 0, size, buffer, 1);

}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_UECapabilityEnquiry (u8 Mod_id, u32 frame, u16 UE_index)
{

  uint8_t buffer[100];
  uint8_t size;

  size = do_UECapabilityEnquiry (Mod_id, buffer, UE_index, rrc_eNB_get_next_transaction_identifier(Mod_id));

  LOG_I (RRC,
         "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate UECapabilityEnquiry (bytes %d, UE id %d)\n",
         Mod_id, frame, size, UE_index);


  LOG_D (RRC,
         "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (UECapabilityEnquiry to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
         frame, Mod_id, size, UE_index, rrc_eNB_mui, Mod_id, DCCH);
  //rrc_rlc_data_req(Mod_id,frame, 1,(UE_index*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
  pdcp_rrc_data_req (Mod_id, UE_index, frame, 1, DCCH,
                     rrc_eNB_mui++, 0, size, buffer, 1);

}

/*------------------------------------------------------------------------------*/
static void rrc_eNB_generate_defaultRRCConnectionReconfiguration (u8 Mod_id, u32 frame,
                                                                  u16 UE_index,
                                                                  u8 ho_state)
{
#if defined(ENABLE_ITTI)
  eNB_RRC_UE_INFO *UE_info = &eNB_rrc_inst[Mod_id].Info.UE[UE_index];
#endif
  u8 buffer[RRC_BUF_SIZE];
  u8 size;
  int i;

  // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE
  eNB_RRC_INST *rrc_inst = &eNB_rrc_inst[Mod_id];

  struct PhysicalConfigDedicated **physicalConfigDedicated = &rrc_inst->physicalConfigDedicated[UE_index];


  struct SRB_ToAddMod *SRB2_config;
  struct SRB_ToAddMod__rlc_Config *SRB2_rlc_config;
  struct SRB_ToAddMod__logicalChannelConfig *SRB2_lchan_config;
  struct LogicalChannelConfig__ul_SpecificParameters
    *SRB2_ul_SpecificParameters;
  SRB_ToAddModList_t *SRB_configList = rrc_inst->SRB_configList[UE_index];
  SRB_ToAddModList_t *SRB_configList2;

  struct DRB_ToAddMod *DRB_config;
  struct RLC_Config *DRB_rlc_config;
  struct PDCP_Config *DRB_pdcp_config;
  struct PDCP_Config__rlc_UM *PDCP_rlc_UM;
  struct LogicalChannelConfig *DRB_lchan_config;
  struct LogicalChannelConfig__ul_SpecificParameters
    *DRB_ul_SpecificParameters;
  DRB_ToAddModList_t **DRB_configList = &rrc_inst->DRB_configList[UE_index];

  MAC_MainConfig_t *mac_MainConfig;
  MeasObjectToAddModList_t *MeasObj_list;
  MeasObjectToAddMod_t *MeasObj;
  ReportConfigToAddModList_t *ReportConfig_list;
  ReportConfigToAddMod_t *ReportConfig_per, *ReportConfig_A1,
    *ReportConfig_A2, *ReportConfig_A3, *ReportConfig_A4, *ReportConfig_A5;
  MeasIdToAddModList_t *MeasId_list;
  MeasIdToAddMod_t *MeasId0, *MeasId1, *MeasId2, *MeasId3, *MeasId4, *MeasId5;
#if Rel10
  long *sr_ProhibitTimer_r9;
#endif

  long *logicalchannelgroup, *logicalchannelgroup_drb;
  long *maxHARQ_Tx, *periodicBSR_Timer;

  RSRP_Range_t *rsrp=NULL;
  struct MeasConfig__speedStatePars *Sparams=NULL;
  QuantityConfig_t *quantityConfig=NULL;
  CellsToAddMod_t *CellToAdd;
  CellsToAddModList_t *CellsToAddModList;
  struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList *dedicatedInfoNASList = NULL;
  DedicatedInfoNAS_t *dedicatedInfoNas;

  C_RNTI_t *cba_RNTI = NULL;
#ifdef CBA
  //struct PUSCH_CBAConfigDedicated_vlola  *pusch_CBAConfigDedicated_vlola;
  uint8_t *cba_RNTI_buf;
  cba_RNTI = CALLOC (1, sizeof (C_RNTI_t));
  cba_RNTI_buf = CALLOC (1, 2 * sizeof (uint8_t));
  cba_RNTI->buf = cba_RNTI_buf;
  cba_RNTI->size = 2;
  cba_RNTI->bits_unused = 0;
  // associate UEs to the CBa groups as a function of their UE id
  if (rrc_inst->num_active_cba_groups)
    {
      cba_RNTI->buf[0] =
        rrc_inst->cba_rnti[UE_index % rrc_inst->num_active_cba_groups] & 0xff;
      cba_RNTI->buf[1] = 0xff;
      LOG_D (RRC,
             "[eNB %d] Frame %d: cba_RNTI = %x in group %d is attribued to UE %d\n",
             Mod_id, frame,
             rrc_inst->cba_rnti[UE_index % rrc_inst->num_active_cba_groups],
             UE_index % rrc_inst->num_active_cba_groups, UE_index);
    }
  else
    {
      cba_RNTI->buf[0] = 0x0;
      cba_RNTI->buf[1] = 0x0;
      LOG_D (RRC, "[eNB %d] Frame %d: no cba_RNTI is configured for UE %d\n",
             Mod_id, frame, UE_index);
    }


#endif
  // Configure SRB2
  /// SRB2
  SRB2_config = CALLOC (1, sizeof (*SRB2_config));
  SRB_configList2 = CALLOC (1, sizeof (*SRB_configList2));
  memset (SRB_configList2, 0, sizeof (*SRB_configList2));

  SRB2_config->srb_Identity = 2;
  SRB2_rlc_config = CALLOC (1, sizeof (*SRB2_rlc_config));
  SRB2_config->rlc_Config = SRB2_rlc_config;

  SRB2_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
  SRB2_rlc_config->choice.explicitValue.present = RLC_Config_PR_am;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms15;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU = PollPDU_p8;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte = PollByte_kB1000;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t32;
  SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms50;
  SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit =  T_StatusProhibit_ms10;

  SRB2_lchan_config = CALLOC (1, sizeof (*SRB2_lchan_config));
  SRB2_config->logicalChannelConfig = SRB2_lchan_config;

  SRB2_lchan_config->present =  SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;

  SRB2_ul_SpecificParameters = CALLOC (1, sizeof (*SRB2_ul_SpecificParameters));

  SRB2_ul_SpecificParameters->priority = 1;
  SRB2_ul_SpecificParameters->prioritisedBitRate = LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
  SRB2_ul_SpecificParameters->bucketSizeDuration = LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

  // LCG for CCCH and DCCH is 0 as defined in 36331
  logicalchannelgroup = CALLOC (1, sizeof (long));
  *logicalchannelgroup = 0;

  SRB2_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;

  SRB2_lchan_config->choice.explicitValue.ul_SpecificParameters =  SRB2_ul_SpecificParameters;
  ASN_SEQUENCE_ADD (&SRB_configList->list, SRB2_config);
  ASN_SEQUENCE_ADD (&SRB_configList2->list, SRB2_config);

  // Configure DRB
  *DRB_configList = CALLOC (1, sizeof (*DRB_configList));
  /// DRB
  DRB_config = CALLOC (1, sizeof (*DRB_config));

  //DRB_config->drb_Identity = (DRB_Identity_t) 1; //allowed values 1..32
  // NN: this is the 1st DRB for this ue, so set it to 1
  DRB_config->drb_Identity = (DRB_Identity_t) 1;        // (UE_index+1); //allowed values 1..32
  DRB_config->logicalChannelIdentity = CALLOC (1, sizeof (long));
  *(DRB_config->logicalChannelIdentity) = (long) 3;
  DRB_rlc_config = CALLOC (1, sizeof (*DRB_rlc_config));
  DRB_config->rlc_Config = DRB_rlc_config;
  DRB_rlc_config->present = RLC_Config_PR_um_Bi_Directional;
  DRB_rlc_config->choice.um_Bi_Directional.ul_UM_RLC.sn_FieldLength =  SN_FieldLength_size10;
  DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.sn_FieldLength =  SN_FieldLength_size10;
  DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.t_Reordering =   T_Reordering_ms5;

  DRB_pdcp_config = CALLOC (1, sizeof (*DRB_pdcp_config));
  DRB_config->pdcp_Config = DRB_pdcp_config;
  DRB_pdcp_config->discardTimer = NULL;
  DRB_pdcp_config->rlc_AM = NULL;
  PDCP_rlc_UM = CALLOC (1, sizeof (*PDCP_rlc_UM));
  DRB_pdcp_config->rlc_UM = PDCP_rlc_UM;
  PDCP_rlc_UM->pdcp_SN_Size = PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits;
  DRB_pdcp_config->headerCompression.present = PDCP_Config__headerCompression_PR_notUsed;

  DRB_lchan_config = CALLOC (1, sizeof (*DRB_lchan_config));
  DRB_config->logicalChannelConfig = DRB_lchan_config;
  DRB_ul_SpecificParameters = CALLOC (1, sizeof (*DRB_ul_SpecificParameters));
  DRB_lchan_config->ul_SpecificParameters = DRB_ul_SpecificParameters;


  DRB_ul_SpecificParameters->priority = 2;      // lower priority than srb1, srb2
  DRB_ul_SpecificParameters->prioritisedBitRate = LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
  DRB_ul_SpecificParameters->bucketSizeDuration = LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

  // LCG for DTCH can take the value from 1 to 3 as defined in 36331: normally controlled by upper layers (like RRM)
  logicalchannelgroup_drb = CALLOC (1, sizeof (long));
  *logicalchannelgroup_drb = 1;
  DRB_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup_drb;

  ASN_SEQUENCE_ADD (&(*DRB_configList)->list, DRB_config);

  mac_MainConfig = CALLOC (1, sizeof (*mac_MainConfig));
  eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index] = mac_MainConfig;

  mac_MainConfig->ul_SCH_Config = CALLOC (1, sizeof (*mac_MainConfig->ul_SCH_Config));

  maxHARQ_Tx = CALLOC (1, sizeof (long));
  *maxHARQ_Tx = MAC_MainConfig__ul_SCH_Config__maxHARQ_Tx_n5;
  mac_MainConfig->ul_SCH_Config->maxHARQ_Tx = maxHARQ_Tx;
  periodicBSR_Timer = CALLOC (1, sizeof (long));
  *periodicBSR_Timer = MAC_MainConfig__ul_SCH_Config__periodicBSR_Timer_sf64;
  mac_MainConfig->ul_SCH_Config->periodicBSR_Timer = periodicBSR_Timer;
  mac_MainConfig->ul_SCH_Config->retxBSR_Timer =  MAC_MainConfig__ul_SCH_Config__retxBSR_Timer_sf320;
  mac_MainConfig->ul_SCH_Config->ttiBundling = 0;       // FALSE

  mac_MainConfig->drx_Config = NULL;

  mac_MainConfig->phr_Config =  CALLOC (1, sizeof (*mac_MainConfig->phr_Config));

  mac_MainConfig->phr_Config->present = MAC_MainConfig__phr_Config_PR_setup;
  mac_MainConfig->phr_Config->choice.setup.periodicPHR_Timer = MAC_MainConfig__phr_Config__setup__periodicPHR_Timer_sf20;       // sf20 = 20 subframes

  mac_MainConfig->phr_Config->choice.setup.prohibitPHR_Timer = MAC_MainConfig__phr_Config__setup__prohibitPHR_Timer_sf20;       // sf20 = 20 subframes

  mac_MainConfig->phr_Config->choice.setup.dl_PathlossChange = MAC_MainConfig__phr_Config__setup__dl_PathlossChange_dB1;        // Value dB1 =1 dB, dB3 = 3 dB

#ifdef Rel10
  sr_ProhibitTimer_r9 = CALLOC (1, sizeof (long));
  *sr_ProhibitTimer_r9 = 0;     // SR tx on PUCCH, Value in number of SR period(s). Value 0 = no timer for SR, Value 2= 2*SR
  mac_MainConfig->sr_ProhibitTimer_r9 = sr_ProhibitTimer_r9;
  //sps_RA_ConfigList_rlola = NULL;
#endif


  // Measurement ID list
  MeasId_list = CALLOC (1, sizeof (*MeasId_list));
  memset ((void *) MeasId_list, 0, sizeof (*MeasId_list));

  MeasId0 = CALLOC (1, sizeof (*MeasId0));
  MeasId0->measId = 1;
  MeasId0->measObjectId = 1;
  MeasId0->reportConfigId = 1;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId0);

  MeasId1 = CALLOC (1, sizeof (*MeasId1));
  MeasId1->measId = 2;
  MeasId1->measObjectId = 1;
  MeasId1->reportConfigId = 2;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId1);

  MeasId2 = CALLOC (1, sizeof (*MeasId2));
  MeasId2->measId = 3;
  MeasId2->measObjectId = 1;
  MeasId2->reportConfigId = 3;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId2);

  MeasId3 = CALLOC (1, sizeof (*MeasId3));
  MeasId3->measId = 4;
  MeasId3->measObjectId = 1;
  MeasId3->reportConfigId = 4;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId3);

  MeasId4 = CALLOC (1, sizeof (*MeasId4));
  MeasId4->measId = 5;
  MeasId4->measObjectId = 1;
  MeasId4->reportConfigId = 5;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId4);

  MeasId5 = CALLOC (1, sizeof (*MeasId5));
  MeasId5->measId = 6;
  MeasId5->measObjectId = 1;
  MeasId5->reportConfigId = 6;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId5);

  //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measIdToAddModList = MeasId_list;

  // Add one EUTRA Measurement Object
  MeasObj_list = CALLOC (1, sizeof (*MeasObj_list));
  memset ((void *) MeasObj_list, 0, sizeof (*MeasObj_list));

  // Configure MeasObject

  MeasObj = CALLOC (1, sizeof (*MeasObj));
  memset ((void *) MeasObj, 0, sizeof (*MeasObj));

  MeasObj->measObjectId = 1;
  MeasObj->measObject.present =  MeasObjectToAddMod__measObject_PR_measObjectEUTRA;
  MeasObj->measObject.choice.measObjectEUTRA.carrierFreq = 36090;
  MeasObj->measObject.choice.measObjectEUTRA.allowedMeasBandwidth = AllowedMeasBandwidth_mbw25;
  MeasObj->measObject.choice.measObjectEUTRA.presenceAntennaPort1 = 1;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf = CALLOC (1, sizeof (uint8_t));
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf[0] = 0;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.size = 1;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.bits_unused = 6;
  MeasObj->measObject.choice.measObjectEUTRA.offsetFreq = NULL; // Default is 15 or 0dB

  MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList = (CellsToAddModList_t *) CALLOC (1, sizeof (*CellsToAddModList));

  CellsToAddModList = MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList;

  // Add adjacent cell lists (6 per eNB)
  for (i = 0; i < 6; i++)   {
      CellToAdd = (CellsToAddMod_t *) CALLOC (1, sizeof (*CellToAdd));
      CellToAdd->cellIndex = i + 1;
      CellToAdd->physCellId = get_adjacent_cell_id (Mod_id, i);
      CellToAdd->cellIndividualOffset = Q_OffsetRange_dB0;

      ASN_SEQUENCE_ADD (&CellsToAddModList->list, CellToAdd);
  }

  ASN_SEQUENCE_ADD (&MeasObj_list->list, MeasObj);
  //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measObjectToAddModList = MeasObj_list;

  // Report Configurations for periodical, A1-A5 events
  ReportConfig_list = CALLOC (1, sizeof (*ReportConfig_list));

  ReportConfig_per = CALLOC (1, sizeof (*ReportConfig_per));

  ReportConfig_A1 = CALLOC (1, sizeof (*ReportConfig_A1));

  ReportConfig_A2 = CALLOC (1, sizeof (*ReportConfig_A2));

  ReportConfig_A3 = CALLOC (1, sizeof (*ReportConfig_A3));

  ReportConfig_A4 = CALLOC (1, sizeof (*ReportConfig_A4));

  ReportConfig_A5 = CALLOC (1, sizeof (*ReportConfig_A5));

  ReportConfig_per->reportConfigId = 1;
  ReportConfig_per->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.present = ReportConfigEUTRA__triggerType_PR_periodical;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.choice.periodical.purpose = ReportConfigEUTRA__triggerType__periodical__purpose_reportStrongestCells;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

  ASN_SEQUENCE_ADD (&ReportConfig_list->list, ReportConfig_per);
 
  ReportConfig_A1->reportConfigId = 2;
  ReportConfig_A1->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.present = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA1;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.a1_Threshold.present =
    ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.a1_Threshold.choice.threshold_RSRP = 10;

  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

  ASN_SEQUENCE_ADD (&ReportConfig_list->list, ReportConfig_A1);

  if (ho_state == 1 /*HO_MEASURMENT*/ ){
    LOG_I(RRC,"[eNB %d] frame %d: requesting A2, A3, A4, A5, and A6 event reporting\n", Mod_id, frame);
    ReportConfig_A2->reportConfigId                                                              = 3;
    ReportConfig_A2->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.present                  = ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present= ReportConfigEUTRA__triggerType__event__eventId_PR_eventA2;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.a2_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.a2_Threshold.choice.threshold_RSRP = 10;
    
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
    
    ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A2);

    ReportConfig_A3->reportConfigId                                                              = 4;
    ReportConfig_A3->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.present                   = ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA3;

    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.a3_Offset = 1;//10;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.reportOnLeave = 1;
    
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
    
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.hysteresis   = 0.5;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.timeToTrigger = TimeToTrigger_ms40;
    ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A3);
    
    ReportConfig_A4->reportConfigId                                                              = 5;
    ReportConfig_A4->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.present                   = ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA4;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.a4_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.a4_Threshold.choice.threshold_RSRP = 10;
    
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
    
    ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A4);
    
    ReportConfig_A5->reportConfigId                                                              = 6;
    ReportConfig_A5->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.present                                    = ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present              = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA5;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold1.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold2.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold1.choice.threshold_RSRP = 10;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold2.choice.threshold_RSRP = 10;
    
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
    
    ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A5);
    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->reportConfigToAddModList = ReportConfig_list;

    rsrp=CALLOC(1,sizeof(RSRP_Range_t));
    *rsrp=20;
    
    Sparams = CALLOC(1,sizeof(*Sparams));
    Sparams->present=MeasConfig__speedStatePars_PR_setup;
    Sparams->choice.setup.timeToTrigger_SF.sf_High=SpeedStateScaleFactors__sf_Medium_oDot75;
    Sparams->choice.setup.timeToTrigger_SF.sf_Medium=SpeedStateScaleFactors__sf_High_oDot5;
    Sparams->choice.setup.mobilityStateParameters.n_CellChangeHigh=10;
    Sparams->choice.setup.mobilityStateParameters.n_CellChangeMedium=5;
    Sparams->choice.setup.mobilityStateParameters.t_Evaluation=MobilityStateParameters__t_Evaluation_s60;
    Sparams->choice.setup.mobilityStateParameters.t_HystNormal=MobilityStateParameters__t_HystNormal_s120;
    
    quantityConfig = CALLOC(1,sizeof(*quantityConfig));
    memset((void *)quantityConfig,0,sizeof(*quantityConfig));
    quantityConfig->quantityConfigEUTRA = CALLOC(1,sizeof(struct QuantityConfigEUTRA));
    memset((void *)quantityConfig->quantityConfigEUTRA,0,sizeof(*quantityConfig->quantityConfigEUTRA));
    quantityConfig->quantityConfigCDMA2000 = NULL;
    quantityConfig->quantityConfigGERAN = NULL;
    quantityConfig->quantityConfigUTRA = NULL;
    quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = CALLOC(1,sizeof(*(quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP)));
    quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = CALLOC(1,sizeof(*(quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ)));
    *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = FilterCoefficient_fc4;
    *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = FilterCoefficient_fc4;
  
    LOG_I(RRC,"[eNB %d] Frame %d: potential handover preparation: store the information in an intermediate structure in case of failure\n",Mod_id, frame);
    // store the information in an intermediate structure for Hanodver management
    //rrc_inst->handover_info.as_config.sourceRadioResourceConfig.srb_ToAddModList = CALLOC(1,sizeof());
    rrc_inst->handover_info[UE_index] = CALLOC(1,sizeof(*(rrc_inst->handover_info[UE_index])));
    //memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.srb_ToAddModList,(void *)SRB_list,sizeof(SRB_ToAddModList_t));
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.srb_ToAddModList = SRB_configList2;
    //memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.drb_ToAddModList,(void *)DRB_list,sizeof(DRB_ToAddModList_t));
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.drb_ToAddModList = *DRB_configList;
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.drb_ToReleaseList = NULL;
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.mac_MainConfig = CALLOC(1, sizeof(*rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.mac_MainConfig));
    memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.mac_MainConfig,
           (void *)mac_MainConfig,
           sizeof(MAC_MainConfig_t));
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.physicalConfigDedicated = CALLOC(1,sizeof(PhysicalConfigDedicated_t));
    memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.physicalConfigDedicated,
           (void *)rrc_inst->physicalConfigDedicated[UE_index],
           sizeof(PhysicalConfigDedicated_t));
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.sps_Config = NULL;
    //memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.sps_Config,(void *)rrc_inst->sps_Config[UE_index],sizeof(SPS_Config_t));

  }
   
#if defined(ENABLE_ITTI)
  /* Initialize NAS list */
  dedicatedInfoNASList = CALLOC (1, sizeof (struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList));
  /* Add all NAS PDUs to the list */
  for (i = 0; i < UE_info->nb_of_e_rabs; i++)
  {
    if (UE_info->e_rab[i].param.nas_pdu.buffer != NULL)
    {
      dedicatedInfoNas = CALLOC (1, sizeof(DedicatedInfoNAS_t));

      OCTET_STRING_fromBuf (dedicatedInfoNas, (char *) UE_info->e_rab[i].param.nas_pdu.buffer, UE_info->e_rab[i].param.nas_pdu.length);
      ASN_SEQUENCE_ADD (&dedicatedInfoNASList->list, dedicatedInfoNas);
    }

    /* TODO parameters yet to process ... */
    {
      UE_info->e_rab[i].param.qos;
      UE_info->e_rab[i].param.sgw_addr;
      UE_info->e_rab[i].param.gtp_teid;
    }

    /* TODO should test if e RAB are Ok before! */
    eNB_rrc_inst[Mod_id].Info.UE[UE_index].e_rab[i].status = E_RAB_STATUS_DONE;
  }
  /* If list is empty free the list and reset the address */
  if(dedicatedInfoNASList->list.count == 0)
  {
    free (dedicatedInfoNASList);
    dedicatedInfoNASList = NULL;
  }
#endif

  memset (buffer, 0, RRC_BUF_SIZE);

  size = do_RRCConnectionReconfiguration (Mod_id, buffer, UE_index, rrc_eNB_get_next_transaction_identifier(Mod_id),  //Transaction_id,
                                          SRB_configList2, *DRB_configList, NULL,       // DRB2_list,
                                          NULL, //*sps_Config,
                                          physicalConfigDedicated[UE_index], MeasObj_list, ReportConfig_list, 
                                          quantityConfig,
                                          MeasId_list, mac_MainConfig, NULL,NULL,Sparams,rsrp,
                                          cba_RNTI, dedicatedInfoNASList);

#if defined(ENABLE_ITTI)
  /* Free all NAS PDUs */
  for (i = 0; i < UE_info->nb_of_e_rabs; i++)
  {
    if (UE_info->e_rab[i].param.nas_pdu.buffer != NULL)
    {
      /* Free the NAS PDU buffer and invalidate it */
      free (UE_info->e_rab[i].param.nas_pdu.buffer);
      UE_info->e_rab[i].param.nas_pdu.buffer = NULL;
    }
  }
#endif

  LOG_I (RRC,"[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate RRCConnectionReconfiguration (bytes %d, UE id %d)\n",
         Mod_id, frame, size, UE_index);

  LOG_D (RRC,"[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionReconfiguration to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
         frame, Mod_id, size, UE_index, rrc_eNB_mui, Mod_id, DCCH);
  //rrc_rlc_data_req(Mod_id,frame, 1,(UE_index*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
  pdcp_rrc_data_req (Mod_id, UE_index, frame, 1, DCCH,
                     rrc_eNB_mui++, 0, size, buffer, 1);

}

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_MeasurementReport (u8 Mod_id, u32 frame, u16 UE_index,
                                   MeasResults_t * measResults2)
{

  LOG_I (RRC,"[eNB %d] Frame %d: Process Measurement Report From UE %d (Measurement Id %d)\n",
         Mod_id, frame, UE_index, (int) measResults2->measId);
  if (measResults2->measResultNeighCells->choice.measResultListEUTRA.list.count > 0) {
      LOG_I (RRC, "Physical Cell Id %d\n",
             (int) measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->physCellId);
      LOG_I (RRC, "RSRP of Target %d\n",
             (int) *(measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->measResult.rsrpResult));
      LOG_I (RRC, "RSRQ of Target %d\n",
             (int) *(measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->measResult.rsrqResult));
  }
#ifdef Rel10
  LOG_I (RRC, "RSRP of Source %d\n",
         measResults2->measResultPCell.rsrpResult);
  LOG_I (RRC, "RSRQ of Source %d\n",
         measResults2->measResultPCell.rsrqResult);
#else
  LOG_I (RRC, "RSRP of Source %d\n",
         measResults2->measResultServCell.rsrpResult);
  LOG_I (RRC, "RSRQ of Source %d\n",
         measResults2->measResultServCell.rsrqResult);
#endif

  if(eNB_rrc_inst[Mod_id].handover_info[UE_index]->ho_prepare != 0xF0){
    rrc_eNB_generate_HandoverPreparationInformation(Mod_id,
                                                    frame,
                                                    UE_index,
                                                    measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->physCellId);
  }else{
    LOG_D(RRC,"[eNB %d] Frame %d: Ignoring MeasReport from UE %d as Handover is in progress... \n",Mod_id,frame, UE_index);
  }
  //Look for IP address of the target eNB
  //Send Handover Request -> target eNB
  //Wait for Handover Acknowledgement <- target eNB
  //Send Handover Command

  //x2delay();
  //    handover_request_x2(UE_index,Mod_id,measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->physCellId);

  //    u8 buffer[100];
  //    int size=rrc_eNB_generate_Handover_Command_TeNB(0,0,buffer);
  //
  //      send_check_message((char*)buffer,size);
  //send_handover_command();

}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_HandoverPreparationInformation (u8 Mod_id, u32 frame, u8 UE_index, PhysCellId_t targetPhyId) {
  u8 UE_id_target;
  u8 mod_id_target = get_adjacent_cell_mod_id(targetPhyId);
  HANDOVER_INFO *handoverInfo = CALLOC(1,sizeof(*handoverInfo));
  /*
  u8 buffer[100];
  u8 size;
  struct PhysicalConfigDedicated  **physicalConfigDedicated = &eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index];
  RadioResourceConfigDedicated_t *radioResourceConfigDedicated = CALLOC(1,sizeof(RadioResourceConfigDedicated_t));
  */

  handoverInfo->as_config.antennaInfoCommon.antennaPortsCount =  0; //Not used 0- but check value
  handoverInfo->as_config.sourceDl_CarrierFreq = 36090; //Verify!
  memcpy((void*) &handoverInfo->as_config.sourceMasterInformationBlock, 
         (void*) &eNB_rrc_inst[Mod_id].mib,
         sizeof(MasterInformationBlock_t));
  memcpy((void*) &handoverInfo->as_config.sourceMeasConfig, 
         (void*) &eNB_rrc_inst[Mod_id].measConfig[UE_index],
         sizeof(MeasConfig_t));
  //to be configured
  memset((void *)&eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_config.sourceSecurityAlgorithmConfig,
         0,
         sizeof(SecurityAlgorithmConfig_t));

  memcpy((void *)&eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_config.sourceSystemInformationBlockType1,
         (void *)&eNB_rrc_inst[Mod_id].SIB1,
         sizeof(SystemInformationBlockType1_t));
  memcpy((void *)&eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_config.sourceSystemInformationBlockType2,
         (void *)&eNB_rrc_inst[Mod_id].SIB23,
         sizeof(SystemInformationBlockType2_t));

  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo = CALLOC(1,sizeof(ReestablishmentInfo_t));
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo->sourcePhysCellId = eNB_rrc_inst[Mod_id].physCellId;
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo->targetCellShortMAC_I.buf = NULL; // Check values later
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo->targetCellShortMAC_I.size = 0;
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo->targetCellShortMAC_I.bits_unused = 0;
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context.reestablishmentInfo->additionalReestabInfoList = NULL;

  eNB_rrc_inst[Mod_id].handover_info[UE_index]->ho_prepare = 0xFF;//0xF0;
  eNB_rrc_inst[Mod_id].handover_info[UE_index]->ho_complete = 0;

  if (mod_id_target != 0xFF) {
    //UE_id_target = rrc_find_free_ue_index(modid_target);
    UE_id_target = rrc_eNB_get_next_free_UE_index(mod_id_target, eNB_rrc_inst[Mod_id].Info.UE_list[UE_index]); //this should return a new index

    if (UE_id_target!=0xFF) {
      LOG_N(RRC,"[eNB %d] Frame %d : Emulate sending HandoverPreparationInformation msg from eNB source %d to eNB target %d: source UE_id %d target UE_id %d source_modId: %d target_modId: %d\n",Mod_id,frame,eNB_rrc_inst[Mod_id].physCellId,targetPhyId,UE_index,UE_id_target,Mod_id,mod_id_target);
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target] = CALLOC(1,sizeof(*(eNB_rrc_inst[mod_id_target].handover_info[UE_id_target])));
      memcpy((void *)&eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->as_context, 
             (void *)&eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_context,
             sizeof(AS_Context_t));
      memcpy((void *)&eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->as_config, 
             (void *)&eNB_rrc_inst[Mod_id].handover_info[UE_index]->as_config,
             sizeof(AS_Config_t));
      
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ho_prepare = 0xFF;
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ho_complete = 0;

      eNB_rrc_inst[Mod_id].handover_info[UE_index]->modid_t = mod_id_target;
      eNB_rrc_inst[Mod_id].handover_info[UE_index]->ueid_s = UE_index;
      eNB_rrc_inst[Mod_id].handover_info[UE_index]->modid_s = Mod_id;
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->modid_t = mod_id_target;
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->modid_s = Mod_id;
      eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ueid_t = UE_id_target;
    }
    else
      LOG_E(RRC,"\nError in obtaining free UE id in target eNB %l for handover \n", targetPhyId);
  }
  else
    LOG_E(RRC,"\nError in obtaining Module ID of target eNB for handover \n");
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_handoverPreparationInformation(u8 Mod_id,u32 frame, u16 UE_index) {

  LOG_I(RRC,"[eNB %d] Frame %d : Logical Channel UL-DCCH, processing RRCHandoverPreparationInformation, sending RRCConnectionReconfiguration to UE %d \n",Mod_id,frame,UE_index);


  //eNB_rrc_inst[Mod_id].Info.UE_list[UE_index]
  rrc_eNB_generate_RRCConnectionReconfiguration_handover(Mod_id,frame,UE_index,NULL,0);

}

/*------------------------------------------------------------------------------*/
void check_handovers(u8 Mod_id, u32 frame) {
  u8 i;
  int result;

  for (i=0;i<NUMBER_OF_UE_MAX;i++) {
    if(eNB_rrc_inst[Mod_id].handover_info[i] != NULL) {
      if(eNB_rrc_inst[Mod_id].handover_info[i]->ho_prepare == 0xFF) {
        LOG_D(RRC,"[eNB %d] Frame %d: Incoming handover detected for new UE_idx %d (source eNB %d->target eNB %d) \n",
              Mod_id, frame, i,Mod_id, eNB_rrc_inst[Mod_id].handover_info[i]->modid_t);
        // source eNB generates rrcconnectionreconfiguration to prepare the HO
        rrc_eNB_process_handoverPreparationInformation(Mod_id,frame,i);
        eNB_rrc_inst[Mod_id].handover_info[i]->ho_prepare = 0xF1;
      }
      
      if(eNB_rrc_inst[Mod_id].handover_info[i]->ho_complete == 0xF1) {
        LOG_D(RRC,"[eNB %d] Frame %d: handover Command received for new UE_idx %d current eNB %d target eNB: %d \n",
              Mod_id, frame, i,Mod_id,eNB_rrc_inst[Mod_id].handover_info[i]->modid_t);
        //rrc_eNB_process_handoverPreparationInformation(Mod_id,frame,i);
        pdcp_data_req(Mod_id, i, frame, 1,
                      (i* NB_RB_MAX)+DCCH,
                      rrc_eNB_mui++,0,
                      eNB_rrc_inst[Mod_id].handover_info[i]->size,
                      eNB_rrc_inst[Mod_id].handover_info[i]->buf,1);
        AssertFatal (result == TRUE, "PDCP data request failed!\n");
        eNB_rrc_inst[Mod_id].handover_info[i]->ho_complete = 0xF2;
      }
    }
  }
}

/*------------------------------------------------------------------------------*/
// 5.3.5.4 RRCConnectionReconfiguration including the mobilityControlInfo to prepare the UE handover
void rrc_eNB_generate_RRCConnectionReconfiguration_handover (u8 Mod_id, u32 frame,u16 UE_index,u8 *nas_pdu,u32 nas_length)  {

  u8 buffer[RRC_BUF_SIZE];
  u8 size;
  int i;
  uint8_t rv[2];
  u16 Idx;
  // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE
  eNB_RRC_INST *rrc_inst = &eNB_rrc_inst[Mod_id];

  struct PhysicalConfigDedicated **physicalConfigDedicated = &rrc_inst->physicalConfigDedicated[UE_index];

  struct SRB_ToAddMod *SRB2_config;
  struct SRB_ToAddMod__rlc_Config *SRB2_rlc_config;
  struct SRB_ToAddMod__logicalChannelConfig *SRB2_lchan_config;
  struct LogicalChannelConfig__ul_SpecificParameters  *SRB2_ul_SpecificParameters; 
  LogicalChannelConfig_t *SRB1_logicalChannelConfig = NULL;
  SRB_ToAddModList_t *SRB_configList = rrc_inst->SRB_configList[UE_index]; // not used in this context: may be removed
  SRB_ToAddModList_t *SRB_configList2;

  struct DRB_ToAddMod *DRB_config;
  struct RLC_Config *DRB_rlc_config;
  struct PDCP_Config *DRB_pdcp_config;
  struct PDCP_Config__rlc_UM *PDCP_rlc_UM;
  struct LogicalChannelConfig *DRB_lchan_config;
  struct LogicalChannelConfig__ul_SpecificParameters   *DRB_ul_SpecificParameters;
  // DRB_ToAddModList_t **DRB_configList = &rrc_inst->DRB_configList[UE_index];
  DRB_ToAddModList_t *DRB_configList2;

  MAC_MainConfig_t *mac_MainConfig;
  MeasObjectToAddModList_t *MeasObj_list;
  MeasObjectToAddMod_t *MeasObj;
  ReportConfigToAddModList_t *ReportConfig_list;
  ReportConfigToAddMod_t *ReportConfig_per, *ReportConfig_A1,
    *ReportConfig_A2, *ReportConfig_A3, *ReportConfig_A4, *ReportConfig_A5;
  MeasIdToAddModList_t *MeasId_list;
  MeasIdToAddMod_t *MeasId0, *MeasId1, *MeasId2, *MeasId3, *MeasId4, *MeasId5;
  QuantityConfig_t *quantityConfig;
  MobilityControlInfo_t *mobilityInfo;
  // HandoverCommand_t handoverCommand;
  u8 sourceModId = get_adjacent_cell_mod_id(rrc_inst->handover_info[UE_index]->as_context.reestablishmentInfo->sourcePhysCellId);
#if Rel10
  long *sr_ProhibitTimer_r9;
#endif

  long *logicalchannelgroup, *logicalchannelgroup_drb;
  long *maxHARQ_Tx, *periodicBSR_Timer;

  // RSRP_Range_t *rsrp;
  struct MeasConfig__speedStatePars *Sparams;
  CellsToAddMod_t *CellToAdd;
  CellsToAddModList_t *CellsToAddModList;
  // srb 1: for HO
  struct SRB_ToAddMod *SRB1_config;
  struct SRB_ToAddMod__rlc_Config *SRB1_rlc_config;
  struct SRB_ToAddMod__logicalChannelConfig *SRB1_lchan_config;
  struct LogicalChannelConfig__ul_SpecificParameters *SRB1_ul_SpecificParameters;
  // phy config dedicated
  PhysicalConfigDedicated_t *physicalConfigDedicated2;
  struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList *dedicatedInfoNASList;


  LOG_D(RRC,"[eNB %d] Frame %d: handover preparation: get the newSourceUEIdentity (C-RNTI): ", Mod_id, frame);
  for (i=0;i<2;i++) {
    rv[i]=taus()&0xff;
    LOG_D(RRC," %x.",rv[i]);
  }
 
  LOG_D(RRC,"[eNB %d] Frame %d : handover reparation: add target eNB SRB1 and PHYConfigDedicated reconfiguration\n",Mod_id, frame);
  // 1st: reconfigure SRB
  SRB_configList2 = CALLOC(1,sizeof(*SRB_configList));
  SRB1_config = CALLOC(1,sizeof(*SRB1_config));
  SRB1_config->srb_Identity = 1;
  SRB1_rlc_config = CALLOC(1,sizeof(*SRB1_rlc_config));
  SRB1_config->rlc_Config   = SRB1_rlc_config;
    
  SRB1_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
  SRB1_rlc_config->choice.explicitValue.present=RLC_Config_PR_am;
  SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit=T_PollRetransmit_ms15;
  SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU=PollPDU_p8;
  SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte=PollByte_kB1000;
  SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold=UL_AM_RLC__maxRetxThreshold_t16;
  SRB1_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering=T_Reordering_ms50;
  SRB1_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit=T_StatusProhibit_ms10;
    
  SRB1_lchan_config = CALLOC(1,sizeof(*SRB1_lchan_config));
  SRB1_config->logicalChannelConfig   = SRB1_lchan_config;
  
  SRB1_lchan_config->present = SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;
  SRB1_ul_SpecificParameters = CALLOC(1,sizeof(*SRB1_ul_SpecificParameters));
  
  SRB1_lchan_config->choice.explicitValue.ul_SpecificParameters = SRB1_ul_SpecificParameters;
  
  SRB1_ul_SpecificParameters->priority = 1;
  
  //assign_enum(&SRB1_ul_SpecificParameters->prioritisedBitRate,LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity);
  SRB1_ul_SpecificParameters->prioritisedBitRate=LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
  
  //assign_enum(&SRB1_ul_SpecificParameters->bucketSizeDuration,LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50);
  SRB1_ul_SpecificParameters->bucketSizeDuration=LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;
  
  logicalchannelgroup = CALLOC(1,sizeof(long));
  *logicalchannelgroup=0;
  SRB1_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;
  
  ASN_SEQUENCE_ADD(&SRB_configList2->list,SRB1_config);
    
  //2nd: now reconfigure phy config dedicated 
  physicalConfigDedicated2 = CALLOC(1,sizeof(*physicalConfigDedicated2));
  *physicalConfigDedicated = physicalConfigDedicated2;
  
  physicalConfigDedicated2->pdsch_ConfigDedicated         = CALLOC(1,sizeof(*physicalConfigDedicated2->pdsch_ConfigDedicated));
  physicalConfigDedicated2->pucch_ConfigDedicated         = CALLOC(1,sizeof(*physicalConfigDedicated2->pucch_ConfigDedicated));
  physicalConfigDedicated2->pusch_ConfigDedicated         = CALLOC(1,sizeof(*physicalConfigDedicated2->pusch_ConfigDedicated));
  physicalConfigDedicated2->uplinkPowerControlDedicated   = CALLOC(1,sizeof(*physicalConfigDedicated2->uplinkPowerControlDedicated));
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH         = CALLOC(1,sizeof(*physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH));
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH         = CALLOC(1,sizeof(*physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH));
  physicalConfigDedicated2->cqi_ReportConfig              = NULL;//CALLOC(1,sizeof(*physicalConfigDedicated2->cqi_ReportConfig));
  physicalConfigDedicated2->soundingRS_UL_ConfigDedicated = NULL;//CALLOC(1,sizeof(*physicalConfigDedicated2->soundingRS_UL_ConfigDedicated));
  physicalConfigDedicated2->antennaInfo                   = CALLOC(1,sizeof(*physicalConfigDedicated2->antennaInfo));
  physicalConfigDedicated2->schedulingRequestConfig       = CALLOC(1,sizeof(*physicalConfigDedicated2->schedulingRequestConfig));
  // PDSCH
  //assign_enum(&physicalConfigDedicated2->pdsch_ConfigDedicated->p_a,
  //          PDSCH_ConfigDedicated__p_a_dB0);
  physicalConfigDedicated2->pdsch_ConfigDedicated->p_a=   PDSCH_ConfigDedicated__p_a_dB0;
  
  // PUCCH
  physicalConfigDedicated2->pucch_ConfigDedicated->ackNackRepetition.present=PUCCH_ConfigDedicated__ackNackRepetition_PR_release;
  physicalConfigDedicated2->pucch_ConfigDedicated->ackNackRepetition.choice.release=0;
  physicalConfigDedicated2->pucch_ConfigDedicated->tdd_AckNackFeedbackMode=NULL;//PUCCH_ConfigDedicated__tdd_AckNackFeedbackMode_multiplexing;
  
  // Pusch_config_dedicated
  physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_ACK_Index = 0; // 2.00
  physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_RI_Index  = 0; // 1.25
  physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_CQI_Index = 8; // 2.25
  
  // UplinkPowerControlDedicated
  physicalConfigDedicated2->uplinkPowerControlDedicated->p0_UE_PUSCH = 0; // 0 dB
  //assign_enum(&physicalConfigDedicated2->uplinkPowerControlDedicated->deltaMCS_Enabled,
  // UplinkPowerControlDedicated__deltaMCS_Enabled_en1);
  physicalConfigDedicated2->uplinkPowerControlDedicated->deltaMCS_Enabled= UplinkPowerControlDedicated__deltaMCS_Enabled_en1;
  physicalConfigDedicated2->uplinkPowerControlDedicated->accumulationEnabled = 1;  // FALSE
  physicalConfigDedicated2->uplinkPowerControlDedicated->p0_UE_PUCCH = 0; // 0 dB
  physicalConfigDedicated2->uplinkPowerControlDedicated->pSRS_Offset = 0; // 0 dB
  physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient = CALLOC(1,sizeof(*physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient));
  //  assign_enum(physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient,FilterCoefficient_fc4); // fc4 dB
  *physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient=FilterCoefficient_fc4; // fc4 dB
  
  // TPC-PDCCH-Config
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->present=TPC_PDCCH_Config_PR_setup;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_Index.present = TPC_Index_PR_indexOfFormat3;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_Index.choice.indexOfFormat3 = 1;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf=CALLOC(1,2);
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.size=2;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf[0]=0x12;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf[1]=0x34+UE_index;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.bits_unused=0;
  
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->present=TPC_PDCCH_Config_PR_setup;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_Index.present = TPC_Index_PR_indexOfFormat3;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_Index.choice.indexOfFormat3 = 1;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf=CALLOC(1,2);
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.size=2;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf[0]=0x22;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf[1]=0x34+UE_index;
  physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.bits_unused=0;
  
  // CQI ReportConfig
  /*
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportModeAperiodic=CALLOC(1,sizeof(*physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportModeAperiodic));
    assign_enum(physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportModeAperiodic,
    CQI_ReportConfig__cqi_ReportModeAperiodic_rm30); // HLC CQI, no PMI
    physicalConfigDedicated2->cqi_ReportConfig->nomPDSCH_RS_EPRE_Offset = 0; // 0 dB
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic=CALLOC(1,sizeof(*physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic));
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->present =  CQI_ReportPeriodic_PR_setup;
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.cqi_PUCCH_ResourceIndex = 0;  // n2_pucch
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.cqi_pmi_ConfigIndex = 0;  // Icqi/pmi
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.cqi_FormatIndicatorPeriodic.present = CQI_ReportPeriodic__setup__cqi_FormatIndicatorPeriodic_PR_subbandCQI;  // subband CQI
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.cqi_FormatIndicatorPeriodic.choice.subbandCQI.k=4;
    
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.ri_ConfigIndex=NULL;
    physicalConfigDedicated2->cqi_ReportConfig->cqi_ReportPeriodic->choice.setup.simultaneousAckNackAndCQI=0;
  */
  
  //soundingRS-UL-ConfigDedicated
  /*
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->present = SoundingRS_UL_ConfigDedicated_PR_setup;
    assign_enum(&physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.srs_Bandwidth,
    SoundingRS_UL_ConfigDedicated__setup__srs_Bandwidth_bw0);
    assign_enum(&physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.srs_HoppingBandwidth,
    SoundingRS_UL_ConfigDedicated__setup__srs_HoppingBandwidth_hbw0);
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.freqDomainPosition=0;
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.duration=1;
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.srs_ConfigIndex=1;
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.transmissionComb=0;
    assign_enum(&physicalConfigDedicated2->soundingRS_UL_ConfigDedicated->choice.setup.cyclicShift,
    SoundingRS_UL_ConfigDedicated__setup__cyclicShift_cs0);
  */
  
  //AntennaInfoDedicated
  physicalConfigDedicated2->antennaInfo = CALLOC(1,sizeof(*physicalConfigDedicated2->antennaInfo));
  physicalConfigDedicated2->antennaInfo->present = PhysicalConfigDedicated__antennaInfo_PR_explicitValue;
  //assign_enum(&physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode,
  //     AntennaInfoDedicated__transmissionMode_tm2);
  /*  
  switch (transmission_mode){
  case 1:
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode=     AntennaInfoDedicated__transmissionMode_tm1;
    break;
  case 2:
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode=     AntennaInfoDedicated__transmissionMode_tm2;
    break;
  case 4:
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode=     AntennaInfoDedicated__transmissionMode_tm4;
    break;
  case 5:
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode=     AntennaInfoDedicated__transmissionMode_tm5;
    break;
  case 6:
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.transmissionMode=     AntennaInfoDedicated__transmissionMode_tm6;
    break;
  }
  */
  physicalConfigDedicated2->antennaInfo->choice.explicitValue.ue_TransmitAntennaSelection.present = AntennaInfoDedicated__ue_TransmitAntennaSelection_PR_release;
  physicalConfigDedicated2->antennaInfo->choice.explicitValue.ue_TransmitAntennaSelection.choice.release = 0;
  
  // SchedulingRequestConfig
  physicalConfigDedicated2->schedulingRequestConfig->present = SchedulingRequestConfig_PR_setup;
  physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_PUCCH_ResourceIndex = UE_index;
  
  if (mac_xface->lte_frame_parms->frame_type == 0){ // FDD
    physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 5+(UE_index%10);  // Isr = 5 (every 10 subframes, offset=2+UE_id mod3)
} else {
    switch (mac_xface->lte_frame_parms->tdd_config) {
    case 1:
      physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7+(UE_index&1)+((UE_index&3)>>1)*5;  // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 7 for UE2, 8 for UE3 , 2 for UE4 etc..)
      break;
    case 3:
      physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7+(UE_index%3);  // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 3 for UE2, 2 for UE3 , etc..)
      break;
    case 4:
      physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7+(UE_index&1);  // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 3 for UE2, 2 for UE3 , etc..)
      break;
    default:
      physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7;  // Isr = 5 (every 10 subframes, offset=2 for all UE0 etc..)
      break;
    }
  }
  
  //  assign_enum(&physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax,
  //SchedulingRequestConfig__setup__dsr_TransMax_n4);
  //  assign_enum(&physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax = SchedulingRequestConfig__setup__dsr_TransMax_n4;
  physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax = SchedulingRequestConfig__setup__dsr_TransMax_n4;
   
  LOG_D(RRC,"handover_config [MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (SRB1 UE %d) --->][MAC_eNB][MOD %02d][]\n",
                frame, Mod_id, UE_index, Mod_id);
  rrc_mac_config_req (Mod_id, 1, UE_index, 0,
                      (RadioResourceConfigCommonSIB_t *) NULL,
                      eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index],
                      (MeasObjectToAddMod_t **) NULL,
                      eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index],
                      1,
                      SRB1_logicalChannelConfig,
                      eNB_rrc_inst[Mod_id].measGapConfig[UE_index],
                      (TDD_Config_t *) NULL,
                      (MobilityControlInfo_t *)NULL,
                      (u8 *) NULL,(u16 *) NULL, NULL, NULL, NULL,
                      (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                      ,
                      0,
                      (MBSFN_AreaInfoList_r9_t *) NULL,
                      (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                      ,
                      eNB_rrc_inst[Mod_id].
                      num_active_cba_groups,
                      eNB_rrc_inst[Mod_id].cba_rnti[0]
#endif
                      );
  

  // Configure target eNB SRB2
  /// SRB2
  SRB2_config = CALLOC (1, sizeof (*SRB2_config));
  SRB_configList2 = CALLOC (1, sizeof (*SRB_configList2));
  memset (SRB_configList2, 0, sizeof (*SRB_configList2));

  SRB2_config->srb_Identity = 2;
  SRB2_rlc_config = CALLOC (1, sizeof (*SRB2_rlc_config));
  SRB2_config->rlc_Config = SRB2_rlc_config;

  SRB2_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
  SRB2_rlc_config->choice.explicitValue.present = RLC_Config_PR_am;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms15;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU = PollPDU_p8;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte = PollByte_kB1000;
  SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t32;
  SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms50;
  SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit = T_StatusProhibit_ms10;

  SRB2_lchan_config = CALLOC (1, sizeof (*SRB2_lchan_config));
  SRB2_config->logicalChannelConfig = SRB2_lchan_config;

  SRB2_lchan_config->present =  SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;

  SRB2_ul_SpecificParameters =  CALLOC (1, sizeof (*SRB2_ul_SpecificParameters));

  SRB2_ul_SpecificParameters->priority = 1;
  SRB2_ul_SpecificParameters->prioritisedBitRate = LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
  SRB2_ul_SpecificParameters->bucketSizeDuration = LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

  // LCG for CCCH and DCCH is 0 as defined in 36331
  logicalchannelgroup = CALLOC (1, sizeof (long));
  *logicalchannelgroup = 0;

  SRB2_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;
  SRB2_lchan_config->choice.explicitValue.ul_SpecificParameters =  SRB2_ul_SpecificParameters;
  ASN_SEQUENCE_ADD (&SRB_configList->list, SRB2_config);
  ASN_SEQUENCE_ADD (&SRB_configList2->list, SRB2_config);

  // Configure target eNB DRB
  DRB_configList2 = CALLOC (1, sizeof (*DRB_configList2));
  /// DRB
  DRB_config = CALLOC (1, sizeof (*DRB_config));

  //DRB_config->drb_Identity = (DRB_Identity_t) 1; //allowed values 1..32
  // NN: this is the 1st DRB for this ue, so set it to 1
  DRB_config->drb_Identity = (DRB_Identity_t) 1;        // (UE_index+1); //allowed values 1..32
  DRB_config->logicalChannelIdentity = CALLOC (1, sizeof (long));
  *(DRB_config->logicalChannelIdentity) = (long) 3;
  DRB_rlc_config = CALLOC (1, sizeof (*DRB_rlc_config));
  DRB_config->rlc_Config = DRB_rlc_config;
  DRB_rlc_config->present = RLC_Config_PR_um_Bi_Directional;
  DRB_rlc_config->choice.um_Bi_Directional.ul_UM_RLC.sn_FieldLength =  SN_FieldLength_size10;
  DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.sn_FieldLength = SN_FieldLength_size10;
  DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.t_Reordering = T_Reordering_ms5;

  DRB_pdcp_config = CALLOC (1, sizeof (*DRB_pdcp_config));
  DRB_config->pdcp_Config = DRB_pdcp_config;
  DRB_pdcp_config->discardTimer = NULL;
  DRB_pdcp_config->rlc_AM = NULL;
  PDCP_rlc_UM = CALLOC (1, sizeof (*PDCP_rlc_UM));
  DRB_pdcp_config->rlc_UM = PDCP_rlc_UM;
  PDCP_rlc_UM->pdcp_SN_Size = PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits;
  DRB_pdcp_config->headerCompression.present =  PDCP_Config__headerCompression_PR_notUsed;

  DRB_lchan_config = CALLOC (1, sizeof (*DRB_lchan_config));
  DRB_config->logicalChannelConfig = DRB_lchan_config;
  DRB_ul_SpecificParameters = CALLOC (1, sizeof (*DRB_ul_SpecificParameters));
  DRB_lchan_config->ul_SpecificParameters = DRB_ul_SpecificParameters;


  DRB_ul_SpecificParameters->priority = 2;      // lower priority than srb1, srb2
  DRB_ul_SpecificParameters->prioritisedBitRate = LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
  DRB_ul_SpecificParameters->bucketSizeDuration = LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

  // LCG for DTCH can take the value from 1 to 3 as defined in 36331: normally controlled by upper layers (like RRM)
  logicalchannelgroup_drb = CALLOC (1, sizeof (long));
  *logicalchannelgroup_drb = 1;
  DRB_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup_drb;


  ASN_SEQUENCE_ADD (&DRB_configList2->list, DRB_config);

  mac_MainConfig = CALLOC (1, sizeof (*mac_MainConfig));
  eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index] = mac_MainConfig;

  mac_MainConfig->ul_SCH_Config = CALLOC (1, sizeof (*mac_MainConfig->ul_SCH_Config));

  maxHARQ_Tx = CALLOC (1, sizeof (long));
  *maxHARQ_Tx = MAC_MainConfig__ul_SCH_Config__maxHARQ_Tx_n5;
  mac_MainConfig->ul_SCH_Config->maxHARQ_Tx = maxHARQ_Tx;

  periodicBSR_Timer = CALLOC (1, sizeof (long));
  *periodicBSR_Timer = MAC_MainConfig__ul_SCH_Config__periodicBSR_Timer_sf64;
  mac_MainConfig->ul_SCH_Config->periodicBSR_Timer = periodicBSR_Timer;

  mac_MainConfig->ul_SCH_Config->retxBSR_Timer = MAC_MainConfig__ul_SCH_Config__retxBSR_Timer_sf320;

  mac_MainConfig->ul_SCH_Config->ttiBundling = 0;       // FALSE

  mac_MainConfig->drx_Config = NULL;

  mac_MainConfig->phr_Config = CALLOC (1, sizeof (*mac_MainConfig->phr_Config));

  mac_MainConfig->phr_Config->present = MAC_MainConfig__phr_Config_PR_setup;
  mac_MainConfig->phr_Config->choice.setup.periodicPHR_Timer = MAC_MainConfig__phr_Config__setup__periodicPHR_Timer_sf20;       // sf20 = 20 subframes

  mac_MainConfig->phr_Config->choice.setup.prohibitPHR_Timer = MAC_MainConfig__phr_Config__setup__prohibitPHR_Timer_sf20;       // sf20 = 20 subframes

  mac_MainConfig->phr_Config->choice.setup.dl_PathlossChange = MAC_MainConfig__phr_Config__setup__dl_PathlossChange_dB1;        // Value dB1 =1 dB, dB3 = 3 dB

#ifdef Rel10
  sr_ProhibitTimer_r9 = CALLOC (1, sizeof (long));
  *sr_ProhibitTimer_r9 = 0;     // SR tx on PUCCH, Value in number of SR period(s). Value 0 = no timer for SR, Value 2= 2*SR
  mac_MainConfig->sr_ProhibitTimer_r9 = sr_ProhibitTimer_r9;
  //sps_RA_ConfigList_rlola = NULL;
#endif
  // Measurement ID list
  MeasId_list = CALLOC (1, sizeof (*MeasId_list));
  memset ((void *) MeasId_list, 0, sizeof (*MeasId_list));

  MeasId0 = CALLOC (1, sizeof (*MeasId0));
  MeasId0->measId = 1;
  MeasId0->measObjectId = 1;
  MeasId0->reportConfigId = 1;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId0);

  MeasId1 = CALLOC (1, sizeof (*MeasId1));
  MeasId1->measId = 2;
  MeasId1->measObjectId = 1;
  MeasId1->reportConfigId = 2;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId1);

  MeasId2 = CALLOC (1, sizeof (*MeasId2));
  MeasId2->measId = 3;
  MeasId2->measObjectId = 1;
  MeasId2->reportConfigId = 3;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId2);

  MeasId3 = CALLOC (1, sizeof (*MeasId3));
  MeasId3->measId = 4;
  MeasId3->measObjectId = 1;
  MeasId3->reportConfigId = 4;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId3);

  MeasId4 = CALLOC (1, sizeof (*MeasId4));
  MeasId4->measId = 5;
  MeasId4->measObjectId = 1;
  MeasId4->reportConfigId = 5;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId4);

  MeasId5 = CALLOC (1, sizeof (*MeasId5));
  MeasId5->measId = 6;
  MeasId5->measObjectId = 1;
  MeasId5->reportConfigId = 6;
  ASN_SEQUENCE_ADD (&MeasId_list->list, MeasId5);

  //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measIdToAddModList = MeasId_list;

  // Add one EUTRA Measurement Object
  MeasObj_list = CALLOC (1, sizeof (*MeasObj_list));
  memset ((void *) MeasObj_list, 0, sizeof (*MeasObj_list));

  // Configure MeasObject

  MeasObj = CALLOC (1, sizeof (*MeasObj));
  memset ((void *) MeasObj, 0, sizeof (*MeasObj));

  MeasObj->measObjectId = 1;
  MeasObj->measObject.present = MeasObjectToAddMod__measObject_PR_measObjectEUTRA;
  MeasObj->measObject.choice.measObjectEUTRA.carrierFreq = 36090;
  MeasObj->measObject.choice.measObjectEUTRA.allowedMeasBandwidth =  AllowedMeasBandwidth_mbw25;
  MeasObj->measObject.choice.measObjectEUTRA.presenceAntennaPort1 = 1;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf = CALLOC (1, sizeof (uint8_t));
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf[0] = 0;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.size = 1;
  MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.bits_unused = 6;
  MeasObj->measObject.choice.measObjectEUTRA.offsetFreq = NULL; // Default is 15 or 0dB

  MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList = (CellsToAddModList_t *) CALLOC (1, sizeof (*CellsToAddModList));
  CellsToAddModList = MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList;

  // Add adjacent cell lists (6 per eNB)
  for (i = 0; i < 6; i++)   {
      CellToAdd = (CellsToAddMod_t *) CALLOC (1, sizeof (*CellToAdd));
      CellToAdd->cellIndex = i + 1;
      CellToAdd->physCellId = get_adjacent_cell_id (Mod_id, i);
      CellToAdd->cellIndividualOffset = Q_OffsetRange_dB0;

      ASN_SEQUENCE_ADD (&CellsToAddModList->list, CellToAdd);
  }

  ASN_SEQUENCE_ADD (&MeasObj_list->list, MeasObj);
  //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measObjectToAddModList = MeasObj_list;

  // Report Configurations for periodical, A1-A5 events
  ReportConfig_list = CALLOC (1, sizeof (*ReportConfig_list));

  ReportConfig_per = CALLOC (1, sizeof (*ReportConfig_per));

  ReportConfig_A1 = CALLOC (1, sizeof (*ReportConfig_A1));

  ReportConfig_A2 = CALLOC (1, sizeof (*ReportConfig_A2));

  ReportConfig_A3 = CALLOC (1, sizeof (*ReportConfig_A3));

  ReportConfig_A4 = CALLOC (1, sizeof (*ReportConfig_A4));

  ReportConfig_A5 = CALLOC (1, sizeof (*ReportConfig_A5));

  ReportConfig_per->reportConfigId = 1;
  ReportConfig_per->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.present = ReportConfigEUTRA__triggerType_PR_periodical;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.choice.periodical.purpose =ReportConfigEUTRA__triggerType__periodical__purpose_reportStrongestCells;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
  ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

  ASN_SEQUENCE_ADD (&ReportConfig_list->list, ReportConfig_per);

  ReportConfig_A1->reportConfigId = 2;
  ReportConfig_A1->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.present = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA1;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.a1_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.a1_Threshold.choice.threshold_RSRP = 10;

  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
  ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

  ASN_SEQUENCE_ADD (&ReportConfig_list->list, ReportConfig_A1);

  ReportConfig_A2->reportConfigId                                                              = 3;
  ReportConfig_A2->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.present                    = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA2;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.a2_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.a2_Threshold.choice.threshold_RSRP = 10;
  
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
  ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
  
  ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A2);
  
  ReportConfig_A3->reportConfigId                                                              = 4;
  ReportConfig_A3->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.present                     = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present= ReportConfigEUTRA__triggerType__event__eventId_PR_eventA3;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.a3_Offset = 10;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.reportOnLeave = 1;
    
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
  ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
  
  ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A3);
  
  ReportConfig_A4->reportConfigId                                                              = 5;
  ReportConfig_A4->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.present                   = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present= ReportConfigEUTRA__triggerType__event__eventId_PR_eventA4;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.a4_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.a4_Threshold.choice.threshold_RSRP = 10;
  
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
  ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
  
  ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A4);
  
  ReportConfig_A5->reportConfigId                                                              = 6;
  ReportConfig_A5->reportConfig.present                                                        = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.present                   = ReportConfigEUTRA__triggerType_PR_event;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present = ReportConfigEUTRA__triggerType__event__eventId_PR_eventA5;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold1.present = ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold2.present = ThresholdEUTRA_PR_threshold_RSRP;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold1.choice.threshold_RSRP = 10;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA5.a5_Threshold2.choice.threshold_RSRP = 10;
  
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerQuantity                       = ReportConfigEUTRA__triggerQuantity_rsrp;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportQuantity                        = ReportConfigEUTRA__reportQuantity_both;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.maxReportCells                        = 2;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportInterval                        = ReportInterval_ms120;
  ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportAmount                          = ReportConfigEUTRA__reportAmount_infinity;
  
  ASN_SEQUENCE_ADD(&ReportConfig_list->list,ReportConfig_A5);
  
  Sparams = CALLOC(1,sizeof(*Sparams));
  Sparams->present=MeasConfig__speedStatePars_PR_setup;
  Sparams->choice.setup.timeToTrigger_SF.sf_High=SpeedStateScaleFactors__sf_Medium_oDot75;
  Sparams->choice.setup.timeToTrigger_SF.sf_Medium=SpeedStateScaleFactors__sf_High_oDot5;
  Sparams->choice.setup.mobilityStateParameters.n_CellChangeHigh=10;
  Sparams->choice.setup.mobilityStateParameters.n_CellChangeMedium=5;
  Sparams->choice.setup.mobilityStateParameters.t_Evaluation=MobilityStateParameters__t_Evaluation_s60;
  Sparams->choice.setup.mobilityStateParameters.t_HystNormal=MobilityStateParameters__t_HystNormal_s120;
  
  quantityConfig = CALLOC(1,sizeof(*quantityConfig));
  memset((void *)quantityConfig,0,sizeof(*quantityConfig));
  quantityConfig->quantityConfigEUTRA = CALLOC(1,sizeof(*quantityConfig->quantityConfigEUTRA));
  memset((void *)quantityConfig->quantityConfigEUTRA,0,sizeof(*quantityConfig->quantityConfigEUTRA));
  quantityConfig->quantityConfigCDMA2000 = NULL;
  quantityConfig->quantityConfigGERAN = NULL;
  quantityConfig->quantityConfigUTRA = NULL;
  quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = CALLOC(1,sizeof(*quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP));
  quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = CALLOC(1,sizeof(*quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ));
  *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = FilterCoefficient_fc4;
  *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = FilterCoefficient_fc4;


  /* mobilityinfo  */

  mobilityInfo = CALLOC(1,sizeof(*mobilityInfo));
  memset((void *)mobilityInfo,0,sizeof(*mobilityInfo));
  mobilityInfo->targetPhysCellId = (PhysCellId_t) two_tier_hexagonal_cellIds[rrc_inst->handover_info[UE_index]->modid_t];
  LOG_D(RRC,"[eNB %d] Frame %d: handover preparation: targetPhysCellId: %d mod_id: %d UE_index: %d \n",
        Mod_id,frame, mobilityInfo->targetPhysCellId,Mod_id,UE_index);

  mobilityInfo->additionalSpectrumEmission = CALLOC(1,sizeof(*mobilityInfo->additionalSpectrumEmission));
  *mobilityInfo->additionalSpectrumEmission = 1; //Check this value!
  
  mobilityInfo->t304 = MobilityControlInfo__t304_ms50; // need to configure an appropriate value here
  
  // New UE Identity (C-RNTI) to identify an UE uniquely in a cell
  mobilityInfo->newUE_Identity.size = 2;
  mobilityInfo->newUE_Identity.bits_unused = 0;
  mobilityInfo->newUE_Identity.buf = rv;
  mobilityInfo->newUE_Identity.buf[0] = rv[0];
  mobilityInfo->newUE_Identity.buf[1] = rv[1];
  
  //memset((void *)&mobilityInfo->radioResourceConfigCommon,(void *)&rrc_inst->sib2->radioResourceConfigCommon,sizeof(RadioResourceConfigCommon_t));
  //memset((void *)&mobilityInfo->radioResourceConfigCommon,0,sizeof(RadioResourceConfigCommon_t));

  // Configuring radioResourceConfigCommon
  mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.rach_ConfigCommon,
         sizeof(RACH_ConfigCommon_t));
  mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo,
         sizeof(PRACH_ConfigInfo_t));
  
  mobilityInfo->radioResourceConfigCommon.prach_Config.rootSequenceIndex = rrc_inst->sib2->radioResourceConfigCommon.prach_Config.rootSequenceIndex;
  mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.pdsch_ConfigCommon,
         sizeof(PDSCH_ConfigCommon_t));
  memcpy((void *)&mobilityInfo->radioResourceConfigCommon.pusch_ConfigCommon,
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.pusch_ConfigCommon,
         sizeof(PUSCH_ConfigCommon_t));
  mobilityInfo->radioResourceConfigCommon.phich_Config = NULL;
  mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.pucch_ConfigCommon,
         sizeof(PUCCH_ConfigCommon_t));
  mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.soundingRS_UL_ConfigCommon,
         sizeof(SoundingRS_UL_ConfigCommon_t));
  mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon = CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon));
  memcpy((void *)mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon, 
         (void *)&rrc_inst->sib2->radioResourceConfigCommon.uplinkPowerControlCommon,
         sizeof(UplinkPowerControlCommon_t));
  mobilityInfo->radioResourceConfigCommon.antennaInfoCommon = NULL;
  mobilityInfo->radioResourceConfigCommon.p_Max = NULL; // CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.p_Max));
  //memcpy((void *)mobilityInfo->radioResourceConfigCommon.p_Max,(void *)rrc_inst->sib1->p_Max,sizeof(P_Max_t));
  mobilityInfo->radioResourceConfigCommon.tdd_Config = NULL; //CALLOC(1,sizeof(TDD_Config_t));
  //memcpy((void *)mobilityInfo->radioResourceConfigCommon.tdd_Config,(void *)rrc_inst->sib1->tdd_Config,sizeof(TDD_Config_t));
  mobilityInfo->radioResourceConfigCommon.ul_CyclicPrefixLength = rrc_inst->sib2->radioResourceConfigCommon.ul_CyclicPrefixLength;
  //End of configuration of radioResourceConfigCommon

  mobilityInfo->carrierFreq = CALLOC(1,sizeof(*mobilityInfo->carrierFreq)); //CALLOC(1,sizeof(CarrierFreqEUTRA_t)); 36090
  mobilityInfo->carrierFreq->dl_CarrierFreq = 36090;
  mobilityInfo->carrierFreq->ul_CarrierFreq = NULL;
  
  mobilityInfo->carrierBandwidth = CALLOC(1,sizeof(*mobilityInfo->carrierBandwidth)); //CALLOC(1,sizeof(struct CarrierBandwidthEUTRA));  AllowedMeasBandwidth_mbw25
  mobilityInfo->carrierBandwidth->dl_Bandwidth = CarrierBandwidthEUTRA__dl_Bandwidth_n25;
  mobilityInfo->carrierBandwidth->ul_Bandwidth = NULL;
  mobilityInfo->rach_ConfigDedicated = NULL;
  

  // store the srb and drb list for ho management, mainly in case of failure
  
  memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.srb_ToAddModList,
         (void *)SRB_configList2,
         sizeof(SRB_ToAddModList_t));
  memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.drb_ToAddModList,
         (void *)DRB_configList2,
         sizeof(DRB_ToAddModList_t));
    rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.drb_ToReleaseList = NULL;
    memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.mac_MainConfig,
           (void *)mac_MainConfig,
           sizeof(MAC_MainConfig_t));
    memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.physicalConfigDedicated,
           (void *)rrc_inst->physicalConfigDedicated[UE_index],
           sizeof(PhysicalConfigDedicated_t));
    /*    memcpy((void *)rrc_inst->handover_info[UE_index]->as_config.sourceRadioResourceConfig.sps_Config,
           (void *)rrc_inst->sps_Config[UE_index],
           sizeof(SPS_Config_t));
    */
    LOG_I(RRC,"[eNB %d] Frame %d: adding new UE\n");
    Idx = (UE_index * NB_RB_MAX) + DCCH;
    // SRB1
    eNB_rrc_inst[Mod_id].Srb1[UE_index].Active = 1;
    eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.Srb_id = Idx;
    memcpy (&eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.Lchan_desc[0], 
            &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    memcpy (&eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.Lchan_desc[1], 
            &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    
    // SRB2
    eNB_rrc_inst[Mod_id].Srb2[UE_index].Active = 1;
    eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.Srb_id = Idx;
    memcpy (&eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.Lchan_desc[0], 
            &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    memcpy (&eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.Lchan_desc[1], 
            &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    
    LOG_I (RRC,"[eNB %d] CALLING RLC CONFIG SRB1 (rbid %d) for UE %d\n",Mod_id, Idx, UE_index);
    
    //      rrc_pdcp_config_req (Mod_id, frame, 1, ACTION_ADD, idx, UNDEF_SECURITY_MODE);
    //      rrc_rlc_config_req(Mod_id,frame,1,ACTION_ADD,Idx,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
    
    rrc_pdcp_config_asn1_req (Mod_id, UE_index, frame, 1,
                              eNB_rrc_inst[Mod_id].SRB_configList[UE_index],
                              (DRB_ToAddModList_t *) NULL,
                              (DRB_ToReleaseList_t *) NULL,
                              0xff,
                              NULL,
                              NULL,
                              NULL
#ifdef Rel10
                              , (PMCH_InfoList_r9_t *) NULL
#endif
                              );
    
    rrc_rlc_config_asn1_req (Mod_id, frame, 1, UE_index,
                             eNB_rrc_inst[Mod_id].SRB_configList[UE_index],
                             (DRB_ToAddModList_t *) NULL,
                             (DRB_ToReleaseList_t *) NULL
#ifdef Rel10
                             , (PMCH_InfoList_r9_t *) NULL
#endif
                             );
    
    /* Initialize NAS list */
    dedicatedInfoNASList = NULL;

  //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->reportConfigToAddModList = ReportConfig_list;
    memset (buffer, 0, RRC_BUF_SIZE);

    size = do_RRCConnectionReconfiguration (Mod_id, buffer, UE_index, rrc_eNB_get_next_transaction_identifier(Mod_id),  //Transaction_id,
                                          SRB_configList2, DRB_configList2, NULL,       // DRB2_list,
                                          NULL, //*sps_Config,
                                          physicalConfigDedicated[UE_index], MeasObj_list, ReportConfig_list,
                                          NULL, //quantityConfig,
                                          MeasId_list, mac_MainConfig, NULL, mobilityInfo,Sparams,
                                          NULL, NULL, dedicatedInfoNASList);

    LOG_I (RRC,"[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate RRCConnectionReconfiguration for handover (bytes %d, UE id %d)\n",
         Mod_id, frame, size, UE_index);
  // to be updated if needed
  /*if (eNB_rrc_inst[Mod_id].SRB1_config[UE_index]->logicalChannelConfig) {
    if (eNB_rrc_inst[Mod_id].SRB1_config[UE_index]->logicalChannelConfig->present == SRB_ToAddMod__logicalChannelConfig_PR_explicitValue) {
      SRB1_logicalChannelConfig = &eNB_rrc_inst[Mod_id].SRB1_config[UE_index]->logicalChannelConfig->choice.explicitValue;
    }
    else {
      SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
    }
  }
  else {
    SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
  }
  */

  LOG_D (RRC,"[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionReconfiguration_handover to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
         frame, Mod_id, size, UE_index, rrc_eNB_mui, Mod_id,
         (UE_index * NB_RB_MAX) + DCCH);
  
  //rrc_rlc_data_req(Mod_id,frame, 1,(UE_index*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
  //pdcp_data_req (Mod_id, frame, 1, (UE_index * NB_RB_MAX) + DCCH,rrc_eNB_mui++, 0, size, (char *) buffer, 1);
   rrc_mac_config_req (Mod_id, 1, UE_index, 0,
                      (RadioResourceConfigCommonSIB_t *) NULL,
                      eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index],
                      (MeasObjectToAddMod_t **) NULL,
                      eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index],
                      1,
                      SRB1_logicalChannelConfig,
                      eNB_rrc_inst[Mod_id].measGapConfig[UE_index],
                      (TDD_Config_t *) NULL,
                      (MobilityControlInfo_t *)mobilityInfo,
                      (u8 *) NULL,(u16 *) NULL, NULL, NULL, NULL,
                      (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                      ,
                      0,
                      (MBSFN_AreaInfoList_r9_t *) NULL,
                      (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                       ,
                       0,
                       0,
#endif
                      );
 

  /*
  handoverCommand.criticalExtensions.present = HandoverCommand__criticalExtensions_PR_c1;
  handoverCommand.criticalExtensions.choice.c1.present = HandoverCommand__criticalExtensions__c1_PR_handoverCommand_r8;
  handoverCommand.criticalExtensions.choice.c1.choice.handoverCommand_r8.handoverCommandMessage.buf = buffer;
  handoverCommand.criticalExtensions.choice.c1.choice.handoverCommand_r8.handoverCommandMessage.size = size;
  */

  if (sourceModId != 0xFF) {
    memcpy(eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[Mod_id].handover_info[UE_index]->ueid_s]->buf,
           (void *)buffer,
           size);
    eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[Mod_id].handover_info[UE_index]->ueid_s]->size = size;
    eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[Mod_id].handover_info[UE_index]->ueid_s]->ho_complete = 0xF1;
    //eNB_rrc_inst[Mod_id].handover_info[UE_index]->ho_complete = 0xFF;
    LOG_D(RRC,"[eNB %d] Frame %d: setting handover complete to 0xF1 for (%d,%d) and to 0xFF for (%d,%d)\n", 
          Mod_id, frame,
          sourceModId,eNB_rrc_inst[Mod_id].handover_info[UE_index]->ueid_s,
          Mod_id, UE_index);
  }
  else
    LOG_W(RRC,"[eNB %d] Frame %d: rrc_eNB_generate_RRCConnectionReconfiguration_handover: Could not find source eNB mod_id.\n", 
          Mod_id, frame);


}

/*
  void ue_rrc_process_rrcConnectionReconfiguration(u8 Mod_id,u32 frame,
  RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,
  u8 CH_index) {

  if (rrcConnectionReconfiguration->criticalExtensions.present == RRCConnectionReconfiguration__criticalExtensions_PR_c1)
  if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.present == RRCConnectionReconfiguration__criticalExtensions__c1_PR_rrcConnectionReconfiguration_r8) {

  if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated) {
  rrc_ue_process_radioResourceConfigDedicated(Mod_id,frame,CH_index,
  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated);


  }

  // check other fields for
  }
  }
*/

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_RRCConnectionReconfigurationComplete (u8 Mod_id, u32 frame,
                                                      u8 UE_index,
                                                      RRCConnectionReconfigurationComplete_r8_IEs_t *rrcConnectionReconfigurationComplete)
{
  int i;
#ifdef NAS_NETLINK
  int oip_ifup = 0;
  int dest_ip_offset = 0;
#endif

  uint8_t *kRRCenc = NULL;
  uint8_t *kRRCint = NULL;
  uint8_t *kUPenc  = NULL;

  DRB_ToAddModList_t *DRB_configList = eNB_rrc_inst[Mod_id].DRB_configList[UE_index];
  SRB_ToAddModList_t *SRB_configList = eNB_rrc_inst[Mod_id].SRB_configList[UE_index];

#if defined(ENABLE_SECURITY)
  /* Derive the keys from kenb */
  if (DRB_configList != NULL) {
    derive_key_up_enc(eNB_rrc_inst[Mod_id].ciphering_algorithm[UE_index],
                      eNB_rrc_inst[Mod_id].kenb[UE_index], &kUPenc);
  }

  derive_key_rrc_enc(eNB_rrc_inst[Mod_id].ciphering_algorithm[UE_index],
                     eNB_rrc_inst[Mod_id].kenb[UE_index], &kRRCenc);
  derive_key_rrc_int(eNB_rrc_inst[Mod_id].integrity_algorithm[UE_index],
                     eNB_rrc_inst[Mod_id].kenb[UE_index], &kRRCint);
#endif

  // Refresh SRBs/DRBs
  rrc_pdcp_config_asn1_req (Mod_id, UE_index, frame, 1,
                            SRB_configList,
                            DRB_configList, (DRB_ToReleaseList_t *) NULL,
                            eNB_rrc_inst[Mod_id].ciphering_algorithm[UE_index] |
                            (eNB_rrc_inst[Mod_id].integrity_algorithm[UE_index] << 4),
                            kRRCenc,
                            kRRCint,
                            kUPenc
#ifdef Rel10
                            , (PMCH_InfoList_r9_t *) NULL
#endif
    );
  // Refresh SRBs/DRBs
  rrc_rlc_config_asn1_req (Mod_id, frame, 1, UE_index,
                           SRB_configList,
                           DRB_configList, (DRB_ToReleaseList_t *) NULL
#ifdef Rel10
                           , (PMCH_InfoList_r9_t *) NULL
#endif
    );


  // Loop through DRBs and establish if necessary

  if (DRB_configList != NULL)
    {
      for (i = 0; i < DRB_configList->list.count; i++)
        {                       // num max DRB (11-3-8)
          if (DRB_configList->list.array[i])
            {
              LOG_I (RRC,
                     "[eNB %d] Frame  %d : Logical Channel UL-DCCH, Received RRCConnectionReconfigurationComplete from UE %d, reconfiguring DRB %d/LCID %d\n",
                     Mod_id, frame, UE_index,
                     (int) DRB_configList->list.array[i]->drb_Identity,
                     (UE_index * NB_RB_MAX) +
                     (int) *DRB_configList->list.array[i]->
                     logicalChannelIdentity);
              if (eNB_rrc_inst[Mod_id].DRB_active[UE_index][i] == 0)
                {
                  /*
                     rrc_pdcp_config_req (Mod_id, frame, 1, ACTION_ADD,
                     (UE_index * NB_RB_MAX) + *DRB_configList->list.array[i]->logicalChannelIdentity,UNDEF_SECURITY_MODE);
                     rrc_rlc_config_req(Mod_id,frame,1,ACTION_ADD,
                     (UE_index * NB_RB_MAX) + (int)*eNB_rrc_inst[Mod_id].DRB_config[UE_index][i]->logicalChannelIdentity,
                     RADIO_ACCESS_BEARER,Rlc_info_um);
                   */
                  eNB_rrc_inst[Mod_id].DRB_active[UE_index][i] = 1;

                  LOG_D (RRC,
                         "[eNB %d] Frame %d: Establish RLC UM Bidirectional, DRB %d Active\n",
                         Mod_id, frame,
                         (int) DRB_configList->list.array[i]->drb_Identity);

#ifdef NAS_NETLINK
                  // can mean also IPV6 since ether -> ipv6 autoconf
#if !defined(OAI_NW_DRIVER_TYPE_ETHERNET) && !defined(EXMIMO)
                  LOG_I (OIP,
                         "[eNB %d] trying to bring up the OAI interface oai%d\n",
                         Mod_id, Mod_id);
                  oip_ifup = nas_config (Mod_id,        // interface index
                                         Mod_id + 1,    // thrid octet
                                         Mod_id + 1);   // fourth octet

                  if (oip_ifup == 0)
                    {           // interface is up --> send a config the DRB
#ifdef OAI_EMU
                      oai_emulation.info.oai_ifup[Mod_id] = 1;
                      dest_ip_offset = NB_eNB_INST;
#else
                      dest_ip_offset = 8;
#endif
                      LOG_I (OIP,
                             "[eNB %d] Config the oai%d to send/receive pkt on DRB %d to/from the protocol stack\n",
                             Mod_id, Mod_id,
                             (UE_index * NB_RB_MAX) +
                             *DRB_configList->list.array[i]->
                             logicalChannelIdentity);
                      rb_conf_ipv4 (0,  //add
                                    UE_index,   //cx
                                    Mod_id,     //inst
                                    (UE_index * NB_RB_MAX) + *DRB_configList->list.array[i]->logicalChannelIdentity, 0, //dscp
                                    ipv4_address (Mod_id + 1, Mod_id + 1),      //saddr
                                    ipv4_address (Mod_id + 1, dest_ip_offset + UE_index + 1));  //daddr

                      LOG_D (RRC, "[eNB %d] State = Attached (UE %d)\n",
                             Mod_id, UE_index);
                    }
#else
#ifdef OAI_EMU
                  oai_emulation.info.oai_ifup[Mod_id] = 1;
#endif
#endif
#endif

                  LOG_D (RRC,
                         "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB][MOD %02d][]\n",
                         frame, Mod_id, UE_index, Mod_id);
                  if (DRB_configList->list.array[i]->logicalChannelIdentity)
                    DRB2LCHAN[i] =
                      (u8) *
                      DRB_configList->list.array[i]->logicalChannelIdentity;
                  rrc_mac_config_req (Mod_id, 1, UE_index, 0,
                                      (RadioResourceConfigCommonSIB_t *) NULL,
                                      eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index],
                                      (MeasObjectToAddMod_t **) NULL,
                                      eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index], 
                                      DRB2LCHAN[i],
                                      DRB_configList->list.array[i]->logicalChannelConfig,
                                      eNB_rrc_inst[Mod_id].measGapConfig[UE_index],
                                      (TDD_Config_t *) NULL, 
                                      NULL,
                                      (u8 *) NULL,
                                      (u16 *) NULL, NULL, NULL, NULL,
                                      (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                                      ,
                                      0,
                                      (MBSFN_AreaInfoList_r9_t *) NULL,
                                      (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                      ,
                                      eNB_rrc_inst[Mod_id].
                                      num_active_cba_groups,
                                      eNB_rrc_inst[Mod_id].cba_rnti[0]
#endif
                                      );

                }
              else
                {               // remove LCHAN from MAC/PHY

                  if (eNB_rrc_inst[Mod_id].DRB_active[UE_index][i] == 1)
                    {
                      // DRB has just been removed so remove RLC + PDCP for DRB
                      /*      rrc_pdcp_config_req (Mod_id, frame, 1, ACTION_REMOVE,
                         (UE_index * NB_RB_MAX) + DRB2LCHAN[i],UNDEF_SECURITY_MODE);
                       */
                      rrc_rlc_config_req (Mod_id, frame, 1, ACTION_REMOVE,
                                          (UE_index * NB_RB_MAX) +
                                          DRB2LCHAN[i], RADIO_ACCESS_BEARER,
                                          Rlc_info_um);
                    }
                  eNB_rrc_inst[Mod_id].DRB_active[UE_index][i] = 0;
                  LOG_D (RRC,
                         "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB][MOD %02d][]\n",
                         frame, Mod_id, UE_index, Mod_id);

                  rrc_mac_config_req (Mod_id, 1, UE_index, 0,
                                      (RadioResourceConfigCommonSIB_t *) NULL,
                                      eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index],
                                      (MeasObjectToAddMod_t **) NULL,
                                      eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index], 
                                      DRB2LCHAN[i],
                                      (LogicalChannelConfig_t *) NULL,
                                      (MeasGapConfig_t *) NULL,
                                      (TDD_Config_t *) NULL, 
                                      NULL,
                                      (u8 *) NULL,
                                      (u16 *) NULL, NULL, NULL, NULL, NULL
#ifdef Rel10
                                      ,0,
                                      (MBSFN_AreaInfoList_r9_t *) NULL,
                                      (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                      ,0, 
                                      0
#endif
                    );
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_RRCConnectionSetup (u8 Mod_id, u32 frame, u16 UE_index) {

  LogicalChannelConfig_t *SRB1_logicalChannelConfig;    //,*SRB2_logicalChannelConfig;
  SRB_ToAddModList_t **SRB_configList;
  SRB_ToAddMod_t *SRB1_config;
  int cnt;

  DevCheck(UE_index < NUMBER_OF_UE_MAX, UE_index, NUMBER_OF_UE_MAX, 0);

  SRB_configList = &eNB_rrc_inst[Mod_id].SRB_configList[UE_index];

  eNB_rrc_inst[Mod_id].Srb0.Tx_buffer.payload_size =
    do_RRCConnectionSetup ((u8 *) eNB_rrc_inst[Mod_id].Srb0.Tx_buffer.Payload,
                           mac_xface->get_transmission_mode (Mod_id,
                                                             find_UE_RNTI
                                                             (Mod_id,
                                                              UE_index)),
                           UE_index,
                           rrc_eNB_get_next_transaction_identifier(Mod_id),
                           mac_xface->lte_frame_parms,
                           SRB_configList,
                           &eNB_rrc_inst[Mod_id].
                           physicalConfigDedicated[UE_index]);

  // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE

  if (*SRB_configList != NULL)
    {
      for (cnt = 0; cnt < (*SRB_configList)->list.count; cnt++)
        {
          if ((*SRB_configList)->list.array[cnt]->srb_Identity == 1)
          {
              SRB1_config = (*SRB_configList)->list.array[cnt];
              if (SRB1_config->logicalChannelConfig)
                {
                  if (SRB1_config->logicalChannelConfig->present == SRB_ToAddMod__logicalChannelConfig_PR_explicitValue)
                    {
                      SRB1_logicalChannelConfig = &SRB1_config->logicalChannelConfig->choice.explicitValue;
                    }
                  else
                    {
                      SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
                    }
                }
              else
                {
                  SRB1_logicalChannelConfig =  &SRB1_logicalChannelConfig_defaultValue;
                }

              LOG_D (RRC,
                     "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (SRB1 UE %d) --->][MAC_eNB][MOD %02d][]\n",
                     frame, Mod_id, UE_index, Mod_id);
              rrc_mac_config_req (Mod_id, 1, UE_index, 0,
                                  (RadioResourceConfigCommonSIB_t *) NULL,
                                  eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index],
                                  (MeasObjectToAddMod_t **) NULL,
                                  eNB_rrc_inst[Mod_id].mac_MainConfig[UE_index], 
                                  1,
                                  SRB1_logicalChannelConfig,
                                  eNB_rrc_inst[Mod_id].measGapConfig[UE_index],
                                  (TDD_Config_t *) NULL, 
                                  NULL,
                                  (u8 *) NULL,
                                  (u16 *) NULL, NULL, NULL, NULL,
                                  (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                                  ,0,
                                  (MBSFN_AreaInfoList_r9_t *) NULL,
                                  (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                  ,0, 
                                  0
#endif
                );
              break;
            }
        }
    }
  LOG_I (RRC,
         "[eNB %d][RAPROC] Frame %d : Logical Channel DL-CCCH, Generating RRCConnectionSetup (bytes %d, UE %d)\n",
         Mod_id, frame, eNB_rrc_inst[Mod_id].Srb0.Tx_buffer.payload_size,
         UE_index);

}

/*------------------------------------------------------------------------------*/
#if defined(ENABLE_ITTI)
char openair_rrc_lite_eNB_init (u8 Mod_id)
{
    /* Dummy function, initialization will be done through ITTI messaging */
    return 0;
}
char openair_rrc_lite_eNB_configuration (u8 Mod_id, RrcConfigurationReq *configuration)
#else
char openair_rrc_lite_eNB_init (u8 Mod_id)
#endif
{
  /*-----------------------------------------------------------------------------*/
  unsigned char j;
  LOG_I (RRC, "[eNB %d] Init (UE State = RRC_IDLE)...\n", Mod_id);
  LOG_D (RRC, "[MSC_NEW][FRAME 00000][RRC_eNB][MOD %02d][]\n", Mod_id);
  LOG_D (RRC, "[MSC_NEW][FRAME 00000][IP][MOD %02d][]\n", Mod_id);

  DevAssert(eNB_rrc_inst != NULL);

  for (j = 0; j < NUMBER_OF_UE_MAX; j++)
    eNB_rrc_inst[Mod_id].Info.UE[j].Status = RRC_IDLE;     //CH_READY;

#if defined(ENABLE_USE_MME)
  /* Connect eNB to MME */
  if (EPC_MODE_ENABLED > 0)
    {
# if !defined(ENABLE_ITTI)
      if (s1ap_eNB_init (EPC_MODE_MME_ADDRESS, Mod_id) < 0)
        {
          mac_xface->macphy_exit ("");
          return -1;
        }
# endif
    }
  else
#endif
  {
    /* Init security parameters */
    for (j = 0; j < NUMBER_OF_UE_MAX; j++) {
      eNB_rrc_inst[Mod_id].ciphering_algorithm[j] = SecurityAlgorithmConfig__cipheringAlgorithm_eea2;
      eNB_rrc_inst[Mod_id].integrity_algorithm[j] = SecurityAlgorithmConfig__integrityProtAlgorithm_eia2;
      rrc_lite_eNB_init_security(Mod_id, j);
    }
  }

  eNB_rrc_inst[Mod_id].Info.Nb_ue = 0;

  eNB_rrc_inst[Mod_id].Srb0.Active = 0;

  for (j = 0; j < (NUMBER_OF_UE_MAX + 1); j++)
    {
      eNB_rrc_inst[Mod_id].Srb2[j].Active = 0;
    }


  /// System Information INIT


  LOG_I (RRC, "[eNB %d] Checking release \n", Mod_id);
#ifdef Rel10

  // This has to come from some top-level configuration
  LOG_I (RRC, "[eNB %d] Rel10 RRC detected, MBMS flag %d\n", Mod_id, eNB_rrc_inst[Mod_id].MBMS_flag);

#else
  LOG_I (RRC, "[eNB %d] Rel8 RRC\n", Mod_id);
#endif
#ifdef CBA
  for (j = 0; j < NUM_MAX_CBA_GROUP; j++)
    eNB_rrc_inst[Mod_id].cba_rnti[j] = CBA_OFFSET + j;

  if (eNB_rrc_inst[Mod_id].num_active_cba_groups > NUM_MAX_CBA_GROUP)
    eNB_rrc_inst[Mod_id].num_active_cba_groups = NUM_MAX_CBA_GROUP;

  LOG_D (RRC, "[eNB %d] Initialization of 4 cba_RNTI values (%x %x %x %x) num active groups %d\n",
         Mod_id, eNB_rrc_inst[Mod_id].cba_rnti[0],
         eNB_rrc_inst[Mod_id].cba_rnti[1], eNB_rrc_inst[Mod_id].cba_rnti[2],
         eNB_rrc_inst[Mod_id].cba_rnti[3],
         eNB_rrc_inst[Mod_id].num_active_cba_groups);
#endif

  init_SI (Mod_id
#if defined(ENABLE_ITTI)
           , configuration
#endif
           );

#ifdef Rel10
  switch (eNB_rrc_inst[Mod_id].MBMS_flag) {
  case 1:
  case 2:
  case 3:
    LOG_I(RRC,"[eNB %d] Configuring 1 MBSFN sync area\n", Mod_id);
    eNB_rrc_inst[Mod_id].num_mbsfn_sync_area=1;
    break;
  case 4:
    LOG_I(RRC,"[eNB %d] Configuring 2 MBSFN sync area\n", Mod_id);
    eNB_rrc_inst[Mod_id].num_mbsfn_sync_area=2;
    break;
  default:
    eNB_rrc_inst[Mod_id].num_mbsfn_sync_area=0;
    break;
  }
  // if we are here the eNB_rrc_inst[Mod_id].MBMS_flag > 0,
  /// MCCH INIT
  if (eNB_rrc_inst[Mod_id].MBMS_flag > 0 ) {
    init_MCCH (Mod_id);
    /// MTCH data bearer init
    init_MBMS (Mod_id, 0);
  }

#endif

#ifdef NO_RRM                   //init ch SRB0, SRB1 & BDTCH
  openair_rrc_on (Mod_id, 1);
#else
  eNB_rrc_inst[Mod_id].Last_scan_req = 0;
  send_msg (&S_rrc,
            msg_rrc_phy_synch_to_MR_ind (Mod_id,
                                         eNB_rrc_inst[Mod_id].Mac_id));
#endif

  return 0;

}

/*------------------------------------------------------------------------------*/
int rrc_eNB_decode_ccch (u8 Mod_id, u32 frame, SRB_INFO * Srb_info)
{
  /*------------------------------------------------------------------------------*/

  u16 Idx, UE_index;

  asn_dec_rval_t dec_rval;
  //UL_CCCH_Message_t ulccchmsg;
  UL_CCCH_Message_t *ul_ccch_msg = NULL;        //&ulccchmsg;
  RRCConnectionRequest_r8_IEs_t *rrcConnectionRequest;
  int i, rval;


  //memset(ul_ccch_msg,0,sizeof(UL_CCCH_Message_t));

  LOG_T (RRC, "[eNB %d] Frame %d: Decoding UL CCCH %x.%x.%x.%x.%x.%x (%p)\n",
         Mod_id, frame, ((uint8_t *) Srb_info->Rx_buffer.Payload)[0],
         ((uint8_t *) Srb_info->Rx_buffer.Payload)[1],
         ((uint8_t *) Srb_info->Rx_buffer.Payload)[2],
         ((uint8_t *) Srb_info->Rx_buffer.Payload)[3],
         ((uint8_t *) Srb_info->Rx_buffer.Payload)[4],
         ((uint8_t *) Srb_info->Rx_buffer.Payload)[5],
         (uint8_t *) Srb_info->Rx_buffer.Payload);
  dec_rval =
    uper_decode (NULL, &asn_DEF_UL_CCCH_Message, (void **) &ul_ccch_msg,
                 (uint8_t *) Srb_info->Rx_buffer.Payload, 100, 0, 0);

#if defined(ENABLE_ITTI)
# if defined(DISABLE_ITTI_XER_PRINT)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_UL_CCCH_MESSAGE);
    memcpy (&message_p->ittiMsg, (void *) ul_ccch_msg, sizeof(RrcUlCcchMessage));

    itti_send_msg_to_task (TASK_UNKNOWN, Mod_id, message_p);
  }
# else
  {
    char        message_string[10000];
    size_t      message_string_size;

    if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_CCCH_Message, (void *)ul_ccch_msg)) > 0)
    {
      MessageDef *message_p;

      message_p = itti_alloc_new_message_sized (TASK_RRC_ENB, GENERIC_LOG, message_string_size);
      memcpy(&message_p->ittiMsg.generic_log, message_string, message_string_size);

      itti_send_msg_to_task(TASK_UNKNOWN, Mod_id, message_p);
    }
  }
# endif
#endif

for (i = 0; i < 8; i++)
    LOG_T (RRC, "%x.", ((u8 *) & ul_ccch_msg)[i]);
  if (dec_rval.consumed == 0)
    {
      LOG_E (RRC, "[eNB %d] FATAL Error in receiving CCCH\n", Mod_id);
      return -1;
    }
  if (ul_ccch_msg->message.present == UL_CCCH_MessageType_PR_c1)
    {

      switch (ul_ccch_msg->message.choice.c1.present)
        {

        case UL_CCCH_MessageType__c1_PR_NOTHING:
          LOG_I (RRC, "[eNB %d] Frame %d : Received PR_NOTHING on UL-CCCH-Message\n",
                 Mod_id, frame);
          break;

        case UL_CCCH_MessageType__c1_PR_rrcConnectionReestablishmentRequest:
          LOG_D (RRC, "[MSC_MSG][FRAME %05d][MAC_eNB][MOD %02d][][--- MAC_DATA_IND (rrcConnectionReestablishmentRequest on SRB0) -->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, Mod_id);
          LOG_I (RRC, "[eNB %d] Frame %d : RRCConnectionReestablishmentRequest not supported yet\n",
                 Mod_id, frame);
          break;

        case UL_CCCH_MessageType__c1_PR_rrcConnectionRequest:
          LOG_D (RRC, "[MSC_MSG][FRAME %05d][MAC_eNB][MOD %02d][][--- MAC_DATA_IND  (rrcConnectionRequest on SRB0) -->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, Mod_id);

          rrcConnectionRequest = &ul_ccch_msg->message.choice.c1.choice.rrcConnectionRequest.criticalExtensions.choice.rrcConnectionRequest_r8;
          {
              uint64_t random_value = 0;

              memcpy(((u8*)&random_value) + 3, rrcConnectionRequest->ue_Identity.choice.randomValue.buf, rrcConnectionRequest->ue_Identity.choice.randomValue.size);
              UE_index = rrc_eNB_get_next_free_UE_index (Mod_id, random_value);
          }

          if (UE_index != UE_INDEX_INVALID)
            {
#if defined(ENABLE_ITTI)
              /* Check s-TMSI presence in message */
              eNB_rrc_inst[Mod_id].Info.UE[UE_index].Initialue_identity_s_TMSI.presence =
                      (rrcConnectionRequest->ue_Identity.present == InitialUE_Identity_PR_s_TMSI);
              if (eNB_rrc_inst[Mod_id].Info.UE[UE_index].Initialue_identity_s_TMSI.presence) {
                /* Save s-TMSI */
                S_TMSI_t s_TMSI = rrcConnectionRequest->ue_Identity.choice.s_TMSI;

                eNB_rrc_inst[Mod_id].Info.UE[UE_index].Initialue_identity_s_TMSI.mme_code = BIT_STRING_to_uint8 (&s_TMSI.mmec);
                eNB_rrc_inst[Mod_id].Info.UE[UE_index].Initialue_identity_s_TMSI.m_tmsi = BIT_STRING_to_uint32 (&s_TMSI.m_TMSI);
              }
              eNB_rrc_inst[Mod_id].Info.UE[UE_index].establishment_cause = rrcConnectionRequest->establishmentCause;
#endif

              //      memcpy(&Rrc_xface->UE_id[Mod_id][UE_index],(u8 *)rrcConnectionRequest->ue_Identity.choice.randomValue.buf,5);
              memcpy (&eNB_rrc_inst[Mod_id].Info.UE_list[UE_index],
                      (u8 *) rrcConnectionRequest->ue_Identity.choice.randomValue.buf, 5);

              LOG_I (RRC, "[eNB %d] Frame %d : Accept new connection from UE %d (0x%" PRIx64 ")\n",
                     Mod_id, frame, UE_index,
                     eNB_rrc_inst[Mod_id].Info.UE_list[UE_index]);

              //CONFIG SRB2  (DCCHs, ONE per User)  //meas && lchan Cfg
              //eNB_rrc_inst[Mod_id].Info.Dtch_bd_config[UE_index].Status=NEED_RADIO_CONFIG;
              //eNB_rrc_inst[Mod_id].Info.Dtch_bd_config[UE_index].Next_eNBeck_frame=Rrc_xface->Frame_index+1;
              eNB_rrc_inst[Mod_id].Info.Nb_ue++;

#ifndef NO_RRM
              send_msg (&S_rrc, msg_rrc_MR_attach_ind (Mod_id, Mac_id));
#else

              Idx = (UE_index * NB_RB_MAX) + DCCH;
              // SRB1
              eNB_rrc_inst[Mod_id].Srb1[UE_index].Active = 1;
              eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.Srb_id = Idx;
              memcpy (&eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.
                      Lchan_desc[0], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
              memcpy (&eNB_rrc_inst[Mod_id].Srb1[UE_index].Srb_info.
                      Lchan_desc[1], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);

              // SRB2
              eNB_rrc_inst[Mod_id].Srb2[UE_index].Active = 1;
              eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.Srb_id = Idx;
              memcpy (&eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.
                      Lchan_desc[0], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
              memcpy (&eNB_rrc_inst[Mod_id].Srb2[UE_index].Srb_info.
                      Lchan_desc[1], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);

              rrc_eNB_generate_RRCConnectionSetup (Mod_id, frame, UE_index);
              //LOG_D(RRC, "[MSC_NBOX][FRAME %05d][RRC_eNB][MOD %02d][][Tx RRCConnectionSetup][RRC_eNB][MOD %02d][]\n",
              //      frame, Mod_id, Mod_id);

              //LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",Mod_id,rlc[Mod_id].m_rlc_am_array[0].allocation);
              //LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",Mod_id,rlc[Mod_id].m_rlc_am_array[1].allocation);
              LOG_I (RRC,"[eNB %d] CALLING RLC CONFIG SRB1 (rbid %d) for UE %d\n",Mod_id, Idx, UE_index);

              //      rrc_pdcp_config_req (Mod_id, frame, 1, ACTION_ADD, idx, UNDEF_SECURITY_MODE);

              //      rrc_rlc_config_req(Mod_id,frame,1,ACTION_ADD,Idx,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);

              rrc_pdcp_config_asn1_req (Mod_id, UE_index, frame, 1,
                                        eNB_rrc_inst[Mod_id].
                                        SRB_configList[UE_index],
                                        (DRB_ToAddModList_t *) NULL,
                                        (DRB_ToReleaseList_t *) NULL,
                                        0xff,
                                        NULL,
                                        NULL,
                                        NULL
#ifdef Rel10
                                        , (PMCH_InfoList_r9_t *) NULL
#endif
                );

              rrc_rlc_config_asn1_req (Mod_id, frame, 1, UE_index,
                                       eNB_rrc_inst[Mod_id].
                                       SRB_configList[UE_index],
                                       (DRB_ToAddModList_t *) NULL,
                                       (DRB_ToReleaseList_t *) NULL
#ifdef Rel10
                                       , (PMCH_InfoList_r9_t *) NULL
#endif
                );
              //LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",Mod_id,rlc[Mod_id].m_rlc_am_array[0].allocation);
              //LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",Mod_id,rlc[Mod_id].m_rlc_am_array[1].allocation);

              /*

                 LOG_D(RRC,"[eNB %d] CALLING RLC CONFIG SRB2 (rbid %d) for UE %d\n",
                 Mod_id,Idx+1,UE_index);
                 Mac_rlc_xface->rrc_rlc_config_req(Mod_id,ACTION_ADD,Idx+1,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
                 LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",Mod_id,rlc[Mod_id].m_rlc_am_array[0].allocation);
                 LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",rlc[Mod_id].m_rlc_am_array[1].allocation);
               */
#endif //NO_RRM
            }
          else
            {
              LOG_E (RRC, "can't add UE, max user count reached!\n");
            }
          break;

        default:
          LOG_E (RRC, "[eNB %d] Frame %d : Unknown message\n", Mod_id, frame);
          rval = -1;
          break;
        }
      rval = 0;
    }
  else
    {
      LOG_E (RRC, "[eNB %d] Frame %d : Unknown error \n", Mod_id, frame);
      rval = -1;
    }
  return rval;
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_decode_dcch (u8 Mod_id, u32 frame, u8 Srb_id, u8 UE_index,
                     u8 * Rx_sdu, u8 sdu_size)
{
  /*------------------------------------------------------------------------------*/

  asn_dec_rval_t dec_rval;
  //UL_DCCH_Message_t uldcchmsg;
  UL_DCCH_Message_t *ul_dcch_msg = NULL;        //&uldcchmsg;
  UE_EUTRA_Capability_t *UE_EUTRA_Capability = NULL;

  if (Srb_id != 1)
    {
      LOG_E (RRC,
             "[eNB %d] Frame %d: Received message on SRB%d, should not have ...\n",
             Mod_id, frame, Srb_id);
    }

  //memset(ul_dcch_msg,0,sizeof(UL_DCCH_Message_t));

  LOG_D (RRC, "[eNB %d] Frame %d: Decoding UL-DCCH Message\n", Mod_id, frame);
  dec_rval = uper_decode (NULL,
                          &asn_DEF_UL_DCCH_Message,
                          (void **) &ul_dcch_msg, Rx_sdu, sdu_size, 0, 0);

#if defined(ENABLE_ITTI)
# if defined(DISABLE_ITTI_XER_PRINT)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_UL_DCCH_MESSAGE);
    memcpy (&message_p->ittiMsg, (void *) ul_dcch_msg, sizeof(RrcUlDcchMessage));

    itti_send_msg_to_task (TASK_UNKNOWN, Mod_id, message_p);
  }
# else
  {
    char        message_string[10000];
    size_t      message_string_size;

    if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_DCCH_Message, (void *)ul_dcch_msg)) >= 0)
    {
      MessageDef *message_p;

      message_p = itti_alloc_new_message_sized (TASK_RRC_ENB, GENERIC_LOG, message_string_size);
      memcpy(&message_p->ittiMsg.generic_log, message_string, message_string_size);

      itti_send_msg_to_task(TASK_UNKNOWN, Mod_id, message_p);
    }
  }
# endif
#endif

  {
    int i;

    for (i = 0; i < sdu_size; i++)
      LOG_T (RRC, "%x.", Rx_sdu[i]);
    LOG_T (RRC, "\n");
  }

  if ((dec_rval.code != RC_OK) && (dec_rval.consumed == 0))
    {
      LOG_E (RRC, "[UE %d] Frame %d : Failed to decode UL-DCCH (%d bytes)\n",
             Mod_id, frame, dec_rval.consumed);
      return -1;
    }

  if (ul_dcch_msg->message.present == UL_DCCH_MessageType_PR_c1)
    {

      switch (ul_dcch_msg->message.choice.c1.present)
        {
        case UL_DCCH_MessageType__c1_PR_NOTHING:        /* No components present */
          break;

        case UL_DCCH_MessageType__c1_PR_csfbParametersRequestCDMA2000:
          break;

        case UL_DCCH_MessageType__c1_PR_measurementReport:
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND "
                 "%d bytes (measurementReport) --->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, DCCH, sdu_size, Mod_id);
          rrc_eNB_process_MeasurementReport (Mod_id, frame, UE_index,
                                             &ul_dcch_msg->message.choice.c1.
                                             choice.measurementReport.
                                             criticalExtensions.choice.c1.
                                             choice.measurementReport_r8.
                                             measResults);
          break;

        case UL_DCCH_MessageType__c1_PR_rrcConnectionReconfigurationComplete:
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(RRCConnectionReconfigurationComplete) --->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, DCCH, sdu_size, Mod_id);
          if (ul_dcch_msg->message.choice.c1.choice.
              rrcConnectionReconfigurationComplete.criticalExtensions.
              present ==
              RRCConnectionReconfigurationComplete__criticalExtensions_PR_rrcConnectionReconfigurationComplete_r8)
          {
            rrc_eNB_process_RRCConnectionReconfigurationComplete (Mod_id,
                                                                  frame,
                                                                  UE_index,
                                                                  &ul_dcch_msg->
                                                                  message.
                                                                  choice.c1.
                                                                  choice.
                                                                  rrcConnectionReconfigurationComplete.
                                                                  criticalExtensions.
                                                                  choice.
                                                                  rrcConnectionReconfigurationComplete_r8);
            eNB_rrc_inst[Mod_id].Info.UE[UE_index].Status = RRC_RECONFIGURED;
            LOG_I (RRC, "[eNB %d] UE %d State = RRC_RECONFIGURED \n",
                   Mod_id, UE_index);
          }

#if defined(ENABLE_USE_MME)
# if defined(ENABLE_ITTI)
          if (EPC_MODE_ENABLED == 1)
          {
            rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP (Mod_id, UE_index);
          }
# endif
#endif
          break;

        case UL_DCCH_MessageType__c1_PR_rrcConnectionReestablishmentComplete:
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(rrcConnectionReestablishmentComplete) --->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, DCCH, sdu_size, Mod_id);
          break;

        case UL_DCCH_MessageType__c1_PR_rrcConnectionSetupComplete:
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(RRCConnectionSetupComplete) --->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, DCCH, sdu_size, Mod_id);

          if (ul_dcch_msg->message.choice.c1.choice.
              rrcConnectionSetupComplete.criticalExtensions.present ==
              RRCConnectionSetupComplete__criticalExtensions_PR_c1)
            {
              if (ul_dcch_msg->message.choice.c1.choice.
                  rrcConnectionSetupComplete.criticalExtensions.choice.c1.
                  present ==
                  RRCConnectionSetupComplete__criticalExtensions__c1_PR_rrcConnectionSetupComplete_r8)
                {
                  rrc_eNB_process_RRCConnectionSetupComplete (Mod_id, frame,
                                                              UE_index,
                                                              &ul_dcch_msg->
                                                              message.choice.
                                                              c1.choice.
                                                              rrcConnectionSetupComplete.
                                                              criticalExtensions.
                                                              choice.c1.
                                                              choice.
                                                              rrcConnectionSetupComplete_r8);
                  eNB_rrc_inst[Mod_id].Info.UE[UE_index].Status = RRC_CONNECTED;
                  LOG_I (RRC, "[eNB %d] UE %d State = RRC_CONNECTED \n",
                         Mod_id, UE_index);
                  LOG_D (RRC,
                         "[MSC_NBOX][FRAME %05d][RRC_eNB][MOD %02d][][Rx RRCConnectionSetupComplete\n"
                         "Now CONNECTED with UE %d][RRC_eNB][MOD %02d][]\n",
                         frame, Mod_id, UE_index, Mod_id);
                }
            }
          break;

        case UL_DCCH_MessageType__c1_PR_securityModeComplete:
          LOG_I (RRC,
                 "[eNB %d] Frame %d received securityModeComplete on UL-DCCH %d from UE %d\n",
                 Mod_id, frame, DCCH, UE_index);
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(securityModeComplete) --->][RRC_eNB][MOD %02d][]\n", frame,
                 Mod_id, DCCH, sdu_size, Mod_id);
#ifdef XER_PRINT
          xer_fprint (stdout, &asn_DEF_UL_DCCH_Message, (void *) ul_dcch_msg);
#endif
          // confirm with PDCP about the security mode for DCCH
          //rrc_pdcp_config_req (Mod_id, frame, 1,ACTION_SET_SECURITY_MODE, (UE_index * NB_RB_MAX) + DCCH, 0x77);
          // continue the procedure
          rrc_eNB_generate_UECapabilityEnquiry (Mod_id, frame, UE_index);
          break;

        case UL_DCCH_MessageType__c1_PR_securityModeFailure:
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(securityModeFailure) --->][RRC_eNB][MOD %02d][]\n", frame,
                 Mod_id, DCCH, sdu_size, Mod_id);
#ifdef XER_PRINT
          xer_fprint (stdout, &asn_DEF_UL_DCCH_Message, (void *) ul_dcch_msg);
#endif
          // cancel the security mode in PDCP

          // followup with the remaining procedure
          rrc_eNB_generate_UECapabilityEnquiry (Mod_id, frame, UE_index);
          break;

        case UL_DCCH_MessageType__c1_PR_ueCapabilityInformation:
          LOG_I (RRC,
                 "[eNB %d] Frame %d received ueCapabilityInformation on UL-DCCH %d from UE %d\n",
                 Mod_id, frame, DCCH, UE_index);
          LOG_D (RRC,
                 "[MSC_MSG][FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                 "(UECapabilityInformation) --->][RRC_eNB][MOD %02d][]\n",
                 frame, Mod_id, DCCH, sdu_size, Mod_id);
#ifdef XER_PRINT
          xer_fprint (stdout, &asn_DEF_UL_DCCH_Message, (void *) ul_dcch_msg);
#endif
          dec_rval = uper_decode (NULL,
                                  &asn_DEF_UE_EUTRA_Capability,
                                  (void **) &UE_EUTRA_Capability,
                                  ul_dcch_msg->message.choice.c1.choice.
                                  ueCapabilityInformation.criticalExtensions.
                                  choice.c1.choice.ueCapabilityInformation_r8.
                                  ue_CapabilityRAT_ContainerList.list.
                                  array[0]->ueCapabilityRAT_Container.buf,
                                  ul_dcch_msg->message.choice.c1.choice.
                                  ueCapabilityInformation.criticalExtensions.
                                  choice.c1.choice.ueCapabilityInformation_r8.
                                  ue_CapabilityRAT_ContainerList.list.
                                  array[0]->ueCapabilityRAT_Container.size, 0,
                                  0);
#ifdef XER_PRINT
          xer_fprint (stdout, &asn_DEF_UE_EUTRA_Capability,
                      (void *) UE_EUTRA_Capability);
#endif

          rrc_eNB_generate_defaultRRCConnectionReconfiguration (Mod_id, frame,
                                                                UE_index,
                                                                eNB_rrc_inst[Mod_id].HO_flag);
          break;

        case UL_DCCH_MessageType__c1_PR_ulHandoverPreparationTransfer:
          break;

        case UL_DCCH_MessageType__c1_PR_ulInformationTransfer:
#if defined(ENABLE_USE_MME)
          if (EPC_MODE_ENABLED == 1)
          {
            rrc_eNB_send_S1AP_UPLINK_NAS(Mod_id, UE_index, ul_dcch_msg);
          }
#endif
          break;

        case UL_DCCH_MessageType__c1_PR_counterCheckResponse:
          break;

#ifdef Rel10
        case UL_DCCH_MessageType__c1_PR_ueInformationResponse_r9:
          break;

        case UL_DCCH_MessageType__c1_PR_proximityIndication_r9:
          break;

        case UL_DCCH_MessageType__c1_PR_rnReconfigurationComplete_r10:
          break;

        case UL_DCCH_MessageType__c1_PR_mbmsCountingResponse_r10:
          break;

        case UL_DCCH_MessageType__c1_PR_interFreqRSTDMeasurementIndication_r10:
          break;
#endif

        default:
          LOG_E (RRC, "[UE %d] Frame %d : Unknown message\n", Mod_id, frame);
          return -1;
        }
      return 0;
    }
  else
    {
      LOG_E (RRC, "[UE %d] Frame %d : Unknown error\n", Mod_id, frame);
      return -1;
    }

}

#if defined(ENABLE_ITTI)
/*------------------------------------------------------------------------------*/
void *rrc_enb_task(void *args_p) {
  MessageDef   *msg_p;
  const char   *msg_name;
  instance_t    instance;
  int           result;
  SRB_INFO     *srb_info_p;

  itti_mark_task_ready (TASK_RRC_ENB);

  while(1) {
    // Wait for a message
    itti_receive_msg (TASK_RRC_ENB, &msg_p);

    msg_name = ITTI_MSG_NAME (msg_p);
    instance = ITTI_MSG_INSTANCE (msg_p);

    switch (ITTI_MSG_ID(msg_p)) {
      case TERMINATE_MESSAGE:
        itti_exit_task ();
        break;

      case MESSAGE_TEST:
        LOG_I(RRC, "[eNB %d] Received %s\n", instance, msg_name);
        break;

      /* Messages from MAC */
      case RRC_MAC_CCCH_DATA_IND:
        LOG_I(RRC, "[eNB %d] Received %s: frame %d,\n", instance, msg_name,
              RRC_MAC_CCCH_DATA_IND (msg_p).frame);

        srb_info_p = &eNB_rrc_inst[instance].Srb0;

        memcpy (srb_info_p->Rx_buffer.Payload, RRC_MAC_CCCH_DATA_IND (msg_p).sdu,
                RRC_MAC_CCCH_DATA_IND (msg_p).sdu_size);
        srb_info_p->Rx_buffer.payload_size = RRC_MAC_CCCH_DATA_IND (msg_p).sdu_size;
        rrc_eNB_decode_ccch (instance, RRC_MAC_CCCH_DATA_IND (msg_p).frame, srb_info_p);
        break;

      /* Messages from PDCP */
      case RRC_DCCH_DATA_IND:
        LOG_I(RRC, "[eNB %d][UE %d] Received %s: frame %d, DCCH %d\n", instance, RRC_DCCH_DATA_IND (msg_p).ue_index, msg_name,
              RRC_DCCH_DATA_IND (msg_p).frame, RRC_DCCH_DATA_IND (msg_p).dcch_index);

        rrc_eNB_decode_dcch (instance, RRC_DCCH_DATA_IND (msg_p).frame, RRC_DCCH_DATA_IND (msg_p).dcch_index,
                             RRC_DCCH_DATA_IND (msg_p).ue_index, RRC_DCCH_DATA_IND (msg_p).sdu_p,
                             RRC_DCCH_DATA_IND (msg_p).sdu_size);

        // Message buffer has been processed, free it now.
        result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), RRC_DCCH_DATA_IND (msg_p).sdu_p);
        AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
        break;

#if defined(ENABLE_USE_MME)
      /* Messages from S1AP */
      case S1AP_DOWNLINK_NAS:
        rrc_eNB_process_S1AP_DOWNLINK_NAS(msg_p, msg_name, instance, &rrc_eNB_mui);
        break;

      case S1AP_INITIAL_CONTEXT_SETUP_REQ:
        rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(msg_p, msg_name, instance);
        break;

      case S1AP_UE_CTXT_MODIFICATION_REQ:
        rrc_eNB_process_S1AP_UE_CTXT_MODIFICATION_REQ(msg_p, msg_name, instance);
        break;

      case S1AP_PAGING_IND:
        LOG_E(RRC, "[eNB %d] Received not yet implemented message %s\n", instance, msg_name);
        break;

      case S1AP_UE_CONTEXT_RELEASE_REQ:
        rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_REQ(msg_p, msg_name, instance);
        break;
#endif

      /* Messages from eNB app */
      case RRC_CONFIGURATION_REQ:
          LOG_I(RRC, "[eNB %d] Received %s\n", instance, msg_name);
          openair_rrc_lite_eNB_configuration(instance, &RRC_CONFIGURATION_REQ (msg_p));
          break;

      default:
        LOG_E(RRC, "[eNB %d] Received unexpected message %s\n", instance, msg_name);
        break;
    }

    result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
    msg_p = NULL;
  }
}
#endif

#ifndef USER_MODE
EXPORT_SYMBOL (Rlc_info_am_config);
#endif
