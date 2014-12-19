/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
    included in this distribution in the file called "COPYING". If not,
    see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

*******************************************************************************/

/*! \file rrc_eNB.c
 * \brief rrc procedures for eNB
 * \author Navid Nikaein and  Raymond Knopp 
 * \date 2011 - 2014
 * \version 1.0
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr and raymond.knopp@eurecom.fr
 */
#define RRC_ENB
#define RRC_ENB_C

#include "defs.h"
#include "extern.h"
#include "assertions.h"
#include "asn1_conversions.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
#include "LAYER2/MAC/proto.h"
#include "UTIL/LOG/log.h"
#include "COMMON/mac_rrc_primitives.h"
#include "RRC/LITE/MESSAGES/asn1_msg.h"
#include "RRCConnectionRequest.h"
#include "RRCConnectionReestablishmentRequest.h"
//#include "ReestablishmentCause.h"
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
#   include "RRC/NAS/nas_config.h"
#   include "RRC/NAS/rb_config.h"
#   include "OCG.h"
#   include "OCG_extern.h"
#endif

#if defined(ENABLE_SECURITY)
#   include "UTIL/OSA/osa_defs.h"
#endif

#if defined(ENABLE_USE_MME)
#   include "rrc_eNB_S1AP.h"
#   include "rrc_eNB_GTPV1U.h"
#   if defined(ENABLE_ITTI)
#   else
#      include "../../S1AP/s1ap_eNB.h"
#   endif
#endif

#include "pdcp.h"

#if defined(ENABLE_ITTI)
#   include "intertask_interface.h"
#endif

#ifdef ENABLE_RAL
#   include "rrc_eNB_ral.h"
#endif

//#define XER_PRINT

#ifdef PHY_EMUL
extern EMULATION_VARS              *Emul_vars;
#endif
extern eNB_MAC_INST                *eNB_mac_inst;
extern UE_MAC_INST                 *UE_mac_inst;
#ifdef BIGPHYSAREA
extern void                        *bigphys_malloc(
    int);
#endif

extern uint16_t                     two_tier_hexagonal_cellIds[7];
extern inline unsigned int          taus(
    void);

/* TS 36.331: RRC-TransactionIdentifier ::= INTEGER (0..3) */
static const uint8_t                RRC_TRANSACTION_IDENTIFIER_NUMBER = 4;

mui_t                               rrc_eNB_mui = 0;

/*------------------------------------------------------------------------------*/
static void init_SI(
    module_id_t enb_mod_idP
#if defined(ENABLE_ITTI)
    ,
    RrcConfigurationReq * configuration
#endif
    ) {
    uint8_t                             SIwindowsize = 1;
    uint16_t                            SIperiod = 8;
#ifdef Rel10
    int                                 i;
#endif
    /*
       uint32_t mib=0;
       int i;
       int N_RB_DL,phich_resource;

       do_MIB(enb_mod_idP, mac_xface->lte_frame_parms,0x321,&mib);

       for (i=0;i<1024;i+=4)
       do_MIB(enb_mod_idP, mac_xface->lte_frame_parms,i,&mib);

       N_RB_DL=6;
       while (N_RB_DL != 0) {
       phich_resource = 1;
       while (phich_resource != 0) {
       for (i=0;i<2;i++) {
       mac_xface->lte_frame_parms->N_RB_DL = N_RB_DL;
       mac_xface->lte_frame_parms->phich_config_common.phich_duration=i;
       mac_xface->lte_frame_parms->phich_config_common.phich_resource = phich_resource;
       do_MIB(enb_mod_idP, mac_xface->lte_frame_parms,0,&mib);
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

    eNB_rrc_inst[enb_mod_idP].sizeof_SIB1 = 0;
    eNB_rrc_inst[enb_mod_idP].sizeof_SIB23 = 0;

    eNB_rrc_inst[enb_mod_idP].SIB1 = (uint8_t *) malloc16(32);

    /*
       printf ("before SIB1 init : Nid_cell %d\n", mac_xface->lte_frame_parms->Nid_cell);
       printf ("before SIB1 init : frame_type %d,tdd_config %d\n",
       mac_xface->lte_frame_parms->frame_type,
       mac_xface->lte_frame_parms->tdd_config);
     */

    if (eNB_rrc_inst[enb_mod_idP].SIB1)
        eNB_rrc_inst[enb_mod_idP].sizeof_SIB1 = do_SIB1(enb_mod_idP, mac_xface->lte_frame_parms, (uint8_t *)
                                                   eNB_rrc_inst[enb_mod_idP].SIB1,
                                                   &eNB_rrc_inst[enb_mod_idP].siblock1, &eNB_rrc_inst[enb_mod_idP].sib1
#if defined(ENABLE_ITTI)
                                                   , configuration
#endif
            );
    else {
        LOG_E(RRC, "[eNB %d] init_SI: FATAL, no memory for SIB1 allocated\n", enb_mod_idP);
        mac_xface->macphy_exit("[RRC][init_SI] FATAL, no memory for SIB1 allocated");
    }
    /*
       printf ("after SIB1 init : Nid_cell %d\n", mac_xface->lte_frame_parms->Nid_cell);
       printf ("after SIB1 init : frame_type %d,tdd_config %d\n",
       mac_xface->lte_frame_parms->frame_type,
       mac_xface->lte_frame_parms->tdd_config);
     */
    if (eNB_rrc_inst[enb_mod_idP].sizeof_SIB1 == 255)
        mac_xface->macphy_exit("[RRC][init_SI] FATAL, eNB_rrc_inst[enb_mod_idP].sizeof_SIB1 == 255");

    eNB_rrc_inst[enb_mod_idP].SIB23 = (uint8_t *) malloc16(64);
    if (eNB_rrc_inst[enb_mod_idP].SIB23) {
        eNB_rrc_inst[enb_mod_idP].sizeof_SIB23 = do_SIB23(enb_mod_idP,
                                                     mac_xface->lte_frame_parms,
                                                     eNB_rrc_inst[enb_mod_idP].SIB23,
                                                     &eNB_rrc_inst[enb_mod_idP].systemInformation,
                                                     &eNB_rrc_inst[enb_mod_idP].sib2, &eNB_rrc_inst[enb_mod_idP].sib3
#ifdef Rel10
                                                     , &eNB_rrc_inst[enb_mod_idP].sib13, eNB_rrc_inst[enb_mod_idP].MBMS_flag
#endif
#if defined(ENABLE_ITTI)
                                                     , configuration
#endif
            );
        /*
           eNB_rrc_inst[enb_mod_idP].sizeof_SIB23 = do_SIB2_AT4(enb_mod_idP,
           eNB_rrc_inst[enb_mod_idP].SIB23,
           &eNB_rrc_inst[enb_mod_idP].systemInformation,
           &eNB_rrc_inst[enb_mod_idP].sib2,
           #if defined(ENABLE_ITTI)
           , configuration
           #endif
           );
         */
        if (eNB_rrc_inst[enb_mod_idP].sizeof_SIB23 == 255)
            mac_xface->macphy_exit("[RRC][init_SI] FATAL, eNB_rrc_inst[enb_mod_idP].sizeof_SIB23 == 255");

        LOG_T(RRC, "[eNB %d] SIB2/3 Contents (partial)\n", enb_mod_idP);
        LOG_T(RRC, "[eNB %d] pusch_config_common.n_SB = %ld\n", enb_mod_idP,
              eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.n_SB);
        LOG_T(RRC, "[eNB %d] pusch_config_common.hoppingMode = %ld\n", enb_mod_idP,
              eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.hoppingMode);
        LOG_T(RRC, "[eNB %d] pusch_config_common.pusch_HoppingOffset = %ld\n",
              enb_mod_idP,
              eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.
              pusch_ConfigBasic.pusch_HoppingOffset);
        LOG_T(RRC, "[eNB %d] pusch_config_common.enable64QAM = %d\n", enb_mod_idP,
              (int)eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.
              pusch_ConfigBasic.enable64QAM);
        LOG_T(RRC, "[eNB %d] pusch_config_common.groupHoppingEnabled = %d\n", enb_mod_idP,
              (int)eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.
              ul_ReferenceSignalsPUSCH.groupHoppingEnabled);
        LOG_T(RRC, "[eNB %d] pusch_config_common.groupAssignmentPUSCH = %ld\n", enb_mod_idP,
              eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.
              groupAssignmentPUSCH);
        LOG_T(RRC, "[eNB %d] pusch_config_common.sequenceHoppingEnabled = %d\n", enb_mod_idP,
              (int)eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.
              sequenceHoppingEnabled);
        LOG_T(RRC, "[eNB %d] pusch_config_common.cyclicShift  = %ld\n", enb_mod_idP,
              eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon.pusch_ConfigCommon.
              ul_ReferenceSignalsPUSCH.cyclicShift);

#ifdef Rel10
        if (eNB_rrc_inst[enb_mod_idP].MBMS_flag > 0) {
            for (i = 0; i < eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.count; i++) {
                // SIB 2
                //   LOG_D(RRC, "[eNB %d] mbsfn_SubframeConfigList.list.count = %ld\n", enb_mod_idP, eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.count);
                LOG_D(RRC, "[eNB %d] SIB13 contents for MBSFN subframe allocation %d/%d(partial)\n",
                      enb_mod_idP, i, eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.count);
                LOG_D(RRC, "[eNB %d] mbsfn_Subframe_pattern is  = %x\n", enb_mod_idP,
                      eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.array[i]->subframeAllocation.
                      choice.oneFrame.buf[0] >> 0);
                LOG_D(RRC, "[eNB %d] radioframe_allocation_period  = %ld (just index number, not the real value)\n", enb_mod_idP, eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.array[i]->radioframeAllocationPeriod);   // need to display the real value, using array of char (like in dumping SIB2)
                LOG_D(RRC, "[eNB %d] radioframe_allocation_offset  = %ld\n",
                      enb_mod_idP,
                      eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList->list.array[i]->radioframeAllocationOffset);
            }
            //   SIB13
            for (i = 0; i < eNB_rrc_inst[enb_mod_idP].sib13->mbsfn_AreaInfoList_r9.list.count; i++) {
                LOG_D(RRC, "[eNB %d] SIB13 contents for MBSFN sync area %d/2 (partial)\n",
                      enb_mod_idP, i, eNB_rrc_inst[enb_mod_idP].sib13->mbsfn_AreaInfoList_r9.list.count);
                LOG_D(RRC, "[eNB %d] MCCH Repetition Period: %d (just index number, not real value)\n",
                      enb_mod_idP,
                      eNB_rrc_inst[enb_mod_idP].sib13->mbsfn_AreaInfoList_r9.list.array[i]->
                      mcch_Config_r9.mcch_RepetitionPeriod_r9);
                LOG_D(RRC, "[eNB %d] MCCH Offset: %d\n", enb_mod_idP,
                      eNB_rrc_inst[enb_mod_idP].sib13->mbsfn_AreaInfoList_r9.list.array[i]->mcch_Config_r9.mcch_Offset_r9);
            }
        }
#endif

        LOG_D(RRC,
              "[FRAME unknown][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (SIB1.tdd & SIB2 params) --->][MAC_UE][MOD %02d][]\n",
              enb_mod_idP, enb_mod_idP);

        rrc_mac_config_req(enb_mod_idP, ENB_FLAG_YES, 0, 0,
                           (RadioResourceConfigCommonSIB_t *) &
                           eNB_rrc_inst[enb_mod_idP].sib2->radioResourceConfigCommon,
                           (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
			   (SCellToAddMod_r10_t *)NULL,
			   //(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                           (MeasObjectToAddMod_t **) NULL,
                           (MAC_MainConfig_t *) NULL, 0,
                           (struct LogicalChannelConfig *)NULL,
                           (MeasGapConfig_t *) NULL,
                           eNB_rrc_inst[enb_mod_idP].sib1->tdd_Config,
                           NULL,
                           &SIwindowsize, &SIperiod,
                           eNB_rrc_inst[enb_mod_idP].sib2->freqInfo.ul_CarrierFreq,
                           eNB_rrc_inst[enb_mod_idP].sib2->freqInfo.ul_Bandwidth,
                           &eNB_rrc_inst[enb_mod_idP].sib2->freqInfo.additionalSpectrumEmission,
                           (MBSFN_SubframeConfigList_t *) eNB_rrc_inst[enb_mod_idP].sib2->mbsfn_SubframeConfigList
#ifdef Rel10
                           ,
                           eNB_rrc_inst[enb_mod_idP].MBMS_flag,
                           (MBSFN_AreaInfoList_r9_t *) & eNB_rrc_inst[enb_mod_idP].sib13->mbsfn_AreaInfoList_r9,
                           (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                           , 0, //eNB_rrc_inst[enb_mod_idP].num_active_cba_groups,
                           0    //eNB_rrc_inst[enb_mod_idP].cba_rnti[0]
#endif
            );
    } else {
        LOG_E(RRC, "[eNB] init_SI: FATAL, no memory for SIB2/3 allocated\n");
        mac_xface->macphy_exit("[RRC][init_SI] FATAL, no memory for SIB2/3 allocated");
    }
}

#ifdef Rel10
/*------------------------------------------------------------------------------*/
static void init_MCCH(
    module_id_t enb_mod_idP) {

    int                                 sync_area = 0;
    // initialize RRC_eNB_INST MCCH entry
    eNB_rrc_inst[enb_mod_idP].MCCH_MESSAGE = malloc(eNB_rrc_inst[enb_mod_idP].num_mbsfn_sync_area * sizeof(uint32_t *));
    for (sync_area = 0; sync_area < eNB_rrc_inst[enb_mod_idP].num_mbsfn_sync_area; sync_area++) {

        eNB_rrc_inst[enb_mod_idP].sizeof_MCCH_MESSAGE[sync_area] = 0;
        eNB_rrc_inst[enb_mod_idP].MCCH_MESSAGE[sync_area] = (uint8_t *) malloc16(32);

        if (eNB_rrc_inst[enb_mod_idP].MCCH_MESSAGE[sync_area] == NULL) {
            LOG_E(RRC, "[eNB %d][MAIN] init_MCCH: FATAL, no memory for MCCH MESSAGE allocated \n", enb_mod_idP);
            mac_xface->macphy_exit("[RRC][init_MCCH] not enough memory\n");
        } else {
            eNB_rrc_inst[enb_mod_idP].sizeof_MCCH_MESSAGE[sync_area] = do_MBSFNAreaConfig(enb_mod_idP,
                                                                                     mac_xface->lte_frame_parms,
                                                                                     sync_area, (uint8_t *)
                                                                                     eNB_rrc_inst[enb_mod_idP].MCCH_MESSAGE
                                                                                     [sync_area],
                                                                                     &eNB_rrc_inst[enb_mod_idP].mcch,
                                                                                     &eNB_rrc_inst
                                                                                     [enb_mod_idP].mcch_message);

            LOG_I(RRC, "mcch message pointer %p for sync area %d \n", eNB_rrc_inst[enb_mod_idP].MCCH_MESSAGE[sync_area],
                  sync_area);
            LOG_D(RRC, "[eNB %d] MCCH_MESSAGE  contents for Sync Area %d (partial)\n", enb_mod_idP, sync_area);
            LOG_D(RRC, "[eNB %d] CommonSF_AllocPeriod_r9 %d\n", enb_mod_idP,
                  eNB_rrc_inst[enb_mod_idP].mcch_message->commonSF_AllocPeriod_r9);
            LOG_D(RRC,
                  "[eNB %d] CommonSF_Alloc_r9.list.count (number of MBSFN Subframe Pattern) %d\n",
                  enb_mod_idP, eNB_rrc_inst[enb_mod_idP].mcch_message->commonSF_Alloc_r9.list.count);
            LOG_D(RRC, "[eNB %d] MBSFN Subframe Pattern: %02x (in hex)\n",
                  enb_mod_idP,
                  eNB_rrc_inst[enb_mod_idP].mcch_message->commonSF_Alloc_r9.list.array[0]->subframeAllocation.
                  choice.oneFrame.buf[0]);

            if (eNB_rrc_inst[enb_mod_idP].sizeof_MCCH_MESSAGE[sync_area] == 255)
                mac_xface->macphy_exit("[RRC][init_MCCH] eNB_rrc_inst[enb_mod_idP].sizeof_MCCH_MESSAGE[sync_area] == 255");
            else
                eNB_rrc_inst[enb_mod_idP].MCCH_MESS[sync_area].Active = 1;
        }
    }
    //Set the eNB_rrc_inst[enb_mod_idP].MCCH_MESS.Active to 1 (allow to  transfer MCCH message RRC->MAC in function mac_rrc_data_req)

    // ??Configure MCCH logical channel
    // call mac_config_req with appropriate structure from ASN.1 description

    //  LOG_I(RRC, "DUY: serviceID is %d\n",eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->tmgi_r9.serviceId_r9.buf[2]);
    //  LOG_I(RRC, "DUY: session ID is %d\n",eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->sessionId_r9->buf[0]);
    rrc_mac_config_req(enb_mod_idP, ENB_FLAG_YES, 0, 0,
                       (RadioResourceConfigCommonSIB_t *) NULL,
                       (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                       (MeasObjectToAddMod_t **) NULL,
                       (MAC_MainConfig_t *) NULL,
                       0,
                       (struct LogicalChannelConfig *)NULL,
                       (MeasGapConfig_t *) NULL,
                       (TDD_Config_t *) NULL,
                       NULL, (uint8_t *) NULL, (uint16_t *) NULL, NULL, NULL, NULL, (MBSFN_SubframeConfigList_t *) NULL
#   ifdef Rel10
                       ,
                       0,
                       (MBSFN_AreaInfoList_r9_t *) NULL,
                       (PMCH_InfoList_r9_t *) & (eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9)
#   endif
#   ifdef CBA
                       , 0, 0
#   endif
        );

    //LOG_I(RRC,"DUY: lcid after rrc_mac_config_req is %02d\n",eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9.list.array[0]->mbms_SessionInfoList_r9.list.array[0]->logicalChannelIdentity_r9);

}

/*------------------------------------------------------------------------------*/
static void init_MBMS(
    module_id_t enb_mod_idP,
    frame_t frameP) {
    // init the configuration for MTCH

    if (eNB_rrc_inst[enb_mod_idP].MBMS_flag > 0) {

        LOG_D(RRC, "[eNB %d] Frame %d : Radio Bearer config request for MBMS\n", enb_mod_idP, frameP);   //check the lcid
        // Configuring PDCP and RLC for MBMS Radio Bearer

        rrc_pdcp_config_asn1_req(enb_mod_idP, 0, frameP, 1, NULL,    // SRB_ToAddModList
                                 NULL,  // DRB_ToAddModList
                                 (DRB_ToReleaseList_t *) NULL, 0,   // security mode
                                 NULL,  // key rrc encryption
                                 NULL,  // key rrc integrity
                                 NULL   // key encryption
#   ifdef Rel10
                                 , &(eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9)
#   endif
            );

        rrc_rlc_config_asn1_req(enb_mod_idP, 0, frameP, ENB_FLAG_YES, NULL, // SRB_ToAddModList
                                NULL,   // DRB_ToAddModList
                                NULL,   // DRB_ToReleaseList
                                &(eNB_rrc_inst[enb_mod_idP].mcch_message->pmch_InfoList_r9));

        //rrc_mac_config_req();
    }
}
#endif

/*------------------------------------------------------------------------------*/
static void rrc_lite_eNB_init_security(
    module_id_t enb_mod_idP,
    module_id_t ue_mod_idP) {
#if defined(ENABLE_SECURITY)
    char                                ascii_buffer[65];
    uint8_t                             i;

    memset(eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], ue_mod_idP, 32);

    for (i = 0; i < 32; i++) {
        sprintf(&ascii_buffer[2 * i], "%02X", eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP][i]);
    }
    ascii_buffer[2 * i] = '\0';

    LOG_T(RRC, "[OSA][eNB %d][UE %d] kenb    = %s\n", enb_mod_idP, ue_mod_idP, ascii_buffer);
#endif
}

/*------------------------------------------------------------------------------*/
uint8_t rrc_eNB_get_next_transaction_identifier(
    module_id_t enb_mod_idP) {
    static uint8_t                      rrc_transaction_identifier[NUMBER_OF_eNB_MAX];

    rrc_transaction_identifier[enb_mod_idP] = (rrc_transaction_identifier[enb_mod_idP] + 1) % RRC_TRANSACTION_IDENTIFIER_NUMBER;

    return rrc_transaction_identifier[enb_mod_idP];
}

/*------------------------------------------------------------------------------*/
/* Functions to handle UE index in eNB UE list */


static module_id_t rrc_eNB_get_UE_index(
    module_id_t enb_mod_idP,
    uint64_t UE_identity) {

    boolean_t      reg = FALSE;
    module_id_t    i;

    AssertFatal(enb_mod_idP < NB_eNB_INST, "eNB index invalid (%d/%d)!", enb_mod_idP, NB_eNB_INST);

    for (i = 0; i < NUMBER_OF_UE_MAX; i++) {
      if (eNB_rrc_inst[enb_mod_idP].Info.UE_list[i] == UE_identity) {
	// UE_identity already registered
	reg = TRUE;
	break;
      }
    }
    
    if (reg == FALSE) {
      return (UE_MODULE_INVALID);
    } else
      return (i);
}

static module_id_t rrc_eNB_get_next_free_UE_index(
    module_id_t enb_mod_idP,
    uint64_t UE_identity) {

    boolean_t      reg = FALSE;
    module_id_t    i, first_index = UE_MODULE_INVALID;

    LOG_D(RRC,"ENB_ID= %d \n",enb_mod_idP);
    LOG_D(RRC,"N_ID= %d \n",NB_eNB_INST);

    AssertFatal(enb_mod_idP < NB_eNB_INST, "eNB index invalid (%d/%d)!", enb_mod_idP, NB_eNB_INST);

    for (i = 0; i < NUMBER_OF_UE_MAX; i++) {
        if ((first_index == UE_MODULE_INVALID) && (eNB_rrc_inst[enb_mod_idP].Info.UE_list[i] == 0)) {
	  first_index = i;    // save first free position
        }

        if (eNB_rrc_inst[enb_mod_idP].Info.UE_list[i] == UE_identity) {
            // UE_identity already registered
	  reg = TRUE;
        }
    }

    if (reg == 0) {
      LOG_I(RRC, "[eNB %d] Adding UE %d with identity " PRIu64 "\n", enb_mod_idP, first_index, UE_identity);
      return (first_index);
    } else
      return (UE_MODULE_INVALID);
}

void rrc_eNB_free_UE_index(
			   module_id_t enb_mod_idP,
			   module_id_t ue_mod_idP,
			   int frameP) {

  DRB_ToAddModList_t                 *DRB_configList = eNB_rrc_inst[enb_mod_idP].DRB_configList[ue_mod_idP];
  SRB_ToAddModList_t                 *SRB_configList = eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP];

  AssertFatal(enb_mod_idP < NB_eNB_INST, "eNB inst invalid (%d/%d) for UE %d!", enb_mod_idP, NB_eNB_INST, ue_mod_idP);
  AssertFatal(ue_mod_idP < NUMBER_OF_UE_MAX, "UE inst invalid (%d/%d) for eNB %d!", ue_mod_idP, NUMBER_OF_UE_MAX,
	      enb_mod_idP);
  
  LOG_W(RRC, "[eNB %d] Removing UE %d rv 0x%" PRIx64 "\n", enb_mod_idP, ue_mod_idP,
	eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_idP]);
  eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_idP].Status = RRC_IDLE;
  eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_idP] = 0;

  rrc_rlc_remove_rlc(enb_mod_idP, ue_mod_idP, frameP, ENB_FLAG_YES, SRB_FLAG_YES, MBMS_FLAG_NO,1);
  free(eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP]);
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_RRCConnectionSetupComplete(
    module_id_t enb_mod_idP,
    frame_t     frameP,
    module_id_t ue_mod_idP,
    RRCConnectionSetupComplete_r8_IEs_t * rrcConnectionSetupComplete) {
    LOG_I(RRC,
          "[eNB %d][RAPROC] Frame %d : Logical Channel UL-DCCH, " "processing RRCConnectionSetupComplete from UE %d\n",
          enb_mod_idP, frameP, ue_mod_idP);

#if defined(ENABLE_USE_MME)
    if (EPC_MODE_ENABLED == 1) {
        // Forward message to S1AP layer
        rrc_eNB_send_S1AP_NAS_FIRST_REQ(enb_mod_idP, ue_mod_idP, rrcConnectionSetupComplete);
    } else
#endif
    {
        // RRC loop back (no S1AP), send SecurityModeCommand to UE
        rrc_eNB_generate_SecurityModeCommand(enb_mod_idP, frameP, ue_mod_idP);
        // rrc_eNB_generate_UECapabilityEnquiry(enb_mod_idP,frameP,ue_mod_idP);
    }
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_SecurityModeCommand(
    module_id_t enb_mod_idP,
    frame_t     frameP,
    module_id_t ue_mod_idP) {
    uint8_t                             buffer[100];
    uint8_t                             size;

    size = do_SecurityModeCommand(enb_mod_idP, buffer, ue_mod_idP, rrc_eNB_get_next_transaction_identifier(enb_mod_idP),
				  eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP],
                                  eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP]);

#ifdef RRC_MSG_PRINT
    uint16_t i=0;
    LOG_F(RRC,"[MSG] RRC Security Mode Command\n");
    for (i = 0; i < size; i++)
      LOG_F(RRC,"%02x ", ((uint8_t*)buffer)[i]);
    LOG_F(RRC,"\n");
#endif

    LOG_I(RRC,
          "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate SecurityModeCommand (bytes %d, UE id %d)\n",
          enb_mod_idP, frameP, size, ue_mod_idP);

    LOG_D(RRC,
          "[FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (securityModeCommand to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
          frameP, enb_mod_idP, size, ue_mod_idP, rrc_eNB_mui, enb_mod_idP, DCCH);
    //rrc_rlc_data_req(enb_mod_idP,frameP, 1,(ue_mod_idP*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
    pdcp_rrc_data_req(enb_mod_idP, ue_mod_idP, frameP, 1, DCCH, rrc_eNB_mui++, 0, size, buffer, 1);

}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_UECapabilityEnquiry(
    module_id_t enb_mod_idP,
    frame_t     frameP,
    module_id_t ue_mod_idP) {

    uint8_t                             buffer[100];
    uint8_t                             size;

    size = do_UECapabilityEnquiry(enb_mod_idP, buffer, ue_mod_idP, rrc_eNB_get_next_transaction_identifier(enb_mod_idP));

    LOG_I(RRC,
          "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate UECapabilityEnquiry (bytes %d, UE id %d)\n",
          enb_mod_idP, frameP, size, ue_mod_idP);

    LOG_D(RRC,
          "[FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (UECapabilityEnquiry to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
          frameP, enb_mod_idP, size, ue_mod_idP, rrc_eNB_mui, enb_mod_idP, DCCH);
    //rrc_rlc_data_req(enb_mod_idP,frameP, 1,(ue_mod_idP*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
    pdcp_rrc_data_req(enb_mod_idP, ue_mod_idP, frameP, 1, DCCH, rrc_eNB_mui++, 0, size, buffer, 1);

}

void rrc_eNB_generate_RRCConnectionRelease(module_id_t enb_mod_idP,
					   frame_t frameP,
					   ue_id_t ue_idP) {

    uint8_t                             buffer[RRC_BUF_SIZE];
    uint16_t                            size;

    memset(buffer, 0, RRC_BUF_SIZE);

    size = do_RRCConnectionRelease(enb_mod_idP, buffer,rrc_eNB_get_next_transaction_identifier(enb_mod_idP));

    LOG_I(RRC,
          "[eNB %d] Frame %d Logical Channel DL-DCCH, Generate RRCConnectionRelease (bytes %d, UE id %d)\n",
          enb_mod_idP, frameP, size, ue_idP);

    LOG_D(RRC,
          "[FRAME %05d][RRC_eNB][MOD %u/%u][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionRelease to UE %d MUI %d) --->][PDCP][MOD %u/%u][RB %u]\n",
          frameP, enb_mod_idP, ue_idP, size, ue_idP, rrc_eNB_mui, enb_mod_idP, ue_idP, DCCH);
    //rrc_rlc_data_req(enb_mod_idP,frameP, 1,(ue_idP*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
    pdcp_rrc_data_req(enb_mod_idP, ue_idP, frameP, 1, DCCH, rrc_eNB_mui++, 0, size, buffer, 1);
}
						  
/*------------------------------------------------------------------------------*/
static void rrc_eNB_generate_defaultRRCConnectionReconfiguration(
								 module_id_t enb_mod_idP,
								 frame_t     frameP,
								 module_id_t ue_mod_idP,
								 uint8_t ho_state) {
#if defined(ENABLE_ITTI)
  eNB_RRC_UE_INFO                    *UE_info = &eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_idP];
#endif
    uint8_t                             buffer[RRC_BUF_SIZE];
    uint16_t                            size;
    int                                 i;

    // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE
    eNB_RRC_INST                       *rrc_inst = &eNB_rrc_inst[enb_mod_idP];

    struct PhysicalConfigDedicated    **physicalConfigDedicated = &rrc_inst->physicalConfigDedicated[ue_mod_idP];

    struct SRB_ToAddMod                *SRB2_config                      = NULL;
    struct SRB_ToAddMod__rlc_Config    *SRB2_rlc_config                  = NULL;
    struct SRB_ToAddMod__logicalChannelConfig *SRB2_lchan_config         = NULL;
    struct LogicalChannelConfig__ul_SpecificParameters
                                       *SRB2_ul_SpecificParameters       = NULL;
    SRB_ToAddModList_t                 *SRB_configList = rrc_inst->SRB_configList[ue_mod_idP];
    SRB_ToAddModList_t                 *SRB_configList2                  = NULL;

    struct DRB_ToAddMod                *DRB_config                       = NULL;
    struct RLC_Config                  *DRB_rlc_config                   = NULL;
    struct PDCP_Config                 *DRB_pdcp_config                  = NULL;
    struct PDCP_Config__rlc_AM         *PDCP_rlc_AM                      = NULL;
    struct PDCP_Config__rlc_UM         *PDCP_rlc_UM                      = NULL;
    struct LogicalChannelConfig        *DRB_lchan_config                 = NULL;
    struct LogicalChannelConfig__ul_SpecificParameters
                                       *DRB_ul_SpecificParameters        = NULL;
    DRB_ToAddModList_t                **DRB_configList = &rrc_inst->DRB_configList[ue_mod_idP];

    MAC_MainConfig_t                   *mac_MainConfig                   = NULL;
    MeasObjectToAddModList_t           *MeasObj_list                     = NULL;
    MeasObjectToAddMod_t               *MeasObj                          = NULL;
    ReportConfigToAddModList_t         *ReportConfig_list                = NULL;
    ReportConfigToAddMod_t             *ReportConfig_per, *ReportConfig_A1,
        *ReportConfig_A2, *ReportConfig_A3, *ReportConfig_A4, *ReportConfig_A5;
    MeasIdToAddModList_t               *MeasId_list                      = NULL;
    MeasIdToAddMod_t                   *MeasId0, *MeasId1, *MeasId2, *MeasId3, *MeasId4, *MeasId5;
#if Rel10
    long                               *sr_ProhibitTimer_r9              = NULL;
//     uint8_t sCellIndexToAdd = rrc_find_free_SCell_index(enb_mod_idP, ue_mod_idP, 1);
    uint8_t                            sCellIndexToAdd = 0;
#endif

    long                               *logicalchannelgroup, *logicalchannelgroup_drb;
    long                               *maxHARQ_Tx, *periodicBSR_Timer;

    RSRP_Range_t                       *rsrp                             = NULL;
    struct MeasConfig__speedStatePars  *Sparams                          = NULL;
    QuantityConfig_t                   *quantityConfig                   = NULL;
    CellsToAddMod_t                    *CellToAdd                        = NULL;
    CellsToAddModList_t                *CellsToAddModList                = NULL;
    struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList *dedicatedInfoNASList = NULL;
    DedicatedInfoNAS_t                 *dedicatedInfoNas                 = NULL;

    C_RNTI_t                           *cba_RNTI                         = NULL;
#ifdef CBA
    //struct PUSCH_CBAConfigDedicated_vlola  *pusch_CBAConfigDedicated_vlola;
    uint8_t                            *cba_RNTI_buf;
    cba_RNTI = CALLOC(1, sizeof(C_RNTI_t));
    cba_RNTI_buf = CALLOC(1, 2 * sizeof(uint8_t));
    cba_RNTI->buf = cba_RNTI_buf;
    cba_RNTI->size = 2;
    cba_RNTI->bits_unused = 0;
    // associate UEs to the CBa groups as a function of their UE id
    if (rrc_inst->num_active_cba_groups) {
        cba_RNTI->buf[0] = rrc_inst->cba_rnti[ue_mod_idP % rrc_inst->num_active_cba_groups] & 0xff;
        cba_RNTI->buf[1] = 0xff;
        LOG_D(RRC,
              "[eNB %d] Frame %d: cba_RNTI = %x in group %d is attribued to UE %d\n",
              enb_mod_idP, frameP,
              rrc_inst->cba_rnti[ue_mod_idP % rrc_inst->num_active_cba_groups],
              ue_mod_idP % rrc_inst->num_active_cba_groups, ue_mod_idP);
    } else {
        cba_RNTI->buf[0] = 0x0;
        cba_RNTI->buf[1] = 0x0;
        LOG_D(RRC, "[eNB %d] Frame %d: no cba_RNTI is configured for UE %d\n", enb_mod_idP, frameP, ue_mod_idP);
    }

#endif
    // Configure SRB2
    /// SRB2
    SRB2_config = CALLOC(1, sizeof(*SRB2_config));
    SRB_configList2 = CALLOC(1, sizeof(*SRB_configList2));
    memset(SRB_configList2, 0, sizeof(*SRB_configList2));

    SRB2_config->srb_Identity = 2;
    SRB2_rlc_config = CALLOC(1, sizeof(*SRB2_rlc_config));
    SRB2_config->rlc_Config = SRB2_rlc_config;

    SRB2_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
    SRB2_rlc_config->choice.explicitValue.present = RLC_Config_PR_am;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms15;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU = PollPDU_p8;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte = PollByte_kB1000;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t32;
    SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms50;
    SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit = T_StatusProhibit_ms10;

    SRB2_lchan_config = CALLOC(1, sizeof(*SRB2_lchan_config));
    SRB2_config->logicalChannelConfig = SRB2_lchan_config;

    SRB2_lchan_config->present = SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;

    SRB2_ul_SpecificParameters = CALLOC(1, sizeof(*SRB2_ul_SpecificParameters));

    SRB2_ul_SpecificParameters->priority = 1;
    SRB2_ul_SpecificParameters->prioritisedBitRate =
        LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
    SRB2_ul_SpecificParameters->bucketSizeDuration =
        LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

    // LCG for CCCH and DCCH is 0 as defined in 36331
    logicalchannelgroup = CALLOC(1, sizeof(long));
    *logicalchannelgroup = 0;

    SRB2_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;

    SRB2_lchan_config->choice.explicitValue.ul_SpecificParameters = SRB2_ul_SpecificParameters;
    ASN_SEQUENCE_ADD(&SRB_configList->list, SRB2_config);
    ASN_SEQUENCE_ADD(&SRB_configList2->list, SRB2_config);

    // Configure DRB
    *DRB_configList = CALLOC(1, sizeof(*DRB_configList));
    /// DRB
    DRB_config = CALLOC(1, sizeof(*DRB_config));

    DRB_config->eps_BearerIdentity = CALLOC(1, sizeof(long));
    *(DRB_config->eps_BearerIdentity) = 5L; // LW set to first value, allowed value 5..15, value : x+4
    // DRB_config->drb_Identity = (DRB_Identity_t) 1; //allowed values 1..32
    // NN: this is the 1st DRB for this ue, so set it to 1
    // NN: this is the 1st DRB for this ue, so set it to 1
    DRB_config->drb_Identity = (DRB_Identity_t) 1;  // (ue_mod_idP+1); //allowed values 1..32, value: x
    DRB_config->logicalChannelIdentity = CALLOC(1, sizeof(long));
    *(DRB_config->logicalChannelIdentity) = (long)3; // value : x+2
    DRB_rlc_config = CALLOC(1, sizeof(*DRB_rlc_config));
    DRB_config->rlc_Config = DRB_rlc_config;

#ifdef RRC_RLC_AM // EXMIMO_IOT
    DRB_rlc_config->present = RLC_Config_PR_am;
    DRB_rlc_config->choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms50;
    DRB_rlc_config->choice.am.ul_AM_RLC.pollPDU = PollPDU_p16;
    DRB_rlc_config->choice.am.ul_AM_RLC.pollByte = PollByte_kBinfinity;
    DRB_rlc_config->choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t8;
    DRB_rlc_config->choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms35;
    DRB_rlc_config->choice.am.dl_AM_RLC.t_StatusProhibit = T_StatusProhibit_ms25;
#else
    DRB_rlc_config->present = RLC_Config_PR_um_Bi_Directional;
    DRB_rlc_config->choice.um_Bi_Directional.ul_UM_RLC.sn_FieldLength = SN_FieldLength_size10;
    DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.sn_FieldLength = SN_FieldLength_size10;
    DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.t_Reordering = T_Reordering_ms25;
#endif

    DRB_pdcp_config = CALLOC(1, sizeof(*DRB_pdcp_config));
    DRB_config->pdcp_Config = DRB_pdcp_config;
    DRB_pdcp_config->discardTimer = CALLOC(1, sizeof(long));
    *DRB_pdcp_config->discardTimer = PDCP_Config__discardTimer_infinity;
    DRB_pdcp_config->rlc_AM = NULL;
    DRB_pdcp_config->rlc_UM = NULL;

#ifdef RRC_RLC_AM // EXMIMO_IOT
    PDCP_rlc_AM = CALLOC(1, sizeof(*PDCP_rlc_AM));
    DRB_pdcp_config->rlc_AM = PDCP_rlc_AM;
    PDCP_rlc_AM->statusReportRequired = FALSE;
#else
    PDCP_rlc_UM = CALLOC(1, sizeof(*PDCP_rlc_UM));
    DRB_pdcp_config->rlc_UM = PDCP_rlc_UM;
    PDCP_rlc_UM->pdcp_SN_Size = PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits;
#endif
    DRB_pdcp_config->headerCompression.present = PDCP_Config__headerCompression_PR_notUsed;

    DRB_lchan_config = CALLOC(1, sizeof(*DRB_lchan_config));
    DRB_config->logicalChannelConfig = DRB_lchan_config;
    DRB_ul_SpecificParameters = CALLOC(1, sizeof(*DRB_ul_SpecificParameters));
    DRB_lchan_config->ul_SpecificParameters = DRB_ul_SpecificParameters;

    DRB_ul_SpecificParameters->priority = 2;    // lower priority than srb1, srb2
    DRB_ul_SpecificParameters->prioritisedBitRate =
        LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
    DRB_ul_SpecificParameters->bucketSizeDuration =
        LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

    // LCG for DTCH can take the value from 1 to 3 as defined in 36331: normally controlled by upper layers (like RRM)
    logicalchannelgroup_drb = CALLOC(1, sizeof(long));
    *logicalchannelgroup_drb = 1;
    DRB_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup_drb;

    ASN_SEQUENCE_ADD(&(*DRB_configList)->list, DRB_config);

    mac_MainConfig = CALLOC(1, sizeof(*mac_MainConfig));
    eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP] = mac_MainConfig;

    mac_MainConfig->ul_SCH_Config = CALLOC(1, sizeof(*mac_MainConfig->ul_SCH_Config));

    maxHARQ_Tx = CALLOC(1, sizeof(long));
    *maxHARQ_Tx = MAC_MainConfig__ul_SCH_Config__maxHARQ_Tx_n5;
    mac_MainConfig->ul_SCH_Config->maxHARQ_Tx = maxHARQ_Tx;
    periodicBSR_Timer = CALLOC(1, sizeof(long));
    *periodicBSR_Timer = MAC_MainConfig__ul_SCH_Config__periodicBSR_Timer_sf64;
    mac_MainConfig->ul_SCH_Config->periodicBSR_Timer = periodicBSR_Timer;
    mac_MainConfig->ul_SCH_Config->retxBSR_Timer = MAC_MainConfig__ul_SCH_Config__retxBSR_Timer_sf320;
    mac_MainConfig->ul_SCH_Config->ttiBundling = 0; // FALSE

    mac_MainConfig->timeAlignmentTimerDedicated = TimeAlignmentTimer_infinity;

    mac_MainConfig->drx_Config = NULL;

    mac_MainConfig->phr_Config = CALLOC(1, sizeof(*mac_MainConfig->phr_Config));

    mac_MainConfig->phr_Config->present = MAC_MainConfig__phr_Config_PR_setup;
    mac_MainConfig->phr_Config->choice.setup.periodicPHR_Timer = MAC_MainConfig__phr_Config__setup__periodicPHR_Timer_sf20; // sf20 = 20 subframes

    mac_MainConfig->phr_Config->choice.setup.prohibitPHR_Timer = MAC_MainConfig__phr_Config__setup__prohibitPHR_Timer_sf20; // sf20 = 20 subframes

    mac_MainConfig->phr_Config->choice.setup.dl_PathlossChange = MAC_MainConfig__phr_Config__setup__dl_PathlossChange_dB1;  // Value dB1 =1 dB, dB3 = 3 dB

#ifdef Rel10
    sr_ProhibitTimer_r9 = CALLOC(1, sizeof(long));
    *sr_ProhibitTimer_r9 = 0;   // SR tx on PUCCH, Value in number of SR period(s). Value 0 = no timer for SR, Value 2= 2*SR
    mac_MainConfig->sr_ProhibitTimer_r9 = sr_ProhibitTimer_r9;
    //sps_RA_ConfigList_rlola = NULL;
#endif

    // Measurement ID list
    MeasId_list = CALLOC(1, sizeof(*MeasId_list));
    memset((void *)MeasId_list, 0, sizeof(*MeasId_list));

    MeasId0 = CALLOC(1, sizeof(*MeasId0));
    MeasId0->measId = 1;
    MeasId0->measObjectId = 1;
    MeasId0->reportConfigId = 1;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId0);

    MeasId1 = CALLOC(1, sizeof(*MeasId1));
    MeasId1->measId = 2;
    MeasId1->measObjectId = 1;
    MeasId1->reportConfigId = 2;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId1);

    MeasId2 = CALLOC(1, sizeof(*MeasId2));
    MeasId2->measId = 3;
    MeasId2->measObjectId = 1;
    MeasId2->reportConfigId = 3;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId2);

    MeasId3 = CALLOC(1, sizeof(*MeasId3));
    MeasId3->measId = 4;
    MeasId3->measObjectId = 1;
    MeasId3->reportConfigId = 4;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId3);

    MeasId4 = CALLOC(1, sizeof(*MeasId4));
    MeasId4->measId = 5;
    MeasId4->measObjectId = 1;
    MeasId4->reportConfigId = 5;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId4);

    MeasId5 = CALLOC(1, sizeof(*MeasId5));
    MeasId5->measId = 6;
    MeasId5->measObjectId = 1;
    MeasId5->reportConfigId = 6;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId5);

    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measIdToAddModList = MeasId_list;

    // Add one EUTRA Measurement Object
    MeasObj_list = CALLOC(1, sizeof(*MeasObj_list));
    memset((void *)MeasObj_list, 0, sizeof(*MeasObj_list));

    // Configure MeasObject

    MeasObj = CALLOC(1, sizeof(*MeasObj));
    memset((void *)MeasObj, 0, sizeof(*MeasObj));

    MeasObj->measObjectId = 1;
    MeasObj->measObject.present = MeasObjectToAddMod__measObject_PR_measObjectEUTRA;
    MeasObj->measObject.choice.measObjectEUTRA.carrierFreq = 3350; //band 7, 2.68GHz 
    //MeasObj->measObject.choice.measObjectEUTRA.carrierFreq = 36090; //band 33, 1.909GHz
    MeasObj->measObject.choice.measObjectEUTRA.allowedMeasBandwidth = AllowedMeasBandwidth_mbw25;
    MeasObj->measObject.choice.measObjectEUTRA.presenceAntennaPort1 = 1;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf = CALLOC(1, sizeof(uint8_t));
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf[0] = 0;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.size = 1;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.bits_unused = 6;
    MeasObj->measObject.choice.measObjectEUTRA.offsetFreq = NULL;   // Default is 15 or 0dB

    MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList =
        (CellsToAddModList_t *) CALLOC(1, sizeof(*CellsToAddModList));

    CellsToAddModList = MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList;

    // Add adjacent cell lists (6 per eNB)
    for (i = 0; i < 6; i++) {
        CellToAdd = (CellsToAddMod_t *) CALLOC(1, sizeof(*CellToAdd));
        CellToAdd->cellIndex = i + 1;
        CellToAdd->physCellId = get_adjacent_cell_id(enb_mod_idP, i);
        CellToAdd->cellIndividualOffset = Q_OffsetRange_dB0;

        ASN_SEQUENCE_ADD(&CellsToAddModList->list, CellToAdd);
    }

    ASN_SEQUENCE_ADD(&MeasObj_list->list, MeasObj);
    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measObjectToAddModList = MeasObj_list;

    // Report Configurations for periodical, A1-A5 events
    ReportConfig_list = CALLOC(1, sizeof(*ReportConfig_list));

    ReportConfig_per = CALLOC(1, sizeof(*ReportConfig_per));

    ReportConfig_A1 = CALLOC(1, sizeof(*ReportConfig_A1));

    ReportConfig_A2 = CALLOC(1, sizeof(*ReportConfig_A2));

    ReportConfig_A3 = CALLOC(1, sizeof(*ReportConfig_A3));

    ReportConfig_A4 = CALLOC(1, sizeof(*ReportConfig_A4));

    ReportConfig_A5 = CALLOC(1, sizeof(*ReportConfig_A5));

    ReportConfig_per->reportConfigId = 1;
    ReportConfig_per->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_periodical;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.choice.periodical.purpose =
        ReportConfigEUTRA__triggerType__periodical__purpose_reportStrongestCells;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_per);

    ReportConfig_A1->reportConfigId = 2;
    ReportConfig_A1->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA1;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.
        a1_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.
        a1_Threshold.choice.threshold_RSRP = 10;

    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A1);

    if (ho_state == 1 /*HO_MEASURMENT */ ) {
        LOG_I(RRC, "[eNB %d] frame %d: requesting A2, A3, A4, A5, and A6 event reporting\n", enb_mod_idP, frameP);
        ReportConfig_A2->reportConfigId = 3;
        ReportConfig_A2->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.present =
            ReportConfigEUTRA__triggerType_PR_event;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
            ReportConfigEUTRA__triggerType__event__eventId_PR_eventA2;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA2.a2_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA2.a2_Threshold.choice.threshold_RSRP = 10;

        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerQuantity =
            ReportConfigEUTRA__triggerQuantity_rsrp;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
        ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

        ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A2);

        ReportConfig_A3->reportConfigId = 4;
        ReportConfig_A3->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.present =
            ReportConfigEUTRA__triggerType_PR_event;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
            ReportConfigEUTRA__triggerType__event__eventId_PR_eventA3;

        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.a3_Offset = 1;   //10;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA3.reportOnLeave = 1;

        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerQuantity =
            ReportConfigEUTRA__triggerQuantity_rsrp;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.hysteresis = 0.5;
        ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.timeToTrigger =
            TimeToTrigger_ms40;
        ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A3);

        ReportConfig_A4->reportConfigId = 5;
        ReportConfig_A4->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.present =
            ReportConfigEUTRA__triggerType_PR_event;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
            ReportConfigEUTRA__triggerType__event__eventId_PR_eventA4;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA4.a4_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA4.a4_Threshold.choice.threshold_RSRP = 10;

        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerQuantity =
            ReportConfigEUTRA__triggerQuantity_rsrp;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
        ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

        ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A4);

        ReportConfig_A5->reportConfigId = 6;
        ReportConfig_A5->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.present =
            ReportConfigEUTRA__triggerType_PR_event;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
            ReportConfigEUTRA__triggerType__event__eventId_PR_eventA5;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA5.a5_Threshold1.present = ThresholdEUTRA_PR_threshold_RSRP;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA5.a5_Threshold2.present = ThresholdEUTRA_PR_threshold_RSRP;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA5.a5_Threshold1.choice.threshold_RSRP = 10;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
            eventA5.a5_Threshold2.choice.threshold_RSRP = 10;

        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerQuantity =
            ReportConfigEUTRA__triggerQuantity_rsrp;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
        ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

        ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A5);
        //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->reportConfigToAddModList = ReportConfig_list;

        rsrp = CALLOC(1, sizeof(RSRP_Range_t));
        *rsrp = 20;

        Sparams = CALLOC(1, sizeof(*Sparams));
        Sparams->present = MeasConfig__speedStatePars_PR_setup;
        Sparams->choice.setup.timeToTrigger_SF.sf_High = SpeedStateScaleFactors__sf_Medium_oDot75;
        Sparams->choice.setup.timeToTrigger_SF.sf_Medium = SpeedStateScaleFactors__sf_High_oDot5;
        Sparams->choice.setup.mobilityStateParameters.n_CellChangeHigh = 10;
        Sparams->choice.setup.mobilityStateParameters.n_CellChangeMedium = 5;
        Sparams->choice.setup.mobilityStateParameters.t_Evaluation = MobilityStateParameters__t_Evaluation_s60;
        Sparams->choice.setup.mobilityStateParameters.t_HystNormal = MobilityStateParameters__t_HystNormal_s120;

        quantityConfig = CALLOC(1, sizeof(*quantityConfig));
        memset((void *)quantityConfig, 0, sizeof(*quantityConfig));
        quantityConfig->quantityConfigEUTRA = CALLOC(1, sizeof(struct QuantityConfigEUTRA));
        memset((void *)quantityConfig->quantityConfigEUTRA, 0, sizeof(*quantityConfig->quantityConfigEUTRA));
        quantityConfig->quantityConfigCDMA2000 = NULL;
        quantityConfig->quantityConfigGERAN = NULL;
        quantityConfig->quantityConfigUTRA = NULL;
        quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP =
            CALLOC(1, sizeof(*(quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP)));
        quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ =
            CALLOC(1, sizeof(*(quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ)));
        *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = FilterCoefficient_fc4;
        *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = FilterCoefficient_fc4;

        LOG_I(RRC,
              "[eNB %d] Frame %d: potential handover preparation: store the information in an intermediate structure in case of failure\n",
              enb_mod_idP, frameP);
        // store the information in an intermediate structure for Hanodver management
        //rrc_inst->handover_info.as_config.sourceRadioResourceConfig.srb_ToAddModList = CALLOC(1,sizeof());
        rrc_inst->handover_info[ue_mod_idP] = CALLOC(1, sizeof(*(rrc_inst->handover_info[ue_mod_idP])));
        //memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.srb_ToAddModList,(void *)SRB_list,sizeof(SRB_ToAddModList_t));
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.srb_ToAddModList = SRB_configList2;
        //memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.drb_ToAddModList,(void *)DRB_list,sizeof(DRB_ToAddModList_t));
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.drb_ToAddModList = *DRB_configList;
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.drb_ToReleaseList = NULL;
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.mac_MainConfig =
            CALLOC(1, sizeof(*rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.mac_MainConfig));
        memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.mac_MainConfig,
               (void *)mac_MainConfig, sizeof(MAC_MainConfig_t));
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.physicalConfigDedicated =
            CALLOC(1, sizeof(PhysicalConfigDedicated_t));
        memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.physicalConfigDedicated,
               (void *)rrc_inst->physicalConfigDedicated[ue_mod_idP], sizeof(PhysicalConfigDedicated_t));
        rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.sps_Config = NULL;
        //memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.sps_Config,(void *)rrc_inst->sps_Config[ue_mod_idP],sizeof(SPS_Config_t));

    }
#if defined(ENABLE_ITTI)
    /* Initialize NAS list */
    dedicatedInfoNASList = CALLOC(1, sizeof(struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList));
    /* Add all NAS PDUs to the list */
    for (i = 0; i < UE_info->nb_of_e_rabs; i++) {
        if (UE_info->e_rab[i].param.nas_pdu.buffer != NULL) {
            dedicatedInfoNas = CALLOC(1, sizeof(DedicatedInfoNAS_t));
	    memset(dedicatedInfoNas, 0, sizeof(OCTET_STRING_t));
            OCTET_STRING_fromBuf(dedicatedInfoNas, (char *)UE_info->e_rab[i].param.nas_pdu.buffer,
                                 UE_info->e_rab[i].param.nas_pdu.length);
            ASN_SEQUENCE_ADD(&dedicatedInfoNASList->list, dedicatedInfoNas);
        }

        /* TODO parameters yet to process ... */
        {
            UE_info->e_rab[i].param.qos;
            UE_info->e_rab[i].param.sgw_addr;
            UE_info->e_rab[i].param.gtp_teid;
        }

        /* TODO should test if e RAB are Ok before! */
        eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_idP].e_rab[i].status = E_RAB_STATUS_DONE;
    }
    /* If list is empty free the list and reset the address */
    if (dedicatedInfoNASList->list.count == 0) {
        free(dedicatedInfoNASList);
        dedicatedInfoNASList = NULL;
    }
#endif

    memset(buffer, 0, RRC_BUF_SIZE);

    size = do_RRCConnectionReconfiguration(enb_mod_idP, buffer, ue_mod_idP, 
					   rrc_eNB_get_next_transaction_identifier(enb_mod_idP),   //Transaction_id,
					   NULL, /// NN: do not reconfig srb1: SRB_configList2,
					   *DRB_configList, NULL,  // DRB2_list,
					   NULL,    // *sps_Config,
#ifdef EXMIMO_IOT
					   NULL, NULL, NULL, NULL,NULL,
#else
					   physicalConfigDedicated[ue_mod_idP], MeasObj_list, ReportConfig_list, quantityConfig, MeasId_list,
#endif
					   mac_MainConfig, NULL, NULL, Sparams, rsrp, cba_RNTI, dedicatedInfoNASList
#ifdef Rel10
					   , NULL //SCellToAddMod_r10_t  
#endif
					   );

#ifdef RRC_MSG_PRINT
    LOG_F(RRC,"[MSG] RRC Connection Reconfiguration\n");
  for (i = 0; i < size; i++)
      LOG_F(RRC,"%02x ", ((uint8_t*)buffer)[i]);
  LOG_F(RRC,"\n");
  ////////////////////////////////////////
#endif

#if defined(ENABLE_ITTI)
    /* Free all NAS PDUs */
    for (i = 0; i < UE_info->nb_of_e_rabs; i++) {
        if (UE_info->e_rab[i].param.nas_pdu.buffer != NULL) {
            /* Free the NAS PDU buffer and invalidate it */
            free(UE_info->e_rab[i].param.nas_pdu.buffer);
            UE_info->e_rab[i].param.nas_pdu.buffer = NULL;
        }
    }
#endif

    LOG_I(RRC,
          "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate RRCConnectionReconfiguration (bytes %d, UE id %d)\n",
          enb_mod_idP, frameP, size, ue_mod_idP);

    LOG_D(RRC,
          "[FRAME %05d][RRC_eNB][MOD %u/%u][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionReconfiguration to UE %d MUI %d) --->][PDCP][MOD %u/%u][RB %u]\n",
          frameP, enb_mod_idP, ue_mod_idP, size, ue_mod_idP, rrc_eNB_mui, enb_mod_idP, ue_mod_idP, DCCH);
    //rrc_rlc_data_req(enb_mod_idP,frameP, 1,(ue_mod_idP*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
    pdcp_rrc_data_req(enb_mod_idP, ue_mod_idP, frameP, 1, DCCH, rrc_eNB_mui++, 0, size, buffer, 1);
}



int rrc_eNB_generate_RRCConnectionReconfiguration_SCell(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frame, uint32_t dl_CarrierFreq_r10) {

  uint8_t size;
  uint8_t buffer[100];
  
#ifdef Rel10
  uint8_t sCellIndexToAdd = 0; //one SCell so far
//   uint8_t sCellIndexToAdd;
//   sCellIndexToAdd = rrc_find_free_SCell_index(enb_mod_idP, ue_mod_idP, 1);
//  if (eNB_rrc_inst[enb_mod_idP].sCell_config[ue_mod_idP][sCellIndexToAdd] ) {
  if (eNB_rrc_inst[enb_mod_idP].sCell_config != NULL) {
    eNB_rrc_inst[enb_mod_idP].sCell_config[ue_mod_idP][sCellIndexToAdd].cellIdentification_r10->dl_CarrierFreq_r10 = dl_CarrierFreq_r10;
  }
  else {
    LOG_E(RRC,"Scell not configured!\n");
    return(-1);
  }  
#endif
  size = do_RRCConnectionReconfiguration(enb_mod_idP,
                                         buffer,
                                         ue_mod_idP,
                                         rrc_eNB_get_next_transaction_identifier(enb_mod_idP),//Transaction_id,
                                         (SRB_ToAddModList_t*)NULL,
                                         (DRB_ToAddModList_t*)NULL,
                                         (DRB_ToReleaseList_t*)NULL,
                                         (struct SPS_Config*)NULL,
                                         (struct PhysicalConfigDedicated*)NULL,
                                         (MeasObjectToAddModList_t*)NULL,
                                         (ReportConfigToAddModList_t*)NULL,
                                         (QuantityConfig_t*)NULL, 
                                         (MeasIdToAddModList_t*)NULL,
                                         (MAC_MainConfig_t*)NULL,
                                         (MeasGapConfig_t*)NULL,
                                         (MobilityControlInfo_t*)NULL,
                                         (struct MeasConfig__speedStatePars*)NULL,
                                         (RSRP_Range_t*)NULL,
                                         (C_RNTI_t*)NULL,
                                         (struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList*)NULL

#ifdef Rel10
					 , eNB_rrc_inst[enb_mod_idP].sCell_config
#endif
					 ); 

  LOG_I(RRC,"[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate RRCConnectionReconfiguration (bytes %d, UE id %d)\n",
        enb_mod_idP,frame, size, ue_mod_idP);

  LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionReconfiguration to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
        frame, enb_mod_idP, size, ue_mod_idP, rrc_eNB_mui, enb_mod_idP, /*(ue_mod_idP*MAX_NUM_RB)+*/DCCH);
  //rrc_rlc_data_req(Mod_id,frame, 1,(UE_index*MAX_NUM_RB)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
//   pdcp_data_req(enb_mod_idP, ue_mod_idP, frame, 1, /*(UE_index * MAX_NUM_RB) + */DCCH, rrc_eNB_mui++, 0, size, (char*)buffer, 1);
    pdcp_rrc_data_req(enb_mod_idP, ue_mod_idP, frame, 1, DCCH, rrc_eNB_mui++, 0, size, buffer, 1);

  return(0);
}


/*------------------------------------------------------------------------------*/
void rrc_eNB_process_MeasurementReport(
    module_id_t enb_mod_idP,
    frame_t frameP,
    module_id_t ue_mod_idP,
    MeasResults_t * measResults2) {

    LOG_I(RRC, "[eNB %d] Frame %d: Process Measurement Report From UE %d (Measurement Id %d)\n",
          enb_mod_idP, frameP, ue_mod_idP, (int)measResults2->measId);
    if (measResults2->measResultNeighCells->choice.measResultListEUTRA.list.count > 0) {
        LOG_I(RRC, "Physical Cell Id %d\n",
              (int)measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->physCellId);
        LOG_I(RRC, "RSRP of Target %d\n",
              (int)*(measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->
                     measResult.rsrpResult));
        LOG_I(RRC, "RSRQ of Target %d\n",
              (int)*(measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->
                     measResult.rsrqResult));
    }
#ifdef Rel10
    LOG_I(RRC, "RSRP of Source %d\n", measResults2->measResultPCell.rsrpResult);
    LOG_I(RRC, "RSRQ of Source %d\n", measResults2->measResultPCell.rsrqResult);
#else
    LOG_I(RRC, "RSRP of Source %d\n", measResults2->measResultServCell.rsrpResult);
    LOG_I(RRC, "RSRQ of Source %d\n", measResults2->measResultServCell.rsrqResult);
#endif

    if (eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ho_prepare != 0xF0) {
    	rrc_eNB_generate_HandoverPreparationInformation(enb_mod_idP,
                                                        frameP,
                                                        ue_mod_idP,
                                                        measResults2->measResultNeighCells->choice.
                                                        measResultListEUTRA.list.array[0]->physCellId);
    } else {
        LOG_D(RRC, "[eNB %d] Frame %d: Ignoring MeasReport from UE %d as Handover is in progress... \n", enb_mod_idP, frameP,
              ue_mod_idP);
    }
    //Look for IP address of the target eNB
    //Send Handover Request -> target eNB
    //Wait for Handover Acknowledgement <- target eNB
    //Send Handover Command

    //x2delay();
    //    handover_request_x2(ue_mod_idP,enb_mod_idP,measResults2->measResultNeighCells->choice.measResultListEUTRA.list.array[0]->physCellId);

    //    uint8_t buffer[100];
    //    int size=rrc_eNB_generate_Handover_Command_TeNB(0,0,buffer);
    //
    //      send_check_message((char*)buffer,size);
    //send_handover_command();

}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_HandoverPreparationInformation(
    module_id_t enb_mod_idP,
    frame_t frameP,
    module_id_t ue_mod_idP,
    PhysCellId_t targetPhyId) {
    uint8_t                             UE_id_target;
    uint8_t                             mod_id_target = get_adjacent_cell_mod_id(targetPhyId);
    HANDOVER_INFO                      *handoverInfo = CALLOC(1, sizeof(*handoverInfo));
    /*
       uint8_t buffer[100];
       uint8_t size;
       struct PhysicalConfigDedicated  **physicalConfigDedicated = &eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP];
       RadioResourceConfigDedicated_t *radioResourceConfigDedicated = CALLOC(1,sizeof(RadioResourceConfigDedicated_t));
     */

    handoverInfo->as_config.antennaInfoCommon.antennaPortsCount = 0;    //Not used 0- but check value
    handoverInfo->as_config.sourceDl_CarrierFreq = 36090;   //Verify!
    memcpy((void *)&handoverInfo->as_config.sourceMasterInformationBlock,
           (void *)&eNB_rrc_inst[enb_mod_idP].mib, sizeof(MasterInformationBlock_t));
    memcpy((void *)&handoverInfo->as_config.sourceMeasConfig,
           (void *)&eNB_rrc_inst[enb_mod_idP].measConfig[ue_mod_idP], sizeof(MeasConfig_t));
    //to be configured
    memset((void *)&eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_config.sourceSecurityAlgorithmConfig,
           0, sizeof(SecurityAlgorithmConfig_t));

    memcpy((void *)&eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_config.sourceSystemInformationBlockType1,
           (void *)&eNB_rrc_inst[enb_mod_idP].SIB1, sizeof(SystemInformationBlockType1_t));
    memcpy((void *)&eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_config.sourceSystemInformationBlockType2,
           (void *)&eNB_rrc_inst[enb_mod_idP].SIB23, sizeof(SystemInformationBlockType2_t));

    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo =
        CALLOC(1, sizeof(ReestablishmentInfo_t));
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo->sourcePhysCellId =
        eNB_rrc_inst[enb_mod_idP].physCellId;
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo->targetCellShortMAC_I.buf = NULL;  // Check values later
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo->targetCellShortMAC_I.size = 0;
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo->targetCellShortMAC_I.bits_unused = 0;
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context.reestablishmentInfo->additionalReestabInfoList = NULL;

    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ho_prepare = 0xFF;    //0xF0;
    eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ho_complete = 0;

    if (mod_id_target != 0xFF) {
        //UE_id_target = rrc_find_free_ue_index(modid_target);
        UE_id_target = rrc_eNB_get_next_free_UE_index(mod_id_target, eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_idP]);  //this should return a new index

        if (UE_id_target != 0xFF) {
            LOG_N(RRC,
                  "[eNB %d] Frame %d : Emulate sending HandoverPreparationInformation msg from eNB source %d to eNB target %d: source UE_id %d target UE_id %d source_modId: %d target_modId: %d\n",
                  enb_mod_idP, frameP, eNB_rrc_inst[enb_mod_idP].physCellId, targetPhyId, ue_mod_idP, UE_id_target, enb_mod_idP,
                  mod_id_target);
            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target] =
                CALLOC(1, sizeof(*(eNB_rrc_inst[mod_id_target].handover_info[UE_id_target])));
            memcpy((void *)&eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->as_context,
                   (void *)&eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_context, sizeof(AS_Context_t));
            memcpy((void *)&eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->as_config,
                   (void *)&eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->as_config, sizeof(AS_Config_t));

            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ho_prepare = 0x00;// 0xFF;
            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ho_complete = 0;

            eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->modid_t = mod_id_target;
            eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ueid_s = ue_mod_idP;
            eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->modid_s = enb_mod_idP;
            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->modid_t = mod_id_target;
            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->modid_s = enb_mod_idP;
            eNB_rrc_inst[mod_id_target].handover_info[UE_id_target]->ueid_t = UE_id_target;
        } else
            LOG_E(RRC, "\nError in obtaining free UE id in target eNB %l for handover \n", targetPhyId);
    } else
        LOG_E(RRC, "\nError in obtaining Module ID of target eNB for handover \n");
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_handoverPreparationInformation(
    module_id_t enb_mod_idP,
    frame_t frameP,
    module_id_t ue_mod_idP) {

    LOG_I(RRC,
          "[eNB %d] Frame %d : Logical Channel UL-DCCH, processing RRCHandoverPreparationInformation, sending RRCConnectionReconfiguration to UE %d \n",
          enb_mod_idP, frameP, ue_mod_idP);

    //eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_idP]
    rrc_eNB_generate_RRCConnectionReconfiguration_handover(enb_mod_idP, frameP, ue_mod_idP, NULL, 0);

}

/*------------------------------------------------------------------------------*/
void check_handovers(
    module_id_t enb_mod_idP,
    frame_t frameP) {
    uint8_t                             i;
    int                                 result;

    for (i = 0; i < NUMBER_OF_UE_MAX; i++) {
        if (eNB_rrc_inst[enb_mod_idP].handover_info[i] != NULL) {
            if (eNB_rrc_inst[enb_mod_idP].handover_info[i]->ho_prepare == 0xFF) {
                LOG_D(RRC,
                      "[eNB %d] Frame %d: Incoming handover detected for new UE_idx %d (source eNB %d->target eNB %d) \n",
                      enb_mod_idP, frameP, i, enb_mod_idP, eNB_rrc_inst[enb_mod_idP].handover_info[i]->modid_t);
                // source eNB generates rrcconnectionreconfiguration to prepare the HO
                rrc_eNB_process_handoverPreparationInformation(enb_mod_idP, frameP, i);
                eNB_rrc_inst[enb_mod_idP].handover_info[i]->ho_prepare = 0xF1;
            }

            if (eNB_rrc_inst[enb_mod_idP].handover_info[i]->ho_complete == 0xF1) {
                LOG_D(RRC,
                      "[eNB %d] Frame %d: handover Command received for new UE_idx %d current eNB %d target eNB: %d \n",
                      enb_mod_idP, frameP, i, enb_mod_idP, eNB_rrc_inst[enb_mod_idP].handover_info[i]->modid_t);
                //rrc_eNB_process_handoverPreparationInformation(enb_mod_idP,frameP,i);
                result = pdcp_data_req(enb_mod_idP, i, frameP, ENB_FLAG_YES, SRB_FLAG_YES,
                                       DCCH,
                                       rrc_eNB_mui++, FALSE,
                                       eNB_rrc_inst[enb_mod_idP].handover_info[i]->size,
                                       eNB_rrc_inst[enb_mod_idP].handover_info[i]->buf, 1);
                AssertFatal(result == TRUE, "PDCP data request failed!\n");
                eNB_rrc_inst[enb_mod_idP].handover_info[i]->ho_complete = 0xF2;
            }
        }
    }
}

/*------------------------------------------------------------------------------*/
// 5.3.5.4 RRCConnectionReconfiguration including the mobilityControlInfo to prepare the UE handover
void rrc_eNB_generate_RRCConnectionReconfiguration_handover(
    module_id_t enb_mod_idP,
    frame_t frameP,
    module_id_t ue_mod_idP,
    uint8_t * nas_pdu,
    uint32_t  nas_length) {

    uint8_t                             buffer[RRC_BUF_SIZE];
    uint16_t                            size;
    int                                 i;
    uint8_t                             rv[2];
    uint16_t                            Idx;
    // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE
    eNB_RRC_INST                       *rrc_inst = &eNB_rrc_inst[enb_mod_idP];

    struct PhysicalConfigDedicated    **physicalConfigDedicated = &rrc_inst->physicalConfigDedicated[ue_mod_idP];

    struct SRB_ToAddMod                *SRB2_config;
    struct SRB_ToAddMod__rlc_Config    *SRB2_rlc_config;
    struct SRB_ToAddMod__logicalChannelConfig *SRB2_lchan_config;
    struct LogicalChannelConfig__ul_SpecificParameters *SRB2_ul_SpecificParameters;
    LogicalChannelConfig_t             *SRB1_logicalChannelConfig = NULL;
    SRB_ToAddModList_t                 *SRB_configList = rrc_inst->SRB_configList[ue_mod_idP];    // not used in this context: may be removed
    SRB_ToAddModList_t                 *SRB_configList2;

    struct DRB_ToAddMod                *DRB_config;
    struct RLC_Config                  *DRB_rlc_config;
    struct PDCP_Config                 *DRB_pdcp_config;
    struct PDCP_Config__rlc_UM         *PDCP_rlc_UM;
    struct LogicalChannelConfig        *DRB_lchan_config;
    struct LogicalChannelConfig__ul_SpecificParameters *DRB_ul_SpecificParameters;
    // DRB_ToAddModList_t **DRB_configList = &rrc_inst->DRB_configList[ue_mod_idP];
    DRB_ToAddModList_t                 *DRB_configList2;

    MAC_MainConfig_t                   *mac_MainConfig;
    MeasObjectToAddModList_t           *MeasObj_list;
    MeasObjectToAddMod_t               *MeasObj;
    ReportConfigToAddModList_t         *ReportConfig_list;
    ReportConfigToAddMod_t             *ReportConfig_per, *ReportConfig_A1,
        *ReportConfig_A2, *ReportConfig_A3, *ReportConfig_A4, *ReportConfig_A5;
    MeasIdToAddModList_t               *MeasId_list;
    MeasIdToAddMod_t                   *MeasId0, *MeasId1, *MeasId2, *MeasId3, *MeasId4, *MeasId5;
    QuantityConfig_t                   *quantityConfig;
    MobilityControlInfo_t              *mobilityInfo;
    // HandoverCommand_t handoverCommand;
    uint8_t                             sourceModId =
        get_adjacent_cell_mod_id(rrc_inst->handover_info[ue_mod_idP]->as_context.reestablishmentInfo->sourcePhysCellId);
#if Rel10
    long                               *sr_ProhibitTimer_r9;
#endif

    long                               *logicalchannelgroup, *logicalchannelgroup_drb;
    long                               *maxHARQ_Tx, *periodicBSR_Timer;

    // RSRP_Range_t *rsrp;
    struct MeasConfig__speedStatePars  *Sparams;
    CellsToAddMod_t                    *CellToAdd;
    CellsToAddModList_t                *CellsToAddModList;
    // srb 1: for HO
    struct SRB_ToAddMod                *SRB1_config;
    struct SRB_ToAddMod__rlc_Config    *SRB1_rlc_config;
    struct SRB_ToAddMod__logicalChannelConfig *SRB1_lchan_config;
    struct LogicalChannelConfig__ul_SpecificParameters *SRB1_ul_SpecificParameters;
    // phy config dedicated
    PhysicalConfigDedicated_t          *physicalConfigDedicated2;
    struct RRCConnectionReconfiguration_r8_IEs__dedicatedInfoNASList *dedicatedInfoNASList;

    LOG_D(RRC, "[eNB %d] Frame %d: handover preparation: get the newSourceUEIdentity (C-RNTI): ", enb_mod_idP, frameP);
    for (i = 0; i < 2; i++) {
        rv[i] = taus() & 0xff;
        LOG_D(RRC, " %x.", rv[i]);
    }

    LOG_D(RRC, "[eNB %d] Frame %d : handover reparation: add target eNB SRB1 and PHYConfigDedicated reconfiguration\n",
          enb_mod_idP, frameP);
    // 1st: reconfigure SRB
    SRB_configList2 = CALLOC(1, sizeof(*SRB_configList));
    SRB1_config = CALLOC(1, sizeof(*SRB1_config));
    SRB1_config->srb_Identity = 1;
    SRB1_rlc_config = CALLOC(1, sizeof(*SRB1_rlc_config));
    SRB1_config->rlc_Config = SRB1_rlc_config;

    SRB1_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
    SRB1_rlc_config->choice.explicitValue.present = RLC_Config_PR_am;
    SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms15;
    SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU = PollPDU_p8;
    SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte = PollByte_kB1000;
    SRB1_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t16;
    SRB1_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms50;
    SRB1_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit = T_StatusProhibit_ms10;

    SRB1_lchan_config = CALLOC(1, sizeof(*SRB1_lchan_config));
    SRB1_config->logicalChannelConfig = SRB1_lchan_config;

    SRB1_lchan_config->present = SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;
    SRB1_ul_SpecificParameters = CALLOC(1, sizeof(*SRB1_ul_SpecificParameters));

    SRB1_lchan_config->choice.explicitValue.ul_SpecificParameters = SRB1_ul_SpecificParameters;

    SRB1_ul_SpecificParameters->priority = 1;

    //assign_enum(&SRB1_ul_SpecificParameters->prioritisedBitRate,LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity);
    SRB1_ul_SpecificParameters->prioritisedBitRate =
        LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;

    //assign_enum(&SRB1_ul_SpecificParameters->bucketSizeDuration,LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50);
    SRB1_ul_SpecificParameters->bucketSizeDuration =
        LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

    logicalchannelgroup = CALLOC(1, sizeof(long));
    *logicalchannelgroup = 0;
    SRB1_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;

    ASN_SEQUENCE_ADD(&SRB_configList2->list, SRB1_config);

    //2nd: now reconfigure phy config dedicated
    physicalConfigDedicated2 = CALLOC(1, sizeof(*physicalConfigDedicated2));
    *physicalConfigDedicated = physicalConfigDedicated2;

    physicalConfigDedicated2->pdsch_ConfigDedicated =
        CALLOC(1, sizeof(*physicalConfigDedicated2->pdsch_ConfigDedicated));
    physicalConfigDedicated2->pucch_ConfigDedicated =
        CALLOC(1, sizeof(*physicalConfigDedicated2->pucch_ConfigDedicated));
    physicalConfigDedicated2->pusch_ConfigDedicated =
        CALLOC(1, sizeof(*physicalConfigDedicated2->pusch_ConfigDedicated));
    physicalConfigDedicated2->uplinkPowerControlDedicated =
        CALLOC(1, sizeof(*physicalConfigDedicated2->uplinkPowerControlDedicated));
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH =
        CALLOC(1, sizeof(*physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH));
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH =
        CALLOC(1, sizeof(*physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH));
    physicalConfigDedicated2->cqi_ReportConfig = NULL;  //CALLOC(1,sizeof(*physicalConfigDedicated2->cqi_ReportConfig));
    physicalConfigDedicated2->soundingRS_UL_ConfigDedicated = NULL; //CALLOC(1,sizeof(*physicalConfigDedicated2->soundingRS_UL_ConfigDedicated));
    physicalConfigDedicated2->antennaInfo = CALLOC(1, sizeof(*physicalConfigDedicated2->antennaInfo));
    physicalConfigDedicated2->schedulingRequestConfig =
        CALLOC(1, sizeof(*physicalConfigDedicated2->schedulingRequestConfig));
    // PDSCH
    //assign_enum(&physicalConfigDedicated2->pdsch_ConfigDedicated->p_a,
    //          PDSCH_ConfigDedicated__p_a_dB0);
    physicalConfigDedicated2->pdsch_ConfigDedicated->p_a = PDSCH_ConfigDedicated__p_a_dB0;

    // PUCCH
    physicalConfigDedicated2->pucch_ConfigDedicated->ackNackRepetition.present =
        PUCCH_ConfigDedicated__ackNackRepetition_PR_release;
    physicalConfigDedicated2->pucch_ConfigDedicated->ackNackRepetition.choice.release = 0;
    physicalConfigDedicated2->pucch_ConfigDedicated->tdd_AckNackFeedbackMode = NULL;    //PUCCH_ConfigDedicated__tdd_AckNackFeedbackMode_multiplexing;

    // Pusch_config_dedicated
    physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_ACK_Index = 0;  // 2.00
    physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_RI_Index = 0;   // 1.25
    physicalConfigDedicated2->pusch_ConfigDedicated->betaOffset_CQI_Index = 8;  // 2.25

    // UplinkPowerControlDedicated
    physicalConfigDedicated2->uplinkPowerControlDedicated->p0_UE_PUSCH = 0; // 0 dB
    //assign_enum(&physicalConfigDedicated2->uplinkPowerControlDedicated->deltaMCS_Enabled,
    // UplinkPowerControlDedicated__deltaMCS_Enabled_en1);
    physicalConfigDedicated2->uplinkPowerControlDedicated->deltaMCS_Enabled =
        UplinkPowerControlDedicated__deltaMCS_Enabled_en1;
    physicalConfigDedicated2->uplinkPowerControlDedicated->accumulationEnabled = 1; // should be TRUE in order to have 0dB power offset
    physicalConfigDedicated2->uplinkPowerControlDedicated->p0_UE_PUCCH = 0; // 0 dB
    physicalConfigDedicated2->uplinkPowerControlDedicated->pSRS_Offset = 0; // 0 dB
    physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient =
        CALLOC(1, sizeof(*physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient));
    //  assign_enum(physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient,FilterCoefficient_fc4); // fc4 dB
    *physicalConfigDedicated2->uplinkPowerControlDedicated->filterCoefficient = FilterCoefficient_fc4;  // fc4 dB

    // TPC-PDCCH-Config
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->present = TPC_PDCCH_Config_PR_setup;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_Index.present = TPC_Index_PR_indexOfFormat3;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_Index.choice.indexOfFormat3 = 1;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf = CALLOC(1, 2);
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.size = 2;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf[0] = 0x12;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.buf[1] = 0x34 + ue_mod_idP;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUCCH->choice.setup.tpc_RNTI.bits_unused = 0;

    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->present = TPC_PDCCH_Config_PR_setup;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_Index.present = TPC_Index_PR_indexOfFormat3;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_Index.choice.indexOfFormat3 = 1;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf = CALLOC(1, 2);
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.size = 2;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf[0] = 0x22;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.buf[1] = 0x34 + ue_mod_idP;
    physicalConfigDedicated2->tpc_PDCCH_ConfigPUSCH->choice.setup.tpc_RNTI.bits_unused = 0;

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
    physicalConfigDedicated2->antennaInfo = CALLOC(1, sizeof(*physicalConfigDedicated2->antennaInfo));
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
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.ue_TransmitAntennaSelection.present =
        AntennaInfoDedicated__ue_TransmitAntennaSelection_PR_release;
    physicalConfigDedicated2->antennaInfo->choice.explicitValue.ue_TransmitAntennaSelection.choice.release = 0;

    // SchedulingRequestConfig
    physicalConfigDedicated2->schedulingRequestConfig->present = SchedulingRequestConfig_PR_setup;
    physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_PUCCH_ResourceIndex = ue_mod_idP;

    if (mac_xface->lte_frame_parms->frame_type == 0) {  // FDD
        physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 5 + (ue_mod_idP % 10);   // Isr = 5 (every 10 subframes, offset=2+UE_id mod3)
    } else {
        switch (mac_xface->lte_frame_parms->tdd_config) {
            case 1:
                physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7 + (ue_mod_idP & 1) + ((ue_mod_idP & 3) >> 1) * 5;    // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 7 for UE2, 8 for UE3 , 2 for UE4 etc..)
                break;
            case 3:
                physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7 + (ue_mod_idP % 3);    // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 3 for UE2, 2 for UE3 , etc..)
                break;
            case 4:
                physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7 + (ue_mod_idP & 1);    // Isr = 5 (every 10 subframes, offset=2 for UE0, 3 for UE1, 3 for UE2, 2 for UE3 , etc..)
                break;
            default:
                physicalConfigDedicated2->schedulingRequestConfig->choice.setup.sr_ConfigIndex = 7; // Isr = 5 (every 10 subframes, offset=2 for all UE0 etc..)
                break;
        }
    }

    //  assign_enum(&physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax,
    //SchedulingRequestConfig__setup__dsr_TransMax_n4);
    //  assign_enum(&physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax = SchedulingRequestConfig__setup__dsr_TransMax_n4;
    physicalConfigDedicated2->schedulingRequestConfig->choice.setup.dsr_TransMax =
        SchedulingRequestConfig__setup__dsr_TransMax_n4;

    LOG_D(RRC,
          "handover_config [FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (SRB1 UE %d) --->][MAC_eNB][MOD %02d][]\n",
          frameP, enb_mod_idP, ue_mod_idP, enb_mod_idP);
    rrc_mac_config_req(enb_mod_idP, ENB_FLAG_YES, ue_mod_idP, 0, (RadioResourceConfigCommonSIB_t *) NULL,
                       eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP], 
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif		       
		       (MeasObjectToAddMod_t **) NULL,
                       eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP], 1, SRB1_logicalChannelConfig,
                       eNB_rrc_inst[enb_mod_idP].measGapConfig[ue_mod_idP], (TDD_Config_t *) NULL,
                       (MobilityControlInfo_t *) NULL, (uint8_t *) NULL, (uint16_t *) NULL, NULL, NULL, NULL,
                       (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                       , 0, (MBSFN_AreaInfoList_r9_t *) NULL, (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                       , eNB_rrc_inst[enb_mod_idP].num_active_cba_groups, eNB_rrc_inst[enb_mod_idP].cba_rnti[0]
#endif
        );

    // Configure target eNB SRB2
    /// SRB2
    SRB2_config = CALLOC(1, sizeof(*SRB2_config));
    SRB_configList2 = CALLOC(1, sizeof(*SRB_configList2));
    memset(SRB_configList2, 0, sizeof(*SRB_configList2));

    SRB2_config->srb_Identity = 2;
    SRB2_rlc_config = CALLOC(1, sizeof(*SRB2_rlc_config));
    SRB2_config->rlc_Config = SRB2_rlc_config;

    SRB2_rlc_config->present = SRB_ToAddMod__rlc_Config_PR_explicitValue;
    SRB2_rlc_config->choice.explicitValue.present = RLC_Config_PR_am;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.t_PollRetransmit = T_PollRetransmit_ms15;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollPDU = PollPDU_p8;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.pollByte = PollByte_kB1000;
    SRB2_rlc_config->choice.explicitValue.choice.am.ul_AM_RLC.maxRetxThreshold = UL_AM_RLC__maxRetxThreshold_t32;
    SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_Reordering = T_Reordering_ms50;
    SRB2_rlc_config->choice.explicitValue.choice.am.dl_AM_RLC.t_StatusProhibit = T_StatusProhibit_ms10;

    SRB2_lchan_config = CALLOC(1, sizeof(*SRB2_lchan_config));
    SRB2_config->logicalChannelConfig = SRB2_lchan_config;

    SRB2_lchan_config->present = SRB_ToAddMod__logicalChannelConfig_PR_explicitValue;

    SRB2_ul_SpecificParameters = CALLOC(1, sizeof(*SRB2_ul_SpecificParameters));

    SRB2_ul_SpecificParameters->priority = 1;
    SRB2_ul_SpecificParameters->prioritisedBitRate =
        LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
    SRB2_ul_SpecificParameters->bucketSizeDuration =
        LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

    // LCG for CCCH and DCCH is 0 as defined in 36331
    logicalchannelgroup = CALLOC(1, sizeof(long));
    *logicalchannelgroup = 0;

    SRB2_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup;
    SRB2_lchan_config->choice.explicitValue.ul_SpecificParameters = SRB2_ul_SpecificParameters;
    ASN_SEQUENCE_ADD(&SRB_configList->list, SRB2_config);
    ASN_SEQUENCE_ADD(&SRB_configList2->list, SRB2_config);

    // Configure target eNB DRB
    DRB_configList2 = CALLOC(1, sizeof(*DRB_configList2));
    /// DRB
    DRB_config = CALLOC(1, sizeof(*DRB_config));

    //DRB_config->drb_Identity = (DRB_Identity_t) 1; //allowed values 1..32
    // NN: this is the 1st DRB for this ue, so set it to 1
    DRB_config->drb_Identity = (DRB_Identity_t) 1;  // (ue_mod_idP+1); //allowed values 1..32
    DRB_config->logicalChannelIdentity = CALLOC(1, sizeof(long));
    *(DRB_config->logicalChannelIdentity) = (long)3;
    DRB_rlc_config = CALLOC(1, sizeof(*DRB_rlc_config));
    DRB_config->rlc_Config = DRB_rlc_config;
    DRB_rlc_config->present = RLC_Config_PR_um_Bi_Directional;
    DRB_rlc_config->choice.um_Bi_Directional.ul_UM_RLC.sn_FieldLength = SN_FieldLength_size10;
    DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.sn_FieldLength = SN_FieldLength_size10;
    DRB_rlc_config->choice.um_Bi_Directional.dl_UM_RLC.t_Reordering = T_Reordering_ms5;

    DRB_pdcp_config = CALLOC(1, sizeof(*DRB_pdcp_config));
    DRB_config->pdcp_Config = DRB_pdcp_config;
    DRB_pdcp_config->discardTimer = NULL;
    DRB_pdcp_config->rlc_AM = NULL;
    PDCP_rlc_UM = CALLOC(1, sizeof(*PDCP_rlc_UM));
    DRB_pdcp_config->rlc_UM = PDCP_rlc_UM;
    PDCP_rlc_UM->pdcp_SN_Size = PDCP_Config__rlc_UM__pdcp_SN_Size_len12bits;
    DRB_pdcp_config->headerCompression.present = PDCP_Config__headerCompression_PR_notUsed;

    DRB_lchan_config = CALLOC(1, sizeof(*DRB_lchan_config));
    DRB_config->logicalChannelConfig = DRB_lchan_config;
    DRB_ul_SpecificParameters = CALLOC(1, sizeof(*DRB_ul_SpecificParameters));
    DRB_lchan_config->ul_SpecificParameters = DRB_ul_SpecificParameters;

    DRB_ul_SpecificParameters->priority = 2;    // lower priority than srb1, srb2
    DRB_ul_SpecificParameters->prioritisedBitRate =
        LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity;
    DRB_ul_SpecificParameters->bucketSizeDuration =
        LogicalChannelConfig__ul_SpecificParameters__bucketSizeDuration_ms50;

    // LCG for DTCH can take the value from 1 to 3 as defined in 36331: normally controlled by upper layers (like RRM)
    logicalchannelgroup_drb = CALLOC(1, sizeof(long));
    *logicalchannelgroup_drb = 1;
    DRB_ul_SpecificParameters->logicalChannelGroup = logicalchannelgroup_drb;

    ASN_SEQUENCE_ADD(&DRB_configList2->list, DRB_config);

    mac_MainConfig = CALLOC(1, sizeof(*mac_MainConfig));
    eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP] = mac_MainConfig;

    mac_MainConfig->ul_SCH_Config = CALLOC(1, sizeof(*mac_MainConfig->ul_SCH_Config));

    maxHARQ_Tx = CALLOC(1, sizeof(long));
    *maxHARQ_Tx = MAC_MainConfig__ul_SCH_Config__maxHARQ_Tx_n5;
    mac_MainConfig->ul_SCH_Config->maxHARQ_Tx = maxHARQ_Tx;

    periodicBSR_Timer = CALLOC(1, sizeof(long));
    *periodicBSR_Timer = MAC_MainConfig__ul_SCH_Config__periodicBSR_Timer_sf64;
    mac_MainConfig->ul_SCH_Config->periodicBSR_Timer = periodicBSR_Timer;

    mac_MainConfig->ul_SCH_Config->retxBSR_Timer = MAC_MainConfig__ul_SCH_Config__retxBSR_Timer_sf320;

    mac_MainConfig->ul_SCH_Config->ttiBundling = 0; // FALSE

    mac_MainConfig->drx_Config = NULL;

    mac_MainConfig->phr_Config = CALLOC(1, sizeof(*mac_MainConfig->phr_Config));

    mac_MainConfig->phr_Config->present = MAC_MainConfig__phr_Config_PR_setup;
    mac_MainConfig->phr_Config->choice.setup.periodicPHR_Timer = MAC_MainConfig__phr_Config__setup__periodicPHR_Timer_sf20; // sf20 = 20 subframes

    mac_MainConfig->phr_Config->choice.setup.prohibitPHR_Timer = MAC_MainConfig__phr_Config__setup__prohibitPHR_Timer_sf20; // sf20 = 20 subframes

    mac_MainConfig->phr_Config->choice.setup.dl_PathlossChange = MAC_MainConfig__phr_Config__setup__dl_PathlossChange_dB1;  // Value dB1 =1 dB, dB3 = 3 dB

#ifdef Rel10
    sr_ProhibitTimer_r9 = CALLOC(1, sizeof(long));
    *sr_ProhibitTimer_r9 = 0;   // SR tx on PUCCH, Value in number of SR period(s). Value 0 = no timer for SR, Value 2= 2*SR
    mac_MainConfig->sr_ProhibitTimer_r9 = sr_ProhibitTimer_r9;
    //sps_RA_ConfigList_rlola = NULL;
#endif
    // Measurement ID list
    MeasId_list = CALLOC(1, sizeof(*MeasId_list));
    memset((void *)MeasId_list, 0, sizeof(*MeasId_list));

    MeasId0 = CALLOC(1, sizeof(*MeasId0));
    MeasId0->measId = 1;
    MeasId0->measObjectId = 1;
    MeasId0->reportConfigId = 1;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId0);

    MeasId1 = CALLOC(1, sizeof(*MeasId1));
    MeasId1->measId = 2;
    MeasId1->measObjectId = 1;
    MeasId1->reportConfigId = 2;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId1);

    MeasId2 = CALLOC(1, sizeof(*MeasId2));
    MeasId2->measId = 3;
    MeasId2->measObjectId = 1;
    MeasId2->reportConfigId = 3;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId2);

    MeasId3 = CALLOC(1, sizeof(*MeasId3));
    MeasId3->measId = 4;
    MeasId3->measObjectId = 1;
    MeasId3->reportConfigId = 4;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId3);

    MeasId4 = CALLOC(1, sizeof(*MeasId4));
    MeasId4->measId = 5;
    MeasId4->measObjectId = 1;
    MeasId4->reportConfigId = 5;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId4);

    MeasId5 = CALLOC(1, sizeof(*MeasId5));
    MeasId5->measId = 6;
    MeasId5->measObjectId = 1;
    MeasId5->reportConfigId = 6;
    ASN_SEQUENCE_ADD(&MeasId_list->list, MeasId5);

    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measIdToAddModList = MeasId_list;

    // Add one EUTRA Measurement Object
    MeasObj_list = CALLOC(1, sizeof(*MeasObj_list));
    memset((void *)MeasObj_list, 0, sizeof(*MeasObj_list));

    // Configure MeasObject

    MeasObj = CALLOC(1, sizeof(*MeasObj));
    memset((void *)MeasObj, 0, sizeof(*MeasObj));

    MeasObj->measObjectId = 1;
    MeasObj->measObject.present = MeasObjectToAddMod__measObject_PR_measObjectEUTRA;
    MeasObj->measObject.choice.measObjectEUTRA.carrierFreq = 36090;
    MeasObj->measObject.choice.measObjectEUTRA.allowedMeasBandwidth = AllowedMeasBandwidth_mbw25;
    MeasObj->measObject.choice.measObjectEUTRA.presenceAntennaPort1 = 1;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf = CALLOC(1, sizeof(uint8_t));
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf[0] = 0;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.size = 1;
    MeasObj->measObject.choice.measObjectEUTRA.neighCellConfig.bits_unused = 6;
    MeasObj->measObject.choice.measObjectEUTRA.offsetFreq = NULL;   // Default is 15 or 0dB

    MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList =
        (CellsToAddModList_t *) CALLOC(1, sizeof(*CellsToAddModList));
    CellsToAddModList = MeasObj->measObject.choice.measObjectEUTRA.cellsToAddModList;

    // Add adjacent cell lists (6 per eNB)
    for (i = 0; i < 6; i++) {
        CellToAdd = (CellsToAddMod_t *) CALLOC(1, sizeof(*CellToAdd));
        CellToAdd->cellIndex = i + 1;
        CellToAdd->physCellId = get_adjacent_cell_id(enb_mod_idP, i);
        CellToAdd->cellIndividualOffset = Q_OffsetRange_dB0;

        ASN_SEQUENCE_ADD(&CellsToAddModList->list, CellToAdd);
    }

    ASN_SEQUENCE_ADD(&MeasObj_list->list, MeasObj);
    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->measObjectToAddModList = MeasObj_list;

    // Report Configurations for periodical, A1-A5 events
    ReportConfig_list = CALLOC(1, sizeof(*ReportConfig_list));

    ReportConfig_per = CALLOC(1, sizeof(*ReportConfig_per));

    ReportConfig_A1 = CALLOC(1, sizeof(*ReportConfig_A1));

    ReportConfig_A2 = CALLOC(1, sizeof(*ReportConfig_A2));

    ReportConfig_A3 = CALLOC(1, sizeof(*ReportConfig_A3));

    ReportConfig_A4 = CALLOC(1, sizeof(*ReportConfig_A4));

    ReportConfig_A5 = CALLOC(1, sizeof(*ReportConfig_A5));

    ReportConfig_per->reportConfigId = 1;
    ReportConfig_per->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_periodical;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerType.choice.periodical.purpose =
        ReportConfigEUTRA__triggerType__periodical__purpose_reportStrongestCells;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_per->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_per);

    ReportConfig_A1->reportConfigId = 2;
    ReportConfig_A1->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA1;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.
        a1_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA1.
        a1_Threshold.choice.threshold_RSRP = 10;

    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A1->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A1);

    ReportConfig_A2->reportConfigId = 3;
    ReportConfig_A2->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA2;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.
        a2_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA2.
        a2_Threshold.choice.threshold_RSRP = 10;

    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A2->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A2);

    ReportConfig_A3->reportConfigId = 4;
    ReportConfig_A3->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA3;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.a3_Offset =
        10;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
        eventA3.reportOnLeave = 1;

    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A3->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A3);

    ReportConfig_A4->reportConfigId = 5;
    ReportConfig_A4->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA4;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.
        a4_Threshold.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA4.
        a4_Threshold.choice.threshold_RSRP = 10;

    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A4->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A4);

    ReportConfig_A5->reportConfigId = 6;
    ReportConfig_A5->reportConfig.present = ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.present =
        ReportConfigEUTRA__triggerType_PR_event;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present =
        ReportConfigEUTRA__triggerType__event__eventId_PR_eventA5;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
        eventA5.a5_Threshold1.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
        eventA5.a5_Threshold2.present = ThresholdEUTRA_PR_threshold_RSRP;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
        eventA5.a5_Threshold1.choice.threshold_RSRP = 10;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.
        eventA5.a5_Threshold2.choice.threshold_RSRP = 10;

    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.triggerQuantity = ReportConfigEUTRA__triggerQuantity_rsrp;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportQuantity = ReportConfigEUTRA__reportQuantity_both;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.maxReportCells = 2;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportInterval = ReportInterval_ms120;
    ReportConfig_A5->reportConfig.choice.reportConfigEUTRA.reportAmount = ReportConfigEUTRA__reportAmount_infinity;

    ASN_SEQUENCE_ADD(&ReportConfig_list->list, ReportConfig_A5);

    Sparams = CALLOC(1, sizeof(*Sparams));
    Sparams->present = MeasConfig__speedStatePars_PR_setup;
    Sparams->choice.setup.timeToTrigger_SF.sf_High = SpeedStateScaleFactors__sf_Medium_oDot75;
    Sparams->choice.setup.timeToTrigger_SF.sf_Medium = SpeedStateScaleFactors__sf_High_oDot5;
    Sparams->choice.setup.mobilityStateParameters.n_CellChangeHigh = 10;
    Sparams->choice.setup.mobilityStateParameters.n_CellChangeMedium = 5;
    Sparams->choice.setup.mobilityStateParameters.t_Evaluation = MobilityStateParameters__t_Evaluation_s60;
    Sparams->choice.setup.mobilityStateParameters.t_HystNormal = MobilityStateParameters__t_HystNormal_s120;

    quantityConfig = CALLOC(1, sizeof(*quantityConfig));
    memset((void *)quantityConfig, 0, sizeof(*quantityConfig));
    quantityConfig->quantityConfigEUTRA = CALLOC(1, sizeof(*quantityConfig->quantityConfigEUTRA));
    memset((void *)quantityConfig->quantityConfigEUTRA, 0, sizeof(*quantityConfig->quantityConfigEUTRA));
    quantityConfig->quantityConfigCDMA2000 = NULL;
    quantityConfig->quantityConfigGERAN = NULL;
    quantityConfig->quantityConfigUTRA = NULL;
    quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP =
        CALLOC(1, sizeof(*quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP));
    quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ =
        CALLOC(1, sizeof(*quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ));
    *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRP = FilterCoefficient_fc4;
    *quantityConfig->quantityConfigEUTRA->filterCoefficientRSRQ = FilterCoefficient_fc4;

    /* mobilityinfo  */

    mobilityInfo = CALLOC(1, sizeof(*mobilityInfo));
    memset((void *)mobilityInfo, 0, sizeof(*mobilityInfo));
    mobilityInfo->targetPhysCellId =
        (PhysCellId_t) two_tier_hexagonal_cellIds[rrc_inst->handover_info[ue_mod_idP]->modid_t];
    LOG_D(RRC, "[eNB %d] Frame %d: handover preparation: targetPhysCellId: %d mod_id: %d ue_mod_idP: %d \n", enb_mod_idP,
          frameP, mobilityInfo->targetPhysCellId, enb_mod_idP, ue_mod_idP);

    mobilityInfo->additionalSpectrumEmission = CALLOC(1, sizeof(*mobilityInfo->additionalSpectrumEmission));
    *mobilityInfo->additionalSpectrumEmission = 1;  //Check this value!

    mobilityInfo->t304 = MobilityControlInfo__t304_ms50;    // need to configure an appropriate value here

    // New UE Identity (C-RNTI) to identify an UE uniquely in a cell
    mobilityInfo->newUE_Identity.size = 2;
    mobilityInfo->newUE_Identity.bits_unused = 0;
    mobilityInfo->newUE_Identity.buf = rv;
    mobilityInfo->newUE_Identity.buf[0] = rv[0];
    mobilityInfo->newUE_Identity.buf[1] = rv[1];

    //memset((void *)&mobilityInfo->radioResourceConfigCommon,(void *)&rrc_inst->sib2->radioResourceConfigCommon,sizeof(RadioResourceConfigCommon_t));
    //memset((void *)&mobilityInfo->radioResourceConfigCommon,0,sizeof(RadioResourceConfigCommon_t));

    // Configuring radioResourceConfigCommon
    mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.rach_ConfigCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.rach_ConfigCommon, sizeof(RACH_ConfigCommon_t));
    mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo,
           sizeof(PRACH_ConfigInfo_t));

    mobilityInfo->radioResourceConfigCommon.prach_Config.rootSequenceIndex =
        rrc_inst->sib2->radioResourceConfigCommon.prach_Config.rootSequenceIndex;
    mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.pdsch_ConfigCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.pdsch_ConfigCommon, sizeof(PDSCH_ConfigCommon_t));
    memcpy((void *)&mobilityInfo->radioResourceConfigCommon.pusch_ConfigCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.pusch_ConfigCommon, sizeof(PUSCH_ConfigCommon_t));
    mobilityInfo->radioResourceConfigCommon.phich_Config = NULL;
    mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.pucch_ConfigCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.pucch_ConfigCommon, sizeof(PUCCH_ConfigCommon_t));
    mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.soundingRS_UL_ConfigCommon,
           sizeof(SoundingRS_UL_ConfigCommon_t));
    mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon =
        CALLOC(1, sizeof(*mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon));
    memcpy((void *)mobilityInfo->radioResourceConfigCommon.uplinkPowerControlCommon,
           (void *)&rrc_inst->sib2->radioResourceConfigCommon.uplinkPowerControlCommon,
           sizeof(UplinkPowerControlCommon_t));
    mobilityInfo->radioResourceConfigCommon.antennaInfoCommon = NULL;
    mobilityInfo->radioResourceConfigCommon.p_Max = NULL;   // CALLOC(1,sizeof(*mobilityInfo->radioResourceConfigCommon.p_Max));
    //memcpy((void *)mobilityInfo->radioResourceConfigCommon.p_Max,(void *)rrc_inst->sib1->p_Max,sizeof(P_Max_t));
    mobilityInfo->radioResourceConfigCommon.tdd_Config = NULL;  //CALLOC(1,sizeof(TDD_Config_t));
    //memcpy((void *)mobilityInfo->radioResourceConfigCommon.tdd_Config,(void *)rrc_inst->sib1->tdd_Config,sizeof(TDD_Config_t));
    mobilityInfo->radioResourceConfigCommon.ul_CyclicPrefixLength =
        rrc_inst->sib2->radioResourceConfigCommon.ul_CyclicPrefixLength;
    //End of configuration of radioResourceConfigCommon

    mobilityInfo->carrierFreq = CALLOC(1, sizeof(*mobilityInfo->carrierFreq));  //CALLOC(1,sizeof(CarrierFreqEUTRA_t)); 36090
    mobilityInfo->carrierFreq->dl_CarrierFreq = 36090;
    mobilityInfo->carrierFreq->ul_CarrierFreq = NULL;

    mobilityInfo->carrierBandwidth = CALLOC(1, sizeof(*mobilityInfo->carrierBandwidth));    //CALLOC(1,sizeof(struct CarrierBandwidthEUTRA));  AllowedMeasBandwidth_mbw25
    mobilityInfo->carrierBandwidth->dl_Bandwidth = CarrierBandwidthEUTRA__dl_Bandwidth_n25;
    mobilityInfo->carrierBandwidth->ul_Bandwidth = NULL;
    mobilityInfo->rach_ConfigDedicated = NULL;

    // store the srb and drb list for ho management, mainly in case of failure

    memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.srb_ToAddModList,
           (void *)SRB_configList2, sizeof(SRB_ToAddModList_t));
    memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.drb_ToAddModList,
           (void *)DRB_configList2, sizeof(DRB_ToAddModList_t));
    rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.drb_ToReleaseList = NULL;
    memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.mac_MainConfig,
           (void *)mac_MainConfig, sizeof(MAC_MainConfig_t));
    memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.physicalConfigDedicated,
           (void *)rrc_inst->physicalConfigDedicated[ue_mod_idP], sizeof(PhysicalConfigDedicated_t));
    /*    memcpy((void *)rrc_inst->handover_info[ue_mod_idP]->as_config.sourceRadioResourceConfig.sps_Config,
       (void *)rrc_inst->sps_Config[ue_mod_idP],
       sizeof(SPS_Config_t));
     */
    LOG_I(RRC, "[eNB %d] Frame %d: adding new UE\n");
    //Idx = (ue_mod_idP * NB_RB_MAX) + DCCH;
    Idx = DCCH;
    // SRB1
    eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_idP].Active = 1;
    eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_idP].Srb_info.Srb_id = Idx;
    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_idP].Srb_info.Lchan_desc[0], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_idP].Srb_info.Lchan_desc[1], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);

    // SRB2
    eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_idP].Active = 1;
    eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_idP].Srb_info.Srb_id = Idx;
    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_idP].Srb_info.Lchan_desc[0], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_idP].Srb_info.Lchan_desc[1], &DCCH_LCHAN_DESC, LCHAN_DESC_SIZE);

    LOG_I(RRC, "[eNB %d] CALLING RLC CONFIG SRB1 (rbid %d) for UE %d\n", enb_mod_idP, Idx, ue_mod_idP);

    //      rrc_pdcp_config_req (enb_mod_idP, frameP, 1, CONFIG_ACTION_ADD, idx, UNDEF_SECURITY_MODE);
    //      rrc_rlc_config_req(enb_mod_idP,frameP,1,CONFIG_ACTION_ADD,Idx,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);

    rrc_pdcp_config_asn1_req(enb_mod_idP, ue_mod_idP, frameP, 1,
                             eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP],
                             (DRB_ToAddModList_t *) NULL, (DRB_ToReleaseList_t *) NULL, 0xff, NULL, NULL, NULL
#ifdef Rel10
                             , (PMCH_InfoList_r9_t *) NULL
#endif
        );

    rrc_rlc_config_asn1_req(enb_mod_idP, ue_mod_idP, frameP, 1,
                            eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP],
                            (DRB_ToAddModList_t *) NULL, (DRB_ToReleaseList_t *) NULL
#ifdef Rel10
                            , (PMCH_InfoList_r9_t *) NULL
#endif
        );

    /* Initialize NAS list */
    dedicatedInfoNASList = NULL;

    //  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.measConfig->reportConfigToAddModList = ReportConfig_list;
    memset(buffer, 0, RRC_BUF_SIZE);

    size = do_RRCConnectionReconfiguration(enb_mod_idP, buffer, ue_mod_idP, rrc_eNB_get_next_transaction_identifier(enb_mod_idP),   //Transaction_id,
                                           SRB_configList2, DRB_configList2, NULL,  // DRB2_list,
                                           NULL,    //*sps_Config,
                                           physicalConfigDedicated[ue_mod_idP], MeasObj_list, ReportConfig_list, NULL,    //quantityConfig,
                                           MeasId_list, mac_MainConfig, NULL, mobilityInfo, Sparams,
                                           NULL, NULL, dedicatedInfoNASList
#ifdef Rel10
					   , NULL   // SCellToAddMod_r10_t  
#endif
					   );

    LOG_I(RRC,
          "[eNB %d] Frame %d, Logical Channel DL-DCCH, Generate RRCConnectionReconfiguration for handover (bytes %d, UE id %d)\n",
          enb_mod_idP, frameP, size, ue_mod_idP);
    // to be updated if needed
    /*if (eNB_rrc_inst[enb_mod_idP].SRB1_config[ue_mod_idP]->logicalChannelConfig) {
       if (eNB_rrc_inst[enb_mod_idP].SRB1_config[ue_mod_idP]->logicalChannelConfig->present == SRB_ToAddMod__logicalChannelConfig_PR_explicitValue) {
       SRB1_logicalChannelConfig = &eNB_rrc_inst[enb_mod_idP].SRB1_config[ue_mod_idP]->logicalChannelConfig->choice.explicitValue;
       }
       else {
       SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
       }
       }
       else {
       SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
       }
     */

    LOG_D(RRC,
          "[FRAME %05d][RRC_eNB][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (rrcConnectionReconfiguration_handover to UE %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
          frameP, enb_mod_idP, size, ue_mod_idP, rrc_eNB_mui, enb_mod_idP, DCCH);

    //rrc_rlc_data_req(enb_mod_idP,frameP, 1,(ue_mod_idP*NB_RB_MAX)+DCCH,rrc_eNB_mui++,0,size,(char*)buffer);
    //pdcp_data_req (enb_mod_idP, frameP, 1, (ue_mod_idP * NB_RB_MAX) + DCCH,rrc_eNB_mui++, 0, size, (char *) buffer, 1);
    rrc_mac_config_req(enb_mod_idP, ENB_FLAG_YES, ue_mod_idP, 0,
                       (RadioResourceConfigCommonSIB_t *) NULL,
                       eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP],
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                       (MeasObjectToAddMod_t **) NULL,
                       eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP],
                       1,
                       SRB1_logicalChannelConfig,
                       eNB_rrc_inst[enb_mod_idP].measGapConfig[ue_mod_idP],
                       (TDD_Config_t *) NULL,
                       (MobilityControlInfo_t *) mobilityInfo,
                       (uint8_t *) NULL, (uint16_t *) NULL, NULL, NULL, NULL, (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                       , 0, (MBSFN_AreaInfoList_r9_t *) NULL, (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                       , 0, 0
#endif
        );

    /*
       handoverCommand.criticalExtensions.present = HandoverCommand__criticalExtensions_PR_c1;
       handoverCommand.criticalExtensions.choice.c1.present = HandoverCommand__criticalExtensions__c1_PR_handoverCommand_r8;
       handoverCommand.criticalExtensions.choice.c1.choice.handoverCommand_r8.handoverCommandMessage.buf = buffer;
       handoverCommand.criticalExtensions.choice.c1.choice.handoverCommand_r8.handoverCommandMessage.size = size;
     */

    if (sourceModId != 0xFF) {
        memcpy(eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ueid_s]->buf,
               (void *)buffer, size);
        eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ueid_s]->size = size;
        eNB_rrc_inst[sourceModId].handover_info[eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ueid_s]->ho_complete =
            0xF1;
        //eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ho_complete = 0xFF;
        LOG_D(RRC, "[eNB %d] Frame %d: setting handover complete to 0xF1 for (%d,%d) and to 0xFF for (%d,%d)\n",
              enb_mod_idP, frameP, sourceModId, eNB_rrc_inst[enb_mod_idP].handover_info[ue_mod_idP]->ueid_s, enb_mod_idP, ue_mod_idP);
    } else
        LOG_W(RRC,
              "[eNB %d] Frame %d: rrc_eNB_generate_RRCConnectionReconfiguration_handover: Could not find source eNB mod_id.\n",
              enb_mod_idP, frameP);

}

/*
  void ue_rrc_process_rrcConnectionReconfiguration(uint8_t enb_mod_idP,frame_t frameP,
  RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,
  uint8_t CH_index) {

  if (rrcConnectionReconfiguration->criticalExtensions.present == RRCConnectionReconfiguration__criticalExtensions_PR_c1)
  if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.present == RRCConnectionReconfiguration__criticalExtensions__c1_PR_rrcConnectionReconfiguration_r8) {

  if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated) {
  rrc_ue_process_radioResourceConfigDedicated(enb_mod_idP,frameP,CH_index,
  rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated);

  }

  // check other fields for
  }
  }
*/

/*------------------------------------------------------------------------------*/
void rrc_eNB_process_RRCConnectionReconfigurationComplete(
    module_id_t enb_mod_idP,
    frame_t     frameP,
    module_id_t ue_mod_idP,
    RRCConnectionReconfigurationComplete_r8_IEs_t * rrcConnectionReconfigurationComplete) {
    int                                 i;
#ifdef NAS_NETLINK
    int                                 oip_ifup = 0;
    int                                 dest_ip_offset = 0;
#endif

    uint8_t                            *kRRCenc = NULL;
    uint8_t                            *kRRCint = NULL;
    uint8_t                            *kUPenc = NULL;

    DRB_ToAddModList_t                 *DRB_configList = eNB_rrc_inst[enb_mod_idP].DRB_configList[ue_mod_idP];
    SRB_ToAddModList_t                 *SRB_configList = eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP];

#if defined(ENABLE_SECURITY)
    /* Derive the keys from kenb */
    if (DRB_configList != NULL) {
        derive_key_up_enc(eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP],
                          eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kUPenc);
    }

    derive_key_rrc_enc(eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP],
                       eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kRRCenc);
    derive_key_rrc_int(eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP],
                       eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kRRCint);

#endif
#ifdef ENABLE_RAL
    {
        MessageDef                         *message_ral_p = NULL;
        rrc_ral_connection_reconfiguration_ind_t connection_reconfiguration_ind;
        int                                 i;

        message_ral_p = itti_alloc_new_message(TASK_RRC_ENB, RRC_RAL_CONNECTION_RECONFIGURATION_IND);
        memset(&connection_reconfiguration_ind, 0, sizeof(rrc_ral_connection_reconfiguration_ind_t));
        connection_reconfiguration_ind.ue_id = ue_mod_idP;
        if (DRB_configList != NULL) {
            connection_reconfiguration_ind.num_drb = DRB_configList->list.count;

            for (i = 0; (i < DRB_configList->list.count) && (i < maxDRB); i++) {
                connection_reconfiguration_ind.drb_id[i] = DRB_configList->list.array[i]->drb_Identity;
            }
        } else {
            connection_reconfiguration_ind.num_drb = 0;
        }
        if (SRB_configList != NULL) {
            connection_reconfiguration_ind.num_srb = SRB_configList->list.count;
        } else {
            connection_reconfiguration_ind.num_srb = 0;
        }
        memcpy(&message_ral_p->ittiMsg, (void *)&connection_reconfiguration_ind,
               sizeof(rrc_ral_connection_reconfiguration_ind_t));
        LOG_I(RRC, "Sending RRC_RAL_CONNECTION_RECONFIGURATION_IND to RAL\n");
        itti_send_msg_to_task(TASK_RAL_ENB, enb_mod_idP, message_ral_p);
    }
#endif
    // Refresh SRBs/DRBs
    rrc_pdcp_config_asn1_req(enb_mod_idP, ue_mod_idP, frameP, ENB_FLAG_YES,
			     NULL,  //LG-RK 14/05/2014 SRB_configList,
                             DRB_configList, (DRB_ToReleaseList_t *) NULL,
                             /*eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP] |
                             (eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP] << 4), 
                              */
                             0xff, // already configured during the securitymodecommand
                             kRRCenc,
                             kRRCint,
                             kUPenc
#ifdef Rel10
                             , (PMCH_InfoList_r9_t *) NULL
#endif
        );
    // Refresh SRBs/DRBs
    rrc_rlc_config_asn1_req(enb_mod_idP,
        ue_mod_idP,
        frameP,
        1,
        NULL,  //LG-RK 14/05/2014 SRB_configList,
        DRB_configList,
        (DRB_ToReleaseList_t *) NULL
#ifdef Rel10
                            , (PMCH_InfoList_r9_t *) NULL
#endif
        );

    // Loop through DRBs and establish if necessary

    if (DRB_configList != NULL) {
        for (i = 0; i < DRB_configList->list.count; i++) {  // num max DRB (11-3-8)
            if (DRB_configList->list.array[i]) {
                LOG_I(RRC,
                      "[eNB %d] Frame  %d : Logical Channel UL-DCCH, Received RRCConnectionReconfigurationComplete from UE %d, reconfiguring DRB %d/LCID %d\n",
                      enb_mod_idP, frameP, ue_mod_idP,
                      (int)DRB_configList->list.array[i]->drb_Identity,
                      (ue_mod_idP * NB_RB_MAX) + (int)*DRB_configList->list.array[i]->logicalChannelIdentity);
                if (eNB_rrc_inst[enb_mod_idP].DRB_active[ue_mod_idP][i] == 0) {
                    /*
                       rrc_pdcp_config_req (enb_mod_idP, frameP, 1, CONFIG_ACTION_ADD,
                       (ue_mod_idP * NB_RB_MAX) + *DRB_configList->list.array[i]->logicalChannelIdentity,UNDEF_SECURITY_MODE);
                       rrc_rlc_config_req(enb_mod_idP,frameP,1,CONFIG_ACTION_ADD,
                       (ue_mod_idP * NB_RB_MAX) + (int)*eNB_rrc_inst[enb_mod_idP].DRB_config[ue_mod_idP][i]->logicalChannelIdentity,
                       RADIO_ACCESS_BEARER,Rlc_info_um);
                     */
                    eNB_rrc_inst[enb_mod_idP].DRB_active[ue_mod_idP][i] = 1;

                    LOG_D(RRC,
                          "[eNB %d] Frame %d: Establish RLC UM Bidirectional, DRB %d Active\n",
                          enb_mod_idP, frameP, (int)DRB_configList->list.array[i]->drb_Identity);

#ifdef NAS_NETLINK
                    // can mean also IPV6 since ether -> ipv6 autoconf
#   if !defined(OAI_NW_DRIVER_TYPE_ETHERNET) && !defined(EXMIMO)
                    LOG_I(OIP, "[eNB %d] trying to bring up the OAI interface oai%d\n", enb_mod_idP, enb_mod_idP);
                    oip_ifup = nas_config(
                        enb_mod_idP,   // interface index
                        enb_mod_idP + 1,   // thrid octet
                        enb_mod_idP + 1);  // fourth octet

                    if (oip_ifup == 0) {    // interface is up --> send a config the DRB
#      ifdef OAI_EMU
                        oai_emulation.info.oai_ifup[enb_mod_idP] = 1;
                        dest_ip_offset = NB_eNB_INST;
#      else
                        dest_ip_offset = 8;
#      endif
                        LOG_I(OIP,
                            "[eNB %d] Config the oai%d to send/receive pkt on DRB %d to/from the protocol stack\n",
                            enb_mod_idP, enb_mod_idP,
                            (ue_mod_idP * maxDRB) + DRB_configList->list.array[i]->drb_Identity);
                        rb_conf_ipv4(0, //add
                            ue_mod_idP,  //cx
                            enb_mod_idP,    //inst
                            (ue_mod_idP * maxDRB) + DRB_configList->list.array[i]->drb_Identity, // RB
                            0,    //dscp
                            ipv4_address(enb_mod_idP + 1, enb_mod_idP + 1),  //saddr
                            ipv4_address(enb_mod_idP + 1, dest_ip_offset + ue_mod_idP + 1));  //daddr

                        LOG_D(RRC, "[eNB %d] State = Attached (UE %d)\n", enb_mod_idP, ue_mod_idP);
                    }
#   else
#      ifdef OAI_EMU
                    oai_emulation.info.oai_ifup[enb_mod_idP] = 1;
#      endif
#   endif
#endif

                    LOG_D(RRC,
                          "[FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB][MOD %02d][]\n",
                          frameP, enb_mod_idP, ue_mod_idP, enb_mod_idP);
                    if (DRB_configList->list.array[i]->logicalChannelIdentity)
                        DRB2LCHAN[i] = (uint8_t) * DRB_configList->list.array[i]->logicalChannelIdentity;
                    rrc_mac_config_req(
                        enb_mod_idP,
                        ENB_FLAG_YES,
                        ue_mod_idP,
                        0,
                        (RadioResourceConfigCommonSIB_t *) NULL,
                        eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP],
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                        (MeasObjectToAddMod_t **) NULL,
                        eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP],
                        DRB2LCHAN[i],
                        DRB_configList->list.array[i]->logicalChannelConfig,
                        eNB_rrc_inst[enb_mod_idP].measGapConfig[ue_mod_idP],
                        (TDD_Config_t *) NULL,
                        NULL,
                        (uint8_t *) NULL,
                        (uint16_t *) NULL, NULL, NULL, NULL, (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                                       , 0, (MBSFN_AreaInfoList_r9_t *) NULL, (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                       , eNB_rrc_inst[enb_mod_idP].num_active_cba_groups, eNB_rrc_inst[enb_mod_idP].cba_rnti[0]
#endif
                        );

                } else {        // remove LCHAN from MAC/PHY

                    if (eNB_rrc_inst[enb_mod_idP].DRB_active[ue_mod_idP][i] == 1) {
                        // DRB has just been removed so remove RLC + PDCP for DRB
                        /*      rrc_pdcp_config_req (enb_mod_idP, frameP, 1, CONFIG_ACTION_REMOVE,
                           (ue_mod_idP * NB_RB_MAX) + DRB2LCHAN[i],UNDEF_SECURITY_MODE);
                         */
                        rrc_rlc_config_req(enb_mod_idP, ue_mod_idP, frameP, ENB_FLAG_YES, SRB_FLAG_NO, MBMS_FLAG_NO, CONFIG_ACTION_REMOVE,
                                           DRB2LCHAN[i], Rlc_info_um);
                    }
                    eNB_rrc_inst[enb_mod_idP].DRB_active[ue_mod_idP][i] = 0;
                    LOG_D(RRC,
                          "[FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB][MOD %02d][]\n",
                          frameP, enb_mod_idP, ue_mod_idP, enb_mod_idP);

                    rrc_mac_config_req(enb_mod_idP,
                                       ENB_FLAG_YES,
                                       ue_mod_idP,
                                       0,
                                       (RadioResourceConfigCommonSIB_t *) NULL,
                                       eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP],
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                                       (MeasObjectToAddMod_t **) NULL,
                                       eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP],
                                       DRB2LCHAN[i],
                                       (LogicalChannelConfig_t *) NULL,
                                       (MeasGapConfig_t *) NULL,
                                       (TDD_Config_t *) NULL,
                                       NULL, (uint8_t *) NULL, (uint16_t *) NULL, NULL, NULL, NULL, NULL
#ifdef Rel10
                                       , 0, (MBSFN_AreaInfoList_r9_t *) NULL, (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                       , 0, 0
#endif
                        );
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_generate_RRCConnectionSetup(
    module_id_t  enb_mod_idP,
    frame_t      frameP,
    module_id_t  ue_mod_idP) {

    LogicalChannelConfig_t             *SRB1_logicalChannelConfig;  //,*SRB2_logicalChannelConfig;
    SRB_ToAddModList_t                **SRB_configList;
    SRB_ToAddMod_t                     *SRB1_config;
    int                                 cnt;

    AssertFatal(ue_mod_idP < NUMBER_OF_UE_MAX, "UE index invalid (%d/%d) for eNB %d!", ue_mod_idP, NUMBER_OF_UE_MAX,
                enb_mod_idP);

    SRB_configList = &eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP];

    eNB_rrc_inst[enb_mod_idP].Srb0.Tx_buffer.payload_size =
        do_RRCConnectionSetup(enb_mod_idP,
                              (uint8_t *) eNB_rrc_inst[enb_mod_idP].Srb0.Tx_buffer.Payload,
			      (mac_xface->lte_frame_parms->nb_antennas_tx==2)?2:1,
			      ue_mod_idP,
                              rrc_eNB_get_next_transaction_identifier(enb_mod_idP),
                              mac_xface->lte_frame_parms,
                              SRB_configList, &eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP]);

#ifdef RRC_MSG_PRINT
    LOG_F(RRC,"[MSG] RRC Connection Setup\n");
    for (cnt = 0; cnt < eNB_rrc_inst[enb_mod_idP].Srb0.Tx_buffer.payload_size; cnt++)
      LOG_F(RRC,"%02x ", ((uint8_t*)eNB_rrc_inst[enb_mod_idP].Srb0.Tx_buffer.Payload)[cnt]);
    LOG_F(RRC,"\n");
  //////////////////////////////////
#endif

    // configure SRB1/SRB2, PhysicalConfigDedicated, MAC_MainConfig for UE

    if (*SRB_configList != NULL) {
        for (cnt = 0; cnt < (*SRB_configList)->list.count; cnt++) {
            if ((*SRB_configList)->list.array[cnt]->srb_Identity == 1) {
                SRB1_config = (*SRB_configList)->list.array[cnt];
                if (SRB1_config->logicalChannelConfig) {
                    if (SRB1_config->logicalChannelConfig->present ==
                        SRB_ToAddMod__logicalChannelConfig_PR_explicitValue) {
                        SRB1_logicalChannelConfig = &SRB1_config->logicalChannelConfig->choice.explicitValue;
                    } else {
                        SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
                    }
                } else {
                    SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
                }

                LOG_D(RRC,
                      "[FRAME %05d][RRC_eNB][MOD %02d][][--- MAC_CONFIG_REQ  (SRB1 UE %d) --->][MAC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, ue_mod_idP, enb_mod_idP);
                rrc_mac_config_req(enb_mod_idP, ENB_FLAG_YES, ue_mod_idP, 0,
                                   (RadioResourceConfigCommonSIB_t *) NULL,
                                   eNB_rrc_inst[enb_mod_idP].physicalConfigDedicated[ue_mod_idP],
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
                                   (MeasObjectToAddMod_t **) NULL,
                                   eNB_rrc_inst[enb_mod_idP].mac_MainConfig[ue_mod_idP],
                                   1,
                                   SRB1_logicalChannelConfig,
                                   eNB_rrc_inst[enb_mod_idP].measGapConfig[ue_mod_idP],
                                   (TDD_Config_t *) NULL,
                                   NULL,
                                   (uint8_t *) NULL,
                                   (uint16_t *) NULL, NULL, NULL, NULL, (MBSFN_SubframeConfigList_t *) NULL
#ifdef Rel10
                                   , 0, (MBSFN_AreaInfoList_r9_t *) NULL, (PMCH_InfoList_r9_t *) NULL
#endif
#ifdef CBA
                                   , 0, 0
#endif
                    );
                break;
            }
        }
    }
    LOG_I(RRC,
          "[eNB %d][RAPROC] Frame %d : Logical Channel DL-CCCH, Generating RRCConnectionSetup (bytes %d, UE %d)\n",
          enb_mod_idP, frameP, eNB_rrc_inst[enb_mod_idP].Srb0.Tx_buffer.payload_size, ue_mod_idP);

}

/*------------------------------------------------------------------------------*/
#if defined(ENABLE_ITTI)
char openair_rrc_lite_eNB_init(
    module_id_t enb_mod_idP) {
    /* Dummy function, initialization will be done through ITTI messaging */
    return 0;
}
#endif

#if defined(ENABLE_ITTI)
char openair_rrc_lite_eNB_configuration(
    uint8_t enb_mod_idP,
    RrcConfigurationReq * configuration)
#else
char openair_rrc_lite_eNB_init(
    module_id_t enb_mod_idP)
#endif
{
  /*-----------------------------------------------------------------------------*/
    module_id_t                       j;
    LOG_I(RRC, "[eNB %d] Init (UE State = RRC_IDLE)...\n", enb_mod_idP);

    AssertFatal(eNB_rrc_inst != NULL, "eNB_rrc_inst not initialized!");
    AssertFatal(NUMBER_OF_UE_MAX < (module_id_t)0xFFFFFFFFFFFFFFFF, " variable overflow");

    for (j = 0; j < NUMBER_OF_UE_MAX; j++)
        eNB_rrc_inst[enb_mod_idP].Info.UE[j].Status = RRC_IDLE;  //CH_READY;

#if defined(ENABLE_USE_MME)
    /* Connect eNB to MME */
    if (EPC_MODE_ENABLED <= 0)
#endif
    {
        /* Init security parameters */
        for (j = 0; j < NUMBER_OF_UE_MAX; j++) {
            eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[j] = SecurityAlgorithmConfig__cipheringAlgorithm_eea0;
            eNB_rrc_inst[enb_mod_idP].integrity_algorithm[j] = SecurityAlgorithmConfig__integrityProtAlgorithm_eia2;
            rrc_lite_eNB_init_security(enb_mod_idP, j);
        }
    }

    eNB_rrc_inst[enb_mod_idP].Info.Nb_ue = 0;

    eNB_rrc_inst[enb_mod_idP].Srb0.Active = 0;

    for (j = 0; j < (NUMBER_OF_UE_MAX + 1); j++) {
        eNB_rrc_inst[enb_mod_idP].Srb2[j].Active = 0;
    }

    /// System Information INIT

    LOG_I(RRC, "[eNB %d] Checking release \n", enb_mod_idP);
#ifdef Rel10

    // This has to come from some top-level configuration
    LOG_I(RRC, "[eNB %d] Rel10 RRC detected, MBMS flag %d\n", enb_mod_idP, eNB_rrc_inst[enb_mod_idP].MBMS_flag);

#else
    LOG_I(RRC, "[eNB %d] Rel8 RRC\n", enb_mod_idP);
#endif
#ifdef CBA
    for (j = 0; j < NUM_MAX_CBA_GROUP; j++)
        eNB_rrc_inst[enb_mod_idP].cba_rnti[j] = CBA_OFFSET + j;

    if (eNB_rrc_inst[enb_mod_idP].num_active_cba_groups > NUM_MAX_CBA_GROUP)
        eNB_rrc_inst[enb_mod_idP].num_active_cba_groups = NUM_MAX_CBA_GROUP;

    LOG_D(RRC, "[eNB %d] Initialization of 4 cba_RNTI values (%x %x %x %x) num active groups %d\n",
          enb_mod_idP, eNB_rrc_inst[enb_mod_idP].cba_rnti[0],
          eNB_rrc_inst[enb_mod_idP].cba_rnti[1], eNB_rrc_inst[enb_mod_idP].cba_rnti[2],
          eNB_rrc_inst[enb_mod_idP].cba_rnti[3], eNB_rrc_inst[enb_mod_idP].num_active_cba_groups);
#endif

    init_SI(enb_mod_idP
#if defined(ENABLE_ITTI)
            , configuration
#endif
        );

#ifdef Rel10
    switch (eNB_rrc_inst[enb_mod_idP].MBMS_flag) {
        case 1:
        case 2:
        case 3:
            LOG_I(RRC, "[eNB %d] Configuring 1 MBSFN sync area\n", enb_mod_idP);
            eNB_rrc_inst[enb_mod_idP].num_mbsfn_sync_area = 1;
            break;
        case 4:
            LOG_I(RRC, "[eNB %d] Configuring 2 MBSFN sync area\n", enb_mod_idP);
            eNB_rrc_inst[enb_mod_idP].num_mbsfn_sync_area = 2;
            break;
        default:
            eNB_rrc_inst[enb_mod_idP].num_mbsfn_sync_area = 0;
            break;
    }
    // if we are here the eNB_rrc_inst[enb_mod_idP].MBMS_flag > 0,
    /// MCCH INIT
    if (eNB_rrc_inst[enb_mod_idP].MBMS_flag > 0) {
        init_MCCH(enb_mod_idP);
        /// MTCH data bearer init
        init_MBMS(enb_mod_idP, 0);
    }
#endif

#ifdef NO_RRM                   //init ch SRB0, SRB1 & BDTCH
    openair_rrc_on(enb_mod_idP, 1);
#else
    eNB_rrc_inst[enb_mod_idP].Last_scan_req = 0;
    send_msg(&S_rrc, msg_rrc_phy_synch_to_MR_ind(enb_mod_idP, eNB_rrc_inst[enb_mod_idP].Mac_id));
#endif

    return 0;

}
 
/*------------------------------------------------------------------------------*/
int rrc_eNB_decode_ccch(
    module_id_t enb_mod_idP,
    frame_t frameP,
    SRB_INFO * Srb_info) {
  /*------------------------------------------------------------------------------*/

    module_id_t                         Idx, ue_mod_id;
    asn_dec_rval_t                      dec_rval;
    UL_CCCH_Message_t                  *ul_ccch_msg = NULL;
    RRCConnectionRequest_r8_IEs_t      *rrcConnectionRequest;
    RRCConnectionReestablishmentRequest_r8_IEs_t *rrcConnectionReestablishmentRequest;
    int                                 i, rval;

    //memset(ul_ccch_msg,0,sizeof(UL_CCCH_Message_t));

    LOG_D(RRC, "[eNB %d] Frame %d: Decoding UL CCCH %x.%x.%x.%x.%x.%x (%p)\n",
          enb_mod_idP, frameP, ((uint8_t *) Srb_info->Rx_buffer.Payload)[0],
          ((uint8_t *) Srb_info->Rx_buffer.Payload)[1],
          ((uint8_t *) Srb_info->Rx_buffer.Payload)[2],
          ((uint8_t *) Srb_info->Rx_buffer.Payload)[3],
          ((uint8_t *) Srb_info->Rx_buffer.Payload)[4],
          ((uint8_t *) Srb_info->Rx_buffer.Payload)[5], (uint8_t *) Srb_info->Rx_buffer.Payload);
    dec_rval =
        uper_decode(NULL, &asn_DEF_UL_CCCH_Message, (void **)&ul_ccch_msg,
                    (uint8_t *) Srb_info->Rx_buffer.Payload, 100, 0, 0);

#if defined(ENABLE_ITTI)
#   if defined(DISABLE_ITTI_XER_PRINT)
    {
        MessageDef                         *message_p;

        message_p = itti_alloc_new_message(TASK_RRC_ENB, RRC_UL_CCCH_MESSAGE);
        memcpy(&message_p->ittiMsg, (void *)ul_ccch_msg, sizeof(RrcUlCcchMessage));

        itti_send_msg_to_task(TASK_UNKNOWN, enb_mod_idP, message_p);
    }
#   else
    {
        char                                message_string[10000];
        size_t                              message_string_size;

        if ((message_string_size =
             xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_CCCH_Message, (void *)ul_ccch_msg)) > 0) {
            MessageDef                         *msg_p;

            msg_p = itti_alloc_new_message_sized(TASK_RRC_ENB, RRC_UL_CCCH, message_string_size + sizeof(IttiMsgText));
            msg_p->ittiMsg.rrc_ul_ccch.size = message_string_size;
            memcpy(&msg_p->ittiMsg.rrc_ul_ccch.text, message_string, message_string_size);

            itti_send_msg_to_task(TASK_UNKNOWN, enb_mod_idP, msg_p);
        }
    }
#   endif
#endif

    for (i = 0; i < 8; i++)
        LOG_T(RRC, "%x.", ((uint8_t *) & ul_ccch_msg)[i]);
    if (dec_rval.consumed == 0) {
        LOG_E(RRC, "[eNB %d] FATAL Error in receiving CCCH\n", enb_mod_idP);
        return -1;
    }
    if (ul_ccch_msg->message.present == UL_CCCH_MessageType_PR_c1) {

        switch (ul_ccch_msg->message.choice.c1.present) {

            case UL_CCCH_MessageType__c1_PR_NOTHING:
                LOG_I(RRC, "[eNB %d] Frame %d : Received PR_NOTHING on UL-CCCH-Message\n", enb_mod_idP, frameP);
                break;

            case UL_CCCH_MessageType__c1_PR_rrcConnectionReestablishmentRequest:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Connection Reestablishement Request\n");
	      for (i = 0; i < Srb_info->Rx_buffer.payload_size; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Srb_info->Rx_buffer.Payload)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_D(RRC,
                      "[FRAME %05d][MAC_eNB][MOD %02d][][--- MAC_DATA_IND (rrcConnectionReestablishmentRequest on SRB0) -->][RRC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, enb_mod_idP);

		rrcConnectionReestablishmentRequest = &ul_ccch_msg->message.choice.c1.choice.rrcConnectionReestablishmentRequest.criticalExtensions.choice.rrcConnectionReestablishmentRequest_r8;

                LOG_I(RRC, "[eNB %d] Frame %d UE %d: RRCConnectionReestablishmentRequest cause %s\n", enb_mod_idP,
                      frameP, ue_mod_id,
		      ((rrcConnectionReestablishmentRequest->reestablishmentCause == ReestablishmentCause_otherFailure) ?    "Other Failure" :
		       (rrcConnectionReestablishmentRequest->reestablishmentCause == ReestablishmentCause_handoverFailure) ? "Handover Failure" : 
		                                                                                                            "reconfigurationFailure"));
		/*
		{
		  uint64_t                            c_rnti = 0;
		  
		  memcpy(((uint8_t *) & c_rnti) + 3, rrcConnectionReestablishmentRequest.UE_identity.c_RNTI.buf,
			 rrcConnectionReestablishmentRequest.UE_identity.c_RNTI.size);
		  ue_mod_id = rrc_eNB_get_UE_index(enb_mod_idP, c_rnti);
                }
		
		if ((eNB_rrc_inst[enb_mod_idP].phyCellId == rrcConnectionReestablishmentRequest.UE_identity.physCellId) && 
		    (ue_mod_id != UE_INDEX_INVALID)){
		  rrc_eNB_generate_RRCConnectionReestablishement(enb_mod_idP, frameP, ue_mod_id);
		}else {
		  rrc_eNB_generate_RRCConnectionReestablishementReject(enb_mod_idP, frameP, ue_mod_id);
		}
		*/
                break;
            case UL_CCCH_MessageType__c1_PR_rrcConnectionRequest:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Connection Request\n");
	      for (i = 0; i < Srb_info->Rx_buffer.payload_size; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Srb_info->Rx_buffer.Payload)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_D(RRC,
                      "[FRAME %05d][MAC_eNB][MOD %02d][][--- MAC_DATA_IND  (rrcConnectionRequest on SRB0) -->][RRC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, enb_mod_idP);

	      rrcConnectionRequest = &ul_ccch_msg->message.choice.c1.choice.rrcConnectionRequest.criticalExtensions.choice.rrcConnectionRequest_r8;
                {
                    uint64_t                            random_value = 0;

                    memcpy(((uint8_t *) & random_value) + 3, rrcConnectionRequest->ue_Identity.choice.randomValue.buf,
                           rrcConnectionRequest->ue_Identity.choice.randomValue.size);
                    ue_mod_id = rrc_eNB_get_next_free_UE_index(enb_mod_idP, random_value);
                }

                if (ue_mod_id != UE_MODULE_INVALID) {
#if defined(ENABLE_ITTI)
                    /* Check s-TMSI presence in message */
                    eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_id].Initialue_identity_s_TMSI.presence =
                        (rrcConnectionRequest->ue_Identity.present == InitialUE_Identity_PR_s_TMSI);
                    if (eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_id].Initialue_identity_s_TMSI.presence) {
                        /* Save s-TMSI */
                        S_TMSI_t                            s_TMSI = rrcConnectionRequest->ue_Identity.choice.s_TMSI;

                        eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_id].Initialue_identity_s_TMSI.mme_code =
                            BIT_STRING_to_uint8(&s_TMSI.mmec);
                        eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_id].Initialue_identity_s_TMSI.m_tmsi =
                            BIT_STRING_to_uint32(&s_TMSI.m_TMSI);
                    }
                    eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_id].establishment_cause =
                        rrcConnectionRequest->establishmentCause;
#endif

                    //      memcpy(&Rrc_xface->UE_id[enb_mod_idP][ue_mod_idP],(uint8_t *)rrcConnectionRequest->ue_Identity.choice.randomValue.buf,5);
                    memcpy(&eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_id],
                           (uint8_t *) rrcConnectionRequest->ue_Identity.choice.randomValue.buf, 5);

                    LOG_I(RRC, "[eNB %d] Frame %d : Accept new connection from UE %d (0x%" PRIx64 ")\n",
                          enb_mod_idP, frameP, ue_mod_id, eNB_rrc_inst[enb_mod_idP].Info.UE_list[ue_mod_id]);

                    //CONFIG SRB2  (DCCHs, ONE per User)  //meas && lchan Cfg
                    //eNB_rrc_inst[enb_mod_idP].Info.Dtch_bd_config[ue_mod_idP].Status=NEED_RADIO_CONFIG;
                    //eNB_rrc_inst[enb_mod_idP].Info.Dtch_bd_config[ue_mod_idP].Next_eNBeck_frame=Rrc_xface->Frame_index+1;
                    eNB_rrc_inst[ue_mod_id].Info.Nb_ue++;

#ifndef NO_RRM
                    send_msg(&S_rrc, msg_rrc_MR_attach_ind(enb_mod_idP, Mac_id));
#else

                    //LG COMMENT Idx = (ue_mod_idP * NB_RB_MAX) + DCCH;
                    Idx = DCCH;
                    // SRB1
                    eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_id].Active = 1;
                    eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_id].Srb_info.Srb_id = Idx;
                    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_id].Srb_info.Lchan_desc[0], &DCCH_LCHAN_DESC,
                           LCHAN_DESC_SIZE);
                    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb1[ue_mod_id].Srb_info.Lchan_desc[1], &DCCH_LCHAN_DESC,
                           LCHAN_DESC_SIZE);

                    // SRB2
                    eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_id].Active = 1;
                    eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_id].Srb_info.Srb_id = Idx;
                    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_id].Srb_info.Lchan_desc[0], &DCCH_LCHAN_DESC,
                           LCHAN_DESC_SIZE);
                    memcpy(&eNB_rrc_inst[enb_mod_idP].Srb2[ue_mod_id].Srb_info.Lchan_desc[1], &DCCH_LCHAN_DESC,
                           LCHAN_DESC_SIZE);

                    rrc_eNB_generate_RRCConnectionSetup(enb_mod_idP, frameP, ue_mod_id);
                    //LOG_D(RRC, "[MSC_NBOX][FRAME %05d][RRC_eNB][MOD %02d][][Tx RRCConnectionSetup][RRC_eNB][MOD %02d][]\n",
                    //      frameP, enb_mod_idP, enb_mod_idP);

                    //LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",enb_mod_idP,rlc[enb_mod_idP].m_rlc_am_array[0].allocation);
                    //LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",enb_mod_idP,rlc[enb_mod_idP].m_rlc_am_array[1].allocation);
                    LOG_I(RRC, "[eNB %d] CALLING RLC CONFIG SRB1 (rbid %d) for UE %d\n", enb_mod_idP, Idx, ue_mod_id);

                    //      rrc_pdcp_config_req (enb_mod_idP, frameP, 1, CONFIG_ACTION_ADD, idx, UNDEF_SECURITY_MODE);

                    //      rrc_rlc_config_req(enb_mod_idP,frameP,1,CONFIG_ACTION_ADD,Idx,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);

                    rrc_pdcp_config_asn1_req(enb_mod_idP, ue_mod_id, frameP, 1,
                                             eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_id],
                                             (DRB_ToAddModList_t *) NULL,
                                             (DRB_ToReleaseList_t *) NULL, 0xff, NULL, NULL, NULL
#   ifdef Rel10
                                             , (PMCH_InfoList_r9_t *) NULL
#   endif
                        );

                    rrc_rlc_config_asn1_req(enb_mod_idP, ue_mod_id, frameP, 1,
                                            eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_id],
                                            (DRB_ToAddModList_t *) NULL, (DRB_ToReleaseList_t *) NULL
#   ifdef Rel10
                                            , (PMCH_InfoList_r9_t *) NULL
#   endif
                        );
                    //LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",enb_mod_idP,rlc[enb_mod_idP].m_rlc_am_array[0].allocation);
                    //LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",enb_mod_idP,rlc[enb_mod_idP].m_rlc_am_array[1].allocation);

                    /*

                       LOG_D(RRC,"[eNB %d] CALLING RLC CONFIG SRB2 (rbid %d) for UE %d\n",
                       enb_mod_idP,Idx+1,ue_mod_idP);
                       Mac_rlc_xface->rrc_rlc_config_req(enb_mod_idP,CONFIG_ACTION_ADD,Idx+1,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
                       LOG_D(RRC,"[eNB %d] RLC AM allocation index@0 is %d\n",enb_mod_idP,rlc[enb_mod_idP].m_rlc_am_array[0].allocation);
                       LOG_D(RRC,"[eNB %d] RLC AM allocation index@1 is %d\n",rlc[enb_mod_idP].m_rlc_am_array[1].allocation);
                     */
#endif //NO_RRM
                } else {
                    LOG_E(RRC, "can't add UE, max user count reached!\n");
                }
                break;

            default:
                LOG_E(RRC, "[eNB %d] Frame %d : Unknown message\n", enb_mod_idP, frameP);
                rval = -1;
                break;
        }
        rval = 0;
    } else {
        LOG_E(RRC, "[eNB %d] Frame %d : Unknown error \n", enb_mod_idP, frameP);
        rval = -1;
    }
    return rval;
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_decode_dcch(
    module_id_t enb_mod_idP,
    frame_t     frameP,
    uint8_t     Srb_id,
    module_id_t ue_mod_idP,
    uint8_t    *Rx_sdu,
    sdu_size_t  sdu_sizeP) {
  /*------------------------------------------------------------------------------*/

    asn_dec_rval_t                      dec_rval;
    //UL_DCCH_Message_t uldcchmsg;
    UL_DCCH_Message_t                  *ul_dcch_msg = NULL; //&uldcchmsg;
    UE_EUTRA_Capability_t              *UE_EUTRA_Capability = NULL;
    int i;
    
    if ((Srb_id != 1) && (Srb_id != 2)) {
        LOG_E(RRC, "[eNB %d] Frame %d: Received message on SRB%d, should not have ...\n", enb_mod_idP, frameP, Srb_id);
    }
    //memset(ul_dcch_msg,0,sizeof(UL_DCCH_Message_t));

    LOG_D(RRC, "[eNB %d] Frame %d: Decoding UL-DCCH Message\n", enb_mod_idP, frameP);
    dec_rval = uper_decode(NULL, &asn_DEF_UL_DCCH_Message, (void **)&ul_dcch_msg, Rx_sdu, sdu_sizeP, 0, 0);

#if defined(ENABLE_ITTI)
#   if defined(DISABLE_ITTI_XER_PRINT)
    {
        MessageDef                         *message_p;

        message_p = itti_alloc_new_message(TASK_RRC_ENB, RRC_UL_DCCH_MESSAGE);
        memcpy(&message_p->ittiMsg, (void *)ul_dcch_msg, sizeof(RrcUlDcchMessage));

        itti_send_msg_to_task(TASK_UNKNOWN, enb_mod_idP, message_p);
    }
#   else
    {
        char                                message_string[10000];
        size_t                              message_string_size;

        if ((message_string_size =
             xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_DCCH_Message, (void *)ul_dcch_msg)) >= 0) {
            MessageDef                         *msg_p;

            msg_p = itti_alloc_new_message_sized(TASK_RRC_ENB, RRC_UL_DCCH, message_string_size + sizeof(IttiMsgText));
            msg_p->ittiMsg.rrc_ul_dcch.size = message_string_size;
            memcpy(&msg_p->ittiMsg.rrc_ul_dcch.text, message_string, message_string_size);

            itti_send_msg_to_task(TASK_UNKNOWN, enb_mod_idP, msg_p);
        }
    }
#   endif
#endif

    {  
        for (i = 0; i < sdu_sizeP; i++)
            LOG_T(RRC, "%x.", Rx_sdu[i]);
        LOG_T(RRC, "\n");
    }

    if ((dec_rval.code != RC_OK) && (dec_rval.consumed == 0)) {
        LOG_E(RRC, "[UE %d] Frame %d : Failed to decode UL-DCCH (%d bytes)\n", enb_mod_idP, frameP, dec_rval.consumed);
        return -1;
    }

    if (ul_dcch_msg->message.present == UL_DCCH_MessageType_PR_c1) {

        switch (ul_dcch_msg->message.choice.c1.present) {
            case UL_DCCH_MessageType__c1_PR_NOTHING:   /* No components present */
                break;

            case UL_DCCH_MessageType__c1_PR_csfbParametersRequestCDMA2000:
                break;

            case UL_DCCH_MessageType__c1_PR_measurementReport:
                LOG_D(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND "
                      "%d bytes (measurementReport) --->][RRC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);
                rrc_eNB_process_MeasurementReport(enb_mod_idP, frameP, ue_mod_idP,
                                                  &ul_dcch_msg->message.choice.c1.choice.measurementReport.
                                                  criticalExtensions.choice.c1.choice.measurementReport_r8.measResults);
                break;

            case UL_DCCH_MessageType__c1_PR_rrcConnectionReconfigurationComplete:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Connection Reconfiguration Complete\n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
	      LOG_D(RRC,
		    "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
		    "(RRCConnectionReconfigurationComplete) --->][RRC_eNB][MOD %02d][]\n",
		    frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);

               if (ul_dcch_msg->message.choice.c1.choice.rrcConnectionReconfigurationComplete.criticalExtensions.
                    present ==
                    RRCConnectionReconfigurationComplete__criticalExtensions_PR_rrcConnectionReconfigurationComplete_r8)
                {
                    rrc_eNB_process_RRCConnectionReconfigurationComplete(enb_mod_idP, frameP, ue_mod_idP,
                                                                         &ul_dcch_msg->message.choice.c1.choice.
                                                                         rrcConnectionReconfigurationComplete.
                                                                         criticalExtensions.choice.
                                                                         rrcConnectionReconfigurationComplete_r8);
                    eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_idP].Status = RRC_RECONFIGURED;
                    LOG_I(RRC, "[eNB %d] UE %d State = RRC_RECONFIGURED \n", enb_mod_idP, ue_mod_idP);
                }
#if defined(ENABLE_USE_MME)
#   if defined(ENABLE_ITTI)
                if (EPC_MODE_ENABLED == 1) {
                    rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP(enb_mod_idP, ue_mod_idP);
                }
#   endif
#endif
                break;

            case UL_DCCH_MessageType__c1_PR_rrcConnectionReestablishmentComplete:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Connection Reestablishment Complete\n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
	      LOG_I(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                      "(rrcConnectionReestablishmentComplete) --->][RRC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);
                break;

            case UL_DCCH_MessageType__c1_PR_rrcConnectionSetupComplete:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Connection SetupComplete\n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_D(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                      "(RRCConnectionSetupComplete) --->][RRC_eNB][MOD %02d][]\n",
                      frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);

                if (ul_dcch_msg->message.choice.c1.choice.rrcConnectionSetupComplete.criticalExtensions.present ==
                    RRCConnectionSetupComplete__criticalExtensions_PR_c1) {
                    if (ul_dcch_msg->message.choice.c1.choice.rrcConnectionSetupComplete.criticalExtensions.choice.c1.
                        present ==
                        RRCConnectionSetupComplete__criticalExtensions__c1_PR_rrcConnectionSetupComplete_r8) {
                        rrc_eNB_process_RRCConnectionSetupComplete(enb_mod_idP, frameP, ue_mod_idP,
                                                                   &ul_dcch_msg->message.choice.c1.choice.
                                                                   rrcConnectionSetupComplete.criticalExtensions.choice.
                                                                   c1.choice.rrcConnectionSetupComplete_r8);
                        eNB_rrc_inst[enb_mod_idP].Info.UE[ue_mod_idP].Status = RRC_CONNECTED;
                        LOG_I(RRC, "[eNB %d] UE %d State = RRC_CONNECTED \n", enb_mod_idP, ue_mod_idP);
                        LOG_D(RRC,
                              "[MSC_NBOX][FRAME %05d][RRC_eNB][MOD %02d][][Rx RRCConnectionSetupComplete\n"
                              "Now CONNECTED with UE %d][RRC_eNB][MOD %02d][]\n", frameP, enb_mod_idP, ue_mod_idP, enb_mod_idP);
                    }
                }
                break;

            case UL_DCCH_MessageType__c1_PR_securityModeComplete:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Security Mode Complete\n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_I(RRC,
                      "[eNB %d] Frame %d received securityModeComplete on UL-DCCH %d from UE %d\n",
                      enb_mod_idP, frameP, DCCH, ue_mod_idP);
                LOG_D(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                      "(securityModeComplete) --->][RRC_eNB][MOD %02d][]\n", frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);
#ifdef XER_PRINT
                xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, (void *)ul_dcch_msg);
#endif
                // confirm with PDCP about the security mode for DCCH
                //rrc_pdcp_config_req (enb_mod_idP, frameP, 1,CONFIG_ACTION_SET_SECURITY_MODE, (ue_mod_idP * NB_RB_MAX) + DCCH, 0x77);
                // continue the procedure
                rrc_eNB_generate_UECapabilityEnquiry(enb_mod_idP, frameP, ue_mod_idP);
                break;

            case UL_DCCH_MessageType__c1_PR_securityModeFailure:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC Security Mode Failure\n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_D(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                      "(securityModeFailure) --->][RRC_eNB][MOD %02d][]\n", frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);
#ifdef XER_PRINT
                xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, (void *)ul_dcch_msg);
#endif
                // cancel the security mode in PDCP

                // followup with the remaining procedure
                rrc_eNB_generate_UECapabilityEnquiry(enb_mod_idP, frameP, ue_mod_idP);
                break;

            case UL_DCCH_MessageType__c1_PR_ueCapabilityInformation:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC UECapablility Information \n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif
                LOG_I(RRC,
                      "[eNB %d] Frame %d received ueCapabilityInformation on UL-DCCH %d from UE %d\n",
                      enb_mod_idP, frameP, DCCH, ue_mod_idP);
                LOG_D(RRC,
                      "[FRAME %05d][RLC][MOD %02d][RB %02d][--- RLC_DATA_IND %d bytes "
                      "(UECapabilityInformation) --->][RRC_eNB][MOD %02d][]\n", frameP, enb_mod_idP, DCCH, sdu_sizeP, enb_mod_idP);
#ifdef XER_PRINT
                xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, (void *)ul_dcch_msg);
#endif
                dec_rval = uper_decode(NULL,
                                       &asn_DEF_UE_EUTRA_Capability,
                                       (void **)&UE_EUTRA_Capability,
                                       ul_dcch_msg->message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.
                                       choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList.list.
                                       array[0]->ueCapabilityRAT_Container.buf,
                                       ul_dcch_msg->message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.
                                       choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList.list.
                                       array[0]->ueCapabilityRAT_Container.size, 0, 0);
		//#ifdef XER_PRINT
                xer_fprint(stdout, &asn_DEF_UE_EUTRA_Capability, (void *)UE_EUTRA_Capability);
		//#endif

#if defined(ENABLE_USE_MME)
                if (EPC_MODE_ENABLED == 1) {
                    rrc_eNB_send_S1AP_UE_CAPABILITIES_IND(enb_mod_idP, ue_mod_idP, ul_dcch_msg);
                }
#endif

                rrc_eNB_generate_defaultRRCConnectionReconfiguration(enb_mod_idP, frameP,
                                                                     ue_mod_idP, eNB_rrc_inst[enb_mod_idP].HO_flag);
                break;

            case UL_DCCH_MessageType__c1_PR_ulHandoverPreparationTransfer:
                break;

            case UL_DCCH_MessageType__c1_PR_ulInformationTransfer:
#ifdef RRC_MSG_PRINT
	      LOG_F(RRC,"[MSG] RRC UL Information Transfer \n");
	      for (i = 0; i < sdu_sizeP; i++)
		LOG_F(RRC,"%02x ", ((uint8_t*)Rx_sdu)[i]);
	      LOG_F(RRC,"\n");
#endif

#if defined(ENABLE_USE_MME)
                if (EPC_MODE_ENABLED == 1) {
                    rrc_eNB_send_S1AP_UPLINK_NAS(enb_mod_idP, ue_mod_idP, ul_dcch_msg);
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
                LOG_E(RRC, "[UE %d] Frame %d : Unknown message\n", enb_mod_idP, frameP);
                return -1;
        }
        return 0;
    } else {
        LOG_E(RRC, "[UE %d] Frame %d : Unknown error\n", enb_mod_idP, frameP);
        return -1;
    }

}

#if defined(ENABLE_ITTI)
/*------------------------------------------------------------------------------*/
void                               *rrc_enb_task(
    void *args_p) {
    MessageDef                         *msg_p;
    const char                         *msg_name_p;
    instance_t                          instance;
    unsigned int  ue_mod_id;
    int                                 result;
    SRB_INFO                           *srb_info_p;

    itti_mark_task_ready(TASK_RRC_ENB);

    while (1) {
        // Wait for a message
        itti_receive_msg(TASK_RRC_ENB, &msg_p);

        msg_name_p = ITTI_MSG_NAME(msg_p);
        instance = ITTI_MSG_INSTANCE(msg_p);

        switch (ITTI_MSG_ID(msg_p)) {
            case TERMINATE_MESSAGE:
                itti_exit_task();
                break;

            case MESSAGE_TEST:
                LOG_I(RRC, "[eNB %d] Received %s\n", instance, msg_name_p);
                break;

                /* Messages from MAC */
            case RRC_MAC_CCCH_DATA_IND:
                LOG_I(RRC, "[eNB %d] Received %s: frameP %d,\n", instance, msg_name_p,
                      RRC_MAC_CCCH_DATA_IND(msg_p).frame);

                srb_info_p = &eNB_rrc_inst[instance].Srb0;

                memcpy(srb_info_p->Rx_buffer.Payload, RRC_MAC_CCCH_DATA_IND(msg_p).sdu,
                       RRC_MAC_CCCH_DATA_IND(msg_p).sdu_size);
                srb_info_p->Rx_buffer.payload_size = RRC_MAC_CCCH_DATA_IND(msg_p).sdu_size;
                rrc_eNB_decode_ccch(instance, RRC_MAC_CCCH_DATA_IND(msg_p).frame, srb_info_p);
                break;

                /* Messages from PDCP */
            case RRC_DCCH_DATA_IND:
                LOG_I(RRC, "[eNB %d][UE %d] Received %s: frameP %d, DCCH %d\n", instance,
                      RRC_DCCH_DATA_IND(msg_p).ue_index, msg_name_p, RRC_DCCH_DATA_IND(msg_p).frame,
                      RRC_DCCH_DATA_IND(msg_p).dcch_index);

                rrc_eNB_decode_dcch(instance, RRC_DCCH_DATA_IND(msg_p).frame, RRC_DCCH_DATA_IND(msg_p).dcch_index,
                                    RRC_DCCH_DATA_IND(msg_p).ue_index, RRC_DCCH_DATA_IND(msg_p).sdu_p,
                                    RRC_DCCH_DATA_IND(msg_p).sdu_size);

                // Message buffer has been processed, free it now.
                result = itti_free(ITTI_MSG_ORIGIN_ID(msg_p), RRC_DCCH_DATA_IND(msg_p).sdu_p);
                AssertFatal(result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
                break;

#   if defined(ENABLE_USE_MME)
                /* Messages from S1AP */
            case S1AP_DOWNLINK_NAS:
                rrc_eNB_process_S1AP_DOWNLINK_NAS(msg_p, msg_name_p, instance, &rrc_eNB_mui);
                break;

            case S1AP_INITIAL_CONTEXT_SETUP_REQ:
                rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(msg_p, msg_name_p, instance);
                break;

            case S1AP_UE_CTXT_MODIFICATION_REQ:
                rrc_eNB_process_S1AP_UE_CTXT_MODIFICATION_REQ(msg_p, msg_name_p, instance);
                break;

            case S1AP_PAGING_IND:
                LOG_E(RRC, "[eNB %d] Received not yet implemented message %s\n", instance, msg_name_p);
                break;

            case S1AP_UE_CONTEXT_RELEASE_REQ:
                rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_REQ(msg_p, msg_name_p, instance);
                break;

            case S1AP_UE_CONTEXT_RELEASE_COMMAND:
                rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_COMMAND(msg_p, msg_name_p, instance);
                break;

            case GTPV1U_ENB_CREATE_TUNNEL_RESP:
              rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(msg_p, msg_name_p, instance);
              break;

            case GTPV1U_ENB_DELETE_TUNNEL_RESP:
                LOG_I(RRC, "[eNB %d] Received message %s, not processed because procedure not synched\n",
                    instance, msg_name_p);
              break;

#   endif

                /* Messages from eNB app */
            case RRC_CONFIGURATION_REQ:
                LOG_I(RRC, "[eNB %d] Received %s\n", instance, msg_name_p);
                openair_rrc_lite_eNB_configuration(instance, &RRC_CONFIGURATION_REQ(msg_p));
                break;

#   ifdef ENABLE_RAL
            case RRC_RAL_CONFIGURE_THRESHOLD_REQ:
                rrc_enb_ral_handle_configure_threshold_request(instance, msg_p);
                break;
#   endif

                //SPECTRA: Add the RRC connection reconfiguration with Second cell configuration
            case RRC_RAL_CONNECTION_RECONFIGURATION_REQ:
//                 ue_mod_id = 0; /* TODO force ue_mod_id to first UE, NAS UE not virtualized yet */
                LOG_I(RRC, "[eNB %d] Send RRC_RAL_CONNECTION_RECONFIGURATION_REQ to UE %s\n", instance, msg_name_p);
                //Method RRC connection reconfiguration command with Second cell configuration
#   ifdef ENABLE_RAL
                rrc_eNB_generate_RRCConnectionReconfiguration_SCell(instance, 0/* TODO put frameP number ! */, /*ue_mod_id force ue_mod_id to first UE*/0, 36126);
#   else
                rrc_eNB_generate_defaultRRCConnectionReconfiguration(instance, 0/* TODO put frameP number ! */, /*ue_mod_id force ue_mod_id to first UE*/0, eNB_rrc_inst[instance].HO_flag);
#   endif
                break;

            default:
                LOG_E(RRC, "[eNB %d] Received unexpected message %s\n", instance, msg_name_p);
                break;
        }

        result = itti_free(ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
        AssertFatal(result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
        msg_p = NULL;
    }
}
#endif

#ifndef USER_MODE
EXPORT_SYMBOL(Rlc_info_am_config);
#endif
