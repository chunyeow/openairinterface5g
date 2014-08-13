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
/* file: viterbi.c
   purpose: SIMD Optimized 802.11/802.16 Viterbi Decoder
   author: raymond.knopp@eurecom.fr
   date: 10.2004 
*/


#ifndef EXPRESSMIMO_TARGET
#include "emmintrin.h"
#endif //EXPRESSMIMO_TARGET

extern unsigned char ccodedot11_table[128],ccodedot11_table_rev[128];




static unsigned char inputs[64][2048];
static unsigned short survivors[64][2048];
static short partial_metrics[64],partial_metrics_new[64];

#ifdef __KERNEL__
#define printf rt_printk
#endif

#ifndef EXPRESSMIMO_TARGET

void phy_viterbi_dot11(char *y,unsigned char *decoded_bytes,unsigned short n) {

/*  y is a pointer to the input
    decoded_bytes is a pointer to the decoded output
    n is the size in bits of the coded block, with the tail */


  char *in = y;
  short m0,m1,w[4],max_metric;
  short position;
  unsigned short prev_state0,prev_state1,state;

  partial_metrics[0] = 0;

  for (state=1; state<64; state++)
    partial_metrics[state] = -127;

  for (position=0;position<n;position++) {

    //    printf("Channel Output %d = (%d,%d)\n",position,*in,*(in+1));

    //        printf("%d %d\n",in[0],in[1]);

    w[3] = in[0] + in[1];  // 1,1
    w[0] = -w[3];          // -1,-1
    w[1] = in[0] - in[1];  // -1, 1
    w[2] = -w[1];          // 1 ,-1
      
    max_metric = -127;
    //    printf("w: %d %d %d %d\n",w[0],w[1],w[2],w[3]);
    for (state=0; state<64 ; state++) {
       
      // input 0
      prev_state0 = (state<<1);
      m0 = partial_metrics[prev_state0%64] + w[ccodedot11_table[prev_state0]];
      /*
      if (position < 8)
	printf("%d,%d : prev_state0 = %d,m0 = %d,w=%d (%d)\n",position,state,prev_state0%64,m0,w[ccodedot11_table[prev_state0]],partial_metrics[prev_state0%64]);
      */
      // input 1
      prev_state1 = (1+ (state<<1));
      m1 = partial_metrics[prev_state1%64] + w[ccodedot11_table[prev_state1]]; 
      /*
      if (position <8)
	printf("%d,%d : prev_state1 = %d,m1 = %d,w=%d (%d)\n",position,state,prev_state1%64,m1,w[ccodedot11_table[prev_state1]],partial_metrics[prev_state0%64]);
      */
      if (m0>m1) {
        partial_metrics_new[state] = m0;
	survivors[state][position] = prev_state0%64;
	inputs[state][position] = (state>31) ? 1 : 0;
	if (m0>max_metric)
	  max_metric = m0;
      }
      else{
        partial_metrics_new[state] = m1;
	survivors[state][position] = prev_state1%64;
	inputs[state][position] = (state>31) ? 1 : 0;
	if (m1>max_metric)
	  max_metric = m1;
      }
	
    }
        
    for (state=0 ; state<64; state++) {

      partial_metrics[state] = partial_metrics_new[state]- max_metric;
      //      printf("%d partial_metrics[%d] = %d\n",position,state,partial_metrics[state]);
    }
    
    in+=2;
  }


  // Traceback
  prev_state0 = 0;
  for (position = n-1 ; position>-1; position--) {

    decoded_bytes[(position)>>3] += (inputs[prev_state0][position]<<(position%8));

    //    if (position%8==0)
    //      printf("%d\n",decoded_bytes[(position)>>3]);


    prev_state0 = survivors[prev_state0][position];

  }


}



static char m0_table[64*256] __attribute__ ((aligned(16)));
static char m1_table[64*256] __attribute__ ((aligned(16)));


// Set up Viterbi tables for SSE2 implementation
void phy_generate_viterbi_tables(void) {

  char w[4],in0,in1;
  unsigned char state,index0,index1;

  for (in0 = -8; in0 <8 ; in0++) {   // use 4-bit quantization
    for (in1 = -8; in1 <8 ;in1++) {

      w[3] = 16+ in0 + in1;  // 1,1
      w[0] = 16 - in0 - in1;          // -1,-1
      w[1] = 16+ in0 - in1;  // -1, 1
      w[2] = 16 - in0 + in1;          // 1 ,-1
      
      //    printf("w: %d %d %d %d\n",w[0],w[1],w[2],w[3]);
      for (state=0; state<64 ; state++) {
	
	// input 0
	index0 = (state<<1);
	m0_table[(in0+8 + (16*(in1+8)))*64 +state]  = w[ccodedot11_table_rev[index0]];
 
	
//	  if (position < 8)
//	  printf("%d,%d : prev_state0 = %d,m0 = %d,w=%d (%d)\n",position,state,prev_state0%64,m0,w[ccodedot11_table[prev_state0]],partial_metrics[prev_state0%64]);
	
	// input 1
	index1 = (1+ (state<<1));
	m1_table[(in0+8 + (16*(in1+8)))*64 +state] = w[ccodedot11_table_rev[index1]]; 

      }
    }
  }
}



#define INIT0 0x00000080
#define RESCALE 0x00000040


static  __m128i  __attribute__((aligned(16))) TB[4*4095*8];

static  __m128i metrics0_15,metrics16_31,metrics32_47,metrics48_63,even0_30a,even0_30b,even32_62a,even32_62b,odd1_31a,odd1_31b,odd33_63a,odd33_63b,TBeven0_30,TBeven32_62,TBodd1_31,TBodd33_63 __attribute__((aligned(16)));

static  __m128i rescale,min_state,min_state2 __attribute__((aligned(16)));

void phy_viterbi_dot11_sse2(char *y,unsigned char *decoded_bytes,unsigned short n,int offset, int traceback ) {
  

  __m128i *m0_ptr,*m1_ptr,*TB_ptr = &TB[offset<<2];


  char *in = y;
  unsigned char prev_state0;
  unsigned char *TB_ptr2;
  unsigned short table_offset;

  short position;

  //  printf("offset %d, TB_ptr %p\n",offset,TB_ptr);
  if (offset == 0) {
    // set initial metrics
    
    metrics0_15 = _mm_cvtsi32_si128(INIT0);
    metrics16_31 = _mm_xor_si128(metrics16_31,metrics16_31);
    metrics32_47 = _mm_xor_si128(metrics32_47,metrics32_47);
    metrics48_63 = _mm_xor_si128(metrics32_47,metrics32_47);
  }
  rescale = _mm_cvtsi32_si128(RESCALE);

  /*
  print_bytes(metrics0_15,"metrics0_15");
  print_bytes(metrics16_31,"metrics16_31");
  print_bytes(metrics32_47,"metrics32_47");
  print_bytes(metrics48_63,"metrics48_63");
  */


  for (position=offset;position<(offset+n);position++) {


	//printf("%d : (%d,%d)\n",position,in[0],in[1]);

    // get branch metric offsets for the 64 states
    table_offset = (in[0]+8 + ((in[1]+8)<<4))<<6;

    //    printf("Table_offset = %u (in[0]=%d,in[1]=%d)\n",table_offset,in[0],in[1]);

    m0_ptr = (__m128i *)&m0_table[table_offset];
    m1_ptr = (__m128i *)&m1_table[table_offset];

    //    printf("\n");

   // even states
    even0_30a  = _mm_adds_epu8(metrics0_15,m0_ptr[0]);
    //    print_bytes(even0_30a,"even0_30a");

    even32_62a = _mm_adds_epu8(metrics16_31,m0_ptr[1]);
    //    print_bytes(even32_62a,"even32_62a");

    even0_30b  = _mm_adds_epu8(metrics32_47,m0_ptr[2]);
    //    print_bytes(even0_30b,"even0_30b");

    even32_62b = _mm_adds_epu8(metrics48_63,m0_ptr[3]);
    //    print_bytes(even32_62b,"even32_62b");

    //    printf("\n");

    // odd states
    odd1_31a   = _mm_adds_epu8(metrics0_15,m1_ptr[0]);

    //    print_bytes(odd1_31a,"odd1_31a");

    odd33_63a  = _mm_adds_epu8(metrics16_31,m1_ptr[1]);

    //    print_bytes(odd33_63a,"odd33_63a");

    odd1_31b   = _mm_adds_epu8(metrics32_47,m1_ptr[2]);

    //    print_bytes(odd1_31b,"odd1_31b");

    odd33_63b  = _mm_adds_epu8(metrics48_63,m1_ptr[3]);

    //    print_bytes(odd33_63b,"odd33_63b");


 
    
    // select maxima
    //    printf("\n");

    even0_30a  = _mm_max_epu8(even0_30a,even0_30b);

    //    print_bytes(even0_30a,"even0_30a");

    even32_62a = _mm_max_epu8(even32_62a,even32_62b);

    //    print_bytes(even32_62a,"even32_62a");

    odd1_31a   = _mm_max_epu8(odd1_31a,odd1_31b);

    //    print_bytes(odd1_31a,"odd1_31a");

    odd33_63a  = _mm_max_epu8(odd33_63a,odd33_63b);

    //    print_bytes(odd33_63a,"odd33_63a");

    //    printf("\n");    
    // Traceback information

    TBeven0_30  = _mm_cmpeq_epi8(even0_30a,even0_30b);


    TBeven32_62 = _mm_cmpeq_epi8(even32_62a,even32_62b);


    TBodd1_31   = _mm_cmpeq_epi8(odd1_31a,odd1_31b);


    TBodd33_63  = _mm_cmpeq_epi8(odd33_63a,odd33_63b);


    metrics0_15        = _mm_unpacklo_epi8(even0_30a ,odd1_31a);
    metrics16_31       = _mm_unpackhi_epi8(even0_30a ,odd1_31a);
    metrics32_47       = _mm_unpacklo_epi8(even32_62a,odd33_63a);
    metrics48_63       = _mm_unpackhi_epi8(even32_62a,odd33_63a);

    
    //print_bytes(metrics0_15,"metrics0_15");
    //print_bytes(metrics16_31,"metrics16_31");
    //print_bytes(metrics32_47,"metrics32_47");
    //print_bytes(metrics48_63,"metrics48_63");
    


    TB_ptr[0]  = _mm_unpacklo_epi8(TBeven0_30,TBodd1_31);

    //    print_bytes(TB_ptr[0],"TB0_15");

    TB_ptr[1] = _mm_unpackhi_epi8(TBeven0_30,TBodd1_31);

    //    print_bytes(TB_ptr[1],"TB16_31");

    TB_ptr[2] = _mm_unpacklo_epi8(TBeven32_62,TBodd33_63);

    //    print_bytes(TB_ptr[2],"TB32_47");

    TB_ptr[3] = _mm_unpackhi_epi8(TBeven32_62,TBodd33_63);

    //    print_bytes(TB_ptr[3],"TB48_63");

    in+=2;
    TB_ptr += 4;

    // rescale by subtracting minimum
    /****************************************************
    USE SSSE instruction phminpos!!!!!!!
    ****************************************************/
    min_state =_mm_min_epu8(metrics0_15,metrics16_31);
    min_state =_mm_min_epu8(min_state,metrics32_47);
    min_state =_mm_min_epu8(min_state,metrics48_63);

    //    print_bytes(min_state,"min_state");
    
    min_state2 = min_state;
    min_state  = _mm_unpacklo_epi8(min_state,min_state);
    min_state2 = _mm_unpackhi_epi8(min_state2,min_state2);
    min_state  = _mm_min_epu8(min_state,min_state2);

    //    print_bytes(min_state,"min_state");
    
    min_state2 = min_state;
    min_state  = _mm_unpacklo_epi8(min_state,min_state);
    min_state2 = _mm_unpackhi_epi8(min_state2,min_state2);
    min_state  = _mm_min_epu8(min_state,min_state2);

    //    print_bytes(min_state,"min_state");
    
    min_state2 = min_state;
    min_state  = _mm_unpacklo_epi8(min_state,min_state);
    min_state2 = _mm_unpackhi_epi8(min_state2,min_state2);
    min_state  = _mm_min_epu8(min_state,min_state2);

    //    print_bytes(min_state,"min_state");
    
    min_state2 = min_state;
    min_state  = _mm_unpacklo_epi8(min_state,min_state);
    min_state2 = _mm_unpackhi_epi8(min_state2,min_state2);
    min_state  = _mm_min_epu8(min_state,min_state2);

    //    print_bytes(min_state,"min_state");
    
    metrics0_15  = _mm_subs_epu8(metrics0_15,min_state);
    metrics16_31 = _mm_subs_epu8(metrics16_31,min_state);
    metrics32_47 = _mm_subs_epu8(metrics32_47,min_state);
    metrics48_63 = _mm_subs_epu8(metrics48_63,min_state);

    /*
    print_bytes(metrics0_15,"metrics0_15");
    print_bytes(metrics16_31,"metrics16_31");
    print_bytes(metrics32_47,"metrics32_47");
    print_bytes(metrics48_63,"metrics48_63");
    */



  }

  // Traceback
  if (traceback == 1) {
    prev_state0 = 0;
    TB_ptr2 = (unsigned char *)&TB[(offset+n-1)<<2];
    for (position = offset+n-1 ; position>-1; position--) {
      //   printf("pos %d: decoded %x\n",position>>3,decoded_bytes[position>>3]);
      decoded_bytes[(position)>>3] += (prev_state0 & 0x1)<<(position & 0x7);
      /*
      if ((position%8) == 0)
	printf("%d: %x\n",(position>>3),decoded_bytes[(position>>3)]);
      
      printf("pos %d : ps = %d -> %d\n",position,prev_state0,TB_ptr2[prev_state0]);
      */
      if (TB_ptr2[prev_state0] == 0) 
	prev_state0 = (prev_state0 >> 1);
      else
	prev_state0 = 32 + (prev_state0>>1);
      
      TB_ptr2-=64;
    }
  }
  _mm_empty();

}

#else //EXPRESSMIMO_TARGET

void phy_viterbi_dot11(char *y,unsigned char *decoded_bytes,unsigned short n) {
}

#endif //EXPRESSMIMO_TARGET

/*
void print_bytes(__m128i x,char *s) {

  unsigned char *tempb = (unsigned char *)&x;

  printf("%s  : %u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",s,
	 tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7],
	 tempb[8],tempb[9],tempb[10],tempb[11],tempb[12],tempb[13],tempb[14],tempb[15]);

}
*/

#ifdef TEST_DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int test_viterbi()
{
  unsigned char test[8];
  //_declspec(align(16))  char channel_output[512];
  //_declspec(align(16))  unsigned char output[512],decoded_output[16], *inPtr, *outPtr;

  char channel_output[512];
  unsigned char output[512],decoded_output[16], *inPtr, *outPtr;
  unsigned int i;

  
  test[0] = 7;
  test[1] = 0xa5;
  test[2] = 0;
  test[3] = 0xfe;
  test[4] = 0x1a;
  test[5] = 0x33;
  test[6] = 0x99;
  test[7] = 0;
  

  ccodedot11_init();
  ccodedot11_init_inv();
  
  inPtr = test;
  outPtr = output;
  phy_generate_viterbi_tables();
  ccodedot11_encode(8, inPtr, outPtr,0);

  for (i = 0; i < 128; i++){
    channel_output[i] = 7*(2*output[i] - 1);
  }

  memset(decoded_output,0,16);
  phy_viterbi_dot11(channel_output,decoded_output,64);

  printf("Input               :");
  for (i =0 ; i<8 ; i++)
    printf("%x ",inPtr[i]);
  printf("\n");

  printf("Unoptimized Viterbi :");
  for (i =0 ; i<8 ; i++)
    printf("%x ",decoded_output[i]);
  printf("\n");

  memset(decoded_output,0,16);
  phy_viterbi_dot11_sse2(channel_output,decoded_output,64);
  printf("Optimized Viterbi   :");
  for (i =0 ; i<8 ; i++)
    printf("%x ",decoded_output[i]);
  printf("\n");


  printf("\n");
}




int main() {


  test_viterbi();
  return(0);
}

#endif // TEST_DEBUG


