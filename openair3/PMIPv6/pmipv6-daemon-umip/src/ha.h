/* $Id: ha.h 1.12 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __HA_H__
#define __HA_H__ 1

extern struct in6_addr ha_myaddr;
extern struct in6_addr ha_mnaddr;

#include "tqueue.h"
#include "mh.h"

struct ha_interface {
	int ifindex;
	struct list_head addr_list;
	struct list_head ha_list;	/* protected by ha_lock */
	uint16_t mpa_flags;		/* protected by mpa_lock */
	struct list_head prefix_list;	/* protected by mpa_lock */
	int prefix_count;		/* protected by mpa_lock */
	struct list_head iflist;
};

struct home_agent {
	struct list_head list;
	struct in6_addr addr;
	uint16_t preference;
	uint16_t flags;
	struct timespec lifetime;
	struct ha_interface *iface;
	struct tq_elem tqe;
};

struct ha_addr_holder {
	struct list_head list;
	struct in6_addr ha_addr;
	struct in6_addr anycast_addr;
};


int homeagents_ifall_init(void);
int homeagent_if_init(int ifindex);
struct ha_interface *ha_get_if(int ifindex);
struct ha_interface *ha_get_if_by_addr(const struct in6_addr *addr);
struct ha_interface *ha_get_if_by_anycast(const struct in6_addr *anycast,
					  struct in6_addr **addr);
int ha_recv_home_bu(const struct ip6_mh *mh, ssize_t len,
		    const struct in6_addr_bundle *in, int iif, uint32_t flags);
#define HA_BU_F_THREAD_JOIN	0x01
#define HA_BU_F_PASSIVE_SEQ	0x02
#define HA_BU_F_SKIP_DAD	0x04
#define HA_BU_F_SKIP_BA		0x08

int ha_init(void);
void ha_cleanup(void);

#endif /* __HA_H__ */
