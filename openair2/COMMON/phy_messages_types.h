/*
 * phy_messages_types.h
 *
 *  Created on: Dec 12, 2013
 *      Author: winckel
 */

#ifndef PHY_MESSAGES_TYPES_H_
#define PHY_MESSAGES_TYPES_H_

#include "ral_messages_types.h"
//-------------------------------------------------------------------------------------------//
// Defines to access message fields.
#define PHY_DEACTIVATE_REQ(mSGpTR)          (mSGpTR)->ittiMsg.phy_deactivate_req

#define PHY_FIND_CELL_REQ(mSGpTR)           (mSGpTR)->ittiMsg.phy_find_cell_req
#define PHY_FIND_NEXT_CELL_REQ(mSGpTR)      (mSGpTR)->ittiMsg.phy_find_next_cell_req

#define PHY_FIND_CELL_IND(mSGpTR)           (mSGpTR)->ittiMsg.phy_find_cell_ind

#define PHY_MEAS_THRESHOLD_REQ(mSGpTR)      (mSGpTR)->ittiMsg.phy_meas_threshold_req
#define PHY_MEAS_THRESHOLD_CONF(mSGpTR)     (mSGpTR)->ittiMsg.phy_meas_threshold_conf
#define PHY_MEAS_REPORT_IND(mSGpTR)         (mSGpTR)->ittiMsg.phy_meas_report_ind
//-------------------------------------------------------------------------------------------//
#define MAX_REPORTED_CELL   10

/* Enhance absolute radio frequency channel number */
typedef uint16_t    Earfcn;

/* Physical cell identity, valid value are from 0 to 503 */
typedef int16_t     PhyCellId;

/* Reference signal received power, valid value are from 0 (rsrp < -140 dBm) to 97 (rsrp <= -44 dBm) */
typedef int8_t      Rsrp;

/* Reference signal received quality, valid value are from 0 (rsrq < -19.50 dB) to 34 (rsrq <= -3 dB) */
typedef int8_t      Rsrq;

typedef struct CellInfo_s {
    Earfcn      earfcn;
    PhyCellId   cell_id;
    Rsrp        rsrp;
    Rsrq        rsrq;
} CellInfo;

//-------------------------------------------------------------------------------------------//
// UE: RRC -> PHY messages
typedef struct PhyDeactivateReq_s {
} PhyDeactivateReq;

typedef struct PhyFindCellReq_s {
//#   if defined(ENABLE_RAL)
    ral_transaction_id_t  transaction_id;
//#   endif
    Earfcn earfcn_start;
    Earfcn earfcn_end;
} PhyFindCellReq;

typedef struct PhyFindNextCellReq_s {
} PhyFindNextCellReq;


typedef struct PhyMeasThresholdReq_s {
    ral_transaction_id_t      transaction_id;
    ral_link_cfg_param_t      cfg_param;
} PhyMeasThresholdReq;

typedef struct PhyMeasReportInd_s {
    ral_threshold_t            threshold;
    ral_link_param_t           link_param;
} PhyMeasReportInd;

// UE: PHY -> RRC messages
typedef struct PhyFindCellInd_s {
//#   if defined(ENABLE_RAL)
    ral_transaction_id_t  transaction_id;
//#   endif
   uint8_t      cell_nb;
   CellInfo     cells[MAX_REPORTED_CELL];
} PhyFindCellInd;

typedef struct PhyMeasThresholdConf_s {
    ral_transaction_id_t     transaction_id;
    ral_status_t             status;
    uint8_t                  num_link_cfg_params;
    ral_link_cfg_status_t    cfg_status[RAL_MAX_LINK_CFG_PARAMS];
}PhyMeasThresholdConf;
#endif /* PHY_MESSAGES_TYPES_H_ */
