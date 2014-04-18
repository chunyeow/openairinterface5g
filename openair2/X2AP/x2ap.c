/*******************************************************************************
Eurecom OpenAirInterface Core Network
Copyright(c) 1999 - 2014 Eurecom

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
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "intertask_interface.h"

#include "x2ap.h"

#include "assertions.h"
#include "conversions.h"


void *x2ap_task(void *arg)
{
    MessageDef *received_msg = NULL;
    int         result;

    X2AP_DEBUG("Starting X2AP layer\n");

    x2ap_prepare_internal_data();

    itti_mark_task_ready(TASK_X2AP);

    while (1) {
        itti_receive_msg(TASK_X2AP, &received_msg);

        switch (ITTI_MSG_ID(received_msg)) {
            case TERMINATE_MESSAGE:
                itti_exit_task();
                break;

            default:
                X2AP_ERROR("Received unhandled message: %d:%s\n",
                           ITTI_MSG_ID(received_msg), ITTI_MSG_NAME(received_msg));
                break;
        }

        result = itti_free (ITTI_MSG_ORIGIN_ID(received_msg), received_msg);
        AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);

        received_msg = NULL;
    }
    return NULL;
}


