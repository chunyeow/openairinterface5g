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

