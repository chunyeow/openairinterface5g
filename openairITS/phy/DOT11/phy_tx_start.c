#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "defs.h"
#include "txvars.h"
#include "PHY/CODING/defs.h"
#include "PHY/TOOLS/defs.h"

//#define DEBUG_TX

int ratemap[8]    = {11,15,10,14,9,13,8,12};
int rateparity[8] = {1,0,0,1,0,1,1,0};
int Ndbps[8] = {24,36,48,72,96,144,192,216}; // CodeRate*48*ModOrder
int Ncbps[8] = {48,48,96,96,192,192,288,288};
int puncturing[8] = {0,1,0,1,0,1,2,1};

int16_t BPSK[2] = {-AMP,AMP};
int16_t QPSK[2] = {-((AMP*ONE_OVER_SQRT_2)>>15),+((AMP*ONE_OVER_SQRT_2)>>15)};

int16_t QAM16[4] = {-((AMP*3*ONE_OVER_SQRT_10)>>15),  //00
		    -((AMP*1*ONE_OVER_SQRT_10)>>15),  //01
		     ((AMP*3*ONE_OVER_SQRT_10)>>15),   //10
		     ((AMP*1*ONE_OVER_SQRT_10)>>15)}; //11

int16_t QAM64[8] = {-((AMP*7*ONE_OVER_SQRT_42)>>15),  //000
		    -((AMP*5*ONE_OVER_SQRT_42)>>15),  //001
		    -((AMP*1*ONE_OVER_SQRT_42)>>15),   //010
		    -((AMP*3*ONE_OVER_SQRT_42)>>15),  //011
		     ((AMP*7*ONE_OVER_SQRT_42)>>15),   //100
		     ((AMP*5*ONE_OVER_SQRT_42)>>15),  //101
		     ((AMP*1*ONE_OVER_SQRT_42)>>15),  //110
		     ((AMP*3*ONE_OVER_SQRT_42)>>15)}; //111

extern uint16_t rev64[64];
extern int16_t twiddle_ifft64[63*4*2];
extern uint8_t scrambler[127*8];
extern int interleaver_bpsk[48];
extern int interleaver_qpsk[96];
extern int interleaver_16qam[192];
extern int interleaver_64qam[288];




uint8_t data_coded[65632]; //65632 = (4095 byte SDU + 4 byte CRC + 2 byte service + 1 byte tail)*8 bits/byte * 2 coded bits per bit
uint8_t data_ind2[4095];

int phy_tx_start_bot(TX_VECTOR_t *tx_vector,int16_t *output_ptr,uint32_t tx_offset,int frame_length,uint8_t *data_ind) {

  uint8_t signal[3];
  int rate_bits;
  int signal_bits[24];

  uint8_t signal_coded[48];
  int16_t signal_interleaved[48];
  uint8_t*dc;   
  int data_interleaved[288];
  int16_t dataF[128] __attribute__((aligned(16)));
  int signal_parity=0;
  int16_t signalF[128] __attribute__((aligned(16)));
  int i,j,k;  // z,p
  int16_t tmp_t[64*2*2];
  uint32_t *crc;
  int dlen,dlen_symb;
  int *interleaver;
  int s,sprime;
  int16_t *output_ptr2;
  int tx_offset2;
  int overlap=0;

  memset(signal_bits,0,24*sizeof(int));
  memset(signalF,0,64*2*sizeof(int16_t));
  memset(dataF,0,64*2*sizeof(int16_t));

  assert(((unsigned int)tx_vector->rate)<8);

  rate_bits     = ratemap[(int)tx_vector->rate];
  signal_parity = rateparity[(int)tx_vector->rate];

  assert(tx_vector->sdu_length<=4095);
  assert(tx_vector->rate<=7);


  for (i=0;i<12;i++)
    signal_parity += ((tx_vector->sdu_length&(1<<i))>0) ? 1:0; 
  signal_parity&=1;
#ifdef DEBUG_TX
  printf("SIGNAL parity %d (rate parity %d, rate_bits %d)\n",signal_parity,rateparity[(int)tx_vector->rate],rate_bits);
#endif
  signal[0] = (rate_bits&0xF)+((tx_vector->sdu_length&0x7)<<5);  // first byte has 4 rate bits, 1-bit zero-padding, 3 LSB of sdu_length
  signal[1] = ((tx_vector->sdu_length>>3)&0xFF);  // second byte has bits 10:3 of sdu_length
  signal[2] = ((tx_vector->sdu_length>>11)&1) + (signal_parity<<1);
#ifdef DEBUG_TX 
  printf("Transmitted TX_VECTOR: %x,%x,%x\n",
	 signal[0],signal[1],signal[2]);
#endif
  ccodedot11_encode(3,signal,signal_coded,0);

#ifdef DEBUG_TX
  printf("SIGNAL (coded) : ");
  for (i=0;i<48;i++)
    printf("%d ",signal_coded[i]);
  printf("\n");
#endif

  // frequency interleaving
  for (i=0;i<48;i++) {
    signal_interleaved[i] = AMP*((signal_coded[interleaver_bpsk[i]]<<1)-1);
  }
#ifdef DEBUG_TX
  printf("SIGNAL (interleaved) : ");
  for (i=0;i<48;i++)
    printf("%d ",signal_interleaved[i]);
  printf("\n");
#endif
  // -ve portion
  for (i=0;i<5;i++)
    signalF[(38+i)<<1]=signal_interleaved[i];
  signalF[(38+5)<<1]=AMP;
  for (;i<18;i++)
    signalF[(38+1+i)<<1]=signal_interleaved[i];
  signalF[(38+19)<<1]=AMP;
  for (;i<24;i++)
    signalF[(38+2+i)<<1]=signal_interleaved[i];

  // +ve portion
  for (;i<30;i++)
    signalF[(-24+1+i)<<1]=signal_interleaved[i];
  signalF[(6+1)<<1]=AMP;
  for (;i<43;i++)
    signalF[(-24+2+i)<<1]=signal_interleaved[i];
  signalF[(19+2)<<1]=-AMP;
  for (;i<48;i++)
    signalF[(-24+3+i)<<1]=signal_interleaved[i];
#ifdef DEBUG_TX
  printf("SIGNAL (TX) :");
  for (i=0;i<64;i++)
    printf("%d ",signalF[i<<1]);
  printf("\n");
#endif


  fft(signalF,         /// complex input
      tmp_t,           /// complex output
      &twiddle_ifft64[0],  /// complex twiddle factors
      rev64,           /// bit reversed permutation vector
      6,               /// log2(FFT_SIZE)
      3,               /// scale (energy normalized for 64-point)
      0);              /// 0 means 64-bit complex interleaved format else complex-multiply ready repeated format

  // copy STS_LTS to start of packet

  tx_offset2 = (tx_offset<<1);
  overlap=0;
  if ((tx_offset2 + 640) > (frame_length<<1))
    overlap = 1;
  output_ptr2 = &output_ptr[tx_offset2];

#ifdef EXMIMO
  for (i=0;i<320;i++) {
    output_ptr2[(i<<1)]   = ((int16_t *)STS_LTS_t)[i<<1]<<2;
    output_ptr2[1+(i<<1)] = ((int16_t *)STS_LTS_t)[1+(i<<1)]<<2;
  }
#else
  memcpy(output_ptr2,STS_LTS_t,640*sizeof(int16_t));

#endif

  if (overlap == 1)
    memcpy((void*)output_ptr,
	   (void*)&output_ptr[frame_length<<1],
	   (tx_offset2 + 640 - (frame_length<<1))<<1);
    
  // position of SIGNAL
  tx_offset2 = (tx_offset<<1) + 640;
  
  overlap=0;
  if (tx_offset2 > (frame_length<<1))
    tx_offset2-=frame_length;
  else if ((tx_offset2 + 160) > (frame_length<<1))
    overlap = 1;
  
  output_ptr2 = &output_ptr[tx_offset2];
 
  // cyclic extension and output scaling 
  for (i=0,j=48;i<80;i++,j++) {
    if (j==64)
      j=0;
    //    printf("j %d : %d,%d\n",j,tmp_t[j<<2],tmp_t[1+(j<<2)]);
#ifdef EXMIMO
    output_ptr2[(i<<1)]     = tmp_t[j<<2]<<2;     // RE component
    output_ptr2[1+(i<<1)]   = tmp_t[1+(j<<2)]<<2; // IM component
#else
    output_ptr2[(i<<1)]     = tmp_t[j<<2];     // RE component
    output_ptr2[1+(i<<1)]   = tmp_t[1+(j<<2)]; // IM component
#endif
  }
  if (overlap == 1) {
    memcpy((void*)output_ptr,
	   (void*)&output_ptr[frame_length<<1],
	   (tx_offset2 + 160 - (frame_length<<1))<<1);
  }

  
  // now do data portion
  crc = (uint32_t*)&data_ind[tx_vector->sdu_length+2];
  *crc = 0xffffffff;
  crc32(data_ind,crc,tx_vector->sdu_length+2);
  
  dlen      = 32+16+6+(tx_vector->sdu_length<<3); // data length is 32-bits CRC + sdu + 16 service + 6 tail
  dlen_symb = dlen/Ndbps[tx_vector->rate];
  if ((dlen%Ndbps[tx_vector->rate])>0)
    dlen_symb++;

#ifdef DEBUG_TX
  printf("Number of symbols in data portion : %d for %d  bytes, CRC %x\n",dlen_symb,tx_vector->sdu_length,*crc);
#endif
  // scramble data

  data_ind2[0] = scrambler[0]; // service byte 0
  data_ind2[1] = scrambler[1]; // service byte 1
  //  printf("DATA (%x)%x.(%x)%x.",data_ind[0]^scrambler[0],data_ind[0],data_ind[1]^scrambler[1],data_ind[1]);
  j=2;
  for (i=0;i<tx_vector->sdu_length+4;i++,j++) {  // sdu+crc
    if (j==(127*8))
      j=0;
    //    printf("(%x)",data_ind[i+2]);
    data_ind2[i+2]=data_ind[i+2]^scrambler[j];
    //    printf("%x.",data_ind[i+2]);

  }
  //  printf("\n");
  data_ind2[tx_vector->sdu_length+2+4] = 0;  // tail (2 extra which are used filled later)
  // encode with conv. code 
  ccodedot11_encode(tx_vector->sdu_length+2+4+1, // 4 for CRC,  2 for service, 1 for tail
		    data_ind2,
		    data_coded,
		    puncturing[tx_vector->rate]);

#ifdef DEBUG_TX
  printf("DATA (coded) : \n");
  for (i=0;i<48*3;i++) {
    printf("%d ",data_coded[i]);
    if ((i%48) == 47)
      printf("\n");
  }
  printf("\n");
#endif

  // interleaving
  switch(tx_vector->rate>>1) {
  case 0:
    interleaver=interleaver_bpsk;
    break;
  case 1:
    interleaver=interleaver_qpsk;
    break;
  case 2:
    interleaver=interleaver_16qam;
    break;
  case 3:
    interleaver=interleaver_64qam;
    break;
  }

  dc = data_coded;
  for (s=0,sprime=1;s<dlen_symb;s++,dc+=Ncbps[tx_vector->rate],sprime++) {

    // index for pilot symbol lookup
    if (sprime==127)
      sprime=0;

    for (k=0;k<Ncbps[tx_vector->rate];k++) {
#ifdef DEBUG_TX
      if (s==0)
	printf("interleaver k %d, j %d (%d) \n",k,interleaver[k],dc[k]);
#endif
      data_interleaved[interleaver[k]] = dc[k];
    }

    switch (tx_vector->rate>>1) {
    case 0: // BPSK
      // -ve portion
      for (i=0;i<5;i++)
	dataF[(38+i)<<1]=BPSK[data_interleaved[i]&1];
      dataF[(38+5)<<1]=Pseq[sprime]; // Pilot 1
      for (;i<18;i++)
	dataF[(38+1+i)<<1]=BPSK[data_interleaved[i]&1];
      dataF[(38+19)<<1]=Pseq[sprime]; // Pilot 2
      for (;i<24;i++)
	dataF[(38+2+i)<<1]=BPSK[data_interleaved[i]&1];
      
      // +ve portion
      for (;i<30;i++)
	dataF[(-24+1+i)<<1]=BPSK[data_interleaved[i]&1];
      dataF[(6+1)<<1]=Pseq[sprime];  // Pilot 3
      for (;i<43;i++)
	dataF[(-24+2+i)<<1]=BPSK[data_interleaved[i]&1];
      dataF[(19+2)<<1]=-Pseq[sprime]; // Pilot 4
      for (;i<48;i++)
	dataF[(-24+3+i)<<1]=BPSK[data_interleaved[i]&1];
      
    break;

    case 1: // QPSK
      // -ve portion
      for (i=0;i<5;i++) {
	dataF[(38+i)<<1]       = QPSK[data_interleaved[i<<1]];
	dataF[1+((38+i)<<1)]   = QPSK[data_interleaved[1+(i<<1)]];
      }
      dataF[(38+5)<<1]=Pseq[sprime];
      for (;i<18;i++) {
	dataF[(38+1+i)<<1]     = QPSK[data_interleaved[i<<1]];
	dataF[1+((38+1+i)<<1)] = QPSK[data_interleaved[1+(i<<1)]];
      }
      dataF[(38+19)<<1]=Pseq[sprime];
      for (;i<24;i++) {
	dataF[(38+2+i)<<1]     = QPSK[data_interleaved[i<<1]];
	dataF[1+((38+2+i)<<1)] = QPSK[data_interleaved[1+(i<<1)]];
      }
      // +ve portion
      for (;i<30;i++) {
	dataF[(-24+1+i)<<1]=QPSK[data_interleaved[i<<1]];
	dataF[1+((-24+1+i)<<1)]=QPSK[data_interleaved[1+(i<<1)]];
      }
      dataF[(6+1)<<1]=Pseq[sprime];
      for (;i<43;i++) {
	dataF[(-24+2+i)<<1]=QPSK[data_interleaved[i<<1]];
	dataF[1+((-24+2+i)<<1)]=QPSK[data_interleaved[1+(i<<1)]];
      }
      dataF[(19+2)<<1]=-Pseq[sprime];
      for (;i<48;i++) {
	dataF[(-24+3+i)<<1]=QPSK[data_interleaved[i<<1]];
	dataF[1+((-24+3+i)<<1)]=QPSK[data_interleaved[1+(i<<1)]];
      }
      break;
    case 2: // 16QAM
      // -ve portion
      for (i=0;i<5;i++) {
	dataF[(38+i)<<1]       = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((38+i)<<1)]   = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      dataF[(38+5)<<1]=Pseq[sprime];
      for (;i<18;i++) {
	dataF[(38+1+i)<<1]     = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((38+1+i)<<1)] = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      dataF[(38+19)<<1]=Pseq[sprime];
      for (;i<24;i++) {
	dataF[(38+2+i)<<1]     = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((38+2+i)<<1)] = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      // +ve portion
      for (;i<30;i++) {
	dataF[(-24+1+i)<<1]    = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((-24+1+i)<<1)]    = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      dataF[(6+1)<<1]=Pseq[sprime];
      for (;i<43;i++) {
	dataF[(-24+2+i)<<1]    = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((-24+2+i)<<1)]    = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      dataF[(19+2)<<1]=-Pseq[sprime];
      for (;i<48;i++) {
	dataF[(-24+3+i)<<1]    = QAM16[data_interleaved[i<<2] + (data_interleaved[1+(i<<2)]<<1)];
	dataF[1+((-24+3+i)<<1)]    = QAM16[data_interleaved[2+(i<<2)] + (data_interleaved[3+(i<<2)]<<1)];
      }
      
    break;

    case 3: // 64QAM
      // -ve portion
      for (i=0;i<5;i++) {
	dataF[(38+i)<<1]       = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)];
	dataF[1+((38+i)<<1)]   = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      dataF[(38+5)<<1]=Pseq[sprime];
      for (;i<18;i++) {
	dataF[(38+1+i)<<1]     = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)];
	dataF[1+((38+1+i)<<1)] = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      dataF[(38+19)<<1]=Pseq[sprime];
      for (;i<24;i++) {
	dataF[(38+2+i)<<1]     = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)];
	dataF[1+((38+2+i)<<1)] = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      // +ve portion
      for (;i<30;i++) {
	dataF[(-24+1+i)<<1]    = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)];
	dataF[1+((-24+1+i)<<1)]    = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      dataF[(6+1)<<1]=Pseq[sprime];
      for (;i<43;i++) {
	dataF[(-24+2+i)<<1]    = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)]; 
	dataF[1+((-24+2+i)<<1)]    = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      dataF[(19+2)<<1]=-Pseq[sprime];
      for (;i<48;i++) {
	dataF[(-24+3+i)<<1]    = QAM64[data_interleaved[i<<3]     + (data_interleaved[1+(i<<3)]<<1) + (data_interleaved[2+(i<<3)]<<2)];
	dataF[1+((-24+3+i)<<1)]    = QAM64[data_interleaved[3+(i<<3)] + (data_interleaved[4+(i<<3)]<<1) + (data_interleaved[5+(i<<3)]<<1)];
      }
      
    break;
    default:
      break;
    }


#ifdef DEBUG_TX
    if (s<12) {
      for (i=0;i<64;i++)
	printf("s %d, k %d: (%d,%d)\n",s,i,dataF[i<<1],dataF[1+(i<<1)]);
    }
#endif
    
    fft(dataF,         /// complex input
	tmp_t,           /// complex output
	&twiddle_ifft64[0],  /// complex twiddle factors
	rev64,           /// bit reversed permutation vector
	6,               /// log2(FFT_SIZE)
	3,               /// scale (energy normalized for 64-point)
	0);              /// 0 means 64-bit complex interleaved format else complex-multiply ready repeated format

    // cyclic extension and output scaling
    tx_offset2 += 160;

    overlap=0;
    if (tx_offset2 >= (frame_length<<1))
      tx_offset2 -= (frame_length<<1);
    else if ((tx_offset2 + 160) > (frame_length<<1))
      overlap = 1;

    output_ptr2 = &output_ptr[tx_offset2];
    for (i=0,j=48;i<80;i++,j++) {
      if (j==64)
	j=0;
      //    printf("j %d : %d,%d\n",j,tmp_t[j<<2],tmp_t[1+(j<<2)]);
#ifdef EXMIMO
      output_ptr2[(i<<1)]     = tmp_t[j<<2]<<2;     // RE component
      output_ptr2[1+(i<<1)]   = tmp_t[1+(j<<2)]<<2; // IM component
#else
      output_ptr2[(i<<1)]     = tmp_t[j<<2];     // RE component
      output_ptr2[1+(i<<1)]   = tmp_t[1+(j<<2)]; // IM component
#endif
    }    
    if (overlap == 1) {
      memcpy((void*)output_ptr,
	     (void*)&output_ptr[frame_length<<1],
	     (tx_offset2 + 160 - (frame_length<<1))<<1);
    }
  }
  
  
  return(400+(80*dlen_symb));
}

int init_tx=0;

int phy_tx_start(TX_VECTOR_t *tx_vector,uint32_t *tx_frame,uint32_t next_TXop_offset,int frame_length,uint8_t *data_ind) {
#ifdef DEBUG_TX
  printf("tx_frame %p\n",tx_frame);
#endif
  return(phy_tx_start_bot(tx_vector,(int16_t *)tx_frame,next_TXop_offset,frame_length,data_ind));

}
