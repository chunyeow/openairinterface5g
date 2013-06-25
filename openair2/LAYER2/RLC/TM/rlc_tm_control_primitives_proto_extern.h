/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

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
Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/*! \file rlc_tm_control_primitives_proto_extern.h
* \brief This file defines the prototypes of the functions dealing with the control primitives and initialization.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
* \note
* \bug
* \warning
*/
#    ifndef __RLC_TM_CONTROL_PRIMITIVES_H__
#        define __RLC_TM_CONTROL_PRIMITIVES_H__
//-----------------------------------------------------------------------------
#        include "rlc_tm_entity.h"
#        include "mem_block.h"
#        include "rrm_config_structs.h"
//-----------------------------------------------------------------------------
extern void     config_req_rlc_tm (struct rlc_tm_entity *rlcP, module_id_t module_idP,rlc_tm_info_t * config_tmP, rb_id_t rb_idP, rb_type_t rb_typeP);
extern void     send_rlc_tm_control_primitive (struct rlc_tm_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);
extern void     init_rlc_tm (struct rlc_tm_entity *rlcP);
extern void     rlc_tm_reset_state_variables (struct rlc_tm_entity *rlcP);
extern void     rlc_tm_free_all_resources (struct rlc_tm_entity *rlcP);
extern void     rlc_tm_set_configured_parameters (struct rlc_tm_entity *rlcP, mem_block_t *cprimitiveP);
#    endif
