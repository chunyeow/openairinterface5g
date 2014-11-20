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
/*! \file sgi_socket.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>  //mmap

#include <asm/types.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_ether.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <poll.h>
#include <arpa/inet.h>
#ifndef SGI_UE_ETHER_TRAFFIC
#include <sys/uio.h>
#endif


#include <netinet/ip6.h>
#include <netinet/ip.h>
#include "sgi.h"
#include "intertask_interface.h"


//static char sgi_command_buffer[256];

#define HW_ADDRESS  "%02x:%02x:%02x:%02x:%02x:%02x"
#define HW_ADDRESS_FORMAT(iFR)    \
    (uint8_t)(iFR)[0],            \
    (uint8_t)(iFR)[1],            \
    (uint8_t)(iFR)[2],            \
    (uint8_t)(iFR)[3],            \
    (uint8_t)(iFR)[4],            \
    (uint8_t)(iFR)[5]

#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

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


/// The number of frames in the ring
//  This number is not set in stone. Nor are block_size, block_nr or frame_size
#define CONF_RING_FRAMES          128

/// Offset of data from start of frame
#define PKT_OFFSET      (TPACKET_ALIGN(sizeof(struct tpacket_hdr)) + \
                         TPACKET_ALIGN(sizeof(struct sockaddr_ll)))

int sgi_create_vlan_interface(char *interface_nameP, int vlan_idP) {
    char vlan_interface_name[IFNAMSIZ];
    char command_line[256];
    int  ret = 0;

    ret = sprintf(vlan_interface_name, "%s.%d", interface_nameP, vlan_idP);
    if (ret > 0) {
        ret = sprintf(command_line, "ifconfig %s down > /dev/null 2>&1", vlan_interface_name);
        if (ret > 0) ret = system(command_line); else return -1;

        ret = sprintf(command_line, "vconfig rem  %s > /dev/null 2>&1", vlan_interface_name);
        if (ret > 0) ret = system(command_line); else return -1;

        ret = sprintf(command_line, "vconfig add  %s %d", interface_nameP, vlan_idP);
        if (ret > 0) ret = system(command_line); else return -1;

        ret = sprintf(command_line, "ifconfig %s up", vlan_interface_name);
        if (ret > 0) ret = system(command_line); else return -1;

        ret = sprintf(command_line, "sync");
        if (ret > 0) ret = system(command_line); else return -1;

        //ret = sprintf(command_line, "ip -4 addr add  10.0.%d.2/24 dev %s", vlan_idP+200, vlan_interface_name);
        //if (ret > 0) ret = system(command_line); else return -1;
        return 0;
    } else {
        return -1;
    }
}


int sgi_create_sockets(sgi_data_t *sgi_data_p)
{
#ifdef SGI_SOCKET_BIND_TO_IF
    struct sockaddr_ll      socket_address;
    char   if_name[16];
#endif
#ifdef SGI_MARKING
    int                     value = 0;
    const int              *val_p=&value;
#endif
#ifdef SGI_PACKET_RX_RING
    struct tpacket_req      tp;
#endif
    uint32_t                i = 0;
#ifndef SGI_UE_ETHER_TRAFFIC
    FILE                   *fp = NULL;
    char                    filename[128];
    int                     rc = 0;
    char                   *router_mac_addr = NULL;
    int                     b = 0;
    char                    addr_mac_byte[16];
#endif

    if (sgi_data_p == NULL) {
        return -1;
    }

    SGI_IF_DEBUG("Creating sockets for sgi on interface %s\n", sgi_data_p->interface_name);

    // qci = 0 is for the default bearers QOS, Non-GBR
        /* Create a RAW IP socket and request for all internet IP traffic */
        // work
    for (i = 0; i < SGI_MAX_EPS_BEARERS_PER_USER; i++) {

        sgi_create_vlan_interface(sgi_data_p->interface_name,i+SGI_MIN_EPS_BEARER_ID);

        // works also
#if defined SGI_SOCKET_RAW
        sgi_data_p->sd[i] = socket(PF_PACKET, SOCK_RAW, htons(IPPROTO_RAW));
#else
#if defined SGI_SOCKET_DGRAM
        SGI_IF_DEBUG("Created socket PF_PACKET/SOCK_DGRAM/ETH_P_ALL\n",strerror(errno), errno);
        sgi_data_p->sd[i] = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
#endif
#endif
        //sgi_data_p->sd[i] = socket(AF_INET, SOCK_RAW, ETH_P_IP);
        //sgi_data_p->sd[i] = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));

        if (sgi_data_p->sd[i] < 0) {
            SGI_IF_ERROR("Error during socket creation (%s:%d)\n",strerror(errno), errno);
            goto error;
        }

      /* socket options, tell the kernel we provide the IP structure
      int one = 1;
      const int *val = &one;
      if(setsockopt(sgi_data_p->sd[i], IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
      {
            SGI_IF_ERROR("Error during socket setsockopt IP_HDRINCL (%s:%d)\n", strerror(errno), errno);
          goto error;
      }*/


#ifdef SGI_MARKING
        // setting socket option to use MARK value
        //value = rab_id + SGI_MIN_EPS_BEARER_ID;
        value = 5;
        if (setsockopt (sgi_data_p->sd[i], SOL_SOCKET, SO_MARK, val_p, sizeof (value)) < 0)
        {
            SGI_IF_ERROR("error notifying kernel about MARK");
            goto error;
        }
        SGI_IF_DEBUG("Created socket %d for rab_id %d (for any UE context)\n", sgi_data_p->sd[i], value);
#endif

#ifdef SGI_PACKET_RX_RING
        // tell kernel to export data through mmap()ped ring
        tp.tp_block_size = CONF_RING_FRAMES * getpagesize();
        tp.tp_block_nr   = 1;
        tp.tp_frame_size = getpagesize();
        tp.tp_frame_nr   = CONF_RING_FRAMES;

        if (setsockopt(sgi_data_p->sd[i], SOL_PACKET, PACKET_RX_RING, (void*) &tp, sizeof(tp))) {
            SGI_IF_ERROR("setsockopt() ring\n");
            goto error;
        }

        // open ring
        sgi_data_p->sock_mmap_ring = mmap(0, tp.tp_block_size * tp.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, sgi_data_p->sd[i], 0);
        if (!sgi_data_p->sock_mmap_ring) {
            SGI_IF_ERROR("Failed to mmap socket (%s:%d)\n", strerror(errno), errno);
            goto error;
        }
        /* Setup our ringbuffer */
        sgi_data_p->malloc_ring = malloc(tp.tp_frame_nr * sizeof(struct iovec));
        for(i=0; i<tp.tp_frame_nr; i++) {
        	sgi_data_p->malloc_ring[i].iov_base=(void *)((long)sgi_data_p->sock_mmap_ring)+(i*tp.tp_frame_size);
        	sgi_data_p->malloc_ring[i].iov_len=tp.tp_frame_size;
        }

#endif

#ifdef SGI_SOCKET_BIND_TO_IF
        sprintf(if_name, "%s.%d",sgi_data_p->interface_name,i+SGI_MIN_EPS_BEARER_ID);

        memset(&socket_address, 0, sizeof(struct sockaddr_ll));
        socket_address.sll_family        = AF_PACKET; //always PF_PACKET

        //socket_address.sll_addr = ;// Filled when we want to tx
        //socket_address.sll_halen = ;// Filled when we want to tx
        //socket_address.sll_family  ;// Filled when we want to tx

        //socket_address.sll_hatype = ;// Filled when packet received
        //socket_address.sll_pkttype = ;// Filled when packet received
        sgi_data_p->if_index[i] = if_nametoindex(if_name);
        socket_address.sll_ifindex       = sgi_data_p->if_index[i];
        //LG 2014-11-07socket_address.sll_protocol      = htons(ETH_P_IP);/* Protocol phy level */
        socket_address.sll_protocol      = htons(ETH_P_ALL);

        // Now we can bind the socket to send the IP traffic
        if (bind(sgi_data_p->sd[i], (struct sockaddr *)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
            SGI_IF_ERROR("Bind socket to %s (%s:%d)\n", if_name, strerror(errno), errno);
            goto error;
        } else {
            SGI_IF_DEBUG("Bind EPS bearer ID %d socket %d to %s\n",
                    i+SGI_MIN_EPS_BEARER_ID, sgi_data_p->sd[i], if_name);
        }
#endif

    }

#ifndef SGI_SOCKET_DGRAM
    if (sprintf(filename, "/sys/class/net/%s/address", sgi_data_p->interface_name) <= 0 ) {
        SGI_IF_ERROR("Build of file name /sys/class/net/sgi_if_name/address Failed (%s:%d)\n", strerror(errno), errno);
        goto error;
    }
    fp = fopen(filename, "r");
    if (fp) {
        rc = fscanf(fp, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &sgi_data_p->interface_hw_address[0],
            &sgi_data_p->interface_hw_address[1],
            &sgi_data_p->interface_hw_address[2],
            &sgi_data_p->interface_hw_address[3],
            &sgi_data_p->interface_hw_address[4],
            &sgi_data_p->interface_hw_address[5]);
        fclose(fp);
        if (rc <= 0) {
            SGI_IF_ERROR("ERROR reading %s\n" ,filename);
            goto error;
        }
    } else {
        SGI_IF_ERROR("Correct /sys/class/net/sgi_if_name file not found\n");
        goto error;
    }
    memcpy((void*)(&sgi_data_p->eh.ether_shost[0]), (void*)(&sgi_data_p->interface_hw_address[0]), ETH_ALEN);
    router_mac_addr = getenv("MAC_ROUTER");
    if (router_mac_addr == NULL) {
        SGI_IF_ERROR("ERROR getting ENV variable MAC_ROUTER, should be set by start script\n");
        goto error;
    }
    for (b = 0 ; b < 6; b++) {
        strncpy(addr_mac_byte, router_mac_addr+2*b, 2);
        addr_mac_byte[2] = '\0';
        // we can do (unsigned int*) because x86 processors are little-endian
        // (done for suppressing a warning)
        rc = sscanf ((const char*)addr_mac_byte, "%02x", (uint32_t*)&i);
        if (rc < 1) {
            SGI_IF_ERROR("Error in converting string MAC_ROUTER address %s in char array\n", router_mac_addr);
            goto error;
        }
        sgi_data_p->eh.ether_dhost[b] = (i & 0x000000FF);
    }
    SGI_IF_DEBUG("Found MAC address for ROUTER : %02x:%02x:%02x:%02x:%02x:%02x\n",
    		NMACADDR(sgi_data_p->eh.ether_dhost));

#ifdef VLAN8021Q
    sgi_data_p->eh.ether_vlan8021q.vlan_tpid = htons(0x8100);
    sgi_data_p->eh.ether_vlan8021q.vlan_tci  = htons(0x7000 | 0x0000 | 0x0000);
#endif
    SGI_IF_DEBUG("Found MAC address for SGI IF %s:  %02x:%02x:%02x:%02x:%02x:%02x\n",
    		sgi_data_p->interface_name, NMACADDR(sgi_data_p->eh.ether_shost));

#endif
    return 0;
error:
    SGI_IF_ERROR("ERROR (%s)\n", strerror(errno));
    for (i=0; i<SGI_MAX_EPS_BEARERS_PER_USER; i++) {
        if (sgi_data_p->sd[i] > 0) {
            close(sgi_data_p->sd[i]);
        }
        sgi_data_p->sd[i] = -1;
    }
    return -1;
}
#ifdef SGI_UE_ETHER_TRAFFIC
int sgi_send_data(uint8_t *buffer_pP, uint32_t length, sgi_data_t *sgi_data_pP, Teid_t originating_sgw_S1u_teidP)
{
    struct ether_header        *eh_p                     = (struct ether_header *) buffer_pP;
    struct iphdr               *iph_p                    = NULL;
    struct arphdr              *arph_p                   = NULL;
    struct ipv6hdr             *ip6h_p                   = NULL;
    sgi_teid_mapping_t         *mapping_p                = NULL;
    sgi_addr_mapping_t         *addr_mapping_p           = NULL;
    hashtable_rc_t                hash_rc;
    struct in6_addr             src6_addr;        /* source address */
    struct in6_addr            *src6_addr_p;      /* source address */
    u_int32_t                   src4_addr;

    if (buffer_pP == NULL) {
        SGI_IF_ERROR("sgi_send_data: received bad parameter\n");
        return -1;
    }

    // get IP version of this packet
    switch (htons(eh_p->ether_type)) {

    //*******************
    case ETHERTYPE_IP:
    //*******************
        iph_p      = (struct iphdr *)   (buffer_pP + sizeof(struct ether_header));
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        src4_addr = iph_p->saddr;
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, src4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {
                if (mapping_p->is_outgoing_ipv4_packet_seen == 0) {
                    mapping_p->hw_addrlen = ETH_ALEN;    // TO DO
                    memcpy(mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);
                    mapping_p->is_outgoing_ipv4_packet_seen = 1;
                    mapping_p->in_add_captured.s_addr = src4_addr;

                    addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                    memcpy(addr_mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);
                    addr_mapping_p->is_outgoing_packet_seen = 1;
                    addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                    addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;
                    hashtable_insert(sgi_data_pP->addr_v4_mapping, src4_addr, (void*)addr_mapping_p);
                    SGI_IF_DEBUG("%s ASSOCIATED %d.%d.%d.%d to MAC %02x:%02x:%02x:%02x:%02x:%02x teid %d\n",
                            __FUNCTION__, NIPADDR(src4_addr), NMACADDR(eh_p->ether_shost), originating_sgw_S1u_teidP);
                } else {
                    SGI_IF_ERROR("Error IPv4 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                    return -1;
                }
            }

        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;

    //*******************
    case ETHERTYPE_ARP:
    //*******************
        arph_p      = (struct arphdr *)   (buffer_pP + sizeof(struct ether_header));
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        memcpy(&src4_addr, &((unsigned char*)(&arph_p[1]))[ETH_ALEN], 4);
        SGI_IF_ERROR("%s ARP OPCODE %s TARGET IP %d.%d.%d.%d\n", __FUNCTION__, sgi_arpopcode_2_str(ntohl(arph_p->ar_op)), NIPADDR(src4_addr));
            if (hashtable_get(sgi_data_pP->addr_v4_mapping, src4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
                hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
                if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                    SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                    return -1;
                } else {
                    if (mapping_p->is_outgoing_ipv4_packet_seen == 0) {
                        mapping_p->hw_addrlen = ETH_ALEN;    // TO DO
                        memcpy(mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);
                        mapping_p->is_outgoing_ipv4_packet_seen = 1;
                        mapping_p->in_add_captured.s_addr = src4_addr;

                        addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                        memcpy(addr_mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);
                        addr_mapping_p->is_outgoing_packet_seen = 1;
                        addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                        addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;
                        hashtable_insert(sgi_data_pP->addr_v4_mapping, src4_addr, (void*)addr_mapping_p);
                        SGI_IF_DEBUG("%s ASSOCIATED %d.%d.%d.%d to MAC %02x:%02x:%02x:%02x:%02x:%02x teid %d\n",
                                __FUNCTION__, NIPADDR(src4_addr), NMACADDR(eh_p->ether_shost), originating_sgw_S1u_teidP);
                    } else {
                        SGI_IF_ERROR("Error IPv4 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                        return -1;
                    }
                }

            } else {
                hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            }
            break;


    //*******************
    case ETHERTYPE_IPV6:
        ip6h_p     = (struct ipv6hdr *) (buffer_pP + sizeof(struct ether_header));

        if (obj_hashtable_get(sgi_data_pP->addr_v6_mapping, &src6_addr, sizeof(struct in6_addr), (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {

                    if (mapping_p->is_outgoing_ipv6_packet_seen < MAX_DEFINED_IPV6_ADDRESSES_PER_UE) {
                        mapping_p->hw_addrlen = ETH_ALEN;
                        memcpy(mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);

                        memcpy(&mapping_p->in6_addr_captured[mapping_p->is_outgoing_ipv6_packet_seen].s6_addr, src6_addr.s6_addr, 16);
                        mapping_p->is_outgoing_ipv6_packet_seen +=1;

                        addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                        memcpy(addr_mapping_p->ue_mac_addr, eh_p->ether_shost, ETH_ALEN);
                        addr_mapping_p->is_outgoing_packet_seen = 1;
                        addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                        addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;

                        src6_addr_p = malloc(sizeof(struct in6_addr));
                        if (src6_addr_p == NULL) {
                            return -1;
                        }
                        memcpy(src6_addr_p->s6_addr, ip6h_p->saddr.s6_addr, 16);
                        obj_hashtable_insert(sgi_data_pP->addr_v6_mapping, src6_addr_p,sizeof(struct in6_addr), (void*)addr_mapping_p);
                    } else {
                        SGI_IF_ERROR("Error TOO MANY IPv6 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                        return -1;
                    }
            }

        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;

    default:
        SGI_IF_WARNING("%s UNHANDLED ETHERTYPE: %X\n", __FUNCTION__, htons(eh_p->ether_type));
        break;
    }

    assert(mapping_p != NULL);
    if (send(sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID], (void *)buffer_pP, length, 0) < 0) {
        SGI_IF_ERROR("Error during send to socket %d bearer id %d : (%s:%d)\n",
        		sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID],
        		mapping_p->eps_bearer_id,
        		strerror(errno),
        		errno);
        return -1;
    }
    return 0;
}
#else
int sgi_send_data(uint8_t *buffer_pP, uint32_t length, sgi_data_t *sgi_data_pP, Teid_t originating_sgw_S1u_teidP)
{
    struct iphdr               *iph_p                    = (struct iphdr *)   buffer_pP;
    struct ipv6hdr             *ip6h_p                   = (struct ipv6hdr *) buffer_pP;
    sgi_teid_mapping_t         *mapping_p                = NULL;
    sgi_addr_mapping_t         *addr_mapping_p           = NULL;
    hashtable_rc_t                hash_rc;
    struct in6_addr             src6_addr;        /* source address */
    struct in6_addr            *src6_addr_p;      /* source address */
    u_int32_t                   src4_addr;
    struct iovec                iov[2];

    if (buffer_pP == NULL) {
        SGI_IF_ERROR("sgi_send_data: received bad parameter\n");
        return -1;
    }

    // get IP version of this packet
    switch (iph_p->version) {

    //*******************
    case 4:
    //*******************
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        sgi_data_pP->eh.ether_type = htons(ETHERTYPE_IP);
        src4_addr = iph_p->saddr;
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, src4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {
                if (mapping_p->is_outgoing_ipv4_packet_seen == 0) {
                    mapping_p->hw_addrlen = ETH_ALEN;    // TO DO
                    mapping_p->is_outgoing_ipv4_packet_seen = 1;
                    mapping_p->in_add_captured.s_addr = src4_addr;

                    addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                    addr_mapping_p->is_outgoing_packet_seen = 1;
                    addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                    addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;
                    hashtable_insert(sgi_data_pP->addr_v4_mapping, src4_addr, (void*)addr_mapping_p);
                    SGI_IF_DEBUG("%s ASSOCIATED %d.%d.%d.%d to teid %d\n",
                            __FUNCTION__, NIPADDR(src4_addr), originating_sgw_S1u_teidP);
                } else {
                    SGI_IF_ERROR("Error IPv4 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                    return -1;
                }
            }

        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;



    //*******************
    case 6:
        sgi_data_pP->eh.ether_type = htons(ETHERTYPE_IPV6);
        if (obj_hashtable_get(sgi_data_pP->addr_v6_mapping, &src6_addr, sizeof(struct in6_addr), (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {

                    if (mapping_p->is_outgoing_ipv6_packet_seen < MAX_DEFINED_IPV6_ADDRESSES_PER_UE) {
                        mapping_p->hw_addrlen = 0;

                        memcpy(&mapping_p->in6_addr_captured[mapping_p->is_outgoing_ipv6_packet_seen].s6_addr, src6_addr.s6_addr, 16);
                        mapping_p->is_outgoing_ipv6_packet_seen +=1;

                        addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                        addr_mapping_p->is_outgoing_packet_seen = 1;
                        addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                        addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;

                        src6_addr_p = malloc(sizeof(struct in6_addr));
                        if (src6_addr_p == NULL) {
                            return -1;
                        }
                        memcpy(src6_addr_p->s6_addr, ip6h_p->saddr.s6_addr, 16);
                        obj_hashtable_insert(sgi_data_pP->addr_v6_mapping, src6_addr_p,sizeof(struct in6_addr), (void*)addr_mapping_p);
                    } else {
                        SGI_IF_ERROR("Error TOO MANY IPv6 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                        return -1;
                    }
            }

        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;

    default:
        SGI_IF_WARNING("%s UNHANDLED IP VERSION: %X\n", __FUNCTION__, iph_p->version);
        break;
    }

    assert(mapping_p != NULL);
    iov[0].iov_base   = &sgi_data_pP->eh;
    iov[0].iov_len    = sizeof(sgi_data_pP->eh);
    iov[1].iov_base   = (void *)buffer_pP;
    iov[1].iov_len    = length;
#ifdef VLAN8021Q
    sgi_data_pP->eh.ether_vlan8021q.vlan_tci  = htons(0x7000 | 0x0000 | mapping_p->eps_bearer_id);
#endif
    //sgi_print_hex_octets(iov[0].iov_base, iov[0].iov_len);
    //sgi_print_hex_octets(iov[1].iov_base, iov[1].iov_len);
    if (writev(sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID], (const struct iovec *)iov, 2) < 0) {
        SGI_IF_ERROR("Error during writev to socket %d bearer id %d : (%s:%d)\n",
        		sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID],
        		mapping_p->eps_bearer_id,
        		strerror(errno),
        		errno);
        return -1;
    }

/*    if (send(sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID], (void *)buffer_pP, length, 0) < 0) {
        SGI_IF_ERROR("Error during send to socket %d bearer id %d : (%s:%d)\n",
        		sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID],
        		mapping_p->eps_bearer_id,
        		strerror(errno),
        		errno);
        return -1;
    }
*/
    return 0;
}

#endif
int sgi_dgram_send_data(uint8_t *buffer_pP, uint32_t length, sgi_data_t *sgi_data_pP, Teid_t originating_sgw_S1u_teidP)
{
    struct iphdr               *iph_p                    = (struct iphdr *)   buffer_pP;
    struct ipv6hdr             *ip6h_p                   = (struct ipv6hdr *) buffer_pP;
    sgi_teid_mapping_t         *mapping_p                = NULL;
    sgi_addr_mapping_t         *addr_mapping_p           = NULL;
    hashtable_rc_t                hash_rc;
    struct in6_addr             src6_addr;        /* source address */
    struct in6_addr            *src6_addr_p;      /* source address */
    u_int32_t                   src4_addr;

    struct sockaddr_ll device;

    if (buffer_pP == NULL) {
        SGI_IF_ERROR("sgi_send_data: received bad parameter\n");
        return -1;
    }

    // get IP version of this packet
    switch (iph_p->version) {

    //*******************
    case 4:
    //*******************
        // The entry should be here but all signalling tied with RRC procedures not finished so
        // a data packet can arrive before the MODIFY_BEARER REQUEST
        sgi_data_pP->eh.ether_type = htons(ETHERTYPE_IP);
        src4_addr = iph_p->saddr;
        if (hashtable_get(sgi_data_pP->addr_v4_mapping, src4_addr, (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {
                if (mapping_p->is_outgoing_ipv4_packet_seen == 0) {
                    mapping_p->hw_addrlen = 0;    // TO DO
                    mapping_p->is_outgoing_ipv4_packet_seen = 1;
                    mapping_p->in_add_captured.s_addr = src4_addr;

                    addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                    addr_mapping_p->is_outgoing_packet_seen = 1;
                    addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                    addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;
                    hashtable_insert(sgi_data_pP->addr_v4_mapping, src4_addr, (void*)addr_mapping_p);
                    SGI_IF_DEBUG("%s ASSOCIATED %d.%d.%d.%d to teid %d\n",
                            __FUNCTION__, NIPADDR(src4_addr), originating_sgw_S1u_teidP);
                } else {
                    SGI_IF_ERROR("Error IPv4 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                    return -1;
                }
            }
        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;



    //*******************
    case 6:
        sgi_data_pP->eh.ether_type = htons(ETHERTYPE_IPV6);
        if (obj_hashtable_get(sgi_data_pP->addr_v6_mapping, &src6_addr, sizeof(struct in6_addr), (void**)&addr_mapping_p) != HASH_TABLE_OK) {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
            if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                SGI_IF_ERROR("%s Error unknown context SGW teid %d\n", __FUNCTION__, originating_sgw_S1u_teidP);
                return -1;
            } else {

                    if (mapping_p->is_outgoing_ipv6_packet_seen < MAX_DEFINED_IPV6_ADDRESSES_PER_UE) {
                        mapping_p->hw_addrlen = 0;

                        memcpy(&mapping_p->in6_addr_captured[mapping_p->is_outgoing_ipv6_packet_seen].s6_addr, src6_addr.s6_addr, 16);
                        mapping_p->is_outgoing_ipv6_packet_seen +=1;

                        addr_mapping_p = calloc(1, sizeof(sgi_addr_mapping_t));
                        addr_mapping_p->is_outgoing_packet_seen = 1;
                        addr_mapping_p->enb_S1U_teid            = mapping_p->enb_S1U_teid;
                        addr_mapping_p->sgw_S1U_teid            = originating_sgw_S1u_teidP;

                        src6_addr_p = malloc(sizeof(struct in6_addr));
                        if (src6_addr_p == NULL) {
                            return -1;
                        }
                        memcpy(src6_addr_p->s6_addr, ip6h_p->saddr.s6_addr, 16);
                        obj_hashtable_insert(sgi_data_pP->addr_v6_mapping, src6_addr_p,sizeof(struct in6_addr), (void*)addr_mapping_p);
                    } else {
                        SGI_IF_ERROR("Error TOO MANY IPv6 address already registered for teid %d\n", originating_sgw_S1u_teidP);
                        return -1;
                    }
            }

        } else {
            hash_rc = hashtable_get(sgi_data_pP->teid_mapping, originating_sgw_S1u_teidP, (void**)&mapping_p);
        }
        break;

    default:
        SGI_IF_WARNING("%s UNHANDLED IP VERSION: %X\n", __FUNCTION__, iph_p->version);
        break;
    }
    memset(&device, 0, sizeof(device));
    device.sll_family   = AF_PACKET;
    device.sll_protocol = htons(ETH_P_ALL);
    device.sll_ifindex = sgi_data_pP->if_index[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID];


    if (sendto(sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID],
            buffer_pP,
            length,
            0,
            (struct sockaddr *) &device,
            sizeof (device)) < 0) {
        SGI_IF_ERROR("%d Error during send to socket %d bearer id %d : (%s:%d)\n",
                __LINE__,
                sgi_data_pP->sd[mapping_p->eps_bearer_id - SGI_MIN_EPS_BEARER_ID],
                mapping_p->eps_bearer_id,
                strerror(errno),
                errno);
        return -1;
    }


    return 0;
}

#ifdef SGI_TEST
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SGI_TEST_BUFF_LEN 500
unsigned char sgi_test_tx_buffer[SGI_TEST_BUFF_LEN];


void sgi_test_send_ping(sgi_data_t *sgi_dataP, uint32_t markP, uint64_t src_mac_addrP, uint64_t dst_mac_addrP, char* src_ip_addrP, char* dst_ip_addrP)
{
#ifdef SGI_PF_PACKET
    struct ether_header *eh   = (struct ether_header *) sgi_test_tx_buffer;
    struct iphdr        *iph  = (struct iphdr *)       (sgi_test_tx_buffer + sizeof(struct ether_header));
    struct icmphdr      *icmp = (struct icmphdr *)     (sgi_test_tx_buffer + sizeof(struct ether_header) + sizeof(struct iphdr));
#else
    struct iphdr        *iph  = (struct iphdr *)    sgi_test_tx_buffer;
    struct icmphdr      *icmp = (struct icmphdr *) (sgi_test_tx_buffer + sizeof(struct iphdr));
#endif
    struct hostent      *hp, *hp2;
    struct in_addr       ip_dst;
    struct in_addr       ip_src;



#ifdef SGI_PF_PACKET
    eh->ether_shost[0] = (u_int8_t)((src_mac_addrP & 0x0000FF0000000000) >> 40);
    eh->ether_shost[1] = (u_int8_t)((src_mac_addrP & 0x000000FF00000000) >> 32);
    eh->ether_shost[2] = (u_int8_t)((src_mac_addrP & 0x00000000FF000000) >> 24);
    eh->ether_shost[3] = (u_int8_t)((src_mac_addrP & 0x0000000000FF0000) >> 16);
    eh->ether_shost[4] = (u_int8_t)((src_mac_addrP & 0x000000000000FF00) >> 8);
    eh->ether_shost[5] = (u_int8_t) (src_mac_addrP & 0x00000000000000FF);

    eh->ether_dhost[0] = (u_int8_t)((dst_mac_addrP & 0x0000FF0000000000) >> 40);
    eh->ether_dhost[1] = (u_int8_t)((dst_mac_addrP & 0x000000FF00000000) >> 32);
    eh->ether_dhost[2] = (u_int8_t)((dst_mac_addrP & 0x00000000FF000000) >> 24);
    eh->ether_dhost[3] = (u_int8_t)((dst_mac_addrP & 0x0000000000FF0000) >> 16);
    eh->ether_dhost[4] = (u_int8_t)((dst_mac_addrP & 0x000000000000FF00) >> 8);
    eh->ether_dhost[5] = (u_int8_t) (dst_mac_addrP & 0x00000000000000FF);
    eh->ether_type     = htons(ETH_P_IP);
#endif
    if ((hp = gethostbyname(dst_ip_addrP)) == NULL)
    {
        SGI_IF_DEBUG("gethostbyname() is OK.\n");
        if ((ip_dst.s_addr = inet_addr(dst_ip_addrP)) == -1)
        {
            fprintf(stderr, "%s: Can't resolve, unknown host.\n", dst_ip_addrP);
            exit(1);
        }
    }
    else
        bcopy(hp->h_addr_list[0], &ip_dst.s_addr, hp->h_length);
    /* The following source address just redundant for target to collect */
    if ((hp2 = gethostbyname(src_ip_addrP)) == NULL)
    {
        SGI_IF_DEBUG("gethostbyname() is OK.\n");
        if ((ip_src.s_addr = inet_addr(src_ip_addrP)) == -1)
        {
            fprintf(stderr, "%s: Can't resolve, unknown host\n", src_ip_addrP);
            exit(1);
        }
    }
    else
        bcopy(hp2->h_addr_list[0], &ip_src.s_addr, hp->h_length);
    printf("Sending to %08X from spoofed %08X\n", ip_dst.s_addr, ip_src.s_addr);
    /* Ip structure, check the ip.h */
    iph->version     = 4;
    iph->ihl         = sizeof *iph >> 2;
    iph->tos         = 0;
    iph->tot_len     = htons(SGI_TEST_BUFF_LEN-sizeof(struct ether_header));
    iph->id          = htons(4321);
    iph->frag_off    = htons(0);
    iph->ttl         = 255;
    iph->protocol    = IPPROTO_ICMP;
    iph->check       = htons(0);
    iph->saddr       = ip_src.s_addr;
    iph->daddr       = ip_dst.s_addr;

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id       = 1000;
    icmp->un.echo.sequence = 10;
    /* Header checksum */
    icmp->checksum = 0;

    icmp-> checksum       = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));
    iph->check            = in_cksum((unsigned short *)iph, sizeof(struct iphdr));
    /* sending time */
    if (send(sgi_dataP->sd[0], sgi_test_tx_buffer, SGI_TEST_BUFF_LEN, 0) < 0)
    {
        SGI_IF_ERROR("sendto() error (%s:%d)\n", strerror(errno), errno);
    }
    else
        SGI_IF_DEBUG("sendto() is OK.\n");
}

#endif

#ifdef ENABLE_USE_RAW_SOCKET_FOR_SGI

void sgi_sock_raw_cleanup_handler(void *args_p)
{
    sgi_data_t                       *sgi_data_p;
    int                               socket_index;

    sgi_data_p   = ((sgi_read_thread_args_t*)args_p)->sgi_data;
    socket_index = ((sgi_read_thread_args_t*)args_p)->socket_index;

    SGI_IF_DEBUG("Called %s\n", __FUNCTION__);
#ifdef SGI_PACKET_RX_RING
    free(sgi_data_p->malloc_ring);
    sgi_data_p->malloc_ring = NULL;
    if (munmap(sgi_data_p->sock_mmap_ring, CONF_RING_FRAMES * getpagesize())) {
        SGI_IF_ERROR("munmap\n");
    } else {
        sgi_data_p->sock_mmap_ring = NULL;
    }
#endif
    if (sgi_data_p->sd[socket_index] > 0) {
        close(sgi_data_p->sd[socket_index]);
        SGI_IF_DEBUG("Closed soket %d\n", sgi_data_p->sd[socket_index]);
    }
    sgi_data_p->sd[socket_index] = -1;

    free(args_p);
}

void* sgi_sock_raw_fw_2_gtpv1u_thread(void* args_p)
{
    sgi_data_t                       *sgi_data_p;
    int                               socket_index;
#ifdef SGI_PACKET_RX_RING
    struct pollfd                     pfd;
    int                               i;
    pthread_mutex_t                   mutex = PTHREAD_MUTEX_INITIALIZER;
    char *names[]={
    	"<", /* incoming */
    	"B", /* broadcast */
    	"M", /* multicast */
    	"P", /* promisc */
    	">", /* outgoing */
    };
#else
    int                               num_bytes;
#endif
    sgi_data_p   = ((sgi_read_thread_args_t*)args_p)->sgi_data;
    socket_index = ((sgi_read_thread_args_t*)args_p)->socket_index;

    SGI_IF_DEBUG("RX thread on raw socket started, operating on Device %s.%d\n", sgi_data_p->interface_name, socket_index+SGI_MIN_EPS_BEARER_ID);


    pthread_cleanup_push(sgi_sock_raw_cleanup_handler, args_p);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_mutex_lock (&sgi_data_p->thread_started_mutex);
    sgi_data_p->thread_started += 1;
    pthread_mutex_unlock (&sgi_data_p->thread_started_mutex);

#ifdef SGI_PACKET_RX_RING
    i = 0;
    while (1) {
        while(*(unsigned long*)sgi_data_p->malloc_ring[i].iov_base) {
            struct tpacket_hdr *h=sgi_data_p->malloc_ring[i].iov_base;
            struct sockaddr_ll *sll=(void *)h + TPACKET_ALIGN(sizeof(*h));
            unsigned char *bp=(unsigned char *)h + h->tp_mac;

            SGI_IF_DEBUG("%u.%.6u: if%u %s %u bytes\n",
                h->tp_sec, h->tp_usec,
                sll->sll_ifindex,
                names[sll->sll_pkttype],
                h->tp_len);

            sgi_process_raw_packet(sgi_data_p, bp, h->tp_len);
            pthread_mutex_lock(&mutex); /* memory barrier */
            /* tell the kernel this packet is done with */
            h->tp_status=0;
            pthread_mutex_unlock(&mutex); /* memory barrier */

            i=(i==CONF_RING_FRAMES-1) ? 0 : i+1;
        }

        /* Sleep when nothings happen */
        pfd.fd=sgi_data_p->sd;
        pfd.events=POLLIN|POLLERR;
        pfd.revents=0;
        poll(&pfd, 1, -1);
    }
#else
    while (1) {
        num_bytes = recvfrom(sgi_data_p->sd[socket_index], &sgi_data_p->recv_buffer[0][socket_index], SGI_BUFFER_RECV_LEN, 0, NULL, NULL);
        if (num_bytes > 0) {
            SGI_IF_DEBUG("recvfrom bearer id %d %d bytes\n", socket_index + SGI_MIN_EPS_BEARER_ID, num_bytes);
#ifdef SGI_SOCKET_RAW
            sgi_process_raw_packet(sgi_data_p, &sgi_data_p->recv_buffer[0][socket_index], num_bytes);
#else
#ifdef SGI_SOCKET_DGRAM
            sgi_process_dgram_packet(sgi_data_p, &sgi_data_p->recv_buffer[0][socket_index], num_bytes);
#endif
#endif
        } else {
            SGI_IF_DEBUG("recvfrom bearer id %d %d (%s:%d)\n", socket_index + SGI_MIN_EPS_BEARER_ID, num_bytes, strerror(errno), errno);
        }
    }
#endif
    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}
#endif

