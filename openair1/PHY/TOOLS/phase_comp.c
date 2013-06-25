/*!\brief Phase error compensation*/
///
/// Accomplishes the phase error compensation for CHBCH and SCH channels
///

#ifndef USER_MODE
#define __NO_VERSION__

//#include "from_grlib_softregs.h"

#else
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#endif

#include "PHY/defs.h"
#include "PHY/extern.h"
#include "SCHED/extern.h"
#include "defs.h"
#include "extern.h"

#include "mmintrin.h"


#ifndef USER_MODE
#define openair_get_mbox() (*(unsigned int *)mbox)
#endif //USER_MODE

static __m64 perror64 __attribute__ ((aligned(16)));
static __m64 Rsymb_conj64 __attribute__ ((aligned(16)));

int phy_phase_compensation_top (unsigned int pilot_type, unsigned int initial_pilot,
				unsigned int last_pilot, int ignore_prefix)
{

#ifdef PC_TARGET

  // i  - pilot index
  // i2 - I have no idea
  // aa - receive antenna index
  unsigned short i, i2, aa;

  // Temporary buffer or similar
  int *input;

  // Our SCH indices
  unsigned int pilot_offset, sch_index;

  unsigned char frequency_reuse_ind = 1;

  unsigned char log2_avg,log2_perror_amp;
	
  struct complex16 perror, *Rchsch, *Rsymb, *Rsch;
  __m64 *Rchsch64, *Rsch64;
	
  register __m64 mm0, mm1;

  int ind, ind64;
  int chr, chi;
  int rx_energy[NB_ANTENNAS_RX];
  unsigned char *chbch_pdu;
  unsigned int chbch_size;
  unsigned int time_in, time_out;

  unsigned int number_of_pilots = last_pilot - initial_pilot;

#ifdef USER_MODE
  char fname[40], vname[40];
#endif //USER_MODE

#ifdef DEBUG_PHY
  msg ("[openair][PHY][CODING] Correction of phase error\n");
#endif // DEBUG_PHY

  for (aa = 0; aa < NB_ANTENNAS_RX; aa++)
    {
      // Lets check to see if the phase compensation is done over the CHBCH or SCH 
      if (pilot_type == 0)	// Were working with the CHBCH
	{

	  for (i = 0; i < NUMBER_OF_CHBCH_SYMBOLS; i++)
	    {

	      // Review this code... It is NOT WORKING!!!
#ifndef USER_MODE
	      if (openair_daq_vars.mode == openair_NOT_SYNCHED)
		{
		  input = &PHY_vars->rx_vars[aa].
		    RX_DMA_BUFFER[(PHY_vars->rx_vars[0].offset +
				   ((i +
				     SYMBOL_OFFSET_CHBCH) <<
				    LOG2_NUMBER_OF_OFDM_CARRIERS) + (i +
								     1 +
								     SYMBOL_OFFSET_CHBCH)
				   * CYCLIC_PREFIX_LENGTH) %
				  FRAME_LENGTH_SAMPLES];
		}
	      else
		{
#ifdef HW_PREFIX_REMOVAL
		  input = &PHY_vars->rx_vars[aa].
		    RX_DMA_BUFFER[((i +
				    SYMBOL_OFFSET_CHBCH) <<
				   LOG2_NUMBER_OF_OFDM_CARRIERS)];
#else
		  input = &PHY_vars->rx_vars[aa].
		    RX_DMA_BUFFER[((i +
				    SYMBOL_OFFSET_CHBCH) <<
				   LOG2_NUMBER_OF_OFDM_CARRIERS) + (i +
								    SYMBOL_OFFSET_CHBCH
								    +
								    1) *
				  CYCLIC_PREFIX_LENGTH];
#endif //HW_PREFIX_REMOVAL
		}
#else //USER_MODE

	      input = &PHY_vars->rx_vars[aa].
		RX_DMA_BUFFER[(PHY_vars->rx_vars[0].offset +
			       ((i +
				 SYMBOL_OFFSET_CHBCH) <<
				LOG2_NUMBER_OF_OFDM_CARRIERS) + (i + 1 +
								 SYMBOL_OFFSET_CHBCH)
			       * CYCLIC_PREFIX_LENGTH) %
			      FRAME_LENGTH_SAMPLES];

#endif //USER_MODE

	      //    msg("[openair][PHY][CHBCH %d] frame %d: Decoding -> FFT %d\n",frequency_reuse_ind,frame,i);

	      //				dump_chbch_pilots (0);

	      fft ((short *) &input[0],
		   (short *) &PHY_vars->chbch_data[frequency_reuse_ind].
		   rx_sig_f[aa][i << (1 + LOG2_NUMBER_OF_OFDM_CARRIERS)],
		   (short *) twiddle_fft, rev, LOG2_NUMBER_OF_OFDM_CARRIERS, 3,
		   0);



	      // Phase error compensation

	      //				dump_chbch_pilots (0);
	      if (NUMBER_OF_CHBCH_PILOTS)
		{

		  perror.r = 0;
		  perror.i = 0;

		  Rchsch = (struct complex16 *) &PHY_vars->
		    chsch_data[frequency_reuse_ind].rx_sig_f[aa][0];
		  Rsymb = (struct complex16 *) &PHY_vars->
		    chbch_data[frequency_reuse_ind].
		    rx_sig_f[aa][i <<
				 (1 + LOG2_NUMBER_OF_OFDM_CARRIERS)];

		  Rchsch64 =
		    (__m64 *) & PHY_vars->
		    chsch_data[frequency_reuse_ind].rx_sig_f[aa][0];


		  // inner product of received CHSCH in pilot positions and received symbol
		  mm1 = _m_pxor (mm1, mm1);

		  for (i2 = 0; i2 < NUMBER_OF_CHBCH_PILOTS; i2++)
		    {
		      ind = PHY_vars->chbch_data[frequency_reuse_ind].
			pilot_indices[i2] << 1;
		      ind64 = PHY_vars->chbch_data[frequency_reuse_ind].
			pilot_indices[i2];

#ifdef DEBUG_PHY
#ifdef USER_MODE
		      msg ("[OPENAIR][PHY][CHBCH DEMOD]Ant %d symbol %d (%p), pilot %d (%d): RX p (%d,%d), RX s (%d,%d)\n", aa, i, Rsymb, i2, ind64, Rchsch[ind].r, Rchsch[ind].i, Rsymb[ind].r, Rsymb[ind].i);
#endif /* USER_MODE */
#endif /* DEBUG_PHY */
		      //      perror.r += ( ((Rchsch[ind].r*Rsymb[ind].r)>>PERROR_SHIFT) + ((Rchsch[ind].i*Rsymb[ind].i)>>PERROR_SHIFT) );
		      //      perror.i += ( ((Rchsch[ind].i*Rsymb[ind].r)>>PERROR_SHIFT) - ((Rchsch[ind].r*Rsymb[ind].i)>>PERROR_SHIFT) );
		      // MMX version

		      ((short *) &Rsymb_conj64)[0] = Rsymb[ind].r;
		      ((short *) &Rsymb_conj64)[1] = Rsymb[ind].i;
		      ((short *) &Rsymb_conj64)[2] = -Rsymb[ind].i;
		      ((short *) &Rsymb_conj64)[3] = Rsymb[ind].r;

		      mm0 = _mm_madd_pi16 (Rchsch64[ind64], Rsymb_conj64);
		      mm1 = _mm_add_pi32 (mm0, mm1);
		    }

		  perror64 =
		    _mm_srai_pi32 (mm1,
				   PERROR_SHIFT +
				   LOG2_NUMBER_OF_CHBCH_PILOTS);
		  perror.r = ((short *) &perror64)[0];
		  perror.i = ((short *) &perror64)[2];

		  // base shift for rotation on amplitude first OFDM symbol only (might be better to compute the max over the CHBCH 
		  if ((aa == 0) && (i == 0))
		    {
		      log2_perror_amp =
			log2_approx ((unsigned int) ((int) perror.
						     r * perror.r +
						     (int) perror.
						     i *
						     perror.
						     i)) >> 1;
		    }

		  // Apply rotation
		  rotate_cpx_vector ((short *) Rsymb, (short *) &perror,
				     (short *) Rsymb, NUMBER_OF_OFDM_CARRIERS,
				     log2_perror_amp, 0);

#ifdef DEBUG_PHY
#ifdef USER_MODE

		  msg ("[OPENAIR][PHY][CHBCH DEMOD] Ant %d : symbol %d, perror = (%d,%d) , approx amp %d\n", aa, i, perror.r, perror.i, log2_perror_amp);
#endif //USER_MODE
#endif //DEBUG_PHY
		}
	    }
	}
      else		// Were working with the SCH
	{
	  // Foreach PILOT SCH
	  for (pilot_offset = initial_pilot; pilot_offset <= last_pilot; pilot_offset++)
	    {
	      // Set the absolute pilot index wrt the frame:
	      sch_index = EMOS_SCH_INDEX;

	      for (i = 0; i < NUMBER_OF_SCH_SYMBOLS; i++)
		{

		  if (ignore_prefix == 1)
		    {
		      input = &PHY_vars->rx_vars[aa].RX_DMA_BUFFER[(i + pilot_offset) << (LOG2_NUMBER_OF_OFDM_CARRIERS)];
		    }
		  else
		    {
		      input = &PHY_vars->rx_vars[aa].RX_DMA_BUFFER[((i + pilot_offset) << (LOG2_NUMBER_OF_OFDM_CARRIERS)) + (i + 1 + pilot_offset) *
								   CYCLIC_PREFIX_LENGTH];
		    }


		  fft ((short *) &input[0],
		       (short *) &PHY_vars->sch_data[sch_index].rx_sig_f[aa][i << (1 + LOG2_NUMBER_OF_OFDM_CARRIERS)],
		       (short *) twiddle_fft, rev,
		       LOG2_NUMBER_OF_OFDM_CARRIERS, 3, 0);

		  perror.r = 0;
		  perror.i = 0;

		  // Reference SCH
		  Rsch = (struct complex16 *) &PHY_vars->sch_data[sch_index].rx_sig_f[aa][initial_pilot];
		  Rsch64 = (__m64 *) & PHY_vars->chsch_data[sch_index].rx_sig_f[aa][initial_pilot]; //IN MMX format

		  // SCH to be de-rotated
		  Rsymb = (struct complex16 *) &PHY_vars->sch_data[sch_index].rx_sig_f[aa][pilot_offset << (1 + LOG2_NUMBER_OF_OFDM_CARRIERS)];

		  // inner product of received CHSCH in pilot positions and received symbol
		  mm1 = _m_pxor (mm1, mm1);

		  for (i2 = 0; i2 < NUMBER_OF_OFDM_CARRIERS; i2++)
		    {
		      // Indices
		      ind = i2 << 1;
		      ind64 = i2;

#ifdef DEBUG_PHY
#ifdef USER_MODE
		      msg ("[OPENAIR][PHY][CHBCH DEMOD]Ant %d symbol %d (%p), pilot %d (%d): RX p (%d,%d), RX s (%d,%d)\n", aa, i, Rsymb, i2, ind64, Rchsch[ind].r, Rchsch[ind].i, Rsymb[ind].r, Rsymb[ind].i);
#endif /* USER_MODE */
#endif /* DEBUG_PHY */
		      //      perror.r += ( ((Rchsch[ind].r*Rsymb[ind].r)>>PERROR_SHIFT) + ((Rchsch[ind].i*Rsymb[ind].i)>>PERROR_SHIFT) );
		      //      perror.i += ( ((Rchsch[ind].i*Rsymb[ind].r)>>PERROR_SHIFT) - ((Rchsch[ind].r*Rsymb[ind].i)>>PERROR_SHIFT) );
		      // MMX version

		      // Initializing the vector for complex mult
		      ((short *) &Rsymb_conj64)[0] = Rsymb[ind].r;
		      ((short *) &Rsymb_conj64)[1] = Rsymb[ind].i;
		      ((short *) &Rsymb_conj64)[2] = -Rsymb[ind].i;
		      ((short *) &Rsymb_conj64)[3] = Rsymb[ind].r;

		      mm0 = _mm_madd_pi16 (Rchsch64[ind64], Rsymb_conj64);
		      mm1 = _mm_add_pi32 (mm0, mm1);
		    }

		  // this is a bitwise shift
		  perror64 = _mm_srai_pi32 (mm1, PERROR_SHIFT + LOG2_NUMBER_OF_OFDM_CARRIERS);
		  perror.r = ((short *) &perror64)[0];
		  perror.i = ((short *) &perror64)[2];

		  // base shift for rotation on amplitude first OFDM symbol only (might be better to compute the max over the CHBCH 
		  if ((aa == 0) && (i == 0))
		    {
		      // For normalization in the followinf routine
		      log2_perror_amp = log2_approx((unsigned int) ((int) perror.r * perror.r + (int)perror.i * perror.i)) >> 1;
		    }

		  // Apply rotation
		  rotate_cpx_vector ((short *) Rsymb,
				     (short *) &perror,
				     (short *) Rsymb,
				     NUMBER_OF_OFDM_CARRIERS,
				     log2_perror_amp,
				     0);

#ifdef DEBUG_PHY
#ifdef USER_MODE

		  msg ("[OPENAIR][PHY][CHBCH DEMOD] Ant %d : symbol %d, perror = (%d,%d) , approx amp %d\n", aa, i, perror.r, perror.i, log2_perror_amp);
#endif //USER_MODE
#endif //DEBUG_PHY
		}











	      phy_channel_estimation ((short *) &PHY_vars->
				      sch_data[sch_index].
				      rx_sig_f[aa][i <<
						   (1 +
						    LOG2_NUMBER_OF_OFDM_CARRIERS)],
				      (short *) &PHY_vars->
				      sch_data[sch_index].
				      channel[aa][i <<
						  (1 +
						   LOG2_NUMBER_OF_OFDM_CARRIERS)],
				      (short *) &PHY_vars->
				      sch_data[sch_index].
				      channel_f[aa][i <<
						    (1 +
						     LOG2_NUMBER_OF_OFDM_CARRIERS)],
				      (short *) &PHY_vars->
				      sch_data[sch_index].
				      channel_matched_filter_f[aa][i <<
								   (1 +
								    LOG2_NUMBER_OF_OFDM_CARRIERS)],
				      (short *) &PHY_vars->sch_data[0].
				      SCH_conj_f[i <<
						 (1 +
						  LOG2_NUMBER_OF_OFDM_CARRIERS)],
				      15, //LOG2_SCH_RX_F_AMP,
				      (NB_ANTENNAS_RX == 1) ? 1 : 0);

#ifdef USER_MODE
#ifdef DEBUG_PHY

	      sprintf (fname, "sch%d_channelF%d.m", sch_index, aa);
	      sprintf (vname, "sch%d_chanF%d", sch_index, aa);


	      write_output (fname,
			    vname,
			    (short *) &PHY_vars->sch_data[sch_index].
			    channel_f[aa][0], 2 * NUMBER_OF_OFDM_CARRIERS, 2, 1);

	      sprintf (fname, "sch%d_channel%d.m", sch_index, aa);
	      sprintf (vname, "sch%d_chan%d", sch_index, aa);

	      write_output (fname,
			    vname,
			    (short *) &PHY_vars->sch_data[sch_index].
			    channel[aa][0], 2 * NUMBER_OF_OFDM_CARRIERS, 2, 1);

#endif // DEBUG_PHY
#endif // USER_MODE

	    }
	}
    }


#endif //PC_TARGET
}


void phy_phase_compensation (short *ref_sch, short *tgt_sch, short *out_sym, int ignore_prefix, int aa, struct complex16 *perror_out)
{
  struct complex16 perror, *Rsymb, *Rsch;
	
  struct complex32 perror32;
	
  __m64 perror64, Rsymb_conj64, Rsch64;
  __m64 Rsymb64[NUMBER_OF_OFDM_CARRIERS];
  __m64 *tgt_sch64 = (__m64 *)tgt_sch;
	
  register __m64 mm0, mm1, mm2;
	
  int i,i2;
	
  unsigned char log2_avg,log2_perror_amp;
	
  int ind, ind64;
	
  perror.r = 0;
  perror.i = 0;
	
  perror32.r = 0;
  perror32.i = 0;
	
  // Reference SCH
  Rsch = (struct complex16 *) ref_sch;
	
  // SCH to be de-rotated
  Rsymb = (struct complex16 *) tgt_sch;
	
	
  // The following code copies the data from *tgt_sch to *Rsymb64 and 
  // converts it into the following format: | Re0 Im0 Re0 Im0 || Re1 Im1 Re1 Im1 |
  i2=0;
  for(i=0;i<((NUMBER_OF_OFDM_CARRIERS)>>1);i++)      // reverse 2 complex samples at a time => does the loop size>>1 times
    {

      mm0 = tgt_sch64[i];

      mm1 = mm0;
      mm2 = mm0;

      mm1 = _m_punpckldq(mm1,mm0);
      mm2 = _m_punpckhdq(mm2,mm0);

      Rsymb64[i2++]=mm1;             // get the output index (reverse bit ordering index)
      Rsymb64[i2++]=mm2;
	
    }
		
  // inner product of received CHSCH in pilot positions and received symbol
  mm1 = _m_pxor (mm1, mm1);
	
  for (i = 0; i < NUMBER_OF_OFDM_CARRIERS; i++)
    {
      // Indices
      ind = i;
      //ind64 = i;

      // Pure C version 
      // 		perror32.r += ( (((int)Rsch[ind].r*(int)Rsymb[ind].r) >> PERROR_SHIFT) + (((int)Rsch[ind].i*(int)Rsymb[ind].i) >> PERROR_SHIFT) );
      // 		perror32.i += ( (((int)Rsch[ind].i*(int)Rsymb[ind].r) >> PERROR_SHIFT) - (((int)Rsch[ind].r*(int)Rsymb[ind].i) >> PERROR_SHIFT) );
		
      // MMX version
      ((short *) &Rsch64)[0] = Rsch[ind].r;
      ((short *) &Rsch64)[1] = Rsch[ind].i;
      ((short *) &Rsch64)[2] = Rsch[ind].r;
      ((short *) &Rsch64)[3] = Rsch[ind].i;
		
      // Initializing the vector for complex mult
      ((short *) &Rsymb_conj64)[0] = Rsymb[ind].r;
      ((short *) &Rsymb_conj64)[1] = Rsymb[ind].i;
      ((short *) &Rsymb_conj64)[2] = -Rsymb[ind].i;
      ((short *) &Rsymb_conj64)[3] = Rsymb[ind].r;
		
      //Complex multiplication 16 -> 32 bits
      mm0 = _mm_madd_pi16 (Rsch64, Rsymb_conj64);
      mm1 = _mm_add_pi32 (mm0, mm1);
    }
	
  // Believe me... this is a bitwise shift! (bringing back to 16 bits)
  perror64 = _mm_srai_pi32 (mm1, PERROR_SHIFT + LOG2_NUMBER_OF_OFDM_CARRIERS);
  perror.r = ((short *) &perror64)[0];
  perror.i = ((short *) &perror64)[2];
			
  // 	perror.r = (short)(perror32.r >> LOG2_NUMBER_OF_OFDM_CARRIERS);
  // 	perror.i = (short)(perror32.i >> LOG2_NUMBER_OF_OFDM_CARRIERS);
	
  // base shift for rotation on amplitude first OFDM symbol only (might be better to compute the max over the CHBCH)
  // 	if (aa == 0)
  // 	{
  // For normalization in the following routine
  log2_perror_amp = log2_approx((unsigned int) ((int)(perror.r * perror.r) + (int)(perror.i * perror.i))) >> 1;
  // 	}

  *perror_out = perror;

  // Apply rotation
  rotate_cpx_vector ((short *) Rsymb64, (short *) &perror, (short *) out_sym, NUMBER_OF_OFDM_CARRIERS, log2_perror_amp,0);
}


