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

/*! \file s1ap_eNB_ue_context.c
 * \brief s1ap UE context management within eNB
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tree.h"

#include "intertask_interface.h"

#include "s1ap_common.h"
#include "s1ap_eNB_defs.h"
#include "s1ap_eNB_ue_context.h"

inline int s1ap_eNB_compare_eNB_ue_s1ap_id(
    struct s1ap_eNB_ue_context_s *p1, struct s1ap_eNB_ue_context_s *p2)
{
    if (p1->eNB_ue_s1ap_id > p2->eNB_ue_s1ap_id) {
        return 1;
    }
    if (p1->eNB_ue_s1ap_id < p2->eNB_ue_s1ap_id) {
        return -1;
    }
    return 0;
}

/* Generate the tree management functions */
RB_GENERATE(s1ap_ue_map, s1ap_eNB_ue_context_s, entries,
            s1ap_eNB_compare_eNB_ue_s1ap_id);

struct s1ap_eNB_ue_context_s *s1ap_eNB_allocate_new_UE_context(void)
{
    struct s1ap_eNB_ue_context_s *new_p;

    new_p = malloc(sizeof(struct s1ap_eNB_ue_context_s));

    if (new_p == NULL) {
        S1AP_ERROR("Cannot allocate new ue context\n");
        return NULL;
    }

    memset(new_p, 0, sizeof(struct s1ap_eNB_ue_context_s));

    return new_p;
}

struct s1ap_eNB_ue_context_s *s1ap_eNB_get_ue_context(
    s1ap_eNB_instance_t *instance_p,
    uint32_t eNB_ue_s1ap_id)
{
    s1ap_eNB_ue_context_t temp;

    memset(&temp, 0, sizeof(struct s1ap_eNB_ue_context_s));

    /* eNB ue s1ap id = 24 bits wide */
    temp.eNB_ue_s1ap_id = eNB_ue_s1ap_id & 0x00FFFFFF;

    return RB_FIND(s1ap_ue_map, &instance_p->s1ap_ue_head, &temp);
}

void s1ap_eNB_free_ue_context(struct s1ap_eNB_ue_context_s *ue_context_p)
{
    if (ue_context_p == NULL) {
        S1AP_ERROR("Trying to free a NULL context\n");
        return;
    }

    /* TODO: check that context is currently not in the tree of known
     * contexts.
     */

    free(ue_context_p);
}
