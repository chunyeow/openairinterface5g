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

#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "intertask_interface.h"

#ifndef NAS_ITTI_MESSAGING_H_
#define NAS_ITTI_MESSAGING_H_

# if defined(EPC_BUILD) && defined(NAS_MME)
#include "conversions.h"

int nas_itti_dl_data_req(const uint32_t ue_id, void *const data,
                         const uint32_t length);

static inline void nas_itti_establish_cnf(const uint32_t ue_id,
        const nas_error_code_t error_code, void *const data,
        const uint32_t length)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_NAS, NAS_CONNECTION_ESTABLISHMENT_CNF);

    NAS_CONNECTION_ESTABLISHMENT_CNF(message_p).UEid            = ue_id;
    NAS_CONNECTION_ESTABLISHMENT_CNF(message_p).errCode         = error_code;
    NAS_CONNECTION_ESTABLISHMENT_CNF(message_p).nasMsg.data     = data;
    NAS_CONNECTION_ESTABLISHMENT_CNF(message_p).nasMsg.length   = length;

    itti_send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, message_p);
}

static inline void nas_itti_auth_info_req(const uint32_t ue_id,
        const imsi_t *const imsi, uint8_t initial_req, const uint8_t *auts)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_NAS, NAS_AUTHENTICATION_PARAM_REQ);

    hexa_to_ascii((uint8_t *)imsi->u.value,
                  NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi, 8);

    NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[15] = '\0';

    if (isdigit(NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[14])) {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi_length = 15;
    } else {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi_length = 14;
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[14] = '\0';
    }
    NAS_AUTHENTICATION_PARAM_REQ(message_p).initial_req = initial_req;
    NAS_AUTHENTICATION_PARAM_REQ(message_p).ue_id = ue_id;

    /* Re-synchronisation */
    if (auts != NULL) {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).re_synchronization = 1;
        memcpy(NAS_AUTHENTICATION_PARAM_REQ(message_p).auts, auts, AUTS_LENGTH);
    } else {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).re_synchronization = 0;
        memset(NAS_AUTHENTICATION_PARAM_REQ(message_p).auts, 0, AUTS_LENGTH);
    }

    itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}

static inline void nas_itti_establish_rej(const uint32_t ue_id,
        const imsi_t *const imsi, uint8_t initial_req)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_NAS, NAS_AUTHENTICATION_PARAM_REQ);

    hexa_to_ascii((uint8_t *)imsi->u.value,
                  NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi, 8);

    NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[15] = '\0';

    if (isdigit(NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[14])) {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi_length = 15;
    } else {
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi_length = 14;
        NAS_AUTHENTICATION_PARAM_REQ(message_p).imsi[14] = '\0';
    }
    NAS_AUTHENTICATION_PARAM_REQ(message_p).initial_req = initial_req;
    NAS_AUTHENTICATION_PARAM_REQ(message_p).ue_id = ue_id;

    itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}
# endif

# if defined(UE_BUILD) && defined(NAS_UE)
int nas_itti_cell_info_req(const plmn_t plmnID, const Byte_t rat);

int nas_itti_nas_establish_req(as_cause_t cause, as_call_type_t type, as_stmsi_t s_tmsi, plmn_t plmnID, Byte_t *data, UInt32_t length);

int nas_itti_ul_data_req(const uint32_t ue_id, void *const data, const uint32_t length);

int nas_itti_rab_establish_rsp(const as_stmsi_t s_tmsi, const as_rab_id_t rabID, const nas_error_code_t errCode);
# endif
#endif /* NAS_ITTI_MESSAGING_H_ */
