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

/******************************************************************************
 *
 * Includes
 *
 *****************************************************************************/

#include "ieee80211p-driver.h"

/******************************************************************************
 *
 * Module information
 *
 *****************************************************************************/

#define DRV_DESCRIPTION	"IEEE 802.11p driver"
#define DRV_VERSION "V1.0"
#define DRV_AUTHOR "EURECOM / THALES COMMUNICATIONS & SECURITY"

MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR(DRV_AUTHOR);

/****************************************************************************** 
 * 
 * Driver's private data 
 *
 *****************************************************************************/

static struct ieee80211p_priv drv_priv_data;

/******************************************************************************
 * 
 * Driver's private data related routines : RX path / Init / Exit 
 *
 *****************************************************************************/

/*********** 
 * RX path *
 ***********/

int find_rate_idx(struct ieee80211p_priv *priv,
							enum ieee80211_band band, u16 bitrate) {

	/* Data rate index  */
	int rate_idx = -1;

	/* Loop variable */
	int i;

	struct wiphy *wiphy = priv->hw->wiphy;

	/* We look for the idx of the RX bitrate in the bitrates of the band */
	for (i=0;i<wiphy->bands[band]->n_bitrates;i++) {
		if (wiphy->bands[band]->bitrates[i].bitrate == bitrate) {
			rate_idx = i;
		}
	}

	return rate_idx;

} /* ieee80211p_find_rate_idx */

/************************
 * RX path / RX tasklet *
 ************************/

static void ieee80211p_tasklet_rx(unsigned long data) {
	
	/* Driver's private data */	
	struct ieee80211p_priv *priv = (void *)data;

	/* RX skb */
	struct sk_buff *skb = priv->rx_skb;

	/* RX status */	
	struct ieee80211_rx_status *rxs = NULL;
	struct ieee80211p_rx_status *rs = NULL;

	/* Netlink header */
	struct nlmsghdr *nlh = NULL;
	
	/* Netlink command */
	char *nlcmd = NULL;

	/* Received data rate index */
	int rate_idx = -1;

	/* lock */	
	spin_lock(&priv->rxq_lock);

	/************************
	 * Netlink skb handling *
	 ************************/

	printk(KERN_ERR "ieee80211p_tasklet_rx: receiving data from PHY\n");

	if (skb == NULL) {
        printk(KERN_ERR "ieee80211_tasklet_rx: received skb == NULL\n");
        goto error;
    }	

	/* Get the netlink message header */
	nlh = (struct nlmsghdr *)skb->data;
 
	/* Check the command of the received msg */
	nlcmd = (char *)NLMSG_DATA(nlh);	
	if (*nlcmd == NLCMD_INIT) {
		/* Keep track of the softmodem pid */
		priv->pid_softmodem = nlh->nlmsg_pid;
		printk(KERN_ERR "ieee80211_tasklet_rx: NLCMD_INIT received / softmodem pid = %u\n",priv->pid_softmodem);
		dev_kfree_skb_any(skb);	
		goto error;
	}

	/* Remove the nlmsg header + netlink command */
	rs = (struct ieee80211p_rx_status *)skb_pull(skb,sizeof(struct nlmsghdr)+NLCMD_SIZE);

	if (rs == NULL) {
		printk(KERN_ERR "ieee80211_tasklet_rx: rx status == NULL\n");
		dev_kfree_skb_any(skb);
		goto error;
	}

	/*********
	 * Stats *
	 *********/

	rxs = IEEE80211_SKB_RXCB(skb);

	if (rxs == NULL) {
		printk(KERN_ERR "ieee80211_tasklet_rx: rx status == NULL\n");
		dev_kfree_skb_any(skb);
		goto error;
	}

	/* Keep track of the stats sent by the softmodem */	
	rxs->freq = priv->cur_chan->center_freq;
	rxs->signal = rs->rssi;
	rxs->band = rs->band;
	rxs->flag = 0;
	rate_idx = find_rate_idx(priv,rxs->band,rs->rate);

	if (rate_idx == -1) {
		printk(KERN_ERR "ieee80211_tasklet_rx: unknown data rate %u\n",rs->rate);
		dev_kfree_skb_any(skb);		
		goto error;
	} else {
		rxs->rate_idx = rate_idx; 
	}

	if (rs->flags & IEEE80211P_MMIC_ERROR) {
		rxs->flag |= RX_FLAG_MMIC_ERROR;
	}
	if (rs->flags & IEEE80211P_FAILED_FCS_CRC) {
		rxs->flag |= RX_FLAG_FAILED_FCS_CRC;
	}
	if (rs->flags & IEEE80211P_FAILED_PLCP_CRC) {
		rxs->flag |= RX_FLAG_FAILED_PLCP_CRC;
	}
	if (rs->flags & IEEE80211P_MACTIME_MPDU) {
		rxs->flag |= RX_FLAG_MACTIME_MPDU;
	}
	if (rs->flags & IEEE80211P_NO_SIGNAL_VAL) {
		rxs->flag |= RX_FLAG_NO_SIGNAL_VAL;
	}

	/* Remove the rx status from the skb */
	skb_pull(skb,sizeof(struct ieee80211p_rx_status));	

	printk(KERN_ERR "ieee80211p_tasklet_rx: sending data to ieee80211\n");

	/* Give skb to the mac80211 subsystem */
	ieee80211_rx(priv->hw, skb);

error:
	/* unlock */
	spin_unlock(&priv->rxq_lock);

} /* ieee80211p_tasklet_rx */

/************************
 * RX path / RX handler *
 ************************/

static void ieee80211p_rx(struct sk_buff *skb) {

	/* We copy the received buffer since we need to modify it */
	drv_priv_data.rx_skb = skb_copy(skb,GFP_ATOMIC);

	/* Schedule a tasklet to handle the receivded skb */
	tasklet_schedule(&drv_priv_data.rx_tq);
}

/********
 * Init *
 ********/

static int reg_copy_regd(const struct ieee80211_regdomain **dst_regd,
			const struct ieee80211_regdomain *src_regd) {

	struct ieee80211_regdomain *regd;
	int size_of_regd = 0;
	int ret = 0;
	int i = 0;

	size_of_regd = sizeof(struct ieee80211_regdomain) + ((src_regd->n_reg_rules + 1)*(sizeof(struct ieee80211_reg_rule)));

	regd = kzalloc(size_of_regd, GFP_KERNEL);

	if (!regd) {
		ret = -1;
		goto error;
	}

	memcpy(regd,src_regd,sizeof(struct ieee80211_regdomain));

	for (i=0;i<src_regd->n_reg_rules;i++) {
		memcpy(&regd->reg_rules[i],&src_regd->reg_rules[i],
			sizeof(struct ieee80211_reg_rule));
	}

	*dst_regd = regd;

error:
	return ret;

} /* reg_copy_regd */

int ieee80211p_priv_init(struct ieee80211p_priv *priv) {	

	/* Configuration and hardware information for an 802.11 PHY */	
	struct ieee80211_hw *hw = priv->hw;
	struct wiphy *wiphy = hw->wiphy; 	

	/* Return value */
	int ret = 0;

	/******************************
	 * Initializing hardware data *
	 ******************************/
	
	/* Received signal power is given in dBm */
	/* hw supports 11.p */	
	hw->flags = IEEE80211_HW_SIGNAL_DBM | IEEE80211_HW_DOT11OCB_SUPPORTED;

	/* Headroom to reserve in each transmit skb */	
	hw->extra_tx_headroom = 0;

	/* Number of available hardware queues */
	hw->queues = IEEE80211P_NUM_TXQ;

	/* Virtual interface's private data size */
	hw->vif_data_size = sizeof(struct ieee80211p_vif_priv);

	/***************************
	 * Initializing wiphy data *
	 ***************************/
	
	/* We have our own regulatory domain */
	ret = reg_copy_regd(&wiphy->regd,&regd);

	if (ret == -1) {
		printk(KERN_ERR "ieee80211p_priv_init: reg domain copy failed\n");
		goto error;
	}

	/* Set interface mode */
	/* For now the only supported type of interface is adhoc */
	wiphy->interface_modes = BIT(NL80211_IFTYPE_ADHOC);
	
	/* Describes the frequency bands a wiphy is able to operate in */
	wiphy->bands[IEEE80211_BAND_0_8GHZ] = &bands;

	/* Enable ieee 80211.p mode */
	wiphy->dot11OCBActivated = 1;

	/***********************************
	 * Initilizing driver private data *
	 ***********************************/

	/* Lock */
	spin_lock_init(&priv->lock);

	/* RX queues setup */
	spin_lock_init(&priv->rxq_lock);

	tasklet_init(&priv->rx_tq,ieee80211p_tasklet_rx,(unsigned long)priv);

	/* Virtual interfaces init */
	priv->nvifs = 0;

	/* Current channel init */
	/* The default current channel is the 1st one in the band */
	priv->cur_chan = &bands.channels[0];

	/* Power level init */
	/* Default value =  max power level in the default curent channel */
	priv->cur_power = priv->cur_chan->max_power;

	/* Data rate init */
	/* Default value = first bitrate of the band */
	priv->cur_datarate = bands.bitrates[0].bitrate;

	/* Netlink socket init */
	priv->nl_sock = netlink_kernel_create(&init_net,NETLINK_80211P,0,ieee80211p_rx,NULL,THIS_MODULE);
	if (priv->nl_sock == NULL) {
		printk(KERN_ERR "ieee80211p_priv_init: netlink_kernel_create failed\n");
		ret = -1;
		goto error;	
	}

	priv->pid_softmodem = 0;

	priv->rx_skb = NULL;

	printk(KERN_ERR "ieee80211p_priv_init: initialization done\n");

error:
	return ret;

} /* ieee80211p_priv_init */

/********
 * Exit *
 ********/

void ieee80211p_priv_exit(struct ieee80211p_priv *priv) {

	/*********************************
	 * Freeing driver's private data *
	 *********************************/

	tasklet_kill(&priv->rx_tq);

	/*TODO: check if needed */	
	/*if (priv->rx_skb != NULL) {
		dev_kfree_skb_any(priv->rx_skb);
	}*/

	sock_release(priv->nl_sock->sk_socket);

} /* ieee80211_priv_exit */

/******************************************************************************
 * 
 * Mandatory callbacks from mac80211 to the driver: TX path, add interface...
 *
 *****************************************************************************/

static void ieee80211p_tx(struct ieee80211_hw *hw, struct sk_buff *skb) {
	
	/* Netlink message header */
	struct nlmsghdr *nlh = NULL;

	/* Netlink skb */
	struct sk_buff *nlskb = NULL;

	/* The size of the skb */	
	int skblen;

	/* Get driver's private data */
	struct ieee80211p_priv *priv = hw->priv;

	/* Get the number of the TX queue */
	int qnum = skb_get_queue_mapping(skb);

	/* Return value */
	int ret = 0;

	printk(KERN_ERR "ieee80211p_tx: receiving data from ieee80211\n");
	
	if (qnum >= IEEE80211P_NUM_TXQ) {
		printk(KERN_ERR "ieee80211p_tx: wrong queue number\n");
		dev_kfree_skb_any(skb);
		return;
	}

	if (priv->pid_softmodem == 0) {
		printk(KERN_ERR "ieee80211_tx: softmodem pid unknown\n");
		dev_kfree_skb_any(skb);
		return;
	}

	/* Get the size of the skb */
	if (skb->data_len == 0) {
		skblen = skb->len;
	}
	else {
		printk(KERN_ERR "ieee80211p_tx: skb not linear\n");
		dev_kfree_skb_any(skb);
		return;	
	}	

	/* Allocate nlskb */
	nlskb = alloc_skb(NLMSG_SPACE(skblen), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);

	if (nlskb == NULL) {
		printk(KERN_ERR "ieee80211p_tx: alloc nlskb failed\n");
		return;
	}    

	/* Add room for the nlmsg header */
	skb_put(nlskb, NLMSG_SPACE(skblen));

    /* Configure the nlmsg header */
	nlh = (struct nlmsghdr *)nlskb->data;
    nlh->nlmsg_len = NLMSG_SPACE(skblen);
    nlh->nlmsg_pid = priv->pid_softmodem;
    nlh->nlmsg_flags = 0;

    NETLINK_CB(nlskb).pid = 0; // nlmsg sent from kernel
    NETLINK_CB(nlskb).dst_group = NETLINK_80211P_GROUP;

	/* Copy the data from the skb to the nlskb */
	memcpy(NLMSG_DATA(nlh),skb->data,skb->len);

	/* Free the old skb */
	dev_kfree_skb_any(skb);

	printk(KERN_ERR "ieee80211p_tx: sending data to PHY using pid = %d\n",priv->pid_softmodem);

    ret = netlink_unicast(priv->nl_sock,nlskb,priv->pid_softmodem,NETLINK_80211P_GROUP);

    if (ret <= 0) {
    	printk(KERN_ERR "ieee80211p_tx: netlink mesg not sent ret = %d\n",ret);
	return;
    }

} /* ieee80211p_tx */


static int ieee80211p_start(struct ieee80211_hw *hw) {

	/* Nothing to be done here */	
	
	return 0;

} /* ieee80211p_start */


static void ieee80211p_stop(struct ieee80211_hw *hw) {

	/* Nothing to be done here */

} /* ieee80211p_stop */


static int ieee80211p_add_interface(struct ieee80211_hw *hw,
		struct ieee80211_vif *vif) {
	
	/* Get driver's private data */
	struct ieee80211p_priv *priv = hw->priv;
	struct ieee80211p_vif_priv *vif_priv = (void *)vif->drv_priv;

	/* Return value */
	int ret = 0;	

	spin_lock(&priv->lock);

	/* Only ADHOC interfaces supported */
	if (vif->type == NL80211_IFTYPE_ADHOC) {
		vif_priv->opmode = vif->type;
	} else {
		printk(KERN_ERR "ieee80211p_add_interface: wrong interface type\n");
		ret = -EOPNOTSUPP;
		goto end;
	}

	/* Keep track of the number of virtual interfaces */
	priv->nvifs++;		

end:
	spin_unlock(&priv->lock);
	return ret;

} /* ieee80211p_add_interface */


static void ieee80211p_remove_interface(struct ieee80211_hw *hw,
		struct ieee80211_vif *vif) {

	/* Get driver's private data */
	struct ieee80211p_priv *priv = hw->priv;

	spin_lock(&priv->lock);
	
	/* Keep track of the number of virtual interfaces */
	priv->nvifs--;

	spin_unlock(&priv->lock);

} /* ieee80211p_remove_interface */


static int ieee80211p_config(struct ieee80211_hw *hw, u32 changed) {
	
	/* Get driver's private data */
	struct ieee80211p_priv *priv = hw->priv;

	/* Get device configuration */
	struct ieee80211_conf *conf = &hw->conf;

	spin_lock(&priv->lock);

	/* Current channel changed */
	if (changed & IEEE80211_CONF_CHANGE_CHANNEL) {
		if (conf->channel != NULL) {
			priv->cur_chan = conf->channel;
		}
	}

	/* Transmit power changed */
	if (changed & IEEE80211_CONF_CHANGE_POWER) {
		priv->cur_power = conf->power_level;
	}
	
	spin_unlock(&priv->lock);
	
	return 0;

} /* ieee80211p_config */


static void ieee80211p_configure_filter(struct ieee80211_hw *hw,
		unsigned int changed_flags,unsigned int *new_flags, u64 multicast) {

#define SUPPORTED_FIF_FLAGS \
	(FIF_PROMISC_IN_BSS | FIF_ALLMULTI | FIF_FCSFAIL | \
	FIF_PLCPFAIL | FIF_CONTROL | FIF_OTHER_BSS | \
	FIF_BCN_PRBRESP_PROMISC)	
	
	*new_flags &= SUPPORTED_FIF_FLAGS;
	
} /* ieee80211p_configure_filter */

/**************************************************************************
 * Only the mandatory callbacks of ieee80211p_ops have been implemented *
 **************************************************************************/

const struct ieee80211_ops ieee80211p_driver_ops = {		
	.tx = ieee80211p_tx,
	.start = ieee80211p_start,
	.stop = ieee80211p_stop,
	.add_interface = ieee80211p_add_interface,
	.remove_interface = ieee80211p_remove_interface,
	.config = ieee80211p_config,
	.configure_filter = ieee80211p_configure_filter,
};

/******************************************************************************
 *
 * Driver's entry point
 *
 *****************************************************************************/

/******************
 * Start function *
 ******************/

static int ieee80211p_driver_start(struct ieee80211p_priv *priv) {

	/* Return value */	
	int ret = 0;

	/* Configuration and hardware information for an 802.11 PHY */
	struct ieee80211_hw *hw = NULL;
	
	/*******************************************
	 * Allocating hw (mac80211 main structure) * 
	 * and priv (driver private data)          *
	 *******************************************/

	hw = ieee80211_alloc_hw(sizeof(struct ieee80211p_priv),&ieee80211p_driver_ops);

	if (hw == NULL) {
		ret = -1;
		printk(KERN_ERR "ieee80211p_driver_start: can't alloc ieee80211 hw\n");
		goto error;
	}	

	priv->hw = hw;
	hw->priv = priv;

	/*************************************************
	 * Initializing hardware and driver private data *
	 *************************************************/

	ret = ieee80211p_priv_init(priv);

	if (ret == -1) {
		printk(KERN_ERR "ieee80211p_driver_start: can't init priv data hw\n");		
		goto error;
	}

	/******************
	 * Registering hw *
	 *****************/

	ret = ieee80211_register_hw(hw);

	if (ret) {
		printk(KERN_ERR "ieee80211p_driver_start: can't reg ieee80211 hw\n");
		goto error;
	}

	return 0;
	
error:
	return ret;

} /* ieee80211p_driver_start */

/*****************
 * Stop function *
 *****************/

static void ieee80211p_driver_stop(struct ieee80211p_priv *priv) {
	
	/********************
	 * Freeing hardware *
	 ********************/

	struct ieee80211_hw *hw = priv->hw;
	
	/* TODO: check if needed */	
	//kfree(hw->wiphy->regd);

	ieee80211_unregister_hw(hw);

	ieee80211_free_hw(hw);

	/*********************************
 	 * Freeing driver's private data *
	 *********************************/

	ieee80211p_priv_exit(priv);

} /* ieee80211p_driver_stop */

/*******************************************
 * Driver's init function called at insmod *
 *******************************************/

static int __init ieee80211p_init(void)
{
	int ret = 0;	

	printk(KERN_DEBUG "ieee80211p_init: ieee80211p module inserted\n");

	ret = ieee80211p_driver_start(&drv_priv_data);

	if (ret == -1) {
		printk(KERN_ERR "ieee80211p_init: can't start ieee80211p driver\n");
	}

	return 0;

} /* ieee80211p_init */

/******************************************
 * Driver's exit function called at rmmod *
 ******************************************/

static void __exit ieee80211p_exit(void)
{
	ieee80211p_driver_stop(&drv_priv_data);	

	printk(KERN_DEBUG "ieee80211p_init: ieee80211p module removed\n");

} /* ieee80211p_exit */

module_exit(ieee80211p_exit);
module_init(ieee80211p_init);
