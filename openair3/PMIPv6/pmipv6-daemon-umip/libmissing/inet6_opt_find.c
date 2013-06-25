/* $Id: inet6_opt_find.c 1.4 04/02/23 18:46:18+02:00 vnuorval@dsl-hkigw1c53.dial.inet.fi $ */

/* This is a substitute for a missing inet6_opt_find(). */

#include <stddef.h>
#include <netinet/in.h>

#ifndef IP6OPT_PAD1
#define IP6OPT_PAD1 0
#endif

int inet6_opt_find(void *extbuf, socklen_t extlen, int offset, 
		   uint8_t type, socklen_t *lenp,
		   void **databufp)
{
	uint8_t *optp, *tailp;

	optp = (uint8_t *)extbuf;

	if (extlen < 2 || extlen <= offset || extlen < ((optp[1] + 1) << 3))
		return -1;

	tailp = optp + extlen;
	optp += (2 + offset);

	while (optp <= tailp) {
		if (optp[0] == IP6OPT_PAD1) {
			optp++;
			continue;
		}
		if (optp + optp[1] + 2 > tailp)
			return -1;
		if (optp[0] == type) {
			*databufp = optp + 2;
			*lenp = optp[1];
			return *lenp + (uint8_t *)optp - (uint8_t *)extbuf;
		}
		optp += (2 + optp[1]);
	}

	*databufp = NULL;
	return -1;
}
