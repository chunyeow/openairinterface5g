/******************************************************************************
 *
 * Copyright(c) EURECOM / Thales Communications & Security
 *
 * Portions of this file are derived from Matthias Ihmig source code.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * Thales Communications & Security <philippe.agostini@thalesgroup.com>
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Includes
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "ieee80211p-netlinkapi.h"

/******************************************************************************
 *
 * Routines
 *
 *****************************************************************************/

int netlink_init() {

	int fd, ret;
    struct sockaddr_nl s_nladdr;
    
    fd = socket(AF_NETLINK,SOCK_RAW,NETLINK_80211P);
    
    if (fd < 0) {
		printf("ERROR: socket() failed\n");
		return -1;
	}
	
   	/* Source address => from user space */
	memset(&s_nladdr, 0 ,sizeof(s_nladdr));	
	s_nladdr.nl_family= AF_NETLINK ;
   	s_nladdr.nl_pad=0;
	s_nladdr.nl_pid = getpid();
	printf("ieee80211 netlink_init(): app pid = %u\n",s_nladdr.nl_pid);
   	s_nladdr.nl_groups = NETLINK_80211P_GROUP;

   	ret = bind(fd, (struct sockaddr*)&s_nladdr, sizeof(s_nladdr));

	if (ret < 0) {
		printf("netlink_init(): bind failed\n");
		return -1;
	}

	return fd;
}

int netlink_send(int fd, int cmd,int payloadlen, char *payload) {
	
	struct sockaddr_nl d_nladdr;
	struct nlmsghdr *nlh =  NULL;
	char *data;
	struct iovec iov;
	struct msghdr msg;
	int ret;

	/* Destination address ⁼> to kernel space */
	memset(&d_nladdr, 0 ,sizeof(d_nladdr));		
	d_nladdr.nl_family= AF_NETLINK ;
   	d_nladdr.nl_pad=0;
   	d_nladdr.nl_pid = 0;
   	d_nladdr.nl_groups = NETLINK_80211P_GROUP;

	/* Netlink message building */
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(payloadlen)+NLMSG_HDRLEN+NLCMD_SIZE);
	if (nlh == NULL) {
		printf("netlink_send(): netlink header memory not allocated\n");
		return -1;
	}
	memset(nlh,0,NLMSG_SPACE(payloadlen)+NLMSG_HDRLEN+NLCMD_SIZE);
	
	/* Header */
	nlh->nlmsg_len = NLMSG_SPACE(payloadlen+NLCMD_SIZE);
	nlh->nlmsg_pid = getpid();
	printf("ieee80211 netlink_send(): sending app pid = %u\n",nlh->nlmsg_pid);
	nlh->nlmsg_flags = NLM_F_REQUEST;
	nlh->nlmsg_type = 21;
	
	data = (char *)NLMSG_DATA(nlh);
	/* Netlink command */
	data[0] = cmd;
	/* Payload */
	memcpy(&data[1],payload,payloadlen);
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	memset(&msg,0,sizeof(msg));
	msg.msg_name = (void *)&d_nladdr;
	msg.msg_namelen = sizeof(d_nladdr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;	

	/* Send the msg through the netlink socket */	
	ret = sendmsg(fd,&msg,0);

	/* Free the netlink msg */	
	free(nlh);

	return ret;		

}

int netlink_recv(int fd,char *packetpayload) {

	struct sockaddr_nl d_nladdr;
	struct nlmsghdr *nlh =  NULL;
	struct iovec iov;
	struct msghdr msg;
	int packetpayloadlen = 0;
	int ret;

	/* Destination address ⁼> to user space */
	memset(&d_nladdr, 0 ,sizeof(d_nladdr));		
	
	d_nladdr.nl_family= AF_NETLINK;
   	d_nladdr.nl_groups = NETLINK_80211P_GROUP;

	/* Netlink message building */
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(NL_MAX_PKT_SIZE)+NLMSG_HDRLEN);
	
	if (nlh == NULL) {
		printf("netlink_recv(): netlink header memory not allocated\n");
		return -1;
	}
	
	memset(nlh,0,NLMSG_SPACE(NL_MAX_PKT_SIZE)+NLMSG_HDRLEN);

	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(NL_MAX_PKT_SIZE);

	memset(&msg,0,sizeof(msg));
	msg.msg_name = (void *)&d_nladdr;
	msg.msg_namelen = sizeof(d_nladdr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	/* Receive the msg from the netlink socket */
	/* Blocking call */
	ret = recvmsg(fd,&msg,0);
	
	if (ret < 0) {
		printf("netlink_recv(): error in recvmsg\n");
		return -1;
	}
	
	packetpayloadlen = nlh->nlmsg_len-NLMSG_HDRLEN;
	
	if (packetpayloadlen < 0) {
		printf("netlink_recv(): error in recvmsg\n");
		return -1;
	}
		
	memcpy(packetpayload,NLMSG_DATA(nlh),packetpayloadlen);	

	free(nlh);
	
	return packetpayloadlen;
		
}

void netlink_close(int fd) {
	close(fd);
}
