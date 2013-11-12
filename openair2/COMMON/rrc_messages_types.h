/*
 * rrc_messages_types.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

#ifndef RRC_MESSAGES_TYPES_H_
#define RRC_MESSAGES_TYPES_H_

#include "BCCH-DL-SCH-Message.h"
#include "DL-CCCH-Message.h"
#include "DL-DCCH-Message.h"
#include "UE-EUTRA-Capability.h"
#include "UL-CCCH-Message.h"
#include "UL-DCCH-Message.h"

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.

#define RRC_MAC_IN_SYNC_IND(mSGpTR)             (mSGpTR)->msg.rrc_mac_in_sync_ind
#define RRC_MAC_OUT_OF_SYNC_IND(mSGpTR)         (mSGpTR)->msg.rrc_mac_out_of_sync_ind

#define RRC_MAC_BCCH_DATA_REQ(mSGpTR)           (mSGpTR)->msg.rrc_mac_bcch_data_req
#define RRC_MAC_BCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_bcch_data_ind

#define RRC_MAC_CCCH_DATA_REQ(mSGpTR)           (mSGpTR)->msg.rrc_mac_ccch_data_req
#define RRC_MAC_CCCH_DATA_CNF(mSGpTR)           (mSGpTR)->msg.rrc_mac_ccch_data_cnf
#define RRC_MAC_CCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_ccch_data_ind

#define RRC_MAC_MCCH_DATA_REQ(mSGpTR)           (mSGpTR)->msg.rrc_mac_mcch_data_req
#define RRC_MAC_MCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_mcch_data_ind

#define RRC_DCCH_DATA_REQ(mSGpTR)               (mSGpTR)->msg.rrc_dcch_data_req
#define RRC_DCCH_DATA_IND(mSGpTR)               (mSGpTR)->msg.rrc_dcch_data_ind

// Some constants from "LAYER2/MAC/defs.h"
#define BCCH_SDU_SIZE                           (128)
#define CCCH_SDU_SIZE                           (128)
#define MCCH_SDU_SIZE                           (128)

typedef BCCH_DL_SCH_Message_t   RrcDlBcchMessage;
typedef DL_CCCH_Message_t       RrcDlCcchMessage;
typedef DL_DCCH_Message_t       RrcDlDcchMessage;
typedef UE_EUTRA_Capability_t   RrcUeEutraCapability;
typedef UL_CCCH_Message_t       RrcUlCcchMessage;
typedef UL_DCCH_Message_t       RrcUlDcchMessage;

//-------------------------------------------------------------------------------------------//
// Messages from MAC layer

typedef struct {
  uint32_t frame;
  uint16_t enb_index;
} RrcMacInSyncInd;

typedef RrcMacInSyncInd RrcMacOutOfSyncInd;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t sdu[BCCH_SDU_SIZE];
  uint8_t enb_index;
} RrcMacBcchDataReq;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t sdu[BCCH_SDU_SIZE];
  uint8_t enb_index;
} RrcMacBcchDataInd;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t sdu[CCCH_SDU_SIZE];
  uint8_t enb_index;
} RrcMacCcchDataReq;

typedef struct {
  uint8_t enb_index;
} RrcMacCcchDataCnf;

typedef struct {
    uint32_t frame;
    uint32_t sdu_size;
    uint8_t sdu[CCCH_SDU_SIZE];
    uint8_t enb_index;
} RrcMacCcchDataInd;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t sdu[MCCH_SDU_SIZE];
  uint8_t enb_index;
  uint8_t mbsfn_sync_area;
} RrcMacMcchDataReq;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t sdu[MCCH_SDU_SIZE];
  uint8_t enb_index;
  uint8_t mbsfn_sync_area;
} RrcMacMcchDataInd;

//-------------------------------------------------------------------------------------------//
// Messages from/to PDCP layer

typedef struct {
  uint32_t frame;
  uint8_t enb_flag;
  uint32_t rb_id;
  uint32_t muip;
  uint32_t confirmp;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint8_t mode;
} RrcDcchDataReq;

typedef struct {
  uint32_t frame;
  uint8_t dcch_index;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint8_t ue_index;
} RrcDcchDataInd;

#endif /* RRC_MESSAGES_TYPES_H_ */
