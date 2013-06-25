/*________________________openair_rrc_vars.h________________________

 Authors : Hicham Anouar
 Company : EURECOM
 Emails  : anouar@eurecom.fr
________________________________________________________________*/


#ifndef __OPENAIR_RRC_VARS_H__
#define __OPENAIR_RRC_VARS_H__
#include "defs.h"
#include "LAYER2/RLC/rlc.h"
#include "COMMON/mac_rrc_primitives.h"
#include "LAYER2/MAC/defs.h"

eNB_RRC_INST *eNB_rrc_inst;
UE_RRC_INST *UE_rrc_inst;
//RRC_XFACE *Rrc_xface;
#ifndef USER_MODE
//MAC_RLC_XFACE *Mac_rlc_xface;
#ifndef NO_RRM
int S_rrc= RRC2RRM_FIFO;
#endif //NO_RRM
//int R_rrc= RRM2RRC_FIFO;
#else
#include "LAYER2/MAC/extern.h"
#ifndef NO_RRM
sock_rrm_t S_rrc;
#endif 
#endif

#ifndef NO_RRM
#ifndef USER_MODE
char *Header_buf;
char *Data;
unsigned short Header_read_idx,Data_read_idx,Header_size;
#endif
unsigned short Data_to_read;
#endif //NO_RRM

u8 DRB2LCHAN[8];

long logicalChannelGroup0 = 0;
long  logicalChannelSR_Mask_r9=0;

struct LogicalChannelConfig__ul_SpecificParameters LCSRB1 =  {1,
							      LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
							      0,
							      &logicalChannelGroup0};
struct LogicalChannelConfig__ul_SpecificParameters LCSRB2 =  {3,
							      LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
							      0,
							      &logicalChannelGroup0};


// These are the default SRB configurations from 36.331 (Chapter 9, p. 176-179 in v8.6)
LogicalChannelConfig_t  SRB1_logicalChannelConfig_defaultValue = {&LCSRB1
#ifdef Rel10
								  ,
								  &logicalChannelSR_Mask_r9
#endif
                                                                 };

LogicalChannelConfig_t SRB2_logicalChannelConfig_defaultValue = {&LCSRB2
#ifdef Rel10
								 ,
								 &logicalChannelSR_Mask_r9
#endif
                                                                 };

//CONSTANTS
rlc_info_t Rlc_info_um,Rlc_info_am_config;
u16 RACH_FREQ_ALLOC;
//u8 NB_RACH;
LCHAN_DESC BCCH_LCHAN_DESC,CCCH_LCHAN_DESC,DCCH_LCHAN_DESC,DTCH_DL_LCHAN_DESC,DTCH_UL_LCHAN_DESC;
MAC_MEAS_T BCCH_MEAS_TRIGGER,CCCH_MEAS_TRIGGER,DCCH_MEAS_TRIGGER,DTCH_MEAS_TRIGGER;
MAC_AVG_T BCCH_MEAS_AVG, CCCH_MEAS_AVG,DCCH_MEAS_AVG, DTCH_MEAS_AVG;
#endif
