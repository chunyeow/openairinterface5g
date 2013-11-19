/*
 * pdcp_messages_def.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

//-------------------------------------------------------------------------------------------//
// Messages between RRC and PDCP layers
MESSAGE_DEF(RRC_DCCH_DATA_REQ,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataReq,              rrc_dcch_data_req)
MESSAGE_DEF(RRC_DCCH_DATA_IND,          MESSAGE_PRIORITY_MED_PLUS, RrcDcchDataInd,              rrc_dcch_data_ind)
