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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tree.h"

#include "intertask_interface.h"
#include "sgw_lite_defs.h"
#include "sgw_lite_context_manager.h"

RB_HEAD(mme_sgw_tunnel_tree, mme_sgw_tunnel_s) mme_sgw_tunnel_map;

//==============================================================================
// LOCAL function prototypes
//==============================================================================
static inline int32_t sgw_lite_compare_teid(
    struct mme_sgw_tunnel_s *a, struct mme_sgw_tunnel_s *b);

RB_PROTOTYPE(mme_sgw_tunnel_tree, mme_sgw_tunnel_s, node, sgw_lite_compare_teid)

static inline int32_t sgw_lite_compare_teid(
    struct mme_sgw_tunnel_s *a, struct mme_sgw_tunnel_s *b) {

    if(a->remote_teid > b->remote_teid)
        return 1;
    if(a->remote_teid < b->remote_teid)
        return -1;
    return 0;
}

RB_GENERATE(mme_sgw_tunnel_tree, mme_sgw_tunnel_s, node, sgw_lite_compare_teid)

mme_sgw_tunnel_t* sgw_lite_cm_create_s11_tunnel(uint32_t remote_teid) {

    mme_sgw_tunnel_t *collision_p;
    mme_sgw_tunnel_t *new_tunnel;

    new_tunnel = malloc(sizeof(mme_sgw_tunnel_t));

    if (new_tunnel == NULL) {
        /* Malloc failed, may be ENOMEM error */
        SGW_ERROR("Failed to create tunnel for remote_teid %u\n", remote_teid);
        return NULL;
    }

    new_tunnel->remote_teid = remote_teid;

    /* Trying to insert the new tunnel into the tree.
     * If collision_p is not NULL (0), it means tunnel is already present.
     */
    collision_p = RB_INSERT(mme_sgw_tunnel_tree, &mme_sgw_tunnel_map, new_tunnel);

    if (collision_p) {
        SGW_WARNING("This TEID already exists: %u\n", remote_teid);
        free(new_tunnel);
        new_tunnel = NULL;
    }

    return new_tunnel;
}

int mme_sgw_remove_s11_tunnel(mme_sgw_tunnel_t* tunnel) {

    mme_sgw_tunnel_t *temp;

    if (tunnel == NULL)
        return -1;

    temp = RB_REMOVE(mme_sgw_tunnel_tree, &mme_sgw_tunnel_map, tunnel);
    if (tunnel != temp)
        return -1;

    free(temp);
    return 0;
}
