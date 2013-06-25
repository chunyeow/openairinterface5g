#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#ifdef CBMIMO1
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#endif
#ifdef PLATON
#include "daq.h"
#endif

#include "PHY/CONFIG/vars.h"
#include "MAC_INTERFACE/vars.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
//#include <sys/time.h>
#include <sys/types.h>
//#include <sys/wait.h>
//#include <ctype.h>
#include <fcntl.h>
//#include <errno.h>
#include <unistd.h>
//#include <string.h>
//#include <netdb.h>


void main(int argc, char** argv) {

	int i;
	FILE *rxsig_fd;
	int openair_fd,rx_sig_fifo_fd,rf_cntl_fifo_fd,sched_cnt;
	unsigned int params;
	int result;
	/* ############ variables declaration for sensing #######*/
	s16 *samples;
	int size_signal = 20480;
	struct complex16 signal[size_signal];


	printf("Allocating memory for PHY_VARS\n");

	PHY_vars = (PHY_VARS *)malloc(sizeof(PHY_VARS));
	PHY_config = (PHY_CONFIG *)malloc(sizeof(PHY_CONFIG));
	mac_xface  = (MAC_xface *)malloc(sizeof(MAC_xface));
	  
	if((config = fopen("./openair_config.cfg","r")) == NULL) // this can be configured
		{
		printf("[Main USER] The widens configuration file <widens_config.cfg> could not be found!");
		exit(0);
		}		
  
	if ((scenario= fopen("./openair_scenario.scn","r")) ==NULL)
		{
		printf("[Main USER] The widens scenario file <widens_scenario.scn> could not be found!");
		exit(0);
		}
  
	printf("Opened configuration files\n");
	printf("[openair][INFO][START] Opening /dev/openair0\n");
	if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0) 
	{
		fprintf(stderr,"Error %d opening /dev/openair0\n",openair_fd);
		exit(-1);
	}
  
	rx_sig_fifo_fd=open("/dev/rtf59",O_RDONLY);
	if (rx_sig_fifo_fd==-1)
	{
		printf("Can not open rxsig FIFO (/dev/rtf59)!\n");
		exit(0);
	}

	rf_cntl_fifo_fd=open("/dev/rtf60",O_RDONLY);
	if (rf_cntl_fifo_fd==-1)
	{
		printf("Can not open rf_cntl FIFO (/dev/rtf60)!\n");
		exit(0);
	}

	if ((rxsig_fd = fopen("rxsig0.dat","w")) == NULL) {
		printf("Cannot open rxsig0 output file\n");
		exit(0);
	}

	reconfigure_MACPHY(scenario);
	mac_xface->is_cluster_head = 0;

	phy_init(NB_ANTENNAS_TX);
    	printf("Initialized PHY variables\n");

	//mac_init();
	
	/*
	PHY_config->PHY_framing.fc_khz = 1907600;

	result=ioctl(openair_fd, openair_DUMP_CONFIG,(char *)PHY_config);
	if (result == 0) {
	  printf ("[openair][CONFIG][INFO] loading openair configuration in kernel space\n");
	} else {
	  printf ("[openair][START][INFO] loading openair configuration in kernel space failed \n");
	}
	*/

	params = 1|(3<<1)|(3<<3);
	result=ioctl(openair_fd,openair_GET_BUFFER,(void *)&params);
	if (result == 0) {
	  printf ("[openair][CONFIG][INFO] getting rx buffer\n");
	} else {
	  printf ("[openair][START][INFO] getting rx buffer failed \n");
	}

	read(rf_cntl_fifo_fd,(void *)&sched_cnt,4);

	printf("Sched count %d\n",sched_cnt);

	for (i=0;i<NB_ANTENNAS;i++) 
	{
		read(rx_sig_fifo_fd,
			PHY_vars->rx_vars[i].RX_DMA_BUFFER,
			FRAME_LENGTH_BYTES);
	}

	printf("Got Frame for %d antennas, demodulating\n",NB_ANTENNAS);

	fwrite(PHY_vars->rx_vars[0].RX_DMA_BUFFER,1,FRAME_LENGTH_BYTES,rxsig_fd);
	
	/*############### sensing code################*/
	samples = (s16 *)PHY_vars->rx_vars[0].RX_DMA_BUFFER; 
	
	for (i=0;i<size_signal;i++)
	{	signal[i].r = samples[2*i];
		signal[i].i = samples[2*i+1];
	}
	
 	printf("sample 1 = %d + i%d\n",signal[0].r,signal[0].i); 
 	printf("sample 2 = %d + i%d\n",signal[1].r,signal[1].i);
 	printf("sample 3 = %d + i%d\n",signal[2].r,signal[2].i);
 	printf("sample 4 = %d + i%d\n",signal[3].r,signal[3].i);
 	printf("sample 5 = %d + i%d\n",signal[4].r,signal[4].i);
 	printf("sample 6 = %d + i%d\n",signal[5].r,signal[5].i);
	
	printf("FRAME_LENGTH_BYTES = %d\n",FRAME_LENGTH_BYTES);
	printf("size_signal = %d\n",size_signal);
	
 	printf("RX_DMA_BUFFER 1 = %d\n",samples[0]); 
 	printf("RX_DMA_BUFFER 2 = %d\n",samples[1]);
 	printf("RX_DMA_BUFFER 3 = %d\n",samples[2]);
 	printf("RX_DMA_BUFFER 4 = %d\n",samples[3]);
 	printf("RX_DMA_BUFFER 5 = %d\n",samples[4]);
 	printf("RX_DMA_BUFFER 6 = %d\n",samples[5]);

	
	fclose(config);
	fclose(rxsig_fd);
	
	close(openair_fd);
	close(rx_sig_fifo_fd);
	close(rf_cntl_fifo_fd);

	free(PHY_vars);
	free(PHY_config);

	printf("Exiting00000\n");

}
