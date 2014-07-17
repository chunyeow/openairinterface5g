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

#include "s1ap_mme_itti_messaging.h"

int s1ap_mme_itti_send_sctp_request(uint8_t *buffer, uint32_t length,
                                    uint32_t assoc_id, uint16_t stream)
{
    MessageDef     *message_p;

    message_p = itti_alloc_new_message(TASK_S1AP, SCTP_DATA_REQ);

    SCTP_DATA_REQ(message_p).buffer  = buffer;
    SCTP_DATA_REQ(message_p).bufLen  = length;
    SCTP_DATA_REQ(message_p).assocId = assoc_id;
    SCTP_DATA_REQ(message_p).stream  = stream;

    return itti_send_msg_to_task(TASK_SCTP, INSTANCE_DEFAULT, message_p);
}

int s1ap_mme_itti_nas_uplink_ind(const uint32_t ue_id, uint8_t * const buffer,
                                 const uint32_t length)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_S1AP, NAS_UPLINK_DATA_IND);

    NAS_UL_DATA_IND(message_p).UEid          = ue_id;
    NAS_UL_DATA_IND(message_p).nasMsg.data   = buffer;
    NAS_UL_DATA_IND(message_p).nasMsg.length = length;

    return itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}

int s1ap_mme_itti_nas_downlink_cnf(const uint32_t ue_id,
                                   nas_error_code_t error_code)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_S1AP, NAS_DOWNLINK_DATA_CNF);

    NAS_DL_DATA_CNF(message_p).UEid = ue_id;
    NAS_DL_DATA_CNF(message_p).errCode = error_code;

    return itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}
