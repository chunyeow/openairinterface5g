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
#if defined(DISABLE_ITTI_XER_PRINT)
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
#endif

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.
#define RRC_CONFIGURATION_REQ(mSGpTR)   (mSGpTR)->ittiMsg.rrc_configuration_req

#define NAS_CELL_SELECTION_REQ(mSGpTR)  (mSGpTR)->ittiMsg.nas_cell_selection_req
#define NAS_CONN_ESTABLI_REQ(mSGpTR)    (mSGpTR)->ittiMsg.nas_conn_establi_req
#define NAS_UPLINK_DATA_REQ(mSGpTR)     (mSGpTR)->ittiMsg.nas_ul_data_req

#define NAS_RAB_ESTABLI_RSP(mSGpTR)     (mSGpTR)->ittiMsg.nas_rab_est_rsp

#define NAS_CELL_SELECTION_CNF(mSGpTR)  (mSGpTR)->ittiMsg.nas_cell_selection_cnf
#define NAS_CELL_SELECTION_IND(mSGpTR)  (mSGpTR)->ittiMsg.nas_cell_selection_ind
#define NAS_PAGING_IND(mSGpTR)          (mSGpTR)->ittiMsg.nas_paging_ind
#define NAS_CONN_ESTABLI_CNF(mSGpTR)    (mSGpTR)->ittiMsg.nas_conn_establi_cnf
#define NAS_CONN_RELEASE_IND(mSGpTR)    (mSGpTR)->ittiMsg.nas_conn_release_ind
#define NAS_UPLINK_DATA_CNF(mSGpTR)     (mSGpTR)->ittiMsg.nas_ul_data_cnf
#define NAS_DOWNLINK_DATA_IND(mSGpTR)   (mSGpTR)->ittiMsg.nas_dl_data_ind

//-------------------------------------------------------------------------------------------//
// eNB: ENB_APP -> RRC messages
typedef struct RrcConfigurationReq_s {
    uint32_t cell_identity;

    uint16_t tac;

    uint16_t mcc;
    uint16_t mnc;
} RrcConfigurationReq;

// UE: NAS -> RRC messages
typedef cell_info_req_t         NasCellSelectionReq;
typedef nas_establish_req_t     NasConnEstabliReq;
typedef ul_info_transfer_req_t  NasUlDataReq;

typedef rab_establish_rsp_t     NasRabEstRsp;

// UE: RRC -> NAS messages
typedef cell_info_cnf_t         NasCellSelectionCnf;
typedef cell_info_ind_t         NasCellSelectionInd;
typedef paging_ind_t            NasPagingInd;
typedef nas_establish_cnf_t     NasConnEstabCnf;
typedef nas_release_ind_t       NasConnReleaseInd;
typedef ul_info_transfer_cnf_t  NasUlDataCnf;
typedef dl_info_transfer_ind_t  NasDlDataInd;

#endif /* RRC_MESSAGES_TYPES_H_ */
