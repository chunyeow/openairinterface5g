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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

*******************************************************************************/

/*! \file ra_procedures.c
 * \brief Routines for UE MAC-layer Random-access procedures (36.321) V8.6 2009-03
 * \author R. Knopp and Navid Nikaein
 * \date 2011
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr navid.nikaein@eurecom.fr
 * \note
 * \warning
 */

#include "extern.h"
#include "defs.h"
#include "proto.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "PHY_INTERFACE/defs.h"
#include "PHY_INTERFACE/extern.h"
#include "COMMON/mac_rrc_primitives.h"
#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "UTIL/LOG/log.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"
#ifdef PHY_EMUL
#include "SIMULATION/simulation_defs.h"
#endif

extern inline unsigned int taus(void);

int8_t get_DELTA_PREAMBLE(module_id_t module_idP) {

  uint8_t prachConfigIndex = UE_mac_inst[module_idP].radioResourceConfigCommon->prach_Config.prach_ConfigInfo.prach_ConfigIndex;
  uint8_t preambleformat;

  if (UE_mac_inst[module_idP].tdd_Config) { // TDD
      if (prachConfigIndex < 20)
        preambleformat = 0;
      else if (prachConfigIndex < 30)
        preambleformat = 1;
      else if (prachConfigIndex < 40)
        preambleformat = 2;
      else if (prachConfigIndex < 48)
        preambleformat = 3;
      else
        preambleformat = 4;
  }
  else { // FDD
      preambleformat = prachConfigIndex>>2;
  }
  switch (preambleformat) {
  case 0:
  case 1:
    return(0);
  case 2:
  case 3:
    return(-3);
  case 4:
    return(8);
  default:
    LOG_E(MAC,"[UE %d] ue_procedures.c: FATAL, Illegal preambleformat %d, prachConfigIndex %d\n",
        module_idP,
        preambleformat,prachConfigIndex);
    mac_xface->macphy_exit("MAC get_DELTA_PREAMBLE Illegal preamble format");
    return(0);
  }

}

/// This routine implements Section 5.1.2 (Random Access Resource Selection) from 36.321
void get_prach_resources(module_id_t module_idP,
    uint8_t eNB_index,
    uint8_t t_id,
    uint8_t first_Msg3,
    RACH_ConfigDedicated_t *rach_ConfigDedicated) {

  uint8_t Msg3_size = UE_mac_inst[module_idP].RA_Msg3_size;
  PRACH_RESOURCES_t *prach_resources = &UE_mac_inst[module_idP].RA_prach_resources;
  RACH_ConfigCommon_t *rach_ConfigCommon = NULL;
  uint8_t noGroupB = 0;
  uint8_t f_id = 0,num_prach=0;

  if (UE_mac_inst[module_idP].radioResourceConfigCommon)
    rach_ConfigCommon = &UE_mac_inst[module_idP].radioResourceConfigCommon->rach_ConfigCommon;
  else {
      LOG_E(MAC,"[UE %d] FATAL  radioResourceConfigCommon is NULL !!!\n",module_idP);
      mac_xface->macphy_exit("MAC FATAL  radioResourceConfigCommon is NULL");
  }

  if (rach_ConfigDedicated) {   // This is for network controlled Mobility, later
      if (rach_ConfigDedicated->ra_PRACH_MaskIndex != 0) {
          prach_resources->ra_PreambleIndex = rach_ConfigDedicated->ra_PreambleIndex;
          prach_resources->ra_RACH_MaskIndex = rach_ConfigDedicated->ra_PRACH_MaskIndex;
          return;
      }
  }

  if (!rach_ConfigCommon->preambleInfo.preamblesGroupAConfig) {
      noGroupB = 1;

  }
  else {
      if (rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA ==
          rach_ConfigCommon->preambleInfo.numberOfRA_Preambles)
        noGroupB = 1;
  }

  if (first_Msg3 == 1) {
      if (noGroupB == 1) {
          // use Group A procedure
          UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  = (taus())&0x3f;
          UE_mac_inst[module_idP].RA_prach_resources.ra_RACH_MaskIndex = 0;
          UE_mac_inst[module_idP].RA_usedGroupA = 1;
      }
      else if ((Msg3_size < rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->messageSizeGroupA) ||
          (mac_xface->get_PL(module_idP,eNB_index) > UE_mac_inst[module_idP].RA_maxPL)) {
          // use Group A procedure
          UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  = (taus())%rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA;
          UE_mac_inst[module_idP].RA_prach_resources.ra_RACH_MaskIndex = 0;
          UE_mac_inst[module_idP].RA_usedGroupA = 1;
      }
      else {  // use Group B
          UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  =
              rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA +
              (taus())%(rach_ConfigCommon->preambleInfo.numberOfRA_Preambles -
                  rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA);
          UE_mac_inst[module_idP].RA_prach_resources.ra_RACH_MaskIndex = 0;
          UE_mac_inst[module_idP].RA_usedGroupA = 0;
      }
      UE_mac_inst[module_idP].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER = get_Po_NOMINAL_PUSCH(module_idP);
  }
  else {  // Msg3 is being retransmitted
      if (UE_mac_inst[module_idP].RA_usedGroupA == 1) {
          if (rach_ConfigCommon->preambleInfo.preamblesGroupAConfig)
            UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  = (taus())%rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA;
          else
            UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  = (taus())&0x3f;
          UE_mac_inst[module_idP].RA_prach_resources.ra_RACH_MaskIndex = 0;
      }
      else {
          UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex  =
              rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA +
              (taus())%(rach_ConfigCommon->preambleInfo.numberOfRA_Preambles -
                  rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA);
          UE_mac_inst[module_idP].RA_prach_resources.ra_RACH_MaskIndex = 0;
      }
  }
  // choose random PRACH resource in TDD
  if (UE_mac_inst[module_idP].tdd_Config) {
      num_prach = mac_xface->get_num_prach_tdd(mac_xface->lte_frame_parms);
      if ((num_prach>0) && (num_prach<6))
        UE_mac_inst[module_idP].RA_prach_resources.ra_TDD_map_index = (taus()%num_prach);
      f_id = mac_xface->get_fid_prach_tdd(mac_xface->lte_frame_parms,
          UE_mac_inst[module_idP].RA_prach_resources.ra_TDD_map_index);
  }

  // choose RA-RNTI
  UE_mac_inst[module_idP].RA_prach_resources.ra_RNTI = 1 + t_id + 10*f_id;
}

void Msg1_tx(module_id_t module_idP,frame_t frameP, uint8_t eNB_id) {

  // start contention resolution timer
  UE_mac_inst[module_idP].RA_attempt_number++;
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.opt_enabled) {
      trace_pdu(0, NULL, 0, module_idP, 3, UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex,
          UE_mac_inst[module_idP].subframe, 0, UE_mac_inst[module_idP].RA_attempt_number);
      LOG_D(OPT,"[UE %d][RAPROC] TX MSG1 Frame %d trace pdu for rnti %x  with size %d\n",
          module_idP, frameP, 1, UE_mac_inst[module_idP].RA_Msg3_size);
  }
#endif
}


void Msg3_tx(module_id_t module_idP,frame_t frameP, uint8_t eNB_id) {

  // start contention resolution timer
  LOG_I(MAC,"[UE %d][RAPROC] Frame %d : Msg3_tx: Setting contention resolution timer\n",module_idP,frameP);
  UE_mac_inst[module_idP].RA_contention_resolution_cnt = 0;
  UE_mac_inst[module_idP].RA_contention_resolution_timer_active = 1;

#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.opt_enabled) { // msg3
      trace_pdu(0, &UE_mac_inst[module_idP].CCCH_pdu.payload[0], UE_mac_inst[module_idP].RA_Msg3_size,
          module_idP, 3, UE_mac_inst[module_idP].crnti, UE_mac_inst[module_idP].subframe, 0, 0);
      LOG_D(OPT,"[UE %d][RAPROC] MSG3 Frame %d trace pdu Preamble %d   with size %d\n",
          module_idP, frameP, UE_mac_inst[module_idP].crnti /*UE_mac_inst[module_idP].RA_prach_resources.ra_PreambleIndex*/, UE_mac_inst[module_idP].RA_Msg3_size);
  }
#endif
}


PRACH_RESOURCES_t *ue_get_rach(module_id_t module_idP,frame_t frameP, uint8_t eNB_indexP,sub_frame_t subframeP){


  uint8_t                        Size=0;
  UE_MODE_t                 UE_mode = mac_xface->get_ue_mode(module_idP,eNB_indexP);
  uint8_t                        lcid = CCCH;
  uint16_t                       Size16;
  struct RACH_ConfigCommon *rach_ConfigCommon = (struct RACH_ConfigCommon *)NULL;
  int32_t                       frame_diff=0;
  mac_rlc_status_resp_t     rlc_status;
  uint8_t                        dcch_header_len=0;
  uint16_t                       sdu_lengths[8];
  uint8_t                        ulsch_buff[MAX_ULSCH_PAYLOAD_BYTES];

  if (UE_mode == PRACH) {
      if (UE_mac_inst[module_idP].radioResourceConfigCommon)
        rach_ConfigCommon = &UE_mac_inst[module_idP].radioResourceConfigCommon->rach_ConfigCommon;
      else {
          return(NULL);
      }
      if (Is_rrc_registered == 1) {

          if (UE_mac_inst[module_idP].RA_active == 0) {
              // check if RRC is ready to initiate the RA procedure
              Size = mac_rrc_data_req(module_idP,
                  frameP,
                  CCCH,1,
                  &UE_mac_inst[module_idP].CCCH_pdu.payload[sizeof(SCH_SUBHEADER_SHORT)+1],0,
                  eNB_indexP,
                  0);
              Size16 = (uint16_t)Size;

              //	LOG_D(MAC,"[UE %d] Frame %d: Requested RRCConnectionRequest, got %d bytes\n",module_idP,frameP,Size);
              LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_DATA_REQ (RRCConnectionRequest eNB %d) --->][MAC_UE][MOD %02d][]\n",
                  frameP, module_idP, eNB_indexP, module_idP);
              LOG_D(MAC,"[UE %d] Frame %d: Requested RRCConnectionRequest, got %d bytes\n",module_idP,frameP,Size);

              if (Size>0) {

                  UE_mac_inst[module_idP].RA_active                        = 1;
                  UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER = 1;
                  UE_mac_inst[module_idP].RA_Msg3_size                     = Size+sizeof(SCH_SUBHEADER_SHORT)+sizeof(SCH_SUBHEADER_SHORT);
                  UE_mac_inst[module_idP].RA_prachMaskIndex                = 0;
                  UE_mac_inst[module_idP].RA_prach_resources.Msg3          = UE_mac_inst[module_idP].CCCH_pdu.payload;
                  UE_mac_inst[module_idP].RA_backoff_cnt                   = 0;  // add the backoff condition here if we have it from a previous RA reponse which failed (i.e. backoff indicator)
                  if (rach_ConfigCommon) {
                      UE_mac_inst[module_idP].RA_window_cnt                    = 2+ rach_ConfigCommon->ra_SupervisionInfo.ra_ResponseWindowSize;
                      if (UE_mac_inst[module_idP].RA_window_cnt == 9)
                        UE_mac_inst[module_idP].RA_window_cnt = 10;  // Note: 9 subframe window doesn't exist, after 8 is 10!
                  }
                  else {
                      LOG_D(MAC,"[UE %d] FATAL Frame %d: rach_ConfigCommon is NULL !!!\n",module_idP,frameP);
                      mac_xface->macphy_exit("MAC rach_ConfigCommon is NULL");
                  }
                  UE_mac_inst[module_idP].RA_tx_frame         = frameP;
                  UE_mac_inst[module_idP].RA_tx_subframe      = subframeP;
                  UE_mac_inst[module_idP].RA_backoff_frame    = frameP;
                  UE_mac_inst[module_idP].RA_backoff_subframe = subframeP;
                  // Fill in preamble and PRACH resource
                  get_prach_resources(module_idP,eNB_indexP,subframeP,1,NULL);

                  generate_ulsch_header((uint8_t*)&UE_mac_inst[module_idP].CCCH_pdu.payload[0],  // mac header
                      1,      // num sdus
                      0,            // short pading
                      &Size16,  // sdu length
                      &lcid,    // sdu lcid
                      NULL,  // power headroom
                      NULL,  // crnti
                      NULL,  // truncated bsr
                      NULL, // short bsr
                      NULL, // long_bsr
                      1); //post_padding

                  return(&UE_mac_inst[module_idP].RA_prach_resources);
              }
              else if (UE_mac_inst[module_idP].scheduling_info.BSR_bytes[DCCH] > 0) {
                  // This is for triggering a transmission on DCCH using PRACH (during handover, or sending SR for example)
                  dcch_header_len = 2 + 2;  /// SHORT Subheader + C-RNTI control element
                  rlc_status = mac_rlc_status_ind(0, module_idP,frameP,ENB_FLAG_NO,MBMS_FLAG_NO,
                      DCCH,
                      6);
                  if (UE_mac_inst[module_idP].crnti_before_ho)
                    LOG_D(MAC,"[UE %d] Frame %d : UL-DCCH -> ULSCH, HO RRCConnectionReconfigurationComplete (%x, %x), RRC message has %d bytes to send throug PRACH (mac header len %d)\n",
                        module_idP,frameP, UE_mac_inst[module_idP].crnti,UE_mac_inst[module_idP].crnti_before_ho, rlc_status.bytes_in_buffer,dcch_header_len);
                  else
                    LOG_D(MAC,"[UE %d] Frame %d : UL-DCCH -> ULSCH, RRC message has %d bytes to send through PRACH(mac header len %d)\n",
                        module_idP,frameP, rlc_status.bytes_in_buffer,dcch_header_len);

                  sdu_lengths[0] = mac_rlc_data_req(eNB_indexP, module_idP,frameP,ENB_FLAG_NO, MBMS_FLAG_NO,
                      DCCH,
                      (char *)&ulsch_buff[0]);

                  LOG_D(MAC,"[UE %d] TX Got %d bytes for DCCH\n",module_idP,sdu_lengths[0]);
                  update_bsr(module_idP, frameP, DCCH,UE_mac_inst[module_idP].scheduling_info.LCGID[DCCH]);
                  //header_len +=2;
                  UE_mac_inst[module_idP].RA_active                        = 1;
                  UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER = 1;
                  UE_mac_inst[module_idP].RA_Msg3_size                     = Size+dcch_header_len;
                  UE_mac_inst[module_idP].RA_prachMaskIndex                = 0;
                  UE_mac_inst[module_idP].RA_prach_resources.Msg3          = ulsch_buff;
                  UE_mac_inst[module_idP].RA_backoff_cnt                   = 0;  // add the backoff condition here if we have it from a previous RA reponse which failed (i.e. backoff indicator)
                  if (rach_ConfigCommon) {
                      UE_mac_inst[module_idP].RA_window_cnt                    = 2+ rach_ConfigCommon->ra_SupervisionInfo.ra_ResponseWindowSize;
                      if (UE_mac_inst[module_idP].RA_window_cnt == 9)
                        UE_mac_inst[module_idP].RA_window_cnt = 10;  // Note: 9 subframe window doesn't exist, after 8 is 10!
                  }
                  else {
                      LOG_D(MAC,"[UE %d] FATAL Frame %d: rach_ConfigCommon is NULL !!!\n",module_idP,frameP);
                      mac_xface->macphy_exit("MAC rach_ConfigCommon is NULL");
                  }
                  UE_mac_inst[module_idP].RA_tx_frame         = frameP;
                  UE_mac_inst[module_idP].RA_tx_subframe      = subframeP;
                  UE_mac_inst[module_idP].RA_backoff_frame    = frameP;
                  UE_mac_inst[module_idP].RA_backoff_subframe = subframeP;
                  // Fill in preamble and PRACH resource
                  get_prach_resources(module_idP,eNB_indexP,subframeP,1,NULL);
                  generate_ulsch_header((uint8_t*)ulsch_buff,  // mac header
                      1,      // num sdus
                      0,            // short pading
                      &Size16,  // sdu length
                      &lcid,    // sdu lcid
                      NULL,  // power headroom
                      &UE_mac_inst[module_idP].crnti,  // crnti
                      NULL,  // truncated bsr
                      NULL, // short bsr
                      NULL, // long_bsr
                      0); //post_padding

                  return(&UE_mac_inst[module_idP].RA_prach_resources);
              }
          }
          else {  // RACH is active
              LOG_D(MAC,"[MAC][UE %d][RAPROC] frameP %d, subframe %d: RA Active, window cnt %d (RA_tx_frame %d, RA_tx_subframe %d)\n",module_idP,
                  frameP,subframeP,UE_mac_inst[module_idP].RA_window_cnt,
                  UE_mac_inst[module_idP].RA_tx_frame,UE_mac_inst[module_idP].RA_tx_subframe);
              // compute backoff parameters
              if (UE_mac_inst[module_idP].RA_backoff_cnt>0) {
                  frame_diff = (sframe_t)frameP - UE_mac_inst[module_idP].RA_backoff_frame;
                  if (frame_diff < 0)
                    frame_diff = -frame_diff;
                  UE_mac_inst[module_idP].RA_backoff_cnt -= ((10*frame_diff) + (subframeP-UE_mac_inst[module_idP].RA_backoff_subframe));

                  UE_mac_inst[module_idP].RA_backoff_frame    = frameP;
                  UE_mac_inst[module_idP].RA_backoff_subframe = subframeP;
              }
              // compute RA window parameters
              if (UE_mac_inst[module_idP].RA_window_cnt>0) {
                  frame_diff = (frame_t)frameP - UE_mac_inst[module_idP].RA_tx_frame;
                  if (frame_diff < 0)
                    frame_diff = -frame_diff;
                  UE_mac_inst[module_idP].RA_window_cnt -= ((10*frame_diff) + (subframeP-UE_mac_inst[module_idP].RA_tx_subframe));
                  LOG_D(MAC,"[MAC][UE %d][RAPROC] frameP %d, subframe %d: RA Active, adjusted window cnt %d\n",module_idP,
                      frameP,subframeP,UE_mac_inst[module_idP].RA_window_cnt);
              }
              if ((UE_mac_inst[module_idP].RA_window_cnt<=0) &&
                  (UE_mac_inst[module_idP].RA_backoff_cnt<=0)) {

                  UE_mac_inst[module_idP].RA_tx_frame    = frameP;
                  UE_mac_inst[module_idP].RA_tx_subframe = subframeP;
                  UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER++;
                  UE_mac_inst[module_idP].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER += (rach_ConfigCommon->powerRampingParameters.powerRampingStep<<1);  // 2dB increments in ASN.1 definition
                  if (UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER == rach_ConfigCommon->ra_SupervisionInfo.preambleTransMax) {
                      LOG_D(MAC,"[UE %d] Frame %d: Maximum number of RACH attempts (%d)\n",module_idP,frameP,rach_ConfigCommon->ra_SupervisionInfo.preambleTransMax);
                      // send message to RRC
                      UE_mac_inst[module_idP].RA_PREAMBLE_TRANSMISSION_COUNTER=1;
                      UE_mac_inst[module_idP].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER = get_Po_NOMINAL_PUSCH(module_idP);
                  }
                  UE_mac_inst[module_idP].RA_window_cnt                    = 2+ rach_ConfigCommon->ra_SupervisionInfo.ra_ResponseWindowSize;
                  UE_mac_inst[module_idP].RA_backoff_cnt                   = 0;

                  // Fill in preamble and PRACH resource
                  get_prach_resources(module_idP,eNB_indexP,subframeP,0,NULL);
                  return(&UE_mac_inst[module_idP].RA_prach_resources);
              }
          }
      }
  }
  else if (UE_mode == PUSCH) {
      LOG_D(MAC,"[UE %d] FATAL: Should not have checked for RACH in PUSCH yet ...",module_idP);
      mac_xface->macphy_exit("MAC FATAL: Should not have checked for RACH in PUSCH yet");
  }
  return(NULL);
}

void cancel_ra_proc(module_id_t module_idP, frame_t frameP, rnti_t rnti) {
  unsigned char i;
  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Cancelling RA procedure for UE rnti %x\n",module_idP,frameP,rnti);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
      if (rnti == eNB_mac_inst[module_idP].RA_template[i].rnti) {
          eNB_mac_inst[module_idP].RA_template[i].RA_active=FALSE;
          eNB_mac_inst[module_idP].RA_template[i].generate_rar=0;
          eNB_mac_inst[module_idP].RA_template[i].generate_Msg4=0;
          eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4=0;
          eNB_mac_inst[module_idP].RA_template[i].timing_offset=0;
          eNB_mac_inst[module_idP].RA_template[i].RRC_timer=20;
          eNB_mac_inst[module_idP].RA_template[i].rnti = 0;
      }
  }
}

void terminate_ra_proc(module_id_t module_idP,frame_t frameP,rnti_t rnti,unsigned char *msg3, uint16_t msg3_len) {

  unsigned char rx_ces[MAX_NUM_CE],num_ce,num_sdu,i,*payload_ptr;
  unsigned char rx_lcids[NB_RB_MAX];
  uint16_t rx_lengths[NB_RB_MAX];
  int8_t UE_id;

  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, Received msg3 %x.%x.%x.%x.%x.%x, Terminating RA procedure for UE rnti %x\n",
      module_idP,frameP,
      msg3[3],msg3[4],msg3[5],msg3[6],msg3[7], msg3[8], rnti);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
      LOG_D(MAC,"[RAPROC] Checking proc %d : rnti (%x, %x), active %d\n",i,
          eNB_mac_inst[module_idP].RA_template[i].rnti, rnti,
          eNB_mac_inst[module_idP].RA_template[i].RA_active);
      if ((eNB_mac_inst[module_idP].RA_template[i].rnti==rnti) &&
          (eNB_mac_inst[module_idP].RA_template[i].RA_active==TRUE)) {

          payload_ptr = parse_ulsch_header(msg3,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,msg3_len);
          LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Received CCCH: length %d, offset %d\n",
              module_idP,frameP,rx_lengths[0],payload_ptr-msg3);
          if (/*(num_ce == 0) &&*/ (num_sdu==1) && (rx_lcids[0] == CCCH)) { // This is an RRCConnectionRequest/Restablishment
              memcpy(&eNB_mac_inst[module_idP].RA_template[i].cont_res_id[0],payload_ptr,6);
              LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Received CCCH: length %d, offset %d\n",
                  module_idP,frameP,rx_lengths[0],payload_ptr-msg3);
              UE_id=add_new_ue(module_idP,eNB_mac_inst[module_idP].RA_template[i].rnti);
              if (UE_id==-1) {
                  mac_xface->macphy_exit("[MAC][eNB] Max user count reached\n");
              }
              else {
                  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Added user with rnti %x => UE %d\n",
                      module_idP,frameP,eNB_mac_inst[module_idP].RA_template[i].rnti,UE_id);
              }

              if (Is_rrc_registered == 1)
                mac_rrc_data_ind(module_idP,frameP,CCCH,(uint8_t *)payload_ptr,rx_lengths[0],1,module_idP,0);
              // add_user.  This is needed to have the rnti for configuring UE (PHY). The UE is removed if RRC
              // doesn't provide a CCCH SDU

          }
          else if (num_ce >0) {  // handle msg3 which is not RRCConnectionRequest
              //	process_ra_message(msg3,num_ce,rx_lcids,rx_ces);
          }

          eNB_mac_inst[module_idP].RA_template[i].generate_Msg4 = 1;
          eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4 = 0;

          return;
      } // if process is active

  } // loop on RA processes
}

void rx_sdu(module_id_t enb_mod_idP,frame_t frameP,rnti_t rntiP,uint8_t *sdu, uint16_t sdu_len) {

  unsigned char  rx_ces[MAX_NUM_CE],num_ce,num_sdu,i,*payload_ptr;
  unsigned char  rx_lcids[NB_RB_MAX];
  unsigned short rx_lengths[NB_RB_MAX];
  module_id_t    ue_mod_id = find_UE_id(enb_mod_idP,rntiP);
  int ii,j;
  start_meas(&eNB_mac_inst[enb_mod_idP].rx_ulsch_sdu);
  
  if ((ue_mod_id >  NUMBER_OF_UE_MAX) || (ue_mod_id == -1) || (ue_mod_id == 255) )
  
  for(ii=0; ii<NB_RB_MAX; ii++) rx_lengths[ii] = 0;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,1);

  LOG_D(MAC,"[eNB %d] Received ULSCH sdu from PHY (rnti %x, UE_id %d), parsing header\n",enb_mod_idP,rntiP,ue_mod_id);
  payload_ptr = parse_ulsch_header(sdu,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,sdu_len);

  // control element
  for (i=0;i<num_ce;i++) {

    switch (rx_ces[i]) { // implement and process BSR + CRNTI +
    case POWER_HEADROOM:
      if (ue_mod_id != UE_INDEX_INVALID ){
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].phr_info =  (payload_ptr[0] & 0x3f);// - PHR_MAPPING_OFFSET;
	LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received PHR PH = %d (db)\n", rx_ces[i], eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].phr_info);
      }
      payload_ptr+=sizeof(POWER_HEADROOM_CMD);
      break;
      case CRNTI:
        LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received CRNTI %d \n", rx_ces[i], payload_ptr[0]);
        payload_ptr+=1;
        break;
    case TRUNCATED_BSR:
    case SHORT_BSR: {
      if (ue_mod_id  != UE_INDEX_INVALID ){
	uint8_t lcgid;
	lcgid = (payload_ptr[0] >> 6);
	LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received short BSR LCGID = %u bsr = %d\n",
	      rx_ces[i], lcgid, payload_ptr[0] & 0x3f);
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[lcgid] = (payload_ptr[0] & 0x3f);
      }
      payload_ptr += 1;//sizeof(SHORT_BSR); // fixme
    } break;
    case LONG_BSR:
      if (ue_mod_id  != UE_INDEX_INVALID ){
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID0] = ((payload_ptr[0] & 0xFC) >> 2);
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID1] =
	  ((payload_ptr[0] & 0x03) << 4) | ((payload_ptr[1] & 0xF0) >> 4);
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID2] =
	  ((payload_ptr[1] & 0x0F) << 2) | ((payload_ptr[2] & 0xC0) >> 6);
	eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID3] = (payload_ptr[2] & 0x3F);
	LOG_D(MAC, "[eNB] MAC CE_LCID %d: Received long BSR LCGID0 = %u LCGID1 = "
	      "%u LCGID2 = %u LCGID3 = %u\n",
	      rx_ces[i],
	      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID0],
	      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID1],
	      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID2],
	      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].bsr_info[LCGID3]);
      }
      payload_ptr += 3;////sizeof(LONG_BSR);
      break;
    default:
      LOG_E(MAC, "[eNB] Received unknown MAC header (0x%02x)\n", rx_ces[i]);
      break;
    }
  }
  
  for (i=0;i<num_sdu;i++) {
    LOG_D(MAC,"SDU Number %d MAC Subheader SDU_LCID %d, length %d\n",i,rx_lcids[i],rx_lengths[i]);
    
    switch (rx_lcids[i]) {
    case CCCH : 
      LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, Received CCCH:  %x.%x.%x.%x.%x.%x, Terminating RA procedure for UE rnti %x\n",
	    enb_mod_idP,frameP,
	    payload_ptr[0],payload_ptr[1],payload_ptr[2],payload_ptr[3],payload_ptr[4], payload_ptr[5], rntiP);

      for (ii=0;ii<NB_RA_PROC_MAX;ii++) {
	LOG_D(MAC,"[RAPROC] Checking proc %d : rnti (%x, %x), active %d\n",ii,
	      eNB_mac_inst[enb_mod_idP].RA_template[ii].rnti, rntiP,
	      eNB_mac_inst[enb_mod_idP].RA_template[ii].RA_active);
	
	if ((eNB_mac_inst[enb_mod_idP].RA_template[ii].rnti==rntiP) &&
	    (eNB_mac_inst[enb_mod_idP].RA_template[ii].RA_active==TRUE)) {
	  
          //payload_ptr = parse_ulsch_header(msg3,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,msg3_len);
	  
	  if (ue_mod_id == UE_INDEX_INVALID) {
	    memcpy(&eNB_mac_inst[enb_mod_idP].RA_template[ii].cont_res_id[0],payload_ptr,6);
	    LOG_I(MAC,"[eNB %d][RAPROC] Frame %d CCCH: Received RRCConnectionRequest: length %d, offset %d\n",
                  enb_mod_idP,frameP,rx_lengths[ii],payload_ptr-sdu);
	    if ((ue_mod_id=add_new_ue(enb_mod_idP,eNB_mac_inst[enb_mod_idP].RA_template[ii].rnti)) == -1 )
	      mac_xface->macphy_exit("[MAC][eNB] Max user count reached\n");
	    else 
	      LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Added user with rnti %x => UE %d\n",
		    enb_mod_idP,frameP,eNB_mac_inst[enb_mod_idP].RA_template[ii].rnti,ue_mod_id);
	  } else {
	     LOG_I(MAC,"[eNB %d][RAPROC] Frame %d CCCH: Received RRCConnectionReestablishment from UE %d: length %d, offset %d\n",
		   enb_mod_idP,frameP,ue_mod_id,rx_lengths[ii],payload_ptr-sdu);
	  }
	  
	  if (Is_rrc_registered == 1)
	    mac_rrc_data_ind(enb_mod_idP,frameP,CCCH,(uint8_t *)payload_ptr,rx_lengths[ii],1,enb_mod_idP,0);
	  
	  
          if (num_ce >0) {  // handle msg3 which is not RRCConnectionRequest
	    //	process_ra_message(msg3,num_ce,rx_lcids,rx_ces);
	  }
	  
	  eNB_mac_inst[enb_mod_idP].RA_template[ii].generate_Msg4 = 1;
	  eNB_mac_inst[enb_mod_idP].RA_template[ii].wait_ack_Msg4 = 0;
	  
	  
	} // if process is active
	
      } // loop on RA processes
      
      break;
    case  DCCH : 
    case DCCH1 :
      //      if(eNB_mac_inst[module_idP].Dcch_lchan[UE_id].Active==1){
      
#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
      for (j=0;j<32;j++)
	LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n");
#endif
      
      //  This check is just to make sure we didn't get a bogus SDU length, to be removed ...
      if (rx_lengths[i]<CCCH_PAYLOAD_SIZE_MAX) {
	LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DCCH, received %d bytes form UE %d on LCID %d(%d) \n",
	      enb_mod_idP,frameP, rx_lengths[i], ue_mod_id, rx_lcids[i], rx_lcids[i]);
	
	mac_rlc_data_ind(enb_mod_idP,ue_mod_id, frameP,ENB_FLAG_YES,MBMS_FLAG_NO,
			 rx_lcids[i],
			 (char *)payload_ptr,
			 rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].num_pdu_rx[rx_lcids[i]]+=1;
	eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];
	
      }
      //      }
      break;
    case DTCH: // default DRB 
      //      if(eNB_mac_inst[module_idP].Dcch_lchan[UE_id].Active==1){
	
#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
      for (j=0;j<32;j++)
	LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n");
#endif
      
      LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DTCH, received %d bytes from UE %d for lcid %d (%d)\n",
	    enb_mod_idP,frameP, rx_lengths[i], ue_mod_id,rx_lcids[i],rx_lcids[i]);
      
      if ((rx_lengths[i] <SCH_PAYLOAD_SIZE_MAX) &&  (rx_lengths[i] > 0) ) {   // MAX SIZE OF transport block
	mac_rlc_data_ind(enb_mod_idP,ue_mod_id, frameP,ENB_FLAG_YES,MBMS_FLAG_NO,
			 DTCH,
			 (char *)payload_ptr,
			   rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].num_pdu_rx[rx_lcids[i]]+=1;
	eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];
	  
      }
	//      }
      break;
    default :  //if (rx_lcids[i] >= DTCH) {
      eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].num_errors_rx+=1;
      LOG_E(MAC,"[eNB %d] received unsupported or unknown LCID %d from UE %d ", rx_lcids[i], ue_mod_id);
      break;
    }
    payload_ptr+=rx_lengths[i];
    
  }

  eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].total_pdu_bytes_rx+=sdu_len;
  eNB_mac_inst[enb_mod_idP].eNB_UE_stats[ue_mod_id].total_num_pdus_rx+=1;
  
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,0);
  stop_meas(&eNB_mac_inst[enb_mod_idP].rx_ulsch_sdu);
}

// First stage of Random-Access Scheduling
void schedule_RA(module_id_t module_idP,frame_t frameP, sub_frame_t subframeP,unsigned char Msg3_subframe,unsigned char *nprb,unsigned int *nCCE) {

  start_meas(&eNB_mac_inst[module_idP].schedule_ra);
  RA_TEMPLATE *RA_template = (RA_TEMPLATE *)&eNB_mac_inst[module_idP].RA_template[0];
  unsigned char i;//,harq_pid,round;
  uint16_t rrc_sdu_length;
  unsigned char lcid,offset;
  module_id_t UE_id= UE_INDEX_INVALID;
  unsigned short TBsize = -1;
  unsigned short msg4_padding,msg4_post_padding,msg4_header;

  for (i=0;i<NB_RA_PROC_MAX;i++) {

      if (RA_template[i].RA_active == TRUE) {

          LOG_I(MAC,"[eNB %d][RAPROC] RA %d is active (generate RAR %d, generate_Msg4 %d, wait_ack_Msg4 %d, rnti %x)\n",
              module_idP,i,RA_template[i].generate_rar,RA_template[i].generate_Msg4,RA_template[i].wait_ack_Msg4, RA_template[i].rnti);

          if (RA_template[i].generate_rar == 1) {
              *nprb= (*nprb) + 3;
              *nCCE = (*nCCE) + 4;
              RA_template[i].Msg3_subframe=Msg3_subframe;
          }
          else if (RA_template[i].generate_Msg4 == 1) {

              // check for Msg4 Message
              UE_id = find_UE_id(module_idP,RA_template[i].rnti);
              if (Is_rrc_registered == 1) {

                  // Get RRCConnectionSetup for Piggyback
                  rrc_sdu_length = mac_rrc_data_req(module_idP,
                      frameP,
                      CCCH,1,
                      &eNB_mac_inst[module_idP].CCCH_pdu.payload[0],
                      1,
                      module_idP,
                      0); // not used in this case
                  if (rrc_sdu_length == -1)
                    mac_xface->macphy_exit("[MAC][eNB Scheduler] CCCH not allocated\n");
                  else {
                      //msg("[MAC][eNB %d] Frame %d, subframeP %d: got %d bytes from RRC\n",module_idP,frameP, subframeP,rrc_sdu_length);
                  }
              }

              LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: UE_id %d, Is_rrc_registered %d, rrc_sdu_length %d\n",
                  module_idP,frameP, subframeP,UE_id, Is_rrc_registered,rrc_sdu_length);

              if (rrc_sdu_length>0) {
                  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: Generating Msg4 with RRC Piggyback (RA proc %d, RNTI %x)\n",
                      module_idP,frameP, subframeP,i,RA_template[i].rnti);

                  //msg("[MAC][eNB %d][RAPROC] Frame %d, subframeP %d: Received %d bytes for Msg4: \n",module_idP,frameP,subframeP,rrc_sdu_length);
                  //	  for (j=0;j<rrc_sdu_length;j++)
                  //	    msg("%x ",(unsigned char)eNB_mac_inst[module_idP].CCCH_pdu.payload[j]);
                  //	  msg("\n");
                  //	  msg("[MAC][eNB] Frame %d, subframeP %d: Generated DLSCH (Msg4) DCI, format 1A, for UE %d\n",frameP, subframeP,UE_id);
                  // Schedule Reflection of Connection request



                  // Compute MCS for 3 PRB
                  msg4_header = 1+6+1;  // CR header, CR CE, SDU header

                  if (mac_xface->lte_frame_parms->frame_type == TDD) {

                      switch (mac_xface->lte_frame_parms->N_RB_DL) {
                      case 6:
                        ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_1_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 25:

                        ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_5MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 50:

                        ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_10MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 100:

                        ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_20MHz_TDD_1_6_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      }
                  }
                  else { // FDD DCI
                      switch (mac_xface->lte_frame_parms->N_RB_DL) {
                      case 6:
                        ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_1_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 25:
                        ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 50:
                        ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_10MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_5MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      case 100:
                        ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->ndi=1;

                        if ((rrc_sdu_length+msg4_header) <= 22) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=4;
                            TBsize = 22;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 28) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=5;
                            TBsize = 28;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 32) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=6;
                            TBsize = 32;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 41) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=7;
                            TBsize = 41;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 49) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=8;
                            TBsize = 49;
                        }
                        else if ((rrc_sdu_length+msg4_header) <= 57) {
                            ((DCI1A_20MHz_FDD_t*)&RA_template[i].RA_alloc_pdu2[0])->mcs=9;
                            TBsize = 57;
                        }
                        break;
                      }
                  }
                  RA_template[i].generate_Msg4=0;
                  RA_template[i].generate_Msg4_dci=1;
                  RA_template[i].wait_ack_Msg4=1;
                  RA_template[i].RA_active = FALSE;
                  lcid=0;

                  if ((TBsize - rrc_sdu_length - msg4_header) <= 2) {
                      msg4_padding = TBsize - rrc_sdu_length - msg4_header;
                      msg4_post_padding = 0;
                  }
                  else {
                      msg4_padding = 0;
                      msg4_post_padding = TBsize - rrc_sdu_length - msg4_header -1;
                  }
                  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d subframeP %d Msg4 : TBS %d, sdu_len %d, msg4_header %d, msg4_padding %d, msg4_post_padding %d\n",
                      module_idP,frameP,subframeP,TBsize,rrc_sdu_length,msg4_header,msg4_padding,msg4_post_padding);
                  offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[module_idP].DLSCH_pdu[(unsigned char)UE_id][0].payload[0],
                      1,                           //num_sdus
                      &rrc_sdu_length,             //
                      &lcid,                       // sdu_lcid
                      255,                         // no drx
                      0,                           // no timing advance
                      RA_template[i].cont_res_id,  // contention res id
                      msg4_padding,                // no padding
                      msg4_post_padding);

                  memcpy((void*)&eNB_mac_inst[module_idP].DLSCH_pdu[(unsigned char)UE_id][0].payload[0][(unsigned char)offset],
                      &eNB_mac_inst[module_idP].CCCH_pdu.payload[0],
                      rrc_sdu_length);

#if defined(USER_MODE) && defined(OAI_EMU)
                  if (oai_emulation.info.opt_enabled){
                      trace_pdu(1, (uint8_t *)eNB_mac_inst[module_idP].DLSCH_pdu[(unsigned char)UE_id][0].payload[0],
                          rrc_sdu_length, UE_id, 3, find_UE_RNTI(module_idP, UE_id),
                          eNB_mac_inst[module_idP].subframe,0,0);
                      LOG_D(OPT,"[eNB %d][DLSCH] Frame %d trace pdu for rnti %x with size %d\n",
                          module_idP, frameP, find_UE_RNTI(module_idP,UE_id), rrc_sdu_length);
                  }
#endif
                  *nprb= (*nprb) + 3;
                  *nCCE = (*nCCE) + 4;
              }
              //try here
          }
          /*
	else if (eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4==1) {
	// check HARQ status and retransmit if necessary
	LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: Checking if Msg4 was acknowledged :\n",module_idP,frameP,subframeP);
	// Get candidate harq_pid from PHY
	mac_xface->get_ue_active_harq_pid(module_idP,eNB_mac_inst[module_idP].RA_template[i].rnti,subframeP,&harq_pid,&round,0);
	if (round>0) {
           *nprb= (*nprb) + 3;
           *nCCE = (*nCCE) + 4;
	}
	}
           */
      }
  }
  stop_meas(&eNB_mac_inst[module_idP].schedule_ra);
}

void initiate_ra_proc(module_id_t module_idP, frame_t frameP, uint16_t preamble_index,int16_t timing_offset,uint8_t sect_id,sub_frame_t subframeP,uint8_t f_id) {

  uint8_t i;

  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Initiating RA procedure for preamble index %d\n",module_idP,frameP,preamble_index);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
      if (eNB_mac_inst[module_idP].RA_template[i].RA_active==FALSE) {
          eNB_mac_inst[module_idP].RA_template[i].RA_active=TRUE;
          eNB_mac_inst[module_idP].RA_template[i].generate_rar=1;
          eNB_mac_inst[module_idP].RA_template[i].generate_Msg4=0;
          eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4=0;
          eNB_mac_inst[module_idP].RA_template[i].timing_offset=timing_offset;
          // Put in random rnti (to be replaced with proper procedure!!)
          eNB_mac_inst[module_idP].RA_template[i].rnti = taus();
          eNB_mac_inst[module_idP].RA_template[i].RA_rnti = 1+subframeP+(10*f_id);
          eNB_mac_inst[module_idP].RA_template[i].preamble_index = preamble_index;
          LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Activating RAR generation for process %d, rnti %x, RA_active %d\n",
              module_idP,frameP,i,eNB_mac_inst[module_idP].RA_template[i].rnti,
              eNB_mac_inst[module_idP].RA_template[i].RA_active);

          return;
      }
  }
}
