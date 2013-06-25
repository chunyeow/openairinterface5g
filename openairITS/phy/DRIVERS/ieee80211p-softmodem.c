/******************************************************************************
 *
 * Copyright(c) EURECOM / Thales Communications & Security
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
#include <unistd.h>
#include <stdlib.h>
#include "ieee80211p-netlinkapi.h"

/******************************************************************************
 *
 * Definitions
 *
 *****************************************************************************/

enum nl80211_band {
	NL80211_BAND_2GHZ,
	NL80211_BAND_5GHZ,
	NL80211_BAND_5_9GHZ,
	NL80211_BAND_0_8GHZ,
};

enum ieee80211_band {
	IEEE80211_BAND_2GHZ = NL80211_BAND_2GHZ,
	IEEE80211_BAND_5GHZ = NL80211_BAND_5GHZ,
	IEEE80211_BAND_5_9GHZ = NL80211_BAND_5_9GHZ,
	IEEE80211_BAND_0_8GHZ = NL80211_BAND_0_8GHZ,
};

struct ieee80211p_rx_status {
	short	data_len;	//frame data length in bytes
	char	rssi; 		//received power in dBm
	char	rate; 		//reveived data rate in units of 100 kbps
	enum ieee80211_band band;
	char	flags; 		//RX flags
}; /* struct ieee80211p_rx_status */

/******************************************************************************
 *
 * Main
 *
 *****************************************************************************/

int main() {

	int fd;
	int ret;
	/* txdata = data received over the air and to be sent to the stack */
	/* txdata = rx_status (12 bytes) + ICMP echo request (116 bytes) */
	char txdata[128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x21, 0x22, 0x23, 0x24, 0x25, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01, 0xb6, 0x57, 0xc0, 0xa8, 0x01, 0x02, 0xc0, 0xa8, 0x01, 0xff, 0x08, 0x00, 0xa6, 0x7e, 0x2a, 0x39, 0x00, 0x0a, 0x4d, 0xcf, 0x1f, 0x50, 0xc5, 0x1b, 0x0a, 0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
	/* rxdata = data received from the stack and to be tx over the air */	
	char *rxdata;
	int i;
	struct ieee80211p_rx_status *rs;

	/***********************
	 * Netlink socket init *
	 ***********************/

	fd = netlink_init();

	if (fd == -1) {
		return -1;
	}

	rxdata = (char *)malloc(2000);

	/*************************************************************************
	 * Send data to the socket to give to the kernel the pid of this process *
	 *************************************************************************/

	ret = netlink_send(fd,NLCMD_INIT,10,&txdata[0]);		

	/************************************************
	 * Send/receive data to/from the netlink socket *
	 ************************************************/
	
	/* Set up struct rx status */
	rs = (struct ieee80211p_rx_status *)&txdata[0];
	rs->data_len = 128;
	rs->rssi = 20;
	rs->rate = 60;
	rs->band = IEEE80211_BAND_0_8GHZ;
	rs->flags = 0;	

	while (1) {

		/* Send ICMP echo request */
		printf("ieee80211p-softmodem main(): sending echo request\n");
		ret = netlink_send(fd,NLCMD_DATA,128,&txdata[0]);
		
		/* Receive ICMP echo reply */
		printf("ieee80211p-softmodem main(): waiting echo reply\n");
		ret = netlink_recv(fd,rxdata);

		if (ret > 0) {

			printf("ieee80211p-softmodem main(): ");
			for (i=0;i<ret;i++) {	
				printf("%02hhx ",rxdata[i]);	
			}

			printf("\n");

		}

		sleep(1);		

	}	

	free(rxdata);

	return 0;
}
