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

/*! \file s1ap_mme_decoder.c
 * \brief s1ap decode procedures for MME
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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

    DevAssert(initiating_p != NULL);

    message->procedureCode = initiating_p->procedureCode;
    message->criticality   = initiating_p->criticality;

    switch(initiating_p->procedureCode) {
//         case S1ap_ProcedureCode_id_downlinkNASTransport:
//             return s1ap_decode_downlinknastransporties(&message->msg.downlinkNASTransportIEs, &initiating_p->value);
        case S1ap_ProcedureCode_id_uplinkNASTransport: {
            ret = s1ap_decode_s1ap_uplinknastransporties(
                &message->msg.s1ap_UplinkNASTransportIEs, &initiating_p->value);
        } break;
        case S1ap_ProcedureCode_id_S1Setup: {
            ret = s1ap_decode_s1ap_s1setuprequesties(&message->msg.s1ap_S1SetupRequestIEs,
                                                &initiating_p->value);
            s1ap_xer_print_s1ap_s1setuprequest(stdout, message);
        } break;
        case S1ap_ProcedureCode_id_initialUEMessage: {
            ret = s1ap_decode_s1ap_initialuemessageies(&message->msg.s1ap_InitialUEMessageIEs,
                                                  &initiating_p->value);
            s1ap_xer_print_s1ap_initialuemessage(stdout, message);
        } break;
        case S1ap_ProcedureCode_id_UEContextReleaseRequest: {
            ret = s1ap_decode_s1ap_uecontextreleaserequesties(
                &message->msg.s1ap_UEContextReleaseRequestIEs, &initiating_p->value);
            s1ap_xer_print_s1ap_uecontextreleaserequest(stdout, message);
        } break;
        case S1ap_ProcedureCode_id_UECapabilityInfoIndication: {
            ret = s1ap_decode_s1ap_uecapabilityinfoindicationies(
                &message->msg.s1ap_UECapabilityInfoIndicationIEs, &initiating_p->value);
            s1ap_xer_print_s1ap_uecapabilityinfoindication(stdout, message);
        } break;
//         case S1ap_ProcedureCode_id_InitialContextSetup:
//             return s1ap_decode_initialcontextsetuprequesties(&message->msg.initialContextSetupRequestIEs, &initiating_p->value);
        default:
            S1AP_ERROR("Unknown procedure ID (%d) for initiating message\n",
                       (int)initiating_p->procedureCode);
            break;
    }
    return ret;
}

static int s1ap_mme_decode_successfull_outcome(
    s1ap_message *message, S1ap_SuccessfulOutcome_t *successfullOutcome_p)
{
    DevAssert(successfullOutcome_p != NULL);

    message->procedureCode = successfullOutcome_p->procedureCode;
    message->criticality   = successfullOutcome_p->criticality;

    switch(successfullOutcome_p->procedureCode) {
        case S1ap_ProcedureCode_id_InitialContextSetup:
            return s1ap_decode_s1ap_initialcontextsetupresponseies(
                       &message->msg.s1ap_InitialContextSetupResponseIEs, &successfullOutcome_p->value);
        case S1ap_ProcedureCode_id_UEContextRelease:
            return s1ap_decode_s1ap_uecontextreleasecompleteies(
                       &message->msg.s1ap_UEContextReleaseCompleteIEs, &successfullOutcome_p->value);
        default:
            S1AP_ERROR("Unknown procedure ID (%d) for successfull outcome message\n",
                       (int)successfullOutcome_p->procedureCode);
            break;
    }
    return -1;
}

static int s1ap_mme_decode_unsuccessfull_outcome(
    s1ap_message *message, S1ap_UnsuccessfulOutcome_t *unSuccessfulOutcome_p)
{
    DevAssert(unSuccessfulOutcome_p != NULL);

    message->procedureCode = unSuccessfulOutcome_p->procedureCode;
    message->criticality   = unSuccessfulOutcome_p->criticality;

    switch(unSuccessfulOutcome_p->procedureCode) {
        case S1ap_ProcedureCode_id_InitialContextSetup:
            return s1ap_decode_s1ap_initialcontextsetupfailureies(
                       &message->msg.s1ap_InitialContextSetupFailureIEs, &unSuccessfulOutcome_p->value);
        default:
            S1AP_ERROR("Unknown procedure ID (%d) for unsuccessfull outcome message\n",
                       (int)unSuccessfulOutcome_p->procedureCode);
            break;
    }
    return -1;
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
