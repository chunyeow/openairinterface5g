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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
/*! \file sgi_egress.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


#include "sgi.h"
#include "intertask_interface.h"
#include "assertions.h"

#include <netinet/ip6.h>
#include <netinet/ip.h>
#include <net/if_arp.h>


struct ipv6hdr {
    __u8            priority:4,
                version:4;
    __u8            flow_lbl[3];

    __be16          payload_len;
    __u8            nexthdr;
    __u8            hop_limit;

    struct  in6_addr    saddr;
    struct  in6_addr    daddr;
};

#ifdef ENABLE_USE_RAW_SOCKET_FOR_SGI

#ifdef SGI_UE_ETHER_TRAFFIC
void sgi_process_raw_packet(sgi_data_t *sgi_data_pP, unsigned char* data_pP, int packet_sizeP)
{
    Gtpv1uTunnelDataReq        *gtpv1u_tunnel_data_req_p = NULL;
    MessageDef                 *message_p                = NULL;
    unsigned char              *message_payload_p        = NULL ;
    struct ether_header        *eh_p                     = (struct ether_header *) data_pP;
    struct arphdr              *arph_p                   = NULL;
    struct iphdr               *iph_p                    = NULL;
    struct ipv6hdr             *ip6h_p                   = NULL;
    //struct arphdr       *rarph    = NULL;
    //unsigned char       *rarp_ptr = NULL;
    //unsigned char       *sha      = NULL;  /* Sender hardware address.  */
    //unsigned char       *tha      = NULL;  /* Target hardware address.  */
    sgi_addr_mapping_t          *addr_mapping_p          = NULL;
    //__be32               sip, tip;         /* Sender, target IP address.  */
    struct in6_addr      dest6_addr;
    u_int32_t            dest4_addr;
    struct in_addr       in_dest_addr;


    switch (htons(eh_p->ether_type)) {

    //*******************
    case ETHERTYPE_IP:
    //*******************
        iph_p      = (struct iphdr *)   (data_pP + sizeof(struct ether_header));
        dest4_addr = iph_p->daddr;
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, dest4_addr, (void**)&addr_mapping_p) == HASH_TABLE_OK) {
            memcpy(eh_p->ether_dhost, addr_mapping_p->ue_mac_addr, ETH_ALEN);
            SGI_IF_DEBUG("--------------------------------------------------------------\n%s :\n", __FUNCTION__);
            sgi_print_hex_octets(data_pP, packet_sizeP);

        } else {
            if (sgi_data_pP->ipv4_addr == dest4_addr) {
                //SGI_IF_DEBUG("%s Dropping incoming egress IPV4 packet not UE IP flow\n", __FUNCTION__);
                return;
            } else {
                in_dest_addr.s_addr = dest4_addr;
                //SGI_IF_WARNING("%s Dropping incoming egress IPV4 packet, IPV4 dest %s not found \n", __FUNCTION__, inet_ntoa(in_dest_addr));
                return;
            }
        }
        break;


    //*******************
    case ETHERTYPE_IPV6:
    //*******************
        ip6h_p     = (struct ipv6hdr *) (data_pP + sizeof(struct ether_header));
        memcpy(dest6_addr.__in6_u.__u6_addr8, ip6h_p->daddr.__in6_u.__u6_addr8, 16);
        if (obj_hashtable_get(sgi_data_pP->addr_v6_mapping, (void*)&dest6_addr, sizeof(struct in6_addr), (void**)&addr_mapping_p) == HASH_TABLE_OK) {
            memcpy(eh_p->ether_dhost, addr_mapping_p->ue_mac_addr, ETH_ALEN);
            SGI_IF_DEBUG("--------------------------------------------------------------\n%s :\n", __FUNCTION__);
            sgi_print_hex_octets(data_pP, packet_sizeP);
        } else {
            //SGI_IF_WARNING("%s Dropping incoming egress IPV6 packet, IPV6 dest %X:%X:%X:%X:%X:%X:%X:%X not found \n", __FUNCTION__, NIP6ADDR(&dest6_addr));
            return;
        }
        break;


    //*******************
    case ETHERTYPE_REVARP:
    //*******************
        SGI_IF_ERROR("%s UNHANDLED ETHERTYPE_REVARP of incoming egress packet\n", __FUNCTION__);
        return;
        break;


    //*******************
    case ETHERTYPE_ARP:
    //*******************
        SGI_IF_DEBUG("%s HANDLED ETHERTYPE_ARP of incoming egress packet\n", __FUNCTION__);
        arph_p      = (struct arphdr *)   (data_pP + sizeof(struct ether_header));
#ifdef SGI_UE_ETHER_TRAFFIC
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        // get "    unsigned char __ar_tip[4];      /* Target IP address.  */" (from /usr/include/net/if_apr.h line 68)
        memcpy(&dest4_addr, &((unsigned char*)(&arph_p[1]))[ETH_ALEN*2+4], 4);
        SGI_IF_DEBUG("%s ARP OPCODE %s TARGET IP %d.%d.%d.%d\n", __FUNCTION__, sgi_arpopcode_2_str(ntohl(arph_p->ar_op)), NIPADDR(dest4_addr));
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, dest4_addr, (void**)&addr_mapping_p) == HASH_TABLE_OK) {
            memcpy(eh_p->ether_dhost, addr_mapping_p->ue_mac_addr, ETH_ALEN);
        } else {
            if (sgi_data_pP->ipv4_addr == dest4_addr) {
                //SGI_IF_DEBUG("%s Dropping incoming egress IPV4 packet not UE IP flow\n", __FUNCTION__);
                return;
            } else {
                in_dest_addr.s_addr = dest4_addr;
                //SGI_IF_WARNING("%s Dropping incoming egress IPV4 packet, IPV4 dest %s not found \n", __FUNCTION__, inet_ntoa(in_dest_addr));
                return;
            }
        }
#else
#endif
        break;
//      rarph = (struct arphdr *)(data_pP + sizeof(struct ether_header));
//
//        // If it's not Ethernet, delete it.
//        if (rarph->ar_pro != htons(ETH_P_IP)) {
//          SGI_IF_ERROR("%s ARP PACKET PROTOCOL OG INCOMING PACKET IS NOT ETHERNET\n",__FUNCTION__);
//            break;
//        }
//        rarp_ptr = (unsigned char *) (rarph + 1);
//        sha = rarp_ptr;
//        rarp_ptr += ETH_ALEN;
//        memcpy(&sip, rarp_ptr, 4);
//        rarp_ptr += 4;
//        tha = rarp_ptr;
//        rarp_ptr += ETH_ALEN;
//        memcpy(&tip, rarp_ptr, 4);
//      SGI_IF_DEBUG("%s ARP DEST IP transport IP = %d.%d.%d.%d\n",__FUNCTION__, NIPADDR(tip));
//        // TO DO IF NECESSARY
//      break;
//

    default:
        SGI_IF_ERROR("%s UNHANDLED ether type %d of incoming egress packet\n", __FUNCTION__, eh_p->ether_type);
        return;
    }

    message_p               = itti_alloc_new_message(TASK_FW_IP, GTPV1U_TUNNEL_DATA_REQ);
    if (message_p == NULL) {
        SGI_IF_ERROR("%s OUT OF MEMORY DROP EGRESS PACKET\n", __FUNCTION__);
        return;
    }
    message_payload_p = itti_malloc(TASK_FW_IP, TASK_GTPV1_U, packet_sizeP);
    if (message_payload_p == NULL) {
        SGI_IF_ERROR("%s OUT OF MEMORY DROP EGRESS PACKET\n", __FUNCTION__);
        return;
    }
    memcpy(message_payload_p, data_pP, packet_sizeP);

    gtpv1u_tunnel_data_req_p = &message_p->ittiMsg.gtpv1uTunnelDataReq;
    //LG HACK gtpv1u_tunnel_data_req_p->S1u_enb_teid   = addr_mapping_p->enb_S1U_teid;
#warning forced S1u_enb_teid to 1 for testing, waiting for MODIFY_BEARER REQUEST
    gtpv1u_tunnel_data_req_p->S1u_enb_teid   = 1;
    gtpv1u_tunnel_data_req_p->local_S1u_teid = addr_mapping_p->sgw_S1U_teid;
    gtpv1u_tunnel_data_req_p->length       = packet_sizeP;
    gtpv1u_tunnel_data_req_p->buffer       = message_payload_p;
    SGI_IF_DEBUG("%s ETHER send GTPV1U_TUNNEL_DATA_REQ to GTPV1U S1u_enb_teid %u local_S1u_teid %u size %u\n", __FUNCTION__, gtpv1u_tunnel_data_req_p->S1u_enb_teid, gtpv1u_tunnel_data_req_p->local_S1u_teid, packet_sizeP);

    itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);

}
#else
void sgi_process_raw_packet(sgi_data_t *sgi_data_pP, unsigned char* data_pP, int packet_sizeP)
{
    Gtpv1uTunnelDataReq        *gtpv1u_tunnel_data_req_p = NULL;
    MessageDef                 *message_p                = NULL;
    unsigned char              *message_payload_p        = NULL ;
    struct ether_header        *eh_p                     = (struct ether_header *) data_pP;
    struct arphdr              *arph_p                   = NULL;
    struct iphdr               *iph_p                    = NULL;
    struct ipv6hdr             *ip6h_p                   = NULL;
    sgi_addr_mapping_t          *addr_mapping_p          = NULL;
    struct in6_addr      dest6_addr;
    u_int32_t            dest4_addr;
    u_int32_t            src4_addr;
    struct in_addr       in_dest_addr;


    switch (htons(eh_p->ether_type)) {
    sgi_print_hex_octets(data_pP, packet_sizeP);

    //*******************
    case ETHERTYPE_IP:
    //*******************
        iph_p      = (struct iphdr *)   (data_pP + sizeof(struct ether_header));
        dest4_addr = iph_p->daddr;
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, dest4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            if (sgi_data_pP->ipv4_addr == dest4_addr) {
                //SGI_IF_DEBUG("%s Dropping incoming egress IPV4 packet not UE IP flow\n", __FUNCTION__);
                return;
            } else {
                in_dest_addr.s_addr = dest4_addr;
                //SGI_IF_WARNING("%s Dropping incoming egress IPV4 packet, IPV4 dest %s not found \n", __FUNCTION__, inet_ntoa(in_dest_addr));
                return;
            }
        }
        break;


    //*******************
    case ETHERTYPE_IPV6:
    //*******************
        ip6h_p     = (struct ipv6hdr *) (data_pP + sizeof(struct ether_header));
        memcpy(dest6_addr.__in6_u.__u6_addr8, ip6h_p->daddr.__in6_u.__u6_addr8, 16);
        if (obj_hashtable_get(sgi_data_pP->addr_v6_mapping, (void*)&dest6_addr, sizeof(struct in6_addr), (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            SGI_IF_WARNING("%s Dropping incoming egress IPV6 packet, IPV6 dest %X:%X:%X:%X:%X:%X:%X:%X not found \n", __FUNCTION__, NIP6ADDR(&dest6_addr));
            return;
        }
        break;


    //*******************
    case ETHERTYPE_REVARP:
    //*******************
        SGI_IF_ERROR("%s UNHANDLED ETHERTYPE_REVARP of incoming egress packet\n", __FUNCTION__);
        return;
        break;


    //*******************
    case ETHERTYPE_ARP:
    //*******************
        SGI_IF_DEBUG("%s HANDLED ETHERTYPE_ARP of incoming egress packet\n", __FUNCTION__);
        arph_p      = (struct arphdr *)   (data_pP + sizeof(struct ether_header));
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        // get "    unsigned char __ar_tip[4];      /* Target IP address.  */" (from /usr/include/net/if_apr.h line 68)
        memcpy(&dest4_addr, &((unsigned char*)(&arph_p[1]))[ETH_ALEN*2+4], 4);
        SGI_IF_DEBUG("%s ARP OPCODE %s TARGET IP %d.%d.%d.%d\n", __FUNCTION__, sgi_arpopcode_2_str(ntohl(arph_p->ar_op)), NIPADDR(dest4_addr));
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, dest4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            if (sgi_data_pP->ipv4_addr == dest4_addr) {
                SGI_IF_DEBUG("%s Dropping incoming egress IPV4 packet not UE IP flow\n", __FUNCTION__);
                return;
            } else {
                if (sgi_data_pP->hw_address_of_router_captured) {
                    in_dest_addr.s_addr = dest4_addr;
                    SGI_IF_WARNING("%s Dropping incoming egress IPV4 packet, IPV4 dest %s not found \n", __FUNCTION__, inet_ntoa(in_dest_addr));
                } else {
                    // may be a response to our ARP request to router
                    if (htons(arph_p->ar_op) == ARPOP_REPLY) {
                        memcpy(&src4_addr, &((unsigned char*)(&arph_p[1]))[ETH_ALEN], 4);
                        SGI_IF_DEBUG("%s FOR ROUTER HW ADDRESS COMPARING: %d - %d   %d - %d\n",
                            __FUNCTION__,
                            sgi_data_pP->local_addr_v4_4_hw_address_router_capture,
                            dest4_addr,
                            sgi_data_pP->ipv4_addr_of_router,
                            src4_addr);
                        if ((sgi_data_pP->local_addr_v4_4_hw_address_router_capture == dest4_addr) && (sgi_data_pP->ipv4_addr_of_router == src4_addr)) {
                            sgi_data_pP->hw_address_of_router_captured = 1;
                            sgi_data_pP->eh.ether_dhost[0] = ((unsigned char*)(&arph_p[1]))[0];
                            sgi_data_pP->eh.ether_dhost[1] = ((unsigned char*)(&arph_p[1]))[1];
                            sgi_data_pP->eh.ether_dhost[2] = ((unsigned char*)(&arph_p[1]))[2];
                            sgi_data_pP->eh.ether_dhost[3] = ((unsigned char*)(&arph_p[1]))[3];
                            sgi_data_pP->eh.ether_dhost[4] = ((unsigned char*)(&arph_p[1]))[4];
                            sgi_data_pP->eh.ether_dhost[5] = ((unsigned char*)(&arph_p[1]))[5];
                            SGI_IF_DEBUG("%s FOUND ROUTER HW ADDRESS: %02x:%02x:%02x:%02x:%02x:%02x\n", __FUNCTION__, NMACADDR(sgi_data_pP->eh.ether_dhost));
                        }
                    }
                }
                return;
            }
        }
        break;

    default:
        //SGI_IF_ERROR("%s UNHANDLED ether type %d of incoming egress packet\n", __FUNCTION__, eh_p->ether_type);
        return;
    }

    message_p               = itti_alloc_new_message(TASK_FW_IP, GTPV1U_TUNNEL_DATA_REQ);
    if (message_p == NULL) {
        SGI_IF_ERROR("%s OUT OF MEMORY DROP EGRESS PACKET\n", __FUNCTION__);
        return;
    }
    AssertFatal((packet_sizeP - sizeof(sgi_data_pP->eh)) > 20, "BAD IP PACKET SIZE");
    message_payload_p = itti_malloc(TASK_FW_IP, TASK_GTPV1_U, packet_sizeP - sizeof(sgi_data_pP->eh));
    if (message_payload_p == NULL) {
        SGI_IF_ERROR("%s OUT OF MEMORY DROP EGRESS PACKET\n", __FUNCTION__);
        return;
    }
    memcpy(message_payload_p, data_pP+sizeof(sgi_data_pP->eh), packet_sizeP - sizeof(sgi_data_pP->eh));

    gtpv1u_tunnel_data_req_p = &message_p->ittiMsg.gtpv1uTunnelDataReq;
    gtpv1u_tunnel_data_req_p->S1u_enb_teid   = addr_mapping_p->enb_S1U_teid;
//#warning forced S1u_enb_teid to 1 for testing, waiting for MODIFY_BEARER REQUEST
//    gtpv1u_tunnel_data_req_p->S1u_enb_teid   = 1;
    gtpv1u_tunnel_data_req_p->local_S1u_teid = addr_mapping_p->sgw_S1U_teid;
    gtpv1u_tunnel_data_req_p->length       = packet_sizeP - sizeof(sgi_data_pP->eh);
    gtpv1u_tunnel_data_req_p->buffer       = message_payload_p;
    SGI_IF_DEBUG("%s send GTPV1U_TUNNEL_DATA_REQ to GTPV1U S1u_enb_teid %u local_S1u_teid %u size %u\n",
            __FUNCTION__,
            gtpv1u_tunnel_data_req_p->S1u_enb_teid,
            gtpv1u_tunnel_data_req_p->local_S1u_teid,
            gtpv1u_tunnel_data_req_p->length);

    itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);

}
#endif
#endif
