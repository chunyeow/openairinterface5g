/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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
/*! \file config.c
* \brief UE and eNB configuration
* \author Raymond Knopp, Navid Nikaein
* \date 2013
* \version 0.1
* \email: navid.nikaein@eurecom.fr
* @ingroup _mac

*/

#include "COMMON/platform_types.h"
#include "COMMON/platform_constants.h"
#include "SystemInformationBlockType2.h"
//#include "RadioResourceConfigCommonSIB.h"
#include "RadioResourceConfigDedicated.h"
#include "MeasGapConfig.h"
#include "MeasObjectToAddModList.h"
#include "TDD-Config.h"
#include "defs.h"
#include "extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#ifdef Rel10
#include "MBSFN-AreaInfoList-r9.h"
#include "MBSFN-AreaInfo-r9.h"
#include "MBSFN-SubframeConfigList.h"
#include "PMCH-InfoList-r9.h"
#endif

/* sec 5.9, 36.321: MAC Reset Procedure */
void ue_mac_reset(u8 Mod_id,u8 eNB_index) {
  
  //Resetting Bj
  UE_mac_inst[Mod_id].scheduling_info.Bj[0] = 0;
  UE_mac_inst[Mod_id].scheduling_info.Bj[1] = 0;
  UE_mac_inst[Mod_id].scheduling_info.Bj[2] = 0;
  //Stopping all timers
  
  //timeAlignmentTimer expires
  
  // PHY changes for UE MAC reset
  mac_xface->phy_reset_ue(Mod_id,eNB_index);
  
  // notify RRC to relase PUCCH/SRS
  // cancel all pending SRs
  UE_mac_inst[Mod_id].scheduling_info.SR_pending=0;
  UE_mac_inst[Mod_id].scheduling_info.SR_COUNTER=0;
  
  // stop ongoing RACH procedure
  
  // discard explicitly signaled ra_PreambleIndex and ra_RACH_MaskIndex, if any
  UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  = 0; // check!
  UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
  
  ue_init_mac(Mod_id); //This will hopefully do the rest of the MAC reset procedure
  
}

int rrc_mac_config_req(u8 Mod_id,u8 eNB_flag,u8 UE_id,u8 eNB_index, 
		       RadioResourceConfigCommonSIB_t *radioResourceConfigCommon,
		       struct PhysicalConfigDedicated *physicalConfigDedicated,
		       MeasObjectToAddMod_t **measObj,
		       MAC_MainConfig_t *mac_MainConfig,
		       long logicalChannelIdentity,
		       LogicalChannelConfig_t *logicalChannelConfig,
		       MeasGapConfig_t *measGapConfig,
		       TDD_Config_t *tdd_Config,
		       MobilityControlInfo_t *mobilityControlInfo,
		       u8 *SIwindowsize,
		       u16 *SIperiod,
		       ARFCN_ValueEUTRA_t *ul_CarrierFreq,
		       long *ul_Bandwidth,
		       AdditionalSpectrumEmission_t *additionalSpectrumEmission,
		       struct MBSFN_SubframeConfigList *mbsfn_SubframeConfigList
#ifdef Rel10
		       ,u8 MBMS_Flag,
		       MBSFN_AreaInfoList_r9_t *mbsfn_AreaInfoList,
		       PMCH_InfoList_r9_t *pmch_InfoList
#endif 
#ifdef CBA
		       ,u8 num_active_cba_groups,
		       u16 cba_rnti
#endif
		       ) {

  int i;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_MAC_CONFIG, VCD_FUNCTION_IN);

  if (eNB_flag==0) {
    LOG_I(MAC,"[CONFIG][UE %d] Configuring MAC/PHY from eNB %d\n",Mod_id,eNB_index);
    if (tdd_Config != NULL)
      UE_mac_inst[Mod_id].tdd_Config = tdd_Config;
  }else {
    if (physicalConfigDedicated == NULL){
      LOG_I(MAC,"[CONFIG][eNB %d] Configuring MAC/PHY\n",Mod_id);
    } else{
      LOG_I(MAC,"[CONFIG][eNB %d] Configuring MAC/PHY for UE %d (%x)\n",Mod_id,UE_id,find_UE_RNTI(Mod_id,UE_id));
    }
  }
  
  if ((tdd_Config!=NULL)||(SIwindowsize!=NULL)||(SIperiod!=NULL)){
    if (eNB_flag==1)
      mac_xface->phy_config_sib1_eNB(Mod_id,tdd_Config,*SIwindowsize,*SIperiod);
    else
      mac_xface->phy_config_sib1_ue(Mod_id,eNB_index,tdd_Config,*SIwindowsize,*SIperiod);
  } 

  if (radioResourceConfigCommon!=NULL) {
    if (eNB_flag==1) {
      LOG_I(MAC,"[CONFIG]SIB2/3 Contents (partial)\n");
      LOG_I(MAC,"[CONFIG]pusch_config_common.n_SB = %ld\n",radioResourceConfigCommon->pusch_ConfigCommon.pusch_ConfigBasic.n_SB);
      LOG_I(MAC,"[CONFIG]pusch_config_common.hoppingMode = %ld\n",radioResourceConfigCommon->pusch_ConfigCommon.pusch_ConfigBasic.hoppingMode);
      LOG_I(MAC,"[CONFIG]pusch_config_common.pusch_HoppingOffset = %ld\n",  radioResourceConfigCommon->pusch_ConfigCommon.pusch_ConfigBasic.pusch_HoppingOffset);
      LOG_I(MAC,"[CONFIG]pusch_config_common.enable64QAM = %d\n",radioResourceConfigCommon->pusch_ConfigCommon.pusch_ConfigBasic.enable64QAM);
      LOG_I(MAC,"[CONFIG]pusch_config_common.groupHoppingEnabled = %d\n",radioResourceConfigCommon->pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupHoppingEnabled);
      LOG_I(MAC,"[CONFIG]pusch_config_common.groupAssignmentPUSCH = %ld\n",radioResourceConfigCommon->pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH);
      LOG_I(MAC,"[CONFIG]pusch_config_common.sequenceHoppingEnabled = %d\n",radioResourceConfigCommon->pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled);
      LOG_I(MAC,"[CONFIG]pusch_config_common.cyclicShift  = %ld\n",radioResourceConfigCommon->pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.cyclicShift); 
      mac_xface->phy_config_sib2_eNB(Mod_id,radioResourceConfigCommon,ul_CarrierFreq,ul_Bandwidth,additionalSpectrumEmission,mbsfn_SubframeConfigList);
    }
    else {
      UE_mac_inst[Mod_id].radioResourceConfigCommon = radioResourceConfigCommon;
      mac_xface->phy_config_sib2_ue(Mod_id,eNB_index,radioResourceConfigCommon,ul_CarrierFreq,ul_Bandwidth,additionalSpectrumEmission,mbsfn_SubframeConfigList);
    }
  }
  // SRB2_lchan_config->choice.explicitValue.ul_SpecificParameters->logicalChannelGroup
  if (logicalChannelConfig!= NULL) {
    if (eNB_flag==0){
      LOG_I(MAC,"[CONFIG][UE %d] Applying RRC logicalChannelConfig from eNB%d\n",Mod_id,eNB_index);
      UE_mac_inst[Mod_id].logicalChannelConfig[logicalChannelIdentity]=logicalChannelConfig;
      UE_mac_inst[Mod_id].scheduling_info.Bj[logicalChannelIdentity]=0; // initilize the bucket for this lcid
      if (logicalChannelConfig->ul_SpecificParameters) {
	UE_mac_inst[Mod_id].scheduling_info.bucket_size[logicalChannelIdentity]=logicalChannelConfig->ul_SpecificParameters->prioritisedBitRate *
	  logicalChannelConfig->ul_SpecificParameters->bucketSizeDuration; // set the max bucket size
	UE_mac_inst[Mod_id].scheduling_info.LCGID[logicalChannelIdentity]=*logicalChannelConfig->ul_SpecificParameters->logicalChannelGroup;
	LOG_D(MAC,"[CONFIG][UE %d] LCID %d is attached to the LCGID %d\n",Mod_id,logicalChannelIdentity,*logicalChannelConfig->ul_SpecificParameters->logicalChannelGroup);
	} else {
	LOG_E(MAC,"[CONFIG][UE %d] LCID %d NULL ul_SpecificParameters\n",Mod_id,logicalChannelIdentity);
	mac_xface->macphy_exit("");
      }
    } 
  }

  if (mac_MainConfig != NULL){
    if (eNB_flag==0){
      LOG_I(MAC,"[CONFIG][UE%d] Applying RRC macMainConfig from eNB%d\n",Mod_id,eNB_index);
      UE_mac_inst[Mod_id].macConfig=mac_MainConfig;
      UE_mac_inst[Mod_id].measGapConfig=measGapConfig;
      if (mac_MainConfig->ul_SCH_Config) {

	if (mac_MainConfig->ul_SCH_Config->periodicBSR_Timer)
	  UE_mac_inst[Mod_id].scheduling_info.periodicBSR_Timer = (u16) *mac_MainConfig->ul_SCH_Config->periodicBSR_Timer;
	else
	  UE_mac_inst[Mod_id].scheduling_info.periodicBSR_Timer = (u16) MAC_MainConfig__ul_SCH_Config__periodicBSR_Timer_infinity;
	
	if (mac_MainConfig->ul_SCH_Config->maxHARQ_Tx)
	  UE_mac_inst[Mod_id].scheduling_info.maxHARQ_Tx     = (u16) *mac_MainConfig->ul_SCH_Config->maxHARQ_Tx;
	else
	  UE_mac_inst[Mod_id].scheduling_info.maxHARQ_Tx     = (u16) MAC_MainConfig__ul_SCH_Config__maxHARQ_Tx_n5;
	if (mac_MainConfig->ul_SCH_Config->retxBSR_Timer)
	  UE_mac_inst[Mod_id].scheduling_info.retxBSR_Timer     = (u16) mac_MainConfig->ul_SCH_Config->retxBSR_Timer;
	else 
	  UE_mac_inst[Mod_id].scheduling_info.retxBSR_Timer     = (u16)MAC_MainConfig__ul_SCH_Config__retxBSR_Timer_sf2560;
      }
#ifdef Rel10   
      if (mac_MainConfig->sr_ProhibitTimer_r9) 
	UE_mac_inst[Mod_id].scheduling_info.sr_ProhibitTimer  = (u16) *mac_MainConfig->sr_ProhibitTimer_r9;
      else
	UE_mac_inst[Mod_id].scheduling_info.sr_ProhibitTimer  = (u16) 0;
#endif
      UE_mac_inst[Mod_id].scheduling_info.periodicBSR_SF  = get_sf_periodicBSRTimer(UE_mac_inst[Mod_id].scheduling_info.periodicBSR_Timer);
      UE_mac_inst[Mod_id].scheduling_info.retxBSR_SF     = get_sf_retxBSRTimer(UE_mac_inst[Mod_id].scheduling_info.retxBSR_Timer);
      
      UE_mac_inst[Mod_id].scheduling_info.drx_config     = mac_MainConfig->drx_Config;
      UE_mac_inst[Mod_id].scheduling_info.phr_config     = mac_MainConfig->phr_Config;
      if (mac_MainConfig->phr_Config){
	UE_mac_inst[Mod_id].PHR_state = mac_MainConfig->phr_Config->present;
	UE_mac_inst[Mod_id].PHR_reconfigured = 1;
	UE_mac_inst[Mod_id].scheduling_info.periodicPHR_Timer = mac_MainConfig->phr_Config->choice.setup.periodicPHR_Timer;
	UE_mac_inst[Mod_id].scheduling_info.prohibitPHR_Timer = mac_MainConfig->phr_Config->choice.setup.prohibitPHR_Timer;
	UE_mac_inst[Mod_id].scheduling_info.PathlossChange = mac_MainConfig->phr_Config->choice.setup.dl_PathlossChange;
      } else {
	UE_mac_inst[Mod_id].PHR_reconfigured = 0;
	UE_mac_inst[Mod_id].PHR_state = MAC_MainConfig__phr_Config_PR_setup;
	UE_mac_inst[Mod_id].scheduling_info.periodicPHR_Timer = MAC_MainConfig__phr_Config__setup__periodicPHR_Timer_sf20;
	UE_mac_inst[Mod_id].scheduling_info.prohibitPHR_Timer = MAC_MainConfig__phr_Config__setup__prohibitPHR_Timer_sf20;
	UE_mac_inst[Mod_id].scheduling_info.PathlossChange = MAC_MainConfig__phr_Config__setup__dl_PathlossChange_dB1;
      }	
      UE_mac_inst[Mod_id].scheduling_info.periodicPHR_SF =  get_sf_perioidicPHR_Timer(UE_mac_inst[Mod_id].scheduling_info.periodicPHR_Timer);
      UE_mac_inst[Mod_id].scheduling_info.prohibitPHR_SF =  get_sf_prohibitPHR_Timer(UE_mac_inst[Mod_id].scheduling_info.prohibitPHR_Timer);
      UE_mac_inst[Mod_id].scheduling_info.PathlossChange_db =  get_db_dl_PathlossChange(UE_mac_inst[Mod_id].scheduling_info.PathlossChange);
      LOG_D(MAC,"[UE %d] config PHR (%d): periodic %d (SF) prohibit %d (SF)  pathlosschange %d (db) \n",
	    Mod_id,mac_MainConfig->phr_Config->present, 
	    UE_mac_inst[Mod_id].scheduling_info.periodicPHR_SF,
	    UE_mac_inst[Mod_id].scheduling_info.prohibitPHR_SF,
	    UE_mac_inst[Mod_id].scheduling_info.PathlossChange_db);
    }
  }

  if (physicalConfigDedicated != NULL) {
    if (eNB_flag==1){
      mac_xface->phy_config_dedicated_eNB(Mod_id,find_UE_RNTI(Mod_id,UE_id),physicalConfigDedicated);
    }else{
      mac_xface->phy_config_dedicated_ue(Mod_id,eNB_index,physicalConfigDedicated);
      UE_mac_inst[Mod_id].physicalConfigDedicated=physicalConfigDedicated; // for SR proc
    }
  }

  if (eNB_flag == 0) {
    if (measObj!= NULL) {
      if (measObj[0]!= NULL){
	UE_mac_inst[Mod_id].n_adj_cells = measObj[0]->measObject.choice.measObjectEUTRA.cellsToAddModList->list.count;
	LOG_I(MAC,"Number of adjacent cells %d\n",UE_mac_inst[Mod_id].n_adj_cells);
	for (i=0;i<UE_mac_inst[Mod_id].n_adj_cells;i++) {
	  UE_mac_inst[Mod_id].adj_cell_id[i] = measObj[0]->measObject.choice.measObjectEUTRA.cellsToAddModList->list.array[i]->physCellId;
	  LOG_I(MAC,"Cell %d : Nid_cell %d\n",i,UE_mac_inst[Mod_id].adj_cell_id[i]);
	}
	mac_xface->phy_config_meas_ue(Mod_id,eNB_index,UE_mac_inst[Mod_id].n_adj_cells,UE_mac_inst[Mod_id].adj_cell_id);
      }
    /*
    if (quantityConfig != NULL) {
    	if (quantityConfig[0] != NULL) {
    		UE_mac_inst[Mod_id].quantityConfig = quantityConfig[0];
    		LOG_I(MAC,"UE %d configured filterCoeff.",UE_mac_inst[Mod_id].crnti);
    		mac_xface->phy_config_meas_ue
    	}
    }
    */
    }
  }
  if (eNB_flag==0) {
    if(mobilityControlInfo != NULL) {
      
      LOG_D(MAC,"[UE%d] MAC Reset procedure triggered by RRC eNB %d \n",Mod_id,eNB_index);
      ue_mac_reset(Mod_id,eNB_index);
      
      if(mobilityControlInfo->radioResourceConfigCommon.rach_ConfigCommon) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->rach_ConfigCommon, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.rach_ConfigCommon,
	       sizeof(RACH_ConfigCommon_t));
      }
      
      memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->prach_Config.prach_ConfigInfo, 
	     (void *)mobilityControlInfo->radioResourceConfigCommon.prach_Config.prach_ConfigInfo,
	     sizeof(PRACH_ConfigInfo_t));
      UE_mac_inst[Mod_id].radioResourceConfigCommon->prach_Config.rootSequenceIndex = mobilityControlInfo->radioResourceConfigCommon.prach_Config.rootSequenceIndex;
      
      if(mobilityControlInfo->radioResourceConfigCommon.pdsch_ConfigCommon) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->pdsch_ConfigCommon, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.pdsch_ConfigCommon,
	       sizeof(PDSCH_ConfigCommon_t));
      }
      // not a pointer: mobilityControlInfo->radioResourceConfigCommon.pusch_ConfigCommon
      memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->pusch_ConfigCommon, 
	     (void *)&mobilityControlInfo->radioResourceConfigCommon.pusch_ConfigCommon,
	     sizeof(PUSCH_ConfigCommon_t));
      
      if(mobilityControlInfo->radioResourceConfigCommon.phich_Config) {
	/* memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->phich_Config, 
	   (void *)mobilityControlInfo->radioResourceConfigCommon.phich_Config,
	   sizeof(PHICH_Config_t)); */
      }
      if(mobilityControlInfo->radioResourceConfigCommon.pucch_ConfigCommon) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->pucch_ConfigCommon, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.pucch_ConfigCommon,
	       sizeof(PUCCH_ConfigCommon_t));
      }
      if(mobilityControlInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->soundingRS_UL_ConfigCommon, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.soundingRS_UL_ConfigCommon,
	       sizeof(SoundingRS_UL_ConfigCommon_t));
	}
      if(mobilityControlInfo->radioResourceConfigCommon.uplinkPowerControlCommon) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->uplinkPowerControlCommon, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.uplinkPowerControlCommon,
	       sizeof(UplinkPowerControlCommon_t));
      }
      //configure antennaInfoCommon somewhere here..
      if(mobilityControlInfo->radioResourceConfigCommon.p_Max) {
	  //to be configured
      }
      if(mobilityControlInfo->radioResourceConfigCommon.tdd_Config) {
	UE_mac_inst[Mod_id].tdd_Config = mobilityControlInfo->radioResourceConfigCommon.tdd_Config;
      }
      if(mobilityControlInfo->radioResourceConfigCommon.ul_CyclicPrefixLength) {
	memcpy((void *)&UE_mac_inst[Mod_id].radioResourceConfigCommon->ul_CyclicPrefixLength, 
	       (void *)mobilityControlInfo->radioResourceConfigCommon.ul_CyclicPrefixLength,
	       sizeof(UL_CyclicPrefixLength_t));
      }
      
      UE_mac_inst[Mod_id].crnti = ((mobilityControlInfo->newUE_Identity.buf[0])|(mobilityControlInfo->newUE_Identity.buf[1]<<8));
      LOG_I(MAC,"[UE %d] Received new identity %x from %d\n", Mod_id, UE_mac_inst[Mod_id].crnti, eNB_index);
      UE_mac_inst[Mod_id].rach_ConfigDedicated = malloc(sizeof(*mobilityControlInfo->rach_ConfigDedicated));
      if (mobilityControlInfo->rach_ConfigDedicated){
	memcpy((void*)UE_mac_inst[Mod_id].rach_ConfigDedicated,
	       (void*)mobilityControlInfo->rach_ConfigDedicated,
	       sizeof(*mobilityControlInfo->rach_ConfigDedicated));
      }
      mac_xface->phy_config_afterHO_ue(Mod_id,eNB_index,mobilityControlInfo,0);
    }
  }
  
  if (mbsfn_SubframeConfigList != NULL) {
    if (eNB_flag == 1) {
      LOG_I(MAC,"[eNB %d][CONFIG] Received %d subframe allocation pattern for MBSFN\n", Mod_id, mbsfn_SubframeConfigList->list.count);
      eNB_mac_inst[Mod_id].num_sf_allocation_pattern= mbsfn_SubframeConfigList->list.count; 
      for (i=0; i<mbsfn_SubframeConfigList->list.count; i++) {
	eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[i] = mbsfn_SubframeConfigList->list.array[i];
	LOG_I(MAC, "[eNB %d][CONFIG] MBSFN_SubframeConfig[%d] pattern is  %x\n", Mod_id, i, 
	      eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[i]->subframeAllocation.choice.oneFrame.buf[0]); 
      }
#ifdef Rel10
      eNB_mac_inst[Mod_id].MBMS_flag = MBMS_Flag;
#endif
    }
    else { // UE
      LOG_I(MAC,"[UE %d][CONFIG] Received %d subframe allocation pattern for MBSFN\n", Mod_id, mbsfn_SubframeConfigList->list.count);
      UE_mac_inst[Mod_id].num_sf_allocation_pattern= mbsfn_SubframeConfigList->list.count; 
      for (i=0; i<mbsfn_SubframeConfigList->list.count; i++) {
	LOG_I(MAC, "[UE %d] Configuring MBSFN_SubframeConfig %d from received SIB2 \n", Mod_id, i); 
	UE_mac_inst[Mod_id].mbsfn_SubframeConfig[i] = mbsfn_SubframeConfigList->list.array[i];
	//	LOG_I("[UE %d] MBSFN_SubframeConfig[%d] pattern is  %ld\n", Mod_id, 
	//    UE_mac_inst[Mod_id].mbsfn_SubframeConfig[i]->subframeAllocation.choice.oneFrame.buf[0]); 
      }
    }
  }

#ifdef Rel10
  if (mbsfn_AreaInfoList != NULL) {
    if (eNB_flag == 1) {
      // One eNB could be part of multiple mbsfn syc area, this could change over time so reset each time
      LOG_I(MAC,"[eNB %d][CONFIG] Received %d MBSFN Area Info\n", Mod_id, mbsfn_AreaInfoList->list.count);
      eNB_mac_inst[Mod_id].num_active_mbsfn_area = mbsfn_AreaInfoList->list.count; 
      for (i =0; i< mbsfn_AreaInfoList->list.count; i++) {
	eNB_mac_inst[Mod_id].mbsfn_AreaInfo[i] = mbsfn_AreaInfoList->list.array[i];
	LOG_I(MAC,"[eNB %d][CONFIG] MBSFN_AreaInfo[%d]: MCCH Repetition Period = %ld\n", Mod_id,i,  
	      eNB_mac_inst[Mod_id].mbsfn_AreaInfo[i]->mcch_Config_r9.mcch_RepetitionPeriod_r9); 
	mac_xface->phy_config_sib13_eNB(Mod_id,i,eNB_mac_inst[Mod_id].mbsfn_AreaInfo[i]->mbsfn_AreaId_r9);
      }
    }
    else {  // UE
      LOG_I(MAC,"[UE %d][CONFIG] Received %d MBSFN Area Info\n", Mod_id, mbsfn_AreaInfoList->list.count);
      UE_mac_inst[Mod_id].num_active_mbsfn_area = mbsfn_AreaInfoList->list.count; 
      for (i =0; i< mbsfn_AreaInfoList->list.count; i++) {
	UE_mac_inst[Mod_id].mbsfn_AreaInfo[i] = mbsfn_AreaInfoList->list.array[i];
	LOG_I(MAC,"[UE %d] MBSFN_AreaInfo[%d]: MCCH Repetition Period = %ld\n",Mod_id, i, 
	      UE_mac_inst[Mod_id].mbsfn_AreaInfo[i]->mcch_Config_r9.mcch_RepetitionPeriod_r9); 
	mac_xface->phy_config_sib13_ue(Mod_id,eNB_index,i,UE_mac_inst[Mod_id].mbsfn_AreaInfo[i]->mbsfn_AreaId_r9);
      }
    }
  }

  
  if (pmch_InfoList != NULL) {

    //    LOG_I(MAC,"DUY: lcid when entering rrc_mac config_req is %02d\n",(pmch_InfoList->list.array[0]->mbms_SessionInfoList_r9.list.array[0]->logicalChannelIdentity_r9));

    if (eNB_flag == 1) {

      LOG_I(MAC, "[CONFIG] Number of PMCH in this MBSFN Area %d\n", pmch_InfoList->list.count);

      for (i =0; i< pmch_InfoList->list.count; i++) {
	eNB_mac_inst[Mod_id].pmch_Config[i] = &pmch_InfoList->list.array[i]->pmch_Config_r9;

	LOG_I(MAC, "[CONFIG] PMCH[%d]: This PMCH stop at subframe  %ldth\n", i, 
	      eNB_mac_inst[Mod_id].pmch_Config[i]->sf_AllocEnd_r9); 
	LOG_I(MAC, "[CONFIG] PMCH[%d]: mch_Scheduling_Period = %ld\n", i, 
	      eNB_mac_inst[Mod_id].pmch_Config[i]->mch_SchedulingPeriod_r9); 
	LOG_I(MAC, "[CONFIG] PMCH[%d]: dataMCS = %ld\n", i, 
	      eNB_mac_inst[Mod_id].pmch_Config[i]->dataMCS_r9); 

	// MBMS session info list in each MCH
	eNB_mac_inst[Mod_id].mbms_SessionList[i] = &pmch_InfoList->list.array[i]->mbms_SessionInfoList_r9;
	LOG_I(MAC, "PMCH[%d] Number of session (MTCH) is: %d\n",i, eNB_mac_inst[Mod_id].mbms_SessionList[i]->list.count);
      }
    }
    else { // UE  
      LOG_I(MAC, "[UE %d] Configuring PMCH_config from MCCH MESSAGE \n",Mod_id);
      for (i =0; i< pmch_InfoList->list.count; i++) {
	UE_mac_inst[Mod_id].pmch_Config[i] = &pmch_InfoList->list.array[i]->pmch_Config_r9;
	LOG_I(MAC, "[UE %d] PMCH[%d]: MCH_Scheduling_Period = %ld\n", Mod_id, i,
	      UE_mac_inst[Mod_id].pmch_Config[i]->mch_SchedulingPeriod_r9); 
      }
      UE_mac_inst[Mod_id].mcch_status = 1;
    }
  }
 
#endif
#ifdef CBA
  if (eNB_flag == 0){
    if (cba_rnti) {
      UE_mac_inst[Mod_id].cba_rnti[num_active_cba_groups-1] = cba_rnti;
      LOG_D(MAC,"[UE %d] configure CBA group %d RNTI %x for eNB %d (total active cba group %d)\n", 
	    Mod_id,Mod_id%num_active_cba_groups, cba_rnti,eNB_index,num_active_cba_groups);
      mac_xface->phy_config_cba_rnti(Mod_id,eNB_flag,eNB_index,cba_rnti,num_active_cba_groups-1, num_active_cba_groups);
    }
  }else {
    if (cba_rnti) {
      LOG_D(MAC,"[eNB %d] configure CBA RNTI for UE  %d (total active cba groups %d)\n", 
	      Mod_id, UE_id, num_active_cba_groups);
      eNB_mac_inst[Mod_id].num_active_cba_groups=num_active_cba_groups;
      for (i=0; i < num_active_cba_groups; i ++){
	if (eNB_mac_inst[Mod_id].cba_rnti[i] != cba_rnti + i)
	  eNB_mac_inst[Mod_id].cba_rnti[i] = cba_rnti + i;
	  //only configure UE ids up to num_active_cba_groups 
	  //we use them as candidates for the transmission of dci format0)
	if (UE_id%num_active_cba_groups == i){
	  mac_xface->phy_config_cba_rnti(Mod_id,eNB_flag,UE_id,cba_rnti + i,i,num_active_cba_groups );
	  LOG_D(MAC,"[eNB %d] configure CBA groups %d with RNTI %x for UE  %d (total active cba groups %d)\n", 
		Mod_id, i, eNB_mac_inst[Mod_id].cba_rnti[i],UE_id, num_active_cba_groups);
	}
      }
    }
  }

#endif
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_MAC_CONFIG, VCD_FUNCTION_OUT);

  return(0);
}
