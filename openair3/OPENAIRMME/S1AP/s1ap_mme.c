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
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#if !defined(MME_CLIENT_TEST)
# include "intertask_interface.h"
#endif
#include "s1ap_mme.h"
#include "s1ap_mme_decoder.h"
#include "s1ap_mme_handlers.h"
#include "s1ap_ies_defs.h"

#define S1AP_DEBUG_LIST
#ifdef S1AP_DEBUG_LIST
# define eNB_LIST_OUT(x, args...) S1AP_DEBUG("[eNB]%*s"x"\n", 4*indent, "", ##args)
# define UE_LIST_OUT(x, args...)  S1AP_DEBUG("[UE] %*s"x"\n", 4*indent, "", ##args)
#else
# define eNB_LIST_OUT(x, args...)
# define UE_LIST_OUT(x, args...)
#endif

#if !defined(MME_CLIENT_TEST)
static pthread_t s1ap_task_thread;

uint8_t nb_eNB_associated;
eNB_description_t *eNB_list_head;
eNB_description_t *eNB_list_tail;

MessageDef *receivedMessage;
static int indent = 0;

void* s1ap_mme_thread(void *args);

static int s1ap_send_init_sctp(void) {
    // Create and alloc new message
    MessageDef *message_p;
    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_S1AP_INIT;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    message_p->msg.sctpS1APInit.port = S1AP_PORT;
    message_p->msg.sctpS1APInit.ppid = S1AP_SCTP_PPID;
    message_p->msg.sctpS1APInit.address = "0.0.0.0"; //ANY address

    return itti_send_msg_to_task(TASK_SCTP, message_p);
}

void* s1ap_mme_thread(void *args) {
    while(1) {
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        itti_receive_msg(TASK_S1AP, &receivedMessage);
        assert(receivedMessage != NULL);
        switch(receivedMessage->messageId) {
            case S1AP_SCTP_NEW_MESSAGE_IND:
            {
                /* New message received from SCTP layer.
                 * Decode and handle it.
                 */
                s1ap_message message;
                S1apSctpNewMessageInd *s1apSctpNewMessageInd;
                s1apSctpNewMessageInd = &receivedMessage->msg.s1apSctpNewMessageInd;

                memset((void*)&message, 0, sizeof(s1ap_message));
                /* Invoke S1AP message decoder */
                if (s1ap_mme_decode_pdu(&message, s1apSctpNewMessageInd->buffer, s1apSctpNewMessageInd->bufLen) < 0) {
                    // TODO: Notify eNB of failure with right cause
                } else {
                    s1ap_mme_handle_message(s1apSctpNewMessageInd->assocId, s1apSctpNewMessageInd->stream, &message);
                }
                // Free received PDU array
                free(s1apSctpNewMessageInd->buffer);
            } break;
            /* SCTP layer notifies S1AP of disconnection of a peer. */
            case S1AP_SCTP_ASSOCIATION_CLOSED:
            {
                S1apSctpAsscociationClosed *s1apSctpAsscociationClosed;
                s1apSctpAsscociationClosed = &receivedMessage->msg.s1apSctpAssociationClosed;
                s1ap_handle_sctp_deconnection(s1apSctpAsscociationClosed->assocId);
            } break;
            case S1AP_NAS_NEW_MESSAGE_IND:
            {
                /* New message received from NAS task.
                 * This corresponds to a S1AP downlink nas transport message.
                 */
                s1ap_generate_downlink_nas_transport(&receivedMessage->msg.s1apNASNewMessageInd);
            } break;
            case SGW_CREATE_SESSION_RESPONSE:
            {
                s1ap_handle_create_session_response(&receivedMessage->msg.sgwCreateSessionResponse);
            } break;
            case SGW_MODIFY_BEARER_RESPONSE:
            {
                
            } break;
            default:
            {
                S1AP_DEBUG("Unkwnon message ID %d\n", receivedMessage->messageId);
            } break;
        }
        free(receivedMessage);
        receivedMessage = NULL;
    }
    return NULL;
}

int s1ap_mme_init(const mme_config_t *mme_config) {
    S1AP_DEBUG("Initializing S1AP interface\n");

    if (get_asn1c_environment_version() < 923)
        S1AP_ERROR("ASN1C version %d fount, expecting at least %d\n", get_asn1c_environment_version(), 923);
    else
        S1AP_DEBUG("ASN1C version %d\n", get_asn1c_environment_version());

#if defined(UPDATE_RELEASE_9)
    S1AP_DEBUG("S1AP Release v9.8\n");
#else
    S1AP_DEBUG("S1AP Release v8.10\n");
#endif

    eNB_list_head = NULL;
    eNB_list_tail = NULL;
    nb_eNB_associated = 0;

    if (pthread_create(&s1ap_task_thread, NULL, &s1ap_mme_thread, NULL) < 0) {
        perror("s1ap phtread_create");
        return -1;
    }
    if (s1ap_send_init_sctp() < 0) {
        return -1;
    }
    S1AP_DEBUG("Initializing S1AP interface: DONE\n");
    return 0;
}

void s1ap_dump_eNB_list(void) {
    eNB_description_t *eNB_ref = eNB_list_head;

    while (eNB_ref != NULL) {
        s1ap_dump_eNB(eNB_ref);
        eNB_ref = eNB_ref->next_eNB;
    }
}

void s1ap_dump_eNB(eNB_description_t *eNB_ref) {
#ifdef S1AP_DEBUG_LIST
    ue_description_t *ue_ref;
    //Reset indentation
    indent = 0;
    if (eNB_ref == NULL) return;
    eNB_LIST_OUT("");
    eNB_LIST_OUT("eNB name:          %s", eNB_ref->eNB_name == NULL ? "not present" : eNB_ref->eNB_name);
    eNB_LIST_OUT("eNB ID:            %d", eNB_ref->eNB_id);
    eNB_LIST_OUT("SCTP assoc id:     %d", eNB_ref->sctp_assoc_id);
    eNB_LIST_OUT("SCTP instreams:    %d", eNB_ref->instreams);
    eNB_LIST_OUT("SCTP outstreams:   %d", eNB_ref->outstreams);
    eNB_LIST_OUT("UE attache to eNB: %d", eNB_ref->nb_ue_associated);

    indent++;
    for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref = ue_ref->next_ue) {
        s1ap_dump_ue(ue_ref);
    }
    indent--;
    eNB_LIST_OUT("");
#else
    eNB_ref = eNB_ref;
    s1ap_dump_ue(NULL);
#endif
}

void s1ap_dump_ue(ue_description_t *ue_ref) {
#ifdef S1AP_DEBUG_LIST
    if (ue_ref == NULL) return;
    UE_LIST_OUT("eNB UE s1ap id:   %d", ue_ref->eNB_ue_s1ap_id);
    UE_LIST_OUT("MME UE s1ap id:   %d", ue_ref->mme_ue_s1ap_id);
    UE_LIST_OUT("SCTP stream recv: %d", ue_ref->sctp_stream_recv);
    UE_LIST_OUT("SCTP stream send: %d", ue_ref->sctp_stream_send);
#else
    ue_ref = ue_ref;
#endif
}

eNB_description_t* s1ap_is_eNB_id_in_list(uint32_t eNB_id) {
    eNB_description_t *eNB_ref;

    for (eNB_ref = eNB_list_head; eNB_ref; eNB_ref = eNB_ref->next_eNB) {
        if (eNB_ref->eNB_id == eNB_id)
            // We fount a matching reference, return it
            break;
    }
    // No matching eNB, return NULL
    return eNB_ref;
}

eNB_description_t* s1ap_is_eNB_assoc_id_in_list(uint32_t sctp_assoc_id) {
    eNB_description_t *eNB_ref;

    for (eNB_ref = eNB_list_head; eNB_ref; eNB_ref = eNB_ref->next_eNB) {
        if (eNB_ref->sctp_assoc_id == sctp_assoc_id)
            // We fount a matching reference, return it
            break;
    }
    // No matching eNB or no eNB in list, return NULL
    return eNB_ref;
}

ue_description_t* s1ap_is_ue_eNB_id_in_list(eNB_description_t *eNB_ref,
                                            uint32_t eNB_ue_s1ap_id) {
    ue_description_t *ue_ref;
    // No eNB_list_head in list, simply returning NULL
    if (eNB_ref == NULL) return NULL;

    for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref = ue_ref->next_ue) {
        if (ue_ref->eNB_ue_s1ap_id == eNB_ue_s1ap_id)
            break;
    }
    // No matching UE, return NULL
    return ue_ref;
}

ue_description_t* s1ap_is_ue_mme_id_in_list(uint32_t mme_ue_s1ap_id) {
    ue_description_t *ue_ref;
    eNB_description_t *eNB_ref;
    // No eNB_list_head in list, simply returning NULL
    if (eNB_list_head == NULL) return NULL;
    for (eNB_ref = eNB_list_head; eNB_ref; eNB_ref = eNB_ref->next_eNB) {
        for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref = ue_ref->next_ue) {
            // We fount a matching reference, return it
            if (ue_ref->mme_ue_s1ap_id == mme_ue_s1ap_id)
                return ue_ref;
        }
    }
    // No matching UE, return NULL
    return NULL;
}

ue_description_t* s1ap_is_teid_in_list(uint32_t teid) {
    ue_description_t *ue_ref;
    eNB_description_t *eNB_ref;
    // No eNB_list_head in list, simply returning NULL
    if (eNB_list_head == NULL) return NULL;
    for (eNB_ref = eNB_list_head; eNB_ref; eNB_ref = eNB_ref->next_eNB) {
        for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref = ue_ref->next_ue) {
            // We fount a matching reference, return it
            if (ue_ref->teid == teid)
                return ue_ref;
        }
    }
    // No matching UE, return NULL
    return NULL;
}

eNB_description_t* s1ap_new_eNB(void) {
    eNB_description_t *eNB_ref = NULL;

    eNB_ref = malloc(sizeof(eNB_description_t));

    /* Something bad happened during malloc...
     * May be we are running out of memory.
     * TODO: Notify eNB with a cause like Hardware Failure.
     */
    if (eNB_ref == NULL) return NULL;

    eNB_ref->next_eNB = NULL;
    eNB_ref->previous_eNB = NULL;
    // No eNB present
    if (eNB_list_head == NULL) {
        eNB_list_head = eNB_ref;
        // Point tail to head (single element in list)
        eNB_list_tail = eNB_list_head;
    } else {
        eNB_ref->previous_eNB = eNB_list_tail;
        eNB_list_tail->next_eNB = eNB_ref;
        // Update list tail with the new eNB added
        eNB_list_tail = eNB_ref;
    }

    // Update number of eNB associated
    nb_eNB_associated++;
    eNB_ref->ue_list_head = NULL;
    eNB_ref->ue_list_tail = NULL;
    eNB_ref->nb_ue_associated = 0;
    return eNB_ref;
}

ue_description_t* s1ap_new_ue(uint32_t sctp_assoc_id) {
    eNB_description_t *eNB_ref = NULL;
    ue_description_t  *ue_ref  = NULL;

    if ((eNB_ref = s1ap_is_eNB_assoc_id_in_list(sctp_assoc_id)) == NULL) {
        /* No eNB attached to this SCTP assoc ID...
         * return NULL.
         */
        return NULL;
    }
    ue_ref = malloc(sizeof(ue_description_t));
    /* Something bad happened during malloc...
     * May be we are running out of memory.
     * TODO: Notify eNB with a cause like Hardware Failure.
     */
    if (ue_ref == NULL) return NULL;

    ue_ref->eNB = eNB_ref;
    ue_ref->next_ue = NULL;

    // Increment number of UE
    eNB_ref->nb_ue_associated++;
    if (eNB_ref->ue_list_head == NULL) {
        // Currently no UE in active list
        eNB_ref->ue_list_head = ue_ref;
        ue_ref->previous_ue = NULL;
        eNB_ref->ue_list_tail = eNB_ref->ue_list_head;
    } else {
        eNB_ref->ue_list_tail->next_ue = ue_ref;
        ue_ref->previous_ue = eNB_ref->ue_list_tail;
        eNB_ref->ue_list_tail = ue_ref;
    }

    return ue_ref;
}

void s1ap_remove_ue(ue_description_t *ue_ref) {
    /* NULL reference... */
    if (ue_ref == NULL) return;

    if (ue_ref->next_ue != NULL) {
        if (ue_ref->previous_ue != NULL) {
            /* Not head and not tail */
            ue_ref->previous_ue->next_ue = ue_ref->next_ue;
            ue_ref->next_ue->previous_ue = ue_ref->previous_ue;
        } else {
            /* Head but not tail */
            ue_ref->eNB->ue_list_head = ue_ref->next_ue;
            ue_ref->next_ue->previous_ue = NULL;
        }
    } else {
        if (ue_ref->previous_ue != NULL) {
            /* Not head but tail */
            ue_ref->eNB->ue_list_tail = ue_ref->previous_ue;
            ue_ref->previous_ue->next_ue = NULL;
        } else {
            /* Head and tail */
            ue_ref->eNB->ue_list_tail = ue_ref->eNB->ue_list_head = NULL;
        }
    }
    /* Updating number of UE */
    ue_ref->eNB->nb_ue_associated--;
    /* Freeing memory */
    free(ue_ref);
}

void s1ap_remove_eNB(eNB_description_t *eNB_ref) {
    ue_description_t *ue_ref;

    if (eNB_ref == NULL) return;

    /* Removing any ue context */
    for (ue_ref = eNB_ref->ue_list_head; ue_ref; ue_ref = ue_ref->next_ue) {
//     while (ue_ref != NULL) {
        s1ap_remove_ue(ue_ref);
//         ue_ref = ue_ref->next_ue;
    }

    if (eNB_ref->next_eNB != NULL) {
        if (eNB_ref->previous_eNB != NULL) {
            /* Not tail and not head */
            eNB_ref->previous_eNB->next_eNB = eNB_ref->next_eNB;
            eNB_ref->next_eNB->previous_eNB = eNB_ref->previous_eNB;
        } else {
            /* Head but not tail */
            eNB_list_head = eNB_ref->next_eNB;
            eNB_ref->next_eNB->previous_eNB = NULL;
        }
    } else {
        if (eNB_ref->previous_eNB != NULL) {
            /* Not head but tail */
            eNB_list_tail = eNB_ref->previous_eNB;
            eNB_ref->previous_eNB->next_eNB = NULL;
        } else {
            /* Head and tail */
            eNB_list_tail = eNB_list_head = NULL;
        }
    }
    free(eNB_ref);
    nb_eNB_associated--;
}

#endif
