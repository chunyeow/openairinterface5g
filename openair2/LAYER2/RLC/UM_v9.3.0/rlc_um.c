/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

The full GNU General Public License is included in this distribution in
the file called "COPYING".

Contact Information
Openair Admin: openair_admin@eurecom.fr
Openair Tech : openair_tech@eurecom.fr
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
#define RLC_UM_MODULE
#define RLC_UM_C
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif
#include "assertions.h"
#include "rlc_um.h"
#include "list.h"
#include "rlc_primitives.h"
#include "mac_primitives.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"


#include "rlc_um_very_simple_test.h"

//#define DEBUG_RLC_UM_TX_STATUS 1
#define TRACE_RLC_UM_PDU 1

#ifdef TRACE_RLC_UM_PDU
char  message_string[10000];
#endif
//-----------------------------------------------------------------------------
void rlc_um_stat_req     (rlc_um_entity_t *rlcP,
		  unsigned int* stat_tx_pdcp_sdu,
		  unsigned int* stat_tx_pdcp_bytes,
		  unsigned int* stat_tx_pdcp_sdu_discarded,
		  unsigned int* stat_tx_pdcp_bytes_discarded,
		  unsigned int* stat_tx_data_pdu,
		  unsigned int* stat_tx_data_bytes,
		  unsigned int* stat_rx_pdcp_sdu,
		  unsigned int* stat_rx_pdcp_bytes,
		  unsigned int* stat_rx_data_pdus_duplicate,
		  unsigned int* stat_rx_data_bytes_duplicate,
		  unsigned int* stat_rx_data_pdu,
		  unsigned int* stat_rx_data_bytes,
		  unsigned int* stat_rx_data_pdu_dropped,
		  unsigned int* stat_rx_data_bytes_dropped,
		  unsigned int* stat_rx_data_pdu_out_of_window,
		  unsigned int* stat_rx_data_bytes_out_of_window,
		  unsigned int* stat_timer_reordering_timed_out) {
//-----------------------------------------------------------------------------
    *stat_tx_pdcp_sdu                     = rlcP->stat_tx_pdcp_sdu;
    *stat_tx_pdcp_bytes                   = rlcP->stat_tx_pdcp_bytes;
    *stat_tx_pdcp_sdu_discarded           = rlcP->stat_tx_pdcp_sdu_discarded;
    *stat_tx_pdcp_bytes_discarded         = rlcP->stat_tx_pdcp_bytes_discarded;
    *stat_tx_data_pdu                     = rlcP->stat_tx_data_pdu;
    *stat_tx_data_bytes                   = rlcP->stat_tx_data_bytes;
    *stat_rx_pdcp_sdu                     = rlcP->stat_rx_pdcp_sdu;
    *stat_rx_pdcp_bytes                   = rlcP->stat_rx_pdcp_bytes;
    *stat_rx_data_pdus_duplicate          = rlcP->stat_rx_data_pdus_duplicate;
    *stat_rx_data_bytes_duplicate         = rlcP->stat_rx_data_bytes_duplicate;
    *stat_rx_data_pdu                     = rlcP->stat_rx_data_pdu;
    *stat_rx_data_bytes                   = rlcP->stat_rx_data_bytes;
    *stat_rx_data_pdu_dropped             = rlcP->stat_rx_data_pdu_dropped;
    *stat_rx_data_bytes_dropped           = rlcP->stat_rx_data_bytes_dropped;
    *stat_rx_data_pdu_out_of_window       = rlcP->stat_rx_data_pdu_out_of_window;
    *stat_rx_data_bytes_out_of_window     = rlcP->stat_rx_data_bytes_out_of_window;
    *stat_timer_reordering_timed_out      = rlcP->stat_timer_reordering_timed_out;
}
//-----------------------------------------------------------------------------
u32_t
rlc_um_get_buffer_occupancy (rlc_um_entity_t *rlcP)
{
//-----------------------------------------------------------------------------
    if (rlcP->buffer_occupancy > 0) {
        return rlcP->buffer_occupancy;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
void
rlc_um_get_pdus (void *argP,u32_t frame)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t *rlc = (rlc_um_entity_t *) argP;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V9.2.0 p43
        // In the NULL state the RLC entity does not exist and therefore it is
        // not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // establishment, the RLC entity:
        //   - is created; and
        //   - enters the DATA_TRANSFER_READY state.
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        // from 3GPP TS 25.322 V9.2.0 p43-44
        // In the DATA_TRANSFER_READY state, unacknowledged mode data can be
        // exchanged between the entities according to subclause 11.2.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // release, the RLC entity:
        // -enters the NULL state; and
        // -is considered as being terminated.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // modification, the RLC entity:
        // - stays in the DATA_TRANSFER_READY state;
        // - modifies only the protocol parameters and timers as indicated by
        // upper layers.
        // Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC
        // entity:
        // - enters the LOCAL_SUSPEND state.

        // SEND DATA TO MAC
    	  if (rlc->tx_sn_length == 10) {
              rlc_um_segment_10 (rlc,frame);
    	  }
    	  if (rlc->tx_sn_length == 5) {
              rlc_um_segment_5 (rlc,frame);
    	  }
        break;

      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V9.2.0 p44
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does
        // not send UMD PDUs with "Sequence Number" greater than or equal to a
        // certain specified value (see subclause 9.7.5).
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // release, the RLC entity:
        // - enters the NULL state; and
        // - is considered as being terminated.
        // Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
        // - enters the DATA_TRANSFER_READY state; and
        // - resumes the data transmission.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // modification, the RLC entity:
        // - stays in the LOCAL_SUSPEND state;
        // - modifies only the protocol parameters and timers as indicated by
        //   upper layers.

        // TO DO TAKE CARE OF SN : THE IMPLEMENTATION OF THIS FUNCTIONNALITY IS NOT CRITICAL
        break;

      default:
        LOG_E(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_REQ UNKNOWN PROTOCOL STATE %02X hex\n", rlc->module_id, rlc->rb_id, frame, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_rx (void *argP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t    *l_rlc_p = (rlc_um_entity_t *) argP;
#ifdef TRACE_RLC_UM_PDU
  mem_block_t        *tb_p;
  s16_t               tb_size_in_bytes;
  size_t              message_string_size = 0;
#   if defined(ENABLE_ITTI)
  MessageDef         *msg_p;
#   endif
  rlc_um_pdu_info_t   pdu_info;
  int                 octet_index, index;
#endif

  switch (l_rlc_p->protocol_state) {

      case RLC_NULL_STATE:
        // from 3GPP TS 25.322 V9.2.0 p43
        // In the NULL state the RLC entity does not exist and therefore it is
        // not possible to transfer any data through it.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // establishment, the RLC entity:
        //   - is created; and
        //   - enters the DATA_TRANSFER_READY state.
        LOG_N(RLC, "[RLC_UM][MOD %d] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", l_rlc_p->module_id);

        /*if (data_indP.data.nb_elements > 0) {
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_IND %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame, data_indP.data.nb_elements);
            rlc[l_rlc_p->module_id].m_mscgen_trace_length = sprintf(rlc[l_rlc_p->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][MAC_%s][MOD %02d][][--- MAC_DATA_IND/ %d TB(s) ",
                frame,
                (l_rlc_p->is_enb) ? "eNB":"UE",
                l_rlc_p->module_id,
                data_indP.data.nb_elements);

            tb = data_indP.data.head;
            while (tb != NULL) {
                rlc[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length], " SN %d %c%c%c %d Bytes ",
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[1]) +  (((u16_t)((((struct mac_tb_ind *) (tb->data))->data_ptr[0]) & 0x03)) << 8),
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x10) ?  '}':'{',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x08) ?  '{':'}',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x04) ?  'E':'_',
                                                                    ((struct mac_tb_ind *) (tb->data))->size);
                tb = tb->next;
            }
            rlc[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length], " DROPPED RLC NULL STATE ---X][RLC_UM][MOD %02d][RB %02d]\n",
                l_rlc_p->module_id,
                l_rlc_p->rb_id);

            rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length] = 0;
            LOG_D(RLC, "%s", rlc[l_rlc_p->module_id].m_mscgen_trace);
        }*/
        list_free (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        // from 3GPP TS 25.322 V9.2.0 p43-44
        // In the DATA_TRANSFER_READY state, unacknowledged mode data can be
        // exchanged between the entities according to subclause 11.2.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // release, the RLC entity:
        // -enters the NULL state; and
        // -is considered as being terminated.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // modification, the RLC entity:
        // - stays in the DATA_TRANSFER_READY state;
        // - modifies only the protocol parameters and timers as indicated by
        // upper layers.
        // Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC
        // entity:
        // - enters the LOCAL_SUSPEND state.
        data_indP.tb_size = data_indP.tb_size >> 3;

#ifdef TRACE_RLC_UM_PDU
        if (data_indP.data.nb_elements > 0) {
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_IND %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame, data_indP.data.nb_elements);

            tb_p = data_indP.data.head;
            while (tb_p != NULL) {
                tb_size_in_bytes   = ((struct mac_tb_ind *) (tb_p->data))->size;

                rlc_um_get_pdu_infos(frame,(rlc_um_pdu_sn_10_t*) ((struct mac_tb_ind *) (tb_p->data))->data_ptr, tb_size_in_bytes, &pdu_info, l_rlc_p->rx_sn_length);
                  message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", tb_size_in_bytes);
                  message_string_size += sprintf(&message_string[message_string_size], "Header size : %u\n", pdu_info.header_size);
                  message_string_size += sprintf(&message_string[message_string_size], "Payload size: %u\n", pdu_info.payload_size);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC UM DATA IND: UMD PDU\n\n");

                  message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
                  message_string_size += sprintf(&message_string[message_string_size], "  FI        : %u\n", pdu_info.fi);
                  message_string_size += sprintf(&message_string[message_string_size], "  E         : %u\n", pdu_info.e);
                  message_string_size += sprintf(&message_string[message_string_size], "  SN        : %u\n", pdu_info.sn);

                  if (pdu_info.e) {
                      message_string_size += sprintf(&message_string[message_string_size], "\nHeader extension  : \n");
                      for (index=0; index < pdu_info.num_li; index++) {
                          message_string_size += sprintf(&message_string[message_string_size], "  LI        : %u\n", pdu_info.li_list[index]);
                      }
                  }
                  message_string_size += sprintf(&message_string[message_string_size], "\nPayload  : \n");
                  message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
                  message_string_size += sprintf(&message_string[message_string_size], "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n");
                  message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
                  for (octet_index = 0; octet_index < pdu_info.payload_size; octet_index++) {
                      if ((octet_index % 16) == 0){
                          if (octet_index != 0) {
                              message_string_size += sprintf(&message_string[message_string_size], " |\n");
                          }
                          message_string_size += sprintf(&message_string[message_string_size], " %04d |", octet_index);
                      }
                      /*
                       * Print every single octet in hexadecimal form
                       */
                      message_string_size += sprintf(&message_string[message_string_size], " %02x", pdu_info.payload[octet_index]);
                      /*
                       * Align newline and pipes according to the octets in groups of 2
                       */
                  }
                  /*
                   * Append enough spaces and put final pipe
                   */
                  for (index = octet_index; index < 16; ++index) {
                      message_string_size += sprintf(&message_string[message_string_size], "   ");
                  }
                  message_string_size += sprintf(&message_string[message_string_size], " |\n");

#   if defined(ENABLE_ITTI)
                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_UM_DATA_PDU_IND, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_um_data_pdu_ind.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_um_data_pdu_ind.text, message_string, message_string_size);

                  itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->module_id + NB_eNB_INST, msg_p);
# else
                  LOG_T(RLC, "%s", message_string);
# endif

                  tb_p = tb_p->next;
            }
        }
#endif
        rlc_um_receive (l_rlc_p, frame, eNB_flag, data_indP);
        break;

      case RLC_LOCAL_SUSPEND_STATE:
        // from 3GPP TS 25.322 V9.2.0 p44
        // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does
        // not send UMD PDUs with "Sequence Number" greater than or equal to a
        // certain specified value (see subclause 9.7.5).
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // release, the RLC entity:
        // - enters the NULL state; and
        // - is considered as being terminated.
        // Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
        // - enters the DATA_TRANSFER_READY state; and
        // - resumes the data transmission.
        // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
        // modification, the RLC entity:
        // - stays in the LOCAL_SUSPEND state;
        // - modifies only the protocol parameters and timers as indicated by
        //   upper layers.
        LOG_N(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RLC_LOCAL_SUSPEND_STATE\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame);
        /*if (data_indP.data.nb_elements > 0) {
            LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_IND %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame, data_indP.data.nb_elements);
            rlc[l_rlc_p->module_id].m_mscgen_trace_length = sprintf(rlc[l_rlc_p->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][MAC_%s][MOD %02d][][--- MAC_DATA_IND/ %d TB(s) ",
                frame,
                (l_rlc_p->is_enb) ? "eNB":"UE",
                l_rlc_p->module_id,
                data_indP.data.nb_elements);

            tb = data_indP.data.head;
            while (tb != NULL) {
                rlc[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length], " SN %d %c%c%c %d Bytes ",
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[1]) +  (((u16_t)((((struct mac_tb_ind *) (tb->data))->data_ptr[0]) & 0x03)) << 8),
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x10) ?  '}':'{',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x08) ?  '{':'}',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x04) ?  'E':'_',
                                                                    ((struct mac_tb_ind *) (tb->data))->size);
                tb = tb->next;
            }
            rlc[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length], " DROPPED RLC LOCAL SUSPEND STATE ---X][RLC_UM][MOD %02d][RB %02d]\n",
                l_rlc_p->module_id,
                l_rlc_p->rb_id);

            rlc[l_rlc_p->module_id].m_mscgen_trace[rlc[l_rlc_p->module_id].m_mscgen_trace_length] = 0;
            LOG_D(RLC, "%s", rlc[l_rlc_p->module_id].m_mscgen_trace);
        }*/
        list_free (&data_indP.data);
        break;

      default:
        LOG_E(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] TX UNKNOWN PROTOCOL STATE %02X hex\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame, l_rlc_p->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_um_mac_status_indication (void *rlcP, u32_t frame, u8_t eNB_flag, u16_t tbs_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;
  u16_t  sdu_size = 0;
  u16_t  sdu_remaining_size = 0;
  s32_t diff_time=0;
  rlc_um_entity_t   *rlc = NULL;
  
  status_resp.buffer_occupancy_in_pdus         = 0;
  status_resp.buffer_occupancy_in_bytes        = 0;
  status_resp.head_sdu_remaining_size_to_send  = 0;
  status_resp.head_sdu_creation_time           = 0;
  status_resp.head_sdu_is_segmented            = 0;
  status_resp.rlc_info.rlc_protocol_state      = ((rlc_um_entity_t *) rlcP)->protocol_state;

  if (rlcP) {
      rlc = (rlc_um_entity_t *) rlcP;
      rlc_um_check_timer_dar_time_out(rlc,frame,eNB_flag);

      rlc->nb_bytes_requested_by_mac = tbs_sizeP;

      status_resp.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy (rlc);
      if (status_resp.buffer_occupancy_in_bytes > 0) {
          
	  status_resp.buffer_occupancy_in_bytes += rlc->tx_header_min_length_in_bytes;
	  status_resp.buffer_occupancy_in_pdus = rlc->nb_sdu;
	 
	  diff_time =   frame - ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index])->data)->sdu_creation_time;
	  status_resp.head_sdu_creation_time = (diff_time > 0 ) ? (u32_t) diff_time :  (u32_t)(0xffffffff - diff_time + frame) ;
	  //msg("rlc status for frame %d diff time %d resp %d\n", frame, diff_time,status_resp.head_sdu_creation_time) ;
	  
	  sdu_size            = ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index])->data)->sdu_size;
	  sdu_remaining_size  = ((struct rlc_um_tx_sdu_management *) (rlc->input_sdus[rlc->current_sdu_index])->data)->sdu_remaining_size;
	  
	  status_resp.head_sdu_remaining_size_to_send = sdu_remaining_size;	
	  if (sdu_size == sdu_remaining_size)  {
           status_resp.head_sdu_is_segmented = 0;
	  }
	  else {
	   status_resp.head_sdu_is_segmented = 1;
	  }
	
      } else {
      }
      //msg("[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_STATUS_INDICATION BO = %d\n", ((rlc_um_entity_t *) rlcP)->module_id, ((rlc_um_entity_t *) rlcP)->rb_id, status_resp.buffer_occupancy_in_bytes);

      status_resp.rlc_info.rlc_protocol_state = ((rlc_um_entity_t *) rlcP)->protocol_state;
      #ifdef DEBUG_RLC_UM_TX_STATUS
      if ((((rlc_um_entity_t *) rlcP)->rb_id > 0) && (status_resp.buffer_occupancy_in_bytes > 0)) {
          LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_STATUS_INDICATION (DATA) %d bytes -> %d bytes\n", ((rlc_um_entity_t *) rlcP)->module_id, ((rlc_um_entity_t *) rlcP)->rb_id, frame, tbs_sizeP, status_resp.buffer_occupancy_in_bytes);
          if ((tx_statusP.tx_status == MAC_TX_STATUS_SUCCESSFUL) && (tx_statusP.no_pdu)) {
              LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_STATUS_INDICATION  TX STATUS   SUCCESSFUL %d PDUs\n",((rlc_um_entity_t *) rlcP)->module_id, ((rlc_um_entity_t *) rlcP)->rb_id, frame, tx_statusP.no_pdu);
          }
          if ((tx_statusP.tx_status == MAC_TX_STATUS_UNSUCCESSFUL) && (tx_statusP.no_pdu)) {
              LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_STATUS_INDICATION  TX STATUS UNSUCCESSFUL %d PDUs\n",((rlc_um_entity_t *) rlcP)->module_id, ((rlc_um_entity_t *) rlcP)->rb_id, frame, tx_statusP.no_pdu);
          }
      }
      #endif
  } else {
     LOG_E(RLC, "[RLC] RLCp not defined!!!\n");
  }
  return status_resp;
}

//-----------------------------------------------------------------------------
struct mac_data_req
rlc_um_mac_data_request (void *rlcP,u32 frame)
{
    //-----------------------------------------------------------------------------
    struct mac_data_req data_req;
    s16_t               tb_size_in_bytes;
    mem_block_t        *tb_p;
#ifdef TRACE_RLC_UM_PDU
    size_t              message_string_size = 0;
#   if defined(ENABLE_ITTI)
    MessageDef         *msg_p;
#   endif
    rlc_um_pdu_info_t   pdu_info;
    int                 octet_index, index;
#endif
    rlc_um_entity_t *l_rlc_p = (rlc_um_entity_t *) rlcP;

    rlc_um_get_pdus (rlcP,frame);

    list_init (&data_req.data, NULL);
    list_add_list (&l_rlc_p->pdus_to_mac_layer, &data_req.data);


    data_req.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy (l_rlc_p);
    if (data_req.buffer_occupancy_in_bytes > 0) {
        data_req.buffer_occupancy_in_bytes += l_rlc_p->tx_header_min_length_in_bytes;
    }
    data_req.rlc_info.rlc_protocol_state = l_rlc_p->protocol_state;
    if (data_req.data.nb_elements > 0) {
        //LOG_I(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_REQUEST %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frame, data_req.data.nb_elements);

        tb_p = data_req.data.head;
        while (tb_p != NULL) {
            tb_size_in_bytes   = ((struct mac_tb_req *) (tb_p->data))->tb_size;

            LOG_I(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] MAC_DATA_REQUEST  TB SIZE %u\n",
                    l_rlc_p->module_id,
                    l_rlc_p->rb_id,
                    frame,
                    ((struct mac_tb_req *) (tb_p->data))->tb_size);
            l_rlc_p->stat_tx_data_pdu   += 1;
            l_rlc_p->stat_tx_data_bytes += tb_size_in_bytes;

            AssertFatal( tb_size_in_bytes > 0 , "RLC UM PDU LENGTH %d", tb_size_in_bytes);

#ifdef TRACE_RLC_UM_PDU
            rlc_um_get_pdu_infos(frame,(rlc_um_pdu_sn_10_t*) ((struct mac_tb_req *) (tb_p->data))->data_ptr, tb_size_in_bytes, &pdu_info, l_rlc_p->rx_sn_length);
            message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
            message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", tb_size_in_bytes);
            message_string_size += sprintf(&message_string[message_string_size], "Header size : %u\n", pdu_info.header_size);
            message_string_size += sprintf(&message_string[message_string_size], "Payload size: %u\n", pdu_info.payload_size);
            message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC UM DATA IND: UMD PDU\n\n");

            message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
            message_string_size += sprintf(&message_string[message_string_size], "  FI        : %u\n", pdu_info.fi);
            message_string_size += sprintf(&message_string[message_string_size], "  E         : %u\n", pdu_info.e);
            message_string_size += sprintf(&message_string[message_string_size], "  SN        : %u\n", pdu_info.sn);

            if (pdu_info.e) {
                message_string_size += sprintf(&message_string[message_string_size], "\nHeader extension  : \n");
                for (index=0; index < pdu_info.num_li; index++) {
                    message_string_size += sprintf(&message_string[message_string_size], "  LI        : %u\n", pdu_info.li_list[index]);
                }
            }
            message_string_size += sprintf(&message_string[message_string_size], "\nPayload  : \n");
            message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
            message_string_size += sprintf(&message_string[message_string_size], "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n");
            message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
            for (octet_index = 0; octet_index < pdu_info.payload_size; octet_index++) {
                if ((octet_index % 16) == 0){
                    if (octet_index != 0) {
                        message_string_size += sprintf(&message_string[message_string_size], " |\n");
                    }
                    message_string_size += sprintf(&message_string[message_string_size], " %04d |", octet_index);
                }
                /*
                 * Print every single octet in hexadecimal form
                 */
                message_string_size += sprintf(&message_string[message_string_size], " %02x", pdu_info.payload[octet_index]);
                /*
                 * Align newline and pipes according to the octets in groups of 2
                 */
            }
            /*
             * Append enough spaces and put final pipe
             */
            for (index = octet_index; index < 16; ++index) {
                message_string_size += sprintf(&message_string[message_string_size], "   ");
            }
            message_string_size += sprintf(&message_string[message_string_size], " |\n");

#   if defined(ENABLE_ITTI)
            msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_UM_DATA_PDU_REQ, message_string_size + sizeof (IttiMsgText));
            msg_p->ittiMsg.rlc_um_data_pdu_req.size = message_string_size;
            memcpy(&msg_p->ittiMsg.rlc_um_data_pdu_req.text, message_string, message_string_size);

            itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->module_id + NB_eNB_INST, msg_p);
# else
            LOG_T(RLC, "%s", message_string);
# endif
#endif
            tb_p = tb_p->next;
        }
    }
    return data_req;
}

//-----------------------------------------------------------------------------
void
rlc_um_mac_data_indication (void *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
    rlc_um_rx (rlcP, frame, eNB_flag, data_indP);
    rlc_um_check_timer_dar_time_out(rlcP,frame,eNB_flag);
}

//-----------------------------------------------------------------------------
void
rlc_um_data_req (void *rlcP, u32_t frame, mem_block_t *sduP)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t *rlc = (rlc_um_entity_t *) rlcP;

#ifndef USER_MODE
  unsigned long int rlc_um_time_us;
  int min, sec, usec;
#endif

  LOG_I(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RLC_UM_DATA_REQ size %d Bytes, BO %d , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n",
     rlc->module_id,
     rlc->rb_id,
     frame,
	 ((struct rlc_um_data_req *) (sduP->data))->data_size,
	 rlc->buffer_occupancy,
	 rlc->nb_sdu,
      rlc->current_sdu_index,
	 rlc->next_sdu_index);

  /*#ifndef USER_MODE
  rlc_um_time_us = (unsigned long int)(rt_get_time_ns ()/(RTIME)1000);
  sec = (rlc_um_time_us/ 1000000);
  min = (sec / 60) % 60;
  sec = sec % 60;
  usec =  rlc_um_time_us % 1000000;
  msg ("[RLC_UM_LITE][RB  %d] at time %2d:%2d.%6d\n", rlc->rb_id, min, sec , usec);
#endif*/
  if (rlc->input_sdus[rlc->next_sdu_index] == NULL) {
    rlc->input_sdus[rlc->next_sdu_index] = sduP;
    // IMPORTANT : do not change order of affectations
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size = ((struct rlc_um_data_req *) (sduP->data))->data_size;
    rlc->buffer_occupancy += ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;
    rlc->nb_sdu += 1;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->first_byte = (u8_t*)&sduP->data[sizeof (struct rlc_um_data_req_alloc)];
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_remaining_size = ((struct rlc_um_tx_sdu_management *)
                                                                              (sduP->data))->sdu_size;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_segmented_size = 0;
    ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_creation_time = frame;
    rlc->next_sdu_index = (rlc->next_sdu_index + 1) % rlc->size_input_sdus_buffer;

    rlc->stat_tx_pdcp_sdu   += 1;
    rlc->stat_tx_pdcp_bytes += ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;

  } else {
    LOG_W(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d] RLC-UM_DATA_REQ input buffer full SDU garbaged\n",rlc->module_id, rlc->rb_id, frame);
    rlc->stat_tx_pdcp_sdu             += 1;
    rlc->stat_tx_pdcp_bytes           += ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;
    rlc->stat_tx_pdcp_sdu_discarded   += 1;
    rlc->stat_tx_pdcp_bytes_discarded += ((struct rlc_um_tx_sdu_management *) (sduP->data))->sdu_size;
    free_mem_block (sduP);
  }
}
