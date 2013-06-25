#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
//#include <sys/user.h>
//#include <errno.h>
#include <math.h>
#include <signal.h>
#include <strings.h>
#include "forms.h"
#include "sach_scope.h"

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
//#include "PHY/TOOLS/defs.h"


FD_sach_scope *form;


short *channel[4],*channel_f[4],*rx_sig[4],*rx_sig_f2[4],*rx_sig_f3[4];

unsigned char nb_tx_ant;

unsigned int mem_base;

void sach_scope_idle_callback(void) {

  int user_index,tchan_index,i;
  
  short *sacch_data,*sach_data;
  float avg,sach_re[4096],sach_im[4096];
  float sacch_re[512],sacch_im[512];
  unsigned short nb_sacch_carriers,nb_sach_carriers;

  SACH_DIAGNOSTICS *sach_diag;


  for (user_index=0;user_index<8;user_index++) {

    for (tchan_index=0;tchan_index<5;tchan_index++) {

      sach_diag = &PHY_vars->Sach_diagnostics[user_index][tchan_index];
      if (sach_diag->active == 1) {

	avg = 0;
	nb_sacch_carriers = (sach_diag->nb_sacch_carriers< 512) ? sach_diag->nb_sacch_carriers : 512;
	nb_sach_carriers = (sach_diag->nb_sach_carriers < 4096) ? sach_diag->nb_sach_carriers : 4096;


	sacch_data = (short *)(mem_base + (unsigned int)PHY_vars->Sach_diagnostics[user_index][tchan_index].sacch_demod_data-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
	sach_data = (short *)(mem_base + (unsigned int)PHY_vars->Sach_diagnostics[user_index][tchan_index].sach_demod_data-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

	/*	
	printf("User_index %d, tchan_index %d (sacch %d,sach %d) (%p,%p) -> (%p,%p)\n",user_index,tchan_index,nb_sacch_carriers,nb_sach_carriers,PHY_vars->Sach_diagnostics[user_index][tchan_index].sacch_demod_data,PHY_vars->Sach_diagnostics[user_index][tchan_index].sach_demod_data,sacch_data,sach_data);
	*/
	for (i=0;i<nb_sacch_carriers>>1;i++) {
	  sacch_re[i] = (float)sacch_data[i<<2]; 
	  sacch_im[i] = (float)sacch_data[(i<<2) + 1];
	  //	  printf("sacch: i=%d : %d %d %d %d\n",i,sacch_data[i<<2],sacch_data[1+(i<<2)],sacch_data[2+(i<<2)],sacch_data[3+(i<<2)]);
	  avg += fabs(sacch_re[i])+fabs(sacch_im[i]);
	}
	for (i=0;i<nb_sach_carriers>>1;i++) {
	    sach_re[i] = (float)sach_data[i<<2]; 
	    sach_im[i] = (float)sach_data[(i<<2) + 1];
//	    	    printf("sach : i=%d : %d %d %d %d\n",i,sach_data[i<<2],sach_data[1+(i<<2)],sach_data[2+(i<<2)],sach_data[3+(i<<2)]);
	avg += fabs(sach_re[i])+fabs(sach_im[i]);
	}

	avg/=(nb_sacch_carriers+nb_sach_carriers);

	for (i=0;i<nb_sacch_carriers>>1;i++) {
	  sacch_re[i] /= avg;
	  sacch_im[i] /= avg;
	  //	  printf("sacch: i=%d : %d %d %d %d\n",i,sacch_data[i<<2],sacch_data[1+(i<<2)],sacch_data[2+(i<<2)],sacch_data[3+(i<<2)]);
	}
	for (i=0;i<nb_sach_carriers>>1;i++) {
	  sach_re[i] /= avg;
	  sach_im[i] /= avg;
	    //	    printf("sach : i=%d : %d %d %d %d\n",i,sach_data[i<<2],sach_data[1+(i<<2)],sach_data[2+(i<<2)],sach_data[3+(i<<2)]);
	}
	//	avg = 2048.0;

	avg = 2.0;
	if ((user_index == 0) && (tchan_index == 0)) {
	  fl_set_xyplot_xbounds(form->sacch00,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch00,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach00,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach00,-avg,avg);

	  fl_set_xyplot_data(form->sacch00,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach00,sach_re,sach_im,nb_sach_carriers>>1,"","","");
	}

	if ((user_index == 0) && (tchan_index == 1)) {
	  fl_set_xyplot_xbounds(form->sacch01,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch01,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach01,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach01,-avg,avg);
	  fl_set_xyplot_data(form->sacch01,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach01,sach_re,sach_im,nb_sach_carriers>>1,"","","");

	}

	if ((user_index == 0) && (tchan_index == 2)) {
	  fl_set_xyplot_xbounds(form->sacch02,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch02,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach02,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach02,-avg,avg);

	  fl_set_xyplot_data(form->sacch02,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach02,sach_re,sach_im,nb_sach_carriers>>1,"","","");
	}

	if ((user_index == 0) && (tchan_index == 3)) {
	  fl_set_xyplot_data(form->sacch03,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach03,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 0) && (tchan_index == 4)) {
	  fl_set_xyplot_data(form->sacch04,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach04,sach_re,sach_im,nb_sach_carriers,"","","");
	}


	if ((user_index == 1) && (tchan_index == 0)) {

	  fl_set_xyplot_xbounds(form->sacch10,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch10,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach10,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach10,-avg,avg);

	  fl_set_xyplot_data(form->sacch10,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach10,sach_re,sach_im,nb_sach_carriers>>1,"","","");
	}

	if ((user_index == 1) && (tchan_index == 1)) {
	  fl_set_xyplot_xbounds(form->sacch11,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch11,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach11,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach11,-avg,avg);
	  fl_set_xyplot_data(form->sacch11,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach11,sach_re,sach_im,nb_sach_carriers>>1,"","","");
	}

	if ((user_index == 1) && (tchan_index == 2)) {
	  fl_set_xyplot_xbounds(form->sacch12,-avg,avg);
	  fl_set_xyplot_ybounds(form->sacch12,-avg,avg);
	  fl_set_xyplot_xbounds(form->sach12,-avg,avg);
	  fl_set_xyplot_ybounds(form->sach12,-avg,avg);

	  fl_set_xyplot_data(form->sacch12,sacch_re,sacch_im,nb_sacch_carriers>>1,"","","");
	  fl_set_xyplot_data(form->sach12,sach_re,sach_im,nb_sach_carriers>>1,"","","");
	}

	if ((user_index == 1) && (tchan_index == 3)) {
	  fl_set_xyplot_data(form->sacch13,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach13,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 1) && (tchan_index == 4)) {
	  fl_set_xyplot_data(form->sacch13,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach13,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 2) && (tchan_index == 0)) {
	  fl_set_xyplot_data(form->sacch20,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach20,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 2) && (tchan_index == 1)) {
	  fl_set_xyplot_data(form->sacch21,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach21,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 2) && (tchan_index == 2)) {
	  fl_set_xyplot_data(form->sacch22,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach22,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 2) && (tchan_index == 3)) {
	  fl_set_xyplot_data(form->sacch23,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach23,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 2) && (tchan_index == 4)) {
	  fl_set_xyplot_data(form->sacch24,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach24,sach_re,sach_im,nb_sach_carriers,"","","");
	}


	if ((user_index == 3) && (tchan_index == 0)) {
	  fl_set_xyplot_data(form->sacch30,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach30,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 3) && (tchan_index == 1)) {
	  fl_set_xyplot_data(form->sacch31,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach31,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 3) && (tchan_index == 2)) {
	  fl_set_xyplot_data(form->sacch32,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach32,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 3) && (tchan_index == 3)) {
	  fl_set_xyplot_data(form->sacch33,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach33,sach_re,sach_im,nb_sach_carriers,"","","");
	}

	if ((user_index == 3) && (tchan_index == 4)) {
	  fl_set_xyplot_data(form->sacch33,sacch_re,sacch_im,nb_sacch_carriers,"","","");
	  fl_set_xyplot_data(form->sach33,sach_re,sach_im,nb_sach_carriers,"","","");
	}

      }  // active
      
    } // tchan loop

  } // user loop

    usleep(10000);

}

//-----------------------------------------------------------------------------
do_scope(){

//-----------------------------------------------------------------------------
  char ch;

  fl_set_idle_callback(sach_scope_idle_callback, NULL);
  fl_do_forms() ;      /* SIGSCOPE */



}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
//-----------------------------------------------------------------------------

  int openair_fd,i;

  unsigned int first_symbol;
  char title[20];
  short *sach_data;

  if (argc<4) {
    printf("Not enough arguments: sach_scope nb_tx_antennas sch_index \n");
    exit(-1);
  }

  PHY_vars = malloc(sizeof(PHY_VARS));

  PHY_config = malloc(sizeof(PHY_CONFIG));
  /*
   if((config = fopen("./widens_config.cfg","r")) == NULL) // this can be configured
	{
	  printf("[Main USER] The widens configuration file <widens_config.cfg> could not be found!");
	  exit(0);
	}		
  
  if ((scenario= fopen("./widens_scenario.scn","r")) ==NULL)
    {
      printf("[Main USER] The widens scenario file <widens_scenario.scn> could not be found!");
      exit(0);
    }
  
  printf("Opened configuration files\n");

  reconfigure_MACPHY(scenario);
  */

  printf("Opening /dev/openair0\n");
  if ((openair_fd = open("/dev/openair0", O_RDONLY)) <0) {
    fprintf(stderr,"Error %d opening /dev/openair0\n",openair_fd);
    exit(-1);
  }

  printf("Getting PHY_vars ...\n");

  ioctl(openair_fd,openair_GET_VARS,PHY_vars);

  printf("Getting PHY_vars->tx_vars[0].TX_DMA_BUFFER=%p\n",PHY_vars->tx_vars[0].TX_DMA_BUFFER);

  printf("Getting PHY_config ...\n");

  ioctl(openair_fd,openair_GET_CONFIG,PHY_config);

  printf("NUMBER_OF_OFDM_CARRIERS = %d\n",NUMBER_OF_OFDM_CARRIERS);
  
  
  mem_base = mmap(0,
		  4096*4096,
		  PROT_READ,
		  MAP_PRIVATE,
		  openair_fd,
		  0);
  if (mem_base != -1)
    msg("MEM base= %p: (%x,%x,%x,%x)\n",mem_base,
	((int*)mem_base)[0],
	((int*)mem_base)[1],
	((int*)mem_base)[2],
	((int*)mem_base)[3]);
  else
    msg("Could not map physical memory\n");

  if (atoi(argv[1]) == 0) {    // Clusterhead
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[atoi(argv[3])].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      printf("channel_f[%d]=%p (%x,%x,%x,%x)\n",i,channel_f[i],channel_f[i][0],channel_f[i][1],channel_f[i][2],channel_f[i][3]);
    }
    
    sach_data = (short *)(mem_base + (unsigned int)PHY_vars->Sach_diagnostics[0][0].sach_demod_data-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
    printf("sach_data %p (%p)\n",PHY_vars->Sach_diagnostics[0][0].sach_demod_data,sach_data);

    printf("sach_data %p, (%x,%x,%x,%x)\n",sach_data,sach_data[0],sach_data[1],sach_data[2],sach_data[3]);
  }
  else if (atoi(argv[1]) == 1) {  // UE

    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->chsch_data[atoi(argv[3])].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      printf("channel_f[%d]=%p\n",i,channel[i]);
    }

  }
  else {
    printf("Unknown node role %d\n",atoi(argv[1]));
    close(openair_fd);
    exit(-1);
    
  }

  nb_tx_ant = atoi(argv[2]);

  sprintf(title, "SACH SCOPE %d %d %d", atoi(argv[1]), atoi(argv[2]), atoi(argv[3])),

  fl_initialize(&argc, argv, title, 0, 0);    /* SIGSCOPE */
  form = create_form_sach_scope();                 /* SIGSCOPE */
  fl_show_form(form->sach_scope,FL_PLACE_HOTSPOT,FL_FULLBORDER,title);   /* SIGSCOPE */

  do_scope();
  sleep(5);
  close(openair_fd);
  return(0);
}
