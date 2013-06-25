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

#ifndef __ieee80211p_netlinkapi_h__
#define __ieee80211p_netlinkapi_h__

/******************************************************************************
 *
 * Macros
 *
 *****************************************************************************/

#define NL_MAX_PKT_SIZE 2000
#define NETLINK_80211P 17
#define NETLINK_80211P_GROUP 1
#define NLCMD_INIT 1
#define NLCMD_DATA 2
#define NLCMD_SIZE 1

/******************************************************************************
 *
 * Declarations
 *
 *****************************************************************************/

int netlink_init();
int netlink_send(int fd, int cmd,int payloadlen, char *payload);
int netlink_recv(int fd,char *packetpayload);
void netlink_close(int fd);

#endif /* __ieee80211_netlinkapi_h__ */
