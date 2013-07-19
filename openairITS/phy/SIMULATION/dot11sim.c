#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "SIMULATION/TOOLS/defs.h"
#include "SIMULATION/RF/defs.h"

#include "phy/DOT11/defs.h"
#include "phy/DOT11/commonvars.h"
#include <malloc.h>

#define BW 5.0

//#include "PHY/TOOLS/twiddle64.h"

#include "UTIL/LOG/log.h"

#define FRAME_LENGTH_SAMPLES_MAX 76800

uint16_t rev64[64];

#define RX_THRES_dB 26

int main(int argc, char **argv) {

  char c;

  int i,aa,aarx;
  double sigma2, sigma2_dB=0,SNR,snr0=10.0,snr1=10.2;
  int snr1set=0;
  uint32_t *txdata,*rxdata[2];
  double *s_re[2],*s_im[2],*r_re[2],*r_im[2];
  double iqim=0.0;
  int trial, ntrials=1;
  int n_rx=1;

  int awgn_flag=0;
  int n_frames=1;
  channel_desc_t *ch;
  uint32_t tx_lev,tx_lev_dB;
  int interf1=-19,interf2=-19;
  SCM_t channel_model=AWGN;
  uint32_t sdu_length_samples;
  TX_VECTOR_t tx_vector;
  int errors=0,misdetected_errors=0,signal_errors=0;
  int symbols=0;
  int tx_offset = 0,rx_offset;
  RX_VECTOR_t *rxv;
  uint8_t *data_ind,*data_ind_rx;
  int no_detection=1;
  int missed_packets=0;
  uint8_t rxp;
  int off,off2;
  double txg,txg_dB;
  int log2_maxh;
  double  snr_array[100];
  int  errors_array[100];
  int  trials_array[100];
  int  misdetected_errors_array[100];
  int  signal_errors_array[100];
  int  missed_packets_array[100];
  int  cnt=0;
  char fname[100],vname[100];
  int stop=0;

  data_ind    = (uint8_t*)malloc(4095+2+1);
  data_ind_rx = (uint8_t*)malloc(4095+2+1);

  tx_vector.rate=1;
  tx_vector.sdu_length=256;
  tx_vector.service=0;

  logInit();

  randominit(0);
  set_taus_seed(0);

  // Basic initializations
  init_fft(64,6,rev64);
  init_interleavers();
  ccodedot11_init();
  ccodedot11_init_inv();
  phy_generate_viterbi_tables();

  init_crc32();

  data_ind[0] = 0;
  data_ind[1] = 0;

  tx_offset = taus()%(FRAME_LENGTH_SAMPLES_MAX/2);

  while ((c = getopt (argc, argv, "hag:n:s:S:z:r:p:d:")) != -1) {
    switch (c) {
    case 'a':
      printf("Running AWGN simulation\n");
      awgn_flag = 1;
      ntrials=1;
      break;
    case 'g':
      switch((char)*optarg) {
      case 'A': 
	channel_model=SCM_A;
	break;
      case 'B': 
	channel_model=SCM_B;
	break;
      case 'C': 
	channel_model=SCM_C;
	break;
      case 'D': 
	channel_model=SCM_D;
	break;
      case 'E': 
	channel_model=EPA;
	break;
      case 'F': 
	channel_model=EVA;
	break;
      case 'G': 
	channel_model=ETU;
	break;
      case 'H':
	channel_model=Rayleigh8;
      case 'I':
	channel_model=Rayleigh1;
      case 'J':
	channel_model=Rayleigh1_corr;
      case 'K':
	channel_model=Rayleigh1_anticorr;
      case 'L':
	channel_model=Rice8;
      case 'M':
	channel_model=Rice1;
	break;
      default:
	printf("Unsupported channel model!\n");
	exit(-1);
      }
      break;
    case 'd':
      tx_offset = atoi(optarg);
      break;
    case 'p':
      tx_vector.sdu_length = atoi(optarg);
      if (atoi(optarg)>4095) {
	printf("Illegal sdu_length %d\n",tx_vector.sdu_length);
	exit(-1);
      }
      break;
    case 'r':
      tx_vector.rate = atoi(optarg);
      if (atoi(optarg)>7) {
	printf("Illegal rate %d\n",tx_vector.rate);
	exit(-1);
      }
      break;
    case 'n':
      n_frames = atoi(optarg);
      break;
    case 's':
      snr0 = atof(optarg);
      printf("Setting SNR0 to %f\n",snr0);
      break;
    case 'S':
      snr1 = atof(optarg);
      snr1set=1;
      printf("Setting SNR1 to %f\n",snr1);
      break;
    case 'z':
      n_rx=atoi(optarg);
      if ((n_rx==0) || (n_rx>2)) {
	printf("Unsupported number of rx antennas %d\n",n_rx);
	exit(-1);
      }
      break;
    default:
    case 'h':
      printf("%s -h(elp) -a(wgn on) -p(extended_prefix) -N cell_id -f output_filename -F input_filename -g channel_model -n n_frames -t Delayspread -r Ricean_FactordB -s snr0 -S snr1 -x transmission_mode -y TXant -z RXant -i Intefrence0 -j Interference1 -A interpolation_file -C(alibration offset dB) -N CellId\n",argv[0]);
      printf("-h This message\n");
      printf("-a Use AWGN channel and not multipath\n");
      printf("-n Number of frames to simulate\n");
      printf("-s Starting SNR, runs from SNR0 to SNR0 + 5 dB.  If n_frames is 1 then just SNR is simulated\n");
      printf("-S Ending SNR, runs from SNR0 to SNR1\n");
      printf("-g [A,B,C,D,E,F,G] Use 3GPP SCM (A,B,C,D) or 36-101 (E-EPA,F-EVA,G-ETU) models (ignores delay spread and Ricean factor)\n");
      printf("-z Number of RX antennas used\n");
      printf("-F Input filename (.txt format) for RX conformance testing\n");
      exit (-1);
      break;
    }
  }

  if (n_frames==1)
    snr1 = snr0+.2;
  else
    snr1 = snr0+5;
  for (i=0;i<tx_vector.sdu_length;i++)
    data_ind[i+2] = i;//taus();  // randomize packet
  data_ind[tx_vector.sdu_length+2+4]=0;  // Tail byte

  // compute number of OFDM symbols in DATA period
  symbols = ((4+2+1+tx_vector.sdu_length)<<1) / nibbles_per_symbol[tx_vector.rate];
  if ((((4+2+1+tx_vector.sdu_length)<<1) % nibbles_per_symbol[tx_vector.rate]) > 0)
    symbols++;

  sdu_length_samples = (symbols + 5) * 80;

  printf("Number of symbols for sdu : %d, samples %d\n",symbols,sdu_length_samples);

  txdata = (uint32_t*)memalign(16,sdu_length_samples*sizeof(uint32_t));
  for (i=0;i<n_rx;i++) {
    rxdata[i] = (uint32_t*)memalign(16,(FRAME_LENGTH_SAMPLES_MAX+1280)*sizeof(uint32_t));
    bzero(rxdata[i],(FRAME_LENGTH_SAMPLES_MAX+1280)*sizeof(uint32_t));
  }
  s_re[0] = (double *)malloc(sdu_length_samples*sizeof(double));
  bzero(s_re[0],sdu_length_samples*sizeof(double));
  s_im[0] = (double *)malloc(sdu_length_samples*sizeof(double));
  bzero(s_im[0],sdu_length_samples*sizeof(double));
  for (i=0;i<n_rx;i++) {
    r_re[i] = (double *)malloc((sdu_length_samples+100)*sizeof(double));
    bzero(r_re[i],(sdu_length_samples+100)*sizeof(double));
    r_im[i] = (double *)malloc((sdu_length_samples+100)*sizeof(double));
    bzero(r_im[i],(sdu_length_samples+100)*sizeof(double));
  }
  
  ch = new_channel_desc_scm(1,
			    n_rx,
			    channel_model,
			    BW,
			    0.0,
			    0,
			    0);
  

  if (ch==NULL) {
    printf("Problem generating channel model. Exiting.\n");
    exit(-1);
  }


  phy_tx_start(&tx_vector,txdata,0,FRAME_LENGTH_SAMPLES_MAX,data_ind);

  tx_lev = signal_energy((int32_t*)txdata,320);
  tx_lev_dB = (unsigned int) dB_fixed(tx_lev);

  write_output("txsig0.m","txs", txdata,sdu_length_samples,1,1);

  // multipath channel

  for (i=0;i<sdu_length_samples;i++) {
    s_re[0][i] = (double)(((short *)txdata)[(i<<1)]);
    s_im[0][i] = (double)(((short *)txdata)[(i<<1)+1]);
  }
  
  for (SNR=snr0;SNR<snr1;SNR+=.2) {

    printf("n_frames %d SNR %f sdu_length %d rate %d\n",n_frames,SNR,tx_vector.sdu_length,tx_vector.rate);
    errors=0;
    misdetected_errors=0;
    signal_errors=0;
    missed_packets=0;
    stop=0;
    for (trial=0; trial<n_frames; trial++) {
      //      printf("Trial %d (errors %d), sdu_length_samples %d\n",trial,errors,sdu_length_samples);
      sigma2_dB = 25; //10*log10((double)tx_lev) - SNR;
      txg_dB = 10*log10((double)tx_lev) - (SNR + sigma2_dB);
      txg = pow(10.0,-.05*txg_dB);
      if (n_frames==1)
	printf("sigma2_dB %f (SNR %f dB) tx_lev_dB %f, txg %f\n",sigma2_dB,SNR,10*log10((double)tx_lev)-txg_dB,txg_dB);
      //AWGN
      sigma2 = pow(10,sigma2_dB/10);
      //      printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);
            
      //          sigma2 = 0;

      multipath_channel(ch,s_re,s_im,r_re,r_im,
			sdu_length_samples,0);

      if (n_frames==1) {
	printf("rx_level data symbol %f, tx_lev %f\n",
	       10*log10(signal_energy_fp(r_re,r_im,1,80,0)),
	       10*log10(tx_lev));
      }

      for (aa=0;aa<n_rx;aa++) {
	for (i=0; i<(sdu_length_samples+100); i++) {

	  
	  ((short*)&rxdata[aa][tx_offset])[(i<<1)]   = (short) (((txg*r_re[aa][i]) + sqrt(sigma2/2)*gaussdouble(0.0,1.0)));
	  ((short*)&rxdata[aa][tx_offset])[1+(i<<1)] = (short) (((txg*r_im[aa][i]) + (iqim*r_re[aa][i]*txg) + sqrt(sigma2/2)*gaussdouble(0.0,1.0)));

	  //	  if (i<128)
	  //	    printf("i%d : rxdata %d, txdata %d\n",i,((short *)rxdata[aa])[rx_offset+(i<<1)],((short *)txdata)[i<<1]);
	}
	
	for (i=0;i<tx_offset;i++) {
	  ((short*) rxdata[aa])[(i<<1)]   = (short) (sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	  ((short*) rxdata[aa])[1+(i<<1)] = (short) (sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	}	
	for (i=(tx_offset+sdu_length_samples+100);i<FRAME_LENGTH_SAMPLES_MAX;i++) {
	  ((short*) rxdata[aa])[(i<<1)]   = (short) (sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	  ((short*) rxdata[aa])[1+(i<<1)] = (short) (sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	}	
	
      }
      if (n_frames==1) {
	write_output("rxsig0.m","rxs", &rxdata[0][0],FRAME_LENGTH_SAMPLES_MAX,1,1);
      }
      no_detection=1;
      off = 0;
      while(off<FRAME_LENGTH_SAMPLES_MAX) {
     
	rxp = dB_fixed(signal_energy(rxdata[0]+off,512));
	if (n_frames==1)
	  printf("off %d: rxp %d (%d)\n",off,rxp,signal_energy(rxdata[0]+off,104));

	if (rxp>RX_THRES_dB) { 
	  if (off<105) 
	    off2 = FRAME_LENGTH_SAMPLES_MAX-105;
	  else
	    off2=off;
	  if ((initial_sync(&rxv,&rx_offset,&log2_maxh,(uint32_t*)rxdata[0],FRAME_LENGTH_SAMPLES_MAX,off2,1) == BUSY)) {
	    if (n_frames==1)
	      printf("Channel is busy, rxv %p, offset %d\n",(void*)rxv,rx_offset);
	    no_detection=0;
	    if (rxv) {
	      if (n_frames==1)
		printf("Rate %d, SDU_LENGTH %d\n",rxv->rate,rxv->sdu_length);
	      if ( (rxv->rate != tx_vector.rate)||(rxv->sdu_length != tx_vector.sdu_length)) {
		signal_errors++;
		if ((signal_errors > (n_frames/10)) && (trial>=100)) {
		  stop=1;
		}
		if (n_frames == 1)
		  printf("SIGNAL error: rx_offset %d, tx_offset %d (off2 %d)\n",rx_offset,tx_offset,off2);
		break;
	      }
	      else {
		memset(data_ind_rx,0,rxv->sdu_length+4+2+1);
		if (data_detection(rxv,data_ind_rx,(uint32_t*)rxdata[0],FRAME_LENGTH_SAMPLES_MAX,rx_offset,log2_maxh,NULL)) {
		  for (i=0;i<rxv->sdu_length+6;i++) {
		    if (data_ind[i]!=data_ind_rx[i]) {
		      //printf("error position %d : %x,%x\n",i,data_ind[i],data_ind_rx[i]);
		      misdetected_errors++;
		      errors++;
		    }
		  }
		  if ((errors > (n_frames/10)) && (trial>100)) {
		    stop=1;
		    break;
		  }
		} // initial_synch returns IDLE
		else {
		  errors++;
		  if (n_frames == 1) {
		    printf("Running data_detection fails\n");
		    
		    for (i=0;i<rxv->sdu_length+6;i++) {
		      if (data_ind[i]!=data_ind_rx[i]) {
			printf("error position %d : %x,%x\n",i,data_ind[i],data_ind_rx[i]);
		      }
		    }
		  }
		  if ((errors > (n_frames/10)) && (trial>=100)) {
		    stop=1;
		    break;
		  }
		  
		}
		break;
	      }
	    }
	  }
	}
    
	off+=105;
      }
      if (no_detection==1)
	missed_packets++;
      if (stop==1)
	break;
    }
    
    printf("\nSNR %f dB: errors %d/%d, misdetected errors %d/%d,signal_errors %d/%d, missed_packets %d/%d\n",SNR,errors,trial-signal_errors,misdetected_errors,trial-signal_errors,signal_errors,trial,missed_packets,trial);
    snr_array[cnt] = SNR;
    errors_array[cnt] = errors;
    trials_array[cnt] = trial;
    misdetected_errors_array[cnt] = misdetected_errors;
    signal_errors_array[cnt] = signal_errors;
    missed_packets_array[cnt] = missed_packets;
    cnt++;
    if (cnt>99) {
      printf("too many SNR points, exiting ...\n");
      break;
    }
    if (errors == 0)
      break;
#ifdef EXECTIME
    print_is_stats();
    print_dd_stats();
#endif
  }


  sprintf(fname,"SNR_%d_%d.m",tx_vector.rate,tx_vector.sdu_length);
  sprintf(vname,"SNR_%d_%d_v",tx_vector.rate,tx_vector.sdu_length);
  write_output(fname,vname,snr_array,cnt,1,7);
  sprintf(fname,"errors_%d_%d.m",tx_vector.rate,tx_vector.sdu_length);
  sprintf(vname,"errors_%d_%d_v",tx_vector.rate,tx_vector.sdu_length);
  write_output(fname,vname,errors_array,cnt,1,2);
  sprintf(fname,"trials_%d_%d.m",tx_vector.rate,tx_vector.sdu_length);
  sprintf(vname,"trials_%d_%d_v",tx_vector.rate,tx_vector.sdu_length);
  write_output(fname,vname,trials_array,cnt,1,2);
  sprintf(fname,"signal_errors_%d_%d.m",tx_vector.rate,tx_vector.sdu_length);
  sprintf(vname,"signal_errors_%d_%d_v",tx_vector.rate,tx_vector.sdu_length);
  write_output(fname,vname,signal_errors_array,cnt,1,2);
  free(data_ind);
  free(data_ind_rx);
  //  free_channel_desc_scm(ch);
  
  free(txdata);
  for (i=0;i<n_rx;i++) {
    free(rxdata[i]);
  }

  free(s_re[0]);
  free(s_im[0]);

  for (i=0;i<n_rx;i++) {
    free(r_re[i]);
    free(r_im[i]);
  }
  
  return(0);

}
