/**************************************************************************
                          mRALlte_constants.h
                             -------------------
    copyright            : (C) 2011 by Eurecom
    email                : lionel.gauthier@eurecom.fr; michelle.wetterwald@eurecom.fr
***************************************************************************
mRALlte Constants
***************************************************************************/
#ifndef __MRALLTE_CONSTANTS_H__
#define __MRALLTE_CONSTANTS_H__
//-----------------------------------------------------------------------------
#define DEFAULT_LOCAL_PORT_RAL    "1235"
#define DEFAULT_REMOTE_PORT_MIHF  "1025"
#define DEFAULT_IP_ADDRESS_MIHF   "127.0.0.1"
#define DEFAULT_IP_ADDRESS_RAL    "127.0.0.1"
#define DEFAULT_LINK_ID           "link" //"lte"
#define DEFAULT_MIHF_ID           "mihf1"
#define DEFAULT_ADDRESS_3GPP      "0335060080149150"

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

#define PREDEFINED_LINK_GOING_DOWN_INDICATION_SIG_STRENGTH 20
#define PREDEFINED_LINK_DETECTED_INDICATION_SIG_STRENGTH 10
#define PREDEFINED_LINK_DETECTED_INDICATION_SINR 45
#define PREDEFINED_LINK_DETECTED_INDICATION_LINK_DATA_RATE 1000

#endif
