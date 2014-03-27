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

/*! \file mobility_parser.h
* \brief A parser for trace-based mobility information (parsed from a file)
* \author  S. Uppoor
* \date 2011
* \version 0.1
* \company INRIA
* \email: sandesh.uppor@inria.fr
* \note
* \warning
*/

#ifndef MOBILITY_PARSER_H_
#define MOBILITY_PARSER_H_

#include "omg_hashtable.h"

/**
 * @struct Simple struct to hold only few information
 * @brief The entry in each node is a line from the external mobility file
 */
typedef struct Exnode {
	double time;
	int vid;
	double x;
	double y;
	double speed;
	int visit;
	struct Exnode* next;

}Exnode;

/**
 * @struct struct acts as gateway to hashtable
 * @brief holds vehicle id and initial pointer to the head of the linked list in hastable
 */
typedef struct node_info {
	int vid;
	int *vid_addr;
	struct node_info* next;
	//struct node_info* prev;
}node_info;
// head pointer to a linked list containing vid,vid_addr



/**
 * function just to initialize the head of the linked list to NULL, this can be removed later(check).
 */
struct Exnode* gen_list();

/**
 * Function to append the line content to build a linked list.
 * @param headRef is the pointer to the head of the existing list to which append has to be done
 * @param newNode , item to be appended
 */
void AppendNode(struct Exnode* headRef, struct Exnode* newNode);

/**
 * function just prints the linked list (need to be removed, used for testing)
 * @param pointer to the head of the linked list
 */
void print_list(struct Exnode* head);

/**
 * function reads each line and checks if vehicle has an entry in the hastable,
 * if so append append is called else a new linked list to the vehicle is created.
 * @param need mobility file to be given
 */

void read_mobility_file(); // mobility file need to be given here, add in omg_param_list, get it from there

/**
 * function builds a linked list which holds vehicle id and its mapping pointer to
 * the head of the linked list in the hash table.
 * @param headRef is the pointer to this linked list
 * @param vid is the vehicle id
 * @param pointer to the head of the linked list to the vehicle entry in the hash table
 */
node_info*  build_node_info(node_info* headRef, int vid, int *vid_addr);

/**
 * just counts the number of nodes in the mobility file
 */
int get_num_nodes();

/**
 * these function are used since there might be a possibility if user provides a mobility
 * discription which is out of order.
 * @param hashtable
 * @param starting pointer for the vehicles linked list
 * @param ending pointer of the vehicles linked list
 */

void sort_veh_movement(omg_hash_table_t *table);
void quicksortlist(Exnode *pLeft, Exnode *pRight);

/**
 * function returns the node containing next position (just reads the linked list from the hashtable)
 * and never repeats, each time next location ahead of the current is returned
 * @param hashtable from which the node is to be looked
 * @param node_id is the nodes whose next location need to be retrieved from the linked list
 */
Exnode* get_next_position(omg_hash_table_t *table,int node_id);

void reset_visit_status(omg_hash_table_t *table, float time, int node_id);

void clear_llist();

#endif /* MOBILITY_PARSER_H_ */
