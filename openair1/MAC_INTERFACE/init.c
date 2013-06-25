/*________________________mac_init.c________________________

 Authors : Hicham Anouar, Raymond Knopp
 Company : EURECOM
 Emails  : anouar@eurecom.fr,  knopp@eurecom.fr
________________________________________________________________*/

/*!\brief Initilization and reconfiguration routines for generic MAC interface */

#include "defs.h"
#include "extern.h"
#include "SCHED/defs.h"

int mac_init(void)
{
  int i;
  
#ifndef USER_MODE
  // mac_xface->macphy_init();
  mac_xface->macphy_exit = openair_sched_exit;
#else
  mac_xface->macphy_exit=(void (*)(void)) exit;
#endif

  /* this is done in cbmimo1_fileops
#ifdef OPENAIR2
  mac_xface->macphy_init();
#endif //OPENAIR2
  */

  return(1);
}

void mac_cleanup(void)
{

}

void mac_resynch(void) {

}

/*
#ifdef OPENAIR2
EXPORT_SYMBOL(frame);
#endif //OPENAIR2
*/
