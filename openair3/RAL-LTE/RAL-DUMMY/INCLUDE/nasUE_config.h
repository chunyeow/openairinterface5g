/***************************************************************************
                          nasUE_config.h  -  description
                             -------------------
    copyright            : (C) 2005, 2007 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
 ***************************************************************************
 Dummy NAS UE configuration- Test interface with A21_MT_RAL_UMTS
 ***************************************************************************/
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

