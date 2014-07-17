/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

#include "s1ap_ies_defs.h"
#include "intertask_interface.h"

#ifndef S1AP_MME_HANDLERS_H_
#define S1AP_MME_HANDLERS_H_

/** \brief Handle decoded incoming messages from SCTP
 * \param assoc_id SCTP association ID
 * \param stream Stream number
 * \param message_p The message decoded by the ASN1C decoder
 * @returns int
 **/
int s1ap_mme_handle_message(uint32_t assoc_id, uint32_t stream,
                            struct s1ap_message_s *message_p);

int s1ap_mme_handle_ue_cap_indication(uint32_t assoc_id, uint32_t stream,
                                      struct s1ap_message_s *message);

/** \brief Handle an S1 Setup request message.
 * Typically add the eNB in the list of served eNB if not present, simply reset
 * UEs association otherwise. S1SetupResponse message is sent in case of success or
 * S1SetupFailure if the MME cannot accept the configuration received.
 * \param assoc_id SCTP association ID
 * \param stream Stream number
 * \param message_p The message decoded by the ASN1C decoder
 * @returns int
 **/
int s1ap_mme_handle_s1_setup_request(uint32_t assoc_id, uint32_t stream,
                                     struct s1ap_message_s *message_p);

int s1ap_mme_handle_path_switch_request(uint32_t assoc_id, uint32_t stream,
                                        struct s1ap_message_s *message_p);

int s1ap_mme_handle_ue_context_release_request(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message_p);

int s1ap_mme_handle_ue_context_release_complete(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message_p);

int s1ap_mme_handle_initial_context_setup_failure(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message_p);

int s1ap_mme_handle_initial_context_setup_response(
    uint32_t assoc_id,
    uint32_t stream,
    struct s1ap_message_s *message_p);

int s1ap_handle_sctp_deconnection(uint32_t assoc_id);

int s1ap_handle_new_association(sctp_new_peer_t *sctp_new_peer_p);

int s1ap_handle_create_session_response(SgwCreateSessionResponse
                                        *session_response_p);

int s1ap_mme_set_cause(S1ap_Cause_t *cause_p, S1ap_Cause_PR cause_type, long cause_value);

int s1ap_mme_generate_s1_setup_failure(
    uint32_t assoc_id, S1ap_Cause_PR cause_type, long cause_value,
    long time_to_wait);

#endif /* S1AP_MME_HANDLERS_H_ */
