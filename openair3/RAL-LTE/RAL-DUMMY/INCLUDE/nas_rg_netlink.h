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
#ifndef NAS_RG_NTL_H
#define NAS_RG_NTL_H

#include <net/if.h>
#include <netinet/in.h>

#include "rrc_d_types.h"

#define SOCK_TQAL_NAS_PATH "/tmp/SOCK_TQAL_NAS"


#define NAS_RG_NETL_MAXLEN 200
//#define NAS_RG_NETL_MAX_RABS 27 //spec value
#define NAS_RG_NETL_MAX_RABS 5 //test value
#define NAS_RG_NETL_MAX_MEASURE_NB  5
#define NAS_RG_NETL_MAX_MTs 3

#define NAS_CONNECTED	      1  //same as NAS interface
#define NAS_DISCONNECTED    0

#define NAS_UNICAST 0
#define NAS_MCAST   2

// type of CTL message
#define NAS_RG_MSG_STATISTIC_REQUEST        1
#define NAS_RG_MSG_STATISTIC_REPLY          2
#define NAS_RG_MSG_CNX_STATUS_REQUEST       3
#define NAS_RG_MSG_CNX_STATUS_REPLY         4
//#define NAS_RG_MSG_CNX_ESTABLISH_REQUEST    5
//#define NAS_RG_MSG_CNX_ESTABLISH_REPLY      6
//#define NAS_RG_MSG_CNX_RELEASE_REQUEST      7
//#define NAS_RG_MSG_CNX_RELEASE_REPLY        8
#define NAS_RG_MSG_RB_LIST_REQUEST          9
#define NAS_RG_MSG_RB_LIST_REPLY           10
#define NAS_RG_MSG_MEASUREMENT_REQUEST     11
#define NAS_RG_MSG_MEASUREMENT_REPLY       12
#define NAS_RG_MSG_MEASUREMENT_INDICATION  14
#define NAS_RG_MSG_CNX_LOSS_INDICATION     15
#define NAS_RG_MSG_RB_ESTABLISH_REQUEST    16
#define NAS_RG_MSG_RB_ESTABLISH_REPLY      17
#define NAS_RG_MSG_RB_RELEASE_REQUEST      18
#define NAS_RG_MSG_RB_RELEASE_REPLY        19
#define NAS_RG_MSG_MT_MCAST_JOIN           20
#define NAS_RG_MSG_MT_MCAST_LEAVE          21
#define NAS_RG_MSG_MT_MCAST_JOIN_REP       22
#define NAS_RG_MSG_MT_MCAST_LEAVE_REP      23

// Parameter types
//----------------------------------------------------------
typedef uint16_t netlIPVersion_t;
typedef struct netlIPAddr {
        netlIPVersion_t ip_version;
        struct in6_addr ip_address;
} netlIPAddr_t;
typedef uint32_t netlStatValue_t;
typedef uint32_t netlProviderId_t;
typedef uint16_t netlCellID_t; // ID of the cell for connection
typedef uint32_t netlMobileId_t; // mobile identifier
typedef uint16_t netlStatus_t; // establishment status
typedef uint32_t netlCnxid_t;
typedef uint16_t netlRadioBearerId_t;
typedef uint16_t netlQoSTrafficClass_t; //QoS traffic class requested
typedef uint16_t netlIPdscp_t; // DSCP code transported to service NAS
typedef struct netlRBDef {
        netlRadioBearerId_t rbId;
        netlQoSTrafficClass_t QoSclass;
        netlIPdscp_t dscp;
} netlrbParms_t;
typedef uint16_t  netlNumRBsInList_t; // number of RBs defined in the list
typedef uint16_t  netlNumCellsMeas_t; // number of RGs that could be measured
typedef uint32_t netlSigLevel_t; // Signal level measured
typedef struct netlMeasures {
        netlCellID_t cell_id;
        netlSigLevel_t level;
        netlProviderId_t provider_id;
} netlMeasures_t;
typedef uint16_t netlMcast_t;



// Parameter structures
//----------------------------------------------------------
struct nas_rg_msg_statistic_reply  //FFS
{
	netlStatValue_t rx_packets;
	netlStatValue_t tx_packets;
	netlStatValue_t rx_bytes;
	netlStatValue_t tx_bytes;
	netlStatValue_t rx_errors;
	netlStatValue_t tx_errors;
	netlStatValue_t rx_dropped;
	netlStatValue_t tx_dropped;
};

struct nas_rg_msg_cnx_status_request  //FFS
{
  netlMobileId_t  ue_id;
};

struct nas_rg_msg_cnx_status_reply  //FFS
{
  netlMobileId_t  ue_id;
	uint8_t iid[8]; 		// interface identification - EUI64
  netlIPAddr_t  l3_addr;
	netlStatus_t status;
	netlNumRBsInList_t num_rb;
	netlSigLevel_t signal_level;
  netlMobileId_t  valid_UEs[NAS_RG_NETL_MAX_MTs];
};

struct nas_rg_msg_rb_list_request  //FFS
{
  netlMobileId_t  ue_id;
};

struct nas_rg_msg_rb_list_reply  //FFS
{
  netlMobileId_t  ue_id;
  netlNumRBsInList_t num_rb;        // actual number of RBs in the list
  netlrbParms_t RBList[NAS_RG_NETL_MAX_RABS];
};

struct nas_rg_msg_signal_loss
{
  netlMobileId_t  ue_id;
};

struct nas_rg_msg_rb_establish_request
{
  netlCnxid_t   cnxid;
  netlrbParms_t RBParms;
  netlIPAddr_t  ip_addr;
  netlMcast_t   mcast_flag;
};

struct nas_rg_msg_rb_establish_reply
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
  netlrbParms_t RBParms;
  netlStatus_t  result;
};
struct nas_rg_msg_rb_release_request
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
  netlRadioBearerId_t rbId;
};

struct nas_rg_msg_rb_release_reply
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
  netlStatus_t  result;
};

struct nas_rg_msg_mt_mcast_join
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
};

struct nas_rg_msg_mt_mcast_leave
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
};

struct nas_rg_msg_mt_mcast_join_rep
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
  netlStatus_t  result;
};

struct nas_rg_msg_mt_mcast_leave_rep
{
  netlCnxid_t   cnxid;
  netlMobileId_t  ue_id;
  netlStatus_t  result;
};

/*****
 * TQAL-NAS Primitive
 *****/
struct nas_rg_netl_hdr
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
};
struct nas_rg_netl_request
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_rg_msg_cnx_status_request   cnx_stat_req;
    struct nas_rg_msg_rb_list_request      rb_list_req;
    struct nas_rg_msg_rb_establish_request rb_est_req;
    struct nas_rg_msg_rb_release_request   rb_rel_req;
    struct nas_rg_msg_mt_mcast_join        mt_mcast_join;
    struct nas_rg_msg_mt_mcast_leave       mt_mcast_leave;
  } tqalNASPrimitive;
};
struct nas_rg_netl_reply
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_rg_msg_statistic_reply    statistics_rep;
    struct nas_rg_msg_cnx_status_reply   cnx_stat_rep;
    struct nas_rg_msg_rb_list_reply      rb_list_rep;
    struct nas_rg_msg_rb_establish_reply rb_est_rep;
    struct nas_rg_msg_rb_release_reply   rb_rel_rep;
    struct nas_rg_msg_mt_mcast_join_rep  mt_mc_join_rep;
    struct nas_rg_msg_mt_mcast_leave_rep mt_mc_leavce_rep;
  } tqalNASPrimitive;
};
struct nas_rg_netl_indication
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_rg_msg_signal_loss sign_loss;
  } tqalNASPrimitive;
};


#endif
