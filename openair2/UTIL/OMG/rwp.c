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

/*! \file rwp.c
* \brief random waypoint mobility generator 
* \author  M. Mahersi,  J. Harri, N. Nikaein,
* \date 2011
* \version 0.1
* \company Eurecom
* \email: 
* \note
* \warning
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "rwp.h"
#include "omg.h"


int start_rwp_generator(omg_global_param omg_param_list) { 
 
  int n_id=0;
  //omg_omg_param_list.seed= time(NULL); 
  srand(omg_param_list.seed + RWP);
  
  double cur_time = 0.0; 
  NodePtr node = NULL;
  MobilityPtr mobility = NULL;
  
  if (omg_param_list.nodes <= 0){
    LOG_W(OMG,"[RWP] Number of nodes has not been set\n");
    return(-1);
  }

	if (omg_param_list.nodes_type == eNB) {
		LOG_I(OMG,"[RWP] Node type has been set to eNB\n");
	} else if (omg_param_list.nodes_type == UE) {
		LOG_I(OMG, "[RWP] Node type has been set to UE\n");
	}
 	LOG_I(OMG, "[RWP] Number of random waypoint nodes has been set to %d\n", omg_param_list.nodes);
  
  for (n_id = 0; n_id< omg_param_list.nodes; n_id++) {
    
    node = (NodePtr) create_node();
    mobility = (MobilityPtr) create_mobility();
    node->mobile = 0;  // static for the moment
    node->ID = n_id;
    node->type = omg_param_list.nodes_type; // UE eNB
    node->generator = omg_param_list.mobility_type;   // STATIC, RWP...
    node->mob = mobility;
    
    place_rwp_node(node);	//initial positions
    
    Pair pair = malloc (sizeof(Pair));
    pair = sleep_rwp_node(node, cur_time); //sleep
    
    Job_Vector = add_job(pair, Job_Vector);
    Job_Vector_len ++;

    if (Job_Vector == NULL)
      LOG_E(OMG, "[RWP] Job Vector is NULL\n");
    // else
    // LOG_T(OMG, "\nJob_Vector_Rwp->pair->b->ID %d\n", Job_Vector_Rwp->pair->b->ID);*/
  }
  return(0);
}


void place_rwp_node(NodePtr node) {

	node->X_pos = (double) ((int) (randomGen(omg_param_list.min_X, omg_param_list.max_X)*100))/ 100;
	node->mob->X_from = node->X_pos;
	node->mob->X_to = node->X_pos;
	node->Y_pos = (double) ((int) (randomGen(omg_param_list.min_Y,omg_param_list.max_Y)*100))/ 100;
	node->mob->Y_from = node->Y_pos;
	node->mob->Y_to = node->Y_pos;

	node->mob->speed = 0.0;
	node->mob->journey_time = 0.0;

  	LOG_I(OMG, "[RWP] Initial position of node ID: %d type: %d (X = %.2f, Y = %.2f) speed = 0.0\n ", node->ID, node->type, node->X_pos, node->Y_pos);   
	Node_Vector[RWP] = (Node_list) add_entry(node, Node_Vector[RWP]);
   Node_Vector_len[RWP]++;
	//Initial_Node_Vector_len[RWP]++;
}


Pair sleep_rwp_node(NodePtr node, double cur_time){
	node->mobile = 0;
	node->mob->speed = 0.0;
	node->mob->X_from = node->mob->X_to;
	node->mob->Y_from = node->mob->Y_to;
	node->X_pos = node->mob->X_to;
	node->Y_pos = node->mob->Y_to;
	Pair pair = malloc(sizeof(Pair)) ;
	
	node->mob->sleep_duration = (double) ((int) (randomGen(omg_param_list.min_sleep, omg_param_list.max_sleep)*100))/ 100;
	LOG_D(OMG, "[RWP] node: %d \tsleep duration : %.2f\n",node->ID, node->mob->sleep_duration);

	node->mob->start_journey = cur_time;
	pair->a = node->mob->start_journey + node->mob->sleep_duration; //when to wake up
	LOG_D(OMG, "[RWP] wake up at time: cur_time + sleep_duration : %.2f\n", pair->a);
	pair->b = node;

	return pair;
}


Pair move_rwp_node(NodePtr node, double cur_time) {
	
	Pair pair = malloc(sizeof(Pair));
	LOG_D(OMG, "[RWP] move node: %d\n",node->ID );
	node->mob->X_from = node->X_pos;
	node->mob->Y_from = node->Y_pos;
	LOG_D(OMG, "[RWP] Current Position: (%.2f, %.2f)\n", node->mob->X_from, node->mob->Y_from);

	node->mobile = 0;
	double X_next = (double) ((int)(randomGen(omg_param_list.min_X,omg_param_list.max_X)*100))/ 100;
	node->mob->X_to = X_next;
	double Y_next = (double) ((int)(randomGen(omg_param_list.min_Y,omg_param_list.max_Y)*100))/ 100;
	node->mob->Y_to = Y_next;
	LOG_D(OMG, "[RWP] destination: (%.2f, %.2f)\n", node->mob->X_to, node->mob->Y_to);

	double speed_next = (double) ((int)(randomGen(omg_param_list.min_speed, omg_param_list.max_speed)*100))/ 100;
	node->mob->speed = speed_next;
    	LOG_D(OMG, "[RWP] speed_next %.2f\n", speed_next); //m/s
	double distance = (double) ((int)(sqrtf(pow(node->mob->X_from - X_next, 2) + pow(node->mob->Y_from - Y_next, 2))*100))/ 100;
	LOG_D(OMG, "[RWP] distance %.2f\n", distance); //m

	double journeyTime_next =  (double) ((int)(distance/speed_next*100))/ 100;   //duration to get to dest
	////node->mob->journey_time = journeyTime_next;
	node->mobile = 1;
    	LOG_D(OMG, "[RWP] mob->journey_time_next %.2f\n",journeyTime_next );
	
	node->mob->start_journey = cur_time;
   	LOG_D(OMG, "[RWP] start_journey %.2f\n", node->mob->start_journey );
	pair->a = node->mob->start_journey + journeyTime_next;      //when to reach the destination
	LOG_D(OMG, "[RWP] reaching the destination at time : start journey + journey_time next =%.2f\n", pair->a);

	pair->b = node;
	return pair;
}
	


void update_rwp_nodes(double cur_time) {
  Job_list tmp = Job_Vector;
   int done = 0; //
   while ((tmp != NULL) && (done == 0)){
     //  if 	(tmp->pair == NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair ==NULL\n" );}
     //  if 	(tmp->pair != NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair !=NULL\n" );}
     LOG_D(OMG, "[RWP] cur_time %f reaching the destination at %f\n", cur_time, tmp->pair->a  );
          
     if((tmp->pair !=NULL) && ( (double)tmp->pair->a >= cur_time - eps) && ( (double)tmp->pair->a <= cur_time + eps) ) { 
       if (tmp->pair->b->generator == RWP){
	 LOG_D(OMG, "[RWP](first_job_time) %.2f == %.2f (cur_time) \n ",tmp->pair->a, cur_time );
	 NodePtr my_node = (NodePtr)tmp->pair->b;
	 if(my_node->mobile == 1) {
	   LOG_D(OMG, "[RWP] node %d goes to sleep\n", my_node->ID);
	   my_node->mobile = 0;
	   Pair pair = malloc(sizeof(Pair));
	   pair = sleep_rwp_node(my_node, cur_time);
	   tmp->pair = pair;
	   tmp = tmp->next;
	 }
	 else if (my_node->mobile ==0) {
	   LOG_D(OMG, "[RWP] node %d starts to move again \n",  my_node->ID);
	   my_node->mobile = 1;
	   Pair pair = malloc(sizeof(Pair));
	   pair = move_rwp_node(my_node, cur_time);
	   tmp->pair = pair;
	   tmp = tmp->next;
	 }
	 else{
	   LOG_E(OMG,"[RWP] update_generator: unsupported node state - mobile : %d \n", my_node->mobile);
	   exit(-1);
	 }
       }
       else {
	 LOG_D(OMG, "[RWP](first_job_time) %.2f == %.2f(cur_time) but (generator=%d) != (RWP=%d)\n ",tmp->pair->a, cur_time, tmp->pair->b->generator, RWP );
	 tmp = tmp->next;
       }
     }
     else if ( (tmp->pair != NULL) && (cur_time < tmp->pair->a ) ){  //&& (tmp->pair->b->generator == RWP)
       LOG_D(OMG, "[RWP] Nothing to do as current time %.2f is less than the time to reach the destination %.2f \n",cur_time, tmp->pair->a);
       done = 1;  //quit the loop
     }
     else {
       LOG_E(OMG,"[RWP] current time %.2f is %f greater than the time to reach the destination %.2f\n", cur_time, eps, tmp->pair->a );   
       done = 1;  //quit the loop
       // exit(-1);
     }	
 }
   //sorting the new entries
   LOG_D(OMG, "--------DISPLAY JOB LIST--------\n"); //LOG_T
   display_job_list(Job_Vector);
   Job_Vector = quick_sort (Job_Vector);///////////
   LOG_D(OMG, "--------DISPLAY JOB LIST AFTER SORTING--------\n"); 
   display_job_list(Job_Vector);
}

void get_rwp_positions_updated(double cur_time){
  
  double X_now=0.0;
  double Y_now=0.0;
  LOG_D(OMG, "--------GET RWP POSITIONS--------\n");
  
  Pair my_pair = Job_Vector->pair;
  if ( (my_pair !=NULL) && (cur_time <= my_pair->a +eps)){
    LOG_D(OMG, "[RWP] current time %.2f <= time when reaching the destination %.2f\n ",cur_time, my_pair->a);
    Job_list tmp = Job_Vector;
    while (tmp != NULL){
      if (tmp->pair->b->generator == RWP){
        if (tmp->pair->b->mobile == 0){ //node is sleeping
          LOG_D(OMG, "[RWP] node number %d is sleeping at location: (%.2f, %.2f)\n", tmp->pair->b->ID, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);
	} 
        else if (tmp->pair->b->mobile == 1){ //node is moving
	  LOG_D(OMG, "[RWP] destination not yet reached for node %d from (%.2f, %.2f)\tto (%.2f, %.2f)\tspeed %.2f\t(X_pos %.2f\tY_pos %.2f)\n",  
		tmp->pair->b->ID, tmp->pair->b->mob->X_from, tmp->pair->b->mob->Y_from,tmp->pair->b->mob->X_to, 
		tmp->pair->b->mob->Y_to,tmp->pair->b->mob->speed, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);

          double len = sqrtf(pow(tmp->pair->b->mob->X_from - tmp->pair->b->mob->X_to,2)+pow(tmp->pair->b->mob->Y_from - tmp->pair->b->mob->Y_to,2));
          double dx = fabs(tmp->pair->b->mob->X_from - tmp->pair->b->mob->X_to) / len;
				  
          double dy = fabs(tmp->pair->b->mob->Y_from - tmp->pair->b->mob->Y_to) / len;
          //LOG_D(OMG, "len %f\tdx %f\tdy %f\n", len, dx, dy);
          if (tmp->pair->b->mob->X_from < tmp->pair->b->mob->X_to ){
            X_now = tmp->pair->b->mob->X_from + (dx * (tmp->pair->b->mob->speed * (cur_time - tmp->pair->b->mob->start_journey) ) );
          }
          else{
            X_now = tmp->pair->b->mob->X_from - (dx * (tmp->pair->b->mob->speed * (cur_time - tmp->pair->b->mob->start_journey)));
          }
			   
          if (tmp->pair->b->mob->Y_from < tmp->pair->b->mob->Y_to ){
            Y_now = tmp->pair->b->mob->Y_from + (dy * (tmp->pair->b->mob->speed * (cur_time - tmp->pair->b->mob->start_journey)));
          }
          else{
            Y_now = tmp->pair->b->mob->Y_from - (dy * (tmp->pair->b->mob->speed * (cur_time - tmp->pair->b->mob->start_journey)));
          }
 
          tmp->pair->b->X_pos = (double) ((int) (X_now*100))/ 100;
          tmp->pair->b->Y_pos = (double) ((int) (Y_now*100))/ 100;
          //tmp->pair->b->mob->X_from = tmp->pair->b->X_pos;
          //tmp->pair->b->mob->Y_from = tmp->pair->b->Y_pos;
	  //tmp->pair->b->mob->start_journey = cur_time;
          LOG_D(OMG, "[RWP] Updated_position of node number %d is :(%.2f, %.2f)\n", tmp->pair->b->ID, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);
        }	
        else{
          LOG_E(OMG, "[RWP] Update_generator: unsupported node state - mobile : %d \n", tmp->pair->b->mobile);
          return; 
	}
      }
      tmp = tmp->next;
    }
 }
 else {
   LOG_E(OMG, "ERROR (current time) %f > %f (first job_time) \n", cur_time ,my_pair->a );
 }		
}


/*void update_rwp_nodes(double cur_time) {
	LOG_D(OMG, "--------UPDATE--------\n");
	int l = 0;
	
   Job_list tmp = Job_Vector;

	while (l < Job_Vector_len){
		LOG_D(OMG, "l == %d \n", l); 

		//Pair my_pair = malloc (sizeof(Pair));
      //my_pair = tmp->pair;

      if 	(tmp->pair == NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair ==NULL\n" );}
      if 	(tmp->pair != NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair ==NULL\n" );}

		LOG_D(OMG, "cur_time %f\n", cur_time );
		LOG_D(OMG, "tmp->pair->a  %f\n", tmp->pair->a  );

		if((tmp->pair !=NULL) && ( (double)tmp->pair->a >= cur_time - eps) && ( (double)tmp->pair->a <= cur_time + eps) ) { 
      	if (tmp->pair->b->generator == RWP){
				LOG_D(OMG, " (first_job_time) %.2f == %.2f (cur_time) \n ",tmp->pair->a, cur_time );
 				LOG_D(OMG, " UPDATE RWP \n ");
				NodePtr my_node = (NodePtr)tmp->pair->b;
				if(my_node->mobile == 1) {
 					LOG_D(OMG, " stop node and let it sleep \n" );
					my_node->mobile = 0;
					Pair pair = malloc(sizeof(Pair));
					pair = sleep_rwp_node(my_node, cur_time);
					tmp->pair = pair;
				}
				else if (my_node->mobile ==0) {
					LOG_D(OMG, " node %d slept enough...let's move again \n",  my_node->ID);
					my_node->mobile = 1;
					Pair pair = malloc(sizeof(Pair));
					pair = move_rwp_node(my_node, cur_time);
					tmp->pair = pair;
				}
				else
				{
			  		LOG_D(OMG, "update_generator: unsupported node state - mobile : %d \n", my_node->mobile);
				}
         
			//sorting the new entries
			LOG_D(OMG, "--------DISPLAY JOB LIST--------\n"); //LOG_T
  			display_job_list(Job_Vector);
 			Job_Vector = quick_sort (Job_Vector);///////////
  			LOG_D(OMG, "--------DISPLAY JOB LIST AFTER SORTING--------\n"); 
 			display_job_list(Job_Vector);
		LOG_D(OMG, "lgggggggggggggggggggggggggggggggggggggggggg\n"); 
      if 	(tmp != NULL){LOG_E(OMG, "UPDATE RWP : tmp !=NULL\n" );}
         tmp = tmp->next;
      if 	(tmp->pair == NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair ==NULL\n" );}
			l++;
		 }
       else {
         LOG_D(OMG, " (first_job_time) %.2f == %.2f(cur_time) but (generator=%d) != (RWP=%d)\n ",tmp->pair->a, cur_time, tmp->pair->b->generator, RWP );
         tmp = tmp->next;
       if 	(tmp->pair == NULL){LOG_E(OMG, "UPDATE RWP : tmp->pair ==NULL\n" );}
         l++;
      } 
      }
		else if ( (tmp->pair != NULL) && (cur_time < tmp->pair->a ) ){  //&& (tmp->pair->b->generator == RWP)
		    LOG_D(OMG, " %.2f < %.2f \n",cur_time, tmp->pair->a);
		    LOG_N(OMG, "Nothing to do\n");
			 l = Job_Vector_len;
		}
		else {
			LOG_E(OMG, "\n %.2f > %.2f", cur_time,tmp->pair->a   );   //LOG_D(OMG, " (generator=%d) != (RWP=%d) \n", tmp->pair->b->generator,  RWP );
		   l = Job_Vector_len;
		}	
 }
}*/









