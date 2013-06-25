/*
                              rlc_um_constants.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_UM_CONSTANTS_H__
#        define __RLC_UM_CONSTANTS_H__


// SN Field
#        define RLC_UM_SN_7BITS_MASK                        0xFF
// li field (values shifted 1 bit left)
#        define RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU            16
//----------------------------------------------------------
// Events defined for state model of the acknowledged mode entity
#        define RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT                 0x00
#        define RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT  0x01
#        define RLC_UM_RECEIVE_CRLC_SUSPEND_REQ_EVENT                                 0x10
#        define RLC_UM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT                                0x11
#        define RLC_UM_RECEIVE_CRLC_RESUME_REQ_EVENT                                  0x12
#    endif
