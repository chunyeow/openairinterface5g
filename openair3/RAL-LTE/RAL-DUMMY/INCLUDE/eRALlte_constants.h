/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

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
