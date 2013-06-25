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
/*! \file rlc_am_init.h
* \brief This file defines the prototypes of the functions initializing a RLC AM protocol instance.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_init_impl_ RLC AM Init Reference Implementation
* @ingroup _rlc_am_impl_
* @{
*/
#ifndef __RLC_AM_INIT_H__
#    define __RLC_AM_INIT_H__

#    include "UTIL/MEM/mem_block.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_AM_INIT_C
#            define private_rlc_am_init(x)    x
#            define protected_rlc_am_init(x)  x
#            define public_rlc_am_init(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_init(x)
#                define protected_rlc_am_init(x)  extern x
#                define public_rlc_am_init(x)     extern x
#            else
#                define private_rlc_am_init(x)
#                define protected_rlc_am_init(x)
#                define public_rlc_am_init(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "platform_types.h"
#include "platform_constants.h"
#include "PHY/defs.h"


/*! \struct  rlc_am_info_t
* \brief Structure containing RLC AM configuration parameters.
*/
typedef volatile struct {
    u16_t max_retx_threshold;  /*!< \brief Maximum number of retransmissions for one RLC AM PDU. */
    u16_t poll_pdu;            /*!< \brief Generate a status each poll_pdu pdu sent. */
    u16_t poll_byte;           /*!< \brief Generate a status each time poll_byte bytes have been sent. */
    u32_t t_poll_retransmit;   /*!< \brief t-PollRetransmit timer initial value. */
    u32_t t_reordering;        /*!< \brief t-Reordering timer initial value. */
    u32_t t_status_prohibit;   /*!< \brief t-StatusProhibit timer initial value. */
} rlc_am_info_t;

//-----------------------------------------------------------------------------
/*! \fn void rlc_am_init   (rlc_am_entity_t* rlcP,u32_t frame)
* \brief    Initialize the RLC AM protocol instance, reset variables, allocate buffers, lists, then, the next step in order have a running RLC AM instance is to configure and set debug informations for this RLC instance.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index
*/
public_rlc_am_init( void rlc_am_init   (rlc_am_entity_t* rlcP,u32_t frame);)

/*! \fn void rlc_am_cleanup(rlc_am_entity_t* rlcP,u32_t frame)
* \brief    Free all memory resources allocated and kept by this RLC AM instance.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index
*/
public_rlc_am_init( void rlc_am_cleanup(rlc_am_entity_t* rlcP,u32_t frame);)

/*! \fn void rlc_am_configure(rlc_am_entity_t *rlcP, u32_t frame, u16_t max_retx_thresholdP, u16_t poll_pduP, u16_t poll_byteP, u32_t t_poll_retransmitP, u32_t t_reorderingP, u32_t t_status_prohibitP)
* \brief    Set RLC AM protocol parameters.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index
* \param[in]  max_retx_thresholdP       Limit the number of retransmissions of an
AMD PDU.
* \param[in]  poll_pduP                 Trigger a poll for every poll_pduP PDUs.
* \param[in]  poll_byteP                Trigger a poll for every pollByte bytes.
* \param[in]  t_poll_retransmitP        This timer is used by the transmitting side of an AM RLC entity in order to retransmit a poll, value in frames.
* \param[in]  t_reorderingP             This timer is used by the receiving side of an AM RLC entity in order to detect loss of RLC PDUs at lower layer, value in frames.
* \param[in]  t_status_prohibitP        This timer is used by the receiving side of an AM RLC entity in order to prohibit transmission of a STATUS PDU, value in frames.
*/
public_rlc_am_init( void rlc_am_configure(rlc_am_entity_t *rlcP,
					  u32_t frame,
                                          u16_t max_retx_thresholdP,
                                          u16_t poll_pduP,
                                          u16_t poll_byteP,
                                          u32_t t_poll_retransmitP,
                                          u32_t t_reorderingP,
                                          u32_t t_status_prohibitP);)

/*! \fn void rlc_am_set_debug_infos(rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flagP, module_id_t module_idP, rb_id_t rb_idP, rb_type_t rb_typeP)
* \brief    Set informations that will be displayed in traces, helping the debug process.
* \param[in]  rlcP                      RLC AM protocol instance pointer.
* \param[in]  frame                     Frame index
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0)
* \param[in]  module_idP                Virtualization variable, module identifier.
* \param[in]  rb_idP                    Radio bearer identifier.
* \param[in]  rb_typeP                  Radio bearer type (Signalling or data).
*/
public_rlc_am_init( void rlc_am_set_debug_infos(rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flagP, module_id_t module_idP, rb_id_t rb_idP, rb_type_t rb_typeP);)
/** @} */
#endif
