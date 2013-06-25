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
#include "intertask_interface.h"

#ifndef S1AP_MME_HANDLERS_H_
#define S1AP_MME_HANDLERS_H_

int s1ap_mme_handle_message(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

/** \brief Handle an S1 Setup request message.
 * Typically add the eNB in the list of served eNB if not present, simply reset
 * UEs association otherwise. S1SetupResponse message is sent in case of success or
 * S1SetupFailure if the MME cannot accept the configuration received.
 * \param sptr Pointer to the IES structure
 * \param ptr_size Size of the structure given in parameter
 * @returns int
 **/
int s1ap_mme_handle_s1_setup_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

/** \brief Handle an Uplink NAS transport message.
 * Process the RRC transparent container and forward it to NAS entity.
 * \param sptr Pointer to the IES structure
 * \param ptr_size Size of the structure given in parameter
 * @returns NONE
 **/
int s1ap_mme_handle_uplink_nas_transport(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_initial_ue_message(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_path_switch_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_ue_context_release_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_ue_context_release_complete(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_initial_context_setup_failure(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message);

int s1ap_mme_handle_initial_context_setup_response(
    uint32_t assocId,
    uint32_t stream,
    struct s1ap_message_s *message);

int s1ap_generate_downlink_nas_transport(S1apNASNewMessageInd *message);

int s1ap_handle_sctp_deconnection(uint8_t assoc_id);

int s1ap_handle_create_session_response(SgwCreateSessionResponse *session_response_p);

#endif /* S1AP_MME_HANDLERS_H_ */
