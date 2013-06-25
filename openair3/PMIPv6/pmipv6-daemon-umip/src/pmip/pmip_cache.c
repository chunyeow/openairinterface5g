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
/*! \file pmip_cache.c
* \brief PMIP binding cache functions
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/

#define PMIP
#define PMIP_CACHE_C
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_cache.h"
#include "pmip_handler.h"
//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
static struct hash      g_pmip_hash;
static int              g_pmip_cache_count = 0;
//---------------------------------------------------------------------------------------------------------------------
int get_pmip_cache_count(int type)
{
    if (type == BCE_PMIP || type == BCE_TEMP) {
        return g_pmip_cache_count;
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void dump_pbce(void *bce, void *os)
{
    pmip_entry_t *e = (pmip_entry_t *) bce;
    FILE *out = (FILE *) os;
    fprintf(out, " == Proxy Binding Cache entry ");
    switch (e->type) {
        case BCE_PMIP:
            fprintf(out, "(BCE_PMIP)\n");
            break;
        case BCE_TEMP:
            fprintf(out, "(BCE_TEMP)\n");
            break;
        default:
            fprintf(out, "(Unknown)\n");
    }
    fprintf(out, " MN IID:                 %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&e->mn_suffix));
    fprintf(out, " MN HW Address:          %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&e->mn_hw_address));
    fprintf(out, " MN Serving MAG Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&e->mn_serv_mag_addr));
    fprintf(out, " MN Serving LMA Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&e->mn_serv_lma_addr));
    fprintf(out, " lifetime %ld\n ", e->lifetime.tv_sec);
    fprintf(out, " seqno    %d\n", e->seqno_out);
    fflush(out);
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_cache_init(void)
{
    int ret;
    int mutex_return_code;
    if (pthread_rwlock_init(&pmip_lock, NULL)) {
        return -1;
    }
    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }

    ret = hash_init(&g_pmip_hash, DOUBLE_ADDR, PMIP_CACHE_BUCKETS);

    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }

    #ifdef ENABLE_VT
    if (ret < 0)
        return ret;
    ret = vt_pbc_init();
    #endif
    return ret;
}
//---------------------------------------------------------------------------------------------------------------------
void init_iface_ra()
{
    router_ad_iface.AdvSendAdvert = DFLT_AdvSendAdv;
    router_ad_iface.MaxRtrAdvInterval = DFLT_MaxRtrAdvInterval;
    router_ad_iface.MinRtrAdvInterval = 1;  //changed from -1
    router_ad_iface.AdvIntervalOpt = DFLT_AdvIntervalOpt;
    router_ad_iface.AdvCurHopLimit = DFLT_AdvCurHopLimit;
    router_ad_iface.AdvHomeAgentFlag = DFLT_AdvHomeAgentFlag;
    router_ad_iface.AdvHomeAgentInfo = DFLT_AdvHomeAgentInfo;
    router_ad_iface.HomeAgentPreference = DFLT_HomeAgentPreference;
    router_ad_iface.HomeAgentLifetime = 10000;  //changed from -1
    router_ad_iface.AdvReachableTime = DFLT_AdvReachableTime;
    router_ad_iface.AdvRetransTimer = DFLT_AdvRetransTimer;
    router_ad_iface.AdvDefaultLifetime = 6000;
    router_ad_iface.AdvManagedFlag = 0;
    router_ad_iface.AdvOtherConfigFlag = 0;
    // default values for Prefix.
    router_ad_iface.Adv_Prefix.AdvOnLinkFlag = DFLT_AdvOnLinkFlag;
    router_ad_iface.Adv_Prefix.AdvAutonomousFlag = DFLT_AdvAutonomousFlag;
    router_ad_iface.Adv_Prefix.AdvRouterAddr = DFLT_AdvRouterAddr;
    router_ad_iface.Adv_Prefix.PrefixLen = 64;
    router_ad_iface.Adv_Prefix.AdvValidLifetime = DFLT_AdvValidLifetime;
    router_ad_iface.Adv_Prefix.AdvPreferredLifetime = DFLT_AdvPreferredLifetime;
}
//---------------------------------------------------------------------------------------------------------------------
pmip_entry_t *pmip_cache_alloc(int type)
{
    pmip_entry_t *tmp;
    tmp = malloc(sizeof(pmip_entry_t));

    if (tmp == NULL) {
        dbg("NO memory allocated for PMIP cache entry..\n");
        return NULL;
    }

    memset(tmp, 0, sizeof(*tmp));

    if (pthread_rwlock_init(&tmp->lock, NULL)) {
        free(tmp);
        return NULL;
    }
    INIT_LIST_HEAD(&tmp->tqe.list);
    tmp->type = type;
    dbg("PMIP cache entry is allocated..\n");
    return tmp;
}
//---------------------------------------------------------------------------------------------------------------------
static int __pmipcache_insert(pmip_entry_t * bce)
{
    int ret;
    ret = hash_add(&g_pmip_hash, bce, &bce->our_addr, &bce->mn_hw_address);
    if (ret) {
        return ret;
    }
    g_pmip_cache_count++;
    dbg("PMIP cache entry is inserted for: %x:%x:%x:%x:%x:%x:%x:%x <-> %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&bce->our_addr), NIP6ADDR(&bce->mn_hw_address));
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_cache_start(pmip_entry_t * bce)
{
    dbg("PMIP cache start is initialized add task pmip_timer_bce_expired_handler in %d seconds\n", bce->lifetime.tv_sec);
    struct timespec expires;
    clock_gettime(CLOCK_REALTIME, &bce->add_time);
    tsadd(bce->add_time, bce->lifetime, expires);

    add_task_abs(&expires, &bce->tqe,  pmip_timer_bce_expired_handler);
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
pmip_entry_t *pmip_cache_add(pmip_entry_t * bce)
{
    int ret = 1;
    int mutex_return_code;
    assert(bce);
    bce->unreach = 0;
    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    if ((ret = __pmipcache_insert(bce)) != 0) {
        pthread_rwlock_unlock(&pmip_lock);
        dbg("WARNING: PMIP ENTRY NOT INSERTED..\n");
        return NULL;
    }
    dbg("Making Entry\n");
    dbg("PMIP cache entry for: %x:%x:%x:%x:%x:%x:%x:%x with type %d is added\n", NIP6ADDR(&bce->mn_hw_address), bce->type);
    bce->n_rets_counter = conf.MaxMessageRetransmissions;
    dbg("Retransmissions counter intialized: %d\n", bce->n_rets_counter);
    if (bce->type == BCE_PMIP) {
        pmip_cache_start(bce);
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    return bce;
}
//---------------------------------------------------------------------------------------------------------------------
pmip_entry_t *pmip_cache_get(const struct in6_addr * our_addr, const struct in6_addr * peer_addr)
{
    pmip_entry_t *bce;
    int mutex_return_code;

    assert(peer_addr && our_addr);
    mutex_return_code = pthread_rwlock_rdlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_rdlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    bce = hash_get(&g_pmip_hash, our_addr, peer_addr);
    if (bce) {
        mutex_return_code = pthread_rwlock_wrlock(&bce->lock);
        if (mutex_return_code != 0) {
            dbg("pthread_rwlock_wrlock(&bce->lock) %s\n", strerror(mutex_return_code));
        }
        dbg("PMIP cache entry is found for: %x:%x:%x:%x:%x:%x:%x:%x with type %d\n", NIP6ADDR(&bce->mn_hw_address), (bce->type));
    } else {
        mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
        if (mutex_return_code != 0) {
            dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
        }
        dbg("PMIP cache entry is NOT found for %x:%x:%x:%x:%x:%x:%x:%x <-> %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(our_addr), NIP6ADDR(peer_addr));
    }
    return bce;
}
//---------------------------------------------------------------------------------------------------------------------
void pmipcache_release_entry(pmip_entry_t * bce)
{
    int mutex_return_code;
    assert(bce);
    mutex_return_code = pthread_rwlock_unlock(&bce->lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&bce->lock) %s\n", strerror(mutex_return_code));
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_cache_exists(const struct in6_addr *our_addr, const struct in6_addr *peer_addr)
{
    pmip_entry_t *bce;
    int type;
    bce = pmip_cache_get(our_addr, peer_addr);
    if (bce == NULL) {
        return -1;
    }
    dbg("PMIP cache entry does exist with type: %d\n", (bce->type));
    type = bce->type;
    pmipcache_release_entry(bce);
    return type;
}
//---------------------------------------------------------------------------------------------------------------------
void pmipcache_free(pmip_entry_t * bce)
{
/* This function should really return allocated space to free
* pool. */
    int mutex_return_code;
    mutex_return_code = pthread_rwlock_destroy(&bce->lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_destroy(&bce->lock) %s\n", strerror(mutex_return_code));
    }
    free(bce);
    dbg("PMIP cache entry is free\n");
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_bce_delete(pmip_entry_t * bce)
{
    int mutex_return_code;

    mutex_return_code = pthread_rwlock_wrlock(&bce->lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&bce->lock) %s\n", strerror(mutex_return_code));
    }
    del_task(&bce->tqe);
    if (bce->cleanup) {
        bce->cleanup(bce);
    }
    g_pmip_cache_count--;
    hash_delete(&g_pmip_hash, &bce->our_addr, &bce->mn_hw_address);
    mutex_return_code = pthread_rwlock_unlock(&bce->lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&bce->lock) %s\n", strerror(mutex_return_code));
    }
    pmipcache_free(bce);
    dbg("PMIP cache entry is deleted!\n");
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_cache_delete(const struct in6_addr *our_addr, const struct in6_addr *peer_addr)
{
    int mutex_return_code;
    pmip_entry_t *bce;
    mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    bce = hash_get(&g_pmip_hash, our_addr, peer_addr);
    if (bce) {
        pmip_bce_delete(bce);
    }
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_cache_iterate(int (*func) (void *, void *), void *arg)
{
    int err;
    int mutex_return_code;
    mutex_return_code = pthread_rwlock_rdlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_rdlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    err = hash_iterate(&g_pmip_hash, func, arg);
    mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
    }
    return err;
}
