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

/*! \file s1ap_eNB_overload.c
 * \brief s1ap procedures for overload messages within eNB
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB_defs.h"

#include "s1ap_eNB.h"
#include "s1ap_eNB_ue_context.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_overload.h"

#include "sctp_primitives_client.h"

#include "assertions.h"

int s1ap_eNB_handle_overload_start(eNB_mme_desc_t *eNB_desc_p,
                                   sctp_queue_item_t *packet_p,
                                   struct s1ap_message_s *message_p)
{
    OverloadStartIEs_t  *overload_start_p;
    s1ap_eNB_mme_data_t *mme_desc_p;

    overload_start_p = &message_p->msg.overloadStartIEs;

    DevCheck(overload_start_p->overloadResponse.present ==
             OverloadResponse_PR_overloadAction,
             OverloadResponse_PR_overloadAction, 0, 0);

    /* Non UE-associated signalling -> stream 0 */
    DevCheck(packet_p->local_stream == 0, packet_p->local_stream,
             packet_p->remote_port, packet_p->assoc_id);

    if ((mme_desc_p = s1ap_eNB_get_MME(eNB_desc_p, packet_p->assoc_id)) == NULL) {
        /* No MME context associated */
        return -1;
    }

    /* Mark the MME as overloaded and set the overload state according to
     * the value received.
     */
    mme_desc_p->state = S1AP_ENB_OVERLOAD;
    mme_desc_p->overload_state =
        overload_start_p->overloadResponse.choice.overloadAction;

    return 0;
}

int s1ap_eNB_handle_overload_stop(eNB_mme_desc_t *eNB_desc_p,
                                  sctp_queue_item_t *packet_p,
                                  struct s1ap_message_s *message_p)
{
    /* We received Overload stop message, meaning that the MME is no more
     * overloaded. This is an empty message, with only message header and no
     * Information Element.
     */
    s1ap_eNB_mme_data_t *mme_desc_p;

    /* Non UE-associated signalling -> stream 0 */
    DevCheck(packet_p->local_stream == 0, packet_p->local_stream,
             packet_p->remote_port, packet_p->assoc_id);

    if ((mme_desc_p = s1ap_eNB_get_MME(eNB_desc_p, packet_p->assoc_id)) == NULL) {
        /* No MME context associated */
        return -1;
    }

    mme_desc_p->state = S1AP_ENB_STATE_CONNECTED;
    mme_desc_p->overload_state = S1AP_NO_OVERLOAD;
    return 0;
}
