/***************************************************************************
                          nasrg_iocontrol.h  -  description
  ***************************************************************************
  Eurecom OpenAirInterface 2
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
/*! \file nasrg_iocontrol.h
* \brief I/O control constnats and structures for upper layers of driver for OpenAirInterface CELLULAR version - RG
* \author  michelle.wetterwald, navid.nikaein, raymond.knopp, Lionel Gauthier
* \company Eurecom
* \email: michelle.wetterwald@eurecom.fr, raymond.knopp@eurecom.fr, navid.nikaein@eurecom.fr,  lionel.gauthier@eurecom.fr
*/
/*******************************************************************************/
#ifndef NASRGD_CTL_H
#define NASRGD_CTL_H

#include <asm/byteorder.h>
#include <asm/types.h>
#include <linux/udp.h>
#include <linux/tcp.h>

#define NAS_MSG_MAXLEN 1100

// type of CTL message
#define NAS_MSG_STATISTIC_REQUEST       1
#define NAS_MSG_STATISTIC_REPLY         2
#define NAS_MSG_ECHO_REQUEST   3
#define NAS_MSG_ECHO_REPLY   4
#define NAS_MSG_CX_ESTABLISHMENT_REQUEST 5
#define NAS_MSG_CX_ESTABLISHMENT_REPLY 6
#define NAS_MSG_CX_RELEASE_REQUEST  7
#define NAS_MSG_CX_RELEASE_REPLY  8
#define NAS_MSG_CX_LIST_REQUEST  9
#define NAS_MSG_CX_LIST_REPLY   10
#define NAS_MSG_RB_ESTABLISHMENT_REQUEST 11
#define NAS_MSG_RB_ESTABLISHMENT_REPLY 12
#define NAS_MSG_RB_RELEASE_REQUEST  13
#define NAS_MSG_RB_RELEASE_REPLY  14
#define NAS_MSG_RB_LIST_REQUEST  15
#define NAS_MSG_RB_LIST_REPLY   16
#define NAS_MSG_CLASS_ADD_REQUEST  17
#define NAS_MSG_CLASS_ADD_REPLY  18
#define NAS_MSG_CLASS_DEL_REQUEST  19
#define NAS_MSG_CLASS_DEL_REPLY  20
#define NAS_MSG_CLASS_LIST_REQUEST  21
#define NAS_MSG_CLASS_LIST_REPLY  22
#define NAS_MSG_NEIGHBOUR_REQUEST  23
#define NAS_MSG_NEIGHBOUR_REPLY  24
//MBMS
#define NAS_RG_MSG_MT_MCAST_JOIN           30
#define NAS_RG_MSG_MT_MCAST_LEAVE          31
#define NAS_RG_MSG_MT_MCAST_REPLY          32
// ENB MEASURES (MEDIEVAL DEMO 3)
#define NAS_MSG_ENB_MEAS_TRIGGER  33
#define NAS_MSG_ENB_MEAS_RETRIEVE 34


// Max number of entry of a message list
#define NAS_LIST_CX_MAX 32
#define NAS_LIST_RB_MAX 32
#define NAS_LIST_CLASS_MAX 32

typedef u16 nasMsgType_t;

struct nas_ioctl
{
 char name[IFNAMSIZ];
 nasMsgType_t type;
 char *msg;
};

//****
struct nas_msg_statistic_reply
{
 u32 rx_packets;
 u32 tx_packets;
 u32 rx_bytes;
 u32 tx_bytes;
 u32 rx_errors;
 u32 tx_errors;
 u32 rx_dropped;
 u32 tx_dropped;
};

//****
struct nas_msg_cx_list_reply
{
 nasLocalConnectionRef_t lcr;  // Local Connection reference
 u8 state;
 nasCellID_t cellid;  // cell identification
 u32 iid6[2];    // IPv6  interface identification
 u8 iid4;    // IPv4 interface identification
 u16 num_rb;
 u16 nsclassifier;
};
//****
struct nas_msg_cx_establishment_reply
{
 int status;
};
struct nas_msg_cx_establishment_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
 nasCellID_t cellid; // Cell identification
};
//****
struct nas_msg_cx_release_reply
{
 int status;
};
struct nas_msg_cx_release_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
};

//****
struct nas_msg_rb_list_reply
{
 nasRadioBearerId_t rab_id;
 nasSapId_t sapi;
 nasQoSTrafficClass_t qos;
 u8 state;
  u32 cnxid;
};
struct nas_msg_rb_list_request
{
 nasLocalConnectionRef_t lcr;  // Local Connection reference
};
//****
struct nas_msg_rb_establishment_reply
{
 int status;
  u32 cnxid;
};
struct nas_msg_rb_establishment_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
 nasRadioBearerId_t rab_id;
 nasQoSTrafficClass_t qos;
  u16 dscp_ul;
  u16 dscp_dl;
  u16 mcast_flag;
  u8 mcast_group[16];
  u32 cnxid;
};

//****
struct nas_msg_rb_release_reply
{
 int status;
  u32 cnxid;
};
struct nas_msg_rb_release_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
 nasRadioBearerId_t rab_id;
  u16 mcast_flag;
  u32 cnxid;
};

//****
struct nas_msg_class_add_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
 nasRadioBearerId_t rab_id;
 u8 dir; // direction (send or receive)
 u8 dscp; // codepoint
 u8 fct;
 u16 classref;
 u8 version;
 union
 {
  struct in6_addr ipv6;
  u32 ipv4;
 } saddr; // IP source address
 u8 splen; // prefix length
 union
 {
  struct in6_addr ipv6;
  u32 ipv4;
 } daddr; // IP destination address
 u8 dplen; // prefix length
 u8 protocol;  // high layer protocol type
 u16 sport;  // source port
 u16 dport;  // destination port
};
struct nas_msg_class_add_reply
{
 int status;
};
//****
struct nas_msg_class_del_request
{
 nasLocalConnectionRef_t lcr; // Local Connection reference
 u8 dir; // direction (send or receive)
 u8 dscp; // codepoint
 u16 classref;
};
struct nas_msg_class_del_reply
{
 int status;
};
//****
#define nas_msg_class_list_reply nas_msg_class_add_request
struct nas_msg_class_list_request
{
 nasLocalConnectionRef_t lcr;  // Local Connection reference
 u8 dir;
 u8 dscp;
};

//****
struct nas_msg_neighbour_cell_list_reply
{
 nasCellID_t cellid;  // cell identification
 u32 iid6[4];    // IPv6  address of access router
};

//MBMS
//****
struct nas_msg_mt_mcast_join
{
  nasLocalConnectionRef_t  ue_id;
  u32   cnxid;
 nasRadioBearerId_t rab_id;
};

struct nas_msg_mt_mcast_leave
{
  nasLocalConnectionRef_t  ue_id;
  u32   cnxid;
  nasRadioBearerId_t rab_id;
};

struct nas_msg_mt_mcast_reply
{
  nasLocalConnectionRef_t  ue_id;
  u32   cnxid;
  int  result;
};
//****

//MEDIEVAL DEMO 3
//****
struct nas_msg_enb_measure_trigger
{
  nasCellID_t cell_id;
};
struct nas_msg_enb_measure_trigger_reply
{
  int status;
};

struct nas_msg_enb_measure_retrieve
{
  nasCellID_t cell_id;
  nasNumConnUEs_t num_UEs;
  nasENbMeasures_t measures[MAX_MEASURE_UE];
  nasENbMeasure_t totalNumPRBs;
};


#endif
