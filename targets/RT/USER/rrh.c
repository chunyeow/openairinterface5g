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
 *  Authors: Raymond Knopp  <raymond.knopp@eurecom.fr>,  Riadh Ghaddab <riadh.ghaddab@eurecom.fr>
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
#include <time.h>

#include "common_lib.h"

#define BUF_LEN 4096+32
#define RRH_eNB_PORT 50000
#define RRH_eNB_DEST_IP "192.168.12.196"  //Haswell ip address
//#define RRH_eNB_DEST_IP "192.168.12.148"  //Hecatonchire ip address

#define RRH_UE_PORT 22222
//#define RRH_UE_DEST_IP "192.168.12.148"  //Hecatonchire ip address
#define RRH_UE_DEST_IP "192.168.12.196"  //Haswell ip address

#define FRAME_MAX_SIZE 307200 
#define DEFAULT_PERIOD_NS 133333

typedef struct {
  int eNB_port;
  char *eNB_dest_ip;
  int UE_port;
  char *UE_dest_ip;
  struct timespec time_req;  
} rrh_desc_t;

int rrh_exit=0;

int32_t tx_buffer_eNB[4][(1+(sizeof(openair0_timestamp)>>2))+FRAME_MAX_SIZE],rx_buffer_eNB[4][(1+(sizeof(openair0_timestamp)>>2))+FRAME_MAX_SIZE];
int32_t tx_buffer_UE[4][(1+(sizeof(openair0_timestamp)>>2))+FRAME_MAX_SIZE],rx_buffer_UE[4][(1+(sizeof(openair0_timestamp)>>2))+FRAME_MAX_SIZE];
void  *rrh_eNB_thread_status;
void  *rrh_UE_thread_status;
int32_t counter_UE_rx[4]={0,0,0,0};
int32_t counter_UE_tx[4]={0,0,0,0};
int32_t counter_eNB_rx[4]={0,0,0,0};
int32_t counter_eNB_tx[4]={0,0,0,0};
int32_t overflow_rx_buffer_UE[4]={0,0,0,0};
int32_t overflow_rx_buffer_eNB[4]={0,0,0,0};
int32_t nsamps_eNB[4]={0,0,0,0};
int32_t nsamps_UE[4]={0,0,0,0};
int32_t UE_tx_started=0,eNB_tx_started=0, UE_rx_started=0,eNB_rx_started=0;
int32_t RT_FLAG=0, NRT_FLAG=1;
openair0_timestamp nrt_eNB_counter[4]={0,0,0,0};
openair0_timestamp nrt_UE_counter[4]={0,0,0,0};

openair0_timestamp timestamp_eNB_tx[4],timestamp_eNB_rx[4]={0,0,0,0};
openair0_timestamp timestamp_UE_tx[4],timestamp_UE_rx[4]={0,0,0,0};
openair0_timestamp hw_counter=0;
 
pthread_cond_t sync_UE_cond[4];
pthread_mutex_t sync_UE_mutex[4];
pthread_cond_t sync_eNB_cond[4];
pthread_mutex_t sync_eNB_mutex[4];
pthread_mutex_t timer_mutex;

int sync_UE_rx[4]={-1,-1,-1,-1};
int sync_eNB_rx[4]={-1,-1,-1,-1};


void timer_signal_handler(int sig){

  if (sig == SIGALRM)
  {
    pthread_mutex_lock(&timer_mutex);
    hw_counter ++;
    //printf("[RRH] : hw_counter : %d\n",(int)hw_counter);
    pthread_mutex_unlock(&timer_mutex);
  }


}


void *timer_proc(void *arg) {
    timer_t             timerid;    // timer ID for timer
    //struct sigevent     event;      // event to deliver
    struct itimerspec   *timer = (struct itimerspec*)arg;      // the timer data structure
    struct itimerspec  *old_value;


  //printf("Starting the timer\n");
  if (timer_create (CLOCK_REALTIME, NULL, &timerid) == -1) {
        fprintf (stderr, "couldn't create a timer\n");
        perror (NULL);
        exit (EXIT_FAILURE);
    }

    signal(SIGALRM, timer_signal_handler);
    // and start it!
    timer_settime (timerid, 0, timer, old_value);

   while (!rrh_exit)
    {
     sleep(1);
    }
timer_delete(timerid);
return (0);


}


void *rrh_proc_eNB_thread() {

  //rrh_desc_t *rrh_desc = (rrh_desc_t *)arg;
  int antenna_index;
  openair0_timestamp truncated_timestamp, truncated_timestamp_final, last_hw_counter=0;
  struct timespec time_req, time_rem;  


  time_req.tv_sec = 0;
  time_req.tv_nsec = 1000;

  while (rrh_exit==0) 
  {
    //wait until some data has been copied
    for (antenna_index=0;antenna_index<4;antenna_index++){
      if (sync_eNB_rx[antenna_index]==0)
      {	
        if (!eNB_tx_started) 
        {
          eNB_tx_started=1; // set this flag to 1 to indicate that eNB started
          if (RT_FLAG==1) 
          {
            last_hw_counter=hw_counter;
          } 
        }
        else
        {
          if (RT_FLAG==1) {
            if (hw_counter > last_hw_counter+1)
            {
              printf("L");
        //      goto end_copy_eNB;
            }
            else
            {
              while (hw_counter != last_hw_counter+1)
                nanosleep(&time_req,&time_rem); 
            }
          }
        }
        truncated_timestamp = timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE);
        truncated_timestamp_final = (timestamp_eNB_tx[antenna_index]+nsamps_eNB[antenna_index])%FRAME_MAX_SIZE;

        if ((truncated_timestamp + nsamps_eNB[antenna_index]) > FRAME_MAX_SIZE)
        {
          if ((timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE < nsamps_eNB[antenna_index]) && (UE_rx_started==1))
          {
            overflow_rx_buffer_UE[antenna_index]++;
            printf("UE Overflow[%d] : %d, timestamp : %d\n",antenna_index,overflow_rx_buffer_UE[antenna_index],(int)truncated_timestamp);
            if (NRT_FLAG==1)
            {
              while ((timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE) < nsamps_eNB[antenna_index])
                nanosleep(&time_req,&time_rem);
            }
          }
          memcpy(&rx_buffer_UE[antenna_index][truncated_timestamp + (sizeof(openair0_timestamp)>>2)],&tx_buffer_eNB[antenna_index][truncated_timestamp],(FRAME_MAX_SIZE<<2)-(truncated_timestamp<<2));
          memcpy(&rx_buffer_UE[antenna_index][(sizeof(openair0_timestamp)>>2)],&tx_buffer_eNB[antenna_index][0],(nsamps_eNB[antenna_index]<<2)-(FRAME_MAX_SIZE<<2)+(truncated_timestamp<<2));
        }
        else
        {
          if (((truncated_timestamp < (timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE)) && (truncated_timestamp_final >  (timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE))) && (UE_rx_started==1))
          {
            overflow_rx_buffer_UE[antenna_index]++;
            printf("UE Overflow[%d] : %d, timestamp : %d\n",antenna_index,overflow_rx_buffer_UE[antenna_index],(int)truncated_timestamp);
            if (NRT_FLAG==1)
            {
              while (truncated_timestamp_final >  timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE)
                 nanosleep(&time_req,&time_rem);
            }
          }
          memcpy(&rx_buffer_UE[antenna_index][truncated_timestamp + (sizeof(openair0_timestamp)>>2)],&tx_buffer_eNB[antenna_index][truncated_timestamp],(nsamps_eNB[antenna_index]<<2));
        }
     
      
// end_copy_eNB : 
      last_hw_counter=hw_counter;
      pthread_mutex_lock(&sync_eNB_mutex[antenna_index]);
      sync_eNB_rx[antenna_index]--;
      pthread_mutex_unlock(&sync_eNB_mutex[antenna_index]);
      
      }
     }
  
  }
  return(0);
}

void *rrh_proc_UE_thread() {

  //rrh_desc_t *rrh_desc = (rrh_desc_t *)arg;
  int antenna_index;
  openair0_timestamp truncated_timestamp, truncated_timestamp_final, last_hw_counter=0;
  struct timespec time_req, time_rem;


  time_req.tv_sec = 0;
  time_req.tv_nsec = 1000;

 
  while (rrh_exit==0) 
  {
    //wait until some data has been copied
    for (antenna_index=0;antenna_index<4;antenna_index++){
      if (sync_UE_rx[antenna_index]==0)
      {
        if (!UE_tx_started) 
        {
          UE_tx_started=1;  //Set this flag to 1 to indicate that a UE started retrieving data
          if (RT_FLAG==1)
          {
            last_hw_counter=hw_counter;
          }
        }
        else
        {
          if (RT_FLAG==1) {
            if (hw_counter > last_hw_counter+1)
            {
              printf("L1");
//              goto end_copy_UE;
            }
            else
            {
              while (hw_counter != last_hw_counter+1)
                nanosleep(&time_req,&time_rem);
            }
          }
        }

      truncated_timestamp = timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE);	
      truncated_timestamp_final =  (timestamp_UE_tx[antenna_index]+nsamps_UE[antenna_index])%FRAME_MAX_SIZE;

        if ((truncated_timestamp + nsamps_UE[antenna_index]) > FRAME_MAX_SIZE)
        {
          if ((timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE < nsamps_UE[antenna_index]) && (eNB_rx_started==1))
          {
            overflow_rx_buffer_eNB[antenna_index]++;
            printf("eNB Overflow[%d] : %d, timestamp : %d\n",antenna_index,overflow_rx_buffer_eNB[antenna_index],(int)truncated_timestamp);
            if (NRT_FLAG==1)
            {
              while ((timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE) < nsamps_UE[antenna_index])
                nanosleep(&time_req,&time_rem);
            }
          }
          memcpy(&rx_buffer_eNB[antenna_index][truncated_timestamp + (sizeof(openair0_timestamp)>>2)],&tx_buffer_UE[antenna_index][truncated_timestamp],(FRAME_MAX_SIZE<<2)-(truncated_timestamp<<2));
          memcpy(&rx_buffer_eNB[antenna_index][(sizeof(openair0_timestamp)>>2)],&tx_buffer_UE[antenna_index][0],(nsamps_UE[antenna_index]<<2)-(FRAME_MAX_SIZE<<2)+(truncated_timestamp<<2));
        }
        else
        {
          if (((truncated_timestamp < (timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE)) && (truncated_timestamp_final >  (timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE))) && (eNB_rx_started==1))
          {
            overflow_rx_buffer_eNB[antenna_index]++;
            printf("eNB Overflow[%d] : %d, timestamp : %d\n",antenna_index,overflow_rx_buffer_eNB[antenna_index],(int)truncated_timestamp);
            if (NRT_FLAG==1)
            {
              while (truncated_timestamp_final >  timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE)
                 nanosleep(&time_req,&time_rem);
            }
          }
          memcpy(&rx_buffer_eNB[antenna_index][truncated_timestamp+ (sizeof(openair0_timestamp)>>2)],&tx_buffer_UE[antenna_index][truncated_timestamp],(nsamps_UE[antenna_index]<<2));
        }
      
 //end_copy_UE :
      last_hw_counter=hw_counter;
      pthread_mutex_lock(&sync_UE_mutex[antenna_index]);
      sync_UE_rx[antenna_index]--;
      pthread_mutex_unlock(&sync_UE_mutex[antenna_index]);
      
      }
     }
  
  }
  return(0);
}

void *rrh_UE_thread(void *arg) {

  int sockid=-1;
  struct sockaddr_in serveraddr;
  struct sockaddr clientaddr;
  socklen_t clientaddrlen;
  rrh_desc_t *rrh_desc = (rrh_desc_t *)arg;
  char str[INET_ADDRSTRLEN];
  //int8_t msg_header[4+sizeof(openair0_timestamp)];
  int8_t buf[BUF_LEN];
  int16_t cmd,nsamps,antenna_index;
  ssize_t bytes_received;
  struct timespec time_rem;
  ssize_t bytes_sent;
  openair0_timestamp temp;
  openair0_timestamp last_hw_counter=0;
  struct timespec time_req_1us, time_rem_1us;


  time_req_1us.tv_sec = 0;
  time_req_1us.tv_nsec = 1000;

  while (rrh_exit==0) {

    
    sockid=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sockid==-1) {
      perror("Cannot create UE socket: ");
      rrh_exit=1;
    }


    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(rrh_desc->UE_port);
    inet_pton(AF_INET,rrh_desc->UE_dest_ip,&serveraddr.sin_addr.s_addr);

    inet_ntop(AF_INET, &(serveraddr.sin_addr), str, INET_ADDRSTRLEN);      
    printf("Binding to UE socket for %s:%d\n",str,ntohs(serveraddr.sin_port));

    if (bind(sockid,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0) {
      perror("Cannot bind to UE socket: ");
      rrh_exit = 1;
    }
     
    // now wait for commands from eNB
    while (rrh_exit==0) {

      // get header info
      bzero((void*)&clientaddr,sizeof(struct sockaddr));
      clientaddrlen = sizeof(struct sockaddr);
      //printf("Waiting for UE ...\n");
      bytes_received = recvfrom(sockid,buf,BUF_LEN,0,&clientaddr,&clientaddrlen);
      cmd = buf[0];
      antenna_index = cmd>>1;
      timestamp_UE_tx[antenna_index] = *(openair0_timestamp*)(buf+4);
      nsamps = *(int16_t *)(buf+2);
      cmd = cmd&1;
      inet_ntop(AF_INET, &(((struct sockaddr_in*)&clientaddr)->sin_addr), str, INET_ADDRSTRLEN);

      switch (cmd) {
      case 0: // RX
	printf("Received UE RX request for antenna %d, nsamps %d (from %s:%d)\n",antenna_index,nsamps,str, 
		       ntohs(((struct sockaddr_in*)&clientaddr)->sin_port));

        if (!UE_rx_started)
        {
          UE_rx_started=1;  //Set this flag to 1 to indicate that a UE started retrieving data
          if (RT_FLAG==1)
          {
            last_hw_counter=hw_counter;
          }
        }
        else
        {
          if (RT_FLAG==1) {
            if (hw_counter > last_hw_counter+1)
            {
              printf("L1");
//              goto end_copy_UE;
            }
            else
            {
              while (hw_counter != last_hw_counter+1)
                nanosleep(&time_req_1us,&time_rem_1us);
            }
          }
        }

	// send return	
	temp=*(openair0_timestamp*)&rx_buffer_UE[antenna_index][timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)];
        *(openair0_timestamp*)&rx_buffer_UE[antenna_index][timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)]=timestamp_UE_rx[antenna_index];
	
	if ((timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)+nsamps) > FRAME_MAX_SIZE)   // Wrap around if nsamps exceeds the buffer limit
	{
          if (((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE)) < ((timestamp_UE_rx[antenna_index]+nsamps)%(FRAME_MAX_SIZE))) && (eNB_tx_started==1))
          {
            printf("UE underflow wraparound timestamp_UE_rx : %d, timestamp_eNB_tx : %d\n",(int)(timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)),(int)(timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE));
            if (NRT_FLAG==1)
            {
              while ((timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE) < ((timestamp_UE_rx[antenna_index]+nsamps)%(FRAME_MAX_SIZE)))
                nanosleep(&time_req_1us,&time_rem_1us);
            }
          }

	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_UE[antenna_index][timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)],
				((FRAME_MAX_SIZE)<<2) - ((timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE))<<2) + sizeof(openair0_timestamp), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH UE : sendto for RX");
	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_UE[antenna_index][0],
				(nsamps<<2) - ((FRAME_MAX_SIZE)<<2) + ((timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE))<<2), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH UE : sendto for RX");
	}
	else{
          if (((timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE)< timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE) && (((timestamp_UE_rx[antenna_index]+nsamps)%FRAME_MAX_SIZE) > (timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE)) && (eNB_tx_started==1) )
          {
            printf("UE underflow timestamp_UE_rx : %d, timestamp_eNB_tx : %d\n",(int)(timestamp_UE_rx[antenna_index]%FRAME_MAX_SIZE),(int)(timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE));
            if (NRT_FLAG==1)
            {
              while (((timestamp_UE_rx[antenna_index]+nsamps)%FRAME_MAX_SIZE) > (timestamp_eNB_tx[antenna_index]%FRAME_MAX_SIZE))
                {nanosleep(&time_req_1us,&time_rem_1us); } 
            }
          }


	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_UE[antenna_index][timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)],
				(nsamps<<2)+sizeof(openair0_timestamp), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH UE thread: sendto for RX");
	}
	printf("bytes_sent %d(timestamp_UE_rx[%d] %d)\n",(int)bytes_sent,antenna_index,(int)timestamp_UE_rx[antenna_index]);
        *(openair0_timestamp*)&rx_buffer_UE[antenna_index][timestamp_UE_rx[antenna_index]%(FRAME_MAX_SIZE)]=temp;
	timestamp_UE_rx[antenna_index]+=nsamps;
        last_hw_counter=hw_counter; 
	break;
      case 1: // TX
        if (NRT_FLAG==1)
        {
          nrt_UE_counter[antenna_index]++;
        }
	printf("Received UE TX request for antenna %d, nsamps %d, timestamp %d bytes_received %d\n",antenna_index,nsamps,(int)timestamp_UE_tx[antenna_index],(int)bytes_received);
        if ((timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE)+nsamps) > FRAME_MAX_SIZE)   // Wrap around if nsamps exceeds the buffer limit
	{
	  memcpy(&tx_buffer_UE[antenna_index][timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE)],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t),(FRAME_MAX_SIZE<<2)-((timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE))<<2));
	  memcpy(&tx_buffer_UE[antenna_index][0],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t)+(FRAME_MAX_SIZE*4)-((timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE))<<2),(nsamps<<2)-((FRAME_MAX_SIZE-(timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE)))<<2));
	  //printf("Received UE TX samples for antenna %d, nsamps %d (%d)\n",antenna_index,nsamps,(int)(bytes_received>>2));
	}
	else {
	  memcpy(&tx_buffer_UE[antenna_index][timestamp_UE_tx[antenna_index]%(FRAME_MAX_SIZE)],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t),(nsamps<<2));
	}
	//printf("Received UE TX samples for antenna %d, nsamps %d (%d)\n",antenna_index,nsamps,(int)(bytes_received>>2));
	
        if (rrh_desc->time_req.tv_sec !=0 || rrh_desc->time_req.tv_nsec !=0)  
        {
          nanosleep(&rrh_desc->time_req,&time_rem); 
        }
        while (sync_UE_rx[antenna_index]==0)
          nanosleep(&time_req_1us,&time_rem_1us);
	
        pthread_mutex_lock(&sync_UE_mutex[antenna_index]);
	sync_UE_rx[antenna_index]++;
	if (!sync_UE_rx[antenna_index])
	{
	  counter_UE_tx[antenna_index]=(counter_UE_tx[antenna_index]+nsamps)%FRAME_MAX_SIZE;
	  nsamps_UE[antenna_index]=nsamps;
	}
	else
	{
	  printf("rrh_eNB_proc thread is busy, will exit\n");
	  exit(-1);
	}
        pthread_mutex_unlock(&sync_UE_mutex[antenna_index]);


	break;
      }
      //      rrh_exit = 1;
    }
  }

  close(sockid);
  
  rrh_UE_thread_status = 0;
  pthread_exit(&rrh_UE_thread_status);

  return(0);
}




void *rrh_eNB_thread(void *arg) {

  int sockid=-1;
  struct sockaddr_in serveraddr;
  struct sockaddr clientaddr;
  socklen_t clientaddrlen;
  rrh_desc_t *rrh_desc = (rrh_desc_t *)arg;
  char str[INET_ADDRSTRLEN];
  //int8_t msg_header[4+sizeof(openair0_timestamp)];
  int8_t buf[BUF_LEN];
  int16_t cmd,nsamps,antenna_index;
  ssize_t bytes_received;
  ssize_t bytes_sent;
  openair0_timestamp temp, last_hw_counter=0;
  struct timespec time_rem;
  struct timespec time_req_1us, time_rem_1us;


  time_req_1us.tv_sec = 0;
  time_req_1us.tv_nsec = 1000;


  while (rrh_exit==0) {

    
    sockid=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sockid==-1) {
      perror("Cannot create eNB socket: ");
      rrh_exit=1;
    }


    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(rrh_desc->eNB_port);
    inet_pton(AF_INET,rrh_desc->eNB_dest_ip,&serveraddr.sin_addr.s_addr);

    inet_ntop(AF_INET, &(serveraddr.sin_addr), str, INET_ADDRSTRLEN);      
    printf("Binding to eNB socket for %s:%d\n",str,ntohs(serveraddr.sin_port));

    if (bind(sockid,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0) {
      perror("Cannot bind to eNB socket: ");
      rrh_exit = 1;
    }
     
    // now wait for commands from eNB
    while (rrh_exit==0) {

      // get header info
      bzero((void*)&clientaddr,sizeof(struct sockaddr));
      clientaddrlen = sizeof(struct sockaddr);
      //printf("Waiting for eNB ...\n");
      bytes_received = recvfrom(sockid,buf,BUF_LEN,0,&clientaddr,&clientaddrlen);
      cmd = buf[0];
      antenna_index = cmd>>1;
      timestamp_eNB_tx[antenna_index] = *(openair0_timestamp*)(buf+4);
      nsamps = *(int16_t *)(buf+2);
      cmd = cmd&1;
      inet_ntop(AF_INET, &(((struct sockaddr_in*)&clientaddr)->sin_addr), str, INET_ADDRSTRLEN);

      switch (cmd) {
      case 0: // RX
	printf("Received eNB RX request for antenna %d, nsamps %d (from %s:%d)\n",antenna_index,nsamps,str, 
		       ntohs(((struct sockaddr_in*)&clientaddr)->sin_port));
	
        if (!eNB_rx_started)
        {
          eNB_rx_started=1; // set this flag to 1 to indicate that eNB started
          if (RT_FLAG==1)
          {
            last_hw_counter=hw_counter;
          }
        }
        else
        {
          if (RT_FLAG==1) {
            if (hw_counter > last_hw_counter+1)
            {
              printf("L");
        //      goto end_copy_eNB;
            }
            else
            {
              while (hw_counter != last_hw_counter+1)
                nanosleep(&time_req_1us,&time_rem_1us);
            }
          }
        }

	// send return
	temp=*(openair0_timestamp*)&rx_buffer_eNB[antenna_index][timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)];
        *(openair0_timestamp*)&rx_buffer_eNB[antenna_index][timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)]=timestamp_eNB_rx[antenna_index];
	
	if ((timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)+nsamps) > FRAME_MAX_SIZE)   // Wrap around if nsamps exceeds the buffer limit
	{
          if ((timestamp_UE_tx[antenna_index]%FRAME_MAX_SIZE < ((timestamp_eNB_rx[antenna_index]+nsamps)%FRAME_MAX_SIZE)) && (UE_tx_started==1))
          {
            printf("eNB underflow\n");
            if (NRT_FLAG==1)
            {
              while ((timestamp_UE_tx[antenna_index]%FRAME_MAX_SIZE) < nsamps)
                nanosleep(&time_req_1us,&time_rem_1us);
            }
          }


	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_eNB[antenna_index][timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)],
				((FRAME_MAX_SIZE)<<2) - ((timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE))<<2) + sizeof(openair0_timestamp), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH eNB : sendto for RX");
	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_eNB[antenna_index][0],
				(nsamps<<2) - ((FRAME_MAX_SIZE)<<2) + ((timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE))<<2), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH eNB : sendto for RX");
	}
	else{
          if (((timestamp_eNB_rx[antenna_index]%FRAME_MAX_SIZE)< timestamp_UE_tx[antenna_index]%FRAME_MAX_SIZE) && (((timestamp_eNB_rx[antenna_index]+nsamps)%FRAME_MAX_SIZE) > (timestamp_UE_tx[antenna_index]%FRAME_MAX_SIZE)) && (UE_tx_started==1))
          {
            printf("eNB underflow\n");
            if (NRT_FLAG==1)
            {
              while (((timestamp_eNB_rx[antenna_index]+nsamps)%FRAME_MAX_SIZE) > (timestamp_UE_tx[antenna_index]%FRAME_MAX_SIZE))
                nanosleep(&time_req_1us,&time_rem_1us);
            }
          }

	  if ((bytes_sent = sendto(sockid, 
				&rx_buffer_eNB[antenna_index][timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)],
				(nsamps<<2)+sizeof(openair0_timestamp), 
				0,
				(struct sockaddr*)&clientaddr, 
				sizeof(struct sockaddr)))<0)
	    perror("RRH eNB : sendto for RX");

	}
	printf("bytes_sent %d(timestamp_eNB_rx[%d] %d)\n",(int)bytes_sent,antenna_index,(int)timestamp_eNB_rx[antenna_index]);
	
	
        *(openair0_timestamp*)&rx_buffer_eNB[antenna_index][timestamp_eNB_rx[antenna_index]%(FRAME_MAX_SIZE)]=temp;
	timestamp_eNB_rx[antenna_index]+=nsamps;
        last_hw_counter=hw_counter;
	break;
      case 1: // TX
        if (NRT_FLAG==1)
        {
          nrt_eNB_counter[antenna_index]++;
        }
	printf("Received eNB TX request for antenna %d, nsamps %d, timestamp %d bytes_received %d\n",antenna_index,nsamps,(int)timestamp_eNB_tx[antenna_index],(int)bytes_received);
	
	if ((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE)+nsamps) > FRAME_MAX_SIZE)   // Wrap around if nsamps exceeds the buffer limit
	{
	memcpy(&tx_buffer_eNB[antenna_index][timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE)],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t),((FRAME_MAX_SIZE)<<2)-((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE))<<2));
	//printf("Done first part size : %d\n",(int32_t)(((FRAME_MAX_SIZE)<<2)-((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE))<<2)));
	memcpy(&tx_buffer_eNB[antenna_index][0],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t) + ((FRAME_MAX_SIZE)<<2) -((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE))<<2),(nsamps<<2)-((FRAME_MAX_SIZE)<<2)+((timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE))<<2));
	//printf("Received eNB TX samples for antenna %d, nsamps %d (%d)\n",antenna_index,nsamps,(int)(bytes_received>>2));
	}
	else {
	  memcpy(&tx_buffer_eNB[antenna_index][timestamp_eNB_tx[antenna_index]%(FRAME_MAX_SIZE)],buf+sizeof(openair0_timestamp)+2*sizeof(int16_t),nsamps<<2);
	}
	
        if (rrh_desc->time_req.tv_sec !=0 || rrh_desc->time_req.tv_nsec !=0)  
        {
          nanosleep(&rrh_desc->time_req,&time_rem); 
        }	
        while (sync_eNB_rx[antenna_index]==0)
          nanosleep(&time_req_1us,&time_rem_1us);
        pthread_mutex_lock(&sync_eNB_mutex[antenna_index]);
	sync_eNB_rx[antenna_index]++;
	if (!sync_eNB_rx[antenna_index])
	{
	  counter_eNB_tx[antenna_index]=(counter_eNB_tx[antenna_index]+nsamps)%FRAME_MAX_SIZE;
	  nsamps_eNB[antenna_index]=nsamps;
	}
	else
	{
	  printf("rrh_eNB_proc thread is busy, will exit\n");
	  exit(-1);
	}
	pthread_mutex_unlock(&sync_eNB_mutex[antenna_index]);
	
	break;
      } //switch(cmd)

    }  //while (rrh_exit==0)
  }  //while (rrh_exit==0)

  close(sockid);
  
  rrh_eNB_thread_status = 0;
  pthread_exit(&rrh_eNB_thread_status);

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
  
  pthread_t main_rrh_eNB_thread, main_rrh_UE_thread, main_rrh_proc_eNB_thread, main_rrh_proc_UE_thread, main_timer_proc_thread;
  pthread_attr_t attr, attr_proc, attr_timer;
  struct sched_param sched_param_rrh, sched_param_rrh_proc, sched_param_timer;
  int error_code_eNB, error_code_UE, error_code_proc_eNB, error_code_proc_UE, error_code_timer;
  int i;
  int opt;
  int nsecs=0, rt_period=0;

  nsecs = 0;

  while ((opt = getopt(argc, argv, "d:t:r")) != -1) {
    switch (opt) {
      case 'd':
        nsecs = atoi(optarg);
        NRT_FLAG=1;
      break;
      case 't':
        rt_period = atoi(optarg);
        RT_FLAG=1;
        NRT_FLAG=0;
      break;
      case 'r':
        rt_period = DEFAULT_PERIOD_NS;
        RT_FLAG=1;
        NRT_FLAG=0;
      break;

      default: /* '?' */
        fprintf(stderr, "Usage: %s [-d nsecs]\n", argv[0]);
        exit(-1);
      }
  }

 //          if (optind >= argc) {
 //              fprintf(stderr, "Expected argument after options\n");
 //              exit(EXIT_FAILURE);
 //          }

  struct itimerspec timer;
    // setup the timer (1s delay, 1s reload)
  timer.it_value.tv_sec = rt_period/1000000000;
  timer.it_value.tv_nsec = rt_period%1000000000;
  timer.it_interval.tv_sec = rt_period/1000000000;
  timer.it_interval.tv_nsec = rt_period%1000000000;


  rrh_desc_t rrh;
  rrh.time_req.tv_sec = nsecs/1000000000;
  rrh.time_req.tv_nsec = nsecs%1000000000;
  
  rrh.eNB_port = RRH_eNB_PORT;
  rrh.eNB_dest_ip = RRH_eNB_DEST_IP;
  rrh.UE_port = RRH_UE_PORT;
  rrh.UE_dest_ip = RRH_UE_DEST_IP;

  // to make a graceful exit when ctrl-c is pressed
  signal(SIGSEGV, signal_handler);
  signal(SIGINT, signal_handler);


  pthread_attr_init(&attr);
  sched_param_rrh.sched_priority = sched_get_priority_max(SCHED_FIFO);
  pthread_attr_init(&attr_proc);
  sched_param_rrh_proc.sched_priority = sched_get_priority_max(SCHED_FIFO-1);
  pthread_attr_init(&attr_timer);
  sched_param_timer.sched_priority = sched_get_priority_max(SCHED_FIFO-2);

  pthread_attr_setschedparam(&attr,&sched_param_rrh);
  pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
  pthread_attr_setschedparam(&attr_proc,&sched_param_rrh_proc);
  pthread_attr_setschedpolicy(&attr_proc,SCHED_FIFO-1);
  pthread_attr_setschedparam(&attr_timer,&sched_param_timer);
  pthread_attr_setschedpolicy(&attr_timer,SCHED_FIFO-2);

  for (i=0;i<4;i++){
  pthread_mutex_init(&sync_eNB_mutex[i],NULL);
  pthread_cond_init(&sync_eNB_cond[i],NULL);
  pthread_mutex_init(&sync_UE_mutex[i],NULL);
  pthread_cond_init(&sync_UE_cond[i],NULL);
  }
  pthread_mutex_init(&timer_mutex,NULL);  

  error_code_eNB = pthread_create(&main_rrh_eNB_thread, &attr, rrh_eNB_thread, (void *)&rrh);
  error_code_UE = pthread_create(&main_rrh_UE_thread, &attr, rrh_UE_thread, (void *)&rrh);
  error_code_proc_UE = pthread_create(&main_rrh_proc_UE_thread, &attr_proc, rrh_proc_UE_thread,NULL);
  error_code_proc_eNB = pthread_create(&main_rrh_proc_eNB_thread, &attr_proc, rrh_proc_eNB_thread, NULL);
  error_code_timer = pthread_create(&main_timer_proc_thread, &attr_timer, timer_proc, (void *)&timer);

  if (error_code_eNB) {printf("Error while creating eNB thread\n"); exit(-1);}
  if (error_code_UE) {printf("Error while creating UE thread\n"); exit(-1);}
  if (error_code_proc_UE) {printf("Error while creating UE proc thread\n"); exit(-1);}
  if (error_code_proc_eNB) {printf("Error while creating eNB proc thread\n"); exit(-1);}
  if (error_code_timer) {printf("Error while creating timer proc thread\n"); exit(-1);}

  printf("TYPE <CTRL-C> TO TERMINATE\n");

  while (rrh_exit==0)
    sleep(1);


  //pthread_join(main_rrh_eNB_thread,&rrh_eNB_thread_status);
return 0;
}
