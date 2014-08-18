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
/*! \file eRALlte_proto.h
 * \brief
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __RAL_LTE_PROTO_H__
#define __RAL_LTE_PROTO_H__

#include "eRALlte_variables.h"

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/*
 * Functions defined in eRALlte_NAS.c
 */

void eRALlte_NAS_get_MTs_list(void);
int  eRALlte_NAS_update_MTs_list(void);

int  eRALlte_NAS_process_message(void);
int  eRALlte_NAS_send_rb_establish_request(int mt_ix, int ch_ix);
int  eRALlte_NAS_send_rb_release_request(int mt_ix, int ch_ix);

#ifdef RAL_REALTIME
int TQAL_process_NAS_message(int ioctl_obj, int ioctl_cmd,
			     int mt_ix, int ch_ix);
#endif

/*
 * Functions defined in eRALlte_process.c
 */
int eRALlte_process_find_channel(unsigned int cnxid, int* mt_ix, int* ch_ix);
int eRALlte_process_find_new_channel(int mt_ix);
void eRALlte_process_clean_channel(struct ral_lte_channel* pchannel);
void eRALlte_process_verify_pending_mt_status(void);
char* eRALlte_process_mt_addr_to_string(const unsigned char* mt_addr);
void eRALlte_process_mt_addr_to_l2id(const unsigned char* mt_addr, unsigned int* l2id);
int eRALlte_process_cmp_mt_addr(const char* mt_addr, const char* l2id);
int eRALlte_process_find_mt_by_addr(const char* mt_addr);
int eRALlte_process_map_qos(int mt_ix, int ch_ix);

#endif

