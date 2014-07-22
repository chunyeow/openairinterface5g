/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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

/*! \file steadystaterwp.h
* \brief random waypoint mobility generator 
* \date 2014
* \version 0.1
* \company Eurecom
* \email: 
* \note
* \warning
*/
#ifndef STEADYSTATERWP_H_
#define STEADYSTATERWP_H_


int start_steadystaterwp_generator (omg_global_param omg_param_list);

void place_steadystaterwp_node (node_struct* node);

void sleep_steadystaterwp_node (pair_struct* pair, double cur_time);

void move_steadystaterwp_node (pair_struct* pair, double cur_time);

double pause_probability(omg_global_param omg_param);


double initial_pause(omg_global_param omg_param);

double initial_speed(omg_global_param omg_param);

void update_steadystaterwp_nodes (double cur_time);

void get_steadystaterwp_positions_updated (double cur_time);

#endif 











