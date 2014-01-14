/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 2013 Eurecom

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


/*! \file rrc_UE_ral.h
 * \brief rrc procedures for handling RAL messages
 * \author Lionel GAUTHIER
 * \date 2013
 * \version 1.0
 * \company Eurecom
 * \email: lionel.gauthier@eurecom.fr
 */
#ifndef __RRC_UE_RAL_H__
#    define __RRC_UE_RAL_H__
//-----------------------------------------------------------------------------
#        ifdef RRC_UE_RAL_C
#            define private_rrc_ue_ral(x)    x
#            define protected_rrc_ue_ral(x)  x
#            define public_rrc_ue_ral(x)     x
#        else
#            ifdef RRC_UE
#                define private_rrc_ue_ral(x)
#                define protected_rrc_ue_ral(x)  extern x
#                define public_rrc_ue_ral(x)     extern x
#            else
#                define private_rrc_ue_ral(x)
#                define protected_rrc_ue_ral(x)
#                define public_rrc_ue_ral(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
#include "intertask_interface.h"
#include "ral_messages_types.h"
#include "defs.h"

private_rrc_ue_ral(  int rrc_ue_ral_delete_all_thresholds_type        (unsigned int mod_idP, ral_link_param_type_t *param_type_pP);)
private_rrc_ue_ral(  int rrc_ue_ral_delete_threshold                  (unsigned int mod_idP, ral_link_param_type_t *param_type_pP, ral_threshold_t *threshold_pP);)
protected_rrc_ue_ral(int rrc_ue_ral_handle_configure_threshold_request(unsigned int mod_idP, MessageDef *msg_pP);)
#endif
