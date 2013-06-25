/*
 * $Id: ha.c 1.126 06/05/07 21:52:42+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Ville Nuorvala <vnuorval@tcs.hut.fi>
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
#include <errno.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/ip6mh.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/if_tunnel.h>

#include "debug.h"
#include "icmp6.h"
#include "mpdisc_ha.h"
#include "mh.h"
#include "tqueue.h"
#include "bcache.h"
#include "util.h"
#include "cn.h"
#include "retrout.h"
#include "tunnelctl.h"
#include "rtnl.h"
#include "ha.h"
#include "dhaad_ha.h"
#include "conf.h"
#ifdef ENABLE_VT
#include "vt.h"
#endif
#include "ipsec.h"
#include "xfrm.h"
#include "ndisc.h"
#include "prefix.h"
#include "statistics.h"

#include "pmip_init.h"
#include "pmip_handler.h"

static pthread_mutex_t bu_worker_mutex;
static volatile unsigned long bu_worker_count = 0;
static pthread_cond_t cond;

LIST_HEAD(bu_worker_list);
LIST_HEAD(ha_interfaces);

static void ha_recv_ra(const struct icmp6_hdr *ih, ssize_t len,
		       const struct in6_addr *src,
		       const struct in6_addr *dst, int iif, int hoplimit)
{
	struct nd_router_advert *ra = (struct nd_router_advert *)ih;
	int optlen = len - sizeof(struct nd_router_advert);
	uint8_t *opt;
	struct nd_opt_prefix_info *pinfo[MAX_HOME_AGENTS];
	int i, num_pinfo = 0;
	struct ha_interface *iface;
	uint16_t pref = 0;
	uint16_t life = 0;
	uint16_t flags = 0;

	/* validity checks */
	if (hoplimit < 255 || !IN6_IS_ADDR_LINKLOCAL(src) ||
	    ih->icmp6_code != 0 || len < 0 ||
	    (size_t)len < sizeof(struct nd_router_advert) ||
	    !conf.pmgr.accept_ra(iif, src, dst, ra))
		return;

	if ((iface = ha_get_if(iif)) == NULL)
	    return;

	opt = (uint8_t *)(ra + 1);

	mpd_handle_mpa_flags(iface, ra->nd_ra_flags_reserved);

	if (ra->nd_ra_flags_reserved & ND_RA_FLAG_HOME_AGENT) {
		life = ntohs(ra->nd_ra_router_lifetime);
	}
	while (optlen > 1 && num_pinfo < MAX_HOME_AGENTS) {
		unsigned int olen = opt[1] << 3;

		if (olen > (size_t)optlen || olen == 0)
			return;

		if (opt[0] == ND_OPT_PREFIX_INFORMATION) {
			struct nd_opt_prefix_info *p;
			p = (struct nd_opt_prefix_info *)opt;

			if (olen < sizeof(*p) || p->nd_opt_pi_prefix_len > 128)
				return;
			p->nd_opt_pi_valid_time = 
				ntohl(p->nd_opt_pi_valid_time);
			p->nd_opt_pi_preferred_time =
				ntohl(p->nd_opt_pi_preferred_time);
			if (ra->nd_ra_flags_reserved & ND_RA_FLAG_HOME_AGENT)
				mpd_handle_pinfo(iface, p);
			pinfo[num_pinfo++] = p;
		} else if (opt[0] == ND_OPT_HOME_AGENT_INFO &&
			   ra->nd_ra_flags_reserved & ND_RA_FLAG_HOME_AGENT) {
			struct nd_opt_homeagent_info *hainfo;
			hainfo = (struct nd_opt_homeagent_info *)opt;

			if (olen < sizeof(*hainfo))
				return;

			pref = ntohs(hainfo->nd_opt_hai_preference);
			life = ntohs(hainfo->nd_opt_hai_lifetime);
			flags = hainfo->nd_opt_hai_flags_reserved;
		}
		optlen -= olen;
		opt += olen;
	}
	for (i = 0; i < num_pinfo; i++) {
		/* if the router is running as HA, add it, else delete it */
		if (pinfo[i]->nd_opt_pi_flags_reserved & 
		    ND_OPT_PI_FLAG_RADDR) {
			dhaad_insert_halist(iface, pref, life,
					    flags, pinfo[i]);
		}
	}
	mpd_del_expired_pinfos(iface);
}

struct icmp6_handler ha_ra_handler = {
	.recv = ha_recv_ra,
};

struct ha_interface *ha_get_if(int ifindex)
{
	struct list_head *lp;

	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *iface;

		iface = list_entry(lp, struct ha_interface, iflist);
		if (iface->ifindex == ifindex)
			return iface;
	}
	return NULL;
}

struct ha_interface *ha_get_if_by_addr(const struct in6_addr *addr)
{
	struct list_head *li;

	list_for_each(li, &ha_interfaces) {
		struct ha_interface *iface;
		struct list_head *la;

		iface = list_entry(li, struct ha_interface, iflist);

		list_for_each(la, &iface->addr_list) {
			struct ha_addr_holder *addrs;
			addrs = list_entry(la, struct ha_addr_holder, list);
			if (IN6_ARE_ADDR_EQUAL(&addrs->ha_addr, addr))
				return iface;
		}
	}
	return NULL;
}

struct ha_interface *ha_get_if_by_anycast(const struct in6_addr *anycast,
					  struct in6_addr **addrp)
{
	struct list_head *li;

	list_for_each(li, &ha_interfaces) {
		struct ha_interface *iface;
		struct list_head *la;

		iface = list_entry(li, struct ha_interface, iflist);

		list_for_each(la, &iface->addr_list) {
			struct ha_addr_holder *addrs;
			addrs = list_entry(la, struct ha_addr_holder, list);
			if (IN6_ARE_ADDR_EQUAL(&addrs->anycast_addr,
					       anycast)) {
				if (addrp)
					*addrp = &addrs->ha_addr;
				return iface;
			}
		}
	}
	return NULL;
}

static int ha_if_addr_setup(__attribute__ ((unused)) const struct sockaddr_nl *who,
			    struct nlmsghdr *n,
			    void *arg)
{
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	struct ha_interface *i = (struct ha_interface *)arg;
	struct ha_addr_holder *addr; 
	struct rtattr * rta_tb[IFA_MAX+1];

	if (n->nlmsg_len < NLMSG_LENGTH(sizeof(*ifa)))
		return -1;

	if (n->nlmsg_type != RTM_NEWADDR)
		return 0;
	if (i->ifindex < 0 || ifa->ifa_index != (uint32_t)i->ifindex)
		return 0;
	if (ifa->ifa_scope != RT_SCOPE_UNIVERSE)
		return 0;

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), 
		     n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));

	if (!rta_tb[IFA_ADDRESS])
		return 0;

	addr = malloc(sizeof(*addr));
	if (addr != NULL) {
		struct in6_addr *ha_addr = RTA_DATA(rta_tb[IFA_ADDRESS]);

		addr->ha_addr = *ha_addr;

		dhaad_gen_ha_anycast(&addr->anycast_addr,
				     ha_addr, ifa->ifa_prefixlen);
				     
		if (if_mc_group(ICMP6_MAIN_SOCK, i->ifindex,
				&addr->anycast_addr, IPV6_JOIN_ANYCAST) < 0)
			return -1;
		dbg("Joined anycast group "
		    "%x:%x:%x:%x:%x:%x:%x:%x on iface %d\n",
		    NIP6ADDR(&addr->anycast_addr), i->ifindex);
		list_add_tail(&addr->list, &i->addr_list);
	}
	return 0;
}

static int ha_addr_setup(void)
{
	struct list_head *lp;
	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *i;
		i = list_entry(lp, struct ha_interface, iflist);
		if (addrs_iterate(ha_if_addr_setup, i) < 0)
			return -1;
		if (if_mc_group(ICMP6_MAIN_SOCK, i->ifindex,
				&in6addr_all_nodes_mc, IPV6_JOIN_GROUP) < 0)
			return -1;
	}
	return 0;
}

static int ha_insert_if(int newifindex)
{
	struct ha_interface *newif;
	struct list_head *lp;

	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *i;
		i = list_entry(lp, struct ha_interface, iflist);
		if (i->ifindex == newifindex)
			return 0; /* No need to bother: iface already there. */
	}

	newif = malloc(sizeof(*newif));
	if (newif == NULL)
		return -ENOMEM;

	memset(newif, 0, sizeof(*newif));
	newif->ifindex = newifindex;
	INIT_LIST_HEAD(&newif->ha_list);
	INIT_LIST_HEAD(&newif->addr_list);
	INIT_LIST_HEAD(&newif->prefix_list);
	list_add_tail(&newif->iflist, &ha_interfaces);
	return 0;
}

int homeagent_if_init(int ifindex)
{
	if (ifindex == 0)
		return -1;
	ha_insert_if(ifindex);
	return 0;
}

int homeagents_ifall_init(void)
{
	struct if_nameindex *ifs, *i;

	if (!list_empty(&ha_interfaces)) return 0;

	ifs = if_nameindex();
	for (i = ifs; i->if_index != 0; i++) {
		if (i->if_index != 1) {
			ha_insert_if(i->if_index);
		}
	}
	if_freenameindex(ifs);

	return 0;
}

void ha_discover_routers(void)
{
	struct list_head *lp;
	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *i;
		i = list_entry(lp, struct ha_interface, iflist);
		ndisc_send_rs(i->ifindex, &in6addr_all_routers_mc, NULL, 0);
	}
}

static void ha_proxy_nd_init(void)
{
	struct list_head *lp;
	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *i;
		i = list_entry(lp, struct ha_interface, iflist);
		proxy_nd_iface_init(i->ifindex);
	}
}

static void ha_proxy_nd_cleanup(void)
{
	struct list_head *lp;
	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *i;
		i = list_entry(lp, struct ha_interface, iflist);
		proxy_nd_iface_cleanup(i->ifindex);
	}
}

#ifdef ENABLE_VT
struct ha_vt_arg {
	const struct vt_handle *vh;
};

static int ha_halist_vt_dump(int ifindex, void *data, void *arg)
{
	struct home_agent *h = (struct home_agent *)data;
	struct ha_vt_arg *hva = (struct ha_vt_arg *)arg;
	const struct vt_handle *vh = hva->vh;
	char buf[IF_NAMESIZE + 1];
	char *dev;

	if (!h)
		return 0;

	dev = if_indextoname(ifindex, buf);
	if (!dev || strlen(dev) == 0)
		fprintf(vh->vh_stream, "(%d)", h->iface->ifindex);
	else
		fprintf(vh->vh_stream, "%s", dev);

	fprintf(vh->vh_stream, " ");

	fprintf_bl(vh, "%x:%x:%x:%x:%x:%x:%x:%x", NIP6ADDR(&h->addr));

	fprintf(vh->vh_stream, "\n");

	fprintf(vh->vh_stream, " preference %d", h->preference);

	fprintf(vh->vh_stream, " lifetime %lu", h->lifetime.tv_sec);

	fprintf(vh->vh_stream, "\n");

	return 0;
}

/**
 * ha_halist_iterate - apply function to every home agent list entry
 * @func: function to apply
 * @arg: extra data for @func
 **/
static void ha_halist_iterate(int (* func)(int, void *, void *), void *arg)
{
	struct list_head *lp;

	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *iface;

		iface = list_entry(lp, struct ha_interface, iflist);

		dhaad_halist_iterate(iface, func, arg);
	}
}

static int ha_plist_vt_dump(int ifindex, void *data, void *arg)
{
	struct prefix_list_entry *ple = (struct prefix_list_entry *)data;
	struct ha_vt_arg *hva = (struct ha_vt_arg *)arg;
	const struct vt_handle *vh = hva->vh;
	char buf[IF_NAMESIZE + 1];
	char *dev;
	struct timespec ts_now;

	if (!ple)
		return 0;

	dev = if_indextoname(ifindex, buf);
	if (!dev || strlen(dev) == 0)
		fprintf(vh->vh_stream, "(%d)", ifindex);
	else
		fprintf(vh->vh_stream, "%s", dev);

	fprintf(vh->vh_stream, " ");

	fprintf_bl(vh, "%x:%x:%x:%x:%x:%x:%x:%x/%u",
		     NIP6ADDR(&ple->ple_prefix), ple->ple_plen);

	fprintf(vh->vh_stream, "\n");

	fprintf(vh->vh_stream, " valid ");
	if (clock_gettime(CLOCK_REALTIME, &ts_now) != 0)
		fprintf(vh->vh_stream, "(error)");
	else {
		if (tsafter(ts_now, ple->timestamp))
			fprintf(vh->vh_stream, "(broken)");
		else {
			struct timespec ts;
			uint32_t diff;

			tssub(ts_now, ple->timestamp, ts);
			diff = ts.tv_sec;
			if (ple->ple_valid_time < diff) {
				fprintf(vh->vh_stream, "-%u",
					diff - ple->ple_valid_time);
			} else {
				fprintf(vh->vh_stream, "%u",
					ple->ple_valid_time - diff);
			}
		}
	}
	fprintf(vh->vh_stream, " / %u", ple->ple_valid_time);

	fprintf(vh->vh_stream, " preferred %u", ple->ple_prefd_time);

	fprintf(vh->vh_stream, " flags %c%c%c",
		  ((ple->ple_flags & ND_OPT_PI_FLAG_ONLINK) ? 'O' : '-'),
		  ((ple->ple_flags & ND_OPT_PI_FLAG_AUTO) ? 'A' : '-'),
		  ((ple->ple_flags & ND_OPT_PI_FLAG_RADDR) ? 'R' : '-'));

	fprintf(vh->vh_stream, "\n");

	return 0;
}

/**
 * ha_plist_iterate - apply function to every prefix list entry
 * @func: function to apply
 * @arg: extra data for @func
 **/
static void ha_plist_iterate(int (* func)(int, void *, void *), void *arg)
{
	struct list_head *lp;

	list_for_each(lp, &ha_interfaces) {
		struct ha_interface *iface;

		iface = list_entry(lp, struct ha_interface, iflist);

		mpd_plist_iterate(iface, func, arg);
	}
}

static int ha_thread_vt_cmd(const struct vt_handle *vh, const char *str)
{
	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}
	pthread_mutex_lock(&bu_worker_mutex);
	fprintf(vh->vh_stream, "bu: %lu\n", bu_worker_count);
	pthread_mutex_unlock(&bu_worker_mutex);
	return 0;
}

static int ha_halist_vt_cmd(const struct vt_handle *vh, const char *str)
{
	struct ha_vt_arg hva;
	hva.vh = vh;
	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}
	ha_halist_iterate(ha_halist_vt_dump, &hva);
	return 0;
}

static int ha_plist_vt_cmd(const struct vt_handle *vh, const char *str)
{
	struct ha_vt_arg hva;
	hva.vh = vh;
	if (strlen(str) > 0) {
		fprintf(vh->vh_stream, "unknown args\n");
		return 0;
	}
	ha_plist_iterate(ha_plist_vt_dump, &hva);
	return 0;
}

static struct vt_cmd_entry vt_cmd_thread = {
	.cmd = "thread",
	.parser = ha_thread_vt_cmd,
};

static struct vt_cmd_entry vt_cmd_hal = {
	.cmd = "hal",
	.parser = ha_halist_vt_cmd,
};

static struct vt_cmd_entry vt_cmd_pl = {
	.cmd = "pl",
	.parser = ha_plist_vt_cmd,
};

static int ha_vt_init(void)
{
	int ret;
	ret = vt_cmd_add_root(&vt_cmd_thread);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add_root(&vt_cmd_hal);
	if (ret < 0)
		return ret;
	ret = vt_cmd_add_root(&vt_cmd_pl);
	return ret;
}
#endif


static void nemo_ha_del_mnp_routes(struct list_head *old_mnps,
				   struct list_head *new_mnps,
				   int ifindex, int all)
{
	struct list_head *list;
	list_for_each(list, old_mnps) {
		struct prefix_list_entry *p;
		p = list_entry(list, struct prefix_list_entry, list);
		if (!all &&
		    prefix_list_find(new_mnps, &p->ple_prefix, p->ple_plen))
			continue;

		route_del(ifindex, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_FWD,
			  NULL, 0, &p->ple_prefix, p->ple_plen, NULL);
	}
}

static int nemo_ha_add_mnp_routes(struct list_head *old_mnps,
				  struct list_head *new_mnps,
				  int ifindex, int all)
{
	struct list_head *list;
	list_for_each(list, new_mnps) {
		struct prefix_list_entry *p;
		p = list_entry(list, struct prefix_list_entry, list);
		if (!all &&
		    prefix_list_find(old_mnps, &p->ple_prefix, p->ple_plen))
			continue;
		if (route_add(ifindex, RT6_TABLE_MIP6, RTPROT_MIP,
			      0, IP6_RT_PRIO_MIP6_FWD,
			      NULL, 0, &p->ple_prefix, p->ple_plen, NULL) < 0)
			return -1;
	}
	return 0;
}

struct home_tnl_ops_parm {
	struct bcentry *bce;
	int ba_status;
	struct list_head mob_net_prefixes;
};

static int home_tnl_del(int old_if, int new_if, struct home_tnl_ops_parm *p)
{
	const struct in6_addr *our_addr, *peer_addr, *coa, *old_coa;
	struct list_head *mnp;

	assert(old_if);

	our_addr = &p->bce->our_addr;
	peer_addr = &p->bce->peer_addr;
	coa = &p->bce->peer_addr;
	old_coa = &p->bce->coa;
	mnp = &p->bce->mob_net_prefixes;

	if (conf.UseMnHaIPsec) {
		/* migrate */ 
		ha_ipsec_tnl_update(our_addr, peer_addr,
				    coa, old_coa, p->bce->tunnel, mnp);
		/* delete SP entry */ 
		ha_ipsec_tnl_pol_del(our_addr, peer_addr, p->bce->tunnel, mnp);
	}
	/* delete HoA route */
	route_del(old_if, RT6_TABLE_MAIN,
		  IP6_RT_PRIO_MIP6_FWD, NULL, 0, peer_addr, 128, NULL);

	/* delete MNP routes */
	nemo_ha_del_mnp_routes(&p->bce->mob_net_prefixes,
			       &p->mob_net_prefixes, old_if, 1);
	/* update tunnel interface */
	p->bce->tunnel = new_if;

	return 0;
}

static int home_tnl_add(int old_if, int new_if, struct home_tnl_ops_parm *p)
{
	const struct in6_addr *our_addr, *peer_addr, *coa, *old_coa;
	struct list_head *mnp;

	assert(new_if);

	our_addr = &p->bce->our_addr;
	peer_addr = &p->bce->peer_addr;
	coa = &p->bce->coa;
	old_coa = IN6_ARE_ADDR_EQUAL(&p->bce->old_coa, &in6addr_any) ?
		&p->bce->peer_addr : &p->bce->old_coa;
	mnp = &p->mob_net_prefixes;

	/* update tunnel interface */
	p->bce->tunnel = new_if;

	/* add MNP routes */
	if (nemo_ha_add_mnp_routes(&p->bce->mob_net_prefixes,
				   &p->mob_net_prefixes, new_if, 1) < 0) {
		if (p->bce->nemo_type == BCE_NEMO_EXPLICIT)
			p->ba_status = IP6_MH_BAS_INVAL_PRFX;
		else
			p->ba_status = IP6_MH_BAS_FWDING_FAILED;
		goto err;
	}
	/* add HoA route */
	if (route_add(new_if, RT6_TABLE_MAIN,
		      RTPROT_MIP, 0, IP6_RT_PRIO_MIP6_FWD,
		      NULL, 0, peer_addr, 128, NULL) < 0) {
		p->ba_status = IP6_MH_BAS_INSUFFICIENT;
		goto err;
	}
	/* add SP entry */	
	if (conf.UseMnHaIPsec) {
		if (ha_ipsec_tnl_pol_add(our_addr, peer_addr,
					 p->bce->tunnel, mnp) < 0) {
			p->ba_status = IP6_MH_BAS_INSUFFICIENT;
			goto err;
		}

		/* Always update transport mode (for signaling) */
		if (ha_ipsec_trns_update(our_addr, peer_addr, coa, old_coa,
					 p->bce->tunnel) < 0) {
			p->ba_status = IP6_MH_BAS_INSUFFICIENT;
			goto err;
		}

		/* migrate */ 
		if (ha_ipsec_tnl_update(our_addr, peer_addr, coa, old_coa,
					p->bce->tunnel, mnp) < 0) {
			p->ba_status = IP6_MH_BAS_INSUFFICIENT;
			goto err;
		}
	}
	return 0;
err:
	home_tnl_del(new_if, old_if, p);
	return -1;
}

static int home_tnl_chg(int old_if, int new_if, struct home_tnl_ops_parm *p)
{
	assert(old_if && new_if);

	if (old_if == new_if) {
		const struct in6_addr *our_addr, *peer_addr, *coa, *old_coa;
		struct list_head *mnp;

		our_addr = &p->bce->our_addr;
		peer_addr = &p->bce->peer_addr;
		coa = &p->bce->coa;
		old_coa = &p->bce->old_coa;
		mnp = &p->mob_net_prefixes;

		/* if interface hasn't changed, at least check if the
		   MR's MNPs have changed */
		if (!prefix_list_cmp(&p->bce->mob_net_prefixes,
				     &p->mob_net_prefixes)) {

			/* Remove old policies and install new ones */
			if (conf.UseMnHaIPsec) {
				ha_ipsec_mnp_pol_del(our_addr, peer_addr,
						     &p->bce->mob_net_prefixes,
						     &p->mob_net_prefixes,
						     p->bce->tunnel);
				ha_ipsec_mnp_pol_add(our_addr, peer_addr,
						     &p->bce->mob_net_prefixes,
						     &p->mob_net_prefixes,
						     p->bce->tunnel);
			}

			/* Do the same for routes */
			nemo_ha_del_mnp_routes(&p->bce->mob_net_prefixes,
					       &p->mob_net_prefixes,
					       old_if, 0);
			if (nemo_ha_add_mnp_routes(&p->bce->mob_net_prefixes,
						   &p->mob_net_prefixes,
						   new_if, 0) < 0) {
				if (p->bce->nemo_type == BCE_NEMO_EXPLICIT)
					p->ba_status = IP6_MH_BAS_INVAL_PRFX;
				else
					p->ba_status = IP6_MH_BAS_FWDING_FAILED;
				return -1;
			}
		}

		/* migrate */ 
		if (conf.UseMnHaIPsec &&
		    !IN6_ARE_ADDR_EQUAL(old_coa, coa)) {
			if (ha_ipsec_trns_update(our_addr, peer_addr,
						coa, old_coa,
						p->bce->tunnel) < 0)
				return -1;

			if (ha_ipsec_tnl_update(our_addr, peer_addr,
						coa, old_coa,
						p->bce->tunnel, mnp) < 0)
				return -1;
		}
	} else { 
		home_tnl_del(old_if, new_if, p);
		if (home_tnl_add(old_if, new_if, p) < 0)
			return -1;
	}
	return 0;
}

static int home_tnl_ops(int request, int old_if, int new_if, void *data)
{
	struct home_tnl_ops_parm *p = data;
	int res = -1;

	if (request == SIOCADDTUNNEL)
		res = home_tnl_add(old_if, new_if, p);
	else if (request == SIOCCHGTUNNEL)
		res = home_tnl_chg(old_if, new_if, p);
	else if (request == SIOCDELTUNNEL)
		res = home_tnl_del(old_if, new_if, p);
	return res;
}

static void home_cleanup(struct bcentry *bce)
{
	mpd_cancel_mpa(&bce->our_addr, &bce->peer_addr);

	if (bce->link > 0) {
		route_del(bce->link, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_OUT,
			  &bce->our_addr, 128, &bce->peer_addr, 128, NULL);
		proxy_nd_stop(bce->link, &bce->peer_addr, bce->flags);
	}
	if (bce->tunnel > 0) {
		struct home_tnl_ops_parm p = {
			.bce = bce,
			.ba_status = IP6_MH_BAS_ACCEPTED
		};
		tunnel_del(bce->tunnel, home_tnl_ops, &p);
	}
	if (conf.UseMnHaIPsec) {
		ha_mn_ipsec_pol_mod(&bce->our_addr, &bce->peer_addr);
	}
}


static int ha_extract_mnps(const struct ip6_mh_binding_update *bu,
			   const struct mh_options *opts,
			   struct list_head *mob_net_prefixes)
{
	struct ip6_mh_opt_mob_net_prefix *op;
	int prefix_count = 0;
	for (op = mh_opt(&bu->ip6mhbu_hdr, opts, IP6_MHOPT_MOB_NET_PRFX);
	     op != NULL;
	     op = mh_opt_next(&bu->ip6mhbu_hdr, opts, op)) {
		struct prefix_list_entry *p;
		p = malloc(sizeof(struct prefix_list_entry));
		if (p == NULL) {
			prefix_list_free(mob_net_prefixes);
			return -1;
		}
		memset(p, 0, sizeof(struct prefix_list_entry));
		p->ple_plen = op->ip6mnp_prefix_len;
		p->ple_prefix = op->ip6mnp_prefix;
		list_add_tail(&p->list, mob_net_prefixes);
		prefix_count++;
	}
	return prefix_count;
}

static int ha_get_mnps(const struct in6_addr *hoa,
		       struct list_head *mob_net_prefixes)
{
	struct nd_opt_prefix_info *mnps;
	int mnp_count = conf.pmgr.get_mnp_count(hoa);
	int i;

	if (mnp_count <= 0)
		return mnp_count;

	mnps = calloc(mnp_count, sizeof(struct nd_opt_prefix_info));
	if (mnps == NULL)
		return -1;

	mnp_count = conf.pmgr.get_mnps(hoa, mnp_count, mnps);
	for (i = 0; i < mnp_count; i++) {
		struct prefix_list_entry *p;
		p = malloc(sizeof(struct prefix_list_entry));
		if (p == NULL) {
			prefix_list_free(mob_net_prefixes);
			free(mnps);
			return -1;
		}
		p->pinfo = *(mnps + i);
		list_add_tail(&p->list, mob_net_prefixes);
	}
	free(mnps);
	return mnp_count;
}

struct ha_recv_bu_args {
	struct list_head list;
	struct in6_addr src;
	struct in6_addr dst;
	struct in6_addr remote_coa;
	struct in6_addr bind_coa;
	struct ip6_mh_binding_update *bu;
	ssize_t len;
	struct mh_options mh_opts;
	struct timespec lft;
	int iif;
	int flags;	/* HA_BU_F_XXX */
	int *statusp;	/* 0 or more than 0 is BA status, otherwise error */
};

static void *ha_recv_bu_worker(void *varg)
{
	struct ha_recv_bu_args *arg = varg;
	struct in6_addr_bundle out;
	struct bcentry *bce;
	struct timespec lft, tmp;
	int iif, status, new, home_ifindex;
	uint16_t bu_flags, seqno;
	uint8_t ba_flags;
	struct home_tnl_ops_parm p;

	pthread_dbg("thread started");
restart:	
	home_ifindex = 0;
	new = 0;
	ba_flags = 0;
	lft = arg->lft;
	iif = arg->iif;
	bu_flags = arg->bu->ip6mhbu_flags;
	seqno = ntohs(arg->bu->ip6mhbu_seqno);
	out.src = &arg->src;
	out.dst = &arg->dst;
	if (!IN6_IS_ADDR_UNSPECIFIED(&arg->remote_coa))
		out.remote_coa = &arg->remote_coa;
	else
		out.remote_coa = NULL;
	if (!IN6_IS_ADDR_UNSPECIFIED(&arg->bind_coa))
		out.bind_coa = &arg->bind_coa;
	else
		out.bind_coa = NULL;
	out.local_coa = NULL;

	bce = bcache_get(out.src, out.dst);
	if (bce) {
		if (bce->type != BCE_NONCE_BLOCK) {
			/* H-bit or R-bit mismatch, flags changed */
			if ((bce->flags ^ bu_flags) &
			    (IP6_MH_BU_HOME | IP6_MH_BU_MR)) {
				bcache_release_entry(bce);
				bce = NULL;
				status = IP6_MH_BAS_REG_NOT_ALLOWED;
				goto send_nack;
			}
			if (bce->type == BCE_DAD) {
				/* DAD is ongoing, so we already received
				 * a BU. There is no need to do anything
				 * for that reemission. */
				bcache_release_entry(bce);
				status = -EBUSY;
				goto out;
			}
			if (!MIP6_SEQ_GT(seqno, bce->seqno)) {
				if (arg->flags & HA_BU_F_PASSIVE_SEQ) {
					/* always use valid sequence */
					seqno = bce->seqno + 1;
				} else {
					/* sequence number expired */
					status = IP6_MH_BAS_SEQNO_BAD;
					seqno = bce->seqno;
					bcache_release_entry(bce);
					bce = NULL;
					goto send_nack;
				}
			}
		} else {
			bcache_release_entry(bce);
			bce = NULL;
			/* don't let MN deregister BCE_NONCE_BLOCK entry */
			if (!tsisset(lft)) {
				status = IP6_MH_BAS_UNSPECIFIED;
				goto send_nack;
			}
			/* else get rid of it */
			bcache_delete(out.src, out.dst);
		}
	} else if (!tsisset(lft)) {
		status = IP6_MH_BAS_NOT_HA;
		goto send_nack;
	}
	if ((status = mpd_prefix_check(out.src, out.dst,
				       &lft, &home_ifindex, new)) < 0) {
		if (!(bu_flags & IP6_MH_BU_MR) ||
		    home_ifindex == 0 ||
		    !prefix_list_find(&conf.nemo_ha_served_prefixes,
		     		      out.dst, 0)) {
			/* not home agent for this subnet */
			status = IP6_MH_BAS_NOT_HOME_SUBNET;
			goto send_nack;
		}
		status = IP6_MH_BAS_ACCEPTED;
	}
	status = conf.pmgr.discard_binding(out.dst, out.bind_coa,
					   out.src, arg->bu, arg->len);
	if (status >= IP6_MH_BAS_UNSPECIFIED)
		goto send_nack;
	/* lifetime may be further decreased by local policy */
	if (conf.pmgr.max_binding_life(out.dst, out.bind_coa, out.src,
				       arg->bu, arg->len, &lft, &tmp)) {
		if (tsbefore(lft, tmp))
			lft = tmp;
	}
	mpd_sanitize_lft(&lft);
	if (!bce) {
		bce = bcache_alloc(BCE_HOMEREG);
		if (!bce) {
			status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}
		bce->our_addr = *out.src;
		bce->peer_addr = *out.dst;
		bce->coa = *out.bind_coa;
		bce->seqno = seqno;
		bce->flags = bu_flags;
		bce->type = BCE_DAD;
		bce->cleanup = NULL;
		bce->link = home_ifindex;

		if (bcache_add_homereg(bce) < 0) {
			free(bce);
			bce = NULL;
			status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}
		if (!(arg->flags & HA_BU_F_SKIP_DAD)) {
			/* Do DAD for home address */
			if (ndisc_do_dad(home_ifindex, out.dst,
					 bu_flags & IP6_MH_BU_LLOCAL) < 0) {
				bcache_delete(out.src, out.dst);
				bce = NULL;
				status =  IP6_MH_BAS_DAD_FAILED;
				goto send_nack;
			}
		}
		bce = bcache_get(out.src, out.dst);
		if (!bce) {
			BUG("BCE deleted before DAD completed!");
			status =  IP6_MH_BAS_UNSPECIFIED;
			goto send_nack;
		}
		new = 1;
	}
	INIT_LIST_HEAD(&p.mob_net_prefixes);
	if (bu_flags & IP6_MH_BU_MR && tsisset(lft)) {
		if (mh_opt(&arg->bu->ip6mhbu_hdr,
			   &arg->mh_opts, IP6_MHOPT_MOB_NET_PRFX) != NULL) {
			if (ha_extract_mnps(arg->bu,
					    &arg->mh_opts,
					    &p.mob_net_prefixes) < 0) {
				status = IP6_MH_BAS_INVAL_PRFX;
				goto send_nack;
			}
			bce->nemo_type = BCE_NEMO_EXPLICIT;
		} else if (ha_get_mnps(out.dst, &p.mob_net_prefixes) > 0) {
			bce->nemo_type = BCE_NEMO_IMPLICIT;
		} else {
			/* Todo: dynamic routing */
			status = IP6_MH_BAS_FWDING_FAILED;
			goto send_nack;
		}
	}
	p.bce = bce;
	p.ba_status = status;
	bce->seqno = seqno;
	bce->flags = bu_flags;
	bce->lifetime = lft;
	if (new) {
		if (tunnel_add(out.src, out.bind_coa, 0, 
			       home_tnl_ops, &p) < 0) {
			if (p.ba_status >= IP6_MH_BAS_UNSPECIFIED)
				status = p.ba_status;
			else
				status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}
		/* Now save the MNP list in the BCE */
		list_splice(&p.mob_net_prefixes, &bce->mob_net_prefixes);

		bce->cleanup = home_cleanup;

		if (route_add(bce->link, RT6_TABLE_MIP6,
			      RTPROT_MIP, 0, IP6_RT_PRIO_MIP6_OUT,
			      &bce->our_addr, 128, &bce->peer_addr, 128, 
			      NULL) < 0) {
			status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}

		if (proxy_nd_start(bce->link, out.dst, out.src,
				   bu_flags) < 0) {
			status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}
		bce->type = BCE_HOMEREG;
		bcache_complete_homereg(bce);
	} else {
		bce->old_coa = bce->coa;
		bce->coa = *out.bind_coa;
		if (tunnel_mod(bce->tunnel, out.src, out.bind_coa, 0,
			       home_tnl_ops, &p) < 0) { 
			if (p.ba_status >= IP6_MH_BAS_UNSPECIFIED)
				status = p.ba_status;
			else
				status = IP6_MH_BAS_INSUFFICIENT;
			goto send_nack;
		}
		/* Now update the MNP list in the BCE */
		prefix_list_free(&bce->mob_net_prefixes);
		list_splice(&p.mob_net_prefixes, &bce->mob_net_prefixes);

		bcache_update_expire(bce);
	}
	/* bce is always valid here */
	bcache_release_entry(bce);
	if (!tsisset(lft))
		bcache_delete(out.src, out.dst);

	if (conf.pmgr.use_keymgm(out.dst, out.src))
	{
		if (bu_flags & IP6_MH_BU_KEYM) {
			ba_flags |= IP6_MH_BA_KEYM;
		} else {
			/* The remote MN does not support session mvt,
			   so we remove from the bce. */
			bce->flags &= ~IP6_MH_BU_KEYM;
			cdbg(
			 "MN (%x:%x:%x:%x:%x:%x:%x:%x) does not support IKE session movement.\n",
			 NIP6ADDR(out.src));
		}
	} else {
		if (bu_flags & IP6_MH_BU_KEYM) {
			/* Local policy does not allow using IKE movement */
			bce->flags &= ~IP6_MH_BU_KEYM;
			cdbg(
			 "MN (%x:%x:%x:%x:%x:%x:%x:%x) would support IKE session movement,"
			 " but local policy denies it.\n",
			 NIP6ADDR(out.src));
		}
	}

	if (ba_flags & IP6_MH_BA_KEYM) {
		/* FUTURE */
		/* Move the peer endpoint of the key management
		 * protocol connection, if any, to the new care-of
		 * address. For an IKE phase 1 connection, this means
		 * that any IKE packets sent to the peer are sent to
		 * this address, and packets from this address with
		 * the original ISAKMP cookies are accepted. */
	} else {
		/* FUTURE */
		/* Discard key management connections, if any, to the
		 * old care-of address. If the mobile node did not
		 * have a binding before sending this Binding Update,
		 * discard the connections to the home address. */
	}
 	if (status < IP6_MH_BAS_UNSPECIFIED && bu_flags & IP6_MH_BU_MR)
 		ba_flags |= IP6_MH_BA_MR;

	if (!(arg->flags & HA_BU_F_SKIP_BA))
		mh_send_ba(&out, status, ba_flags, seqno, &lft, NULL, iif);
	if (new && tsisset(lft))
		mpd_start_mpa(&bce->our_addr, &bce->peer_addr);
out:
	pthread_mutex_lock(&bu_worker_mutex);
	if (!list_empty(&bu_worker_list)) {
		struct list_head *l = bu_worker_list.next;
		list_del(l);
		free(arg);
		arg = list_entry(l, struct ha_recv_bu_args, list);
		pthread_mutex_unlock(&bu_worker_mutex);
		goto restart;
	}
	if (--bu_worker_count == 0)
		pthread_cond_signal(&cond);
	if (arg->flags & HA_BU_F_THREAD_JOIN)
		*(arg->statusp) = status;
	free(arg);
	pthread_mutex_unlock(&bu_worker_mutex);
	pthread_exit(NULL);
send_nack:
	if (bce) {
		bcache_release_entry(bce);
		bcache_delete(out.src, out.dst);
	}
	if (!(arg->flags & HA_BU_F_SKIP_BA))
		mh_send_ba_err(&out, status, 0, seqno, NULL, iif);
	goto out;
}

/* After parsing, perform HA-specific checks on *home* bu */
static inline int ha_home_bu_check(struct ip6_mh_binding_update *bu,
			      struct mh_options *mh_opts)
{
	if (!(bu->ip6mhbu_flags & IP6_MH_BU_HOME) ||
	    mh_opt(&bu->ip6mhbu_hdr, mh_opts, IP6_MHOPT_NONCEID) != NULL ||
	    mh_opt(&bu->ip6mhbu_hdr, mh_opts, IP6_MHOPT_BAUTH) != NULL)
		return -1;

	return IP6_MH_BAS_ACCEPTED;
}

/* Handle *Home* BU on HA (dispatch for received BU is done in BU handler) */
int ha_recv_home_bu(const struct ip6_mh *mh, ssize_t len,
		    const struct in6_addr_bundle *in, int iif,
		    uint32_t flags)
{
	struct mh_options mh_opts;
	struct in6_addr_bundle out;
	struct ha_recv_bu_args *arg;
	struct ip6_mh_binding_update *bu;
	struct timespec lft;
	int status;
	pthread_t worker;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_BU);

	bu = (struct ip6_mh_binding_update *)mh;

	if (mh_bu_parse(bu, len, in, &out, &mh_opts, &lft) < 0)
		return -EINVAL;

	if ((status = ha_home_bu_check(bu, &mh_opts)) < 0)
		return -EINVAL;

	arg = malloc(sizeof(struct ha_recv_bu_args) + len);
	if (!arg) {
		if (bce_exists(out.src, out.dst))
			bcache_delete(out.src, out.dst);

		if (!(arg->flags & HA_BU_F_SKIP_BA))
			mh_send_ba_err(&out, IP6_MH_BAS_INSUFFICIENT, 0,
				       ntohs(bu->ip6mhbu_seqno), NULL, iif);
		return -ENOMEM;
	}
	arg->src = *out.src;
	arg->dst = *out.dst;
	if (out.remote_coa)
		arg->remote_coa = *out.remote_coa;
	else 
		arg->remote_coa = in6addr_any;
	if (out.bind_coa)
		arg->bind_coa = *out.bind_coa;
	else
		arg->bind_coa = in6addr_any;
	arg->bu = (struct ip6_mh_binding_update *)(arg + 1);
	arg->len = len;
	arg->mh_opts = mh_opts;
	arg->lft = lft;
	arg->iif = iif;
	memcpy(arg->bu, bu, len);
	arg->flags = flags;
	arg->statusp = &status;

	pthread_mutex_lock(&bu_worker_mutex);
	bu_worker_count++;
	if (pthread_create(&worker, NULL, ha_recv_bu_worker, arg)) {
		free(arg);
		if (--bu_worker_count == 0)
			pthread_cond_signal(&cond);
	} else {
		if (!(arg->flags & HA_BU_F_THREAD_JOIN))
			pthread_detach(worker);
	}
	pthread_mutex_unlock(&bu_worker_mutex);

	if (arg->flags & HA_BU_F_THREAD_JOIN) {
		pthread_join(worker, NULL);
		return status;
	}

	return 0;
}

static void ha_recv_bu(const struct ip6_mh *mh, ssize_t len,
		       const struct in6_addr_bundle *in, int iif)
{
	struct ip6_mh_binding_update *bu = (struct ip6_mh_binding_update *)mh;

	if (bu->ip6mhbu_flags & IP6_MH_BU_PR)
		pmip_lma_recv_pbu(mh, len, in, iif);
	else if (bu->ip6mhbu_flags & IP6_MH_BU_HOME)
		(void)ha_recv_home_bu(mh, len, in, iif, 0);
	else
		(void)cn_recv_bu(mh, len, in, iif);
}

static struct mh_handler ha_bu_handler = {
	.recv = ha_recv_bu,
};

int ha_init(void)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&bu_worker_mutex, &mattrs) ||
	    pthread_cond_init(&cond, NULL))
		return -1;
#ifdef ENABLE_VT
	if (ha_vt_init() < 0)
		return -1;
#endif
	if (homeagents_ifall_init() < 0)
		return -1;
	if (ha_addr_setup() < 0)
		return -1;
	if (dhaad_ha_init() < 0)
		return -1;
	if (mpd_ha_init() < 0)
		return -1;
	if (rule_add(NULL, RT6_TABLE_MIP6,
		     IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST,
		     &in6addr_any, 0, &in6addr_any, 0, 0) < 0)
		return -1;
	icmp6_handler_reg(ND_ROUTER_ADVERT, &ha_ra_handler);
	ha_discover_routers(); /* Let's gather RA */
	ha_proxy_nd_init();
	mh_handler_reg(IP6_MH_TYPE_BU, &ha_bu_handler);
	pmip_lma_init();
	return 0;
}

void ha_cleanup(void)
{
	pmip_cleanup();
	mh_handler_dereg(IP6_MH_TYPE_BU, &ha_bu_handler);
	ha_proxy_nd_cleanup();
	icmp6_handler_dereg(ND_ROUTER_ADVERT, &ha_ra_handler);
	pthread_mutex_lock(&bu_worker_mutex);
	if (bu_worker_count)
		pthread_cond_wait(&cond, &bu_worker_mutex);
	pthread_mutex_unlock(&bu_worker_mutex);
	bcache_flush();
	rule_del(NULL, RT6_TABLE_MIP6,
		 IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST,
		 &in6addr_any, 0, &in6addr_any, 0, 0);
	mpd_ha_cleanup();
	dhaad_ha_cleanup();
}
