/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2010 Eurecom

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
/*! \file defs.h
* \brief mac phy interface primitives 
* \author Raymond Knopp and Navid Nikaein
* \date 2011
* \version 0.5
* \mail navid.nikaein@eurecom.fr or openair_tech@eurecom.fr
* @ingroup _mac

*/

#ifndef __MAC_PHY_PRIMITIVES_H__
#    define __MAC_PHY_PRIMITIVES_H__


#include "LAYER2/MAC/defs.h"



#define MAX_NUMBER_OF_MAC_INSTANCES 16

#define NULL_PDU 255
#define DCI 0
#define DLSCH 1
#define ULSCH 2




/*! \brief MACPHY Interface */
typedef struct
  {
    /// Pointer function that initializes L2
    int (*macphy_init)(int eMBMS_active, u8 CBA_active,u8 HO_active);

    /// Pointer function that stops the low-level scheduler due an exit condition        
    void (*macphy_exit)(const char *);

    // eNB functions
    /// Invoke dlsch/ulsch scheduling procedure for new subframe
    void (*eNB_dlsch_ulsch_scheduler)(u8 Mod_id, u8 cooperation_flag, u32 frame, u8 subframe);//, int calibration_flag);

    /// Fill random access response sdu, passing timing advance
    u16 (*fill_rar)(u8 Mod_id,u32 frame,u8 *dlsch_buffer,u16 N_RB_UL, u8 input_buffer_length);

    /// Terminate the RA procedure upon reception of l3msg on ulsch
    void (*terminate_ra_proc)(u8 Mod_id,u32 frame,u16 UE_id, u8 *l3msg,u16 l3msg_len);

    /// Initiate the RA procedure upon reception (hypothetical) of a valid preamble
    void (*initiate_ra_proc)(u8 Mod_id,u32 frame,u16 preamble,s16 timing_offset,u8 sect_id,u8 subframe,u8 f_id);

    /// cancel an ongoing RA procedure 
    void (*cancel_ra_proc)(u8 Mod_id,u32 frame,u16 preamble);

    /// Get DCI for current subframe from MAC
    DCI_PDU* (*get_dci_sdu)(u8 Mod_id,u32 frame,u8 subframe);

    /// Get DLSCH sdu for particular RNTI and Transport block index
    u8* (*get_dlsch_sdu)(u8 Mod_id,u32 frame,u16 rnti,u8 TB_index);

    /// Send ULSCH sdu to MAC for given rnti
    void (*rx_sdu)(u8 Mod_id,u32 frame,u16 rnti, u8 *sdu,u16 sdu_len);

    /// Indicate failure to synch to external source
    void (*mrbch_phy_sync_failure) (u8 Mod_id,u32 frame, u8 free_eNB_index);

    /// Indicate Scheduling Request from UE
    void (*SR_indication)(u8 Mod_id,u32 frame,u16 rnti,u8 subframe);

    /// Configure Common PHY parameters from SIB1
    void (*phy_config_sib1_eNB)(u8 Mod_id,
				TDD_Config_t *tdd_config,
				u8 SIwindowsize,
				u16 SIperiod);
    
    /// Configure Common PHY parameters from SIB2
    void (*phy_config_sib2_eNB)(u8 Mod_id,
				RadioResourceConfigCommonSIB_t *radioResourceConfigCommon,
				ARFCN_ValueEUTRA_t *ul_CArrierFreq,
				long *ul_Bandwidth,
				AdditionalSpectrumEmission_t *additionalSpectrumEmission,
				struct MBSFN_SubframeConfigList	*mbsfn_SubframeConfigList);
    
#ifdef Rel10
    /// Configure Common PHY parameters from SIB13
    void (*phy_config_sib13_eNB)(u8 Mod_id,int mbsfn_Area_idx,
				long mbsfn_AreaId_r9);
#endif
    
    /// PHY-Config-Dedicated eNB
    void (*phy_config_dedicated_eNB)(u8 Mod_id,u16 rnti,
				     struct PhysicalConfigDedicated *physicalConfigDedicated);

#ifdef Rel10
    /// Get MCH sdu and corresponding MCS for particular MBSFN subframe
    MCH_PDU* (*get_mch_sdu)(uint8_t Mod_id,uint32_t frame,uint32_t subframe);
#endif
    // configure the cba rnti at the physical layer 
    void (*phy_config_cba_rnti)(u8 Mod_id,u8 eNB_flag, u8 index, u16 cba_rnti, u8 cba_group_id, u8 num_active_cba_groups);

    /// UE functions
    
    /// reset the ue phy  
    void (*phy_reset_ue)(u8 Mod_id,u8 eNB_index);
    
    /// Indicate loss of synchronization of PBCH for this eNB to MAC layer
    void (*out_of_sync_ind)(u8 Mod_id,u32 frame,u16 eNB_index);

    ///  Send a received SI sdu
    void (*ue_decode_si)(u8 Mod_id,u32 frame, u8 CH_index, void *pdu, u16 len);

    /// Send a received DLSCH sdu to MAC
    void (*ue_send_sdu)(u8 Mod_id,u32 frame,u8 *sdu,u16 sdu_len,u8 CH_index);

#ifdef Rel10
    /// Send a received MCH sdu to MAC
    void (*ue_send_mch_sdu)(u8 Mod_id,u32 frame,u8 *sdu,u16 sdu_len,u8 eNB_index,u8 sync_area);

    /// Function to check if UE PHY needs to decode MCH for MAC
    /// get the sync area id, and teturn MCS value if need to decode, otherwise -1
    int (*ue_query_mch)(uint8_t Mod_id,uint32_t frame,uint32_t subframe,uint8_t eNB_index,uint8_t *sync_area, uint8_t *mcch_active);
#endif

  /// Retrieve ULSCH sdu from MAC
    void (*ue_get_sdu)(u8 Mod_id,u32 frame,u8 subframe, u8 CH_index,u8 *ulsch_buffer,u16 buflen,u8 *access_mode);

    /// Retrieve RRCConnectionReq from MAC
    PRACH_RESOURCES_t* (*ue_get_rach)(u8 Mod_id,u32 frame,u8 Msg3_flag,u8 subframe);

    /// Process Random-Access Response
    u16 (*ue_process_rar)(u8 Mod_id,u32 frame,u8 *dlsch_buffer,u16 *t_crnti,u8 preamble_index);

    /// Get SR payload (0,1) from UE MAC
    u32 (*ue_get_SR)(u8 Mod_id,u32 frame,u8 eNB_id,u16 rnti,u8 subframe);

    /// Indicate synchronization with valid PBCH
    void (*dl_phy_sync_success) (u8 Mod_id,u32 frame, u8 CH_index,u8 first_sync);

    /// Only calls the PDCP for now
    UE_L2_STATE_t (*ue_scheduler)(u8 Mod_id, u32 frame,u8 subframe, lte_subframe_t direction,u8 eNB_id);

    /// PHY-Config-Dedicated UE
    void (*phy_config_dedicated_ue)(u8 Mod_id,u8 CH_index,
				    struct PhysicalConfigDedicated *physicalConfigDedicated);

    /// Configure Common PHY parameters from SIB1
    void (*phy_config_sib1_ue)(u8 Mod_id,u8 CH_index,
			       TDD_Config_t *tdd_config,
			       u8 SIwindowsize,
			       u16 SIperiod);
    
    /// Configure Common PHY parameters from SIB2
    void (*phy_config_sib2_ue)(u8 Mod_id,u8 CH_index,
			       RadioResourceConfigCommonSIB_t *radioResourceConfigCommon,
			       ARFCN_ValueEUTRA_t *ul_CArrierFreq,
			       long *ul_Bandwidth,
			       AdditionalSpectrumEmission_t *additionalSpectrumEmission,
			       struct MBSFN_SubframeConfigList	*mbsfn_SubframeConfigList);

#ifdef Rel10
    /// Configure Common PHY parameters from SIB13
    void (*phy_config_sib13_ue)(u8 Mod_id,u8 CH_index,int mbsfn_Area_idx,
				long mbsfn_AreaId_r9);
#endif
    /// Configure Common PHY parameters from mobilityControlInfo
    void (*phy_config_afterHO_ue)(u8 Mod_id,u8 CH_index,
				  MobilityControlInfo_t *mobilityControlInfo, 
				  u8 ho_failed);

    /// Function to indicate failure of contention resolution or RA procedure
    void (*ra_failed)(u8 Mod_id,u8 eNB_index);

    /// Function to indicate success of contention resolution or RA procedure
    void (*ra_succeeded)(u8 Mod_id,u8 eNB_index);

    /// Function to indicate the transmission of msg1/rach to MAC
    void (*Msg1_transmitted)(u8 Mod_id,u32 frame,u8 eNB_id);

    /// Function to indicate Msg3 transmission/retransmission which initiates/reset Contention Resolution Timer
    void (*Msg3_transmitted)(u8 Mod_id,u32 frame,u8 eNB_id);

    /// Function to pass inter-cell measurement parameters to PHY (cell Ids)
    void (*phy_config_meas_ue)(u8 Mod_id,u8 eNB_index,u8 n_adj_cells,u32 *adj_cell_id);

    // PHY Helper Functions

    /// RIV computation from PHY
    u16 (*computeRIV)(u16 N_RB_DL,u16 RBstart,u16 Lcrbs);

    /// Downlink TBS table lookup from PHY
    u16 (*get_TBS_DL)(u8 mcs, u16 nb_rb);

    /// Uplink TBS table lookup from PHY
    u16 (*get_TBS_UL)(u8 mcs, u16 nb_rb);

    /// Function to retrieve the HARQ round index for a particular UL/DLSCH and harq_pid
    int (*get_ue_active_harq_pid)(u8 Mod_id, u16 rnti, u8 subframe, u8 *harq_pid, u8 *round, u8 ul_flag);

    /// Function to retrieve number of CCE
    u16 (*get_nCCE_max)(u8 Mod_id);

    /// Function to retrieve number of PRB in an rb_alloc
    u32 (*get_nb_rb)(u8 ra_header, u32 rb_alloc, int n_rb_dl);

    /// Function to retrieve transmission mode for UE
    u8 (*get_transmission_mode)(u16 Mod_id,u16 rnti);

    /// Function to retrieve rb_alloc bitmap from dci rballoc field and VRB type
    u32 (*get_rballoc)(u8 vrb_type, u16 rb_alloc_dci);

    /// Function for UE MAC to retrieve current PHY connectivity mode (PRACH,RA_RESPONSE,PUSCH)
    UE_MODE_t (*get_ue_mode)(u8 Mod_id,u8 eNB_index);

    /// Function for UE MAC to retrieve measured Path Loss
    s16 (*get_PL)(u8 Mod_id,u8 eNB_index);

    /// Function for UE MAC to retrieve the rssi
    u8 (*get_RSSI)(u8 Mod_id);

    /// Function for UE MAC to retrieve the total gain 
    u8 (*get_rx_total_gain_dB)(u8 Mod_id);

    /// Function for UE MAC to retrieve the number of adjustent cells
    u8 (*get_n_adj_cells)(u8 Mod_id);

    /// Function for UE MAC to retrieve RSRP/RSRQ measurements
    u8 (*get_RSRP)(u8 Mod_id,u8 eNB_index);

    /// Function for UE MAC to retrieve RSRP/RSRQ measurements
    u8 (*get_RSRQ)(u8 Mod_id,u8 eNB_index);

    /// Function for UE MAC to set the layer3 filtered RSRP/RSRQ measurements
    u8 (*set_RSRP_filtered)(u8 Mod_id,u8 eNB_index,float rsrp);

    /// Function for UE MAC to set the layer3 filtered RSRP/RSRQ measurements
    u8 (*set_RSRQ_filtered)(u8 Mod_id,u8 eNB_index,float rsrq);

    /// Function for UE/eNB MAC to retrieve number of PRACH in TDD
    u8 (*get_num_prach_tdd)(LTE_DL_FRAME_PARMS *frame_parms);

    /// Function for UE/eNB MAC to retrieve f_id of particular PRACH resource in TDD
    u8 (*get_fid_prach_tdd)(LTE_DL_FRAME_PARMS *frame_parms,u8 tdd_map_index);

    /// Function for eNB MAC to retrieve subframe direction
    lte_subframe_t (*get_subframe_direction)(u8 Mod_id, u8 subframe);

    // MAC Helper functions
    /// Function for UE/PHY to compute PUSCH transmit power in power-control procedure (Po_NOMINAL_PUSCH parameter)
    s8 (*get_Po_NOMINAL_PUSCH)(u8 Mod_id);

    /// Function for UE/PHY to compute PUSCH transmit power in power-control procedure (deltaP_rampup parameter)
    s8 (*get_deltaP_rampup)(u8 Mod_id);

    /// Function for UE/PHY to compute PHR
    s8 (*get_PHR)(u8 Mod_id, u8 eNB_index);

    void (*process_timing_advance)(u8 Mod_id,s16 timing_advance);

    LTE_eNB_UE_stats* (*get_eNB_UE_stats)(u8 Mod_id, u16 rnti);

    unsigned char is_cluster_head;
    unsigned char is_primary_cluster_head;
    unsigned char is_secondary_cluster_head;
    unsigned char cluster_head_index;

    /// PHY Frame Configuration
    LTE_DL_FRAME_PARMS *lte_frame_parms;

    //ICIC algos
    u8 (*get_SB_size)(u8 n_rb_dl);

    //end ALU's algo
    
  } MAC_xface;


#endif


