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
/*! \file proto.h
 * \brief RRC functions prototypes for eNB and UE
 * \author Navid Nikaein and Raymond Knopp
 * \date 2010 - 2014 
 * \email navid.nikaein@eurecom.fr
 * \version 1.0
 * @ingroup _rrc

 */

//main.c
int rrc_init_global_param(void);
int L3_xface_init(void);
void openair_rrc_top_init(int eMBMS_active, uint8_t cba_group_active,uint8_t HO_enabled);
char openair_rrc_lite_eNB_init(module_id_t module_idP);
char openair_rrc_lite_ue_init(module_id_t module_idP,uint8_t CH_IDX);
void rrc_config_buffer(SRB_INFO *srb_info, uint8_t Lchan_type, uint8_t Role);
void openair_rrc_on(module_id_t module_idP, eNB_flag_t eNB_flagP);
void rrc_top_cleanup(void);

/** \brief Function to update timers every subframe.  For UE it updates T300,T304 and T310.
@param module_idP Instance of UE/eNB
@param frame Frame index
@param eNB_flag Flag to indicate if this instance is and eNB or UE
@param index Index of corresponding eNB (for UE)
*/
RRC_status_t rrc_rx_tx(module_id_t module_idP,frame_t frameP, eNB_flag_t eNB_flagP,uint8_t index,int CC_id);

// UE RRC Procedures

/** \brief Decodes DL-CCCH message and invokes appropriate routine to handle the message
    \param module_idP Instance ID of UE
    \param Srb_info Pointer to SRB_INFO structure (SRB0)
    \param eNB_index Index of corresponding eNB/CH*/
int rrc_ue_decode_ccch(module_id_t module_idP, frame_t frameP, SRB_INFO *Srb_info,uint8_t eNB_index);

/** \brief Decodes a DL-DCCH message and invokes appropriate routine to handle the message
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param buffer_pP Pointer to received SDU
    \param eNB_index Index of corresponding CH/eNB*/
void rrc_ue_decode_dcch(module_id_t module_idP, frame_t frameP, uint8_t Srb_id, uint8_t* buffer_pP,uint8_t eNB_index);

/** \brief Generate/Encodes RRCConnnectionRequest message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionRequest(module_id_t module_idP, frame_t frameP, uint8_t eNB_index);

/** \brief Generates/Encodes RRCConnnectionSetupComplete message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionSetupComplete(module_id_t module_idP,frame_t frameP,uint8_t eNB_index, uint8_t Transaction_id);

/** \brief process the received rrcConnectionReconfiguration message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param *rrcConnectionReconfiguration pointer to the sturcture
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_process_rrcConnectionReconfiguration(module_id_t module_idP, frame_t frameP,RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,uint8_t eNB_index);

/** \brief Generates/Encodes RRCConnectionReconfigurationComplete  message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionReconfigurationComplete(module_id_t module_idP, frame_t frameP, uint8_t eNB_index, uint8_t Transaction_id);

/** \brief Establish SRB1 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
int32_t  rrc_ue_establish_srb1(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish SRB2 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
int32_t  rrc_ue_establish_srb2(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish a DRB according to DRB_ToAddMod structure
    \param module_idP Instance ID of UE
    \param eNB_index Index of corresponding CH/eNB
    \param DRB_config Pointer to DRB_ToAddMod IE from configuration
    @returns 0 on success */
int32_t  rrc_ue_establish_drb(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,struct DRB_ToAddMod *DRB_config);

/** \brief Process MobilityControlInfo Message to proceed with handover and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param mobilityControlInfo Pointer to mobilityControlInfo
*/
void rrc_ue_process_mobilityControlInfo(module_id_t enb_module_idP, module_id_t ue_module_idP, frame_t frameP, struct MobilityControlInfo *mobilityControlInfo);

/** \brief Process a measConfig Message and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param  measConfig Pointer to MeasConfig  IE from configuration*/
void rrc_ue_process_measConfig(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,MeasConfig_t *measConfig);

/** \brief Process a RadioResourceConfigDedicated Message and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param eNB_index Index of corresponding CH/eNB
    \param radioResourceConfigDedicated Pointer to RadioResourceConfigDedicated IE from configuration*/
void rrc_ue_process_radioResourceConfigDedicated(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,
						 RadioResourceConfigDedicated_t *radioResourceConfigDedicated);

// eNB/CH RRC Procedures

/**\brief Function to get the next transaction identifier.
   \param module_idP Instance ID for CH/eNB
   \return a transaction identifier*/
uint8_t rrc_eNB_get_next_transaction_identifier(module_id_t module_idP);

/**\brief Entry routine to decode a UL-CCCH-Message.  Invokes PER decoder and parses message.
   \param module_idP Instance ID for CH/eNB
   \param frame  Frame index
   \param Srb_info Pointer to SRB0 information structure (buffer, etc.)*/
int rrc_eNB_decode_ccch(module_id_t module_idP, frame_t frameP, SRB_INFO *Srb_info);

/**\brief Entry routine to decode a UL-DCCH-Message.  Invokes PER decoder and parses message.
   \param module_idP Instance ID for CH/eNB
   \param frame Frame index
   \param ue_module_idP Index of UE sending the message
   \param Rx_sdu Pointer Received Message
   \param sdu_size Size of incoming SDU*/
int rrc_eNB_decode_dcch(module_id_t module_idP, frame_t frameP, uint8_t Srb_id, module_id_t ue_module_idP, uint8_t *Rx_sdu, sdu_size_t sdu_size);

/**\brief Generate the RRCConnectionSetup based on information coming from RRM
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE receiving the message*/
void rrc_eNB_generate_RRCConnectionSetup(module_id_t module_idP,frame_t frameP, module_id_t ue_module_idP);

/**\brief Process the RRCConnectionSetupComplete based on information coming from UE
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE transmitting the message
   \param rrcConnectionSetupComplete Pointer to RRCConnectionSetupComplete message*/
void rrc_eNB_process_RRCConnectionSetupComplete(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP,
    RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete);

/**\brief Process the RRCConnectionReconfigurationComplete based on information coming from UE
   \param module_idP Instance ID for eNB/CH
   \param ue_module_idP Index of UE transmitting the messages
   \param rrcConnectionReconfigurationComplete Pointer to RRCConnectionReconfigurationComplete message*/
void rrc_eNB_process_RRCConnectionReconfigurationComplete(module_id_t module_idP,frame_t frameP,module_id_t ue_module_idP,RRCConnectionReconfigurationComplete_r8_IEs_t *rrcConnectionReconfigurationComplete);

/**\brief Generate the RRCConnectionRelease 
   \param module_idP Instance ID for eNB
   \param frame Frame index
   \param ue_module_idP Index of UE receiving the message*/
void rrc_eNB_generate_RRCConnectionRelease(module_id_t module_idP,frame_t frameP, ue_id_t ue_idP);

#if defined(ENABLE_ITTI)
/**\brief RRC eNB task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_enb_task(void *args_p);

/**\brief RRC UE task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_ue_task(void *args_p);
#endif

/**\brief Generate/decode the handover RRCConnectionReconfiguration at eNB
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE transmitting the messages*/
void rrc_eNB_generate_RRCConnectionReconfiguration_handover(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP, uint8_t *nas_pdu, uint32_t nas_length);

//L2_interface.c
int8_t mac_rrc_lite_data_req( module_id_t module_idP, frame_t frameP, rb_id_t Srb_id, uint8_t Nb_tb, uint8_t *buffer_pP,eNB_flag_t eNB_flagP, uint8_t eNB_index, uint8_t mbsfn_sync_area);

int8_t mac_rrc_lite_data_ind( module_id_t module_idP,  frame_t frameP, rb_id_t Srb_id, uint8_t *Sdu, sdu_size_t Sdu_len,eNB_flag_t eNB_flagP,uint8_t eNB_index, uint8_t mbsfn_sync_area);

void mac_sync_ind( module_id_t Mod_instP, uint8_t status);

uint8_t rrc_lite_data_req(module_id_t enb_mod_idP,
                     module_id_t      ue_mod_idP,
                     frame_t          frameP,
                     eNB_flag_t       eNB_flagP,
                     rb_id_t          rb_idP,
                     mui_t            muiP,
                     confirm_t        confirmP,
                     sdu_size_t       sdu_sizeP,
                     uint8_t         *Buffer_pP,
                     pdcp_transmission_mode_t   modeP);

void rrc_lite_data_ind(module_id_t eNB_id, module_id_t UE_id, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t Rb_id, sdu_size_t sdu_size,uint8_t *buffer_pP);

void rrc_lite_in_sync_ind(module_id_t module_idP, frame_t frameP, uint16_t eNB_index);

void rrc_lite_out_of_sync_ind(module_id_t module_idP, frame_t frameP, unsigned short eNB_index);

int decode_MCCH_Message(module_id_t module_idP, frame_t frameP, uint8_t eNB_index, uint8_t *Sdu, uint8_t Sdu_len,uint8_t mbsfn_sync_area);

void decode_MBSFNAreaConfiguration(module_id_t module_idP, uint8_t eNB_index, frame_t frameP,uint8_t mbsfn_sync_area);

int decode_BCCH_DLSCH_Message(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,uint8_t *Sdu,uint8_t Sdu_len, uint8_t rsrq, uint8_t rsrp);

int decode_SIB1(module_id_t module_idP,uint8_t eNB_index, uint8_t rsrq, uint8_t rsrp);

int decode_SI(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,uint8_t si_window);

void ue_meas_filtering(module_id_t module_idP,frame_t frameP,uint8_t eNB_index);

void ue_measurement_report_triggering(module_id_t module_idP, frame_t frameP,uint8_t eNB_index);

int mac_get_rrc_lite_status(module_id_t module_idP,eNB_flag_t eNB_flagP,uint8_t index);

void rrc_eNB_generate_UECapabilityEnquiry(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP);

void rrc_eNB_generate_SecurityModeCommand(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP);

void rrc_eNB_process_MeasurementReport(uint8_t module_idP,frame_t frameP, module_id_t ue_module_idP,MeasResults_t *measResults2) ;

void rrc_ue_generate_MeasurementReport(module_id_t eNB_id, module_id_t UE_id, frame_t frameP);

void rrc_eNB_generate_HandoverPreparationInformation (uint8_t module_idP, frame_t frameP, module_id_t ue_module_idP, PhysCellId_t targetPhyId) ;

void check_handovers(uint8_t module_idP, frame_t frameP);

uint8_t check_trigger_meas_event(uint8_t module_idP,frame_t frameP, uint8_t eNB_index, uint8_t ue_cnx_index, uint8_t meas_index,
			    Q_OffsetRange_t ofn, Q_OffsetRange_t ocn, Hysteresis_t hys, 
			    Q_OffsetRange_t ofs, Q_OffsetRange_t ocs, long a3_offset, TimeToTrigger_t ttt);

//void rrc_ue_process_ueCapabilityEnquiry(uint8_t module_idP,uint32_t frame,UECapabilityEnquiry_t *UECapabilityEnquiry,uint8_t eNB_index);
//void rrc_ue_process_securityModeCommand(uint8_t module_idP,uint32_t frame,SecurityModeCommand_t *securityModeCommand,uint8_t eNB_index);

void rrc_eNB_free_UE_index (module_id_t enb_mod_idP, module_id_t ue_mod_idP,int frameP);

long binary_search_int(int elements[], long numElem, int value);

long binary_search_float(float elements[], long numElem, float value);

