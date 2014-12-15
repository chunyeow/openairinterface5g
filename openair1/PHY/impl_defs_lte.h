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

/*! \file PHY/impl_defs_lte.h
* \brief LTE Physical channel configuration and variable structure definitions
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/

#ifndef __PHY_IMPLEMENTATION_DEFS_LTE_H__
#define __PHY_IMPLEMENTATION_DEFS_LTE_H__


#include "types.h"
#include "spec_defs_top.h"
//#include "defs.h"

#define LTE_NUMBER_OF_SUBFRAMES_PER_FRAME 10
#define LTE_SLOTS_PER_FRAME  20
#define LTE_CE_FILTER_LENGTH 5
#define LTE_CE_OFFSET LTE_CE_FILTER_LENGTH
#define TX_RX_SWITCH_SYMBOL (NUMBER_OF_SYMBOLS_PER_FRAME>>1) 
#define PBCH_PDU_SIZE 3 //bytes

#define PRACH_SYMBOL 3 //position of the UL PSS wrt 2nd slot of special subframe

#define NUMBER_OF_FREQUENCY_GROUPS (lte_frame_parms->N_RB_DL)

#define SSS_AMP 1148

#define MAX_NUM_PHICH_GROUPS 56  //110 RBs Ng=2, p.60 36-212, Sec. 6.9

#define MAX_MBSFN_AREA 8


typedef enum {TDD=1,FDD=0} lte_frame_type_t;

typedef enum {EXTENDED=1,NORMAL=0} lte_prefix_type_t;

typedef enum {
  normal=0,
  extended=1
} PHICH_DURATION_t;

typedef enum {
  oneSixth=1,
  half=3,
  one=6,
  two=12
} PHICH_RESOURCE_t;

typedef struct {
  /// phich Duration, see 36.211 (Table 6.9.3-1)
  PHICH_DURATION_t phich_duration;
  /// phich_resource, see 36.211 (6.9)
  PHICH_RESOURCE_t phich_resource;
} PHICH_CONFIG_COMMON;

typedef struct {
  /// Config Index
  uint8_t prach_ConfigIndex;
  /// High Speed Flag (0,1)
  uint8_t highSpeedFlag;
  /// Zero correlation zone
  uint8_t zeroCorrelationZoneConfig;
  /// Frequency offset
  uint8_t prach_FreqOffset;
} PRACH_CONFIG_INFO;

typedef struct {
  ///Root Sequence Index (0...837)
  uint16_t rootSequenceIndex;
  /// prach_Config_enabled=1 means enabled
  uint8_t prach_Config_enabled;
  ///PRACH Configuration Information
  PRACH_CONFIG_INFO prach_ConfigInfo;
} PRACH_CONFIG_COMMON;

typedef enum {
  n2=0,
  n4,
  n6
} ACKNAKREP_t;

typedef enum {
  bundling=0,
  multiplexing
} ANFBmode_t;

/// PUCCH-ConfigCommon Structure from 36.331 RRC spec
typedef struct {
  /// Flag to indicate ACK NAK repetition activation, see 36.213 (10.1)
  uint8_t ackNackRepetition;
  /// NANRep, see 36.213 (10.1)
  ACKNAKREP_t repetitionFactor;
  /// n1PUCCH-AN-Rep, see 36.213 (10.1)
  uint16_t n1PUCCH_AN_Rep;
  /// Feedback mode, see 36.213 (7.3).  Applied to both PUCCH and PUSCH feedback.  For TDD, should always be set to bundling.
  ANFBmode_t tdd_AckNackFeedbackMode;
} PUCCH_CONFIG_DEDICATED;

/// PUCCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Parameter rom 36.211, 5.4.1, values 1,2,3
  uint8_t deltaPUCCH_Shift;
  /// NRB2 from 36.211, 5.4
  uint8_t nRB_CQI;
  /// NCS1 from 36.211, 5.4
  uint8_t nCS_AN;
  /// N1PUCCH from 36.213, 10.1
  uint16_t n1PUCCH_AN;
} PUCCH_CONFIG_COMMON;

/// UL-ReferenceSignalsPUSCH from 36.331 RRC spec
typedef struct {
  /// See 36.211 (5.5.1.3) (0,1)
  uint8_t groupHoppingEnabled;
  ///deltaSS see 36.211 (5.5.1.3)
  uint8_t groupAssignmentPUSCH;
  /// See 36.211 (5.5.1.4) (0,1)
  uint8_t sequenceHoppingEnabled;
  /// cyclicShift from 36.211 (see Table 5.5.2.1.1-2) (0...7) n_DMRS1
  uint8_t cyclicShift;
  /// nPRS for cyclic shift of DRS
  uint8_t nPRS[20];
  /// group hopping sequence for DRS
  uint8_t grouphop[20];
  /// sequence hopping sequence for DRS
  uint8_t seqhop[20];
} UL_REFERENCE_SIGNALS_PUSCH_t;
 
typedef enum {
  interSubFrame=0, 
  intraAndInterSubFrame=1
} PUSCH_HOPPING_t;

/// PUSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Nsb from 36.211 (5.3.4)
  uint8_t n_SB;
  /// Hopping mode, see 36.211 (5.3.4)
  PUSCH_HOPPING_t hoppingMode;
  /// NRBHO from 36.211 (5.3.4)
  uint8_t pusch_HoppingOffset;
  /// 1 indicates 64QAM is allowed, 0 not allowed, see 36.213
  uint8_t enable64QAM;
  /// Ref signals configuration
  UL_REFERENCE_SIGNALS_PUSCH_t ul_ReferenceSignalsPUSCH;
} PUSCH_CONFIG_COMMON;

typedef struct {
  /// 
  uint16_t betaOffset_ACK_Index;
  ///
  uint16_t betaOffset_RI_Index;
  /// 
  uint16_t betaOffset_CQI_Index;
} PUSCH_CONFIG_DEDICATED;

/// lola CBA information 
typedef struct {
  /// 
  uint16_t betaOffset_CA_Index;
  ///
  uint16_t cShift;
} PUSCH_CA_CONFIG_DEDICATED;

/// PDSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Donwlink Reference Signal EPRE (-60... 50), 36.213 (5.2)
  int8_t referenceSignalPower;
  /// Parameter PB, 36.213 (Table 5.2-1)
  uint8_t p_b;
} PDSCH_CONFIG_COMMON;

typedef enum {
  dBm6=0,
  dBm477,
  dBm3,
  dBm177,
  dB0,
  dB1,
  dB2,
  dB3
} PA_t;

/// PDSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Parameter PA in dB, 36.213 (5.2)
   PA_t p_a;
} PDSCH_CONFIG_DEDICATED;

/// SoundingRS-UL-ConfigCommon Information Element from 36.331 RRC spec
typedef struct {
  /// enabled flag=1 means SRS is enabled
  uint8_t enabled_flag;
  ///SRS BandwidthConfiguration \f$\in\{0,1,...,7\}\f$ see 36.211 (Table 5.5.3.2-1,5.5.3.2-2,5.5.3-2.3 and 5.5.3.2-4). Actual configuration depends on UL bandwidth.
  uint8_t srs_BandwidthConfig;
  ///SRS Subframe configuration \f$\in\{0,...,15\}\f$ see 36.211 (Table 5.5.3.3-1 FDD, Table 5.5.3.3-2 TDD)
  uint8_t srs_SubframeConfig;
  ///SRS Simultaneous-AN-and-SRS, see 36.213 (8.2)
  uint8_t ackNackSRS_SimultaneousTransmission;
  ///srsMaxUpPts \f$\in\{0,1\}\f$, see 36.211 (5.5.3.2).  If this field is 1, reconfiguration of mmax_SRS0 applies for UpPts, otherwise reconfiguration does not apply
  uint8_t srs_MaxUpPts;
} SOUNDINGRS_UL_CONFIG_COMMON;

typedef enum {
  ulpc_al0=0,
  ulpc_al04=1,
  ulpc_al05=2,
  ulpc_al06=3,
  ulpc_al07=4,
  ulpc_al08=5,
  ulpc_al09=6,
  ulpc_al11=7
} UL_POWER_CONTROL_COMMON_alpha_t;

typedef enum {
        deltaF_PUCCH_Format1_deltaF_2 = 0,
        deltaF_PUCCH_Format1_deltaF0  = 1,
        deltaF_PUCCH_Format1_deltaF2  = 2
} deltaF_PUCCH_Format1_t;
typedef enum {
        deltaF_PUCCH_Format1b_deltaF1 = 0,
        deltaF_PUCCH_Format1b_deltaF3 = 1,
        deltaF_PUCCH_Format1b_deltaF5 = 2
} deltaF_PUCCH_Format1b_t;
typedef enum {
        deltaF_PUCCH_Format2_deltaF_2 = 0,
        deltaF_PUCCH_Format2_deltaF0  = 1,
        deltaF_PUCCH_Format2_deltaF1  = 2,
        deltaF_PUCCH_Format2_deltaF2  = 3
} deltaF_PUCCH_Format2_t;
typedef enum {
        deltaF_PUCCH_Format2a_deltaF_2        = 0,
        deltaF_PUCCH_Format2a_deltaF0 = 1,
        deltaF_PUCCH_Format2a_deltaF2 = 2
} deltaF_PUCCH_Format2a_t;
typedef enum {
        deltaF_PUCCH_Format2b_deltaF_2        = 0,
        deltaF_PUCCH_Format2b_deltaF0         = 1,
        deltaF_PUCCH_Format2b_deltaF2         = 2
} deltaF_PUCCH_Format2b_t;

typedef struct {
        deltaF_PUCCH_Format1_t   deltaF_PUCCH_Format1;
        deltaF_PUCCH_Format1b_t  deltaF_PUCCH_Format1b;
        deltaF_PUCCH_Format2_t   deltaF_PUCCH_Format2;
        deltaF_PUCCH_Format2a_t  deltaF_PUCCH_Format2a;
        deltaF_PUCCH_Format2b_t  deltaF_PUCCH_Format2b;
} deltaFList_PUCCH_t;

/// SoundingRS-UL-ConfigDedicated Information Element from 36.331 RRC spec
typedef struct {
  ///SRS Bandwidth b \f$\in\{0,1,2,3\}\f$
  uint8_t srs_Bandwidth;
  ///SRS Hopping bandwidth bhop \f$\in\{0,1,2,3\}\f$
  uint8_t srs_HoppingBandwidth;
  ///SRS n_RRC Frequency Domain Position \f$\in\{0,1,...,23\}\f$, see 36.211 (5.5.3.2)
  uint8_t freqDomainPosition;
  ///SRS duration, see 36.213 (8.2), 0 corresponds to "single" and 1 to "indefinite"
  uint8_t duration;
  ///SRS Transmission comb kTC \f$\in\{0,1\}\f$, see 36.211 (5.5.3.2)
  uint8_t transmissionComb;
  ///SRS Config Index (Isrs) \f$\in\{0,1,...,1023\}\f$, see 36.213 (8.2)
  uint16_t srs_ConfigIndex;
  ///cyclicShift, n_SRS \f$\in\{0,1,...,7\}\f$, see 36.211 (5.5.3.1)
  uint8_t cyclicShift;
} SOUNDINGRS_UL_CONFIG_DEDICATED;

typedef struct {
  int8_t p0_UE_PUSCH;
  uint8_t deltaMCS_Enabled;
  uint8_t accumulationEnabled;
  int8_t p0_UE_PUCCH;
  int8_t pSRS_Offset;
  uint8_t filterCoefficient;
} UL_POWER_CONTROL_DEDICATED;

typedef enum {
  al0=0,
  al04=1,
  al05=2,
  al06=3,
  al07=4,
  al08=5,
  al09=6,
  al1=7
} PUSCH_alpha_t;

typedef enum {
  deltaFm2=0,
  deltaF0,
  deltaF1,
  deltaF2,
  deltaF3,
  deltaF5  
} deltaF_PUCCH_t;

/// UplinkPowerControlCommon Information Element from 36.331 RRC spec
typedef struct {
  /// p0-NominalPUSCH \f$\in\{-126,...24\}\f$, see 36.213 (5.1.1)
  int8_t p0_NominalPUSCH;
  /// alpha, See 36.213 (5.1.1.1)
  PUSCH_alpha_t alpha;
  /// p0-NominalPUCCH \f$\in\{-127,...,-96\}\f$, see 36.213 (5.1.1)
  int8_t p0_NominalPUCCH;
  /// Power parameter for RRCConnectionRequest
  int8_t deltaPreambleMsg3;
  /// deltaF-PUCCH-Format1, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format1;
  /// deltaF-PUCCH-Format1a, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format1a;
  /// deltaF-PUCCH-Format1b, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format1b;
  /// deltaF-PUCCH-Format2, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format2;
  /// deltaF-PUCCH-Format2a, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format2a;
  /// deltaF-PUCCH-Format2b, see 36.213 (5.1.2)
  long deltaF_PUCCH_Format2b;
} UL_POWER_CONTROL_CONFIG_COMMON;

typedef union {
    /// indexOfFormat3 \f$\in\{1,...,15\}\f$
    uint8_t indexOfFormat3;
    /// indexOfFormat3A \f$\in\{1,...,31\}\f$
    uint8_t indexOfFormat3A;
} TPC_INDEX_t;

typedef struct
{
  uint16_t rnti;
  TPC_INDEX_t tpc_Index;
} TPC_PDCCH_CONFIG;

typedef enum {
  rm12=0,
  rm20=1,
  rm22=2,
  rm30=3,
  rm31=4
} CQI_REPORTMODEAPERIODIC;

typedef enum {
  sr_n4=0,
  sr_n8=1,
  sr_n16=2,
  sr_n32=3,
  sr_n64=4
} DSR_TRANSMAX_t;

typedef struct {
  uint16_t sr_PUCCH_ResourceIndex;
  uint8_t sr_ConfigIndex;
  DSR_TRANSMAX_t dsr_TransMax;
} SCHEDULING_REQUEST_CONFIG;

typedef struct {
  /// Parameter n2pucch, see 36.213 (7.2)
  uint16_t cqi_PUCCH_ResourceIndex;
  /// Parameter Icqi/pmi, see 36.213 (tables 7.2.2-1A and 7.2.2-1C)
  uint16_t cqi_PMI_ConfigIndex;
  /// Parameter K from 36.213 (4.2.2)
  uint8_t K;
  /// Parameter IRI, 36.213 (7.2.2-1B)
  uint16_t ri_ConfigIndex;
  /// Parameter simultaneousAckNackAndCQI
  uint8_t simultaneousAckNackAndCQI;
} CQI_REPORTPERIODIC;

 
typedef struct {
  CQI_REPORTMODEAPERIODIC cqi_ReportModeAperiodic;
  int8_t nomPDSCH_RS_EPRE_Offset;
  CQI_REPORTPERIODIC CQI_ReportPeriodic;
} CQI_REPORT_CONFIG;

typedef struct {
  int radioframeAllocationPeriod;
  int radioframeAllocationOffset;
  int fourFrames_flag;
  int mbsfn_SubframeConfig;
} MBSFN_config_t;

typedef struct {
  /// Number of resource blocks (RB) in DL
  uint8_t N_RB_DL;
  /// Number of resource blocks (RB) in UL
  uint8_t N_RB_UL;
  ///  total Number of Resource Block Groups: this is ceil(N_PRB/P)
  uint8_t N_RBG;
  /// Total Number of Resource Block Groups SubSets: this is P
  uint8_t N_RBGS;
  /// Cell ID                 
  uint16_t Nid_cell;
  /// MBSFN Area ID
  uint16_t Nid_cell_mbsfn;
  /// Cyclic Prefix for DL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp;
  /// Cyclic Prefix for UL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp_UL;                   
  /// shift of pilot position in one RB
  uint8_t nushift;
  /// Frame type (0 FDD, 1 TDD)
  lte_frame_type_t frame_type;
  /// TDD subframe assignment (0-7) (default = 3) (254=RX only, 255=TX only)
  uint8_t tdd_config;
  /// TDD S-subframe configuration (0-9) 
  uint8_t tdd_config_S;
  /// indicates if node is a UE (NODE=2) or eNB (PRIMARY_CH=0).
  uint8_t node_id;
  /// Frequency index of CBMIMO1 card
  uint8_t freq_idx;
  /// RX Frequency for ExpressMIMO/LIME
  uint32_t carrier_freq[4];
  /// TX Frequency for ExpressMIMO/LIME
  uint32_t carrier_freqtx[4];
  /// RX gain for ExpressMIMO/LIME
  uint32_t rxgain[4];
  /// TX gain for ExpressMIMO/LIME
  uint32_t txgain[4];
  /// RF mode for ExpressMIMO/LIME
  uint32_t rfmode[4];
  /// RF RX DC Calibration for ExpressMIMO/LIME
  uint32_t rxdc[4];
  /// RF TX DC Calibration for ExpressMIMO/LIME
  uint32_t rflocal[4];
  /// RF VCO calibration for ExpressMIMO/LIME
  uint32_t rfvcolocal[4];
  /// Turns on second TX of CBMIMO1 card
  uint8_t dual_tx;
  /// flag to indicate SISO transmission
  uint8_t mode1_flag;
  /// Size of FFT  
  uint16_t ofdm_symbol_size;
  /// log2(Size of FFT)  
  uint8_t log2_symbol_size;
  /// Number of prefix samples in all but first symbol of slot
  uint16_t nb_prefix_samples;
  /// Number of prefix samples in first symbol of slot
  uint16_t nb_prefix_samples0;
  /// Carrier offset in FFT buffer for first RE in PRB0
  uint16_t first_carrier_offset;
  /// Number of samples in a subframe
  uint32_t samples_per_tti;
  /// Number of OFDM/SC-FDMA symbols in one subframe (to be modified to account for potential different in UL/DL)
  uint16_t symbols_per_tti;
  /// Number of Transmit antennas in node
  uint8_t nb_antennas_tx;
  /// Number of Receive antennas in node
  uint8_t nb_antennas_rx;
  /// Number of Transmit antennas in eNodeB
  uint8_t nb_antennas_tx_eNB;
  /// Pointer to twiddle factors for FFT
  int16_t *twiddle_fft;
  ///pointer to twiddle factors for IFFT
  int16_t *twiddle_ifft;
  ///pointer to FFT permutation vector
  uint16_t *rev;
  /// PRACH_CONFIG
  PRACH_CONFIG_COMMON prach_config_common;
  /// PUCCH Config Common (from 36-331 RRC spec)
  PUCCH_CONFIG_COMMON pucch_config_common;
  /// PDSCH Config Common (from 36-331 RRC spec)
  PDSCH_CONFIG_COMMON pdsch_config_common;
  /// PUSCH Config Common (from 36-331 RRC spec)
  PUSCH_CONFIG_COMMON pusch_config_common;
  /// PHICH Config (from 36-331 RRC spec)
  PHICH_CONFIG_COMMON phich_config_common;
  /// SRS Config (from 36-331 RRC spec)
  SOUNDINGRS_UL_CONFIG_COMMON soundingrs_ul_config_common;
  /// UL Power Control (from 36-331 RRC spec)
  UL_POWER_CONTROL_CONFIG_COMMON ul_power_control_config_common;
  /// Number of MBSFN Configurations
  int num_MBSFN_config;
  /// Array of MBSFN Configurations (max 8 elements as per 36.331)
  MBSFN_config_t MBSFN_config[8];
  /// Maximum Number of Retransmissions of RRCConnectionRequest (from 36-331 RRC Spec)
  uint8_t maxHARQ_Msg3Tx;
  /// Size of SI windows used for repetition of one SI message (in frames)
  uint8_t SIwindowsize;
  /// Period of SI windows used for repetition of one SI message (in frames)
  uint16_t SIPeriod;
  /// REGs assigned to PCFICH
  uint16_t pcfich_reg[4];
  /// Index of first REG assigned to PCFICH
  uint8_t pcfich_first_reg_idx;
  /// REGs assigned to PHICH
  uint16_t phich_reg[MAX_NUM_PHICH_GROUPS][3];

  struct MBSFN_SubframeConfig *mbsfn_SubframeConfig[MAX_MBSFN_AREA];

} LTE_DL_FRAME_PARMS;

typedef enum {
  SISO=0,
  ALAMOUTI=1,
  LARGE_CDD=2,
  UNIFORM_PRECODING11=3,
  UNIFORM_PRECODING1m1=4,
  UNIFORM_PRECODING1j=5,
  UNIFORM_PRECODING1mj=6,
  PUSCH_PRECODING0=7,
  PUSCH_PRECODING1=8,
  DUALSTREAM_UNIFORM_PRECODING1=9,
  DUALSTREAM_UNIFORM_PRECODINGj=10,
  DUALSTREAM_PUSCH_PRECODING=11,
  TM8=12,
  TM9_10=13
} MIMO_mode_t;

typedef struct{
  ///holds the transmit data in time domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  int32_t **txdata[3];
  ///holds the transmit data in the frequency domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  mod_sym_t **txdataF[3];    
  ///holds the received data in time domain (should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  int32_t **rxdata[3];
  ///holds the last subframe of received data in time domain after removal of 7.5kHz frequency offset
  int32_t **rxdata_7_5kHz[3];
  ///holds the received data in the frequency domain
  int32_t **rxdataF[3];
  /// holds output of the sync correlator
  uint32_t *sync_corr[3];
} LTE_eNB_COMMON;

typedef struct{
  /// hold the channel estimates in frequency domain based on SRS
  int32_t **srs_ch_estimates[3];
  /// hold the channel estimates in time domain based on SRS
  int32_t **srs_ch_estimates_time[3];
  /// holds the SRS for channel estimation at the RX    
  int32_t *srs;
} LTE_eNB_SRS;

typedef struct{
  ///holds the received data in the frequency domain for the allocated RBs in repeated format
  int32_t **rxdataF_ext[3];
  ///holds the received data in the frequency domain for the allocated RBs in normal format
  int32_t **rxdataF_ext2[3];
  /// hold the channel estimates in time domain based on DRS   
  int32_t **drs_ch_estimates_time[3];
  /// hold the channel estimates in frequency domain based on DRS   
  int32_t **drs_ch_estimates[3];
  /// hold the channel estimates for UE0 in case of Distributed Alamouti Scheme
  int32_t **drs_ch_estimates_0[3];
  /// hold the channel estimates for UE1 in case of Distributed Almouti Scheme 
  int32_t **drs_ch_estimates_1[3];
  /// holds the compensated signal
  int32_t **rxdataF_comp[3];
  /// hold the compensated data (y)*(h0*) in case of Distributed Alamouti Scheme
  int32_t **rxdataF_comp_0[3];
  /// hold the compensated data (y*)*(h1) in case of Distributed Alamouti Scheme
  int32_t **rxdataF_comp_1[3];
  int32_t **ul_ch_mag[3];
  int32_t **ul_ch_magb[3];
  /// hold the channel mag for UE0 in case of Distributed Alamouti Scheme
  int32_t **ul_ch_mag_0[3];
  /// hold the channel magb for UE0 in case of Distributed Alamouti Scheme
  int32_t **ul_ch_magb_0[3];
  /// hold the channel mag for UE1 in case of Distributed Alamouti Scheme
  int32_t **ul_ch_mag_1[3];
  /// hold the channel magb for UE1 in case of Distributed Alamouti Scheme
  int32_t **ul_ch_magb_1[3];
  /// measured RX power based on DRS
  int ulsch_power[2];
  /// measured RX power based on DRS for UE0 in case of Distributed Alamouti Scheme
  int ulsch_power_0[2];
  /// measured RX power based on DRS for UE0 in case of Distributed Alamouti Scheme
  int ulsch_power_1[2];
  /// llr values
  int16_t *llr;
#ifdef LOCALIZATION
  /// number of active subcarrier for a specific UE
  int32_t active_subcarrier;
  /// subcarrier power in dBm
  int32_t *subcarrier_power;
#endif
} LTE_eNB_PUSCH;

typedef struct {
  ///holds the transmit data in time domain (for IFFT_FPGA this points to the same memory as PHY_vars->tx_vars[a].TX_DMA_BUFFER)
  int32_t **txdata;
  ///holds the transmit data in the frequency domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  mod_sym_t **txdataF;    
  ///holds the received data in time domain (should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  int32_t **rxdata;
  ///holds the received data in the frequency domain
  int32_t **rxdataF;
  int32_t **rxdataF2;
  /// hold the channel estimates in frequency domain
  int32_t **dl_ch_estimates[7];
  /// hold the channel estimates in time domain (used for tracking)
  int32_t **dl_ch_estimates_time[7];
  /// holds output of the sync correlator  
  int32_t *sync_corr;
  /// estimated frequency offset (in radians) for all subcarriers
  int32_t freq_offset;
  /// eNb_id user is synched to          
  int32_t eNb_id;
} LTE_UE_COMMON;

typedef struct {
  /// Received frequency-domain signal after extraction
  int32_t **rxdataF_ext;
  /// Received frequency-domain signal after extraction and channel compensation
  int32_t **rxdataF_comp0;
  /// Received frequency-domain signal after extraction and channel compensation
  int32_t **rxdataF_comp1[8];
  /// Downlink channel estimates extracted in PRBS
  int32_t **dl_ch_estimates_ext;
  /// Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS
  int32_t **dl_ch_rho_ext;
  /// Downlink PMIs extracted in PRBS and grouped in subbands
  uint8_t *pmi_ext;
  /// Magnitude of Downlink Channel first layer (16QAM level/First 64QAM level)
  int32_t **dl_ch_mag0;
  /// Magnitude of Downlink Channel second layer (16QAM level/First 64QAM level)
  int32_t **dl_ch_mag1;
  /// Magnitude of Downlink Channel, first layer (2nd 64QAM level)
  int32_t **dl_ch_magb0;
  /// Magnitude of Downlink Channel second layer (2nd 64QAM level)
  int32_t **dl_ch_magb1;
  /// Cross-correlation of two eNB signals
  int32_t **rho;
  /// never used... always send dl_ch_rho_ext instead...
  int32_t **rho_i;
  /// Pointers to llr vectors (2 TBs)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  int16_t log2_maxh;
  /// LLR shifts for subband scaling
  uint8_t *llr_shifts;
  /// Pointer to LLR shifts
  uint8_t *llr_shifts_p;
  /// Pointers to llr vectors (128-bit alignment)
  int16_t **llr128;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
} LTE_UE_PDSCH;

typedef struct {
  /// Received frequency-domain signal after extraction
  int32_t **rxdataF_ext;
  /// Received frequency-domain signal after extraction and channel compensation
  double **rxdataF_comp;
  /// Downlink channel estimates extracted in PRBS
  int32_t **dl_ch_estimates_ext;
  /// Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS
  double **dl_ch_rho_ext;
  /// Downlink PMIs extracted in PRBS and grouped in subbands
  uint8_t *pmi_ext;
  /// Magnitude of Downlink Channel (16QAM level/First 64QAM level)
  double **dl_ch_mag;
  /// Magnitude of Downlink Channel (2nd 64QAM level)
  double **dl_ch_magb;
  /// Cross-correlation of two eNB signals
  double **rho;
  /// never used... always send dl_ch_rho_ext instead...
  double **rho_i;  
  /// Pointers to llr vectors (2 TBs)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  uint8_t log2_maxh;
  /// Pointers to llr vectors (128-bit alignment)
  int16_t **llr128;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
} LTE_UE_PDSCH_FLP;

typedef struct {
  /// pointers to extracted PDCCH symbols in frequency-domain
  int32_t **rxdataF_ext;
  /// pointers to extracted and compensated PDCCH symbols in frequency-domain
  int32_t **rxdataF_comp;
  /// pointers to extracted channel estimates of PDCCH symbols
  int32_t **dl_ch_estimates_ext;
  /// pointers to channel cross-correlation vectors for multi-eNB detection
  int32_t **dl_ch_rho_ext;
  /// pointers to channel cross-correlation vectors for multi-eNB detection
  int32_t **rho;
  /// pointer to llrs, 4-bit resolution
  uint16_t *llr;
  /// pointer to llrs, 16-bit resolution
  uint16_t *llr16;
  /// \f$\overline{w}\f$ from 36-211
  uint16_t *wbar;
  /// PDCCH/DCI e-sequence (input to rate matching)
  int8_t *e_rx;
  /// number of PDCCH symbols in current subframe
  uint8_t num_pdcch_symbols;
  /// Allocated CRNTI for UE
  uint16_t crnti;
  /// Total number of PDU errors (diagnostic mode)
  uint32_t dci_errors;
  /// Total number of PDU received
  uint32_t dci_received;
  /// Total number of DCI False detection (diagnostic mode)
  uint32_t dci_false;
  /// Total number of DCI missed (diagnostic mode)
  uint32_t dci_missed;
  /// nCCE for PUCCH per subframe
  uint8_t nCCE[10];
} LTE_UE_PDCCH;

#define PBCH_A 24
typedef struct {
  uint8_t pbch_d[96+(3*(16+PBCH_A))];
  uint8_t pbch_w[3*3*(16+PBCH_A)];
  uint8_t pbch_e[1920];
} LTE_eNB_PBCH;

typedef struct {
  /// Pointers to extracted PBCH symbols in frequency-domain
  int32_t **rxdataF_ext;
  /// Pointers to extracted and compensated PBCH symbols in frequency-domain
  int32_t **rxdataF_comp;
  /// Pointers to downlink channel estimates in frequency-domain extracted in PRBS
  int32_t **dl_ch_estimates_ext;
  /// Pointer to PBCH llrs
  int8_t *llr;
  /// Pointer to PBCH decoded output
  uint8_t *decoded_output;
  /// Total number of PDU errors
  uint32_t pdu_errors;
  /// Total number of PDU errors 128 frames ago
  uint32_t pdu_errors_last;
  /// Total number of consecutive PDU errors
  uint32_t pdu_errors_conseq;
  /// FER (in percent) 
  uint32_t pdu_fer;
} LTE_UE_PBCH;

typedef struct {
  int16_t amp;
  int16_t *prachF;
  int16_t *prach;
} LTE_UE_PRACH;

typedef struct {
  int16_t *prachF;
  int16_t *rxsigF[4];
} LTE_eNB_PRACH;

typedef struct {
  /// Preamble index for PRACH (0-63)
  uint8_t ra_PreambleIndex;
  /// RACH MaskIndex
  uint8_t ra_RACH_MaskIndex;
  /// Target received power at eNB (-120 ... -82 dBm)
  int8_t ra_PREAMBLE_RECEIVED_TARGET_POWER;
  /// PRACH index for TDD (0 ... 6) depending on TDD configuration and prachConfigIndex
  uint8_t ra_TDD_map_index;
  /// Corresponding RA-RNTI for UL-grant
  uint16_t ra_RNTI;
  /// Pointer to Msg3 payload for UL-grant
  uint8_t *Msg3;
} PRACH_RESOURCES_t;

typedef struct {
  /// Downlink Power offset field
  uint8_t dl_pow_off;
  ///Subband resource allocation field
  uint8_t rballoc_sub[50];
  ///Total number of PRBs indicator
  uint8_t pre_nb_available_rbs;
}MU_MIMO_mode;

typedef enum {
  NOT_SYNCHED=0,
  PRACH=1,
  RA_RESPONSE=2,
  PUSCH=3,
  RESYNCH=4
} UE_MODE_t;



typedef enum {SF_DL, SF_UL, SF_S} lte_subframe_t;

#endif


