/*
 * rrc_messages_def.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

//-------------------------------------------------------------------------------------------//
// Messages for RRC logging
MESSAGE_DEF(RRC_DL_BCCH_MESSAGE,        MESSAGE_PRIORITY_MED_PLUS,  RrcDlBcchMessage,           rrc_dl_bcch_message)
MESSAGE_DEF(RRC_DL_CCCH_MESSAGE,        MESSAGE_PRIORITY_MED_PLUS,  RrcDlCcchMessage,           rrc_dl_ccch_message)
MESSAGE_DEF(RRC_DL_DCCH_MESSAGE,        MESSAGE_PRIORITY_MED_PLUS,  RrcDlDcchMessage,           rrc_dl_dcch_message)

MESSAGE_DEF(RRC_UE_EUTRA_CAPABILITY,    MESSAGE_PRIORITY_MED_PLUS,  RrcUeEutraCapability,       rrc_ue_eutra_capability)

MESSAGE_DEF(RRC_UL_CCCH_MESSAGE,        MESSAGE_PRIORITY_MED_PLUS,  RrcUlCcchMessage,           rrc_ul_ccch_message)
MESSAGE_DEF(RRC_UL_DCCH_MESSAGE,        MESSAGE_PRIORITY_MED_PLUS,  RrcUlDcchMessage,           rrc_ul_dcch_message)

//-------------------------------------------------------------------------------------------//
// Messages between NAS and RRC layers
MESSAGE_DEF(NAS_DOWNLINK_DATA_IND,      MESSAGE_PRIORITY_MED,       NasDlDataInd,               nas_dl_data_ind)

MESSAGE_DEF(NAS_UPLINK_DATA_REQ,        MESSAGE_PRIORITY_MED,       NasUlDataReq,               nas_ul_data_req)
MESSAGE_DEF(NAS_UPLINK_DATA_CNF,        MESSAGE_PRIORITY_MED,       NasUlDataCnf,               nas_ul_data_cnf)
MESSAGE_DEF(NAS_UPLINK_DATA_IND,        MESSAGE_PRIORITY_MED,       NasUlDataInd,               nas_ul_data_ind)
