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

/*
                                 platform_types.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PLATFORM_TYPES_H__
#    define __PLATFORM_TYPES_H__

#ifdef USER_MODE
#include <stdint.h>
#endif

//-----------------------------------------------------------------------------
// GENERIC TYPES
//-----------------------------------------------------------------------------
typedef signed char        boolean_t;

#if !defined(TRUE)
#define TRUE               (boolean_t)0x01
#endif

#if !defined(FALSE)
#define FALSE              (boolean_t)0x00
#endif

#define BOOL_NOT(b) (b^TRUE)

//-----------------------------------------------------------------------------
// GENERIC ACCESS STRATUM TYPES
//-----------------------------------------------------------------------------
typedef int32_t               sdu_size_t;
typedef uint32_t              frame_t;
typedef int32_t               sframe_t;
typedef uint32_t              sub_frame_t;
typedef uint8_t               module_id_t;
typedef uint16_t              ue_id_t;
typedef int16_t               smodule_id_t;
typedef uint16_t              rb_id_t;
typedef uint16_t              srb_id_t;

typedef boolean_t             MBMS_flag_t;
#define  MBMS_FLAG_NO         FALSE
#define  MBMS_FLAG_YES        TRUE

typedef boolean_t             eNB_flag_t;
#define  ENB_FLAG_NO          FALSE
#define  ENB_FLAG_YES         TRUE

typedef boolean_t             srb_flag_t;
#define  SRB_FLAG_NO          FALSE
#define  SRB_FLAG_YES         TRUE

typedef enum link_direction_e {
    UNKNOWN_DIR          = 0,
    DIR_UPLINK           = 1,
    DIR_DOWNLINK         = 2
} link_direction_t;

typedef enum rb_type_e {
    UNKNOWN_RADIO_BEARER        = 0,
    SIGNALLING_RADIO_BEARER     = 1,
    RADIO_ACCESS_BEARER         = 2
} rb_type_t;

//-----------------------------------------------------------------------------
// PHY TYPES
//-----------------------------------------------------------------------------
typedef uint8_t            crc8_t;
typedef uint16_t           crc16_t;
typedef uint32_t           crc32_t;
typedef unsigned int       crc_t;

//-----------------------------------------------------------------------------
// MAC TYPES
//-----------------------------------------------------------------------------
typedef sdu_size_t         tbs_size_t;
typedef sdu_size_t         tb_size_t;
typedef unsigned int       logical_chan_id_t;
typedef unsigned int       num_tb_t;

//-----------------------------------------------------------------------------
// RLC TYPES
//-----------------------------------------------------------------------------
typedef unsigned int       mui_t;
typedef unsigned int       confirm_t;
typedef unsigned int       rlc_tx_status_t;
typedef int16_t            rlc_sn_t;
typedef uint16_t           rlc_usn_t;
typedef int32_t            rlc_buffer_occupancy_t;
typedef signed int         rlc_op_status_t;

//-----------------------------------------------------------------------------
// PDCP TYPES
//-----------------------------------------------------------------------------
typedef uint16_t           pdcp_sn_t;
typedef uint32_t           pdcp_hfn_t;
typedef int16_t            pdcp_hfn_offset_t;

typedef enum pdcp_transmission_mode_e {
   PDCP_TRANSMISSION_MODE_UNKNOWN     = 0,
   PDCP_TRANSMISSION_MODE_CONTROL     = 1,
   PDCP_TRANSMISSION_MODE_DATA        = 2,
   PDCP_TRANSMISSION_MODE_TRANSPARENT = 3
} pdcp_transmission_mode_t;
//-----------------------------------------------------------------------------
// IP DRIVER / PDCP TYPES
//-----------------------------------------------------------------------------
typedef uint16_t           tcp_udp_port_t;
typedef enum  ip_traffic_type_e {
       TRAFFIC_IPVX_TYPE_UNKNOWN    =  0,
       TRAFFIC_IPV6_TYPE_UNICAST    =  1,
       TRAFFIC_IPV6_TYPE_MULTICAST  =  2,
       TRAFFIC_IPV6_TYPE_UNKNOWN    =  3,
       TRAFFIC_IPV4_TYPE_UNICAST    =  5,
       TRAFFIC_IPV4_TYPE_MULTICAST  =  6,
       TRAFFIC_IPV4_TYPE_BROADCAST  =  7,
       TRAFFIC_IPV4_TYPE_UNKNOWN    =  8
} ip_traffic_type_t;

//-----------------------------------------------------------------------------
// RRC TYPES
//-----------------------------------------------------------------------------
typedef uint32_t           mbms_session_id_t;
typedef uint16_t           mbms_service_id_t;
typedef uint16_t           rnti_t;
typedef enum config_action_e {
    CONFIG_ACTION_NULL              = 0,
    CONFIG_ACTION_ADD               = 1,
    CONFIG_ACTION_REMOVE            = 2,
    CONFIG_ACTION_MODIFY            = 3,
    CONFIG_ACTION_SET_SECURITY_MODE = 4,
    CONFIG_ACTION_MBMS_ADD          = 10,
    CONFIG_ACTION_MBMS_MODIFY       = 11
} config_action_t;

//-----------------------------------------------------------------------------
// GTPV1U TYPES
//-----------------------------------------------------------------------------
typedef uint32_t           teid_t; // tunnel endpoint identifier
typedef uint8_t            ebi_t;  // eps bearer id
#endif
