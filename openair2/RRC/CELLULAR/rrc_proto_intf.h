/***************************************************************************
                          rrc_proto_intf.h  -  description
                             -------------------
    begin                : Jan 17, 2002
    copyright            : (C) 2001, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Prototypes for functions related to interfaces
 ***************************************************************************/
#ifndef __RRC_PROTO_INTF_H__
#define __RRC_PROTO_INTF_H__

//PHY
//-----------------------------------------------------------------------------
#ifdef NODE_MT
  // functions in L1C/L1C_rrc_control.c
int  rrc_ue_DL_CPHY_TrCH_config_req (int ccTrCh, int activationTime);
int  rrc_ue_UL_CPHY_TrCH_config_req (int ccTrCh, int activationTime);

int  rrc_ue_CPHY_TrCH_release_req (int configId);
int  rrc_ue_CPHY_OutOfSync_Config_req (int configId);        // ???
void rrc_ue_CPHY_Measurement_req (int power_threshold, int meas_type);
  // functions in AS/L3/RRC/rrc_L1_control.c
void rrc_ue_CPHY_Synch_rx (int CCTrCH);
void rrc_ue_CPHY_Out_Synch_rx (int CCTrCH);
// removed 25/11 MW
// void rrc_ue_CPHY_Measurement_rx (struct Measurements results);
void rrc_ue_CPHY_Measurement_rx (void);
#endif
#ifdef NODE_RG
  // functions in L1C/L1C_rrc_control.c
int  rrc_rg_CPHY_TrCH_config_req (int configId, int activationTime);
int  rrc_rg_CPHY_TrCH_release_req (int configId);
int  rrc_rg_CPHY_OutOfSync_Config_req (int configId);        // ???
void rrc_rg_CPHY_Measurement_req (int power_threshold, int meas_type);
  // functions in AS/L3/RRC/rrc_L1_control.c
void rrc_rg_CPHY_Synch_rx (int CCTrCH);
void rrc_rg_CPHY_Out_Synch_rx (int CCTrCH);
// removed 25/11 MW
// void rrc_rg_CPHY_Measurement_rx (struct Measurements results);
void rrc_rg_CPHY_Measurement_rx (void);
#endif

//MAC
//-----------------------------------------------------------------------------
#ifdef NODE_MT
void rrc_ue_connection_mac_setup (void);
void rrc_ue_connection_mac_release (void);
void mac_remove_all (void);
void cmac_fach_rach_setup (void);
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
void rrc_rg_connection_mac_setup (void);
void rrc_rg_connection_mac_release (int userP);
#endif

//RB
//-----------------------------------------------------------------------------
#ifdef NODE_MT
void rrc_ue_connection_srb_setup (void);
void rrc_ue_connection_srb_release (void);
void rrc_ue_connection_rb_setup (void);
void rrc_ue_connection_rb_release (void);
void rb_remove_all (void);
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
void rrc_rg_connection_srb_setup (void);
void rrc_rg_connection_srb_release (int userP);
void rrc_rg_connection_rb_setup (void);
void rrc_rg_connection_rb_release (int userP);
#endif

//-----------------------------------------------------------------------------
#ifdef NODE_MT
void CPHY_release_UE_resources (void);
void CPHY_config_fach_rach (void);
#endif

//OAI - rrc__L2_frontend.c
//-----------------------------------------------------------------------------
#ifdef NODE_MT
void rrc_ue_L2_setupFachRach(void);
void rrc_ue_config_common_channels (void);
void rrc_ue_xmit_ccch (void);
void rrc_ue_config_LTE_srb1 (void);
void rrc_ue_config_LTE_srb2 (void);
void rrc_ue_config_common_channels_SIB2 (void);
void rrc_ue_config_LTE_default_drb (unsigned char Mod_id);
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
void rrc_rg_get_common_config_SIB (int *config_length, char* *config_ptr);
void rrc_rg_init_mac (unsigned char Mod_id);
void rrc_rg_config_LTE_srb1 (unsigned char Mod_id);
void rrc_rg_rcve_ccch(u8 Mod_id, char *Sdu, u16 Sdu_len);
void rrc_rg_config_LTE_srb2 (unsigned char Mod_id);
void rrc_rg_config_LTE_default_drb (unsigned char Mod_id);
int rrc_rg_ENbMeas_get_rlcBufferOccupancy(int UE_id);
int rrc_rg_ENbMeas_get_scheduledPRB(int UE_id);
int rrc_rg_ENbMeas_get_totalDataVolume(int UE_id);
int rrc_rg_ENbMeas_get_totalNumPRBs(void);

#endif
//-----------------------------------------------------------------------------
void wcdma_handle_error (int errorP);

//CONFIG
void crb_config_req (int activation_timeP);
void cmac_config_req (int userP, int activation_timeP);
void CPHY_config_req (void *config, int activation_time, int userP);

//SRB
//-----------------------------------------------------------------------------
#ifdef NODE_RG
extern void *rrc_rg_srb0_rx (void *unusedP, mem_block_t * sduP);
extern void  rrc_rg_mcch_tx (u8 * dataP, u16 lengthP);
extern int rrc_rg_srb_rx (char* sduP, int srb_id,  int UE_Id);
#endif

#ifdef NODE_MT
extern void *rrc_ue_ccch_rx_idle_mode (void *unusedP, struct mac_data_ind data_indicationP);
extern void *rrc_mt_ccch_rx_idle_mode (void *unusedP, mem_block_t * data_indicationP, int rb_idP);
//extern void *rrc_mt_mcch_rx  (void *unusedP, mem_block_t * data_indicationP, int rb_idP);

#endif
extern void *rrc_srb_rx (void *unusedP, mem_block_t * sduP, u8 rb_idP);
extern void *rrc_srb_confirm (u32 muiP, u8 rb_idP, u8 statusP);

//RRM
//-----------------------------------------------------------------------------
#ifdef NODE_RG
  // function  in AS/L3/RRM
//void rrm_config_change_request (struct rrc_rrm_request config_request);

  // function  in AS/L3/RRC
void rrc_config_indication (int transaction_Id, int return_code);

  // function  in AS/L3/RRM
// void rrc_rrm_measure_request (struct rrc_rrm_measure_ctl rrm_control);

// Measure report
// from Mobile Terminal
// void rrm_meas_report_mt_if (struct rrc_rrm_meas_report_mt_if *pmeas);
// void rrm_meas_report_mt_tv (struct rrc_rrm_meas_report_mt_tv *pmeas);
// void rrm_meas_report_mt_q (struct rrc_rrm_meas_report_mt_q *pmeas);
// void rrm_meas_report_mt_int (struct rrc_rrm_meas_report_mt_int *pmeas);

// from Base station
// void rrm_meas_report_bs_tv (struct rrc_rrm_meas_report_bs_tv *pmeas);
// void rrm_meas_report_bs_q (struct rrc_rrm_meas_report_bs_q *pmeas);
// void rrm_meas_report_bs_int (struct rrc_rrm_meas_report_bs_int *pmeas);

// in rrc rrm intf
void rrm_add_user_confirm (int ue_idP);
void rrm_add_radio_access_bearer_confirm (int ue_idP, int rb_idP);
int  rrm_get_ue_cctrch_index_ul (int mobileIdP, int rabP);

void RRC_RG_O_O_NAS_RB_Failure (int UE_Id);

#endif

//ASN1 messages
/*uint8_t do_RRCConnectionSetup(uint8_t *buffer,
			      uint8_t transmission_mode,
			      uint8_t UE_id,
			      uint8_t Transaction_id,
			      LTE_DL_FRAME_PARMS *frame_parms,
			      struct SRB_ToAddMod **SRB1_config,
			      struct SRB_ToAddMod **SRB2_config,
			      struct PhysicalConfigDedicated  **physicalConfigDedicated);*/
//uint8_t do_SIB2_cell(uint8_t Mod_id, uint8_t *buffer, SystemInformation_t *systemInformation, SystemInformationBlockType2_t **sib2);
uint8_t do_SIB2_cell(uint8_t Mod_id, LTE_DL_FRAME_PARMS *frame_parms, uint8_t *buffer,
                  BCCH_DL_SCH_Message_t *bcch_message, SystemInformationBlockType2_t **sib2);

uint8_t do_SIB1_TDD_config_cell (LTE_DL_FRAME_PARMS *frame_parms,TDD_Config_t *tdd_Config);
uint8_t do_RRCConnReconf_defaultCELL(uint8_t Mod_id, uint8_t *buffer, uint8_t UE_id, uint8_t Transaction_id,
                                      struct SRB_ToAddMod **SRB2_config,
                                      struct DRB_ToAddMod **DRB_config,
                                      struct PhysicalConfigDedicated  **physicalConfigDedicated) ;
//

#endif

