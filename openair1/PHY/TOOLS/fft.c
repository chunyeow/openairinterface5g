#include "defs.h"
#ifndef EXPRESSMIMO_TARGET
/*! \brief 
//  FFT base 2 in fixed point
//  Alessandro Nordio, 03/2001
//  Modified for Intel Free Compiler : R. Knopp, 06.04
*/

//  OPTIMIZATION OPTIONS ICC COMPILER : -o3 -tpp7 -xW  -parallel
//  -o3 : loop transformations and data prefetching
//  -tpp7 : enables optimal instruction scheduling and cache management for the pentium 4 processor
//  -axW : generates specialized code for the pentium 4 processor while also generating generic IA-32 code
//  -xW : generates code to run exclusively on the pentium 4 processor
//  -ip : single file optimization
//  -parallel : the compiler can detect loop which may benefit from multi-thread execution


//  This version is optimized for MMX.  (Version 4.0)

//  x contains N = 2^log2size complex elements in the format | Re0 Im0 Re1 Im1 | Re2 Im2 Re3 Im3 |...
//  So first we do reverse bit ordering and duplication (| Re0 Im0 Re1 Im1 | -> | Re0 Im0 Re0 Im0 || Re1 Im1 Re1 Im1 |)
//  then we run the FFT stages starting from the 1st
//
//
//  y - output vector of size N = 2^log2size mmx_t element packed in the format
//      Output format: [Re0,Im0,Re0,Im0, Re1,Im1,Re1,Im1, ....., Re(N-1),Im(N-1),Re(N-1),Im(N-1)]
//
//  twiddle    - twiddle factors are precomputed and are in the format
//                 Twiddle format [Re0,-Im0,Im0,-Re0|Re1,-Im1,Im1,-Re1|,.....]
//                 Each element of twiddle is a mmx_t union (64 bit) that contains |Re,-Im,Im,Re|
//                   of a certain twidde factor.
//                 This way of storing the twiddles is useful to increase the performance of the FFT
//                 The twiddle vector is logically divided into 'log2size' consecutive parts:
//                   (e.g. log2size=11 for a 2048 point fft)
//                 The i-th part contains n_i=2^i elements, for i=0..log2size-1
//                 The elements of the i-th part are  round(A*exp(-j*2*pi*n/(2^(i+1)))) for n=0..n_i
//                   where A is a power-of-two constant integer. We assume A=2^15.
//                   (Since A=2^15 then in the butterflies we shift the output of the mpy by 14 (see below))
//                 The elements of the twiddle are precomputed using a matlab file 'twiddle_mmx.m' and
//                   stored in a binary file in little endian format.
//                 The overall twiddle vector contains 2^log2size -1 complex elements
//                 The i-th part of the twiddle vector is used during the i-th stage of the FFT
//                 In this version the first stage of the FFT is simplified and has been taken out of the loop.
//
// rev         - reverse buffer. Contains the reverse indexes (N indexes)
//
// log2size    - is the logarithm in base 2 of the size of the FFT.
//                 ** WARNING log2size>=2 **
//

#include "emmintrin.h"
#include "mmintrin.h"

/// Function ReverseBits()
/// computes bit reversed permutation vector

u16 ReverseBits(u16 index, 
			   u16 NumBits)
{
  // This routine reverse bit orders the bits of the argument 'index'
  // It assumes the index has NumBits bits.
  unsigned i, rev;

  for ( i=rev=0; i < NumBits; i++ )
    {
      rev = (rev << 1) | (index & 1);
      index >>= 1;
    }

  return rev;
}


void init_fft(u16 size, u8 logsize,u16 *local_rev) {


  int i;
  //msg("[openair][FFT][INIT] Using %d point fft (%d, %p)\n",size,logsize,local_rev );


  for(i=0;i<size;i++) {
    local_rev[i] = ReverseBits(i,logsize);    // 8 if SAMPLES_PERCHIP=2, 9 if SAMPLES_PER_CHIP = 4
  }
}


#define SHIFT 14 // Shift of twiddle amplitude

void fft(s16 *x,          /// complex input
	 s16 *y,          /// complex output
	 s16 *twiddle,    /// complex twiddle factors
	 u16 *rev,        /// bit reversed permutation vector
	 u8 log2size,     /// log2(FFT_SIZE)
	 u8 scale,
	 u8 input_fmt)   /// 0 means 64-bit complex interleaved format else complex-multiply ready repeated format
{

  int i,j,k;             // counters
  int bs;                // block_size
  int n_b;               // numer of blocks
  u16 index;       // reverse index
  u8 scale2=1;//scale;
  u16 *revl=rev;

  register __m64 mm0,mm1,mm2;//mm3,mm4,mm5,mm6,mm7;

  __m64 *x_pt = (__m64 *)x,*y_pt = (__m64 *)y,*tw_pt=(__m64 *)twiddle;  // output data pointer and twiddle pointer

  u32   size = 1<<log2size;   // size of the FFT
  // FFT does not shift the result


  // Here we do reverse bit ordering and duplication and then we start from the FIRST stage of the FFT.

  //  msg("fft: x %x, y %x, twiddle %x,rev %x, format %d, log2size %d, scale %d\n",
  //        x,y,twiddle,rev,input_fmt,log2size,scale);
  
  //  for (i=0;i<10;i++)
  //    printf("%d ",rev[i]);
  //  printf("\n");

  if (input_fmt == 0)
    for(i=0;i<(size>>1);i++)      // reverse 2 complex samples at a time => does the loop size>>1 times
      {
	
	mm0 = x_pt[i];


	mm1 = mm0;
	mm2 = mm0;



	mm1 = _m_punpckldq(mm1,mm0);

	mm2 = _m_punpckhdq(mm2,mm0);

	index = revl[0];             // get the output index (reverse bit ordering index)

	//	printf("%x\n",index);
	y_pt[index] = mm1;

	//	printk("fft: rev0 = %d\n",index);

	//	printf("%x\n",index);
	index = revl[1];             // get the output index (reverse bit ordering index)

	y_pt[index] = mm2;
	revl+=2;
	//	printk("fft: rev1 = %d\n",index);
	// increase reverse buffer pointer
      }
  else
    for(i=0;i<size;i++)      // reverse 2 complex samples at a time => does the loop size>>1 times
      y_pt[rev[i]] = x_pt[i];


  //return;
 
  // *** FIRST FFT STAGE *** 

  // Now the input data is stored in y in reverse bit order in the format |x_i 0 x_i 0| (Re Im Re Im)
  // SECOND STEP: does the radix-2 FFT itself
  // - First stage of the FFT
  //   In the first stage we do not use twiddle factors since the twiddle is (1,0)
  //   so we avoid multiplications and shifts
  //   This loop is unrolled by a factor 2 ->(n_b>>1)
  //   So here we implicitly assume size>=4
  //   For each loop it computes two butterflies
  //   butterfly input : 4 cpx elements : x0, x1, x2, x3
  //   butterfly output: 4 cpx elements : X0, X1, X2, X3
  //   twiddles associated with the butterflies: w0 and w1
  //   X0 = x0+w0*x1  = x0+x1
  //   X1 = x0-w0*x1  = x0-x1
  //   X2 = x2+w1*x3  = x2+x3
  //   X3 = x2-w1*x3  = x2-x3
  //   For the first stage w0 = w1 = [1 0 0 1] (see twiddle format)

  //  msg("[PHY_fft_intel_mmx] : Stage 1\n");
  y_pt = (__m64 *)&y[0];

  //1 butterfly = 2 inputs
  for(i=0;i<(size>>1);i++){

    /*
      msg("[PHY_fft_intel_mmx: y_pt = %p\n",y_pt);

      msg("[PHY_fft_intel_mmx] : y[0] = (%d %d %d %d)  y[1] = (%d %d %d %d)\n",
      ((s16 *)(&y_pt[0]))[0],
      ((s16 *)(&y_pt[0]))[1],
      ((s16 *)(&y_pt[0]))[2],
      ((s16 *)(&y_pt[0]))[3],
      ((s16 *)(&y_pt[1]))[0],
      ((s16 *)(&y_pt[1]))[1],
      ((s16 *)(&y_pt[1]))[2],
      ((s16 *)(&y_pt[1]))[3]);
    */
    mm0 = _mm_adds_pi16(y_pt[0],y_pt[1]);//top of butterfly
    mm1 = _mm_subs_pi16(y_pt[0],y_pt[1]);//bottom of butterfly
    //shift four 16-bit values right by 1 while shifting in the sign bit <=> PSRAWI and store the result for next stage
    y_pt[0] = mm0; //_mm_srai_pi16(mm0,1);
    y_pt[1] = mm1; //_mm_srai_pi16(mm1,1);
      

    y_pt += 2;


    
  }
  
  // - Stages from 2 to log2size-1

  tw_pt++;                      // for the 2nd stage we start with twiddle[4]
  bs  =2;                       // the blocksize is two
  n_b =size>>2;                 // half n_b

  for(i=1;i<(log2size>>1);i++) {
    y_pt = (__m64 *)&y[0];
    
    for(k=0;k<n_b;k++){
      // bs is greater than or equal to 2 so I can unroll a bit the inner loop
      // so that every loop computes two butterflies
      // bs is greater than or equal to 2 so I can unroll a bit the inner loop
      //   For each loop it computes two butterflies
      //   butterfly input : 4 cpx elements : x0, x1, x2, x3
      //   butterfly output: 4 cpx elements : X0, X1, X2, X3
      //   twiddles associated with the butterflies: w0 and w1
      //   X0 = x0+w0*x1
      //   X1 = x0-w0*x1
      //   X2 = x2+w1*x3
      //   X3 = x2-w1*x3
      
      for(j=0;j<bs;j+=2) {
	mm0 = _mm_madd_pi16(tw_pt[0],y_pt[bs]);//PMADDWD
	mm0 = _mm_srai_pi32(mm0,SHIFT);//PSRAD, divide the 32-bit result of multiplication by 2^15 while shifting in the sign bit
	mm0 = _mm_packs_pi32(mm0,mm0);//PACKSSWD pack with signed saturation to restore the result in format |Re Im Re Im|
	y_pt[bs] = _mm_subs_pi16(y_pt[0],mm0);//PSUBSW
	y_pt[0] = _mm_adds_pi16(y_pt[0],mm0);//PADDSW
	
	
	
	
	mm0 = _mm_madd_pi16(tw_pt[1],y_pt[bs+1]);//PMADDWD
	mm0 = _mm_srai_pi32(mm0,SHIFT);//PSRAD, divide the 32-bit result of multiplication by 2^15 while shifting in the sign bit
	mm0 = _mm_packs_pi32(mm0,mm0);//PACKSSWD pack with signed saturation to restore the result in format |Re Im Re Im|
	y_pt[bs+1] = _mm_subs_pi16(y_pt[1],mm0);//PSUBSW
	y_pt[1] = _mm_adds_pi16(y_pt[1],mm0);//PADDSW
	
	
	y_pt += 2;
	tw_pt += 2;
      }
      tw_pt -=bs;                  // twiddle pointer now points to the first twiddle of this stage
      y_pt +=bs;                   // jump to next block
    }
    tw_pt+=bs;                     // jump to the first twiddle of the next stage
    
    bs  <<=1;                       // double bs
    n_b >>=1;                       // half n_b

  }
  
  // last stage we shift by scale (for a normalized fft, log2size/2)
  for(i=(log2size>>1);i<log2size;i++) {
    y_pt = (__m64 *)&y[0];
    
    for(k=0;k<n_b;k++) {
      // bs is greater than or equal to 2 so I can unroll a bit the inner loop
      // so that every loop computes two butterflies
      // bs is greater than or equal to 2 so I can unroll a bit the inner loop
      //   For each loop it computes two butterflies
      //   butterfly input : 4 cpx elements : x0, x1, x2, x3
      //   butterfly output: 4 cpx elements : X0, X1, X2, X3
      //   twiddles associated with the butterflies: w0 and w1
      //   X0 = x0+w0*x1
      //   X1 = x0-w0*x1
      //   X2 = x2+w1*x3
      //   X3 = x2-w1*x3
      
      for(j=0;j<bs;j+=2) {
	mm0 = _mm_madd_pi16(tw_pt[0],y_pt[bs]);//PMADDWD
	mm0 = _mm_srai_pi32(mm0,SHIFT);//PSRAD, divide the 32-bit result of multiplication by 2^15 while shifting in the sign bit
	mm0 = _mm_packs_pi32(mm0,mm0);//PACKSSWD pack with signed saturation to restore the result in format |Re Im Re Im|
	
	mm1 = _mm_adds_pi16(y_pt[0],mm0);//PADDSW
	mm2 = _mm_subs_pi16(y_pt[0],mm0);//PSUBSW
	//shift four 16-bit values right by 1 while shifting in the sign bit <=> PSRAWI and store the result for next stage
	y_pt[0] = _mm_srai_pi16(mm1,scale2);
	y_pt[bs] = _mm_srai_pi16(mm2,scale2);
	
	mm0 = _mm_madd_pi16(tw_pt[1],y_pt[bs+1]);//PMADDWD
	mm0 = _mm_srai_pi32(mm0,SHIFT);//PSRAD, divide the 32-bit result of multiplication by 2^15 while shifting in the sign bit
	mm0 = _mm_packs_pi32(mm0,mm0);//PACKSSWD pack with signed saturation to restore the result in format |Re Im Re Im|
	mm1 = _mm_adds_pi16(y_pt[1],mm0);//PADDSW
	mm2 = _mm_subs_pi16(y_pt[1],mm0);//PSUBSW
	//shift four 16-bit values right by 1 while shifting in the sign bit <=> PSRAWI and store the result for next stage
	y_pt[1] = _mm_srai_pi16(mm1,scale2);
	y_pt[bs+1] = _mm_srai_pi16(mm2,scale2);
	
	y_pt += 2;
	tw_pt += 2;
      }
      tw_pt -=bs;                  // twiddle pointer now points to the first twiddle of this stage
      y_pt +=bs;                   // jump to next block
    }
    tw_pt+=bs;                     // jump to the first twiddle of the next stage
    bs  <<=1;                       // double bs
    n_b >>=1;                       // half n_b
  }
  
  _mm_empty();
  _m_empty();


}


#else //EXPRESSMIMO_TARGET

void init_fft(u16 size, u8 logsize,u16 *local_rev) {

  return;
}

void fft(s16 *x,          /// complex input
	 s16 *y,          /// complex output
	 s16 *twiddle,    /// complex twiddle factors
	 u16 *rev,        /// bit reversed permutation vector
	 u8 log2size,     /// log2(FFT_SIZE)
	 u8 scale,
	 u8 input_fmt)   /// 0 means 64-bit complex interleaved format else complex-multiply ready repeated format
{

}

#endif //EXPRESSMIMO_TARGET






#ifdef MAIN
#include "twiddle256.h"

u32 s0, s1, s2, b;

inline void pset_taus_seed(u32 off) {


  s0 = (u32)0x1e23d852 + (off<<4);
  s1 = (u32)0x81f38a1c + (off<<4);
  s2 = (u32)0xfe1a133e + (off<<4);

}

inline u32 ptaus() {

  b = (((s0 << 13) ^ s0) >> 19);
  s0 = (((s0 & 0xFFFFFFFE) << 12)^  b);
  b = (((s1 << 2) ^ s1) >> 25);
  s1 = (((s1 & 0xFFFFFFF8) << 4)^  b);
  b = (((s2 << 3) ^ s2) >> 11);
  s2 = (((s2 & 0xFFFFFFF0) << 17)^  b);
  return s0 ^ s1 ^ s2;
}

main() {

  s16 local_rev[256];
  int i;
  int input[512],output[512];
  
  init_fft(256,8,local_rev);

  pset_taus_seed(3);

  for (i=0;i<256;i++) {
    input[i] = (((ptaus()>>5)&0xffff0000) | ((ptaus()>>21)&0x0000ffff);
  }

  fft(input, 
      output,
      twiddle_fft256,    /// complex twiddle factors
      local_rev,        /// bit reversed permutation vector
      8,
      4,
      0);
/*

  fft(input, 
      output,
      twiddle_fft256,    /// complex twiddle factors
      local_rev,        /// bit reversed permutation vector
      8,
      4,
      0);
*/

  printf("input = [");
  for (i=0;i<256;i++)
    printf("%d+sqrt(-1)*(%d)\n",((s16*)input)[2*i],((s16*)input)[1+(2*i)]);
  printf("];\n");

  printf("output = [");
  for (i=0;i<256;i++)
    printf("%d+sqrt(-1)*(%d)\n",((s16*)output)[4*i],((s16*)output)[1+(4*i)]);
  printf("];\n");
}

#endif //MAIN
















