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
#include "UTIL/LOG/log.h"
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

void MIH_C_init(void);
void MIH_C_exit(void);

#define DEBUG_TRACE_DETAILS
#endif
