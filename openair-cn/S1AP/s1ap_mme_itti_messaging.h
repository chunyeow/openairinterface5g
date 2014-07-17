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

#include <stdint.h>
#include <string.h>

#include "intertask_interface.h"

#ifndef S1AP_MME_ITTI_MESSAGING_H_
#define S1AP_MME_ITTI_MESSAGING_H_

int s1ap_mme_itti_send_sctp_request(uint8_t *buffer, uint32_t length,
                                    uint32_t assoc_id, uint16_t stream);

int s1ap_mme_itti_nas_uplink_ind(const uint32_t ue_id, uint8_t *const buffer,
                                 const uint32_t length);

int s1ap_mme_itti_nas_downlink_cnf(const uint32_t ue_id,
                                   nas_error_code_t error_code);


static inline void s1ap_mme_itti_mme_app_establish_ind(
    const uint32_t  ue_id,
    const uint8_t * const nas_msg,
    const uint32_t  nas_msg_length,
    const long      cause,
    const uint16_t  tac)
{
    MessageDef  *message_p = NULL;

    message_p = itti_alloc_new_message(TASK_S1AP, MME_APP_CONNECTION_ESTABLISHMENT_IND);

    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).mme_ue_s1ap_id           = ue_id;

    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.UEid                 = ue_id;
    /* Mapping between asn1 definition and NAS definition */
    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.asCause              = cause + 1;
    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.tac                  = tac;
    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.initialNasMsg.length = nas_msg_length;

    MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.initialNasMsg.data   = malloc(sizeof(uint8_t) * nas_msg_length);
    memcpy(MME_APP_CONNECTION_ESTABLISHMENT_IND(message_p).nas.initialNasMsg.data, nas_msg, nas_msg_length);

    // should be sent to MME_APP, but this one would forward it to NAS_MME, so send it directly to NAS_MME
    // but let's see
    itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}



static inline void s1ap_mme_itti_nas_establish_ind(
    const uint32_t ue_id, uint8_t * const nas_msg, const uint32_t nas_msg_length,
    const long cause, const uint16_t tac)
{
    MessageDef     *message_p;

    message_p = itti_alloc_new_message(TASK_S1AP, NAS_CONNECTION_ESTABLISHMENT_IND);

    NAS_CONN_EST_IND(message_p).nas.UEid                 = ue_id;
    /* Mapping between asn1 definition and NAS definition */
    NAS_CONN_EST_IND(message_p).nas.asCause              = cause + 1;
    NAS_CONN_EST_IND(message_p).nas.tac                  = tac;
    NAS_CONN_EST_IND(message_p).nas.initialNasMsg.length = nas_msg_length;

    NAS_CONN_EST_IND(message_p).nas.initialNasMsg.data = malloc(sizeof(uint8_t) * nas_msg_length);
    memcpy(NAS_CONN_EST_IND(message_p).nas.initialNasMsg.data, nas_msg, nas_msg_length);

    // should be sent to MME_APP, but this one would forward it to NAS_MME, so send it directly to NAS_MME
    // but let's see
    itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}

#endif /* S1AP_MME_ITTI_MESSAGING_H_ */
