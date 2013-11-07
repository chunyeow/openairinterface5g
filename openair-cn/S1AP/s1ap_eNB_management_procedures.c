/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "intertask_interface.h"

#include "assertions.h"
// #include "conversions.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB_defs.h"
#include "s1ap_eNB.h"

#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_management_procedures.h"

#include "sctp_primitives_client.h"

// int s1ap_eNB_ue_capabilities(eNB_mme_desc_t         *eNB_desc_p,
//                              s1ap_ue_cap_info_ind_t *ue_cap_info_ind_p)
// {
//     struct s1ap_eNB_ue_context_s *ue_context_p;
//     UECapabilityInfoIndicationIEs_t *ue_cap_info_ind_ies_p;
// 
//     s1ap_message  message;
// 
//     uint8_t  *buffer;
//     uint32_t length;
//     int      ret = -1;
// 
//     DevAssert(ue_cap_info_ind_p != NULL);
//     DevAssert(eNB_desc_p != NULL);
// 
//     if ((ue_context_p = s1ap_eNB_get_ue_context(eNB_desc_p, ue_cap_info_ind_p->eNB_ue_s1ap_id)) == NULL)
//     {
//         /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
//         S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
//                   ue_cap_info_ind_p->eNB_ue_s1ap_id);
//         return -1;
//     }
// 
//     /* UE capabilities message can occur either during an s1ap connected state
//      * or during initial attach (for example: NAS authentication).
//      */
//     if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
//         ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
//     {
//         S1AP_WARN("You are attempting to send NAS data over non-connected "
//         "eNB ue s1ap id: %u, current state: %d\n",
//         ue_cap_info_ind_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
//         return -1;
//     }
// 
//     /* Prepare the S1AP message to encode */
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     message.direction = S1AP_PDU_PR_initiatingMessage;
//     message.procedureCode = ProcedureCode_id_UECapabilityInfoIndication;
// 
//     ue_cap_info_ind_ies_p = &message.msg.ueCapabilityInfoIndicationIEs;
// 
//     ue_cap_info_ind_ies_p->ueRadioCapability.buf = ue_cap_info_ind_p->ue_radio_cap.buffer;
//     ue_cap_info_ind_ies_p->ueRadioCapability.size = ue_cap_info_ind_p->ue_radio_cap.length;
// 
//     ue_cap_info_ind_ies_p->eNB_UE_S1AP_ID = ue_cap_info_ind_p->eNB_ue_s1ap_id;
//     ue_cap_info_ind_ies_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         /* Encode procedure has failed... */
//         S1AP_ERROR("Failed to encode UE capabilities indication\n");
//         return -1;
//     }
// 
//     /* UE associated signalling -> use the allocated stream */
//     if ((ret = sctp_send_msg(&ue_context_p->mme_ref->sctp_data, S1AP_SCTP_PPID,
//         ue_context_p->stream, buffer, length)) < 0)
//     {
//         S1AP_ERROR("[SCTP %d] Failed to send message over SCTP: %d\n",
//                    ue_context_p->mme_ref->sctp_data.assoc_id, ret);
//     }
// 
//     free(buffer);
//     return ret;
// }
