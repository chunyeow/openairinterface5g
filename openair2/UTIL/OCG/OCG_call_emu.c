/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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

/*! \file OCG_call_emu.c
* \brief Call the emulator
* \author Lusheng Wang
* \date 2011
* \version 0.1
* \company Eurecom
* \email: lusheng.wang@eurecom.fr
* \note
* \warning
*/

/*--- INCLUDES ---------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "OCG_vars.h"
#include "OCG_call_emu.h"
#include "UTIL/LOG/log.h"
/*----------------------------------------------------------------------------*/

OAI_Emulation oai_emulation;

int call_emu(char dst_dir[DIR_LENGTH_MAX]) {


	////////// print the configuration 
	FILE *file;
	char dst_file[DIR_LENGTH_MAX] = "";
	strcat(dst_file, dst_dir);
	strcat(dst_file, "emulation_result.txt");
	file = fopen(dst_file,"w");
	//system("../../../openair1/SIMULATION/LTE_PHY_L2/physim --help");
	
	fclose(file);

	LOG_I(OCG, "Emulation finished\n");
	return MODULE_OK;
}
