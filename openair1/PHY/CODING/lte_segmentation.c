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
/* file: lte_segmentation.c
   purpose: Procedures for transport block segmentation for LTE (turbo-coded transport channels) 
   author: raymond.knopp@eurecom.fr
   date: 21.10.2009 
*/
#include "PHY/defs.h"
#include "SCHED/extern.h"

//#define DEBUG_SEGMENTATION

int lte_segmentation(unsigned char *input_buffer,
		      unsigned char **output_buffers,
		      unsigned int B,
		      unsigned int *C,
		      unsigned int *Cplus,
		      unsigned int *Cminus,
		      unsigned int *Kplus,
		      unsigned int *Kminus,
		      unsigned int *F) {

  unsigned int L,Bprime,Bprime_by_C,r,Kr,k,s,crc;

  if (B<=6144) {
    L=0;
    *C=1;
    Bprime=B;
  }
  else {
    L=24;
    *C = B/(6144-L);
    if ((6144-L)*(*C) < B)
      *C=*C+1;
    Bprime = B+((*C)*L); 
#ifdef DEBUG_SEGMENTATION
    printf("Bprime %d\n",Bprime);
#endif
  }
  if ((*C)>MAX_NUM_DLSCH_SEGMENTS) {
    msg("lte_segmentation.c: too many segments %d\n",*C);
    return(-1);
  }

  // Find K+
  Bprime_by_C  = Bprime/(*C);
#ifdef DEBUG_SEGMENTATION
  printf("Bprime_by_C %d\n",Bprime_by_C);
#endif
    //  Bprime = Bprime_by_C>>3;

  if (Bprime_by_C <= 40) {
    *Kplus = 40;
    *Kminus = 0;
  }
  else if (Bprime_by_C<=512) { // increase by 1 byte til here
    *Kplus = (Bprime_by_C>>3)<<3;
    *Kminus = Bprime_by_C-8;
  }
  else if (Bprime_by_C <=1024) {// increase by 2 bytes til here
    *Kplus = (Bprime_by_C>>4)<<4;
    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 16;
    *Kminus = (*Kplus - 16);
  }
  else if (Bprime_by_C <= 2048) { // increase by 4 bytes til here
    *Kplus = (Bprime_by_C>>5)<<5;
    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 32;
    *Kminus = (*Kplus - 32);
  }
  else if (Bprime_by_C <=6144 ) { // increase by 8 bytes til here

   *Kplus = (Bprime_by_C>>6)<<6;
#ifdef DEBUG_SEGMENTATION
    printf("Bprime_by_C_by_C %d , Kplus %d\n",Bprime_by_C,*Kplus);
#endif 
    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 64;
#ifdef DEBUG_SEGMENTATION
    printf("Bprime_by_C_by_C %d , Kplus2 %d\n",Bprime_by_C,*Kplus);
#endif
    *Kminus = (*Kplus - 64);
  }
  else {
    msg("lte_segmentation.c: Illegal codeword size !!!\n");
    return(-1);
  }  

  if (*C == 1) {
    *Cplus = *C;
    *Kminus = 0;
    *Cminus = 0;
  }
  else {

    //    printf("More than one segment (%d), exiting \n",*C);
    //    exit(-1);
    *Cminus = ((*C)*(*Kplus) - (Bprime))/((*Kplus) - (*Kminus));
    *Cplus  = (*C) - (*Cminus);
  }



  *F = ((*Cplus)*(*Kplus) + (*Cminus)*(*Kminus) - (Bprime));
#ifdef DEBUG_SEGMENTATION
  printf("C %d, Cplus %d, Cminus %d, Kplus %d, Kminus %d, Bprime_bytes %d, Bprime %d, F %d\n",*C,*Cplus,*Cminus,*Kplus,*Kminus,Bprime>>3,Bprime,*F);
#endif
  if ((input_buffer) && (output_buffers)) {

    for (k=0;k<*F>>3;k++) {
      output_buffers[0][k] = 0;
    } 
    s=0;
    for (r=0;r<*C;r++) {

      if (r<*Cminus)
	Kr = *Kminus;
      else
	Kr = *Kplus;

      while (k<((Kr - L)>>3)) {
	output_buffers[r][k] = input_buffer[s];
	//	printf("encoding segment %d : byte %d => %d\n",r,k,input_buffer[s]);
	k++;
	s++;
      }

      if (*C > 1) { // add CRC
	crc = crc24b(output_buffers[r],Kr-24)>>8;
	output_buffers[r][(Kr-24)>>3] = ((uint8_t*)&crc)[2];
	output_buffers[r][1+((Kr-24)>>3)] = ((uint8_t*)&crc)[1];
	output_buffers[r][2+((Kr-24)>>3)] = ((uint8_t*)&crc)[0];
#ifdef DEBUG_SEGMENTATION
	printf("Segment %d : CRC %x\n",r,crc);
#endif
      }
      k=0;
    }
  }
  return(0);
}

// uint8_t eNB_id,uint8_t harq_pid, uint8_t UE_id,
int16_t estimate_ue_tx_power(uint32_t tbs, uint32_t nb_rb, uint8_t control_only, lte_prefix_type_t ncp, uint8_t use_srs){

  /// The payload + CRC size in bits, "B" 
  uint32_t B;  
  /// Number of code segments 
  uint32_t C;                         
  /// Number of "small" code segments 
  uint32_t Cminus;                    
  /// Number of "large" code segments 
  uint32_t Cplus;                     
  /// Number of bits in "small" code segments (<6144) 
  uint32_t Kminus;                    
  /// Number of bits in "large" code segments (<6144) 
  uint32_t Kplus;                     
  /// Total number of bits across all segments
  uint32_t sumKr;
  /// Number of "Filler" bits 
  uint32_t F;                         
  // num resource elements
  uint32_t num_re=0.0;
  // num symbols
  uint32_t num_symb=0.0;
  /// effective spectral efficiency of the PUSCH
  uint32_t MPR_x100=0; 
  /// beta_offset
  uint16_t beta_offset_pusch_x8=8;
  /// delta mcs
  float delta_mcs=0.0;
  /// bandwidth factor 
  float bw_factor=0.0;

  B= tbs+24;
  lte_segmentation(NULL,
		   NULL,
		   B,
		   &C,
		   &Cplus,
		   &Cminus,
		   &Kplus,
		   &Kminus,		     
		   &F);
 

  sumKr = Cminus*Kminus + Cplus*Kplus;    
  num_symb = 12-(ncp<<1)-(use_srs==0?0:1);
  num_re = num_symb * nb_rb * 12;
  
  if (num_re == 0)
    return(0);

  MPR_x100 = 100*sumKr/num_re;
  if (control_only == 1 )
    beta_offset_pusch_x8=8; // fixme
  //(beta_offset_pusch_x8=phy_vars_ue->ulsch_ue[eNB_id]->harq_processes[harq_pid]->control_only == 1) ? phy_vars_ue->ulsch_ue[eNB_id]->beta_offset_cqi_times8:8;

  // if deltamcs_enabledm
  delta_mcs = ((hundred_times_delta_TF[MPR_x100/6]+10*dB_fixed_times10((beta_offset_pusch_x8)>>3))/100.0);
  bw_factor = (hundred_times_log10_NPRB[nb_rb-1]/100.0);
#ifdef DEBUG_SEGMENTATION
 printf("estimated ue tx power %d (num_re %d, sumKr %d, mpr_x100 %d, delta_mcs %f, bw_factor %f)\n", 
	(int16_t)ceil(delta_mcs + bw_factor), num_re, sumKr, MPR_x100, delta_mcs, bw_factor);
#endif 
  return (int16_t)ceil(delta_mcs + bw_factor);
  
}

#ifdef MAIN
main() {

  unsigned int Kplus,Kminus,C,Cplus,Cminus,F,Bbytes;
  
  for (Bbytes=5;Bbytes<2*768;Bbytes++) {
    lte_segmentation(0,0,Bbytes<<3,&C,&Cplus,&Cminus,&Kplus,&Kminus,&F);
    printf("Bbytes %d : C %d, Cplus %d, Cminus %d, Kplus %d, Kminus %d, F %d\n",
	   Bbytes, C, Cplus, Cminus, Kplus, Kminus, F);
  }
}
#endif
