/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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

/*! \file pdcp_util.h
* \brief PDCP Util Methods
* \author Baris Demiray
* \date 2011
*/

#ifndef PDCP_UTIL_H
#define PDCP_UTIL_H

#include "UTIL/LOG/log.h"
#include "../../COMMON/platform_types.h"
#include "pdcp.h"

/*
 * Prints incoming byte stream in hexadecimal and readable form
 *
 * @param component Utilised as with macros defined in UTIL/LOG/log.h
 * @param data unsigned char* pointer for data buffer
 * @param size Number of octets in data buffer
 * @return none
 */
void util_print_hex_octets(comp_name_t component, unsigned char* data, unsigned long size);

/*
 * Flush incoming byte stream in hexadecimal without formating
 *
 * @param component Utilised as with macros defined in UTIL/LOG/log.h
 * @param data unsigned char* pointer for data buffer
 * @param size Number of octets in data buffer
 * @return none
 */
void util_flush_hex_octets(comp_name_t component, unsigned char* data, unsigned long size);

/*
 * Prints binary representation of given octet prepending 
 * passed log message
 *
 * @param Octet as an unsigned character
 * @return None
 */
void util_print_binary_representation(unsigned char* message, u8_t octet);

/*
 * Sets the bit of `octet' at index `index'
 *
 * @param octet Octet
 * @param index Index
 * @return TRUE on success, FALSE otherwise
 */
BOOL util_mark_nth_bit_of_octet(u8_t* octet, u8_t index);

#endif // PDCP_UTIL_H
