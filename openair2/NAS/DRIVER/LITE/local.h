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

struct cx_entity {
  int                        sap[OAI_NW_DRV_SAPI_CX_MAX];
  u8                         state;                     // state of the connection
  OaiNwDrvLocalConnectionRef_t    lcr;                       // Local connection reference
  OaiNwDrvCellID_t                cellid;                    // cell identification
  u32                        countimer;                 // timeout's counter
  u8                         retry;                     // number of retransmission

  u32                        iid6[2];                   // IPv6  interface identification
  u8                         iid4;                      // IPv4 interface identification
  int                        lastRRCprimitive;
  //measures
  int                        req_prov_id [OAI_NW_DRV_MAX_MEASURE_NB];
  int                        num_measures;
  int                        meas_cell_id[OAI_NW_DRV_MAX_MEASURE_NB];
  int                        meas_level  [OAI_NW_DRV_MAX_MEASURE_NB];
  int                        provider_id [OAI_NW_DRV_MAX_MEASURE_NB];
};

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
  //struct classifier_entity  *rclassifier[OAI_NW_DRV_DSCP_MAX]; // receive classifier
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

extern u8 OAI_NW_DRV_NULL_IMEI[14];



#endif