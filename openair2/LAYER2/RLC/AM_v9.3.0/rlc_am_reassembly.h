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
/*! \file rlc_am_reassembly.h
* \brief This file defines the prototypes of the functions dealing with the reassembly of segments.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_internal_reassembly_impl_ RLC AM Reassembly Internal Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#ifndef __RLC_AM_REASSEMBLY_H__
#    define __RLC_AM_REASSEMBLY_H__
#    ifdef RLC_AM_REASSEMBLY_C
#        define private_rlc_am_reassembly(x)    x
#        define protected_rlc_am_reassembly(x)  x
#        define public_rlc_am_reassembly(x)     x
#    else
#        ifdef RLC_AM_MODULE
#            define private_rlc_am_reassembly(x)
#            define protected_rlc_am_reassembly(x)  extern x
#            define public_rlc_am_reassembly(x)     extern x
#        else
#            define private_rlc_am_reassembly(x)
#            define protected_rlc_am_reassembly(x)
#            define public_rlc_am_reassembly(x)     extern x
#        endif
#    endif
/*! \fn void rlc_am_clear_rx_sdu (rlc_am_entity_t *rlcP)
* \brief    Reset the data cursor index in the output SDU buffer to zero.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
*/
private_rlc_am_reassembly(   void rlc_am_clear_rx_sdu (rlc_am_entity_t *rlcP);)

/*! \fn void rlc_am_reassembly   (u8_t * srcP, s32_t lengthP, rlc_am_entity_t *rlcP,u32_t frame)
* \brief    Concatenate datas at the tail of the output SDU in construction. This SDU in construction will be sent to higher layer.
* \param[in]  srcP                      Pointer on data to be reassemblied.
* \param[in]  lengthP                   Length of data to be reassemblied.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index.
*/
private_rlc_am_reassembly(   void rlc_am_reassembly   (u8_t * srcP, s32_t lengthP, rlc_am_entity_t *rlcP,u32_t frame);)

/*! \fn void rlc_am_send_sdu     (rlc_am_entity_t *rlcP,u32_t frame)
* \brief    Send the output SDU in construction to higher layer.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index.
*/
private_rlc_am_reassembly(   void rlc_am_send_sdu     (rlc_am_entity_t *rlcP,u32_t frame, u8_t eNB_flag);)

/*! \fn void rlc_am_reassemble_pdu(rlc_am_entity_t* rlcP, u32_t frame, u8_t eNB_flag, mem_block_t* tbP)
* \brief    Reassembly a RLC AM PDU, depending of the content of this PDU, data will be reassemblied to the current output SDU, the current will be sent to higher layers or not, after or before the reassembly, or no send of SDU will be triggered, depending on FI field in PDU header.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index.
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0).
* \param[in]  tbP                       RLC AM PDU embedded in a mem_block_t.
*/
protected_rlc_am_reassembly( void rlc_am_reassemble_pdu(rlc_am_entity_t* rlcP, u32_t frame, u8_t eNB_flag, mem_block_t* tbP);)
/** @} */
#endif

