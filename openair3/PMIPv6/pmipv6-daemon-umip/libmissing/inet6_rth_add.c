/* $Id: inet6_rth_add.c 1.3 05/03/07 16:49:46+02:00 anttit@tcs.hut.fi $ */

/* This is a substitute for a missing inet6_rth_add(). */

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <string.h>

int inet6_rth_add(void *bp, const struct in6_addr *addr)
{
	struct ip6_rthdr *rth;

	rth = (struct ip6_rthdr *)bp;
	
	memcpy((uint8_t *)bp + 8 + rth->ip6r_segleft * sizeof(struct in6_addr),
	       addr, sizeof(struct in6_addr));

	rth->ip6r_segleft += 1;

	return 0;
}
