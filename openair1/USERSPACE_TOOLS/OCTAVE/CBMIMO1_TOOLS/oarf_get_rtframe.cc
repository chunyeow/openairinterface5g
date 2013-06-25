
// Maxime Guillaud - created Fri May 12 16:20:04 CEST 2006
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile



#include <octave/oct.h>
#include <fcntl.h>
#include <sys/ioctl.h>

extern "C" {
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "PHY/CONFIG/defs.h"
#include "PHY/CONFIG/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
}

#include <time.h>

//#include <rtai.h>


//#include "config_vars.h"
#include "chansounder_defs.h"


//#include "oarf_common.c"

#define FCNNAME "oarf_get_rtframe"


#define SLOTS_PER_FRAME 4
#define FRAME_LENGTH_COMPLEX_SAMPLES (OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*SLOTS_PER_FRAME)

extern PHY_CONFIG *PHY_config;


typedef struct                   /* this shouldn't be here, but including mac_defs.h screws up the macros defining the symbol/slot/frame length :-( */
{
    int registers;
    void *params_ptr;
    int params_size;
} MACPHY_PARAMS;




static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=1)
  {
    error(FCNNAME);
    error("syntax: oarf_get_rtframe(freqband)\n      freqband in 0-3.");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 3.0))
  {
    error(FCNNAME);
      error("freqband must be 0, 1, 2, or 3.");
    return true;
  }

  return false;
}








DEFUN_DLD (oarf_get_rtframe, args, nargout,"Get frame through RT mode (Action 20)")
{
  MACPHY_PARAMS MACPHY_params;
  CHANSOUNDER_PARAMS CHANSOUNDER_params;
  int result, slot, openair_fifo_fd;
  char *signalbuffer;

  if (any_bad_argument(args))
       return octave_value_list();
       
  const int freq = args(0).int_value();  

  octave_value returnvalue;
  int openair_fd,i;

  ComplexMatrix dx (FRAME_LENGTH_COMPLEX_SAMPLES,nb_rx);
  char buffer_local[4*FRAME_LENGTH_COMPLEX_SAMPLES+sizeof(long long)];

  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }



  CHANSOUNDER_params.slot_cyclelength=10000;
  CHANSOUNDER_params.slot_save=SLOTS_PER_FRAME;

  MACPHY_params.registers=((freq&3)<<1) | ((freq&3)<<3); 
  MACPHY_params.params_ptr=&CHANSOUNDER_params;
  MACPHY_params.params_size=sizeof(CHANSOUNDER_params);

  result=ioctl(openair_fd,openair_START_CHANSOUNDER,&MACPHY_params);
//  printf("start sounder ioctl result=%d, sleeping\n",result);
  
  usleep(20000);          // sleep for 20 ms 

  CHANSOUNDER_params.slot_save=0;
  result=ioctl(openair_fd,openair_START_CHANSOUNDER,&MACPHY_params);
//  printf("start sounder with slot_save=0, ioctl result=%d, reading fifo\n",result);

//  result=ioctl(openair_fd,openair_STOP,NULL);   // do not stop sounder since doing it too many times crashes RTAI. 

  close(openair_fd);

  
  openair_fifo_fd=open("/dev/rtf3",O_RDONLY);
  if (openair_fifo_fd==-1)
  {
    printf("Can not open openair FIFO (/dev/rtf3), load kernel modules first !\n");
    return octave_value_list();
  }

  for (slot=0;slot<SLOTS_PER_FRAME;slot++)
  {
    read(openair_fifo_fd,buffer_local,CHANSOUNDER_BYTESPERSLOT+sizeof(long long));
//    printf("slot=%d, slot_time=%llu\n",slot,*(long long *)buffer_local);
    signalbuffer=buffer_local+sizeof(long long);

    for (i=0;i<NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++)
    {
      dx(i+slot*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,0)=Complex( signalbuffer[i*4], signalbuffer[i*4+1] );
      dx(i+slot*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1)=Complex( signalbuffer[i*4+2], signalbuffer[i*4+3] );
    }  
    
  }

  close(openair_fifo_fd);
  
  
  return octave_value (dx);
}


