/* $Id: ndisc.h 1.13 04/09/20 12:47:39+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __NDISC_H__
#define __NDISC_H__ 1

#include <net/if_arp.h>

#define DAD_TIMEOUT         1  /* one second */
#define L2ADDR_MAX_SIZE     6  /* Max required size for supported L2 @ */
#define MAP_L2ADDR_MAX_SIZE 6  /* Max required size for mapped @ format used
				* in ND Src/Tgt Link-Layer Address option */

short ndisc_get_l2addr_len(unsigned short iface_type);

int ndisc_set_linklocal(struct in6_addr *lladdr, uint8_t *hwa,
			unsigned short iface_type);

int ndisc_l2addr_from_opt(unsigned short iface_type, uint8_t *hwa,
			  uint8_t *mapped_addr, int mapped_addr_len);

int ndisc_l2addr_to_opt(int ifindex, uint8_t *addr);

int nd_get_iface_type(int ifindex);

int ndisc_do_dad(int ifi, struct in6_addr *addr, int ll);

int ndisc_send_rs(int ifindex, const struct in6_addr *dst,
		  struct iovec *opts, size_t optslen);

int ndisc_send_ns(int ifindex, const struct in6_addr *target);

int ndisc_send_na(int ifindex, const struct in6_addr *src, 
		  const struct in6_addr *dst,
		  const struct in6_addr *target, uint32_t flags);

void proxy_nd_iface_init(int ifindex);

void proxy_nd_iface_cleanup(int ifindex);

int proxy_nd_start(int ifindex, struct in6_addr *target, 
		   struct in6_addr *src, int bu_flags);

void proxy_nd_stop(int ifindex, struct in6_addr *target, int bu_flags);

int neigh_add(int ifindex, uint16_t state, uint8_t flags,
	      struct in6_addr *dst, uint8_t *hwa, int hwalen,
	      int override);

int neigh_del(int ifindex, struct in6_addr *dst);

int pneigh_add(int ifindex, uint8_t flags, struct in6_addr *dst);

int pneigh_del(int ifindex, struct in6_addr *dst);


#endif
