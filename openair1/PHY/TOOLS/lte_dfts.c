#ifdef USER_MODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <stdint.h>

#ifndef MR_MAIN
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "defs.h"
#else
#include "PHY/TOOLS/twiddle512.h"
#include "PHY/TOOLS/twiddle2048.h"
#include "PHY/TOOLS/twiddle4096.h"
#include "PHY/TOOLS/twiddle8192.h"
#include "time_meas.h"

int rev2048[2048],rev512[512],rev4096[4096],rev8192[8192];
#define debug_msg
#define ONE_OVER_SQRT2_Q15 23170

#endif


#include "emmintrin.h"
#include "xmmintrin.h"


static int16_t conjugatedft[8] __attribute__((aligned(16))) = {-1,1,-1,1,-1,1,-1,1} ;


#ifndef __SSE3__
__m128i zerodft;
#define _mm_abs_epi16(xmmx) _mm_xor_si128((xmmx),_mm_cmpgt_epi16(zerodft,(xmmx)))
#define _mm_sign_epi16(xmmx,xmmy) _mm_xor_si128((xmmx),_mm_cmpgt_epi16(zerodft,(xmmy)))
#else
#include <pmmintrin.h>
#include <tmmintrin.h>
#endif

static short reflip[8]  __attribute__((aligned(16))) = {1,-1,1,-1,1,-1,1,-1};

static inline void cmac(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));
static inline void cmac(__m128i a,__m128i b, __m128i *re32, __m128i *im32) {

  __m128i cmac_tmp,cmac_tmp_re32,cmac_tmp_im32;

  cmac_tmp    = _mm_sign_epi16(b,*(__m128i*)reflip);
  cmac_tmp_re32  = _mm_madd_epi16(a,cmac_tmp);


  cmac_tmp    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  cmac_tmp    = _mm_shufflehi_epi16(cmac_tmp,_MM_SHUFFLE(2,3,0,1));

  cmac_tmp_im32  = _mm_madd_epi16(cmac_tmp,a);

  *re32 = _mm_add_epi32(*re32,cmac_tmp_re32);
  *im32 = _mm_add_epi32(*im32,cmac_tmp_im32);
}




static inline void cmult(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));

static inline void cmult(__m128i a,__m128i b, __m128i *re32, __m128i *im32) {

  register __m128i mmtmpb;

  mmtmpb    = _mm_sign_epi16(b,*(__m128i*)reflip);
  *re32     = _mm_madd_epi16(a,mmtmpb);
  mmtmpb    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  mmtmpb    = _mm_shufflehi_epi16(mmtmpb,_MM_SHUFFLE(2,3,0,1));
  *im32  = _mm_madd_epi16(a,mmtmpb);

}

static inline void cmultc(__m128i a,__m128i b, __m128i *re32, __m128i *im32) __attribute__((always_inline));

static inline void cmultc(__m128i a,__m128i b, __m128i *re32, __m128i *im32) {

  register __m128i mmtmpb;

  *re32     = _mm_madd_epi16(a,b);
  mmtmpb    = _mm_sign_epi16(b,*(__m128i*)reflip);
  mmtmpb    = _mm_shufflelo_epi16(mmtmpb,_MM_SHUFFLE(2,3,0,1));
  mmtmpb    = _mm_shufflehi_epi16(mmtmpb,_MM_SHUFFLE(2,3,0,1));
  *im32  = _mm_madd_epi16(a,mmtmpb);

}


static inline __m128i cpack(__m128i xre,__m128i xim) __attribute__((always_inline));

static inline __m128i cpack(__m128i xre,__m128i xim) {

  register __m128i cpack_tmp1,cpack_tmp2;

  cpack_tmp1 = _mm_unpacklo_epi32(xre,xim);
  cpack_tmp2 = _mm_unpackhi_epi32(xre,xim);
  return(_mm_packs_epi32(_mm_srai_epi32(cpack_tmp1,15),_mm_srai_epi32(cpack_tmp2,15)));

}


static inline void packed_cmult(__m128i a,__m128i b, __m128i *c) __attribute__((always_inline));

static inline void packed_cmult(__m128i a,__m128i b, __m128i *c) {

  __m128i cre,cim;
  cmult(a,b,&cre,&cim);
  *c = cpack(cre,cim);

}


static inline void packed_cmultc(__m128i a,__m128i b, __m128i *c) __attribute__((always_inline));

static inline void packed_cmultc(__m128i a,__m128i b, __m128i *c) {

  __m128i cre,cim;

  cmultc(a,b,&cre,&cim);
  *c = cpack(cre,cim);

}

static inline __m128i packed_cmult2(__m128i a,__m128i b,__m128i b2) __attribute__((always_inline));

static inline __m128i packed_cmult2(__m128i a,__m128i b,__m128i b2) {
  
  
  register __m128i cre,cim;

  cre       = _mm_madd_epi16(a,b);
  cim       = _mm_madd_epi16(a,b2);
  /*
  mmtmpb    = _mm_sign_epi16(b,*(__m128i*)reflip);
  cre       = _mm_madd_epi16(a,mmtmpb);
  mmtmpb    = _mm_shufflelo_epi16(b,_MM_SHUFFLE(2,3,0,1));
  mmtmpb    = _mm_shufflehi_epi16(mmtmpb,_MM_SHUFFLE(2,3,0,1));
  cim       = _mm_madd_epi16(a,mmtmpb);
  */
  /*
  __m128i cre,cim;
  cmult(a,b,&cre,&cim);
  */

  return(cpack(cre,cim));

}

/*
static inline __m128i packed_cmultc2(__m128i a,__m128i b,__m128i b2) __attribute__((always_inline));

static inline __m128i packed_cmultc2(__m128i a,__m128i b,__m128i b2) {

  __m128i cre,cim;
  
  cmultc(a,b,&cre,&cim);
  return(cpack(cre,cim));

}
*/

static int16_t W0s[8]__attribute__((aligned(16))) = {32767,0,32767,0,32767,0,32767,0};

static int16_t W13s[8]__attribute__((aligned(16))) = {-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378};
static int16_t W23s[8]__attribute__((aligned(16))) = {-16384,28378,-16384,28378,-16384,28378,-16384,28378};

static int16_t W15s[8]__attribute__((aligned(16))) = {10126,-31163,10126,-31163,10126,-31163,10126,-31163};
static int16_t W25s[8]__attribute__((aligned(16))) = {-26509,-19260,-26509,-19260,-26509,-19260,-26509,-19260};
static int16_t W35s[8]__attribute__((aligned(16))) = {-26510,19260,-26510,19260,-26510,19260,-26510,19260};
static int16_t W45s[8]__attribute__((aligned(16))) = {10126,31163,10126,31163,10126,31163,10126,31163};

__m128i *W0 = (__m128i *)W0s;
__m128i *W13 = (__m128i *)W13s;
__m128i *W23 = (__m128i *)W23s;
__m128i *W15 = (__m128i *)W15s;
__m128i *W25 = (__m128i *)W25s;
__m128i *W35 = (__m128i *)W35s;
__m128i *W45 = (__m128i *)W45s;

static int16_t dft_norm_table[16] = {9459,  //12
				     6689,//24
				     5461,//36
				     4729,//48
				     4230,//60
				     23170,//72 
				     3344,//96
				     3153,//108
				     2991,//120
				     18918,//sqrt(3),//144
				     18918,//sqrt(3),//180
				     16384,//2, //192
				     18918,//sqrt(3), // 216
				     16384,//2, //240
				     18918,//sqrt(3), // 288
				     14654}; //sqrt(5) //300


static inline void bfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)__attribute__((always_inline));

static inline void bfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw) {

  __m128i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m128i bfly2_tmp1,bfly2_tmp2;

  cmult(*(x0),*(W0),&x0r_2,&x0i_2);
  cmult(*(x1),*(tw),&x1r_2,&x1i_2);

  dy0r = _mm_srai_epi32(_mm_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm_srai_epi32(_mm_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm_srai_epi32(_mm_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm_srai_epi32(_mm_sub_epi32(x0i_2,x1i_2),15);
  
  bfly2_tmp1 = _mm_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}

static inline void bfly2_tw1(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1)__attribute__((always_inline));

static inline void bfly2_tw1(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1) {

  *y0  = _mm_adds_epi16(*x0,*x1);
  *y1  = _mm_subs_epi16(*x0,*x1);

}

static inline void bfly2_16(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1, __m128i *tw, __m128i *twb)__attribute__((always_inline));

static inline void bfly2_16(__m128i *x0, __m128i *x1, __m128i *y0, __m128i *y1, __m128i *tw, __m128i *twb) {

  register __m128i x1t;

  x1t = packed_cmult2(*(x1),*(tw),*(twb));
  
  *y0  = _mm_adds_epi16(*x0,x1t);
  *y1  = _mm_subs_epi16(*x0,x1t);

}


static inline void ibfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw)__attribute__((always_inline));

static inline void ibfly2(__m128i *x0, __m128i *x1,__m128i *y0, __m128i *y1,__m128i *tw){

  __m128i x0r_2,x0i_2,x1r_2,x1i_2,dy0r,dy1r,dy0i,dy1i;
  __m128i bfly2_tmp1,bfly2_tmp2;

  cmultc(*(x0),*(W0),&x0r_2,&x0i_2);
  cmultc(*(x1),*(tw),&x1r_2,&x1i_2);

  dy0r = _mm_srai_epi32(_mm_add_epi32(x0r_2,x1r_2),15);
  dy1r = _mm_srai_epi32(_mm_sub_epi32(x0r_2,x1r_2),15);
  dy0i = _mm_srai_epi32(_mm_add_epi32(x0i_2,x1i_2),15);
  //  printf("y0i %d\n",((int16_t *)y0i)[0]);
  dy1i = _mm_srai_epi32(_mm_sub_epi32(x0i_2,x1i_2),15);
  
  bfly2_tmp1 = _mm_unpacklo_epi32(dy0r,dy0i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy0r,dy0i);
  *y0 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);

  bfly2_tmp1 = _mm_unpacklo_epi32(dy1r,dy1i);
  bfly2_tmp2 = _mm_unpackhi_epi32(dy1r,dy1i);
  *y1 = _mm_packs_epi32(bfly2_tmp1,bfly2_tmp2);
}



// This is the radix-3 butterfly (fft)
static inline void bfly3(__m128i *x0,__m128i *x1,__m128i *x2,
			 __m128i *y0,__m128i *y1,__m128i *y2,
			 __m128i *tw1,__m128i *tw2) __attribute__((always_inline));

static inline void bfly3(__m128i *x0,__m128i *x1,__m128i *x2,
			 __m128i *y0,__m128i *y1,__m128i *y2,
			 __m128i *tw1,__m128i *tw2)  { 

  __m128i tmpre,tmpim,x1_2,x2_2;

  packed_cmult(*(x1),*(tw1),&x1_2); 
  packed_cmult(*(x2),*(tw2),&x2_2); 
  *(y0)  = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1_2,x2_2)); 
  cmult(x1_2,*(W13),&tmpre,&tmpim); 
  cmac(x2_2,*(W23),&tmpre,&tmpim);  
  *(y1) = cpack(tmpre,tmpim); 
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(x1_2,*(W23),&tmpre,&tmpim); 
  cmac(x2_2,*(W13),&tmpre,&tmpim);  
  *(y2) = cpack(tmpre,tmpim); 
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
}

static inline void bfly3_tw1(__m128i *x0,__m128i *x1,__m128i *x2,
			     __m128i *y0,__m128i *y1,__m128i *y2) __attribute__((always_inline));

static inline void bfly3_tw1(__m128i *x0,__m128i *x1,__m128i *x2,
			     __m128i *y0,__m128i *y1,__m128i *y2) {

  __m128i tmpre,tmpim;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),*(x2)));	
  cmult(*(x1),*(W13),&tmpre,&tmpim); 
  cmac(*(x2),*(W23),&tmpre,&tmpim);  
  *(y1) = cpack(tmpre,tmpim); 
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(*(x1),*(W23),&tmpre,&tmpim); 
  cmac(*(x2),*(W13),&tmpre,&tmpim);  
  *(y2) = cpack(tmpre,tmpim); 
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
}


static inline void bfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			 __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			 __m128i *tw1,__m128i *tw2,__m128i *tw3)__attribute__((always_inline));

static inline void bfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			 __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			 __m128i *tw1,__m128i *tw2,__m128i *tw3) {   

  __m128i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;

  //  cmult(*(x0),*(W0),&x0r_2,&x0i_2);		
  cmult(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmult(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmult(*(x3),*(tw3),&x3r_2,&x3i_2);
  //  dy0r = _mm_add_epi32(x0r_2,_mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2)));
  //  dy0i = _mm_add_epi32(x0i_2,_mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2)));
  //  *(y0)  = cpack(dy0r,dy0i);
  dy0r = _mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2));
  dy0i = _mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm_add_epi16(*(x0),cpack(dy0r,dy0i));
  //  dy1r = _mm_add_epi32(x0r_2,_mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2)));
  //  dy1i = _mm_sub_epi32(x0i_2,_mm_add_epi32(x1r_2,_mm_sub_epi32(x2i_2,x3r_2)));
  //  *(y1)  = cpack(dy1r,dy1i);
  dy1r = _mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2));
  dy1i = _mm_sub_epi32(_mm_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y1)  = _mm_add_epi16(*(x0),cpack(dy1r,dy1i));
  //  dy2r = _mm_sub_epi32(x0r_2,_mm_sub_epi32(x1r_2,_mm_sub_epi32(x2r_2,x3r_2)));
  //  dy2i = _mm_sub_epi32(x0i_2,_mm_sub_epi32(x1i_2,_mm_sub_epi32(x2i_2,x3i_2)));
  //  *(y2)  = cpack(dy2r,dy2i);
  dy2r = _mm_sub_epi32(_mm_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm_sub_epi32(_mm_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm_add_epi16(*(x0),cpack(dy2r,dy2i));
  //  dy3r = _mm_sub_epi32(x0r_2,_mm_add_epi32(x1i_2,_mm_sub_epi32(x2r_2,x3i_2)));
  //  dy3i = _mm_add_epi32(x0i_2,_mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2)));
  //  *(y3) = cpack(dy3r,dy3i);
  dy3r = _mm_sub_epi32(_mm_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy3i = _mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2));
  *(y3) = _mm_add_epi16(*(x0),cpack(dy3r,dy3i));
}

static inline void ibfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			  __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			  __m128i *tw1,__m128i *tw2,__m128i *tw3)__attribute__((always_inline));
   
static inline void ibfly4(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			  __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			  __m128i *tw1,__m128i *tw2,__m128i *tw3) {   

  __m128i x1r_2,x1i_2,x2r_2,x2i_2,x3r_2,x3i_2,dy0r,dy0i,dy1r,dy1i,dy2r,dy2i,dy3r,dy3i;

  //  cmultc(*(x0),*(W0),&x0r_2,&x0i_2);		
  cmultc(*(x1),*(tw1),&x1r_2,&x1i_2);
  cmultc(*(x2),*(tw2),&x2r_2,&x2i_2);
  cmultc(*(x3),*(tw3),&x3r_2,&x3i_2);
  /*
  dy0r = _mm_add_epi32(x0r_2,_mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2)));
  dy0i = _mm_add_epi32(x0i_2,_mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2)));
  *(y0)  = cpack(dy0r,dy0i);
  dy3r = _mm_add_epi32(x0r_2,_mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2)));
  dy3i = _mm_sub_epi32(x0i_2,_mm_add_epi32(x1r_2,_mm_sub_epi32(x2i_2,x3r_2)));
  *(y3)  = cpack(dy3r,dy3i);
  dy2r = _mm_sub_epi32(x0r_2,_mm_sub_epi32(x1r_2,_mm_sub_epi32(x2r_2,x3r_2)));
  dy2i = _mm_sub_epi32(x0i_2,_mm_sub_epi32(x1i_2,_mm_sub_epi32(x2i_2,x3i_2)));
  *(y2)  = cpack(dy2r,dy2i);
  dy1r = _mm_sub_epi32(x0r_2,_mm_add_epi32(x1i_2,_mm_sub_epi32(x2r_2,x3i_2)));
  dy1i = _mm_add_epi32(x0i_2,_mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2)));
  *(y1) = cpack(dy1r,dy1i);
  */
  dy0r = _mm_add_epi32(x1r_2,_mm_add_epi32(x2r_2,x3r_2));
  dy0i = _mm_add_epi32(x1i_2,_mm_add_epi32(x2i_2,x3i_2));
  *(y0)  = _mm_add_epi16(*(x0),cpack(dy0r,dy0i));
  dy3r = _mm_sub_epi32(x1i_2,_mm_add_epi32(x2r_2,x3i_2));
  dy3i = _mm_sub_epi32(_mm_sub_epi32(x3r_2,x2i_2),x1r_2);
  *(y3)  = _mm_add_epi16(*(x0),cpack(dy3r,dy3i));
  dy2r = _mm_sub_epi32(_mm_sub_epi32(x2r_2,x3r_2),x1r_2);
  dy2i = _mm_sub_epi32(_mm_sub_epi32(x2i_2,x3i_2),x1i_2);
  *(y2)  = _mm_add_epi16(*(x0),cpack(dy2r,dy2i));
  dy1r = _mm_sub_epi32(_mm_sub_epi32(x3i_2,x2r_2),x1i_2);
  dy1i = _mm_sub_epi32(x1r_2,_mm_add_epi32(x2i_2,x3r_2));
  *(y1) = _mm_add_epi16(*(x0),cpack(dy1r,dy1i));
}


static inline void bfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			     __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3)__attribute__((always_inline));

static inline void bfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			     __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3) { 

  register __m128i x1_flip,x3_flip;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),_mm_adds_epi16(*(x2),*(x3)))); 

  x1_flip = _mm_sign_epi16(*(x1),*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(*(x3),*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(*(x2),x3_flip)));
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(*(x1),_mm_subs_epi16(*(x2),*(x3))));
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(*(x2),x3_flip)));
}

static inline void ibfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			      __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3)__attribute__((always_inline)); 

static inline void ibfly4_tw1(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			      __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3) { 
  
  register __m128i x1_flip,x3_flip;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),_mm_adds_epi16(*(x2),*(x3)))); 

  x1_flip = _mm_sign_epi16(*(x1),*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(*(x3),*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  *(y1)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(*(x2),x3_flip)));
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(*(x1),_mm_subs_epi16(*(x2),*(x3))));
  *(y3)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(*(x2),x3_flip)));
}

static inline void bfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			    __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			    __m128i *tw1,__m128i *tw2,__m128i *tw3,
			    __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)__attribute__((always_inline));

static inline void bfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			    __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			    __m128i *tw1,__m128i *tw2,__m128i *tw3,
			    __m128i *tw1b,__m128i *tw2b,__m128i *tw3b) {   

  register __m128i x1t,x2t,x3t,x02t,x13t;
  register __m128i x1_flip,x3_flip;

  x1t = packed_cmult2(*(x1),*(tw1),*(tw1b));
  x2t = packed_cmult2(*(x2),*(tw2),*(tw2b));
  x3t = packed_cmult2(*(x3),*(tw3),*(tw3b));


  //  bfly4_tw1(x0,&x1t,&x2t,&x3t,y0,y1,y2,y3);
  x02t  = _mm_adds_epi16(*(x0),x2t);
  x13t  = _mm_adds_epi16(x1t,x3t);
  /*
  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1t,_mm_adds_epi16(x2t,x3t))); 
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(x1t,_mm_subs_epi16(x2t,x3t)));
  */
  *(y0)   = _mm_adds_epi16(x02t,x13t);
  *(y2)   = _mm_subs_epi16(x02t,x13t);

  x1_flip = _mm_sign_epi16(x1t,*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(x3t,*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t  = _mm_subs_epi16(*(x0),x2t);
  x13t  = _mm_subs_epi16(x1_flip,x3_flip);
  /*
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(x2t,x3_flip)));  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(x2t,x3_flip)));  // x0 - x1f - x2 + x3f
  */
  *(y1)   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

}

static inline void ibfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			     __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			     __m128i *tw1,__m128i *tw2,__m128i *tw3,
			     __m128i *tw1b,__m128i *tw2b,__m128i *tw3b)__attribute__((always_inline));

static inline void ibfly4_16(__m128i *x0,__m128i *x1,__m128i *x2,__m128i *x3,
			     __m128i *y0,__m128i *y1,__m128i *y2,__m128i *y3,
			     __m128i *tw1,__m128i *tw2,__m128i *tw3,
			     __m128i *tw1b,__m128i *tw2b,__m128i *tw3b) {   
  
  register __m128i x1t,x2t,x3t,x02t,x13t;
  register __m128i x1_flip,x3_flip;

  x1t = packed_cmult2(*(x1),*(tw1),*(tw1b));
  x2t = packed_cmult2(*(x2),*(tw2),*(tw2b));
  x3t = packed_cmult2(*(x3),*(tw3),*(tw3b));


  //  bfly4_tw1(x0,&x1t,&x2t,&x3t,y0,y1,y2,y3);
  x02t  = _mm_adds_epi16(*(x0),x2t);
  x13t  = _mm_adds_epi16(x1t,x3t);
  /*
  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1t,_mm_adds_epi16(x2t,x3t))); 
  *(y2)   = _mm_subs_epi16(*(x0),_mm_subs_epi16(x1t,_mm_subs_epi16(x2t,x3t)));
  */
  *(y0)   = _mm_adds_epi16(x02t,x13t);
  *(y2)   = _mm_subs_epi16(x02t,x13t);

  x1_flip = _mm_sign_epi16(x1t,*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(x3t,*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t  = _mm_subs_epi16(*(x0),x2t);
  x13t  = _mm_subs_epi16(x1_flip,x3_flip);
  /*
  *(y1)   = _mm_adds_epi16(*(x0),_mm_subs_epi16(x1_flip,_mm_adds_epi16(x2t,x3_flip)));  // x0 + x1f - x2 - x3f
  *(y3)   = _mm_subs_epi16(*(x0),_mm_adds_epi16(x1_flip,_mm_subs_epi16(x2t,x3_flip)));  // x0 - x1f - x2 + x3f
  */
  *(y3)   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  *(y1)   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f

}

static inline void bfly5(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
			 __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4,
			 __m128i *tw1,__m128i *tw2,__m128i *tw3,__m128i *tw4)__attribute__((always_inline));

static inline void bfly5(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
			 __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4,
			 __m128i *tw1,__m128i *tw2,__m128i *tw3,__m128i *tw4) {



  __m128i x1_2,x2_2,x3_2,x4_2,tmpre,tmpim;

  packed_cmult(*(x1),*(tw1),&x1_2);
  packed_cmult(*(x2),*(tw2),&x2_2);
  packed_cmult(*(x3),*(tw3),&x3_2);
  packed_cmult(*(x4),*(tw4),&x4_2);

  *(y0)  = _mm_adds_epi16(*(x0),_mm_adds_epi16(x1_2,_mm_adds_epi16(x2_2,_mm_adds_epi16(x3_2,x4_2))));
  cmult(x1_2,*(W15),&tmpre,&tmpim); 
  cmac(x2_2,*(W25),&tmpre,&tmpim);  
  cmac(x3_2,*(W35),&tmpre,&tmpim);  
  cmac(x4_2,*(W45),&tmpre,&tmpim);  
  *(y1) = cpack(tmpre,tmpim); 
  *(y1) = _mm_adds_epi16(*(x0),*(y1));

  cmult(x1_2,*(W25),&tmpre,&tmpim); 
  cmac(x2_2,*(W45),&tmpre,&tmpim);  
  cmac(x3_2,*(W15),&tmpre,&tmpim);  
  cmac(x4_2,*(W35),&tmpre,&tmpim);  
  *(y2) = cpack(tmpre,tmpim); 
  *(y2) = _mm_adds_epi16(*(x0),*(y2));

  cmult(x1_2,*(W35),&tmpre,&tmpim); 
  cmac(x2_2,*(W15),&tmpre,&tmpim);  
  cmac(x3_2,*(W45),&tmpre,&tmpim);  
  cmac(x4_2,*(W25),&tmpre,&tmpim);  
  *(y3) = cpack(tmpre,tmpim); 
  *(y3) = _mm_adds_epi16(*(x0),*(y3));

  cmult(x1_2,*(W45),&tmpre,&tmpim); 
  cmac(x2_2,*(W35),&tmpre,&tmpim);  
  cmac(x3_2,*(W25),&tmpre,&tmpim);  
  cmac(x4_2,*(W15),&tmpre,&tmpim);  
  *(y4) = cpack(tmpre,tmpim); 
  *(y4) = _mm_adds_epi16(*(x0),*(y4));


}  



static inline void bfly5_tw1(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
			     __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4) __attribute__((always_inline));

static inline void bfly5_tw1(__m128i *x0, __m128i *x1, __m128i *x2, __m128i *x3,__m128i *x4,
			     __m128i *y0, __m128i *y1, __m128i *y2, __m128i *y3,__m128i *y4) {

  __m128i tmpre,tmpim;

  *(y0) = _mm_adds_epi16(*(x0),_mm_adds_epi16(*(x1),_mm_adds_epi16(*(x2),_mm_adds_epi16(*(x3),*(x4))))); 
  cmult(*(x1),*(W15),&tmpre,&tmpim);   
  cmac(*(x2),*(W25),&tmpre,&tmpim);  
  cmac(*(x3),*(W35),&tmpre,&tmpim);  
  cmac(*(x4),*(W45),&tmpre,&tmpim);  
  *(y1) = cpack(tmpre,tmpim); 
  *(y1) = _mm_adds_epi16(*(x0),*(y1));
  cmult(*(x1),*(W25),&tmpre,&tmpim); 
  cmac(*(x2),*(W45),&tmpre,&tmpim);  
  cmac(*(x3),*(W15),&tmpre,&tmpim);  
  cmac(*(x4),*(W35),&tmpre,&tmpim);  
  *(y2) = cpack(tmpre,tmpim); 
  *(y2) = _mm_adds_epi16(*(x0),*(y2));
  cmult(*(x1),*(W35),&tmpre,&tmpim); 
  cmac(*(x2),*(W15),&tmpre,&tmpim);  
  cmac(*(x3),*(W45),&tmpre,&tmpim);  
  cmac(*(x4),*(W25),&tmpre,&tmpim);  
  *(y3) = cpack(tmpre,tmpim); 
  *(y3) = _mm_adds_epi16(*(x0),*(y3));
  cmult(*(x1),*(W45),&tmpre,&tmpim); 
  cmac(*(x2),*(W35),&tmpre,&tmpim);  
  cmac(*(x3),*(W25),&tmpre,&tmpim);  
  cmac(*(x4),*(W15),&tmpre,&tmpim);  
  *(y4) = cpack(tmpre,tmpim); 
  *(y4) = _mm_adds_epi16(*(x0),*(y4));
}

// performs 4x4 transpose of input x (complex interleaved) using 128bit SIMD intrinsics
// i.e. x = [x0r x0i x1r x1i ... x15r x15i], y = [x0r x0i x4r x4i x8r x8i x12r x12i x1r x1i x5r x5i x9r x9i x13r x13i x2r x2i ... x15r x15i]

static inline void transpose16(__m128i *x,__m128i *y) __attribute__((always_inline));
static inline void transpose16(__m128i *x,__m128i *y) {
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3;

  ytmp0 = _mm_unpacklo_epi32(x[0],x[1]);
  ytmp1 = _mm_unpackhi_epi32(x[0],x[1]);
  ytmp2 = _mm_unpacklo_epi32(x[2],x[3]);
  ytmp3 = _mm_unpackhi_epi32(x[2],x[3]);
  y[0]    = _mm_unpacklo_epi64(ytmp0,ytmp2);
  y[1]    = _mm_unpackhi_epi64(ytmp0,ytmp2);
  y[2]    = _mm_unpacklo_epi64(ytmp1,ytmp3);
  y[3]    = _mm_unpackhi_epi64(ytmp1,ytmp3);
}

// same as above but output is offset by off
static inline void transpose16_ooff(__m128i *x,__m128i *y,int off) __attribute__((always_inline));

static inline void transpose16_ooff(__m128i *x,__m128i *y,int off) {
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3;
  __m128i *y2=y;

  ytmp0 = _mm_unpacklo_epi32(x[0],x[1]);
  ytmp1 = _mm_unpackhi_epi32(x[0],x[1]);
  ytmp2 = _mm_unpacklo_epi32(x[2],x[3]);
  ytmp3 = _mm_unpackhi_epi32(x[2],x[3]);
  *y2     = _mm_unpacklo_epi64(ytmp0,ytmp2);y2+=off;
  *y2     = _mm_unpackhi_epi64(ytmp0,ytmp2);y2+=off;
  *y2     = _mm_unpacklo_epi64(ytmp1,ytmp3);y2+=off;
  *y2     = _mm_unpackhi_epi64(ytmp1,ytmp3);
}

static inline void transpose4_ooff(__m64 *x,__m64 *y,int off)__attribute__((always_inline));
static inline void transpose4_ooff(__m64 *x,__m64 *y,int off) {
  y[0]   = _mm_unpacklo_pi32(x[0],x[1]);
  y[off] = _mm_unpackhi_pi32(x[0],x[1]);
}

// 16-point optimized DFT kernel

int16_t tw16[24] __attribute__((aligned(16))) = { 32767,0,30272,-12540,23169 ,-23170,12539 ,-30273,
						  32767,0,23169,-23170,0     ,-32767,-23170,-23170,
						  32767,0,12539,-30273,-23170,-23170,-30273,12539};

int16_t tw16a[24] __attribute__((aligned(16))) = {32767,0,30272,12540,23169 ,23170,12539 ,30273,
						  32767,0,23169,23170,0     ,32767,-23170,23170,
						  32767,0,12539,30273,-23170,23170,-30273,-12539};

int16_t tw16b[24] __attribute__((aligned(16))) = { 0,32767,-12540,30272,-23170,23169 ,-30273,12539,
						   0,32767,-23170,23169,-32767,0     ,-23170,-23170,
						   0,32767,-30273,12539,-23170,-23170,12539 ,-30273};

int16_t tw16c[24] __attribute__((aligned(16))) = { 0,32767,12540,30272,23170,23169 ,30273 ,12539,
						   0,32767,23170,23169,32767,0     ,23170 ,-23170,
						   0,32767,30273,12539,23170,-23170,-12539,-30273};

static inline void dft16(int16_t *x,int16_t *y) __attribute__((always_inline));

static inline void dft16(int16_t *x,int16_t *y) {

  __m128i ytmp[4],*tw16a_128=(__m128i *)tw16a,*tw16b_128=(__m128i *)tw16b,*x128=(__m128i *)x,*y128=(__m128i *)y;
  
  /*
  bfly4_tw1(x128,x128+1,x128+2,x128+3,
	    y128,y128+1,y128+2,y128+3);

  transpose16(y128,ytmp);

  bfly4_16(ytmp,ytmp+1,ytmp+2,ytmp+3,
	   y128,y128+1,y128+2,y128+3,
	   tw16_128,tw16_128+1,tw16_128+2);
  */

  register __m128i x1_flip,x3_flip,x02t,x13t;
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;

  // First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm_adds_epi16(x128[0],x128[2]);
  x13t    = _mm_adds_epi16(x128[1],x128[3]);
  xtmp0   = _mm_adds_epi16(x02t,x13t);
  xtmp2   = _mm_subs_epi16(x02t,x13t);

  /*
  xtmp0   = _mm_adds_epi16(x128[0],_mm_adds_epi16(x128[1],_mm_adds_epi16(x128[2],x128[3]))); 
  xtmp2   = _mm_subs_epi16(x128[0],_mm_subs_epi16(x128[1],_mm_subs_epi16(x128[2],x128[3])));
  */
  x1_flip = _mm_sign_epi16(x128[1],*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(x128[3],*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t    = _mm_subs_epi16(x128[0],x128[2]);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  xtmp1   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp3   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
  /*
  xtmp1   = _mm_adds_epi16(x128[0],_mm_subs_epi16(x1_flip,_mm_adds_epi16(x128[2],x3_flip)));
  xtmp3   = _mm_subs_epi16(x128[0],_mm_adds_epi16(x1_flip,_mm_subs_epi16(x128[2],x3_flip)));
  */

  ytmp0   = _mm_unpacklo_epi32(xtmp0,xtmp1);
  ytmp1   = _mm_unpackhi_epi32(xtmp0,xtmp1);
  ytmp2   = _mm_unpacklo_epi32(xtmp2,xtmp3);
  ytmp3   = _mm_unpackhi_epi32(xtmp2,xtmp3);
  xtmp0   = _mm_unpacklo_epi64(ytmp0,ytmp2);
  xtmp1   = _mm_unpackhi_epi64(ytmp0,ytmp2);
  xtmp2   = _mm_unpacklo_epi64(ytmp1,ytmp3);
  xtmp3   = _mm_unpackhi_epi64(ytmp1,ytmp3);

  // Second stage : 4 Radix-4 butterflies with input twiddles
  xtmp1 = packed_cmult2(xtmp1,tw16a_128[0],tw16b_128[0]);
  xtmp2 = packed_cmult2(xtmp2,tw16a_128[1],tw16b_128[1]);
  xtmp3 = packed_cmult2(xtmp3,tw16a_128[2],tw16b_128[2]);

  x02t    = _mm_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm_adds_epi16(xtmp1,xtmp3);
  y128[0] = _mm_adds_epi16(x02t,x13t);
  y128[2] = _mm_subs_epi16(x02t,x13t);

  /*
  y128[0] = _mm_adds_epi16(xtmp0,_mm_adds_epi16(xtmp1,_mm_adds_epi16(xtmp2,xtmp3))); 
  y128[2] = _mm_subs_epi16(xtmp0,_mm_subs_epi16(xtmp1,_mm_subs_epi16(xtmp2,xtmp3)));
  */

  x1_flip = _mm_sign_epi16(xtmp1,*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(xtmp3,*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t    = _mm_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  y128[1] = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  y128[3] = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
  /*
  y128[1] = _mm_adds_epi16(xtmp0,_mm_subs_epi16(x1_flip,_mm_adds_epi16(xtmp2,x3_flip)));
  y128[3] = _mm_subs_epi16(xtmp0,_mm_adds_epi16(x1_flip,_mm_subs_epi16(xtmp2,x3_flip)));
  */
}

static inline void idft16(int16_t *x,int16_t *y) __attribute__((always_inline));

static inline void idft16(int16_t *x,int16_t *y) {

  __m128i ytmp[4],*tw16a_128=(__m128i *)tw16,*tw16b_128=(__m128i *)tw16c,*x128=(__m128i *)x,*y128=(__m128i *)y;
  
  /*
  bfly4_tw1(x128,x128+1,x128+2,x128+3,
	    y128,y128+1,y128+2,y128+3);

  transpose16(y128,ytmp);

  bfly4_16(ytmp,ytmp+1,ytmp+2,ytmp+3,
	   y128,y128+1,y128+2,y128+3,
	   tw16_128,tw16_128+1,tw16_128+2);
  */

  register __m128i x1_flip,x3_flip,x02t,x13t;
  register __m128i ytmp0,ytmp1,ytmp2,ytmp3,xtmp0,xtmp1,xtmp2,xtmp3;

  // First stage : 4 Radix-4 butterflies without input twiddles

  x02t    = _mm_adds_epi16(x128[0],x128[2]);
  x13t    = _mm_adds_epi16(x128[1],x128[3]);
  xtmp0   = _mm_adds_epi16(x02t,x13t);
  xtmp2   = _mm_subs_epi16(x02t,x13t);

  /*
  xtmp0   = _mm_adds_epi16(x128[0],_mm_adds_epi16(x128[1],_mm_adds_epi16(x128[2],x128[3]))); 
  xtmp2   = _mm_subs_epi16(x128[0],_mm_subs_epi16(x128[1],_mm_subs_epi16(x128[2],x128[3])));
  */
  x1_flip = _mm_sign_epi16(x128[1],*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(x128[3],*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t    = _mm_subs_epi16(x128[0],x128[2]);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  xtmp3   = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  xtmp1   = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
  /*
  xtmp1   = _mm_adds_epi16(x128[0],_mm_subs_epi16(x1_flip,_mm_adds_epi16(x128[2],x3_flip)));
  xtmp3   = _mm_subs_epi16(x128[0],_mm_adds_epi16(x1_flip,_mm_subs_epi16(x128[2],x3_flip)));
  */

  ytmp0   = _mm_unpacklo_epi32(xtmp0,xtmp1);
  ytmp1   = _mm_unpackhi_epi32(xtmp0,xtmp1);
  ytmp2   = _mm_unpacklo_epi32(xtmp2,xtmp3);
  ytmp3   = _mm_unpackhi_epi32(xtmp2,xtmp3);
  xtmp0   = _mm_unpacklo_epi64(ytmp0,ytmp2);
  xtmp1   = _mm_unpackhi_epi64(ytmp0,ytmp2);
  xtmp2   = _mm_unpacklo_epi64(ytmp1,ytmp3);
  xtmp3   = _mm_unpackhi_epi64(ytmp1,ytmp3);

  // Second stage : 4 Radix-4 butterflies with input twiddles
  xtmp1 = packed_cmult2(xtmp1,tw16a_128[0],tw16b_128[0]);
  xtmp2 = packed_cmult2(xtmp2,tw16a_128[1],tw16b_128[1]);
  xtmp3 = packed_cmult2(xtmp3,tw16a_128[2],tw16b_128[2]);

  x02t    = _mm_adds_epi16(xtmp0,xtmp2);
  x13t    = _mm_adds_epi16(xtmp1,xtmp3);
  y128[0] = _mm_adds_epi16(x02t,x13t);
  y128[2] = _mm_subs_epi16(x02t,x13t);

  /*
  y128[0] = _mm_adds_epi16(xtmp0,_mm_adds_epi16(xtmp1,_mm_adds_epi16(xtmp2,xtmp3))); 
  y128[2] = _mm_subs_epi16(xtmp0,_mm_subs_epi16(xtmp1,_mm_subs_epi16(xtmp2,xtmp3)));
  */

  x1_flip = _mm_sign_epi16(xtmp1,*(__m128i*)conjugatedft);
  x1_flip = _mm_shufflelo_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x1_flip = _mm_shufflehi_epi16(x1_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_sign_epi16(xtmp3,*(__m128i*)conjugatedft);
  x3_flip = _mm_shufflelo_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));
  x3_flip = _mm_shufflehi_epi16(x3_flip,_MM_SHUFFLE(2,3,0,1));

  x02t    = _mm_subs_epi16(xtmp0,xtmp2);
  x13t    = _mm_subs_epi16(x1_flip,x3_flip);
  y128[3] = _mm_adds_epi16(x02t,x13t);  // x0 + x1f - x2 - x3f
  y128[1] = _mm_subs_epi16(x02t,x13t);  // x0 - x1f - x2 + x3f
  /*
  y128[1] = _mm_adds_epi16(xtmp0,_mm_subs_epi16(x1_flip,_mm_adds_epi16(xtmp2,x3_flip)));
  y128[3] = _mm_subs_epi16(xtmp0,_mm_adds_epi16(x1_flip,_mm_subs_epi16(xtmp2,x3_flip)));
  */
}

/*
static inline void idft16(int16_t *x,int16_t *y)__attribute__((always_inline)); 

static inline void idft16(int16_t *x,int16_t *y) {

  __m128i ytmp[4],*tw16_128=(__m128i *)tw16,*x128=(__m128i *)x,*y128=(__m128i *)y;


  ibfly4_tw1(x128,x128+1,x128+2,x128+3,
	    y128,y128+1,y128+2,y128+3);

  transpose16(y128,ytmp);

  ibfly4(ytmp,ytmp+1,ytmp+2,ytmp+3,
	 y128,y128+1,y128+2,y128+3,
	 tw16_128,tw16_128+1,tw16_128+2);

}
*/



// 64-point optimized DFT kernel

int16_t tw64[96] __attribute__((aligned(16))) = { 32767,0,32609,-3212,32137,-6393,31356,-9512,30272,-12540,28897,-15447,27244,-18205,25329,-20788,23169,-23170,20787,-25330,18204,-27245,15446,-28898,12539,-30273,9511,-31357,6392,-32138,3211,-32610,
						  32767,0,32137,-6393,30272,-12540,27244,-18205,23169,-23170,18204,-27245,12539,-30273,6392,-32138,0,-32767,-6393,-32138,-12540,-30273,-18205,-27245,-23170,-23170,-27245,-18205,-30273,-12540,-32138,-6393,
						  32767,0,31356,-9512,27244,-18205,20787,-25330,12539,-30273,3211,-32610,-6393,-32138,-15447,-28898,-23170,-23170,-28898,-15447,-32138,-6393,-32610,3211,-30273,12539,-25330,20787,-18205,27244,-9512,31356};

int16_t tw64a[96] __attribute__((aligned(16))) = { 32767,0,32609,3212,32137,6393,31356,9512,30272,12540,28897,15447,27244,18205,25329,20788,23169,23170,20787,25330,18204,27245,15446,28898,12539,30273,9511,31357,6392,32138,3211,32610,
						   32767,0,32137,6393,30272,12540,27244,18205,23169,23170,18204,27245,12539,30273,6392,32138,0,32767,-6393,32138,-12540,30273,-18205,27245,-23170,23170,-27245,18205,-30273,12540,-32138,6393,
						   32767,0,31356,9512,27244,18205,20787,25330,12539,30273,3211,32610,-6393,32138,-15447,28898,-23170,23170,-28898,15447,-32138,6393,-32610,-3211,-30273,-12539,-25330,-20787,-18205,-27244,-9512,-31356};

int16_t tw64b[96] __attribute__((aligned(16))) = { 0,32767,-3212,32609,-6393,32137,-9512,31356,-12540,30272,-15447,28897,-18205,27244,-20788,25329,-23170,23169,-25330,20787,-27245,18204,-28898,15446,-30273,12539,-31357,9511,-32138,6392,-32610,3211,
						   0,32767,-6393,32137,-12540,30272,-18205,27244,-23170,23169,-27245,18204,-30273,12539,-32138,6392,-32767,0,-32138,-6393,-30273,-12540,-27245,-18205,-23170,-23170,-18205,-27245,-12540,-30273,-6393,-32138,
						   0,32767,-9512,31356,-18205,27244,-25330,20787,-30273,12539,-32610,3211,-32138,-6393,-28898,-15447,-23170,-23170,-15447,-28898,-6393,-32138,3211,-32610,12539,-30273,20787,-25330,27244,-18205,31356,-9512};

int16_t tw64c[96] __attribute__((aligned(16))) = { 0,32767,3212,32609,6393,32137,9512,31356,12540,30272,15447,28897,18205,27244,20788,25329,23170,23169,25330,20787,27245,18204,28898,15446,30273,12539,31357,9511,32138,6392,32610,3211,
						   0,32767,6393,32137,12540,30272,18205,27244,23170,23169,27245,18204,30273,12539,32138,6392,32767,0,32138,-6393,30273,-12540,27245,-18205,23170,-23170,18205,-27245,12540,-30273,6393,-32138,
						   0,32767,9512,31356,18205,27244,25330,20787,30273,12539,32610,3211,32138,-6393,28898,-15447,23170,-23170,15447,-28898,6393,-32138,-3211,-32610,-12539,-30273,-20787,-25330,-27244,-18205,-31356,-9512};



void dft64(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[16],ytmp[16],*tw64a_128=(__m128i *)tw64a,*tw64b_128=(__m128i *)tw64b,*x128=(__m128i *)x,*y128=(__m128i *)y;

  
#ifdef D64STATS
  time_stats_t ts_t,ts_d,ts_b;

  reset_meas(&ts_t);
  reset_meas(&ts_d);
  reset_meas(&ts_b);
  start_meas(&ts_t);
#endif
  

  transpose16_ooff(x128,xtmp,4);
  transpose16_ooff(x128+4,xtmp+1,4);
  transpose16_ooff(x128+8,xtmp+2,4);
  transpose16_ooff(x128+12,xtmp+3,4);

  
#ifdef D64STATS
  stop_meas(&ts_t);
  start_meas(&ts_d);
#endif
  

  dft16((int16_t*)(xtmp),(int16_t*)ytmp);
  dft16((int16_t*)(xtmp+4),(int16_t*)(ytmp+4));
  dft16((int16_t*)(xtmp+8),(int16_t*)(ytmp+8));
  dft16((int16_t*)(xtmp+12),(int16_t*)(ytmp+12));

    
#ifdef D64STATS
  stop_meas(&ts_d);
  start_meas(&ts_b);
#endif
  

  bfly4_16(ytmp,ytmp+4,ytmp+8,ytmp+12,
	   y128,y128+4,y128+8,y128+12,
	   tw64a_128,tw64a_128+4,tw64a_128+8,
	   tw64b_128,tw64b_128+4,tw64b_128+8);

  bfly4_16(ytmp+1,ytmp+5,ytmp+9,ytmp+13,
	   y128+1,y128+5,y128+9,y128+13,
	   tw64a_128+1,tw64a_128+5,tw64a_128+9,
	   tw64b_128+1,tw64b_128+5,tw64b_128+9);

  bfly4_16(ytmp+2,ytmp+6,ytmp+10,ytmp+14,
	   y128+2,y128+6,y128+10,y128+14,
	   tw64a_128+2,tw64a_128+6,tw64a_128+10,
	   tw64b_128+2,tw64b_128+6,tw64b_128+10);

  bfly4_16(ytmp+3,ytmp+7,ytmp+11,ytmp+15,
	   y128+3,y128+7,y128+11,y128+15,
	   tw64a_128+3,tw64a_128+7,tw64a_128+11,
	   tw64b_128+3,tw64b_128+7,tw64b_128+11);
   
#ifdef D64STATS
  stop_meas(&ts_b);
  printf("t: %llu cycles, d: %llu cycles, b: %llu cycles\n",ts_t.diff,ts_d.diff,ts_b.diff);
#endif
  

  if (scale>0) {

    y128[0]  = _mm_srai_epi16(y128[0],3);
    y128[1]  = _mm_srai_epi16(y128[1],3);
    y128[2]  = _mm_srai_epi16(y128[2],3);
    y128[3]  = _mm_srai_epi16(y128[3],3);
    y128[4]  = _mm_srai_epi16(y128[4],3);
    y128[5]  = _mm_srai_epi16(y128[5],3);
    y128[6]  = _mm_srai_epi16(y128[6],3);
    y128[7]  = _mm_srai_epi16(y128[7],3);
    y128[8]  = _mm_srai_epi16(y128[8],3);
    y128[9]  = _mm_srai_epi16(y128[9],3);
    y128[10] = _mm_srai_epi16(y128[10],3);
    y128[11] = _mm_srai_epi16(y128[11],3);
    y128[12] = _mm_srai_epi16(y128[12],3);
    y128[13] = _mm_srai_epi16(y128[13],3);
    y128[14] = _mm_srai_epi16(y128[14],3);
    y128[15] = _mm_srai_epi16(y128[15],3);

  }
  _mm_empty();
  _m_empty();

}

void idft64(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[16],ytmp[16],*tw64a_128=(__m128i *)tw64,*tw64b_128=(__m128i *)tw64c,*x128=(__m128i *)x,*y128=(__m128i *)y;

  
#ifdef D64STATS
  time_stats_t ts_t,ts_d,ts_b;

  reset_meas(&ts_t);
  reset_meas(&ts_d);
  reset_meas(&ts_b);
  start_meas(&ts_t);
#endif
  

  transpose16_ooff(x128,xtmp,4);
  transpose16_ooff(x128+4,xtmp+1,4);
  transpose16_ooff(x128+8,xtmp+2,4);
  transpose16_ooff(x128+12,xtmp+3,4);

  
#ifdef D64STATS
  stop_meas(&ts_t);
  start_meas(&ts_d);
#endif
  

  idft16((int16_t*)(xtmp),(int16_t*)ytmp);
  idft16((int16_t*)(xtmp+4),(int16_t*)(ytmp+4));
  idft16((int16_t*)(xtmp+8),(int16_t*)(ytmp+8));
  idft16((int16_t*)(xtmp+12),(int16_t*)(ytmp+12));

    
#ifdef D64STATS
  stop_meas(&ts_d);
  start_meas(&ts_b);
#endif
  

  ibfly4_16(ytmp,ytmp+4,ytmp+8,ytmp+12,
	    y128,y128+4,y128+8,y128+12,
	    tw64a_128,tw64a_128+4,tw64a_128+8,
	    tw64b_128,tw64b_128+4,tw64b_128+8);

  ibfly4_16(ytmp+1,ytmp+5,ytmp+9,ytmp+13,
	    y128+1,y128+5,y128+9,y128+13,
	    tw64a_128+1,tw64a_128+5,tw64a_128+9,
	    tw64b_128+1,tw64b_128+5,tw64b_128+9);
  
  ibfly4_16(ytmp+2,ytmp+6,ytmp+10,ytmp+14,
	    y128+2,y128+6,y128+10,y128+14,
	    tw64a_128+2,tw64a_128+6,tw64a_128+10,
	    tw64b_128+2,tw64b_128+6,tw64b_128+10);
  
  ibfly4_16(ytmp+3,ytmp+7,ytmp+11,ytmp+15,
	    y128+3,y128+7,y128+11,y128+15,
	    tw64a_128+3,tw64a_128+7,tw64a_128+11,
	    tw64b_128+3,tw64b_128+7,tw64b_128+11);
   
#ifdef D64STATS
  stop_meas(&ts_b);
  printf("t: %llu cycles, d: %llu cycles, b: %llu cycles\n",ts_t.diff,ts_d.diff,ts_b.diff);
#endif
  

  if (scale>0) {

    y128[0]  = _mm_srai_epi16(y128[0],3);
    y128[1]  = _mm_srai_epi16(y128[1],3);
    y128[2]  = _mm_srai_epi16(y128[2],3);
    y128[3]  = _mm_srai_epi16(y128[3],3);
    y128[4]  = _mm_srai_epi16(y128[4],3);
    y128[5]  = _mm_srai_epi16(y128[5],3);
    y128[6]  = _mm_srai_epi16(y128[6],3);
    y128[7]  = _mm_srai_epi16(y128[7],3);
    y128[8]  = _mm_srai_epi16(y128[8],3);
    y128[9]  = _mm_srai_epi16(y128[9],3);
    y128[10] = _mm_srai_epi16(y128[10],3);
    y128[11] = _mm_srai_epi16(y128[11],3);
    y128[12] = _mm_srai_epi16(y128[12],3);
    y128[13] = _mm_srai_epi16(y128[13],3);
    y128[14] = _mm_srai_epi16(y128[14],3);
    y128[15] = _mm_srai_epi16(y128[15],3);

  }
  _mm_empty();
  _m_empty();

}


/*
void idft64(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[16],ytmp[16],*tw64_128=(__m128i *)tw64,*x128=(__m128i *)x,*y128=(__m128i *)y;

  transpose16_ooff(x128,xtmp,4);
  transpose16_ooff(x128+4,xtmp+1,4);
  transpose16_ooff(x128+8,xtmp+2,4);
  transpose16_ooff(x128+12,xtmp+3,4);
  
  idft16((int16_t*)(xtmp),(int16_t*)ytmp);
  idft16((int16_t*)(xtmp+4),(int16_t*)(ytmp+4));
  idft16((int16_t*)(xtmp+8),(int16_t*)(ytmp+8));
  idft16((int16_t*)(xtmp+12),(int16_t*)(ytmp+12));
  

  ibfly4(ytmp,ytmp+4,ytmp+8,ytmp+12,
	 y128,y128+4,y128+8,y128+12,
	 tw64_128,tw64_128+4,tw64_128+8);

  ibfly4(ytmp+1,ytmp+5,ytmp+9,ytmp+13,
	 y128+1,y128+5,y128+9,y128+13,
	 tw64_128+1,tw64_128+5,tw64_128+9);

  ibfly4(ytmp+2,ytmp+6,ytmp+10,ytmp+14,
	 y128+2,y128+6,y128+10,y128+14,
	 tw64_128+2,tw64_128+6,tw64_128+10);

  ibfly4(ytmp+3,ytmp+7,ytmp+11,ytmp+15,
	 y128+3,y128+7,y128+11,y128+15,
	 tw64_128+3,tw64_128+7,tw64_128+11);

  if (scale>0) {

    y128[0]  = _mm_srai_epi16(y128[0],3);
    y128[1]  = _mm_srai_epi16(y128[1],3);
    y128[2]  = _mm_srai_epi16(y128[2],3);
    y128[3]  = _mm_srai_epi16(y128[3],3);
    y128[4]  = _mm_srai_epi16(y128[4],3);
    y128[5]  = _mm_srai_epi16(y128[5],3);
    y128[6]  = _mm_srai_epi16(y128[6],3);
    y128[7]  = _mm_srai_epi16(y128[7],3);
    y128[8]  = _mm_srai_epi16(y128[8],3);
    y128[9]  = _mm_srai_epi16(y128[9],3);
    y128[10] = _mm_srai_epi16(y128[10],3);
    y128[11] = _mm_srai_epi16(y128[11],3);
    y128[12] = _mm_srai_epi16(y128[12],3);
    y128[13] = _mm_srai_epi16(y128[13],3);
    y128[14] = _mm_srai_epi16(y128[14],3);
    y128[15] = _mm_srai_epi16(y128[15],3);

  }
  _mm_empty();
  _m_empty();

}
*/

int16_t tw128[128] __attribute__((aligned(16))) = {  32767,0,32727,-1608,32609,-3212,32412,-4808,32137,-6393,31785,-7962,31356,-9512,30851,-11039,30272,-12540,29621,-14010,28897,-15447,28105,-16846,27244,-18205,26318,-19520,25329,-20788,24278,-22005,23169,-23170,22004,-24279,20787,-25330,19519,-26319,18204,-27245,16845,-28106,15446,-28898,14009,-29622,12539,-30273,11038,-30852,9511,-31357,7961,-31786,6392,-32138,4807,-32413,3211,-32610,1607,-32728,0,-32767,-1608,-32728,-3212,-32610,-4808,-32413,-6393,-32138,-7962,-31786,-9512,-31357,-11039,-30852,-12540,-30273,-14010,-29622,-15447,-28898,-16846,-28106,-18205,-27245,-19520,-26319,-20788,-25330,-22005,-24279,-23170,-23170,-24279,-22005,-25330,-20788,-26319,-19520,-27245,-18205,-28106,-16846,-28898,-15447,-29622,-14010,-30273,-12540,-30852,-11039,-31357,-9512,-31786,-7962,-32138,-6393,-32413,-4808,-32610,-3212,-32728,-1608};

int16_t tw128a[128] __attribute__((aligned(16))) = { 32767,0,32727,1608,32609,3212,32412,4808,32137,6393,31785,7962,31356,9512,30851,11039,30272,12540,29621,14010,28897,15447,28105,16846,27244,18205,26318,19520,25329,20788,24278,22005,23169,23170,22004,24279,20787,25330,19519,26319,18204,27245,16845,28106,15446,28898,14009,29622,12539,30273,11038,30852,9511,31357,7961,31786,6392,32138,4807,32413,3211,32610,1607,32728,0,32767,-1608,32728,-3212,32610,-4808,32413,-6393,32138,-7962,31786,-9512,31357,-11039,30852,-12540,30273,-14010,29622,-15447,28898,-16846,28106,-18205,27245,-19520,26319,-20788,25330,-22005,24279,-23170,23170,-24279,22005,-25330,20788,-26319,19520,-27245,18205,-28106,16846,-28898,15447,-29622,14010,-30273,12540,-30852,11039,-31357,9512,-31786,7962,-32138,6393,-32413,4808,-32610,3212,-32728,1608};

int16_t tw128b[128] __attribute__((aligned(16))) = {0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,-32767,0,-32728,-1608,-32610,-3212,-32413,-4808,-32138,-6393,-31786,-7962,-31357,-9512,-30852,-11039,-30273,-12540,-29622,-14010,-28898,-15447,-28106,-16846,-27245,-18205,-26319,-19520,-25330,-20788,-24279,-22005,-23170,-23170,-22005,-24279,-20788,-25330,-19520,-26319,-18205,-27245,-16846,-28106,-15447,-28898,-14010,-29622,-12540,-30273,-11039,-30852,-9512,-31357,-7962,-31786,-6393,-32138,-4808,-32413,-3212,-32610,-1608,-32728};

int16_t tw128c[128] __attribute__((aligned(16))) = {0,32767,1608,32727,3212,32609,4808,32412,6393,32137,7962,31785,9512,31356,11039,30851,12540,30272,14010,29621,15447,28897,16846,28105,18205,27244,19520,26318,20788,25329,22005,24278,23170,23169,24279,22004,25330,20787,26319,19519,27245,18204,28106,16845,28898,15446,29622,14009,30273,12539,30852,11038,31357,9511,31786,7961,32138,6392,32413,4807,32610,3211,32728,1607,32767,0,32728,-1608,32610,-3212,32413,-4808,32138,-6393,31786,-7962,31357,-9512,30852,-11039,30273,-12540,29622,-14010,28898,-15447,28106,-16846,27245,-18205,26319,-19520,25330,-20788,24279,-22005,23170,-23170,22005,-24279,20788,-25330,19520,-26319,18205,-27245,16846,-28106,15447,-28898,14010,-29622,12540,-30273,11039,-30852,9512,-31357,7962,-31786,6393,-32138,4808,-32413,3212,-32610,1608,-32728};

void dft128(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[64],*x64 = (__m64 *)x;
  __m128i ytmp[32],*tw128a_128p=(__m128i *)tw128a,*tw128b_128p=(__m128i *)tw128b,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  transpose4_ooff(x64  ,xtmp,32);
  transpose4_ooff(x64+2,xtmp+1,32);
  transpose4_ooff(x64+4,xtmp+2,32);
  transpose4_ooff(x64+6,xtmp+3,32);
  transpose4_ooff(x64+8,xtmp+4,32);
  transpose4_ooff(x64+10,xtmp+5,32);
  transpose4_ooff(x64+12,xtmp+6,32);
  transpose4_ooff(x64+14,xtmp+7,32);
  transpose4_ooff(x64+16,xtmp+8,32);
  transpose4_ooff(x64+18,xtmp+9,32);
  transpose4_ooff(x64+20,xtmp+10,32);
  transpose4_ooff(x64+22,xtmp+11,32);
  transpose4_ooff(x64+24,xtmp+12,32);
  transpose4_ooff(x64+26,xtmp+13,32);
  transpose4_ooff(x64+28,xtmp+14,32);
  transpose4_ooff(x64+30,xtmp+15,32);
  transpose4_ooff(x64+32,xtmp+16,32);
  transpose4_ooff(x64+34,xtmp+17,32);
  transpose4_ooff(x64+36,xtmp+18,32);
  transpose4_ooff(x64+38,xtmp+19,32);
  transpose4_ooff(x64+40,xtmp+20,32);
  transpose4_ooff(x64+42,xtmp+21,32);
  transpose4_ooff(x64+44,xtmp+22,32);
  transpose4_ooff(x64+46,xtmp+23,32);
  transpose4_ooff(x64+48,xtmp+24,32);
  transpose4_ooff(x64+50,xtmp+25,32);
  transpose4_ooff(x64+52,xtmp+26,32);
  transpose4_ooff(x64+54,xtmp+27,32);
  transpose4_ooff(x64+56,xtmp+28,32);
  transpose4_ooff(x64+58,xtmp+29,32);
  transpose4_ooff(x64+60,xtmp+30,32);
  transpose4_ooff(x64+62,xtmp+31,32);

  dft64((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft64((int16_t*)(xtmp+32),(int16_t*)(ytmp+16),1);


  for (i=0;i<16;i++) {
    bfly2_16(ytmpp,ytmpp+16,
	     y128p,y128p+16,
	     tw128a_128p,
	     tw128b_128p);
    tw128a_128p++;
    tw128b_128p++;
    y128p++;
    ytmpp++;    
  }

  if (scale>0) {

    y128[0]  = _mm_mulhi_epi16(y128[0],ONE_OVER_SQRT2_Q15_128);y128[0] = _mm_slli_epi16(y128[0],1);
    y128[1]  = _mm_mulhi_epi16(y128[1],ONE_OVER_SQRT2_Q15_128);y128[1] = _mm_slli_epi16(y128[1],1);
    y128[2]  = _mm_mulhi_epi16(y128[2],ONE_OVER_SQRT2_Q15_128);y128[2] = _mm_slli_epi16(y128[2],1);
    y128[3]  = _mm_mulhi_epi16(y128[3],ONE_OVER_SQRT2_Q15_128);y128[3] = _mm_slli_epi16(y128[3],1);
    y128[4]  = _mm_mulhi_epi16(y128[4],ONE_OVER_SQRT2_Q15_128);y128[4] = _mm_slli_epi16(y128[4],1);
    y128[5]  = _mm_mulhi_epi16(y128[5],ONE_OVER_SQRT2_Q15_128);y128[5] = _mm_slli_epi16(y128[5],1);
    y128[6]  = _mm_mulhi_epi16(y128[6],ONE_OVER_SQRT2_Q15_128);y128[6] = _mm_slli_epi16(y128[6],1);
    y128[7]  = _mm_mulhi_epi16(y128[7],ONE_OVER_SQRT2_Q15_128);y128[7] = _mm_slli_epi16(y128[7],1);
    y128[8]  = _mm_mulhi_epi16(y128[8],ONE_OVER_SQRT2_Q15_128);y128[8] = _mm_slli_epi16(y128[8],1);
    y128[9]  = _mm_mulhi_epi16(y128[9],ONE_OVER_SQRT2_Q15_128);y128[9] = _mm_slli_epi16(y128[9],1);
    y128[10] = _mm_mulhi_epi16(y128[10],ONE_OVER_SQRT2_Q15_128);y128[10] = _mm_slli_epi16(y128[10],1);
    y128[11] = _mm_mulhi_epi16(y128[11],ONE_OVER_SQRT2_Q15_128);y128[11] = _mm_slli_epi16(y128[11],1);
    y128[12] = _mm_mulhi_epi16(y128[12],ONE_OVER_SQRT2_Q15_128);y128[12] = _mm_slli_epi16(y128[12],1);
    y128[13] = _mm_mulhi_epi16(y128[13],ONE_OVER_SQRT2_Q15_128);y128[13] = _mm_slli_epi16(y128[13],1);
    y128[14] = _mm_mulhi_epi16(y128[14],ONE_OVER_SQRT2_Q15_128);y128[14] = _mm_slli_epi16(y128[14],1);
    y128[15] = _mm_mulhi_epi16(y128[15],ONE_OVER_SQRT2_Q15_128);y128[15] = _mm_slli_epi16(y128[15],1);

    y128[16]  = _mm_mulhi_epi16(y128[16],ONE_OVER_SQRT2_Q15_128);y128[16] = _mm_slli_epi16(y128[16],1);
    y128[17]  = _mm_mulhi_epi16(y128[17],ONE_OVER_SQRT2_Q15_128);y128[17] = _mm_slli_epi16(y128[17],1);
    y128[18]  = _mm_mulhi_epi16(y128[18],ONE_OVER_SQRT2_Q15_128);y128[18] = _mm_slli_epi16(y128[18],1);
    y128[19]  = _mm_mulhi_epi16(y128[19],ONE_OVER_SQRT2_Q15_128);y128[19] = _mm_slli_epi16(y128[19],1);
    y128[20]  = _mm_mulhi_epi16(y128[20],ONE_OVER_SQRT2_Q15_128);y128[20] = _mm_slli_epi16(y128[20],1);
    y128[21]  = _mm_mulhi_epi16(y128[21],ONE_OVER_SQRT2_Q15_128);y128[21] = _mm_slli_epi16(y128[21],1);
    y128[22]  = _mm_mulhi_epi16(y128[22],ONE_OVER_SQRT2_Q15_128);y128[22] = _mm_slli_epi16(y128[22],1);
    y128[23]  = _mm_mulhi_epi16(y128[23],ONE_OVER_SQRT2_Q15_128);y128[23] = _mm_slli_epi16(y128[23],1);
    y128[24]  = _mm_mulhi_epi16(y128[24],ONE_OVER_SQRT2_Q15_128);y128[24] = _mm_slli_epi16(y128[24],1);
    y128[25]  = _mm_mulhi_epi16(y128[25],ONE_OVER_SQRT2_Q15_128);y128[25] = _mm_slli_epi16(y128[25],1);
    y128[26] = _mm_mulhi_epi16(y128[26],ONE_OVER_SQRT2_Q15_128);y128[26] = _mm_slli_epi16(y128[26],1);
    y128[27] = _mm_mulhi_epi16(y128[27],ONE_OVER_SQRT2_Q15_128);y128[27] = _mm_slli_epi16(y128[27],1);
    y128[28] = _mm_mulhi_epi16(y128[28],ONE_OVER_SQRT2_Q15_128);y128[28] = _mm_slli_epi16(y128[28],1);
    y128[29] = _mm_mulhi_epi16(y128[29],ONE_OVER_SQRT2_Q15_128);y128[29] = _mm_slli_epi16(y128[29],1);
    y128[30] = _mm_mulhi_epi16(y128[30],ONE_OVER_SQRT2_Q15_128);y128[30] = _mm_slli_epi16(y128[30],1);
    y128[31] = _mm_mulhi_epi16(y128[31],ONE_OVER_SQRT2_Q15_128);y128[31] = _mm_slli_epi16(y128[31],1);

  }
  _mm_empty();
  _m_empty();

}

void idft128(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[64],*x64 = (__m64 *)x;
  __m128i ytmp[32],*tw128_128p=(__m128i *)tw128,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  transpose4_ooff(x64  ,xtmp,32);
  transpose4_ooff(x64+2,xtmp+1,32);
  transpose4_ooff(x64+4,xtmp+2,32);
  transpose4_ooff(x64+6,xtmp+3,32);
  transpose4_ooff(x64+8,xtmp+4,32);
  transpose4_ooff(x64+10,xtmp+5,32);
  transpose4_ooff(x64+12,xtmp+6,32);
  transpose4_ooff(x64+14,xtmp+7,32);
  transpose4_ooff(x64+16,xtmp+8,32);
  transpose4_ooff(x64+18,xtmp+9,32);
  transpose4_ooff(x64+20,xtmp+10,32);
  transpose4_ooff(x64+22,xtmp+11,32);
  transpose4_ooff(x64+24,xtmp+12,32);
  transpose4_ooff(x64+26,xtmp+13,32);
  transpose4_ooff(x64+28,xtmp+14,32);
  transpose4_ooff(x64+30,xtmp+15,32);
  transpose4_ooff(x64+32,xtmp+16,32);
  transpose4_ooff(x64+34,xtmp+17,32);
  transpose4_ooff(x64+36,xtmp+18,32);
  transpose4_ooff(x64+38,xtmp+19,32);
  transpose4_ooff(x64+40,xtmp+20,32);
  transpose4_ooff(x64+42,xtmp+21,32);
  transpose4_ooff(x64+44,xtmp+22,32);
  transpose4_ooff(x64+46,xtmp+23,32);
  transpose4_ooff(x64+48,xtmp+24,32);
  transpose4_ooff(x64+50,xtmp+25,32);
  transpose4_ooff(x64+52,xtmp+26,32);
  transpose4_ooff(x64+54,xtmp+27,32);
  transpose4_ooff(x64+56,xtmp+28,32);
  transpose4_ooff(x64+58,xtmp+29,32);
  transpose4_ooff(x64+60,xtmp+30,32);
  transpose4_ooff(x64+62,xtmp+31,32);

  idft64((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft64((int16_t*)(xtmp+32),(int16_t*)(ytmp+16),1);


  for (i=0;i<16;i++) {
    ibfly2(ytmpp,ytmpp+16,
	   y128p,y128p+16,
	   tw128_128p);
    tw128_128p++;
    y128p++;
    ytmpp++;    
  }

  if (scale>0) {

    y128[0]  = _mm_mulhi_epi16(y128[0],ONE_OVER_SQRT2_Q15_128);y128[0] = _mm_slli_epi16(y128[0],1);
    y128[1]  = _mm_mulhi_epi16(y128[1],ONE_OVER_SQRT2_Q15_128);y128[1] = _mm_slli_epi16(y128[1],1);
    y128[2]  = _mm_mulhi_epi16(y128[2],ONE_OVER_SQRT2_Q15_128);y128[2] = _mm_slli_epi16(y128[2],1);
    y128[3]  = _mm_mulhi_epi16(y128[3],ONE_OVER_SQRT2_Q15_128);y128[3] = _mm_slli_epi16(y128[3],1);
    y128[4]  = _mm_mulhi_epi16(y128[4],ONE_OVER_SQRT2_Q15_128);y128[4] = _mm_slli_epi16(y128[4],1);
    y128[5]  = _mm_mulhi_epi16(y128[5],ONE_OVER_SQRT2_Q15_128);y128[5] = _mm_slli_epi16(y128[5],1);
    y128[6]  = _mm_mulhi_epi16(y128[6],ONE_OVER_SQRT2_Q15_128);y128[6] = _mm_slli_epi16(y128[6],1);
    y128[7]  = _mm_mulhi_epi16(y128[7],ONE_OVER_SQRT2_Q15_128);y128[7] = _mm_slli_epi16(y128[7],1);
    y128[8]  = _mm_mulhi_epi16(y128[8],ONE_OVER_SQRT2_Q15_128);y128[8] = _mm_slli_epi16(y128[8],1);
    y128[9]  = _mm_mulhi_epi16(y128[9],ONE_OVER_SQRT2_Q15_128);y128[9] = _mm_slli_epi16(y128[9],1);
    y128[10] = _mm_mulhi_epi16(y128[10],ONE_OVER_SQRT2_Q15_128);y128[10] = _mm_slli_epi16(y128[10],1);
    y128[11] = _mm_mulhi_epi16(y128[11],ONE_OVER_SQRT2_Q15_128);y128[11] = _mm_slli_epi16(y128[11],1);
    y128[12] = _mm_mulhi_epi16(y128[12],ONE_OVER_SQRT2_Q15_128);y128[12] = _mm_slli_epi16(y128[12],1);
    y128[13] = _mm_mulhi_epi16(y128[13],ONE_OVER_SQRT2_Q15_128);y128[13] = _mm_slli_epi16(y128[13],1);
    y128[14] = _mm_mulhi_epi16(y128[14],ONE_OVER_SQRT2_Q15_128);y128[14] = _mm_slli_epi16(y128[14],1);
    y128[15] = _mm_mulhi_epi16(y128[15],ONE_OVER_SQRT2_Q15_128);y128[15] = _mm_slli_epi16(y128[15],1);

    y128[16]  = _mm_mulhi_epi16(y128[16],ONE_OVER_SQRT2_Q15_128);y128[16] = _mm_slli_epi16(y128[16],1);
    y128[17]  = _mm_mulhi_epi16(y128[17],ONE_OVER_SQRT2_Q15_128);y128[17] = _mm_slli_epi16(y128[17],1);
    y128[18]  = _mm_mulhi_epi16(y128[18],ONE_OVER_SQRT2_Q15_128);y128[18] = _mm_slli_epi16(y128[18],1);
    y128[19]  = _mm_mulhi_epi16(y128[19],ONE_OVER_SQRT2_Q15_128);y128[19] = _mm_slli_epi16(y128[19],1);
    y128[20]  = _mm_mulhi_epi16(y128[20],ONE_OVER_SQRT2_Q15_128);y128[20] = _mm_slli_epi16(y128[20],1);
    y128[21]  = _mm_mulhi_epi16(y128[21],ONE_OVER_SQRT2_Q15_128);y128[21] = _mm_slli_epi16(y128[21],1);
    y128[22]  = _mm_mulhi_epi16(y128[22],ONE_OVER_SQRT2_Q15_128);y128[22] = _mm_slli_epi16(y128[22],1);
    y128[23]  = _mm_mulhi_epi16(y128[23],ONE_OVER_SQRT2_Q15_128);y128[23] = _mm_slli_epi16(y128[23],1);
    y128[24]  = _mm_mulhi_epi16(y128[24],ONE_OVER_SQRT2_Q15_128);y128[24] = _mm_slli_epi16(y128[24],1);
    y128[25]  = _mm_mulhi_epi16(y128[25],ONE_OVER_SQRT2_Q15_128);y128[25] = _mm_slli_epi16(y128[25],1);
    y128[26] = _mm_mulhi_epi16(y128[26],ONE_OVER_SQRT2_Q15_128);y128[26] = _mm_slli_epi16(y128[26],1);
    y128[27] = _mm_mulhi_epi16(y128[27],ONE_OVER_SQRT2_Q15_128);y128[27] = _mm_slli_epi16(y128[27],1);
    y128[28] = _mm_mulhi_epi16(y128[28],ONE_OVER_SQRT2_Q15_128);y128[28] = _mm_slli_epi16(y128[28],1);
    y128[29] = _mm_mulhi_epi16(y128[29],ONE_OVER_SQRT2_Q15_128);y128[29] = _mm_slli_epi16(y128[29],1);
    y128[30] = _mm_mulhi_epi16(y128[30],ONE_OVER_SQRT2_Q15_128);y128[30] = _mm_slli_epi16(y128[30],1);
    y128[31] = _mm_mulhi_epi16(y128[31],ONE_OVER_SQRT2_Q15_128);y128[31] = _mm_slli_epi16(y128[31],1);

  }
  _mm_empty();
  _m_empty();

}
  

int16_t tw256[384] __attribute__((aligned(16))) = {  32767,0,32757,-805,32727,-1608,32678,-2411,32609,-3212,32520,-4012,32412,-4808,32284,-5602,32137,-6393,31970,-7180,31785,-7962,31580,-8740,31356,-9512,31113,-10279,30851,-11039,30571,-11793,30272,-12540,29955,-13279,29621,-14010,29268,-14733,28897,-15447,28510,-16151,28105,-16846,27683,-17531,27244,-18205,26789,-18868,26318,-19520,25831,-20160,25329,-20788,24811,-21403,24278,-22005,23731,-22595,23169,-23170,22594,-23732,22004,-24279,21402,-24812,20787,-25330,20159,-25832,19519,-26319,18867,-26790,18204,-27245,17530,-27684,16845,-28106,16150,-28511,15446,-28898,14732,-29269,14009,-29622,13278,-29956,12539,-30273,11792,-30572,11038,-30852,10278,-31114,9511,-31357,8739,-31581,7961,-31786,7179,-31971,6392,-32138,5601,-32285,4807,-32413,4011,-32521,3211,-32610,2410,-32679,1607,-32728,804,-32758,
						     32767,0,32727,-1608,32609,-3212,32412,-4808,32137,-6393,31785,-7962,31356,-9512,30851,-11039,30272,-12540,29621,-14010,28897,-15447,28105,-16846,27244,-18205,26318,-19520,25329,-20788,24278,-22005,23169,-23170,22004,-24279,20787,-25330,19519,-26319,18204,-27245,16845,-28106,15446,-28898,14009,-29622,12539,-30273,11038,-30852,9511,-31357,7961,-31786,6392,-32138,4807,-32413,3211,-32610,1607,-32728,0,-32767,-1608,-32728,-3212,-32610,-4808,-32413,-6393,-32138,-7962,-31786,-9512,-31357,-11039,-30852,-12540,-30273,-14010,-29622,-15447,-28898,-16846,-28106,-18205,-27245,-19520,-26319,-20788,-25330,-22005,-24279,-23170,-23170,-24279,-22005,-25330,-20788,-26319,-19520,-27245,-18205,-28106,-16846,-28898,-15447,-29622,-14010,-30273,-12540,-30852,-11039,-31357,-9512,-31786,-7962,-32138,-6393,-32413,-4808,-32610,-3212,-32728,-1608,
						     32767,0,32678,-2411,32412,-4808,31970,-7180,31356,-9512,30571,-11793,29621,-14010,28510,-16151,27244,-18205,25831,-20160,24278,-22005,22594,-23732,20787,-25330,18867,-26790,16845,-28106,14732,-29269,12539,-30273,10278,-31114,7961,-31786,5601,-32285,3211,-32610,804,-32758,-1608,-32728,-4012,-32521,-6393,-32138,-8740,-31581,-11039,-30852,-13279,-29956,-15447,-28898,-17531,-27684,-19520,-26319,-21403,-24812,-23170,-23170,-24812,-21403,-26319,-19520,-27684,-17531,-28898,-15447,-29956,-13279,-30852,-11039,-31581,-8740,-32138,-6393,-32521,-4012,-32728,-1608,-32758,804,-32610,3211,-32285,5601,-31786,7961,-31114,10278,-30273,12539,-29269,14732,-28106,16845,-26790,18867,-25330,20787,-23732,22594,-22005,24278,-20160,25831,-18205,27244,-16151,28510,-14010,29621,-11793,30571,-9512,31356,-7180,31970,-4808,32412,-2411,32678};

int16_t tw256a[384] __attribute__((aligned(16))) = { 32767,0,32757,804,32727,1607,32678,2410,32609,3211,32520,4011,32412,4807,32284,5601,32137,6392,31970,7179,31785,7961,31580,8739,31356,9511,31113,10278,30851,11038,30571,11792,30272,12539,29955,13278,29621,14009,29268,14732,28897,15446,28510,16150,28105,16845,27683,17530,27244,18204,26789,18867,26318,19519,25831,20159,25329,20787,24811,21402,24278,22004,23731,22594,23169,23169,22594,23731,22004,24278,21402,24811,20787,25329,20159,25831,19519,26318,18867,26789,18204,27244,17530,27683,16845,28105,16150,28510,15446,28897,14732,29268,14009,29621,13278,29955,12539,30272,11792,30571,11038,30851,10278,31113,9511,31356,8739,31580,7961,31785,7179,31970,6392,32137,5601,32284,4807,32412,4011,32520,3211,32609,2410,32678,1607,32727,804,32757,
						     32767,0,32727,1607,32609,3211,32412,4807,32137,6392,31785,7961,31356,9511,30851,11038,30272,12539,29621,14009,28897,15446,28105,16845,27244,18204,26318,19519,25329,20787,24278,22004,23169,23169,22004,24278,20787,25329,19519,26318,18204,27244,16845,28105,15446,28897,14009,29621,12539,30272,11038,30851,9511,31356,7961,31785,6392,32137,4807,32412,3211,32609,1607,32727,0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,
						     32767,0,32678,2410,32412,4807,31970,7179,31356,9511,30571,11792,29621,14009,28510,16150,27244,18204,25831,20159,24278,22004,22594,23731,20787,25329,18867,26789,16845,28105,14732,29268,12539,30272,10278,31113,7961,31785,5601,32284,3211,32609,804,32757,-1608,32727,-4012,32520,-6393,32137,-8740,31580,-11039,30851,-13279,29955,-15447,28897,-17531,27683,-19520,26318,-21403,24811,-23170,23169,-24812,21402,-26319,19519,-27684,17530,-28898,15446,-29956,13278,-30852,11038,-31581,8739,-32138,6392,-32521,4011,-32728,1607,-32758,-805,-32610,-3212,-32285,-5602,-31786,-7962,-31114,-10279,-30273,-12540,-29269,-14733,-28106,-16846,-26790,-18868,-25330,-20788,-23732,-22595,-22005,-24279,-20160,-25832,-18205,-27245,-16151,-28511,-14010,-29622,-11793,-30572,-9512,-31357,-7180,-31971,-4808,-32413,-2411,-32679};

int16_t tw256b[384] __attribute__((aligned(16))) = {0,32767,-805,32757,-1608,32727,-2411,32678,-3212,32609,-4012,32520,-4808,32412,-5602,32284,-6393,32137,-7180,31970,-7962,31785,-8740,31580,-9512,31356,-10279,31113,-11039,30851,-11793,30571,-12540,30272,-13279,29955,-14010,29621,-14733,29268,-15447,28897,-16151,28510,-16846,28105,-17531,27683,-18205,27244,-18868,26789,-19520,26318,-20160,25831,-20788,25329,-21403,24811,-22005,24278,-22595,23731,-23170,23169,-23732,22594,-24279,22004,-24812,21402,-25330,20787,-25832,20159,-26319,19519,-26790,18867,-27245,18204,-27684,17530,-28106,16845,-28511,16150,-28898,15446,-29269,14732,-29622,14009,-29956,13278,-30273,12539,-30572,11792,-30852,11038,-31114,10278,-31357,9511,-31581,8739,-31786,7961,-31971,7179,-32138,6392,-32285,5601,-32413,4807,-32521,4011,-32610,3211,-32679,2410,-32728,1607,-32758,804,
						    0,32767,-1608,32727,-3212,32609,-4808,32412,-6393,32137,-7962,31785,-9512,31356,-11039,30851,-12540,30272,-14010,29621,-15447,28897,-16846,28105,-18205,27244,-19520,26318,-20788,25329,-22005,24278,-23170,23169,-24279,22004,-25330,20787,-26319,19519,-27245,18204,-28106,16845,-28898,15446,-29622,14009,-30273,12539,-30852,11038,-31357,9511,-31786,7961,-32138,6392,-32413,4807,-32610,3211,-32728,1607,-32767,0,-32728,-1608,-32610,-3212,-32413,-4808,-32138,-6393,-31786,-7962,-31357,-9512,-30852,-11039,-30273,-12540,-29622,-14010,-28898,-15447,-28106,-16846,-27245,-18205,-26319,-19520,-25330,-20788,-24279,-22005,-23170,-23170,-22005,-24279,-20788,-25330,-19520,-26319,-18205,-27245,-16846,-28106,-15447,-28898,-14010,-29622,-12540,-30273,-11039,-30852,-9512,-31357,-7962,-31786,-6393,-32138,-4808,-32413,-3212,-32610,-1608,-32728,
						    0,32767,-2411,32678,-4808,32412,-7180,31970,-9512,31356,-11793,30571,-14010,29621,-16151,28510,-18205,27244,-20160,25831,-22005,24278,-23732,22594,-25330,20787,-26790,18867,-28106,16845,-29269,14732,-30273,12539,-31114,10278,-31786,7961,-32285,5601,-32610,3211,-32758,804,-32728,-1608,-32521,-4012,-32138,-6393,-31581,-8740,-30852,-11039,-29956,-13279,-28898,-15447,-27684,-17531,-26319,-19520,-24812,-21403,-23170,-23170,-21403,-24812,-19520,-26319,-17531,-27684,-15447,-28898,-13279,-29956,-11039,-30852,-8740,-31581,-6393,-32138,-4012,-32521,-1608,-32728,804,-32758,3211,-32610,5601,-32285,7961,-31786,10278,-31114,12539,-30273,14732,-29269,16845,-28106,18867,-26790,20787,-25330,22594,-23732,24278,-22005,25831,-20160,27244,-18205,28510,-16151,29621,-14010,30571,-11793,31356,-9512,31970,-7180,32412,-4808,32678,-2411};

void dft256(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[64],ytmp[64],*tw256a_128p=(__m128i *)tw256a,*tw256b_128p=(__m128i *)tw256b,*x128=(__m128i *)x,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i,j;

#ifdef D256STATS
  time_stats_t ts_t,ts_d,ts_b;

  reset_meas(&ts_t);
  reset_meas(&ts_d);
  reset_meas(&ts_b);
  start_meas(&ts_t);
#endif  
  /*
  for (i=0,j=0;i<64;i+=4,j++) {
    transpose16_ooff(x128+i,xtmp+j,16);
  }
  */
    transpose16_ooff(x128+0,xtmp+0,16);
    transpose16_ooff(x128+4,xtmp+1,16);
    transpose16_ooff(x128+8,xtmp+2,16);
    transpose16_ooff(x128+12,xtmp+3,16);
    transpose16_ooff(x128+16,xtmp+4,16);
    transpose16_ooff(x128+20,xtmp+5,16);
    transpose16_ooff(x128+24,xtmp+6,16);
    transpose16_ooff(x128+28,xtmp+7,16);
    transpose16_ooff(x128+32,xtmp+8,16);
    transpose16_ooff(x128+36,xtmp+9,16);
    transpose16_ooff(x128+40,xtmp+10,16);
    transpose16_ooff(x128+44,xtmp+11,16);
    transpose16_ooff(x128+48,xtmp+12,16);
    transpose16_ooff(x128+52,xtmp+13,16);
    transpose16_ooff(x128+56,xtmp+14,16);
    transpose16_ooff(x128+60,xtmp+15,16);

#ifdef D256STATS
  stop_meas(&ts_t);
  start_meas(&ts_d);
#endif

  dft64((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  dft64((int16_t*)(xtmp+16),(int16_t*)(ytmp+16),1);
  dft64((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);
  dft64((int16_t*)(xtmp+48),(int16_t*)(ytmp+48),1);

#ifdef D256STATS
  stop_meas(&ts_d);
  start_meas(&ts_b);
#endif

  for (i=0;i<16;i+=4) {
    bfly4_16(ytmpp,ytmpp+16,ytmpp+32,ytmpp+48,
	     y128p,y128p+16,y128p+32,y128p+48,
	     tw256a_128p,tw256a_128p+16,tw256a_128p+32,
	     tw256b_128p,tw256b_128p+16,tw256b_128p+32);
    bfly4_16(ytmpp+1,ytmpp+17,ytmpp+33,ytmpp+49,
	     y128p+1,y128p+17,y128p+33,y128p+49,
	     tw256a_128p+1,tw256a_128p+17,tw256a_128p+33,
	     tw256b_128p+1,tw256b_128p+17,tw256b_128p+33);
    bfly4_16(ytmpp+2,ytmpp+18,ytmpp+34,ytmpp+50,
	     y128p+2,y128p+18,y128p+34,y128p+50,
	     tw256a_128p+2,tw256a_128p+18,tw256a_128p+34,
	     tw256b_128p+2,tw256b_128p+18,tw256b_128p+34);
    bfly4_16(ytmpp+3,ytmpp+19,ytmpp+35,ytmpp+51,
	     y128p+3,y128p+19,y128p+35,y128p+51,
	     tw256a_128p+3,tw256a_128p+19,tw256a_128p+35,
	     tw256b_128p+3,tw256b_128p+19,tw256b_128p+35);
    tw256a_128p+=4;
    tw256b_128p+=4;
    y128p+=4;
    ytmpp+=4;
  }

#ifdef D256STATS
  stop_meas(&ts_b);
  printf("t: %llu cycles, d: %llu cycles, b: %llu cycles\n",ts_t.diff,ts_d.diff,ts_b.diff);
#endif
    
  if (scale>0) {

    for (i=0;i<4;i++) {
      y128[0]  = _mm_srai_epi16(y128[0],1);
      y128[1]  = _mm_srai_epi16(y128[1],1);
      y128[2]  = _mm_srai_epi16(y128[2],1);
      y128[3]  = _mm_srai_epi16(y128[3],1);
      y128[4]  = _mm_srai_epi16(y128[4],1);
      y128[5]  = _mm_srai_epi16(y128[5],1);
      y128[6]  = _mm_srai_epi16(y128[6],1);
      y128[7]  = _mm_srai_epi16(y128[7],1);
      y128[8]  = _mm_srai_epi16(y128[8],1);
      y128[9]  = _mm_srai_epi16(y128[9],1);
      y128[10] = _mm_srai_epi16(y128[10],1);
      y128[11] = _mm_srai_epi16(y128[11],1);
      y128[12] = _mm_srai_epi16(y128[12],1);
      y128[13] = _mm_srai_epi16(y128[13],1);
      y128[14] = _mm_srai_epi16(y128[14],1);
      y128[15] = _mm_srai_epi16(y128[15],1);

      y128+=16;
    }

  }
  _mm_empty();
  _m_empty();

}



void idft256(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[64],ytmp[64],*tw256_128p=(__m128i *)tw256,*x128=(__m128i *)x,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i,j;
  
  for (i=0,j=0;i<64;i+=4,j++) {
    transpose16_ooff(x128+i,xtmp+j,16);
  }
  

  idft64((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  idft64((int16_t*)(xtmp+16),(int16_t*)(ytmp+16),1);
  idft64((int16_t*)(xtmp+32),(int16_t*)(ytmp+32),1);
  idft64((int16_t*)(xtmp+48),(int16_t*)(ytmp+48),1);

  for (i=0;i<16;i++) {
    ibfly4(ytmpp,ytmpp+16,ytmpp+32,ytmpp+48,
	  y128p,y128p+16,y128p+32,y128p+48,
	  tw256_128p,tw256_128p+16,tw256_128p+32);
    tw256_128p++;
    y128p++;
    ytmpp++;
  }
    
  if (scale>0) {

    for (i=0;i<4;i++) {
      y128[0]  = _mm_srai_epi16(y128[0],1);
      y128[1]  = _mm_srai_epi16(y128[1],1);
      y128[2]  = _mm_srai_epi16(y128[2],1);
      y128[3]  = _mm_srai_epi16(y128[3],1);
      y128[4]  = _mm_srai_epi16(y128[4],1);
      y128[5]  = _mm_srai_epi16(y128[5],1);
      y128[6]  = _mm_srai_epi16(y128[6],1);
      y128[7]  = _mm_srai_epi16(y128[7],1);
      y128[8]  = _mm_srai_epi16(y128[8],1);
      y128[9]  = _mm_srai_epi16(y128[9],1);
      y128[10] = _mm_srai_epi16(y128[10],1);
      y128[11] = _mm_srai_epi16(y128[11],1);
      y128[12] = _mm_srai_epi16(y128[12],1);
      y128[13] = _mm_srai_epi16(y128[13],1);
      y128[14] = _mm_srai_epi16(y128[14],1);
      y128[15] = _mm_srai_epi16(y128[15],1);

      y128+=16;
    }

  }
  _mm_empty();
  _m_empty();

} 

int16_t tw512[512] __attribute__((aligned(16))) = { 
32767,0,32764,-403,32757,-805,32744,-1207,32727,-1608,32705,-2010,32678,-2411,32646,-2812,32609,-3212,32567,-3612,32520,-4012,32468,-4410,32412,-4808,32350,-5206,32284,-5602,32213,-5998,32137,-6393,32056,-6787,31970,-7180,31880,-7572,31785,-7962,31684,-8352,31580,-8740,31470,-9127,31356,-9512,31236,-9896,31113,-10279,30984,-10660,30851,-11039,30713,-11417,30571,-11793,30424,-12167,30272,-12540,30116,-12910,29955,-13279,29790,-13646,29621,-14010,29446,-14373,29268,-14733,29085,-15091,28897,-15447,28706,-15800,28510,-16151,28309,-16500,28105,-16846,27896,-17190,27683,-17531,27466,-17869,27244,-18205,27019,-18538,26789,-18868,26556,-19195,26318,-19520,26077,-19841,25831,-20160,25582,-20475,25329,-20788,25072,-21097,24811,-21403,24546,-21706,24278,-22005,24006,-22302,23731,-22595,23452,-22884,23169,-23170,22883,-23453,22594,-23732,22301,-24007,22004,-24279,21705,-24547,21402,-24812,21096,-25073,20787,-25330,20474,-25583,20159,-25832,19840,-26078,19519,-26319,19194,-26557,18867,-26790,18537,-27020,18204,-27245,17868,-27467,17530,-27684,17189,-27897,16845,-28106,16499,-28310,16150,-28511,15799,-28707,15446,-28898,15090,-29086,14732,-29269,14372,-29447,14009,-29622,13645,-29791,13278,-29956,12909,-30117,12539,-30273,12166,-30425,11792,-30572,11416,-30714,11038,-30852,10659,-30985,10278,-31114,9895,-31237,9511,-31357,9126,-31471,8739,-31581,8351,-31685,7961,-31786,7571,-31881,7179,-31971,6786,-32057,6392,-32138,5997,-32214,5601,-32285,5205,-32351,4807,-32413,4409,-32469,4011,-32521,3611,-32568,3211,-32610,2811,-32647,2410,-32679,2009,-32706,1607,-32728,1206,-32745,804,-32758,402,-32765,0,-32767,-403,-32765,-805,-32758,-1207,-32745,-1608,-32728,-2010,-32706,-2411,-32679,-2812,-32647,-3212,-32610,-3612,-32568,-4012,-32521,-4410,-32469,-4808,-32413,-5206,-32351,-5602,-32285,-5998,-32214,-6393,-32138,-6787,-32057,-7180,-31971,-7572,-31881,-7962,-31786,-8352,-31685,-8740,-31581,-9127,-31471,-9512,-31357,-9896,-31237,-10279,-31114,-10660,-30985,-11039,-30852,-11417,-30714,-11793,-30572,-12167,-30425,-12540,-30273,-12910,-30117,-13279,-29956,-13646,-29791,-14010,-29622,-14373,-29447,-14733,-29269,-15091,-29086,-15447,-28898,-15800,-28707,-16151,-28511,-16500,-28310,-16846,-28106,-17190,-27897,-17531,-27684,-17869,-27467,-18205,-27245,-18538,-27020,-18868,-26790,-19195,-26557,-19520,-26319,-19841,-26078,-20160,-25832,-20475,-25583,-20788,-25330,-21097,-25073,-21403,-24812,-21706,-24547,-22005,-24279,-22302,-24007,-22595,-23732,-22884,-23453,-23170,-23170,-23453,-22884,-23732,-22595,-24007,-22302,-24279,-22005,-24547,-21706,-24812,-21403,-25073,-21097,-25330,-20788,-25583,-20475,-25832,-20160,-26078,-19841,-26319,-19520,-26557,-19195,-26790,-18868,-27020,-18538,-27245,-18205,-27467,-17869,-27684,-17531,-27897,-17190,-28106,-16846,-28310,-16500,-28511,-16151,-28707,-15800,-28898,-15447,-29086,-15091,-29269,-14733,-29447,-14373,-29622,-14010,-29791,-13646,-29956,-13279,-30117,-12910,-30273,-12540,-30425,-12167,-30572,-11793,-30714,-11417,-30852,-11039,-30985,-10660,-31114,-10279,-31237,-9896,-31357,-9512,-31471,-9127,-31581,-8740,-31685,-8352,-31786,-7962,-31881,-7572,-31971,-7180,-32057,-6787,-32138,-6393,-32214,-5998,-32285,-5602,-32351,-5206,-32413,-4808,-32469,-4410,-32521,-4012,-32568,-3612,-32610,-3212,-32647,-2812,-32679,-2411,-32706,-2010,-32728,-1608,-32745,-1207,-32758,-805,-32765,-403
};

int16_t tw512a[512] __attribute__((aligned(16))) = { 
  32767,0,32764,403,32757,805,32744,1207,32727,1608,32705,2010,32678,2411,32646,2812,32609,3212,32567,3612,32520,4012,32468,4410,32412,4808,32350,5206,32284,5602,32213,5998,32137,6393,32056,6787,31970,7180,31880,7572,31785,7962,31684,8352,31580,8740,31470,9127,31356,9512,31236,9896,31113,10279,30984,10660,30851,11039,30713,11417,30571,11793,30424,12167,30272,12540,30116,12910,29955,13279,29790,13646,29621,14010,29446,14373,29268,14733,29085,15091,28897,15447,28706,15800,28510,16151,28309,16500,28105,16846,27896,17190,27683,17531,27466,17869,27244,18205,27019,18538,26789,18868,26556,19195,26318,19520,26077,19841,25831,20160,25582,20475,25329,20788,25072,21097,24811,21403,24546,21706,24278,22005,24006,22302,23731,22595,23452,22884,23169,23170,22883,23453,22594,23732,22301,24007,22004,24279,21705,24547,21402,24812,21096,25073,20787,25330,20474,25583,20159,25832,19840,26078,19519,26319,19194,26557,18867,26790,18537,27020,18204,27245,17868,27467,17530,27684,17189,27897,16845,28106,16499,28310,16150,28511,15799,28707,15446,28898,15090,29086,14732,29269,14372,29447,14009,29622,13645,29791,13278,29956,12909,30117,12539,30273,12166,30425,11792,30572,11416,30714,11038,30852,10659,30985,10278,31114,9895,31237,9511,31357,9126,31471,8739,31581,8351,31685,7961,31786,7571,31881,7179,31971,6786,32057,6392,32138,5997,32214,5601,32285,5205,32351,4807,32413,4409,32469,4011,32521,3611,32568,3211,32610,2811,32647,2410,32679,2009,32706,1607,32728,1206,32745,804,32758,402,32765,0,32767,-403,32765,-805,32758,-1207,32745,-1608,32728,-2010,32706,-2411,32679,-2812,32647,-3212,32610,-3612,32568,-4012,32521,-4410,32469,-4808,32413,-5206,32351,-5602,32285,-5998,32214,-6393,32138,-6787,32057,-7180,31971,-7572,31881,-7962,31786,-8352,31685,-8740,31581,-9127,31471,-9512,31357,-9896,31237,-10279,31114,-10660,30985,-11039,30852,-11417,30714,-11793,30572,-12167,30425,-12540,30273,-12910,30117,-13279,29956,-13646,29791,-14010,29622,-14373,29447,-14733,29269,-15091,29086,-15447,28898,-15800,28707,-16151,28511,-16500,28310,-16846,28106,-17190,27897,-17531,27684,-17869,27467,-18205,27245,-18538,27020,-18868,26790,-19195,26557,-19520,26319,-19841,26078,-20160,25832,-20475,25583,-20788,25330,-21097,25073,-21403,24812,-21706,24547,-22005,24279,-22302,24007,-22595,23732,-22884,23453,-23170,23170,-23453,22884,-23732,22595,-24007,22302,-24279,22005,-24547,21706,-24812,21403,-25073,21097,-25330,20788,-25583,20475,-25832,20160,-26078,19841,-26319,19520,-26557,19195,-26790,18868,-27020,18538,-27245,18205,-27467,17869,-27684,17531,-27897,17190,-28106,16846,-28310,16500,-28511,16151,-28707,15800,-28898,15447,-29086,15091,-29269,14733,-29447,14373,-29622,14010,-29791,13646,-29956,13279,-30117,12910,-30273,12540,-30425,12167,-30572,11793,-30714,11417,-30852,11039,-30985,10660,-31114,10279,-31237,9896,-31357,9512,-31471,9127,-31581,8740,-31685,8352,-31786,7962,-31881,7572,-31971,7180,-32057,6787,-32138,6393,-32214,5998,-32285,5602,-32351,5206,-32413,4808,-32469,4410,-32521,4012,-32568,3612,-32610,3212,-32647,2812,-32679,2411,-32706,2010,-32728,1608,-32745,1207,-32758,805,-32765,403};



int16_t tw512b[512] __attribute__((aligned(16))) = {
  0,32767,-403,32764,-805,32757,-1207,32744,-1608,32727,-2010,32705,-2411,32678,-2812,32646,-3212,32609,-3612,32567,-4012,32520,-4410,32468,-4808,32412,-5206,32350,-5602,32284,-5998,32213,-6393,32137,-6787,32056,-7180,31970,-7572,31880,-7962,31785,-8352,31684,-8740,31580,-9127,31470,-9512,31356,-9896,31236,-10279,31113,-10660,30984,-11039,30851,-11417,30713,-11793,30571,-12167,30424,-12540,30272,-12910,30116,-13279,29955,-13646,29790,-14010,29621,-14373,29446,-14733,29268,-15091,29085,-15447,28897,-15800,28706,-16151,28510,-16500,28309,-16846,28105,-17190,27896,-17531,27683,-17869,27466,-18205,27244,-18538,27019,-18868,26789,-19195,26556,-19520,26318,-19841,26077,-20160,25831,-20475,25582,-20788,25329,-21097,25072,-21403,24811,-21706,24546,-22005,24278,-22302,24006,-22595,23731,-22884,23452,-23170,23169,-23453,22883,-23732,22594,-24007,22301,-24279,22004,-24547,21705,-24812,21402,-25073,21096,-25330,20787,-25583,20474,-25832,20159,-26078,19840,-26319,19519,-26557,19194,-26790,18867,-27020,18537,-27245,18204,-27467,17868,-27684,17530,-27897,17189,-28106,16845,-28310,16499,-28511,16150,-28707,15799,-28898,15446,-29086,15090,-29269,14732,-29447,14372,-29622,14009,-29791,13645,-29956,13278,-30117,12909,-30273,12539,-30425,12166,-30572,11792,-30714,11416,-30852,11038,-30985,10659,-31114,10278,-31237,9895,-31357,9511,-31471,9126,-31581,8739,-31685,8351,-31786,7961,-31881,7571,-31971,7179,-32057,6786,-32138,6392,-32214,5997,-32285,5601,-32351,5205,-32413,4807,-32469,4409,-32521,4011,-32568,3611,-32610,3211,-32647,2811,-32679,2410,-32706,2009,-32728,1607,-32745,1206,-32758,804,-32765,402,-32767,0,-32765,-403,-32758,-805,-32745,-1207,-32728,-1608,-32706,-2010,-32679,-2411,-32647,-2812,-32610,-3212,-32568,-3612,-32521,-4012,-32469,-4410,-32413,-4808,-32351,-5206,-32285,-5602,-32214,-5998,-32138,-6393,-32057,-6787,-31971,-7180,-31881,-7572,-31786,-7962,-31685,-8352,-31581,-8740,-31471,-9127,-31357,-9512,-31237,-9896,-31114,-10279,-30985,-10660,-30852,-11039,-30714,-11417,-30572,-11793,-30425,-12167,-30273,-12540,-30117,-12910,-29956,-13279,-29791,-13646,-29622,-14010,-29447,-14373,-29269,-14733,-29086,-15091,-28898,-15447,-28707,-15800,-28511,-16151,-28310,-16500,-28106,-16846,-27897,-17190,-27684,-17531,-27467,-17869,-27245,-18205,-27020,-18538,-26790,-18868,-26557,-19195,-26319,-19520,-26078,-19841,-25832,-20160,-25583,-20475,-25330,-20788,-25073,-21097,-24812,-21403,-24547,-21706,-24279,-22005,-24007,-22302,-23732,-22595,-23453,-22884,-23170,-23170,-22884,-23453,-22595,-23732,-22302,-24007,-22005,-24279,-21706,-24547,-21403,-24812,-21097,-25073,-20788,-25330,-20475,-25583,-20160,-25832,-19841,-26078,-19520,-26319,-19195,-26557,-18868,-26790,-18538,-27020,-18205,-27245,-17869,-27467,-17531,-27684,-17190,-27897,-16846,-28106,-16500,-28310,-16151,-28511,-15800,-28707,-15447,-28898,-15091,-29086,-14733,-29269,-14373,-29447,-14010,-29622,-13646,-29791,-13279,-29956,-12910,-30117,-12540,-30273,-12167,-30425,-11793,-30572,-11417,-30714,-11039,-30852,-10660,-30985,-10279,-31114,-9896,-31237,-9512,-31357,-9127,-31471,-8740,-31581,-8352,-31685,-7962,-31786,-7572,-31881,-7180,-31971,-6787,-32057,-6393,-32138,-5998,-32214,-5602,-32285,-5206,-32351,-4808,-32413,-4410,-32469,-4012,-32521,-3612,-32568,-3212,-32610,-2812,-32647,-2411,-32679,-2010,-32706,-1608,-32728,-1207,-32745,-805,-32758,-403,-32765};

int16_t tw512c[512] __attribute__((aligned(16))) = {
  0,32767,403,32764,805,32757,1207,32744,1608,32727,2010,32705,2411,32678,2812,32646,3212,32609,3612,32567,4012,32520,4410,32468,4808,32412,5206,32350,5602,32284,5998,32213,6393,32137,6787,32056,7180,31970,7572,31880,7962,31785,8352,31684,8740,31580,9127,31470,9512,31356,9896,31236,10279,31113,10660,30984,11039,30851,11417,30713,11793,30571,12167,30424,12540,30272,12910,30116,13279,29955,13646,29790,14010,29621,14373,29446,14733,29268,15091,29085,15447,28897,15800,28706,16151,28510,16500,28309,16846,28105,17190,27896,17531,27683,17869,27466,18205,27244,18538,27019,18868,26789,19195,26556,19520,26318,19841,26077,20160,25831,20475,25582,20788,25329,21097,25072,21403,24811,21706,24546,22005,24278,22302,24006,22595,23731,22884,23452,23170,23169,23453,22883,23732,22594,24007,22301,24279,22004,24547,21705,24812,21402,25073,21096,25330,20787,25583,20474,25832,20159,26078,19840,26319,19519,26557,19194,26790,18867,27020,18537,27245,18204,27467,17868,27684,17530,27897,17189,28106,16845,28310,16499,28511,16150,28707,15799,28898,15446,29086,15090,29269,14732,29447,14372,29622,14009,29791,13645,29956,13278,30117,12909,30273,12539,30425,12166,30572,11792,30714,11416,30852,11038,30985,10659,31114,10278,31237,9895,31357,9511,31471,9126,31581,8739,31685,8351,31786,7961,31881,7571,31971,7179,32057,6786,32138,6392,32214,5997,32285,5601,32351,5205,32413,4807,32469,4409,32521,4011,32568,3611,32610,3211,32647,2811,32679,2410,32706,2009,32728,1607,32745,1206,32758,804,32765,402,32767,0,32765,-403,32758,-805,32745,-1207,32728,-1608,32706,-2010,32679,-2411,32647,-2812,32610,-3212,32568,-3612,32521,-4012,32469,-4410,32413,-4808,32351,-5206,32285,-5602,32214,-5998,32138,-6393,32057,-6787,31971,-7180,31881,-7572,31786,-7962,31685,-8352,31581,-8740,31471,-9127,31357,-9512,31237,-9896,31114,-10279,30985,-10660,30852,-11039,30714,-11417,30572,-11793,30425,-12167,30273,-12540,30117,-12910,29956,-13279,29791,-13646,29622,-14010,29447,-14373,29269,-14733,29086,-15091,28898,-15447,28707,-15800,28511,-16151,28310,-16500,28106,-16846,27897,-17190,27684,-17531,27467,-17869,27245,-18205,27020,-18538,26790,-18868,26557,-19195,26319,-19520,26078,-19841,25832,-20160,25583,-20475,25330,-20788,25073,-21097,24812,-21403,24547,-21706,24279,-22005,24007,-22302,23732,-22595,23453,-22884,23170,-23170,22884,-23453,22595,-23732,22302,-24007,22005,-24279,21706,-24547,21403,-24812,21097,-25073,20788,-25330,20475,-25583,20160,-25832,19841,-26078,19520,-26319,19195,-26557,18868,-26790,18538,-27020,18205,-27245,17869,-27467,17531,-27684,17190,-27897,16846,-28106,16500,-28310,16151,-28511,15800,-28707,15447,-28898,15091,-29086,14733,-29269,14373,-29447,14010,-29622,13646,-29791,13279,-29956,12910,-30117,12540,-30273,12167,-30425,11793,-30572,11417,-30714,11039,-30852,10660,-30985,10279,-31114,9896,-31237,9512,-31357,9127,-31471,8740,-31581,8352,-31685,7962,-31786,7572,-31881,7180,-31971,6787,-32057,6393,-32138,5998,-32214,5602,-32285,5206,-32351,4808,-32413,4410,-32469,4012,-32521,3612,-32568,3212,-32610,2812,-32647,2411,-32679,2010,-32706,1608,-32728,1207,-32745,805,-32758,403,-32765};


void dft512(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[256],*xtmpp,*x64 = (__m64 *)x;
  __m128i ytmp[128],*tw512a_128p=(__m128i *)tw512a,*tw512b_128p=(__m128i *)tw512b,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  xtmpp = xtmp;

  for (i=0;i<4;i++) {
    transpose4_ooff(x64  ,xtmpp,128);
    transpose4_ooff(x64+2,xtmpp+1,128);
    transpose4_ooff(x64+4,xtmpp+2,128);
    transpose4_ooff(x64+6,xtmpp+3,128);
    transpose4_ooff(x64+8,xtmpp+4,128);
    transpose4_ooff(x64+10,xtmpp+5,128);
    transpose4_ooff(x64+12,xtmpp+6,128);
    transpose4_ooff(x64+14,xtmpp+7,128);
    transpose4_ooff(x64+16,xtmpp+8,128);
    transpose4_ooff(x64+18,xtmpp+9,128);
    transpose4_ooff(x64+20,xtmpp+10,128);
    transpose4_ooff(x64+22,xtmpp+11,128);
    transpose4_ooff(x64+24,xtmpp+12,128);
    transpose4_ooff(x64+26,xtmpp+13,128);
    transpose4_ooff(x64+28,xtmpp+14,128);
    transpose4_ooff(x64+30,xtmpp+15,128);
    transpose4_ooff(x64+32,xtmpp+16,128);
    transpose4_ooff(x64+34,xtmpp+17,128);
    transpose4_ooff(x64+36,xtmpp+18,128);
    transpose4_ooff(x64+38,xtmpp+19,128);
    transpose4_ooff(x64+40,xtmpp+20,128);
    transpose4_ooff(x64+42,xtmpp+21,128);
    transpose4_ooff(x64+44,xtmpp+22,128);
    transpose4_ooff(x64+46,xtmpp+23,128);
    transpose4_ooff(x64+48,xtmpp+24,128);
    transpose4_ooff(x64+50,xtmpp+25,128);
    transpose4_ooff(x64+52,xtmpp+26,128);
    transpose4_ooff(x64+54,xtmpp+27,128);
    transpose4_ooff(x64+56,xtmpp+28,128);
    transpose4_ooff(x64+58,xtmpp+29,128);
    transpose4_ooff(x64+60,xtmpp+30,128);
    transpose4_ooff(x64+62,xtmpp+31,128);
    x64+=64;
    xtmpp+=32;
  }

  dft256((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft256((int16_t*)(xtmp+128),(int16_t*)(ytmp+64),1);


  for (i=0;i<64;i+=8) {
    bfly2_16(ytmpp,ytmpp+64,
	     y128p,y128p+64,
	     tw512a_128p,
	     tw512b_128p);
    bfly2_16(ytmpp+1,ytmpp+65,
	     y128p+1,y128p+65,
	     tw512a_128p+1,
	     tw512b_128p+1);
    bfly2_16(ytmpp+2,ytmpp+66,
	     y128p+2,y128p+66,
	     tw512a_128p+2,
	     tw512b_128p+2);
    bfly2_16(ytmpp+3,ytmpp+67,
	     y128p+3,y128p+67,
	     tw512a_128p+3,
	     tw512b_128p+3);
    bfly2_16(ytmpp+4,ytmpp+68,
	     y128p+4,y128p+68,
	     tw512a_128p+4,
	     tw512b_128p+4);
    bfly2_16(ytmpp+5,ytmpp+69,
	     y128p+5,y128p+69,
	     tw512a_128p+5,
	     tw512b_128p+5);
    bfly2_16(ytmpp+6,ytmpp+70,
	     y128p+6,y128p+70,
	     tw512a_128p+6,
	     tw512b_128p+6);
    bfly2_16(ytmpp+7,ytmpp+71,
	     y128p+7,y128p+71,
	     tw512a_128p+7,
	     tw512b_128p+7);
    tw512a_128p+=8;
    tw512b_128p+=8;
    y128p+=8;
    ytmpp+=8;    
  }

  if (scale>0) {
    y128p = y128;
    for (i=0;i<8;i++) {
      y128p[0]  = _mm_mulhi_epi16(y128p[0],ONE_OVER_SQRT2_Q15_128);y128p[0] = _mm_slli_epi16(y128p[0],1);
      y128p[1]  = _mm_mulhi_epi16(y128p[1],ONE_OVER_SQRT2_Q15_128);y128p[1] = _mm_slli_epi16(y128p[1],1);
      y128p[2]  = _mm_mulhi_epi16(y128p[2],ONE_OVER_SQRT2_Q15_128);y128p[2] = _mm_slli_epi16(y128p[2],1);
      y128p[3]  = _mm_mulhi_epi16(y128p[3],ONE_OVER_SQRT2_Q15_128);y128p[3] = _mm_slli_epi16(y128p[3],1);
      y128p[4]  = _mm_mulhi_epi16(y128p[4],ONE_OVER_SQRT2_Q15_128);y128p[4] = _mm_slli_epi16(y128p[4],1);
      y128p[5]  = _mm_mulhi_epi16(y128p[5],ONE_OVER_SQRT2_Q15_128);y128p[5] = _mm_slli_epi16(y128p[5],1);
      y128p[6]  = _mm_mulhi_epi16(y128p[6],ONE_OVER_SQRT2_Q15_128);y128p[6] = _mm_slli_epi16(y128p[6],1);
      y128p[7]  = _mm_mulhi_epi16(y128p[7],ONE_OVER_SQRT2_Q15_128);y128p[7] = _mm_slli_epi16(y128p[7],1);
      y128p[8]  = _mm_mulhi_epi16(y128p[8],ONE_OVER_SQRT2_Q15_128);y128p[8] = _mm_slli_epi16(y128p[8],1);
      y128p[9]  = _mm_mulhi_epi16(y128p[9],ONE_OVER_SQRT2_Q15_128);y128p[9] = _mm_slli_epi16(y128p[9],1);
      y128p[10] = _mm_mulhi_epi16(y128p[10],ONE_OVER_SQRT2_Q15_128);y128p[10] = _mm_slli_epi16(y128p[10],1);
      y128p[11] = _mm_mulhi_epi16(y128p[11],ONE_OVER_SQRT2_Q15_128);y128p[11] = _mm_slli_epi16(y128p[11],1);
      y128p[12] = _mm_mulhi_epi16(y128p[12],ONE_OVER_SQRT2_Q15_128);y128p[12] = _mm_slli_epi16(y128p[12],1);
      y128p[13] = _mm_mulhi_epi16(y128p[13],ONE_OVER_SQRT2_Q15_128);y128p[13] = _mm_slli_epi16(y128p[13],1);
      y128p[14] = _mm_mulhi_epi16(y128p[14],ONE_OVER_SQRT2_Q15_128);y128p[14] = _mm_slli_epi16(y128p[14],1);
      y128p[15] = _mm_mulhi_epi16(y128p[15],ONE_OVER_SQRT2_Q15_128);y128p[15] = _mm_slli_epi16(y128p[15],1);
      y128p+=16;
    }
  }
  _mm_empty();
  _m_empty();

}

void idft512(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[256],*xtmpp,*x64 = (__m64 *)x;
  __m128i ytmp[128],*tw512_128p=(__m128i *)tw512,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  xtmpp = xtmp;

  for (i=0;i<4;i++) {
    transpose4_ooff(x64  ,xtmpp,128);
    transpose4_ooff(x64+2,xtmpp+1,128);
    transpose4_ooff(x64+4,xtmpp+2,128);
    transpose4_ooff(x64+6,xtmpp+3,128);
    transpose4_ooff(x64+8,xtmpp+4,128);
    transpose4_ooff(x64+10,xtmpp+5,128);
    transpose4_ooff(x64+12,xtmpp+6,128);
    transpose4_ooff(x64+14,xtmpp+7,128);
    transpose4_ooff(x64+16,xtmpp+8,128);
    transpose4_ooff(x64+18,xtmpp+9,128);
    transpose4_ooff(x64+20,xtmpp+10,128);
    transpose4_ooff(x64+22,xtmpp+11,128);
    transpose4_ooff(x64+24,xtmpp+12,128);
    transpose4_ooff(x64+26,xtmpp+13,128);
    transpose4_ooff(x64+28,xtmpp+14,128);
    transpose4_ooff(x64+30,xtmpp+15,128);
    transpose4_ooff(x64+32,xtmpp+16,128);
    transpose4_ooff(x64+34,xtmpp+17,128);
    transpose4_ooff(x64+36,xtmpp+18,128);
    transpose4_ooff(x64+38,xtmpp+19,128);
    transpose4_ooff(x64+40,xtmpp+20,128);
    transpose4_ooff(x64+42,xtmpp+21,128);
    transpose4_ooff(x64+44,xtmpp+22,128);
    transpose4_ooff(x64+46,xtmpp+23,128);
    transpose4_ooff(x64+48,xtmpp+24,128);
    transpose4_ooff(x64+50,xtmpp+25,128);
    transpose4_ooff(x64+52,xtmpp+26,128);
    transpose4_ooff(x64+54,xtmpp+27,128);
    transpose4_ooff(x64+56,xtmpp+28,128);
    transpose4_ooff(x64+58,xtmpp+29,128);
    transpose4_ooff(x64+60,xtmpp+30,128);
    transpose4_ooff(x64+62,xtmpp+31,128);
    x64+=64;
    xtmpp+=32;
  }

  idft256((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft256((int16_t*)(xtmp+128),(int16_t*)(ytmp+64),1);


  for (i=0;i<64;i++) {
    ibfly2(ytmpp,ytmpp+64,
	  y128p,y128p+64,
	  tw512_128p);
    tw512_128p++;
    y128p++;
    ytmpp++;    
  }

  if (scale>0) {
    y128p = y128;
    for (i=0;i<8;i++) {
      y128p[0]  = _mm_mulhi_epi16(y128p[0],ONE_OVER_SQRT2_Q15_128);y128p[0] = _mm_slli_epi16(y128p[0],1);
      y128p[1]  = _mm_mulhi_epi16(y128p[1],ONE_OVER_SQRT2_Q15_128);y128p[1] = _mm_slli_epi16(y128p[1],1);
      y128p[2]  = _mm_mulhi_epi16(y128p[2],ONE_OVER_SQRT2_Q15_128);y128p[2] = _mm_slli_epi16(y128p[2],1);
      y128p[3]  = _mm_mulhi_epi16(y128p[3],ONE_OVER_SQRT2_Q15_128);y128p[3] = _mm_slli_epi16(y128p[3],1);
      y128p[4]  = _mm_mulhi_epi16(y128p[4],ONE_OVER_SQRT2_Q15_128);y128p[4] = _mm_slli_epi16(y128p[4],1);
      y128p[5]  = _mm_mulhi_epi16(y128p[5],ONE_OVER_SQRT2_Q15_128);y128p[5] = _mm_slli_epi16(y128p[5],1);
      y128p[6]  = _mm_mulhi_epi16(y128p[6],ONE_OVER_SQRT2_Q15_128);y128p[6] = _mm_slli_epi16(y128p[6],1);
      y128p[7]  = _mm_mulhi_epi16(y128p[7],ONE_OVER_SQRT2_Q15_128);y128p[7] = _mm_slli_epi16(y128p[7],1);
      y128p[8]  = _mm_mulhi_epi16(y128p[8],ONE_OVER_SQRT2_Q15_128);y128p[8] = _mm_slli_epi16(y128p[8],1);
      y128p[9]  = _mm_mulhi_epi16(y128p[9],ONE_OVER_SQRT2_Q15_128);y128p[9] = _mm_slli_epi16(y128p[9],1);
      y128p[10] = _mm_mulhi_epi16(y128p[10],ONE_OVER_SQRT2_Q15_128);y128p[10] = _mm_slli_epi16(y128p[10],1);
      y128p[11] = _mm_mulhi_epi16(y128p[11],ONE_OVER_SQRT2_Q15_128);y128p[11] = _mm_slli_epi16(y128p[11],1);
      y128p[12] = _mm_mulhi_epi16(y128p[12],ONE_OVER_SQRT2_Q15_128);y128p[12] = _mm_slli_epi16(y128p[12],1);
      y128p[13] = _mm_mulhi_epi16(y128p[13],ONE_OVER_SQRT2_Q15_128);y128p[13] = _mm_slli_epi16(y128p[13],1);
      y128p[14] = _mm_mulhi_epi16(y128p[14],ONE_OVER_SQRT2_Q15_128);y128p[14] = _mm_slli_epi16(y128p[14],1);
      y128p[15] = _mm_mulhi_epi16(y128p[15],ONE_OVER_SQRT2_Q15_128);y128p[15] = _mm_slli_epi16(y128p[15],1);
      y128p+=16;
    }
  }
  _mm_empty();
  _m_empty();

}

int16_t tw1024[1536] __attribute__((aligned(16))) = {  32767,0,32766,-202,32764,-403,32761,-604,32757,-805,32751,-1006,32744,-1207,32736,-1407,32727,-1608,32717,-1809,32705,-2010,32692,-2210,32678,-2411,32662,-2611,32646,-2812,32628,-3012,32609,-3212,32588,-3412,32567,-3612,32544,-3812,32520,-4012,32495,-4211,32468,-4410,32441,-4609,32412,-4808,32382,-5007,32350,-5206,32318,-5404,32284,-5602,32249,-5800,32213,-5998,32176,-6196,32137,-6393,32097,-6590,32056,-6787,32014,-6983,31970,-7180,31926,-7376,31880,-7572,31833,-7767,31785,-7962,31735,-8157,31684,-8352,31633,-8546,31580,-8740,31525,-8933,31470,-9127,31413,-9320,31356,-9512,31297,-9704,31236,-9896,31175,-10088,31113,-10279,31049,-10470,30984,-10660,30918,-10850,30851,-11039,30783,-11228,30713,-11417,30643,-11605,30571,-11793,30498,-11981,30424,-12167,30349,-12354,30272,-12540,30195,-12725,30116,-12910,30036,-13095,29955,-13279,29873,-13463,29790,-13646,29706,-13828,29621,-14010,29534,-14192,29446,-14373,29358,-14553,29268,-14733,29177,-14912,29085,-15091,28992,-15269,28897,-15447,28802,-15624,28706,-15800,28608,-15976,28510,-16151,28410,-16326,28309,-16500,28208,-16673,28105,-16846,28001,-17018,27896,-17190,27790,-17361,27683,-17531,27575,-17700,27466,-17869,27355,-18037,27244,-18205,27132,-18372,27019,-18538,26905,-18703,26789,-18868,26673,-19032,26556,-19195,26437,-19358,26318,-19520,26198,-19681,26077,-19841,25954,-20001,25831,-20160,25707,-20318,25582,-20475,25456,-20632,25329,-20788,25201,-20943,25072,-21097,24942,-21250,24811,-21403,24679,-21555,24546,-21706,24413,-21856,24278,-22005,24143,-22154,24006,-22302,23869,-22449,23731,-22595,23592,-22740,23452,-22884,23311,-23028,23169,-23170,23027,-23312,22883,-23453,22739,-23593,22594,-23732,22448,-23870,22301,-24007,22153,-24144,22004,-24279,21855,-24414,21705,-24547,21554,-24680,21402,-24812,21249,-24943,21096,-25073,20942,-25202,20787,-25330,20631,-25457,20474,-25583,20317,-25708,20159,-25832,20000,-25955,19840,-26078,19680,-26199,19519,-26319,19357,-26438,19194,-26557,19031,-26674,18867,-26790,18702,-26906,18537,-27020,18371,-27133,18204,-27245,18036,-27356,17868,-27467,17699,-27576,17530,-27684,17360,-27791,17189,-27897,17017,-28002,16845,-28106,16672,-28209,16499,-28310,16325,-28411,16150,-28511,15975,-28609,15799,-28707,15623,-28803,15446,-28898,15268,-28993,15090,-29086,14911,-29178,14732,-29269,14552,-29359,14372,-29447,14191,-29535,14009,-29622,13827,-29707,13645,-29791,13462,-29874,13278,-29956,13094,-30037,12909,-30117,12724,-30196,12539,-30273,12353,-30350,12166,-30425,11980,-30499,11792,-30572,11604,-30644,11416,-30714,11227,-30784,11038,-30852,10849,-30919,10659,-30985,10469,-31050,10278,-31114,10087,-31176,9895,-31237,9703,-31298,9511,-31357,9319,-31414,9126,-31471,8932,-31526,8739,-31581,8545,-31634,8351,-31685,8156,-31736,7961,-31786,7766,-31834,7571,-31881,7375,-31927,7179,-31971,6982,-32015,6786,-32057,6589,-32098,6392,-32138,6195,-32177,5997,-32214,5799,-32250,5601,-32285,5403,-32319,5205,-32351,5006,-32383,4807,-32413,4608,-32442,4409,-32469,4210,-32496,4011,-32521,3811,-32545,3611,-32568,3411,-32589,3211,-32610,3011,-32629,2811,-32647,2610,-32663,2410,-32679,2209,-32693,2009,-32706,1808,-32718,1607,-32728,1406,-32737,1206,-32745,1005,-32752,804,-32758,603,-32762,402,-32765,201,-32767,
			 32767,0,32764,-403,32757,-805,32744,-1207,32727,-1608,32705,-2010,32678,-2411,32646,-2812,32609,-3212,32567,-3612,32520,-4012,32468,-4410,32412,-4808,32350,-5206,32284,-5602,32213,-5998,32137,-6393,32056,-6787,31970,-7180,31880,-7572,31785,-7962,31684,-8352,31580,-8740,31470,-9127,31356,-9512,31236,-9896,31113,-10279,30984,-10660,30851,-11039,30713,-11417,30571,-11793,30424,-12167,30272,-12540,30116,-12910,29955,-13279,29790,-13646,29621,-14010,29446,-14373,29268,-14733,29085,-15091,28897,-15447,28706,-15800,28510,-16151,28309,-16500,28105,-16846,27896,-17190,27683,-17531,27466,-17869,27244,-18205,27019,-18538,26789,-18868,26556,-19195,26318,-19520,26077,-19841,25831,-20160,25582,-20475,25329,-20788,25072,-21097,24811,-21403,24546,-21706,24278,-22005,24006,-22302,23731,-22595,23452,-22884,23169,-23170,22883,-23453,22594,-23732,22301,-24007,22004,-24279,21705,-24547,21402,-24812,21096,-25073,20787,-25330,20474,-25583,20159,-25832,19840,-26078,19519,-26319,19194,-26557,18867,-26790,18537,-27020,18204,-27245,17868,-27467,17530,-27684,17189,-27897,16845,-28106,16499,-28310,16150,-28511,15799,-28707,15446,-28898,15090,-29086,14732,-29269,14372,-29447,14009,-29622,13645,-29791,13278,-29956,12909,-30117,12539,-30273,12166,-30425,11792,-30572,11416,-30714,11038,-30852,10659,-30985,10278,-31114,9895,-31237,9511,-31357,9126,-31471,8739,-31581,8351,-31685,7961,-31786,7571,-31881,7179,-31971,6786,-32057,6392,-32138,5997,-32214,5601,-32285,5205,-32351,4807,-32413,4409,-32469,4011,-32521,3611,-32568,3211,-32610,2811,-32647,2410,-32679,2009,-32706,1607,-32728,1206,-32745,804,-32758,402,-32765,0,-32767,-403,-32765,-805,-32758,-1207,-32745,-1608,-32728,-2010,-32706,-2411,-32679,-2812,-32647,-3212,-32610,-3612,-32568,-4012,-32521,-4410,-32469,-4808,-32413,-5206,-32351,-5602,-32285,-5998,-32214,-6393,-32138,-6787,-32057,-7180,-31971,-7572,-31881,-7962,-31786,-8352,-31685,-8740,-31581,-9127,-31471,-9512,-31357,-9896,-31237,-10279,-31114,-10660,-30985,-11039,-30852,-11417,-30714,-11793,-30572,-12167,-30425,-12540,-30273,-12910,-30117,-13279,-29956,-13646,-29791,-14010,-29622,-14373,-29447,-14733,-29269,-15091,-29086,-15447,-28898,-15800,-28707,-16151,-28511,-16500,-28310,-16846,-28106,-17190,-27897,-17531,-27684,-17869,-27467,-18205,-27245,-18538,-27020,-18868,-26790,-19195,-26557,-19520,-26319,-19841,-26078,-20160,-25832,-20475,-25583,-20788,-25330,-21097,-25073,-21403,-24812,-21706,-24547,-22005,-24279,-22302,-24007,-22595,-23732,-22884,-23453,-23170,-23170,-23453,-22884,-23732,-22595,-24007,-22302,-24279,-22005,-24547,-21706,-24812,-21403,-25073,-21097,-25330,-20788,-25583,-20475,-25832,-20160,-26078,-19841,-26319,-19520,-26557,-19195,-26790,-18868,-27020,-18538,-27245,-18205,-27467,-17869,-27684,-17531,-27897,-17190,-28106,-16846,-28310,-16500,-28511,-16151,-28707,-15800,-28898,-15447,-29086,-15091,-29269,-14733,-29447,-14373,-29622,-14010,-29791,-13646,-29956,-13279,-30117,-12910,-30273,-12540,-30425,-12167,-30572,-11793,-30714,-11417,-30852,-11039,-30985,-10660,-31114,-10279,-31237,-9896,-31357,-9512,-31471,-9127,-31581,-8740,-31685,-8352,-31786,-7962,-31881,-7572,-31971,-7180,-32057,-6787,-32138,-6393,-32214,-5998,-32285,-5602,-32351,-5206,-32413,-4808,-32469,-4410,-32521,-4012,-32568,-3612,-32610,-3212,-32647,-2812,-32679,-2411,-32706,-2010,-32728,-1608,-32745,-1207,-32758,-805,-32765,-403,
			 32767,0,32761,-604,32744,-1207,32717,-1809,32678,-2411,32628,-3012,32567,-3612,32495,-4211,32412,-4808,32318,-5404,32213,-5998,32097,-6590,31970,-7180,31833,-7767,31684,-8352,31525,-8933,31356,-9512,31175,-10088,30984,-10660,30783,-11228,30571,-11793,30349,-12354,30116,-12910,29873,-13463,29621,-14010,29358,-14553,29085,-15091,28802,-15624,28510,-16151,28208,-16673,27896,-17190,27575,-17700,27244,-18205,26905,-18703,26556,-19195,26198,-19681,25831,-20160,25456,-20632,25072,-21097,24679,-21555,24278,-22005,23869,-22449,23452,-22884,23027,-23312,22594,-23732,22153,-24144,21705,-24547,21249,-24943,20787,-25330,20317,-25708,19840,-26078,19357,-26438,18867,-26790,18371,-27133,17868,-27467,17360,-27791,16845,-28106,16325,-28411,15799,-28707,15268,-28993,14732,-29269,14191,-29535,13645,-29791,13094,-30037,12539,-30273,11980,-30499,11416,-30714,10849,-30919,10278,-31114,9703,-31298,9126,-31471,8545,-31634,7961,-31786,7375,-31927,6786,-32057,6195,-32177,5601,-32285,5006,-32383,4409,-32469,3811,-32545,3211,-32610,2610,-32663,2009,-32706,1406,-32737,804,-32758,201,-32767,-403,-32765,-1006,-32752,-1608,-32728,-2210,-32693,-2812,-32647,-3412,-32589,-4012,-32521,-4609,-32442,-5206,-32351,-5800,-32250,-6393,-32138,-6983,-32015,-7572,-31881,-8157,-31736,-8740,-31581,-9320,-31414,-9896,-31237,-10470,-31050,-11039,-30852,-11605,-30644,-12167,-30425,-12725,-30196,-13279,-29956,-13828,-29707,-14373,-29447,-14912,-29178,-15447,-28898,-15976,-28609,-16500,-28310,-17018,-28002,-17531,-27684,-18037,-27356,-18538,-27020,-19032,-26674,-19520,-26319,-20001,-25955,-20475,-25583,-20943,-25202,-21403,-24812,-21856,-24414,-22302,-24007,-22740,-23593,-23170,-23170,-23593,-22740,-24007,-22302,-24414,-21856,-24812,-21403,-25202,-20943,-25583,-20475,-25955,-20001,-26319,-19520,-26674,-19032,-27020,-18538,-27356,-18037,-27684,-17531,-28002,-17018,-28310,-16500,-28609,-15976,-28898,-15447,-29178,-14912,-29447,-14373,-29707,-13828,-29956,-13279,-30196,-12725,-30425,-12167,-30644,-11605,-30852,-11039,-31050,-10470,-31237,-9896,-31414,-9320,-31581,-8740,-31736,-8157,-31881,-7572,-32015,-6983,-32138,-6393,-32250,-5800,-32351,-5206,-32442,-4609,-32521,-4012,-32589,-3412,-32647,-2812,-32693,-2210,-32728,-1608,-32752,-1006,-32765,-403,-32767,201,-32758,804,-32737,1406,-32706,2009,-32663,2610,-32610,3211,-32545,3811,-32469,4409,-32383,5006,-32285,5601,-32177,6195,-32057,6786,-31927,7375,-31786,7961,-31634,8545,-31471,9126,-31298,9703,-31114,10278,-30919,10849,-30714,11416,-30499,11980,-30273,12539,-30037,13094,-29791,13645,-29535,14191,-29269,14732,-28993,15268,-28707,15799,-28411,16325,-28106,16845,-27791,17360,-27467,17868,-27133,18371,-26790,18867,-26438,19357,-26078,19840,-25708,20317,-25330,20787,-24943,21249,-24547,21705,-24144,22153,-23732,22594,-23312,23027,-22884,23452,-22449,23869,-22005,24278,-21555,24679,-21097,25072,-20632,25456,-20160,25831,-19681,26198,-19195,26556,-18703,26905,-18205,27244,-17700,27575,-17190,27896,-16673,28208,-16151,28510,-15624,28802,-15091,29085,-14553,29358,-14010,29621,-13463,29873,-12910,30116,-12354,30349,-11793,30571,-11228,30783,-10660,30984,-10088,31175,-9512,31356,-8933,31525,-8352,31684,-7767,31833,-7180,31970,-6590,32097,-5998,32213,-5404,32318,-4808,32412,-4211,32495,-3612,32567,-3012,32628,-2411,32678,-1809,32717,-1207,32744,-604,32761};

void dft1024(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[256],ytmp[256],*tw1024_128p=(__m128i *)tw1024,*x128=(__m128i *)x,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i,j;
  
  for (i=0,j=0;i<256;i+=4,j++) {
    transpose16_ooff(x128+i,xtmp+j,64);
  }
  

  dft256((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  dft256((int16_t*)(xtmp+64),(int16_t*)(ytmp+64),1);
  dft256((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);
  dft256((int16_t*)(xtmp+192),(int16_t*)(ytmp+192),1);

  for (i=0;i<64;i++) {
    bfly4(ytmpp,ytmpp+64,ytmpp+128,ytmpp+192,
	  y128p,y128p+64,y128p+128,y128p+192,
	  tw1024_128p,tw1024_128p+64,tw1024_128p+128);
    tw1024_128p++;
    y128p++;
    ytmpp++;
  }
    
  if (scale>0) {

    for (i=0;i<16;i++) {
      y128[0]  = _mm_srai_epi16(y128[0],1);
      y128[1]  = _mm_srai_epi16(y128[1],1);
      y128[2]  = _mm_srai_epi16(y128[2],1);
      y128[3]  = _mm_srai_epi16(y128[3],1);
      y128[4]  = _mm_srai_epi16(y128[4],1);
      y128[5]  = _mm_srai_epi16(y128[5],1);
      y128[6]  = _mm_srai_epi16(y128[6],1);
      y128[7]  = _mm_srai_epi16(y128[7],1);
      y128[8]  = _mm_srai_epi16(y128[8],1);
      y128[9]  = _mm_srai_epi16(y128[9],1);
      y128[10] = _mm_srai_epi16(y128[10],1);
      y128[11] = _mm_srai_epi16(y128[11],1);
      y128[12] = _mm_srai_epi16(y128[12],1);
      y128[13] = _mm_srai_epi16(y128[13],1);
      y128[14] = _mm_srai_epi16(y128[14],1);
      y128[15] = _mm_srai_epi16(y128[15],1);

      y128+=16;
    }

  }
  _mm_empty();
  _m_empty();

}

void idft1024(int16_t *x,int16_t *y,int scale) {

  __m128i xtmp[256],ytmp[256],*tw1024_128p=(__m128i *)tw1024,*x128=(__m128i *)x,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i,j;
  
  for (i=0,j=0;i<256;i+=4,j++) {
    transpose16_ooff(x128+i,xtmp+j,64);
  }
  

  idft256((int16_t*)(xtmp),(int16_t*)(ytmp),1);
  idft256((int16_t*)(xtmp+64),(int16_t*)(ytmp+64),1);
  idft256((int16_t*)(xtmp+128),(int16_t*)(ytmp+128),1);
  idft256((int16_t*)(xtmp+192),(int16_t*)(ytmp+192),1);

  for (i=0;i<64;i++) {
    ibfly4(ytmpp,ytmpp+64,ytmpp+128,ytmpp+192,
	   y128p,y128p+64,y128p+128,y128p+192,
	   tw1024_128p,tw1024_128p+64,tw1024_128p+128);
    tw1024_128p++;
    y128p++;
    ytmpp++;
  }
    
  if (scale>0) {

    for (i=0;i<16;i++) {
      y128[0]  = _mm_srai_epi16(y128[0],1);
      y128[1]  = _mm_srai_epi16(y128[1],1);
      y128[2]  = _mm_srai_epi16(y128[2],1);
      y128[3]  = _mm_srai_epi16(y128[3],1);
      y128[4]  = _mm_srai_epi16(y128[4],1);
      y128[5]  = _mm_srai_epi16(y128[5],1);
      y128[6]  = _mm_srai_epi16(y128[6],1);
      y128[7]  = _mm_srai_epi16(y128[7],1);
      y128[8]  = _mm_srai_epi16(y128[8],1);
      y128[9]  = _mm_srai_epi16(y128[9],1);
      y128[10] = _mm_srai_epi16(y128[10],1);
      y128[11] = _mm_srai_epi16(y128[11],1);
      y128[12] = _mm_srai_epi16(y128[12],1);
      y128[13] = _mm_srai_epi16(y128[13],1);
      y128[14] = _mm_srai_epi16(y128[14],1);
      y128[15] = _mm_srai_epi16(y128[15],1);

      y128+=16;
    }

  }
  _mm_empty();
  _m_empty();

}

int16_t tw2048[2048] __attribute__((aligned(16))) = {32767,0,32766,-101,32766,-202,32765,-302,32764,-403,32763,-503,32761,-604,32759,-704,32757,-805,32754,-905,32751,-1006,32748,-1106,32744,-1207,32740,-1307,32736,-1407,32732,-1508,32727,-1608,32722,-1709,32717,-1809,32711,-1909,32705,-2010,32699,-2110,32692,-2210,32685,-2311,32678,-2411,32670,-2511,32662,-2611,32654,-2712,32646,-2812,32637,-2912,32628,-3012,32618,-3112,32609,-3212,32599,-3312,32588,-3412,32578,-3512,32567,-3612,32556,-3712,32544,-3812,32532,-3912,32520,-4012,32508,-4111,32495,-4211,32482,-4311,32468,-4410,32455,-4510,32441,-4609,32426,-4709,32412,-4808,32397,-4908,32382,-5007,32366,-5107,32350,-5206,32334,-5305,32318,-5404,32301,-5503,32284,-5602,32267,-5701,32249,-5800,32231,-5899,32213,-5998,32194,-6097,32176,-6196,32156,-6294,32137,-6393,32117,-6492,32097,-6590,32077,-6689,32056,-6787,32035,-6885,32014,-6983,31992,-7082,31970,-7180,31948,-7278,31926,-7376,31903,-7474,31880,-7572,31856,-7669,31833,-7767,31809,-7865,31785,-7962,31760,-8060,31735,-8157,31710,-8254,31684,-8352,31659,-8449,31633,-8546,31606,-8643,31580,-8740,31553,-8837,31525,-8933,31498,-9030,31470,-9127,31442,-9223,31413,-9320,31385,-9416,31356,-9512,31326,-9608,31297,-9704,31267,-9800,31236,-9896,31206,-9992,31175,-10088,31144,-10183,31113,-10279,31081,-10374,31049,-10470,31017,-10565,30984,-10660,30951,-10755,30918,-10850,30885,-10945,30851,-11039,30817,-11134,30783,-11228,30748,-11323,30713,-11417,30678,-11511,30643,-11605,30607,-11699,30571,-11793,30535,-11887,30498,-11981,30461,-12074,30424,-12167,30386,-12261,30349,-12354,30311,-12447,30272,-12540,30234,-12633,30195,-12725,30156,-12818,30116,-12910,30076,-13003,30036,-13095,29996,-13187,29955,-13279,29915,-13371,29873,-13463,29832,-13554,29790,-13646,29748,-13737,29706,-13828,29663,-13919,29621,-14010,29577,-14101,29534,-14192,29490,-14282,29446,-14373,29402,-14463,29358,-14553,29313,-14643,29268,-14733,29222,-14823,29177,-14912,29131,-15002,29085,-15091,29038,-15180,28992,-15269,28945,-15358,28897,-15447,28850,-15535,28802,-15624,28754,-15712,28706,-15800,28657,-15888,28608,-15976,28559,-16064,28510,-16151,28460,-16239,28410,-16326,28360,-16413,28309,-16500,28259,-16587,28208,-16673,28156,-16760,28105,-16846,28053,-16932,28001,-17018,27948,-17104,27896,-17190,27843,-17275,27790,-17361,27736,-17446,27683,-17531,27629,-17616,27575,-17700,27520,-17785,27466,-17869,27411,-17953,27355,-18037,27300,-18121,27244,-18205,27188,-18288,27132,-18372,27076,-18455,27019,-18538,26962,-18621,26905,-18703,26847,-18786,26789,-18868,26731,-18950,26673,-19032,26615,-19114,26556,-19195,26497,-19277,26437,-19358,26378,-19439,26318,-19520,26258,-19600,26198,-19681,26137,-19761,26077,-19841,26016,-19921,25954,-20001,25893,-20080,25831,-20160,25769,-20239,25707,-20318,25645,-20397,25582,-20475,25519,-20554,25456,-20632,25392,-20710,25329,-20788,25265,-20865,25201,-20943,25136,-21020,25072,-21097,25007,-21174,24942,-21250,24877,-21327,24811,-21403,24745,-21479,24679,-21555,24613,-21630,24546,-21706,24480,-21781,24413,-21856,24346,-21931,24278,-22005,24211,-22080,24143,-22154,24075,-22228,24006,-22302,23938,-22375,23869,-22449,23800,-22522,23731,-22595,23661,-22667,23592,-22740,23522,-22812,23452,-22884,23382,-22956,23311,-23028,23240,-23099,23169,-23170,23098,-23241,23027,-23312,22955,-23383,22883,-23453,22811,-23523,22739,-23593,22666,-23662,22594,-23732,22521,-23801,22448,-23870,22374,-23939,22301,-24007,22227,-24076,22153,-24144,22079,-24212,22004,-24279,21930,-24347,21855,-24414,21780,-24481,21705,-24547,21629,-24614,21554,-24680,21478,-24746,21402,-24812,21326,-24878,21249,-24943,21173,-25008,21096,-25073,21019,-25137,20942,-25202,20864,-25266,20787,-25330,20709,-25393,20631,-25457,20553,-25520,20474,-25583,20396,-25646,20317,-25708,20238,-25770,20159,-25832,20079,-25894,20000,-25955,19920,-26017,19840,-26078,19760,-26138,19680,-26199,19599,-26259,19519,-26319,19438,-26379,19357,-26438,19276,-26498,19194,-26557,19113,-26616,19031,-26674,18949,-26732,18867,-26790,18785,-26848,18702,-26906,18620,-26963,18537,-27020,18454,-27077,18371,-27133,18287,-27189,18204,-27245,18120,-27301,18036,-27356,17952,-27412,17868,-27467,17784,-27521,17699,-27576,17615,-27630,17530,-27684,17445,-27737,17360,-27791,17274,-27844,17189,-27897,17103,-27949,17017,-28002,16931,-28054,16845,-28106,16759,-28157,16672,-28209,16586,-28260,16499,-28310,16412,-28361,16325,-28411,16238,-28461,16150,-28511,16063,-28560,15975,-28609,15887,-28658,15799,-28707,15711,-28755,15623,-28803,15534,-28851,15446,-28898,15357,-28946,15268,-28993,15179,-29039,15090,-29086,15001,-29132,14911,-29178,14822,-29223,14732,-29269,14642,-29314,14552,-29359,14462,-29403,14372,-29447,14281,-29491,14191,-29535,14100,-29578,14009,-29622,13918,-29664,13827,-29707,13736,-29749,13645,-29791,13553,-29833,13462,-29874,13370,-29916,13278,-29956,13186,-29997,13094,-30037,13002,-30077,12909,-30117,12817,-30157,12724,-30196,12632,-30235,12539,-30273,12446,-30312,12353,-30350,12260,-30387,12166,-30425,12073,-30462,11980,-30499,11886,-30536,11792,-30572,11698,-30608,11604,-30644,11510,-30679,11416,-30714,11322,-30749,11227,-30784,11133,-30818,11038,-30852,10944,-30886,10849,-30919,10754,-30952,10659,-30985,10564,-31018,10469,-31050,10373,-31082,10278,-31114,10182,-31145,10087,-31176,9991,-31207,9895,-31237,9799,-31268,9703,-31298,9607,-31327,9511,-31357,9415,-31386,9319,-31414,9222,-31443,9126,-31471,9029,-31499,8932,-31526,8836,-31554,8739,-31581,8642,-31607,8545,-31634,8448,-31660,8351,-31685,8253,-31711,8156,-31736,8059,-31761,7961,-31786,7864,-31810,7766,-31834,7668,-31857,7571,-31881,7473,-31904,7375,-31927,7277,-31949,7179,-31971,7081,-31993,6982,-32015,6884,-32036,6786,-32057,6688,-32078,6589,-32098,6491,-32118,6392,-32138,6293,-32157,6195,-32177,6096,-32195,5997,-32214,5898,-32232,5799,-32250,5700,-32268,5601,-32285,5502,-32302,5403,-32319,5304,-32335,5205,-32351,5106,-32367,5006,-32383,4907,-32398,4807,-32413,4708,-32427,4608,-32442,4509,-32456,4409,-32469,4310,-32483,4210,-32496,4110,-32509,4011,-32521,3911,-32533,3811,-32545,3711,-32557,3611,-32568,3511,-32579,3411,-32589,3311,-32600,3211,-32610,3111,-32619,3011,-32629,2911,-32638,2811,-32647,2711,-32655,2610,-32663,2510,-32671,2410,-32679,2310,-32686,2209,-32693,2109,-32700,2009,-32706,1908,-32712,1808,-32718,1708,-32723,1607,-32728,1507,-32733,1406,-32737,1306,-32741,1206,-32745,1105,-32749,1005,-32752,904,-32755,804,-32758,703,-32760,603,-32762,502,-32764,402,-32765,301,-32766,201,-32767,100,-32767,0,-32767,-101,-32767,-202,-32767,-302,-32766,-403,-32765,-503,-32764,-604,-32762,-704,-32760,-805,-32758,-905,-32755,-1006,-32752,-1106,-32749,-1207,-32745,-1307,-32741,-1407,-32737,-1508,-32733,-1608,-32728,-1709,-32723,-1809,-32718,-1909,-32712,-2010,-32706,-2110,-32700,-2210,-32693,-2311,-32686,-2411,-32679,-2511,-32671,-2611,-32663,-2712,-32655,-2812,-32647,-2912,-32638,-3012,-32629,-3112,-32619,-3212,-32610,-3312,-32600,-3412,-32589,-3512,-32579,-3612,-32568,-3712,-32557,-3812,-32545,-3912,-32533,-4012,-32521,-4111,-32509,-4211,-32496,-4311,-32483,-4410,-32469,-4510,-32456,-4609,-32442,-4709,-32427,-4808,-32413,-4908,-32398,-5007,-32383,-5107,-32367,-5206,-32351,-5305,-32335,-5404,-32319,-5503,-32302,-5602,-32285,-5701,-32268,-5800,-32250,-5899,-32232,-5998,-32214,-6097,-32195,-6196,-32177,-6294,-32157,-6393,-32138,-6492,-32118,-6590,-32098,-6689,-32078,-6787,-32057,-6885,-32036,-6983,-32015,-7082,-31993,-7180,-31971,-7278,-31949,-7376,-31927,-7474,-31904,-7572,-31881,-7669,-31857,-7767,-31834,-7865,-31810,-7962,-31786,-8060,-31761,-8157,-31736,-8254,-31711,-8352,-31685,-8449,-31660,-8546,-31634,-8643,-31607,-8740,-31581,-8837,-31554,-8933,-31526,-9030,-31499,-9127,-31471,-9223,-31443,-9320,-31414,-9416,-31386,-9512,-31357,-9608,-31327,-9704,-31298,-9800,-31268,-9896,-31237,-9992,-31207,-10088,-31176,-10183,-31145,-10279,-31114,-10374,-31082,-10470,-31050,-10565,-31018,-10660,-30985,-10755,-30952,-10850,-30919,-10945,-30886,-11039,-30852,-11134,-30818,-11228,-30784,-11323,-30749,-11417,-30714,-11511,-30679,-11605,-30644,-11699,-30608,-11793,-30572,-11887,-30536,-11981,-30499,-12074,-30462,-12167,-30425,-12261,-30387,-12354,-30350,-12447,-30312,-12540,-30273,-12633,-30235,-12725,-30196,-12818,-30157,-12910,-30117,-13003,-30077,-13095,-30037,-13187,-29997,-13279,-29956,-13371,-29916,-13463,-29874,-13554,-29833,-13646,-29791,-13737,-29749,-13828,-29707,-13919,-29664,-14010,-29622,-14101,-29578,-14192,-29535,-14282,-29491,-14373,-29447,-14463,-29403,-14553,-29359,-14643,-29314,-14733,-29269,-14823,-29223,-14912,-29178,-15002,-29132,-15091,-29086,-15180,-29039,-15269,-28993,-15358,-28946,-15447,-28898,-15535,-28851,-15624,-28803,-15712,-28755,-15800,-28707,-15888,-28658,-15976,-28609,-16064,-28560,-16151,-28511,-16239,-28461,-16326,-28411,-16413,-28361,-16500,-28310,-16587,-28260,-16673,-28209,-16760,-28157,-16846,-28106,-16932,-28054,-17018,-28002,-17104,-27949,-17190,-27897,-17275,-27844,-17361,-27791,-17446,-27737,-17531,-27684,-17616,-27630,-17700,-27576,-17785,-27521,-17869,-27467,-17953,-27412,-18037,-27356,-18121,-27301,-18205,-27245,-18288,-27189,-18372,-27133,-18455,-27077,-18538,-27020,-18621,-26963,-18703,-26906,-18786,-26848,-18868,-26790,-18950,-26732,-19032,-26674,-19114,-26616,-19195,-26557,-19277,-26498,-19358,-26438,-19439,-26379,-19520,-26319,-19600,-26259,-19681,-26199,-19761,-26138,-19841,-26078,-19921,-26017,-20001,-25955,-20080,-25894,-20160,-25832,-20239,-25770,-20318,-25708,-20397,-25646,-20475,-25583,-20554,-25520,-20632,-25457,-20710,-25393,-20788,-25330,-20865,-25266,-20943,-25202,-21020,-25137,-21097,-25073,-21174,-25008,-21250,-24943,-21327,-24878,-21403,-24812,-21479,-24746,-21555,-24680,-21630,-24614,-21706,-24547,-21781,-24481,-21856,-24414,-21931,-24347,-22005,-24279,-22080,-24212,-22154,-24144,-22228,-24076,-22302,-24007,-22375,-23939,-22449,-23870,-22522,-23801,-22595,-23732,-22667,-23662,-22740,-23593,-22812,-23523,-22884,-23453,-22956,-23383,-23028,-23312,-23099,-23241,-23170,-23170,-23241,-23099,-23312,-23028,-23383,-22956,-23453,-22884,-23523,-22812,-23593,-22740,-23662,-22667,-23732,-22595,-23801,-22522,-23870,-22449,-23939,-22375,-24007,-22302,-24076,-22228,-24144,-22154,-24212,-22080,-24279,-22005,-24347,-21931,-24414,-21856,-24481,-21781,-24547,-21706,-24614,-21630,-24680,-21555,-24746,-21479,-24812,-21403,-24878,-21327,-24943,-21250,-25008,-21174,-25073,-21097,-25137,-21020,-25202,-20943,-25266,-20865,-25330,-20788,-25393,-20710,-25457,-20632,-25520,-20554,-25583,-20475,-25646,-20397,-25708,-20318,-25770,-20239,-25832,-20160,-25894,-20080,-25955,-20001,-26017,-19921,-26078,-19841,-26138,-19761,-26199,-19681,-26259,-19600,-26319,-19520,-26379,-19439,-26438,-19358,-26498,-19277,-26557,-19195,-26616,-19114,-26674,-19032,-26732,-18950,-26790,-18868,-26848,-18786,-26906,-18703,-26963,-18621,-27020,-18538,-27077,-18455,-27133,-18372,-27189,-18288,-27245,-18205,-27301,-18121,-27356,-18037,-27412,-17953,-27467,-17869,-27521,-17785,-27576,-17700,-27630,-17616,-27684,-17531,-27737,-17446,-27791,-17361,-27844,-17275,-27897,-17190,-27949,-17104,-28002,-17018,-28054,-16932,-28106,-16846,-28157,-16760,-28209,-16673,-28260,-16587,-28310,-16500,-28361,-16413,-28411,-16326,-28461,-16239,-28511,-16151,-28560,-16064,-28609,-15976,-28658,-15888,-28707,-15800,-28755,-15712,-28803,-15624,-28851,-15535,-28898,-15447,-28946,-15358,-28993,-15269,-29039,-15180,-29086,-15091,-29132,-15002,-29178,-14912,-29223,-14823,-29269,-14733,-29314,-14643,-29359,-14553,-29403,-14463,-29447,-14373,-29491,-14282,-29535,-14192,-29578,-14101,-29622,-14010,-29664,-13919,-29707,-13828,-29749,-13737,-29791,-13646,-29833,-13554,-29874,-13463,-29916,-13371,-29956,-13279,-29997,-13187,-30037,-13095,-30077,-13003,-30117,-12910,-30157,-12818,-30196,-12725,-30235,-12633,-30273,-12540,-30312,-12447,-30350,-12354,-30387,-12261,-30425,-12167,-30462,-12074,-30499,-11981,-30536,-11887,-30572,-11793,-30608,-11699,-30644,-11605,-30679,-11511,-30714,-11417,-30749,-11323,-30784,-11228,-30818,-11134,-30852,-11039,-30886,-10945,-30919,-10850,-30952,-10755,-30985,-10660,-31018,-10565,-31050,-10470,-31082,-10374,-31114,-10279,-31145,-10183,-31176,-10088,-31207,-9992,-31237,-9896,-31268,-9800,-31298,-9704,-31327,-9608,-31357,-9512,-31386,-9416,-31414,-9320,-31443,-9223,-31471,-9127,-31499,-9030,-31526,-8933,-31554,-8837,-31581,-8740,-31607,-8643,-31634,-8546,-31660,-8449,-31685,-8352,-31711,-8254,-31736,-8157,-31761,-8060,-31786,-7962,-31810,-7865,-31834,-7767,-31857,-7669,-31881,-7572,-31904,-7474,-31927,-7376,-31949,-7278,-31971,-7180,-31993,-7082,-32015,-6983,-32036,-6885,-32057,-6787,-32078,-6689,-32098,-6590,-32118,-6492,-32138,-6393,-32157,-6294,-32177,-6196,-32195,-6097,-32214,-5998,-32232,-5899,-32250,-5800,-32268,-5701,-32285,-5602,-32302,-5503,-32319,-5404,-32335,-5305,-32351,-5206,-32367,-5107,-32383,-5007,-32398,-4908,-32413,-4808,-32427,-4709,-32442,-4609,-32456,-4510,-32469,-4410,-32483,-4311,-32496,-4211,-32509,-4111,-32521,-4012,-32533,-3912,-32545,-3812,-32557,-3712,-32568,-3612,-32579,-3512,-32589,-3412,-32600,-3312,-32610,-3212,-32619,-3112,-32629,-3012,-32638,-2912,-32647,-2812,-32655,-2712,-32663,-2611,-32671,-2511,-32679,-2411,-32686,-2311,-32693,-2210,-32700,-2110,-32706,-2010,-32712,-1909,-32718,-1809,-32723,-1709,-32728,-1608,-32733,-1508,-32737,-1407,-32741,-1307,-32745,-1207,-32749,-1106,-32752,-1006,-32755,-905,-32758,-805,-32760,-704,-32762,-604,-32764,-503,-32765,-403,-32766,-302,-32767,-202,-32767,-101};


void dft2048(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[2048],*xtmpp,*x64 = (__m64 *)x;
  __m128i ytmp[512],*tw2048_128p=(__m128i *)tw2048,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  xtmpp = xtmp;

  for (i=0;i<16;i++) {
    transpose4_ooff(x64  ,xtmpp,512);
    transpose4_ooff(x64+2,xtmpp+1,512);
    transpose4_ooff(x64+4,xtmpp+2,512);
    transpose4_ooff(x64+6,xtmpp+3,512);
    transpose4_ooff(x64+8,xtmpp+4,512);
    transpose4_ooff(x64+10,xtmpp+5,512);
    transpose4_ooff(x64+12,xtmpp+6,512);
    transpose4_ooff(x64+14,xtmpp+7,512);
    transpose4_ooff(x64+16,xtmpp+8,512);
    transpose4_ooff(x64+18,xtmpp+9,512);
    transpose4_ooff(x64+20,xtmpp+10,512);
    transpose4_ooff(x64+22,xtmpp+11,512);
    transpose4_ooff(x64+24,xtmpp+12,512);
    transpose4_ooff(x64+26,xtmpp+13,512);
    transpose4_ooff(x64+28,xtmpp+14,512);
    transpose4_ooff(x64+30,xtmpp+15,512);
    transpose4_ooff(x64+32,xtmpp+16,512);
    transpose4_ooff(x64+34,xtmpp+17,512);
    transpose4_ooff(x64+36,xtmpp+18,512);
    transpose4_ooff(x64+38,xtmpp+19,512);
    transpose4_ooff(x64+40,xtmpp+20,512);
    transpose4_ooff(x64+42,xtmpp+21,512);
    transpose4_ooff(x64+44,xtmpp+22,512);
    transpose4_ooff(x64+46,xtmpp+23,512);
    transpose4_ooff(x64+48,xtmpp+24,512);
    transpose4_ooff(x64+50,xtmpp+25,512);
    transpose4_ooff(x64+52,xtmpp+26,512);
    transpose4_ooff(x64+54,xtmpp+27,512);
    transpose4_ooff(x64+56,xtmpp+28,512);
    transpose4_ooff(x64+58,xtmpp+29,512);
    transpose4_ooff(x64+60,xtmpp+30,512);
    transpose4_ooff(x64+62,xtmpp+31,512);
    x64+=64;
    xtmpp+=32;
  }

  dft1024((int16_t*)(xtmp),(int16_t*)ytmp,1);
  dft1024((int16_t*)(xtmp+512),(int16_t*)(ytmp+256),1);


  for (i=0;i<256;i++) {
    bfly2(ytmpp,ytmpp+256,
	  y128p,y128p+256,
	  tw2048_128p);
    tw2048_128p++;
    y128p++;
    ytmpp++;    
  }

  if (scale>0) {
    y128p = y128;
    for (i=0;i<32;i++) {
      y128p[0]  = _mm_mulhi_epi16(y128p[0],ONE_OVER_SQRT2_Q15_128);y128p[0] = _mm_slli_epi16(y128p[0],1);
      y128p[1]  = _mm_mulhi_epi16(y128p[1],ONE_OVER_SQRT2_Q15_128);y128p[1] = _mm_slli_epi16(y128p[1],1);
      y128p[2]  = _mm_mulhi_epi16(y128p[2],ONE_OVER_SQRT2_Q15_128);y128p[2] = _mm_slli_epi16(y128p[2],1);
      y128p[3]  = _mm_mulhi_epi16(y128p[3],ONE_OVER_SQRT2_Q15_128);y128p[3] = _mm_slli_epi16(y128p[3],1);
      y128p[4]  = _mm_mulhi_epi16(y128p[4],ONE_OVER_SQRT2_Q15_128);y128p[4] = _mm_slli_epi16(y128p[4],1);
      y128p[5]  = _mm_mulhi_epi16(y128p[5],ONE_OVER_SQRT2_Q15_128);y128p[5] = _mm_slli_epi16(y128p[5],1);
      y128p[6]  = _mm_mulhi_epi16(y128p[6],ONE_OVER_SQRT2_Q15_128);y128p[6] = _mm_slli_epi16(y128p[6],1);
      y128p[7]  = _mm_mulhi_epi16(y128p[7],ONE_OVER_SQRT2_Q15_128);y128p[7] = _mm_slli_epi16(y128p[7],1);
      y128p[8]  = _mm_mulhi_epi16(y128p[8],ONE_OVER_SQRT2_Q15_128);y128p[8] = _mm_slli_epi16(y128p[8],1);
      y128p[9]  = _mm_mulhi_epi16(y128p[9],ONE_OVER_SQRT2_Q15_128);y128p[9] = _mm_slli_epi16(y128p[9],1);
      y128p[10] = _mm_mulhi_epi16(y128p[10],ONE_OVER_SQRT2_Q15_128);y128p[10] = _mm_slli_epi16(y128p[10],1);
      y128p[11] = _mm_mulhi_epi16(y128p[11],ONE_OVER_SQRT2_Q15_128);y128p[11] = _mm_slli_epi16(y128p[11],1);
      y128p[12] = _mm_mulhi_epi16(y128p[12],ONE_OVER_SQRT2_Q15_128);y128p[12] = _mm_slli_epi16(y128p[12],1);
      y128p[13] = _mm_mulhi_epi16(y128p[13],ONE_OVER_SQRT2_Q15_128);y128p[13] = _mm_slli_epi16(y128p[13],1);
      y128p[14] = _mm_mulhi_epi16(y128p[14],ONE_OVER_SQRT2_Q15_128);y128p[14] = _mm_slli_epi16(y128p[14],1);
      y128p[15] = _mm_mulhi_epi16(y128p[15],ONE_OVER_SQRT2_Q15_128);y128p[15] = _mm_slli_epi16(y128p[15],1);
      y128p+=16;
    }
  }
  _mm_empty();
  _m_empty();

}

void idft2048(int16_t *x,int16_t *y,int scale) {

  __m64 xtmp[2048],*xtmpp,*x64 = (__m64 *)x;
  __m128i ytmp[512],*tw2048_128p=(__m128i *)tw2048,*y128=(__m128i *)y,*y128p=(__m128i *)y;
  __m128i *ytmpp = &ytmp[0];
  int i;
  __m128i ONE_OVER_SQRT2_Q15_128 = _mm_set_epi16(ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15,
						 ONE_OVER_SQRT2_Q15);

  xtmpp = xtmp;

  for (i=0;i<16;i++) {
    transpose4_ooff(x64  ,xtmpp,512);
    transpose4_ooff(x64+2,xtmpp+1,512);
    transpose4_ooff(x64+4,xtmpp+2,512);
    transpose4_ooff(x64+6,xtmpp+3,512);
    transpose4_ooff(x64+8,xtmpp+4,512);
    transpose4_ooff(x64+10,xtmpp+5,512);
    transpose4_ooff(x64+12,xtmpp+6,512);
    transpose4_ooff(x64+14,xtmpp+7,512);
    transpose4_ooff(x64+16,xtmpp+8,512);
    transpose4_ooff(x64+18,xtmpp+9,512);
    transpose4_ooff(x64+20,xtmpp+10,512);
    transpose4_ooff(x64+22,xtmpp+11,512);
    transpose4_ooff(x64+24,xtmpp+12,512);
    transpose4_ooff(x64+26,xtmpp+13,512);
    transpose4_ooff(x64+28,xtmpp+14,512);
    transpose4_ooff(x64+30,xtmpp+15,512);
    transpose4_ooff(x64+32,xtmpp+16,512);
    transpose4_ooff(x64+34,xtmpp+17,512);
    transpose4_ooff(x64+36,xtmpp+18,512);
    transpose4_ooff(x64+38,xtmpp+19,512);
    transpose4_ooff(x64+40,xtmpp+20,512);
    transpose4_ooff(x64+42,xtmpp+21,512);
    transpose4_ooff(x64+44,xtmpp+22,512);
    transpose4_ooff(x64+46,xtmpp+23,512);
    transpose4_ooff(x64+48,xtmpp+24,512);
    transpose4_ooff(x64+50,xtmpp+25,512);
    transpose4_ooff(x64+52,xtmpp+26,512);
    transpose4_ooff(x64+54,xtmpp+27,512);
    transpose4_ooff(x64+56,xtmpp+28,512);
    transpose4_ooff(x64+58,xtmpp+29,512);
    transpose4_ooff(x64+60,xtmpp+30,512);
    transpose4_ooff(x64+62,xtmpp+31,512);
    x64+=64;
    xtmpp+=32;
  }

  idft1024((int16_t*)(xtmp),(int16_t*)ytmp,1);
  idft1024((int16_t*)(xtmp+512),(int16_t*)(ytmp+256),1);


  for (i=0;i<256;i++) {
    ibfly2(ytmpp,ytmpp+256,
	   y128p,y128p+256,
	   tw2048_128p);
    tw2048_128p++;
    y128p++;
    ytmpp++;    
  }

  if (scale>0) {
    y128p = y128;
    for (i=0;i<32;i++) {
      y128p[0]  = _mm_mulhi_epi16(y128p[0],ONE_OVER_SQRT2_Q15_128);y128p[0] = _mm_slli_epi16(y128p[0],1);
      y128p[1]  = _mm_mulhi_epi16(y128p[1],ONE_OVER_SQRT2_Q15_128);y128p[1] = _mm_slli_epi16(y128p[1],1);
      y128p[2]  = _mm_mulhi_epi16(y128p[2],ONE_OVER_SQRT2_Q15_128);y128p[2] = _mm_slli_epi16(y128p[2],1);
      y128p[3]  = _mm_mulhi_epi16(y128p[3],ONE_OVER_SQRT2_Q15_128);y128p[3] = _mm_slli_epi16(y128p[3],1);
      y128p[4]  = _mm_mulhi_epi16(y128p[4],ONE_OVER_SQRT2_Q15_128);y128p[4] = _mm_slli_epi16(y128p[4],1);
      y128p[5]  = _mm_mulhi_epi16(y128p[5],ONE_OVER_SQRT2_Q15_128);y128p[5] = _mm_slli_epi16(y128p[5],1);
      y128p[6]  = _mm_mulhi_epi16(y128p[6],ONE_OVER_SQRT2_Q15_128);y128p[6] = _mm_slli_epi16(y128p[6],1);
      y128p[7]  = _mm_mulhi_epi16(y128p[7],ONE_OVER_SQRT2_Q15_128);y128p[7] = _mm_slli_epi16(y128p[7],1);
      y128p[8]  = _mm_mulhi_epi16(y128p[8],ONE_OVER_SQRT2_Q15_128);y128p[8] = _mm_slli_epi16(y128p[8],1);
      y128p[9]  = _mm_mulhi_epi16(y128p[9],ONE_OVER_SQRT2_Q15_128);y128p[9] = _mm_slli_epi16(y128p[9],1);
      y128p[10] = _mm_mulhi_epi16(y128p[10],ONE_OVER_SQRT2_Q15_128);y128p[10] = _mm_slli_epi16(y128p[10],1);
      y128p[11] = _mm_mulhi_epi16(y128p[11],ONE_OVER_SQRT2_Q15_128);y128p[11] = _mm_slli_epi16(y128p[11],1);
      y128p[12] = _mm_mulhi_epi16(y128p[12],ONE_OVER_SQRT2_Q15_128);y128p[12] = _mm_slli_epi16(y128p[12],1);
      y128p[13] = _mm_mulhi_epi16(y128p[13],ONE_OVER_SQRT2_Q15_128);y128p[13] = _mm_slli_epi16(y128p[13],1);
      y128p[14] = _mm_mulhi_epi16(y128p[14],ONE_OVER_SQRT2_Q15_128);y128p[14] = _mm_slli_epi16(y128p[14],1);
      y128p[15] = _mm_mulhi_epi16(y128p[15],ONE_OVER_SQRT2_Q15_128);y128p[15] = _mm_slli_epi16(y128p[15],1);
      y128p+=16;
    }
  }
  _mm_empty();
  _m_empty();

}

#include "twiddle1536.h"
// 512 x 3
void ifft1536(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][1024 ]__attribute__((aligned(16)));
  uint32_t tmpo[3][1024] __attribute__((aligned(16)));
  
  for (i=0,j=0;i<1024;i+=2) {
    ((int16_t*)tmp[0])[i]   = input[j++];
    ((int16_t*)tmp[0])[i+1]   = -input[j++];
    ((int16_t*)tmp[1])[i]   = input[j++];
    ((int16_t*)tmp[1])[i+1]   = -input[j++];
    ((int16_t*)tmp[2])[i]   = input[j++];
    ((int16_t*)tmp[2])[i+1]   = -input[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft512,rev512,9,4,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft512,rev512,9,4,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft512,rev512,9,4,0);
  for (i=1;i<512;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }

  //  write_output("in.m","in",input,6144,1,1);
  //  write_output("out0.m","o0",tmpo[0],2048,1,1);
  //  write_output("out1.m","o1",tmpo[1],2048,1,1);
  //  write_output("out2.m","o2",tmpo[2],2048,1,1);
  
  for (i=0,i2=0;i<1024;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),((__m128i*)&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+1024+i),(__m128i*)(output+2048+i),
	  (__m128i*)(twa1536+i),(__m128i*)(twb1536+i));
  }
  for (i=1;i<3072;i+=2)
    output[i] = -output[i];

  _mm_empty();
  _m_empty();

}

void fft1536(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][1024] __attribute__((aligned(16)));
  uint32_t tmpo[3][1024] __attribute__((aligned(16)));

  for (i=0,j=0;i<512;i++) {
    tmp[0][i] = ((uint32_t *)input)[j++];
    tmp[1][i] = ((uint32_t *)input)[j++];
    tmp[2][i] = ((uint32_t *)input)[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft512,rev512,9,4,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft512,rev512,9,4,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft512,rev512,9,4,0);

  for (i=1;i<512;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }
  //  write_output("out0.m","o0",tmpo[0],2048,1,1);
  //  write_output("out1.m","o1",tmpo[1],2048,1,1);
  //  write_output("out2.m","o2",tmpo[2],2048,1,1);
  for (i=0,i2=0;i<1024;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),(__m128i*)(&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+1024+i),(__m128i*)(output+2048+i),
	  (__m128i*)(twa1536+i),(__m128i*)(twb1536+i));
  }
  _mm_empty();
  _m_empty();

}

// 1024 x 3
void fft3072(int16_t *input, int16_t *output) {

}

void ifft3072(int16_t *input, int16_t *output) {

}

#include "twiddle6144.h"

void ifft6144(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][4096] __attribute__((aligned(16)));
  uint32_t tmpo[3][4096] __attribute__((aligned(16)));

  for (i=0,j=0;i<4096;i+=2) {
    ((int16_t*)tmp[0])[i]   = input[j++];
    ((int16_t*)tmp[0])[i+1]   = -input[j++];
    ((int16_t*)tmp[1])[i]   = input[j++];
    ((int16_t*)tmp[1])[i+1]   = -input[j++];
    ((int16_t*)tmp[2])[i]   = input[j++];
    ((int16_t*)tmp[2])[i+1]   = -input[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft2048,rev2048,11,5,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft2048,rev2048,11,5,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft2048,rev2048,11,5,0);
  for (i=1;i<2048;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }

  //  write_output("in.m","in",input,6144,1,1);
  //  write_output("out0.m","o0",tmpo[0],2048,1,1);
  //  write_output("out1.m","o1",tmpo[1],2048,1,1);
  //  write_output("out2.m","o2",tmpo[2],2048,1,1);
  
  for (i=0,i2=0;i<4096;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),((__m128i*)&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+4096+i),(__m128i*)(output+8192+i),
	  (__m128i*)(twa6144+i),(__m128i*)(twb6144+i));
  }
  for (i=1;i<12288;i+=2)
    output[i] = -output[i];

  //  write_output("out.m","out",output,6144,1,1);    
  _mm_empty();
  _m_empty();

}


void fft6144(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][4096] __attribute__((aligned(16)));
  uint32_t tmpo[3][4096] __attribute__((aligned(16)));

  for (i=0,j=0;i<2048;i++) {
    tmp[0][i] = ((uint32_t *)input)[j++];
    tmp[1][i] = ((uint32_t *)input)[j++];
    tmp[2][i] = ((uint32_t *)input)[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft2048,rev2048,11,5,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft2048,rev2048,11,5,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft2048,rev2048,11,5,0);

  for (i=1;i<2048;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }
  //  write_output("out0.m","o0",tmpo[0],2048,1,1);
  //  write_output("out1.m","o1",tmpo[1],2048,1,1);
  //  write_output("out2.m","o2",tmpo[2],2048,1,1);
  for (i=0,i2=0;i<4096;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),(__m128i*)(&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+4096+i),(__m128i*)(output+8192+i),
	  (__m128i*)(twa6144+i),(__m128i*)(twb6144+i));
  }
  _mm_empty();
  _m_empty();

}

#include "twiddle12288.h"

// 4096 x 3
void fft12288(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][8192] __attribute__((aligned(16)));
  uint32_t tmpo[3][8192] __attribute__((aligned(16)));

  for (i=0,j=0;i<4096;i++) {
    tmp[0][i] = ((uint32_t *)input)[j++];
    tmp[1][i] = ((uint32_t *)input)[j++];
    tmp[2][i] = ((uint32_t *)input)[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft4096,rev4096,12,6,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft4096,rev4096,12,6,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft4096,rev4096,12,6,0);

  for (i=1;i<4096;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }
  //  write_output("out0.m","o0",tmpo[0],4096,1,1);
  //  write_output("out1.m","o1",tmpo[1],4096,1,1);
  //  write_output("out2.m","o2",tmpo[2],4096,1,1);
  for (i=0,i2=0;i<8192;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),(__m128i*)(&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+8192+i),(__m128i*)(output+16384+i),
	  (__m128i*)(twa12288+i),(__m128i*)(twb12288+i));
  }
  _mm_empty();
  _m_empty();

}

void ifft12288(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][8192] __attribute__((aligned(16)));
  uint32_t tmpo[3][8192] __attribute__((aligned(16)));

  for (i=0,j=0;i<8192;i+=2) {
    ((int16_t*)tmp[0])[i]   = input[j++];
    ((int16_t*)tmp[0])[i+1]   = -input[j++];
    ((int16_t*)tmp[1])[i]   = input[j++];
    ((int16_t*)tmp[1])[i+1]   = -input[j++];
    ((int16_t*)tmp[2])[i]   = input[j++];
    ((int16_t*)tmp[2])[i+1]   = -input[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft4096,rev4096,12,6,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft4096,rev4096,12,6,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft4096,rev4096,12,6,0);
  for (i=1;i<4096;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }

  //  write_output("in.m","in",input,6144,1,1);
  //  write_output("out0.m","o0",tmpo[0],4096,1,1);
  //  write_output("out1.m","o1",tmpo[1],4096,1,1);
  //  write_output("out2.m","o2",tmpo[2],4096,1,1);
  
  for (i=0,i2=0;i<8192;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),((__m128i*)&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+8192+i),(__m128i*)(output+16384+i),
	  (__m128i*)(twa12288+i),(__m128i*)(twb12288+i));
  }
  for (i=1;i<24576;i+=2)
    output[i] = -output[i];
  _mm_empty();
  _m_empty();

  //  write_output("out.m","out",output,6144,1,1); 
}

// 6144 x 3
void fft18432(int16_t *input, int16_t *output) {

}

void ifft18432(int16_t *input, int16_t *output) {

}

#include "twiddle24576.h"
// 8192 x 3
void fft24576(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][16384] __attribute__((aligned(16)));
  uint32_t tmpo[3][16384] __attribute__((aligned(16)));

  for (i=0,j=0;i<8192;i++) {
    tmp[0][i] = ((uint32_t *)input)[j++];
    tmp[1][i] = ((uint32_t *)input)[j++];
    tmp[2][i] = ((uint32_t *)input)[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft8192,rev8192,13,6,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft8192,rev8192,13,6,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft8192,rev8192,13,6,0);

  for (i=1;i<8192;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }
  //   write_output("out0.m","o0",tmpo[0],8192,1,1);
  //    write_output("out1.m","o1",tmpo[1],8192,1,1);
  //    write_output("out2.m","o2",tmpo[2],8192,1,1);
  for (i=0,i2=0;i<16384;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),(__m128i*)(&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+16384+i),(__m128i*)(output+32768+i),
	  (__m128i*)(twa24576+i),(__m128i*)(twb24576+i));
  }
  _mm_empty();
  _m_empty();

  //  write_output("out.m","out",output,24576,1,1); 
}

void ifft24576(int16_t *input, int16_t *output) {
  int i,i2,j;
  uint32_t tmp[3][16384] __attribute__((aligned(16)));
  uint32_t tmpo[3][16384] __attribute__((aligned(16)));

  for (i=0,j=0;i<16384;i+=2) {
    ((int16_t*)tmp[0])[i]   = input[j++];
    ((int16_t*)tmp[0])[i+1]   = -input[j++];
    ((int16_t*)tmp[1])[i]   = input[j++];
    ((int16_t*)tmp[1])[i+1]   = -input[j++];
    ((int16_t*)tmp[2])[i]   = input[j++];
    ((int16_t*)tmp[2])[i+1]   = -input[j++];
  }
  fft((int16_t*)(tmp[0]),(int16_t*)(tmpo[0]),twiddle_fft8192,rev8192,13,6,0);
  fft((int16_t*)(tmp[1]),(int16_t*)(tmpo[1]),twiddle_fft8192,rev8192,13,6,0);
  fft((int16_t*)(tmp[2]),(int16_t*)(tmpo[2]),twiddle_fft8192,rev8192,13,6,0);
  for (i=1;i<8192;i++) {
    tmpo[0][i] = tmpo[0][i<<1];
    tmpo[1][i] = tmpo[1][i<<1];
    tmpo[2][i] = tmpo[2][i<<1];
  }
  /*  
    write_output("in.m","in",input,24576,1,1);
    write_output("out0.m","o0",tmpo[0],8192,1,1);
    write_output("out1.m","o1",tmpo[1],8192,1,1);
    write_output("out2.m","o2",tmpo[2],8192,1,1);
  */

  for (i=0,i2=0;i<16384;i+=8,i2+=4)  {
    bfly3((__m128i*)(&tmpo[0][i2]),(__m128i*)(&tmpo[1][i2]),((__m128i*)&tmpo[2][i2]),
	  (__m128i*)(output+i),(__m128i*)(output+16384+i),(__m128i*)(output+32768+i),
	  (__m128i*)(twa24576+i),(__m128i*)(twb24576+i));
  }
  for (i=1;i<(24576*2);i+=2)
    output[i] = -output[i];

  _mm_empty();
  _m_empty();

  //  write_output("out.m","out",output,24576,1,1); 
}

///  THIS SECTION IS FOR ALL PUSCH DFTS (i.e. radix 2^a * 3^b * 4^c * 5^d)
///  They use twiddles for 4-way parallel DFTS (i.e. 4 DFTS with interleaved input/output)

static int16_t W1_12s[8]__attribute__((aligned(16))) = {28377,-16383,28377,-16383,28377,-16383,28377,-16383};
static int16_t W2_12s[8]__attribute__((aligned(16))) = {16383,-28377,16383,-28377,16383,-28377,16383,-28377};
static int16_t W3_12s[8]__attribute__((aligned(16))) = {0,-32767,0,-32767,0,-32767,0,-32767};
static int16_t W4_12s[8]__attribute__((aligned(16))) = {-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377};
static int16_t W6_12s[8]__attribute__((aligned(16))) = {-32767,0,-32767,0,-32767,0,-32767,0};

__m128i *W1_12=(__m128i *)W1_12s;
__m128i *W2_12=(__m128i *)W2_12s;
__m128i *W3_12=(__m128i *)W3_12s;
__m128i *W4_12=(__m128i *)W4_12s;
__m128i *W6_12=(__m128i *)W6_12s;


static __m128i norm128;

static inline void dft12f(__m128i *x0,
			  __m128i *x1,
			  __m128i *x2,
			  __m128i *x3,
			  __m128i *x4,
			  __m128i *x5,
			  __m128i *x6,
			  __m128i *x7,
			  __m128i *x8,
			  __m128i *x9,
			  __m128i *x10,
			  __m128i *x11,
			  __m128i *y0,
			  __m128i *y1,
			  __m128i *y2,
			  __m128i *y3,
			  __m128i *y4,
			  __m128i *y5,
			  __m128i *y6,
			  __m128i *y7,
			  __m128i *y8,
			  __m128i *y9,
			  __m128i *y10,
			  __m128i *y11) __attribute__((always_inline));

static inline void dft12f(__m128i *x0,
			  __m128i *x1,
			  __m128i *x2,
			  __m128i *x3,
			  __m128i *x4,
			  __m128i *x5,
			  __m128i *x6,
			  __m128i *x7,
			  __m128i *x8,
			  __m128i *x9,
			  __m128i *x10,
			  __m128i *x11,
			  __m128i *y0,
			  __m128i *y1,
			  __m128i *y2,
			  __m128i *y3,
			  __m128i *y4,
			  __m128i *y5,
			  __m128i *y6,
			  __m128i *y7,
			  __m128i *y8,
			  __m128i *y9,
			  __m128i *y10,
			  __m128i *y11) {
   
  
  __m128i tmp_dft12[12];
  
  __m128i *tmp_dft12_ptr = &tmp_dft12[0];
  
  // msg("dft12\n");
  
  bfly4_tw1(x0, 
	    x3, 
	    x6, 
	    x9,
	    tmp_dft12_ptr,
	    tmp_dft12_ptr+3,
	    tmp_dft12_ptr+6,
	    tmp_dft12_ptr+9);
  
  
  bfly4_tw1(x1, 
	    x4, 
	    x7, 
	    x10,
	    tmp_dft12_ptr+1,
	    tmp_dft12_ptr+4,
	    tmp_dft12_ptr+7,
	    tmp_dft12_ptr+10);
  
  
  bfly4_tw1(x2, 
	    x5, 
	    x8, 
	    x11,
	    tmp_dft12_ptr+2,
	    tmp_dft12_ptr+5,
	    tmp_dft12_ptr+8,
	    tmp_dft12_ptr+11);
  
  //  k2=0;
  bfly3_tw1(tmp_dft12_ptr,
	    tmp_dft12_ptr+1,
	    tmp_dft12_ptr+2,
	    y0,
	    y4,
	    y8);
  
  
  
  //  k2=1;
  bfly3(tmp_dft12_ptr+3,
	tmp_dft12_ptr+4,
	tmp_dft12_ptr+5,
	y1,
	y5,
	y9,
 	W1_12,
        W2_12);
  
  
  
  //  k2=2;
  bfly3(tmp_dft12_ptr+6,
	tmp_dft12_ptr+7,
	tmp_dft12_ptr+8,
	y2,
	y6,
	y10,
	W2_12,
	W4_12);
  
  //  k2=3;
  bfly3(tmp_dft12_ptr+9,
	tmp_dft12_ptr+10,
	tmp_dft12_ptr+11,
	y3,
	y7,
	y11,	
	W3_12,
        W6_12);
  /*
  norm128 = _mm_set1_epi16(dft_norm_table[0]);

  *y0 = _mm_slli_epi16(_mm_mulhi_epi16(*y0,norm128),1);
  *y1 = _mm_slli_epi16(_mm_mulhi_epi16(*y1,norm128),1);
  *y2 = _mm_slli_epi16(_mm_mulhi_epi16(*y2,norm128),1);
  *y3 = _mm_slli_epi16(_mm_mulhi_epi16(*y3,norm128),1);
  *y4 = _mm_slli_epi16(_mm_mulhi_epi16(*y4,norm128),1);
  *y5 = _mm_slli_epi16(_mm_mulhi_epi16(*y5,norm128),1);
  *y6 = _mm_slli_epi16(_mm_mulhi_epi16(*y6,norm128),1);
  *y7 = _mm_slli_epi16(_mm_mulhi_epi16(*y7,norm128),1);
  *y8 = _mm_slli_epi16(_mm_mulhi_epi16(*y8,norm128),1);
  *y9 = _mm_slli_epi16(_mm_mulhi_epi16(*y9,norm128),1);
  *y10 = _mm_slli_epi16(_mm_mulhi_epi16(*y10,norm128),1);
  *y11 = _mm_slli_epi16(_mm_mulhi_epi16(*y11,norm128),1);
  */

}	    




void dft12(int16_t *x,int16_t *y) {

  __m128i *x128 = (__m128i *)x,*y128 = (__m128i *)y;
  dft12f(&x128[0],
	 &x128[1],
	 &x128[2],
	 &x128[3],
	 &x128[4],
	 &x128[5],
	 &x128[6],
	 &x128[7],
	 &x128[8],
	 &x128[9],
	 &x128[10],
	 &x128[11],
	 &y128[0],
	 &y128[1],
	 &y128[2],
	 &y128[3],
	 &y128[4],
	 &y128[5],
	 &y128[6],
	 &y128[7],
	 &y128[8],
	 &y128[9],
	 &y128[10],
	 &y128[11]);

  _mm_empty();
  _m_empty();

}

static int16_t tw24[88]__attribute__((aligned(16))) = {31650,-8480,31650,-8480,31650,-8480,31650,-8480,
						      28377,-16383,28377,-16383,28377,-16383,28377,-16383,
						      23169,-23169,23169,-23169,23169,-23169,23169,-23169,
						      16383,-28377,16383,-28377,16383,-28377,16383,-28377,
						      8480,-31650,8480,-31650,8480,-31650,8480,-31650,
						      0,-32767,0,-32767,0,-32767,0,-32767,
						      -8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
						      -16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
						      -23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
						      -28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
						      -31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480};

//static __m128i ytmp128array[300];
//static __m128i ytmp128array2[300];
//static __m128i ytmp128array3[300];
//static __m128i x2128array[300];

void dft24(int16_t *x,int16_t *y,unsigned char scale_flag) {
  
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *tw128=(__m128i *)&tw24[0];
  __m128i ytmp128[24];//=&ytmp128array[0];
  int i,j,k;

  //  msg("dft24\n");
  dft12f(x128,
	x128+2,
	x128+4,
	x128+6,
	x128+8,
	x128+10,
	x128+12,
	x128+14,
	x128+16,
	x128+18,
	x128+20,
	x128+22,
	ytmp128,
	ytmp128+2,
	ytmp128+4,
	ytmp128+6,
	ytmp128+8,
	ytmp128+10,
	ytmp128+12,
	ytmp128+14,
	ytmp128+16,
	ytmp128+18,
	ytmp128+20,
	ytmp128+22);
  //  msg("dft24b\n");

  dft12f(x128+1,
	x128+3,
	x128+5,
	x128+7,
	x128+9,
	x128+11,
	x128+13,
	x128+15,
	x128+17,
	x128+19,
	x128+21,
	x128+23,
	ytmp128+1,
	ytmp128+3,
	ytmp128+5,
	ytmp128+7,
	ytmp128+9,
	ytmp128+11,
	ytmp128+13,
	ytmp128+15,
	ytmp128+17,
	ytmp128+19,
	ytmp128+21,
	ytmp128+23);

  //  msg("dft24c\n");

  bfly2_tw1(ytmp128, 
	    ytmp128+1,
	    y128, 
	    y128+12);

  //  msg("dft24d\n");
  
  for (i=2,j=1,k=0;i<24;i+=2,j++,k++) {
    
    bfly2(ytmp128+i, 
	  ytmp128+i+1,
	  y128+j, 
	  y128+j+12,
	  tw128+k);
    //    msg("dft24e\n");
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[1]);
    
    for (i=0;i<24;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa36[88]__attribute__((aligned(16))) = {32269,-5689,32269,-5689,32269,-5689,32269,-5689,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
0,-32767,0,-32767,0,-32767,0,-32767,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790};

static int16_t twb36[88]__attribute__((aligned(16))) = {30790,-11206,30790,-11206,30790,-11206,30790,-11206,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-32767,0,-32767,0,-32767,0,-32767,0,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-25100,21062,-25100,21062,-25100,21062,-25100,21062};

void dft36(int16_t *x,int16_t *y,unsigned char scale_flag) {
  
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa36[0];
  __m128i *twb128=(__m128i *)&twb36[0];
  __m128i ytmp128[36];//&ytmp128array[0];


  int i,j,k;

  dft12f(x128,
	x128+3,
	x128+6,
	x128+9,
	x128+12,
	x128+15,
	x128+18,
	x128+21,
	x128+24,
	x128+27,
	x128+30,
	x128+33,
	ytmp128,
	ytmp128+3,
	ytmp128+6,
	ytmp128+9,
	ytmp128+12,
	ytmp128+15,
	ytmp128+18,
	ytmp128+21,
	ytmp128+24,
	ytmp128+27,
	ytmp128+30,
	ytmp128+33);

  dft12f(x128+1,
	x128+4,
	x128+7,
	x128+10,
	x128+13,
	x128+16,
	x128+19,
	x128+22,
	x128+25,
	x128+28,
	x128+31,
	x128+34,
	ytmp128+1,
	ytmp128+4,
	ytmp128+7,
	ytmp128+10,
	ytmp128+13,
	ytmp128+16,
	ytmp128+19,
	ytmp128+22,
	ytmp128+25,
	ytmp128+28,
	ytmp128+31,
	ytmp128+34);

  dft12f(x128+2,
	x128+5,
	x128+8,
	x128+11,
	x128+14,
	x128+17,
	x128+20,
	x128+23,
	x128+26,
	x128+29,
	x128+32,
	x128+35,
	ytmp128+2,
	ytmp128+5,
	ytmp128+8,
	ytmp128+11,
	ytmp128+14,
	ytmp128+17,
	ytmp128+20,
	ytmp128+23,
	ytmp128+26,
	ytmp128+29,
	ytmp128+32,
	ytmp128+35);


  bfly3_tw1(ytmp128, 
	    ytmp128+1,
	    ytmp128+2,
	    y128, 
	    y128+12,
	    y128+24);
  
  for (i=3,j=1,k=0;i<36;i+=3,j++,k++) {
    
    bfly3(ytmp128+i, 
	  ytmp128+i+1,
	  ytmp128+i+2,
	  y128+j, 
	  y128+j+12,
	  y128+j+24,
	  twa128+k,
	  twb128+k);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[2]);
    
    for (i=0;i<36;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa48[88]__attribute__((aligned(16))) = {32486,-4276,32486,-4276,32486,-4276,32486,-4276,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
4276,-32486,4276,-32486,4276,-32486,4276,-32486};

static int16_t twb48[88]__attribute__((aligned(16))) = {31650,-8480,31650,-8480,31650,-8480,31650,-8480,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
0,-32767,0,-32767,0,-32767,0,-32767,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480};

static int16_t twc48[88]__attribute__((aligned(16))) = {30272,-12539,30272,-12539,30272,-12539,30272,-12539,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
0,-32767,0,-32767,0,-32767,0,-32767,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-32767,0,-32767,0,-32767,0,-32767,0,
-30272,12539,-30272,12539,-30272,12539,-30272,12539,
-23169,23169,-23169,23169,-23169,23169,-23169,23169,
-12539,30272,-12539,30272,-12539,30272,-12539,30272};

void dft48(int16_t *x, int16_t *y,unsigned char scale_flag) {
  
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa48[0];
  __m128i *twb128=(__m128i *)&twb48[0];
  __m128i *twc128=(__m128i *)&twc48[0];
  __m128i ytmp128[48];//=&ytmp128array[0];
  int i,j,k;


  dft12f(x128,
	x128+4,
	x128+8,
	x128+12,
	x128+16,
	x128+20,
	x128+24,
	x128+28,
	x128+32,
	x128+36,
	x128+40,
	x128+44,
	ytmp128,
	ytmp128+4,
	ytmp128+8,
	ytmp128+12,
	ytmp128+16,
	ytmp128+20,
	ytmp128+24,
	ytmp128+28,
	ytmp128+32,
	ytmp128+36,
	ytmp128+40,
	ytmp128+44);


  dft12f(x128+1,
	x128+5,
	x128+9,
	x128+13,
	x128+17,
	x128+21,
	x128+25,
	x128+29,
	x128+33,
	x128+37,
	x128+41,
	x128+45,
	ytmp128+1,
	ytmp128+5,
	ytmp128+9,
	ytmp128+13,
	ytmp128+17,
	ytmp128+21,
	ytmp128+25,
	ytmp128+29,
	ytmp128+33,
	ytmp128+37,
	ytmp128+41,
	ytmp128+45);


  dft12f(x128+2,
	x128+6,
	x128+10,
	x128+14,
	x128+18,
	x128+22,
	x128+26,
	x128+30,
	x128+34,
	x128+38,
	x128+42,
	x128+46,
	ytmp128+2,
	ytmp128+6,
	ytmp128+10,
	ytmp128+14,
	ytmp128+18,
	ytmp128+22,
	ytmp128+26,
	ytmp128+30,
	ytmp128+34,
	ytmp128+38,
	ytmp128+42,
	ytmp128+46);


  dft12f(x128+3,
	x128+7,
	x128+11,
	x128+15,
	x128+19,
	x128+23,
	x128+27,
	x128+31,
	x128+35,
	x128+39,
	x128+43,
	x128+47,
	ytmp128+3,
	ytmp128+7,
	ytmp128+11,
	ytmp128+15,
	ytmp128+19,
	ytmp128+23,
	ytmp128+27,
	ytmp128+31,
	ytmp128+35,
	ytmp128+39,
	ytmp128+43,
	ytmp128+47);



  bfly4_tw1(ytmp128, 
	    ytmp128+1,
	    ytmp128+2,
	    ytmp128+3,
	    y128, 
	    y128+12,
	    y128+24,
	    y128+36);


  
  for (i=4,j=1,k=0;i<48;i+=4,j++,k++) {
    
    bfly4(ytmp128+i, 
	  ytmp128+i+1,
	  ytmp128+i+2,
	  ytmp128+i+3,
	  y128+j, 
	  y128+j+12,
	  y128+j+24,
	  y128+j+36,
	  twa128+k,
	  twb128+k,
	  twc128+k);

  }

  if (scale_flag == 1) {
    norm128 = _mm_set1_epi16(dft_norm_table[3]);
    
    for (i=0;i<48;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa60[88]__attribute__((aligned(16))) = {32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
13327,-29934,13327,-29934,13327,-29934,13327,-29934};
static int16_t twb60[88]__attribute__((aligned(16))) = {32050,-6812,32050,-6812,32050,-6812,32050,-6812,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-21925,-24350,-21925,-24350,-21925,-24350,-21925,-24350};
static int16_t twc60[88]__attribute__((aligned(16))) = {31163,-10125,31163,-10125,31163,-10125,31163,-10125,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
0,-32767,0,-32767,0,-32767,0,-32767,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-19259,-26509,-19259,-26509,-19259,-26509,-19259,-26509,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-31163,-10125,-31163,-10125,-31163,-10125,-31163,-10125,
-32767,0,-32767,0,-32767,0,-32767,0,
-31163,10125,-31163,10125,-31163,10125,-31163,10125};
static int16_t twd60[88]__attribute__((aligned(16))) = {29934,-13327,29934,-13327,29934,-13327,29934,-13327,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-32050,-6812,-32050,-6812,-32050,-6812,-32050,-6812,
-32050,6812,-32050,6812,-32050,6812,-32050,6812,
-26509,19259,-26509,19259,-26509,19259,-26509,19259,
-16383,28377,-16383,28377,-16383,28377,-16383,28377,
-3425,32587,-3425,32587,-3425,32587,-3425,32587};

void dft60(int16_t *x,int16_t *y,unsigned char scale) {
  
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa60[0];
  __m128i *twb128=(__m128i *)&twb60[0];
  __m128i *twc128=(__m128i *)&twc60[0];
  __m128i *twd128=(__m128i *)&twd60[0];
  __m128i ytmp128[60];//=&ytmp128array[0];
  int i,j,k;

  dft12f(x128,
	x128+5,
	x128+10,
	x128+15,
	x128+20,
	x128+25,
	x128+30,
	x128+35,
	x128+40,
	x128+45,
	x128+50,
	x128+55,
	ytmp128,
	ytmp128+5,
	ytmp128+10,
	ytmp128+15,
	ytmp128+20,
	ytmp128+25,
	ytmp128+30,
	ytmp128+35,
	ytmp128+40,
	ytmp128+45,
	ytmp128+50,
	ytmp128+55);

  dft12f(x128+1,
	x128+6,
	x128+11,
	x128+16,
	x128+21,
	x128+26,
	x128+31,
	x128+36,
	x128+41,
	x128+46,
	x128+51,
	x128+56,
	ytmp128+1,
	ytmp128+6,
	ytmp128+11,
	ytmp128+16,
	ytmp128+21,
	ytmp128+26,
	ytmp128+31,
	ytmp128+36,
	ytmp128+41,
	ytmp128+46,
	ytmp128+51,
	ytmp128+56);

  dft12f(x128+2,
	x128+7,
	x128+12,
	x128+17,
	x128+22,
	x128+27,
	x128+32,
	x128+37,
	x128+42,
	x128+47,
	x128+52,
	x128+57,
	ytmp128+2,
	ytmp128+7,
	ytmp128+12,
	ytmp128+17,
	ytmp128+22,
	ytmp128+27,
	ytmp128+32,
	ytmp128+37,
	ytmp128+42,
	ytmp128+47,
	ytmp128+52,
	ytmp128+57);

  dft12f(x128+3,
	x128+8,
	x128+13,
	x128+18,
	x128+23,
	x128+28,
	x128+33,
	x128+38,
	x128+43,
	x128+48,
	x128+53,
	x128+58,
	ytmp128+3,
	ytmp128+8,
	ytmp128+13,
	ytmp128+18,
	ytmp128+23,
	ytmp128+28,
	ytmp128+33,
	ytmp128+38,
	ytmp128+43,
	ytmp128+48,
	ytmp128+53,
	ytmp128+58);

  dft12f(x128+4,
	x128+9,
	x128+14,
	x128+19,
	x128+24,
	x128+29,
	x128+34,
	x128+39,
	x128+44,
	x128+49,
	x128+54,
	x128+59,
	ytmp128+4,
	ytmp128+9,
	ytmp128+14,
	ytmp128+19,
	ytmp128+24,
	ytmp128+29,
	ytmp128+34,
	ytmp128+39,
	ytmp128+44,
	ytmp128+49,
	ytmp128+54,
	ytmp128+59);

  bfly5_tw1(ytmp128, 
	    ytmp128+1,
	    ytmp128+2,
	    ytmp128+3,
	    ytmp128+4,
	    y128, 
	    y128+12,
	    y128+24,
	    y128+36,
	    y128+48);
  
  for (i=5,j=1,k=0;i<60;i+=5,j++,k++) {
    
    bfly5(ytmp128+i, 
	  ytmp128+i+1,
	  ytmp128+i+2,
	  ytmp128+i+3,
	  ytmp128+i+4,
	  y128+j, 
	  y128+j+12,
	  y128+j+24,
	  y128+j+36,
	  y128+j+48,
	  twa128+k,
	  twb128+k,
	  twc128+k,
	  twd128+k);
  }

  if (scale == 1) {
    norm128 = _mm_set1_epi16(dft_norm_table[4]);
    
    for (i=0;i<60;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t tw72[280]__attribute__((aligned(16))) = {32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
0,-32767,0,-32767,0,-32767,0,-32767,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18794,-26841,-18794,-26841,-18794,-26841,-18794,-26841,
-21062,-25100,-21062,-25100,-21062,-25100,-21062,-25100,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-26841,-18794,-26841,-18794,-26841,-18794,-26841,-18794,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29696,-13847,-29696,-13847,-29696,-13847,-29696,-13847,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32269,-5689,-32269,-5689,-32269,-5689,-32269,-5689,
-32642,-2855,-32642,-2855,-32642,-2855,-32642,-2855,
};

void dft72(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *tw128=(__m128i *)&tw72[0];
  __m128i x2128[72];// = (__m128i *)&x2128array[0];

  __m128i ytmp128[72];//=&ytmp128array2[0];

  for (i=0,j=0;i<36;i++,j+=2) {
    x2128[i]    = x128[j];    // even inputs
    x2128[i+36] = x128[j+1];  // odd inputs
  }

  dft36((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft36((int16_t *)(x2128+36),(int16_t *)(ytmp128+36),1);

  bfly2_tw1(ytmp128,ytmp128+36,y128,y128+36);
  for (i=1,j=0;i<36;i++,j++) {
    bfly2(ytmp128+i,
	  ytmp128+36+i,
	  y128+i,
	  y128+36+i,
	  tw128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[5]);
    
    for (i=0;i<72;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t tw96[376]__attribute__((aligned(16))) = {32696,-2143,32696,-2143,32696,-2143,32696,-2143,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32137,-6392,32137,-6392,32137,-6392,32137,-6392,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31028,-10532,31028,-10532,31028,-10532,31028,-10532,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29387,-14492,29387,-14492,29387,-14492,29387,-14492,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27244,-18204,27244,-18204,27244,-18204,27244,-18204,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
24635,-21604,24635,-21604,24635,-21604,24635,-21604,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21604,-24635,21604,-24635,21604,-24635,21604,-24635,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
18204,-27244,18204,-27244,18204,-27244,18204,-27244,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14492,-29387,14492,-29387,14492,-29387,14492,-29387,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
10532,-31028,10532,-31028,10532,-31028,10532,-31028,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
6392,-32137,6392,-32137,6392,-32137,6392,-32137,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
2143,-32696,2143,-32696,2143,-32696,2143,-32696,
0,-32767,0,-32767,0,-32767,0,-32767,
-2143,-32696,-2143,-32696,-2143,-32696,-2143,-32696,
-4276,-32486,-4276,-32486,-4276,-32486,-4276,-32486,
-6392,-32137,-6392,-32137,-6392,-32137,-6392,-32137,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-10532,-31028,-10532,-31028,-10532,-31028,-10532,-31028,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-14492,-29387,-14492,-29387,-14492,-29387,-14492,-29387,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18204,-27244,-18204,-27244,-18204,-27244,-18204,-27244,
-19947,-25995,-19947,-25995,-19947,-25995,-19947,-25995,
-21604,-24635,-21604,-24635,-21604,-24635,-21604,-24635,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24635,-21604,-24635,-21604,-24635,-21604,-24635,-21604,
-25995,-19947,-25995,-19947,-25995,-19947,-25995,-19947,
-27244,-18204,-27244,-18204,-27244,-18204,-27244,-18204,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29387,-14492,-29387,-14492,-29387,-14492,-29387,-14492,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-31028,-10532,-31028,-10532,-31028,-10532,-31028,-10532,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32137,-6392,-32137,-6392,-32137,-6392,-32137,-6392,
-32486,-4276,-32486,-4276,-32486,-4276,-32486,-4276,
-32696,-2143,-32696,-2143,-32696,-2143,-32696,-2143};

void dft96(int16_t *x,int16_t *y,unsigned char scale_flag){


  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *tw128=(__m128i *)&tw96[0];
  __m128i x2128[96];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[96];//=&ytmp128array2[0];


  for (i=0,j=0;i<48;i++,j+=2) {
    x2128[i]    = x128[j];
    x2128[i+48] = x128[j+1];
  }

  dft48((int16_t *)x2128,(int16_t *)ytmp128,0);
  dft48((int16_t *)(x2128+48),(int16_t *)(ytmp128+48),0);


  bfly2_tw1(ytmp128,ytmp128+48,y128,y128+48);
  for (i=1,j=0;i<48;i++,j++) {
    bfly2(ytmp128+i,
	  ytmp128+48+i,
	  y128+i,
	  y128+48+i,
	  tw128+j);
  }
  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[6]);
    
    for (i=0;i<96;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa108[280]__attribute__((aligned(16))) = {32711,-1905,32711,-1905,32711,-1905,32711,-1905,
32545,-3804,32545,-3804,32545,-3804,32545,-3804,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31883,-7556,31883,-7556,31883,-7556,31883,-7556,
31390,-9397,31390,-9397,31390,-9397,31390,-9397,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30087,-12978,30087,-12978,30087,-12978,30087,-12978,
29281,-14705,29281,-14705,29281,-14705,29281,-14705,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27376,-18005,27376,-18005,27376,-18005,27376,-18005,
26283,-19567,26283,-19567,26283,-19567,26283,-19567,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
23833,-22486,23833,-22486,23833,-22486,23833,-22486,
22486,-23833,22486,-23833,22486,-23833,22486,-23833,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
19567,-26283,19567,-26283,19567,-26283,19567,-26283,
18005,-27376,18005,-27376,18005,-27376,18005,-27376,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14705,-29281,14705,-29281,14705,-29281,14705,-29281,
12978,-30087,12978,-30087,12978,-30087,12978,-30087,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
9397,-31390,9397,-31390,9397,-31390,9397,-31390,
7556,-31883,7556,-31883,7556,-31883,7556,-31883,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
3804,-32545,3804,-32545,3804,-32545,3804,-32545,
1905,-32711,1905,-32711,1905,-32711,1905,-32711,
0,-32767,0,-32767,0,-32767,0,-32767,
-1905,-32711,-1905,-32711,-1905,-32711,-1905,-32711,
-3804,-32545,-3804,-32545,-3804,-32545,-3804,-32545,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-7556,-31883,-7556,-31883,-7556,-31883,-7556,-31883,
-9397,-31390,-9397,-31390,-9397,-31390,-9397,-31390,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12978,-30087,-12978,-30087,-12978,-30087,-12978,-30087,
-14705,-29281,-14705,-29281,-14705,-29281,-14705,-29281};

static int16_t twb108[280]__attribute__((aligned(16))) = {32545,-3804,32545,-3804,32545,-3804,32545,-3804,
31883,-7556,31883,-7556,31883,-7556,31883,-7556,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
29281,-14705,29281,-14705,29281,-14705,29281,-14705,
27376,-18005,27376,-18005,27376,-18005,27376,-18005,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
22486,-23833,22486,-23833,22486,-23833,22486,-23833,
19567,-26283,19567,-26283,19567,-26283,19567,-26283,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
12978,-30087,12978,-30087,12978,-30087,12978,-30087,
9397,-31390,9397,-31390,9397,-31390,9397,-31390,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
1905,-32711,1905,-32711,1905,-32711,1905,-32711,
-1905,-32711,-1905,-32711,-1905,-32711,-1905,-32711,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-9397,-31390,-9397,-31390,-9397,-31390,-9397,-31390,
-12978,-30087,-12978,-30087,-12978,-30087,-12978,-30087,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-19567,-26283,-19567,-26283,-19567,-26283,-19567,-26283,
-22486,-23833,-22486,-23833,-22486,-23833,-22486,-23833,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-27376,-18005,-27376,-18005,-27376,-18005,-27376,-18005,
-29281,-14705,-29281,-14705,-29281,-14705,-29281,-14705,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31883,-7556,-31883,-7556,-31883,-7556,-31883,-7556,
-32545,-3804,-32545,-3804,-32545,-3804,-32545,-3804,
-32767,0,-32767,0,-32767,0,-32767,0,
-32545,3804,-32545,3804,-32545,3804,-32545,3804,
-31883,7556,-31883,7556,-31883,7556,-31883,7556,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-29281,14705,-29281,14705,-29281,14705,-29281,14705,
-27376,18005,-27376,18005,-27376,18005,-27376,18005,
-25100,21062,-25100,21062,-25100,21062,-25100,21062,
-22486,23833,-22486,23833,-22486,23833,-22486,23833,
-19567,26283,-19567,26283,-19567,26283,-19567,26283};

void dft108(int16_t *x,int16_t *y,unsigned char scale_flag){


  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa108[0];
  __m128i *twb128=(__m128i *)&twb108[0];
  __m128i x2128[108];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[108];//=&ytmp128array2[0];


  for (i=0,j=0;i<36;i++,j+=3) {
    x2128[i]    = x128[j];
    x2128[i+36] = x128[j+1];
    x2128[i+72] = x128[j+2];
  }

  dft36((int16_t *)x2128,(int16_t *)ytmp128,0);
  dft36((int16_t *)(x2128+36),(int16_t *)(ytmp128+36),0);
  dft36((int16_t *)(x2128+72),(int16_t *)(ytmp128+72),0);

  bfly3_tw1(ytmp128,ytmp128+36,ytmp128+72,y128,y128+36,y128+72);
  for (i=1,j=0;i<36;i++,j++) {
    bfly3(ytmp128+i,
	  ytmp128+36+i,
	  ytmp128+72+i,
	  y128+i,
	  y128+36+i,
	  y128+72+i,
	  twa128+j,
	  twb128+j);

  }
  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[7]);
    
    for (i=0;i<108;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t tw120[472]__attribute__((aligned(16))) = {32722,-1714,32722,-1714,32722,-1714,32722,-1714,
32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32363,-5125,32363,-5125,32363,-5125,32363,-5125,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30590,-11742,30590,-11742,30590,-11742,30590,-11742,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29195,-14875,29195,-14875,29195,-14875,29195,-14875,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27480,-17846,27480,-17846,27480,-17846,27480,-17846,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25464,-20620,25464,-20620,25464,-20620,25464,-20620,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
20620,-25464,20620,-25464,20620,-25464,20620,-25464,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
17846,-27480,17846,-27480,17846,-27480,17846,-27480,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14875,-29195,14875,-29195,14875,-29195,14875,-29195,
13327,-29934,13327,-29934,13327,-29934,13327,-29934,
11742,-30590,11742,-30590,11742,-30590,11742,-30590,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
6812,-32050,6812,-32050,6812,-32050,6812,-32050,
5125,-32363,5125,-32363,5125,-32363,5125,-32363,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
1714,-32722,1714,-32722,1714,-32722,1714,-32722,
0,-32767,0,-32767,0,-32767,0,-32767,
-1714,-32722,-1714,-32722,-1714,-32722,-1714,-32722,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-5125,-32363,-5125,-32363,-5125,-32363,-5125,-32363,
-6812,-32050,-6812,-32050,-6812,-32050,-6812,-32050,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-11742,-30590,-11742,-30590,-11742,-30590,-11742,-30590,
-13327,-29934,-13327,-29934,-13327,-29934,-13327,-29934,
-14875,-29195,-14875,-29195,-14875,-29195,-14875,-29195,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-17846,-27480,-17846,-27480,-17846,-27480,-17846,-27480,
-19259,-26509,-19259,-26509,-19259,-26509,-19259,-26509,
-20620,-25464,-20620,-25464,-20620,-25464,-20620,-25464,
-21925,-24350,-21925,-24350,-21925,-24350,-21925,-24350,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24350,-21925,-24350,-21925,-24350,-21925,-24350,-21925,
-25464,-20620,-25464,-20620,-25464,-20620,-25464,-20620,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-27480,-17846,-27480,-17846,-27480,-17846,-27480,-17846,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29195,-14875,-29195,-14875,-29195,-14875,-29195,-14875,
-29934,-13327,-29934,-13327,-29934,-13327,-29934,-13327,
-30590,-11742,-30590,-11742,-30590,-11742,-30590,-11742,
-31163,-10125,-31163,-10125,-31163,-10125,-31163,-10125,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32050,-6812,-32050,-6812,-32050,-6812,-32050,-6812,
-32363,-5125,-32363,-5125,-32363,-5125,-32363,-5125,
-32587,-3425,-32587,-3425,-32587,-3425,-32587,-3425,
-32722,-1714,-32722,-1714,-32722,-1714,-32722,-1714};

void dft120(int16_t *x,int16_t *y, unsigned char scale_flag){


  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *tw128=(__m128i *)&tw120[0];
  __m128i x2128[120];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[120];//=&ytmp128array2[0];


  for (i=0,j=0;i<60;i++,j+=2) {
    x2128[i]    = x128[j];
    x2128[i+60] = x128[j+1];
  }

  dft60((int16_t *)x2128,(int16_t *)ytmp128,0);
  dft60((int16_t *)(x2128+60),(int16_t *)(ytmp128+60),0);


  bfly2_tw1(ytmp128,ytmp128+60,y128,y128+60);
  for (i=1,j=0;i<60;i++,j++) {
    bfly2(ytmp128+i,
	  ytmp128+60+i,
	  y128+i,
	  y128+60+i,
	  tw128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[8]);
    
    for (i=0;i<120;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa144[376]__attribute__((aligned(16))) = {32735,-1429,32735,-1429,32735,-1429,32735,-1429,
32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31990,-7092,31990,-7092,31990,-7092,31990,-7092,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31250,-9853,31250,-9853,31250,-9853,31250,-9853,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
29064,-15130,29064,-15130,29064,-15130,29064,-15130,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27635,-17605,27635,-17605,27635,-17605,27635,-17605,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
24158,-22137,24158,-22137,24158,-22137,24158,-22137,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22137,-24158,22137,-24158,22137,-24158,22137,-24158,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
17605,-27635,17605,-27635,17605,-27635,17605,-27635,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15130,-29064,15130,-29064,15130,-29064,15130,-29064,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
9853,-31250,9853,-31250,9853,-31250,9853,-31250,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7092,-31990,7092,-31990,7092,-31990,7092,-31990,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
1429,-32735,1429,-32735,1429,-32735,1429,-32735,
0,-32767,0,-32767,0,-32767,0,-32767,
-1429,-32735,-1429,-32735,-1429,-32735,-1429,-32735,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-4276,-32486,-4276,-32486,-4276,-32486,-4276,-32486,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-7092,-31990,-7092,-31990,-7092,-31990,-7092,-31990,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-9853,-31250,-9853,-31250,-9853,-31250,-9853,-31250,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-15130,-29064,-15130,-29064,-15130,-29064,-15130,-29064};

static int16_t twb144[376]__attribute__((aligned(16))) = {32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
0,-32767,0,-32767,0,-32767,0,-32767,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18794,-26841,-18794,-26841,-18794,-26841,-18794,-26841,
-21062,-25100,-21062,-25100,-21062,-25100,-21062,-25100,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-26841,-18794,-26841,-18794,-26841,-18794,-26841,-18794,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29696,-13847,-29696,-13847,-29696,-13847,-29696,-13847,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32269,-5689,-32269,-5689,-32269,-5689,-32269,-5689,
-32642,-2855,-32642,-2855,-32642,-2855,-32642,-2855,
-32767,0,-32767,0,-32767,0,-32767,0,
-32642,2855,-32642,2855,-32642,2855,-32642,2855,
-32269,5689,-32269,5689,-32269,5689,-32269,5689,
-31650,8480,-31650,8480,-31650,8480,-31650,8480,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-29696,13847,-29696,13847,-29696,13847,-29696,13847,
-28377,16383,-28377,16383,-28377,16383,-28377,16383,
-26841,18794,-26841,18794,-26841,18794,-26841,18794,
-25100,21062,-25100,21062,-25100,21062,-25100,21062,
-23169,23169,-23169,23169,-23169,23169,-23169,23169,
-21062,25100,-21062,25100,-21062,25100,-21062,25100,
-18794,26841,-18794,26841,-18794,26841,-18794,26841};

void dft144(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa144[0];
  __m128i *twb128=(__m128i *)&twb144[0];
  __m128i x2128[144];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[144];//=&ytmp128array2[0];



  for (i=0,j=0;i<48;i++,j+=3) {
    x2128[i]    = x128[j];
    x2128[i+48] = x128[j+1];
    x2128[i+96] = x128[j+2];
  }

  dft48((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft48((int16_t *)(x2128+48),(int16_t *)(ytmp128+48),1);
  dft48((int16_t *)(x2128+96),(int16_t *)(ytmp128+96),1);

  bfly3_tw1(ytmp128,ytmp128+48,ytmp128+96,y128,y128+48,y128+96);
  for (i=1,j=0;i<48;i++,j++) {
    bfly3(ytmp128+i,
	  ytmp128+48+i,
	  ytmp128+96+i,
	  y128+i,
	  y128+48+i,
	  y128+96+i,
	  twa128+j,
	  twb128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[9]);
    
    for (i=0;i<144;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa180[472]__attribute__((aligned(16))) = {32747,-1143,32747,-1143,32747,-1143,32747,-1143,
32687,-2285,32687,-2285,32687,-2285,32687,-2285,
32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32448,-4560,32448,-4560,32448,-4560,32448,-4560,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31793,-7927,31793,-7927,31793,-7927,31793,-7927,
31497,-9031,31497,-9031,31497,-9031,31497,-9031,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30381,-12274,30381,-12274,30381,-12274,30381,-12274,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29450,-14364,29450,-14364,29450,-14364,29450,-14364,
28931,-15383,28931,-15383,28931,-15383,28931,-15383,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27787,-17363,27787,-17363,27787,-17363,27787,-17363,
27165,-18323,27165,-18323,27165,-18323,27165,-18323,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25820,-20173,25820,-20173,25820,-20173,25820,-20173,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
23570,-22761,23570,-22761,23570,-22761,23570,-22761,
22761,-23570,22761,-23570,22761,-23570,22761,-23570,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
20173,-25820,20173,-25820,20173,-25820,20173,-25820,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
18323,-27165,18323,-27165,18323,-27165,18323,-27165,
17363,-27787,17363,-27787,17363,-27787,17363,-27787,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15383,-28931,15383,-28931,15383,-28931,15383,-28931,
14364,-29450,14364,-29450,14364,-29450,14364,-29450,
13327,-29934,13327,-29934,13327,-29934,13327,-29934,
12274,-30381,12274,-30381,12274,-30381,12274,-30381,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
9031,-31497,9031,-31497,9031,-31497,9031,-31497,
7927,-31793,7927,-31793,7927,-31793,7927,-31793,
6812,-32050,6812,-32050,6812,-32050,6812,-32050,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
4560,-32448,4560,-32448,4560,-32448,4560,-32448,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
2285,-32687,2285,-32687,2285,-32687,2285,-32687,
1143,-32747,1143,-32747,1143,-32747,1143,-32747,
0,-32767,0,-32767,0,-32767,0,-32767,
-1143,-32747,-1143,-32747,-1143,-32747,-1143,-32747,
-2285,-32687,-2285,-32687,-2285,-32687,-2285,-32687,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-4560,-32448,-4560,-32448,-4560,-32448,-4560,-32448,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-6812,-32050,-6812,-32050,-6812,-32050,-6812,-32050,
-7927,-31793,-7927,-31793,-7927,-31793,-7927,-31793,
-9031,-31497,-9031,-31497,-9031,-31497,-9031,-31497,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12274,-30381,-12274,-30381,-12274,-30381,-12274,-30381,
-13327,-29934,-13327,-29934,-13327,-29934,-13327,-29934,
-14364,-29450,-14364,-29450,-14364,-29450,-14364,-29450,
-15383,-28931,-15383,-28931,-15383,-28931,-15383,-28931};

static int16_t twb180[472]__attribute__((aligned(16))) = {32687,-2285,32687,-2285,32687,-2285,32687,-2285,
32448,-4560,32448,-4560,32448,-4560,32448,-4560,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31497,-9031,31497,-9031,31497,-9031,31497,-9031,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
28931,-15383,28931,-15383,28931,-15383,28931,-15383,
27787,-17363,27787,-17363,27787,-17363,27787,-17363,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
23570,-22761,23570,-22761,23570,-22761,23570,-22761,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
20173,-25820,20173,-25820,20173,-25820,20173,-25820,
18323,-27165,18323,-27165,18323,-27165,18323,-27165,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14364,-29450,14364,-29450,14364,-29450,14364,-29450,
12274,-30381,12274,-30381,12274,-30381,12274,-30381,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
7927,-31793,7927,-31793,7927,-31793,7927,-31793,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
1143,-32747,1143,-32747,1143,-32747,1143,-32747,
-1143,-32747,-1143,-32747,-1143,-32747,-1143,-32747,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-7927,-31793,-7927,-31793,-7927,-31793,-7927,-31793,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-12274,-30381,-12274,-30381,-12274,-30381,-12274,-30381,
-14364,-29450,-14364,-29450,-14364,-29450,-14364,-29450,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18323,-27165,-18323,-27165,-18323,-27165,-18323,-27165,
-20173,-25820,-20173,-25820,-20173,-25820,-20173,-25820,
-21925,-24350,-21925,-24350,-21925,-24350,-21925,-24350,
-23570,-22761,-23570,-22761,-23570,-22761,-23570,-22761,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-27787,-17363,-27787,-17363,-27787,-17363,-27787,-17363,
-28931,-15383,-28931,-15383,-28931,-15383,-28931,-15383,
-29934,-13327,-29934,-13327,-29934,-13327,-29934,-13327,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31497,-9031,-31497,-9031,-31497,-9031,-31497,-9031,
-32050,-6812,-32050,-6812,-32050,-6812,-32050,-6812,
-32448,-4560,-32448,-4560,-32448,-4560,-32448,-4560,
-32687,-2285,-32687,-2285,-32687,-2285,-32687,-2285,
-32767,0,-32767,0,-32767,0,-32767,0,
-32687,2285,-32687,2285,-32687,2285,-32687,2285,
-32448,4560,-32448,4560,-32448,4560,-32448,4560,
-32050,6812,-32050,6812,-32050,6812,-32050,6812,
-31497,9031,-31497,9031,-31497,9031,-31497,9031,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-29934,13327,-29934,13327,-29934,13327,-29934,13327,
-28931,15383,-28931,15383,-28931,15383,-28931,15383,
-27787,17363,-27787,17363,-27787,17363,-27787,17363,
-26509,19259,-26509,19259,-26509,19259,-26509,19259,
-25100,21062,-25100,21062,-25100,21062,-25100,21062,
-23570,22761,-23570,22761,-23570,22761,-23570,22761,
-21925,24350,-21925,24350,-21925,24350,-21925,24350,
-20173,25820,-20173,25820,-20173,25820,-20173,25820,
-18323,27165,-18323,27165,-18323,27165,-18323,27165};

void dft180(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa180[0];
  __m128i *twb128=(__m128i *)&twb180[0];
  __m128i x2128[180];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[180];//=&ytmp128array2[0];



  for (i=0,j=0;i<60;i++,j+=3) {
    x2128[i]    = x128[j];
    x2128[i+60] = x128[j+1];
    x2128[i+120] = x128[j+2];
  }

  dft60((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft60((int16_t *)(x2128+60),(int16_t *)(ytmp128+60),1);
  dft60((int16_t *)(x2128+120),(int16_t *)(ytmp128+120),1);

  bfly3_tw1(ytmp128,ytmp128+60,ytmp128+120,y128,y128+60,y128+120);
  for (i=1,j=0;i<60;i++,j++) {
    bfly3(ytmp128+i,
	  ytmp128+60+i,
	  ytmp128+120+i,
	  y128+i,
	  y128+60+i,
	  y128+120+i,
	  twa128+j,
	  twb128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[10]);
    
    for (i=0;i<180;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa192[376]__attribute__((aligned(16))) = {32749,-1072,32749,-1072,32749,-1072,32749,-1072,
32696,-2143,32696,-2143,32696,-2143,32696,-2143,
32609,-3211,32609,-3211,32609,-3211,32609,-3211,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32329,-5337,32329,-5337,32329,-5337,32329,-5337,
32137,-6392,32137,-6392,32137,-6392,32137,-6392,
31911,-7440,31911,-7440,31911,-7440,31911,-7440,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31356,-9511,31356,-9511,31356,-9511,31356,-9511,
31028,-10532,31028,-10532,31028,-10532,31028,-10532,
30666,-11542,30666,-11542,30666,-11542,30666,-11542,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29846,-13523,29846,-13523,29846,-13523,29846,-13523,
29387,-14492,29387,-14492,29387,-14492,29387,-14492,
28897,-15446,28897,-15446,28897,-15446,28897,-15446,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27825,-17303,27825,-17303,27825,-17303,27825,-17303,
27244,-18204,27244,-18204,27244,-18204,27244,-18204,
26634,-19086,26634,-19086,26634,-19086,26634,-19086,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
25329,-20787,25329,-20787,25329,-20787,25329,-20787,
24635,-21604,24635,-21604,24635,-21604,24635,-21604,
23915,-22399,23915,-22399,23915,-22399,23915,-22399,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22399,-23915,22399,-23915,22399,-23915,22399,-23915,
21604,-24635,21604,-24635,21604,-24635,21604,-24635,
20787,-25329,20787,-25329,20787,-25329,20787,-25329,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
19086,-26634,19086,-26634,19086,-26634,19086,-26634,
18204,-27244,18204,-27244,18204,-27244,18204,-27244,
17303,-27825,17303,-27825,17303,-27825,17303,-27825,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15446,-28897,15446,-28897,15446,-28897,15446,-28897,
14492,-29387,14492,-29387,14492,-29387,14492,-29387,
13523,-29846,13523,-29846,13523,-29846,13523,-29846,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
11542,-30666,11542,-30666,11542,-30666,11542,-30666,
10532,-31028,10532,-31028,10532,-31028,10532,-31028,
9511,-31356,9511,-31356,9511,-31356,9511,-31356,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7440,-31911,7440,-31911,7440,-31911,7440,-31911,
6392,-32137,6392,-32137,6392,-32137,6392,-32137,
5337,-32329,5337,-32329,5337,-32329,5337,-32329,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
3211,-32609,3211,-32609,3211,-32609,3211,-32609,
2143,-32696,2143,-32696,2143,-32696,2143,-32696,
1072,-32749,1072,-32749,1072,-32749,1072,-32749};

static int16_t twb192[376]__attribute__((aligned(16))) = {32696,-2143,32696,-2143,32696,-2143,32696,-2143,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32137,-6392,32137,-6392,32137,-6392,32137,-6392,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31028,-10532,31028,-10532,31028,-10532,31028,-10532,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29387,-14492,29387,-14492,29387,-14492,29387,-14492,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27244,-18204,27244,-18204,27244,-18204,27244,-18204,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
24635,-21604,24635,-21604,24635,-21604,24635,-21604,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21604,-24635,21604,-24635,21604,-24635,21604,-24635,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
18204,-27244,18204,-27244,18204,-27244,18204,-27244,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14492,-29387,14492,-29387,14492,-29387,14492,-29387,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
10532,-31028,10532,-31028,10532,-31028,10532,-31028,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
6392,-32137,6392,-32137,6392,-32137,6392,-32137,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
2143,-32696,2143,-32696,2143,-32696,2143,-32696,
0,-32767,0,-32767,0,-32767,0,-32767,
-2143,-32696,-2143,-32696,-2143,-32696,-2143,-32696,
-4276,-32486,-4276,-32486,-4276,-32486,-4276,-32486,
-6392,-32137,-6392,-32137,-6392,-32137,-6392,-32137,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-10532,-31028,-10532,-31028,-10532,-31028,-10532,-31028,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-14492,-29387,-14492,-29387,-14492,-29387,-14492,-29387,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18204,-27244,-18204,-27244,-18204,-27244,-18204,-27244,
-19947,-25995,-19947,-25995,-19947,-25995,-19947,-25995,
-21604,-24635,-21604,-24635,-21604,-24635,-21604,-24635,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24635,-21604,-24635,-21604,-24635,-21604,-24635,-21604,
-25995,-19947,-25995,-19947,-25995,-19947,-25995,-19947,
-27244,-18204,-27244,-18204,-27244,-18204,-27244,-18204,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29387,-14492,-29387,-14492,-29387,-14492,-29387,-14492,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-31028,-10532,-31028,-10532,-31028,-10532,-31028,-10532,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32137,-6392,-32137,-6392,-32137,-6392,-32137,-6392,
-32486,-4276,-32486,-4276,-32486,-4276,-32486,-4276,
-32696,-2143,-32696,-2143,-32696,-2143,-32696,-2143};

static int16_t twc192[376]__attribute__((aligned(16))) = {32609,-3211,32609,-3211,32609,-3211,32609,-3211,
32137,-6392,32137,-6392,32137,-6392,32137,-6392,
31356,-9511,31356,-9511,31356,-9511,31356,-9511,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
28897,-15446,28897,-15446,28897,-15446,28897,-15446,
27244,-18204,27244,-18204,27244,-18204,27244,-18204,
25329,-20787,25329,-20787,25329,-20787,25329,-20787,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
20787,-25329,20787,-25329,20787,-25329,20787,-25329,
18204,-27244,18204,-27244,18204,-27244,18204,-27244,
15446,-28897,15446,-28897,15446,-28897,15446,-28897,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
9511,-31356,9511,-31356,9511,-31356,9511,-31356,
6392,-32137,6392,-32137,6392,-32137,6392,-32137,
3211,-32609,3211,-32609,3211,-32609,3211,-32609,
0,-32767,0,-32767,0,-32767,0,-32767,
-3211,-32609,-3211,-32609,-3211,-32609,-3211,-32609,
-6392,-32137,-6392,-32137,-6392,-32137,-6392,-32137,
-9511,-31356,-9511,-31356,-9511,-31356,-9511,-31356,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-15446,-28897,-15446,-28897,-15446,-28897,-15446,-28897,
-18204,-27244,-18204,-27244,-18204,-27244,-18204,-27244,
-20787,-25329,-20787,-25329,-20787,-25329,-20787,-25329,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-25329,-20787,-25329,-20787,-25329,-20787,-25329,-20787,
-27244,-18204,-27244,-18204,-27244,-18204,-27244,-18204,
-28897,-15446,-28897,-15446,-28897,-15446,-28897,-15446,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-31356,-9511,-31356,-9511,-31356,-9511,-31356,-9511,
-32137,-6392,-32137,-6392,-32137,-6392,-32137,-6392,
-32609,-3211,-32609,-3211,-32609,-3211,-32609,-3211,
-32767,0,-32767,0,-32767,0,-32767,0,
-32609,3211,-32609,3211,-32609,3211,-32609,3211,
-32137,6392,-32137,6392,-32137,6392,-32137,6392,
-31356,9511,-31356,9511,-31356,9511,-31356,9511,
-30272,12539,-30272,12539,-30272,12539,-30272,12539,
-28897,15446,-28897,15446,-28897,15446,-28897,15446,
-27244,18204,-27244,18204,-27244,18204,-27244,18204,
-25329,20787,-25329,20787,-25329,20787,-25329,20787,
-23169,23169,-23169,23169,-23169,23169,-23169,23169,
-20787,25329,-20787,25329,-20787,25329,-20787,25329,
-18204,27244,-18204,27244,-18204,27244,-18204,27244,
-15446,28897,-15446,28897,-15446,28897,-15446,28897,
-12539,30272,-12539,30272,-12539,30272,-12539,30272,
-9511,31356,-9511,31356,-9511,31356,-9511,31356,
-6392,32137,-6392,32137,-6392,32137,-6392,32137,
-3211,32609,-3211,32609,-3211,32609,-3211,32609};

void dft192(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa192[0];
  __m128i *twb128=(__m128i *)&twb192[0];
  __m128i *twc128=(__m128i *)&twc192[0];
  __m128i x2128[192];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[192];//=&ytmp128array2[0];



  for (i=0,j=0;i<48;i++,j+=4) {
    x2128[i]    = x128[j];
    x2128[i+48] = x128[j+1];
    x2128[i+96] = x128[j+2];
    x2128[i+144] = x128[j+3];
  }

  dft48((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft48((int16_t *)(x2128+48),(int16_t *)(ytmp128+48),1);
  dft48((int16_t *)(x2128+96),(int16_t *)(ytmp128+96),1);
  dft48((int16_t *)(x2128+144),(int16_t *)(ytmp128+144),1);

  bfly4_tw1(ytmp128,ytmp128+48,ytmp128+96,ytmp128+144,y128,y128+48,y128+96,y128+144);
  for (i=1,j=0;i<48;i++,j++) {
    bfly4(ytmp128+i,
	  ytmp128+48+i,
	  ytmp128+96+i,
	  ytmp128+144+i,
	  y128+i,
	  y128+48+i,
	  y128+96+i,
	  y128+144+i,
	  twa128+j,
	  twb128+j,
	  twc128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[11]);
    
    for (i=0;i<216;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa216[568]__attribute__((aligned(16))) = {32753,-953,32753,-953,32753,-953,32753,-953,
32711,-1905,32711,-1905,32711,-1905,32711,-1905,
32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32545,-3804,32545,-3804,32545,-3804,32545,-3804,
32421,-4748,32421,-4748,32421,-4748,32421,-4748,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
32090,-6626,32090,-6626,32090,-6626,32090,-6626,
31883,-7556,31883,-7556,31883,-7556,31883,-7556,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31390,-9397,31390,-9397,31390,-9397,31390,-9397,
31103,-10306,31103,-10306,31103,-10306,31103,-10306,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30451,-12097,30451,-12097,30451,-12097,30451,-12097,
30087,-12978,30087,-12978,30087,-12978,30087,-12978,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
29281,-14705,29281,-14705,29281,-14705,29281,-14705,
28841,-15551,28841,-15551,28841,-15551,28841,-15551,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27888,-17201,27888,-17201,27888,-17201,27888,-17201,
27376,-18005,27376,-18005,27376,-18005,27376,-18005,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
26283,-19567,26283,-19567,26283,-19567,26283,-19567,
25702,-20323,25702,-20323,25702,-20323,25702,-20323,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
24477,-21783,24477,-21783,24477,-21783,24477,-21783,
23833,-22486,23833,-22486,23833,-22486,23833,-22486,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22486,-23833,22486,-23833,22486,-23833,22486,-23833,
21783,-24477,21783,-24477,21783,-24477,21783,-24477,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
20323,-25702,20323,-25702,20323,-25702,20323,-25702,
19567,-26283,19567,-26283,19567,-26283,19567,-26283,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
18005,-27376,18005,-27376,18005,-27376,18005,-27376,
17201,-27888,17201,-27888,17201,-27888,17201,-27888,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15551,-28841,15551,-28841,15551,-28841,15551,-28841,
14705,-29281,14705,-29281,14705,-29281,14705,-29281,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
12978,-30087,12978,-30087,12978,-30087,12978,-30087,
12097,-30451,12097,-30451,12097,-30451,12097,-30451,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
10306,-31103,10306,-31103,10306,-31103,10306,-31103,
9397,-31390,9397,-31390,9397,-31390,9397,-31390,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7556,-31883,7556,-31883,7556,-31883,7556,-31883,
6626,-32090,6626,-32090,6626,-32090,6626,-32090,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
4748,-32421,4748,-32421,4748,-32421,4748,-32421,
3804,-32545,3804,-32545,3804,-32545,3804,-32545,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
1905,-32711,1905,-32711,1905,-32711,1905,-32711,
953,-32753,953,-32753,953,-32753,953,-32753,
0,-32767,0,-32767,0,-32767,0,-32767,
-953,-32753,-953,-32753,-953,-32753,-953,-32753,
-1905,-32711,-1905,-32711,-1905,-32711,-1905,-32711,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-3804,-32545,-3804,-32545,-3804,-32545,-3804,-32545,
-4748,-32421,-4748,-32421,-4748,-32421,-4748,-32421,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-6626,-32090,-6626,-32090,-6626,-32090,-6626,-32090,
-7556,-31883,-7556,-31883,-7556,-31883,-7556,-31883,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-9397,-31390,-9397,-31390,-9397,-31390,-9397,-31390,
-10306,-31103,-10306,-31103,-10306,-31103,-10306,-31103,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12097,-30451,-12097,-30451,-12097,-30451,-12097,-30451,
-12978,-30087,-12978,-30087,-12978,-30087,-12978,-30087,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-14705,-29281,-14705,-29281,-14705,-29281,-14705,-29281,
-15551,-28841,-15551,-28841,-15551,-28841,-15551,-28841};

static int16_t twb216[568]__attribute__((aligned(16))) = {32711,-1905,32711,-1905,32711,-1905,32711,-1905,
32545,-3804,32545,-3804,32545,-3804,32545,-3804,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31883,-7556,31883,-7556,31883,-7556,31883,-7556,
31390,-9397,31390,-9397,31390,-9397,31390,-9397,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30087,-12978,30087,-12978,30087,-12978,30087,-12978,
29281,-14705,29281,-14705,29281,-14705,29281,-14705,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27376,-18005,27376,-18005,27376,-18005,27376,-18005,
26283,-19567,26283,-19567,26283,-19567,26283,-19567,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
23833,-22486,23833,-22486,23833,-22486,23833,-22486,
22486,-23833,22486,-23833,22486,-23833,22486,-23833,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
19567,-26283,19567,-26283,19567,-26283,19567,-26283,
18005,-27376,18005,-27376,18005,-27376,18005,-27376,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14705,-29281,14705,-29281,14705,-29281,14705,-29281,
12978,-30087,12978,-30087,12978,-30087,12978,-30087,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
9397,-31390,9397,-31390,9397,-31390,9397,-31390,
7556,-31883,7556,-31883,7556,-31883,7556,-31883,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
3804,-32545,3804,-32545,3804,-32545,3804,-32545,
1905,-32711,1905,-32711,1905,-32711,1905,-32711,
0,-32767,0,-32767,0,-32767,0,-32767,
-1905,-32711,-1905,-32711,-1905,-32711,-1905,-32711,
-3804,-32545,-3804,-32545,-3804,-32545,-3804,-32545,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-7556,-31883,-7556,-31883,-7556,-31883,-7556,-31883,
-9397,-31390,-9397,-31390,-9397,-31390,-9397,-31390,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12978,-30087,-12978,-30087,-12978,-30087,-12978,-30087,
-14705,-29281,-14705,-29281,-14705,-29281,-14705,-29281,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18005,-27376,-18005,-27376,-18005,-27376,-18005,-27376,
-19567,-26283,-19567,-26283,-19567,-26283,-19567,-26283,
-21062,-25100,-21062,-25100,-21062,-25100,-21062,-25100,
-22486,-23833,-22486,-23833,-22486,-23833,-22486,-23833,
-23833,-22486,-23833,-22486,-23833,-22486,-23833,-22486,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-26283,-19567,-26283,-19567,-26283,-19567,-26283,-19567,
-27376,-18005,-27376,-18005,-27376,-18005,-27376,-18005,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29281,-14705,-29281,-14705,-29281,-14705,-29281,-14705,
-30087,-12978,-30087,-12978,-30087,-12978,-30087,-12978,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31390,-9397,-31390,-9397,-31390,-9397,-31390,-9397,
-31883,-7556,-31883,-7556,-31883,-7556,-31883,-7556,
-32269,-5689,-32269,-5689,-32269,-5689,-32269,-5689,
-32545,-3804,-32545,-3804,-32545,-3804,-32545,-3804,
-32711,-1905,-32711,-1905,-32711,-1905,-32711,-1905,
-32767,0,-32767,0,-32767,0,-32767,0,
-32711,1905,-32711,1905,-32711,1905,-32711,1905,
-32545,3804,-32545,3804,-32545,3804,-32545,3804,
-32269,5689,-32269,5689,-32269,5689,-32269,5689,
-31883,7556,-31883,7556,-31883,7556,-31883,7556,
-31390,9397,-31390,9397,-31390,9397,-31390,9397,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-30087,12978,-30087,12978,-30087,12978,-30087,12978,
-29281,14705,-29281,14705,-29281,14705,-29281,14705,
-28377,16383,-28377,16383,-28377,16383,-28377,16383,
-27376,18005,-27376,18005,-27376,18005,-27376,18005,
-26283,19567,-26283,19567,-26283,19567,-26283,19567,
-25100,21062,-25100,21062,-25100,21062,-25100,21062,
-23833,22486,-23833,22486,-23833,22486,-23833,22486,
-22486,23833,-22486,23833,-22486,23833,-22486,23833,
-21062,25100,-21062,25100,-21062,25100,-21062,25100,
-19567,26283,-19567,26283,-19567,26283,-19567,26283,
-18005,27376,-18005,27376,-18005,27376,-18005,27376};

void dft216(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa216[0];
  __m128i *twb128=(__m128i *)&twb216[0];
  __m128i x2128[216];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[216];//=&ytmp128array3[0];



  for (i=0,j=0;i<72;i++,j+=3) {
    x2128[i]    = x128[j];
    x2128[i+72] = x128[j+1];
    x2128[i+144] = x128[j+2];
  }

  dft72((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft72((int16_t *)(x2128+72),(int16_t *)(ytmp128+72),1);
  dft72((int16_t *)(x2128+144),(int16_t *)(ytmp128+144),1);

  bfly3_tw1(ytmp128,ytmp128+72,ytmp128+144,y128,y128+72,y128+144);
  for (i=1,j=0;i<72;i++,j++) {
    bfly3(ytmp128+i,
	  ytmp128+72+i,
	  ytmp128+144+i,
	  y128+i,
	  y128+72+i,
	  y128+144+i,
	  twa128+j,
	  twb128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[12]);
    
    for (i=0;i<216;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa240[472]__attribute__((aligned(16))) = {32755,-857,32755,-857,32755,-857,32755,-857,
32722,-1714,32722,-1714,32722,-1714,32722,-1714,
32665,-2570,32665,-2570,32665,-2570,32665,-2570,
32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32363,-5125,32363,-5125,32363,-5125,32363,-5125,
32218,-5971,32218,-5971,32218,-5971,32218,-5971,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31861,-7649,31861,-7649,31861,-7649,31861,-7649,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31417,-9306,31417,-9306,31417,-9306,31417,-9306,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30887,-10937,30887,-10937,30887,-10937,30887,-10937,
30590,-11742,30590,-11742,30590,-11742,30590,-11742,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29575,-14106,29575,-14106,29575,-14106,29575,-14106,
29195,-14875,29195,-14875,29195,-14875,29195,-14875,
28796,-15635,28796,-15635,28796,-15635,28796,-15635,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27938,-17120,27938,-17120,27938,-17120,27938,-17120,
27480,-17846,27480,-17846,27480,-17846,27480,-17846,
27004,-18559,27004,-18559,27004,-18559,27004,-18559,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
25464,-20620,25464,-20620,25464,-20620,25464,-20620,
24916,-21280,24916,-21280,24916,-21280,24916,-21280,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
23768,-22555,23768,-22555,23768,-22555,23768,-22555,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22555,-23768,22555,-23768,22555,-23768,22555,-23768,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
21280,-24916,21280,-24916,21280,-24916,21280,-24916,
20620,-25464,20620,-25464,20620,-25464,20620,-25464,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
18559,-27004,18559,-27004,18559,-27004,18559,-27004,
17846,-27480,17846,-27480,17846,-27480,17846,-27480,
17120,-27938,17120,-27938,17120,-27938,17120,-27938,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15635,-28796,15635,-28796,15635,-28796,15635,-28796,
14875,-29195,14875,-29195,14875,-29195,14875,-29195,
14106,-29575,14106,-29575,14106,-29575,14106,-29575,
13327,-29934,13327,-29934,13327,-29934,13327,-29934,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
11742,-30590,11742,-30590,11742,-30590,11742,-30590,
10937,-30887,10937,-30887,10937,-30887,10937,-30887,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
9306,-31417,9306,-31417,9306,-31417,9306,-31417,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7649,-31861,7649,-31861,7649,-31861,7649,-31861,
6812,-32050,6812,-32050,6812,-32050,6812,-32050,
5971,-32218,5971,-32218,5971,-32218,5971,-32218,
5125,-32363,5125,-32363,5125,-32363,5125,-32363,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
2570,-32665,2570,-32665,2570,-32665,2570,-32665,
1714,-32722,1714,-32722,1714,-32722,1714,-32722,
857,-32755,857,-32755,857,-32755,857,-32755};

static int16_t twb240[472]__attribute__((aligned(16))) = {32722,-1714,32722,-1714,32722,-1714,32722,-1714,
32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32363,-5125,32363,-5125,32363,-5125,32363,-5125,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30590,-11742,30590,-11742,30590,-11742,30590,-11742,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29195,-14875,29195,-14875,29195,-14875,29195,-14875,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27480,-17846,27480,-17846,27480,-17846,27480,-17846,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25464,-20620,25464,-20620,25464,-20620,25464,-20620,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
20620,-25464,20620,-25464,20620,-25464,20620,-25464,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
17846,-27480,17846,-27480,17846,-27480,17846,-27480,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
14875,-29195,14875,-29195,14875,-29195,14875,-29195,
13327,-29934,13327,-29934,13327,-29934,13327,-29934,
11742,-30590,11742,-30590,11742,-30590,11742,-30590,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
6812,-32050,6812,-32050,6812,-32050,6812,-32050,
5125,-32363,5125,-32363,5125,-32363,5125,-32363,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
1714,-32722,1714,-32722,1714,-32722,1714,-32722,
0,-32767,0,-32767,0,-32767,0,-32767,
-1714,-32722,-1714,-32722,-1714,-32722,-1714,-32722,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-5125,-32363,-5125,-32363,-5125,-32363,-5125,-32363,
-6812,-32050,-6812,-32050,-6812,-32050,-6812,-32050,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-11742,-30590,-11742,-30590,-11742,-30590,-11742,-30590,
-13327,-29934,-13327,-29934,-13327,-29934,-13327,-29934,
-14875,-29195,-14875,-29195,-14875,-29195,-14875,-29195,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-17846,-27480,-17846,-27480,-17846,-27480,-17846,-27480,
-19259,-26509,-19259,-26509,-19259,-26509,-19259,-26509,
-20620,-25464,-20620,-25464,-20620,-25464,-20620,-25464,
-21925,-24350,-21925,-24350,-21925,-24350,-21925,-24350,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24350,-21925,-24350,-21925,-24350,-21925,-24350,-21925,
-25464,-20620,-25464,-20620,-25464,-20620,-25464,-20620,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-27480,-17846,-27480,-17846,-27480,-17846,-27480,-17846,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29195,-14875,-29195,-14875,-29195,-14875,-29195,-14875,
-29934,-13327,-29934,-13327,-29934,-13327,-29934,-13327,
-30590,-11742,-30590,-11742,-30590,-11742,-30590,-11742,
-31163,-10125,-31163,-10125,-31163,-10125,-31163,-10125,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-32050,-6812,-32050,-6812,-32050,-6812,-32050,-6812,
-32363,-5125,-32363,-5125,-32363,-5125,-32363,-5125,
-32587,-3425,-32587,-3425,-32587,-3425,-32587,-3425,
-32722,-1714,-32722,-1714,-32722,-1714,-32722,-1714};

static int16_t twc240[472]__attribute__((aligned(16))) = {32665,-2570,32665,-2570,32665,-2570,32665,-2570,
32363,-5125,32363,-5125,32363,-5125,32363,-5125,
31861,-7649,31861,-7649,31861,-7649,31861,-7649,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29195,-14875,29195,-14875,29195,-14875,29195,-14875,
27938,-17120,27938,-17120,27938,-17120,27938,-17120,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
24916,-21280,24916,-21280,24916,-21280,24916,-21280,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
21280,-24916,21280,-24916,21280,-24916,21280,-24916,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
17120,-27938,17120,-27938,17120,-27938,17120,-27938,
14875,-29195,14875,-29195,14875,-29195,14875,-29195,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
7649,-31861,7649,-31861,7649,-31861,7649,-31861,
5125,-32363,5125,-32363,5125,-32363,5125,-32363,
2570,-32665,2570,-32665,2570,-32665,2570,-32665,
0,-32767,0,-32767,0,-32767,0,-32767,
-2570,-32665,-2570,-32665,-2570,-32665,-2570,-32665,
-5125,-32363,-5125,-32363,-5125,-32363,-5125,-32363,
-7649,-31861,-7649,-31861,-7649,-31861,-7649,-31861,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-14875,-29195,-14875,-29195,-14875,-29195,-14875,-29195,
-17120,-27938,-17120,-27938,-17120,-27938,-17120,-27938,
-19259,-26509,-19259,-26509,-19259,-26509,-19259,-26509,
-21280,-24916,-21280,-24916,-21280,-24916,-21280,-24916,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24916,-21280,-24916,-21280,-24916,-21280,-24916,-21280,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-27938,-17120,-27938,-17120,-27938,-17120,-27938,-17120,
-29195,-14875,-29195,-14875,-29195,-14875,-29195,-14875,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-31163,-10125,-31163,-10125,-31163,-10125,-31163,-10125,
-31861,-7649,-31861,-7649,-31861,-7649,-31861,-7649,
-32363,-5125,-32363,-5125,-32363,-5125,-32363,-5125,
-32665,-2570,-32665,-2570,-32665,-2570,-32665,-2570,
-32767,0,-32767,0,-32767,0,-32767,0,
-32665,2570,-32665,2570,-32665,2570,-32665,2570,
-32363,5125,-32363,5125,-32363,5125,-32363,5125,
-31861,7649,-31861,7649,-31861,7649,-31861,7649,
-31163,10125,-31163,10125,-31163,10125,-31163,10125,
-30272,12539,-30272,12539,-30272,12539,-30272,12539,
-29195,14875,-29195,14875,-29195,14875,-29195,14875,
-27938,17120,-27938,17120,-27938,17120,-27938,17120,
-26509,19259,-26509,19259,-26509,19259,-26509,19259,
-24916,21280,-24916,21280,-24916,21280,-24916,21280,
-23169,23169,-23169,23169,-23169,23169,-23169,23169,
-21280,24916,-21280,24916,-21280,24916,-21280,24916,
-19259,26509,-19259,26509,-19259,26509,-19259,26509,
-17120,27938,-17120,27938,-17120,27938,-17120,27938,
-14875,29195,-14875,29195,-14875,29195,-14875,29195,
-12539,30272,-12539,30272,-12539,30272,-12539,30272,
-10125,31163,-10125,31163,-10125,31163,-10125,31163,
-7649,31861,-7649,31861,-7649,31861,-7649,31861,
-5125,32363,-5125,32363,-5125,32363,-5125,32363,
-2570,32665,-2570,32665,-2570,32665,-2570,32665};

void dft240(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa240[0];
  __m128i *twb128=(__m128i *)&twb240[0];
  __m128i *twc128=(__m128i *)&twc240[0];
  __m128i x2128[240];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[240];//=&ytmp128array2[0];



  for (i=0,j=0;i<60;i++,j+=4) {
    x2128[i]    = x128[j];
    x2128[i+60] = x128[j+1];
    x2128[i+120] = x128[j+2];
    x2128[i+180] = x128[j+3];
  }

  dft60((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft60((int16_t *)(x2128+60),(int16_t *)(ytmp128+60),1);
  dft60((int16_t *)(x2128+120),(int16_t *)(ytmp128+120),1);
  dft60((int16_t *)(x2128+180),(int16_t *)(ytmp128+180),1);

  bfly4_tw1(ytmp128,ytmp128+60,ytmp128+120,ytmp128+180,y128,y128+60,y128+120,y128+180);
  for (i=1,j=0;i<60;i++,j++) {
    bfly4(ytmp128+i,
	  ytmp128+60+i,
	  ytmp128+120+i,
	  ytmp128+180+i,
	  y128+i,
	  y128+60+i,
	  y128+120+i,
	  y128+180+i,
	  twa128+j,
	  twb128+j,
	  twc128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[13]);
    
    for (i=0;i<240;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

/* Twiddles generated with
twa = floor(32767*exp(-sqrt(-1)*2*pi*(1:95)/288));
twb = floor(32767*exp(-sqrt(-1)*2*pi*(2:2:190)/288));
twa2 = zeros(1,191);
twb2 = zeros(1,191);
twa2(1:2:end) = real(twa);
twa2(2:2:end) = imag(twa);
twb2(1:2:end) = real(twb);
twb2(2:2:end) = imag(twb);


 */
static int16_t twa288[760]__attribute__((aligned(16))) = {32759,-714,32759,-714,32759,-714,32759,-714,
32735,-1429,32735,-1429,32735,-1429,32735,-1429,
32696,-2143,32696,-2143,32696,-2143,32696,-2143,
32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32572,-3567,32572,-3567,32572,-3567,32572,-3567,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32385,-4984,32385,-4984,32385,-4984,32385,-4984,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
32137,-6392,32137,-6392,32137,-6392,32137,-6392,
31990,-7092,31990,-7092,31990,-7092,31990,-7092,
31827,-7788,31827,-7788,31827,-7788,31827,-7788,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31457,-9169,31457,-9169,31457,-9169,31457,-9169,
31250,-9853,31250,-9853,31250,-9853,31250,-9853,
31028,-10532,31028,-10532,31028,-10532,31028,-10532,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30539,-11876,30539,-11876,30539,-11876,30539,-11876,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29992,-13196,29992,-13196,29992,-13196,29992,-13196,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
29387,-14492,29387,-14492,29387,-14492,29387,-14492,
29064,-15130,29064,-15130,29064,-15130,29064,-15130,
28727,-15760,28727,-15760,28727,-15760,28727,-15760,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
28012,-16998,28012,-16998,28012,-16998,28012,-16998,
27635,-17605,27635,-17605,27635,-17605,27635,-17605,
27244,-18204,27244,-18204,27244,-18204,27244,-18204,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
26424,-19375,26424,-19375,26424,-19375,26424,-19375,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
25554,-20509,25554,-20509,25554,-20509,25554,-20509,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
24635,-21604,24635,-21604,24635,-21604,24635,-21604,
24158,-22137,24158,-22137,24158,-22137,24158,-22137,
23669,-22658,23669,-22658,23669,-22658,23669,-22658,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22658,-23669,22658,-23669,22658,-23669,22658,-23669,
22137,-24158,22137,-24158,22137,-24158,22137,-24158,
21604,-24635,21604,-24635,21604,-24635,21604,-24635,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
20509,-25554,20509,-25554,20509,-25554,20509,-25554,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
19375,-26424,19375,-26424,19375,-26424,19375,-26424,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
18204,-27244,18204,-27244,18204,-27244,18204,-27244,
17605,-27635,17605,-27635,17605,-27635,17605,-27635,
16998,-28012,16998,-28012,16998,-28012,16998,-28012,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15760,-28727,15760,-28727,15760,-28727,15760,-28727,
15130,-29064,15130,-29064,15130,-29064,15130,-29064,
14492,-29387,14492,-29387,14492,-29387,14492,-29387,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
13196,-29992,13196,-29992,13196,-29992,13196,-29992,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
11876,-30539,11876,-30539,11876,-30539,11876,-30539,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
10532,-31028,10532,-31028,10532,-31028,10532,-31028,
9853,-31250,9853,-31250,9853,-31250,9853,-31250,
9169,-31457,9169,-31457,9169,-31457,9169,-31457,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7788,-31827,7788,-31827,7788,-31827,7788,-31827,
7092,-31990,7092,-31990,7092,-31990,7092,-31990,
6392,-32137,6392,-32137,6392,-32137,6392,-32137,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
4984,-32385,4984,-32385,4984,-32385,4984,-32385,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
3567,-32572,3567,-32572,3567,-32572,3567,-32572,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
2143,-32696,2143,-32696,2143,-32696,2143,-32696,
1429,-32735,1429,-32735,1429,-32735,1429,-32735,
714,-32759,714,-32759,714,-32759,714,-32759,
0,-32767,0,-32767,0,-32767,0,-32767,
-714,-32759,-714,-32759,-714,-32759,-714,-32759,
-1429,-32735,-1429,-32735,-1429,-32735,-1429,-32735,
-2143,-32696,-2143,-32696,-2143,-32696,-2143,-32696,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-3567,-32572,-3567,-32572,-3567,-32572,-3567,-32572,
-4276,-32486,-4276,-32486,-4276,-32486,-4276,-32486,
-4984,-32385,-4984,-32385,-4984,-32385,-4984,-32385,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-6392,-32137,-6392,-32137,-6392,-32137,-6392,-32137,
-7092,-31990,-7092,-31990,-7092,-31990,-7092,-31990,
-7788,-31827,-7788,-31827,-7788,-31827,-7788,-31827,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-9169,-31457,-9169,-31457,-9169,-31457,-9169,-31457,
-9853,-31250,-9853,-31250,-9853,-31250,-9853,-31250,
-10532,-31028,-10532,-31028,-10532,-31028,-10532,-31028,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-11876,-30539,-11876,-30539,-11876,-30539,-11876,-30539,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-13196,-29992,-13196,-29992,-13196,-29992,-13196,-29992,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-14492,-29387,-14492,-29387,-14492,-29387,-14492,-29387,
-15130,-29064,-15130,-29064,-15130,-29064,-15130,-29064,
-15760,-28727,-15760,-28727,-15760,-28727,-15760,-28727};

static int16_t twb288[760]__attribute__((aligned(16))) = {32735,-1429,32735,-1429,32735,-1429,32735,-1429,
32642,-2855,32642,-2855,32642,-2855,32642,-2855,
32486,-4276,32486,-4276,32486,-4276,32486,-4276,
32269,-5689,32269,-5689,32269,-5689,32269,-5689,
31990,-7092,31990,-7092,31990,-7092,31990,-7092,
31650,-8480,31650,-8480,31650,-8480,31650,-8480,
31250,-9853,31250,-9853,31250,-9853,31250,-9853,
30790,-11206,30790,-11206,30790,-11206,30790,-11206,
30272,-12539,30272,-12539,30272,-12539,30272,-12539,
29696,-13847,29696,-13847,29696,-13847,29696,-13847,
29064,-15130,29064,-15130,29064,-15130,29064,-15130,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
27635,-17605,27635,-17605,27635,-17605,27635,-17605,
26841,-18794,26841,-18794,26841,-18794,26841,-18794,
25995,-19947,25995,-19947,25995,-19947,25995,-19947,
25100,-21062,25100,-21062,25100,-21062,25100,-21062,
24158,-22137,24158,-22137,24158,-22137,24158,-22137,
23169,-23169,23169,-23169,23169,-23169,23169,-23169,
22137,-24158,22137,-24158,22137,-24158,22137,-24158,
21062,-25100,21062,-25100,21062,-25100,21062,-25100,
19947,-25995,19947,-25995,19947,-25995,19947,-25995,
18794,-26841,18794,-26841,18794,-26841,18794,-26841,
17605,-27635,17605,-27635,17605,-27635,17605,-27635,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15130,-29064,15130,-29064,15130,-29064,15130,-29064,
13847,-29696,13847,-29696,13847,-29696,13847,-29696,
12539,-30272,12539,-30272,12539,-30272,12539,-30272,
11206,-30790,11206,-30790,11206,-30790,11206,-30790,
9853,-31250,9853,-31250,9853,-31250,9853,-31250,
8480,-31650,8480,-31650,8480,-31650,8480,-31650,
7092,-31990,7092,-31990,7092,-31990,7092,-31990,
5689,-32269,5689,-32269,5689,-32269,5689,-32269,
4276,-32486,4276,-32486,4276,-32486,4276,-32486,
2855,-32642,2855,-32642,2855,-32642,2855,-32642,
1429,-32735,1429,-32735,1429,-32735,1429,-32735,
0,-32767,0,-32767,0,-32767,0,-32767,
-1429,-32735,-1429,-32735,-1429,-32735,-1429,-32735,
-2855,-32642,-2855,-32642,-2855,-32642,-2855,-32642,
-4276,-32486,-4276,-32486,-4276,-32486,-4276,-32486,
-5689,-32269,-5689,-32269,-5689,-32269,-5689,-32269,
-7092,-31990,-7092,-31990,-7092,-31990,-7092,-31990,
-8480,-31650,-8480,-31650,-8480,-31650,-8480,-31650,
-9853,-31250,-9853,-31250,-9853,-31250,-9853,-31250,
-11206,-30790,-11206,-30790,-11206,-30790,-11206,-30790,
-12539,-30272,-12539,-30272,-12539,-30272,-12539,-30272,
-13847,-29696,-13847,-29696,-13847,-29696,-13847,-29696,
-15130,-29064,-15130,-29064,-15130,-29064,-15130,-29064,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-17605,-27635,-17605,-27635,-17605,-27635,-17605,-27635,
-18794,-26841,-18794,-26841,-18794,-26841,-18794,-26841,
-19947,-25995,-19947,-25995,-19947,-25995,-19947,-25995,
-21062,-25100,-21062,-25100,-21062,-25100,-21062,-25100,
-22137,-24158,-22137,-24158,-22137,-24158,-22137,-24158,
-23169,-23169,-23169,-23169,-23169,-23169,-23169,-23169,
-24158,-22137,-24158,-22137,-24158,-22137,-24158,-22137,
-25100,-21062,-25100,-21062,-25100,-21062,-25100,-21062,
-25995,-19947,-25995,-19947,-25995,-19947,-25995,-19947,
-26841,-18794,-26841,-18794,-26841,-18794,-26841,-18794,
-27635,-17605,-27635,-17605,-27635,-17605,-27635,-17605,
-28377,-16383,-28377,-16383,-28377,-16383,-28377,-16383,
-29064,-15130,-29064,-15130,-29064,-15130,-29064,-15130,
-29696,-13847,-29696,-13847,-29696,-13847,-29696,-13847,
-30272,-12539,-30272,-12539,-30272,-12539,-30272,-12539,
-30790,-11206,-30790,-11206,-30790,-11206,-30790,-11206,
-31250,-9853,-31250,-9853,-31250,-9853,-31250,-9853,
-31650,-8480,-31650,-8480,-31650,-8480,-31650,-8480,
-31990,-7092,-31990,-7092,-31990,-7092,-31990,-7092,
-32269,-5689,-32269,-5689,-32269,-5689,-32269,-5689,
-32486,-4276,-32486,-4276,-32486,-4276,-32486,-4276,
-32642,-2855,-32642,-2855,-32642,-2855,-32642,-2855,
-32735,-1429,-32735,-1429,-32735,-1429,-32735,-1429,
-32767,0,-32767,0,-32767,0,-32767,0,
-32735,1429,-32735,1429,-32735,1429,-32735,1429,
-32642,2855,-32642,2855,-32642,2855,-32642,2855,
-32486,4276,-32486,4276,-32486,4276,-32486,4276,
-32269,5689,-32269,5689,-32269,5689,-32269,5689,
-31990,7092,-31990,7092,-31990,7092,-31990,7092,
-31650,8480,-31650,8480,-31650,8480,-31650,8480,
-31250,9853,-31250,9853,-31250,9853,-31250,9853,
-30790,11206,-30790,11206,-30790,11206,-30790,11206,
-30272,12539,-30272,12539,-30272,12539,-30272,12539,
-29696,13847,-29696,13847,-29696,13847,-29696,13847,
-29064,15130,-29064,15130,-29064,15130,-29064,15130,
-28377,16383,-28377,16383,-28377,16383,-28377,16383,
-27635,17605,-27635,17605,-27635,17605,-27635,17605,
-26841,18794,-26841,18794,-26841,18794,-26841,18794,
-25995,19947,-25995,19947,-25995,19947,-25995,19947,
-25100,21062,-25100,21062,-25100,21062,-25100,21062,
-24158,22137,-24158,22137,-24158,22137,-24158,22137,
-23169,23169,-23169,23169,-23169,23169,-23169,23169,
-22137,24158,-22137,24158,-22137,24158,-22137,24158,
-21062,25100,-21062,25100,-21062,25100,-21062,25100,
-19947,25995,-19947,25995,-19947,25995,-19947,25995,
-18794,26841,-18794,26841,-18794,26841,-18794,26841,
-17605,27635,-17605,27635,-17605,27635,-17605,27635};

void dft288(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa288[0];
  __m128i *twb128=(__m128i *)&twb288[0];
  __m128i x2128[288];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[288];//=&ytmp128array3[0];



  for (i=0,j=0;i<96;i++,j+=3) {
    x2128[i]    = x128[j];
    x2128[i+96] = x128[j+1];
    x2128[i+192] = x128[j+2];
  }

  dft96((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft96((int16_t *)(x2128+96),(int16_t *)(ytmp128+96),1);
  dft96((int16_t *)(x2128+192),(int16_t *)(ytmp128+192),1);

  bfly3_tw1(ytmp128,ytmp128+96,ytmp128+192,y128,y128+96,y128+192);
  for (i=1,j=0;i<96;i++,j++) {
    bfly3(ytmp128+i,
	  ytmp128+96+i,
	  ytmp128+192+i,
	  y128+i,
	  y128+96+i,
	  y128+192+i,
	  twa128+j,
	  twb128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[14]);
    
    for (i=0;i<288;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

static int16_t twa300[472]__attribute__((aligned(16))) = {32759,-686,32759,-686,32759,-686,32759,-686,
32738,-1372,32738,-1372,32738,-1372,32738,-1372,
32702,-2057,32702,-2057,32702,-2057,32702,-2057,
32652,-2741,32652,-2741,32652,-2741,32652,-2741,
32587,-3425,32587,-3425,32587,-3425,32587,-3425,
32508,-4106,32508,-4106,32508,-4106,32508,-4106,
32415,-4786,32415,-4786,32415,-4786,32415,-4786,
32308,-5464,32308,-5464,32308,-5464,32308,-5464,
32186,-6139,32186,-6139,32186,-6139,32186,-6139,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31901,-7482,31901,-7482,31901,-7482,31901,-7482,
31737,-8148,31737,-8148,31737,-8148,31737,-8148,
31559,-8811,31559,-8811,31559,-8811,31559,-8811,
31368,-9470,31368,-9470,31368,-9470,31368,-9470,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30944,-10775,30944,-10775,30944,-10775,30944,-10775,
30711,-11421,30711,-11421,30711,-11421,30711,-11421,
30465,-12062,30465,-12062,30465,-12062,30465,-12062,
30206,-12697,30206,-12697,30206,-12697,30206,-12697,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29648,-13951,29648,-13951,29648,-13951,29648,-13951,
29349,-14569,29349,-14569,29349,-14569,29349,-14569,
29038,-15180,29038,-15180,29038,-15180,29038,-15180,
28713,-15785,28713,-15785,28713,-15785,28713,-15785,
28377,-16383,28377,-16383,28377,-16383,28377,-16383,
28027,-16974,28027,-16974,28027,-16974,28027,-16974,
27666,-17557,27666,-17557,27666,-17557,27666,-17557,
27292,-18132,27292,-18132,27292,-18132,27292,-18132,
26906,-18700,26906,-18700,26906,-18700,26906,-18700,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
26099,-19810,26099,-19810,26099,-19810,26099,-19810,
25679,-20353,25679,-20353,25679,-20353,25679,-20353,
25247,-20886,25247,-20886,25247,-20886,25247,-20886,
24804,-21410,24804,-21410,24804,-21410,24804,-21410,
24350,-21925,24350,-21925,24350,-21925,24350,-21925,
23886,-22430,23886,-22430,23886,-22430,23886,-22430,
23411,-22925,23411,-22925,23411,-22925,23411,-22925,
22925,-23411,22925,-23411,22925,-23411,22925,-23411,
22430,-23886,22430,-23886,22430,-23886,22430,-23886,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
21410,-24804,21410,-24804,21410,-24804,21410,-24804,
20886,-25247,20886,-25247,20886,-25247,20886,-25247,
20353,-25679,20353,-25679,20353,-25679,20353,-25679,
19810,-26099,19810,-26099,19810,-26099,19810,-26099,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
18700,-26906,18700,-26906,18700,-26906,18700,-26906,
18132,-27292,18132,-27292,18132,-27292,18132,-27292,
17557,-27666,17557,-27666,17557,-27666,17557,-27666,
16974,-28027,16974,-28027,16974,-28027,16974,-28027,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15785,-28713,15785,-28713,15785,-28713,15785,-28713,
15180,-29038,15180,-29038,15180,-29038,15180,-29038,
14569,-29349,14569,-29349,14569,-29349,14569,-29349,
13951,-29648,13951,-29648,13951,-29648,13951,-29648,
13327,-29934,13327,-29934,13327,-29934,13327,-29934,
12697,-30206,12697,-30206,12697,-30206,12697,-30206,
12062,-30465,12062,-30465,12062,-30465,12062,-30465,
11421,-30711,11421,-30711,11421,-30711,11421,-30711,
10775,-30944,10775,-30944,10775,-30944,10775,-30944};

static int16_t twb300[472]__attribute__((aligned(16))) = {32738,-1372,32738,-1372,32738,-1372,32738,-1372,
32652,-2741,32652,-2741,32652,-2741,32652,-2741,
32508,-4106,32508,-4106,32508,-4106,32508,-4106,
32308,-5464,32308,-5464,32308,-5464,32308,-5464,
32050,-6812,32050,-6812,32050,-6812,32050,-6812,
31737,-8148,31737,-8148,31737,-8148,31737,-8148,
31368,-9470,31368,-9470,31368,-9470,31368,-9470,
30944,-10775,30944,-10775,30944,-10775,30944,-10775,
30465,-12062,30465,-12062,30465,-12062,30465,-12062,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
29349,-14569,29349,-14569,29349,-14569,29349,-14569,
28713,-15785,28713,-15785,28713,-15785,28713,-15785,
28027,-16974,28027,-16974,28027,-16974,28027,-16974,
27292,-18132,27292,-18132,27292,-18132,27292,-18132,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25679,-20353,25679,-20353,25679,-20353,25679,-20353,
24804,-21410,24804,-21410,24804,-21410,24804,-21410,
23886,-22430,23886,-22430,23886,-22430,23886,-22430,
22925,-23411,22925,-23411,22925,-23411,22925,-23411,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
20886,-25247,20886,-25247,20886,-25247,20886,-25247,
19810,-26099,19810,-26099,19810,-26099,19810,-26099,
18700,-26906,18700,-26906,18700,-26906,18700,-26906,
17557,-27666,17557,-27666,17557,-27666,17557,-27666,
16383,-28377,16383,-28377,16383,-28377,16383,-28377,
15180,-29038,15180,-29038,15180,-29038,15180,-29038,
13951,-29648,13951,-29648,13951,-29648,13951,-29648,
12697,-30206,12697,-30206,12697,-30206,12697,-30206,
11421,-30711,11421,-30711,11421,-30711,11421,-30711,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
8811,-31559,8811,-31559,8811,-31559,8811,-31559,
7482,-31901,7482,-31901,7482,-31901,7482,-31901,
6139,-32186,6139,-32186,6139,-32186,6139,-32186,
4786,-32415,4786,-32415,4786,-32415,4786,-32415,
3425,-32587,3425,-32587,3425,-32587,3425,-32587,
2057,-32702,2057,-32702,2057,-32702,2057,-32702,
686,-32759,686,-32759,686,-32759,686,-32759,
-686,-32759,-686,-32759,-686,-32759,-686,-32759,
-2057,-32702,-2057,-32702,-2057,-32702,-2057,-32702,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-4786,-32415,-4786,-32415,-4786,-32415,-4786,-32415,
-6139,-32186,-6139,-32186,-6139,-32186,-6139,-32186,
-7482,-31901,-7482,-31901,-7482,-31901,-7482,-31901,
-8811,-31559,-8811,-31559,-8811,-31559,-8811,-31559,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-11421,-30711,-11421,-30711,-11421,-30711,-11421,-30711,
-12697,-30206,-12697,-30206,-12697,-30206,-12697,-30206,
-13951,-29648,-13951,-29648,-13951,-29648,-13951,-29648,
-15180,-29038,-15180,-29038,-15180,-29038,-15180,-29038,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-17557,-27666,-17557,-27666,-17557,-27666,-17557,-27666,
-18700,-26906,-18700,-26906,-18700,-26906,-18700,-26906,
-19810,-26099,-19810,-26099,-19810,-26099,-19810,-26099,
-20886,-25247,-20886,-25247,-20886,-25247,-20886,-25247,
-21925,-24350,-21925,-24350,-21925,-24350,-21925,-24350,
-22925,-23411,-22925,-23411,-22925,-23411,-22925,-23411,
-23886,-22430,-23886,-22430,-23886,-22430,-23886,-22430,
-24804,-21410,-24804,-21410,-24804,-21410,-24804,-21410,
-25679,-20353,-25679,-20353,-25679,-20353,-25679,-20353};

static int16_t twc300[472]__attribute__((aligned(16))) = {32702,-2057,32702,-2057,32702,-2057,32702,-2057,
32508,-4106,32508,-4106,32508,-4106,32508,-4106,
32186,-6139,32186,-6139,32186,-6139,32186,-6139,
31737,-8148,31737,-8148,31737,-8148,31737,-8148,
31163,-10125,31163,-10125,31163,-10125,31163,-10125,
30465,-12062,30465,-12062,30465,-12062,30465,-12062,
29648,-13951,29648,-13951,29648,-13951,29648,-13951,
28713,-15785,28713,-15785,28713,-15785,28713,-15785,
27666,-17557,27666,-17557,27666,-17557,27666,-17557,
26509,-19259,26509,-19259,26509,-19259,26509,-19259,
25247,-20886,25247,-20886,25247,-20886,25247,-20886,
23886,-22430,23886,-22430,23886,-22430,23886,-22430,
22430,-23886,22430,-23886,22430,-23886,22430,-23886,
20886,-25247,20886,-25247,20886,-25247,20886,-25247,
19259,-26509,19259,-26509,19259,-26509,19259,-26509,
17557,-27666,17557,-27666,17557,-27666,17557,-27666,
15785,-28713,15785,-28713,15785,-28713,15785,-28713,
13951,-29648,13951,-29648,13951,-29648,13951,-29648,
12062,-30465,12062,-30465,12062,-30465,12062,-30465,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
8148,-31737,8148,-31737,8148,-31737,8148,-31737,
6139,-32186,6139,-32186,6139,-32186,6139,-32186,
4106,-32508,4106,-32508,4106,-32508,4106,-32508,
2057,-32702,2057,-32702,2057,-32702,2057,-32702,
0,-32767,0,-32767,0,-32767,0,-32767,
-2057,-32702,-2057,-32702,-2057,-32702,-2057,-32702,
-4106,-32508,-4106,-32508,-4106,-32508,-4106,-32508,
-6139,-32186,-6139,-32186,-6139,-32186,-6139,-32186,
-8148,-31737,-8148,-31737,-8148,-31737,-8148,-31737,
-10125,-31163,-10125,-31163,-10125,-31163,-10125,-31163,
-12062,-30465,-12062,-30465,-12062,-30465,-12062,-30465,
-13951,-29648,-13951,-29648,-13951,-29648,-13951,-29648,
-15785,-28713,-15785,-28713,-15785,-28713,-15785,-28713,
-17557,-27666,-17557,-27666,-17557,-27666,-17557,-27666,
-19259,-26509,-19259,-26509,-19259,-26509,-19259,-26509,
-20886,-25247,-20886,-25247,-20886,-25247,-20886,-25247,
-22430,-23886,-22430,-23886,-22430,-23886,-22430,-23886,
-23886,-22430,-23886,-22430,-23886,-22430,-23886,-22430,
-25247,-20886,-25247,-20886,-25247,-20886,-25247,-20886,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-27666,-17557,-27666,-17557,-27666,-17557,-27666,-17557,
-28713,-15785,-28713,-15785,-28713,-15785,-28713,-15785,
-29648,-13951,-29648,-13951,-29648,-13951,-29648,-13951,
-30465,-12062,-30465,-12062,-30465,-12062,-30465,-12062,
-31163,-10125,-31163,-10125,-31163,-10125,-31163,-10125,
-31737,-8148,-31737,-8148,-31737,-8148,-31737,-8148,
-32186,-6139,-32186,-6139,-32186,-6139,-32186,-6139,
-32508,-4106,-32508,-4106,-32508,-4106,-32508,-4106,
-32702,-2057,-32702,-2057,-32702,-2057,-32702,-2057,
-32767,0,-32767,0,-32767,0,-32767,0,
-32702,2057,-32702,2057,-32702,2057,-32702,2057,
-32508,4106,-32508,4106,-32508,4106,-32508,4106,
-32186,6139,-32186,6139,-32186,6139,-32186,6139,
-31737,8148,-31737,8148,-31737,8148,-31737,8148,
-31163,10125,-31163,10125,-31163,10125,-31163,10125,
-30465,12062,-30465,12062,-30465,12062,-30465,12062,
-29648,13951,-29648,13951,-29648,13951,-29648,13951,
-28713,15785,-28713,15785,-28713,15785,-28713,15785,
-27666,17557,-27666,17557,-27666,17557,-27666,17557};

static int16_t twd300[472]__attribute__((aligned(16))) = {32652,-2741,32652,-2741,32652,-2741,32652,-2741,
32308,-5464,32308,-5464,32308,-5464,32308,-5464,
31737,-8148,31737,-8148,31737,-8148,31737,-8148,
30944,-10775,30944,-10775,30944,-10775,30944,-10775,
29934,-13327,29934,-13327,29934,-13327,29934,-13327,
28713,-15785,28713,-15785,28713,-15785,28713,-15785,
27292,-18132,27292,-18132,27292,-18132,27292,-18132,
25679,-20353,25679,-20353,25679,-20353,25679,-20353,
23886,-22430,23886,-22430,23886,-22430,23886,-22430,
21925,-24350,21925,-24350,21925,-24350,21925,-24350,
19810,-26099,19810,-26099,19810,-26099,19810,-26099,
17557,-27666,17557,-27666,17557,-27666,17557,-27666,
15180,-29038,15180,-29038,15180,-29038,15180,-29038,
12697,-30206,12697,-30206,12697,-30206,12697,-30206,
10125,-31163,10125,-31163,10125,-31163,10125,-31163,
7482,-31901,7482,-31901,7482,-31901,7482,-31901,
4786,-32415,4786,-32415,4786,-32415,4786,-32415,
2057,-32702,2057,-32702,2057,-32702,2057,-32702,
-686,-32759,-686,-32759,-686,-32759,-686,-32759,
-3425,-32587,-3425,-32587,-3425,-32587,-3425,-32587,
-6139,-32186,-6139,-32186,-6139,-32186,-6139,-32186,
-8811,-31559,-8811,-31559,-8811,-31559,-8811,-31559,
-11421,-30711,-11421,-30711,-11421,-30711,-11421,-30711,
-13951,-29648,-13951,-29648,-13951,-29648,-13951,-29648,
-16383,-28377,-16383,-28377,-16383,-28377,-16383,-28377,
-18700,-26906,-18700,-26906,-18700,-26906,-18700,-26906,
-20886,-25247,-20886,-25247,-20886,-25247,-20886,-25247,
-22925,-23411,-22925,-23411,-22925,-23411,-22925,-23411,
-24804,-21410,-24804,-21410,-24804,-21410,-24804,-21410,
-26509,-19259,-26509,-19259,-26509,-19259,-26509,-19259,
-28027,-16974,-28027,-16974,-28027,-16974,-28027,-16974,
-29349,-14569,-29349,-14569,-29349,-14569,-29349,-14569,
-30465,-12062,-30465,-12062,-30465,-12062,-30465,-12062,
-31368,-9470,-31368,-9470,-31368,-9470,-31368,-9470,
-32050,-6812,-32050,-6812,-32050,-6812,-32050,-6812,
-32508,-4106,-32508,-4106,-32508,-4106,-32508,-4106,
-32738,-1372,-32738,-1372,-32738,-1372,-32738,-1372,
-32738,1372,-32738,1372,-32738,1372,-32738,1372,
-32508,4106,-32508,4106,-32508,4106,-32508,4106,
-32050,6812,-32050,6812,-32050,6812,-32050,6812,
-31368,9470,-31368,9470,-31368,9470,-31368,9470,
-30465,12062,-30465,12062,-30465,12062,-30465,12062,
-29349,14569,-29349,14569,-29349,14569,-29349,14569,
-28027,16974,-28027,16974,-28027,16974,-28027,16974,
-26509,19259,-26509,19259,-26509,19259,-26509,19259,
-24804,21410,-24804,21410,-24804,21410,-24804,21410,
-22925,23411,-22925,23411,-22925,23411,-22925,23411,
-20886,25247,-20886,25247,-20886,25247,-20886,25247,
-18700,26906,-18700,26906,-18700,26906,-18700,26906,
-16383,28377,-16383,28377,-16383,28377,-16383,28377,
-13951,29648,-13951,29648,-13951,29648,-13951,29648,
-11421,30711,-11421,30711,-11421,30711,-11421,30711,
-8811,31559,-8811,31559,-8811,31559,-8811,31559,
-6139,32186,-6139,32186,-6139,32186,-6139,32186,
-3425,32587,-3425,32587,-3425,32587,-3425,32587,
-686,32759,-686,32759,-686,32759,-686,32759,
2057,32702,2057,32702,2057,32702,2057,32702,
4786,32415,4786,32415,4786,32415,4786,32415,
7482,31901,7482,31901,7482,31901,7482,31901};

void dft300(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa300[0];
  __m128i *twb128=(__m128i *)&twb300[0];
  __m128i *twc128=(__m128i *)&twc300[0];
  __m128i *twd128=(__m128i *)&twd300[0];
  __m128i x2128[300];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[300];//=&ytmp128array2[0];



  for (i=0,j=0;i<60;i++,j+=5) {
    x2128[i]    = x128[j];
    x2128[i+60] = x128[j+1];
    x2128[i+120] = x128[j+2];
    x2128[i+180] = x128[j+3];
    x2128[i+240] = x128[j+4];
  }

  dft60((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft60((int16_t *)(x2128+60),(int16_t *)(ytmp128+60),1);
  dft60((int16_t *)(x2128+120),(int16_t *)(ytmp128+120),1);
  dft60((int16_t *)(x2128+180),(int16_t *)(ytmp128+180),1);
  dft60((int16_t *)(x2128+240),(int16_t *)(ytmp128+240),1);

  bfly5_tw1(ytmp128,ytmp128+60,ytmp128+120,ytmp128+180,ytmp128+240,y128,y128+60,y128+120,y128+180,y128+240);
  for (i=1,j=0;i<60;i++,j++) {
    bfly5(ytmp128+i,
	  ytmp128+60+i,
	  ytmp128+120+i,
	  ytmp128+180+i,
	  ytmp128+240+i,
	  y128+i,
	  y128+60+i,
	  y128+120+i,
	  y128+180+i,
	  y128+240+i,
	  twa128+j,
	  twb128+j,
	  twc128+j,
	  twd128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(dft_norm_table[15]);
    
    for (i=0;i<300;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}

int16_t twa1200[4784] = { 32766,-172,32766,-172,32766,-172,32766,-172,
32765,-344,32765,-344,32765,-344,32765,-344,
32762,-515,32762,-515,32762,-515,32762,-515,
32759,-687,32759,-687,32759,-687,32759,-687,
32755,-858,32755,-858,32755,-858,32755,-858,
32750,-1030,32750,-1030,32750,-1030,32750,-1030,
32744,-1201,32744,-1201,32744,-1201,32744,-1201,
32738,-1373,32738,-1373,32738,-1373,32738,-1373,
32730,-1544,32730,-1544,32730,-1544,32730,-1544,
32722,-1715,32722,-1715,32722,-1715,32722,-1715,
32712,-1887,32712,-1887,32712,-1887,32712,-1887,
32702,-2058,32702,-2058,32702,-2058,32702,-2058,
32691,-2229,32691,-2229,32691,-2229,32691,-2229,
32679,-2400,32679,-2400,32679,-2400,32679,-2400,
32665,-2571,32665,-2571,32665,-2571,32665,-2571,
32652,-2742,32652,-2742,32652,-2742,32652,-2742,
32637,-2913,32637,-2913,32637,-2913,32637,-2913,
32621,-3084,32621,-3084,32621,-3084,32621,-3084,
32604,-3255,32604,-3255,32604,-3255,32604,-3255,
32587,-3426,32587,-3426,32587,-3426,32587,-3426,
32569,-3596,32569,-3596,32569,-3596,32569,-3596,
32549,-3767,32549,-3767,32549,-3767,32549,-3767,
32529,-3937,32529,-3937,32529,-3937,32529,-3937,
32508,-4107,32508,-4107,32508,-4107,32508,-4107,
32486,-4277,32486,-4277,32486,-4277,32486,-4277,
32463,-4447,32463,-4447,32463,-4447,32463,-4447,
32440,-4617,32440,-4617,32440,-4617,32440,-4617,
32415,-4787,32415,-4787,32415,-4787,32415,-4787,
32389,-4957,32389,-4957,32389,-4957,32389,-4957,
32363,-5126,32363,-5126,32363,-5126,32363,-5126,
32336,-5296,32336,-5296,32336,-5296,32336,-5296,
32308,-5465,32308,-5465,32308,-5465,32308,-5465,
32279,-5634,32279,-5634,32279,-5634,32279,-5634,
32249,-5803,32249,-5803,32249,-5803,32249,-5803,
32218,-5972,32218,-5972,32218,-5972,32218,-5972,
32186,-6140,32186,-6140,32186,-6140,32186,-6140,
32154,-6309,32154,-6309,32154,-6309,32154,-6309,
32120,-6477,32120,-6477,32120,-6477,32120,-6477,
32086,-6645,32086,-6645,32086,-6645,32086,-6645,
32050,-6813,32050,-6813,32050,-6813,32050,-6813,
32014,-6981,32014,-6981,32014,-6981,32014,-6981,
31977,-7148,31977,-7148,31977,-7148,31977,-7148,
31940,-7316,31940,-7316,31940,-7316,31940,-7316,
31901,-7483,31901,-7483,31901,-7483,31901,-7483,
31861,-7650,31861,-7650,31861,-7650,31861,-7650,
31821,-7817,31821,-7817,31821,-7817,31821,-7817,
31779,-7983,31779,-7983,31779,-7983,31779,-7983,
31737,-8149,31737,-8149,31737,-8149,31737,-8149,
31694,-8315,31694,-8315,31694,-8315,31694,-8315,
31650,-8481,31650,-8481,31650,-8481,31650,-8481,
31605,-8647,31605,-8647,31605,-8647,31605,-8647,
31559,-8812,31559,-8812,31559,-8812,31559,-8812,
31513,-8977,31513,-8977,31513,-8977,31513,-8977,
31465,-9142,31465,-9142,31465,-9142,31465,-9142,
31417,-9307,31417,-9307,31417,-9307,31417,-9307,
31368,-9471,31368,-9471,31368,-9471,31368,-9471,
31318,-9635,31318,-9635,31318,-9635,31318,-9635,
31267,-9799,31267,-9799,31267,-9799,31267,-9799,
31215,-9963,31215,-9963,31215,-9963,31215,-9963,
31163,-10126,31163,-10126,31163,-10126,31163,-10126,
31109,-10289,31109,-10289,31109,-10289,31109,-10289,
31055,-10452,31055,-10452,31055,-10452,31055,-10452,
31000,-10614,31000,-10614,31000,-10614,31000,-10614,
30944,-10776,30944,-10776,30944,-10776,30944,-10776,
30887,-10938,30887,-10938,30887,-10938,30887,-10938,
30829,-11100,30829,-11100,30829,-11100,30829,-11100,
30771,-11261,30771,-11261,30771,-11261,30771,-11261,
30711,-11422,30711,-11422,30711,-11422,30711,-11422,
30651,-11583,30651,-11583,30651,-11583,30651,-11583,
30590,-11743,30590,-11743,30590,-11743,30590,-11743,
30528,-11903,30528,-11903,30528,-11903,30528,-11903,
30465,-12063,30465,-12063,30465,-12063,30465,-12063,
30402,-12222,30402,-12222,30402,-12222,30402,-12222,
30338,-12381,30338,-12381,30338,-12381,30338,-12381,
30272,-12540,30272,-12540,30272,-12540,30272,-12540,
30206,-12698,30206,-12698,30206,-12698,30206,-12698,
30139,-12856,30139,-12856,30139,-12856,30139,-12856,
30072,-13014,30072,-13014,30072,-13014,30072,-13014,
30003,-13171,30003,-13171,30003,-13171,30003,-13171,
29934,-13328,29934,-13328,29934,-13328,29934,-13328,
29863,-13485,29863,-13485,29863,-13485,29863,-13485,
29792,-13641,29792,-13641,29792,-13641,29792,-13641,
29721,-13797,29721,-13797,29721,-13797,29721,-13797,
29648,-13952,29648,-13952,29648,-13952,29648,-13952,
29575,-14107,29575,-14107,29575,-14107,29575,-14107,
29500,-14262,29500,-14262,29500,-14262,29500,-14262,
29425,-14416,29425,-14416,29425,-14416,29425,-14416,
29349,-14570,29349,-14570,29349,-14570,29349,-14570,
29273,-14723,29273,-14723,29273,-14723,29273,-14723,
29195,-14876,29195,-14876,29195,-14876,29195,-14876,
29117,-15029,29117,-15029,29117,-15029,29117,-15029,
29038,-15181,29038,-15181,29038,-15181,29038,-15181,
28958,-15333,28958,-15333,28958,-15333,28958,-15333,
28877,-15485,28877,-15485,28877,-15485,28877,-15485,
28796,-15636,28796,-15636,28796,-15636,28796,-15636,
28713,-15786,28713,-15786,28713,-15786,28713,-15786,
28630,-15936,28630,-15936,28630,-15936,28630,-15936,
28547,-16086,28547,-16086,28547,-16086,28547,-16086,
28462,-16235,28462,-16235,28462,-16235,28462,-16235,
28377,-16384,28377,-16384,28377,-16384,28377,-16384,
28290,-16532,28290,-16532,28290,-16532,28290,-16532,
28203,-16680,28203,-16680,28203,-16680,28203,-16680,
28116,-16828,28116,-16828,28116,-16828,28116,-16828,
28027,-16975,28027,-16975,28027,-16975,28027,-16975,
27938,-17121,27938,-17121,27938,-17121,27938,-17121,
27848,-17267,27848,-17267,27848,-17267,27848,-17267,
27757,-17413,27757,-17413,27757,-17413,27757,-17413,
27666,-17558,27666,-17558,27666,-17558,27666,-17558,
27573,-17703,27573,-17703,27573,-17703,27573,-17703,
27480,-17847,27480,-17847,27480,-17847,27480,-17847,
27386,-17990,27386,-17990,27386,-17990,27386,-17990,
27292,-18133,27292,-18133,27292,-18133,27292,-18133,
27197,-18276,27197,-18276,27197,-18276,27197,-18276,
27100,-18418,27100,-18418,27100,-18418,27100,-18418,
27004,-18560,27004,-18560,27004,-18560,27004,-18560,
26906,-18701,26906,-18701,26906,-18701,26906,-18701,
26808,-18842,26808,-18842,26808,-18842,26808,-18842,
26709,-18982,26709,-18982,26709,-18982,26709,-18982,
26609,-19121,26609,-19121,26609,-19121,26609,-19121,
26509,-19260,26509,-19260,26509,-19260,26509,-19260,
26407,-19399,26407,-19399,26407,-19399,26407,-19399,
26305,-19537,26305,-19537,26305,-19537,26305,-19537,
26203,-19674,26203,-19674,26203,-19674,26203,-19674,
26099,-19811,26099,-19811,26099,-19811,26099,-19811,
25995,-19948,25995,-19948,25995,-19948,25995,-19948,
25891,-20084,25891,-20084,25891,-20084,25891,-20084,
25785,-20219,25785,-20219,25785,-20219,25785,-20219,
25679,-20354,25679,-20354,25679,-20354,25679,-20354,
25572,-20488,25572,-20488,25572,-20488,25572,-20488,
25464,-20621,25464,-20621,25464,-20621,25464,-20621,
25356,-20754,25356,-20754,25356,-20754,25356,-20754,
25247,-20887,25247,-20887,25247,-20887,25247,-20887,
25137,-21019,25137,-21019,25137,-21019,25137,-21019,
25027,-21150,25027,-21150,25027,-21150,25027,-21150,
24916,-21281,24916,-21281,24916,-21281,24916,-21281,
24804,-21411,24804,-21411,24804,-21411,24804,-21411,
24692,-21541,24692,-21541,24692,-21541,24692,-21541,
24578,-21670,24578,-21670,24578,-21670,24578,-21670,
24465,-21798,24465,-21798,24465,-21798,24465,-21798,
24350,-21926,24350,-21926,24350,-21926,24350,-21926,
24235,-22053,24235,-22053,24235,-22053,24235,-22053,
24119,-22180,24119,-22180,24119,-22180,24119,-22180,
24003,-22306,24003,-22306,24003,-22306,24003,-22306,
23886,-22431,23886,-22431,23886,-22431,23886,-22431,
23768,-22556,23768,-22556,23768,-22556,23768,-22556,
23649,-22680,23649,-22680,23649,-22680,23649,-22680,
23530,-22803,23530,-22803,23530,-22803,23530,-22803,
23411,-22926,23411,-22926,23411,-22926,23411,-22926,
23290,-23049,23290,-23049,23290,-23049,23290,-23049,
23169,-23170,23169,-23170,23169,-23170,23169,-23170,
23048,-23291,23048,-23291,23048,-23291,23048,-23291,
22925,-23412,22925,-23412,22925,-23412,22925,-23412,
22802,-23531,22802,-23531,22802,-23531,22802,-23531,
22679,-23650,22679,-23650,22679,-23650,22679,-23650,
22555,-23769,22555,-23769,22555,-23769,22555,-23769,
22430,-23887,22430,-23887,22430,-23887,22430,-23887,
22305,-24004,22305,-24004,22305,-24004,22305,-24004,
22179,-24120,22179,-24120,22179,-24120,22179,-24120,
22052,-24236,22052,-24236,22052,-24236,22052,-24236,
21925,-24351,21925,-24351,21925,-24351,21925,-24351,
21797,-24466,21797,-24466,21797,-24466,21797,-24466,
21669,-24579,21669,-24579,21669,-24579,21669,-24579,
21540,-24693,21540,-24693,21540,-24693,21540,-24693,
21410,-24805,21410,-24805,21410,-24805,21410,-24805,
21280,-24917,21280,-24917,21280,-24917,21280,-24917,
21149,-25028,21149,-25028,21149,-25028,21149,-25028,
21018,-25138,21018,-25138,21018,-25138,21018,-25138,
20886,-25248,20886,-25248,20886,-25248,20886,-25248,
20753,-25357,20753,-25357,20753,-25357,20753,-25357,
20620,-25465,20620,-25465,20620,-25465,20620,-25465,
20487,-25573,20487,-25573,20487,-25573,20487,-25573,
20353,-25680,20353,-25680,20353,-25680,20353,-25680,
20218,-25786,20218,-25786,20218,-25786,20218,-25786,
20083,-25892,20083,-25892,20083,-25892,20083,-25892,
19947,-25996,19947,-25996,19947,-25996,19947,-25996,
19810,-26100,19810,-26100,19810,-26100,19810,-26100,
19673,-26204,19673,-26204,19673,-26204,19673,-26204,
19536,-26306,19536,-26306,19536,-26306,19536,-26306,
19398,-26408,19398,-26408,19398,-26408,19398,-26408,
19259,-26510,19259,-26510,19259,-26510,19259,-26510,
19120,-26610,19120,-26610,19120,-26610,19120,-26610,
18981,-26710,18981,-26710,18981,-26710,18981,-26710,
18841,-26809,18841,-26809,18841,-26809,18841,-26809,
18700,-26907,18700,-26907,18700,-26907,18700,-26907,
18559,-27005,18559,-27005,18559,-27005,18559,-27005,
18417,-27101,18417,-27101,18417,-27101,18417,-27101,
18275,-27198,18275,-27198,18275,-27198,18275,-27198,
18132,-27293,18132,-27293,18132,-27293,18132,-27293,
17989,-27387,17989,-27387,17989,-27387,17989,-27387,
17846,-27481,17846,-27481,17846,-27481,17846,-27481,
17702,-27574,17702,-27574,17702,-27574,17702,-27574,
17557,-27667,17557,-27667,17557,-27667,17557,-27667,
17412,-27758,17412,-27758,17412,-27758,17412,-27758,
17266,-27849,17266,-27849,17266,-27849,17266,-27849,
17120,-27939,17120,-27939,17120,-27939,17120,-27939,
16974,-28028,16974,-28028,16974,-28028,16974,-28028,
16827,-28117,16827,-28117,16827,-28117,16827,-28117,
16679,-28204,16679,-28204,16679,-28204,16679,-28204,
16531,-28291,16531,-28291,16531,-28291,16531,-28291,
16383,-28378,16383,-28378,16383,-28378,16383,-28378,
16234,-28463,16234,-28463,16234,-28463,16234,-28463,
16085,-28548,16085,-28548,16085,-28548,16085,-28548,
15935,-28631,15935,-28631,15935,-28631,15935,-28631,
15785,-28714,15785,-28714,15785,-28714,15785,-28714,
15635,-28797,15635,-28797,15635,-28797,15635,-28797,
15484,-28878,15484,-28878,15484,-28878,15484,-28878,
15332,-28959,15332,-28959,15332,-28959,15332,-28959,
15180,-29039,15180,-29039,15180,-29039,15180,-29039,
15028,-29118,15028,-29118,15028,-29118,15028,-29118,
14875,-29196,14875,-29196,14875,-29196,14875,-29196,
14722,-29274,14722,-29274,14722,-29274,14722,-29274,
14569,-29350,14569,-29350,14569,-29350,14569,-29350,
14415,-29426,14415,-29426,14415,-29426,14415,-29426,
14261,-29501,14261,-29501,14261,-29501,14261,-29501,
14106,-29576,14106,-29576,14106,-29576,14106,-29576,
13951,-29649,13951,-29649,13951,-29649,13951,-29649,
13796,-29722,13796,-29722,13796,-29722,13796,-29722,
13640,-29793,13640,-29793,13640,-29793,13640,-29793,
13484,-29864,13484,-29864,13484,-29864,13484,-29864,
13327,-29935,13327,-29935,13327,-29935,13327,-29935,
13170,-30004,13170,-30004,13170,-30004,13170,-30004,
13013,-30073,13013,-30073,13013,-30073,13013,-30073,
12855,-30140,12855,-30140,12855,-30140,12855,-30140,
12697,-30207,12697,-30207,12697,-30207,12697,-30207,
12539,-30273,12539,-30273,12539,-30273,12539,-30273,
12380,-30339,12380,-30339,12380,-30339,12380,-30339,
12221,-30403,12221,-30403,12221,-30403,12221,-30403,
12062,-30466,12062,-30466,12062,-30466,12062,-30466,
11902,-30529,11902,-30529,11902,-30529,11902,-30529,
11742,-30591,11742,-30591,11742,-30591,11742,-30591,
11582,-30652,11582,-30652,11582,-30652,11582,-30652,
11421,-30712,11421,-30712,11421,-30712,11421,-30712,
11260,-30772,11260,-30772,11260,-30772,11260,-30772,
11099,-30830,11099,-30830,11099,-30830,11099,-30830,
10937,-30888,10937,-30888,10937,-30888,10937,-30888,
10775,-30945,10775,-30945,10775,-30945,10775,-30945,
10613,-31001,10613,-31001,10613,-31001,10613,-31001,
10451,-31056,10451,-31056,10451,-31056,10451,-31056,
10288,-31110,10288,-31110,10288,-31110,10288,-31110,
10125,-31164,10125,-31164,10125,-31164,10125,-31164,
9962,-31216,9962,-31216,9962,-31216,9962,-31216,
9798,-31268,9798,-31268,9798,-31268,9798,-31268,
9634,-31319,9634,-31319,9634,-31319,9634,-31319,
9470,-31369,9470,-31369,9470,-31369,9470,-31369,
9306,-31418,9306,-31418,9306,-31418,9306,-31418,
9141,-31466,9141,-31466,9141,-31466,9141,-31466,
8976,-31514,8976,-31514,8976,-31514,8976,-31514,
8811,-31560,8811,-31560,8811,-31560,8811,-31560,
8646,-31606,8646,-31606,8646,-31606,8646,-31606,
8480,-31651,8480,-31651,8480,-31651,8480,-31651,
8314,-31695,8314,-31695,8314,-31695,8314,-31695,
8148,-31738,8148,-31738,8148,-31738,8148,-31738,
7982,-31780,7982,-31780,7982,-31780,7982,-31780,
7816,-31822,7816,-31822,7816,-31822,7816,-31822,
7649,-31862,7649,-31862,7649,-31862,7649,-31862,
7482,-31902,7482,-31902,7482,-31902,7482,-31902,
7315,-31941,7315,-31941,7315,-31941,7315,-31941,
7147,-31978,7147,-31978,7147,-31978,7147,-31978,
6980,-32015,6980,-32015,6980,-32015,6980,-32015,
6812,-32051,6812,-32051,6812,-32051,6812,-32051,
6644,-32087,6644,-32087,6644,-32087,6644,-32087,
6476,-32121,6476,-32121,6476,-32121,6476,-32121,
6308,-32155,6308,-32155,6308,-32155,6308,-32155,
6139,-32187,6139,-32187,6139,-32187,6139,-32187,
5971,-32219,5971,-32219,5971,-32219,5971,-32219,
5802,-32250,5802,-32250,5802,-32250,5802,-32250,
5633,-32280,5633,-32280,5633,-32280,5633,-32280,
5464,-32309,5464,-32309,5464,-32309,5464,-32309,
5295,-32337,5295,-32337,5295,-32337,5295,-32337,
5125,-32364,5125,-32364,5125,-32364,5125,-32364,
4956,-32390,4956,-32390,4956,-32390,4956,-32390,
4786,-32416,4786,-32416,4786,-32416,4786,-32416,
4616,-32441,4616,-32441,4616,-32441,4616,-32441,
4446,-32464,4446,-32464,4446,-32464,4446,-32464,
4276,-32487,4276,-32487,4276,-32487,4276,-32487,
4106,-32509,4106,-32509,4106,-32509,4106,-32509,
3936,-32530,3936,-32530,3936,-32530,3936,-32530,
3766,-32550,3766,-32550,3766,-32550,3766,-32550,
3595,-32570,3595,-32570,3595,-32570,3595,-32570,
3425,-32588,3425,-32588,3425,-32588,3425,-32588,
3254,-32605,3254,-32605,3254,-32605,3254,-32605,
3083,-32622,3083,-32622,3083,-32622,3083,-32622,
2912,-32638,2912,-32638,2912,-32638,2912,-32638,
2741,-32653,2741,-32653,2741,-32653,2741,-32653,
2570,-32666,2570,-32666,2570,-32666,2570,-32666,
2399,-32680,2399,-32680,2399,-32680,2399,-32680,
2228,-32692,2228,-32692,2228,-32692,2228,-32692,
2057,-32703,2057,-32703,2057,-32703,2057,-32703,
1886,-32713,1886,-32713,1886,-32713,1886,-32713,
1714,-32723,1714,-32723,1714,-32723,1714,-32723,
1543,-32731,1543,-32731,1543,-32731,1543,-32731,
1372,-32739,1372,-32739,1372,-32739,1372,-32739,
1200,-32745,1200,-32745,1200,-32745,1200,-32745,
1029,-32751,1029,-32751,1029,-32751,1029,-32751,
857,-32756,857,-32756,857,-32756,857,-32756,
686,-32760,686,-32760,686,-32760,686,-32760,
514,-32763,514,-32763,514,-32763,514,-32763,
343,-32766,343,-32766,343,-32766,343,-32766,
171,-32767,171,-32767,171,-32767,171,-32767};

int16_t twb1200[4784] = {32765,-344,32765,-344,32765,-344,32765,-344,
32759,-687,32759,-687,32759,-687,32759,-687,
32750,-1030,32750,-1030,32750,-1030,32750,-1030,
32738,-1373,32738,-1373,32738,-1373,32738,-1373,
32722,-1715,32722,-1715,32722,-1715,32722,-1715,
32702,-2058,32702,-2058,32702,-2058,32702,-2058,
32679,-2400,32679,-2400,32679,-2400,32679,-2400,
32652,-2742,32652,-2742,32652,-2742,32652,-2742,
32621,-3084,32621,-3084,32621,-3084,32621,-3084,
32587,-3426,32587,-3426,32587,-3426,32587,-3426,
32549,-3767,32549,-3767,32549,-3767,32549,-3767,
32508,-4107,32508,-4107,32508,-4107,32508,-4107,
32463,-4447,32463,-4447,32463,-4447,32463,-4447,
32415,-4787,32415,-4787,32415,-4787,32415,-4787,
32363,-5126,32363,-5126,32363,-5126,32363,-5126,
32308,-5465,32308,-5465,32308,-5465,32308,-5465,
32249,-5803,32249,-5803,32249,-5803,32249,-5803,
32186,-6140,32186,-6140,32186,-6140,32186,-6140,
32120,-6477,32120,-6477,32120,-6477,32120,-6477,
32050,-6813,32050,-6813,32050,-6813,32050,-6813,
31977,-7148,31977,-7148,31977,-7148,31977,-7148,
31901,-7483,31901,-7483,31901,-7483,31901,-7483,
31821,-7817,31821,-7817,31821,-7817,31821,-7817,
31737,-8149,31737,-8149,31737,-8149,31737,-8149,
31650,-8481,31650,-8481,31650,-8481,31650,-8481,
31559,-8812,31559,-8812,31559,-8812,31559,-8812,
31465,-9142,31465,-9142,31465,-9142,31465,-9142,
31368,-9471,31368,-9471,31368,-9471,31368,-9471,
31267,-9799,31267,-9799,31267,-9799,31267,-9799,
31163,-10126,31163,-10126,31163,-10126,31163,-10126,
31055,-10452,31055,-10452,31055,-10452,31055,-10452,
30944,-10776,30944,-10776,30944,-10776,30944,-10776,
30829,-11100,30829,-11100,30829,-11100,30829,-11100,
30711,-11422,30711,-11422,30711,-11422,30711,-11422,
30590,-11743,30590,-11743,30590,-11743,30590,-11743,
30465,-12063,30465,-12063,30465,-12063,30465,-12063,
30338,-12381,30338,-12381,30338,-12381,30338,-12381,
30206,-12698,30206,-12698,30206,-12698,30206,-12698,
30072,-13014,30072,-13014,30072,-13014,30072,-13014,
29934,-13328,29934,-13328,29934,-13328,29934,-13328,
29792,-13641,29792,-13641,29792,-13641,29792,-13641,
29648,-13952,29648,-13952,29648,-13952,29648,-13952,
29500,-14262,29500,-14262,29500,-14262,29500,-14262,
29349,-14570,29349,-14570,29349,-14570,29349,-14570,
29195,-14876,29195,-14876,29195,-14876,29195,-14876,
29038,-15181,29038,-15181,29038,-15181,29038,-15181,
28877,-15485,28877,-15485,28877,-15485,28877,-15485,
28713,-15786,28713,-15786,28713,-15786,28713,-15786,
28547,-16086,28547,-16086,28547,-16086,28547,-16086,
28377,-16384,28377,-16384,28377,-16384,28377,-16384,
28203,-16680,28203,-16680,28203,-16680,28203,-16680,
28027,-16975,28027,-16975,28027,-16975,28027,-16975,
27848,-17267,27848,-17267,27848,-17267,27848,-17267,
27666,-17558,27666,-17558,27666,-17558,27666,-17558,
27480,-17847,27480,-17847,27480,-17847,27480,-17847,
27292,-18133,27292,-18133,27292,-18133,27292,-18133,
27100,-18418,27100,-18418,27100,-18418,27100,-18418,
26906,-18701,26906,-18701,26906,-18701,26906,-18701,
26709,-18982,26709,-18982,26709,-18982,26709,-18982,
26509,-19260,26509,-19260,26509,-19260,26509,-19260,
26305,-19537,26305,-19537,26305,-19537,26305,-19537,
26099,-19811,26099,-19811,26099,-19811,26099,-19811,
25891,-20084,25891,-20084,25891,-20084,25891,-20084,
25679,-20354,25679,-20354,25679,-20354,25679,-20354,
25464,-20621,25464,-20621,25464,-20621,25464,-20621,
25247,-20887,25247,-20887,25247,-20887,25247,-20887,
25027,-21150,25027,-21150,25027,-21150,25027,-21150,
24804,-21411,24804,-21411,24804,-21411,24804,-21411,
24578,-21670,24578,-21670,24578,-21670,24578,-21670,
24350,-21926,24350,-21926,24350,-21926,24350,-21926,
24119,-22180,24119,-22180,24119,-22180,24119,-22180,
23886,-22431,23886,-22431,23886,-22431,23886,-22431,
23649,-22680,23649,-22680,23649,-22680,23649,-22680,
23411,-22926,23411,-22926,23411,-22926,23411,-22926,
23169,-23170,23169,-23170,23169,-23170,23169,-23170,
22925,-23412,22925,-23412,22925,-23412,22925,-23412,
22679,-23650,22679,-23650,22679,-23650,22679,-23650,
22430,-23887,22430,-23887,22430,-23887,22430,-23887,
22179,-24120,22179,-24120,22179,-24120,22179,-24120,
21925,-24351,21925,-24351,21925,-24351,21925,-24351,
21669,-24579,21669,-24579,21669,-24579,21669,-24579,
21410,-24805,21410,-24805,21410,-24805,21410,-24805,
21149,-25028,21149,-25028,21149,-25028,21149,-25028,
20886,-25248,20886,-25248,20886,-25248,20886,-25248,
20620,-25465,20620,-25465,20620,-25465,20620,-25465,
20353,-25680,20353,-25680,20353,-25680,20353,-25680,
20083,-25892,20083,-25892,20083,-25892,20083,-25892,
19810,-26100,19810,-26100,19810,-26100,19810,-26100,
19536,-26306,19536,-26306,19536,-26306,19536,-26306,
19259,-26510,19259,-26510,19259,-26510,19259,-26510,
18981,-26710,18981,-26710,18981,-26710,18981,-26710,
18700,-26907,18700,-26907,18700,-26907,18700,-26907,
18417,-27101,18417,-27101,18417,-27101,18417,-27101,
18132,-27293,18132,-27293,18132,-27293,18132,-27293,
17846,-27481,17846,-27481,17846,-27481,17846,-27481,
17557,-27667,17557,-27667,17557,-27667,17557,-27667,
17266,-27849,17266,-27849,17266,-27849,17266,-27849,
16974,-28028,16974,-28028,16974,-28028,16974,-28028,
16679,-28204,16679,-28204,16679,-28204,16679,-28204,
16383,-28378,16383,-28378,16383,-28378,16383,-28378,
16085,-28548,16085,-28548,16085,-28548,16085,-28548,
15785,-28714,15785,-28714,15785,-28714,15785,-28714,
15484,-28878,15484,-28878,15484,-28878,15484,-28878,
15180,-29039,15180,-29039,15180,-29039,15180,-29039,
14875,-29196,14875,-29196,14875,-29196,14875,-29196,
14569,-29350,14569,-29350,14569,-29350,14569,-29350,
14261,-29501,14261,-29501,14261,-29501,14261,-29501,
13951,-29649,13951,-29649,13951,-29649,13951,-29649,
13640,-29793,13640,-29793,13640,-29793,13640,-29793,
13327,-29935,13327,-29935,13327,-29935,13327,-29935,
13013,-30073,13013,-30073,13013,-30073,13013,-30073,
12697,-30207,12697,-30207,12697,-30207,12697,-30207,
12380,-30339,12380,-30339,12380,-30339,12380,-30339,
12062,-30466,12062,-30466,12062,-30466,12062,-30466,
11742,-30591,11742,-30591,11742,-30591,11742,-30591,
11421,-30712,11421,-30712,11421,-30712,11421,-30712,
11099,-30830,11099,-30830,11099,-30830,11099,-30830,
10775,-30945,10775,-30945,10775,-30945,10775,-30945,
10451,-31056,10451,-31056,10451,-31056,10451,-31056,
10125,-31164,10125,-31164,10125,-31164,10125,-31164,
9798,-31268,9798,-31268,9798,-31268,9798,-31268,
9470,-31369,9470,-31369,9470,-31369,9470,-31369,
9141,-31466,9141,-31466,9141,-31466,9141,-31466,
8811,-31560,8811,-31560,8811,-31560,8811,-31560,
8480,-31651,8480,-31651,8480,-31651,8480,-31651,
8148,-31738,8148,-31738,8148,-31738,8148,-31738,
7816,-31822,7816,-31822,7816,-31822,7816,-31822,
7482,-31902,7482,-31902,7482,-31902,7482,-31902,
7147,-31978,7147,-31978,7147,-31978,7147,-31978,
6812,-32051,6812,-32051,6812,-32051,6812,-32051,
6476,-32121,6476,-32121,6476,-32121,6476,-32121,
6139,-32187,6139,-32187,6139,-32187,6139,-32187,
5802,-32250,5802,-32250,5802,-32250,5802,-32250,
5464,-32309,5464,-32309,5464,-32309,5464,-32309,
5125,-32364,5125,-32364,5125,-32364,5125,-32364,
4786,-32416,4786,-32416,4786,-32416,4786,-32416,
4446,-32464,4446,-32464,4446,-32464,4446,-32464,
4106,-32509,4106,-32509,4106,-32509,4106,-32509,
3766,-32550,3766,-32550,3766,-32550,3766,-32550,
3425,-32588,3425,-32588,3425,-32588,3425,-32588,
3083,-32622,3083,-32622,3083,-32622,3083,-32622,
2741,-32653,2741,-32653,2741,-32653,2741,-32653,
2399,-32680,2399,-32680,2399,-32680,2399,-32680,
2057,-32703,2057,-32703,2057,-32703,2057,-32703,
1714,-32723,1714,-32723,1714,-32723,1714,-32723,
1372,-32739,1372,-32739,1372,-32739,1372,-32739,
1029,-32751,1029,-32751,1029,-32751,1029,-32751,
686,-32760,686,-32760,686,-32760,686,-32760,
343,-32766,343,-32766,343,-32766,343,-32766,
0,-32767,0,-32767,0,-32767,0,-32767,
-344,-32766,-344,-32766,-344,-32766,-344,-32766,
-687,-32760,-687,-32760,-687,-32760,-687,-32760,
-1030,-32751,-1030,-32751,-1030,-32751,-1030,-32751,
-1373,-32739,-1373,-32739,-1373,-32739,-1373,-32739,
-1715,-32723,-1715,-32723,-1715,-32723,-1715,-32723,
-2058,-32703,-2058,-32703,-2058,-32703,-2058,-32703,
-2400,-32680,-2400,-32680,-2400,-32680,-2400,-32680,
-2742,-32653,-2742,-32653,-2742,-32653,-2742,-32653,
-3084,-32622,-3084,-32622,-3084,-32622,-3084,-32622,
-3426,-32588,-3426,-32588,-3426,-32588,-3426,-32588,
-3767,-32550,-3767,-32550,-3767,-32550,-3767,-32550,
-4107,-32509,-4107,-32509,-4107,-32509,-4107,-32509,
-4447,-32464,-4447,-32464,-4447,-32464,-4447,-32464,
-4787,-32416,-4787,-32416,-4787,-32416,-4787,-32416,
-5126,-32364,-5126,-32364,-5126,-32364,-5126,-32364,
-5465,-32309,-5465,-32309,-5465,-32309,-5465,-32309,
-5803,-32250,-5803,-32250,-5803,-32250,-5803,-32250,
-6140,-32187,-6140,-32187,-6140,-32187,-6140,-32187,
-6477,-32121,-6477,-32121,-6477,-32121,-6477,-32121,
-6813,-32051,-6813,-32051,-6813,-32051,-6813,-32051,
-7148,-31978,-7148,-31978,-7148,-31978,-7148,-31978,
-7483,-31902,-7483,-31902,-7483,-31902,-7483,-31902,
-7817,-31822,-7817,-31822,-7817,-31822,-7817,-31822,
-8149,-31738,-8149,-31738,-8149,-31738,-8149,-31738,
-8481,-31651,-8481,-31651,-8481,-31651,-8481,-31651,
-8812,-31560,-8812,-31560,-8812,-31560,-8812,-31560,
-9142,-31466,-9142,-31466,-9142,-31466,-9142,-31466,
-9471,-31369,-9471,-31369,-9471,-31369,-9471,-31369,
-9799,-31268,-9799,-31268,-9799,-31268,-9799,-31268,
-10126,-31164,-10126,-31164,-10126,-31164,-10126,-31164,
-10452,-31056,-10452,-31056,-10452,-31056,-10452,-31056,
-10776,-30945,-10776,-30945,-10776,-30945,-10776,-30945,
-11100,-30830,-11100,-30830,-11100,-30830,-11100,-30830,
-11422,-30712,-11422,-30712,-11422,-30712,-11422,-30712,
-11743,-30591,-11743,-30591,-11743,-30591,-11743,-30591,
-12063,-30466,-12063,-30466,-12063,-30466,-12063,-30466,
-12381,-30339,-12381,-30339,-12381,-30339,-12381,-30339,
-12698,-30207,-12698,-30207,-12698,-30207,-12698,-30207,
-13014,-30073,-13014,-30073,-13014,-30073,-13014,-30073,
-13328,-29935,-13328,-29935,-13328,-29935,-13328,-29935,
-13641,-29793,-13641,-29793,-13641,-29793,-13641,-29793,
-13952,-29649,-13952,-29649,-13952,-29649,-13952,-29649,
-14262,-29501,-14262,-29501,-14262,-29501,-14262,-29501,
-14570,-29350,-14570,-29350,-14570,-29350,-14570,-29350,
-14876,-29196,-14876,-29196,-14876,-29196,-14876,-29196,
-15181,-29039,-15181,-29039,-15181,-29039,-15181,-29039,
-15485,-28878,-15485,-28878,-15485,-28878,-15485,-28878,
-15786,-28714,-15786,-28714,-15786,-28714,-15786,-28714,
-16086,-28548,-16086,-28548,-16086,-28548,-16086,-28548,
-16384,-28378,-16384,-28378,-16384,-28378,-16384,-28378,
-16680,-28204,-16680,-28204,-16680,-28204,-16680,-28204,
-16975,-28028,-16975,-28028,-16975,-28028,-16975,-28028,
-17267,-27849,-17267,-27849,-17267,-27849,-17267,-27849,
-17558,-27667,-17558,-27667,-17558,-27667,-17558,-27667,
-17847,-27481,-17847,-27481,-17847,-27481,-17847,-27481,
-18133,-27293,-18133,-27293,-18133,-27293,-18133,-27293,
-18418,-27101,-18418,-27101,-18418,-27101,-18418,-27101,
-18701,-26907,-18701,-26907,-18701,-26907,-18701,-26907,
-18982,-26710,-18982,-26710,-18982,-26710,-18982,-26710,
-19260,-26510,-19260,-26510,-19260,-26510,-19260,-26510,
-19537,-26306,-19537,-26306,-19537,-26306,-19537,-26306,
-19811,-26100,-19811,-26100,-19811,-26100,-19811,-26100,
-20084,-25892,-20084,-25892,-20084,-25892,-20084,-25892,
-20354,-25680,-20354,-25680,-20354,-25680,-20354,-25680,
-20621,-25465,-20621,-25465,-20621,-25465,-20621,-25465,
-20887,-25248,-20887,-25248,-20887,-25248,-20887,-25248,
-21150,-25028,-21150,-25028,-21150,-25028,-21150,-25028,
-21411,-24805,-21411,-24805,-21411,-24805,-21411,-24805,
-21670,-24579,-21670,-24579,-21670,-24579,-21670,-24579,
-21926,-24351,-21926,-24351,-21926,-24351,-21926,-24351,
-22180,-24120,-22180,-24120,-22180,-24120,-22180,-24120,
-22431,-23887,-22431,-23887,-22431,-23887,-22431,-23887,
-22680,-23650,-22680,-23650,-22680,-23650,-22680,-23650,
-22926,-23412,-22926,-23412,-22926,-23412,-22926,-23412,
-23170,-23170,-23170,-23170,-23170,-23170,-23170,-23170,
-23412,-22926,-23412,-22926,-23412,-22926,-23412,-22926,
-23650,-22680,-23650,-22680,-23650,-22680,-23650,-22680,
-23887,-22431,-23887,-22431,-23887,-22431,-23887,-22431,
-24120,-22180,-24120,-22180,-24120,-22180,-24120,-22180,
-24351,-21926,-24351,-21926,-24351,-21926,-24351,-21926,
-24579,-21670,-24579,-21670,-24579,-21670,-24579,-21670,
-24805,-21411,-24805,-21411,-24805,-21411,-24805,-21411,
-25028,-21150,-25028,-21150,-25028,-21150,-25028,-21150,
-25248,-20887,-25248,-20887,-25248,-20887,-25248,-20887,
-25465,-20621,-25465,-20621,-25465,-20621,-25465,-20621,
-25680,-20354,-25680,-20354,-25680,-20354,-25680,-20354,
-25892,-20084,-25892,-20084,-25892,-20084,-25892,-20084,
-26100,-19811,-26100,-19811,-26100,-19811,-26100,-19811,
-26306,-19537,-26306,-19537,-26306,-19537,-26306,-19537,
-26510,-19260,-26510,-19260,-26510,-19260,-26510,-19260,
-26710,-18982,-26710,-18982,-26710,-18982,-26710,-18982,
-26907,-18701,-26907,-18701,-26907,-18701,-26907,-18701,
-27101,-18418,-27101,-18418,-27101,-18418,-27101,-18418,
-27293,-18133,-27293,-18133,-27293,-18133,-27293,-18133,
-27481,-17847,-27481,-17847,-27481,-17847,-27481,-17847,
-27667,-17558,-27667,-17558,-27667,-17558,-27667,-17558,
-27849,-17267,-27849,-17267,-27849,-17267,-27849,-17267,
-28028,-16975,-28028,-16975,-28028,-16975,-28028,-16975,
-28204,-16680,-28204,-16680,-28204,-16680,-28204,-16680,
-28378,-16384,-28378,-16384,-28378,-16384,-28378,-16384,
-28548,-16086,-28548,-16086,-28548,-16086,-28548,-16086,
-28714,-15786,-28714,-15786,-28714,-15786,-28714,-15786,
-28878,-15485,-28878,-15485,-28878,-15485,-28878,-15485,
-29039,-15181,-29039,-15181,-29039,-15181,-29039,-15181,
-29196,-14876,-29196,-14876,-29196,-14876,-29196,-14876,
-29350,-14570,-29350,-14570,-29350,-14570,-29350,-14570,
-29501,-14262,-29501,-14262,-29501,-14262,-29501,-14262,
-29649,-13952,-29649,-13952,-29649,-13952,-29649,-13952,
-29793,-13641,-29793,-13641,-29793,-13641,-29793,-13641,
-29935,-13328,-29935,-13328,-29935,-13328,-29935,-13328,
-30073,-13014,-30073,-13014,-30073,-13014,-30073,-13014,
-30207,-12698,-30207,-12698,-30207,-12698,-30207,-12698,
-30339,-12381,-30339,-12381,-30339,-12381,-30339,-12381,
-30466,-12063,-30466,-12063,-30466,-12063,-30466,-12063,
-30591,-11743,-30591,-11743,-30591,-11743,-30591,-11743,
-30712,-11422,-30712,-11422,-30712,-11422,-30712,-11422,
-30830,-11100,-30830,-11100,-30830,-11100,-30830,-11100,
-30945,-10776,-30945,-10776,-30945,-10776,-30945,-10776,
-31056,-10452,-31056,-10452,-31056,-10452,-31056,-10452,
-31164,-10126,-31164,-10126,-31164,-10126,-31164,-10126,
-31268,-9799,-31268,-9799,-31268,-9799,-31268,-9799,
-31369,-9471,-31369,-9471,-31369,-9471,-31369,-9471,
-31466,-9142,-31466,-9142,-31466,-9142,-31466,-9142,
-31560,-8812,-31560,-8812,-31560,-8812,-31560,-8812,
-31651,-8481,-31651,-8481,-31651,-8481,-31651,-8481,
-31738,-8149,-31738,-8149,-31738,-8149,-31738,-8149,
-31822,-7817,-31822,-7817,-31822,-7817,-31822,-7817,
-31902,-7483,-31902,-7483,-31902,-7483,-31902,-7483,
-31978,-7148,-31978,-7148,-31978,-7148,-31978,-7148,
-32051,-6813,-32051,-6813,-32051,-6813,-32051,-6813,
-32121,-6477,-32121,-6477,-32121,-6477,-32121,-6477,
-32187,-6140,-32187,-6140,-32187,-6140,-32187,-6140,
-32250,-5803,-32250,-5803,-32250,-5803,-32250,-5803,
-32309,-5465,-32309,-5465,-32309,-5465,-32309,-5465,
-32364,-5126,-32364,-5126,-32364,-5126,-32364,-5126,
-32416,-4787,-32416,-4787,-32416,-4787,-32416,-4787,
-32464,-4447,-32464,-4447,-32464,-4447,-32464,-4447,
-32509,-4107,-32509,-4107,-32509,-4107,-32509,-4107,
-32550,-3767,-32550,-3767,-32550,-3767,-32550,-3767,
-32588,-3426,-32588,-3426,-32588,-3426,-32588,-3426,
-32622,-3084,-32622,-3084,-32622,-3084,-32622,-3084,
-32653,-2742,-32653,-2742,-32653,-2742,-32653,-2742,
-32680,-2400,-32680,-2400,-32680,-2400,-32680,-2400,
-32703,-2058,-32703,-2058,-32703,-2058,-32703,-2058,
-32723,-1715,-32723,-1715,-32723,-1715,-32723,-1715,
-32739,-1373,-32739,-1373,-32739,-1373,-32739,-1373,
-32751,-1030,-32751,-1030,-32751,-1030,-32751,-1030,
-32760,-687,-32760,-687,-32760,-687,-32760,-687,
-32766,-344,-32766,-344,-32766,-344,-32766,-344};
 
int16_t twc1200[4784] = { 32762,-515,32762,-515,32762,-515,32762,-515,
32750,-1030,32750,-1030,32750,-1030,32750,-1030,
32730,-1544,32730,-1544,32730,-1544,32730,-1544,
32702,-2058,32702,-2058,32702,-2058,32702,-2058,
32665,-2571,32665,-2571,32665,-2571,32665,-2571,
32621,-3084,32621,-3084,32621,-3084,32621,-3084,
32569,-3596,32569,-3596,32569,-3596,32569,-3596,
32508,-4107,32508,-4107,32508,-4107,32508,-4107,
32440,-4617,32440,-4617,32440,-4617,32440,-4617,
32363,-5126,32363,-5126,32363,-5126,32363,-5126,
32279,-5634,32279,-5634,32279,-5634,32279,-5634,
32186,-6140,32186,-6140,32186,-6140,32186,-6140,
32086,-6645,32086,-6645,32086,-6645,32086,-6645,
31977,-7148,31977,-7148,31977,-7148,31977,-7148,
31861,-7650,31861,-7650,31861,-7650,31861,-7650,
31737,-8149,31737,-8149,31737,-8149,31737,-8149,
31605,-8647,31605,-8647,31605,-8647,31605,-8647,
31465,-9142,31465,-9142,31465,-9142,31465,-9142,
31318,-9635,31318,-9635,31318,-9635,31318,-9635,
31163,-10126,31163,-10126,31163,-10126,31163,-10126,
31000,-10614,31000,-10614,31000,-10614,31000,-10614,
30829,-11100,30829,-11100,30829,-11100,30829,-11100,
30651,-11583,30651,-11583,30651,-11583,30651,-11583,
30465,-12063,30465,-12063,30465,-12063,30465,-12063,
30272,-12540,30272,-12540,30272,-12540,30272,-12540,
30072,-13014,30072,-13014,30072,-13014,30072,-13014,
29863,-13485,29863,-13485,29863,-13485,29863,-13485,
29648,-13952,29648,-13952,29648,-13952,29648,-13952,
29425,-14416,29425,-14416,29425,-14416,29425,-14416,
29195,-14876,29195,-14876,29195,-14876,29195,-14876,
28958,-15333,28958,-15333,28958,-15333,28958,-15333,
28713,-15786,28713,-15786,28713,-15786,28713,-15786,
28462,-16235,28462,-16235,28462,-16235,28462,-16235,
28203,-16680,28203,-16680,28203,-16680,28203,-16680,
27938,-17121,27938,-17121,27938,-17121,27938,-17121,
27666,-17558,27666,-17558,27666,-17558,27666,-17558,
27386,-17990,27386,-17990,27386,-17990,27386,-17990,
27100,-18418,27100,-18418,27100,-18418,27100,-18418,
26808,-18842,26808,-18842,26808,-18842,26808,-18842,
26509,-19260,26509,-19260,26509,-19260,26509,-19260,
26203,-19674,26203,-19674,26203,-19674,26203,-19674,
25891,-20084,25891,-20084,25891,-20084,25891,-20084,
25572,-20488,25572,-20488,25572,-20488,25572,-20488,
25247,-20887,25247,-20887,25247,-20887,25247,-20887,
24916,-21281,24916,-21281,24916,-21281,24916,-21281,
24578,-21670,24578,-21670,24578,-21670,24578,-21670,
24235,-22053,24235,-22053,24235,-22053,24235,-22053,
23886,-22431,23886,-22431,23886,-22431,23886,-22431,
23530,-22803,23530,-22803,23530,-22803,23530,-22803,
23169,-23170,23169,-23170,23169,-23170,23169,-23170,
22802,-23531,22802,-23531,22802,-23531,22802,-23531,
22430,-23887,22430,-23887,22430,-23887,22430,-23887,
22052,-24236,22052,-24236,22052,-24236,22052,-24236,
21669,-24579,21669,-24579,21669,-24579,21669,-24579,
21280,-24917,21280,-24917,21280,-24917,21280,-24917,
20886,-25248,20886,-25248,20886,-25248,20886,-25248,
20487,-25573,20487,-25573,20487,-25573,20487,-25573,
20083,-25892,20083,-25892,20083,-25892,20083,-25892,
19673,-26204,19673,-26204,19673,-26204,19673,-26204,
19259,-26510,19259,-26510,19259,-26510,19259,-26510,
18841,-26809,18841,-26809,18841,-26809,18841,-26809,
18417,-27101,18417,-27101,18417,-27101,18417,-27101,
17989,-27387,17989,-27387,17989,-27387,17989,-27387,
17557,-27667,17557,-27667,17557,-27667,17557,-27667,
17120,-27939,17120,-27939,17120,-27939,17120,-27939,
16679,-28204,16679,-28204,16679,-28204,16679,-28204,
16234,-28463,16234,-28463,16234,-28463,16234,-28463,
15785,-28714,15785,-28714,15785,-28714,15785,-28714,
15332,-28959,15332,-28959,15332,-28959,15332,-28959,
14875,-29196,14875,-29196,14875,-29196,14875,-29196,
14415,-29426,14415,-29426,14415,-29426,14415,-29426,
13951,-29649,13951,-29649,13951,-29649,13951,-29649,
13484,-29864,13484,-29864,13484,-29864,13484,-29864,
13013,-30073,13013,-30073,13013,-30073,13013,-30073,
12539,-30273,12539,-30273,12539,-30273,12539,-30273,
12062,-30466,12062,-30466,12062,-30466,12062,-30466,
11582,-30652,11582,-30652,11582,-30652,11582,-30652,
11099,-30830,11099,-30830,11099,-30830,11099,-30830,
10613,-31001,10613,-31001,10613,-31001,10613,-31001,
10125,-31164,10125,-31164,10125,-31164,10125,-31164,
9634,-31319,9634,-31319,9634,-31319,9634,-31319,
9141,-31466,9141,-31466,9141,-31466,9141,-31466,
8646,-31606,8646,-31606,8646,-31606,8646,-31606,
8148,-31738,8148,-31738,8148,-31738,8148,-31738,
7649,-31862,7649,-31862,7649,-31862,7649,-31862,
7147,-31978,7147,-31978,7147,-31978,7147,-31978,
6644,-32087,6644,-32087,6644,-32087,6644,-32087,
6139,-32187,6139,-32187,6139,-32187,6139,-32187,
5633,-32280,5633,-32280,5633,-32280,5633,-32280,
5125,-32364,5125,-32364,5125,-32364,5125,-32364,
4616,-32441,4616,-32441,4616,-32441,4616,-32441,
4106,-32509,4106,-32509,4106,-32509,4106,-32509,
3595,-32570,3595,-32570,3595,-32570,3595,-32570,
3083,-32622,3083,-32622,3083,-32622,3083,-32622,
2570,-32666,2570,-32666,2570,-32666,2570,-32666,
2057,-32703,2057,-32703,2057,-32703,2057,-32703,
1543,-32731,1543,-32731,1543,-32731,1543,-32731,
1029,-32751,1029,-32751,1029,-32751,1029,-32751,
514,-32763,514,-32763,514,-32763,514,-32763,
0,-32767,0,-32767,0,-32767,0,-32767,
-515,-32763,-515,-32763,-515,-32763,-515,-32763,
-1030,-32751,-1030,-32751,-1030,-32751,-1030,-32751,
-1544,-32731,-1544,-32731,-1544,-32731,-1544,-32731,
-2058,-32703,-2058,-32703,-2058,-32703,-2058,-32703,
-2571,-32666,-2571,-32666,-2571,-32666,-2571,-32666,
-3084,-32622,-3084,-32622,-3084,-32622,-3084,-32622,
-3596,-32570,-3596,-32570,-3596,-32570,-3596,-32570,
-4107,-32509,-4107,-32509,-4107,-32509,-4107,-32509,
-4617,-32441,-4617,-32441,-4617,-32441,-4617,-32441,
-5126,-32364,-5126,-32364,-5126,-32364,-5126,-32364,
-5634,-32280,-5634,-32280,-5634,-32280,-5634,-32280,
-6140,-32187,-6140,-32187,-6140,-32187,-6140,-32187,
-6645,-32087,-6645,-32087,-6645,-32087,-6645,-32087,
-7148,-31978,-7148,-31978,-7148,-31978,-7148,-31978,
-7650,-31862,-7650,-31862,-7650,-31862,-7650,-31862,
-8149,-31738,-8149,-31738,-8149,-31738,-8149,-31738,
-8647,-31606,-8647,-31606,-8647,-31606,-8647,-31606,
-9142,-31466,-9142,-31466,-9142,-31466,-9142,-31466,
-9635,-31319,-9635,-31319,-9635,-31319,-9635,-31319,
-10126,-31164,-10126,-31164,-10126,-31164,-10126,-31164,
-10614,-31001,-10614,-31001,-10614,-31001,-10614,-31001,
-11100,-30830,-11100,-30830,-11100,-30830,-11100,-30830,
-11583,-30652,-11583,-30652,-11583,-30652,-11583,-30652,
-12063,-30466,-12063,-30466,-12063,-30466,-12063,-30466,
-12540,-30273,-12540,-30273,-12540,-30273,-12540,-30273,
-13014,-30073,-13014,-30073,-13014,-30073,-13014,-30073,
-13485,-29864,-13485,-29864,-13485,-29864,-13485,-29864,
-13952,-29649,-13952,-29649,-13952,-29649,-13952,-29649,
-14416,-29426,-14416,-29426,-14416,-29426,-14416,-29426,
-14876,-29196,-14876,-29196,-14876,-29196,-14876,-29196,
-15333,-28959,-15333,-28959,-15333,-28959,-15333,-28959,
-15786,-28714,-15786,-28714,-15786,-28714,-15786,-28714,
-16235,-28463,-16235,-28463,-16235,-28463,-16235,-28463,
-16680,-28204,-16680,-28204,-16680,-28204,-16680,-28204,
-17121,-27939,-17121,-27939,-17121,-27939,-17121,-27939,
-17558,-27667,-17558,-27667,-17558,-27667,-17558,-27667,
-17990,-27387,-17990,-27387,-17990,-27387,-17990,-27387,
-18418,-27101,-18418,-27101,-18418,-27101,-18418,-27101,
-18842,-26809,-18842,-26809,-18842,-26809,-18842,-26809,
-19260,-26510,-19260,-26510,-19260,-26510,-19260,-26510,
-19674,-26204,-19674,-26204,-19674,-26204,-19674,-26204,
-20084,-25892,-20084,-25892,-20084,-25892,-20084,-25892,
-20488,-25573,-20488,-25573,-20488,-25573,-20488,-25573,
-20887,-25248,-20887,-25248,-20887,-25248,-20887,-25248,
-21281,-24917,-21281,-24917,-21281,-24917,-21281,-24917,
-21670,-24579,-21670,-24579,-21670,-24579,-21670,-24579,
-22053,-24236,-22053,-24236,-22053,-24236,-22053,-24236,
-22431,-23887,-22431,-23887,-22431,-23887,-22431,-23887,
-22803,-23531,-22803,-23531,-22803,-23531,-22803,-23531,
-23170,-23170,-23170,-23170,-23170,-23170,-23170,-23170,
-23531,-22803,-23531,-22803,-23531,-22803,-23531,-22803,
-23887,-22431,-23887,-22431,-23887,-22431,-23887,-22431,
-24236,-22053,-24236,-22053,-24236,-22053,-24236,-22053,
-24579,-21670,-24579,-21670,-24579,-21670,-24579,-21670,
-24917,-21281,-24917,-21281,-24917,-21281,-24917,-21281,
-25248,-20887,-25248,-20887,-25248,-20887,-25248,-20887,
-25573,-20488,-25573,-20488,-25573,-20488,-25573,-20488,
-25892,-20084,-25892,-20084,-25892,-20084,-25892,-20084,
-26204,-19674,-26204,-19674,-26204,-19674,-26204,-19674,
-26510,-19260,-26510,-19260,-26510,-19260,-26510,-19260,
-26809,-18842,-26809,-18842,-26809,-18842,-26809,-18842,
-27101,-18418,-27101,-18418,-27101,-18418,-27101,-18418,
-27387,-17990,-27387,-17990,-27387,-17990,-27387,-17990,
-27667,-17558,-27667,-17558,-27667,-17558,-27667,-17558,
-27939,-17121,-27939,-17121,-27939,-17121,-27939,-17121,
-28204,-16680,-28204,-16680,-28204,-16680,-28204,-16680,
-28463,-16235,-28463,-16235,-28463,-16235,-28463,-16235,
-28714,-15786,-28714,-15786,-28714,-15786,-28714,-15786,
-28959,-15333,-28959,-15333,-28959,-15333,-28959,-15333,
-29196,-14876,-29196,-14876,-29196,-14876,-29196,-14876,
-29426,-14416,-29426,-14416,-29426,-14416,-29426,-14416,
-29649,-13952,-29649,-13952,-29649,-13952,-29649,-13952,
-29864,-13485,-29864,-13485,-29864,-13485,-29864,-13485,
-30073,-13014,-30073,-13014,-30073,-13014,-30073,-13014,
-30273,-12540,-30273,-12540,-30273,-12540,-30273,-12540,
-30466,-12063,-30466,-12063,-30466,-12063,-30466,-12063,
-30652,-11583,-30652,-11583,-30652,-11583,-30652,-11583,
-30830,-11100,-30830,-11100,-30830,-11100,-30830,-11100,
-31001,-10614,-31001,-10614,-31001,-10614,-31001,-10614,
-31164,-10126,-31164,-10126,-31164,-10126,-31164,-10126,
-31319,-9635,-31319,-9635,-31319,-9635,-31319,-9635,
-31466,-9142,-31466,-9142,-31466,-9142,-31466,-9142,
-31606,-8647,-31606,-8647,-31606,-8647,-31606,-8647,
-31738,-8149,-31738,-8149,-31738,-8149,-31738,-8149,
-31862,-7650,-31862,-7650,-31862,-7650,-31862,-7650,
-31978,-7148,-31978,-7148,-31978,-7148,-31978,-7148,
-32087,-6645,-32087,-6645,-32087,-6645,-32087,-6645,
-32187,-6140,-32187,-6140,-32187,-6140,-32187,-6140,
-32280,-5634,-32280,-5634,-32280,-5634,-32280,-5634,
-32364,-5126,-32364,-5126,-32364,-5126,-32364,-5126,
-32441,-4617,-32441,-4617,-32441,-4617,-32441,-4617,
-32509,-4107,-32509,-4107,-32509,-4107,-32509,-4107,
-32570,-3596,-32570,-3596,-32570,-3596,-32570,-3596,
-32622,-3084,-32622,-3084,-32622,-3084,-32622,-3084,
-32666,-2571,-32666,-2571,-32666,-2571,-32666,-2571,
-32703,-2058,-32703,-2058,-32703,-2058,-32703,-2058,
-32731,-1544,-32731,-1544,-32731,-1544,-32731,-1544,
-32751,-1030,-32751,-1030,-32751,-1030,-32751,-1030,
-32763,-515,-32763,-515,-32763,-515,-32763,-515,
-32767,-1,-32767,-1,-32767,-1,-32767,-1,
-32763,514,-32763,514,-32763,514,-32763,514,
-32751,1029,-32751,1029,-32751,1029,-32751,1029,
-32731,1543,-32731,1543,-32731,1543,-32731,1543,
-32703,2057,-32703,2057,-32703,2057,-32703,2057,
-32666,2570,-32666,2570,-32666,2570,-32666,2570,
-32622,3083,-32622,3083,-32622,3083,-32622,3083,
-32570,3595,-32570,3595,-32570,3595,-32570,3595,
-32509,4106,-32509,4106,-32509,4106,-32509,4106,
-32441,4616,-32441,4616,-32441,4616,-32441,4616,
-32364,5125,-32364,5125,-32364,5125,-32364,5125,
-32280,5633,-32280,5633,-32280,5633,-32280,5633,
-32187,6139,-32187,6139,-32187,6139,-32187,6139,
-32087,6644,-32087,6644,-32087,6644,-32087,6644,
-31978,7147,-31978,7147,-31978,7147,-31978,7147,
-31862,7649,-31862,7649,-31862,7649,-31862,7649,
-31738,8148,-31738,8148,-31738,8148,-31738,8148,
-31606,8646,-31606,8646,-31606,8646,-31606,8646,
-31466,9141,-31466,9141,-31466,9141,-31466,9141,
-31319,9634,-31319,9634,-31319,9634,-31319,9634,
-31164,10125,-31164,10125,-31164,10125,-31164,10125,
-31001,10613,-31001,10613,-31001,10613,-31001,10613,
-30830,11099,-30830,11099,-30830,11099,-30830,11099,
-30652,11582,-30652,11582,-30652,11582,-30652,11582,
-30466,12062,-30466,12062,-30466,12062,-30466,12062,
-30273,12539,-30273,12539,-30273,12539,-30273,12539,
-30073,13013,-30073,13013,-30073,13013,-30073,13013,
-29864,13484,-29864,13484,-29864,13484,-29864,13484,
-29649,13951,-29649,13951,-29649,13951,-29649,13951,
-29426,14415,-29426,14415,-29426,14415,-29426,14415,
-29196,14875,-29196,14875,-29196,14875,-29196,14875,
-28959,15332,-28959,15332,-28959,15332,-28959,15332,
-28714,15785,-28714,15785,-28714,15785,-28714,15785,
-28463,16234,-28463,16234,-28463,16234,-28463,16234,
-28204,16679,-28204,16679,-28204,16679,-28204,16679,
-27939,17120,-27939,17120,-27939,17120,-27939,17120,
-27667,17557,-27667,17557,-27667,17557,-27667,17557,
-27387,17989,-27387,17989,-27387,17989,-27387,17989,
-27101,18417,-27101,18417,-27101,18417,-27101,18417,
-26809,18841,-26809,18841,-26809,18841,-26809,18841,
-26510,19259,-26510,19259,-26510,19259,-26510,19259,
-26204,19673,-26204,19673,-26204,19673,-26204,19673,
-25892,20083,-25892,20083,-25892,20083,-25892,20083,
-25573,20487,-25573,20487,-25573,20487,-25573,20487,
-25248,20886,-25248,20886,-25248,20886,-25248,20886,
-24917,21280,-24917,21280,-24917,21280,-24917,21280,
-24579,21669,-24579,21669,-24579,21669,-24579,21669,
-24236,22052,-24236,22052,-24236,22052,-24236,22052,
-23887,22430,-23887,22430,-23887,22430,-23887,22430,
-23531,22802,-23531,22802,-23531,22802,-23531,22802,
-23170,23169,-23170,23169,-23170,23169,-23170,23169,
-22803,23530,-22803,23530,-22803,23530,-22803,23530,
-22431,23886,-22431,23886,-22431,23886,-22431,23886,
-22053,24235,-22053,24235,-22053,24235,-22053,24235,
-21670,24578,-21670,24578,-21670,24578,-21670,24578,
-21281,24916,-21281,24916,-21281,24916,-21281,24916,
-20887,25247,-20887,25247,-20887,25247,-20887,25247,
-20488,25572,-20488,25572,-20488,25572,-20488,25572,
-20084,25891,-20084,25891,-20084,25891,-20084,25891,
-19674,26203,-19674,26203,-19674,26203,-19674,26203,
-19260,26509,-19260,26509,-19260,26509,-19260,26509,
-18842,26808,-18842,26808,-18842,26808,-18842,26808,
-18418,27100,-18418,27100,-18418,27100,-18418,27100,
-17990,27386,-17990,27386,-17990,27386,-17990,27386,
-17558,27666,-17558,27666,-17558,27666,-17558,27666,
-17121,27938,-17121,27938,-17121,27938,-17121,27938,
-16680,28203,-16680,28203,-16680,28203,-16680,28203,
-16235,28462,-16235,28462,-16235,28462,-16235,28462,
-15786,28713,-15786,28713,-15786,28713,-15786,28713,
-15333,28958,-15333,28958,-15333,28958,-15333,28958,
-14876,29195,-14876,29195,-14876,29195,-14876,29195,
-14416,29425,-14416,29425,-14416,29425,-14416,29425,
-13952,29648,-13952,29648,-13952,29648,-13952,29648,
-13485,29863,-13485,29863,-13485,29863,-13485,29863,
-13014,30072,-13014,30072,-13014,30072,-13014,30072,
-12540,30272,-12540,30272,-12540,30272,-12540,30272,
-12063,30465,-12063,30465,-12063,30465,-12063,30465,
-11583,30651,-11583,30651,-11583,30651,-11583,30651,
-11100,30829,-11100,30829,-11100,30829,-11100,30829,
-10614,31000,-10614,31000,-10614,31000,-10614,31000,
-10126,31163,-10126,31163,-10126,31163,-10126,31163,
-9635,31318,-9635,31318,-9635,31318,-9635,31318,
-9142,31465,-9142,31465,-9142,31465,-9142,31465,
-8647,31605,-8647,31605,-8647,31605,-8647,31605,
-8149,31737,-8149,31737,-8149,31737,-8149,31737,
-7650,31861,-7650,31861,-7650,31861,-7650,31861,
-7148,31977,-7148,31977,-7148,31977,-7148,31977,
-6645,32086,-6645,32086,-6645,32086,-6645,32086,
-6140,32186,-6140,32186,-6140,32186,-6140,32186,
-5634,32279,-5634,32279,-5634,32279,-5634,32279,
-5126,32363,-5126,32363,-5126,32363,-5126,32363,
-4617,32440,-4617,32440,-4617,32440,-4617,32440,
-4107,32508,-4107,32508,-4107,32508,-4107,32508,
-3596,32569,-3596,32569,-3596,32569,-3596,32569,
-3084,32621,-3084,32621,-3084,32621,-3084,32621,
-2571,32665,-2571,32665,-2571,32665,-2571,32665,
-2058,32702,-2058,32702,-2058,32702,-2058,32702,
-1544,32730,-1544,32730,-1544,32730,-1544,32730,
-1030,32750,-1030,32750,-1030,32750,-1030,32750,
-515,32762,-515,32762,-515,32762,-515,32762
};


void dft1200(int16_t *x,int16_t *y,unsigned char scale_flag){

  int i,j;
  __m128i *x128=(__m128i *)x;
  __m128i *y128=(__m128i *)y;
  __m128i *twa128=(__m128i *)&twa1200[0];
  __m128i *twb128=(__m128i *)&twb1200[0];
  __m128i *twc128=(__m128i *)&twc1200[0];
  __m128i x2128[1200];// = (__m128i *)&x2128array[0];
  __m128i ytmp128[1200];//=&ytmp128array2[0];



  for (i=0,j=0;i<300;i++,j+=4) {
    x2128[i]    = x128[j];
    x2128[i+300] = x128[j+1];
    x2128[i+600] = x128[j+2];
    x2128[i+900] = x128[j+3];
  }

  dft300((int16_t *)x2128,(int16_t *)ytmp128,1);
  dft300((int16_t *)(x2128+300),(int16_t *)(ytmp128+300),1);
  dft300((int16_t *)(x2128+600),(int16_t *)(ytmp128+600),1);
  dft300((int16_t *)(x2128+900),(int16_t *)(ytmp128+900),1);

  bfly4_tw1(ytmp128,ytmp128+300,ytmp128+600,ytmp128+900,y128,y128+300,y128+600,y128+900);
  for (i=1,j=0;i<300;i++,j++) {
    bfly4(ytmp128+i,
	  ytmp128+300+i,
	  ytmp128+600+i,
	  ytmp128+900+i,
	  y128+i,
	  y128+300+i,
	  y128+600+i,
	  y128+900+i,
	  twa128+j,
	  twb128+j,
	  twc128+j);
  }

  if (scale_flag==1) {
    norm128 = _mm_set1_epi16(16384);//dft_norm_table[13]);
    
    for (i=0;i<1200;i++) {
      y128[i] = _mm_slli_epi16(_mm_mulhi_epi16(y128[i],norm128),1);
    }
  }

  _mm_empty();
  _m_empty();

}


#ifdef MR_MAIN

int main(int argc, char**argv) {


  time_stats_t ts;
  __m128i x[2048],y[2048],tw0,tw1,tw2,tw3;


  int i;

  set_taus_seed(0);
/*
  ((int16_t *)&tw0)[0] = 32767;
  ((int16_t *)&tw0)[1] = 0;
  ((int16_t *)&tw0)[2] = 32767;
  ((int16_t *)&tw0)[3] = 0;
  ((int16_t *)&tw0)[4] = 32767;
  ((int16_t *)&tw0)[5] = 0;
  ((int16_t *)&tw0)[6] = 32767;
  ((int16_t *)&tw0)[7] = 0;

  ((int16_t *)&tw1)[0] = 32767;
  ((int16_t *)&tw1)[1] = 0;
  ((int16_t *)&tw1)[2] = 32767;
  ((int16_t *)&tw1)[3] = 0;
  ((int16_t *)&tw1)[4] = 32767;
  ((int16_t *)&tw1)[5] = 0;
  ((int16_t *)&tw1)[6] = 32767;
  ((int16_t *)&tw1)[7] = 0;

  ((int16_t *)&tw2)[0] = 32767;
  ((int16_t *)&tw2)[1] = 0;
  ((int16_t *)&tw2)[2] = 32767;
  ((int16_t *)&tw2)[3] = 0;
  ((int16_t *)&tw2)[4] = 32767;
  ((int16_t *)&tw2)[5] = 0;
  ((int16_t *)&tw2)[6] = 32767;
  ((int16_t *)&tw2)[7] = 0;

  ((int16_t *)&tw3)[0] = 32767;
  ((int16_t *)&tw3)[1] = 0;
  ((int16_t *)&tw3)[2] = 32767;
  ((int16_t *)&tw3)[3] = 0;
  ((int16_t *)&tw3)[4] = 32767;
  ((int16_t *)&tw3)[5] = 0;
  ((int16_t *)&tw3)[6] = 32767;
  ((int16_t *)&tw3)[7] = 0;
  
  for (i=0;i<300;i++) {
    x[i] = _mm_set1_epi32(taus());
    x[i] = _mm_srai_epi16(x[i],4);
  }

  bfly2_tw1(x,x+1,y,y+1);
  printf("(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1]);
  printf("(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[2],((int16_t*)&y[0])[3],((int16_t*)&y[1])[2],((int16_t*)&y[1])[3]);
  printf("(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[4],((int16_t*)&y[0])[5],((int16_t*)&y[1])[4],((int16_t*)&y[1])[5]);
  printf("(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[6],((int16_t*)&y[0])[7],((int16_t*)&y[1])[6],((int16_t*)&y[1])[7]);
  bfly2(x,x+1,y,y+1, &tw0);
  printf("0(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1]);
  printf("1(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[2],((int16_t*)&y[0])[3],((int16_t*)&y[1])[2],((int16_t*)&y[1])[3]);
  printf("2(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[4],((int16_t*)&y[0])[5],((int16_t*)&y[1])[4],((int16_t*)&y[1])[5]);
  printf("3(%d,%d) (%d,%d) => (%d,%d) (%d,%d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&y[0])[6],((int16_t*)&y[0])[7],((int16_t*)&y[1])[6],((int16_t*)&y[1])[7]);
  bfly2(x,x+1,y,y+1, &tw0);


  bfly3_tw1(x,x+1,x+2,y, y+1,y+2);
  printf("0(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1],((int16_t*)&y[2])[0],((int16_t*)&y[2])[1]);
  printf("1(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[2],((int16_t*)&y[0])[3],((int16_t*)&y[1])[2],((int16_t*)&y[1])[3],((int16_t*)&y[2])[2],((int16_t*)&y[2])[3]);
  printf("2(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[4],((int16_t*)&y[0])[5],((int16_t*)&y[1])[4],((int16_t*)&y[1])[5],((int16_t*)&y[2])[4],((int16_t*)&y[2])[5]);
  printf("3(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[6],((int16_t*)&y[0])[7],((int16_t*)&y[1])[6],((int16_t*)&y[1])[7],((int16_t*)&y[2])[6],((int16_t*)&y[2])[7]);
  bfly3(x,x+1,x+2,y, y+1,y+2,&tw0,&tw1);

  printf("0(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1],((int16_t*)&y[2])[0],((int16_t*)&y[2])[1]);
  printf("1(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[2],((int16_t*)&y[0])[3],((int16_t*)&y[1])[2],((int16_t*)&y[1])[3],((int16_t*)&y[2])[2],((int16_t*)&y[2])[3]);
  printf("2(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[4],((int16_t*)&y[0])[5],((int16_t*)&y[1])[4],((int16_t*)&y[1])[5],((int16_t*)&y[2])[4],((int16_t*)&y[2])[5]);
  printf("3(%d,%d) (%d,%d) (%d %d) => (%d,%d) (%d,%d) (%d %d)\n",((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&y[0])[6],((int16_t*)&y[0])[7],((int16_t*)&y[1])[6],((int16_t*)&y[1])[7],((int16_t*)&y[2])[6],((int16_t*)&y[2])[7]);

  
  bfly4_tw1(x,x+1,x+2,x+3,y, y+1,y+2,y+3);
  printf("(%d,%d) (%d,%d) (%d %d) (%d,%d) => (%d,%d) (%d,%d) (%d %d) (%d,%d)\n",
	 ((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],
	 ((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&x[3])[0],((int16_t*)&x[3])[1],
	 ((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1],
	 ((int16_t*)&y[2])[0],((int16_t*)&y[2])[1],((int16_t*)&y[3])[0],((int16_t*)&y[3])[1]);

  bfly4(x,x+1,x+2,x+3,y, y+1,y+2,y+3,&tw0,&tw1,&tw2);
  printf("0(%d,%d) (%d,%d) (%d %d) (%d,%d) => (%d,%d) (%d,%d) (%d %d) (%d,%d)\n",
	 ((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],
	 ((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&x[3])[0],((int16_t*)&x[3])[1],
	 ((int16_t*)&y[0])[0],((int16_t*)&y[0])[1],((int16_t*)&y[1])[0],((int16_t*)&y[1])[1],
	 ((int16_t*)&y[2])[0],((int16_t*)&y[2])[1],((int16_t*)&y[3])[0],((int16_t*)&y[3])[1]);
  printf("1(%d,%d) (%d,%d) (%d %d) (%d,%d) => (%d,%d) (%d,%d) (%d %d) (%d,%d)\n",
	 ((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],
	 ((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&x[3])[0],((int16_t*)&x[3])[1],
	 ((int16_t*)&y[0])[2],((int16_t*)&y[0])[3],((int16_t*)&y[1])[2],((int16_t*)&y[1])[3],
	 ((int16_t*)&y[2])[2],((int16_t*)&y[2])[3],((int16_t*)&y[3])[2],((int16_t*)&y[3])[3]);
  printf("2(%d,%d) (%d,%d) (%d %d) (%d,%d) => (%d,%d) (%d,%d) (%d %d) (%d,%d)\n",
	 ((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],
	 ((int16_t*)&x[2])[0],((int16_t*)&x[2])[1],((int16_t*)&x[3])[0],((int16_t*)&x[3])[1],
	 ((int16_t*)&y[0])[4],((int16_t*)&y[0])[5],((int16_t*)&y[1])[4],((int16_t*)&y[1])[5],
	 ((int16_t*)&y[2])[4],((int16_t*)&y[2])[5],((int16_t*)&y[3])[4],((int16_t*)&y[3])[5]);
  printf("3(%d,%d) (%d,%d) (%d %d) (%d,%d) => (%d,%d) (%d,%d) (%d %d) (%d,%d)\n",
	 ((int16_t*)&x[0])[0],((int16_t*)&x[0])[1],((int16_t*)&x[1])[0],((int16_t*)&x[1])[1],
	 ((int16_t*)&x[2])[6],((int16_t*)&x[2])[7],((int16_t*)&x[3])[6],((int16_t*)&x[3])[7],
	 ((int16_t*)&y[0])[6],((int16_t*)&y[0])[7],((int16_t*)&y[1])[6],((int16_t*)&y[1])[7],
	 ((int16_t*)&y[2])[0],((int16_t*)&y[2])[1],((int16_t*)&y[3])[0],((int16_t*)&y[3])[1]);

  bfly5_tw1(x,x+1,x+2,x+3,x+4,y,y+1,y+2,y+3,y+4);

  for (i=0;i<5;i++)
    printf("%d,%d,",
	   ((int16_t*)&x[i])[0],((int16_t*)&x[i])[1]);
  printf("\n");
  for (i=0;i<5;i++)
    printf("%d,%d,",
	   ((int16_t*)&y[i])[0],((int16_t*)&y[i])[1]);
  printf("\n");

  bfly5(x,x+1,x+2,x+3,x+4,y, y+1,y+2,y+3,y+4,&tw0,&tw1,&tw2,&tw3);
  for (i=0;i<5;i++)
    printf("%d,%d,",
	   ((int16_t*)&x[i])[0],((int16_t*)&x[i])[1]);
  printf("\n");
  for (i=0;i<5;i++)
    printf("%d,%d,",
	   ((int16_t*)&y[i])[0],((int16_t*)&y[i])[1]);
  printf("\n");


  printf("\n\n12-point\n");   
  dft12f(x,
	 x+1,
	 x+2,
	 x+3,
	 x+4,
	 x+5,
	 x+6,
	 x+7,
	 x+8,
	 x+9,
	 x+10,
	 x+11,
	 y,
	 y+1,
	 y+2,
	 y+3,
	 y+4,
	 y+5,
	 y+6,
	 y+7,
	 y+8,
	 y+9,
	 y+10,
	 y+11);


  printf("X: ");
  for (i=0;i<12;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<12;i++)
    printf("%d,%d,",((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");
  
  dft24((int16_t *)x,(int16_t *)y,1);
  printf("\n\n24-point\n");
  printf("X: ");
  for (i=0;i<24;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<24;i++)
    printf("%d,%d,",((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");
  
  dft36((int16_t *)x,(int16_t *)y,1);
  printf("\n\n36-point\n");
  printf("X: ");
  for (i=0;i<36;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<36;i++)
    printf("%d,%d,",((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft48((int16_t *)x,(int16_t *)y,1);
  printf("\n\n48-point\n");
  printf("X: ");
  for (i=0;i<48;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<48;i++)
    printf("%d,%d,",((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft60((int16_t *)x,(int16_t *)y,1);
  printf("\n\n60-point\n");
  printf("X: ");
  for (i=0;i<60;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<60;i++)
    printf("%d,%d,",((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft72((int16_t *)x,(int16_t *)y,1);
  printf("\n\n72-point\n");
  printf("X: ");
  for (i=0;i<72;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<72;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft96((int16_t *)x,(int16_t *)y,1);
  printf("\n\n96-point\n");
  printf("X: ");
  for (i=0;i<96;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<96;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft108((int16_t *)x,(int16_t *)y,1);
  printf("\n\n108-point\n");
  printf("X: ");
  for (i=0;i<108;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<108;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft120((int16_t *)x,(int16_t *)y,1);
  printf("\n\n120-point\n");
  printf("X: ");
  for (i=0;i<120;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<120;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft144((int16_t *)x,(int16_t *)y,1);
  printf("\n\n144-point\n");
  printf("X: ");
  for (i=0;i<144;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<144;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft180((int16_t *)x,(int16_t *)y,1);
  printf("\n\n180-point\n");
  printf("X: ");
  for (i=0;i<180;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<180;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft192((int16_t *)x,(int16_t *)y,1);
  printf("\n\n192-point\n");
  printf("X: ");
  for (i=0;i<192;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<192;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft216((int16_t *)x,(int16_t *)y,1);
  printf("\n\n216-point\n");
  printf("X: ");
  for (i=0;i<216;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<216;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft240((int16_t *)x,(int16_t *)y,1);
  printf("\n\n240-point\n");
  printf("X: ");
  for (i=0;i<240;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<240;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft288((int16_t *)x,(int16_t *)y,1);
  printf("\n\n288-point\n");
  printf("X: ");
  for (i=0;i<288;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<288;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  dft300((int16_t *)x,(int16_t *)y,1);
  printf("\n\n300-point\n");
  printf("X: ");
  for (i=0;i<300;i++)
    printf("%d,%d,",((int16_t*)(&x[i]))[0],((int16_t *)(&x[i]))[1]);
  printf("\nY:");
  for (i=0;i<300;i++)
    printf("%d: %d,%d\n",i,((int16_t*)(&y[i]))[0],((int16_t *)(&y[i]))[1]);
  printf("\n");

  for (i=0;i<32;i++) {
    ((int16_t*)x)[i] = (int16_t)((taus()&0xffff))>>5;
  }
  memset((void*)&y[0],0,16*4);
  idft16((int16_t *)x,(int16_t *)y);
  printf("\n\n16-point\n");
  printf("X: ");
  for (i=0;i<4;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&x[i])[0],((int16_t *)&x[i])[1],((int16_t*)&x[i])[2],((int16_t *)&x[i])[3],((int16_t*)&x[i])[4],((int16_t*)&x[i])[5],((int16_t*)&x[i])[6],((int16_t*)&x[i])[7]);
  printf("\nY:");

  for (i=0;i<4;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&y[i])[0],((int16_t *)&y[i])[1],((int16_t*)&y[i])[2],((int16_t *)&y[i])[3],((int16_t*)&y[i])[4],((int16_t *)&y[i])[5],((int16_t*)&y[i])[6],((int16_t *)&y[i])[7]);
  printf("\n");
*/
  for (i=0;i<128;i++) {
    ((int16_t*)x)[i] = (int16_t)((taus()&0xffff))>>5;
  }
  memset((void*)&y[0],0,64*4);
  dft64((int16_t *)x,(int16_t *)y,1);
  dft64((int16_t *)x,(int16_t *)y,1);
  dft64((int16_t *)x,(int16_t *)y,1); 
  reset_meas(&ts);
  for (i=0;i<1000;i++) {
    start_meas(&ts);
    dft64((int16_t *)x,(int16_t *)y,1);
    stop_meas(&ts);

  }
  printf("\n\n64-point (%f cycles)\n",(double)ts.diff/(double)ts.trials);
  /*printf("X: ");
  for (i=0;i<16;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&x[i])[0],((int16_t *)&x[i])[1],((int16_t*)&x[i])[2],((int16_t *)&x[i])[3],((int16_t*)&x[i])[4],((int16_t*)&x[i])[5],((int16_t*)&x[i])[6],((int16_t*)&x[i])[7]);
  printf("\nY:");

  for (i=0;i<16;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&y[i])[0],((int16_t *)&y[i])[1],((int16_t*)&y[i])[2],((int16_t *)&y[i])[3],((int16_t*)&y[i])[4],((int16_t *)&y[i])[5],((int16_t*)&y[i])[6],((int16_t *)&y[i])[7]);
  printf("\n");
*/
  for (i=0;i<256;i++) {
    ((int16_t*)x)[i] = (int16_t)((taus()&0xffff))>>5;
  }
  memset((void*)&y[0],0,128*4);
  reset_meas(&ts);
  for (i=0;i<10000;i++) {
    start_meas(&ts);
    dft128((int16_t *)x,(int16_t *)y,0);
    stop_meas(&ts);
  }
  printf("\n\n128-point(%f cycles)\n",(double)ts.diff/(double)ts.trials);
 /* printf("X: ");
  for (i=0;i<32;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&x[i])[0],((int16_t *)&x[i])[1],((int16_t*)&x[i])[2],((int16_t *)&x[i])[3],((int16_t*)&x[i])[4],((int16_t*)&x[i])[5],((int16_t*)&x[i])[6],((int16_t*)&x[i])[7]);
  printf("\nY:");

  for (i=0;i<32;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&y[i])[0],((int16_t *)&y[i])[1],((int16_t*)&y[i])[2],((int16_t *)&y[i])[3],((int16_t*)&y[i])[4],((int16_t *)&y[i])[5],((int16_t*)&y[i])[6],((int16_t *)&y[i])[7]);
  printf("\n");
*/
  for (i=0;i<512;i++) {
    ((int16_t*)x)[i] = (int16_t)((taus()&0xffff))>>5;
  }
  memset((void*)&y[0],0,256*4);
  reset_meas(&ts);
  for (i=0;i<10000;i++) {
    start_meas(&ts);
    dft256((int16_t *)x,(int16_t *)y,1);
    stop_meas(&ts);
  }
  printf("\n\n256-point(%f cycles)\n",(double)ts.diff/(double)ts.trials);
 /* printf("X: ");
  for (i=0;i<64;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&x[i])[0],((int16_t *)&x[i])[1],((int16_t*)&x[i])[2],((int16_t *)&x[i])[3],((int16_t*)&x[i])[4],((int16_t*)&x[i])[5],((int16_t*)&x[i])[6],((int16_t*)&x[i])[7]);
  printf("\nY:");

  for (i=0;i<64;i++)
    printf("%d,%d,%d,%d,%d,%d,%d,%d,",((int16_t*)&y[i])[0],((int16_t *)&y[i])[1],((int16_t*)&y[i])[2],((int16_t *)&y[i])[3],((int16_t*)&y[i])[4],((int16_t *)&y[i])[5],((int16_t*)&y[i])[6],((int16_t *)&y[i])[7]);
  printf("\n");
*/
  reset_meas(&ts);
  for (i=0;i<10000;i++) {
    start_meas(&ts);
    dft512((int16_t *)x,(int16_t *)y,1);
    stop_meas(&ts);
  }
  printf("\n\n512-point(%f cycles)\n",(double)ts.diff/(double)ts.trials);

  reset_meas(&ts);
  for (i=0;i<10000;i++) {
    start_meas(&ts);
    dft1024((int16_t *)x,(int16_t *)y,1);
    stop_meas(&ts);
  }
  printf("\n\n1024-point(%f cycles)\n",(double)ts.diff/(double)ts.trials);

  reset_meas(&ts);
  for (i=0;i<10000;i++) {
    start_meas(&ts);
    dft2048((int16_t *)x,(int16_t *)y,1);
    stop_meas(&ts);
  }
  printf("\n\n2048-point(%f cycles)\n",(double)ts.diff/(double)ts.trials);
  return(0);
}


#endif
