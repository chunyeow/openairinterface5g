
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


/*
 * Statically sized hash table implementation
 * (C) 2012  Sasha Levin <levinsasha928@gmail.com>
 */

#ifndef _LINUX_HASHTABLE_H
#define _LINUX_HASHTABLE_H

#include <linux/list.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/rculist.h>

#define DEFINE_HASHTABLE(name, bits)                                            \
        struct hlist_head name[1 << (bits)] =                                   \
        { [0 ... ((1 << (bits)) - 1)] = HLIST_HEAD_INIT }

#define DECLARE_HASHTABLE(name, bits)                                           \
        struct hlist_head name[1 << (bits)]

#define HASH_SIZE(name) (ARRAY_SIZE(name))
#define HASH_BITS(name) ilog2(HASH_SIZE(name))

/* Use hash_32 when possible to allow for fast 32bit hashing in 64bit kernels. */
#define hash_min(val, bits)                                                     \
        (sizeof(val) <= 4 ? hash_32(val, bits) : hash_long(val, bits))

static inline void __hash_init(struct hlist_head *ht, unsigned int sz)
{
    unsigned int i;

    for (i = 0; i < sz; i++)
        INIT_HLIST_HEAD(&ht[i]);
}

/**
 * hash_init - initialize a hash table
 * @hashtable: hashtable to be initialized
 *
 * Calculates the size of the hashtable from the given parameter, otherwise
 * same as hash_init_size.
 *
 * This has to be a macro since HASH_BITS() will not work on pointers since
 * it calculates the size during preprocessing.
 */
#define hash_init(hashtable) __hash_init(hashtable, HASH_SIZE(hashtable))

/**
 * hash_add - add an object to a hashtable
 * @hashtable: hashtable to add to
 * @node: the &struct hlist_node of the object to be added
 * @key: the key of the object to be added
 */
#define hash_add(hashtable, node, key)                                          \
        hlist_add_head(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])

/**
 * hash_add_rcu - add an object to a rcu enabled hashtable
 * @hashtable: hashtable to add to
 * @node: the &struct hlist_node of the object to be added
 * @key: the key of the object to be added
 */
#define hash_add_rcu(hashtable, node, key)                                      \
        hlist_add_head_rcu(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])

/**
 * hash_hashed - check whether an object is in any hashtable
 * @node: the &struct hlist_node of the object to be checked
 */
static inline bool hash_hashed(struct hlist_node *node)
{
    return !hlist_unhashed(node);
}

static inline bool __hash_empty(struct hlist_head *ht, unsigned int sz)
{
    unsigned int i;

    for (i = 0; i < sz; i++)
        if (!hlist_empty(&ht[i]))
            return false;

    return true;
}

/**
 * hash_empty - check whether a hashtable is empty
 * @hashtable: hashtable to check
 *
 * This has to be a macro since HASH_BITS() will not work on pointers since
 * it calculates the size during preprocessing.
 */
#define hash_empty(hashtable) __hash_empty(hashtable, HASH_SIZE(hashtable))

/**
 * hash_del - remove an object from a hashtable
 * @node: &struct hlist_node of the object to remove
 */
static inline void hash_del(struct hlist_node *node)
{
    hlist_del_init(node);
}

/**
 * hash_del_rcu - remove an object from a rcu enabled hashtable
 * @node: &struct hlist_node of the object to remove
 */
static inline void hash_del_rcu(struct hlist_node *node)
{
    hlist_del_init_rcu(node);
}

/**
 * hash_for_each - iterate over a hashtable
 * @name: hashtable to iterate
 * @bkt: integer to use as bucket loop cursor
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 */
#define hash_for_each(name, bkt, obj, member)                           \
        for ((bkt) = 0, obj = NULL; obj == NULL && (bkt) < HASH_SIZE(name);\
        (bkt)++)\
        hlist_for_each_entry(obj, &name[bkt], member)

/**
 * hash_for_each_rcu - iterate over a rcu enabled hashtable
 * @name: hashtable to iterate
 * @bkt: integer to use as bucket loop cursor
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 */
#define hash_for_each_rcu(name, bkt, obj, member)                       \
        for ((bkt) = 0, obj = NULL; obj == NULL && (bkt) < HASH_SIZE(name);\
        (bkt)++)\
        hlist_for_each_entry_rcu(obj, &name[bkt], member)

/**
 * hash_for_each_safe - iterate over a hashtable safe against removal of
 * hash entry
 * @name: hashtable to iterate
 * @bkt: integer to use as bucket loop cursor
 * @tmp: a &struct used for temporary storage
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 */
#define hash_for_each_safe(name, bkt, tmp, obj, member)                 \
        for ((bkt) = 0, obj = NULL; obj == NULL && (bkt) < HASH_SIZE(name);\
        (bkt)++)\
        hlist_for_each_entry_safe(obj, tmp, &name[bkt], member)

/**
 * hash_for_each_possible - iterate over all possible objects hashing to the
 * same bucket
 * @name: hashtable to iterate
 * @tpos:   the type * to use as a loop cursor.
 * @pos: the &struct hlist_node to use as a loop cursor.
 * @member: the name of the hlist_node within the struct
 * @key: the key of the objects to iterate over
 */
#define hash_for_each_possible(name, tpos, pos, member, key)                  \
        hlist_for_each_entry(tpos, pos, &name[hash_min(key, HASH_BITS(name))], member)


/**
 * hash_for_each_possible_rcu - iterate over all possible objects hashing to the
 * same bucket in an rcu enabled hashtable
 * in a rcu enabled hashtable
 * @name: hashtable to iterate
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 * @key: the key of the objects to iterate over
 */
#define hash_for_each_possible_rcu(name, obj, member, key)              \
        hlist_for_each_entry_rcu(obj, &name[hash_min(key, HASH_BITS(name))],\
                member)

/**
 * hash_for_each_possible_rcu_notrace - iterate over all possible objects hashing
 * to the same bucket in an rcu enabled hashtable in a rcu enabled hashtable
 * @name: hashtable to iterate
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 * @key: the key of the objects to iterate over
 *
 * This is the same as hash_for_each_possible_rcu() except that it does
 * not do any RCU debugging or tracing.
 */
#define hash_for_each_possible_rcu_notrace(name, obj, member, key) \
        hlist_for_each_entry_rcu_notrace(obj, \
                &name[hash_min(key, HASH_BITS(name))], member)

/**
 * hash_for_each_possible_safe - iterate over all possible objects hashing to the
 * same bucket safe against removals
 * @name: hashtable to iterate
 * @obj: the type * to use as a loop cursor for each entry
 * @tmp: a &struct used for temporary storage
 * @member: the name of the hlist_node within the struct
 * @key: the key of the objects to iterate over
 */
#define hash_for_each_possible_safe(name, obj, tmp, member, key)        \
        hlist_for_each_entry_safe(obj, tmp,\
                &name[hash_min(key, HASH_BITS(name))], member)


#endif







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


typedef struct ip_fragment_entry_s {
    struct sk_buff    *skb;
    //struct timeval     first_ts; // TO DO WITH IP_FRAG_TIME
    int                flags;  //ip header field
    int                offset; //ip header field
    int                end;
    struct hlist_node  node;
}ip_fragment_entry_t;

typedef struct ip_reassembly_entry_s {
}ip_reassembly_entry_t;



#define MAX_IP_FRAGMENTS 16;

#define GTPURH_HASHTABLE_BITS 7

DEFINE_HASHTABLE(ip_fragments, GTPURH_HASHTABLE_BITS);

//-----------------------------------------------------------------------------
void _gtpurh_print_hex_octets(unsigned char* dataP, unsigned short sizeP)
//-----------------------------------------------------------------------------
{
  unsigned long octet_index = 0;
  unsigned long buffer_marker = 0;
  unsigned char aindex;
  struct timeval tv;
  char timeofday[64];
  unsigned int h,m,s;

  if (dataP == NULL) {
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
  for (octet_index = 0; octet_index < sizeP; octet_index++) {
    if ((octet_index % 16) == 0){
      if (octet_index != 0) {
          buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
      }
      buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, "%s %04ld |",timeofday, octet_index);
    }
    /*
     * Print every single octet in hexadecimal form
     */
    buffer_marker+=snprintf(&_gtpurh_print_buffer[buffer_marker], GTPURH_2_PRINT_BUFFER_LEN - buffer_marker, " %02x", dataP[octet_index]);
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


static void
_gtpurh_target_reassembly(struct sk_buff *orig_skb, const struct xt_gtpurh_target_info *tgi)
{
    struct iphdr               *iph           = ip_hdr(orig_skb);
    struct sk_buff             *skb           = NULL;
    struct sk_buff             *new_skb       = NULL;
    uint16_t                    key           = 0;
    int                         flags, offset, last_frag_offset;
    int                         ihl, end;
    struct ip_fragment_entry_s *ip_fragment_p = NULL;
    struct hlist_node          *pos_p         = NULL;
    int                         all_segments_availables = 1;

    offset = ntohs(iph->frag_off);
    flags  = offset & ~IP_OFFSET;
    offset &= IP_OFFSET; /* offset is in 8-byte chunks */
    offset <<= 3;
    ihl     = ip_hdrlen(skb);
    end = offset + skb->len - ihl;

    key = iph->id;

    /* Is this the final fragment? */
    if ((flags  & IP_MF) == 0) {
        pr_info("GTPURH: try reassembly last segment id %u len %u offset %u end %u\n",
                        key,orig_skb->len, offset, end);
        // may be the last segment received
        // normally iterator gives elements from head to tail, but elements
        // have been inserted at the head, so iterator should retrieve fragments
        // in reverse order.
        last_frag_offset = offset;
        hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
            pr_info("GTPURH: try reassembly segment id %u len %u offset %u end %u\n",
                    key,ip_fragment_p->skb->len, ip_fragment_p->offset, ip_fragment_p->end);
            if (ip_fragment_p->end == offset) {
                offset = ip_fragment_p->offset;
            } else {
                pr_info("GTPURH: try reassembly segment id %u failed\n",
                        key);
                all_segments_availables = 0;
                break;
            }
        }
        if (all_segments_availables) {
            // not optimal, but optimize later
            new_skb = skb_copy_expand(orig_skb,
                    last_frag_offset + skb_headroom(orig_skb),
                    skb_tailroom(orig_skb),
                    GFP_ATOMIC);
            if (new_skb != NULL)
            {
                hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
                    skb = ip_fragment_p->skb;
                    skb_pull(new_skb, ihl);

                    pr_info("GTPURH: reassembly segment id %u len %u offset %u end %u\n",
                            key, skb->len, ip_fragment_p->offset, ip_fragment_p->end);

                    iph = (struct iphdr*)skb_push(new_skb, skb->len);
                    ihl = ip_hdrlen(skb);
                    memcpy(iph, skb_network_header(skb), skb->len);
                }
                skb_set_network_header(new_skb, 0);
                if (_gtpurh_route_packet(new_skb, tgi) != GTPURH_SUCCESS)
                {
                    kfree_skb(new_skb);
                }

            } else {
                pr_info("GTPURH: Failed in skb allocation (%u bytes)\n",
                        last_frag_offset + skb_headroom(orig_skb));
            }
        }
        if (1) {
            hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
                kfree_skb(ip_fragment_p->skb);
                hash_del(pos_p);
                kfree(ip_fragment_p);
            }
        }
    } else {
        pr_info("GTPURH: but in buffer not last segment id %u len %u offset %u end %u\n",
                        key,orig_skb->len, offset, end);
        // not the last fragment received
        // assuming no out of order
        ip_fragment_p = kmalloc(sizeof(struct ip_fragment_entry_s), GFP_KERNEL);

        if (NULL != ip_fragment_p) {
#if defined(ROUTE_PACKET)
            ip_fragment_p->skb    = orig_skb;
#else
            ip_fragment_p->skb    = skb_copy(orig_skb, GFP_ATOMIC); // may use skb_clone(), TODO
#endif
            ip_fragment_p->flags  = flags;
            ip_fragment_p->offset = offset;
            ip_fragment_p->end    = end;

            INIT_HLIST_NODE(&ip_fragment_p->node);
            hash_add(ip_fragments, &ip_fragment_p->node, key);
        } else {
            pr_info("GTPURH: ERROR could not allocate memory for buffering IP fragment\n");
        }
    }
}


static unsigned int
_gtpurh_target_rem(struct sk_buff *orig_skb, const struct xt_gtpurh_target_info *tgi)
{
    struct iphdr   *iph   = ip_hdr(orig_skb);
    struct iphdr   *iph2  = NULL;
    struct udphdr  *udph  = NULL;
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

    if (ip_is_fragment(iph)) {
        _gtpurh_target_reassembly(skb, tgi);
        return NF_DROP; // orig_skb is copied, original can be destroyed.
    }

    /* Remove IP header */
    udph = (struct udphdr*)skb_pull(skb, (iph->ihl << 2));

    if (iph->protocol != IPPROTO_UDP) {
        pr_info("GTPURH(%d): ERROR in decapsulating packet: %d.%d.%d.%d --> %d.%d.%d.%d Bad Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
                tgi->action,
                iph->saddr  & 0xFF,
                (iph->saddr & 0x0000FF00) >> 8,
                (iph->saddr & 0x00FF0000) >> 16,
                iph->saddr >> 24,
                iph->daddr  & 0xFF,
                (iph->daddr & 0x0000FF00) >> 8,
                (iph->daddr & 0x00FF0000) >> 16,
                iph->daddr >> 24,
                iph->protocol,
                ntohs(iph2->tot_len),
                skb->mark,
                ntohs(iph->frag_off) & 0x1FFFFFFF,
                ntohs(iph->frag_off) >> 13);
        return NF_ACCEPT;
    }

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

    if ((iph2->version  != 4 ) && (iph2->version  != 6)) {
        pr_info("\nGTPURH: Decapsulated packet dropped because not IPvx protocol see all GTPU packet here:\n");
        _gtpurh_print_hex_octets((unsigned char*)iph, ntohs(iph->tot_len));
        return NF_DROP;
    }
//#if 0
    if ((skb->mark == 0) || (gtp_payload_size != ntohs(iph2->tot_len))) {
        pr_info("\nGTPURH(%d): Decapsulated packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
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
            pr_info("GTPURH(%d): Mismatch in lengths GTPU length: %u -> %u, IP length %u\n",
                    tgi->action,
                    ntohs(gtpuh->length),
                    gtp_payload_size,
                    ntohs(iph2->tot_len));

            _gtpurh_print_hex_octets((unsigned char*)iph, ntohs(iph->tot_len));
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
    hash_init(ip_fragments);
    return xt_register_target(&xt_gtpurh_reg);
}

static void __exit xt_gtpurh_exit(void)
{
    xt_unregister_target(&xt_gtpurh_reg);
    pr_info("GTPURH: Unloading module\n");
}

module_init(xt_gtpurh_init);
module_exit(xt_gtpurh_exit);

