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
#include <linux/time.h>
#include <net/checksum.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/route.h> 
#include <net/addrconf.h>
#include <net/ip6_checksum.h>
#include <net/ip6_route.h>
#include <net/ipv6.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#    include <linux/netfilter_bridge.h>
#endif
#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
#    define WITH_IPV6 1
#endif
#include "xt_GTPUAH.h"
#if !(defined KVERSION)
#    error "Kernel version is not defined!!!! Exiting."
#endif
#define INT_MODULE_PARM(n, v) static int n = v; module_param(n, int, 0444)
//-----------------------------------------------------------------------------
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradip Biswas <pradip_biswas@polarisnetworks.net>");
MODULE_DESCRIPTION("GTPu Data Path extension on netfilter");
//-----------------------------------------------------------------------------
static char*        _gtpuah_nf_inet_hook_2_string(int nf_inet_hookP);
static void         _gtpuah_print_hex_octets(unsigned char* data_pP, unsigned short sizeP);
static void         _gtpuah_tg4_add(struct sk_buff *old_skb_pP, const struct xt_action_param *par_pP);
#ifdef WITH_IPV6
static void         _gtpuah_tg6_add(struct sk_buff *old_skb_pP, const struct xt_action_param *par_pP);
static unsigned int gtpuah_tg6(struct sk_buff *skb_pP, const struct xt_action_param *par_pP);
#endif
static unsigned int gtpuah_tg4(struct sk_buff *skb_pP, const struct xt_action_param *par_pP);
static int          __init gtpuah_tg_init(void);
static void         __exit gtpuah_tg_exit(void);
//-----------------------------------------------------------------------------
static struct xt_target gtpuah_tg_reg[] __read_mostly = {
                {
                                .name           = "GTPUAH",
                                .revision       = 0,
                                .family         = NFPROTO_IPV4,
                                .hooks          = (1 << NF_INET_POST_ROUTING) |
                                                  (1 << NF_INET_LOCAL_IN),
                                .table          = "mangle",
                                .target         = gtpuah_tg4,
                                .targetsize     = sizeof(struct xt_gtpuah_target_info),
                                .me             = THIS_MODULE,
                },
#ifdef WITH_IPV6
                {
                                .name           = "GTPUAH",
                                .revision       = 0,
                                .family         = NFPROTO_IPV6,
                                .hooks          = (1 << NF_INET_POST_ROUTING) |
                                                  (1 << NF_INET_LOCAL_IN),
                                .table          = "mangle",
                                .target         = gtpuah_tg6,
                                .targetsize     = sizeof(struct xt_gtpuah_target_info),
                                .me             = THIS_MODULE,
                },
#endif
};

struct gtpuhdr {
    char      flags;
    char      msgtype;
    u_int16_t length;
    u_int32_t tunid;
};

//-----------------------------------------------------------------------------
#define GTPU_HDR_PNBIT 1
#define GTPU_HDR_SBIT 1 << 1
#define GTPU_HDR_EBIT 1 << 2
#define GTPU_ANY_EXT_HDR_BIT (GTPU_HDR_PNBIT | GTPU_HDR_SBIT | GTPU_HDR_EBIT)

#define GTPU_FAILURE 1
#define GTPU_SUCCESS !GTPU_FAILURE
#define GTPUAH_2_PRINT_BUFFER_LEN 8192

#define IP_MORE_FRAGMENTS 0x2000
#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)
//-----------------------------------------------------------------------------
static char _gtpuah_print_buffer[GTPUAH_2_PRINT_BUFFER_LEN];
INT_MODULE_PARM(tunnel_local, 0);
MODULE_PARM_DESC(tunnel_local, "Act as a boolean, tels if the S1U tunnel(s) are both start/end local");
INT_MODULE_PARM(gtpu_port, 2152);
MODULE_PARM_DESC(gtpu_port, "UDP port number for S1U interface (eNB and S-GW sides)");
INT_MODULE_PARM(mtu, 1564);
MODULE_PARM_DESC(mtu, "MTU of the S1U IP interface");
//-----------------------------------------------------------------------------
static char*
_gtpuah_nf_inet_hook_2_string(int nf_inet_hookP) {
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
void
_gtpuah_print_hex_octets(unsigned char* data_pP, unsigned short sizeP) {
//-----------------------------------------------------------------------------

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

  buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker,"%s------+-------------------------------------------------+\n",timeofday);
  buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker,"%s      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n",timeofday);
  buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker,"%s------+-------------------------------------------------+\n",timeofday);
  pr_info("%s",_gtpuah_print_buffer);buffer_marker = 0;
  for (octet_index = 0; octet_index < sizeP; octet_index++) {
    if ((octet_index % 16) == 0){
      if (octet_index != 0) {
          buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
          pr_info("%s",_gtpuah_print_buffer);buffer_marker = 0;
      }
      buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker, "%s %04ld |",timeofday, octet_index);
    }
    /*
     * Print every single octet in hexadecimal form
     */
    buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker, " %02x", data_pP[octet_index]);
    /*
     * Align newline and pipes according to the octets in groups of 2
     */
  }

  /*
   * Append enough spaces and put final pipe
   */
  for (aindex = octet_index; aindex < 16; ++aindex)
    buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker, "   ");
    //SGI_IF_DEBUG("   ");
  buffer_marker+=snprintf(&_gtpuah_print_buffer[buffer_marker], GTPUAH_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
  pr_info("%s",_gtpuah_print_buffer);
}

#ifdef WITH_IPV6
//-----------------------------------------------------------------------------
static void
_gtpuah_tg6_add(struct sk_buff *old_skb_pP, const struct xt_action_param *par_pP) {
//-----------------------------------------------------------------------------
}
#endif

//-----------------------------------------------------------------------------
static void
_gtpuah_tg4_add(struct sk_buff *old_skb_pP, const struct xt_action_param *par_pP) {
//-----------------------------------------------------------------------------
    struct rtable  *rt              = NULL;
    struct ethhdr  *ethhdr_p        = NULL;
    struct iphdr   *old_iph_p       = ip_hdr(old_skb_pP);
    struct iphdr   *new_iph_p       = NULL;
    struct iphdr   *tmp_iph_p       = NULL;
    struct udphdr  *udph_p          = NULL;
    struct gtpuhdr *gtpuh_p         = NULL;
    struct sk_buff *new_skb_p       = NULL;
    uint16_t        headroom_reqd   =  ETH_HLEN + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct gtpuhdr);
    uint16_t        orig_iplen = 0, udp_len = 0, ip_len = 0;
    int             flags = 0, offset = 0;
    unsigned int    addr_type       = RTN_UNSPEC;


    if (ip_is_fragment(ip_hdr(old_skb_pP))) {
        pr_info("GTPUAH: IP fragment, dropped\n");
        return;
    }
    if (skb_linearize(old_skb_pP) < 0) {
        pr_info("GTPUAH: skb no linearize\n");
        return;
    }
    if (old_skb_pP->mark == 0) {
        pr_info("GTPUAH: _gtpuah_target_add force info_pP mark %u to skb_pP mark %u\n",
                old_skb_pP->mark,
                ((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->rtun);
        old_skb_pP->mark = ((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->rtun;
    }
    /* Keep the length of the source IP packet */
    orig_iplen = ntohs(old_iph_p->tot_len);
    offset = ntohs(old_iph_p->frag_off);
    flags  = offset & ~IP_OFFSET;


    /* Create a new copy of the original skb...can't avoid :-( */
    if ((orig_iplen + headroom_reqd) <= mtu) {

        new_skb_p = alloc_skb(headroom_reqd + orig_iplen, GFP_ATOMIC);
        if (new_skb_p == NULL) {
            pr_info("GTPUAH: alloc_skb returned NULL\n");
            return;
        }
        if (skb_linearize(new_skb_p) < 0) {
            pr_info("GTPUAH: skb no linearize\n");
            goto free_new_skb;
        }
        skb_reserve(new_skb_p, headroom_reqd + orig_iplen);
        tmp_iph_p = (void *)skb_push(new_skb_p, orig_iplen);
        memcpy(tmp_iph_p, old_iph_p, orig_iplen);

        /* Add GTPu header */
        gtpuh_p          = (struct gtpuhdr*)skb_push(new_skb_p, sizeof(struct gtpuhdr));
        gtpuh_p->flags   = 0x30; /* v1 and Protocol-type=GTP */
        gtpuh_p->msgtype = 0xff; /* T-PDU */
        gtpuh_p->length  = htons(orig_iplen);
        gtpuh_p->tunid   = htonl(old_skb_pP->mark);

        /* Add UDP header */
        udp_len        = sizeof(struct udphdr) + sizeof(struct gtpuhdr) + orig_iplen;
        udph_p         = (struct udphdr*)skb_push(new_skb_p, sizeof(struct udphdr));
        udph_p->source = htons(gtpu_port);
        udph_p->dest   = htons(gtpu_port);
        udph_p->len    = htons(udp_len);
        udph_p->check  = 0;
        udph_p->check  = csum_tcpudp_magic(((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->laddr,
                                         ((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->raddr,
                                         udp_len,
                                         IPPROTO_UDP,
                                         csum_partial((char*)udph_p, udp_len, 0));
        skb_set_transport_header(new_skb_p, 0);

        /* Add IP header */
        ip_len = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct gtpuhdr) + orig_iplen;
        new_iph_p = (struct iphdr*)skb_push(new_skb_p, sizeof(struct iphdr));
        new_iph_p->ihl      = 5;
        new_iph_p->version  = 4;
        new_iph_p->tos      = 0;
        new_iph_p->tot_len  = htons(ip_len);
        new_iph_p->id       = (uint16_t)(((uint64_t)new_skb_p) >> 8);
        new_iph_p->frag_off = 0;
        new_iph_p->ttl      = 64;
        new_iph_p->protocol = IPPROTO_UDP;
        new_iph_p->saddr    = ((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->laddr;
        // !!!!!!!! LG TEST !!!!!!!!!!
        //new_iph_p->saddr    = old_iph_p->saddr;
        new_iph_p->daddr    = ((const struct xt_gtpuah_target_info *)(par_pP->targinfo))->raddr;
        new_iph_p->check    = 0;
        new_iph_p->check    = ip_fast_csum((unsigned char *)new_iph_p, new_iph_p->ihl);
        skb_set_network_header(new_skb_p, 0);

        skb_set_inner_network_header(new_skb_p, -ETH_HLEN);
        skb_set_inner_transport_header(new_skb_p, -ETH_HLEN);



        // CHECKSUM_NONE, CHECKSUM_UNNECESSARY, CHECKSUM_COMPLETE, CHECKSUM_PARTIAL
        new_skb_p->ip_summed = CHECKSUM_NONE;
        new_skb_p->mark      = old_skb_pP->mark;

        switch (par_pP->hooknum) {
            case NF_INET_POST_ROUTING: {
                new_skb_p->pkt_type = PACKET_OTHERHOST; // PACKET_OUTGOING
#ifdef CONFIG_BRIDGE_NETFILTER
                if (new_skb_p->nf_bridge != NULL && new_skb_p->nf_bridge->mask & BRNF_BRIDGED) {
                    addr_type = RTN_LOCAL;
                    new_skb_p->pkt_type =PACKET_HOST;
                }
#endif
                if (tunnel_local == 0) {
                    struct flowi   fl    = {
                        .u = {
                            .ip4 = {
                                .daddr        = new_iph_p->daddr,
                                .flowi4_tos   = RT_TOS(new_iph_p->tos),
                                .flowi4_scope = RT_SCOPE_UNIVERSE,
                            }
                        }
                    };
                    pr_info("GTPUAH: PACKET -> NF_HOOK NF_INET_POST_ROUTING/%s encapsulated src: %u.%u.%u.%u dst: %u.%u.%u.%u\n",
                            gtpuah_tg_reg[0].table,
                            NIPADDR(old_iph_p->saddr),
                            NIPADDR(old_iph_p->daddr));


                    rt = ip_route_output_key(&init_net, &fl.u.ip4);
                    if (rt == NULL) {
                        pr_info("GTPURH: Failed to route packet to dst 0x%x.\n", fl.u.ip4.daddr);
                        goto free_new_skb;
                    }
                    new_skb_p->priority = rt_tos2priority(new_iph_p->tos);
                    skb_dst_drop(new_skb_p);
                    if (rt->dst.dev) {
                        pr_info("GTPURH: dst dev name %s\n", rt->dst.dev->name);
                        skb_dst_set(new_skb_p, dst_clone(&rt->dst));
                        new_skb_p->dev      = skb_dst(new_skb_p)->dev;
                        if (new_skb_p->len > dst_mtu(skb_dst(new_skb_p))) {
                            goto free_new_skb;
                        }
                        nf_ct_attach(new_skb_p, old_skb_pP);
                        ip_local_out(new_skb_p);
                    } else {
                        pr_info("GTPURH: rt->dst.dev == NULL\n");
                        goto free_new_skb;
                    }
                } else { // (tunnel_local)

                    new_skb_p->pkt_type = PACKET_HOST;
                    new_skb_p->priority = rt_tos2priority(new_iph_p->tos);
                    new_skb_p->protocol = htons(ETH_P_IP);

                    // fake mac header
                    ethhdr_p = (struct ethhdr*)skb_push(new_skb_p, ETH_HLEN);
                    skb_set_mac_header(new_skb_p, 0);
                    memset(ethhdr_p, 0, ETH_HLEN);
                    ethhdr_p->h_proto = ntohs(ETH_P_IP);

                    //_gtpuah_print_hex_octets(new_iph_p, ip_len);
                    //ip_local_deliver_fn_ptr(new_skb_p);

                    new_skb_p->ip_summed = CHECKSUM_NONE;
                    skb_dst_drop(new_skb_p);
                    nf_reset(new_skb_p);
                    /*pr_info("GTPUAH(tun): PACKET -> NF_HOOK NF_INET_POST_ROUTING/%s encapsulated src: %u.%u.%u.%u dst: %u.%u.%u.%u\n",
                            gtpuah_tg_reg[0].table,
                            NIPADDR(old_iph_p->saddr),
                            NIPADDR(old_iph_p->daddr));*/
                    if ( dev_forward_skb(old_skb_pP->dev, new_skb_p) != NET_RX_SUCCESS) {
                        pr_info("GTPUAH(tun): dev_forward_skb failed!!!\n");
                    }
                    return;
                }
            }
            break;

            default:
                pr_info("GTPUAH: NF_HOOK %u not processed\n", par_pP->hooknum);
                goto free_new_skb;
        }
        return;
    } else {
        pr_info("GTPUAH: PACKET DROPPED because of mtu %u < (%u + %u)\n",
                mtu, orig_iplen, headroom_reqd);
    }
free_new_skb:
    pr_info("GTPUAH: PACKET DROPPED\n");
    kfree_skb(new_skb_p);
    return ;
}

#ifdef WITH_IPV6
//-----------------------------------------------------------------------------
static unsigned int
gtpuah_tg6(struct sk_buff *skb_pP, const struct xt_action_param *par_pP) {
//-----------------------------------------------------------------------------

    const struct xt_gtpuah_target_info *tgi_p = par_pP->targinfo;

    if (tgi_p == NULL) {
        return NF_ACCEPT;
    }

    if (tgi_p->action == PARAM_GTPUAH_ACTION_ADD) {
        _gtpuah_tg6_add(skb_pP, par_pP);
        return NF_DROP; // TODO
    }
    return NF_ACCEPT;
}
#endif

//-----------------------------------------------------------------------------
static unsigned int
gtpuah_tg4(struct sk_buff *skb_pP, const struct xt_action_param *par_pP) {
//-----------------------------------------------------------------------------
    const struct iphdr                 *iph_p = ip_hdr(skb_pP);
    const struct xt_gtpuah_target_info *tgi_p = par_pP->targinfo;
    if (tgi_p == NULL) {
        return NF_ACCEPT;
    }

    if (tgi_p->action == PARAM_GTPUAH_ACTION_ADD) {
        _gtpuah_tg4_add(skb_pP, par_pP);
        return NF_DROP;
    }
    return NF_ACCEPT;
}

//-----------------------------------------------------------------------------
static int
__init gtpuah_tg_init(void) {
//-----------------------------------------------------------------------------
    pr_info("GTPUAH: Initializing module (KVersion: %d)\n", KVERSION);
    pr_info("GTPUAH: Copyright Polaris Networks 2010-2011\n");
    pr_info("GTPUAH: Modified by EURECOM Lionel GAUTHIER 2014\n");
#if defined(WITH_IPV6)
    pr_info("GTPURH: IPv4/IPv6 enabled\n");
#else
    pr_info("GTPURH: IPv4 only enabled\n");
#endif
    return xt_register_targets(gtpuah_tg_reg, ARRAY_SIZE(gtpuah_tg_reg));
}

//-----------------------------------------------------------------------------
static void
__exit gtpuah_tg_exit(void) {
    //-----------------------------------------------------------------------------
    xt_unregister_targets(gtpuah_tg_reg, ARRAY_SIZE(gtpuah_tg_reg));
    pr_info("GTPUAH: Unloading module\n");
}


module_init(gtpuah_tg_init);
module_exit(gtpuah_tg_exit);
MODULE_ALIAS("ipt6_GTPUAH");
MODULE_ALIAS("ipt_GTPUAH");

