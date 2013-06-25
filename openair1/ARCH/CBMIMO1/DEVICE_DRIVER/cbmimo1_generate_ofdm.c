#ifndef USER_MODE
#define __NO_VERSION__

//#include "rt_compat.h"

#endif


#include "defs.h"
#include "extern.h"
#include "cbmimo1_pci.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#ifndef OPENAIR_LTE
#include "PHY/TRANSPORT/defs.h"
#endif

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"


void openair_generate_ofdm() {

  unsigned char dummy_mac_pdu[1024];

#ifndef OPENAIR_LTE

  phy_generate_chbch(0,0,NB_ANTENNAS_TX,dummy_mac_pdu);

#endif 

}
