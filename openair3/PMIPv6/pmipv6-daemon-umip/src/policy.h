/* $Id: policy.h 1.57 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __POLICY_H__
#define __POLICY_H__ 1

#include <netinet/in.h>
#include "list.h"

#define POL_MN_IF_MIN_PREFERENCE 10
#define POL_MN_IF_DEF_PREFERENCE POL_MN_IF_MIN_PREFERENCE

struct ip6_mh_binding_update;
struct nd_router_advert;
struct nd_opt_prefix_info;

struct policy_bind_acl_entry {
	struct list_head list;
	struct in6_addr hoa;
	int plen;
	int bind_policy;
	int mnp_count;
	struct list_head mob_net_prefixes;
};

/**
 * default_best_iface - select best interface during handoff
 * @hoa: MN's home address
 * @ha: HA's address
 * @pref_interface: preferred interface
 *
 * Return interface index of preferred interface.
 **/
int default_best_iface(const struct in6_addr *hoa,
		       const struct in6_addr *ha,
		       int pref_iface);

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
int default_best_coa(const struct in6_addr *hoa,
		     const struct in6_addr *ha, int iif,
		     const struct in6_addr *pref_coa,
		     struct in6_addr *coa);

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
int default_max_binding_life(const struct in6_addr *remote_hoa,
			     const struct in6_addr *remote_coa,
			     const struct in6_addr *local_addr,
			     const struct ip6_mh_binding_update *bu, 
			     ssize_t len,
			     const struct timespec *suggested,
			     struct timespec *lifetime);

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
			    const struct in6_addr *remote_coa,
			    const struct in6_addr *local_addr,
			    const struct ip6_mh_binding_update *bu,
			    ssize_t len);

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
int default_use_bradv(const struct in6_addr *remote_hoa,
		      const struct in6_addr *remote_coa,
		      const struct in6_addr *local_addr,
		      const struct timespec *lft,
		      struct timespec *refresh);

/**
 * default_use_keymgm - use K-bit
 * @remote_addr: address of remote node
 * @local_addr: address of local node
 *
 * Determine whetherm to use the Key Management Mobility Capability bit
 * for giver addresses.
 **/
int default_use_keymgm(const struct in6_addr *remote_addr,
		       const struct in6_addr *local_addr);

/**
 * policy_accept_inet6_iface - use interface for MIPv6
 * @iif: interface index
 *
 * Determine whether to allow movement events from interface @ifindex or not.
 * Return 0, if unacceptable, otherwise a positive preference value,
 * 1 being the most preferrable.
 **/
int default_accept_inet6_iface(int iif);

/**
 * default_accept_ra - check if RA is acceptable
 * @iif: the incoming interface index
 * @saddr: the source of the RA
 * @daddr: the destination of the RA
 * @ra: the RA message
 *
 * Determine whether to accept RA or not
 **/
int default_accept_ra(int iif,
		      const struct in6_addr *saddr,
		      const struct in6_addr *daddr,
		      const struct nd_router_advert *ra);

/**
 * default_best_ro_coa - get a suitable care-of address for RO
 * @hoa: own home address
 * @cn: CN address
 * @coa: care-of address
 *
 * Returns ifindex of the CoA, or <= 0 if no CoA is available,
 **/
int default_best_ro_coa(const struct in6_addr *hoa,
			const struct in6_addr *cn,
			struct in6_addr *coa);

int default_get_mnp_count(const struct in6_addr *hoa);

int default_get_mnps(const struct in6_addr *hoa,
		     const int mnp_count,
		     struct nd_opt_prefix_info *mnps);

void policy_cleanup(void);

int policy_init(void);

#endif
