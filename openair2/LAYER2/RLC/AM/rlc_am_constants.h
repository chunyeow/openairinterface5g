/***************************************************************************
                          rlc_am_constant.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_CONSTANT_H__
#        define __RLC_AM_CONSTANT_H__

#        define RLC_AM_LOCATION_UTRAN                    0xBA
#        define RLC_AM_LOCATION_UE                       0x01

//----------------------------------------------------------
// AMD DATA, CONTROL PDU parameters
//----------------------------------------------------------
#        define NB_MAX_SUFI                              30
#        define SUFI_MAX_SIZE                            40
// SUFI field (4 bits)
#        define RLC_AM_SUFI_NO_MORE                      0x0
#        define RLC_AM_SUFI_WINDOW                       0x1
#        define RLC_AM_SUFI_ACK                          0x2
#        define RLC_AM_SUFI_LIST                         0x3
#        define RLC_AM_SUFI_BITMAP                       0x4
#        define RLC_AM_SUFI_RLIST                        0x5
#        define RLC_AM_SUFI_MRW                          0x6
#        define RLC_AM_SUFI_MRW_ACK                      0x7

#        define RLC_AM_SUFI_NO_MORE_SIZE                 4
                                                        //in bits
#        define RLC_AM_SUFI_ACK_SIZE                     16
                                                        //in bits
#        define RLC_AM_SUFI_LIST_SIZE_MIN                24
                                                        //in bits
#        define RLC_AM_SUFI_BITMAP_SIZE_MIN              28
                                                        //in bits
//----------------------------------------------------------
// values for ack field of struct rlc_am_tx_pdu_management
// this struct is mapped on the misc field of each pdu
#        define  RLC_AM_PDU_ACK_NO_EVENT                 0
#        define  RLC_AM_PDU_ACK_EVENT                    1
#        define  RLC_AM_PDU_NACK_EVENT                   -1
//----------------------------------------------------------
#        define  RLC_AM_SEND_MRW_OFF                     0x0F
#        define  RLC_AM_SEND_MRW_ON                      0xF0
//----------------------------------------------------------
// SN Field
#        define RLC_AM_SN_1ST_PART_MASK                  0x7F
#        define RLC_AM_SN_2ND_PART_MASK                  0xF8
//----------------------------------------------------------
// Polling bit (values shifted 2 bits left)
#        define RLC_AM_P_STATUS_REPORT_NOT_REQUESTED     0
#        define RLC_AM_P_STATUS_REPORT_REQUESTED         4
#        define RLC_AM_P_STATUS_REPORT_MASK              4
//----------------------------------------------------------
// li field (values shifted 1 bit left)
#        define RLC_AM_SEGMENT_NB_MAX_LI_PER_PDU  32
//----------------------------------------------------------
// shifted 3 bits left
#        define RLC_AM_RESET_SEQUENCE_NUMBER_MASK            0x08

#        define RLC_AM_TIMER_POLL_TIME_OUT_EVENT             0x001
#        define RLC_AM_TIMER_POLL_PROHIBIT_TIME_OUT_EVENT    0x002
#        define RLC_AM_TIMER_EPC_TIME_OUT_EVENT              0x004
#        define RLC_AM_TIMER_DISCARD_TIME_OUT_EVENT          0x008
#        define RLC_AM_TIMER_POLL_PERIODIC_TIME_OUT_EVENT    0x010
#        define RLC_AM_TIMER_STATUS_PROHIBIT_TIME_OUT_EVENT  0x020
#        define RLC_AM_TIMER_STATUS_PERIODIC_TIME_OUT_EVENT  0x040
#        define RLC_AM_TIMER_RST_TIME_OUT_EVENT              0x080
#        define RLC_AM_TIMER_MRW_TIME_OUT_EVENT              0x100
//----------------------------------------------------------
#        define RLC_AM_SDU_SEGMENTS_SUBMITTED_TO_LOWER_LAYER           0xFF
                                                                        // for sdu_header_copy
#        define RLC_AM_SN_INVALID                                      0xFFFF

// PDU transmission
#        define RLC_AM_PDU_COPY_LOCATION_RETRANSMISSION_BUFFER_TO_SEND 0x10
#        define RLC_AM_PDU_COPY_LOCATION_PDUS_TO_MAC_LAYER             0x20
#        define RLC_AM_PDU_COPY_LOCATION_MASK                          0xF0
//----------------------------------------------------------
// Events defined for state model of the acknowledged mode entity
#        define RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT                 0x00
#        define RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT  0x01
#        define RLC_AM_RECEIVE_CRLC_SUSPEND_REQ_EVENT                                 0x10
#        define RLC_AM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT                                0x11
#        define RLC_AM_RECEIVE_CRLC_RESUME_REQ_EVENT                                  0x12
#        define RLC_AM_RECEIVE_RESET_EVENT                                            0x20
#        define RLC_AM_TRANSMIT_RESET_EVENT                                           0x21
#        define RLC_AM_RECEIVE_RESET_ACK_EVENT                                        0x22
#        define RLC_AM_TRANSMIT_RESET_ACK_EVENT                                       0x23
//----------------------------------------------------------
#        define RLC_AM_TRAFFIC_NOT_ALLOWED                                            0x00
#        define RLC_AM_TRAFFIC_ALLOWED_FOR_STATUS                                     0xC0
                                                                                        // mutual exclusion of set bits with next value
#        define RLC_AM_TRAFFIC_ALLOWED_FOR_DATA                                       0x0D
                                                                                        // mutual exclusion of set bits with previous value

#        define RLC_AM_DCCH_ID    0xC0
                                // mutual exclusion of set bits with next value
#        define RLC_AM_DTCH_ID    0x0D
                                // mutual exclusion of set bits with previous value
//----------------------------------------------------------
// for status report of transmission by MAC layer
#        define RLC_AM_STATUS_PDU_TYPE                         0x0001
#        define RLC_AM_FIRST_STATUS_PDU_TYPE                   0x0011
#        define RLC_AM_LAST_STATUS_PDU_TYPE                    0x0021
#        define RLC_AM_MRW_STATUS_PDU_TYPE                     0x0040
#        define RLC_AM_RESET_PDU_TYPE                          0x0080
#        define RLC_AM_RESET_ACK_PDU_TYPE                      0x0100
#        define RLC_AM_DATA_POLL_PDU_TYPE                      0x1800
#        define RLC_AM_DATA_PDU_TYPE                           0x1000
//----------------------------------------------------------
// TIMER EPC
#        define TIMER_EPC_STATE_IDLE                               0x00
#        define TIMER_EPC_STATE_TIMER_ARMED                        0x01
#        define TIMER_EPC_STATE_TIMED_OUT                          0x02
#        define TIMER_EPC_STATE_VR_EP_COUNTING_DOWN                0x04
#        define TIMER_EPC_STATE_VR_EP_EQUAL_ZERO                   0x08

#        define TIMER_EPC_PDU_STATUS_SUBMITTED_LOWER_LAYER_EVENT   0x01
#        define TIMER_EPC_PDU_STATUS_TRANSMITED_EVENT              0x02
#        define TIMER_EPC_TIMER_TIMED_OUT_EVENT                    0x04
#        define TIMER_EPC_VR_EP_EQUAL_ZERO_EVENT                   0x08
#    endif
