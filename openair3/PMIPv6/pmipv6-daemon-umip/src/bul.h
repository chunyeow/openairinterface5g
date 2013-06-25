/* $Id: bul.h 1.64 06/05/15 13:41:12+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __BUL_H__
#define __BUL_H__ 1

#include <netinet/in.h>

#include "mipv6.h"
#include "tqueue.h"
#include "hash.h"
#include "list.h"

struct home_addr_info;

struct retrout_info {
	int state;
	uint16_t co_ni;
	uint16_t ho_ni;
};

struct addr_holder {
	struct list_head list;
	struct in6_addr addr;
};

struct bulentry {
	struct home_addr_info *home;    /* Pointer to home_address structure */
					/* to which this entry belongs to */
	struct tq_elem tqe;             /* Timer queue entry */
	struct in6_addr peer_addr;      /* CN / HA address */
	struct in6_addr hoa;
	struct in6_addr coa;		/* care-of address of the sent BU */
	int if_coa;
	int type;                       /* BUL / NON_MIP_CN / UNREACH  */
	uint16_t seq;			/* sequence number of the latest BU */
	uint16_t flags;			/* BU send flags */
	struct in6_addr last_coa;        /* Last good coa */      
	struct timespec lastsent;
	struct timespec lifetime;      	/* lifetime sent in this BU */
	struct timespec delay;		/* call back time in ms*/
	struct timespec expires;        /* Absolute time for timer expire */
	struct timespec hard_expire;    /* Absolute bulentry expiry time */
	int consecutive_resends;	/* Number of consecutive BU's resent */
	int8_t coa_changed;
	uint8_t wait_ack;      		/* WAIT / READY */
	uint8_t xfrm_state;
	uint8_t use_alt_coa;            /* Whether to use alt. CoA option */
	uint8_t dereg;                  /* for calculating BSA key */
	uint8_t do_send_bu;             /* send bu / not send bu */

	/* Information for return routability */
	struct retrout_info rr;
	uint8_t Kbm[HMAC_SHA1_KEY_SIZE];

	void (* callback)(struct tq_elem *);
	void (*ext_cleanup)(struct bulentry *);
};	





/* Types for bulentry */
enum {
	BUL_ENTRY,
	NON_MIP_CN_ENTRY,
	UNREACH_ENTRY,
};

/* RR states */
enum {
	RR_READY,
	RR_STARTED,
	RR_NOT_STARTED,
	RR_H_EXPIRED,
	RR_C_EXPIRED,
	RR_EXPIRED,
	RR_NON_MIP_CN
};

/* Types of xfrm_states */
#define BUL_XFRM_STATE_SIG 0x1
#define BUL_XFRM_STATE_DATA 0x2

struct bulentry *bul_get(struct home_addr_info *hinfo,
			 const struct in6_addr *our_addr,
			 const struct in6_addr *peer_addr);

int bul_add(struct bulentry *bule);

void bul_update(struct bulentry *bule);
void bul_delete(struct bulentry *bule);
void bul_update_timer(struct bulentry *bule);
void bul_update_expire(struct bulentry *bule);

int bul_iterate(struct hash *h, int (* func)(void *bule, void *arg), void *arg);

int bul_init(void);
int bul_home_init(struct home_addr_info *home);
void bul_home_cleanup(struct hash *bul);
void bul_flush(void);
void bul_cleanup(void);
void dump_bule(void *bule, void *os);
struct bulentry *create_bule(const struct in6_addr *hoa,
			     const struct in6_addr *cn_addr);
void free_bule(struct bulentry *bule);

#endif
