/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

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
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/***************************************************************************
                          local.h  -  description
                             -------------------
    copyright            : (C) 2002 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
                           yan.moret@eurecom.fr
                           knopp@eurecom.fr
 ***************************************************************************

 ***************************************************************************/

#ifndef OAI_LOCAL_H
#define OAI_LOCAL_H

#include <linux/if_arp.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/ipv6.h>
#include <linux/ip.h>
#include <linux/sysctl.h>
#include <linux/timer.h>
#include <linux/unistd.h>
#include <asm/param.h>
//#include <sys/sysctl.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/in.h>
#include <net/ndisc.h>

#define PDCP2IP_FIFO 21
#define IP2PDCP_FIFO 22

#include "constant.h"
#include "sap.h"
//#include "rrc_nas_primitives.h"


struct rb_entity {
  OaiNwDrvRadioBearerId_t   rab_id;
  OaiNwDrvSapId_t           sapi;
  OaiNwDrvQoSTrafficClass_t qos;
  u8                   state;
  u8                   retry;
  u32                  countimer;
  struct rb_entity    *next;
};

struct cx_entity {
  int                        sap[OAI_NW_DRV_SAPI_CX_MAX];
  u8                         state;                     // state of the connection
  OaiNwDrvLocalConnectionRef_t    lcr;                       // Local connection reference
  OaiNwDrvCellID_t                cellid;                    // cell identification
  u32                        countimer;                 // timeout's counter
  u8                         retry;                     // number of retransmission
  struct classifier_entity  *sclassifier[OAI_NW_DRV_DSCP_MAX]; // send classifier;
  struct classifier_entity  *fclassifier[OAI_NW_DRV_DSCP_MAX]; // forward classifier;
  u16                        nsclassifier;
  u16                        nfclassifier;
  u32                        iid6[2];                   // IPv6  interface identification
  u8                         iid4;                      // IPv4 interface identification
  struct rb_entity          *rb;
  u16                        num_rb;                    // number of radio bearer in linked list
  int                        lastRRCprimitive;
  //measures
  int                        req_prov_id [OAI_NW_DRV_MAX_MEASURE_NB];
  int                        num_measures;
  int                        meas_cell_id[OAI_NW_DRV_MAX_MEASURE_NB];
  int                        meas_level  [OAI_NW_DRV_MAX_MEASURE_NB];
  int                        provider_id [OAI_NW_DRV_MAX_MEASURE_NB];
};

struct classifier_entity {
  u32                        classref;  // classifier identity
  struct classifier_entity  *next;      // linked list
  u8                         ip_version;   // IP version 4 or 6
  union{
    struct in6_addr ipv6;
    u32             ipv4;
  }                          saddr;     // IP source address
  u8                         splen;     // IP source prefix length
  union{
    struct in6_addr ipv6;
    u32             ipv4;
    unsigned int    mpls_label;
  }                          daddr;     // IP destination address
  u8                         dplen;     // IP destination prefix length
  u8                         protocol;  // high layer protocol type (TCP, UDP,..)
  unsigned char              protocol_message_type;
  u16                        sport;     // source port
  u16                        dport;     // destination port
  struct rb_entity          *rb;        // pointer to rb_entity for sending function or receiving in case of forwarding rule
  struct rb_entity          *rb_rx;     // pointer to rb_entity for receiving (in case of forwarding rule)
  OaiNwDrvRadioBearerId_t         rab_id;    // RAB identification for sending
  OaiNwDrvRadioBearerId_t         rab_id_rx; // RAB identification for receiving (in case of forwarding rule)
  void (*fct)(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst);
};

//#define NAS_RETRY_LIMIT_DEFAULT 5

struct oai_nw_drv_priv {
  int                        irq;
  int                        rx_flags;
  struct timer_list          timer;
  spinlock_t                 lock;
  struct net_device_stats    stats;
  u8                         retry_limit;
  u32                        timer_establishment;
  u32                        timer_release;
  struct cx_entity           cx[OAI_NW_DRV_CX_MAX];
  struct classifier_entity  *rclassifier[OAI_NW_DRV_DSCP_MAX]; // receive classifier
  u16                        nrclassifier;
  int                        sap[OAI_NW_DRV_SAPI_MAX];
  struct sock               *nl_sk;
  u8                         nlmsg[OAI_NW_DRV_PRIMITIVE_MAX_LENGTH+sizeof(struct nlmsghdr)];
  u8                         xbuffer[OAI_NW_DRV_PRIMITIVE_MAX_LENGTH]; // transmition buffer
  u8                         rbuffer[OAI_NW_DRV_PRIMITIVE_MAX_LENGTH]; // reception buffer
};

struct ipversion {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    u8    reserved:4,
          version:4;
#else
    u8    version:4,
          reserved:4;
#endif
};

typedef struct pdcp_data_req_header_t {
  unsigned int           rb_id;
  unsigned int           data_size;
  int                    inst;
} pdcp_data_req_header_t;

typedef struct pdcp_data_ind_header_t {
  unsigned int           rb_id;
  unsigned int           data_size;
  int                    inst;
} pdcp_data_ind_header_t;



extern struct net_device *oai_nw_drv_dev[OAI_NW_DRV_NB_INSTANCES_MAX];
//extern int bytes_wrote;
//extern int bytes_read;

extern u8 OAI_NW_DRV_NULL_IMEI[14];

//global variables shared with RRC
#ifndef OAI_NW_DRIVER_USE_NETLINK
extern int pdcp_2_oai_nw_drv_irq;
#endif
//extern u8 nas_IMEI[14];

#endif