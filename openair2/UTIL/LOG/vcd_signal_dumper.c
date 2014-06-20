/*******************************************************************************
 * 
 * Eurecom OpenAirInterface 1
 * Copyright(c) 1999 - 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/

/*! \file vcd_signal_dumper.c
 * \brief Dump functions calls and variables to VCD file. Use GTKWave to display this file.
 * \author S. Roux
 * \date 2012
 * \version 0.1
 * \company Eurecom
 * \email: roux@eurecom.fr
 * \note
 * \warning
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <error.h>
#include <time.h>
#include <unistd.h>

#include "assertions.h"
#include "signals.h"

#include "vcd_signal_dumper.h"

#if defined(ENABLE_RTAI_CLOCK)
#include "rtai_lxrt.h"
#endif

#define VCDSIGNALDUMPER_VERSION_MAJOR 0
#define VCDSIGNALDUMPER_VERSION_MINOR 1

// Global variable. If the VCD option is set at execution time, output VCD trace. Otherwise this module has no effect.
int ouput_vcd = 0;

struct vcd_module_s {
    const char     *name;
    int             number_of_signals;
    const char    **signals_names;
    vcd_signal_type signal_type;
    int             signal_size;
} vcd_module_s;
 
const char* eurecomVariablesNames[] = {
    "frame_number_eNB",
    "slot_number_eNB",
    "frame_number_UE",
    "slot_number_UE",
    "daq_mbox",
    "diff2",
    "itti_send_msg",
    "itti_poll_msg",
    "itti_recv_msg",
    "itti_alloc_msg",
    "mp_alloc",
    "mp_free",
};
 
const char* eurecomFunctionsNames[] = {
    "macxface_macphy_init",
    "macxface_macphy_exit",
    "macxface_eNB_dlsch_ulsch_scheduler",
    "macxface_fill_rar",
    "macxface_terminate_ra_proc",
    "macxface_initiate_ra_proc",
    "macxface_cancel_ra_proc",
    "macxface_get_dci_sdu",
    "macxface_get_dlsch_sdu",
    "macxface_rx_sdu",
    "macxface_mrbch_phy_sync_failure",
    "macxface_SR_indication",
    "macxface_phy_config_sib1_eNB",
    "macxface_phy_config_sib2_eNB",
    "macxface_phy_config_dedicated_eNB",
    "macxface_out_of_sync_ind",
    "macxface_ue_decode_si",
    "macxface_ue_decode_ccch",
    "macxface_ue_decode_bcch",
    "macxface_ue_send_sdu",
    "macxface_ue_get_sdu",
    "macxface_ue_get_rach",
    "macxface_ue_process_rar",
    "macxface_ue_scheduler",
    "macxface_ue_get_sr",
    "ue_send_mch_sdu",
    "lte_ue_measurement_procedures",
    "lte_ue_pdcch_procedures",
    "lte_ue_pbch_procedures",
    "phy_procedures_eNb_tx",
    "phy_procedures_eNb_rx",
    "phy_procedures_ue_tx",
    "phy_procedures_ue_rx",
    "phy_procedures_eNB_lte",
    "phy_procedures_UE_lte",
    "eNB_proc0",
    "eNB_proc1",
    "eNB_proc2",
    "eNB_proc3",
    "eNB_proc4",
    "eNB_proc5",
    "eNB_proc6",
    "eNB_proc7",
    "eNB_proc8",
    "eNB_proc9",
    "emu_transport",
    "log_record",
    "pdcp_run",
    "rt_sleep",
    "pdsch_thread",
    "dlsch_thread0",
    "dlsch_thread1",
    "dlsch_thread2",
    "dlsch_thread3",
    "dlsch_thread4",
    "dlsch_thread5",
    "dlsch_thread6",
    "dlsch_thread7",
    "dlsch_decoding0",
    "dlsch_decoding1",
    "dlsch_decoding2",
    "dlsch_decoding3",
    "dlsch_decoding4",
    "dlsch_decoding5",
    "dlsch_decoding6",
    "dlsch_decoding7",
    "rx_pdcch",
    "dci_decoding",
    "rrc_mac_config_req",
    "rrc_ue_decode_sib1",
    "rrc_ue_decode_si",
    "phy_ue_config_sib2",
    "phy_ue_compute_prach",
    "phy_enb_ulsch_decoding",
    "phy_enb_sfgen",
    "phy_enb_prach_rx",
    "phy_enb_pdcch_tx",
    "phy_enb_rs_tx",
    "phy_ue_ulsch_modulation",
    "phy_ue_ulsch_encoding",
    "phy_ue_ulsch_scrambling",
    "phy_eNB_dlsch_modulation",
    "phy_eNB_dlsch_encoding",
    "phy_eNB_dlsch_scrambling",
    "pdcp_apply_security",
    "pdcp_validate_security",
    "itti_enqueue_message",
    "itti_dump_enqueue_message",
    "itti_dump_enqueue_message_malloc",
    "itti_relay_thread",
    "test"
};

struct vcd_module_s vcd_modules[VCD_SIGNAL_DUMPER_MODULE_END] = {
    { "variables", VCD_SIGNAL_DUMPER_VARIABLES_END, eurecomVariablesNames, VCD_WIRE, 64 },
    { "functions", VCD_SIGNAL_DUMPER_FUNCTIONS_END, eurecomFunctionsNames, VCD_WIRE, 1 },
//    { "ue_procedures_functions", VCD_SIGNAL_DUMPER_UE_PROCEDURES_FUNCTIONS_END, eurecomUEFunctionsNames, VCD_WIRE, 1 },
};

FILE *vcd_fd = NULL;
static inline unsigned long long int vcd_get_time(void);

#if defined(ENABLE_USE_CPU_EXECUTION_TIME)
struct timespec     g_time_start;
#elif defined(ENABLE_RTAI_CLOCK)
RTIME start;
#endif


#if defined(ENABLE_VCD_FIFO)

# define VCD_POLL_DELAY         (500)           // Poll delay in micro-seconds
# define VCD_MAX_WAIT_DELAY     (200 * 1000)    // Maximum data ready wait delay in micro-seconds
# define VCD_FIFO_NB_ELEMENTS   (1 << 20)       // Must be a power of 2
# define VCD_FIFO_MASK          (VCD_FIFO_NB_ELEMENTS - 1)

typedef struct vcd_queue_user_data_s {
    uint32_t log_id;
    vcd_signal_dumper_modules module;
    union data_u {
        struct function_s {
            vcd_signal_dump_functions function_name;
            vcd_signal_dump_in_out    in_out;
        } function;
        struct variable_s {
            vcd_signal_dump_variables variable_name;
            unsigned long value;
        } variable;
    } data;

    long long unsigned int time;
} vcd_queue_user_data_t;

typedef struct vcd_fifo_s {
    vcd_queue_user_data_t user_data[VCD_FIFO_NB_ELEMENTS];

    volatile uint32_t write_index;
    volatile uint32_t read_index;
} vcd_fifo_t;

vcd_fifo_t vcd_fifo;

pthread_t vcd_dumper_thread;
#endif

#define BYTE_SIZE   8
#define NIBBLE_SIZE 4
static void uint64_to_binary(uint64_t value, char *binary)
{
    static const char * const nibbles_start[] =
            {
                 "",    "1",   "10",   "11",
              "100",  "101",  "110",  "111",
             "1000", "1001", "1010", "1011",
             "1100", "1101", "1110", "1111",
            };
    static const char * const nibbles[] =
            {
             "0000", "0001", "0010", "0011",
             "0100", "0101", "0110", "0111",
             "1000", "1001", "1010", "1011",
             "1100", "1101", "1110", "1111",
            };
    int nibble;
    int nibble_value;
    int nibble_size;
    int zero = 1;

    for (nibble = 0; nibble < (sizeof (uint64_t) * (BYTE_SIZE / NIBBLE_SIZE)); nibble++)
    {
        nibble_value = value >> ((sizeof (uint64_t) * BYTE_SIZE) - NIBBLE_SIZE);

        if (zero)
        {
            if (nibble_value > 0)
            {
                zero = 0;
                nibble_size = strlen(nibbles_start[nibble_value]);
                memcpy (binary, nibbles_start[nibble_value], nibble_size);
                binary += nibble_size;
            }
        }
        else
        {
            memcpy (binary, nibbles[nibble_value], NIBBLE_SIZE);
            binary += NIBBLE_SIZE;
        }
        value <<= NIBBLE_SIZE;
    }
    /* Add a '0' if the value was null */
    if (zero)
    {
        binary[0] = '0';
        binary ++;
    }
    /* Add a null value at the end of the string */
    binary[0] = '\0';
}

#if defined(ENABLE_VCD_FIFO)
inline static uint32_t vcd_get_write_index(void)
{
    uint32_t write_index;
    uint32_t read_index;

    /* Get current write index and increment it (atomic operation) */
    write_index = __sync_fetch_and_add(&vcd_fifo.write_index, 1);
    /* Wrap index */
    write_index &= VCD_FIFO_MASK;

    /* Check FIFO overflow (increase VCD_FIFO_NB_ELEMENTS if this assert is triggered) */
    DevCheck((read_index = vcd_fifo.read_index, ((write_index + 1) & VCD_FIFO_MASK) != read_index), write_index, read_index, VCD_FIFO_NB_ELEMENTS);

    return write_index;
}

void *vcd_dumper_thread_rt(void *args)
{
    vcd_queue_user_data_t *data;
    char binary_string[(sizeof (uint64_t) * BYTE_SIZE) + 1];
    struct sched_param sched_param;
    uint32_t data_ready_wait;

# if defined(ENABLE_ITTI)
    signal_mask();
# endif

    sched_param.sched_priority = sched_get_priority_min(SCHED_FIFO) + 1;
    sched_setscheduler(0, SCHED_FIFO, &sched_param);

    while(1) {
        if (vcd_fifo.read_index == (vcd_fifo.write_index & VCD_FIFO_MASK)) {
            /* No element -> sleep a while */
            usleep(VCD_POLL_DELAY);
        } else {
            data = &vcd_fifo.user_data[vcd_fifo.read_index];
            data_ready_wait = 0;
            while (data->module == VCD_SIGNAL_DUMPER_MODULE_FREE)
            {
                /* Check wait delay (increase VCD_MAX_WAIT_DELAY if this assert is triggered and that no thread is locked) */
                DevCheck(data_ready_wait < VCD_MAX_WAIT_DELAY, data_ready_wait, VCD_MAX_WAIT_DELAY, 0);

                /* data is not yet ready, wait for it to be completed */
                data_ready_wait += VCD_POLL_DELAY;
                usleep(VCD_POLL_DELAY);
            }
            switch (data->module) {
                case VCD_SIGNAL_DUMPER_MODULE_VARIABLES:
                    if (vcd_fd != NULL)
                    {
                        int variable_name;
                        variable_name = (int)data->data.variable.variable_name;
                        fprintf(vcd_fd, "#%llu\n", data->time);
                        /* Set variable to value */
                        uint64_to_binary(data->data.variable.value, binary_string);
                        fprintf(vcd_fd, "b%s %s_w\n", binary_string,
                                eurecomVariablesNames[variable_name]);
                    }
                    break;

                case VCD_SIGNAL_DUMPER_MODULE_FUNCTIONS:
                    if (vcd_fd != NULL)
                    {
                        int function_name;

                        function_name = (int)data->data.function.function_name;
                        fprintf(vcd_fd, "#%llu\n", data->time);

                        /* Check if we are entering or leaving the function ( 0 = leaving, 1 = entering) */
                        if (data->data.function.in_out == VCD_FUNCTION_IN)
                            /* Set event to 1 */
                            fprintf(vcd_fd, "1%s_w\n", eurecomFunctionsNames[function_name]);
                        else
                            fprintf(vcd_fd, "0%s_w\n", eurecomFunctionsNames[function_name]);
                        fflush(vcd_fd);
                    }
                    break;

                default:
                    DevParam(data->module, 0, 0);
                    break;
            }
            data->module = VCD_SIGNAL_DUMPER_MODULE_FREE;
            vcd_fifo.read_index = (vcd_fifo.read_index + 1) & VCD_FIFO_MASK;
        }
    }
    return NULL;
}
#endif

void vcd_signal_dumper_init(char *filename)
{
    if (ouput_vcd) {
      //        char filename[] = "/tmp/openair_vcd_dump.vcd";

        if ((vcd_fd = fopen(filename, "w+")) == NULL)
        {
            perror("vcd_signal_dumper_init: cannot open file");
            return;
        }

#if defined(ENABLE_USE_CPU_EXECUTION_TIME)
        clock_gettime(CLOCK_MONOTONIC, &g_time_start);
#elif defined(ENABLE_RTAI_CLOCK)
        start=rt_get_time_ns();
#endif

        vcd_signal_dumper_create_header();

#if defined(ENABLE_VCD_FIFO)
        vcd_fifo.write_index = 0;
        vcd_fifo.read_index = 0;

        fprintf(stderr, "[VCD] Creating dumper thread\n");

        if (pthread_create(&vcd_dumper_thread, NULL, vcd_dumper_thread_rt, NULL) < 0)
        {
            fprintf(stderr, "vcd_signal_dumper_init: Failed to create thread: %s\n",
                    strerror(errno));
            ouput_vcd = 0;
            return;
        }
#endif
    }
}

void vcd_signal_dumper_close(void)
{
    if (ouput_vcd) {
#if defined(ENABLE_VCD_FIFO)
        
#else
        if (vcd_fd != NULL)
        {
            fclose(vcd_fd);
            vcd_fd = NULL;
        }
#endif
    }
}

static inline void vcd_signal_dumper_print_time_since_start(void)
{
    if (vcd_fd != NULL)
    {
#if defined(ENABLE_USE_CPU_EXECUTION_TIME)
        struct timespec time;
        long long unsigned int nanosecondsSinceStart;
        long long unsigned int secondsSinceStart;

        clock_gettime(CLOCK_MONOTONIC, &time);

        /* Get current execution time in nanoseconds */
        nanosecondsSinceStart = (long long unsigned int)((time.tv_nsec - g_time_start.tv_nsec));
        secondsSinceStart     = (long long unsigned int)time.tv_sec - (long long unsigned int)g_time_start.tv_sec;
        /* Write time in nanoseconds */
        fprintf(vcd_fd, "#%llu\n", nanosecondsSinceStart + (secondsSinceStart * 1000000000UL));
#elif defined(ENABLE_RTAI_CLOCK)
        /* Write time in nanoseconds */
        fprintf(vcd_fd, "#%llu\n",rt_get_time_ns()-start);
#endif
    }
}

static inline unsigned long long int vcd_get_time(void)
{
#if defined(ENABLE_USE_CPU_EXECUTION_TIME)
    struct timespec time;

    clock_gettime(CLOCK_MONOTONIC, &time);

    return (long long unsigned int)((time.tv_nsec - g_time_start.tv_nsec)) +
    ((long long unsigned int)time.tv_sec - (long long unsigned int)g_time_start.tv_sec) * 1000000000UL;
#elif defined(ENABLE_RTAI_CLOCK)
    return rt_get_time_ns() - start;
#endif
}

void vcd_signal_dumper_create_header(void)
{
    if (ouput_vcd) {
        struct tm *pDate;
        time_t intps;

        intps = time(NULL);
        pDate = localtime(&intps);

        if (vcd_fd != NULL)
        {
            int i, j;
            fprintf(vcd_fd, "$date\n\t%s$end\n", asctime(pDate));
            // Display version
            fprintf(vcd_fd, "$version\n\tVCD plugin ver%d.%d\n$end\n", VCDSIGNALDUMPER_VERSION_MAJOR, VCDSIGNALDUMPER_VERSION_MINOR);
            // Init timescale, here = 1ns
            fprintf(vcd_fd, "$timescale 1 ns $end\n");

            /* Initialize each module definition */
            for(i = 0; i < VCD_SIGNAL_DUMPER_MODULE_END; i++) {
                struct vcd_module_s *module;
                module = &vcd_modules[i];
                fprintf(vcd_fd, "$scope module %s $end\n", module->name);
                /* Declare each signal as defined in array */
                for (j = 0; j < module->number_of_signals; j++) {
                    const char *signal_name;
                    signal_name = module->signals_names[j];
                    if (VCD_WIRE == module->signal_type) {
                        fprintf(vcd_fd, "$var wire %d %s_w %s $end\n", module->signal_size, signal_name, signal_name);
                    } else  if (VCD_REAL == module->signal_type) {
                        fprintf(vcd_fd, "$var real %d %s_r %s $end\n", module->signal_size, signal_name, signal_name);
                    } else {
                        // Handle error here
                    }
                }
                fprintf(vcd_fd, "$upscope $end\n");
            }

            /* Init variables and functions to 0 */
            fprintf(vcd_fd, "$dumpvars\n");
            for(i = 0; i < VCD_SIGNAL_DUMPER_MODULE_END; i++) {
                struct vcd_module_s *module;
                module = &vcd_modules[i];
                /* Declare each signal as defined in array */
                for (j = 0; j < module->number_of_signals; j++) {
                    const char *signal_name;
                    signal_name = module->signals_names[j];
                    if (VCD_WIRE == module->signal_type) {
                        if (module->signal_size > 1) {
                            fprintf(vcd_fd, "b0 %s_w $end\n", signal_name);
                        }
                        else {
                            fprintf(vcd_fd, "0%s_w $end\n", signal_name);
                        }
                    } else  if (VCD_REAL == module->signal_type) {
                        fprintf(vcd_fd, "r0 %s_r $end\n", signal_name);
                    } else {
                        // Handle error here
                    }
                }
            }
            fprintf(vcd_fd, "$end\n");
            fprintf(vcd_fd, "$enddefinitions $end\n\n");
            //fflush(vcd_fd);
        }
    }
}

void vcd_signal_dumper_dump_variable_by_name(vcd_signal_dump_variables variable_name,
                                             unsigned long             value)
{
    DevCheck((0 <= variable_name) && (variable_name < VCD_SIGNAL_DUMPER_VARIABLES_END),
             variable_name, VCD_SIGNAL_DUMPER_VARIABLES_END, 0);

    if (ouput_vcd) {
#if defined(ENABLE_VCD_FIFO)
        uint32_t write_index = vcd_get_write_index();

        vcd_fifo.user_data[write_index].time = vcd_get_time();
        vcd_fifo.user_data[write_index].data.variable.variable_name = variable_name;
        vcd_fifo.user_data[write_index].data.variable.value = value;
        vcd_fifo.user_data[write_index].module = VCD_SIGNAL_DUMPER_MODULE_VARIABLES; // Set when all other fields are set to validate the user_data
#else
        char binary_string[(sizeof (uint64_t) * BYTE_SIZE) + 1];

        if (vcd_fd != NULL)
        {
            vcd_signal_dumper_print_time_since_start();

            /* Set variable to value */
            uint64_to_binary(value, binary_string);
            fprintf(vcd_fd, "b%s %s_w\n", binary_string, eurecomVariablesNames[variable_name]);
            //fflush(vcd_fd);
        }
#endif
    }
}

void vcd_signal_dumper_dump_function_by_name(vcd_signal_dump_functions  function_name,
                                             vcd_signal_dump_in_out     in_out)
{
    DevCheck((0 <= function_name) && (function_name < VCD_SIGNAL_DUMPER_FUNCTIONS_END),
             function_name, VCD_SIGNAL_DUMPER_FUNCTIONS_END, 0);

    if (ouput_vcd) {
#if defined(ENABLE_VCD_FIFO)
        uint32_t write_index = vcd_get_write_index();

        vcd_fifo.user_data[write_index].time = vcd_get_time();
        vcd_fifo.user_data[write_index].data.function.function_name = function_name;
        vcd_fifo.user_data[write_index].data.function.in_out = in_out;
        vcd_fifo.user_data[write_index].module = VCD_SIGNAL_DUMPER_MODULE_FUNCTIONS; // Set when all other fields are set to validate the user_data
#else
        if (vcd_fd != NULL)
        {
            vcd_signal_dumper_print_time_since_start();

            /* Check if we are entering or leaving the function ( 0 = leaving, 1 = entering) */
            if (in_out == VCD_FUNCTION_IN)
                /* Set event to 1 */
                fprintf(vcd_fd, "1%s_w\n", eurecomFunctionsNames[function_name]);
            else
                fprintf(vcd_fd, "0%s_w\n", eurecomFunctionsNames[function_name]);
            //fflush(vcd_fd);
        }
#endif
    }
}

