/*
  \author R. Knopp, F. Kaltenberger
  \company EURECOM
  \email knopp@eurecom.fr
*/

#ifndef __openair_SCHED_H__
#define __openair_SCHED_H__

#include "PHY/defs.h"

enum THREAD_INDEX { OPENAIR_THREAD_INDEX = 0,
		    TOP_LEVEL_SCHEDULER_THREAD_INDEX,
                    DLC_SCHED_THREAD_INDEX,
                    openair_SCHED_NB_THREADS}; // do not modify this line


#define OPENAIR_THREAD_PRIORITY        255


#define OPENAIR_THREAD_STACK_SIZE    8192 //4096 //RTL_PTHREAD_STACK_MIN*6
//#define DLC_THREAD_STACK_SIZE        4096 //DLC stack size




enum openair_SCHED_STATUS {
      openair_SCHED_STOPPED=1,
      openair_SCHED_STARTING,
      openair_SCHED_STARTED,
      openair_SCHED_STOPPING};

enum openair_ERROR {
  // HARDWARE CAUSES
  openair_ERROR_HARDWARE_CLOCK_STOPPED= 1,

  // SCHEDULER CAUSE
  openair_ERROR_OPENAIR_RUNNING_LATE,
  openair_ERROR_OPENAIR_SCHEDULING_FAILED,

  // OTHERS
  openair_ERROR_OPENAIR_TIMING_OFFSET_OUT_OF_BOUNDS,
};

enum openair_SYNCH_STATUS {
      openair_NOT_SYNCHED=1,
#ifdef OPENAIR_LTE
      openair_SYNCHED,
#else
      openair_SYNCHED_TO_CHSCH,
      openair_SYNCHED_TO_MRSCH,
#endif
      openair_SCHED_EXIT};


#define DAQ_AGC_ON 1
#define DAQ_AGC_OFF 0


typedef struct {
  u8 is_eNB;
  u8 mode;
  u8 synch_source;
  u32  slot_count;
  u32  sched_cnt;
  u32  synch_wait_cnt;
  u32  sync_state;
  u32  scheduler_interval_ns;
  u32  last_adac_cnt;
  u8 first_sync_call;
  s32  instance_cnt;
  u8 one_shot_get_frame;
  u8 do_synch;
  u8 node_configured;  // &1..basic config, &3..ue config &5..eNb config
  u8 node_running;
  u8 tx_test;
  u8 mac_registered;
  //u8 freq;
  u32  freq;
  u32  rx_gain_val;
  u32  rx_gain_mode;
  u32  tcxo_dac;
  u32  auto_freq_correction;
  s32  freq_offset;
  u32  tx_rx_switch_point;
  u32  manual_timing_advance;  /// 1 to override automatic timing advance
  u32  timing_advance;
  u32  dual_tx;                /// 1 for dual-antenna TX, 0 for single-antenna TX
  u32  tdd;                    /// 1 for TDD mode, 0 for FDD mode
  u32  rx_rf_mode;
  u32  node_id;
  u32  rach_detection_count;
  u32  channel_vacant[4];  
  u32  target_ue_dl_mcs;
  u32  target_ue_ul_mcs;
  u32  ue_ul_nb_rb;
  u32  ue_dl_rb_alloc;
  u32  dlsch_rate_adaptation;
  u32  dlsch_transmission_mode;
  u32  ulsch_allocation_mode;
  u32  rx_total_gain_dB;
  u32  hw_frame;
  u32  get_frame_done;
  u32  use_ia_receiver;
} OPENAIR_DAQ_VARS;

#ifndef USER_MODE
s32 openair_sched_init(void);
void openair_sched_cleanup(void);
void openair_sched_exit(char *);
void openair1_restart(void);
s32 init_dlsch_threads(void); 
void cleanup_dlsch_threads(void); 
#endif //USER_MODE

#ifdef OPENAIR_LTE
/** @addtogroup _PHY_PROCEDURES_
 * @{
 */


/*!
  \brief Top-level entry routine for eNB procedures.  Called every slot by process scheduler. In even slots, it performs RX functions from previous subframe (if required).  On odd slots, it generate TX waveform for the following subframe.
  @param last_slot Index of last slot (0-19)
  @param next_slot Index of next_slot (0-19)
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
  @param *phy_vars_rn pointer to RN variables
*/
void phy_procedures_eNB_lte(u8 last_slot, u8 next_slot,PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag, relaying_type_t r_type, PHY_VARS_RN *phy_vars_rn);
/*!
  \brief Top-level entry routine for UE procedures.  Called every slot by process scheduler. In even slots, it performs RX functions from previous subframe (if required).  On odd slots, it generate TX waveform for the following subframe.
  @param last_slot Index of last slot (0-19)
  @param next_slot Index of next_slot (0-19)
  @param phy_vars_ue Pointer to UE variables on which to act
  @param eNB_id ID of eNB on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param mode calibration/debug mode
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
  @param *phy_vars_rn pointer to RN variables
*/
void phy_procedures_UE_lte(u8 last_slot, u8 next_slot,PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 abstraction_flag,runmode_t mode,relaying_type_t r_type,PHY_VARS_RN *phy_vars_rn);

#ifdef Rel10  
/*!
  \brief Top-level entry routine for relay node procedures when acting as eNB. This proc will make us of the existing eNB procs. 
  @param last_slot Index of last slot (0-19)
  @param next_slot Index of next_slot (0-19)
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
int phy_procedures_RN_eNB_TX(unsigned char last_slot, unsigned char next_slot, relaying_type_t r_type);
/*!
  \brief Top-level entry routine for relay node procedures actinf as UE. This proc will make us of the existing UE procs. 
  @param last_slot Index of last slot (0-19)
  @param next_slot Index of next_slot (0-19)
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
int phy_procedures_RN_UE_RX(unsigned char last_slot, unsigned char next_slot, relaying_type_t r_type);
#endif

/*!
  \brief Scheduling for UE TX procedures in normal subframes.  
  @param next_slot Index of next slot (0-19)
  @param phy_vars_ue Pointer to UE variables on which to act
  @param eNB_id Local id of eNB on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param mode calib/normal mode
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_UE_TX(u8 next_slot,PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 abstraction_flag,runmode_t mode,relaying_type_t r_type);
/*!
  \brief Scheduling for UE RX procedures in normal subframes.  
  @param last_slot Index of last slot (0-19)
  @param phy_vars_ue Pointer to UE variables on which to act
  @param eNB_id Local id of eNB on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param mode calibration/debug mode
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
  @param phy_vars_rn pointer to RN variables
*/
int phy_procedures_UE_RX(u8 last_slot,PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 abstraction_flag,runmode_t mode,relaying_type_t r_type,PHY_VARS_RN *phy_vars_rn);

/*!
  \brief Scheduling for UE TX procedures in TDD S-subframes.  
  @param next_slot Index of next slot (0-19)
  @param phy_vars_ue Pointer to UE variables on which to act
  @param eNB_id Local id of eNB on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_UE_S_TX(u8 next_slot,PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 abstraction_flag,relaying_type_t r_type);

/*!
  \brief Scheduling for UE RX procedures in TDD S-subframes.  
  @param last_slot Index of last slot (0-19)
  @param phy_vars_ue Pointer to UE variables on which to act
  @param eNB_id Local id of eNB on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_UE_S_RX(u8 last_slot,PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 abstraction_flag, relaying_type_t r_type);

/*!
  \brief Scheduling for eNB TX procedures in normal subframes.  
  @param next_slot Index of next slot (0-19)
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
  @param phy_vars_rn pointer to the RN variables
*/
void phy_procedures_eNB_TX(u8 next_slot,PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag,relaying_type_t r_type,PHY_VARS_RN *phy_vars_rn);

/*!
  \brief Scheduling for eNB RX procedures in normal subframes.  
  @param last_slot Index of last slot (0-19)
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_eNB_RX(u8 last_slot,PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag,relaying_type_t r_type);

/*!
  \brief Scheduling for eNB TX procedures in TDD S-subframes.  
  @param next_slot Index of next slot (0-19)
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_eNB_S_TX(u8 next_slot,PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag,relaying_type_t r_type);

/*!
  \brief Scheduling for eNB RX procedures in TDD S-subframes.  
  @param last_slot Index of next slot (0-19)
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param abstraction_flag Indicator of PHY abstraction
  @param r_type indicates the relaying operation: 0: no_relaying, 1: unicast relaying type 1, 2: unicast relaying type 2, 3: multicast relaying
*/
void phy_procedures_eNB_S_RX(u8 last_slot,PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag,relaying_type_t r_type);

/*!
  \brief Function to compute subframe type as a function of Frame type and TDD Configuration (implements Table 4.2.2 from 36.211, p.11 from version 8.6) and subframe index.
  @param frame_parms Pointer to DL frame parameter descriptor
  @param subframe Subframe index
  @returns Subframe type (DL,UL,S) 
*/
lte_subframe_t subframe_select(LTE_DL_FRAME_PARMS *frame_parms,u8 subframe);

/*!
  \brief Function to compute subframe type as a function of Frame type and TDD Configuration (implements Table 4.2.2 from 36.211, p.11 from version 8.6) and subframe index.  Same as subframe_select, except that it uses the Mod_id and is provided as a service to the MAC scheduler.
  @param Mod_id Index of eNB
  @param subframe Subframe index
  @returns Subframe type (DL,UL,S) 
*/
lte_subframe_t get_subframe_direction(u8 Mod_id, u8 subframe);

/*!
  \brief Function to indicate PHICH transmission subframes.  Implements Table 9.1.2-1 for TDD.
  @param frame_parms Pointer to DL frame parameter descriptor
  @param subframe Subframe index
  @returns 1 if PHICH can be transmitted in subframe (always 1 for FDD)
*/
u32 is_phich_subframe(LTE_DL_FRAME_PARMS *frame_parms,u8 subframe);

/*!
  \brief Function to compute timing of Msg3 transmission on UL-SCH (first UE transmission in RA procedure). This implements the timing in paragraph a) from Section 6.1.1 in 36.213 (p. 17 in version 8.6).  Used by eNB upon transmission of random-access response (RA_RNTI) to program corresponding ULSCH reception procedure.  Used by UE upon reception of random-access response (RA_RNTI) to program corresponding ULSCH transmission procedure.  This does not support the UL_delay field in RAR (always assumed to be 0).
  @param frame_parms Pointer to DL frame parameter descriptor
  @param current_subframe Index of subframe where RA_RNTI was received 
  @param current_frame Index of frame where RA_RNTI was received
  @param frame Frame index where Msg3 is to be transmitted (n+6 mod 10 for FDD, different for TDD)
  @param subframe subframe index where Msg3 is to be transmitted (n, n+1 or n+2)
*/
void get_Msg3_alloc(LTE_DL_FRAME_PARMS *frame_parms,
		    u8 current_subframe, 
		    u32 current_frame,
		    u32 *frame,
		    u8 *subframe);

/*!
  \brief Function to compute timing of Msg3 retransmission on UL-SCH (first UE transmission in RA procedure). 
  @param frame_parms Pointer to DL frame parameter descriptor
  @param current_subframe Index of subframe where RA_RNTI was received 
  @param current_frame Index of frame where RA_RNTI was received
  @param frame Frame index where Msg3 is to be transmitted (n+6 mod 10 for FDD, different for TDD)
  @param subframe subframe index where Msg3 is to be transmitted (n, n+1 or n+2)
*/
void get_Msg3_alloc_ret(LTE_DL_FRAME_PARMS *frame_parms,
			u8 current_subframe, 
			u32 current_frame,
			u32 *frame,
			u8 *subframe);

/* \brief Get ULSCH harq_pid for Msg3 from RAR subframe.  This returns n+k mod 10 (k>6) and corresponds to the rule in Section 6.1.1 from 36.213
   @param frame_parms Pointer to DL Frame Parameters
   @param frame Frame index
   @param current_subframe subframe of RAR transmission
   @returns harq_pid (0 ... 7)
 */
u8 get_Msg3_harq_pid(LTE_DL_FRAME_PARMS *frame_parms,u32 frame,u8 current_subframe);

/* \brief Get ULSCH harq_pid from PHICH subframe
   @param frame_parms Pointer to DL Frame Parameters
   @param subframe subframe of PHICH
   @returns harq_pid (0 ... 7)
 */

/** \brief Function to indicate failure of contention resolution or RA procedure.  It places the UE back in PRACH mode.
    @param Mod_id Instance index of UE
    @param eNB_index Index of eNB
 */
void ra_failed(u8 Mod_id,u8 eNB_index);

/** \brief Function to indicate success of contention resolution or RA procedure.
    @param Mod_id Instance index of UE
    @param eNB_index Index of eNB
 */
void ra_succeeded(u8 Mod_id,u8 eNB_index);

u8 phich_subframe_to_harq_pid(LTE_DL_FRAME_PARMS *frame_parms,u32 frame,u8 subframe);

/* \brief Get PDSCH subframe (n+k) from PDCCH subframe n using relationship from Table 8-2 from 36.213
   @param frame_parms Pointer to DL Frame Parameters
   @param n subframe of PDCCH
   @returns PDSCH subframe (0 ... 7) (note: this is n+k from Table 8-2)
 */
u8 pdcch_alloc2ul_subframe(LTE_DL_FRAME_PARMS *frame_parms,u8 n);

//
/*!
  \brief Compute ACK/NACK information for PUSCH/PUCCH for UE transmission in subframe n. This function implements table 10.1-1 of 36.213, p. 69.
  @param frame_parms Pointer to DL frame parameter descriptor
  @param harq_ack Pointer to dlsch_ue harq_ack status descriptor
  @param subframe Subframe for UE transmission (n in 36.213)
  @param o_ACK Pointer to ACK/NAK payload for PUCCH/PUSCH
  @returns status indicator for PUCCH/PUSCH transmission
*/
u8 get_ack(LTE_DL_FRAME_PARMS *frame_parms,harq_status_t *harq_ack,u8 subframe,u8 *o_ACK);

/*!
  \brief Compute UL ACK subframe from DL subframe. This is used to retrieve corresponding DLSCH HARQ pid at eNB upon reception of ACK/NAK information on PUCCH/PUSCH.  Derived from Table 10.1-1 in 36.213 (p. 69 in version 8.6)
  @param frame_parms Pointer to DL frame parameter descriptor
  @param subframe Subframe for UE transmission (n in 36.213)
  @param ACK_index TTI bundling index (0,1)
  @returns Subframe index for corresponding DL transmission
*/
u8 ul_ACK_subframe2_dl_subframe(LTE_DL_FRAME_PARMS *frame_parms,u8 subframe,u8 ACK_index);

/*!
  \brief Computes number of DL subframes represented by a particular ACK received on UL (M from Table 10.1-1 in 36.213, p. 69 in version 8.6)
  @param frame_parms Pointer to DL frame parameter descriptor
  @param subframe Subframe for UE transmission (n in 36.213)
  @returns Number of DL subframes (M)
*/
u8 ul_ACK_subframe2_M(LTE_DL_FRAME_PARMS *frame_parms,unsigned char subframe);

/*!
  \brief Indicates the SR TXOp in current subframe.  Implements Table 10.1-5 from 36.213.
  @param phy_vars_ue Pointer to UE variables
  @param eNB_id ID of eNB which is to receive the SR
  @param subframe index of next subframe
  @returns 1 if TXOp is active.
*/
u8 is_SR_TXOp(PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 subframe);

/*!
  \brief Indicates the SR TXOp in current subframe for eNB and particular UE index.  Implements Table 10.1-5 from 36.213.
  @param phy_vars_eNB Pointer to eNB variables
  @param UE_id ID of UE which may be issuing the SR
  @param subframe index of last subframe
  @returns 1 if TXOp is active.
*/
u8 is_SR_subframe(PHY_VARS_eNB *phy_vars_eNB,u8 UE_id,u8 subframe);

/*!
  \brief Gives the UL subframe corresponding to a PDDCH order in subframe n
  @param frame_parms Pointer to DL frame parameters
  @param n subframe of PDCCH
  @returns UL subframe corresponding to pdcch order
*/
u8 pdcch_alloc2ul_subframe(LTE_DL_FRAME_PARMS *frame_parms,u8 n);

/*!
  \brief Gives the UL frame corresponding to a PDDCH order in subframe n
  @param frame_parms Pointer to DL frame parameters
  @param frame Frame of received PDCCH
  @param n subframe of PDCCH
  @returns UL frame corresponding to pdcch order
*/
u8 pdcch_alloc2ul_frame(LTE_DL_FRAME_PARMS *frame_parms,u32 frame, u8 n);


u16 get_Np(u8 N_RB_DL,u8 nCCE,u8 plus1);

int get_nCCE_offset(unsigned char L, int nCCE, int common_dci, unsigned short rnti, unsigned char subframe);


s8 find_ue(u16 rnti, PHY_VARS_eNB *phy_vars_eNB);
s32 add_ue(s16 rnti, PHY_VARS_eNB *phy_vars_eNB);
s32 remove_ue(u16 rnti, PHY_VARS_eNB *phy_vars_eNB,u8 abstraction_flag);

void process_timing_advance(u8 Mod_id,s16 timing_advance);
void process_timing_advance_rar(PHY_VARS_UE *phy_vars_ue,u16 timing_advance);


/** \brief This function retrives the resource (n1_pucch) corresponding to a PDSCH transmission in 
subframe n-4 which is acknowledged in subframe n (for FDD) according to n1_pucch = Ncce + N1_pucch.  For
TDD, this routine computes the complex procedure described in Section 10.1 of 36.213 (through tables 10.1-1,10.1-2)
@param phy_vars_ue Pointer to UE variables
@param eNB_id Index of eNB
@param subframe Index of subframe
@param b Pointer to PUCCH payload (b[0],b[1])
@param SR 1 means there's a positive SR in parallel to ACK/NAK
@returns n1_pucch
*/
u16 get_n1_pucch(PHY_VARS_UE *phy_vars_ue,
		 u8 eNB_id,
		 u8 subframe,
		 u8 *b,
		 u8 SR);

/** \brief This function retrives the resource (n1_pucch) corresponding to a PDSCH transmission in 
subframe n-4 which is acknowledged in subframe n (for FDD) according to n1_pucch = Ncce + N1_pucch.  For
TDD, this routine computes the procedure described in Section 10.1 of 36.213 (through tables 10.1-1,10.1-2)
@param phy_vars_eNB Pointer to UE variables
@param eNB_id Index of eNB
@param subframe Index of subframe
@param b Pointer to PUCCH payload (b[0],b[1])
@param n1_pucch0 Pointer to n1_pucch0
@param n1_pucch1 Pointer to n1_pucch1
@param n1_pucch2 Pointer to n1_pucch2
@param n1_pucch3 Pointer to n1_pucch3
*/
void get_n1_pucch_eNB(PHY_VARS_eNB *phy_vars_eNB,
		      u8 UE_id,
		      u8 subframe,
		      s16 *n1_pucch0,
		      s16 *n1_pucch1,
		      s16 *n1_pucch2,
		      s16 *n1_pucch3);


/*!
  \brief This function retrieves the harq_pid of the corresponding DLSCH process and updates the error statistics of the DLSCH based on the received ACK info from UE along with the round index.  It also performs the fine-grain rate-adaptation based on the error statistics derived from the ACK/NAK process.
  @param UE_id Local UE index on which to act
  @param subframe Index of subframe
  @param phy_vars_eNB Pointer to eNB variables on which to act
  @param pusch_flag Indication that feedback came from PUSCH
  @param pucch_payload Resulting payload from pucch
  @param pucch_sel Selection of n1_pucch0 or n1_pucch1 (TDD specific)
  @param SR_payload Indication of SR presence (TDD specific)
*/
void process_HARQ_feedback(u8 UE_id, 
			   u8 subframe, 
			   PHY_VARS_eNB *phy_vars_eNB,
			   u8 pusch_flag, 
			   u8 *pucch_payload, 
			   u8 pucch_sel,
			   u8 SR_payload);

/*!
  \brief This function retrieves the PHY UE mode. It is used as a helper function for the UE MAC.
  @param Mod_id Local UE index on which to act
  @param eNB_index ID of eNB
  @returns UE mode
*/ 
UE_MODE_t get_ue_mode(u8 Mod_id,u8 eNB_index);

/** \brief This function implements the power control mechanism for PUCCH from 36.213.
    @param phy_vars_ue PHY variables
    @param subframe Index of subframe
    @param eNB_id Index of eNB
    @param pucch_fmt Format of PUCCH that is being transmitted
    @returns Transmit power
 */
s8 pucch_power_cntl(PHY_VARS_UE *phy_vars_ue,u8 subframe,u8 eNB_id,PUCCH_FMT_t pucch_fmt);

/** \brief This function implements the power control mechanism for PUCCH from 36.213.
    @param phy_vars_ue PHY variables
    @param subframe Index of subframe
    @param eNB_id Index of eNB
    @param j index of type of PUSCH (SPS, Normal, Msg3)
    @returns Transmit power
 */
void pusch_power_cntl(PHY_VARS_UE *phy_vars_ue,u8 subframe,u8 eNB_id,u8 j, u8 abstraction_flag);

s8 get_PHR(u8 Mod_id, u8 eNB_index);

LTE_eNB_UE_stats* get_eNB_UE_stats(u8 Mod_id, u16 rnti);
int get_ue_active_harq_pid(u8 Mod_id,u16 rnti,u8 subframe,u8 *harq_pid,u8 *round,u8 ul_flag);
void ulsch_decoding_procedures(unsigned char last_slot, unsigned int i, PHY_VARS_eNB *phy_vars_eNB, unsigned char abstraction_flag);


void dump_dlsch(PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 subframe,u8 harq_pid);
void dump_dlsch_SI(PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 subframe);
void dump_dlsch_ra(PHY_VARS_UE *phy_vars_ue,u8 eNB_id,u8 subframe);

u8 pdcch_alloc2ul_frame(LTE_DL_FRAME_PARMS *frame_parms,u32 frame, u8 n);

/**@}*/
#endif //OPENAIR_LTE

extern int slot_irq_handler(int irq, void *cookie);

#endif


