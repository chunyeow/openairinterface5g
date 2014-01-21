/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file device.c
* \brief Networking Device Driver for OpenAirInterface
* \author  navid.nikaein, yan.moret(no longer valid), michelle.wetterwald(no longer valid), raymond.knopp, Lionel Gauthier
* \company Eurecom
* \email: raymond.knopp@eurecom.fr, navid.nikaein@eurecom.fr, lionel.gauthier@eurecom.fr

*/
/*******************************************************************************/

#ifndef OAI_NW_DRIVER_USE_NETLINK
#ifdef RTAI
#include "rtai_posix.h"
#define RTAI_IRQ 30 //try to get this irq with RTAI
#endif // RTAI
#endif // OAI_NW_DRIVER_USE_NETLINK

#include "constant.h"
#include "local.h"
#include "proto_extern.h"

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/moduleparam.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/delay.h>
#include <asm/unistd.h>
#include <linux/netdevice.h>



struct net_device *ue_ip_dev[UE_IP_NB_INSTANCES_MAX];

#ifdef OAI_NW_DRIVER_USE_NETLINK
extern void ue_ip_netlink_release(void);
extern int ue_ip_netlink_init(void);
#endif

//---------------------------------------------------------------------------
int ue_ip_find_inst(struct net_device *dev) {
//---------------------------------------------------------------------------
  int i;

  for (i=0;i<UE_IP_NB_INSTANCES_MAX;i++)
    if (ue_ip_dev[i] == dev)
      return(i);
  return(-1);
}

//---------------------------------------------------------------------------

#ifndef OAI_NW_DRIVER_USE_NETLINK
void *ue_ip_interrupt(void){
  //---------------------------------------------------------------------------
  u8 cxi;

  //  struct ue_ip_priv *priv=netdev_priv(dev_id);
  //  unsigned int flags;
  //  priv->lock = SPIN_LOCK_UNLOCKED;

#ifdef OAI_DRV_DEBUG_INTERRUPT
  printk("INTERRUPT - begin\n");
#endif
  //  spin_lock_irqsave(&priv->lock,flags);
  cxi=0;
  //    mesh_GC_receive();
  //    mesh_DC_receive(naspriv->cx+cxi);
#ifndef OAI_NW_DRIVER_USE_NETLINK
  ue_ip_common_wireless2ip();
#endif
  //  spin_unlock_irqrestore(&priv->lock,flags);
#ifdef OAI_DRV_DEBUG_INTERRUPT
  printk("INTERRUPT: end\n");
#endif
  //  return 0;
}
#endif //NETLINK
//---------------------------------------------------------------------------
void ue_ip_timer(unsigned long data){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv=(struct ue_ip_priv *)data;
  spin_lock(&priv->lock);
  (priv->timer).function=ue_ip_timer;
  (priv->timer).expires=jiffies+UE_IP_TIMER_TICK;
  (priv->timer).data=data;
  add_timer(&priv->timer);
  spin_unlock(&priv->lock);
  return;
//  add_timer(&gpriv->timer);
//  spin_unlock(&gpriv->lock);
}

//---------------------------------------------------------------------------
// Called by ifconfig when the device is activated by ifconfig
int ue_ip_open(struct net_device *dev){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv=netdev_priv(dev);

  // Address has already been set at init
#ifndef OAI_NW_DRIVER_USE_NETLINK
  if (pdcp_2_ue_ip_irq==-EBUSY){
      printk("[UE_IP_DRV][%s] : irq failure\n", __FUNCTION__);
      return -EBUSY;
  }
#endif //OAI_NW_DRIVER_USE_NETLINK

  if(!netif_queue_stopped(dev))
          netif_start_queue(dev);
  else
          netif_wake_queue(dev);

  init_timer(&priv->timer);
  (priv->timer).expires   = jiffies+UE_IP_TIMER_TICK;
  (priv->timer).data      = (unsigned long)priv;
  (priv->timer).function  = ue_ip_timer;
  //add_timer(&priv->timer);

  printk("[UE_IP_DRV][%s] name = %s\n", __FUNCTION__, dev->name);
  return 0;
}

//---------------------------------------------------------------------------
// Called by ifconfig when the device is desactivated
int ue_ip_stop(struct net_device *dev){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv = netdev_priv(dev);

  printk("[UE_IP_DRV][%s] Begin\n", __FUNCTION__);
  del_timer(&(priv->timer));
  netif_stop_queue(dev);
  //    MOD_DEC_USE_COUNT;
  printk("[UE_IP_DRV][%s] End\n", __FUNCTION__);
  return 0;
}

//---------------------------------------------------------------------------
void ue_ip_teardown(struct net_device *dev){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv;
  int              inst;

  printk("[UE_IP_DRV][%s] Begin\n", __FUNCTION__);
  if (dev) {
      priv = netdev_priv(dev);
      inst = ue_ip_find_inst(dev);
      if ((inst<0) || (inst>=UE_IP_NB_INSTANCES_MAX)) {
          printk("[UE_IP_DRV][%s] ERROR, couldn't find instance\n", __FUNCTION__);
          return;
      }


      printk("[UE_IP_DRV][%s] End\n", __FUNCTION__);
  } // check dev
  else {
      printk("[UE_IP_DRV][%s] Device is null\n", __FUNCTION__);
  }
}
//---------------------------------------------------------------------------
int ue_ip_set_config(struct net_device *dev, struct ifmap *map){
  //---------------------------------------------------------------------------
  printk("[UE_IP_DRV][%s] Begin\n", __FUNCTION__);
  if (dev->flags & IFF_UP)
      return -EBUSY;
  if (map->base_addr != dev->base_addr) {
      printk(KERN_WARNING "[UE_IP_DRV][%s] Can't change I/O address\n", __FUNCTION__);
      return -EOPNOTSUPP;
  }
  if (map->irq != dev->irq)
      dev->irq = map->irq;
  printk("[UE_IP_DRV][%s] End\n", __FUNCTION__);
  return 0;
}

//---------------------------------------------------------------------------
//
int ue_ip_hard_start_xmit(struct sk_buff *skb, struct net_device *dev){
  //---------------------------------------------------------------------------
  int inst;

  if (dev) {
    inst = ue_ip_find_inst(dev);
  } else {
    printk("[UE_IP_DRV][%s] ERROR, device is null\n", __FUNCTION__);
    return -1;
  }

  if ((inst>=0) && (inst<UE_IP_NB_INSTANCES_MAX)) {
      #ifdef OAI_DRV_OAI_DRV_DEBUG_DEVICE
      printk("[UE_IP_DRV][%s] inst %d,  begin\n", __FUNCTION__,inst);
      #endif

      if (!skb){
          printk("[UE_IP_DRV][%s] input parameter skb is NULL\n", __FUNCTION__);
          return -1;
      }
      // End debug information
      netif_stop_queue(dev);
      dev->trans_start = jiffies;
      #ifdef OAI_DRV_DEBUG_DEVICE
      printk("[UE_IP_DRV][%s] step 1\n", __FUNCTION__);
      #endif
      ue_ip_common_ip2wireless(skb,inst);
      #ifdef OAI_DRV_DEBUG_DEVICE
      printk("[UE_IP_DRV][%s] step 2\n", __FUNCTION__);
      #endif
      dev_kfree_skb(skb);
      #ifdef OAI_DRV_DEBUG_DEVICE
      printk("[UE_IP_DRV][%s] step 3\n", __FUNCTION__);
      #endif
      netif_wake_queue(dev);
      #ifdef OAI_DRV_DEBUG_DEVICE
      printk("[UE_IP_DRV][%s] end\n", __FUNCTION__);
      #endif
  } else {
      printk("[UE_IP_DRV][%s] ERROR, couldn't find instance\n", __FUNCTION__);
      return(-1);
  }
  return 0;
}

//---------------------------------------------------------------------------
struct net_device_stats *ue_ip_get_stats(struct net_device *dev){
//---------------------------------------------------------------------------
  //    return &((struct ue_ip_priv *)dev->priv)->stats;
  struct ue_ip_priv *priv = netdev_priv(dev);
  //printk("[UE_IP_DRV][%s]\n", __FUNCTION__);
  return &priv->stats;
}
//---------------------------------------------------------------------------
int ue_ip_set_mac_address(struct net_device *dev, void *mac) {
//---------------------------------------------------------------------------
  //struct sockaddr *addr = mac;
  printk("[UE_IP_DRV][%s] CHANGE MAC ADDRESS UNSUPPORTED\n", __FUNCTION__);
  //memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
  return 0;
}
//---------------------------------------------------------------------------
int ue_ip_change_mtu(struct net_device *dev, int mtu){
  //---------------------------------------------------------------------------
  printk("[UE_IP_DRV][%s] CHANGE MTU %d bytes\n", __FUNCTION__, mtu);
  if ((mtu<50) || (mtu>1500))
      return -EINVAL;
  dev->mtu = mtu;
  return 0;
}
//---------------------------------------------------------------------------
void ue_ip_change_rx_flags(struct net_device *dev, int flags){
  //---------------------------------------------------------------------------
  struct ue_ip_priv *priv =  netdev_priv(dev);
  printk("[UE_IP_DRV][%s] CHANGE RX FLAGS %08X\n", __FUNCTION__, flags);
  priv->rx_flags ^= flags;
}

//---------------------------------------------------------------------------
void ue_ip_tx_timeout(struct net_device *dev){
  //---------------------------------------------------------------------------
  // Transmitter timeout, serious problems.
  struct ue_ip_priv *priv =  netdev_priv(dev);

  printk("[UE_IP_DRV][%s] begin\n", __FUNCTION__);
  //  (struct ue_ip_priv *)(dev->priv)->stats.tx_errors++;
  (priv->stats).tx_errors++;
  dev->trans_start = jiffies;
  netif_wake_queue(dev);
  printk("[UE_IP_DRV][%s] transmit timed out %s\n", __FUNCTION__,dev->name);
}

static const struct net_device_ops ue_ip_netdev_ops = {
    .ndo_open               = ue_ip_open,
    .ndo_stop               = ue_ip_stop,
    .ndo_start_xmit         = ue_ip_hard_start_xmit,
    .ndo_validate_addr      = NULL,
    .ndo_get_stats          = ue_ip_get_stats,
    .ndo_set_mac_address    = ue_ip_set_mac_address,
    .ndo_set_config         = ue_ip_set_config,
    .ndo_do_ioctl           = NULL,
    .ndo_change_mtu         = ue_ip_change_mtu,
    .ndo_tx_timeout         = ue_ip_tx_timeout,
    .ndo_change_rx_flags    = ue_ip_change_rx_flags,
};
    /*.ndo_set_multicast_list = NULL,*/

//---------------------------------------------------------------------------
// Initialisation of the network device
void ue_ip_init(struct net_device *dev){
  //---------------------------------------------------------------------------
    struct ue_ip_priv *priv;
    if (dev) {
        priv = netdev_priv(dev);
        memset(priv, 0, sizeof(struct ue_ip_priv));
        spin_lock_init(&priv->lock);
        #ifdef KERNEL_VERSION_GREATER_THAN_2629
        dev->netdev_ops = &ue_ip_netdev_ops;
        #else
        #error "KERNEL VERSION MUST BE NEWER THAN 2.6.29"
        #endif
        dev->hard_header_len = 0;
        dev->addr_len = UE_IP_ADDR_LEN;
        dev->flags = IFF_BROADCAST|IFF_MULTICAST|IFF_NOARP;
        dev->tx_queue_len = UE_IP_TX_QUEUE_LEN;
        dev->mtu = UE_IP_MTU;
    } else {
        printk("[UE_IP_DRV][%s] ERROR, Device is NULL!!\n", __FUNCTION__);
        return;
    }
}
//---------------------------------------------------------------------------
int init_module (void) {
//---------------------------------------------------------------------------
  int err,inst;
  char devicename[100];


  // Initialize parameters shared with RRC
  printk("[UE_IP_DRV][%s] Starting OAI IP driver", __FUNCTION__);

  for (inst=0;inst<UE_IP_NB_INSTANCES_MAX;inst++) {
    printk("[UE_IP_DRV][%s] begin init instance %d\n", __FUNCTION__,inst);
    sprintf(devicename,"oip%d",inst);
    ue_ip_dev[inst]  = alloc_netdev(sizeof(struct ue_ip_priv),devicename, ue_ip_init);
    //netif_stop_queue(ue_ip_dev[inst]);
    if (ue_ip_dev[inst] == NULL) {
         printk("[UE_IP_DRV][%s][INST %02d] alloc_netdev FAILED\n", __FUNCTION__,inst);
    } else {
        // linux/net/core/dev.c line 4767
        err= register_netdev(ue_ip_dev[inst]);
        if (err){
          printk("[UE_IP_DRV][%s] (inst %d): error %i registering device %s\n", __FUNCTION__, inst,err, ue_ip_dev[inst]->name);
        }else{
          printk("[UE_IP_DRV][%s] registering device %s, ifindex = %d\n\n", __FUNCTION__,ue_ip_dev[inst]->name, ue_ip_dev[inst]->ifindex);
        }
    }
  }

  printk("[UE_IP_DRV][%s] NETLINK INIT\n", __FUNCTION__);
  if ((err=ue_ip_netlink_init()) == -1)
    printk("[UE_IP_DRV][%s] NETLINK failed\n", __FUNCTION__);

  return err;

}

//---------------------------------------------------------------------------
void cleanup_module(void){
//---------------------------------------------------------------------------
    int inst;

    printk("[UE_IP_DRV][CLEANUP]nasmesh_cleanup_module: begin\n");

    for (inst=0;inst<UE_IP_NB_INSTANCES_MAX;inst++) {
#ifdef OAI_DRV_DEBUG_DEVICE
        printk("nasmesh_cleanup_module: unregister and free net device instance %d\n",inst);
#endif
        if (ue_ip_dev[inst]) {
            unregister_netdev(ue_ip_dev[inst]);
            ue_ip_teardown(ue_ip_dev[inst]);
            free_netdev(ue_ip_dev[inst]);
        }
    }
    ue_ip_netlink_release();
    printk("nasmesh_cleanup_module: end\n");
}

#define DRV_NAME        "ue_ip"
#define DRV_VERSION     "1.0"DRV_NAME
#define DRV_DESCRIPTION "OPENAIR UE IP Device Driver"
#define DRV_COPYRIGHT   "-Copyright(c) GNU GPL Eurecom 2013"
#define DRV_AUTHOR      "Lionel GAUTHIER: <firstname.name@eurecom.fr>"DRV_COPYRIGHT


