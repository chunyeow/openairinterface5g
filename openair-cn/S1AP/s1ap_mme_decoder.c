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

/*! \file s1ap_mme_decoder.c
 * \brief s1ap decode procedures for MME
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "assertions.h"
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_decoder.h"
#include "s1ap_mme_handlers.h"

#if !defined(MME_CLIENT_TEST)
# include "intertask_interface.h"
#endif

#include "assertions.h"

static int s1ap_mme_decode_initiating(
    s1ap_message *message, S1ap_InitiatingMessage_t *initiating_p)
{
    int ret = -1;

    MessageDef *message_p;
    char       *message_string = NULL;
    size_t      message_string_size;
    MessagesIds message_id;

    DevAssert(initiating_p != NULL);

    message_string = calloc(10000, sizeof(char));
    s1ap_string_total_size = 0;

    message->procedureCode = initiating_p->procedureCode;
    message->criticality   = initiating_p->criticality;

    switch(initiating_p->procedureCode) {
        case S1ap_ProcedureCode_id_uplinkNASTransport: {
            ret = s1ap_decode_s1ap_uplinknastransporties(
                      &message->msg.s1ap_UplinkNASTransportIEs, &initiating_p->value);
            s1ap_xer_print_s1ap_uplinknastransport(s1ap_xer__print2sp, message_string,
                                                   message);
            message_id = S1AP_UPLINK_NAS_LOG;
        }
        break;

        case S1ap_ProcedureCode_id_S1Setup: {
            ret = s1ap_decode_s1ap_s1setuprequesties(&message->msg.s1ap_S1SetupRequestIEs,
                    &initiating_p->value);
            s1ap_xer_print_s1ap_s1setuprequest(s1ap_xer__print2sp, message_string, message);
            message_id = S1AP_S1_SETUP_LOG;
        }
        break;

        case S1ap_ProcedureCode_id_initialUEMessage: {
            ret = s1ap_decode_s1ap_initialuemessageies(
                      &message->msg.s1ap_InitialUEMessageIEs,
                      &initiating_p->value);
            s1ap_xer_print_s1ap_initialuemessage(s1ap_xer__print2sp, message_string,
                                                 message);
            message_id = S1AP_INITIAL_UE_MESSAGE_LOG;
        }
        break;

        case S1ap_ProcedureCode_id_UEContextReleaseRequest: {
            ret = s1ap_decode_s1ap_uecontextreleaserequesties(
                      &message->msg.s1ap_UEContextReleaseRequestIEs, &initiating_p->value);
            s1ap_xer_print_s1ap_uecontextreleaserequest(s1ap_xer__print2sp, message_string,
                    message);
            message_id = S1AP_UE_CONTEXT_RELEASE_REQ_LOG;
        }
        break;

        case S1ap_ProcedureCode_id_UECapabilityInfoIndication: {
            ret = s1ap_decode_s1ap_uecapabilityinfoindicationies(
                      &message->msg.s1ap_UECapabilityInfoIndicationIEs, &initiating_p->value);
            s1ap_xer_print_s1ap_uecapabilityinfoindication(s1ap_xer__print2sp,
                    message_string, message);
            message_id = S1AP_UE_CAPABILITY_IND_LOG;
        }
        break;

        default: {
            S1AP_ERROR("Unknown procedure ID (%d) for initiating message\n",
                       (int)initiating_p->procedureCode);
            AssertFatal(0, "Unknown procedure ID (%d) for initiating message\n",
                       (int)initiating_p->procedureCode);
        }
        break;
    }

    message_string_size = strlen(message_string);

    message_p = itti_alloc_new_message_sized(TASK_S1AP, message_id, message_string_size + sizeof (IttiMsgText));
    message_p->ittiMsg.s1ap_uplink_nas_log.size = message_string_size;
    memcpy(&message_p->ittiMsg.s1ap_uplink_nas_log.text, message_string, message_string_size);

    itti_send_msg_to_task(TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);

    free(message_string);

    return ret;
}

static int s1ap_mme_decode_successfull_outcome(
    s1ap_message *message, S1ap_SuccessfulOutcome_t *successfullOutcome_p)
{
    int ret                         = -1;
    MessageDef *message_p           = NULL;
    char       *message_string      = NULL;
    size_t      message_string_size = 0;
    MessagesIds message_id          = MESSAGES_ID_MAX;

    DevAssert(successfullOutcome_p != NULL);

    message_string = calloc(10000, sizeof(char));
    s1ap_string_total_size = 0;

    message->procedureCode = successfullOutcome_p->procedureCode;
    message->criticality   = successfullOutcome_p->criticality;

    switch(successfullOutcome_p->procedureCode) {
        case S1ap_ProcedureCode_id_InitialContextSetup: {
            ret = s1ap_decode_s1ap_initialcontextsetupresponseies(
                      &message->msg.s1ap_InitialContextSetupResponseIEs,
                      &successfullOutcome_p->value);
            s1ap_xer_print_s1ap_initialcontextsetupresponse(s1ap_xer__print2sp,
                    message_string, message);
            message_id = S1AP_INITIAL_CONTEXT_SETUP_LOG;
        }
        break;

        case S1ap_ProcedureCode_id_UEContextRelease: {
            ret = s1ap_decode_s1ap_uecontextreleasecompleteies(
                      &message->msg.s1ap_UEContextReleaseCompleteIEs, &successfullOutcome_p->value);
            s1ap_xer_print_s1ap_uecontextreleasecomplete(s1ap_xer__print2sp,
                    message_string, message);
            message_id = S1AP_UE_CONTEXT_RELEASE_LOG;
        }
        break;

        default: {
            S1AP_ERROR("Unknown procedure ID (%ld) for successfull outcome message\n",
                       successfullOutcome_p->procedureCode);
        }
        break;
    }

    message_string_size = strlen(message_string);

    message_p = itti_alloc_new_message_sized(TASK_S1AP, message_id, message_string_size + sizeof (IttiMsgText));
    message_p->ittiMsg.s1ap_initial_context_setup_log.size = message_string_size;
    memcpy(&message_p->ittiMsg.s1ap_initial_context_setup_log.text, message_string, message_string_size);

    itti_send_msg_to_task(TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);

    free(message_string);

    return ret;
}

static int s1ap_mme_decode_unsuccessfull_outcome(
    s1ap_message *message, S1ap_UnsuccessfulOutcome_t *unSuccessfulOutcome_p)
{
    int ret = -1;

    MessageDef *message_p;
    char       *message_string = NULL;
    size_t      message_string_size;
    MessagesIds message_id;

    DevAssert(unSuccessfulOutcome_p != NULL);

    message_string = calloc(10000, sizeof(char));
    s1ap_string_total_size = 0;

    message->procedureCode = unSuccessfulOutcome_p->procedureCode;
    message->criticality   = unSuccessfulOutcome_p->criticality;

    switch(unSuccessfulOutcome_p->procedureCode) {
        case S1ap_ProcedureCode_id_InitialContextSetup: {
            ret = s1ap_decode_s1ap_initialcontextsetupfailureies(
                      &message->msg.s1ap_InitialContextSetupFailureIEs, &unSuccessfulOutcome_p->value);
            s1ap_xer_print_s1ap_initialcontextsetupfailure(s1ap_xer__print2sp,
                    message_string, message);
            message_id = S1AP_INITIAL_CONTEXT_SETUP_LOG;
        }
        break;

        default: {
            S1AP_ERROR("Unknown procedure ID (%d) for unsuccessfull outcome message\n",
                       (int)unSuccessfulOutcome_p->procedureCode);
        }
        break;
    }

    message_string_size = strlen(message_string);

    message_p = itti_alloc_new_message_sized(TASK_S1AP, message_id, message_string_size + sizeof (IttiMsgText));
    message_p->ittiMsg.s1ap_initial_context_setup_log.size = message_string_size;
    memcpy(&message_p->ittiMsg.s1ap_initial_context_setup_log.text, message_string, message_string_size);

    itti_send_msg_to_task(TASK_UNKNOWN, INSTANCE_DEFAULT, message_p);

    free(message_string);

    return ret;
}

int s1ap_mme_decode_pdu(s1ap_message *message, uint8_t *buffer, uint32_t len)
{
    S1AP_PDU_t  pdu;
    S1AP_PDU_t *pdu_p = &pdu;
    asn_dec_rval_t dec_ret;

    DevAssert(buffer != NULL);

    memset((void *)pdu_p, 0, sizeof(S1AP_PDU_t));

    dec_ret = aper_decode(NULL,
                          &asn_DEF_S1AP_PDU,
                          (void **)&pdu_p,
                          buffer,
                          len,
                          0,
                          0);

    if (dec_ret.code != RC_OK) {
        S1AP_ERROR("Failed to decode PDU\n");
        return -1;
    }

    message->direction = pdu_p->present;

    switch(pdu_p->present) {
        case S1AP_PDU_PR_initiatingMessage:
            return s1ap_mme_decode_initiating(message, &pdu_p->choice.initiatingMessage);
        case S1AP_PDU_PR_successfulOutcome:
            return s1ap_mme_decode_successfull_outcome(message,
                    &pdu_p->choice.successfulOutcome);
        case S1AP_PDU_PR_unsuccessfulOutcome:
            return s1ap_mme_decode_unsuccessfull_outcome(message,
                    &pdu_p->choice.unsuccessfulOutcome);
        default:
            S1AP_ERROR("Unknown message outcome (%d) or not implemented",
                       (int)pdu_p->present);
            break;
    }
    return -1;
}
