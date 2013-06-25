/* $Id: ipsec.h 1.27 06/04/22 02:11:18+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __IPSEC_H__
#define __IPSEC_H__
#include <linux/xfrm.h>
#include "list.h"
	
#define IPSEC_F_MH_BUBA		0x001
#define IPSEC_F_MH_BERR		0x002
#define IPSEC_F_MH		0x004
#define IPSEC_F_ICMP_MPD	0x008
#define IPSEC_F_ICMP_ND		0x010
#define IPSEC_F_ICMP		0x020
#define IPSEC_F_ANY		0x040

#define IPSEC_F_TNL_MH_RR	0x100
#define IPSEC_F_TNL_MH		0x200
#define IPSEC_F_TNL_ANY		0x400

#define IPSEC_POLICY_TYPE_HOMEREGBINDING IPSEC_F_MH_BUBA
#define IPSEC_POLICY_TYPE_BERROR IPSEC_F_MH_BERR
#define IPSEC_POLICY_TYPE_MH (IPSEC_F_MH_BUBA|IPSEC_F_MH_BERR|IPSEC_F_MH)
#define IPSEC_POLICY_TYPE_MOBPFXDISC IPSEC_F_ICMP_MPD
#define IPSEC_POLICY_TYPE_NDISC IPSEC_F_ICMP_ND
#define IPSEC_POLICY_TYPE_ICMP (IPSEC_F_ICMP_MPD|IPSEC_F_ICMP_ND|IPSEC_F_ICMP)
#define IPSEC_POLICY_TYPE_ANY (IPSEC_F_MH_BUBA|IPSEC_F_MH_BERR|IPSEC_F_MH|IPSEC_F_ICMP_MPD|IPSEC_F_ICMP_ND|IPSEC_F_ICMP|IPSEC_F_ANY)

#define IPSEC_POLICY_TYPE_TUNNELHOMETESTING IPSEC_F_TNL_MH_RR
#define IPSEC_POLICY_TYPE_TUNNELMH (IPSEC_F_TNL_MH_RR|IPSEC_F_TNL_MH)
#define IPSEC_POLICY_TYPE_TUNNELPAYLOAD (IPSEC_F_TNL_MH_RR|IPSEC_F_TNL_MH|IPSEC_F_TNL_ANY)

struct ipsec_policy_entry {
	struct list_head list;
	struct in6_addr ha_addr;
	struct in6_addr mn_addr;
	int type;
	int ipsec_protos;
	int action;
	uint32_t reqid_toha;
	uint32_t reqid_tomn;
};

#define IPSEC_PROTO_ESP 0x1
#ifndef MULTIPROTO_MIGRATE
#define IPSEC_PROTO_AH 0
#define IPSEC_PROTO_IPCOMP 0
#else
#define IPSEC_PROTO_AH 0x2
#define IPSEC_PROTO_IPCOMP 0x4
#endif

static inline int ipsec_use_esp(struct ipsec_policy_entry *e)
{
	return e->ipsec_protos & IPSEC_PROTO_ESP;
}

static inline int ipsec_use_ah(struct ipsec_policy_entry *e)
{
	return e->ipsec_protos & IPSEC_PROTO_AH;
}

static inline int ipsec_use_ipcomp(struct ipsec_policy_entry *e)
{
	return e->ipsec_protos & IPSEC_PROTO_IPCOMP;
}

int ipsec_policy_apply(const struct in6_addr *haaddr,
		       const struct in6_addr *hoa,
		       int (* func)(const struct in6_addr *haaddr,
				    const struct in6_addr *hoa,
				    struct ipsec_policy_entry *e, void *arg),
		       void *arg);
int ipsec_policy_walk(int (* func)(const struct in6_addr *haaddr,
				   const struct in6_addr *hoa,
				   struct ipsec_policy_entry *e, void *arg),
		      void *arg);
int ipsec_policy_entry_check(const struct in6_addr *haaddr,
			     const struct in6_addr *hoa,
			     int type);

int ha_ipsec_tnl_update(const struct in6_addr *haaddr,
			const struct in6_addr *hoa,
			const struct in6_addr *coa,
			const struct in6_addr *old_coa,
			int tunnel,
			struct list_head *mnp);

int ha_ipsec_mnp_pol_del(const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr,
			 struct list_head *old_mnps,
			 struct list_head *new_mnps,
			 int tunnel);

int ha_ipsec_mnp_pol_add(const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr,
			 struct list_head *old_mnps,
			 struct list_head *new_mnps,
			 int tunnel);

int ha_ipsec_trns_update(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 const struct in6_addr *coa,
			 const struct in6_addr *old_coa,
			 int tunnel);

int ha_ipsec_tnl_pol_add(const struct in6_addr *our_addr, 
			 const struct in6_addr *peer_addr,
			 int tunnel,
			 struct list_head *mnp);

int ha_ipsec_tnl_pol_del(const struct in6_addr *our_addr, 
			 const struct in6_addr *peer_addr,
			 int tunnel,
			 struct list_head *mnp);

int mn_ipsec_tnl_update(const struct in6_addr *haaddr,
			const struct in6_addr *hoa,
			void *arg);

int mn_ipsec_trns_update(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 void *arg);

int mn_ipsec_tnl_pol_add(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 void *arg);

int mn_ipsec_tnl_pol_del(const struct in6_addr *haaddr,
			 const struct in6_addr *hoa,
			 void *arg);

extern int ipsec_policy_dump_config(const struct in6_addr *haaddr,
				    const struct in6_addr *hoa,
				    struct ipsec_policy_entry *e, void *arg);

#endif	/* __IPSEC_H__ */
