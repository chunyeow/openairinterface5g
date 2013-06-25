/*
 * $Id: mpdisc_mn.c 1.20 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
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
#include <syslog.h>
#include <pthread.h>

#include "debug.h"
#include "mipv6.h"
#include "util.h"
#include "icmp6.h"
#include "mpdisc_mn.h"
#include "list.h"
#include "conf.h"
#include "prefix.h"
#include "mn.h"
#include "hash.h"
#include "statistics.h"

struct mps_entry {
	struct in6_addr hoa;
	struct in6_addr ha;
	struct timespec delay;
	struct timespec lastsent;
	struct tq_elem tqe;
	uint16_t id;
};


#define MPS_BUCKETS 32
static struct hash mps_hash;
static pthread_mutex_t mps_lock;

static inline struct mps_entry *mps_get(const struct in6_addr *hoa,
					const struct in6_addr *ha)
{
	return hash_get(&mps_hash, hoa, ha);
}

#ifdef ENABLE_VT
int mpd_poll_mps(const struct in6_addr *hoa,
		 const struct in6_addr *ha,
		 struct timespec *delay,
		 struct timespec *lastsent)
{
	struct mps_entry *e;
	int err = -ENOENT;
	pthread_mutex_lock(&mps_lock);
	e = mps_get(hoa, ha);
	if (e != NULL) {
		*delay = e->delay;
		*lastsent = e->lastsent;
		err = 0;
	}
	pthread_mutex_unlock(&mps_lock);
	return err;
}
#endif
/* MN functions */

static int mpd_send_mps(struct mps_entry *e)
{
	struct iovec iov;
	struct mip_prefix_solicit *ih;

	ih = icmp6_create(&iov, MIP_PREFIX_SOLICIT, 0);

	if (ih == NULL)
		return -ENOMEM;

	/* no need to worry about network byte order since 
	   mip_ps_id only serves as an identifier */
	ih->mip_ps_id = e->id;
	clock_gettime(CLOCK_REALTIME, &e->lastsent);
	icmp6_send(0, 0, &e->hoa, &e->ha, &iov, 1);
	free_iov_data(&iov, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_MPS);
	return 0;
}

void mpd_cancel_mps(const struct in6_addr *hoa,
		    const struct in6_addr *ha)
{
	struct mps_entry *e;
	pthread_mutex_lock(&mps_lock);
	e = mps_get(hoa, ha);
	if (e != NULL) {
		dbg("canceling MPS\n");
		hash_delete(&mps_hash, &e->hoa, &e->ha);
		if (tsisset(e->delay))
			del_task(&e->tqe);
		free(e);
	}
	pthread_mutex_unlock(&mps_lock);
}

static void mpd_resend_mps(struct tq_elem *tqe)
{
	pthread_mutex_lock(&mps_lock);
	if (!task_interrupted()) {
		struct mps_entry *e = tq_data(tqe, struct mps_entry, tqe);
		if (tsbefore(MAX_BINDACK_TIMEOUT_TS, e->delay)) {
			mpd_send_mps(e);
			tsadd(e->delay, e->delay, e->delay);
			e->delay = tsmin(e->delay, MAX_BINDACK_TIMEOUT_TS);
			add_task_rel(&e->delay, &e->tqe, mpd_resend_mps);
		} else
			tsclear(e->delay);
	}
	pthread_mutex_unlock(&mps_lock);
}

static void mpd_send_first_mps(struct tq_elem *tqe)
{
	pthread_mutex_lock(&mps_lock);
	if (!task_interrupted()) {
		struct mps_entry *e = tq_data(tqe, struct mps_entry, tqe);
		e->id = random();
		mpd_send_mps(e);
		e->delay = INITIAL_SOLICIT_TIMER_TS;
		add_task_rel(&e->delay, &e->tqe, mpd_resend_mps);
	}
	pthread_mutex_unlock(&mps_lock);

}

#define PREFIX_LIFETIME_MAX_FINITE 0x7FFFFFFF

int mpd_schedule_first_mps(const struct in6_addr *hoa,
			   const struct in6_addr *ha,
			   const struct timespec *valid_time)
{
	struct mps_entry *e;
	int err = -ENOMEM;
	uint32_t delay;

	if (!conf.SendMobPfxSols)
		return 0;

	pthread_mutex_lock(&mps_lock);
	e = mps_get(hoa, ha);
	if (e == NULL) {
		e = malloc(sizeof(struct mps_entry));
		if (e == NULL)
			goto out;
		memset(e, 0, sizeof(struct mps_entry));
		e->hoa = *hoa;
		e->ha = *ha;
		if (hash_add(&mps_hash, e, &e->hoa, &e->ha)) {
			free(e);
			goto out;
		}
		INIT_LIST_HEAD(&e->tqe.list);
	}
	err = 0;
	delay = umin(valid_time->tv_sec, PREFIX_LIFETIME_MAX_FINITE);
	tssetsec(e->delay, delay * MPS_REFRESH_DELAY);
	add_task_rel(&e->delay, &e->tqe, mpd_send_first_mps);
	dbg("schedule MPS in %u s\n", e->delay.tv_sec);
out:
	pthread_mutex_unlock(&mps_lock);
	return err;
}

static void mpd_recv_mpa(const struct icmp6_hdr *ih, ssize_t len,
			 const struct in6_addr *src,
			 const struct in6_addr *dst,
			 __attribute__ ((unused)) int iif,
			 __attribute__ ((unused)) int hoplimit)
{
	uint8_t *opt = (uint8_t *)(ih + 1);
	struct mps_entry *e;
	struct home_addr_info *hai;
	struct timespec lastsent;
	
	pthread_mutex_lock(&mps_lock);
	e = mps_get(dst, src);
	if (e == NULL) {
		pthread_mutex_unlock(&mps_lock);
		return;
	}
	if (e->id != ih->icmp6_id) {
		pthread_mutex_unlock(&mps_lock);
		mpd_trigger_mps(dst, src);
		return;
	}
	del_task(&e->tqe);
	tsclear(e->delay);
	lastsent = e->lastsent;
	pthread_mutex_unlock(&mps_lock);

	/* todo: do something about the MO flags */

	pthread_rwlock_wrlock(&mn_lock);
	hai = mn_get_home_addr(dst);
	if (hai != NULL) {
		int optlen = len - sizeof(struct icmp6_hdr);
		while (optlen > 1) {
			uint16_t olen = opt[1] << 3;
			
			if (olen > (unsigned int)optlen || olen == 0)
				break;

			if (opt[0] == ND_OPT_PREFIX_INFORMATION) {
				struct nd_opt_prefix_info *pinfo;

				if (olen < sizeof(*pinfo))
					continue;

				pinfo = (struct nd_opt_prefix_info *) opt;
				/* internal representation host byte order */
				pinfo->nd_opt_pi_valid_time = 
					ntohl(pinfo->nd_opt_pi_valid_time);
				pinfo->nd_opt_pi_preferred_time =
					ntohl(pinfo->nd_opt_pi_preferred_time);

				if (pinfo->nd_opt_pi_valid_time <
				    pinfo->nd_opt_pi_preferred_time)
					continue;

				mn_update_home_prefix(hai, &lastsent, pinfo);
			}
			optlen -= olen;
			opt += olen;
		}
	}
	pthread_rwlock_unlock(&mn_lock);
}

static struct icmp6_handler mpd_mpa_handler = {
	.recv = mpd_recv_mpa,
};

int mpd_mn_init(void)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&mps_lock, &mattrs))
		return -1;
	if (hash_init(&mps_hash, DOUBLE_ADDR, MPS_BUCKETS) < 0)
		return -1;
	icmp6_handler_reg(MIP_PREFIX_ADVERT, &mpd_mpa_handler);
	return 0;
}

void mpd_mn_cleanup(void)
{
	icmp6_handler_dereg(MIP_PREFIX_ADVERT, &mpd_mpa_handler);
	pthread_mutex_lock(&mps_lock);
	hash_cleanup(&mps_hash);
	pthread_mutex_unlock(&mps_lock);
}
