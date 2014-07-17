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
#include <string.h>

#include "intertask_interface.h"

#include "sctp_itti_messaging.h"

int sctp_itti_send_new_association(uint32_t assoc_id, uint16_t instreams,
                                   uint16_t outstreams)
{
    MessageDef      *message_p;
    sctp_new_peer_t *sctp_new_peer_p;

    message_p = itti_alloc_new_message(TASK_SCTP, SCTP_NEW_ASSOCIATION);

    sctp_new_peer_p = &message_p->ittiMsg.sctp_new_peer;

    sctp_new_peer_p->assoc_id   = assoc_id;
    sctp_new_peer_p->instreams  = instreams;
    sctp_new_peer_p->outstreams = outstreams;

    return itti_send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

int sctp_itti_send_new_message_ind(int n, uint8_t *buffer, uint32_t assoc_id,
                                   uint16_t stream,
                                   uint16_t instreams, uint16_t outstreams)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_SCTP, SCTP_DATA_IND);

    SCTP_DATA_IND(message_p).buffer = malloc(sizeof(uint8_t) * n);

    memcpy((void *)SCTP_DATA_IND(message_p).buffer, (void *)buffer, n);

    SCTP_DATA_IND(message_p).stream     = stream;
    SCTP_DATA_IND(message_p).buf_length = n;
    SCTP_DATA_IND(message_p).assoc_id   = assoc_id;
    SCTP_DATA_IND(message_p).instreams  = instreams;
    SCTP_DATA_IND(message_p).outstreams = outstreams;

    return itti_send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

int sctp_itti_send_com_down_ind(uint32_t assoc_id)
{
    MessageDef *message_p;
    sctp_close_association_t *sctp_close_association_p;

    message_p = itti_alloc_new_message(TASK_SCTP, SCTP_CLOSE_ASSOCIATION);

    sctp_close_association_p = &message_p->ittiMsg.sctp_close_association;

    sctp_close_association_p->assoc_id = assoc_id;

    return itti_send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}
