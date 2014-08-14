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

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
/***************************************************************************
                          rlc_am_segment_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#    ifndef __RLC_AM_SEGMENT_PROTO_EXTERN_H__
#        define __RLC_AM_SEGMENT_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
#        include "list.h"
//-----------------------------------------------------------------------------
extern inline uint16_t rlc_am_get_next_sn (struct rlc_am_entity *rlcP, uint16_t snP);
extern inline int rlc_am_sn_in_tx_window (struct rlc_am_entity *rlcP, uint16_t snP);

extern inline void rlc_am_sdu_confirm_map_register_pdu (mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP, mem_block_t * sdu_header_confirm_copyP);

extern inline void rlc_am_sdu_discard_map_register_pdu (mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP, mem_block_t * sdu_header_discard_copyP);

extern mem_block_t *rlc_am_segment_15 (struct rlc_am_entity *rlcP);
extern mem_block_t *rlc_am_segment_7 (struct rlc_am_entity *rlcP);
#    endif
