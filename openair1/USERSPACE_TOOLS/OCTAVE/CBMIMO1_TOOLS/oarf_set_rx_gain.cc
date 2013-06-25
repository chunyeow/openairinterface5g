
// Maxime Guillaud - created Fri May 12 14:55:02 CEST 2006
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile



#include <octave/oct.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"

#define FCNNAME "oarf_set_rx_gain"

#define TRACE 1



static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=4)
  {
    error(FCNNAME);
    error("syntax: oarf_set_rx_gain(2391_G0,  2391_G1, 9862_RX_G0,,9862_RX_G1)");
    error("Valid ranges:  2391_G0, 2391_G1: 0-255,  9862_RX_G0, 9862_RX_G1: 0-19 dB");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 255))
  {
    error(FCNNAME);
    error("2391_G0 must be an integer in 0..255.");
    return true;
  }

  v=args(2);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 19))
  {
    error(FCNNAME);
    error("9862_RX_G0 must be an integer in 0..19.");
    return true;
  }

  v=args(1);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 255))
  {
    error(FCNNAME);
    error("2391_G1 must be an integer in 0..255.");
    return true;
  }

  v=args(3);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 19))
  {
    error(FCNNAME);
    error("9862_RX_G1 must be an integer in 0..19.");
    return true;
  }


  return false;
}








DEFUN_DLD (oarf_set_rx_gain, args, nargout,"Set RX gains (Action 7) - params 2391_G0, 2391_G1 in 0-255, 9862_RX_G0 and 9862_RX_G1 in 0-19 dB.")
{

  if (any_bad_argument(args))
       return octave_value_list();
       
  unsigned char temp[4] = {args(0).int_value(),args(1).int_value(),args(2).int_value(),args(3).int_value()};

  octave_value returnvalue;
  int openair_fd;

  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  returnvalue=ioctl(openair_fd,openair_SET_RX_GAIN,&temp[0]);
  close(openair_fd);

  return octave_value(returnvalue);
}


