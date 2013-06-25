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

