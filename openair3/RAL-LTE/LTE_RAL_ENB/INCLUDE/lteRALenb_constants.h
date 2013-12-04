/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source lteRALenb_constants.h
 *
 * Version 0.1
 *
 * Date  06/26/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem RAL-LTE constants
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description Defines constants used by the RAL-LTE process at the network
 *  side.
 *
 *****************************************************************************/
#ifndef __LTE_RAL_ENB_CONSTANTS_H__
#define __LTE_RAL_ENB_CONSTANTS_H__

#include <sys/types.h>

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

//-----------------------------------------------------------------------------
#define ENB_DEFAULT_LOCAL_PORT_RAL    "1235"
#define ENB_DEFAULT_REMOTE_PORT_MIHF  "1025"
#define ENB_DEFAULT_IP_ADDRESS_MIHF   "127.0.0.1"
#define ENB_DEFAULT_IP_ADDRESS_RAL    "127.0.0.1"
#define ENB_DEFAULT_LINK_ID_RAL       "enb_lte_link"
#define ENB_DEFAULT_LINK_ADDRESS_RAL  "060080149150"
#define ENB_DEFAULT_MIHF_ID           "mihf_enb"
#define ENB_DEFAULT_ADDRESS_3GPP      "0335060080149150"


//-----------------------------------------------------------------------------
// Constants for scenario
#define PREDEFINED_MIH_NETWORK_ID "eurecom"
#define PREDEFINED_MIH_NETAUX_ID "netaux"

#define PREDEFINED_CHANNEL_ID 2
#define PREDEFINED_CLASSES_SERVICE_SUPPORTED 2
#define PREDEFINED_QUEUES_SUPPORTED 2

#endif
