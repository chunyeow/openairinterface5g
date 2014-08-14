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
                          rlc_am_discard_rx_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_DISCARD_RX_PROTO_EXTERN_H__
#        define __RLC_AM_DISCARD_RX_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_received_sufi_ack_check_discard_procedures (struct rlc_am_entity *rlcP);
extern void     rlc_am_free_discard_procedure (mem_block_t * mb_current_procedureP);
extern inline void rlc_am_discard_free_receiver_buffer (struct rlc_am_entity *rlcP, uint16_t sn_mrw_iP, uint8_t nlengthP);
extern uint8_t      *retransmission_buffer_management_mrw (struct rlc_am_entity *rlcP, uint8_t * byte1P, uint8_t * byte_alignedP);
extern uint8_t      *retransmission_buffer_management_mrw_ack (struct rlc_am_entity *rlcP, uint8_t * byte1P, uint8_t * byte_alignedP);
#    endif
