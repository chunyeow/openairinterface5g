/*
 * This file is part of the PMIP, Proxy Mobile IPv6 for Linux.
 *
 * Authors: OPENAIR3 <openair_tech@eurecom.fr>
 *
 * Copyright 2010-2011 EURECOM (Sophia-Antipolis, FRANCE)
 * 
 * Proxy Mobile IPv6 (or PMIPv6, or PMIP) is a network-based mobility 
 * management protocol standardized by IETF. It is a protocol for building 
 * a common and access technology independent of mobile core networks, 
 * accommodating various access technologies such as WiMAX, 3GPP, 3GPP2 
 * and WLAN based access architectures. Proxy Mobile IPv6 is the only 
 * network-based mobility management protocol standardized by IETF.
 * 
 * PMIP Proxy Mobile IPv6 for Linux has been built above MIPL free software;
 * which it involves that it is under the same terms of GNU General Public
 * License version 2. See MIPL terms condition if you need more details. 
 */
/*! \file pmip_types.h
* \brief Describe all types for pmip
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

/** @defgroup PACKETS TYPES
 * @ingroup PMIP6D
 *  PMIP Types
 *  @{
 */

#ifndef __PMIP_TYPES_H__
#    define __PMIP_TYPES_H__
//---------------------------------------------------------------------------------------------------------------------
#    include <linux/types.h>
#    include <netinet/ip6mh.h>
//---------------------------------------------------------------------------------------------------------------------
#    include "mh.h"
//---------------------------------------------------------------------------------------------------------------------

/*
* Mobility Option TLV data structure
*New options defined for Proxy BU & BA
*/
/*! \struct  ip6_mh_opt_home_net_prefix_t
* \brief Mobility Option TLV data structure: Home Network Prefix Option.
*/
struct ip6_mh_opt_home_net_prefix_t {   /*Home netowork prefix option */
    __u8            ip6hnp_type;        /*!< \brief Type is  "Home Network Prefix Option" value.*/
    __u8            ip6hnp_len;         /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 18.*/
    __u8            ip6hnp_reserved;    /*!< \brief This 8-bit field is unused for now.  The value MUST be initialized to 0 by the sender and MUST be ignored by the receiver.*/
    __u8            ip6hnp_prefix_len;  /*!< \brief 8-bit unsigned integer indicating the prefix length of the IPv6 prefix contained in the option.*/
    struct in6_addr ip6hnp_prefix;      /*!< \brief A sixteen-byte field containing the mobile node's IPv6 Home Network Prefix.*/
} __attribute__ ((__packed__));

typedef struct ip6_mh_opt_home_net_prefix_t ip6_mh_opt_home_net_prefix_t;


/*! \struct  ip6mnid_t
* \brief Contain a mobile node identifier.
*/
typedef struct {
    __u32       first;
    __u32       second;
} ip6mnid_t;


/*! \struct  ip6ts_t
* \brief Timestamp structure.
*/
typedef struct {
    __u32       first;
    __u32       second;
} ip6ts_t;


/*! \struct  ip6_mh_opt_mobile_node_identifier_t
* \brief Mobility Option TLV data structure: Mobile Node Identifier Option (RFC4283).
*/
struct ip6_mh_opt_mobile_node_identifier_t {
    __u8                ip6mnid_type;   /*!< \brief MN-ID-OPTION-TYPE has been assigned value 8 by the IANA. It is an  8-bit identifier of the type mobility option.*/
    __u8                ip6mnid_len;    /*!< \brief 8-bit unsigned integer, representing the length in octets of the Subtype and Identifier fields.*/
    __u8                ip6mnid_subtype;/*!< \brief Mobile interface identifier subtype .*/
    ip6mnid_t           ip6mnid_id;     /*!< \brief Mobile interface identifier (NOT RFC4283).*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_mobile_node_identifier_t ip6_mh_opt_mobile_node_identifier_t;



/*! \struct  ip6_mh_opt_mobile_node_link_layer_identifier_t
* \brief Mobility Option TLV data structure: Mobile Node Link-Layer Identifier Option (RFC5213).
*/
struct ip6_mh_opt_mobile_node_link_layer_identifier_t {
    __u8                ip6mnllid_type;     /*!< \brief MN-ID-OPTION-TYPE has been assigned value 8 by the IANA. It is an  8-bit identifier of the type mobility option.*/
    __u8                ip6mnllid_len;      /*!< \brief 8-bit unsigned integer, representing the length in octets of the Subtype and Identifier fields.*/
    __u16               ip6mnllid_reserved; /*!< \brief  16 bits reserved.*/
    ip6mnid_t           ip6mnllid_lli;      /*!< \brief Mobile node link-layer identifier.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_mobile_node_link_layer_identifier_t ip6_mh_opt_mobile_node_link_layer_identifier_t;


/*! \struct  ip6_mh_opt_time_stamp_t
* \brief Mobility Option TLV data structure: Timestamp Option (RFC5213).
*/
struct ip6_mh_opt_time_stamp_t {
    __u8                ip6mots_type;   /*!< \brief Option type, value is 27.*/
    __u8                ip6mots_len;    /*!< \brief 8-bit unsigned integer indicating the length in octets of the option, excluding the type and length fields. The value for this field MUST be set to 8.*/
    ip6ts_t             time_stamp;     /*!< \brief A 64-bit unsigned integer field containing a timestamp.  The value indicates the number of seconds since January 1, 1970, 00:00 UTC, by using a fixed point format.  In this format, the integer number of seconds is contained in the first 48 bits of the field, and the remaining 16 bits indicate the number of 1/65536 fractions of a second.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_time_stamp_t ip6_mh_opt_time_stamp_t;


/*! \struct  ip6_mh_link_local_add_t
* \brief Mobility Option TLV data structure: Link-local Address Option (RFC5213).
*/
struct ip6_mh_opt_link_local_address_t {
    __u8                ip6link_type;   /*!< \brief Option type, value is 26.*/
    __u8                ip6link_len;    /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 16.*/
    struct in6_addr     ip6link_addr;   /*!< \brief A sixteen-byte field containing the link-local address.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_link_local_address_t ip6_mh_opt_link_local_address_t;

/*! \struct  ip6_mh_handoff_indicator_t
* \brief Mobility Option TLV data structure: Handoff Indicator Option (RFC5213).
*/
struct ip6_mh_opt_handoff_indicator_t {
    __u8                ip6hi_type;     /*!< \brief Option type, value is 23.*/
    __u8                ip6hi_len;      /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 2.*/
    __u8                ip6hi_reserved; /*!< \brief Field unused for now (RFC5213), must be set to 0.*/
    __u8                ip6hi_hi;       /*!< \brief A 8-bit field containing the handoff indicator.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_handoff_indicator_t ip6_mh_opt_handoff_indicator_t;

/*! \struct  ip6_mh_handoff_indicator_t
* \brief Mobility Option TLV data structure: Handoff Indicator Option (RFC5213).
*/
struct ip6_mh_opt_access_technology_type_t {
    __u8                ip6att_type;     /*!< \brief Option type, value is 23.*/
    __u8                ip6att_len;      /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 2.*/
    __u8                ip6att_reserved; /*!< \brief Field unused for now (RFC5213), must be set to 0.*/
    __u8                ip6att_att;      /*!< \brief A 8-bit field containing the access technology type.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_access_technology_type_t ip6_mh_opt_access_technology_type_t;


// ******** Extended options for cluster based architecture & Route optimiztion ***********
struct ip6_mh_opt_dst_mn_addr_t {
    __u8                ip6dma_type;
    __u8                ip6dma_len;
    struct in6_addr     dst_mn_addr;    /* Destination MN Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_dst_mn_addr_t ip6_mh_opt_dst_mn_addr_t;


struct ip6_mh_opt_serv_mag_addr_t {
    __u8                ip6sma_type;
    __u8                ip6sma_len;
    struct in6_addr     serv_mag_addr;  /* Serving MAG Address of the destination */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_serv_mag_addr_t ip6_mh_opt_serv_mag_addr_t;


struct ip6_mh_opt_serv_lma_addr_t {
    __u8                ip6sla_type;
    __u8                ip6sla_len;
    struct in6_addr     serv_lma_addr;  /* Serving LMA Address of the destination */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_serv_lma_addr_t ip6_mh_opt_serv_lma_addr_t;


struct ip6_mh_opt_src_mn_addr {
    __u8                ip6sma_type;
    __u8                ip6sma_len;
    struct in6_addr     src_mn_addr;    /* Source MN Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_src_mn_addr_t ip6_mh_opt_src_mn_addr_t;


struct ip6_mh_opt_src_mag_addr_t {
    __u8                ip6sma_type;
    __u8                ip6sma_len;
    struct in6_addr     src_mag_addr;   /* Source MAG Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_src_mag_addr_t ip6_mh_opt_src_mag_addr_t;

/*! \struct  msg_info_t
* \brief Meta structure that can store all usefull information of any PMIP message.
*/
typedef struct msg_info_t {
    struct in6_addr             src;                /*!< \brief Source address of the message.*/
    struct in6_addr             dst;                /*!< \brief Destination address of the message.*/
    struct in6_addr_bundle      addrs;              /*!< \brief Tuple composed of src and dst address.*/
    int                         iif;                /*!< \brief Interface identifier.*/
    uint32_t                    msg_event;          /*!< \brief Type of event received: hasDEREG hasWLCCP hasRS hasNA hasNS hasPBU hasPBA hasPBREQ hasPBRES */
    struct in6_addr             mn_iid;             /*!< \brief Mobile node IID.*/
    struct in6_addr             mn_addr;            /*!< \brief Full mobile node address */
    struct in6_addr             mn_prefix;          /*!< \brief Network Address Prefix for MN */
    struct in6_addr             mn_serv_mag_addr;   /*!< \brief Serving MAG Address */
    struct in6_addr             mn_serv_lma_addr;   /*!< \brief Serving LMA Address */
    struct in6_addr             mn_link_local_addr; /*!< \brief Link Local Address  for mobile node */
    struct timespec             addtime;            /*!< \brief When was the binding added or modified */
    struct timespec             lifetime;           /*!< \brief Lifetime sent in this BU, in seconds */
    uint16_t                    seqno;              /*!< \brief Sequence number of the message */
    uint16_t                    PBU_flags;          /*!< \brief PBU flags */
    uint8_t                     PBA_flags;          /*!< \brief PBA flags */
    ip6ts_t                     timestamp;          /*!< \brief Timestamp */
    struct in6_addr             src_mag_addr;       /*!< \brief Route optimization or flow control: Source MAG Address */
    struct in6_addr             src_mn_addr;        /*!< \brief Route optimization or flow control: Source MN Address */
    struct in6_addr             na_target;          /*!< \brief Route optimization or flow control: Neighbour advertisement target*/
    struct in6_addr             ns_target;          /*!< \brief Route optimization or flow control: Neighbour solicitation target*/
    int                         is_dad;             /*!< \brief Route optimization or flow control: is NS used for DAD process?*/
    int                         hoplimit;           /*!< \brief Route optimization or flow control: Hop limit*/
} msg_info_t;
#endif
/*@}*/
