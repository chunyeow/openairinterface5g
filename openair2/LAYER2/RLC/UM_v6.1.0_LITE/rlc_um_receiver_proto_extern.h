/*
                              rlc_um_receiver_proto_extern.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_UM_RECEIVER_PROTO_EXTERN_H__
#        define __RLC_UM_RECEIVER_PROTO_EXTERN_H__

#        include "rlc_um_entity.h"
#        include "mac_primitives.h"
void            rlc_um_receive (struct rlc_um_entity *rlcP, struct mac_data_ind data_indP);
#    endif
