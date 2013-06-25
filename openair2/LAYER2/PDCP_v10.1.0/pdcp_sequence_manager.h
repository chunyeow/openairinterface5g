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

/*! \file pdcp_sequence_manager.h
* \brief PDCP Sequence Numbering Methods
* \author Baris Demiray
* \date 2011
*/

#ifndef PDCP_SEQUENCE_MANAGER_H
#define PDCP_SEQUENCE_MANAGER_H

#include "pdcp.h"

/**
 * Initializes sequence numbering state
 * @param pdcp_entity The PDCP entity to be initialized
 * @return none
 */
BOOL pdcp_init_seq_numbers(pdcp_t* pdcp_entity);
/**
 * Checks if incoming PDCP entitiy pointer and relevant sequence number size is valid
 * @return TRUE (0x01) if it is valid, FALSE (0x00) otherwise
 */
BOOL pdcp_is_seq_num_size_valid(pdcp_t* pdcp_entity);
/**
 * Check if SN number is in the range according to SN size
 * @return TRUE if it is valid, FALSE otherwise
 */
BOOL pdcp_is_seq_num_valid(u16 seq_num, u8 seq_num_size);
/**
 * Returns the maximum allowed sequence number value for given size of SN field
 * @return Max sequence number value
 */
u16 pdcp_calculate_max_seq_num_for_given_size(u8 seq_num_size);
/**
 * Returns the next TX sequence number for given PDCP entity
 */
u16 pdcp_get_next_tx_seq_number(pdcp_t* pdcp_entity);
/**
 * Advances the RX window state of given PDCP entity upon successfull receipt of a SDU
 */
BOOL pdcp_advance_rx_window(pdcp_t* pdcp_entity);
/**
 * Checks if incoming PDU has a sequence number in accordance with the RX window
 * @return TRUE if it is valid, FALSE otherwise
 */
BOOL pdcp_is_rx_seq_number_valid(u16 seq_num, pdcp_t* pdcp_entity);
/**
* Updates missing PDU bitmap with incoming sequence number
* @return TRUE if successful, FALSE otherwise
*/
BOOL pdcp_mark_current_pdu_as_received(u16 seq_num, pdcp_t* pdcp_entity);

#endif
