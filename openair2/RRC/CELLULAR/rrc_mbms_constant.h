/***************************************************************************
                           rrc_mbms_constant.h
                          -------------------
   begin                : May 4, 2005
   copyright            : (C) 2002, 2010 by Eurecom
   author               : Michelle.Wetterwald@eurecom.fr, Huu-Nghia.Nguyen@eurecom.fr
   goal                 : This file contains all the constants concerned with MBMS
**************************************************************************/
#ifndef _RRC_MBMS_CONSTANT_H_
#define _RRC_MBMS_CONSTANT_H_
#include "rrc_platform_types.h"

#if defined(FALSE) && (FALSE != 0)
#error FALSE was defined elsewhere with a none zero value  !!! 
#endif

/********************************/
/*  MBMS impl. specific types   */
typedef int o3;  /* OctetString(3) */
typedef  u8 mbms_bool; /* mbms_bool used in RRC MBMS */


/********************************/
/*  MBMS impl. constants        */
//Data size
#define MBMS_MAX_BUFFER_SIZE 512
//#define MBMS_SERVICE_IDENTITY_SIZE  3
#define MBMS_MIN_TRANSACTION_ID 3
#define MBMS_DEFAULT_CGID 1 // default Cell Group Identity

//Channels for the test
#define MCCH_ID 0
#define DCCH_ID 1
#define MSCH_ID 2

//States for FSM
#define ACQUI_START  0
#define ACQUI_I_ACQUISITION 1
#define ACQUI_WAIT_MBMS_MSG 2

//MBMS Message header + length
#define MCCH_MSG_HEAD_LGTH           4 //type only, no integrity check
#define MSCH_MSG_HEAD_LGTH           4 //type only, no integrity check
#define MBMS_MAX_NEIGBOURING_COUNT   4 // Temp
#define MBMS_L12_CONFIGURATION_SIZE  200
#define NEIGHBOUR_CONFIGURATION_SIZE 200 //Size of neighbouring cell's configuration
#define MTCH_CONFIGURATION_SIZE      400
//Default value for Scheduling
#define ACCESS_PERIOD_COEF_DEFAULT    2  //[0..3] HGN 2
#define REPETITION_PERIOD_COEF_DEFAULT 1  //[0..3] HGN 3
#define MODIF_PERIOD_COEF_DEFAULT       7  //[7..10] HGN 7
#define SCHED_PERIOD_COEF_DEFAULT       3

//SHOULD BE IDENTICAL WITH MAX_MBMS_SERVICES in rrc_nas_primitives.h
#define maxMBMSServices  4

//Service Type defined in MBMS Activated Services List
//FFS
#define MBMS_MULTICAST  5
#define MBMS_BROADCAST  8

/********************************/
/*  Defined in TS 25.331        */

//Values of T_318, use  these constants instead of enumeration in standard T25.331
#define ms250   250
#define ms500   500
#define ms750   750
#define ms1000  1000
#define ms1250  1250
#define ms1500  1500
#define ms1750  1750
#define ms2000  2000
#define ms3000  3000
#define ms4000  4000
#define ms6000  6000
#define ms8000  8000
#define ms10000 10000
#define ms12000 12000
#define ms16000 16000   

//Values of Access Probability Factor. use these constants instead of enumertion in TS25.331    
#define apf0 0
#define apf32 32
#define apf64 64
#define apf96 96
#define apf128 128
#define apf160 160
#define apf192 192
#define apf224 224
#define apf256 256
#define apf288 288
#define apf320 320
#define apf352 352
#define apf384 384
#define apf416 416
#define apf448 448
#define apf480 480
#define apf512 512
#define apf544 544
#define apf576 576
#define apf608 608
#define apf640 640
#define apf672 672
#define apf704 704
#define apf736 736
#define apf768 768
#define apf800 800
#define apf832 832
#define apf864 864
#define apf896 896
#define apf928 928
#define apf960 960
#define apf1000 1000

//Values of MBMS_RequiredUEAction_Mod, use these constants instead of enum in TS25.331
#define Mod_none                   0
#define Mod_acquireCountingInfo    1 //not on DCCH
#define Mod_acquirePTM_RBInfo      2
#define Mod_establishPMMConnection 3 //not used
#define Mod_releasePTM_RB          4

//Values of MBMS_RequiredUEAction_UMod. use these constants instead of enum in TS25.331
#define UMod_none                   0
#define UMod_acquirePTM_RBInfo      1
#define UMod_establishPMMConnection 2
#define UMod_releasePTM_RB          3

//#define maxMBMS_CommonCCTrCh  32
//#define maxMBMS_CommonPhyCh   32
//#define maxMBMS_CommonRB      32
//#define maxMBMS_CommonTrCh    32
#define maxMBMSservCount    4  // spec v6.a.0 = 8
//#define maxMBMSservDedic    4 // spec v6.a.0 removed
#define maxMBMSservModif    4  // spec v6.a.0 = 32
#define maxMBMSservUnmodif  4  // spec v6.a.0 = 64
#define maxMBMSservSched    4  // spec v6.a.0 = 16

#define maxMBMSTransmis     4

#endif //_RRC_MBMS_CONSTANT_H_
