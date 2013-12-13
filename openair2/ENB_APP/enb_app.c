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

extern unsigned char NB_eNB_INST;
#endif

#if defined(ENABLE_ITTI)

/*------------------------------------------------------------------------------*/
# if defined(ENABLE_USE_MME)
#   define ENB_REGISTER_RETRY_DELAY 10
# endif

typedef struct mme_ip_address_s {
    unsigned ipv4:1;
    unsigned ipv6:1;
    char *ipv4_address;
    char *ipv6_address;
} mme_ip_address_t;

typedef struct Enb_properties_s {
    /* Unique eNB_id to identify the eNB within EPC.
     * For macro eNB ids this field should be 20 bits long.
     * For home eNB ids this field should be 28 bits long.
     */
    uint32_t eNB_id;

    /* The type of the cell */
    enum cell_type_e cell_type;

    /* Optional name for the cell
     * NOTE: the name can be NULL (i.e no name) and will be cropped to 150
     * characters.
     */
    char *eNB_name;

    /* Tracking area code */
    uint16_t tac;

    /* Mobile Country Code
     * Mobile Network Code
     */
    uint16_t mcc;
    uint16_t mnc;

    /* Default Paging DRX of the eNB as defined in TS 36.304 */
    paging_drx_t default_drx;

    /* Nb of MME to connect to */
    uint8_t          nb_mme;
    /* List of MME to connect to */
    mme_ip_address_t mme_ip_address[S1AP_MAX_NB_MME_IP_ADDRESS];
} Enb_properties_t;

/*------------------------------------------------------------------------------*/
# if defined(ENABLE_USE_MME)
static uint32_t enb_nb = 1; /* Default number of eNB */
# endif

/* eNB 0 properties */
static Enb_properties_t enb_0_properties =
{
    347472,
    CELL_MACRO_ENB,
    "eNB_Eurecom_0",
    0,
    208,
    34,
    PAGING_DRX_256,
    1, /* There are 2 addresses defined, but use only one by default */
    {
        {
            1,
            0,
            "192.168.12.87",
            "2001:660:5502:12:30da:829a:2343:b6cf"
        },
        {
            1,
            0,
            "192.168.12.86",
            ""
        }
    }
};

/* eNB 1 properties */
static Enb_properties_t enb_1_properties =
{
    347473,
    CELL_MACRO_ENB,
    "eNB_Eurecom_1",
    0,
    208,
    34,
    PAGING_DRX_256,
    1, /* There are 2 addresses defined, but use only one by default */
    {
        {
            1,
            0,
            "192.168.12.87",
            "2001:660:5502:12:30da:829a:2343:b6cf"
        },
        {
            1,
            0,
            "192.168.12.88",
            ""
        }
    }
};

/* eNB 2 properties */
static Enb_properties_t enb_2_properties =
{
    347474,
    CELL_MACRO_ENB,
    "eNB_Eurecom_2",
    0,
    208,
    34,
    PAGING_DRX_256,
    1,
    {
        {
            1,
            0,
            "192.168.12.87",
            "2001:660:5502:12:30da:829a:2343:b6cf"
        },
    }
};

/* eNB 3 properties */
static Enb_properties_t enb_3_properties =
{
    347475,
    CELL_MACRO_ENB,
    "eNB_Eurecom_3",
    0,
    208,
    34,
    PAGING_DRX_256,
    1,
    {
        {
            1,
            0,
            "192.168.12.87",
            "2001:660:5502:12:30da:829a:2343:b6cf"
        },
    }
};

static Enb_properties_t *enb_properties[] =
{
    &enb_0_properties,
    &enb_1_properties,
    &enb_2_properties,
    &enb_3_properties,
};

/*------------------------------------------------------------------------------*/
static void configure_rrc()
{
    uint32_t eNB_id_start = 0;
    uint32_t eNB_id_end = 1;
    uint32_t eNB_id;
    MessageDef *msg_p;

#   if defined(OAI_EMU)
    eNB_id_start = oai_emulation.info.first_enb_local;
    eNB_id_end = oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local;
#   endif

    for (eNB_id = eNB_id_start; (eNB_id < eNB_id_end) ; eNB_id++)
    {
        msg_p = itti_alloc_new_message (TASK_ENB_APP, RRC_CONFIGURATION_REQ);

        RRC_CONFIGURATION_REQ (msg_p).cell_identity =   enb_properties[eNB_id]->eNB_id;
        RRC_CONFIGURATION_REQ (msg_p).tac =             enb_properties[eNB_id]->tac;
        RRC_CONFIGURATION_REQ (msg_p).mcc =             enb_properties[eNB_id]->mcc;
        RRC_CONFIGURATION_REQ (msg_p).mnc =             enb_properties[eNB_id]->mnc;

        itti_send_msg_to_task (TASK_RRC_ENB, eNB_id, msg_p);
    }
}

# if defined(ENABLE_USE_MME)
static uint32_t eNB_app_register()
{
    uint32_t eNB_id_start = 0;
    uint32_t eNB_id_end = 1;
    uint32_t eNB_id;
    uint32_t mme_id;
    MessageDef *msg_p;
    uint32_t register_enb_pending = 0;

#   if defined(OAI_EMU)
    eNB_id_start = oai_emulation.info.first_enb_local;
    eNB_id_end = oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local;

    DevCheck(eNB_id_end <= NUMBER_OF_eNB_MAX, eNB_id_end, NUMBER_OF_eNB_MAX, 0);
#   endif
    DevCheck(eNB_id_end <= (sizeof(enb_properties) / sizeof(enb_properties[0])), eNB_id_end, (sizeof(enb_properties) / sizeof(enb_properties[0])), 0);

    for (eNB_id = eNB_id_start; (eNB_id < eNB_id_end) ; eNB_id++)
    {
#   if defined(OAI_EMU)
        if (oai_emulation.info.cli_start_enb[eNB_id] == 1)
#   endif
        {
            s1ap_register_enb_req_t *s1ap_register_eNB;
            uint32_t hash;

            /* Overwrite default eNB ID */
            hash = s1ap_generate_eNB_id ();
            enb_properties[eNB_id]->eNB_id = eNB_id + (hash & 0xFFFF8);

            if (EPC_MODE_ENABLED)
            {
                /* Overwrite default IP v4 address by value from command line */
                enb_properties[eNB_id]->mme_ip_address[0].ipv4_address = EPC_MODE_MME_ADDRESS;
            }

            /* note:  there is an implicit relationship between the data structure and the message name */
            msg_p = itti_alloc_new_message (TASK_ENB_APP, S1AP_REGISTER_ENB_REQ);

            s1ap_register_eNB = &S1AP_REGISTER_ENB_REQ(msg_p);

            /* Some default/random parameters */
            s1ap_register_eNB->eNB_id = enb_properties[eNB_id]->eNB_id;
            s1ap_register_eNB->cell_type = enb_properties[eNB_id]->cell_type;
            s1ap_register_eNB->eNB_name = enb_properties[eNB_id]->eNB_name;
            s1ap_register_eNB->tac = enb_properties[eNB_id]->tac;
            s1ap_register_eNB->mcc = enb_properties[eNB_id]->mcc;
            s1ap_register_eNB->mnc = enb_properties[eNB_id]->mnc;
            s1ap_register_eNB->default_drx = enb_properties[eNB_id]->default_drx;

            s1ap_register_eNB->nb_mme = enb_properties[eNB_id]->nb_mme;
            DevCheck(s1ap_register_eNB->nb_mme <= S1AP_MAX_NB_MME_IP_ADDRESS, eNB_id, s1ap_register_eNB->nb_mme, S1AP_MAX_NB_MME_IP_ADDRESS);

            for (mme_id = 0; mme_id < s1ap_register_eNB->nb_mme; mme_id++)
            {
                s1ap_register_eNB->mme_ip_address[mme_id].ipv4 = enb_properties[eNB_id]->mme_ip_address[mme_id].ipv4;
                s1ap_register_eNB->mme_ip_address[mme_id].ipv6 = enb_properties[eNB_id]->mme_ip_address[mme_id].ipv6;
                strncpy (s1ap_register_eNB->mme_ip_address[mme_id].ipv4_address,
                         enb_properties[eNB_id]->mme_ip_address[mme_id].ipv4_address,
                         sizeof(s1ap_register_eNB->mme_ip_address[0].ipv4_address));
                strncpy (s1ap_register_eNB->mme_ip_address[mme_id].ipv6_address,
                         enb_properties[eNB_id]->mme_ip_address[mme_id].ipv6_address,
                         sizeof(s1ap_register_eNB->mme_ip_address[0].ipv6_address));
            }

            itti_send_msg_to_task (TASK_S1AP, eNB_id, msg_p);

            register_enb_pending++;
        }
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

    configure_rrc();

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

#   if defined(OAI_EMU)
                        /* If also inform all NAS UE tasks */
                        for (instance = NB_eNB_INST + oai_emulation.info.first_ue_local;
                            instance < (NB_eNB_INST + oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local); instance ++)
                        {
                            msg_init_p = itti_alloc_new_message (TASK_ENB_APP, INITIALIZE_MESSAGE);
                            itti_send_msg_to_task (TASK_NAS_UE, instance, msg_init_p);
                        }
#   endif
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

            case S1AP_DEREGISTERED_ENB_IND:
                LOG_W(ENB_APP, "[eNB %d] Received %s: associated MME %d\n", instance, msg_name,
                      S1AP_DEREGISTERED_ENB_IND(msg_p).nb_mme);

                /* TODO handle recovering of registration */
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

        itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
    } while (1);
#endif

    return NULL;
}
