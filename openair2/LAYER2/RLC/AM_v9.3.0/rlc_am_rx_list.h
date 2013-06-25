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
/*! \file rlc_am_rx_list.h
* \brief This file defines the prototypes of the functions dealing with a RX list data structure supporting re-segmentation.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_internal_rx_list_impl_ RLC AM RX List Reference Implementation
* @ingroup _rlc_am_internal_receiver_impl_
* @{
*/

#ifndef __RLC_AM_RX_LIST_H__
#    define __RLC_AM_RX_LIST_H__

#    include "UTIL/MEM/mem_block.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_AM_RX_LIST_C
#            define private_rlc_am_rx_list(x)    x
#            define protected_rlc_am_rx_list(x)  x
#            define public_rlc_am_rx_list(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_rx_list(x)
#                define protected_rlc_am_rx_list(x)  extern x
#                define public_rlc_am_rx_list(x)     extern x
#            else
#                define private_rlc_am_rx_list(x)
#                define protected_rlc_am_rx_list(x)
#                define public_rlc_am_rx_list(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "platform_types.h"
#include "platform_constants.h"
#include "PHY/defs.h"

//-----------------------------------------------------------------------------
/*! \fn signed int rlc_am_rx_list_insert_pdu(rlc_am_entity_t* rlcP u32_t frame, mem_block_t* tbP)
* \brief      Insert a PDU in the RX buffer (implemented with a list).
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in]  frame        Frame index.
* \param[in]  tbP          A PDU embedded in a mem_block_t.
* \return     Zero if the PDU could be inserted in the RX buffer, a negative value if the PDU could not be inserted.
*/
protected_rlc_am_rx_list( signed int rlc_am_rx_list_insert_pdu(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* tbP);)

/*! \fn void rlc_am_rx_check_all_byte_segments(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* tbP)
* \brief      Check if all sub-segments of a PDU are received, if yes then call rlc_am_rx_mark_all_segments_received() procedure.
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in]  frame        Frame index.
* \param[in]  tbP          A PDU embedded in a mem_block_t.
*/
protected_rlc_am_rx_list( void rlc_am_rx_check_all_byte_segments(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* tbP);)

/*! \fn void rlc_am_rx_mark_all_segments_received (rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* first_segment_tbP)
* \brief      Mark all PDUs having the same sequence number as first_segment_tbP with the information that all segments have been received.
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in]  frame        Frame index.
* \param[in]  first_segment_tbP   A PDU embedded in a mem_block_t, it is the first PDU in the RX buffer (list) that have its sequence number.
*/
protected_rlc_am_rx_list( void rlc_am_rx_mark_all_segments_received(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t* first_segment_tbP);)

/*! \fn void rlc_am_rx_list_reassemble_rlc_sdus(rlc_am_entity_t* rlcP,u32_t frame,u8_t eNB_flag)
* \brief      Reassembly all SDUS that it is possible to reassembly by parsing the RX buffer and looking for PDUs having the flag 'all segments received'.
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in]  frame        Frame index.
* \param[in]  eNB_flag     Flag to indicate eNB (1) or UE (0).
*/
protected_rlc_am_rx_list( void rlc_am_rx_list_reassemble_rlc_sdus(rlc_am_entity_t* rlcP,u32_t frame,u8_t eNB_flag);)

/*! \fn mem_block_t* list2_insert_before_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP)
* \brief      Insert a PDU embedded in a mem_block_t in a list at a position before a designated element of the list.
* \param[in]  element_to_insertP    Element to insert in the list listP, before elementP position.
* \param[in]  elementP              Element in the list.
* \param[in]  listP                 List where elements are linked.
*/
public_rlc_am_rx_list( mem_block_t* list2_insert_before_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP);)

/*! \fn mem_block_t* list2_insert_after_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP)
* \brief      Insert a PDU embedded in a mem_block_t in a list at a position after a designated element of the list.
* \param[in]  element_to_insertP    Element to insert in the list listP, after elementP position.
* \param[in]  elementP              Element in the list.
* \param[in]  listP                 List where elements are linked.
*/
public_rlc_am_rx_list( mem_block_t* list2_insert_after_element (mem_block_t * element_to_insertP, mem_block_t * elementP, list2_t * listP);)

/*! \fn void rlc_am_rx_list_display (rlc_am_entity_t* rlcP, char* messageP)
* \brief      Display the dump of the RX buffer.
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in]  messageP     Message to be displayed along with the display of the dump of the RX buffer.
*/
protected_rlc_am_rx_list( void rlc_am_rx_list_display (rlc_am_entity_t* rlcP, char* messageP);)
/** @} */
#endif
