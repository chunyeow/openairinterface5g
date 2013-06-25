#include "oaisim.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "UTIL/FIFO/pad_list.h"

void get_simulation_options(int argc, char *argv[]);

void check_and_adjust_params();

void init_omv();

void init_seed(u8 set_seed);

void init_openair1();

void init_openair2();

void init_ocm();

void init_otg_pdcp_buffer();

void update_omg();

void update_omg_ocm();

void update_ocm();

void update_otg_eNB(int module_id, unsigned int ctime);

void update_otg_UE(int module_id, unsigned int ctime);

void exit_fun(const char* s);

void init_pad();

