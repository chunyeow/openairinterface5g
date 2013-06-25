/***************************************************************************
                          rlc_am_entity.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_ENTITY_H__
#        define __RLC_AM_ENTITY_H__
//-----------------------------------------------------------------------------
#        include "mem_block.h"
#        include "rlc_am_structs.h"
#        include "rlc_def.h"
#        include "platform_types.h"
#        include "platform_constants.h"
//-----------------------------------------------------------------------------
struct rlc_am_entity {
  module_id_t    module_id;
  // for stats and trace purpose :
  u16_t             data_plane;   // act as a boolean

  u16_t              rb_id;
  //-----------------------------
  // polling info
  //-----------------------------
  u16_t             poll_pdu_trigger;
  u16_t             poll_sdu_trigger;
  u16_t             timer_poll_trigger;
  u16_t             timer_poll_prohibit_trigger;
  u8_t              last_transmission_pdu_poll_trigger;
  u8_t              last_retransmission_pdu_poll_trigger;
  u8_t              poll_window_trigger;
  //mem_block_t      *timer_rst;
  u16_t             time_out_events;
  //mem_block_t      *timer_mrw;    // if NULL : no timer is running
  //u8_t timer_mrw_is_running;


  u16_t             timer_rst_init;
  u16_t             timer_mrw_init;

  u32_t             transmitted_pdu_types;
  int                last_tx_status_frame;

  u32_t            *frame_tick_milliseconds;      // pointer on this tick variable handled by RRC : READ ONLY

  u8_t              missing_pdu_indicator;
  //-----------------------------
  // tranmission
  //-----------------------------

  //struct cnt_dbl_lk_list_up  segmentation_buffer;  // output of segmentation/concatenation function
  list2_t         sdu_conf_segmented;   // contain sdu_headers:sdu processed by the segmentation unit. (we keep them for confirm)
  list2_t         sdu_discard_segmented;        // contain sdu_headers:sdu processed by the segmentation unit. (we keep them for discard)
  list2_t         sdu_discarded;        // contain sdu_headers

  list2_t         discard_procedures;   // contain procedures (struct rlc_am_discard_procedure)

  //struct cnt_dbl_lk_list_up  transmission_buffer; // output of mux module

  //u16_t                        data_pdu_size;
  //u16_t                        control_pdu_size;



  //struct cnt_list_up        dcch_pdus_to_mac_layer;
  //struct cnt_list_up        dtch_pdus_to_mac_layer;

  //struct list_up            dcch_pdus_from_mac_layer;
  //struct list_up            dtch_pdus_from_mac_layer;

  // implementation specific: our transmiter buffer is an array whose size must be a power of 2
#        define RLC_AM_DISCARD_REASSEMBLY_AT_LI_INDEX_0  0x00
  u8_t              discard_reassembly_after_li;  // when received mrw sufi
  u16_t             discard_reassembly_start_sn;
  //-----------------------------
  // management of received PDU for piggybacked status PDU and status PDU
  //-----------------------------
  struct sufi_to_insert_in_status status_in_construction[NB_MAX_SUFI];
  struct sufi_ack ack;
  u8_t              sufi_to_insert_index;
  //-----------------------------
  // Reset
  //-----------------------------
  u8_t              send_status_pdu_requested;
  u8_t              reset_sequence_number;
  u8_t              last_received_rsn;
  u8_t              max_rst;
  //-----------------------------
  // Mapping info
  //-----------------------------
  u8_t              dcch_logical_channel_identity;
  u8_t              dtch_logical_channel_identity;
  u8_t              nb_logical_channels_per_rlc;
  //-----------------------------
  // buffer occupancy measurements sent to MAC
  //-----------------------------
  // note occupancy of other buffers is deducted from nb elements in lists
  u32_t             buffer_occupancy_retransmission_buffer;       // nb of pdus

  //**************************************************************
  // new members
  //**************************************************************
  u8_t              allocation;
  u8_t              location;     // UTRAN/UE
  u8_t              protocol_state;
  //-----------------------------
  // STATE VARIABLES
  //-----------------------------
  u16_t             vt_s;         // send state variable
  // This state variable contains the "Sequence Number" of the next AMD PDU to 
  // be transmitted for the first time (i.e. excluding retransmitted PDUs). It 
  // shall be updated after the aforementioned AMD PDU is transmitted or after 
  // transmission of a MRW SUFI which includes SN_MRWLENGTH >VT(S) 
  // (see subclause 11.6). 
  // The initial value of this variable is 0
  
  
  u16_t             vt_a;         // Acknowledge state variable
  // This state variable contains the "Sequence Number" following the "Sequence 
  // Number" of the last in-sequence acknowledged AMD PDU. This forms the lower 
  // edge of the transmission window of acceptable acknowledgements. VT(A) shall
  // be updated based on the receipt of a STATUS PDU including an ACK (see 
  // subclause 9.2.2.11.2) and/or an MRW_ACK SUFI (see subclause 11.6).
  // The initial value of this variable is 0. For the purpose of initialising 
  // the protocol, this value shall be assumed to be the first "Sequence Number"
  // following the last in-sequence acknowledged AMD PDU.
	
  
  u16_t             vt_ms;        // Maximum send state variable
  // This state variable contains the "Sequence Number" of the first AMD PDU 
  // that can be rejected by the peer Receiver, VT(MS) = VT(A) + VT(WS). 
  // This value represents the upper edge of the transmission window. The 
  // transmitter shall not transmit AMD PDUs with "Sequence Number" >= VT(MS) 
  // unless VT(S) >= VT(MS). In that case, the AMD PDU with 
  // "Sequence Number" = VT(S) - 1 can also be transmitted. VT(MS) shall be 
  // updated when VT(A) or VT(WS) is updated.
  // The initial value of this variable is Configured_Tx_Window_size.
  
  
  u16_t             vt_pdu;
  // This state variable is used when the "poll every Poll_PDU PDU" polling 
  // trigger is configured. It shall be incremented by 1 for each AMD PDU that 
  // is transmitted including both new and retransmitted AMD PDUs. When it 
  // becomes equal to the value Poll_PDU, a new poll shall be transmitted and 
  // the state variable shall be set to zero.
  // The initial value of this variable is 0.
  
  
  u16_t             vt_sdu;
  // This state variable is used when the "poll every Poll_SDU SDU" polling 
  // trigger is configured. It shall be incremented by 1 for a given SDU when 
  // the AMD PDU carrying the first segment of this SDU is scheduled to be 
  // transmitted for the first time. When it becomes equal to the value Poll_SDU
  // a new poll shall be transmitted and the state variable shall be set to 
  // zero. The "Polling bit" shall be set to "1" in the first transmission of 
  // the AMD PDU that contains the last segment of an RLC SDU (indicated either 
  // by the "Length Indicator" indicating the end of the SDU or by the special 
  // value of the HE field).
  // The initial value of this variable is 0.
	
	
  u16_t             vt_rst;       // Reset state variable
  // This state variable is used to count the number of times a RESET PDU is 
  // scheduled to be transmitted before the reset procedure is completed. 
  // VT(RST) shall be incremented by 1 according to subclauses 11.4.2 and 
  // 11.4.5.1. VT(RST) shall only be reset upon the reception of a RESET ACK 
  // PDU ( i.e. VT(RST) shall not be reset when an RLC reset initiated by the 
  // peer RLC entity occurs) unless otherwise specified in subclause 9.7.7.
  // The initial value of this variable is 0.
	
	
  u16_t             vt_mrw;       // MRW command send state variable
  // This state variable is used to count the number of times a MRW command is 
  // transmitted. VT(MRW) is incremented by 1 each time a timer Timer_MRW 
  // expires. VT(MRW) shall be reset when the SDU discard with explicit 
  // signalling procedure is terminated. 
  // The initial value of this variable is 0. 
  
  
  u16_t             vt_ws;        // transmitter window size state variable
  // This state variable contains the size that shall be used for the 
  // transmission window. VT(WS) shall be set equal to the WSN field when the 
  // transmitter receives a STATUS PDU including a WINDOW SUFI.
  // The initial value of this variable is Configured_Tx_Window_size.
	
	
  u16_t             vr_r;         // Receive state variable
  // This state variable contains the "Sequence Number" following that of the 
  // last in-sequence AMD PDU received. It shall be updated upon the receipt of 
  // the AMD PDU with "Sequence Number" equal to VR(R).
  // The initial value of this variable is 0. For the purpose of initialising 
  // the protocol, this value shall be assumed to be the first "Sequence Number"
  // following the last in-sequence received AMD PDU.
	
	
  u16_t             vr_h;         // Highest expected state variable
  // This state variable contains the "Sequence Number" following the highest 
  // "Sequence Number" of any AMD PDU received or identified to be missing.. 
  // When a AMD PDU is received with "Sequence Number" x or a POLL SUFI is 
  // received with POLL_SN=x such that VR(H)<=x<VR(MR), this state variable 
  // shall be set equal to x+1.
  // The initial value of this variable is 0.
	
	
  u16_t             vr_mr;        // Maximum acceptable Receive state variable
  // This state variable contains the "Sequence Number" of the first AMD PDU 
  // that shall be rejected by the Receiver, 
  // VR(MR) = VR(R) + Configured_Rx_Window_Size..
  

  //-----------------------------
  // TIMERS
  //-----------------------------
  list2_t         rlc_am_timer_list;
  
  signed int        timer_poll;
  signed int        timer_poll_init;
  //This timer shall only be used when so configured by upper layers. The value 
  // of the timer is signalled by upper layers. In the UE this timer shall be 
  // started (or restarted) when the successful or unsuccessful transmission of 
  // a poll is indicated by lower layer. In UTRAN it should be started when a 
  // poll is submitted to lower layer. If x is the value of the state variable 
  // VT(S) after the poll was submitted to lower layer, the timer shall be 
  // stopped upon receiving:
  // -	positive acknowledgements for all the AMD PDUs with "Sequence Number" 
  //    up to and including x - 1; or
  // -	a negative acknowledgement for the AMD PDU with 
  //    "Sequence Number" = x - 1.
  // If the timer expires and no STATUS PDU fulfilling the criteria above has 
  // been received:
  // -	the Receiver shall be polled once more;
  // -	the timer shall be restarted; and
  // -	the new value of VT(S) shall be saved.
  // If a new poll is sent when the timer is active, the timer shall be 
  // restarted at the time specified above, and the value of VT(S) shall be saved.

  signed int        timer_poll_prohibit;
  signed int        timer_poll_prohibit_init;
  // This timer shall only be used when so configured by upper layers. It is 
  // used to prohibit transmission of polls within a certain period. The value 
  // of the timer is signalled by upper layers.
  // In the UE this timer shall be started (or restarted) when the successful or
  // unsuccessful transmission of a poll is indicated by lower layer. In UTRAN 
  // it should be started when a poll is submitted to lower layer.
  // From the time a poll is triggered until the timer expires, polling is 
  // prohibited. If another poll is triggered while polling is prohibited, its 
  // transmission shall be delayed until the timer expires 
  // (see subclause 9.7.1). Only one poll shall be transmitted when 
  // Timer_Poll_Prohibit expires even if several polls were triggered in the 
  // meantime. This timer shall not be affected by the reception of STATUS PDUs.
  // When Timer_Poll_Prohibit is not configured by upper layers, polling is 
  // never prohibited.
  
  
  signed int        timer_discard;
  signed int        timer_discard_init;
  // This timer shall be used when timer-based SDU discard is configured by 
  // upper layers. The value of the timer is signalled by upper layers. In the 
  // transmitter, a new timer is started upon reception of an SDU from upper 
  // layer.
  // ...In AM, if a timer expires before the corresponding SDU is acknowledged, 
  // "SDU discard with explicit signalling" specified in subclause 11.6 shall be
  // initiated.
  
  
  signed int        timer_poll_periodic;
  signed int        timer_poll_periodic_init;
  // This timer shall only be used when "timer based polling" is configured by 
  // upper layers. The value of the timer is signalled by upper layers. The 
  // timer shall be started when the RLC entity is created. When the timer 
  // expires, the RLC entity shall:
  // -	restart the timer;
  // -	if AMD PDUs are available for transmission or retransmission (not yet 
  //    acknowledged):
  //     -	trigger a poll.


  signed int        timer_status_prohibit;
  signed int        timer_status_prohibit_init;
  // This timer shall only be used when so configured by upper layers. It is 
  // meant to prohibit the Receiver from sending consecutive acknowledgement 
  // status reports. A status report is an acknowledgement status report if it 
  // contains any of the SUFIs LIST, BITMAP, RLIST or ACK. The value of the 
  // timer is signalled by upper layers.
  // In the UE, this timer shall be started (or restarted) when the successful 
  // or unsuccessful transmission of the last STATUS PDU of an acknowledgement 
  // status report is indicated by lower layer. In UTRAN it should be started 
  // when the last STATUS PDU of an acknowledgement status report is submitted 
  // to lower layer.
  //   From the time an acknowledgement status report is triggered until the 
  // Timer_Status_Prohibit timer expires, acknowledgement is prohibited. If 
  // another such status report is triggered while acknowledgement is 
  // prohibited, its transmission shall be delayed until the timer expires 
  // (see subclause 9.7.2). The status report may be updated during this time. 
  // The transmission of SUFIs MRW, MRW_ACK, WINDOW or NO_MORE is not restricted.
  // When Timer_Status_Prohibit is not configured by upper layers, 
  // acknowledgment is not prohibited.
  
  
  signed int        timer_status_periodic;
  signed int        timer_status_periodic_init;
  // This timer shall only be used when timer based status reporting is 
  // configured by upper layers.
  // This timer shall be started when the RLC entity is created. When the timer 
  // expires the transmission of a status report shall be triggered and the 
  // timer shall be restarted. This timer can be blocked by upper layers. The 
  // timer shall be restarted when upper layers indicate that it is no longer 
  // blocked.
  
  
  signed int        timer_rst;
  signed int        timer_rst_init;
  // This timer is meant to handle the loss of a RESET PDU by the peer entity, 
  // or the loss of a RESET ACK PDU from the peer entity. The value of the timer
  // is signalled by upper layers.
  // In the UE this timer shall be started (or restarted) when the successful or
  // unsuccessful transmission of a RESET PDU is indicated by lower layer. In 
  // UTRAN it should be started when a RESET PDU is submitted to lower layer.
  // Timer_RST shall only be stopped upon reception of a RESET ACK PDU (with 
  // same RSN as RESET PDU), i.e. this timer shall not be stopped when an RLC 
  // reset initiated by the peer RLC entity occurs. If this timer expires, the 
  // RESET PDU shall be retransmitted.
  
  
  signed int        timer_mrw;
  signed int        timer_mrw_init;
  // This timer is used to trigger the retransmission of a status report 
  // containing an MRW SUFI field. The value of the timer is signalled by upper 
  // layers.
  // In the UE this timer shall be started (or restarted) when the successful or
  // unsuccessful transmission of a STATUS PDU containing the MRW SUFI is 
  // indicated by lower layer. In UTRAN, it should be started when a STATUS PDU 
  // containing the MRW SUFI is submitted to lower layer.
  // Each time the timer expires the MRW SUFI is retransmitted. It shall be 
  // stopped when one of the termination criteria for the SDU discard with 
  // explicit signalling procedure is fulfilled (see subclause 11.6.4). 
  

  //-----------------------------
  // PROTOCOL PARAMETERS
  //-----------------------------
  u16_t             max_dat;  
  // The maximum number of transmissions of an AMD PDU is equal to MaxDAT – 1.
  // This protocol parameter represents the upper limit for state variable 
  // VT(DAT). When VT(DAT) equals the value MaxDAT, either RLC RESET procedure 
  // or SDU discard procedure shall be initiated according to the configuration 
  // by upper layers.
  
  u16_t             poll_pdu;  
  // This protocol parameter indicates how often the transmitter shall poll the 
  // Receiver in the case where "polling every Poll_PDU PDU" is configured by 
  // upper layers. It represents the upper limit for the state variable VT(PDU).
  // When VT(PDU) equals the value Poll_PDU a poll shall be transmitted to the 
  // peer entity.
  
  u16_t             poll_sdu;  
  // This protocol parameter indicates how often the transmitter shall poll the 
  // Receiver in the case where "polling every Poll_SDU SDU" is configured by 
  // upper layers. It represents the upper limit for state variable VT(SDU). 
  // When VT(SDU) equals the value Poll_SDU a poll shall be transmitted to the 
  // peer entity.
  
  
  u16_t             poll_window;  
  // This protocol parameter indicates when the transmitter shall poll the 
  // Receiver in the case where "window-based polling" is configured by upper 
  // layers. A poll is triggered for each AMD PDU when J >= Poll_Window, where 
  // J is the transmission window percentage.(see formula of J in spec document)
  
  
  u16_t             max_rst;  
  // The maximum number of transmissions of a RESET PDU is equal to MaxRST – 1. 
  // This protocol parameter represents the upper limit for state variable 
  // VT(RST). When VT(RST) equals the value MaxRST, unrecoverable error shall be
  // indicated to upper layers.
  
  
  u16_t             configured_tx_window_size;
  // This protocol parameter indicates both the maximum allowed transmission 
  // window size and the value for the state variable VT(WS).
  
  
  //u16_t             configured_rx_window_size;
  // This protocol parameter indicates the reception window size. This parameter
  // is applicable both for RLC UM and AM. This parameter is only configured for
  // RLC-UM in case out-of-sequence reception is supported.
  
  
  u16_t             max_mrw;
  // The maximum number of transmissions of an MRW command is equal to MaxMRW. 
  // This protocol parameter represents the upper limit for state variable 
  // VT(MRW). When VT(MRW) equals the value MaxMRW, the RLC RESET procedure 
  // shall be initiated.
            
      
  u16_t             first_li_in_next_pdu; // indicates :
  // value = 000000000000000 that the previous PDU was exactly
  // with the last segment of an RLC SDU and there is no LI that
  // indicates the end of the SDU in the previous RLC PDU.
  // value = 111111111111011 The last segment of an RLC SDU was one octet
  // short of exactly filling the previous RLC PDU and there is no LI that
  // indicates the end of the SDU in the previous RLC PDU. The remaining one
  // octet in the previous RLC PDU is ignored.
  // value = 111111111111110 AMD PDU: The rest of the RLC PDU includes a
  // piggybacked STATUS PDU.
  // value = 111111111111111 The rest of the RLC PDU is padding. The padding
  // length can be zero.



  //-----------------------------
  // C-SAP
  //-----------------------------
  list_t          c_sap;

  //-----------------------------
  // discard
  //-----------------------------
  u8_t              sdu_discard_mode;
  u8_t              send_mrw;
  //-----------------------------
  // tranmission
  //-----------------------------
  mem_block_t     **input_sdus;   // should be accessed as an array
  mem_block_t      *input_sdus_alloc;     // allocation of the array
  u16_t             size_input_sdus_buffer;
  u16_t             nb_sdu;

  u16_t             next_sdu_index;       // next location of incoming sdu
  u16_t             current_sdu_index;
  u32_t             buffer_occupancy;
  // for segmentation procedures (optimization save space on stack)
  u16_t             li[RLC_AM_SEGMENT_NB_MAX_LI_PER_PDU];



  // implementation specific: our transmiter buffer is an array whose size must be a power of 2
  u16_t             recomputed_configured_tx_window_size;

  mem_block_t      *(*rlc_segment) (struct rlc_am_entity * rlcP);

  mem_block_t      *retransmission_buffer_alloc;
  mem_block_t     **retransmission_buffer;

  list2_t         retransmission_buffer_to_send;        // contains PDUs that must be immediatly retransmitted
  list_t          control;      // contains control pdus

  u16_t             nb_pdu_requested_by_mac_on_ch1;
  u16_t             nb_pdu_requested_by_mac_on_ch2;
  u16_t             pdu_size;

  u8_t              li_one_byte_short_to_add_in_next_pdu;
  u8_t              li_exactly_filled_to_add_in_next_pdu;

  list_t          pdus_to_mac_layer_ch1;
  list_t          pdus_to_mac_layer_ch2;

  //-----------------------------
  // receiver
  //-----------------------------
  sdu_size_t       output_sdu_size_to_write;
  mem_block_t      *output_sdu_in_construction;

  mem_block_t      *receiver_buffer_alloc;
  mem_block_t     **receiver_buffer;

  u16_t             last_reassemblied_sn;
  u16_t             configured_rx_window_size;
  u16_t             recomputed_configured_rx_window_size;

  list_t          pdus_from_mac_layer_ch1;
  list_t          pdus_from_mac_layer_ch2;

  //-----------------------------
  // status generation
  //-----------------------------
  mem_block_t      *holes_alloc;
  struct rlc_am_hole *holes;
  u16_t             nb_missing_pdus;
  u16_t             ack_sn;
  u16_t             nb_holes;
#        ifdef BYPASS_L1
  unsigned int    pdu_counter;
#        endif


  unsigned int stat_tx_pdcp_sdu;
  unsigned int stat_tx_pdcp_sdu_discarded;
  unsigned int stat_tx_retransmit_pdu_unblock;
  unsigned int stat_tx_retransmit_pdu_by_status;
  unsigned int stat_tx_retransmit_pdu;  
  unsigned int stat_tx_data_pdu;  
  unsigned int stat_tx_control_pdu;  
  
  unsigned int stat_rx_sdu;  
  unsigned int stat_rx_error_pdu;  
  unsigned int stat_rx_data_pdu;  
  unsigned int stat_rx_data_pdu_out_of_window;  
  unsigned int stat_rx_control_pdu;  
};
#    endif
