/***************************************************************************
                          rlc_am_demux.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
//#include "mac_log_interface_struct.h"
#include "LAYER2/RLC/rlc.h"

#include "mac_primitives.h"
#include "list.h"
#include "rlc_am_entity.h"
#include "rlc_am_structs.h"
#include "rlc_am_constants.h"
#include "rlc_am_receiver_proto_extern.h"
#include "rlc_am_reset_proto_extern.h"
#include "rlc_am_status_proto_extern.h"
#include "rlc_am_timers_proto_extern.h"
#include "LAYER2/MAC/extern.h"



#define DEBUG_DEMUX_RESET
#define DEBUG_DEMUX

//-----------------------------------------------------------------------------
void
rlc_am_demux_routing (struct rlc_am_entity *rlcP, unsigned int traffic_typeP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  struct rlc_am_pdu_header *data;
  struct rlc_am_reset_header *control;
  mem_block_t      *tb;
  u8_t             *first_byte;
  u16_t             tb_size_in_bytes;
  u8_t              first_bit;
  u8_t              bits_to_shift;
  u8_t              bits_to_shift_last_loop;

  u8_t              data_received;
  int index;

  //-------------------------------------------------------
  //   D A T A    P D U
  //-------------------------------------------------------
  data_received = 0;
  while ((tb = list_remove_head (&data_indP.data))) {
    if (!(((struct mac_tb_ind *) (tb->data))->error_indication)) {

      first_byte = ((struct mac_tb_ind *) (tb->data))->data_ptr;

      tb_size_in_bytes = data_indP.tb_size >> 3;
      first_bit = ((struct mac_tb_ind *) (tb->data))->first_bit;
      if (first_bit > 0) {
        // shift data of transport_block TO CHECK
        bits_to_shift_last_loop = 0;
        while ((tb_size_in_bytes)) {
          bits_to_shift = first_byte[tb_size_in_bytes] >> (8 - first_bit);
          first_byte[tb_size_in_bytes] = (first_byte[tb_size_in_bytes] << first_bit) | (bits_to_shift_last_loop);
          tb_size_in_bytes -= 1;
          bits_to_shift_last_loop = bits_to_shift;
        }
        first_byte[0] = (first_byte[0] << first_bit) | (bits_to_shift_last_loop);
      }
      ((struct rlc_am_rx_pdu_management *) (tb->data))->first_byte = first_byte;
      data = (struct rlc_am_pdu_header *) (first_byte);

      if ((data->byte1 & RLC_DC_MASK) == RLC_DC_DATA_PDU) {
#ifdef DEBUG_DEMUX
        msg ("[RLC_AM][RB %d][DEMUX] RX AMD PDU  Frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
	for (index=0; index < rlcP->pdu_size ; index++) {
	  msg("%02X.",first_byte[index]);
	}
	msg("\n");
#endif

	rlcP->stat_rx_data_pdu += 1;
       
        if (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_DATA) {
          ((struct rlc_am_rx_pdu_management *) (tb->data))->piggybacked_processed = 0;
          receiver_retransmission_management (rlcP, tb, data);
          // pdu is data;
          data_received = 1;
        } else {
#ifdef DEBUG_DEMUX
          msg ("[RLC_AM][RB %d][DEMUX] DROP DATA TB NOT ALLOWED IN PROTOCOL STATE 0x%02X\n", rlcP->rb_id, rlcP->protocol_state);
#endif
          free_mem_block (tb);
        }

      } else {
       rlcP->stat_rx_control_pdu += 1;
        control = (struct rlc_am_reset_header *) first_byte;
        if ((control->byte1 & RLC_PDU_TYPE_MASK) == RLC_PDU_TYPE_STATUS) {
          if (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_DATA) {
#ifdef DEBUG_DEMUX
            msg ("[RLC_AM][RB %d][DEMUX] RX STATUS PDU ON DTCH  Frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
            rlc_am_process_status_info (rlcP, &(control->byte1));
          }
        } else if ((control->byte1 & RLC_PDU_TYPE_MASK) == RLC_PDU_TYPE_RESET) {
#ifdef DEBUG_DEMUX_RESET
          msg ("[RLC_AM][RB %d][DEMUX] RX RESET PDU  Frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
          process_reset (tb, control, rlcP);
        } else if ((control->byte1 & RLC_PDU_TYPE_MASK) == RLC_PDU_TYPE_RESET_ACK) {
#ifdef DEBUG_DEMUX_RESET
          msg ("[RLC_AM][RB %d][DEMUX] RX RESET ACK PDU  Frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
          process_reset_ack (tb, control, rlcP);
#ifndef USER_MODE
rlc_info_t Rlc_info_am_config1;

  Rlc_info_am_config1.rlc_mode=RLC_AM;	
  Rlc_info_am_config1.rlc.rlc_am_info.sdu_discard_mode      = SDU_DISCARD_MODE_RESET;//SDU_DISCARD_MODE_MAX_DAT_RETRANSMISSION;//
  Rlc_info_am_config1.rlc.rlc_am_info.timer_poll            = 0;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_poll_prohibit   = 0;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_discard         = 500;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_poll_periodic   = 0;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_status_prohibit = 250;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_status_periodic = 500;
  Rlc_info_am_config1.rlc.rlc_am_info.timer_rst             = 250;//250
  Rlc_info_am_config1.rlc.rlc_am_info.max_rst               = 500;//500
  Rlc_info_am_config1.rlc.rlc_am_info.timer_mrw             = 0;
  
  Rlc_info_am_config1.rlc.rlc_am_info.pdu_size              = 32; //416; // in bits
  //Rlc_info_am.rlc.rlc_am_info.in_sequence_delivery  = 1;//boolean
  Rlc_info_am_config1.rlc.rlc_am_info.max_dat               = 32;//127;
  
  Rlc_info_am_config1.rlc.rlc_am_info.poll_pdu              = 0;
  Rlc_info_am_config1.rlc.rlc_am_info.poll_sdu              = 0;//256;/
  
  

  Rlc_info_am_config1.rlc.rlc_am_info.poll_window           = 80;//128
  Rlc_info_am_config1.rlc.rlc_am_info.tx_window_size        = 512;
  Rlc_info_am_config1.rlc.rlc_am_info.rx_window_size        = 512;
  
  
  Rlc_info_am_config1.rlc.rlc_am_info.max_mrw               = 8;
  
  Rlc_info_am_config1.rlc.rlc_am_info.last_transmission_pdu_poll_trigger   = 1;//boolean
  Rlc_info_am_config1.rlc.rlc_am_info.last_retransmission_pdu_poll_trigger = 1;//boolean
  Rlc_info_am_config1.rlc.rlc_am_info.send_mrw              = 1;//boolean*
	  Mac_rlc_xface->rrc_rlc_config_req(0,ACTION_REMOVE,rlcP->rb_id,RADIO_ACCESS_BEARER,Rlc_info_am_config1);
	  Mac_rlc_xface->rrc_rlc_config_req(0,ACTION_ADD,rlcP->rb_id,RADIO_ACCESS_BEARER,Rlc_info_am_config1);

#endif
        }
        free_mem_block (tb);
      }
    } else {
#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[PDU RX ERROR] FRAME %d RLC-AM %p\n", Mac_rlc_xface->frame, rlcP);
#endif
#ifdef DEBUG_DEMUX
      msg ("[RLC_AM][RB %d][DEMUX] RX PDU WITH ERROR INDICATION\n", rlcP->rb_id);
#endif
      rlcP->stat_rx_error_pdu += 1;
      free_mem_block (tb);
    }
  }

  if ((data_received)) {        //avoid call
    if (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_DATA) {
      if (rlcP->pdu_size <= 126) {
#ifdef DEBUG_DEMUX
	msg("[RLC_AM][RB %d] Calling process_receiver_buffer_7\n",rlcP->rb_id);
#endif
        process_receiver_buffer_7 (rlcP);
      } else {
#ifdef DEBUG_DEMUX
	msg("[RLC_AM][RB %d] Calling process_receiver_buffer_15\n",rlcP->rb_id);
#endif
        process_receiver_buffer_15 (rlcP);
      }
    }
  }
}
