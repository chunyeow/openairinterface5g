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
/*! \file lteRALue_proto.h
 * \brief This file defines the prototypes of the common functions
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __RAL_UE_LTE_PROTO_H__
#define __RAL_UE_LTE_PROTO_H__

//lteRALue_ioctl.c
int RAL_ue_process_NAS_message(int ioctl_obj, int ioctl_cmd, int ioctl_cellid);

//mRALuD_process.c
void IAL_ue_NAS_measures_init(void);
void IAL_ue_NAS_measures_update(int i);
void IAL_ue_integrate_measure(int measure, int i);
void rallte_ue_NAS_measures_polling(void);
int  rallte_ue_NAS_corresponding_cell(int req_index);
void rallte_ue_verifyPendingConnection(void);

void* ral_ue_task(void *args_p);

#endif

