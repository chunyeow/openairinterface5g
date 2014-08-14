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

/*! \file rlc_tm_reassembly_proto_extern.h
* \brief This file defines the prototypes of the functions dealing with the reassembly and the sent of SDUs to upper layer.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
* \note
* \bug
* \warning
*/
#    ifndef __RLC_TM_REASSEMBLY_H__
#        define __RLC_TM_REASSEMBLY_H__
//-----------------------------------------------------------------------------
#        include "rlc_tm_entity.h"
//-----------------------------------------------------------------------------
extern void     rlc_tm_send_sdu_no_segment (struct rlc_tm_entity *rlcP, uint8_t error_indicationP, uint8_t * srcP, uint16_t length_in_bitsP);
extern void     rlc_tm_send_sdu_segment (struct rlc_tm_entity *rlcP, uint8_t error_indicationP);
#    endif
