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
#include "emmintrin.h"
#include "smmintrin.h"
#include "defs.h"

#ifndef EXPRESSMIMO_TARGET
static  __m128i alpha_128 __attribute__ ((aligned(16))); 
static  __m128i shift     __attribute__ ((aligned(16)));


void multadd_complex_vector_real_scalar(int16_t *x,
					int16_t alpha,
					int16_t *y,
					uint8_t zero_flag,
					uint32_t N) {

  __m128i alpha_128,*x_128=(__m128i*)x,*y_128=(__m128i*)y;
  int n;

  alpha_128 = _mm_set1_epi16(alpha);

  if (zero_flag == 1)
    for (n=0;n<N>>2;n++) {
      y_128[n] = _mm_slli_epi16(_mm_mulhi_epi16(x_128[n],alpha_128),1);
    }
  
  else
    for (n=0;n<N>>2;n++) {
      y_128[n] = _mm_adds_epi16(y_128[n],_mm_slli_epi16(_mm_mulhi_epi16(x_128[n],alpha_128),1));
    }

  _mm_empty();
  _m_empty();

}
void multadd_real_vector_complex_scalar(int16_t *x,
					int16_t *alpha,
					int16_t *y,
					uint32_t N) {

  uint32_t i;

  // do 8 multiplications at a time
  __m128i alpha_r_128,alpha_i_128,yr,yi,*x_128=(__m128i*)x,*y_128=(__m128i*)y;
  int j;


  //  printf("alpha = %d,%d\n",alpha[0],alpha[1]);
  alpha_r_128 = _mm_set_epi16(alpha[0],alpha[0],alpha[0],alpha[0],alpha[0],alpha[0],alpha[0],alpha[0]);
  alpha_i_128 = _mm_set_epi16(alpha[1],alpha[1],alpha[1],alpha[1],alpha[1],alpha[1],alpha[1],alpha[1]);


  j=0;
  for (i=0;i<N>>3;i++) {

    yr     = _mm_slli_epi16(_mm_mulhi_epi16(alpha_r_128,x_128[i]),2);
    yi     = _mm_slli_epi16(_mm_mulhi_epi16(alpha_i_128,x_128[i]),2);
 
    //    print_shorts("yr",&yr);
    //    print_shorts("yi",&yi);

    y_128[j]   = _mm_adds_epi16(y_128[j],_mm_unpacklo_epi16(yr,yi));
    //    print_shorts("y",&y_128[j]);
    j++;
    y_128[j]   = _mm_adds_epi16(y_128[j],_mm_unpackhi_epi16(yr,yi));
    //    print_shorts("y",&y_128[j]);
    j++;

  }

  _mm_empty();
  _m_empty();

}

int rotate_cpx_vector(int16_t *x, 
		      int16_t *alpha, 
		      int16_t *y, 
		      uint32_t N, 
		      uint16_t output_shift,
		      uint8_t format)
{
  // Multiply elementwise two complex vectors of N elements
  // x        - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dynamic of 15 bit maximum
  //
  // alpha      - input 2    in the format  |Re0 Im0|
  //            We assume x2 with a dynamic of 15 bit maximum
  //
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // output_shift - shift at output to return in Q1.15
  // format - 0 means alpha is in shuffled format, 1 means x is in shuffled format            

  uint32_t i;                 // loop counter

  register __m128i m0,m1;

  //    short *temps;
  //    int *tempd;



  __m128i *x_128; 
  __m128i *y_128; 
  //  __m128i temp;
  
  /*  
  msg("rotate_cpx_vector: %x,%x,%x,%d,%d\n",
      x, 
      alpha, 
      y, 
      N, 
      output_shift);
  */


  shift = _mm_cvtsi32_si128(output_shift);
  x_128 = (__m128i *)&x[0];

  if (format==0) {  // alpha is in shuffled format for complex multiply
    ((int16_t *)&alpha_128)[0] = alpha[0];
    ((int16_t *)&alpha_128)[1] = -alpha[1];
    ((int16_t *)&alpha_128)[2] = alpha[1];
    ((int16_t *)&alpha_128)[3] = alpha[0];
    ((int16_t *)&alpha_128)[4] = alpha[0];
    ((int16_t *)&alpha_128)[5] = -alpha[1];
    ((int16_t *)&alpha_128)[6] = alpha[1];
    ((int16_t *)&alpha_128)[7] = alpha[0];
  }
  else { // input is in shuffled format for complex multiply
    ((int16_t *)&alpha_128)[0] = alpha[0];
    ((int16_t *)&alpha_128)[1] = alpha[1];
    ((int16_t *)&alpha_128)[2] = alpha[0];
    ((int16_t *)&alpha_128)[3] = alpha[1];
    ((int16_t *)&alpha_128)[4] = alpha[0];
    ((int16_t *)&alpha_128)[5] = alpha[1];
    ((int16_t *)&alpha_128)[6] = alpha[0];
    ((int16_t *)&alpha_128)[7] = alpha[1];
  }
  y_128 = (__m128i *)&y[0];

  //  _mm_empty();
  //  return(0);

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
    //    printf("i=%d\n",i);
    /*
        temps = (short *)x_128;
        printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
        temps = (int16_t *)&alpha_128;
        printf("alpha : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    */

    m0 = _mm_madd_epi16(x_128[0],alpha_128); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    /*
        temp = m0;

        tempd = &temp;
        printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    */
    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude
    /*
        temp = m0;

        tempd = (int *)&temp;
        printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    */

    m1=m0;
    m0 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    y_128[0] = _mm_unpacklo_epi32(m0,m0);        // 1- pack in a 128 bit register [re im re im]

    //    temps = (int16_t *)&y_128[0];
    //    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);



    m0 = _mm_madd_epi16(x_128[1],alpha_128); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x_128[2],alpha_128); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[2] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x_128[3],alpha_128); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[3] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]


    if (format==1) {  // Put output in proper format (Re,-Im,Im,Re), shuffle = (0,1,3,2) = 0x1e

      //      print_shorts(y_128[0],"y_128[0]=");
      y_128[0] = _mm_shufflelo_epi16(y_128[0],0x1e);
      y_128[0] = _mm_shufflehi_epi16(y_128[0],0x1e);
      ((int16_t*)&y_128[0])[1] = -((int16_t*)&y_128[0])[1];
      ((int16_t*)&y_128[0])[5] = -((int16_t*)&y_128[0])[5];
      //      print_shorts(y_128[0],"y_128[0]="); 

      //      print_shorts(y_128[1],"y_128[1]="); 
      y_128[1] = _mm_shufflelo_epi16(y_128[1],0x1e);
      y_128[1] = _mm_shufflehi_epi16(y_128[1],0x1e);
      ((int16_t*)&y_128[1])[1] = -((int16_t*)&y_128[1])[1];
      ((int16_t*)&y_128[1])[5] = -((int16_t*)&y_128[1])[5];
      //      print_shorts(y_128[1],"y_128[1]="); 

      //      print_shorts(y_128[2],"y_128[2]="); 
      y_128[2] = _mm_shufflelo_epi16(y_128[2],0x1e);
      y_128[2] = _mm_shufflehi_epi16(y_128[2],0x1e);
      ((int16_t*)&y_128[2])[1] = -((int16_t*)&y_128[2])[1];
      ((int16_t*)&y_128[2])[5] = -((int16_t*)&y_128[2])[5];
      //      print_shorts(y_128[2],"y_128[2]="); 

      //      print_shorts(y_128[3],"y_128[3]="); 
      y_128[3] = _mm_shufflelo_epi16(y_128[3],0x1e);
      y_128[3] = _mm_shufflehi_epi16(y_128[3],0x1e);
      ((int16_t*)&y_128[3])[1] = -((int16_t*)&y_128[3])[1];
      ((int16_t*)&y_128[3])[5] = -((int16_t*)&y_128[3])[5];
      //      print_shorts(y_128[3],"y_128[3]="); 

    }

 
    x_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}

int rotate_cpx_vector2(int16_t *x, 
                       int16_t *alpha, 
                       int16_t *y, 
                       uint32_t N, 
                       uint16_t output_shift,
		       uint8_t format)
{
  // Multiply elementwise two complex vectors of N elements
  // x        - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dynamic of 15 bit maximum
  //
  // alpha      - input 2    in the format  |Re0 Im0|
  //            We assume x2 with a dynamic of 15 bit maximum
  //
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  register __m128i m0,m1;


  __m128i *x_128; 
  __m128i *y_128; 

  
    /*      
  printf("rotate_cpx_vector2: %x,%x,%x,%d,%d\n",
      x, 
      alpha, 
      y, 
      N, 
      output_shift);
    */


  shift = _mm_cvtsi32_si128(output_shift);
  x_128 = (__m128i *)&x[0];

  if (format==0) {  // alpha is in shuffled format for complex multiply
    ((int16_t *)&alpha_128)[0] = alpha[0];
    ((int16_t *)&alpha_128)[1] = -alpha[1];
    ((int16_t *)&alpha_128)[2] = alpha[1];
    ((int16_t *)&alpha_128)[3] = alpha[0];
    ((int16_t *)&alpha_128)[4] = alpha[0];
    ((int16_t *)&alpha_128)[5] = -alpha[1];
    ((int16_t *)&alpha_128)[6] = alpha[1];
    ((int16_t *)&alpha_128)[7] = alpha[0];
  }
  else { // input is in shuffled format for complex multiply
    ((int16_t *)&alpha_128)[0] = alpha[0];
    ((int16_t *)&alpha_128)[1] = alpha[1];
    ((int16_t *)&alpha_128)[2] = alpha[0];
    ((int16_t *)&alpha_128)[3] = alpha[1];
    ((int16_t *)&alpha_128)[4] = alpha[0];
    ((int16_t *)&alpha_128)[5] = alpha[1];
    ((int16_t *)&alpha_128)[6] = alpha[0];
    ((int16_t *)&alpha_128)[7] = alpha[1];
  }

  y_128 = (__m128i *)&y[0];

  //  _mm_empty();
  //  return(0);

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>1);i++)
  {
    

    //        temps = (short *)&x_128[i];
    //        printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    //        temps = (short *)&alpha_128;
    //        printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);

    m0 = _mm_madd_epi16(x_128[i],alpha_128); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //        temp = m0;

    //        tempd = &temp;
    //        printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    //        temp = m0;

    //        tempd = (int *)&temp;
    //        printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m1=m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]



    y_128[i] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]


    if (format==1) {  // Put output in proper format (Re,-Im,Im,Re), shuffle = (0,1,3,2) = 0x1e

      //      print_shorts(y_128[0],"y_128[0]=");
      y_128[i] = _mm_shufflelo_epi16(y_128[i],0x1e);
      y_128[i] = _mm_shufflehi_epi16(y_128[i],0x1e);
      ((int16_t*)&y_128[i])[1] = -((int16_t*)&y_128[i])[1];
      ((int16_t*)&y_128[i])[5] = -((int16_t*)&y_128[i])[5];
      //      print_shorts(y_128[0],"y_128[0]="); 

    }

  }


  _mm_empty();
  _m_empty();


  return(0);
}

int rotate_cpx_vector_norep(int16_t *x, 
			    int16_t *alpha, 
			    int16_t *y, 
			    uint32_t N, 
			    uint16_t output_shift)
{
  // Multiply elementwise two complex vectors of N elements
  // x        - input 1    in the format  |Re0  Im0 |,......,|Re(N-1) Im(N-1)|
  //            We assume x1 with a dynamic of 15 bit maximum
  //
  // alpha      - input 2    in the format  |Re0 Im0|
  //            We assume x2 with a dynamic of 15 bit maximum
  //
  // y        - output     in the format  |Re0  Im0|,......,|Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  register __m128i m0,m1,m2,m3;

  //  int16_t *temps;
  //  int *tempd;

  int *xd;
  //__m128i *x_128; 
  __m128i *y_128; 
  //  __m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  xd = (int *) x;
  y_128 = (__m128i *) y;

  ((int16_t *)&alpha_128)[0] = alpha[0];
  ((int16_t *)&alpha_128)[1] = -alpha[1];
  ((int16_t *)&alpha_128)[2] = alpha[1];
  ((int16_t *)&alpha_128)[3] = alpha[0];
  ((int16_t *)&alpha_128)[4] = alpha[0];
  ((int16_t *)&alpha_128)[5] = -alpha[1];
  ((int16_t *)&alpha_128)[6] = alpha[1];
  ((int16_t *)&alpha_128)[7] = alpha[0];

  //  _mm_empty();
  //  return(0);

  for(i=0;i<N>>2;i++)
  {

    m0 = _mm_setr_epi32(xd[0],xd[0],xd[1],xd[1]);
    m1 = _mm_setr_epi32(xd[2],xd[2],xd[3],xd[3]);
    
    //    printf("i=%d\n",i);
    //    temps = (short *)x1_128;
    //    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    //    temps = (short *)x2_128;
    //    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);

    m2 = _mm_madd_epi16(m0,alpha_128); //complex multiply. result is 32bit [Re Im Re Im]
    m3 = _mm_madd_epi16(m1,alpha_128); //complex multiply. result is 32bit [Re Im Re Im]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m2 = _mm_sra_epi32(m2,shift);        // shift right by shift in order to  compensate for the input amplitude
    m3 = _mm_sra_epi32(m3,shift);        // shift right by shift in order to  compensate for the input amplitude

    //    temp = m0;

    //    tempd = (int *)&temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);


    y_128[0] = _mm_packs_epi32(m2,m3);        // pack in 16bit integers with saturation [re im re im re im re im]

    //    temps = (short *)&y_128[0];
    //    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);

 
    xd+=4;
    y_128+=1;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


int mult_vector32_scalar(int16_t *x1, 
			 int x2, 
			 int16_t *y, 
			 uint32_t N)

{
  // Multiply elementwise two real vectors of N elements
  // x1       - input 1    in the format  |Re(0)  xxx  Re(1) xxx|,......,|Re(N-2) xxx Re(N-1) xxx|
  //            We assume x1 with a dinamic of 31 bit maximum
  //
  // x1       - input 2    
  //
  // y        - output     in the format  |Re0 (64bit) |,......,|Re(N-1) (64bit)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //

  uint32_t i;                 // loop counter

  /*
#ifdef USER_MODE
  char *tempc;
  short *temps;
  int *tempd;
  long long *templ;
  __m128i temp;
#endif
  */

  __m128i *x1_128; 
  __m128i x2_128; 
  __m128i *y_128; 


  x1_128 = (__m128i *)&x1[0];
  x2_128 = _mm_setr_epi32(x2,0,x2,0);
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    y_128[0] = _mm_mul_epu32(x1_128[0],x2_128);

    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    tempd = (int *)x1_128;
    printf("x1 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    tempd = (int *)&x2_128;
    printf("x2 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    //    tempd = (int *)y_128;
    //    printf("y : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)y_128;
    printf("y : %lld,%lld\n",templ[0],templ[1]);
#endif
    */
    y_128[1] = _mm_mul_epu32(x1_128[1],x2_128);
    y_128[2] = _mm_mul_epu32(x1_128[2],x2_128);
    y_128[3] = _mm_mul_epu32(x1_128[3],x2_128);

 
    x1_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


int complex_conjugate(int16_t *x1, 
		      int16_t *y, 
		      uint32_t N)

{
  uint32_t i;                 // loop counter

  /*
#ifdef USER_MODE
  char *tempc;
  short *temps;
  int *tempd;
  long long *templ;
  __m128i temp;
#endif
  */

  __m128i *x1_128; 
  __m128i x2_128; 
  __m128i *y_128; 


  x1_128 = (__m128i *)&x1[0];
  x2_128 = _mm_set_epi16(-1,1,-1,1,-1,1,-1,1);
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    y_128[0] = _mm_mullo_epi16(x1_128[0],x2_128);

    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    tempd = (int *)x1_128;
    printf("x1 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    tempd = (int *)&x2_128;
    printf("x2 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    //    tempd = (int *)y_128;
    //    printf("y : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)y_128;
    printf("y : %lld,%lld\n",templ[0],templ[1]);
#endif
    */
    y_128[1] = _mm_mullo_epi16(x1_128[1],x2_128);
    y_128[2] = _mm_mullo_epi16(x1_128[2],x2_128);
    y_128[3] = _mm_mullo_epi16(x1_128[3],x2_128);

 
    x1_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


#ifdef MAIN
#define L 8

main () {

  int16_t input[256] __attribute__((aligned(16)));
  int16_t input2[256] __attribute__((aligned(16)));
  int16_t output[256] __attribute__((aligned(16)));
  int16_t alpha[2];

  int i;
 
  input[0] = 100;
  input[1] = 200;
  input[2] = -200;
  input[3] = 100;
  input[4] = 1000;
  input[5] = 2000;
  input[6] = -2000;
  input[7] = 1000;
  input[8] = 100;
  input[9] = 200;
  input[10] = -200;
  input[11] = 100;
  input[12] = 1000;
  input[13] = 2000;
  input[14] = -2000;
  input[15] = 1000;
  
  input2[0] = 1;
  input2[1] = 2;
  input2[2] = 1;
  input2[3] = 2;
  input2[4] = 10;
  input2[5] = 20;
  input2[6] = 10;
  input2[7] = 20;
  input2[8] = 1;
  input2[9] = 2;
  input2[10] = 1;
  input2[11] = 2;
  input2[12] = 1000;
  input2[13] = 2000;
  input2[14] = 1000;
  input2[15] = 2000;
 
  alpha[0]=32767;
  alpha[1]=0; 
  
  //mult_cpx_vector(input,input2,output,L,0);
  rotate_cpx_vector_norep(input,alpha,input,L,15);
  
  for(i=0;i<L<<1;i++)
    printf("output[i]=%d\n",input[i]);
  
}

#endif //MAIN


#else //EXPRESSMIMO_TARGET

int rotate_cpx_vector(int16_t *x, 
		      int16_t *alpha, 
		      int16_t *y, 
		      uint32_t N, 
		      uint16_t output_shift,
		      uint8_t format)
{

}
#endif //EXPRESSMIMO_TARGET
