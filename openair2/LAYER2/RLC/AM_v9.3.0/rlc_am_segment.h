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
/*! \file rlc_am_segment.h
* \brief This file defines the prototypes of the functions dealing with the segmentation of PDCP SDUs.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_internal_segment_impl_ RLC AM Segmentation Internal Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#    ifndef __RLC_AM_SEGMENT_H__
#        define __RLC_AM_SEGMENT_H__
//-----------------------------------------------------------------------------
#        ifdef RLC_AM_SEGMENT_C
#            define private_rlc_am_segment(x)    x
#            define protected_rlc_am_segment(x)  x
#            define public_rlc_am_segment(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_segment(x)
#                define protected_rlc_am_segment(x)  extern x
#                define public_rlc_am_segment(x)     extern x
#            else
#                define private_rlc_am_segment(x)
#                define protected_rlc_am_segment(x)
#                define public_rlc_am_segment(x)     extern x
#            endif
#        endif

/*! \fn void rlc_am_pdu_polling (rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_sn_10_t *pduP, s16_t payload_sizeP)
* \brief      Set or not the poll bit in the PDU header depending on RLC AM protocol variables.
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in[  frame          Frame index.
* \param[in]  pduP           Pointer on the header of the PDU in order to be able to set the poll bit if necessary.
* \param[in]  payload_sizeP  Size of the payload of the PDU.
*/
protected_rlc_am_segment(void rlc_am_pdu_polling (rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_sn_10_t *pduP, s16_t payload_sizeP);)

/*! \fn void rlc_am_segment_10 (rlc_am_entity_t *rlcP)
* \brief      Segment a PDU with 10 bits sequence number, based on segmentation information given by MAC (size to transmit).
* \param[in]  rlcP         RLC AM protocol instance pointer.
* \param[in[  frame        Frame index.
*/
protected_rlc_am_segment(void rlc_am_segment_10 (rlc_am_entity_t *rlcP,u32_t frame);)
/** @} */
#    endif
