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
/*! \file pmip_handler.c
 * \brief
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair_tech@eurecom.fr
 */
#define PMIP
#define PMIP_HANDLER_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_handler.h"
#include "pmip_lma_proc.h"
#include "pmip_mag_proc.h"
#include "pmip_msgs.h"
//---------------------------------------------------------------------------------------------------------------------
#include "tunnelctl.h"
#include "ndisc.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *link_local_addr(struct in6_addr *id)
{
    static struct in6_addr ADDR;
    ADDR = in6addr_any;
    ADDR.s6_addr32[0] = htonl(0xfe800000);
    //copy the MN_ID.
    memcpy(&ADDR.s6_addr32[2], &id->s6_addr32[2], sizeof(ip6mnid_t));
    return &ADDR;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *CONVERT_ID2ADDR(struct in6_addr *result, struct in6_addr *prefix, struct in6_addr *id)
{
    *result = in6addr_any;
    memcpy(&result->s6_addr32[0], &prefix->s6_addr32[0], sizeof(ip6mnid_t));
    memcpy(&result->s6_addr32[2], &id->s6_addr32[2], sizeof(ip6mnid_t));
    return result;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *get_mn_addr(pmip_entry_t * bce)
{
    CONVERT_ID2ADDR(&bce->mn_addr, &bce->mn_prefix, &bce->mn_suffix);
    return &bce->mn_addr;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *solicited_mcast(struct in6_addr *id)
{
    //NUD_ADDR converts an ID into a Multicast Address for NS Unreachability!
    static struct in6_addr ADDR2;
    ADDR2 = in6addr_any;
    ADDR2.s6_addr32[0] = htonl(0xff020000);
    ADDR2.s6_addr32[1] = htonl(0x00000000);
    ADDR2.s6_addr32[2] = htonl(0x00000001);
    ADDR2.s6_addr[12] = 0xff;
    //copy the least 24 bits from the MN_ID.
    memcpy(&ADDR2.s6_addr[13], &id->s6_addr[13], 3 * sizeof(ADDR2.s6_addr[0]));
    return &ADDR2;
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_retrans_pbu_handler(struct tq_elem *tqe)
{
    int mutex_return_code;

    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    printf("-------------------------------------\n");
    if (!task_interrupted()) {
        pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
        mutex_return_code = pthread_rwlock_wrlock(&e->lock);
        if (mutex_return_code != 0) {
            dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
        }
        dbg("Retransmissions counter : %d\n", e->n_rets_counter);
        if (e->n_rets_counter == 0) {
            free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
            dbg("No PBA received from LMA....\n");
            dbg("Abort Trasmitting the PBU....\n");
            mutex_return_code = pthread_rwlock_unlock(&e->lock);
            if (mutex_return_code != 0) {
                dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
            }
            pmip_bce_delete(e);
        } else {
            //Decrement the N trasnmissions counter.
            e->n_rets_counter--;
            struct in6_addr_bundle addrs;
            addrs.src = &conf.OurAddress;
            addrs.dst = &conf.LmaAddress;
            //sends a PBU
            dbg("Send PBU again....\n");

            // INCREMENT SEQ NUMBER OF PBU
            e->seqno_out        = get_new_pbu_sequence_number();
            ((struct ip6_mh_binding_update *)(e->mh_vec[0].iov_base))->ip6mhbu_seqno = htons(e->seqno_out);

            pmip_mh_send(&addrs, e->mh_vec, e->iovlen, e->link);
            //add a new task for PBU retransmission.
            struct timespec expires;
            clock_gettime(CLOCK_REALTIME, &e->add_time);
            tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
            add_task_abs(&expires, &e->tqe, pmip_timer_retrans_pbu_handler);
            dbg("PBU Retransmissions timer is triggered again....\n");
            mutex_return_code = pthread_rwlock_unlock(&e->lock);
            if (mutex_return_code != 0) {
                dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
            }
        }
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_bce_expired_handler(struct tq_elem *tqe)
{
    int mutex_return_code;
    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    printf("-------------------------------------\n");
    if (!task_interrupted()) {
        pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
        mutex_return_code = pthread_rwlock_wrlock(&e->lock);
        if (mutex_return_code != 0) {
            dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
        }
        dbg("Retransmissions counter : %d\n", e->n_rets_counter);
        if (e->n_rets_counter == 0) {
            dbg("Retransmissions counter expired\n");
            free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
            if (is_mag()) {
                //++e->seqno_out;
                dbg("Calling deregistration\n");
                mag_dereg(e, 1);
                pmipcache_release_entry(e);
                pmip_bce_delete(e);
                return;
            }
            //Delete existing route for the deleted MN
            if (is_ha()) {
                lma_dereg(e, 0, 0);
                pmipcache_release_entry(e);
                pmip_bce_delete(e);
                return;
            }
            mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
            if (mutex_return_code != 0) {
                dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
            }
            return;
        }
        if (is_mag()) {
            dbg("Send NS for Neighbour Reachability for:%x:%x:%x:%x:%x:%x:%x:%x iif=%d\n", NIP6ADDR(&e->mn_hw_address), e->link);
            //Create NS for Reachability test!
            //ndisc_send_ns(e->link, &conf.MagAddressIngress[0], solicited_mcast(&e->mn_suffix), get_mn_addr(e));
            ndisc_send_ns(e->link, get_mn_addr(e));


            struct timespec expires;
            clock_gettime(CLOCK_REALTIME, &e->add_time);
            tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
            // Add a new task for deletion of entry if No Na is received.
            add_task_abs(&expires, &e->tqe, pmip_timer_bce_expired_handler);
            dbg("Start the Timer for Retransmission/Deletion ....\n");
            //Decrements the Retransmissions counter.
            e->n_rets_counter--;
            mutex_return_code = pthread_rwlock_unlock(&e->lock);
            if (mutex_return_code != 0) {
                dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
            }
        }
        if (is_ha()) {
            lma_dereg(e, 0, 0);
            pmipcache_release_entry(e);
            pmip_bce_delete(e);
            return;
        }
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_tunnel_expired_handler(struct tq_elem *tqe)
{
    int mutex_return_code;
	int res;

    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    printf("-------------------------------------\n");
    printf("-pmip_timer_tunnel_expired_handler()-\n");
    printf("-------------------------------------\n");
    if (!task_interrupted()) {
        tunnel_timer_t *tt = container_of(tqe, tunnel_timer_t, tqe);
        res = tunnel_del(tt->tunnel, 0, 0);
        tt->lifetime.tv_nsec = 0;
        tt->lifetime.tv_sec  = 0;
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
}


/**
 * Handlers defined for MH and ICMP messages.
 **/

/*!
 * check if address is solicited multicast
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_solicited_mcast(const struct in6_addr *addr)
{
    return (addr->s6_addr32[0] == htonl(0xff020000)
            && addr->s6_addr32[1] == htonl(0x00000000)
            && addr->s6_addr32[2] == htonl(0x00000001)
            && addr->s6_addr[12] == 0xff);
}

/*!
 * check if address is multicast
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_multicast(const struct in6_addr *addr)
{
    return (addr->s6_addr32[0] & htonl(0xFF000000)) == htonl(0xFF000000);
}

/*!
 * check if address is linklocal
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_linklocal(const struct in6_addr *addr)
{
    return IN6_IS_ADDR_LINKLOCAL(addr);
}


/*!
 * handler called when receiving a router solicitation
 */
//hip
static void pmip_mag_recv_rs(const struct icmp6_hdr *ih, __attribute__ ((unused)) ssize_t len, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
    dbg("\n");
    dbg("Router Solicitation received \n");
    printf("-------------------------------------\n");
    dbg("Router Solicitation (RS) Received iif %d\n", iif);
    dbg("Received RS Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(saddr));
    dbg("Received RS Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(daddr));
    msg_info_t rs_info;
    bzero(&rs_info, sizeof(rs_info));
    icmp_rs_parse(&rs_info, (struct nd_router_solicit *) ih, saddr, daddr, iif, hoplimit);
    mag_fsm(&rs_info);
}

/*!
 * handler called when receiving a proxy binding acknowledgment
 */
static void pmip_mag_recv_pba(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
    printf("=====================================\n");
    dbg("Proxy Binding Acknowledgement (PBA) Received\n");
    dbg("Received PBA Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->src));
    dbg("Received PBA Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->dst));
    //define the values for calling the parsing function
    //call the parsing function
    struct ip6_mh_binding_ack *pba;
    //call the fsm function.
    msg_info_t info;
    pba = (struct ip6_mh_binding_ack *) ((void *) mh);
    mh_pba_parse(&info, pba, len, in_addrs, iif);
    mag_fsm(&info);
}


/*!
 * handler called when receiving a proxy binding update
 */
void pmip_lma_recv_pbu(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
    printf("=====================================\n");
    dbg("Proxy Binding Update (PBU) Received\n");
    dbg("Received PBU Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->src));
    dbg("Received PBU Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->dst));
    //define the values for the parsing function
    //call the parsing function
    struct ip6_mh_binding_update *pbu = (struct ip6_mh_binding_update *) mh;
    //call the fsm function.
    msg_info_t info;
    bzero(&info, sizeof(info));
    mh_pbu_parse(&info, pbu, len, in_addrs, iif);
    lma_fsm(&info);
}

/*!
 * handler called when MAG receive a neighbor advertisement
 */
static void pmip_mag_recv_na(const struct icmp6_hdr *ih, ssize_t len, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
    // define the MN identifier
    //struct in6_addr id = in6addr_any;
    struct nd_neighbor_advert *msg = (struct nd_neighbor_advert *) ih;
    //Check target is not link local address.
    if (ipv6_addr_is_linklocal(&msg->nd_na_target)) {
        return;
    }
    //Check target is not multicast.
    if (ipv6_addr_is_multicast(&msg->nd_na_target)) {
        return;
    }
    if (len - sizeof(struct nd_neighbor_advert) > 0) {
        printf("-------------------------------------\n");
        dbg("Neighbor Advertisement (NA) Received\n");
        dbg("Received NA Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(saddr));
        dbg("Received NA Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(daddr));
        msg_info_t na_info;
        bzero(&na_info, sizeof(na_info));
        icmp_na_parse(&na_info, (struct nd_neighbor_advert *) ih, saddr, daddr, iif, hoplimit);
        mag_fsm(&na_info);
    }
    return;
}


struct icmp6_handler pmip_mag_rs_handler = {
        .recv = pmip_mag_recv_rs
};

struct mh_handler pmip_mag_pba_handler = {
        .recv = pmip_mag_recv_pba
};
struct mh_handler pmip_lma_pbu_handler = {
        .recv = pmip_lma_recv_pbu
};
struct icmp6_handler pmip_mag_recv_na_handler = {
        .recv = pmip_mag_recv_na
};
