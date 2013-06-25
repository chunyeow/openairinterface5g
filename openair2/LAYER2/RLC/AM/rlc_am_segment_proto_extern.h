/***************************************************************************
                          rlc_am_segment_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#    ifndef __RLC_AM_SEGMENT_PROTO_EXTERN_H__
#        define __RLC_AM_SEGMENT_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
#        include "list.h"
//-----------------------------------------------------------------------------
extern inline u16_t rlc_am_get_next_sn (struct rlc_am_entity *rlcP, u16 snP);
extern inline int rlc_am_sn_in_tx_window (struct rlc_am_entity *rlcP, u16_t snP);

extern inline void rlc_am_sdu_confirm_map_register_pdu (mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP, mem_block_t * sdu_header_confirm_copyP);

extern inline void rlc_am_sdu_discard_map_register_pdu (mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP, mem_block_t * sdu_header_discard_copyP);

extern mem_block_t *rlc_am_segment_15 (struct rlc_am_entity *rlcP);
extern mem_block_t *rlc_am_segment_7 (struct rlc_am_entity *rlcP);
#    endif
