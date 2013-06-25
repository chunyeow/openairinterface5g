/*****************************************************************************
 *			Eurecom OpenAirInterface 3
 * 			Copyright(c) 2012 Eurecom
 *
 * Source	eRALlte_constants.h
 *
 * Version	0.1
 *
 * Date		06/26/2012
 *
 * Product	MIH RAL LTE
 *
 * Subsystem	RAL-LTE constants
 *
 * Authors	Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description	Defines constants used by the RAL-LTE process at the network
 *		side.
 *
 *****************************************************************************/

#ifndef __RAL_LTE_CONS_H__
#define __RAL_LTE_CONS_H__

#include <sys/types.h>

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

//-----------------------------------------------------------------------------
#define DEFAULT_LOCAL_PORT_RAL    "1235"
#define DEFAULT_REMOTE_PORT_MIHF  "1025"
#define DEFAULT_IP_ADDRESS_MIHF   "127.0.0.1"
#define DEFAULT_IP_ADDRESS_RAL    "127.0.0.1"
#define DEFAULT_LINK_ID           "link"
#define DEFAULT_MIHF_ID           "mihf1"
#define DEFAULT_ADDRESS_3GPP      "0335060080149150"

#define DEFAULT_PLMN		  "208020"
#define DEFAULT_MCC		  "208"		// France
#define DEFAULT_MNC		  "020"		// Bouygues Telecom

//-----------------------------------------------------------------------------
#define MIH_C_RADIO_POLLING_INTERVAL_MICRO_SECONDS 50000
#define MIH_C_RADIO_POLLING_INTERVAL_SECONDS       0

//-----------------------------------------------------------------------------
// Constants for scenario
#define PREDEFINED_MIH_NETWORK_ID "eurecom"
#define PREDEFINED_MIH_NETAUX_ID "netaux"

#define PREDEFINED_CHANNEL_ID 2
#define PREDEFINED_CLASSES_SERVICE_SUPPORTED 2
#define PREDEFINED_QUEUES_SUPPORTED 2

#define PREDEFINED_LINK_DETECTED_INDICATION_SIG_STRENGTH 5
#define PREDEFINED_LINK_DETECTED_INDICATION_SINR 45
#define PREDEFINED_LINK_DETECTED_INDICATION_LINK_DATA_RATE 1000

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif
