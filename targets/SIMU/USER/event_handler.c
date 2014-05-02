/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2014 Eurecom

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
/*! \file event_handler.c
* \brief event handler primitives
* \author Navid Nikaein and Mohamed Said MOSLI BOUKSIAA,
* \date 2014
* \version 0.5
* @ingroup _oai
*/

#include <stdio.h>
#include <stdlib.h>

#include "event_handler.h"

//extern frame_t frame;
//End_Of_Sim_Event end_event; //Could later be a list of condition_events (if the end condition is more complicated)

Event_List_t event_list;

void add_event(Event_t event) {

  Event_elt_t * counter = event_list.head;
  Event_elt_t * previous = counter;
  Event_elt_t * elt = malloc(sizeof(Event_elt_t));
  
  while (counter != NULL) {
    if ((counter->event).frame > event.frame) break;
    //else if ((counter->event).next_slot > event.next_slot) break;
    else {
      previous = counter;
      counter = counter->next;
    }
  }
  
  elt->event = event;
  
  if (event_list.head != NULL)
    event_list_add_element(elt, previous, &event_list);
  else
    event_list_add_head(elt, &event_list);
  
}

void schedule(Event_Type_t type, int frame, char * key, void * value) {
    Event_t event;
    event.frame = frame;
    if (key == NULL) { //Global model update
        event.key = NULL;
    } else { //Update of a single parameter
        event.key = malloc (sizeof(char) * (strlen(key)+1));
        strcpy(event.key, key);
    }
    event.value = value;
    event.type = type;
    add_event(event);
}

/*
 * this function can reschedule events in the future to build a desired (more dynamic) scenario 
 * TODO: make sure that OCG OAI_Emulation is decoupled with the local vars
 */
void schedule_events(){
  
  /*
   * Step 1: vars needed for future changes
   */
  
  // overall config associated with OAI_ET
  // OAI_Emulation * oai_frame_1000; 
  
  // specific config
  /* 
   * associated with SYS_ET
   */
  //Environment_System_Config * system_frame_200;
  /*
   * associated with TOPO_ET
   */
  //Topology_Config *topology_frame_10;
  /* 
   * associated with APP_ET
   */
  //Application_Config *application_frame_20;
  /* 
   * associated with EMU_ET
   */
  //Emulation_Config * emulation_frame_100;
  
  
  /*
   * Step 2: set the desired future changes in the vars 
   */
  // i.e. optionally provide an XML file and update OCG vars
  
  //mob_frame_10 -> ...
  //application_frame_30 -> ...

  /*
   * Step 3: schedule the execution of changes 
   */

  // general OAI dynamic configuration

  //schedule(OAI_ET, 1000, NULL, oai_frame_1000);
  //schedule(SYS_ET, 200, NULL, system_frame_200);
  //schedule(TOPO_ET, 10, NULL, topology_frame_10);
  //schedule(APP_ET, 20, NULL, application_frame_20);
  //schedule(EMU_ET, 100, NULL,emulation_frame_100);

  // protocol dynamic configuration
  //schedule(MAC_ET, 100, NULL,mac_frame_100);


  //event_list_display(&event_list);

}

void execute_events(frame_t frame){
  
  Event_elt_t *user_defined_event;
  Event_t event;
 
  while ((user_defined_event = event_list_get_head(&event_list)) != NULL) {
    
    event = user_defined_event->event;
    
    if (event.frame == frame) {
      switch (event.type) {
      case OAI_ET:
	update_oai_model(event.key, event.value); 
	user_defined_event = event_list_remove_head(&event_list);
	break;
	
      case SYS_ET:
	update_sys_model(event.key, event.value); 
	user_defined_event = event_list_remove_head(&event_list);
	break;
	    
      case TOPO_ET:
	update_topo_model(event.key, event.value); //implement it with assigning the new values to that of oai_emulation & second thing is to ensure mob model is always read from oai_emulation
	user_defined_event = event_list_remove_head(&event_list);
	break;
	
      case APP_ET:
	update_app_model(event.key, event.value);
	user_defined_event = event_list_remove_head(&event_list);
	break;
	
      case EMU_ET:
	update_emu_model(event.key, event.value); 
	user_defined_event = event_list_remove_head(&event_list);
	break;
	
      case MAC_ET:
	update_mac(event.key, event.value); 
	user_defined_event = event_list_remove_head(&event_list);
	break;
	
      default :
	break;
      }
    } else {
      break;
    }
  }
  
}

void update_mac(char * key, void * value) {

}

void update_oai_model(char * key, void * value) {

}

void update_sys_model(char * key, void * value) {

}

void update_topo_model(char * key, void * value) {
    printf("\n\n\nA NEW MOB MODEL\n\n\n");
/*
    if (key != NULL) { //Global model update
        Mobility * new_mobility_model = (Mobility *) value;

        sprintf(oai_emulation.topology_config.mobility.UE_mobility.UE_mobility_type.selected_option, "%s", new_mobility_model->UE_mobility.UE_mobility_type.selected_option);
        oai_emulation.topology_config.mobility.UE_mobility.grid_walk.grid_map.horizontal_grid = new_mobility_model->UE_mobility.grid_walk.grid_map.horizontal_grid;
        oai_emulation.topology_config.mobility.UE_mobility.grid_walk.grid_map.vertical_grid = new_mobility_model->UE_mobility.grid_walk.grid_map.vertical_grid;
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.grid_walk.grid_trip_type.selected_option, "%s", new_mobility_model->UE_mobility.grid_walk.grid_trip_type.selected_option);
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.UE_initial_distribution.selected_option, "%s", new_mobility_model->UE_mobility.UE_initial_distribution.selected_option);
        oai_emulation.topology_config.mobility.UE_mobility.random_UE_distribution.number_of_nodes = new_mobility_model->UE_mobility.random_UE_distribution.number_of_nodes;
        oai_emulation.topology_config.mobility.UE_mobility.concentrated_UE_distribution.number_of_nodes = new_mobility_model->UE_mobility.concentrated_UE_distribution.number_of_nodes;
        oai_emulation.topology_config.mobility.UE_mobility.grid_UE_distribution.random_grid.number_of_nodes = new_mobility_model->UE_mobility.grid_UE_distribution.random_grid.number_of_nodes;
        oai_emulation.topology_config.mobility.UE_mobility.grid_UE_distribution.border_grid.number_of_nodes = new_mobility_model->UE_mobility.grid_UE_distribution.border_grid.number_of_nodes;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_speed_mps = new_mobility_model->UE_mobility.UE_moving_dynamics.min_speed_mps;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_speed_mps = new_mobility_model->UE_mobility.UE_moving_dynamics.max_speed_mps;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_sleep_ms = new_mobility_model->UE_mobility.UE_moving_dynamics.min_sleep_ms;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_sleep_ms = new_mobility_model->UE_mobility.UE_moving_dynamics.max_sleep_ms;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_journey_time_ms = new_mobility_model->UE_mobility.UE_moving_dynamics.min_journey_time_ms;
        oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_journey_time_ms = new_mobility_model->UE_mobility.UE_moving_dynamics.max_journey_time_ms;
        sprintf(oai_emulation.topology_config.mobility.eNB_mobility.eNB_mobility_type.selected_option, "%s", new_mobility_model->eNB_mobility.eNB_mobility_type.selected_option);
        sprintf(oai_emulation.topology_config.mobility.eNB_mobility.eNB_initial_distribution.selected_option, "%s", new_mobility_model->eNB_mobility.eNB_initial_distribution.selected_option);
        oai_emulation.topology_config.mobility.eNB_mobility.random_eNB_distribution.number_of_cells = new_mobility_model->eNB_mobility.random_eNB_distribution.number_of_cells;
        oai_emulation.topology_config.mobility.eNB_mobility.hexagonal_eNB_distribution.number_of_cells = new_mobility_model->eNB_mobility.hexagonal_eNB_distribution.number_of_cells;
        oai_emulation.topology_config.mobility.eNB_mobility.hexagonal_eNB_distribution.inter_eNB_distance_km = new_mobility_model->eNB_mobility.hexagonal_eNB_distribution.inter_eNB_distance_km;
        oai_emulation.topology_config.mobility.eNB_mobility.grid_eNB_distribution.number_of_grid_x = new_mobility_model->eNB_mobility.grid_eNB_distribution.number_of_grid_x;
        oai_emulation.topology_config.mobility.eNB_mobility.grid_eNB_distribution.number_of_grid_y = new_mobility_model->eNB_mobility.grid_eNB_distribution.number_of_grid_y;

        sprintf(oai_emulation.topology_config.mobility.eNB_mobility.trace_config.trace_mobility_file,"static_1enb.tr");
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.trace_config.trace_mobility_file,"static_2ues.tr");
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.sumo_config.command,"sumo");
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.sumo_config.file,"%s/UTIL/OMG/SUMO/SCENARIOS/scen.sumo.cfg",getenv("OPENAIR2_DIR"));
        sprintf(oai_emulation.topology_config.mobility.UE_mobility.sumo_config.hip,"127.0.1.1");

        oai_emulation.topology_config.mobility.UE_mobility.sumo_config.start = new_mobility_model->UE_mobility.sumo_config.start;
        oai_emulation.topology_config.mobility.UE_mobility.sumo_config.end = new_mobility_model->UE_mobility.sumo_config.end;
        oai_emulation.topology_config.mobility.UE_mobility.sumo_config.step = new_mobility_model->UE_mobility.sumo_config.step; //  1000ms
        oai_emulation.topology_config.mobility.UE_mobility.sumo_config.hport = new_mobility_model->UE_mobility.sumo_config.hport;

    } else { //Updating a single parameter::: Here I'll only put some examples

        if (!strcmp((char *) key, "UE_mobility_type")) {
            sprintf(oai_emulation.topology_config.mobility.UE_mobility.UE_mobility_type.selected_option, "%s", (char *) value);
        } else if (!strcmp((char *) key, "eNB_mobility_type")) {
            sprintf(oai_emulation.topology_config.mobility.eNB_mobility.eNB_mobility_type.selected_option, "%s", (char *) value);
        } else if (!strcmp((char *) key, "grid_trip_type")) {
            sprintf(oai_emulation.topology_config.mobility.UE_mobility.grid_walk.grid_trip_type.selected_option, "%s", (char *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_min_speed_mps")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_speed_mps = *((double *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_max_speed_mps")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_speed_mps = *((double *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_min_sleep_ms")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_sleep_ms = *((double *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_max_sleep_ms")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_sleep_ms = *((double *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_min_journey_time_ms")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.min_journey_time_ms = *((double *) value);
        } else if (!strcmp((char *) key, "UE_moving_dynamics_max_journey_time_ms")) {
            oai_emulation.topology_config.mobility.UE_mobility.UE_moving_dynamics.max_journey_time_ms = *((double *) value);
        }
    }
    */
}

void update_app_model(char * key, void * value) {
    printf("\n\n\nA NEW APP MODEL\n\n\n");
    //int i;
/*


    if (key != NULL) { //Global model update
        Application_Config * new_application_model = (Application_Config *)value;

        sprintf(oai_emulation.application_config.packet_gen_type, "%s", new_application_model->packet_gen_type);

        for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
            sprintf(oai_emulation.application_config.predefined_traffic.source_id[i], "%s", new_application_model->predefined_traffic.source_id[i]);
            sprintf(oai_emulation.application_config.predefined_traffic.application_type[i], "%s", new_application_model->predefined_traffic.application_type[i]);
            sprintf(oai_emulation.application_config.predefined_traffic.background[i], "%s", new_application_model->predefined_traffic.background[i]);
            oai_emulation.application_config.predefined_traffic.aggregation_level[i] = new_application_model->predefined_traffic.aggregation_level[i];
            oai_emulation.application_config.predefined_traffic.destination_id[i] = new_application_model->predefined_traffic.destination_id[i];

            sprintf(oai_emulation.application_config.customized_traffic.source_id[i], "%s", new_application_model->customized_traffic.source_id[i]);
            sprintf(oai_emulation.application_config.customized_traffic.destination_id[i], "%s", new_application_model->customized_traffic.destination_id[i]);
            sprintf(oai_emulation.application_config.customized_traffic.transport_protocol[i], "%s", new_application_model->customized_traffic.transport_protocol[i]);
            sprintf(oai_emulation.application_config.customized_traffic.background[i], "%s", new_application_model->customized_traffic.background[i]);
            sprintf(oai_emulation.application_config.customized_traffic.ip_version[i], "%s", new_application_model->customized_traffic.ip_version[i]);
            oai_emulation.application_config.customized_traffic.aggregation_level[i] = new_application_model->customized_traffic.aggregation_level[i];
            sprintf(oai_emulation.application_config.customized_traffic.idt_dist[i], "%s", new_application_model->customized_traffic.idt_dist[i]);
            oai_emulation.application_config.customized_traffic.idt_min_ms[i] = new_application_model->customized_traffic.idt_min_ms[i];
            oai_emulation.application_config.customized_traffic.idt_max_ms[i] = new_application_model->customized_traffic.idt_max_ms[i];
            oai_emulation.application_config.customized_traffic.idt_standard_deviation[i] = new_application_model->customized_traffic.idt_standard_deviation[i];
            oai_emulation.application_config.customized_traffic.idt_lambda[i] = new_application_model->customized_traffic.idt_lambda[i];
            sprintf(oai_emulation.application_config.customized_traffic.size_dist[i], "%s", new_application_model->customized_traffic.size_dist[i]);
            oai_emulation.application_config.customized_traffic.size_min_byte[i] = new_application_model->customized_traffic.size_min_byte[i];
            oai_emulation.application_config.customized_traffic.size_max_byte[i] = new_application_model->customized_traffic.size_max_byte[i];
            oai_emulation.application_config.customized_traffic.size_standard_deviation[i] = new_application_model->customized_traffic.size_standard_deviation[i];
            oai_emulation.application_config.customized_traffic.size_lambda[i] = new_application_model->customized_traffic.size_lambda[i];
            oai_emulation.application_config.customized_traffic.stream[i] = new_application_model->customized_traffic.stream[i];
            oai_emulation.application_config.customized_traffic.destination_port[i] = new_application_model->customized_traffic.destination_port[i];
            oai_emulation.application_config.customized_traffic.prob_off_pu[i]= new_application_model->customized_traffic.prob_off_pu[i];
            oai_emulation.application_config.customized_traffic.prob_off_ed[i]= new_application_model->customized_traffic.prob_off_ed[i];
            oai_emulation.application_config.customized_traffic.prob_off_pe[i]= new_application_model->customized_traffic.prob_off_pe[i];
            oai_emulation.application_config.customized_traffic.prob_pu_ed[i]= new_application_model->customized_traffic.prob_pu_ed[i];
            oai_emulation.application_config.customized_traffic.prob_pu_pe[i]= new_application_model->customized_traffic.prob_pu_pe[i];
            oai_emulation.application_config.customized_traffic.prob_ed_pe[i]= new_application_model->customized_traffic.prob_ed_pe[i];
            oai_emulation.application_config.customized_traffic.prob_ed_pu[i]= new_application_model->customized_traffic.prob_ed_pu[i];
            oai_emulation.application_config.customized_traffic.holding_time_off_ed[i]= new_application_model->customized_traffic.holding_time_off_ed[i];
            oai_emulation.application_config.customized_traffic.holding_time_off_pu[i]= new_application_model->customized_traffic.holding_time_off_pu[i];
            oai_emulation.application_config.customized_traffic.holding_time_off_pe[i]= new_application_model->customized_traffic.holding_time_off_pe[i];
            oai_emulation.application_config.customized_traffic.holding_time_pe_off[i]= new_application_model->customized_traffic.holding_time_pe_off[i];
            oai_emulation.application_config.customized_traffic.pu_size_pkts[i]= new_application_model->customized_traffic.pu_size_pkts[i];
            oai_emulation.application_config.customized_traffic.ed_size_pkts[i]= new_application_model->customized_traffic.ed_size_pkts[i];
        }
    } else { //Updating a single parameter::: Here I'll only put some examples

        if (!strcmp((char *) key, "packet_gen_type")) {
            sprintf(oai_emulation.application_config.packet_gen_type, "%s", (char *) value);
        } else if (!strcmp((char *) key, "predefined_traffic_source_id")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.predefined_traffic.source_id[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "predefined_traffic_application_type")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.predefined_traffic.application_type[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "predefined_traffic_background")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.predefined_traffic.background[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_source_id")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.source_id[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_destination_id")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.destination_id[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_transport_protocol")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.transport_protocol[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_background")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.background[i], "%s", (char *) value);
            }
        }  else if (!strcmp((char *) key, "customized_traffic_ip_version")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.ip_version[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_idt_dist")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.idt_dist[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_size_dist")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                sprintf(oai_emulation.application_config.customized_traffic.size_dist[i], "%s", (char *) value);
            }
        } else if (!strcmp((char *) key, "predefined_traffic_aggregation_level")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.predefined_traffic.aggregation_level[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "predefined_traffic_destination_id")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.predefined_traffic.destination_id[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_aggregation_level")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.aggregation_level[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_idt_min_ms")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.idt_min_ms[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_idt_max_ms")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.idt_max_ms[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_idt_standard_deviation")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.idt_standard_deviation[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_idt_lambda")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.idt_lambda[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_size_min_byte")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.size_min_byte[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_size_max_byte")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.size_max_byte[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_size_standard_deviation")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.size_standard_deviation[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_size_lambda")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.size_lambda[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_stream")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.stream[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_destination_port")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.destination_port[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_off_pu")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_off_pu[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_off_ed")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_off_ed[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_off_pe")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_off_pe[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_pu_ed")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_pu_ed[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_pu_pe")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_pu_pe[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_ed_pe")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_ed_pe[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_prob_ed_pu")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.prob_ed_pu[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_holding_time_off_ed")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.holding_time_off_ed[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_holding_time_off_pu")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.holding_time_off_pu[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_holding_time_off_pe")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.holding_time_off_pe[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_holding_time_pe_off")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.holding_time_pe_off[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_pu_size_pkts")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.pu_size_pkts[i] = *((int *) value);
            }
        } else if (!strcmp((char *) key, "customized_traffic_ed_size_pkts")) {
            for (i = 0; i < NUMBER_OF_eNB_MAX + NUMBER_OF_UE_MAX; i++) {
                oai_emulation.application_config.customized_traffic.ed_size_pkts[i] = *((int *) value);
            }
        }
    }
    */
}

void update_emu_model(char * key, void * value) {

}

