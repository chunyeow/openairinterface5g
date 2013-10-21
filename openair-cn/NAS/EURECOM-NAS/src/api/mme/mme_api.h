/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		mme_api.h

Version		0.1

Date		2013/02/28

Product		NAS stack

Subsystem	Application Programming Interface

Author		Frederic Maurel

Description	Implements the API used by the NAS layer running in the MME
		to interact with a Mobility Management Entity

*****************************************************************************/
#ifndef __MME_API_H__
#define __MME_API_H__

#ifdef NAS_MME
#include "commonDef.h"
#include "securityDef.h"
#include "OctetString.h"
#endif

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* Maximum number of UEs the MME may simultaneously support */
#define MME_API_NB_UE_MAX		1

#ifdef NAS_MME

/* Features supported by the MME */
typedef enum {
    MME_API_NO_FEATURE_SUPPORTED	= 0,
    MME_API_EMERGENCY_ATTACH		= (1<<0),
    MME_API_UNAUTHENTICATED_IMSI	= (1<<1),
    MME_API_IPV4			= (1<<2),
    MME_API_IPV6			= (1<<3),
    MME_API_SINGLE_ADDR_BEARERS		= (1<<4),
} mme_api_feature_t;

/*
 * EPS Mobility Management configuration data
 * ------------------------------------------
 */
typedef struct {
    mme_api_feature_t features;	/* Supported features			*/
    gummei_t          gummei;	/* EPS Globally Unique MME Identity	*/
} mme_api_emm_config_t;

/*
 * EPS Session Management configuration data
 * -----------------------------------------
 */
typedef struct {
    mme_api_feature_t features;	/* Supported features			*/
} mme_api_esm_config_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/* EPS subscribed QoS profile  */
typedef struct {
#define MME_API_UPLINK		0
#define MME_API_DOWNLINK	1
#define MME_API_DIRECTION	2
    int gbr[MME_API_DIRECTION];	/* Guaranteed Bit Rate			*/
    int mbr[MME_API_DIRECTION];	/* Maximum Bit Rate			*/
    int qci;			/* QoS Class Identifier			*/
} mme_api_qos_t;

/* Traffic Flow Template */
typedef struct {
} mme_api_tft_t;

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

int mme_api_get_emm_config(mme_api_emm_config_t* config);
int mme_api_get_esm_config(mme_api_esm_config_t* config);

int mme_api_identify_guti(const GUTI_t* guti, auth_vector_t* vector);
int mme_api_identify_imsi(const imsi_t* imsi, auth_vector_t* vector);
int mme_api_identify_imei(const imei_t* imei, auth_vector_t* vector);
int mme_api_new_guti(const imsi_t* imsi, GUTI_t* guti, tac_t* tac, int* n_tacs);

int mme_api_subscribe(OctetString* apn, OctetString* pdn_addr, int is_emergency, mme_api_qos_t* qos);
int mme_api_unsubscribe(OctetString* apn);

#endif // NAS_MME

#endif /* __MME_API_H__*/
