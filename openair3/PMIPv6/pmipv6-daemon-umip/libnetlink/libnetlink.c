/*
 * libnetlink.c	RTnetlink service routines.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/uio.h>

#include "libnetlink.h"

#define SO_SNDBUF_SIZE 32768
#define SO_RCVBUF_SIZE 32768
#define NL_DUMP_SIZE 16384
#define NL_TALK_SIZE 16384
#define NL_LISTEN_SIZE 8192

#undef DEBUG

#ifdef DEBUG
#define NLDBG(format, arg...) fprintf(stderr, format, ##arg)
#define NLDBG_SYS(message) perror(message)
#else
#define NLDBG(format, arg...)
#define NLDBG_SYS(message)
#endif

void rtnl_close(struct rtnl_handle *rth)
{
	if (rth->fd >= 0) {
		close(rth->fd);
		rth->fd = -1;
	}
}

int rtnl_open_byproto(struct rtnl_handle *rth, unsigned subscriptions,
		      int protocol)
{
	socklen_t addr_len;
	int sndbuf = SO_SNDBUF_SIZE;
	int rcvbuf = SO_RCVBUF_SIZE;

	memset(rth, 0, sizeof(rth));

	rth->fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (rth->fd < 0) {
		NLDBG_SYS("Cannot open netlink socket");
		return -1;
	}

	if (setsockopt(rth->fd,SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf)) < 0) {
		NLDBG_SYS("SO_SNDBUF");
		return -1;
	}

	if (setsockopt(rth->fd,SOL_SOCKET,SO_RCVBUF,&rcvbuf,sizeof(rcvbuf)) < 0) {
		NLDBG_SYS("SO_RCVBUF");
		return -1;
	}

	memset(&rth->local, 0, sizeof(rth->local));
	rth->local.nl_family = AF_NETLINK;
	rth->local.nl_groups = subscriptions;

	if (bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local)) < 0) {
		NLDBG_SYS("Cannot bind netlink socket");
		return -1;
	}
	addr_len = sizeof(rth->local);
	if (getsockname(rth->fd, (struct sockaddr*)&rth->local, &addr_len) < 0) {
		NLDBG_SYS("Cannot getsockname");
		return -1;
	}
	if (addr_len != sizeof(rth->local)) {
		NLDBG("Wrong address length %d\n", addr_len);
		return -1;
	}
	if (rth->local.nl_family != AF_NETLINK) {
		NLDBG("Wrong address family %d\n", rth->local.nl_family);
		return -1;
	}
	rth->seq = time(NULL);
	return 0;
}

int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions)
{
	return rtnl_open_byproto(rth, subscriptions, NETLINK_ROUTE);
}

int rtnl_wilddump_request(struct rtnl_handle *rth, int family, int type)
{
	struct {
		struct nlmsghdr nlh;
		struct rtgenmsg g;
	} req;
	struct sockaddr_nl nladdr;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	memset(&req, 0, sizeof(req));
	req.nlh.nlmsg_len = sizeof(req);
	req.nlh.nlmsg_type = type;
	req.nlh.nlmsg_flags = NLM_F_ROOT|NLM_F_MATCH|NLM_F_REQUEST;
	req.nlh.nlmsg_pid = 0;
	req.nlh.nlmsg_seq = rth->dump = ++rth->seq;
	req.g.rtgen_family = family;

	return sendto(rth->fd, (void*)&req, sizeof(req), 0,
		      (struct sockaddr*)&nladdr, sizeof(nladdr));
}

int rtnl_dump_request(struct rtnl_handle *rth, int type, void *req, int len)
{
	struct nlmsghdr nlh;
	struct sockaddr_nl nladdr;
	struct iovec iov[2] = {
		{ .iov_base = &nlh, .iov_len = sizeof(nlh) },
		{ .iov_base = req, .iov_len = len }
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = 	sizeof(nladdr),
		.msg_iov = iov,
		.msg_iovlen = 2,
	};

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	nlh.nlmsg_len = NLMSG_LENGTH(len);
	nlh.nlmsg_type = type;
	nlh.nlmsg_flags = NLM_F_ROOT|NLM_F_MATCH|NLM_F_REQUEST;
	nlh.nlmsg_pid = 0;
	nlh.nlmsg_seq = rth->dump = ++rth->seq;

	return sendmsg(rth->fd, &msg, 0);
}

int rtnl_dump_filter(struct rtnl_handle *rth,
		     rtnl_filter_t filter,
		     void *arg1,
		     rtnl_filter_t junk,
		     void *arg2)
{
	struct sockaddr_nl nladdr;
	struct iovec iov;
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	char buf[NL_DUMP_SIZE];

	iov.iov_base = buf;
	while (1) {
		int status;
		struct nlmsghdr *h;

		iov.iov_len = sizeof(buf);
		status = recvmsg(rth->fd, &msg, 0);

		if (status < 0) {
			if (errno == EINTR)
				continue;
			NLDBG_SYS("OVERRUN");
			continue;
		}

		/* Everyone can send empty messages which will led to
		 * status == 0. Before checking if status == 0, check
		 * the origin. Here, we only allow messages from kernel.
		 * --arno */
		if (nladdr.nl_pid != 0) {
			NLDBG("Dropping non-kernel Netlink message.\n");
			continue;
		}

		if (status == 0) {
			NLDBG("EOF on netlink\n");
			return -1;
		}

		h = (struct nlmsghdr*)buf;
		while (status > 0 && NLMSG_OK(h, (size_t)status)) {
			int err;

			if (nladdr.nl_pid != 0 ||
			    h->nlmsg_pid != rth->local.nl_pid ||
			    h->nlmsg_seq != rth->dump) {
				if (junk) {
					err = junk(&nladdr, h, arg2);
					if (err < 0)
						return err;
				}
				goto skip_it;
			}

			if (h->nlmsg_type == NLMSG_DONE)
				return 0;
			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
				if (h->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
					NLDBG("ERROR truncated\n");
				} else {
					errno = -err->error;
					NLDBG_SYS("RTNETLINK answers");
				}
				return -1;
			}
			err = filter(&nladdr, h, arg1);
			if (err < 0)
				return err;

skip_it:
			h = NLMSG_NEXT(h, status);
		}
		if (msg.msg_flags & MSG_TRUNC) {
			NLDBG("Message truncated\n");
			continue;
		}
		if (status) {
			NLDBG("!!!Remnant of size %d\n", status);
			return -2;
		}
	}
}

int rtnl_talk(struct rtnl_handle *rtnl, struct nlmsghdr *n, pid_t peer,
	      unsigned groups, struct nlmsghdr *answer,
	      rtnl_filter_t junk,
	      void *jarg)
{
	int status;
	unsigned seq;
	struct nlmsghdr *h;
	struct sockaddr_nl nladdr;
	struct iovec iov = {
		.iov_base = (void*) n,
		.iov_len = n->nlmsg_len
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	char   buf[NL_TALK_SIZE];

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = peer;
	nladdr.nl_groups = groups;

	n->nlmsg_seq = seq = ++rtnl->seq;

	if (answer == NULL)
		n->nlmsg_flags |= NLM_F_ACK;

	status = sendmsg(rtnl->fd, &msg, 0);

	if (status < 0) {
		NLDBG_SYS("Cannot talk to rtnetlink");
		return -1;
	}

	memset(buf,0,sizeof(buf));

	iov.iov_base = buf;

	while (1) {
		iov.iov_len = sizeof(buf);
		status = recvmsg(rtnl->fd, &msg, 0);

		if (status < 0) {
			if (errno == EINTR)
				continue;
			NLDBG_SYS("OVERRUN");
			continue;
		}

		if (msg.msg_namelen != sizeof(nladdr)) {
			NLDBG("sender address length == %d\n", msg.msg_namelen);
			return -2;
		}
		/* Everyone can send empty messages which will led to
		 * status == 0. Before checking if status == 0, check
		 * the origin. --arno */
		if (nladdr.nl_pid != (__u32)peer) {
			NLDBG("Received Netlink message from unknown peer.\n");
			continue;
		}

		if (status == 0) {
			NLDBG("EOF on netlink\n");
			return -1;
		}

		for (h = (struct nlmsghdr*)buf;
		     status > 0 && (size_t)status >= sizeof(*h); ) {
			int err;
			int len = h->nlmsg_len;
			int l = len - sizeof(*h);

			if (l<0 || len>status) {
				if (msg.msg_flags & MSG_TRUNC) {
					NLDBG("Truncated message\n");
					return -1;
				}
				NLDBG("!!!malformed message: len=%d\n", len);
				return -2;
			}

			if (nladdr.nl_pid != (__u32)peer ||
			    h->nlmsg_pid != rtnl->local.nl_pid ||
			    h->nlmsg_seq != seq) {
				if (junk) {
					err = junk(&nladdr, h, jarg);
					if (err < 0)
						return err;
				}
				/* Don't forget to skip that message. */
				status -= NLMSG_ALIGN(len);
				h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
				continue;
			}

			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
				if ((size_t)l < sizeof(struct nlmsgerr)) {
					NLDBG("ERROR truncated\n");
				} else {
					errno = -err->error;
					if (errno == 0) {
						if (answer)
							memcpy(answer, h, h->nlmsg_len);
						return 0;
					}
					NLDBG_SYS("RTNETLINK answers");
				}
				return -1;
			}
			if (answer) {
				memcpy(answer, h, h->nlmsg_len);
				return 0;
			}

			NLDBG("Unexpected reply!!!\n");

			status -= NLMSG_ALIGN(len);
			h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
		}
		if (msg.msg_flags & MSG_TRUNC) {
			NLDBG("Message truncated\n");
			continue;
		}
		if (status) {
			NLDBG("!!!Remnant of size %d\n", status);
			return -2;
		}
	}
}

int rtnl_listen(struct rtnl_handle *rtnl,
		rtnl_filter_t handler,
		void *jarg)
{
	int status;
	struct nlmsghdr *h;
	struct sockaddr_nl nladdr;
	struct iovec iov;
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	char   buf[NL_LISTEN_SIZE];

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;

	iov.iov_base = buf;
	while (1) {
		iov.iov_len = sizeof(buf);
		status = recvmsg(rtnl->fd, &msg, 0);

		if (status < 0) {
			if (errno == EINTR)
				continue;
			NLDBG_SYS("OVERRUN");
			continue;
		}
		if (msg.msg_namelen != sizeof(nladdr)) {
			NLDBG("Sender address length == %d\n", msg.msg_namelen);
			return -2;
		}
		/* Everyone can send empty messages which will led to
		 * status == 0. Before checking if status == 0, check
		 * the origin. Here, we only allow messages from kernel.
		 * --arno */
		if (nladdr.nl_pid != 0) {
			NLDBG("Dropping non-kernel Netlink message.\n");
			continue;
		}

		if (status == 0) {
			NLDBG("EOF on netlink\n");
			return -1;
		}
		for (h = (struct nlmsghdr*)buf;
		     status > 0 && (size_t)status >= sizeof(*h); ) {
			int err;
			int len = h->nlmsg_len;
			int l = len - sizeof(*h);

			if (l<0 || len>status) {
				if (msg.msg_flags & MSG_TRUNC) {
					NLDBG("Truncated message\n");
					return -1;
				}
				NLDBG("!!!malformed message: len=%d\n", len);
				return -2;
			}

			err = handler(&nladdr, h, jarg);
			if (err < 0)
				return err;

			status -= NLMSG_ALIGN(len);
			h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
		}
		if (msg.msg_flags & MSG_TRUNC) {
			NLDBG("Message truncated\n");
			continue;
		}
		if (status) {
			NLDBG("!!!Remnant of size %d\n", status);
			return -2;
		}
	}
}

int addattr32(struct nlmsghdr *n, int maxlen, int type, __u32 data)
{
	int len = RTA_LENGTH(4);
	struct rtattr *rta;
	if (NLMSG_ALIGN(n->nlmsg_len) + len > (unsigned int)maxlen) {
		NLDBG("addattr32: Error! max allowed bound %d exceeded\n",maxlen);
		return -1;
	}
	rta = NLMSG_TAIL(n);
	rta->rta_type = type;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), &data, 4);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + len;
	return 0;
}

int addattr_l(struct nlmsghdr *n, int maxlen, int type, const void *data,
	      int alen)
{
	int len = RTA_LENGTH(alen);
	struct rtattr *rta;

	if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > (unsigned int)maxlen) {
		NLDBG("addattr_l ERROR: message exceeded bound of %d\n",maxlen);
		return -1;
	}
	rta = NLMSG_TAIL(n);
	rta->rta_type = type;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), data, alen);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
	return 0;
}

int addraw_l(struct nlmsghdr *n, int maxlen, const void *data, int len)
{
	if (NLMSG_ALIGN(n->nlmsg_len) + NLMSG_ALIGN(len) > (unsigned int)maxlen) {
		NLDBG("addraw_l ERROR: message exceeded bound of %d\n",maxlen);
		return -1;
	}

	memcpy(NLMSG_TAIL(n), data, len);
	memset((void *) NLMSG_TAIL(n) + len, 0, NLMSG_ALIGN(len) - len);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + NLMSG_ALIGN(len);
	return 0;
}

int rta_addattr32(struct rtattr *rta, int maxlen, int type, __u32 data)
{
	int len = RTA_LENGTH(4);
	struct rtattr *subrta;

	if (RTA_ALIGN(rta->rta_len) + len > maxlen) {
		NLDBG("rta_addattr32: Error! max allowed bound %d exceeded\n",maxlen);
		return -1;
	}
	subrta = (struct rtattr*)(((char*)rta) + RTA_ALIGN(rta->rta_len));
	subrta->rta_type = type;
	subrta->rta_len = len;
	memcpy(RTA_DATA(subrta), &data, 4);
	rta->rta_len = NLMSG_ALIGN(rta->rta_len) + len;
	return 0;
}

int rta_addattr_l(struct rtattr *rta, int maxlen, int type,
		  const void *data, int alen)
{
	struct rtattr *subrta;
	int len = RTA_LENGTH(alen);

	if (RTA_ALIGN(rta->rta_len) + RTA_ALIGN(len) > maxlen) {
		NLDBG("rta_addattr_l: Error! max allowed bound %d exceeded\n",maxlen);
		return -1;
	}
	subrta = (struct rtattr*)(((char*)rta) + RTA_ALIGN(rta->rta_len));
	subrta->rta_type = type;
	subrta->rta_len = len;
	memcpy(RTA_DATA(subrta), data, alen);
	rta->rta_len = NLMSG_ALIGN(rta->rta_len) + RTA_ALIGN(len);
	return 0;
}

int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max)
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len) {
		NLDBG("!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	}
	return 0;
}

int parse_rtattr_byindex(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	int i = 0;

	memset(tb, 0, sizeof(struct rtattr *) * max);
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max && i < max)
			tb[i++] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len) {
		NLDBG("!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	}
	return i;
}
