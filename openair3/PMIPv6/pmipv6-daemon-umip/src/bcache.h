/* $Id: bcache.h 1.38 06/01/30 18:29:16+02:00 anttit@tcs.hut.fi $ */

#ifndef __BCACHE_H__
#define __BCACHE_H__ 1

#include "tqueue.h"
#include "hash.h"

struct bcentry {
	struct in6_addr our_addr;	/* Address to which we got BU */
	struct in6_addr peer_addr;	/* MN home address */
	struct in6_addr old_coa;	/* Previous care-of address */
	struct in6_addr coa;		/* MN care-of address */
	struct timespec add_time;       /* When was the binding added or modified */
	struct timespec lifetime;      	/* lifetime sent in this BU, in seconds */
	uint16_t seqno;			/* sequence number of the latest BU */
	uint16_t flags;			/* BU flags */

	uint16_t nonce_coa;
	uint16_t nonce_hoa;
	uint16_t type;     		/* Entry type */
	uint16_t nemo_type;    		/* NEMO registration type */
	int unreach;			/* ICMP dest unreach count */
	int tunnel;			/* Tunnel interface index */
	int link;			/* Home link interface index */

	int id;				/* For testing */

	/* Following fields are for internal use only */
	struct timespec br_lastsent;	/* BR ratelimit */
	int br_count;			/* BR ratelimit */

	pthread_rwlock_t lock;		/* Protects the entry */ 
	struct tq_elem tqe;		/* Timer queue entry for expire */

	void (*cleanup)(struct bcentry *bce); /* Clean up bce data */

	struct list_head mob_net_prefixes;
};

#define BCE_NONCE_BLOCK 0
#define BCE_HOMEREG 1
#define BCE_CACHED  2
#define BCE_CACHE_DYING 3
#define BCE_DAD 4

#define BCE_NEMO_EXPLICIT 1
#define BCE_NEMO_IMPLICIT 2
#define BCE_NEMO_DYNAMIC 3

struct bcentry *bcache_alloc(int type);

void bcache_free(struct bcentry *bce);

struct bcentry *bcache_get(const struct in6_addr *our_addr,
			   const struct in6_addr *peer_addr);

int bcache_add(struct bcentry *bce);

int bcache_add_homereg(struct bcentry *bce);
int bcache_complete_homereg(struct bcentry *bce);

int bcache_update_expire(struct bcentry *bce);

void bcache_delete(const struct in6_addr *our_addr,
		   const struct in6_addr *peer_addr);

int bcache_init(void);
void bcache_flush(void);
void bcache_cleanup(void);

int bcache_iterate(int (* func)(void *, void *), void *arg);

void bcache_release_entry(struct bcentry *bce);

int bce_type(const struct in6_addr *our_addr,
	     const struct in6_addr *peer_addr);

static inline int bce_exists(const struct in6_addr *our_addr,
			     const struct in6_addr *peer_addr)
{
	return bce_type(our_addr, peer_addr) >= BCE_NONCE_BLOCK;
}

void dump_bce(void *bce, void *os);

extern pthread_rwlock_t bc_lock; /* Protects binding cache */

#endif
