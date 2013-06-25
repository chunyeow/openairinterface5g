/***************************************************************************
                          rrc_bch_constant.h  -  description
                             -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for Broadcast MIB and SIBs
 ***************************************************************************/
#ifndef __RRC_BCH_CONSTANT_H__
#define __RRC_BCH_CONSTANT_H__

#include "rrc_bch_per_basic.h"

#define maxBlock 210
typedef char    ENCODEDBLOCK[maxBlock];
#define maxSI 30            // (262-16)bits/8 = 30.75
typedef char    ENCODEDSI[maxSI];
#define LSIBfixed 24        //= 30-4-2 * spec=28 when PER unaligned
#define LSIBcompl 25        //= 30-4-1 * spec=29 when PER unaligned

#define supportedSIBs 6     //with value_tags [SIBs 1,2,5,11,18] - 14 is time-out

#define maxSIB1NAS 200      // max num of bytes for NAS data in SIB 1
#define maxSIBNAS 110       // max num of bytes for NAS data in SIB 18


typedef struct {                /* PER control block         */
  ENCODEDBLOCK   *buffer;       /* start of data buffer               */
  int             buff_index;   /* index for next data                */
  int             buff_size;    /* size of data in the encoded buffer */
  int             bitoffset;    /* for PER unaligned - FFS            */
  ENCODEDBLOCK   *data;         /* start of encoded data              */
  int             data_size;    /* size of encoded data               */
  int             data_offset;  /* in bytes - PER unaligned is FFS    */
  int             errInfo;      /* error info                         */
} PERParms;

#define NO_BLOCK  0xFF

// Default broadcast scheduling
/*
  Rules : Repetition must be a constant as defined below
          0 <= position <= 2 pow sib_rep -1
  eg : sib_rep = 4096, position<= 2047
  Current definition  MIB: Rep32, pos 0
  Long SIBs (>5segments) Rep128, pos 7 [9 segments available]: SIB1, SIB5, SIB18
  Short SIBs (<=5 segments) Rep64, pos 3 [5 segments available]: SIB2, SIB11?, SIB14?
*/
#define LONG_SIB_POS 7
#define SHORT_SIB_POS 2

#define MIB_REP  SchedulingInformation_scheduling_rep32
#define MIB_POS  0
#define SIB1_REP  SchedulingInformation_scheduling_rep64    //128
#define SIB1_POS  LONG_SIB_POS
//#define SIB2_REP  SchedulingInformation_scheduling_rep64
//#define SIB2_POS  SHORT_SIB_POS
#define SIB2_REP  SchedulingInformation_scheduling_norep
#define SIB2_POS  0
#define SIB5_REP  SchedulingInformation_scheduling_rep64    //128
#define SIB5_POS  LONG_SIB_POS + 16 //=(64/2)/2 +7
#define SIB11_REP  SchedulingInformation_scheduling_norep
#define SIB11_POS  0
#define SIB14_REP  SchedulingInformation_scheduling_rep64
#define SIB14_POS  SHORT_SIB_POS + 2
#define SIB14_TFACTOR 7
#define SIB18_REP  SchedulingInformation_scheduling_rep64
#define SIB18_POS  SHORT_SIB_POS + 16


//Temp
#define maxSIBperMsg 16
//#define P_SUCCESS       0
//#define P_MISCLERROR    1
//#define P_OUTOFBOUNDS   2
//#define P_INVDIGIT      3

/* Segment Combination Types */
#define  BCH_noSegment 1
#define  BCH_firstSegment 2
#define  BCH_subsequentSegment 3
#define  BCH_lastSegmentShort 4
#define  BCH_lastAndFirst 5
#define  BCH_lastAndComplete 6
#define  BCH_lastAndCompleteAndFirst 7
#define  BCH_completeSIB_List 8
#define  BCH_completeAndFirst 9
#define  BCH_completeSIB 10
#define  BCH_lastSegment 11

/*  PLMN_Type    */
#define PLMN_gsm_MAP             1
#define PLMN_ansi_41             2
#define PLMN_gsm_MAP_and_ANSI_41 3
#define PLMN_MobyDick            4

/* SIBSb tag constants */
#define SIBSb_sysInfoType1 1
#define SIBSb_sysInfoType2 2
#define SIBSb_sysInfoType3 3
#define SIBSb_sysInfoType4 4
#define SIBSb_sysInfoType5 5
#define SIBSb_sysInfoType6 6
#define SIBSb_sysInfoType7 7
#define SIBSb_sysInfoType8 8
#define SIBSb_sysInfoType9 9
#define SIBSb_sysInfoType10 10
#define SIBSb_sysInfoType11 11
#define SIBSb_sysInfoType12 12
#define SIBSb_sysInfoType13 13
#define SIBSb_sysInfoType13_1 14
#define SIBSb_sysInfoType13_2 15
#define SIBSb_sysInfoType13_3 16
#define SIBSb_sysInfoType13_4 17
#define SIBSb_sysInfoType14 18
#define SIBSb_sysInfoType15 19
#define SIBSb_sysInfoType16 20
#define SIBSb_sysInfoType17 21
#define SIBSb_sysInfoTypeSB1 22
#define SIBSb_sysInfoTypeSB2 23
#define SIBSb_sysInfoType15_1 24
#define SIBSb_sysInfoType15_2 25
#define SIBSb_sysInfoType15_3 26
#define SIBSb_sysInfoType15_4 27
#define SIBSb_sysInfoType18 28
#define SIBSb_sysInfoType15_5 29

/*  SchedulingInformation   Sib_Pos  */
#define SchedulingInformation_scheduling_norep    0
#define SchedulingInformation_scheduling_rep4     1
#define SchedulingInformation_scheduling_rep8     2
#define SchedulingInformation_scheduling_rep16    3
#define SchedulingInformation_scheduling_rep32    4
#define SchedulingInformation_scheduling_rep64    5
#define SchedulingInformation_scheduling_rep128   6
#define SchedulingInformation_scheduling_rep256   7
#define SchedulingInformation_scheduling_rep512   8
#define SchedulingInformation_scheduling_rep1024  9
#define SchedulingInformation_scheduling_rep2048 10
#define SchedulingInformation_scheduling_rep4096 11

#endif
