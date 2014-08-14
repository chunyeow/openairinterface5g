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
