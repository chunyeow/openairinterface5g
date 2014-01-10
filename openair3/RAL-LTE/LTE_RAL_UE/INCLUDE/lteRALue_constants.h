/***************************************************************************
                         lteRALue_constants.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALue_constants.h
 * \brief Constants for LTE-RAL-UE
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __LTERALUE_CONSTANTS_H__
#define __LTERALUE_CONSTANTS_H__
//-----------------------------------------------------------------------------
#define UE_DEFAULT_LOCAL_PORT_RAL    "1234"
#define UE_DEFAULT_REMOTE_PORT_MIHF  "1025"
#define UE_DEFAULT_IP_ADDRESS_MIHF   "127.0.0.1"
#define UE_DEFAULT_IP_ADDRESS_RAL    "127.0.0.1"
#define UE_DEFAULT_LINK_ID_RAL       "ue_lte_link"
#define UE_DEFAULT_LINK_ADDRESS_RAL  "060080149150"
#define UE_DEFAULT_MIHF_ID           "mihf2_ue"
#define DEFAULT_ADDRESS_3GPP      "0335060080149150"
#define DEFAULT_ADDRESS_eNB       "0000000000000001"

#define USE_3GPP_ADDR_AS_LINK_ADDR 1
//-----------------------------------------------------------------------------
// Constants for scenario
#define PREDEFINED_MIH_PLMN_ID    503
#define PREDEFINED_MIH_NETWORK_ID "eurecom"
#define PREDEFINED_MIH_NETAUX_ID  "netaux"

#define PREDEFINED_CHANNEL_ID 2
#define PREDEFINED_CLASSES_SERVICE_SUPPORTED 2
#define PREDEFINED_QUEUES_SUPPORTED 2

#endif
