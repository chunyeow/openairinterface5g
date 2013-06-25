/***************************************************************************
                          rlc_am_demux_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_DEMUX_PROTO_EXTERN_H__
#        define __RLC_AM_DEMUX_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "mac_primitives.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_demux_routing (struct rlc_am_entity *rlcP, unsigned int traffic_typeP, struct mac_data_ind data_indP);
#    endif
