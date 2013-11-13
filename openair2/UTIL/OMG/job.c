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

/*! \file job.c
* \brief handle jobs for future nodes' update
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

Job_list add_job(Pair job, Job_list Job_Vector){
    Job_list entry = malloc(sizeof(Job_list));
    entry->pair = job;
   
    //LOG_D(OMG, "  Job_Vector_len %d", Job_Vector_len); 
    entry->next = NULL;

    if (Job_Vector == NULL) {
      //LOG_D(OMG, "empty Job_Vector\n");
      //LOG_D(OMG, "added elmt ID %d\n", entry->pair->b->ID);
        return entry;
    }
    else {
        Job_list tmp = Job_Vector;
        while (tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = entry;
        //LOG_D(OMG, "non empty Job_Vector\n");
	    //LOG_D(OMG, "dded elmt ID %d\n", entry->pair->b->ID);

        return Job_Vector;
    }
}


// display list of jobs
void display_job_list(Job_list Job_Vector){

    	Job_list tmp = Job_Vector;
   if (tmp == NULL){LOG_D(OMG, "Empty Job_list\n");}
   else{
        //LOG_D(OMG, "first_Job_time in Job_Vector %f\n", Job_Vector->pair->a);
     while (tmp != NULL){
     	if ((tmp->pair != NULL))
	  LOG_D(OMG, "node %d \ttime %.2f\n", tmp->pair->b->ID, tmp->pair->a);

        tmp = tmp->next;

    }
 }
}

// quick sort of the linked list
Job_list job_list_sort (Job_list list, Job_list end){

    Job_list pivot, tmp, next, before, after;
    if ( list != end && list->next != end ){

        pivot = list;
	before = pivot;
	after = end;
        for ( tmp=list->next; tmp != end; tmp=next )
        {
            next = tmp->next;
            if (tmp->pair->a > pivot->pair->a)
                tmp->next = after, after = tmp;
            else
                tmp->next = before, before = tmp; 
        }
        
        before = job_list_sort (before, pivot);
        after = job_list_sort (after, end);
        
        pivot->next = after;
        return before;
    }
    return list;
}

Job_list quick_sort (Job_list list)
{
    return job_list_sort(list, NULL);
}


Job_list remove_job(Job_list list, int nID, int node_type){

  Job_list  current, previous;
  //int cond=0;
  int i=0;
  if (list == NULL){
    return NULL;
  }
  else{                             //start search
    current = list;
    if ((current != NULL) && (current->pair->b->ID == nID) && (current->pair->b->type == node_type )){LOG_D(OMG, "(current != NULL) && (current->pair->b->ID != nID) && (current->pair->b->type != node_type ");}
    while ( (current != NULL) && ((current->pair->b->ID != nID) || (current->pair->b->type != node_type ))){
      LOG_D(OMG, "current->pair->b->ID %d",current->pair->b->ID);
      LOG_D(OMG, "nID %d",nID);
      LOG_D(OMG, "current->pair->b->type  %d",current->pair->b->type ); // UE, eNB   1
      LOG_D(OMG, "node_type  %d",node_type ); //UE, eNB  0
      LOG_D(OMG, "current->pair->b->generator  %d",current->pair->b->generator );  //static
      
      LOG_D(OMG, "i = %d", i);
      previous = current;        //  previous hobbles after
      current = current->next;
      i++;
      if (current ==NULL){LOG_D(OMG, "current ==NULL");}
      
    }
    
    
    //if (current->pair->b->ID == nID){LOG_D(OMG, "current->pair->b->ID == nID");}
    //if (current->pair->b->type == node_type){LOG_D(OMG, "current->pair->b->type == node_type");}
    
    if (current ==NULL) { 
      LOG_D(OMG," Job to remove is not found\n "); //LOG_N 
      return NULL;
    }              //value not found
    else{
      if (current == list) {
	list = current->next  ;
	LOG_D(OMG,"Job to remove is found at the beginning\n");
      }    
      else {
	previous->next = current->next;
      }
    }
    return list;
  }
}


