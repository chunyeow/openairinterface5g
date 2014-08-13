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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {

  FILE *file_id = NULL; 

  struct iw_struct {
    char wday[8];
    char mon[8];
    int day;
    int hour;
    int min;
    int sec;
    int year;
    char addr[32];
    char dev[32];
    int inactive_time;
    int rx_bytes;
    int rx_packets;
    int tx_bytes;
    int tx_packets;
    int tx_retries;
    int tx_failed;
    int signal;
    int signal_ctl0;
    int signal_ctl1;
    float tx_bitrate;
    int tx_mcs;
    int tx_bw;
    char tx_gi1[32];
    char tx_gi2[32];
    float rx_bitrate;
    int rx_mcs;
    int rx_bw;
    char rx_gi1[32];
    char rx_gi2[32];
  } iw_field;

  char line[256];

  int i;
  if (argc < 2) {
    printf("Program requires an argument (filename)\n");
    return(-1);
  }

  file_id = fopen(argv[1],"r");
  if (file_id == NULL) {
    printf("Error opening file %s\n", argv[1]);
    return(-1);
  }

  i=0;
  while (~feof(file_id)) {
    fgets(line, 256, file_id); 
    if (feof(file_id))
      break;
    sscanf(line,"%s %s %d %d:%d:%d GMT %d", 
	   iw_field.wday,
	   iw_field.mon,
	   &iw_field.day,
	   &iw_field.hour,
	   &iw_field.min,
	   &iw_field.sec,
	   &iw_field.year);
    fgets(line, 256, file_id); 
    sscanf(line,"Station %s (on %s", 
	   iw_field.addr,
	   iw_field.dev);
    fgets(line, 256, file_id); 
    sscanf(line," inactive time: %d ms",
	   &iw_field.inactive_time);
    fgets(line, 256, file_id); 
    sscanf(line," rx bytes: %d", 
	   &iw_field.rx_bytes);
    fgets(line, 256, file_id); 
    sscanf(line," rx packets: %d", 
	   &iw_field.rx_packets);
    fgets(line, 256, file_id); 
    sscanf(line," tx bytes: %d", 
	   &iw_field.tx_bytes);
    fgets(line, 256, file_id); 
    sscanf(line," tx packets: %d", 
	   &iw_field.tx_packets);
    fgets(line, 256, file_id); 
    sscanf(line," tx retries: %d", 
	   &iw_field.tx_retries);
    fgets(line, 256, file_id); 
    sscanf(line," tx failed: %d", 
	   &iw_field.tx_failed);
    fgets(line, 256, file_id); 
    sscanf(line," signal: %d dBm", 
	   &iw_field.signal);
    fgets(line, 256, file_id); 
    sscanf(line," signal_ctl0: %d dBm", 
	   &iw_field.signal_ctl0);
    fgets(line, 256, file_id); 
    sscanf(line," signal_ctl1: %d dBm", 
	   &iw_field.signal_ctl1);
    fgets(line, 256, file_id); 
    sscanf(line," tx bitrate: %e MBit/s MCS %d %dMhz %s %s", 
	   &iw_field.tx_bitrate,
	   &iw_field.tx_mcs,
	   &iw_field.tx_bw,
	   iw_field.tx_gi1,
	   iw_field.tx_gi2);
    fgets(line, 256, file_id); 
    sscanf(line," rx bitrate: %e MBit/s MCS %d %dMhz %s %s",	   
	   &iw_field.rx_bitrate,
	   &iw_field.rx_mcs,
	   &iw_field.rx_bw,
	   iw_field.rx_gi1,
	   iw_field.rx_gi2);
    //printf("read set %d, time %d:%d:%d\n",i,iw_field.hour,iw_field.min,iw_field.sec);
    if (i==0)
      printf("num, date; inactive_time; rx_bytes; rx_packets; tx_bytes; tx_packets; tx_retries; tx_failed; signal; signal0; signal1; tx_bitrate; tx_mcs; rx_bitrate; rx_mcs\n");
    printf("%d; %d-%s-%d %d:%d:%d; %d; %d; %d; %d; %d; %d; %d; %d; %d; %d; %e; %d; %e; %d\n",
	   i,
	   iw_field.day,
	   iw_field.mon,
	   iw_field.year,
	   iw_field.hour,
	   iw_field.min,
	   iw_field.sec,
	   iw_field.inactive_time,
	   iw_field.rx_bytes,
	   iw_field.rx_packets,
	   iw_field.tx_bytes,
	   iw_field.tx_packets,
	   iw_field.tx_retries,
	   iw_field.tx_failed,
	   iw_field.signal,
	   iw_field.signal_ctl0,
	   iw_field.signal_ctl1,
	   iw_field.tx_bitrate,
	   iw_field.tx_mcs,
	   iw_field.rx_bitrate,
	   iw_field.rx_mcs);

    i=i+1;
  }

  fclose(file_id);

  return (0);
}
