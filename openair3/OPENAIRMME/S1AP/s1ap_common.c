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

/*! \file s1ap_common.c
 * \brief s1ap procedures for both eNB and MME
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <stdint.h>

#include "S1AP-PDU.h"

#include "S1SetupRequest.h"
#include "UplinkNASTransport.h"

#include "s1ap_common.h"

int asn_debug = 0;
int asn1_xer_print = 0;

ssize_t s1ap_generate_initiating_message(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr) {

    S1AP_PDU_t pdu;
    ssize_t    encoded;

    memset(&pdu, 0, sizeof(S1AP_PDU_t));

    pdu.present = S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage.procedureCode = procedureCode;
    pdu.choice.initiatingMessage.criticality   = criticality;
    ANY_fromType_aper(&pdu.choice.initiatingMessage.value, td, sptr);

    if (asn1_xer_print)
        xer_fprint(stdout,&asn_DEF_S1AP_PDU,(void*)&pdu);

    if ((encoded = aper_encode_to_new_buffer(&asn_DEF_S1AP_PDU, 0, &pdu, (void**)buffer)) < 0) {
        return -1;
    }

    *length = encoded;
    return encoded;
}

ssize_t s1ap_generate_successfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr) {

    S1AP_PDU_t pdu;
    ssize_t    encoded;

    memset(&pdu, 0, sizeof(S1AP_PDU_t));

    pdu.present = S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome.procedureCode = procedureCode;
    pdu.choice.successfulOutcome.criticality   = criticality;
    ANY_fromType_aper(&pdu.choice.successfulOutcome.value, td, sptr);

    if ((encoded = aper_encode_to_new_buffer(&asn_DEF_S1AP_PDU, 0, &pdu, (void**)buffer)) < 0) {
        return -1;
    }

    *length = encoded;

    return encoded;
}

ssize_t s1ap_generate_unsuccessfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr) {

    S1AP_PDU_t pdu;
    ssize_t    encoded;

    memset(&pdu, 0, sizeof(S1AP_PDU_t));

    pdu.present = S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.successfulOutcome.procedureCode = procedureCode;
    pdu.choice.successfulOutcome.criticality   = criticality;
    ANY_fromType_aper(&pdu.choice.successfulOutcome.value, td, sptr);

    if ((encoded = aper_encode_to_new_buffer(&asn_DEF_S1AP_PDU, 0, &pdu, (void**)buffer)) < 0) {
        return -1;
    }

    *length = encoded;

    return encoded;
}

IE_t *s1ap_new_ie(
    ProtocolIE_ID_t        id,
    Criticality_t          criticality,
    asn_TYPE_descriptor_t *type,
    void                  *sptr) {

    IE_t *buff;

    if ((buff = malloc(sizeof(IE_t))) == NULL) {
        // Possible error on malloc
        return NULL;
    }
    memset((void*)buff, 0, sizeof(IE_t));

    buff->id = id;
    buff->criticality = criticality;

    ANY_fromType_aper(&buff->value, type, sptr);

    if (asn1_xer_print)
        if (xer_fprint(stdout, &asn_DEF_IE, buff) < 0) {
            free(buff);
            return NULL;
        }

    return buff;
}

void s1ap_handle_criticality(e_Criticality criticality) {
    
}
