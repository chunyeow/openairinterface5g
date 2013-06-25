/* $Id: movement.h 1.49 06/05/19 01:51:45+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__ 1

#include <time.h>
#include <net/if.h>

#include "list.h"
#include "tqueue.h"
#include "ndisc.h" /* for L2ADDR_MAX_SIZE*/

#define IP6_MIN_MTU 1280

#define DEFAULT_HOP_LIMIT 64

#define DEFAULT_RETRANSMIT_TIMER 1 * TIME_SEC_MSEC
#define DEFAULT_REACHABLE_TIME 30 * TIME_SEC_MSEC

struct md_router {
	struct list_head list;
	struct in6_addr lladdr;
	struct timespec rtr_lifetime;
	struct timespec reachable;
	struct timespec retransmit;
	struct timespec adv_ival;
	struct timespec lifetime;
	struct md_inet6_iface *iface;
	int used;
	uint8_t hoplimit;
	uint8_t ra_flags;
	short hwalen;
	uint8_t hwa[L2ADDR_MAX_SIZE];
	uint32_t mtu;
	int ifindex;
	int prefix_cnt;
	int raddr_cnt;
	int probed;
	struct in6_addr solicited_addr;
	struct timespec timestamp;
	struct tq_elem tqe;
	struct list_head prefixes;
};

struct md_coa {
	struct list_head list;
	uint8_t flags;
	uint8_t plen;
	uint8_t scope;
	int ifindex;
	int if_is_tunnel;
	struct in6_addr addr;
	struct timespec timestamp;
	struct timespec valid_time;
	struct timespec preferred_time;
};

/* index values for the variables in ipv6_devconf */

/* 
   This is stupid!
   To do: have these enums moved somewhere else than <include/linux/ipv6.h>
*/

enum {
	DEVCONF_FORWARDING = 0,
	DEVCONF_HOPLIMIT,
	DEVCONF_MTU6,
	DEVCONF_ACCEPT_RA,
	DEVCONF_ACCEPT_RA_DEFRTR,
	DEVCONF_ACCEPT_RA_PINFO,
	DEVCONF_ACCEPT_REDIRECTS,
	DEVCONF_AUTOCONF,
	DEVCONF_DAD_TRANSMITS,
	DEVCONF_RTR_SOLICITS,
	DEVCONF_RTR_SOLICIT_INTERVAL,
	DEVCONF_RTR_SOLICIT_DELAY,
	DEVCONF_USE_TEMPADDR,
	DEVCONF_TEMP_VALID_LFT,
	DEVCONF_TEMP_PREFERED_LFT,
	DEVCONF_REGEN_MAX_RETRY,
	DEVCONF_MAX_DESYNC_FACTOR,
	DEVCONF_MAX_ADDRESSES,
	DEVCONF_FORCE_MLD_VERSION,
	DEVCONF_MAX
};

#define MD_LINK_LOCAL_DAD 0x1
#define MD_BLOCK_TRAFFIC 0x2

struct md_inet6_iface {
	struct list_head list;
	int ifindex;
	char name[IF_NAMESIZE];
	unsigned int preference;
	unsigned int link_flags;
	int home_link;
	int ll_dad_unsafe;
	int is_tunnel;
	unsigned short type;
	short hwalen;
	uint8_t hwa[16];
	int32_t devconf[DEVCONF_MAX];
	int32_t app_solicit;
	struct timespec reachable;
	struct timespec retransmit;
	int router_solicits;
	struct in6_addr lladdr;
	unsigned int iface_flags;
	struct list_head default_rtr;
	struct list_head backup_rtrs;
	struct list_head expired_rtrs;
	struct list_head coas;
	struct list_head expired_coas;
	struct tq_elem tqe;
};

enum {
	MD_STRATEGY_EAGER,
	MD_STRATEGY_LAZY
};

enum {
	ME_DHAAD,
	ME_IFACE_UP,
	ME_IFACE_DOWN,
	ME_LINK_UP,
	ME_LINK_DOWN,
	ME_RTR_NEW,
	ME_RTR_EXPIRED,
	ME_RTR_BACK,
	ME_RTR_UPDATED,
	ME_COA_NEW,
	ME_COA_EXPIRED,
	ME_COA_LFT_DEC,
	ME_COA_LFT_INC,
};


struct movement_event {
	int md_strategy;
	int event_type;
	int data;
	struct list_head *iface_list;
	struct md_inet6_iface *iface;
	struct md_coa *coa;
};


static inline struct md_inet6_iface *
md_get_inet6_iface(struct list_head *iface_list, int ifindex)
{
	struct list_head *list;
	list_for_each(list, iface_list) {
		struct md_inet6_iface *iface;
		iface = list_entry(list, struct md_inet6_iface, list);
		if (iface->ifindex == ifindex)
			return iface;
	}
	return NULL;
}

static inline struct md_router *md_get_first_router(struct list_head *rtr_list)
{
	if (!list_empty(rtr_list))
		return list_entry(rtr_list->next, struct md_router, list);
	return NULL;
}

static inline struct md_coa *md_get_coa(struct list_head *coa_list, 
					const struct in6_addr *coaddr)
{
	struct list_head *list;
	struct md_coa *coa = NULL;

	list_for_each(list, coa_list) {
		coa = list_entry(list, struct md_coa, list);
		if (coaddr == NULL || IN6_ARE_ADDR_EQUAL(&coa->addr, coaddr))
			return coa;
	}
	return NULL;
}

int md_init(void);
void md_cleanup(void);

int md_start(void);
void md_stop(void);

void md_trigger_movement_event(int event_type, int data, int ifindex);

struct icmp6_hdr;

static inline int md_is_link_up(struct md_inet6_iface *iface)
{
	return (iface->link_flags & IFF_UP &&
		iface->link_flags & IFF_RUNNING);
} 

int rtr_addr_chk(struct md_router *rtr, struct in6_addr *rtr_addr);

#endif
