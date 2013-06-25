/***************************************************************************
                          nas_ue_netlink.h  -  description
                             -------------------
    copyright            : (C) 2005 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
                           yan.moret@eurecom.fr
 ***************************************************************************

 ***************************************************************************/
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
typedef u16 netlIPVersion_t;
typedef struct netlIPAddr {
        netlIPVersion_t ip_version;
        struct in6_addr ip_address;
} netlIPAddr_t;
typedef u32 netlStatValue_t;
typedef u32 netlProviderId_t;
typedef u16 netlCellID_t; // ID of the cell for connection
typedef u16 netlConnectionStatus_t; // connection establishment status
typedef u16 netlRadioBearerId_t;
typedef u16 netlQoSTrafficClass_t; //QoS traffic class requested
typedef u16 netlIPdscp_t; // DSCP code transported to service NAS
typedef struct netlRBDef {
        netlRadioBearerId_t rbId;
        netlQoSTrafficClass_t QoSclass;
        netlIPdscp_t dscp;
        u8 state;
} netlrbParms_t;
typedef u16  netlNumRBsInList_t; // number of RBs defined in the list
typedef u16  netlNumCellsMeas_t; // number of RGs that could be measured
typedef u32 netlSigLevel_t; // Signal level measured
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
	u8 iid[8]; 		// interface identification - EUI64
  netlIPAddr_t   ip_addr;
	netlConnectionStatus_t status;
	netlNumRBsInList_t num_rb;
	netlSigLevel_t signal_level;
};

struct nas_ue_msg_cnx_list_reply
{
	u8 state;
	u16 cellid;		// cell identification
	u32 iid6[2]; 			// IPv6  interface identification
	u8 iid4; 			// IPv4 interface identification
	u16 num_rb;
	u16 nsclassifier;
};


struct nas_ue_msg_cnx_establish_request
{
	netlCellID_t cellid; // Cell identification
};

struct nas_ue_msg_cnx_establish_reply
{
	netlConnectionStatus_t status;
//	u8 iid6[8]; 	// IPv6  interface identification - EUI64
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
  u16 num_providers;
	netlProviderId_t provider_id[NAS_UE_NETL_MAX_MEASURE_NB]; // Provider identification
};

struct nas_ue_msg_measure_reply
{
  netlNumCellsMeas_t num_cells;
  netlMeasures_t measures[NAS_UE_NETL_MAX_MEASURE_NB];
  u16 signal_lost_flag;
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
  u32 l2id [2];
};

///////////////////////////

/*****
 * IAL-NAS Primitive
 *****/
struct nas_ue_netl_hdr
{
	char name[IFNAMSIZ];
	u16 type;
  u16 length;
};
struct nas_ue_netl_request
{
	char name[IFNAMSIZ];
	u16 type;
  u16 length;
  union {
    struct nas_ue_msg_cnx_establish_request cnx_req;
    struct nas_ue_msg_measure_request meas_req;
  } ialNASPrimitive;
};
struct nas_ue_netl_reply
{
	char name[IFNAMSIZ];
	u16 type;
  u16 length;
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
	u16 type;
  u16 length;
  union {
    struct nas_ue_msg_measure_indicate meas_ind;
    struct nas_ue_msg_signal_loss sign_loss;
  } ialNASPrimitive;
};


#endif
