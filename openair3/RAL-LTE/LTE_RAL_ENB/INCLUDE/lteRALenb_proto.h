/***************************************************************************
                         lteRALenb_proto.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

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
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALenb_proto.h
 * \brief This file defines the prototypes of the common functions
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __LTE_RAL_ENB_PROTO_H__
#define __LTE_RAL_ENB_PROTO_H__

# error "Remove inclusion"
//#include "lteRALenb_variables.h"
/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/
/*
 * Functions defined in eRALlte_NAS.c
 */

//void eRALlte_NAS_get_MTs_list(void);
//int  eRALlte_NAS_update_MTs_list(void);

//int eRALlte_NAS_process_message(void);
//int eRALlte_NAS_send_rb_establish_request(int mt_ix, int ch_ix);
//int eRALlte_NAS_send_rb_release_request(int mt_ix, int ch_ix);
//int eRALlte_NAS_send_measure_request(void);

//#ifdef RAL_REALTIME
//int RAL_process_NAS_message(int ioctl_obj, int ioctl_cmd, int mt_ix, int ch_ix);
//#endif

/*
 * Functions defined in eRALlte_process.c
 */
//int eRALlte_process_find_channel(unsigned int cnxid, int* mt_ix, int* ch_ix);
//int eRALlte_process_find_new_channel(int mt_ix);
//void eRALlte_process_clean_channel(struct ral_lte_channel* pchannel);
//void eRALlte_process_verify_pending_mt_status(void);
//char* eRALlte_process_mt_addr_to_string(const unsigned char* mt_addr);
//void eRALlte_process_mt_addr_to_l2id(const unsigned char* mt_addr, unsigned int* l2id);
//int eRALlte_process_cmp_mt_addr(const char* mt_addr, const char* l2id);
//int eRALlte_process_find_mt_by_addr(const char* mt_addr);
//int eRALlte_process_map_qos(int mt_ix, int ch_ix);
// MW Added
//void RAL_printInitStatus(void);
//void RAL_NAS_measures_polling(void);
//void RAL_NAS_measures_analyze(void);
//void RAL_NAS_report_congestion(int ix);

//void* ral_enb_task(void *args_p);

#endif

