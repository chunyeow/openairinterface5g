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

/*! \file trace.h
* \brief The trace-based mobility model for OMG/OAI (mobility is statically imported from a file)
* \author  S. Uppoor
* \date 2011
* \version 0.1
* \company INRIA
* \email: sandesh.uppor@inria.fr
* \note
* \warning
*/

#ifndef TRACE_H_
#define TRACE_H_
//#include "defs.h"
#include "omg.h"
#include "trace_hashtable.h"
#include "mobility_parser.h"

int start_trace_generator (omg_global_param omg_param_list);

void place_trace_node (node_struct* node, node_data* n);

void move_trace_node (pair_struct* pair, node_data* n_data, double cur_time);

void schedule_trace_node ( pair_struct* pair, node_data* n_data, double cur_time);

void sleep_trace_node ( pair_struct* pair, node_data* n_data, double cur_time);

void update_trace_nodes (double cur_time);

void get_trace_positions_updated (double cur_time);
void clear_list (void);

#endif /* TRACE_H_ */
