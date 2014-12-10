
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


#define ROUTE_PACKET 1

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
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @member: the name of the hlist_node within the struct
 */
#define hash_for_each(name, bkt, obj, pos, member)                           \
        for ((bkt) = 0, obj = NULL; obj == NULL && (bkt) < HASH_SIZE(name);\
        (bkt)++)\
        hlist_for_each_entry(obj, pos, &name[bkt], member)


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


/*typedef struct ip_fragment_entry_s {
    struct sk_buff    *skb_p;
    //struct timeval     first_ts; // TO DO WITH IP_FRAG_TIME
    int                flags;  //ip header field
    int                offset; //ip header field
    int                end;
    struct hlist_node  node;
}ip_fragment_entry_t;

typedef struct ip_reassembly_entry_s {
}ip_reassembly_entry_t;
*/

static inline bool _gtpurh_ip_is_fragment(const struct iphdr *iph_p)
{
    return (iph_p->frag_off & htons(IP_MF | IP_OFFSET)) != 0;
}
#define MAX_IP_FRAGMENTS 16

#define GTPURH_HASHTABLE_BITS 7

DEFINE_HASHTABLE(ip_fragments, GTPURH_HASHTABLE_BITS);

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
static bool _gtpurh_route_packet(struct sk_buff *skb_pP, const struct xt_gtpurh_target_info *info_pP)
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
    skb_pP->pkt_type = PACKET_OTHERHOST;

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
    if (err != 0) 
    { 
        pr_info("GTPURH: Failed to route packet to dst 0x%x. Error: (%d)", fl.u.ip4.daddr, err);
        return GTPURH_FAILURE;
    } 

#if 1
    if (rt->dst.dev)
    {
        pr_info("GTPURH: dst dev name %s\n", rt->dst.dev->name);
    }
    else
    {
        pr_info("GTPURH: dst dev NULL\n");
    }
#endif

    //if (info_pP->action == PARAM_GTPURH_ACTION_ADD) //LG was commented
    {
        skb_dst_drop(skb_pP);
        skb_dst_set(skb_pP, &rt->dst);
        skb_pP->dev      = skb_dst(skb_pP)->dev;
    }

    skb_pP->protocol = htons(ETH_P_IP);

    /* Send the GTPu message out...gggH */
    err = dst_output(skb_pP);

    if (err == 0)
    {
        return GTPURH_SUCCESS;
    }
    else
    {
        return GTPURH_FAILURE;
    }
}
#endif
/*
static void
_gtpurh_delete_collection_ip_fragments(void)
{
    int                         bucket_loop_cursor = 0;
    struct ip_fragment_entry_s *ip_fragment_p      = NULL;
    struct hlist_node          *pos_p              = NULL;

    hash_for_each(ip_fragments, bucket_loop_cursor, ip_fragment_p, pos_p, node) {
        pr_info("GTPURH: %s struct ip_fragment_entry_s* %p, struct hlist_node* %p\n",
                __FUNCTION__,
                ip_fragment_p,
                pos_p);
    }
}


static int
_gtpurh_target_reassembly(struct sk_buff *orig_skb_pP, const struct xt_gtpurh_target_info *tgi_pP)
{
    struct iphdr               *iph_p         = ip_hdr(orig_skb_pP);
    struct sk_buff             *skb_p         = NULL;
    struct sk_buff             *new_skb_p     = NULL;
    uint16_t                    key           = 0;
    int                         flags, offset, last_frag_offset;
    int                         ihl, end;
    struct ip_fragment_entry_s *ip_fragment_p = NULL;
    struct hlist_node          *pos_p         = NULL;
    int                         all_segments_availables = 1;

    if ((orig_skb_pP) && (tgi_pP)) {
        offset   = ntohs(iph_p->frag_off);
        flags    = offset & ~IP_OFFSET;
        offset  &= IP_OFFSET; // offset is in 8-byte chunks
        offset <<= 3;
        ihl      = ip_hdrlen(orig_skb_pP);
        end      = offset + orig_skb_pP->len - ihl;

        key = iph_p->id;

        pr_info("---------------GTPURH: Dump fragment:------------------------------------------------\n");
        _gtpurh_print_hex_octets((unsigned char*)iph_p, orig_skb_pP->len);
        // Is this the final fragment?
        if ((flags  & IP_MF) == 0) {
            pr_info("GTPURH: try reassembly last segment id %u len %u offset %u end %u\n",
                    key,orig_skb_pP->len, offset, end);
            // may be the last segment received
            // normally iterator gives elements from head to tail, but elements
            // have been inserted at the head, so iterator should retrieve fragments
            // in reverse order.
            last_frag_offset = offset;
            hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
                pr_info("GTPURH: try reassembly segment id %u len %u offset %u end %u\n",
                        key,ip_fragment_p->skb_p->len, ip_fragment_p->offset, ip_fragment_p->end);
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
                new_skb_p = skb_copy_expand(orig_skb_pP,
                        last_frag_offset + skb_headroom(orig_skb_pP),
                        skb_tailroom(orig_skb_pP),
                        GFP_ATOMIC);
                if (new_skb_p != NULL)
                {
                    hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
                        skb_p = ip_fragment_p->skb_p;
                        skb_pull(new_skb_p, ihl);

                        pr_info("GTPURH: reassembly segment id %u len %u offset %u end %u\n",
                                key, skb_p->len, ip_fragment_p->offset, ip_fragment_p->end);

                        iph_p = (struct iphdr*)skb_push(new_skb_p, skb_p->len);
                        ihl = ip_hdrlen(skb_p);
                        memcpy(iph_p, skb_network_header(skb_p), skb_p->len);
                    }
                    skb_set_network_header(new_skb_p, 0);
                    if (_gtpurh_route_packet(new_skb_p, tgi_pP) != GTPURH_SUCCESS)
                    {
                        kfree_skb(new_skb_p);
                    }

                } else {
                    pr_info("GTPURH: Failed in skb_p allocation (%u bytes)\n",
                            last_frag_offset + skb_headroom(orig_skb_pP));
                }
            }
            if (1) {
                hash_for_each_possible(ip_fragments, ip_fragment_p, pos_p, node, key) {
                    kfree_skb(ip_fragment_p->skb_p);
                    hash_del(pos_p);
                    kfree(ip_fragment_p);
                }
            }
        } else {
            pr_info("GTPURH: but in buffer not last segment id %u len %u offset %u end %u\n",
                    key,orig_skb_pP->len, offset, end);
            // not the last fragment received
            // assuming no out of order
            ip_fragment_p = kmalloc(sizeof(struct ip_fragment_entry_s), GFP_KERNEL);

            if (NULL != ip_fragment_p) {
                ip_fragment_p->skb_p    = skb_copy(orig_skb_pP, GFP_ATOMIC); // may use skb_clone(), TODO
                ip_fragment_p->flags  = flags;
                ip_fragment_p->offset = offset;
                ip_fragment_p->end    = end;
                pr_info("GTPURH: adding in hashtable key %04x ip_fragment %p skbuff %p\n",
                        key, ip_fragment_p, ip_fragment_p->skb_p);

                INIT_HLIST_NODE(&ip_fragment_p->node);
                hash_add(ip_fragments, &ip_fragment_p->node, key);
                return 0;
            } else {
                pr_info("GTPURH: ERROR could not allocate memory for buffering IP fragment\n");
            }
        }
    } else {
        pr_info("%s(skb_p=%p,tgi_pP=%p) parameter skb_p or tgi_pP is NULL\n",
                __FUNCTION__, orig_skb_pP, tgi_pP);
    }
    return -1;
}*/


static unsigned int
_gtpurh_target_rem(struct sk_buff *orig_skb_pP, const struct xt_gtpurh_target_info *tgi_pP)
{
    struct iphdr   *iph_p            = ip_hdr(orig_skb_pP);
    struct iphdr   *iph2_p           = NULL;
    struct udphdr  *udph_p           = NULL;
    struct gtpuhdr *gtpuh_p          = NULL;
    struct sk_buff *skb_p            = NULL;
    uint16_t        gtp_payload_size = 0;

    /* Create a new copy of the original skb_p...can't avoid :-( LG: WHY???*/
#if defined(ROUTE_PACKET)
    skb_p = skb_copy(orig_skb_pP, GFP_ATOMIC);
    if (skb_p == NULL)
    {
        return NF_ACCEPT;
    }
    skb_p->skb_iif  = orig_skb_pP->skb_iif;
    skb_p->protocol = orig_skb_pP->protocol;
#else
    skb_p = orig_skb_pP;
#endif
    pr_info("GTPURH(%d) IP packet arrived\n",tgi_pP->action);
    //---------------------------
    // check if is GTPU TUNNEL
    if (iph_p->protocol != IPPROTO_UDP) {
        pr_info("GTPURH(%d): Not GTPV1U packet (not UDP)\n",tgi_pP->action);
        return NF_ACCEPT;
    }

    //---------------------------
    // check if is fragment
    // but should not happen since MTU should have been set bigger than 1500 + GTP encap.
    // TO DO later segment, but did not succeed in getting in this target all framents of an ip packet!
    if (_gtpurh_ip_is_fragment(iph_p)) {
        pr_info("GTPURH(%d): ip_is_fragment YES, FLAGS %04X & %04X = %04X\n",
                tgi_pP->action,
                iph_p->frag_off,
                htons(IP_MF | IP_OFFSET),
                iph_p->frag_off & htons(IP_MF | IP_OFFSET));
        /*if ((rc = _gtpurh_target_reassembly(skb_p, tgi_pP)) == 0) {
            return NF_DROP; // orig_skb_pP is copied, original can be destroyed.
        } else  {*/
            return NF_ACCEPT;
        //}
    } /*else {
        pr_info("GTPURH(%d): ip_is_fragment NO, FLAGS %04X & %04X = %04X\n",
                tgi_pP->action,
                iph_p->frag_off,
                htons(IP_MF | IP_OFFSET),
                iph_p->frag_off & htons(IP_MF | IP_OFFSET));
    }*/

    if (skb_p->len <= sizeof (struct udphdr) + sizeof (struct gtpuhdr) + sizeof (struct iphdr)) {
        pr_info("GTPURH(%d): Thought was GTPV1U packet but too short length\n",tgi_pP->action);
        return NF_ACCEPT;
    }
    /* Remove IP header */
    udph_p = (struct udphdr*)skb_pull(skb_p, (iph_p->ihl << 2));

    if (udph_p->dest != htons(GTPURH_PORT)) {
        pr_info("GTPURH(%d): Not GTPV1U packet (bad UDP dest port)\n",tgi_pP->action);
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
        skb_pull(skb_p, sizeof(short) + sizeof(char) + sizeof(char)); /* #Seq, #N-PDU, #ExtHdr Type */
        gtp_payload_size = gtp_payload_size - sizeof(short) - sizeof(char) - sizeof(char);
    }
    //skb_p->mac_len = 0;
    //skb_set_mac_header(skb_p, 0);
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
        pr_info("\nGTPURH(%d): Decapsulated packet: %d.%d.%d.%d --> %d.%d.%d.%d Proto: %d, Total Len (IP): %u mark %u Frag offset %u Flags 0x%0x\n",
                tgi_pP->action,
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
            pr_info("GTPURH(%d): Mismatch in lengths GTPU length: %u -> %u, IP length %u\n",
                    tgi_pP->action,
                    ntohs(gtpuh_p->length),
                    gtp_payload_size,
                    ntohs(iph2_p->tot_len));

            _gtpurh_print_hex_octets((unsigned char*)iph_p, ntohs(iph_p->tot_len));
        }
    }
//#endif
    pr_info("GTPURH(%d) IP packet prcessed\n",tgi_pP->action);

    /* Route the packet */
#if defined(ROUTE_PACKET)
    _gtpurh_route_packet(skb_p, tgi_pP);
    return NF_DROP;
#else
    return NF_ACCEPT;
#endif
}


static unsigned int
xt_gtpurh_target(struct sk_buff *skb_pP, const struct xt_action_param *par)
{
    const struct xt_gtpurh_target_info *tgi_p = par->targinfo;
    int result = NF_ACCEPT;

    if (tgi_p == NULL)
    {
        return result;
    }

    if (tgi_p->action == PARAM_GTPURH_ACTION_REM)
    {
        result = _gtpurh_target_rem(skb_pP, tgi_p);
    }
    return result;
}

static struct xt_target xt_gtpurh_reg __read_mostly =
{
    .name           = "GTPURH",
    .revision       = 0,
    .family         = AF_INET,
    .hooks          = (1 << NF_INET_PRE_ROUTING) |
                      (1 << NF_INET_LOCAL_OUT),
    .table          = "raw",
    .target         = xt_gtpurh_target,
    .targetsize     = sizeof(struct xt_gtpurh_target_info),
    .me             = THIS_MODULE,
};

static int __init xt_gtpurh_init(void)
{
    pr_info("GTPURH: Initializing module (KVersion: %d)\n", KVERSION);
    pr_info("GTPURH: Copyright Polaris Networks 2010-2011\n");
    pr_info("GTPURH: Modified by EURECOM Lionel GAUTHIER 2014\n");
    //hash_init(ip_fragments);
    return xt_register_target(&xt_gtpurh_reg);
}

static void __exit xt_gtpurh_exit(void)
{
    xt_unregister_target(&xt_gtpurh_reg);
    //_gtpurh_delete_collection_ip_fragments();
    pr_info("GTPURH: Unloading module\n");
}

module_init(xt_gtpurh_init);
module_exit(xt_gtpurh_exit);

