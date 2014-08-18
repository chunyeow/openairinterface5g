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
#ifndef NAS_UE_NTL_H
#define NAS_UE_NTL_H

#include <net/if.h>
#include <netinet/in.h>

#include "rrc_d_types.h"

//#define SOCK_NAS_PATH "/opt/eurecom/ipc/SOCK_IAL_NAS"
#define SOCK_NAS_PATH "/tmp/SOCK_IAL_NAS"

// Temp constants = arguments gioctl
//arg[0]
#define IO_OBJ_STATS 0
#define IO_OBJ_CNX   1
#define IO_OBJ_RB    2
#define IO_OBJ_MEAS  3
#define IO_OBJ_IMEI  4
//arg[1]
#define IO_CMD_ADD   0
#define IO_CMD_DEL   1
#define IO_CMD_LIST  2

#define NAS_UE_NETL_MAXLEN 500
#define NAS_UE_NETL_MAX_RABS 27
#define NAS_UE_NETL_MAX_MEASURE_NB  5

//#define NAS_CONNECTED	      1  //same as NAS interface
//#define NAS_DISCONNECTED    0
#define NAS_CONNECTED	      1  //same as NAS interface
#define NAS_DISCONNECTED    0

// type of CTL message
#define NAS_UE_MSG_STATISTIC_REQUEST        1
#define NAS_UE_MSG_STATISTIC_REPLY          2
#define NAS_UE_MSG_CNX_STATUS_REQUEST       3
#define NAS_UE_MSG_CNX_STATUS_REPLY         4
#define NAS_UE_MSG_CNX_ESTABLISH_REQUEST    5
#define NAS_UE_MSG_CNX_ESTABLISH_REPLY      6
#define NAS_UE_MSG_CNX_RELEASE_REQUEST      7
#define NAS_UE_MSG_CNX_RELEASE_REPLY        8
#define NAS_UE_MSG_CNX_LIST_REQUEST         9
#define NAS_UE_MSG_CNX_LIST_REPLY          10
#define NAS_UE_MSG_RB_LIST_REQUEST         15
#define NAS_UE_MSG_RB_LIST_REPLY           16
#define NAS_UE_MSG_MEAS_REQUEST            23
#define NAS_UE_MSG_MEAS_REPLY              24
#define NAS_UE_MSG_IMEI_REQUEST            25
#define NAS_UE_MSG_IMEI_REPLY              26
//
#define NAS_UE_MSG_MEASUREMENT_INDICATION  17
#define NAS_UE_MSG_CNX_LOSS_INDICATION     18


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
typedef uint16_t netlConnectionStatus_t; // connection establishment status
typedef uint16_t netlRadioBearerId_t;
typedef uint16_t netlQoSTrafficClass_t; //QoS traffic class requested
typedef uint16_t netlIPdscp_t; // DSCP code transported to service NAS
typedef struct netlRBDef {
        netlRadioBearerId_t rbId;
        netlQoSTrafficClass_t QoSclass;
        netlIPdscp_t dscp;
        uint8_t state;
} netlrbParms_t;
typedef uint16_t  netlNumRBsInList_t; // number of RBs defined in the list
typedef uint16_t  netlNumCellsMeas_t; // number of RGs that could be measured
typedef uint32_t netlSigLevel_t; // Signal level measured
typedef struct netlMeasures {
        netlCellID_t cell_id;
        netlSigLevel_t level;
        netlProviderId_t provider_id;
} netlMeasures_t;



// Parameter structures
//----------------------------------------------------------
struct nas_ue_msg_statistic_reply
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

struct nas_ue_msg_cnx_status_reply
{
	netlCellID_t cellid;		// cell identification
	uint8_t iid[8]; 		// interface identification - EUI64
  netlIPAddr_t   ip_addr;
	netlConnectionStatus_t status;
	netlNumRBsInList_t num_rb;
	netlSigLevel_t signal_level;
};

struct nas_ue_msg_cnx_list_reply
{
	uint8_t state;
	uint16_t cellid;		// cell identification
	uint32_t iid6[2]; 			// IPv6  interface identification
	uint8_t iid4; 			// IPv4 interface identification
	uint16_t num_rb;
	uint16_t nsclassifier;
};


struct nas_ue_msg_cnx_establish_request
{
	netlCellID_t cellid; // Cell identification
};

struct nas_ue_msg_cnx_establish_reply
{
	netlConnectionStatus_t status;
//	uint8_t iid6[8]; 	// IPv6  interface identification - EUI64
};

struct nas_ue_msg_cnx_release_reply
{
	netlConnectionStatus_t status;
};

struct nas_ue_msg_rb_list_reply
{
  netlNumRBsInList_t num_rb;        // actual number of RBs in the list
  netlrbParms_t RBList[NAS_UE_NETL_MAX_RABS];
};

struct nas_ue_msg_measure_request
{
  netlNumCellsMeas_t num_cells;
	netlCellID_t cellid[NAS_UE_NETL_MAX_MEASURE_NB]; // Cell identification
  uint16_t num_providers;
	netlProviderId_t provider_id[NAS_UE_NETL_MAX_MEASURE_NB]; // Provider identification
};

struct nas_ue_msg_measure_reply
{
  netlNumCellsMeas_t num_cells;
  netlMeasures_t measures[NAS_UE_NETL_MAX_MEASURE_NB];
  uint16_t signal_lost_flag;
};

struct nas_ue_msg_measure_indicate
{
  netlMeasures_t measure;
};

struct nas_ue_msg_signal_loss
{
  netlMeasures_t measure;
};

struct nas_ue_l2id_reply
{
  uint32_t l2id [2];
};

///////////////////////////

/*****
 * IAL-NAS Primitive
 *****/
struct nas_ue_netl_hdr
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
};
struct nas_ue_netl_request
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_ue_msg_cnx_establish_request cnx_req;
    struct nas_ue_msg_measure_request meas_req;
  } ialNASPrimitive;
};
struct nas_ue_netl_reply
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_ue_msg_statistic_reply  statistics_rep;
    struct nas_ue_msg_cnx_status_reply cnx_stat_rep;
    struct nas_ue_msg_cnx_list_reply   cnx_list_rep;
    struct nas_ue_msg_cnx_establish_reply cnx_est_rep;
    struct nas_ue_msg_cnx_release_reply   cnx_rel_rep;
    struct nas_ue_msg_rb_list_reply    rb_list_rep;
    struct nas_ue_msg_measure_reply    meas_rep;
    struct nas_ue_l2id_reply  l2id_rep;
  } ialNASPrimitive;
};
struct nas_ue_netl_indication
{
	char name[IFNAMSIZ];
	uint16_t type;
  uint16_t length;
  union {
    struct nas_ue_msg_measure_indicate meas_ind;
    struct nas_ue_msg_signal_loss sign_loss;
  } ialNASPrimitive;
};


#endif
