/* file: viterbit_lte.c
   purpose: SIMD optimized LTE Viterbi Decoder for rate 1/3 Tail-biting convolutional code.  Performs two iterations
            of code.  First pass does Viterbi with all initial partial metrics set to zero.  Second pass does Viterbi
            with initial partial metrics set to values from final state values after first pass. Max is selected at
	    end to do trace-back.
   author: raymond.knopp@eurecom.fr
   date: 21.10.2009 
*/

#ifdef USER_MODE
#include <stdio.h>
#endif

#ifndef TEST_DEBUG
#include "PHY/defs.h"
#include "PHY/extern.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define msg printf
#endif

#ifndef EXPRESSMIMO_TARGET
#include "emmintrin.h"
#endif //EXPRESSMIMO_TARGET

extern u8 ccodelte_table[128],ccodelte_table_rev[128];




#ifdef __KERNEL__
#define printf rt_printk
#endif

#ifndef EXPRESSMIMO_TARGET

static s8 m0_table[64*16*16*16] __attribute__ ((aligned(16)));
static s8 m1_table[64*16*16*16] __attribute__ ((aligned(16)));


// Set up Viterbi tables for SSE2 implementation
void phy_generate_viterbi_tables_lte() {

  s8 w[8],in0,in1,in2;
  u8 state,index0,index1;

  for (in0 = -8; in0 <8 ; in0++) {   // use 4-bit quantization
    for (in1 = -8; in1 <8 ;in1++) {
      for (in2 = -8; in2 <8 ;in2++) {

	
	w[0] = 24 - in0 - in1 - in2;           // -1,-1,-1
	w[1] = 24 + in0 - in1 - in2;           // -1, 1,-1
	w[2] = 24 - in0 + in1 - in2;           //  1,-1,-1
	w[3] = 24 + in0 + in1 - in2;           //  1, 1,-1
	w[4] = 24 - in0 - in1 + in2;           // -1,-1, 1
	w[5] = 24 + in0 - in1 + in2;           // -1, 1, 1
	w[6] = 24 - in0 + in1 + in2;           //  1,-1, 1
	w[7] = 24 + in0 + in1 + in2;           //  1, 1, 1
	
	//    printf("w: %d %d %d %d\n",w[0],w[1],w[2],w[3]);
	for (state=0; state<64 ; state++) {
	  
	  // input 0
	  index0 = (state<<1);

	  m0_table[(in0+8 + (16*(in1+8)) + (256*(in2+8)))*64 +state] = w[ccodelte_table_rev[index0]];
	  
	  
	  //	  if (position < 8)
	  //	  printf("%d,%d : prev_state0 = %d,m0 = %d,w=%d (%d)\n",position,state,prev_state0%64,m0,w[ccodelte_table[prev_state0]],partial_metrics[prev_state0%64]);
	  
	  // input 1
	  index1 = (1+ (state<<1));
	  m1_table[(in0+8 + (16*(in1+8)) + (256*(in2+8)))*64 +state] = w[ccodelte_table_rev[index1]]; 
	  
	}
      }
    }
  }
}


#define INIT0 0x00000080
#define RESCALE 0x00000040

//#define DEBUG_VITERBI

#ifdef DEBUG_VITERBI
void print_bytes(char *s,__m128i *x) {

  u8 *tempb = (u8 *)x;

  printf("%s  : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",s,
	 tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7],
	 tempb[8],tempb[9],tempb[10],tempb[11],tempb[12],tempb[13],tempb[14],tempb[15]);

}

/*
void print_shorts(__m128i x,char *s) {

  s16 *tempb = (s16 *)&x;

  printf("%s  : %d,%d,%d,%d,%d,%d,%d,%d\n",s,
	 tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7]
	 );

}
*/
#endif // USER_MODE


static __m128i  TB[4*8192];

static __m128i metrics0_15,metrics16_31,metrics32_47,metrics48_63,even0_30a,even0_30b,even32_62a,even32_62b,odd1_31a,odd1_31b,odd33_63a,odd33_63b,TBeven0_30,TBeven32_62,TBodd1_31,TBodd33_63;// __attribute__((aligned(16)));

static __m128i min_state,min_state2;// __attribute__((aligned(16)));

void phy_viterbi_lte_sse2(s8 *y,u8 *decoded_bytes,u16 n) {
  

  static __m128i *m0_ptr,*m1_ptr,*TB_ptr = &TB[0];


  s8 *in = y;
  u8 prev_state0,maxm,s;
  static u8 *TB_ptr2;
  u32 table_offset;
  u8 iter;
  s16 position;

  // set initial metrics
  //debug_msg("Doing viterbi\n");

  metrics0_15 = _mm_xor_si128(metrics0_15,metrics0_15);
  metrics16_31 = _mm_xor_si128(metrics16_31,metrics16_31);
  metrics32_47 = _mm_xor_si128(metrics32_47,metrics32_47);
  metrics48_63 = _mm_xor_si128(metrics32_47,metrics32_47);
#ifndef USER_MODE
  //debug_msg("Doing viterbi 2\n");
#endif
  /*
  print_bytes(metrics0_15,"metrics0_15");
  print_bytes(metrics16_31,"metrics16_31");
  print_bytes(metrics32_47,"metrics32_47");
  print_bytes(metrics48_63,"metrics48_63");
  */

  for (iter=0;iter<2;iter++) {
    in = y;
    TB_ptr=&TB[0];

    //    printf("Iteration %d\n",iter);
    for (position=0;position<n;position++) {
      
      
      //      printf("%d/%d : (%d,%d,%d)\n",position,n-1,in[0],in[1],in[2]);
      

      // get branch metric offsets for the 64 states
      table_offset = (in[0]+8 + ((in[1]+8)<<4) + ((in[2]+8)<<8))<<6;
      /*       
      printf("Table_offset = %u (in[0]=%d,in[1]=%d,in[2]=%d)\n",table_offset,in[0],in[1],in[2]);
      print_bytes("m0",&m0_table[table_offset]);
      print_bytes("m1",&m1_table[table_offset]);
      */
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
      
      /*      
      print_bytes(metrics0_15,"metrics0_15");
      print_bytes(metrics16_31,"metrics16_31");
      print_bytes(metrics32_47,"metrics32_47");
      print_bytes(metrics48_63,"metrics48_63");
      */
      
      
      TB_ptr[0]  = _mm_unpacklo_epi8(TBeven0_30,TBodd1_31);
      
      //    print_bytes(TB_ptr[0],"TB0_15");
      
      TB_ptr[1] = _mm_unpackhi_epi8(TBeven0_30,TBodd1_31);
      
      //    print_bytes(TB_ptr[1],"TB16_31");
      
      TB_ptr[2] = _mm_unpacklo_epi8(TBeven32_62,TBodd33_63);
      
      //    print_bytes(TB_ptr[2],"TB32_47");
      
      TB_ptr[3] = _mm_unpackhi_epi8(TBeven32_62,TBodd33_63);
      
      //    print_bytes(TB_ptr[3],"TB48_63");
      
      in+=3;
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
	print_bytes("metrics0_15",&metrics0_15);
	print_bytes("metrics16_31",&metrics16_31);
	print_bytes("metrics32_47",&metrics32_47);
	print_bytes("metrics48_63",&metrics48_63);
      
      printf("\n");
      */
      
    }

  } // iteration

  // Traceback
  prev_state0 = 0;
  maxm = 0;

  for (s=0;s<16;s++)
    if (((u8 *)&metrics0_15)[s] > maxm) {
      maxm = ((u8 *)&metrics0_15)[s];
      prev_state0 = s;
    }

  for (s=0;s<16;s++)
    if (((u8 *)&metrics16_31)[s] > maxm) {
      maxm = ((u8 *)&metrics16_31)[s];
      prev_state0 = s+16;
    }

  for (s=0;s<16;s++)
    if (((u8 *)&metrics32_47)[s] > maxm) {
      maxm = ((u8 *)&metrics32_47)[s];
      prev_state0 = s+32;
    }

  for (s=0;s<16;s++)
    if (((u8 *)&metrics48_63)[s] > maxm) {
      maxm = ((u8 *)&metrics48_63)[s];
      prev_state0 = s+48;
    }

  //  printf("Max state %d\n",prev_state0);
  TB_ptr2 = (u8 *)&TB[(n-1)*4];
  for (position = n-1 ; position>-1; position--) {

//    if ((position%8) == 0)
//	printf("%d: %x\n",1+(position>>3),decoded_bytes[1+(position>>3)]);
		     	
    decoded_bytes[(position)>>3] += (prev_state0 & 0x1)<<(7-(position & 0x7));
    
    //    printf("pos %d : ps = %d -> %d\n",position,prev_state0,TB_ptr2[prev_state0]);
    
    if (TB_ptr2[prev_state0] == 0) 
      prev_state0 = (prev_state0 >> 1);
    else
      prev_state0 = 32 + (prev_state0>>1);

    TB_ptr2-=64;
  }
  //  printf("Max state %d\n",prev_state0);
  _mm_empty();
  _m_empty();

}

#else //EXPRESSMIMO_TARGET

void phy_viterbi_lte(s8 *y,u8 *decoded_bytes,u16 n) {
}

#endif //EXPRESSMIMO_TARGET

/*
void print_bytes(__m128i x,s8 *s) {

  u8 *tempb = (u8 *)&x;

  printf("%s  : %u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",s,
	 tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7],
	 tempb[8],tempb[9],tempb[10],tempb[11],tempb[12],tempb[13],tempb[14],tempb[15]);

}
*/

#ifdef TEST_DEBUG
int test_viterbi(u8 dabflag)
{
  u8 test[8];
  //_declspec(align(16))  s8 channel_output[512];
  //_declspec(align(16))  u8 output[512],decoded_output[16], *inPtr, *outPtr;

  s8 channel_output[512];
  u8 output[512],decoded_output[16], *inPtr, *outPtr;
  u32 i;

  
  test[0] = 7;
  test[1] = 0xa5;
  test[2] = 0;
  test[3] = 0xfe;
  test[4] = 0x1a;
  test[5] = 0x33;
  test[6] = 0x99;
  test[7] = 0x14;
  
  if (dabflag==0) {
    ccodelte_init();
    ccodelte_init_inv();
  }
  else {
    ccodedab_init();
    ccodedab_init_inv();
    printf("Running with DAB polynomials\n");
  }
  inPtr = test;
  outPtr = output;
  phy_generate_viterbi_tables_lte();
  ccodelte_encode(64,     //input length in bits 
		  0,      // add 16-bit crc with rnti scrambling
		  inPtr,  // input pointer
		  outPtr, // output pointer
		  0);     // rnti (optional)

  for (i = 0; i < 64*3; i++){
    channel_output[i] = 7*(2*output[i] - 1);
  }

  memset(decoded_output,0,16);
  phy_viterbi_lte_sse2(channel_output,decoded_output,64);
  printf("Optimized Viterbi :");
  for (i =0 ; i<8 ; i++)
    printf("input %d : %x => %x\n",i,inPtr[i],decoded_output[i]);
}




int main(int argc, char **argv) {

  char c;
  u8 dabflag=0;

  while ((c = getopt (argc, argv, "d")) != -1) {
    if (c=='d')
      dabflag=1;
  }

  test_viterbi(dabflag);
  return(0);
}

#endif // TEST_DEBUG


