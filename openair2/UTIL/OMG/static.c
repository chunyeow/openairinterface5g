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
* \author  M. Mahersi,  J. Harri, N. Nikaein,
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

#include "omg.h"
void place_static_node(NodePtr node);

void start_static_generator(omg_global_param omg_param_list) {
  
  int n_id=0;
  //  double cur_time = 1.0; 
  NodePtr node = NULL;
  MobilityPtr mobility = NULL;
  
  
  if (omg_param_list.nodes <= 0){
    LOG_W(OMG, "Number of static nodes has not been set\n");
    return;
  }
  
  srand(omg_param_list.seed + STATIC); 
  // for (n_id = omg_param_list.first_ix; n_id< omg_param_list.first_ix + omg_param_list.nodes; n_id++){

  if (omg_param_list.nodes_type == eNB) {
    LOG_I(OMG, "Static mobility model for %d eNBs \n", omg_param_list.nodes);
  } else if (omg_param_list.nodes_type == UE) {
    LOG_I(OMG, "Static mobility model for %d UE\n",omg_param_list.nodes);
  }
    
  for (n_id = 0; n_id< omg_param_list.nodes; n_id++) {
    
    node = (NodePtr) create_node();
    mobility = (MobilityPtr) create_mobility();
    
    node->ID = n_id; 
    node->generator = omg_param_list.mobility_type;
    node->type =  omg_param_list.nodes_type;
    node->mob = mobility;
    node->generator = STATIC;

    place_static_node(node);	//initial positions
  }
}


void place_static_node(NodePtr node) {
 	
  node->X_pos = (double) ((int) (randomGen(omg_param_list.min_X, omg_param_list.max_X)*100))/ 100;
  node->mob->X_from = node->X_pos;
  node->mob->X_to = node->X_pos;
  node->Y_pos = (double) ((int) (randomGen(omg_param_list.min_Y,omg_param_list.max_Y)*100))/ 100;
  node->mob->Y_from = node->Y_pos;
  node->mob->Y_to = node->Y_pos;
  
  node->mob->speed = 0.0;
  node->mob->journey_time = 0.0;
  
  LOG_I(OMG, "[STATIC] Initial position of node ID: %d type(UE, eNB): %d (X = %.2f, Y = %.2f) speed = 0.0\n", node->ID, node->type, node->X_pos, node->Y_pos);  
  Node_Vector[STATIC] = (Node_list) add_entry(node, Node_Vector[STATIC]);
  Node_Vector_len[STATIC]++;
  //Initial_Node_Vector_len[STATIC]++;
  
}

