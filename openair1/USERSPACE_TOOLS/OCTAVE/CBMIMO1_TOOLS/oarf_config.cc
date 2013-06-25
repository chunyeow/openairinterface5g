
// Maxime Guillaud - created Thu May 11 17:19:25 CEST 2006
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
  //#include "PHY/extern.h"
  //#include "PHY/CONFIG/defs.h"
  //#include "PHY/CONFIG/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
//#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
}
//#include "PHY/vars.h"
//#include "PHY/CONFIG/vars.h"

//#include "oarf_common.c"

#define FCNNAME "oarf_config"

#define TRACE 1

//PHY_CONFIG PHY_config_mem;

static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v,w;

  if (args.length()!=4)
  {
    error(FCNNAME);
    error("syntax: oarf_config(freqband,tdd,dual_tx,tdd_config)");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 6.0))
  {
    error(FCNNAME);
    error("freqband must be 0-5");
    return true;
  }

  if ((!args(1).is_real_scalar()))// || (args(1).scalar_value()!=0.0) || (args(1).scalar_value()!=1.0))
  {
      error(FCNNAME);
      error("tdd must be 0 or 1.");
      return true;
  }

  if ((!args(2).is_real_scalar()))// || (args(2).scalar_value()!=0.0) || (args(2).scalar_value()!=1.0))
  {
      error(FCNNAME);
      error("dual_tx must be 0 or 1.");
      return true;
  }

  if ((!args(3).is_real_scalar()) || (args(3).scalar_value()<0.0) || (args(3).scalar_value()>255.0))
  {
      error(FCNNAME);
      error("dual_tx must be 0-9, 254=RX only or 255=TX only.");
      return true;
  }
  return false;
}








DEFUN_DLD (oarf_config, args, nargout,"configure the openair interface - returns 0 if successful")
{

  if (any_bad_argument(args))
       return octave_value_list();
       
  const uint freq = args(0).int_value();  
  //const std::string configfile = args(1).string_value();
  //const std::string scenariofile = args(2).string_value();
  const int tdd = args(1).int_value();
  const int dual_tx = args(2).int_value();  
  const int tdd_config = args(3).int_value();

  octave_value returnvalue;
  int openair_fd;

  LTE_DL_FRAME_PARMS *frame_parms;


  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  /*  
  if((config = fopen(configfile.c_str(),"r")) == NULL)
  {
    error(FCNNAME);
    error("configuration file could not be opened!");
    return octave_value_list();
  } 

  if((scenario = fopen(scenariofile.c_str(),"r")) == NULL)    // this file is closed by function reconfigure_MACPHY of oarf_common.c
  {
    error(FCNNAME);
    error("scenario file could not be opened!");
    return octave_value_list();
  }

  PHY_config = (PHY_CONFIG *)&PHY_config_mem;
  reconfigure_MACPHY(scenario);
  fclose(config);
  */

  frame_parms = (LTE_DL_FRAME_PARMS*) malloc(sizeof(LTE_DL_FRAME_PARMS));
  frame_parms->N_RB_DL            = 25;
  frame_parms->N_RB_UL            = 25;
  frame_parms->Ncp                = 1;
  frame_parms->Nid_cell           = 0;
  frame_parms->nushift            = 0;
  frame_parms->nb_antennas_tx     = NB_ANTENNAS_TX;
  frame_parms->nb_antennas_rx     = NB_ANTENNAS_RX;
  frame_parms->mode1_flag         = 1; //default == SISO
  frame_parms->tdd_config         = tdd_config;
  frame_parms->dual_tx            = dual_tx;
  frame_parms->frame_type         = TDD;
  frame_parms->freq_idx           = freq;
  
  init_frame_parms(frame_parms,1);
  dump_frame_parms(frame_parms);

  returnvalue=ioctl(openair_fd, openair_DUMP_CONFIG,(char *)frame_parms);

  close(openair_fd);

  return octave_value(returnvalue);
}


