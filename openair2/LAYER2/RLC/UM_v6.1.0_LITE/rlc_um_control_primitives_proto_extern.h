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

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/
#    ifndef __RLC_UM_CONTROL_PRIMITIVES_H__
#        define __RLC_UM_CONTROL_PRIMITIVES_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
#        include "mem_block.h"
#        include "rrm_config_structs.h"
//-----------------------------------------------------------------------------
extern void     config_req_rlc_um (struct rlc_um_entity *rlcP, module_id_t module_idP, rlc_um_info_t * config_umP, uint8_t rb_idP, rb_type_t rb_typeP);
extern void     send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);
extern void     init_rlc_um (struct rlc_um_entity *rlcP);
extern void     rlc_um_reset_state_variables (struct rlc_um_entity *rlcP);
extern void     rlc_um_free_all_resources (struct rlc_um_entity *rlcP);
extern void     rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, mem_block_t *cprimitiveP);
//extern void     rlc_um_probing_get_buffer_occupancy_measurements (struct rlc_um_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP);

#    endif
