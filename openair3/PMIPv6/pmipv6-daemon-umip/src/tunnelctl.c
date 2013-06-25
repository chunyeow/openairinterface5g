/*
 * $Id: tunnelctl.c 1.44 06/04/25 13:24:14+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Ville Nuorvala <vnuorval@tcs.hut.fi>
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
 * 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <linux/if_tunnel.h>
#include <linux/ip6_tunnel.h>
#include <pthread.h>

#include "debug.h"
#include "hash.h"
#include "list.h"
#include "util.h"
#include "tunnelctl.h"

#define TUNNEL_DEBUG_LEVEL 1

#if TUNNEL_DEBUG_LEVEL >= 1
#define TDBG dbg
#else
#define TDBG(x...)
#endif

const char basedev[] = "ip6tnl0";

static pthread_mutex_t tnl_lock;

static int tnl_fd;

struct mip6_tnl {
	struct list_head list;
	struct ip6_tnl_parm parm;
	int ifindex;
	int users;
};

static inline void tnl_dump(struct mip6_tnl *tnl)
{
	TDBG("name: %s\n"
	     "link: %d\n"
	     "proto: %d\n"
	     "encap_limit: %d\n"
	     "hop_limit: %d\n"
	     "flowinfo: %d\n"
	     "flags: %x\n"
	     "laddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "raddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "ifindex: %d\n"
	     "users: %d\n",
	     tnl->parm.name,
	     tnl->parm.link,
	     tnl->parm.proto,
	     tnl->parm.encap_limit,
	     tnl->parm.hop_limit,
	     tnl->parm.flowinfo,
	     tnl->parm.flags,
	     NIP6ADDR(&tnl->parm.laddr),
	     NIP6ADDR(&tnl->parm.raddr),
	     tnl->ifindex,
	     tnl->users);
}

static inline void tnl_parm_dump(struct ip6_tnl_parm *parm)
{
	TDBG("name: %s\n"
	     "link: %d\n"
	     "proto: %d\n"
	     "encap_limit: %d\n"
	     "hop_limit: %d\n"
	     "flowinfo: %d\n"
	     "flags: %x\n"
	     "laddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "raddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
	     "ifindex: %d\n"
	     "users: %d\n",
	     parm->name,
	     parm->link,
	     parm->proto,
	     parm->encap_limit,
	     parm->hop_limit,
	     parm->flowinfo,
	     parm->flags,
	     NIP6ADDR(&parm->laddr),
	     NIP6ADDR(&parm->raddr));
}

#define TNL_BUCKETS 32

static struct hash tnl_hash;

LIST_HEAD(tnl_list);

static inline struct mip6_tnl *get_tnl(int ifindex)
{
	struct mip6_tnl *tnl = NULL;
	struct list_head *list;
	list_for_each(list, &tnl_list) {
		struct mip6_tnl *tmp;
		tmp = list_entry(list, struct mip6_tnl, list);
		if (tmp->ifindex == ifindex) {
			tnl = tmp;
			break;
		}
	}
	return tnl;
}

static int __tunnel_del(struct mip6_tnl *tnl)
{
	int res = 0;

	tnl->users--;

	TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
	     "to %x:%x:%x:%x:%x:%x:%x:%x user count decreased to %d\n",
	     tnl->parm.name, tnl->ifindex, 
	     NIP6ADDR(&tnl->parm.laddr), NIP6ADDR(&tnl->parm.raddr),
	     tnl->users);

	if (tnl->users == 0) {
		struct ifreq ifr;
		list_del(&tnl->list);
		hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
		strcpy(ifr.ifr_name, tnl->parm.name);
		if ((res = ioctl(tnl_fd, SIOCDELTUNNEL, &ifr)) < 0) {
			TDBG("SIOCDELTUNNEL failed status %d %s\n", 
			     errno, strerror(errno));
			res = -1;
		} else
			TDBG("tunnel deleted\n");
		free(tnl);
	}
	return res;
}

/**
 * tunnel_del - delete tunnel
 * @ifindex: tunnel interface index
 *
 * Deletes a tunnel identified by @ifindex.  Returns negative if
 * tunnel does not exist, otherwise zero.
 **/
int tunnel_del(int ifindex,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data)
{
	struct mip6_tnl *tnl;
	int res;

	pthread_mutex_lock(&tnl_lock);
	if ((tnl = get_tnl(ifindex)) == NULL) {
		TDBG("tunnel %d doesn't exist\n", ifindex);
		res = -1;
	} else {
		if (ext_tunnel_ops &&
		    ext_tunnel_ops(SIOCDELTUNNEL, tnl->ifindex, 0, data) < 0)
			TDBG("ext_tunnel_ops failed\n");

		if ((res = __tunnel_del(tnl)) < 0)
			TDBG("tunnel %d deletion failed\n", ifindex);
	}
	pthread_mutex_unlock(&tnl_lock);
	return res;
}

static struct mip6_tnl *__tunnel_add(struct in6_addr *local,
				     struct in6_addr *remote,
				     int link)
{
	struct mip6_tnl *tnl = NULL;
	struct ifreq ifr;

	if ((tnl = malloc(sizeof(struct mip6_tnl))) == NULL)
		return NULL;

	memset(tnl, 0, sizeof(struct mip6_tnl));
	tnl->users = 1;
	tnl->parm.proto = IPPROTO_IPV6;
	tnl->parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	tnl->parm.hop_limit = 64;
	tnl->parm.laddr = *local;
	tnl->parm.raddr = *remote;
	tnl->parm.link = link;

	strcpy(ifr.ifr_name, basedev);
	ifr.ifr_ifru.ifru_data = (void *)&tnl->parm;
	if (ioctl(tnl_fd, SIOCADDTUNNEL, &ifr) < 0) {
	    TDBG("SIOCADDTUNNEL failed status %d %s\n", 
		 errno, strerror(errno));
	    goto err;
	}
	if (!(tnl->parm.flags & IP6_TNL_F_MIP6_DEV)) {
		TDBG("tunnel exists,but isn't used for MIPv6\n");
		goto err;
	}
	strcpy(ifr.ifr_name, tnl->parm.name);
	if (ioctl(tnl_fd, SIOCGIFFLAGS, &ifr) < 0) {
		TDBG("SIOCGIFFLAGS failed status %d %s\n", 
		     errno, strerror(errno));
		goto err;
	}
	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	if (ioctl(tnl_fd, SIOCSIFFLAGS, &ifr) < 0) {
		TDBG("SIOCSIFFLAGS failed status %d %s\n",
		     errno, strerror(errno));
		goto err;
	}
	if (!(tnl->ifindex = if_nametoindex(tnl->parm.name))) {
		TDBG("no device called %s\n", tnl->parm.name);
		goto err;
	}
	if (hash_add(&tnl_hash, tnl, &tnl->parm.laddr, &tnl->parm.raddr) < 0)
		goto err;
	
	list_add_tail(&tnl->list, &tnl_list);

	TDBG("created tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
	     "to %x:%x:%x:%x:%x:%x:%x:%x user count %d\n",
	     tnl->parm.name, tnl->ifindex, 
	     NIP6ADDR(&tnl->parm.laddr), NIP6ADDR(&tnl->parm.raddr),
	     tnl->users);

	return tnl;
err:
	free(tnl);
	return NULL;
}

/**
 * tunnel_add - add a tunnel
 * @local: local tunnel address
 * @remote: remote tunnel address
 *
 * Create an IP6-IP6 tunnel between @local and @remote.  Returns
 * interface index of the newly created tunnel, or negative on error.
 **/
int tunnel_add(struct in6_addr *local,
	       struct in6_addr *remote,
	       int link,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data)
{
	struct mip6_tnl *tnl;
	int res;

	pthread_mutex_lock(&tnl_lock);
	if ((tnl = hash_get(&tnl_hash, local, remote)) != NULL) { 
		tnl->users++;
		TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
		     "to %x:%x:%x:%x:%x:%x:%x:%x user count increased to %d\n",
		     tnl->parm.name, tnl->ifindex, 
		     NIP6ADDR(local), NIP6ADDR(remote), tnl->users);
	} else {
		if ((tnl = __tunnel_add(local, remote, link)) == NULL) {
			TDBG("failed to create tunnel "
			     "from %x:%x:%x:%x:%x:%x:%x:%x "
			     "to %x:%x:%x:%x:%x:%x:%x:%x\n",
			     NIP6ADDR(local), NIP6ADDR(remote));
			pthread_mutex_unlock(&tnl_lock);
			return -1;
		}
	}
	if (ext_tunnel_ops &&
	    ext_tunnel_ops(SIOCADDTUNNEL, 0, tnl->ifindex, data) < 0) {
		TDBG("ext_tunnel_ops failed\n");
		__tunnel_del(tnl);
		pthread_mutex_unlock(&tnl_lock);
		return -1;
	}
	res = tnl->ifindex;
	pthread_mutex_unlock(&tnl_lock);
	return res;
}

static int __tunnel_mod(struct mip6_tnl *tnl,
			struct in6_addr *local,
			struct in6_addr *remote,
			int link)
{
	struct ip6_tnl_parm parm;
	struct ifreq ifr;

	memset(&parm, 0, sizeof(struct ip6_tnl_parm));
	parm.proto = IPPROTO_IPV6;
	parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	parm.hop_limit = 64;
	parm.laddr = *local;
	parm.raddr = *remote;
	parm.link = link;

	strcpy(ifr.ifr_name, tnl->parm.name);
	ifr.ifr_ifru.ifru_data = (void *)&parm;

	if(ioctl(tnl_fd, SIOCCHGTUNNEL, &ifr) < 0) {
		TDBG("SIOCCHGTUNNEL failed status %d %s\n", 
		     errno, strerror(errno));
		return -1;
	}
	hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
	memcpy(&tnl->parm, &parm, sizeof(struct ip6_tnl_parm));
	if (hash_add(&tnl_hash, tnl, &tnl->parm.laddr, &tnl->parm.raddr) < 0) {
		free(tnl);
		return -1;
	}
	TDBG("modified tunnel iface %s (%d)"
	     "from %x:%x:%x:%x:%x:%x:%x:%x "
	     "to %x:%x:%x:%x:%x:%x:%x:%x\n",
	     tnl->parm.name, tnl->ifindex, NIP6ADDR(&tnl->parm.laddr),
	     NIP6ADDR(&tnl->parm.raddr));
	return tnl->ifindex;
	
}


/**
 * tunnel_mod - modify tunnel
 * @ifindex: tunnel interface index
 * @local: new local address
 * @remote: new remote address
 *
 * Modifies tunnel end-points.  Returns negative if error, zero on
 * success.
 **/
int tunnel_mod(int ifindex,
	       struct in6_addr *local,
	       struct in6_addr *remote,
	       int link,
	       int (*ext_tunnel_ops)(int request, 
				     int old_if, 
				     int new_if,
				     void *data),
	       void *data)
{
	struct mip6_tnl *old, *new;
	int res = -1;

	pthread_mutex_lock(&tnl_lock);

	TDBG("modifying tunnel %d end points with "
	     "from %x:%x:%x:%x:%x:%x:%x:%x "
	     "to %x:%x:%x:%x:%x:%x:%x:%x\n",
	     ifindex, NIP6ADDR(local), NIP6ADDR(remote));

	old = get_tnl(ifindex);
	assert(old != NULL);

	if ((new = hash_get(&tnl_hash, local, remote)) != NULL) {
		if (new != old) {
			new->users++;
			TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
			     "to %x:%x:%x:%x:%x:%x:%x:%x user count "
			     "increased to %d\n",
			     new->parm.name, new->ifindex, 
			     NIP6ADDR(local), NIP6ADDR(remote), new->users);
		}
	} else {
		new = old;

		if (old->users == 1 &&
		    (res = __tunnel_mod(old, local, remote, link)) < 0 &&
		    (new = __tunnel_add(local, remote, link)) == NULL) {
			pthread_mutex_unlock(&tnl_lock);
			return -1;
		}
	}
	if (ext_tunnel_ops &&
	    ext_tunnel_ops(SIOCCHGTUNNEL, 
			   old->ifindex, new->ifindex, data) < 0) {
		TDBG("ext_tunnel_ops failed\n");
		if (old != new)
			__tunnel_del(new);
		pthread_mutex_unlock(&tnl_lock);
		return -1;
	}
	if (old != new)
		__tunnel_del(old);

	res = new->ifindex;
	pthread_mutex_unlock(&tnl_lock);
	return res;

}

int tunnelctl_init(void)
{
	int res = 0;
	pthread_mutexattr_t mattrs;

	if ((tnl_fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
		return -1;

	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&tnl_lock, &mattrs))
		return -1;

	pthread_mutex_lock(&tnl_lock);
	res = hash_init(&tnl_hash, DOUBLE_ADDR, TNL_BUCKETS);
	pthread_mutex_unlock(&tnl_lock);
	return res;
}


static int tnl_cleanup(void *data, __attribute__ ((unused)) void *arg)
{
	struct mip6_tnl *tnl = (struct mip6_tnl *) data;
	list_del(&tnl->list);
	hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
	free(tnl);
	return 0;
}

void tunnelctl_cleanup(void)
{
	pthread_mutex_lock(&tnl_lock);
	hash_iterate(&tnl_hash, tnl_cleanup, NULL);	
	hash_cleanup(&tnl_hash);
	pthread_mutex_unlock(&tnl_lock);
	close(tnl_fd);
}

int tunnel_getusers(int tun_index)
{
    struct mip6_tnl *tnl;
    int usercount = -1;
    pthread_mutex_lock(&tnl_lock);
    if ((tnl = get_tnl(tun_index)) != NULL) usercount = tnl->users;
    pthread_mutex_unlock(&tnl_lock);
    return usercount;
}

