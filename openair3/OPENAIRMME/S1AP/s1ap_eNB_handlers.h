/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#include "s1ap_ies_defs.h"

#ifndef S1AP_ENB_HANDLERS_H_
#define S1AP_ENB_HANDLERS_H_

int s1ap_eNB_handle_s1_setup_response(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_eNB_handle_s1_setup_failure(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_eNB_handle_initial_context_setup(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_eNB_handle_downlink_nas_transport(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

#endif /* S1AP_ENB_HANDLERS_H_ */
