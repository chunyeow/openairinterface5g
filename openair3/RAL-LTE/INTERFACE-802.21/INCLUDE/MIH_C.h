/*******************************************************************************
 *
 * Eurecom OpenAirInterface 3
 * Copyright(c) 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/
/*! \file MIH_C.h
 * \brief This file must be the only file to be included in code dealing with the MIH_C interface.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_H__
#    define __MIH_C_H__
#include "MIH_C_bit_buffer.h"
#include "MIH_C_log.h"
#include "MIH_C_Link_Constants.h"
#include "MIH_C_Types.h"
#include "MIH_C_header_codec.h"
#include "MIH_C_F1_basic_data_types_codec.h"
#include "MIH_C_F2_general_data_types_codec.h"
#include "MIH_C_F3_data_types_for_address_codec.h"
#include "MIH_C_F4_data_types_for_links_codec.h"
#include "MIH_C_F9_data_types_for_qos_codec.h"
#include "MIH_C_F13_data_types_for_information_elements_codec.h"
#include "MIH_C_L2_type_values_for_tlv_encoding.h"
#include "MIH_C_Medieval_extensions.h"
#include "MIH_C_Link_Primitives.h"
#include "MIH_C_Link_Messages.h"
#include "MIH_C_primitive_codec.h"
#include "MIH_C_msg_codec.h"

void MIH_C_init(int log_outputP);
void MIH_C_exit(void);

#define DEBUG_TRACE_DETAILS
#endif
