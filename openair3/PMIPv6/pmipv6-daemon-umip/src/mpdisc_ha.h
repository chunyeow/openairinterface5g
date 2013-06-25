/* $Id: mpdisc_ha.h 1.5 06/03/06 20:43:55+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __MPDISC_HA_H__
#define __MPDISC_HA_H__ 1

#include <netinet/icmp6.h>

struct ha_interface;

#ifdef ENABLE_VT
int mpd_poll_mpa(const struct in6_addr *ha,
		 const struct in6_addr *hoa,
		 struct timespec *delay,
		 struct timespec *lastsent);

void mpd_plist_iterate(struct ha_interface *iface,
		       int (* func)(int, void *, void *), void *arg);
#endif

int mpd_prefix_check(struct in6_addr *dst,
		     struct in6_addr *src,
		     struct timespec *lft,
		     int *ifindex,
		     int dad);

void mpd_handle_mpa_flags(struct ha_interface *iface, uint8_t ra_flags);

int mpd_handle_pinfo(struct ha_interface *iface,
		     struct nd_opt_prefix_info *pinfo);

void mpd_del_expired_pinfos(struct ha_interface *i);

void mpd_cancel_mpa(const struct in6_addr *ha, const struct in6_addr *hoa);
int mpd_start_mpa(const struct in6_addr *ha, const struct in6_addr *hoa);

int mpd_ha_init(void);
void mpd_ha_cleanup(void);

#endif
