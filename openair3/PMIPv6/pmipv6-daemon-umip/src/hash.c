/*
 * $Id: hash.c 1.22 06/04/25 13:24:14+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Henrik Petander <petander@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include <stdio.h>

#include "debug.h"
#include "hash.h"
#include "util.h"

/* Hash entry */

struct hash_entry {
	struct in6_addr *our_addr; /* pointer to our_addr in data */
	struct in6_addr *peer_addr; /* pointer to peer_addr in data */
	struct hash_entry *next;
	void *data;
};

static uint32_t calc_hash2(int s, const struct in6_addr *our_addr,
			   const struct in6_addr *peer_addr)
{
	assert(our_addr);

	return (our_addr->s6_addr32[0] ^
		our_addr->s6_addr32[1] ^
		our_addr->s6_addr32[2] ^
		our_addr->s6_addr32[3] ^
		peer_addr->s6_addr32[0] ^
		peer_addr->s6_addr32[1] ^
		peer_addr->s6_addr32[2] ^
		peer_addr->s6_addr32[3]) % s;
	
}

static uint32_t calc_hash1(int s,
			   __attribute__ ((unused)) const struct in6_addr *dummy,
			   const struct in6_addr *peer_addr)
{
	return 	(peer_addr->s6_addr32[0] ^
		peer_addr->s6_addr32[1] ^
		peer_addr->s6_addr32[2] ^
		peer_addr->s6_addr32[3]) % s;
	
}

static int match2(struct hash_entry *h, const struct in6_addr *our_addr, 
		  const struct in6_addr *peer_addr)
{
	assert(h);
	assert(our_addr && h->our_addr);
	assert(peer_addr && h->peer_addr);
#ifdef DEBUG
	char s[INET6_ADDRSTRLEN];
	printf("match 2: peer %s\n", inet_ntop(AF_INET6, peer_addr, s, sizeof(s)));
	printf("h->peer %s\n", inet_ntop(AF_INET6, h->peer_addr, s, sizeof(s)));
	printf("our_addr %s\n", inet_ntop(AF_INET6, our_addr, s, sizeof(s)));
	printf("h->our_addr %s\n", inet_ntop(AF_INET6, h->our_addr, s, sizeof(s)));
#endif
	if (IN6_ARE_ADDR_EQUAL(h->peer_addr, peer_addr) &&
	    IN6_ARE_ADDR_EQUAL(h->our_addr, our_addr)) {
		return 1;
	}
	return 0;

}

static int match1(struct hash_entry *h,
		  __attribute__ ((unused)) const struct in6_addr *dummy,
		  const struct in6_addr *peer_addr)
{
	assert(h);
	assert(h->peer_addr && peer_addr);
#ifdef DEBUG
	char s[INET6_ADDRSTRLEN];
	printf("match1: peer %s\n", inet_ntop(AF_INET6, peer_addr, s, sizeof(s)));
	printf("h->peer %s\n", inet_ntop(AF_INET6, h->peer_addr, s, sizeof(s)));
#endif
	if (!IN6_ARE_ADDR_EQUAL(h->peer_addr, peer_addr))
		return 0;
	return 1;
}

/* Caller must have reserved the memory for the hash */
int hash_init(struct hash *h, int type, int buckets)
{
	assert(h);
	assert(type == DOUBLE_ADDR || type == SINGLE_ADDR);
	assert(buckets);
	h->hash_buckets = (struct hash_entry **)malloc(buckets * sizeof(struct hash_entry *));
	if (!h->hash_buckets)
		return -ENOMEM;
	memset(h->hash_buckets, 0, buckets * sizeof(struct hash_entry *));
	h->type = type;
	h->buckets = buckets;

	if (h->type == DOUBLE_ADDR){
		h->calc_hash = calc_hash2;
		h->match = match2;
	} else if (h->type == SINGLE_ADDR) {
		h->calc_hash = calc_hash1;
		h->match = match1;
	} 
	return 0;
}

void hash_cleanup(struct hash *h)
{
	struct hash_entry *he, *tmp;
	int i;
	assert(h);
	
	for(i=0; i < h->buckets; i++) {
		he = h->hash_buckets[i];
		while(he) {
			tmp = he;
			he = he->next;
			free(tmp);
		}
	}
	free(h->hash_buckets);
}

void *hash_get(const struct hash *h,
	       const struct in6_addr *our_addr, 
	       const struct in6_addr *peer_addr)
{
	struct hash_entry *hptr = NULL;
	
	assert(h);
	assert(peer_addr);

	hptr = h->hash_buckets[h->calc_hash(h->buckets, our_addr , peer_addr)];

	while(hptr) {
		if (h->match(hptr, our_addr, peer_addr))
			return hptr->data;
		hptr = hptr->next;
	}
	return NULL;
}

int hash_add(const struct hash *h, void *data,
	     struct in6_addr *our_addr, struct in6_addr *peer_addr)
{
	struct hash_entry *hptr = NULL, *new;
	uint32_t hash_ind;
	
	assert(h); 

	if (h->type == DOUBLE_ADDR)
		assert(our_addr); 
	assert(peer_addr);

	if ((new = (struct hash_entry *)malloc(sizeof(struct hash_entry))) == NULL) {
		dbg("out of memory\n");
		return -ENOMEM;
	}
	new->our_addr = our_addr;
	new->peer_addr = peer_addr;
	new->data = data;
	hash_ind  = h->calc_hash(h->buckets, our_addr, peer_addr);
	hptr = h->hash_buckets[hash_ind];
	if (hptr) {
		while (hptr->next){
			if (h->match(hptr, our_addr, peer_addr)) {
				free(new);
				return -EEXIST;
			}
			hptr = hptr->next;
		}
		hptr->next = new;
	} else {
		h->hash_buckets[hash_ind] = new;
	}
	
	new->next = NULL;
	return 0;
}

int hash_iterate(const struct hash *h, int (*func)(void *, void *), void *arg)
{
	int err = 0;
	int i;
	struct hash_entry *hptr, *nptr;

	assert(func);
	
	for (i=0; i < h->buckets; i++) {
		if((hptr = h->hash_buckets[i]) != NULL) {
			while(hptr) {
				nptr = hptr->next;
				if ((err = func(hptr->data, arg)))
					return err;
				hptr = nptr;
			}
		}
	}
	return 0;
}

void hash_delete(const struct hash *h,
		 const struct in6_addr *our_addr,
		 const struct in6_addr *peer_addr)
{
	struct hash_entry *hptr, *pptr, *head;
	int hash_ind;

	assert(h);
	if (h->type == DOUBLE_ADDR) assert(our_addr);
	assert(peer_addr);

	hash_ind = h->calc_hash(h->buckets, our_addr, peer_addr);
	head = h->hash_buckets[hash_ind];
	
	pptr = hptr = head;
	while (hptr) {
		if (h->match(hptr, our_addr, peer_addr)){
			if (hptr != head) 
				pptr->next = hptr->next;
			else {
				/* First entry */ 
				h->hash_buckets[hash_ind] = hptr->next;
			}
			hptr->data = NULL;
			free(hptr);
			hptr = NULL;
			return;
		}
		pptr = hptr;
		hptr = hptr->next;
	}
	/* Not found */
	return;
}	
