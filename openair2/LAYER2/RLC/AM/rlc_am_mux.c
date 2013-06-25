/***************************************************************************
                          rlc_am_mux.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_constants.h"
#include "list.h"
#include "mem_block.h"
#include "rlc_am_retrans_proto_extern.h"
#include "rlc_am_mac_status_proto_extern.h"
#include "rlc_am_discard_tx_proto_extern.h"
#include "rlc_am_proto_extern.h"
#include "LAYER2/MAC/extern.h"
//#define DEBUG_MUX
//#define DEBUG_RLC_AM_POLL

#ifdef NODE_MT
/*
 * mux PDUs from segmentation buffer and PDUs from retransmission buffer
 * onto transmission buffer
 */
//-----------------------------------------------------------------------------
void
rlc_am_mux_ue (struct rlc_am_entity *rlcP, unsigned int traffic_typeP)
{
//-----------------------------------------------------------------------------

  mem_block_t      *pdu;
  mem_block_t      *sdu;
  mem_block_t      *copy_pdu;
  struct rlc_am_pdu_header *rlc_header;
  int             pool_is_set;
  int             index;
  int             sdu_index;
  unsigned int    j, segmentation_returned_pdu;
  unsigned int    data_pdu_tx;  // data pdu effectively transmitted
  unsigned int    tx_pdu;
  s16_t             nb_pdu_to_transmit_ch1;
  s16_t             nb_pdu_to_transmit_ch2;
#    ifdef DEBUG_MUX
  u16_t             id;
#    endif
  data_pdu_tx = 0;
  nb_pdu_to_transmit_ch1 = rlcP->nb_pdu_requested_by_mac_on_ch1;
  nb_pdu_to_transmit_ch2 = rlcP->nb_pdu_requested_by_mac_on_ch2;


  // discard
  if ((rlcP->sdu_discarded.head) && (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
    rlc_am_process_sdu_discarded (rlcP);
  }
  /******************************************
  *             CONTROL PDU                 *
  ******************************************/
  // from TS25.322 V4.2.0 p13
  // In case two logical channels are configured in the uplink, AMD PDUs are transmitted
  // on the first logical channel, and control PDUs are transmitted on the second logical
  // channel.
  if (rlcP->nb_logical_channels_per_rlc == 1) {
    while ((rlcP->control.head) && (nb_pdu_to_transmit_ch1)) {
      pdu = list_remove_head (&rlcP->control);
      if ((traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
        tx_pdu = 1;
      } else {
        if (((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type & (RLC_AM_RESET_PDU_TYPE | RLC_AM_RESET_ACK_PDU_TYPE)) {
          tx_pdu = 1;
        } else {
          tx_pdu = 0;
        }
      }
      if ((tx_pdu)) {
        nb_pdu_to_transmit_ch1--;
        // for polling
        rlcP->vt_pdu++;
        ((struct mac_tb_req *) (pdu->data))->rlc_tb_type = ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)];
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer_ch1);
#    ifdef DEBUG_MUX
        msg ("[RLC_AM][RB %d][MUX]  TX CONTROL PDU CH1 VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      } else {
        free_mem_block (pdu);
      }
    }
  } else if (rlcP->nb_logical_channels_per_rlc == 2) {
    while ((rlcP->control.head) && (nb_pdu_to_transmit_ch2)) {
      pdu = list_remove_head (&rlcP->control);
      if ((traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
        tx_pdu = 1;
      } else {
        if (((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type & (RLC_AM_RESET_PDU_TYPE | RLC_AM_RESET_ACK_PDU_TYPE)) {
          tx_pdu = 1;
        } else {
          tx_pdu = 0;
        }
      }
      if ((tx_pdu)) {
        nb_pdu_to_transmit_ch2--;
        // for polling
        rlcP->vt_pdu++;
        ((struct mac_tb_req *) (pdu->data))->rlc_tb_type = ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)];
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer_ch2);
#    ifdef DEBUG_MUX
        msg ("[RLC_AM][RB %d][MUX]  TX CONTROL PDU CH2 VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X \n",
             rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      } else {
        free_mem_block (pdu);
      }
    }
  }

  if (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_DATA) {
    /******************************************
    *       RETRANSMITED DATA PDU             *
    ******************************************/
    // priority is then made at retransmitted PDUs
    while ((rlcP->retransmission_buffer_to_send.head) && (nb_pdu_to_transmit_ch1) && !(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
      // each retransmission may send RLC in RESET STATE if no discard is configured
      copy_pdu = list2_remove_head (&rlcP->retransmission_buffer_to_send);

      // unlink the pdu in retransmission buffer and its copy (mechanism avoiding multi-retransmission of the same
      // pdu in the same queue)
      pdu = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->copy;
#    ifdef DEBUG_MUX
            rlc_am_display_data_pdu7(copy_pdu);
#    endif
      ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy = NULL;

      ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
      ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
      ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
      ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
      list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
      nb_pdu_to_transmit_ch1--;
      data_pdu_tx++;
      // for polling
      if (rlcP->poll_pdu_trigger) {
        rlcP->vt_pdu++;
        if (rlcP->vt_pdu >= rlcP->poll_pdu_trigger) {
          // set poll bit
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
          rlcP->vt_pdu = 0;
        } else {
          // reset poll bit
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 =
            (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2) & ~RLC_AM_P_STATUS_REPORT_REQUESTED;
        }
      }
#    ifdef DEBUG_MUX
      id = (((u16_t) (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
        ((((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

      msg ("[RLC_AM][RB %d][MUX] RETRANSMIT DATA PDU %04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
           rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
    }

    /******************************************
    *                DATA PDU                 *
    ******************************************/
    if (!(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
      segmentation_returned_pdu = 1;

      while ((nb_pdu_to_transmit_ch1) && (rlcP->vt_s != rlcP->vt_ms) && (segmentation_returned_pdu) && !(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {

        if ((pdu = rlcP->rlc_segment (rlcP))) {

          rlc_header = (struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]);
          // check polling
          pool_is_set = 0;
          if (rlcP->poll_pdu_trigger) {
            rlcP->vt_pdu++;
            // test every poll_PDU trigger
            if (rlcP->vt_pdu >= rlcP->poll_pdu_trigger) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              rlc_header->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              rlcP->vt_pdu = 0;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY poll_pdu trigger\n", rlcP);
#    endif
            }
          }
          // test every poll_SDU trigger
          if ((rlcP->poll_sdu_trigger) && (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->last_pdu_of_sdu)) {
            if (rlcP->vt_sdu >= rlcP->poll_sdu_trigger) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              rlc_header->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              rlcP->vt_sdu = 0;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY poll_sdu trigger\n", rlcP);
#    endif
            } else {
              rlcP->vt_sdu += ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->last_pdu_of_sdu;
            }
          }

          if (!pool_is_set) {
            // test last pdu in transmission buffer trigger == last sdu in input buffer in this implementation of RLC AM
            if ((rlcP->last_transmission_pdu_poll_trigger > 0) && (rlcP->nb_sdu == 0)) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              rlc_header->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY last pdu in tr buffer trigger\n", rlcP);
#    endif
            }
            if (!pool_is_set) {
              // test last pdu in retransmission buffer trigger
              if ((rlcP->retransmission_buffer_to_send.head) && (rlcP->retransmission_buffer_to_send.head == rlcP->retransmission_buffer_to_send.tail) && (rlcP->last_retransmission_pdu_poll_trigger)) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
                rlc_header->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
                pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
                msg ("[RLC_AM %p] POOL SET BY last pdu in retrans buffer trigger\n", rlcP);
#    endif
              }
              if (!pool_is_set) {
                // test window based trigger
                if (rlcP->poll_window_trigger) {
                  j = (unsigned int) (((rlcP->vt_s + 4096) - rlcP->vt_a) % 4096) * 100 / rlcP->vt_ws;

                  if (j >= rlcP->poll_window_trigger) {
                    ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
                    rlc_header->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
                    pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
                    msg ("[RLC_AM %p][POOL] POOL SET BY window based trigger j=%d\n", rlcP, j);
#    endif
                  }
                }
              }
            }
          }

          rlc_header->byte1 |= (rlcP->vt_s >> 5);
          rlc_header->byte2 |= ((u8_t) rlcP->vt_s << 3);
          ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sn = rlcP->vt_s;
          insert_into_retransmission_buffer (rlcP, rlcP->vt_s % rlcP->recomputed_configured_tx_window_size, pdu);


          // check if timer based discard configured
          if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_EXPLICIT)) {
            for (index = 0; index < ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->nb_sdu; index++) {
              sdu_index = ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index];
              sdu = rlcP->input_sdus[sdu_index];

              // mark the sdu as invalid except if confirm requested
              if (!((struct rlc_am_tx_sdu_management *) (sdu->data))->confirm) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index] = -1;        // tag sdu as non valid
              }

              ((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus_time += 1;
              if ((((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus_time ==
                   ((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus) &&
                  !(((struct rlc_am_tx_sdu_management *) (sdu->data))->confirm) && (((struct rlc_am_tx_sdu_management *) (sdu->data))->segmented)
                ) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index] = -1;        // tag sdu as non valid

                // we can remove the sdu if timer based discard and all pdus submitted to lower layers and no confirm running
                free_mem_block (sdu);
                rlcP->input_sdus[sdu_index] = NULL;
#    ifdef DEBUG_RLC_AM_FREE_SDU
                msg ("[RLC_AM][RB %d] MUX FREE_SDU INDEX %d\n", rlcP->rb_id, sdu_index);
#    endif
              } else {
                ((struct rlc_am_tx_sdu_management *) (sdu->data))->last_pdu_sn = rlcP->vt_s;
              }
            }
          } else if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_MAX_DAT_RETRANSMISSION)) {
            for (index = 0; index < ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->nb_sdu; index++) {
              // update the sn of the last transmitted pdu were this sdu was segmented
              ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->last_pdu_sn = rlcP->vt_s;
              // register the sn of the pdus where the sdu was segmented
              ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->
                pdus_index[((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->nb_pdus_internal_use++] = rlcP->vt_s;
#    ifdef DEBUG_MUX
              msg ("[RLC_AM][RB %d] MUX SDU INDEX %d LINK PDU SN 0x%04X\n", rlcP->rb_id, ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index], rlcP->vt_s);
#    endif
            }
          }

          rlcP->vt_s = (rlcP->vt_s + 1) & SN_12BITS_MASK;


          copy_pdu = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation) + GUARD_CRC_LIH_SIZE);
          if (copy_pdu) {
            memcpy (copy_pdu->data, pdu->data, rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation));
#    ifdef DEBUG_MUX
            rlc_am_display_data_pdu7(pdu);
#    endif


            ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
            ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
            ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
            ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
            ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
            ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
            list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
            nb_pdu_to_transmit_ch1--;
            data_pdu_tx++;


#    ifdef DEBUG_MUX
            id = (((u16_t) (rlc_header->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) | ((rlc_header->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);
            msg ("[RLC_AM][RB %d][MUX] TX DATA PDU 0x%04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
                 rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
            /*        for (index = 0; index < 32 ; index++) {
               msg("%02X.", pdu->data[index]);
               }
               msg("\n");
             */
#    endif
          } else {
            msg ("[RLC_AM][RB %d][MUX] OUT OF MEMORY \n", rlcP->rb_id);
          }
        } else {                // segmentation returned no PDU
          segmentation_returned_pdu = 0;
        }
      }
    }
    // From 3GPP TS 25.322 V5.0.0 (2002-03)
    // -        if a poll has been triggered by either the poll triggers "Poll timer" or "Timer based" (see subclause 9.7.1);
    // AND
    // -        if no AMD PDU is scheduled for transmission or retransmission:
    //     -    if the value of "Configured_Tx_Window_Size" is larger than or equal to "2048":
    //         -        select the AMD PDU with "Sequence Number" equal to VT(S)-1.
    //     -    otherwise if the "Configured_Tx_Window_Size" is less than "2048";
    //         -        select the AMD PDU with "Sequence Number" equal to VT(S)-1; or
    //         -        select an AMD PDU that has not yet been acknowledged by the peer entity;
    //     -    schedule the selected AMD PDU for retransmission (in order to transmit a poll).

    // The Sender may also schedule an AMD PDU for retransmission even if none of the criteria above is fulfilled. In this case, the Sender may:
    // -        if the value of "Configured_Tx_Window_Size" is larger than or equal to "2048":
    //     -    select the AMD PDU with "Sequence Number" equal to VT(S)-1.
    // -        otherwise if the "Configured_Tx_Window_Size" is less than "2048":
    //     -    select the AMD PDU with "Sequence Number" equal to VT(S)-1; or
    //     -    select an AMD PDU that has not yet been acknowledged by the peer entity;
    // -        schedule the selected AMD PDU for retransmission.

    //  TO DO : triggers "Poll timer" or "Timer based"
    if (!(data_pdu_tx)) {
      if (rlcP->configured_tx_window_size < 2048) {
        rlc_am_get_not_acknowledged_pdu_optimized (rlcP);
      } else if (rlcP->vt_s != rlcP->vt_a) {
	rlc_am_get_not_acknowledged_pdu_optimized (rlcP);//rlc_am_get_not_acknowledged_pdu_vt_s_minus_1 (rlcP);
      }
      // each retransmission may send RLC in RESET STATE if no discard is configured
      copy_pdu = list2_remove_head (&rlcP->retransmission_buffer_to_send);

      if ((copy_pdu)) {
        // unlink the pdu in retransmission buffer and its copy (mechanism avoiding multi-retransmission of the same
        // pdu in the same queue)
        pdu = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->copy;
#    ifdef DEBUG_MUX
            rlc_am_display_data_pdu7(copy_pdu);
#    endif
        ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy = NULL;

        ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
        ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
        nb_pdu_to_transmit_ch1--;
        data_pdu_tx++;
        // for polling
        rlcP->vt_pdu++;
        // set poll bit
        ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;

#    ifdef DEBUG_MUX
        id = (((u16_t) (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
          ((((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

        msg ("[RLC_AM][RB %d][MUX] RETRANSMIT DATA PDU 0x%04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      }
    }
  }
}
#endif
#ifdef NODE_RG
//#define DEBUG_MUX
/*
 * mux PDUs from segmentation buffer and PDUs from retransmission buffer
 * onto transmission buffer
 */
//-----------------------------------------------------------------------------
void
rlc_am_mux_rg (struct rlc_am_entity *rlcP, unsigned int traffic_typeP)
{
//-----------------------------------------------------------------------------

  mem_block_t      *pdu;
  mem_block_t      *sdu;
  mem_block_t      *copy_pdu = NULL;
  struct rlc_am_pdu_header *rlc_header;
  int             pool_is_set;
  int             index;
  int             sdu_index;
  unsigned int    j, segmentation_returned_pdu;
  unsigned int    data_pdu_tx;  // data pdu effectively transmitted
  unsigned int    tx_pdu;
  static unsigned int last_scheduled;

  s16_t             nb_pdu_to_transmit_ch1;
  s16_t             nb_pdu_to_transmit_ch2;
#    ifdef DEBUG_MUX
  u16_t             id;
#    endif
  data_pdu_tx = 0;
  nb_pdu_to_transmit_ch1 = rlcP->nb_pdu_requested_by_mac_on_ch1;
  nb_pdu_to_transmit_ch2 = rlcP->nb_pdu_requested_by_mac_on_ch2;

  // discard
  if ((rlcP->sdu_discarded.head) && (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
    rlc_am_process_sdu_discarded (rlcP);
  }
  // from TS25.322 V4.2.0 p13
  // In case two logical channels are configured in the downlink, AMD and Control
  // PDUs can be transmitted on any of the two logical channels.
  /******************************************
  *             CONTROL PDU                 *
  ******************************************/
  if (rlcP->nb_logical_channels_per_rlc == 1) {
    while ((rlcP->control.head) && (nb_pdu_to_transmit_ch1)) {
      pdu = list_remove_head (&rlcP->control);
      if ((traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
        tx_pdu = 1;
      } else {
        if (((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type & (RLC_AM_RESET_PDU_TYPE | RLC_AM_RESET_ACK_PDU_TYPE)) {
          tx_pdu = 1;
        } else {
          tx_pdu = 0;
        }
      }
      if ((tx_pdu)) {
        nb_pdu_to_transmit_ch1--;
        // for polling
        rlcP->vt_pdu++;
        ((struct mac_tb_req *) (pdu->data))->rlc_tb_type = ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)];
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer_ch1);
        rlcP->stat_tx_control_pdu += 1;
#    ifdef DEBUG_MUX
        msg ("[RLC_AM][RB %d][MUX]  TX CONTROL PDU  CH1 VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      } else {
        free_mem_block (pdu);
      }
    }
  } else if (rlcP->nb_logical_channels_per_rlc == 2) {
    while ((rlcP->control.head) && (nb_pdu_to_transmit_ch2)) {
      pdu = list_remove_head (&rlcP->control);
      if ((traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
        tx_pdu = 1;
      } else {
        if (((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type & (RLC_AM_RESET_PDU_TYPE | RLC_AM_RESET_ACK_PDU_TYPE)) {
          tx_pdu = 1;
        } else {
          tx_pdu = 0;
        }
      }
      if ((tx_pdu)) {
        nb_pdu_to_transmit_ch2--;
        // for polling
        rlcP->vt_pdu++;
        ((struct mac_tb_req *) (pdu->data))->rlc_tb_type = ((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)];
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer_ch2);
        rlcP->stat_tx_control_pdu += 1;

#    ifdef DEBUG_MUX
        msg ("[RLC_AM][RB %d][MUX]  TX CONTROL PDU  CH2 VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      } else {
        free_mem_block (pdu);
      }
    }
    while ((rlcP->control.head) && (nb_pdu_to_transmit_ch1)) {
      pdu = list_remove_head (&rlcP->control);
      if ((traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS)) {
        tx_pdu = 1;
      } else {
        if (((struct rlc_am_tx_control_pdu_management *) (pdu->data))->rlc_tb_type & (RLC_AM_RESET_PDU_TYPE | RLC_AM_RESET_ACK_PDU_TYPE)) {
          tx_pdu = 1;
        } else {
          tx_pdu = 0;
        }
      }
      if ((tx_pdu)) {
        nb_pdu_to_transmit_ch1--;
        // for polling
        rlcP->vt_pdu++;
        ((struct mac_tb_req *) (pdu->data))->rlc_tb_type = ((struct rlc_am_tx_control_pdu_management *) (copy_pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (pdu->data))->data_ptr = &pdu->data[sizeof (struct rlc_am_tx_control_pdu_allocation)];
        ((struct mac_tb_req *) (pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (pdu, &rlcP->pdus_to_mac_layer_ch1);
        rlcP->stat_tx_control_pdu += 1;


#    ifdef DEBUG_MUX
        msg ("[RLC_AM][RB %d][MUX]  TX CONTROL PDU  CH1 VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      } else {
        free_mem_block (pdu);
      }
    }
  }

  if (traffic_typeP & RLC_AM_TRAFFIC_ALLOWED_FOR_DATA) {
    /******************************************
    *       RETRANSMITED DATA PDU             *
    ******************************************/
    while ((rlcP->retransmission_buffer_to_send.head) && (nb_pdu_to_transmit_ch1) && !(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
      // each retransmission may send RLC in RESET STATE if no discard is configured
      copy_pdu = list2_remove_head (&rlcP->retransmission_buffer_to_send);

      // unlink the pdu in retransmission buffer and its copy (mechanism avoiding multi-retransmission of the same
      // pdu in the same queue)
      pdu = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->copy;
#    ifdef DEBUG_MUX
       rlc_am_display_data_pdu7(copy_pdu);
#    endif
      ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy = NULL;

      ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
      ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
      ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
      ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;

      list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
        rlcP->stat_tx_retransmit_pdu += 1;

      nb_pdu_to_transmit_ch1--;
      data_pdu_tx++;
      // for polling
      if (rlcP->poll_pdu_trigger) {
        rlcP->vt_pdu++;
        if (rlcP->vt_pdu >= rlcP->poll_pdu_trigger) {
          // set poll bit
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
          rlcP->vt_pdu = 0;
        } else {
          // reset poll bit
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 =
            ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & ~RLC_AM_P_STATUS_REPORT_REQUESTED;
        }
      }
#    ifdef DEBUG_MUX
      id = (((u16_t) (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
        ((((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

      msg ("[RLC_AM][RB %d][MUX] RETRANSMIT DATA PDU %04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
           rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
    }

    while ((rlcP->retransmission_buffer_to_send.head) && (nb_pdu_to_transmit_ch2) && !(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
      // each retransmission may send RLC in RESET STATE if no discard is configured
      copy_pdu = list2_remove_head (&rlcP->retransmission_buffer_to_send);

      // unlink the pdu in retransmission buffer and its copy (mechanism avoiding multi-retransmission of the same
      // pdu in the same queue)
      pdu = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->copy;
#    ifdef DEBUG_MUX
       rlc_am_display_data_pdu7(copy_pdu);
#    endif
      ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy = NULL;

      ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
      ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
      ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
      ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
      ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;

      list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch2);
        rlcP->stat_tx_retransmit_pdu += 1;
      nb_pdu_to_transmit_ch2--;
      data_pdu_tx++;
      // for polling
      if (rlcP->poll_pdu_trigger) {
        rlcP->vt_pdu++;
        if (rlcP->vt_pdu >= rlcP->poll_pdu_trigger) {
          // set poll bit
#    ifdef DEBUG_RLC_AM_POLL
          msg ("[RLC_AM %p] POOL SET BY poll_pdu trigger RETRANS PDU\n", rlcP);
#    endif
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
          rlcP->vt_pdu = 0;
        } else {
          // reset poll bit
          ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 =
            ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & ~RLC_AM_P_STATUS_REPORT_REQUESTED;
        }
      }
#    ifdef DEBUG_MUX
      id = (((u16_t) (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
        ((((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

      msg ("[RLC_AM][RB %d][MUX] RETRANSMIT DATA PDU 0x%04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
           rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
    }

    /******************************************
    *                DATA PDU                 *
    ******************************************/
    if (!(rlcP->protocol_state & RLC_RESET_PENDING_STATE)) {
      segmentation_returned_pdu = 1;
      while (((nb_pdu_to_transmit_ch1) || (nb_pdu_to_transmit_ch2)) && (rlcP->vt_s != rlcP->vt_ms) && (segmentation_returned_pdu)) {

        if ((pdu = rlcP->rlc_segment (rlcP))) {

          rlc_header = (struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]);
          // check polling
          pool_is_set = 0;
          if (rlcP->poll_pdu_trigger) {
            rlcP->vt_pdu++;
            // test every poll_PDU trigger
            if (rlcP->vt_pdu >= rlcP->poll_pdu_trigger) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              ((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              rlcP->vt_pdu = 0;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY poll_pdu trigger\n", rlcP);
#    endif
            }
          }
          // test every poll_SDU trigger
          if ((rlcP->poll_sdu_trigger) && (((struct rlc_am_tx_data_pdu_management *) (pdu->data))->last_pdu_of_sdu)) {
            if (rlcP->vt_sdu >= rlcP->poll_sdu_trigger) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              ((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              rlcP->vt_sdu = 0;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY poll_sdu trigger\n", rlcP);
#    endif
            } else {
              rlcP->vt_sdu += ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->last_pdu_of_sdu;
            }
          }

          if (!pool_is_set) {
            // test last pdu in transmission buffer trigger == last sdu in input buffer in this implementation of RLC AM
            if ((rlcP->last_transmission_pdu_poll_trigger > 0) && (rlcP->nb_sdu == 0)) {
              ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
              ((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
              pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
              msg ("[RLC_AM %p] POOL SET BY last pdu in tr buffer trigger\n", rlcP);
#    endif
            }
            if (!pool_is_set) {
              // test last pdu in retransmission buffer trigger
              if ((rlcP->retransmission_buffer_to_send.head) && (rlcP->retransmission_buffer_to_send.head == rlcP->retransmission_buffer_to_send.tail) && (rlcP->last_retransmission_pdu_poll_trigger)) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
                ((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
                pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
                msg ("[RLC_AM %p] POOL SET BY last pdu in retrans buffer trigger\n", rlcP);
#    endif
              }
              if (!pool_is_set) {
                // test window based trigger
                if (rlcP->poll_window_trigger) {
                  j = (unsigned int) (((rlcP->vt_s + 4096) - rlcP->vt_a) % 4096) * 100 / rlcP->vt_ws;

                  if (j >= rlcP->poll_window_trigger) {
                    ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->rlc_tb_type = RLC_AM_DATA_POLL_PDU_TYPE;
                    ((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;
                    pool_is_set = 1;
#    ifdef DEBUG_RLC_AM_POLL
                    msg ("[RLC_AM %p] POOL SET BY window based trigger J=%d\n", rlcP, j);
#    endif
                  }
                }
              }
            }
          }

          rlc_header->byte1 |= (rlcP->vt_s >> 5);
          rlc_header->byte2 |= ((u8_t) rlcP->vt_s << 3);
          ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sn = rlcP->vt_s;
          insert_into_retransmission_buffer (rlcP, rlcP->vt_s % rlcP->recomputed_configured_tx_window_size, pdu);

          // check if timer based discard configured
          if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_TIMER_BASED_EXPLICIT)) {
            for (index = 0; index < ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->nb_sdu; index++) {
              sdu_index = ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index];
              sdu = rlcP->input_sdus[sdu_index];

              // mark the sdu as invalid except if confirm requested
              if (!((struct rlc_am_tx_sdu_management *) (sdu->data))->confirm) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index] = -1;        // tag sdu as non valid
              }

              ((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus_time += 1;
              if ((((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus_time ==
                   ((struct rlc_am_tx_sdu_management *) (sdu->data))->nb_pdus) &&
                  !(((struct rlc_am_tx_sdu_management *) (sdu->data))->confirm) && (((struct rlc_am_tx_sdu_management *) (sdu->data))->segmented)
                ) {
                ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index] = -1;        // tag sdu as non valid

                // we can remove the sdu if timer based discard and all pdus submitted to lower layers and no confirm running
                free_mem_block (sdu);
                rlcP->input_sdus[sdu_index] = NULL;
#    ifdef DEBUG_RLC_AM_FREE_SDU
                msg ("[RLC_AM][RB %d] MUX FREE_SDU INDEX %d\n", rlcP->rb_id, sdu_index);
#    endif
              } else {
                ((struct rlc_am_tx_sdu_management *) (sdu->data))->last_pdu_sn = rlcP->vt_s;
              }
            }
          } else if ((rlcP->sdu_discard_mode & RLC_SDU_DISCARD_MAX_DAT_RETRANSMISSION)) {
            for (index = 0; index < ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->nb_sdu; index++) {
              // update the sn of the last transmitted pdu were this sdu was segmented
              ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->last_pdu_sn = rlcP->vt_s;
              // register the sn of the pdus where the sdu was segmented
              ((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->
                pdus_index[((struct rlc_am_tx_sdu_management *) (rlcP->input_sdus[((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index]]->data))->nb_pdus_internal_use++] = rlcP->vt_s;
#    ifdef DEBUG_MUX
              msg ("[RLC_AM][RB %d] MUX SDU INDEX %d LINK PDU SN 0x%04X\n", rlcP->rb_id, ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->sdu[index], rlcP->vt_s);
#    endif
            }
          }

          rlcP->vt_s = (rlcP->vt_s + 1) & SN_12BITS_MASK;


          copy_pdu = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation) + GUARD_CRC_LIH_SIZE);
          if (copy_pdu) {
            memcpy (copy_pdu->data, pdu->data, rlcP->pdu_size + sizeof (struct rlc_am_tx_data_pdu_allocation));
#    ifdef DEBUG_MUX
            rlc_am_display_data_pdu7(copy_pdu);
#    endif
            ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
            ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
            ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
            ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
            ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
            ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;

            if ((nb_pdu_to_transmit_ch1)) {
              list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
                      rlcP->stat_tx_data_pdu += 1;
              nb_pdu_to_transmit_ch1--;
            } else if ((nb_pdu_to_transmit_ch2)) {
              list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch2);
                      rlcP->stat_tx_data_pdu += 1;
              nb_pdu_to_transmit_ch2--;
            }
            data_pdu_tx++;


#    ifdef DEBUG_MUX
            id = (((u16_t) (((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
              ((((struct rlc_am_pdu_header *) (&pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);
            msg ("[RLC_AM][RB %d][MUX] TX DATA PDU 0x%04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
                 rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
            /*        for (index = 0; index < 32 ; index++) {
               msg("%02X.", pdu->data[index]);
               }
               msg("\n");
             */
#    endif
          } else {
            msg ("[RLC_AM][RB %d][MUX] OUT OF MEMORY \n", rlcP->rb_id);
          }
        } else {                // segmentation returned no PDU
          segmentation_returned_pdu = 0;
        }
      }
    }
    // From 3GPP TS 25.322 V5.0.0 (2001-09)
    // -        if a poll has been triggered by either the poll triggers "Poll timer" or "Timer based" (see subclause 9.7.1);
    // AND
    // -        if no AMD PDU is scheduled for transmission or retransmission:
    //     -    if the value of "Configured_Tx_Window_Size" is larger than or equal to "2048":
    //         -        select the AMD PDU with "Sequence Number" equal to VT(S)-1.
    //     -    otherwise if the "Configured_Tx_Window_Size" is less than "2048";
    //         -        select the AMD PDU with "Sequence Number" equal to VT(S)-1; or
    //         -        select an AMD PDU that has not yet been acknowledged by the peer entity;
    //     -    schedule the selected AMD PDU for retransmission (in order to transmit a poll).

    // The Sender may also schedule an AMD PDU for retransmission even if none of the criteria above is fulfilled. In this case, the Sender may:
    // -        if the value of "Configured_Tx_Window_Size" is larger than or equal to "2048":
    //     -    select the AMD PDU with "Sequence Number" equal to VT(S)-1.
    // -        otherwise if the "Configured_Tx_Window_Size" is less than "2048":
    //     -    select the AMD PDU with "Sequence Number" equal to VT(S)-1; or
    //     -    select an AMD PDU that has not yet been acknowledged by the peer entity;
    // -        schedule the selected AMD PDU for retransmission.

    //  TO DO : triggers "Poll timer" or "Timer based"
    if (!(data_pdu_tx)) {
      // this variable last_scheduled is used because a RLC may be scheduled
      // when several DSCH are configured in the RG
      if ((last_scheduled != (unsigned int) Mac_rlc_xface->frame) && (((unsigned int) Mac_rlc_xface->frame) % 48 == 0) ) {

         if (rlcP->configured_tx_window_size < 2048) {
          // rlc_am_get_not_acknowledged_pdu(rlcP);
	   rlc_am_get_not_acknowledged_pdu_optimized (rlcP);
	   //rlc_am_get_not_acknowledged_pdu_vt_s_minus_1 (rlcP);  // LG JUST FOR TEST
        } else if (rlcP->vt_s != rlcP->vt_a) {
	   rlc_am_get_not_acknowledged_pdu_optimized (rlcP);//rlc_am_get_not_acknowledged_pdu_vt_s_minus_1 (rlcP);
        }
      }
      last_scheduled = (unsigned int) Mac_rlc_xface->frame;

      // each retransmission may send RLC in RESET STATE if no discard is configured
      copy_pdu = list2_remove_head (&rlcP->retransmission_buffer_to_send);

      if ((copy_pdu)) {
        // unlink the pdu in retransmission buffer and its copy (mechanism avoiding multi-retransmission of the same
        // pdu in the same queue)
        pdu = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->copy;
#    ifdef DEBUG_MUX
        rlc_am_display_data_pdu7(copy_pdu);
#    endif
        ((struct rlc_am_tx_data_pdu_management *) (pdu->data))->copy = NULL;

        ((struct mac_tb_req *) (copy_pdu->data))->rlc_tb_type = ((struct rlc_am_tx_data_pdu_management *) (copy_pdu->data))->rlc_tb_type;
        ((struct mac_tb_req *) (copy_pdu->data))->rlc = rlcP;
        ((struct mac_tb_req *) (copy_pdu->data))->rlc_callback = rlc_am_status_report_from_mac;
        ((struct mac_tb_req *) (copy_pdu->data))->data_ptr = &copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)];
        ((struct mac_tb_req *) (copy_pdu->data))->first_bit = 0;
        ((struct mac_tb_req *) (copy_pdu->data))->tb_size_in_bits = rlcP->pdu_size << 3;
        list_add_tail_eurecom (copy_pdu, &rlcP->pdus_to_mac_layer_ch1);
        rlcP->stat_tx_retransmit_pdu += 1;
        nb_pdu_to_transmit_ch1--;
        data_pdu_tx++;
        // for polling
        rlcP->vt_pdu++;
        // set poll bit
        ((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 |= RLC_AM_P_STATUS_REPORT_REQUESTED;

#    ifdef DEBUG_MUX
        id = (((u16_t) (((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte1 & RLC_AM_SN_1ST_PART_MASK)) << 5) |
          ((((struct rlc_am_pdu_header *) (&copy_pdu->data[sizeof (struct rlc_am_tx_data_pdu_allocation)]))->byte2 & RLC_AM_SN_2ND_PART_MASK) >> 3);

        msg ("[RLC_AM][RB %d][MUX] RETRANSMIT DATA PDU 0x%04X   VT(A) 0x%03X VT(S) 0x%03X VT(MS) 0x%03X VR(R) 0x%03X VR(MR) 0x%03X\n",
             rlcP->rb_id, id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vr_r, rlcP->vr_mr);
#    endif
      }
    }
  }
}
#endif
