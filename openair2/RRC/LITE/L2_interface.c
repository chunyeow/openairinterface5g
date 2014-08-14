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

/*! \file l2_interface.c
* \brief layer 2 interface, used to support different RRC sublayer
* \author Raymond Knopp and Navid Nikaein 
* \date 2010-2014
* \version 1.0 
* \company Eurecom
* \email: raymond.knopp@eurecom.fr 
*/ 

#include "platform_types.h"
//#include "openair_defs.h"
//#include "openair_proto.h"
#include "defs.h"
#include "extern.h"
//#include "mac_lchan_interface.h"
//#include "openair_rrc_utils.h"
//#include "openair_rrc_main.h"
#include "UTIL/LOG/log.h"
#include "pdcp.h"

#ifdef PHY_EMUL
#include "SIMULATION/simulation_defs.h"
extern EMULATION_VARS *Emul_vars;
extern eNB_MAC_INST *eNB_mac_inst;
extern UE_MAC_INST *UE_mac_inst;
#endif

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

//#define RRC_DATA_REQ_DEBUG
#define DEBUG_RRC

mui_t mui=0;

//-------------------------------------------------------------------------------------------//
int8_t mac_rrc_lite_data_req(module_id_t Mod_idP, frame_t frameP, uint16_t Srb_id, uint8_t Nb_tb, uint8_t *buffer_pP, eNB_flag_t enb_flagP, uint8_t eNB_index,
                         uint8_t mbsfn_sync_area) {
//-------------------------------------------------------------------------------------------//
  SRB_INFO *Srb_info;
  uint8_t Sdu_size=0;

#ifdef DEBUG_RRC
  int i;
  LOG_D(RRC,"[eNB %d] mac_rrc_data_req to SRB ID=%d\n",Mod_idP,Srb_id);
#endif

  if( enb_flagP == ENB_FLAG_YES){

    if((Srb_id & RAB_OFFSET) == BCCH){
      if(eNB_rrc_inst[Mod_idP].SI.Active==0) return 0;

      // All even frames transmit SIB in SF 5
      if (eNB_rrc_inst[Mod_idP].sizeof_SIB1 == 255) {
	LOG_E(RRC,"[eNB %d] MAC Request for SIB1 and SIB1 not initialized\n",Mod_idP);
	mac_xface->macphy_exit("");
      }
      if ((frameP%2) == 0) {
        memcpy(&buffer_pP[0],eNB_rrc_inst[Mod_idP].SIB1,eNB_rrc_inst[Mod_idP].sizeof_SIB1);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int sib1_size = eNB_rrc_inst[Mod_idP].sizeof_SIB1;
        int sdu_size = sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu);

        if (sib1_size > sdu_size)
        {
          LOG_E(RRC, "SIB1 SDU larger than BCCH SDU buffer size (%d, %d)", sib1_size, sdu_size);
          sib1_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_BCCH_DATA_REQ);
        RRC_MAC_BCCH_DATA_REQ (message_p).frame    = frameP;
        RRC_MAC_BCCH_DATA_REQ (message_p).sdu_size = sib1_size;
        memset (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, 0, sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu));
        memcpy (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_idP].SIB1, sib1_size);
        RRC_MAC_BCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_idP, message_p);
      }
#endif

#ifdef DEBUG_RRC
	LOG_D(RRC,"[eNB %d] Frame %d : BCCH request => SIB 1\n",Mod_idP,frameP);
	for (i=0;i<eNB_rrc_inst[Mod_idP].sizeof_SIB1;i++)
	  LOG_T(RRC,"%x.",buffer_pP[i]);
	LOG_T(RRC,"\n");
#endif

	return (eNB_rrc_inst[Mod_idP].sizeof_SIB1);
      } // All RFN mod 8 transmit SIB2-3 in SF 5
      else if ((frameP%8) == 1){
	memcpy(&buffer_pP[0],eNB_rrc_inst[Mod_idP].SIB23,eNB_rrc_inst[Mod_idP].sizeof_SIB23);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int sib23_size = eNB_rrc_inst[Mod_idP].sizeof_SIB23;
        int sdu_size = sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu);

        if (sib23_size > sdu_size)
        {
          LOG_E(RRC, "SIB23 SDU larger than BCCH SDU buffer size (%d, %d)", sib23_size, sdu_size);
          sib23_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_BCCH_DATA_REQ);
        RRC_MAC_BCCH_DATA_REQ (message_p).frame = frameP;
        RRC_MAC_BCCH_DATA_REQ (message_p).sdu_size = sib23_size;
        memset (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, 0, sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu));
        memcpy (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_idP].SIB23, sib23_size);
        RRC_MAC_BCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_idP, message_p);
      }
#endif

#ifdef DEBUG_RRC
	LOG_D(RRC,"[eNB %d] Frame %d BCCH request => SIB 2-3\n",Mod_idP,frameP);
	for (i=0;i<eNB_rrc_inst[Mod_idP].sizeof_SIB23;i++)
	  LOG_T(RRC,"%x.",buffer_pP[i]);
	LOG_T(RRC,"\n");
#endif
	return(eNB_rrc_inst[Mod_idP].sizeof_SIB23);
      }
      else
	return(0);
    }


    if( (Srb_id & RAB_OFFSET ) == CCCH){
      LOG_D(RRC,"[eNB %d] Frame %d CCCH request (Srb_id %d)\n",Mod_idP,frameP, Srb_id);

      if(eNB_rrc_inst[Mod_idP].Srb0.Active==0) {
	LOG_E(RRC,"[eNB %d] CCCH Not active\n",Mod_idP);
	return -1;
      }
      Srb_info=&eNB_rrc_inst[Mod_idP].Srb0;

      // check if data is there for MAC
      if(Srb_info->Tx_buffer.payload_size>0){//Fill buffer
	LOG_D(RRC,"[eNB %d] CCCH (%p) has %d bytes (dest: %p, src %p)\n",Mod_idP,Srb_info,Srb_info->Tx_buffer.payload_size,buffer_pP,Srb_info->Tx_buffer.Payload);

#if defined(ENABLE_ITTI)
        {
          MessageDef *message_p;
          int ccch_size = Srb_info->Tx_buffer.payload_size;
          int sdu_size = sizeof(RRC_MAC_CCCH_DATA_REQ (message_p).sdu);

          if (ccch_size > sdu_size) {
            LOG_E(RRC, "SDU larger than CCCH SDU buffer size (%d, %d)", ccch_size, sdu_size);
            ccch_size = sdu_size;
          }

          message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_CCCH_DATA_REQ);
          RRC_MAC_CCCH_DATA_REQ (message_p).frame = frameP;
          RRC_MAC_CCCH_DATA_REQ (message_p).sdu_size = ccch_size;
          memset (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, 0, sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu));
          memcpy (RRC_MAC_CCCH_DATA_REQ (message_p).sdu, Srb_info->Tx_buffer.Payload, ccch_size);
          RRC_MAC_CCCH_DATA_REQ (message_p).enb_index = eNB_index;

          itti_send_msg_to_task (TASK_MAC_ENB, Mod_idP, message_p);
        }
#endif

        memcpy(buffer_pP,Srb_info->Tx_buffer.Payload,Srb_info->Tx_buffer.payload_size);
	Sdu_size = Srb_info->Tx_buffer.payload_size;
	Srb_info->Tx_buffer.payload_size=0;
      }

      return (Sdu_size);
    }

#ifdef Rel10
    if((Srb_id & RAB_OFFSET) == MCCH){
      if(eNB_rrc_inst[Mod_idP].MCCH_MESS[mbsfn_sync_area].Active==0) return 0; // this parameter is set in function init_mcch in rrc_eNB.c
      // this part not needed as it is done in init_mcch 
      /*     if (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area] == 255) {
	LOG_E(RRC,"[eNB %d] MAC Request for MCCH MESSAGE and MCCH MESSAGE is not initialized\n",Mod_id);
	mac_xface->macphy_exit("");
	}*/


#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int mcch_size = eNB_rrc_inst[Mod_idP].sizeof_MCCH_MESSAGE[mbsfn_sync_area];
        int sdu_size = sizeof(RRC_MAC_MCCH_DATA_REQ (message_p).sdu);

        if (mcch_size > sdu_size) {
          LOG_E(RRC, "SDU larger than MCCH SDU buffer size (%d, %d)", mcch_size, sdu_size);
          mcch_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_MCCH_DATA_REQ);
        RRC_MAC_MCCH_DATA_REQ (message_p).frame = frameP;
        RRC_MAC_MCCH_DATA_REQ (message_p).sdu_size = mcch_size;
        memset (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, 0, sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu));
        memcpy (RRC_MAC_MCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_idP].MCCH_MESSAGE[mbsfn_sync_area], mcch_size);
        RRC_MAC_MCCH_DATA_REQ (message_p).enb_index = eNB_index;
        RRC_MAC_MCCH_DATA_REQ (message_p).mbsfn_sync_area = mbsfn_sync_area;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_idP, message_p);
      }
#endif

      memcpy(&buffer_pP[0],
	     eNB_rrc_inst[Mod_idP].MCCH_MESSAGE[mbsfn_sync_area],
	     eNB_rrc_inst[Mod_idP].sizeof_MCCH_MESSAGE[mbsfn_sync_area]);
      
#ifdef DEBUG_RRC
      LOG_D(RRC,"[eNB %d] Frame %d : MCCH request => MCCH_MESSAGE \n",Mod_idP,frameP);
      for (i=0;i<eNB_rrc_inst[Mod_idP].sizeof_MCCH_MESSAGE[mbsfn_sync_area];i++)
	LOG_T(RRC,"%x.",buffer_pP[i]);
      LOG_T(RRC,"\n");
#endif
      
      return (eNB_rrc_inst[Mod_idP].sizeof_MCCH_MESSAGE[mbsfn_sync_area]);
      //      }
      //else
      //return(0);
    }
#endif //Rel10    
  }

  else{   //This is an UE
#ifdef DEBUG_RRC
    LOG_D(RRC,"[UE %d] Frame %d Filling CCCH SRB_ID %d\n",Mod_idP,frameP,Srb_id);
    LOG_D(RRC,"[UE %d] Frame %d buffer_pP status %d,\n",Mod_idP,frameP, UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size);
#endif
    if( (UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size > 0) ) {

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int ccch_size = UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size;
        int sdu_size = sizeof(RRC_MAC_CCCH_DATA_REQ (message_p).sdu);

        if (ccch_size > sdu_size) {
          LOG_E(RRC, "SDU larger than CCCH SDU buffer size (%d, %d)", ccch_size, sdu_size);
          ccch_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_UE, RRC_MAC_CCCH_DATA_REQ);
        RRC_MAC_CCCH_DATA_REQ (message_p).frame = frameP;
        RRC_MAC_CCCH_DATA_REQ (message_p).sdu_size = ccch_size;
        memset (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, 0, sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu));
        memcpy (RRC_MAC_CCCH_DATA_REQ (message_p).sdu, UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.Payload, ccch_size);
        RRC_MAC_CCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_UE, Mod_idP + NB_eNB_INST, message_p);
      }
#endif

      memcpy(&buffer_pP[0],&UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.Payload[0],UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size);
      uint8_t Ret_size=UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size;
      //   UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size=0;
      UE_rrc_inst[Mod_idP].Info[eNB_index].T300_active = 1;
      UE_rrc_inst[Mod_idP].Info[eNB_index].T300_cnt = 0;
      //      msg("[RRC][UE %d] Sending rach\n",Mod_id);
      return(Ret_size);
    }
    else{
      return 0;
    }
  }
  return(0);
}

//-------------------------------------------------------------------------------------------//
int8_t mac_rrc_lite_data_ind(module_id_t Mod_idP, frame_t frameP, rb_id_t Srb_id, uint8_t *Sdu, sdu_size_t sdu_size,eNB_flag_t enb_flagP,uint8_t eNB_index,uint8_t mbsfn_sync_area){
//-------------------------------------------------------------------------------------------//
  SRB_INFO *Srb_info;
  /*
  int si_window;
  */

  if(enb_flagP == ENB_FLAG_NO){

    if(Srb_id == BCCH){
      
      LOG_T(RRC,"[UE %d] Received SDU for BCCH on SRB %d from eNB %d\n",Mod_idP,Srb_id,eNB_index);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int msg_sdu_size = sizeof(RRC_MAC_BCCH_DATA_IND (message_p).sdu);

        if (sdu_size > msg_sdu_size)
        {
          LOG_E(RRC, "SDU larger than BCCH SDU buffer size (%d, %d)", sdu_size, msg_sdu_size);
          sdu_size = msg_sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_BCCH_DATA_IND);
        RRC_MAC_BCCH_DATA_IND (message_p).frame = frameP;
        RRC_MAC_BCCH_DATA_IND (message_p).sdu_size = sdu_size;
        memset (RRC_MAC_CCCH_DATA_IND (message_p).sdu, 0, sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu));
        memcpy (RRC_MAC_BCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
        RRC_MAC_BCCH_DATA_IND (message_p).enb_index = eNB_index;
        RRC_MAC_BCCH_DATA_IND (message_p).rsrq = 30 /* TODO change phy to report rspq */;
        RRC_MAC_BCCH_DATA_IND (message_p).rsrp = 45 /* TODO change phy to report rspp */;

        itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
      }
#else
      decode_BCCH_DLSCH_Message(Mod_idP,frameP,eNB_index,Sdu,sdu_size, 0, 0);
      /*
      if ((frameP %2) == 0) {
	if (UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status == 0) {
	  LOG_D(RRC,"[UE %d] Frame %d : Received SIB1 from eNB %d (%d bytes)\n",Mod_id,frameP,eNB_index,sdu_size);
	  if (UE_rrc_inst[Mod_id].SIB1[eNB_index])
	    memcpy(UE_rrc_inst[Mod_id].SIB1[eNB_index],&Sdu[0],sdu_size);
	  else {
	    LOG_E(RRC,"[FATAL ERROR] SIB1 buffer for eNB %d not allocated, exiting ...\n",eNB_index);
	    mac_xface->macphy_exit("");
	    return(-1);
	  }
	  UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status = 1;
	  decode_SIB1(Mod_id,eNB_index);
	}
      }
      else {
	if ((UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status == 1) &&
	    (UE_rrc_inst[Mod_id].Info[eNB_index].SIStatus == 0)) {
	  si_window = (frameP%UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod)/frameP%UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize;
	  LOG_D(RRC,"[UE %d] Frame %d : Received SI (%d bytes), in window %d (SIperiod %d, SIwindowsize %d)\n",Mod_id,frameP,sdu_size,si_window,UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod,UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize);
	  memcpy(UE_rrc_inst[Mod_id].SI[eNB_index],&Sdu[0],sdu_size);
	  if (decode_SI(Mod_id,frameP,eNB_index,si_window)==0) {
	    LOG_D(RRC,"[UE %d] Frame %d :Decoded SI successfully\n",Mod_id,frameP);
	    UE_rrc_inst[Mod_id].Info[eNB_index].SIStatus = 1;
	  }
	  }


    
	  }


      if ((UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status == 1) &&
	  (UE_rrc_inst[Mod_id].Info[eNB_index].SIStatus == 1)) {
	if (UE_rrc_inst[Mod_id].Info[eNB_index].State == RRC_IDLE) {
	  LOG_I(RRC,"[UE %d] Received SIB1/SIB2/SIB3 Switching to RRC_SI_RECEIVED\n",Mod_id);
	  UE_rrc_inst[Mod_id].Info[eNB_index].State = RRC_SI_RECEIVED;
	}
      }
      */
#endif
    }

    if((Srb_id & RAB_OFFSET) == CCCH){
      if (sdu_size>0) {
        LOG_T(RRC,"[UE %d] Received SDU for CCCH on SRB %d from eNB %d\n",Mod_idP,Srb_id & RAB_OFFSET,eNB_index);

#if defined(ENABLE_ITTI)
        {
          MessageDef *message_p;
          int msg_sdu_size = sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu);

          if (sdu_size > msg_sdu_size)
          {
            LOG_E(RRC, "SDU larger than CCCH SDU buffer size (%d, %d)", sdu_size, msg_sdu_size);
            sdu_size = msg_sdu_size;
          }

          message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_CCCH_DATA_IND);
          RRC_MAC_CCCH_DATA_IND (message_p).frame = frameP;
          RRC_MAC_CCCH_DATA_IND (message_p).sdu_size = sdu_size;
          memset (RRC_MAC_CCCH_DATA_IND (message_p).sdu, 0, sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu));
          memcpy (RRC_MAC_CCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
          RRC_MAC_CCCH_DATA_IND (message_p).enb_index = eNB_index;

          itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
      }
#else
        Srb_info = &UE_rrc_inst[Mod_idP].Srb0[eNB_index];

        memcpy(Srb_info->Rx_buffer.Payload,Sdu,sdu_size);
        Srb_info->Rx_buffer.payload_size = sdu_size;
        rrc_ue_decode_ccch(Mod_idP, frameP, Srb_info, eNB_index);
#endif
      }
    }
      
#ifdef Rel10
    if ((Srb_id & RAB_OFFSET) == MCCH) {
      LOG_T(RRC,"[UE %d] Frame %d: Received SDU on MBSFN sync area %d for MCCH on SRB %d from eNB %d\n",
          Mod_idP,frameP, mbsfn_sync_area, Srb_id & RAB_OFFSET,eNB_index);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int msg_sdu_size = sizeof(RRC_MAC_MCCH_DATA_IND (message_p).sdu);

        if (sdu_size > msg_sdu_size)
        {
          LOG_E(RRC, "SDU larger than MCCH SDU buffer size (%d, %d)", sdu_size, msg_sdu_size);
          sdu_size = msg_sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_MCCH_DATA_IND);
        RRC_MAC_MCCH_DATA_IND (message_p).frame           = frameP;
        RRC_MAC_MCCH_DATA_IND (message_p).sdu_size        = sdu_size;
        memset (RRC_MAC_CCCH_DATA_IND (message_p).sdu, 0, sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu));
        memcpy (RRC_MAC_MCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
        RRC_MAC_MCCH_DATA_IND (message_p).enb_index       = eNB_index;
        RRC_MAC_MCCH_DATA_IND (message_p).mbsfn_sync_area = mbsfn_sync_area;

        itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
      }
#else
      decode_MCCH_Message(Mod_idP, frameP, eNB_index, Sdu, sdu_size, mbsfn_sync_area);
#endif
    }
#endif // Rel10

  }

  else{  // This is an eNB
    Srb_info = &eNB_rrc_inst[Mod_idP].Srb0;
    LOG_T(RRC,"[eNB %d] Received SDU for CCCH on SRB %d\n",Mod_idP,Srb_info->Srb_id);
    
#if defined(ENABLE_ITTI)
        {
          MessageDef *message_p;
          int msg_sdu_size = sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu);

          if (sdu_size > msg_sdu_size)
          {
            LOG_E(RRC, "SDU larger than CCCH SDU buffer size (%d, %d)", sdu_size, msg_sdu_size);
            sdu_size = msg_sdu_size;
          }

          message_p = itti_alloc_new_message (TASK_MAC_ENB, RRC_MAC_CCCH_DATA_IND);
          RRC_MAC_CCCH_DATA_IND (message_p).frame = frameP;
          RRC_MAC_CCCH_DATA_IND (message_p).sdu_size = sdu_size;
          memset (RRC_MAC_CCCH_DATA_IND (message_p).sdu, 0, sizeof(RRC_MAC_CCCH_DATA_IND (message_p).sdu));
          memcpy (RRC_MAC_CCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);

          itti_send_msg_to_task (TASK_RRC_ENB, Mod_idP, message_p);
      }
#else
    //    msg("\n******INST %d Srb_info %p, Srb_id=%d****\n\n",Mod_id,Srb_info,Srb_info->Srb_id);
    if (sdu_size > 0) {
	 		memcpy(Srb_info->Rx_buffer.Payload,Sdu,sdu_size);
			Srb_info->Rx_buffer.payload_size = sdu_size;
    	rrc_eNB_decode_ccch(Mod_idP,frameP,Srb_info);
		}
#endif
  }

  return(0);

}

//-------------------------------------------------------------------------------------------//
// this function is Not USED anymore
void mac_lite_sync_ind(module_id_t Mod_idP,uint8_t Status){
//-------------------------------------------------------------------------------------------//
}

//-------------------------------------------------------------------------------------------//
uint8_t rrc_lite_data_req(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, rb_id_t rb_idP, mui_t muiP, uint32_t confirmP,
                     sdu_size_t sdu_size, uint8_t* buffer_pP, pdcp_transmission_mode_t mode) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;
    // Uses a new buffer to avoid issue with PDCP buffer content that could be changed by PDCP (asynchronous message handling).
    uint8_t *message_buffer;

    message_buffer = itti_malloc (enb_flagP ? TASK_RRC_ENB : TASK_RRC_UE, enb_flagP ? TASK_PDCP_ENB : TASK_PDCP_UE, sdu_size);
    memcpy (message_buffer, buffer_pP, sdu_size);

    message_p = itti_alloc_new_message (enb_flagP ? TASK_RRC_ENB : TASK_RRC_UE, RRC_DCCH_DATA_REQ);
    RRC_DCCH_DATA_REQ (message_p).frame     = frameP;
    RRC_DCCH_DATA_REQ (message_p).enb_flag  = enb_flagP;
    RRC_DCCH_DATA_REQ (message_p).rb_id     = rb_idP;
    RRC_DCCH_DATA_REQ (message_p).muip      = muiP;
    RRC_DCCH_DATA_REQ (message_p).confirmp  = confirmP;
    RRC_DCCH_DATA_REQ (message_p).sdu_size  = sdu_size;
    RRC_DCCH_DATA_REQ (message_p).sdu_p     = message_buffer;
    RRC_DCCH_DATA_REQ (message_p).mode      = mode;
    RRC_DCCH_DATA_REQ (message_p).eNB_index = enb_mod_idP;
    RRC_DCCH_DATA_REQ (message_p).ue_index  = ue_mod_idP;

    itti_send_msg_to_task (enb_flagP ? TASK_PDCP_ENB : TASK_PDCP_UE, enb_flagP ? enb_mod_idP : NB_eNB_INST + ue_mod_idP, message_p);
    return TRUE; // TODO should be changed to a CNF message later, currently RRC lite does not used the returned value anyway.

  }
#else
  return pdcp_data_req (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, SRB_FLAG_YES, rb_idP, muiP, confirmP, sdu_size, buffer_pP, mode);
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_data_ind(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP,rb_id_t Srb_id, sdu_size_t sdu_sizeP,uint8_t *buffer_pP){
//-------------------------------------------------------------------------------------------//
  rb_id_t    DCCH_index = Srb_id;
#if defined(ENABLE_ITTI)
  instance_t instance;
#endif

  if (enb_flagP == ENB_FLAG_NO) {
#if defined(ENABLE_ITTI)
      instance = ue_mod_idP + NB_eNB_INST;
#endif
      LOG_N(RRC, "[UE %d] Frame %d: received a DCCH %d message on SRB %d with Size %d from eNB %d\n",
          ue_mod_idP, frameP, DCCH_index,Srb_id-1,sdu_sizeP, enb_mod_idP);
  } else {
#if defined(ENABLE_ITTI)
      instance = enb_mod_idP;
#endif
      LOG_N(RRC, "[eNB %d] Frame %d: received a DCCH %d message on SRB %d with Size %d from UE %d\n",
          enb_mod_idP, frameP, DCCH_index,Srb_id-1,sdu_sizeP, ue_mod_idP);
  }

#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;
    // Uses a new buffer to avoid issue with PDCP buffer content that could be changed by PDCP (asynchronous message handling).
    uint8_t *message_buffer;

    message_buffer = itti_malloc (enb_flagP ? TASK_PDCP_ENB : TASK_PDCP_UE, enb_flagP ? TASK_RRC_ENB : TASK_RRC_UE, sdu_sizeP);
    memcpy (message_buffer, buffer_pP, sdu_sizeP);

    message_p = itti_alloc_new_message (enb_flagP ? TASK_PDCP_ENB : TASK_PDCP_UE, RRC_DCCH_DATA_IND);
    RRC_DCCH_DATA_IND (message_p).frame      = frameP;
    RRC_DCCH_DATA_IND (message_p).dcch_index = DCCH_index;
    RRC_DCCH_DATA_IND (message_p).sdu_size   = sdu_sizeP;
    RRC_DCCH_DATA_IND (message_p).sdu_p      = message_buffer;
    RRC_DCCH_DATA_IND (message_p).ue_index   = ue_mod_idP;
    RRC_DCCH_DATA_IND (message_p).eNB_index  = enb_mod_idP;

    itti_send_msg_to_task (enb_flagP ? TASK_RRC_ENB : TASK_RRC_UE, instance, message_p);
  }
#else
  if (enb_flagP == ENB_FLAG_YES) {
    rrc_eNB_decode_dcch(enb_mod_idP,frameP,DCCH_index,ue_mod_idP,buffer_pP,sdu_sizeP);
  }
  else {
    rrc_ue_decode_dcch(ue_mod_idP,frameP,DCCH_index,buffer_pP,enb_mod_idP);
  }
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_in_sync_ind(module_id_t Mod_idP, frame_t frameP, uint16_t eNB_index) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_IN_SYNC_IND);
    RRC_MAC_IN_SYNC_IND (message_p).frame = frameP;
    RRC_MAC_IN_SYNC_IND (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
  }
#else
  UE_rrc_inst[Mod_idP].Info[eNB_index].N310_cnt=0;
  if (UE_rrc_inst[Mod_idP].Info[eNB_index].T310_active==1)
    UE_rrc_inst[Mod_idP].Info[eNB_index].N311_cnt++;
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_out_of_sync_ind(module_id_t Mod_idP, frame_t frameP, uint16_t eNB_index){
//-------------------------------------------------------------------------------------------//
  LOG_I(RRC,"[UE %d] Frame %d: OUT OF SYNC FROM eNB %d (T310 %d, N310 %d, N311 %d)\n ",
      Mod_idP,frameP,eNB_index,
        UE_rrc_inst[Mod_idP].Info[eNB_index].T310_cnt,
        UE_rrc_inst[Mod_idP].Info[eNB_index].N310_cnt,
        UE_rrc_inst[Mod_idP].Info[eNB_index].N311_cnt);

#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_OUT_OF_SYNC_IND);
    RRC_MAC_OUT_OF_SYNC_IND (message_p).frame = frameP;
    RRC_MAC_OUT_OF_SYNC_IND (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
  }
#else
  UE_rrc_inst[Mod_idP].Info[eNB_index].N310_cnt++;
#endif
}

//-------------------------------------------------------------------------------------------//
int mac_get_rrc_lite_status(module_id_t Mod_idP,eNB_flag_t enb_flagP,uint8_t index){
//-------------------------------------------------------------------------------------------//
  if(enb_flagP == ENB_FLAG_YES)
    return(eNB_rrc_inst[Mod_idP].Info.UE[index].Status);
  else
    return(UE_rrc_inst[Mod_idP].Info[index].State);
}

//-------------------------------------------------------------------------------------------//
int mac_ue_ccch_success_ind(module_id_t Mod_idP, uint8_t eNB_index) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_CCCH_DATA_CNF);
    RRC_MAC_CCCH_DATA_CNF (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_idP + NB_eNB_INST, message_p);
  }
#else
  // reset the tx buffer to indicate RRC that ccch was successfully transmitted (for example if contention resolution succeeds)
  UE_rrc_inst[Mod_idP].Srb0[eNB_index].Tx_buffer.payload_size=0;
#endif
  return 0;
}
