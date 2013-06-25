/*! \file multicast.h
* \brief 
* \author Lionel Gauthier and Navid Nikaein 
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/select.h>
//#include "openair_defs.h"
//#include <sys/socket.h>
#include <netinet/in.h>
//#include "openair_defs.h"
#define MULTICAST_LINK_C

#include "socket.h"
#include "multicast_link.h"
#ifndef USER_MODE
#ifdef NODE_RG
//#include "mac_rg_bypass.h"
#endif
#ifdef NODE_MT
//#include "mac_ue_bypass.h"
#endif
#endif //USER_MODE

#ifdef USER_MODE
//#include <rtai.h>
# define msg printf
# include "UTIL/LOG/log.h"
#endif //USER_MODE
//#include "extern.h"
extern unsigned short Master_id;

#define MULTICAST_LINK_NUM_GROUPS 4

char *multicast_group_list[MULTICAST_LINK_NUM_GROUPS] = {
  "239.0.0.161\0",
  "239.0.0.162\0",
  "239.0.0.163\0",
  "239.0.0.164\0"
};

static multicast_group_t group_list[MULTICAST_LINK_NUM_GROUPS];

static fd_set   socks;          /* Socket file descriptors we want to wake up for, using select() */
static int      highsock;       /* Highest #'d file descriptor, needed for select() */
static pthread_t main_loop_thread;
static void (*rx_handler) (unsigned int, char*);
static unsigned char multicast_group; 
static char *multicast_if; 

//------------------------------------------------------------------------------
void
multicast_link_init ()
{
//------------------------------------------------------------------------------
  int             group;
  int             multicast_loop;
  int             reuse_addr = 1;       /* Used so we can re-bind to our port
                                           while a previous connection is still
                                           in TIME_WAIT state. */
  static struct ip_mreq command;
  struct sockaddr_in sin;
  // struct ifreq ifr;
  
  for (group = 0; group < MULTICAST_LINK_NUM_GROUPS; group++) {
    strcpy (group_list[group].host_addr, multicast_group_list[group]);
    group_list[group].port = 46014 + group;
    group_list[group].socket = make_socket_inet (SOCK_DGRAM, &group_list[group].port, &sin);
    msg("multicast_link_init(): Created socket %d for group %d, port %d\n",
	   group_list[group].socket,group,group_list[group].port);
    if (setsockopt (group_list[group].socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof (reuse_addr)) < 0) {
            msg ("[MULTICAST] ERROR : setsockopt:SO_REUSEADDR, exiting ...");
      exit (EXIT_FAILURE);
    }
    if (multicast_if != NULL) {
      /*   memset(&ifr, 0, sizeof(struct ifreq));
      snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), multicast_if);
      ioctl(group_list[group].socket, SIOCGIFINDEX, &ifr);
      if (setsockopt (group_list[group].socket, SOL_SOCKET,SO_BINDTODEVICE,(void*)&ifr, sizeof(struct ifreq)) < 0) { */
      if (setsockopt (group_list[group].socket, SOL_SOCKET,SO_BINDTODEVICE,multicast_if, 4) < 0) {
	msg ("[MULTICAST] ERROR : setsockopt:SO_BINDTODEVICE on interface %s, exiting ...\n", multicast_if);
	msg ("[MULTICAST] make sure that you have a root privilage or run with sudo -E \n");
	exit (EXIT_FAILURE);
      }
    }
    socket_setnonblocking (group_list[group].socket);

    //
    multicast_loop = 0;
    if (setsockopt (group_list[group].socket, IPPROTO_IP, IP_MULTICAST_LOOP, &multicast_loop, sizeof (multicast_loop)) < 0) {
      msg ("[MULTICAST] ERROR: %s line %d multicast_link_main_loop() IP_MULTICAST_LOOP %m", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    // Join the broadcast group:
    command.imr_multiaddr.s_addr = inet_addr (group_list[group].host_addr);
    command.imr_interface.s_addr = htonl (INADDR_ANY);
    if (command.imr_multiaddr.s_addr == -1) {
       msg ("[MULTICAST] ERROR: %s line %d NO MULTICAST", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    if (setsockopt (group_list[group].socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof (command)) < 0) {
       msg ("[MULTICAST] ERROR: %s line %d IP_ADD_MEMBERSHIP %m", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }


    memset (&group_list[group].sock_remote_addr, 0, sizeof (struct sockaddr_in));
    group_list[group].sock_remote_addr.sin_family = AF_INET;
    group_list[group].sock_remote_addr.sin_addr.s_addr = inet_addr (multicast_group_list[group]);
    group_list[group].sock_remote_addr.sin_port = htons (group_list[group].port);
  }
}

//------------------------------------------------------------------------------
void
multicast_link_build_select_list ()
{
//------------------------------------------------------------------------------
  int             group;

  /* First put together fd_set for select(), which will
     consist of the sock veriable in case a new connection
     is coming in, plus all the sockets we have already
     accepted. */


  /* FD_ZERO() clears out the fd_set called socks, so that
     it doesn't contain any file descriptors. */

  FD_ZERO (&socks);


  /* Loops through all the possible connections and adds
     those sockets to the fd_set */

  for (group = 0; group < MULTICAST_LINK_NUM_GROUPS; group++) {
    if (group_list[group].socket != 0) {
      FD_SET (group_list[group].socket, &socks);
      if (group_list[group].socket > highsock)
        highsock = group_list[group].socket;
    }
  }
}
//------------------------------------------------------------------------------
void
multicast_link_read_data (int groupP)
{
//------------------------------------------------------------------------------

#ifdef BYPASS_PHY
  //pthread_mutex_lock(&Bypass_phy_wr_mutex);
 // while(Bypass_phy_wr){
  //  msg("[Multicast read] BYPASS_PHY_NOT_YET READY, Waiting for signal\n");
   // pthread_cond_wait(&Bypass_phy_wr_cond,&Bypass_phy_wr_mutex);
 // }
       //msg("[Multicast read] BYPASS_PHY TX Signal\n");
       //pthread_mutex_unlock(&Bypass_phy_wr_mutex);
       //pthread_mutex_lock(&Bypass_phy_wr_mutex);
 // Bypass_phy_wr=1;
 // pthread_mutex_unlock(&Bypass_phy_wr_mutex);
#endif //BYPASS_PHY

  int num_bytes;
  //msg("multicast link read INNNNNNNNNNNNNNNNNN\n");
     // msg("multicast_link_read_data: groupP=%d,rx_buffer = %p\n",groupP,group_list[groupP].rx_buffer );
  if ((groupP  <= MULTICAST_LINK_NUM_GROUPS) && (groupP >= 0)) {
    if ((num_bytes = recvfrom (group_list[groupP].socket, group_list[groupP].rx_buffer, 40000, 0, 0, 0)) < 0) {
      fprintf (stderr, "ERROR: %s line %d multicast_link_read_data()/recvfrom() %m", __FILE__, __LINE__);
    } else {
      //    msg("multicast_link_read_data: groupP=%d,rx_buffer = %p,NUm_bytes=%d\n",groupP,group_list[groupP].rx_buffer,num_bytes );
      //msg("MULTICAST calling rx_handler\n");
       rx_handler(num_bytes,group_list[groupP].rx_buffer);
    }
  }
  else {
    fprintf(stderr,"ERROR: groupP out of bounds %d\n",groupP);
  }
  //msg("ENNNNND multicast_link_read_data: groupP=%d,rx_buffer = %p, num_bytes=%d\n",groupP,group_list[groupP].rx_buffer,num_bytes );
    
}
//------------------------------------------------------------------------------
void
multicast_link_read ()
{
//------------------------------------------------------------------------------
  int             group;        /* Current item in connectlist for for loops */

  /* Now check connectlist for available data */

  /* Run through our sockets and check to see if anything
     happened with them, if so 'service' them. */
  
  for (group = multicast_group; group < MULTICAST_LINK_NUM_GROUPS ; group++) {
    if (FD_ISSET (group_list[group].socket, &socks)) {
      multicast_link_read_data (group);    
      break;
    }
  }                             /* for (all entries in queue) */
}

//------------------------------------------------------------------------------
int
multicast_link_write_sock (int groupP, char *dataP, uint32_t sizeP)
{
//------------------------------------------------------------------------------
  int             num;
  if ((num = sendto (group_list[groupP].socket, dataP, sizeP, 0, (struct sockaddr *) &group_list[groupP].sock_remote_addr, sizeof (group_list[groupP].sock_remote_addr))) < 0) {
    fprintf (stderr, "ERROR: %s line %d multicast_link_write_sock()/sendto() %m", __FILE__, __LINE__);
  }
  return num;
}

//------------------------------------------------------------------------------
void           *
multicast_link_main_loop (void *param)
{
//------------------------------------------------------------------------------
  struct timeval  timeout;      /* Timeout for select */
  int             readsocks;    /* Number of sockets ready for reading */
#ifdef USER_MODE

#endif //USER_MODE
   highsock = -1;
   while (1) {
    multicast_link_build_select_list ();
    /* [24/06/13] SR: Set the timeout to one second ->
     * avoid infinite loop if no data to read.
     */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    readsocks = select (highsock + 1, &socks, (fd_set *) 0, (fd_set *) 0, &timeout);
    if (readsocks < 0) {
      LOG_E(EMU, "Multicast select failed (%d:%s)\n", errno, strerror(errno));
      // exit();  
    } else if (readsocks > 0) {
      //msg("calling multicast link read\n");
      multicast_link_read ();
      // usleep(1);
    } else {
      /* Timeout */
      LOG_I(EMU, "Multicast select time-out\n");
    }
  }
#ifdef USER_MODE

#endif //USER_MODE
}

//-----------------------------------------------------------------------------
void
multicast_link_start (  void (*rx_handlerP) (unsigned int, char*), unsigned char multicast_group, char * multicast_ifname)
{
  //-----------------------------------------------------------------------------
  rx_handler = rx_handlerP;
  multicast_group = multicast_group;
  multicast_if =  multicast_ifname;
  msg("[MULTICAST] LINK START on interface=%s for group=%d: handler=%p\n",
      (multicast_if == NULL) ? "not specified" : multicast_if, multicast_group, rx_handler);
#ifdef BYPASS_PHY  
  //  pthread_mutex_init(&Bypass_phy_wr_mutex,NULL);
  //pthread_cond_init(&Bypass_phy_wr_cond,NULL);
  //Bypass_phy_wr=0;
#endif //BYPASS_PHY
  multicast_link_init ();
  msg("[MULTICAST] multicast link start thread\n");
  if (pthread_create (&main_loop_thread, NULL, multicast_link_main_loop, NULL) != 0) {
    msg ("[MULTICAST LINK] Thread started\n");
    exit (-2);
  } else {
    pthread_detach (main_loop_thread);  // disassociate from parent
    msg ("[MULTICAST LINK] Thread detached\n");
   
  }
}
