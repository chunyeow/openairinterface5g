/*******************************************************************************
Eurecom OpenAirInterface 2
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
/***************************************************************************
                          rlc_am_retrans_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_RETRANS_PROTO_EXTERN_H__
#        define __RLC_AM_RETRANS_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
//-----------------------------------------------------------------------------
extern int16_t      retransmission_buffer_management_ack (struct rlc_am_entity *rlcP, uint8_t * sufiP, uint8_t byte_alignedP, int16_t * first_error_indicated_snP);
extern uint8_t      *retransmission_buffer_management_bitmap (struct rlc_am_entity *rlcP, uint8_t * bitmap_sufiP, uint8_t byte_alignedP, int16_t * first_error_indicated_snP);
extern uint8_t      *retransmission_buffer_management_list (struct rlc_am_entity *rlcP, uint8_t * bitmap_sufiP, uint8_t byte_alignedP, int16_t * first_error_indicated_snP);
extern void     free_retransmission_buffer (struct rlc_am_entity *rlcP, uint16_t indexP);
extern void     free_retransmission_buffer_no_confirmation (struct rlc_am_entity *rlcP, uint16_t indexP);
extern void     insert_into_retransmission_buffer (struct rlc_am_entity *rlcP, uint16_t indexP, mem_block_t * pduP);
extern uint8_t       retransmit_pdu (struct rlc_am_entity *rlcP, uint16_t snP);
extern void     rlc_am_get_not_acknowledged_pdu (struct rlc_am_entity *rlcP);
extern void     rlc_am_get_not_acknowledged_pdu_vt_s_minus_1 (struct rlc_am_entity *rlcP);
extern void     rlc_am_get_not_acknowledged_pdu_optimized (struct rlc_am_entity *rlcP);
#    endif
