/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2014 Eurecom

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
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/*! \file event_handler.h
* \brief primitives to handle event acting on oai
* \author Navid Nikaein and Mohamed Said MOSLI BOUKSIAA,
* \date 2014
* \version 0.5
* @ingroup _oai
*/

#include "oaisim.h"
#include "UTIL/FIFO/pad_list.h"



void add_event(Event_t event);

void schedule(Event_Type_t type, int frame, char * key, void* value);

void schedule_events(void);

void execute_events(frame_t frame);


void update_oai_model(char * key, void * value);

void update_sys_model(char * key, void * value);

void update_topo_model(char * key, void * value);

void update_app_model(char * key, void * value);

void update_emu_model(char * key, void * value);

void update_mac(char * key, void * value);

/*
void schedule_end_of_simulation(End_Of_Sim_Event_Type type, int value);

int end_of_simulation();
*/
