
// Maxime Guillaud - created Fri May 12 14:55:02 CEST 2006
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile



#include <octave/oct.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "PHY/vars.h"

#define FCNNAME "oarf_set_calibrated_rx_gain"

#define TRACE 1



static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=1)
  {
    error(FCNNAME);
    error("syntax: oarf_set_calibrated_rx_gain(gain)");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 255.0))
  {
    error(FCNNAME);
    error("gain must be >=0 and <=255.");
    return true;
  }

  return false;
}







DEFUN_DLD (oarf_set_calibrated_rx_gain, args, nargout,"Set RX gains (Action 16) - params bool")
{

  if (any_bad_argument(args))
       return octave_value_list();
       
  unsigned int temp = args(0).int_value();

  octave_value returnvalue;
  int openair_fd;

  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  returnvalue=ioctl(openair_fd,openair_SET_CALIBRATED_RX_GAIN,&temp);
  close(openair_fd);

  return octave_value(returnvalue);
}


