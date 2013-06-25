
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

/*! \file otg_vars.h
* \brief Global structure  
* \author A. Hafsaoui
* \date 2011
* \version 1.0
* \company Eurecom
* \email: openair_tech@eurecom.fr
* \note
* \warning
*/
#ifndef __OTG_VARS_H__
#    define __OTG_VARS_H__


/*!< \brief main log variables */
otg_t *g_otg; /*!< \brief  global params */
otg_multicast_t *g_otg_multicast; /*!< \brief  global params */
otg_info_t *otg_info; /*!< \brief  info otg: measurements about the simulation  */
otg_multicast_info_t *otg_multicast_info; /*!< \brief  info otg: measurements about the simulation  */
otg_forms_info_t *otg_forms_info;



//int ptime; /*!< \brief ptime is uses to compute the local simulation time */

#endif
