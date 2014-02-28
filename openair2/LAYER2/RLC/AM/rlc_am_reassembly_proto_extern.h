/***************************************************************************
                          rlc_am_reassembly_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_REASSEMBLY_PROTO_EXTERN_H__
#        define __RLC_AM_REASSEMBLY_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
//-----------------------------------------------------------------------------
extern void     reassembly (uint8_t * srcP, uint16_t lengthP, struct rlc_am_entity *rlcP);
extern void     send_sdu (struct rlc_am_entity *rlcP);
#    endif
