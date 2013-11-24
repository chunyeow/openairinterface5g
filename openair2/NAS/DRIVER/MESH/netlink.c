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
/*! \file netlink.c
* \brief estqblish a netlink
* \author Raymond knopp, and Navid Nikaein
* \company Eurecom
* \email: knopp@eurecom.fr, and navid.nikaein@eurecom.fr
*/ 

//#include <linux/config.h>
#include <linux/version.h>
#include <linux/socket.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/kthread.h>

#include <linux/mutex.h>

#include "local.h"
#include "proto_extern.h"
//#include "platform_constants.h"

//#define NETLINK_DEBUG 1


#define NAS_NETLINK_ID 31
#define NL_DEST_PID 1


static struct sock *nas_nl_sk = NULL;
static int exit_netlink_thread=0;
static int nas_netlink_rx_thread(void *);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
struct netlink_kernel_cfg oai_netlink_cfg;
#endif

static DEFINE_MUTEX(nasmesh_mutex);

static inline void nasmesh_lock(void)
{
	mutex_lock(&nasmesh_mutex);
}

static inline void nasmesh_unlock(void)
{
	mutex_unlock(&nasmesh_mutex);
}

#ifdef KERNEL_VERSION_GREATER_THAN_2629
// This can also be implemented using thread to get the data from PDCP without blocking.
static void nas_nl_data_ready (struct sk_buff *skb)
{
  // wake_up_interruptible(skb->sk->sk_sleep);
  //nasmesh_lock();
  //netlink_rcv_skb(skb, &my_rcv_msg);// my_rcv_msg is the call back func>
  //nasmesh_unlock();

  struct nlmsghdr *nlh = NULL;
  int j;  
  if (skb) {
    
#ifdef NETLINK_DEBUG
    printk("[NAS][NETLINK] Received socket from PDCP\n");
#endif //NETLINK_DEBUG
    nlh = (struct nlmsghdr *)skb->data;
  
    nas_COMMON_QOS_receive(nlh);
    
    //kfree_skb(skb); // not required,
  }

}
#else

static struct task_struct *netlink_rx_thread;

// this thread is used to avoid blocking other system calls from entering the kernel
static int nas_netlink_rx_thread(void *data) { 

  int err;
  struct sk_buff *skb = NULL;
  struct nlmsghdr *nlh = NULL;
  
  printk("[NAS][NETLINK] Starting RX Thread \n");

  while (!kthread_should_stop()) {
    
    if (nas_nl_sk) {
      skb = skb_recv_datagram(nas_nl_sk, 0, 0, &err);   

      if (skb) {
	
#ifdef NETLINK_DEBUG
	printk("[NAS][NETLINK] Received socket from PDCP\n");
#endif //NETLINK_DEBUG
	nlh = (struct nlmsghdr *)skb->data;
	
	nas_COMMON_QOS_receive(nlh);
	
	skb_free_datagram(nas_nl_sk,skb);
      }
      
    }
    else {
      if (exit_netlink_thread == 1) {
	printk("[NAS][NETLINK] exit_netlink_thread\n");
	break;
      }
    }
  } // while
  
  printk("[NAS][NETLINK] Exiting RX thread\n");
  
  return(0);
  
}

static void nas_nl_data_ready (struct sock *sk, int len)

{ 
  wake_up_interruptible(sk->sk_sleep);
}
#endif


int nas_netlink_init()
{

  printk("[NAS][NETLINK] Running init ...\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
  oai_netlink_cfg.groups   = 0;
  oai_netlink_cfg.input    = nas_nl_data_ready;
  oai_netlink_cfg.cb_mutex = &nasmesh_mutex;
  oai_netlink_cfg.bind     = NULL;

  nas_nl_sk = netlink_kernel_create(
      &init_net,
      NAS_NETLINK_ID,
# if LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0)
      THIS_MODULE,
# endif
      &oai_netlink_cfg);
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0) */
  nas_nl_sk = netlink_kernel_create(
# ifdef KERNEL_VERSION_GREATER_THAN_2622
                    &init_net,
# endif
                    NAS_NETLINK_ID,
                    0, 
                    nas_nl_data_ready,
# ifdef KERNEL_VERSION_GREATER_THAN_2622
                    &nasmesh_mutex, // NULL
# endif
                    THIS_MODULE);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0) */

  if (nas_nl_sk == NULL) {

    printk("[NAS][NETLINK] netlink_kernel_create failed \n");
    return(-1);
  }

#ifdef KERNEL_VERSION_GREATER_THAN_2629
  
#else
    // Create receive thread
  netlink_rx_thread = kthread_run(nas_netlink_rx_thread, NULL, "NAS_NETLINK_RX_THREAD");
#endif

  return(0);
  
}


void nas_netlink_release(void) {  

  exit_netlink_thread=1;
  printk("[NAS][NETLINK] Releasing netlink socket\n");
 
  if(nas_nl_sk){
#ifdef KERNEL_VERSION_GREATER_THAN_2629 
    netlink_kernel_release(nas_nl_sk); //or skb->sk
#else
    sock_release(nas_nl_sk->sk_socket);
#endif
    
  }
  
 //  printk("[NAS][NETLINK] Removing netlink_rx_thread\n");
 //kthread_stop(netlink_rx_thread);

}




int nas_netlink_send(unsigned char *data,unsigned int len) {


  struct sk_buff *nl_skb = alloc_skb(NLMSG_SPACE(len),GFP_ATOMIC);
  struct nlmsghdr *nlh = (struct nlmsghdr *)nl_skb->data;
  int status;


  //  printk("[NAS][NETLINK] Sending %d bytes (%d)\n",len,NLMSG_SPACE(len));
  skb_put(nl_skb, NLMSG_SPACE(len));
  memcpy(NLMSG_DATA(nlh),data,len);

  nlh->nlmsg_len = NLMSG_SPACE(len);


  nlh->nlmsg_pid = 0;      /* from kernel */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
  NETLINK_CB(nl_skb).portid = 0;
#else
  NETLINK_CB(nl_skb).pid = 0;
#endif

#ifdef NETLINK_DEBUG
  printk("[NAS][NETLINK] In nas_netlink_send, nl_skb %p, nl_sk %x, nlh %p, nlh->nlmsg_len %d\n",nl_skb,nas_nl_sk,nlh,nlh->nlmsg_len);
#endif //DEBUG_NETLINK

  if (nas_nl_sk) {

    //  nasmesh_lock();
    status = netlink_unicast(nas_nl_sk, nl_skb, NL_DEST_PID, MSG_DONTWAIT);
    // mutex_unlock(&nasmesh_mutex);

    if (status < 0) {
	printk("[NAS][NETLINK] SEND status is %d\n",status);
	return(0);
    }
    else {
#ifdef NETLINK_DEBUG
      printk("[NAS][NETLINK] SEND status is %d\n",status);
#endif
      return len;
    }
  }
  else {
    printk("[NAS][SEND] socket is NULL\n");
    return(0);
  }
}
