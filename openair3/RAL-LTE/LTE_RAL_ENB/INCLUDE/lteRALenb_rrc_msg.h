/*******************************************************************************
 *
 * Eurecom OpenAirInterface 3
 * Copyright(c) 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/
/*! \file lteRALenb_rrc_msg.h
 * \brief
 * \author GAUTHIER Lionel
 * \date 2013
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __LTE_RAL_ENB_RRC_MSG_H__
#define __LTE_RAL_ENB_RRC_MSG_H__
//-----------------------------------------------------------------------------
#        ifdef LTE_RAL_ENB_RRC_MSG_C
#            define private_lteralenb_rrc_msg(x)    x
#            define protected_lteralenb_rrc_msg(x)  x
#            define public_lteralenb_rrc_msg(x)     x
#        else
#            ifdef LTE_RAL_ENB
#                define private_lteralenb_rrc_msg(x)
#                define protected_lteralenb_rrc_msg(x)  extern x
#                define public_lteralenb_rrc_msg(x)     extern x
#            else
#                define private_lteralenb_rrc_msg(x)
#                define protected_lteralenb_rrc_msg(x)
#                define public_lteralenb_rrc_msg(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include "lteRALenb.h"
#include "intertask_interface.h"

protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_system_configuration_indication       (instance_t instance, MessageDef *msg_p);)
protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_connection_establishment_indication   (instance_t instance, MessageDef *msg_p);)
protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_connection_reestablishment_indication (instance_t instance, MessageDef *msg_p);)
protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_connection_reconfiguration_indication (instance_t instance, MessageDef *msg_p);)
protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_measurement_report_indication         (instance_t instance, MessageDef *msg_p);)
protected_lteralenb_rrc_msg(void eRAL_rx_rrc_ral_connection_release_indication         (instance_t instance, MessageDef *msg_p);)


#endif
