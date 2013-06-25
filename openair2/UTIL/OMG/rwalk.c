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
* \brief random walk mobility generator 
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
#include "rwalk.h"
#include "omg.h"


int start_rwalk_generator(omg_global_param omg_param_list) { 
 
  int n_id=0;
  //omg_omg_param_list.seed= time(NULL); 
  srand(omg_param_list.seed + RWALK);
   
  double cur_time = 0.0; 
  NodePtr node = NULL;
  MobilityPtr mobility = NULL;
  
  if (omg_param_list.nodes <= 0){
    LOG_W(OMG, "Number of nodes has not been set\n");
    return(-1);
  }
  
	if (omg_param_list.nodes_type == eNB) {
		LOG_I(OMG, "Node type has been set to eNB\n");
	} else if (omg_param_list.nodes_type == UE) {
		LOG_I(OMG, "Node type has been set to UE\n");
	}
 	LOG_I(OMG, "Number of random walk nodes has been set to %d\n", omg_param_list.nodes);

  
  for (n_id = 0; n_id< omg_param_list.nodes; n_id++) {
    
    node = (NodePtr) create_node();
    mobility = (MobilityPtr) create_mobility();
    node->mobile = 0; // static for the moment
    node->ID = n_id;
    node->type = omg_param_list.nodes_type; 
    node->generator = omg_param_list.mobility_type; 
         
    node->mob = mobility;
    
    place_rwalk_node(node);	//initial positions
    
    Pair pair = malloc (sizeof(Pair));
    pair = (Pair) sleep_rwalk_node(node, cur_time); //sleep
    
    Job_Vector = add_job(pair, Job_Vector);
    Job_Vector_len ++;

    if (Job_Vector == NULL)
      LOG_E(OMG, "Job Vector is NULL\n");
  }
 return(0); 
}


void place_rwalk_node(NodePtr node) {

	node->X_pos = (double) ((int) (randomGen(omg_param_list.min_X, omg_param_list.max_X)*100))/ 100;
	node->mob->X_from = node->X_pos;
	node->mob->X_to = node->X_pos;
	node->Y_pos = (double) ((int) (randomGen(omg_param_list.min_Y,omg_param_list.max_Y)*100))/ 100;
	node->mob->Y_from = node->Y_pos;
	node->mob->Y_to = node->Y_pos;

	node->mob->speed = 0.0;
	node->mob->journey_time = 0.0;

	LOG_D(OMG, " INITIALIZE RWALK NODE\n ");
    LOG_I(OMG, "Initial position of node ID: %d type: %d (X = %.2f, Y = %.2f) speed = 0.0\n", node->ID, node->type, node->X_pos, node->Y_pos);   
	Node_Vector[RWALK] = (Node_list) add_entry(node, Node_Vector[RWALK]);
   Node_Vector_len[RWALK]++;
   //Initial_Node_Vector_len[RWALK]++;
}


Pair sleep_rwalk_node(NodePtr node, double cur_time){
	node->mobile = 0;
	node->mob->speed = 0.0;
	node->mob->X_from = node->mob->X_to;
	node->mob->Y_from = node->mob->Y_to;
	node->X_pos = node->mob->X_to;
	node->Y_pos = node->mob->Y_to;
	Pair pair = malloc(sizeof(Pair)) ;

	node->mob->sleep_duration = (double) ((int) (randomGen(omg_param_list.min_sleep, omg_param_list.max_sleep)*100))/ 100;
	LOG_D(OMG, "node: %d \tsleep duration : %.2f\n",node->ID, node->mob->sleep_duration);

	node->mob->start_journey = cur_time;
	pair->a = node->mob->start_journey + node->mob->sleep_duration; //when to wake up
	LOG_D(OMG, "to wake up at time: cur_time + sleep_duration : %.2f\n", pair->a);
	pair->b = node;

	return pair;
}
 
Pair move_rwalk_node(NodePtr node, double cur_time) {
	Pair pair = malloc(sizeof(Pair));
	LOG_D(OMG, "MOVE RWALK NODE ID: %d\n",node->ID);

    node->X_pos = node->mob->X_to;
	node->Y_pos = node->mob->Y_to;
	node->mob->X_from = node->X_pos;
	node->mob->Y_from = node->Y_pos;
	node->mobile = 1; 
	double speed_next = randomGen(omg_param_list.min_speed, omg_param_list.max_speed);
	node->mob->speed = speed_next;
    	LOG_D(OMG, "speed_next: %f\n", speed_next); //m/s

	double azimuth_next = randomGen(omg_param_list.min_azimuth, omg_param_list.max_azimuth);
	node->mob->azimuth = azimuth_next;
    	LOG_D(OMG, "azimuth_next: %f\n", node->mob->azimuth); 

	double journeyTime_next = randomGen(omg_param_list.min_journey_time, omg_param_list.max_journey_time);
	node->mob->journey_time = journeyTime_next;
    	LOG_D(OMG, "journey_time_next: %f\n", node->mob->journey_time); 

	double distance = node->mob->speed * node->mob->journey_time;
	LOG_D(OMG, "distance = speed * journey_time: %f\n", distance); 

	double dX = distance * cos(node->mob->azimuth*M_PI/180);
	LOG_D(OMG, "dX = distance * cos(azimuth): %f\n", dX); 
	double dY = distance * sin(node->mob->azimuth*M_PI/180);
	LOG_D(OMG, "dY = distance * sin(azimuth): %f\n", dY); 
	
	LOG_D(OMG, "from: (%.2f, %.2f)\n", node->X_pos, node->Y_pos);
	double X_next = (double) ((int)((node->X_pos + dX) *100))/ 100;
	double Y_next = (double) ((int)((node->X_pos + dY) *100))/ 100;
	LOG_D(OMG, "theoritical_destination: (%f, %f)\n", X_next, Y_next);

	/*if (X_next<param_list.min_X){   // first method
		node->mob->X_to = param_list.min_X;  
	}
	else if (X_next>param_list.max_X){
		node->mob->X_to =  param_list.max_X;
	}
	else {
		node->mob->X_to = X_next;
	}

	if (Y_next<param_list.min_Y){ 
		node->mob->Y_to = param_list.min_Y;  
	}
	else if (Y_next>param_list.max_Y){
		node->mob->Y_to =  param_list.max_Y;
	}
	else {
		node->mob->Y_to = Y_next;
	}*/
   if (X_next < omg_param_list.min_X){
	  while (X_next < omg_param_list.min_X){ 
	   	X_next = X_next + omg_param_list.max_X;  
	  }
    node->mob->X_to = X_next;
   }
	else if (X_next > omg_param_list.max_X){
     while (X_next > omg_param_list.max_X){ 
		 X_next =  X_next - omg_param_list.max_X;
	  }
    node->mob->X_to = X_next;
   }
	else {
		node->mob->X_to = X_next;
	}

	if (Y_next < omg_param_list.min_Y){ 
     while (Y_next < omg_param_list.min_Y){
		Y_next = Y_next + omg_param_list.max_Y;
	  }
     node->mob->Y_to = Y_next;
   }
	else if (Y_next > omg_param_list.max_Y){
     while (Y_next > omg_param_list.max_Y){
		Y_next =  Y_next - omg_param_list.max_Y;
     }
     node->mob->Y_to = Y_next;
	}
	else {
		node->mob->Y_to = Y_next;
	}
	
	LOG_I(OMG, "destination: (%.2f, %.2f)\n", node->mob->X_to, node->mob->Y_to);

	node->mob->start_journey = cur_time;
   	LOG_D(OMG, "start_journey %.2f\n", node->mob->start_journey );
	
	pair->a = (double) ((int) ( (node->mob->start_journey + journeyTime_next) *100))/ 100 ;
	LOG_D(OMG, "pair->a= start journey + journeyTime_next next %.2f\n", pair->a);

	pair->b = node;
	return pair;
	
}





void update_rwalk_nodes(double cur_time) {// need to implement an out-of-area check as well as a rebound function to stay in the area
   LOG_D(OMG, "--------UPDATE--------\n");
   Job_list tmp = Job_Vector;
   int done = 0; //
   while ((tmp != NULL) && (done == 0)){
      //if 	(tmp->pair == NULL){LOG_E(OMG, "UPDATE RWALK : tmp->pair ==NULL\n" );}
      //if 	(tmp->pair != NULL){LOG_E(OMG, "UPDATE RWALK : tmp->pair !=NULL\n" );}
	   LOG_D(OMG, "cur_time %f\n", cur_time );
		LOG_D(OMG, "tmp->pair->a  %f\n", tmp->pair->a  );

      if((tmp->pair !=NULL) && ( (double)tmp->pair->a >= cur_time - eps) && ( (double)tmp->pair->a <= cur_time + eps) ) { 
		   if (tmp->pair->b->generator == RWALK){
				LOG_D(OMG, " (first_job_time) %.2f == %.2f (cur_time) \n ",tmp->pair->a, cur_time );
 				LOG_D(OMG, " UPDATE RWALK \n ");
				NodePtr my_node = (NodePtr)tmp->pair->b;
				/*if(my_node->mobile == 1) {
 					LOG_D(OMG, " stop node and let it sleep \n" );
					my_node->mobile = 0;
					Pair pair = malloc(sizeof(Pair));
					pair = sleep_rwalk_node(my_node, cur_time);
					tmp->pair = pair;
					tmp = tmp->next;
				}*/
				if ((my_node->mobile ==0) || (my_node->mobile == 1)) {
					LOG_D(OMG, " node %d...let's move again \n",  my_node->ID);
					my_node->mobile = 1;
					Pair pair = malloc(sizeof(Pair));
					pair = move_rwalk_node(my_node, cur_time);
					tmp->pair = pair;
					tmp = tmp->next;
				}
				else{
			  		LOG_E(OMG, "update_generator: unsupported node state - mobile : %d \n", my_node->mobile);
					//exit(-1);
				}
         }
         else {
            LOG_D(OMG, " (first_job_time) %.2f == %.2f(cur_time) but (generator=%d) != (RWALK=%d)\n ",tmp->pair->a, cur_time, tmp->pair->b->generator, RWALK );
            tmp = tmp->next;
         }
      }
		else if ( (tmp->pair != NULL) && (cur_time < tmp->pair->a ) ){  //&& (tmp->pair->b->generator == RWALK)
		    LOG_D(OMG, " %.2f < %.2f \n",cur_time, tmp->pair->a);
		    LOG_D(OMG, "Nothing to do\n");
			 done = 1;  //quit the loop
		}
      else {
			LOG_E(OMG, "\n %.2f > %.2f", cur_time,tmp->pair->a   );   //LOG_D(OMG, " (generator=%d) != (RWALK=%d) \n", tmp->pair->b->generator,  RWALK );
		   done = 1;  //quit the loop
         exit(-1);
		}	
 }
			//sorting the new entries
			LOG_D(OMG, "--------DISPLAY JOB LIST--------\n"); //LOG_T
  			display_job_list(Job_Vector);
 			Job_Vector = quick_sort (Job_Vector);///////////
  			LOG_D(OMG, "--------DISPLAY JOB LIST AFTER SORTING--------\n"); 
 			display_job_list(Job_Vector);
}

void get_rwalk_positions_updated(double cur_time){
  
  double X_now=0.0;
  double Y_now=0.0;
  LOG_D(OMG,"--------GET RWALK POSITIONS--------\n"); 
  Pair my_pair = Job_Vector->pair;
  if ( my_pair ==NULL) {
    LOG_E(OMG, "my_pair ==NULL");
  }
  else if ( (my_pair !=NULL) && (cur_time <= my_pair->a ) ){
      LOG_D(OMG," %.2f <= %.2f \n",cur_time, my_pair->a);
      Job_list tmp = Job_Vector;
      while (tmp != NULL){
		  if (tmp->pair->b->generator == RWALK){
			 if (tmp->pair->b->mobile == 0){ //node is sleeping
			    LOG_T(OMG,"node number %d is sleeping at location: (%.2f, %.2f)\n", tmp->pair->b->ID, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);
			    LOG_T(OMG,"Nothing to do, node is sleeping\n");
			 } 
			else if (tmp->pair->b->mobile == 1){ //node is moving
			    LOG_T(OMG,"Node_number %d\n", tmp->pair->b->ID);
			    LOG_T(OMG, "destination not yet reached\tfrom (%.2f, %.2f)\tto (%.2f, %.2f)\tspeed %.2f\t(X_pos %.2f\tY_pos %.2f)\n", tmp->pair->b->mob->X_from, tmp->pair->b->mob->Y_from,tmp->pair->b->mob->X_to, tmp->pair->b->mob->Y_to,tmp->pair->b->mob->speed, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);

			    double len = sqrtf(pow(tmp->pair->b->mob->X_from - tmp->pair->b->mob->X_to,2)+pow(tmp->pair->b->mob->Y_from - tmp->pair->b->mob->Y_to,2));
			    double dx = fabs(tmp->pair->b->mob->X_from - tmp->pair->b->mob->X_to) / len;
				  
			    double dy = fabs(tmp->pair->b->mob->Y_from - tmp->pair->b->mob->Y_to) / len;
			    LOG_D(OMG,"len %f\tdx %f\tdy %f\n", len, dx, dy);
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


				    LOG_D(OMG,"X_now %f\tY_now %f\n", X_now, Y_now);

            if (X_now < omg_param_list.min_X){
	            while (X_now < omg_param_list.min_X){ 
	   	        X_now = X_now + omg_param_list.max_X;  
	            }
             }
	          else if (X_now > omg_param_list.max_X){
               while (X_now > omg_param_list.max_X){ 
	              X_now =  X_now - omg_param_list.max_X;
	            }
            }
	         else {
				   X_now = X_now;
	         }

	        if (Y_now < omg_param_list.min_Y){ 
             while (Y_now < omg_param_list.min_Y){
	            Y_now = Y_now + omg_param_list.max_Y;
	          }
           }
	        else if (Y_now > omg_param_list.max_Y){
             while (Y_now > omg_param_list.max_Y){
	            Y_now =  Y_now - omg_param_list.max_Y;
            }
	       }
	      else {
		     Y_now = Y_now;
	      }
		LOG_D(OMG,"X_now %f\tY_now %f\n", X_now, Y_now);
		
		
		tmp->pair->b->X_pos = (double) ((int) (X_now*100))/ 100;
		tmp->pair->b->Y_pos = (double) ((int) (Y_now*100))/ 100;
		//tmp->pair->b->mob->X_from = tmp->pair->b->X_pos;
		//tmp->pair->b->mob->Y_from = tmp->pair->b->Y_pos;
		//tmp->pair->b->mob->start_journey = cur_time;
		LOG_D(OMG,"updated_position of node number %d is :(%.2f, %.2f)\n", tmp->pair->b->ID, tmp->pair->b->X_pos, tmp->pair->b->Y_pos);
			}
			 
			else{
			  LOG_E(OMG,"update_generator: unsupported node state : %d \n", tmp->pair->b->mobile);
			   return; 
			}
		 }
		 tmp = tmp->next;
		}
	}
	else {
  	   LOG_E(OMG,"ERROR current time %f > first job_time %f\n", cur_time,   my_pair->a);		
  }	
}


/*void update_rwalk_nodes(double cur_time) {// need to implement an out-of-area check as well as a rebound function to stay in the area

	LOG_D(OMG,"--------UPDATE--------\n");
	int l = 0;
	
	Job_list tmp = Job_Vector;
	while (l < Job_Vector_len){
		LOG_D(OMG,"l == %d \n", l); 
		Pair my_pair = tmp->pair;
		LOG_D(OMG,"%.2f\n ",my_pair->a);
		if((my_pair !=NULL) && ( (double)my_pair->a >= cur_time - eps) && ( (double)my_pair->a <= cur_time + eps)) { 
			LOG_D(OMG,"%.2f == %.2f \n ",my_pair->a, cur_time );
         if (my_pair->b->generator == RWALK) {
 				LOG_D(OMG," UPDATE RWALK \n ");
			   NodePtr my_node= (NodePtr)my_pair->b;
			//if(my_node->mobile == 1) {
 			//	LOG_D(OMG," move again \n" );
			//	// stop node and let it sleep
			//	my_node->mobile = 0;
			//	Pair pair = malloc(sizeof(Pair));
			//	pair = sleep_rwalk_node(my_node, cur_time);
			//	Job_Vector->pair = pair;
			//}
			   if ((my_node->mobile ==0) ||( my_node->mobile == 1)) {
				  LOG_D(OMG," ...let's move again \n" );
				  // ...let's move again
				  my_node->mobile = 1;
				  Pair pair = malloc(sizeof(Pair));
				  pair = move_rwalk_node(my_node, cur_time);
				  Job_Vector->pair = pair;
			  }
			  else{
			    LOG_E(OMG, "update_generator: unsupported node state - mobile : %d \n", my_node->mobile);
			  }
			//sorting the new entries
			LOG_D(OMG, "--------DISPLAY JOB LIST--------\n"); //LOG_T
  			display_job_list(Job_Vector);
  			Job_Vector = quick_sort (Job_Vector); //////////
 			LOG_D(OMG, "--------DISPLAY JOB LIST AFTER SORTING--------\n"); 
 			display_job_list(Job_Vector);
         tmp = tmp->next;
			l++;
		 }
       else {
			LOG_D(OMG, " (first_job_time) %.2f == %.2f(cur_time) but (generator %d) != (RWALK %d)\n ",my_pair->a, cur_time, my_pair->b->generator, RWALK );
         tmp = tmp->next;
         l++;
       }
      }
		else if ( (my_pair !=NULL) && (cur_time < my_pair->a ) && (my_pair->b->generator == RWALK) ){
		    LOG_D(OMG," %.2f < %.2f\n ",cur_time, my_pair->a);
		    l = Job_Vector_len;
		    LOG_D(OMG,"Nothing to do\n");
		}
		else {
		    l = Job_Vector_len;
		    //LOG_D(OMG,"(generator=%d) != (RWALK=%d)\n", my_pair->b->generator,  RWALK );
			 LOG_E(OMG, " %.2f > %.2f \n", cur_time,my_pair->a);
		}	
}
}*/






		
