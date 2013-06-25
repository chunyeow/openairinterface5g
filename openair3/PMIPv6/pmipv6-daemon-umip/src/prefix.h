/* $Id: prefix.h 1.10 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __PREFIX_H__
#define __PREFIX_H__ 1

#include <stdlib.h>
#include <time.h>

#include <netinet/in.h>
#include <netinet/icmp6.h>

#include "list.h"
#include "mipv6.h"
#include "util.h"

#define PREFIX_LIFETIME_INFINITE 0xFFFFFFFF

static inline int prefix_lft_infinite(uint32_t lft)
{
	return lft == PREFIX_LIFETIME_INFINITE;
}

struct prefix_list_entry
{
	struct list_head list;
	struct timespec timestamp;
	struct nd_opt_prefix_info pinfo;
#define ple_plen pinfo.nd_opt_pi_prefix_len
#define ple_flags pinfo.nd_opt_pi_flags_reserved
#define ple_valid_time pinfo.nd_opt_pi_valid_time
#define ple_prefd_time pinfo.nd_opt_pi_preferred_time
#define ple_prefix pinfo.nd_opt_pi_prefix
};

static inline void ipv6_addr_prefix(struct in6_addr *pfx,
				    const struct in6_addr *addr,
				    int plen)
{
	int o, b;

	if (plen < 0) plen = 0;
	else if (plen > 128) plen = 128;

	o = plen >> 3;
	b = plen & 0x7;

	memcpy(pfx->s6_addr, addr, o);
	if (b != 0) {
		pfx->s6_addr[o] = addr->s6_addr[o] & (0xff00 >> b);
		o++;
	}
	memset(pfx->s6_addr + o, 0, 16 - o);
}

static inline void ipv6_addr_create(struct in6_addr *addr,
				    const struct in6_addr *pfx,
				    const struct in6_addr *sfx, int plen)
{
	int o, b;

	plen = (plen > 128 || plen < 0) ? 0 : plen;
	o = plen >> 3;
	b = plen & 0x7;

	memcpy(addr->s6_addr,  pfx->s6_addr, o);
	if (b != 0) {
		addr->s6_addr[o] = ((pfx->s6_addr[o] & (0xff00 >> b)) |
				    (sfx->s6_addr[o] & (0x00ff >> b)));
		o++;
	}
	memcpy(addr->s6_addr + o, sfx->s6_addr + o, 16 - o);
}

/**
 * ipv6_pfx_cmp - Compare two IPv6 prefixes
 * @p1: IPv6 address
 * @p2: IPv6 address
 * @plen: number of bits to compare
 *
 * Perform prefix comparison bitwise for the @plen first bits.
 * Returns zero if the prefixes are the same, otherwise 1.
 **/
static inline int ipv6_pfx_cmp(const struct in6_addr *p1,
			       const struct in6_addr *p2, 
			       int plen)
{
	int o = plen >> 3, b = plen & 0x7;
	int res;

	if ((res = memcmp(p1->s6_addr, p2->s6_addr, o)) !=  0 || b == 0)
		return res;

	return (p1->s6_addr[o] ^ p2->s6_addr[o]) & (0xff00 >> b);

}

static inline void prefix_list_free(struct list_head *pl)
{
	struct list_head *l, *n;
	list_for_each_safe(l, n, pl) {
		list_del(l);
		free(list_entry(l, struct prefix_list_entry, list));
	}
}

struct prefix_list_entry *
prefix_list_get(const struct list_head *, const struct in6_addr *, int);
int prefix_list_cmp(const struct list_head *, const struct list_head *);
int prefix_list_copy(const struct list_head *, struct list_head *);

static inline int  prefix_list_find(const struct list_head *pl,
				    const struct in6_addr *addr, int plen)
{
	return prefix_list_get(pl, addr, plen) != NULL;
}

unsigned long mpd_curr_lft(const struct timespec *now,
			   const struct timespec *tstamp,
			   unsigned long lft);

void dhaad_gen_ha_anycast(struct in6_addr *anycast,
			  const struct in6_addr *pfx, int plen);

static inline void mpd_sanitize_lft(struct timespec *lft)
{
	/* make sure the lifetime doesn't exceed 0x3fffc (0xffff << 2)
	   seconds and is given in four second time units */
	lft->tv_sec = (umin(lft->tv_sec, MAX_BINDING_LIFETIME) &
		       MAX_BINDING_LIFETIME);
	lft->tv_nsec = 0;
}

#endif
