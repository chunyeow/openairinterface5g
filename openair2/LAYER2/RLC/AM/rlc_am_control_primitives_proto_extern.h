/***************************************************************************
                          rlc_am_control_primitives_proto_extern.h  -
                             -------------------
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_CONTROL_PRIMITIVES_H__
#        define __RLC_AM_CONTROL_PRIMITIVES_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "mem_block.h"
#        include "rrm_config_structs.h"
//-----------------------------------------------------------------------------
extern void     config_req_rlc_am (struct rlc_am_entity *rlcP, module_id_t module_idP, rlc_am_info_t * config_amP, u8_t rb_idP, rb_type_t rb_typeP);
extern void     send_rlc_am_control_primitive (struct rlc_am_entity *rlcP, module_id_t module_idP, mem_block_t * cprimitiveP);
extern void     init_rlc_am (struct rlc_am_entity *rlcP);
extern void     rlc_am_reset_state_variables (struct rlc_am_entity *rlcP);
extern void     rlc_am_alloc_buffers_after_establishment (struct rlc_am_entity *rlcP);
extern void     rlc_am_discard_all_pdus (struct rlc_am_entity *rlcP);
extern void     rlc_am_stop_all_timers (struct rlc_am_entity *rlcP);
extern void     rlc_am_free_all_resources (struct rlc_am_entity *rlcP);
extern void     rlc_am_set_configured_parameters (struct rlc_am_entity *rlcP, mem_block_t * cprimitiveP);
//extern void     rlc_am_probing_get_buffer_occupancy_measurements (struct rlc_am_entity *rlcP, probing_report_traffic_rb_parameters *reportP, int measurement_indexP);
#    endif
