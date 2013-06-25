
// Maxime Guillaud - created Fri May 12 16:20:04 CEST 2006
// Matthias Ihmig, 2013
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile
// Update: Wed May 23 17:25:39 CEST 2007, fifo acquisition of signal buffer (RK)
//         29.01.2013: adopted to new kernel driver


#include <octave/oct.h>

extern "C" {
#include "openair0_lib.h"
}

#define FCNNAME "oarf_get_frame"

#define TRACE 1

static bool any_bad_argument(const octave_value_list &args)
{
    octave_value v;
    if (args.length()!=1)
    {
        error(FCNNAME);
        error("syntax: oarf_get_frame(card)");
        return true;
    }

    v=args(0);
    if ((!v.is_real_scalar()) || (v.scalar_value() < -2) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() >= MAX_CARDS))
    {
        error(FCNNAME);
        error("card must be 0-3 for a specific card, or -1 to get frame from all cards.\nSet framing.sync_mode =SYNCMODE_MASTER for one card and =SYNCMODE_SLAVE to obtain synchronized frames.\n");
        return true;
    }
    return false;
}

int find_mastercard()
{
    int card=0, found_master=0, i;
    const char *str[] = { "FREE", "MASTER", "SLAVE" };
    
    printf("Card configuration: card");
    for (i=0; i<openair0_num_detected_cards; i++)
        printf(" [%i]=%s,", i, str[ openair0_exmimo_pci[i].exmimo_config_ptr->framing.multicard_syncmode ]);
    printf("\n");

    for (i=0; i<openair0_num_detected_cards; i++)
    {
        if ( openair0_exmimo_pci[i].exmimo_config_ptr->framing.multicard_syncmode == SYNCMODE_MASTER )
        {
            if (found_master >= 1)
            {
                printf("Warning: There is more than one master active! Will put card[%i] back into SYNCMODE_FREE!\n", i);
                openair0_exmimo_pci[i].exmimo_config_ptr->framing.multicard_syncmode == SYNCMODE_FREE;
            }
            else
            {
                card = i;
                found_master++;
            }
        }
    }
    
    if (found_master == 0)
        printf("Warning: No master activated! Will trigger getframe on card 0\n");
    
    return card;
}

DEFUN_DLD (oarf_get_frame, args, nargout,"Get frame")
{
    int numant, no_getframe_ioctl=0;
    
    if (any_bad_argument(args))
        return octave_value_list();
       
    int card = args(0).int_value();
    
    octave_value returnvalue;
    int i,aa;
    short *rx_sig[MAX_CARDS * MAX_ANTENNAS];
    int ret;

    ret = openair0_open();
    if ( ret != 0 )
    {
        error(FCNNAME);
        if (ret == -1)
            error("Error opening /dev/openair0");
        if (ret == -2)
            error("Error mapping bigshm");
        if (ret == -3)
            error("Error mapping RX or TX buffer");
        return octave_value(ret);
    }
    
    if (card == -2)
    {
        no_getframe_ioctl = 1;
        card = -1;
    }
    
    if (card <-1 || card >= openair0_num_detected_cards)
        error("card number must be between 0 and %d. Or -1 for all cards.", openair0_num_detected_cards-1);

    if (card == -1) {
        numant = openair0_num_detected_cards * openair0_num_antennas[0];
        card = find_mastercard();
    }
    else
        numant = openair0_num_antennas[card];
    
    if ( no_getframe_ioctl )
    {
        // print mbox counters
        for (i=0; i<numant; i++)
        {
            
            if ( numant == openair0_num_antennas[card] )
                printf("rx_cnt[%d] = %d, tx_cnt[%d] = %d,    ", i, *openair0_exmimo_pci[card].rxcnt_ptr[i],
                                                                i, *openair0_exmimo_pci[card].txcnt_ptr[i]);
            else
                printf("rx_cnt[%d] = %d, tx_cnt[%d] = %d,    ", i, *openair0_exmimo_pci[i / (int)openair0_num_antennas[0]].rxcnt_ptr[i % openair0_num_antennas[0]],
                                                                i, *openair0_exmimo_pci[i / (int)openair0_num_antennas[0]].txcnt_ptr[i % openair0_num_antennas[0]]);
        }
        printf("\n");
    }
    
    ComplexMatrix dx (FRAME_LENGTH_COMPLEX_SAMPLES, numant);

    // set the tx buffer to 0x00010001 to put switch in rx mode
    for (aa=0; aa<numant; aa++) 
      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) 
	((int*)(openair0_exmimo_pci[card].adc_head[aa]))[i] = 0x00010001;

    // assign userspace pointers
    for (i=0; i<numant; i++)
    {
        if ( numant == openair0_num_antennas[card] )
            rx_sig[i] = (short*) openair0_exmimo_pci[ card ].adc_head[ i ];
        else
            rx_sig[i] = (short*) openair0_exmimo_pci[ i / (int)openair0_num_antennas[0] ].adc_head[i % openair0_num_antennas[0]];
            
        //printf("adc_head[%i] = %p ", i, rx_sig[i]);
    }
    printf("\n");

    //  msg("Getting buffer...\n");
    if ( no_getframe_ioctl == 0)
        openair0_get_frame(card);

    for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++)
        for (aa=0; aa<numant; aa++)
            dx(i, aa) = Complex( rx_sig[aa][i*2], rx_sig[aa][i*2+1] );
    
    openair0_close();

    return octave_value (dx);
}


