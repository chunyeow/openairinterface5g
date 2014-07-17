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
