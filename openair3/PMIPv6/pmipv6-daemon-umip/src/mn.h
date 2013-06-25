/* $Id: mn.h 1.85 06/05/15 13:45:42+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __MN_H__
#define __MN_H__ 1

#include <net/if.h>
#include <netinet/in.h>
#include "list.h"
#include "hash.h"
#include "prefix.h"
#include "tqueue.h"

#define DEREG_BU_LIFETIME               420
extern const struct timespec dereg_bu_lifetime_ts;
#define DEREG_BU_LIFETIME_TS dereg_bu_lifetime_ts

#define BU_REFRESH_DELAY                95/100
#define MPS_REFRESH_DELAY               9/10
#define MN_RR_BEFORE_EXPIRE             1 /* second */
#define MN_TEST_INIT_DELAY		5 /* XXX: second */

#define	IFA_F_HOMEADDRESS_NODAD	(IFA_F_HOMEADDRESS | IFA_F_NODAD)

#define NON_MIP_CN_LTIME                420 /* s */
extern const struct timespec non_mip_cn_ltime_ts;
#define NON_MIP_CN_LTIME_TS non_mip_cn_ltime_ts

#define MN_BE_TIME_THRESHOLD            10 /* last use of binding at
					    * which we still delete a
					    * bule in response to BE,
					    * in seconds */
#define MN_RO_RESTART_THRESHOLD         10 /* s */
#define MIN_VALID_BU_LIFETIME           4 /* seconds */
extern const struct timespec min_valid_bu_lifetime_ts;
#define MIN_VALID_BU_LIFETIME_TS min_valid_bu_lifetime_ts

struct ha_candidate_list {
	struct list_head home_agents;
	struct tq_elem tqe;
	struct timespec dhaad_delay;
	struct in6_addr last_ha;
	int dhaad_resends;
	int dhaad_id;
	int if_block;
	pthread_mutex_t c_lock;
};

#define	HOME_LINK_BLOCK	0x01
#define	HOME_ADDR_BLOCK	0x02
#define	HOME_ADDR_RULE_BLOCK	0x04
#define	NEMO_RA_BLOCK	0x08
#define	NEMO_FWD_BLOCK	0x10

struct mn_addr {
	struct in6_addr addr;
	int iif;
	struct timespec timestamp;
	struct timespec valid_time;
	struct timespec preferred_time;
};

#define HOME_REG_NONE 0
#define HOME_REG_UNCERTAIN 1
#define HOME_REG_VALID 2

struct home_addr_info {
	struct list_head list;
	struct mn_addr hoa; /* Home address */
	uint8_t plen; 
	uint8_t home_reg_status;
	uint8_t home_block;
	uint8_t use_dhaad;
	uint16_t lladdr_comp;
	uint8_t at_home;
	uint8_t home_plen;
	struct in6_addr home_prefix;
	struct hash bul; /* Binding Update List */
	struct mn_addr primary_coa;
	struct list_head ro_policies;
	struct ha_candidate_list ha_list;	
	struct in6_addr ha_addr;
	int pend_ba;
	int verdict;
	int if_tunnel;
	int if_home;
	int if_block;
	uint8_t altcoa;
	uint16_t mob_rtr;
	char name[IF_NAMESIZE];
	int mnp_count;
	struct list_head mob_net_prefixes;
};

enum {
	MN_HO_NONE,
	MN_HO_INVALIDATE,
	MN_HO_IGNORE,
	MN_HO_PROCEED,
	MN_HO_REESTABLISH,
	MN_HO_CHECK_LIFETIME,
	MN_HO_RETURN_HOME
};

static inline int movement_ho_verdict(int verdict)
{
	return verdict == MN_HO_PROCEED;
}

static inline int positive_ho_verdict(int verdict)
{
	switch (verdict) {
	case MN_HO_PROCEED:
	case MN_HO_REESTABLISH:
	case MN_HO_CHECK_LIFETIME:
	case MN_HO_RETURN_HOME:
		return 1;
	}
	return 0;
}

int mn_init(void);
void mn_cleanup(void);

/* Protects both bul and homelink structures in mn */
extern pthread_rwlock_t mn_lock;

struct home_addr_info *mn_get_home_addr(const struct in6_addr *haddr);
/* Interface to configuration system */

struct bulentry;

void mn_send_cn_bu(struct bulentry *bule);

struct home_addr_info *mn_get_home_addr_by_dhaadid(uint16_t dhaad_id);

struct movement_event;

/* Interface to movement detection */
int mn_movement_event(struct movement_event *event);

struct nd_opt_prefix_info;
int mn_update_home_prefix(struct home_addr_info *hai,
			  const struct timespec *mps_sent,
			  const struct nd_opt_prefix_info *p);

struct ifaddrmsg;
struct rtattr;
int mn_addr_changed(int add, struct ifaddrmsg *ifa, struct rtattr **rta_tb);

int mn_lladdr_dad(struct ifaddrmsg *ifa, struct rtattr *rta_tb[], void *arg);

int mn_rr_start_handoff(void *vbule, void *vcoa);

int mn_rr_post_home_handoff(void *bule, void *vcoa);

void mn_start_ro(struct in6_addr *cn_addr, struct in6_addr *home_addr);

static inline int mn_is_at_home(struct list_head *prefixes,
				const struct in6_addr *home_prefix,
				int home_plen)
{
	return prefix_list_find(prefixes, home_prefix, home_plen);
}


#endif /* __MN_H__ */
