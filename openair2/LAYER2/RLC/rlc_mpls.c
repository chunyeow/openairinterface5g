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
/*
                                rlc_mpls.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*/

#define RLC_MPLS_C
#include "rlc.h"


//-----------------------------------------------------------------------------
rlc_op_status_t mpls_rlc_data_req     (
    const module_id_t enb_module_idP,
    const module_id_t ue_module_idP,
    const frame_t frame,
    const rb_id_t rb_idP,
    const sdu_size_t sdu_sizeP,
    mem_block_t* const sduP) {
//-----------------------------------------------------------------------------
  // third arg should be set to 1 or 0
  return rlc_data_req(enb_module_idP, ue_module_idP, frame, ENB_FLAG_NO, SRB_FLAG_NO, MBMS_FLAG_NO, rb_idP, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, sdu_sizeP, sduP);
}

