/*
 * rrc_messages_def.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

MESSAGE_DEF(RRC_UE_EUTRA_CAPABILITY,    MESSAGE_PRIORITY_MED_PLUS, RrcUeEutraCapability,        rrc_ue_eutra_capability)

//-------------------------------------------------------------------------------------------//
// Messages from MAC layer
MESSAGE_DEF(RRC_MAC_IN_SYNC_IND,        MESSAGE_PRIORITY_MED_PLUS, RrcMacInSyncInd,             rrc_mac_in_sync_ind)
MESSAGE_DEF(RRC_MAC_OUT_OF_SYNC_IND,    MESSAGE_PRIORITY_MED_PLUS, RrcMacOutOfSyncInd,          rrc_mac_out_of_sync_ind)

MESSAGE_DEF(RRC_MAC_BCCH_DATA_REQ,      MESSAGE_PRIORITY_MED_PLUS, RrcMacBcchDataReq,           rrc_mac_bcch_data_req)
MESSAGE_DEF(RRC_MAC_BCCH_DATA_IND,      MESSAGE_PRIORITY_MED_PLUS, RrcMacBcchDataInd,           rrc_mac_bcch_data_ind)
MESSAGE_DEF(RRC_MAC_CCCH_DATA_IND,      MESSAGE_PRIORITY_MED_PLUS, RrcMacCcchDataInd,           rrc_mac_ccch_data_ind)
MESSAGE_DEF(RRC_MAC_CCCH_SUCCESS_IND,   MESSAGE_PRIORITY_MED_PLUS, RrcMacCcchSuccessInd,        rrc_mac_ccch_success_ind)
MESSAGE_DEF(RRC_MAC_MCCH_DATA_IND,      MESSAGE_PRIORITY_MED_PLUS, RrcMacMcchDataInd,           rrc_mac_mcch_data_ind)

//-------------------------------------------------------------------------------------------//
// Messages from/to PDCP layer
MESSAGE_DEF(RRC_DCCH_DATA_REQ,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataReq,              rrc_dcch_data_req)
MESSAGE_DEF(RRC_DCCH_DATA_IND,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataInd,              rrc_dcch_data_ind)
