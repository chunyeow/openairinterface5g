#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "intertask_interface.h"

#include "nas_defs.h"

#if !defined(DISABLE_USE_NAS)
# include "nas_proc.h"
# include "emm_main.h"
# include "nas_log.h"
#endif

#define NAS_ERROR(x, args...) do { fprintf(stderr, "[NAS] [E]"x, ##args); } while(0)
#define NAS_DEBUG(x, args...) do { fprintf(stdout, "[NAS] [D]"x, ##args); } while(0)
#define NAS_WARN(x, args...)  do { fprintf(stdout, "[NAS] [W]"x, ##args); } while(0)

static void *nas_intertask_interface(void *args_p)
{
    intertask_interface_mark_task_ready(TASK_NAS);

    while(1) {
        MessageDef *received_message_p;

next_message:
        receive_msg(TASK_NAS, &received_message_p);
        switch(received_message_p->header.messageId) {
            case NAS_CONNECTION_ESTABLISHMENT_IND: {
#if defined(DISABLE_USE_NAS)
                MessageDef       *message_p;
                nas_attach_req_t *nas_req_p;
                s1ap_initial_ue_message_t *transparent;

                NAS_DEBUG("NAS abstraction: Generating NAS ATTACH REQ\n");

                message_p = alloc_new_message(TASK_NAS, NAS_ATTACH_REQ);

                nas_req_p = &message_p->msg.nas_attach_req;
                transparent = &message_p->msg.nas_attach_req.transparent;

                nas_req_p->initial = INITIAL_REQUEST;
                sprintf(nas_req_p->imsi, "%14llu", 20834123456789ULL);

                memcpy(transparent, &received_message_p->msg.nas_conn_est_ind.transparent,
                       sizeof(s1ap_initial_ue_message_t));

                send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
#else
                nas_establish_ind_t *nas_est_ind_p;

                nas_est_ind_p = &received_message_p->msg.nas_conn_est_ind.nas;

                nas_proc_establish_ind(nas_est_ind_p->UEid,
                                       nas_est_ind_p->tac,
                                       nas_est_ind_p->initialNasMsg.data,
                                       nas_est_ind_p->initialNasMsg.length);
#endif
            } break;
            case NAS_ATTACH_ACCEPT: {
                send_msg_to_task(TASK_S1AP, INSTANCE_DEFAULT, received_message_p);
                goto next_message;
            } break;
            case NAS_AUTHENTICATION_REQ: {
                MessageDef      *message_p;
                nas_auth_resp_t *nas_resp_p;

                NAS_DEBUG("NAS abstraction: Generating NAS AUTHENTICATION RESPONSE\n");

                message_p = alloc_new_message(TASK_NAS, NAS_AUTHENTICATION_RESP);

                nas_resp_p = &message_p->msg.nas_auth_resp;

                memcpy(nas_resp_p->imsi, received_message_p->msg.nas_auth_req.imsi, 16);

                send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
            } break;
            default: {
                NAS_DEBUG("Unkwnon message ID %s:%d\n",
                          get_message_name(received_message_p->header.messageId),
                          received_message_p->header.messageId);
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int nas_init(const mme_config_t *mme_config_p)
{
    NAS_DEBUG("Initializing NAS task interface\n");
#if !defined(DISABLE_USE_NAS)
    nas_log_init(LOG_DEBUG);
    emm_main_initialize();
#endif

    if (intertask_interface_create_task(TASK_NAS, &nas_intertask_interface,
                                        NULL) < 0) {
        NAS_ERROR("Create task failed");
        NAS_DEBUG("Initializing NAS task interface: FAILED\n");
        return -1;
    }
    NAS_DEBUG("Initializing NAS task interface: DONE\n");
    return 0;
}
