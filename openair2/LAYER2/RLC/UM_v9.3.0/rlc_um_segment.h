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
/*! \file rlc_um_segment.h
* \brief This file defines the prototypes of the functions dealing with the segmentation of PDCP SDUs.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_um_segment_impl_ RLC UM Segmentation Implementation
* @ingroup _rlc_um_impl_
* @{
*/
#    ifndef __RLC_UM_SEGMENT_PROTO_EXTERN_H__
#        define __RLC_UM_SEGMENT_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
#        include "rlc_um_structs.h"
#        include "rlc_um_constants.h"
#        include "list.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_UM_SEGMENT_C
#            define private_rlc_um_segment(x)    x
#            define protected_rlc_um_segment(x)  x
#            define public_rlc_um_segment(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_segment(x)
#                define protected_rlc_um_segment(x)  extern x
#                define public_rlc_um_segment(x)     extern x
#            else
#                define private_rlc_um_segment(x)
#                define protected_rlc_um_segment(x)
#                define public_rlc_um_segment(x)     extern x
#            endif
#        endif
/*! \fn void rlc_um_segment_10 (rlc_um_entity_t *rlcP,frame_t frame)
* \brief    Segmentation procedure with 10 bits sequence number, segment the first SDU in buffer and create a PDU of the size (nb_bytes_to_transmit) requested by MAC if possible and put it in the list "pdus_to_mac_layer".
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  frame       Frame Index.
*/
protected_rlc_um_segment(void rlc_um_segment_10 (rlc_um_entity_t *rlcP,frame_t frame));


/*! \fn void rlc_um_segment_5 (rlc_um_entity_t *rlcP,frame_t frame)
* \brief    Segmentation procedure with 5 bits sequence number, segment the first SDU in buffer and create a PDU of the size (nb_bytes_to_transmit) requested by MAC if possible and put it in the list "pdus_to_mac_layer".
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  frame       Frame Index.
*/
protected_rlc_um_segment(void rlc_um_segment_5 (rlc_um_entity_t *rlcP,frame_t frame));
/** @} */
#    endif
