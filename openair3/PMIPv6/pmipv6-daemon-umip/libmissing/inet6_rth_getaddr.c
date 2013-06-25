/* $Id: inet6_rth_getaddr.c 1.2 04/02/12 16:16:54+02:00 henkku@mart10.hut.mediapoli.com $ */

/* This is a substitute for a missing inet6_rth_getaddr(). */

#include <stdio.h>
#include <netinet/in.h>

struct in6_addr *inet6_rth_getaddr(const void *bp, int index)
{
	uint8_t *rthp = (uint8_t *)bp;
	struct in6_addr *addr = NULL;

	if (rthp[1] & 1) return NULL;
	if (index < 0 || index > rthp[3]) return NULL;

	addr = (struct in6_addr *)
		(rthp + 8 + index * sizeof(struct in6_addr));

	return addr;
}
