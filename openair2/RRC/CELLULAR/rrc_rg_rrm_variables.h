/***************************************************************************
                          rrc_rg_rrm_variables.h
                          -------------------
    copyright            : (C) 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Definition of RRC interface to RRM
 **************************************************************************/
#ifndef __RRC_RG_RRM_VARS_H__
#define __RRC_RG_RRM_VARS_H__
//-----------------------------------------------------------------------------
//#include "platform.h"
#include "rrc_rrm_primitives.h"

#define RRC_RRM_RX_BUFFER_SIZE RRM_MAX_MESSAGE_SIZE * 10

struct rrc_rg_rrm_variables{
  u8  rx_buffer[RRC_RRM_RX_BUFFER_SIZE];
  u8  rx_message[RRM_MAX_MESSAGE_SIZE];
  int rx_bytes;

  int input_fifo;
  int output_fifo;

  int connected_to_rrm;
  int equipment_id;

  u8 *config_to_be_expanded;
  int config_read_byte_index;
  int config_read_bit_index;

//  void (*rrc_handler[RPC_NB_RPC]) (void *rpcP, int lengthP);

} rrc_rg_rrm_variables;

#endif
