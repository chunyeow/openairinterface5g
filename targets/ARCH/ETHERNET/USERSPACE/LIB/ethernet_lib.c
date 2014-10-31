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

 /** ethernet_lib : API to stream I/Q samples over standard ethernet
 * 
 *  Authors: Pedro Dinis    <pedrodinis20@gmail.com>
 *           Lucio Ferreira <lucio.ferreira@inov.pt>
 *           Raymond Knopp  <raymond.knopp@eurecom.fr>
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
#include <errno.h>

#include "common_lib.h"

#define DEFAULT_IF	"eth0"
#define BUF_SIZ		8960 /*Jumbo frame size*/
#define MAX_INST        4

int sockfd[MAX_INST];
struct sockaddr_in dest_addr[MAX_INST];
int dest_addr_len[MAX_INST];

int i;
int tx_len = 0;
char sendbuf[MAX_INST][BUF_SIZ]; /*TODO*/
 

/**PDF: Initialization of UDP Socket to communicate with one DEST */
int ethernet_socket_init(int Mod_id, char *dest_ip,int dest_port)
{  
  
  /**PDF: To be passed by input argument*/
  //  DEST_port = 32000;
  struct sockaddr_in *dest = &dest_addr[Mod_id];
  char str[INET_ADDRSTRLEN];

  /* Open RAW socket to send on */

  if ((sockfd[Mod_id] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("ETHERNET: Error opening socket");
    exit(0);
  }

  bzero((void *)dest,sizeof(struct sockaddr));
  dest->sin_family = AF_INET;
  dest->sin_addr.s_addr=inet_addr(dest_ip);
  dest->sin_port=htons(dest_port);
  dest_addr_len[Mod_id] = sizeof(struct sockaddr);

  inet_ntop(AF_INET, &(dest->sin_addr), str, INET_ADDRSTRLEN);      
  printf("Connecting to %s:%d\n",str,ntohs(dest->sin_port));


}
  


int ethernet_write_data(int Mod_id, openair0_timestamp timestamp, const void *buff, int antenna_id, int nsamps) {

  void *buff2 = (void*)buff-sizeof(openair0_timestamp)-(sizeof(int16_t)*2);
  int32_t temp0 = *(int32_t *)buff2;
  openair0_timestamp temp1 = *(openair0_timestamp *)(buff2+(sizeof(int16_t)*2));

  int n_written;
  
  n_written = 0;
  ((int16_t *)buff2)[0] = 1+(antenna_id<<1);
  ((int16_t *)buff2)[1] = nsamps;
  *((openair0_timestamp *)(buff2+(sizeof(int16_t)*2))) = timestamp;

  while(n_written < nsamps) {
    /* Send packet */
    if ((n_written += sendto(sockfd[Mod_id], 
			     buff2,
			     (nsamps<<2)+sizeof(openair0_timestamp)+(2*sizeof(int16_t)), 
			     0, 
			     (struct sockaddr*)&dest_addr[Mod_id], 
			     dest_addr_len[Mod_id])) < 0) {
      printf("Send failed for Mod_id %d\n",Mod_id);
      exit(-1);
    }    
  }
  *(int32_t *)buff2 = temp0;
  *(openair0_timestamp *)(buff2+2*sizeof(int16_t)) = temp1;
  return n_written;
  
}


int ethernet_read_data(int Mod_id,openair0_timestamp *timestamp,void *buff, int antenna_id, int nsamps) {

  void *buff2 = buff-sizeof(openair0_timestamp);
  int samples_received;
  int block_cnt;
  int ret;
  openair0_timestamp temp = *(openair0_timestamp*)buff2;
  int16_t mesg[2];
  char str[INET_ADDRSTRLEN];

  mesg[0] = 0+(antenna_id<<1);
  mesg[1] = nsamps;

  inet_ntop(AF_INET, &(dest_addr[Mod_id].sin_addr), str, INET_ADDRSTRLEN);
  // send command RX for nsamps samples
  //  printf("requesting %d samples from (%s:%d)\n",nsamps,str,ntohs(dest_addr[Mod_id].sin_port));

  sendto(sockfd[Mod_id],mesg,4,0,(struct sockaddr *)&dest_addr[Mod_id],dest_addr_len[Mod_id]);

  samples_received=-sizeof(openair0_timestamp);
  block_cnt=0;
  while(samples_received < (int)((nsamps<<2))) {
    ret=recvfrom(sockfd[Mod_id],
		 &buff2[samples_received],
		 (nsamps<<2)+sizeof(openair0_timestamp)-samples_received,
		 0,//MSG_DONTWAIT,
		 (struct sockaddr *)&dest_addr[Mod_id],
		 &dest_addr_len[Mod_id]);
    printf("samples_received %d (ret %d)\n",samples_received+ret,ret);
    if (ret==-1) {
      if (errno == EAGAIN) {
	perror("ETHERNET: ");
	return((nsamps<<2)+sizeof(openair0_timestamp));
      }
      else if (errno == EWOULDBLOCK) {
	block_cnt++;
	usleep(10);
	if (block_cnt == 100) return(-1);
      }
    }
    else {
      samples_received+=ret;
    }
  }


  *timestamp =  *(openair0_timestamp *)(buff-sizeof(openair0_timestamp));
  printf("Received %d samples, timestamp = %d\n",samples_received>>2,*timestamp);
  *(openair0_timestamp *)(buff-sizeof(openair0_timestamp)) = temp;
  return samples_received>>2;
  
}


int trx_eth_start(openair0_device *openair0) {

  return(ethernet_socket_init(openair0->openair0_cfg.Mod_id, openair0->openair0_cfg.rrh_ip,openair0->openair0_cfg.rrh_port));

}

void trx_eth_write(openair0_device *device, openair0_timestamp timestamp, const void *buff, int nsamps, int cc, int flags)
{
  int i;
  for (i=0;i<cc;i++)
    ethernet_write_data(device->Mod_id,timestamp,buff,i,nsamps);
}
int trx_eth_read(openair0_device *device, openair0_timestamp *ptimestamp, void *buff, int nsamps,int cc) {

  int i;
  for (i=0;i<cc;i++)
    return(ethernet_read_data(device->Mod_id,ptimestamp,buff,0,nsamps));

}

void trx_eth_end(openair0_device *device) {




}

int num_devices = 0;

int openair0_device_init(openair0_device *device, openair0_config_t *openair0_cfg)
{

  printf("ETHERNET: Initializing openair0_device\n");
  device->Mod_id         = num_devices++;
  device->trx_start_func = trx_eth_start;
  device->trx_end_func   = trx_eth_end;
  device->trx_read_func  = trx_eth_read;
  device->trx_write_func = trx_eth_write;
  memcpy((void*)&device->openair0_cfg,(void*)openair0_cfg,sizeof(openair0_config_t));
}
