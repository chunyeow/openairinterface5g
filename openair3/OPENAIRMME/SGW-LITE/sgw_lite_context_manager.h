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

#include "tree.h"

#ifndef SGW_LITE_CONTEXT_MANAGER_H_
#define SGW_LITE_CONTEXT_MANAGER_H_

typedef struct mme_sgw_tunnel_s {
    uint32_t remote_teid;  ///< Tunnel endpoint Identifier
    RB_ENTRY(mme_sgw_tunnel_s) node;
} mme_sgw_tunnel_t;

mme_sgw_tunnel_t * sgw_lite_cm_create_s11_tunnel(uint32_t remote_teid);

int mme_sgw_remove_s11_tunnel(mme_sgw_tunnel_t* tunnel);

#endif /* SGW_LITE_CONTEXT_MANAGER_H_ */
