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

/*! \file openair2/LAYER2/MAC/ra_procedures.c
* \brief Routines for UE MAC-layer Random-access procedures (36.321) V8.6 2009-03
* \author R. Knopp
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr
* \note
* \warning
*/

#include "extern.h"
#include "defs.h"
#include "PHY_INTERFACE/defs.h"
#include "PHY_INTERFACE/extern.h"
#include "COMMON/mac_rrc_primitives.h"
#include "RRC/LITE/extern.h"
#include "UTIL/LOG/log.h"
#include "OCG.h"
#include "OCG_extern.h"
#ifdef PHY_EMUL
#include "SIMULATION/simulation_defs.h"
#endif

extern inline unsigned int taus(void);

s8 get_DELTA_PREAMBLE(u8 Mod_id) {

  u8 prachConfigIndex = UE_mac_inst[Mod_id].radioResourceConfigCommon->prach_Config.prach_ConfigInfo.prach_ConfigIndex;
  u8 preambleformat;

  if (UE_mac_inst[Mod_id].tdd_Config) { // TDD
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
	Mod_id,
	preambleformat,prachConfigIndex);
    mac_xface->macphy_exit("");
    return(0);
  }

}

/// This routine implements Section 5.1.2 (Random Access Resource Selection) from 36.321
void get_prach_resources(u8 Mod_id,
			 u8 eNB_index,
			 u8 t_id,
			 u8 first_Msg3,
			 RACH_ConfigDedicated_t *rach_ConfigDedicated) {

  u8 Msg3_size = UE_mac_inst[Mod_id].RA_Msg3_size;
  PRACH_RESOURCES_t *prach_resources = &UE_mac_inst[Mod_id].RA_prach_resources;
  RACH_ConfigCommon_t *rach_ConfigCommon = NULL;
  u8 noGroupB = 0;
  u8 f_id = 0,num_prach=0;

  if (UE_mac_inst[Mod_id].radioResourceConfigCommon)
    rach_ConfigCommon = &UE_mac_inst[Mod_id].radioResourceConfigCommon->rach_ConfigCommon;
  else {
    LOG_E(MAC,"[UE %d] FATAL  radioResourceConfigCommon is NULL !!!\n",Mod_id);
    mac_xface->macphy_exit("");
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
      UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  = (taus())&0x3f;
      UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
      UE_mac_inst[Mod_id].RA_usedGroupA = 1;
    }
    else if ((Msg3_size < rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->messageSizeGroupA) ||
	     (mac_xface->get_PL(Mod_id,eNB_index) > UE_mac_inst[Mod_id].RA_maxPL)) {
      // use Group A procedure
      UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  = (taus())%rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA;
      UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
      UE_mac_inst[Mod_id].RA_usedGroupA = 1;
    }
    else {  // use Group B
      UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  =
	rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA +
	(taus())%(rach_ConfigCommon->preambleInfo.numberOfRA_Preambles -
		  rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA);
      UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
      UE_mac_inst[Mod_id].RA_usedGroupA = 0;
    }
    UE_mac_inst[Mod_id].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER = get_Po_NOMINAL_PUSCH(Mod_id);
  }
  else {  // Msg3 is being retransmitted
    if (UE_mac_inst[Mod_id].RA_usedGroupA == 1) {
      if (rach_ConfigCommon->preambleInfo.preamblesGroupAConfig)
	UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  = (taus())%rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA;
      else
	UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  = (taus())&0x3f;
      UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
    }
    else {
      UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex  =
	rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA +
	(taus())%(rach_ConfigCommon->preambleInfo.numberOfRA_Preambles -
		  rach_ConfigCommon->preambleInfo.preamblesGroupAConfig->sizeOfRA_PreamblesGroupA);
      UE_mac_inst[Mod_id].RA_prach_resources.ra_RACH_MaskIndex = 0;
    }
  }
  // choose random PRACH resource in TDD
  if (UE_mac_inst[Mod_id].tdd_Config) {
    num_prach = mac_xface->get_num_prach_tdd(mac_xface->lte_frame_parms);
    if ((num_prach>0) && (num_prach<6))
      UE_mac_inst[Mod_id].RA_prach_resources.ra_TDD_map_index = (taus()%num_prach);
    f_id = mac_xface->get_fid_prach_tdd(mac_xface->lte_frame_parms,
				       UE_mac_inst[Mod_id].RA_prach_resources.ra_TDD_map_index);
  }

  // choose RA-RNTI
  UE_mac_inst[Mod_id].RA_prach_resources.ra_RNTI = 1 + t_id + 10*f_id;
}

void Msg1_tx(u8 Mod_id,u32 frame, u8 eNB_id) {

  // start contention resolution timer
   UE_mac_inst[Mod_id].RA_attempt_number++;
#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.opt_enabled) {
    trace_pdu(0, NULL, 0, Mod_id, 3, UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex,
              UE_mac_inst[Mod_id].subframe, 0, UE_mac_inst[Mod_id].RA_attempt_number);
    LOG_D(OPT,"[UE %d][RAPROC] TX MSG1 Frame %d trace pdu for rnti %x  with size %d\n", 
	  Mod_id, frame, 1, UE_mac_inst[Mod_id].RA_Msg3_size);
  }
#endif
}


void Msg3_tx(u8 Mod_id,u32 frame, u8 eNB_id) {

  // start contention resolution timer
  LOG_I(MAC,"[UE %d][RAPROC] Frame %d : Msg3_tx: Setting contention resolution timer\n",Mod_id,frame);
  UE_mac_inst[Mod_id].RA_contention_resolution_cnt = 0;
  UE_mac_inst[Mod_id].RA_contention_resolution_timer_active = 1;

#if defined(USER_MODE) && defined(OAI_EMU)
  if (oai_emulation.info.opt_enabled) { // msg3
    trace_pdu(0, &UE_mac_inst[Mod_id].CCCH_pdu.payload[0], UE_mac_inst[Mod_id].RA_Msg3_size,
              Mod_id, 3, UE_mac_inst[Mod_id].crnti, UE_mac_inst[Mod_id].subframe, 0, 0);
    LOG_D(OPT,"[UE %d][RAPROC] MSG3 Frame %d trace pdu Preamble %d   with size %d\n", 
          Mod_id, frame, UE_mac_inst[Mod_id].crnti /*UE_mac_inst[Mod_id].RA_prach_resources.ra_PreambleIndex*/, UE_mac_inst[Mod_id].RA_Msg3_size);
  }
#endif
}


PRACH_RESOURCES_t *ue_get_rach(u8 Mod_id,u32 frame, u8 eNB_index,u8 subframe){


  u8 Size=0;
  UE_MODE_t UE_mode = mac_xface->get_ue_mode(Mod_id,eNB_index);
  u8 lcid = CCCH;
  u16 Size16;
  struct RACH_ConfigCommon *rach_ConfigCommon = (struct RACH_ConfigCommon *)NULL;
  s32 frame_diff=0;


  if (UE_mode == PRACH) {
    if (UE_mac_inst[Mod_id].radioResourceConfigCommon)
      rach_ConfigCommon = &UE_mac_inst[Mod_id].radioResourceConfigCommon->rach_ConfigCommon;
    else {
      return(NULL);
    }
    if (Is_rrc_registered == 1) {

      if (UE_mac_inst[Mod_id].RA_active == 0) {
	// check if RRC is ready to initiate the RA procedure
	Size = mac_rrc_data_req(Mod_id,
				frame,
				CCCH,1,
				(char*)&UE_mac_inst[Mod_id].CCCH_pdu.payload[sizeof(SCH_SUBHEADER_SHORT)+1],0,
				eNB_index);
	Size16 = (u16)Size;
	
	//	LOG_D(MAC,"[UE %d] Frame %d: Requested RRCConnectionRequest, got %d bytes\n",Mod_id,frame,Size);
	LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_DATA_REQ (RRCConnectionRequest eNB %d) --->][MAC_UE][MOD %02d][]\n",
	      frame, Mod_id, eNB_index, Mod_id);
	LOG_D(MAC,"[UE %d] Frame %d: Requested RRCConnectionRequest, got %d bytes\n",Mod_id,frame,Size);

	if (Size>0) {



	  UE_mac_inst[Mod_id].RA_active                        = 1;
	  UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER = 1;
      UE_mac_inst[Mod_id].RA_Msg3_size                     = Size+sizeof(SCH_SUBHEADER_SHORT)+sizeof(SCH_SUBHEADER_SHORT);
	  UE_mac_inst[Mod_id].RA_prachMaskIndex                = 0;
	  UE_mac_inst[Mod_id].RA_prach_resources.Msg3          = UE_mac_inst[Mod_id].CCCH_pdu.payload;
	  UE_mac_inst[Mod_id].RA_backoff_cnt                   = 0;  // add the backoff condition here if we have it from a previous RA reponse which failed (i.e. backoff indicator)
	  if (rach_ConfigCommon) {
	    UE_mac_inst[Mod_id].RA_window_cnt                    = 2+ rach_ConfigCommon->ra_SupervisionInfo.ra_ResponseWindowSize;
	    if (UE_mac_inst[Mod_id].RA_window_cnt == 9)
	      UE_mac_inst[Mod_id].RA_window_cnt = 10;  // Note: 9 subframe window doesn't exist, after 8 is 10!
	  }
	  else {
	    LOG_D(MAC,"[UE %d] FATAL Frame %d: rach_ConfigCommon is NULL !!!\n",Mod_id,frame);
	    mac_xface->macphy_exit("");
	  }
	  UE_mac_inst[Mod_id].RA_tx_frame    = frame;
	  UE_mac_inst[Mod_id].RA_tx_subframe = subframe;
	  UE_mac_inst[Mod_id].RA_backoff_frame    = frame;
	  UE_mac_inst[Mod_id].RA_backoff_subframe = subframe;
	  // Fill in preamble and PRACH resource
	  get_prach_resources(Mod_id,eNB_index,subframe,1,NULL);

	  generate_ulsch_header((u8*)&UE_mac_inst[Mod_id].CCCH_pdu.payload[0],  // mac header
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

	  return(&UE_mac_inst[Mod_id].RA_prach_resources);
	}
      }
      else {  // RACH is active
	LOG_D(MAC,"[MAC][UE %d][RAPROC] frame %d, subframe %d: RA Active, window cnt %d (RA_tx_frame %d, RA_tx_subframe %d)\n",Mod_id,
	      frame,subframe,UE_mac_inst[Mod_id].RA_window_cnt,
	      UE_mac_inst[Mod_id].RA_tx_frame,UE_mac_inst[Mod_id].RA_tx_subframe);
	// compute backoff parameters
	if (UE_mac_inst[Mod_id].RA_backoff_cnt>0) {
	  frame_diff = (s32)frame - UE_mac_inst[Mod_id].RA_backoff_frame;
	  if (frame_diff < 0)
	    frame_diff = -frame_diff;
	  UE_mac_inst[Mod_id].RA_backoff_cnt -= ((10*frame_diff) + (subframe-UE_mac_inst[Mod_id].RA_backoff_subframe));

	  UE_mac_inst[Mod_id].RA_backoff_frame    = frame;
	  UE_mac_inst[Mod_id].RA_backoff_subframe = subframe;
	}
	// compute RA window parameters
	if (UE_mac_inst[Mod_id].RA_window_cnt>0) {
	  frame_diff = (s32)frame - UE_mac_inst[Mod_id].RA_tx_frame;
	  if (frame_diff < 0)
	    frame_diff = -frame_diff;
	  UE_mac_inst[Mod_id].RA_window_cnt -= ((10*frame_diff) + (subframe-UE_mac_inst[Mod_id].RA_tx_subframe));
	  LOG_D(MAC,"[MAC][UE %d][RAPROC] frame %d, subframe %d: RA Active, adjusted window cnt %d\n",Mod_id,
		frame,subframe,UE_mac_inst[Mod_id].RA_window_cnt);
	}
	if ((UE_mac_inst[Mod_id].RA_window_cnt<=0) &&
	    (UE_mac_inst[Mod_id].RA_backoff_cnt<=0)) {

	  UE_mac_inst[Mod_id].RA_tx_frame    = frame;
	  UE_mac_inst[Mod_id].RA_tx_subframe = subframe;
	  UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER++;
	  UE_mac_inst[Mod_id].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER += (rach_ConfigCommon->powerRampingParameters.powerRampingStep<<1);  // 2dB increments in ASN.1 definition
	  if (UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER == rach_ConfigCommon->ra_SupervisionInfo.preambleTransMax) {
	    LOG_D(MAC,"[UE %d] Frame %d: Maximum number of RACH attempts (%d)\n",Mod_id,frame,rach_ConfigCommon->ra_SupervisionInfo.preambleTransMax);
	    // send message to RRC
	    UE_mac_inst[Mod_id].RA_PREAMBLE_TRANSMISSION_COUNTER=1;
	    UE_mac_inst[Mod_id].RA_prach_resources.ra_PREAMBLE_RECEIVED_TARGET_POWER = get_Po_NOMINAL_PUSCH(Mod_id);
	  }
	  UE_mac_inst[Mod_id].RA_window_cnt                    = 2+ rach_ConfigCommon->ra_SupervisionInfo.ra_ResponseWindowSize;
	  UE_mac_inst[Mod_id].RA_backoff_cnt                   = 0;

	// Fill in preamble and PRACH resource
	  get_prach_resources(Mod_id,eNB_index,subframe,0,NULL);
	  return(&UE_mac_inst[Mod_id].RA_prach_resources);
	}
      }
    }
  }
  else if (UE_mode == PUSCH) {
    LOG_D(MAC,"[UE %d] FATAL: Should not have checked for RACH in PUSCH yet ...",Mod_id);
    mac_xface->macphy_exit("");
  }
  return(NULL);
}
