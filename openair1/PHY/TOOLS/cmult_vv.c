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
//#include "MAC_INTERFACE/extern.h"
#ifdef USER_MODE
#include <stdio.h>
#endif

#ifndef EXPRESSMIMO_TARGET
static  __m128i shift     __attribute__ ((aligned(16)));
static  __m128i m0,m1,m2,m4     __attribute__ ((aligned(16)));

//#define DEBUG_CMULT

int mult_cpx_vector(int16_t *x1, 
		    int16_t *x2, 
		    int16_t *y, 
		    uint32_t N, 
		    int output_shift)
{
  // Multiply elementwise two complex vectors of N elements with repeated formatted output
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  /*
#ifdef USER_MODE
  int16_t *temps;
  int *tempd;
#endif
  */
  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  //  __m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
    //msg("mult_cpx_vector: iteration %d, x1=%p, x2=%p, y=%p\n",i,x1_128,x2_128,y_128);
    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */

    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    //    temp = m0;

    //    tempd = (int *)&temp;
    //  printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    y_128[0] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    /*
#ifdef USER_MODE
    temps = (int16_t *)&y_128[0];
    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif USER_MODE
    */

    m0 = _mm_madd_epi16(x1_128[1],x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x1_128[2],x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[2] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x1_128[3],x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[3] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]




 
    x1_128+=4;
    x2_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}

int mult_cpx_vector_unprepared(int16_t *x1, 
			       int16_t *x2, 
			       int16_t *y, 
			       uint32_t N, 
			       int output_shift)
{
  // Multiply elementwise two complex vectors of N elements with repeated formatted output
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 Im0 Re0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0 Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

#ifdef DEBUG_CMULT
  int16_t *temps;
  int *tempd;
#endif
  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  __m128i shuf_x2;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
    //msg("mult_cpx_vector: iteration %d, x1=%p, x2=%p, y=%p\n",i,x1_128,x2_128,y_128);
    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */
    
    shuf_x2 = _mm_shufflelo_epi16(x2_128[0],_MM_SHUFFLE(2,3,0,1));
    shuf_x2 = _mm_shufflehi_epi16(shuf_x2,_MM_SHUFFLE(2,3,0,1));

#ifdef DEBUG_CMULT
    
    tempd = &shuf_x2;
    printf("shuf_x2 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    
    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    //    temp = m0;

    //    tempd = (int *)&temp;
    //  printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    y_128[0] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    /*
#ifdef USER_MODE
    temps = (int16_t *)&y_128[0];
    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif USER_MODE
    */

    m0 = _mm_madd_epi16(x1_128[1],x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x1_128[2],x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[2] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x1_128[3],x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[3] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]




 
    x1_128+=4;
    x2_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}
 
//__attribute__ ((force_align_arg_pointer)) 
int mult_cpx_vector_norep(int16_t *x1, 
			   int16_t *x2, 
			   int16_t *y, 
			   uint32_t N, 
			   int output_shift)
{
  // Multiply elementwise two complex vectors of N elements with normal formatted output
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy). WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1,m2,m3;

       
#ifdef DEBUG_CMULT
  __m128i temp;
  int *tempd;
  int16_t *temps;
#endif //DEBUG_CMULT
  

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  //__m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];

#ifndef USER_MODE
  //debug_msg("mult_cpx_vector_norep: x1 %p, x2 %p, y %p, shift %d\n",x1,x2,y,output_shift);
#endif

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
        
#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    

    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    
#ifdef DEBUG_CMULT
    temp = m0;
    
    tempd = &temp;
    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

        
#ifdef DEBUG_CMULT
    temp = m0;

    tempd = (int *)&temp;
    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m1 = m0;

    

    m0 = _mm_madd_epi16(x1_128[1],x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[1];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[1];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif

#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[1] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    
    


    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[1] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    


    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    //    print_shorts(m2,"m2");

    y_128[0] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]



    m0 = _mm_madd_epi16(x1_128[2],x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    
#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[2];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[2];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif //DEBUG_CMULT

#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[2] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[2] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m1 = m0;
    //    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]

    m0 = _mm_madd_epi16(x1_128[3],x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
   
#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[3];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[3];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
 
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[3] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[3] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]




 
    x1_128+=4;
    x2_128+=4;
    y_128 +=2;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


int mult_cpx_vector_norep_unprepared_conjx2(int16_t *x1, 
					    int16_t *x2, 
					    int16_t *y, 
					    uint32_t N, 
					    int output_shift)
{
  // Multiply elementwise two complex vectors of N elements with normal formatted output, conjugate x1
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 Im0 Re0 Im0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy). WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1,m2,m3;

  short conj_x2s[8] __attribute__((aligned(16))) = {1,1,-1,1,1,1,-1,1};
  __m128i *conj_x2 = (__m128i *)&conj_x2s[0];

#ifdef DEBUG_CMULT
  __m128i temp;
  int *tempd;
  int16_t *temps;
#endif //DEBUG_CMULT
  

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  __m128i shuf_x2;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
        
#ifdef DEBUG_CMULT
    printf("**i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    

    shuf_x2 = _mm_shufflelo_epi16(x2_128[0],_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_shufflehi_epi16(shuf_x2,_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_sign_epi16(shuf_x2,*conj_x2);
#ifdef DEBUG_CMULT
    
    temps = &shuf_x2;
    printf("shuf_x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif //DEBUG_CMULT

    m0 = _mm_madd_epi16(x1_128[0],shuf_x2); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    
#ifdef DEBUG_CMULT
    temp = m0;
    
    tempd = &temp;
    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

        
#ifdef DEBUG_CMULT
    temp = m0;

    tempd = (int *)&temp;
    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m1 = m0;

    

    shuf_x2 = _mm_shufflelo_epi16(x2_128[1],_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_shufflehi_epi16(shuf_x2,_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_sign_epi16(shuf_x2,*conj_x2);
    m0 = _mm_madd_epi16(x1_128[1],shuf_x2); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[1];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[1];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif

#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[1] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    
    


    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[1] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    


    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    //    print_shorts(m2,"m2");

    y_128[0] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]


    shuf_x2 = _mm_shufflelo_epi16(x2_128[2],_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_shufflehi_epi16(shuf_x2,_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_sign_epi16(shuf_x2,*conj_x2);
    m0 = _mm_madd_epi16(x1_128[2],shuf_x2); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    
#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[2];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[2];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif //DEBUG_CMULT

#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[2] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[2] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m1 = m0;
    //    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    shuf_x2 = _mm_shufflelo_epi16(x2_128[3],_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_shufflehi_epi16(shuf_x2,_MM_SHUFFLE(2,3,1,0));
    shuf_x2 = _mm_sign_epi16(shuf_x2,*conj_x2);

    m0 = _mm_madd_epi16(x1_128[3],shuf_x2); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
   
#ifdef DEBUG_CMULT
    printf("i=%d\n",i);
    temps = (int16_t *)&x1_128[3];
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)&x2_128[3];
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
 
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[3] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
#ifdef DEBUG_CMULT
    temp = m0;
    tempd = (int *)&temp;
    printf("m0[3] : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif //DEBUG_CMULT
    

    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]




 
    x1_128+=4;
    x2_128+=4;
    y_128 +=2;
  }


  _mm_empty();
  _m_empty();

  return(0);
}

static __m128i norep_tmp32 __attribute__ ((aligned(16)));

//__attribute__ ((force_align_arg_pointer)) 
int mult_cpx_vector_norep2(int16_t *x1, 
			   int16_t *x2, 
			   int16_t *y, 
			   uint32_t N, 
			   int output_shift) 
{
  // Multiply elementwise two complex vectors of N elements with normal formatted output and no loop unrollin
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0 Re1 Im1 Re1 Im1|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy). WARNING: N>=2;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1,m2,m3;

  /*
#ifdef USER_MODE
  __m128i temp;
  int *tempd;
  int16_t *temps;
#endif
  */

  __m128i *x1_128; 
  __m128i *x2_128; 
  int     *y_32 = (int*)y; 

  //  __m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];

#ifndef USER_MODE
  //debug_msg("mult_cpx_vector_norep2: x1 %p, x2 %p, y %p, shift %d, N %d\n",x1,x2,y,output_shift,N);
#endif

  // we compute 2 cpx multiply for each loop
  for(i=0;i<(N>>1);i++)
  {
    /*    
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */

    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

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

    norep_tmp32 = _mm_packs_epi32(m0,m0);        // Re0 Im0 Re1 Im1 Re0 Im0 Re1 Im1

    /*
#ifdef USER_MODE
    printf("tmp : %d,%d,%d,%d\n",((int16_t *)&tmp32)[0],((int16_t *)&tmp32)[1],((int16_t *)&tmp32)[2],((int16_t *)&tmp32)[3]);
#endif
    */

    y_32[0] = ((int *)&norep_tmp32)[0];        // 1- pack in a 128 bit register [re im re im]
    y_32[1] = ((int *)&norep_tmp32)[1];        // 1- pack in a 128 bit register [re im re im]

    x1_128+=1;
    x2_128+=1;
    y_32 +=2;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


int mult_cpx_vector_norep_conj(int16_t *x1, 
			       int16_t *x2, 
			       int16_t *y, 
			       uint32_t N, 
			       int output_shift)
{
  // Multiply elementwise two complex vectors of N elements after conjugating and shuffling x1
  // x1       - input 1    in the format  |Re0  -Im0 Im0 Re0|,......,|Re(N-1)  -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x1 with a dynamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dynamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy). WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1,m2,m4;

  /*    
#ifdef USER_MODE
  int16_t *temps;
  int *tempw;
#endif
  */

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  //  __m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];

  //  printf("mult_cpx_vector_norep: shift %d\n",output_shift);

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    
    /*    
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */

    m4 = _mm_shufflelo_epi16(x1_128[0],_MM_SHUFFLE(1,0,1,0));
    m4 = _mm_shufflehi_epi16(m4,_MM_SHUFFLE(1,0,1,0));
    /*
    temps = (int16_t *)&m4;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    */
    m0 = _mm_madd_epi16(m4,x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    /*
    tempw = (int *)&m0;
    printf("m0[0] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    /*
    tempw = (int *)&m0;
    printf("m0[0] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */
    //    temp = m0;

    //    tempd = (int *)&temp;
    //  printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m1 = m0;

    

    m4 = _mm_shufflelo_epi16(x1_128[1],_MM_SHUFFLE(1,0,1,0));
    m4 = _mm_shufflehi_epi16(m4,_MM_SHUFFLE(1,0,1,0));
    m0 = _mm_madd_epi16(m4,x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    /*
    tempw = (int *)&m0;
    printf("m0[1] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */


    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    /*
    tempw = (int *)&m0;
    printf("m0[1] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */


    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    //    print_shorts(m2,"m2");

    y_128[0] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]



    m4 = _mm_shufflelo_epi16(x1_128[2],_MM_SHUFFLE(1,0,1,0));
    m4 = _mm_shufflehi_epi16(m4,_MM_SHUFFLE(1,0,1,0));
    m0 = _mm_madd_epi16(m4,x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    /*
    tempw = (int *)&m0;
    printf("m0[2] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    /*
    tempw = (int *)&m0;
    printf("m0[2] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */

    m1 = m0;
    //    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]


    m4 = _mm_shufflelo_epi16(x1_128[3],_MM_SHUFFLE(1,0,1,0));
    m4 = _mm_shufflehi_epi16(m4,_MM_SHUFFLE(1,0,1,0));
    m0 = _mm_madd_epi16(m4,x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    /*
    tempw = (int *)&m0;
    printf("m0[3] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    /*
    tempw = (int *)&m0;
    printf("m0[3] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    */

    m2 = m0;
    //    m2 = _mm_packs_epi32(m2,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_packs_epi32(m1,m2);        // 1- pack in a 128 bit register [re im re im]




 
    x1_128+=4;
    x2_128+=4;
    y_128 +=2;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


int mult_cpx_vector_norep_conj2(int16_t *x1, 
				int16_t *x2, 
				int16_t *y, 
				uint32_t N, 
				int output_shift)
{
  // Multiply elementwise two complex vectors of N elements after conjugating and shuffling x1
  // x1       - input 1    in the format  |Re0  -Im0 Im0 Re0|,......,|Re(N-1)  -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x1 with a dynamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dynamic of 14 bit maximum
  ///
  // y        - output     in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy). WARNING: N>=2;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1,m2,m4;
  __m128i tmp32;

      

//   int16_t *temps;
  int *tempw;

  

  __m128i *x1_128; 
  __m128i *x2_128; 
  int *y_32 = (int *)&y[0]; 

  //  __m128i temp,*tempd;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];


  //  printf("mult_cpx_vector_norep: shift %d\n",output_shift);

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>1);i++)
  {
    
/*
    //#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    //#endif
*/

    // conjuaget and shuffle x1
    m4 = _mm_shufflelo_epi16(x1_128[0],_MM_SHUFFLE(1,0,1,0));
    m4 = _mm_shufflehi_epi16(m4,_MM_SHUFFLE(1,0,1,0));
/*
    temps = (int16_t *)&m4;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
*/
    m0 = _mm_madd_epi16(m4,x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    /*
        temp = m0;

        tempd = &temp;
        printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    
    tempw = (int *)&m0;
    printf("m0[0] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    
    */
    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    
    tempw = (int *)&m0;
    //   printf("m0[1] : %d,%d,%d,%d\n",tempw[0],tempw[1],tempw[2],tempw[3]);
    



    tmp32 = _mm_packs_epi32(m0,m0);        // Re0 Im0 Re1 Im1 Re0 Im0 Re1 Im1



    //        printf("tmp : %d,%d,%d,%d\n",((int16_t *)&tmp32)[0],((int16_t *)&tmp32)[1],((int16_t *)&tmp32)[2],((int16_t *)&tmp32)[3]);

    y_32[0] = ((int *)&tmp32)[0];        // 1- pack in a 128 bit register [re im re im]
    y_32[1] = ((int *)&tmp32)[1];        // 1- pack in a 128 bit register [re im re im]



    x1_128+=1;
    x2_128+=1;
    y_32 +=2;


  }


  _mm_empty();
  _m_empty();

  return(0);
}


int mult_cpx_vector2(int16_t *x1, 
		     int16_t *x2, 
		     int16_t *y, 
		     uint32_t N, 
		     int output_shift)
{
  // Multiply elementwise two complex vectors of N elements
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  //
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N must be a multiple of 2;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1;

  /*
#ifdef USER_MODE
  int16_t *temps;
  int *tempd;
#endif
  */

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 

  //  __m128i temp;
  

  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];



  for(i=0;i<(N>>1);i++)
  {
    
/* #ifdef USER_MODE */
/*     printf("i=%d\n",i); */
/*     temps = (int16_t *)x1_128; */
/*     printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]); */
/*     temps = (int16_t *)x2_128; */
/*     printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]); */
/* #endif */

    m0 = _mm_madd_epi16(x1_128[i],x2_128[i]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    //    temp = m0;

    //    tempd = (int *)&temp;
    //  printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    y_128[i] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

  }


  _mm_empty();
  _m_empty();

  return(0);
}

int mult_cpx_vector_add(int16_t *x1, 
		    int16_t *x2, 
		    int16_t *y, 
		    uint32_t N, 
		    int output_shift)
{
  // Multiply elementwise two complex vectors of N elements and add it to y
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  //
  // y        - output     in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //
  // log2_amp - increase the output amplitude by a factor 2^log2_amp (default is 0)
  //            WARNING: log2_amp>0 can cause overflow!!

  uint32_t i;                 // loop counter

  //register __m128i m0,m1;

  /*
#ifdef USER_MODE
  int16_t *temps;
  int *tempd;
  __m128i temp;
#endif
  */

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 


  shift = _mm_cvtsi32_si128(output_shift);
  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    //unroll 0

    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */
    
    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    //    temp = m0;

    //    tempd = &temp;
    //    printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude

    //    temp = m0;

    //    tempd = (int *)&temp;
    //  printf("m0 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);


    m0 = _mm_packs_epi32(m0,m0);        // 1- pack in a 128 bit register [re im re im]
    m0 = _mm_unpacklo_epi32(m0,m0);        // 1- pack in a 128 bit register [re im re im]

    y_128[0] = _mm_add_epi16(m0,y_128[0]);


    //        temps = (int16_t *)&y_128[0];
    //    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);


    //unroll 1
    m0 = _mm_madd_epi16(x1_128[1],x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    m1 = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    y_128[1] = _mm_add_epi16(m1,y_128[1]);


//unroll 2
    m0 = _mm_madd_epi16(x1_128[2],x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    m1 = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    y_128[2] = _mm_add_epi16(m1,y_128[2]);



    //unroll 3
    m0 = _mm_madd_epi16(x1_128[3],x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]

    m0 = _mm_sra_epi32(m0,shift);        // 1- shift right by shift in order to  compensate for the input amplitude




    m1 = m0;
    m1 = _mm_packs_epi32(m1,m0);        // 1- pack in a 128 bit register [re im re im]
    m1 = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    y_128[3] = _mm_add_epi16(m1,y_128[3]);

 
    x1_128+=4;
    x2_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}

int mult_cpx_vector_add32(int16_t *x1, 
			  int16_t *x2, 
			  int16_t *y, 
			  uint32_t N)

{
  // Multiply elementwise two complex vectors of N elements and add it to y
  // x1       - input 1    in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
  //            We assume x1 with a dinamic of 15 bit maximum
  //
  // x2       - input 2    in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
  //            We assume x2 with a dinamic of 14 bit maximum
  //
  // y        - output     in the format  |Re0 (32bit)  Im0 (32bit) |,......,|Re(N-1) (32bit)  Im(N-1) (32bit)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //

  uint32_t i;                 // loop counter
  //register __m128i m0;

  /*
#ifdef USER_MODE
  int16_t *temps;
  int *tempd;
  __m128i temp;
#endif
  */

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 


  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    //unroll 0

    
    m0 = _mm_madd_epi16(x1_128[0],x2_128[0]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0
    y_128[0] = _mm_add_epi32(y_128[0],m0);

    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    tempd = (int *)y_128;
    printf("y : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
#endif
    */

    
    m0 = _mm_madd_epi16(x1_128[1],x2_128[1]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    y_128[1] = _mm_add_epi32(y_128[1],m0);

    m0 = _mm_madd_epi16(x1_128[2],x2_128[2]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    y_128[2] = _mm_add_epi32(y_128[2],m0);

    m0 = _mm_madd_epi16(x1_128[3],x2_128[3]); //pmaddwd_r2r(mm1,mm0);         // 1- compute x1[0]*x2[0]
    y_128[3] = _mm_add_epi32(y_128[3],m0);

 
    x1_128+=4;
    x2_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}

int mult_vector32(int16_t *x1, 
		  int16_t *x2, 
		  int16_t *y, 
		  uint32_t N)

{
  // Multiply elementwise two real vectors of N elements y = real(x1).*real(x2)
  // x1       - input 1    in the format  |Re(0)  xxx  Re(1) xxx|,......,|Re(N-2) xxx Re(N-1) xxx|
  //            We assume x1 with a dinamic of 31 bit maximum
  //
  // x1       - input 2    in the format  |Re(0)  xxx Re(2) xxx|,......,|Re(N-2)  xxx Re(N-1) xxx|
  //            We assume x2 with a dinamic of 31 bit maximum
  //
  // y        - output     in the format  |Re0 (64bit) |,......,|Re(N-1) (64bit)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //

  uint32_t i;                 // loop counter

  __m128i *x1_128; 
  __m128i *x2_128; 
  __m128i *y_128; 


  x1_128 = (__m128i *)&x1[0];
  x2_128 = (__m128i *)&x2[0];
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    y_128[0] = _mm_mul_epu32(x1_128[0],x2_128[0]);

    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    tempd = (int *)x1_128;
    printf("x1 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    tempd = (int *)x2_128;
    printf("x2 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    //    tempd = (int *)y_128;
    //    printf("y : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)y_128;
    printf("y : %lld,%lld\n",templ[0],templ[1]);
#endif
    */

    y_128[1] = _mm_mul_epu32(x1_128[1],x2_128[1]);
    y_128[2] = _mm_mul_epu32(x1_128[2],x2_128[2]);
    y_128[3] = _mm_mul_epu32(x1_128[3],x2_128[3]);

 
    x1_128+=4;
    x2_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}


/*
The following code does not work, because there is no signed 32bit multiplication intrinsic. It only works for unsigned values

int mult_cpx_vector32_conj(int16_t *x, 
			   int16_t *y, 
			   uint32_t N)

{
  // elementwise multiplication of two complex vectors of N elements such that y = x * conj(x) = real(x)*real(x)+imag(x)*imag(x)
  // x        - input      in the format  |Re(0)  Im(0) Re(1) Im(1) |,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //            We assume x with a dinamic of 31 bit maximum
  //
  // y        - output     in the format  |Re0 (64bit) |,......,|Re(N-1) (64bit)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //

  uint32_t i;                 // loop counter

#ifdef USER_MODE
  char *tempc;
  int16_t *temps;
  int *tempd;
  long long *templ;
  __m128i temp;
#endif

  __m128i *x_128; 
  __m128i *y_128; 

  __m128i m0,m1,m2,m3; 

  x_128 = (__m128i *)&x[0];
  y_128 = (__m128i *)&y[0];

  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    // Re(a)*Re(b)
    m0 = _mm_mul_epu32(x_128[0],x_128[0]);
    // Im(a)*Im(b)
    m1 = _mm_shuffle_epi32(x_128[0],_MM_SHUFFLE(2,3,0,1));
    m3 = _mm_mul_epu32(m1,m1);
    // Re(a)*Re(b)+Im(a)*Im(b)
    y_128[0] = _mm_add_epi64(m0,m3);

#ifdef USER_MODE
    printf("i=%d\n",i);
    tempd = (int *)x_128;
    printf("x : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)&m0;
    printf("m0 : %lld,%lld\n",templ[0],templ[1]);
    tempd = (int *)&m1;
    printf("m1 : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)&m3;
    printf("m3 : %lld,%lld\n",templ[0],templ[1]);
    //    tempd = (int *)y_128;
    //    printf("y : %d,%d,%d,%d\n",tempd[0],tempd[1],tempd[2],tempd[3]);
    templ = (long long *)y_128;
    printf("y : %lld,%lld\n",templ[0],templ[1]);
#endif

    // Re(a)*Re(b)
    m0 = _mm_mul_epu32(x_128[1],x_128[1]);
    // Im(a)*Im(b)
    m1 = _mm_shuffle_epi32(x_128[1],_MM_SHUFFLE(1,0,3,2));
    m3 = _mm_mul_epu32(m1,m1);
    // Re(a)*Re(b)+Im(a)*Im(b)
    y_128[1] = _mm_add_epi64(m0,m3);


    // Re(a)*Re(b)
    m0 = _mm_mul_epu32(x_128[2],x_128[2]);
    // Im(a)*Im(b)
    m1 = _mm_shuffle_epi32(x_128[2],_MM_SHUFFLE(1,0,3,2));
    m3 = _mm_mul_epu32(m1,m1);
    // Re(a)*Re(b)+Im(a)*Im(b)
    y_128[2] = _mm_add_epi64(m0,m3);


    // Re(a)*Re(b)
    m0 = _mm_mul_epu32(x_128[3],x_128[3]);
    // Im(a)*Im(b)
    m1 = _mm_shuffle_epi32(x_128[3],_MM_SHUFFLE(1,0,3,2));
    m3 = _mm_mul_epu32(m1,m1);
    // Re(a)*Re(b)+Im(a)*Im(b)
    y_128[3] = _mm_add_epi64(m0,m3);

 
    x_128+=4;
    y_128 +=4;
  }


  _mm_empty();
  _m_empty();

  return(0);
}
*/

int mult_cpx_vector32_conj(int16_t *x, 
			   int16_t *y, 
			   uint32_t N)

{
  // Elementwise multiplication of two complex vectors of N elements such that y = x * conj(x) = real(x)*real(x)+imag(x)*imag(x)
  // x        - input      in the format  |Re(0)  Im(0) Re(1) Im(1) |,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
  //                       We assume x1 with a dinamic of 31 bit maximum
  //
  // y        - output     in the format  |Re0 (64bit) |,......,|Re(N-1) (64bit)|
  //
  // N        - the size f the vectors (this function does N cpx mpy. WARNING: N>=4;
  //

  uint32_t i;                 // loop counter

  int *xi = (int*) x; 
  long long int *yl = (long long int*) y; 

  long long int temp1,temp2;


  for(i=0;i<N/2;i++)
  {
    // Re(a)*Re(b)
    temp1 = ((long long int) xi[0])* ((long long int) xi[0]);
    // Im(a)*Im(b)
    temp2 = ((long long int) xi[1])* ((long long int) xi[1]);
    yl[0] = temp1+temp2;

    temp1 = ((long long int) xi[2])* ((long long int) xi[2]);
    temp2 = ((long long int) xi[3])* ((long long int) xi[3]);
    yl[1] = temp1+temp2;

    /*    
#ifdef USER_MODE
    printf("i=%d\n",i);
    printf("x1 : %d,%d,%d,%d\n",x1i[0],x1i[1],x1i[2],x1i[3]);
    printf("x2 : %d,%d,%d,%d\n",x2i[0],x2i[1],x2i[2],x2i[3]);
    printf("temp : %lld,%lld\n",temp1,temp2);
    printf("y : %lld,%lld\n",yl[0],yl[1]);
#endif
    */

    xi+=4;
    yl +=2;
  }

  return(0);
}


int shift_and_pack(int16_t *y, 
		   uint32_t N, 
		   int output_shift)
{
  uint32_t i;                 // loop counter

  //register __m128i m0,m1;

  /*
#ifdef USER_MODE
  int16_t *temps;
  int *tempd;
  __m128i temp;
#endif
  */

  __m128i *y_128; 


  shift = _mm_cvtsi32_si128(output_shift);
  y_128 = (__m128i *)&y[0];


  // we compute 4 cpx multiply for each loop
  for(i=0;i<(N>>3);i++)
  {
    /*
#ifdef USER_MODE
    printf("i=%d\n",i);
    temps = (int16_t *)x1_128;
    printf("x1 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
    temps = (int16_t *)x2_128;
    printf("x2 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);
#endif
    */
    
    //unroll 0
    m0 = _mm_sra_epi32(y_128[0],shift);        // 1- shift right by shift in order to  compensate for the input amplitude
    m0 = _mm_packs_epi32(m0,m0);        // 1- pack in a 128 bit register [re im re im]
    y_128[0] = _mm_unpacklo_epi32(m0,m0);        // 1- pack in a 128 bit register [re im re im]

    //    temps = (int16_t *)&y_128[0];
    //    printf("y0 : %d,%d,%d,%d,%d,%d,%d,%d\n",temps[0],temps[1],temps[2],temps[3],temps[4],temps[5],temps[6],temps[7]);

    //unroll 1
    m1 = _mm_sra_epi32(y_128[1],shift);        // 1- shift right by shift in order to  compensate for the input amplitude
    m1 = _mm_packs_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]
    y_128[1] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    //unroll 2
    m1 = _mm_sra_epi32(y_128[2],shift);        // 1- shift right by shift in order to  compensate for the input amplitude
    m1 = _mm_packs_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]
    y_128[2] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    //unroll 3
    m1 = _mm_sra_epi32(y_128[3],shift);        // 1- shift right by shift in order to  compensate for the input amplitude
    m1 = _mm_packs_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]
    y_128[3] = _mm_unpacklo_epi32(m1,m1);        // 1- pack in a 128 bit register [re im re im]

    y_128 +=4;
  }

  _mm_empty();
  _m_empty();

  return(0);
}


#ifdef MAIN
#define L 16

main () {

int16_t input[256] __attribute__((aligned(16)));
int16_t input2[256] __attribute__((aligned(16)));
int16_t output[256] __attribute__((aligned(16)));

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

 
  mult_cpx_vector32_conj(input,output,8);


}

#endif //MAIN


#else  //EXPRESSMIMO_TARGET

/*
int mult_cpx_vector(int16_t *x1, 
		    int16_t *x2, 
		    int16_t *y, 
		    uint32_t N, 
		    uint16_t output_shift)
{

}
*/

#endif //EXPRESSMIMO_TARGET
