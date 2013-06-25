/*******************************************************************************

  Eurecom OpenAirInterface 2
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


/*! \file oaisim_config.h
* \brief configure an emulation
* \author Lusheng Wang
* \date 2006-2010
* \version 4.0 
* \company Eurecom
* \email: wangl@eurecom.fr
* \note this a note 
* \bug 	this is a bug
* \warning  this is a warning
*/ 

//-----------------------------------begin group-----------------------------
/** @defgroup _oaisim The sturcture of OAISIM

The current sturcture of oaisim is shown by the figure.

\image html new_OCG_structure.png "new_OCG_structure"


 * @{*/
 
/* @}*/

#include "UTIL/LOG/log_if.h"
#include "UTIL/LOG/log_extern.h"
#include "UTIL/OCG/OCG.h"
#include "UTIL/OPT/opt.h" // to test OPT
#include "UTIL/OMG/omg.h"
#include "UTIL/CLI/cli_if.h"
#include "SIMULATION/ETH_TRANSPORT/defs.h"

/** @defgroup _init_oai Initial oaisim
 *  @ingroup _fn
 *  @brief Initialize all the parameters before start an emulation
 * @{*/ 
	void init_oai_emulation();
/* @}*/

/** @defgroup _config_oaisim All the configurations for an emulation
 *  @ingroup _fn
 *  @brief This is the function that calls all the other configuration functions
 * @{*/ 
void oaisim_config();
/* @}*/


/** @defgroup _config_module Configuration functions for various modules
 *  @ingroup _fn
 *  @brief There are the functions to configure different various modules in the emulator
 * @{*/ 
int olg_config();
int ocg_config_env();
int ocg_config_omg();
int ocg_config_topo();
int ocg_config_app();
int ocg_config_emu();
/* @}*/

