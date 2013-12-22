/*******************************************************************************

  Eurecom OpenAirInterface 2
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

/*! \file l2_interface.c
* \brief layer 2 interface 
* \author Raymond Knopp and Navid Nikaein 
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: raymond.knopp@eurecom.fr 
*/ 
//#include "openair_types.h"
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

u32 mui=0;

//-------------------------------------------------------------------------------------------//
s8 mac_rrc_lite_data_req(u8 Mod_id, u32 frame, u16 Srb_id, u8 Nb_tb, u8 *Buffer, u8 eNB_flag, u8 eNB_index,
                         u8 mbsfn_sync_area) {
//-------------------------------------------------------------------------------------------//
  SRB_INFO *Srb_info;
  u8 Sdu_size=0;

#ifdef DEBUG_RRC
  int i;
  LOG_D(RRC,"[eNB %d] mac_rrc_data_req to SRB ID=%d\n",Mod_id,Srb_id);
#endif

  if( eNB_flag == 1){

    if((Srb_id & RAB_OFFSET) == BCCH){
      if(eNB_rrc_inst[Mod_id].SI.Active==0) return 0;

      // All even frames transmit SIB in SF 5
      if (eNB_rrc_inst[Mod_id].sizeof_SIB1 == 255) {
	LOG_E(RRC,"[eNB %d] MAC Request for SIB1 and SIB1 not initialized\n",Mod_id);
	mac_xface->macphy_exit("");
      }
      if ((frame%2) == 0) {
        memcpy(&Buffer[0],eNB_rrc_inst[Mod_id].SIB1,eNB_rrc_inst[Mod_id].sizeof_SIB1);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int sib1_size = eNB_rrc_inst[Mod_id].sizeof_SIB1;
        int sdu_size = sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu);

        if (sib1_size > sdu_size)
        {
          LOG_E(RRC, "SIB1 SDU larger than BCCH SDU buffer size (%d, %d)", sib1_size, sdu_size);
          sib1_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_BCCH_DATA_REQ);
        RRC_MAC_BCCH_DATA_REQ (message_p).frame = frame;
        RRC_MAC_BCCH_DATA_REQ (message_p).sdu_size = sib1_size;
        memcpy (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_id].SIB1, sib1_size);
        RRC_MAC_BCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_id, message_p);
      }
#endif

#ifdef DEBUG_RRC
	LOG_D(RRC,"[eNB %d] Frame %d : BCCH request => SIB 1\n",Mod_id,frame);
	for (i=0;i<eNB_rrc_inst[Mod_id].sizeof_SIB1;i++)
	  LOG_T(RRC,"%x.",Buffer[i]);
	LOG_T(RRC,"\n");
#endif

	return (eNB_rrc_inst[Mod_id].sizeof_SIB1);
      } // All RFN mod 8 transmit SIB2-3 in SF 5
      else if ((frame%8) == 1){
	memcpy(&Buffer[0],eNB_rrc_inst[Mod_id].SIB23,eNB_rrc_inst[Mod_id].sizeof_SIB23);

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int sib23_size = eNB_rrc_inst[Mod_id].sizeof_SIB23;
        int sdu_size = sizeof(RRC_MAC_BCCH_DATA_REQ (message_p).sdu);

        if (sib23_size > sdu_size)
        {
          LOG_E(RRC, "SIB23 SDU larger than BCCH SDU buffer size (%d, %d)", sib23_size, sdu_size);
          sib23_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_BCCH_DATA_REQ);
        RRC_MAC_BCCH_DATA_REQ (message_p).frame = frame;
        RRC_MAC_BCCH_DATA_REQ (message_p).sdu_size = sib23_size;
        memcpy (RRC_MAC_BCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_id].SIB23, sib23_size);
        RRC_MAC_BCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_id, message_p);
      }
#endif

#ifdef DEBUG_RRC
	LOG_D(RRC,"[eNB %d] Frame %d BCCH request => SIB 2-3\n",Mod_id,frame);
	for (i=0;i<eNB_rrc_inst[Mod_id].sizeof_SIB23;i++)
	  LOG_T(RRC,"%x.",Buffer[i]);
	LOG_T(RRC,"\n");
#endif
	return(eNB_rrc_inst[Mod_id].sizeof_SIB23);
      }
      else
	return(0);
    }


    if( (Srb_id & RAB_OFFSET ) == CCCH){
      LOG_D(RRC,"[eNB %d] Frame %d CCCH request (Srb_id %d)\n",Mod_id,frame, Srb_id);

      if(eNB_rrc_inst[Mod_id].Srb0.Active==0) {
	LOG_E(RRC,"[eNB %d] CCCH Not active\n",Mod_id);
	return -1;
      }
      Srb_info=&eNB_rrc_inst[Mod_id].Srb0;

      // check if data is there for MAC
      if(Srb_info->Tx_buffer.payload_size>0){//Fill buffer
	LOG_D(RRC,"[eNB %d] CCCH (%p) has %d bytes (dest: %p, src %p)\n",Mod_id,Srb_info,Srb_info->Tx_buffer.payload_size,Buffer,Srb_info->Tx_buffer.Payload);

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
          RRC_MAC_CCCH_DATA_REQ (message_p).frame = frame;
          RRC_MAC_CCCH_DATA_REQ (message_p).sdu_size = ccch_size;
          memcpy (RRC_MAC_CCCH_DATA_REQ (message_p).sdu, Srb_info->Tx_buffer.Payload, ccch_size);
          RRC_MAC_CCCH_DATA_REQ (message_p).enb_index = eNB_index;

          itti_send_msg_to_task (TASK_MAC_ENB, Mod_id, message_p);
        }
#endif

        memcpy(Buffer,Srb_info->Tx_buffer.Payload,Srb_info->Tx_buffer.payload_size);
	Sdu_size = Srb_info->Tx_buffer.payload_size;
	Srb_info->Tx_buffer.payload_size=0;
      }

      return (Sdu_size);
    }

#ifdef Rel10
    if((Srb_id & RAB_OFFSET) == MCCH){
      if(eNB_rrc_inst[Mod_id].MCCH_MESS[mbsfn_sync_area].Active==0) return 0; // this parameter is set in function init_mcch in rrc_eNB.c                                                                              
      // this part not needed as it is done in init_mcch 
      /*     if (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area] == 255) {
	LOG_E(RRC,"[eNB %d] MAC Request for MCCH MESSAGE and MCCH MESSAGE is not initialized\n",Mod_id);
	mac_xface->macphy_exit("");
	}*/


#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int mcch_size = eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area];
        int sdu_size = sizeof(RRC_MAC_MCCH_DATA_REQ (message_p).sdu);

        if (mcch_size > sdu_size) {
          LOG_E(RRC, "SDU larger than MCCH SDU buffer size (%d, %d)", mcch_size, sdu_size);
          mcch_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_ENB, RRC_MAC_MCCH_DATA_REQ);
        RRC_MAC_MCCH_DATA_REQ (message_p).frame = frame;
        RRC_MAC_MCCH_DATA_REQ (message_p).sdu_size = mcch_size;
        memcpy (RRC_MAC_MCCH_DATA_REQ (message_p).sdu, eNB_rrc_inst[Mod_id].MCCH_MESSAGE[mbsfn_sync_area], mcch_size);
        RRC_MAC_MCCH_DATA_REQ (message_p).enb_index = eNB_index;
        RRC_MAC_MCCH_DATA_REQ (message_p).mbsfn_sync_area = mbsfn_sync_area;

        itti_send_msg_to_task (TASK_MAC_ENB, Mod_id, message_p);
      }
#endif

      memcpy(&Buffer[0],
	     eNB_rrc_inst[Mod_id].MCCH_MESSAGE[mbsfn_sync_area],
	     eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area]);
      
#ifdef DEBUG_RRC
      LOG_D(RRC,"[eNB %d] Frame %d : MCCH request => MCCH_MESSAGE \n",Mod_id,frame);
      for (i=0;i<eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area];i++)
	LOG_T(RRC,"%x.",Buffer[i]);
      LOG_T(RRC,"\n");
#endif
      
      return (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE[mbsfn_sync_area]);
      //      }
      //else
      //return(0);
    }
#endif //Rel10    
  }

  else{   //This is an UE
#ifdef DEBUG_RRC
    LOG_D(RRC,"[UE %d] Frame %d Filling CCCH SRB_ID %d\n",Mod_id,frame,Srb_id);
    LOG_D(RRC,"[UE %d] Frame %d Buffer status %d,\n",Mod_id,frame, UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size);
#endif
    if( (UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size > 0) ) {

#if defined(ENABLE_ITTI)
      {
        MessageDef *message_p;
        int ccch_size = UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size;
        int sdu_size = sizeof(RRC_MAC_CCCH_DATA_REQ (message_p).sdu);

        if (ccch_size > sdu_size) {
          LOG_E(RRC, "SDU larger than CCCH SDU buffer size (%d, %d)", ccch_size, sdu_size);
          ccch_size = sdu_size;
        }

        message_p = itti_alloc_new_message (TASK_RRC_UE, RRC_MAC_CCCH_DATA_REQ);
        RRC_MAC_CCCH_DATA_REQ (message_p).frame = frame;
        RRC_MAC_CCCH_DATA_REQ (message_p).sdu_size = ccch_size;
        memcpy (RRC_MAC_CCCH_DATA_REQ (message_p).sdu, UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.Payload, ccch_size);
        RRC_MAC_CCCH_DATA_REQ (message_p).enb_index = eNB_index;

        itti_send_msg_to_task (TASK_MAC_UE, Mod_id + NB_eNB_INST, message_p);
      }
#endif

      memcpy(&Buffer[0],&UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.Payload[0],UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size);
      u8 Ret_size=UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size;
      //   UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size=0;
      UE_rrc_inst[Mod_id].Info[eNB_index].T300_active = 1;
      UE_rrc_inst[Mod_id].Info[eNB_index].T300_cnt = 0;
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
s8 mac_rrc_lite_data_ind(u8 Mod_id, u32 frame, u16 Srb_id, u8 *Sdu, u16 sdu_size,u8 eNB_flag,u8 eNB_index,u8 mbsfn_sync_area){
//-------------------------------------------------------------------------------------------//
  SRB_INFO *Srb_info;
  /*
  int si_window;
  */

  if(eNB_flag == 0){

    if(Srb_id == BCCH){
      
      LOG_T(RRC,"[UE %d] Received SDU for BCCH on SRB %d from eNB %d\n",Mod_id,Srb_id,eNB_index);

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
        RRC_MAC_BCCH_DATA_IND (message_p).frame = frame;
        RRC_MAC_BCCH_DATA_IND (message_p).sdu_size = sdu_size;
        memcpy (RRC_MAC_BCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
        RRC_MAC_BCCH_DATA_IND (message_p).enb_index = eNB_index;
        RRC_MAC_BCCH_DATA_IND (message_p).rsrq = 30 /* TODO change phy to report rspq */;
        RRC_MAC_BCCH_DATA_IND (message_p).rsrp = 45 /* TODO change phy to report rspp */;

        itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
      }
#else
      decode_BCCH_DLSCH_Message(Mod_id,frame,eNB_index,Sdu,sdu_size, 0, 0);
      /*
      if ((frame %2) == 0) {
	if (UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status == 0) {
	  LOG_D(RRC,"[UE %d] Frame %d : Received SIB1 from eNB %d (%d bytes)\n",Mod_id,frame,eNB_index,sdu_size);
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
	  si_window = (frame%UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod)/frame%UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize;
	  LOG_D(RRC,"[UE %d] Frame %d : Received SI (%d bytes), in window %d (SIperiod %d, SIwindowsize %d)\n",Mod_id,frame,sdu_size,si_window,UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod,UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize);
	  memcpy(UE_rrc_inst[Mod_id].SI[eNB_index],&Sdu[0],sdu_size);
	  if (decode_SI(Mod_id,frame,eNB_index,si_window)==0) {
	    LOG_D(RRC,"[UE %d] Frame %d :Decoded SI successfully\n",Mod_id,frame);
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
        LOG_T(RRC,"[UE %d] Received SDU for CCCH on SRB %d from eNB %d\n",Mod_id,Srb_id & RAB_OFFSET,eNB_index);

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
          RRC_MAC_CCCH_DATA_IND (message_p).frame = frame;
          RRC_MAC_CCCH_DATA_IND (message_p).sdu_size = sdu_size;
          memcpy (RRC_MAC_CCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
          RRC_MAC_CCCH_DATA_IND (message_p).enb_index = eNB_index;

          itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
      }
#else
        Srb_info = &UE_rrc_inst[Mod_id].Srb0[eNB_index];

        memcpy(Srb_info->Rx_buffer.Payload,Sdu,sdu_size);
        Srb_info->Rx_buffer.payload_size = sdu_size;
        rrc_ue_decode_ccch(Mod_id, frame, Srb_info, eNB_index);
#endif
      }
    }
      
#ifdef Rel10
    if ((Srb_id & RAB_OFFSET) == MCCH) {
      LOG_T(RRC,"[UE %d] Frame %d: Received SDU on MBSFN sync area %d for MCCH on SRB %d from eNB %d\n",
	    Mod_id,frame, mbsfn_sync_area, Srb_id & RAB_OFFSET,eNB_index);

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
        RRC_MAC_MCCH_DATA_IND (message_p).frame = frame;
        RRC_MAC_MCCH_DATA_IND (message_p).sdu_size = sdu_size;
        memcpy (RRC_MAC_MCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);
        RRC_MAC_MCCH_DATA_IND (message_p).enb_index = eNB_index;
        RRC_MAC_MCCH_DATA_IND (message_p).mbsfn_sync_area = mbsfn_sync_area;

        itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
      }
#else
      decode_MCCH_Message(Mod_id, frame, eNB_index, Sdu, sdu_size, mbsfn_sync_area);
#endif
    }
#endif // Rel10

  }

  else{  // This is an eNB
    Srb_info = &eNB_rrc_inst[Mod_id].Srb0;
    LOG_T(RRC,"[eNB %d] Received SDU for CCCH on SRB %d\n",Mod_id,Srb_info->Srb_id);
    
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
          RRC_MAC_CCCH_DATA_IND (message_p).frame = frame;
          RRC_MAC_CCCH_DATA_IND (message_p).sdu_size = sdu_size;
          memcpy (RRC_MAC_CCCH_DATA_IND (message_p).sdu, Sdu, sdu_size);

          itti_send_msg_to_task (TASK_RRC_ENB, Mod_id, message_p);
      }
#else
    //    msg("\n******INST %d Srb_info %p, Srb_id=%d****\n\n",Mod_id,Srb_info,Srb_info->Srb_id);
    memcpy(Srb_info->Rx_buffer.Payload,Sdu,6);
    rrc_eNB_decode_ccch(Mod_id,frame,Srb_info);
#endif
  }

  return(0);

}

//-------------------------------------------------------------------------------------------//
// this function is Not USED anymore
void mac_lite_sync_ind(u8 Mod_id,u8 Status){
//-------------------------------------------------------------------------------------------//
}

//-------------------------------------------------------------------------------------------//
u8 rrc_lite_data_req(u8 eNB_id, u8 UE_id, u32 frame, u8 eNB_flag, unsigned int rb_id, u32 muiP, u32 confirmP,
                     unsigned int sdu_size, u8* Buffer, u8 mode) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;
    // Uses a new buffer to avoid issue with PDCP buffer content that could be changed by PDCP (asynchronous message handling).
    u8 *message_buffer;

    message_buffer = itti_malloc (eNB_flag ? TASK_RRC_ENB : TASK_RRC_UE, eNB_flag ? TASK_PDCP_ENB : TASK_PDCP_UE, sdu_size);
    memcpy (message_buffer, Buffer, sdu_size);

    message_p = itti_alloc_new_message (eNB_flag ? TASK_RRC_ENB : TASK_RRC_UE, RRC_DCCH_DATA_REQ);
    RRC_DCCH_DATA_REQ (message_p).frame = frame;
    RRC_DCCH_DATA_REQ (message_p).enb_flag = eNB_flag;
    RRC_DCCH_DATA_REQ (message_p).rb_id = rb_id;
    RRC_DCCH_DATA_REQ (message_p).muip = muiP;
    RRC_DCCH_DATA_REQ (message_p).confirmp = confirmP;
    RRC_DCCH_DATA_REQ (message_p).sdu_size = sdu_size;
    RRC_DCCH_DATA_REQ (message_p).sdu_p = message_buffer;
    RRC_DCCH_DATA_REQ (message_p).mode = mode;
    RRC_DCCH_DATA_REQ (message_p).eNB_index = eNB_id;
    RRC_DCCH_DATA_REQ (message_p).ue_index = UE_id;

    itti_send_msg_to_task (eNB_flag ? TASK_PDCP_ENB : TASK_PDCP_UE, eNB_flag ? eNB_id : NB_eNB_INST + UE_id, message_p);
    return TRUE; // TODO should be changed to a CNF message later, currently RRC lite does not used the returned value anyway.

  }
#else
  return pdcp_data_req (eNB_id, UE_id, frame, eNB_flag, rb_id, muiP, confirmP, sdu_size, Buffer, mode);
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_data_ind(u8_t eNB_id, u8_t UE_id, u32 frame, u8 eNB_flag,u32 Srb_id, u32 sdu_size,u8 *Buffer){
//-------------------------------------------------------------------------------------------//
  u8 DCCH_index = Srb_id % NB_RB_MAX;
  u8_t Mod_id;

  if (eNB_flag == 0) {
    Mod_id = UE_id + NB_eNB_INST;
    LOG_N(RRC, "[UE %d] Frame %d: received a DCCH %d message on SRB %d with Size %d from eNB %d\n",
          UE_id, frame, DCCH_index,Srb_id-1,sdu_size, eNB_id);
  } else {
    Mod_id = eNB_id;
    LOG_N(RRC, "[eNB %d] Frame %d: received a DCCH %d message on SRB %d with Size %d from UE %d\n",
          eNB_id, frame, DCCH_index,Srb_id-1,sdu_size, UE_id);
  }

#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;
    // Uses a new buffer to avoid issue with PDCP buffer content that could be changed by PDCP (asynchronous message handling).
    u8 *message_buffer;

    message_buffer = itti_malloc (eNB_flag ? TASK_PDCP_ENB : TASK_PDCP_UE, eNB_flag ? TASK_RRC_ENB : TASK_RRC_UE, sdu_size);
    memcpy (message_buffer, Buffer, sdu_size);

    message_p = itti_alloc_new_message (eNB_flag ? TASK_PDCP_ENB : TASK_PDCP_UE, RRC_DCCH_DATA_IND);
    RRC_DCCH_DATA_IND (message_p).frame = frame;
    RRC_DCCH_DATA_IND (message_p).dcch_index = DCCH_index;
    RRC_DCCH_DATA_IND (message_p).sdu_size = sdu_size;
    RRC_DCCH_DATA_IND (message_p).sdu_p = message_buffer;
    RRC_DCCH_DATA_IND (message_p).ue_index = UE_id;
    RRC_DCCH_DATA_IND (message_p).eNB_index = eNB_id;

    itti_send_msg_to_task (eNB_flag ? TASK_RRC_ENB : TASK_RRC_UE, Mod_id, message_p);
  }
#else
  if (eNB_flag ==1) {
    rrc_eNB_decode_dcch(eNB_id,frame,DCCH_index,UE_id,Buffer,sdu_size);
  }
  else {
    rrc_ue_decode_dcch(UE_id,frame,DCCH_index,Buffer,eNB_id);
  }
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_in_sync_ind(u8 Mod_id, u32 frame, u16 eNB_index) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_IN_SYNC_IND);
    RRC_MAC_IN_SYNC_IND (message_p).frame = frame;
    RRC_MAC_IN_SYNC_IND (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
  }
#else
  UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt=0;
  if (UE_rrc_inst[Mod_id].Info[eNB_index].T310_active==1)
    UE_rrc_inst[Mod_id].Info[eNB_index].N311_cnt++;
#endif
}

//-------------------------------------------------------------------------------------------//
void rrc_lite_out_of_sync_ind(u8  Mod_id, u32 frame, u16 eNB_index){
//-------------------------------------------------------------------------------------------//
//  rlc_info_t rlc_infoP;
//  rlc_infoP.rlc_mode=RLC_UM;

  LOG_I(RRC,"[UE %d] Frame %d: OUT OF SYNC FROM eNB %d (T310 %d, N310 %d, N311 %d)\n ",
        Mod_id,frame,eNB_index,
        UE_rrc_inst[Mod_id].Info[eNB_index].T310_cnt,
        UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt,
        UE_rrc_inst[Mod_id].Info[eNB_index].N311_cnt);

#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_OUT_OF_SYNC_IND);
    RRC_MAC_OUT_OF_SYNC_IND (message_p).frame = frame;
    RRC_MAC_OUT_OF_SYNC_IND (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
  }
#else
  UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt++;
#endif
}

//-------------------------------------------------------------------------------------------//
int mac_get_rrc_lite_status(u8 Mod_id,u8 eNB_flag,u8 index){
//-------------------------------------------------------------------------------------------//
  if(eNB_flag == 1)
    return(eNB_rrc_inst[Mod_id].Info.UE[index].Status);
  else
    return(UE_rrc_inst[Mod_id].Info[index].State);
}

//-------------------------------------------------------------------------------------------//
int mac_ue_ccch_success_ind(u8 Mod_id, u8 eNB_index) {
//-------------------------------------------------------------------------------------------//
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_MAC_UE, RRC_MAC_CCCH_DATA_CNF);
    RRC_MAC_CCCH_DATA_CNF (message_p).enb_index = eNB_index;

    itti_send_msg_to_task (TASK_RRC_UE, Mod_id + NB_eNB_INST, message_p);
  }
#else
  // reset the tx buffer to indicate RRC that ccch was successfully transmitted (for example if contention resolution succeeds)
  UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size=0;
#endif
  return 0;
}
