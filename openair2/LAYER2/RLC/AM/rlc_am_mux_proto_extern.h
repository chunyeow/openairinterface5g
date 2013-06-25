/***************************************************************************
                          rlc_am_mux_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_MUX_PROTO_EXTERN_H__
#        define __RLC_AM_MUX_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
//-----------------------------------------------------------------------------
#        ifdef NODE_MT
extern void     rlc_am_mux_ue (struct rlc_am_entity *rlcP, unsigned int traffic_typeP);
#        endif
#        ifdef NODE_RG
extern void     rlc_am_mux_rg (struct rlc_am_entity *rlcP, unsigned int traffic_typeP);
#        endif
#    endif
