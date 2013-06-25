/* $Id: retrout.h 1.46 06/01/31 23:48:23+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __RETROUT_H__
#define __RETROUT_H__ 1

#include <stdio.h>

struct bulentry;

void mn_rr_refresh(struct bulentry *bule);

void mn_rr_force_refresh(struct bulentry *bule);

int rr_init(void);

void mn_rr_delete_co(struct in6_addr *coa);
void mn_rr_delete_bule(struct bulentry *bule);

int mn_rr_error_check(const struct in6_addr *peer,
		      const struct in6_addr *own,
		      struct in6_addr *hoa);

void rrl_dump(FILE *os);

void rr_cleanup(void);

#endif
