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
/*! \file l1_helper.c
* \brief phy helper function
* \author Raymond Knopp
* \date 2012
* \version 0.5
* @ingroup _mac

*/

#include "defs.h"
#include "extern.h"
#include "UTIL/LOG/log.h"

s8 get_Po_NOMINAL_PUSCH(u8 Mod_id) {
  RACH_ConfigCommon_t *rach_ConfigCommon = NULL;

  if (UE_mac_inst[Mod_id].radioResourceConfigCommon)
    rach_ConfigCommon = &UE_mac_inst[Mod_id].radioResourceConfigCommon->rach_ConfigCommon;
  else {
    LOG_E(MAC,"[UE %d] FATAL radioResourceConfigCommon is NULL !!!\n",Mod_id);
    mac_xface->macphy_exit("");
  }

  return(-120 + (rach_ConfigCommon->powerRampingParameters.preambleInitialReceivedTargetPower<<1) + 
	 get_DELTA_PREAMBLE(Mod_id));
}

s8 get_deltaP_rampup(u8 Mod_id) {

  LOG_D(MAC,"[PUSCH]%d dB\n",UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER<<1);
  return((s8)(UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER<<1));
 
}
