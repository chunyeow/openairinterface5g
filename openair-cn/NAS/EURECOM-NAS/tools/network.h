/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		network.h

Version		0.1

Date		2013/03/26

Product		USIM data generator

Subsystem	PLMN network operators

Author		Frederic Maurel

Description	Defines a list of PLMN network operators

*****************************************************************************/
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "commonDef.h"
#include "networkDef.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

#define SFR_PLMN_1	{0,2,0x0f,8,0,1}	// 20810
#define SFR_PLMN_2	{0,2,0x0f,8,1,1}	// 20811
#define SFR_PLMN_3	{0,2,0x0f,8,3,1}	// 20813
#define VDF_PLMN_1	{2,2,0x0f,2,0,1}	// 22210
#define VDF_PLMN_2	{1,2,0x0f,4,0x0f,1}	// 2141
#define VDF_PLMN_3	{1,2,0x0f,4,0x0f,6}	// 2146
#define VDF_PLMN_4	{6,2,0x0f,2,0x0f,2}	// 2622
#define VDF_PLMN_5	{6,2,0x0f,2,0x0f,4}	// 2624

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/*
 * PLMN network operator record index
 */
enum {
    SFR1=0,
    SFR2,
    SFR3,
    VDF1,
    VDF2,
    VDF3,
    VDF4,
    VDF5
};

/*
 * PLMN network operator record
 */
typedef struct {
    unsigned int num;
    plmn_t plmn;
    char fullname[NET_FORMAT_LONG_SIZE + 1];
    char shortname[NET_FORMAT_SHORT_SIZE + 1];
    tac_t tac_start;
    tac_t tac_end;
} network_record_t;


/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/*
 * The list of PLMN network operator records
 */
network_record_t network_records[] = {
    {20810, SFR_PLMN_1, "SFR France", "SFR", 0x0000, 0xfffe},
    {20811, SFR_PLMN_2, "SFR France", "SFR", 0x0000, 0xfffe},
    {20813, SFR_PLMN_3, "SFR France", "SFR", 0x0000, 0xfffe},
    {22210, VDF_PLMN_1, "Vodafone Italia", "VODAFONE", 0x0000, 0xfffe},
    {2141,  VDF_PLMN_2, "Vodafone Spain", "VODAFONE", 0x0000, 0xfffe},
    {2146,  VDF_PLMN_3, "Vodafone Spain", "VODAFONE", 0x0000, 0xfffe},
    {2622,  VDF_PLMN_4, "Vodafone Germ", "VODAFONE", 0x0000, 0xfffe},
    {2624,  VDF_PLMN_5, "Vodafone Germ", "VODAFONE", 0x0000, 0xfffe},
};

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif /* __NETWORK_H__*/
