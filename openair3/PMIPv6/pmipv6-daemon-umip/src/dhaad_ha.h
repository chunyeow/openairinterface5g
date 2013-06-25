/* $Id: dhaad_ha.h 1.3 05/12/08 19:42:43+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __DHAAD_HA_H__
#define __DHAAD_HA_H__ 1

#define MAX_HOME_AGENTS 77

struct icmp6_hdr;

/* Home Agent functions */

struct ha_interface;
struct nd_opt_prefix_info;

#ifdef ENABLE_VT
void dhaad_halist_iterate(struct ha_interface *iface,
			  int (* func)(int, void *, void *), void *arg);
#endif

void dhaad_insert_halist(struct ha_interface *i, uint16_t key,
			 uint16_t life_sec, uint16_t flags,
			 struct nd_opt_prefix_info *pinfo);

int dhaad_ha_init(void);
void dhaad_ha_cleanup(void);

#endif
