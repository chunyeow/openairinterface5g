#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "openair_device.h"


#define PI 3.14159265359
#define MAXPOW 24


FILE *rx_frame_file;
FILE *fid;

struct complex
{
  double r;
  double i;
};

struct complex_fixed
{
  short r;
  short i;
};

int pow_2[MAXPOW];
int pow_4[MAXPOW];


void twiddle(struct complex *W, int N, double stuff)
{
  W->r=cos(stuff*2.0*PI/(double)N);
  W->i=-sin(stuff*2.0*PI/(double)N);
}


void bit_r4_reorder(struct complex *W, int N)
{
  int bits, i, j, k;
  double tempr, tempi;
    
  for (i=0; i<MAXPOW; i++)
    if (pow_2[i]==N) bits=i;

  for (i=0; i<N; i++)
    {
      j=0;
      for (k=0; k<bits; k+=2)
	{
	  if (i&pow_2[k]) j+=pow_2[bits-k-2];
	  if (i&pow_2[k+1]) j+=pow_2[bits-k-1];
	}

      if (j>i)  /** Only make "up" swaps */
	{
	  tempr=W[i].r;
	  tempi=W[i].i;
	  W[i].r=W[j].r;
	  W[i].i=W[j].i;
	  W[j].r=tempr;
	  W[j].i=tempi;
	}
    }
}


/** RADIX-4 FFT ALGORITHM */
void radix4(struct complex *x, int N)
{ 
  int    n2, k1, N1, N2;
  struct complex W, bfly[4];

  N1=4;
  N2=N/4;
    
  /** Do 4 Point DFT */ 
  for (n2=0; n2<N2; n2++)
    {
      /** Don't hurt the butterfly */
      bfly[0].r = (x[n2].r + x[N2 + n2].r + x[2*N2+n2].r + x[3*N2+n2].r);
      bfly[0].i = (x[n2].i + x[N2 + n2].i + x[2*N2+n2].i + x[3*N2+n2].i);

      bfly[1].r = (x[n2].r + x[N2 + n2].i - x[2*N2+n2].r - x[3*N2+n2].i);
      bfly[1].i = (x[n2].i - x[N2 + n2].r - x[2*N2+n2].i + x[3*N2+n2].r);

      bfly[2].r = (x[n2].r - x[N2 + n2].r + x[2*N2+n2].r - x[3*N2+n2].r);
      bfly[2].i = (x[n2].i - x[N2 + n2].i + x[2*N2+n2].i - x[3*N2+n2].i);

      bfly[3].r = (x[n2].r - x[N2 + n2].i - x[2*N2+n2].r + x[3*N2+n2].i);
      bfly[3].i = (x[n2].i + x[N2 + n2].r - x[2*N2+n2].i - x[3*N2+n2].r);


      /** In-place results */
      for (k1=0; k1<N1; k1++)
	{
	  twiddle(&W, N, (double)k1*(double)n2);
	  x[n2 + N2*k1].r = bfly[k1].r*W.r - bfly[k1].i*W.i;
	  x[n2 + N2*k1].i = bfly[k1].i*W.r + bfly[k1].r*W.i;
	}
    }
    
  /** Don't recurse if we're down to one butterfly */
  if (N2!=1)
    for (k1=0; k1<N1; k1++)
      {
	radix4(&x[N2*k1], N2);
      }
}



void fft_distortion_test(int N, char test,double input_dB,char *scale,double *maxSNR,char *maxscale,struct complex *data,struct complex_fixed *data_fixed, int openair_fd) {


  double mean_in=0.0,mean_error=0.0,SNR;
  int i;


  for (i=0; i<N; i++)
    {
      data[i].r=0.0;
      data[i].i=0.0;
      data_fixed[i].r=0;
      data_fixed[i].i=0;
    }
      
      

  switch (test) {
  case 0:       /** Generate cosine **/
    for (i=0; i<N; i++)
      data[i].r=pow(10,.05*input_dB)*cos(2.0*PI*.1*i);

    break;
  case 1:    // QPSK

    QAM_input(data,(short)(pow(10,.05*input_dB)*32767.0),N,3*N/4,0);

    for (i=0;i<N;i++) {
      data[i].r /= 32767;
      data[i].i /= 32767;
    }
    
    break;

  case 2:    // 16-QAM
    QAM_input(data,(short)(pow(10,.05*input_dB)*32767.0),N,3*N/4,1);

    for (i=0;i<N;i++) {
      data[i].r /= 32767;
      data[i].i /= 32767;
    }
    break;

  default:
    break;
  }

  for (i=0;i<N;i++) {
    data_fixed[i].r = (short)(data[i].r*32767);
    data_fixed[i].i = (short)(data[i].i*32767);
    //    printf("data fields[%d] = %d : %d :: %d : %d\n",i,(short)data[i].r,data_fixed[i].r,(short)data[i].i,data_fixed[i].i);
  }

  radix4(data, N);
  for (i=0;i<N;i++) {

    data[i].r /= sqrt(N);
    data[i].i /= sqrt(N);
  }
  bit_r4_reorder(data, N);



  //  radix4_xilixn(data_fixed, N,scale);
  //  bit_r4_reorder_fixed(data_fixed, N,scale[6]);

  ioctl(openair_fd,openair_FFT_TEST,(void *)scale);
  
  /*  
  if ((rx_frame_file = fopen("rx_frame.dat","w")) == NULL)
    {
      printf("[openair][FFT_TEST][INFO] Cannot open rx_frame.m data file\n");
      exit(0);
    }
  */
  data_fixed = &((struct complex_fixed *)scale)[2];
  
  /*
  fwrite(data_fixed,4,256,rx_frame_file);
  close(rx_frame_file);
  */
  


  mean_error = 0.0;
  mean_in = 0.0;
  for (i=0;i<N;i++) {
    mean_in += data[i].r*data[i].r + data[i].i*data[i].i;
    mean_error += pow((data[i].r-((double)data_fixed[i].r/32767.0)),2) + pow((data[i].i-((double)data_fixed[i].i/32767.0)),2);
    //    printf("data_fixed[%d] = %f : %d :: %f : %d\n",i,data[i].r*32767,data_fixed[i].r,data[i].i*32767,data_fixed[i].i);
  }
		

  SNR = 10*log10(mean_in/mean_error);
  // printf("%d %d %d %d %d %d %d : %f\n",scale[0],scale[1],scale[2],scale[3],scale[4],scale[5],scale[6],SNR);
  if (SNR > *maxSNR) {
    *maxSNR = SNR;
    memcpy(maxscale,scale,7);
  }



}

void fft_test(char *dma_buffer_local, char log2N, char test, int openair_fd)
{

  int    N=pow(2,log2N), radix=4; //,test=0;
  int    i;
  char maxscale[7],MAXSHIFT;
  char *scale;  //scale[7]
  double maxSNR,input_dB;
  struct complex *data;
  struct complex_fixed *data_fixed;



  set_taus_seed();

  if ((data=malloc(sizeof(struct complex)*(size_t)N))==NULL)
    {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
    }


  //  data_fixed = &((struct complex_fixed *)dma_buffer_local)[2];
  scale = &((char *)dma_buffer_local)[0];

 
  if ((data_fixed=malloc(sizeof(struct complex_fixed)*(size_t)N))==NULL)
    {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
    }


  /** Set up power of two arrays */
  pow_2[0]=1;
  for (i=1; i<MAXPOW; i++)
    pow_2[i]=pow_2[i-1]*2;
  pow_4[0]=1;
  for (i=1; i<MAXPOW; i++)
    pow_4[i]=pow_4[i-1]*4;
    
#ifdef OPEN_OUTPUT_FILE
  if ((fid = fopen("res_output.m","w")) == NULL)
    {
      printf("[FFT_TEST] Cannot open res_output.m data file\n");
      exit(0);
    }
  fprintf(fid,"res16_%d = [ \n",N);
#endif
  
  printf("res16_%d = [ \n",N);    
    
  for (input_dB=-50;input_dB<0;input_dB++) {
    //  input_dB = -40;
  
    switch (N) {
    case 64:
      //  scale = SCALE64;
      MAXSHIFT=3;
      break;
    case 256:
      //      scale = SCALE256;
      MAXSHIFT=4;
      break;
    case 1024:
      //      scale = SCALE1024;
      MAXSHIFT=5;
      break;
    case 4096:
      //      scale = SCALE4096;
      MAXSHIFT=6;
      break;
    }

    for (i=0;i<7;i++)
      scale[i] = 0;

    maxSNR = -1000;
    switch (N) {
    case 4096:

      for (scale[0]=0;scale[0]<=MAXSHIFT;scale[0]++)
	for (scale[1]=0;scale[1]<=MAXSHIFT-scale[0];scale[1]++)
	  for (scale[2]=0;scale[2]<=MAXSHIFT-scale[0]-scale[1];scale[2]++)
	    for (scale[3]=0;scale[3]<=MAXSHIFT-scale[0]-scale[1]-scale[2];scale[3]++)
	      for (scale[4]=0;scale[4]<=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3];scale[4]++)
		for (scale[5]=0;scale[5]<=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3]-scale[4];scale[5]++){
		  scale[6]=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3]-scale[4]-scale[5];
		  fft_distortion_test(N,test,input_dB,scale,&maxSNR,maxscale,data,data_fixed,openair_fd);
		
	      }
      printf("%f, %f, %% Optimum Scaling : %d %d %d %d %d %d %d\n",input_dB,maxSNR,maxscale[0],maxscale[1],maxscale[2],maxscale[3],maxscale[4],maxscale[5],maxscale[6]);
      break;
    case 1024:

      for (scale[0]=0;scale[0]<=MAXSHIFT;scale[0]++)
	for (scale[1]=0;scale[1]<=MAXSHIFT-scale[0];scale[1]++)
	  for (scale[2]=0;scale[2]<=MAXSHIFT-scale[0]-scale[1];scale[2]++)
	    for (scale[3]=0;scale[3]<=MAXSHIFT-scale[0]-scale[1]-scale[2];scale[3]++) 
	      for (scale[4]=0;scale[4]<=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3];scale[4]++){
		scale[6]=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3]-scale[4];
		fft_distortion_test(N,test,input_dB,scale,&maxSNR,maxscale,data,data_fixed,openair_fd);
	      }
      printf("%f, %f, %% Optimum Scaling : %d %d %d %d %d %d %d\n",input_dB,maxSNR,maxscale[0],maxscale[1],maxscale[2],maxscale[3],maxscale[4],maxscale[5],maxscale[6]);
      break;

    case 256:

      for (scale[0]=0;scale[0]<=MAXSHIFT;scale[0]++)
      	for (scale[1]=0;scale[1]<=MAXSHIFT-scale[0];scale[1]++)
	  for (scale[2]=0;scale[2]<=MAXSHIFT-scale[0]-scale[1];scale[2]++)
      	    for (scale[3]=0;scale[3]<=MAXSHIFT-scale[0]-scale[1]-scale[2];scale[3]++) {
	      scale[6]=MAXSHIFT-scale[0]-scale[1]-scale[2]-scale[3];
	      //      scale[0] = 2;
	      //      scale[1] = 1;
	      //      scale[2] = 1;
	      fft_distortion_test(N,test,input_dB,scale,&maxSNR,maxscale,data,&((struct complex_fixed *)dma_buffer_local)[2],openair_fd);
	    }
      printf("%f, %f, %% Optimum Scaling : %d %d %d %d %d %d %d\n",input_dB,maxSNR,maxscale[0],maxscale[1],maxscale[2],maxscale[3],maxscale[4],maxscale[5],maxscale[6]);
#ifdef OPEN_OUTPUT_FILE
      fprintf(fid,"%f, %f, %% Optimum Scaling : %d %d %d %d %d %d %d\n",input_dB,maxSNR,maxscale[0],maxscale[1],maxscale[2],maxscale[3],maxscale[4],maxscale[5],maxscale[6]);
#endif
      break;

    case 64:

      for (scale[0]=0;scale[0]<=MAXSHIFT;scale[0]++)
	for (scale[1]=0;scale[1]<=MAXSHIFT-scale[0];scale[1]++)
	  for (scale[2]=0;scale[2]<=MAXSHIFT-scale[0]-scale[1];scale[2]++) {
	      scale[6]=MAXSHIFT-scale[0]-scale[1]-scale[2];
	      fft_distortion_test(N,test,input_dB,scale,&maxSNR,maxscale,data,data_fixed,openair_fd);
	  }
      printf("%f, %f, %% Optimum Scaling : %d %d %d %d %d %d %d\n",input_dB,maxSNR,maxscale[0],maxscale[1],maxscale[2],maxscale[3],maxscale[4],maxscale[5],maxscale[6]);
      break;
      
    }
    /*
      
    for (i=0;i<N;i++) {
    printf("%d + sqrt(-1)*(%d)\n",(short)(32767*data[i].r),(short)(32767*data[i].i));
    }
    printf("];\n");
    printf("out_fixed = [ \n");
    
    for (i=0;i<N;i++)
    printf("%d + sqrt(-1)*(%d)\n",(short)(data_fixed[i].r),(short)(data_fixed[i].i));
    printf("];\n");
    */
  }
  
#ifdef OPEN_OUTPUT_FILE
  fprintf(fid,"];\n");
#endif
  printf("];\n");
}

