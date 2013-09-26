/***************************************************************************
                          rrc_ue_nas_intf.c  -
                          -------------------
    begin                : Tue Jan 15 2002
    copyright            : (C) 2002, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
 **************************************************************************
    This file contains the functions used to interface the NAS
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"
#include "rtos_header.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_nas_sap.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
//-----------------------------------------------------------------------------
// For FIFOS / NETLINK interface
#ifdef USER_MODE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
 #ifdef RRC_NETLINK
 #include <sys/socket.h>
 #include <linux/netlink.h>
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #endif
#endif

extern int rrc_ue_mobileId;

#ifdef RRC_NETLINK
//#define MAX_PAYLOAD 4096  /* this sould cover the max mtu size*/
struct nlmsghdr *rrcnl_nlh;
int rrcnl_sock_fd;
struct sockaddr_nl rrcnl_src_addr, rrcnl_dest_addr;
struct iovec rrcnl_iov;
struct msghdr rrcnl_msg;
#endif


#ifdef USER_MODE
#ifndef RRC_NETLINK
//-----------------------------------------------------------------------------
// Create and initialize FIFOs for UE RRC SAPs
void rrc_ue_sap_init (void){
//-----------------------------------------------------------------------------
  int  write_flag = O_WRONLY | O_NONBLOCK | O_NDELAY;
  int  read_flag = O_RDONLY | O_NONBLOCK | O_NDELAY;
  char gcsap[40], ntsap[40], dcsap_in[40], dcsap_out[40];

  sprintf (gcsap, "%s%d", RRC_SAPI_UE_GCSAP, rrc_ue_mobileId);
  sprintf (ntsap, "%s%d", RRC_SAPI_UE_NTSAP, rrc_ue_mobileId);
  sprintf (dcsap_in, "%s%d", RRC_SAPI_UE_DCSAP_IN, rrc_ue_mobileId);
  sprintf (dcsap_out, "%s%d", RRC_SAPI_UE_DCSAP_OUT, rrc_ue_mobileId);

  // Create FIFOs
  rrc_create_fifo (gcsap);
  rrc_create_fifo (ntsap);
  rrc_create_fifo (dcsap_in);
  rrc_create_fifo (dcsap_out);

  // Open FIFOs
  while ((protocol_ms->rrc.rrc_ue_GC_fifo = open (gcsap, write_flag)) < 0) {
    msg ("%s returned value %d\n", gcsap, protocol_ms->rrc.rrc_ue_GC_fifo);
    sleep (1);
  }

  while ((protocol_ms->rrc.rrc_ue_NT_fifo = open (ntsap, write_flag)) < 0) {
    msg ("%s returned value %d\n", ntsap, protocol_ms->rrc.rrc_ue_NT_fifo);
    sleep (1);
  }

  while ((protocol_ms->rrc.rrc_ue_DCIn_fifo = open (dcsap_in, read_flag)) < 0) {
    msg ("%s returned value %d\n", dcsap_in, protocol_ms->rrc.rrc_ue_DCIn_fifo);
    sleep (1);
  }

  while ((protocol_ms->rrc.rrc_ue_DCOut_fifo = open (dcsap_out, write_flag)) < 0) {
    msg ("%s returned value %d\n", dcsap_out, protocol_ms->rrc.rrc_ue_DCOut_fifo);
    sleep (1);
  }

  // Print result
  msg ("[RRC] %s returned value %d\n", gcsap, protocol_ms->rrc.rrc_ue_GC_fifo);
  msg ("[RRC] %s returned value %d\n", ntsap, protocol_ms->rrc.rrc_ue_NT_fifo);
  msg ("[RRC] %s returned value %d\n", dcsap_in, protocol_ms->rrc.rrc_ue_DCIn_fifo);
  msg ("[RRC] %s returned value %d\n", dcsap_out, protocol_ms->rrc.rrc_ue_DCOut_fifo);
}
#else
//-----------------------------------------------------------------------------
// Create and initialize NETLINK Sockets for UE RRC SAPs
void rrc_ue_netlink_init (void){
//-----------------------------------------------------------------------------
  int ret;

  rrcnl_sock_fd = socket(PF_NETLINK, SOCK_RAW, NAS_RRCNL_ID);
  if (rrcnl_sock_fd == -1) {
    fprintf(stderr, "socket failed (%d:%s)\n", errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("rrc_ue_netlink_init - Opened socket with fd %d\n", rrcnl_sock_fd);

  ret = fcntl(rrcnl_sock_fd,F_SETFL,O_NONBLOCK);
  printf("rrc_ue_netlink_init - fcntl returns %d\n",ret);

  memset(&rrcnl_src_addr, 0, sizeof(rrcnl_src_addr));
  rrcnl_src_addr.nl_family = AF_NETLINK;
  rrcnl_src_addr.nl_pid = NL_DEST_RRC_PID;//getpid();  /* self pid */
  rrcnl_src_addr.nl_groups = 0;  /* not in mcast groups */
  ret = bind(rrcnl_sock_fd, (struct sockaddr*)&rrcnl_src_addr, sizeof(rrcnl_src_addr));
  printf("rrc_ue_netlink_init - bind returns %d\n",ret);

  memset(&rrcnl_dest_addr, 0, sizeof(rrcnl_dest_addr));
  rrcnl_dest_addr.nl_family = AF_NETLINK;
  rrcnl_dest_addr.nl_pid = 0;   /* For Linux Kernel */
  rrcnl_dest_addr.nl_groups = 0; /* unicast */

  rrcnl_nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(RRC_NAS_MAX_SIZE));
  //memset(rrcnl_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  /* Fill the netlink message header */
  rrcnl_nlh->nlmsg_len = NLMSG_SPACE(RRC_NAS_MAX_SIZE);
  rrcnl_nlh->nlmsg_pid = NL_DEST_RRC_PID;//getpid();  /* self pid */
  rrcnl_nlh->nlmsg_flags = 0;

  rrcnl_iov.iov_base = (void *)rrcnl_nlh;
  rrcnl_iov.iov_len = rrcnl_nlh->nlmsg_len;
  memset(&rrcnl_msg,0,sizeof(rrcnl_msg));
  rrcnl_msg.msg_name = (void *)&rrcnl_dest_addr;
  rrcnl_msg.msg_namelen = sizeof(rrcnl_dest_addr);
  rrcnl_msg.msg_iov = &rrcnl_iov;
  rrcnl_msg.msg_iovlen = 1;

  /* Read message from kernel */
  memset(rrcnl_nlh, 0, NLMSG_SPACE(RRC_NAS_MAX_SIZE));
  // Initialize FIFO values (to be used for SAP indicator in netlink xmit)
  protocol_ms->rrc.rrc_ue_GC_fifo = RRC_NAS_GC_OUT;
  protocol_ms->rrc.rrc_ue_NT_fifo = RRC_NAS_NT_OUT;
  protocol_ms->rrc.rrc_ue_DCOut_fifo = RRC_NAS_DC0_OUT;
}
#endif //RRC_NETLINK
#endif //USER_MODE

#ifndef RRC_NETLINK
//-----------------------------------------------------------------------------
// This function sends data from RRC to the NAS
void rrc_ue_write_FIFO (mem_block_t * p){
//-----------------------------------------------------------------------------
  int count = 0;
  int xmit_length;
  char *xmit_ptr;

  // transmit the primitive
  xmit_length = ((struct nas_ue_if_element *) p->data)->prim_length;
  xmit_ptr = (char *) &((struct nas_ue_if_element *) p->data)->nasUePrimitive;
  count = rtf_put (((struct nas_ue_if_element *) p->data)->xmit_fifo, xmit_ptr, xmit_length);

  if (count == xmit_length) {
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE][NAS] NAS primitive sent successfully, length %d \n", count);
    //msg("\n[RRC_UE][NAS] on FIFO, %d \n", ((struct nas_ue_if_element *) p->data)->xmit_fifo);
  #endif
    protocol_ms->rrc.NASMessageToXmit = p->next;        //Dequeue next message if any
    free_mem_block (p);
  #ifndef USER_MODE
    if (protocol_ms->rrc.ip_rx_irq > 0) {   //Temp - later a specific control irq
      rt_pend_linux_srq (protocol_ms->rrc.ip_rx_irq);
    } else {
      msg ("[RRC_UE] ERROR IF IP STACK WANTED NOTIF PACKET(S) ip_rx_irq not initialized\n");
    }
  #endif
  } else {
    msg ("[RRC_UE][NAS] transmission on FIFO failed, %d bytes sent\n", count);
  }
}
#else
//-----------------------------------------------------------------------------
// This function sends data from RRC to the NAS via Netlink socket 
// Name has been kept rrc_ue_write_FIFO for backwards compatibility
void rrc_ue_write_FIFO (mem_block_t * p){
//-----------------------------------------------------------------------------
  int count = 0;
  int xmit_length;
  char *xmit_ptr;
  int ret;
  char sap_to_write;

  sap_to_write = (char) ((struct nas_ue_if_element *) p->data)->xmit_fifo;

  // transmit the SAP value
  memcpy (NLMSG_DATA(rrcnl_nlh), &sap_to_write, 1);
  //msg ("[RRC_UE][NAS] rrc_ue_write_FIFO - xmit_fifo %d - sap_to_write %d\n", ((struct nas_ue_if_element *) p->data)->xmit_fifo, sap_to_write);

  // transmit the primitive
  xmit_length = ((struct nas_ue_if_element *) p->data)->prim_length;
  xmit_ptr = (char *) &((struct nas_ue_if_element *) p->data)->nasUePrimitive;
  memcpy (NLMSG_DATA(rrcnl_nlh)+1, xmit_ptr, xmit_length);
  rrcnl_nlh->nlmsg_len = xmit_length+1;
  //rrc_print_buffer (xmit_ptr, xmit_length);

  ret = sendmsg(rrcnl_sock_fd,&rrcnl_msg,0);
  if (ret<0) {
    msg ("[RRC_UE][NAS] rrc_ue_write_FIFO - sendmsg returns %d (errno: %d)\n", ret, errno);
    mac_xface->macphy_exit("RRC Netlink Socket could not be written");
  }else{
    count = xmit_length+1;
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE][NAS] NAS primitive sent successfully, length %d (including NETLINK SAP value)\n", count);
    //rrc_print_buffer (NLMSG_DATA(rrcnl_nlh), count);
    //msg("\n[RRC_UE][NAS] on FIFO, %d \n", ((struct nas_ue_if_element *) p->data)->xmit_fifo);
    #endif
    protocol_ms->rrc.NASMessageToXmit = p->next;        //Dequeue next message if any
    free_mem_block (p);
  }
}
#endif

//-----------------------------------------------------------------------------
// Enqueue a message for NAS -- NOT IMPLEMENTED YET
void rrc_ue_nas_xmit_enqueue (mem_block_t * p){
//-----------------------------------------------------------------------------
  protocol_ms->rrc.NASMessageToXmit = p;
}

//-----------------------------------------------------------------------------
// This function receives data in RRC from the NAS
void rrc_ue_read_FIFO (void){
//-----------------------------------------------------------------------------
  int count = 0;
  u8  rcve_buffer[RRC_NAS_MAX_SIZE];
  struct nas_ue_dc_element *p;
  int prim_length;
  int prim_type;
  char sap_to_read;

  memset (rcve_buffer, 0, RRC_NAS_MAX_SIZE);
  // Read Message header
  #ifndef RRC_NETLINK
  count = rtf_get (protocol_ms->rrc.rrc_ue_DCIn_fifo, rcve_buffer, NAS_TL_SIZE);
  if (count  > 0) 
    sap_to_read = RRC_NAS_DC0_IN;  // This should be replicated if read from several SAPs
  #else
  count = recvmsg(rrcnl_sock_fd, &rrcnl_msg, 0);
  #ifdef DEBUG_RRC_DETAILS_2
  if (!(protocol_ms->rrc.current_SFN%50)){
    msg ("[RRC_UE][NETLINK] rrc_ue_read_FIFO - count = %d\n", count);
  }
  #endif
  if (count  > 0){ 
    msg ("[RRC_UE][NETLINK] Received socket with length %d (nlmsg_len = %d)\n", count, (rrcnl_nlh->nlmsg_len)-sizeof(struct nlmsghdr));
    sap_to_read = ((char*)NLMSG_DATA(rrcnl_nlh))[0];
    if (sap_to_read != RRC_NAS_DC0_IN){
      return;
    }
    memcpy(rcve_buffer, &((char*)NLMSG_DATA(rrcnl_nlh))[1], NAS_TL_SIZE);
    count --;
  }
  #endif

  if (count  > 0){
    // Message received in DC-SAP - Continue
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE] Message Received from NAS: -%hx- \n", rcve_buffer[0]);
    #endif

    p = (struct nas_ue_dc_element *) rcve_buffer;
    prim_length = (int) (p->length);
    prim_type = (int) (p->type);
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_UE] Primitive Type %d,\t Primitive length %d \n", prim_type, prim_length);
    #endif
    //get the rest of the primitive
    #ifndef RRC_NETLINK
    count += rtf_get (protocol_ms->rrc.rrc_ue_DCIn_fifo, &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);
    #else
    memcpy(&(rcve_buffer[NAS_TL_SIZE]), &((char*)NLMSG_DATA(rrcnl_nlh))[1+NAS_TL_SIZE], prim_length - NAS_TL_SIZE);
    count = prim_length;
    #endif
    rrc_ue_read_DCin_FIFO (p, count);
  }
}


#ifdef RRC_NETLINK
//-----------------------------------------------------------------------------
// This function receives data in RRC from the NAS
int rrc_ue_read_data_from_nlh (char * buffer, int length, int offset){
//-----------------------------------------------------------------------------
  int count = length;
  memcpy(buffer, &((char*)NLMSG_DATA(rrcnl_nlh))[1+offset], count);
  return count;
}
#endif