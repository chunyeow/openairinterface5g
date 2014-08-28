/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

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

#define DEBUG_RLC_UM_TX_STATUS 1
// NN --> LG: this could become a perf bottleneck, I added this to the makefile.
//#define TRACE_RLC_UM_PDU 1

#ifdef TRACE_RLC_UM_PDU
char  message_string[10000];
#endif
//-----------------------------------------------------------------------------
void rlc_um_stat_req     (rlc_um_entity_t *rlc_pP,
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
    *stat_tx_pdcp_sdu                     = rlc_pP->stat_tx_pdcp_sdu;
    *stat_tx_pdcp_bytes                   = rlc_pP->stat_tx_pdcp_bytes;
    *stat_tx_pdcp_sdu_discarded           = rlc_pP->stat_tx_pdcp_sdu_discarded;
    *stat_tx_pdcp_bytes_discarded         = rlc_pP->stat_tx_pdcp_bytes_discarded;
    *stat_tx_data_pdu                     = rlc_pP->stat_tx_data_pdu;
    *stat_tx_data_bytes                   = rlc_pP->stat_tx_data_bytes;
    *stat_rx_pdcp_sdu                     = rlc_pP->stat_rx_pdcp_sdu;
    *stat_rx_pdcp_bytes                   = rlc_pP->stat_rx_pdcp_bytes;
    *stat_rx_data_pdus_duplicate          = rlc_pP->stat_rx_data_pdus_duplicate;
    *stat_rx_data_bytes_duplicate         = rlc_pP->stat_rx_data_bytes_duplicate;
    *stat_rx_data_pdu                     = rlc_pP->stat_rx_data_pdu;
    *stat_rx_data_bytes                   = rlc_pP->stat_rx_data_bytes;
    *stat_rx_data_pdu_dropped             = rlc_pP->stat_rx_data_pdu_dropped;
    *stat_rx_data_bytes_dropped           = rlc_pP->stat_rx_data_bytes_dropped;
    *stat_rx_data_pdu_out_of_window       = rlc_pP->stat_rx_data_pdu_out_of_window;
    *stat_rx_data_bytes_out_of_window     = rlc_pP->stat_rx_data_bytes_out_of_window;
    *stat_timer_reordering_timed_out      = rlc_pP->stat_timer_reordering_timed_out;
}
//-----------------------------------------------------------------------------
uint32_t
rlc_um_get_buffer_occupancy (rlc_um_entity_t *rlc_pP)
{
//-----------------------------------------------------------------------------
    if (rlc_pP->buffer_occupancy > 0) {
        return rlc_pP->buffer_occupancy;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
void
rlc_um_get_pdus (void *argP, frame_t frameP)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t *rlc_p = (rlc_um_entity_t *) argP;

  switch (rlc_p->protocol_state) {

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
    	  if (rlc_p->tx_sn_length == 10) {
              rlc_um_segment_10 (rlc_p,frameP);
    	  }
    	  if (rlc_p->tx_sn_length == 5) {
              rlc_um_segment_5 (rlc_p,frameP);
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
        LOG_E(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_DATA_REQ UNKNOWN PROTOCOL STATE %02X hex\n",
                frameP,
                (rlc_p->is_enb) ? "eNB" : "UE",
                rlc_p->enb_module_id,
                rlc_p->ue_module_id,
                rlc_p->rb_id,
                rlc_p->protocol_state);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_rx (void *argP, frame_t frameP, eNB_flag_t eNB_flagP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t    *l_rlc_p = (rlc_um_entity_t *) argP;
#ifdef TRACE_RLC_UM_PDU
  mem_block_t        *tb_p;
  int16_t               tb_size_in_bytes;
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
        LOG_N(RLC, "[RLC_UM][MOD %02u/%02u] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", l_rlc_p->enb_module_id, l_rlc_p->ue_module_id);

        /*if (data_indP.data.nb_elements > 0) {
            LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_DATA_IND %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frameP, data_indP.data.nb_elements);
            rlc_p[l_rlc_p->module_id].m_mscgen_trace_length = sprintf(rlc_p[l_rlc_p->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][MAC_%s][MOD %02d][][--- MAC_DATA_IND/ %d TB(s) ",
                frameP,
                (l_rlc_p->is_enb) ? "eNB":"UE",
                l_rlc_p->module_id,
                data_indP.data.nb_elements);

            tb = data_indP.data.head;
            while (tb != NULL) {
                rlc_p[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length], " SN %d %c%c%c %d Bytes ",
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[1]) +  (((uint16_t)((((struct mac_tb_ind *) (tb->data))->data_ptr[0]) & 0x03)) << 8),
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x10) ?  '}':'{',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x08) ?  '{':'}',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x04) ?  'E':'_',
                                                                    ((struct mac_tb_ind *) (tb->data))->size);
                tb = tb->next;
            }
            rlc_p[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length], " DROPPED RLC NULL STATE ---X][RLC_UM][MOD %02d][RB %02d]\n",
                l_rlc_p->module_id,
                l_rlc_p->rb_id);

            rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length] = 0;
            LOG_D(RLC, "%s", rlc_p[l_rlc_p->module_id].m_mscgen_trace);
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
            LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_DATA_IND %d TBs\n",
                    frameP,
                    (l_rlc_p->is_enb) ? "eNB" : "UE",
                    l_rlc_p->enb_module_id,
                    l_rlc_p->ue_module_id,
                    l_rlc_p->rb_id,
                    data_indP.data.nb_elements);

            tb_p = data_indP.data.head;
            while (tb_p != NULL) {
                tb_size_in_bytes   = ((struct mac_tb_ind *) (tb_p->data))->size;

                rlc_um_get_pdu_infos(frameP,(rlc_um_pdu_sn_10_t*) ((struct mac_tb_ind *) (tb_p->data))->data_ptr, tb_size_in_bytes, &pdu_info, l_rlc_p->rx_sn_length);
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
                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb ? TASK_RLC_ENB:TASK_RLC_UE , RLC_UM_DATA_PDU_IND, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_um_data_pdu_ind.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_um_data_pdu_ind.text, message_string, message_string_size);

                  if (l_rlc_p->is_enb) {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
                  } else {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
                  }
# else
                  LOG_T(RLC, "%s", message_string);
# endif

                  tb_p = tb_p->next;
            }
        }
#endif
        rlc_um_receive (l_rlc_p, frameP, eNB_flagP, data_indP);
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
        LOG_N(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] RLC_LOCAL_SUSPEND_STATE\n",
                frameP,
                (l_rlc_p->is_enb) ? "eNB" : "UE",
                l_rlc_p->enb_module_id,
                l_rlc_p->ue_module_id,
                l_rlc_p->rb_id);
        /*if (data_indP.data.nb_elements > 0) {
            LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_DATA_IND %d TBs\n", l_rlc_p->module_id, l_rlc_p->rb_id, frameP, data_indP.data.nb_elements);
            rlc_p[l_rlc_p->module_id].m_mscgen_trace_length = sprintf(rlc_p[l_rlc_p->module_id].m_mscgen_trace, "[MSC_MSG][FRAME %05d][MAC_%s][MOD %02d][][--- MAC_DATA_IND/ %d TB(s) ",
                frameP,
                (l_rlc_p->is_enb) ? "eNB":"UE",
                l_rlc_p->module_id,
                data_indP.data.nb_elements);

            tb = data_indP.data.head;
            while (tb != NULL) {
                rlc_p[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length], " SN %d %c%c%c %d Bytes ",
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[1]) +  (((uint16_t)((((struct mac_tb_ind *) (tb->data))->data_ptr[0]) & 0x03)) << 8),
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x10) ?  '}':'{',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x08) ?  '{':'}',
                                                                    (((struct mac_tb_ind *) (tb->data))->data_ptr[0] & 0x04) ?  'E':'_',
                                                                    ((struct mac_tb_ind *) (tb->data))->size);
                tb = tb->next;
            }
            rlc_p[l_rlc_p->module_id].m_mscgen_trace_length += sprintf(&rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length], " DROPPED RLC LOCAL SUSPEND STATE ---X][RLC_UM][MOD %02d][RB %02d]\n",
                l_rlc_p->module_id,
                l_rlc_p->rb_id);

            rlc_p[l_rlc_p->module_id].m_mscgen_trace[rlc_p[l_rlc_p->module_id].m_mscgen_trace_length] = 0;
            LOG_D(RLC, "%s", rlc_p[l_rlc_p->module_id].m_mscgen_trace);
        }*/
        list_free (&data_indP.data);
        break;

      default:
        LOG_E(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] TX UNKNOWN PROTOCOL STATE %02X hex\n",
                frameP,
                (l_rlc_p->is_enb) ? "eNB" : "UE",
                l_rlc_p->enb_module_id,
                l_rlc_p->ue_module_id,
                l_rlc_p->rb_id,
                l_rlc_p->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_um_mac_status_indication (void *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, uint16_t tbs_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp status_resp;
  uint16_t  sdu_size = 0;
  uint16_t  sdu_remaining_size = 0;
  int32_t diff_time=0;
  rlc_um_entity_t   *rlc_p = NULL;

  status_resp.buffer_occupancy_in_pdus         = 0;
  status_resp.buffer_occupancy_in_bytes        = 0;
  status_resp.head_sdu_remaining_size_to_send  = 0;
  status_resp.head_sdu_creation_time           = 0;
  status_resp.head_sdu_is_segmented            = 0;
  status_resp.rlc_info.rlc_protocol_state      = ((rlc_um_entity_t *) rlc_pP)->protocol_state;

  if (rlc_pP) {
      rlc_p = (rlc_um_entity_t *) rlc_pP;
      rlc_um_check_timer_dar_time_out(rlc_p,frameP,eNB_flagP);

      rlc_p->nb_bytes_requested_by_mac = tbs_sizeP;

      status_resp.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy (rlc_p);
      if (status_resp.buffer_occupancy_in_bytes > 0) {

	  status_resp.buffer_occupancy_in_bytes += rlc_p->tx_header_min_length_in_bytes;
	  status_resp.buffer_occupancy_in_pdus = rlc_p->nb_sdu;

	  diff_time =   frameP - ((struct rlc_um_tx_sdu_management *) (rlc_p->input_sdus[rlc_p->current_sdu_index])->data)->sdu_creation_time;
	  status_resp.head_sdu_creation_time = (diff_time > 0 ) ? (uint32_t) diff_time :  (uint32_t)(0xffffffff - diff_time + frameP) ;
	  //msg("rlc_p status for frameP %d diff time %d resp %d\n", frameP, diff_time,status_resp.head_sdu_creation_time) ;

	  sdu_size            = ((struct rlc_um_tx_sdu_management *) (rlc_p->input_sdus[rlc_p->current_sdu_index])->data)->sdu_size;
	  sdu_remaining_size  = ((struct rlc_um_tx_sdu_management *) (rlc_p->input_sdus[rlc_p->current_sdu_index])->data)->sdu_remaining_size;

	  status_resp.head_sdu_remaining_size_to_send = sdu_remaining_size;
	  if (sdu_size == sdu_remaining_size)  {
           status_resp.head_sdu_is_segmented = 0;
	  }
	  else {
	   status_resp.head_sdu_is_segmented = 1;
	  }

      } else {
      }
      //msg("[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_STATUS_INDICATION BO = %d\n", ((rlc_um_entity_t *) rlc_pP)->module_id, ((rlc_um_entity_t *) rlc_pP)->rb_id, status_resp.buffer_occupancy_in_bytes);

      status_resp.rlc_info.rlc_protocol_state = ((rlc_um_entity_t *) rlc_pP)->protocol_state;
      #ifdef DEBUG_RLC_UM_TX_STATUS
      if ((((rlc_um_entity_t *) rlc_pP)->rb_id > 0) && (status_resp.buffer_occupancy_in_bytes > 0)) {
          LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_STATUS_INDICATION (DATA) %d bytes -> %d bytes\n",
                  frameP,
                  (rlc_p->is_enb) ? "eNB" : "UE",
                  rlc_p->enb_module_id,
                  rlc_p->ue_module_id,
                  rlc_p->rb_id,
                  tbs_sizeP,
                  status_resp.buffer_occupancy_in_bytes);

          if ((tx_statusP.tx_status == MAC_TX_STATUS_SUCCESSFUL) && (tx_statusP.no_pdu)) {
              LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_STATUS_INDICATION  TX STATUS   SUCCESSFUL %d PDUs\n",
                      frameP,
                      (rlc_p->is_enb) ? "eNB" : "UE",
                      rlc_p->enb_module_id,
                      rlc_p->ue_module_id,
                      rlc_p->rb_id,
                      tx_statusP.no_pdu);
          }
          if ((tx_statusP.tx_status == MAC_TX_STATUS_UNSUCCESSFUL) && (tx_statusP.no_pdu)) {
              LOG_D(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_STATUS_INDICATION  TX STATUS UNSUCCESSFUL %d PDUs\n",
                      frameP,
                      (rlc_p->is_enb) ? "eNB" : "UE",
                      rlc_p->enb_module_id,
                      rlc_p->ue_module_id,
                      rlc_p->rb_id,
                      tx_statusP.no_pdu);
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
rlc_um_mac_data_request (void *rlc_pP,frame_t frameP)
{
    //-----------------------------------------------------------------------------
    struct mac_data_req data_req;
    int16_t               tb_size_in_bytes;
    mem_block_t        *tb_p;
#ifdef TRACE_RLC_UM_PDU
    size_t              message_string_size = 0;
#   if defined(ENABLE_ITTI)
    MessageDef         *msg_p;
#   endif
    rlc_um_pdu_info_t   pdu_info;
    int                 octet_index, index;
#endif
    rlc_um_entity_t *l_rlc_p = (rlc_um_entity_t *) rlc_pP;

    rlc_um_get_pdus (rlc_pP,frameP);

    list_init (&data_req.data, NULL);
    list_add_list (&l_rlc_p->pdus_to_mac_layer, &data_req.data);


    data_req.buffer_occupancy_in_bytes = rlc_um_get_buffer_occupancy (l_rlc_p);
    if (data_req.buffer_occupancy_in_bytes > 0) {
        data_req.buffer_occupancy_in_bytes += l_rlc_p->tx_header_min_length_in_bytes;
    }
    data_req.rlc_info.rlc_protocol_state = l_rlc_p->protocol_state;
    if (data_req.data.nb_elements > 0) {
        tb_p = data_req.data.head;
        while (tb_p != NULL) {
            tb_size_in_bytes   = ((struct mac_tb_req *) (tb_p->data))->tb_size;

            LOG_I(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] MAC_DATA_REQUEST  TB SIZE %u\n",
                    frameP,
                    (l_rlc_p->is_enb) ? "eNB" : "UE",
                    l_rlc_p->enb_module_id,
                    l_rlc_p->ue_module_id,
                    l_rlc_p->rb_id,
                    ((struct mac_tb_req *) (tb_p->data))->tb_size);
            l_rlc_p->stat_tx_data_pdu   += 1;
            l_rlc_p->stat_tx_data_bytes += tb_size_in_bytes;

            AssertFatal( tb_size_in_bytes > 0 , "RLC UM PDU LENGTH %d", tb_size_in_bytes);

#ifdef TRACE_RLC_UM_PDU
            rlc_um_get_pdu_infos(frameP,(rlc_um_pdu_sn_10_t*) ((struct mac_tb_req *) (tb_p->data))->data_ptr, tb_size_in_bytes, &pdu_info, l_rlc_p->rx_sn_length);
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

            if (l_rlc_p->is_enb) {
                itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
            } else {
                itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
            }
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
rlc_um_mac_data_indication (void *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
    rlc_um_rx (rlc_pP, frameP, eNB_flagP, data_indP);
    rlc_um_check_timer_dar_time_out(rlc_pP,frameP,eNB_flagP);
}

//-----------------------------------------------------------------------------
void
rlc_um_data_req (void *rlc_pP, frame_t frameP, mem_block_t *sdu_pP)
{
//-----------------------------------------------------------------------------
  rlc_um_entity_t *rlc_p = (rlc_um_entity_t *) rlc_pP;

#ifndef USER_MODE
  unsigned long int rlc_um_time_us;
  int min, sec, usec;
#endif
#if defined(ENABLE_ITTI)
  char                 message_string[7000];
  size_t               message_string_size = 0;
  MessageDef          *msg_p;
  int                  octet_index, index;
  uint16_t             data_offset;
  uint16_t             data_size;
#endif

  LOG_I(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] RLC_UM_DATA_REQ size %d Bytes, BO %d , NB SDU %d current_sdu_index=%d next_sdu_index=%d\n",
     frameP,
     (rlc_p->is_enb) ? "eNB" : "UE",
     rlc_p->enb_module_id,
     rlc_p->ue_module_id,
     rlc_p->rb_id,
     ((struct rlc_um_data_req *) (sdu_pP->data))->data_size,
     rlc_p->buffer_occupancy,
     rlc_p->nb_sdu,
     rlc_p->current_sdu_index,
     rlc_p->next_sdu_index);
  rlc_util_print_hex_octets(
      RLC,
      (uint8_t*)&sdu_pP->data[sizeof (struct rlc_um_data_req_alloc)],
      ((struct rlc_um_data_req *) (sdu_pP->data))->data_size);

  /*#ifndef USER_MODE
  rlc_um_time_us = (unsigned long int)(rt_get_time_ns ()/(RTIME)1000);
  sec = (rlc_um_time_us/ 1000000);
  min = (sec / 60) % 60;
  sec = sec % 60;
  usec =  rlc_um_time_us % 1000000;
  msg ("[RLC_UM_LITE][RB  %d] at time %2d:%2d.%6d\n", rlc_p->rb_id, min, sec , usec);
#endif*/
  if (rlc_p->input_sdus[rlc_p->next_sdu_index] == NULL) {
    rlc_p->input_sdus[rlc_p->next_sdu_index] = sdu_pP;
    // IMPORTANT : do not change order of affectations
    ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size = ((struct rlc_um_data_req *) (sdu_pP->data))->data_size;
    rlc_p->buffer_occupancy += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
    rlc_p->nb_sdu += 1;
    ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->first_byte = (uint8_t*)&sdu_pP->data[sizeof (struct rlc_um_data_req_alloc)];
    ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_remaining_size = ((struct rlc_um_tx_sdu_management *)
                                                                              (sdu_pP->data))->sdu_size;
    ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_segmented_size = 0;
    ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_creation_time = frameP;
    rlc_p->next_sdu_index = (rlc_p->next_sdu_index + 1) % rlc_p->size_input_sdus_buffer;

    rlc_p->stat_tx_pdcp_sdu   += 1;
    rlc_p->stat_tx_pdcp_bytes += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
#   if defined(TRACE_RLC_UM_PDU)
      data_offset = sizeof (struct rlc_um_data_req_alloc);
      data_size   = ((struct rlc_um_tx_sdu_management *)(sdu_pP->data))->sdu_size;
      message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", rlc_p->rb_id);
      message_string_size += sprintf(&message_string[message_string_size], "SDU size    : %u\n", data_size);

      message_string_size += sprintf(&message_string[message_string_size], "\nPayload  : \n");
      message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
      message_string_size += sprintf(&message_string[message_string_size], "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n");
      message_string_size += sprintf(&message_string[message_string_size], "------+-------------------------------------------------|\n");
      for (octet_index = 0; octet_index < data_size; octet_index++) {
          if ((octet_index % 16) == 0){
              if (octet_index != 0) {
                  message_string_size += sprintf(&message_string[message_string_size], " |\n");
              }
              message_string_size += sprintf(&message_string[message_string_size], " %04d |", octet_index);
          }
          /*
           * Print every single octet in hexadecimal form
           */
          message_string_size += sprintf(&message_string[message_string_size], " %02x", (uint8_t*)(&sdu_pP->data[data_offset])[octet_index]);
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
      msg_p = itti_alloc_new_message_sized (rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_UM_SDU_REQ, message_string_size + sizeof (IttiMsgText));
      msg_p->ittiMsg.rlc_um_sdu_req.size = message_string_size;
      memcpy(&msg_p->ittiMsg.rlc_um_sdu_req.text, message_string, message_string_size);

      if (rlc_p->is_enb) {
          itti_send_msg_to_task(TASK_UNKNOWN, rlc_p->enb_module_id, msg_p);
      } else {
          itti_send_msg_to_task(TASK_UNKNOWN, rlc_p->ue_module_id + NB_eNB_INST, msg_p);
      }
#else 
      LOG_T(RLC, "%s", message_string);
#endif 
#   endif

  } else {
    LOG_W(RLC, "[FRAME %05d][%s][RLC_UM][MOD %02u/%02u][RB %02d] RLC-UM_DATA_REQ input buffer full SDU garbaged\n",
          frameP,
          (rlc_p->is_enb) ? "eNB" : "UE",
          rlc_p->enb_module_id,
          rlc_p->ue_module_id,
          rlc_p->rb_id);
    rlc_p->stat_tx_pdcp_sdu             += 1;
    rlc_p->stat_tx_pdcp_bytes           += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
    rlc_p->stat_tx_pdcp_sdu_discarded   += 1;
    rlc_p->stat_tx_pdcp_bytes_discarded += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
    free_mem_block (sdu_pP);
#if defined(STOP_ON_IP_TRAFFIC_OVERLOAD)
      AssertFatal(0, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] RLC_UM_DATA_REQ, SDU DROPPED, INPUT BUFFER OVERFLOW\n",
          frameP,
          (rlc_p->is_enb) ? "eNB" : "UE",
          rlc_p->enb_module_id,
          rlc_p->ue_module_id,
          rlc_p->rb_id);
#endif
  }
}
