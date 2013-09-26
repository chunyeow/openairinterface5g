/***************************************************************************
                          rrc_rg_nas_intf.c  -
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
#include "rrc_rg_vars.h"
#include "rrc_nas_sap.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"

//-----------------------------------------------------------------------------
// For FIFOS interface
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
void rrc_rg_sap_init (void){
//-----------------------------------------------------------------------------
  int write_flag = O_WRONLY | O_NONBLOCK | O_NDELAY;
  int read_flag = O_RDONLY | O_NONBLOCK | O_NDELAY;

  // Create FIFOs
  rrc_create_fifo (RRC_SAPI_RG_GCSAP);
  rrc_create_fifo (RRC_SAPI_RG_NTSAP);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP0_IN);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP0_OUT);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP1_IN);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP1_OUT);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP2_IN);
  rrc_create_fifo (RRC_SAPI_RG_DCSAP2_OUT);

  // Open FIFOs
  while ((protocol_bs->rrc.rrc_rg_GC_fifo = open (RRC_SAPI_RG_GCSAP, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_GCSAP, protocol_bs->rrc.rrc_rg_GC_fifo);
    perror("RRC_SAPI_RG_GCSAP - open failed: ");
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_GCSAP);

  while ((protocol_bs->rrc.rrc_rg_NT_fifo = open (RRC_SAPI_RG_NTSAP, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_NTSAP, protocol_bs->rrc.rrc_rg_NT_fifo);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_NTSAP);

// Currently 3 MT supported -- will become a loop ???
  while ((protocol_bs->rrc.rrc_rg_DCIn_fifo[0] = open (RRC_SAPI_RG_DCSAP0_IN, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP0_IN, protocol_bs->rrc.rrc_rg_DCIn_fifo[0]);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP0_IN);

  while ((protocol_bs->rrc.rrc_rg_DCOut_fifo[0] = open (RRC_SAPI_RG_DCSAP0_OUT, write_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP0_OUT, protocol_bs->rrc.rrc_rg_DCOut_fifo[0]);
    perror("RRC_SAPI_RG_DCSAP0_OUT - open failed: ");
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP0_OUT);

  while ((protocol_bs->rrc.rrc_rg_DCIn_fifo[1] = open (RRC_SAPI_RG_DCSAP1_IN, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP1_IN, protocol_bs->rrc.rrc_rg_DCIn_fifo[1]);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP1_IN);

  while ((protocol_bs->rrc.rrc_rg_DCOut_fifo[1] = open (RRC_SAPI_RG_DCSAP1_OUT, write_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP1_OUT, protocol_bs->rrc.rrc_rg_DCOut_fifo[1]);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP1_OUT);

  while ((protocol_bs->rrc.rrc_rg_DCIn_fifo[2] = open (RRC_SAPI_RG_DCSAP2_IN, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP2_IN, protocol_bs->rrc.rrc_rg_DCIn_fifo[2]);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP2_IN);

  while ((protocol_bs->rrc.rrc_rg_DCOut_fifo[2] = open (RRC_SAPI_RG_DCSAP2_OUT, write_flag)) < 0) {
    msg ("%s returned value %d\n", RRC_SAPI_RG_DCSAP2_OUT, protocol_bs->rrc.rrc_rg_DCOut_fifo[2]);
    sleep (1);
  }
  msg("FIFO opened %s\n", RRC_SAPI_RG_DCSAP2_OUT);

  // Print result
  msg ("%s returned value %d\n", "RRC_SAPI_RG_GCSAP", protocol_bs->rrc.rrc_rg_GC_fifo);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_NTSAP", protocol_bs->rrc.rrc_rg_NT_fifo);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP0_IN", protocol_bs->rrc.rrc_rg_DCIn_fifo[0]);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP0_OUT", protocol_bs->rrc.rrc_rg_DCOut_fifo[0]);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP1_IN", protocol_bs->rrc.rrc_rg_DCIn_fifo[1]);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP1_OUT", protocol_bs->rrc.rrc_rg_DCOut_fifo[1]);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP2_IN", protocol_bs->rrc.rrc_rg_DCIn_fifo[2]);
  msg ("%s returned value %d\n", "RRC_SAPI_RG_DCSAP2_OUT", protocol_bs->rrc.rrc_rg_DCOut_fifo[2]);
}
#else
//-----------------------------------------------------------------------------
// Create and initialize NETLINK Sockets for UE RRC SAPs
void rrc_rg_netlink_init (void){
//-----------------------------------------------------------------------------
  int ret;


  rrcnl_sock_fd = socket(PF_NETLINK, SOCK_RAW, NAS_RRCNL_ID);
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
  protocol_bs->rrc.rrc_rg_DCOut_fifo[0] = RRC_NAS_DC0_OUT;
  protocol_bs->rrc.rrc_rg_DCOut_fifo[1] = RRC_NAS_DC1_OUT;
  protocol_bs->rrc.rrc_rg_DCOut_fifo[2] = RRC_NAS_DC2_OUT;
}
#endif //RRC_NETLINK

#endif

#ifndef RRC_NETLINK
//-----------------------------------------------------------------------------
// This function sends data from RRC to the NAS
void rrc_rg_write_FIFO (mem_block_t *p){
//-----------------------------------------------------------------------------
  int count = 0;
  int xmit_length;
//  int message_type;
  char *xmit_ptr;

  // transmit the primitive
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_DEBUG] Message to transmit in DC FIFO\n");
  #endif
  xmit_length = ((struct nas_rg_if_element *) p->data)->prim_length;
  xmit_ptr = (char *) &((struct nas_rg_if_element *) p->data)->nasRgPrimitive;
  if (xmit_ptr != NULL) {
    count = rtf_put (((struct nas_rg_if_element *) p->data)->xmit_fifo, xmit_ptr, xmit_length);
  }else{
    count = 0;
  }
  if (count == xmit_length) {
    #ifdef DEBUG_RRC_DETAILS
     msg ("[RRC_RG][NAS] NAS primitive sent successfully, length %d \n", count);
     msg("\n[RRC_RG][NAS] on FIFO, %d \n", ((struct nas_rg_if_element *) p->data)->xmit_fifo);
    #endif
    protocol_bs->rrc.NASMessageToXmit = p->next;        //Dequeue next message if any
    free_mem_block (p);
    #ifndef USER_MODE
    if (protocol_bs->rrc.ip_rx_irq > 0) {
      rt_pend_linux_srq (protocol_bs->rrc.ip_rx_irq);
    }else{
      msg ("[RRC_RG] ERROR IF IP STACK WANTED NOTIF PACKET(S) ip_rx_irq not initialized\n");
    }
    #endif
  }else{
    msg ("[RRC_RG][NAS] transmission on FIFO failed, %d bytes sent\n", count);
  }
}
#else
//-----------------------------------------------------------------------------
// This function sends data from RRC to the NAS via Netlink socket 
// Name has been kept rrc_ue_write_FIFO for backwards compatibility
void rrc_rg_write_FIFO (mem_block_t * p){
//-----------------------------------------------------------------------------
  int count = 0;
  int xmit_length;
  char *xmit_ptr;
  int ret;
  char sap_to_write;

  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_DEBUG] Message to transmit in DC FIFO\n");
  #endif

  sap_to_write = (char) ((struct nas_rg_if_element *) p->data)->xmit_fifo;

  // transmit the SAP value
  memcpy (NLMSG_DATA(rrcnl_nlh), &sap_to_write, 1);

  // transmit the primitive
  xmit_length = ((struct nas_rg_if_element *) p->data)->prim_length;
  xmit_ptr = (char *) &((struct nas_rg_if_element *) p->data)->nasRgPrimitive;
  if (xmit_ptr != NULL) {
  memcpy (NLMSG_DATA(rrcnl_nlh)+1, xmit_ptr, xmit_length);
  }
  rrcnl_nlh->nlmsg_len = xmit_length+1;
  //rrc_print_buffer (xmit_ptr, xmit_length);

  ret = sendmsg(rrcnl_sock_fd,&rrcnl_msg,0);
  if (ret<0) {
    msg ("[RRC_RG][NAS] rrc_rg_write_FIFO - sendmsg returns %d (errno: %d)\n", ret, errno);
    mac_xface->macphy_exit("RRC Netlink Socket could not be written");
  }else{
    count = xmit_length+1;
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][NAS] NAS primitive sent successfully, length %d (including NETLINK SAP value)\n", count);
    //rrc_print_buffer (NLMSG_DATA(rrcnl_nlh), count);
    //msg("\n[RRC_UE][NAS] on FIFO, %d \n", ((struct nas_rg_if_element *) p->data)->xmit_fifo);
    #endif
    protocol_bs->rrc.NASMessageToXmit = p->next;        //Dequeue next message if any
    free_mem_block (p);
  }
}
#endif
//-----------------------------------------------------------------------------
// Enqueue a message for NAS
void rrc_rg_nas_xmit_enqueue (mem_block_t * p){
//-----------------------------------------------------------------------------
  protocol_bs->rrc.NASMessageToXmit = p;
}

//-----------------------------------------------------------------------------
// This function receives data in RRC from the NAS
void rrc_rg_read_FIFO (void){
//-----------------------------------------------------------------------------
  int count = 0;
  u8  rcve_buffer[RRC_NAS_MAX_SIZE];
  #ifdef RRC_NETLINK
  struct nas_rg_dc_element *p;
  #endif
  u16 prim_length;
  char sap_to_read;
  #ifndef RRC_NETLINK
  int i;
  #endif

  memset (rcve_buffer, 0, RRC_NAS_MAX_SIZE);
  // Read Message header
  #ifndef RRC_NETLINK
  for (i = 0; i < maxUsers; i++) {
    if ((count = rtf_get (protocol_bs->rrc.rrc_rg_DCIn_fifo[i], rcve_buffer, NAS_TL_SIZE)) > 0) {
      sap_to_read = RRC_NAS_DC0_IN + (2*i);
      prim_length = (int) (((struct nas_rg_dc_element *) rcve_buffer)->length);
      count += rtf_get (protocol_bs->rrc.rrc_rg_DCIn_fifo[i], &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);
      rrc_rg_read_DCin_FIFO (i, rcve_buffer, count);
      break;
    }
  }
  //msg ("[RRC_RG]rrc_rg_read_FIFO - After DC-SAPs - count = %d\n", count);
  if (count<0){
    if ((count = rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, rcve_buffer, NAS_TL_SIZE)) > 0) {
      sap_to_read = RRC_NAS_GC_IN;
     //msg ("[RRC_RG]rrc_rg_read_FIFO - GC SAP - count = %d\n", count);
     prim_length =  ((struct nas_rg_gc_element *) rcve_buffer)->length;
      count += rtf_get (protocol_bs->rrc.rrc_rg_GC_fifo, &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);
      rrc_rg_read_GC_FIFO (rcve_buffer, count);
    }else{
      if ((count = rtf_get (protocol_bs->rrc.rrc_rg_NT_fifo, rcve_buffer, NAS_TL_SIZE)) > 0) {
        sap_to_read = RRC_NAS_NT_IN;
        prim_length =  ((struct nas_rg_nt_element *) rcve_buffer)->length;
        count += rtf_get (protocol_bs->rrc.rrc_rg_NT_fifo, &(rcve_buffer[NAS_TL_SIZE]), prim_length - NAS_TL_SIZE);
        rrc_rg_read_NT_FIFO (rcve_buffer, count);
      }
    }
  }
  #else
  count = recvmsg(rrcnl_sock_fd, &rrcnl_msg, 0);
/*  if (!(protocol_bs->rrc.current_SFN%50)){
    msg ("[RRC_RG][NETLINK] rrc_rg_read_FIFO - count = %d\n", count);
  }*/
  if (count  > 0){ 
    msg ("[RRC_RG][NETLINK] Received socket with length %d (nlmsg_len = %d)\n", count, (rrcnl_nlh->nlmsg_len)-sizeof(struct nlmsghdr));
    sap_to_read = ((char*)NLMSG_DATA(rrcnl_nlh))[0];
    p = (struct nas_rg_dc_element *)&((char*)NLMSG_DATA(rrcnl_nlh))[1];
    //memcpy(rcve_buffer, &((char*)NLMSG_DATA(rrcnl_nlh))[1], prim_length);
    //prim_length = (int) (((struct nas_rg_dc_element *) rcve_buffer)->length);
    prim_length = p->length;
    msg ("[RRC_RG][NETLINK] SAP %d, prim_length %d\n", sap_to_read, prim_length);
    memcpy(rcve_buffer, p, prim_length);
    count --;
    switch (sap_to_read){
      case RRC_NAS_GC_IN:
        rrc_rg_read_GC_FIFO (rcve_buffer, count);
        break;
      case RRC_NAS_NT_IN:
        rrc_rg_read_NT_FIFO (rcve_buffer, count);
        break;
      case RRC_NAS_DC0_IN:
        rrc_rg_read_DCin_FIFO (0, rcve_buffer, count);
        break;
      case RRC_NAS_DC1_IN:
        rrc_rg_read_DCin_FIFO (1, rcve_buffer, count);
        break;
      case RRC_NAS_DC2_IN:
        rrc_rg_read_DCin_FIFO (2, rcve_buffer, count);
        break;
        break;
      default:
        msg ("[RRC_RG][NETLINK] ERROR Invalid SAP received %d \n", sap_to_read);
        return;
    }
  }
  #endif

}


#ifdef RRC_NETLINK
//-----------------------------------------------------------------------------
// This function receives data in RRC from the NAS
int rrc_rg_read_data_from_nlh (char * buffer, int length, int offset){
//-----------------------------------------------------------------------------
  int count = length;
  memcpy(buffer, &((char*)NLMSG_DATA(rrcnl_nlh))[1+offset], count);
  return count;
}
#endif
