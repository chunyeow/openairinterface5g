/*
 * rrc_messages_types.h
 *
 *  Created on: Oct 24, 2013
 *      Author: winckel
 */

#ifndef RRC_MESSAGES_TYPES_H_
#define RRC_MESSAGES_TYPES_H_

#include "UE-EUTRA-Capability.h"

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.

#define RRC_MAC_IN_SYNC_IND(mSGpTR)             (mSGpTR)->msg.rrc_mac_in_sync_ind
#define RRC_MAC_OUT_OF_SYNC_IND(mSGpTR)         (mSGpTR)->msg.rrc_mac_out_of_sync_ind
#define RRC_MAC_BCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_bcch_data_ind
#define RRC_MAC_CCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_ccch_data_ind
#define RRC_MAC_CCCH_SUCCESS_IND(mSGpTR)        (mSGpTR)->msg.rrc_mac_ccch_success_ind
#define RRC_MAC_MCCH_DATA_IND(mSGpTR)           (mSGpTR)->msg.rrc_mac_mcch_data_ind

#define RRC_DCCH_DATA_REQ(mSGpTR)               (mSGpTR)->msg.rrc_dcch_data_req
#define RRC_DCCH_DATA_IND(mSGpTR)               (mSGpTR)->msg.rrc_dcch_data_ind

typedef UE_EUTRA_Capability_t RrcUeEutraCapability;

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
  uint8_t *sdu_p;
  uint8_t enb_index;
} RrcMacBcchDataInd;

typedef RrcMacBcchDataInd RrcMacCcchDataInd;

typedef struct {
  uint8_t enb_index;
} RrcMacCcchSuccessInd;

typedef struct {
  uint32_t frame;
  uint32_t sdu_size;
  uint8_t *sdu_p;
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
