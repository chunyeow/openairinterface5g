/******************************************************************************
 *
 * Copyright(c) EURECOM / Thales Communications & Security
 *
 * Portions of this file are derived from the Atheros ath5k project.
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

#ifndef __ieee80211p_driver_h__
#define __ieee80211p_driver_h__

/******************************************************************************
 *
 * Includes
 *
 *****************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/gfp.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <net/mac80211.h>
#include <net/cfg80211.h>

/******************************************************************************
 *
 * Macros
 *
 *****************************************************************************/

#define ETH_ALEN 6
#define IEEE80211P_NUM_TXQ 1
#define IEEE80211P_TXQ_LEN_MAX 100
#define FALSE 0
#define TRUE 1
#define NETLINK_80211P 17
#define NETLINK_80211P_GROUP 1
#define NLCMD_INIT 1
#define NLCMD_DATA 2
#define NLCMD_SIZE 1

/******************************************************************************
 *
 * Device structures 
 *
 *****************************************************************************/

/****************************** 										
 * Channels / Birates / Bands *
 ******************************/

/* Supported channel */
static struct ieee80211_channel channels = {
	.band = IEEE80211_BAND_0_8GHZ,
	.center_freq = 800,
	.hw_value = 0,
	.flags = 0,
	.max_antenna_gain = 3,
	.max_power = 10,
	.max_reg_power = 10,
	.beacon_found = FALSE,
	.orig_flags = 0,
	.orig_mag = 0,
	.orig_mpwr = 0,
};/* struct ieee80211_channel */


/* Supported bitrate */
static struct ieee80211_rate bitrates = {
	.flags = 0,
	.bitrate = 60, //bitrate in units of 100 Kbps
	.hw_value = 0,
	.hw_value_short = 0,
};/* struct ieee80211_rate */

/* Supported bands (channels + bitrates) */
static struct ieee80211_supported_band bands = {
	.channels = &channels,
	.bitrates = &bitrates,
	.band = IEEE80211_BAND_0_8GHZ,
	.n_channels = 1,
	.n_bitrates = 1,
	.ht_cap.ht_supported = FALSE,
};/* struct ieee80211_supported_band */

/*********** 
 * RX path *
 ***********/

enum ieee80211p_rx_flags {
	IEEE80211P_MMIC_ERROR = 1 << 0,
	IEEE80211P_FAILED_FCS_CRC = 1 << 1,
	IEEE80211P_FAILED_PLCP_CRC = 1 << 2,
	IEEE80211P_MACTIME_MPDU = 1 << 3,
	IEEE80211P_NO_SIGNAL_VAL = 1 << 4,
};

struct ieee80211p_rx_status {
	u16	data_len;		//frame data length
	u8	rssi; 			//received power
	u8	rate; 			//reveived data rate in units of 100 kbps
	enum ieee80211_band band;
	u8	flags; 			//RX flags
}; /* struct ieee80211p_rx_status */

/*********************
 * Regulatory domain *									
 *********************/

/* Regulatory domain */ 
static struct ieee80211_regdomain regd = {
	.n_reg_rules = 1,
	.alpha2 = "99",
	.dfs_region = 0,
	.reg_rules = {
		/* start freq / end freq / bandwidth / gain / eirp / flags */
		REG_RULE(0,6000,40,3,47,0),
	}
};

/************************************
 * Virtual interface's private data *
 ************************************/

struct ieee80211p_vif_priv {
	enum nl80211_iftype opmode;
};

/******************************************************************************
 *
 * Driver's private data 
 *
 *****************************************************************************/

struct ieee80211p_priv {
	
	/* Configuration and hardware information for an 802.11 PHY */	
	struct ieee80211_hw *hw;	

	/* RX */
	spinlock_t rxq_lock;
	struct tasklet_struct rx_tq;

	/* Nb of virtual interfaces */
	int nvifs;

	/* Current channel in use */
	struct ieee80211_channel *cur_chan;

	/* Current power level in dBm */
	int cur_power;

	/* Current data rate in units of 100 kbps */
	u16 cur_datarate;

	/* Lock used for misc purposes */
	spinlock_t lock;

	/* Netlink socket used for softmodem <-> ieee80211p communications */
	struct sock *nl_sock;
	unsigned int pid_softmodem;
	struct sk_buff *rx_skb;

};/* ieee80211p_device_priv */

#endif /* __ieee80211_device_h__ */
