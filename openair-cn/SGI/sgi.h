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
/*! \file sgi.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#ifndef SGI_H_
#define SGI_H_

#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
 #include <stdio.h>

#ifdef ENABLE_USE_PCAP_FOR_SGI
#include <pcap.h>
#endif

//#define SGI_TEST
// Packets include a ethernet header if SGI_PF_PACKET is defined
#define SGI_PF_PACKET

#include "common_types.h"
#include "hashtable.h"
#include "obj_hashtable.h"
#include "sgw_lite_ie_defs.h"
#include "ip_forward_messages_types.h"

//# define SGI_IF_DEBUG(x, args...)
//# define SGI_IF_ERROR(x, args...)
//# define SGI_IF_WARNING(x, args...)
//-----------------------------------------------------------------------------
#ifndef SGI_IF_DEBUG
# define SGI_IF_DEBUG(x, args...) do { fprintf(stdout, "[SGI_IF][D]"x, ##args); } \
    while(0)
#endif
#ifndef SGI_IF_ERROR
# define SGI_IF_ERROR(x, args...) do { fprintf(stderr, "[SGI_IF][E]"x, ##args); } \
    while(0)
#endif
#ifndef SGI_IF_WARNING
 # define SGI_IF_WARNING(x, args...) do { fprintf(stderr, "[SGI_IF][W]"x, ##args); } \
     while(0)
#endif

// refer to 3GPP TS 23.203 V10.6.0 (2012-03) page 33
//#define MAX_DEFINED_QCI 0
#define MAX_DEFINED_QCI                    9
#define SGI_MIN_EPS_BEARER_ID              5
#define SGI_MAX_SERVED_USERS_PER_PGW      1024
#define SGI_MAX_EPS_BEARERS_PER_USER      11
#define MAX_DEFINED_IPV6_ADDRESSES_PER_UE 16


#define NIP6ADDR(addr) \
        ntohs((addr)->s6_addr16[0]), \
        ntohs((addr)->s6_addr16[1]), \
        ntohs((addr)->s6_addr16[2]), \
        ntohs((addr)->s6_addr16[3]), \
        ntohs((addr)->s6_addr16[4]), \
        ntohs((addr)->s6_addr16[5]), \
        ntohs((addr)->s6_addr16[6]), \
        ntohs((addr)->s6_addr16[7])

#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)

#define NMACADDR(addr) \
        (uint8_t)(addr[0]), \
        (uint8_t)(addr[1]), \
        (uint8_t)(addr[2]), \
        (uint8_t)(addr[3]), \
        (uint8_t)(addr[4]), \
        (uint8_t)(addr[5])
#ifdef VLAN8021Q
typedef struct vlan_tag_s {
    uint16_t       vlan_tpid;              /* ETH_P_8021Q */
    uint16_t       vlan_tci;               /* VLAN TCI */
} vlan_tag_t __attribute__ ((__packed__));

typedef struct ether_header_8021q_s
{
  u_int8_t  ether_dhost[ETH_ALEN];	/* destination eth addr	*/
  u_int8_t  ether_shost[ETH_ALEN];	/* source ether addr	*/
  vlan_tag_t ether_vlan8021q;
  u_int16_t ether_type;		        /* packet type ID field	*/
} ether_header_8021q_t __attribute__ ((__packed__));
#endif

typedef struct arphdr_s {
	__be16		ar_hrd;		/* format of hardware address	*/
	__be16		ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	__be16		ar_op;		/* ARP opcode (command)		*/
	 /*
	  *	 Ethernet looks like this : This bit is variable sized however...
	  */
	unsigned char		ar_sha[ETH_ALEN];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		/* sender IP address		*/
	unsigned char		ar_tha[ETH_ALEN];	/* target hardware address	*/
	unsigned char		ar_tip[4];		/* target IP address		*/

}arphdr_t;

typedef struct sgi_teid_mapping_s {
	int                        is_outgoing_ipv4_packet_seen;
	int                        is_outgoing_ipv6_packet_seen;
	Teid_t                     enb_S1U_teid;        ///< dest tunnel identifier
	qci_t                      qci;
	ebi_t                      eps_bearer_id;       ///< EPS bearer identifier
    char                       hw_addrlen;
#ifdef SGI_UE_ETHER_TRAFFIC
    char                       ue_mac_addr[8];
#endif
    struct in6_addr            in6_addr_captured[MAX_DEFINED_IPV6_ADDRESSES_PER_UE];
    struct in_addr             in_add_captured;
} sgi_teid_mapping_t;


// Main goal is to do an association IP address <-> MAC address
// may be usefull for netfilter that does not capture MAC address
// if not set on certain hook points (table,path).
typedef struct sgi_addr_mapping_s {
	int                        is_outgoing_packet_seen;
	ebi_t                      eps_bearer_id;       ///< EPS default bearer identifier, (really ?)
	Teid_t                     enb_S1U_teid;        ///< dest tunnel identifier, for default bearer
	Teid_t                     sgw_S1U_teid;        ///< dest tunnel identifier, for default bearer
#ifdef SGI_UE_ETHER_TRAFFIC
    char                       ue_mac_addr[ETH_ALEN];
#endif
} sgi_addr_mapping_t;




typedef struct sgi_data_s {
    int                 sd[SGI_MAX_EPS_BEARERS_PER_USER];
    int                 sd6;
    char               *interface_name;
    int                 interface_name_len;
    int                 interface_index;
#ifndef SGI_UE_ETHER_TRAFFIC
    char                interface_hw_address[ETH_ALEN];
#ifdef VLAN8021Q
    struct ether_header_8021q_s eh;
#else
    struct ether_header eh;
#endif
#endif
    uint32_t            ipv4_addr;
    hash_table_t       *teid_mapping;
    hash_table_t       *addr_v4_mapping;
    obj_hash_table_t   *addr_v6_mapping;
    pthread_t           capture_on_sgi_if_thread;
    pthread_mutex_t     thread_started_mutex;
    int                 thread_started;

    void               *sock_mmap_ring;
    struct iovec       *malloc_ring;

    int                 hw_address_of_router_captured;
    uint32_t            local_addr_v4_4_hw_address_router_capture;
    uint32_t            ipv4_addr_of_router;


#ifdef ENABLE_USE_PCAP_FOR_SGI
    //brief PCAP descriptor for capturing packets on SGI interface.
    pcap_t * pcap_descr;
#endif
#define SGI_BUFFER_RECV_LEN 3000
    unsigned char recv_buffer[sizeof(struct ether_header) + SGI_BUFFER_RECV_LEN][SGI_MAX_EPS_BEARERS_PER_USER];

} sgi_data_t;


typedef struct sgi_read_thread_args_s {
    sgi_data_t     *sgi_data;
    int            socket_index;
}sgi_read_thread_args_t;
//-----------------------------------------------------------------------------

#include "mme_config.h"


#define SGI_MAX_EPS_BEARERS (SGI_MAX_SERVED_USERS_PER_PGW*SGI_MAX_EPS_BEARERS_PER_USER)



//-----------------------------------------------------------------------------
// sgi_task.c
//-----------------------------------------------------------------------------
#include "spgw_config.h"
int sgi_init(const pgw_config_t *pgw_config_p);
char* sgi_status_2_str(SGIStatus_t statusP);

//-----------------------------------------------------------------------------
// sgi_nf.c
//-----------------------------------------------------------------------------
#ifdef ENABLE_USE_NETFILTER_FOR_SGI
void* sgi_nf_fw_2_gtpv1u_thread(void *args_p);
#endif

//-----------------------------------------------------------------------------
// sgi_pcap.c
//-----------------------------------------------------------------------------
#ifdef ENABLE_USE_PCAP_FOR_SGI
#    define SGI_PCAPMAXBYTES2CAPTURE 65535
#    define SGI_PCAPTIMEDELAYKERNEL2USERSPACE 1000

/*! \fn void* sgi_pcap_loop(void*)
* \brief
* \param[in]  devname The name of the device (ex "eth1") that will be listened for capturing packets.
* \param[in]  iif     The interface identifier that will be listened for capturing packets.
*/
void* sgi_pcap_fw_2_gtpv1u_thread(void*);
#endif
//-----------------------------------------------------------------------------
// sgi_util.c
//-----------------------------------------------------------------------------
void  sgi_print_hex_octets(unsigned char* dataP, unsigned long sizeP);
char* sgi_status_2_str    (SGIStatus_t statusP);
char* sgi_arpopcode_2_str (unsigned short int opcodeP);
unsigned short in_cksum(unsigned short *addr, int len);
void  sgi_send_arp_request(sgi_data_t *sgi_dataP, char* dst_ip_addrP);

//-----------------------------------------------------------------------------
// sgi_socket.c
//-----------------------------------------------------------------------------
int sgi_create_sockets(sgi_data_t *sgi_data_p);
int sgi_send_data(uint8_t *buffer, uint32_t length, sgi_data_t *sgi_dataP, Teid_t originating_sgw_S1u_teidP);
#ifdef SGI_TEST
unsigned short in_cksum(unsigned short *addr, int len);
void sgi_test_send_ping(sgi_data_t *sgi_dataP, uint32_t markP, uint64_t src_mac_addrP, uint64_t dst_mac_addrP, char* src_ip_addrP, char* dst_ip_addrP);
#endif
#ifdef ENABLE_USE_RAW_SOCKET_FOR_SGI
void  sgi_sock_raw_cleanup_handler(void *args_p);
void* sgi_sock_raw_fw_2_gtpv1u_thread(void* args_p);
#endif

//-----------------------------------------------------------------------------
// sgi_egress.c
//-----------------------------------------------------------------------------
void sgi_forward_ip_packet(sgi_data_t *sgi_dataP, struct ether_header *ehP, int packet_sizeP, sgi_addr_mapping_t *addr_mappingP);
#ifdef ENABLE_USE_RAW_SOCKET_FOR_SGI
void sgi_process_raw_packet(sgi_data_t *sgi_dataP, unsigned char* dataP, int packet_sizeP);
#endif
#endif /* SGI_H_ */
