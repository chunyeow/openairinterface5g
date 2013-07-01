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
* \brief implementation of emultor tx and rx
* \author Navid Nikaein and Raymomd Knopp, Lionel GAUTHIER
* \date 2011
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
#ifdef OAI_NW_DRIVER_TYPE_ETHERNET
#include <linux/etherdevice.h>
#endif

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
void oai_nw_drv_common_class_wireless2ip(u16 dlen,
                        void *pdcp_sdu,
                        int inst,
                        struct classifier_entity *rclass,
                        OaiNwDrvRadioBearerId_t rb_id) {

  //---------------------------------------------------------------------------
    struct sk_buff      *skb;
    struct ipversion    *ipv;
    struct oai_nw_drv_priv     *gpriv=netdev_priv(oai_nw_drv_dev[inst]);
    unsigned int         hard_header_len = 0;
    u16                 *p_ether_type;
    u16                  ether_type;
    #ifdef OAI_DRV_DEBUG_RECEIVE
    int i;
    unsigned char *addr;
    #endif
    unsigned char        protocol;
    struct iphdr        *network_header;

    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] begin RB %d Inst %d Length %d bytes\n",__FUNCTION__, rb_id,inst,dlen);
    #endif
    if (rclass == NULL) {
        printk("[OAI_IP_DRV][%s] rclass Not found Drop RX packet\n",__FUNCTION__);
        ++gpriv->stats.rx_dropped;
        return;
    }
    skb = dev_alloc_skb( dlen + 2 );

    if(!skb) {
        printk("[OAI_IP_DRV][%s] low on memory\n",__FUNCTION__);
        ++gpriv->stats.rx_dropped;
        return;
    }
    skb_reserve(skb,2);
    memcpy(skb_put(skb, dlen), pdcp_sdu,dlen);

    skb->dev = oai_nw_drv_dev[inst];
    hard_header_len = oai_nw_drv_dev[inst]->hard_header_len;

    #ifdef KERNEL_VERSION_GREATER_THAN_2622
    skb->mac_header = skb->data;
    #else
    skb->mac.raw = skb->data;
    #endif
    //printk("[NAC_COMMIN_RECEIVE]: Packet Type %d (%d,%d)",skb->pkt_type,PACKET_HOST,PACKET_BROADCAST);
    skb->pkt_type = PACKET_HOST;


    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] Receiving packet of size %d from PDCP \n",__FUNCTION__, skb->len);

    for (i=0;i<skb->len;i++)
        printk("%2x ",((unsigned char *)(skb->data))[i]);
    printk("\n");
    #endif
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] skb->data           @ %p\n",__FUNCTION__,  skb->data);
    printk("[OAI_IP_DRV][%s] skb->mac_header     @ %p\n",__FUNCTION__,  skb->mac_header);
    #endif


    if (rclass->ip_version != OAI_NW_DRV_MPLS_VERSION_CODE) {  // This is an IP packet

        // LG TEST skb->ip_summed = CHECKSUM_NONE;
        skb->ip_summed = CHECKSUM_UNNECESSARY;


        ipv = (struct ipversion *)&(skb->data[hard_header_len]);
        switch (ipv->version) {

            case 6:
                #ifdef OAI_DRV_DEBUG_RECEIVE
                printk("[OAI_IP_DRV][%s] receive IPv6 message\n",__FUNCTION__);
                #endif
                #ifdef KERNEL_VERSION_GREATER_THAN_2622
                skb->network_header = &skb->data[hard_header_len];
                #else
                skb->nh.ipv6h = (struct ipv6hdr *)&skb->data[hard_header_len];
                #endif
                if (hard_header_len == 0) {
                    skb->protocol = htons(ETH_P_IPV6);
                } else {
                    #ifdef OAI_NW_DRIVER_TYPE_ETHERNET
                    skb->protocol = eth_type_trans(skb, oai_nw_drv_dev[inst]);
                    #else
                    #endif
                }
                //printk("Writing packet with protocol %x\n",ntohs(skb->protocol));
                break;

            case 4:
                #ifdef NAS_ADDRESS_FIX
                // Make the third byte of both the source and destination equal to the fourth of the destination
                daddr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->daddr;
                odaddr = ((struct iphdr *)skb->data)->daddr;
                //sn = addr[3];
                saddr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->saddr;
                osaddr = ((struct iphdr *)&skb->data[hard_header_len])->saddr;

                if (daddr[0] == saddr[0]) {// same network
                    daddr[2] = daddr[3]; // set third byte of destination to that of local machine so that local IP stack accepts the packet
                    saddr[2] = daddr[3]; // set third byte of source to that of local machine so that local IP stack accepts the packet
                }  else { // get the 3rd byte from device address in net_device structure
                    ifaddr = (unsigned char *)(&(((struct in_device *)((oai_nw_drv_dev[inst])->ip_ptr))->ifa_list->ifa_local));
                    if (saddr[0] == ifaddr[0]) { // source is in same network as local machine
                        daddr[0] += saddr[3];        // fix address of remote destination to undo change at source
                        saddr[2] =  ifaddr[2];       // set third byte to that of local machine so that local IP stack accepts the packet
                    } else {                         // source is remote machine from outside network
                        saddr[0] -= daddr[3];        // fix address of remote source to be understood by destination
                        daddr[2] =  daddr[3];        // fix 3rd byte of local address to be understood by IP stack of
                        // destination
                    }
                }
                #endif //NAS_ADDRESS_FIX
                #ifdef OAI_DRV_DEBUG_RECEIVE
                //printk("NAS_TOOL_RECEIVE: receive IPv4 message\n");
                addr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->saddr;
                if (addr) {
                //addr[2]^=0x01;
                printk("[OAI_IP_DRV][%s] Source %d.%d.%d.%d\n",__FUNCTION__, addr[0],addr[1],addr[2],addr[3]);
                }
                addr = (unsigned char *)&((struct iphdr *)&skb->data[hard_header_len])->daddr;
                if (addr){
                //addr[2]^=0x01;
                printk("[OAI_IP_DRV][%s] Dest %d.%d.%d.%d\n",__FUNCTION__, addr[0],addr[1],addr[2],addr[3]);
                }
                printk("[OAI_IP_DRV][%s] protocol  %d\n",__FUNCTION__, ((struct iphdr *)&skb->data[hard_header_len])->protocol);
                #endif

                #ifdef KERNEL_VERSION_GREATER_THAN_2622
                skb->network_header = &skb->data[hard_header_len];
                network_header = (struct iphdr *)skb_network_header(skb);
                protocol = network_header->protocol;

                #else
                skb->nh.iph = (struct iphdr *)&skb->data[hard_header_len];
                protocol=skb->nh.iph->protocol;
                #endif

                #ifdef OAI_DRV_DEBUG_RECEIVE
                switch (protocol) {
                    case IPPROTO_IP:
                        printk("[OAI_IP_DRV][%s] Received Raw IPv4 packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_IPV6:
                        printk("[OAI_IP_DRV][%s] Received Raw IPv6 packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_ICMP:
                        printk("[OAI_IP_DRV][%s] Received Raw ICMP packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_TCP:
                        printk("[OAI_IP_DRV][%s] Received TCP packet\n",__FUNCTION__);
                        break;
                    case IPPROTO_UDP:
                        printk("[OAI_IP_DRV][%s] Received UDP packet\n",__FUNCTION__);
                        break;
                    default:
                        break;
                }
                #endif

                #ifdef NAS_ADDRESS_FIX
                    #ifdef KERNEL_VERSION_GREATER_THAN_2622
                    network_header->check = 0;
                    network_header->check = ip_fast_csum((unsigned char *) network_header, network_header->ihl);
                    //printk("[OAI_IP_DRV][COMMON][RECEIVE] IP Fast Checksum %x \n", network_header->check);
                    #else
                    skb->nh.iph->check = 0;
                    skb->nh.iph->check = ip_fast_csum((unsigned char *)&skb->data[hard_header_len], skb->nh.iph->ihl);
                    //    if (!(skb->nh.iph->frag_off & htons(IP_OFFSET))) {
                    #endif

                switch(protocol) {
                    case IPPROTO_TCP:

                        #ifdef KERNEL_VERSION_GREATER_THAN_2622
                        cksum  = (u16*)&(((struct tcphdr*)((network_header + (network_header->ihl<<2))))->check);
                        check  = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, 0,0, ~(*cksum));
                        //check  = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, tcp_hdrlen(skb), IPPROTO_TCP, ~(*cksum));
                        //check  = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, dlen, IPPROTO_TCP, ~(*cksum));
                        #else
                        cksum  = (u16*)&(((struct tcphdr*)((skb->data + (skb->nh.iph->ihl<<2))))->check);
                        check  = csum_tcpudp_magic(((struct iphdr *)skb->data)->saddr, ((struct iphdr *)skb->data)->daddr,0,0, ~(*cksum));
                        //check  = csum_tcpudp_magic(((struct iphdr *)skb->data)->saddr, ((struct iphdr *)skb->data)->daddr,tcp_hdrlen(skb), IPPROTO_TCP, ~(*cksum));
                        // check  = csum_tcpudp_magic(((struct iphdr *)skb->data)->saddr, ((struct iphdr *)skb->data)->daddr, dlen, IPPROTO_TCP, ~(*cksum));
                        #endif

                        *cksum = csum_tcpudp_magic(~osaddr, ~odaddr, 0, 0, ~check);
                        //*cksum = csum_tcpudp_magic(~osaddr, ~odaddr, dlen, IPPROTO_TCP, ~check);
                        #ifdef OAI_DRV_DEBUG_RECEIVE
                        printk("[OAI_IP_DRV][%s] Inst %d TCP packet calculated CS %x, CS = %x (before), SA (%x)%x, DA (%x)%x\n",__FUNCTION__,
                                inst,
                                network_header->check,
                                *cksum,
                                osaddr,
                                ((struct iphdr *)skb->data)->saddr,
                                odaddr,
                                ((struct iphdr *)skb->data)->daddr);

                        printk("[OAI_IP_DRV][%s] Inst %d TCP packet NEW CS %x\n",__FUNCTION__,
                                inst,
                                *cksum);
                        #endif
                        break;

                    case IPPROTO_UDP:
                        #ifdef KERNEL_VERSION_GREATER_THAN_2622
                        cksum  = (u16*)&(((struct udphdr*)((network_header + (network_header->ihl<<2))))->check);
                        check = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, 0,0, ~(*cksum));
                        // check = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, udp_hdr(skb)->len, IPPROTO_UDP, ~(*cksum));
                        //check = csum_tcpudp_magic(((struct iphdr *)network_header)->saddr, ((struct iphdr *)network_header)->daddr, dlen, IPPROTO_UDP, ~(*cksum));
                        #else
                        cksum  = (u16*)&(((struct udphdr*)((&skb->data[hard_header_len] + (skb->nh.iph->ihl<<2))))->check);
                        check = csum_tcpudp_magic(((struct iphdr *)&skb->data[hard_header_len])->saddr, ((struct iphdr *)&skb->data[hard_header_len])->daddr, 0,0, ~(*cksum));
                        //check = csum_tcpudp_magic(((struct iphdr *)skb->data)->saddr, ((struct iphdr *)skb->data)->daddr, udp_hdr(skb)->len, IPPROTO_UDP, ~(*cksum));
                        //check = csum_tcpudp_magic(((struct iphdr *)skb->data)->saddr, ((struct iphdr *)skb->data)->daddr, dlen, IPPROTO_UDP, ~(*cksum));
                        #endif
                        *cksum= csum_tcpudp_magic(~osaddr, ~odaddr,0,0, ~check);
                        //*cksum= csum_tcpudp_magic(~osaddr, ~odaddr,udp_hdr(skb)->len, IPPROTO_UDP, ~check);
                        //*cksum= csum_tcpudp_magic(~osaddr, ~odaddr,dlen, IPPROTO_UDP, ~check);

                        #ifdef OAI_DRV_DEBUG_RECEIVE
                        printk("[OAI_IP_DRV][%s] Inst %d UDP packet CS = %x (before), SA (%x)%x, DA (%x)%x\n",__FUNCTION__,
                           inst,*cksum,osaddr,((struct iphdr *)&skb->data[hard_header_len])->saddr,odaddr,((struct iphdr *)&skb->data[hard_header_len])->daddr);

                        printk("[OAI_IP_DRV][%s] Inst %d UDP packet NEW CS %x\n",__FUNCTION__,inst,*cksum);
                        #endif
                        //if ((check = *cksum) != 0) {
                        // src, dst, len, proto, sum
                        //          }
                        break;

                    default:
                       break;
                }
                //#endif  // KERNEL VERSION > 22
                #endif //NAS_ADDRESS_FIX
                if (hard_header_len == 0) {
                    skb->protocol = htons(ETH_P_IP);
                } else {
                    #ifdef OAI_NW_DRIVER_TYPE_ETHERNET
                    skb->protocol = eth_type_trans(skb, oai_nw_drv_dev[inst]);
                    #else
                    #endif
                }
                //printk("[OAI_IP_DRV][COMMON] Writing packet with protocol %x\n",ntohs(skb->protocol));
                break;

            default:
                #ifdef OAI_NW_DRIVER_TYPE_ETHERNET
                // fill skb->pkt_type, skb->dev
                #ifdef OAI_DRV_DEBUG_RECEIVE
                printk("[OAI_IP_DRV][%s] skb->data           @ %p\n",__FUNCTION__, skb->data);
                printk("[OAI_IP_DRV][%s] skb->network_header @ %p\n",__FUNCTION__, skb->network_header);
                printk("[OAI_IP_DRV][%s] skb->mac_header     @ %p\n",__FUNCTION__, skb->mac_header);
                #endif
                skb->protocol = eth_type_trans(skb, oai_nw_drv_dev[inst]);
                // minus 1(short) instead of 2(bytes) because u16*
                p_ether_type = (u16 *)&(skb->mac_header[hard_header_len-2]);
                ether_type = ntohs(*p_ether_type);
                #ifdef OAI_DRV_DEBUG_RECEIVE
                printk("[OAI_IP_DRV][%s] Packet is not IPv4 or IPv6 ether_type=%04X\n",__FUNCTION__, ether_type);
                printk("[OAI_IP_DRV][%s] skb->data           @ %p\n",__FUNCTION__, skb->data);
                printk("[OAI_IP_DRV][%s] skb->network_header @ %p\n",__FUNCTION__, skb->network_header);
                printk("[OAI_IP_DRV][%s] skb->mac_header     @ %p\n",__FUNCTION__, skb->mac_header);
                #endif
                switch (ether_type) {
                    case ETH_P_ARP:
                        #ifdef OAI_DRV_DEBUG_RECEIVE
                        printk("[OAI_IP_DRV][%s] ether_type = ETH_P_ARP\n",__FUNCTION__);
                        #endif
                        //skb->pkt_type = PACKET_HOST;
                        skb->protocol = htons(ETH_P_ARP);
                        #ifdef KERNEL_VERSION_GREATER_THAN_2622
                        skb->network_header = &skb->mac_header[hard_header_len];
                        #else
                        skb->nh.iph = (struct iphdr *)&skb->data[hard_header_len];
                        #endif
                        break;
                    default:
                        ;
                }
                #else
                printk("[OAI_IP_DRV][%s] begin RB %d Inst %d Length %d bytes\n",__FUNCTION__,rb_id,inst,dlen);
                printk("[OAI_IP_DRV][%s] Inst %d: receive unknown message (version=%d)\n",__FUNCTION__,inst,ipv->version);
                #endif
        }
    } else {  // This is an MPLS packet
        #ifdef OAI_DRV_DEBUG_RECEIVE
        printk("[OAI_IP_DRV][%s] Received an MPLS packet on RB %d\n",__FUNCTION__,rb_id);
        #endif
        if (hard_header_len == 0) {
            skb->protocol = htons(ETH_P_MPLS_UC);
        } else {
            #ifdef OAI_NW_DRIVER_TYPE_ETHERNET
            skb->protocol = eth_type_trans(skb, oai_nw_drv_dev[inst]);
            #endif
        }
    }
    ++gpriv->stats.rx_packets;
    gpriv->stats.rx_bytes += dlen;
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] sending packet of size %d to kernel\n",__FUNCTION__,skb->len);
    for (i=0;i<skb->len;i++)
        printk("%2x ",((unsigned char *)(skb->data))[i]);
    printk("\n");
    #endif //OAI_DRV_DEBUG_RECEIVE
    netif_rx(skb);
    #ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] end\n",__FUNCTION__);
    #endif
}

//---------------------------------------------------------------------------
// Delete the data
void oai_nw_drv_common_ip2wireless_drop(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *sp,int inst){
  //---------------------------------------------------------------------------
  struct oai_nw_drv_priv *priv=netdev_priv(oai_nw_drv_dev[inst]);
  ++priv->stats.tx_dropped;
}

//---------------------------------------------------------------------------
// Request the transfer of data (QoS SAP)
void oai_nw_drv_common_ip2wireless(struct sk_buff *skb, struct cx_entity *cx, struct classifier_entity *gc,int inst){
  //---------------------------------------------------------------------------
  struct pdcp_data_req_header_t     pdcph;
  struct oai_nw_drv_priv *priv=netdev_priv(oai_nw_drv_dev[inst]);
#ifdef LOOPBACK_TEST
  int i;
#endif
#ifdef OAI_DRV_DEBUG_SEND
  int j;
#endif
  unsigned int bytes_wrote;
  // Start debug information
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s] inst %d begin \n",__FUNCTION__,inst);
#endif
  //    if (cx->state!=NAS_STATE_CONNECTED) // <--- A REVOIR
  //    {
  //            priv->stats.tx_dropped ++;
  //            printk("NAS_QOS_SEND: No connected, so message are dropped \n");
  //            return;
  //    }
  if (skb==NULL){
#ifdef OAI_DRV_DEBUG_SEND
    printk("[OAI_IP_DRV][%s] input parameter skb is NULL \n",__FUNCTION__);
#endif
    return;
  }
  if (gc==NULL){
#ifdef OAI_DRV_DEBUG_SEND
    printk("[OAI_IP_DRV][%s] input parameter gc is NULL \n",__FUNCTION__);
#endif
    priv->stats.tx_dropped ++;
    return;
  }
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_SEND
    printk("[OAI_IP_DRV][%s] input parameter cx is NULL \n",__FUNCTION__);
#endif
    priv->stats.tx_dropped ++;
    return;
  }
  // End debug information
  if (gc->rb==NULL)
    {
      gc->rb=oai_nw_drv_common_search_rb(cx, gc->rab_id);
      if (gc->rb==NULL)
        {
          ++priv->stats.tx_dropped;
          printk("[OAI_IP_DRV][%s] No corresponding Radio Bearer, so message are dropped, rab_id=%u \n", __FUNCTION__, gc->rab_id);
          return;
        }
    }
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s] #1 :",__FUNCTION__);
  printk("lcr %u, rab_id %u, rab_id %u, skb_len %d\n", cx->lcr, (gc->rb)->rab_id, gc->rab_id,skb->len);
  oai_nw_drv_print_classifier(gc);
#endif
  pdcph.data_size  = skb->len;
  pdcph.rb_id      = (gc->rb)->rab_id;
  pdcph.inst       = inst;


#ifdef OAI_NW_DRIVER_USE_NETLINK
  bytes_wrote = oai_nw_drv_netlink_send((char *)&pdcph,OAI_NW_DRV_PDCPH_SIZE);
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s] Wrote %d bytes (header for %d byte skb) to PDCP via netlink\n",__FUNCTION__,
  	       bytes_wrote,skb->len);
#endif
#else
  bytes_wrote = rtf_put(IP2PDCP_FIFO, &pdcph, OAI_NW_DRV_PDCPH_SIZE);
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s]Wrote %d bytes (header for %d byte skb) to PDCP fifo\n",__FUNCTION__,
               bytes_wrote,skb->len);
#endif
#endif //OAI_NW_DRIVER_USE_NETLINK

  if (bytes_wrote != OAI_NW_DRV_PDCPH_SIZE)
    {
      printk("[OAI_IP_DRV][%s] problem while writing PDCP's header (bytes wrote = %d to fifo %d)\n",__FUNCTION__,bytes_wrote,IP2PDCP_FIFO);
      printk("rb_id %d, Wrote %d, Header Size %d \n", pdcph.rb_id , bytes_wrote, OAI_NW_DRV_PDCPH_SIZE);
#ifndef OAI_NW_DRIVER_USE_NETLINK
      rtf_reset(IP2PDCP_FIFO);
#endif //OAI_NW_DRIVER_USE_NETLINK
      priv->stats.tx_dropped ++;
      return;
    }

#ifdef  OAI_NW_DRIVER_USE_NETLINK
  bytes_wrote += oai_nw_drv_netlink_send((char *)skb->data,skb->len);
#else
  bytes_wrote += rtf_put(IP2PDCP_FIFO, skb->data, skb->len);
#endif //OAI_NW_DRIVER_USE_NETLINK

  if (bytes_wrote != skb->len+OAI_NW_DRV_PDCPH_SIZE)
    {
      printk("[OAI_IP_DRV][%s] Inst %d, RB_ID %d: problem while writing PDCP's data, bytes_wrote = %d, Data_len %d, PDCPH_SIZE %d\n",
             __FUNCTION__,
	     inst,
             pdcph.rb_id,
             bytes_wrote,
             skb->len,
             OAI_NW_DRV_PDCPH_SIZE); // congestion
#ifndef OAI_NW_DRIVER_USE_NETLINK
      rtf_reset(IP2PDCP_FIFO);
#endif //OAI_NW_DRIVER_USE_NETLINK
      priv->stats.tx_dropped ++;
      return;
    }
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s] Sending packet of size %d to PDCP \n",__FUNCTION__,skb->len);

 for (j=0;j<skb->len;j++)
    printk("%2x ",((unsigned char *)(skb->data))[j]);
  printk("\n");
#endif

  priv->stats.tx_bytes   += skb->len;
  priv->stats.tx_packets ++;
#ifdef OAI_DRV_DEBUG_SEND
  printk("[OAI_IP_DRV][%s] end \n",__FUNCTION__);
#endif
}

#ifndef OAI_NW_DRIVER_USE_NETLINK
//---------------------------------------------------------------------------
void oai_nw_drv_common_wireless2ip(void){
  //---------------------------------------------------------------------------
  u8 sapi;
  struct pdcp_data_ind_header_t     pdcph;
  unsigned char data_buffer[2048];
  struct classifier_entity *rclass;
  struct oai_nw_drv_priv *priv;
  int bytes_read;

  // Start debug information
#ifdef OAI_DRV_DEBUG_RECEIVE
  printk("[OAI_IP_DRV][%s] - begin \n", __FUNCTION__);
#endif

  // End debug information

  bytes_read =  rtf_get(PDCP2IP_FIFO,&pdcph, OAI_NW_DRV_PDCPH_SIZE);

  while (bytes_read>0) {
    if (bytes_read != OAI_NW_DRV_PDCPH_SIZE)
      {
        printk("[OAI_IP_DRV][%s] problem while reading PDCP header\n", __FUNCTION__);
        return;
      }

    priv=netdev_priv(oai_nw_drv_dev[pdcph.inst]);
    rclass = oai_nw_drv_common_search_class_for_rb(pdcph.rb_id,priv);

    bytes_read+= rtf_get(PDCP2IP_FIFO,
                         data_buffer,
                         pdcph.data_size);

#ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] - Got header for RB %d, Inst %d \n",__FUNCTION__,
           pdcph.rb_id,
           pdcph.inst);
#endif

    if (rclass) {
#ifdef OAI_DRV_DEBUG_RECEIVE
      printk("[OAI_IP_DRV][%s] Found corresponding connection in classifier for RAB\n",__FUNCTION__);
#endif //OAI_DRV_DEBUG_RECEIVE

      oai_nw_drv_common_class_wireless2ip(pdcph.data_size,
                         (void *)data_buffer,
                         pdcph.inst,
                         rclass,
                         pdcph.rb_id);
    } else {
      priv->stats.tx_dropped += 1;
    }

    bytes_read =  rtf_get(PDCP2IP_FIFO, &pdcph, OAI_NW_DRV_PDCPH_SIZE);
  }
#ifdef OAI_DRV_DEBUG_RECEIVE
  printk("[OAI_IP_DRV][%s] - end \n",__FUNCTION__);
#endif
}
#else
//---------------------------------------------------------------------------
void oai_nw_drv_common_wireless2ip(struct nlmsghdr *nlh) {
//---------------------------------------------------------------------------

  struct pdcp_data_ind_header_t     *pdcph = (struct pdcp_data_ind_header_t *)NLMSG_DATA(nlh);
  struct classifier_entity *rclass;
  struct oai_nw_drv_priv *priv;

  priv = netdev_priv(oai_nw_drv_dev[pdcph->inst]);


#ifdef OAI_DRV_DEBUG_RECEIVE
  printk("[OAI_IP_DRV][%s] QOS receive from PDCP, size %d, rab %d, inst %d\n",__FUNCTION__,
         pdcph->data_size,pdcph->rb_id,pdcph->inst);
#endif //OAI_DRV_DEBUG_RECEIVE

  rclass = oai_nw_drv_common_search_class_for_rb(pdcph->rb_id,priv);

  if (rclass) {
#ifdef OAI_DRV_DEBUG_RECEIVE
    printk("[OAI_IP_DRV][%s] Found corresponding connection in classifier for RAB\n",__FUNCTION__);
#endif //OAI_DRV_DEBUG_RECEIVE

    oai_nw_drv_common_class_wireless2ip(pdcph->data_size,
                       (unsigned char *)NLMSG_DATA(nlh) + OAI_NW_DRV_PDCPH_SIZE,
                       pdcph->inst,
                       rclass,
                       pdcph->rb_id);
  } else {
      priv->stats.tx_dropped += 1;
  }
}
#endif //OAI_NW_DRIVER_USE_NETLINK

//---------------------------------------------------------------------------
struct cx_entity *oai_nw_drv_common_search_cx(OaiNwDrvLocalConnectionRef_t lcr,struct oai_nw_drv_priv *priv){
  //---------------------------------------------------------------------------
#ifdef OAI_DRV_DEBUG_CLASS
  printk("[OAI_IP_DRV][%s] - lcr %d\n",__FUNCTION__,lcr);
#endif
  if (lcr<OAI_NW_DRV_CX_MAX)
    return priv->cx+lcr;
  else
    return NULL;
}

//---------------------------------------------------------------------------
// Search a Radio Bearer
struct rb_entity *oai_nw_drv_common_search_rb(struct cx_entity *cx, OaiNwDrvRadioBearerId_t rab_id){
  //---------------------------------------------------------------------------
  struct rb_entity *rb;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("[OAI_IP_DRV][%s] - rab_id %d\n",__FUNCTION__, rab_id);
#endif
  for (rb=cx->rb; rb!=NULL; rb=rb->next)
    {
      if (rb->rab_id==rab_id)
        return rb;
    }
  return NULL;
}

//
// Search for a classifier with corresponding radio bearer

//---------------------------------------------------------------------------
struct classifier_entity *oai_nw_drv_common_search_class_for_rb(OaiNwDrvRadioBearerId_t rab_id,struct oai_nw_drv_priv *priv) {
//---------------------------------------------------------------------------

  //struct rb_entity *rb;
  int dscp;
  struct classifier_entity *rclass;

#ifdef OAI_DRV_DEBUG_CLASS
  printk("[OAI_IP_DRV][%s] - rab_id %d\n",__FUNCTION__, rab_id);
#endif
  for (dscp=0;dscp<OAI_NW_DRV_DSCP_MAX;dscp++) {

    //      printk("[OAI_IP_DRV][COMMON] priv->rclassifier[%d] = %p\n",dscp,priv->rclassifier[dscp]);
    for (rclass=priv->rclassifier[dscp]; rclass!=NULL; rclass=rclass->next) {
#ifdef OAI_DRV_DEBUG_CLASS
      printk("[OAI_IP_DRV][%s] - dscp %d, rb %d\n",__FUNCTION__, dscp,rclass->rab_id);
#endif
      if (rclass->rab_id==rab_id)
        return rclass;
    }
  }
  return NULL;

}

//---------------------------------------------------------------------------
struct rb_entity *oai_nw_drv_common_add_rb(struct oai_nw_drv_priv *gpriv, struct cx_entity *cx, OaiNwDrvRadioBearerId_t rab_id, OaiNwDrvQoSTrafficClass_t qos){
  //--------------------------------------------------------------------------
    struct rb_entity         *rb;
    //struct classifier_entity *pclassifier;
    //struct classifier_entity *rclassifier;

    #ifdef OAI_DRV_DEBUG_CLASS
    printk("[OAI_IP_DRV][%s] begin for rab_id %d , qos %d\n",__FUNCTION__, rab_id, qos );
    #endif
    if (cx==NULL){
        #ifdef OAI_DRV_DEBUG_CLASS
        printk("[OAI_IP_DRV][%s] input parameter cx is NULL \n",__FUNCTION__);
        #endif
        return NULL;
    }
    rb=oai_nw_drv_common_search_rb(cx, rab_id);
    if (rb==NULL) {
        rb=(struct rb_entity *)kmalloc(sizeof(struct rb_entity), GFP_KERNEL);
        if (rb!=NULL) {
            rb->retry=0;
            rb->countimer=OAI_NW_DRV_TIMER_IDLE;
            rb->rab_id=rab_id;
            //rb->rab_id=rab_id+(32*cx->lcr);
            #ifdef OAI_DRV_DEBUG_DC
            printk("[OAI_IP_DRV][%s] rab_id=%u, mt_id=%u\n",__FUNCTION__,rb->rab_id, cx->lcr);
            #endif
            rb->qos=qos;
            rb->sapi=OAI_NW_DRV_RAB_INPUT_SAPI;
            rb->state=OAI_NW_DRV_IDLE;
            rb->next=cx->rb;
            cx->rb=rb;
            ++cx->num_rb;
        } else {
            printk("[OAI_IP_DRV][%s] NAS_ADD_CTL_RB: no memory\n",__FUNCTION__);
        }
        /*if (cx->num_rb == 1) {
            // first RAB added, add default classification rule for multicast signalling
            pclassifier=oai_nw_drv_class_add_send_classifier(cx, OAI_NW_DRV_DSCP_DEFAULT, 0);
            if (pclassifier != NULL) {
              pclassifier->rab_id      = rab_id;
              pclassifier->rb          = rb;
              oai_nw_drv_TOOL_fct(pclassifier, OAI_NW_DRV_FCT_QOS_SEND);
              pclassifier->ip_version  = OAI_NW_DRV_IP_VERSION_6;
              memset((u8*)&pclassifier->saddr.ipv6,0,16);
              memset((u8*)&pclassifier->daddr.ipv6,0,16);
              printk("[OAI_IP_DRV][%s] ADD DEFAULT TX CLASSIFIER ON RAB %d OAI_NW_DRV_DSCP_DEFAULT Adding IPv6 %X:%X:%X:%X:%X:%X:%X:%X -> %X:%X:%X:%X:%X:%X:%X:%X \n",
                      __FUNCTION__, rab_id, NIP6ADDR(&pclassifier->saddr.ipv6), NIP6ADDR(&pclassifier->daddr.ipv6));
              pclassifier->splen                 = 0;
              pclassifier->dplen                 = 0;
              pclassifier->protocol              = OAI_NW_DRV_PROTOCOL_DEFAULT;
              pclassifier->protocol_message_type = 0; //LG ??
              pclassifier->sport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
              pclassifier->dport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
            }
            // first RAB added, add default classification rule for multicast signalling
            rclassifier=oai_nw_drv_class_add_recv_classifier(OAI_NW_DRV_DSCP_DEFAULT, 1, gpriv);
            if (rclassifier != NULL) {
              rclassifier->rab_id      = rab_id;
              rclassifier->rb          = rb;
              //oai_nw_drv_TOOL_fct(rclassifier, OAI_NW_DRV_FCT_QOS_SEND);
              rclassifier->ip_version  = OAI_NW_DRV_IP_VERSION_6;
              memset((u8*)&rclassifier->saddr.ipv6,0,16);
              memset((u8*)&rclassifier->daddr.ipv6,0,16);
              printk("[OAI_IP_DRV][%s] ADD DEFAULT RX CLASSIFIER ON RAB %d OAI_NW_DRV_DSCP_DEFAULT Adding IPv6 %X:%X:%X:%X:%X:%X:%X:%X -> %X:%X:%X:%X:%X:%X:%X:%X \n",
                      __FUNCTION__, rab_id, NIP6ADDR(&rclassifier->saddr.ipv6), NIP6ADDR(&rclassifier->daddr.ipv6));
              rclassifier->splen                 = 0;
              rclassifier->dplen                 = 0;
              rclassifier->protocol              = OAI_NW_DRV_PROTOCOL_DEFAULT;
              rclassifier->protocol_message_type = 0; //LG ??
              rclassifier->sport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
              rclassifier->dport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
            }
            pclassifier=oai_nw_drv_class_add_send_classifier(cx, OAI_NW_DRV_DSCP_DEFAULT, 2);
            if (pclassifier != NULL) {
              pclassifier->rab_id      = rab_id;
              pclassifier->rb          = rb;
              oai_nw_drv_TOOL_fct(pclassifier, OAI_NW_DRV_FCT_QOS_SEND);
              pclassifier->ip_version  = OAI_NW_DRV_IP_VERSION_4;
              memset((u8*)&pclassifier->saddr.ipv4,0,4);
              memset((u8*)&pclassifier->daddr.ipv4,0,4);
              printk("[OAI_IP_DRV][%s] ADD DEFAULT TX CLASSIFIER ON RAB %d OAI_NW_DRV_DSCP_DEFAULT Adding IPv4 %d:%d:%d:%d -> %d.%d.%d.%d\n",
                      __FUNCTION__, rab_id, NIPADDR(pclassifier->saddr.ipv4), NIPADDR(pclassifier->daddr.ipv4));
              pclassifier->splen                 = 0;
              pclassifier->dplen                 = 0;
              pclassifier->protocol              = OAI_NW_DRV_PROTOCOL_DEFAULT;
              pclassifier->protocol_message_type = 0; //LG ??
              pclassifier->sport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
              pclassifier->dport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
            }
            // first RAB added, add default classification rule for multicast signalling
            rclassifier=oai_nw_drv_class_add_recv_classifier(OAI_NW_DRV_DSCP_DEFAULT, 3, gpriv);
            if (rclassifier != NULL) {
              rclassifier->rab_id      = rab_id;
              rclassifier->rb          = rb;
              //oai_nw_drv_TOOL_fct(rclassifier, OAI_NW_DRV_FCT_QOS_SEND);
              rclassifier->ip_version  = OAI_NW_DRV_IP_VERSION_4;
              memset((u8*)&rclassifier->saddr.ipv4,0,4);
              memset((u8*)&rclassifier->daddr.ipv4,0,4);
              printk("[OAI_IP_DRV][%s] ADD DEFAULT RX CLASSIFIER ON RAB %d OAI_NW_DRV_DSCP_DEFAULT Adding IPv4 %d:%d:%d:%d -> %d.%d.%d.%d\n",
                      __FUNCTION__, rab_id, NIPADDR(rclassifier->saddr.ipv4), NIPADDR(rclassifier->daddr.ipv4));
              rclassifier->splen                 = 0;
              rclassifier->dplen                 = 0;
              rclassifier->protocol              = OAI_NW_DRV_PROTOCOL_DEFAULT;
              rclassifier->protocol_message_type = 0; //LG ??
              rclassifier->sport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
              rclassifier->dport                 = htons(OAI_NW_DRV_PORT_DEFAULT);
            }
        }*/
    }
    #ifdef OAI_DRV_DEBUG_CLASS
    printk("[OAI_IP_DRV][%s] end \n",__FUNCTION__ );
    #endif
    return rb;
}

//---------------------------------------------------------------------------
void oai_nw_drv_common_flush_rb(struct cx_entity *cx){
  //---------------------------------------------------------------------------
  struct rb_entity *rb;
  struct classifier_entity *gc;
  u8 dscp;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("[OAI_IP_DRV][%s] begin\n",__FUNCTION__);
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("[OAI_IP_DRV][%s] input parameter cx is NULL \n",__FUNCTION__);
#endif
    return;
  }
  // End debug information
  for (rb=cx->rb; rb!=NULL; rb=cx->rb){
    printk("[OAI_IP_DRV][%s] del rab_id %u\n",__FUNCTION__, rb->rab_id);
    cx->rb=rb->next;
    kfree(rb);
  }
  cx->num_rb=0;
  cx->rb=NULL;
  for(dscp=0; dscp<OAI_NW_DRV_DSCP_MAX; ++dscp)
    {
      for (gc=cx->sclassifier[dscp]; gc!=NULL; gc=gc->next)
        gc->rb=NULL;
    }
#ifdef OAI_DRV_DEBUG_CLASS
  printk("[OAI_IP_DRV][%s] end\n",__FUNCTION__);
#endif
}
