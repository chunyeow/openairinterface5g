/***************************************************************************
                          rlc_um_segment_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_UM_SEGMENT_PROTO_EXTERN_H__
#        define __RLC_UM_SEGMENT_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
//-----------------------------------------------------------------------------
extern u8_t     rlc_um_crc8_xor (char *mem_areaP, signed int num_bytesP);
extern void     rlc_um_segment_15 (struct rlc_um_entity *rlcP);
extern void     rlc_um_segment_7 (struct rlc_um_entity *rlcP);
#    endif
