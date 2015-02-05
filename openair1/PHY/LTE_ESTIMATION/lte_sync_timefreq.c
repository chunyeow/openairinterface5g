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

/*! \file PHY/LTE_ESTIMATION/lte_sync_timefreq.c
* \brief Initial time frequency scan of entire LTE band
* \author R. Knopp
* \date 2014
* \version 0.1
* \company Eurecom
* \email: raymond.knopp@eurecom.fr
* \note
* \warning
*/
/* file: lte_sync_timefreq.c
   purpose: scan for likely cells over entire LTE band using PSS. Provides coarse frequency offset in addtition to 10 top likelihoods per PSS sequence
   author: raymond.knopp@eurecom.fr
   date: 23.01.2015
*/

//#include "defs.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "pss6144.h"

#define DEBUG_TF 1

void lte_sync_timefreq(PHY_VARS_UE *ue,int band,unsigned int DL_freq) {

  UE_SCAN_INFO_t *scan_info = &ue->scan_info[band];
  int16_t spectrum[12288] __attribute__((aligned(16)));
  int16_t spectrum_p5ms[12288] __attribute__((aligned(16)));
  int i,f,f2,band_idx;
  __m128i autocorr0[256/4],autocorr1[256/4],autocorr2[256/4];
  __m128i autocorr0_t[256/4],autocorr1_t[256/4],autocorr2_t[256/4];
  __m128i tmp_t[256/4];
  int32_t *rxp;
  int16_t *sp;
  __m128i *sp2;
  __m128i s;
  int re;
  __m128i mmtmp00,mmtmp01,mmtmp02,mmtmp10,mmtmp11,mmtmp12;
  int maxcorr[3],minamp,pos,pssind;
  int16_t *pss6144_0,*pss6144_1,*pss6144_2;
  
  
  
  //    for (i=0;i<38400*4;i+=3072)   // steps of 200 us with 100 us overlap, 0 to 5s

  for (i = 15360-3072*2;i<15360+3072+1;i+=3072)  {

 
      //compute frequency-domain representation of 6144-sample chunk
      
      rxp = &ue->lte_ue_common_vars.rxdata[0][i];
      sp=spectrum;
      while (1) {

	//compute frequency-domain representation of 6144-sample chunk
	fft6144((int16_t *)rxp,
		sp);
	printf("sp %p\n",sp);
	if (i==12288)
	  write_output("scan6144F.m","s6144F",sp,6144,1,1);
	  for (f = -130;f<-125;f++) {  // this is -10MHz to 10 MHz in 5 kHz steps
	    
	    if ((f<-256)||(f>=0)) { // no split around DC
	      printf("No split, f %d\n",f);
	      // align filters and input buffer pointer to 128-bit
	      switch (f&3) {
	      case 0: 
		pss6144_0 = &pss6144_0_0[0];
		pss6144_1 = &pss6144_1_0[0];
		pss6144_2 = &pss6144_2_0[0];
		sp2 = (f<0) ? (__m128i*)&sp[12288+(f<<1)] : (__m128i*)&sp;
		break;
	      case 1: 
		pss6144_0 = &pss6144_0_1[0];
		pss6144_1 = &pss6144_1_1[0];
		pss6144_2 = &pss6144_2_1[0];
		sp2 = (f<0) ? (__m128i*)&sp[12286+(f<<1)] : (__m128i*)sp;
		break;
	      case 2: 
		pss6144_0 = &pss6144_0_2[0];
		pss6144_1 = &pss6144_1_2[0];
		pss6144_2 = &pss6144_2_2[0];
		sp2 = (f<0) ? (__m128i*)&sp[12284+(f<<1)] : (__m128i*)sp;
		break;
	      case 3: 
		pss6144_0 = &pss6144_0_3[0];
		pss6144_1 = &pss6144_1_3[0];
		pss6144_2 = &pss6144_2_3[0];
		sp2 = (f<0) ? (__m128i*)&sp[12282+(f<<1)] : (__m128i*)sp;
		break;
	      } 

	      for (re = 0; re<256/4; re++) {  // loop over 256 points of upsampled PSS
		s = sp2[re];
		mmtmp00 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp01 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp02 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		s = _mm_shufflelo_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_shufflehi_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_sign_epi16(s,*(__m128i*)&conjugate[0]);
		mmtmp10 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp11 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp12 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		autocorr0[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp00,mmtmp10),_mm_unpackhi_epi32(mmtmp00,mmtmp10));
		autocorr1[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp01,mmtmp11),_mm_unpackhi_epi32(mmtmp01,mmtmp11));
		autocorr2[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp02,mmtmp12),_mm_unpackhi_epi32(mmtmp02,mmtmp12));
		
	      }
	    }
	    else { // Split around DC, this is the negative frequencies
	      printf("split around DC, f %d (f/4 %d)\n",f,f>>2);
	      // align filters and input buffer pointer to 128-bit	      
	      switch (f&3) {
	      case 0: 
		pss6144_0 = &pss6144_0_0[0];
		pss6144_1 = &pss6144_1_0[0];
		pss6144_2 = &pss6144_2_0[0];
		sp2 = (__m128i*)&sp[12288+(f<<1)];
		break;
	      case 1: 
		pss6144_0 = &pss6144_0_1[0];
		pss6144_1 = &pss6144_1_1[0];
		pss6144_2 = &pss6144_2_1[0];
		sp2 = (__m128i*)&sp[12286+(f<<1)];
		break;
	      case 2: 
		pss6144_0 = &pss6144_0_2[0];
		pss6144_1 = &pss6144_1_2[0];
		pss6144_2 = &pss6144_2_2[0];
		sp2 = (__m128i*)&sp[12284+(f<<1)];
		break;
	      case 3: 
		pss6144_0 = &pss6144_0_3[0];
		pss6144_1 = &pss6144_1_3[0];
		pss6144_2 = &pss6144_2_3[0];
		sp2 = (__m128i*)&sp[12282+(f<<1)];
		break;
	      } 
	      for (re = 0; re<(256+f)/4; re++) {  // loop over 256 points of upsampled PSS
		s = sp2[re];
		printf("re %d, %p\n",re,&sp2[re]);
		print_shorts("s",&s);
		print_shorts("pss",&pss6144_0[re]);

		mmtmp00 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp01 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp02 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		s = _mm_shufflelo_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_shufflehi_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_sign_epi16(s,*(__m128i*)&conjugate[0]);
		mmtmp10 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp11 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp12 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		autocorr0[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp00,mmtmp10),_mm_unpackhi_epi32(mmtmp00,mmtmp10));
		autocorr1[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp01,mmtmp11),_mm_unpackhi_epi32(mmtmp01,mmtmp11));
		autocorr2[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp02,mmtmp12),_mm_unpackhi_epi32(mmtmp02,mmtmp12));
		
		
	      }
	      // This is the +ve frequencies

	      // align filters to 128-bit
	      sp2 = (__m128i*)&sp[0];
	      switch (f&3) {
	      case 0: 
		pss6144_0 = &pss6144_0_0[256];
		pss6144_1 = &pss6144_1_0[256];
		pss6144_2 = &pss6144_2_0[256];
		break;
	      case 1: 
		pss6144_0 = &pss6144_0_1[256];
		pss6144_1 = &pss6144_1_1[256];
		pss6144_2 = &pss6144_2_1[256];
		break;
	      case 2: 
		pss6144_0 = &pss6144_0_2[256];
		pss6144_1 = &pss6144_1_2[256];
		pss6144_2 = &pss6144_2_2[256];
		break;
	      case 3: 
		pss6144_0 = &pss6144_0_3[256];
		pss6144_1 = &pss6144_1_3[256];
		pss6144_2 = &pss6144_2_3[256];
		break;
	      } 
	      for (re = 0; re<-f/4; re++) {  // loop over 256 points of upsampled PSS
		s = sp2[re];
		printf("re %d %p\n",re,&sp2[re]);
		print_shorts("s",&s);
		print_shorts("pss",&pss6144_0[re]);
		mmtmp00 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp01 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp02 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		s = _mm_shufflelo_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_shufflehi_epi16(s,_MM_SHUFFLE(2,3,0,1));
		s = _mm_sign_epi16(s,*(__m128i*)&conjugate[0]);
		mmtmp10 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_0)[re],s),15);
		mmtmp11 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_1)[re],s),15);
		mmtmp12 = _mm_srai_epi32(_mm_madd_epi16(((__m128i*)pss6144_2)[re],s),15);
		
		autocorr0[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp00,mmtmp10),_mm_unpackhi_epi32(mmtmp00,mmtmp10));
		autocorr1[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp01,mmtmp11),_mm_unpackhi_epi32(mmtmp01,mmtmp11));
		autocorr2[re] = _mm_packs_epi32(_mm_unpacklo_epi32(mmtmp02,mmtmp12),_mm_unpackhi_epi32(mmtmp02,mmtmp12));
		
	      }
	      
	    }
	    // ifft, accumulate energy over two half-frames
	    idft256((int16_t*)autocorr0,(int16_t*)tmp_t,1);
	    memset((void*)autocorr0_t,0,256*4);
	    memset((void*)autocorr1_t,0,256*4);
	    memset((void*)autocorr2_t,0,256*4);
	    for (re=0;re<(256/4);re++)
	      autocorr0_t[re] = _mm_add_epi32(autocorr0_t[re],_mm_madd_epi16(tmp_t[re],tmp_t[re]));
	    idft256((int16_t*)autocorr1,(int16_t*)autocorr1_t,1);
	    for (re=0;re<(256/4);re++)
	      autocorr1_t[re] = _mm_add_epi32(autocorr1_t[re],_mm_madd_epi16(tmp_t[re],tmp_t[re]));
	    idft256((int16_t*)autocorr2,(int16_t*)autocorr2_t,1);
	    for (re=0;re<(256/4);re++)
	      autocorr2_t[re] = _mm_add_epi32(autocorr2_t[re],_mm_madd_epi16(tmp_t[re],tmp_t[re]));

	  
	    //compute max correlation over time window
	    maxcorr[0] = 0;
	    maxcorr[1] = 0;
	    maxcorr[2] = 0;
	    for (re=0;re<256;re++) {
#ifdef DEBUG_TF
	      printf("%d,",((int32_t*)autocorr0_t)[re]);
#endif
	      if (((int32_t*)autocorr0_t)[re] > maxcorr[0]) {
		maxcorr[0]=((int32_t*)autocorr0_t)[re];
		printf("*");
	      }
	      if (((int32_t*)autocorr1_t)[re] > maxcorr[1])
		maxcorr[1]=((int32_t*)autocorr1_t)[re];
	      if (((int32_t*)autocorr2_t)[re] > maxcorr[2])
		maxcorr[2]=((int32_t*)autocorr2_t)[re];
	    }
#ifdef DEBUG_TF
	    printf("\n");
#endif
	    
	    
	    for (pssind=0;pssind<3;pssind++) {
	      printf("pss %d, amp %d freq %u, i %d\n",pssind,maxcorr[pssind],((f+128)*5000)+DL_freq,i);
	      minamp=(int)((1<<30)-1);
	      for (band_idx=0;band_idx<10;band_idx++)
		if (minamp > scan_info->amp[pssind][band_idx]) {
		  minamp = scan_info->amp[pssind][band_idx];
		  pos    = band_idx;
		}
	      if (maxcorr[pssind]>minamp) {
		scan_info->amp[pssind][pos]=maxcorr[pssind];
		scan_info->freq_offset_Hz[pssind][pos]=(f*5000)+DL_freq;
	      }
	    } // loop on pss index
	  }
	  if (rxp == &ue->lte_ue_common_vars.rxdata[0][i+38400*4]) {
	    rxp = &ue->lte_ue_common_vars.rxdata[0][i+38400*4];
	    sp=spectrum_p5ms; 
	  }
	  else {
	    break;
	  }
      }   
      }// loop on time index i
#ifdef DEBUG_TF
      exit(-1);
#endif
}

