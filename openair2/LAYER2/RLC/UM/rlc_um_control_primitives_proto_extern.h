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
extern void     config_req_rlc_um (struct rlc_um_entity *rlcP, UM * config_umP, void *upper_layerP, void *(*data_indP) (void *, struct mem_block * sduP), uint8_t rb_idP);
extern void     send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, struct mem_block *cprimitiveP);
extern void     init_rlc_um (struct rlc_um_entity *rlcP);
extern void     rlc_um_reset_state_variables (struct rlc_um_entity *rlcP);
extern void     rlc_um_free_all_resources (struct rlc_um_entity *rlcP);
extern void     rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, struct mem_block *cprimitiveP);
#    endif
