/*
 * $Id: mpdisc_ha.c 1.21 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors:
 *  Ville Nuorvala <vnuorval@tcs.hut.fi>,
 *  Jaakko Laine <jola@tcs.hut.fi>
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
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <netinet/ip6mh.h>
#include <errno.h>
#include <pthread.h>

#include "debug.h"
#include "mipv6.h"
#include "util.h"
#include "icmp6.h"
#include "mpdisc_ha.h"
#include "list.h"
#include "conf.h"
#include "prefix.h"
#include "ha.h"
#include "ndisc.h"
#include "hash.h"
#include "statistics.h"

struct mpa_entry {
	struct in6_addr ha;
	struct in6_addr hoa;
	struct timespec delay;
	struct timespec lastsent;
	struct tq_elem tqe;
	int retries;
};

#define MPA_BUCKETS 32
static struct hash mpa_hash;
static pthread_mutex_t mpa_lock;

static pthread_rwlock_t prefix_lock;
static volatile uint16_t mpa_id;

static inline struct mpa_entry *mpa_get(const struct in6_addr *ha,
					const struct in6_addr *hoa)
					
{
	return hash_get(&mpa_hash, ha, hoa);
}

/* HA functions */

#ifdef ENABLE_VT

int mpd_poll_mpa(const struct in6_addr *ha,
		 const struct in6_addr *hoa,
		 struct timespec *delay,
		 struct timespec *lastsent)
{
	struct mpa_entry *e;
	int ret = -ENOENT;
	pthread_mutex_lock(&mpa_lock);
	e = mpa_get(ha, hoa);
	if (e != NULL) {
		*delay = e->delay;
		*lastsent = e->lastsent;
		ret = e->retries;
	}
	pthread_mutex_unlock(&mpa_lock);
	return ret;
}

/**
 * mpd_plist_iterate - apply function to every prefix list entry
 * @func: function to apply
 * @arg: extra data for @func
 **/

void mpd_plist_iterate(struct ha_interface *iface,
		       int (* func)(int, void *, void *), void *arg)
{
	struct list_head *lp;

	list_for_each(lp, &iface->prefix_list) {
		struct prefix_list_entry *ple;
		int ret;

		ple = list_entry(lp, struct prefix_list_entry, list);

		pthread_rwlock_rdlock(&prefix_lock);
		ret = func(iface->ifindex, ple, arg);
		pthread_rwlock_unlock(&prefix_lock);

		if (ret)
			break;
	}
}
#endif

static int mpd_get_mpa_prefixes(struct ha_interface *iface, struct iovec *iov)
{
	struct list_head *pos;
	struct timespec now;
	int buf_size;
	struct nd_opt_prefix_info *p;
	int n;
	int expired = -1;

	clock_gettime(CLOCK_REALTIME, &now);

	pthread_rwlock_rdlock(&prefix_lock);

	buf_size = iface->prefix_count * sizeof(struct nd_opt_prefix_info);

	p = malloc(buf_size);

	if (p == NULL)
		goto out;

	n = 0;
	expired = 0;
	list_for_each(pos, &iface->prefix_list) {
		struct prefix_list_entry *entry;

		entry = list_entry(pos, struct prefix_list_entry, list);

		memcpy(&p[n], &entry->pinfo,
		       sizeof(struct nd_opt_prefix_info));		

		p[n].nd_opt_pi_valid_time = 
			htonl(mpd_curr_lft(&now, &entry->timestamp,
					   entry->ple_valid_time));

		if (!p[n].nd_opt_pi_valid_time)
			expired++;

		p[n].nd_opt_pi_preferred_time = 
			htonl(mpd_curr_lft(&now, &entry->timestamp,
					   entry->ple_prefd_time));
		n++;
	}
	iov[1].iov_base = p;
	iov[1].iov_len = buf_size;
out:
	pthread_rwlock_unlock(&prefix_lock);
	return expired;
}

static void mpd_send_mpa(struct mpa_entry *e, uint16_t id)
{
	struct timespec now;
	struct ha_interface *iface;
	struct mip_prefix_advert *mpa;
	struct iovec iov[2];
	int expired;

	clock_gettime(CLOCK_REALTIME, &now);

	if (tsisset(e->lastsent)) {
		struct timespec diff;
		/* rate limit */
		tssub(now, e->lastsent, diff);
		if (diff.tv_sec == 0)
			return;
	}
	iface = ha_get_if_by_addr(&e->ha);

	if (iface == NULL)
		return;
	mpa = icmp6_create(iov, MIP_PREFIX_ADVERT, 0);

	if (mpa == NULL)
		return;
	mpa->mip_pa_id = id;
	mpa->mip_pa_flags_reserved = iface->mpa_flags;

	if ((expired = mpd_get_mpa_prefixes(iface, iov)) > 0)
		mpd_del_expired_pinfos(iface);
	else if (expired < 0) {
		free_iov_data(iov, 1);
		return;
	}
	icmp6_send(iface->ifindex, 0, &e->ha, &e->hoa, iov, 2);
	free_iov_data(iov, 2);
	e->lastsent = now;
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_MPA);
}

void mpd_cancel_mpa(const struct in6_addr *ha, const struct in6_addr *hoa)
{
	struct mpa_entry *e;
	pthread_mutex_lock(&mpa_lock);
	e = mpa_get(ha, hoa);
	if (e != NULL) {
		hash_delete(&mpa_hash, &e->ha, &e->hoa);
		if (tsisset(e->delay))
			del_task(&e->tqe);
		free(e);
	}
	pthread_mutex_unlock(&mpa_lock);
}

int mpd_start_mpa(const struct in6_addr *ha, const struct in6_addr *hoa)
{
	struct mpa_entry *e;
	int err = -ENOMEM;

	if (!conf.SendUnsolMobPfxAdvs)
		return 0;

	pthread_mutex_lock(&mpa_lock);
	e = mpa_get(ha, hoa);
	if (e == NULL) {
		e = malloc(sizeof(struct mpa_entry));
		if (e == NULL)
			goto out;
		memset(e, 0, sizeof(struct mpa_entry));
		e->ha = *ha;
		e->hoa = *hoa;
		if (hash_add(&mpa_hash, e, &e->ha, &e->hoa)) {
			free(e);
			goto out;
		}
		INIT_LIST_HEAD(&e->tqe.list);
	}
	err = 0;
out:
	pthread_mutex_unlock(&mpa_lock);
	return err;

}

static void mpd_recv_mps(const struct icmp6_hdr *ih,
			 __attribute__ ((unused)) ssize_t len,
			 const struct in6_addr *src, 
			 const struct in6_addr *dst,
			 __attribute__ ((unused)) int iif,
			 __attribute__ ((unused)) int hoplimit)
{
	struct mpa_entry *e;

	if (!conf.SendMobPfxAdvs)
		return;
	pthread_mutex_lock(&mpa_lock);
	e = mpa_get(dst, src);
	if (e != NULL) {
		if (tsisset(e->delay))
			del_task(&e->tqe);
		mpd_send_mpa(e, ih->icmp6_id);
	}
	pthread_mutex_unlock(&mpa_lock);
}

static struct icmp6_handler mpd_mps_handler = {
	.recv = mpd_recv_mps,
};

int mpd_prefix_check(struct in6_addr *dst,
		     struct in6_addr *src,
		     struct timespec *lft,
		     int *ifindex,
		     int dad)
{
	struct ha_interface *i;
	struct prefix_list_entry *p;
	int res = -ENODEV;

	i = ha_get_if_by_addr(dst);
	if (i != NULL) {
		int expired = 0;
		res = -ENOENT;
		pthread_rwlock_rdlock(&prefix_lock);
		p = prefix_list_get(&i->prefix_list, src, 0);
		if (p != NULL) {
			unsigned long valid, preferred;
			struct timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			valid = umin(mpd_curr_lft(&now, &p->timestamp,
						  p->ple_valid_time),
				     MAX_BINDING_LIFETIME + 1);
			preferred = umin(mpd_curr_lft(&now, &p->timestamp,
						      p->ple_prefd_time),
					 MAX_BINDING_LIFETIME + 1);

			if (!valid)
				expired = 1;
			else if (dad) {
				/* decrease DAD timeout */
				valid -= DAD_TIMEOUT;
			}
			if (preferred <= (unsigned long)lft->tv_sec)
				res = IP6_MH_BAS_PRFX_DISCOV;
			else
				res = IP6_MH_BAS_ACCEPTED;

			if (valid < (unsigned long)lft->tv_sec)
				tssetsec(*lft,
					 umin(valid, MAX_BINDING_LIFETIME));
			*ifindex = i->ifindex;
		}
		pthread_rwlock_unlock(&prefix_lock);
		if (expired)
			mpd_del_expired_pinfos(i);
	}
	return res;
}

static void mpd_resend_unsol_mpa(struct tq_elem *tqe)
{
	pthread_mutex_lock(&mpa_lock);
	if (!task_interrupted()) {
		struct mpa_entry *e = tq_data(tqe, struct mpa_entry, tqe);
		mpd_send_mpa(e, mpa_id++);
		if (++e->retries < PREFIX_ADV_RETRIES) {
			struct timespec expires;
			tsadd(e->delay, e->delay, e->delay);
			tsadd(e->lastsent,e->delay, expires);
			add_task_abs(&expires, &e->tqe, mpd_resend_unsol_mpa);
		} else {
			tsclear(e->delay);
			e->retries = 0;
		}
	}
	pthread_mutex_unlock(&mpa_lock);
}

static inline void mpd_rand_adv_delay(struct timespec *delay,
				      uint32_t preferred_time)
{
	unsigned long max_schedule_delay;
	unsigned long rand_adv_delay;

	max_schedule_delay = min(conf.MaxMobPfxAdvInterval, preferred_time);
	rand_adv_delay = conf.MinMobPfxAdvInterval;
	if (max_schedule_delay != conf.MinMobPfxAdvInterval)
		rand_adv_delay += random() %
			labs((long)max_schedule_delay - 
			     (long)conf.MinMobPfxAdvInterval);

	tssetsec(*delay, rand_adv_delay);
}

static void mpd_send_unsol_mpa(struct tq_elem *tqe)
{
	pthread_mutex_lock(&mpa_lock);
	if (!task_interrupted()) {
		struct mpa_entry *e = tq_data(tqe, struct mpa_entry, tqe);
		struct timespec expires;
		mpd_send_mpa(e, mpa_id++);
		e->delay = PREFIX_ADV_TIMEOUT_TS;
		tsadd(e->lastsent, e->delay, expires);
		add_task_abs(&expires, &e->tqe, mpd_resend_unsol_mpa);
	}
	pthread_mutex_unlock(&mpa_lock);
}

static void mpd_schedule_unsol_mpa_send(struct mpa_entry *e, 
					struct prefix_list_entry *ple)
{
	struct timespec tmp;

	if (!conf.SendUnsolMobPfxAdvs)
		return;
	if (ipv6_pfx_cmp(&e->hoa, &ple->ple_prefix, ple->ple_plen) &&
	    tsisset(e->lastsent)) {
		struct timespec diff;
		tssub(ple->timestamp, e->lastsent, diff);
		if ((unsigned long)diff.tv_sec < conf.MaxMobPfxAdvInterval)
			return;
	}
	mpd_rand_adv_delay(&tmp, ple->ple_prefd_time);
	if (!tsisset(e->delay) || tsbefore(e->delay, tmp))
		e->delay = tmp;
	tsadd(ple->timestamp, e->delay, tmp);
	add_task_abs(&tmp, &e->tqe, mpd_send_unsol_mpa);
}

static int mpd_prefix_changed(void *ve, void *vple)
{
	struct mpa_entry *e = ve;
	struct prefix_list_entry *ple = vple;
	mpd_schedule_unsol_mpa_send(e, ple);
	return 0;
}

void mpd_handle_mpa_flags(struct ha_interface *iface, uint8_t ra_flags)
{
	pthread_rwlock_wrlock(&prefix_lock);
	iface->mpa_flags = ((ra_flags & ND_RA_FLAG_MANAGED ? 
			     MIP_PA_FLAG_MANAGED : 0) |
			    (ra_flags & ND_RA_FLAG_OTHER ? 
			     MIP_PA_FLAG_OTHER : 0));
	pthread_rwlock_unlock(&prefix_lock);

}

static int mpa_iterate(int (* func)(void *, void *), void *arg)
{
	int err;
	pthread_mutex_lock(&mpa_lock); 
	err = hash_iterate(&mpa_hash, func, arg);
	pthread_mutex_unlock(&mpa_lock);
	return err;
}


#define MPD_NO_CHANGE 0
#define MPD_PREFIX_MODIFIED 1
#define MPD_NEW_PREFIX 2

int mpd_handle_pinfo(struct ha_interface *iface, struct nd_opt_prefix_info *p)
{
	int status = MPD_NEW_PREFIX;
	struct prefix_list_entry *e, tmp;
	struct timespec now;


	clock_gettime(CLOCK_REALTIME, &now);

	pthread_rwlock_wrlock(&prefix_lock);

	/* ignore the router address in the prefix */
	e = prefix_list_get(&iface->prefix_list,
			    &p->nd_opt_pi_prefix,
			    p->nd_opt_pi_prefix_len);

	if (e != NULL) {
		struct timespec tmp;
		unsigned long diff;
		uint8_t cf;
		int cv;
		int cp;
		int expired = 0;

		status = MPD_NO_CHANGE;

		/* check prefix flags, ignore R bit */
		cf = ((p->nd_opt_pi_flags_reserved ^e->ple_flags) &
		      (ND_OPT_PI_FLAG_ONLINK|ND_OPT_PI_FLAG_AUTO));

		/* the lifetime decreases real time in the MN, so the
		   HA needs to renew the prefix also when the advertised
		   lifetime stays unchanged */
		
		/* if RA interval is in milliseconds, don't update entry
		   before a noticeable change has occurred in the prefix */

		/* todo(?): allow some clock drift */

		tssub(now, e->timestamp, tmp);
		diff = tmp.tv_sec;

		if (e->ple_valid_time == 0 && p->nd_opt_pi_valid_time == 0) {
			expired = 1;
		}

		/* check valid and preferred lifetimes, also take infinite
		   lifetimes into account */

		cv = ((!prefix_lft_infinite(p->nd_opt_pi_valid_time) ||
		       !prefix_lft_infinite(e->ple_valid_time)) &&
		      p->nd_opt_pi_valid_time + diff != e->ple_valid_time);

		cp = ((!prefix_lft_infinite(p->nd_opt_pi_preferred_time) ||
		       !prefix_lft_infinite(e->ple_prefd_time)) &&
		      p->nd_opt_pi_preferred_time + diff != e->ple_prefd_time);

		if (cf || cv || cp) {    
			e->timestamp = now;
			memcpy(&e->pinfo, p, 
			       sizeof(struct nd_opt_prefix_info));
			if (!expired)
				status = MPD_PREFIX_MODIFIED;
		}
	} else {
		e = malloc(sizeof(struct prefix_list_entry));
		if (!e) {
			pthread_rwlock_unlock(&prefix_lock);
			return -1;
		}
		memcpy(&e->pinfo, p, sizeof(struct nd_opt_prefix_info));
		list_add(&e->list, &iface->prefix_list);
		iface->prefix_count++;
		e->timestamp = now;
	}
	memcpy(&tmp, e, sizeof(struct prefix_list_entry));
	pthread_rwlock_unlock(&prefix_lock);

	if (status != MPD_NO_CHANGE) {
		mpa_iterate(mpd_prefix_changed, &tmp);
	}
	return 0;
}

void mpd_del_expired_pinfos(struct ha_interface *i)
{
	struct list_head *l, *n;
	struct timespec now;

	pthread_rwlock_wrlock(&prefix_lock);

	clock_gettime(CLOCK_REALTIME, &now);

	list_for_each_safe(l, n, &i->prefix_list) {
		struct prefix_list_entry *e;
		struct timespec tmp;
		unsigned long diff;

		e = list_entry(l, struct prefix_list_entry, list);

		tssub(now, e->timestamp, tmp);
		diff = tmp.tv_sec;

		if (e->ple_valid_time == 0) {
			if (diff > 2 * conf.MaxMobPfxAdvInterval) {
				/* All the MNs should have received a MPA, 
				   so it's probably safe to remove this 
				   entry. */
				list_del(&e->list);
				free(e);
				i->prefix_count--;
			}
		} else if(!prefix_lft_infinite(e->ple_valid_time) &&
			  diff >= e->ple_valid_time) {
			/* time to expire this entry */
			tsinc(e->timestamp, e->ple_valid_time, 0);
			e->ple_valid_time = 0;
			e->ple_prefd_time = 0;
		}
	}
	pthread_rwlock_unlock(&prefix_lock);
}

int mpd_ha_init(void)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&mpa_lock, &mattrs) ||
	    pthread_rwlock_init(&prefix_lock, NULL) ||
	    hash_init(&mpa_hash, DOUBLE_ADDR, MPA_BUCKETS) < 0)
		return -1;
	icmp6_handler_reg(MIP_PREFIX_SOLICIT, &mpd_mps_handler);
	return 0;
}

void mpd_ha_cleanup(void)
{
	icmp6_handler_dereg(MIP_PREFIX_SOLICIT, &mpd_mps_handler);
	pthread_mutex_lock(&mpa_lock);
	hash_cleanup(&mpa_hash);
	pthread_mutex_unlock(&mpa_lock);
}
