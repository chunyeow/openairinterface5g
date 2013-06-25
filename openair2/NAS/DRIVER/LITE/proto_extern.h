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
                          proto_extern.h  -  description
                             -------------------
    copyright            : (C) 2002 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
                           yan.moret@eurecom.fr
                           knopp@eurecom.fr
               ***************************************************************************

***************************************************************************/

#ifndef _NAS_PROTO_H
#define _NAS_PROTO_H

#include <linux/if_arp.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/ipv6.h>
#include <linux/ip.h>
#include <linux/sysctl.h>
#include <linux/timer.h>
#include <asm/param.h>
//#include <sys/sysctl.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/in.h>
#include <net/ndisc.h>

#include "local.h"

// device.c


/** @defgroup _oai_nw_drv_impl_ OAI Network Device for RRC Lite
* @ingroup _ref_implementation_
* @{
\fn int oai_nw_drv_find_inst(struct net_device *dev)
\brief This function determines the instance id for a particular device pointer.
@param dev Pointer to net_device structure
 */
int oai_nw_drv_find_inst(struct net_device *dev);

// common.c
/**
\fn void oai_nw_drv_common_class_wireless2ip(unsigned short dlen, void* pdcp_sdu,int inst,struct classifier_entity *rclass,OaiNwDrvRadioBearerId_t rb_id)
\brief Receive classified LTE packet, build skbuff struct with it and deliver it to the OS network layer.
@param dlen Length of SDU in bytes
@param pdcp_sdu Pointer to received SDU
@param inst Instance number
@param rclass RX Classifier entity
@param rb_id Radio Bearer Id
 */
void oai_nw_drv_common_class_wireless2ip(unsigned short dlen,
            void *pdcp_sdu,
            int inst,
            struct classifier_entity *rclass,
            OaiNwDrvRadioBearerId_t rb_id);

/**
\fn void oai_nw_drv_common_ip2wireless(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst)
\brief Request the transfer of data (QoS SAP)
@param skb pointer to socket buffer
@param cx pointer to connection entity for SDU
@param gc pointer to classifier entity for SDU
@param inst device instance
 */
void oai_nw_drv_common_ip2wireless(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst);

/**
\fn void oai_nw_drv_common_ip2wireless_drop(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst)
\brief  Drop the IP packet comming from the OS network layer.
@param skb pointer to socket buffer
@param cx pointer to connection entity for SDU
@param gc pointer to classifier entity for SDU
@param inst device instance
 */
void oai_nw_drv_common_ip2wireless_drop(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst);

#ifndef OAI_NW_DRIVER_USE_NETLINK
/**
\fn void oai_nw_drv_common_wireless2ip()
\brief Retrieve PDU from PDCP through RT-fifos for delivery to the IP stack.
 */
void oai_nw_drv_common_wireless2ip(void);
#else
/**
\fn void oai_nw_drv_common_wireless2ip(struct nlmsghdr *nlh)
\brief Retrieve PDU from PDCP through netlink sockets for delivery to the IP stack.
 */
void oai_nw_drv_common_wireless2ip(struct nlmsghdr *nlh);
#endif //OAI_NW_DRIVER_USE_NETLINK

/**
\fn struct rb_entity *oai_nw_drv_common_add_rb(struct cx_entity *cx, OaiNwDrvRadioBearerId_t rabi, OaiNwDrvQoSTrafficClass_t qos)
\brief Add a radio-bearer descriptor
@param gpriv pointer to driver instance private datas
@param cx pointer to connection entity
@param rabi radio-bearer index
@param qos NAS QOS traffic class
 */
struct rb_entity *oai_nw_drv_common_add_rb(struct oai_nw_drv_priv *gpriv, struct cx_entity *cx, OaiNwDrvRadioBearerId_t rab_id, OaiNwDrvQoSTrafficClass_t qos);

/**
\fn struct rb_entity *oai_nw_drv_common_search_rb(struct cx_entity *cx, OaiNwDrvRadioBearerId_t rabi)
\brief Search for a radio-bearer entity for a particular connection and radio-bearer index
@param cx pointer to connection entity
@param rabi radio-bearer index
@returns A pointer to the radio-bearer entity
 */
struct rb_entity *oai_nw_drv_common_search_rb(struct cx_entity *cx, OaiNwDrvRadioBearerId_t rabi);

/**
\fn struct cx_entity *oai_nw_drv_common_search_cx(OaiNwDrvLocalConnectionRef_t lcr,struct oai_nw_drv_priv *gpriv)
\brief  Search for a connection entity based on its index and pointer to oai_nw_drv_priv
@param lcr index of local connection
@param gpriv pointer to oai_nw_drv_priv for device
@returns A pointer to the connection entity
 */
struct cx_entity *oai_nw_drv_common_search_cx(OaiNwDrvLocalConnectionRef_t lcr,struct oai_nw_drv_priv *gpriv);

/**
\fn struct classifier_entity *oai_nw_drv_common_search_class_for_rb(OaiNwDrvRadioBearerId_t rab_id,struct oai_nw_drv_priv *priv)
\brief  Search for an RX classifier entity based on a RB id and pointer to oai_nw_drv_priv
@param rab_id Index of RAB for search
@param priv pointer to oai_nw_drv_priv for device
@returns A pointer to the corresponding RX classifier entity
 */
struct classifier_entity *oai_nw_drv_common_search_class_for_rb(OaiNwDrvRadioBearerId_t rab_id,struct oai_nw_drv_priv *priv);

/**
\fn void oai_nw_drv_common_flush_rb(struct cx_entity *cx)
\brief Clear all RB's for a particular connection
@param cx pointer to connection entity
 */
void oai_nw_drv_common_flush_rb(struct cx_entity *cx);

#ifdef OAI_NW_DRIVER_USE_NETLINK
/**
\fn int oai_nw_drv_netlink_send(unsigned char *data,unsigned int len)
\brief Request the transfer of data by PDCP via netlink socket
@param data pointer to SDU
@param len length of SDU in bytes
@returns Numeber of bytes transfered by netlink socket
 */
int oai_nw_drv_netlink_send(unsigned char *data,unsigned int len);

/**
\fn void oai_nw_drv_COMMON_QOS_receive(struct nlmsghdr *nlh)
\brief Request a PDU from PDCP
@param nlh pointer to netlink message header
 */
void oai_nw_drv_COMMON_QOS_receive(struct nlmsghdr *nlh);

#endif //OAI_NW_DRIVER_USE_NETLINK


// int  oai_nw_drv_mesh_DC_receive(struct cx_entity *cx,struct oai_nw_drv_priv *gpriv);
// int  oai_nw_drv_mesh_GC_receive(struct oai_nw_drv_priv *gpriv);
// int  oai_nw_drv_mesh_DC_send_cx_establish_request(struct cx_entity *cx,struct oai_nw_drv_priv *gpriv);
// int  oai_nw_drv_mesh_DC_send_cx_release_request(struct cx_entity *cx,struct oai_nw_drv_priv *gpriv);
// void oai_nw_drv_mesh_DC_send_sig_data_request(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,struct oai_nw_drv_priv *gpriv);

// iocontrol.c
void oai_nw_drv_CTL_send(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc, int inst);
//int oai_nw_drv_CTL_receive_authentication(struct ipv6hdr *iph, struct cx-entity *cx, unsigned char sapi);
int oai_nw_drv_CTL_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

// classifier.c

/**
  \brief Send a socket received from IP to classifier for a particular instance ID.
*/
void oai_nw_drv_class_send(struct sk_buff *skb,    //!< Pointer to socket buffer
                          int inst                //!< Instance ID
                          );
/**
  \brief
*/
struct classifier_entity *oai_nw_drv_class_add_send_classifier(struct cx_entity *cx, unsigned char dscp, unsigned short classref);

/**
  \brief Send a socket received from IP to classifier for a particular instance ID.
*/
struct classifier_entity *oai_nw_drv_class_add_fwd_classifier(struct cx_entity *cx,
                            unsigned char dscp,
                            unsigned short classref
                            );

/**
  \brief Send a socket received from IP to classifier for a particular instance ID.
*/
struct classifier_entity *oai_nw_drv_class_add_recv_classifier(unsigned char dscp,
                            unsigned short classref,
                            struct oai_nw_drv_priv*
                            );

/**
  \brief
*/
void oai_nw_drv_class_del_send_classifier(struct cx_entity *cx,
                   unsigned char dscp,
                   unsigned short classref
                   );

/**
  \brief
*/
void oai_nw_drv_class_del_fwd_classifier(struct cx_entity *cx,
                   unsigned char dscp,
                   unsigned short classref
                   );

/**
  \brief
*/
void oai_nw_drv_class_del_recv_classifier(unsigned char dscp,
                   unsigned short classref,
                   struct oai_nw_drv_priv*
                   );

/**
  \brief
*/
void oai_nw_drv_class_flush_send_classifier(struct cx_entity *cx);

/**
  \brief
*/
void oai_nw_drv_class_flush_fwd_classifier(struct cx_entity *cx);

/**
  \brief
*/
void oai_nw_drv_class_flush_recv_classifier(struct oai_nw_drv_priv *gpriv);


// tool.c
unsigned char oai_nw_drv_TOOL_invfct(struct classifier_entity *gc);
void oai_nw_drv_TOOL_fct(struct classifier_entity *gc, unsigned char fct);
void oai_nw_drv_TOOL_imei2iid(unsigned char *imei, unsigned char *iid);
void oai_nw_drv_TOOL_eNB_imei2iid(unsigned char *imei, unsigned char *iid, unsigned char len);
unsigned char oai_nw_drv_TOOL_get_dscp6(struct ipv6hdr *iph);
unsigned char oai_nw_drv_TOOL_get_dscp4(struct iphdr *iph);
unsigned char *oai_nw_drv_TOOL_get_protocol6(struct ipv6hdr *iph, unsigned char *protocol);
unsigned char *oai_nw_drv_TOOL_get_protocol4(struct iphdr *iph, unsigned char *protocol);
char *oai_nw_drv_TOOL_get_udpmsg(struct udphdr *udph);
unsigned short oai_nw_drv_TOOL_udpcksum(struct in6_addr *saddr, struct in6_addr *daddr, unsigned char proto, unsigned int udplen, void *data);
int oai_nw_drv_TOOL_network6(struct in6_addr *addr, struct in6_addr *prefix, unsigned char plen);
int oai_nw_drv_TOOL_network4(unsigned int *addr, unsigned int *prefix, unsigned char plen);
void print_TOOL_pk_icmp6(struct icmp6hdr *icmph);

void print_TOOL_pk_all(struct sk_buff *skb);
void print_TOOL_pk_ipv6(struct ipv6hdr *iph);
void print_TOOL_state(unsigned char state);
void oai_nw_drv_tool_print_buffer(char * buffer,int length);
void oai_nw_drv_print_rb_entity(struct rb_entity *rb);
void oai_nw_drv_print_classifier(struct classifier_entity *gc);

#ifdef OAI_NW_DRIVER_USE_NETLINK
// netlink.c

void oai_nw_drv_netlink_release(void);
int oai_nw_drv_netlink_init(void);

#endif

/** @} */
#endif
