/*
 * GTPu klm for Linux/iptables
 *
 * Copyright (c) 2010-2011 Polaris Networks
 * Author: Pradip Biswas <pradip_biswas@polarisnetworks.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/if_ether.h>
#include <linux/route.h> 
#include <net/checksum.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/route.h> 
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#if 0
#include <net/netfilter/ipv4/nf_defrag_ipv4.h>
#endif
#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
#    define WITH_IPV6 1
#endif


//#define ROUTE_PACKET 1
#define NEW_SKB 1

#include "xt_GTPURH.h"

#if !(defined KVERSION)
#error "Kernel version is not defined!!!! Exiting."
#endif

#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradip Biswas <pradip_biswas@polarisnetworks.net>");
MODULE_DESCRIPTION("GTPu Data Path extension on netfilter");

static char*         _gtpurh_nf_inet_hook_2_string(int nf_inet_hookP);
static inline bool   _gtpurh_ip_is_fragment(const struct iphdr *iph_p);
static void          _gtpurh_print_hex_octets(unsigned char* data_pP, unsigned short sizeP);
static unsigned int  _gtpurh_tg4_rem(struct sk_buff *orig_skb_pP, const struct xt_action_param *par_pP);
#if defined(WITH_IPV6)
static unsigned int  gtpurh_tg6(struct sk_buff *skb_pP, const struct xt_action_param *par_pP);
#endif
static unsigned int  gtpurh_tg4(struct sk_buff *skb_pP, const struct xt_action_param *par_pP);

static struct xt_target gtpurh_tg_reg[] __read_mostly = {
                 {
                                 .name           = "GTPURH",
                                 .revision       = 0,
                                 .family         = NFPROTO_IPV4,
                                 .hooks          = (1 << NF_INET_PRE_ROUTING) |
                                                   (1 << NF_INET_LOCAL_OUT),
                                 .proto          = IPPROTO_UDP,
                                 .table          = "raw",
                                 .target         = gtpurh_tg4,
                                 .targetsize     = sizeof(struct xt_gtpurh_target_info),
                                 .me             = THIS_MODULE,
                 },
#if defined(WITH_IPV6)
                 {
                                 .name       = "GTPURH",
                                 .revision   = 0,
                                 .family     = NFPROTO_IPV6,
                                 .proto      = IPPROTO_UDP,
                                 .table      = "raw",
                                 .target     = gtpurh_tg6,
                                 .me         = THIS_MODULE,
                },
#endif
};

struct gtpuhdr
{
    char flags;
    char msgtype;
    u_int16_t length;
    u_int32_t tunid;
};

#define GTPURH_HDR_PNBIT 1
#define GTPURH_HDR_SBIT 1 << 1
#define GTPURH_HDR_EBIT 1 << 2
#define GTPURH_ANY_EXT_HDR_BIT (GTPURH_HDR_PNBIT | GTPURH_HDR_SBIT | GTPURH_HDR_EBIT)

#define GTPURH_FAILURE 1
#define GTPURH_SUCCESS !GTPURH_FAILURE

#define GTPURH_PORT 2152

#define GTPURH_2_PRINT_BUFFER_LEN 8192
static char _gtpurh_print_buffer[GTPURH_2_PRINT_BUFFER_LEN];


//-----------------------------------------------------------------------------
static char*
_gtpurh_nf_inet_hook_2_string(int nf_inet_hookP) {
    //-----------------------------------------------------------------------------
    switch (nf_inet_hookP) {
        case NF_INET_PRE_ROUTING:   return "NF_INET_PRE_ROUTING";break;
        case NF_INET_LOCAL_IN:      return "NF_INET_LOCAL_IN";break;
        case NF_INET_FORWARD:       return "NF_INET_FORWARD";break;
        case NF_INET_LOCAL_OUT:     return "NF_INET_LOCAL_OUT";break;
        case NF_INET_POST_ROUTING:  return "NF_INET_POST_ROUTING";break;
        default: return "NF_INET_UNKNOWN";
    }
}

//-----------------------------------------------------------------------------
static inline bool
_gtpurh_ip_is_fragment(const struct iphdr *iph_p) {
    //-----------------------------------------------------------------------------
    return (iph_p->frag_off & htons(IP_MF | IP_OFFSET)) != 0;
}

//-----------------------------------------------------------------------------
void _gtpurh_print_hex_octets(unsigned char* data_pP, unsigned short sizeP)
//-----------------------------------------------------------------------------
{
  unsigned long octet_index = 0;
  unsigned long buffer_marker = 0;
  unsigned char aindex;
  struct timeval tv;
  char timeofday[64];
  unsigned int h,m,s;

  if (data_pP == NULL) {
    return;
  }
  if (sizeP > 2000) {
      return;
  }

  do_gettimeofday(&tv);
  h = (tv.tv_sec/3600) % 24;
  m = (tv.tv_sec / 60) % 60;
  s = tv.tv_sec % 60;
  snprintf(timeofday, 64, "%02d:%02d:%02d.%06ld", h,m,s,tv.tv_usec);

  buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker,"%s------+-------------------------------------------------+\n",timeofday);
  buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker,"%s      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n",timeofday);
  buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker,"%s------+-------------------------------------------------+\n",timeofday);
  pr_info("%s",_gtpurh_print_buffer);buffer_marker = 0;
  for (octet_index = 0; octet_index < sizeP; octet_index++) {
    if ((octet_index % 16) == 0){
      if (octet_index != 0) {
          buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
          pr_info("%s",_gtpurh_print_buffer);buffer_marker = 0;
      }
      buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, "%s %04ld |",timeofday, octet_index);
    }
    /*
     * Print every single octet in hexadecimal form
     */
    buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, " %02x", data_pP[octet_index]);
    /*
     * Align newline and pipes according to the octets in groups of 2
     */
  }

  /*
   * Append enough spaces and put final pipe
   */
  for (aindex = octet_index; aindex < 16; ++aindex)
    buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, "   ");
    //SGI_IF_DEBUG("   ");
  buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
  pr_info("%s",_gtpurh_print_buffer);
}


#if defined(ROUTE_PACKET)
//-----------------------------------------------------------------------------
static bool _gtpurh_route_packet(struct sk_buff *skb_pP, const struct xt_gtpurh_target_info *info_pP)
//-----------------------------------------------------------------------------
{
    int            err   = 0;
    struct rtable *rt    = NULL;
    struct iphdr  *iph_p = ip_hdr(skb_pP);
    int            daddr = iph_p->daddr;
    struct flowi   fl    = {
        .u = {
            .ip4 = {
                .daddr        = daddr,
                .flowi4_tos   = RT_TOS(iph_p->tos),
                .flowi4_scope = RT_SCOPE_UNIVERSE,
            } 
        } 
    }; 
    //skb_pP->pkt_type = PACKET_OTHERHOST;
    skb_pP->pkt_type = PACKET_OUTGOING;
#if 1
    pr_info("GTPURH(%d): Routing packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Len: %d Mark: %u Packet type: %u\n",
            info_pP->action,
            iph_p->saddr  & 0xFF,
            (iph_p->saddr & 0x0000FF00) >> 8,
            (iph_p->saddr & 0x00FF0000) >> 16,
            iph_p->saddr >> 24,
            iph_p->daddr  & 0xFF,
            (iph_p->daddr & 0x0000FF00) >> 8,
            (iph_p->daddr & 0x00FF0000) >> 16,
            iph_p->daddr >> 24,
            iph_p->protocol,
            ntohs(iph_p->tot_len),
            skb_pP->mark,
            skb_pP->pkt_type);
#endif
    rt = ip_route_output_key(&init_net, &fl.u.ip4);
    if (err != 0) {
        pr_info("GTPURH: Failed to route packet to dst 0x%x. Error: (%d)", fl.u.ip4.daddr, err);
        return GTPURH_FAILURE;
    } 

#if 1
    if (rt->dst.dev) {
        pr_info("GTPURH: dst dev name %s\n", rt->dst.dev->name);
    } else {
        pr_info("GTPURH: dst dev NULL\n");
    }
#endif

    skb_pP->priority = rt_tos2priority(iph_p->tos);
    skb_dst_drop(skb_pP);
    skb_dst_set(skb_pP, &rt->dst);
    skb_pP->dev      = skb_dst(skb_pP)->dev;

    // Send the GTPu message out
    ip_local_out(skb_pP);

    if (err == 0) {
        return GTPURH_SUCCESS;
    } else {
        return GTPURH_FAILURE;
    }
}
#endif

//-----------------------------------------------------------------------------
static unsigned int
_gtpurh_tg4_rem(struct sk_buff *orig_skb_pP, const struct xt_action_param *par_pP) {
//-----------------------------------------------------------------------------

    struct iphdr   *iph_p            = ip_hdr(orig_skb_pP);
    struct iphdr   *iph2_p           = NULL;
    struct udphdr  *udph_p           = NULL;
    struct gtpuhdr *gtpuh_p          = NULL;
    struct sk_buff *skb_p            = NULL;
#if defined(NEW_SKB)
    struct sk_buff *new_skb_p        = NULL;
    struct iphdr   *new_ip_p         = NULL;
#endif
    uint16_t        gtp_payload_size = 0;

    /* Create a new copy of the original skb_p...can't avoid :-( LG: WHY???*/
#if defined(ROUTE_PACKET)
    skb_p = skb_copy(orig_skb_pP, GFP_ATOMIC);
    if (skb_p == NULL) {
        return NF_ACCEPT;
    }
    skb_p->skb_iif  = orig_skb_pP->skb_iif;
    pr_info("GTPURH: skb protocol %04X\n", orig_skb_pP->protocol);
    skb_p->protocol = orig_skb_pP->protocol;
#else
    skb_p = orig_skb_pP;
    if (skb_linearize(skb_p) < 0) {
        pr_info("GTPURH: skb DROPPED (no linearize)\n");
        return NF_DROP;
    }
#endif
    //---------------------------
    // check if is GTPU TUNNEL
    if (iph_p->protocol != IPPROTO_UDP) {
        pr_info("GTPURH: skb DROPPED Not GTPV1U packet (not UDP)\n");
        return NF_ACCEPT;
    }

    //---------------------------
    // check if is fragment
    // but should not happen since MTU should have been set bigger than 1500 + GTP encap.
    // TO DO later segment, but did not succeed in getting in this target all framents of an ip packet!
    if (_gtpurh_ip_is_fragment(iph_p)) {
        pr_info("GTPURH: ip_is_fragment YES, FLAGS %04X & %04X = %04X\n",
                iph_p->frag_off,
                htons(IP_MF | IP_OFFSET),
                iph_p->frag_off & htons(IP_MF | IP_OFFSET));
            return NF_ACCEPT;
    }
    /*pr_info("GTPURH(%d): IN : GTPV1U skbuff len %u data_len %u data %p head %p tail %p end %p nwh %p th %p inwh %p ith %p\n",
            tgi_pP->action,
            skb_p->len,
            skb_p->data_len,
            skb_p->data,
            skb_p->head,
            skb_p->tail,
            skb_p->end,
            skb_network_header(skb_p),
            skb_transport_header(skb_p),
            skb_inner_network_header(skb_p),
            skb_inner_transport_header(skb_p));*/

    if (skb_p->len <= sizeof (struct udphdr) + sizeof (struct gtpuhdr) + sizeof (struct iphdr)) {
        pr_info("GTPURH: Thought was GTPV1U packet but too short length\n");
        return NF_ACCEPT;
    }
    /* Remove IP header */
    udph_p = (struct udphdr*)skb_pull(skb_p, (iph_p->ihl << 2));

    if (udph_p->dest != htons(GTPURH_PORT)) {
        pr_info("GTPURH: Not GTPV1U packet (bad UDP dest port)\n");
        skb_push(skb_p, (iph_p->ihl << 2));
        return NF_ACCEPT;
    }

/*    if (iph_p->protocol != IPPROTO_UDP) {
        pr_info("GTPURH(%d): ERROR in decapsulating packet: %d.%d.%d.%d --> %d.%d.%d.%d Bad Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
                tgi_pP->action,
                iph_p->saddr  & 0xFF,
                (iph_p->saddr & 0x0000FF00) >> 8,
                (iph_p->saddr & 0x00FF0000) >> 16,
                iph_p->saddr >> 24,
                iph_p->daddr  & 0xFF,
                (iph_p->daddr & 0x0000FF00) >> 8,
                (iph_p->daddr & 0x00FF0000) >> 16,
                iph_p->daddr >> 24,
                iph_p->protocol,
                ntohs(iph2_p->tot_len),
                skb_p->mark,
                ntohs(iph_p->frag_off) & 0x1FFFFFFF,
                ntohs(iph_p->frag_off) >> 13);
        return NF_ACCEPT;
    }*/

    /* Remove UDP header */
    gtpuh_p = (struct gtpuhdr*)skb_pull(skb_p, sizeof(struct udphdr));
    gtp_payload_size = ntohs(gtpuh_p->length);

    skb_p->mark = ntohl(gtpuh_p->tunid);
    /* Remove GTPu header */
    skb_pull(skb_p, sizeof(struct gtpuhdr));

    /* If additional fields are present in header, remove them also */
    if (gtpuh_p->flags & GTPURH_ANY_EXT_HDR_BIT)
    {
        pr_info("GTPURH: GTPURH_ANY_EXT_HDR_BIT found\n");
        skb_pull(skb_p, sizeof(short) + sizeof(char) + sizeof(char)); /* #Seq, #N-PDU, #ExtHdr Type */
        gtp_payload_size = gtp_payload_size - sizeof(short) - sizeof(char) - sizeof(char);
    }
    skb_set_network_header(skb_p, 0);
    iph2_p   = ip_hdr(skb_p);
    skb_set_transport_header(skb_p, iph2_p->ihl << 2);


    if ((iph2_p->version  != 4 ) && (iph2_p->version  != 6)) {
        pr_info("\nGTPURH: Decapsulated packet dropped because not IPvx protocol see all GTPU packet here:\n");
        _gtpurh_print_hex_octets((unsigned char*)iph_p, ntohs(iph_p->tot_len));
        return NF_DROP;
    }
//#if 0
    if ((skb_p->mark == 0) || (gtp_payload_size != ntohs(iph2_p->tot_len))) {
        pr_info("\nGTPURH: Decapsulated packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
                iph2_p->saddr  & 0xFF,
                (iph2_p->saddr & 0x0000FF00) >> 8,
                (iph2_p->saddr & 0x00FF0000) >> 16,
                iph2_p->saddr >> 24,
                iph2_p->daddr  & 0xFF,
                (iph2_p->daddr & 0x0000FF00) >> 8,
                (iph2_p->daddr & 0x00FF0000) >> 16,
                iph2_p->daddr >> 24,
                iph2_p->protocol,
                ntohs(iph2_p->tot_len),
                skb_p->mark,
                ntohs(iph_p->frag_off) & 0x1FFFFFFF,
                ntohs(iph_p->frag_off) >> 13);

        if (gtp_payload_size != ntohs(iph2_p->tot_len)) {
            pr_info("GTPURH: Mismatch in lengths GTPU length: %u -> %u, IP length %u\n",
                    ntohs(gtpuh_p->length),
                    gtp_payload_size,
                    ntohs(iph2_p->tot_len));

            _gtpurh_print_hex_octets((unsigned char*)iph_p, ntohs(iph_p->tot_len));
            return NF_DROP;
        }
    }
//#endif

    /* Route the packet */
#if defined(ROUTE_PACKET)
    _gtpurh_route_packet(skb_p, tgi_pP);
    return NF_DROP;
#else
    {
        int            err = 0;
        struct rtable *rt    = NULL;
        int            daddr = iph2_p->daddr;
        struct flowi   fl    = {
                           .u = {
                               .ip4 = {
                                   .daddr        = daddr,
                                   .flowi4_tos   = RT_TOS(iph2_p->tos),
                                   .flowi4_scope = RT_SCOPE_UNIVERSE,
                               }
                           }
                       };
        rt = ip_route_output_key(&init_net, &fl.u.ip4);
        if (rt == NULL) {
            pr_info("GTPURH: Failed to route packet to dst 0x%x. Error: (%d)", fl.u.ip4.daddr, err);
            return NF_DROP;
        }
#if 0
        if (rt->dst.dev) {
            pr_info("GTPURH: dst dev name %s\n", rt->dst.dev->name);
        } else {
            pr_info("GTPURH: dst dev NULL\n");
        }
#endif
        skb_p->priority = rt_tos2priority(iph2_p->tos);
        skb_p->pkt_type = PACKET_OTHERHOST;
        skb_dst_drop(skb_p);
        skb_dst_set(skb_p, dst_clone(&rt->dst));
        skb_p->dev      = skb_dst(skb_p)->dev;

#if defined(NEW_SKB)
        new_skb_p = alloc_skb(LL_MAX_HEADER + ntohs(iph2_p->tot_len), GFP_ATOMIC);
        if (new_skb_p == NULL) {
            return NF_DROP;
        }


        skb_reserve(new_skb_p, LL_MAX_HEADER);
        new_skb_p->protocol = skb_p->protocol;

        skb_reset_network_header(new_skb_p);
        new_ip_p = (void *)skb_put(new_skb_p, iph2_p->ihl << 2);
        skb_reset_transport_header(new_skb_p);
        skb_put(new_skb_p, ntohs(iph2_p->tot_len) - (iph2_p->ihl << 2));
        memcpy(new_ip_p, iph2_p, ntohs(iph2_p->tot_len));

        new_skb_p->mark = ntohl(gtpuh_p->tunid);
          //new_skb_p->mark     = skb_p->mark;

        /* ip_route_me_harder expects skb->dst to be set */
        skb_dst_set(new_skb_p, dst_clone(skb_dst(skb_p)));

        /*if (ip_route_me_harder(new_skb_p, RTN_UNSPEC) < 0) {
            pr_info("GTPURH: cannot route harder dest 0x%x\n", daddr);
            goto free_skb;
        }*/

        new_ip_p->ttl        = new_ip_p->ttl -1; // ip4_dst_hoplimit(skb_dst(new_skb_p));
        new_skb_p->ip_summed = CHECKSUM_NONE;

        if (new_skb_p->len > dst_mtu(skb_dst(new_skb_p))) {
            pr_info("GTPURH: bad length\n");
            goto free_skb;
        }

        nf_ct_attach(new_skb_p, skb_p);
        pr_info("GTPURH: ip_local_out %s/%s dev %s src %u.%u.%u.%u dst  %u.%u.%u.%u \n",
                _gtpurh_nf_inet_hook_2_string(par_pP->hooknum),
                gtpurh_tg_reg[0].table,
                (new_skb_p->dev == NULL) ? "NULL" : new_skb_p->dev->name,
                NIPADDR(new_ip_p->saddr),
                NIPADDR(new_ip_p->daddr));
        ip_local_out(new_skb_p);
        return NF_DROP;
free_skb:
        pr_info("GTPURH: Dropped skb ip_local_out %s/%s\n",
                _gtpurh_nf_inet_hook_2_string(par_pP->hooknum),
                gtpurh_tg_reg[0].table);
        kfree_skb(new_skb_p);
        return NF_DROP;

#else

        return NF_ACCEPT;
free_skb:
        pr_info("GTPURH: Dropped skb\n");
        kfree_skb(skb_p);
        return NF_DROP;
#endif
    }
#endif
}


#if defined(WITH_IPV6)
//-----------------------------------------------------------------------------
static unsigned int
gtpurh_tg6(struct sk_buff *skb_pP, const struct xt_action_param *par_pP)
//-----------------------------------------------------------------------------
{
    const struct xt_gtpurh_target_info *tgi_p = par_pP->targinfo;
    int result = NF_DROP;

    if (tgi_p == NULL) {
        return result;
    }

    if (tgi_p->action == PARAM_GTPURH_ACTION_REM) {
        result = NF_DROP; // TO DO
    }
    return result;
}
#endif

//-----------------------------------------------------------------------------
static unsigned int
gtpurh_tg4(struct sk_buff *skb_pP, const struct xt_action_param *par_pP)
//-----------------------------------------------------------------------------
{
    const struct iphdr                 *iph_p = ip_hdr(skb_pP);
    const struct rtable                *rt_p  = skb_rtable(skb_pP);
    const struct xt_gtpurh_target_info *tgi_p = par_pP->targinfo;

    if (tgi_p == NULL) {
        return NF_ACCEPT;
    }

    if ((tgi_p->raddr != iph_p->saddr) || (tgi_p->laddr != iph_p->daddr)) {
        pr_info("GTPURH: Not processed because of not matching saddr %d.%d.%d.%d raddr %d.%d.%d.%d  laddr %d.%d.%d.%d daddr %d.%d.%d.%d\n",
                NIPADDR(tgi_p->raddr),
                NIPADDR(iph_p->saddr),
                NIPADDR(tgi_p->laddr),
                NIPADDR(iph_p->daddr));
        return NF_ACCEPT;
    }

    if (ip_hdrlen(skb_pP) != sizeof(struct iphdr)) {
        pr_info("GTPURH: Dropped because IP options\n");
        return NF_DROP;
    }

    // Drop fragments
    if (iph_p->frag_off & htons(IP_OFFSET)) {
        pr_info("GTPURH: Dropped because is fragment\n");
        return NF_DROP;
    }

    if (tgi_p->action == PARAM_GTPURH_ACTION_REM) {
        return _gtpurh_tg4_rem(skb_pP, par_pP);
    }
    return NF_DROP;
}


//-----------------------------------------------------------------------------
static int
__init gtpurh_tg_init(void) {
//-----------------------------------------------------------------------------

    pr_info("GTPURH: Initializing module (KVersion: %d)\n", KVERSION);
    pr_info("GTPURH: Copyright Polaris Networks 2010-2011\n");
    pr_info("GTPURH: Modified by EURECOM Lionel GAUTHIER 2014\n");
#if defined(WITH_IPV6)
    pr_info("GTPURH: IPv4/IPv6 enabled\n");
#else
    pr_info("GTPURH: IPv4 only enabled\n");
#endif
    return xt_register_targets(gtpurh_tg_reg, ARRAY_SIZE(gtpurh_tg_reg));
}

//-----------------------------------------------------------------------------
static void
__exit gtpurh_tg_exit(void) {
//-----------------------------------------------------------------------------

    xt_unregister_targets(gtpurh_tg_reg, ARRAY_SIZE(gtpurh_tg_reg));
    pr_info("GTPURH: Unloading module\n");
}

module_init(gtpurh_tg_init);
module_exit(gtpurh_tg_exit);
MODULE_ALIAS("ipt6_GTPURH");
MODULE_ALIAS("ipt_GTPURH");

