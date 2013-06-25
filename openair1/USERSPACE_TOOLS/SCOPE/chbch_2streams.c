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
#include "chbch_2streams_scope.h"

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


FD_chbch_scope *form;


//short channel[2048];
//short channel_f[2048];
//char demod_data[2048];

short *channel[4],*channel_f[4],*rx_sig[4],*rx_sig_f2[4],*rx_sig_f3[4];
int  *determ;
short *ideterm;
char *demod_data,*rx_sig_f4;

int length,offset;
float avg=1;
int sach_flag; 
int antenna_idx=0; //needed to read correct data for MMSE receiver

unsigned char nb_tx_ant;

void chbch_scope_idle_callback(void) {

  int i,j,ind,k;

  float Re,Im,mag_sig[NB_ANTENNAS_RX*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*8],mag_sig2[NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*8],sig_time[NB_ANTENNAS_RX*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*8],scat_sig_re[NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*8],scat_sig_im[NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*8];
  float cum_avg;
  float det[NUMBER_OF_OFDM_CARRIERS], idet[NUMBER_OF_OFDM_CARRIERS], carriers[NUMBER_OF_OFDM_CARRIERS];

  for (i=0;i<2*CYCLIC_PREFIX_LENGTH;i++){
    sig_time[i] = (float)i;
    Re = (float)(channel[0][4*(NUMBER_OF_OFDM_CARRIERS - 2*CYCLIC_PREFIX_LENGTH) + 4*i]);
    Im = (float)(channel[0][4*(NUMBER_OF_OFDM_CARRIERS - 2*CYCLIC_PREFIX_LENGTH) + 1+(4*i)]);
    mag_sig[i] = 10*log10(1+Re*Re + Im*Im);
    Re = (float)(channel[1][4*(NUMBER_OF_OFDM_CARRIERS - 2*CYCLIC_PREFIX_LENGTH) + 4*i]);
    Im = (float)(channel[1][4*(NUMBER_OF_OFDM_CARRIERS - 2*CYCLIC_PREFIX_LENGTH) + 1+(4*i)]);
    mag_sig2[i] = 10*log10(1+Re*Re + Im*Im);
  }
  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,136,"","","");
  fl_set_xyplot_ybounds(form->channel_t_re,30,80);
  fl_set_xyplot_ybounds(form->channel_t_im,30,80);
  fl_set_xyplot_data(form->channel_t_re,sig_time,mag_sig,2*CYCLIC_PREFIX_LENGTH,"","","");
  fl_set_xyplot_data(form->channel_t_im,sig_time,mag_sig2,2*CYCLIC_PREFIX_LENGTH,"","","");

  cum_avg = 0;
  ind = 0;
  for (k=0;k<NB_ANTENNAS_RX;k++){
    for (j=0;j<nb_tx_ant;j++) {
      
      for (i=0;i<NUMBER_OF_OFDM_CARRIERS/nb_tx_ant;i++){
	sig_time[ind] = (float)ind;
	Re = (float)(channel_f[k][(4*j)+  (4*(((i*nb_tx_ant)+FIRST_CARRIER_OFFSET)%NUMBER_OF_OFDM_CARRIERS))]);
	Im = (float)(channel_f[k][(4*j)+1+(4*(((i*nb_tx_ant)+FIRST_CARRIER_OFFSET)%NUMBER_OF_OFDM_CARRIERS))]);
	if (((i*nb_tx_ant)< NUMBER_OF_USEFUL_CARRIERS)){
	  mag_sig[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im)); 
	  cum_avg += (short)sqrt((double)Re*Re + (double)Im*Im) ;
	}
	else
	  mag_sig[ind] = 0;
	ind++;
      }
      //      ind+=NUMBER_OF_OFDM_CARRIERS/4; // spacing for visualization
    }
  }

  avg = cum_avg/NUMBER_OF_USEFUL_CARRIERS;

  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,136,"","","");
  fl_set_xyplot_ybounds(form->channel_f,30,70);
  fl_set_xyplot_data(form->channel_f,sig_time,mag_sig,NB_ANTENNAS_RX*NUMBER_OF_OFDM_CARRIERS,"","","");
  j=0;

  if (sach_flag==0) // CHBCH or MRBCH 
    for (i=0;i<256*length;i+=2){
      
      if ((i%256) == 80)
	i+=96;
      
      sig_time[j] = (float)j;
      mag_sig[j++] = (float)(rx_sig_f2[antenna_idx][4*i]); //+rx_sig_f2[1][4*i]);
      sig_time[j] = (float)j;
      mag_sig[j++] = (float)(rx_sig_f2[antenna_idx][1+(4*i)]); //+rx_sig_f2[1][1+(4*i)]);
      
    }
  else {
    for (i=0;i<256*length;i++){
      
      sig_time[j] = (float)j;
      mag_sig[j++] = (float)(rx_sig_f2[0][4*i]+rx_sig_f2[1][4*i]);
      sig_time[j] = (float)j;
      mag_sig[j++] = (float)(rx_sig_f2[0][1+(4*i)]+rx_sig_f2[1][1+(4*i)]);
    }
  }
  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,136,"","","");
  fl_set_xyplot_ybounds(form->demod_out,-4096,4096);
  fl_set_xyplot_data(form->demod_out,sig_time,mag_sig,160*length,"","","");


  j=0;
  for (i=0;i<(length-1)*160*2;i++){

    sig_time[j] = (float)j;
    mag_sig[j++] = (float)demod_data[i];
    sig_time[j] = (float)j;
    mag_sig[j++] = (float)demod_data[i];
  }
  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,136,"","","");
  fl_set_xyplot_ybounds(form->decoder_input,-10,10);
  if (length == 8)
    fl_set_xyplot_data(form->decoder_input,sig_time,mag_sig,2*160*8,"","","");
  else
    fl_set_xyplot_data(form->decoder_input,sig_time,&mag_sig[384],384,"","","");

  j=0;
  if (sach_flag == 0) 
    for (i=0;i<256*length;i+=2){
      
      if ((i%256) == 0)
	i+= 16;
      
      if ((i%256) == 40)
	i+=226;
      
      
      Re = .5*(float)(rx_sig_f2[antenna_idx][4*i]); // + rx_sig_f2[1][4*i]); 
      Im = .5*(float)(rx_sig_f2[antenna_idx][1+(4*i)]); //+ rx_sig_f2[1][1+(4*i)]); 
      
      scat_sig_re[j] = Re;
      scat_sig_im[j] = Im;
      j++;
      
      
    }
  else {
    for (i=0;i<256*length;i+=2){
      
      if ((i%160) == 0)
	i+= 16;
      
      if ((i%160) == 40)
	i+=120;
      
      
      Re = .5*(float)(rx_sig_f3[0][2*i] + rx_sig_f3[1][2*i]); 
      Im = .5*(float)(rx_sig_f3[0][1+(2*i)] + rx_sig_f3[1][1+(2*i)]); 
      
      scat_sig_re[j] = Re;
      scat_sig_im[j] = Im;
      j++;
    }
  }
  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,j,"","","");
  fl_set_xyplot_ybounds(form->scatter_plot,-4096,4096);
  fl_set_xyplot_xbounds(form->scatter_plot,-4096,4096);
  fl_set_xyplot_data(form->scatter_plot,scat_sig_re,scat_sig_im,j,"","","");
  
  for (i=0;i<(NUMBER_OF_OFDM_CARRIERS+CYCLIC_PREFIX_LENGTH)*NUMBER_OF_SYMBOLS_PER_FRAME;i++){
    sig_time[i] = (float)i;
    mag_sig[i] = (float)(rx_sig[0][2*i]);
  }
  //fl_set_xyplot_data(form->scatter_sig,real_mf,imag_mf,136,"","","");
  fl_set_xyplot_ybounds(form->control_miniframe,-1024,1024);
  fl_set_xyplot_data(form->control_miniframe,sig_time,mag_sig,(NUMBER_OF_OFDM_CARRIERS+CYCLIC_PREFIX_LENGTH),"","","");

  //plot inverse determinant
  if(sach_flag==0) {
    for (i=0; i<NUMBER_OF_OFDM_CARRIERS; i++) {
      carriers[i] = (float)i;
      det[i] = 10*log10(1.0+(float) determ[2*i]);
      idet[i] = 10*log10(1.0+(float) ideterm[4*i]);
    }
    fl_set_xyplot_data(form->determ,carriers,det,NUMBER_OF_OFDM_CARRIERS,"","","");
    fl_set_xyplot_ybounds(form->determ,0,50);
    fl_set_xyplot_data(form->ideterm,carriers,idet,NUMBER_OF_OFDM_CARRIERS,"","","");
    fl_set_xyplot_ybounds(form->ideterm,0,50);
  }

  usleep(10000);
}
//-----------------------------------------------------------------------------
do_scope(){

//-----------------------------------------------------------------------------
  char ch;

  fl_set_idle_callback(chbch_scope_idle_callback, NULL);
  fl_do_forms() ;      /* SIGSCOPE */



}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
//-----------------------------------------------------------------------------

  int openair_fd,i;
  unsigned int mem_base;
  unsigned int first_symbol;
  char title[20];

  if (argc<4) {
    printf("Not enough arguments (transport_channel nb_tx_antennas chsch_index (only relevant for bch))\n");
    printf("0=CHBCH 1=RACH 2=DCCH/SACH 3=MRBCH\n");
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

  printf("PHY_vars chbch_data.rx_vars[0].RX_DMA_BUFFER = %p\n",PHY_vars->rx_vars[0].RX_DMA_BUFFER);
  printf("PHY_vars chsch_data.chsch_channel[0] = %p\n",PHY_vars->chsch_data[1].channel[0]);
  printf("PHY_vars chsch_data.chsch_channel_f[0] = %p\n",PHY_vars->chsch_data[1].channel_f[0]);
  printf("PHY_vars chbch_data.rx_sig_f2[0] = %p\n",PHY_vars->chbch_data[1].rx_sig_f2[0]);

  printf("NUMBER_OF_OFDM_CARRIERS = %d\n",NUMBER_OF_OFDM_CARRIERS);
  
  
  mem_base = mmap(0,
		  2048*4096,
		  PROT_READ,
		  MAP_PRIVATE,
		  openair_fd,
		  0);
  if (mem_base != -1)
    msg("MEM base= %p\n",mem_base);
  else
    msg("Could not map physical memory\n");

  if (atoi(argv[1]) == 0) {    // CHBCH
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel[i]    = (short*)(mem_base + (unsigned int)PHY_vars->chsch_data[atoi(argv[3])].channel[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->chsch_data[atoi(argv[3])].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig[i] = (short *)(mem_base + (unsigned int)PHY_vars->rx_vars[i].RX_DMA_BUFFER-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig_f2[i] = (short *)(mem_base + (unsigned int)PHY_vars->chbch_data[atoi(argv[3])].rx_sig_f2[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      printf("channel[%d] = %p,channel_f[%d]=%p,rx_sig[%d]=%p,rx_sig_f2[%d]=%p\n",i,channel[i],i,channel_f[i],i,rx_sig[i],i,rx_sig_f2[i]);
    }
    rx_sig_f4  = (char*)(mem_base + (unsigned int)PHY_vars->chbch_data[atoi(argv[3])].rx_sig_f4 - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

    demod_data  = (char *)(mem_base + (unsigned int)PHY_vars->chbch_data[atoi(argv[3])].demod_data - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);

    determ = (int *)(mem_base + (unsigned int)PHY_vars->chsch_data[1].det - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);
    ideterm = (short *)(mem_base + (unsigned int)PHY_vars->chsch_data[1].idet - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);
      
    length = 8;
    sach_flag = 0;
    if (atoi(argv[3])==2)
      antenna_idx=1;
    else
      antenna_idx=0;
  }
  else if (atoi(argv[1]) == 1) {  // RACH
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel[i]    = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[0].channel[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[0].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      first_symbol = 2+(NUMBER_OF_SYMBOLS_PER_FRAME>>1);
      rx_sig[i] = (short *)(mem_base + 
			    (unsigned int)&PHY_vars->rx_vars[i].RX_DMA_BUFFER[(first_symbol<<LOG2_NUMBER_OF_OFDM_CARRIERS)]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig_f2[i] = (short *)(mem_base + (unsigned int)PHY_vars->sach_data[0].rx_sig_f2[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      printf("channel[%d] = %p,channel_f[%d]=%p,rx_sig[%d]=%p\n",i,channel[i],i,channel_f[i],i,rx_sig[i]);
      rx_sig_f3[i] = (short *)(mem_base + (unsigned int)PHY_vars->sach_data[0].rx_sig_f3[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

    }

    rx_sig_f4  = (char*)(mem_base + (unsigned int)PHY_vars->sach_data[0].rx_sig_f4 - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

    demod_data  = (char *)(mem_base + (unsigned int)PHY_vars->sach_data[0].demod_data - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);
    
    length = 1;
    sach_flag = 1;
  }
  else if (atoi(argv[1]) == 2) {  // DCCH/SACH
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel[i]    = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[1].channel[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[1].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      first_symbol = 8+(NUMBER_OF_SYMBOLS_PER_FRAME>>1);
      rx_sig[i] = (short *)(mem_base + 
			    (unsigned int)&PHY_vars->rx_vars[i].RX_DMA_BUFFER[(first_symbol<<LOG2_NUMBER_OF_OFDM_CARRIERS)]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig_f2[i] = (short *)(mem_base + (unsigned int)PHY_vars->sach_data[1].rx_sig_f2[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig_f3[i] = (short *)(mem_base + (unsigned int)PHY_vars->sach_data[1].rx_sig_f3[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

      printf("channel[%d] = %p,channel_f[%d]=%p,rx_sig[%d]=%p\n",i,channel[i],i,channel_f[i],i,rx_sig[i]);
    }
    rx_sig_f4 = (char*)(mem_base + (unsigned int)PHY_vars->sach_data[1].rx_sig_f4 - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

    demod_data  = (char *)(mem_base + (unsigned int)PHY_vars->sach_data[1].demod_data - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);

    printf("demod_data = %p\n",demod_data);

    length = 20;
    sach_flag = 1;
  }
  else if (atoi(argv[1]) == 3) {    // MRBCH
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      channel[i]    = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[MRSCH_INDEX].channel[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      channel_f[i]  = (short*)(mem_base + (unsigned int)PHY_vars->sch_data[MRSCH_INDEX].channel_f[i] - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig[i] = (short *)(mem_base + (unsigned int)PHY_vars->rx_vars[i].RX_DMA_BUFFER-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      rx_sig_f2[i] = (short *)(mem_base + (unsigned int)PHY_vars->mrbch_data[0].rx_sig_f2[i]-(unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);
      printf("channel[%d] = %p,channel_f[%d]=%p,rx_sig[%d]=%p,rx_sig_f2[%d]=%p\n",i,channel[i],i,channel_f[i],i,rx_sig[i],i,rx_sig_f2[i]);
    }
    rx_sig_f4  = (char*)(mem_base + (unsigned int)PHY_vars->mrbch_data[0].rx_sig_f4 - (unsigned int)&PHY_vars->tx_vars[0].TX_DMA_BUFFER[0]);

    demod_data  = (char *)(mem_base + (unsigned int)PHY_vars->mrbch_data[0].demod_data - (unsigned int)PHY_vars->tx_vars[0].TX_DMA_BUFFER);
      
    length = 1;
    sach_flag = 0;
  }

  else {
    printf("Unknown command %d\n",atoi(argv[1]));
    close(openair_fd);
    exit(-1);
    
  }

  nb_tx_ant = atoi(argv[2]);

  for (i=0;i<16;i++)
    printf("%x\n",rx_sig[0][i]);

  sprintf(title, "CHBCH SCOPE %d %d %d", atoi(argv[1]), atoi(argv[2]), atoi(argv[3])),

  fl_initialize(&argc, argv, title, 0, 0);    /* SIGSCOPE */
  form = create_form_chbch_scope();                 /* SIGSCOPE */
  fl_show_form(form->chbch_scope,FL_PLACE_HOTSPOT,FL_FULLBORDER,title);   /* SIGSCOPE */

  do_scope();
  sleep(5);
  close(openair_fd);
  return(0);
}
