/* $Id: proc_sys.h 1.3 06/01/03 19:42:47+02:00 vnuorval@tcs.hut.fi $ */

#ifndef __PROC_SYS_H__
#define __PROC_SYS_H__ 1

#define PROC_SYS_IP6_AUTOCONF "/proc/sys/net/ipv6/conf/%s/autoconf"
#define PROC_SYS_IP6_ACCEPT_RA "/proc/sys/net/ipv6/conf/%s/accept_ra"
#define PROC_SYS_IP6_ACCEPT_RA_DEFRTR "/proc/sys/net/ipv6/conf/%s/accept_ra_defrtr"
#define PROC_SYS_IP6_ACCEPT_RA_PINFO "/proc/sys/net/ipv6/conf/%s/accept_ra_pinfo"
#define PROC_SYS_IP6_RTR_SOLICITS "/proc/sys/net/ipv6/conf/%s/router_solicitations"
#define PROC_SYS_IP6_RTR_SOLICIT_INTERVAL "/proc/sys/net/ipv6/conf/%s/router_solicitation_interval"
#define PROC_SYS_IP6_LINKMTU "/proc/sys/net/ipv6/conf/%s/mtu"
#define PROC_SYS_IP6_CURHLIM "/proc/sys/net/ipv6/conf/%s/hop_limit"
#define PROC_SYS_IP6_APP_SOLICIT "/proc/sys/net/ipv6/neigh/%s/app_solicit"
#define PROC_SYS_IP6_BASEREACHTIME_MS "/proc/sys/net/ipv6/neigh/%s/base_reachable_time_ms"
#define PROC_SYS_IP6_RETRANSTIMER_MS "/proc/sys/net/ipv6/neigh/%s/retrans_time_ms"
#define PROC_SYS_IP6_FORWARDING "/proc/sys/net/ipv6/conf/%s/forwarding"
#define PROC_SYS_IP6_PROXY_NDP "/proc/sys/net/ipv6/conf/%s/proxy_ndp"

int set_iface_proc_entry(const char *tmpl, const char *if_name, int val);

int get_iface_proc_entry(const char *tmpl, const char *if_name, int *val);

#endif
