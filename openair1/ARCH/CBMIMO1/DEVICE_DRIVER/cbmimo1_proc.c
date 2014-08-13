/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
#ifndef USER_MODE
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#endif //USER_MODE

#include "PHY/defs.h"
#include "PHY/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "MAC_INTERFACE/extern.h"
#include "SCHED/extern.h"

#ifndef USER_MODE
static struct proc_dir_entry *proc_openair1_root;
#endif 


#ifndef USER_MODE
static int openair1_state_read(char *buffer, char **my_buffer, off_t off, int length) {

  int len = 0;

  switch (openair_daq_vars.mode) {

  case openair_NOT_SYNCHED:
    len += sprintf(&buffer[len], "NOT IN SYNCH\n");
    break;
#ifdef OPENAIR_LTE
  case openair_SYNCHED:
    len += sprintf(&buffer[len], "SYNCHED\n");
    break;
#else
  case openair_SYNCHED_TO_CHSCH:
    len += sprintf(&buffer[len], "SYNCHED TO CH %d\n",openair_daq_vars.synch_source);
    break;
  case openair_SYNCHED_TO_MRSCH:
    len += sprintf(&buffer[len], "SYNCHED TO MR\n");
    break;
#endif
  case openair_SCHED_EXIT:
    len += sprintf(&buffer[len], "EXITED\n");
    break;
  }

}
#endif //USER_MODE

#ifndef USER_MODE
static int chbch_stats_read(char *buffer, char **my_buffer, off_t off, int length)
#else
int chbch_stats_read(char *buffer, char **my_buffer, off_t off, int length)
#endif
{

  int len = 0,i,fg,eNB;
  /*
   * Get the current time and format it.
   */
#ifdef OPENAIR1
  if (mac_xface->is_cluster_head == 0) {
    if (PHY_vars_UE_g)
      len += dump_ue_stats(PHY_vars_UE_g[0],buffer,len);
  } // is_clusterhead
  else {
    if (PHY_vars_eNB_g) 
      len += dump_eNB_stats(PHY_vars_eNB_g[0],buffer,len);
  }
#endif

  return len;
}

/*
 * Initialize the module and add the /proc file.
 */
#ifndef USER_MODE
int add_openair1_stats(void)
{
 
  msg("Creating openair1 proc entry\n"); 
  proc_openair1_root = proc_mkdir("openair1",0);
  
  //  create_proc_info_entry("bch_stats", S_IFREG | S_IRUGO, proc_openair1_root, chbch_stats_read);
  //  create_proc_info_entry("openair1_state", S_IFREG | S_IRUGO, proc_openair1_root, openair1_state_read);
  create_proc_read_entry("bch_stats", S_IFREG | S_IRUGO, proc_openair1_root, (read_proc_t*)&chbch_stats_read,NULL);
  create_proc_read_entry("openair1_state", S_IFREG | S_IRUGO, proc_openair1_root, (read_proc_t*)&openair1_state_read,NULL);
  return 0;
}

/*
 * Unregister the file when the module is closed.
 */
void remove_openair_stats(void)
{

  if (proc_openair1_root) {
    printk("[OPENAIR][CLEANUP] Removing openair proc entry\n");
    remove_proc_entry("bch_stats", proc_openair1_root);
    remove_proc_entry("openair1_state", proc_openair1_root);
    remove_proc_entry("openair1",NULL);
  }
}
#endif
