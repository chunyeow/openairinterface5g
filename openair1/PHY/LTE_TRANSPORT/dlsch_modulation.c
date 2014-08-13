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

/*! \file PHY/LTE_TRANSPORT/dlsch_modulation.c
* \brief Top-level routines for generating the PDSCH physical channel from 36-211, V8.6 2009-03
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "PHY/CODING/defs.h"
#include "PHY/CODING/extern.h"
#include "PHY/CODING/lte_interleaver_inline.h"
#include "PHY/LTE_TRANSPORT/defs.h"
#include "defs.h"
#include "UTIL/LOG/vcd_signal_dumper.h"

//#define DEBUG_DLSCH_MODULATION 

//#define is_not_pilot(pilots,re,nushift,use2ndpilots) ((pilots==0) || ((re!=nushift) && (re!=nushift+6)&&((re!=nushift+3)||(use2ndpilots==1))&&((re!=nushift+9)||(use2ndpilots==1)))?1:0)

uint8_t is_not_pilot(uint8_t pilots, uint8_t re, uint8_t nushift, uint8_t use2ndpilots) {

  uint8_t offset = (pilots==2)?3:0;
  int nushiftmod3 = nushift%3;
 
  if (pilots==0)
    return(1);

  if (use2ndpilots==1) {  // This is for SISO (mode 1)
    if ((re!=nushift+offset) && (re!=((nushift+6+offset)%12)))
      return(1);
  }
  else { // 2 antenna pilots
    if ((re!=nushiftmod3) && (re!=nushiftmod3+6) && (re!=nushiftmod3+3) && (re!=nushiftmod3+9))
      return(1);
  }
  return(0);
}

uint8_t is_not_UEspecRS(int first_layer,int re) {

  return(1);
}

void generate_64qam_table(void) {

  int a,b,c,index;


  for (a=-1;a<=1;a+=2) 
    for (b=-1;b<=1;b+=2) 
      for (c=-1;c<=1;c+=2) {
	index = (1+a)*2 + (1+b) + (1+c)/2;  
	qam64_table[index] = -a*(QAM64_n1 + b*(QAM64_n2 + (c*QAM64_n3))); // 0 1 2
      } 
}

void generate_16qam_table(void) {

  int a,b,index;

  for (a=-1;a<=1;a+=2) 
    for (b=-1;b<=1;b+=2) {
	index = (1+a) + (1+b)/2;  
	qam16_table[index] = -a*(QAM16_n1 + (b*QAM16_n2)); 
      } 
}




void layer1prec2A(int32_t *antenna0_sample, int32_t *antenna1_sample, uint8_t precoding_index) {

  switch (precoding_index) {

  case 0: // 1 1
    *antenna1_sample=*antenna0_sample;
    break;

  case 1: // 1 -1
    ((int16_t *)antenna1_sample)[0] = -((int16_t *)antenna0_sample)[0];
    ((int16_t *)antenna1_sample)[1] = -((int16_t *)antenna0_sample)[1];
    break;

  case 2: // 1 j
    ((int16_t *)antenna1_sample)[0] = -((int16_t *)antenna0_sample)[1];
    ((int16_t *)antenna1_sample)[1] = ((int16_t *)antenna0_sample)[0];
    break;

  case 3: // 1 -j
    ((int16_t *)antenna1_sample)[0] = ((int16_t *)antenna0_sample)[1];
    ((int16_t *)antenna1_sample)[1] = -((int16_t *)antenna0_sample)[0];
    break;
  }

  // normalize
  /*  ((int16_t *)antenna0_sample)[0] = (int16_t)((((int16_t *)antenna0_sample)[0]*ONE_OVER_SQRT2_Q15)>>15);  
  ((int16_t *)antenna0_sample)[1] = (int16_t)((((int16_t *)antenna0_sample)[1]*ONE_OVER_SQRT2_Q15)>>15);  ((int16_t *)antenna1_sample)[0] = (int16_t)((((int16_t *)antenna1_sample)[0]*ONE_OVER_SQRT2_Q15)>>15);  
  ((int16_t *)antenna1_sample)[1] = (int16_t)((((int16_t *)antenna1_sample)[1]*ONE_OVER_SQRT2_Q15)>>15);  */
} 

int allocate_REs_in_RB(mod_sym_t **txdataF,
		       uint32_t *jj,
		       uint16_t re_offset,
		       uint32_t symbol_offset,
		       uint8_t *output,
		       MIMO_mode_t mimo_mode,
		       uint8_t nu,
		       uint8_t pilots,
		       uint8_t mod_order,
		       uint8_t precoder_index,
		       int16_t amp,
		       int16_t *qam_table_s,
		       uint32_t *re_allocated,
		       uint8_t skip_dc,
		       uint8_t skip_half,
		       uint8_t use2ndpilots,
		       uint8_t Nlayers,
		       uint8_t first_layer,
		       LTE_DL_FRAME_PARMS *frame_parms) {

  uint32_t tti_offset,aa;
  uint8_t re;
  uint8_t qam64_table_offset_re = 0;
  uint8_t qam64_table_offset_im = 0;
  uint8_t qam16_table_offset_re = 0;
  uint8_t qam16_table_offset_im = 0;
  int16_t gain_lin_QPSK;//,gain_lin_16QAM1,gain_lin_16QAM2;
  int16_t re_off=re_offset;
  gain_lin_QPSK = (int16_t)((amp*ONE_OVER_SQRT2_Q15)>>15);  
  uint8_t first_re,last_re;
  int32_t tmp_sample1,tmp_sample2;
  int16_t tmp_amp=amp;
  uint8_t layer;
  /*
  switch (mod_order) {
  case 2:
    // QPSK single stream
    
    break;
  case 4:
    //16QAM Single stream
    gain_lin_16QAM1 = (int16_t)(((int32_t)amp*QAM16_n1)>>15);
    gain_lin_16QAM2 = (int16_t)(((int32_t)amp*QAM16_n2)>>15);
    
    break;
    
  case 6:
    //64QAM Single stream
    break;
  default:
    break;
  }
*/

#ifdef DEBUG_DLSCH_MODULATION
  printf("allocate_re (mod %d): symbol_offset %d re_offset %d (%d,%d), jj %d -> %d,%d, nu %d\n",mod_order,symbol_offset,re_offset,skip_dc,skip_half,*jj, output[*jj], output[1+*jj],nu);
#endif

  first_re=0;
  last_re=12;

  if (skip_half==1) 
    last_re=6;
  else if (skip_half==2)
    first_re=6;
  
  for (re=first_re;re<last_re;re++) {
    
    if ((skip_dc == 1) && (re==6))
      re_off=re_off - frame_parms->ofdm_symbol_size+1;
    
    tti_offset = symbol_offset + re_off + re;
    
    // check that RE is not from Cell-specific RS

    if (is_not_pilot(pilots,re,frame_parms->nushift,use2ndpilots)==1) { 
      //     printf("re %d (jj %d)\n",re,*jj);

      
      if (mimo_mode == SISO) {  //SISO mapping
	*re_allocated = *re_allocated + 1;	
	switch (mod_order) {
	case 2:  //QPSK
	  //	  printf("%d(%d) : %d,%d => ",tti_offset,*jj,((int16_t*)&txdataF[0][tti_offset])[0],((int16_t*)&txdataF[0][tti_offset])[1]);
	  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	    ((int16_t*)&txdataF[aa][tti_offset])[0] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //I //b_i
	  }
	  *jj = *jj + 1;
	  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	    ((int16_t*)&txdataF[aa][tti_offset])[1] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //Q //b_{i+1}
	  }
	  *jj = *jj + 1;
	  
	  // printf("%d,%d\n",((int16_t*)&txdataF[0][tti_offset])[0],((int16_t*)&txdataF[0][tti_offset])[1]);
	  break;
	  
	case 4:  //16QAM
	  
	  qam16_table_offset_re = 0;
	  qam16_table_offset_im = 0;
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=2;
	  *jj=*jj+1;
	  
	  
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	    ((int16_t *)&txdataF[aa][tti_offset])[0]+=qam_table_s[qam16_table_offset_re];
	    ((int16_t *)&txdataF[aa][tti_offset])[1]+=qam_table_s[qam16_table_offset_im];
	    //	    ((int16_t *)&txdataF[aa][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	    //	    ((int16_t *)&txdataF[aa][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);
	  }
	  
	  break;
	  
	case 6:  //64QAM
	  
	  
	  qam64_table_offset_re = 0;
	  qam64_table_offset_im = 0;
	  
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	    ((int16_t *)&txdataF[aa][tti_offset])[0]+=qam_table_s[qam64_table_offset_re];//(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	    ((int16_t *)&txdataF[aa][tti_offset])[1]+=qam_table_s[qam64_table_offset_im];//(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);
	  }
	  break;
	  
	}
      }
            
      else if (mimo_mode == ALAMOUTI){
	*re_allocated = *re_allocated + 1;	
          // normalization for 2 tx antennas
	amp = (int16_t)(((int32_t)tmp_amp*ONE_OVER_SQRT2_Q15)>>15);

	switch (mod_order) {
	case 2:  //QPSK
	  
	  // first antenna position n -> x0
	  
	  ((int16_t*)&tmp_sample1)[0] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj=*jj+1;
	  ((int16_t*)&tmp_sample1)[1] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj=*jj+1;
	  
	  // second antenna position n -> -x1*
	  
	  ((int16_t*)&tmp_sample2)[0] = (output[*jj]==1) ? (gain_lin_QPSK) : -gain_lin_QPSK;
	  *jj=*jj+1;
	  ((int16_t*)&tmp_sample2)[1] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj=*jj+1;
	  
      // normalization for 2 tx antennas
      ((int16_t*)&txdataF[0][tti_offset])[0] += (int16_t)((((int16_t*)&tmp_sample1)[0]*ONE_OVER_SQRT2_Q15)>>15);
	  ((int16_t*)&txdataF[0][tti_offset])[1] += (int16_t)((((int16_t*)&tmp_sample1)[1]*ONE_OVER_SQRT2_Q15)>>15);
      ((int16_t*)&txdataF[1][tti_offset])[0] += (int16_t)((((int16_t*)&tmp_sample2)[0]*ONE_OVER_SQRT2_Q15)>>15);
      ((int16_t*)&txdataF[1][tti_offset])[1] += (int16_t)((((int16_t*)&tmp_sample2)[1]*ONE_OVER_SQRT2_Q15)>>15);

	  break;
	  
	case 4:  //16QAM
	  
	  // Antenna 0 position n 
	  
	  qam16_table_offset_re = 0;
	  qam16_table_offset_im = 0;
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=2;
	  *jj=*jj+1;
	  
	  
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[0][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	  ((int16_t *)&txdataF[0][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);
	  
	  // Antenna 1 position n Real part -> -x1*
	  
	  qam16_table_offset_re = 0;
	  qam16_table_offset_im = 0;
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=2;
	  *jj=*jj+1;
	  
	  
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[1][tti_offset])[0]+=-(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	  ((int16_t *)&txdataF[1][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);
	  
	  
	  break;
	case 6:   // 64-QAM
	  
	  // Antenna 0
	  qam64_table_offset_re = 0;
	  qam64_table_offset_im = 0;

	  if (output[*jj] == 1)
	    qam64_table_offset_re+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[0][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	  ((int16_t *)&txdataF[0][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);
	  
	  
	  // Antenna 1 => -x1*
	  qam64_table_offset_re = 0;
	  qam64_table_offset_im = 0;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[1][tti_offset])[0]+=-(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	  ((int16_t *)&txdataF[1][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);
	  
	  break;
	}
	// fill in the rest of the ALAMOUTI precoding
	if (is_not_pilot(pilots,re + 1,frame_parms->nushift,use2ndpilots)==1) {
	  ((int16_t *)&txdataF[0][tti_offset+1])[0] += -((int16_t *)&txdataF[1][tti_offset])[0]; //x1
	  ((int16_t *)&txdataF[0][tti_offset+1])[1] += ((int16_t *)&txdataF[1][tti_offset])[1];
	  ((int16_t *)&txdataF[1][tti_offset+1])[0] += ((int16_t *)&txdataF[0][tti_offset])[0];  //x0*
	  ((int16_t *)&txdataF[1][tti_offset+1])[1] += -((int16_t *)&txdataF[0][tti_offset])[1];
	}
	else {
	  ((int16_t *)&txdataF[0][tti_offset+2])[0] += -((int16_t *)&txdataF[1][tti_offset])[0]; //x1
	  ((int16_t *)&txdataF[0][tti_offset+2])[1] += ((int16_t *)&txdataF[1][tti_offset])[1];
	  ((int16_t *)&txdataF[1][tti_offset+2])[0] += ((int16_t *)&txdataF[0][tti_offset])[0];  //x0*
	  ((int16_t *)&txdataF[1][tti_offset+2])[1] += -((int16_t *)&txdataF[0][tti_offset])[1];
	}
      }
      else if (mimo_mode == ANTCYCLING ) {
	*re_allocated = *re_allocated + 1;	
	switch (mod_order) {
	case 2:  //QPSK
	  
	  ((int16_t*)&txdataF[re&1][tti_offset])[0] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj = *jj + 1;
	  ((int16_t*)&txdataF[re&1][tti_offset])[1] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj = *jj + 1;
	  break;
	  
	case 4:  //16QAM
	  
	  qam16_table_offset_re = 0;
	  qam16_table_offset_im = 0;

	  if (output[*jj] == 1)
	    qam16_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=2;
	  *jj=*jj+1;
	  
	  
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[re&1][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	  ((int16_t *)&txdataF[re&1][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);
	  
	  
	  break;
	  
	case 6:  //64QAM
	  
	  qam64_table_offset_re = 0;
	  qam64_table_offset_im = 0;

	  if (output[*jj] == 1)
	    qam64_table_offset_re+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t *)&txdataF[re&1][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	  ((int16_t *)&txdataF[re&1][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);
	  
	}
      }
       
      else if ((mimo_mode >= UNIFORM_PRECODING11)&&(mimo_mode <= PUSCH_PRECODING1)) {
	// this is for transmission modes 4-6 (1 layer)
	*re_allocated = *re_allocated + 1;	         
	amp = (int16_t)(((int32_t)tmp_amp*ONE_OVER_SQRT2_Q15)>>15);

	switch (mod_order) {
	case 2:  //QPSK

	  ((int16_t*)&tmp_sample1)[0] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj = *jj + 1;
	  ((int16_t*)&tmp_sample1)[1] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK;
	  *jj = *jj + 1;

      // normalization for 2 tx antennas
	  ((int16_t*)&txdataF[0][tti_offset])[0] += (int16_t)((((int16_t*)&tmp_sample1)[0]*ONE_OVER_SQRT2_Q15)>>15);
	  ((int16_t*)&txdataF[0][tti_offset])[1] += (int16_t)((((int16_t*)&tmp_sample1)[1]*ONE_OVER_SQRT2_Q15)>>15);

	  if (frame_parms->nb_antennas_tx == 2) {
	    layer1prec2A(&tmp_sample1,&tmp_sample2,precoder_index);
        ((int16_t*)&txdataF[1][tti_offset])[0] += (int16_t)((((int16_t*)&tmp_sample2)[0]*ONE_OVER_SQRT2_Q15)>>15);
        ((int16_t*)&txdataF[1][tti_offset])[1] += (int16_t)((((int16_t*)&tmp_sample2)[1]*ONE_OVER_SQRT2_Q15)>>15);
	  }

	  break;
	  
	case 4:  //16QAM
	  
	  qam16_table_offset_re = 0;
	  qam16_table_offset_im = 0;

	  if (output[*jj] == 1)
	    qam16_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=2;
	  *jj=*jj+1;
	  
	  
	  if (output[*jj] == 1)
	    qam16_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam16_table_offset_im+=1;
	  *jj=*jj+1;
	  
	   ((int16_t*)&tmp_sample1)[0] = (int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	   ((int16_t*)&tmp_sample1)[1] = (int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);

	   ((int16_t *)&txdataF[0][tti_offset])[0] += ((int16_t*)&tmp_sample1)[0];
	   ((int16_t *)&txdataF[0][tti_offset])[1] += ((int16_t*)&tmp_sample1)[1];
	  
	  if (frame_parms->nb_antennas_tx == 2) {
	    layer1prec2A(&tmp_sample1,&tmp_sample2,precoder_index);
	    ((int16_t*)&txdataF[1][tti_offset])[0] += ((int16_t*)&tmp_sample2)[0];
	    ((int16_t*)&txdataF[1][tti_offset])[1] += ((int16_t*)&tmp_sample2)[1];
	  }

	  break;
	  
	case 6:  //64QAM
	  
	  
	  qam64_table_offset_re = 0;
	  qam64_table_offset_im = 0;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=4;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=2;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_re+=1;
	  *jj=*jj+1;
	  if (output[*jj] == 1)
	    qam64_table_offset_im+=1;
	  *jj=*jj+1;
	  
	  ((int16_t*)&tmp_sample1)[0] = (int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	  ((int16_t*)&tmp_sample1)[1] = (int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);

	  ((int16_t *)&txdataF[0][tti_offset])[0] += ((int16_t*)&tmp_sample1)[0];
	  ((int16_t *)&txdataF[0][tti_offset])[1] += ((int16_t*)&tmp_sample1)[1];
	  
	  if (frame_parms->nb_antennas_tx == 2) {
	    layer1prec2A(&tmp_sample1,&tmp_sample2,precoder_index);
	    ((int16_t*)&txdataF[1][tti_offset])[0] += ((int16_t*)&tmp_sample2)[0];
	    ((int16_t*)&txdataF[1][tti_offset])[1] += ((int16_t*)&tmp_sample2)[1];
	  }
	  
	  break;
	  
	}
      }
      if (mimo_mode == ALAMOUTI) {
	re++;  // adjacent carriers are taken care of by precoding
	*re_allocated = *re_allocated + 1;
	if (is_not_pilot(pilots,re,frame_parms->nushift,use2ndpilots)==0) {
	  re++;  
	  *re_allocated = *re_allocated + 1;
	}
      }
    
      if (mimo_mode >= TM8) { //TM8,TM9,TM10

	if (is_not_UEspecRS(first_layer,re)) {	  	  	
	  switch (mod_order) {
	  case 2:  //QPSK
	    //	  printf("%d : %d,%d => ",tti_offset,((int16_t*)&txdataF[0][tti_offset])[0],((int16_t*)&txdataF[0][tti_offset])[1]);
	    for (layer=first_layer;layer<=(first_layer+Nlayers);layer++) {
	      
	      ((int16_t*)&txdataF[layer][tti_offset])[0] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //I //b_i
	      *jj = *jj + 1;
	      ((int16_t*)&txdataF[layer][tti_offset])[1] = (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //Q //b_{i+1}
	      *jj = *jj + 1;
	    }
	    break;
	  
	  case 4:  //16QAM
	    if (is_not_UEspecRS(layer,re)) {	  	  
	      qam16_table_offset_re = 0;
	      qam16_table_offset_im = 0;
	      if (output[*jj] == 1)
		qam16_table_offset_re+=2;
	      *jj=*jj+1;
	      if (output[*jj] == 1)
		qam16_table_offset_im+=2;
	      *jj=*jj+1;
	      
	      
	      if (output[*jj] == 1)
		qam16_table_offset_re+=1;
	      *jj=*jj+1;
	      if (output[*jj] == 1)
		qam16_table_offset_im+=1;
	      *jj=*jj+1;
	      
	    
	      for (layer=first_layer;layer<=(first_layer+Nlayers);layer++) {
		((int16_t*)&txdataF[layer][tti_offset])[0] = qam_table_s[qam16_table_offset_re];
		((int16_t*)&txdataF[layer][tti_offset])[1] = qam_table_s[qam16_table_offset_im];
	      }
	    }
	    break;
	    
	  case 6:  //64QAM


	    qam64_table_offset_re = 0;
	    qam64_table_offset_im = 0;
	    
	    if (output[*jj] == 1)
	      qam64_table_offset_re+=4;
	    *jj=*jj+1;
	    if (output[*jj] == 1)
	      qam64_table_offset_im+=4;
	    *jj=*jj+1;
	    if (output[*jj] == 1)
	      qam64_table_offset_re+=2;
	    *jj=*jj+1;
	    if (output[*jj] == 1)
	      qam64_table_offset_im+=2;
	    *jj=*jj+1;
	    if (output[*jj] == 1)
	      qam64_table_offset_re+=1;
	    *jj=*jj+1;
	    if (output[*jj] == 1)
	      qam64_table_offset_im+=1;
	    *jj=*jj+1;
	    
	    for (layer=first_layer;layer<=(first_layer+Nlayers);layer++) {
	      ((int16_t*)&txdataF[layer][tti_offset])[0] = qam_table_s[qam64_table_offset_re];
	      ((int16_t*)&txdataF[layer][tti_offset])[1] = qam_table_s[qam64_table_offset_im];
	    }	  
	    break;
	    
	  }
	}
      }
      else if (mimo_mode>=TM9_10) {
	msg("allocate_REs_in_RB() [dlsch.c] : ERROR, unknown mimo_mode %d\n",mimo_mode);
	return(-1);
      }
    }
      
     
  }

  return(0);
}

int allocate_REs_in_RB_MCH(mod_sym_t **txdataF,
			   uint32_t *jj,
			   uint16_t re_offset,
			   uint32_t symbol_offset,
			   uint8_t *output,
			   uint8_t l,
			   uint8_t mod_order,
			   int16_t amp,
			   int16_t *qam_table_s,
			   uint32_t *re_allocated,
			   uint8_t skip_dc,
			   LTE_DL_FRAME_PARMS *frame_parms) {

  uint32_t tti_offset,aa;
  uint8_t re;
  uint8_t qam64_table_offset_re = 0;
  uint8_t qam64_table_offset_im = 0;
  uint8_t qam16_table_offset_re = 0;
  uint8_t qam16_table_offset_im = 0;
  int16_t gain_lin_QPSK;//,gain_lin_16QAM1,gain_lin_16QAM2;
  int16_t re_off=re_offset;
  gain_lin_QPSK = (int16_t)((amp*ONE_OVER_SQRT2_Q15)>>15);  
  uint8_t first_re,last_re;
  int inc;
#ifdef DEBUG_DLSCH_MODULATION
  printf("allocate_re_MCH (mod %d): symbol_offset %d re_offset %d (%d), jj %d -> %d,%d\n",mod_order,symbol_offset,re_offset,skip_dc,*jj, output[*jj], output[1+*jj]);
#endif

  last_re=12;
  first_re=0;
  inc=1;
  if ((l==2)||(l==10)) {
    inc=2;
    first_re=1;
  }
  else if (l==6) {
    inc=2;
  }
  
  for (re=first_re;re<last_re;re+=inc) {
    
    if ((skip_dc == 1) && (re==(6+first_re)))
      re_off=re_off - frame_parms->ofdm_symbol_size+1;
    
    tti_offset = symbol_offset + re_off + re;
    
    //    printf("re %d (jj %d)\n",re,*jj);
    *re_allocated = *re_allocated + 1;
    
    	
    switch (mod_order) {
    case 2:  //QPSK
      //      printf("%d : %d,%d => ",tti_offset,((int16_t*)&txdataF[0][tti_offset])[0],((int16_t*)&txdataF[0][tti_offset])[1]);
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
	((int16_t*)&txdataF[aa][tti_offset])[0] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //I //b_i
	  *jj = *jj + 1;
	  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
	    ((int16_t*)&txdataF[aa][tti_offset])[1] += (output[*jj]==1) ? (-gain_lin_QPSK) : gain_lin_QPSK; //Q //b_{i+1}
	  *jj = *jj + 1;
	  
	  //  printf("%d,%d\n",((int16_t*)&txdataF[0][tti_offset])[0],((int16_t*)&txdataF[0][tti_offset])[1]);
	  break;
	  
    case 4:  //16QAM
      
      qam16_table_offset_re = 0;
      qam16_table_offset_im = 0;
      if (output[*jj] == 1)
	qam16_table_offset_re+=2;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam16_table_offset_im+=2;
      *jj=*jj+1;
      
      
      if (output[*jj] == 1)
	qam16_table_offset_re+=1;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam16_table_offset_im+=1;
      *jj=*jj+1;
      
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	((int16_t *)&txdataF[aa][tti_offset])[0]+=qam_table_s[qam16_table_offset_re];
	((int16_t *)&txdataF[aa][tti_offset])[1]+=qam_table_s[qam16_table_offset_im];
	//	    ((int16_t *)&txdataF[aa][tti_offset])[0]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_re])>>15);
	//	    ((int16_t *)&txdataF[aa][tti_offset])[1]+=(int16_t)(((int32_t)amp*qam16_table[qam16_table_offset_im])>>15);
      }
      
      break;
      
    case 6:  //64QAM
      
      
      qam64_table_offset_re = 0;
      qam64_table_offset_im = 0;
      
      if (output[*jj] == 1)
	qam64_table_offset_re+=4;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam64_table_offset_im+=4;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam64_table_offset_re+=2;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam64_table_offset_im+=2;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam64_table_offset_re+=1;
      *jj=*jj+1;
      if (output[*jj] == 1)
	qam64_table_offset_im+=1;
      *jj=*jj+1;
      
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	((int16_t *)&txdataF[aa][tti_offset])[0]+=qam_table_s[qam64_table_offset_re];//(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_re])>>15);
	((int16_t *)&txdataF[aa][tti_offset])[1]+=qam_table_s[qam64_table_offset_im];//(int16_t)(((int32_t)amp*qam64_table[qam64_table_offset_im])>>15);
      }
      break;
      
    }
  }           
  return(0);
}

uint8_t get_pmi(uint8_t N_RB_DL,MIMO_mode_t mode,uint32_t pmi_alloc,uint16_t rb) {

  //  printf("Getting pmi for RB %d => %d\n",rb,(pmi_alloc>>((rb>>2)<<1))&3);
  switch (N_RB_DL) {
  case 6:   // 1 PRB per subband
    if (mode <= PUSCH_PRECODING1)
      return((pmi_alloc>>(rb<<1))&3);
    else
      return((pmi_alloc>>rb)&1);    
    break;
  case 25:  // 4 PRBs per subband
    if (mode <= PUSCH_PRECODING1)
      return((pmi_alloc>>((rb>>2)<<1))&3);
    else
      return((pmi_alloc>>(rb>>2))&1);
    break;
  case 50: // 6 PRBs per subband
    if (mode <= PUSCH_PRECODING1)
      return((pmi_alloc>>((rb/6)<<1))&3);
    else
      return((pmi_alloc>>(rb/6))&1);
    break;
  case 100: // 8 PRBs per subband
    if (mode <= PUSCH_PRECODING1)
      return((pmi_alloc>>((rb>>3)<<1))&3);
    else
      return((pmi_alloc>>(rb>>3))&1);    
    break;
  default:
    break;
  }
}


int dlsch_modulation(mod_sym_t **txdataF,
		     int16_t amp,
		     uint32_t subframe_offset,
		     LTE_DL_FRAME_PARMS *frame_parms,
		     uint8_t num_pdcch_symbols,
		     LTE_eNB_DLSCH_t *dlsch){

  uint8_t nsymb;
  uint8_t harq_pid = dlsch->current_harq_pid;
  uint32_t i,jj,re_allocated,symbol_offset;
  uint16_t l,rb,re_offset;
  uint32_t rb_alloc_ind;
  uint32_t *rb_alloc = dlsch->rb_alloc;
  uint8_t pilots=0;
  uint8_t skip_dc,skip_half;
  uint8_t mod_order = get_Qm(dlsch->harq_processes[harq_pid]->mcs);
  int16_t amp_rho_a, amp_rho_b;
  int16_t qam16_table_a[4],qam64_table_a[8],qam16_table_b[4],qam64_table_b[8];
  int16_t *qam_table_s;
  uint8_t Nl = dlsch->harq_processes[harq_pid]->Nl;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ENB_DLSCH_MODULATION, VCD_FUNCTION_IN);

  nsymb = (frame_parms->Ncp==0) ? 14:12;
  
  amp_rho_a = (int16_t)(((int32_t)amp*dlsch->sqrt_rho_a)>>13);
  amp_rho_b = (int16_t)(((int32_t)amp*dlsch->sqrt_rho_b)>>13);

  if (mod_order == 4)
    for (i=0;i<4;i++) {
      qam16_table_a[i] = (int16_t)(((int32_t)qam16_table[i]*amp_rho_a)>>15);
      qam16_table_b[i] = (int16_t)(((int32_t)qam16_table[i]*amp_rho_b)>>15);
    }
  else if (mod_order == 6)
    for (i=0;i<8;i++) {
      qam64_table_a[i] = (int16_t)(((int32_t)qam64_table[i]*amp_rho_a)>>15);
      qam64_table_b[i] = (int16_t)(((int32_t)qam64_table[i]*amp_rho_b)>>15);
    } 
  //Modulation mapping (difference w.r.t. LTE specs)
  
  jj=0;
  re_allocated=0;
  //  printf("num_pdcch_symbols %d, nsymb %d\n",num_pdcch_symbols,nsymb);
  for (l=num_pdcch_symbols;l<nsymb;l++) {
    
#ifdef DEBUG_DLSCH_MODULATION
    msg("Generating DLSCH (harq_pid %d,mimo %d, pmi_alloc %x, mod %d, nu %d, rb_alloc[0] %d) in %d\n",harq_pid,dlsch->harq_processes[harq_pid]->mimo_mode,pmi2hex_2Ar1(dlsch->pmi_alloc),mod_order, dlsch->layer_index, rb_alloc[0], l);
#endif    

    if (frame_parms->Ncp==0) { // normal prefix
      if ((l==4)||(l==11))
	pilots=2;   // pilots in nushift+3, nushift+9
      else if (l==7)
	pilots=1;   // pilots in nushift, nushift+6
      else
	pilots=0;
    }
    else {
      if ((l==3)||(l==9))
	pilots=2;
      else if (l==6)
	pilots=1;
      else
	pilots=0;
    }

    re_offset = frame_parms->first_carrier_offset;
    symbol_offset = (uint32_t)frame_parms->ofdm_symbol_size*(l+(subframe_offset*nsymb));

    //for (aa=0;aa<frame_parms->nb_antennas_tx;aa++)
    //	memset(&txdataF[aa][symbol_offset],0,frame_parms->ofdm_symbol_size<<2);
    //printf("symbol_offset %d,subframe offset %d : pilots %d\n",symbol_offset,subframe_offset,pilots);
      for (rb=0;rb<frame_parms->N_RB_DL;rb++) {

	if (rb < 32)
	  rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
	else if (rb < 64)
	  rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
	else if (rb < 96)
	  rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
	else if (rb < 100)
	  rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
	else
	  rb_alloc_ind = 0;
	
	// check for PBCH
	skip_half=0;
	if ((frame_parms->N_RB_DL&1) == 1) { // ODD N_RB_DL

	  if ((rb==frame_parms->N_RB_DL>>1))
	    skip_dc = 1;
	  else
	    skip_dc = 0;
	  // PBCH
	  if ((subframe_offset==0) && 
	      (rb>((frame_parms->N_RB_DL>>1)-3)) && 
	      (rb<((frame_parms->N_RB_DL>>1)+3)) && 
	      (l>=(nsymb>>1)) && 
	      (l<((nsymb>>1) + 4))) {
	    rb_alloc_ind = 0;
	  }
	  //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
	  if ((subframe_offset==0) && 
	      (rb==((frame_parms->N_RB_DL>>1)-3)) && 
	      (l>=(nsymb>>1)) && 
	      (l<((nsymb>>1) + 4)))
	    skip_half=1;
	  else if ((subframe_offset==0) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
	    skip_half=2;
	
	  if (frame_parms->frame_type == TDD) { // TDD
	    //SSS TDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==(nsymb-1)) ) {
	      rb_alloc_ind = 0;
	    }
	    //SSS TDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==(nsymb-1)))
	      skip_half=1;
	    else if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==(nsymb-1)))
	      skip_half=2;
	    
	    //PSS TDD
	    if (((subframe_offset==1) || (subframe_offset==6)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==2) ) {
	      rb_alloc_ind = 0;
	    }
	    //PSS TDD
	    if (((subframe_offset==1)||(subframe_offset==6)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==2))
	      skip_half=1;
	    else if (((subframe_offset==1)||(subframe_offset==6)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==2))
	      skip_half=2;
	  }
	  else {
	    //PSS FDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && 
		(rb>((frame_parms->N_RB_DL>>1)-3)) && 
		(rb<((frame_parms->N_RB_DL>>1)+3)) && 
		(l==((nsymb>>1)-1)) ) {
	      rb_alloc_ind = 0;
	    }
	    //PSS FDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==((nsymb>>1)-1)))
	      skip_half=1;
	    else if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==(((nsymb>>1)-1))))
	      skip_half=2;

	    //SSS FDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==((nsymb>>1)-2)) ) {
	      rb_alloc_ind = 0;
	    }
	    //SSS FDD
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && ((l==((nsymb>>1)-2))))
	      skip_half=1;
	    else if (((subframe_offset==0)||(subframe_offset==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && ((l==(nsymb>>1)-2)))
	      skip_half=2;

	  }
	  
	}
	else {  // EVEN N_RB_DL
	  //PBCH
	  if ((subframe_offset==0) && 
	      (rb>=((frame_parms->N_RB_DL>>1)-3)) && 
	      (rb<((frame_parms->N_RB_DL>>1)+3)) && 
	      (l>=nsymb>>1) && (l<((nsymb>>1) + 4)))
	    rb_alloc_ind = 0;
	  skip_dc=0;
	  skip_half=0;
	  
	  if (frame_parms->frame_type == TDD) { // TDD
	    //SSS
	    if (((subframe_offset==0)||
		 (subframe_offset==5)) && 
		(rb>=((frame_parms->N_RB_DL>>1)-3)) && 
		(rb<((frame_parms->N_RB_DL>>1)+3)) && 
		(l==nsymb-1) ) {
	      rb_alloc_ind = 0;
	    }	    
	    //PSS
	    if (((subframe_offset==1)||
		 (subframe_offset==6)) && 
		(rb>=((frame_parms->N_RB_DL>>1)-3)) && 
		(rb<((frame_parms->N_RB_DL>>1)+3)) && 
		(l==2) ) {
	      rb_alloc_ind = 0;
	    }
	  }
	  else { // FDD
	    //SSS
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==((nsymb>>1)-2)) ) {
	      rb_alloc_ind = 0;
	    }	    
	    //PSS
	    if (((subframe_offset==0)||(subframe_offset==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==((nsymb>>1)-1)) ) {
	      rb_alloc_ind = 0;
	    }
	  }
	}
	
	if (dlsch->layer_index>1) {
	  msg("layer_index %d: re_offset %d, symbol %d offset %d\n",dlsch->layer_index,re_offset,l,symbol_offset); 
	  return(-1);
	}
	if (mod_order == 4)
	  qam_table_s = ((pilots) ? qam16_table_b : qam16_table_a);
	else if (mod_order == 6)
	  qam_table_s = ((pilots) ? qam64_table_b : qam64_table_a);
	else
	  qam_table_s = NULL;

	if (rb_alloc_ind > 0)
        {
	  //	  printf("Allocated rb %d, subframe_offset %d, symbol_offset %d, re_offset %d\n",rb,subframe_offset,symbol_offset,re_offset);
	  allocate_REs_in_RB(txdataF,
			     &jj,
			     re_offset,
			     symbol_offset,
			     dlsch->e,
			     dlsch->harq_processes[harq_pid]->mimo_mode,
			     dlsch->layer_index,
			     pilots,
			     mod_order,
			     get_pmi(frame_parms->N_RB_DL,dlsch->harq_processes[harq_pid]->mimo_mode,dlsch->pmi_alloc,rb),
			     ((pilots) ? amp_rho_b : amp_rho_a),
			     qam_table_s,
			     &re_allocated,
			     skip_dc,
			     skip_half,
			     (frame_parms->mode1_flag==1)?1:0,
			     dlsch->harq_processes[harq_pid]->Nlayers, 
			     dlsch->harq_processes[harq_pid]->first_layer, 
			     frame_parms);
      }
	re_offset+=12; // go to next RB
	

	// check if we crossed the symbol boundary and skip DC
	if (re_offset >= frame_parms->ofdm_symbol_size) {
	  if (skip_dc == 0)  //even number of RBs (doesn't straddle DC)
	    re_offset=1;
	  else
	    re_offset=7;  // odd number of RBs
	}
      }
	
  }
  


#ifdef DEBUG_DLSCH_MODULATION
  msg("generate_dlsch : jj = %d,re_allocated = %d (G %d)\n",jj,re_allocated,get_G(frame_parms,dlsch->nb_rb,dlsch->rb_alloc,mod_order,Nl,2,0,subframe_offset));
#endif
  
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ENB_DLSCH_MODULATION, VCD_FUNCTION_OUT);

  return (re_allocated);
}

int mch_modulation(mod_sym_t **txdataF,
		   int16_t amp,
		   uint32_t subframe_offset,
		   LTE_DL_FRAME_PARMS *frame_parms,
		   LTE_eNB_DLSCH_t *dlsch){
  
  uint8_t nsymb,nsymb_pmch;
  uint32_t i,jj,re_allocated,symbol_offset;
  uint16_t l,rb,re_offset;
  uint8_t skip_dc=0;
  uint8_t mod_order = get_Qm(dlsch->harq_processes[0]->mcs);
  int16_t qam16_table_a[4],qam64_table_a[8];//,qam16_table_b[4],qam64_table_b[8];
  int16_t *qam_table_s;

  nsymb_pmch = 12;
  nsymb = (frame_parms->Ncp == NORMAL) ? 14 : 12;
  if (mod_order == 4)
    for (i=0;i<4;i++) {
      qam16_table_a[i] = (int16_t)(((int32_t)qam16_table[i]*amp)>>15);
    }
  else if (mod_order == 6)
    for (i=0;i<8;i++) {
      qam64_table_a[i] = (int16_t)(((int32_t)qam64_table[i]*amp)>>15);
    } 
  
   jj=0;
  re_allocated=0;
  //  printf("num_pdcch_symbols %d, nsymb %d\n",num_pdcch_symbols,nsymb);
  for (l=2;l<nsymb_pmch;l++) {
    
#ifdef DEBUG_DLSCH_MODULATION
    msg("Generating MCH (mod %d) in %d\n",mod_order, l);
#endif    

    re_offset = frame_parms->first_carrier_offset;
    symbol_offset = (uint32_t)frame_parms->ofdm_symbol_size*(l+(subframe_offset*nsymb));

    for (rb=0;rb<frame_parms->N_RB_DL;rb++) {
      

      if ((frame_parms->N_RB_DL&1) == 1) { // ODD N_RB_DL
	
	if ((rb==frame_parms->N_RB_DL>>1))
	  skip_dc = 1;
	else
	  skip_dc = 0;

      }

      
      if (mod_order == 4)
	qam_table_s = qam16_table_a;
      else if (mod_order == 6)
	qam_table_s = qam64_table_a;
      else
	qam_table_s = NULL;

      //      printf("Allocated rb %d, subframe_offset %d\n",rb,subframe_offset);
      allocate_REs_in_RB_MCH(txdataF,
			     &jj,
			     re_offset,
			     symbol_offset,
			     dlsch->e,
			     l,
			     mod_order,
			     amp,
			     qam_table_s,
			     &re_allocated,
			     skip_dc,
			     frame_parms);
      
      re_offset+=12; // go to next RB
	
      
      // check if we crossed the symbol boundary and skip DC
      if (re_offset >= frame_parms->ofdm_symbol_size) {
	if (skip_dc == 0)  //even number of RBs (doesn't straddle DC)
	  re_offset=1;
	else
	  re_offset=7;  // odd number of RBs
      }
    }
  }
	

  


#ifdef DEBUG_DLSCH_MODULATION
  msg("generate_dlsch(MCH) : jj = %d,re_allocated = %d (G %d)\n",jj,re_allocated,get_G(frame_parms,dlsch->nb_rb,dlsch->rb_alloc,mod_order,1,2,0,subframe_offset));
#endif
    
    return (re_allocated);
}
