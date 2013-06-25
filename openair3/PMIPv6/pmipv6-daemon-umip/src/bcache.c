/*
 * $Id: bcache.c 1.99 06/05/07 21:53:32+03:00 anttit@tcs.hut.fi $
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
#include <pthread.h>
#include <errno.h>

#include "debug.h"
#include "bcache.h"
#include "xfrm.h"
#include "util.h"
#include "tunnelctl.h"
#include "keygen.h"
#include "mh.h"
#include "cn.h"
#include "vt.h"
#include "prefix.h"

#define BCACHE_BUCKETS 32

static struct hash bc_hash;

pthread_rwlock_t bc_lock; /* Protects binding cache */

void dump_bce(void *bce, void *os)
{
	struct bcentry *e = (struct bcentry *)bce;
	FILE *out = (FILE *)os;

	fprintf(out, " == Binding Cache entry ");

	switch(e->type) {
	case BCE_NONCE_BLOCK:
		fprintf(out, "(NONCE_BLOCK)\n");
		break;
	case BCE_CACHED:
		fprintf(out, "(CACHED)\n");
		break;
	case BCE_HOMEREG:
		fprintf(out, "(HOMEREG)\n");
		break;
	default:
		fprintf(out, "(Unknown)\n");
	}
	fprintf(out, " Care-of address %x:%x:%x:%x:%x:%x:%x:%x\n",
		NIP6ADDR(&e->coa));
	fprintf(out, " Home address    %x:%x:%x:%x:%x:%x:%x:%x\n", 
	     NIP6ADDR(&e->peer_addr));
	fprintf(out, " Local address   %x:%x:%x:%x:%x:%x:%x:%x\n",
		NIP6ADDR(&e->our_addr));
	fprintf(out, " lifetime %ld\n ", e->lifetime.tv_sec);
	fprintf(out, " seqno %d\n", e->seqno);

	if (e->flags & IP6_MH_BA_MR) {
		struct list_head *list;
		int mnpcount = 0;

		/* MR registration type */
		fprintf(out, "MR Registration type: ");
		switch(e->nemo_type) {
		case BCE_NEMO_EXPLICIT:
			fprintf(out, "explicit.\n");
			break;
		case BCE_NEMO_IMPLICIT:
			fprintf(out, "implicit.\n");
			break;
		default:
			fprintf(out, "unknown.\n");
		}

		/* Mobile Network prefixes */
		fprintf(out, "MR Mobile network prefixes: ");
		list_for_each(list, &e->mob_net_prefixes) {
			struct prefix_list_entry *p;
			p = list_entry(list, struct prefix_list_entry, list);
			if (mnpcount)
				fprintf(out, "                            ");
			fprintf(out, "%x:%x:%x:%x:%x:%x:%x:%x/%d\n",
				NIP6ADDR(&p->ple_prefix), p->ple_plen);
			mnpcount++;
		}
		if (!mnpcount)
			fprintf(out, " none registered.\n");
	}

	fflush(out);
}

static void bce_delete(struct bcentry *bce, int flush);

/**
 * _expire - expire binding cache entry
 **/
static void _expire(struct tq_elem *tqe)
{	
	pthread_rwlock_wrlock(&bc_lock);
	if (!task_interrupted()) {
		struct bcentry *e = tq_data(tqe, struct bcentry, tqe);
		pthread_rwlock_wrlock(&e->lock);
		if (e->type == BCE_CACHED) {
			/* To do: MN needs to reverse tunnel this */
			mh_send_brr(&e->peer_addr, &e->our_addr);
			e->type = BCE_CACHE_DYING;
			add_task_rel(&CN_BRR_BEFORE_EXPIRY_TS,
				     &e->tqe, _expire);
			pthread_rwlock_unlock(&e->lock);
			pthread_rwlock_unlock(&bc_lock);
			return;
		}
		pthread_rwlock_unlock(&e->lock);
		bce_delete(e, 0);
	}
	pthread_rwlock_unlock(&bc_lock);
}
	
/**
 * bcache_alloc - allocate binding cache entry
 * @type: type of entry
 *
 * Allocates a new binding cache entry.  @type may be %BCE_HOMEREG or
 * %BCE_CACHED.  Returns allocated space for an entry or NULL if none
 * available.
 **/
struct bcentry *bcache_alloc(__attribute__ ((unused)) int type)
{
	struct bcentry *tmp;
	/* This function should really return space from a
	 * pre-allocated pool.  If pool is empty, but type is
	 * BCE_HOMEREG, already existing BCE_CACHED entry may be
	 * replaced. */
	tmp = malloc(sizeof(struct bcentry));

	if (tmp == NULL)
		return NULL;

	memset(tmp, 0, sizeof(*tmp));
	if (pthread_rwlock_init(&tmp->lock, NULL)) {
		free(tmp);
		return NULL;
	}
	INIT_LIST_HEAD(&tmp->tqe.list);
	INIT_LIST_HEAD(&tmp->mob_net_prefixes);
	return tmp;
}

/**
 * bcache_free - release allocated memory
 * @bce: BC entry to free
 *
 * Release allocated memory back to unused pool.
 **/
void bcache_free(struct bcentry *bce)
{
	/* This function should really return allocated space to free
	 * pool. */
	pthread_rwlock_destroy(&bce->lock);
	prefix_list_free(&bce->mob_net_prefixes);
	free(bce);
}

/**
 * bcache_get - returns a binding cache entry
 * @our_addr: our IPv6 address
 * @peer_addr: peer's IPv6 address
 *
 * Returns reference to non-null entry on success and null on failure.
 * If caller adjusts lifetime of entry, caller must call
 * bcache_update_expire() for BC entry to reschedule.  Caller must
 * call bcache_release_entry() after entry is not used anymore.
 **/
struct bcentry *bcache_get(const struct in6_addr *our_addr,
			   const struct in6_addr *peer_addr)
{
	struct bcentry *bce;

	assert(peer_addr && our_addr);

	pthread_rwlock_rdlock(&bc_lock);

	bce = hash_get(&bc_hash, our_addr, peer_addr);

	if (bce) 
		pthread_rwlock_wrlock(&bce->lock);
	else
		pthread_rwlock_unlock(&bc_lock);

	return bce;
}

/**
 * bcache_release_entry - unlocks a binding cache entry 
 **/
void bcache_release_entry(struct bcentry *bce)
{
	assert(bce);
	pthread_rwlock_unlock(&bce->lock);
	pthread_rwlock_unlock(&bc_lock);
}

/**
 * bce_type - get type of binding cache entry
 * @our_addr: our IPv6 address
 * @peer_addr: peer's IPv6 address
 *
 * Looks up entry from binding cache and returns its type.  If not
 * found, returns -%ENOENT.
 **/
int bce_type(const struct in6_addr *our_addr, const struct in6_addr *peer_addr)
{
        struct bcentry *bce;
        int type;

        bce = bcache_get(our_addr, peer_addr);

        if (bce == NULL)
                return -ENOENT;

        type = bce->type;
        bcache_release_entry(bce);

        return type;
}

static int __bcache_insert(struct bcentry *bce)
{
	int ret;

	ret = hash_add(&bc_hash, bce, &bce->our_addr, &bce->peer_addr);
	if (ret)
		return ret;

	return 0;
}

static int __bcache_start(struct bcentry *bce)
{
	struct timespec expires, tmp;

	tssub(bce->lifetime, CN_BRR_BEFORE_EXPIRY_TS, tmp);
	clock_gettime(CLOCK_REALTIME, &bce->add_time);
	tsadd(bce->add_time,
	      bce->type == BCE_HOMEREG ? bce->lifetime : tmp,
	      expires);
	add_task_abs(&expires, &bce->tqe, _expire); 
	xfrm_add_bce(&bce->our_addr, &bce->peer_addr, &bce->coa, 0);
	return 0;
}

/* Adds bce entry and adds a timer for expiry / resend.  Caller must
   fill all non-private fields of bce */
int bcache_add(struct bcentry *bce)
{
	int ret = 0;

	assert(bce);

	bce->unreach = 0;
	pthread_rwlock_wrlock(&bc_lock);

	if ((ret = __bcache_insert(bce)) != 0) {
		pthread_rwlock_unlock(&bc_lock);
		return ret;
	}
	__bcache_start(bce);

	pthread_rwlock_unlock(&bc_lock);

	return 0;
}

int bcache_add_homereg(struct bcentry *bce)
{
	int ret = 0;

	assert(bce);
	assert(bce->type == BCE_DAD);

	bce->unreach = 0;
	pthread_rwlock_wrlock(&bc_lock);

	ret = __bcache_insert(bce);

	pthread_rwlock_unlock(&bc_lock);

	return ret;
}

int bcache_complete_homereg(struct bcentry *bce)
{
	assert(bce);

	/* XXX: This function has no lock for BCE because
	 * bcache_update_expire() doesn't. Why?
	 */
	__bcache_start(bce);

	return 0;
}

int bcache_update_expire(struct bcentry *bce)
{
	struct timespec expires;

	clock_gettime(CLOCK_REALTIME, &bce->add_time);
	if (bce->type == BCE_HOMEREG)
		expires = bce->lifetime;
	else {
		bce->type = BCE_CACHED;
		tssub(bce->lifetime, CN_BRR_BEFORE_EXPIRY_TS, expires);
	}
	tsadd(expires, bce->add_time, expires);
	add_task_abs(&expires, &bce->tqe, _expire);	
	xfrm_add_bce(&bce->our_addr, &bce->peer_addr, &bce->coa, 1);

	return 0;
}

/**
 * bcache_delete - deletes a bul entry
 **/
void bcache_delete(const struct in6_addr *our_addr,
		   const struct in6_addr *peer_addr)
{
	struct bcentry *bce;
	pthread_rwlock_wrlock(&bc_lock);
	bce = hash_get(&bc_hash, our_addr, peer_addr);
	if (bce)
		bce_delete(bce, 0);
	pthread_rwlock_unlock(&bc_lock);
}

/**
 * bcache_init - initializes binding cache
 **/
int bcache_init(void)
{
	int ret;

	if (pthread_rwlock_init(&bc_lock, NULL))
		return -1;

	pthread_rwlock_wrlock(&bc_lock);
	ret = hash_init(&bc_hash, DOUBLE_ADDR, BCACHE_BUCKETS);
	pthread_rwlock_unlock(&bc_lock);

#ifdef ENABLE_VT
	if (ret < 0)
		return ret;

	ret = vt_bc_init();
#endif

	return ret;
}

static void bce_delete(struct bcentry *bce, int flush)
{
	pthread_rwlock_wrlock(&bce->lock);
	if (bce->type != BCE_DAD) {
		del_task(&bce->tqe);
		if (bce->type != BCE_NONCE_BLOCK)
			xfrm_del_bce(&bce->our_addr, &bce->peer_addr);
	}
	if (bce->cleanup)
		bce->cleanup(bce);
	if (!flush &&
	    (bce->type == BCE_CACHED || bce->type == BCE_CACHE_DYING)) {
		struct timespec minlft;
		if (!rr_cn_nonce_lft(bce->nonce_hoa, &minlft)) {
			bce->type = BCE_NONCE_BLOCK;
			add_task_abs(&minlft, &bce->tqe, _expire);
			pthread_rwlock_unlock(&bce->lock);
			return;
		}
	}
	hash_delete(&bc_hash, &bce->our_addr, &bce->peer_addr);
	pthread_rwlock_unlock(&bce->lock);
	bcache_free(bce);
}

/**
 * bce_cleanup - cleans up a bcentry
 **/
static int bce_cleanup(void *data, __attribute__ ((unused)) void *arg)
{
	bce_delete(data, 1);
	return 0;
}

void bcache_flush(void)
{
	pthread_rwlock_wrlock(&bc_lock); 
	hash_iterate(&bc_hash, bce_cleanup, NULL);
	pthread_rwlock_unlock(&bc_lock);
}

void bcache_cleanup(void)
{
	pthread_rwlock_wrlock(&bc_lock); 
	hash_cleanup(&bc_hash);
	pthread_rwlock_unlock(&bc_lock);
}
/**
 * bcache_iterate - apply function to every BC entry
 * @func: function to apply
 * @arg: extra data for @func
 *
 * Iterates through binding cache, calling @func for each entry.
 * Extra data may be passed to @func in @arg.  @func takes a bcentry
 * as its first argument and @arg as second argument.
 **/
int bcache_iterate(int (* func)(void *, void *), void *arg)
{
	int err;
	pthread_rwlock_rdlock(&bc_lock); 
	err = hash_iterate(&bc_hash, func, arg);
	pthread_rwlock_unlock(&bc_lock);
	return err;
}
