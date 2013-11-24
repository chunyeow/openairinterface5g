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

#include "s1ap_mme_itti_messaging.h"

int s1ap_mme_itti_send_sctp_request(uint8_t *buffer, uint32_t length,
                                    uint32_t assoc_id, uint16_t stream)
{
    MessageDef     *message_p;
    SctpNewDataReq *sctpNewDataReq_p;

    message_p = itti_alloc_new_message(TASK_SCTP, SCTP_NEW_DATA_REQ);

    sctpNewDataReq_p = &message_p->ittiMsg.sctpNewDataReq;

    sctpNewDataReq_p->buffer  = buffer;
    sctpNewDataReq_p->bufLen  = length;
    sctpNewDataReq_p->assocId = assoc_id;
    sctpNewDataReq_p->stream  = stream;

    return itti_send_msg_to_task(TASK_SCTP, INSTANCE_DEFAULT, message_p);
}

int s1ap_mme_itti_forward_nas_uplink(uint8_t *buffer, uint32_t length)
{
    return 0;
}
