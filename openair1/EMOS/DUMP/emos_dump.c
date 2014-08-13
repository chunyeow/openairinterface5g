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
/*! \file emod_dump.c
* \brief reads data from a fifo and dumps it to disk
* \author F. Kaltenberger, Leonardo Cardoso
* \date 2012
* \version 0.2
* \company Eurecom
* \email: florian.kaltenberger@eurecom.fr
* \note
* \warning
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <gps.h>
#include <forms.h>

#include "SCHED/phy_procedures_emos.h"
#include "emos_dump.h"

int end=0;

static void endme(int dummy)
{
  printf("SIGINT received\n");
  end=1;
}

/*
#define TIMESTAMP_SIZE 8
#define PDU_ERRORS_SIZE 4
#define RX_RSSI_SIZE 2
#define COMPLEX16_SIZE 4
#define NO_OF_OFDM_CARRIERS 256
#define CHANNEL_BUFFER_SIZE (NB_ANTENNAS*COMPLEX16_SIZE*NO_OF_OFDM_CARRIERS+TIMESTAMP_SIZE+PDU_ERRORS_SIZE+RX_RSSI_SIZE) //in bytes
*/
#define NO_ESTIMATES_DISK 100 //No. of estimates that are aquired before dumped to disk

int main (int argc, char **argv)
{
  char c, eNB_flag=0;
  char *fifo2file_buffer, *fifo2file_ptr;

  int fifo, counter=0, bytes;

  FILE  *dumpfile_id;
  char  dumpfile_name[1024];
  time_t starttime_tmp;
  struct tm starttime;
  
  int channel_buffer_size;
  
  time_t timer;
  struct tm *now;

  struct gps_data_t *gps_data = NULL;
  struct gps_fix_t dummy_gps_data;
 
  while ((c = getopt (argc, argv, "he")) != -1) {
    switch (c) {
    case 'e':
      eNB_flag=1;
      break;
    case 'h':
      printf("EMOS FIFO to file dump.\n");
      printf("Usage: %s -h(elp) -e(NB) (default=ue)\n",argv[0]);
      exit(0);
    default:
      break;
    }
  }

  timer = time(NULL);
  now = localtime(&timer);

  memset(&dummy_gps_data,1,sizeof(struct gps_fix_t));
  
  gps_data = gps_open("127.0.0.1","2947");
  if (gps_data == NULL) 
    {
      printf("Could not open GPS\n");
      //exit(-1);
    }
#if GPSD_API_MAJOR_VERSION>=4
  else if (gps_stream(gps_data, WATCH_ENABLE,NULL) != 0)
#else
  else if (gps_query(gps_data, "w+x") != 0)
#endif
    {
      //sprintf(tmptxt,"Error sending command to GPS, gps_data = %x", gps_data);
      printf("Error sending command to GPS\n");
      //exit(-1);
    }
  
  if (eNB_flag==1)
    channel_buffer_size = sizeof(fifo_dump_emos_eNB);
  else
    channel_buffer_size = sizeof(fifo_dump_emos_UE);

  // allocate memory for NO_FRAMES_DISK channes estimations
  fifo2file_buffer = malloc(NO_ESTIMATES_DISK*channel_buffer_size);
  fifo2file_ptr = fifo2file_buffer;

  if (fifo2file_buffer == NULL)
    {
      printf("Cound not allocate memory for fifo2file_buffer\n");
      exit(EXIT_FAILURE);
    }

  if ((fifo = open(CHANSOUNDER_FIFO_DEV, O_RDONLY)) < 0)
    {
      fprintf(stderr, "Error opening the fifo\n");
      exit(EXIT_FAILURE);
    }


  time(&starttime_tmp);
  localtime_r(&starttime_tmp,&starttime);
  snprintf(dumpfile_name,1024,"%s_data_%d%02d%02d_%02d%02d%02d.EMOS",
	   (eNB_flag==1) ? "eNB" : "UE",
	   1900+starttime.tm_year, starttime.tm_mon+1, starttime.tm_mday, starttime.tm_hour, starttime.tm_min, starttime.tm_sec);

  dumpfile_id = fopen(dumpfile_name,"w");
  if (dumpfile_id == NULL)
    {
      fprintf(stderr, "Error opening dumpfile\n");
      exit(EXIT_FAILURE);
    }

  signal(SIGINT, endme);

  printf("starting dump, channel_buffer_size=%d ...\n",channel_buffer_size);
  while (!end)
    {
      bytes = rtf_read_all_at_once(fifo, fifo2file_ptr, channel_buffer_size);
      /*
      if (eNB_flag==1)
	printf("eNB: count %d, frame %d, read: %d bytes from the fifo\n",counter, ((fifo_dump_emos_eNB*)fifo2file_ptr)->frame_tx,bytes);
      else
	printf("UE: count %d, frame %d, read: %d bytes from the fifo\n",counter, ((fifo_dump_emos_UE*)fifo2file_ptr)->frame_rx,bytes);
      */

      fifo2file_ptr += channel_buffer_size;
      counter ++;

      if (counter == NO_ESTIMATES_DISK)
        {
          //reset stuff
          fifo2file_ptr = fifo2file_buffer;
          counter = 0;

          //flush buffer to disk
	  if (eNB_flag==1)
	    printf("eNB: count %d, frame %d, flushing buffer to disk\n",
		   counter, ((fifo_dump_emos_eNB*)fifo2file_ptr)->frame_tx);
	  else
	    printf("UE: count %d, frame %d, flushing buffer to disk\n",
		   counter, ((fifo_dump_emos_UE*)fifo2file_ptr)->frame_rx);


          if (fwrite(fifo2file_buffer, sizeof(char), NO_ESTIMATES_DISK*channel_buffer_size, dumpfile_id) != NO_ESTIMATES_DISK*channel_buffer_size)
            {
              fprintf(stderr, "Error writing to dumpfile\n");
              exit(EXIT_FAILURE);
            }
	  if (gps_data)
	    {
	      if (gps_poll(gps_data) != 0) {
		printf("problem polling data from gps\n");
	      }
	      else {
		printf("lat %g, lon %g\n",gps_data->fix.latitude,gps_data->fix.longitude);
	      }
	      if (fwrite(&(gps_data->fix), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("Error writing to dumpfile, stopping recording\n");
		  exit(EXIT_FAILURE);
		}
	    }
	  else
	    {
	      printf("WARNING: No GPS data available, storing dummy packet\n");
	      if (fwrite(&(dummy_gps_data), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("Error writing to dumpfile, stopping recording\n");
		  exit(EXIT_FAILURE);
		}
	    } 
        }
    }
  
  free(fifo2file_buffer);
  fclose(dumpfile_id);
  close(fifo);
  
  return 0;

}

