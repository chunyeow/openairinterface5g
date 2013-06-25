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
/*! \file rlc_tm_structs.h
* \brief This file defines structures used inside the RLC TM.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
* \note
* \bug
* \warning
*/
#    ifndef __RLC_TM_STRUCTS_H__
#        define __RLC_TM_STRUCTS_H__
#        include "platform_types.h"
#        include "list.h"
#        include "mac_primitives.h"
#        include "rlc_primitives.h"
#        include "mac_rlc_primitives.h"
//-----------------------
struct rlc_tm_tx_sdu_management {
  u8_t             *first_byte;
  s32_t             sdu_creation_time;
  u16_t             sdu_size;
};
//-----------------------
struct rlc_tm_tx_pdu_management {
  u8_t             *first_byte;
  u8_t              dummy[MAC_HEADER_MAX_SIZE];
};
//-----------------------
struct rlc_tm_rx_pdu_management {
  u8_t             *first_byte;
};
//-----------------------
struct rlc_tm_tx_data_pdu_struct {
  union {
    struct rlc_tm_tx_pdu_management tx_pdu_mngmnt;
    struct mac_tb_req tb_req;
    struct mac_tx_tb_management tb_mngt;
#        ifdef BYPASS_L1
    struct rlc_tm_rx_pdu_management dummy1;
    struct mac_tb_ind dummy2;
    struct mac_rx_tb_management dummy3;
    struct rlc_indication dummy4;
#        endif
  } dummy;
  u8_t              data[1];
};
//-----------------------
struct rlc_tm_data_req_alloc {  // alloc enought bytes for sdu mngt also
  union {
    struct rlc_tm_data_req dummy1;
    struct rlc_tm_tx_sdu_management dummy2;
  } dummy;
};
#    endif
