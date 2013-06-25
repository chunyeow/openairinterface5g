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
/**
 * \file static.h
 * \brief Prototypes of the functions used for the STATIC model
 * \date 22 May 2011
 * 
 */

#ifndef STATIC_H_
#define STATIC_H_

#include "omg.h"

/**
 * \fn void start_static_generator(omg_global_param omg_param_list)
 * \brief Start the STATIC model by setting the initial position of each node 
 * \param omg_param_list a structure that contains the main parameters needed to establish the random positions distribution
 */
void start_static_generator(omg_global_param omg_param_list);


/**
 \fn void place_static_node(NodePtr node)
 * \brief Generates a random position ((X,Y) coordinates) and assign it to the node passed as argument. This latter node is then added to the Node_Vector[STATIC] 
 * \param node a pointer of type NodePtr that represents the node to which the random position is assigned
 */
void place_static_node(NodePtr node);

#endif /* STATIC_H_ */
