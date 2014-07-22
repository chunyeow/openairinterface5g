/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

 *******************************************************************************/
/*! \file eNB_scheduler.c
 * \brief procedures related to UE on the BCH transport channel
 * \author  Navid Nikaein and Raymond Knopp
 * \date 2011
 * \email: navid.nikaein@eurecom.fr
 * \version 0.5
 * @ingroup _mac

 */

#include "assertions.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/proto.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/from_grlib_softregs.h"

#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"

//#include "LAYER2/MAC/pre_processor.c"
#include "pdcp.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#define ENABLE_MAC_PAYLOAD_DEBUG
#define DEBUG_eNB_SCHEDULER 1


void schedule_SI(module_id_t module_idP,frame_t frameP, unsigned char *nprb,unsigned int *nCCE) {

  start_meas(&eNB_mac_inst[module_idP].schedule_si);

  unsigned char bcch_sdu_length;
  int mcs = -1;
  void *BCCH_alloc_pdu=(void*)&eNB_mac_inst[module_idP].BCCH_alloc_pdu;

  bcch_sdu_length = mac_rrc_data_req(module_idP,
      frameP,
      BCCH,1,
      &eNB_mac_inst[module_idP].BCCH_pdu.payload[0],
      1,
      module_idP,
      0); // not used in this case
  if (bcch_sdu_length > 0) {
      LOG_D(MAC,"[eNB %d] Frame %d : BCCH->DLSCH, Received %d bytes \n",module_idP,frameP,bcch_sdu_length);


      if (bcch_sdu_length <= (mac_xface->get_TBS_DL(0,3)))
        mcs=0;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(1,3)))
        mcs=1;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(2,3)))
        mcs=2;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(3,3)))
        mcs=3;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(4,3)))
        mcs=4;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(5,3)))
        mcs=5;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(6,3)))
        mcs=6;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(7,3)))
        mcs=7;
      else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(8,3)))
        mcs=8;

      if (mac_xface->lte_frame_parms->frame_type == TDD) {
          switch (mac_xface->lte_frame_parms->N_RB_DL) {
          case 6:
            ((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 25:
            ((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 50:
            ((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 100:
            ((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;

          }
      }
      else {
          switch (mac_xface->lte_frame_parms->N_RB_DL) {
          case 6:
            ((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 25:
            ((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 50:
            ((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;
          case 100:
            ((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
            break;

          }
      }

#if defined(USER_MODE) && defined(OAI_EMU)
      if (oai_emulation.info.opt_enabled) {
        trace_pdu(1,
                  &eNB_mac_inst[module_idP].BCCH_pdu.payload[0],
                  bcch_sdu_length,
                  0xffff,
                  4,
                  0xffff,
                  eNB_mac_inst[module_idP].subframe,
                  0,
                  0);
      }
      LOG_D(OPT,"[eNB %d][BCH] Frame %d trace pdu for rnti %x with size %d\n",
                 module_idP, frameP, 0xffff, bcch_sdu_length);
#endif

      if (mac_xface->lte_frame_parms->frame_type == TDD) {
          LOG_D(MAC,"[eNB] Frame %d : Scheduling BCCH->DLSCH (TDD) for SI %d bytes (mcs %d, rb 3, TBS %d)\n",
              frameP,
              bcch_sdu_length,
              mcs,
              mac_xface->get_TBS_DL(mcs,3));
      }
      else {
          LOG_D(MAC,"[eNB] Frame %d : Scheduling BCCH->DLSCH (FDD) for SI %d bytes (mcs %d, rb 3, TBS %d)\n",
              frameP,
              bcch_sdu_length,
              mcs,
              mac_xface->get_TBS_DL(mcs,3));
      }
      eNB_mac_inst[module_idP].bcch_active=1;
      *nprb=3;
      *nCCE=4;
  }
  else {
    eNB_mac_inst[module_idP].bcch_active=0;
    *nprb=0;
    *nCCE=0;
    //LOG_D(MAC,"[eNB %d] Frame %d : BCCH not active \n",Mod_id,frame);
  }
  // this might be misleading when bcch is inactive
  stop_meas(&eNB_mac_inst[module_idP].schedule_si);
  return;
}
