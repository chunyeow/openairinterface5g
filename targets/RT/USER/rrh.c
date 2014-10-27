/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
    included in this distribution in the file called "COPYING". If not, 
    see <http://www.gnu.org/licenses/>.

   Contact Information
   OpenAirInterface Admin: openair_admin@eurecom.fr
   OpenAirInterface Tech : openair_tech@eurecom.fr
   OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
   Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

 /** ethernet_lib : API to stream I/Q samples over standard ethernet (RRH component)
 * 
 *  Authors: Raymond Knopp  <raymond.knopp@eurecom.fr>
 * 
 *  Changelog:
 *  06.10.2014: Initial version
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <pthread.h>

#include "common_lib.h"

typedef struct {
  int port;
  char *dest_ip;
} rrh_desc_t;

int rrh_exit=0;

int32_t tx_buffer[4][(1+(sizeof(openair0_timestamp)>>2))+76800*4],rx_buffer[4][(1+(sizeof(openair0_timestamp)>>2))+76800*4];

void *rrh_eNB_thread(void *arg) {

  int sockid=-1;
  struct sockaddr_in serveraddr;
  struct sockaddr clientaddr;
  socklen_t clientaddrlen;
  rrh_desc_t *rrh_desc = (rrh_desc_t *)arg;
  char str[INET_ADDRSTRLEN];
  int8_t msg_header[4+sizeof(openair0_timestamp)];
  int16_t cmd,nsamps,antenna_index;
  ssize_t bytes_received;
  openair0_timestamp timestamp_tx,timestamp_rx=0;
  ssize_t bytes_sent;


  while (rrh_exit==0) {

    
    sockid=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sockid==-1) {
      perror("Cannot create socket: ");
      rrh_exit=1;
    }


    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(rrh_desc->port);
    inet_pton(AF_INET,"0.0.0.0",&serveraddr.sin_addr.s_addr);

    inet_ntop(AF_INET, &(serveraddr.sin_addr), str, INET_ADDRSTRLEN);      
    printf("Binding to socket for %s:%d\n",str,ntohs(serveraddr.sin_port));

    if (bind(sockid,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0) {
      perror("Cannot bind to socket: ");
      rrh_exit = 1;
    }
     
    // now wait for commands from eNB
    while (rrh_exit==0) {

      // get header info
      bzero((void*)&clientaddr,sizeof(struct sockaddr));
      clientaddrlen = sizeof(struct sockaddr);

      bytes_received = recvfrom(sockid,msg_header,4+sizeof(openair0_timestamp),0,&clientaddr,&clientaddrlen);
      cmd = msg_header[0];
      antenna_index = cmd>>1;
      timestamp_tx = *(openair0_timestamp*)(msg_header+4);
      nsamps = *(int16_t *)(msg_header+2);
      cmd = cmd&1;
      inet_ntop(AF_INET, &(((struct sockaddr_in*)&clientaddr)->sin_addr), str, INET_ADDRSTRLEN);

      switch (cmd) {
      case 0: // RX
	//	printf("Received RX request for antenna %d, nsamps %d (from %s:%d)\n",antenna_index,nsamps,str, 
	//	       ntohs(((struct sockaddr_in*)&clientaddr)->sin_port));
	// send return
	
	if ((bytes_sent = sendto(sockid, 
				&rx_buffer[antenna_index][timestamp_rx%(76800*4)],
				(nsamps<<2)+sizeof(openair0_timestamp), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	  perror("RRH: sendto for RX");
	//	printf("bytes_sent %d(timestamp_rx %d)\n",bytes_sent);
	timestamp_rx+=nsamps;
	break;
      case 1: // TX
	printf("Received TX request for antenna %d, nsamps %d, timestamp %llu\n",antenna_index,nsamps,timestamp_tx);
	bytes_received = recvfrom(sockid,&tx_buffer[antenna_index][timestamp_tx%(76800*4)],(nsamps<<2),0,NULL,NULL);
	printf("Received TX samples for antenna %d, nsamps %d (%d)\n",antenna_index,nsamps,(bytes_received>>2));
	break;
      }
      //      rrh_exit = 1;
    }
  }

  close(sockid);
  

  return(0);
}


void signal_handler(int sig)
{
  void *array[10];
  size_t size;

  if (sig==SIGSEGV) {
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
    
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, 2);
    exit(-1);
  }
  else {
    printf("trying to exit gracefully...\n"); 
    rrh_exit = 1;
  }
}

int main(int argc, char **argv) {
  
  pthread_t main_rrh_eNB_thread;
  pthread_attr_t attr;
  struct sched_param sched_param_rrh;
  int error_code;
  rrh_desc_t rrh;

  rrh.port = 22222;
  rrh.dest_ip = "127.0.0.1";

  pthread_attr_init(&attr);
  sched_param_rrh.sched_priority = sched_get_priority_max(SCHED_FIFO);


  pthread_attr_setschedparam(&attr,&sched_param_rrh);
  pthread_attr_setschedpolicy(&attr,SCHED_FIFO);

  error_code = pthread_create(&main_rrh_eNB_thread, &attr, rrh_eNB_thread, (void *)&rrh);

  printf("TYPE <CTRL-C> TO TERMINATE\n");

  while (rrh_exit==0)
    sleep(1);


}
