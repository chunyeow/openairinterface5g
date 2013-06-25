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
/*! \file rlc_am_status_report.h
* \brief This file defines the prototypes of the functions dealing with the reading/writting of informations from/in RLC AM control PDUs, and the processing of received control PDUs.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_status_report_impl_ RLC AM Status Report Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#ifndef __RLC_AM_STATUS_REPORT_H__
#    define __RLC_AM_STATUS_REPORT_H__

#    include "UTIL/MEM/mem_block.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_AM_STATUS_REPORT_C
#            define private_rlc_am_status_report(x)    x
#            define protected_rlc_am_status_report(x)  x
#            define public_rlc_am_status_report(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_status_report(x)
#                define protected_rlc_am_status_report(x)  extern x
#                define public_rlc_am_status_report(x)     extern x
#            else
#                define private_rlc_am_status_report(x)
#                define protected_rlc_am_status_report(x)
#                define public_rlc_am_status_report(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "platform_types.h"
#include "platform_constants.h"
#include "PHY/defs.h"

//-----------------------------------------------------------------------------
/*! \fn u16_t      rlc_am_read_bit_field    (u8_t** dataP, unsigned int* bit_posP, signed int bits_to_readP)
* \brief      Read N bits in a byte area seen as a bit-field with the help of a byte index and a modulo 8 bit index.
* \param[in,out]  dataP           Data pointer on area to read, updated after the read.
* \param[in,out]  bit_posP        Last ending read bit position, updated after the read.
* \param[in]      bits_to_readP   Number of bits to read (max 16).
* \return         Value read on 16 bits, bits read are shifted to least significant bits of returned short value.
*/
protected_rlc_am_status_report( u16_t      rlc_am_read_bit_field             (u8_t** dataP, unsigned int* bit_posP, signed int bits_to_readP);)

/*! \fn void        rlc_am_write8_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u8_t valueP)
* \brief      Write N bits (max 8) in a byte area seen as a bit-field with the help of a byte index and a modulo 8 bit index.
* \param[in,out]  dataP           Data pointer on area to write, updated after the write.
* \param[in,out]  bit_posP        Last ending read write position, updated after the write.
* \param[in]      bits_to_writeP  Number of bits to write (max 8).
* \param[in]      valueP          Value to write.
*/
protected_rlc_am_status_report(void        rlc_am_write8_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u8_t valueP);)

/*! \fn void        rlc_am_write16_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u16_t valueP)
* \brief      Write N bits (max 16) in a byte area seen as a bit-field with the help of a byte index and a modulo 8 bit index.
* \param[in,out]  dataP           Data pointer on area to write, updated after the write.
* \param[in,out]  bit_posP        Last ending read write position, updated after the write.
* \param[in]      bits_to_writeP  Number of bits to write (max 16).
* \param[in]      valueP          Value to write.
*/
protected_rlc_am_status_report(void        rlc_am_write16_bit_field(u8_t** dataP, unsigned int* bit_posP, signed int bits_to_writeP, u16_t valueP);)

/*! \fn signed int rlc_am_get_control_pdu_infos      (rlc_am_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_am_control_pdu_info_t* pdu_infoP)
* \brief      Retrieve control PDU informations from a serialized control PDU.
* \param[in]  headerP           Pointer on the header of the RLC AM PDU.
* \param[in]  total_sizeP       PDU size in bytes.
* \param[in,out]  pdu_infoP     Struct containing interpreted PDU control informations.
*/
protected_rlc_am_status_report( signed int rlc_am_get_control_pdu_infos (rlc_am_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_am_control_pdu_info_t* pdu_infoP);)

/*! \fn void rlc_am_display_control_pdu_infos(rlc_am_control_pdu_info_t* pdu_infoP)
* \brief      Dump on LOG output the informations contained in the pdu_infoP structure.
* \param[in]  pdu_infoP         Struct containing interpreted PDU control informations.
*/
protected_rlc_am_status_report( void rlc_am_display_control_pdu_infos(rlc_am_control_pdu_info_t* pdu_infoP);)

/*! \fn void rlc_am_receive_process_control_pdu(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t*  tbP, u8_t* first_byte, u16_t tb_size_in_bytes)
* \brief      Process the informations contained in the control PDU.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  frame             Frame index.
* \param[in]  tbP               Control PDU embedded in a mem_block_t structure.
* \param[in]  first_byte        Pointer on first byte of control PDU.
* \param[in]  tb_size_in_bytes  Size of serialized control PDU in bytes.
*/
protected_rlc_am_status_report( void       rlc_am_receive_process_control_pdu(rlc_am_entity_t* rlcP, u32_t frame, mem_block_t*  tbP, u8_t* first_byte, u16_t tb_size_in_bytes);)

/*! \fn int  rlc_am_write_status_pdu(u32_t frame,rlc_am_pdu_sn_10_t* rlc_am_pdu_sn_10P, rlc_am_control_pdu_info_t* pdu_infoP)
* \brief      Remove all marked holes for PDU with sequence number "snP".
* \param[in]  frame               Frame index.
* \param[in]  rlc_am_pdu_sn_10P   Pointer on the header of the RLC AM control PDU.
* \param[in]  pdu_infoP           Struct containing PDU control informations elements.
* \return     The number of bytes that have been written.
*/
protected_rlc_am_status_report(int  rlc_am_write_status_pdu(u32_t frame, rlc_am_pdu_sn_10_t* rlc_am_pdu_sn_10P, rlc_am_control_pdu_info_t* pdu_infoP);)

/*! \fn void        rlc_am_send_status_pdu(rlc_am_entity_t* rlcP, u32_t frame)
* \brief      Send a status PDU based on the receiver buffer content.
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  frame          Frame index.
*/
protected_rlc_am_status_report(void        rlc_am_send_status_pdu(rlc_am_entity_t* rlcP, u32_t frame);)
/** @} */
#endif
