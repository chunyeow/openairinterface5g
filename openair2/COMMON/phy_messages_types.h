/*
 * phy_messages_types.h
 *
 *  Created on: Dec 12, 2013
 *      Author: winckel
 */

#ifndef PHY_MESSAGES_TYPES_H_
#define PHY_MESSAGES_TYPES_H_

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.
#define PHY_DEACTIVATE_REQ(mSGpTR)          (mSGpTR)->ittiMsg.phy_deactivate_req

#define PHY_FIND_CELL_REQ(mSGpTR)           (mSGpTR)->ittiMsg.phy_find_cell_req
#define PHY_FIND_NEXT_CELL_REQ(mSGpTR)      (mSGpTR)->ittiMsg.phy_find_next_cell_req

#define PHY_FIND_CELL_IND(mSGpTR)           (mSGpTR)->ittiMsg.phy_find_cell_ind

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
    Earfcn earfcn_start;
    Earfcn earfcn_end;
} PhyFindCellReq;

typedef struct PhyFindNextCellReq_s {
} PhyFindNextCellReq;

// UE: PHY -> RRC messages
typedef struct PhyFindCellInd_s {
   uint8_t      cell_nb;
   CellInfo     cells[MAX_REPORTED_CELL];
} PhyFindCellInd;
#endif /* PHY_MESSAGES_TYPES_H_ */
