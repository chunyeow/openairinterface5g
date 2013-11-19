/*
 * rrc_messages_types.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

#ifndef RRC_MESSAGES_TYPES_H_
#define RRC_MESSAGES_TYPES_H_

#include "as_message.h"

//-------------------------------------------------------------------------------------------//
// Messages for RRC logging
#include "BCCH-DL-SCH-Message.h"
#include "DL-CCCH-Message.h"
#include "DL-DCCH-Message.h"
#include "UE-EUTRA-Capability.h"
#include "UL-CCCH-Message.h"
#include "UL-DCCH-Message.h"

typedef BCCH_DL_SCH_Message_t   RrcDlBcchMessage;
typedef DL_CCCH_Message_t       RrcDlCcchMessage;
typedef DL_DCCH_Message_t       RrcDlDcchMessage;
typedef UE_EUTRA_Capability_t   RrcUeEutraCapability;
typedef UL_CCCH_Message_t       RrcUlCcchMessage;
typedef UL_DCCH_Message_t       RrcUlDcchMessage;

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.
#define NAS_DOWNLINK_DATA_IND(mSGpTR)               (mSGpTR)->msg.nas_dl_data_ind

#define NAS_UPLINK_DATA_REQ(mSGpTR)                 (mSGpTR)->msg.nas_ul_data_req
#define NAS_UPLINK_DATA_CNF(mSGpTR)                 (mSGpTR)->msg.nas_ul_data_cnf
#define NAS_UPLINK_DATA_IND(mSGpTR)                 (mSGpTR)->msg.nas_ul_data_ind

//-------------------------------------------------------------------------------------------//
// Messages between NAS and RRC layers
typedef dl_info_transfer_ind_t NasDlDataInd;

typedef ul_info_transfer_req_t NasUlDataReq;
typedef ul_info_transfer_cnf_t NasUlDataCnf;
typedef ul_info_transfer_ind_t NasUlDataInd;

#endif /* RRC_MESSAGES_TYPES_H_ */
