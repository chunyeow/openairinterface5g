#include <stdio.h>
#include <stdlib.h>
#include <gps.h>

#include "phy_procedures_emos.h"

fifo_dump_emos *fifo_output = NULL;
char *fifo_buffer = NULL;
char *fifo_ptr = NULL;
char  dumpfile_dir[1024] = "/tmp/";
FILE *dumpfile_id = NULL; 

int open_dumpfile()
{
  char  dumpfile_name[1024];
  char temp_label[1024];
  
  // create the dumpfile buffer
  // allocate memory for NO_FRAMES_DISK channes estimations 
  fifo_buffer = malloc(NO_ESTIMATES_DISK*CHANNEL_BUFFER_SIZE);
  fifo_ptr = fifo_buffer;
  
  if(fifo_buffer == NULL)
    {
      printf( "Could not allocate memory for fifo2file_buffer\n");
      return(-1);
    }
  
  // open the dumpfile
  time_t starttime_tmp;
  struct tm starttime;
  
  time(&starttime_tmp);
  localtime_r(&starttime_tmp,&starttime);
  snprintf(dumpfile_name,1024,"%sdata_term%d_idx%d_%d%0.2d%0.2d_%0.2d%0.2d%0.2d.EMOS",dumpfile_dir,terminal_idx,file_index,1900+starttime.tm_year, starttime.tm_mon+1, starttime.tm_mday, starttime.tm_hour, starttime.tm_min, starttime.tm_sec); 
  
  dumpfile_id = fopen(dumpfile_name,"w");
  if (dumpfile_id == NULL)
    {
      printf("Error opening dumpfile %s\n", dumpfile_name);
      return(-1);
    }
  else 
    {
      printf("Openend dumpfile %s\n", dumpfile_name);
    }

  return 0;
}



int main(int argc, char *argv[])
{

  FILE *infile = NULL;
  FILE *outfile = NULL;
  
  char* infile_name = argv[1];
  char outfile_name[1024];

  fifo_dump_emos frame_data;
  struct gps_fix_t gps_data;

  int frame_read = 0;
  int rec_frame_counter = 0;

  // open the file
  infile = fopen(infile_name,"r");
  if (infile == NULL)
    {
      fprintf(stderr,"opening of file failes\n");
      return -1;
    }
  
  while ~feof(infile) 
    {

      // read frame
      fread(&frame_data, sizeof(char), sizeof(fifo_dump_emos), infile);
      frame_read++;
      
      //read GPS data
      if (frame_read % NO_ESTIMATES_DISK == 0 )
	fread(&gps_data, sizeof(char), sizeof(struct gps_fix_t), infile);

      rec_frame_counter =  frame_data->PHY_measurements.frame_tx[0];

      if (rec_frame_counter % REC_FRAMES_MAX == 0) 
	{

	  // release the buffer
	  free(fifo_buffer);
	  fifo_buffer = NULL;
	  fifo_output = NULL;
	  fifo_ptr = NULL;
	  
	  // close the dumpfile
	  fclose(dumpfile_id);
	  dumpfile_id = NULL;
	
	  // open another dumpfile
	  if (open_dumpfile() != 0)
	    {
	      printf("Error opening dumpfile\n");
	      return(-1);
	    }
	}
      

      if (rec_frame_counter % REC_FRAMES_MAX < REC_FRAMES_PER_FILE)  
	record_multi = REC_ON;
      else 
	record_multi = REC_OFF;

      if (record_multi == REC_ON) {

	// store data to disk
	if (fwrite(&frame_data, sizeof(char), CHANNEL_BUFFER_SIZE, dumpfile_id) != CHANNEL_BUFFER_SIZE)
	  {
	    printf("Error writing to dumpfile, stopping recording\n");
	    return(-1);
	  }
	
	// write GPS data to disk
	if (rec_frame_counter%NO_ESTIMATES_DISK==0)
	  if (gps_data)
	    {
	      if (fwrite(&(gps_data->fix), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("Error writing to dumpfile, stopping recording\n");
		  return(-1);
		}
	    }
	  else
	    {
	      printf("WARNING: No GPS data available, storing dummy packet\n");
	      if (fwrite(&(dummy_gps_data), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
		{
		  printf("Error writing to dumpfile, stopping recording\n");
		  return(-1);
		}
	    }
      }
    }
}

