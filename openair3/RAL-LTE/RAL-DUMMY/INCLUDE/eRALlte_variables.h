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

#ifndef __RAL_LTE_VAR_H__
#define __RAL_LTE_VAR_H__

#include "rrc_d_types.h"
#include "nas_rg_netlink.h"

#include "MIH_C_Types.h"
#include "MIH_C_header_codec.h"
#include "MIH_C_Link_Primitives.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

#define RAL_DUMMY
//#define RAL_REALTIME


/* Radio Bearer attachment status; must be the same as NAS interface	    */
#define RB_CONNECTED		NAS_CONNECTED
#define RB_DISCONNECTED		NAS_DISCONNECTED

#ifdef RAL_REALTIME
/*
 * --------------------------------------------------------------------------
 * Arguments ioctl command
 * --------------------------------------------------------------------------
 */

//arg[0]
#define IO_OBJ_STATS 0
#define IO_OBJ_CNX   1
#define IO_OBJ_RB    2
#define IO_OBJ_MC    3  // multicast
//arg[1]
#define IO_CMD_ADD   0
#define IO_CMD_DEL   1
#define IO_CMD_LIST  2

#endif // RAL_REALTIME

/*
 * --------------------------------------------------------------------------
 * Access Router configuration
 * --------------------------------------------------------------------------
 */

/* Maximum number of supported Radio Bearers */
#define RAL_MAX_RB 	NAS_RG_NETL_MAX_RABS

/* Maximum number of supported Mobile Terminals */
#define RAL_MAX_MT	NAS_RG_NETL_MAX_MTs

/* Maximum number of Radio Bearers per User Equipment */
#define RAL_MAX_RB_PER_UE	32

/* Default Radio Bearer identifier */
#define RAL_DEFAULT_RAB_ID 	6 // RBID 5 => MBMS, 6 => DEFAULTRAB, 7+ => others

/* Default Radio Bearer QoS value */
#define RAL_DEFAULT_RAB_QoS	2 // RRC_QOS_CONV_64_64

/* Default current cell identifier */
#define RAL_DEFAULT_CELL_ID	5

/* Default bit rates */
#define RAL_BITRATE_32k   32000
#define RAL_BITRATE_64k   64000
#define RAL_BITRATE_128k 128000
#define RAL_BITRATE_256k 256000
#define RAL_BITRATE_320k 320000
#define RAL_BITRATE_384k 384000
#define RAL_BITRATE_440k 440000

/* Public Lan Mobile Network */
#define DEFAULT_PLMN_SIZE	  3
#ifdef DEFINE_GLOBAL_CONSTANTS
const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE] = {0x20, 0x80, 0x20};
#else
extern const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE];
#endif

/*
 * Destination addresses
 * ---------------------
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
TYPEDEF_BITMAP8(MIH_C_LINK_AC_TYPE_LIST)

/*
 * Radio Bearer data
 */
struct ral_lte_channel {
    uint32_t cnx_id;
    uint8_t  multicast;
// MIHF parameters  ix=0 UPLINK, ix=1 DOWNLINK
    uint16_t flowId[2];
    uint16_t classId[2];
    float resBitrate[2];
    float meanBitrate[2];
    float bktDepth[2];
    float pkBitrate[2];
    float MTU[2];
//NAS driver parameters
    uint16_t rbId;
    uint16_t RadioQoSclass;
    uint16_t dscpUL;
    uint16_t dscpDL;
    uint16_t nas_state;
    uint16_t status;
};

/*
 * Mobile Terminal data
 */
struct ral_lte_mt {
    /* The identifier of the link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    uint8_t  ipv6_addr[16];
    uint32_t ipv6_l2id[2];
    uint32_t ue_id;
    struct ral_lte_channel radio_channel[RAL_MAX_RB];
    int num_rbs;
    int num_class;
    int nas_state;
    int mt_state;
};

/*
 * Multicast data
 */
struct ral_lte_mcast {
    /* The identifier of the multicast link that is associated with a PoA */
    MIH_C_LINK_TUPLE_ID_T ltid;
    struct ral_lte_channel radio_channel;
    uint8_t mc_group_addr[16];
};

/*
 * RAL LTE internal data
 */
struct ral_lte_priv {

    uint8_t plmn[3];
    int curr_cellId;
    int num_connected_mts;

    uint8_t pending_req_flag;
    uint8_t pending_req_mt_ix;
    uint8_t pending_req_ch_ix;
    MIH_C_FLOW_ID_T pending_req_fid;

    struct ral_lte_mt pending_mt;
    int pending_mt_timer;

    struct ral_lte_mt mt[RAL_MAX_MT];
    struct ral_lte_mcast mcast;

    /* MIH-INTERFACE data */
    MIH_C_LINK_AC_TYPE_LIST_T       mih_supported_link_action_list;
    MIH_C_LINK_EVENT_LIST_T         mih_supported_link_event_list;
    MIH_C_LINK_CMD_LIST_T           mih_supported_link_command_list;

    MIH_C_LINK_EVENT_LIST_T         mih_subscribe_req_event_list;

    MIH_C_LINK_AC_TYPE_T            pending_req_action;
    MIH_C_STATUS_T                  pending_req_status;
    MIH_C_LINK_AC_RESULT_T          pending_req_ac_result;
    MIH_C_TRANSACTION_ID_T          pending_req_transaction_id;
};

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

extern struct ral_lte_priv *ralpriv;

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif

