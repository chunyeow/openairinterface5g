/***************************************************************************
                          rrc_rrm_primitives.h
                          -------------------
    copyright            : (C) 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Definition of RRC external interface to RRM
 **************************************************************************/
#ifndef __RRC_RRMPRIM_H__
#define __RRC_RRMPRIM_H__

#include "rrm_constants.h"

//----------------------------------------------------------
// Primitives
//----------------------------------------------------------
// LG : IT IS MANDATORY THAT ALL MESSAGES ARE INDEXED FROM
// 0 TO N WHITH NO HOLE IN NUMEROTATION -> ARRAY OF POINTERS
#define RPC_CONNECTION_REQUEST    0
#define RPC_CONNECTION_RESPONSE   1
#define RPC_CONNECTION_CLOSE      2
#define RPC_CONNECTION_CLOSE_ACK  3
#define RPC_ADD_USER_REQUEST      4
#define RPC_ADD_USER_RESPONSE     5
#define RPC_ADD_USER_CONFIRM      6
#define RPC_REMOVE_USER_REQUEST   7
#define RPC_REMOVE_USER_RESPONSE  8
#define RPC_ADD_RADIO_ACCESS_BEARER_REQUEST       9
#define RPC_ADD_RADIO_ACCESS_BEARER_RESPONSE     10
#define RPC_ADD_RADIO_ACCESS_BEARER_CONFIRM      11
#define RPC_REMOVE_RADIO_ACCESS_BEARER_REQUEST   12
#define RPC_REMOVE_RADIO_ACCESS_BEARER_RESPONSE  13
#define RPC_MEASUREMENT_REQUEST                  14
#define RPC_L1_MEASUREMENT_RG_INTERNAL_REPORT        15
#define RPC_L1_MEASUREMENT_RG_QUALITY_REPORT         16
#define RPC_L1_MEASUREMENT_RG_TRAFFIC_VOLUME_REPORT  17
#define RPC_L1_MEASUREMENT_RG_PHYSICAL_CHANNEL_DATA_AMP_REPORT  18
#define RPC_L1_MEASUREMENT_MT_INTERNAL_REPORT                   19
#define RPC_L1_MEASUREMENT_MT_QUALITY_REPORT                    20
#define RPC_L1_MEASUREMENT_MT_TRAFFIC_VOLUME_REPORT             21
#define RPC_L1_MEASUREMENT_MT_INTRA_FREQUENCY_REPORT            22
#define RPC_UPLINK_PHYSICAL_CHANNEL_CONTROL                     23
#define RPC_CONFIG_REQUEST                                      24
#define RPC_ACK                                                 25
#define RPC_NB_RPC                                              26

//----------------------------------------------------------
// Constants
//----------------------------------------------------------
#define RRM_MAX_MESSAGE_SIZE  1500

#define RRM_SERVER_PORT 9900L
#define RRC_SERVER_PORT 9899L

#define RADIO_GATEWAY_MASTER     77
#define RADIO_GATEWAY_SLAVE      83
#define RADIO_GATEWAY_SIMULATION 83
#define RADIO_GATEWAY_SIMULATION_BYPASS_L1 66
#define RADIO_GATEWAY_REAL_TIME_RF 82
#define RADIO_GATEWAY_REAL_TIME_IF 73

// CLASS_OF_TRAFFIC
#define CONVERSATIONAL 1
#define STREAMING      2
#define INTERACTIVE    3
#define BACKGROUND     4

// CONNECTION RESPONSE STATUS 
#define STATUS_CONNECTION_ACCEPTED                   0
#define STATUS_CONNECTION_REFUSED_ALREADY_CONNECTED -1
#define STATUS_CONNECTION_REFUSED_TOO_MANY_RG       -2
// ADD USER RESPONSE STATUS
#define ADD_USER_SUCCESSFUL  3
#define ADD_USER_FAILED     -3
// REMOVE USER RESPONSE STATUS
#define REMOVE_USER_SUCCESSFUL  4
#define REMOVE_USER_FAILED     -4
// ADD RADIO ACCESS BEARER STATUS
#define ADD_RADIO_ACCESS_BEARER_SUCCESSFUL  5
#define ADD_RADIO_ACCESS_BEARER_FAILED     -5
// REMOVE RADIO ACCESS BEARER STATUS
#define REMOVE_RADIO_ACCESS_BEARER_SUCCESSFUL  6
#define REMOVE_RADIO_ACCESS_BEARER_FAILED     -6

// Measure command
#define RRC_RRM_SETUP    1
#define RRC_RRM_RELEASE  3

// Measure type
#define RRC_RRM_TYPE_IntraFreq      0
#define RRC_RRM_TYPE_TrafficVolume  1
#define RRC_RRM_TYPE_Quality        2
#define RRC_RRM_TYPE_Internal       3


//----------------------------------------------------------
// Parameter types
//----------------------------------------------------------
struct rrc_rrm_measure_ctl {
  u8  command;      //setup-release as above
  u8  amount;       //number of measures 0-1-2-4-8-16-32-64
  u16 period;       //250--> 64000 ms
  u8  type;
  u8  BaseStation;  //1=YES
  u16 UE_Id;
};

// Measure report
// from Mobile Terminal
struct rrc_rrm_meas_report_mt_if {
  u16 UE_Id __attribute__ ((packed));
  u16 if_num_cells __attribute__ ((packed));
  u16 if_cell_id[maxCells] __attribute__ ((packed));
  u16 if_cell_parms_id[maxCells] __attribute__ ((packed));
  u16 if_BCH_RSCP[maxCells] __attribute__ ((packed));
  u16 if_pathloss[maxCells] __attribute__ ((packed));
  u16 if_slot_iscp[maxCells][JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
};
struct rrc_rrm_meas_report_mt_tv {
  u16 UE_Id __attribute__ ((packed));
  u16 tv_num_rbs __attribute__ ((packed));
  u16 tv_rbid[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_payload[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_average[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_variance[MAXMEASRBS] __attribute__ ((packed));
};
struct rrc_rrm_meas_report_mt_q {
  u16 UE_Id __attribute__ ((packed));
  u16 q_num_TrCH __attribute__ ((packed));
  u16 q_dl_TrCH_id[MAXMEASTRCH] __attribute__ ((packed));
  u16 q_dl_TrCH_BLER[MAXMEASTRCH] __attribute__ ((packed));
  u16 q_num_tfcs __attribute__ ((packed));
  u16 q_tfcs_id[MAXMEASTFCS] __attribute__ ((packed));
  u16 q_sir[MAXMEASTFCS][JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
};
struct rrc_rrm_meas_report_mt_int {
//  u8 UE_Id;
  u16 UE_Id __attribute__ ((packed));
  u16 int_xmit_power[JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
  u16 int_timing_advance __attribute__ ((packed));
};

union rrc_rrm_meas_report_mt_rrc {
  struct rrc_rrm_meas_report_mt_if meas_rep_mt_if;
  struct rrc_rrm_meas_report_mt_tv meas_rep_mt_tv;
  struct rrc_rrm_meas_report_mt_q meas_rep_mt_q;
  struct rrc_rrm_meas_report_mt_int meas_rep_mt_int;
};

// from Base station
struct rrc_rrm_meas_report_bs_tv {
  u16 UE_Id __attribute__ ((packed));
  u16 tv_num_rbs __attribute__ ((packed));
  u16 tv_rbid[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_payload[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_average[MAXMEASRBS] __attribute__ ((packed));
  u16 tv_rb_variance[MAXMEASRBS] __attribute__ ((packed));
};

struct rrc_rrm_meas_report_bs_q {
  u16 UE_Id __attribute__ ((packed));
  u16 q_num_TrCH __attribute__ ((packed));
  u16 q_dl_TrCH_id[MAXMEASTRCH] __attribute__ ((packed));
  u16 q_dl_TrCH_BLER[MAXMEASTRCH] __attribute__ ((packed));
  u16 q_num_tfcs __attribute__ ((packed));
  u16 q_tfcs_id[MAXMEASTFCS] __attribute__ ((packed));
  u16 q_sir[MAXMEASTFCS][JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
};

struct rrc_rrm_meas_report_bs_int {
  s32 int_xmit_power[numANTENNAS][JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
  s32 int_rscp[JRRM_SLOTS_PER_FRAME][MAXCH] __attribute__ ((packed));
  s32 int_rssi_rf[numANTENNAS][JRRM_SLOTS_PER_FRAME] __attribute__ ((packed));
  u16 int_spare[NUMSPARE] __attribute__ ((packed));
};

union rrc_rrm_meas_report_bs_rrc {
  struct rrc_rrm_meas_report_bs_tv meas_rep_bs_tv;
  struct rrc_rrm_meas_report_bs_q meas_rep_bs_q;
  struct rrc_rrm_meas_report_bs_int meas_rep_bs_int;
};

//----------------------------------------------------------
// Primitive definitions
//----------------------------------------------------------
// Message Header
typedef struct rrc_rrm_message_header {
  u16  type;
  u16  length;       // length includes only data following this header
  u32  frame;
} rpc_message;


// Message Content

// Primitives without content
//  RPC_CONNECTION_CLOSE_ACK  3
//  RPC_UPLINK_PHYSICAL_CHANNEL_CONTROL                     23
//  RPC_CONFIG_REQUEST                                      24
//  RPC_ACK                                                 25
//  RPC_NB_RPC                                              26

//-------------------------------------------
//  RPC_CONNECTION_REQUEST    0
typedef struct connection_request {
  char radio_gateway_type;   // master or slave
  char radio_gateway_execution_mode; // simulation, simulation + bypass L1, real time RF, real time IF
  u8   nb_antennas;
} connection_request;
//-------------------------------------------
//  RPC_CONNECTION_RESPONSE   1
typedef struct connection_response {
  s8   status;
  u8   equipment_id;
} connection_response;
//-------------------------------------------
//  RPC_CONNECTION_CLOSE      2
typedef struct connection_close {
  u8   equipment_id;
} connection_close;
//-------------------------------------------
//  RPC_ADD_USER_REQUEST      4
typedef struct add_user_request {
  u8   equipment_id;
  u8   user_id;
  u8   tx_id;
} add_user_request;

//-------------------------------------------
//  RPC_ADD_USER_RESPONSE     5
typedef struct add_user_response {
  u8   equipment_id;
  u8   user_id;
  u8   status;
  u8   tx_id;
} add_user_response;
//-------------------------------------------
//  RPC_ADD_USER_CONFIRM      6
typedef struct add_user_confirm {
  u8   equipment_id;
  u8   user_id;
} add_user_confirm;
//-------------------------------------------
//  RPC_REMOVE_USER_REQUEST   7
typedef struct remove_user_request {
  u8   equipment_id;
  u8   user_id;
  u8   tx_id;
} remove_user_request;
//-------------------------------------------
//  RPC_REMOVE_USER_RESPONSE  8
typedef struct remove_user_response {
  u8   equipment_id;
  u8   user_id;
  u8   status;
  u8   tx_id;
} remove_user_response;
//-------------------------------------------
//  RPC_ADD_RADIO_ACCESS_BEARER_REQUEST       9
typedef struct add_radio_access_bearer_request {
  u8   equipment_id;
  u8   user_id;
  u8   tx_id;
  u8   rab_id;
  u16  traffic_class;
  u16  delay;
  u32  guaranted_bit_rate_uplink;
  u32  max_bit_rate_uplink;
  u32  guaranted_bit_rate_downlink;
  u32  max_bit_rate_downlink;
} add_radio_access_bearer_request;
//-------------------------------------------
//  RPC_ADD_RADIO_ACCESS_BEARER_RESPONSE     10
typedef struct add_radio_access_bearer_response {
  u8   user_id;
  u8   rab_id;
  u8   status;
  u8   tx_id;
} add_radio_access_bearer_response;
//-------------------------------------------
//  RPC_ADD_RADIO_ACCESS_BEARER_CONFIRM      11
typedef struct add_radio_access_bearer_confirm {
  u8   equipment_id;
  u8   user_id;
  u8   rab_id;
} add_radio_access_bearer_confirm;
//-------------------------------------------
//  RPC_REMOVE_RADIO_ACCESS_BEARER_REQUEST   12
typedef struct remove_radio_access_bearer_request {
  u8   equipment_id;
  u8   user_id;
  u8   rab_id;
  u8   tx_id;
} remove_radio_access_bearer_request;
//-------------------------------------------
//  RPC_REMOVE_RADIO_ACCESS_BEARER_RESPONSE  13
typedef struct remove_radio_access_bearer_response {
  u8   user_id;
  u8   rab_id;
  u8   status;
  u8   tx_id;
} remove_radio_access_bearer_response;
//-------------------------------------------
//  RPC_MEASUREMENT_REQUEST                  14
typedef struct measurement_request {
  u8   measurement_type;
  u8   measurement_command;
  u8   equipment_type;
  u8   equipment_id;
  u16  amount;
  u16  period;
} measurement_request;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_RG_INTERNAL_REPORT        15
typedef struct l1_measurement_rg_internal_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_bs_int measurements;
} l1_measurement_rg_internal_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_RG_QUALITY_REPORT         16
typedef struct l1_measurement_rg_quality_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_bs_q measurements;
} l1_measurement_rg_quality_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_RG_TRAFFIC_VOLUME_REPORT  17
typedef struct l1_measurement_rg_traffic_volume_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_bs_tv measurements;
} l1_measurement_rg_traffic_volume_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_RG_PHYSICAL_CHANNEL_DATA_AMP_REPORT  18
//-------------------------------------------
//  RPC_L1_MEASUREMENT_MT_INTERNAL_REPORT                   19
typedef struct l1_measurement_mt_internal_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_mt_int measurements;
} l1_measurement_mt_internal_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_MT_QUALITY_REPORT                    20
typedef struct l1_measurement_mt_quality_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_mt_q measurements;
} l1_measurement_mt_quality_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_MT_TRAFFIC_VOLUME_REPORT             21
typedef struct l1_measurement_mt_traffic_volume_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_mt_tv measurements;
} l1_measurement_mt_traffic_volume_report;
//-------------------------------------------
//  RPC_L1_MEASUREMENT_MT_INTRA_FREQUENCY_REPORT            22
typedef struct l1_measurement_mt_intra_frequency_report {
  u32  equipment_id;
  struct rrc_rrm_meas_report_mt_if measurements;
} l1_measurement_mt_intra_frequency_report;
//-------------------------------------------


#endif
