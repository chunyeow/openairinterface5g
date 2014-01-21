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
* \brief establish a netlink
* \author Raymond knopp, and Navid Nikaein, Lionel Gauthier
* \company Eurecom
* \email: knopp@eurecom.fr, and navid.nikaein@eurecom.fr, lionel.gauthier@eurecom.fr
*/ 

//#include <linux/config.h>
#include <linux/socket.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/kthread.h>

#include <linux/mutex.h>

#include "local.h"
#include "proto_extern.h"

//#define NETLINK_DEBUG 1


#define NAS_NETLINK_ID 31
#define NL_DEST_PID 1


static struct sock *nas_nl_sk = NULL;
static int exit_netlink_thread=0;
static int nas_netlink_rx_thread(void *);


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
  if (skb) {
#ifdef NETLINK_DEBUG
    printk("[UE_IP_DRV][NETLINK] Received socket from PDCP\n");
#endif //NETLINK_DEBUG
    nlh = (struct nlmsghdr *)skb->data;
    ue_ip_common_wireless2ip(nlh);
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
  
  printk("[UE_IP_DRV][NETLINK] Starting RX Thread \n");

  while (!kthread_should_stop()) {
    
    if (nas_nl_sk) {
      skb = skb_recv_datagram(nas_nl_sk, 0, 0, &err);   

      if (skb) {
	
#ifdef NETLINK_DEBUG
	printk("[UE_IP_DRV][NETLINK] Received socket from PDCP\n");
#endif //NETLINK_DEBUG
	nlh = (struct nlmsghdr *)skb->data;
	
	nas_COMMON_QOS_receive(nlh);
	
	skb_free_datagram(nas_nl_sk,skb);
      }
      
    }
    else {
      if (exit_netlink_thread == 1) {
	printk("[UE_IP_DRV][NETLINK] exit_netlink_thread\n");
	break;
      }
    }
  } // while
  
  printk("[UE_IP_DRV][NETLINK] Exiting RX thread\n");
  
  return(0);
  
}

static void nas_nl_data_ready (struct sock *sk, int len)

{ 
  wake_up_interruptible(sk->sk_sleep);
}
#endif


int ue_ip_netlink_init(void)
{

  printk("[UE_IP_DRV][NETLINK] Running init ...\n");
  

  nas_nl_sk = netlink_kernel_create(
#ifdef KERNEL_VERSION_GREATER_THAN_2622
				    &init_net,       
#endif
				    NAS_NETLINK_ID, 
				    0, 
				    nas_nl_data_ready, 
#ifdef KERNEL_VERSION_GREATER_THAN_2622
				    &nasmesh_mutex, // NULL
#endif
				    THIS_MODULE);


  if (nas_nl_sk == NULL) {

    printk("[UE_IP_DRV][NETLINK] netlink_kernel_create failed \n");
    return(-1);
  }

#ifdef KERNEL_VERSION_GREATER_THAN_2629
  
#else
    // Create receive thread
  netlink_rx_thread = kthread_run(nas_netlink_rx_thread, NULL, "NAS_NETLINK_RX_THREAD");
#endif

  return(0);
  
}


void ue_ip_netlink_release(void) {

  exit_netlink_thread=1;
  printk("[UE_IP_DRV][NETLINK] Releasing netlink socket\n");
 
  if(nas_nl_sk){
#ifdef KERNEL_VERSION_GREATER_THAN_2629 
    netlink_kernel_release(nas_nl_sk); //or skb->sk
#else
    sock_release(nas_nl_sk->sk_socket);
#endif
    
  }
  
 //  printk("[UE_IP_DRV][NETLINK] Removing netlink_rx_thread\n");
 //kthread_stop(netlink_rx_thread);

}




int ue_ip_netlink_send(unsigned char *data,unsigned int len) {


  struct sk_buff *nl_skb = alloc_skb(NLMSG_SPACE(len),GFP_ATOMIC);
  struct nlmsghdr *nlh = (struct nlmsghdr *)nl_skb->data;
  int status;


  //  printk("[UE_IP_DRV][NETLINK] Sending %d bytes (%d)\n",len,NLMSG_SPACE(len));
  skb_put(nl_skb, NLMSG_SPACE(len));
  memcpy(NLMSG_DATA(nlh),data,len);

  nlh->nlmsg_len = NLMSG_SPACE(len);


  nlh->nlmsg_pid = 0;      /* from kernel */

  NETLINK_CB(nl_skb).pid = 0;

#ifdef NETLINK_DEBUG
  printk("[UE_IP_DRV][NETLINK] In nas_netlink_send, nl_skb %p, nl_sk %x, nlh %p, nlh->nlmsg_len %d\n",nl_skb,nas_nl_sk,nlh,nlh->nlmsg_len);
#endif //DEBUG_NETLINK

  if (nas_nl_sk) {

    //  nasmesh_lock();
    status = netlink_unicast(nas_nl_sk, nl_skb, NL_DEST_PID, MSG_DONTWAIT);
    // mutex_unlock(&nasmesh_mutex);

    if (status < 0) {
	printk("[UE_IP_DRV][NETLINK] SEND status is %d\n",status);
	return(0);
    }
    else {
#ifdef NETLINK_DEBUG
      printk("[UE_IP_DRV][NETLINK] SEND status is %d\n",status);
#endif
      return len;
    }
  }
  else {
    printk("[UE_IP_DRV][SEND] socket is NULL\n");
    return(0);
  }
}
