/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "tree.h"
#include "assertions.h"

#include "intertask_interface.h"

#include "timer.h"
#include "s1ap_common.h"
#include "s1ap_mme_retransmission.h"

inline int s1ap_mme_timer_map_compare_id(
    struct s1ap_timer_map_s *p1, struct s1ap_timer_map_s *p2);

/* Reference to tree root element */
RB_HEAD(s1ap_timer_map, s1ap_timer_map_s) s1ap_timer_tree = RB_INITIALIZER();

/* RB tree functions for s1ap timer map are not exposed to the rest of the code
 * only declare prototypes here.
 */
RB_PROTOTYPE(s1ap_timer_map, s1ap_timer_map_s, entries,
             s1ap_mme_timer_map_compare_id);

RB_GENERATE(s1ap_timer_map, s1ap_timer_map_s, entries,
            s1ap_mme_timer_map_compare_id);

inline int s1ap_mme_timer_map_compare_id(
    struct s1ap_timer_map_s *p1, struct s1ap_timer_map_s *p2)
{
    if (p1->mme_ue_s1ap_id > 0) {
        if (p1->mme_ue_s1ap_id > p2->mme_ue_s1ap_id) {
            return 1;
        }
        if (p1->mme_ue_s1ap_id < p2->mme_ue_s1ap_id) {
            return -1;
        }
        return 0;
    }
    if (p1->timer_id > p2->timer_id) {
        return 1;
    }
    if (p1->timer_id < p2->timer_id) {
        return -1;
    }
    /* Match -> return 0 */
    return 0;
}

int s1ap_timer_insert(uint32_t mme_ue_s1ap_id, long timer_id)
{
    struct s1ap_timer_map_s *new;

    new = malloc(sizeof(struct s1ap_timer_map_s));

    new->timer_id = timer_id;
    new->mme_ue_s1ap_id = mme_ue_s1ap_id;

    if (RB_INSERT(s1ap_timer_map, &s1ap_timer_tree, new) != NULL) {
        S1AP_ERROR("Timer with id 0x%lx already exists\n", timer_id);
        free(new);
        return -1;
    }

    return 0;
}

int s1ap_handle_timer_expiry(timer_has_expired_t *timer_has_expired)
{
    struct s1ap_timer_map_s *find;
    struct s1ap_timer_map_s elm;

    DevAssert(timer_has_expired != NULL);

    memset(&elm, 0, sizeof(elm));

    elm.timer_id = timer_has_expired->timer_id;

    if ((find = RB_FIND(s1ap_timer_map, &s1ap_timer_tree, &elm)) == NULL) {
        S1AP_WARN("Timer id 0x%lx has not been found in tree. Maybe the timer "
        "reference has been removed before receiving tiemr signal\n",
        timer_has_expired->timer_id);
        return 0;
    }

    /* Remove the timer from the map */
    RB_REMOVE(s1ap_timer_map, &s1ap_timer_tree, find);

    /* Destroy the element */
    free(find);

    /* TODO: notify NAS and remove ue context */
    return 0;
}

int s1ap_timer_remove_ue(uint32_t mme_ue_s1ap_id)
{
    struct s1ap_timer_map_s *find;

    S1AP_DEBUG("Removing timer associated with UE 0x%08x\n",
               mme_ue_s1ap_id);

    DevAssert(mme_ue_s1ap_id != 0);

    RB_FOREACH(find, s1ap_timer_map, &s1ap_timer_tree) {
        if (find->mme_ue_s1ap_id == mme_ue_s1ap_id) {
            timer_remove(find->timer_id);
            /* Remove the timer from the map */
            RB_REMOVE(s1ap_timer_map, &s1ap_timer_tree, find);
            /* Destroy the element */
            free(find);
        }
    }
    return 0;
}
