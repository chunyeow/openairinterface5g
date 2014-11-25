
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
#include <linux/time.h>
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

#include "xt_GTPUAH.h"

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

#define MTU            1500
#define GTPU_HDR_PNBIT 1
#define GTPU_HDR_SBIT 1 << 1
#define GTPU_HDR_EBIT 1 << 2
#define GTPU_ANY_EXT_HDR_BIT (GTPU_HDR_PNBIT | GTPU_HDR_SBIT | GTPU_HDR_EBIT)

#define GTPU_FAILURE 1
#define GTPU_SUCCESS !GTPU_FAILURE

#define GTPU_PORT 2152

#define IP_MORE_FRAGMENTS 0x2000


static bool _gtpuah_route_packet(struct sk_buff *skb, const struct xt_gtpuah_target_info *info)
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

#if 1
    int                         flags, offset;

    offset = ntohs(iph->frag_off);
    flags  = offset & ~IP_OFFSET;
    offset &= IP_OFFSET; /* offset is in 8-byte chunks */
    offset <<= 3;

    pr_info("GTPUAH(%d): Routing %s packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Len: %05u, Id %04X, Offset %u, Flags %04X, Mark: %u\n",
            info->action,
            (ip_is_fragment(iph) == 0) ? "":"fragmented",
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
            ntohs(iph->id),
            offset,
            flags,
            skb->mark);
#endif

    rt = ip_route_output_key(&init_net, &fl.u.ip4);
    if (err != 0) 
    { 
        pr_info("GTPU: Failed to route packet to dst 0x%x. Error: (%d)", fl.u.ip4.daddr, err);
        return GTPU_FAILURE; 
    } 

#if 0
    if (rt->dst.dev)
    {
        pr_info("GTPU: dst dev name %s\n", rt->dst.dev->name);
    }
    else
    {
        pr_info("GTPU: dst dev NULL\n"); 
    }
#endif

    //if (info->action == PARAM_GTPUAH_ACTION_ADD) //LG was commented
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
        return GTPU_SUCCESS;
    }
    else
    {
        return GTPU_FAILURE;
    }
}

static unsigned int
_gtpuah_target_add(struct sk_buff *skb, const struct xt_gtpuah_target_info *tgi)
{
    struct iphdr   *iph           = ip_hdr(skb);
    struct iphdr   *iph2          = NULL;
    struct udphdr  *udph          = NULL;
    struct gtpuhdr *gtpuh         = NULL;
    struct sk_buff *new_skb       = NULL;
    struct sk_buff *new_skb2      = NULL;
    uint16_t        headroom_reqd =  sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct gtpuhdr);
    uint16_t        orig_iplen = 0, udp_len = 0, ip_len = 0;
    int16_t         remaining_bytes_to_send = 0, remaining_payload_to_send = 0,
            payload_to_send = 0, bytes_to_remove = 0, bytes_to_send = 0;
    int             flags = 0, offset = 0, offset2 = 0;

    if (skb->mark == 0) {
        pr_info("GTPUAH: _gtpuah_target_add force skb mark %u to tgi mark %u", skb->mark, tgi->rtun);
        skb->mark = tgi->rtun;
    }
    /* Keep the length of the source IP packet */
    orig_iplen = ntohs(iph->tot_len);
    offset = ntohs(iph->frag_off);
    flags  = offset & ~IP_OFFSET;


    /* Create a new copy of the original skb...can't avoid :-( */
    if (((orig_iplen + headroom_reqd) <= MTU) || (flags & IP_DF)) {

        if (flags & IP_DF) {
            pr_info("GTPUAH: DONT FRAGMENT id %04X", ntohs(iph->id));
        }

        new_skb = skb_copy_expand(skb, headroom_reqd + skb_headroom(skb), skb_tailroom(skb), GFP_ATOMIC);
        if (new_skb == NULL)
        {
            return NF_ACCEPT;
        }
        /* Add GTPu header */
        gtpuh          = (struct gtpuhdr*)skb_push(new_skb, sizeof(struct gtpuhdr));
        gtpuh->flags   = 0x30; /* v1 and Protocol-type=GTP */
        gtpuh->msgtype = 0xff; /* T-PDU */
        gtpuh->length  = htons(orig_iplen);
        gtpuh->tunid   = htonl(skb->mark);

        /* Add UDP header */
        udp_len      = sizeof(struct udphdr) + sizeof(struct gtpuhdr) + orig_iplen;
        udph         = (struct udphdr*)skb_push(new_skb, sizeof(struct udphdr));
        udph->source = htons(GTPU_PORT);
        udph->dest   = htons(GTPU_PORT);
        udph->len    = htons(udp_len);
        udph->check  = 0;
        udph->check  = csum_tcpudp_magic(tgi->laddr,
                tgi->raddr,
                udp_len,
                IPPROTO_UDP,
                csum_partial((char*)udph, udp_len, 0));
        skb_set_transport_header(new_skb, 0);

        /* Add IP header */
        ip_len = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct gtpuhdr) + orig_iplen;
        iph = (struct iphdr*)skb_push(new_skb, sizeof(struct iphdr));
        iph->ihl      = 5;
        iph->version  = 4;
        iph->tos      = 0;
        iph->tot_len  = htons(ip_len);
        iph->id       = (uint16_t)(((uint64_t)new_skb) >> 8);
        iph->frag_off = 0;
        iph->ttl      = 64;
        iph->protocol = IPPROTO_UDP;
        iph->saddr    = (tgi->laddr);
        iph->daddr    = (tgi->raddr);
        iph->check    = 0;
        iph->check    = ip_fast_csum((unsigned char *)iph, iph->ihl);
        skb_set_network_header(new_skb, 0);

        /* Route the packet */
        if (_gtpuah_route_packet(new_skb, tgi) == GTPU_SUCCESS)
        {
            /* Succeeded. Drop the original packet */
            return NF_DROP;
        }
        else
        {
            kfree_skb(new_skb);
            return NF_ACCEPT; /* What should we do here ??? ACCEPT seems to be the best option */
        }
    } else {
        new_skb = skb_copy_expand(skb, headroom_reqd + skb_headroom(skb), 0, GFP_ATOMIC);
        if (new_skb == NULL)
        {
            return NF_ACCEPT;
        }

        // must segment if added headers (IP, UDP, GTP) + original data + original headers > MTU
        pr_info("GTPUAH: Fragmentation Id %04X payload size %u (IP Head inc), total ip packet size %u",
                htons((uint16_t)(((uint64_t)new_skb) >> 8)),
                MTU - headroom_reqd,
                MTU);


        gtpuh          = (struct gtpuhdr*)skb_push(new_skb, sizeof(struct gtpuhdr));
        gtpuh->flags   = 0x30; /* v1 and Protocol-type=GTP */
        gtpuh->msgtype = 0xff; /* T-PDU */
        gtpuh->length  = htons(orig_iplen);
        gtpuh->tunid   = htonl(skb->mark);

        /* Add UDP header */
        udp_len      = sizeof(struct udphdr) + sizeof(struct gtpuhdr) + orig_iplen;
        udph         = (struct udphdr*)skb_push(new_skb, sizeof(struct udphdr));
        udph->source = htons(GTPU_PORT);
        udph->dest   = htons(GTPU_PORT);
        udph->len    = htons(udp_len);
        udph->check  = 0;
        udph->check  = csum_tcpudp_magic(tgi->laddr,
                tgi->raddr,
                udp_len,
                IPPROTO_UDP,
                csum_partial((char*)udph, udp_len, 0));
        skb_set_transport_header(new_skb, 0);

        /* Add IP header */
        ip_len = MTU;
        iph = (struct iphdr*)skb_push(new_skb, sizeof(struct iphdr));
        iph->ihl      = 5;
        iph->version  = 4;
        iph->tos      = 0;
        iph->tot_len  = htons(ip_len);
        iph->id       = (uint16_t)(((uint64_t)new_skb) >> 8);
        iph->frag_off = htons(IP_MORE_FRAGMENTS);
        iph->ttl      = 64;
        iph->protocol = IPPROTO_UDP;
        iph->saddr    = (tgi->laddr);
        iph->daddr    = (tgi->raddr);
        iph->check    = 0;
        iph->check    = ip_fast_csum((unsigned char *)iph, iph->ihl);
        skb_set_network_header(new_skb, 0);

        skb_trim(new_skb, MTU - headroom_reqd);

        /* Route the packet */
        if (_gtpuah_route_packet(new_skb, tgi) == GTPU_SUCCESS)
        {
            remaining_payload_to_send = orig_iplen - MTU + headroom_reqd;
            remaining_bytes_to_send   = remaining_payload_to_send + sizeof(struct iphdr);
            bytes_to_remove = MTU - headroom_reqd;
            offset2 = MTU - headroom_reqd - sizeof(struct iphdr);

            // the localhost generated traffic can have len > MTU
            // but can cause problem at UE rx side
            while (remaining_payload_to_send > 0) {
                if ((remaining_payload_to_send + sizeof(struct iphdr)) > MTU) {
                    bytes_to_send = MTU;
                    payload_to_send = MTU - sizeof(struct iphdr);
                } else {
                    bytes_to_send = remaining_payload_to_send + sizeof(struct iphdr);
                    payload_to_send = remaining_payload_to_send;
                }
                pr_info("GTPUAH: Fragmentation Id %04X payload size %u, total ip packet size %u",
                        htons((uint16_t)(((uint64_t)new_skb) >> 8)),
                        payload_to_send,
                        bytes_to_send);
                // Not optimal, TO OPTIMIZE copies
                new_skb2 = skb_copy(skb, GFP_ATOMIC);
                skb_pull(new_skb2, bytes_to_remove);
                /* Add IP header */
                iph2 = (struct iphdr*)skb_push(new_skb2, sizeof(struct iphdr));
                iph2->ihl      = 5;
                iph2->version  = 4;
                iph2->tos      = 0;
                iph2->tot_len  = htons(bytes_to_send);
                iph2->id       = (uint16_t)(((uint64_t)new_skb) >> 8);
                iph2->frag_off = htons(offset2 / 8);
                iph2->ttl      = 64;
                iph2->protocol = IPPROTO_UDP;
                iph2->saddr    = (tgi->laddr);
                iph2->daddr    = (tgi->raddr);
                iph2->check    = 0;
                iph2->check    = ip_fast_csum((unsigned char *)iph2, iph2->ihl);
                skb_set_network_header(new_skb2, 0);
                skb_trim(new_skb2, MTU);

                _gtpuah_route_packet(new_skb2, tgi);

                remaining_payload_to_send = remaining_payload_to_send - payload_to_send;
                bytes_to_remove = bytes_to_remove + payload_to_send;
                offset2 = offset2 + payload_to_send;
            }
            return NF_DROP;
        }
        else
        {
            kfree_skb(new_skb);
            return NF_ACCEPT; /* What should we do here ??? ACCEPT seems to be the best option */
        }
    }
}



static unsigned int
xt_gtpuah_target(struct sk_buff *skb, const struct xt_action_param *par)
{
    const struct xt_gtpuah_target_info *tgi = par->targinfo;
    int result = NF_ACCEPT;

    if (tgi == NULL)
    {
        return result;
    }

    if (tgi->action == PARAM_GTPUAH_ACTION_ADD)
    {
        result = _gtpuah_target_add(skb, tgi);
    }
    return result;
}

static struct xt_target xt_gtpuah_reg __read_mostly =
{
    .name           = "GTPUAH",
    .revision       = 0,
    .family         = AF_INET,
    .hooks          = (1 << NF_INET_FORWARD) |
                      (1 << NF_INET_POST_ROUTING),
    .table          = "mangle",
    .target         = xt_gtpuah_target,
    .targetsize     = sizeof(struct xt_gtpuah_target_info),
    .me             = THIS_MODULE,
};

static int __init xt_gtpuah_init(void)
{
    pr_info("GTPUAH: Initializing module (KVersion: %d)\n", KVERSION);
    pr_info("GTPUAH: Copyright Polaris Networks 2010-2011\n");
    return xt_register_target(&xt_gtpuah_reg);
}

static void __exit xt_gtpuah_exit(void)
{
    xt_unregister_target(&xt_gtpuah_reg);
    pr_info("GTPUAH: Unloading module\n");
}

module_init(xt_gtpuah_init);
module_exit(xt_gtpuah_exit);

