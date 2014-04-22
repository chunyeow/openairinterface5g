/*******************************************************************************
Eurecom OpenAirInterface Core Network
Copyright(c) 1999 - 2014 Eurecom

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
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
/*! \file mme_app_bearer.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intertask_interface.h"
#include "mme_config.h"

#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"

#include "secu_defs.h"

#include "assertions.h"
#include "common_types.h"

int
mme_app_handle_s1ap_ue_capabilities_ind(
        const s1ap_ue_cap_ind_t  const * s1ap_ue_cap_ind_pP)
{
  DevAssert(s1ap_ue_cap_ind_pP != NULL);
  //unsigned eNB_ue_s1ap_id:24;
  //uint32_t mme_ue_s1ap_id;
  //uint8_t  radio_capabilities[100];
  //uint32_t radio_capabilities_length;
  return 0;
}
