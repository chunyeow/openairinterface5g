/*
 * $Id: ipsec.c 1.52 06/05/15 18:34:56+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 *
 * Authors:
 *  Shinta Sugimoto	<shinta.sugimoto@ericsson.com>
 *
 * Copyright 2004-2005 USAGI/WIDE Project
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

/*
 * Special thanks to Francis Dupont who initially had the idea of making
 * PF_KEY extension as an interface between Mobile IPv6 and IPsec/IKE
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/param.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <netinet/ip6.h>
#include <netinet/in.h>
#include <netinet/ip6mh.h>

#include "ipsec.h"
#include "xfrm.h"
#include "conf.h"
#include "bul.h"
#include "util.h"
#include "mn.h"
#include "rtnl.h"
#include "debug.h"

static void _set_tmpl(struct xfrm_user_tmpl *tmpl,
		      uint16_t family,
		      uint8_t proto,
		      uint8_t mode,
		      const struct in6_addr *tdst,
		      const struct in6_addr *tsrc,
		      uint32_t reqid)
{
	memset(tmpl, 0, sizeof(*tmpl));
	tmpl->family = family;
        tmpl->ealgos = ~(uint32_t)0;
        tmpl->aalgos = ~(uint32_t)0;
	tmpl->id.proto = proto;
	tmpl->optional = 0;
	tmpl->mode = mode;
	tmpl->reqid = reqid;
	if (mode == XFRM_MODE_TUNNEL && tdst)
                memcpy(&tmpl->id.daddr, tdst, sizeof(tmpl->id.daddr));
	if (mode == XFRM_MODE_TUNNEL && tsrc)
                memcpy(&tmpl->saddr, tsrc, sizeof(tmpl->saddr));
}

static void _set_sp(struct xfrm_userpolicy_info *sp,
		    struct ipsec_policy_entry *e,
		    int dir,
		    const struct in6_addr *in6_dst,
		    int  dst_len,
		    const struct in6_addr *in6_src,
		    int src_len,
		    __attribute__ ((unused)) int ifindex,
		    int nodetype)
{
	assert(sp);
	assert(e);
	assert(in6_dst);
	assert(in6_src);

	memset(sp, 0, sizeof(*sp));

	sp->sel.family = AF_INET6;
	sp->dir = dir;
	sp->action = e->action;
	memcpy(&sp->sel.saddr.a6, in6_src, sizeof(sp->sel.saddr.a6));
	memcpy(&sp->sel.daddr.a6, in6_dst, sizeof(sp->sel.daddr.a6));
	sp->sel.prefixlen_s = src_len;
	if (!src_len && (!IN6_ARE_ADDR_EQUAL(in6_src, &in6addr_any)))
		sp->sel.prefixlen_s = 128;
	sp->sel.prefixlen_d = dst_len;
	if (!dst_len && (!IN6_ARE_ADDR_EQUAL(in6_dst, &in6addr_any)))
		sp->sel.prefixlen_d = 128;

	sp->sel.ifindex = 0;

	switch (e->type) {
		/* Tunnel */
	case IPSEC_POLICY_TYPE_TUNNELHOMETESTING:
		if (dir == XFRM_POLICY_IN || dir == XFRM_POLICY_FWD) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(IP6_MH_TYPE_HOT);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(IP6_MH_TYPE_HOTI);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else if (dir == XFRM_POLICY_OUT) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(IP6_MH_TYPE_HOTI);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(IP6_MH_TYPE_HOT);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else {
			sp->sel.sport = 0;
		}
		sp->sel.proto = IPPROTO_MH;
		sp->priority = MIP6_PRIO_RO_SIG_RR;
		break;
	case IPSEC_POLICY_TYPE_TUNNELMH:
		sp->sel.proto = IPPROTO_MH;
		sp->priority = MIP6_PRIO_RO_SIG_RR;
		break;
	case IPSEC_POLICY_TYPE_TUNNELPAYLOAD:
		sp->priority = MIP6_PRIO_RO_SIG_RR;
		break;

		/* Transport */
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		if (dir == XFRM_POLICY_IN) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(IP6_MH_TYPE_BACK);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(IP6_MH_TYPE_BU);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else if (dir == XFRM_POLICY_OUT) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(IP6_MH_TYPE_BU);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(IP6_MH_TYPE_BACK);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else {
			sp->sel.sport = 0;
		}
	case IPSEC_POLICY_TYPE_MH:
		sp->sel.proto = IPPROTO_MH;
		sp->priority = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_BERROR:
		sp->sel.proto = IPPROTO_MH;
		sp->priority = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		if (dir == XFRM_POLICY_IN) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(MIP_PREFIX_ADVERT);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(MIP_PREFIX_SOLICIT);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else if (dir == XFRM_POLICY_OUT) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(MIP_PREFIX_SOLICIT);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(MIP_PREFIX_ADVERT);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else {
			sp->sel.sport = 0;
		}
		sp->sel.proto = IPPROTO_ICMPV6;
		sp->priority = MIP6_PRIO_HOME_DATA_IPSEC;
		break;
	case IPSEC_POLICY_TYPE_NDISC: /* XXX Makes sense ? --arno */
		if (dir == XFRM_POLICY_IN) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(ND_NEIGHBOR_SOLICIT);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(ND_NEIGHBOR_ADVERT);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else if (dir == XFRM_POLICY_OUT) {
			if (nodetype == MIP6_ENTITY_MN) {
				sp->sel.sport = htons(ND_NEIGHBOR_ADVERT);
				sp->sel.sport_mask = ~((__u16)0);
			} else if (nodetype == MIP6_ENTITY_HA) {
				sp->sel.sport = htons(MIP_PREFIX_SOLICIT);
				sp->sel.sport_mask = ~((__u16)0);
			} else
				sp->sel.sport = 0;
		} else {
			sp->sel.sport = 0;
		}
		sp->sel.proto = IPPROTO_ICMPV6;
		sp->priority = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_ICMP:
		sp->sel.proto = IPPROTO_ICMPV6;
		sp->priority = MIP6_PRIO_HOME_DATA_IPSEC;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		sp->priority = MIP6_PRIO_HOME_DATA_IPSEC;
		break;
	default:
		/* not tunnel IPsec type */
		break;
	}

	return;
}

#ifdef XFRM_MSG_MIGRATE
/*
 * xfrm_sendmigrate -- send MIGRATE message to the kernel
 *
 * @sp: security policy
 * @tmpl: template which includes key information {proto, mode, reqid,
 *        old_dst, old_src} for MIGRATE message
 * @ndst: new destination address
 * @nsrc: new source address
 *
 * return value:
 *	success:  0
 *	failure: -1
 */
static int xfrm_sendmigrate(struct xfrm_userpolicy_info *sp,
			    const struct xfrm_user_tmpl *tmpl,
			    const struct in6_addr *ndst,
			    const struct in6_addr *nsrc)
{
	struct {
		struct nlmsghdr			n;
		struct xfrm_userpolicy_id	xpid;
		char				buf[256];
	} req;
	struct xfrm_user_migrate um;
	struct xfrm_user_kmaddress kma;
	int err = 0;

	memset(&req, 0, sizeof(req));
	memset(&um, 0, sizeof(um));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(req.xpid));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = XFRM_MSG_MIGRATE;

	/* policy information */
	memset(&req.xpid, 0, sizeof(req.xpid));
	memcpy(&req.xpid.sel, &sp->sel, sizeof(req.xpid.sel));
	req.xpid.index = 0;
	req.xpid.dir = sp->dir;

	/* migrate */
	um.old_family = AF_INET6;
	um.new_family = AF_INET6;
	um.proto = tmpl->id.proto;
	um.mode = tmpl->mode;
	um.reqid = tmpl->reqid;

	memcpy(&um.old_daddr, &tmpl->id.daddr, sizeof(um.old_daddr));
	memcpy(&um.old_saddr, &tmpl->saddr, sizeof(um.old_saddr));
	memcpy(&um.new_daddr, ndst, sizeof(um.new_daddr));
	memcpy(&um.new_saddr, nsrc, sizeof(um.new_saddr));

	addattr_l(&req.n, sizeof(req), XFRMA_MIGRATE,
		  (void *)&um, sizeof(struct xfrm_user_migrate));

	/* kmaddress */
	kma.family = AF_INET6;
	kma.reserved = 0;
	if (sp->dir == XFRM_POLICY_OUT) {
		memcpy(&kma.local,  nsrc, sizeof(kma.local));
		memcpy(&kma.remote, ndst, sizeof(kma.remote));
	} else {
		memcpy(&kma.local,  ndst, sizeof(kma.local));
		memcpy(&kma.remote, nsrc, sizeof(kma.remote));
	}


	addattr_l(&req.n, sizeof(req), XFRMA_KMADDRESS,
		  (void *)&kma, sizeof(struct xfrm_user_kmaddress));

#if 0
	dbg("sel.family = %d\n", xpid.sel.family);
	dbg("sel.saddr = %x:%x:%x:%x:%x:%x:%x:%x\n",
	    NIP6ADDR((struct in6_addr *)&xpid.sel.saddr));
	dbg("sel.prefixlen_s = %d\n", xpid.sel.prefixlen_s);
	dbg("sel.sport = %d\n", xpid.sel.sport);
	dbg("sel.sport_mask = %d\n", xpid.sel.sport_mask);
	dbg("sel.daddr = %x:%x:%x:%x:%x:%x:%x:%x\n",
	    NIP6ADDR((struct in6_addr *)&xpid.sel.daddr));
	dbg("sel.prefixlen_d = %d\n", xpid.sel.prefixlen_d);
	dbg("sel.dport = %d\n", xpid.sel.dport);
	dbg("sel.dport_mask = %d\n", xpid.sel.dport_mask);
	dbg("sel.proto = %d\n", xpid.sel.proto);
	dbg("sel.ifindex = %d\n", xpid.sel.ifindex);
	dbg("sel.user = %d\n", xpid.sel.user);
#endif

	err = rtnl_xfrm_do(&req.n, NULL);
	if (err < 0)
		dbg("err = %d (%s)\n", err, strerror(-err));

	return ((err == 0 || err == -ENOENT) ? 0 : -1);
}
#else
static int xfrm_sendmigrate(struct xfrm_userpolicy_info *sp,
			    const struct xfrm_user_tmpl *tmpl,
			    const struct in6_addr *ndst,
			    const struct in6_addr *nsrc)
{
	dbg("Error because it is built without XFRM_MSG_MIGRATE\n");
	return -1;
}
#endif

int ipsec_policy_apply(const struct in6_addr *haaddr,
		       const struct in6_addr *hoa,
		       int (* func)(const struct in6_addr *haaddr,
				    const struct in6_addr *hoa,
				    struct ipsec_policy_entry *e, void *arg),
		       void *arg)
{
	struct list_head *lp;
	int ret = 0;

	list_for_each(lp, &conf.ipsec_policies) {
		struct ipsec_policy_entry *e;
		
		e = list_entry(lp, struct ipsec_policy_entry, list);

		if (haaddr && !IN6_ARE_ADDR_EQUAL(haaddr, &e->ha_addr))
			continue;

		if (hoa && !IN6_ARE_ADDR_EQUAL(hoa, &e->mn_addr))
			continue;

		ret = func(&e->ha_addr, &e->mn_addr, e, arg);
		if (ret)
			break;
	}
	return ret;
}

int ipsec_policy_dump_config(const struct in6_addr *haaddr,
			     const struct in6_addr *hoa,
			     struct ipsec_policy_entry *e,
			     __attribute__ ((unused)) void *arg)
{
	dbg("IPsec: HA address = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(haaddr));
	dbg("IPsec: Home address = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(hoa));

	dbg("IPsec: IPsec type = %s\n",
	    (e->type == IPSEC_POLICY_TYPE_HOMEREGBINDING) ? "HomeRegBinding" :
	    (e->type == IPSEC_POLICY_TYPE_MH) ? "Mh" :
	    (e->type == IPSEC_POLICY_TYPE_MOBPFXDISC) ? "MobPfxDisc" :
	    (e->type == IPSEC_POLICY_TYPE_ICMP) ? "ICMP" :
	    (e->type == IPSEC_POLICY_TYPE_ANY) ? "any" :
	    (e->type == IPSEC_POLICY_TYPE_TUNNELHOMETESTING) ? "TunnelHomeTesting" :
	    (e->type == IPSEC_POLICY_TYPE_TUNNELMH) ? "TunnelMh" :
	    (e->type == IPSEC_POLICY_TYPE_TUNNELPAYLOAD) ? "TunnelPayload" : "?");
	dbg("IPsec: IPsec templates = %s%s%s\n",
	    ipsec_use_esp(e) ? "ESP " : "",
	    ipsec_use_ah(e) ? "AH " : "",
	    ipsec_use_ipcomp(e) ? "IPComp " : "");
	dbg("IPsec: IPsec reqid to-HA, to-MN = %u, %u\n", e->reqid_toha, e->reqid_tomn);
	dbg("IPsec: IPsec action = %s\n",
	    e->action == XFRM_POLICY_ALLOW ? "allow" : "block");
	dbg("IPsec: ---\n");

	return 0;
}

static void dump_migrate(int ifindex,
			 u_int8_t ipsec_proto,
			 const struct in6_addr *hoa,
			 const struct in6_addr *haaddr,
			 const struct in6_addr *oldcoa,
			 const struct in6_addr *newcoa)
{
	dbg("ifindex\t%d\n", ifindex);
	dbg("hoa\t%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(hoa));
	dbg("ha\t%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(haaddr));
	if (oldcoa)
		dbg("ocoa\t%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(oldcoa));
	if (newcoa)
		dbg("ncoa\t%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(newcoa));
	dbg("ipsec\t%s\n",
	    (ipsec_proto == IPPROTO_ESP) ? "ESP" :
	    (ipsec_proto == IPPROTO_AH) ? "AH" :
	    (ipsec_proto == IPPROTO_COMP) ? "IPComp" : "?");

	return;
}

int ipsec_policy_walk(int (* func)(const struct in6_addr *haaddr,
				   const struct in6_addr *hoa,
				   struct ipsec_policy_entry *e, void *arg),
		      void *arg)
{
	return ipsec_policy_apply(NULL, NULL, func, arg);
}

int ipsec_policy_entry_check(const struct in6_addr *haaddr,
			     const struct in6_addr *hoa,
			     int type)
{
	struct list_head *lp;
	int ret = 0;

	list_for_each(lp, &conf.ipsec_policies) {
		struct ipsec_policy_entry *e;

		e = list_entry(lp, struct ipsec_policy_entry, list);

		if ((haaddr && !IN6_ARE_ADDR_EQUAL(haaddr, &e->ha_addr)) ||
		    (hoa && !IN6_ARE_ADDR_EQUAL(hoa, &e->mn_addr)))
			continue;

		if (e->type & type) {
			ret = e->ipsec_protos;
			break;
		}
	}
	return ret;
}

struct ha_ipsec_tnl_update {
	int tunnel;
	struct in6_addr coa;
	struct in6_addr old_coa;
	struct list_head *mnp;
};

/*
 *   Tunnel Update (for HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_tnl_update(const struct in6_addr *haaddr,
			  const struct in6_addr *hoa,
			  struct ipsec_policy_entry *e,
			  void *arg)
{
	int err = 0;
	struct ha_ipsec_tnl_update *info = (struct ha_ipsec_tnl_update *)arg;
	int ifindex;
	const struct in6_addr *oldcoa, *newcoa;
	const struct in6_addr *peer_addr = hoa;
	struct list_head *mnp;
	u_int8_t ipsec_proto;
	struct xfrm_user_tmpl tmpl;
	struct xfrm_userpolicy_info sp;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	switch (e->type) {
	case IPSEC_POLICY_TYPE_TUNNELHOMETESTING:
	case IPSEC_POLICY_TYPE_TUNNELMH:
	case IPSEC_POLICY_TYPE_TUNNELPAYLOAD:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}
	
	ifindex = info->tunnel;
	oldcoa = IN6_ARE_ADDR_EQUAL(&info->old_coa, &in6addr_any) ?
		peer_addr : &info->old_coa;
	newcoa = &info->coa;
	mnp = info->mnp;

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, oldcoa, newcoa);

	/* inbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, oldcoa, e->reqid_toha);
	_set_sp(&sp, e, XFRM_POLICY_IN, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_HA);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
		dbg("migrate for INBOUND policy failed\n");
        	goto end;
	}

	/* forward */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, oldcoa, e->reqid_toha);
	_set_sp(&sp, e, XFRM_POLICY_FWD, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_HA);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
		dbg("migrate for FORWARD policy failed\n");
		goto end;
	}

	/* outbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
		  oldcoa, haaddr, e->reqid_tomn);
	_set_sp(&sp, e, XFRM_POLICY_OUT, hoa, 0, &in6addr_any, 0,
		ifindex, MIP6_ENTITY_HA);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
		dbg("migrate for OUTBOUND policy failed\n");
        	goto end;
	}

	/* Mobile router case */
	if ( (e->type == IPSEC_POLICY_TYPE_TUNNELPAYLOAD) && mnp)
	{
		struct list_head *list;

		/* We have to modify rules to protect traffic to and from MNP's, the same way as HoA */
		list_for_each(list, mnp)
		{
			struct prefix_list_entry *p;
			p = list_entry(list, struct prefix_list_entry, list);

			/* inbound */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  haaddr, oldcoa, e->reqid_toha);
			_set_sp(&sp, e, XFRM_POLICY_IN, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
				ifindex, MIP6_ENTITY_HA);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
				dbg("migrate for INBOUND policy failed\n");
        			goto end;
			}

			/* forward */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  haaddr, oldcoa, e->reqid_toha);
			_set_sp(&sp, e, XFRM_POLICY_FWD, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
				ifindex, MIP6_ENTITY_HA);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
				dbg("migrate for FORWARD policy failed\n");
				goto end;
			}

			/* outbound */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  oldcoa, haaddr, e->reqid_tomn);
			_set_sp(&sp, e, XFRM_POLICY_OUT, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
				ifindex, MIP6_ENTITY_HA);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
				dbg("migrate for OUTBOUND policy failed\n");
        			goto end;
			}
		}
	}

 end:
	return err;
}

int ha_ipsec_tnl_update(const struct in6_addr *haaddr,
			const struct in6_addr *hoa,
			const struct in6_addr *coa,
			const struct in6_addr *old_coa,
			int tunnel,
			struct list_head *mnp)
{
	struct ha_ipsec_tnl_update b;
	b.coa = *coa;
	b.old_coa = *old_coa;
	b.tunnel = tunnel;
	b.mnp = mnp;
	return ipsec_policy_apply(haaddr, hoa, _ha_tnl_update, &b);
}

struct ha_ipsec_mnp_update {
	int tunnel;
	struct list_head *old_mnps;
	struct list_head *new_mnps;
};

/*
 *   Add/Delete MNP IPsec Security Policy
 */
static int _ha_mnp_pol_mod(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg,
			   int add)
{
	int err = 0;
	struct ha_ipsec_mnp_update *parms = (struct ha_ipsec_mnp_update *)arg;
	struct xfrm_userpolicy_info sp;
	struct xfrm_user_tmpl tmpl;
	u_int16_t ipsec_proto;
	struct list_head *list, *old_mnps, *new_mnps, *main_mnps, *ref_mnps;
	int ifindex;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	ifindex = parms->tunnel;
	old_mnps = parms->old_mnps;
	new_mnps = parms->new_mnps;

	if (e->type != IPSEC_POLICY_TYPE_TUNNELPAYLOAD)
		goto end;

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}

	/* Reverse the search logic on lists based on expected
	 * action (add/del) */
	main_mnps = add ? new_mnps : old_mnps;
	ref_mnps = add ? old_mnps : new_mnps;

	if (main_mnps == NULL)
		goto end;

	/* We have to add/delete rules to protect traffic to
	   and from MNP's, the same way as HoA */
	list_for_each(list, main_mnps) {
		struct prefix_list_entry *p;
		p = list_entry(list, struct prefix_list_entry, list);

		if (ref_mnps &&
		    prefix_list_find(ref_mnps, &p->ple_prefix, p->ple_plen))
			continue;

		/* inbound */
		_set_sp(&sp, e, XFRM_POLICY_IN, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
			ifindex, MIP6_ENTITY_HA);
		_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
			  haaddr, hoa, e->reqid_toha);
		if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
			dbg("modifying INBOUND policy failed\n");
			err = -1;
			goto end;
		}

		/* forward */
		_set_sp(&sp, e, XFRM_POLICY_FWD, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
			ifindex, MIP6_ENTITY_HA);
		_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
			  haaddr, hoa, e->reqid_toha);
		if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
			dbg("modifying FORWARD policy failed\n");
			err = -1;
			goto end;
		}

		/* outbound */
		_set_sp(&sp, e, XFRM_POLICY_OUT, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
			ifindex, MIP6_ENTITY_HA);
		_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
			  hoa, haaddr, e->reqid_tomn);
		if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
			dbg("modifying OUTBOUND policy failed\n");
			err = -1;
			goto end;
		}
	}

 end:
	return err;
}


/*
 *   Add SP entry (for MNP on HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_mnp_pol_add(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _ha_mnp_pol_mod(haaddr, hoa, e, arg, 1);
}

int ha_ipsec_mnp_pol_add(const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr,
			 struct list_head *old_mnps,
			 struct list_head *new_mnps,
			 int tunnel)
{
	struct ha_ipsec_mnp_update b;
	b.tunnel = tunnel;
	b.old_mnps = old_mnps;
	b.new_mnps = new_mnps;

	return ipsec_policy_apply(our_addr, peer_addr, _ha_mnp_pol_add, &b);
}

/*
 *   Delete SP entry (for MNP on HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_mnp_pol_del(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _ha_mnp_pol_mod(haaddr, hoa, e, arg, 0);
}

int ha_ipsec_mnp_pol_del(const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr,
			 struct list_head *old_mnps,
			 struct list_head *new_mnps,
			 int tunnel)
{
	struct ha_ipsec_mnp_update b;
	b.tunnel = tunnel;
	b.old_mnps = old_mnps;
	b.new_mnps = new_mnps;

	return ipsec_policy_apply(our_addr, peer_addr,
				  _ha_mnp_pol_del, &b);
}


struct ha_ipsec_trns_update {
	int tunnel;
	struct in6_addr coa;
	struct in6_addr old_coa;
};

/*
 *   Transport Update (for HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_trns_update(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	int err = 0;
	struct ha_ipsec_trns_update *info = (struct ha_ipsec_trns_update *)arg;
	int ifindex;
	const struct in6_addr *oldcoa, *newcoa;
	const struct in6_addr *peer_addr = hoa;
	u_int8_t ipsec_proto;
	struct xfrm_user_tmpl tmpl;
	struct xfrm_userpolicy_info sp;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	switch (e->type) {
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
	case IPSEC_POLICY_TYPE_BERROR:
	case IPSEC_POLICY_TYPE_MH:
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
	case IPSEC_POLICY_TYPE_NDISC:
	case IPSEC_POLICY_TYPE_ICMP:
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}

	ifindex = info->tunnel;
	oldcoa = IN6_ARE_ADDR_EQUAL(&info->old_coa, &in6addr_any) ?
		peer_addr : &info->old_coa;
	newcoa = &info->coa;

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, oldcoa, newcoa);

	/* inbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TRANSPORT,
		  haaddr, oldcoa, e->reqid_toha);
	_set_sp(&sp, e, XFRM_POLICY_IN, haaddr, 128,
		hoa, 128, ifindex, MIP6_ENTITY_HA);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
		dbg("migrate for INBOUND policy failed\n");
        	goto end;
	}

	/* outbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TRANSPORT,
		  oldcoa, haaddr, e->reqid_tomn);
	_set_sp(&sp, e, XFRM_POLICY_OUT, hoa, 128,
		haaddr, 128, ifindex, MIP6_ENTITY_HA);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
		dbg("migrate for OUTBOUND policy failed\n");
        	goto end;
	}

 end:
	return err;
}

int ha_ipsec_trns_update(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 const struct in6_addr *coa,
			 const struct in6_addr *old_coa,
			 int tunnel)
{
	struct ha_ipsec_trns_update b;
	b.tunnel = tunnel;
	b.coa = *coa;
	b.old_coa = *old_coa;
	return ipsec_policy_apply(haaddr, hoa, _ha_trns_update, &b);
}




/*
 *   Add/Delete IPsec Security Policy 
 */
static int _ha_tnl_pol_mod(const struct in6_addr *haaddr, 
			   const struct in6_addr *hoa, 
			   struct ipsec_policy_entry *e, 
			   void *arg, 
			   int add)
{
	int err = 0;
	struct ha_ipsec_tnl_update *parms = (struct ha_ipsec_tnl_update *)arg;
	int ifindex;
	struct list_head *mnp;
	struct xfrm_userpolicy_info sp;
	struct xfrm_user_tmpl tmpl;
	u_int16_t ipsec_proto;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	ifindex = parms->tunnel;
	mnp = parms->mnp;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_TUNNELHOMETESTING:
	case IPSEC_POLICY_TYPE_TUNNELMH:
	case IPSEC_POLICY_TYPE_TUNNELPAYLOAD:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, NULL, NULL);

	/* inbound */
	_set_sp(&sp, e, XFRM_POLICY_IN, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_HA);
	_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, hoa, e->reqid_toha);
	if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
		dbg("modifying INBOUND policy failed\n");
		err = -1;
		goto end;
	}

	/* forward */
	_set_sp(&sp, e, XFRM_POLICY_FWD, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_HA);
	_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, hoa, e->reqid_toha);
	if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
		dbg("modifying FORWARD policy failed\n");
		err = -1;
		goto end;
	}

	/* outbound */
	_set_sp(&sp, e, XFRM_POLICY_OUT, hoa, 0, &in6addr_any, 0,
		ifindex, MIP6_ENTITY_HA);
	_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
		  hoa, haaddr, e->reqid_tomn);
	if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
		dbg("modifying OUTBOUND policy failed\n");
		err = -1;
		goto end;
	}

	/* Mobile Router case */
	if ((e->type == IPSEC_POLICY_TYPE_TUNNELPAYLOAD) && mnp) {
		struct ha_ipsec_mnp_update b;

		b.tunnel = ifindex;
		b.old_mnps = add ? NULL : mnp;
		b.new_mnps = add ? mnp : NULL;

		err = _ha_mnp_pol_mod(haaddr, hoa, e, (void *)&b, add);
	}
 end:
	return err;
}

/*
 *   Add SP entry (for HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_tnl_pol_add(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _ha_tnl_pol_mod(haaddr, hoa, e, arg, 1);
}

int ha_ipsec_tnl_pol_add(const struct in6_addr *our_addr, 
			 const struct in6_addr *peer_addr,
			 int tunnel,
			 struct list_head *mnp)
{
	struct ha_ipsec_tnl_update b;
	b.tunnel = tunnel;
	b.mnp = mnp;

	return ipsec_policy_apply(our_addr, peer_addr, _ha_tnl_pol_add, &b);
}

/*
 *   Delete SP entry (for HA)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _ha_tnl_pol_del(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _ha_tnl_pol_mod(haaddr, hoa, e, arg, 0);
}

int ha_ipsec_tnl_pol_del(const struct in6_addr *our_addr, 
			 const struct in6_addr *peer_addr,
			 int tunnel,
			 struct list_head *mnp)
{
	struct ha_ipsec_tnl_update b;
	b.tunnel = tunnel;
	b.mnp = mnp;

	return ipsec_policy_apply(our_addr, peer_addr,
				  _ha_tnl_pol_del, &b);
}

/*
 *   Tunnel Update (for MN)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _mn_tnl_update(const struct in6_addr *haaddr,
			  const struct in6_addr *hoa,
			  struct ipsec_policy_entry *e,
			  void *arg)
{
	int err = 0;
	struct bulentry *bule;
	int ifindex;
	struct in6_addr *oldcoa, *newcoa;
	u_int8_t ipsec_proto;
	struct xfrm_user_tmpl tmpl;
	struct xfrm_userpolicy_info sp;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	switch (e->type) {
	case IPSEC_POLICY_TYPE_TUNNELHOMETESTING:
	case IPSEC_POLICY_TYPE_TUNNELMH:
	case IPSEC_POLICY_TYPE_TUNNELPAYLOAD:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}

	bule = (struct bulentry *)arg;
	ifindex = bule->home->if_tunnel;
	oldcoa = &bule->last_coa;
	newcoa = &bule->coa;

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, oldcoa, newcoa);

	/* outbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, oldcoa, e->reqid_toha);
	_set_sp(&sp, e, XFRM_POLICY_OUT, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_MN);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
		dbg("migrate for OUTBOUND policy failed\n");
		goto end;
	}

	/* inbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
		  oldcoa, haaddr, e->reqid_tomn);
	_set_sp(&sp, e, XFRM_POLICY_IN, hoa, 0, &in6addr_any, 0,
		ifindex, MIP6_ENTITY_MN);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
		dbg("migrate for INBOUND policy (1) failed\n");
		goto end;
	}

	/*
	 * Additionally, we need to update endpoint address stored in the
	 * policy entry for processing BU from peer MN. Note that incoming 
	 * BU is normally IPsec-tunneled by the HA.
	 */
	if (e->type == IPSEC_POLICY_TYPE_TUNNELMH) {
		/* template */
		_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
			  oldcoa, haaddr, e->reqid_tomn);
		_set_sp(&sp, e, XFRM_POLICY_IN, hoa, 0, &in6addr_any, 0,
			ifindex, MIP6_ENTITY_MN);
		/* additional settings */
		sp.priority = MIP6_PRIO_RO_SIG_IPSEC;
		sp.sel.sport = htons(IP6_MH_TYPE_BU);
		sp.sel.sport_mask = ~((__u16)0);
		if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
			dbg("migrate for INBOUND policy (2) failed\n");
			goto end;
		}
	}

	/*
	 * If we are a Mobile Router, we also need to migrate IN/FWD/OUT rules
	 * for forwarded traffic in case we have TUNNELPAYLOAD protection.
	 */
	if ((e->type == IPSEC_POLICY_TYPE_TUNNELPAYLOAD) && (bule->home->mob_rtr))
	{
		struct list_head *mnp;

		list_for_each(mnp, &bule->home->mob_net_prefixes)
		{
			struct prefix_list_entry *p;
			p = list_entry(mnp, struct prefix_list_entry, list);

			/* outbound */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  haaddr, oldcoa, e->reqid_toha);
			_set_sp(&sp, e, XFRM_POLICY_OUT, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
				ifindex, MIP6_ENTITY_MN);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
				dbg("migrate for OUTBOUND policy failed\n");
				goto end;
			}

			/* forwarded */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  oldcoa, haaddr, e->reqid_tomn);
			_set_sp(&sp, e, XFRM_POLICY_IN, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
				ifindex, MIP6_ENTITY_MN);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
				dbg("migrate for INBOUND policy (1) failed\n");
				goto end;
			}

			/* inbound */
			_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TUNNEL,
				  oldcoa, haaddr, e->reqid_tomn);
			_set_sp(&sp, e, XFRM_POLICY_IN, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
				ifindex, MIP6_ENTITY_MN);
			if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
				dbg("migrate for INBOUND policy (1) failed\n");
				goto end;
			}

		}
	}

 end:
	return err;
}

/*
 *   Transport mode Update (for MN)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 *   - It might be beneficial to merge it with tunnel version --arno
 */
static int _mn_trns_update(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	int err = 0;
	struct bulentry *bule;
	int ifindex;
	struct in6_addr *oldcoa, *newcoa;
	u_int8_t ipsec_proto;
	struct xfrm_user_tmpl tmpl;
	struct xfrm_userpolicy_info sp;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	switch (e->type) {
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
	case IPSEC_POLICY_TYPE_BERROR:
	case IPSEC_POLICY_TYPE_MH:
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
	case IPSEC_POLICY_TYPE_NDISC:
	case IPSEC_POLICY_TYPE_ICMP:
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}

	bule = (struct bulentry *)arg;
	ifindex = bule->home->if_tunnel;
	oldcoa = &bule->last_coa;
	newcoa = &bule->coa;

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, oldcoa, newcoa);

	/* outbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TRANSPORT,
		  haaddr, oldcoa, e->reqid_toha);
	_set_sp(&sp, e, XFRM_POLICY_OUT, haaddr, 128,
		hoa, 128, ifindex, MIP6_ENTITY_MN);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, haaddr, newcoa)) < 0) {
		dbg("migrate for OUTBOUND policy failed\n");
		goto end;
	}

	/* inbound */
	_set_tmpl(&tmpl, 0, ipsec_proto, XFRM_MODE_TRANSPORT,
		  oldcoa, haaddr, e->reqid_tomn);
	_set_sp(&sp, e, XFRM_POLICY_IN, hoa, 128,
		haaddr, 128, ifindex, MIP6_ENTITY_MN);
	if ((err = xfrm_sendmigrate(&sp, &tmpl, newcoa, haaddr)) < 0) {
		dbg("migrate for INBOUND policy (1) failed\n");
		goto end;
	}

	/* XXX We should probably add something for MN-MN and MN-CN transport
	 * XXX mode entries. --arno */
 end:
	return err;
}

int mn_ipsec_trns_update(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 void *arg)
{
	return ipsec_policy_apply(haaddr, hoa, _mn_trns_update, arg);
}

int mn_ipsec_tnl_update(const struct in6_addr *haaddr,
			const struct in6_addr *hoa,
			void *arg)
{
	return ipsec_policy_apply(haaddr, hoa, _mn_tnl_update, arg);
}

static int _mn_tnl_pol_mod(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg,
			   int add)
{
	int err = 0;
	struct bulentry *bule = (struct bulentry *)arg;
	int ifindex;
	struct xfrm_userpolicy_info sp;
	struct xfrm_user_tmpl tmpl;
	u_int16_t ipsec_proto;

	assert(haaddr);
	assert(hoa);
	assert(e);
	assert(arg);

	switch (e->type) {
	case IPSEC_POLICY_TYPE_TUNNELHOMETESTING:
	case IPSEC_POLICY_TYPE_TUNNELMH:
	case IPSEC_POLICY_TYPE_TUNNELPAYLOAD:
		break;
	default:
		goto end;
	}

	/* XXX Limitation: Single IPsec proto can only be applied */
	if (ipsec_use_esp(e))
		ipsec_proto = IPPROTO_ESP;
	else if (ipsec_use_ah(e))
		ipsec_proto = IPPROTO_AH;
	else if (ipsec_use_ipcomp(e))
		ipsec_proto = IPPROTO_COMP;
	else {
		dbg("invalid ipsec proto\n");
		goto end;
	}
	
	ifindex = bule->home->if_tunnel;

	dump_migrate(ifindex, ipsec_proto, hoa, haaddr, NULL, NULL);

	/* inbound */
	_set_sp(&sp, e, XFRM_POLICY_IN, hoa, 0, &in6addr_any, 0,
		ifindex, MIP6_ENTITY_MN);
	_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
		  hoa, haaddr, e->reqid_tomn);
	if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
		dbg("modifying INBOUND policy failed.\n");
		err = -1;
		goto end;
	}
		
	/* outbound */
	_set_sp(&sp, e, XFRM_POLICY_OUT, &in6addr_any, 0, hoa, 0,
		ifindex, MIP6_ENTITY_MN);
	_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
		  haaddr, hoa, e->reqid_toha);
	if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
		dbg("modifying OUTBOUND policy failed.\n");
		err = -1;
		goto end;
	}
	
	/*
	 * Additionally, we need to create SPD entry to process incoming BU
	 * from peer MN.  Note that BU is normally IPsec-tunneled by the HA.
	 */
        if (e->type == IPSEC_POLICY_TYPE_TUNNELMH) {
		if (add) {
			/* flush wildrecv SPD entry for processing BU */
			cn_wildrecv_bu_pol_del();
			err = mn_ipsec_recv_bu_tnl_pol_add(bule, ifindex, e);
		} else {
			mn_ipsec_recv_bu_tnl_pol_del(bule, ifindex);
			/* restore wildrecv SPD entry for processing BU */
			err = cn_wildrecv_bu_pol_add();
		}
	}

	/*
	 * If we are a Mobile Router, we also need to create IN/FWD/OUT rules
	 * for forwarded traffic in case we have TUNNELPAYLOAD protection.
	 */
	if ((e->type == IPSEC_POLICY_TYPE_TUNNELPAYLOAD) && (bule->home->mob_rtr))
	{
		struct list_head *mnp;

		list_for_each(mnp, &bule->home->mob_net_prefixes)
		{
			struct prefix_list_entry *p;
			p = list_entry(mnp, struct prefix_list_entry, list);

			/* inbound */
			_set_sp(&sp, e, XFRM_POLICY_IN, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
				ifindex, MIP6_ENTITY_MN);
			_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
				  hoa, haaddr, e->reqid_tomn);
			if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
				dbg("modifying INBOUND policy failed.\n");
				err = -1;
				goto end;
			}

			/* forward */
			_set_sp(&sp, e, XFRM_POLICY_FWD, &p->ple_prefix, p->ple_plen, &in6addr_any, 0,
				ifindex, MIP6_ENTITY_MN);
			_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
				  hoa, haaddr, e->reqid_tomn);
			if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
				dbg("modifying INBOUND policy failed.\n");
				err = -1;
				goto end;
			}

			/* outbound */
			_set_sp(&sp, e, XFRM_POLICY_OUT, &in6addr_any, 0, &p->ple_prefix, p->ple_plen,
				ifindex, MIP6_ENTITY_MN);
			_set_tmpl(&tmpl, AF_INET6, ipsec_proto, XFRM_MODE_TUNNEL,
				  haaddr, hoa, e->reqid_toha);
			if (xfrm_ipsec_policy_mod(&sp, &tmpl, 1, add) < 0) {
				dbg("modifying OUTBOUND policy failed.\n");
				err = -1;
				goto end;
			}
		}
	}

 end:
	return err;
}

/*
 *   Add SP entry (for MN)
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _mn_tnl_pol_add(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _mn_tnl_pol_mod(haaddr, hoa, e, arg, 1);
}

int mn_ipsec_tnl_pol_add(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa, void *arg)
{
	return ipsec_policy_apply(haaddr, hoa, _mn_tnl_pol_add, arg);
}

/*
 *   Delete SP entry
 *
 *   NOTE:
 *   - This is a hook routine to ipsec_policy_apply()
 */
static int _mn_tnl_pol_del(const struct in6_addr *haaddr,
			   const struct in6_addr *hoa,
			   struct ipsec_policy_entry *e,
			   void *arg)
{
	return _mn_tnl_pol_mod(haaddr, hoa, e, arg, 0);
}

int mn_ipsec_tnl_pol_del(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa, void *arg)
{
	return ipsec_policy_apply(haaddr, hoa, _mn_tnl_pol_del, arg);
}
