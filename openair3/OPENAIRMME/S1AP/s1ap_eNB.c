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

/*! \file s1ap_eNB.c
 * \brief s1ap procedures for eNB
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <string.h>
#include <assert.h>

#include "sctp_primitives_client.h"
// #include "s1ap_common.h"
#include "s1ap_eNB.h"
#include "s1ap_ies_defs.h"

static struct s1ap_eNB_description_s *eNB_list_head = NULL;
static struct s1ap_eNB_description_s *eNB_list_tail = NULL;
static uint32_t nb_eNB = 0;

struct s1ap_eNB_description_s* s1ap_eNB_add_new(void) {

    struct s1ap_eNB_description_s* eNB_ref;
    if ((eNB_ref = malloc(sizeof(struct s1ap_eNB_description_s))) == NULL)
        return NULL;
    if (eNB_list_head == NULL) {
        eNB_list_head = eNB_list_tail = eNB_ref;
        eNB_ref->next_eNB = NULL;
        eNB_ref->previous_eNB = NULL;
    } else {
        eNB_list_tail->next_eNB = eNB_ref;
        eNB_ref->previous_eNB = eNB_list_tail;
        eNB_ref->next_eNB = NULL;
        eNB_list_tail = eNB_ref;
    }
    /* Initializing UE related data */
    eNB_ref->nb_ue = 0;
    eNB_ref->ue_list_head = NULL;
    eNB_ref->ue_list_tail = NULL;

    nb_eNB++;
    return eNB_ref;
}

struct s1ap_eNB_description_s* s1ap_get_eNB_assoc_id(uint32_t assocId) {
    struct s1ap_eNB_description_s* eNB_ref;

    eNB_ref = eNB_list_head;

    while(eNB_ref != NULL) {
        if (eNB_ref->assocId == assocId)
            return eNB_ref;
        eNB_ref = eNB_ref->next_eNB;
    }
    return NULL;
}

struct s1ap_eNB_description_s* s1ap_get_eNB_eNB_id(uint8_t eNB_id) {
    struct s1ap_eNB_description_s* eNB_ref;

    eNB_ref = eNB_list_head;

    while(eNB_ref != NULL) {
        if (eNB_ref->eNB_id == eNB_id)
            return eNB_ref;
        eNB_ref = eNB_ref->next_eNB;
    }
    return NULL;
}

struct s1ap_eNB_UE_description_s *s1ap_UE_add_new(struct s1ap_eNB_description_s *eNB_ref) {
    struct s1ap_eNB_UE_description_s *ue_ref;

    if (eNB_ref == NULL) return NULL;

    ue_ref = malloc(sizeof(struct s1ap_eNB_UE_description_s));
    if (ue_ref == NULL) return NULL;
    /* No UE in list */
    if (eNB_ref->ue_list_head == NULL) {
        eNB_ref->ue_list_head = eNB_ref->ue_list_tail = ue_ref;
        ue_ref->next_ue = NULL;
        ue_ref->previous_ue = NULL;
    }
    ue_ref->eNB = eNB_ref;
    eNB_ref->nb_ue ++;
    return ue_ref;
}

struct s1ap_eNB_UE_description_s *s1ap_get_ue_id_pair(uint8_t eNB_id, uint8_t ue_id) {
    struct s1ap_eNB_description_s* eNB_ref;
    struct s1ap_eNB_UE_description_s* ue_ref;

    if ((eNB_ref = s1ap_get_eNB_eNB_id(eNB_id)) == NULL) return NULL;
    for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref->next_ue) {
        if (ue_ref->eNB_UE_s1ap_id == ue_id)
            break;
    }
    return ue_ref;
}

struct s1ap_eNB_UE_description_s *s1ap_get_ue_assoc_id_eNB_ue_s1ap_id(
    uint32_t assoc_id,
    uint32_t eNB_ue_s1ap_id) {

    struct s1ap_eNB_description_s* eNB_ref;
    struct s1ap_eNB_UE_description_s* ue_ref;

    if ((eNB_ref = s1ap_get_eNB_assoc_id(assoc_id)) == NULL) return NULL;

    ue_ref = eNB_ref->ue_list_head;

    while(ue_ref != NULL) {
        if (ue_ref->eNB_UE_s1ap_id == eNB_ue_s1ap_id) return ue_ref;
        ue_ref = ue_ref->next_ue;
    }
    /* Not found -> return NULL */
    return NULL;
}

int s1ap_sctp_connected_callback(void *args, uint32_t assocId, uint32_t instreams, uint32_t outstreams) {

    struct s1ap_eNB_description_s* eNB_ref;

    if ((eNB_ref = s1ap_eNB_add_new()) == NULL)
        return;

    eNB_ref->eNB_id = *((uint8_t*)args);
    eNB_ref->assocId = assocId;
    eNB_ref->instreams = instreams;
    eNB_ref->outstreams = outstreams;
    eNB_ref->state = S1AP_ENB_STATE_DECONNECTED;
    /* First usable stream for UE associated signalling */
    eNB_ref->nextstream = 1;
    if (s1ap_eNB_generate_s1_setup_request(eNB_ref) < 0) {
        S1AP_ERROR("[eNB %02d] Failed to encode S1 Setup Request\n", eNB_ref->eNB_id);
        return -1;
    }
    /* Waiting for the response from MME */
    while ((volatile)(eNB_ref->state) & S1AP_ENB_STATE_WAITING) {
    }
    return 0;
}

/* Function called every time we received something on SCTP */
int s1ap_sctp_recv_callback(uint32_t assocId, uint32_t stream, uint8_t *buffer, uint32_t length) {
    s1ap_message message;

    if (s1ap_eNB_decode_pdu(&message, buffer, length) < 0)
        return -1;
    return s1ap_eNB_handle_message(assocId, stream, &message);
}

int s1ap_eNB_new_data_request(uint8_t eNB_id, uint8_t ue_id, uint8_t *buffer, uint32_t length) {
    s1ap_message message;
    struct s1ap_eNB_UE_description_s *ue_ref;
    struct s1ap_eNB_description_s *eNB_ref;

    /* If we don't found the eNB as S1 associated, this may indicate that no S1 Setup Request has been sent,
     * or S1 Setup has failed.
     */
    if ((eNB_ref = s1ap_get_eNB_eNB_id(eNB_id)) == NULL) {
        S1AP_ERROR("This eNB (%d) has no known S1 association with any MME\n", eNB_id);
        return -1;
    }

    ///TODO: Check if eNB is associated to a MME

    /* Searching for UE in eNB.
     * If we failed founding it, request a new initial UE message to MME.
     */
    if ((ue_ref = s1ap_get_ue_id_pair(eNB_id, ue_id)) == NULL) {
        if ((ue_ref = s1ap_UE_add_new(eNB_ref)) == NULL) {
            /* We failed to allocate a new UE in list */
            S1AP_ERROR("Failed to allocate new UE description for UE %d attached to eNB %d\n",
                  ue_id, eNB_id);
            return -1;
        }
        ue_ref->eNB_UE_s1ap_id = ue_id;
        /* MME UE S1AP ID is allocated by MME, waiting for Initial Context Setup Request Message
         * before setting it.
         */
        ue_ref->mme_UE_s1ap_id = 0;
        ue_ref->eNB->nextstream++;
        /* Do we reached max number of output streams ?.
         * If so wrap to 1.
         */
        if (ue_ref->eNB->nextstream == ue_ref->eNB->outstreams)
            ue_ref->eNB->nextstream = 1;

        /* Set the output stream that will be used for this UE */
        ue_ref->stream_send = ue_ref->eNB->nextstream;

        S1AP_ERROR("Sending Initial UE message for UE %d on eNB %d\n",
              ue_id, eNB_id);

        return s1ap_eNB_generate_initial_ue_message(ue_ref, buffer, length);
    } else {
        /* UE is already associated to a valid Context.
         * Consider request as Uplink NAS transport.
         */
        S1AP_ERROR("Sending Uplink NAS transport message for UE %d on eNB %d\n",
              ue_id, eNB_id);
        return s1ap_eNB_generate_uplink_nas_transport(ue_ref, buffer, length);
    }
}

int s1ap_eNB_init(const char *mme_ip_address, const uint8_t eNB_id) {
    uint8_t args = eNB_id;
    if (sctp_connect_to_remote_host(
        mme_ip_address, 36412, &args,
        s1ap_sctp_connected_callback,
        s1ap_sctp_recv_callback) < 0)
    {
        S1AP_ERROR("[eNB %02d] Failed to setup SCTP\n", eNB_id);
        return -1;
    }

    S1AP_DEBUG("[eNB %02d] successfully connected to MME\n", eNB_id);
    return 0;
}
