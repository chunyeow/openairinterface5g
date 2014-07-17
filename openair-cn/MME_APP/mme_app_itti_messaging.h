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


#ifndef MME_APP_ITTI_MESSAGING_H_
#define MME_APP_ITTI_MESSAGING_H_

static inline void
mme_app_itti_auth_fail(
        const uint32_t ue_id,
        const nas_cause_t cause)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_MME_APP, NAS_AUTHENTICATION_PARAM_FAIL);

    NAS_AUTHENTICATION_PARAM_FAIL(message_p).ue_id = ue_id;
    NAS_AUTHENTICATION_PARAM_FAIL(message_p).cause = cause;

    itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}



static inline void mme_app_itti_auth_rsp(
        const uint32_t                 ue_id,
        const uint8_t                  nb_vectors,
        const  eutran_vector_t * const vector)
{
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_MME_APP, NAS_AUTHENTICATION_PARAM_RSP);

    NAS_AUTHENTICATION_PARAM_RSP(message_p).ue_id       = ue_id;
    NAS_AUTHENTICATION_PARAM_RSP(message_p).nb_vectors  = nb_vectors;
    memcpy(&NAS_AUTHENTICATION_PARAM_RSP(message_p).vector, vector, sizeof(*vector));

    itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
}

#endif /* MME_APP_ITTI_MESSAGING_H_ */
