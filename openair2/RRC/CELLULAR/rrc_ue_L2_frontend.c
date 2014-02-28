/***************************************************************************
                          rrc_ue_L2_frontend.c - description
                          -------------------
    begin               : July 2010
    copyright           : (C) 2005, 2010 by Eurecom
    created by		: Michelle.Wetterwald@eurecom.fr 
 **************************************************************************
      This file contain front end functions to isolate L2 interface
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"
// TEMP DRB
//#include "DL-DCCH-Message.h"


/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_proto_intf.h"

extern rlc_info_t Rlc_info_um;
extern rlc_info_t Rlc_info_am_config;


//-----------------------------------------------------------------------------
/*  global variables copied from RRC LITE for logical channels */
long logicalChannelGroup0 = 0;
long  logicalChannelSR_Mask_r9=0;

struct LogicalChannelConfig__ul_SpecificParameters LCSRB1 =  {1,
							      LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
							      0,
							      &logicalChannelGroup0};

struct LogicalChannelConfig__ul_SpecificParameters LCSRB2 =  {3,
							      LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
							      0,
							      &logicalChannelGroup0};

// These are the default SRB configurations from 36.331 (Chapter 9, p. 176-179 in v8.6)

LogicalChannelConfig_t  SRB1_logicalChannelConfig_defaultValue = {&LCSRB1
#ifdef Rel10
								  , &logicalChannelSR_Mask_r9
#endif
                                                                 };

LogicalChannelConfig_t SRB2_logicalChannelConfig_defaultValue = {&LCSRB2
#ifdef Rel10
								 , &logicalChannelSR_Mask_r9
#endif
                                                                 };
/*  END - global variables copied from RRC LITE for logical channels */
//-----------------------------------------------------------------------------

// Configuration Functions for L1-L2 layers
//-----------------------------------------------------------------------------
void crb_config_req (int activation_timeP){
//-----------------------------------------------------------------------------
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to crb_config_req\n");
  #endif
}
//-----------------------------------------------------------------------------
void cmac_config_req (int userP, int activation_timeP){
//-----------------------------------------------------------------------------
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to cmac_config_req\n");
  #endif
}
//-----------------------------------------------------------------------------
void CPHY_config_req (void *config, int activation_time, int userP){
//-----------------------------------------------------------------------------
  #ifndef BYPASS_L1
  #ifdef RRC_DEBUG_DUMMIES
  msg ("\n[RRC][RG-DUMMIES] DUMMY CALL to CPHY_config_req\n");
  #endif
  #endif
}

// Functions for data transmission
//-------------------------------------------------------------------
int rrc_ue_test_rlc_intf_xmit_dcch (void){
//-----------------------------------------------------------------------------
  //char *tx_dcch_info = {"This is a sample data to test the interface with the RLC module. Check segmentation and transmission"};
  char *tx_dcch_info = {"DYNAMIC ACCESS REQUEST"};
  int data_length = 0;
  int retcode;

  #ifdef DEBUG_RRC_TEMP_OPENAIR
  msg ("\n[RRC][UE-DUMMIES] TEMP - CALL to rrc_ue_test_rlc_intf_xmit_dcch\n");
  #endif
  data_length=strlen(tx_dcch_info);
    //retcode = rb_tx_data_srb_rg (RRC_SRB2_ID + (msgId * maxRB), tx_dcch_info, data_length * 8, protocol_bs->rrc.next_MUI++, TRUE);

//  Mac_rlc_xface->rrc_rlc_data_req (Mod_id+NB_CH_INST, UE_rrc_inst[Mod_id].Srb2[Idx2].Srb_info.Srb_id, mui++, 0, W_IDX, UE_rrc_inst[Mod_id].Srb2[Idx2].Srb_info.Tx_buffer.Payload);
  retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LTE_DCCH_ID, protocol_ms->rrc.next_MUI++, 0, data_length, tx_dcch_info);
  #ifdef DEBUG_RRC_TEMP_OPENAIR
  msg ("[RRC][UE-DUMMIES] rrc_ue_test_rlc_intf_xmit_dcch -- retcode = %d\n",retcode);  //RC = 1 ==> OK
  #endif
  return 0;
}

//-------------------------------------------------------------------
int rrc_ue_force_uplink (void){
//-----------------------------------------------------------------------------
  //TEST ACCESS communication
  if ((Mac_rlc_xface->frame%5) == 2){
      rrc_ue_test_rlc_intf_xmit_dcch ();
  }
  return 0;
}

//-----------------------------------------------------------------------------
void rrc_remove_UE(u8 Mod_id,u8 UE_id) {
//-----------------------------------------------------------------------------
  msg ("\n[RRC-UE]call to rrc_remove_UE [empty - to be implemented in CELLULAR mode]\n");
}

// Unified function to send data 
//-----------------------------------------------------------------------------
int rrc_ue_send_to_srb_rlc (int rb_id, char * data_buffer, int data_length){
//-----------------------------------------------------------------------------
  char tx_data[500];
  int stxtlen = 0;
  int result =0;
  //int Mod_id = protocol_ms->rrc.mod_id;
  int Mod_id = rrc_ue_mobileId;

  int eNB_flag = 0; //1=eNB, 0=UE
  int srb1 =1;
  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] Send Data to PDCP-RLC, srb %d at frame %d\n",rb_id, protocol_ms->rrc.current_SFN );
  rrc_print_buffer (data_buffer, data_length);
  #endif

  // OpenAirInterface, as of 02/01/2013, requires passing all the RRC CELL srb through the DCCH
  // Multiplexing is performed by adding the srb_id as first byte of data buffer
  memset(tx_data,0,500);
  tx_data[0] = rb_id;
  memcpy ((char*)&tx_data[1],data_buffer, data_length);
  data_length = data_length +1;

  switch (rb_id){
    case RRC_SRB0_ID: //CCCH
    case RRC_SRB1_ID: //DCCH-UM
      //result = rrc_rlc_data_req(Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, srb1, protocol_ms->rrc.next_MUI++, RRC_RLC_CONFIRM_NO, data_length, tx_data);
      if (pdcp_data_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN, eNB_flag,srb1,protocol_ms->rrc.next_MUI++,RRC_RLC_CONFIRM_NO,data_length,tx_data,PDCP_MODE_DATA_PDU))
        result = 1;
      break;
    case RRC_SRB2_ID: //DCCH-AM
    case RRC_SRB3_ID: //DCCH-AM - NAS
      //result = rrc_rlc_data_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN, eNB_flag, srb1,protocol_ms->rrc.next_MUI++, RRC_RLC_CONFIRM_YES, data_length, tx_data);
      protocol_ms->rrc.rrc_ue_ackSimu_mui = protocol_ms->rrc.next_MUI;
      if (pdcp_data_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN, eNB_flag,srb1,protocol_ms->rrc.next_MUI++,RRC_RLC_CONFIRM_YES,data_length,tx_data,1)){
        result = 1;
        protocol_ms->rrc.rrc_ue_ackSimu_flag = 1;
        protocol_ms->rrc.rrc_ue_ackSimu_srbid = rb_id;
      }
      break;
    default:
       msg ("\n[RRC-UE-FRONTEND] ERROR - Unable to send data to RLC, Channel srb %d not supported\n",rb_id);
  }
  if (result !=1)
       msg ("\n[RRC-UE-FRONTEND] ERROR - RLC returned an error code %d\n", result);

  return result;

}

//-----------------------------------------------------------------------------
int rrc_ue_receive_from_srb_rlc (char* sduP, u8 ch_idP, unsigned int Sdu_size){
//-----------------------------------------------------------------------------
  int srb_id, rb_id;
  int UE_Id;
  int sdu_offset=0;

  // get UE_Id
  rb_id = ch_idP;
  srb_id = rb_id % NB_RB_MAX;
  UE_Id = (rb_id - srb_id) / NB_RB_MAX;
  // get RRC_CELL srb_id
  srb_id = sduP[0];

  #ifdef DEBUG_RRC_DETAILS
  #ifndef DEBUG_RRC_MBMS
  if (srb_id != RRC_MCCH_ID)
  #endif
    msg ("\n[RRC-UE-FRONTEND] Begin rrc_ue_receive_from_srb_rlc, lchannel %d, srb %d for UE %d at frame %d\n", ch_idP, srb_id, UE_Id, protocol_ms->rrc.current_SFN);
    //rrc_print_buffer ((char*)&sduP[0], Sdu_size);
  #endif

  switch (srb_id){
    case RRC_SRB0_ID: //CCCH
    case RRC_SRB1_ID: //DCCH-UM
    case RRC_SRB2_ID: //DCCH-AM
    case RRC_SRB3_ID: //DCCH-AM - NAS
    case RRC_MCCH_ID: //MCCH
      rrc_ue_srb_rx ((char*)&sduP[1], srb_id, UE_Id);
      break;
    default:
       msg ("\n[RRC-UE-FRONTEND] Invalid Channel srb  number %d\n",srb_id);
  }

  return 0;
}

//-----------------------------------------------------------------------------
void rrc_ue_simu_receive_ack_from_rlc (void){
//-----------------------------------------------------------------------------
  unsigned char Mod_id =protocol_ms->rrc.mod_id;

  rrc_L2_rlc_confirm_ind_rx (Mod_id, protocol_ms->rrc.rrc_ue_ackSimu_srbid, protocol_ms->rrc.rrc_ue_ackSimu_mui);
  protocol_ms->rrc.rrc_ue_ackSimu_mui = 0;
  protocol_ms->rrc.rrc_ue_ackSimu_flag = 0;
  protocol_ms->rrc.rrc_ue_ackSimu_srbid = 0;
}

/*****************************************
 Configuration Functions for L1-L2 layers
 *****************************************/
//-----------------------------------------------------------------------------
void  rrc_ue_L2_setupFachRach(void){
//-----------------------------------------------------------------------------
  rrc_ue_config_common_channels ();

  rrc_ue_xmit_ccch();
}

//-----------------------------------------------------------------------------
void rrc_ue_config_common_channels (void){
//-----------------------------------------------------------------------------
  //int Mod_id = protocol_ms->rrc.mod_id;
  int Mod_id = rrc_ue_mobileId;
  int eNB_flag=0;
  int UE_index=rrc_ue_mobileId;
  int eNB_index=0;
//  int eNB_index = 0;
//  int Mod_id = 0;

  u8 result;
  u8 SIwindowsize=1;
  u16 SIperiod=8;

// Simulate decoding SIB1
  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_common_channels \n");
  #endif

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] do_SIB1_TDD_config_cell\n");
  #endif
  // Initialize TDD_config parameters (TDD_Config_t tdd_Config;) calling asn1_msg.c
  result = do_SIB1_TDD_config_cell (mac_xface->lte_frame_parms, &protocol_ms->rrc.ue_bch_asn1.tdd_Config);

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] Frame TDD config %d, protocol_ms value %d\n", (int)mac_xface->lte_frame_parms->tdd_config, (int)protocol_ms->rrc.ue_bch_asn1.tdd_Config.subframeAssignment);
  msg ("[RRC-UE-FRONTEND] MAC_CONFIG_REQ  (SIB1)--->][MAC_UE]\n");
  msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
  #endif

 // After Decoding SIB1 (l. 776)
 /* rrc_mac_config_req(Mod_id,0,0,eNB_index,
	     (RadioResourceConfigCommonSIB_t *)NULL,
	     (struct PhysicalConfigDedicated *)NULL,
	     (MAC_MainConfig_t *)NULL,
	     0,
	     (struct LogicalChannelConfig *)NULL,
	     (MeasGapConfig_t *)NULL,
	     UE_rrc_inst[Mod_id].sib1[eNB_index]->tdd_Config,
	     &UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize,
	     &UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod); */

  rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,
       NULL, //(RadioResourceConfigCommonSIB_t *)
       NULL, //(struct PhysicalConfigDedicated *)
       (MeasObjectToAddMod_t **)NULL,
       NULL, // (MAC_MainConfig_t *)
       0,
       NULL, // (struct LogicalChannelConfig *)
       NULL, //(MeasGapConfig_t *)
       (TDD_Config_t *)&protocol_ms->rrc.ue_bch_asn1.tdd_Config,
       &SIwindowsize,
       &SIperiod,
       NULL,
       NULL,
       NULL,
       (MBSFN_SubframeConfigList_t *)NULL
       #ifdef Rel10
        , 0,
       (MBSFN_AreaInfoList_r9_t *)NULL,
       (PMCH_InfoList_r9_t *)NULL
      #endif
  );

// Simulate decoding SIB2
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] do_SIB2_cell\n");
  #endif

//uint8_t do_SIB2_cell(uint8_t Mod_id, LTE_DL_FRAME_PARMS *frame_parms, uint8_t *buffer,
//                  BCCH_DL_SCH_Message_t *bcch_message, SystemInformationBlockType2_t **sib2) {
  protocol_ms->rrc.ue_bch_asn1.sizeof_SIB2 = do_SIB2_cell(0, mac_xface->lte_frame_parms, &protocol_ms->rrc.ue_bch_asn1.SIB23,
                    &protocol_ms->rrc.ue_bch_asn1.systemInformation, &protocol_ms->rrc.ue_bch_asn1.sib2);

  #ifdef DEBUG_RRC_DETAILS
  msg ("\n[RRC-UE] SystemInformation SIB2 %d bytes\n", protocol_ms->rrc.ue_bch_asn1.sizeof_SIB2);
  //LOG_D(RRC,"[eNB] SystemInformation Encoded %d bits (%d bytes)\n",enc_rval.encoded,(enc_rval.encoded+7)/8);
  #endif

  if (protocol_ms->rrc.ue_bch_asn1.sizeof_SIB2 == 255)
    mac_xface->macphy_exit("rrc_sizeof_SIB2 is 255");
  else{
    //protocol_ms->rrc.ue_bch_asn1.sizeof_SIB2 = (u8)sizeof(SystemInformationBlockType2_t);

    #ifdef DEBUG_RRC_STATE
    msg ("[RRC-UE-FRONTEND] MAC_CONFIG_REQ  (SIB2)--->][MAC_UE]\n");
    msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
    #endif
   // After Decoding SIB2 (l. 1363)
   /*       rrc_mac_config_req(Mod_id,0,0,eNB_index,
			 &UE_rrc_inst[Mod_id].sib2[eNB_index]->radioResourceConfigCommon,
			 (struct PhysicalConfigDedicated *)NULL,
			 (MeasObjectToAddMod_t **)NULL,
			 (MAC_MainConfig_t *)NULL,
			 0,
			 (struct LogicalChannelConfig *)NULL,
			 (MeasGapConfig_t *)NULL,
			 (TDD_Config_t *)NULL,
			 NULL,
			 NULL,
			 UE_rrc_inst[Mod_id].sib2[eNB_index]->freqInfo.ul_CarrierFreq,
			 UE_rrc_inst[Mod_id].sib2[eNB_index]->freqInfo.ul_Bandwidth,
			 &UE_rrc_inst[Mod_id].sib2[eNB_index]->freqInfo.additionalSpectrumEmission,
			 UE_rrc_inst[Mod_id].sib2[eNB_index]->mbsfn_SubframeConfigList
#ifdef Rel10	       
			 ,
			 0,
			 (MBSFN_AreaInfoList_r9_t *)NULL,
			 (PMCH_InfoList_r9_t *)NULL
#endif
			 ); */

    rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,
//       &UE_rrc_inst[Mod_id].sib2[eNB_index]->radioResourceConfigCommon,
         (RadioResourceConfigCommonSIB_t *)&protocol_ms->rrc.ue_bch_asn1.sib2->radioResourceConfigCommon,
         NULL, // (struct PhysicalConfigDedicated *)
         (MeasObjectToAddMod_t **)NULL,
         NULL, //(MAC_MainConfig_t *)
         0,
         NULL, //(struct LogicalChannelConfig *)
         NULL, // (MeasGapConfig_t *)
         NULL, //(TDD_Config_t *)
         NULL,
         NULL,
         protocol_ms->rrc.ue_bch_asn1.sib2->freqInfo.ul_CarrierFreq,
         protocol_ms->rrc.ue_bch_asn1.sib2->freqInfo.ul_Bandwidth,
         &protocol_ms->rrc.ue_bch_asn1.sib2->freqInfo.additionalSpectrumEmission,
         protocol_ms->rrc.ue_bch_asn1.sib2->mbsfn_SubframeConfigList
         #ifdef Rel10
         , 0,
         (MBSFN_AreaInfoList_r9_t *)NULL,
         (PMCH_InfoList_r9_t *)NULL
         #endif
    );


  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_common_channels - END\n\n");
  #endif
  }
}

//-----------------------------------------------------------------------------
void rrc_ue_xmit_ccch (void){
//-----------------------------------------------------------------------------
  u8 i=0,rv[6];
  //char ccch_buffer[100];
  //int ccch_buffer_size;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] rrc_ue_xmit_ccch\n");
  #endif

  if(protocol_ms->rrc.ccch_buffer_size ==0){

    for (i=0;i<6;i++) {
      rv[i]=i+rrc_ue_mobileId;
    }
    protocol_ms->rrc.ccch_buffer_size = do_RRCConnectionRequest((u8 *)protocol_ms->rrc.ccch_buffer,rv);
 }
}

//-----------------------------------------------------------------------------
void rrc_ue_config_LTE_srb1 (void){
//-----------------------------------------------------------------------------
//  char Mod_id = protocol_ms->rrc.mod_id;
  char Mod_id = rrc_ue_mobileId;
  int UE_index = rrc_ue_mobileId;
  int eNB_index = 0;
  int eNB_flag = 0; //1=eNB, 0=UE
  int srb1 = 1;
  char buffer[1024];
  //LogicalChannelConfig_t *SRB1_logicalChannelConfig,*SRB2_logicalChannelConfig;

  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_LTE_srb1\n");
  #endif

  // get the parameters values SRB1_config, SRB2_config, physicalConfigDedicated
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] do_RRCConnectionSetup\n");
  #endif
  do_RRCConnectionSetup(buffer,
      //mac_xface->get_transmission_mode(Mod_id,find_UE_RNTI(Mod_id,UE_index)),
      1,  // TEMP - hardcoded in UE
      UE_index,0,
      mac_xface->lte_frame_parms,
      //&eNB_rrc_inst[Mod_id].SRB1_config[UE_index],
      //&protocol_ms->rrc.ue_rb_asn1.SRB1_config,
      //&eNB_rrc_inst[Mod_id].SRB2_config[UE_index],
      //&protocol_ms->rrc.ue_rb_asn1.SRB2_config,
      &protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
      //&eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index]);
      &protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated);

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] SRB1_logicalChannelConfig\n");
  #endif
  // get the parameters values SRB1_logicalChannelConfig
  // Default value set as global variable
  protocol_ms->rrc.ue_rb_asn1.SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] MAC_CONFIG_REQ  (SRB1 UE %d)--->][MAC_eNB]\n", UE_index);
  msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
  #endif
  //Apply configurations to MAC and RLC for SRB1 and SRB2
  /*
  UE:   rrc_mac_config_req(Mod_id,0,0,eNB_index,
		     (RadioResourceConfigCommonSIB_t *)NULL,
		     UE_rrc_inst[Mod_id].physicalConfigDedicated[eNB_index],
		     UE_rrc_inst[Mod_id].mac_MainConfig[eNB_index],
		     1,
		     SRB1_logicalChannelConfig,
		     (MeasGapConfig_t *)NULL,
		     NULL,
		     NULL,
		     NULL);
	}
  */

  rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,  //OK
       (RadioResourceConfigCommonSIB_t *)NULL,  //OK
       //(PhysicalConfigDedicated_t *)&protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated,  //OK
       protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated,  //OK
                     (MeasObjectToAddMod_t **)NULL,
       //(MAC_MainConfig_t*)&protocol_ms->rrc.ue_rb_asn1.mac_MainConfig,  //OK = NULL
       (MAC_MainConfig_t*)NULL,
       srb1, //logicalChannelIdentity  //OK
       protocol_ms->rrc.ue_rb_asn1.SRB1_logicalChannelConfig,  //OK
       protocol_ms->rrc.ue_rb_asn1.measGapConfig, //OK = NULL
       //(MeasGapConfig_t *)NULL, //OK = NULL
       (TDD_Config_t *)NULL, //OK
       (u8 *)NULL,  //OK
       (u16 *)NULL, //OK
       NULL,
       NULL,
       NULL,
       (MBSFN_SubframeConfigList_t *)NULL
       #ifdef Rel10
       , 0,
       (MBSFN_AreaInfoList_r9_t *)NULL,
       (PMCH_InfoList_r9_t *)NULL
       #endif
  );

  msg("[UE %d], CONFIG_SRB1 %d corresponding to eNB_index %d\n", Mod_id,srb1,eNB_index);
  msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, NB_eNB_INST %d \n",Mod_id, eNB_flag, UE_index, NB_eNB_INST);
//   rrc_pdcp_config_req (Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, ACTION_ADD, srb1);
//   rrc_rlc_config_req(Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, ACTION_ADD, srb1, SIGNALLING_RADIO_BEARER, Rlc_info_am_config);
  rrc_pdcp_config_asn1_req(Mod_id,UE_index,protocol_ms->rrc.current_SFN,eNB_flag,
      protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
      (DRB_ToAddModList_t*)NULL,
      (DRB_ToReleaseList_t*)NULL
      #ifdef Rel10
      ,(MBMS_SessionInfoList_r9_t *)NULL
      #endif
  );

 rrc_rlc_config_asn1_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,UE_index,
      protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
      (DRB_ToAddModList_t*)NULL, 
      (DRB_ToReleaseList_t*)NULL
      #ifdef Rel10
      ,(MBMS_SessionInfoList_r9_t *)NULL
      #endif
      );

  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_LTE_srb1 - END\n\n");
  #endif
}

//-----------------------------------------------------------------------------
void rrc_ue_config_LTE_srb2 (void){
//-----------------------------------------------------------------------------
  char Mod_id = protocol_ms->rrc.mod_id;
  int UE_index = rrc_ue_mobileId;
  int eNB_index = 0;
  int eNB_flag = 0; //1=eNB, 0=UE
  int srb2 = 2;
  //char buffer[1024];
  //LogicalChannelConfig_t *SRB1_logicalChannelConfig,*SRB2_logicalChannelConfig;

  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_LTE_srb2\n");
  #endif

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] SRB2_logicalChannelConfig\n");
  #endif
  // get the parameters values SRB1_logicalChannelConfig SRB2_logicalChannelConfig
  // Default value set as global variable
  protocol_ms->rrc.ue_rb_asn1.SRB2_logicalChannelConfig = &SRB2_logicalChannelConfig_defaultValue;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] MAC_CONFIG_REQ  (SRB2 UE 0)--->][MAC_eNB]\n");
  msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
  #endif
  rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,  //OK
       (RadioResourceConfigCommonSIB_t *)NULL,  //OK
       protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated,
                     (MeasObjectToAddMod_t **)NULL,
       //(MAC_MainConfig_t*)&protocol_ms->rrc.ue_rb_asn1.mac_MainConfig,  //OK = NULL
       (MAC_MainConfig_t*)NULL,
       srb2, //logicalChannelIdentity  //OK
       protocol_ms->rrc.ue_rb_asn1.SRB2_logicalChannelConfig,  //OK
       //(MeasGapConfig_t *)&protocol_ms->rrc.ue_rb_asn1.measGapConfig, //OK = NULL
       (MeasGapConfig_t *)NULL, //OK = NULL
       (TDD_Config_t *)NULL, //OK
       (u8 *)NULL,  //OK
       (u16 *)NULL, //OK
       NULL,
       NULL,
       NULL,
       (MBSFN_SubframeConfigList_t *)NULL
       #ifdef Rel10
       , 0,
       (MBSFN_AreaInfoList_r9_t *)NULL,
       (PMCH_InfoList_r9_t *)NULL
       #endif
  );

  msg("[UE %d], CONFIG_SRB2 %d corresponding to eNB_index %d\n", Mod_id,srb2,eNB_index);
//   rrc_pdcp_config_req (Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, ACTION_ADD, srb2);
//   rrc_rlc_config_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,ACTION_ADD,srb2,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
  rrc_pdcp_config_asn1_req(Mod_id,UE_index,protocol_ms->rrc.current_SFN,eNB_flag,
      protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
      (DRB_ToAddModList_t*)NULL,
      (DRB_ToReleaseList_t*)NULL
      #ifdef Rel10
      ,(MBMS_SessionInfoList_r9_t *)NULL
      #endif
  );

 rrc_rlc_config_asn1_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,UE_index,
      protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
      (DRB_ToAddModList_t*)NULL, 
      (DRB_ToReleaseList_t*)NULL
      #ifdef Rel10
      ,(MBMS_SessionInfoList_r9_t *)NULL
      #endif
      );

  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_LTE_srb2 - END\n\n");
  #endif
}

//-----------------------------------------------------------------------------
void rrc_ue_config_LTE_default_drb (unsigned char Mod_id){
//-----------------------------------------------------------------------------
  int UE_index = rrc_ue_mobileId;
  int eNB_index = 0;
  int eNB_flag = 0; //1=eNB, 0=UE
  int drb_ix=0;  // default DRB
  u8 buffer[100];
  u8 size;
  u8 DRB2LCHAN[8];
  //DL_DCCH_Message_t *dl_dcch_msg=NULL;//&dldcchmsg;
  struct PhysicalConfigDedicated  physicalConfigDedicated_drb;

  #ifdef DEBUG_RRC_STATE
  msg ("\n[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb: begin\n");
  #endif
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] do_RRCConnReconf_defaultCELL\n");
  #endif

  size = do_RRCConnReconf_defaultCELL(Mod_id, buffer, UE_index, 0,
                                         //&eNB_rrc_inst[Mod_id].SRB2_config[UE_index],
                                         &protocol_ms->rrc.ue_rb_asn1.SRB_configList[UE_index],
                                         //&eNB_rrc_inst[Mod_id].DRB_config[UE_id][0],
                                         &protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index],
                                         //&eNB_rrc_inst[Mod_id].physicalConfigDedicated[UE_index]);
                                         //&protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated);
                                         &physicalConfigDedicated_drb);

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-UE-FRONTEND] do_DRB_UE_MAC_config_cell\n");
  #endif
  do_DRB_UE_MAC_config_cell (&protocol_ms->rrc.ue_rb_asn1.mac_MainConfig);

  if (protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]){

    DRB2LCHAN[drb_ix] = (u8)*protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]->logicalChannelIdentity;

      //if (eNB_rrc_inst[Mod_id].DRB_config[UE_index][i]) {
    msg("[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb: reconfiguring DRB %d/LCID %d, DRB2LCHAN %d\n",
        (int)protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]->drb_Identity,
        (int)*protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]->logicalChannelIdentity, (int) DRB2LCHAN[drb_ix]);

    if (protocol_ms->rrc.ue_rb_asn1.DRB1_active == 0) {
      msg("[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb: Frame %d: Establish PDCP + RLC UM Bidirectional, DRB %d Active\n",
            protocol_ms->rrc.current_SFN, (int)protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]->drb_Identity);

/*      rrc_pdcp_config_req (Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, ACTION_ADD,
                            (UE_index * NB_RB_MAX) + (int)*protocol_ms->rrc.ue_rb_asn1.DRB1_config->logicalChannelIdentity);
      rrc_rlc_config_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,ACTION_ADD,
                          (UE_index * NB_RB_MAX) + (int)*protocol_ms->rrc.ue_rb_asn1.DRB1_config->logicalChannelIdentity,
                          RADIO_ACCESS_BEARER,Rlc_info_um);*/
      rrc_pdcp_config_asn1_req(Mod_id,UE_index,protocol_ms->rrc.current_SFN,eNB_flag,
          (SRB_ToAddModList_t*)NULL,
          protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index],
          (DRB_ToReleaseList_t*)NULL
          #ifdef Rel10
          ,(MBMS_SessionInfoList_r9_t *)NULL
          #endif
      );
      rrc_rlc_config_asn1_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,UE_index,
          (SRB_ToAddModList_t*)NULL,
          protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index],
          (DRB_ToReleaseList_t*)NULL
          #ifdef Rel10
          ,(MBMS_SessionInfoList_r9_t *)NULL
          #endif
      );
      protocol_ms->rrc.ue_rb_asn1.DRB1_active = 1;

      msg("[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb:[--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB]\n", UE_index);
      msg("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
      //DRB2LCHAN[drb_ix] = (u8)*protocol_ms->rrc.ue_rb_asn1.DRB1_config->logicalChannelIdentity;
  
      rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,
        (RadioResourceConfigCommonSIB_t *)NULL,
        protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated,
        (MeasObjectToAddMod_t **)NULL,
        (MAC_MainConfig_t*)&protocol_ms->rrc.ue_rb_asn1.mac_MainConfig,
        DRB2LCHAN[drb_ix],
        protocol_ms->rrc.ue_rb_asn1.DRB_configList[UE_index]->list.array[0]->logicalChannelConfig,
        (MeasGapConfig_t *)&protocol_ms->rrc.ue_rb_asn1.measGapConfig,
        (TDD_Config_t *)NULL,
        (u8 *)NULL,
        (u16 *)NULL, //OK
        NULL,
        NULL,
        NULL,
        (MBSFN_SubframeConfigList_t *)NULL
        #ifdef Rel10
        , 0,
        (MBSFN_AreaInfoList_r9_t *)NULL,
        (PMCH_InfoList_r9_t *)NULL
        #endif
    );
  
    }else{ // remove LCHAN from MAC/PHY
      // Initialized here because it is a local variable (global in RRC Lite)
      //DRB2LCHAN[drb_ix] = (u8)*protocol_ms->rrc.ue_rb_asn1.DRB1_config->logicalChannelIdentity;
      if (protocol_ms->rrc.ue_rb_asn1.DRB1_active ==1) {
        // DRB has just been removed so remove RLC + PDCP for DRB
        msg("[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb: Frame %d: Remove PDCP + RLC UM Bidirectional, DRB 0 \n",
            //protocol_ms->rrc.current_SFN, (int)protocol_ms->rrc.ue_rb_asn1.DRB1_config->drb_Identity);
            protocol_ms->rrc.current_SFN);
        msg("                  COMMENTED - not yet supported with ASN1 \n");

/*        rrc_pdcp_config_req (Mod_id+NB_eNB_INST, protocol_ms->rrc.current_SFN, eNB_flag, ACTION_REMOVE,
                              (UE_index * NB_RB_MAX) + DRB2LCHAN[drb_ix]);*/
/*        rrc_rlc_config_req(Mod_id+NB_eNB_INST,protocol_ms->rrc.current_SFN,eNB_flag,ACTION_REMOVE,
                            (UE_index * NB_RB_MAX) + DRB2LCHAN[drb_ix],
                            RADIO_ACCESS_BEARER,Rlc_info_um);*/
      }
      protocol_ms->rrc.ue_rb_asn1.DRB1_active = 0;
      msg("[RRC-UE-FRONTEND] rrc_ue_config_LTE_default_drb:[--- MAC_CONFIG_REQ  (DRB UE %d) --->][MAC_eNB]\n", UE_index);
      msg ("[RRC-UE-FRONTEND] Parameters : Mod_id %d, eNB_flag %d, UE_index %d, eNB_index %d \n",Mod_id, eNB_flag, UE_index, eNB_index);
      rrc_mac_config_req(Mod_id,eNB_flag,UE_index,eNB_index,
        (RadioResourceConfigCommonSIB_t *)NULL,
        protocol_ms->rrc.ue_rb_asn1.physicalConfigDedicated,
        (MeasObjectToAddMod_t **)NULL,
        (MAC_MainConfig_t*)&protocol_ms->rrc.ue_rb_asn1.mac_MainConfig,
        DRB2LCHAN[drb_ix],
        (LogicalChannelConfig_t *)NULL,
        (MeasGapConfig_t *)NULL,
        (TDD_Config_t *)NULL,
        (u8 *)NULL,
        (u16 *)NULL, //OK
        NULL,
        NULL,
        NULL,
        (MBSFN_SubframeConfigList_t *)NULL
        #ifdef Rel10
        , 0,
        (MBSFN_AreaInfoList_r9_t *)NULL,
        (PMCH_InfoList_r9_t *)NULL
        #endif
      );
    }
  }
}

