/*
 * $Id: policy.c 1.100 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Ville Nuorvala <vnuorval@tcs.hut.fi>,
 *          Henrik Petander <petander@tcs.hut.fi>
 *
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
#include <netinet/ip6mh.h>

#include "debug.h"
#include "mh.h"
#include "mn.h"
#include "movement.h"
#include "util.h"
#include "conf.h"
#include "policy.h"
#include "hash.h"

#define POLICY_ACL_HASHSIZE 32

pthread_rwlock_t policy_lock;
struct hash policy_bind_acl_hash;
int def_bind_policy = IP6_MH_BAS_PROHIBIT;

/**
 * default_best_iface - select best interface during handoff
 * @hoa: MN's home address
 * @ha: HA's address
 * @pref_interface: preferred interface
 *
 * Return interface index of preferred interface.
 **/
int default_best_iface(__attribute__ ((unused)) const struct in6_addr *hoa,
		       __attribute__ ((unused)) const struct in6_addr *ha,
		       __attribute__ ((unused)) int pref_iface)
{
	return 0;
}

/**
 * default_best_coa - select best CoA duning handoff
 * @hoa: MN's home address
 * @ha: HA's address
 * @iif: required interface
 * @pref_coa: preferred CoA
 * @coa: suggested CoA
 *
 * Return iif if a CoA is available and store the address in @coa 
 **/
int default_best_coa(__attribute__ ((unused)) const struct in6_addr *hoa,
		     __attribute__ ((unused)) const struct in6_addr *ha,
		     __attribute__ ((unused)) int iif,
		     __attribute__ ((unused)) const struct in6_addr *pref_coa,
		     __attribute__ ((unused)) struct in6_addr *coa)
{
	return 0;
}

/**
 * default_max_binding_life - binding lifetime policy
 * @remote_hoa: remote MN's home address
 * @remote_coa: remote MN's care-of address
 * @local_addr: local address
 * @bu: Binding Update message 
 * @suggested: suggested lifetime
 * @lifetime: granted lifetime
 *
 * Stores configurable maximum lifetime for a binding in @lifetime.
 * Returns 1 if successful, otherwise 0.
 **/
int default_max_binding_life(__attribute__ ((unused)) const struct in6_addr *remote_hoa,
			     __attribute__ ((unused)) const struct in6_addr *remote_coa,
			     __attribute__ ((unused)) const struct in6_addr *local_addr,
			     const struct ip6_mh_binding_update *bu, 
			     __attribute__ ((unused)) ssize_t len,
			     __attribute__ ((unused)) const struct timespec *suggested,
			     struct timespec *lifetime)
{
	if (bu->ip6mhbu_flags & IP6_MH_BU_HOME) {
		tssetsec(*lifetime, conf.HaMaxBindingLife);
		return 1;
	}
	return 0;
}

static inline int
policy_check_mob_net_prefix(const struct policy_bind_acl_entry *acl,
			    const struct ip6_mh_binding_update *bu,
			    const struct mh_options *opts)
{
	struct ip6_mh_opt_mob_net_prefix *op;
	for (op = mh_opt(&bu->ip6mhbu_hdr, opts, IP6_MHOPT_MOB_NET_PRFX);
	     op != NULL;
	     op = mh_opt_next(&bu->ip6mhbu_hdr, opts, op)) {
		if (!prefix_list_get(&acl->mob_net_prefixes,
				     &op->ip6mnp_prefix,
				     op->ip6mnp_prefix_len))
			return IP6_MH_BAS_NOT_AUTH_FOR_PRFX;
	}
	return IP6_MH_BAS_ACCEPTED;
}

/**
 * default_discard_binding - check for discard policy
 * @remote_hoa: remote MN's home address
 * @remote_coa: remote MN's care-of address
 * @local_addr: local address
 * @bu: binding update
 *
 * Checks if there is a policy to discard this BU.  Valid return
 * values are %IP6_MH_BAS_ACCEPTED, %IP6_MH_BAS_UNSPECIFIED, and
 * %IP6_MH_BAS_PROHIBIT.
 **/
int default_discard_binding(const struct in6_addr *remote_hoa,
			    __attribute__ ((unused)) const struct in6_addr *remote_coa,
			    __attribute__ ((unused)) const struct in6_addr *local_addr,
			    const struct ip6_mh_binding_update *bu,
			    ssize_t len)
{
	int ret = def_bind_policy;
	struct policy_bind_acl_entry *acl;

	if (bu->ip6mhbu_flags & IP6_MH_BU_MR && !conf.HaAcceptMobRtr)
		return IP6_MH_BAS_MR_OP_NOT_PERMITTED;

	pthread_rwlock_rdlock(&policy_lock);
	acl = hash_get(&policy_bind_acl_hash, NULL, remote_hoa);
	if (acl != NULL) {
		ret = acl->bind_policy;
		if (ret < IP6_MH_BAS_UNSPECIFIED &&
		    bu->ip6mhbu_flags & IP6_MH_BU_MR) {
			struct mh_options opts;
			mh_opt_parse(&bu->ip6mhbu_hdr, len,
				     sizeof(*bu), &opts);
			ret = policy_check_mob_net_prefix(acl, bu, &opts);
		}
	}
	pthread_rwlock_unlock(&policy_lock);
	return ret;
}

/**
 * policy_use_bravd - use Binding refresh advice
 * @remote_hoa: remote MN's home address
 * @remote_coa: remote MN's care-of address
 * @local_addr: local address
 * @lft: lifetime of binding
 * @refersh: used for storing refresh interval returned by policy
 *
 * Checks if a Binding Refresh Advice should be inserted in a Binding
 * Ack.  Returns 0 if BRA should not be used.  Stores proposed refresh
 * advice in @refresh,
 **/
int default_use_bradv(__attribute__ ((unused)) const struct in6_addr *remote_hoa,
		      __attribute__ ((unused)) const struct in6_addr *remote_coa,
		      __attribute__ ((unused)) const struct in6_addr *local_addr,
		      __attribute__ ((unused)) const struct timespec *lft,
		      __attribute__ ((unused)) struct timespec *refresh)
{
	return 0;
}

/**
 * default_use_keymgm - use K-bit
 * @remote_addr: address of remote node
 * @local_addr: address of local node
 *
 * Determine whether to use the Key Management Mobility Capability bit
 * for giver addresses.
 **/
int default_use_keymgm(__attribute__ ((unused)) const struct in6_addr *remote_addr,
		       __attribute__ ((unused)) const struct in6_addr *local_addr)
{
	return conf.KeyMngMobCapability;
}

/**
 * policy_accept_inet6_iface - use interface for MIPv6
 * @iif: interface index
 *
 * Determine whether to allow movement events from interface @ifindex or not.
 * Return 0, if unacceptable, otherwise a positive preference value,
 * 1 being the most preferrable.
 **/
int default_accept_inet6_iface(int iif)
{
	struct list_head *list;
	list_for_each(list, &conf.net_ifaces) {
		struct net_iface *nif;
		nif = list_entry(list, struct net_iface, list);
		if (nif->ifindex == iif) {
			if (is_if_mn(nif)) 
				return nif->mn_if_preference;
			return 0;
		}
	}
	return conf.MnUseAllInterfaces;
}


/**
 * default_accept_ra - check if RA is acceptable
 * @iif: the incoming interface index
 * @saddr: the source of the RA
 * @daddr: the destination of the RA
 * @ra: the RA message
 *
 * Determine whether to accept RA or not
 **/

int default_accept_ra(__attribute__ ((unused)) int iif,
		      __attribute__ ((unused)) const struct in6_addr *saddr,
		      __attribute__ ((unused)) const struct in6_addr *daddr,
		      __attribute__ ((unused)) const struct nd_router_advert *ra)
{
	return 1;
}

/**
 * default_best_ro_coa - get a suitable care-of address for RO
 * @hoa: own home address
 * @cn: CN address
 * @coa: care-of address
 *
 * Returns ifindex of the CoA, or <= 0 if no CoA is available,
 **/
int default_best_ro_coa(__attribute__ ((unused)) const struct in6_addr *hoa,
			__attribute__ ((unused)) const struct in6_addr *cn,
			__attribute__ ((unused)) struct in6_addr *coa)
{
	return 0;
}

int default_get_mnp_count(const struct in6_addr *hoa)
{
	int ret = 0;
	struct policy_bind_acl_entry *acl;
	pthread_rwlock_rdlock(&policy_lock);
	acl = hash_get(&policy_bind_acl_hash, NULL, hoa);
	if (acl != NULL)
		ret = acl->mnp_count;
	pthread_rwlock_unlock(&policy_lock);
	return ret;

}

int default_get_mnps(const struct in6_addr *hoa,
		     const int mnp_count,
		     struct nd_opt_prefix_info *mnps)
{
	int i = 0;
	struct policy_bind_acl_entry *acl;

	pthread_rwlock_rdlock(&policy_lock);
	acl = hash_get(&policy_bind_acl_hash, NULL, hoa);
	if (acl != NULL) {
		struct list_head *l;
		list_for_each(l, &acl->mob_net_prefixes) {
			struct prefix_list_entry *e;
			if (i >= mnp_count)
				break;
			e = list_entry(l, struct prefix_list_entry, list);
			mnps[i++] = e->pinfo;
		}
	}
	pthread_rwlock_unlock(&policy_lock);
	return i;
}

static int policy_bind_acle_cleanup(void *data,
				    __attribute__ ((unused)) void *arg)
{
	struct policy_bind_acl_entry *acl = data;
	free(acl);
	return 0;
}

static void policy_bind_acl_cleanup(void)
{
	def_bind_policy = IP6_MH_BAS_PROHIBIT;
	hash_iterate(&policy_bind_acl_hash, policy_bind_acle_cleanup, NULL);
	hash_cleanup(&policy_bind_acl_hash);
}

void policy_cleanup(void)
{
	pthread_rwlock_wrlock(&policy_lock);
	policy_bind_acl_cleanup();
	pthread_rwlock_unlock(&policy_lock);
}

static int policy_bind_acl_add(struct policy_bind_acl_entry *acl)
{
	int err;
	err = hash_add(&policy_bind_acl_hash, acl, NULL, &acl->hoa);
	if (!err) {
		list_del(&acl->list);
	}
	return err;
}

static int policy_bind_acl_config(void)
{
	struct list_head *list, *n;
	int err;

	pthread_rwlock_wrlock(&policy_lock);

	if ((err = hash_init(&policy_bind_acl_hash, SINGLE_ADDR,
			     POLICY_ACL_HASHSIZE)) < 0)
		goto out;

	def_bind_policy = conf.DefaultBindingAclPolicy;

	list_for_each_safe(list, n, &conf.bind_acl) {
		struct policy_bind_acl_entry *acl;
		acl = list_entry(list, struct policy_bind_acl_entry, list);
		if ((err = policy_bind_acl_add(acl)) < 0) {
			policy_bind_acl_cleanup();
			break;
		}
	}
out:
	pthread_rwlock_unlock(&policy_lock);
	return err;
}

int policy_init(void)
{
	if (pthread_rwlock_init(&policy_lock, NULL))
		return -1;
	return policy_bind_acl_config();
}

