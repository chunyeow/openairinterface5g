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

#include "intertask_interface.h"
#include "conversions.h"

#ifndef NAS_ITTI_MESSAGING_H_
#define NAS_ITTI_MESSAGING_H_

int nas_itti_dl_data_req(const uint32_t ue_id, void * const data,
                         const uint32_t length);

void nas_itti_establish_cnf(const nas_error_code_t error_code, void * const data,
                            const uint32_t length);

static inline void nas_itti_auth_info_req(const imsi_t * const imsi,
                                          uint8_t initial_req)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_NAS, NAS_AUTHENTICATION_PARAM_REQ);

    hexa_to_ascii((uint8_t *)imsi->u.value, NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi,
                  imsi->length);
    NAS_AUTHENTICATION_PARAM_REQ(message_p).initial_req = initial_req;
    NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi_length = imsi->length - imsi->u.num.parity;

    itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}

#endif /* NAS_ITTI_MESSAGING_H_ */
