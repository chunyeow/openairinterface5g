/*
 * $Id: dhaad_mn.c 1.21 06/05/15 12:03:38+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Ville Nuorvala <vnuorval@tcs.hut.fi>,
 *          Antti Tuominen <anttit@tcs.hut.fi>
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
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <stdlib.h>
#include <errno.h>

#include "mipv6.h"
#include "icmp6.h"
#include "dhaad_mn.h"
#include "movement.h"
#include "mn.h"
#include "debug.h"
#include "xfrm.h"
#include "statistics.h"

/*********************************************************
 * Mobile Node functions
 *********************************************************/

static volatile uint16_t dhaad_id;

static int dhaad_flush_candidates(struct ha_candidate_list *t)
{

	struct list_head *l, *n;

	pthread_mutex_lock(&t->c_lock);

	/* DHAAD Reply received, flush old candidates */
	/* Delete all entries from lh */

	list_for_each_safe(l, n, &t->home_agents) {
		list_del(l);
		free(list_entry(l, struct ha_candidate, list));
	}
	pthread_mutex_unlock(&t->c_lock);
	return 0;
}

static int dhaad_append_candidate(struct ha_candidate_list *t,
				  struct in6_addr *ha)
{

	struct ha_candidate *new;
	new = malloc(sizeof(struct ha_candidate));
	if (new == NULL)
		return -ENOMEM;
	new->retry = 0;
	new->addr = *ha;

	/* Append new candidate from a DHAAD Reply */
	pthread_mutex_lock(&t->c_lock);
	list_add_tail(&new->list, &t->home_agents);
	pthread_mutex_unlock(&t->c_lock);

	return 0;
}

static int dhaad_send_request(int oif, struct in6_addr *src, 
			      struct in6_addr *pfx, int plen,
			      uint16_t flags)
{
	struct mip_dhaad_req *ih;
	struct iovec iov;
	struct in6_addr dst;
	uint16_t id;

	/* Send DHAAD Request */
	if ((ih = icmp6_create(&iov, MIP_HA_DISCOVERY_REQUEST, 0)) == NULL)
	    return -1;
	id = dhaad_id++;
	ih->mip_dhreq_id = htons(id);
	ih->mip_dhreq_flags_reserved = flags;
	dhaad_gen_ha_anycast(&dst, pfx, plen);
	icmp6_send(oif, 0, src, &dst, &iov, 1);
	free_iov_data(&iov, 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_DHAAD_REQ);
	return id;
}

static void dhaad_resend(struct tq_elem *tqe)
{
	pthread_rwlock_rdlock(&mn_lock);
	if (!task_interrupted()) {
		struct home_addr_info *hai;
		struct ha_candidate_list *t;
		hai = tq_data(tqe, struct home_addr_info, ha_list.tqe);
		t = &hai->ha_list;
		pthread_mutex_lock(&t->c_lock);
		if (t->dhaad_resends == DHAAD_RETRIES) {
			pthread_mutex_unlock(&t->c_lock);
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
		t->dhaad_id = dhaad_send_request(hai->primary_coa.iif,
						 &hai->primary_coa.addr,
						 &hai->home_prefix,
						 hai->home_plen,
						 hai->mob_rtr?
						 MIP_DHREQ_FLAG_SUPPORT_MR:0);
		t->dhaad_resends++;
		tsadd(t->dhaad_delay, t->dhaad_delay, t->dhaad_delay);
		add_task_rel(&t->dhaad_delay, &t->tqe, dhaad_resend);
		pthread_mutex_unlock(&t->c_lock);
	}
	pthread_rwlock_unlock(&mn_lock);
}

static void _dhaad_start(struct home_addr_info *hai, int force)
{
	struct ha_candidate_list *t = &hai->ha_list;
	if (force || 
	    (movement_ho_verdict(hai->verdict) && 
	     (!tsisset(t->dhaad_delay) ||
	      t->dhaad_resends == DHAAD_RETRIES))) {
		if (!(hai->home_block & HOME_ADDR_BLOCK))
			xfrm_block_hoa(hai);
		if (hai->mob_rtr && !(hai->home_block & NEMO_RA_BLOCK))
			xfrm_block_ra(hai);
		t->dhaad_resends = 0;
		t->dhaad_id = dhaad_send_request(hai->primary_coa.iif,
						 &hai->primary_coa.addr,
						 &hai->home_prefix,
						 hai->home_plen,
						 hai->mob_rtr?
						 MIP_DHREQ_FLAG_SUPPORT_MR:0);
		t->dhaad_delay = INITIAL_DHAAD_TIMEOUT_TS;
		add_task_rel(&t->dhaad_delay, &t->tqe, dhaad_resend);
	}
}

void dhaad_start(struct home_addr_info *hai)
{
	struct ha_candidate_list *t = &hai->ha_list;
	pthread_mutex_lock(&t->c_lock);
	_dhaad_start(hai, 0);
	pthread_mutex_unlock(&t->c_lock);
}

static void _dhaad_stop(struct home_addr_info *hai)
{
	struct ha_candidate_list *t = &hai->ha_list;
	t->dhaad_id = -1;
	t->dhaad_resends = 0;
	tsclear(t->dhaad_delay);
	if (hai->home_block & HOME_ADDR_BLOCK)
		xfrm_unblock_hoa(hai);
	if (hai->home_block & NEMO_RA_BLOCK)
		xfrm_unblock_ra(hai);
}

void dhaad_stop(struct home_addr_info *hai)
{
	struct ha_candidate_list *t = &hai->ha_list;
	pthread_mutex_lock(&t->c_lock);
	if (tsisset(t->dhaad_delay)) {
		del_task(&t->tqe);
		_dhaad_stop(hai);
	}
	t->last_ha = in6addr_any;
	pthread_mutex_unlock(&t->c_lock);
}

static int _dhaad_next_candidate(struct home_addr_info *hai,
				 struct ha_candidate_list *t)
{
	struct list_head *l, *n;
	list_for_each_safe(l, n, &t->home_agents) {
		struct ha_candidate *c;
		c = list_entry(l, struct ha_candidate, list);
		hai->ha_addr = c->addr;
		list_del(&c->list);
		free(c);
		if (IN6_IS_ADDR_UNSPECIFIED(&t->last_ha) ||
		    !IN6_ARE_ADDR_EQUAL(&t->last_ha, &hai->ha_addr))
			return 0;
	}
	hai->ha_addr = in6addr_any;
	return -ENOENT;
}

int dhaad_next_candidate(struct home_addr_info *hai)
{
	int err;
	struct ha_candidate_list *t = &hai->ha_list;
	pthread_mutex_lock(&t->c_lock);
	err = _dhaad_next_candidate(hai, t);
	pthread_mutex_unlock(&t->c_lock);
	return err;
}

int dhaad_home_reg_failed(struct home_addr_info *hai)
{
	struct ha_candidate_list *t = &hai->ha_list;
	int err;
	pthread_mutex_lock(&t->c_lock);
	t->last_ha = hai->ha_addr;
	if ((err = _dhaad_next_candidate(hai, t)) < 0) {
		_dhaad_start(hai, 1);
		err = -EAGAIN;
	}
	pthread_mutex_unlock(&t->c_lock);
	return err;
}

static void dhaad_recv_rep(const struct icmp6_hdr *ih, ssize_t len,
			   __attribute__ ((unused)) const struct in6_addr *src,
			   __attribute__ ((unused)) const struct in6_addr *dst,
			   int iif,
			   __attribute__ ((unused)) int hoplimit)
{
	struct mip_dhaad_rep *rph = (struct mip_dhaad_rep *)ih;
	int i;
	struct in6_addr *ha;
	struct home_addr_info *hai;
	int ulen = len - sizeof(struct icmp6_hdr);
	int n_addr = ulen >> 4;
	uint16_t id;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_DHAAD_REP);

	/* Check packet validity */
	if (ulen % sizeof(struct in6_addr) || rph->mip_dhrep_code)
		return;

	id = ntohs(rph->mip_dhrep_id);

	pthread_rwlock_wrlock(&mn_lock);
	if ((hai = mn_get_home_addr_by_dhaadid(id)) == NULL) {
		dbg("No matching request for dhaad reply\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	if (hai->mob_rtr &&
	    !(rph->mip_dhrep_flags_reserved & MIP_DHREP_FLAG_SUPPORT_MR)) {
		dbg("HA doesn't support MR\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	ha = (struct in6_addr *)(ih + 1);

	dhaad_flush_candidates(&hai->ha_list);

	for (i = 0; i < n_addr; i++) {
		dhaad_append_candidate(&hai->ha_list, ha);
		ha++;
	}
	if (dhaad_next_candidate(hai) < 0) {
		dbg("No valid HAs\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	pthread_rwlock_unlock(&mn_lock);
	/* Trigger BU in MN if DHAAD reply contained any HAs */
	md_trigger_movement_event(ME_DHAAD, id, iif);
	return;
}

static struct icmp6_handler dhaad_rep_handler = {
	.recv = dhaad_recv_rep,
};

void dhaad_mn_init(void)
{
	icmp6_handler_reg(MIP_HA_DISCOVERY_REPLY, &dhaad_rep_handler);
}

void dhaad_mn_cleanup(void)
{
	icmp6_handler_dereg(MIP_HA_DISCOVERY_REPLY, &dhaad_rep_handler);
}
