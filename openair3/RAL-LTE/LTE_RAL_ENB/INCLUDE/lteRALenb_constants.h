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
#define ENB_DEFAULT_3GPP_ADDRESS      "0000000000000000"


//-----------------------------------------------------------------------------
// Constants for scenario
#define PREDEFINED_MIH_NETWORK_ID "eurecom"
#define PREDEFINED_MIH_NETAUX_ID "netaux"

#define PREDEFINED_CHANNEL_ID 2
#define PREDEFINED_CLASSES_SERVICE_SUPPORTED 2
#define PREDEFINED_QUEUES_SUPPORTED 2

#endif
