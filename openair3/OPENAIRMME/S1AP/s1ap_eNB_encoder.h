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

#ifndef S1AP_ENB_ENCODER_H_
#define S1AP_ENB_ENCODER_H_

int s1ap_eNB_generate_s1_setup_request(struct s1ap_eNB_description_s* eNB_ref);

int s1ap_eNB_generate_initial_ue_message(struct s1ap_eNB_UE_description_s *ue_ref,
                                         uint8_t                          *nas_pdu,
                                         uint32_t                          nas_len);

int s1ap_eNB_encode_initial_ue_message(InitialUEMessageIEs_t *initialUEmessageIEs_p,
                                       uint8_t              **buffer,
                                       uint32_t              *length);

int s1ap_eNB_encode_initial_context_setup_response(InitialContextSetupResponseIEs_t  *sptr,
                                                   uint8_t             **buffer,
                                                   uint32_t             *length);

int s1ap_eNB_encode_s1_setup_request(S1SetupRequestIEs_t  *s1SetupRequestIEs,
                                     uint8_t             **buffer,
                                     uint32_t             *length);

int s1ap_eNB_generate_uplink_nas_transport(struct s1ap_eNB_UE_description_s *ue_ref,
                                           uint8_t                          *nas_pdu,
                                           uint32_t                          nas_len);

#endif /* S1AP_ENB_ENCODER_H_ */
