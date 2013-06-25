/*
 * $Id: movement.c 1.172 06/05/19 01:51:45+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Ville Nuorvala <vnuorval@tcs.hut.fi>
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
#include <syslog.h>
#include <errno.h>
#include <netinet/icmp6.h>
#include <net/if.h>
#include <linux/types.h>
#include <linux/ipv6_route.h>

#include "debug.h"
#include "icmp6.h"
#include "util.h"
#include "tqueue.h"
#include "list.h"
#include "movement.h"
#include "mn.h"
#include "rtnl.h"
#include "ndisc.h"
#include "proc_sys.h"
#include "prefix.h"
#include "conf.h"

#define MD_DEBUG_LEVEL 1

#if MD_DEBUG_LEVEL >= 1
#define MDBG dbg
#else 
#define MDBG(...) 
#endif /* MDBG */

#if MD_DEBUG_LEVEL >= 2
#define MDBG2 dbg
#else
#define MDBG2(...) 
#endif /* MDBG2 */

#if MD_DEBUG_LEVEL >= 3
#define MDBG3 dbg
#else
#define MDBG3(...)
#endif /* MDBG3 */

static LIST_HEAD(ifaces);
static pthread_mutex_t iface_lock;

static pthread_t md_listener;

static int conf_default_autoconf = 1;
static int conf_default_ra = 1;
static int conf_default_ra_defrtr = 1;
static int conf_default_rs = 3;
static int conf_default_rs_ival = 4;

static int conf_forwarding = 0;
static int conf_autoconf = 0;
static int conf_ra = 0;
static int conf_ra_defrtr = 0;
static int conf_rs = 0;

static unsigned int adv_ivals_md_trigger = 1;
static unsigned int adv_ivals_dad_limit = 3;

static int nud_expire_rtr = 1;

static void __md_trigger_movement_event(int event_type, int data,
					struct md_inet6_iface *iface,
					struct md_coa *coa)
{
	struct movement_event e;

	memset(&e, 0, sizeof(struct movement_event));
	e.md_strategy = (conf.MnRouterProbes > 0 ? 
			 MD_STRATEGY_LAZY : MD_STRATEGY_EAGER);
	e.event_type = event_type;
	e.data = data;
	e.iface_list = &ifaces;
	e.iface = iface;
	e.coa = coa;

	MDBG2("strategy %d type %d iface %s (%d) " 	
	      "CoA %x:%x:%x:%x:%x:%x:%x:%x\n",
	      e.md_strategy, e.event_type,
	      e.iface->name, e.iface->ifindex, 
	      NIP6ADDR(e.coa ? &e.coa->addr : &in6addr_any));

	mn_movement_event(&e);
}

void md_trigger_movement_event(int event_type, int data, int ifindex)
{
	pthread_mutex_lock(&iface_lock);
	if (!list_empty(&ifaces)) {
		struct md_inet6_iface *iface;
		iface = md_get_inet6_iface(&ifaces, ifindex);
		__md_trigger_movement_event(event_type, data, iface, NULL);
	}
	pthread_mutex_unlock(&iface_lock);
}

static inline void md_free_coa(struct md_coa *coa)
{
	MDBG3("freeing CoA %x:%x:%x:%x:%x:%x:%x:%x on iface %d\n", 
	      NIP6ADDR(&coa->addr), coa->ifindex);
	list_del(&coa->list);
	free(coa);
}

static void md_flush_coa(struct md_coa *coa)
{
	/* For tunnels, addresses are not managed by us so
	   we don't remove them from the interface */
	if (!(coa->flags&IFA_F_HOMEADDRESS_NODAD) &&
	    !(coa->if_is_tunnel)) {
		MDBG2("deleting CoA "
		      "%x:%x:%x:%x:%x:%x:%x:%x on iface %d\n", 
		      NIP6ADDR(&coa->addr), coa->ifindex);
		addr_del(&coa->addr, coa->plen, coa->ifindex);
	}
	md_free_coa(coa);
}

static void md_flush_coa_list(struct list_head *coas)
{
	struct list_head *list, *n;
	list_for_each_safe(list, n, coas) {
		struct md_coa *coa;
		coa = list_entry(list, struct md_coa, list);
		md_flush_coa(coa);
	}
}

static inline void md_free_router_prefix(struct prefix_list_entry *p)
{
	MDBG3("freeing prefix %x:%x:%x:%x:%x:%x:%x:%x/%d\n", 
	      NIP6ADDR(&p->ple_prefix), p->ple_plen);
	list_del(&p->list);
	free(p);
}

static void md_prefix_rule_del(struct prefix_list_entry *p)
{
	struct in6_addr prefix;
	ipv6_addr_prefix(&prefix, &p->ple_prefix, p->ple_plen);
	rule_del(NULL, RT6_TABLE_MAIN, IP6_RULE_PRIO_MIP6_COA_OUT,
		 RTN_UNICAST, &prefix, p->ple_plen,
		 &in6addr_any, 0, 0);
}

static void __md_free_router(struct md_router *rtr)
{
	struct list_head *l, *n;
	if (rtr->used) {
		MDBG2("deleting default route via %x:%x:%x:%x:%x:%x:%x:%x\n", 
		      NIP6ADDR(&rtr->lladdr));

		route_del(rtr->ifindex, RT_TABLE_MAIN, 0,
			  &in6addr_any, 0, &in6addr_any, 0, &rtr->lladdr);
	}
	list_for_each_safe(l, n, &rtr->prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(l, struct prefix_list_entry, list);
		if (rtr->used) {
			md_prefix_rule_del(p);
			MDBG2("deleting prefix route "
			      "%x:%x:%x:%x:%x:%x:%x:%x/%d\n", 
			      NIP6ADDR(&p->ple_prefix), p->ple_plen);
			route_del(rtr->ifindex, RT_TABLE_MAIN, 0, &in6addr_any,
				  0, &p->ple_prefix, p->ple_plen, NULL);
		}
		md_free_router_prefix(p);
	}
	list_del(&rtr->list);
	MDBG3("freeing router %x:%x:%x:%x:%x:%x:%x:%x\n", 
	      NIP6ADDR(&rtr->lladdr));
	free(rtr);
}

static inline void md_free_router(struct md_router *rtr)
{
	assert(!tsisset(rtr->lifetime));
	__md_free_router(rtr);
}

static void md_flush_router_list(struct list_head *routers)
{
	struct list_head *list, *n;
	list_for_each_safe(list, n, routers) {
		struct md_router *rtr;
		rtr = list_entry(list, struct md_router, list);
		md_free_router(rtr);
	}
}

static void md_free_inet6_iface(struct md_inet6_iface *iface)
{
	MDBG3("freeing iface %s (%d)\n", iface->name, iface->ifindex);
	if (!iface->is_tunnel) {
		/* tunnel interfaces have no router information */
		assert(list_empty(&iface->default_rtr));
		md_flush_router_list(&iface->expired_rtrs);
	}
	assert(list_empty(&iface->coas));
	md_flush_coa_list(&iface->expired_coas);
	free(iface);
}

static void md_expire_coa(struct md_inet6_iface *iface, struct md_coa *coa)
{
	list_del(&coa->list);
	MDBG2("expiring CoA %x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
	      NIP6ADDR(&coa->addr), iface->name, iface->ifindex);
	list_add_tail(&coa->list, &iface->expired_coas);
}

static void md_reset_egress_forward(void)
{
	struct list_head *l;
	int forward = 0;;

	if (list_empty(&ifaces))
		return;

	list_for_each(l, &ifaces) {
		struct md_inet6_iface *i;
		i = list_entry(l, struct md_inet6_iface, list);
		forward |= i->home_link;
	}
	list_for_each(l, &ifaces) {
		struct md_inet6_iface *i;
		i = list_entry(l, struct md_inet6_iface, list);
		set_iface_proc_entry(PROC_SYS_IP6_FORWARDING,
				     i->name, forward);
	}
}

static void md_reset_home_link(struct md_inet6_iface *i)
{
	if (i->home_link)
		md_reset_egress_forward();
	i->home_link = 0;
	i->ll_dad_unsafe = 0;
}

static void md_expire_router(struct md_inet6_iface *iface,
			     struct md_router *old, struct md_router *new)
{
	struct list_head *plist, *pn;

	assert(old != NULL);

	del_task(&old->tqe);

	tsclear(old->lifetime);
	list_del(&old->list);

	MDBG("expiring router %x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
	     NIP6ADDR(&old->lladdr), iface->name, iface->ifindex);

	if (old->used) {
		list_for_each_safe(plist, pn, &old->prefixes) {
			struct prefix_list_entry *p;
			struct list_head *clist, *cn;
			p = list_entry(plist, struct prefix_list_entry, list);
			
			/* don't expire prefixes also advertised by the
			   new router */
			if (new && prefix_list_find(&new->prefixes, 
						    &p->ple_prefix,
						    p->ple_plen)) {
				md_free_router_prefix(p);
				continue;
			}
			list_for_each_safe(clist, cn, &iface->coas) {
				struct md_coa *coa;
				coa = list_entry(clist, struct md_coa, list);
				if (!ipv6_pfx_cmp(&p->ple_prefix,
						  &coa->addr, coa->plen))
					md_expire_coa(iface, coa);
			}
		}
		if (new == NULL)
			md_reset_home_link(iface);
	}
	list_add_tail(&old->list, &iface->expired_rtrs);
}

static void md_block_rule_del(struct md_inet6_iface *iface)
{
	rule_del(NULL, 0, IP6_RULE_PRIO_MIP6_BLOCK_HOA, RTN_BLACKHOLE,
		 &in6addr_any, 0, &in6addr_any, 0, 0);
	rule_del(NULL, RT6_TABLE_MAIN, IP6_RULE_PRIO_MIP6_COA_OUT, RTN_UNICAST,
		 &in6addr_any, 128, &in6addr_any, 0, 0);
	iface->iface_flags &= ~MD_BLOCK_TRAFFIC;
}

static void md_flush_inet6_iface(struct md_inet6_iface *iface)
{
	struct md_router *rtr;
	struct list_head *l, *n;
	del_task(&iface->tqe);

	/* We do not maintain router related
	   information for tunnel interfaces */
	if (!iface->is_tunnel) {
		iface->router_solicits = 0;
		list_for_each_safe(l, n, &iface->backup_rtrs) {
			rtr = list_entry(l, struct md_router, list);
			md_expire_router(iface, rtr, NULL);
		}
		if ((rtr = md_get_first_router(&iface->default_rtr)) != NULL) {
			md_expire_router(iface, rtr, NULL);
		}
		md_flush_router_list(&iface->expired_rtrs);
	}

	list_for_each_safe(l, n, &iface->coas) {
		struct md_coa *coa = list_entry(l, struct md_coa, list);
		md_expire_coa(iface, coa);
	}
	md_flush_coa_list(&iface->expired_coas);
}

static void md_expire_inet6_iface(struct md_inet6_iface *iface)
{
	MDBG2("expiring iface %s (%d)\n", iface->name, iface->ifindex);
	md_flush_inet6_iface(iface);
	if (iface->iface_flags & MD_BLOCK_TRAFFIC)
		md_block_rule_del(iface);
	list_del(&iface->list);
	__md_trigger_movement_event(ME_IFACE_DOWN, 0, iface, NULL);
}

static void md_link_down(struct md_inet6_iface *iface)
{
	MDBG2("link down on iface %s (%d)\n", iface->name, iface->ifindex);
	md_flush_inet6_iface(iface);
	__md_trigger_movement_event(ME_LINK_DOWN, 0, iface, NULL);
}

static void
md_init_coa(struct md_coa *coa, struct ifaddrmsg *ifa, struct rtattr **rta_tb,
	    int if_is_tunnel)
{
	memset(coa, 0, sizeof(struct md_coa));
	INIT_LIST_HEAD(&coa->list);
	coa->flags = ifa->ifa_flags;
	coa->plen = ifa->ifa_prefixlen;
	coa->scope = ifa->ifa_scope;
	coa->ifindex = ifa->ifa_index;
	/* For tunnel interfaces, mark the CoA
	   to avoid removing the address from
	   the interface when flushing it. */
	coa->if_is_tunnel = if_is_tunnel;
	coa->addr = *(struct in6_addr *) RTA_DATA(rta_tb[IFA_ADDRESS]);
}

static struct md_coa *md_create_coa(struct md_inet6_iface *iface,
				    struct ifaddrmsg *ifa,
				    struct rtattr **rta_tb)
{
	struct md_coa *coa = malloc(sizeof(struct md_coa));
	if (coa != NULL) {
		struct ifa_cacheinfo *ci;
		md_init_coa(coa, ifa, rta_tb, iface->is_tunnel);
		ci = RTA_DATA(rta_tb[IFA_CACHEINFO]);
		clock_gettime(CLOCK_REALTIME, &coa->timestamp);
		tssetsec(coa->valid_time, ci->ifa_valid);
		tssetsec(coa->preferred_time, ci->ifa_prefered);
		MDBG3("creating CoA %x:%x:%x:%x:%x:%x:%x:%x on "
		      "iface %s (%d)\n", 
		      NIP6ADDR(&coa->addr), iface->name, iface->ifindex);
	}
	return coa;
}

static int update_coa(struct md_inet6_iface *iface, 
		      struct ifaddrmsg *ifa, struct rtattr **rta_tb)
{
	struct md_coa *new, *old;
	struct in6_addr *addr;
	
	addr = RTA_DATA(rta_tb[IFA_ADDRESS]);
	if (!in6_is_addr_routable_unicast(addr))
		return 0;

	if ((new = md_create_coa(iface, ifa, rta_tb)) == NULL)
		return -ENOMEM;

	if ((old = md_get_coa(&iface->coas, addr)) == NULL) {
		list_add(&new->list, &iface->coas); 
		MDBG2("adding CoA %x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
		      NIP6ADDR(&new->addr), iface->name, iface->ifindex);
		if (!(iface->iface_flags & MD_LINK_LOCAL_DAD)) {
			__md_trigger_movement_event(ME_COA_NEW, 0, iface, new);
			if (iface->iface_flags & MD_BLOCK_TRAFFIC)
				md_block_rule_del(iface);
		}
	} else {
		struct timespec oexp, nexp;

		tsadd(old->valid_time, old->timestamp, oexp);
		tsadd(new->valid_time, new->timestamp, nexp);

		MDBG3("updating CoA "
		      "%x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
		      NIP6ADDR(&old->addr), iface->name, iface->ifindex);

		old->flags = new->flags;
		old->plen = new->plen;
		old->timestamp = new->timestamp;
		old->valid_time = new->valid_time;
		old->preferred_time = new->preferred_time;

		free(new);

		/* check if lifetime of CoA changed */
		if (tsbefore(oexp, nexp))
			__md_trigger_movement_event(ME_COA_LFT_DEC, 0,
						    iface, old);
		else if (tsafter(oexp, nexp))
			__md_trigger_movement_event(ME_COA_LFT_INC, 0,
						    iface, old);
	}
	return 0;
}

static int process_new_addr(struct ifaddrmsg *ifa, struct rtattr **rta_tb)
{
	struct md_inet6_iface *iface;
	int res = 0;
	
	MDBG3("new address %x:%x:%x:%x:%x:%x:%x:%x on iface %d\n", 
	      NIP6ADDR((struct in6_addr *)RTA_DATA(rta_tb[IFA_ADDRESS])), 
	      ifa->ifa_index);

	pthread_mutex_lock(&iface_lock);
	if ((iface = md_get_inet6_iface(&ifaces, ifa->ifa_index)) != NULL) {
		if (ifa->ifa_scope == RT_SCOPE_LINK) {
			iface->iface_flags &= ~MD_LINK_LOCAL_DAD;
			__md_trigger_movement_event(ME_LINK_UP, 0, 
						    iface, NULL);
			if (iface->iface_flags & MD_BLOCK_TRAFFIC)
				md_block_rule_del(iface);
		} else if (ifa->ifa_scope == RT_SCOPE_UNIVERSE) {
			res = update_coa(iface, ifa, rta_tb);
		}
	}
	pthread_mutex_unlock(&iface_lock);
	return res;
}

static void md_inet6_iface_init(struct md_inet6_iface *i, int ifindex)
{
	struct list_head *list;

	memset(i, 0, sizeof(struct md_inet6_iface));
	i->ifindex = ifindex;

	/* Mark interface as tunnel if set in the conf */
	list_for_each(list, &conf.net_ifaces) {
		struct net_iface *nif;
		nif = list_entry(list, struct net_iface, list);
		if (nif->ifindex == ifindex)
			i->is_tunnel = nif->is_tunnel;
	}

	INIT_LIST_HEAD(&i->list);
	INIT_LIST_HEAD(&i->default_rtr);
	INIT_LIST_HEAD(&i->backup_rtrs);
	INIT_LIST_HEAD(&i->expired_rtrs);
	INIT_LIST_HEAD(&i->coas);
	INIT_LIST_HEAD(&i->expired_coas);
	INIT_LIST_HEAD(&i->tqe.list);
}

static int process_del_addr(struct ifaddrmsg *ifa, struct rtattr **rta_tb)
{
	struct in6_addr *addr = RTA_DATA(rta_tb[IFA_ADDRESS]);
	struct md_inet6_iface *iface;
	struct md_coa *coa;
	struct md_inet6_iface iface_h;
	struct md_coa coa_h;
	
	int res = 0;

	MDBG3("deleted address %x:%x:%x:%x:%x:%x:%x:%x on iface %d\n", 
	      NIP6ADDR(addr), ifa->ifa_index);

	if (ifa->ifa_scope != RT_SCOPE_UNIVERSE ||
	    !in6_is_addr_routable_unicast(addr))
		return 0;

	pthread_mutex_lock(&iface_lock);
	if ((iface = md_get_inet6_iface(&ifaces, ifa->ifa_index)) != NULL) {
		coa = md_get_coa(&iface->coas, addr);
		if (coa != NULL)
			md_expire_coa(iface, coa);
		else
			coa = md_get_coa(&iface->expired_coas, addr);
	} else {
		md_inet6_iface_init(&iface_h, ifa->ifa_index);
		iface = &iface_h;
		coa = NULL;
	}
	if (coa == NULL) {
		md_init_coa(&coa_h, ifa, rta_tb, iface->is_tunnel);
		coa = &coa_h;
	}
	__md_trigger_movement_event(ME_COA_EXPIRED, 0, iface, coa);
	if (coa != &coa_h)
		md_free_coa(coa);
	pthread_mutex_unlock(&iface_lock);
	return res;
}

static int process_addr(struct nlmsghdr *n)
{
	struct ifaddrmsg *ifa;
	struct rtattr *rta_tb[IFA_MAX+1];

	if (n->nlmsg_len < NLMSG_LENGTH(sizeof(*ifa)))
		return -1;

	ifa = NLMSG_DATA(n);

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa),
		     n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));

	if (!rta_tb[IFA_ADDRESS] || !rta_tb[IFA_CACHEINFO]) 
		return -1;
	
	if (n->nlmsg_type == RTM_NEWADDR)
		process_new_addr(ifa, rta_tb);
	else if (n->nlmsg_type == RTM_DELADDR)
		process_del_addr(ifa, rta_tb);

	return 0;
}

static void md_discover_router(struct tq_elem *tqe);

static void __md_discover_router(struct md_inet6_iface *iface)
{
	MDBG("discover link on iface %s (%d)\n", iface->name, iface->ifindex);

	if (md_is_link_up(iface) && 
	    iface->router_solicits++ <= iface->devconf[DEVCONF_RTR_SOLICITS]) {
		struct timespec exp_in;
		ndisc_send_rs(iface->ifindex, &in6addr_all_routers_mc, NULL, 0);
		tssetsec(exp_in, iface->devconf[DEVCONF_RTR_SOLICIT_INTERVAL]);
		add_task_rel(&exp_in, &iface->tqe, md_discover_router);
	}
}

static void md_discover_router(struct tq_elem *tqe)
{
	pthread_mutex_lock(&iface_lock);
	if (!task_interrupted()) {
		struct md_inet6_iface *iface;
		iface = tq_data(tqe, struct md_inet6_iface, tqe);
		__md_discover_router(iface);
	}
	pthread_mutex_unlock(&iface_lock);
}

static void md_check_expired_coas(struct md_inet6_iface *iface, 
				  struct md_router *rtr);

/* Called on netlink retrieved information for an address
   in order to update list of coa on that iface (function
   follows rtnl_filter_t prototype). */
static int apply_update_tunnel_coa(__attribute__ ((unused)) const struct sockaddr_nl *who,
				   struct nlmsghdr *n, void *arg)
{
	struct md_inet6_iface *iface = (struct md_inet6_iface *)arg;
	int ifindex = iface->ifindex;
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	struct rtattr *rta_tb[RTA_MAX+1];

	if (n->nlmsg_type == NLMSG_DONE ||
	    n->nlmsg_type == NLMSG_ERROR)
		return 0;

	if (n->nlmsg_type != RTM_NEWADDR ||
	    n->nlmsg_len < NLMSG_LENGTH(sizeof(*ifa)) ||
	    ifa->ifa_family != AF_INET6)
			return -EINVAL;

	if (ifa->ifa_flags & IFA_F_TENTATIVE)
		return 0;

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa),
		     n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));

	if (!rta_tb[IFA_ADDRESS])
		return 0;

	if (ifindex < 0 || ifa->ifa_index != (uint32_t)ifindex)
		return 0;

	update_coa(iface, ifa, rta_tb);
	return 0;
}

/* Grab the list of addresses configured on a provided
   tunnel interface and add usable ones to CoA list */
static int __md_update_tunnel_iface_coa_list(struct md_inet6_iface *iface)
{
	struct ifaddrmsg ifa;
	int len, res = 0;
	struct rtnl_handle rth;

	/* Open socket */
	if (rtnl_open_byproto(&rth, 0, NETLINK_ROUTE) < 0)
		return -1;

	/* Fill ifaddrmsg (will serve as request) */
	memset(&ifa, 0, sizeof(ifa));
	ifa.ifa_family = AF_INET6;
	ifa.ifa_prefixlen = 0;
	ifa.ifa_flags = IFA_F_PERMANENT;
	ifa.ifa_scope = RT_SCOPE_UNIVERSE;
	ifa.ifa_index = iface->ifindex;
	len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));

	/* Dump address information for previous request and work on
	 * every part of the result */
	if (rtnl_dump_request(&rth, RTM_GETADDR, &ifa, len) < 0 ||
	    rtnl_dump_filter(&rth, apply_update_tunnel_coa,
			     (void *)iface, NULL, NULL) < 0)
		res = -1;

	rtnl_close(&rth);
	return res;
}

static uint32_t md_router_compute_def_route_metric(uint16_t iface_pref,
						   uint8_t  rtr_pref);

/* called on all routes find by __md_update_tunnel_iface_routes */
static int apply_update_tunnel_route(__attribute__ ((unused)) const struct sockaddr_nl *who,
				     struct nlmsghdr *n, void *arg)
{
	struct md_inet6_iface *iface = (struct md_inet6_iface *)arg;
	uint32_t old_metric=0, new_metric;
	int ifindex = iface->ifindex;
	struct rtmsg *r = NLMSG_DATA(n);
	struct rtattr *rta_tb[RTA_MAX+1];
	struct rtattr *rta;
	int len = n->nlmsg_len;
	struct in6_addr *gateway = NULL;

	if (n->nlmsg_type == NLMSG_DONE ||
	    n->nlmsg_type == NLMSG_ERROR)
		return 0;

	if (n->nlmsg_type != RTM_NEWROUTE||
	    len < 0 || (size_t)len < NLMSG_LENGTH(sizeof(*r)) ||
	    r->rtm_family != AF_INET6) {
		return -EINVAL;
	}

	len -= NLMSG_LENGTH(sizeof(*r));
	if (len < 0)
		return -1;

	parse_rtattr(rta_tb, RTA_MAX, RTM_RTA(r), len);

	rta = rta_tb[RTA_OIF]; /* Main filter is ifindex */
	if (rta == NULL || *(int*)RTA_DATA(rta) != ifindex) {
		return 0;
	}

	/* Man page is explicit enough, we only deal with the default
	   route in main table by removing it and installing a new one
	   through the device with a modified metric. Gateway is kept
	   if present (for 6to4 mainly). We just skip other routes. */
	if (r->rtm_dst_len != 0 ||
	    r->rtm_src_len != 0 ||
	    r->rtm_table != RT_TABLE_MAIN ||
	    r->rtm_scope != RT_SCOPE_UNIVERSE ||
	    r->rtm_type != RTN_UNICAST ||
	    rta_tb[RTA_DST] != NULL ||
	    rta_tb[RTA_SRC] != NULL)
		return 0;

	if (rta_tb[RTA_GATEWAY]) {
		gateway = RTA_DATA(rta_tb[RTA_GATEWAY]);
	}

	if (rta_tb[RTA_PRIORITY])
		old_metric = *(uint32_t*)RTA_DATA(rta_tb[RTA_PRIORITY]);

	/* Compute new metric */
	new_metric = md_router_compute_def_route_metric(iface->preference, 0);

	/* No change needed */
	if (old_metric == new_metric)
		return 0;

	/* Different metrics, start by adding the new route and then remove
	 * the old one. Having done some tests, I cannot manage to do that
	 * reliably in a single operation (i.e. change the metric). --arno */
	route_add(ifindex, RT_TABLE_MAIN, RTPROT_STATIC, r->rtm_flags,
		  new_metric, &in6addr_any, 0, &in6addr_any, 0, gateway);
	route_del(ifindex, RT_TABLE_MAIN, old_metric,
		  &in6addr_any, 0, &in6addr_any, 0, gateway);

	return 0;
}

/* Grab routes that use provided interface, keep link local ones,
 * modify metric of default one(s ?) and remove others. */
static int __md_update_tunnel_iface_routes(struct md_inet6_iface *iface)
{
	struct rtmsg r;
	struct rtnl_handle rth;
	int len, res = 0;

	if (rtnl_open_byproto(&rth, 0, NETLINK_ROUTE) < 0)
		return -1;

	/* Fill rtmsg (will serve as request) */
	memset(&r, 0, sizeof(r));
	r.rtm_family = AF_INET6;
	r.rtm_table = RT_TABLE_MAIN;
	r.rtm_scope = RT_SCOPE_UNIVERSE;
	r.rtm_protocol = RTPROT_UNSPEC;
	r.rtm_type = RTN_UNICAST;
	len = NLMSG_LENGTH(sizeof(r));

	/* Dump route information for previous request and work on
	 * every part of the result */
	if (rtnl_dump_request(&rth, RTM_GETROUTE, &r, len) < 0 ||
	    rtnl_dump_filter(&rth, apply_update_tunnel_route,
			     (void *)iface, NULL, NULL) < 0)
		res = -1;

	rtnl_close(&rth);
	return res;
}

static void md_link_up(struct md_inet6_iface *iface)
{
	MDBG2("link up on iface %s (%d)\n", iface->name, iface->ifindex);

	if (iface->is_tunnel) {
		/* When interface is a tunnel, we do not deal with router
		 * discovery and interface configuration. We only:
		 * - grab and update the list of CoAs for the interface.
		 * - updates routes (i.e. change metric) */
		__md_update_tunnel_iface_coa_list(iface);
		__md_update_tunnel_iface_routes(iface);
	} else
		__md_discover_router(iface);
}

static void __md_new_link(struct md_inet6_iface *iface, int link_changed)
{
	MDBG2("new link on iface %s (%d)\n", iface->name, iface->ifindex);

	del_task(&iface->tqe);

	/* Skip router related updates for autoconfigured tunnel iface*/
	if (!iface->is_tunnel) {
		assert(!list_empty(&iface->default_rtr));
		iface->router_solicits = 0;

		if (link_changed) {
			struct list_head *l, *n;
			if (!iface->ll_dad_unsafe) {
				iface->iface_flags |= MD_LINK_LOCAL_DAD;
				addr_do(&iface->lladdr, 64, iface->ifindex,
					NULL, mn_lladdr_dad);
			}
			list_for_each_safe(l, n, &iface->backup_rtrs) {
				struct md_router *rtr;
				rtr = list_entry(l, struct md_router, list);
				md_expire_router(iface, rtr, NULL);
			}
		}
		md_flush_router_list(&iface->expired_rtrs);
	}

	md_flush_coa_list(&iface->expired_coas);
}


static struct md_inet6_iface *
md_create_inet6_iface(struct ifinfomsg *ifi, struct rtattr **rta_tb)
{
	struct md_inet6_iface *iface;
	int hwalen;
	
	/* Note: steps performed below are ok even for tunnel interfaces */
	if ((iface = malloc(sizeof(struct md_inet6_iface))) != NULL) {
		md_inet6_iface_init(iface, ifi->ifi_index);
		if (rta_tb[IFLA_IFNAME])
			strncpy(iface->name, RTA_DATA(rta_tb[IFLA_IFNAME]), 
				IF_NAMESIZE - 1);
		iface->link_flags = ifi->ifi_flags;
		iface->type = ifi->ifi_type;

		/* For interfaces w/ meaningful L2 addresses, if the address
		 * is available and ok, we derive a link-local address */
		if (rta_tb[IFLA_ADDRESS] != NULL  &&
		    (hwalen = ndisc_get_l2addr_len(ifi->ifi_type)) > 0 &&
		    rta_tb[IFLA_ADDRESS]->rta_len == RTA_LENGTH(hwalen)) {
			ndisc_set_linklocal(&iface->lladdr,
					    RTA_DATA(rta_tb[IFLA_ADDRESS]),
					    iface->type);
		}

		if (rta_tb[IFLA_PROTINFO]) {
			struct rtattr *inet6_tb[IFLA_INET6_MAX+1];

			memset(inet6_tb, 0, sizeof(inet6_tb));
		
			parse_rtattr(inet6_tb, IFLA_INET6_MAX, 
				     RTA_DATA(rta_tb[IFLA_PROTINFO]),
				     rta_tb[IFLA_PROTINFO]->rta_len);
		
			if (inet6_tb[IFLA_INET6_CONF]) {
				memcpy(iface->devconf, 
				       RTA_DATA(inet6_tb[IFLA_INET6_CONF]),
				       sizeof(iface->devconf));
			}
		}
		if (iface->devconf[DEVCONF_RTR_SOLICITS] <= 0)
			iface->devconf[DEVCONF_RTR_SOLICITS] = conf_default_rs;
		if (iface->devconf[DEVCONF_RTR_SOLICIT_INTERVAL] <= 0)
			iface->devconf[DEVCONF_RTR_SOLICIT_INTERVAL] = conf_default_rs_ival;
		MDBG3("creating iface %s (%d)\n", iface->name, iface->ifindex);
	}
	return iface;
}

static void iface_proc_entries_init(struct md_inet6_iface *iface)
{
	set_iface_proc_entry(PROC_SYS_IP6_FORWARDING, iface->name,
			     conf_forwarding);
	set_iface_proc_entry(PROC_SYS_IP6_AUTOCONF, iface->name,
			     conf_autoconf);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA, iface->name,
			     conf_ra);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA_DEFRTR, iface->name,
			     conf_ra_defrtr);
	set_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICITS, iface->name,
			     conf_rs);
	tssetmsec(iface->reachable, DEFAULT_REACHABLE_TIME);
	tssetmsec(iface->retransmit, DEFAULT_RETRANSMIT_TIMER);
}

static inline int link_flags_changed(unsigned int nf, unsigned int of)
{
	return (nf & (IFF_UP|IFF_RUNNING)) != (of & (IFF_UP|IFF_RUNNING));
}

/* Look for interface by name (if any) in the list of interfaces
 * referenced in the configuration and update its ifindex as the
 * interface is now available (dongle/adapter was plugged) */
static void iface_ifindex_update(int ifindex)
{
	char ifname[IF_NAMESIZE];
	struct list_head *list;

	/* We need the name of the interface to find it and
	 * update its ifindex */
	if (if_indextoname(ifindex, ifname) == NULL)
		return;

	list_for_each(list, &conf.net_ifaces) {
		struct net_iface *nif;
		nif = list_entry(list, struct net_iface, list);
		if (strncmp(ifname, nif->name, IF_NAMESIZE-1) == 0) {
			nif->ifindex = ifindex;
			break;
		}
	}
}

/* Look for interface by ifindex in the list of interfaces referenced
 * by configuration and invalidate its ifindex as the interface was
 * removed */
static void iface_ifindex_invalidate(int ifindex)
{
	struct list_head *list;

	/* Invalidate ifindex for that interface */
	list_for_each(list, &conf.net_ifaces) {
		struct net_iface *nif;
		nif = list_entry(list, struct net_iface, list);
		if (nif->ifindex == ifindex) {
			nif->ifindex = 0;
			break;
		}
	}
}

static int process_new_inet6_iface(struct ifinfomsg *ifi,
				   struct rtattr **rta_tb)
{
	struct md_inet6_iface *iface;

	if ((iface = md_get_inet6_iface(&ifaces, ifi->ifi_index)) == NULL) {
		unsigned int pref;

		iface_ifindex_update(ifi->ifi_index);

		if ((pref = conf.pmgr.accept_inet6_iface(ifi->ifi_index)) &&
		    (iface = md_create_inet6_iface(ifi, rta_tb)) != NULL) {
			MDBG2("adding iface %s (%d)\n",
			      iface->name, iface->ifindex);
			iface->preference = pref;
			list_add_tail(&iface->list, &ifaces);
			if (!iface->is_tunnel)
				iface_proc_entries_init(iface);
			if (md_is_link_up(iface))
				md_link_up(iface);
		}
	} else if (link_flags_changed(ifi->ifi_flags, iface->link_flags)) {
		iface->link_flags = ifi->ifi_flags;
		if (md_is_link_up(iface))
			md_link_up(iface);
		else 
			md_link_down(iface);
	}
	return 0;
}

static int 
process_del_inet6_iface(struct ifinfomsg *ifi)
{
       	struct md_inet6_iface *iface;
	if ((iface = md_get_inet6_iface(&ifaces, ifi->ifi_index)) != NULL) {
		MDBG2("deleting iface %s (%d)\n", iface->name, iface->ifindex);
		md_expire_inet6_iface(iface);
		md_free_inet6_iface(iface);
		iface_ifindex_invalidate(ifi->ifi_index);
	}
	return 0;
}

static int process_link(struct nlmsghdr *n)
{
	struct ifinfomsg *ifi;
	struct rtattr *rta_tb[IFLA_MAX+1];

	if (n->nlmsg_len < NLMSG_LENGTH(sizeof(*ifi)))
		return -1;

	ifi = NLMSG_DATA(n);

	if (ifi->ifi_family != AF_UNSPEC && ifi->ifi_family != AF_INET6)
		return 0;

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, IFLA_MAX, IFLA_RTA(ifi),
		     n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi)));

	/* Check if interface type is supported, i.e. if we have
	 * internal logic to perform minimal L2 operations. We do
	 * that by checking we do know HW address length. */
	if (ndisc_get_l2addr_len(ifi->ifi_type) < 0) {
		syslog(LOG_WARNING,
		       "Interface %d (%s):type %d unsupported",
		       ifi->ifi_index,
		       (char *) RTA_DATA(rta_tb[IFLA_IFNAME]),
		       ifi->ifi_type);
		return 0;
	}

	pthread_mutex_lock(&iface_lock);
	if (n->nlmsg_type == RTM_NEWLINK)
		process_new_inet6_iface(ifi, rta_tb);
	else if (n->nlmsg_type == RTM_DELLINK)
		process_del_inet6_iface(ifi);
	pthread_mutex_unlock(&iface_lock);

	return 0;
}

static void md_router_timeout(struct md_router *rtr);

static int process_fail_neigh(struct ndmsg *ndm, struct rtattr **rta_tb)
{
	struct md_inet6_iface *iface;
	struct md_router *rtr;
	struct in6_addr *addr;

	if (!nud_expire_rtr)
		return 0;

	iface = md_get_inet6_iface(&ifaces, ndm->ndm_ifindex);
	if (iface == NULL || iface->is_tunnel)
		return 0;

	rtr = md_get_first_router(&iface->default_rtr);
	if (rtr == NULL)
		return 0;

	addr = RTA_DATA(rta_tb[NDA_DST]);
	if (rtr_addr_chk(rtr,addr) || IN6_ARE_ADDR_EQUAL(&rtr->lladdr,addr))
		md_router_timeout(rtr);
	return 0;
}

static int process_neigh(struct nlmsghdr *n)
{
	struct ndmsg *ndm;
	struct rtattr *rta_tb[NDA_MAX+1];

	if (n->nlmsg_len < NLMSG_LENGTH(sizeof(*ndm)))
		return -1;

	ndm = NLMSG_DATA(n);

	if (ndm->ndm_family != AF_INET6 || !(ndm->ndm_state & NUD_FAILED))
		return 0;

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, NDA_MAX, NDA_RTA(ndm),
		     n->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm)));

	pthread_mutex_lock(&iface_lock);
	process_fail_neigh(ndm, rta_tb);
	pthread_mutex_unlock(&iface_lock);

	return 0;
}

static int process_nlmsg(__attribute__ ((unused)) const struct sockaddr_nl *who,
			 struct nlmsghdr *n,
			 __attribute__ ((unused)) void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	switch (n->nlmsg_type) {
	case RTM_NEWLINK:
	case RTM_DELLINK:
		/* interface or link, up or down */ 
		process_link(n);
		break;
	case RTM_NEWNEIGH:
		/* changes in reachability state of default router */
		process_neigh(n);
		break;
	case RTM_NEWADDR:
	case RTM_DELADDR:
		/* new or deleted CoAs */
		process_addr(n);
		break;
	default:
		/* To do: listen to changes in default and prefix routes(?) */
		break;
	}
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	return 0;
}

static struct prefix_list_entry *
md_create_router_prefix(struct md_router *rtr, 
			struct nd_opt_prefix_info *pinfo)
{
	struct prefix_list_entry *p;

	if (!(p = malloc(sizeof(struct prefix_list_entry))))
		return NULL;

	p->timestamp = rtr->timestamp;
	memcpy(&p->pinfo, pinfo, sizeof(struct nd_opt_prefix_info));

	if (p->ple_flags & ND_OPT_PI_FLAG_RADDR)
		rtr->raddr_cnt++;
	rtr->prefix_cnt++;

	MDBG3("creating new prefix %x:%x:%x:%x:%x:%x:%x:%x/%d\n", 
	     NIP6ADDR(&p->ple_prefix), p->ple_plen);

	return p;
}

static void md_check_home_link(struct md_inet6_iface *i, struct md_router *rtr)
{
	struct list_head *l;
	int home_link = 0;
	int ll_dad_unsafe = 0;

	/* Don't bother checking: MN can't be at home on a tunnel iface */
	if (!i->is_tunnel) {
		list_for_each(l, &conf.home_addrs) {
			struct home_addr_info *hai;
			hai = list_entry(l, struct home_addr_info, list);
			if (mn_is_at_home(&rtr->prefixes,
					  &hai->home_prefix,
					  hai->home_plen)) {
				home_link = 1;
				ll_dad_unsafe |= hai->lladdr_comp;
			}
		}
	}
	if (i->home_link != home_link)
		md_reset_egress_forward();
	i->home_link = home_link;
	i->ll_dad_unsafe = ll_dad_unsafe;
}

static struct md_router *md_create_router(struct md_inet6_iface *iface, 
					  const struct in6_addr *saddr, 
					  struct nd_router_advert *ra, int len)
{
	struct md_router *new;
	int optlen = len - sizeof(struct nd_router_advert);
	uint8_t *opt = (uint8_t *)(ra + 1);

	if (!(new = malloc(sizeof(struct md_router))))
		return NULL;

	memset(new, 0, sizeof(struct md_router));
	clock_gettime(CLOCK_REALTIME, &new->timestamp);
	INIT_LIST_HEAD(&new->prefixes);
	INIT_LIST_HEAD(&new->list);
	INIT_LIST_HEAD(&new->tqe.list);

	while (optlen > 1) {
		uint16_t olen = opt[1] << 3;

		if (olen > (unsigned int)optlen || olen == 0)
			goto free_rtr;
		switch (opt[0]) {
			struct nd_opt_prefix_info *pinfo;
			struct prefix_list_entry *p;
			struct nd_opt_mtu *mtu;
			struct nd_opt_adv_interval *r;

		case ND_OPT_SOURCE_LINKADDR:
			new->hwalen = ndisc_l2addr_from_opt(iface->type,
							    new->hwa,
							    &opt[2], olen-2);
			if (new->hwalen < 0)
				goto free_rtr;
			break;

		case ND_OPT_PREFIX_INFORMATION:
			if (olen < sizeof(struct nd_opt_prefix_info))
				goto free_rtr;

			pinfo = (struct nd_opt_prefix_info *)opt;
			/* internal representation host byte order */
			pinfo->nd_opt_pi_valid_time = 
				ntohl(pinfo->nd_opt_pi_valid_time);
			pinfo->nd_opt_pi_preferred_time =
				ntohl(pinfo->nd_opt_pi_preferred_time);

			if (pinfo->nd_opt_pi_prefix_len > 128 ||
			    !(p = md_create_router_prefix(new, pinfo)))
				goto free_rtr;

			list_add_tail(&p->list, &new->prefixes);
			break;

		case ND_OPT_MTU:
			if (olen < sizeof(struct nd_opt_mtu))
				goto free_rtr;

			mtu = (struct nd_opt_mtu *)opt;
			new->mtu = ntohl(mtu->nd_opt_mtu_mtu);
			break;

		case ND_OPT_RTR_ADV_INTERVAL:
			if (olen < sizeof(struct nd_opt_adv_interval))
				goto free_rtr;

			r = (struct nd_opt_adv_interval *) opt;
			tssetmsec(new->adv_ival,
				  ntohl(r->nd_opt_adv_interval_ival));
			break;
		}
		optlen -= olen;
		opt += olen;
	}
	if (new->prefix_cnt == 0)
		goto free_rtr;
	
	new->iface = iface;
	new->hoplimit = ra->nd_ra_curhoplimit;
	new->ra_flags = ra->nd_ra_flags_reserved;
	tssetsec(new->rtr_lifetime, ntohs(ra->nd_ra_router_lifetime));
	tssetmsec(new->reachable, ntohl(ra->nd_ra_reachable));
	tssetmsec(new->retransmit, ntohl(ra->nd_ra_retransmit));

	if (tsisset(new->adv_ival)) {
		struct timespec tmp;
		tssetmsec(tmp, tstomsec(new->adv_ival) * adv_ivals_md_trigger);
		new->lifetime = tsmin(tmp, new->rtr_lifetime);
	} else
		new->lifetime = new->rtr_lifetime;

	new->lladdr = *saddr;
	new->ifindex = iface->ifindex;

	MDBG3("creating new router "
	      "%x:%x:%x:%x:%x:%x:%x:%x on interface %s (%d)\n", 
	     NIP6ADDR(saddr), iface->name, iface->ifindex);

	return new;
free_rtr:
	__md_free_router(new);
	return NULL;
}

int rtr_addr_chk(struct md_router *rtr, struct in6_addr *rtr_addr)
{
	if (rtr->raddr_cnt > 0) {
		struct list_head *lp;
		list_for_each(lp, &rtr->prefixes) {
			struct prefix_list_entry *p;
			p = list_entry(lp, struct prefix_list_entry, list);
			if (p->ple_flags & ND_OPT_PI_FLAG_RADDR &&
			    IN6_ARE_ADDR_EQUAL(&p->ple_prefix, rtr_addr))
				return 1;
		}
	}
	return 0;
}

static int rtr_addr_match(struct md_router *r1, struct md_router *r2)
{
	struct list_head *l1;

	list_for_each(l1, &r1->prefixes) {
		struct prefix_list_entry *p1;
		struct list_head *l2;

		p1 = list_entry(l1, struct prefix_list_entry, list);

		if (!(p1->ple_flags & ND_OPT_PI_FLAG_RADDR))
			continue;

		list_for_each(l2, &r2->prefixes) {
			struct prefix_list_entry *p2;

			p2 = list_entry(l2, struct prefix_list_entry, list);
			if (p2->ple_flags & ND_OPT_PI_FLAG_RADDR &&
			    p1->ple_plen == p2->ple_plen &&
			    !IN6_ARE_ADDR_EQUAL(&p1->ple_prefix, 
						&p2->ple_prefix))
				return 0;
			
		}
	}
	return 1;
}

static int md_router_cmp(struct md_router *new, struct md_router *old)
{
	/* Neither router nor link-local addresses match; different nodes */
	if (IN6_ARE_ADDR_EQUAL(&new->lladdr, &old->lladdr)) {
		if (new->raddr_cnt > 0 || old->raddr_cnt > 0)  
			return !rtr_addr_match(new, old);
		return 0;
	}
        return -1;
}

static struct in6_addr *md_get_rtr_addr(struct md_router *rtr)
{
	if (rtr->raddr_cnt > 0) {
		struct list_head *list;
		list_for_each(list, &rtr->prefixes) {
			struct prefix_list_entry *p;
			p = list_entry(list, struct prefix_list_entry, list);
			if (p->ple_flags & ND_OPT_PI_FLAG_RADDR)
				return &p->ple_prefix;
		}

	}
	return &rtr->lladdr;
}

static void md_router_timeout_probe(struct tq_elem *tqe);

static void md_probe_router(struct md_router *rtr)
{
	if (md_is_link_up(rtr->iface)) {
		struct in6_addr *rtr_addr = md_get_rtr_addr(rtr);
		struct timespec expires;

		MDBG2("preform NUD for router "
		      "%x:%x:%x:%x:%x:%x:%x:%x on interface %s (%d)\n", 
		      NIP6ADDR(&rtr->lladdr), rtr->iface->name,
		      rtr->iface->ifindex);

		rtr->probed = 1;
		rtr->solicited_addr = *rtr_addr;

		neigh_add(rtr->ifindex, NUD_PROBE, NTF_ROUTER,
			  rtr_addr, rtr->hwalen > 0 ? rtr->hwa : NULL,
			  rtr->hwalen, 1);

		clock_gettime(CLOCK_REALTIME, &rtr->timestamp);
		tssetmsec(rtr->lifetime,
			  tstomsec(rtr->iface->retransmit) *
			  conf.MnRouterProbes);
		if (tsisset(conf.MnRouterProbeTimeout_ts) &&
		    tsbefore(rtr->lifetime, conf.MnRouterProbeTimeout_ts))
			rtr->lifetime = conf.MnRouterProbeTimeout_ts;
		tsadd(rtr->lifetime, rtr->timestamp, expires);
		add_task_abs(&expires, &rtr->tqe, md_router_timeout_probe);
	}
}

static void md_update_backup_router_stats(struct md_router *rtr,
					  struct timespec *now,
					  struct timespec *tstamp)
{
	struct list_head *l;
	list_for_each(l, &rtr->prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(l, struct prefix_list_entry, list);
		p->ple_valid_time = mpd_curr_lft(now, tstamp, 
						 p->ple_valid_time);
		p->ple_prefd_time = mpd_curr_lft(now, tstamp, 
						 p->ple_prefd_time);;
	}
}

static void md_change_default_router(struct md_inet6_iface *iface,
				     struct md_router *new,
 				     struct md_router *old);

static int md_change_to_backup_router(struct md_inet6_iface *iface,
				      struct md_router *old)
{
	struct list_head *l, *n;

	list_for_each_safe(l, n, &iface->backup_rtrs) {
		struct md_router *new = list_entry(l, struct md_router, list);
		struct timespec expires;

		tsadd(new->timestamp, new->lifetime, expires);

		if (tsafter(old->timestamp, expires)) {
			del_task(&new->tqe);
			list_del(&new->list);
			md_update_backup_router_stats(new,
						      &old->timestamp, 
						      &new->timestamp);
			md_change_default_router(iface, new, old);
			return 0;
		}
	}
	return -1;
}

static void md_router_timeout(struct md_router *rtr)
{
	struct md_inet6_iface *iface = rtr->iface;

	MDBG2("expire router %x:%x:%x:%x:%x:%x:%x:%x "
	      "without NUD on interface %s (%d)\n", 
	      NIP6ADDR(&rtr->lladdr), iface->name,
	      iface->ifindex);

	clock_gettime(CLOCK_REALTIME, &rtr->timestamp);

	if (rtr->probed)
		neigh_del(rtr->ifindex, &rtr->solicited_addr);

	if (rtr->used) {
		if (list_empty(&iface->backup_rtrs) ||
		    md_change_to_backup_router(iface, rtr)) {
			md_expire_router(iface, rtr, NULL);
			__md_discover_router(rtr->iface);
			__md_trigger_movement_event(ME_RTR_EXPIRED, 0, 
						    rtr->iface, NULL);
		}
	} else
		md_expire_router(iface, rtr, NULL);
}

static void md_router_timeout_probe(struct tq_elem *tqe)
{
	pthread_mutex_lock(&iface_lock);
	if (!task_interrupted()) {
		struct md_router *rtr = tq_data(tqe, struct md_router, tqe);
		if (rtr->probed)
			md_router_timeout(rtr);
		else
			md_probe_router(rtr);
	}
	pthread_mutex_unlock(&iface_lock);
}

/* Some notes on route metric, interface preferences and router preferences:
 *
 * Below, we deal with the metric associated with default routes. As a
 * remainder, on Linux, the higher the metric value on the route, the
 * lower the priority of the route. Usual IPv6 routes installed by kernel
 * from RA are given a priority of 1024.
 *
 * In UMIP, the *basis* we use for the default metric we use is 1023
 * (DEFAULT_ROUTE_METRIC as defined below). This value is not used
 * directly for the route metric. Keep reading.
 *
 * In UMIP, interfaces are given preference values, used by the policy
 * manager for the selection of interfaces. Preference values are
 * in the range [1, POL_MN_IF_MIN_PREFERENCE] (i.e. 10). The higher the
 * value, the lower the preference. Note that 0 means the interface
 * will not be used. The preference value for a given interface is also
 * used in the computation of the metric m for the default route associated
 * with the interface:
 *
 *  m = DEFAULT_ROUTE_METRIC - 3*(POL_MN_IF_MIN_PREFERENCE - ifpref)
 *
 * And because multiple routers may be available on a subnet associated
 * with a given interface and may report specific router preference
 * values, i.e. 'low' (3), 'medium' (0) or 'high' (1) (see Section 2.1 of
 * RFC 4191), the router preference advertised by a router is also used
 * in the computation of the *final* metric (hence the 3 above) for the
 * default route via a given router:
 *
 *   rtrpref is 'low' (-1)  =>  m = m-1
 *   rtrpref is 'high' (1)  =>  m = m+1
 *   rtrpref is sth else    =>  m is untouched
 */

#define DEFAULT_ROUTE_METRIC 1023

/* Return 1 if new router has a strictly lower default router preference
 * value than old one. */
static int md_router_prefer_old(struct md_router *old, struct md_router *new)
{
	uint8_t old_prf_flag = (old->ra_flags >> 3) & 0x03;
	uint8_t new_prf_flag = (new->ra_flags >> 3) & 0x03;
	int old_prf_val, new_prf_val;

	/* Map flags (low, medium, high) to -1, 0, 1. Reserved val (0b00)
	 * is mapped to medium, i.e. 0. */
	old_prf_val = ((old_prf_flag >> 1) ? -1 : 1) * (old_prf_flag & 0x1);
	new_prf_val = ((new_prf_flag >> 1) ? -1 : 1) * (new_prf_flag & 0x1);

	return (new_prf_val < old_prf_val);
}

/* Given the interface preference between 1 and POL_MN_IF_MIN_PREFERENCE
 * and a router preference value (0 if none), the function returns a metric
 * to use for the default route using that interface via that router. */
static uint32_t md_router_compute_def_route_metric(uint16_t iface_pref,
						   uint8_t  rtr_pref)
{
	uint32_t metric = DEFAULT_ROUTE_METRIC;
	metric -= 3*(POL_MN_IF_MIN_PREFERENCE - iface_pref);

	if (iface_pref > POL_MN_IF_MIN_PREFERENCE ||
	    iface_pref == 0) /* 0 should not be met */
		iface_pref = POL_MN_IF_MIN_PREFERENCE;

	/* "sub-modulate" with possible def rtr pref from RA. We
	 * simply consider it as a two-bit signed integer here */
	metric -= (((rtr_pref & 0x3) >> 1) ? -1 : 1) * (rtr_pref & 0x1);

	return metric;
}

static void md_update_router_stats(struct md_router *rtr)
{
	struct list_head *list;
	struct in6_addr coa;
	uint8_t rtr_pref = (rtr->ra_flags >> 3) & 0x03;
	uint32_t metric;
	uint16_t iface_pref = POL_MN_IF_MIN_PREFERENCE;
	struct md_inet6_iface *iface;

	neigh_add(rtr->ifindex, NUD_STALE, NTF_ROUTER,
		  &rtr->lladdr, rtr->hwa, rtr->hwalen, 1);

	/* Deal with interface preference as set by user ... */
	if ((iface = md_get_inet6_iface(&ifaces, rtr->ifindex)) == NULL)
		MDBG2("Router we are inserting a route for is "
		      "reachable via an unknown interface (%d)\n",
		      rtr->ifindex);
	else
		iface_pref = iface->preference;

	metric = md_router_compute_def_route_metric(iface_pref, rtr_pref);

	MDBG2("adding default route via %x:%x:%x:%x:%x:%x:%x:%x with metric"
	      " %d\n", NIP6ADDR(&rtr->lladdr), metric);

	route_add(rtr->ifindex, RT_TABLE_MAIN, RTPROT_RA,
		  RTF_DEFAULT|RTF_ADDRCONF, metric,
		  &in6addr_any, 0, &in6addr_any, 0, &rtr->lladdr);
	
	list_for_each(list, &rtr->prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(list, struct prefix_list_entry, list);
		/* pass prefix to kernel if it was included in the latest RA */
		if (!tsbefore(rtr->timestamp, p->timestamp) &&
		    p->ple_prefd_time <= p->ple_valid_time) {
			ipv6_addr_set(&coa,
				      (&p->ple_prefix)->s6_addr32[0],
				      (&p->ple_prefix)->s6_addr32[1],
				      (&(rtr->iface)->lladdr)->s6_addr32[2],
				      (&(rtr->iface)->lladdr)->s6_addr32[3]);

			MDBG("Adding CoA %x:%x:%x:%x:%x:%x:%x:%x on interface"
			     " (%d)\n", NIP6ADDR(&coa),rtr->ifindex);

			addr_add(&coa, p->ple_plen, 0, RT_SCOPE_UNIVERSE,
				 rtr->ifindex, p->ple_prefd_time,
				 p->ple_valid_time);

			if (p->ple_flags & ND_OPT_PI_FLAG_RADDR)
				neigh_add(rtr->ifindex, NUD_STALE,
					  NTF_ROUTER, &p->ple_prefix,
					  rtr->hwa, rtr->hwalen, 1);
		}
	}
	if (rtr->hoplimit != 0) {
		set_iface_proc_entry(PROC_SYS_IP6_CURHLIM,
				     rtr->iface->name, rtr->hoplimit);
	} else {
		set_iface_proc_entry(PROC_SYS_IP6_CURHLIM,
				     rtr->iface->name, DEFAULT_HOP_LIMIT);
	}
	if (rtr->mtu >= IP6_MIN_MTU)
		set_iface_proc_entry(PROC_SYS_IP6_LINKMTU,
				     rtr->iface->name, rtr->mtu);
	if (tsisset(rtr->reachable)) {
		set_iface_proc_entry(PROC_SYS_IP6_BASEREACHTIME_MS,
				     rtr->iface->name, 
				     tstomsec(rtr->reachable));
		rtr->iface->reachable = rtr->reachable;
	} else {
		set_iface_proc_entry(PROC_SYS_IP6_BASEREACHTIME_MS,
				     rtr->iface->name, 
				     DEFAULT_REACHABLE_TIME);
		tssetmsec(rtr->iface->reachable, DEFAULT_REACHABLE_TIME);
	}
	if (tsisset(rtr->retransmit)) {
		set_iface_proc_entry(PROC_SYS_IP6_RETRANSTIMER_MS,
				     rtr->iface->name, 
				     tstomsec(rtr->retransmit));
		rtr->iface->retransmit = rtr->retransmit;
	} else {
		set_iface_proc_entry(PROC_SYS_IP6_RETRANSTIMER_MS,
				     rtr->iface->name, 
				     DEFAULT_RETRANSMIT_TIMER);
		tssetmsec(rtr->iface->retransmit, DEFAULT_RETRANSMIT_TIMER);
	}
}

static void md_prefix_rule_add(struct prefix_list_entry *p)
{
	struct in6_addr prefix;
	ipv6_addr_prefix(&prefix, &p->ple_prefix, p->ple_plen);
	rule_add(NULL, RT6_TABLE_MAIN, IP6_RULE_PRIO_MIP6_COA_OUT,
		 RTN_UNICAST, &prefix, p->ple_plen,
		 &in6addr_any, 0, 0);
}

static void md_update_router(struct md_router *new, struct md_router *old)
{
	struct list_head *lnew, *n;

	MDBG2("updating router %x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
	      NIP6ADDR(&old->lladdr), old->iface->name, old->iface->ifindex);

	if (tsisset(old->lifetime))
		del_task(&old->tqe);

	old->timestamp = new->timestamp;
	old->adv_ival = new->adv_ival;
	old->rtr_lifetime = new->rtr_lifetime;
	old->hoplimit = new->hoplimit;
	old->ra_flags = new->ra_flags;
	old->reachable = new->reachable;
	old->retransmit = new->retransmit;
	old->mtu = new->mtu;
	old->lifetime = new->lifetime;

	list_for_each_safe(lnew, n, &new->prefixes) {
		struct prefix_list_entry *pnew, *pold;

		pnew = list_entry(lnew, struct prefix_list_entry, list);
		if ((pold = prefix_list_get(&old->prefixes,
					    &pnew->ple_prefix,
					    pnew->ple_plen)) != NULL) {
			pold->timestamp = pnew->timestamp;
			pold->ple_flags = pnew->ple_flags;
			pold->ple_valid_time = pnew->ple_valid_time;
			pold->ple_prefd_time = pnew->ple_prefd_time;
		} else {
			if (old->used)
				md_prefix_rule_add(pnew);
			list_del(lnew);
			list_add_tail(lnew, &old->prefixes);
			old->prefix_cnt++;
			if (pnew->ple_flags & ND_OPT_PI_FLAG_RADDR)
				old->raddr_cnt++;
		}
	}
	__md_free_router(new);
	if (old->used)
		md_update_router_stats(old);
	if (tsisset(old->lifetime)) {
		struct timespec expires;
		tsadd(old->lifetime, old->timestamp, expires);
		add_task_abs(&expires, &old->tqe, md_router_timeout_probe);
	}
}

static int md_block_rule_add(struct md_inet6_iface *iface)
{
	iface->iface_flags |= MD_BLOCK_TRAFFIC;
	/* Allow DAD probes and RS messages */
	rule_add(NULL, RT6_TABLE_MAIN,
		 IP6_RULE_PRIO_MIP6_COA_OUT, RTN_UNICAST,
		 &in6addr_any, 128, &in6addr_any, 0, 0);
	/* drop outgoing global traffic until DAD has been performed
	   on CoA to make routing and tunnel end-point updates atomic
	   during handoff */
	return rule_add(NULL, 0,
			IP6_RULE_PRIO_MIP6_BLOCK_HOA, RTN_BLACKHOLE,
			&in6addr_any, 0, &in6addr_any, 0, 0);
}

static void md_add_default_router(struct md_inet6_iface *iface,
				  struct md_router *rtr)
{
	assert(list_empty(&iface->default_rtr));
	assert(rtr->used);
	list_add(&rtr->list, &iface->default_rtr);
	md_check_home_link(iface, rtr);
}

static void md_change_default_router(struct md_inet6_iface *iface,
				     struct md_router *new,
 				     struct md_router *old)
{
	struct timespec expires;
	struct list_head *l;
	int link_changed = 1;
	if (!tsisset(new->lifetime)) {
		__md_free_router(new);
		return;
	}
	MDBG("add new router %x:%x:%x:%x:%x:%x:%x:%x on interface %s (%d)\n", 
	     NIP6ADDR(&new->lladdr), new->iface->name, new->iface->ifindex);

	list_for_each(l, &new->prefixes) {
		struct prefix_list_entry *p;

		p = list_entry(l, struct prefix_list_entry, list);

		if (old && prefix_list_find(&old->prefixes, 
					    &p->ple_prefix,
					    p->ple_plen))
			link_changed = 0;
		else
			md_prefix_rule_add(p);
	}
	if (old)
		md_expire_router(iface, old, new);

	if (list_empty(&iface->coas) &&
	    !(iface->iface_flags & MD_BLOCK_TRAFFIC)) {
		md_block_rule_add(iface);
	}
	new->used = 1;
	md_add_default_router(iface, new);
	__md_new_link(iface, link_changed);

	md_update_router_stats(new);
	tsadd(new->lifetime, new->timestamp, expires);
	add_task_abs(&expires, &new->tqe, md_router_timeout_probe);

	if (!list_empty(&iface->coas) &&
	    !(iface->iface_flags & MD_LINK_LOCAL_DAD))
		__md_trigger_movement_event(ME_RTR_NEW, 0, iface, NULL);
}

static void
md_check_expired_coas(struct md_inet6_iface *iface, struct md_router *rtr)
{
	struct list_head *clist, *n;
	struct list_head *plist;
	
	list_for_each(plist, &rtr->prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(plist, struct prefix_list_entry, list);
		list_for_each_safe(clist, n, &iface->expired_coas) { 
			struct md_coa *coa;
			coa = list_entry(clist, struct md_coa, list);

			if (ipv6_pfx_cmp(&p->ple_prefix,  
					 &coa->addr, coa->plen))
				continue;

			MDBG2("CoA %x:%x:%x:%x:%x:%x:%x:%x still usable\n", 
			      NIP6ADDR(&coa->addr));

			list_del(&coa->list);
			list_add_tail(&coa->list, &iface->coas);
		}
	}
}


static int 
md_check_expired_routers(struct md_inet6_iface *iface, struct md_router *new)
{
	struct list_head *list, *n;
	
	list_for_each_safe(list, n, &iface->expired_rtrs) {
		struct md_router *old;
		old = list_entry(list, struct md_router, list);
		if (!md_router_cmp(new, old)) {
			struct timespec diff;
			long passed_ms, valid_ms;

			/* don't used previously unused expired md_routers
			   since they might contain outdated information */
			if (!old->used)
				return 0;

			tssub(new->timestamp, old->timestamp, diff);
			passed_ms = tstomsec(diff);
			valid_ms = adv_ivals_dad_limit * 
				tstomsec(old->adv_ival);
			
			if (passed_ms >= valid_ms)
				break;

			MDBG2("router %x:%x:%x:%x:%x:%x:%x:%x still usable\n", 
			      NIP6ADDR(&old->lladdr));

			md_update_router(new, old);
			list_del(&old->list);
			md_add_default_router(iface, old);
			md_check_expired_coas(iface, old);
			__md_new_link(iface, 0);
			if (!list_empty(&iface->coas) &&
			    !(iface->iface_flags & MD_LINK_LOCAL_DAD))
				__md_trigger_movement_event(ME_RTR_BACK, 0,
							    iface, NULL);
			return 1;
		}
		
	}
	return 0;
}

static int
md_check_backup_routers(struct md_inet6_iface *iface, struct md_router *new)
{
	struct list_head *list, *n;
	
	list_for_each_safe(list, n, &iface->backup_rtrs) {
		struct md_router *old;
		old = list_entry(list, struct md_router, list);
		if (!md_router_cmp(new, old)) {
			md_update_router(new, old);
			if (!tsisset(old->lifetime)) {
				md_expire_router(iface, old, NULL);
			}
			return 1;
		}
	}
	return 0;
}

static void md_add_backup_router(struct md_inet6_iface *iface,
				 struct md_router *new)
{
	struct timespec expires;

	if (!tsisset(new->lifetime)) {
		__md_free_router(new);
		return;
	}

	MDBG("add new backup router %x:%x:%x:%x:%x:%x:%x:%x on interface %s (%d)\n", 
	     NIP6ADDR(&new->lladdr), new->iface->name, new->iface->ifindex);

	list_add(&new->list, &iface->backup_rtrs);

	tsadd(new->lifetime, new->timestamp, expires);
	add_task_abs(&expires, &new->tqe, md_router_timeout_probe);
}

static void 
md_check_default_router(struct md_inet6_iface *iface, struct md_router *new)
{
	struct md_router *old;

	MDBG2("looking for existing routers on iface %s (%d)\n", 
	      iface->name, iface->ifindex);

	if ((old = md_get_first_router(&iface->default_rtr)) == NULL)
		goto change_def_rtr;

	/* We had a default router referenced on that interface (old).
	 * Now check if new one and old one are in fact the same. */
	if (md_router_cmp(new, old))
		goto new_router_found;

	md_update_router(new, old); /* new: freed in update */
	if (!tsisset(old->lifetime)) {
		md_expire_router(iface, old, NULL);
		__md_discover_router(iface);
		__md_trigger_movement_event(ME_RTR_EXPIRED, 0, iface, NULL);
	} else {
		__md_new_link(iface, 0);
		__md_trigger_movement_event(ME_RTR_UPDATED, 0, iface, NULL);
	}
	return;

 new_router_found:
	/* There are some cases for which we want to perform NUD against
	 * old router instead of directly switching to new:
	 *  - if new has lower default router preference value
	 *  - if asked by configuration to perform NUD before switching
	 * In that case, the new router becomes a backup router */
	if (md_router_prefer_old(old, new) || conf.MnRouterProbes > 0) {
		md_probe_router(old);
		md_add_backup_router(iface, new);
		return;
	}

 change_def_rtr:
	md_change_default_router(iface, new, old);
}

static void md_recv_na(const struct icmp6_hdr *ih, ssize_t len,
		       __attribute__ ((unused)) const struct in6_addr *saddr,
		       const struct in6_addr *daddr, int iif, int hoplimit)
{
	struct nd_neighbor_advert *na = (struct nd_neighbor_advert *)ih;
	int optlen;
	struct md_inet6_iface *iface;
	struct md_router *rtr;
	uint8_t *opt;
	uint8_t hwa[L2ADDR_MAX_SIZE];
	int hwalen = 0;

	if (hoplimit < 255 || ih->icmp6_code != 0 ||
	    len < 0 || (size_t)len < sizeof(struct nd_neighbor_advert) ||
	    IN6_IS_ADDR_MULTICAST(&na->nd_na_target) ||
	    (na->nd_na_flags_reserved & ND_NA_FLAG_SOLICITED &&
	     IN6_IS_ADDR_MULTICAST(daddr)))
		return;

	pthread_mutex_lock(&iface_lock);

	if ((iface = md_get_inet6_iface(&ifaces, iif)) == NULL ||
	    (rtr = md_get_first_router(&iface->default_rtr)) == NULL ||
	    rtr->probed == 0 || rtr->hwalen < 0 ||
	    !IN6_ARE_ADDR_EQUAL(&rtr->solicited_addr, &na->nd_na_target)) {
		goto out;
	}
	optlen = len - sizeof(struct nd_neighbor_advert);
	opt = (uint8_t *)(na + 1);

	MDBG2("received NA from %x:%x:%x:%x:%x:%x:%x:%x on iface %s (%d)\n", 
	      NIP6ADDR(&rtr->lladdr), iface->name, iface->ifindex);
		
		
	while (optlen > 1) {
		int olen = opt[1] << 3;
		
		if (olen > optlen || olen == 0) 
			goto out;
		
		switch (opt[0]) {
		case ND_OPT_TARGET_LINKADDR:
			hwalen = ndisc_l2addr_from_opt(iface->type, hwa,
						       &opt[2], olen - 2);
			if (hwalen < 0 || hwalen != rtr->hwalen)
				goto out;
			break;
		}
		optlen -= olen;
		opt += olen;
	}
	if (na->nd_na_flags_reserved & ND_NA_FLAG_ROUTER &&
	    (hwalen || !memcmp(hwa, &rtr->hwa, rtr->hwalen))) {
		struct timespec expires;
		clock_gettime(CLOCK_REALTIME, &rtr->timestamp);
		if (tsisset(rtr->lifetime))
			del_task(&rtr->tqe);
		rtr->lifetime = rtr->iface->reachable;
		rtr->solicited_addr = in6addr_any;
		rtr->probed = 0;
		tsadd(rtr->lifetime, rtr->timestamp, expires);
		add_task_abs(&expires, &rtr->tqe, md_router_timeout_probe);
		if (!list_empty(&iface->coas) &&
		    !(iface->iface_flags & MD_LINK_LOCAL_DAD))
			__md_trigger_movement_event(ME_RTR_BACK, 0,
						    iface, NULL);
	} else {
		if (tsisset(rtr->lifetime))
			del_task(&rtr->tqe);
		md_router_timeout(rtr);
	}
out:
	pthread_mutex_unlock(&iface_lock);
}

static struct icmp6_handler md_na_handler = {
	.recv = md_recv_na,
};

static void md_recv_ra(const struct icmp6_hdr *ih, ssize_t len,
		       const struct in6_addr *saddr,
		       const struct in6_addr *daddr, int iif, int hoplimit)
{
	struct nd_router_advert *ra = (struct nd_router_advert *)ih;
	struct md_inet6_iface *iface;
	struct md_router *new;

	/* validity checks */
	if (hoplimit < 255 || !IN6_IS_ADDR_LINKLOCAL(saddr) ||
	    ih->icmp6_code != 0 || len < 0 ||
	    (size_t)len < sizeof(struct nd_router_advert) ||
	    !conf.pmgr.accept_ra(iif, saddr, daddr, ra))
		return;

	MDBG2("received RA from %x:%x:%x:%x:%x:%x:%x:%x on iface %d\n", 
	      NIP6ADDR(saddr), iif);

	pthread_mutex_lock(&iface_lock);
	if ((iface = md_get_inet6_iface(&ifaces, iif)) != NULL &&
	    (!iface->is_tunnel) &&
	    (new = md_create_router(iface, saddr, ra, len)) != NULL) {
		if (!md_check_expired_routers(iface, new) &&
		    !md_check_backup_routers(iface, new))
			md_check_default_router(iface, new);
	}
	pthread_mutex_unlock(&iface_lock);
}

static struct icmp6_handler md_ra_handler = {
	.recv = md_recv_ra,
};

struct rtnl_handle md_rth;

static void *md_nl_listen(__attribute__ ((unused)) void *arg)
{
	pthread_dbg("thread started");
	rtnl_listen(&md_rth, process_nlmsg, NULL);
	pthread_exit(NULL);
}

static void iface_default_proc_entries_init(void)
{
	get_iface_proc_entry(PROC_SYS_IP6_AUTOCONF,
			     "default", &conf_default_autoconf);
	get_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA,
			     "default", &conf_default_ra);
	get_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA_DEFRTR,
			     "default", &conf_default_ra_defrtr);
	get_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICITS,
			     "default", &conf_default_rs);
	get_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICIT_INTERVAL,
			     "default", &conf_default_rs_ival);

	set_iface_proc_entry(PROC_SYS_IP6_AUTOCONF,
			     "default", conf_autoconf);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA,
			     "default", conf_ra);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA_DEFRTR,
			     "default", conf_ra_defrtr);
	set_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICITS,
			     "default", conf_rs);
}

int md_init(void)
{
	int err;
	pthread_mutexattr_t mattrs;
	int val;

	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&iface_lock, &mattrs))
		return -1;

	if ((err = rtnl_route_open(&md_rth, 0)) < 0)
		return err;

	val = RTNLGRP_LINK;
	if (setsockopt(md_rth.fd, SOL_NETLINK,
		       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0) {
		dbg("%d %s\n", __LINE__, strerror(errno));  
		return -1;
	}
	val = RTNLGRP_NEIGH;
	if (setsockopt(md_rth.fd, SOL_NETLINK,
		       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0) {
		dbg("%d %s\n", __LINE__, strerror(errno));  
		return -1;
	}
	val = RTNLGRP_IPV6_IFADDR;
	if (setsockopt(md_rth.fd, SOL_NETLINK,
		       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0) {
		dbg("%d %s\n", __LINE__, strerror(errno));  
		return -1;
	}
	val = RTNLGRP_IPV6_IFINFO;
	if (setsockopt(md_rth.fd, SOL_NETLINK,
		       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0) {
		dbg("%d %s\n", __LINE__, strerror(errno));  
		return -1;
	}
	iface_default_proc_entries_init();
	return 0;
}

int md_start(void)
{
	icmp6_handler_reg(ND_NEIGHBOR_ADVERT, &md_na_handler);
	icmp6_handler_reg(ND_ROUTER_ADVERT, &md_ra_handler);
	if (pthread_create(&md_listener, NULL, md_nl_listen, NULL))
		return -1;
	inet6_ifaces_iterate(process_nlmsg, NULL);
	return 0;
}

void md_stop(void)
{
	rtnl_close(&md_rth);
	icmp6_handler_dereg(ND_ROUTER_ADVERT, &md_ra_handler);
	icmp6_handler_dereg(ND_NEIGHBOR_ADVERT, &md_na_handler);
	pthread_cancel(md_listener);
	pthread_join(md_listener, NULL);
}

static void iface_default_proc_entries_cleanup(void)
{
	set_iface_proc_entry(PROC_SYS_IP6_AUTOCONF,
			     "default", conf_default_autoconf);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA,
			     "default", conf_default_ra);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA_DEFRTR,
			     "default", conf_default_ra_defrtr);
	set_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICITS,
			     "default", conf_default_rs);
}

static void iface_proc_entries_cleanup(struct md_inet6_iface *iface)
{
	set_iface_proc_entry(PROC_SYS_IP6_FORWARDING, iface->name,
			     iface->devconf[DEVCONF_FORWARDING]);
	set_iface_proc_entry(PROC_SYS_IP6_AUTOCONF, iface->name,
			     iface->devconf[DEVCONF_AUTOCONF]);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA, iface->name,
			     iface->devconf[DEVCONF_ACCEPT_RA]);
	set_iface_proc_entry(PROC_SYS_IP6_ACCEPT_RA_DEFRTR, iface->name,
			     iface->devconf[DEVCONF_ACCEPT_RA_DEFRTR]);
	set_iface_proc_entry(PROC_SYS_IP6_RTR_SOLICITS, iface->name, 
			     iface->devconf[DEVCONF_RTR_SOLICITS]);
}

void md_cleanup(void)
{
	struct list_head *l, *n;

	pthread_mutex_lock(&iface_lock);
	iface_default_proc_entries_cleanup();
	list_for_each_safe(l, n, &ifaces) {
		struct md_inet6_iface *iface;
		iface = list_entry(l, struct md_inet6_iface, list);
		md_expire_inet6_iface(iface);
		iface_proc_entries_cleanup(iface);
		if (!iface->is_tunnel)
			ndisc_send_rs(iface->ifindex, &in6addr_all_routers_mc,
				      NULL, 0);
		md_free_inet6_iface(iface);
	}
	pthread_mutex_unlock(&iface_lock);
	return;
}
