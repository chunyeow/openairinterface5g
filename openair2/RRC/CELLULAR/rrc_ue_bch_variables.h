/***************************************************************************
                          rrc_ue_bch_variables.h  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Some additional definitions for UE broadcast
 ***************************************************************************/
#ifndef __RRC_UE_BCH_H__
#define __RRC_UE_BCH_H__

#include "rrc_bch_mib.h"

#include "SystemInformationBlockType2.h"
#include "SystemInformation.h"
#include "BCCH-DL-SCH-Message.h"
#include "TDD-Config.h"

struct rrc_ue_bch_value_tags {
  int             mib_vt;
  int             sib1_vt;
  int             sib2_vt;
  int             sib5_vt;
  int             sib11_vt;
  int             sib18_vt;
};

struct rrc_ue_bch_blocks {
  struct rrc_ue_bch_value_tags bch_ue_vts;
  PERParms        perParms;
  int             next_block_type;
  int             curr_block_type;
  int             curr_block_index;
  int             curr_block_length;
  int             curr_segment_index;;
  int             curr_segment_count;
  int             SIB14_timeout;
  int             SIB14_timeout_value;
  SystemInformation_BCH currSI_BCH;
  ENCODEDSI       encoded_currSIBCH;
  ENCODEDSI       encoded_prevSIBCH;
  MasterInformationBlock currMIB;
  int             encodedMIB_lgth;
  ENCODEDBLOCK    encoded_currMIB;
  ENCODEDBLOCK    encoded_prevMIB;
  struct SysInfoType1 currSIB1;
  int             encodedSIB1_lgth;
  ENCODEDBLOCK    encoded_currSIB1;
  ENCODEDBLOCK    encoded_prevSIB1;
  struct SysInfoType2 currSIB2;
  int             encodedSIB2_lgth;
  ENCODEDBLOCK    encoded_currSIB2;
  ENCODEDBLOCK    encoded_prevSIB2;
  struct SysInfoType5 currSIB5;
  int             encodedSIB5_lgth;
  ENCODEDBLOCK    encoded_currSIB5;
  ENCODEDBLOCK    encoded_prevSIB5;
  //struct SysInfoType11 currSIB11;
  int             encodedSIB11_lgth;
  ENCODEDBLOCK    encoded_currSIB11;
  ENCODEDBLOCK    encoded_prevSIB11;
  struct SysInfoType14 currSIB14;
  int             encodedSIB14_lgth;
  ENCODEDBLOCK    encoded_currSIB14;
  ENCODEDBLOCK    encoded_prevSIB14;
  struct SysInfoType18 currSIB18;
  int             encodedSIB18_lgth;
  ENCODEDBLOCK    encoded_currSIB18;
  ENCODEDBLOCK    encoded_prevSIB18;
};


#endif
