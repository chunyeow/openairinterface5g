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
/*! \fn void rlc_am_clear_rx_sdu (rlc_am_entity_t *rlc_pP)
* \brief    Reset the data cursor index in the output SDU buffer to zero.
* \param[in]  rlc_pP                      RLC AM protocol instance pointer.
*/
private_rlc_am_reassembly(   void rlc_am_clear_rx_sdu (rlc_am_entity_t *rlc_pP);)

/*! \fn void rlc_am_reassembly   (uint8_t * srcP, int32_t lengthP, rlc_am_entity_t *rlc_pP,frame_t frameP)
* \brief    Concatenate datas at the tail of the output SDU in construction. This SDU in construction will be sent to higher layer.
* \param[in]  srcP                      Pointer on data to be reassemblied.
* \param[in]  lengthP                   Length of data to be reassemblied.
* \param[in]  rlc_pP                      RLC AM protocol instance pointer.
* \param[in]  frameP                     Frame index.
*/
private_rlc_am_reassembly(   void rlc_am_reassembly   (uint8_t * srcP, int32_t lengthP, rlc_am_entity_t *rlc_pP,frame_t frameP);)

/*! \fn void rlc_am_send_sdu     (rlc_am_entity_t *rlc_pP,frame_t frameP)
* \brief    Send the output SDU in construction to higher layer.
* \param[in]  rlc_pP                      RLC AM protocol instance pointer.
* \param[in]  frameP                     Frame index.
*/
private_rlc_am_reassembly(   void rlc_am_send_sdu     (rlc_am_entity_t *rlc_pP,frame_t frameP, eNB_flag_t eNB_flagP);)

/*! \fn void rlc_am_reassemble_pdu(rlc_am_entity_t* rlc_pP, frame_t frameP, uint8_t eNB_flagP, mem_block_t* tb_pP)
* \brief    Reassembly a RLC AM PDU, depending of the content of this PDU, data will be reassemblied to the current output SDU, the current will be sent to higher layers or not, after or before the reassembly, or no send of SDU will be triggered, depending on FI field in PDU header.
* \param[in]  rlc_pP                      RLC AM protocol instance pointer.
* \param[in]  frameP                     Frame index.
* \param[in]  eNB_flagP                  Flag to indicate eNB (1) or UE (0).
* \param[in]  tb_pP                       RLC AM PDU embedded in a mem_block_t.
*/
protected_rlc_am_reassembly( void rlc_am_reassemble_pdu(rlc_am_entity_t* rlc_pP, frame_t frameP, eNB_flag_t eNB_flagP, mem_block_t* tb_pP);)
/** @} */
#endif

