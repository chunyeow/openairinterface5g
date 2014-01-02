/*******************************************************************************

  Eurecom OpenAirInterface
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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#ifndef RRC_TYPES_H_
#define RRC_TYPES_H_

typedef enum Rrc_State_e {
  RRC_STATE_INACTIVE=0,
  RRC_STATE_IDLE,
  RRC_STATE_CONNECTED,

  RRC_STATE_FIRST = RRC_STATE_INACTIVE,
  RRC_STATE_LAST = RRC_STATE_CONNECTED,
} Rrc_State_t;

typedef enum Rrc_Sub_State_e {
  RRC_SUB_STATE_INACTIVE=0,

  RRC_SUB_STATE_IDLE_SEARCHING,
  RRC_SUB_STATE_IDLE_RECEIVING_SIB,
  RRC_SUB_STATE_IDLE_SIB_COMPLETE,
  RRC_SUB_STATE_IDLE_CONNECTING,
  RRC_SUB_STATE_IDLE,

  RRC_SUB_STATE_CONNECTED,

  RRC_SUB_STATE_INACTIVE_FIRST = RRC_SUB_STATE_INACTIVE,
  RRC_SUB_STATE_INACTIVE_LAST = RRC_SUB_STATE_INACTIVE,

  RRC_SUB_STATE_IDLE_FIRST = RRC_SUB_STATE_IDLE_SEARCHING,
  RRC_SUB_STATE_IDLE_LAST = RRC_SUB_STATE_IDLE,

  RRC_SUB_STATE_CONNECTED_FIRST = RRC_SUB_STATE_CONNECTED,
  RRC_SUB_STATE_CONNECTED_LAST = RRC_SUB_STATE_CONNECTED,
  } Rrc_Sub_State_t;

#endif /* RRC_TYPES_H_ */
