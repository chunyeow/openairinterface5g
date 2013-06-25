/*
 * $Id: dhaad_ha.c 1.14 06/05/07 21:52:42+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Antti Tuominen <anttit@tcs.hut.fi>
 *          Ville Nuorvala <vnuorval@tcs.hut.fi>
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
#include <net/if.h>
#include <errno.h>

#include "icmp6.h"
#include "dhaad_ha.h"
#include "ha.h"
#include "debug.h"
#include "statistics.h"

static pthread_rwlock_t ha_lock;

/*********************************************************
 * Home Agent functions
 *********************************************************/

#ifdef ENABLE_VT
/**
 * dhaad_halist_iterate - apply function to every home agent list entry
 * @func: function to apply
 * @arg: extra data for @func
 **/
void dhaad_halist_iterate(struct ha_interface *iface,
			  int (* func)(int, void *, void *), void *arg)
{
	struct list_head *lp;

	list_for_each(lp, &iface->ha_list) {
		struct home_agent *h;
		int ret;

		h = list_entry(lp, struct home_agent, list);

		pthread_rwlock_rdlock(&ha_lock);
		ret = func(iface->ifindex, h, arg);
		pthread_rwlock_unlock(&ha_lock);

		if (ret)
			break;
	}
}
#endif

static void dhaad_expire_halist(struct tq_elem *tqe)
{
	pthread_rwlock_wrlock(&ha_lock);
	if (!task_interrupted()) {
		struct home_agent *ha = tq_data(tqe, struct home_agent, tqe);
		list_del(&ha->list);
		free(ha);
	}
	pthread_rwlock_unlock(&ha_lock);
}

void dhaad_insert_halist(struct ha_interface *i, uint16_t key,
			 uint16_t life_sec, uint16_t flags,
			 struct nd_opt_prefix_info *pinfo)
{
	struct home_agent *ha = NULL, *tmp;
	struct list_head *lp;
	struct in6_addr *addr = &pinfo->nd_opt_pi_prefix;

	pthread_rwlock_wrlock(&ha_lock);

	list_for_each(lp, &i->ha_list) {
		tmp = list_entry(lp, struct home_agent, list);
		if (!IN6_ARE_ADDR_EQUAL(&tmp->addr, addr))
			continue;
		ha = tmp;
		list_del(&ha->list);
		break;
	}
	if (ha == NULL) {
		/* allocate new HAL entry */
		ha = malloc(sizeof(*ha));
		if (ha == NULL) {
			pthread_rwlock_unlock(&ha_lock);
			return;
		}
		memset(ha, 0, sizeof(*ha));
		ha->flags = flags;
		ha->iface = i;
		ha->addr = *addr;
		INIT_LIST_HEAD(&ha->tqe.list);
	}
	ha->preference = key;
	tssetsec(ha->lifetime, life_sec);

	/* insert ha entry to the HAL */
	list_for_each(lp, &i->ha_list) {
		tmp = list_entry(lp, struct home_agent, list);

		if (key >= tmp->preference) {
			list_add_tail(&ha->list, &tmp->list);
			add_task_rel(&ha->lifetime, &ha->tqe,
				     dhaad_expire_halist);
			pthread_rwlock_unlock(&ha_lock);
			return;
		}
	}
	list_add_tail(&ha->list, &i->ha_list);
	add_task_rel(&ha->lifetime, &ha->tqe, dhaad_expire_halist);

	pthread_rwlock_unlock(&ha_lock);
	return;
}

static int dhaad_get_halist(struct ha_interface *i, uint16_t flags,
			    int max, struct iovec *iov)
{
	struct list_head *lp;
	int n = 0;
	list_for_each(lp, &i->ha_list) {
		struct home_agent *h;
		h = list_entry(lp, struct home_agent, list);
		if (!(flags & MIP_DHREQ_FLAG_SUPPORT_MR) ||
		    h->flags & ND_OPT_HAI_FLAG_SUPPORT_MR) {
			n++;
			iov[n].iov_len = sizeof(struct in6_addr);
			iov[n].iov_base = &h->addr;
			if (n >= max)
				break;
		}
	}
	return n;
}

static void dhaad_recv_req(const struct icmp6_hdr *ih, ssize_t len,
			   const struct in6_addr *src, 
			   const struct in6_addr *dst,
			   __attribute__ ((unused)) int iif,
			   __attribute__ ((unused)) int hoplimit)
{
	struct mip_dhaad_req *rqh = (struct mip_dhaad_req *)ih;
	struct mip_dhaad_rep *rph;
	struct iovec iov[MAX_HOME_AGENTS + 1];
	int iovlen;
	struct ha_interface *i;
	struct in6_addr *ha_addr = NULL;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_DHAAD_REQ);

	/* validity check */
	if (len < 0 || (size_t)len < sizeof(struct mip_dhaad_req))
		return;

	if ((i = ha_get_if_by_anycast(dst, &ha_addr)) == NULL) {
		dbg("no halist for anycast address %x:%x:%x:%x:%x:%x:%x:%x\n", 
		    NIP6ADDR(dst));
		return;
	}
	if ((rph = icmp6_create(iov, MIP_HA_DISCOVERY_REPLY, 0)) == NULL)
		return;

	rph->mip_dhrep_id = rqh->mip_dhreq_id;

	if (rqh->mip_dhreq_flags_reserved & MIP_DHREQ_FLAG_SUPPORT_MR)
		rph->mip_dhrep_flags_reserved = MIP_DHREP_FLAG_SUPPORT_MR;

	pthread_rwlock_rdlock(&ha_lock);
	iovlen = dhaad_get_halist(i, rqh->mip_dhreq_flags_reserved,
				  MAX_HOME_AGENTS, iov);
	icmp6_send(i->ifindex, 64, ha_addr, src, iov, iovlen + 1);
	pthread_rwlock_unlock(&ha_lock);
	free_iov_data(&iov[0], 1);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_DHAAD_REP);
}

static struct icmp6_handler dhaad_req_handler = {
	.recv = dhaad_recv_req,
};

int dhaad_ha_init(void)
{
	if (pthread_rwlock_init(&ha_lock, NULL))
		return -1;
	icmp6_handler_reg(MIP_HA_DISCOVERY_REQUEST, &dhaad_req_handler);
	return 0;
}

void dhaad_ha_cleanup(void)
{
	icmp6_handler_dereg(MIP_HA_DISCOVERY_REQUEST, &dhaad_req_handler);
}
