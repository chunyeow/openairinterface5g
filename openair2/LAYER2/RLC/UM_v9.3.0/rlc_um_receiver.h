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
/*! \file rlc_um_receiver.h
* \brief This file defines the prototypes of the functions dealing with the first stage of the receiving process.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @addtogroup _rlc_um_receiver_impl_ RLC UM Receiver Implementation
* @{
*/
#    ifndef __RLC_UM_RECEIVER_PROTO_EXTERN_H__
#        define __RLC_UM_RECEIVER_PROTO_EXTERN_H__
#        ifdef RLC_UM_RECEIVER_C
#            define private_rlc_um_receiver(x)    x
#            define protected_rlc_um_receiver(x)  x
#            define public_rlc_um_receiver(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_receiver(x)
#                define protected_rlc_um_receiver(x)  extern x
#                define public_rlc_um_receiver(x)     extern x
#            else
#                define private_rlc_um_receiver(x)
#                define protected_rlc_um_receiver(x)
#                define public_rlc_um_receiver(x)     extern x
#            endif
#        endif

#        include "rlc_um_entity.h"
#        include "mac_primitives.h"

/*! \fn void rlc_um_display_rx_window(struct rlc_um_entity *rlcP)
* \brief    Display the content of the RX buffer, the output stream is targeted to TTY terminals because of escape sequences.
* \param[in]  rlcP        RLC UM protocol instance pointer.
*/
protected_rlc_um_receiver( void rlc_um_display_rx_window(struct rlc_um_entity *rlcP);)

/*! \fn void rlc_um_receive (struct rlc_um_entity *rlcP, u32_t frame, struct mac_data_ind data_indP)
* \brief    Handle the MAC data indication, retreive the transport blocks and send them one by one to the DAR process.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  frame       Frame index.
* \param[in]  data_indP   Data indication structure containing transport block received from MAC layer.
*/
protected_rlc_um_receiver( void rlc_um_receive (struct rlc_um_entity *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP));
/** @} */
#    endif
