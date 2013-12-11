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
# include "UTIL/LOG/log.h"

# include "nas_user.h"
# include "user_api.h"
# include "nas_parser.h"

# define NAS_UE_AUTOSTART 1

extern unsigned char NB_eNB_INST;

static int user_fd;

static int nas_ue_process_events(struct epoll_event *events, int nb_events, unsigned int Mod_id)
{
  int event;
  int exit_loop = FALSE;

  LOG_I(NAS, "[UE %d] Received %d events\n", Mod_id, nb_events);

  for (event = 0; event < nb_events; event++) {
    if (events[event].events != 0)
    {
      /* If the event has not been yet been processed (not an itti message) */
      if (events[event].data.fd == user_fd) {
        exit_loop = user_api_receive_and_process(&user_fd);
      } else {
        LOG_E(NAS, "[UE %d] Received an event from an unknown fd %d!\n", Mod_id, events[event].data.fd);
      }
    }
  }

  return (exit_loop);
}

void *nas_ue_task(void *args_p) {
  int                   nb_events;
  struct epoll_event   *events;
  MessageDef           *msg_p;
  const char           *msg_name;
  instance_t            instance;
  unsigned int          Mod_id;

  itti_mark_task_ready (TASK_NAS_UE);

  /* Initialize UE NAS (EURECOM-NAS) */
  {
    /* Initialize user interface (to exchange AT commands with user process) */
    {
      if (user_api_initialize (NAS_PARSER_DEFAULT_USER_HOSTNAME, NAS_PARSER_DEFAULT_USER_PORT_NUMBER, NULL,
                               NULL) != RETURNok) {
        LOG_E(NAS, "[UE] user interface initialization failed!");
        exit (EXIT_FAILURE);
      }
      user_fd = user_api_get_fd ();
      itti_subscribe_event_fd (TASK_NAS_UE, user_fd);
    }

    /* Initialize NAS user */
    nas_user_initialize (&user_api_emm_callback, &user_api_esm_callback, FIRMWARE_VERSION);
  }

  while(1) {
    // Wait for a message or an event
    itti_receive_msg (TASK_NAS_UE, &msg_p);

    if (msg_p != NULL) {
      msg_name = ITTI_MSG_NAME (msg_p);
      instance = ITTI_MSG_INSTANCE (msg_p);
      Mod_id = instance - NB_eNB_INST;

      switch (ITTI_MSG_ID(msg_p)) {
        case INITIALIZE_MESSAGE:
          LOG_I(NAS, "[UE %d] Received %s\n", Mod_id, msg_name);
  #if defined(NAS_UE_AUTOSTART)
  #endif
          break;

        case TERMINATE_MESSAGE:
          itti_exit_task ();
          break;

        case MESSAGE_TEST:
          LOG_I(NAS, "[UE %d] Received %s\n", Mod_id, msg_name);
          break;

        case NAS_DOWNLINK_DATA_IND:
          LOG_I(NAS, "[UE %d] Received %s: UEid %u, length %u\n", Mod_id, msg_name,
                NAS_DOWNLINK_DATA_IND (msg_p).UEid, NAS_DOWNLINK_DATA_IND (msg_p).nasMsg.length);
          break;

        default:
          LOG_E(NAS, "[UE %d] Received unexpected message %s\n", Mod_id, msg_name);
          break;
      }

      itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
      msg_p = NULL;
      }

    nb_events = itti_get_events(TASK_NAS_UE, &events);
    if ((nb_events > 0) && (events != NULL)) {
      if (nas_ue_process_events(events, nb_events, Mod_id) == TRUE) {
        LOG_E(NAS, "[UE %d] Received exit loop\n", Mod_id);
      }
    }
  }
}
#endif
