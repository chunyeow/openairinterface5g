/* $Id: xfrm.h 1.75 06/05/15 17:36:48+03:00 vnuorval@tcs.hut.fi $ */
#ifndef __XFRM_H__
#define __XFRM_H__

#include <linux/xfrm.h>
#include "list.h"

#define MIP6_PRIO_HOME_ERROR		1
#define MIP6_PRIO_HOME_SIG		2
#define MIP6_PRIO_HOME_SIG_ANY		3
#define MIP6_PRIO_HOME_BLOCK		4
#define MIP6_PRIO_BYPASS_BU		4	/* XXX: BU towards CN */
#define MIP6_PRIO_HOME_DATA_IPSEC	5
#define MIP6_PRIO_HOME_DATA		6
#define MIP6_PRIO_RO_SIG_IPSEC		7	/* XXX: BU between MN-MN with IPsec */
#define MIP6_PRIO_RO_SIG		8	/* XXX: BU between MN-CN */
#define MIP6_PRIO_RO_SIG_ANY		9
#define MIP6_PRIO_MR_LOCAL_DATA_BYPASS	9	/* Bypass rule for local traffic in mobile network */
#define MIP6_PRIO_RO_SIG_RR		10	/* XXX: MH(or HoTI/HoT) between MN-CN */
#define MIP6_PRIO_RO_BLOCK		11
#define MIP6_PRIO_NO_RO_SIG_ANY		12
#define MIP6_PRIO_NO_RO_DATA		13
#define MIP6_PRIO_RO_BULE_BCE_DATA	14
#define MIP6_PRIO_RO_BULE_DATA		15
#define MIP6_PRIO_RO_BCE_DATA		16
#define MIP6_PRIO_RO_TRIG		17
#define MIP6_PRIO_RO_TRIG_ANY		18
#define MIP6_PRIO_RO_DATA_ANY		19

typedef enum {
	MIP6_TYPE_MOVEMENT_UNKNOWN = 0,
	MIP6_TYPE_MOVEMENT_HL2FL,	/* Home to Foreign */
	MIP6_TYPE_MOVEMENT_FL2FL,	/* Foreign to Foreign */
	MIP6_TYPE_MOVEMENT_FL2HL,	/* Foreign to Home */
} movement_t;

struct xfrm_ro_pol {
	struct list_head list;
	struct in6_addr cn_addr;
	int do_ro;     /* 1 for RO, 0 for reverse tunnel */
};

int xfrm_init(void);
void xfrm_cleanup(void);

struct in6_addr;
struct bulentry;
struct ipsec_policy_entry;
struct home_addr_info;

int xfrm_add_bce(const struct in6_addr *our_addr,
		 const struct in6_addr *peer_addr,
		 const struct in6_addr *coa,
		 int replace);

void xfrm_del_bce(const struct in6_addr *our_addr,
		  const struct in6_addr *peer_addr);

int xfrm_pre_bu_add_bule(struct bulentry *bule);
int xfrm_post_ba_mod_bule(struct bulentry *bule);
void xfrm_del_bule(struct bulentry *bule);

long xfrm_last_used(const struct in6_addr *daddr,
		    const struct in6_addr *saddr, 
		    int proto,
		    const struct timespec *now);

long mn_bule_xfrm_last_used(const struct in6_addr *peer, 
			    const struct in6_addr *hoa, 
			    const struct timespec *now);

int mn_ro_pol_add(struct home_addr_info *hai, int ifindex, int changed);
void mn_ro_pol_del(struct home_addr_info *hai, int ifindex, int changed);
int mn_bule_ro_pol_del(void *vbule, void *viif);

int mn_ipsec_recv_bu_tnl_pol_add(struct bulentry *bule, int ifindex,
				 struct ipsec_policy_entry *e);
void mn_ipsec_recv_bu_tnl_pol_del(struct bulentry *bule, int ifindex);

int xfrm_cn_policy_mh_out_touch(int update);

int cn_wildrecv_bu_pol_add(void);
void cn_wildrecv_bu_pol_del(void);

int xfrm_block_link(struct home_addr_info *hai);
void xfrm_unblock_link(struct home_addr_info *hai);

int xfrm_block_hoa(struct home_addr_info *hai);
void xfrm_unblock_hoa(struct home_addr_info *hai);

int xfrm_block_ra(struct home_addr_info *hai);
void xfrm_unblock_ra(struct home_addr_info *hai);

int xfrm_block_fwd(struct home_addr_info *hai);
void xfrm_unblock_fwd(struct home_addr_info *hai);

int ha_mn_ipsec_pol_mod(struct in6_addr *haaddr,
			struct in6_addr *hoa);

int xfrm_ipsec_policy_mod(struct xfrm_userpolicy_info *sp,
			  struct xfrm_user_tmpl *tmpl,
			  int num_tmpl,
			  int cmd);

static inline int pre_bu_bul_update(struct bulentry *bule)
{
	return xfrm_pre_bu_add_bule(bule);
}

static inline int post_ba_bul_update(struct bulentry *bule)
{
	return xfrm_post_ba_mod_bule(bule);
} 

#endif /* __XFRM_H__ */
