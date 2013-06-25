/***************************************************************************
                          rrc_bch_per_basic.h  -  description
                             -------------------
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define basic types for RRC peer-to-peer messages and broadcast
 ***************************************************************************/
#ifndef __RRC_BCH_PER_BASIC_H__
#define __RRC_BCH_PER_BASIC_H__

#include "rrc_platform_types.h"
typedef int     Digit;

//Return codes
#define P_SUCCESS       0
#define P_MISCLERROR    1
#define P_OUTOFBOUNDS   2
#define P_INVDIGIT      3
#define P_INVMSGTYPE    4
#define P_NOMSG         5
#define P_INVVALUE      6
#define P_INVINTEGRITY  7
#define P_INVPROC       8

#endif
