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
/*! \file rlc_primitives.h
* \brief This file contains constants definition for RLC primitives.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
#ifndef __RLC_PRIMITIVES_H__
#    define __RLC_PRIMITIVES_H__

#    ifdef USER_MODE
#        include <stdio.h>
#        include <stdlib.h>
#    endif
#    include "platform_types.h"
#    include "platform_constants.h"
#    include "mem_block.h"


#    define RLC_NO_SAP                  RLC_NONE
#    define RLC_AM_SAP                  RLC_MODE_AM
#    define RLC_UM_SAP                  RLC_MODE_UM
#    define RLC_TM_SAP                  RLC_MODE_TM
//----------------------------------------------------------
// primitives
//----------------------------------------------------------
#    define RLC_AM_DATA_REQ                          0x01
#    define RLC_UM_DATA_REQ                          0x02
#    define RLC_TM_DATA_REQ                          0x04

#    define RLC_AM_DATA_IND                          0x08
#    define RLC_UM_DATA_IND                          0x10
#    define RLC_TM_DATA_IND                          0x20

#    define RLC_AM_DATA_CNF                          0x40
//----------------------------------------------------------
// control primitives
//----------------------------------------------------------
#    define CRLC_CONFIG_REQ                          0x81
#    define CRLC_SUSPEND_REQ                         0x82
#    define CRLC_SUSPEND_CONF                        0x84
#    define CRLC_RESUME_REQ                          0x88
#    define CRLC_STATUS_IND                          0x90
//----------------------------------------------------------
// primitives definition
//----------------------------------------------------------
#    define RLC_AM_SDU_MNGT_LAST_PDU_SN_EXTENDS_ABOVE_CONF_TX_WINDOW      0X4000
#    define RLC_AM_SDU_MNGT_LAST_PDU_SN_UNKNOWN                           0X8000
#    define RLC_TX_CONFIRM_SUCCESSFULL                                    0xCF
#    define RLC_TX_CONFIRM_FAILURE                                        0xFA

struct rlc_am_data_req {
  //struct rlc_am_sdu_management sdu_mngt;
  u32_t             mui;
  u16_t             data_size;    // in bytes
  u16_t             data_offset;  // beginning of payload data may not start just after the header (PDCP header compression, etc)
  u8_t              conf;         // confirm of sdu delivery to the peer entity
};

struct rlc_tm_data_req {
  u16_t             data_size;    // in bits
  u16_t             data_offset;  // beginning of payload data may not start just after the header (PDCP header compression, etc)
};


struct rlc_um_data_req {
  u16_t             data_size;    // in bytes
  u16_t             data_offset;  // beginning of payload data may not start just after the header (PDCP header compression, etc)
  u8_t              use_special_li;
};

typedef struct rlc_am_data_req  rlc_am_data_req_t;
typedef struct rlc_tm_data_req  rlc_tm_data_req_t;
typedef struct rlc_um_data_req  rlc_um_data_req_t;

//----------------------------------------------------------
// control primitives definition
//----------------------------------------------------------
#    define RLC_E_R_RELEASE          0x00
#    define RLC_E_R_MODIFICATION     0x01
#    define RLC_E_R_ESTABLISHMENT    0x02
#    define RLC_E_R_RE_ESTABLISHMENT 0x04
#    define RLC_STOP                 0x08
#    define RLC_CONTINUE             0x10

#    define RLC_TM_SEGMENTATION_NOT_ALLOWED            0x00
#    define RLC_TM_SEGMENTATION_ALLOWED                0x01

#    define RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO        0x01
#    define RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_YES       0x02
#    define RLC_TM_DELIVERY_OF_ERRONEOUS_SDU_NO_DETECT 0x04


struct rlc_am_parameters {
  u8_t              rb_id;
  u8_t              e_r;          // reestablishment, release or modification of the rlc
  u8_t              stop;
  u8_t              cont;         // continue
  // timers
  u32_t            *frame_tick_milliseconds;

  u16_t             timer_poll;
  u16_t             timer_poll_prohibit;
  u16_t             timer_epc;
  u16_t             timer_discard;
  u16_t             timer_poll_periodic;
  u16_t             timer_status_prohibit;
  u16_t             timer_status_periodic;
  u16_t             timer_rst;
  u8_t              max_rst;
  u16_t             timer_mrw;

  // protocol_parameters
  u16_t             pdu_size;     // in bits
  u8_t              missing_pdu_indicator;
  u8_t              in_sequence_delivery; // no action : this version always deliver sdus in sequence
  u16_t             max_dat;
  u16_t             poll_pdu;
  u16_t             poll_sdu;
  u8_t              poll_window;  // in percent of the window
  u16_t             configured_tx_window_size;
  u16_t             configured_rx_window_size;
  u16_t             max_mrw;

  u8_t              last_transmission_pdu_poll_trigger;
  u8_t              last_retransmission_pdu_poll_trigger;
  u8_t              sdu_discard_mode;
  u8_t              send_mrw;
};

struct rlc_um_parameters {
  u32_t            *frame_tick_milliseconds;
  u8_t              rb_id;
  u8_t              e_r;          // reestablishment, release or modification of the rlc
  u8_t              stop;
  u8_t              cont;         // continue
  u8_t              sdu_discard_mode;
  u16_t             timer_discard;
  u16_t             size_input_sdus_buffer;
};

struct rlc_tm_parameters {
  u32_t            *frame_tick_milliseconds;
  u8_t              rb_id;
  u8_t              e_r;          // reestablishment, release or modification of the rlc
  u8_t              sdu_discard_mode;
  u8_t              segmentation_indication;
  u8_t              delivery_of_erroneous_sdu;
  u16_t             timer_discard;
  u16_t             size_input_sdus_buffer;
};


struct crlc_config_req {
  union {
    struct rlc_am_parameters am_parameters;
    struct rlc_um_parameters um_parameters;
    struct rlc_tm_parameters tm_parameters;
  } parameters;
};

struct crlc_suspend_conf {
  u16_t             vt_s_or_vt_us;
};

struct crlc_resume_req {
  // no parameters
  u8_t              todo;
};

struct crlc_status_ind {
  u16_t             evc;
};

struct crlc_primitive {
  u8_t              type;
  union {
    struct crlc_config_req c_config_req;
    struct crlc_suspend_conf c_suspend_conf;
    struct crlc_resume_req c_resume_req;
    struct crlc_status_ind c_status_ind;
  } primitive;
};
#endif
