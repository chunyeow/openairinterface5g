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
# include "nas_ue_task.h"
# include "log.h"

extern unsigned char NB_eNB_INST;

void *nas_ue_task(void *args_p) {
  MessageDef *msg_p;
  const char *msg_name;
  instance_t instance;
  unsigned int Mod_id;

  itti_mark_task_ready (TASK_NAS_UE);

  while(1) {
    // Wait for a message
    itti_receive_msg (TASK_NAS_UE, &msg_p);

    msg_name = ITTI_MSG_NAME (msg_p);
    instance = ITTI_MSG_INSTANCE (msg_p);
    Mod_id = instance - NB_eNB_INST;

    switch (ITTI_MSG_ID(msg_p)) {
      case TERMINATE_MESSAGE:
        itti_exit_task ();
        break;

      case MESSAGE_TEST:
        LOG_I(NAS, "[UE %d] Received %s\n", Mod_id, msg_name);
        break;


      default:
        LOG_E(NAS, "[UE %d] Received unexpected message %s\n", Mod_id, msg_name);
        break;
    }

    free (msg_p);
    msg_p = NULL;
  }
}
#endif
