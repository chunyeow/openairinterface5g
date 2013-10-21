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

#include "tree.h"

#ifndef S1AP_MME_RETRANSMISSION_H_
#define S1AP_MME_RETRANSMISSION_H_

typedef struct s1ap_timer_map_s {
    long     timer_id;
    uint32_t mme_ue_s1ap_id;

    RB_ENTRY(s1ap_timer_map_s) entries;
} s1ap_timer_map_t;

inline int s1ap_mme_timer_map_compare_id(
    struct s1ap_timer_map_s *p1, struct s1ap_timer_map_s *p2);

int s1ap_handle_timer_expiry(timer_has_expired_t *timer_has_expired);

int s1ap_timer_insert(uint32_t mme_ue_s1ap_id, long timer_id);

int s1ap_timer_remove_ue(uint32_t mme_ue_s1ap_id);

#endif /* S1AP_MME_RETRANSMISSION_H_ */
