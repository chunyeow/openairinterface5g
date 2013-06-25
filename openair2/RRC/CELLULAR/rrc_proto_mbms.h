/***************************************************************************
                          rrc_proto_mbms.h - description
                          -------------------
    begin               : Jun 9, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Huu-Nghia.Nguyen@eurecom.fr	
    modified by         : Michelle.Wetterwald@eurecom.fr
    description         : This file defines all the MBMS functions
    which are accessible from outside 
 **************************************************************************/
#ifndef _RRC_PROTO_MBMS_H_
#define _RRC_PROTO_MBMS_H_

//rrc_mbms_pdus.c
#ifdef NODE_RG
int rrc_PEREnc_MBMSAccessInformation (MBMSAccessInformation * pvalue);
int rrc_PEREnc_MBMSCommonPTMRBInformation (MBMSCommonPTMRBInformation * pvalue);
int rrc_PEREnc_MBMSCurrentCellPTMRBInformation (MBMSCurrentCellPTMRBInformation * pvalue);
int rrc_PEREnc_MBMSGeneralInformation (MBMSGeneralInformation * pvalue);
int rrc_PEREnc_MBMSModifiedServicesInformation (MBMSModifiedServicesInformation * pvalue);
int rrc_PEREnc_MBMSNeighbouringCellPTMRBInformation (int cellIndex, MBMSNeighbouringCellPTMRBInformation * pvalue);
int rrc_PEREnc_MBMSSchedulingInformation (MBMSSchedulingInformation * pvalue);
int rrc_PEREnc_MBMSUnmodifiedServicesInformation (MBMSUnmodifiedServicesInformation * pvalue);
#endif
#ifdef NODE_MT
int rrc_PERDec_MBMSAccessInformation (MBMSAccessInformation * pvalue);
int rrc_PERDec_MBMSCommonPTMRBInformation (MBMSCommonPTMRBInformation * pvalue);
int rrc_PERDec_MBMSCurrentCellPTMRBInformation (MBMSCurrentCellPTMRBInformation * pvalue);
int rrc_PERDec_MBMSGeneralInformation (MBMSGeneralInformation * pvalue);
int rrc_PERDec_MBMSModifiedServicesInformation (MBMSModifiedServicesInformation * pvalue);
int rrc_PERDec_MBMSNeighbouringCellPTMRBInformation (int *pCellIndex, MBMSNeighbouringCellPTMRBInformation * pvalue);
int rrc_PERDec_MBMSSchedulingInformation (MBMSSchedulingInformation * pvalue);
int rrc_PERDec_MBMSUnmodifiedServicesInformation (MBMSUnmodifiedServicesInformation * pvalue);
#endif

#ifdef NODE_MT
//rrc_ue_mbms_decode.c
//void rrc_ue_mcch_decode (mem_block_t * sduP, u16 length);
void rrc_ue_mcch_decode (char * sduP, int offset);
int rrc_ue_mbms_MCCH_decode(MCCH_Message *mcch_msg);
int rrc_ue_mbms_DCCH_decode(DL_DCCH_Message *dl_dcch_msg);
int rrc_ue_mbms_MSCH_decode(MSCH_Message *msch_msg);

//rrc_ue_mbms_fsm.c
void RRC_UE_MBMS_I_CONTROLING_CELL_CHANGED(void);
void RRC_UE_MBMS_I_RETURN_FROM_LOSS_COVERAGE(void);
void RRC_UE_MBMS_I_ACTIVATED_SERVICE_CHANGED(void);
void RRC_UE_MBMS_I_SELECTING_CELL_MBMS(void);
void RRC_UE_MBMS_I_MODIF_SERV_INFO(void);
void RRC_UE_MBMS_I_MCCH_MODIF_SERV_INFO(void);
void RRC_UE_MBMS_I_ALL_UNMODIF_PTM_SERVICES(void);
void RRC_UE_MBMS_I_UNMODIF_SERV_INFO(void);
void RRC_UE_MBMS_I_COMMON_CELL_RB_INFO(void);
void RRC_UE_MBMS_I_CURRENT_CELL_RB_INFO(void);
void RRC_UE_MBMS_I_NEIGHBOURING_CELL_RB_INFO(void);
void RRC_UE_MBMS_I_MODIF_PERIOD_ENDED(void);
void rrc_ue_mbms_fsm(void);
void rrc_ue_mbms_fsm_reset(void);

//rrc_ue_mbms_if.c
extern rrc_ue_mbms_variables * volatile p_ue_mbms; //pointer refer to protocol_ms->rrc.mbms
void rrc_ue_mbms_init(void);
void rrc_ue_mbms_scheduling_check(void);

//rrc_ue_mbms_outputs.c
void RRC_UE_MBMS_O_NAS_MBMS_UE_NOTIFY_IND(void);
void RRC_UE_MBMS_O_ANALYSE_UNMODIF(void);
void RRC_UE_MBMS_O_CURRENT_CELL_RB_CONFIGURATION(void);
void RRC_UE_MBMS_O_NEIGHBOURING_CELL_RB_CONFIGURATION(void);
void RRC_UE_MBMS_O_MCCH_NOTIFICATION(void);
void RRC_UE_MBMS_O_DCCH_NOTIFICATION(void);

//rrc_ue_mbms_test_simu.c
void rrc_ue_mbms_destroy(void);
void rx_simulate(int chanelID);
void rrc_ue_mbms_run(void);
mbms_bool rrc_ue_mbms_MCCH_rx(void);
mbms_bool rrc_ue_mbms_DCCH_rx(void);
mbms_bool rrc_ue_mbms_MSCH_rx(void);

#endif

#ifdef NODE_RG
//rrc_rg_mbms_encode.c
void rrc_rg_mbms_MCCH_encode(int Message_Type);
void rrc_rg_mbms_DCCH_encode(int Message_Type);
void rrc_rg_mbms_MSCH_encode(int Message_Type);

//rrc_rg_mbms_if.c
extern rrc_rg_mbms_variables * volatile p_rg_mbms; // pointer refer to protocol_bs->rrc.mbms
void rrc_rg_mbms_init(void);
void rrc_rg_mbms_scheduling_check(void);
void rrc_rg_mbms_end_modification_period_check(void);
void rrc_rg_mbms_MCCH_tx(void);
void rrc_rg_mbms_DCCH_tx(int ueID);
void rrc_rg_mbms_MSCH_tx(void);
void rrc_mbms_tx(void);

//rrc_rg_mbms_nas_if.c
mbms_bool rrc_rg_mbms_checkAcquireRBInfo(void);
mbms_bool rrc_rg_mbms_addUnmodifService(int index, o3 serviceID, u8 umod_requiredAction);
mbms_bool rrc_rg_mbms_deleteUnmodifService(int index);
mbms_bool rrc_rg_mbms_addModifService(int index, o3 serviceID, u8 umod_requiredAction);
mbms_bool rrc_rg_mbms_deleteModifService(int index);
void rrc_rg_mbms_NAS_ServStart_rx(void);
void rrc_rg_mbms_NAS_ServStop_rx(void);
void rrc_rg_mbms_NAS_Notification_rx(void);

//rrc_rg_mbms_outputs.c
//void rrc_rg_mbms_config_indication (int transaction_Id, int return_code);
void RRC_RG_MBMS_O_UE_NOTIFY_CNF(void);
void RRC_RG_MBMS_O_GET_RB_INFORMATION(int action);
void RRC_RG_MBMS_O_L12_CONFIGURE(void);
void RRC_RG_MBMS_O_SEND_DCCH_UM(int ueID, char* pmsg, int msglen);
void RRC_RG_MBMS_O_SEND_MCCH(char* pmsg, int msglen);
void RRC_RG_MBMS_O_SEND_MSCH(char* pmsg, int msglen);
void RRC_RG_O_O_NAS_MBMS_RB_ESTAB_CNF (void);

//rrc_rg_mbms_test_simu.c
void rrc_rg_mbms_destroy(void);
void rrc_rg_mbms_run(void);
void tx_simulate(int chanelID, char* pmsg, int msglen);
void rrc_rg_mbms_scenario_check(void); //create the NAS scenario
#endif

//rrc_mbms_utilities.c
#ifdef NODE_MT
void rrc_ue_mbms_modif_services_print(void);
void rrc_ue_mbms_status_services_print(void);
#endif

#ifdef NODE_RG
void rrc_rg_mbms_modif_services_print(void);
void rrc_rg_mbms_unmodif_services_print(void);
void rrc_rg_mbms_message_update(rrc_rg_mbms_msg_buf * ptrMsgInfo);
void rrc_rg_mbms_message_reset(rrc_rg_mbms_msg_buf * ptrMsgInfo);
void rrc_rg_mbms_processJoinedServices(void);
void rrc_rg_mbms_processLeftServices(void);
#endif

void rrc_mbms_mcch_message_name_print(int mcchMsgType);
int  rrc_mbms_service_id_find(u8 numService, void * services, int serviceID);

//rrc_mbms_test.c
#ifdef MBMS_TEST_MODE
extern u8 * chanel_data_tampon[3];
extern int * chanel_data_length;
//extern int frame;
#endif

#endif //_RRC_MBMS_PROTO_H_
