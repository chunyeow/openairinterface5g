#include "defs.h"
#include <stdint.h>
#include "PHY/TOOLS/defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EXECTIME
#ifdef RTAI
#include <rtai_lxrt.h>
#include <rtai_sem.h>
#include <rtai_msg.h>
#else
#include <time.h>
#endif
#endif

#define TWO_OVER_SQRT_10 20724
#define FOUR_OVER_SQRT_42 20225
#define TWO_OVER_SQRT_42 10112

extern int16_t twiddle_fft64[63*4*2];
extern int16_t twiddle_ifft64[63*4*2];

extern uint16_t rev64[64];
extern int Ndbps[8];
extern int Ncbps[8];

//#define DEBUG_DATA 1

extern int16_t chest[256] __attribute__((aligned(16)));
extern int interleaver_bpsk[48];
extern int interleaver_qpsk[48];
extern int interleaver_16qam[48];
extern int interleaver_64qam[48];

extern uint8_t scrambler[127*8];

#ifdef EXECTIME
#ifdef RTAI
RTIME dd_t1=0,dd_t2=0,dd_t3=0,dd_t4=0;
#else
long dd_t1=0,dd_t2=0,dd_t3=0,dd_t4=0;
#endif
int dd_trials=0;

#ifdef RTAI
extern unsigned int *DAQ_MBOX;
#endif

int16_t Pseq_rx[127]     = { 1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
			     -1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1, 
			     1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
			     -1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
			     -1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
			     -1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
			     -1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
			     -1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1};

int32_t rxDATA_F_comp_aggreg2[48*1024];
int32_t rxDATA_F_comp_aggreg3[48*1024];

void print_dd_stats() {

  if (dd_trials>0)
    printf("Data detection                   : Trials %d, dd_t1 (64pt FFT) %d ns, dd_t2 %d ns (Ch Comp.), dd_t3 %d ns (Deinter), dd_t4 %d ns (Viterbi)\n",dd_trials,
	   (int)dd_t1/dd_trials,(int)dd_t2/dd_trials,(int)dd_t3/dd_trials,(int)dd_t4/dd_trials);
  dd_trials=0;
  dd_t1=0;
  dd_t2=0;
  dd_t3=0;
  dd_t4=0;
}
#endif

int data_detection(RX_VECTOR_t *rxv,uint8_t *data_ind,uint32_t* rx_data,int frame_length,int rx_offset,int log2_maxh,int (*wait(int,int))) {

  uint32_t pilot1,pilot2,pilot3,pilot4;

  int16_t rxDATA_F[128*2] __attribute__((aligned(16)));
  uint32_t rxDATA_F_comp[64*2] __attribute__((aligned(16)));
  uint32_t rxDATA_F_comp2[48] __attribute__((aligned(16)));
  uint32_t rxDATA_F_comp3[48] __attribute__((aligned(16)));

  int8_t rxDATA_llr[384] __attribute__((aligned(16)));
  int8_t rxDATA_llr2[432] __attribute__((aligned(16)));
  int16_t rxDATA_F_mag[48];
  int16_t rxDATA_F_mag2[48];
  int8_t *llr_ptr;
  int i,j,k,k2,tmp,pos;
  int *interleaver;
  int dlen,dlen_symb;
  int s,sprime;
  uint32_t crc_rx;
  char fname[30],vname[30];
  int32_t scale;
  int ret;
  int32_t cfo_re32,cfo_im32;
  int32_t cfo_Q15;
  int rx_offset2;

#ifdef RTAI
  int mbox_off = 0,old_mbox;
#endif
#ifdef EXECTIME
#ifdef RTAI
  RTIME tin,tout;
#else
  struct timespec tin,tout;
#endif
#endif

  // loop over all symbols
  dlen      = 32+16+6+(rxv->sdu_length<<3); // data length is 32-bits CRC + sdu + 16 service + 6 tail
  dlen_symb = dlen/Ndbps[rxv->rate];
  if ((dlen%Ndbps[rxv->rate])>0)
    dlen_symb++;
#ifdef RTAI
  old_mbox = ((unsigned int *)DAQ_MBOX)[0];
#endif
  for (s=0,sprime=1;s<dlen_symb;s++,sprime++,rx_offset+=80) {
#ifdef DEBUG_DATA
    printf("DATA symbol %d, rx_offset %d\n",s,rx_offset);
#endif
    // synchronize to HW if needed
#ifdef RTAI
    rx_offset2=rx_offset+1024;
    if (rx_offset2 > frame_length)
      rx_offset2 -= frame_length;

    if (old_mbox > ((unsigned int *)DAQ_MBOX)[0])
      mbox_off = 150;
    //    printf("dd: s %d (%d), old_mbox %d, new_mbox %d\n",
    //	       s,rx_offset,old_mbox,((unsigned int *)DAQ_MBOX)[0]);
    old_mbox = ((unsigned int *)DAQ_MBOX)[0];

    while (((unsigned int *)DAQ_MBOX)[0]+mbox_off < (rx_offset2>>9) ) {
      //      printf("sleeping\n");
      rt_sleep(nano2count(66666));
    }
#endif

#ifdef EXECTIME
    dd_trials++;
#endif

    if (rx_offset>frame_length) {
      rx_offset -= frame_length;
#ifdef RTAI
      mbox_off = 0;
#endif
    }
    // index for pilot symbol lookup
    if (sprime==127)
      sprime=0;
    
    if ((rx_offset+80) > frame_length)
      memcpy((void *)(rx_data+frame_length),
	     (void *)rx_data,
	     2*sizeof(int16_t)*(rx_offset+80-frame_length));
    

#ifdef EXECTIME
#ifdef RTAI
    tin=rt_get_time_ns();
#else
    ret=clock_gettime(CLOCK_REALTIME,&tin);
#endif
#endif
    fft((int16_t *)(rx_data+rx_offset+16),         /// complex input
	rxDATA_F,           /// complex output
	&twiddle_fft64[0],  /// complex twiddle factors
	rev64,           /// bit reversed permutation vector
	6,               /// log2(FFT_SIZE)
	3,               /// scale (energy normalized for 64-point)
	0);              /// 0 means 64-bit complex interleaved format else complex-multiply ready repeated format
#ifdef EXECTIME
#ifdef RTAI
    tout=rt_get_time_ns();
    dd_t1 += (tout-tin);
    tin=rt_get_time_ns();
#else
    ret=clock_gettime(CLOCK_REALTIME,&tout);
    dd_t1 += (tout.tv_nsec-tin.tv_nsec);
    
    ret=clock_gettime(CLOCK_REALTIME,&tin);
#endif
#endif

    //        log2_maxh=7;
    mult_cpx_vector_norep_unprepared_conjx2(rxDATA_F,(int16_t*)chest,(int16_t*)rxDATA_F_comp,64,log2_maxh);
#ifdef EXECTIME
#ifdef RTAI
    tout=rt_get_time_ns();
    dd_t2 += (tout-tin);
    tin=rt_get_time_ns();
#else
    ret=clock_gettime(CLOCK_REALTIME,&tout);
    dd_t2 += (tout.tv_nsec-tin.tv_nsec);
    
    ret=clock_gettime(CLOCK_REALTIME,&tin);
#endif   
#endif
 
    if ((s==0)&&((rxv->rate>>1)>1)) {  // Compute channel magnitude for first symbol when 16QAM or 64QAM
      for (i=0;i<5;i++)
	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(38+i)<<2]*(int32_t)chest[(38+i)<<2]) + ((int32_t)chest[1+((38+i)<<2)]*(int32_t)chest[1+((38+i)<<2)]))>>log2_maxh);
      for (;i<18;i++)
	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(38+1+i)<<2]*(int32_t)chest[(38+1+i)<<2] + (int32_t)chest[1+((38+1+i)<<2)]*(int32_t)chest[1+((38+1+i)<<2)]))>>log2_maxh);
      
      for (;i<24;i++)
      	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(38+2+i)<<2]*(int32_t)chest[(38+2+i)<<2] + (int32_t)chest[1+((38+2+i)<<2)]*(int32_t)chest[1+((38+2+i)<<2)]))>>log2_maxh);
      // +ve portion
      for (;i<30;i++)
      	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(-24+1+i)<<2]*(int32_t)chest[(-24+1+i)<<2] + (int32_t)chest[1+((-24+1+i)<<2)]*(int32_t)chest[1+((-24+1+i)<<2)]))>>log2_maxh);

      for (;i<43;i++)
      	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(-24+2+i)<<2]*(int32_t)chest[(-24+2+i)<<2] + (int32_t)chest[1+((-24+2+i)<<2)]*(int32_t)chest[1+((-24+2+i)<<2)]))>>log2_maxh);

      for (;i<48;i++)
      	rxDATA_F_mag[i] = (int16_t)((((int32_t)chest[(-24+3+i)<<2]*(int32_t)chest[(-24+3+i)<<2] + (int32_t)chest[1+((-24+3+i)<<2)]*(int32_t)chest[1+((-24+3+i)<<2)]))>>log2_maxh);

      if ((rxv->rate>>1) == 2)
	scale = TWO_OVER_SQRT_10;
      else
	scale = FOUR_OVER_SQRT_42;

      for (i=0;i<48;i++) {
	//		printf("mag[%d] = %d\n",i,rxDATA_F_mag[i]);
	rxDATA_F_mag[i] = (int16_t)((rxDATA_F_mag[i]*scale)>>15);
      }
      if ((rxv->rate>>1) == 3)
	for (i=0;i<48;i++)
	  rxDATA_F_mag2[i] = (int16_t)((rxDATA_F_mag[i]*TWO_OVER_SQRT_42)>>15);
    }

    // extract 48 statistics and 4 pilot symbols 
    // -ve portion
    for (i=0;i<5;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(38+i)];
    pilot1 = ((uint32_t*)rxDATA_F)[(38+5)<<1];
    
    for (;i<18;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(38+1+i)];
    pilot2 = ((uint32_t*)rxDATA_F)[(38+19)<<1];
    
    for (;i<24;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(38+2+i)];
    
    // +ve portion
    for (;i<30;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(-24+1+i)];
    pilot3 = ((uint32_t*)rxDATA_F)[(6+1)<<1];
    for (;i<43;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(-24+2+i)];
    pilot4 = ((uint32_t*)rxDATA_F)[(19+2)<<1];((int16_t *)&pilot4)[0]=-((int16_t *)&pilot4)[0];((int16_t *)&pilot4)[1]=-((int16_t *)&pilot4)[1];
    for (;i<48;i++)
      rxDATA_F_comp2[i] = rxDATA_F_comp[(-24+3+i)];
    



    // CFO compensation
    
    cfo_re32 = ((((int16_t *)&pilot1)[0]*(int32_t)chest[(38+5)<<2]) + 
		(((int16_t *)&pilot1)[1]*(int32_t)chest[1+((38+5)<<2)]))>>2;
    cfo_im32 = (((((int16_t *)&pilot1)[1]*(int32_t)chest[(38+5)<<2]) - 
		 (((int16_t *)&pilot1)[0]*(int32_t)chest[1+((38+5)<<2)])))>>2;
    
    
    cfo_re32 += (((((int16_t *)&pilot2)[0]*(int32_t)chest[(38+19)<<2]) + 
		 (((int16_t *)&pilot2)[1]*(int32_t)chest[1+((38+19)<<2)])))>>2;
    cfo_im32 += (((((int16_t *)&pilot2)[1]*(int32_t)chest[(38+19)<<2]) - 
		   (((int16_t *)&pilot2)[0]*(int32_t)chest[1+((38+19)<<2)])))>>2;

    cfo_re32 += (((((int16_t *)&pilot3)[0]*(int32_t)chest[(6+1)<<2]) + 
		  (((int16_t *)&pilot3)[1]*(int32_t)chest[1+((6+1)<<2)])))>>2;
    cfo_im32 += (((((int16_t *)&pilot3)[1]*(int32_t)chest[(6+1)<<2]) - 
		   (((int16_t *)&pilot3)[0]*(int32_t)chest[1+((6+1)<<2)])))>>2;

    cfo_re32 += (((((int16_t *)&pilot4)[0]*(int32_t)chest[(19+2)<<2]) + 
		  (((int16_t *)&pilot4)[1]*(int32_t)chest[1+((19+2)<<2)])))>>2;
    cfo_im32 += (((((int16_t *)&pilot4)[1]*(int32_t)chest[(19+2)<<2]) - 
		   (((int16_t *)&pilot4)[0]*(int32_t)chest[1+((19+2)<<2)])))>>2;
    
    ((int16_t*)&cfo_Q15)[0] = (int16_t)(cfo_re32>>((log2_maxh)))*Pseq_rx[sprime];
    ((int16_t*)&cfo_Q15)[1] = -(int16_t)(cfo_im32>>((log2_maxh)))*Pseq_rx[sprime];
#ifdef DEBUG_DATA  
    printf("dd: s %d, p=[%d+(%d)*j , %d+(%d)*j , %d+(%d)*j , %d+(%d)*j] * ch =[%d+(%d)*j , %d+(%d)*j , %d+(%d)*j , %d+(%d)*j] => cfo_Q15 (%d,%d), CFO32 (%d,%d)\n",s,
	   ((int16_t *)&pilot1)[0]*Pseq_rx[sprime],((int16_t *)&pilot1)[1]*Pseq_rx[sprime],
	   ((int16_t *)&pilot2)[0]*Pseq_rx[sprime],((int16_t *)&pilot2)[1]*Pseq_rx[sprime],
	   ((int16_t *)&pilot3)[0]*Pseq_rx[sprime],((int16_t *)&pilot3)[1]*Pseq_rx[sprime],
	   ((int16_t *)&pilot4)[0]*Pseq_rx[sprime],((int16_t *)&pilot4)[1]*Pseq_rx[sprime],
	   chest[(38+5)<<2],chest[1+((38+5)<<2)],
	   chest[(38+19)<<2],chest[1+((38+19)<<2)],
	   chest[(6+1)<<2],chest[1+((6+1)<<2)],
	   chest[(19+2)<<2],chest[1+((19+2)<<2)],
	   ((int16_t*)&cfo_Q15)[0],((int16_t*)&cfo_Q15)[1],
	   cfo_re32,cfo_im32);
#endif

    rotate_cpx_vector_norep(rxDATA_F_comp2,&cfo_Q15,rxDATA_F_comp3,48,log2_maxh>>1);

#ifdef DEBUG_DATA
    if (s==0)
      write_output("rxDATA_F_mag.m","rxDAT_mag",rxDATA_F_mag,48,1,0);
    sprintf(fname,"rxDATA_F%d.m",s);
    sprintf(vname,"rxDAT_F_%d",s);
    write_output(fname,vname, rxDATA_F,128,2,1);
    write_output("rxDATA_F_comp.m","rxDAT_F_comp", rxDATA_F_comp,64,1,1);
    sprintf(fname,"rxDATA_F_comp2_%d.m",s);
    sprintf(vname,"rxDAT_F_comp2_%d",s);
    write_output(fname,vname, rxDATA_F_comp2,48,1,1);
    sprintf(fname,"rxDATA_F_comp3_%d.m",s);
    sprintf(vname,"rxDAT_F_comp3_%d",s);
    write_output(fname,vname, rxDATA_F_comp3,48,1,1);
#endif
    // LLR Computation

    for (i=0;i<48;i++) {
      rxDATA_F_comp_aggreg3[(48*s) + i] = rxDATA_F_comp3[i];
      rxDATA_F_comp_aggreg2[(48*s) + i] = rxDATA_F_comp2[i];
    }

    switch (rxv->rate>>1) {
    case 0: // BPSK
      llr_ptr = rxDATA_llr;
      memset(rxDATA_llr,0,48);
      for (k=0;k<48;k++) {
	pos = interleaver_bpsk[k];
	tmp = ((int16_t*)rxDATA_F_comp3)[pos<<1]>>4;
	if (tmp<-8)
	  rxDATA_llr[k] = -8;
	else if (tmp>7)
	  rxDATA_llr[k] = 7;
	else
	  rxDATA_llr[k] = (int8_t)tmp;

	//	printf("rxDATA_coded %d(%d) : %d\n",k,pos,rxDATA_llr[k]);
      }
      if (rxv->rate==1) { // rate 3/4, so add zeros for punctured bits
	llr_ptr = rxDATA_llr2;
	memset(rxDATA_llr2,0,72);
	for (k=0,k2=0;k<48;k++,k2++) {
	  rxDATA_llr2[k2] = rxDATA_llr[k];
	  if ((k&3) == 2)
	    k2+=2;
	}
      }
      break;
    case 1:  // QPSK
      llr_ptr = rxDATA_llr;
      memset(rxDATA_llr,0,96);
      for (k=0;k<96;k++) {
	pos = interleaver_qpsk[k];
	tmp = ((int16_t*)rxDATA_F_comp3)[pos]>>4;

	if (tmp<-8)
	  rxDATA_llr[k] = -8;
	else if (tmp>7)
	  rxDATA_llr[k] = 7;
	else
	  rxDATA_llr[k] = (int8_t)tmp;
      }
      if (rxv->rate==3) { // rate 3/4, so add zeros for punctured bits
	llr_ptr = rxDATA_llr2;
	memset(rxDATA_llr2,0,144);
	for (k=0,k2=0;k<96;k++,k2++) {
	  rxDATA_llr2[k2] = rxDATA_llr[k];
	  if ((k&3) == 2)
	    k2+=2;
	}
      }
      break;
    case 2:  // 16QAM
      llr_ptr = rxDATA_llr;
      memset(rxDATA_llr,0,192);
      for (k=0;k<192;k++) {
	pos = interleaver_16qam[k];
	//	printf("k %d, pos %d\n",k,pos);
	if ((pos&1)==1) {
	  tmp = ((int16_t*)rxDATA_F_comp2)[pos>>1]>>4;
	  //	  printf("pos (msb) %d : %d\n",pos>>1,tmp);
	}
	else {
	  tmp = ((int16_t*)rxDATA_F_comp2)[pos>>1];
	  tmp = (tmp<0)? tmp : -tmp;
	  tmp = (tmp + rxDATA_F_mag[pos>>2])>>4;
	  //	  printf("pos (lsb) %d : rxDATA_F_mag[%d] %d : %d (%d)\n",pos>>1,pos>>2,rxDATA_F_mag[pos>>2],tmp,((int16_t*)rxDATA_F_comp2)[pos>>1]);
	}
	
	if (tmp<-8)
	  rxDATA_llr[k] = -8;
	else if (tmp>7)
	  rxDATA_llr[k] = 7;
	else
	  rxDATA_llr[k] = (int8_t)tmp;
	/*
	if (tmp<-128)
	  rxDATA_llr[k] = -128;
	else if (tmp>127)
	  rxDATA_llr[k] = 127;
	else
	  rxDATA_llr[k] = (int8_t)tmp;
	*/
      }
      if (rxv->rate==5) { // rate 3/4, so add zeros for punctured bits
	llr_ptr = rxDATA_llr2;
	memset(rxDATA_llr2,0,288);
	for (k=0,k2=0;k<192;k++,k2++) {
	  rxDATA_llr2[k2] = rxDATA_llr[k];
	  if ((k&3) == 2)
	    k2+=2;
	}
      }   
#ifdef DEBUG_DATA  
      sprintf(fname,"rxDATA_llr_%d.m",s);
      sprintf(vname,"rxDAT_llr_%d",s);
      write_output(fname,vname, rxDATA_llr,192,1,4);
#endif
      break;
    case 3:  // 64QAM
      return(0==1);
      break;
    }
    /*    
    printf("LLRs:"); 
    for (i=0;i<48;i++)
      printf("%d,",llr_ptr[i]);

      printf("viterbi s %d/%d\n",s,dlen_symb-1);
    */
#ifdef EXECTIME
#ifdef RTAI
    tout=rt_get_time_ns();
    dd_t3 += (tout-tin);
    tin=rt_get_time_ns();
#else
    ret=clock_gettime(CLOCK_REALTIME,&tout);
    dd_t3 += (tout.tv_nsec-tin.tv_nsec);
    
    ret=clock_gettime(CLOCK_REALTIME,&tin);
#endif
#endif

    if (s < (dlen_symb-1))
      phy_viterbi_dot11_sse2(llr_ptr,data_ind,Ndbps[rxv->rate],s*Ndbps[rxv->rate],0);
    else {
      phy_viterbi_dot11_sse2(llr_ptr,data_ind,Ndbps[rxv->rate],s*Ndbps[rxv->rate],1);
      
      // scramble data
      //      printf("DATA %x.%x.",data_ind[0],data_ind[1]);
      data_ind[0] ^= scrambler[0]; // service byte 0
      data_ind[1] ^= scrambler[1]; // service byte 1

      j=2;
      for (i=0;i<(rxv->sdu_length+4);i++,j++) {  // sdu+crc
	//	printf("%x.",data_ind[i+2]);
	if (j==(127*8))
	  j=0;
	data_ind[i+2]^=scrambler[j];

      }
      //      printf("\n");

      crc_rx = 0xffffffff;
      crc32(data_ind,&crc_rx,2+rxv->sdu_length);
#ifdef DEBUG_DATA
      printf("Received CRC %x.%x.%x.%x (%x), computed %x\n",
	     data_ind[2+rxv->sdu_length],
	     data_ind[2+rxv->sdu_length+1],
	     data_ind[2+rxv->sdu_length+2],
	     data_ind[2+rxv->sdu_length+3],
	     *(uint32_t*)&data_ind[2+rxv->sdu_length],
	     crc_rx);

      printf("SDU length %d\n",rxv->sdu_length);
      for (i=0;i<rxv->sdu_length;i++) {
	if ((i&15) == 0)
	  printf("\n %04x :  %02x",i,data_ind[2+i]);
	else
	  printf(".%02x",data_ind[2+i]);
      }
      printf("\n");
#endif
    }
#ifdef EXECTIME
#ifdef RTAI
    tout=rt_get_time_ns();
    dd_t4 += (tout-tin);
    tin=rt_get_time_ns();
#else
    ret=clock_gettime(CLOCK_REALTIME,&tout);
    dd_t4 += (tout.tv_nsec-tin.tv_nsec);
#endif
#endif
  }




  return(*(uint32_t*)&data_ind[2+rxv->sdu_length] == crc_rx);

}
