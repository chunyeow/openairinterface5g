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

/*! \file common.c
* \brief 
* \author Navid Nikaein and Raymond Knopp, Lionel GAUTHIER
* \date 2013
* \version 1.0
* \company Eurecom
* \email: navid.nikaein@eurecom.fr, lionel.gauthier@eurecom.fr
*/

#include "local.h"
#include "proto_extern.h"
#ifndef OAI_NW_DRIVER_USE_NETLINK
#include "rtai_fifos.h"
#endif


#include <linux/inetdevice.h>

#include <net/tcp.h>
#include <net/udp.h>

#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)

#define NIP6ADDR(addr) \
        ntohs((addr)->s6_addr16[0]), \
        ntohs((addr)->s6_addr16[1]), \
        ntohs((addr)->s6_addr16[2]), \
        ntohs((addr)->s6_addr16[3]), \
        ntohs((addr)->s6_addr16[4]), \
        ntohs((addr)->s6_addr16[5]), \
        ntohs((addr)->s6_addr16[6]), \
        ntohs((addr)->s6_addr16[7])


//#define OAI_DRV_DEBUG_SEND
//#define OAI_DRV_DEBUG_RECEIVE
void ue_ip_common_class_wireless2ip(u16 dlen,
                        void *pdcp_sdu,
                        int inst,
                        OaiNwDrvRadioBearerId_t rb_id) {

  //---------------------------------------------------------------------------
    struct sk_buff      *skb;
    struct ipversion    *ipv;
    struct ue_ip_priv     *gpriv=netdev_priv(ue_ip_dev[inst]);
    unsigned int         hard_header_len;
    #ifdef OAI_DRV_DEBUG_RECEIVE
    int i;
    unsigned char *addr;
    #endif
    unsigned char        protocol;
    struct iphdr        *network_header;

    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[UE_IP_DRV][%s] begin RB %d Inst %d Length %d bytes\n",__FUNCTION__, rb_id,inst,dlen);
    #endif

    skb = dev_alloc_skb( dlen + 2 );

    if(!skb) {
        printk("[UE_IP_DRV][%s] low on memory\n",__FUNCTION__);
        ++gpriv->stats.rx_dropped;
        return;
    }
    skb_reserve(skb,2);
    memcpy(skb_put(skb, dlen), pdcp_sdu,dlen);

    skb->dev = ue_ip_dev[inst];
    hard_header_len = ue_ip_dev[inst]->hard_header_len;

    skb_set_mac_header(skb, 0);
    //skb->mac_header = skb->data;

    //printk("[NAC_COMMIN_RECEIVE]: Packet Type %d (%d,%d)",skb->pkt_type,PACKET_HOST,PACKET_BROADCAST);
    skb->pkt_type = PACKET_HOST;


    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[UE_IP_DRV][%s] Receiving packet of size %d from PDCP \n",__FUNCTION__, skb->len);

    for (i=0;i<skb->len;i++)
        printk("%2x ",((unsigned char *)(skb->data))[i]);
    printk("\n");
    #endif
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[UE_IP_DRV][%s] skb->data           @ %p\n",__FUNCTION__,  skb->data);
    printk("[UE_IP_DRV][%s] skb->mac_header     @ %p\n",__FUNCTION__,  skb->mac_header);
    #endif



        // LG TEST skb->ip_summed = CHECKSUM_NONE;
        skb->ip_summed = CHECKSUM_UNNECESSARY;


        ipv = (struct ipversion *)&(skb->data[hard_header_len]);
        switch (ipv->version) {

            case 6:
                #ifdef OAI_DRV_DEBUG_RECEIVE
                printk("[UE_IP_DRV][%s] receive IPv6 message\n",__FUNCTION__);
                #endif
                skb_set_network_header(skb, hard_header_len);
                //skb->network_header = &skb->data[hard_header_len];

                if (hard_header_len == 0) {
                    skb->protocol = htons(ETH_P_IPV6);
                } else {
                    #ifdef OAI_NW_DRIVER_TYPE_ETHERNET
                    skb->protocol = eth_type_trans(skb, ue_ip_dev[inst]);
                    #else
                    #endif
                }
                //printk("Writing packet with protocol %x\n",ntohs(skb->protocol));
                break;

            case 4:

                #ifdef OAI_DRV_DEBUG_RECEIVE
                //printk("NAS_TOOL_RECEIVE: receive IPv4 message\n");
                addr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->saddr;
                if (addr) {
                    printk("[UE_IP_DRV][%s] Source %d.%d.%d.%d\n",__FUNCTION__, addr[0],addr[1],addr[2],addr[3]);
                }
                addr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->daddr;
                if (addr){
                    printk("[UE_IP_DRV][%s] Dest %d.%d.%d.%d\n",__FUNCTION__, addr[0],addr[1],addr[2],addr[3]);
                }
                printk("[UE_IP_DRV][%s] protocol  %d\n",__FUNCTION__, ((struct iphdr *)&skb->data[hard_header_len])->protocol);
                #endif

                skb->network_header = &skb->data[hard_header_len];
                //network_header = (struct iphdr *)skb_network_header(skb);
                network_header = (struct iphdr *)skb_network_header(skb);
                protocol = network_header->protocol;

                #ifdef OAI_DRV_DEBUG_RECEIVE
                switch (protocol) {
                    case IPPROTO_IP:
                        printk("[UE_IP_DRV][%s] Received Raw IPv4 packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_IPV6:
                        printk("[UE_IP_DRV][%s] Received Raw IPv6 packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_ICMP:
                        printk("[UE_IP_DRV][%s] Received Raw ICMP packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_TCP:
                        printk("[UE_IP_DRV][%s] Received TCP packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_UDP:
                        printk("[UE_IP_DRV][%s] Received UDP packet\n",__FUNCTION__);
                        break;
                    default:
                        break;
                }
                #endif

                if (hard_header_len == 0) {
                    skb->protocol = htons(ETH_P_IP);
                }
                //printk("[UE_IP_DRV][COMMON] Writing packet with protocol %x\n",ntohs(skb->protocol));
                break;

            default:
                printk("[UE_IP_DRV][%s] begin RB %d Inst %d Length %d bytes\n",__FUNCTION__,rb_id,inst,dlen);
                printk("[UE_IP_DRV][%s] Inst %d: receive unknown message (version=%d)\n",__FUNCTION__,inst,ipv->version);
        }

    ++gpriv->stats.rx_packets;
    gpriv->stats.rx_bytes += dlen;
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[UE_IP_DRV][%s] sending packet of size %d to kernel\n",__FUNCTION__,skb->len);
    for (i=0;i<skb->len;i++)
        printk("%2x ",((unsigned char *)(skb->data))[i]);
    printk("\n");
    #endif //OAI_DRV_DEBUG_RECEIVE
    netif_rx(skb);
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[UE_IP_DRV][%s] end\n",__FUNCTION__);
    #endif
}

//---------------------------------------------------------------------------
// Delete the data
void ue_ip_common_ip2wireless_drop(struct sk_buff *skb,  int inst){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv=netdev_priv(ue_ip_dev[inst]);
  ++priv->stats.tx_dropped;
}

//---------------------------------------------------------------------------
// Request the transfer of data (QoS SAP)
void ue_ip_common_ip2wireless(struct sk_buff *skb, int inst){
  //---------------------------------------------------------------------------
  struct pdcp_data_req_header_t     pdcph;
  struct ue_ip_priv *priv=netdev_priv(ue_ip_dev[inst]);
#ifdef LOOPBACK_TEST
  int i;
#endif
#ifdef OAI_DRV_DEBUG_SEND
  int j;
#endif
  unsigned int bytes_wrote;
  // Start debug information
#ifdef OAI_DRV_DEBUG_SEND
  printk("[UE_IP_DRV][%s] inst %d begin \n",__FUNCTION__,inst);
#endif

  if (skb==NULL){
#ifdef OAI_DRV_DEBUG_SEND
    printk("[UE_IP_DRV][%s] input parameter skb is NULL \n",__FUNCTION__);
#endif
    return;
  }


  pdcph.data_size  = skb->len;
  if (skb->mark) {
      pdcph.rb_id      = skb->mark;
  } else {
      pdcph.rb_id      = UE_IP_DEFAULT_RAB_ID;
  }
  pdcph.inst       = inst;


  bytes_wrote = ue_ip_netlink_send((char *)&pdcph,UE_IP_PDCPH_SIZE);
#ifdef OAI_DRV_DEBUG_SEND
  printk("[UE_IP_DRV][%s] Wrote %d bytes (header for %d byte skb) to PDCP via netlink\n",__FUNCTION__,
  	       bytes_wrote,skb->len);
#endif

  if (bytes_wrote != UE_IP_PDCPH_SIZE)
    {
      printk("[UE_IP_DRV][%s] problem while writing PDCP's header (bytes wrote = %d)\n",__FUNCTION__,bytes_wrote);
      printk("rb_id %d, Wrote %d, Header Size %d \n", pdcph.rb_id , bytes_wrote, UE_IP_PDCPH_SIZE);
      priv->stats.tx_dropped ++;
      return;
    }

  bytes_wrote += ue_ip_netlink_send((char *)skb->data,skb->len);


  if (bytes_wrote != skb->len+UE_IP_PDCPH_SIZE)
    {
      printk("[UE_IP_DRV][%s] Inst %d, RB_ID %d: problem while writing PDCP's data, bytes_wrote = %d, Data_len %d, PDCPH_SIZE %d\n",
             __FUNCTION__,
	     inst,
             pdcph.rb_id,
             bytes_wrote,
             skb->len,
             UE_IP_PDCPH_SIZE); // congestion

      priv->stats.tx_dropped ++;
      return;
    }
#ifdef OAI_DRV_DEBUG_SEND
  printk("[UE_IP_DRV][%s] Sending packet of size %d to PDCP \n",__FUNCTION__,skb->len);

 for (j=0;j<skb->len;j++)
    printk("%2x ",((unsigned char *)(skb->data))[j]);
  printk("\n");
#endif

  priv->stats.tx_bytes   += skb->len;
  priv->stats.tx_packets ++;
#ifdef OAI_DRV_DEBUG_SEND
  printk("[UE_IP_DRV][%s] end \n",__FUNCTION__);
#endif
}

//---------------------------------------------------------------------------
void ue_ip_common_wireless2ip(struct nlmsghdr *nlh) {
//---------------------------------------------------------------------------

  struct pdcp_data_ind_header_t     *pdcph = (struct pdcp_data_ind_header_t *)NLMSG_DATA(nlh);
  struct ue_ip_priv *priv;

  priv = netdev_priv(ue_ip_dev[pdcph->inst]);


#ifdef OAI_DRV_DEBUG_RECEIVE
  printk("[UE_IP_DRV][%s] QOS receive from PDCP, size %d, rab %d, inst %d\n",__FUNCTION__,
         pdcph->data_size,pdcph->rb_id,pdcph->inst);
#endif

  ue_ip_common_class_wireless2ip(pdcph->data_size,
                       (unsigned char *)NLMSG_DATA(nlh) + UE_IP_PDCPH_SIZE,
                       pdcph->inst,
                       pdcph->rb_id);

}

