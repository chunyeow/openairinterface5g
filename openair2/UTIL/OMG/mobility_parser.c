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

/*! \file mobility_parser.c
* \brief A parser for trace-based mobility information (parsed from a file)
* \author  S. Uppoor
* \date 2011
* \version 0.1
* \company INRIA
* \email: sandesh.uppoor@inria.fr
* \note
* \warning
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mobility_parser.h"
#include "omg_hashtable.h"
#include "omg.h"


node_info* head_node_info =NULL;
omg_hash_table_t* table=NULL;

//need to be removed , used only once (old code)
struct Exnode* gen_list(){
  struct Exnode* head = NULL;
  return head;
}


//read the mobility file and generates a hashtable of linked list with key pointing to vehicle id
void read_mobility_file(char* mobility_file[]){
  FILE *fp;
  char str[128],*p;
  if (table == NULL){
    table = hash_table_new(MODE_ALLREF);
  }
  
  if((fp=fopen(mobility_file, "r"))==NULL) {
    LOG_D(OMG,"Cannot open file %s\n", mobility_file);
    exit(1);
  }
  Exnode* headRef;
  static node_info* Node_info=NULL;
  
  int *keyholder[10];
  int i=0;
  
  while(!feof(fp)) {
    if(fgets(str, 126, fp)) { // happy go for small mobility file :-)
      char * pch;
      int fmt=0;
      p=str;
      while(*p==' ' || *p=='\t') p++; // skip whitespaces
      if(*p=='\r') p++;
      if (*p!='\n') {
	pch = strtok (p," "); // the separator between the items in the list is a space
	Exnode* node = malloc(sizeof(Exnode));
	
	while (pch != NULL){
	  node->visit=0;
	  switch(fmt){
	  case 0:
	    node->time=(atof(pch));
	    break;
	  case 1:
	    node->vid =atoi(pch);
	    break;
	  case 2:
	    node->x=atof(pch);
	    break;
	  case 3:
	    node->y=atof(pch);
	    break;
	  case 4:
	    node->speed=atof(pch);
	    break;
	  default:
	    //need a log statement here
	    break;
	  }
	  fmt +=1;
	  pch = strtok (NULL, " ");
	}
	node->next=NULL;
	
	//check in the hash table if the key exist node->vid if exist ? initialize headRef
	int *value = NULL;
	value = (int *)HT_LOOKUP(table, &(node->vid));
        
	if (value==NULL){
	  if (Node_info==NULL){
	    Node_info=build_node_info(Node_info,node->vid,&(node->vid));
	    head_node_info=Node_info;
	  }
	  else{
	    Node_info=build_node_info(Node_info,node->vid,&(node->vid));
	  }
	  Node_info->next=NULL;
	  //LOG_D(OMG,"[TRACE] build info for node %d %d head %p node %p next %p\n", 
	  //      Node_info->vid, node->vid, head_node_info,  Node_info, Node_info->next);
	  keyholder[i]=&node->vid;
	  i++;
	  hash_table_add(table, &(node->vid), sizeof(node->vid), node, sizeof(node));
	  headRef=gen_list();
	  
	}
	else{
	  //puts("Yes node exist");
	  headRef = (Exnode *)value;
	  //printf("value returned %f\n",headRef->time);
	  //printf("After from hash %p %d\n",headRef, headRef->vid );
	}
	if (headRef!=NULL){
	  AppendNode(headRef, node);
	}
      }
    }
  }
  fclose(fp);
  //return table;
}

//builds linked list with each node holding vehicle is and linked list pointer on the hastable
node_info*  build_node_info(node_info* headRef, int vid, int *vid_addr){

  node_info* newNode=malloc(sizeof(node_info));
  newNode->vid=vid;
  newNode->vid_addr=vid_addr;
  if (headRef==NULL){
    headRef=newNode;
    return headRef;
  }
  while(headRef->next!=NULL){
    headRef = headRef->next;
  }
  if (headRef->next==NULL ){
    headRef->next = newNode;
    
  }
  return headRef->next;
}

void AppendNode(struct Exnode* headRef, struct Exnode* newNode) {
  
  while(headRef->next!=NULL){
    headRef = headRef->next;
  }
  if (headRef->next==NULL ){
    headRef->next = newNode;
  }
}

//Just used for testing, it prints a linked list given the head pointer
void print_list(struct Exnode* head){
    
  while(head->next !=NULL)
    head=head->next;
}

Exnode* get_next_position(omg_hash_table_t *table,int node_id){
  node_info* head_node=head_node_info;
  while(head_node->next!=NULL){
    
    if(head_node->vid==node_id){
      int *value1 = NULL;
      value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
      if (value1!=NULL){
	Exnode* value2=(Exnode *)value1;
	while(value2->next!=NULL){
	  if (value2->visit==1) value2=value2->next;
	  else {
	    value2->visit=1;
	    return value2;
	  }
	}
	if (value2->visit!=1){
	  value2->visit=1;
	  return value2;
	}
      }
    }
    head_node=head_node->next;
  }
  // not to leave the last node with ->next=NULL
  if(head_node->vid==node_id){
    int *value1 = NULL;
    value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
    if (value1!=NULL){
      Exnode* value2=(Exnode *)value1;
      while(value2->next!=NULL){
	if (value2->visit==1) value2=value2->next;
	else {
	  value2->visit=1;
	  return value2;
	}
      }
      if (value2->visit!=1){
	value2->visit=1;
	return value2;
      }
    }
  }
  return NULL;
}


void reset_visit_status(omg_hash_table_t *table,float time, int node_id){
  node_info* head_node=head_node_info;
  while(head_node->next!=NULL){
    
    if(head_node->vid==node_id){
      int *value1 = NULL;
      value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
      if (value1!=NULL){
	Exnode* value2=(Exnode *)value1;
	while(value2->next!=NULL){
	  if (value2->time == time) {
	    value2->visit=0;
	  }
	  value2=value2->next;
	}
	if (value2->time == time){
	  value2->visit=0;
	}
      }
    }
    head_node=head_node->next;
  }
  // not to leave the last node with ->next=NULL
  if(head_node->vid==node_id){
    int *value1 = NULL;
    value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
    if (value1!=NULL){
      Exnode* value2=(Exnode *)value1;
      while(value2->next!=NULL){
	if (value2->time==time) value2->visit=0;
	value2=value2->next;
      }
      if (value2->time==time) value2->visit=0;
    }
  }
  
}

int get_num_nodes(){
  int count=1; //Last node also need to be counted
  node_info * head_node=head_node_info;
  while (head_node->next!=NULL){
    count += 1;
    head_node=head_node->next;
  }
  return count;
}
void sort_veh_movement(omg_hash_table_t *table){
  node_info* head_node=head_node_info;
  while(head_node->next!=NULL){
    int *value1 = NULL;
    value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
    Exnode* head_veh_node = (Exnode *)value1;
    Exnode* value2 = (Exnode *)value1;
    while (value2->next!=NULL){
      value2 = value2->next;
    }
    Exnode* tail_veh_node = (Exnode *)value2;
    quicksortlist(head_veh_node, tail_veh_node);
    head_node=head_node->next;
  }
  // come on !! use functions :-)
  // last node with ->next == NULL
  int *value1 = NULL;
  value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
  Exnode* head_veh_node = (Exnode *)value1;
  Exnode* value2 = (Exnode *)value1;
  while (value2->next!=NULL){
    value2 = value2->next;
  }
  Exnode* tail_veh_node = (Exnode *)value2;
  quicksortlist(head_veh_node, tail_veh_node);
}

// quick sort list
void quicksortlist(Exnode *pLeft, Exnode *pRight){
  Exnode *pStart;
  Exnode *pCurrent;
  double swp_time;
  int swp_vid;
  double swp_x;
  double swp_y;
  double swp_speed;
  int swp_visit;
  
  // If the left and right pointers are the same, then return
  if (pLeft == pRight) return;
  
  // Set the Start and the Current item pointers
  pStart = pLeft;
  pCurrent = pStart->next;
  // Loop forever (well until we get to the right)
  while (1){
    if (pStart->time > pCurrent->time){
      // Swap the items,swapping address could be critical, head pointer change will disrupt
      // so swapping values
      swp_time = pCurrent->time;
      swp_vid = pCurrent->vid;
      swp_x = pCurrent->x;
      swp_y = pCurrent->y;
      swp_speed = pCurrent->speed;
      swp_visit = pCurrent->visit;
      
      pCurrent->time = pStart->time;
      pCurrent->vid = pStart->vid;
      pCurrent->x = pStart->x;
      pCurrent->y = pStart->y;
      pCurrent->speed = pStart->speed;
      pCurrent->visit = pStart->visit;
      
      pStart->time = swp_time;
      pStart->vid = swp_vid;
      pStart->x = swp_x;
      pStart->y = swp_y;
      pStart->speed = swp_speed;
      pStart->visit = swp_visit;
    }
    
    // Check if we have reached the end
    if (pCurrent == pRight) break;
    
    // Move to the next item in the list
    pCurrent = pCurrent->next;
  }
  quicksortlist(pStart->next,pCurrent);
  
}

void clear_llist(){
  
  node_info* TempNode=NULL;
  node_info* tmp=NULL;
  TempNode=head_node_info;
  
  while(TempNode->next!=NULL){
    int *value1 = NULL;
    value1 = (int *) HT_LOOKUP(table, TempNode->vid_addr);
    Exnode* TempMob = (Exnode *)value1;
    
    while (TempMob->next!=NULL){
      Exnode* tmp=NULL;
      tmp=TempMob;
      TempMob=TempMob->next;
      free(tmp);
      tmp=NULL;
    }
    if (TempMob->next == NULL){
      free(TempMob);
      TempMob=NULL;
    }
    tmp=TempNode;
    TempNode=TempNode->next;
    free(tmp);
    tmp=NULL;
  }
  if (TempNode->next==NULL ){
    free(TempNode);
    TempNode=NULL;
  }
  hash_table_delete(table);
}
/*     

int main(){
        //char *mobility_file[50];
	Exnode* next_loc=NULL;
        //mobility_file = (char*) malloc(256);
	//mobility_file=strtok("regular.tr");
	omg_hash_table_t *table=read_mobility_file();
	sort_veh_movement(table);
	printf("Number of nodes --> %d \n",get_num_nodes());
	next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	//next_loc=NULL;
	next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	//next_loc=NULL;
	next_loc=get_next_position(table,140392);
	if (next_loc!=NULL){
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	//next_loc=NULL;
	}next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	next_loc=get_next_position(table,140392);
	//if (next_loc!=NULL){
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140392);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
        next_loc=get_next_position(table,140396);
	printf("node details %d\n %lf %lf %lf %lf %d\n",next_loc->vid,next_loc->time,next_loc->x,next_loc->y,next_loc->speed,next_loc->vid);
	//reset_visit_status(table,20.0,11);
	node_info* head_node=head_node_info;

	while(head_node->next!=NULL){

		if(head_node->vid==140392){
			int *value1 = NULL;
			value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
			Exnode *value=(Exnode *)value1;
			while(value->next!=NULL){
					printf("checking reset %d\n",value->visit);
					value=value->next;
				}
			printf("checking reset %d\n",value->visit);

		}
		head_node=head_node->next;
	}
	int *value1 = NULL;
	value1 = (int *) HT_LOOKUP(table, head_node->vid_addr);
				Exnode *value=(Exnode *)value1;
				while(value->next!=NULL){
						printf("checking reset %d %d\n",value->visit,value->vid);
						value=value->next;
					}
				printf("checking reset %d %d\n",value->visit,value->vid);
	return 0;
}*/
