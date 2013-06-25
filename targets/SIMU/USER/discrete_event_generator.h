#include "oaisim.h"
#include "UTIL/FIFO/pad_list.h"

void add_event(struct Event event);

void schedule(enum Event_Type type, int frame, char * key, void* value);

void schedule_end_of_simulation(End_Of_Sim_Event_Type type, int value);

int end_of_simulation();

void update_omg_model(char * key, void * value);

void update_otg_model(char * key, void * value);
