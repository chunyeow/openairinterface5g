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

#include "local.h"
#include "proto_extern.h"

//#include <linux/in.h>
//#include <net/ndisc.h>
//#include <linux/icmpv6.h>
//#include <linux/icmp.h>
//#include <linux/udp.h>
//#include <linux/tcp.h>

//#define  KERNEL_VERSION_GREATER_THAN_2622 1
//#define  KERNEL_VERSION_GREATER_THAN_2630 1

//#define OAI_NW_DRV_DEBUG_TOOL 1

//---------------------------------------------------------------------------
//
void oai_nw_drv_TOOL_fct(struct classifier_entity *classifier, u8 fct){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
  printk("OAI_NW_DRV_TOOL_FCT - begin \n");
#endif
  if (classifier==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_FCT - input parameter classifier is NULL \n");
#endif
      return;
  }
// End debug information
  switch(fct){
      case OAI_NW_DRV_FCT_QOS_SEND:
          classifier->fct=oai_nw_drv_common_ip2wireless;
          break;
      case OAI_NW_DRV_FCT_CTL_SEND:
          classifier->fct=oai_nw_drv_CTL_send;
          break;
      case OAI_NW_DRV_FCT_DEL_SEND:
          classifier->fct=oai_nw_drv_common_ip2wireless_drop;
          break;
      default:
          classifier->fct=oai_nw_drv_common_ip2wireless_drop;
  }
}

//---------------------------------------------------------------------------
u8 oai_nw_drv_TOOL_invfct(struct classifier_entity *classifier){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_INVFCT - begin \n");
#endif
  if (classifier==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_INVFCT - input parameter classifier is NULL \n");
#endif
    return 0;
  }
// End debug information
    if (classifier->fct==oai_nw_drv_common_ip2wireless)
        return OAI_NW_DRV_FCT_QOS_SEND;
    if (classifier->fct==oai_nw_drv_CTL_send)
        return OAI_NW_DRV_FCT_CTL_SEND;
    if (classifier->fct==oai_nw_drv_common_ip2wireless_drop)
        return OAI_NW_DRV_FCT_DEL_SEND;
    return 0;
}

//---------------------------------------------------------------------------
u8 oai_nw_drv_TOOL_get_dscp6(struct ipv6hdr *iph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_GET_DSCP6 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_DSCP6 - input parameter iph is NULL \n");
#endif
    return 0;
  }
// End debug information
  return (ntohl(((*(__u32 *)iph)&OAI_NW_DRV_TRAFFICCLASS_MASK)))>>22;
  //return ntohs(*(const __be16 *)iph) >> 4; // see linux/dsfield.h

}

//---------------------------------------------------------------------------
u8 oai_nw_drv_TOOL_get_dscp4(struct iphdr *iph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_GET_DSCP4 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_DSCP4 - input parameter iph is NULL \n");
#endif
    return 0;
  }
// End debug information
  return (iph->tos);

}

//---------------------------------------------------------------------------
int oai_nw_drv_TOOL_network6(struct in6_addr *addr, struct in6_addr *prefix, u8 plen){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_NETWORK6 - begin \n");
#endif
  if (addr==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_NETWORK6 - input parameter addr is NULL \n");
#endif
    return 0;
  }
  if (prefix==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_NETWORK6 - input parameter prefix is NULL \n");
#endif
    return 0;
  }
// End debug information
    switch(plen/32)
    {
    case 0:
        return (((addr->s6_addr32[0]>>(32-plen))<<(32-plen))==prefix->s6_addr[0]);
    case 1:
        return ((addr->s6_addr32[0]==prefix->s6_addr[0])&&
            (((addr->s6_addr32[1]>>(64-plen))<<(64-plen))==prefix->s6_addr[1]));
    case 2:
        return ((addr->s6_addr32[0]==prefix->s6_addr[0])&&
            (addr->s6_addr32[1]==prefix->s6_addr[1])&&
            (((addr->s6_addr32[2]>>(96-plen))<<(96-plen))==prefix->s6_addr[2]));
    case 3:
        return ((addr->s6_addr32[0]==prefix->s6_addr[0])&&
            (addr->s6_addr32[1]==prefix->s6_addr[1])&&
            (addr->s6_addr32[2]==prefix->s6_addr[2])&&
            (((addr->s6_addr32[3]>>(128-plen))<<(128-plen))==prefix->s6_addr[3]));
    default:
        return ((addr->s6_addr32[0]==prefix->s6_addr[0])&&
            (addr->s6_addr32[1]==prefix->s6_addr[1])&&
            (addr->s6_addr32[2]==prefix->s6_addr[2])&&
            (addr->s6_addr32[3]==prefix->s6_addr[3]));
    }
}

//---------------------------------------------------------------------------
int oai_nw_drv_TOOL_network4(u32 *addr, u32 *prefix, u8 plen){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_NETWORK4 - begin \n");
#endif
  if (addr==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_NETWORK4 - input parameter addr is NULL \n");
#endif
    return 0;
  }
  if (prefix==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_NETWORK4 - input parameter prefix is NULL \n");
#endif
    return 0;
  }
// End debug information
    if (plen>=32)
        return (*addr==*prefix);
    else
        return (((*addr>>(32-plen))<<(32-plen))==*prefix);
}

//---------------------------------------------------------------------------
//struct udphdr *oai_nw_drv_TOOL_get_udp6(struct ipv6hdr *iph){
//---------------------------------------------------------------------------
//  return (struct udphdr *)((char *)iph+OAI_NW_DRV_IPV6_SIZE); // to modify
//}

//---------------------------------------------------------------------------
u8 *oai_nw_drv_TOOL_get_protocol6(struct ipv6hdr *iph, u8 *protocol){
//---------------------------------------------------------------------------
    u16 size;
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_GET_PROTOCOL6 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_PROTOCOL6 - input parameter iph is NULL \n");
#endif
    return NULL;
  }
  if (protocol==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_PROTOCOL6 - input parameter protocol is NULL \n");
#endif
    return NULL;
  }
// End debug information
    *protocol=iph->nexthdr;
    size=OAI_NW_DRV_IPV6_SIZE;
    while (1)
    {
        switch(*protocol)
        {
        case IPPROTO_UDP:
        case IPPROTO_TCP:
        case IPPROTO_ICMPV6:
            return (u8 *)((u8 *)iph+size);
        case IPPROTO_HOPOPTS:
        case IPPROTO_ROUTING:
        case IPPROTO_DSTOPTS:
            *protocol=((u8 *)iph+size)[0];
            size+=((u8 *)iph+size)[1]*8+8;
            break;
        case IPPROTO_FRAGMENT:
            *protocol=((u8 *)iph+size)[0];
            size+=((u8 *)iph+size)[1]+8;
            break;
        case IPPROTO_NONE:
        case IPPROTO_AH:
        case IPPROTO_ESP:
        default:
            return NULL;
        }
    }
}

//---------------------------------------------------------------------------
u8 *oai_nw_drv_TOOL_get_protocol4(struct iphdr *iph, u8 *protocol){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_GET_PROTOCOL4 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_PROTOCOL4 - input parameter iph is NULL \n");
#endif
    return NULL;
  }
  if (protocol==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_PROTOCOL4 - input parameter protocol is NULL \n");
#endif
    return NULL;
  }
// End debug information
    *protocol=iph->protocol;
    switch(*protocol)
    {
    case IPPROTO_UDP:
    case IPPROTO_TCP:
    case IPPROTO_ICMP:
        return (u8 *)((u8 *)iph+iph->tot_len);
    default:
        return NULL;
    }
}

//---------------------------------------------------------------------------
// Convert the IMEI to iid
void oai_nw_drv_TOOL_imei2iid(u8 *imei, u8 *iid){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
        printk("OAI_NW_DRV_TOOL_IMEI2IID - begin \n");
#endif
  if (imei==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
          printk("OAI_NW_DRV_TOOL_IMEI2IID - input parameter imei is NULL \n");
#endif
    return;
  }
  if (iid==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
          printk("OAI_NW_DRV_TOOL_IMEI2IID - input parameter iid is NULL \n");
#endif
    return;
  }
// End debug information
        memset(iid, 0, OAI_NW_DRV_ADDR_LEN);
        iid[0] = 0x03;
        iid[1] = 16*imei[0]+imei[1];
        iid[2] = 16*imei[2]+imei[3];
        iid[3] = 16*imei[4]+imei[5];
        iid[4] = 16*imei[6]+imei[7];
        iid[5] = 16*imei[8]+imei[9];
        iid[6] = 16*imei[10]+imei[11];
        iid[7] = 16*imei[12]+imei[13];
}
//---------------------------------------------------------------------------
// Convert the IMEI to iid
void oai_nw_drv_TOOL_eNB_imei2iid(unsigned char *imei, unsigned char *iid, unsigned char len){
//---------------------------------------------------------------------------
  unsigned int index;
  // Start debug information
  #ifdef OAI_NW_DRV_DEBUG_TOOL
  printk("OAI_NW_DRV_TOOL_eNB_IMEI2IID - begin \n");
  #endif
  if (imei==NULL){
      #ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_eNB_IMEI2IID - input parameter imei is NULL \n");
      #endif
      return;
  }
  if (iid==NULL){
      #ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_eNB_IMEI2IID - input parameter iid is NULL \n");
      #endif
      return;
  }
  // End debug information
  memset(iid, 0, len);
  iid[0] = 0x00;  // to be compatible between link local and global
  // len -1 because of insertion of 0 above
  for (index = 0; index < (len-1); index++) {
      iid[index+1] = 16*imei[index*2]+imei[index*2+1];
  }
}

//struct udphdr *oai_nw_drv_TOOL_get_udp4(struct iphdr *iph)
//{
//  return (struct udphdr *)((char *)iph+OAI_NW_DRV_IPV4_SIZE); // to modify
//}


//---------------------------------------------------------------------------
char *oai_nw_drv_TOOL_get_udpmsg(struct udphdr *udph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_GET_UDPMSG - begin \n");
#endif
  if (udph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_GET_UDPMSG - input parameter udph is NULL \n");
#endif
    return NULL;
  }
  // End debug information
  return ((char *)udph+sizeof(struct udphdr));
}

//---------------------------------------------------------------------------
// Compute the UDP checksum (the data size must be odd)
u16 oai_nw_drv_TOOL_udpcksum(struct in6_addr *saddr, struct in6_addr *daddr, u8 proto, u32 udplen, void *data){
//---------------------------------------------------------------------------
    u32 i;
  u16 *data16;
    u32 csum=0;

// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_UDPCKSUM - begin \n");
#endif
  if (saddr==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_UDPCKSUM - input parameter saddr is NULL \n");
#endif
    return 0;
  }
  if (daddr==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_UDPCKSUM - input parameter daddr is NULL \n");
#endif
    return 0;
  }
  if (data==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_UDPCKSUM - input parameter data is NULL \n");
#endif
    return 0;
  }
// End debug information
    data16=data;
    for (i=0; i<8; ++i)
    {
        csum+=ntohs(saddr->s6_addr16[i]);
        if (csum>0xffff)
            csum-=0xffff;
    }
    for (i=0; i<8; ++i)
    {
        csum+=ntohs(daddr->s6_addr16[i]);
        if (csum>0xffff)
            csum-=0xffff;
    }
    csum+=(udplen>>16); // udplen checksum
    if (csum>0xffff)
        csum -= 0xffff;
    csum+=udplen & 0xffff;
    if (csum>0xffff)
        csum -= 0xffff;
    csum+=proto; // protocol checksum
    if (csum>0xffff)
        csum-=0xffff;
    for (i = 0; 2*i < udplen; i++)
    {
        csum+=ntohs(data16[i]);
        if (csum>0xffff)
            csum-=0xffff;
    }
    return htons((u16)(~csum)&0xffff);
}

//---------------------------------------------------------------------------
void print_TOOL_pk_udp(struct udphdr *udph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("PRINT_TOOL_PK_UDP - begin \n");
#endif
  if (udph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PK_UDP - input parameter udph is NULL \n");
#endif
    return;
  }
// End debug information
    if (udph!=NULL)
    {
        printk("UDP:\t source = %u, dest = %u, len = %u, check = %x\n", ntohs(udph->source), ntohs(udph->dest), ntohs(udph->len), udph->check);
    }
}

//---------------------------------------------------------------------------
void print_TOOL_pk_tcp(struct tcphdr *tcph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("PRINT_TOOL_PK_TDP - begin \n");
#endif
  if (tcph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PK_TDP - input parameter tcph is NULL \n");
#endif
    return;
  }
// End debug information
    if (tcph!=NULL)
    {
        printk("TCP:\t source = %u, dest = %u\n", tcph->source, tcph->dest);
    }
}

//---------------------------------------------------------------------------
void print_TOOL_pk_icmp6(struct icmp6hdr *icmph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("PRINT_TOOL_PK_ICMP6 - begin \n");
#endif
  if (icmph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PK_ICMP6 - input parameter icmph is NULL \n");
#endif
    return;
  }
// End debug information
    if (icmph!=NULL)
    {
        printk("ICMPv6:\t type= %d, code = %d\n", icmph->icmp6_type, icmph->icmp6_code);
        switch(icmph->icmp6_type)
        {
        case ICMPV6_DEST_UNREACH:printk("Destination unreachable\n");break;
        case ICMPV6_PKT_TOOBIG:printk("Packet too big\n");break;
        case ICMPV6_TIME_EXCEED:printk("Time exceeded\n");break;
        case ICMPV6_PARAMPROB:printk("Parameter problem\n");break;
        case ICMPV6_ECHO_REQUEST:printk("Echo request\n");break;
        case ICMPV6_ECHO_REPLY:printk("Echo reply\n");break;
        case ICMPV6_MGM_QUERY:printk("Multicast listener query\n");break;
        case ICMPV6_MGM_REPORT:printk("Multicast listener report\n");break;
        case ICMPV6_MGM_REDUCTION:printk("Multicast listener done\n");break;
        case NDISC_ROUTER_SOLICITATION:printk("Router solicitation\n");break;
        case NDISC_ROUTER_ADVERTISEMENT:printk("Router advertisment\n");break;
        case NDISC_NEIGHBOUR_SOLICITATION:printk("Neighbour solicitation\n");break;
        case NDISC_NEIGHBOUR_ADVERTISEMENT:printk("Neighbour advertisment\n");break;
        case NDISC_REDIRECT:printk("redirect message\n");break;
        }
    }
}

//---------------------------------------------------------------------------
void print_TOOL_pk_ipv6(struct ipv6hdr *iph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("PRINT_TOOL_PK_IPv6 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PK_IPv6 - input parameter iph is NULL \n");
#endif
    return;
  }
// End debug information

    if (iph!=NULL)
    {
//      char addr[OAI_NW_DRV_INET6_ADDRSTRLEN];
        printk("IP:\t version = %u, priority = %u, payload_len = %u\n", iph->version, iph->priority, ntohs(iph->payload_len));
        printk("\t fl0 = %u, fl1 = %u, fl2 = %u\n",iph->flow_lbl[0],iph->flow_lbl[1],iph->flow_lbl[2]);
        printk("\t next header = %u, hop_limit = %u\n", iph->nexthdr, iph->hop_limit);
//      inet_ntop(AF_INET6, (void *)(&iph->saddr), addr, OAI_NW_DRV_INET6_ADDRSTRLEN);
//      printk("\t saddr = %s",addr);
//      inet_ntop(AF_INET6, (void *)(&iph->daddr), addr, OAI_NW_DRV_INET6_ADDRSTRLEN);
//      printk(", daddr = %s\n",addr);
        switch(iph->nexthdr)
        {
        case IPPROTO_UDP:
            print_TOOL_pk_udp((struct udphdr *)((char *)iph+sizeof(struct ipv6hdr)));
            break;
        case IPPROTO_TCP:
            print_TOOL_pk_tcp((struct tcphdr *)((char *)iph+sizeof(struct ipv6hdr)));
            break;
        case IPPROTO_ICMPV6:
            print_TOOL_pk_icmp6((struct icmp6hdr *)((char *)iph+sizeof(struct ipv6hdr)));
            break;
        case IPPROTO_IPV6:
            print_TOOL_pk_ipv6((struct ipv6hdr *)((char *)iph+sizeof(struct ipv6hdr)));
            break;
        default:
            printk("Unknown upper layer\n");
        }
    }
}

//---------------------------------------------------------------------------
void print_TOOL_pk_ipv4(struct iphdr *iph){
//---------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("PRINT_TOOL_PK_IPv4 - begin \n");
#endif
  if (iph==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PK_IPv4 - input parameter iph is NULL \n");
#endif
    return;
  }
// End debug information

    if (iph!=NULL)
    {
//      char addr[OAI_NW_DRV_INET_ADDRSTRLEN];
        printk("IP:\t version = %u, IP length = %u\n", iph->version, iph->ihl);
//      inet_ntop(AF_INET, (void *)(&iph->saddr), addr, OAI_NW_DRV_INET_ADDRSTRLEN);
//      printk("\t saddr = %s", addr);
//      inet_ntop(AF_INET, (void *)(&iph->saddr), addr, OAI_NW_DRV_INET_ADDRSTRLEN);
//      printk("\t saddr = %s", addr);
    }
}

//---------------------------------------------------------------------------
void print_TOOL_pk_all(struct sk_buff *skb){
//---------------------------------------------------------------------------
    printk("Skb:\t %u, len = %u\n", (unsigned int)skb, skb->len);
#ifdef KERNEL_VERSION_GREATER_THAN_2629
    //navid: need to calculate the current used space: fixme?
    printk("Skb:\t available buf space = %u \n", skb->truesize);
#else
    printk("Skb:\t available buf space = %u, cur used space = %u \n", skb->end-skb->head, skb->tail-skb->data);
#endif
    switch (ntohs(skb->protocol))
    {
    case ETH_P_IPV6:
#ifdef KERNEL_VERSION_GREATER_THAN_2622
      print_TOOL_pk_ipv6((struct ipv6hdr *)skb->network_header);
#else
      print_TOOL_pk_ipv6(skb->nh.ipv6h);
#endif
      break;
    case ETH_P_IP:
#ifdef KERNEL_VERSION_GREATER_THAN_2622
      print_TOOL_pk_ipv4((struct iphdr *)skb->network_header);
#else
      print_TOOL_pk_ipv4(skb->nh.iph);
#endif
      break;
    }
}

//---------------------------------------------------------------------------
/*int isdigit(char c)
{
    switch(c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return 1;
    default:
        return 0;
    }
}*/

/*int oai_nw_drv_TOOL_inet_pton4(char *src, u32 *dst)
{
    u32 val;
    int n;
    u8 c;
    u32 parts[4];

    c = *src;
    val=0;
    n=0
    for (;;)
    {
        for (;;)
        {
            if (isdigit(c))
            {
                val = (val * 10) + c - '0';
                c = *++src;
            }
            else
                break;
        }
        if (c == '.')
        {
            if (n>4)
                return -1;
            parts[n]=val;
            c = *++src;
            ++n;
        }
        else
            break;
    }
    if ((c != '\0')||(n!=3))
        return (0);
    if ((parts[0] | parts[1] | parts[2] | val) > 256)
        return (0);
    val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
    if (dst)
        dst = htonl(val);
    return (1);
}*/

//---------------------------------------------------------------------------
void print_TOOL_state(u8 state){
//---------------------------------------------------------------------------
//  case OAI_NW_DRV_STATE_IDLE:printk(" State OAI_NW_DRV_STATE_IDLE\n");return;
//  case OAI_NW_DRV_STATE_CONNECTED:printk(" State OAI_NW_DRV_STATE_CONNECTED\n");return;
//  case OAI_NW_DRV_STATE_ESTABLISHMENT_REQUEST:printk(" State OAI_NW_DRV_STATE_ESTABLISHMENT_REQUEST\n");return;
//  case OAI_NW_DRV_STATE_ESTABLISHMENT_FAILURE:printk(" State OAI_NW_DRV_STATE_ESTABLISHMENT_FAILURE\n");return;
//  case OAI_NW_DRV_STATE_RELEASE_FAILURE:printk(" State OAI_NW_DRV_STATE_RELEASE_FAILURE\n");return;

    switch(state){
    case  OAI_NW_DRV_IDLE:printk("OAI_NW_DRV_IDLE\n");return;
    case  OAI_NW_DRV_CX_FACH:printk("OAI_NW_DRV_CX_FACH\n");return;
    case  OAI_NW_DRV_CX_DCH:printk("OAI_NW_DRV_CX_DCH\n");return;
    case  OAI_NW_DRV_CX_RECEIVED:printk("OAI_NW_DRV_CX_RECEIVED\n");return;
    case  OAI_NW_DRV_CX_CONNECTING:printk("OAI_NW_DRV_CX_CONNECTING\n");return;
    case  OAI_NW_DRV_CX_RELEASING:printk("OAI_NW_DRV_CX_RELEASING\n");return;
    case  OAI_NW_DRV_CX_CONNECTING_FAILURE:printk("OAI_NW_DRV_CX_CONNECTING_FAILURE\n");return;
    case  OAI_NW_DRV_CX_RELEASING_FAILURE:printk("OAI_NW_DRV_CX_RELEASING_FAILURE\n");return;
    case  OAI_NW_DRV_RB_ESTABLISHING:printk("OAI_NW_DRV_RB_ESTABLISHING\n");return;
    case  OAI_NW_DRV_RB_RELEASING:printk("OAI_NW_DRV_RB_RELEASING\n");return;
    case  OAI_NW_DRV_RB_DCH:printk("OAI_NW_DRV_RB_DCH\n");return;

    default: printk(" Unknown state\n");
    }
}

//-----------------------------------------------------------------------------
// Print the content of a buffer in hexadecimal
void oai_nw_drv_tool_print_buffer(char * buffer,int length) {
//-----------------------------------------------------------------------------
   int i;

// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_TOOL_PRINT_BUFFER - begin \n");
#endif
  if (buffer==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_TOOL_PRINT_BUFFER - input parameter buffer is NULL \n");
#endif
    return;
  }
// End debug information
   printk("\nBuffer content: ");
     for (i=0;i<length;i++)
         printk("-%hx-",buffer[i]);
     printk(",\t length %d\n", length);
}
//-----------------------------------------------------------------------------
void oai_nw_drv_print_rb_entity(struct rb_entity *rb){
//-----------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_PRINT_RB_ENTITY - begin \n");
#endif
  if (rb==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_PRINT_RB_ENTITY - input parameter rb is NULL \n");
#endif
    return;
  }
// End debug information
   printk("\nrb_entity content: rab_id %d, sapi %d, qos %d, \n", rb->rab_id, rb->sapi, rb->qos);
   printk("state %d, retry %d, countimer %d\n",rb->state, rb->retry, rb->countimer);
};

//-----------------------------------------------------------------------------
void oai_nw_drv_print_classifier(struct classifier_entity *classifier){
//-----------------------------------------------------------------------------
// Start debug information
#ifdef OAI_NW_DRV_DEBUG_TOOL
    printk("OAI_NW_DRV_PRINT_GC_ENTITY - begin \n");
#endif
  if (classifier==NULL){
#ifdef OAI_NW_DRV_DEBUG_TOOL
      printk("OAI_NW_DRV_PRINT_GC_ENTITY - input parameter classifier is NULL \n");
#endif
    return;
  }
// End debug information
   printk("\nClassifier content: classref %d, version %d, splen %d, dplen %d,\n", classifier->classref, classifier->ip_version, classifier->splen, classifier->dplen);
   printk("protocol %d, sport %d, dport %d, rab_id %d\n", classifier->protocol, classifier->sport, classifier->dport, classifier->rab_id);
   if (classifier->rb != NULL){
    oai_nw_drv_print_rb_entity(classifier->rb);
   }
};

