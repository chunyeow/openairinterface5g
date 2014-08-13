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
#include <gps.h>
#include <forms.h>

struct gps_data_t *gps_data = NULL;
struct gps_fix_t dummy_gps_data;
FILE *dumpfile_id = NULL; 


void gps_data_callback(int gps_fd, void* data)
{
  //char tmptxt[1024];
  time_t timer;
  struct tm *now;

  timer = time(NULL);

  //printf("GPS timer called\n");
  if (gps_data)
    {
      if (gps_poll(gps_data) != 0)
	{
	  //sprintf(tmptxt,"Error polling data from GPS, gps_data = %x", gps_data);
	  printf("Error polling data from GPS\n");
	}
      else
	now = localtime(&timer);
	printf("%04d%02d%02d%02d%02d%02d,%g,%g\n",
	       now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour, now->tm_min, now->tm_sec,
	       //asctime(localtime(&timer)),
	       gps_data->fix.latitude,gps_data->fix.longitude);
    }
  //fl_set_timer(ob, 0.05);
  
  //write GPS
  if (gps_data)
    {
      if (fwrite(&(gps_data->fix), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
	{
	  printf("Error writing to dumpfile, stopping recording\n");
	}
    }
  else
    {
      printf("WARNING: No GPS data available, storing dummy packet\n");
      if (fwrite(&(dummy_gps_data), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
	{
	  printf("Error writing to dumpfile, stopping recording\n");
	}
    } 
}

void stop_gps(int sig) {

  // stop gps
  if (gps_data) 
    fl_remove_io_callback(gps_data->gps_fd, FL_READ , &gps_data_callback);

  // close the GPS 
  if (gps_data) 
    gps_close(gps_data);

  fclose(dumpfile_id);
  dumpfile_id = NULL;

  fl_finish();

  exit(0);
}

int main(int argc, char *argv[]) {

  time_t timer;
  char log_filename[256], date_str[128];
  struct tm *now;

  timer = time(NULL);
  now = localtime(&timer);

  // open GPS
  gps_data = gps_open("127.0.0.1","2947");
  if (gps_data == NULL) 
    {
      printf("Could not open GPS\n");
      exit(-1);
    }
  else if (gps_stream(gps_data, WATCH_ENABLE,NULL) != 0)
    {
      //sprintf(tmptxt,"Error sending command to GPS, gps_data = %x", gps_data);
      printf("Error sending command to GPS\n");
      exit(-1);
    }

  strftime(date_str, 128, "%Y%m%d_%H%M%S", now); 
  sprintf(log_filename,"gps_trace_%s.log", date_str);
  dumpfile_id = fopen(log_filename,"w");
  if (dumpfile_id == NULL)
    {
      printf("Error opening dumpfile\n");
      exit(-1);
    }

  signal(SIGINT, stop_gps); 

  fl_initialize(&argc, argv, "GPS Tracer", 0, 0);

  if (gps_data)  
    fl_add_io_callback(gps_data->gps_fd, FL_READ, &gps_data_callback, NULL);

  fl_do_forms();

  exit(0);
}

