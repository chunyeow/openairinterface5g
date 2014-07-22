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

/*! \file rwalk.c
* \brief static  mobility generator 
* \author  M. Mahersi, N. Nikaein, J. Harri
* \date 2011
* \version 0.1
* \company Eurecom
* \email: 
* \note
* \warning
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "static.h"


void
start_static_generator (omg_global_param omg_param_list)
{

  int id;
  static int n_id = 0;
  node_struct *node = NULL;
  mobility_struct *mobility = NULL;

  srand (omg_param_list.seed + STATIC);

  LOG_I (OMG, "Static mobility model for %d %d nodes\n", omg_param_list.nodes,
	 omg_param_list.nodes_type);
  for (id = n_id; id < (omg_param_list.nodes + n_id); id++)
    {

      node = create_node ();
      mobility = create_mobility ();

      node->id = id;
      node->type = omg_param_list.nodes_type;
      node->mob = mobility;
      node->generator = STATIC;
      place_static_node (node);	//initial positions
    }
  n_id += omg_param_list.nodes;


}



void
place_static_node (node_struct * node)
{
  if (omg_param_list[node->type].user_fixed && node->type == eNB)
    {
      if (omg_param_list[node->type].fixed_x <=
	  omg_param_list[node->type].max_x
	  && omg_param_list[node->type].fixed_x >=
	  omg_param_list[node->type].min_x)
	node->x_pos = omg_param_list[node->type].fixed_x;
      else
	node->x_pos =
	  (double) ((int)
		    (randomgen
		     (omg_param_list[node->type].min_x,
		      omg_param_list[node->type].max_x) * 100)) / 100;
      node->mob->x_from = node->x_pos;
      node->mob->x_to = node->x_pos;
      if (omg_param_list[node->type].fixed_y <=
	  omg_param_list[node->type].max_y
	  && omg_param_list[node->type].fixed_y >=
	  omg_param_list[node->type].min_y)
	node->y_pos = omg_param_list[node->type].fixed_y;
      else
	node->y_pos =
	  (double) ((int)
		    (randomgen
		     (omg_param_list[node->type].min_y,
		      omg_param_list[node->type].max_y) * 100)) / 100;
      node->mob->y_from = node->y_pos;
      node->mob->y_to = node->y_pos;
    }
  else
    {

      node->x_pos =
	(double) ((int)
		  (randomgen
		   (omg_param_list[node->type].min_x,
		    omg_param_list[node->type].max_x) * 100)) / 100;
      node->mob->x_from = node->x_pos;
      node->mob->x_to = node->x_pos;
      node->y_pos =
	(double) ((int)
		  (randomgen
		   (omg_param_list[node->type].min_y,
		    omg_param_list[node->type].max_y) * 100)) / 100;
      node->mob->y_from = node->y_pos;
      node->mob->y_to = node->y_pos;
    }

  node->mob->speed = 0.0;
  node->mob->journey_time = 0.0;

  LOG_I (OMG,
	 "[STATIC] Initial position of node ID: %d type(%d):  (X = %.2f, Y = %.2f) speed = 0.0\n",
	 node->id, node->type, node->x_pos, node->y_pos);
  node_vector_end[node->type] =
    (node_list *) add_entry (node, node_vector_end[node->type]);

  if (node_vector[node->type] == NULL)
    node_vector[node->type] = node_vector_end[node->type];

  node_vector_len[node->type]++;
}
