
// Maxime Guillaud - created Mon May 22 15:54:44 CEST 2006
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile



#include <octave/oct.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"

#define FCNNAME "oarf_set_rx_rfmode"

#define TRACE 1



static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=1)
  {
    error(FCNNAME);
    error("syntax: oarf_set_rx_rfmode(mode)");
    error("mode: (see MAXIM 2391 data sheet p. 17)");
    error("  0: Low gain (LNA low gain, Mixer low gain) ");
    error("  1: Med gain (LNA high gain, Mixer low gain)");
    error("  2: High gain (LNA high gain, Mixer high gain)");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 2.0))
  {
    error(FCNNAME);
    error("mode must be 0, 1 or 2.");
    return true;
  }
  
  return false;
}




DEFUN_DLD (oarf_set_rx_rfmode, args, nargout,"Set RX RF Mode (MAX2391 CONFIG and OPCNTRL registers - p. 17 of the data sheet) (Action 14) - mode is 0, 1 or 2.")
{

  if (any_bad_argument(args))
       return octave_value_list(); 
       
  unsigned int modenumber = args(0).int_value();

  octave_value returnvalue;
  int openair_fd;


  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  returnvalue=ioctl(openair_fd,openair_RX_RF_MODE,&modenumber);
  close(openair_fd); 

  return octave_value(returnvalue);
}


