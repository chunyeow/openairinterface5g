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
/*! \file pmip_msgs.c
 * \brief creates new options and sends and parses PBU/PBA
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair_tech@eurecom.fr
 */
#define PMIP
#define PMIP_MSGS_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#include <sys/time.h>
#include <errno.h>
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_msgs.h"
//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
/*! \var struct sock mh_sock
\brief Global var declared in mipl component
 */
extern struct sock mh_sock;
/* We can use these safely, since they are only read and never change */
static const uint8_t _pad1[1] = { 0x00 };
static const uint8_t _pad2[2] = { 0x01, 0x00 };
static const uint8_t _pad3[3] = { 0x01, 0x01, 0x00 };
static const uint8_t _pad4[4] = { 0x01, 0x02, 0x00, 0x00 };
static const uint8_t _pad5[5] = { 0x01, 0x03, 0x00, 0x00, 0x00 };
static const uint8_t _pad6[6] = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t _pad7[7] = { 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };
//---------------------------------------------------------------------------------------------------------------------
static int create_opt_pad(struct iovec *iov, int pad)
{
    if (pad == 2) {
        //iov->iov_base = (void *) _pad2;
        iov->iov_base = malloc(sizeof(_pad2));
        memcpy(iov->iov_base, _pad2, sizeof(_pad2));
    }
    else if (pad == 4) {
        //iov->iov_base = (void *) _pad4;
        iov->iov_base = malloc(sizeof(_pad4));
        memcpy(iov->iov_base, _pad4, sizeof(_pad4));
    }
    else if (pad == 6) {
        //iov->iov_base = (void *) _pad6;
        iov->iov_base = malloc(sizeof(_pad6));
        memcpy(iov->iov_base, _pad6, sizeof(_pad6));
    }
    /* Odd pads do not occur with current spec, so test them last */
    else if (pad == 1) {
        iov->iov_base = (void *) _pad1;
        iov->iov_base = malloc(sizeof(_pad1));
        memcpy(iov->iov_base, _pad1, sizeof(_pad1));
    }
    else if (pad == 3) {
        //iov->iov_base = (void *) _pad3;
        iov->iov_base = malloc(sizeof(_pad3));
        memcpy(iov->iov_base, _pad3, sizeof(_pad3));
    }
    else if (pad == 5) {
        //iov->iov_base = (void *) _pad5;
        iov->iov_base = malloc(sizeof(_pad5));
        memcpy(iov->iov_base, _pad5, sizeof(_pad5));
    }
    else if (pad == 7) {
        //iov->iov_base = (void *) _pad7;
        iov->iov_base = malloc(sizeof(_pad7));
        memcpy(iov->iov_base, _pad7, sizeof(_pad7));
    }
    iov->iov_len = pad;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
static inline int optpad(int xn, int y, int offset)
{
    return ((y - offset) & (xn - 1));
}
//---------------------------------------------------------------------------------------------------------------------
static int mh_try_pad(const struct iovec *in, struct iovec *out, int count)
{
    size_t len = 0;
    int m, n = 1, pad = 0;
    struct ip6_mh_opt *opt;
    out[0].iov_len = in[0].iov_len;
    out[0].iov_base = in[0].iov_base;
    len += in[0].iov_len;
    for (m = 1; m < count; m++) {
        opt = (struct ip6_mh_opt *) in[m].iov_base;
        switch (opt->ip6mhopt_type) {
        case IP6_MHOPT_BREFRESH:
            pad = optpad(2, 0, len);    /* 2n */
            break;
        case IP6_MHOPT_ALTCOA:
            pad = optpad(8, 6, len);    /* 8n+6 */
            break;
        case IP6_MHOPT_NONCEID:
            pad = optpad(2, 0, len);    /* 2n */
            break;
        case IP6_MHOPT_BAUTH:
            pad = optpad(8, 2, len);    /* 8n+2 */
            break;
        case IP6_MHOPT_MOBILE_NODE_IDENTIFIER:
            pad = 0;    /* This option does not have any alignment requirements. */
            break;
        case IP6_MHOPT_HOME_NETWORK_PREFIX:
            pad = optpad(8, 4, len);    /* 8n+4 */
            break;
        case IP6_MHOPT_HANDOFF_INDICATOR:
            pad = 0;    /* The Handoff Indicator option has no alignment requirement. */
            break;
        case IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE:
            pad = 0;    /* The Access Technology Type Option has no alignment requirement. */
            break;
        case IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER:
            pad = optpad(8, 0, len);    /* 8n */
            break;
        case IP6_MHOPT_LINK_LOCAL_ADDRESS:
            pad = optpad(8, 6, len);    /* 8n+6 */
            break;
        case IP6_MHOPT_TIME_STAMP:
            pad = optpad(8, 2, len);    /* 8n+2 */
            break;


        }
        if (pad > 0) {
            create_opt_pad(&out[n++], pad);
            len += pad;
        }
        len += in[m].iov_len;
        out[n].iov_len = in[m].iov_len;
        out[n].iov_base = in[m].iov_base;
        n++;
    }
    if (count == 1) {
        pad = optpad(8, 0, len);
        create_opt_pad(&out[n++], pad);
    }
    return n;
}
//---------------------------------------------------------------------------------------------------------------------
static size_t mh_length(struct iovec *vec, int count)
{
    size_t len = 0;
    int i;
    for (i = 0; i < count; i++) {
        len += vec[i].iov_len;
    }
    return len;
}
//---------------------------------------------------------------------------------------------------------------------
void init_pbu_sequence_number(void) {
    g_mag_sequence_number = 0;
}
//---------------------------------------------------------------------------------------------------------------------
int get_new_pbu_sequence_number(void)
{
    // TO DO: should be thread protected
    int seq = g_mag_sequence_number;
    g_mag_sequence_number = g_mag_sequence_number + 1;
    return seq;
}
//---------------------------------------------------------------------------------------------------------------------
int is_pba_is_response_to_last_pbu_sent(msg_info_t * pba_info, pmip_entry_t *bce)
{
    if ( pba_info->seqno == bce->seqno_out) {
        if ((pba_info->timestamp.first == bce->timestamp.first) && (pba_info->timestamp.second == bce->timestamp.second)) {
            if (IN6_ARE_ADDR_EQUAL(&pba_info->mn_prefix, &bce->mn_prefix)) {
                if (IN6_ARE_ADDR_EQUAL(&pba_info->mn_iid, &bce->mn_suffix)) {
                    return 1;
                } else {
                    dbg("Not identical Mobile Node Link-layer Identifier Option: PBU:%x:%x:%x:%x:%x:%x:%x:%x  PBA:%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&bce->mn_suffix), NIP6ADDR(&pba_info->mn_iid));
                }
            } else {
                dbg("Not identical Home Network Prefix option: PBU:%x:%x:%x:%x:%x:%x:%x:%x  PBA:%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&bce->mn_prefix), NIP6ADDR(&pba_info->mn_prefix));
            }
        } else {
            dbg("Not identical Timestamp option: PBU:%08X%08X  PBA:%08X%08X\n", bce->timestamp.first, bce->timestamp.second, pba_info->timestamp.first, pba_info->timestamp.second);
        }
    } else {
        dbg("Not identical Sequence Number: PBU:%d  PBA:%d\n", bce->seqno_out, pba_info->seqno);
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr get_node_id(struct in6_addr *mn_addr)
{
    struct in6_addr result;
    result = in6addr_any;
    memcpy(&result.s6_addr32[2], &mn_addr->s6_addr32[2], sizeof(ip6mnid_t));
    return result;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr get_node_prefix(struct in6_addr *mn_addr)
{
    struct in6_addr result;
    result = in6addr_any;
    memcpy(&result.s6_addr32[0], &mn_addr->s6_addr32[0], PREFIX_LENGTH / 8);
    return result;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_home_net_prefix(struct iovec *iov, struct in6_addr *Home_Network_Prefix)
{
    /* From RFC 5213
     * 8.3. Home Network Prefix Option


        A new option, Home Network Prefix option is defined for use with the
        Proxy Binding Update and Proxy Binding Acknowledgement messages
        exchanged between a local mobility anchor and a mobile access
        gateway.  This option is used for exchanging the mobile node's home
        network prefix information.  There can be multiple Home Network
        Prefix options present in the message.

        The Home Network Prefix Option has an alignment requirement of 8n+4.
        Its format is as follows:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |      Type     |   Length      |   Reserved    | Prefix Length |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                                                               +
      |                                                               |
      +                    Home Network Prefix                        +
      |                                                               |
      +                                                               +
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

       Type
           22

       Length

           8-bit unsigned integer indicating the length of the option
           in octets, excluding the type and length fields.  This field
           MUST be set to 18.

       Reserved (R)

           This 8-bit field is unused for now.  The value MUST be
           initialized to 0 by the sender and MUST be ignored by the
           receiver.

       Prefix Length

           8-bit unsigned integer indicating the prefix length of the
           IPv6 prefix contained in the option.

       Home Network Prefix

           A sixteen-byte field containing the mobile node's IPv6 Home
           Network Prefix.
     */
    ip6_mh_opt_home_net_prefix_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_home_net_prefix_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_home_net_prefix_t *) iov->iov_base;
    opt->ip6hnp_type       = IP6_MHOPT_HOME_NETWORK_PREFIX;
    opt->ip6hnp_len        = 18;
    opt->ip6hnp_reserved   = 0;
    opt->ip6hnp_prefix_len = 128;   //128 bits
    opt->ip6hnp_prefix     = *Home_Network_Prefix;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_handoff_indicator(struct iovec *iov, int handoff_indicator)
{
    /* From RFC 5213
8.4. Handoff Indicator Option


   A new option, Handoff Indicator option is defined for use with the
   Proxy Binding Update and Proxy Binding Acknowledgement messages
   exchanged between a local mobility anchor and a mobile access
   gateway.  This option is used for exchanging the mobile node's
   handoff-related hints.

   The Handoff Indicator option has no alignment requirement.  Its
   format is as follows:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Type     |   Length      |  Reserved (R) |       HI      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    Type
        23

    Length

        8-bit unsigned integer indicating the length of the option
        in octets, excluding the type and length fields.  This field
        MUST be set to 2.

    Reserved (R)

        This 8-bit field is unused for now.  The value MUST be
        initialized to 0 by the sender and MUST be ignored by the
        receiver.

    Handoff Indicator (HI)

        An 8-bit field that specifies the type of handoff.  The values
        (0 - 255) will be allocated and managed by IANA.  The following
        values are currently defined.

        0: Reserved
        1: Attachment over a new interface
        2: Handoff between two different interfaces of the mobile node
        3: Handoff between mobile access gateways for the same interface
        4: Handoff state unknown
        5: Handoff state not changed (Re-registration)

     */
    ip6_mh_opt_handoff_indicator_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_handoff_indicator_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_handoff_indicator_t *) iov->iov_base;
    opt->ip6hi_type     = IP6_MHOPT_HANDOFF_INDICATOR;
    opt->ip6hi_len      = 2;  //set to 2 bytes
    opt->ip6hi_reserved = 0;
    opt->ip6hi_hi       = (__u8)handoff_indicator;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_access_technology_type(struct iovec *iov, int att)
{
    /* From RFC 5213
8.5. Access Technology Type Option


   A new option, Access Technology Type option is defined for use with
   the Proxy Binding Update and Proxy Binding Acknowledgement messages
   exchanged between a local mobility anchor and a mobile access
   gateway.  This option is used for exchanging the type of the access
   technology by which the mobile node is currently attached to the
   mobile access gateway.


   The Access Technology Type Option has no alignment requirement.  Its
   format is as follows:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Type     |   Length      |  Reserved (R) |      ATT      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    Type
        24

    Length

        8-bit unsigned integer indicating the length of the option
        in octets, excluding the type and length fields.  This field
        MUST be set to 2.

    Reserved (R)

        This 8-bit field is unused for now.  The value MUST be
        initialized to 0 by the sender and MUST be ignored by the
        receiver.

    Access Technology Type (ATT)

        An 8-bit field that specifies the access technology through
        which the mobile node is connected to the access link on the
        mobile access gateway.

        The values (0 - 255) will be allocated and managed by IANA.  The
        following values are currently reserved for the below specified
        access technology types.

        0: Reserved         ("Reserved")
        1: Virtual          ("Logical Network Interface")
        2: PPP              ("Point-to-Point Protocol")
        3: IEEE 802.3       ("Ethernet")
        4: IEEE 802.11a/b/g ("Wireless LAN")
        5: IEEE 802.16e     ("WIMAX")
*/
    ip6_mh_opt_access_technology_type_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_access_technology_type_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_access_technology_type_t *) iov->iov_base;
    opt->ip6att_type     = IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE;
    opt->ip6att_len      = 2;  //set to 2 bytes
    opt->ip6att_reserved = 0;
    opt->ip6att_att      = (__u8)att;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_mobile_node_identifier(struct iovec *iov, int subtype, ip6mnid_t * MN_ID)
{
    /* From RFC 4283
    3. Mobile Node Identifier Option


       The Mobile Node Identifier option is a new optional data field that
       is carried in the Mobile IPv6-defined messages that includes the
       Mobility header.  Various forms of identifiers can be used to
       identify a Mobile Node (MN).  Two examples are a Network Access
       Identifier (NAI) [RFC4282] and an opaque identifier applicable to a
       particular application.  The Subtype field in the option defines the
       specific type of identifier.

       This option can be used in mobility messages containing a mobility
       header.  The subtype field in the option is used to interpret the
       specific type of identifier.

           0                   1                   2                   3
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                                           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                           |  Option Type  | Option Length |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           |  Subtype      |          Identifier ...
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

          Option Type:
             MN-ID-OPTION-TYPE has been assigned value 8 by the IANA.  It is
             an  8-bit identifier of the type mobility option.

          Option Length:
             8-bit unsigned integer, representing the length in octets of
             the Subtype and Identifier fields.

          Subtype:
             Subtype field defines the specific type of identifier included
             in the Identifier field.

          Identifier:
             A variable length identifier of type, as specified by the
             Subtype field of this option.


       This option does not have any alignment requirements.

     */
    ip6_mh_opt_mobile_node_identifier_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_mobile_node_identifier_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_mobile_node_identifier_t *) iov->iov_base;
    opt->ip6mnid_type    = IP6_MHOPT_MOBILE_NODE_IDENTIFIER;
    opt->ip6mnid_len     = 9;  //set to 9 bytes.
    opt->ip6mnid_subtype = subtype;
    opt->ip6mnid_id      = *MN_ID;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_mobile_node_link_layer_identifier(struct iovec *iov, ip6mnid_t * mnlli)
{
    /* From RFC 5213
     * 8.6. Mobile Node Link-layer Identifier Option


   A new option, Mobile Node Link-layer Identifier option is defined for
   use with the Proxy Binding Update and Proxy Binding Acknowledgement
   messages exchanged between a local mobility anchor and a mobile
   access gateway.  This option is used for exchanging the mobile node's
   link-layer identifier.

   The format of the Link-layer Identifier option is shown below.  Based
   on the size of the identifier, the option MUST be aligned
   appropriately, as per mobility option alignment requirements
   specified in [RFC3775].

     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   Type        |    Length     |          Reserved             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                                                               |
    +                        Link-layer Identifier                  +
    .                              ...                              .
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

     Type
         25

     Length
         8-bit unsigned integer indicating the length of the option
         in octets, excluding the type and length fields.

     Reserved

         This field is unused for now.  The value MUST be initialized to
         0 by the sender and MUST be ignored by the receiver.

     Link-layer Identifier

         A variable length field containing the mobile node's link-layer
         identifier.

         The content and format of this field (including byte and bit
         ordering) is as specified in Section 4.6 of [RFC4861] for
         carrying link-layer addresses.  On certain access links, where
         the link-layer address is not used or cannot be determined,
         this option cannot be used.


     */
    ip6_mh_opt_mobile_node_link_layer_identifier_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_mobile_node_link_layer_identifier_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_mobile_node_link_layer_identifier_t *) iov->iov_base;
    opt->ip6mnllid_type     = IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER;
    opt->ip6mnllid_len      = 10;  //set to 10 bytes.
    opt->ip6mnllid_reserved = 0;
    opt->ip6mnllid_lli      = *mnlli;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_time_stamp(struct iovec *iov, ip6ts_t * Timestamp)
{
    /* From RFC 5213
     8.8. Timestamp Option


        A new option, Timestamp option is defined for use in the Proxy
        Binding Update and Proxy Binding Acknowledgement messages.

        The Timestamp option has an alignment requirement of 8n+2.  Its
        format is as follows:

          0                   1                   2                   3
          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                                          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                          |      Type     |   Length      |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |                                                               |
          +                          Timestamp                            +
          |                                                               |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

           Type
               27

           Length

               8-bit unsigned integer indicating the length in octets of
               the option, excluding the type and length fields.  The value
               for this field MUST be set to 8.

           Timestamp

               A 64-bit unsigned integer field containing a timestamp.  The
               value indicates the number of seconds since January 1, 1970,
               00:00 UTC, by using a fixed point format.  In this format, the
               integer number of seconds is contained in the first 48 bits of
               the field, and the remaining 16 bits indicate the number of
               1/65536 fractions of a second.
     */
    ip6_mh_opt_time_stamp_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_time_stamp_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len  = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_time_stamp_t *) iov->iov_base;
    opt->ip6mots_type = IP6_MHOPT_TIME_STAMP;
    opt->ip6mots_len  = 8;   // set to 8 bytes.
    opt->time_stamp   = *Timestamp;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_link_local_address(struct iovec *iov, struct in6_addr *lla)
{
    /* From RFC 5213
     8.7. Link-local Address Option


        A new option, Link-local Address option is defined for use with the
        Proxy Binding Update and Proxy Binding Acknowledgement messages
        exchanged between a local mobility anchor and a mobile access
        gateway.  This option is used for exchanging the link-local address
        of the mobile access gateway.

        The Link-local Address option has an alignment requirement of 8n+6.
        Its format is as follows:

            0                   1                   2                   3
            0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                                           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                           |   Type        |    Length     |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           |                                                               |
           +                                                               +
           |                                                               |
           +                  Link-local Address                           +
           |                                                               |
           +                                                               +
           |                                                               |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            Type
                26

            Length

                8-bit unsigned integer indicating the length of the option
                in octets, excluding the type and length fields.  This field
                MUST be set to 16.

            Link-local Address

                A sixteen-byte field containing the link-local address.
     */
    ip6_mh_opt_link_local_address_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_link_local_address_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_link_local_address_t *) iov->iov_base;
    opt->ip6link_type = IP6_MHOPT_LINK_LOCAL_ADDRESS;
    opt->ip6link_len  = 16;  //set to 16 bytes
    opt->ip6link_addr = *lla;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_dst_mn_addr(struct iovec *iov, struct in6_addr *dst_mn_addr)
{
    ip6_mh_opt_dst_mn_addr_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_dst_mn_addr_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_dst_mn_addr_t *) iov->iov_base;
    opt->ip6dma_type = IP6_MHOPT_DST_MN_ADDR;
    opt->ip6dma_len = 16;
    opt->dst_mn_addr = *dst_mn_addr;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_serv_mag_addr(struct iovec *iov, struct in6_addr *Serv_MAG_addr)
{
    ip6_mh_opt_serv_mag_addr_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_serv_mag_addr_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_serv_mag_addr_t *) iov->iov_base;
    opt->ip6sma_type = IP6_MHOPT_SERV_MAG_ADDR;
    opt->ip6sma_len = 16;   //16 bytes
    opt->serv_mag_addr = *Serv_MAG_addr;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_serv_lma_addr(struct iovec *iov, struct in6_addr *serv_lma_addr)
{
    ip6_mh_opt_serv_lma_addr_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_serv_lma_addr_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_serv_lma_addr_t *) iov->iov_base;
    opt->ip6sla_type = IP6_MHOPT_SERV_LMA_ADDR;
    opt->ip6sla_len = 16;   //16 bytes
    opt->serv_lma_addr = *serv_lma_addr;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_src_mn_addr(struct iovec *iov, struct in6_addr *src_mn_addr)
{
    struct ip6_mh_opt_src_mn_addr *opt;
    size_t optlen = sizeof(struct ip6_mh_opt_src_mn_addr);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (struct ip6_mh_opt_src_mn_addr *) iov->iov_base;
    opt->ip6sma_type = IP6_MHOPT_SRC_MN_ADDR;
    opt->ip6sma_len = 16;   //16 bytes
    opt->src_mn_addr = *src_mn_addr;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_src_mag_addr(struct iovec *iov, struct in6_addr *src_mag_addr)
{
    ip6_mh_opt_src_mag_addr_t *opt;
    size_t optlen = sizeof(ip6_mh_opt_src_mag_addr_t);
    iov->iov_base = malloc(optlen);
    iov->iov_len = optlen;
    if (iov->iov_base == NULL)
        return -ENOMEM;
    opt = (ip6_mh_opt_src_mag_addr_t *) iov->iov_base;
    opt->ip6sma_type = IP6_MHOPT_SRC_MAG_ADDR;
    opt->ip6sma_len = 16;   //16 bytes
    opt->src_mag_addr = *src_mag_addr;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_pbu_parse(msg_info_t * info, struct ip6_mh_binding_update *pbu, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
    static struct mh_options mh_opts;
    ip6_mh_opt_home_net_prefix_t                     *home_net_prefix_opt;
    ip6_mh_opt_mobile_node_identifier_t              *mobile_node_identifier_opt;
    ip6_mh_opt_mobile_node_link_layer_identifier_t   *mobile_node_link_layer_identifier_opt;
    ip6_mh_opt_time_stamp_t                          *time_stamp_opt;
    ip6_mh_opt_handoff_indicator_t                   *handoff_indicator_opt;
    ip6_mh_opt_access_technology_type_t              *access_technology_type_opt;
    struct in6_addr                                  mn_identifier;
    int                                              return_code;
    bzero(&mh_opts, sizeof(mh_opts));
    info->src = *in_addrs->src;
    info->dst = *in_addrs->dst;
    info->iif = iif;
    info->addrs.src = &info->src;
    info->addrs.dst = &info->dst;
    if (len < (ssize_t)sizeof(struct ip6_mh_binding_update)) {
        dbg("Bad len of PBU mobility header   : %d versus sizeof(struct ip6_mh_binding_update)= %d\n", len, sizeof(struct ip6_mh_binding_update));
        return 0;
    }
    if ((return_code = mh_opt_parse(&pbu->ip6mhbu_hdr, len, sizeof(struct ip6_mh_binding_update), &mh_opts)) < 0) {
        dbg("Error %d in parsing PBU options\n", return_code);
        return 0;
    }
    info->PBU_flags = ntohs(pbu->ip6mhbu_flags);
    info->lifetime.tv_sec = (ntohs(pbu->ip6mhbu_lifetime) << 2);
    info->seqno = ntohs(pbu->ip6mhbu_seqno);
    dbg("Serving MAG Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->src));
    dbg("Our Address        : %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->dst));
    dbg("PBU FLAGS          : %04X\n", info->PBU_flags);
    dbg("PBU Lifetime       : %d (%d seconds)\n", pbu->ip6mhbu_lifetime, info->lifetime.tv_sec);
    dbg("PBU Sequence No    : %d\n", info->seqno);

    mobile_node_identifier_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_IDENTIFIER);
    if (mobile_node_identifier_opt) {
        //copy
        mn_identifier = in6addr_any;
        memcpy(&mn_identifier.s6_addr32[2], &mobile_node_identifier_opt->ip6mnid_id, sizeof(ip6mnid_t));
        dbg("Mobile Node Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&mn_identifier));
    }

    home_net_prefix_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_HOME_NETWORK_PREFIX);
    if (home_net_prefix_opt)
    {
        //copy
        info->mn_prefix = home_net_prefix_opt->ip6hnp_prefix;
        dbg("Mobile Node Home Network Prefix option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_prefix));
    }

    mobile_node_link_layer_identifier_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER);
    if (mobile_node_link_layer_identifier_opt) {
        //copy
        info->mn_iid = in6addr_any;
        memcpy(&info->mn_iid.s6_addr32[2], &mobile_node_link_layer_identifier_opt->ip6mnllid_lli, sizeof(ip6mnid_t));
        dbg("Mobile Node Link-layer Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
    }

    time_stamp_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_TIME_STAMP);
    if (time_stamp_opt) {
        //copy
        info->timestamp.first = time_stamp_opt->time_stamp.first;
        info->timestamp.second = time_stamp_opt->time_stamp.second;
        unsigned long long int seconds = info->timestamp.first << 16;
        seconds = seconds | ((info->timestamp.second & 0xFFFF0000) >> 16);
        unsigned int useconds = (info->timestamp.second & 0x0000FFFF) * 1000000 / 65536;
        dbg("Timestamp option: %ld sec %d usec\n", seconds, useconds);
    }

    handoff_indicator_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_HANDOFF_INDICATOR);
    if (handoff_indicator_opt) {
        dbg("Handoff Indicator option: %d\n", handoff_indicator_opt->ip6hi_hi);
    }

    access_technology_type_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE);
    if (access_technology_type_opt) {
        dbg("Access Technology Type option: %d\n", access_technology_type_opt->ip6att_att);
    }

    info->msg_event = hasPBU;
    dbg("FSM Message Event: %d\n", info->msg_event);
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_pba_parse(msg_info_t * info, struct ip6_mh_binding_ack *pba, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
    static struct mh_options mh_opts;
    ip6_mh_opt_home_net_prefix_t                     *home_net_prefix_opt;
    ip6_mh_opt_mobile_node_identifier_t              *mobile_node_identifier_opt;
    ip6_mh_opt_mobile_node_link_layer_identifier_t   *mobile_node_link_layer_identifier_opt;
    ip6_mh_opt_time_stamp_t                          *time_stamp_opt;
    ip6_mh_opt_handoff_indicator_t                   *handoff_indicator_opt;
    ip6_mh_opt_access_technology_type_t              *access_technology_type_opt;
    struct in6_addr                                  mn_identifier;

    bzero(&mh_opts, sizeof(mh_opts));
    info->src = *in_addrs->src;
    info->dst = *in_addrs->dst;
    info->iif = iif;
    info->addrs.src = &info->src;
    info->addrs.dst = &info->dst;

    mobile_node_identifier_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_IDENTIFIER);
    if (mobile_node_identifier_opt) {
        //copy
        mn_identifier = in6addr_any;
        memcpy(&mn_identifier.s6_addr32[2], &mobile_node_identifier_opt->ip6mnid_id, sizeof(ip6mnid_t));
        dbg("Mobile Node Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&mn_identifier));
    }

    if ((len < (ssize_t)sizeof(struct ip6_mh_binding_ack)
            || mh_opt_parse(&pba->ip6mhba_hdr, len, sizeof(struct ip6_mh_binding_ack), &mh_opts) < 0)) {
        return 0;
    }
    home_net_prefix_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_HOME_NETWORK_PREFIX);
    if (home_net_prefix_opt) {
        info->mn_prefix = home_net_prefix_opt->ip6hnp_prefix;
        dbg("Mobile Node Home Network Prefix option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_prefix));
    }

    mobile_node_link_layer_identifier_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER);
    if (mobile_node_link_layer_identifier_opt) {
        //copy
        info->mn_iid = in6addr_any;
        memcpy(&info->mn_iid.s6_addr32[2], &mobile_node_link_layer_identifier_opt->ip6mnllid_lli, sizeof(ip6mnid_t));
        dbg("Mobile Node Link-layer Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
    }

    time_stamp_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_TIME_STAMP);
    if (time_stamp_opt) {
        //copy
        info->timestamp.first = time_stamp_opt->time_stamp.first;
        info->timestamp.second = time_stamp_opt->time_stamp.second;
        unsigned long long int seconds = info->timestamp.first << 16;
        seconds = seconds | ((info->timestamp.second & 0xFFFF0000) >> 16);
        unsigned int useconds = (info->timestamp.second & 0x0000FFFF) * 1000000 / 65536;
        dbg("Timestamp option: %ld sec %d usec\n", seconds, useconds);
    }

    handoff_indicator_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_HANDOFF_INDICATOR);
    if (handoff_indicator_opt) {
        dbg("Handoff Indicator option: %d\n", handoff_indicator_opt->ip6hi_hi);
    }

    access_technology_type_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE);
    if (access_technology_type_opt) {
        dbg("Access Technology Type option: %d\n", access_technology_type_opt->ip6att_att);
    }

    info->seqno = ntohs(pba->ip6mhba_seqno);
    info->PBA_flags = ntohs(pba->ip6mhba_flags);
    info->lifetime.tv_sec = ntohs(pba->ip6mhba_lifetime) << 2;
    dbg("PBA FLAGS      : %04X\n", info->PBA_flags);
    dbg("PBA Lifetime   : %d (%d seconds)\n", pba->ip6mhba_lifetime, info->lifetime.tv_sec);
    dbg("PBA Sequence No: %d\n", info->seqno);
    info->msg_event = hasPBA;
    dbg("FSM Message Event: %d\n", info->msg_event);
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int icmp_rs_parse(msg_info_t * info, __attribute__ ((unused)) struct nd_router_solicit *rs, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
    bzero(info, sizeof(msg_info_t));
    //info->ns_target = ns->nd_ns_target;
    info->hoplimit = hoplimit;
    info->msg_event = hasRS;
    info->src = *saddr;
    info->dst = *daddr;
    info->iif = iif;
    info->addrs.src = &info->src;
    info->addrs.dst = &info->dst;
    //Calculated fields
    info->mn_iid = get_node_id(&info->src);
    dbg("MN IID: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int icmp_na_parse(msg_info_t * info, struct nd_neighbor_advert *na, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
    bzero(info, sizeof(msg_info_t));
    info->na_target = na->nd_na_target;
    info->hoplimit = hoplimit;
    info->msg_event = hasNA;
    info->src = *saddr;
    info->dst = *daddr;
    info->iif = iif;
    info->addrs.src = &info->src;
    info->addrs.dst = &info->dst;
    //Calculated fields
    info->mn_iid = get_node_id(&info->na_target);
    info->mn_addr = info->na_target;
    info->mn_prefix = get_node_prefix(&info->na_target);
    dbg("NA Target: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&na->nd_na_target));
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_mh_send(const struct in6_addr_bundle *addrs, const struct iovec *mh_vec, int iovlen, int oif)
{
    struct sockaddr_in6 daddr;
    struct iovec iov[2 * (IP6_MHOPT_MAX + 1)];
    struct msghdr msg;
    struct cmsghdr *cmsg;
    int cmsglen;
    struct in6_pktinfo pinfo;
    int ret = 0, on = 1;
    struct ip6_mh *mh;
    int iov_count;

    iov_count = mh_try_pad(mh_vec, iov, iovlen);

    mh = (struct ip6_mh *) iov[0].iov_base;
    mh->ip6mh_hdrlen = (mh_length(iov, iov_count) >> 3) - 1;
    dbg("Sending MH type %d\n" "from %x:%x:%x:%x:%x:%x:%x:%x\n" "to %x:%x:%x:%x:%x:%x:%x:%x\n", mh->ip6mh_type, NIP6ADDR(addrs->src), NIP6ADDR(addrs->dst));

    memset(&daddr, 0, sizeof(struct sockaddr_in6));
    daddr.sin6_family = AF_INET6;
    daddr.sin6_addr = *addrs->dst;
    daddr.sin6_port = htons(IPPROTO_MH);

    memset(&pinfo, 0, sizeof(pinfo));
    pinfo.ipi6_addr = *addrs->src;
    pinfo.ipi6_ifindex = oif;

    cmsglen = CMSG_SPACE(sizeof(pinfo));
    cmsg = malloc(cmsglen);

    if (cmsg == NULL) {
        dbg("malloc failed\n");
        return -ENOMEM;
    }
    memset(cmsg, 0, cmsglen);
    memset(&msg, 0, sizeof(msg));
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsglen;
    msg.msg_iov = iov;
    msg.msg_iovlen = iov_count;
    msg.msg_name = (void *) &daddr;
    msg.msg_namelen = sizeof(daddr);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(pinfo));
    cmsg->cmsg_level = IPPROTO_IPV6;
    cmsg->cmsg_type = IPV6_PKTINFO;
    memcpy(CMSG_DATA(cmsg), &pinfo, sizeof(pinfo));

    pthread_mutex_lock(&mh_sock.send_mutex);
    setsockopt(mh_sock.fd, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(int));
    ret = sendmsg(mh_sock.fd, &msg, 0);
    if (ret < 0) {
        dbg("sendmsg: %s\n", strerror(errno));
    }
    pthread_mutex_unlock(&mh_sock.send_mutex);
    free(msg.msg_control);
    dbg("MH is sent....\n");
    return ret;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_send_pbu(const struct in6_addr_bundle *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif)
{
    /*  FROM RFC 5213
     *
     * 6.9.1.5. Constructing the Proxy Binding Update Message

       o  The mobile access gateway, when sending the Proxy Binding Update
          message to the local mobility anchor, MUST construct the message
          as specified below.

              IPv6 header (src=Proxy-CoA, dst=LMAA)
                Mobility header
                   - BU  P & A flags MUST be set to value 1
                  Mobility Options
                   - Mobile Node Identifier option            (mandatory)
                   - Home Network Prefix option(s)            (mandatory)
                   - Handoff Indicator option                 (mandatory)
                   - Access Technology Type option            (mandatory)
                   - Timestamp option                         (optional)
                   - Mobile Node Link-layer Identifier option (optional)
                   - Link-local Address option                (optional)

                    Figure 12: Proxy Binding Update Message Format


       o  The Source Address field in the IPv6 header of the message MUST be
          set to the global address configured on the egress interface of
          the mobile access gateway.  When there is no Alternate Care-of
          Address option present in the request, this address will be
          considered as the Proxy-CoA for this Proxy Binding Update message.
          However, when there is an Alternate Care-of Address option present
          in the request, this address will be not be considered as the
          Proxy-CoA, but the address in the Alternate Care-of Address option
          will be considered as the Proxy-CoA.

       o  The Destination Address field in the IPv6 header of the message
          MUST be set to the local mobility anchor address.

       o  The Mobile Node Identifier option [RFC4283] MUST be present.

       o  At least one Home Network Prefix option MUST be present.

       o  The Handoff Indicator option MUST be present.

       o  The Access Technology Type option MUST be present.

       o  The Timestamp option MAY be present.

       o  The Mobile Node Link-layer Identifier option MAY be present.

       o  The Link-local Address option MAY be present.

       o  If IPsec is used for protecting the signaling messages, the
          message MUST be protected, using the security association existing
          between the local mobility anchor and the mobile access gateway.

       o  Unlike in Mobile IPv6 [RFC3775], the Home Address option [RFC3775]
          MUST NOT be present in the IPv6 Destination Options extension
          header of the Proxy Binding Update message.
    */

    int                           iovlen = 1;
    struct ip6_mh_binding_update *pbu;
    static struct iovec           mh_vec[2 * (IP6_MHOPT_MAX + 1)];
    static struct iovec           dummy_mh_vec[2 * (IP6_MHOPT_MAX + 1)];
    struct timeval                tv;
    struct timezone               tz;
    ip6mnid_t                     mn_id;
    ip6mnid_t                     mn_hardware_address;
    uint16_t                      p_flag = 1;
    int                           iov_count;

    memset((void*)mh_vec       , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));
    memset((void*)dummy_mh_vec , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));

    pbu = mh_create(&mh_vec[0], IP6_MH_TYPE_BU);
    if (!pbu) {
        dbg("mh_create(&mh_vec[0], IP6_MH_TYPE_BU) failed\n");
        return -ENOMEM;
    }
    bce->seqno_out        = get_new_pbu_sequence_number();
    pbu->ip6mhbu_seqno    = htons(bce->seqno_out);
    pbu->ip6mhbu_flags    = bce->PBU_flags; // no htons since endianess already taken in account in ip6mh.h
    pbu->ip6mhbu_lifetime = htons(lifetime->tv_sec >> 2);
    dbg("Create PBU with lifetime = %d seconds (config = %d seconds)\n", lifetime->tv_sec, conf.PBULifeTime);
    memcpy(&mn_id, &bce->mn_suffix.s6_addr32[2], sizeof(ip6mnid_t));
    memcpy(&mn_hardware_address, &bce->mn_hw_address.s6_addr32[2], sizeof(ip6mnid_t));

    dbg("Create PBU options...\n");
    mh_create_opt_mobile_node_identifier(&mh_vec[iovlen++], p_flag, &mn_hardware_address);
    mh_create_opt_home_net_prefix(&mh_vec[iovlen++], &bce->mn_prefix);
    mh_create_opt_handoff_indicator(&mh_vec[iovlen++], IP6_MHOPT_HI_HANDOFF_BETWEEN_MAGS_FOR_SAME_INTERFACE);
    mh_create_opt_access_technology_type(&mh_vec[iovlen++], IP6_MHOPT_ATT_IEEE802_11ABG);
    mh_create_opt_mobile_node_link_layer_identifier(&mh_vec[iovlen++], &mn_id);

    tz.tz_minuteswest = 0;          /* minutes west of Greenwich */
    tz.tz_dsttime     = 3; //DST_WET;    /* type of DST correction */
    if (gettimeofday(&tv, &tz) == 0) {
        bce->timestamp.first  = tv.tv_sec >> 16;
        bce->timestamp.second = (tv.tv_sec & 0x0000FFFF) << 16;
        bce->timestamp.second += (((tv.tv_usec * 65536)/1000000) & 0x0000FFFF);
        mh_create_opt_time_stamp(&mh_vec[iovlen++], &bce->timestamp);
    } else {
        bce->timestamp.first  = 0;
        bce->timestamp.second = 0;
        dbg("Timestamp option failed to get time, discard option\n");
    }

    iov_count = mh_try_pad(mh_vec, dummy_mh_vec, iovlen);


    //calculate the length of the message.
    pbu->ip6mhbu_hdr.ip6mh_hdrlen = mh_length(dummy_mh_vec, iov_count);

    dbg("Send PBU....\n");
    pmip_mh_send(addrs, dummy_mh_vec, iov_count, oif);

    dbg("Copy PBU message into TEMP PMIP entry iovec....\n");
    //copy the PBU message into the mh_vector for the entry for future retransmissions.
    memcpy((void*)(bce->mh_vec), (void*)mh_vec, 2 * (IP6_MHOPT_MAX + 1)* sizeof(struct iovec));
    bce->iovlen = iovlen;
    //do not free, keep for retransmission free_iov_data(dummy_mh_vec, iov_count);
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_send_pba(const struct in6_addr_bundle *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif)
{
    static struct iovec           mh_vec[2 * (IP6_MHOPT_MAX + 1)];
    static struct iovec           dummy_mh_vec[2 * (IP6_MHOPT_MAX + 1)];
    volatile int                  iovlen = 1;
    struct ip6_mh_binding_ack    *pba;
    uint16_t                      p_flag = 1;
    ip6mnid_t                     mn_id;
    ip6mnid_t                     mn_hardware_address;
    int                           iov_count;

    //bzero(mh_vec, sizeof(mh_vec));
    memset((void*)mh_vec       , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));
    memset((void*)dummy_mh_vec , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));

    pba = mh_create(&mh_vec[0], IP6_MH_TYPE_BACK);
    if (!pba) {
        dbg("mh_create() failed\n");
        return -ENOMEM;
    }
    dbg("Create PBA with lifetime = %d seconds\n", lifetime->tv_sec);
    pba->ip6mhba_status = bce->status;
    pba->ip6mhba_flags  = bce->PBA_flags;
    pba->ip6mhba_seqno  = htons(bce->seqno_in);

    bce->seqno_out      = bce->seqno_in;

    pba->ip6mhba_lifetime = htons(lifetime->tv_sec >> 2);
    dbg("Create PBA options....\n");
    memcpy(&mn_id, &bce->mn_suffix.s6_addr32[2], sizeof(ip6mnid_t));
    memcpy(&mn_hardware_address, &bce->mn_hw_address.s6_addr32[2], sizeof(ip6mnid_t));
    mh_create_opt_mobile_node_identifier(&mh_vec[iovlen++], p_flag, &mn_hardware_address);
    mh_create_opt_home_net_prefix(&mh_vec[iovlen++], &bce->mn_prefix);
    mh_create_opt_mobile_node_link_layer_identifier(&mh_vec[iovlen++], &mn_id);
    mh_create_opt_time_stamp(&mh_vec[iovlen++], &bce->timestamp);

    iov_count = mh_try_pad(mh_vec, dummy_mh_vec, iovlen);

    //calculate the length of the message.
    pba->ip6mhba_hdr.ip6mh_hdrlen = mh_length(dummy_mh_vec, iov_count);
    dbg("Send PBA...\n");
    pmip_mh_send(addrs, dummy_mh_vec, iov_count, oif);
    free_iov_data(dummy_mh_vec, iov_count);
    return 0;
}
