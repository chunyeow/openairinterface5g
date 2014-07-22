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
* \author Navid Nikaein, Raymond Knopp
* \date 2012 - 2014
* \version 0.5
* \email navid.nikaein@eurecom.fr
* @ingroup _mac

*/

#include "defs.h"
#include "extern.h"
#include "UTIL/LOG/log.h"
#include "proto.h"

int8_t get_Po_NOMINAL_PUSCH(module_id_t module_idP) {
  RACH_ConfigCommon_t *rach_ConfigCommon = NULL;

  if (UE_mac_inst[module_idP].radioResourceConfigCommon)
    rach_ConfigCommon = &UE_mac_inst[module_idP].radioResourceConfigCommon->rach_ConfigCommon;
  else {
    LOG_E(MAC,"[UE %d] FATAL radioResourceConfigCommon is NULL !!!\n",module_idP);
    mac_xface->macphy_exit("FATAL radioResourceConfigCommon is NULL");
  }

  return(-120 + (rach_ConfigCommon->powerRampingParameters.preambleInitialReceivedTargetPower<<1) + 
	 get_DELTA_PREAMBLE(module_idP));
}

int8_t get_deltaP_rampup(module_id_t module_idP) {

  LOG_D(MAC,"[PUSCH]%d dB\n",UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER<<1);
  return((int8_t)(UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER<<1));
 
}
