/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_variables.h
 *
 * Version 0.1
 *
 * Date  06/26/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem RAL-LTE internal data structure
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description Defines the data structure managed by the RAL-LTE process at
 *  the network side and the access router dummy configuration.
 *
 *****************************************************************************/
#ifndef __RAL_LTE_VAR_H__
#define __RAL_LTE_VAR_H__

// Define working mode : Dummy or Realtime
//#define RAL_DUMMY
#define RAL_REALTIME

//#define ENABLE_MEDIEVAL_DEMO3
//#define MUSER_CONTROL // in demo3, for triggering the congestion report manually

//flag to reduce the logs
#define DEBUG_RAL_DETAILS

#include "rrc_d_types.h"
#ifdef RAL_DUMMY
#include "nas_rg_netlink.h"
#endif

#include "MIH_C_Types.h"
#include "MIH_C_header_codec.h"
#include "MIH_C_Link_Primitives.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

#ifdef RAL_REALTIME
/*Arguments ioctl command
 */
//arg[0]
#define IO_OBJ_STATS 0
#define IO_OBJ_CNX   1
#define IO_OBJ_RB    2
#define IO_OBJ_MEAS  3
#define IO_OBJ_MC    4  // multicast

//arg[1]
#define IO_CMD_ADD   0
#define IO_CMD_DEL   1
#define IO_CMD_LIST  2

#define NAS_CONNECTED     1  //same as NAS interface
#define NAS_DISCONNECTED  0
#endif // RAL_REALTIME

/* Radio Bearer attachment status; must be the same as NAS interface     */
#define RB_CONNECTED  NAS_CONNECTED
#define RB_DISCONNECTED  NAS_DISCONNECTED

#define RAL_TRUE 1
#define RAL_FALSE 0


/*Access Router configuration
 */
#ifdef RAL_REALTIME
//#define NAS_RG_NETL_MAX_RABS 27 //spec value
#define NAS_RG_NETL_MAX_RABS 5 //test value
#define NAS_RG_NETL_MAX_MEASURE_NB  5
#define NAS_RG_NETL_MAX_MTs 3
#endif

/* Maximum number of supported Radio Bearers */
#define RAL_MAX_RB  NAS_RG_NETL_MAX_RABS
/* Maximum number of supported Mobile Terminals */
#define RAL_MAX_MT NAS_RG_NETL_MAX_MTs
/* Maximum number of Radio Bearers per User Equipment */
#define RAL_MAX_RB_PER_UE 32
/* Default Radio Bearer identifier */
#define RAL_DEFAULT_MC_RAB_ID 5
#define RAL_DEFAULT_RAB_ID  6 // RBID 5 => MBMS, 6 => DEFAULTRAB, 7+ => others
/* Default Radio Bearer QoS value */
#define RAL_DEFAULT_RAB_QoS 2 // RRC_QOS_CONV_64_64
/* Default current cell identifier */
#define RAL_DEFAULT_CELL_ID 5
// Constants for Measures
#define RAL_DEFAULT_MEAS_POLLING_INTERVAL 51
#define RAL_DEFAULT_CONGESTION_THRESHOLD 80

/* Default bit rates */
#define RAL_BITRATE_32k   32000
#define RAL_BITRATE_64k   64000
#define RAL_BITRATE_128k 128000
#define RAL_BITRATE_256k 256000
#define RAL_BITRATE_320k 320000
#define RAL_BITRATE_384k 384000
#define RAL_BITRATE_440k 440000

/* Public Land Mobile Network */
#define DEFAULT_PLMN_SIZE   3
#ifdef DEFINE_GLOBAL_CONSTANTS
const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE] = {0x20, 0x80, 0x20};
#else
extern const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE];
#endif

/*Destination addresses
 */
enum {
    ADDR_MT1 = 0,
    ADDR_MT2,
    ADDR_MBMS,
    ADDR_MT3,
    ADDR_MAX
};

#ifdef DEFINE_GLOBAL_CONSTANTS  // Modified MW 23/05/13
const char DestIpv6Addr[ADDR_MAX][16] = { // DUMMY
    // MT1 - 2001:660:382:14:335:600:8014:9150
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x50},
    // MT2 - 2001:660:382:14:335:600:8014:9151
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x51},
    // Multicast - FF3E:20:2001:DB8::43
    {0xFF,0x3E,0x00,0x20,0x20,0x01,0x0D,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43},
    // MT3 - 2001:660:382:14:335:600:8014:9153
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x53}
};
#else
extern const char DestIpv6Addr[ADDR_MAX][16];
#endif

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/* List of link action types */
TYPEDEF_BITMAP8(MIH_C_LINK_AC_TYPE_LIST);

/*
 * Radio Bearer data
 */
struct ral_lte_channel {
    u32 cnx_id;
    u8  multicast;
// MIHF parameters  ix=0 UPLINK, ix=1 DOWNLINK
    u16 flowId[2];
    u16 classId[2];
    float resBitrate[2];
    float meanBitrate[2];
    float bktDepth[2];
    float pkBitrate[2];
    float MTU[2];
//NAS driver parameters
    u16 rbId;
    u16 RadioQoSclass;
    u16 dscpUL;
    u16 dscpDL;
    u16 nas_state;
    u16 status;
};

/*
 * Mobile Terminal data
 */
struct ral_lte_mt {
    /* The identifier of the link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    u8  ipv6_addr[16];
    u32 ipv6_l2id[2];
    u32 ue_id;
    struct ral_lte_channel radio_channel[RAL_MAX_RB];
    int num_rbs;
    int num_class;
    int nas_state;
    int mt_state;
};

/*
 * Multicast data  // TEMP MW A supprimer!!!!
 */
struct ral_lte_mcast {
    /* The identifier of the multicast link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    struct ral_lte_channel radio_channel;
    u8 mc_group_addr[16];
};

/*
 * RAL LTE internal data
 */
struct ral_lte_priv {

    u8 plmn[3];
    int curr_cellId;
    int num_connected_mts;

    u8 pending_req_flag;
    u8 pending_req_mt_ix;
    u8 pending_req_ch_ix;
    u8 pending_req_multicast;
//    u16 pending_req_transaction_id;
//    u8 pending_req_status;
    MIH_C_FLOW_ID_T pending_req_fid;

    struct ral_lte_mt pending_mt;
    int pending_mt_timer;
    int pending_mt_flag;

    struct ral_lte_mt mt[RAL_MAX_MT];
    struct ral_lte_mcast mcast;
//     struct tqal_ar_mobile mt[TQAL_MAX_MTs];
//     struct tqal_ar_channel multicast_channel;
//     u8  mc_group_addr[16];
    //Added for demo 3 - MW
    int meas_polling_interval;
    int meas_polling_counter;
    u16 num_UEs;
    u32 rlcBufferOccupancy[RAL_MAX_MT];
    u32 scheduledPRB[RAL_MAX_MT];
    u32 totalDataVolume[RAL_MAX_MT];
    u32 totalNumPRBs;
    int congestion_flag;
    int congestion_threshold;
    int measures_triggered_flag;
    int requested_period;
    // MIH-INTERFACE data
    MIH_C_LINK_AC_TYPE_LIST_T  mih_supported_link_action_list;
    MIH_C_LINK_EVENT_LIST_T    mih_supported_link_event_list;
    MIH_C_LINK_CMD_LIST_T      mih_supported_link_command_list;
    MIH_C_LINK_EVENT_LIST_T    mih_subscribe_req_event_list;

    LIST(MIH_C_LINK_CFG_PARAM, mih_link_cfg_param_thresholds);
    // to tell what are the configured thresholds in mih_link_cfg_param_thresholds_list
    MIH_C_BOOLEAN_T  active_mih_link_cfg_param_threshold[MIH_C_LINK_CFG_PARAM_LIST_LENGTH];


    MIH_C_LINK_AC_TYPE_T    pending_req_action;
    MIH_C_STATUS_T          pending_req_status;
    MIH_C_LINK_AC_RESULT_T  pending_req_ac_result;
    MIH_C_TRANSACTION_ID_T  pending_req_transaction_id;

    char buffer[800];
};

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

extern struct ral_lte_priv *ralpriv;

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif

