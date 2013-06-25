/* $Id: rtnl.h 1.25 06/01/25 04:46:24+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __RTNL_H__
#define __RTNL_H__ 1

#include <libnetlink.h>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#define RT6_TABLE_MIP6 252
#define RT6_TABLE_MAIN 254

// Defined for Proxy MIPv6//////////////////
#define RT6_TABLE_PMIP 200
#define IP6_RULE_PRIO_PMIP6_FWD   1000
///////////////////////////////////////////

#define IP6_RT_PRIO_MIP6_OUT 128
#define IP6_RT_PRIO_MIP6_FWD 192
#define IP6_RT_PRIO_ADDRCONF 256

#define IP6_RULE_PRIO_MIP6_MNP_IN    1000
#define IP6_RULE_PRIO_MIP6_HOA_OUT   1001
#define IP6_RULE_PRIO_MIP6_COA_OUT   1002
#define IP6_RULE_PRIO_MIP6_BLOCK     1003
#define IP6_RULE_PRIO_MIP6_BLOCK_HOA 1004
#define IP6_RULE_PRIO_MIP6_FWD       1005

static inline int rtnl_route_open(struct rtnl_handle *rth, 
				  unsigned subscriptions)
{
	return rtnl_open_byproto(rth, subscriptions, NETLINK_ROUTE);
}

static inline int rtnl_xfrm_open(struct rtnl_handle *rth,
				 unsigned subscriptions)
{
	return rtnl_open_byproto(rth, subscriptions, NETLINK_XFRM);
}

int rtnl_do(int proto, struct nlmsghdr *sn, struct nlmsghdr *rn);

static inline int rtnl_route_do(struct nlmsghdr *sn, struct nlmsghdr *rn)
{
	return rtnl_do(NETLINK_ROUTE, sn, rn);
}

static inline int rtnl_xfrm_do(struct nlmsghdr *sn, struct nlmsghdr *rn)
{
	return rtnl_do(NETLINK_XFRM, sn, rn);
}

int addr_do(const struct in6_addr *addr, int plen, int ifindex, void *arg,
	    int (*do_callback)(struct ifaddrmsg *ifa, 
			       struct rtattr *rta_tb[], void *arg));

int addr_del(const struct in6_addr *addr, uint8_t plen, int ifindex);

int addr_add(const struct in6_addr *addr, uint8_t plen, 
	     uint8_t flags, uint8_t scope, int ifindex, 
	     uint32_t prefered, uint32_t valid);

struct nd_opt_prefix_info;

int prefix_add(int ifindex, const struct nd_opt_prefix_info *pinfo);

int route_add(int oif, uint8_t table, uint8_t proto,
	      unsigned flags, uint32_t metric,
	      const struct in6_addr *src, int src_plen,
	      const struct in6_addr *dst, int dst_plen, 
	      const struct in6_addr *gateway);


int route_del(int oif, uint8_t table, uint32_t metric,
	      const struct in6_addr *src, int src_plen,
	      const struct in6_addr *dst, int dst_plen, 
	      const struct in6_addr *gateway);

int rule_add(const char *iface, uint8_t table,
	     uint32_t priority, uint8_t action,
	     const struct in6_addr *src, int src_plen,
	     const struct in6_addr *dst, int dst_plen, int flags);

int rule_del(const char *iface, uint8_t table,
	     uint32_t priority, uint8_t action,
	     const struct in6_addr *src, int src_plen,
	     const struct in6_addr *dst, int dst_plen,int flags);

int rtnl_iterate(int proto, int type, rtnl_filter_t func, void *extarg);

/**
 * routes_iterate - apply something to all routes
 * @func: pointer to function to apply
 * @extarg: extra arguments for iterator
 *
 * Retrieves all routes assigned to the node and applies function
 * @func to all of them.  Returns zero on success, negative otherwise.
 **/

static inline int routes_iterate(rtnl_filter_t func, void *extarg)
{
	return rtnl_iterate(NETLINK_ROUTE, RTM_GETROUTE, func, extarg);
}

/**
 * addrs_iterate - apply something to all addresses
 * @func: pointer to function to apply
 * @extarg: extra arguments for iterator
 *
 * Retrieves all addresses assigned to the node and applies function
 * @func to all of them.  Returns zero on success, negative otherwise.
 **/
static inline int addrs_iterate(rtnl_filter_t func, void *extarg)
{
	return rtnl_iterate(NETLINK_ROUTE, RTM_GETADDR, func, extarg);
}

/**
 * inet6_ifaces_iterate - apply something to all IPv6 capable interfaces
 * @func: pointer to function to apply
 * @extarg: extra arguments for iterator
 *
 * Retrieves all IPv6 capable interfaces to the node and applies function
 * @func to all of them.  Returns zero on success, negative otherwise.
 **/
static inline int inet6_ifaces_iterate(rtnl_filter_t func, void *extarg)
{
	return rtnl_iterate(NETLINK_ROUTE, RTM_GETLINK, func, extarg);
}

#endif
