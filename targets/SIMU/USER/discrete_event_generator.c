#include "discrete_event_generator.h"
#include <stdio.h>
#include <stdlib.h>

extern u32 frame;
End_Of_Sim_Event end_event; //Could later be a list of condition_events (if the end condition is more complicated)
Event_List event_list;

void add_event(struct Event event) {

    Event_elt * counter = event_list.head;
    Event_elt * previous = counter;
    Event_elt * elt = malloc(sizeof(Event_elt));

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

void schedule(enum Event_Type type, int frame, char * key, void * value) {
    Event event;
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

void schedule_end_of_simulation(End_Of_Sim_Event_Type type, int value) {
    end_event.type = type;
    end_event.value = value;
}

int end_of_simulation() {
  switch (end_event.type) {
    case FRAME:
        if (frame == end_event.value)
            return 1;
        break;

   /* default:
      return 0;*/
    }
  return 0;
}

void update_omg_model(char * key, void * value) {
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

void update_otg_model(char * key, void * value) {
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
