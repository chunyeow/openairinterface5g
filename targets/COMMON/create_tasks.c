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

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
# include "create_tasks.h"
# include "log.h"

# ifdef OPENAIR2
#   if defined(ENABLE_USE_MME)
#     include "sctp_eNB_task.h"
#     include "s1ap_eNB.h"
#     include "nas_ue_task.h"
#   endif
#   include "RRC/LITE/defs.h"
# endif
# include "enb_app.h"

int create_tasks(uint32_t enb_nb, uint32_t ue_nb)
{
# ifdef OPENAIR2
    {
#   if defined(ENABLE_USE_MME)
        {
            if (enb_nb > 0)
            {
                if (itti_create_task (TASK_SCTP, sctp_eNB_task, NULL) < 0)
                {
                    LOG_E (EMU, "Create task failed");
                    LOG_D (EMU, "Initializing SCTP task interface: FAILED\n");
                    return -1;
                }

                if (itti_create_task (TASK_S1AP, s1ap_eNB_task, NULL) < 0)
                {
                    LOG_E (EMU, "Create task failed");
                    LOG_D (EMU, "Initializing S1AP task interface: FAILED\n");
                    return -1;
                }
            }

            if (ue_nb > 0)
            {
                if (itti_create_task (TASK_NAS_UE, nas_ue_task, NULL) < 0)
                {
                    LOG_E (EMU, "Create task failed");
                    LOG_D (EMU, "Initializing NAS UE task interface: FAILED\n");
                    return -1;
                }
            }
        }
#   endif

        if (enb_nb > 0)
        {
            if (itti_create_task (TASK_RRC_ENB, rrc_enb_task, NULL) < 0)
            {
                LOG_E (EMU, "Create task failed");
                LOG_D (EMU, "Initializing RRC eNB task interface: FAILED\n");
                exit (-1);
            }
        }

        if (ue_nb > 0)
        {
            if (itti_create_task (TASK_RRC_UE, rrc_ue_task, NULL) < 0)
            {
                LOG_E (EMU, "Create task failed");
                LOG_D (EMU, "Initializing RRC UE task interface: FAILED\n");
                exit (-1);
            }
        }
    }
# endif

    if (itti_create_task (TASK_L2L1, l2l1_task, NULL) < 0)
    {
        LOG_E (EMU, "Create task failed");
        LOG_D (EMU, "Initializing L2L1 task interface: FAILED\n");
        return -1;
    }

    if (enb_nb > 0)
    {
        /* Last task to create, others task must be ready before its start */
        if (itti_create_task (TASK_ENB_APP, eNB_app_task, NULL) < 0)
        {
            LOG_E (EMU, "Create task failed");
            LOG_D (EMU, "Initializing eNB APP task interface: FAILED\n");
            return -1;
        }
    }
    return 0;
}
#endif
