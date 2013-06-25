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
  u8 prach_ConfigIndex;
  /// High Speed Flag (0,1)
  u8 highSpeedFlag;
  /// Zero correlation zone
  u8 zeroCorrelationZoneConfig;
  /// Frequency offset
  u8 prach_FreqOffset;
} PRACH_CONFIG_INFO;

typedef struct {
  ///Root Sequence Index (0...837)
  u16 rootSequenceIndex;
  /// prach_Config_enabled=1 means enabled
  u8 prach_Config_enabled;
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
  u8 ackNackRepetition;
  /// NANRep, see 36.213 (10.1)
  ACKNAKREP_t repetitionFactor;
  /// n1PUCCH-AN-Rep, see 36.213 (10.1)
  u16 n1PUCCH_AN_Rep;
  /// Feedback mode, see 36.213 (7.3).  Applied to both PUCCH and PUSCH feedback.  For TDD, should always be set to bundling.
  ANFBmode_t tdd_AckNackFeedbackMode;
} PUCCH_CONFIG_DEDICATED;

/// PUCCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Parameter rom 36.211, 5.4.1, values 1,2,3
  u8 deltaPUCCH_Shift;
  /// NRB2 from 36.211, 5.4
  u8 nRB_CQI;
  /// NCS1 from 36.211, 5.4
  u8 nCS_AN;
  /// N1PUCCH from 36.213, 10.1
  u16 n1PUCCH_AN;
} PUCCH_CONFIG_COMMON;

/// UL-ReferenceSignalsPUSCH from 36.331 RRC spec
typedef struct {
  /// See 36.211 (5.5.1.3) (0,1)
  u8 groupHoppingEnabled;
  ///deltaSS see 36.211 (5.5.1.3)
  u8 groupAssignmentPUSCH;
  /// See 36.211 (5.5.1.4) (0,1)
  u8 sequenceHoppingEnabled;
  /// cyclicShift from 36.211 (see Table 5.5.2.1.1-2) (0...7) n_DMRS1
  u8 cyclicShift;
  /// nPRS for cyclic shift of DRS
  u8 nPRS[20];
  /// group hopping sequence for DRS
  u8 grouphop[20];
  /// sequence hopping sequence for DRS
  u8 seqhop[20];
} UL_REFERENCE_SIGNALS_PUSCH_t;
 
typedef enum {
  interSubFrame=0, 
  intraAndInterSubFrame=1
} PUSCH_HOPPING_t;

/// PUSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Nsb from 36.211 (5.3.4)
  u8 n_SB;
  /// Hopping mode, see 36.211 (5.3.4)
  PUSCH_HOPPING_t hoppingMode;
  /// NRBHO from 36.211 (5.3.4)
  u8 pusch_HoppingOffset;
  /// 1 indicates 64QAM is allowed, 0 not allowed, see 36.213
  u8 enable64QAM;
  /// Ref signals configuration
  UL_REFERENCE_SIGNALS_PUSCH_t ul_ReferenceSignalsPUSCH;
} PUSCH_CONFIG_COMMON;

typedef struct {
  /// 
  u16 betaOffset_ACK_Index;
  ///
  u16 betaOffset_RI_Index;
  /// 
  u16 betaOffset_CQI_Index;
} PUSCH_CONFIG_DEDICATED;

/// lola CBA information 
typedef struct {
  /// 
  u16 betaOffset_CA_Index;
  ///
  u16 cShift;
} PUSCH_CA_CONFIG_DEDICATED;

/// PDSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Donwlink Reference Signal EPRE (-60... 50), 36.213 (5.2)
  s8 referenceSignalPower;
  /// Parameter PB, 36.213 (Table 5.2-1)
  u8 p_b;
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
  u8 enabled_flag;
  ///SRS BandwidthConfiguration \f$\in\{0,1,...,7\}\f$ see 36.211 (Table 5.5.3.2-1,5.5.3.2-2,5.5.3-2.3 and 5.5.3.2-4). Actual configuration depends on UL bandwidth.
  u8 srs_BandwidthConfig;
  ///SRS Subframe configuration \f$\in\{0,...,15\}\f$ see 36.211 (Table 5.5.3.3-1 FDD, Table 5.5.3.3-2 TDD)
  u8 srs_SubframeConfig;
  ///SRS Simultaneous-AN-and-SRS, see 36.213 (8.2)
  u8 ackNackSRS_SimultaneousTransmission;
  ///srsMaxUpPts \f$\in\{0,1\}\f$, see 36.211 (5.5.3.2).  If this field is 1, reconfiguration of mmax_SRS0 applies for UpPts, otherwise reconfiguration does not apply
  u8 srs_MaxUpPts;
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
  u8 srs_Bandwidth;
  ///SRS Hopping bandwidth bhop \f$\in\{0,1,2,3\}\f$
  u8 srs_HoppingBandwidth;
  ///SRS n_RRC Frequency Domain Position \f$\in\{0,1,...,23\}\f$, see 36.211 (5.5.3.2)
  u8 freqDomainPosition;
  ///SRS duration, see 36.213 (8.2), 0 corresponds to "single" and 1 to "indefinite"
  u8 duration;
  ///SRS Transmission comb kTC \f$\in\{0,1\}\f$, see 36.211 (5.5.3.2)
  u8 transmissionComb;
  ///SRS Config Index (Isrs) \f$\in\{0,1,...,1023\}\f$, see 36.213 (8.2)
  u16 srs_ConfigIndex;
  ///cyclicShift, n_SRS \f$\in\{0,1,...,7\}\f$, see 36.211 (5.5.3.1)
  u8 cyclicShift;
} SOUNDINGRS_UL_CONFIG_DEDICATED;

typedef struct {
  s8 p0_UE_PUSCH;
  u8 deltaMCS_Enabled;
  u8 accumulationEnabled;
  s8 p0_UE_PUCCH;
  s8 pSRS_Offset;
  u8 filterCoefficient; 
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
  s8 p0_NominalPUSCH;
  /// alpha, See 36.213 (5.1.1.1)
  PUSCH_alpha_t alpha;
  /// p0-NominalPUCCH \f$\in\{-127,...,-96\}\f$, see 36.213 (5.1.1)
  s8 p0_NominalPUCCH;
  /// Power parameter for RRCConnectionRequest
  s8 deltaPreambleMsg3;
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
    u8 indexOfFormat3;
    /// indexOfFormat3A \f$\in\{1,...,31\}\f$
    u8 indexOfFormat3A;
} TPC_INDEX_t;

typedef struct
{
  u16 rnti;
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
  u16 sr_PUCCH_ResourceIndex;
  u8 sr_ConfigIndex;
  DSR_TRANSMAX_t dsr_TransMax;
} SCHEDULING_REQUEST_CONFIG;

typedef struct {
  /// Parameter n2pucch, see 36.213 (7.2)
  u16 cqi_PUCCH_ResourceIndex;
  /// Parameter Icqi/pmi, see 36.213 (tables 7.2.2-1A and 7.2.2-1C)
  u16 cqi_PMI_ConfigIndex;
  /// Parameter K from 36.213 (4.2.2)
  u8 K;
  /// Parameter IRI, 36.213 (7.2.2-1B)
  u16 ri_ConfigIndex;
  /// Parameter simultaneousAckNackAndCQI
  u8 simultaneousAckNackAndCQI;
} CQI_REPORTPERIODIC;

 
typedef struct {
  CQI_REPORTMODEAPERIODIC cqi_ReportModeAperiodic;
  s8 nomPDSCH_RS_EPRE_Offset;
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
  u8 N_RB_DL;                
  /// Number of resource blocks (RB) in UL
  u8 N_RB_UL;
/// Number of Resource Block Groups for P=2
  u8 N_RBGS;
  /// Cell ID                 
  u16 Nid_cell;               
  /// MBSFN Area ID
  u16 Nid_cell_mbsfn;
  /// Cyclic Prefix for DL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp;
  /// Cyclic Prefix for UL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp_UL;                   
  /// shift of pilot position in one RB
  u8 nushift;                
  /// Frame type (0 FDD, 1 TDD)
  lte_frame_type_t frame_type;
  /// TDD subframe assignment (0-7) (default = 3) (254=RX only, 255=TX only)
  u8 tdd_config;
  /// TDD S-subframe configuration (0-9) 
  u8 tdd_config_S;
  /// indicates if node is a UE (NODE=2) or eNB (PRIMARY_CH=0).
  u8 node_id;
  /// Frequency index of CBMIMO1 card
  u8 freq_idx;
  /// RX Frequency for ExpressMIMO/LIME
  u32 carrier_freq[4];
  /// TX Frequency for ExpressMIMO/LIME
  u32 carrier_freqtx[4];
  /// RX gain for ExpressMIMO/LIME
  u32 rxgain[4];
  /// TX gain for ExpressMIMO/LIME
  u32 txgain[4];
  /// RF mode for ExpressMIMO/LIME
  u32 rfmode[4];
  /// RF RX DC Calibration for ExpressMIMO/LIME
  u32 rxdc[4];
  /// RF TX DC Calibration for ExpressMIMO/LIME
  u32 rflocal[4];
  /// RF VCO calibration for ExpressMIMO/LIME
  u32 rfvcolocal[4];
  /// Turns on second TX of CBMIMO1 card
  u8 dual_tx;                
  /// flag to indicate SISO transmission
  u8 mode1_flag;           
  /// Size of FFT  
  u16 ofdm_symbol_size;
  /// log2(Size of FFT)  
  u8 log2_symbol_size;
  /// Number of prefix samples in all but first symbol of slot
  u16 nb_prefix_samples;
  /// Number of prefix samples in first symbol of slot
  u16 nb_prefix_samples0;
  /// Carrier offset in FFT buffer for first RE in PRB0
  u16 first_carrier_offset;
  /// Number of samples in a subframe
  u32 samples_per_tti;
  /// Number of OFDM/SC-FDMA symbols in one subframe (to be modified to account for potential different in UL/DL)
  u16 symbols_per_tti;
  /// Number of Transmit antennas in node
  u8 nb_antennas_tx;
  /// Number of Receive antennas in node
  u8 nb_antennas_rx;
  /// Number of Transmit antennas in eNodeB
  u8 nb_antennas_tx_eNB;
  /// Pointer to twiddle factors for FFT
  s16 *twiddle_fft;
  ///pointer to twiddle factors for IFFT
  s16 *twiddle_ifft;                 
  ///pointer to FFT permutation vector
  u16 *rev;
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
  u8 maxHARQ_Msg3Tx;
  /// Size of SI windows used for repetition of one SI message (in frames)
  u8 SIwindowsize;
  /// Period of SI windows used for repetition of one SI message (in frames)
  u16 SIPeriod;
  /// REGs assigned to PCFICH
  u16 pcfich_reg[4];
  /// Index of first REG assigned to PCFICH
  u8 pcfich_first_reg_idx;
  /// REGs assigned to PHICH
  u16 phich_reg[MAX_NUM_PHICH_GROUPS][3];

  struct MBSFN_SubframeConfig *mbsfn_SubframeConfig[MAX_MBSFN_AREA];

} LTE_DL_FRAME_PARMS;

typedef enum {
  SISO=0,
  ALAMOUTI=1,
  ANTCYCLING=2,
  UNIFORM_PRECODING11=3,
  UNIFORM_PRECODING1m1=4,
  UNIFORM_PRECODING1j=5,
  UNIFORM_PRECODING1mj=6,
  PUSCH_PRECODING0=7,
  PUSCH_PRECODING1=8,
  DUALSTREAM_UNIFORM_PRECODING1=9,
  DUALSTREAM_UNIFORM_PRECODINGj=10,
  DUALSTREAM_PUSCH_PRECODING=11
} MIMO_mode_t;

typedef struct{
  ///holds the transmit data in time domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  s32 **txdata[3];           
  ///holds the transmit data in the frequency domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  mod_sym_t **txdataF[3];    
  ///holds the received data in time domain (should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  s32 **rxdata[3];
  ///holds the last subframe of received data in time domain after removal of 7.5kHz frequency offset
  s32 **rxdata_7_5kHz[3];
  ///holds the received data in the frequency domain
  s32 **rxdataF[3];          
  /// holds output of the sync correlator
  u32 *sync_corr[3];         
} LTE_eNB_COMMON;

typedef struct{
  /// hold the channel estimates in frequency domain based on SRS
  s32 **srs_ch_estimates[3];   
  /// hold the channel estimates in time domain based on SRS
  s32 **srs_ch_estimates_time[3];
  /// holds the SRS for channel estimation at the RX    
  s32 *srs;               
} LTE_eNB_SRS;

typedef struct{
  ///holds the received data in the frequency domain for the allocated RBs in repeated format
  s32 **rxdataF_ext[3];       
  ///holds the received data in the frequency domain for the allocated RBs in normal format
  s32 **rxdataF_ext2[3];       
  /// hold the channel estimates in time domain based on DRS   
  s32 **drs_ch_estimates_time[3]; 
  /// hold the channel estimates in frequency domain based on DRS   
  s32 **drs_ch_estimates[3]; 
  /// hold the channel estimates for UE0 in case of Distributed Alamouti Scheme
  s32 **drs_ch_estimates_0[3];
  /// hold the channel estimates for UE1 in case of Distributed Almouti Scheme 
  s32 **drs_ch_estimates_1[3];
  /// holds the compensated signal
  s32 **rxdataF_comp[3];
  /// hold the compensated data (y)*(h0*) in case of Distributed Alamouti Scheme
  s32 **rxdataF_comp_0[3];
  /// hold the compensated data (y*)*(h1) in case of Distributed Alamouti Scheme
  s32 **rxdataF_comp_1[3];
  s32 **ul_ch_mag[3];
  s32 **ul_ch_magb[3];
  /// hold the channel mag for UE0 in case of Distributed Alamouti Scheme
  s32 **ul_ch_mag_0[3];   
  /// hold the channel magb for UE0 in case of Distributed Alamouti Scheme
  s32 **ul_ch_magb_0[3];  
  /// hold the channel mag for UE1 in case of Distributed Alamouti Scheme
  s32 **ul_ch_mag_1[3];   
  /// hold the channel magb for UE1 in case of Distributed Alamouti Scheme
  s32 **ul_ch_magb_1[3]; 
  /// measured RX power based on DRS
  int ulsch_power[2];
  /// measured RX power based on DRS for UE0 in case of Distributed Alamouti Scheme
  int ulsch_power_0[2];
  /// measured RX power based on DRS for UE0 in case of Distributed Alamouti Scheme
  int ulsch_power_1[2];
  /// llr values
  s16 *llr;
} LTE_eNB_PUSCH;

typedef struct {
  ///holds the transmit data in time domain (for IFFT_FPGA this points to the same memory as PHY_vars->tx_vars[a].TX_DMA_BUFFER)
  s32 **txdata;           
  ///holds the transmit data in the frequency domain (for IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  mod_sym_t **txdataF;    
  ///holds the received data in time domain (should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER)
  s32 **rxdata;          
  ///holds the received data in the frequency domain
  s32 **rxdataF;         
  s32 **rxdataF2;         
  /// hold the channel estimates in frequency domain
  s32 **dl_ch_estimates[7];  
  /// hold the channel estimates in time domain (used for tracking)
  s32 **dl_ch_estimates_time[7];
  /// holds output of the sync correlator  
  s32 *sync_corr;         
  /// estimated frequency offset (in radians) for all subcarriers
  s32 freq_offset;
  /// eNb_id user is synched to          
  s32 eNb_id;     
} LTE_UE_COMMON;

typedef struct {
  /// Received frequency-domain signal after extraction
  s32 **rxdataF_ext;
  /// Received frequency-domain signal after extraction and channel compensation
  s32 **rxdataF_comp;
  /// Downlink channel estimates extracted in PRBS
  s32 **dl_ch_estimates_ext;
  /// Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS
  s32 **dl_ch_rho_ext;
  /// Downlink PMIs extracted in PRBS and grouped in subbands
  u8 *pmi_ext;
  /// Magnitude of Downlink Channel (16QAM level/First 64QAM level)
  s32 **dl_ch_mag;
  /// Magnitude of Downlink Channel (2nd 64QAM level)
  s32 **dl_ch_magb;
  /// Cross-correlation of two eNB signals
  s32 **rho;
  /// never used... always send dl_ch_rho_ext instead...
  s32 **rho_i;  
  /// Pointers to llr vectors (2 TBs)
  s16 *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  s16 log2_maxh;
  /// LLR shifts for subband scaling
  u8 *llr_shifts;
  /// Pointer to LLR shifts
  u8 *llr_shifts_p;
  /// Pointers to llr vectors (128-bit alignment)
  s16 **llr128;  
  //u32 *rb_alloc;
  //u8 Qm[2];
  //MIMO_mode_t mimo_mode;
} LTE_UE_PDSCH;

typedef struct {
  /// Received frequency-domain signal after extraction
  s32 **rxdataF_ext;
  /// Received frequency-domain signal after extraction and channel compensation
  double **rxdataF_comp;
  /// Downlink channel estimates extracted in PRBS
  s32 **dl_ch_estimates_ext;
  /// Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS
  double **dl_ch_rho_ext;
  /// Downlink PMIs extracted in PRBS and grouped in subbands
  u8 *pmi_ext;
  /// Magnitude of Downlink Channel (16QAM level/First 64QAM level)
  double **dl_ch_mag;
  /// Magnitude of Downlink Channel (2nd 64QAM level)
  double **dl_ch_magb;
  /// Cross-correlation of two eNB signals
  double **rho;
  /// never used... always send dl_ch_rho_ext instead...
  double **rho_i;  
  /// Pointers to llr vectors (2 TBs)
  s16 *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  u8 log2_maxh;
  /// Pointers to llr vectors (128-bit alignment)
  s16 **llr128;  
  //u32 *rb_alloc;
  //u8 Qm[2];
  //MIMO_mode_t mimo_mode;
} LTE_UE_PDSCH_FLP;

typedef struct {
  /// pointers to extracted PDCCH symbols in frequency-domain
  s32 **rxdataF_ext;
  /// pointers to extracted and compensated PDCCH symbols in frequency-domain
  s32 **rxdataF_comp;
  /// pointers to extracted channel estimates of PDCCH symbols
  s32 **dl_ch_estimates_ext;
  /// pointers to channel cross-correlation vectors for multi-eNB detection
  s32 **dl_ch_rho_ext;
  /// pointers to channel cross-correlation vectors for multi-eNB detection
  s32 **rho;
  /// pointer to llrs, 4-bit resolution
  u16 *llr;
  /// pointer to llrs, 16-bit resolution
  u16 *llr16;
  /// \f$\overline{w}\f$ from 36-211
  u16 *wbar;
  /// PDCCH/DCI e-sequence (input to rate matching)
  s8 *e_rx;
  /// number of PDCCH symbols in current subframe
  u8 num_pdcch_symbols;
  /// Allocated CRNTI for UE
  u16 crnti;
  /// Total number of PDU errors (diagnostic mode)
  u32 dci_errors;          
  /// Total number of PDU received
  u32 dci_received;        
  /// Total number of DCI False detection (diagnostic mode)
  u32 dci_false;           
  /// Total number of DCI missed (diagnostic mode)
  u32 dci_missed;          
  /// nCCE for PUCCH per subframe
  u8 nCCE[10];
} LTE_UE_PDCCH;

#define PBCH_A 24
typedef struct {
  u8 pbch_d[96+(3*(16+PBCH_A))];
  u8 pbch_w[3*3*(16+PBCH_A)];
  u8 pbch_e[1920]; 
} LTE_eNB_PBCH;

typedef struct {
  /// Pointers to extracted PBCH symbols in frequency-domain
  s32 **rxdataF_ext;
  /// Pointers to extracted and compensated PBCH symbols in frequency-domain
  s32 **rxdataF_comp;
  /// Pointers to downlink channel estimates in frequency-domain extracted in PRBS
  s32 **dl_ch_estimates_ext;
  /// Pointer to PBCH llrs
  s8 *llr;
  /// Pointer to PBCH decoded output
  u8 *decoded_output;
  /// Total number of PDU errors
  u32 pdu_errors;          
  /// Total number of PDU errors 128 frames ago
  u32 pdu_errors_last;     
  /// Total number of consecutive PDU errors
  u32 pdu_errors_conseq;   
  /// FER (in percent) 
  u32 pdu_fer;             
} LTE_UE_PBCH;

typedef struct {
  s16 amp;
  s16 *prachF;
  s16 *prach;
} LTE_UE_PRACH;

typedef struct {
  s16 *prachF;
  s16 *rxsigF[4];
} LTE_eNB_PRACH;

typedef struct {
  /// Preamble index for PRACH (0-63)
  u8 ra_PreambleIndex;
  /// RACH MaskIndex
  u8 ra_RACH_MaskIndex;
  /// Target received power at eNB (-120 ... -82 dBm)
  s8 ra_PREAMBLE_RECEIVED_TARGET_POWER;
  /// PRACH index for TDD (0 ... 6) depending on TDD configuration and prachConfigIndex
  u8 ra_TDD_map_index;
  /// Corresponding RA-RNTI for UL-grant
  u16 ra_RNTI;
  /// Pointer to Msg3 payload for UL-grant
  u8 *Msg3;
} PRACH_RESOURCES_t;

typedef struct {
  /// Downlink Power offset field
  u8 dl_pow_off;
  ///Subband resource allocation field
  u8 rballoc_sub[50];
  ///Total number of PRBs indicator
  u8 pre_nb_available_rbs;
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


