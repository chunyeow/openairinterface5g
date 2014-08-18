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
