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

#include <string.h>

#include "enb_app.h"
#include "assertions.h"

#include "log.h"
#if defined(OAI_EMU)
# include "OCG.h"
# include "OCG_extern.h"
#endif

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
# include "timer.h"
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
#   include "sctp_eNB_task.h"
# endif
#endif

#if defined(ENABLE_ITTI)
# if defined(ENABLE_USE_MME)

/*------------------------------------------------------------------------------*/
#define ENB_REGISTER_RETRY_DELAY 10

/*------------------------------------------------------------------------------*/
static uint32_t enb_nb = 1;

/*------------------------------------------------------------------------------*/
static uint32_t eNB_app_register()
{
    uint32_t eNB_id_start = 0;
    uint32_t eNB_id_end = 1;
    uint32_t eNB_id;
    MessageDef *msg_p;
    uint32_t register_enb_pending = 0;

#   if defined(OAI_EMU)
    eNB_id_start = oai_emulation.info.first_enb_local;
    eNB_id_end = oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local;
#   endif

    for (eNB_id = eNB_id_start; (eNB_id < eNB_id_end)
#   if defined(OAI_EMU)
            && (oai_emulation.info.cli_start_enb[eNB_id] == 1)
#   endif
            ; eNB_id++)
    {
        char *mme_address_v4;

        if (EPC_MODE_ENABLED)
        {
            mme_address_v4 = EPC_MODE_MME_ADDRESS;
        }
        else
        {
            mme_address_v4 = "192.168.12.87";
        }
        char *mme_address_v6 = "2001:660:5502:12:30da:829a:2343:b6cf";
        s1ap_register_enb_req_t *s1ap_register_eNB;
        uint32_t hash;

        //note:  there is an implicit relationship between the data struct and the message name
        msg_p = itti_alloc_new_message (TASK_ENB_APP, S1AP_REGISTER_ENB_REQ);

        s1ap_register_eNB = &S1AP_REGISTER_ENB_REQ(msg_p);

        hash = s1ap_generate_eNB_id ();

        /* Some default/random parameters */
        s1ap_register_eNB->eNB_id = eNB_id + (hash & 0xFFFF8);
        s1ap_register_eNB->cell_type = CELL_MACRO_ENB;
        s1ap_register_eNB->tac = 0;
        s1ap_register_eNB->mcc = 208;
        s1ap_register_eNB->mnc = 34;
        s1ap_register_eNB->default_drx = PAGING_DRX_256;

        s1ap_register_eNB->nb_mme = 1;
        s1ap_register_eNB->mme_ip_address[0].ipv4 = 1;
        s1ap_register_eNB->mme_ip_address[0].ipv6 = 0;
        strncpy (s1ap_register_eNB->mme_ip_address[0].ipv4_address, mme_address_v4, sizeof(s1ap_register_eNB->mme_ip_address[0].ipv4_address));
        strncpy (s1ap_register_eNB->mme_ip_address[0].ipv6_address, mme_address_v6, sizeof(s1ap_register_eNB->mme_ip_address[0].ipv6_address));

#   if defined ENB_APP_ENB_REGISTER_2_MME
        s1ap_register_eNB->nb_mme = 2;
        s1ap_register_eNB->mme_ip_address[1].ipv4 = 1;
        s1ap_register_eNB->mme_ip_address[1].ipv6 = 0;
        mme_address_v4 = "192.168.12.88";
        strncpy(s1ap_register_eNB->mme_ip_address[1].ipv4_address, mme_address_v4, sizeof(s1ap_register_eNB->mme_ip_address[1].ipv4_address));
        strncpy(s1ap_register_eNB->mme_ip_address[1].ipv6_address, mme_address_v6, sizeof(s1ap_register_eNB->mme_ip_address[1].ipv6_address));
#   endif

        itti_send_msg_to_task (TASK_S1AP, eNB_id, msg_p);

        register_enb_pending++;
    }

    return register_enb_pending;
}
# endif
#endif

/*------------------------------------------------------------------------------*/
void *eNB_app_task(void *args_p)
{
#if defined(ENABLE_ITTI)
# if defined(ENABLE_USE_MME)
    static uint32_t register_enb_pending;
    static uint32_t registered_enb;
    static long enb_register_retry_timer_id;
# endif
    MessageDef *msg_p;
    const char *msg_name;
    instance_t instance;
    itti_mark_task_ready (TASK_ENB_APP);

# if defined(ENABLE_USE_MME)
#   if defined(OAI_EMU)
    enb_nb = oai_emulation.info.nb_enb_local;
#   endif
# endif

# if defined(ENABLE_USE_MME)
    /* Try to register each eNB */
    registered_enb = 0;
    register_enb_pending = eNB_app_register ();
# else
    /* Start L2L1 task */
    msg_p = itti_alloc_new_message(TASK_ENB_APP, INITIALIZE_MESSAGE);
    itti_send_msg_to_task(TASK_L2L1, INSTANCE_DEFAULT, msg_p);
# endif

    do
    {
        // Wait for a message
        itti_receive_msg (TASK_ENB_APP, &msg_p);

        msg_name = ITTI_MSG_NAME (msg_p);
        instance = ITTI_MSG_INSTANCE (msg_p);

        switch (ITTI_MSG_ID(msg_p))
        {
            case TERMINATE_MESSAGE:
                itti_exit_task ();
                break;

            case MESSAGE_TEST:
                LOG_I(ENB_APP, "Received %s\n", ITTI_MSG_NAME(msg_p));
                break;

# if defined(ENABLE_USE_MME)
            case S1AP_REGISTER_ENB_CNF:
                LOG_I(ENB_APP, "[eNB %d] Received %s: associated MME %d\n", instance, msg_name,
                      S1AP_REGISTER_ENB_CNF(msg_p).nb_mme);

                DevAssert(register_enb_pending > 0);
                register_enb_pending--;

                /* Check if at least eNB is registered with one MME */
                if (S1AP_REGISTER_ENB_CNF(msg_p).nb_mme > 0)
                {
                    registered_enb++;
                }

                /* Check if all register eNB requests have been processed */
                if (register_enb_pending == 0)
                {
                    if (registered_enb == enb_nb)
                    {
                        /* If all eNB are registered, start L2L1 task */
                        MessageDef *msg_init_p;

                        msg_init_p = itti_alloc_new_message (TASK_ENB_APP, INITIALIZE_MESSAGE);
                        itti_send_msg_to_task (TASK_L2L1, INSTANCE_DEFAULT, msg_init_p);
                    }
                    else
                    {
                        uint32_t not_associated = enb_nb - registered_enb;

                        LOG_W(ENB_APP, " %d eNB %s not associated with a MME, retrying registration in %d seconds ...\n",
                              not_associated, not_associated > 1 ? "are" : "is", ENB_REGISTER_RETRY_DELAY);

                        /* Restart the eNB registration process in ENB_REGISTER_RETRY_DELAY seconds */
                        if (timer_setup (ENB_REGISTER_RETRY_DELAY, 0, TASK_ENB_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT,
                                         NULL, &enb_register_retry_timer_id) < 0)
                        {
                            LOG_E(ENB_APP, " Can not start eNB register retry timer, use \"usleep\" instead!\n");

                            usleep(ENB_REGISTER_RETRY_DELAY * 1000000);
                            /* Restart the registration process */
                            registered_enb = 0;
                            register_enb_pending = eNB_app_register ();
                        }
                    }
                }
                break;

            case TIMER_HAS_EXPIRED:
                LOG_I(ENB_APP, " Received %s: timer_id %d\n", msg_name, TIMER_HAS_EXPIRED(msg_p).timer_id);

                if (TIMER_HAS_EXPIRED (msg_p).timer_id == enb_register_retry_timer_id)
                {
                    /* Restart the registration process */
                    registered_enb = 0;
                    register_enb_pending = eNB_app_register ();
                }
                break;
# endif

            default:
                LOG_E(ENB_APP, "Received unexpected message %s\n", msg_name);
                break;
        }

        free (msg_p);
    } while (1);
#endif

    return NULL;
}
