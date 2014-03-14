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
                          rlc_am_receiver_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_RECEIVER_PROTO_EXTERN_H__
#        define __RLC_AM_RECEIVER_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     receiver_retransmission_management (struct rlc_am_entity *rlcP, mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP);
extern void     free_receiver_buffer (struct rlc_am_entity *rlcP, uint16_t indexP);
extern void     insert_into_receiver_buffer (struct rlc_am_entity *rlcP, uint16_t indexP, mem_block_t * pduP);
extern void     process_receiver_buffer_15 (struct rlc_am_entity *rlcP);
extern void     process_receiver_buffer_7 (struct rlc_am_entity *rlcP);
#    endif
