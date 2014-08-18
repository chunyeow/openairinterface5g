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
/*! \file mRALlte_get.h
 * \brief
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MRALLTE_GET_H__
#    define __MRALLTE_GET_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_GET_C
#            define private_mRALlte_get(x)    x
#            define protected_mRALlte_get(x)  x
#            define public_mRALlte_get(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_get(x)
#                define protected_mRALlte_get(x)  extern x
#                define public_mRALlte_get(x)     extern x
#            else
#                define private_mRALlte_get(x)
#                define protected_mRALlte_get(x)
#                define public_mRALlte_get(x)     extern x
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
//-----------------------------------------------------------------------------
public_mRALlte_get(void MIH_C_3GPP_ADDR_load_3gpp_str_address(MIH_C_3GPP_ADDR_T* _3gpp_addrP, u_int8_t* strP);)

#endif
