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

/*! \file OCG_if.h
* \brief Interfaces to the outside of OCG
* \author Lusheng Wang
* \date 2011
* \version 0.1
* \company Eurecom
* \email: lusheng.wang@eurecom.fr
* \note
* \warning
*/

#ifndef __CONFIGEN_IF_H__
#define __CONFIG_IF_H__

#ifdef __cplusplus
extern "C" {
#endif
/*--- INCLUDES ---------------------------------------------------------------*/
#    include "OCG.h"
/*----------------------------------------------------------------------------*/


#    ifdef COMPONENT_CONFIGEN
#        ifdef COMPONENT_CONFIGEN_IF
#            define private_configen_if(x) x
#            define friend_configen_if(x) x
#            define public_configen_if(x) x
#        else
#            define private_configen_if(x)
#            define friend_configen_if(x) extern x
#            define public_configen_if(x) extern x
#        endif
#    else
#        define private_configen_if(x)
#        define friend_configen_if(x)
#        define public_configen_if(x) extern x
#    endif

#ifdef __cplusplus
}
#endif

#endif
