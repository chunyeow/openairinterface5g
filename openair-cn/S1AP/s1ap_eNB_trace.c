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

#include <stdint.h>

#include "intertask_interface.h"

#include "s1ap_eNB_default_values.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB_defs.h"

#include "s1ap_eNB.h"
#include "s1ap_eNB_ue_context.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_trace.h"

#include "sctp_primitives_client.h"

#include "assertions.h"

// int s1ap_eNB_generate_trace_failure(sctp_data_t        *sctp_data_p,
//                                     int32_t             stream,
//                                     uint32_t            eNB_ue_s1ap_id,
//                                     uint32_t            mme_ue_s1ap_id,
//                                     E_UTRAN_Trace_ID_t *trace_id,
//                                     Cause_t            *cause_p)
// {
//     s1ap_message message;
//     TraceFailureIndicationIEs_t *trace_failure_p;
//     uint8_t  *buffer;
//     uint32_t  length;
//     int       ret;
// 
//     DevAssert(sctp_data_p != NULL);
// 
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     trace_failure_p = &message.msg.traceFailureIndicationIEs;
// 
//     trace_failure_p->mme_ue_s1ap_id = mme_ue_s1ap_id;
//     trace_failure_p->eNB_UE_S1AP_ID = eNB_ue_s1ap_id;
// 
//     memcpy(&trace_failure_p->e_UTRAN_Trace_ID, trace_id, sizeof(E_UTRAN_Trace_ID_t));
//     memcpy(&trace_failure_p->cause, cause_p, sizeof(Cause_t));
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         return -1;
//     }
//     if ((ret = sctp_send_msg(sctp_data_p, S1AP_SCTP_PPID,
//                              stream, buffer, length)) < 0) {
//         S1AP_ERROR("Failed to send Trace failure\n");
//     }
//     free(buffer);
//     return ret;
// }

int s1ap_eNB_handle_trace_start(uint32_t               assoc_id,
                                uint32_t               stream,
                                struct s1ap_message_s *message_p)
// int s1ap_eNB_handle_trace_start(eNB_mme_desc_t *eNB_desc_p,
//                                 sctp_queue_item_t *packet_p,
//                                 struct s1ap_message_s *message_p)
{
    TraceStartIEs_t              *trace_start_p;
    struct s1ap_eNB_ue_context_s *ue_desc_p;

    DevAssert(message_p != NULL);

    trace_start_p = &message_p->msg.traceStartIEs;

//     if ((ue_desc_p = s1ap_eNB_get_ue_context(eNB_desc_p,
//                      trace_start_p->eNB_UE_S1AP_ID)) == NULL) {
//         /* Could not find context associated with this eNB_ue_s1ap_id -> generate
//          * trace failure indication.
//          */
//         struct s1ap_eNB_mme_data_s *mme_ref_p;
//         E_UTRAN_Trace_ID_t trace_id;
//         Cause_t cause;
// 
//         memset(&trace_id, 0, sizeof(E_UTRAN_Trace_ID_t));
//         memset(&cause, 0, sizeof(Cause_t));
//         mme_ref_p = s1ap_eNB_get_MME(eNB_desc_p, packet_p->assoc_id);
// 
//         cause.present = Cause_PR_radioNetwork;
//         cause.choice.radioNetwork = CauseRadioNetwork_unknown_pair_ue_s1ap_id;
// 
//         return s1ap_eNB_generate_trace_failure(&mme_ref_p->sctp_data,
//                                                packet_p->local_stream,
//                                                trace_start_p->eNB_UE_S1AP_ID,
//                                                trace_start_p->mme_ue_s1ap_id, &trace_id, &cause);
//     }
    return 0;
}

int s1ap_eNB_handle_deactivate_trace(uint32_t               assoc_id,
                                     uint32_t               stream,
                                     struct s1ap_message_s *message_p)
// int s1ap_eNB_handle_deactivate_trace(eNB_mme_desc_t *eNB_desc_p,
//                                      sctp_queue_item_t *packet_p,
//                                      struct s1ap_message_s *message_p)
{
    DeactivateTraceIEs_t *deactivate_trace_p;

    deactivate_trace_p = &message_p->msg.deactivateTraceIEs;

    return 0;
}
