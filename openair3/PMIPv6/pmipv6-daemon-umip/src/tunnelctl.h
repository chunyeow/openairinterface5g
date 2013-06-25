/* $Id: tunnelctl.h 1.5 06/01/11 10:16:53+02:00 vnuorval@tcs.hut.fi $ */
#ifndef __TUNNELCTL_H__
#define __TUNNELCTL_H__ 1

int tunnel_add(struct in6_addr *local,
	       struct in6_addr *remote,
	       int link,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data);

int tunnel_mod(int ifindex,
	       struct in6_addr *local,
	       struct in6_addr *remote,
	       int link,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data);

int tunnel_del(int ifindex,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data);


int tunnelctl_init(void);

void tunnelctl_cleanup(void);

int tunnel_getusers(int tun_index);
#endif
