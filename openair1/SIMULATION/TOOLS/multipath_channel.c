#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "defs.h"
#include "SIMULATION/RF/defs.h"

//#define DEBUG_CH
/*
int init_multipath_channel(channel_desc_t *channel) {
  int i;
  if (channel->nb_taps<=0) {
    msg("init_multipath_channel: nb_taps must be > 0\n");
    return(-1);
  }
  if (channel->channel_length<=0) {
    msg("init_multipath_channel: channel_length must be > 0\n");
    return(-1);
  }
  if (channel->nb_tx<=0) {
    msg("init_multipath_channel: nb_tx must be > 0\n");
    return(-1);
  }
  if (channel->nb_rx<=0) {
    msg("init_multipath_channel: nb_rx must be > 0\n");
    return(-1);
  }
  channel->amps = (double*) malloc(channel->nb_taps*sizeof(double));
  if (!channel->amps) {
    msg("init_multipath_channel: cannot allocate amps\n");
    return(-1);
  }
  channel->delays = (double*) malloc(channel->nb_taps*sizeof(double));
  if (!channel->delays) {
    msg("init_multipath_channel: cannot allocate delays\n");
    return(-1);
  }
  channel->state = (struct complex **) malloc(channel->nb_tx*channel->nb_rx*sizeof(struct complex *));
  if (!channel->state) {
    msg("init_multipath_channel: cannot allocate state\n");
    return(-1);
  }
  for (i=0; i<channel->nb_tx*channel->nb_rx; i++) {
    channel->state[i] = (struct complex *) malloc(channel->nb_taps*sizeof(struct complex));
    if (!channel->state[i]) {
      msg("init_multipath_channel: cannot allocate state\n");
      return(-1);
    }
  }
  channel->ch = (struct complex **) malloc(channel->nb_tx*channel->nb_rx*sizeof(struct complex *));
  if (!channel->ch) {
    msg("init_multipath_channel: cannot allocate ch\n");
    return(-1);
  }
  for (i=0; i<channel->nb_tx*channel->nb_rx; i++) {
    channel->ch[i] = (struct complex *) malloc(channel->channel_length*sizeof(struct complex));
    if (!channel->ch[i]) {
      msg("init_multipath_channel: cannot allocate ch\n");
      return(-1);
    }
  }
  channel->R_tx_sqrt = (struct complex **) malloc(channel->nb_tx*channel->nb_tx*sizeof(struct complex *));
  if (!channel->Rx_tx_sqrt) {
    msg("init_multipath_channel: cannot allocate R_tx_sqrt\n");
    return(-1);
  }
  for (i=0; i<channel->nb_tx*channel->nb_tx; i++) {
    channel->R_tx_sqrt[i] = (struct complex *) malloc(channel->channel_length*sizeof(struct complex));
    if (!channel->R_tx_sqrt[i]) {
      msg("init_multipath_channel: cannot allocate R_tx_sqrt\n");
      return(-1);
    }
  }
  channel->R_rx_sqrt = (struct complex **) malloc(channel->nb_rx*channel->nb_rx*sizeof(struct complex *));
  if (!channel->R_rx_sqrt) {
    msg("init_multipath_channel: cannot allocate R_rx_sqrt\n");
    return(-1);
  }
  for (i=0; i<channel->nb_rx*channel->nb_rx; i++) {
    channel->R_rx_sqrt[i] = (struct complex *) malloc(channel->channel_length*sizeof(struct complex));
    if (!channel->R_rx_sqrt[i]) {
      msg("init_multipath_channel: cannot allocate R_rx_sqrt\n");
      return(-1);
    }
  }
  return(0);
}

int free_multipath_channel(channel_desc_t *channel) {
  int i;
  free(channel->amps);
  free(channel->delays);
  for (i=0; i<channel->nb_tx*channel->nb_rx; i++) {
    free(channel->state[i] = (struct complex *) malloc(channel->nb_taps*sizeof(struct complex));
    if (!channel->state[i]) {
      msg("init_multipath_channel: cannot allocate state\n");
      return(-1);
    }
  }
  free(channel->state);
  channel->ch = (struct complex **) malloc(channel->nb_tx*channel->nb_rx*sizeof(struct complex *));
  if (!channel->ch) {
    msg("init_multipath_channel: cannot allocate ch\n");
    return(-1);
  }
  for (i=0; i<channel->nb_tx*channel->nb_rx; i++) {
    channel->ch[i] = (struct complex *) malloc(channel->channel_length*sizeof(struct complex));
    if (!channel->ch[i]) {
      msg("init_multipath_channel: cannot allocate ch\n");
      return(-1);
    }
  }
  channel->R_tx_sqrt = (struct complex *) malloc(channel->nb_tx*channel->nb_tx*sizeof(struct complex));
  if (!channel->Rx_tx_sqrt) {
    msg("init_multipath_channel: cannot allocate Rx_tx_sqrt\n");
    return(-1);
  }
  channel->R_rx_sqrt = (struct complex *) malloc(channel->nb_rx*channel->nb_rx*sizeof(struct complex));
  if (!channel->R_rx_sqrt) {
    msg("init_multipath_channel: cannot allocate R_rx_sqrt\n");
    return(-1);
  }
  return(0);
}
*/

u8 multipath_channel_nosigconv(channel_desc_t *desc){
  
       random_channel(desc);
       return(1);
}

//#define DEBUG_CH
void multipath_channel(channel_desc_t *desc,
		       double **tx_sig_re, 
		       double **tx_sig_im, 
		       double **rx_sig_re,
		       double **rx_sig_im,
		       u32 length,
		       u8 keep_channel) {
 
  int i,ii,j,l;
  struct complex rx_tmp,tx;

  double path_loss = pow(10,desc->path_loss_dB/20);
  int dd;
  dd = abs(desc->channel_offset);

#ifdef DEBUG_CH
  printf("[CHANNEL] keep = %d : path_loss = %g (%f), nb_rx %d, nb_tx %d, dd %d, len %d \n",keep_channel,path_loss,desc->path_loss_dB,desc->nb_rx,desc->nb_tx,dd,desc->channel_length);
#endif

    if (keep_channel) {
      // do nothing - keep channel
    } else {
      random_channel(desc);
    }

#ifdef DEBUG_CH
  for (l = 0;l<(int)desc->channel_length;l++) {
    printf("%p (%f,%f) ",desc->ch[0],desc->ch[0][l].x,desc->ch[0][l].y);
  }
  printf("\n");
#endif

  for (i=0;i<((int)length-dd);i++) {
    for (ii=0;ii<desc->nb_rx;ii++) {
      rx_tmp.x = 0;
      rx_tmp.y = 0;
      for (j=0;j<desc->nb_tx;j++) {
	for (l = 0;l<(int)desc->channel_length;l++) {
	  if ((i>=0) && (i-l)>=0) {
	    tx.x = tx_sig_re[j][i-l];
	    tx.y = tx_sig_im[j][i-l];
	  }
	  else {
	    tx.x =0;
	    tx.y =0;
	  }
	  rx_tmp.x += (tx.x * desc->ch[ii+(j*desc->nb_rx)][l].x) - (tx.y * desc->ch[ii+(j*desc->nb_rx)][l].y);
	  rx_tmp.y += (tx.y * desc->ch[ii+(j*desc->nb_rx)][l].x) + (tx.x * desc->ch[ii+(j*desc->nb_rx)][l].y);
	} //l
      }  // j
      rx_sig_re[ii][i+dd] = rx_tmp.x*path_loss;
      rx_sig_im[ii][i+dd] = rx_tmp.y*path_loss;
      /*
      if ((ii==0)&&((i%32)==0)) {
	printf("%p %p %f,%f => %e,%e\n",rx_sig_re[ii],rx_sig_im[ii],rx_tmp.x,rx_tmp.y,rx_sig_re[ii][i-dd],rx_sig_im[ii][i-dd]);
      }
      */
      //rx_sig_re[ii][i] = sqrt(.5)*(tx_sig_re[0][i] + tx_sig_re[1][i]);
      //rx_sig_im[ii][i] = sqrt(.5)*(tx_sig_im[0][i] + tx_sig_im[1][i]);
      
    } // ii
  } // i
}



