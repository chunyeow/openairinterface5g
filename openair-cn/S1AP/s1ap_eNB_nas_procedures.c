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

#include "assertions.h"
#include "conversions.h"

#include "intertask_interface.h"

#include "s1ap_common.h"
#include "s1ap_eNB_defs.h"

#include "s1ap_ies_defs.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_ue_context.h"
#include "s1ap_eNB_nas_procedures.h"

#include "sctp_primitives_client.h"

// int s1ap_eNB_handle_nas_downlink(eNB_mme_desc_t *eNB_desc_p,
//                                  sctp_queue_item_t *packet_p,
//                                  struct s1ap_message_s *message_p)
int s1ap_eNB_handle_nas_downlink(uint32_t               assoc_id,
                                 uint32_t               stream,
                                 struct s1ap_message_s *message_p)
{
    DownlinkNASTransportIEs_t *downlink_NAS_transport_p;
    s1ap_eNB_mme_data_t       *mme_desc_p;
    s1ap_eNB_ue_context_t     *ue_desc_p;

    DevAssert(message_p != NULL);

    downlink_NAS_transport_p = &message_p->msg.downlinkNASTransportIEs;

    /* UE-related procedure -> stream != 0 */
    if (stream == 0) {
        S1AP_ERROR("[SCTP %d] Received UE-related procedure on stream == 0\n",
                   assoc_id);
        return -1;
    }

//     if ((mme_desc_p = s1ap_eNB_get_MME(eNB_desc_p, assoc_id)) == NULL) {
//         S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
//                    "existing MME context\n", assoc_id);
//         return -1;
//     }
//     if ((ue_desc_p = s1ap_eNB_get_ue_context(eNB_desc_p,
//          downlink_NAS_transport_p->eNB_UE_S1AP_ID)) == NULL)
//     {
//         S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
//                    "existing UE context\n", assoc_id);
//         return -1;
//     }

    /* Is it the first outcome of the MME for this UE ? If so store the mme
     * UE s1ap id.
     */
//     if (ue_desc_p->mme_ue_s1ap_id == 0) {
//         ue_desc_p->mme_ue_s1ap_id = downlink_NAS_transport_p->mme_ue_s1ap_id;
//     } else {
//         /* We already have a mme ue s1ap id check the received is the same */
//         if (ue_desc_p->mme_ue_s1ap_id != downlink_NAS_transport_p->mme_ue_s1ap_id) {
//             S1AP_ERROR("[SCTP %d] Mismatch is MME UE S1AP ID (0x%08x != 0x%08x)\n",
//                        downlink_NAS_transport_p->mme_ue_s1ap_id,
//                        ue_desc_p->mme_ue_s1ap_id,
//                        packet_p->assoc_id);
//         }
//     }

    /* TODO: forward NAS pdu to RRC for transmission */

    return 0;
}

// int s1ap_eNB_nas_uplink(eNB_mme_desc_t    *eNB_desc_p,
//                         s1ap_nas_uplink_t *nas_uplink_p)
// {
//     struct s1ap_eNB_ue_context_s *ue_context_p;
//     UplinkNASTransportIEs_t      *uplink_NAS_transport_p;
// 
//     s1ap_message  message;
// 
//     uint8_t  *buffer;
//     uint32_t length;
//     int      ret = -1;
// 
//     DevAssert(nas_uplink_p != NULL);
//     DevAssert(eNB_desc_p != NULL);
// 
//     if ((ue_context_p = s1ap_eNB_get_ue_context(eNB_desc_p, nas_uplink_p->eNB_ue_s1ap_id)) == NULL)
//     {
//         /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
//         S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
//                   nas_uplink_p->eNB_ue_s1ap_id);
//         return -1;
//     }
// 
//     /* Uplink NAS transport can occur either during an s1ap connected state
//      * or during initial attach (for example: NAS authentication).
//      */
//     if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
//         ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
//     {
//         S1AP_WARN("You are attempting to send NAS data over non-connected "
//                   "eNB ue s1ap id: %u, current state: %d\n",
//                   nas_uplink_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
//         return -1;
//     }
// 
//     /* Prepare the S1AP message to encode */
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     message.direction = S1AP_PDU_PR_initiatingMessage;
//     message.procedureCode = ProcedureCode_id_uplinkNASTransport;
// 
//     uplink_NAS_transport_p = &message.msg.uplinkNASTransportIEs;
// 
//     uplink_NAS_transport_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
//     uplink_NAS_transport_p->eNB_UE_S1AP_ID = ue_context_p->eNB_ue_s1ap_id;
// 
//     uplink_NAS_transport_p->nas_pdu.buf  = nas_uplink_p->nas_pdu.buffer;
//     uplink_NAS_transport_p->nas_pdu.size = nas_uplink_p->nas_pdu.length;
// 
//     MCC_MNC_TO_PLMNID(eNB_desc_p->mcc, eNB_desc_p->mnc,
//                       &uplink_NAS_transport_p->eutran_cgi.pLMNidentity);
//     MACRO_ENB_ID_TO_CELL_IDENTITY(eNB_desc_p->eNB_id,
//                                   &uplink_NAS_transport_p->eutran_cgi.cell_ID);
// 
//     /* MCC/MNC should be repeated in TAI and EUTRAN CGI */
//     MCC_MNC_TO_PLMNID(eNB_desc_p->mcc, eNB_desc_p->mnc,
//                       &uplink_NAS_transport_p->tai.pLMNidentity);
//     TAC_TO_ASN1(eNB_desc_p->tac, &uplink_NAS_transport_p->tai.tAC);
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         S1AP_ERROR("Failed to encode uplink NAS transport\n");
//         /* Encode procedure has failed... */
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

// int s1ap_eNB_initial_ctxt_resp(eNB_mme_desc_t                 *eNB_desc_p,
//                                s1ap_initial_ctxt_setup_resp_t *initial_ctxt_resp_p)
// {
//     struct s1ap_eNB_ue_context_s *ue_context_p;
//     InitialContextSetupResponseIEs_t *initial_ies_p;
// 
//     s1ap_message  message;
// 
//     uint8_t  *buffer;
//     uint32_t length;
//     int      ret = -1;
//     int      i;
// 
//     DevAssert(initial_ctxt_resp_p != NULL);
//     DevAssert(eNB_desc_p != NULL);
// 
//     if ((ue_context_p = s1ap_eNB_get_ue_context(eNB_desc_p,
//         initial_ctxt_resp_p->eNB_ue_s1ap_id)) == NULL)
//     {
//         /* The context for this eNB ue s1ap id doesn't exist in the map of eNB UEs */
//         S1AP_WARN("Failed to find ue context associated with eNB ue s1ap id: %u\n",
//                   initial_ctxt_resp_p->eNB_ue_s1ap_id);
//         return -1;
//     }
// 
//     /* Uplink NAS transport can occur either during an s1ap connected state
//      * or during initial attach (for example: NAS authentication).
//      */
//     if (!(ue_context_p->ue_state == S1AP_UE_CONNECTED ||
//         ue_context_p->ue_state == S1AP_UE_WAITING_CSR))
//     {
//         S1AP_WARN("You are attempting to send NAS data over non-connected "
//                   "eNB ue s1ap id: %u, current state: %d\n",
//                   initial_ctxt_resp_p->eNB_ue_s1ap_id, ue_context_p->ue_state);
//         return -1;
//     }
// 
//     /* Prepare the S1AP message to encode */
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     message.direction = S1AP_PDU_PR_successfulOutcome;
//     message.procedureCode = ProcedureCode_id_InitialContextSetup;
// 
//     initial_ies_p = &message.msg.initialContextSetupResponseIEs;
// 
//     initial_ies_p->eNB_UE_S1AP_ID = initial_ctxt_resp_p->eNB_ue_s1ap_id;
//     initial_ies_p->mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;
// 
//     for (i = 0; i < initial_ctxt_resp_p->nb_of_e_rabs; i++)
//     {
//         E_RABSetupItemCtxtSURes_t *new_item;
// 
//         new_item = calloc(1, sizeof(E_RABSetupItemCtxtSURes_t));
// 
//         new_item->e_RAB_ID = initial_ctxt_resp_p->e_rabs[i].e_rab_id;
//         GTP_TEID_TO_ASN1(initial_ctxt_resp_p->e_rabs[i].gtp_teid, &new_item->gTP_TEID);
//         new_item->transportLayerAddress.buf = initial_ctxt_resp_p->e_rabs[i].eNB_addr.buffer;
//         new_item->transportLayerAddress.size = initial_ctxt_resp_p->e_rabs[i].eNB_addr.length;
//         new_item->transportLayerAddress.bits_unused = 0;
// 
//         ASN_SEQUENCE_ADD(&initial_ies_p->e_RABSetupListCtxtSURes.e_RABSetupItemCtxtSURes, new_item);
//     }
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         S1AP_ERROR("Failed to encode uplink NAS transport\n");
//         /* Encode procedure has failed... */
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
