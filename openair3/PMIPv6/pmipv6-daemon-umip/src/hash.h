/* $Id: hash.h 1.9 05/12/08 18:25:09+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __HASH_H__
#define __HASH_H__ 1

/* Types for hash */
#define DOUBLE_ADDR 0
#define SINGLE_ADDR 1

struct hash_entry;
struct in6_addr;

/** Hash structure
 * @buckets : number of buckets in hash 
 * @calc_hash: internal data
 * @match: internal data
 * @hash: array of hashbuckets 
 */
struct hash {
	int buckets;
	int type; 
	uint32_t (*calc_hash)(int, const struct in6_addr *, const struct in6_addr *);
	int (*match)(struct hash_entry *, const struct in6_addr *, const struct in6_addr *);
	struct hash_entry **hash_buckets;
};

/** hash_init - initializes hash 
 * 
 * @h: uninitialized memory with enough space for hash 
 * @type: either double address, or single address
 * @buckets: number of hashbuckets in hash
 */  
int hash_init(struct hash *h, int type, int buckets);

/** hash_cleanup - cleans up hash and all its entries
 *
 */
void hash_cleanup(struct hash *h);
/*
 *  Get data stored in a hash entry based on one or two addresses, depending on type of hash
 */
void *hash_get(const struct hash *h,
	       const struct in6_addr *our_addr, 
	       const struct in6_addr *peer_addr) ;

/* Iterate through the hash and call func for every entry */

int hash_iterate(const struct hash *h, int (*func)(void *data, void *arg), void *arg);

/* Add a hash entry to hash */		 
int hash_add(const struct hash *h, void  *data,
	     struct in6_addr *our_addr, struct in6_addr *peer_addr);

/*
 *Delete entry from hash
 */

void hash_delete(const struct hash *h,
		 const struct in6_addr *our_addr, 
		 const struct in6_addr *peer_addr);

#endif /* _HASH_H */
