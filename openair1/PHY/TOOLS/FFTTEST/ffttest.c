#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "PHY/defs.h"
#include "PHY/TOOLS/defs.h"
#include "PHY/TOOLS/twiddle64.h"
#include "PHY/TOOLS/twiddle256.h"
#include "PHY/TOOLS/twiddle1024.h"
#include "PHY/TOOLS/twiddle4096.h"

#define PI 3.14159265359
#define MAXPOW 24


extern double gaussdouble(double,double);
extern unsigned int taus();
extern void randominit();

int pow_2[MAXPOW];
int pow_4[MAXPOW];

void twiddle(struct complex *W, int N, double stuff)
{
  W->r=cos(stuff*2.0*PI/(double)N);
  W->i=-sin(stuff*2.0*PI/(double)N);
}


void bit_r4_reorder(struct complex *W,// Final output  
		    int N)            // size of FFT

{
  int bits=0, i, j, k;
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
/* Double precision*/
void radix4(struct complex *x, int N)
{ 
  int    n2, k1, N1, N2;
  struct complex W, bfly[4];

  N1=4;
  N2=N/4;
    
  /** Do 4 Point DFT */ 
  for (n2=0; n2<N2; n2++)
    {
      /** Radix 4 butterfly */
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



void QAM_input(struct complex *data,double amp,int N,int Nu,char M) {

  int i,rv;
  int FCO = (N-(Nu>>1));   // First non-zero carrier offset

  for (i=0;i<N;i++) {
    data[i].r = 0.0;
    data[i].i = 0.0;
  }

  for (i=0;i<Nu;i++) {

    rv = taus();

    switch (M) {

    case 0 :   // QPSK
      data[(i+FCO)%N].r = ((rv&1) ? -amp : amp)/sqrt(2.0);
      data[(i+FCO)%N].r = (((rv>>1)&1) ? -amp : amp)/sqrt(2.0);
      break;
    case 1 :   // 16QAM 
      data[(i+FCO)%N].r = (2*(rv&3) - 3)*amp/sqrt(10);
      data[(i+FCO)%N].i = (2*((rv>>2)&3) - 3)*amp/sqrt(10);
      break;
    default:
      break;
    }
    
  }
}

void fft_distortion_test(int N,                              // dimension of FFT under test 
			 char test,                          // type of test
			 double input_dB,                    // strength of input
			 struct complex *data,               // pointer to floating point data vector
			 struct complex16 *data16,           // pointer to Q15 data vector
			 struct complex16 *data16_out,
			 unsigned short *rev)           // pointer to Q15 data vector
{


  double mean_in=0.0,mean_error=0.0,SNR;
  int i;

  short *twiddle;
  unsigned char log2N,log2N_over2;

  srand(1);
  randominit();

  for (i=0; i<N; i++)
    {
      data[i].r=0.0;
      data[i].i=0.0;
      data16[i].r=0;
      data16[i].i=0;
    }
      
      

  switch (test) {
  case 0:       /** Generate complex exponential **/
    for (i=0; i<N; i++){
      data[i].r=pow(10,.05*input_dB)*cos(2.0*PI*.1*i);
      data[i].i=pow(10,.05*input_dB)*sin(2.0*PI*.1*i);
    }
    break;
  case 1:    // QPSK
    QAM_input(data,pow(10,.05*input_dB),N,N,0);
    break;

  case 2:    // 16-QAM
    QAM_input(data,pow(10,.05*input_dB),N,N,1);
    break;

  case 3:       /** Generate cosine **/
    for (i=0; i<N; i++) {
      data[i].r=sqrt(.5)*pow(10,.05*input_dB)*gaussdouble(0.0,1.0);
      data[i].i=sqrt(.5)*pow(10,.05*input_dB)*gaussdouble(0.0,1.0);
    }
    break;
  default:

    break;
  }

  // Make fixed-point versions of data
  for (i=0;i<N;i++) {
    data16[i].r = (short)(data[i].r*32767);
    data16[i].i = (short)(data[i].i*32767);
  }

  // Do Floating-point FFT
  radix4(data, N);
  for (i=0;i<N;i++) {

    data[i].r /= sqrt(N);
    data[i].i /= sqrt(N);
  }
  bit_r4_reorder(data, N);

  switch (N) {
  case 64:
    twiddle = twiddle_fft64;
    log2N=6;
    log2N_over2=3;
    break;
  case 256:
    twiddle = twiddle_fft256;
    log2N=8;
    log2N_over2=4;
    break;
  case 1024:
    twiddle = twiddle_fft1024;
    log2N=10;
    log2N_over2=5;
    break;
  case 4096:
    log2N=12;
    log2N_over2=6;
    twiddle = twiddle_fft4096;
    break;
  default:
    twiddle = twiddle_fft256;
      log2N=8;
      log2N_over2=4;
      break;
  }
  

  // Do Q15 FFT
  fft((short*)data16,
      (short*)data16_out,
      twiddle,
      rev,
      log2N,
      log2N_over2,
      0);

  // Compute Distortion statistics
  mean_error = 0.0;
  mean_in = 0.0;
  for (i=0;i<N;i++) {
    mean_in += data[i].r*data[i].r + data[i].i*data[i].i;
    mean_error += pow((data[i].r-((double)data16_out[2*i].r/32767.0)),2) + pow((data[i].i-((double)data16_out[2*i].i/32767.0)),2);
  }
		  
  SNR = 10*log10(mean_in/mean_error);
  printf("Input %f -> SNR: %f (amp %f, %f)\n",input_dB,SNR,pow(10,.05*input_dB),32767*pow(10,.05*input_dB));
}

#define SCALE64 0x0006
#define SCALE256 0x0016
#define SCALE1024 0x0056
#define SCALE4096 0x0156

void main(int argc, char *argv[])
{

  int    N, test,log2N;
  int    i;

  double maxSNR,input_dB;
  struct complex *data;
  struct complex16 *data16;  
  struct complex16 *data16_out;

  unsigned short local_rev[4096];
  char fname[100],vname[100];

  if (argc!= 3) {
    printf("fft size(16-4096) test(0-3)!!\n");
    exit(-1);
  }

  N = atoi(argv[1]);

  test = atoi(argv[2]);

  if ((N&1) == 1) {
    printf("Size must be a power of 4!\n");
    exit(-1);
  }
   
  if ((test>3) || (test<0)) {
    printf("test must be in (0-3)\n");
    exit(-1);
  }


  switch (N) {
  case 64:
    log2N=6;
    break;
  case 256:
    log2N=8;
    break;
  case 1024:
    log2N=10;
    break;
  case 4096:
    log2N=12;
    break;
  default:
    printf("Illegal fft size\n");
    exit(-1);
    break;
  }

  init_fft(N,log2N,local_rev);

  set_taus_seed(3);


  /** Set up power of two arrays */
  pow_2[0]=1;
  for (i=1; i<MAXPOW; i++)
    pow_2[i]=pow_2[i-1]*2;
  pow_4[0]=1;
  for (i=1; i<MAXPOW; i++)
    pow_4[i]=pow_4[i-1]*4;
    
  if ((data=(struct complex*)malloc(sizeof(struct complex)*(size_t)N))==NULL)
    {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
    }

  if ((data16=(struct complex16*)malloc16(sizeof(struct complex16)*(size_t)N))==NULL)
    {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
    }

  if ((data16_out=(struct complex16*)malloc16(2*sizeof(struct complex16)*(size_t)N))==NULL)
    {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
    }

  printf("res_%d = [ \n",N);    
    
  for (input_dB=-60;input_dB<0;input_dB++) {
    


    maxSNR = -1000;
    fft_distortion_test(N,test,input_dB,data,data16,data16_out,local_rev);

    sprintf(fname,"fftoutput_m%d.m",(int)abs(input_dB));
    sprintf(vname,"fftout_m%d",(int)abs(input_dB));
    
    write_output(fname,vname,
		 (s16 *)data16_out,
		 N*2,
		 2,
		 1);

    sprintf(fname,"fftinput_m%d.m",(int)abs(input_dB));
    sprintf(vname,"fftin_m%d",(int)abs(input_dB));
    
    write_output(fname,vname,
		 (s16 *)data16,
		 N,
		 1,
		 1);
    
  }

}

