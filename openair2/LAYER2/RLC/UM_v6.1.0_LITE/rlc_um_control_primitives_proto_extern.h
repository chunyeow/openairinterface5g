/***************************************************************************
                          rlc_um_control_primitives.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_UM_CONTROL_PRIMITIVES_H__
#        define __RLC_UM_CONTROL_PRIMITIVES_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
#        include "mem_block.h"
#        include "rrm_config_structs.h"
//-----------------------------------------------------------------------------
extern void     config_req_rlc_um (struct rlc_um_entity *rlcP, module_id_t module_idP, rlc_um_info_t * config_umP, u8_t rb_idP, rb_type_t rb_typeP);
extern void     send_rlc_um_control_primitive (struct rlc_um_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);
extern void     init_rlc_um (struct rlc_um_entity *rlcP);
extern void     rlc_um_reset_state_variables (struct rlc_um_entity *rlcP);
extern void     rlc_um_free_all_resources (struct rlc_um_entity *rlcP);
extern void     rlc_um_set_configured_parameters (struct rlc_um_entity *rlcP, mem_block_t *cprimitiveP);
//extern void     rlc_um_probing_get_buffer_occupancy_measurements (struct rlc_um_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP);

#    endif
