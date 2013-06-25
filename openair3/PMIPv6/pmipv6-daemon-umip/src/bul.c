/*
 * $Id: bul.c 1.114 06/05/15 13:45:42+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 *
 * Author: Henrik Petander <petander@tcs.hut.fi>
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
#include <errno.h>
#include <netinet/ip6mh.h>
#include <syslog.h>

#include "bul.h"
#include "mn.h"
#include "util.h"
#include "xfrm.h"
#include "debug.h"
#include "retrout.h"
#ifdef ENABLE_VT
#include "vt.h"
#endif

#define BUL_DEBUG_LEVEL 1

#if BUL_DEBUG_LEVEL >= 1
#define BDBG dbg
#else
#define BDBG(...)
#endif

#define BUL_BUCKETS 32

static struct hash bul_hash;

struct bulentry *create_bule(const struct in6_addr *hoa,
			     const struct in6_addr *cn_addr)
{
	struct bulentry *bule;
	if ((bule = malloc(sizeof(*bule))) != NULL) {
		memset(bule, 0, sizeof(*bule));
		bule->hoa = *hoa;
		bule->last_coa = *hoa;
		bule->peer_addr = *cn_addr;
		INIT_LIST_HEAD(&bule->tqe.list);
		bule->seq = random();
	}
	return bule;
}

void free_bule(struct bulentry *bule)
{
	assert(bule != NULL);
	free(bule);
}

void dump_bule(void *bule, void *os)
{
	struct bulentry *e = (struct bulentry *)bule;
	FILE *out = (FILE *)os;

	if (e->type == BUL_ENTRY)
		fprintf(out, "== BUL_ENTRY ==\n");
	else if (e->type == NON_MIP_CN_ENTRY)
		fprintf(out, "== NON_MIP_CN_ENTRY ==\n");
	else if (e->type == UNREACH_ENTRY)
		fprintf(out, "== UNREACH_ENTRY ==\n");
	else 
		fprintf(out, "== Unknown BUL entry ==\n");

	fprintf(out, "Home address    %x:%x:%x:%x:%x:%x:%x:%x\n",
		NIP6ADDR(&e->hoa));
	fprintf(out, "Care-of address %x:%x:%x:%x:%x:%x:%x:%x\n",
		NIP6ADDR(&e->coa));
	fprintf(out, "CN address      %x:%x:%x:%x:%x:%x:%x:%x\n",
		NIP6ADDR(&e->peer_addr));
	fprintf(out, " lifetime = %ld, ", e->lifetime.tv_sec);
	fprintf(out, " delay = %ld\n", tstomsec(e->delay));
	fprintf(out, " flags: ");
	if (e->flags & IP6_MH_BU_HOME)
		fprintf(out, "IP6_MH_BU_HOME ");
	if (e->flags & IP6_MH_BU_ACK)
		fprintf(out, "IP6_MH_BU_ACK ");
	if (e->flags & IP6_MH_BU_LLOCAL)
		fprintf(out, "IP6_MH_BU_LLOCAL ");
	if (e->flags & IP6_MH_BU_KEYM)
		fprintf(out, "IP6_MH_BU_KEYM ");
	if (e->flags & IP6_MH_BU_MR)
		fprintf(out, "IP6_MH_BU_MR");

	fprintf(out, "\n");
	fflush(out);
}

/**
 * bul_get - returns a binding update list entry
 * @hinfo: home address info, optional if our_addr is present
 * @our_addr: local address (home address)
 * @peer_addr: address of CN
 *
 * Returns non-null entry on success and null on failure. Caller must
 * call del_task and add_task, if lifetime of the entry is changed.
 **/
struct bulentry *bul_get(struct home_addr_info *hinfo,
			 const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr)
{
	struct bulentry *bule;

	assert(hinfo || our_addr);

	if (hinfo)
		bule = hash_get(&hinfo->bul, NULL, peer_addr);
	else bule = hash_get(&bul_hash, our_addr, peer_addr);
	return bule;
}

/*
 * need to be separated into two phase:
 * phase 1: before sending BU
 * 		add policy/state for BU
 * phase 2: after sending BU
 * 		add policy/state for RO
 */
void bul_update_timer(struct bulentry *bule)
{
	struct timespec timer_expire;
	tsadd(bule->delay, bule->lastsent, timer_expire);
	dbg("Updating timer\n");
	dbg_func(bule, dump_bule);
	add_task_abs(&timer_expire, &bule->tqe, bule->callback);
}

void bul_update_expire(struct bulentry *bule)
{

	if (bule->type != BUL_ENTRY)
		bule->expires = bule->lastsent;
	else if (tsisset(bule->lifetime))
		tsadd(bule->lastsent, bule->lifetime, bule->expires);
	else {
		/* Deregistration entry, expires after 420 seconds...*/
		tsadd(DEREG_BU_LIFETIME_TS, bule->lastsent, bule->expires);
	}
}

/* Adds bul entry to both hashes and adds a timer for expiry / resend. 
   Caller must fill all non-private fields of bule */
int bul_add(struct bulentry *bule)
{
	int ret = 0;
	struct timespec timer_expire;
	struct home_addr_info *hai = bule->home;

	assert(bule && tsisset(bule->lifetime) && hai);
	
	if ((ret = hash_add(&bul_hash, bule, &bule->hoa, &bule->peer_addr)) < 0)
		return ret;
	if ((ret = hash_add(&hai->bul, bule, NULL, &bule->peer_addr)) < 0)
		goto bul_free;

	clock_gettime(CLOCK_REALTIME, &bule->lastsent);
	if (bule->type == BUL_ENTRY) {
		if ((ret = pre_bu_bul_update(bule)) < 0)
			goto home_bul_free;
	} else if (bule->type == NON_MIP_CN_ENTRY) {
		if (bule->flags & IP6_MH_BU_HOME) {
			if (xfrm_block_hoa(hai) < 0 ||
			    (hai->mob_rtr && xfrm_block_ra(hai) < 0))
				goto home_bul_free;
		}
	}
	tsadd(bule->delay, bule->lastsent, timer_expire);
	dbg("Adding bule\n");
	dbg_func(bule, dump_bule);
	add_task_abs(&timer_expire, &bule->tqe, bule->callback);
	return 0;
home_bul_free:
	hash_delete(&hai->bul, &bule->hoa, &bule->peer_addr);
bul_free:
	hash_delete(&bul_hash, &bule->hoa, &bule->peer_addr);
	return ret; 
}

/* bul_delete - deletes a bul entry */
void bul_delete(struct bulentry *bule)
{
	struct home_addr_info *hai = bule->home;

	del_task(&bule->tqe);
	hash_delete(&bul_hash, &bule->hoa, &bule->peer_addr);
	hash_delete(&hai->bul, NULL, &bule->peer_addr);

	if (!IN6_ARE_ADDR_EQUAL(&bule->hoa, &bule->coa)) {
		bule->last_coa = bule->coa;
		bule->coa = bule->hoa;
		bule->coa_changed = 1;
	}
	if (bule->type == BUL_ENTRY) {
		xfrm_del_bule(bule);
		if (!(bule->flags & IP6_MH_BU_HOME))
			mn_rr_delete_bule(bule);
	}
	if (bule->flags & IP6_MH_BU_HOME) {
		if (bule->type == UNREACH_ENTRY) {
			pthread_mutex_lock(&hai->ha_list.c_lock);
			if (IN6_ARE_ADDR_EQUAL(&bule->peer_addr, 
					       &hai->ha_list.last_ha))
				hai->ha_list.last_ha = in6addr_any;
			pthread_mutex_unlock(&hai->ha_list.c_lock);
		} else {
			if (hai->home_block & HOME_LINK_BLOCK)
				xfrm_unblock_link(hai);
			if (hai->home_block & HOME_ADDR_BLOCK)
				xfrm_unblock_hoa(hai);
			if (hai->home_block & NEMO_RA_BLOCK)
				xfrm_unblock_ra(hai);
			if (hai->home_block & NEMO_FWD_BLOCK)
				xfrm_unblock_fwd(hai);
		}
	}
	while (bule->ext_cleanup)
		bule->ext_cleanup(bule);
	dbg("Deleting bule\n");
	dbg_func(bule, dump_bule);
	free_bule(bule);
}

/* bul_init - initializes global bul */
int bul_init(void)
{
	int ret;

	ret = hash_init(&bul_hash, DOUBLE_ADDR, BUL_BUCKETS);

#ifdef ENABLE_VT
	if (ret < 0)
		return ret;

	ret = vt_bul_init();
#endif

	return ret;
}

/* bul_home_init - initializes a bul */
int bul_home_init(struct home_addr_info *home)
{
	return hash_init(&home->bul, SINGLE_ADDR, BUL_BUCKETS);
}

/* bule_cleanup - cleans up a bulentry */
static int bule_cleanup(void *vbule, void *vbul)
{
	if (vbul == NULL)
		BUG("bul_hash should be empty!\n");
	bul_delete(vbule);
	return 0;
}

/* bul_home_cleanup - cleans up a bul 
 * @bul: binding update list to clean up
 */
void bul_home_cleanup(struct hash *bul)
{
	hash_iterate(bul, bule_cleanup, bul);	
	hash_cleanup(bul);
}

void bul_flush(void)
{
	hash_iterate(&bul_hash, bule_cleanup, NULL);	
}

/* bul_cleanup - cleans up global bul */
void bul_cleanup(void)
{
	hash_cleanup(&bul_hash);
}

/* bul_iterate - iterates through binding update list calling func for
 * every entry. 
 * @func: function to be called for every entry, @func
 * takes a void cast bulentry as its first argument and @arg as its
 * second.  
 * @arg: second argument with which @func is called for every
 * bul entry 
 */
int bul_iterate(struct hash *h, int (* func)(void *, void *), void *arg)
{
	struct hash *tmp = h ? h : &bul_hash;
	return hash_iterate(tmp, func, arg);
}
