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
#ifndef __NASUE_CONFIG_H__
#define __NASUE_CONFIG_H__

#ifdef MIH_USER_CONTROL
#define MIH_USER_IP_ADDRESS             "127.0.0.1"
#define MIH_USER_REMOTE_PORT            "0"
#define NAS_IP_ADDRESS                  "127.0.0.1"
#define NAS_LISTENING_PORT_FOR_MIH_USER "22222"
#define RSSI_INCREMENT_STEP             5
#endif
//#define SOCKET_RAL_TD_CDMA_NAME		"/tmp/graal"

#define MIHLink_MAX_LENGTH 500


//max number of UE per BS
// Warning : This must be coordinated with the number of FIFOs in the RG (rrc_sap.h)

// max number of cells or RGs
// to be coordinated with NAS list of RGs
#define maxCells 2


//// Daidalos II
//// parameters are defined in tables,
////

#define CONF_ACTIVATE_REQ   2
#define CONF_DEACTIVATE_REQ 3
#define CONF_ALL_MEAS_REQ   4
#define CONF_SEL_MEAS_REQ   5

#define CONF_NUM_MEAS 5
#define CONF_PERIOD_MEAS 30   // 30*100 = 3000 ms = 3 secs
        // MIHF measure every CONF_PERIOD_MEAS * 100 ms
        // Ex : CONF_PERIOD_MEAS = 20 ==> measure every 2 secs
#define CONF_TEST_DURATION 300  // 60*100 = 60000 ms = 60 secs
        // Actual duration = CONF_TEST_DURATION * 100 ms
        // must be a mulptiple of CONF_PERIOD_MEAS
#define CONF_TEST_COUNTER CONF_TEST_DURATION/CONF_PERIOD_MEAS

#define CONF_CURR_CELL_ID 5

// start NAS config
#ifdef CONF_NASUE_DUMMY

#define CONF_MT0 0
#define CONF_MT1 1

#define CONF_iid4 2
int conf_iid6_0[2] = {    406787,     406787};
int conf_iid6_1[2] = {1351685248, 1368462464};


#define CONF_rx_packets   1529
#define CONF_tx_packets    254
#define CONF_rx_bytes   157896
#define CONF_tx_bytes    32546
#define CONF_rx_errors      12
#define CONF_tx_errors       5
#define CONF_rx_dropped      0
#define CONF_tx_dropped     24

//rb_list
#define CONF_num_rb 2
int conf_rbId[CONF_num_rb]     = {12  ,14 };
int conf_qoSclass[CONF_num_rb] = {2   ,9  };
int conf_dscp[CONF_num_rb]     = {5   ,25 };

// measures
#define CONF_num_cells 3
int conf_cell_id[CONF_num_cells] = {CONF_CURR_CELL_ID, 8, 2 };
// LGint conf_level [CONF_num_cells] =  {50, 5, 20};
int conf_level [CONF_num_cells] =  {5, 50, 20};
int conf_provider_id [CONF_num_cells] = {25, 1 , 25 };

//float conf_MTU[3]         = {1000.,1100.,1200.};
#endif

#endif

