/*! \file netlink_init.c
* \brief initiate the netlink socket for communication with nas dirver 
* \author Navid Nikaein and Raymomd Knopp
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
*/ 

#include <sys/socket.h>
#include <linux/netlink.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#define MAX_PAYLOAD 4096  /* this sould cover the max mtu size*/
struct sockaddr_nl nas_src_addr, nas_dest_addr;
struct nlmsghdr *nas_nlh = NULL;
struct iovec nas_iov;
int nas_sock_fd;
struct msghdr nas_msg;

#define GRAAL_NETLINK_ID 31

int netlink_init(void) {


  int ret;

  
  nas_sock_fd = socket(PF_NETLINK, SOCK_RAW,GRAAL_NETLINK_ID);
  if (nas_sock_fd==-1) {
    printf("[NETLINK] Error opening socket %d\n",nas_sock_fd);
    return(-1);
  }  
  printf("[NETLINK]Opened socket with fd %d\n",nas_sock_fd);

  ret = fcntl(nas_sock_fd,F_SETFL,O_NONBLOCK);
  printf("[NETLINK] fcntl returns %d\n",ret);

  memset(&nas_src_addr, 0, sizeof(nas_src_addr));
  nas_src_addr.nl_family = AF_NETLINK;
  nas_src_addr.nl_pid = 1;//getpid();  /* self pid */
  nas_src_addr.nl_groups = 0;  /* not in mcast groups */
  ret = bind(nas_sock_fd, (struct sockaddr*)&nas_src_addr,
	     sizeof(nas_src_addr));

  printf("[NETLINK] bind returns %d\n",ret);

  memset(&nas_dest_addr, 0, sizeof(nas_dest_addr));
  nas_dest_addr.nl_family = AF_NETLINK;
  nas_dest_addr.nl_pid = 0;   /* For Linux Kernel */
  nas_dest_addr.nl_groups = 0; /* unicast */
  
  nas_nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  /* Fill the netlink message header */
  nas_nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nas_nlh->nlmsg_pid = 1;//getpid();  /* self pid */
  nas_nlh->nlmsg_flags = 0;
  
  nas_iov.iov_base = (void *)nas_nlh;
  nas_iov.iov_len = nas_nlh->nlmsg_len;
  memset(&nas_msg,0,sizeof(nas_msg));
  nas_msg.msg_name = (void *)&nas_dest_addr;
  nas_msg.msg_namelen = sizeof(nas_dest_addr);
  nas_msg.msg_iov = &nas_iov;
  nas_msg.msg_iovlen = 1;
  
  
  /* Read message from kernel */
  memset(nas_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  

  return(nas_sock_fd);
}








