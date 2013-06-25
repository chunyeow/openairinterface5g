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
/*! \file rlc_am_windows.h
* \brief This file defines the prototypes of the functions testing window, based on SN modulo and rx and tx protocol state variables.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_windowing_impl_ RLC AM Windowing Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#    ifndef __RLC_AM_WINDOWS_H__
#        define __RLC_AM_WINDOWS_H__
//-----------------------------------------------------------------------------
#        ifdef RLC_AM_WINDOWS_C
#            define private_rlc_am_windows(x)    x
#            define protected_rlc_am_windows(x)  x
#            define public_rlc_am_windows(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_windows(x)
#                define protected_rlc_am_windows(x)  extern x
#                define public_rlc_am_windows(x)     extern x
#            else
#                define private_rlc_am_windows(x)
#                define protected_rlc_am_windows(x)
#                define public_rlc_am_windows(x)     extern x
#            endif
#        endif

/*! \fn signed int rlc_am_in_tx_window(rlc_am_entity_t* rlcP, u16_t snP)
* \brief      Boolean function, check if sequence number is VT(A) <= snP < VT(MS).
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  snP               Sequence number.
* \return 1 if snP in tx window, else 0.
*/
protected_rlc_am_windows(signed int rlc_am_in_tx_window(rlc_am_entity_t* rlcP, u16_t snP);)

/*! \fn signed int rlc_am_in_rx_window(rlc_am_entity_t* rlcP, u16_t snP)
* \brief      Boolean function, check if sequence number is VR(R) <= snP < VR(MR).
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  snP               Sequence number.
* \return 1 if snP in rx window, else 0.
*/
protected_rlc_am_windows(signed int rlc_am_in_rx_window(rlc_am_entity_t* rlcP, u16_t snP);)

/*! \fn signed int rlc_am_sn_gte_vr_h (rlc_am_entity_t* rlcP, u16_t snP)
* \brief      Boolean function, check if sequence number is greater than or equal VR(R).
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  snP               Sequence number.
* \return 1 if sequence number is greater than or equal VR(R), else 0.
*/
protected_rlc_am_windows(signed int rlc_am_sn_gte_vr_h (rlc_am_entity_t* rlcP, u16_t snP);)

/*! \fn signed int rlc_am_sn_gte_vr_x (rlc_am_entity_t* rlcP, u16_t snP)
* \brief      Boolean function, check if sequence number is greater than or equal VR(X).
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  snP               Sequence number.
* \return 1 if sequence number is greater than or equal VR(X), else 0.
*/
protected_rlc_am_windows(signed int rlc_am_sn_gte_vr_x (rlc_am_entity_t* rlcP, u16_t snP);)

/*! \fn signed int rlc_am_sn_gt_vr_ms (rlc_am_entity_t* rlcP, u16_t snP)
* \brief      Boolean function, check if sequence number is greater than VR(MS).
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  snP               Sequence number.
* \return 1 if sequence number is greater than VR(MS), else 0.
*/
protected_rlc_am_windows(signed int rlc_am_sn_gt_vr_ms(rlc_am_entity_t* rlcP, u16_t snP);)

/*! \fn signed int rlc_am_tx_sn1_gt_sn2 (rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P)
* \brief      Boolean function, in the context of the tx window, check if sn1P is greater than sn2P.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  sn1P              Sequence number.
* \param[in]  sn2P              Sequence number.
* \return 1 if sn1P is greater than sn2P, else 0.
*/
protected_rlc_am_windows(signed int rlc_am_tx_sn1_gt_sn2(rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P);)

/*! \fn signed int rlc_am_rx_sn1_gt_sn2(rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P)
* \brief      Boolean function, in the context of the rx window, check if sn1P is greater than sn2P.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  sn1P              Sequence number.
* \param[in]  sn2P              Sequence number.
* \return 1 if sn1P is greater than sn2P, else 0.
*/
protected_rlc_am_windows(signed int rlc_am_rx_sn1_gt_sn2(rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P);)
/** @} */
#    endif
