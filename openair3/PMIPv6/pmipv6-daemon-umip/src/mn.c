/*
 * $Id: mn.c 1.380 06/05/15 18:34:56+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors:
 *  Ville Nuorvala	<vnuorval@tcs.hut.fi>
 *  Antti Tuominen	<anttit@tcs.hut.fi>
 *  Henrik Petander	<petander@tcs.hut.fi>
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
#include <time.h>
#include <unistd.h> 

#include <netinet/icmp6.h>
#include <netinet/ip6mh.h>
#include <netinet/in.h>
#include <netinet/ip6.h>

#include <linux/fib_rules.h>

#include "debug.h"
#include "mipv6.h"
#include "icmp6.h"
#include "ndisc.h"
#include "mpdisc_mn.h"
#include "mh.h"
#include "mn.h"
#include "cn.h"
#include "movement.h"
#include "util.h"
#include "list.h"
#include "bul.h"
#include "xfrm.h"
#include "tunnelctl.h"
#include "rtnl.h"
#include "conf.h"
#include "prefix.h"
#include "retrout.h"
#include "keygen.h"
#include "dhaad_mn.h"
#include "ipsec.h"
#include "statistics.h"

#define MN_DEBUG_LEVEL 1

#if MN_DEBUG_LEVEL >= 1
#define MDBG dbg
#else
#define MDBG(...)
#endif

#if MN_DEBUG_LEVEL >= 2
#define MDBG2 dbg
#else
#define MDBG2(...)
#endif

#ifndef FIB_RULE_FIND_SADDR
#define	FIB_RULE_FIND_SADDR	0
#endif

LIST_HEAD(home_addr_list); 

pthread_rwlock_t mn_lock;

const struct timespec dereg_bu_lifetime_ts =
{ DEREG_BU_LIFETIME, 0 };
const struct timespec non_mip_cn_ltime_ts =
{ NON_MIP_CN_LTIME, 0 };
const struct timespec min_valid_bu_lifetime_ts =
{ MIN_VALID_BU_LIFETIME, 0 };

static int pending_bas = 0;

static void mn_send_home_bu(struct home_addr_info *hai);
static int mn_ext_tunnel_ops(int request, int old_if, int new_if, void *data);

static int mn_block_rule_del(struct home_addr_info *hai)
{
	int ret = -1;

	if (!(hai->home_block & HOME_ADDR_RULE_BLOCK)) {
		MDBG("blackhole is not set.\n");
		return ret;
	}

	if ((ret = rule_del(NULL, 0, IP6_RULE_PRIO_MIP6_BLOCK, RTN_BLACKHOLE,
		     &hai->hoa.addr, 128, &in6addr_any, 0,
		     FIB_RULE_FIND_SADDR)) < 0)
		MDBG("failed to delete blackhole rule.\n");
	else
		hai->home_block &= ~HOME_ADDR_RULE_BLOCK;

	return ret;
}

static int mn_block_rule_add(struct home_addr_info *hai)
{
	int ret = -1;

	if (hai->home_block & HOME_ADDR_RULE_BLOCK) {
		MDBG("blackhole is already set.\n");
		return ret;
	}
	if ((ret = rule_add(NULL, 0, IP6_RULE_PRIO_MIP6_BLOCK, RTN_BLACKHOLE,
		     &hai->hoa.addr, 128, &in6addr_any, 0,
		     FIB_RULE_FIND_SADDR)) < 0)
		MDBG("failed to add blackhole rule.\n");
	else
		hai->home_block |= HOME_ADDR_RULE_BLOCK;

	return ret;
}

static void bul_expire(struct tq_elem *tqe)
{
	pthread_rwlock_wrlock(&mn_lock);
	if (!task_interrupted()) {
		struct bulentry *bule = tq_data(tqe, struct bulentry, tqe);
		/* Set lifetime to 0 to help bul_delete */
		tsclear(bule->lifetime);
		MDBG("Bul expire type %d", bule->type);
		bul_delete(bule);
	}
	pthread_rwlock_unlock(&mn_lock);
}

static void bule_invalidate(struct bulentry *e,
			    struct timespec *timestamp,
			    int block)
{
	struct home_addr_info *hai = e->home;
	struct in6_addr hoa, peer_addr;
	int type = e->type;
	uint16_t flags = e->flags;

	if (type != BUL_ENTRY) 
		return;

	MDBG("%s unable to handle binding, changing BUL entry to %s\n",
	     (e->flags & IP6_MH_BU_HOME ? "HA" : "CN"),
	     (block ? "NON_MIP_CN_ENTRY" : "UNREACH_ENTRY"));

	hoa = e->hoa;
	peer_addr = e->peer_addr;

	bul_delete(e);

	if (hai->at_home)
		return;

	e = create_bule(&hoa, &peer_addr);

	if (e == NULL)
		return;

	e->type = block ? NON_MIP_CN_ENTRY : UNREACH_ENTRY;
	e->flags = flags;
	e->home = hai;
	e->lastsent = *timestamp;
	e->lifetime = NON_MIP_CN_LTIME_TS;
	e->delay = NON_MIP_CN_LTIME_TS;
	e->callback = bul_expire;
	if (bul_add(e) < 0)
		bul_delete(e);
}

static void mn_reset_ro_bule(struct bulentry *e)
{
	e->callback = bul_expire;
	e->lifetime = MAX_TOKEN_LIFETIME_TS;
	e->delay = MAX_TOKEN_LIFETIME_TS;
	e->do_send_bu = 1;
}

static void mn_rr_check_entry(struct tq_elem *tqe)
{
	struct bulentry *bule;
	struct timespec now;
	long last_used;

	pthread_rwlock_wrlock(&mn_lock);

	if (task_interrupted()) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	bule = tq_data(tqe, struct bulentry, tqe);

	clock_gettime(CLOCK_REALTIME, &now);

	last_used = mn_bule_xfrm_last_used(&bule->peer_addr, &bule->hoa, &now);

	if (last_used >= 0 && last_used < MN_RO_RESTART_THRESHOLD) {
		bule->lastsent = now;
		mn_reset_ro_bule(bule);
		pre_bu_bul_update(bule);
		mn_rr_refresh(bule);
		if (bule->rr.state == RR_READY)
			mn_send_cn_bu(bule);
		else {
			bul_update_expire(bule);
			bul_update_timer(bule);
		}
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	bul_delete(bule);
	pthread_rwlock_unlock(&mn_lock);
}

static void mn_recv_param_prob(const struct icmp6_hdr *ih, ssize_t len,
			       __attribute__ ((unused)) const struct in6_addr *src,
			       const struct in6_addr *dst,
			       __attribute__ ((unused)) int iif,
			       __attribute__ ((unused)) int hoplimit)
{
	struct ip6_hdr *ip6h = (struct ip6_hdr *)(ih + 1);
	int optlen = len - sizeof(struct icmp6_hdr);
	uint32_t errptr;
	uint8_t *off_octet;
	struct bulentry *e = NULL;
	struct in6_addr *laddr = &ip6h->ip6_src;
	struct in6_addr *raddr = &ip6h->ip6_dst;
	struct in6_addr addr;

	/* We only handle code 1 & 2 messages. */
	if (ih->icmp6_code != ICMP6_PARAMPROB_NEXTHEADER &&
	    ih->icmp6_code != ICMP6_PARAMPROB_OPTION)
		return;

	/* Find offending octet in the original packet. */
	errptr = ntohl(ih->icmp6_pptr);

	/* Validity checks */
	if (len < 0 ||
	    (uint32_t)len <= errptr || !IN6_ARE_ADDR_EQUAL(laddr, dst) ||
	    icmp6_parse_data(ip6h, optlen, &laddr, &raddr) < 0)
		return;

	off_octet = ((uint8_t *) ip6h + errptr);
	MDBG("Parameter problem: offending octet %d [0x%2x]\n",
	      errptr, *off_octet);

	/* If CN did not understand Mobility Header, set BUL entry to
	 * ACK_ERROR so no further BUs are sumbitted to this CN. */
	if (ih->icmp6_code == ICMP6_PARAMPROB_NEXTHEADER && 
	    *off_octet == IPPROTO_MH) {
		MDBG("CN doesn't implement MH handling.\n");
		statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_X_MH_PARAM);
	} else if (ih->icmp6_code == ICMP6_PARAMPROB_OPTION &&
		  *off_octet == IP6OPT_HOME_ADDRESS) {
		MDBG("CN doesn't implement Home Address Option processing\n");
		statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_X_HAO_PARAM);
	} else {
		MDBG("Got ICMPv6 paramprob not resulting from HAO or MH\n");
		return;
	}
	pthread_rwlock_wrlock(&mn_lock);
	if (mn_rr_error_check(laddr, raddr, &addr))
		laddr = &addr;
	e = bul_get(NULL, laddr, raddr);
	if (e != NULL) { 
		if (e->flags & IP6_MH_BU_HOME) {
			if (!conf.UseMnHaIPsec &&
			    !conf.MnDiscardHaParamProb) {
				clock_gettime(CLOCK_REALTIME, &e->lastsent);
				bule_invalidate(e, &e->lastsent, 1);
			}
		} else if (ih->icmp6_code == ICMP6_PARAMPROB_OPTION) {
			syslog(LOG_ERR,
			       "CN participated in RO but can't handle HAO\n");
		} else {
			clock_gettime(CLOCK_REALTIME, &e->lastsent);
			bule_invalidate(e, &e->lastsent, 1);
		}
	}
	pthread_rwlock_unlock(&mn_lock);
}

static struct icmp6_handler mn_param_prob_handler = {
	.recv = mn_recv_param_prob,
};

static int mn_send_bu_msg(struct bulentry *bule)
{
	struct ip6_mh_binding_update *bu;
	
	struct iovec iov[IP6_MHOPT_MAX+1];
	int iov_ind = 0;
	int ret = -ENOMEM;
	uint8_t *bind_key = NULL;
	struct in6_addr_bundle addrs;

	memset(iov, 0, IP6_MHOPT_MAX+1);
	bu = mh_create(&iov[iov_ind++], IP6_MH_TYPE_BU);
	if (!bu)
		return -ENOMEM;

	bu->ip6mhbu_seqno = htons(bule->seq);
	bu->ip6mhbu_flags = bule->flags;
	bu->ip6mhbu_lifetime = htons(bule->lifetime.tv_sec >> 2);

	if (bule->use_alt_coa && 
	    mh_create_opt_altcoa(&iov[iov_ind++], &bule->coa)) {
		free_iov_data(iov, iov_ind);
		return -ENOMEM;
	}
	if (bule->flags & IP6_MH_BU_HOME) {
		struct home_addr_info *hai = bule->home;
		if (bule->flags & IP6_MH_BU_MR && bu->ip6mhbu_lifetime &&
		    bule->home->mnp_count > 0 && conf.MobRtrUseExplicitMode &&
		    mh_create_opt_mob_net_prefix(&iov[iov_ind++],
						 hai->mnp_count,
						 &hai->mob_net_prefixes) < 0) {
			free_iov_data(iov, iov_ind);
			return -ENOMEM;
		}
	} else {
		if (mh_create_opt_nonce_index(&iov[iov_ind++], bule->rr.ho_ni,
					      bule->rr.co_ni) ||
		    mh_create_opt_auth_data(&iov[iov_ind++])) {
			free_iov_data(iov, iov_ind);
			return -ENOMEM;
		} 
		bind_key = bule->Kbm;
	}
	if (bule->flags & IP6_MH_BU_ACK)
		bule->wait_ack = 1;
	addrs.src = &bule->hoa;
	addrs.dst = &bule->peer_addr;
	addrs.local_coa = &bule->coa;
	addrs.remote_coa = NULL;
	addrs.bind_coa = &bule->coa;

	ret = mh_send(&addrs, iov, iov_ind, bind_key, bule->if_coa);

	if (ret <= 0)
		MDBG("mh_send failed  ret: %d\n", ret);

	free_iov_data(iov, iov_ind);
	statistics_inc(&mipl_stat, MIPL_STATISTICS_OUT_BU);
	
	return ret;
}

static int mn_get_home_lifetime(struct home_addr_info *hai,
				struct timespec *lifetime, int dereg)
{
	if (!hai->at_home && !dereg) {
		struct mn_addr *coa = &hai->primary_coa;
		struct timespec now;
		unsigned long coa_lft;
		unsigned long hoa_lft;

		clock_gettime(CLOCK_REALTIME, &now);		

		coa_lft = mpd_curr_lft(&now, &coa->timestamp,
				       coa->valid_time.tv_sec);
		hoa_lft = mpd_curr_lft(&now, &hai->hoa.timestamp,
				       hai->hoa.valid_time.tv_sec);

		tssetsec(*lifetime, umin(umin(coa_lft, hoa_lft),
					 conf.MnMaxHaBindingLife));

		mpd_sanitize_lft(lifetime);

		MDBG("CoA lifetime %u s, HoA lifetime %u s, BU lifetime %u s\n",
		     coa_lft, hoa_lft, lifetime->tv_sec);

		return 0;
	} else {
		tsclear(*lifetime);
		MDBG("BU lifetime %u s\n", lifetime->tv_sec);
		return 1;
	}
}

static int mn_get_ro_lifetime(struct home_addr_info *hai,
			      struct timespec *lifetime, int dereg)
{
	tsclear(*lifetime);

	if (!hai->at_home && !dereg) {
		struct bulentry *e;

		e = bul_get(hai, NULL, &hai->ha_addr);

		if (e == NULL || !(e->flags & IP6_MH_BU_HOME)) {
			MDBG("No valid home registration");
		} else {
			struct timespec now;
			struct mn_addr *coa = &hai->primary_coa;
			unsigned long coa_lft;
			unsigned long home_lft;

			clock_gettime(CLOCK_REALTIME, &now);		

			coa_lft = mpd_curr_lft(&now, &coa->timestamp,
					       coa->valid_time.tv_sec);

			home_lft = mpd_curr_lft(&now, &e->lastsent,
						e->lifetime.tv_sec);

			tssetsec(*lifetime, umin(umin(coa_lft, home_lft),
						 conf.MnMaxCnBindingLife));

			mpd_sanitize_lft(lifetime);
		}
		return 0;
	}
	MDBG("BU lifetime %u s\n", lifetime->tv_sec);
	return 1;
}

static int mn_dereg(void *vbule, __attribute__ ((unused)) void *arg)
{
	struct bulentry *bule = vbule;

	if (bule->type == BUL_ENTRY) {
		if (!(bule->flags & IP6_MH_BU_HOME)) {
			bule->dereg = 1;
			mn_rr_refresh(bule);
			if (bule->rr.state != RR_READY)
				return 0;
		}
		bule->seq++;
		tsclear(bule->lifetime);
		mn_send_bu_msg(bule);
	}
	bul_delete(bule);
	return 0;
}

#define FLUSH_ALL 0 
#define FLUSH_VALID 1 
#define FLUSH_FAILED 2 

static int _bul_flush(void *vbule, void *arg)
{
	struct bulentry *bule = vbule;
	int *type = arg;
	if (*type == FLUSH_ALL || 
	    (*type == FLUSH_VALID && bule->type == BUL_ENTRY) ||
	    (*type == FLUSH_FAILED && bule->type != BUL_ENTRY))
		bul_delete(bule);
	return 0;
}

static void mn_change_ha(struct home_addr_info *hai)
{
	int dhaad = dhaad_home_reg_failed(hai);
	int type = FLUSH_VALID;
	if (hai->home_reg_status != HOME_REG_NONE)
		bul_iterate(&hai->bul, _bul_flush, &type);
	syslog(LOG_ERR,
	       "Home registration failed with HA, trying next one\n");
	if (!dhaad && !hai->at_home)
		mn_send_home_bu(hai);
}

static int bu_lft_check(struct bulentry *bule)
{
	if (!tsisset(bule->lifetime))
		return 0;

	int dereg;

	if (bule->flags & IP6_MH_BU_HOME)
		dereg = mn_get_home_lifetime(bule->home, &bule->lifetime, 0);
	else
		dereg = mn_get_ro_lifetime(bule->home, &bule->lifetime,
					   bule->dereg);

	return !dereg && !tsisset(bule->lifetime);
}

/*
 * bu_resend - resend a binding update
 */
static void bu_resend(struct tq_elem *tqe)
{      
	pthread_rwlock_wrlock(&mn_lock);
	if (!task_interrupted()) {
		struct bulentry *bule = tq_data(tqe, struct bulentry, tqe);
		struct home_addr_info *hai = bule->home;
		int expired;

		MDBG("Bul resend [%p] type %d\n",  bule, bule->type);

		clock_gettime(CLOCK_REALTIME, &bule->lastsent);
		tsadd(bule->delay, bule->delay, bule->delay);
		bule->delay = tsmin(bule->delay, MAX_BINDACK_TIMEOUT_TS);
		bule->consecutive_resends++;

		expired = bu_lft_check(bule);

		bule->seq++;

		if (bule->flags & IP6_MH_BU_HOME &&
		    hai->use_dhaad && 
		    bule->consecutive_resends > MAX_CONSECUTIVE_RESENDS) {
			struct timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			bule_invalidate(bule, &now, 0);
			mn_change_ha(hai);
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
		mn_send_bu_msg(bule);

		if (expired)
			bul_delete(bule);
		else {
			bul_update_expire(bule);
			bul_update_timer(bule);
		}
	}
	pthread_rwlock_unlock(&mn_lock);
}

static void bu_refresh(struct tq_elem *tqe)
{
	pthread_rwlock_wrlock(&mn_lock);
	if (!task_interrupted()) {
		struct bulentry *bule = tq_data(tqe, struct bulentry, tqe);
		int expired;
		MDBG("Bul refresh type: %d\n", bule->type);

		clock_gettime(CLOCK_REALTIME, &bule->lastsent);

		bule->delay = conf.InitialBindackTimeoutReReg_ts;
	
		expired = bu_lft_check(bule);

		bule->seq++;
		bule->callback = bu_resend;
		pre_bu_bul_update(bule);
		mn_send_bu_msg(bule);

		if (expired)
			bul_delete(bule);
		else {
			bul_update_expire(bule);
			bul_update_timer(bule);
			if (conf.OptimisticHandoff)
				post_ba_bul_update(bule);	
		}
	}
	pthread_rwlock_unlock(&mn_lock);
}

static void mn_update_hoa_lifetime(struct mn_addr *mn_hoa,
				   struct timespec *timestamp,
				   uint32_t valid_time,
				   uint32_t preferred_time)
{
	mn_hoa->timestamp = *timestamp;
	tssetsec(mn_hoa->valid_time, valid_time);
	tssetsec(mn_hoa->preferred_time, preferred_time);
	MDBG2("updated HoA lifetimes: preferred %u s and valid %u s\n", 
	      mn_hoa->preferred_time.tv_sec, mn_hoa->valid_time.tv_sec);
}

struct mv_hoa_args {
	struct home_addr_info *target;
	int if_next;
};

/*
 * Move home address between 
 */
static int mv_hoa(struct ifaddrmsg *ifa, struct rtattr *rta_tb[], void *arg)
{
	struct mv_hoa_args *mha = arg;
	struct home_addr_info *hai = mha->target;
	struct mn_addr *hoa = &hai->hoa;
	int err;
	struct timespec now;
	uint32_t preferred = PREFIX_LIFETIME_INFINITE;
	uint32_t valid = PREFIX_LIFETIME_INFINITE;
	int plen = (mha->if_next == hai->if_tunnel ? 128 : hai->plen);

	clock_gettime(CLOCK_REALTIME, &now);

	if (hai->lladdr_comp && rta_tb[IFA_CACHEINFO] != NULL) {
		struct ifa_cacheinfo *ci = RTA_DATA(rta_tb[IFA_CACHEINFO]);
		mn_update_hoa_lifetime(hoa, &now, 
				       ci->ifa_valid, ci->ifa_prefered);
		valid = ci->ifa_valid;
		preferred = ci->ifa_prefered;
	}

	if (mha->if_next >= 0 && (unsigned int)mha->if_next == ifa->ifa_index)
		return 0;

	MDBG("move HoA %x:%x:%x:%x:%x:%x:%x:%x/%d from iface %d to %d\n",
	     NIP6ADDR(&hoa->addr), plen, ifa->ifa_index, mha->if_next);

	err = addr_add(&hoa->addr, plen,
		       ifa->ifa_flags|IFA_F_HOMEADDRESS_NODAD,
		       ifa->ifa_scope, mha->if_next, preferred, valid);
	if (err < 0)
		return err;

	hoa->iif = mha->if_next;
	addr_del(&hoa->addr, ifa->ifa_prefixlen, ifa->ifa_index);
	return 0;
}

int nemo_mr_tnl_routes_add(struct home_addr_info *hai, int ifindex)
{
	struct list_head *l;
	struct prefix_list_entry *pe;
	list_for_each(l, &hai->mob_net_prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(l, struct prefix_list_entry, list);
		if (route_add(ifindex, RT6_TABLE_MIP6, RTPROT_MIP,
			      0, IP6_RT_PRIO_MIP6_FWD,
			      &p->ple_prefix, p->ple_plen,
			      &in6addr_any, 0, NULL) < 0) {
			pe = p;
			goto undo;
		}
	}
	return 0;
undo:
	list_for_each(l, &hai->mob_net_prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(l, struct prefix_list_entry, list);
		route_del(ifindex, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_FWD,
			  &p->ple_prefix, p->ple_plen, &in6addr_any, 0, NULL);
		if (p == pe)
			break;
	}
	return -1;
}

static int mn_tnl_state_add(struct home_addr_info *hai, int ifindex, int all)
{
	int err = 0;
	if (hai->home_reg_status != HOME_REG_NONE) {
		if ((err = mn_ro_pol_add(hai, ifindex, all)) < 0)
			return err;
		if ((err = route_add(ifindex, RT6_TABLE_MIP6, RTPROT_MIP, 0,
				     IP6_RT_PRIO_MIP6_OUT, &in6addr_any, 0,
				     &in6addr_any, 0, NULL)) < 0) {
			mn_ro_pol_del(hai, ifindex, all);
		}
	}
	if (hai->mob_rtr &&
	    (err = nemo_mr_tnl_routes_add(hai, ifindex)) < 0) {
		route_del(ifindex, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_OUT,
			  &hai->hoa.addr, 128, &in6addr_any, 0, NULL);
		mn_ro_pol_del(hai, ifindex, all);
	}
	return err;
}

static void nemo_mr_tnl_routes_del(struct home_addr_info *hai, int ifindex)
{
	struct list_head *l;
	list_for_each(l, &hai->mob_net_prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(l, struct prefix_list_entry, list);
		route_del(ifindex, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_FWD,
			  &p->ple_prefix, p->ple_plen, &in6addr_any, 0, NULL);
	}
}

static void mn_tnl_state_del(struct home_addr_info *hai, int ifindex, int all)
{
	if (hai->home_reg_status != HOME_REG_NONE) {
		if (hai->mob_rtr)
			nemo_mr_tnl_routes_del(hai, ifindex);
		route_del(ifindex, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_OUT, 
			  &hai->hoa.addr, 128, &in6addr_any, 0, NULL);
		mn_ro_pol_del(hai, ifindex, all);
	}
}


static void mn_home_ext_cleanup(struct bulentry *bule)
{
	struct home_addr_info *hai = bule->home; 

	hai->home_reg_status = HOME_REG_NONE;

	if (hai->pend_ba) {
		hai->pend_ba = 0;
		pending_bas--;
	}
	bule->ext_cleanup = NULL;
}

/* Clean up policy route for HoA when bulentry is deleted. */
static void mn_pol_ext_cleanup(struct bulentry *bule)
{
	MDBG("\n");
	mpd_cancel_mps(&bule->hoa, &bule->peer_addr);
	mn_tnl_state_del(bule->home, bule->home->if_tunnel, 0);

	if (conf.UseMnHaIPsec) {
		mn_ipsec_tnl_update(&bule->peer_addr, &bule->hoa, bule);
		mn_ipsec_tnl_pol_del(&bule->peer_addr, &bule->hoa, bule);
	}
	bule->ext_cleanup = mn_home_ext_cleanup;
}

static int process_first_home_bu(struct bulentry *bule,
				 struct home_addr_info *hai,
				 struct timespec *lifetime)
{
	int err = 0;
	bule->type = BUL_ENTRY;
	bule->flags = (IP6_MH_BU_HOME | IP6_MH_BU_ACK |
		       hai->lladdr_comp | hai->mob_rtr);
	if (conf.UseMnHaIPsec && conf.KeyMngMobCapability)
		bule->flags |= IP6_MH_BU_KEYM;
	bule->coa_changed = -1;
	bule->coa = hai->primary_coa.addr;
	bule->if_coa = hai->primary_coa.iif;
	bule->lifetime = *lifetime;
	bule->delay = conf.InitialBindackTimeoutFirstReg_ts;
	bule->callback = bu_resend;
	/* Use alt. coa with IPsec */
	bule->use_alt_coa = 1;
	bule->ext_cleanup = mn_pol_ext_cleanup;
	bule->home = hai;
	bule->consecutive_resends = 0;

	hai->home_reg_status = HOME_REG_UNCERTAIN;

	if ((err = mn_tnl_state_add(hai, hai->if_tunnel, 0)) < 0)
		MDBG("Failed to initialize new bule for HA\n");
	else
		MDBG("New bule for HA\n");

	return err;
}

static int mn_do_dad(struct home_addr_info *hai, int dereg);
static void mn_send_home_na(struct home_addr_info *hai);

static int mn_bu_ratelimit(struct bulentry *e)
{
	if (IN6_ARE_ADDR_EQUAL(&e->coa, &e->last_coa)) {
		if (e->wait_ack) {
			MDBG("Rate limit BU\n");
			return 1;
		}
	} else {
		e->wait_ack = 0;
		e->consecutive_resends = 0;
		e->coa_changed = 1;
	}
	return 0;
}

static void mn_send_home_bu(struct home_addr_info *hai)
{
	struct bulentry *bule = NULL;
	struct timespec lifetime;
	int homereg_expired = 0;
	movement_t type_movement = MIP6_TYPE_MOVEMENT_UNKNOWN;

	TRACE;

	if (IN6_IS_ADDR_UNSPECIFIED(&hai->ha_addr)) {
		MDBG("HA not set for home link\n");
		return;
	}
	mn_get_home_lifetime(hai, &lifetime, 0);

	if ((bule = bul_get(hai, NULL, &hai->ha_addr)) == NULL) {
		assert(!hai->at_home);
		/* Create new bul entry for HA */
		if (!tsisset(lifetime)) {
			dbg("HoA or CoA expired, unable to create binding.\n");
			return;
		}
		if (conf.UseMnHaIPsec &&
		    !ipsec_policy_entry_check(&hai->ha_addr, &hai->hoa.addr,
					      IPSEC_F_MH_BUBA)) {
			syslog(LOG_ERR,
			       "MN lacks the necessary IPsec policies for "
			       "protecting the home registration to HA "
			       "%x:%x:%x:%x:%x:%x:%x:%x\n",
			       NIP6ADDR(&hai->ha_addr));
			if (hai->use_dhaad)
				mn_change_ha(hai);
			return;
		}
		bule = create_bule(&hai->hoa.addr, &hai->ha_addr);
		if (bule == NULL)
			return;
		if (process_first_home_bu(bule, hai, &lifetime) < 0 ||
		    bul_add(bule) < 0) {
			bul_delete(bule);
			return;
		}
		type_movement = MIP6_TYPE_MOVEMENT_HL2FL;
		MDBG("New bule for HA\n");
	} else if (bule->type == BUL_ENTRY) {
		/* Update bule */
		clock_gettime(CLOCK_REALTIME, &bule->lastsent);

		if (IN6_ARE_ADDR_EQUAL(&bule->hoa, &bule->coa)) {
			if (process_first_home_bu(bule, 
						  hai, &lifetime) < 0) {
				bul_delete(bule);
				return;
			}
			type_movement = MIP6_TYPE_MOVEMENT_HL2FL;
		} else {
			bule->coa = hai->primary_coa.addr;
			bule->if_coa = hai->primary_coa.iif;

			/* Rate limiting home registration binding updates
			   is necessary for multihomed MNs */
			if (mn_bu_ratelimit(bule))
				return;
			if (!hai->at_home) {
				MDBG("Moved to foreign network\n");
				bule->lifetime = lifetime;
				type_movement = MIP6_TYPE_MOVEMENT_FL2FL;
				bul_iterate(&hai->bul, mn_bule_ro_pol_del, NULL);
			} else {
				MDBG("Moved to home network\n");
				type_movement = MIP6_TYPE_MOVEMENT_FL2HL;
				tsclear(bule->lifetime);
				mn_pol_ext_cleanup(bule);
			}
			bule->delay = conf.InitialBindackTimeoutReReg_ts;
			bule->callback = bu_resend;
		}
		bule->seq++;
		pre_bu_bul_update(bule);
		MDBG("Bule for HA exists. Updating it.\n");
	} else {
		MDBG("HA doesn't accept BU\n");
		return;
	}
	if (type_movement == MIP6_TYPE_MOVEMENT_FL2HL) {
		if (tsisset(bule->hard_expire) &&
		    tsafter(bule->hard_expire, bule->lastsent)) {
			/* If MN returns to home link after the expiration of
			 * home registration, MN does not send de-reg BU to HA 
			 */
			homereg_expired = 1;
		}
	}
	if (conf.UseMnHaIPsec) {
        	/* create SP entry for protecting RR signals */
		if (type_movement == MIP6_TYPE_MOVEMENT_HL2FL) {
			mn_ipsec_tnl_pol_add(&bule->home->ha_addr,
					     &bule->hoa, bule);
	        }
		/* migrate tunnel endpoint and update transport mode SA */
		if (bule->coa_changed) {
			/* Always update transport mode (for signaling) */
			mn_ipsec_trns_update(&bule->home->ha_addr,
					     &bule->hoa, bule);
			/* No need to update tunnel when back home */
			if (type_movement != MIP6_TYPE_MOVEMENT_FL2HL)
				mn_ipsec_tnl_update(&bule->home->ha_addr,
						    &bule->hoa, bule);
		}
        }
	if (!homereg_expired) {
		bule->do_send_bu = 1;
		mn_send_bu_msg(bule);
		bul_update_timer(bule);
		if (conf.OptimisticHandoff)
			post_ba_bul_update(bule);
	}
	/* Before bul_iterate, tunnel modification should be done. */
	tunnel_mod(hai->if_tunnel, &hai->primary_coa.addr, &hai->ha_addr,
		   hai->primary_coa.iif, mn_ext_tunnel_ops, hai);
		
	bule->last_coa = bule->coa;
	bule->coa_changed = 0;

	if (homereg_expired) {
		bul_delete(bule);
		mn_do_dad(hai, 1);
	}
}

void mn_send_cn_bu(struct bulentry *bule)
{
	/* Rate limiting CN registration binding updates
	   is necessary for multihomed MNs */
	if (mn_bu_ratelimit(bule))
		return;

	mn_get_ro_lifetime(bule->home, &bule->lifetime, bule->dereg);

	bule->seq++;

	clock_gettime(CLOCK_REALTIME, &bule->lastsent);
	pre_bu_bul_update(bule);
	mn_send_bu_msg(bule);
	bule->last_coa = bule->coa;
	bule->coa_changed = 0;
	if (bule->flags & IP6_MH_BU_ACK) {
		bule->callback = bu_resend;
		bule->delay = conf.InitialBindackTimeoutReReg_ts;
	} else {
		if (bule->dereg) {
			bul_delete(bule);
			return;
		}
		bule->callback = mn_rr_check_entry;
		bule->delay = bule->lifetime;
		tsadd(bule->lastsent, bule->lifetime, bule->hard_expire);
		bule->do_send_bu = 0;
		post_ba_bul_update(bule);
	}
	bul_update_expire(bule);
	bul_update_timer(bule);
}

/* Sets lifetime and expire of bulentry based on lifetime of received BA 
 * @bule: corresponding bul entry
 * @ba_lifetime: lifetime from BA
 * @bu_resend: refresh in seconds
 */
static inline int set_bule_lifetime(struct bulentry *bule,
				    struct timespec *ba_lifetime,
				    struct timespec *br_advice)
{
	assert(bule->type == BUL_ENTRY);

	bule->lifetime = tsmin(bule->lifetime, *ba_lifetime);
	if (bule->flags & IP6_MH_BU_HOME) {
		tssetsec(bule->delay, bule->lifetime.tv_sec * BU_REFRESH_DELAY);
		if (tsbefore(bule->delay, *br_advice))
			bule->delay = *br_advice;
	} else {
		tssetsec(bule->delay, max(ba_lifetime->tv_sec, 0));
	}
	dbg("Set bule lifetime to %ld (s) and resend to bule->delay %d (s)\n", 
	    bule->lifetime.tv_sec, bule->delay.tv_sec);

	return 0;
}

static void mn_send_home_na(struct home_addr_info *hai)
{
	uint32_t na_flags = ND_NA_FLAG_OVERRIDE;
	
	ndisc_send_na(hai->hoa.iif, &hai->hoa.addr,
		      &in6addr_all_nodes_mc, &hai->hoa.addr, na_flags);
	if (hai->lladdr_comp && hai->home_reg_status != HOME_REG_NONE) {
		struct in6_addr lladdr;

		ipv6_addr_llocal(&hai->hoa.addr, &lladdr);
		ndisc_send_na(hai->hoa.iif, &lladdr, 
			      &in6addr_all_nodes_mc, 
			      &lladdr, na_flags);
	}
}

/* Returns number of pending BAs, when it is 0, MN can start registering the 
   addresses on the link as CoAs
*/
static int mn_dereg_home(struct home_addr_info *hai)
{
	if (hai->pend_ba) { 
		hai->pend_ba = 0;
		pending_bas--;
		if (hai->at_home)
			mn_send_home_na(hai);
	}
	return 0;
}

static int mn_chk_bauth(struct ip6_mh_binding_ack *ba, ssize_t len,
			const struct mh_options *mh_opts,
			struct bulentry *e)
{
	struct ip6_mh_opt_auth_data *bauth;
	uint8_t status = ba->ip6mhba_status;

	if ((bauth = mh_opt(&ba->ip6mhba_hdr, mh_opts, IP6_MHOPT_BAUTH))) {
		/* Authenticator is calculated with MH checksum set to 0 */
		ba->ip6mhba_hdr.ip6mh_cksum = 0;
		return mh_verify_auth_data(ba, len, bauth, 
					   &e->coa, &e->peer_addr, e->Kbm);
	}
	if (e->seq == ntohs(ba->ip6mhba_seqno)) {
		if (status == IP6_MH_BAS_HOME_NI_EXPIRED) {
			/* resend Hoti */
			e->rr.state = RR_H_EXPIRED;
		} else if (status == IP6_MH_BAS_COA_NI_EXPIRED) {
			/* resend CoTi */
			e->rr.state = RR_C_EXPIRED;
		} else if (status == IP6_MH_BAS_NI_EXPIRED){ 
			e->rr.state = RR_EXPIRED;
		}
		mn_rr_force_refresh(e);
		mn_reset_ro_bule(e);
		pre_bu_bul_update(e);
	}
	return -1;
}

static void mn_recv_ba(const struct ip6_mh *mh, ssize_t len,
		       const struct in6_addr_bundle *in,
		       __attribute__ ((unused)) int iif)
{
	struct ip6_mh_binding_ack *ba;
	struct mh_options mh_opts;
	struct bulentry *bule;
	struct timespec now, ba_lifetime, br_adv, mps_delay;
	uint16_t seqno;

	TRACE;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_BA);

	if (len < 0 || (size_t)len < sizeof(struct ip6_mh_binding_ack) ||
	    mh_opt_parse(mh, len,
			 sizeof(struct ip6_mh_binding_ack), &mh_opts) < 0)
	    return;

	ba = (struct ip6_mh_binding_ack *)mh;

	pthread_rwlock_wrlock(&mn_lock);
	bule = bul_get(NULL, in->dst, in->src);
	if (!bule || bule->type != BUL_ENTRY) {
		MDBG("Got BA without corresponding BUL entry "
		     "from %x:%x:%x:%x:%x:%x:%x:%x "
		     "to home address %x:%x:%x:%x:%x:%x:%x:%x "
		     "with coa %x:%x:%x:%x:%x:%x:%x:%x\n",
		     NIP6ADDR(in->src),  
		     NIP6ADDR(in->dst),
		     NIP6ADDR(in->local_coa != NULL ? 
			      in->local_coa : &in6addr_any));
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	dbg("Got BA from %x:%x:%x:%x:%x:%x:%x:%x "
	    "to home address %x:%x:%x:%x:%x:%x:%x:%x "
	    "with coa %x:%x:%x:%x:%x:%x:%x:%x and status %d\n",
	    NIP6ADDR(in->src), NIP6ADDR(in->dst),
	    NIP6ADDR(in->local_coa != NULL ? in->local_coa : &in6addr_any),
	    ba->ip6mhba_status);
	dbg("Dumping corresponding BULE\n");
	dbg_func(bule, dump_bule);
	/* First check authenticator */
	if (!(bule->flags & IP6_MH_BU_HOME) &&
	    mn_chk_bauth(ba, len, &mh_opts, bule)) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	/* Then sequence number */
	seqno = ntohs(ba->ip6mhba_seqno);
	if (bule->seq != seqno) {
		if (ba->ip6mhba_status != IP6_MH_BAS_SEQNO_BAD) {
			/*
			 * In this case, ignore BA and resends BU.
			 */
			MDBG("Got BA with incorrect sequence number %d, " 
			     "the one sent in BU was %d\n", seqno, bule->seq);
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
	}
	bule->do_send_bu = 0;
	bule->consecutive_resends = 0;
	clock_gettime(CLOCK_REALTIME, &now);
	if (ba->ip6mhba_status >= IP6_MH_BAS_UNSPECIFIED) {
		if (ba->ip6mhba_status == IP6_MH_BAS_SEQNO_BAD) {
			MDBG("out of sync seq nr\n");
			clock_gettime(CLOCK_REALTIME, &bule->lastsent);
			bule->seq = seqno + 1;
			if (bule->flags & IP6_MH_BU_HOME)
				mn_get_home_lifetime(bule->home,
						     &bule->lifetime, 0);
			else
				mn_get_ro_lifetime(bule->home,
						   &bule->lifetime, 0);
			bule->callback = bu_resend;
			pre_bu_bul_update(bule);
			mn_send_bu_msg(bule);
			bule->delay = conf.InitialBindackTimeoutReReg_ts;
			bul_update_timer(bule);
			if (bule->flags & IP6_MH_BU_HOME &&
			    conf.OptimisticHandoff) {
				post_ba_bul_update(bule);	
			}
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
		if (bule->flags & IP6_MH_BU_HOME) { 
			struct home_addr_info *hai = bule->home;
			char err_str[MAX_BA_STATUS_STR_LEN];

			if (hai->at_home) {
				bul_delete(bule);
				mn_do_dad(hai, 1);
				pthread_rwlock_unlock(&mn_lock);
				return;
			}

			mh_ba_status_to_str(ba->ip6mhba_status, err_str);
			syslog(LOG_ERR, "Received BA with error status %s. "
			       "Unable to register with HA. Deleting entry\n",
			       err_str);

			if (hai->use_dhaad) {
				bule_invalidate(bule, &now, 0);
				mn_change_ha(hai);
			} else {
				bule_invalidate(bule, &now, 1);
			}
			pthread_rwlock_unlock(&mn_lock);
			return;
		} else {
			/* Don't resend BUs to this CN */
			bule_invalidate(bule, &now, 1);
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
	}
	if (bule->wait_ack) 
		bule->wait_ack = 0;
	else {
		MDBG("unexpected BA, ignoring\n");
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	tssetsec(ba_lifetime, ntohs(ba->ip6mhba_lifetime) << 2);
	br_adv = ba_lifetime;
	tsadd(bule->lastsent, ba_lifetime, bule->hard_expire);
	if (!(bule->flags & IP6_MH_BU_HOME) || !conf.OptimisticHandoff)
		post_ba_bul_update(bule);
	if (bule->flags & IP6_MH_BU_HOME) {
		struct home_addr_info *hai = bule->home;
		struct ip6_mh_opt_refresh_advice *bra;

		if (bule->flags & IP6_MH_BU_MR &&
		    !(ba->ip6mhba_flags & IP6_MH_BA_MR)) {
			if (hai->use_dhaad) {
				mn_change_ha(hai);
			} else {
				int one = 1;
				bul_iterate(&hai->bul, mn_dereg, &one);
			}
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
		if (!tsisset(ba_lifetime)) {
			int type = FLUSH_FAILED;
			mn_dereg_home(hai);
			bul_delete(bule);
			/* If BA was for home registration & succesful 
			 *  Send RO BUs to CNs for this home address.
			 */
			bul_iterate(&hai->bul, _bul_flush, &type);
			bul_iterate(&hai->bul, mn_rr_start_handoff, NULL);
			pthread_rwlock_unlock(&mn_lock);
			mn_movement_event(NULL);
			mn_block_rule_del(hai);
			return;
		}
		/* If status of BA is 0 or 1, Binding Update is accepted. */
		if (ba->ip6mhba_status == IP6_MH_BAS_PRFX_DISCOV){
			mpd_trigger_mps(&bule->hoa, &bule->peer_addr);
		}else if( hai->home_reg_status == HOME_REG_UNCERTAIN && tsisset(ba_lifetime)){
			if(tsisset(hai->hoa.timestamp)){
				mps_delay = tsmin(hai->hoa.valid_time, ba_lifetime);
				mpd_schedule_first_mps(&bule->hoa, &bule->peer_addr, &mps_delay);
			}else
				mpd_trigger_mps(&bule->hoa, &bule->peer_addr);
		}

		/* If BA was for home registration & succesful 
		 *  Send RO BUs to CNs for this home address.
		 */
		hai->home_reg_status = HOME_REG_VALID;
		bul_iterate(&hai->bul, mn_rr_start_handoff, NULL);

		/* IP6_MH_BA_KEYM  */
		if (bule->flags & IP6_MH_BU_KEYM) {
			if (ba->ip6mhba_flags & IP6_MH_BA_KEYM) {
				/* Inform IKE  to send readdress msg */

			} else {
				/* Inform IKE to renegotiate SAs */

				/* Remove the flag from this bule */
				bule->flags &= ~IP6_MH_BU_KEYM;

				/* Issue a warning */
				syslog(LOG_ERR,
			         "HA does not support IKE session surviving, "
			         "traffic may be interrupted after movements.\n"
				 );
			}
		}
		bra = mh_opt(&ba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_BREFRESH);
		if (bra)
			tssetsec(br_adv, ntohs(bra->ip6mora_interval) << 2);
	}
	if (!tsisset(ba_lifetime)) {
		dbg("Deleting bul entry\n");
		bul_delete(bule);
	}  else {
		set_bule_lifetime(bule, &ba_lifetime, &br_adv);
		if (bule->flags & IP6_MH_BU_HOME) {
			dbg("Callback to bu_refresh after %d seconds\n",
			    bule->delay.tv_sec);
			bule->callback = bu_refresh;
		} else {
			dbg("Callback to mn_rr_check_entry after %d seconds\n",
			    bule->delay.tv_sec);
			bule->callback = mn_rr_check_entry; 
		}
		bul_update_expire(bule);
		bul_update_timer(bule);
	}
	pthread_rwlock_unlock(&mn_lock);
}

static struct mh_handler mn_ba_handler = {
	.recv = mn_recv_ba,
};

static int do_handoff(struct home_addr_info *hai)
{
	if (!IN6_IS_ADDR_UNSPECIFIED(&hai->ha_addr)) {
		if (hai->pend_ba)
			mn_send_home_bu(hai);
		else if (!pending_bas) {
			if (!hai->at_home ||
			    hai->home_reg_status != HOME_REG_NONE)
				mn_send_home_bu(hai);
			bul_iterate(&hai->bul, mn_rr_start_handoff, NULL);
			hai->verdict = MN_HO_NONE;
		}
	} else if (!pending_bas) {
		if (hai->at_home)
			dhaad_stop(hai);
		else
			dhaad_start(hai);
		
	}
	return 0;
}

struct home_addr_info *mn_get_home_addr(const struct in6_addr *hoa)
{
	struct list_head *l;
        list_for_each(l, &home_addr_list) {
		struct home_addr_info *hai;
		hai = list_entry(l, struct home_addr_info, list);
		if (IN6_ARE_ADDR_EQUAL(hoa, &hai->hoa.addr))
			return hai;
	}
	return NULL;
}

struct home_addr_info *mn_get_home_addr_by_dhaadid(uint16_t dhaad_id)
{
	struct list_head *lh;

	list_for_each(lh, &home_addr_list) {
		struct home_addr_info *hai;
		hai = list_entry(lh, struct home_addr_info, list);
		pthread_mutex_lock(&hai->ha_list.c_lock);
		if (hai->ha_list.dhaad_id == dhaad_id) {
			pthread_mutex_unlock(&hai->ha_list.c_lock);
			return hai;
		}
		pthread_mutex_unlock(&hai->ha_list.c_lock);
	}
	return NULL;
}

struct flag_hoa_args {
	struct home_addr_info *target;
	int flag;
};

static int flag_hoa(struct ifaddrmsg *ifa, struct rtattr *rta_tb[], void *arg)
{
	/*
	 * To do: also call this function for addresses learned through MPD 
	 */

	struct flag_hoa_args *fhoa = arg;
	struct home_addr_info *hai = fhoa->target;
	struct mn_addr *hoa = &hai->hoa;
	struct in6_addr *addr = RTA_DATA(rta_tb[IFA_ADDRESS]);
	struct timespec now;
	uint32_t preferred;
	uint32_t valid;
	int err, plen;

	if (hai->if_tunnel >= 0 && ifa->ifa_index == (uint32_t)hai->if_tunnel)
		plen = 128;
	else
		plen = hai->plen;

	clock_gettime(CLOCK_REALTIME, &now);

	if (rta_tb[IFA_CACHEINFO] != NULL) {
		struct ifa_cacheinfo *ci = RTA_DATA(rta_tb[IFA_CACHEINFO]);
		mn_update_hoa_lifetime(hoa, &now, 
				       ci->ifa_valid, ci->ifa_prefered);
		valid = ci->ifa_valid;
		preferred = ci->ifa_prefered;
	} else {
		preferred = mpd_curr_lft(&now,
					 &hoa->timestamp,
					 hoa->preferred_time.tv_sec);
		valid = mpd_curr_lft(&now,
				     &hoa->timestamp,
				     hoa->valid_time.tv_sec);
	}
	if (fhoa->flag)
		ifa->ifa_flags |= IFA_F_HOMEADDRESS_NODAD;
	else
		ifa->ifa_flags &= ~IFA_F_HOMEADDRESS_NODAD;

	MDBG("set HoA %x:%x:%x:%x:%x:%x:%x:%x/%d iif %d flags %x preferred_time %u valid_time %u\n",
	     NIP6ADDR(addr), plen, ifa->ifa_index, ifa->ifa_flags, preferred, valid);

	if ((err = addr_add(addr, plen,
			    ifa->ifa_flags, ifa->ifa_scope, ifa->ifa_index,
			    preferred, valid)) < 0) {
		MDBG("failed with %d\n", err);
		return err;
	}
	hoa->iif = ifa->ifa_index;
	return 0;
}

static void nemo_mr_rules_del(struct home_addr_info *hinfo)
{
	struct list_head *l;

	list_for_each(l, &hinfo->mob_net_prefixes) {
		struct prefix_list_entry *p = NULL;
		p = list_entry(l, struct prefix_list_entry, list);
		rule_del(NULL, RT6_TABLE_MIP6,
			 IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST,
			 &p->ple_prefix, p->ple_plen, &in6addr_any, 0, 0);
		rule_del(NULL, RT6_TABLE_MAIN,
			 IP6_RULE_PRIO_MIP6_MNP_IN, RTN_UNICAST,
			 &in6addr_any, 0, &p->ple_prefix, p->ple_plen, 0);
	}
}

static int nemo_mr_rules_add(struct home_addr_info *hinfo)
{
	struct prefix_list_entry *pe = NULL;
	struct list_head *l;

	list_for_each(l, &hinfo->mob_net_prefixes) {
		struct prefix_list_entry *p = NULL;
		p = list_entry(l, struct prefix_list_entry, list);
		if (rule_add(NULL, RT6_TABLE_MAIN,
			     IP6_RULE_PRIO_MIP6_MNP_IN, RTN_UNICAST,
			     &in6addr_any, 0,
			     &p->ple_prefix, p->ple_plen, 0) < 0) {
			pe = p;
			goto undo;
		}
		if (rule_add(NULL, RT6_TABLE_MIP6,
			     IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST,
			     &p->ple_prefix, p->ple_plen,
			     &in6addr_any, 0, 0) < 0) {
			rule_del(NULL, RT6_TABLE_MAIN,
				 IP6_RULE_PRIO_MIP6_MNP_IN, RTN_UNICAST,
				 &in6addr_any, 0, &p->ple_prefix, p->ple_plen, 0);
			pe = p;
			goto undo;
		}
	}
	return 0;
undo:
	list_for_each(l, &hinfo->mob_net_prefixes) {
		struct prefix_list_entry *p = NULL;
		p = list_entry(l, struct prefix_list_entry, list);
		rule_del(NULL, RT6_TABLE_MIP6,
			 IP6_RULE_PRIO_MIP6_FWD,  RTN_UNICAST,
			 &p->ple_prefix, p->ple_plen, &in6addr_any, 0, 0);
		rule_del(NULL, RT6_TABLE_MAIN,
			 IP6_RULE_PRIO_MIP6_MNP_IN, RTN_UNICAST,
			 &in6addr_any, 0, &p->ple_prefix, p->ple_plen, 0);
		if (p == pe)
			break;
	}
	return -1;
}

static void clean_home_addr_info(struct home_addr_info *hai)
{
	struct flag_hoa_args arg;
	int plen = (hai->hoa.iif == hai->if_tunnel ? 128 : hai->plen);

	list_del(&hai->list);
	if (hai->mob_rtr)
		nemo_mr_rules_del(hai);
	arg.target = hai;
	arg.flag = 0;
	addr_do(&hai->hoa.addr, plen,
		hai->hoa.iif, &arg, flag_hoa);
	bul_iterate(&hai->bul, mn_dereg, NULL);
	bul_home_cleanup(&hai->bul);

	mn_block_rule_del(hai);

	rule_del(NULL, RT6_TABLE_MIP6,
		 IP6_RULE_PRIO_MIP6_HOA_OUT, RTN_UNICAST,
		 &hai->hoa.addr, 128, &in6addr_any, 0, FIB_RULE_FIND_SADDR);
	tunnel_del(hai->if_tunnel, NULL, NULL);
	dhaad_stop(hai);
	free(hai);
}	

static int rpl_copy(const struct list_head *rpl1, struct list_head *rpl2)
{
	struct list_head *l, *n;
	struct xfrm_ro_pol *rp1, *rp2;

	list_for_each(l, rpl1) {
		rp1 = list_entry(l, struct xfrm_ro_pol, list);
		rp2 = malloc(sizeof(struct xfrm_ro_pol));
		if (rp2 == NULL)
			goto undo;
		memcpy(rp2, rp1, sizeof(struct xfrm_ro_pol));
		list_add_tail(&rp2->list, rpl2);
	}
	return 0;
undo:
	list_for_each_safe(l, n, rpl2) {
		list_del(l);
		rp2 = list_entry(l, struct xfrm_ro_pol, list);
		free(rp2);
	}
	return -1;
}

static struct home_addr_info *hai_copy(struct home_addr_info *conf_hai)
{
	struct home_addr_info *hai = malloc(sizeof(struct home_addr_info));

	if (hai != NULL) {
		pthread_mutexattr_t mattrs;
		pthread_mutexattr_init(&mattrs);
		pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);

		memcpy(hai, conf_hai, sizeof(struct home_addr_info));

		if (pthread_mutex_init(&hai->ha_list.c_lock, NULL))
			goto undo;

		INIT_LIST_HEAD(&hai->mob_net_prefixes);
		if (hai->mob_rtr &&
		    prefix_list_copy(&conf_hai->mob_net_prefixes,
				     &hai->mob_net_prefixes) < 0)
			goto mutex_undo;

		INIT_LIST_HEAD(&hai->ro_policies);
		if (rpl_copy(&conf_hai->ro_policies, &hai->ro_policies) < 0)
			goto mnp_undo;

		INIT_LIST_HEAD(&hai->ha_list.tqe.list);
		INIT_LIST_HEAD(&hai->ha_list.home_agents);
	}
	return hai;
mnp_undo:
	prefix_list_free(&hai->mob_net_prefixes);
mutex_undo:
	pthread_mutex_destroy(&hai->ha_list.c_lock);
undo:
	free(hai);
	return NULL;
}

static int conf_home_addr_info(struct home_addr_info *conf_hai)
{
	struct list_head *list, *n;
	struct timespec init = { 0, 0 };
	struct flag_hoa_args arg;
	struct home_addr_info *hai;

	MDBG("HoA address %x:%x:%x:%x:%x:%x:%x:%x\n", 
	     NIP6ADDR(&conf_hai->hoa.addr)); 

	if  ((hai = hai_copy(conf_hai)) == NULL)
		goto err;

	if (hai->mob_rtr) {
		MDBG("is Mobile Router\n");
		list_for_each(list, &hai->mob_net_prefixes) {
			struct prefix_list_entry *p;
			p = list_entry(list, struct prefix_list_entry, list);
			MDBG("Mobile Network Prefix %x:%x:%x:%x:%x:%x:%x:%x/%d\n",
			     NIP6ADDR(&p->ple_prefix), p->ple_plen);
		}
	}
	if (IN6_IS_ADDR_UNSPECIFIED(&hai->ha_addr)) {
		hai->use_dhaad = 1;
	} else {
		MDBG("HA address %x:%x:%x:%x:%x:%x:%x:%x\n", 
		     NIP6ADDR(&hai->ha_addr)); 
	}
	hai->if_tunnel = tunnel_add(&hai->hoa.addr, &hai->ha_addr,
				    hai->if_home, NULL, NULL);

	if (hai->if_tunnel <= 0) {
		MDBG("failed to create MN-HA tunnel\n"); 
		goto clean_err;
	}
	if (rule_add(NULL, RT6_TABLE_MIP6,
		     IP6_RULE_PRIO_MIP6_HOA_OUT, RTN_UNICAST,
		     &hai->hoa.addr, 128, &in6addr_any, 0, FIB_RULE_FIND_SADDR) < 0) {
		goto clean_err;
	}

	if (mn_block_rule_add(hai) < 0)
		goto clean_err;

	if(bul_home_init(hai)) {
		goto clean_err;
	}

	MDBG("Home address %x:%x:%x:%x:%x:%x:%x:%x\n", 
	     NIP6ADDR(&hai->hoa.addr)); 
	
	hai->home_reg_status = HOME_REG_NONE;
	hai->verdict = MN_HO_NONE;

	mn_update_hoa_lifetime(&hai->hoa, &init,
			       PREFIX_LIFETIME_INFINITE,
			       PREFIX_LIFETIME_INFINITE);

	arg.target = hai;
	arg.flag = 1;

	if (addr_do(&hai->hoa.addr, 128,
		    hai->if_tunnel, &arg, flag_hoa) < 0) {
		goto clean_err;
	}
	if (hai->mob_rtr && nemo_mr_rules_add(hai) < 0) {
		goto clean_err;
	}
	hai->at_home = hai->hoa.iif == hai->if_home;
	pthread_rwlock_wrlock(&mn_lock);
	list_add(&hai->list, &home_addr_list);
	pthread_rwlock_unlock(&mn_lock);

	MDBG("Added new home_addr_info successfully\n");

	return 0;
clean_err:
	clean_home_addr_info(hai);
err:
	list_for_each_safe(list, n, &home_addr_list) {
		hai = list_entry(list, struct home_addr_info, list);
		clean_home_addr_info(hai);
	}
	return -1;
}

static int mn_home_reg_addr_expires(struct bulentry *e, struct mn_addr *addr)
{
	struct timespec next_bu, addr_expires;

	if (prefix_lft_infinite(addr->valid_time.tv_sec))
		return 0;

	tsadd(e->lastsent, e->delay, next_bu);
	tsadd(addr->timestamp, addr->valid_time, addr_expires);
	
	if (!tsbefore(next_bu, addr_expires))
		return 0;
	e->consecutive_resends = 0;
	return 1;
}

static int update_hoa(struct ifaddrmsg *ifa,
		      __attribute__ ((unused)) struct rtattr *rta_tb[],
		      void *arg)
{
	struct home_addr_info *hai = arg;

        return addr_add(&hai->hoa.addr, 128, ifa->ifa_flags,
			ifa->ifa_scope, hai->hoa.iif,
			hai->hoa.preferred_time.tv_sec, 
			hai->hoa.valid_time.tv_sec);
}

int mn_update_home_prefix(struct home_addr_info *hai,
			  const struct timespec *timestamp,
			  const struct nd_opt_prefix_info *pinfo)
{
	struct timespec now;
	struct nd_opt_prefix_info p;
	uint32_t valid_time;

	if (hai->plen != pinfo->nd_opt_pi_prefix_len ||
	    ipv6_pfx_cmp(&pinfo->nd_opt_pi_prefix, &hai->hoa.addr, hai->plen))
		return 0;

	clock_gettime(CLOCK_REALTIME, &now);

	memcpy(&p, pinfo, sizeof(struct nd_opt_prefix_info));

	p.nd_opt_pi_valid_time = mpd_curr_lft(&now,
					      timestamp,
					      p.nd_opt_pi_valid_time);

	p.nd_opt_pi_preferred_time = mpd_curr_lft(&now,
						  timestamp,
						  p.nd_opt_pi_preferred_time);
	/* todo: also add new HoAs */

	MDBG2("preferred_life %u valid_life %u\n",
	      p.nd_opt_pi_preferred_time, p.nd_opt_pi_valid_time);


	valid_time = mpd_curr_lft(&now, &hai->hoa.timestamp,
				  hai->hoa.valid_time.tv_sec);
	
	mn_update_hoa_lifetime(&hai->hoa, &now, p.nd_opt_pi_valid_time,
			       p.nd_opt_pi_preferred_time);
	
	if (hai->home_reg_status == HOME_REG_NONE) {
		int plen = (hai->hoa.iif == hai->if_tunnel ? 128 : hai->plen);

		/* check if HoA has expired and reinsert it */
		if (valid_time == 0 && hai->hoa.valid_time.tv_sec) {
			struct flag_hoa_args arg;
			arg.target = hai;
			arg.flag = 1;
			addr_do(&hai->hoa.addr, plen,
				hai->hoa.iif, &arg, flag_hoa);
		}
	} else if (hai->home_reg_status != HOME_REG_VALID) {
		if (hai->hoa.valid_time.tv_sec) {
			addr_do(&hai->hoa.addr, 128, hai->hoa.iif, 
				hai, update_hoa);
		} else
			addr_del(&hai->hoa.addr, 128, hai->hoa.iif);
	}

	if (!hai->at_home) {
		struct bulentry *e;
		
		e = bul_get(hai,  NULL, &hai->ha_addr);
		if (e == NULL || !(e->flags & IP6_MH_BU_HOME))
			return -ENOENT;
		
		/* check if new HoA lifetime is smaller than current
		   home registration lifetime */
		MDBG2("preferred_time %u valid_life %u\n",
		      p.nd_opt_pi_preferred_time, p.nd_opt_pi_valid_time);
		
		if (mn_home_reg_addr_expires(e, &hai->hoa)) {
			MDBG("HoA expires before next BU, do_handoff()\n");
			do_handoff(hai);
		} else
			hai->verdict = MN_HO_NONE;

		if (p.nd_opt_pi_valid_time && 
		    !prefix_lft_infinite(p.nd_opt_pi_valid_time))
			mpd_schedule_first_mps(&e->hoa, &e->peer_addr,
					       &hai->hoa.valid_time);
	}
	return 0;
}

/* Decide which CoA should be used for RO. Policy Manager is asked
 * first. If it does not provide a hint, primary CoA is used. The
 * function returns the ifindex of the interface the CoA is associated
 * with and copies the CoA to 'coa'.
 */
static int mn_get_ro_coa(const struct in6_addr *cn,
			 const struct home_addr_info *hai,
			 struct in6_addr *coa)
{
	int ret = conf.pmgr.best_ro_coa(&hai->hoa.addr, cn, coa);

	if (ret <= 0) { /* Policy manager remained silent, let's use hai */
		*coa = hai->primary_coa.addr;
		ret = hai->primary_coa.iif;
	}

	return ret;
}

static struct in6_addr linklocal_prefix = { { { 0xfe,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } };

static inline void linklocal_rt_rules_del(void)
{
	rule_del(NULL, RT6_TABLE_MAIN,
		 IP6_RULE_PRIO_MIP6_COA_OUT, RTN_UNICAST,
		 &linklocal_prefix, 64, &in6addr_any, 0, 0);
}

static inline int linklocal_rt_rules_add(void)
{
	return rule_add(NULL, RT6_TABLE_MAIN,
			IP6_RULE_PRIO_MIP6_COA_OUT, RTN_UNICAST,
			&linklocal_prefix, 64, &in6addr_any, 0, 0);
}

static int mn_ext_tunnel_ops(__attribute__ ((unused)) int request,
			     int old_if, int new_if, void *data)
{
	struct home_addr_info *hai = data;
	struct mv_hoa_args mha;

	if (old_if == new_if)
		return 0;
	
	mha.if_next = new_if;
	mha.target = hai;

	if (hai->hoa.iif == old_if &&
	    (mn_tnl_state_add(hai, new_if, 1) ||
	     addr_do(&hai->hoa.addr, 128, old_if, &mha, mv_hoa) < 0))
		goto undo;

	mn_tnl_state_del(hai, old_if, 1);
	hai->if_tunnel = new_if;
	return 0;
undo:
	mha.if_next = old_if;

	if (hai->hoa.iif == new_if)
		addr_do(&hai->hoa.addr, 128, new_if, &mha, mv_hoa); 
	mn_tnl_state_del(hai, new_if, 1);
	return -1;
}

static int mn_move(struct home_addr_info *hai)
{
	struct mv_hoa_args mha;

	mha.target = hai;

	TRACE;

	if (hai->at_home) {
		int plen = (hai->hoa.iif == hai->if_tunnel ? 128 : hai->plen);
		struct bulentry *e;
		MDBG("in home net\n");
		if (hai->home_reg_status == HOME_REG_NONE &&
		    (e = bul_get(hai,  NULL, &hai->ha_addr)) != NULL &&
		    e->flags & IP6_MH_BU_HOME && e->type != BUL_ENTRY)
			bul_delete(e);
		if (hai->hoa.iif != hai->primary_coa.iif) {
			mha.if_next = hai->primary_coa.iif;
			addr_do(&hai->hoa.addr, plen,
				hai->hoa.iif, &mha, mv_hoa);
			if (hai->home_reg_status == HOME_REG_NONE) {
				mn_send_home_na(hai);
				do_handoff(hai);
				return 0;
			}
		}
		if (hai->home_reg_status != HOME_REG_NONE && !hai->pend_ba) {
			pending_bas++;
			hai->pend_ba = 1;	
			do_handoff(hai);
		}
	} else {
		MDBG("in foreign net\n");
		mn_block_rule_add(hai);
		if (hai->pend_ba) {
			hai->pend_ba = 0;
			pending_bas--;
		}
		if (hai->hoa.iif != hai->if_tunnel) {
			mha.if_next = hai->if_tunnel;
			addr_do(&hai->hoa.addr, hai->plen,
				hai->hoa.iif, &mha, mv_hoa);
		}
		do_handoff(hai);
	}
	return 0;
}

static int mn_recv_na(int fd, struct home_addr_info *hai, 
		      struct in6_addr *addr, int ifindex, int has_home_reg)
{
	unsigned char msg[MAX_PKT_LEN];
	struct sockaddr_in6 saddr;
	struct in6_addr *daddr;
	struct in6_pktinfo pkt_info;
	int len, iif, hoplimit;
	struct nd_neighbor_advert *na;
	uint8_t hwa[L2ADDR_MAX_SIZE];
	int ret, hwalen = 0;
	int iface_type;

	len = icmp6_recv(fd, msg, sizeof(msg), &saddr, &pkt_info, &hoplimit);

	if (len < 0)
		return 0;

	daddr = &pkt_info.ipi6_addr;
	iif = pkt_info.ipi6_ifindex;
	na = (struct nd_neighbor_advert *)msg;

	if (iif != ifindex || hoplimit < 255 || (size_t)len < sizeof(*na) ||
	    na->nd_na_code != 0 || IN6_IS_ADDR_MULTICAST(&na->nd_na_target) ||
	    (na->nd_na_flags_reserved & ND_NA_FLAG_SOLICITED &&
	     IN6_IS_ADDR_MULTICAST(daddr)) ||
	    (!IN6_ARE_ADDR_EQUAL(addr, &na->nd_na_target)))
		return 0;

	if (!has_home_reg)
		return 1;

	/* No need to look for HA L2 @ if no hai was provided or
	 * if interface type does not have L2 address */
	if (hai != NULL &&
	    ((iface_type = nd_get_iface_type(iif)) >= 0) &&
	    ndisc_get_l2addr_len(iface_type) > 0) {
		int optlen = len - sizeof(struct nd_neighbor_advert);
		uint8_t *opt = (uint8_t *)(na + 1);

		while (optlen > 1) {
			int olen = opt[1] << 3;

			if (olen > optlen || olen == 0) 
				return 1;
			
			switch (opt[0]) {
			case ND_OPT_TARGET_LINKADDR:
				hwalen = ndisc_l2addr_from_opt(iface_type,
							       hwa, &opt[2],
							       olen - 2);
				if (hwalen < 0)
					return 1;
				break;
			}

			optlen -= olen;
			opt += olen;
		}
	}

	if (hwalen) {
		ret = neigh_add(iif, NUD_STALE, NTF_ROUTER,
				&hai->ha_addr, hwa, hwalen, 1);
		dbg("ret %d\n", ret);
	}

	return 1;
}

int mn_lladdr_dad(struct ifaddrmsg *ifa, struct rtattr *rta_tb[],
		  __attribute__ ((unused)) void *arg)
{
	struct in6_addr *lladdr = RTA_DATA(rta_tb[IFA_ADDRESS]);
	addr_del(lladdr, ifa->ifa_prefixlen, ifa->ifa_index);
	return addr_add(lladdr, ifa->ifa_prefixlen, ifa->ifa_flags,
			ifa->ifa_scope, ifa->ifa_index, 0, 0);
}

static int mn_dad_probe(struct in6_addr *addr, int plen, int ifindex)
{
	int deleted;

	if (IN6_IS_ADDR_LINKLOCAL(addr))
		return addr_do(addr, plen, ifindex, NULL, mn_lladdr_dad);

	/* make sure address isn't configured on interface before
	   DAD probe or MN will receive reply from itself */
	deleted = !addr_del(addr, plen, ifindex);
	ndisc_send_ns(ifindex, addr);
	return deleted;
}

static int mn_hoa_add(struct home_addr_info *hai, 
		      struct in6_addr *addr, int plen, int ifindex)
{
	int scope;
	int flag;
	uint32_t preferred;
	uint32_t valid;

	if (hai == NULL) {
		scope = RT_SCOPE_LINK;
		preferred = 0;
		valid = 0;
		flag = 0;
	} else {
		struct mn_addr *hoa = &hai->hoa;
		struct timespec now;
		
		scope = RT_SCOPE_UNIVERSE;

		clock_gettime(CLOCK_REALTIME, &now);

		preferred = mpd_curr_lft(&now, &hoa->timestamp,
					 hoa->preferred_time.tv_sec);
		
		valid = mpd_curr_lft(&now, &hoa->timestamp,
				     hoa->valid_time.tv_sec);

		flag = IFA_F_HOMEADDRESS_NODAD;
	}
	return addr_add(addr, plen, flag, scope, ifindex, preferred, valid);
}

static int mn_addr_do_dad(int fd, struct home_addr_info *hai, 
			  struct in6_addr *addr, int plen,
			  int ifindex, int has_home_reg)
{
	fd_set rset;
	struct timeval tv;
	int add = (mn_dad_probe(addr, plen, ifindex) || hai == NULL);

	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	tv.tv_sec = DAD_TIMEOUT;
	tv.tv_usec = 0;


	for (;;) {
		int ret;
		/* Note on portability: we assume that tv is modified to show
		   the time left which is AFAIK true only in Linux 
		   timeout 
		*/
		ret = select(fd+1, &rset, NULL, NULL, &tv);
		if (ret < 0)
			return -1;
		
		if (ret == 0) {
			if (add)
				mn_hoa_add(hai, addr, plen, ifindex);
			if (has_home_reg) {
				MDBG("HA didn't answer DAD probe!\n");
				return -1;
			} else {
				MDBG("DAD succeeded!\n");
				MDBG("address = %x:%x:%x:%x:%x:%x:%x:%x\n",
				     NIP6ADDR(addr));
				if (!IN6_IS_ADDR_LINKLOCAL(addr) && hai)
					mn_block_rule_del(hai);
				return 0;
			}
		} else {
			if (!mn_recv_na(fd, hai, addr, ifindex, has_home_reg))
				continue;

			if (has_home_reg) {
				MDBG("HA answered DAD probe!\n");
				if (add)
					mn_hoa_add(hai, addr, plen, ifindex);
				return 0;
			} else {
				MDBG("DAD failed!\n");
				return -1;
			}
		}
	}
}

static int mn_do_dad(struct home_addr_info *hai, int dereg)
{
	int sock, ret = -1, val = 1;
	struct icmp6_filter filter;
	struct in6_addr solicit;
	int type = FLUSH_ALL;

	if ((sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
		return ret;

	ICMP6_FILTER_SETBLOCKALL(&filter);
	ICMP6_FILTER_SETPASS(ND_NEIGHBOR_ADVERT, &filter);

	setsockopt(sock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &val, sizeof(val));
	setsockopt(sock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &val, sizeof(val));
	setsockopt(sock, IPPROTO_ICMPV6, ICMP6_FILTER, &filter,
		   sizeof(struct icmp6_filter));

	ipv6_addr_solict_mult(&hai->hoa.addr, &solicit);
	if_mc_group(sock, hai->primary_coa.iif, &in6addr_all_nodes_mc, 
		    IPV6_JOIN_GROUP);
	if_mc_group(sock, hai->primary_coa.iif, &solicit, IPV6_JOIN_GROUP);

	if (hai->home_reg_status == HOME_REG_NONE) {
		if (hai->lladdr_comp) {
			struct in6_addr lladdr;
			ipv6_addr_llocal(&hai->hoa.addr, &lladdr);
			if (mn_addr_do_dad(sock, NULL, &lladdr, 64, 
					   hai->primary_coa.iif, 0) < 0) {
				MDBG("Link-local DAD failed!\n");
				goto err;
			}
		}
		if (mn_addr_do_dad(sock, hai, &hai->hoa.addr, hai->plen, 
				   hai->primary_coa.iif, 0) < 0) {
			MDBG("HoA DAD failed!\n");
			goto err;
		}
		if (dereg)
			mn_send_home_na(hai);
		ret = mn_move(hai);
	} else if (!mn_addr_do_dad(sock, hai, &hai->hoa.addr, 
				   hai->plen, hai->primary_coa.iif, 1)) {
		ret = mn_move(hai);
	}
out:
	close(sock);
	return ret;
err:
	bul_iterate(&hai->bul, _bul_flush, &type);
	goto out;
}

static inline void mn_update_coa_lifetime(struct mn_addr *mn_coa,
					  struct md_coa *md_coa)
{
	mn_coa->timestamp = md_coa->timestamp;
	mn_coa->valid_time = md_coa->valid_time;
	mn_coa->preferred_time = md_coa->preferred_time;
	MDBG2("updated CoA lifetimes: preferred %u s and valid %u s\n", 
	      mn_coa->preferred_time.tv_sec, mn_coa->valid_time.tv_sec);
}

static void mn_coa_updated(struct home_addr_info *hai)
{
	if (!hai->at_home) {
		struct bulentry *e;
		struct mn_addr *coa;
		
		e = bul_get(hai, NULL, &hai->ha_addr);
		if (e == NULL || !(e->flags & IP6_MH_BU_HOME))
			return;
		
		coa = &hai->primary_coa;
		
		MDBG2("preferred_time %u valid_life %u\n",
		      coa->preferred_time.tv_sec,
		      coa->valid_time.tv_sec);
		
		/* check if new CoA lifetime is smaller than current
		   home registration lifetime */
		if (mn_home_reg_addr_expires(e, coa)) {
			MDBG("CoA expires before next BU, do_handoff()\n");
			do_handoff(hai);
		} else
			hai->verdict = MN_HO_NONE;
	}
}

static int mn_home_rtr_chk(struct home_addr_info *hai, struct md_router *rtr)
{
	int at_home;
	struct list_head *lrp;

	if (hai->verdict != MN_HO_RETURN_HOME) {
		hai->at_home = 0;
		return 0;
	}
	at_home = hai->at_home;
	hai->at_home = 1;
	list_for_each(lrp, &rtr->prefixes) {
		struct prefix_list_entry *p;
		p = list_entry(lrp, struct prefix_list_entry, list);
		mn_update_home_prefix(hai, &p->timestamp, &p->pinfo);
	}
	return !at_home;
}


static inline int mn_verify_iface(const struct md_inet6_iface *iface)
{
	/* Tunnel interfaces do not have a default router
	   (route is via the device itself) */
 	return (!list_empty(&iface->coas) &&
		(iface->is_tunnel || !list_empty(&iface->default_rtr)));
}

static struct md_inet6_iface *mn_get_iface(const struct home_addr_info *hai,
					   int pref_iif,
 					   struct list_head *iface_list)
{
	int iif;
	struct md_inet6_iface *best_iface;
	struct list_head *l;

	if ((iif = conf.pmgr.best_iface(&hai->hoa.addr,
					&hai->ha_addr, pref_iif)) > 0 &&
	    (best_iface = md_get_inet6_iface(iface_list, iif)) != NULL &&
	    mn_verify_iface(best_iface))
		return best_iface;
	best_iface = NULL;
	list_for_each(l, iface_list) {
		struct md_inet6_iface *iface;
		iface = list_entry(l, struct md_inet6_iface, list);
		if (mn_verify_iface(iface) &&
		    (best_iface == NULL ||
		     (best_iface)->preference > iface->preference ||
		     (best_iface->preference == iface->preference &&
		      iface->ifindex == pref_iif))) {
			best_iface = iface;
		}
	}
	return best_iface;
}

static struct md_coa *mn_get_coa(const struct home_addr_info *hai, int iif,
				 const struct in6_addr *pref_coa,
				 struct list_head *coa_list)
{
	struct in6_addr coa = in6addr_any;
	int coa_iif;
	struct md_coa *best_coa;
	struct list_head *l;

	coa_iif = conf.pmgr.best_coa(&hai->hoa.addr, &hai->ha_addr,
				     iif, pref_coa, &coa);
	if (coa_iif == iif &&
	    (best_coa = md_get_coa(coa_list, &coa)) != NULL &&
	    tsisset(best_coa->valid_time))
		return best_coa;
	if ((best_coa = md_get_coa(coa_list, pref_coa)) != NULL) {
		if (!tsisset(best_coa->valid_time))
			best_coa = NULL;
		else if (tsisset(best_coa->preferred_time))
			return best_coa;
	}
	list_for_each(l, coa_list) {
		struct md_coa *test_coa = list_entry(l, struct md_coa, list);
		if (tsisset(test_coa->valid_time)) {
			if (tsisset(test_coa->preferred_time))
				return test_coa;
			if (best_coa == NULL)
				best_coa = test_coa;
		}
	}
	return best_coa;
}

static int mn_make_ho_verdict(const struct movement_event *me,
			      const struct home_addr_info *hai, 
			      struct md_router **next_rtr,
			      struct md_coa **next_coa)
{
	struct md_inet6_iface *old_iface = NULL, *new_iface = NULL;
	struct md_router *rtr;
	struct md_coa *coa = NULL;
	int pref_iif = hai->primary_coa.iif; /* prefer current CoA interface */
	int force = 0;

	if (me->iface != NULL && 
	    (!me->iface->is_tunnel) &&
	    (rtr = md_get_first_router(&me->iface->default_rtr)) != NULL &&
	    mn_is_at_home(&rtr->prefixes, &hai->home_prefix, hai->home_plen)) {
		*next_rtr = rtr;
		*next_coa = NULL;
		return MN_HO_RETURN_HOME;
	}
	if (pref_iif > 0) {
		old_iface = md_get_inet6_iface(me->iface_list, pref_iif);
		if (old_iface == NULL)
			pref_iif = 0;
	}
	switch (me->event_type) {
	case ME_DHAAD:
		force = 1;
		break;
	case ME_IFACE_DOWN:
	case ME_LINK_DOWN:
	case ME_RTR_EXPIRED:
		assert(me->iface != NULL);
		if (old_iface != NULL && old_iface != me->iface)
			return MN_HO_IGNORE;
		break;
	case ME_RTR_NEW:
		assert(!list_empty(&me->iface->default_rtr));
	case ME_LINK_UP:
		assert(me->iface != NULL);
		break;
	case ME_RTR_BACK:
	case ME_RTR_UPDATED:
		assert(me->iface != NULL);
		assert(!list_empty(&me->iface->default_rtr));

		if (old_iface == NULL)
			break;

		if (old_iface != me->iface)
			return MN_HO_IGNORE;

		*next_coa = md_get_coa(&old_iface->coas,
				       &hai->primary_coa.addr);
		if (*next_coa == NULL)
			break;
		*next_rtr = md_get_first_router(&old_iface->default_rtr);
		return MN_HO_REESTABLISH;
	case ME_COA_NEW:
		assert(me->iface != NULL);
		assert(me->coa != NULL);
		assert(me->iface->ifindex == me->coa->ifindex);
		break;
	case ME_COA_EXPIRED:
		assert(me->iface != NULL);
		assert(me->coa != NULL);
		assert(me->iface->ifindex == me->coa->ifindex);

		if (old_iface == NULL ||
		    (old_iface == me->iface &&
		     IN6_ARE_ADDR_EQUAL(&hai->primary_coa.addr, 
					&me->coa->addr)))
			break;
		return MN_HO_IGNORE;		
	case ME_COA_LFT_DEC:
	case ME_COA_LFT_INC:
		assert(me->iface != NULL);
		assert(me->coa != NULL);
		assert(me->iface->ifindex == me->coa->ifindex);

		if (old_iface == NULL)
			break;

		if (old_iface != me->iface ||
		    !IN6_ARE_ADDR_EQUAL(&hai->primary_coa.addr,
					&me->coa->addr))
			return MN_HO_IGNORE;

		*next_coa = me->coa;
		return MN_HO_CHECK_LIFETIME;
	default:
		return MN_HO_IGNORE;
	}
	new_iface = mn_get_iface(hai, pref_iif, me->iface_list);

	if (new_iface == NULL)
		return MN_HO_INVALIDATE;

	coa = mn_get_coa(hai, new_iface->ifindex,
			 &hai->primary_coa.addr, &new_iface->coas);

	if (coa == NULL)
		return MN_HO_INVALIDATE;

	if (!force && new_iface == old_iface && 
	    IN6_ARE_ADDR_EQUAL(&coa->addr, &hai->primary_coa.addr))
		return MN_HO_IGNORE;

	if (!new_iface->is_tunnel) {
 		if (list_empty(&new_iface->default_rtr))
			return MN_HO_IGNORE;

		*next_rtr = md_get_first_router(&new_iface->default_rtr);
	}

	*next_coa = coa;
	return MN_HO_PROCEED;
}

static void mn_chk_ho_verdict(struct home_addr_info *hai,
			      const struct movement_event *event)
{
	struct md_router *rtr = NULL;
	struct md_coa *coa = NULL;
	int move_home = 0;

	if (event->event_type == ME_COA_EXPIRED &&
	    IN6_ARE_ADDR_EQUAL(&event->coa->addr, &hai->hoa.addr))
		return;

	hai->verdict = mn_make_ho_verdict(event, hai, &rtr, &coa);

	if (hai->verdict == MN_HO_IGNORE)
		return;

	if (hai->verdict == MN_HO_INVALIDATE) {
		hai->primary_coa.iif = 0;
		return;
	} 
	if (rtr != NULL)
		move_home = mn_home_rtr_chk(hai, rtr);

	if (hai->verdict == MN_HO_CHECK_LIFETIME) {
		mn_update_coa_lifetime(&hai->primary_coa, coa);
		mn_coa_updated(hai);
	} else if (hai->verdict == MN_HO_REESTABLISH) {
		hai->primary_coa.iif = coa->ifindex;
		mn_update_coa_lifetime(&hai->primary_coa, coa);
		mn_coa_updated(hai);
	} else { 
		switch (hai->verdict) {
		case MN_HO_PROCEED:
			hai->primary_coa.iif = coa->ifindex;
			hai->primary_coa.addr = coa->addr;
			mn_update_coa_lifetime(&hai->primary_coa, coa);
			break;
		case MN_HO_RETURN_HOME:
			hai->primary_coa.iif = rtr->ifindex;
			hai->primary_coa.addr = hai->hoa.addr;
			break;
		default:
			return;
		}
		if (hai->at_home && !hai->pend_ba) {
			/* check if router is HA */
			if (hai->home_reg_status != HOME_REG_NONE &&
			    rtr_addr_chk(rtr, &hai->ha_addr)) {
				mn_move(hai);
			} else if (hai->home_reg_status != HOME_REG_NONE ||
				   move_home) {
				int type = FLUSH_FAILED;
				mn_do_dad(hai, 0);
				if (hai->home_reg_status == HOME_REG_NONE)
					bul_iterate(&hai->bul,
						    _bul_flush, &type);
			}
		}
	}
}

int mn_movement_event(struct movement_event *event)
{
	struct list_head *lh;
	struct home_addr_info *hai;

        /* First de-registration */

	pthread_rwlock_wrlock(&mn_lock);

	if (event != NULL) {
		if (event->event_type == ME_DHAAD) {
			hai = mn_get_home_addr_by_dhaadid(event->data);
			if (hai == NULL) {
				pthread_rwlock_unlock(&mn_lock);
				return 0;
			}
			dhaad_stop(hai);
			mn_chk_ho_verdict(hai, event);
		} else {
			if (event->event_type == ME_COA_EXPIRED)
				mn_rr_delete_co(&event->coa->addr);
			list_for_each(lh, &home_addr_list) {
				hai = list_entry(lh, 
						 struct home_addr_info, list);
				mn_chk_ho_verdict(hai, event);
			}
		}
	}
	/* Then registration if we are not at home,
	   otherwise we need to wait for BA to avoid forwarding loops */
	if (!pending_bas) {
		list_for_each(lh, &home_addr_list) {
			hai = list_entry(lh, struct home_addr_info, list);
			if (!hai->at_home && 
			    positive_ho_verdict(hai->verdict)) {
				mn_move(hai);
			}
		}
	}
	pthread_rwlock_unlock(&mn_lock);
	return 0;
}

/**
 * mn_rr_start_handoff - start RR procedure after changing CoA
 * vbule: bulentry
 * 
 * Triggers RR with CN if necessary, else sends BU to CN.
 **/

int mn_rr_start_handoff(void *vbule, __attribute__ ((unused)) void *dummy)
{
	struct bulentry *bule = vbule;

	if (bule->type != BUL_ENTRY || bule->flags & IP6_MH_BU_HOME)
		return 0;

	if (bule->home->at_home) {
		MDBG("Returning home, no need for Care-of keygen token\n");
		bule->dereg = 1;
		tsclear(bule->lifetime);
	}
	bule->if_coa = mn_get_ro_coa(&bule->peer_addr, bule->home, &bule->coa);
	if (bule->if_coa < 0)
		goto delete_entry;

	bule->do_send_bu = 1;
	mn_rr_refresh(bule);
	if (bule->rr.state == RR_READY)
		mn_send_cn_bu(bule);
	return 0;
delete_entry:
	bul_delete(bule);
	return 0;
}

/* mn_start_ro - start RO, triggered by tunneled packet */
void mn_start_ro(struct in6_addr *cn, struct in6_addr *hoa)
{
	struct bulentry *bule;
	struct home_addr_info *hai;
	struct in6_addr coa;
	int if_coa;

	pthread_rwlock_wrlock(&mn_lock);

	hai = mn_get_home_addr(hoa);
	if (!hai || hai->at_home) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	if_coa = mn_get_ro_coa(cn, hai, &coa);

	if (if_coa > 0) {
		MDBG("MN: Start RO to %x:%x:%x:%x:%x:%x:%x:%x, "
		     "from %x:%x:%x:%x:%x:%x:%x:%x\n", 
		     NIP6ADDR(cn), NIP6ADDR(hoa));

		bule = bul_get(NULL, hoa, cn);

		if (bule) {
			/* If BUL entry exists, RR is done or in progress */
			pthread_rwlock_unlock(&mn_lock);
			return;
		}

		bule = create_bule(hoa, cn);
		if (!bule) {
			MDBG("Malloc failed at starting of RO\n");
			pthread_rwlock_unlock(&mn_lock);
			return;
		}

		bule->type = BUL_ENTRY;
		bule->flags = conf.CnBuAck;
		bule->coa = coa;
		bule->if_coa = if_coa;
		bule->coa_changed = -1;
		bule->home = hai;
		bule->rr.state = RR_NOT_STARTED;

		mn_reset_ro_bule(bule);

		if (bul_add(bule) < 0) {
			bul_delete(bule);
			pthread_rwlock_unlock(&mn_lock);
			return;
		}
		mn_rr_refresh(bule);

		if (bule->rr.state == RR_NOT_STARTED)
			bul_delete(bule);
	}
	pthread_rwlock_unlock(&mn_lock);
}

static void mn_recv_brr(__attribute__ ((unused)) const struct ip6_mh *mh,
			ssize_t len, const struct in6_addr_bundle *in,
			__attribute__ ((unused)) int iif)

{
	struct bulentry *e;
	struct in6_addr *cn, *hoa;
	struct timespec now;
	long last_used;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_BRR);

	if (len <  0 || (size_t)len < sizeof(struct ip6_mh_binding_request))
		return;

	cn = in->src;
	hoa = in->dst;
	pthread_rwlock_wrlock(&mn_lock);
	/* Do we have BUL entry for cn?  If not, drop. */
	if ((e = bul_get(NULL, hoa, cn)) == NULL) {
		pthread_rwlock_unlock(&mn_lock);
		return;
	}
	dbg("Received BRR\n");
	dbg_func(e, dump_bule);
	clock_gettime(CLOCK_REALTIME, &now);

	/* Do we need a binding?  */
	last_used = xfrm_last_used(cn, hoa, IPPROTO_DSTOPTS, &now);
	
	if (last_used >= 0 && last_used < MN_RO_RESTART_THRESHOLD)
		mn_rr_start_handoff(e, NULL);

	pthread_rwlock_unlock(&mn_lock);
}

static struct mh_handler mn_brr_handler = {
	.recv = mn_recv_brr,
};

static void mn_recv_be(const struct ip6_mh *mh, ssize_t len,
		       const struct in6_addr_bundle *in,
		       __attribute__ ((unused)) int iif)
{
	struct ip6_mh_binding_error *berr;
	struct bulentry *e;
	struct in6_addr *cn, *hoa;
	struct timespec now;
	struct in6_addr addr;

	statistics_inc(&mipl_stat, MIPL_STATISTICS_IN_BE);

	if (len <  0 || (size_t)len < sizeof(struct ip6_mh_binding_error))
		return;

	berr = (struct ip6_mh_binding_error *)mh;

	cn = in->src;

	pthread_rwlock_wrlock(&mn_lock);

	if (!IN6_IS_ADDR_UNSPECIFIED(&berr->ip6mhbe_homeaddr))
		hoa = &berr->ip6mhbe_homeaddr;
	else if (mn_rr_error_check(in->dst, in->src, &addr))
		hoa = &addr;
	else
		hoa = in->dst;

	/* Do we have BUL entry for cn?  If not, drop. */
	if ((e = bul_get(NULL, hoa, cn)) == NULL || e->type != BUL_ENTRY)
		goto out;

	clock_gettime(CLOCK_REALTIME, &now);

	/* Invalidate old entry, if it is not home entry */
	if (e->flags & IP6_MH_BU_HOME) {
		MDBG("Got BE from HA, it does not understand us ?\n");
		bule_invalidate(e, &now, 1);
		goto out;
	}
	if (berr->ip6mhbe_status == IP6_MH_BES_UNKNOWN_HAO) {
		bul_delete(e);
	} else if (berr->ip6mhbe_status == IP6_MH_BES_UNKNOWN_MH) {
		/* if no ack expected, ignore */
		/* if extension in use, stop using it */
		/* if no extensions, stop route optimization */

		if (!e->do_send_bu)
			goto out;
		if (hoa == &berr->ip6mhbe_homeaddr && e->rr.state != RR_READY)
			goto out;
		if (e->rr.state != RR_READY || e->wait_ack)
			bule_invalidate(e, &now, 1);
	}
out:
	pthread_rwlock_unlock(&mn_lock);
}

static struct mh_handler mn_be_handler = {
	.recv = mn_recv_be,
};

int mn_init(void)
{
	struct list_head *l, *n;

	if (conf.DoRouteOptimizationMN &&
	    ipsec_policy_entry_check(NULL, NULL, IPSEC_F_TNL_ANY)) {
		syslog(LOG_ERR,
		       "MN can't use both DoRouteOptimizationMN and TunnelPayload IPsecPolicy");
		return -1;
	}
	if (pthread_rwlock_init(&mn_lock, NULL))
		return -1;
	if (md_init() < 0)
		return -1;
	if (bul_init() < 0)
		goto err_bul;
	dhaad_mn_init();
	if (mpd_mn_init() < 0)
		goto err_mpd;
	if (rr_init() < 0)
		goto err_rr;
	list_for_each(l, &conf.home_addrs) {
		struct home_addr_info *hai;
		hai = list_entry(l, struct home_addr_info, list); 
		if (conf_home_addr_info(hai) < 0)
			goto err_hoa;
	}
	if_mc_group(ICMP6_MAIN_SOCK, 0, &in6addr_all_nodes_mc, 
		    IPV6_JOIN_GROUP);
	if (linklocal_rt_rules_add() < 0)
		goto err_rule;
	icmp6_handler_reg(ICMP6_PARAM_PROB, &mn_param_prob_handler);
	mh_handler_reg(IP6_MH_TYPE_BERROR, &mn_be_handler);
	mh_handler_reg(IP6_MH_TYPE_BACK, &mn_ba_handler);
	mh_handler_reg(IP6_MH_TYPE_BRR, &mn_brr_handler);
	if (md_start() < 0)
		goto err_md;
	return 0;
err_md:
	mh_handler_dereg(IP6_MH_TYPE_BRR, &mn_brr_handler);
	mh_handler_dereg(IP6_MH_TYPE_BACK, &mn_ba_handler);
	mh_handler_dereg(IP6_MH_TYPE_BERROR, &mn_be_handler);
	icmp6_handler_dereg(ICMP6_PARAM_PROB, &mn_param_prob_handler);
	linklocal_rt_rules_del();
err_rule:
	pthread_rwlock_wrlock(&mn_lock);
	list_for_each_safe(l, n, &home_addr_list) {
		struct home_addr_info *hai;
		hai = list_entry(l, struct home_addr_info, list);
		clean_home_addr_info(hai);
	}
	pthread_rwlock_unlock(&mn_lock);
err_hoa:
	pthread_rwlock_wrlock(&mn_lock);
	bul_flush();
	pthread_rwlock_unlock(&mn_lock);
	rr_cleanup();
err_rr:
	mpd_mn_cleanup();
err_mpd:
	dhaad_mn_cleanup();
	bul_cleanup();
err_bul:
	md_cleanup();
	return -1;
}

void mn_cleanup()
{
	struct list_head *l, *n;
	md_stop();
	mh_handler_dereg(IP6_MH_TYPE_BRR, &mn_brr_handler);
	mh_handler_dereg(IP6_MH_TYPE_BACK, &mn_ba_handler);
	mh_handler_dereg(IP6_MH_TYPE_BERROR, &mn_be_handler);
	icmp6_handler_dereg(ICMP6_PARAM_PROB, &mn_param_prob_handler);
	linklocal_rt_rules_del();
	pthread_rwlock_wrlock(&mn_lock);
	list_for_each_safe(l, n, &home_addr_list) {
		struct home_addr_info *hai;
		hai = list_entry(l, struct home_addr_info, list);
		clean_home_addr_info(hai);
	}
	bul_flush();
	pthread_rwlock_unlock(&mn_lock);
	rr_cleanup();
	mpd_mn_cleanup();
	dhaad_mn_cleanup();
	bul_cleanup();
	md_cleanup();
}

