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
/*! \file mRALlte_thresholds.h
 * \brief
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MRALLTE_THRESHOLDS_H__
#    define __MRALLTE_THRESHOLDS_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_THRESHOLDS_C
#            define private_mRALlte_thresholds(x)    x
#            define protected_mRALlte_thresholds(x)  x
#            define public_mRALlte_thresholds(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_thresholds(x)
#                define protected_mRALlte_thresholds(x)  extern x
#                define public_mRALlte_thresholds(x)     extern x
#            else
#                define private_mRALlte_thresholds(x)
#                define protected_mRALlte_thresholds(x)
#                define public_mRALlte_thresholds(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
#include "mRALlte_mih_msg.h"
//-----------------------------------------------------------------------------
protected_mRALlte_thresholds(LIST(MIH_C_LINK_CFG_PARAM, g_link_cfg_param_thresholds);)
//-----------------------------------------------------------------------------
protected_mRALlte_thresholds(void mRALlte_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* messageP);)
public_mRALlte_thresholds(   void mRALlte_check_thresholds_signal_strength(MIH_C_THRESHOLD_VAL_T new_valP, MIH_C_THRESHOLD_VAL_T old_valP);)

#endif
