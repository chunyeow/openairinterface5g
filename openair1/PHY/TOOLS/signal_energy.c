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
#include "defs.h"

#ifndef EXPRESSMIMO_TARGET
#include "mmintrin.h"
#endif //EXPRESSMIMO_TARGET

// Compute Energy of a complex signal vector, removing the DC component!
// input  : points to vector
// length : length of vector in complex samples

#define shift 4
#define shift_DC 0


#ifndef EXPRESSMIMO_TARGET
int32_t signal_energy(int32_t *input,uint32_t length) {

  int32_t i;
  int32_t temp,temp2;
  register __m64 mm0,mm1,mm2,mm3;
  __m64 *in = (__m64 *)input;

#ifdef MAIN
  int16_t *printb;
#endif

  mm0 = _m_pxor(mm0,mm0);
  mm3 = _m_pxor(mm3,mm3);

  for (i=0;i<length>>1;i++) {
    
    mm1 = in[i]; 
    mm2 = mm1;
    mm1 = _m_pmaddwd(mm1,mm1);
    mm1 = _m_psradi(mm1,shift);// shift any 32 bits blocs of the word by the value shift
    mm0 = _m_paddd(mm0,mm1);// add the two 64 bits words 4 bytes by 4 bytes
    //    temp2 = mm0;
    //    printf("%d %d\n",((int *)&temp2)[0],((int *)&temp2)[1]);


    //    printb = (int16_t *)&mm2;
    //    printf("mm2 %d : %d %d %d %d\n",i,printb[0],printb[1],printb[2],printb[3]);

    mm2 = _m_psrawi(mm2,shift_DC);
    mm3 = _m_paddw(mm3,mm2);// add the two 64 bits words 2 bytes by 2 bytes

    //    printb = (int16_t *)&mm3;
    //    printf("mm3 %d : %d %d %d %d\n",i,printb[0],printb[1],printb[2],printb[3]);

  }

  /*
#ifdef MAIN
  printb = (int16_t *)&mm3;
  printf("%d %d %d %d\n",printb[0],printb[1],printb[2],printb[3]);
#endif
  */
  mm1 = mm0;

  mm0 = _m_psrlqi(mm0,32);

  mm0 = _m_paddd(mm0,mm1);

  temp = _m_to_int(mm0);

  temp/=length;
  temp<<=shift;   // this is the average of x^2

  // now remove the DC component
  

  mm2 = _m_psrlqi(mm3,32);
  mm2 = _m_paddw(mm2,mm3);

  mm2 = _m_pmaddwd(mm2,mm2);

  temp2 = _m_to_int(mm2);

  temp2/=(length*length);

  temp2<<=(2*shift_DC);
#ifdef MAIN
  printf("E x^2 = %d\n",temp);  
#endif
  temp -= temp2;
#ifdef MAIN
  printf("(E x)^2=%d\n",temp2);
#endif
  _mm_empty();
  _m_empty();



  return((temp>0)?temp:1);
}

int32_t signal_energy_nodc(int32_t *input,uint32_t length) {

  int32_t i;
  int32_t temp;
  register __m64 mm0,mm1,mm2,mm3;
  __m64 *in = (__m64 *)input;

#ifdef MAIN
  int16_t *printb;
#endif

  mm0 = _m_pxor(mm0,mm0);
  mm3 = _m_pxor(mm3,mm3);

  for (i=0;i<length>>1;i++) {
    
    mm1 = in[i]; 
    mm2 = mm1;
    mm1 = _m_pmaddwd(mm1,mm1);// SIMD complex multiplication
    mm1 = _m_psradi(mm1,shift);
    mm0 = _m_paddd(mm0,mm1);
    //    temp2 = mm0;
    //    printf("%d %d\n",((int *)&in[i])[0],((int *)&in[i])[1]);


    //    printb = (int16_t *)&mm2;
    //    printf("mm2 %d : %d %d %d %d\n",i,printb[0],printb[1],printb[2],printb[3]);


  }

  /*
#ifdef MAIN
  printb = (int16_t *)&mm3;
  printf("%d %d %d %d\n",printb[0],printb[1],printb[2],printb[3]);
#endif
  */
  mm1 = mm0;

  mm0 = _m_psrlqi(mm0,32);

  mm0 = _m_paddd(mm0,mm1);

  temp = _m_to_int(mm0);

  temp/=length;
  temp<<=shift;   // this is the average of x^2

#ifdef MAIN
  printf("E x^2 = %d\n",temp);  
#endif
  _mm_empty();
  _m_empty();



  return((temp>0)?temp:1);
}

double signal_energy_fp(double **s_re,double **s_im,uint32_t nb_antennas,uint32_t length,uint32_t offset) {

  int32_t aa,i;
  double V=0.0;

  for (i=0;i<length;i++) {
    for (aa=0;aa<nb_antennas;aa++) {
      V= V + (s_re[aa][i+offset]*s_re[aa][i+offset]) + (s_im[aa][i+offset]*s_im[aa][i+offset]); 
    }
  }
  return(V/length/nb_antennas);
}

double signal_energy_fp2(struct complex *s,uint32_t length) {

  int32_t i;
  double V=0.0;

  for (i=0;i<length;i++) {
    //    printf("signal_energy_fp2 : %f,%f => %f\n",s[i].x,s[i].y,V);
    //      V= V + (s[i].y*s[i].x) + (s[i].y*s[i].x); 
      V= V + (s[i].x*s[i].x) + (s[i].y*s[i].y); 
    }
  return(V/length);
}
#else

int32_t signal_energy(int32_t *input,uint32_t length) {
}

#endif

#ifdef MAIN
#define LENGTH 256
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
main(int argc,char **argv) {

  int input[LENGTH];
  int energy=0,dc_r=0,dc_i=0;
  int16_t s=1,i;
  int amp;

  amp = atoi(argv[1]);// arguments to integer
  if (argc>1)
    printf("Amp = %d\n",amp);

  for (i=0;i<LENGTH;i++) {
    s = -s;
    ((int16_t*)input)[2*i]     = 31 + (int16_t)(amp*sin(2*M_PI*i/LENGTH));
    ((int16_t*)input)[1+(2*i)] = 30 + (int16_t)(amp*cos(2*M_PI*i/LENGTH));
    energy += (((int16_t*)input)[2*i]*((int16_t*)input)[2*i]) + (((int16_t*)input)[1+(2*i)]*((int16_t*)input)[1+(2*i)]);
    dc_r += ((int16_t*)input)[2*i];
    dc_i += ((int16_t*)input)[1+(2*i)];


  }
  energy/=LENGTH;
  dc_r/=LENGTH;
  dc_i/=LENGTH;

  printf("signal_energy = %d dB(%d,%d,%d,%d)\n",dB_fixed(signal_energy(input,LENGTH)),signal_energy(input,LENGTH),energy-(dc_r*dc_r+dc_i*dc_i),energy,(dc_r*dc_r+dc_i*dc_i));
  printf("dc = (%d,%d)\n",dc_r,dc_i);
}
#endif

