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
rlc_op_status_t mpls_rlc_data_req     (module_id_t enb_module_idP, module_id_t ue_module_idP, u32_t frame, rb_id_t rb_idP, sdu_size_t sdu_sizeP, mem_block_t* sduP) {
//-----------------------------------------------------------------------------
  // third arg should be set to 1 or 0
  return rlc_data_req(enb_module_idP, ue_module_idP, frame, 0, 0,rb_idP, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, sdu_sizeP, sduP);
}

