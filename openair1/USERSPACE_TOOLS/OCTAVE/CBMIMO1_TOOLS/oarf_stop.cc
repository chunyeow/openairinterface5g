
// Florian Kaltenberger - created 2010-01-22
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
#include "PHY/impl_defs_lte.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
}
#include "PHY/vars.h"

#define FCNNAME "oarf_stop"

#define TRACE 1



static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=1)
  {
    error(FCNNAME);
    error("syntax: oarf_stop(freqband)");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 6.0))
  {
    error(FCNNAME);
      error("freqband must be 0-5");
    return true;
  }
  
  return false;
}




DEFUN_DLD (oarf_stop, args, nargout,"Stop RT acquisition and write registers.")
{

  if (any_bad_argument(args))
       return octave_value_list(); 
       
  unsigned int fd, frequency = args(0).int_value();

  octave_value returnvalue;
  int openair_fd;

  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  fd = (1 | ((frequency&7)<<1) | ((frequency&7)<<4));
  returnvalue=ioctl(openair_fd,openair_STOP,(void*)&fd);

  close(openair_fd); 

  return octave_value(returnvalue);
}


