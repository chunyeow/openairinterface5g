/* $Id: inet6_rth_gettype.c 1.1 05/03/10 11:32:50+02:00 anttit@tcs.hut.fi $ */

/* This is a substitute for a missing inet6_rth_getaddr(). */

#include <stdint.h>

int inet6_rth_gettype(void *bp)
{
	uint8_t *rthp = (uint8_t *)bp;

	return rthp[2];
}
