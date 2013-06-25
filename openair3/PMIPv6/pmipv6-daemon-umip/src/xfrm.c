/*
 * $Id: xfrm.c 1.250 06/05/15 18:34:56+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors:
 *  USAGI Team
 *  Ville Nuorvala <vnuorval@tcs.hut.fi>
 *  Henrik Petander <petander@tcs.hut.fi>
 *
 * Copyright 2003 USAGI/WIDE Project
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

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <netinet/ip6mh.h>

#include <linux/xfrm.h>

#include "debug.h"
#include "mn.h" /* For mn_lock */
#include "xfrm.h"
#include "util.h"
#include "retrout.h"
#include "bul.h"
#include "bcache.h"
#include "mh.h"
#include "rtnl.h"
#include "conf.h"
#include "ipsec.h"

#define XFRM_DEBUG_LEVEL 1

#if XFRM_DEBUG_LEVEL >= 1
#define XDBG dbg
#else
#define XDBG(...)
#endif

#if XFRM_DEBUG_LEVEL >= 2
#define XDBG2 dbg
#else
#define XDBG2(...)
#endif

#define MIPV6_MAX_TMPLS 3 /* AH, ESP, IPCOMP */

static pthread_t xfrm_listener;

static void xfrm_sel_dump(const struct xfrm_selector *sel)
{
	cdbg("sel.daddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "sel.saddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "sel.dport %x\n"
	     "sel.dport_mask %x\n"
	     "sel.sport %x\n"
	     "sel.sport_mask %x\n"
	     "sel.prefixlen_d %d\n"
	     "sel.prefixlen_s %d\n"
	     "sel.proto %d\n"
	     "sel.ifindex %d\n",
	     NIP6ADDR((struct in6_addr *)&sel->daddr),
	     NIP6ADDR((struct in6_addr *)&sel->saddr),
	     ntohs(sel->dport),
	     ntohs(sel->dport_mask),
	     ntohs(sel->sport),
	     ntohs(sel->sport_mask),
	     sel->prefixlen_d,
	     sel->prefixlen_s,
	     sel->proto,
	     sel->ifindex);
}
			 
static void nlmsg_dump(int nlmsg_flags, int nlmsg_type)
{
	cdbg("nlmsg_flags %x\n"
	     "nlmsg_type %d\n",
	     nlmsg_flags,
	     nlmsg_type);
}

static void xfrm_tmpl_dump(const struct xfrm_user_tmpl *tmpl)
{
	cdbg("xfrma_tmpl.id.daddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "xfrma_tmpl.id.spi %x\n"
	     "xfrma_tmpl.id.proto %d\n"
	     "xfrma_tmpl.saddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "xfrma_tmpl.reqid %d\n"
	     "xfrma_tmpl.mode %d\n"
	     "xfmra_tmpl.optional %d\n"
	     "xfrma_tmpl.aalgos %x\n"
	     "xfrma_tmpl.ealgos %d\n"
	     "xfrma_tmpl.calgos %d\n",
	     NIP6ADDR((struct in6_addr *)&tmpl->id.daddr),
	     tmpl->id.spi,
	     tmpl->id.proto,
	     NIP6ADDR((struct in6_addr *)&tmpl->saddr),
	     tmpl->reqid,
	     tmpl->mode,
	     tmpl->aalgos,
	     tmpl->ealgos,
	     tmpl->calgos);
}

static void xfrm_policy_dump(const char *msg, int nlmsg_flags, int nlmsg_type,
			     const struct xfrm_userpolicy_info *sp,
			     const struct xfrm_userpolicy_type *ptype,
			     struct xfrm_user_tmpl *tmpls, int num_tmpl)
{
	int i;
	char ptype_buf[64];
	sprintf(ptype_buf, "%u", ptype->type);

	cdbg(msg);
	nlmsg_dump(nlmsg_flags, nlmsg_type);
	xfrm_sel_dump(&sp->sel);
	cdbg("priority %d\n"
	     "dir %d\n"
	     "action %d\n"
	     "type %s\n",
	    sp->priority,
	    sp->dir,
	    sp->action,
	    ptype_buf);
	for (i = 0; i < num_tmpl; i++)
		xfrm_tmpl_dump(&tmpls[i]);
}

static void xfrm_policy_id_dump(const char *msg, 
				const struct xfrm_userpolicy_id *sp_id,
				const struct xfrm_userpolicy_type *ptype)
{
	char ptype_buf[64];
	sprintf(ptype_buf, "%u", ptype->type);
	cdbg(msg);
	xfrm_sel_dump(&sp_id->sel);
	cdbg("dir %d\n"
	     "type %s\n",
	     sp_id->dir,
	     ptype_buf);
}

static void xfrm_state_dump(const char *msg, int nlmsg_flags, int nlmsg_type,
			    const struct xfrm_usersa_info *sa,
			    const struct in6_addr *coa)
{
	cdbg(msg);
	nlmsg_dump(nlmsg_flags, nlmsg_type);
	xfrm_sel_dump(&sa->sel);
	cdbg("id.daddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "id.spi %x\n"
	     "id.proto %d\n"
	     "saddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "reqid %d\n"
	     "mode %d\n"
	     "flags %x\n"
	     "xfrma_addr %x:%x:%x:%x:%x:%x:%x:%x\n",
	     NIP6ADDR((struct in6_addr *)&sa->id.daddr),
	     sa->id.spi,
	     sa->id.proto,
	     NIP6ADDR((struct in6_addr *)&sa->saddr),
	     sa->reqid,
	     sa->mode,
	     sa->flags,
	     NIP6ADDR(coa));
}

static void xfrm_state_id_dump(const char *msg,
			       const struct xfrm_usersa_id *sa_id,
			       const xfrm_address_t *saddr)
{
	cdbg(msg);
	cdbg("daddr %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "spi %x\n"
	     "proto %d\n"
	     "saddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	     NIP6ADDR((struct in6_addr *)&sa_id->daddr),
	     sa_id->spi,
	     sa_id->proto,
	     NIP6ADDR((struct in6_addr *)saddr));
}

/* Set xfrm_selector fields for MIPv6 and IPsec policies and MIPv6
 * states */
static void set_selector(const struct in6_addr *daddr, 
			 const struct in6_addr *saddr,
			 int proto, int type, int code,
			 __attribute__ ((unused)) int ifindex,
			 struct xfrm_selector *sel)
{
	memset(sel, 0, sizeof(*sel));

	sel->family = AF_INET6;
	sel->user = getuid();
	sel->ifindex = 0;
	sel->proto = proto;
	switch (proto) {
	case 0: /* Any */
		break;
	case IPPROTO_ICMPV6:
		sel->sport = htons(type);
		if (type)
			sel->sport_mask = ~((__u16)0);
		sel->dport = htons(code);
		if (code)
			sel->dport_mask = ~((__u16)0);
		break;
	case IPPROTO_MH:
		sel->sport = htons(type);
		if (type)
			sel->sport_mask = ~((__u16)0);
		sel->dport = code;
		if (code)
			sel->dport_mask = code;
		break;
	default:
		sel->sport = htons(type);
		if (type)
			sel->sport_mask = ~((__u16)0);
		sel->dport = code;
		if (code)
			sel->dport_mask = ~((__u16)0);

	}
	memcpy(&sel->saddr.a6, saddr, sizeof(*saddr));
	memcpy(&sel->daddr.a6, daddr, sizeof(*daddr));

	if (!IN6_ARE_ADDR_EQUAL(daddr, &in6addr_any))
		sel->prefixlen_d = 128;
	if (!IN6_ARE_ADDR_EQUAL(saddr, &in6addr_any))
		sel->prefixlen_s = 128;
}

/* NEMO specific version of set_selector(): set xfrm_selector
 * fields for IPsec policies and states. If NULL is passed as
 * src or dst prefix, any is used, i.e. ::/0 */
static void mr_set_selector(const struct prefix_list_entry *src,
			    const struct prefix_list_entry *dst,
			    uid_t uid, struct xfrm_selector *sel)
{
	memset(sel, 0, sizeof(*sel));
	sel->family = AF_INET6;
	sel->user = uid;

	if (src != NULL) {
		memcpy(&sel->saddr.a6, &src->ple_prefix,
		       sizeof(sel->saddr.a6));
		sel->prefixlen_s = src->ple_plen;
	}

	if (dst != NULL) {
		memcpy(&sel->daddr.a6, &dst->ple_prefix,
		       sizeof(sel->daddr.a6));
		sel->prefixlen_d = dst->ple_plen;
	}
}

/** 
 * xfrm_last_used - when was a binding  last used
 * @daddr: destination address (home address)
 * @saddr: source address (home address)
 * @proto: protocol. Either IPPROTO_ROUTING or IPPROTO_DSTOPTS 
 **/
long xfrm_last_used(const struct in6_addr *daddr, 
		    const struct in6_addr *saddr, int proto,
		    const struct timespec *now)
{
	uint8_t sbuf[NLMSG_SPACE(sizeof(struct xfrm_usersa_id)) +
		     RTA_SPACE(sizeof(xfrm_address_t))];
	uint8_t rbuf[1024];
	struct nlmsghdr *sn, *rn;
	struct xfrm_usersa_id *sa_id;
	struct xfrm_usersa_info *sa;
	int err;
	uint64_t lastused;
	long time_used;

	memset(sbuf, 0, sizeof(sbuf));
	sn = (struct nlmsghdr *)sbuf;
	sn->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_usersa_id));
	sn->nlmsg_flags = NLM_F_REQUEST;
	sn->nlmsg_type = XFRM_MSG_GETSA;

	sa_id = NLMSG_DATA(sn);
	memcpy(&sa_id->daddr.a6, daddr, sizeof(sa_id->daddr.a6));
	sa_id->family = AF_INET6;
	sa_id->proto = proto;

	addattr_l(sn, sizeof(sbuf), XFRMA_SRCADDR, saddr,
		  sizeof(xfrm_address_t));
	
	memset(rbuf, 0, sizeof(rbuf));
	rn = (struct nlmsghdr *)rbuf;

	err = rtnl_xfrm_do(sn, rn);
	if (err < 0)
		return -1;
	sa = NLMSG_DATA(rn);

	{
#define XFRMS_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_usersa_info))))
		struct rtattr *rta_tb[XFRMA_MAX+1];
		memset(rta_tb, 0, sizeof(rta_tb));
		parse_rtattr(rta_tb, XFRMA_MAX, XFRMS_RTA(sa), 
			     rn->nlmsg_len - NLMSG_LENGTH(sizeof(*sa)));

		if (!rta_tb[XFRMA_LASTUSED])
			return -1;

		lastused = *(uint64_t *)RTA_DATA(rta_tb[XFRMA_LASTUSED]);
	}
	if (!lastused) {
		XDBG("binding was unused\n");
		return -1;
	}
	time_used = now->tv_sec - (long)lastused;
	XDBG("last use of binding was %ld seconds ago\n", time_used);
	return time_used; 
}

long mn_bule_xfrm_last_used(const struct in6_addr *peer, 
			    const struct in6_addr *hoa, 
			    const struct timespec *now)
{
	long dst_used, rh_used, min_used, max_used;
	dst_used = xfrm_last_used(peer, hoa, IPPROTO_DSTOPTS, now);
	rh_used = xfrm_last_used(hoa, peer, IPPROTO_ROUTING, now);
	min_used = min(dst_used, rh_used);
	max_used = max(dst_used, rh_used);
	if (min_used < 0)
		return max_used;
	return min_used;
}

static inline void xfrm_lft(struct xfrm_lifetime_cfg *lft)
{
	lft->soft_byte_limit = XFRM_INF;
	lft->soft_packet_limit = XFRM_INF;
	lft->hard_byte_limit = XFRM_INF;
	lft->hard_packet_limit = XFRM_INF;
}

static int xfrm_policy_add(uint8_t type, const struct xfrm_selector *sel,
			   int update, int dir, int action, int priority,
			   struct xfrm_user_tmpl *tmpls, int num_tmpl)
{
	uint8_t buf[NLMSG_SPACE(sizeof(struct xfrm_userpolicy_info)) +
		    RTA_SPACE(sizeof(struct xfrm_userpolicy_type)) +
		    RTA_SPACE(sizeof(struct xfrm_user_tmpl) * num_tmpl)];
	struct nlmsghdr *n;
	struct xfrm_userpolicy_info *pol;
	struct xfrm_userpolicy_type ptype;

	int err;
	memset(buf, 0, sizeof(buf));
	n = (struct nlmsghdr *)buf;
	n->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_userpolicy_info));
	if (update) {
		n->nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE;
		n->nlmsg_type = XFRM_MSG_UPDPOLICY;
	} else {
		n->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE;
		n->nlmsg_type = XFRM_MSG_NEWPOLICY;
	}
	pol = NLMSG_DATA(n);
	memcpy(&pol->sel, sel, sizeof(struct xfrm_selector));
	xfrm_lft(&pol->lft);
	pol->priority = priority;
	pol->dir = dir;
	pol->action = action;
	pol->share = XFRM_SHARE_ANY;

	memset(&ptype, 0, sizeof(ptype));
	ptype.type = type;
	addattr_l(n, sizeof(buf), XFRMA_POLICY_TYPE, &ptype, sizeof(ptype));

	if(num_tmpl > 0)
		addattr_l(n, sizeof(buf), XFRMA_TMPL, 
			  tmpls, sizeof(struct xfrm_user_tmpl) * num_tmpl);

	if ((err = rtnl_xfrm_do(n, NULL)) < 0)
		xfrm_policy_dump("Failed to add policy:\n",
				 n->nlmsg_flags, n->nlmsg_type, 
				 pol, &ptype, tmpls, num_tmpl);
	return err;
}

static int xfrm_ipsec_policy_add(const struct xfrm_selector *sel,
				 int update, int dir, int action, int priority,
				 struct xfrm_user_tmpl *tmpls, int num_tmpl)
{
	return xfrm_policy_add(XFRM_POLICY_TYPE_MAIN, sel, update, dir,
			       action, priority, tmpls, num_tmpl);
}

static int xfrm_mip_policy_add(const struct xfrm_selector *sel,
			       int update, int dir, int action, int priority,
			       struct xfrm_user_tmpl *tmpls, int num_tmpl)
{
	return xfrm_policy_add(XFRM_POLICY_TYPE_SUB, sel, update, dir,
			       action, priority, tmpls, num_tmpl);
}

static int xfrm_policy_del(uint8_t type, const struct xfrm_selector *sel, int dir)
{
	uint8_t buf[NLMSG_SPACE(sizeof(struct xfrm_userpolicy_id))
		    + RTA_SPACE(sizeof(struct xfrm_userpolicy_type))];
	struct nlmsghdr *n;
	struct xfrm_userpolicy_id *pol_id;
	struct xfrm_userpolicy_type ptype;
	int err;

	memset(buf, 0, sizeof(buf));
	n = (struct nlmsghdr *)buf;
	n->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_userpolicy_id));
	n->nlmsg_flags = NLM_F_REQUEST;
	n->nlmsg_type = XFRM_MSG_DELPOLICY;

	pol_id = NLMSG_DATA(n);
	memcpy(&pol_id->sel, sel, sizeof(struct xfrm_selector));
	pol_id->dir = dir;

	memset(&ptype, 0, sizeof(ptype));
	ptype.type = type;
	addattr_l(n, sizeof(buf), XFRMA_POLICY_TYPE, &ptype, sizeof(ptype));

	if ((err = rtnl_xfrm_do(n, NULL)) < 0)
		xfrm_policy_id_dump("Failed to del policy:\n", pol_id, &ptype);
	return err;
}

static int xfrm_ipsec_policy_del(const struct xfrm_selector *sel, int dir)
{
	return xfrm_policy_del(XFRM_POLICY_TYPE_MAIN, sel, dir);
}

static int xfrm_mip_policy_del(const struct xfrm_selector *sel, int dir)
{
	return xfrm_policy_del(XFRM_POLICY_TYPE_SUB, sel, dir);
}

static int xfrm_state_add(const struct xfrm_selector *sel,
			  int proto, const struct in6_addr *coa,
			  int update, uint8_t flags)
{
	uint8_t buf[NLMSG_SPACE(sizeof(struct xfrm_usersa_info))
		    + RTA_SPACE(sizeof(struct in6_addr))];
	struct nlmsghdr *n;
	struct xfrm_usersa_info *sa;
	int err;

	memset(buf, 0, sizeof(buf));
	n = (struct nlmsghdr *)buf;
	n->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_usersa_info));
	if (update) {
		n->nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE;
		n->nlmsg_type = XFRM_MSG_UPDSA;
	} else {
		n->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE;
		n->nlmsg_type = XFRM_MSG_NEWSA;
	}
	sa = NLMSG_DATA(n);
	memcpy(&sa->sel, sel, sizeof(struct xfrm_selector));
	/* State src and dst addresses */
	memcpy(sa->id.daddr.a6, sel->daddr.a6, sizeof(sel->daddr.a6));
	sa->id.proto = proto;
	memcpy(sa->saddr.a6, sel->saddr.a6, sizeof(sel->saddr.a6));
	xfrm_lft(&sa->lft);
	sa->family = AF_INET6;
	sa->mode = XFRM_MODE_ROUTEOPTIMIZATION;
	sa->flags = flags;

	addattr_l(n, sizeof(buf), XFRMA_COADDR, coa, sizeof(struct in6_addr));

	if ((err = rtnl_xfrm_do(n, NULL)) < 0)
		xfrm_state_dump("Failed to add state:\n",
				 n->nlmsg_flags, n->nlmsg_type, sa, coa);
	return err;
}

static int xfrm_state_del(int proto, const struct xfrm_selector *sel)
{
	uint8_t buf[NLMSG_SPACE(sizeof(struct xfrm_usersa_id)) +
		    RTA_SPACE(sizeof(xfrm_address_t))];
	struct nlmsghdr *n;
	struct xfrm_usersa_id *sa_id;
	int err;

	memset(buf, 0, sizeof(buf));
	n = (struct nlmsghdr *)buf;
	n->nlmsg_len = NLMSG_LENGTH(sizeof(struct xfrm_usersa_id));
	n->nlmsg_flags = NLM_F_REQUEST;
	n->nlmsg_type = XFRM_MSG_DELSA;

	sa_id = NLMSG_DATA(n);
	/* State src and dst addresses */
	memcpy(sa_id->daddr.a6, sel->daddr.a6, sizeof(sel->daddr.a6));
	sa_id->family = AF_INET6;
	sa_id->proto = proto;

	addattr_l(n, sizeof(buf), XFRMA_SRCADDR, &sel->saddr,
		  sizeof(sel->saddr));

	if ((err = rtnl_xfrm_do(n, NULL)) < 0)
		xfrm_state_id_dump("Failed to del state:\n", sa_id, &sel->saddr);
	return err;
}

static void _create_dstopt_tmpl(struct xfrm_user_tmpl *tmpl,
			       const struct in6_addr *dst,
			       const struct in6_addr *src,
			       int mode)
{
	memset(tmpl, 0, sizeof(*tmpl));
	tmpl->family = AF_INET6;
       	tmpl->id.proto = IPPROTO_DSTOPTS;
	tmpl->mode = mode;
	tmpl->optional = 1;
	tmpl->reqid = 0;
	if (dst)
		memcpy(&tmpl->id.daddr, dst, sizeof(tmpl->id.daddr));
	if (src)
		memcpy(&tmpl->saddr, src, sizeof(tmpl->saddr));
}

static inline void create_dstopt_tmpl(struct xfrm_user_tmpl *tmpl,
				      const struct in6_addr *dst,
				      const struct in6_addr *src)
{
	_create_dstopt_tmpl(tmpl, dst, src, XFRM_MODE_ROUTEOPTIMIZATION);
}

static inline void create_trig_dstopt_tmpl(struct xfrm_user_tmpl *tmpl,
					   const struct in6_addr *dst,
					   const struct in6_addr *src)
{
	_create_dstopt_tmpl(tmpl, dst, src,
			    XFRM_MODE_IN_TRIGGER);
}

static void _create_rh_tmpl(struct xfrm_user_tmpl *tmpl, int mode)
{	
	memset(tmpl, 0, sizeof(*tmpl));
	tmpl->family = AF_INET6;
	tmpl->id.proto = IPPROTO_ROUTING;
	tmpl->mode = mode;
	tmpl->optional = 1;
	tmpl->reqid = 0;
}
static void create_rh_tmpl(struct xfrm_user_tmpl *tmpl)
{
	_create_rh_tmpl(tmpl, XFRM_MODE_ROUTEOPTIMIZATION);
}

static void create_trig_rh_tmpl(struct xfrm_user_tmpl *tmpl)
{
	_create_rh_tmpl(tmpl, XFRM_MODE_IN_TRIGGER);
}

/* Creates a ESP/AH/IPComp policy for protecting signaling bewteen MN
 * and HA */ 
static void create_ipsec_tmpl(struct xfrm_user_tmpl *tmpl, uint8_t proto, 
			      int tunnel,
			      const struct in6_addr *tdst,
			      const struct in6_addr *tsrc,
			      uint32_t reqid)
{
	memset(tmpl, 0, sizeof(*tmpl));
	tmpl->family = AF_INET6;
	tmpl->ealgos = ~(uint32_t)0;
	tmpl->aalgos = ~(uint32_t)0;
	tmpl->id.proto = proto;
	tmpl->optional = 0;
	tmpl->mode = tunnel;
	tmpl->reqid = reqid;
	if (tunnel) {
		memcpy(&tmpl->id.daddr, tdst, sizeof(struct in6_addr));
		memcpy(&tmpl->saddr, tsrc, sizeof(struct in6_addr));
	}
}

static int _mn_ha_ipsec_init(const struct in6_addr *haaddr,
			     const struct in6_addr *hoa,
			     struct ipsec_policy_entry *e,
			     __attribute__ ((unused)) void *arg)
{
	struct xfrm_user_tmpl tmpls[MIPV6_MAX_TMPLS];
	struct xfrm_selector sel;
	int ti;
	int ulp = 0;
	int type_in = 0, type_out = 0;
	int prio = MIP6_PRIO_HOME_DATA_IPSEC;
	int dir;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_NDISC:
		type_in = ND_NEIGHBOR_SOLICIT;
		type_out = ND_NEIGHBOR_ADVERT;
		ulp = IPPROTO_ICMPV6;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_ADVERT;
		type_out = MIP_PREFIX_SOLICIT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
		break;
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		type_in = IP6_MH_TYPE_BACK;
		type_out = IP6_MH_TYPE_BU;
	case IPSEC_POLICY_TYPE_MH:
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	/* inbound */
	set_selector(hoa, haaddr, ulp, type_in, 0, 0, &sel);
	dir = XFRM_POLICY_IN;
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_tomn);
	if (xfrm_ipsec_policy_add(&sel, 0, dir, e->action, prio, tmpls, ti))
		return -1;

	/* outbound */
	set_selector(haaddr, hoa, ulp, type_out, 0, 0, &sel);
	dir = XFRM_POLICY_OUT;
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_toha);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_toha);
	/* AH template must be placed as the last one for outbound SP */
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_toha);
	return xfrm_ipsec_policy_add(&sel, 0, dir, e->action, prio, &tmpls[0], ti);
}

static int _mn_ha_ipsec_bypass_init(__attribute__ ((unused)) const struct in6_addr *haaddr,
				    const struct in6_addr *hoa,
				    struct ipsec_policy_entry  *e,
				    __attribute__ ((unused)) void *arg)
{
	struct xfrm_selector sel;
	int prio = MIP6_PRIO_BYPASS_BU;
	int err = 0;

	/* set bypass policy for allowing MN to send BU over RO path to
	   its CN. */
	switch (e->type) {
	case IPSEC_POLICY_TYPE_MH:
		set_selector(&in6addr_any, hoa, IPPROTO_MH,
			     IP6_MH_TYPE_BU, 0, 0, &sel);
		err = xfrm_ipsec_policy_add(&sel, 0, XFRM_POLICY_OUT,
				            XFRM_POLICY_ALLOW, prio,
					    NULL, 0);
		break;
	default:
		break;
	}

	return err;
}

static int mr_ipsec_bypass_init(void)
{
	struct list_head *home;
	struct list_head *mnps;
	int prio = MIP6_PRIO_MR_LOCAL_DATA_BYPASS;
	int err=0;

	/* Loop for each HomeAddress info */
	list_for_each(home, &conf.home_addrs) {
		struct home_addr_info *hai;
		hai = list_entry(home, struct home_addr_info, list);

		if (!hai->mob_rtr)
			continue;

		/* Mobile Router for this link so loop for each MNP to
		 * add bypass policies to *and* from the MNP link */
		list_for_each(mnps, &hai->mob_net_prefixes) {
			struct prefix_list_entry * mnp;
			struct xfrm_selector sel;
			uid_t uid = getuid();

			mnp = list_entry(mnps, struct prefix_list_entry, list);

			/* IN, src = MNP , dst = any */
			mr_set_selector(mnp, NULL, uid, &sel);
			err = xfrm_ipsec_policy_add(&sel, 0, XFRM_POLICY_IN,
						    XFRM_POLICY_ALLOW, prio,
						    NULL, 0);

			/* FWD, src = MNP , dst = any */
			err = xfrm_ipsec_policy_add(&sel, 0, XFRM_POLICY_FWD,
						    XFRM_POLICY_ALLOW, prio,
						    NULL, 0);

			/* OUT, src = any , dst = MNP */
			mr_set_selector(NULL, mnp, uid, &sel);
			err = xfrm_ipsec_policy_add(&sel, 0, XFRM_POLICY_OUT,
						    XFRM_POLICY_ALLOW, prio,
						    NULL, 0);
		}
	}

	return err;
}

static inline int mn_ha_ipsec_init(void)
{
	/* insert bypass policy */
	if (ipsec_policy_walk(_mn_ha_ipsec_bypass_init, NULL))
		return -1;

	/* insert NEMO-related bypass */
	if (mr_ipsec_bypass_init())
		return -1;

	if (ipsec_policy_walk(_mn_ha_ipsec_init, NULL))
		return -1;

	return 0;
}

/* Create a state and policy for receiving routing header type 2 to
 * any home address from any CN / HA */
static int xfrm_mn_init(void)
{
	struct xfrm_selector sel;
	struct xfrm_user_tmpl tmpl;

	XDBG("Adding policies and states for MN\n");

	if (conf.UseMnHaIPsec && mn_ha_ipsec_init() < 0)
		return -1;

	/* policy for sending BE */
	/* The priolity is higher than the block policy so that MN can send BE during registration */
	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_MH, IP6_MH_TYPE_BERROR, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_HOME_SIG_ANY, NULL, 0) < 0)
		return -1;

	XDBG2("Adding RTHdr type 2 handling 2 state for MN\n");
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	create_rh_tmpl(&tmpl);
	if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_IN, XFRM_POLICY_ALLOW,
				MIP6_PRIO_RO_DATA_ANY, &tmpl, 1))
		return -1;

	return xfrm_state_add(&sel, IPPROTO_ROUTING, &in6addr_any, 0,
			      XFRM_STATE_WILDRECV);
}

static int _mn_ha_ipsec_cleanup(const struct in6_addr *haaddr,
				const struct in6_addr *hoa,
				struct ipsec_policy_entry *e,
				__attribute__ ((unused)) void *arg)
{
	struct xfrm_selector sel;
	int ulp = 0;
	int type_in = 0, type_out = 0;
	int prio = MIP6_PRIO_HOME_DATA_IPSEC;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_NDISC:
		type_in = ND_NEIGHBOR_SOLICIT;
		type_out = ND_NEIGHBOR_ADVERT;
		ulp = IPPROTO_ICMPV6;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_ADVERT;
		type_out = MIP_PREFIX_SOLICIT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
		break;
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		type_in = IP6_MH_TYPE_BACK;
		type_out = IP6_MH_TYPE_BU;
	case IPSEC_POLICY_TYPE_MH:
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	/* inbound */
	set_selector(hoa, haaddr, ulp, type_in, 0, 0, &sel);
	xfrm_ipsec_policy_del(&sel, XFRM_POLICY_IN);
	/* outbound */
	set_selector(haaddr, hoa, ulp, type_out, 0, 0, &sel);
	xfrm_ipsec_policy_del(&sel, XFRM_POLICY_OUT);
	return 0;
}

static int _mn_ha_ipsec_bypass_cleanup(__attribute__ ((unused)) const struct in6_addr *haaddr,
				       const struct in6_addr *hoa,
				       struct ipsec_policy_entry *e,
				       __attribute__ ((unused)) void *arg)
{
	struct xfrm_selector sel;
	int err = 0;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_MH:
		set_selector(&in6addr_any, hoa, IPPROTO_MH,
			     IP6_MH_TYPE_BU, 0, 0, &sel);
		err = xfrm_ipsec_policy_del(&sel, XFRM_POLICY_OUT);
		break;
	default:
		break;
	}

	return err;
}

static int mr_ipsec_bypass_cleanup(void)
{
	struct list_head *home;
	struct list_head *mnps;
	int err=0;

	/* Loop for each HomeAddress info */
	list_for_each(home, &conf.home_addrs)
	{
		struct home_addr_info *hai;
		hai = list_entry(home, struct home_addr_info, list);

		if (!hai->mob_rtr)
			continue;

		/* Mobile Router for this link so loop for each MNP to
		 * delete bypass policies to *and* from the MNP link */
		list_for_each(mnps, &hai->mob_net_prefixes) {
			struct prefix_list_entry * mnp;
			struct xfrm_selector sel;
			uid_t uid = getuid();

			mnp = list_entry(mnps, struct prefix_list_entry, list);

			/* IN, src = MNP , dst = any */
			mr_set_selector(mnp, NULL, uid, &sel);
			err = xfrm_ipsec_policy_del(&sel, XFRM_POLICY_IN);

			/* FWD, src = MNP , dst = any */
			err = xfrm_ipsec_policy_del(&sel, XFRM_POLICY_FWD);

			/* OUT, src = any , dst = MNP */
			mr_set_selector(NULL, mnp, uid, &sel);
			err = xfrm_ipsec_policy_del(&sel, XFRM_POLICY_OUT);
		}
	}

	return err;
}


static inline void mn_ha_ipsec_cleanup(void)
{
	ipsec_policy_walk(_mn_ha_ipsec_bypass_cleanup, NULL);

	(void)mr_ipsec_bypass_cleanup();

	ipsec_policy_walk(_mn_ha_ipsec_cleanup, NULL);
}

static void xfrm_mn_cleanup(void)
{
	struct xfrm_selector sel;

	/* the policy for sending BE */
	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_MH, IP6_MH_TYPE_BERROR, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);


	XDBG("Deleting policies and states for MN\n");
	XDBG2("Deleting RTHdr type 2 handling 2 state for MN\n");
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	xfrm_state_del(IPPROTO_ROUTING, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
	if (conf.UseMnHaIPsec)
		mn_ha_ipsec_cleanup();

}

static int _ha_mn_ipsec_init(const struct in6_addr *haaddr,
			     const struct in6_addr *hoa,
			     struct ipsec_policy_entry *e,
			     __attribute__ ((unused)) void *arg)
{
	struct xfrm_user_tmpl tmpls[MIPV6_MAX_TMPLS];
	struct xfrm_selector sel;
	int ti;
	int ulp = 0;
	int type_in = 0, type_out = 0;
	int prio = MIP6_PRIO_HOME_DATA_IPSEC;
	int use_dst = 0;
	int dir;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_NDISC:
		type_in = ND_NEIGHBOR_ADVERT;
		type_out = ND_NEIGHBOR_SOLICIT;
		ulp = IPPROTO_ICMPV6;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_SOLICIT;
		type_out = MIP_PREFIX_ADVERT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
		break;
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		type_in = IP6_MH_TYPE_BU;
		type_out = IP6_MH_TYPE_BACK;
	case IPSEC_POLICY_TYPE_MH:
		ulp = IPPROTO_MH;
		use_dst = 1;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_BERROR:
		type_in = -1;
		type_out = IP6_MH_TYPE_BERROR;
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_ERROR;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	if (type_in >= 0) {
		/* inbound */
		set_selector(haaddr, hoa, ulp, type_in, 0, 0, &sel);
		dir = XFRM_POLICY_IN;
		ti = 0;
		if (ipsec_use_ipcomp(e))
			create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
					  NULL, NULL, e->reqid_toha);
		if (ipsec_use_esp(e))
			create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
					  NULL, NULL, e->reqid_toha);
		if (ipsec_use_ah(e))
			create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
					  NULL, NULL, e->reqid_toha);
		if (xfrm_ipsec_policy_add(&sel, 0, dir, e->action, prio,
					  tmpls, ti))
			return -1;
	}
	/* outbound */
	set_selector(hoa, haaddr, ulp, type_out, 0, 0, &sel);
	dir = XFRM_POLICY_OUT;
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_tomn);
	/* AH template must be placed as the last one for outbound SP */
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_tomn);
	return xfrm_ipsec_policy_add(&sel, 0, dir, e->action, prio, &tmpls[0], ti);
}

static inline int ha_mn_ipsec_init(void)
{
	XDBG("Adding IPsec policies and states for HA\n");
	return ipsec_policy_walk(_ha_mn_ipsec_init, NULL);
}

static int xfrm_ha_init(void)
{
	XDBG("Adding policies and states for HA\n");
	if (conf.UseMnHaIPsec && ha_mn_ipsec_init() < 0)
		return -1;
	return 0;
}

static int _ha_mn_ipsec_cleanup(const struct in6_addr *haaddr,
				const struct in6_addr *hoa,
				struct ipsec_policy_entry *e,
				__attribute__ ((unused)) void *arg)
{
	struct xfrm_selector sel;
	int ulp = 0;
	int type_in = 0, type_out = 0;
	int prio = MIP6_PRIO_HOME_DATA_IPSEC;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_NDISC:
		type_in = ND_NEIGHBOR_ADVERT;
		type_out = ND_NEIGHBOR_SOLICIT;
		ulp = IPPROTO_ICMPV6;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_SOLICIT;
		type_out = MIP_PREFIX_ADVERT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
		break;
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		type_in = IP6_MH_TYPE_BU;
		type_out = IP6_MH_TYPE_BACK;
	case IPSEC_POLICY_TYPE_MH:
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_BERROR:
		type_in = -1;
		type_out = IP6_MH_TYPE_BERROR;
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_ERROR;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	if (type_in >= 0) {
		/* inbound */
		set_selector(haaddr, hoa, ulp, type_in, 0, 0, &sel);
		xfrm_ipsec_policy_del(&sel, XFRM_POLICY_IN);
	}
	/* outbound */
	set_selector(hoa, haaddr, ulp, type_out, 0, 0, &sel);
	xfrm_ipsec_policy_del(&sel, XFRM_POLICY_OUT);
	return 0;
}

static inline void ha_mn_ipsec_cleanup(void)
{
	ipsec_policy_walk(_ha_mn_ipsec_cleanup, NULL);
}

static void xfrm_ha_cleanup(void)
{
	XDBG("Deleting policies and states for HA\n");

	if (conf.UseMnHaIPsec)
		ha_mn_ipsec_cleanup();
}

int xfrm_cn_policy_mh_out_touch(int update)
{
	struct xfrm_selector sel;

	/* policy to not add rth type 2 to MH msgs */
	set_selector(&in6addr_any, &in6addr_any, IPPROTO_MH, 0, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, update, XFRM_POLICY_OUT,
				XFRM_POLICY_ALLOW, MIP6_PRIO_NO_RO_SIG_ANY,
				NULL, 0) < 0)
		return -1;
	return 0;
}

/* Create or clean up initial xfrm policies and states for CN */
static int xfrm_cn_init(void)
{
	struct xfrm_selector sel;

	XDBG("Adding policies and states for CN\n");

	/* Create policy for all BUs with home flag NOT set to 
	   use home address option */
	if (cn_wildrecv_bu_pol_add())
		return -1;

	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	if (xfrm_state_add(&sel, IPPROTO_DSTOPTS, 
			   &in6addr_any, 0, XFRM_STATE_WILDRECV) < 0)
		return -1;

	if (xfrm_cn_policy_mh_out_touch(0) < 0)
		return -1;

	/* Let Neighbor Solicitation messages bypass bindings */
	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, ND_NEIGHBOR_SOLICIT, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_NO_RO_SIG_ANY, NULL, 0) < 0)
		return -1;

	/*
	 * Let Neighbor Advertisement messages bypass bindings 
	 * This policy is high priority(priory 3) not to block 
	 * by the BlockPolicy during registration.
	 */
	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, ND_NEIGHBOR_ADVERT, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_HOME_SIG_ANY, NULL, 0) < 0)
		return -1;

	/* Let ICMPv6 error messages bypass bindings */
	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, 0, 0, 0, &sel);
	sel.sport_mask = htons(0x80);
	return xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				   MIP6_PRIO_NO_RO_SIG_ANY, NULL, 0);
}

static void xfrm_cn_cleanup(void)
{
	struct xfrm_selector sel;
	
	XDBG("Deleting policies and states for CN\n");

	cn_wildrecv_bu_pol_del();

	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	xfrm_state_del(IPPROTO_DSTOPTS, &sel);

	set_selector(&in6addr_any, &in6addr_any, IPPROTO_MH, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);

	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, ND_NEIGHBOR_SOLICIT, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);

	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, ND_NEIGHBOR_ADVERT, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);

	set_selector(&in6addr_any, &in6addr_any,
		     IPPROTO_ICMPV6, 0, 0, 0, &sel);
	sel.sport_mask = htons(0x80);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
}

static int mn_ro_pol_chk(const struct home_addr_info *hai,
			 const struct in6_addr *cn)
{
	struct list_head *l;
	int res = MIP6_PRIO_RO_TRIG_ANY;

	if (!conf.DoRouteOptimizationMN)
		return -1; 

	list_for_each(l, &hai->ro_policies) {
		struct xfrm_ro_pol *pol;
		pol = list_entry(l, struct xfrm_ro_pol, list);

		if (IN6_ARE_ADDR_EQUAL(cn, &pol->cn_addr))
			return pol->do_ro ? MIP6_PRIO_RO_TRIG : -1;

		if (IN6_IS_ADDR_UNSPECIFIED(&pol->cn_addr) && !pol->do_ro)
			res = -1;
 	}
	return res;
}

static inline int mn_has_cn_ro_pol(struct bulentry *e)
{
	return mn_ro_pol_chk(e->home, &e->peer_addr) == MIP6_PRIO_RO_TRIG;
}

static int _mn_bule_ro_pol_add(struct bulentry *e, int iif, int replace)
{
	struct xfrm_selector sel;
	int action;
	int prio;
	if (e->flags & IP6_MH_BU_HOME) {
		/* block payload data to HA until binding is complete */
		action = XFRM_POLICY_BLOCK;
		prio = MIP6_PRIO_HOME_BLOCK;
	} else {
		/* reverse tunnel payload traffic until binding is complete */
		action = XFRM_POLICY_ALLOW;
		prio = MIP6_PRIO_NO_RO_DATA;
		replace |= mn_has_cn_ro_pol(e);
	}
	set_selector(&e->peer_addr, &e->hoa, 0, 0, 0, iif, &sel);
	return xfrm_mip_policy_add(&sel, replace, XFRM_POLICY_OUT,
				   action, prio, NULL, 0);
}

static int mn_bule_ro_pol_add(void *vbule, void *viif)
{
	struct bulentry *e = vbule;
	int *iif = viif;
	if (e->type != BUL_ENTRY)
		return 0;
	return _mn_bule_ro_pol_add(e, *iif, 0);
}


static int _mn_bce_ro_pol_add(const struct in6_addr *our_addr,
			      const struct in6_addr *peer_addr)
{
	struct xfrm_user_tmpl tmpls[2];
	struct xfrm_selector sel;
	create_rh_tmpl(&tmpls[0]);
	create_trig_dstopt_tmpl(&tmpls[1], peer_addr, our_addr);
	set_selector(peer_addr, our_addr, 0, 0, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_RO_BCE_DATA, tmpls, 2))
		return -1;
	create_dstopt_tmpl(&tmpls[0], our_addr, peer_addr);
	create_trig_rh_tmpl(&tmpls[1]);
	set_selector(our_addr, peer_addr, 0, 0, 0, 0, &sel);
	return xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_IN, XFRM_POLICY_ALLOW,
				   MIP6_PRIO_RO_BCE_DATA, tmpls, 2);

}

static int mn_bce_ro_pol_add(void *vbce, void *vhai)
{
	struct bcentry *e = vbce;
	struct home_addr_info *hai = vhai;
	int err = 0;
	pthread_rwlock_rdlock(&e->lock);
	if (e->type > BCE_NONCE_BLOCK && 
	    e->type != BCE_HOMEREG && e->type != BCE_DAD &&
	    mn_ro_pol_chk(hai, &e->peer_addr))
		err = _mn_bce_ro_pol_add(&e->our_addr, &e->peer_addr);
	pthread_rwlock_unlock(&e->lock);
	return err;
}

/** mn_ro_pol_add - adds xfrm policies related to RO in MN
 * @hai HoA info
 * @ifindex tunnel iface index
 *
 * Create IPsec policies for protection of RR signaling in MN and adds
 * user configurable policies for triggering RO.
 */ 

int mn_ro_pol_add(struct home_addr_info *hai, int ifindex, int changed)
{
	int wildcard = 0;
	struct xfrm_selector sel;
	struct xfrm_user_tmpl otmpl, itmpl;
	struct list_head *list;

	assert(hai && hai->home_reg_status != HOME_REG_NONE);

	if (!conf.DoRouteOptimizationMN)
		return 0;
	if (changed)
		bul_iterate(&hai->bul, mn_bule_ro_pol_add, &ifindex);
	else
		bcache_iterate(mn_bce_ro_pol_add, hai);

	create_trig_rh_tmpl(&itmpl);

	/* RO triggering policy */	
	list_for_each(list, &hai->ro_policies) {
		struct xfrm_ro_pol *pol; 
		int ntmpl = 0;
		int prio = MIP6_PRIO_RO_TRIG;

		pol = list_entry(list, struct xfrm_ro_pol, list);
		if (IN6_IS_ADDR_UNSPECIFIED(&pol->cn_addr)) {
			prio = MIP6_PRIO_RO_TRIG_ANY;
			wildcard = 1;
		}
		if (pol->do_ro) {
			ntmpl = 1;
			create_trig_dstopt_tmpl(&otmpl, &pol->cn_addr,
						&hai->hoa.addr);
		}
		XDBG("Adding %sRO policies for %x:%x:%x:%x:%x:%x:%x:%x\n",
		    pol->do_ro ? "": "no-", NIP6ADDR(&pol->cn_addr));
		set_selector(&pol->cn_addr, &hai->hoa.addr,
			     0, 0, 0, ifindex, &sel);
		if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT,
					XFRM_POLICY_ALLOW, prio, &otmpl, ntmpl))
			return -1;
		set_selector(&hai->hoa.addr, &pol->cn_addr,
			     0, 0, 0, ifindex, &sel);
		if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_IN,
					XFRM_POLICY_ALLOW, prio, &itmpl, ntmpl))
			return -1;
	}
	if (!wildcard) {
		XDBG("Adding default RO triggering policies for all Correspondent Nodes\n");
		create_trig_dstopt_tmpl(&otmpl, &in6addr_any, &hai->hoa.addr);
		set_selector(&in6addr_any, &hai->hoa.addr, 0,
			     0, 0, 0, &sel);
		if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT,
					XFRM_POLICY_ALLOW,
					MIP6_PRIO_RO_TRIG_ANY, &otmpl, 1))
			return -1;
#if 0  // Disable inboud trigger because of ifindex problem
		set_selector(&hai->hoa.addr, &in6addr_any,
			     0, 0, 0, 0, &sel);
		if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_IN,
					XFRM_POLICY_ALLOW,
					MIP6_PRIO_RO_TRIG_ANY, &itmpl, 1))
			return -1;
#endif
	}

	return 0;
}

static void _mn_bule_ro_pol_del(struct bulentry *e)
{
	struct xfrm_selector sel;
	set_selector(&e->peer_addr, &e->hoa, 0, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);

	/*
	 * XXX: inbound is missed???
	 */
	set_selector(&e->hoa, &e->peer_addr, 0, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
}

static int _xfrm_bce_reset(struct bulentry *bule);
int mn_bule_ro_pol_del(void *vbule, __attribute__ ((unused)) void *viif)
{
	struct bulentry *e = vbule;
	if (e->type == BUL_ENTRY)
		_mn_bule_ro_pol_del(e);
	if (!(e->flags & IP6_MH_BU_HOME) )
		_xfrm_bce_reset(e);
	return 0;
}

/** mn_ro_pol_del - deletes xfrm policies related to RO in MN
 * @hai HoA info
 * @ifindex tunnel iface index
 *
 * Removes IPsec policies for protection of RR signaling in MN and deletes
 * user configurable policies for triggering RO.
 */ 

void mn_ro_pol_del(struct home_addr_info *hai, int ifindex, int changed)
{
	int wildcard = 0;
	struct xfrm_selector sel;
	struct list_head *list;

	assert(hai && hai->home_reg_status != HOME_REG_NONE);

	if (!conf.DoRouteOptimizationMN)
		return;

	if (changed)
		bul_iterate(&hai->bul, mn_bule_ro_pol_del, &ifindex);

	list_for_each(list, &hai->ro_policies) {
		struct xfrm_ro_pol *pol; 
		pol = list_entry(list, struct xfrm_ro_pol, list);
		if (IN6_IS_ADDR_UNSPECIFIED(&pol->cn_addr))
			wildcard = 1;
		set_selector(&pol->cn_addr, &hai->hoa.addr,
			     0, 0, 0, ifindex, &sel);
		xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
		set_selector(&hai->hoa.addr, &pol->cn_addr,
			     0, 0, 0, ifindex, &sel);
		xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
	}
	if (!wildcard) {
		XDBG("Deleting default RO triggering policies for all Correspondent Nodes\n");
		set_selector(&in6addr_any, &hai->hoa.addr, 0,
			     0, 0, 0, &sel);
		xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
#if 0  // Disable inboud trigger because of ifindex problem
		set_selector(&hai->hoa.addr, &in6addr_any,
			     0, 0, 0, 0, &sel);
		xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
#endif
	}
}

/* XXX TENTATIVE */
int xfrm_ipsec_policy_mod(struct xfrm_userpolicy_info *sp,
			  struct xfrm_user_tmpl *tmpl,
			  int num_tmpl,
			  int cmd)
{
	if (cmd == 0) { /* delete */
		return (xfrm_ipsec_policy_del(&sp->sel, sp->dir));
	} else
	if (cmd == 1) { /* add */
		return (xfrm_ipsec_policy_add(&sp->sel, 0, sp->dir, sp->action,
					      sp->priority, tmpl, num_tmpl));
	} else
		return -1;
}

/** 
 * _ha_mn_ipsec_pol_mod - modifies wildcard and ICMP policies between HA and MN
 * @haaddr: HA's address
 * @hoa: Home address
 * @e: pointer to the ipsec_policy_entry
 * @arg: registration
 *
 * Adds xfrm_policies into kernel that are needed for home reg/dereg.
 */
static int _ha_mn_ipsec_pol_mod(const struct in6_addr *haaddr,
				const struct in6_addr *hoa,
				struct ipsec_policy_entry *e,
				__attribute__ ((unused)) void *arg)
{
	struct xfrm_user_tmpl tmpls[MIPV6_MAX_TMPLS];
	int ti = 0;
	struct xfrm_selector sel;
	int type_in = 0, type_out = 0;
	int ulp = 0;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_SOLICIT;
		type_out = MIP_PREFIX_ADVERT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	
	/* outbound */
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_tomn);
	/* AH template must be placed as the last one for outbound SP */
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_tomn);

	set_selector(hoa, haaddr, ulp, type_out, 0, 0, &sel);
	if (xfrm_ipsec_policy_add(&sel, 1,
				  XFRM_POLICY_OUT, e->action,
				  MIP6_PRIO_HOME_DATA_IPSEC, &tmpls[0], ti))
		return -1;

	/* inbound */
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_toha);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_toha);
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_toha);
	set_selector(haaddr, hoa, ulp, type_in, 0, 0, &sel);
	if (xfrm_ipsec_policy_add(&sel, 1,
				  XFRM_POLICY_IN, e->action,
				  MIP6_PRIO_HOME_DATA_IPSEC, &tmpls[0], ti))
		return -1;

	return 0;
}

int ha_mn_ipsec_pol_mod(struct in6_addr *haaddr, struct in6_addr *hoa)
{
	return ipsec_policy_apply(haaddr, hoa, _ha_mn_ipsec_pol_mod, NULL);
}

static int _xfrm_add_bce(const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr,
			 int replace)
{
	struct xfrm_user_tmpl tmpl;
	struct xfrm_selector sel;

	/* MN - CN case data out & in */
	create_rh_tmpl(&tmpl);
	set_selector(peer_addr, our_addr, 0, 0, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, replace, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_RO_BCE_DATA, &tmpl, 1))
		return -1;
	create_dstopt_tmpl(&tmpl, our_addr, peer_addr);
	set_selector(our_addr, peer_addr, 0, 0, 0, 0, &sel);
	return xfrm_mip_policy_add(&sel, replace, XFRM_POLICY_IN,
				   XFRM_POLICY_ALLOW, MIP6_PRIO_RO_BCE_DATA,
				   &tmpl, 1);
}

static int _xfrm_add_bule_bce(const struct in6_addr *our_addr,
			      const struct in6_addr *peer_addr,
			      int replace)
{
	struct xfrm_user_tmpl tmpls[2];
	struct xfrm_selector sel;
	replace = 1;
	create_rh_tmpl(&tmpls[0]);
	create_dstopt_tmpl(&tmpls[1], peer_addr, our_addr);
	set_selector(peer_addr, our_addr, 0, 0, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, replace, XFRM_POLICY_OUT, XFRM_POLICY_ALLOW,
				MIP6_PRIO_RO_BULE_BCE_DATA, tmpls, 2))
		return -1;
	create_dstopt_tmpl(&tmpls[0], our_addr, peer_addr);
	create_rh_tmpl(&tmpls[1]);
	set_selector(our_addr, peer_addr, 0, 0, 0, 0, &sel);
	return xfrm_mip_policy_add(&sel, replace, XFRM_POLICY_IN,
				   XFRM_POLICY_ALLOW,
				   MIP6_PRIO_RO_BULE_BCE_DATA, tmpls, 2);
}

static int xfrm_bule_bce_update(const struct in6_addr *our_addr,
				const struct in6_addr *peer_addr,
				int replace)
{
	struct home_addr_info *hai;
	struct bulentry *e;
	int res = -1;
	pthread_rwlock_rdlock(&mn_lock);
	if ((hai = mn_get_home_addr(our_addr)) != NULL) {
		if ((e = bul_get(hai, NULL, peer_addr)) != NULL) {
			if (e->type == BUL_ENTRY &&
			    !_xfrm_add_bule_bce(our_addr, peer_addr, replace))
				res = 0;
		} else if (mn_ro_pol_chk(hai, peer_addr) >= 0 &&
			   !_mn_bce_ro_pol_add(our_addr, peer_addr))
			res = 0;
	}
	pthread_rwlock_unlock(&mn_lock);
	return res;
}

/** 
 * xfrm_add_bce - add xfrm_states and xfrm_policies for a BC entry 
 * @our_addr: our IPv6 address
 * @peer_addr: peer's IPv6 address
 * @coa: care-of address
 * @replace: udpate or new entry 
 *
 * Adds binding cache entry to kernel for route optimization and also
 * IPsec policies for protecting MH signaling between MN and HA. Merges
 * existing xfrm_policy in case of MN-MN communication withe the one resulting
 * from binding update list entry.  
 */
int xfrm_add_bce(const struct in6_addr *our_addr,
		 const struct in6_addr *peer_addr,
		 const struct in6_addr *coa,
		 int replace)
{
	struct xfrm_selector sel;

	/* Create policy for outbound RO data traffic */
	set_selector(peer_addr, our_addr, 0, 0, 0, 0, &sel);
	if (xfrm_state_add(&sel, IPPROTO_ROUTING, coa, replace, 0)){
		/* 
		 * WORKAROUND 
		 * In some cases, MN fail to add it because of the state
		 * inserted by kernel when notifying aquire. So,update it.
		 */
		if (xfrm_state_add(&sel, IPPROTO_ROUTING, coa, 1, 0))
			return -1;
	}
	set_selector(our_addr, peer_addr, 0, 0, 0, 0, &sel);
	if (xfrm_state_add(&sel, IPPROTO_DSTOPTS, coa, replace, 0)){
		/* 
		 * WORKAROUND 
		 * In some cases, MN fail to add it because of the state
		 * inserted by kernel when notifying aquire. So,update it.
		 */
		if (xfrm_state_add(&sel, IPPROTO_DSTOPTS, coa, 1, 0))
		return -1;
	}
	if (is_mn() && !xfrm_bule_bce_update(our_addr, peer_addr, replace))
		return 0;
	if (is_ha() && conf.UseMnHaIPsec) {
		if (ipsec_policy_apply(our_addr, peer_addr,
				       _ha_mn_ipsec_pol_mod, NULL) < 0)
			return -1;
	}
	_xfrm_add_bce(our_addr, peer_addr, replace);
	return 0;
}

/** 
 * xfrm_del_bce - remove xfrm_states and xfrm_policies for a BC entry 
 * @our_addr: our IPv6 address
 * @peer_addr: peer's IPv6 address
 *
 * Deletes binding cache entry from kernel and also IPsec
 * policies.
 */
void xfrm_del_bce(const struct in6_addr *our_addr,
		  const struct in6_addr *peer_addr)
{
	struct xfrm_selector sel;
	set_selector(peer_addr, our_addr, 0, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
	xfrm_state_del(IPPROTO_ROUTING,  &sel);
	set_selector(our_addr, peer_addr, 0, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
	xfrm_state_del(IPPROTO_DSTOPTS, &sel);

	if (is_mn()){
		struct home_addr_info *hai;
		struct bulentry *e;
		/* for MN-MN communications, checking BUL to insert RO policy */
		pthread_rwlock_rdlock(&mn_lock);
		if ((hai = mn_get_home_addr(our_addr)) != NULL) {
			if ((e = bul_get(hai, NULL, peer_addr)) != NULL) {
				if (e->type == BUL_ENTRY)
			    	_xfrm_add_bule_bce(our_addr, peer_addr, 0);
			}
		}
		pthread_rwlock_unlock(&mn_lock);
	}
}

/** 
 * _mn_ha_ipsec_pol_mod - modifies XFRM policy
 * @haaddr: HA's address
 * @hoa: Home address
 * @e: pointer to the ipsec_policy_entry
 *
 * Adds xfrm_policies into kernel that are needed for home reg/dereg.
 */
static int _mn_ha_ipsec_pol_mod(const struct in6_addr *haaddr,
				const struct in6_addr *hoa,
				struct ipsec_policy_entry *e,
				__attribute__ ((unused)) void *arg)
{
	struct xfrm_user_tmpl tmpls[MIPV6_MAX_TMPLS];
	struct xfrm_selector sel;
	int ti;
	int ulp = 0;
	int type_in = 0, type_out = 0;
	int prio = MIP6_PRIO_HOME_DATA_IPSEC;
	int dir;

	switch (e->type) {
	case IPSEC_POLICY_TYPE_MOBPFXDISC:
		type_in = MIP_PREFIX_ADVERT;
		type_out = MIP_PREFIX_SOLICIT;
	case IPSEC_POLICY_TYPE_ICMP:
		ulp = IPPROTO_ICMPV6;
		break;
	case IPSEC_POLICY_TYPE_HOMEREGBINDING:
		type_in = IP6_MH_TYPE_BACK;
		type_out = IP6_MH_TYPE_BU;
	case IPSEC_POLICY_TYPE_MH:
		ulp = IPPROTO_MH;
		prio = MIP6_PRIO_HOME_SIG;
		break;
	case IPSEC_POLICY_TYPE_ANY:
		break;
	default:
		return 0;
	}
	/* inbound */
	set_selector(hoa, haaddr, ulp, type_in, 0, 0, &sel);
	dir = XFRM_POLICY_IN;
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_tomn);
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_tomn);
	if (xfrm_ipsec_policy_add(&sel, 1, dir, e->action, prio, tmpls, ti))
		return -1;

	/* outbound */
	set_selector(haaddr, hoa, ulp, type_out, 0, 0, &sel);
	dir = XFRM_POLICY_OUT;
	ti = 0;
	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, 0,
				  NULL, NULL, e->reqid_toha);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, 0,
				  NULL, NULL, e->reqid_toha);
	/* AH template must be placed as the last one for outbound SP */
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, 0,
				  NULL, NULL, e->reqid_toha);
	if (xfrm_ipsec_policy_add(&sel, 1, dir, e->action, prio, &tmpls[0], ti))
		return -1;
	return 0;
}

static inline int mn_ha_ipsec_pol_mod(struct in6_addr *ha_addr,
				      struct in6_addr *hoa)
{
	return ipsec_policy_apply(ha_addr, hoa, _mn_ha_ipsec_pol_mod, NULL);
}

static int _xfrm_bce_reset(struct bulentry *bule)
{
	/* for MN - MN communication */
	struct bcentry *bce = bcache_get(&bule->hoa, &bule->peer_addr);
	int res = 0;
	if (!bce)
		return 0;
	if (bce->type > BCE_NONCE_BLOCK) {
		if (bule->home->at_home)
			res = _xfrm_add_bce(&bule->hoa, &bule->peer_addr, 1);
		else if (mn_ro_pol_chk(bule->home, &bule->peer_addr) >= 0)
			res =_mn_bce_ro_pol_add(&bule->hoa, &bule->peer_addr);
	}
	bcache_release_entry(bce);
	return res;
}

static int _xfrm_del_bule_data(struct bulentry *bule)
{
	struct xfrm_selector sel;
	int prio;

	set_selector(&bule->peer_addr, &bule->hoa, 0, 0, 0, 0, &sel);
	xfrm_state_del(IPPROTO_DSTOPTS,  &sel);

	if (bule->home->home_reg_status != HOME_REG_NONE && 
	    mn_has_cn_ro_pol(bule)) {
		struct xfrm_user_tmpl tmpl;
		create_trig_dstopt_tmpl(&tmpl, &bule->peer_addr, &bule->hoa);
		set_selector(&bule->peer_addr, &bule->hoa,
			     0, 0, 0, bule->home->if_tunnel, &sel);
		xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_OUT,
				    XFRM_POLICY_ALLOW, MIP6_PRIO_RO_TRIG, &tmpl, 1);
	} else
		_mn_bule_ro_pol_del(bule);

	if (!(bule->flags & IP6_MH_BU_HOME))
		_xfrm_bce_reset(bule);
	if (bule->flags & IP6_MH_BU_HOME && conf.UseMnHaIPsec) {
		ipsec_policy_apply(&bule->home->ha_addr, &bule->hoa,
				   _mn_ha_ipsec_pol_mod, NULL);
	}
	/* MN - CN/HA case, BU out */
	prio = (bule->flags & IP6_MH_BU_HOME ?
		MIP6_PRIO_HOME_SIG : MIP6_PRIO_RO_SIG);
	if (bule->flags & IP6_MH_BU_ACK) {
		set_selector(&bule->hoa, &bule->peer_addr, IPPROTO_MH,
			     IP6_MH_TYPE_BACK, 0, 0, &sel);
		if (xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_IN,
					XFRM_POLICY_ALLOW, prio, NULL, 0))
			return -1;
	}
	set_selector(&bule->peer_addr, &bule->hoa, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_OUT,
				XFRM_POLICY_ALLOW, prio, NULL, 0))
		return -1;

	bule->xfrm_state &= ~BUL_XFRM_STATE_DATA;
	return 0;
}

static void _xfrm_del_bule_sig(struct bulentry *bule)
{
	if (bule->xfrm_state & BUL_XFRM_STATE_SIG) { 
		struct xfrm_selector sel;
		set_selector(&bule->peer_addr, &bule->hoa, IPPROTO_MH,
			     IP6_MH_TYPE_BU, 0, 0, &sel);
		xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
		if (bule->flags & IP6_MH_BU_ACK) {
			set_selector(&bule->hoa, &bule->peer_addr,
				     IPPROTO_MH, IP6_MH_TYPE_BACK,
				     0, 0, &sel);
			xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
		}
	}
	bule->xfrm_state = 0;
}

void xfrm_del_bule(struct bulentry *bule) 
{
	if (bule->xfrm_state & BUL_XFRM_STATE_DATA)
		_xfrm_del_bule_data(bule);
	_xfrm_del_bule_sig(bule);
}

/* before sending BU, MN should insert policy/state only for BU/BA */
int xfrm_pre_bu_add_bule(struct bulentry *bule)
{
	struct xfrm_selector sel;
	struct xfrm_user_tmpl tmpl;
	int rsig = bule->xfrm_state & BUL_XFRM_STATE_SIG;
	int rdata = bule->xfrm_state & BUL_XFRM_STATE_DATA;
	int prio;
	int exist = 0;

	if (bule->flags & IP6_MH_BU_HOME) {
		struct home_addr_info *hai = bule->home;
		if (hai->home_block & HOME_LINK_BLOCK)
			xfrm_unblock_link(hai);
		xfrm_block_link(hai);
		if (hai->mob_rtr && !(hai->home_block & NEMO_FWD_BLOCK))
			xfrm_block_fwd(hai);
	}
	if (IN6_ARE_ADDR_EQUAL(&bule->hoa, &bule->coa)) {
		if (rdata)
			return _xfrm_del_bule_data(bule);
		return 0;
	}
	bule->xfrm_state = BUL_XFRM_STATE_SIG|BUL_XFRM_STATE_DATA;

	if (bule->flags & IP6_MH_BU_HOME && conf.UseMnHaIPsec) {
		if (mn_ha_ipsec_pol_mod(&bule->peer_addr, &bule->hoa))
			return -1;
	}
	/* MN - CN/HA case, BU out */
	prio = (bule->flags & IP6_MH_BU_HOME ?
		MIP6_PRIO_HOME_SIG : MIP6_PRIO_RO_SIG);
	if (bule->flags & IP6_MH_BU_ACK) {
		create_rh_tmpl(&tmpl);
		set_selector(&bule->hoa, &bule->peer_addr, IPPROTO_MH,
			     IP6_MH_TYPE_BACK, 0, 0, &sel);
		if (xfrm_mip_policy_add(&sel, rsig, XFRM_POLICY_IN,
					XFRM_POLICY_ALLOW, prio, &tmpl, 1))
			return -1;
	}
	create_dstopt_tmpl(&tmpl, &bule->peer_addr, &bule->hoa);
	set_selector(&bule->peer_addr, &bule->hoa, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, 0, &sel);
	if (xfrm_mip_policy_add(&sel, rsig, XFRM_POLICY_OUT,
				XFRM_POLICY_ALLOW, prio, &tmpl, 1))
		return -1;

	if (!(bule->flags & IP6_MH_BU_HOME)) {
		struct bcentry *bce = bcache_get(&bule->hoa, &bule->peer_addr);
		if (bce) {
			if (bce->type >  BCE_NONCE_BLOCK &&
			    _xfrm_add_bce(&bule->hoa, &bule->peer_addr, 1)) {
				bcache_release_entry(bce);
				return -1;
			}
			bcache_release_entry(bce);
			exist = 1;
		}
	}
	if(!exist &&_mn_bule_ro_pol_add(bule, bule->home->if_tunnel, rdata))
		return -1;
	set_selector(&bule->peer_addr, &bule->hoa, 0, 0, 0, 0, &sel);
	/* XXX: acquired state is already inserted */
	if (!(bule->flags & IP6_MH_BU_HOME)) {
		XDBG2("%s: original rdata = %d\n", __FUNCTION__, rdata);
		rdata = 1;
	}
	return xfrm_state_add(&sel, IPPROTO_DSTOPTS, &bule->coa, rdata, 0);
}

/* After receiving BA, MN should update MIP policies for data traffic */
int xfrm_post_ba_mod_bule(struct bulentry *bule)
{
	struct xfrm_selector sel;
	struct xfrm_user_tmpl tmpls[2];
	int prio;
	int ret = 0;
	
	if (bule->flags & IP6_MH_BU_HOME) {
		struct home_addr_info *hai = bule->home;
		if (hai->home_block & HOME_LINK_BLOCK)
			xfrm_unblock_link(hai);
		if (hai->home_block & NEMO_FWD_BLOCK)
			xfrm_unblock_fwd(hai);
	}
	/* check if XFRM policies and states have already been cleaned up */
	if (IN6_ARE_ADDR_EQUAL(&bule->hoa, &bule->coa))
		return 0;

	if (!(bule->flags & IP6_MH_BU_HOME)) {
		struct bcentry *bce = bcache_get(&bule->hoa, &bule->peer_addr);
		if (bce) {
			if (bce->type >  BCE_NONCE_BLOCK &&
			    _xfrm_add_bule_bce(&bule->hoa,
					       &bule->peer_addr, 1)) {
				bcache_release_entry(bce);
				return -1;
			}
			bcache_release_entry(bce);
			/* return 0: not to overwrite policy (for MN-MN communication) */
			return 0;
		}
	}
	prio = (bule->flags & IP6_MH_BU_HOME ?
		MIP6_PRIO_HOME_DATA : MIP6_PRIO_RO_BULE_DATA);
	set_selector(&bule->peer_addr, &bule->hoa, 0, 0, 0,
		     bule->home->if_tunnel, &sel);
	create_dstopt_tmpl(&tmpls[0], &bule->peer_addr, &bule->hoa);
	ret = xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_OUT,
				   XFRM_POLICY_ALLOW, prio, tmpls, 1);
	if (ret)
		XDBG("failed to insert outbound policy\n");

	/* XXX: inbound is missed??? */
	create_rh_tmpl(&tmpls[0]);
	set_selector(&bule->hoa, &bule->peer_addr, 0, 0, 0,
		     0, &sel);
	ret = xfrm_mip_policy_add(&sel, 1, XFRM_POLICY_IN,
				   XFRM_POLICY_ALLOW, prio, tmpls, 1);
	if (ret)
		XDBG("failed to insert inbound policy\n");

	return ret;
}

static void parse_acquire(struct nlmsghdr *msg)
{
	struct xfrm_user_acquire *acq;
	int do_ro = 0;
	struct in6_addr *hoa = NULL, *cn = NULL;
	xfrm_address_t *daddr, *saddr;

	if (msg->nlmsg_len < NLMSG_LENGTH(sizeof(*acq))) {
		XDBG("Too short nlmsg");
		return;
	}
	acq = NLMSG_DATA(msg);
	daddr = &acq->sel.daddr;
	saddr = &acq->sel.saddr;

	/* First mandatory checks */
	if (acq->sel.proto == IPPROTO_MH ||
	    (acq->sel.proto == IPPROTO_ICMPV6 &&
	     ntohs(acq->sel.sport) < 128) ||
	    !in6_is_addr_routable_unicast((struct in6_addr *)daddr) ||
	    !in6_is_addr_routable_unicast((struct in6_addr *)saddr))
		return;

	if (acq->policy.dir == 	XFRM_POLICY_OUT) {
		XDBG2("xfrm_policy_out\n");
		if (acq->id.proto == IPPROTO_DSTOPTS) {
			hoa = (struct in6_addr *)acq->sel.saddr.a6;
			cn = (struct in6_addr *)acq->sel.daddr.a6; 
			do_ro = 1;
		} else {
			XDBG("Unknown protocol %d in acquire", acq->id.proto);
		}
	} else if (acq->policy.dir == XFRM_POLICY_IN) {
		XDBG2("xfrm_policy_in\n");
		if (acq->id.proto == IPPROTO_ROUTING) {
			hoa = (struct in6_addr *)acq->sel.daddr.a6; 
			cn = (struct in6_addr *)acq->sel.saddr.a6;
			do_ro = 1;
		} else {
			XDBG("Unknown protocol %d in acquire", acq->id.proto);
		}
	} else {
		XDBG2("xfrm parse acquire: ignoring forwarded packets");
	}
	XDBG2("Acquire daddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	      NIP6ADDR((struct in6_addr *)&acq->sel.daddr.a6));
	XDBG2("Acquire saddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	      NIP6ADDR((struct in6_addr *)&acq->sel.saddr.a6));
	XDBG2("ifindex %d\n", acq->sel.ifindex);
	if (do_ro)
		mn_start_ro(cn, hoa);
}

#define XFRMRPT_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_user_report))))

static int parse_report(struct nlmsghdr *msg)
{
	struct xfrm_user_report *rpt;
	struct rtattr *rta_tb[XFRMA_MAX+1];
	uint8_t status = IP6_MH_BES_UNKNOWN_HAO;
	xfrm_address_t *hoaddr = NULL;
	xfrm_address_t *cnaddr = NULL;
	xfrm_address_t *coaddr;

	if (msg->nlmsg_len < NLMSG_LENGTH(sizeof(*rpt))) {
		XDBG("Too short nlmsg");
		return -1;
	}
	rpt = NLMSG_DATA(msg);

	if (rpt->proto != IPPROTO_DSTOPTS ||
	    rpt->sel.family != AF_INET6)
		return 0;

	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, XFRMA_MAX, XFRMRPT_RTA(rpt), 
		     msg->nlmsg_len - NLMSG_LENGTH(sizeof(*rpt)));

	if (!rta_tb[XFRMA_COADDR])
		return -1;

	coaddr = (xfrm_address_t *) RTA_DATA(rta_tb[XFRMA_COADDR]);

	if (rpt->sel.proto == IPPROTO_MH && 
	    ntohs(rpt->sel.sport) > IP6_MH_TYPE_MAX)
		status = IP6_MH_BES_UNKNOWN_MH;

	hoaddr = &rpt->sel.saddr;
	cnaddr = &rpt->sel.daddr;

	XDBG("flow proto = %u\n", rpt->sel.proto);
	XDBG("hoaddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	     NIP6ADDR((struct in6_addr *)hoaddr));
	XDBG("coaddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	     NIP6ADDR((struct in6_addr *)coaddr));
	XDBG("cnaddr %x:%x:%x:%x:%x:%x:%x:%x\n",
	     NIP6ADDR((struct in6_addr *)cnaddr));
	XDBG("ifindex %d\n", rpt->sel.ifindex);
	XDBG("sending BE status = %u\n", status);

	/* ignore if CoA/HoA of original packet is invalid */
	if (!in6_is_addr_routable_unicast((struct in6_addr *) coaddr) ||
	    !in6_is_addr_routable_unicast((struct in6_addr *) hoaddr)) {
		XDBG("Invalid CoA/HoA appeared in original packet\n");
		return 0;
	}
	mh_send_be((struct in6_addr *)coaddr, (struct in6_addr *)hoaddr,
		   (struct in6_addr *)cnaddr, status, rpt->sel.ifindex);

	return 0;
}

static int xfrm_rcv(__attribute__ ((unused)) const struct sockaddr_nl *who,
		    struct nlmsghdr *n, __attribute__ ((unused)) void *arg)
{
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	switch (n->nlmsg_type) {
	case XFRM_MSG_ACQUIRE:
		/* Start RO or send BRR */
		if (is_mn())
			parse_acquire(n);
		break;
	case XFRM_MSG_REPORT:
		parse_report(n);
		break;
	}
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	return 0;
}

struct rtnl_handle xfrm_rth;

static void *xfrm_listen(__attribute__ ((unused)) void *dummy)
{
	pthread_dbg("thread started");
	rtnl_listen(&xfrm_rth, xfrm_rcv, NULL);
	pthread_exit(NULL);
}

int xfrm_init(void)
{
	int val;

	if (xfrm_cn_init() < 0 ||
	    (is_ha() && xfrm_ha_init() < 0) ||
	    (is_mn() && xfrm_mn_init() < 0) ||
	    rtnl_xfrm_open(&xfrm_rth, 0) < 0)
		goto error;

	val = XFRMNLGRP_REPORT;
	if (setsockopt(xfrm_rth.fd, SOL_NETLINK,
		       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0)
		goto error;
	if (is_mn()) {
		val = XFRMNLGRP_ACQUIRE;
		if (setsockopt(xfrm_rth.fd, SOL_NETLINK,
			       NETLINK_ADD_MEMBERSHIP, &val, sizeof(val)) < 0)
			goto error;
	}
	/* create netlink listener thread */
	if (pthread_create(&xfrm_listener, NULL, xfrm_listen, NULL))
		goto error;
	return 0;
error:
	if (is_mn())
		xfrm_mn_cleanup();
	if (is_ha())
		xfrm_ha_cleanup();
	xfrm_cn_cleanup();
	return -1;
}

void xfrm_cleanup(void)
{		
	rtnl_close(&xfrm_rth);
	pthread_cancel(xfrm_listener);
	pthread_join(xfrm_listener, NULL);

	if (is_mn())
		xfrm_mn_cleanup();
	if (is_ha())
		xfrm_ha_cleanup();
	xfrm_cn_cleanup();
}

/* blocking all payload packets from MN */
int xfrm_block_link(struct home_addr_info *hai)
{
	int ret = 0;
	struct xfrm_selector sel;
	hai->home_block |= HOME_LINK_BLOCK;
	hai->if_block = hai->hoa.iif;

	/* block any packets from HoA to the CN */
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, hai->if_block, &sel);
	if ((ret = xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT,
				       XFRM_POLICY_BLOCK,
				       MIP6_PRIO_HOME_BLOCK, NULL, 0)))
		return ret;
	return ret;
}

void xfrm_unblock_link(struct home_addr_info *hai)
{
	struct xfrm_selector sel;
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, hai->if_block, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);

	hai->if_block = 0;
	hai->home_block &= ~HOME_LINK_BLOCK;
}
/*
 * while searching HA, MN should not use HoA
 */
int xfrm_block_hoa(struct home_addr_info *hai)
{
	struct xfrm_selector sel;
	hai->home_block |= HOME_ADDR_BLOCK;
	hai->ha_list.if_block = hai->hoa.iif;
	set_selector(&in6addr_any, &hai->hoa.addr,
		     0, 0, 0, hai->ha_list.if_block, &sel);
	if (xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT,
				XFRM_POLICY_BLOCK, MIP6_PRIO_HOME_ERROR, NULL, 0))
		return -1;
	return 0;
}

void xfrm_unblock_hoa(struct home_addr_info *hai)
{
	struct xfrm_selector sel;
	set_selector(&in6addr_any, &hai->hoa.addr,
		     0, 0, 0, hai->ha_list.if_block, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
	hai->ha_list.if_block = 0;
	hai->home_block &= ~HOME_ADDR_BLOCK;
}

/* block all RA messages sent by MR */
int xfrm_block_ra(struct home_addr_info *hai)
{
	int ret = 0;
	struct xfrm_selector sel;
	hai->home_block |= NEMO_RA_BLOCK;
	set_selector(&in6addr_any, &in6addr_any, IPPROTO_ICMPV6,
		     ND_ROUTER_ADVERT, 0, 0, &sel);
	if ((ret = xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_OUT, XFRM_POLICY_BLOCK,
				   MIP6_PRIO_HOME_BLOCK, NULL, 0)))
		return ret;
	return ret;
}

void xfrm_unblock_ra(struct home_addr_info *hai)
{
	struct xfrm_selector sel;
	set_selector(&in6addr_any, &in6addr_any, IPPROTO_ICMPV6,
		     ND_ROUTER_ADVERT, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_OUT);
	hai->home_block &= ~NEMO_RA_BLOCK;
}

/* block all forwarded packets */
int xfrm_block_fwd(struct home_addr_info *hai)
{
	int ret = 0;
	struct xfrm_selector sel;
	hai->home_block |= NEMO_FWD_BLOCK;
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	if ((ret = xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_FWD, XFRM_POLICY_BLOCK,
				   MIP6_PRIO_HOME_BLOCK, NULL, 0)))
		return ret;
	return ret;
}

void xfrm_unblock_fwd(struct home_addr_info *hai)
{
	struct xfrm_selector sel;
	set_selector(&in6addr_any, &in6addr_any, 0, 0, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_FWD);
	hai->home_block &= ~NEMO_FWD_BLOCK;
}

int mn_ipsec_recv_bu_tnl_pol_add(struct bulentry *bule, int ifindex, 
				 struct ipsec_policy_entry *e)
{
	struct xfrm_selector sel;
	struct xfrm_user_tmpl tmpls[MIPV6_MAX_TMPLS];
	int ti = 0;

	set_selector(&bule->hoa, &in6addr_any, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, ifindex, &sel);

	if (ipsec_use_ipcomp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_COMP, XFRM_MODE_TUNNEL,
				  &bule->hoa, &bule->home->ha_addr,
				  e->reqid_tomn);
	if (ipsec_use_esp(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_ESP, XFRM_MODE_TUNNEL,
				  &bule->hoa, &bule->home->ha_addr,
				  e->reqid_tomn);
	if (ipsec_use_ah(e))
		create_ipsec_tmpl(&tmpls[ti++], IPPROTO_AH, XFRM_MODE_TUNNEL,
				  &bule->hoa, &bule->home->ha_addr,
				  e->reqid_tomn);
	return xfrm_ipsec_policy_add(&sel, 1, XFRM_POLICY_IN, e->action,
				     MIP6_PRIO_RO_SIG_IPSEC, tmpls, ti);
}

void mn_ipsec_recv_bu_tnl_pol_del(struct bulentry *bule, int ifindex)
{
	struct xfrm_selector sel;
	memset(&sel, 0, sizeof(sel));
	set_selector(&bule->hoa, &in6addr_any, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, ifindex, &sel);
	xfrm_ipsec_policy_del(&sel, XFRM_POLICY_IN);
}

int cn_wildrecv_bu_pol_add(void)
{
	struct xfrm_selector sel;
	struct xfrm_user_tmpl tmpl;
	set_selector(&in6addr_any, &in6addr_any, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, 0, &sel);
	create_dstopt_tmpl(&tmpl, &in6addr_any, &in6addr_any);

	return xfrm_mip_policy_add(&sel, 0, XFRM_POLICY_IN, XFRM_POLICY_ALLOW,
				   MIP6_PRIO_RO_SIG_ANY, &tmpl, 1);
}

void cn_wildrecv_bu_pol_del(void)
{
	struct xfrm_selector sel;
	set_selector(&in6addr_any, &in6addr_any, IPPROTO_MH,
		     IP6_MH_TYPE_BU, 0, 0, &sel);
	xfrm_mip_policy_del(&sel, XFRM_POLICY_IN);
}
