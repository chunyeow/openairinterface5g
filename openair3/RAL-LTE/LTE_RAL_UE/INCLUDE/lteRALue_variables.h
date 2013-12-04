/***************************************************************************
                         lteRALue_variables.h  -  description
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
/*! \file lteRALue_mih_msg.h
 * \brief This file defines the data structures and LTE-RAL-UE protocol parameters
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __LTERALUE_VARIABLES_H__
#define __LTERALUE_VARIABLES_H__
//-------------------------------------------------------------------

#include "lteRALue.h"

/***************************************************************************
     CONSTANTS
 ***************************************************************************/
//#define RAL_DUMMY
#define RAL_REALTIME
//#define MRALU_SIMU_LINKDOWN
//#define DEBUG_MRALU_MEASURES
//#define UPDATE


#define RALU_MT_EVENT_MAP MIH_EVENT_Link_AdvertisementIndication + MIH_EVENT_Link_Parameters_ReportIndication + MIH_EVENT_Link_UpIndication + MIH_EVENT_Link_DownIndication

#define MIHLink_MAX_LENGTH 500
#define MAX_NUMBER_BS   3  /* Max number of base stations */
#define CONF_UNKNOWN_CELL_ID 0
#define UMTS_INTF_NAME "graal0"

//Attachment status
#define UNKNOWN         9
#define CONNECTED       1
#define DISCONNECTED    0
//max number RB supported  (27 in Spec)
#define RAL_MAX_RB 5

// constant for integrating measures according to the formula
// y[n] = (1-LAMBDA)*u[n-1] + LAMBDA*u[n]
#define LAMBDA 8
#define MEAS_MAX_RSSI 110

/***************************************************************************
     VARIABLES
 ***************************************************************************/
//struct ral_lte_priv {

//};

//-----------------------------------------------------------------------------
//extern struct ral_lte_priv *ralpriv;

#endif

