/*
 * $Id: retrout.c 1.141 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors:
 *  Henrik Petander <petander@tcs.hut.fi>
 *  Antti Tuominen <anttit@tcs.hut.fi>
 *  Ville Nuorvala <vnuorval@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/ip6mh.h>
#ifdef HAVE_LIBCRYPTO
#include <openssl/rand.h>
#else
#include "crypto.h"
#endif

#include "debug.h"
#include "mipv6.h"
#include "util.h"
#include "bul.h"
#include "mh.h"
#include "xfrm.h"
#include "mn.h"
#include "keygen.h"
#include "retrout.h"
#include "conf.h"
#include "statistics.h"

#define RR_DEBUG_LEVEL 1

#if RR_DEBUG_LEVEL >= 1
#define RRDBG dbg
#else 
#define RRDBG(...) 
#endif /* RRDBG */

struct rrlentry {
	struct tq_elem tqe;            /* Timer queue entry */
	struct in6_addr peer;     /* CN address */
	struct in6_addr own1;
	struct in6_addr own2;
	int iif;

	struct timespec lastsent;
	struct timespec expires;
	struct timespec delay;	       /* call back time */

	int resend_count; /* Number of consecutive [HC]oTI's sent */

	void (*callback)(struct tq_elem *);

	uint8_t type; /* HOT entry / COT entry */
	uint8_t wait;
	uint16_t index;
	uint8_t cookie[8];
	uint8_t kgen_token[8];

	struct list_head home_addrs; /* List of HoAs for CoT entry */
};

enum {
	COT_ENTRY,
	HOT_ENTRY
};

struct hash rrl_hash;

const struct timespec mn_test_init_delay_ts = { MN_TEST_INIT_DELAY, 0 };
#define MN_TEST_INIT_DELAY_TS mn_test_init_delay_ts

static void ti_resend(struct tq_elem *tqe);

static int rrl_init(void)
{
	return hash_init(&rrl_hash, DOUBLE_ADDR, 32);
}

static int rre_co_add_hoa(struct rrlentry *cote, struct in6_addr *addr)
{
	struct list_head *list;
	struct addr_holder *addr_c;

	list_for_each(list, &cote->home_addrs) {
		addr_c = list_entry(list, struct addr_holder, list);
		if (IN6_ARE_ADDR_EQUAL(addr, &addr_c->addr)) 
			return 1;
	}
	addr_c = malloc(sizeof(*addr_c));
	if (!addr_c) 
		return -1;
	addr_c->addr = *addr;
	list_add(&addr_c->list, &cote->home_addrs);

	return 0;
}
static void rrl_delete(struct rrlentry *rre);

static struct rrlentry *rre_create(int type, struct in6_addr *own1, int iif,
				   struct in6_addr *peer,
				   struct in6_addr *own2)
{
	struct rrlentry *rre;

	dbg("%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(own1));
	dbg("%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(peer));

	rre = malloc(sizeof(*rre));

	if (!rre) {
		RRDBG("Malloc failed\n");
		return NULL;
	}
	memset(rre, 0, sizeof(*rre));
	INIT_LIST_HEAD(&rre->tqe.list);
	INIT_LIST_HEAD(&rre->home_addrs);

	if (type == COT_ENTRY) {
		if (rre_co_add_hoa(rre, own2) < 0) {
			free(rre);
			return NULL;
		}
	}
	rre->type = type;
	rre->own1 = *own1;
	rre->peer = *peer;
	rre->own2 = *own2;
	rre->iif = iif;

	if (hash_add(&rrl_hash, rre, &rre->own1, &rre->peer) < 0) {
		rrl_delete(rre);
		return NULL;
	}
	return rre;
}

static struct rrlentry *rrl_get(int type, const struct in6_addr *our_addr,
				const struct in6_addr *peer_addr)
{
	struct rrlentry *rre;

	dbg("%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(our_addr));
	dbg("%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(peer_addr));

	assert(our_addr);

	rre = (struct rrlentry *)hash_get(&rrl_hash, our_addr, peer_addr);

	if (rre != NULL && rre->type != type)
		return NULL;

	return rre;
}

static int rrl_iterate(int (* func)(void *, void *), void *arg)
{
	return hash_iterate(&rrl_hash, func, arg);
}

static void rrl_delete(struct rrlentry *rre)
{
	assert(rre);

	TRACE;

	del_task(&rre->tqe);
	hash_delete(&rrl_hash, &rre->own1, &rre->peer);

	if (rre->type == COT_ENTRY) {
		struct list_head *list, *n;

		list_for_each_safe(list, n, &rre->home_addrs) {
			list_del(list);
			free(list_entry(list, struct addr_holder, list));
		}
	}

	free(rre);
}

static void rrl_delete_co_hoa(const struct in6_addr *coa,
			      const struct in6_addr *peer,
			      const struct in6_addr *hoa)
{
	struct rrlentry *rre = rrl_get(COT_ENTRY, coa, peer);

	TRACE;

	if (rre != NULL) {
		struct list_head *l, *n;
		list_for_each_safe(l, n, &rre->home_addrs) {
			struct addr_holder *ah;
			ah = list_entry(l, struct addr_holder, list);

			if (!IN6_ARE_ADDR_EQUAL(hoa, &ah->addr))
				continue;

			list_del(l);
			free(ah);
		}
		if (list_empty(&rre->home_addrs))
			rrl_delete(rre);
	}
}

static void rrl_update_timer(struct rrlentry *rre)
{
	struct timespec timer_expire;

	tsadd(rre->delay, rre->lastsent, timer_expire);
	add_task_abs(&timer_expire, &rre->tqe, rre->callback);
}

static int rre_dump(void *entry, void *os)
{
	struct rrlentry *e = (struct rrlentry *)entry;
	FILE *out = (FILE *)os;
	char buf[IF_NAMESIZE + 1];
	char *dev = if_indextoname(e->iif, buf);
	struct timespec ts, ts_now;

	fprintf(out, "== Return Routability Entry (%s) == \n",
		e->type == COT_ENTRY ? "COT_ENTRY" : "HOT_ENTRY");

	fprintf(out, " %s %x:%x:%x:%x:%x:%x:%x:%x\n",
		(e->type == COT_ENTRY) ? "CoA" : "HoA",
		NIP6ADDR(&e->own1));
	
	fprintf(out, " CN  %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&e->peer));

	if (e->type == COT_ENTRY) {
		struct list_head *l;
		list_for_each(l, &e->home_addrs) {
			struct addr_holder *a;
			a = list_entry(l, struct addr_holder, list);
			fprintf(out, " HoA %x:%x:%x:%x:%x:%x:%x:%x\n",
				NIP6ADDR(&a->addr));
		}
	} else {
		fprintf(out, " CoA %x:%x:%x:%x:%x:%x:%x:%x\n",
			NIP6ADDR(&e->own2));
	}

	if (!dev || strlen(dev) == 0)
		fprintf(out, " Interface (%d)\n", e->iif);
	else
		fprintf(out, " Interface %s\n", dev);

	clock_gettime(CLOCK_REALTIME, &ts_now);

	fprintf(out, " resend %d", e->resend_count);
	fprintf(out, " delay %ld (after %ld seconds)", e->delay.tv_sec,
		e->lastsent.tv_sec + e->delay.tv_sec - ts_now.tv_sec);

	tssub(e->expires, ts_now, ts);
	fprintf(out, " expires in %ld seconds\n", ts.tv_sec);

	fflush(out);

	return 0;
}

void rrl_dump(FILE *os)
{
	rrl_iterate(rre_dump, os);
}

static inline int cookiecmp(const uint8_t *cookie_a, const uint8_t *cookie_b)
{
	return memcmp(cookie_a, cookie_b, 8);
}

static void mn_send_hoti(struct in6_addr *hoa, struct in6_addr *peer,
			 uint8_t *cookie, int oif)
{
	struct iovec iov;
	struct ip6_mh_home_test_init *hti;
	struct in6_addr_bundle out;

	out.src = hoa;
	out.dst = peer;
	out.local_coa = NULL;
	out.remote_coa = NULL;

	hti = mh_create(&iov, IP6_MH_TYPE_HOTI);
	if (!hti)
		return;

#ifdef HAVE_LIBCRYPTO
	RAND_pseudo_bytes((uint8_t *)hti->ip6mhhti_cookie, 8);
#else
	random_bytes((uint8_t *)hti->ip6mhhti_cookie, 8);
#endif
	cookiecpy(cookie, hti->ip6mhhti_cookie);

	mh_send(&out, &iov, 1, NULL, oif);
	free(iov.iov_base);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_HOTI);
}

static void mn_send_coti(struct in6_addr *coa, struct in6_addr *peer,
			 uint8_t *cookie, int oif)
{
	struct iovec iov;
	struct ip6_mh_careof_test_init *cti;
	struct in6_addr_bundle out;

	out.src = coa;
	out.dst = peer;
	out.local_coa = NULL;
	out.remote_coa = NULL;

	cti = mh_create(&iov, IP6_MH_TYPE_COTI);
	if (!cti)
		return;

#ifdef HAVE_LIBCRYPTO
	RAND_pseudo_bytes((uint8_t *)cti->ip6mhcti_cookie, 8);
#else
	random_bytes((uint8_t *)cti->ip6mhcti_cookie, 8);
#endif
	cookiecpy(cookie, cti->ip6mhcti_cookie);

	mh_send(&out, &iov, 1, NULL, oif);
	free(iov.iov_base);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_COTI);
}

/* Resend HoTI or CoTI, if we haven't got HoT or CoT */ 
static void ti_resend(struct tq_elem *tqe)
{
	struct rrlentry *rre;

	pthread_rwlock_wrlock(&mn_lock);

	if (task_interrupted()) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	rre = tq_data(tqe, struct rrlentry, tqe);

	clock_gettime(CLOCK_REALTIME, &rre->lastsent);

	if (rre->type == COT_ENTRY)
		mn_send_coti(&rre->own1, &rre->peer, rre->cookie, rre->iif);
	else
		mn_send_hoti(&rre->own1, &rre->peer, rre->cookie, rre->iif);

	/* exponential backoff */
	tsadd(rre->delay, rre->delay, rre->delay); /* 2 * rre->delay */
	rre->delay = tsmin(rre->delay, MAX_BINDACK_TIMEOUT_TS);
	rre->resend_count++;
	rrl_update_timer(rre);

	pthread_rwlock_unlock(&mn_lock);
}

static void rre_reset(struct rrlentry *rre, struct timespec *now)
{
	rre->lastsent = *now;
	rre->wait = 1;
	rre->resend_count = 0;
	rre->delay = MN_TEST_INIT_DELAY_TS;
	rre->callback = ti_resend;
	rre->expires = rre->lastsent;
}

/* Renew HoTI before home keygen token expires to optimize handoff
 * performance, if kernel bule has been recently used
 */
static void mn_rr_homekgt_refresh(struct tq_elem *tqe)
{
	struct rrlentry *rre_ho;
	struct timespec now;
	long last_used;

	pthread_rwlock_wrlock(&mn_lock);

	if (task_interrupted()) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	rre_ho = tq_data(tqe, struct rrlentry, tqe);

	clock_gettime(CLOCK_REALTIME, &now);

	last_used = mn_bule_xfrm_last_used(&rre_ho->peer, &rre_ho->own1, &now);

	if (last_used >= 0 && last_used < MN_RO_RESTART_THRESHOLD) {
		rre_reset(rre_ho, &now);
		mn_send_hoti(&rre_ho->own1, &rre_ho->peer,
			     rre_ho->cookie, rre_ho->iif);
		rrl_update_timer(rre_ho);
	} else
		rrl_delete(rre_ho);

	pthread_rwlock_unlock(&mn_lock);
}

/* Renew CoTI before home keygen token expires to optimize handoff
 * performance, if kernel bule has been recently used
 */
static void mn_rr_careofkgt_refresh(struct tq_elem *tqe)
{
	struct rrlentry *rre_co;
	struct timespec now;
	struct list_head *l;
	int refresh = 0;
	pthread_rwlock_wrlock(&mn_lock);

	if (task_interrupted()) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	rre_co = tq_data(tqe, struct rrlentry, tqe);

	clock_gettime(CLOCK_REALTIME, &now);

	list_for_each(l, &rre_co->home_addrs) {
		struct addr_holder *ah;
		long last_used;

		ah = list_entry(l, struct addr_holder, list);

		last_used = xfrm_last_used(&rre_co->peer, &ah->addr,
					   IPPROTO_DSTOPTS, &now);
		
		if (last_used >= 0 && last_used < MN_RO_RESTART_THRESHOLD) {
			rre_reset(rre_co, &now);
			mn_send_coti(&rre_co->own1, &rre_co->peer,
				     rre_co->cookie, rre_co->iif);
			rrl_update_timer(rre_co);
			refresh = 1;
			break;
		}
	}
	if (!refresh)
		rrl_delete(rre_co);

	pthread_rwlock_unlock(&mn_lock);
}

/* Checks if COT/HOT token is valid  */
static inline int mn_rr_token_valid(struct rrlentry *rre, struct timespec *now)
{
	if (!rre->wait && tsbefore(rre->expires, *now))
		return 1;

	return 0;
}

/** 
 * mn_rr_cond_start_hot - send HoTI, if it is necessary
 * bule: bul entry for RO binding created in start_ro  
 * uncond: set this to override all freshness checks and send HoTI in any case
 */
static int mn_rr_cond_start_hot(struct bulentry *bule, int uncond)
{
	struct rrlentry *rre;
	struct timespec now;

	assert(bule->type == BUL_ENTRY);

	rre = rrl_get(HOT_ENTRY, &bule->hoa, &bule->peer_addr);

	clock_gettime(CLOCK_REALTIME, &now);

	if (rre != NULL) {
		rre->own2 = bule->coa;
		if (!uncond && mn_rr_token_valid(rre, &now)) {
			RRDBG("Home keygen token valid, no HoTI\n");
			return 0;
		}
		if (rre->wait) {
			RRDBG("HoTI already sent\n");
			return 2;
		}
	} else {
		rre = rre_create(HOT_ENTRY, &bule->hoa, bule->home->hoa.iif,
				 &bule->peer_addr, &bule->coa);
		if (rre == NULL)
			return 0;
	}
	RRDBG("Home keygen token not valid, send HoTI\n");
	rre_reset(rre, &now);
	mn_send_hoti(&rre->own1, &rre->peer, rre->cookie, bule->home->hoa.iif);
	rrl_update_timer(rre);
	return 1;
}

/**
 * mn_rr_cond_start_cot - send CoTI, if necessary 
 * @bule: RO bul entry
 * @coa: Care-of address for CoT
 * @ifindex: interface index for CoA
 * @uncond: send CoT even if current kgen token is fresh
 *
 * Function manages sending of CoTI in a handoff and also changes the
 * CoA in RO bul entry.
 **/
static int mn_rr_cond_start_cot(struct bulentry *bule, int uncond)
{
	struct rrlentry *rre;
	struct timespec now;

	assert(bule->type == BUL_ENTRY);

	rre = rrl_get(COT_ENTRY, &bule->coa, &bule->peer_addr);

	clock_gettime(CLOCK_REALTIME, &now);

	if (rre != NULL) {
		if (!uncond && mn_rr_token_valid(rre, &now)) {
			RRDBG("Care-of keygen token valid, no CoTI\n");
			return 0;
		}
		if (rre->wait) {
			RRDBG("CoTI already sent\n");
			return 2;
		}
	} else {
		rre = rre_create(COT_ENTRY, &bule->coa, bule->if_coa,
				 &bule->peer_addr, &bule->hoa);
		if (rre == NULL)
			return 0;
	}
	if (rre_co_add_hoa(rre, &bule->hoa) < 0) {
		RRDBG("Failed to add HoA to CoT entry\n");
		return 0;
	}
	RRDBG("Care-of keygen token not valid, send CoTI\n"); 
	rre_reset(rre, &now);
	mn_send_coti(&rre->own1, &rre->peer, rre->cookie, rre->iif);
	rrl_update_timer(rre);
	return 1;
}

void mn_rr_force_refresh(struct bulentry *bule)
{
	if (bule->rr.state == RR_H_EXPIRED || bule->rr.state == RR_EXPIRED)
		mn_rr_cond_start_hot(bule, 1);
	if (bule->rr.state == RR_C_EXPIRED || bule->rr.state == RR_EXPIRED)
		mn_rr_cond_start_cot(bule, 1);
	bule->rr.state = RR_STARTED;
}

void mn_rr_refresh(struct bulentry *bule)
{
	struct rrlentry *rre_ho, *rre_co;

	if (bule->coa_changed > 0)
		rrl_delete_co_hoa(&bule->last_coa,
				  &bule->peer_addr, &bule->hoa);

	if (mn_rr_cond_start_hot(bule, 0))
		bule->rr.state = RR_STARTED;
	if (!bule->dereg && mn_rr_cond_start_cot(bule, 0))
		bule->rr.state = RR_STARTED;

	if (bule->rr.state == RR_STARTED || bule->rr.state == RR_NOT_STARTED)
		return;

	rre_ho = rrl_get(HOT_ENTRY, &bule->hoa, &bule->peer_addr);

	if (bule->dereg) {
		rr_mn_calc_Kbm(rre_ho->kgen_token, NULL, bule->Kbm);
		bule->rr.state = RR_READY;
		bule->rr.co_ni = 0;
		return;
	}

	rre_co = rrl_get(COT_ENTRY, &bule->coa, &bule->peer_addr);

	rr_mn_calc_Kbm(rre_ho->kgen_token, rre_co->kgen_token, bule->Kbm);

	bule->rr.state = RR_READY;
}

static int _mn_rr_delete_co(void *vrre, void *vcoa)
{
	struct rrlentry *rre = vrre;
	struct in6_addr *coa = vcoa;

	if (rre->type == COT_ENTRY && IN6_ARE_ADDR_EQUAL(&rre->own1, coa))
		rrl_delete(rre);
	return 0;
}

void mn_rr_delete_co(struct in6_addr *coa)
{
	rrl_iterate(_mn_rr_delete_co, coa);
}

void mn_rr_delete_bule(struct bulentry *e)
{
	struct rrlentry *rre_ho;

	if (e->coa_changed > 0)
		rrl_delete_co_hoa(&e->last_coa, &e->peer_addr, &e->hoa);

	rre_ho = rrl_get(HOT_ENTRY, &e->hoa, &e->peer_addr);
	if (rre_ho != NULL)
		rrl_delete(rre_ho);
}

int mn_rr_error_check(const struct in6_addr *own,
		      const struct in6_addr *peer,
		      struct in6_addr *hoa)
{
	struct rrlentry *rre = hash_get(&rrl_hash, own, peer);

	if (rre == NULL || !rre->wait)
		return 0;

	if (rre->type == HOT_ENTRY) {
		*hoa = rre->own1;
		return 1;
	} else if (!list_empty(&rre->home_addrs)) {
		/* just return the first available HoA */
		struct addr_holder *ah;
		ah = list_entry(rre->home_addrs.next,
				struct addr_holder, list);
		*hoa = ah->addr;
		return 1;
	}
	return 0;
}

static void mn_recv_cot(const struct ip6_mh *mh, ssize_t len,
			const struct in6_addr_bundle *in,
			__attribute__ ((unused)) int iif)
{
	struct in6_addr *cn_addr = in->src;
	struct in6_addr *co_addr = in->dst;
	uint8_t *cookie;
	uint8_t *keygen;
	uint16_t index;
	struct rrlentry *rre_ho;
	struct rrlentry *rre_co;
	struct ip6_mh_careof_test *ct;
	struct list_head *list, *n;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_COT);

	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_careof_test) ||
	    in->remote_coa)
		return;

	ct = (struct ip6_mh_careof_test *)mh;
	cookie = (uint8_t *)ct->ip6mhct_cookie;
	keygen = (uint8_t *)ct->ip6mhct_keygen;
	index = ntohs(ct->ip6mhct_nonce_index);

	pthread_rwlock_wrlock(&mn_lock);
	rre_co = rrl_get(COT_ENTRY, co_addr, cn_addr);

	if (rre_co == NULL || cookiecmp(rre_co->cookie, cookie)) {
		RRDBG("Got CoT, but no corresponding RR entry\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}

	if (!rre_co->wait) {
		RRDBG("Got unexpected CoT\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}

	rre_co->wait = 0;
	rre_co->resend_count = 0;
	memcpy(rre_co->kgen_token, keygen, sizeof(rre_co->kgen_token));
	rre_co->index = index;
	/* Send BU to CN for every home address waiting for the CoT */
	list_for_each_safe(list, n, &rre_co->home_addrs) {
		struct bulentry *bule = NULL;
		struct addr_holder *ah;

		ah = list_entry(list, struct addr_holder, list);
		bule = bul_get(NULL, &ah->addr, cn_addr);

		if (bule == NULL || bule->type != BUL_ENTRY ||
		    !IN6_ARE_ADDR_EQUAL(&rre_co->own1, &bule->coa)) {
			list_del(list);
			free(ah);
			continue;
		}

		bule->rr.co_ni = index;

		if (!bule->do_send_bu) {
			/* This happens when we automatically refresh home
			 * keygen token while binding still in use */
			continue;
		}
		rre_ho = rrl_get(HOT_ENTRY, &ah->addr, cn_addr);

		if (rre_ho == NULL || rre_ho->wait) {
			RRDBG("Still waiting for HoT, not sending BU\n");
			continue;
		}
		RRDBG("Got CoT and found RR entry for home address\n");
		bule->rr.state = RR_READY;
		bule->rr.ho_ni = rre_ho->index;
		rr_mn_calc_Kbm(rre_ho->kgen_token, keygen, bule->Kbm);
		mn_send_cn_bu(bule);
	}
	if (list_empty(&rre_co->home_addrs))
		rrl_delete(rre_co);
	else {
		rre_co->callback = mn_rr_careofkgt_refresh;
		rre_co->delay = MAX_TOKEN_LIFETIME_TS;
		tsadd(rre_co->delay, rre_co->lastsent, rre_co->expires);
		rrl_update_timer(rre_co);
	}
	pthread_rwlock_unlock(&mn_lock);
}

static struct mh_handler mn_cot_handler = {
	.recv = mn_recv_cot,
};

/* mh_hot_recv - handles MH HoT msg */
static void mn_recv_hot(const struct ip6_mh *mh, ssize_t len,
			const struct in6_addr_bundle *in,
			__attribute__ ((unused)) int iif)
{
	struct in6_addr *cn_addr = in->src;
	struct in6_addr *home_addr = in->dst;
	uint8_t *cookie;
	uint8_t *keygen;
	uint16_t index;
	struct rrlentry *rre_ho;
	struct rrlentry *rre_co = NULL;
	struct bulentry *bule = NULL;
	struct ip6_mh_home_test *ht;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_HOT);

	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_home_test) ||
	    in->remote_coa)
		return;

	ht = (struct ip6_mh_home_test *)mh;
	cookie = (uint8_t *)ht->ip6mhht_cookie;
	keygen = (uint8_t *)ht->ip6mhht_keygen;
	index = ntohs(ht->ip6mhht_nonce_index);

	pthread_rwlock_wrlock(&mn_lock);

	rre_ho = rrl_get(HOT_ENTRY, home_addr, cn_addr);

	if (rre_ho == NULL || cookiecmp(rre_ho->cookie, cookie)) {
		RRDBG("Got HoT, but no corresponding RR entry\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}

	if (!rre_ho->wait) {
		RRDBG("Got unexpected HoT\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	bule = bul_get(NULL, home_addr, cn_addr);

	if (bule == NULL || bule->type != BUL_ENTRY) {
		rrl_delete(rre_ho);
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	rre_ho->wait = 0;
	rre_ho->resend_count = 0;
	memcpy(rre_ho->kgen_token, keygen, sizeof(rre_ho->kgen_token));
	rre_ho->index = index;
	bule->rr.ho_ni = index;

	if (bule->dereg) {
		/* Dereg BUL entry waiting for RR_READY */
		RRDBG("Got HoT\n");
		if (bule->do_send_bu) {
			bule->rr.state = RR_READY;
			bule->rr.co_ni = 0;
			rr_mn_calc_Kbm(keygen, NULL, bule->Kbm);
			mn_send_cn_bu(bule);
		}
	} else {
		rre_co = rrl_get(COT_ENTRY, &rre_ho->own2, cn_addr);
		if (rre_co == NULL || rre_co->wait)
			RRDBG("Still waiting for CoT, not sending BU\n");
		else if (bule->do_send_bu) {
			RRDBG("Got HoT and found RR entry for care-of address\n");
			/* Foreign Reg BU case */
			bule->rr.state = RR_READY;
			bule->rr.co_ni = rre_co->index;
			rr_mn_calc_Kbm(keygen, rre_co->kgen_token, bule->Kbm);
			mn_send_cn_bu(bule);
		}
	}
	rre_ho->callback = mn_rr_homekgt_refresh;
	rre_ho->delay = MAX_TOKEN_LIFETIME_TS;
	tsadd(rre_ho->delay, rre_ho->lastsent, rre_ho->expires);
	rrl_update_timer(rre_ho);
	pthread_rwlock_unlock(&mn_lock);
}

static struct mh_handler mn_hot_handler = {
	.recv = mn_recv_hot,
};

int rr_init(void)
{
	if (rrl_init() < 0)
		return -1;
	mh_handler_reg(IP6_MH_TYPE_COT, &mn_cot_handler);
	mh_handler_reg(IP6_MH_TYPE_HOT, &mn_hot_handler);
	return 0;
}

static int rre_cleanup(void *vbule, __attribute__ ((unused)) void *dummy)
{
	BUG("rrl_hash should be empty");
	rrl_delete(vbule);
	return 0;
}

void rr_cleanup(void)
{
	mh_handler_dereg(IP6_MH_TYPE_HOT, &mn_hot_handler);
	mh_handler_dereg(IP6_MH_TYPE_COT, &mn_cot_handler);
	pthread_rwlock_wrlock(&mn_lock);
	rrl_iterate(rre_cleanup, NULL);
	pthread_rwlock_unlock(&mn_lock);
	hash_cleanup(&rrl_hash);
}
