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
#    ifndef __RLC_TM_PROTO_EXTERN_H__
#        define __RLC_TM_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "platform_types.h"
#        include "platform_constants.h"
//-----------------------------------------------------------------------------
#        include "list.h"
#        include "rlc_tm_entity.h"
#        include "mac_primitives.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     display_protocol_vars_rlc_tm (struct rlc_tm_entity *rlcP);
extern u32_t      rlc_tm_get_buffer_occupancy (struct rlc_tm_entity *rlcP, u8_t logical_channelsP);
extern void     init_rlc_tm (struct rlc_tm_entity *rlcP);
extern void    *rlc_tm_tx (void *argP);
extern void     rlc_tm_rx_no_segment (void *argP, struct mac_data_ind data_indP);
extern void     rlc_tm_rx_segment (void *argP, struct mac_data_ind data_indP);
extern void     send_rlc_tm_control_primitive (struct rlc_tm_entity *rlcP, module_id_t module_idP, mem_block_t *cprimitiveP);

extern struct mac_status_resp rlc_tm_mac_status_indication (void *rlcP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_data_req rlc_tm_mac_data_request (void *rlcP);
extern void     rlc_tm_mac_data_indication (void *rlcP, struct mac_data_ind data_indP);
extern void     rlc_tm_data_req (void *rlcP, mem_block_t *sduP);
#    endif
