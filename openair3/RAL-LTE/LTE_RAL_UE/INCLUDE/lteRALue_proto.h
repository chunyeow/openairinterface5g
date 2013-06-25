/***************************************************************************
                         lteRALue_proto.h  -  description
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
/*! \file lteRALue_proto.h
 * \brief This file defines the prototypes of the common functions
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __RAL_LTE_PROTO_H__
#define __RAL_LTE_PROTO_H__

//lteRALue_ioctl.c
int RAL_process_NAS_message(int ioctl_obj, int ioctl_cmd, int ioctl_cellid);

//mRALuD_process.c
void IAL_NAS_measures_init(void);
void IAL_NAS_measures_update(int i);
void IAL_integrate_measure(int measure, int i);
void rallte_NAS_measures_polling(void);
int  rallte_NAS_corresponding_cell(int req_index);
void rallte_verifyPendingConnection(void);

#endif

