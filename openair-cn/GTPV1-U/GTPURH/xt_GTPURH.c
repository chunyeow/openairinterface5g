
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
#include <linux/route.h> 
#include <net/checksum.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/ip.h>
#include <net/route.h> 
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#if 0
#include <net/netfilter/ipv4/nf_defrag_ipv4.h>
#endif

#include "xt_GTPURH.h"

#if !(defined KVERSION)
#error "Kernel version is not defined!!!! Exiting."
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradip Biswas <pradip_biswas@polarisnetworks.net>");
MODULE_DESCRIPTION("GTPu Data Path extension on netfilter");


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

static bool _gtpurh_route_packet(struct sk_buff *skb, const struct xt_gtpurh_target_info *info)
{
    int err = 0; 
    struct rtable *rt = NULL;
    struct iphdr *iph = ip_hdr(skb); 
    int daddr = iph->daddr;
    struct flowi fl = { 
        .u = {
            .ip4 = {
                .daddr        = daddr,
                .flowi4_tos   = RT_TOS(iph->tos),
                .flowi4_scope = RT_SCOPE_UNIVERSE,
            } 
        } 
    }; 

#if 0
    pr_info("GTPURH(%d): Routing packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Len: %d Mark: %u\n",
            info->action,
            iph->saddr  & 0xFF,
            (iph->saddr & 0x0000FF00) >> 8,
            (iph->saddr & 0x00FF0000) >> 16,
            iph->saddr >> 24,
            iph->daddr  & 0xFF,
            (iph->daddr & 0x0000FF00) >> 8,
            (iph->daddr & 0x00FF0000) >> 16,
            iph->daddr >> 24,
            iph->protocol,
            ntohs(iph->tot_len),
            skb->mark);
#endif

    rt = ip_route_output_key(&init_net, &fl.u.ip4);
    if (err != 0) 
    { 
        pr_info("GTPURH: Failed to route packet to dst 0x%x. Error: (%d)", fl.u.ip4.daddr, err);
        return GTPURH_FAILURE;
    } 

#if 0
    if (rt->dst.dev)
    {
        pr_info("GTPURH: dst dev name %s\n", rt->dst.dev->name);
    }
    else
    {
        pr_info("GTPURH: dst dev NULL\n");
    }
#endif

    //if (info->action == PARAM_GTPURH_ACTION_ADD) //LG was commented
    {
        skb_dst_drop(skb);
        skb_dst_set(skb, &rt->dst);
        skb->dev      = skb_dst(skb)->dev;
    }

    skb->protocol = htons(ETH_P_IP); 

    /* Send the GTPu message out...gggH */
    err = dst_output(skb);

    if (err == 0)
    {
        return GTPURH_SUCCESS;
    }
    else
    {
        return GTPURH_FAILURE;
    }
}

static unsigned int
_gtpurh_target_rem(struct sk_buff *orig_skb, const struct xt_gtpurh_target_info *tgi)
{
    struct iphdr   *iph   = ip_hdr(orig_skb);
    struct iphdr   *iph2  = NULL;
    struct gtpuhdr *gtpuh = NULL;
    struct sk_buff *skb   = NULL;
    uint16_t        gtp_payload_size = 0;

    /* Create a new copy of the original skb...can't avoid :-( LG: WHY???*/
#if defined(ROUTE_PACKET)
    skb = skb_copy(orig_skb, GFP_ATOMIC);
    if (skb == NULL)
    {
        return NF_ACCEPT;
    }
    skb->skb_iif  = orig_skb->skb_iif;
    skb->protocol = orig_skb->protocol;
#else
    skb = orig_skb;
#endif

    /* Remove IP header */
    skb_pull(skb, (iph->ihl << 2));

    /* Remove UDP header */
    gtpuh = (struct gtpuhdr*)skb_pull(skb, sizeof(struct udphdr));
    gtp_payload_size = ntohs(gtpuh->length);

    skb->mark = ntohl(gtpuh->tunid);
    /* Remove GTPu header */
    skb_pull(skb, sizeof(struct gtpuhdr));

    /* If additional fields are present in header, remove them also */
    if (gtpuh->flags & GTPURH_ANY_EXT_HDR_BIT)
    {
        skb_pull(skb, sizeof(short) + sizeof(char) + sizeof(char)); /* #Seq, #N-PDU, #ExtHdr Type */
        gtp_payload_size = gtp_payload_size - sizeof(short) - sizeof(char) - sizeof(char);
    }
    //skb->mac_len = 0;
    //skb_set_mac_header(skb, 0);
    skb_set_network_header(skb, 0);
    iph2   = ip_hdr(skb);
    skb_set_transport_header(skb, iph2->ihl << 2);

//#if 0
    if ((skb->mark == 0) || (gtp_payload_size != ntohs(iph2->tot_len))) {
        pr_info("GTPURH(%d): Decapsulating packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
                tgi->action,
                iph2->saddr  & 0xFF,
                (iph2->saddr & 0x0000FF00) >> 8,
                (iph2->saddr & 0x00FF0000) >> 16,
                iph2->saddr >> 24,
                iph2->daddr  & 0xFF,
                (iph2->daddr & 0x0000FF00) >> 8,
                (iph2->daddr & 0x00FF0000) >> 16,
                iph2->daddr >> 24,
                iph2->protocol,
                ntohs(iph2->tot_len),
                skb->mark,
                ntohs(iph->frag_off) & 0x1FFFFFFF,
                ntohs(iph->frag_off) >> 13);

        if (gtp_payload_size != ntohs(iph2->tot_len)) {
            pr_info("\nGTPURH(%d): Mismatch in lengths GTPU length: %u -> %u, IP length %u\n\n",
                    ntohs(gtpuh->length), gtp_payload_size, ntohs(iph->tot_len));
        }
    }
//#endif

    /* Route the packet */
#if defined(ROUTE_PACKET)
    _gtpurh_route_packet(skb, tgi);
    return NF_DROP;
#else
    return NF_ACCEPT;
#endif
}


static unsigned int
xt_gtpurh_target(struct sk_buff *skb, const struct xt_action_param *par)
{
    const struct xt_gtpurh_target_info *tgi = par->targinfo;
    int result = NF_ACCEPT;

    if (tgi == NULL)
    {
        return result;
    }

    if (tgi->action == PARAM_GTPURH_ACTION_REM)
    {
        result = _gtpurh_target_rem(skb, tgi);
    }
    return result;
}

static struct xt_target xt_gtpurh_reg __read_mostly =
{
    .name           = "GTPURH",
    .revision       = 0,
    .family         = AF_INET,
    .hooks          = (1 << NF_INET_PRE_ROUTING),
    .table          = "raw",
    .target         = xt_gtpurh_target,
    .targetsize     = sizeof(struct xt_gtpurh_target_info),
    .me             = THIS_MODULE,
};

static int __init xt_gtpurh_init(void)
{
    pr_info("GTPURH: Initializing module (KVersion: %d)\n", KVERSION);
    pr_info("GTPURH: Copyright Polaris Networks 2010-2011\n");
    return xt_register_target(&xt_gtpurh_reg);
}

static void __exit xt_gtpurh_exit(void)
{
    xt_unregister_target(&xt_gtpurh_reg);
    pr_info("GTPURH: Unloading module\n");
}

module_init(xt_gtpurh_init);
module_exit(xt_gtpurh_exit);

