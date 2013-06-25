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

/*! \file pdcp_sequence_manager.c
* \brief PDCP Sequence Numbering Methods
* \author Baris Demiray
* \date 2011
*/

#include "pdcp_sequence_manager.h"
#include "UTIL/LOG/log_if.h"
#include "pdcp_util.h"

/*
 * Initializes sequence numbering state
 * @param pdcp_entity The PDCP entity to be initialized
 * @return BOOL TRUE on success, FALSE otherwise
 */
BOOL pdcp_init_seq_numbers(pdcp_t* pdcp_entity)
{
  if (pdcp_entity == NULL)
    return FALSE;

  /* Sequence number state variables */

  // TX and RX window
  pdcp_entity->next_pdcp_tx_sn = 0;
  pdcp_entity->next_pdcp_rx_sn = 0;

  // TX and RX Hyper Frame Numbers
  pdcp_entity->tx_hfn = 0;
  pdcp_entity->rx_hfn = 0;

  // SN of the last PDCP SDU delivered to upper layers
  // Shall UE and eNB behave differently on initialization? (see 7.1.e)
  pdcp_entity->last_submitted_pdcp_rx_sn = 4095;

  return TRUE;
}

BOOL pdcp_is_seq_num_size_valid(pdcp_t* pdcp_entity)
{
  if (pdcp_entity == NULL)
    return FALSE;

  // Check if the size of SN is valid (see 3GPP TS 36.323 v10.1.0 item 6.3.2)
  if (pdcp_entity->seq_num_size != 5 && pdcp_entity->seq_num_size != 7 && pdcp_entity->seq_num_size != 12) {
    LOG_W(PDCP, "Incoming SN size is invalid! (Expected: {5 | 7 | 12}, Received: %d\n", pdcp_entity->seq_num_size);
    return FALSE;
  }

  return TRUE;
}

/**
 * Check if SN number is in the range according to SN size
 */
BOOL pdcp_is_seq_num_valid(u16 seq_num, u8 seq_num_size)
{
  if (seq_num >= 0 && seq_num <= pdcp_calculate_max_seq_num_for_given_size(seq_num_size))
    return TRUE;

  return FALSE;
}

u16 pdcp_calculate_max_seq_num_for_given_size(u8 seq_num_size)
{
  u16 max_seq_num = 1;

  max_seq_num <<= seq_num_size;

  return max_seq_num - 1;
}

u16 pdcp_get_next_tx_seq_number(pdcp_t* pdcp_entity)
{
  if (pdcp_is_seq_num_size_valid(pdcp_entity) == FALSE)
    return -1;

  // Sequence number should be incremented after it is assigned for a PDU
  u16 pdcp_seq_num = pdcp_entity->next_pdcp_tx_sn;

  /*
   * Update sequence numbering state and Hyper Frame Number if SN has already reached
   * its max value (see 5.1 PDCP Data Transfer Procedures)
   */
  if (pdcp_entity->next_pdcp_tx_sn == pdcp_calculate_max_seq_num_for_given_size(pdcp_entity->seq_num_size)) {
    pdcp_entity->next_pdcp_tx_sn = 0;
    pdcp_entity->tx_hfn++;
    LOG_D(PDCP,"Reseting the PDCP sequence number\n");
  } else {
    pdcp_entity->next_pdcp_tx_sn++;
  }

  return pdcp_seq_num;
}

BOOL pdcp_advance_rx_window(pdcp_t* pdcp_entity)
{
  if (pdcp_is_seq_num_size_valid(pdcp_entity) == FALSE)
    return FALSE;

  /*
   * Update sequence numbering state and Hyper Frame Number if SN has already reached
   * its max value (see 5.1 PDCP Data Transfer Procedures)
   */
  LOG_I(PDCP, "Advancing RX window...\n");
  if (pdcp_entity->next_pdcp_rx_sn == pdcp_calculate_max_seq_num_for_given_size(pdcp_entity->seq_num_size)) {
    pdcp_entity->next_pdcp_rx_sn = 0;
    pdcp_entity->rx_hfn++;
  } else {
    pdcp_entity->next_pdcp_rx_sn++;
  }

  return TRUE;
}

/**
 * Checks if incoming PDU has a sequence number in accordance with the RX window
 * @return 1 if SN is okay, 0 otherwise
 * XXX Reordering window should also be handled here
 */
BOOL pdcp_is_rx_seq_number_valid(u16 seq_num, pdcp_t* pdcp_entity)
{
  LOG_D(PDCP, "Incoming RX Seq # is %04d\n", seq_num);

  if (pdcp_is_seq_num_size_valid(pdcp_entity) == FALSE || pdcp_is_seq_num_valid(seq_num, pdcp_entity->seq_num_size) == FALSE)
    return FALSE;

  /*
   * Mark received sequence numbers to keep track of missing ones
   * (and to build PDCP Control PDU for PDCP status report)
   */
  if (pdcp_mark_current_pdu_as_received(seq_num, pdcp_entity) == TRUE) {
    LOG_I(PDCP, "Received sequence number successfuly marked\n");
  } else {
    LOG_W(PDCP, "Cannot mark received sequence number on the bitmap!\n");
  }

  /*
   * XXX Since we do not implement reordering window yet we expect to receive
   * exactly the next SN from lower layer. When reordering window is implemented
   * the operator utilized here should be >= as stated in 5.1.2.1.2
   */
  if (seq_num == pdcp_entity->next_pdcp_rx_sn) {
    LOG_I(PDCP, "Next expected SN (%d) arrived, advancing RX window\n", seq_num);

    return pdcp_advance_rx_window(pdcp_entity);
  } else {
    LOG_E(PDCP, "D'oh! Incoming SN is not the one we expected to receive! (Incoming:%d, Expected:%d)\n", \
        seq_num, pdcp_entity->next_pdcp_rx_sn);

    /*
     * Update first missing PDU (used in PDCP Control PDU for 
     * PDCP status report, see 6.2.6)
     */
    if (pdcp_entity->first_missing_pdu != -1)
      pdcp_entity->first_missing_pdu = pdcp_entity->next_pdcp_rx_sn;

    return FALSE;
  }
}

BOOL pdcp_mark_current_pdu_as_received(u16 seq_num, pdcp_t* pdcp_entity)
{
  /*
   * Incoming sequence number and PDCP entity were already
   * validated in pdcp_is_rx_seq_number_valid() so we don't 
   * check here
   */

  /*
   * Find relevant octet
   */
  u16 octet_index = seq_num / 8;
  /*
   * Set relevant bit
   */
  LOG_D(PDCP, "Marking %d. bit of %d. octet of status bitmap\n", (seq_num % 8) + 1, octet_index);
  util_mark_nth_bit_of_octet(&pdcp_entity->missing_pdu_bitmap[octet_index], seq_num % 8); 
  util_print_binary_representation((u8*)"Current state of relevant octet: ", pdcp_entity->missing_pdu_bitmap[octet_index]);

  return TRUE;
}
