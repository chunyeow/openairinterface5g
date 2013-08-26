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
* \author Raymond Knopp 
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

#ifdef PHY_EMUL
#include "SIMULATION/simulation_defs.h"
extern EMULATION_VARS *Emul_vars;
extern eNB_MAC_INST *eNB_mac_inst;
extern UE_MAC_INST *UE_mac_inst;
#endif

//#define RRC_DATA_REQ_DEBUG
//#define DEBUG_RRC

u32 mui=0;
//---------------------------------------------------------------------------------------------//

s8 mac_rrc_lite_data_req( u8 Mod_id,
			  u32 frame,
			  u16 Srb_id,
			  u8 Nb_tb,
			  char *Buffer,
			  u8 eNB_flag,
			  u8 eNB_index){
  //------------------------------------------------------------------------------------------------------------------//


  SRB_INFO *Srb_info;
  u8 Sdu_size=0;

#ifdef DEBUG_RRC
  int i;
  LOG_T(RRC,"[eNB %d] mac_rrc_data_req to SRB ID=%d\n",Mod_id,Srb_id);
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
	memcpy(Buffer,Srb_info->Tx_buffer.Payload,Srb_info->Tx_buffer.payload_size);
	Sdu_size = Srb_info->Tx_buffer.payload_size;
	Srb_info->Tx_buffer.payload_size=0;
      }

      return (Sdu_size);
    }

#ifdef Rel10
    if((Srb_id & RAB_OFFSET) == MCCH){
      if(eNB_rrc_inst[Mod_id].MCCH_MESS.Active==0) return 0; // this parameter is set in function init_mcch in rrc_eNB.c                                                                              
      if (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE == 255) {
	LOG_E(RRC,"[eNB %d] MAC Request for MCCH MESSAGE and MCCH MESSAGE is not initialized\n",Mod_id);
	mac_xface->macphy_exit("");
      }
      memcpy(&Buffer[0],eNB_rrc_inst[Mod_id].MCCH_MESSAGE,eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE);
      
#ifdef DEBUG_RRC
      LOG_D(RRC,"[eNB %d] Frame %d : MCCH request => MCCH_MESSAGE \n",Mod_id,frame);
      for (i=0;i<eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE;i++)
	LOG_T(RRC,"%x.",Buffer[i]);
      LOG_T(RRC,"\n");
#endif
      
      return (eNB_rrc_inst[Mod_id].sizeof_MCCH_MESSAGE);
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

//--------------------------------------------------------------------------------------------//
s8 mac_rrc_lite_data_ind(u8 Mod_id, u32 frame, u16 Srb_id, char *Sdu, u16 Sdu_len,u8 eNB_flag,u8 eNB_index ){
  //------------------------------------------------------------------------------------------//

  SRB_INFO *Srb_info;
  int si_window;

#ifdef DEBUG_RRC
  if (Srb_id == BCCH)
    msg("[RRC]Node =%d: mac_rrc_data_ind to SI, eNB_UE_INDEX %d...\n",Mod_id,eNB_index);
  else
    msg("[RRC]Node =%d: mac_rrc_data_ind to SRB ID=%d, eNB_UE_INDEX %d...\n",Mod_id,Srb_id,eNB_index);
#endif

  if(eNB_flag == 0){

    //LOG_D(RRC,"[RRC][UE %d] Received SDU for SRB %d\n",Mod_id,Srb_id);

    if(Srb_id == BCCH){

      decode_BCCH_DLSCH_Message(Mod_id,frame,eNB_index,Sdu,Sdu_len);
      /*
      if ((frame %2) == 0) {
	if (UE_rrc_inst[Mod_id].Info[eNB_index].SIB1Status == 0) {
	  LOG_D(RRC,"[UE %d] Frame %d : Received SIB1 from eNB %d (%d bytes)\n",Mod_id,frame,eNB_index,Sdu_len);
	  if (UE_rrc_inst[Mod_id].SIB1[eNB_index])
	    memcpy(UE_rrc_inst[Mod_id].SIB1[eNB_index],&Sdu[0],Sdu_len);
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
	  LOG_D(RRC,"[UE %d] Frame %d : Received SI (%d bytes), in window %d (SIperiod %d, SIwindowsize %d)\n",Mod_id,frame,Sdu_len,si_window,UE_rrc_inst[Mod_id].Info[eNB_index].SIperiod,UE_rrc_inst[Mod_id].Info[eNB_index].SIwindowsize);
	  memcpy(UE_rrc_inst[Mod_id].SI[eNB_index],&Sdu[0],Sdu_len);
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
    }


    if((Srb_id & RAB_OFFSET) == CCCH){
      Srb_info = &UE_rrc_inst[Mod_id].Srb0[eNB_index];
      
      if (Sdu_len>0) {
	memcpy(Srb_info->Rx_buffer.Payload,Sdu,Sdu_len);
	Srb_info->Rx_buffer.payload_size = Sdu_len;
	rrc_ue_decode_ccch(Mod_id,frame,Srb_info,eNB_index);
      }
    }
      
#ifdef Rel10
    if ((Srb_id & RAB_OFFSET) == MCCH) {

       decode_MCCH_Message(Mod_id, frame, eNB_index, Sdu, Sdu_len);
    }
#endif // Rel10

  }

  else{  // This is an eNB
    Srb_info = &eNB_rrc_inst[Mod_id].Srb0;
    //    msg("\n***********************************INST %d Srb_info %p, Srb_id=%d**********************************\n\n",Mod_id,Srb_info,Srb_info->Srb_id);
    memcpy(Srb_info->Rx_buffer.Payload,Sdu,6);
    rrc_eNB_decode_ccch(Mod_id,frame,Srb_info);
 }

  return(0);

}

//-------------------------------------------------------------------------------------------//
void mac_lite_sync_ind(u8 Mod_id,u8 Status){
//-------------------------------------------------------------------------------------------//
}

//------------------------------------------------------------------------------------------------------------------//
void rrc_lite_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag,u32 Srb_id, u32 sdu_size,u8 *Buffer){
    //------------------------------------------------------------------------------------------------------------------//

  u8 UE_index=(Srb_id-1)/NB_RB_MAX;
  u8 DCCH_index = Srb_id % NB_RB_MAX;

  LOG_D(RRC,"[SRB %d]RECEIVED MSG ON DCCH %d, UE %d, Size %d\n",
	Srb_id-1, DCCH_index,UE_index,sdu_size);
  if (eNB_flag ==1)
    rrc_eNB_decode_dcch(Mod_id,frame,DCCH_index,UE_index,Buffer,sdu_size);
  else
    rrc_ue_decode_dcch(Mod_id-NB_eNB_INST,frame,DCCH_index,Buffer,UE_index);

}

void rrc_lite_in_sync_ind(u8 Mod_id, u32 frame, u16 eNB_index) {

  UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt=0;
  if (UE_rrc_inst[Mod_id].Info[eNB_index].T310_active==1)
    UE_rrc_inst[Mod_id].Info[eNB_index].N311_cnt++;
}
/*-------------------------------------------------------------------------------------------*/
void rrc_lite_out_of_sync_ind(u8  Mod_id, u32 frame, u16 eNB_index){
/*-------------------------------------------------------------------------------------------*/


//  rlc_info_t rlc_infoP;
//  rlc_infoP.rlc_mode=RLC_UM;

  LOG_D(RRC,"[UE %d] Frame %d OUT OF SYNC FROM CH %d (T310 %d, N310 %d, N311 %d)\n ",Mod_id,frame,eNB_index,
	UE_rrc_inst[Mod_id].Info[eNB_index].T310_cnt,
	UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt,
	UE_rrc_inst[Mod_id].Info[eNB_index].N311_cnt);

  UE_rrc_inst[Mod_id].Info[eNB_index].N310_cnt++;

}

int mac_get_rrc_lite_status(u8 Mod_id,u8 eNB_flag,u8 index){
  if(eNB_flag == 1)
    return(eNB_rrc_inst[Mod_id].Info.Status[index]);
  else
    return(UE_rrc_inst[Mod_id].Info[index].State);
}


int mac_ue_ccch_success_ind(u8 Mod_id, u8 eNB_index) {
  // reset the tx buffer to indicate RRC that ccch was successfully transmitted (for example if contention resolution succeeds)
  UE_rrc_inst[Mod_id].Srb0[eNB_index].Tx_buffer.payload_size=0;
  return 0;
}
