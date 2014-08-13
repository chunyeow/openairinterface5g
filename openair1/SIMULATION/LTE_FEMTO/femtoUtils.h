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
#include "PHY/types.h"
#include "SIMULATION/TOOLS/defs.h"
#include <stdio.h>

#ifndef _FEMTO_UTILS
#define  _FEMTO_UTILS

/*! \file SIMULATION/LTE_FEMTO/femtoUtils.h
* \brief Defines structure and function  headers 
* \author L. Garcia
* \date 2012
* \version 0.1
* \company Eurecom
* \note
* \warning
*/

///Top-level data Structure for general options of simulation and other global variables
typedef struct {
	
    double snr_init;
    double snr_max;
    double snr_step;
    int nframes;
    int interf_count[8];
    int fixed_channel_flag;
    int fixed_data_set;
    int extended_prefix_flag;
    
    ///Frame type (0 FDD, 1 TDD).
    uint8_t frame_type;				
    ///Transmission mode (1 for the moment)
    uint8_t transmission_mode;		
    ///Number of Transmit antennas in node.  
    uint8_t n_tx;
    ///Number of Receive antennas in node. 
    uint8_t n_rx;
    ///Number of interference to simulate
    int nInterf;
    ///Array with interference  level in dB 
    double *dbInterf;
    double *probabilityInterf;
    char interfLevels[150];
    char interfProbability[150];
    
    uint16_t Nid_cell;
    uint16_t tdd_config;
    uint8_t oversampling;
    SCM_t channel_model;  
    SCM_t channel_modeli;   
    int awgn_flag;
    int awgn_flagi;
    int nsymb;
    int num_layers;
    uint16_t n_rnti;
    ///Modulation and code scheme
    uint8_t mcs,mcs2;				    


    uint8_t pilot1,pilot2,pilot3;
    ///Pointer  to the output file SNRvsBLER
    FILE *outputFile;
    ///Pointer  to the output file errors and trials for each SNR 
    FILE *outputBler;
    FILE *outputBer;
    FILE *outputTrougput;    
    
    uint8_t num_rounds;
    uint8_t fix_rounds;
    uint8_t subframe;
 //   int eNB_id;
    ///	Amplitude of QPSK symbols
    int16_t amp;			
    ///1- Analysis  of errors on DCI, 0- No analysis of errors in DCI
    uint8_t dci_flag;		    
    int testNumber;
    char folderName[50];
    char parameters[300];
    
    int SIZE_TXDATAF;
	int SIZE_TXDATA;
	int SIZE_RXDATA;
	int SIZE_RXDATAF;
	
	uint16_t interCellId;
	int n_adj_cells;
	uint8_t dual_stream_UE;
	
	int perfect_ce;
	int common_flag;
	int TPC;
	uint8_t N_RB_DL;
	int rballocset;
	uint32_t DLSCH_RB_ALLOC;
	
	PA_t p_a;
	uint8_t 	 p_b;	
	int8_t d_offset;
	int ratio;
	uint32_t nprb1, nprb2;
	int prob_flag;
	int search_prb2;
	
	char power[50];

} options_t;

/// Store signal data
typedef struct {
    double **s_re;
    double **s_im;
    double **r_re;
    double **r_im;
    double ***is_re;
    double ***is_im;
    double ***ir_re;
    double ***ir_im;
} data_t;

/// Parses the command line options and assigns values ​​to  pilots, num_symbols, etc.  modified by the options selected
void _parseOptions(options_t *opts, int argc, char ** argv);
void _printOptions(options_t *opts);
/// Interference Levels are recivend in form  num,num,num this function parse the string and fill dbInterf array 
void _parseInterferenceLevels(options_t *opts, char *interfLevels,int nInterf);
void _parseInterferenceProbability(options_t *opts, char *interfLevels,int nInterf);


void _parsePower(options_t *opts);
/// Allocate memory  for signal data arrays 
void _allocData(options_t opts,data_t *data, uint8_t n_tx,uint8_t n_rx,int Frame_length_complex_samples);

void copyDataFixed(data_t *origin,data_t *destination,options_t  *opts, int Frame_length_complex_samples);	
/// Generate output dir with the prefix specified in testNumber
void _makeOutputDir(options_t *opts);

/// Initializes  lte_frame_parms structure and make the structures of the  eNB and EU involved in the simulation, including interference, if any.
LTE_DL_FRAME_PARMS* _lte_param_init(options_t opts);
/// Set defaults values for the simulations
void _initDefaults(options_t *opts);
/// Allocate and fill  UL, CCCH and DLSCH structures
void _fill_Ul_CCCH_DLSCH_Alloc(options_t opts);
/// Allocate the eNB2UE structure for transmision and interference
void _generatesRandomChannel(options_t opts);
/** @brief This function allocates structures for a particular DLSCH at eNB.
 *
 * modifications  will be required with more than 2 users
 * It's used to generate the dci. they contain informations such as the modulation and coding scheme
 * either if the transmit power control is required or not and so on...
 * dlsch_eNB can suport 8 users.
 * Create transport channel structures for 2 transport blocks (MIMO)
*/
void _allocDLSChannel(options_t opts);
void _generateDCI(options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx);//,uint8_t **input_buffer);
uint32_t  _allocRBs(options_t *opts, int ind);
void _freeMemory(data_t data,options_t opts);
void _makeSimulation(data_t data,options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx,uint32_t *NB_RB2,LTE_DL_FRAME_PARMS  *frame_parms,uint8_t num_pdcch_symbols);
void _printResults(uint32_t *errs,uint32_t *round_trials,uint32_t dci_errors,double rate);
void _printFileResults(double SNR, double rate1,double rate2	, double rate,uint32_t  *errs,uint32_t  *round_trials,uint32_t dci_errors,options_t opts,double BER);
void _initErrsRoundsTrials(uint32_t **errs,uint32_t **trials,int allocFlag,options_t opts);
void _fillData(options_t opts,data_t data,int numSubFrames);
void _applyNoise(options_t *opts,data_t data,double sigma2,double iqim,int numSubFrames);
uint8_t _generate_dci_top(int num_ue_spec_dci,int num_common_dci,DCI_ALLOC_t *dci_alloc,options_t opts,uint8_t num_pdcch_symbols);
void do_OFDM_mod(mod_sym_t **txdataF, int32_t **txdata, uint16_t next_slot, LTE_DL_FRAME_PARMS *frame_parms);
//void _apply_Multipath_Noise_Interference(options_t opts,data_t data,data_t data_fixed,double sigma2_dB,double sigma2,int numSubFrames);
void _apply_Multipath_Noise_Interference(options_t *opts,data_t data,double sigma2_dB,double sigma2,int numSubFrames,int round);
void _writeOuputOneFrame(options_t opts,uint32_t coded_bits_per_codeword,short *uncoded_ber_bit,uint32_t tbs);
void _dumpTransportBlockSegments(uint32_t C,uint32_t Cminus,uint32_t Kminus,uint32_t Kplus,  uint8_t ** c_UE,uint8_t ** c_eNB);
void _applyInterference(options_t *opts,data_t data,double sigma2,double iqim,int numSubFrames,int round);
double compute_ber_soft(uint8_t* ref, int16_t* rec, int n);
void _fillPerfectChannelDescription(options_t opts,uint8_t l);

void _writeTxData(char *num,char *desc, int init, int numframes,options_t opts, int output,int senial);


#endif
