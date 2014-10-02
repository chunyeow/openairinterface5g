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
#define RLC_AM_MODULE
#define RLC_AM_C
//-----------------------------------------------------------------------------
#include "platform_types.h"
#include "platform_constants.h"
//-----------------------------------------------------------------------------
#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif
#include "assertions.h"
#include "hashtable.h"
#include "rlc_am.h"
#include "rlc_am_segment.h"
#include "rlc_am_timer_poll_retransmit.h"
#include "mac_primitives.h"
#include "rlc_primitives.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#include "UL-AM-RLC.h"
#include "DL-AM-RLC.h"
//#define TRACE_RLC_AM_DATA_REQUEST
//#define TRACE_RLC_AM_TX_STATUS
//#define TRACE_RLC_AM_TX
//#define TRACE_RLC_AM_RX
//#define TRACE_RLC_AM_BO

extern rlc_am_control_pdu_info_t  g_rlc_am_control_pdu_info;

//-----------------------------------------------------------------------------
uint32_t
rlc_am_get_buffer_occupancy_in_bytes (rlc_am_entity_t *rlc_pP,frame_t frameP)
{
//-----------------------------------------------------------------------------
  uint32_t max_li_overhead;
  uint32_t header_overhead;

  // priority of control trafic
  if (rlc_pP->status_requested) {
      if (rlc_pP->t_status_prohibit.running == 0) {
#ifdef TRACE_RLC_AM_BO
          if (((15  +  rlc_pP->num_nack_sn*(10+1)  +  rlc_pP->num_nack_so*(15+15+1) + 7) >> 3) > 0) {
              LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] BO : CONTROL PDU %d bytes \n", frameP, rlc_pP->module_id, rlc_pP->rb_id, ((15  +  rlc_pP->num_nack_sn*(10+1)  +  rlc_pP->num_nack_so*(15+15+1) + 7) >> 3));
          }
#endif
          return ((15  +  rlc_pP->num_nack_sn*(10+1)  +  rlc_pP->num_nack_so*(15+15+1) + 7) >> 3);
      }
  }

  // data traffic
  if (rlc_pP->nb_sdu_no_segmented <= 1) {
      max_li_overhead = 0;
  } else {
      max_li_overhead = (((rlc_pP->nb_sdu_no_segmented - 1) * 3) / 2) + ((rlc_pP->nb_sdu_no_segmented - 1) % 2);
  }
  if (rlc_pP->sdu_buffer_occupancy == 0) {
      header_overhead = 0;
  } else {
      header_overhead = 2;
  }


#ifdef TRACE_RLC_AM_BO
  if ((rlc_pP->status_buffer_occupancy + rlc_pP->retransmission_buffer_occupancy + rlc_pP->sdu_buffer_occupancy + max_li_overhead + header_overhead) > 0) {
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] BO : STATUS  BUFFER %d bytes \n", frameP, rlc_pP->module_id, rlc_pP->rb_id, rlc_pP->status_buffer_occupancy);
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] BO : RETRANS BUFFER %d bytes \n", frameP, rlc_pP->module_id,rlc_pP->rb_id, rlc_pP->retransmission_buffer_occupancy);
    LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] BO : SDU     BUFFER %d bytes + li_overhead %d bytes header_overhead %d bytes (nb sdu not segmented %d)\n", frameP, rlc_pP->module_id,rlc_pP->rb_id, rlc_pP->sdu_buffer_occupancy, max_li_overhead, header_overhead, rlc_pP->nb_sdu_no_segmented);
  }
#endif
  return rlc_pP->status_buffer_occupancy + rlc_pP->retransmission_buffer_occupancy + rlc_pP->sdu_buffer_occupancy + max_li_overhead + header_overhead;
}
//-----------------------------------------------------------------------------
void rlc_am_release (rlc_am_entity_t *rlc_pP)
//-----------------------------------------------------------------------------
{

}
//-----------------------------------------------------------------------------
void config_req_rlc_am (
    module_id_t     enb_module_idP,
    module_id_t     ue_module_idP,
    frame_t         frameP,
    eNB_flag_t      eNB_flagP,
    srb_flag_t      srb_flagP,
    rlc_am_info_t  *config_am_pP,
    rb_id_t         rb_idP)
{
//-----------------------------------------------------------------------------
    rlc_union_t       *rlc_union_p = NULL;
    rlc_am_entity_t *l_rlc_p         = NULL;
    hash_key_t       key           = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
    hashtable_rc_t   h_rc;

    h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
    if (h_rc == HASH_TABLE_OK) {
        l_rlc_p = &rlc_union_p->rlc.am;
        LOG_D(RLC,
            "[FRAME %5u][%s][RRC][MOD %u/%u][][--- CONFIG_REQ (max_retx_threshold=%d poll_pdu=%d poll_byte=%d t_poll_retransmit=%d t_reord=%d t_status_prohibit=%d) --->][RLC_AM][MOD %u/%u][RB %u]\n",
            frameP,
            ( eNB_flagP > 0) ? "eNB":"UE",
            enb_module_idP,
            ue_module_idP,
            config_am_pP->max_retx_threshold,
            config_am_pP->poll_pdu,
            config_am_pP->poll_byte,
            config_am_pP->t_poll_retransmit,
            config_am_pP->t_reordering,
            config_am_pP->t_status_prohibit,
            enb_module_idP,
            ue_module_idP,
            rb_idP);
        rlc_am_init(l_rlc_p,frameP);
        rlc_am_set_debug_infos(l_rlc_p, frameP, eNB_flagP, enb_module_idP, ue_module_idP, rb_idP, srb_flagP);
        rlc_am_configure(l_rlc_p,frameP,
               config_am_pP->max_retx_threshold,
               config_am_pP->poll_pdu,
               config_am_pP->poll_byte,
               config_am_pP->t_poll_retransmit,
               config_am_pP->t_reordering,
               config_am_pP->t_status_prohibit);
    } else {
        LOG_E(RLC, "[FRAME %5u][%s][RRC][MOD %u/%u][][--- CONFIG_REQ  --->][RLC_AM][MOD %u/%u][RB %u] RLC NOT FOUND\n",
            frameP,
            (eNB_flagP) ? "eNB" : "UE",
            enb_module_idP,
            ue_module_idP,
            enb_module_idP,
            ue_module_idP,
            rb_idP);
    }
}
uint32_t pollPDU_tab[PollPDU_pInfinity+1]={4,8,16,32,64,128,256,1024};  // What is PollPDU_pInfinity??? 1024 for now
uint32_t maxRetxThreshold_tab[UL_AM_RLC__maxRetxThreshold_t32+1]={1,2,3,4,6,8,16,32};
uint32_t pollByte_tab[PollByte_spare1]={25,50,75,100,125,250,375,500,750,1000,1250,1500,2000,3000,10000};  // What is PollByte_kBinfinity??? 10000 for now
uint32_t PollRetransmit_tab[T_PollRetransmit_spare9]={5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250,300,350,400,450,500};
uint32_t am_t_Reordering_tab[T_Reordering_spare1]={0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,110,120,130,140,150,160,170,180,190,200};
uint32_t t_StatusProhibit_tab[T_StatusProhibit_spare8]={0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250,300,350,400,450,500};

//-----------------------------------------------------------------------------
void config_req_rlc_am_asn1 (
    const module_id_t             enb_module_idP,
    const module_id_t             ue_module_idP,
    const frame_t                 frameP,
    const eNB_flag_t              eNB_flagP,
    const srb_flag_t              srb_flagP,
    const struct RLC_Config__am  * const config_am_pP,
    const rb_id_t                 rb_idP)
{
//-----------------------------------------------------------------------------
  rlc_union_t     *rlc_union_p   = NULL;
  rlc_am_entity_t *l_rlc_p         = NULL;
  hash_key_t       key           = RLC_COLL_KEY_VALUE(enb_module_idP, ue_module_idP, eNB_flagP, rb_idP, srb_flagP);
  hashtable_rc_t   h_rc;

  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);
  if (h_rc == HASH_TABLE_OK) {
      l_rlc_p = &rlc_union_p->rlc.am;
      if ((config_am_pP->ul_AM_RLC.maxRetxThreshold <= UL_AM_RLC__maxRetxThreshold_t32) &&
          (config_am_pP->ul_AM_RLC.pollPDU<=PollPDU_pInfinity) &&
          (config_am_pP->ul_AM_RLC.pollByte<PollByte_spare1) &&
          (config_am_pP->ul_AM_RLC.t_PollRetransmit<T_PollRetransmit_spare9) &&
          (config_am_pP->dl_AM_RLC.t_Reordering<T_Reordering_spare1) &&
          (config_am_pP->dl_AM_RLC.t_StatusProhibit<T_StatusProhibit_spare8) ){

          LOG_D(RLC, "[FRAME %5u][%s][RRC][MOD %u/%u][][--- CONFIG_REQ (max_retx_threshold=%d poll_pdu=%d poll_byte=%d t_poll_retransmit=%d t_reord=%d t_status_prohibit=%d) --->][RLC_AM][MOD %u/%u][RB %u]\n",
              frameP,
              (eNB_flagP) ? "eNB" : "UE",
              enb_module_idP,
              ue_module_idP,
              maxRetxThreshold_tab[config_am_pP->ul_AM_RLC.maxRetxThreshold],
              pollPDU_tab[config_am_pP->ul_AM_RLC.pollPDU],
              pollByte_tab[config_am_pP->ul_AM_RLC.pollByte],
              PollRetransmit_tab[config_am_pP->ul_AM_RLC.t_PollRetransmit],
              am_t_Reordering_tab[config_am_pP->dl_AM_RLC.t_Reordering],
              t_StatusProhibit_tab[config_am_pP->dl_AM_RLC.t_StatusProhibit],
              enb_module_idP,
              ue_module_idP,
              rb_idP);

          rlc_am_init(l_rlc_p,frameP);
          rlc_am_set_debug_infos(l_rlc_p, frameP, eNB_flagP, srb_flagP, enb_module_idP, ue_module_idP, rb_idP);
          rlc_am_configure(l_rlc_p,
              frameP,
              maxRetxThreshold_tab[config_am_pP->ul_AM_RLC.maxRetxThreshold],
              pollPDU_tab[config_am_pP->ul_AM_RLC.pollPDU],
              pollByte_tab[config_am_pP->ul_AM_RLC.pollByte],
              PollRetransmit_tab[config_am_pP->ul_AM_RLC.t_PollRetransmit],
              am_t_Reordering_tab[config_am_pP->dl_AM_RLC.t_Reordering],
              t_StatusProhibit_tab[config_am_pP->dl_AM_RLC.t_StatusProhibit]);
      } else {
          LOG_D(RLC,
              "[FRAME %5u][%s][RRC][MOD %u/%u][][--- ILLEGAL CONFIG_REQ (max_retx_threshold=%d poll_pdu=%d poll_byte=%d t_poll_retransmit=%d t_reord=%d t_status_prohibit=%d) --->][RLC_AM][MOD %u/%u][RB %u], RLC-AM NOT CONFIGURED\n",
              frameP,
              (eNB_flagP) ? "eNB" : "UE",
              enb_module_idP,
              ue_module_idP,
              config_am_pP->ul_AM_RLC.maxRetxThreshold,
              config_am_pP->ul_AM_RLC.pollPDU,
              config_am_pP->ul_AM_RLC.pollByte,
              config_am_pP->ul_AM_RLC.t_PollRetransmit,
              config_am_pP->dl_AM_RLC.t_Reordering,
              config_am_pP->dl_AM_RLC.t_StatusProhibit,
              enb_module_idP,
              ue_module_idP,
              rb_idP);
      }
  } else {
      LOG_E(RLC, "[FRAME %5u][%s][RRC][MOD %u/%u][][--- CONFIG_REQ  --->][RLC_AM][MOD %u/%u][RB %u] RLC NOT FOUND\n",
          frameP,
          (eNB_flagP) ? "eNB" : "UE",
          enb_module_idP,
          ue_module_idP,
          enb_module_idP,
          ue_module_idP,
          rb_idP);
  }
}

  //-----------------------------------------------------------------------------
void rlc_am_stat_req     (rlc_am_entity_t *rlc_pP,
                              unsigned int* stat_tx_pdcp_sdu,
                              unsigned int* stat_tx_pdcp_bytes,
                              unsigned int* stat_tx_pdcp_sdu_discarded,
                              unsigned int* stat_tx_pdcp_bytes_discarded,
                              unsigned int* stat_tx_data_pdu,
                              unsigned int* stat_tx_data_bytes,
                              unsigned int* stat_tx_retransmit_pdu_by_status,
                              unsigned int* stat_tx_retransmit_bytes_by_status,
                              unsigned int* stat_tx_retransmit_pdu,
                              unsigned int* stat_tx_retransmit_bytes,
                              unsigned int* stat_tx_control_pdu,
                              unsigned int* stat_tx_control_bytes,
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
                              unsigned int* stat_rx_control_pdu,
                              unsigned int* stat_rx_control_bytes,
                              unsigned int* stat_timer_reordering_timed_out,
                              unsigned int* stat_timer_poll_retransmit_timed_out,
                              unsigned int* stat_timer_status_prohibit_timed_out)
//-----------------------------------------------------------------------------
{
    *stat_tx_pdcp_sdu                     = rlc_pP->stat_tx_pdcp_sdu;
    *stat_tx_pdcp_bytes                   = rlc_pP->stat_tx_pdcp_bytes;
    *stat_tx_pdcp_sdu_discarded           = rlc_pP->stat_tx_pdcp_sdu_discarded;
    *stat_tx_pdcp_bytes_discarded         = rlc_pP->stat_tx_pdcp_bytes_discarded;
    *stat_tx_data_pdu                     = rlc_pP->stat_tx_data_pdu;
    *stat_tx_data_bytes                   = rlc_pP->stat_tx_data_bytes;
    *stat_tx_retransmit_pdu_by_status     = rlc_pP->stat_tx_retransmit_pdu_by_status;
    *stat_tx_retransmit_bytes_by_status   = rlc_pP->stat_tx_retransmit_bytes_by_status;
    *stat_tx_retransmit_pdu               = rlc_pP->stat_tx_retransmit_pdu;
    *stat_tx_retransmit_bytes             = rlc_pP->stat_tx_retransmit_bytes;
    *stat_tx_control_pdu                  = rlc_pP->stat_tx_control_pdu;
    *stat_tx_control_bytes                = rlc_pP->stat_tx_control_bytes;
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
    *stat_rx_control_pdu                  = rlc_pP->stat_rx_control_pdu;
    *stat_rx_control_bytes                = rlc_pP->stat_rx_control_bytes;
    *stat_timer_reordering_timed_out      = rlc_pP->stat_timer_reordering_timed_out;
    *stat_timer_poll_retransmit_timed_out = rlc_pP->stat_timer_poll_retransmit_timed_out;
    *stat_timer_status_prohibit_timed_out = rlc_pP->stat_timer_status_prohibit_timed_out;

}
//-----------------------------------------------------------------------------
void
rlc_am_get_pdus (rlc_am_entity_t *rlc_pP, frame_t frameP)
{
//-----------------------------------------------------------------------------
  int display_flag = 0;
  // 5.1.3.1 Transmit operations
  // 5.1.3.1.1
  // General
  // The transmitting side of an AM RLC entity shall prioritize transmission of RLC control PDUs over RLC data PDUs.
  // The transmitting side of an AM RLC entity shall prioritize retransmission of RLC data PDUs over transmission of new
  // AMD PDUs.


    switch (rlc_pP->protocol_state) {

        case RLC_NULL_STATE:
            break;

        case RLC_DATA_TRANSFER_READY_STATE:
            // TRY TO SEND CONTROL PDU FIRST
            if ((rlc_pP->nb_bytes_requested_by_mac > 2) && (rlc_pP->status_requested)) {
                // When STATUS reporting has been triggered, the receiving side of an AM RLC entity shall:
                // - if t-StatusProhibit is not running:
                //     - at the first transmission opportunity indicated by lower layer, construct a STATUS PDU and deliver it to lower layer;
                // - else:
                //     - at the first transmission opportunity indicated by lower layer after t-StatusProhibit expires, construct a single
                //       STATUS PDU even if status reporting was triggered several times while t-StatusProhibit was running and
                //       deliver it to lower layer;
                //
                // When a STATUS PDU has been delivered to lower layer, the receiving side of an AM RLC entity shall:
                //     - start t-StatusProhibit.
                if (rlc_pP->t_status_prohibit.running == 0) {
                    rlc_am_send_status_pdu(rlc_pP,frameP);
                    mem_block_t* pdu = list_remove_head(&rlc_pP->control_pdu_list);
                    if (pdu) {
                        list_add_tail_eurecom (pdu, &rlc_pP->pdus_to_mac_layer);
                        rlc_pP->status_requested = 0;
                        rlc_am_start_timer_status_prohibit(rlc_pP,frameP);
                        return;
                    }
                }
                  else {
                      LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] DELAYED SENT STATUS PDU BECAUSE T-STATUS-PROHIBIT RUNNING (TIME-OUT FRAME %5u)\n",
                        frameP,
                        (rlc_pP->is_enb) ? "eNB" : "UE",
                        rlc_pP->enb_module_id,
                        rlc_pP->ue_module_id,
                        rlc_pP->rb_id,
                        rlc_pP->t_status_prohibit.frame_time_out);
                }
            }
            /*while ((rlc_pP->nb_bytes_requested_by_mac > 0) && (stay_on_this_list)) {
                mem_block_t* pdu = list_get_head(&rlc_pP->control_pdu_list);
                if (pdu != NULL {
                    if ( ((rlc_am_tx_control_pdu_management_t*)(pdu->data))->size <= rlc_pP->nb_bytes_requested_by_mac) {
                        pdu = list_remove_head(&rlc_pP->control_pdu_list);
#ifdef TRACE_RLC_AM_TX
                        msg ("[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] SEND CONTROL PDU\n", ((rlc_am_entity_t *) rlc_pP)->module_id,((rlc_am_entity_t *) rlc_pP)->rb_id, frameP);
#endif
                        list_add_tail_eurecom (pdu, &rlc_pP->pdus_to_mac_layer);
                        rlc_pP->nb_bytes_requested_by_mac = rlc_pP->nb_bytes_requested_by_mac - ((rlc_am_tx_control_pdu_management_t*)(pdu->data))->size;
                    } else {
                      stay_on_this_list = 0;
                    }
                } else {
                    stay_on_this_list = 0;
                }
            }*/
            // THEN TRY TO SEND RETRANS PDU
            if (rlc_pP->first_retrans_pdu_sn >= 0) {
                rlc_am_tx_data_pdu_management_t* tx_data_pdu_management;
                // tx min 3 bytes because of the size of the RLC header
                while ((rlc_pP->nb_bytes_requested_by_mac > 2) &&
                       (rlc_pP->first_retrans_pdu_sn  >= 0) &&
                       (rlc_pP->first_retrans_pdu_sn != rlc_pP->vt_s)) {

                    tx_data_pdu_management = &rlc_pP->pdu_retrans_buffer[rlc_pP->first_retrans_pdu_sn];

                    if ((tx_data_pdu_management->header_and_payload_size <= rlc_pP->nb_bytes_requested_by_mac) && (tx_data_pdu_management->retx_count >= 0) && (tx_data_pdu_management->nack_so_start == 0) && (tx_data_pdu_management->nack_so_stop == 0x7FFF)) {
                        mem_block_t* copy = rlc_am_retransmit_get_copy(rlc_pP, frameP,rlc_pP->first_retrans_pdu_sn);
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] RE-SEND DATA PDU SN %04d   %d BYTES\n",
                            frameP,
                            (rlc_pP->is_enb) ? "eNB" : "UE",
                            rlc_pP->enb_module_id,
                            rlc_pP->ue_module_id,
                            rlc_pP->rb_id,
                            rlc_pP->first_retrans_pdu_sn,
                            tx_data_pdu_management->header_and_payload_size);
                        rlc_pP->stat_tx_data_pdu                   += 1;
                        rlc_pP->stat_tx_retransmit_pdu             += 1;
                        rlc_pP->stat_tx_retransmit_pdu_by_status   += 1;
                        rlc_pP->stat_tx_data_bytes                 += tx_data_pdu_management->header_and_payload_size;
                        rlc_pP->stat_tx_retransmit_bytes           += tx_data_pdu_management->header_and_payload_size;
                        rlc_pP->stat_tx_retransmit_bytes_by_status += tx_data_pdu_management->header_and_payload_size;

                        list_add_tail_eurecom (copy, &rlc_pP->pdus_to_mac_layer);
                        rlc_pP->nb_bytes_requested_by_mac = rlc_pP->nb_bytes_requested_by_mac - tx_data_pdu_management->header_and_payload_size;

                        tx_data_pdu_management->retx_count += 1;
                        return;
                    } else if ((tx_data_pdu_management->retx_count >= 0) && (rlc_pP->nb_bytes_requested_by_mac >= RLC_AM_MIN_SEGMENT_SIZE_REQUEST)) {
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] SEND SEGMENT OF DATA PDU SN %04d MAC BYTES %d SIZE %d RTX COUNT %d  nack_so_start %d nack_so_stop %04X(hex)\n",
                                frameP,
                                (rlc_pP->is_enb) ? "eNB" : "UE",
                                rlc_pP->enb_module_id,
                                rlc_pP->ue_module_id,
                                rlc_pP->rb_id,
                                rlc_pP->first_retrans_pdu_sn,
                                rlc_pP->nb_bytes_requested_by_mac,
                                tx_data_pdu_management->header_and_payload_size,
                                tx_data_pdu_management->retx_count,
                                tx_data_pdu_management->nack_so_start,
                                tx_data_pdu_management->nack_so_stop);

                        mem_block_t* copy = rlc_am_retransmit_get_subsegment(
                                rlc_pP,
                                frameP,
                                rlc_pP->first_retrans_pdu_sn,
                                &rlc_pP->nb_bytes_requested_by_mac);
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] SEND SEGMENT OF DATA PDU SN %04d (NEW SO %05d)\n",
                              frameP,
                              (rlc_pP->is_enb) ? "eNB" : "UE",
                              rlc_pP->enb_module_id,
                              rlc_pP->ue_module_id,
                              rlc_pP->rb_id,
                              rlc_pP->first_retrans_pdu_sn,
                              tx_data_pdu_management->nack_so_start);

                        rlc_pP->stat_tx_data_pdu                   += 1;
                        rlc_pP->stat_tx_retransmit_pdu             += 1;
                        rlc_pP->stat_tx_retransmit_pdu_by_status   += 1;
                        rlc_pP->stat_tx_data_bytes                 += (((struct mac_tb_req*)(copy->data))->tb_size);
                        rlc_pP->stat_tx_retransmit_bytes           += (((struct mac_tb_req*)(copy->data))->tb_size);
                        rlc_pP->stat_tx_retransmit_bytes_by_status += (((struct mac_tb_req*)(copy->data))->tb_size);
                        list_add_tail_eurecom (copy, &rlc_pP->pdus_to_mac_layer);
                    } else {
                        break;
                    }
                    // update first_retrans_pdu_sn
                    while ((rlc_pP->first_retrans_pdu_sn != rlc_pP->vt_s) &&
                           (!(rlc_pP->pdu_retrans_buffer[rlc_pP->first_retrans_pdu_sn].flags.retransmit))) {
                        rlc_pP->first_retrans_pdu_sn = (rlc_pP->first_retrans_pdu_sn+1) & RLC_AM_SN_MASK;
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] UPDATED first_retrans_pdu_sn SN %04d\n",
                              frameP,
                              (rlc_pP->is_enb) ? "eNB" : "UE",
                              rlc_pP->enb_module_id,
                              rlc_pP->ue_module_id,
                              rlc_pP->rb_id,
                              rlc_pP->first_retrans_pdu_sn);
                    };

                    display_flag = 1;
                    if (rlc_pP->first_retrans_pdu_sn == rlc_pP->vt_s) {
                        // no more pdu to be retransmited
                        rlc_pP->first_retrans_pdu_sn = -1;
                        display_flag = 0;
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] CLEAR first_retrans_pdu_sn\n",
                              frameP,
                              (rlc_pP->is_enb) ? "eNB" : "UE",
                              rlc_pP->enb_module_id,
                              rlc_pP->ue_module_id,
                              rlc_pP->rb_id);
                    }
                    if (display_flag > 0) {
                        LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] UPDATED first_retrans_pdu_sn %04d\n",
                              frameP,
                              (rlc_pP->is_enb) ? "eNB" : "UE",
                              rlc_pP->enb_module_id,
                              rlc_pP->ue_module_id,
                              rlc_pP->rb_id,
                              rlc_pP->first_retrans_pdu_sn);
                    }
                    return;

/* ONLY ONE TB PER TTI
                    if ((tx_data_pdu_management->retx_count >= 0) && (rlc_pP->nb_bytes_requested_by_mac < RLC_AM_MIN_SEGMENT_SIZE_REQUEST)) {
#ifdef TRACE_RLC_AM_TX
                      msg ("[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] BREAK LOOP ON RETRANSMISSION BECAUSE ONLY %d BYTES ALLOWED TO TRANSMIT BY MAC\n",frameP,  ((rlc_am_entity_t *) rlc_pP)->module_id,((rlc_am_entity_t *) rlc_pP)->rb_id, rlc_pP->nb_bytes_requested_by_mac);
#endif
                      break;
                    }*/
                }
            }
            if ((rlc_pP->nb_bytes_requested_by_mac > 2) && (rlc_pP->vt_s != rlc_pP->vt_ms)) {
                rlc_am_segment_10(rlc_pP,frameP);
                list_add_list (&rlc_pP->segmentation_pdu_list, &rlc_pP->pdus_to_mac_layer);
                if (rlc_pP->pdus_to_mac_layer.head != NULL) {
                    rlc_pP->stat_tx_data_pdu                   += 1;
                    rlc_pP->stat_tx_data_bytes                 += (((struct mac_tb_req*)(rlc_pP->pdus_to_mac_layer.head->data))->tb_size);
                    return;
                }
            }
            if ((rlc_pP->pdus_to_mac_layer.head == NULL) && (rlc_am_is_timer_poll_retransmit_timed_out(rlc_pP)) && (rlc_pP->nb_bytes_requested_by_mac > 2)) {
                rlc_am_retransmit_any_pdu(rlc_pP,frameP);
                return;
            } else {
                LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] COULD NOT RETRANSMIT ANY PDU BECAUSE ",
                      frameP,
                      (rlc_pP->is_enb) ? "eNB" : "UE",
                      rlc_pP->enb_module_id,
                      rlc_pP->ue_module_id,
                      rlc_pP->rb_id);
                if (rlc_pP->pdus_to_mac_layer.head != NULL) {
                    LOG_D(RLC, "THERE ARE SOME PDUS READY TO TRANSMIT ");
                }
                if (!(rlc_am_is_timer_poll_retransmit_timed_out(rlc_pP))) {
                    LOG_D(RLC, "TIMER POLL DID NOT TIMED OUT (RUNNING = %d NUM PDUS TO RETRANS = %d  NUM BYTES TO RETRANS = %d) ", rlc_pP->t_poll_retransmit.running, rlc_pP->retrans_num_pdus, rlc_pP->retrans_num_bytes_to_retransmit);
                }
                if (rlc_pP->nb_bytes_requested_by_mac <= 2) {
                    LOG_D(RLC, "NUM BYTES REQUESTED BY MAC = %d", rlc_pP->nb_bytes_requested_by_mac);
                }
                LOG_D(RLC, "\n");
            }
            break;

        default:
            LOG_E(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] MAC_DATA_REQ UNKNOWN PROTOCOL STATE 0x%02X\n",
                  frameP,
                  (rlc_pP->is_enb) ? "eNB" : "UE",
                  rlc_pP->enb_module_id,
                  rlc_pP->ue_module_id,
                  rlc_pP->rb_id,
                  rlc_pP->protocol_state);
    }
}
//-----------------------------------------------------------------------------
void
rlc_am_rx (void *arg_pP, frame_t frameP, eNB_flag_t eNB_flagP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------

  rlc_am_entity_t *rlc = (rlc_am_entity_t *) arg_pP;

  switch (rlc->protocol_state) {

      case RLC_NULL_STATE:
       LOG_N(RLC, "[RLC_AM %p] ERROR MAC_DATA_IND IN RLC_NULL_STATE\n", arg_pP);
        list_free (&data_indP.data);
        break;

      case RLC_DATA_TRANSFER_READY_STATE:
        rlc_am_receive_routing (rlc, frameP, eNB_flagP, data_indP);
        break;

      default:
        LOG_E(RLC, "[RLC_AM %p] TX UNKNOWN PROTOCOL STATE 0x%02X\n", rlc, rlc->protocol_state);
  }
}

//-----------------------------------------------------------------------------
struct mac_status_resp
rlc_am_mac_status_indication (void *rlc_pP, frame_t frameP, uint16_t tb_sizeP, struct mac_status_ind tx_statusP)
{
//-----------------------------------------------------------------------------
  struct mac_status_resp  status_resp;
  uint16_t  sdu_size = 0;
  uint16_t  sdu_remaining_size = 0;
  int32_t diff_time=0;
  rlc_am_entity_t *rlc = (rlc_am_entity_t *) rlc_pP;

  status_resp.buffer_occupancy_in_bytes        = 0;
  status_resp.buffer_occupancy_in_pdus         = 0;
  status_resp.head_sdu_remaining_size_to_send  = 0;
  status_resp.head_sdu_creation_time           = 0;
  status_resp.head_sdu_is_segmented            = 0;
  status_resp.rlc_info.rlc_protocol_state = rlc->protocol_state;

  if (rlc->last_frame_status_indication != frameP) {
    rlc_am_check_timer_poll_retransmit(rlc,frameP);
    rlc_am_check_timer_reordering(rlc,frameP);
    rlc_am_check_timer_status_prohibit(rlc,frameP);
  }
  rlc->last_frame_status_indication = frameP;

  rlc->nb_bytes_requested_by_mac = tb_sizeP;

  status_resp.buffer_occupancy_in_bytes = rlc_am_get_buffer_occupancy_in_bytes(rlc,frameP);

  if ((rlc->input_sdus[rlc->current_sdu_index].mem_block != NULL) && (status_resp.buffer_occupancy_in_bytes)) {

          //status_resp.buffer_occupancy_in_bytes += ((rlc_am_entity_t *) rlc)->tx_header_min_length_in_bytes;
          status_resp.buffer_occupancy_in_pdus = rlc->nb_sdu;
          diff_time =   frameP - ((rlc_am_tx_sdu_management_t *) (rlc->input_sdus[rlc->current_sdu_index].mem_block->data))->sdu_creation_time;

          status_resp.head_sdu_creation_time = (diff_time > 0 ) ? (uint32_t) diff_time :  (uint32_t)(0xffffffff - diff_time + frameP) ;

          sdu_size            = ((rlc_am_tx_sdu_management_t *) (rlc->input_sdus[rlc->current_sdu_index].mem_block->data))->sdu_size;
          sdu_remaining_size  = ((rlc_am_tx_sdu_management_t *) (rlc->input_sdus[rlc->current_sdu_index].mem_block->data))->sdu_remaining_size;

          status_resp.head_sdu_remaining_size_to_send = sdu_remaining_size;
          if (sdu_size == sdu_remaining_size)  {
           status_resp.head_sdu_is_segmented = 0;
          }
          else {
              status_resp.head_sdu_is_segmented = 1;
          }

  } else {
  }


#ifdef TRACE_RLC_AM_TX_STATUS
  if (tb_sizeP > 0) {
      LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] MAC_STATUS_INDICATION (DATA) %d bytes -> %d bytes\n",
            frameP,
            (rlc->is_enb) ? "eNB" : "UE",
            rlc->enb_module_id,
            rlc->ue_module_id,
            rlc->rb_id,
            tb_sizeP,
            status_resp.buffer_occupancy_in_bytes);
      /*if ((tx_statusP.tx_status == MAC_TX_STATUS_SUCCESSFUL) && (tx_statusP.no_pdu)) {
          msg ("[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] MAC_STATUS_INDICATION  TX STATUS   SUCCESSFUL %d PDUs\n",rlc->module_id,
rlc->rb_id, frameP, tx_statusP.no_pdu);
      }
      if ((tx_statusP.tx_status == MAC_TX_STATUS_UNSUCCESSFUL) && (tx_statusP.no_pdu)) {
          msg ("[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] MAC_STATUS_INDICATION  TX STATUS UNSUCCESSFUL %d PDUs\n",rlc->module_id, rlc->rb_id,
frameP, tx_statusP.no_pdu);
      }*/
  }
#endif
  return status_resp;
}
//-----------------------------------------------------------------------------
struct mac_data_req
rlc_am_mac_data_request (void *rlc_pP, frame_t frameP)
{
//-----------------------------------------------------------------------------
  struct mac_data_req data_req;
  rlc_am_entity_t *l_rlc_p = (rlc_am_entity_t *) rlc_pP;
  unsigned int nb_bytes_requested_by_mac = ((rlc_am_entity_t *) rlc_pP)->nb_bytes_requested_by_mac;
#ifdef TRACE_RLC_AM_PDU
  rlc_am_pdu_info_t   pdu_info;
  rlc_am_pdu_sn_10_t *rlc_am_pdu_sn_10_p;
  mem_block_t        *tb_p;
  tb_size_t           tb_size_in_bytes;
  int                 num_nack;
  char                message_string[9000];
  size_t              message_string_size = 0;
#   if defined(ENABLE_ITTI)
  MessageDef         *msg_p;
#   endif
  int                 octet_index, index;
#endif

  rlc_am_get_pdus (rlc_pP,frameP);

  list_init (&data_req.data, NULL);
  list_add_list (&l_rlc_p->pdus_to_mac_layer, &data_req.data);
  //((rlc_am_entity_t *) rlc_pP)->tx_pdus += data_req.data.nb_elements;
  if ((nb_bytes_requested_by_mac + data_req.data.nb_elements) > 0) {
      LOG_D(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] MAC_DATA_REQUEST %05d BYTES REQUESTED -> %d TBs\n",
            frameP,
            (l_rlc_p->is_enb) ? "eNB" : "UE",
            l_rlc_p->enb_module_id,
            l_rlc_p->ue_module_id,
            l_rlc_p->rb_id,
            nb_bytes_requested_by_mac,
            data_req.data.nb_elements);
  }
  data_req.buffer_occupancy_in_bytes   = rlc_am_get_buffer_occupancy_in_bytes(l_rlc_p,frameP);
  data_req.rlc_info.rlc_protocol_state = l_rlc_p->protocol_state;

#ifdef TRACE_RLC_AM_PDU
  if (data_req.data.nb_elements > 0) {

      tb_p = data_req.data.head;

      while (tb_p != NULL) {

          rlc_am_pdu_sn_10_p = (rlc_am_pdu_sn_10_t*)((struct mac_tb_req *) (tb_p->data))->data_ptr;
          tb_size_in_bytes   = ((struct mac_tb_req *) (tb_p->data))->tb_size;

          if ((((struct mac_tb_req *) (tb_p->data))->data_ptr[0] & RLC_DC_MASK) == RLC_DC_DATA_PDU ) {
              if (rlc_am_get_data_pdu_infos(frameP,rlc_am_pdu_sn_10_p, tb_size_in_bytes, &pdu_info) >= 0) {
#   if defined(ENABLE_ITTI)
                  message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", tb_size_in_bytes);
                  message_string_size += sprintf(&message_string[message_string_size], "Header size : %u\n", pdu_info.header_size);
                  message_string_size += sprintf(&message_string[message_string_size], "Payload size: %u\n", pdu_info.payload_size);
                  if (pdu_info.rf) {
                      message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA REQ: AMD PDU segment\n\n");
                  } else {
                      message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA REQ: AMD PDU\n\n");
                  }
                  message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
                  message_string_size += sprintf(&message_string[message_string_size], "  D/C       : %u\n", pdu_info.d_c);
                  message_string_size += sprintf(&message_string[message_string_size], "  RF        : %u\n", pdu_info.rf);
                  message_string_size += sprintf(&message_string[message_string_size], "  P         : %u\n", pdu_info.p);
                  message_string_size += sprintf(&message_string[message_string_size], "  FI        : %u\n", pdu_info.fi);
                  message_string_size += sprintf(&message_string[message_string_size], "  E         : %u\n", pdu_info.e);
                  message_string_size += sprintf(&message_string[message_string_size], "  SN        : %u\n", pdu_info.sn);
                  if (pdu_info.rf) {
                      message_string_size += sprintf(&message_string[message_string_size], "  LSF       : %u\n", pdu_info.lsf);
                      message_string_size += sprintf(&message_string[message_string_size], "  SO        : %u\n", pdu_info.so);
                  }
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

                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_AM_DATA_PDU_REQ, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_am_data_pdu_req.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_am_data_pdu_req.text, message_string, message_string_size);

                  if (l_rlc_p->is_enb) {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
                  } else {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
                  }
# else
                  rlc_am_display_data_pdu_infos(l_rlc_p, frameP, &pdu_info);
# endif
              }
          } else {
              if (rlc_am_get_control_pdu_infos(rlc_am_pdu_sn_10_p, &tb_size_in_bytes, &g_rlc_am_control_pdu_info) >= 0) {
                  tb_size_in_bytes   = ((struct mac_tb_req *) (tb_p->data))->tb_size; //tb_size_in_bytes modified by rlc_am_get_control_pdu_infos!

#   if defined(ENABLE_ITTI)
                  message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", tb_size_in_bytes);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA REQ: STATUS PDU\n\n");
                  message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
                  message_string_size += sprintf(&message_string[message_string_size], "  D/C       : %u\n", g_rlc_am_control_pdu_info.d_c);
                  message_string_size += sprintf(&message_string[message_string_size], "  CPT       : %u\n", g_rlc_am_control_pdu_info.cpt);
                  message_string_size += sprintf(&message_string[message_string_size], "  ACK_SN    : %u\n", g_rlc_am_control_pdu_info.ack_sn);
                  message_string_size += sprintf(&message_string[message_string_size], "  E1        : %u\n", g_rlc_am_control_pdu_info.e1);
                  for (num_nack = 0; num_nack < g_rlc_am_control_pdu_info.num_nack; num_nack++) {
                      if (g_rlc_am_control_pdu_info.nack_list[num_nack].e2) {
                          message_string_size += sprintf(&message_string[message_string_size], "  NACK SN %04d SO START %05d SO END %05d",
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].nack_sn,
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].so_start,
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].so_end);
                      } else {
                              message_string_size += sprintf(&message_string[message_string_size], "  NACK SN %04d",  g_rlc_am_control_pdu_info.nack_list[num_nack].nack_sn);
                      }
                  }
                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_AM_STATUS_PDU_REQ, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_am_status_pdu_req.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_am_status_pdu_req.text, message_string, message_string_size);

                  if (l_rlc_p->is_enb) {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
                  } else {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
                  }
#   endif
              }
          }
          tb_p = tb_p->next;
      }
  }
#endif
  return data_req;
}
//-----------------------------------------------------------------------------
void
rlc_am_mac_data_indication (void *rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, struct mac_data_ind data_indP)
{
//-----------------------------------------------------------------------------
    rlc_am_entity_t           *l_rlc_p = (rlc_am_entity_t *) rlc_pP;
    /*rlc_am_control_pdu_info_t control_pdu_info;
    int                       num_li;
    int16_t                     tb_size;*/
#ifdef TRACE_RLC_AM_PDU
  rlc_am_pdu_info_t   pdu_info;
  rlc_am_pdu_sn_10_t *rlc_am_pdu_sn_10_p;
  mem_block_t        *tb_p;
  sdu_size_t          tb_size_in_bytes;
  int                 num_nack;
  char                message_string[7000];
  size_t              message_string_size = 0;
#   if defined(ENABLE_ITTI)
  MessageDef         *msg_p;
#   endif
  int                 octet_index, index;
#endif

#ifdef TRACE_RLC_AM_PDU
  if (data_indP.data.nb_elements > 0) {

      tb_p = data_indP.data.head;

      while (tb_p != NULL) {

          rlc_am_pdu_sn_10_p = (rlc_am_pdu_sn_10_t*)((struct mac_tb_ind *) (tb_p->data))->data_ptr;
          tb_size_in_bytes   = ((struct mac_tb_ind *) (tb_p->data))->size;

          if ((((struct mac_tb_ind *) (tb_p->data))->data_ptr[0] & RLC_DC_MASK) == RLC_DC_DATA_PDU ) {
              if (rlc_am_get_data_pdu_infos(frameP,rlc_am_pdu_sn_10_p, tb_size_in_bytes, &pdu_info) >= 0) {
#   if defined(ENABLE_ITTI)
                  message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", tb_size_in_bytes);
                  message_string_size += sprintf(&message_string[message_string_size], "Header size : %u\n", pdu_info.header_size);
                  message_string_size += sprintf(&message_string[message_string_size], "Payload size: %u\n", pdu_info.payload_size);
                  if (pdu_info.rf) {
                      message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA IND: AMD PDU segment\n\n");
                  } else {
                      message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA IND: AMD PDU\n\n");
                  }
                  message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
                  message_string_size += sprintf(&message_string[message_string_size], "  D/C       : %u\n", pdu_info.d_c);
                  message_string_size += sprintf(&message_string[message_string_size], "  RF        : %u\n", pdu_info.rf);
                  message_string_size += sprintf(&message_string[message_string_size], "  P         : %u\n", pdu_info.p);
                  message_string_size += sprintf(&message_string[message_string_size], "  FI        : %u\n", pdu_info.fi);
                  message_string_size += sprintf(&message_string[message_string_size], "  E         : %u\n", pdu_info.e);
                  message_string_size += sprintf(&message_string[message_string_size], "  SN        : %u\n", pdu_info.sn);
                  if (pdu_info.rf) {
                      message_string_size += sprintf(&message_string[message_string_size], "  LSF       : %u\n", pdu_info.lsf);
                      message_string_size += sprintf(&message_string[message_string_size], "  SO        : %u\n", pdu_info.so);
                  }
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

                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_AM_DATA_PDU_IND, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_am_data_pdu_ind.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_am_data_pdu_ind.text, message_string, message_string_size);

                  if (l_rlc_p->is_enb) {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
                  } else {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
                  }
# else
                  rlc_am_display_data_pdu_infos(l_rlc_p, frameP, &pdu_info);
# endif
              }
          } else {
              if (rlc_am_get_control_pdu_infos(rlc_am_pdu_sn_10_p, &tb_size_in_bytes, &g_rlc_am_control_pdu_info) >= 0) {

#   if defined(ENABLE_ITTI)
                  message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU size    : %u\n", ((struct mac_tb_ind *) (tb_p->data))->size);
                  message_string_size += sprintf(&message_string[message_string_size], "PDU type    : RLC AM DATA IND: STATUS PDU\n\n");
                  message_string_size += sprintf(&message_string[message_string_size], "Header      :\n");
                  message_string_size += sprintf(&message_string[message_string_size], "  D/C       : %u\n", g_rlc_am_control_pdu_info.d_c);
                  message_string_size += sprintf(&message_string[message_string_size], "  CPT       : %u\n", g_rlc_am_control_pdu_info.cpt);
                  message_string_size += sprintf(&message_string[message_string_size], "  ACK_SN    : %u\n", g_rlc_am_control_pdu_info.ack_sn);
                  message_string_size += sprintf(&message_string[message_string_size], "  E1        : %u\n", g_rlc_am_control_pdu_info.e1);
                  for (num_nack = 0; num_nack < g_rlc_am_control_pdu_info.num_nack; num_nack++) {
                      if (g_rlc_am_control_pdu_info.nack_list[num_nack].e2) {
                          message_string_size += sprintf(&message_string[message_string_size], "  NACK SN %04d SO START %05d SO END %05d",
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].nack_sn,
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].so_start,
                                  g_rlc_am_control_pdu_info.nack_list[num_nack].so_end);
                      } else {
                              message_string_size += sprintf(&message_string[message_string_size], "  NACK SN %04d",  g_rlc_am_control_pdu_info.nack_list[num_nack].nack_sn);
                      }
                  }
                  msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_AM_STATUS_PDU_IND, message_string_size + sizeof (IttiMsgText));
                  msg_p->ittiMsg.rlc_am_status_pdu_ind.size = message_string_size;
                  memcpy(&msg_p->ittiMsg.rlc_am_status_pdu_ind.text, message_string, message_string_size);

                  if (l_rlc_p->is_enb) {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
                  } else {
                      itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
                  }
#   endif
              }
          }
          tb_p = tb_p->next;
      }
  }
#endif
    rlc_am_rx (rlc_pP, frameP, eNB_flagP, data_indP);
}

//-----------------------------------------------------------------------------
void
rlc_am_data_req (void *rlc_pP, frame_t frameP, mem_block_t * sdu_pP)
{
//-----------------------------------------------------------------------------
  rlc_am_entity_t     *l_rlc_p = (rlc_am_entity_t *) rlc_pP;
  uint32_t             mui;
  uint16_t             data_offset;
  uint16_t             data_size;
  uint8_t              conf;
#if defined(TRACE_RLC_AM_PDU)
  char                 message_string[7000];
  size_t               message_string_size = 0;
#if defined(ENABLE_ITTI)
  MessageDef          *msg_p;
#endif
  int                  octet_index, index;
#endif

  if ((l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].mem_block == NULL) &&
      (l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.segmented == 0) &&
      (((l_rlc_p->next_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE) != l_rlc_p->current_sdu_index)) {


      memset(&l_rlc_p->input_sdus[l_rlc_p->next_sdu_index], 0, sizeof(rlc_am_tx_sdu_management_t));
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].mem_block = sdu_pP;

      mui         = ((struct rlc_am_data_req *) (sdu_pP->data))->mui;
      data_offset = ((struct rlc_am_data_req *) (sdu_pP->data))->data_offset;
      data_size   = ((struct rlc_am_data_req *) (sdu_pP->data))->data_size;
      conf        = ((struct rlc_am_data_req *) (sdu_pP->data))->conf;

#if defined(TRACE_RLC_AM_PDU)
      message_string_size += sprintf(&message_string[message_string_size], "Bearer      : %u\n", l_rlc_p->rb_id);
      message_string_size += sprintf(&message_string[message_string_size], "SDU size    : %u\n", data_size);
      message_string_size += sprintf(&message_string[message_string_size], "MUI         : %u\n", mui);
      message_string_size += sprintf(&message_string[message_string_size], "CONF        : %u\n", conf);

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
          message_string_size += sprintf(&message_string[message_string_size], " %02x", ((uint8_t*)(&sdu_pP->data[data_offset]))[octet_index]);
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
      msg_p = itti_alloc_new_message_sized (l_rlc_p->is_enb > 0 ? TASK_RLC_ENB:TASK_RLC_UE , RLC_AM_SDU_REQ, message_string_size + sizeof (IttiMsgText));
      msg_p->ittiMsg.rlc_am_sdu_req.size = message_string_size;
      memcpy(&msg_p->ittiMsg.rlc_am_sdu_req.text, message_string, message_string_size);

      if (l_rlc_p->is_enb) {
          itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->enb_module_id, msg_p);
      } else {
          itti_send_msg_to_task(TASK_UNKNOWN, l_rlc_p->ue_module_id + NB_eNB_INST, msg_p);
      }
#   else
      LOG_T(RLC, "%s", message_string);
#   endif
#endif

     l_rlc_p->stat_tx_pdcp_sdu   += 1;
     l_rlc_p->stat_tx_pdcp_bytes += data_size;

      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].mui      = mui;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].sdu_size = data_size;
      //l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].confirm  = conf;

      l_rlc_p->sdu_buffer_occupancy += data_size;
      l_rlc_p->nb_sdu += 1;
      l_rlc_p->nb_sdu_no_segmented += 1;

      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].first_byte = (uint8_t*)(&sdu_pP->data[data_offset]);
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].sdu_remaining_size = l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].sdu_size;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].sdu_segmented_size = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].sdu_creation_time  = frameP;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].nb_pdus            = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].nb_pdus_ack        = 0;
      //l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].nb_pdus_time = 0;
      //l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].nb_pdus_internal_use = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.discarded    = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.segmented    = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.segmentation_in_progress = 0;
      l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.no_new_sdu_segmented_in_last_pdu = 0;
      //l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].li_index_for_discard = -1;
      l_rlc_p->next_sdu_index = (l_rlc_p->next_sdu_index + 1) % RLC_AM_SDU_CONTROL_BUFFER_SIZE;
      LOG_I(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] RLC_AM_DATA_REQ size %d Bytes,  NB SDU %d current_sdu_index=%d next_sdu_index=%d conf %d mui %d\n",
            frameP,
            (l_rlc_p->is_enb) ? "eNB" : "UE",
            l_rlc_p->enb_module_id,
            l_rlc_p->ue_module_id,
            l_rlc_p->rb_id,
            data_size,
            l_rlc_p->nb_sdu,
            l_rlc_p->current_sdu_index,
            l_rlc_p->next_sdu_index,
            conf,
            mui);
  } else {
      LOG_W(RLC, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] RLC_AM_DATA_REQ BUFFER FULL, NB SDU %d current_sdu_index=%d next_sdu_index=%d size_input_sdus_buffer=%d\n",
            frameP,
            (l_rlc_p->is_enb) ? "eNB" : "UE",
            l_rlc_p->enb_module_id,
            l_rlc_p->ue_module_id,
            l_rlc_p->rb_id,
            l_rlc_p->nb_sdu,
            l_rlc_p->current_sdu_index,
            l_rlc_p->next_sdu_index,
            RLC_AM_SDU_CONTROL_BUFFER_SIZE);
      LOG_W(RLC, "                                        input_sdus[].mem_block=%p next input_sdus[].flags.segmented=%d\n", l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].mem_block, l_rlc_p->input_sdus[l_rlc_p->next_sdu_index].flags.segmented);
      l_rlc_p->stat_tx_pdcp_sdu_discarded   += 1;
      l_rlc_p->stat_tx_pdcp_bytes_discarded += ((struct rlc_am_data_req *) (sdu_pP->data))->data_size;
      free_mem_block (sdu_pP);
#if defined(STOP_ON_IP_TRAFFIC_OVERLOAD)
      AssertFatal(0, "[FRAME %5u][%s][RLC_AM][MOD %u/%u][RB %u] RLC_AM_DATA_REQ size %d Bytes, SDU DROPPED, INPUT BUFFER OVERFLOW NB SDU %d current_sdu_index=%d next_sdu_index=%d \n",
          frameP,
          (l_rlc_p->is_enb) ? "eNB" : "UE",
          l_rlc_p->enb_module_id,
          l_rlc_p->ue_module_id,
          l_rlc_p->rb_id,
          data_size,
          l_rlc_p->nb_sdu,
          l_rlc_p->current_sdu_index,
          l_rlc_p->next_sdu_index);
#endif
  }
}
