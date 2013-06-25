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
/*! \file rlc_am_segments_holes.h
* \brief This file defines the prototypes of the functions dealing with the re-segmentation of RLC PDUs stored in the retransmission buffer,.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_segment_holes_impl_ RLC AM Resegmentation Implementation
* @ingroup _rlc_am_internal_segment_impl_
* @{
*/

#ifndef __RLC_AM_SEGMENT_HOLES_H__
#    define __RLC_AM_SEGMENT_HOLES_H__
//-----------------------------------------------------------------------------
#    ifdef RLC_AM_SEGMENT_HOLES_C
#        define private_rlc_am_segments_holes(x)    x
#        define protected_rlc_am_segments_holes(x)  x
#        define public_rlc_am_segments_holes(x)     x
#    else
#        ifdef RLC_AM_MODULE
#            define private_rlc_am_segments_holes(x)
#            define protected_rlc_am_segments_holes(x)  extern x
#            define public_rlc_am_segments_holes(x)     extern x
#        else
#            define private_rlc_am_segments_holes(x)
#            define protected_rlc_am_segments_holes(x)
#            define public_rlc_am_segments_holes(x)     extern x
#        endif
#    endif
/*! \fn void rlc_am_clear_holes (rlc_am_entity_t *rlcP, u16_t snP)
* \brief      Remove all marked holes for PDU with sequence number "snP".
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  snP            Sequence number.
*/
protected_rlc_am_segments_holes(void rlc_am_clear_holes (rlc_am_entity_t *rlcP, u16_t snP);)

/*! \fn void rlc_am_remove_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP)
* \brief      Remove for PDU with sequence number "snP" a NACK for byte segment offset [so_startP, so_stopP].
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in[  frame          Frame index.
* \param[in]  snP            Sequence number.
* \param[in]  so_startP      Start of segment offset.
* \param[in]  so_stopP       End of segment offset.
*/
protected_rlc_am_segments_holes(void rlc_am_remove_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP);)

/*! \fn void rlc_am_get_next_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, int* so_startP, int* so_stopP)
* \brief      Get for PDU with sequence number "snP" the first hole start and stop parameters.
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in[  frame          Frame index.
* \param[in]  snP            Sequence number.
* \param[in,out]  so_startP  Start of segment offset.
* \param[in,out]  so_stopP   End of segment offset.
*/
protected_rlc_am_segments_holes(void rlc_am_get_next_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, int* so_startP, int* so_stopP);)

/*! \fn void rlc_am_add_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP)
* \brief      Mark for PDU with sequence number "snP" a NACK for byte segment offset [so_startP, so_stopP].
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in[  frame          Frame index.
* \param[in]  snP            Sequence number.
* \param[in,out]  so_startP  Start of segment offset.
* \param[in,out]  so_stopP   End of segment offset.
*/
protected_rlc_am_segments_holes(void rlc_am_add_hole (rlc_am_entity_t *rlcP, u32_t frame, u16_t snP, u16_t so_startP, u16_t so_stopP);)
/** @} */
#endif
