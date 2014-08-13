/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
//**************************************************************
// Compile with:
// $ make colabsim
//**************************************************************

#include <string.h>
#include <math.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

#include "SIMULATION/TOOLS/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"

#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"
#include "SCHED/defs.h"
#include "SCHED/vars.h"
#include "LAYER2/MAC/vars.h"

#include "OCG_vars.h"

#ifndef RA_RNTI
#define RA_RNTI 0xfffe
#endif

typedef unsigned char bool;
const bool false = 0;
const bool true = 1;

#define BW 7.68
#define N_PRB 25
#define RBG_SIZE 2
#define NID_CELL 0
#define MAX_RELAYS 8
#define MAX_HARQ_ROUNDS 4
#define MAX_FRAMES 2*MAX_HARQ_ROUNDS+1

const uint8_t cp_type = 0;         // Normal cyclic prefix
const uint8_t n_txantenna_ch = 1;  // Number of CH transmit antennas
const uint8_t n_rxantenna_ch = 1;  // Number of CH receive antennas
const uint8_t n_txantenna_mr = 1;  // Number of MR transmit antennas
const uint8_t n_rxantenna_mr = 1;  // Number of MR receive antennas
const uint8_t oversampling = 1;
const uint8_t subframe_hop1 = 1;   // Subframe for CH1 PDCCH+PDSCH transmission
const uint8_t subframe_hop2 = 7;  // Subframe for MR PDU to CH2
const uint8_t n_pdcch_symbols = 3; // Number of PDCCH symbols in DL subframes

typedef enum {
  analysis_single,      // Simulate one SNR point
  analysis_snrsweep_a,  // Sweep SNR of first relay from negative to positive 
                        // for both hops
  analysis_snrsweep_b,  // Sweep SNR of first relay from negative to positive 
                        // for hop 1 and from postive to negative for hop 2
  analysis_snrsweep_c
} analysis_t;

typedef enum {
  strategy_wait_all, // Wait for all relays to decode before starting hop 2
  strategy_wait_one  // Start hop 2 when one relay has decoded
} strategy_t;

// Structure for command line parsed arguments
typedef struct {
  bool debug_output;              // Output MATLAB signal files
  int verbose;                    // Verbosity level
  analysis_t analysis;            // Analysis mode
  int range;                      // Sweep range
  double step;                    // Sweep step size
  strategy_t strategy;            // HARQ strategy
  int n_relays;                   // Number of relays
  int n_pdu;                      // Number of MAC PDUs to simulate
  int n_harq;                     // Maximum number of HARQ rounds
  int mcs_hop1;                   // MCS for hop 1
  int mcs_hop2;                   // MCS for hop 2
  int n_prb_hop1;                 // Number of PRB utilized in hop 1
  int n_prb_hop2;                 // Number of PRB utilized in hop 2
  bool autorb;                    // Reduce number of PRB to balance TBS
  SCM_t channel_model;            // Channel model
  double channel_correlation;     // Channel reutilization factor
  double snr_hop1[MAX_RELAYS];    // SNR used in hop 1 for all links
  double snr_hop2[MAX_RELAYS];    // SNR used in hop 2 for all links
  const char* results_fn;         // File to save simulation results to
} args_t;

// Structure containing link simulation results for one test
// Note: mcs_hop*, tbs_hop*, n_prb_hop* are matrices containing values for each
//   transmission attempt in the simulation. The first index is the MAC PDU
//   index and the second is the HARQ round index. Currently no AMC is
//   implemented, and all these values are the same for each MAC PDU.
// Note: The meaning of n_harq_success_hop1 is dependent on the HARQ strategy.
//   In HARQ strategy 1, both relays must be decode.
//   Other HARQ strategies are not implemented yet.
typedef struct {
  int n_relays;             // number of relays
  int n_pdu;                // number of transmitted MAC PDUs
  int n_harq;               // number of HARQ rounds
  SCM_t channel_model;      // used channel model
  double* snr_hop1;         // SNRs for each link in hop 1
  double* snr_hop2;         // SNRs for each link in hop 2
  int** mcs_hop1;           // MCS used in hop 1
  int** mcs_hop2;           // MCS used in hop 2
  int** tbs_hop1;           // transport block size for hop 1
  int** tbs_hop2;           // transport block size for hop 2
  int** n_prb_hop1;         // number of used PRBs in hop 1
  int** n_prb_hop2;         // number of used PRBs in hop 2
  int n_frames_hop1;        // number of transmitted LTE frames in hop 1
  int n_frames_hop2;        // number of transmitted LTE frames in hop 2
  int n_bits_hop1;          // number of correctly received information bits over hop 1
  int n_bits_hop2;          // number of correctly received information bits over hop 2
  double ber_hop1[MAX_RELAYS]; // raw BER in hop 1
  double ber_hop2;          // raw BER in hop 2
  int n_pdu_success_hop1;   // number of correctly received MAC PDUs in hop 1
  int n_pdu_success_hop2;   // number of correctly received MAC PDUs in hop 2
  int n_harq_tries_hop1[MAX_HARQ_ROUNDS]; // number of transmitted MAC PDUs in each HARQ round in hop 1
  int n_harq_success_hop1[MAX_HARQ_ROUNDS]; // number of successfully decoded MAC PDUs in each HARQ round in hop 1
  int n_harq_tries_hop2[MAX_HARQ_ROUNDS]; // number of transmitted MAC PDUs in each HARQ round in hop 2
  int n_harq_success_hop2[MAX_HARQ_ROUNDS]; // number of successfully decoded MAC PDUs in each HARQ round in hop 2
  int n_transmissions[MAX_HARQ_ROUNDS][MAX_HARQ_ROUNDS]; // PDF of number of transmissions in the two hops for
                                                         // MAC PDUs correctly received at CH2
  int* relay_activity;      // PDF of relay activity, [1]: MR1 active, [2]: MR2 active, [3]: MR1+MR2 active
} results_t;

// Relay role in distributed Alamouti coding
typedef enum {
  RELAY_ROLE_STANDARD,    // Relay sends [ x1   x2 ]
  RELAY_ROLE_ALTERNATE    // Relay sends [-x2*  x1*]
} relay_role_t;

typedef struct {
  double* s_re[1];
  double* s_im[1];
  double* r_re[1];
  double* r_im[1];
  double* r_re_t[1];
  double* r_im_t[1];
} channel_vars_t;

typedef struct {
  channel_vars_t* cvars;
  channel_desc_t* channel;
} sh_channel_t;

// Simulation context
typedef struct {
  LTE_DL_FRAME_PARMS* frame_parms;
  PHY_VARS_eNB* phy_vars_ch_src;
  PHY_VARS_eNB* phy_vars_ch_dest;
  PHY_VARS_UE** phy_vars_mr;
  sh_channel_t** channels_hop1;
  sh_channel_t** channels_hop2;
  int32_t* rxdata[1];
  double* snr_hop1;
  double* snr_hop2;
  int mcs_hop1;
  int mcs_hop2;
  uint32_t tbs_hop1;
  uint32_t tbs_hop2;
  uint32_t tbs_col;
  uint16_t rnti_hop1;
  uint16_t rnti_hop2;
  int input_buffer_length;
  uint8_t* input_buffer;
  int mr_buffer_length;
  uint8_t* mr_buffer[MAX_RELAYS];
  uint32_t n_coded_bits_hop1;
  uint32_t n_coded_bits_hop2;
  int n_ber_frames_hop1[MAX_RELAYS];
  int n_ber_frames_hop2;
  uint8_t n_avail_pdcch_symbols;
  uint8_t subframe_hop1;
  uint8_t subframe_hop2;
  uint8_t harq_pid_hop2;
} context_t;

void transmit_one_pdu(args_t* args, context_t* context, int pdu, results_t* results);
int parse_args(int argc, char** argv, args_t* args);
int parse_channel_model(const char* str, SCM_t* model);
bool parse_snr(const char* str, double* snr, int n);
void print_usage(const char* prog);
void print_channel_usage();
void signal_handler(int sig);
void setup_single(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays);
void setup_snrsweep_a(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end);
void setup_snrsweep_b(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end);
void setup_snrsweep_c(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end);
void setup_frame_params(LTE_DL_FRAME_PARMS* frame_parms, unsigned char transmission_mode);
void setup_phy_vars(LTE_DL_FRAME_PARMS* frame_parms, PHY_VARS_eNB* phy_vars_ch_src, 
    PHY_VARS_UE** phy_vars_mr, PHY_VARS_eNB* phy_vars_ch_dest, int n_relays);
uint16_t rballoc_type0(int n_rb, int rbg_size);
void setup_broadcast_dci(DCI_ALLOC_t* dci, uint16_t rnti, int harq_round, int mcs, int n_rb);
void setup_distributed_dci(DCI_ALLOC_t* dci, uint16_t rnti, int harq_round, int mcs, int n_rb);
void alloc_broadcast_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays, uint16_t rnti);
void free_broadcast_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays);
void alloc_distributed_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays, uint16_t rnti);
void free_distributed_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays);
void ofdm_modulation(mod_sym_t** tx_f, int32_t** tx_t, LTE_DL_FRAME_PARMS* frame_parms, uint8_t subframe, uint8_t nsymb);
channel_vars_t alloc_channel_vars(LTE_DL_FRAME_PARMS* frame_parms);
void free_channel_vars(channel_vars_t v);
sh_channel_t* alloc_sh_channel(channel_vars_t* cvars, SCM_t channel_model, int n_txantennas, int n_rxantennas, double channel_correlation);
void free_sh_channel(sh_channel_t* c);
void transmit_subframe(sh_channel_t* channel, int32_t** src, LTE_DL_FRAME_PARMS* frame_parms, uint8_t subframe, uint8_t nsymb, double ampl, bool accumulate);
void deliver_subframe(sh_channel_t* channel, int32_t** dst, LTE_DL_FRAME_PARMS* frame_parms, uint8_t subframe, uint8_t nsymb, double stddev);
void ofdm_fep(PHY_VARS_UE* phy_vars_mr, uint8_t subframe);
int rx_dlsch_symbol(PHY_VARS_UE* phy_vars, uint8_t subframe, uint8_t symbol, uint8_t first_symbol);
uint32_t get_ulsch_G(LTE_UE_ULSCH_t *ulsch, uint8_t harq_pid);
double compute_ber_soft(uint8_t* ref, int16_t* rec, int n);
void print_dlsch_eNB_stats(LTE_eNB_DLSCH_t* d);
void print_dlsch_ue_stats(LTE_UE_DLSCH_t* d);
void print_ulsch_ue_stats(LTE_UE_ULSCH_t* d);
void print_ulsch_eNB_stats(LTE_eNB_ULSCH_t* d);
int block_valid(uint8_t* ref, uint8_t* rec, int n);
void init_results(results_t* r, args_t* a);
void clear_results(results_t* r);
void free_results(results_t* r);
void print_results(results_t* r);
void write_results_header(FILE* f, results_t* r, int n_tests);
void write_results_data(FILE* f, results_t* r);
double calc_delay(int* n_frames, int n_harq);

// Function declarations missing in LTE_TRANSPORT/proto.h:
uint8_t pdcch_alloc2ul_subframe(LTE_DL_FRAME_PARMS* frame_parms, uint8_t n);
uint8_t ul_subframe2pdcch_alloc_subframe(LTE_DL_FRAME_PARMS* frame_parms, uint8_t n);

int main(int argc, char **argv) {
  args_t args;
  results_t results;
  context_t context;

  DCI_ALLOC_t dci_hop1;
  DCI_ALLOC_t dci_hop2;

  channel_vars_t channel_vars;

  double* snrs;
  int n_tests;

  int test;  // Current test
  int pdu;   // Current MAC PDU
  int k;

  bool store_results = false;
  FILE* results_file = 0;

  // Parse arguments
  k = parse_args(argc, argv, &args);
  if(k == 1) {
    print_usage(argv[0]);
    exit(1);
  }
  else if(k == 2) {
    print_channel_usage();
    exit(1);
  }

  // Check argument bounds
  if(args.n_relays > MAX_RELAYS) {
    printf("Too many relays, increase MAX_RELAYS\n");
    exit(1);
  }

  if(args.n_harq > MAX_HARQ_ROUNDS) {
    printf("Too many HARQ rounds, increase MAX_HARQ_ROUNDS\n");
    exit(1);
  }

  // General setup
  signal(SIGSEGV, signal_handler);
  randominit(0);
  set_taus_seed(0);

  // Allocate memory for frame parameters and node structures
  context.frame_parms = malloc(sizeof(LTE_DL_FRAME_PARMS));
  memset(context.frame_parms, 0, sizeof(LTE_DL_FRAME_PARMS));
  context.phy_vars_ch_src = malloc(sizeof(PHY_VARS_eNB));
  context.phy_vars_ch_dest = malloc(sizeof(PHY_VARS_eNB));
  context.phy_vars_mr = malloc(args.n_relays*sizeof(PHY_VARS_UE*));
  for(k = 0; k < args.n_relays; k++) {
    context.phy_vars_mr[k] = malloc(sizeof(PHY_VARS_UE));
    memset(context.phy_vars_mr[k], 0, sizeof(PHY_VARS_UE));
  }
  memset(context.phy_vars_ch_src, 0, sizeof(PHY_VARS_eNB));
  memset(context.phy_vars_ch_dest, 0, sizeof(PHY_VARS_eNB));

  // Initialize log
  logInit();

  // Initialize result data
  init_results(&results, &args);

  // Allocate channel structures
  context.channels_hop1 = malloc(args.n_relays*sizeof(sh_channel_t*));
  context.channels_hop2 = malloc(args.n_relays*sizeof(sh_channel_t*));
  memset(context.channels_hop1, 0, args.n_relays*sizeof(sh_channel_t*));
  memset(context.channels_hop2, 0, args.n_relays*sizeof(sh_channel_t*));

  // Setup analysis structures
  switch(args.analysis) {
  case analysis_single:
    setup_single(&snrs, &n_tests, args.snr_hop1, args.snr_hop2, args.n_relays);
    break;
  case analysis_snrsweep_a:
    setup_snrsweep_a(&snrs, &n_tests, args.snr_hop1, args.snr_hop2, args.n_relays, args.step, -args.range, args.range);
    break;
  case analysis_snrsweep_b:
    setup_snrsweep_b(&snrs, &n_tests, args.snr_hop1, args.snr_hop2, args.n_relays, args.step, -args.range, args.range);
    break;
  case analysis_snrsweep_c:
    setup_snrsweep_c(&snrs, &n_tests, args.snr_hop1, args.snr_hop2, args.n_relays, args.step, -args.range, args.range);
    break;
  }

  // Open results file (if requested)
  if(args.results_fn) {
    store_results = true;
    results_file = fopen(args.results_fn, "w");
    if(!results_file) {
      perror("fopen");
      exit(1);
    }
  }

  if(store_results) {
    write_results_header(results_file, &results, n_tests);
  }

  // Setup PHY structures
  setup_frame_params(context.frame_parms, 1);
  setup_phy_vars(context.frame_parms, context.phy_vars_ch_src, context.phy_vars_mr, context.phy_vars_ch_dest, args.n_relays);

  // Setup simulation context
  context.rnti_hop1 = 0x1515;
  context.rnti_hop2 = 0x1516;
  context.mcs_hop1 = args.mcs_hop1;
  context.mcs_hop2 = args.mcs_hop2;
  context.n_avail_pdcch_symbols = n_pdcch_symbols;
  context.subframe_hop1 = subframe_hop1;
  context.subframe_hop2 = subframe_hop2;

  // Allocate temporary signal structures
  context.rxdata[0] = malloc(10*context.frame_parms->samples_per_tti);

  // Allocate first hop transport channel
  alloc_broadcast_transport_channel(context.phy_vars_ch_src, context.phy_vars_mr, args.n_relays, context.rnti_hop1);
        
  // Allocate second hop transport channel
  alloc_distributed_transport_channel(context.phy_vars_ch_dest, context.phy_vars_mr, args.n_relays, context.rnti_hop2);
  
  // Setup channel structures
  channel_vars = alloc_channel_vars(context.frame_parms);
  for(k = 0; k < args.n_relays; k++) {
    context.channels_hop1[k] = alloc_sh_channel(&channel_vars, args.channel_model, n_txantenna_ch, n_rxantenna_mr, args.channel_correlation);
    context.channels_hop2[k] = alloc_sh_channel(&channel_vars, args.channel_model, n_txantenna_mr, n_rxantenna_ch, args.channel_correlation);
  }
  // Create broadcast DCI and generate transport channel parameters,
  // in order to determine hop 1 transfer block size and number of coded bits
  setup_broadcast_dci(&dci_hop1, context.rnti_hop1, 0, args.mcs_hop1, args.n_prb_hop1);
  generate_eNB_dlsch_params_from_dci(subframe_hop1, dci_hop1.dci_pdu, 
      context.rnti_hop1, format1, context.phy_vars_ch_src->dlsch_eNB[0], context.frame_parms, 
      SI_RNTI, RA_RNTI, P_RNTI,
      context.phy_vars_ch_src->eNB_UE_stats[0].DL_pmi_single);
  context.tbs_hop1 = context.phy_vars_ch_src->dlsch_eNB[0][0]->harq_processes[0]->TBS;
  context.n_coded_bits_hop1 = get_G(context.frame_parms, context.phy_vars_ch_src->dlsch_eNB[0][0]->nb_rb,
      context.phy_vars_ch_src->dlsch_eNB[0][0]->rb_alloc,
      get_Qm(context.phy_vars_ch_src->dlsch_eNB[0][0]->harq_processes[0]->mcs),
      context.n_avail_pdcch_symbols, subframe_hop1);

  // Create distributed DCI and generate transport channel parameters,
  // in order to determine hop 2 transfer block size and number of coded bits
  context.harq_pid_hop2 = subframe2harq_pid(context.frame_parms, 0, subframe_hop2);
  setup_distributed_dci(&dci_hop2, context.rnti_hop2, 0, args.mcs_hop2, args.n_prb_hop2);
  generate_ue_ulsch_params_from_dci(dci_hop2.dci_pdu, context.rnti_hop2, 
				    ul_subframe2pdcch_alloc_subframe(context.frame_parms, subframe_hop2),
				    format0, context.phy_vars_mr[0], SI_RNTI, RA_RNTI, P_RNTI, 0, 0);
  context.tbs_hop2 = context.phy_vars_mr[0]->ulsch_ue[0]->harq_processes[context.harq_pid_hop2]->TBS;
  context.n_coded_bits_hop2 = get_ulsch_G(context.phy_vars_mr[0]->ulsch_ue[0], context.harq_pid_hop2);

  if(args.verbose > 1) {
    print_dlsch_eNB_stats(context.phy_vars_ch_src->dlsch_eNB[0][0]);
    print_ulsch_ue_stats(context.phy_vars_mr[0]->ulsch_ue[0]);
    dump_dci(context.frame_parms, &dci_hop1);
    dump_dci(context.frame_parms, &dci_hop2);
  }
  printf("Hop 1: TBS=%d, G=%d, rate=%f. Hop 2: TBS=%d, G=%d, rate=%f\n", 
      context.tbs_hop1, context.n_coded_bits_hop1, (float)context.tbs_hop1/(float)context.n_coded_bits_hop1,
      context.tbs_hop2, context.n_coded_bits_hop2, (float)context.tbs_hop2/(float)context.n_coded_bits_hop2);

  context.tbs_col = context.tbs_hop1 < context.tbs_hop2 ? context.tbs_hop1 : context.tbs_hop2;

  // Allocate input buffer
  context.input_buffer_length = context.tbs_hop1/8;
  context.input_buffer = malloc(context.input_buffer_length+4);
  memset(context.input_buffer, 0, context.input_buffer_length+4);
  if(args.verbose > 0)
    printf("Input buffer: %d bytes\n", context.input_buffer_length);

  // Allocate MR data buffers
  context.mr_buffer_length = context.tbs_hop2/8;
  for(k = 0; k < args.n_relays; k++) {
    context.mr_buffer[k] = malloc(context.mr_buffer_length+4);
    memset(context.mr_buffer[k], 0, context.mr_buffer_length+4);
  }

  for(test = 0; test < n_tests; test++) {
    // Set SNRs
    context.snr_hop1 = &snrs[2*args.n_relays*test];
    context.snr_hop2 = &snrs[2*args.n_relays*test + args.n_relays];

    // Clear results
    clear_results(&results);
    for(k = 0; k < args.n_relays; k++) {
      context.n_ber_frames_hop1[k] = 0;
    }
    context.n_ber_frames_hop2 = 0;
    results.snr_hop1 = context.snr_hop1;
    results.snr_hop2 = context.snr_hop2;

    // Print test info.
    printf("\n*** Test %d/%d ***\n", test+1, n_tests);

    for(pdu = 0; pdu < args.n_pdu; pdu++) {
      transmit_one_pdu(&args, &context, pdu, &results);
    }

    // Compute average BER for the links
    for(k = 0; k < args.n_relays; k++)
      if(context.n_ber_frames_hop1[k] > 0)
        results.ber_hop1[k] /= (double)context.n_ber_frames_hop1[k];
      else
        results.ber_hop1[k] = 0.0;
    if(context.n_ber_frames_hop2 > 0)
      results.ber_hop2 /= (double)context.n_ber_frames_hop2;
    else
      results.ber_hop2 = 0.0;

    print_results(&results);
    if(store_results)
      write_results_data(results_file, &results);
  }

  if(store_results)
    fclose(results_file);

  free_results(&results);
  free(context.input_buffer);
  free(context.rxdata[0]);
  free_broadcast_transport_channel(context.phy_vars_ch_src, context.phy_vars_mr, args.n_relays);
  free_distributed_transport_channel(context.phy_vars_ch_dest, context.phy_vars_mr, args.n_relays);
  free(snrs);
  for(k = 0; k < args.n_relays; k++) {
    free_sh_channel(context.channels_hop1[k]);
    free_sh_channel(context.channels_hop2[k]);
  }
  free_channel_vars(channel_vars);
  free(context.channels_hop1);
  free(context.channels_hop2);
  free(context.phy_vars_ch_src);
  free(context.phy_vars_ch_dest);
  for(k = 0; k < args.n_relays; k++)
    free(context.phy_vars_mr[k]);
  free(context.phy_vars_mr);
  free(context.frame_parms);

  return 0;
}

void transmit_one_pdu(args_t* args, context_t* context, int pdu, results_t* results)
{
  // State variables:
  int frame = 0;      // Current LTE frame
  bool hop1_active = false;
  bool hop2_active = false;
  bool start_hop2;
  int round_hop1 = 0; // Current HARQ round in hop 1
  int round_hop2 = 0; // Current HARQ round in hop 2
  int rounds_hop1;    // Number of rounds in hop 1 before hop 2 started
  int rounds_hop2;    // Number of rounds in hop 2 until CH decoded
  relay_role_t relay_role[MAX_RELAYS]; // the roles of the relays in Alamouti coding
  bool decoded_at_all_mr;
  bool decoded_at_mr[MAX_RELAYS];
  bool activate_mr[MAX_RELAYS];
  bool decoded_at_ch;

  LTE_DL_FRAME_PARMS* frame_parms = context->frame_parms;
  PHY_VARS_eNB* phy_vars_ch_src = context->phy_vars_ch_src;
  PHY_VARS_eNB* phy_vars_ch_dest = context->phy_vars_ch_dest;
  PHY_VARS_UE** phy_vars_mr = context->phy_vars_mr;

  DCI_ALLOC_t dci_hop1;
  DCI_ALLOC_t dci_hop2;

  int n_symbols_per_slot = (cp_type == 0 ? 7 : 6);
  int pilot1_symbol = (cp_type == 0 ? 4 : 3);

  // Temporary variables
  int n_re_hop1;
  int n_re_hop2;
  int n_active_relays;
  uint8_t n_used_pdcch_symbols;
  uint32_t tx_energy;
  //double awgn_stddev;
  double tx_ampl;
  double raw_ber;
  double awgn_stddev;
  bool accumulate_at_rx;
  int i;
  int k;
  int l;
  int n_iter;

  // Temporary strings
  char fnbuf[80];
  char varbuf[80];

  if(args->verbose == 0) {
    printf("Transmitting MAC PDU %d\r", pdu);
    fflush(stdout);
  } else
    printf("Transmitting MAC PDU %d\n", pdu);

  for(k = 0; k < args->n_relays; k++)
    decoded_at_mr[k] = false;
  decoded_at_ch = false;

  // Set role of each relay (alternating STANDARD and ALTERNATE)
  for(k = 0; k < args->n_relays; k++) {
    relay_role[k] = k & 1;
  }

  // Generate input data
  for(k = 0; k < context->input_buffer_length; k++)
    context->input_buffer[k] = (uint8_t)(taus()&0xff);

  hop1_active = true;
  while(hop1_active || hop2_active) {
    if(args->verbose > 0) {
      fprintf(stderr, "LTE frame %d: hop 1 %s, hop 2 %s, decoded at relays: ", frame, 
          hop1_active ? "active" : "inactive",
          hop2_active ? "active" : "inactive");
      for(k = 0; k < args->n_relays; k++)
        fprintf(stderr, "%s", decoded_at_mr[k] ? "X" : ".");
      fprintf(stderr, "\n");
    }

    for(k = 0; k < args->n_relays; k++)
      activate_mr[k] = false;

    // Do hop 1 transmission if hop 1 is active
    if(hop1_active) {
      // Clear txdataF vector
      memset(&phy_vars_ch_src->lte_eNB_common_vars.txdataF[0][0][0], 0, 
          FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));

      // Fill results
      results->mcs_hop1[pdu][round_hop1] = context->mcs_hop1;
      results->tbs_hop1[pdu][round_hop1] = context->tbs_hop1;
      results->n_prb_hop1[pdu][round_hop1] = args->n_prb_hop1;

      // Create first hop DCI
      setup_broadcast_dci(&dci_hop1, context->rnti_hop1, round_hop1, context->mcs_hop1, args->n_prb_hop1);
      if(args->verbose > 1)
        dump_dci(frame_parms, &dci_hop1);

      // Generate eNB transport channel parameters
      generate_eNB_dlsch_params_from_dci(context->subframe_hop1, dci_hop1.dci_pdu, 
          context->rnti_hop1, format1, phy_vars_ch_src->dlsch_eNB[0], frame_parms, 
          SI_RNTI, RA_RNTI, P_RNTI,
          phy_vars_ch_src->eNB_UE_stats[0].DL_pmi_single);

      // Create PDCCH
      n_used_pdcch_symbols = generate_dci_top(1, 0, &dci_hop1, 0, 1024, frame_parms,
          phy_vars_ch_src->lte_eNB_common_vars.txdataF[0], context->subframe_hop1);

      if(n_used_pdcch_symbols > context->n_avail_pdcch_symbols) {
        printf("Need %d PDCCH symbols\n", n_used_pdcch_symbols);
        exit(1);
      }
      
      // Encode source data
      if(dlsch_encoding(context->input_buffer, frame_parms, context->n_avail_pdcch_symbols,
            phy_vars_ch_src->dlsch_eNB[0][0], context->subframe_hop1) < 0)
        exit(-1);

      // Scramble data
      dlsch_scrambling(frame_parms, context->n_avail_pdcch_symbols,
          phy_vars_ch_src->dlsch_eNB[0][0], context->n_coded_bits_hop1, 0, context->subframe_hop1 << 1);

      // Modulate data
      n_re_hop1 = dlsch_modulation(phy_vars_ch_src->lte_eNB_common_vars.txdataF[0],
          1024, context->subframe_hop1, frame_parms, context->n_avail_pdcch_symbols, 
          phy_vars_ch_src->dlsch_eNB[0][0]);
      if(args->verbose > 0)
        printf("Hop 1, HARQ round %d: %d coded bits, Modulated %d REs\n", round_hop1, context->n_coded_bits_hop1, n_re_hop1);

      if(args->verbose > 2)
        print_dlsch_eNB_stats(phy_vars_ch_src->dlsch_eNB[0][0]);

      // Generate pilots
      generate_pilots(phy_vars_ch_src, phy_vars_ch_src->lte_eNB_common_vars.txdataF[0],
          1024, LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);

      // OFDM modulation
      ofdm_modulation(phy_vars_ch_src->lte_eNB_common_vars.txdataF[0],
          phy_vars_ch_src->lte_eNB_common_vars.txdata[0],
          frame_parms, context->subframe_hop1, frame_parms->symbols_per_tti/2*3);

      // Compute transmitter signal energy ( E{abs(X)^2} )
      tx_energy = signal_energy(&phy_vars_ch_src->lte_eNB_common_vars.txdata[0][0]
          [context->subframe_hop1*frame_parms->samples_per_tti], frame_parms->samples_per_tti);

      // Transmit over channel
      for(k = 0; k < args->n_relays; k++) {
        //awgn_stddev = sqrt((double)tx_energy*((double)frame_parms->ofdm_symbol_size/(args->n_prb_hop1*12))/pow(10.0, ((double)context->snr_hop1[k])/10.0)/2.0);
        awgn_stddev = sqrt((double)tx_energy)/pow(10.0, ((double)context->snr_hop1[k])/20.0);
        tx_ampl = awgn_stddev/sqrt((double)tx_energy)*pow(10.0, ((double)context->snr_hop1[k])/20.0);
        //printf("hop 1: E=%d, ampl=%f, awgn=%f\n", tx_energy, tx_ampl, awgn_stddev);

        transmit_subframe(context->channels_hop1[k],
            phy_vars_ch_src->lte_eNB_common_vars.txdata[0],
            frame_parms, context->subframe_hop1, frame_parms->symbols_per_tti+1, tx_ampl, false);
        deliver_subframe(context->channels_hop1[k],
            phy_vars_mr[k]->lte_ue_common_vars.rxdata,
            frame_parms, context->subframe_hop1, frame_parms->symbols_per_tti+1, awgn_stddev);
      }
      results->n_frames_hop1++;
      if(!hop2_active)
        results->n_harq_tries_hop1[round_hop1]++;

      // Decode at all relays that have not yet decoded
      for(k = 0; k < args->n_relays; k++) {
        if(decoded_at_mr[k])
          continue;

        // Front end processor up to first pilot
        for(l = 0; l <= pilot1_symbol; l++)
          slot_fep(phy_vars_mr[k], l, context->subframe_hop1<<1, 0, 0);

        // Skip decoding of DCI
        phy_vars_mr[k]->lte_ue_pdcch_vars[0]->crnti = context->rnti_hop1;
        phy_vars_mr[k]->lte_ue_pdcch_vars[0]->num_pdcch_symbols = context->n_avail_pdcch_symbols;
        generate_ue_dlsch_params_from_dci(context->subframe_hop1, dci_hop1.dci_pdu, context->rnti_hop1, 
            format1, phy_vars_mr[k]->dlsch_ue[0], frame_parms, SI_RNTI, RA_RNTI, P_RNTI);

        // Receive DLSCH data
        // Front end processor up to second pilot
        for(l = pilot1_symbol+1; l < n_symbols_per_slot; l++)
          slot_fep(phy_vars_mr[k], l, context->subframe_hop1<<1, 0, 0);
        slot_fep(phy_vars_mr[k], 0, (context->subframe_hop1<<1)+1, 0, 0);
        // Receive DLSCH for first slot
        if(rx_dlsch_symbol(phy_vars_mr[k], context->subframe_hop1, context->n_avail_pdcch_symbols, 1) == -1)
          break;
        for(l = context->n_avail_pdcch_symbols + 1; l < n_symbols_per_slot; l++)
          if(rx_dlsch_symbol(phy_vars_mr[k], context->subframe_hop1, l, 0) == -1)
            break;
        // Front end processor up to third pilot
        for(l = 1; l <= pilot1_symbol; l++)
          slot_fep(phy_vars_mr[k], l, (context->subframe_hop1<<1)+1, 0, 0);
        // Receive DLSCH up to third pilot
        for(l = n_symbols_per_slot; l < n_symbols_per_slot+pilot1_symbol; l++)
          if(rx_dlsch_symbol(phy_vars_mr[k], context->subframe_hop1, l, 0) == -1)
            break;
        // Front end processor for rest of subframe
        for(l = pilot1_symbol+1; l < n_symbols_per_slot; l++)
          slot_fep(phy_vars_mr[k], l, (context->subframe_hop1<<1)+1, 0, 0);
        slot_fep(phy_vars_mr[k], 0, (context->subframe_hop1<<1)+2, 0, 0);
        // Receive DLSCH for rest of subframe
        for(l = n_symbols_per_slot+pilot1_symbol; l < 2*n_symbols_per_slot; l++)
          if(rx_dlsch_symbol(phy_vars_mr[k], context->subframe_hop1, l, 0) == -1)
            break;

        // Compute raw bit error rate
        raw_ber = compute_ber_soft(phy_vars_ch_src->dlsch_eNB[0][0]->e,
            phy_vars_mr[k]->lte_ue_pdsch_vars[0]->llr[0], context->n_coded_bits_hop1);
        results->ber_hop1[k] += raw_ber;
        context->n_ber_frames_hop1[k]++;
        if(args->verbose > 0)
          printf("Received %d bits at MR %d, raw BER: %f\n", context->n_coded_bits_hop1, k, raw_ber);

        // Unscramble received bits
        dlsch_unscrambling(frame_parms, phy_vars_mr[k]->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
            phy_vars_mr[k]->dlsch_ue[0][0], context->n_coded_bits_hop1, phy_vars_mr[k]->lte_ue_pdsch_vars[0]->llr[0],
            0, context->subframe_hop1 << 1);

        // Decode received bits
        n_iter = dlsch_decoding(phy_vars_mr[k]->lte_ue_pdsch_vars[0]->llr[0],
            frame_parms, phy_vars_mr[k]->dlsch_ue[0][0], context->subframe_hop1, 
            phy_vars_mr[k]->lte_ue_pdcch_vars[0]->num_pdcch_symbols);

        if(args->verbose > 2)
          print_dlsch_ue_stats(phy_vars_mr[k]->dlsch_ue[0][0]);

        if(n_iter <= MAX_TURBO_ITERATIONS) {
          if(args->verbose > 0)
            printf("Successfully decoded at MR %d\n", k);
          activate_mr[k] = true;

          // copy received data to intermediate buffer
          memcpy(context->mr_buffer[k], phy_vars_mr[k]->dlsch_ue[0][0]->harq_processes[0]->b, context->tbs_col>>3);
          //memset(&context->mr_buffer[k][context->tbs_col>>3], 0, context->mr_buffer_length+4-(context->tbs_col>>3));
        }
      }

      // Write debug signals if required
      if(args->debug_output) {
        if(round_hop1 == 0)
          write_output("hop1_e.m", "e", phy_vars_ch_src->dlsch_eNB[0][0]->e, context->n_coded_bits_hop1, 1, 4);
        snprintf(fnbuf, 80, "hop1_r%d_ch_txdataFv.m", round_hop1);
        snprintf(varbuf, 80, "hop1_r%d_ch_txdataF", round_hop1);
        write_output(fnbuf, varbuf, phy_vars_ch_src->lte_eNB_common_vars.txdataF[0][0],
            FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX, 1, 1);
        snprintf(fnbuf, 80, "hop1_r%d_ch_txdatav.m", round_hop1);
        snprintf(varbuf, 80, "hop1_r%d_ch_txdata", round_hop1);
        write_output(fnbuf, varbuf, phy_vars_ch_src->lte_eNB_common_vars.txdata[0][0], 
            10*frame_parms->samples_per_tti, 1, 1);
        for(k = 0; k < args->n_relays; k++) {
          snprintf(fnbuf, 80, "hop1_r%d_mr%d_rxdatav.m", round_hop1, k);
          snprintf(varbuf, 80, "hop1_r%d_mr%d_rxdata", round_hop1, k);
          write_output(fnbuf, varbuf, phy_vars_mr[k]->lte_ue_common_vars.rxdata[0], 
              10*frame_parms->samples_per_tti, 1, 1);
          snprintf(fnbuf, 80, "hop1_r%d_mr%d_rxdataFv.m", round_hop1, k);
          snprintf(varbuf, 80, "hop1_r%d_mr%d_rxdataF", round_hop1, k);
          write_output(fnbuf, varbuf, phy_vars_mr[k]->lte_ue_common_vars.rxdataF[0],
              2*frame_parms->ofdm_symbol_size*2*n_symbols_per_slot, 2, 1);
        }
      }
    }

    if(hop2_active) {
      // Fill results
      results->mcs_hop2[pdu][round_hop2] = context->mcs_hop2;
      results->tbs_hop2[pdu][round_hop2] = context->tbs_hop2;
      results->n_prb_hop2[pdu][round_hop2] = args->n_prb_hop2;
      l = 0;
      for(k = args->n_relays-1; k >= 0; k--)
        if(decoded_at_mr[k])
          l = (l << 1) + 1;
        else
          l = (l << 1);
      results->relay_activity[l]++;

      // create second hop dci
      setup_distributed_dci(&dci_hop2, context->rnti_hop2, round_hop2, context->mcs_hop2, args->n_prb_hop2);
      if(args->verbose > 1)
        dump_dci(frame_parms, &dci_hop2);

      if(args->verbose > 0)
        printf("Hop 2, HARQ round %d\n", round_hop2);

      // Clear eNB receive vector
      memset(phy_vars_ch_dest->lte_eNB_common_vars.rxdata[0][0], 0, FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(int));

      // Determine how many relays are active for this transmission, split the total power between them
      n_active_relays = 0;
      for(k = 0; k < args->n_relays; k++)
        if(decoded_at_mr[k])
          n_active_relays++;

      // Normalization of received signal, fix this..
      tx_energy = 300.0e3;
      //      awgn_stddev = sqrt((double)tx_energy)/pow(10.0, ((double)context->snr_hop2[0])/20.0);
      awgn_stddev = pow(10,.05*40);

      //      for(k = 1; k < args->n_relays; k++)
      //        awgn_stddev = min(sqrt((double)tx_energy)/pow(10.0, ((double)context->snr_hop2[k])/20.0), awgn_stddev);
      //      printf("hop2 awgn_stddev %f\n",10*log10(awgn_stddev));
      // transmit from all active relays
      accumulate_at_rx = false;

      for(k = 0; k < args->n_relays; k++) {
        if(!decoded_at_mr[k])
          continue;

        // Clear txdataF vector
        memset(phy_vars_mr[k]->lte_ue_common_vars.txdataF[0], 0, 
            FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));

        // Generate transport channel parameters
        generate_ue_ulsch_params_from_dci(dci_hop2.dci_pdu, context->rnti_hop2, 
					  ul_subframe2pdcch_alloc_subframe(&phy_vars_mr[k]->lte_frame_parms,context->subframe_hop2),//(context->subframe_hop2+6)%10, 
					  format0, phy_vars_mr[k], SI_RNTI, RA_RNTI, P_RNTI, 0, 0);

        // Set relay role in Alamouti coding (this could be done better)
        if(relay_role[k] == RELAY_ROLE_STANDARD) {
          phy_vars_mr[k]->ulsch_ue[0]->cooperation_flag = 0;
        }
        else {
          phy_vars_mr[k]->ulsch_ue[0]->cooperation_flag = 2;
        }

        // Generate uplink reference signal
        generate_drs_pusch(phy_vars_mr[k], 0, AMP, context->subframe_hop2, 0, args->n_prb_hop2);
  
        // Encode ULSCH data
	//	printf("transmit_one_pdu 1 : ulsch_encoding mr %d\n",k);
        if(ulsch_encoding(context->mr_buffer[k], frame_parms, phy_vars_mr[k]->ulsch_ue[0],
              context->harq_pid_hop2, 1, 0, 1) == -1) {
          printf("ulsch_encoding failed\n");
          exit(1);
        }

        // Modulate ULSCH data
        ulsch_modulation(phy_vars_mr[k]->lte_ue_common_vars.txdataF, AMP, 0, context->subframe_hop2, 
            frame_parms, phy_vars_mr[k]->ulsch_ue[0]);

        // Compute number of resource elements from coded bits and modulation order
        n_re_hop2 = context->n_coded_bits_hop2/get_Qm(context->mcs_hop2);

        if(args->verbose > 2)
          print_ulsch_ue_stats(phy_vars_mr[k]->ulsch_ue[0]);

        // OFDM modulation
        ofdm_modulation(phy_vars_mr[k]->lte_ue_common_vars.txdataF,
            phy_vars_mr[k]->lte_ue_common_vars.txdata, frame_parms, context->subframe_hop2, frame_parms->symbols_per_tti);

        tx_energy = signal_energy(&phy_vars_mr[k]->lte_ue_common_vars.txdata[0]
            [frame_parms->samples_per_tti*context->subframe_hop2], frame_parms->samples_per_tti);

        // Transmit over channel
        // Redo this in a more intuitive manner:
        //awgn_stddev = sqrt((double)tx_energy*((double)frame_parms->ofdm_symbol_size/(args->n_prb_hop2*12))/pow(10.0, ((double)context->snr_hop2[k])/10.0)/2.0);
        tx_ampl = awgn_stddev/sqrt((double)tx_energy)*pow(10.0, ((double)context->snr_hop2[k])/20.0)/sqrt((double)n_active_relays);
	//	printf("hop 2 (%d): E=%d, ampl=%f, awgn=%f (accum %d)\n", k,tx_energy, tx_ampl, awgn_stddev,(unsigned char)accumulate_at_rx);
        transmit_subframe(context->channels_hop2[k],
			  phy_vars_mr[k]->lte_ue_common_vars.txdata, frame_parms, 
            //context->subframe_hop2, frame_parms->symbols_per_tti, 256.0/sqrt((double)n_active_relays)/awgn_stddev, accumulate_at_rx);
            context->subframe_hop2, frame_parms->symbols_per_tti, tx_ampl, accumulate_at_rx);
        accumulate_at_rx = true;
      }

      // This is ugly. Fix it.
      deliver_subframe(context->channels_hop2[0],
          phy_vars_ch_dest->lte_eNB_common_vars.rxdata[0], frame_parms,
          context->subframe_hop2, frame_parms->symbols_per_tti, awgn_stddev);

      results->n_harq_tries_hop2[round_hop2]++;

      // Fill the last symbol of the frame with random data (used for SNR estimation?)
      for (i=0;i<OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++) {
        ((short*) &phy_vars_ch_dest->lte_eNB_common_vars.rxdata[0][0]
         [(frame_parms->samples_per_tti<<1) -frame_parms->ofdm_symbol_size])[2*i] = 
          (short) ((awgn_stddev*0.707*gaussdouble(0.0,1.0)));
        ((short*) &phy_vars_ch_dest->lte_eNB_common_vars.rxdata[0][0]
         [(frame_parms->samples_per_tti<<1) -frame_parms->ofdm_symbol_size])[2*i+1] = 
          (short) ((awgn_stddev*0.707*gaussdouble(0.0,1.0)));
      }

      // Generate eNB transport channel parameters
      generate_eNB_ulsch_params_from_dci(dci_hop2.dci_pdu, context->rnti_hop2, 
					 ul_subframe2pdcch_alloc_subframe(&phy_vars_ch_dest->lte_frame_parms,context->subframe_hop2),//(context->subframe_hop2+6)%10, 
					 format0, 0, phy_vars_ch_dest, SI_RNTI, RA_RNTI, P_RNTI, 0);

      // Front end processing at destination CH
      for(l = 0; l < frame_parms->symbols_per_tti>>1; l++)
        slot_fep_ul(frame_parms, &phy_vars_ch_dest->lte_eNB_common_vars, l, 2*context->subframe_hop2, 0, 0);
      for(l = 0; l < frame_parms->symbols_per_tti>>1; l++)
        slot_fep_ul(frame_parms, &phy_vars_ch_dest->lte_eNB_common_vars, l, 2*context->subframe_hop2+1, 0, 0);

      // Receive ULSCH data
      rx_ulsch(phy_vars_ch_dest, context->subframe_hop2, 0, 0, phy_vars_ch_dest->ulsch_eNB, 2);

      // Compute uncoded bit error rate
      k = 0;
      while(!decoded_at_mr[k])
        k++;
      raw_ber = compute_ber_soft(phy_vars_mr[k]->ulsch_ue[0]->b_tilde,
          phy_vars_ch_dest->lte_eNB_pusch_vars[0]->llr, context->n_coded_bits_hop2);
      results->ber_hop2 += raw_ber;
      context->n_ber_frames_hop2++;
      results->n_frames_hop2++;
      if(args->verbose > 0) {
        printf("Received %d bits at dest CH, raw BER: %f\n", context->n_coded_bits_hop2, raw_ber);
      }

      // Decode ULSCH data
      n_iter = ulsch_decoding(phy_vars_ch_dest, 0, context->subframe_hop2, 0, 1);

      if(args->verbose > 2)
        print_ulsch_eNB_stats(phy_vars_ch_dest->ulsch_eNB[0]);

      if(n_iter <= MAX_TURBO_ITERATIONS) {
        if(args->verbose > 0)
          printf("Successfully decoded at dest CH\n");
        decoded_at_ch = true;
      }

      // Write debug output if requested
      if(args->debug_output) {
        for(k = 0; k < args->n_relays; k++) {
          snprintf(fnbuf, 80, "hop2_r%d_mr%d_txdataFv.m", round_hop2, k);
          snprintf(varbuf, 80, "hop2_r%d_mr%d_txdataF", round_hop2, k); 
          write_output(fnbuf, varbuf, phy_vars_mr[k]->lte_ue_common_vars.txdataF[0],
              FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX, 1, 1);
          snprintf(fnbuf, 80, "hop2_r%d_mr%d_txdatav.m", round_hop2, k);
          snprintf(varbuf, 80, "hop2_r%d_mr%d_txdata", round_hop2, k);
          write_output(fnbuf, varbuf, phy_vars_mr[k]->lte_ue_common_vars.txdata[0], 
              10*frame_parms->samples_per_tti, 1, 1);
        }
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdatav.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdata", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_common_vars.rxdata[0][0], 
            10*frame_parms->samples_per_tti, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdataFv.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdataF", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_common_vars.rxdataF[0][0],
            20*frame_parms->ofdm_symbol_size*2*n_symbols_per_slot, 2, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdataF_ext2v.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdataF_ext2", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->rxdataF_ext2[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdataF_comp.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdataF_comp", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->rxdataF_comp[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdataF_comp_0.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdataF_comp_0", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->rxdataF_comp_0[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_rxdataF_comp_1.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_rxdataF_comp_1", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->rxdataF_comp_1[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_drs_ch_estimates_0.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_drs_ch_estimates_0", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->drs_ch_estimates_0[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
        snprintf(fnbuf, 80, "hop2_r%d_ch_drs_ch_estimates_1.m", round_hop2);
        snprintf(varbuf, 80, "hop2_r%d_ch_drs_ch_estimates_1", round_hop2);
        write_output(fnbuf, varbuf, phy_vars_ch_dest->lte_eNB_pusch_vars[0]->drs_ch_estimates_1[0][0],
            12*phy_vars_ch_dest->lte_frame_parms.N_RB_UL*n_symbols_per_slot*2, 1, 1);
      }
    }

    // Activate MRs that decoded during this frame
    for(k = 0; k < args->n_relays; k++)
      if(activate_mr[k]) {
        setup_distributed_dci(&dci_hop2, context->rnti_hop2, 0, context->mcs_hop2, args->n_prb_hop2);

        // Generate transport channel parameters
        generate_ue_ulsch_params_from_dci(dci_hop2.dci_pdu, context->rnti_hop2, 
					  ul_subframe2pdcch_alloc_subframe(&phy_vars_mr[k]->lte_frame_parms,context->subframe_hop2),//(context->subframe_hop2+6)%10, 
					  format0, phy_vars_mr[k], SI_RNTI, RA_RNTI, P_RNTI, 0, 0);

        // Set relay role in Alamouti coding (this could be done better)
        if(relay_role[k] == RELAY_ROLE_STANDARD) {
          phy_vars_mr[k]->ulsch_ue[0]->cooperation_flag = 0;
        }
        else {
          phy_vars_mr[k]->ulsch_ue[0]->cooperation_flag = 2;
        }

        // Encode ULSCH data
	//	printf("transmit one pdu 2 : ulsch_encoding mr %d\n",k);
        if(ulsch_encoding(context->mr_buffer[k], frame_parms, phy_vars_mr[k]->ulsch_ue[0],
              context->harq_pid_hop2, 1, 0, 1) == -1) {
          printf("ulsch_encoding failed\n");
          exit(1);
        }
        decoded_at_mr[k] = true;
      }

    // Do strategy logic
    start_hop2 = false;
    switch(args->strategy) {
      case strategy_wait_all:
        if(hop1_active && !hop2_active) {
          // Start hop 2 if all relays have decoded
          start_hop2 = true;
          for(k = 0; k < args->n_relays; k++)
            if(!decoded_at_mr[k])
              start_hop2 = false;
        }
        break;
      case strategy_wait_one:
        if(hop1_active && !hop2_active) {
          // Start hop 2 if at least one relay has decoded
          for(k = 0; k < args->n_relays; k++)
            if(decoded_at_mr[k])
              start_hop2 = true;
        }
        break;
      default:
        exit(1);
    }

    // If we start hop 2 now, save statistics for hop 1
    if(start_hop2) {
      results->n_harq_success_hop1[round_hop1]++;
      results->n_pdu_success_hop1++;
      results->n_bits_hop1 += context->tbs_hop1;
      rounds_hop1 = round_hop1;
    }

    if(hop1_active) {
      // Check if all relays decoded the PDU
      decoded_at_all_mr = true;
      for(k = 0; k < args->n_relays; k++)
        if(!decoded_at_mr[k])
          decoded_at_all_mr = false;

      // Disable hop 1 if all relays decoded or the maximum HARQ round was reached
      round_hop1++;
      if(decoded_at_all_mr || round_hop1 == args->n_harq)
        hop1_active = false;
    }

    if(hop2_active) {
      // If successfully decoded at CH2, save statistics
      if(decoded_at_ch) {
        results->n_harq_success_hop2[round_hop2]++;
        results->n_pdu_success_hop2++;
        results->n_bits_hop2 += context->tbs_hop2;
        rounds_hop2 = round_hop2;
        if(rounds_hop1 < 0 || rounds_hop1 >= MAX_HARQ_ROUNDS)
          fprintf(stderr, "rounds_hop1 has invalid value %d\n", rounds_hop1);
        else if(rounds_hop2 < 0 || rounds_hop2 >= MAX_HARQ_ROUNDS)
          fprintf(stderr, "rounds_hop2 has invalid value %d\n", rounds_hop2);
        else
          results->n_transmissions[rounds_hop1][rounds_hop2]++;

        if(!block_valid(context->input_buffer, phy_vars_ch_dest->ulsch_eNB[0]->harq_processes[context->harq_pid_hop2]->b,
              context->tbs_col/8)) {
          printf("MAC PDU %d decoded successfully, but contained errors\n", pdu);
        }
      }

      // If successfully decoded at CH2 or the maximum HARQ round was reached, disable both hops
      round_hop2++;
      if(decoded_at_ch || round_hop2 == args->n_harq) {
        hop1_active = false;
        hop2_active = false;
      }
    }

    if(start_hop2)
      hop2_active = true;

    frame++;

    /*
    if(!decoded_at_all_mr) {
      if(args.verbose > 0)
        printf("Not decoded at all relays, dropping block\n");
      continue;
    }
    */
  }
}

int parse_args(int argc, char** argv, args_t* args)
{
  int c;
  int k;
  bool snr_set;
  const struct option long_options[] = {
    {"mcs1", required_argument, NULL, 256},
    {"mcs2", required_argument, NULL, 257},
    {"snr", required_argument, NULL, 258},
    {"snr1", required_argument, NULL, 259},
    {"snr2", required_argument, NULL, 260},
    {"single", no_argument, NULL, 261},
    {"sweep", no_argument, NULL, 262},
    {"bsweep", no_argument, NULL, 263},
    {"csweep", no_argument, NULL, 271},
    {"strategy", required_argument, NULL, 264},
    {"rb1", required_argument, NULL, 265},
    {"rb2", required_argument, NULL, 266},
    {"autorb", no_argument, NULL, 267},
    {"range", required_argument, NULL, 268},
    {"step", required_argument, NULL, 269},
    {"corr", required_argument, NULL, 270},
    {NULL, 0, NULL, 0}};

  args->n_relays = 2;
  args->debug_output = false;
  args->verbose = 0;
  args->n_pdu = 1;
  args->n_harq = 4;
  args->mcs_hop1 = 0;
  args->mcs_hop2 = 0;
  args->n_prb_hop1 = N_PRB;
  args->n_prb_hop2 = N_PRB;
  args->autorb = false;
  args->channel_model = AWGN;
  args->channel_correlation = 0.0;
  args->results_fn = 0;
  args->analysis = analysis_single;
  args->strategy = strategy_wait_all;
  args->range = 10.0;
  args->step = 1.0;

  for(k = 0; k < args->n_relays; k++)
    args->snr_hop1[k] = 10.0;
  for(k = 0; k < args->n_relays; k++)
    args->snr_hop2[k] = 10.0;

  snr_set = false;
  while((c = getopt_long(argc, argv, "hovN:n:m:r:H:C:", long_options, NULL)) != -1) {
    switch(c) {
    case 'h':
      return 1;
    case 'o':
      args->debug_output = true;
      break;
    case 'v':
      args->verbose++;
      break;
    case 'N':
      args->n_relays = atoi(optarg);
      if(args->n_relays <= 0)
        return 1;
      break;
    case 'n':
      args->n_pdu = atoi(optarg);
      if(args->n_pdu <= 0)
        return 1;
      break;
    case 'H':
      args->n_harq = atoi(optarg);
      break;
    case 'C':
      if(strcmp(optarg, "help") == 0)
        return 2;
      if(!parse_channel_model(optarg, &args->channel_model))
        return 1;
      break;
    case 'm':
      args->mcs_hop1 = args->mcs_hop2 = atoi(optarg);
      break;
    case 'r':
      args->results_fn = optarg;
      break;
    case 256:
      args->mcs_hop1 = atoi(optarg);
      break;
    case 257:
      args->mcs_hop2 = atoi(optarg);
      break;
    case 258:
      for(k = 0; k < args->n_relays; k++)
        args->snr_hop1[k] = atof(optarg);
      for(k = 0; k < args->n_relays; k++)
        args->snr_hop2[k] = atof(optarg);
      snr_set = true;
      break;
    case 259:
      if(!parse_snr(optarg, args->snr_hop1, args->n_relays))
        return 1;
      snr_set = true;
      break;
    case 260:
      if(!parse_snr(optarg, args->snr_hop2, args->n_relays))
        return 1;
      snr_set = true;
      break;
    case 261:
      args->analysis = analysis_single;
      break;
    case 262:
      args->analysis = analysis_snrsweep_a;
      break;
    case 263:
      args->analysis = analysis_snrsweep_b;
      break;
    case 271:
      args->analysis = analysis_snrsweep_c;
      break;
    case 264:
      switch(atoi(optarg)) {
        case 1:
          args->strategy = strategy_wait_all;
          break;
        case 2:
          args->strategy = strategy_wait_one;
          break;
        default:
          return 1;
      }
      break;
    case 265: // --rb1
      args->n_prb_hop1 = atoi(optarg);
      if(args->n_prb_hop1 <= 0 || args->n_prb_hop1 > N_PRB)
        return 1;
      break;
    case 266: // --rb2
      args->n_prb_hop2 = atoi(optarg);
      if(args->n_prb_hop2 <= 0 || args->n_prb_hop2 > N_PRB)
        return 1;
      break;
    case 267: // --autorb
      args->autorb = true;
      break;
    case 268: // --range
      args->range = atof(optarg);
      if(args->range <= 0.0)
        return 1;
      break;
    case 269: // --step
      args->step = atof(optarg);
      if(args->step <= 0.0)
        return 1;
      break;
    case 270: // --corr
      args->channel_correlation = atof(optarg);
      if(args->channel_correlation < 0.0 || args->channel_correlation > 1.0)
        return 1;
      break;
    default:
      return 1;
    }
  }

  return 0;
}

bool parse_snr(const char* str, double* snr, int n)
{
  char* p;
  int k;

  for(k = 0; k < n; k++) {
    snr[k] = strtod(str, &p);
    if(p == str)
      break;
    str = p;
  }

  if(k == 0)
    for(k = 1; k < n; k++)
      snr[k] = snr[0];
  else if(k < n-1)
    return false;
  return true;
}

int parse_channel_model(const char* str, SCM_t* model)
{
  if(strcmp(str, "0") == 0) *model = AWGN;
  else if(strcmp(str, "A") == 0) *model = SCM_A;
  else if(strcmp(str, "B") == 0) *model = SCM_B;
  else if(strcmp(str, "C") == 0) *model = SCM_C;
  else if(strcmp(str, "D") == 0) *model = SCM_D;
  else if(strcmp(str, "E") == 0) *model = EPA;
  else if(strcmp(str, "F") == 0) *model = EVA;
  else if(strcmp(str, "G") == 0) *model = ETU;
  else if(strcmp(str, "H") == 0) *model = Rayleigh8;
  else if(strcmp(str, "I") == 0) *model = Rayleigh1;
  else if(strcmp(str, "J") == 0) *model = Rayleigh1_corr;
  else if(strcmp(str, "K") == 0) *model = Rayleigh1_anticorr;
  else if(strcmp(str, "L") == 0) *model = Rice8;
  else if(strcmp(str, "M") == 0) *model = Rice1;
  else return false;
  return true;
}

void print_usage(const char* prog)
{
  printf("Usage: %s [options]\n", prog);
  printf("\n");
  printf("    General options:\n");
  printf("  -h         : print usage\n");
  printf("  -v         : increase verbosity level [0]\n");
  printf("  -o         : output MATLAB signal files (implies -n 1) [no]\n");
  printf("  -r FILE    : write results to FILE\n");
  printf("  -N NRELAYS : simulate using NRELAYS relays [2]\n");
  printf("  -n NUM     : simulate NUM MAC PDUs [1]\n");
  printf("  -H NUM     : do NUM HARQ rounds in each hop [4]\n");
  printf("     note: the hop 1 RVs are 0,0,1,1,2,2,3,3,0,0,..., the hop 2 RVs are 0,2,3,1,...\n");
  printf("  -C CHANNEL : set the channel model, use -C help for available models [AWGN]\n");
  printf("  --corr CORR : set channel realization correlation (0.0 .. 1.0) [0.0]\n");
  printf("  --strategy X : set the HARQ strategy to X [1]\n");
  printf("     1: decode at all relays before starting hop 2\n");
  printf("     2: start hop 2 when at least one relay has decoded\n");
  printf("\n");
  printf("    SNR options:\n");
  printf("  --snr SNR  : set snr for all links to SNR [10.0]\n");
  printf("  --snr1 SNR : set snr for hop 1 to SNR\n");
  printf("  --snr2 SNR : set snr for hop 2 to SNR\n");
  printf("      for --snr1 and --snr2, SNR may be either a single value or a vector with n_relays elements\n");
  printf("      e.g.: --snr1 \"4.0 6.0\" sets the SNR from CH1 to MR1 and MR2 to 4.0 and 6.0, respectively\n");
  printf("\n");
  printf("    Analysis setup (only one may be specified):\n");
  printf("  --single : single point analysis [default]\n");
  printf("  --sweep  : sweep snr of first relay of both hops [-RANGE*STEP..+RANGE*STEP]\n");
  printf("  --bsweep : sweep first relay of hop 1 [-RANGE*STEP..+RANGE*STEP],\n");
  printf("             sweep first relay of hop 2 [+RANGE*STEP..-RANGE*STEP]\n");
  printf("  --range RANGE : set sweep range [10]\n");
  printf("  --step STEP   : set sweep step size [1.0]\n");
  printf("      note: the swept range is relative to SNR specified with --snr* options\n");
  printf("\n");
  printf("    Link and resource parameters:\n");
  printf("  -m MCS     : set mcs for both hops to MCS [0]\n");
  printf("  --mcs1 MCS : set mcs for hop 1 to MCS\n");
  printf("  --mcs2 MCS : set mcs for hop 2 to MCS\n");
  printf("  --rb1 NUM  : set number of resource blocks for hop 1 [%d]\n", N_PRB);
  printf("  --rb2 NUM  : set number of resource blocks for hop 2 [%d]\n", N_PRB);
  //printf("  --autorb   : adjust the hop bandwidths to have similar TBS\n");
  printf("\n");
  printf("    Definition of results:\n");
  printf("  BLER is the fraction of MAC PDUs that are not successfully delivered from CH1 to CH2\n");
  printf("  BER is the fraction of coded bits that are incorrectly received for each link\n");
  printf("  avg bits/frame is TBS*n_pdu/n_frames, where n_pdu is the number of successfully received MAC PDUs\n");
  printf("    (for hop 1: decoded by both MR), and n_frames is the number of LTE frames transmitted for the hop\n");
  printf("  norm. delay is the average number of LTE frames required for the successful transmission of a MAC PDU\n");
}

void print_channel_usage()
{
  printf("Available channel models:\n");
  printf("  0: AWGN\n");
  printf("  A: SCM-A\n");
  printf("  B: SCM-B\n");
  printf("  C: SCM-C\n");
  printf("  D: SCM-D\n");
  printf("  E: EPA\n");
  printf("  F: EVA\n");
  printf("  G: ETU\n");
  printf("  H: Rayleigh8\n");
  printf("  I: Rayleigh1\n");
  printf("  J: Rayleigh1_corr\n");
  printf("  K: Rayleigh1_anticorr\n");
  printf("  L: Rice8\n");
  printf("  M: Rice1\n");
}

void signal_handler(int sig) 
{
  void *array[10];
  size_t size;

  size = backtrace(array, 10);
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

void setup_single(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays)
{
  int k;

  *snrs = malloc(2*n_relays*sizeof(double));

  for(k = 0; k < n_relays; k++)
    (*snrs)[k] = snr_hop1[k];
  for(k = 0; k < n_relays; k++)
    (*snrs)[k+n_relays] = snr_hop2[k];

  *n_tests = 1;
}

void setup_snrsweep_a(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end)
{
  int l;
  int k;
  *n_tests = end-start+1;

  *snrs = malloc((*n_tests)*2*n_relays*sizeof(double));
  for(l = 0; l < *n_tests; l++) {
    (*snrs)[2*n_relays*l] = snr_hop1[0] + step*(start+l);
    (*snrs)[2*n_relays*l + n_relays] = snr_hop2[0] + step*(start+l);
    for(k = 1; k < n_relays; k++) {
      (*snrs)[2*n_relays*l + k] = snr_hop1[k];
      (*snrs)[2*n_relays*l + n_relays + k] = snr_hop2[k];
    }
  }
}

void setup_snrsweep_b(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end)
{
  int l;
  int k;
  *n_tests = end-start+1;

  *snrs = malloc((*n_tests)*2*n_relays*sizeof(double));
  for(l = 0; l < *n_tests; l++) {
    (*snrs)[2*n_relays*l] = snr_hop1[0] + step*(start+l);
    (*snrs)[2*n_relays*l + n_relays] = snr_hop2[0] + step*(end-l);
    for(k = 1; k < n_relays; k++) {
      (*snrs)[2*n_relays*l + k] = snr_hop1[k];
      (*snrs)[2*n_relays*l + n_relays + k] = snr_hop2[k];
    }
  }
}

void setup_snrsweep_c(double** snrs, int* n_tests, double* snr_hop1, double* snr_hop2, int n_relays, double step, int start, int end)
{
  int l;
  int k;
  *n_tests = end-start+1;

  *snrs = malloc((*n_tests)*2*n_relays*sizeof(double));
  for(l = 0; l < *n_tests; l++) {
    for(k = 0; k < n_relays; k++) {
      (*snrs)[2*n_relays*l + k] = snr_hop1[k] + step*(start+l);
      (*snrs)[2*n_relays*l + n_relays + k] = snr_hop2[k] + step*(start+l);
    }
  }
}

void setup_frame_params(LTE_DL_FRAME_PARMS* frame_parms, unsigned char transmission_mode)
{
  frame_parms->N_RB_DL = N_PRB;
  frame_parms->N_RB_UL = N_PRB;
  frame_parms->Nid_cell = NID_CELL;
  frame_parms->Ncp = cp_type;
  frame_parms->Ncp_UL = cp_type;
  frame_parms->nushift = 0;
  frame_parms->frame_type = 1; // TDD frames
  frame_parms->tdd_config = 1; // TDD frame type 1
  frame_parms->mode1_flag = (transmission_mode == 1 ? 1 : 0);
  frame_parms->nb_antennas_tx = n_txantenna_ch;
  frame_parms->nb_antennas_rx = n_rxantenna_mr;

  init_frame_parms(frame_parms, oversampling);
  phy_init_top(frame_parms);
  frame_parms->twiddle_fft = twiddle_fft;
  frame_parms->twiddle_ifft = twiddle_ifft;
  frame_parms->rev = rev;

  phy_init_lte_top(frame_parms);

  frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.groupHoppingEnabled = 1;
  frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled = 0;
  frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;
  init_ul_hopping(frame_parms);
  //dump_frame_parms(frame_parms);
}

void setup_phy_vars(LTE_DL_FRAME_PARMS* frame_parms, PHY_VARS_eNB* phy_vars_ch_src, 
    PHY_VARS_UE** phy_vars_mr, PHY_VARS_eNB* phy_vars_ch_dest, int n_relays)
{
  int k;

  phy_vars_ch_src->lte_frame_parms = *frame_parms;
  phy_vars_ch_src->frame = 1;
  phy_init_lte_eNB(phy_vars_ch_src, 0, 0, 0);
  for(k = 0; k < n_relays; k++) {
    phy_vars_mr[k]->lte_frame_parms = *frame_parms;
    phy_vars_mr[k]->frame = 1;
    lte_gold(frame_parms, phy_vars_mr[k]->lte_gold_table[0], 0);
    lte_gold(frame_parms, phy_vars_mr[k]->lte_gold_table[1], 1);
    lte_gold(frame_parms, phy_vars_mr[k]->lte_gold_table[2], 2);

    phy_init_lte_ue(phy_vars_mr[k], 0);
    phy_vars_mr[k]->pucch_config_dedicated[0].tdd_AckNackFeedbackMode = bundling;
    phy_vars_mr[k]->pusch_config_dedicated[0].betaOffset_ACK_Index = 0;
    phy_vars_mr[k]->pusch_config_dedicated[0].betaOffset_RI_Index  = 0;
    phy_vars_mr[k]->pusch_config_dedicated[0].betaOffset_CQI_Index = 2;
  }
    
  phy_vars_ch_dest->lte_frame_parms = *frame_parms;
  phy_vars_ch_dest->frame = 1;
  phy_init_lte_eNB(phy_vars_ch_dest, 0, 2, 0);

  phy_vars_ch_dest->transmission_mode[0] = 2;
  phy_vars_ch_dest->pucch_config_dedicated[0].tdd_AckNackFeedbackMode = bundling;
  phy_vars_ch_dest->pusch_config_dedicated[0].betaOffset_ACK_Index = 0;
  phy_vars_ch_dest->pusch_config_dedicated[0].betaOffset_RI_Index  = 0;
  phy_vars_ch_dest->pusch_config_dedicated[0].betaOffset_CQI_Index = 2;
}

void alloc_broadcast_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays, uint16_t rnti)
{
  int k;

  // Workaround for memory leak:
  phy_vars_ch->dlsch_eNB[0][0] = new_eNB_dlsch(1, 8, 0);
  free(phy_vars_ch->dlsch_eNB[0][0]->harq_processes[0]->b);

  for(k = 0; k < n_relays; k++) {
    phy_vars_mr[k]->dlsch_ue[0][0] = new_ue_dlsch(1, 8, 0);
    phy_vars_mr[k]->dlsch_ue[0][0]->mode1_flag = 0;
    memset(phy_vars_mr[k]->dlsch_ue[0][0]->rb_alloc, 0, 16);
  }
}

void free_broadcast_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays)
{
  int k;

  // Workaround for memory leak:
  phy_vars_ch->dlsch_eNB[0][0]->harq_processes[0]->b = 0;
  free_eNB_dlsch(phy_vars_ch->dlsch_eNB[0][0]);
  for(k = 0; k < n_relays; k++) {
    free_ue_dlsch(phy_vars_mr[k]->dlsch_ue[0][0]);
  }
}

void alloc_distributed_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays, uint16_t rnti)
{
  int k;
  int l;

  for(k = 0; k < n_relays; k++) {
    phy_vars_mr[k]->ulsch_ue[0] = new_ue_ulsch(8, 0);
    phy_vars_mr[k]->ulsch_ue[0]->o_ACK[0] = 0;
    phy_vars_mr[k]->ulsch_ue[0]->o_ACK[1] = 0;
    phy_vars_mr[k]->ulsch_ue[0]->o_ACK[2] = 0;
    phy_vars_mr[k]->ulsch_ue[0]->o_ACK[3] = 0;
    for(l = 0; l < 3; l++)
      if(phy_vars_mr[k]->ulsch_ue[0]->harq_processes[l]) {
        phy_vars_mr[k]->ulsch_ue[0]->harq_processes[l]->status = DISABLED;
        phy_vars_mr[k]->ulsch_ue[0]->harq_processes[l]->B = 0;
      }
  }
  phy_vars_ch->ulsch_eNB[0] = new_eNB_ulsch(8, 0);
}

void free_distributed_transport_channel(PHY_VARS_eNB* phy_vars_ch, PHY_VARS_UE** phy_vars_mr, int n_relays)
{
  int k;

  for(k = 0; k < n_relays; k++) {
    free_ue_ulsch(phy_vars_mr[k]->ulsch_ue[0]);
  }
  //free_eNB_ulsch(phy_vars_ch->ulsch_eNB[0]);
}

uint16_t rballoc_type0(int n_rb, int rbg_size)
{
  int rb = 0;
  int k;

  for(k = 0; k < n_rb; k += rbg_size)
    rb = (rb << 1) + 1;
  return rb;
}

void setup_broadcast_dci(DCI_ALLOC_t* dci, uint16_t rnti, int harq_round, int mcs, int n_rb)
{
  DCI1_5MHz_TDD_t* dci_data = (DCI1_5MHz_TDD_t*) dci->dci_pdu;

  memset(dci, 0, sizeof(DCI_ALLOC_t));

  dci_data->dai = 1;
  dci_data->TPC = 0;
  dci_data->rv = (harq_round >> 1) & 0x03;
  dci_data->ndi = (harq_round == 0 ? 1 : 0);
  dci_data->harq_pid = 0;
  dci_data->mcs = mcs;
  dci_data->rballoc = rballoc_type0(n_rb, RBG_SIZE);
  dci_data->rah = 0;

  dci->dci_length = sizeof_DCI1_5MHz_TDD_t;
  dci->L = 1;
  dci->rnti = rnti;
  dci->format = format1;
}

void setup_distributed_dci(DCI_ALLOC_t* dci, uint16_t rnti, int harq_round, int mcs, int n_rb)
{
  DCI0_5MHz_TDD_1_6_t* dci_data = (DCI0_5MHz_TDD_1_6_t*) dci->dci_pdu;

  memset(dci, 0, sizeof(DCI_ALLOC_t));

  dci_data->cqi_req = 0;
  dci_data->dai = 1;
  dci_data->cshift = 0;
  dci_data->TPC = 0;
  if(harq_round == 0) {
    dci_data->ndi = 1;
    dci_data->mcs = mcs;
  } else {
    dci_data->ndi = 0;
    switch(harq_round % 4) {
      case 0:
        dci_data->mcs = mcs;
        break;
      case 1:
	//        dci_data->mcs = 30;
        dci_data->mcs = mcs;
        break;
      case 2:
	//        dci_data->mcs = 31;
        dci_data->mcs = 29;
        break;
      case 3:
        dci_data->mcs = 29;
        break;
    }
  }
  dci_data->rballoc = computeRIV(N_PRB,0,n_rb);
  dci_data->hopping = 0;
  dci_data->type = 0;

  dci->dci_length = sizeof_DCI0_5MHz_TDD_1_6_t;
  dci->L = 1;
  dci->rnti = rnti;
  dci->format = format0;
}

void ofdm_modulation(mod_sym_t** tx_f, int32_t** tx_t, LTE_DL_FRAME_PARMS* frame_parms, uint8_t subframe, uint8_t nsymb)
{
  mod_sym_t* src;
  int32_t* dst;

  if(frame_parms->Ncp == 0) { // Normal prefix
    src = &tx_f[0][subframe*14*frame_parms->ofdm_symbol_size];
    dst = &tx_t[0][subframe*frame_parms->samples_per_tti];
    normal_prefix_mod(src, dst, nsymb, frame_parms);
  } else { // Extended prefix
    src = &tx_f[0][subframe*12*frame_parms->ofdm_symbol_size];
    dst = &tx_t[0][subframe*frame_parms->samples_per_tti];
    PHY_ofdm_mod(src, dst, frame_parms->log2_symbol_size,
        nsymb, frame_parms->nb_prefix_samples, frame_parms->twiddle_ifft, 
        frame_parms->rev, CYCLIC_PREFIX);
  }
}

channel_vars_t alloc_channel_vars(LTE_DL_FRAME_PARMS* frame_parms)
{
  channel_vars_t v;
  v.s_re[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  v.s_im[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  v.r_re[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  v.r_im[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  v.r_re_t[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  v.r_im_t[0] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  return v;
}

void free_channel_vars(channel_vars_t v)
{
  free(v.s_re[0]);
  free(v.s_im[0]);
  free(v.r_re[0]);
  free(v.r_im[0]);
  free(v.r_re_t[0]);
  free(v.r_im_t[0]);
}

sh_channel_t* alloc_sh_channel(channel_vars_t* cvars, SCM_t channel_model, int n_txantennas, int n_rxantennas,
    double channel_correlation)
{
  sh_channel_t* ch = malloc(sizeof(sh_channel_t));

  ch->cvars = cvars;
  ch->channel = new_channel_desc_scm(n_txantennas, n_rxantennas, channel_model, BW, channel_correlation, 0, 0.0);

  return ch;
}

void free_sh_channel(sh_channel_t* c)
{
  free(c->channel);
}

void transmit_subframe(sh_channel_t* channel, int32_t** src, LTE_DL_FRAME_PARMS* frame_parms, 
    uint8_t subframe, uint8_t nsymb, double ampl, bool accumulate)
{
  int k;
  int symbols_per_slot = (frame_parms->Ncp == 0 ? 7 : 6);
  int nsamples = 0;

  for(k = 0; k < nsymb; k++) {
    if(k % symbols_per_slot == 0)
      nsamples += frame_parms->nb_prefix_samples0;
    else
      nsamples += frame_parms->nb_prefix_samples;
    nsamples += frame_parms->ofdm_symbol_size;
  }

  for(k = 0; k < nsamples; k++) {
    channel->cvars->s_re[0][k] = (double)((int16_t*)src[0])[2*subframe*frame_parms->samples_per_tti + (k<<1)];
    channel->cvars->s_im[0][k] = (double)((int16_t*)src[0])[2*subframe*frame_parms->samples_per_tti + (k<<1) + 1];
    /*
    if(accumulate) {
      channel->cvars->r_re_t[0][k] = channel->cvars->s_re[0][k];
      channel->cvars->r_im_t[0][k] = channel->cvars->s_im[0][k];
    } else {
      channel->cvars->r_re[0][k] = channel->cvars->s_re[0][k];
      channel->cvars->r_im[0][k] = channel->cvars->s_im[0][k];
    }
    */
    //channel->cvars->s_re[1][k] = 0;
    //channel->cvars->s_im[1][k] = 0;
  }

  if(accumulate) {
    multipath_channel(channel->channel, channel->cvars->s_re, channel->cvars->s_im,
        channel->cvars->r_re_t, channel->cvars->r_im_t, nsamples, 0);
    for(k = 0; k < nsamples; k++) {
      channel->cvars->r_re[0][k] += channel->cvars->r_re_t[0][k] * ampl;
      channel->cvars->r_im[0][k] += channel->cvars->r_im_t[0][k] * ampl;
    }
  } else {
    multipath_channel(channel->channel, channel->cvars->s_re, channel->cvars->s_im,
        channel->cvars->r_re, channel->cvars->r_im, nsamples, 0);
    for(k = 0; k < nsamples; k++) {
      channel->cvars->r_re[0][k] *= ampl;
      channel->cvars->r_im[0][k] *= ampl;
    }
  }
}

void deliver_subframe(sh_channel_t* channel, int32_t** dst, LTE_DL_FRAME_PARMS* frame_parms,
    uint8_t subframe, uint8_t nsymb, double stddev)
{
  int k;
  int symbols_per_slot = (frame_parms->Ncp == 0 ? 7 : 6);
  int nsamples = 0;

  for(k = 0; k < nsymb; k++) {
    //    printf("deliver_subframe symbol k %d\n",k);
    if(k % symbols_per_slot == 0)
      nsamples += frame_parms->nb_prefix_samples0;
    else
      nsamples += frame_parms->nb_prefix_samples;
    nsamples += frame_parms->ofdm_symbol_size;
  }

  for(k = 0; k < nsamples; k++) {
    ((int16_t*)dst[0])[2*subframe*frame_parms->samples_per_tti + (k<<1)] = 
      (int16_t) (channel->cvars->r_re[0][k] + stddev*0.707*gaussdouble(0.0, 1.0));
    ((int16_t*)dst[0])[2*subframe*frame_parms->samples_per_tti + (k<<1) + 1] = 
      (int16_t) (channel->cvars->r_im[0][k] + stddev*0.707*gaussdouble(0.0, 1.0));
  }
}

void ofdm_fep(PHY_VARS_UE* phy_vars_mr, uint8_t subframe)
{
  int n_symbols_per_slot = (phy_vars_mr->lte_frame_parms.Ncp == 0 ? 7 : 6);
  int slot;
  int symbol;

  //slot = subframe;
  for(slot = 2*subframe; slot < 2*subframe+2; slot++)
    for(symbol = 0; symbol < n_symbols_per_slot; symbol++)
      slot_fep(phy_vars_mr, symbol, slot, 0, 0);
  slot_fep(phy_vars_mr, 0, 2*subframe+2, 0, 0);
}

int rx_dlsch_symbol(PHY_VARS_UE* phy_vars, uint8_t subframe, uint8_t symbol, uint8_t first_symbol)
{
  int s;
  s = rx_pdsch(phy_vars, PDSCH, 0, 0, subframe, symbol, first_symbol, 0, 0);
  if(s == -1)
    printf("DLSCH receiver error\n");
  return s;
}

uint32_t get_ulsch_G(LTE_UE_ULSCH_t *ulsch, uint8_t harq_pid)
{
  uint8_t Q_m = 0;
  uint32_t Kr = 0;
  int r;
  uint32_t sumKr = 0;
  uint32_t Qprime;
  uint32_t L;
  uint32_t G;
  uint32_t Q_CQI = 0;
  uint32_t Q_RI = 0;

  Q_m = get_Qm(ulsch->harq_processes[harq_pid]->mcs);

  sumKr = 0;
  for (r=0;r<ulsch->harq_processes[harq_pid]->C;r++) {
    if (r<ulsch->harq_processes[harq_pid]->Cminus)
      Kr = ulsch->harq_processes[harq_pid]->Kminus;
    else
      Kr = ulsch->harq_processes[harq_pid]->Kplus;
    sumKr += Kr;
  }

  Qprime = ulsch->O_RI * ulsch->harq_processes[harq_pid]->Msc_initial * 
    ulsch->harq_processes[harq_pid]->Nsymb_initial * ulsch->beta_offset_ri_times8;

  if (Qprime > 0) {
    if ((Qprime % (8*sumKr)) > 0)
      Qprime = 1+(Qprime/(8*sumKr));
    else
      Qprime = Qprime/(8*sumKr);
    
    if (Qprime > 4*ulsch->harq_processes[harq_pid]->nb_rb * 12)
      Qprime = 4*ulsch->harq_processes[harq_pid]->nb_rb * 12;
  }

  Q_RI = Q_m*Qprime;

  if (ulsch->O < 12)
    L=0;
  else 
    L=8;

  Qprime = (ulsch->O + L) * ulsch->harq_processes[harq_pid]->Msc_initial * 
    ulsch->harq_processes[harq_pid]->Nsymb_initial * ulsch->beta_offset_cqi_times8;

  if (Qprime > 0) {
    if ((Qprime % (8*sumKr)) > 0)
      Qprime = 1+(Qprime/(8*sumKr));
    else
      Qprime = Qprime/(8*sumKr);
  }
    
  G = ulsch->harq_processes[harq_pid]->nb_rb * (12 * Q_m) * (ulsch->Nsymb_pusch);

  if (Qprime > (G - ulsch->O_RI))
    Qprime = G - ulsch->O_RI;
  Q_CQI = Q_m * Qprime;
  
  G = G - Q_RI - Q_CQI;
  
  return G;
}

double compute_ber_soft(uint8_t* ref, int16_t* rec, int n)
{
  int k;
  int e = 0;

  for(k = 0; k < n; k++) {
    if((ref[k]==1) != (rec[k]<0)) {
            //printf("error pos %d ( %d => %d)\n",k,ref[k],rec[k]);
      e++;
    }
  }

  return (double)e / (double)n;
}

void print_dlsch_eNB_stats(LTE_eNB_DLSCH_t* d)
{
  int k;
  LTE_DL_eNB_HARQ_t* h;
  if(d)
  {
    printf("eNB dlsch: rnti=%04x, active=%d, current_harq_pid=%d, rb_alloc=%08x %08x %08x %08x, nb_rb=%d, G=%d, layer_index=%d, codebook_index=%d, Mdlharq=%d, Kmimo=%d\n",
        d->rnti, d->active, d->current_harq_pid, 
        d->rb_alloc[0], d->rb_alloc[1], d->rb_alloc[2], d->rb_alloc[3], d->nb_rb, d->G,
        d->layer_index, d->codebook_index, d->Mdlharq, d->Kmimo);
    for(k = 0; k < 8; k++) {
      if(d->harq_processes[k]) {
        h = d->harq_processes[k];
        if(h->status == ACTIVE) {
          printf("HARQ process %d: Ndi=%d, status=%d, TBS=%d, B=%d, round=%d, mcs=%d, rvidx=%d, Nl=%d\n",
              k, h->Ndi, h->status, h->TBS, h->B, h->round, h->mcs, h->rvidx, h->Nl);
        }
      }
    }
  }
}

void print_dlsch_ue_stats(LTE_UE_DLSCH_t* d)
{
  int k;
  LTE_DL_UE_HARQ_t* h;
  if(d)
  {
    printf("UE dlsch: rnti=%04x, active=%d, mode1_flag=%d, current_harq_pid=%d, rb_alloc=%08x %08x %08x %08x, nb_rb=%d, G=%d, layer_index=%d, Mdlharq=%d, Kmimo=%d\n",
        d->rnti, d->active, d->mode1_flag, d->current_harq_pid, 
        d->rb_alloc[0], d->rb_alloc[1], d->rb_alloc[2], d->rb_alloc[3], d->nb_rb, d->G,
        d->layer_index, d->Mdlharq, d->Kmimo);
    for(k = 0; k < 8; k++) {
      if(d->harq_processes[k]) {
        h = d->harq_processes[k];
        if(h->status == ACTIVE || h->TBS > 0) {
          printf("HARQ process %d: Ndi=%d, status=%d, TBS=%d, B=%d, round=%d, mcs=%d, rvidx=%d, Nl=%d\n",
              k, h->Ndi, h->status, h->TBS, h->B, h->round, h->mcs, h->rvidx, h->Nl);
        }
      }
    }
  }
}

void print_ulsch_ue_stats(LTE_UE_ULSCH_t* d)
{
  int k;
  LTE_UL_UE_HARQ_t* h;
  if(d) {
    printf("UE ulsch: Nsymb_pusch=%d, O=%d, o_ACK=%d %d %d %d, O_ACK=%d, Mdlharq=%d, n_DMRS2=%d, cooperation_flag=%d\n",
        d->Nsymb_pusch, d->O, d->o_ACK[0], d->o_ACK[1], d->o_ACK[2], d->o_ACK[3], d->O_ACK, d->Mdlharq, d->n_DMRS2, d->cooperation_flag);
    for(k = 0; k < 3; k++) {
      if(d->harq_processes[k]) {
        h = d->harq_processes[k];
        if(h->status == ACTIVE) {
          printf("HARQ process %d: Ndi=%d, status=%d, subframe_scheduling_flag=%d, first_rb=%d, nb_rb=%d, TBS=%d, B=%d, round=%d, mcs=%d, rvidx=%d\n",
              k, h->Ndi, h->status, h->subframe_scheduling_flag, h->first_rb, h->nb_rb, h->TBS, h->B, h->round, h->mcs, h->rvidx);
        }
      }
    }
  }
}

void print_ulsch_eNB_stats(LTE_eNB_ULSCH_t* d)
{
  int k;
  LTE_UL_eNB_HARQ_t* h;
  if(d) {
    printf("eNB ulsch: Nsymb_pusch=%d, Mdlharq=%d, cqi_crc_status=%d, Or1=%d, Or2=%d, o_RI=%d %d, O_RI=%d, o_ACK=%d %d %d %d, O_ACK=%d, o_RCC=%d, beta_offset_cqi_times8=%d, beta_offset_ri_times8=%d, beta_offset_harqack_times8=%d, rnti=%x, n_DMRS2=%d, cyclicShift=%d, cooperation_flag=%d\n",
        d->Nsymb_pusch, d->Mdlharq, d->cqi_crc_status, d->Or1, d->Or2, d->o_RI[0], d->o_RI[1], d->O_RI, d->o_ACK[0], d->o_ACK[1], d->o_ACK[2], d->o_ACK[3], d->O_ACK, d->o_RCC, d->beta_offset_cqi_times8, d->beta_offset_ri_times8, d->beta_offset_harqack_times8, d->rnti, d->n_DMRS2, d->cyclicShift, d->cooperation_flag);
    for(k = 0; k < 3; k++) {
      if(d->harq_processes[k]) {
        h = d->harq_processes[k];
        if(h->status == ACTIVE) {
          printf("HARQ process %d: Ndi=%d, status=%d, subframe_scheduling_flag=%d, phich_active=%d, phich_ACK=%d, TPC=%d, first_rb=%d, nb_rb=%d, TBS=%d, B=%d, round=%d, mcs=%d, rvidx=%d\n",
              k, h->Ndi, h->status, h->subframe_scheduling_flag, h->phich_active, h->phich_ACK, h->TPC, h->first_rb, h->nb_rb, h->TBS, h->B, h->round, h->mcs, h->rvidx);
        }
      }
    }
  }
}

int block_valid(uint8_t* ref, uint8_t* rec, int n)
{
  int k;

  for(k = 0; k < n; k++) {
    if(ref[k] != rec[k])
      return 0;
      //printf("%d ",k);
  }
  //printf("\n");
  return 1;
}

void init_results(results_t* r, args_t* a)
{
  int k;

  r->n_relays = a->n_relays;
  r->n_pdu = a->n_pdu;
  r->n_harq = a->n_harq;
  r->channel_model = a->channel_model;
  r->mcs_hop1 = malloc(a->n_pdu*sizeof(int*));
  r->mcs_hop2 = malloc(a->n_pdu*sizeof(int*));
  r->tbs_hop1 = malloc(a->n_pdu*sizeof(int*));
  r->tbs_hop2 = malloc(a->n_pdu*sizeof(int*));
  r->n_prb_hop1 = malloc(a->n_pdu*sizeof(int*));
  r->n_prb_hop2 = malloc(a->n_pdu*sizeof(int*));
  for(k = 0; k < a->n_pdu; k++) {
    r->mcs_hop1[k] = malloc(a->n_harq*sizeof(int));
    r->mcs_hop2[k] = malloc(a->n_harq*sizeof(int));
    r->tbs_hop1[k] = malloc(a->n_harq*sizeof(int));
    r->tbs_hop2[k] = malloc(a->n_harq*sizeof(int));
    r->n_prb_hop1[k] = malloc(a->n_harq*sizeof(int));
    r->n_prb_hop2[k] = malloc(a->n_harq*sizeof(int));
  }
  r->relay_activity = malloc((1 << a->n_relays)*sizeof(int));
}

void clear_results(results_t* r)
{
  int k;
  r->snr_hop1 = 0;
  r->snr_hop2 = 0;
  r->n_frames_hop1 = 0;
  r->n_frames_hop2 = 0;
  r->n_bits_hop1 = 0;
  r->n_bits_hop2 = 0;
  for(k = 0; k < MAX_RELAYS; k++) {
    r->ber_hop1[k] = 0.0;
  }
  r->ber_hop2 = 0.0;
  r->n_pdu_success_hop1 = 0;
  r->n_pdu_success_hop2 = 0;
  for(k = 0; k < MAX_HARQ_ROUNDS; k++) {
    r->n_harq_tries_hop1[k] = 0;
    r->n_harq_tries_hop2[k] = 0;
    r->n_harq_success_hop1[k] = 0;
    r->n_harq_success_hop2[k] = 0;
  }
  for(k = 0; k < r->n_pdu; k++) {
    memset(r->mcs_hop1[k], 0, r->n_harq*sizeof(int));
    memset(r->mcs_hop2[k], 0, r->n_harq*sizeof(int));
    memset(r->tbs_hop1[k], 0, r->n_harq*sizeof(int));
    memset(r->tbs_hop2[k], 0, r->n_harq*sizeof(int));
    memset(r->n_prb_hop1[k], 0, r->n_harq*sizeof(int));
    memset(r->n_prb_hop2[k], 0, r->n_harq*sizeof(int));
  }
  memset(r->n_transmissions, 0, MAX_HARQ_ROUNDS*MAX_HARQ_ROUNDS*sizeof(int));
  memset(r->relay_activity, 0, (1 << (r->n_relays))*sizeof(int));
}

void free_results(results_t* r)
{
  int k;
  for(k = 0; k < r->n_pdu; k++) {
    free(r->mcs_hop1[k]);
    free(r->mcs_hop2[k]);
    free(r->tbs_hop1[k]);
    free(r->tbs_hop2[k]);
    free(r->n_prb_hop1[k]);
    free(r->n_prb_hop2[k]);
  }
  free(r->mcs_hop1);
  free(r->mcs_hop2);
  free(r->tbs_hop1);
  free(r->tbs_hop2);
  free(r->n_prb_hop1);
  free(r->n_prb_hop2);
  free(r->relay_activity);
}

void print_results(results_t* r)
{
  int k;

  printf("Hop 1: SNR (");
  for(k = 0; k < r->n_relays; k++)
    printf("%.1f%s", r->snr_hop1[k], k < r->n_relays-1 ? ", " : "");
  printf("), BER (");
  for(k = 0; k < r->n_relays; k++)
    printf("%f%s", r->ber_hop1[k], k < r->n_relays-1 ? ", " : "");
  printf(")\n");
  printf("  avg bits/frame %f, avg frames/received PDU %f, avg delay %f\n",
      (double)r->n_bits_hop1/(double)r->n_frames_hop1,
      (double)r->n_frames_hop1/(double)r->n_pdu_success_hop1,
      calc_delay(r->n_harq_success_hop1, r->n_harq));
      //(double)(r->n_frames_hop1-(r->n_harq_tries_hop1[0]-r->n_pdu_success_hop1)*r->n_harq)/(double)r->n_pdu_success_hop1);
  printf("  HARQ (n_success/n_tries):");
  for(k = 0; k < r->n_harq; k++)
    printf(" %d/%d", r->n_harq_success_hop1[k], r->n_harq_tries_hop1[k]);
  printf("\n");
  printf("Hop 2: SNR (");
  for(k = 0; k < r->n_relays; k++)
    printf("%.1f%s", r->snr_hop2[k], k < r->n_relays-1 ? ", " : "");
  printf("), BER %f\n", r->ber_hop2);
  printf("  avg bits/frame %f, avg frames/received PDU %f, avg delay %f\n", 
      (double)r->n_bits_hop2/(double)r->n_frames_hop2,
      (double)r->n_frames_hop2/(double)r->n_pdu_success_hop2,
      calc_delay(r->n_harq_success_hop2, r->n_harq));
      //(double)(r->n_frames_hop2-(r->n_harq_tries_hop2[0]-r->n_pdu_success_hop2)*r->n_harq)/(double)r->n_pdu_success_hop2);
  printf("  HARQ (n_success/n_tries):");
  for(k = 0; k < r->n_harq; k++)
    printf(" %d/%d", r->n_harq_success_hop2[k], r->n_harq_tries_hop2[k]);
  printf("\n");
  printf("Collaborative link BLER: %d/%d\n", r->n_pdu-r->n_pdu_success_hop2, r->n_pdu);
  printf("Relay activity:");
  if(r->n_relays == 2)
    printf(" MR1(%d) MR2(%d) MR1+MR2(%d)\n", r->relay_activity[1], r->relay_activity[2], r->relay_activity[3]);
  else {
    for(k = 1; k < (1 << r->n_relays); k++)
      printf(" %d", r->relay_activity[k]);
    printf("\n");
  }
}

void write_results_header(FILE* f, results_t* r, int n_tests)
{
  fprintf(f, "%d %d %d %d %d\n", r->n_relays, r->channel_model, n_tests, r->n_pdu, r->n_harq);
}
void write_results_data(FILE* f, results_t* r)
{
  int k;
  int l;
  for(k = 0; k < r->n_relays; k++)
    fprintf(f, "%f ", r->snr_hop1[k]);
  fprintf(f, "\n");
  for(k = 0; k < r->n_relays; k++)
    fprintf(f, "%f ", r->snr_hop2[k]);
  fprintf(f, "\n");

  fprintf(f, "%d %d %d %d %d %d\n", r->n_frames_hop1, r->n_frames_hop2,
      r->n_bits_hop1, r->n_bits_hop2, r->n_pdu_success_hop1, r->n_pdu_success_hop2);

  for(k = 0; k < r->n_relays; k++)
    fprintf(f, "%f ", r->ber_hop1[k]);
  fprintf(f, "%f\n", r->ber_hop2);

  for(k = 0; k < r->n_harq; k++)
    fprintf(f, "%d ", r->n_harq_tries_hop1[k]);
  fprintf(f, "\n");
  for(k = 0; k < r->n_harq; k++)
    fprintf(f, "%d ", r->n_harq_success_hop1[k]);
  fprintf(f, "\n");
  for(k = 0; k < r->n_harq; k++)
    fprintf(f, "%d ", r->n_harq_tries_hop2[k]);
  fprintf(f, "\n");
  for(k = 0; k < r->n_harq; k++)
    fprintf(f, "%d ", r->n_harq_success_hop2[k]);
  fprintf(f, "\n");

  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->mcs_hop1[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->mcs_hop2[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->tbs_hop1[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->tbs_hop2[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->n_prb_hop1[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_pdu; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->n_prb_hop2[l][k]);
    fprintf(f, "\n");
  }
  for(l = 0; l < r->n_harq; l++) {
    for(k = 0; k < r->n_harq; k++)
      fprintf(f, "%d ", r->n_transmissions[l][k]);
    fprintf(f, "\n");
  }
  for(k = 0; k < (1 << r->n_relays); k++)
    fprintf(f, "%d ", r->relay_activity[k]);
  fprintf(f, "\n");
}

double calc_delay(int* n_frames, int n_harq)
{
  int n = 0;
  int f = 0;
  int k;
  for(k = 0; k < n_harq; k++) {
    n += n_frames[k]*(k+1);
    f += n_frames[k];
  }
  return (double)n/(double)f;
}

