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
/*! \file eNB_scheduler.c
 * \brief procedures related to UE
 * \author Raymond Knopp, Navid Nikaein
 * \date 2011
 * \version 0.5
 * @ingroup _mac

 */

#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/from_grlib_softregs.h"

#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"

//#include "LAYER2/MAC/pre_processor.c"
#include "pdcp.h"

double snr_mcs[28]={-4.1130, -3.3830, -2.8420, -2.0480, -1.3230, -0.6120, 0.1080, 0.977, 1.7230, 2.7550, 3.1960, 3.8080, 4.6870, 5.6840, 6.6550, 7.7570, 8.3730, 9.3040, 9.5990, 10.9020, 11.7220, 12.5950, 13.4390, 14.8790, 15.8800, 16.4800, 17.8690, 18.7690};
int cqi_mcs[3][16] = {{0, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 20, 22, 25, 27, 27},{0, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 20, 22, 25, 27, 27},{0, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 19, 22, 24, 26, 27}};
double cqi_snr[16] = {-8, -7, -6.08, -5.252, -3.956, -2.604, -1.107, 0.87, 2.599, 4.79, 6.716, 8, 10.593, 13.286, 15.745, 18.5};
double snr_tm6=0;


#define ENABLE_MAC_PAYLOAD_DEBUG
#define DEBUG_eNB_SCHEDULER 1
//#define DEBUG_HEADER_PARSING 1
//#define DEBUG_PACKET_TRACE 1

/*
  #ifndef USER_MODE
  #define msg debug_msg
  #endif
*/

extern inline unsigned int taus(void);


void init_ue_sched_info(void){
  int i,j;
  for (i=0;i<NUMBER_OF_eNB_MAX;i++){
    for (j=0;j<NUMBER_OF_UE_MAX;j++){
      // init DL
      eNB_dlsch_info[i][j].weight           = 0;
      eNB_dlsch_info[i][j].subframe         = 0;
      eNB_dlsch_info[i][j].serving_num      = 0;
      eNB_dlsch_info[i][j].status           = S_DL_NONE;
      // init UL
      eNB_ulsch_info[i][j].subframe         = 0;
      eNB_ulsch_info[i][j].serving_num      = 0;
      eNB_ulsch_info[i][j].status           = S_UL_NONE;
    }
  }
}

void add_ue_ulsch_info(unsigned char Mod_id, unsigned char UE_id, unsigned char subframe, UE_ULSCH_STATUS status){

  eNB_ulsch_info[Mod_id][UE_id].rnti             = find_UE_RNTI(Mod_id,UE_id);
  eNB_ulsch_info[Mod_id][UE_id].subframe         = subframe;
  eNB_ulsch_info[Mod_id][UE_id].status           = status;

  eNB_ulsch_info[Mod_id][UE_id].serving_num++;

}
void add_ue_dlsch_info(unsigned char Mod_id, unsigned char UE_id, unsigned char subframe, UE_DLSCH_STATUS status){

  eNB_dlsch_info[Mod_id][UE_id].rnti             = find_UE_RNTI(Mod_id,UE_id);
  //  eNB_dlsch_info[Mod_id][UE_id].weight           = weight;
  eNB_dlsch_info[Mod_id][UE_id].subframe         = subframe;
  eNB_dlsch_info[Mod_id][UE_id].status           = status;

  eNB_dlsch_info[Mod_id][UE_id].serving_num++;

}

unsigned char get_ue_weight(unsigned char Mod_id, unsigned char UE_id){

  return(eNB_dlsch_info[Mod_id][UE_id].weight);

}

// return is rnti ???
unsigned char schedule_next_ulue(unsigned char Mod_id, unsigned char UE_id, unsigned char subframe){

  unsigned char next_ue;

  // first phase: scheduling for ACK
  switch (subframe) {
    // scheduling for subframe 2: for scheduled user during subframe 5 and 6
  case 8:
    if  ((eNB_dlsch_info[Mod_id][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[Mod_id][UE_id].subframe == 5 || eNB_dlsch_info[Mod_id][UE_id].subframe == 6)){
      // set the downlink status
      eNB_dlsch_info[Mod_id][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
    // scheduling for subframe 3: for scheduled user during subframe 7 and 8
  case 9:
    if  ((eNB_dlsch_info[Mod_id][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[Mod_id][UE_id].subframe == 7 || eNB_dlsch_info[Mod_id][UE_id].subframe == 8)){
      eNB_dlsch_info[Mod_id][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
    // scheduling UL subframe 4: for scheduled user during subframe 9 and 0
  case 0 :
    if  ((eNB_dlsch_info[Mod_id][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[Mod_id][UE_id].subframe == 9 || eNB_dlsch_info[Mod_id][UE_id].subframe == 0)){
      eNB_dlsch_info[Mod_id][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
  default:
    break;
  }

  // second phase
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ ){

    if  (eNB_ulsch_info[Mod_id][next_ue].status == S_UL_WAITING )
      return next_ue;
    else if (eNB_ulsch_info[Mod_id][next_ue].status == S_UL_SCHEDULED){
      eNB_ulsch_info[Mod_id][next_ue].status = S_UL_BUFFERED;
    }
  }
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ ){
    if (eNB_ulsch_info[Mod_id][next_ue].status != S_UL_NONE )// do this just for active UEs
      eNB_ulsch_info[Mod_id][next_ue].status = S_UL_WAITING;
  }
  next_ue = 0;
  return next_ue;

}

unsigned char schedule_next_dlue(unsigned char Mod_id, unsigned char subframe){

  unsigned char next_ue;

  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ ){
    if  (eNB_dlsch_info[Mod_id][next_ue].status == S_DL_WAITING)
      return next_ue;
  }
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ )
    if  (eNB_dlsch_info[Mod_id][next_ue].status == S_DL_BUFFERED) {
      eNB_dlsch_info[Mod_id][next_ue].status = S_DL_WAITING;
    }
  // next_ue = -1;
  return (-1);//next_ue;

}

void initiate_ra_proc(u8 Mod_id, u32 frame, u16 preamble_index,s16 timing_offset,u8 sect_id,u8 subframe,u8 f_id) {

  u8 i;

  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Initiating RA procedure for preamble index %d\n",Mod_id,frame,preamble_index);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
    if (eNB_mac_inst[Mod_id].RA_template[i].RA_active==0) {
      eNB_mac_inst[Mod_id].RA_template[i].RA_active=1;
      eNB_mac_inst[Mod_id].RA_template[i].generate_rar=1;
      eNB_mac_inst[Mod_id].RA_template[i].generate_Msg4=0;
      eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4=0;
      eNB_mac_inst[Mod_id].RA_template[i].timing_offset=timing_offset;
      // Put in random rnti (to be replaced with proper procedure!!)
      eNB_mac_inst[Mod_id].RA_template[i].rnti = taus();
      eNB_mac_inst[Mod_id].RA_template[i].RA_rnti = 1+subframe+(10*f_id);
      eNB_mac_inst[Mod_id].RA_template[i].preamble_index = preamble_index;
      LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Activating RAR generation for process %d, rnti %x, RA_active %d\n",
	    Mod_id,frame,i,eNB_mac_inst[Mod_id].RA_template[i].rnti,
	    eNB_mac_inst[Mod_id].RA_template[i].RA_active);
      
      return;
    }
  }
}

void cancel_ra_proc(u8 Mod_id, u32 frame, u16 rnti) {
  unsigned char i;
  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Cancelling RA procedure for UE rnti %x\n",Mod_id,frame,rnti);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
    if (rnti == eNB_mac_inst[Mod_id].RA_template[i].rnti) {
      eNB_mac_inst[Mod_id].RA_template[i].RA_active=0;
      eNB_mac_inst[Mod_id].RA_template[i].generate_rar=0;
      eNB_mac_inst[Mod_id].RA_template[i].generate_Msg4=0;
      eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4=0;
      eNB_mac_inst[Mod_id].RA_template[i].timing_offset=0;
      eNB_mac_inst[Mod_id].RA_template[i].RRC_timer=20;
      eNB_mac_inst[Mod_id].RA_template[i].rnti = 0;
    }
  }
}

void terminate_ra_proc(u8 Mod_id,u32 frame,u16 rnti,unsigned char *msg3, u16 msg3_len) {

  unsigned char rx_ces[MAX_NUM_CE],num_ce,num_sdu,i,*payload_ptr;
  unsigned char rx_lcids[NB_RB_MAX];
  u16 rx_lengths[NB_RB_MAX];
  s8 UE_id;

  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, Received msg3 %x.%x.%x.%x.%x.%x, Terminating RA procedure for UE rnti %x\n",
	Mod_id,frame,
	msg3[3],msg3[4],msg3[5],msg3[6],msg3[7], msg3[8], rnti);

  for (i=0;i<NB_RA_PROC_MAX;i++) {
    LOG_D(MAC,"[RAPROC] Checking proc %d : rnti (%x, %x), active %d\n",i,
	  eNB_mac_inst[Mod_id].RA_template[i].rnti, rnti,
	  eNB_mac_inst[Mod_id].RA_template[i].RA_active);
    if ((eNB_mac_inst[Mod_id].RA_template[i].rnti==rnti) &&
	(eNB_mac_inst[Mod_id].RA_template[i].RA_active==1)) {

      payload_ptr = parse_ulsch_header(msg3,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,msg3_len);
      LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Received CCCH: length %d, offset %d\n",
	    Mod_id,frame,rx_lengths[0],payload_ptr-msg3);
      if ((num_ce == 0) && (num_sdu==1) && (rx_lcids[0] == CCCH)) { // This is an RRCConnectionRequest/Restablishment
	memcpy(&eNB_mac_inst[Mod_id].RA_template[i].cont_res_id[0],payload_ptr,6);
	LOG_D(MAC,"[eNB %d][RAPROC] Frame %d Received CCCH: length %d, offset %d\n",
	      Mod_id,frame,rx_lengths[0],payload_ptr-msg3);
	UE_id=add_new_ue(Mod_id,eNB_mac_inst[Mod_id].RA_template[i].rnti);
	if (UE_id==-1) {
	  mac_xface->macphy_exit("[MAC][eNB] Max user count reached\n");
	}
	else {
	  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Added user with rnti %x => UE %d\n",
		Mod_id,frame,eNB_mac_inst[Mod_id].RA_template[i].rnti,UE_id);
	}

	if (Is_rrc_registered == 1)
	  mac_rrc_data_ind(Mod_id,frame,CCCH,(char *)payload_ptr,rx_lengths[0],1,Mod_id);
	// add_user.  This is needed to have the rnti for configuring UE (PHY). The UE is removed if RRC
	// doesn't provide a CCCH SDU

      }
      else if (num_ce >0) {  // handle msg3 which is not RRCConnectionRequest
	//	process_ra_message(msg3,num_ce,rx_lcids,rx_ces);
      }
 
      eNB_mac_inst[Mod_id].RA_template[i].generate_Msg4 = 1;
      eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4 = 0;

      return;
    } // if process is active

  } // loop on RA processes
}

DCI_PDU *get_dci_sdu(u8 Mod_id, u32 frame, u8 subframe) {

  return(&eNB_mac_inst[Mod_id].DCI_pdu);

}

s8 find_UE_id(unsigned char Mod_id,u16 rnti) {

  unsigned char i;

  for (i=0;i<NUMBER_OF_UE_MAX;i++) {
    if (eNB_mac_inst[Mod_id].UE_template[i].rnti==rnti) {
      return(i);
    }
  }
  return(-1);

}

s16 find_UE_RNTI(unsigned char Mod_id, unsigned char UE_id) {

  return (eNB_mac_inst[Mod_id].UE_template[UE_id].rnti);

}
u8 is_UE_active(unsigned char Mod_id, unsigned char UE_id ){
  if (eNB_mac_inst[Mod_id].UE_template[UE_id].rnti !=0 )
    return 1;
  else
    return 0 ;
}
s8 find_active_UEs(unsigned char Mod_id){

  unsigned char UE_id;
  u16 rnti;
  unsigned char nb_active_ue=0;

  for (UE_id=0;UE_id<NUMBER_OF_UE_MAX;UE_id++) {

    if (((rnti=eNB_mac_inst[Mod_id].UE_template[UE_id].rnti) !=0)&&(eNB_mac_inst[Mod_id].UE_template[UE_id].ul_active==1)){

      if (mac_xface->get_eNB_UE_stats(Mod_id,rnti) != NULL){ // check at the phy enb_ue state for this rnti
	nb_active_ue++;
      }
      else { // this ue is removed at the phy => remove it at the mac as well
	mac_remove_ue(Mod_id, UE_id);
      }
    }
  }
  return(nb_active_ue);
}

// function for determining which active ue should be granted resources in uplink based on BSR+QoS
u16 find_ulgranted_UEs(unsigned char Mod_id){

  // all active users should be granted
  return(find_active_UEs(Mod_id));
}

// function for determining which active ue should be granted resources in downlink based on CQI, SI, and BSR
u16 find_dlgranted_UEs(unsigned char Mod_id){

  // all active users should be granted
  return(find_active_UEs(Mod_id));
}
// get aggregatiob form phy for a give UE
unsigned char process_ue_cqi (unsigned char Mod_id, unsigned char UE_id) {

  unsigned char aggregation=2;
  // check the MCS and SNR and set the aggregation accordingly

  return aggregation;
}
#ifdef CBA
u8 find_num_active_UEs_in_cbagroup(unsigned char Mod_id, unsigned char group_id){

  u8 UE_id;
  u16 rnti;
  unsigned char nb_ue_in_pusch=0;
  LTE_eNB_UE_stats* eNB_UE_stats;
  
  for (UE_id=group_id;UE_id<NUMBER_OF_UE_MAX;UE_id+=eNB_mac_inst[Mod_id].num_active_cba_groups) {

    if (((rnti=eNB_mac_inst[Mod_id].UE_template[UE_id].rnti) !=0) && 
	(eNB_mac_inst[Mod_id].UE_template[UE_id].ul_active==1)    && 
	(mac_get_rrc_status(Mod_id,1,UE_id) > RRC_CONNECTED)){
      //  && (UE_is_to_be_scheduled(Mod_id,UE_id))) 
      // check at the phy enb_ue state for this rnti
      if ((eNB_UE_stats= mac_xface->get_eNB_UE_stats(Mod_id,rnti)) != NULL){ 
	if ((eNB_UE_stats->mode == PUSCH) && (UE_is_to_be_scheduled(Mod_id,UE_id) == 0)){ 
	  nb_ue_in_pusch++;
	}
      }
    }
  }
  return(nb_ue_in_pusch);
}
#endif 
s8 add_new_ue(unsigned char Mod_id, u16 rnti) {
  unsigned char i;

  for (i=0;i<NUMBER_OF_UE_MAX;i++) {
    if (eNB_mac_inst[Mod_id].UE_template[i].rnti==0) {
      eNB_mac_inst[Mod_id].UE_template[i].rnti=rnti;
      eNB_ulsch_info[Mod_id][i].status = S_UL_WAITING;
      eNB_dlsch_info[Mod_id][i].status = S_UL_WAITING;
      LOG_D(MAC,"[eNB] Add UE_id %d : rnti %x\n",i,eNB_mac_inst[Mod_id].UE_template[i].rnti);
      return((s8)i);
    }
  }
  return(-1);
}

s8 mac_remove_ue(unsigned char Mod_id, unsigned char UE_id) {

  LOG_I(MAC,"Removing UE %d (rnti %x)\n",UE_id,eNB_mac_inst[Mod_id].UE_template[UE_id].rnti);

  // clear all remaining pending transmissions
  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]  = 0;
  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]  = 0;
  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]  = 0;
  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]  = 0;

  eNB_mac_inst[Mod_id].UE_template[UE_id].ul_SR           = 0;
  eNB_mac_inst[Mod_id].UE_template[UE_id].rnti            = 0;
  eNB_mac_inst[Mod_id].UE_template[UE_id].ul_active       = 0;
  eNB_ulsch_info[Mod_id][UE_id].rnti          = 0;
  eNB_ulsch_info[Mod_id][UE_id].status        = S_UL_NONE;
  eNB_dlsch_info[Mod_id][UE_id].rnti          = 0;
  eNB_dlsch_info[Mod_id][UE_id].status        = S_DL_NONE;

  rrc_remove_UE(Mod_id,UE_id);

  return(1);
}

unsigned char *get_dlsch_sdu(u8 Mod_id,u32 frame,u16 rnti,u8 TBindex) {

  unsigned char UE_id;

  if (rnti==SI_RNTI) {
    LOG_D(MAC,"[eNB %d] Frame %d Get DLSCH sdu for BCCH \n",Mod_id,frame);

    return((unsigned char *)&eNB_mac_inst[Mod_id].BCCH_pdu.payload[0]);
  }
  else {

    UE_id = find_UE_id(Mod_id,rnti);
    LOG_D(MAC,"[eNB %d] Frame %d  Get DLSCH sdu for rnti %x => UE_id %d\n",Mod_id,frame,rnti,UE_id);

    return((unsigned char *)&eNB_mac_inst[Mod_id].DLSCH_pdu[UE_id][TBindex].payload[0]);
  }

}

unsigned char *parse_ulsch_header(unsigned char *mac_header,
				  unsigned char *num_ce,
				  unsigned char *num_sdu,
				  unsigned char *rx_ces,
				  unsigned char *rx_lcids,
				  unsigned short *rx_lengths,
				  unsigned short tb_length) {

  unsigned char not_done=1,num_ces=0,num_sdus=0,lcid,num_sdu_cnt;
  unsigned char *mac_header_ptr = mac_header;
  unsigned short length, ce_len=0;

  while (not_done==1) {

    if (((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E == 0)
      not_done = 0;

    lcid = ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID;
    if (lcid < EXTENDED_POWER_HEADROOM) { 
      if (not_done==0) { // last MAC SDU, length is implicit
	mac_header_ptr++;
	length = tb_length-(mac_header_ptr-mac_header)-ce_len;	
	for (num_sdu_cnt=0; num_sdu_cnt < num_sdus ; num_sdu_cnt++)
	  length -= rx_lengths[num_sdu_cnt]; 
      }
      else {
	if (((SCH_SUBHEADER_SHORT *)mac_header_ptr)->F == 0) {
	  length = ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->L;
	  mac_header_ptr += 2;//sizeof(SCH_SUBHEADER_SHORT);
	}
	else { // F = 1 
	  length = ((((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_MSB & 0x7f ) << 8 ) | (((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_LSB & 0xff);
	  mac_header_ptr += 3;//sizeof(SCH_SUBHEADER_LONG);
	}
      }
      LOG_D(MAC,"[eNB] sdu %d lcid %d tb_length %d length %d (offset now %d)\n",
	    num_sdus,lcid,tb_length, length,mac_header_ptr-mac_header);
      rx_lcids[num_sdus] = lcid;
      rx_lengths[num_sdus] = length;
      num_sdus++;
    }
    else {  // This is a control element subheader POWER_HEADROOM, BSR and CRNTI
      if (lcid == SHORT_PADDING) {
	mac_header_ptr++;
      }
      else {
	rx_ces[num_ces] = lcid;
	num_ces++;
	mac_header_ptr++;	
	if (lcid==LONG_BSR)
	  ce_len+=4;
	else if (lcid==CRNTI)
	  ce_len+=2;
	else if ((lcid==POWER_HEADROOM) || (lcid==TRUNCATED_BSR)|| (lcid== SHORT_BSR))
	  ce_len++;
      }
    }
  }
  *num_ce = num_ces;
  *num_sdu = num_sdus;

  return(mac_header_ptr);
}

void SR_indication(u8 Mod_id,u32 frame, u16 rnti, u8 subframe) {

  u8 UE_id = find_UE_id(Mod_id,rnti);

  LOG_D(MAC,"[eNB %d][SR %x] Frame %d subframe %d Signaling SR for UE %d \n",Mod_id,rnti,frame,subframe, UE_id);
  eNB_mac_inst[Mod_id].UE_template[UE_id].ul_SR = 1;
  eNB_mac_inst[Mod_id].UE_template[UE_id].ul_active = 1;
}

void rx_sdu(u8 Mod_id,u32 frame,u16 rnti,u8 *sdu, u16 sdu_len) {

  unsigned char rx_ces[MAX_NUM_CE],num_ce,num_sdu,i,*payload_ptr;
  unsigned char rx_lcids[NB_RB_MAX];
  unsigned short rx_lengths[NB_RB_MAX];
  unsigned char UE_id = find_UE_id(Mod_id,rnti);
  int ii,j;
  for(ii=0; ii<NB_RB_MAX; ii++) rx_lengths[ii] = 0;
  
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,1);

  eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].total_pdu_bytes_rx+=sdu_len;
  eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].total_num_pdus_rx+=1;

  LOG_D(MAC,"[eNB %d] Received ULSCH sdu from PHY (rnti %x, UE_id %d), parsing header\n",Mod_id,rnti,UE_id);
  payload_ptr = parse_ulsch_header(sdu,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,sdu_len);

  // control element
  for (i=0;i<num_ce;i++) {

    switch (rx_ces[i]) { // implement and process BSR + CRNTI +
    case POWER_HEADROOM:
      eNB_mac_inst[Mod_id].UE_template[UE_id].phr_info =  (payload_ptr[0] & 0x3f);// - PHR_MAPPING_OFFSET; 
      LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received PHR PH = %d (db)\n", rx_ces[i], eNB_mac_inst[Mod_id].UE_template[UE_id].phr_info);
      payload_ptr+=sizeof(POWER_HEADROOM_CMD);
      break;
    case CRNTI:
      LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received CRNTI %d \n", rx_ces[i], payload_ptr[0]);
      payload_ptr+=1;
      break;
    case TRUNCATED_BSR:
    case SHORT_BSR: {
      u8 lcgid;

      lcgid = (payload_ptr[0] >> 6);
      LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received short BSR LCGID = %u bsr = %d\n",
            rx_ces[i], lcgid, payload_ptr[0] & 0x3f);
      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[lcgid] = (payload_ptr[0] & 0x3f);
      payload_ptr += 1;//sizeof(SHORT_BSR); // fixme
    } break;
    case LONG_BSR:
      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0] = ((payload_ptr[0] & 0xFC) >> 2);
      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1] =
	((payload_ptr[0] & 0x03) << 4) | ((payload_ptr[1] & 0xF0) >> 4);
      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2] =
	((payload_ptr[1] & 0x0F) << 2) | ((payload_ptr[2] & 0xC0) >> 6);
      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3] = (payload_ptr[2] & 0x3F);
      LOG_D(MAC, "[eNB] MAC CE_LCID %d: Received long BSR LCGID0 = %u LCGID1 = "
	    "%u LCGID2 = %u LCGID3 = %u\n",
            rx_ces[i],
            eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0],
            eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1],
            eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2],
            eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]);
      break;
    default:
      LOG_E(MAC, "[eNB] Received unknown MAC header (0x%02x)\n", rx_ces[i]);
      break;
    }
  }

  for (i=0;i<num_sdu;i++) {
    LOG_D(MAC,"SDU Number %d MAC Subheader SDU_LCID %d, length %d\n",i,rx_lcids[i],rx_lengths[i]);
   
    if ((rx_lcids[i] == DCCH)||(rx_lcids[i] == DCCH1)) {
      //      if(eNB_mac_inst[Mod_id].Dcch_lchan[UE_id].Active==1){

#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
      for (j=0;j<32;j++)
        LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n");
#endif

      //  This check is just to make sure we didn't get a bogus SDU length, to be removed ...
      if (rx_lengths[i]<CCCH_PAYLOAD_SIZE_MAX) {
	LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DCCH, received %d bytes form UE %d on LCID %d(%d) \n",
	      Mod_id,frame, rx_lengths[i], UE_id, rx_lcids[i], rx_lcids[i]+(UE_id*NB_RB_MAX));

	mac_rlc_data_ind(Mod_id,frame,1,RLC_MBMS_NO,
			 rx_lcids[i]+(UE_id*NB_RB_MAX),
			 (char *)payload_ptr,
			 rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].num_pdu_rx[rx_lcids[i]]+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];

      }
      //      }
    } else if (rx_lcids[i] >= DTCH) {
      //      if(eNB_mac_inst[Mod_id].Dcch_lchan[UE_id].Active==1){

#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
     
      for (j=0;j<32;j++)
        LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n"); 
#endif

      LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DTCH, received %d bytes from UE %d for lcid %d (%d)\n",
	    Mod_id,frame, rx_lengths[i], UE_id,rx_lcids[i],rx_lcids[i]+(UE_id*NB_RB_MAX));

      if ((rx_lengths[i] <SCH_PAYLOAD_SIZE_MAX) &&  (rx_lengths[i] > 0) ) {   // MAX SIZE OF transport block
	mac_rlc_data_ind(Mod_id,frame,1,RLC_MBMS_NO,
			 DTCH+(UE_id*NB_RB_MAX),
			 (char *)payload_ptr,
			 rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].num_pdu_rx[rx_lcids[i]]+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];
	
      }
      //      }
    } else {
      eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].num_errors_rx+=1;
      LOG_E(MAC,"[eNB %d] received unknown LCID %d from UE %d ", rx_lcids[i], UE_id);
    }
    
    payload_ptr+=rx_lengths[i];
  }

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,0);

}

unsigned char generate_dlsch_header(unsigned char *mac_header,
				    unsigned char num_sdus,
				    unsigned short *sdu_lengths,
				    unsigned char *sdu_lcids,
				    unsigned char drx_cmd,
				    short timing_advance_cmd,
				    unsigned char *ue_cont_res_id,
				    unsigned char short_padding,
				    unsigned short post_padding) {

  SCH_SUBHEADER_FIXED *mac_header_ptr = (SCH_SUBHEADER_FIXED *)mac_header;
  u8 first_element=0,last_size=0,i;
  u8 mac_header_control_elements[16],*ce_ptr;

  ce_ptr = &mac_header_control_elements[0];

  // compute header components
  
  if ((short_padding == 1) || (short_padding == 2)) {
    mac_header_ptr->R    = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    first_element=1;
    last_size=1;
  }
  if (short_padding == 2) {
    mac_header_ptr->E = 1;
    mac_header_ptr++;
    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    last_size=1;
  }

  if (drx_cmd != 255) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      mac_header_ptr++;
    }
    else {
      first_element=1;
    }
    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = DRX_CMD;
    last_size=1;
  }

  if (timing_advance_cmd != 0) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      mac_header_ptr++;
    }
    else {
      first_element=1;
    }
    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = TIMING_ADV_CMD;
    last_size=1;
    //    msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);
    ((TIMING_ADVANCE_CMD *)ce_ptr)->R=0;
    ((TIMING_ADVANCE_CMD *)ce_ptr)->TA=timing_advance_cmd&0x3f;
    LOG_I(MAC,"timing advance =%d (%d)\n",timing_advance_cmd,((TIMING_ADVANCE_CMD *)ce_ptr)->TA);
    ce_ptr+=sizeof(TIMING_ADVANCE_CMD);
    //msg("offset %d\n",ce_ptr-mac_header_control_elements);
  }

  if (ue_cont_res_id) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      /*   
	   printf("[eNB][MAC] last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
	   ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
	   ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
	   ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);
      */
      mac_header_ptr++;
    }
    else {
      first_element=1;
    }

    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = UE_CONT_RES;
    last_size=1;
    
    LOG_T(MAC,"[eNB ][RAPROC] Generate contention resolution msg: %x.%x.%x.%x.%x.%x\n",
	  ue_cont_res_id[0],
	  ue_cont_res_id[1],
	  ue_cont_res_id[2],
	  ue_cont_res_id[3],
	  ue_cont_res_id[4],
	  ue_cont_res_id[5]);
    
    memcpy(ce_ptr,ue_cont_res_id,6);
    ce_ptr+=6;
    // msg("(cont_res) : offset %d\n",ce_ptr-mac_header_control_elements);
  }

  //msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);

  for (i=0;i<num_sdus;i++) {
    LOG_T(MAC,"[eNB] Generate DLSCH header num sdu %d len sdu %d\n",num_sdus, sdu_lengths[i]);

    if (first_element>0) {
      mac_header_ptr->E = 1;
      /*msg("last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
	((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
	((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
	((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);
      */
      mac_header_ptr+=last_size;
      //msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);
    }
    else {
      first_element=1;
    }
    if (sdu_lengths[i] < 128) {
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->R    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->E    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->F    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->LCID = sdu_lcids[i];
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->L    = (unsigned char)sdu_lengths[i];
      last_size=2;
    }
    else {
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->R    = 0;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->E    = 0;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->F    = 1;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->LCID = sdu_lcids[i];
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_MSB    = ((unsigned short) sdu_lengths[i]>>8)&0x7f;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_LSB    = (unsigned short) sdu_lengths[i]&0xff;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->padding   = 0x00;
      last_size=3;
#ifdef DEBUG_HEADER_PARSING
      LOG_D(MAC,"[eNB] generate long sdu, size %x (MSB %x, LSB %x)\n", 
	    sdu_lengths[i],
	    ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_MSB,
	    ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_LSB);
#endif
    }
  }
  /*

    printf("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);
  
    printf("last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);


    if (((SCH_SUBHEADER_FIXED*)mac_header_ptr)->LCID < UE_CONT_RES) {
    if (((SCH_SUBHEADER_SHORT*)mac_header_ptr)->F == 0)
    printf("F = 0, sdu len (L field) %d\n",(((SCH_SUBHEADER_SHORT*)mac_header_ptr)->L));
    else
    printf("F = 1, sdu len (L field) %d\n",(((SCH_SUBHEADER_LONG*)mac_header_ptr)->L));
    }
  */
  if (post_padding>0) {// we have lots of padding at the end of the packet
    mac_header_ptr->E = 1;
    mac_header_ptr+=last_size;
    // add a padding element
    mac_header_ptr->R    = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    mac_header_ptr++;
  }
  else { // no end of packet padding
    // last SDU subhead is of fixed type (sdu length implicitly to be computed at UE)
    mac_header_ptr++;
  }

  //msg("After subheaders %d\n",(u8*)mac_header_ptr - mac_header);
  
  if ((ce_ptr-mac_header_control_elements) > 0) {
    // printf("Copying %d bytes for control elements\n",ce_ptr-mac_header_control_elements);
    memcpy((void*)mac_header_ptr,mac_header_control_elements,ce_ptr-mac_header_control_elements);
    mac_header_ptr+=(unsigned char)(ce_ptr-mac_header_control_elements);
  }
  //msg("After CEs %d\n",(u8*)mac_header_ptr - mac_header);

  return((unsigned char*)mac_header_ptr - mac_header);

}

/*
  #ifdef Rel10
  unsigned char generate_mch_header( unsigned char *mac_header,
  unsigned char num_sdus,
  unsigned short *sdu_lengths,
  unsigned char *sdu_lcids,
  unsigned char msi,
  unsigned char short_padding,
  unsigned short post_padding) {

  SCH_SUBHEADER_FIXED *mac_header_ptr = (SCH_SUBHEADER_FIXED *)mac_header;
  u8 first_element=0,last_size=0,i;
  u8 mac_header_control_elements[2*num_sdus],*ce_ptr;

  ce_ptr = &mac_header_control_elements[0];

  if ((short_padding == 1) || (short_padding == 2)) {
  mac_header_ptr->R    = 0;
  mac_header_ptr->E    = 0;
  mac_header_ptr->LCID = SHORT_PADDING;
  first_element=1;
  last_size=1;
  }
  if (short_padding == 2) {
  mac_header_ptr->E = 1;
  mac_header_ptr++;
  mac_header_ptr->R = 0;
  mac_header_ptr->E    = 0;
  mac_header_ptr->LCID = SHORT_PADDING;
  last_size=1;
  }

  // SUBHEADER for MSI CE
  if (msi != 0) {// there is MSI MAC Control Element
  if (first_element>0) {
  mac_header_ptr->E = 1;
  mac_header_ptr+=last_size;
  }
  else {
  first_element = 1;
  }
  if (num_sdus*2 < 128) {
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->R    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->E    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->F    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->LCID = MCH_SCHDL_INFO;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->L    = num_sdus*2;
  last_size=2;
  }
  else {
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->R    = 0;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->E    = 0;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->F    = 1;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->LCID = MCH_SCHDL_INFO;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L    = (num_sdus*2)&0x7fff;
  last_size=3;
  }
  // Create the MSI MAC Control Element here
  }
 
  // SUBHEADER for MAC SDU (MCCH+MTCHs)
  for (i=0;i<num_sdus;i++) {
  if (first_element>0) {
  mac_header_ptr->E = 1;
  mac_header_ptr+=last_size;
  }
  else {
  first_element = 1;
  }
  if (sdu_lengths[i] < 128) {
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->R    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->E    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->F    = 0;
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->LCID = sdu_lcids[i];
  ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->L    = (unsigned char)sdu_lengths[i];
  last_size=2;
  }
  else {
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->R    = 0;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->E    = 0;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->F    = 1;
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->LCID = sdu_lcids[i];
  ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L    = (unsigned short) sdu_lengths[i]&0x7fff;
  last_size=3;
  }
  }

  if (post_padding>0) {// we have lots of padding at the end of the packet
  mac_header_ptr->E = 1;
  mac_header_ptr+=last_size;
  // add a padding element
  mac_header_ptr->R    = 0;
  mac_header_ptr->E    = 0;
  mac_header_ptr->LCID = SHORT_PADDING;
  mac_header_ptr++;
  }
  else { // no end of packet padding
  // last SDU subhead is of fixed type (sdu length implicitly to be computed at UE)
  mac_header_ptr++;
  }

  // Copy MSI Control Element to the end of the MAC Header if it presents
  if ((ce_ptr-mac_header_control_elements) > 0) {
  // printf("Copying %d bytes for control elements\n",ce_ptr-mac_header_control_elements);
  memcpy((void*)mac_header_ptr,mac_header_control_elements,ce_ptr-mac_header_control_elements);
  mac_header_ptr+=(unsigned char)(ce_ptr-mac_header_control_elements);
  }

  return((unsigned char*)mac_header_ptr - mac_header);
  }
  #endif
*/
void add_common_dci(DCI_PDU *DCI_pdu,
		    void *pdu,
		    u16 rnti,
		    unsigned char dci_size_bytes,
		    unsigned char aggregation,
		    unsigned char dci_size_bits,
		    unsigned char dci_fmt,
		    u8 ra_flag) {

  memcpy(&DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].dci_pdu[0],pdu,dci_size_bytes);
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].dci_length = dci_size_bits;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].L          = aggregation;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].rnti       = rnti;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].format     = dci_fmt;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].ra_flag    = ra_flag;


  DCI_pdu->Num_common_dci++;
}

void add_ue_spec_dci(DCI_PDU *DCI_pdu,void *pdu,u16 rnti,unsigned char dci_size_bytes,unsigned char aggregation,unsigned char dci_size_bits,unsigned char dci_fmt,u8 ra_flag) {

  memcpy(&DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].dci_pdu[0],pdu,dci_size_bytes);
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].dci_length = dci_size_bits;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].L          = aggregation;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].rnti       = rnti;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].format     = dci_fmt;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].ra_flag    = ra_flag;

  DCI_pdu->Num_ue_spec_dci++;
}

void schedule_SI(unsigned char Mod_id,u32 frame, unsigned char *nprb,unsigned int *nCCE) {

  unsigned char bcch_sdu_length;
  int mcs;
  void *BCCH_alloc_pdu=(void*)&eNB_mac_inst[Mod_id].BCCH_alloc_pdu;

  bcch_sdu_length = mac_rrc_data_req(Mod_id,
				     frame,
				     BCCH,1,
				     (char*)&eNB_mac_inst[Mod_id].BCCH_pdu.payload[0],
				     1,
				     Mod_id);
  if (bcch_sdu_length > 0) {
    LOG_D(MAC,"[eNB %d] Frame %d : BCCH->DLSCH, Received %d bytes \n",Mod_id,frame,bcch_sdu_length);


    if (bcch_sdu_length <= (mac_xface->get_TBS_DL(0,3)))
      mcs=0;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(1,3)))
      mcs=1;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(2,3)))
      mcs=2;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(3,3)))
      mcs=3;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(4,3)))
      mcs=4;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(5,3)))
      mcs=5;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(6,3)))
      mcs=6;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(7,3)))
      mcs=7;
    else if (bcch_sdu_length <= (mac_xface->get_TBS_DL(8,3)))
      mcs=8;
    
    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      switch (mac_xface->lte_frame_parms->N_RB_DL) {
      case 6:
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 25:
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 50:
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 100:
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;

      }
    }
    else {
      switch (mac_xface->lte_frame_parms->N_RB_DL) {
      case 6:
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 25:
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 50:
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;
      case 100:
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->mcs = mcs;
	break;

      }
    }

#if defined(USER_MODE) && defined(OAI_EMU)
    if (oai_emulation.info.opt_enabled)
      trace_pdu(1, &eNB_mac_inst[Mod_id].BCCH_pdu.payload[0], bcch_sdu_length,
                0xffff, 4, 0xffff, eNB_mac_inst[Mod_id].subframe, 0, 0);
    LOG_D(OPT,"[eNB %d][BCH] Frame %d trace pdu for rnti %x with size %d\n", 
          Mod_id, frame, 0xffff, bcch_sdu_length);
#endif

    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      LOG_D(MAC,"[eNB] Frame %d : Scheduling BCCH->DLSCH (TDD) for SI %d bytes (mcs %d, rb 3, TBS %d)\n",
	    frame,
	    bcch_sdu_length,
	    mcs,
	    mac_xface->get_TBS_DL(mcs,3));
    }
    else {
      LOG_D(MAC,"[eNB] Frame %d : Scheduling BCCH->DLSCH (FDD) for SI %d bytes (mcs %d, rb 3, TBS %d)\n",
	    frame,
	    bcch_sdu_length,
	    mcs,
	    mac_xface->get_TBS_DL(mcs,3));
    }
    eNB_mac_inst[Mod_id].bcch_active=1;
    *nprb=3;
    *nCCE=4;
    return;
  }
  eNB_mac_inst[Mod_id].bcch_active=0;
  *nprb=0;
  *nCCE=0;
}

#ifdef Rel10

int schedule_MBMS(unsigned char Mod_id,u32 frame, u8 subframe) {

  int mcch_flag=0,mtch_flag=0, msi_flag=0;
  int mbsfn_period = 1<<(eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->radioframeAllocationPeriod);
  int mcch_period = 32<<(eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_RepetitionPeriod_r9);
  int mch_scheduling_period = 8<<(eNB_mac_inst[Mod_id].pmch_Config[0]->mch_SchedulingPeriod_r9); 
  unsigned char mcch_sdu_length;
  unsigned char header_len_mcch=0,header_len_msi=0,header_len_mtch=0, header_len_mtch_temp=0, header_len_mcch_temp=0, header_len_msi_temp=0; 
  int ii=0, msi_pos=0;
  int mcch_mcs;
  u16 TBS,j,padding=0,post_padding=0;
  mac_rlc_status_resp_t rlc_status;
  int num_mtch;
  int msi_length,i;
  unsigned char sdu_lcids[11], num_sdus=0, offset=0;
  u16 sdu_lengths[11], sdu_length_total=0;
  unsigned char mch_buffer[MAX_DLSCH_PAYLOAD_BYTES]; // check the max value, this is for dlsch only

  switch (eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.signallingMCS_r9) {
  case 0:
    mcch_mcs = 2;
    break;
  case 1:
    mcch_mcs = 7;
    break;
  case 2:
    mcch_mcs = 13;
    break;
  case 3:
    mcch_mcs = 19;
    break;
  }

  // 1st: Check the MBSFN subframes
  if (frame %  mbsfn_period == eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->radioframeAllocationOffset){ // MBSFN frame
    if (eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.present == MBSFN_SubframeConfig__subframeAllocation_PR_oneFrame){// one-frame format

      //  Find the first subframe in this MCH to transmit MSI
      if (frame % mch_scheduling_period == eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->radioframeAllocationOffset ) {
	while (ii == 0) {
	  ii = eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & (0x80>>msi_pos);
	  msi_pos++;
	}
      }
      
      // Check if the subframe is for MSI, MCCH or MTCHs and Set the correspoding flag to 1
      switch (subframe) {     
      case 1:
	if (mac_xface->lte_frame_parms->frame_type == FDD) {      
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF1) == MBSFN_FDD_SF1) {
	    if (msi_pos == 1) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF1) == MBSFN_FDD_SF1) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 2:
	if (mac_xface->lte_frame_parms->frame_type == FDD){
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF2) == MBSFN_FDD_SF2) {
	    if (msi_pos == 2) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF2) == MBSFN_FDD_SF2) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 3:
	if (mac_xface->lte_frame_parms->frame_type == TDD){// TDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_TDD_SF3) == MBSFN_TDD_SF3) {
	    if (msi_pos == 1) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_TDD_SF3) == MBSFN_TDD_SF3) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	else {// FDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF3) == MBSFN_FDD_SF3) {
	    if (msi_pos == 3) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF3) == MBSFN_FDD_SF3) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 4:
	if (mac_xface->lte_frame_parms->frame_type == TDD){
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_TDD_SF4) == MBSFN_TDD_SF4) {
	    if (msi_pos == 2) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_TDD_SF4) == MBSFN_TDD_SF4) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 6:
	if (mac_xface->lte_frame_parms->frame_type == FDD){
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF6) == MBSFN_FDD_SF6) {
	    if (msi_pos == 4) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF6) == MBSFN_FDD_SF6) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 7:
	if (mac_xface->lte_frame_parms->frame_type == TDD){ // TDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_TDD_SF7) == MBSFN_TDD_SF7) {
	    if (msi_pos == 3) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_TDD_SF7) == MBSFN_TDD_SF7) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	else {// FDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF7) == MBSFN_FDD_SF7) {
	    if (msi_pos == 5) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF7) == MBSFN_FDD_SF7) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 8:
	if (mac_xface->lte_frame_parms->frame_type == TDD){ //TDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_TDD_SF8) == MBSFN_TDD_SF8) {
	    if (msi_pos == 4) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_TDD_SF8) == MBSFN_TDD_SF8) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	else{ // FDD
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_FDD_SF8) == MBSFN_FDD_SF8) {
	    if (msi_pos == 6) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_FDD_SF8) == MBSFN_FDD_SF8) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
	
      case 9:
	if (mac_xface->lte_frame_parms->frame_type == TDD){
	  if ((eNB_mac_inst[Mod_id].mbsfn_SubframeConfig[0]->subframeAllocation.choice.oneFrame.buf[0] & MBSFN_TDD_SF9) == MBSFN_TDD_SF9) {
	    if (msi_pos == 5) 
	      msi_flag = 1;
	    if ( (frame % mcch_period == eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.mcch_Offset_r9) && 
		 ((eNB_mac_inst[Mod_id].mbsfn_AreaInfo[0]->mcch_Config_r9.sf_AllocInfo_r9.buf[0] & MBSFN_TDD_SF9) == MBSFN_TDD_SF9) )
	      mcch_flag = 1;
	    mtch_flag = 1;
	  }
	}
	break;
      }// end switch
    }
    else {// four-frame format
    }
  }
  
    // Calculate the mcs
  if ((msi_flag==1) || (mcch_flag==1)) {
    eNB_mac_inst[Mod_id].MCH_pdu.mcs = mcch_mcs;
  }
  else if (mtch_flag == 1) { // only MTCH in this subframe 
    eNB_mac_inst[Mod_id].MCH_pdu.mcs = eNB_mac_inst[Mod_id].pmch_Config[0]->dataMCS_r9;
  }
  
  
  // 2nd: Create MSI, get MCCH from RRC and MTCHs from RLC
 
  // there is MSI (MCH Scheduling Info) 
  if (msi_flag == 1) {
    // Create MSI here
    u16 msi_control_element[29], *msi_ptr;
   
    msi_ptr = &msi_control_element[0];
    ((MSI_ELEMENT *) msi_ptr)->lcid = MCCH_LCHANID; //MCCH
    if (mcch_flag==1) {   
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_MSB = 0;
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_LSB = 0;
    }
    else {                    // no mcch for this MSP
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_MSB = 0x7;// stop value is 2047
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_LSB = 0xff;
    }
    msi_ptr+= sizeof(MSI_ELEMENT);

         //Header for MTCHs
    num_mtch = eNB_mac_inst[Mod_id].mbms_SessionList[0]->list.count;
    for (i=0;i<num_mtch;i++) { // loop for all session in this MCH (MCH[0]) at this moment
      ((MSI_ELEMENT *) msi_ptr)->lcid = eNB_mac_inst[Mod_id].mbms_SessionList[0]->list.array[i]->logicalChannelIdentity_r9;//mtch_lcid;
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_MSB = 0; // last subframe of this mtch (only one mtch now)
      ((MSI_ELEMENT *) msi_ptr)->stop_sf_LSB = 0xB;
      msi_ptr+=sizeof(MSI_ELEMENT);
    }
    msi_length = msi_ptr-msi_control_element;
    if (msi_length<128)
      header_len_msi = 2;
    else 
      header_len_msi = 3;

    LOG_D(MAC,"[eNB %d] Frame %d : MSI->MCH, length of MSI is %d bytes \n",Mod_id,frame,msi_length);
    //LOG_D(MAC,"Scheduler: MSI is transmitted in this subframe \n" );

    //   LOG_D(MAC,"Scheduler: MSI length is %d bytes\n",msi_length);
    // Store MSI data to mch_buffer[0]
    memcpy((char *)&mch_buffer[sdu_length_total],
	   msi_control_element,
	   msi_length);

    sdu_lcids[num_sdus] = MCH_SCHDL_INFO;
    sdu_lengths[num_sdus] = msi_length;
    sdu_length_total += sdu_lengths[num_sdus];
    LOG_I(MAC,"[eNB %d] Create %d bytes for MSI\n",Mod_id,sdu_lengths[num_sdus]); 
    num_sdus++;
  }

  // there is MCCH
  if (mcch_flag == 1) {
    LOG_D(MAC,"Scheduler: MCCH MESSAGE is transmitted in this subframe \n" );
    
    mcch_sdu_length = mac_rrc_data_req(Mod_id,
				       frame,
				       MCCH,1,
				       (char*)&eNB_mac_inst[Mod_id].MCCH_pdu.payload[0],
				       1,// this is eNB
				       Mod_id);
    if (mcch_sdu_length > 0) {
      LOG_D(MAC,"[eNB %d] Frame %d : MCCH->MCH, Received %d bytes from RRC \n",Mod_id,frame,mcch_sdu_length);
      
      header_len_mcch = 2; 
      if (mac_xface->lte_frame_parms->frame_type == TDD) {
	LOG_D(MAC,"[eNB %d] Frame %d : Scheduling MCCH->MCH (TDD) for MCCH message %d bytes (mcs %d )\n", 
	      Mod_id,
	      frame,
	      mcch_sdu_length,
	      mcch_mcs);
      }
      else {
	LOG_I(MAC,"[eNB %d] Frame %d : Scheduling MCCH->MCH (FDD) for MCCH message %d bytes (mcs %d)\n",
	      Mod_id,
	      frame,
	      mcch_sdu_length,
	      mcch_mcs); 
      }
      eNB_mac_inst[Mod_id].mcch_active=1;

      memcpy((char *)&mch_buffer[sdu_length_total], 
	     &eNB_mac_inst[Mod_id].MCCH_pdu.payload[0], 
	     mcch_sdu_length);
      sdu_lcids[num_sdus] = MCCH_LCHANID; 
      sdu_lengths[num_sdus] = mcch_sdu_length;
      if (sdu_lengths[num_sdus]>128)
	header_len_mcch = 3;
      sdu_length_total += sdu_lengths[num_sdus];
      LOG_D(MAC,"[eNB %d] Got %d bytes for MCCH from RRC \n",Mod_id,sdu_lengths[num_sdus]);
      num_sdus++;
    }
    eNB_mac_inst[Mod_id].mcch_active=0;
  }
  
     // there is MTCHs, loop if there are more than 1
  if (mtch_flag == 1) {
        // Calculate TBS
    /*                                if ((msi_flag==1) || (mcch_flag==1)) {
                                    TBS = mac_xface->get_TBS(mcch_mcs, mac_xface->lte_frame_parms->N_RB_DL);
                                           }
                                    else { // only MTCH in this subframe 
                                    TBS = mac_xface->get_TBS(eNB_mac_inst[Mod_id].pmch_Config[0]->dataMCS_r9, mac_xface->lte_frame_parms->N_RB_DL);
                                          }
    */
    TBS = mac_xface->get_TBS_DL(eNB_mac_inst[Mod_id].MCH_pdu.mcs, mac_xface->lte_frame_parms->N_RB_DL);
    
        // get MTCH data from RLC (like for DTCH)
    LOG_D(MAC,"[eNB %d] Frame %d : MTCH data is transmitted on subframe %d\n",Mod_id,frame,subframe);  
    
    header_len_mtch = 3;
	  LOG_D(MAC,"[eNB %d], Frame %d, MTCH->MCH, Checking RLC status (rab %d, tbs %d, len %d)\n",
	  Mod_id,frame,MTCH,TBS,
	  TBS-header_len_mcch-header_len_msi-sdu_length_total-header_len_mtch);
    
    rlc_status = mac_rlc_status_ind(Mod_id,frame,1,RLC_MBMS_YES,MTCH+ (maxDRB + 3) * MAX_MOBILES_PER_RG,
				    TBS-header_len_mcch-header_len_msi-sdu_length_total-header_len_mtch);
    printf("frame %d, subframe %d,  rlc_status.bytes_in_buffer is %d\n",frame,subframe, rlc_status.bytes_in_buffer);

    if (rlc_status.bytes_in_buffer >0) {
      LOG_I(MAC,"[eNB %d][MBMS USER-PLANE], Frame %d, MTCH->MCH, Requesting %d bytes from RLC (header len mtch %d)\n",
	    Mod_id,frame,TBS-header_len_mcch-header_len_msi-sdu_length_total-header_len_mtch,header_len_mtch);
      
      sdu_lengths[num_sdus] = mac_rlc_data_req(Mod_id,frame, RLC_MBMS_YES,
					       MTCH + (maxDRB + 3) * MAX_MOBILES_PER_RG,
					       (char*)&mch_buffer[sdu_length_total]);
      //sdu_lengths[num_sdus] = mac_rlc_data_req(Mod_id,frame, RLC_MBMS_NO,  MTCH+(MAX_NUM_RB*(NUMBER_OF_UE_MAX+1)), (char*)&mch_buffer[sdu_length_total]);
      LOG_I(MAC,"[eNB %d][MBMS USER-PLANE] Got %d bytes for MTCH %d\n",Mod_id,sdu_lengths[num_sdus],MTCH);
      sdu_lcids[num_sdus] = MTCH;
      sdu_length_total += sdu_lengths[num_sdus];
      if (sdu_lengths[num_sdus] < 128)
	header_len_mtch = 2;
      num_sdus++;
    }
    else {
      header_len_mtch = 0;    
    }
  }
  
  // FINAL STEP: Prepare and multiplexe MSI, MCCH and MTCHs
  if ((sdu_length_total + header_len_msi + header_len_mcch + header_len_mtch) >0) {
    // Adjust the last subheader
    /*                                 if ((msi_flag==1) || (mcch_flag==1)) {
                                         eNB_mac_inst[Mod_id].MCH_pdu.mcs = mcch_mcs;
                                          }
                                        else if (mtch_flag == 1) { // only MTCH in this subframe 
                                       eNB_mac_inst[Mod_id].MCH_pdu.mcs = eNB_mac_inst[Mod_id].pmch_Config[0]->dataMCS_r9;
                                          }
    */
    header_len_mtch_temp = header_len_mtch;
    header_len_mcch_temp = header_len_mcch;
    header_len_msi_temp = header_len_msi;
    if (header_len_mtch>0)
      header_len_mtch=1;         // remove Length field in the  subheader for the last PDU
    else if (header_len_mcch>0)
      header_len_mcch=1;
    else header_len_msi=1;
    // Calculate the padding
    if ((TBS - header_len_mtch - header_len_mcch - header_len_msi - sdu_length_total) <= 2) {
      padding = (TBS - header_len_mtch - header_len_mcch - header_len_msi - sdu_length_total);
      post_padding = 0;
    }
    else {// using post_padding, give back the Length field of subheader  for the last PDU
      padding = 0;
      if (header_len_mtch>0)
	header_len_mtch = header_len_mtch_temp;
      else if (header_len_mcch>0)
	header_len_mcch = header_len_mcch_temp;
      else header_len_msi = header_len_msi_temp;
      post_padding = TBS - sdu_length_total - header_len_msi - header_len_mcch - header_len_mtch;
    }
    // Generate the MAC Header for MCH
    // here we use the function for DLSCH because DLSCH & MCH have the same Header structure
    offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[Mod_id].MCH_pdu.payload,
				   num_sdus, 
				   sdu_lengths, 
				   sdu_lcids,
				   255,    // no drx
				   NULL,  // no timing advance
				   NULL,  // no contention res id
				   padding,                        
				   post_padding);

    LOG_D(MAC," MCS for this sf is %d\n", eNB_mac_inst[Mod_id].MCH_pdu.mcs);

    LOG_I(MAC,"[eNB %d][MBMS USER-PLANE ] Generate header : sdu_length_total %d, num_sdus %d, sdu_lengths[0] %d, sdu_lcids[0] %d => payload offset %d,padding %d,post_padding %d (mcs %d, TBS %d), header MTCH %d, header MCCH %d, header MSI %d\n",
	  Mod_id,sdu_length_total,num_sdus,sdu_lengths[0],sdu_lcids[0],offset,padding,post_padding,eNB_mac_inst[Mod_id].MCH_pdu.mcs,TBS,header_len_mtch, header_len_mcch, header_len_msi);
    // copy SDU to mch_pdu after the MAC Header
    memcpy(&eNB_mac_inst[Mod_id].MCH_pdu.payload[offset],mch_buffer,sdu_length_total);
    // filling remainder of MCH with random data if necessery
    for (j=0;j<(TBS-sdu_length_total-offset);j++)
      eNB_mac_inst[Mod_id].MCH_pdu.payload[offset+sdu_length_total+j] = (char)(taus()&0xff);
/*    
   for (j=0;j<sdu_length_total;j++)
      printf("%2x.",eNB_mac_inst[Mod_id].MCH_pdu.payload[j+offset]);
      printf(" \n");*/
    return 1;
  } 
  else {
    // for testing purpose, fill with random data 
    //for (j=0;j<(TBS-sdu_length_total-offset);j++)
    //  eNB_mac_inst[Mod_id].MCH_pdu.payload[offset+sdu_length_total+j] = (char)(taus()&0xff);
    return 0;
  }
  //this is for testing 
  /*  
  if (mtch_flag == 1) {
  //  LOG_D(MAC,"DUY: mch_buffer length so far is : %ld\n", &mch_buffer[sdu_length_total]-&mch_buffer[0]);
  return 1;
  }
  else 
  return 0;
  */
}

MCH_PDU *get_mch_sdu(uint8_t Mod_id,uint32_t frame, uint32_t subframe) {
  //  eNB_mac_inst[Mod_id].MCH_pdu.mcs=0;
  LOG_D(MAC," MCH_pdu.mcs is %d\n", eNB_mac_inst[Mod_id].MCH_pdu.mcs);
  return(&eNB_mac_inst[Mod_id].MCH_pdu);
}

#endif

// First stage of Random-Access Scheduling
void schedule_RA(unsigned char Mod_id,u32 frame, unsigned char subframe,unsigned char Msg3_subframe,unsigned char *nprb,unsigned int *nCCE) {

  RA_TEMPLATE *RA_template = (RA_TEMPLATE *)&eNB_mac_inst[Mod_id].RA_template[0];
  unsigned char i;//,harq_pid,round;
  u16 rrc_sdu_length;
  unsigned char lcid,offset;
  s8 UE_id;
  unsigned short TBsize,msg4_padding,msg4_post_padding,msg4_header;

  for (i=0;i<NB_RA_PROC_MAX;i++) {

    if (RA_template[i].RA_active == 1) {

      LOG_I(MAC,"[eNB %d][RAPROC] RA %d is active (generate RAR %d, generate_Msg4 %d, wait_ack_Msg4 %d, rnti %x)\n",
	    Mod_id,i,RA_template[i].generate_rar,RA_template[i].generate_Msg4,RA_template[i].wait_ack_Msg4, RA_template[i].rnti);

      if (RA_template[i].generate_rar == 1) {
	*nprb= (*nprb) + 3;
	*nCCE = (*nCCE) + 4;
	RA_template[i].Msg3_subframe=Msg3_subframe;
      }
      else if (RA_template[i].generate_Msg4 == 1) {

	// check for Msg4 Message
	UE_id = find_UE_id(Mod_id,RA_template[i].rnti);
	if (Is_rrc_registered == 1) {

	  // Get RRCConnectionSetup for Piggyback
	  rrc_sdu_length = mac_rrc_data_req(Mod_id,
					    frame,
					    CCCH,1,
					    (char*)&eNB_mac_inst[Mod_id].CCCH_pdu.payload[0],
					    1,
					    Mod_id);
	  if (rrc_sdu_length == -1)
	    mac_xface->macphy_exit("[MAC][eNB Scheduler] CCCH not allocated\n");
	  else {
	    //msg("[MAC][eNB %d] Frame %d, subframe %d: got %d bytes from RRC\n",Mod_id,frame, subframe,rrc_sdu_length);
	  }
	}

	LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: UE_id %d, Is_rrc_registered %d, rrc_sdu_length %d\n",
	      Mod_id,frame, subframe,UE_id, Is_rrc_registered,rrc_sdu_length);

	if (rrc_sdu_length>0) {
	  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: Generating Msg4 with RRC Piggyback (RA proc %d, RNTI %x)\n",
		Mod_id,frame, subframe,i,RA_template[i].rnti);

	  //msg("[MAC][eNB %d][RAPROC] Frame %d, subframe %d: Received %d bytes for Msg4: \n",Mod_id,frame,subframe,rrc_sdu_length);
	  //	  for (j=0;j<rrc_sdu_length;j++)
	  //	    msg("%x ",(unsigned char)eNB_mac_inst[Mod_id].CCCH_pdu.payload[j]);
	  //	  msg("\n");
	  //	  msg("[MAC][eNB] Frame %d, subframe %d: Generated DLSCH (Msg4) DCI, format 1A, for UE %d\n",frame, subframe,UE_id);
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
	  RA_template[i].RA_active = 0;
	  lcid=0;

	  if ((TBsize - rrc_sdu_length - msg4_header) <= 2) {
	    msg4_padding = TBsize - rrc_sdu_length - msg4_header;
	    msg4_post_padding = 0;
	  }
	  else {
	    msg4_padding = 0;
	    msg4_post_padding = TBsize - rrc_sdu_length - msg4_header -1;
	  }
	  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d subframe %d Msg4 : TBS %d, sdu_len %d, msg4_header %d, msg4_padding %d, msg4_post_padding %d\n",
		Mod_id,frame,subframe,TBsize,rrc_sdu_length,msg4_header,msg4_padding,msg4_post_padding); 
	  offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)UE_id][0].payload[0],
					 1,                           //num_sdus
					 &rrc_sdu_length,             //
					 &lcid,                       // sdu_lcid
					 255,                         // no drx
					 0,                           // no timing advance
					 RA_template[i].cont_res_id,  // contention res id
					 msg4_padding,                // no padding
					 msg4_post_padding);

	  memcpy((void*)&eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)UE_id][0].payload[0][(unsigned char)offset],
		 &eNB_mac_inst[Mod_id].CCCH_pdu.payload[0],
		 rrc_sdu_length);

#if defined(USER_MODE) && defined(OAI_EMU)
	  if (oai_emulation.info.opt_enabled){
	    trace_pdu(1, (uint8_t *)eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)UE_id][0].payload[0],
                  rrc_sdu_length, UE_id, 3, find_UE_RNTI(Mod_id, UE_id),
                  eNB_mac_inst[Mod_id].subframe,0,0);
	    LOG_D(OPT,"[eNB %d][DLSCH] Frame %d trace pdu for rnti %x with size %d\n",
		  Mod_id, frame, find_UE_RNTI(Mod_id,UE_id), rrc_sdu_length);
	  }
#endif
	  *nprb= (*nprb) + 3;
	  *nCCE = (*nCCE) + 4;
	}
	//try here
      } 
      /*
	else if (eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4==1) {
	// check HARQ status and retransmit if necessary
	LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: Checking if Msg4 was acknowledged :\n",Mod_id,frame,subframe);
	// Get candidate harq_pid from PHY
	mac_xface->get_ue_active_harq_pid(Mod_id,eNB_mac_inst[Mod_id].RA_template[i].rnti,subframe,&harq_pid,&round,0);
	if (round>0) {
	*nprb= (*nprb) + 3;
	*nCCE = (*nCCE) + 4;
	}
	}
      */
    }
  }
}

// This has to be updated to include BSR information
u8 UE_is_to_be_scheduled(u8 Mod_id,u8 UE_id) {


  //  LOG_D(MAC,"[eNB %d][PUSCH] Frame %d subframe %d Scheduling UE %d\n",Mod_id,rnti,frame,subframe,
  //	UE_id);

  if ((eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]>0) ||
      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]>0) ||
      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]>0) || 
      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]>0) || 
      (eNB_mac_inst[Mod_id].UE_template[UE_id].ul_SR>0)) // uplink scheduling request
    return(1);
  else 
    return(0);
}

u32 bytes_to_bsr_index(s32 nbytes) {

  u32 i=0;

  if (nbytes<0)
    return(0);

  while ((i<BSR_TABLE_SIZE)&&
	 (BSR_TABLE[i]<=nbytes)){
    i++;
  }
  return(i-1);
}

// This table holds the allowable PRB sizes for ULSCH transmissions
u8 rb_table[33] = {1,2,3,4,5,6,8,9,10,12,15,16,18,20,24,25,27,30,32,36,40,45,48,50,54,60,72,75,80,81,90,96,100};

void schedule_ulsch(unsigned char Mod_id,u32 frame,unsigned char cooperation_flag,unsigned char subframe,unsigned char sched_subframe,unsigned int *nCCE) {//,int calibration_flag) {

  unsigned char granted_UEs;
  unsigned int nCCE_available;


  u16 first_rb=1,i;

  granted_UEs = find_ulgranted_UEs(Mod_id);
  nCCE_available = mac_xface->get_nCCE_max(Mod_id) - *nCCE;
 
  // UE data info;
  // check which UE has data to transmit
  // function to decide the scheduling
  // e.g. scheduling_rslt = Greedy(granted_UEs, nb_RB)

  // default function for default scheduling
  // 

  // output of scheduling, the UE numbers in RBs, where it is in the code???
  // check if RA (Msg3) is active in this subframe, if so skip the PRBs used for Msg3
  // Msg3 is using 1 PRB so we need to increase first_rb accordingly
  // not sure about the break (can there be more than 1 active RA procedure?)

  for (i=0;i<NB_RA_PROC_MAX;i++) {
    if ((eNB_mac_inst[Mod_id].RA_template[i].RA_active == 1) && 
	(eNB_mac_inst[Mod_id].RA_template[i].generate_rar == 0) &&
	(eNB_mac_inst[Mod_id].RA_template[i].Msg3_subframe == sched_subframe)) {  
      first_rb++; 
      break;
    }
  }
  
  schedule_ulsch_rnti(Mod_id, cooperation_flag, frame, subframe, sched_subframe, granted_UEs, nCCE, &nCCE_available, &first_rb);

#ifdef CBA
  if ((eNB_mac_inst[Mod_id].num_active_cba_groups > 0) && (*nCCE == 0))
    schedule_ulsch_cba_rnti(Mod_id, cooperation_flag, frame, subframe, sched_subframe, granted_UEs, nCCE, &nCCE_available, &first_rb);
#endif   
 
}
#ifdef CBA
void schedule_ulsch_cba_rnti(u8 Mod_id, unsigned char cooperation_flag, u32 frame, unsigned char subframe, unsigned char sched_subframe, u8 granted_UEs, unsigned int *nCCE, unsigned int *nCCE_available, u16 *first_rb){ 

  DCI0_5MHz_TDD_1_6_t *ULSCH_dci_tdd16;
  DCI0_5MHz_FDD_t *ULSCH_dci_fdd;
  DCI_PDU *DCI_pdu= &eNB_mac_inst[Mod_id].DCI_pdu;
  
  u8 rb_table_index=0, aggregation=2;
  u32 rballoc;
  u8 cba_group, cba_resources;
  u8 required_rbs[NUM_MAX_CBA_GROUP], weight[NUM_MAX_CBA_GROUP], num_cba_resources[NUM_MAX_CBA_GROUP];
  u8 available_rbs= ceil(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb);
  u8 remaining_rbs= available_rbs;
  u8 allocated_rbs;
  // We compute the weight of each group and initialize some variables
  for (cba_group=0;cba_group<eNB_mac_inst[Mod_id].num_active_cba_groups;cba_group++) {
    // UEs in PUSCH with traffic 
    weight[cba_group] = find_num_active_UEs_in_cbagroup(Mod_id, cba_group);
    required_rbs[cba_group] = 0;
    num_cba_resources[cba_group]=0;
  }
  //LOG_D(MAC, "[eNB ] CBA granted ues are %d\n",granted_UEs );
  for (cba_group=0;cba_group<eNB_mac_inst[Mod_id].num_active_cba_groups  && (*nCCE_available > (1<<aggregation));cba_group++) {
    if (remaining_rbs <= 0 ) 
      break;
    // If the group needs some resource
    if ((weight[cba_group] > 0) && eNB_mac_inst[Mod_id].cba_rnti[cba_group] != 0){ 
      // to be refined in case of : granted_UEs >> weight[cba_group]*available_rbs
      required_rbs[cba_group] = (u8)ceil((weight[cba_group]*available_rbs)/granted_UEs);

      while (remaining_rbs < required_rbs[cba_group] )
	required_rbs[cba_group]--;
      
      /*
	while (rb_table[rb_table_index] < required_rbs[cba_group])
	rb_table_index++;
	  
	while (rb_table[rb_table_index] > remaining_rbs )
	rb_table_index--;
      
	remaining_rbs-=rb_table[rb_table_index];
	required_rbs[cba_group]=rb_table[rb_table_index];
      */
      // to be refined 
      if (weight[cba_group] <  required_rbs[cba_group])
	num_cba_resources[cba_group]=(u8)ceil(weight[cba_group]/2.0);
      else 
	num_cba_resources[cba_group]=(u8)ceil(required_rbs[cba_group]/2.0);
      
      while ((*nCCE) + (1<<aggregation) * num_cba_resources[cba_group] > *nCCE_available)
	num_cba_resources[cba_group]--;
         
      LOG_N(MAC,"[eNB %d] Frame %d, subframe %d: cba group %d weight/granted_ues %d/%d available/required rb (%d/%d), num resources %d->1 (*scaled down*) \n", 
	    Mod_id, frame, subframe, cba_group, 
	    weight[cba_group], granted_UEs, available_rbs,required_rbs[cba_group],
	    num_cba_resources[cba_group]);
      
      num_cba_resources[cba_group]=1;
      
    }
  }
  // phase 2   
  for (cba_group=0;cba_group<eNB_mac_inst[Mod_id].num_active_cba_groups;cba_group++) {
    for (cba_resources=0; cba_resources < num_cba_resources[cba_group]; cba_resources++){
      rb_table_index =0;
      // check if there was an allocation for this group in the 1st phase 
      if (required_rbs[cba_group] == 0 )
	continue;

      while (rb_table[rb_table_index] < (u8) ceil(required_rbs[cba_group] / num_cba_resources[cba_group]) )
	rb_table_index++;

      while (rb_table[rb_table_index] > remaining_rbs )
	rb_table_index--;
      
      remaining_rbs-=rb_table[rb_table_index];
      allocated_rbs=rb_table[rb_table_index];
      
      rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
				      *first_rb,
				      rb_table[rb_table_index]);
      
      *first_rb+=rb_table[rb_table_index];
      LOG_D(MAC,"[eNB %d] Frame %d, subframe %d: CBA %d rnti %x, total/required/allocated/remaining rbs (%d/%d/%d/%d), rballoc %d, nCCE (%d/%d)\n", 
	    Mod_id, frame, subframe, cba_group,eNB_mac_inst[Mod_id].cba_rnti[cba_group],
	    available_rbs, required_rbs[cba_group], allocated_rbs, remaining_rbs,rballoc,
	    *nCCE_available,*nCCE);
      
      
      if (mac_xface->lte_frame_parms->frame_type == TDD) {
	ULSCH_dci_tdd16 = (DCI0_5MHz_TDD_1_6_t *)eNB_mac_inst[Mod_id].UE_template[cba_group].ULSCH_DCI[0];
	
	ULSCH_dci_tdd16->type     = 0;
	ULSCH_dci_tdd16->hopping  = 0;
	ULSCH_dci_tdd16->rballoc  = rballoc;
	ULSCH_dci_tdd16->mcs      = 2;
	ULSCH_dci_tdd16->ndi      = 1;
	ULSCH_dci_tdd16->TPC      = 1;
	ULSCH_dci_tdd16->cshift   = cba_group;
	ULSCH_dci_tdd16->dai      = eNB_mac_inst[Mod_id].UE_template[cba_group].DAI_ul[sched_subframe];
	ULSCH_dci_tdd16->cqi_req  = 1;
	
	//add_ue_spec_dci
	add_common_dci(DCI_pdu,
		       ULSCH_dci_tdd16,
		       eNB_mac_inst[Mod_id].cba_rnti[cba_group],
		       sizeof(DCI0_5MHz_TDD_1_6_t),
		       aggregation,
		       sizeof_DCI0_5MHz_TDD_1_6_t,
		       format0,
		       0);
      }
      else {
	ULSCH_dci_fdd           = (DCI0_5MHz_FDD_t *)eNB_mac_inst[Mod_id].UE_template[cba_group].ULSCH_DCI[0];
	
	ULSCH_dci_fdd->type     = 0;
	ULSCH_dci_fdd->hopping  = 0;
	ULSCH_dci_fdd->rballoc  = rballoc;
	ULSCH_dci_fdd->mcs      = 2;
	ULSCH_dci_fdd->ndi      = 1;
	ULSCH_dci_fdd->TPC      = 1;
	ULSCH_dci_fdd->cshift   = 0;
	ULSCH_dci_fdd->cqi_req  = 1;
	
	//add_ue_spec_dci
	add_common_dci(DCI_pdu,
		       ULSCH_dci_fdd,
		       eNB_mac_inst[Mod_id].cba_rnti[cba_group],
		       sizeof(DCI0_5MHz_FDD_t),
		       aggregation,
		       sizeof_DCI0_5MHz_FDD_t,
		       format0,
		       0);
      }
      *nCCE = (*nCCE) + (1<<aggregation) * num_cba_resources[cba_group];
      *nCCE_available = mac_xface->get_nCCE_max(Mod_id) - *nCCE;
      //      break;// for the moment only schedule one
    }
  } 
}
#endif 
void schedule_ulsch_rnti(u8 Mod_id, unsigned char cooperation_flag, u32 frame, unsigned char subframe, unsigned char sched_subframe, u8 granted_UEs, unsigned int *nCCE, unsigned int *nCCE_available, u16 *first_rb){ 
  unsigned char UE_id;
  unsigned char next_ue;
  unsigned char aggregation=2;
  u16 rnti;
  u8 round = 0;
  u8 harq_pid = 0;
  void *ULSCH_dci;

  LTE_eNB_UE_stats* eNB_UE_stats;
  DCI_PDU *DCI_pdu= &eNB_mac_inst[Mod_id].DCI_pdu;
  u8 status=0;//,status0 = 0,status1 = 0;
  //  u8 k=0;
  u8 rb_table_index;
  u16 TBS,i;
  u32 buffer_occupancy;
  u32 tmp_bsr;
  u32 cqi_req,cshift,ndi,mcs,rballoc;
 
  for (UE_id=0;UE_id<granted_UEs && (*nCCE_available > (1<<aggregation));UE_id++) {

    //    msg("[MAC][eNB] subframe %d: checking UE_id %d\n",subframe,UE_id);
    next_ue = UE_id;     // find next ue to schedule
    //    msg("[MAC][eNB] subframe %d: next ue %d\n",subframe,next_ue);
    rnti = find_UE_RNTI(Mod_id,next_ue); // radio network temp id is obtained
    //    msg("[MAC][eNB] subframe %d: rnti %x\n",subframe,rnti);

    if (rnti==0) // if so, go to next UE
      continue;

    eNB_UE_stats = mac_xface->get_eNB_UE_stats(Mod_id,rnti);
    if (eNB_UE_stats==NULL)
      mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

    LOG_D(MAC,"[eNB %d] Scheduler Frame %d, subframe %d, nCCE %d: Checking ULSCH next UE_id %d mode id %d (rnti %x,mode %s), format 0\n",Mod_id,frame,subframe,*nCCE,next_ue,Mod_id, rnti,mode_string[eNB_UE_stats->mode]);

    if (eNB_UE_stats->mode == PUSCH) { // ue has a ulsch channel
      s8 ret;
      // Get candidate harq_pid from PHY
      ret = mac_xface->get_ue_active_harq_pid(Mod_id,rnti,subframe,&harq_pid,&round,1); 
      //	printf("Got harq_pid %d, round %d, next_ue %d\n",harq_pid,round,next_ue);

      /* [SR] 01/07/13: Don't schedule UE if we cannot get harq pid */
      if ((((UE_is_to_be_scheduled(Mod_id,UE_id)>0)) || (round>0) || ((frame%10)==0)) && (ret == 0)) {
	// if there is information on bsr of DCCH, DTCH or if there is UL_SR, or if there is a packet to retransmit, or we want to schedule a periodic feedback every 10 frames 
	
	
	LOG_D(MAC,"[eNB %d][PUSCH %x] Frame %d subframe %d Scheduling UE %d (SR %d)\n",
	      Mod_id,rnti,frame,subframe,UE_id,
	      eNB_mac_inst[Mod_id].UE_template[UE_id].ul_SR);
    
	// reset the scheduling request
	eNB_mac_inst[Mod_id].UE_template[UE_id].ul_SR = 0;

	aggregation = process_ue_cqi(Mod_id,next_ue); // =2 by default!! 
	//    msg("[MAC][eNB] subframe %d: aggregation %d\n",subframe,aggregation);

	status = mac_get_rrc_status(Mod_id,1,next_ue);

	if (status < RRC_CONNECTED) 
	  cqi_req = 0;
	else
	  cqi_req = 1;

	if (round > 0) {
	  ndi = 0; // if round != 0, it means the data is not new. ndi:new data indicator
	}
	else {
	  ndi = 1;
	}
	//if ((frame&1)==0) {

	// choose this later based on Power Headroom
	if (ndi == 1) {// set mcs for first round
	  mcs = openair_daq_vars.target_ue_ul_mcs;
	}
	else { // increment RV
	  if ((round&3)==0)
	    mcs = openair_daq_vars.target_ue_ul_mcs; // same as inital transmission
	  else
	    mcs = (round&3) + 28; //not correct for round==4! 
	}

	LOG_D(MAC,"[eNB %d] ULSCH scheduler: Ndi %d, mcs %d\n",Mod_id,ndi,mcs);

	if((cooperation_flag > 0) && (next_ue == 1)) { // Allocation on same set of RBs
	  // RIV:resource indication value // function in openair1/PHY/LTE_TRANSPORT/dci_tools.c
	  rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL, 
					  ((next_ue-1)*4),//openair_daq_vars.ue_ul_nb_rb),
					  4);//openair_daq_vars.ue_ul_nb_rb);
	}
	else if ((ndi==1) && (mcs < 29)) {
	  rb_table_index = 1;
	  TBS = mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
	  buffer_occupancy = ((eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]  == 0) &&
			      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]  == 0) &&
			      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]  == 0) &&
			      (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3] == 0))?
	    BSR_TABLE[1] :   // This is when we've received SR and buffers are fully served
	    BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]]+
	    BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]]+
	    BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]]+
	    BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]];  // This is when remaining data in UE buffers (even if SR is triggered)

	  LOG_D(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframe %d Scheduled UE, BSR for LCGID0 %d, LCGID1 %d, LCGID2 %d LCGID3 %d, BO %d\n",
		Mod_id,UE_id,rnti,frame,subframe,
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0],
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1],
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2],
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3],
		buffer_occupancy);

	  while ((TBS < buffer_occupancy) &&
		 rb_table[rb_table_index]<(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb)){
	    // continue until we've exhauster the UEs request or the total number of available PRBs
	    /*	    LOG_I(MAC,"[eNB %d][PUSCH %x] Frame %d subframe %d Scheduled UE (rb_table_index %d => TBS %d)\n",
		    Mod_id,rnti,frame,subframe,
		    rb_table_index,TBS);
	    */
	    rb_table_index++;
	    TBS = mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
	  }

	  if (rb_table[rb_table_index]>(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb)) {
	    rb_table_index--;
	    TBS = mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
	  }
	  //rb_table_index = 8;
	  
	  LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframe %d Scheduled UE (mcs %d, first rb %d, nb_rb %d, rb_table_index %d, TBS %d, harq_pid %d)\n",
		Mod_id,UE_id,rnti,frame,subframe,mcs,
		*first_rb,rb_table[rb_table_index],
		rb_table_index,mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]), 
		harq_pid);
	  
	  rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
					  *first_rb,
					  rb_table[rb_table_index]);//openair_daq_vars.ue_ul_nb_rb);

	  *first_rb+=rb_table[rb_table_index];  // increment for next UE allocation
	  eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb_ul[harq_pid] = rb_table[rb_table_index]; //store for possible retransmission

	  buffer_occupancy -= mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
	  i = bytes_to_bsr_index((s32)buffer_occupancy);

	  // Adjust BSR entries for LCGIDs
	  if (i>0) {
	    if (eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0] <= i) {
	      tmp_bsr = BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]];
	      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0] = 0;
	      if (BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]] <= (buffer_occupancy-tmp_bsr)) {
		tmp_bsr += BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]];
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1] = 0;
		if (BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]] <= (buffer_occupancy-tmp_bsr)) {
		  tmp_bsr += BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]];
		  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2] = 0;
		  if (BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]] <= (buffer_occupancy-tmp_bsr)) {
		    tmp_bsr += BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]];
		    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3] = 0;
		  } else {
		    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3] = bytes_to_bsr_index((s32)BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3]] - ((s32)buffer_occupancy - (s32)tmp_bsr));
		  }
		}
		else {
		  eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2] = bytes_to_bsr_index((s32)BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2]] - ((s32)buffer_occupancy -(s32)tmp_bsr));
		}
	      }
	      else {
		eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1] = bytes_to_bsr_index((s32)BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1]] - (s32)buffer_occupancy);
	      }
	    }
	    else {
	      eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0] = bytes_to_bsr_index((s32)BSR_TABLE[eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0]] - (s32)buffer_occupancy);
	    }
	  }
	  else {  // we have flushed all buffers so clear bsr
	    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID0] = 0;
	    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID1] = 0;
	    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID2] = 0; 
	    eNB_mac_inst[Mod_id].UE_template[UE_id].bsr_info[LCGID3] = 0;
	  }
	  

	} // ndi==1 
	else { //we schedule a retransmission
	  LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframe %d Scheduled UE retransmission (mcs %d, first rb %d, nb_rb %d, TBS %d, harq_pid %d)\n",
		Mod_id,UE_id,rnti,frame,subframe,mcs,
		*first_rb,eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb_ul[harq_pid],
		mac_xface->get_TBS_UL(mcs,eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb_ul[harq_pid]),
		harq_pid);
	  
	  rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
					  *first_rb,
					  eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb_ul[harq_pid]);

	  *first_rb+=eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb_ul[harq_pid];  // increment for next UE allocation
	}

	// Cyclic shift for DM RS
	if(cooperation_flag == 2) {
	  if(next_ue == 1)// For Distriibuted Alamouti, cyclic shift applied to 2nd UE
	    cshift = 1;
	  else
	    cshift = 0;
	}
	else
	  cshift = 0;// values from 0 to 7 can be used for mapping the cyclic shift (36.211 , Table 5.5.2.1.1-1)
	  
	if (mac_xface->lte_frame_parms->frame_type == TDD) {
	  switch (mac_xface->lte_frame_parms->N_RB_UL) {
	  case 6:
	    ULSCH_dci = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[sched_subframe];
	    ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->cqi_req  = cqi_req;
	    
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_1_5MHz_TDD_1_6_t),
			    aggregation,
			    sizeof_DCI0_1_5MHz_TDD_1_6_t,
			    format0,
			    0);
	    break;
	  default:
	  case 25:
	    ULSCH_dci = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[sched_subframe];
	    ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->cqi_req  = cqi_req;
	     
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_5MHz_TDD_1_6_t),
			    aggregation,
			    sizeof_DCI0_5MHz_TDD_1_6_t,
			    format0,
			    0);
	    break;
	  case 50:
	    ULSCH_dci = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	     
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[sched_subframe];
	    ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->cqi_req  = cqi_req;
	     
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_10MHz_TDD_1_6_t),
			    aggregation,
			    sizeof_DCI0_10MHz_TDD_1_6_t,
			    format0,
			    0);
	    break;
	  case 100:
	    ULSCH_dci = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	     
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[sched_subframe];
	    ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->cqi_req  = cqi_req;
	     
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_20MHz_TDD_1_6_t),
			    aggregation,
			    sizeof_DCI0_20MHz_TDD_1_6_t,
			    format0,
			    0);
	    break;
	  }
	}
	else { //FDD
	  switch (mac_xface->lte_frame_parms->N_RB_UL) {
	  case 25:
	  default:
	    
	    ULSCH_dci          = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_5MHz_FDD_t *)ULSCH_dci)->cqi_req  = cqi_req;
	    
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_5MHz_FDD_t),
			    aggregation,
			    sizeof_DCI0_5MHz_FDD_t,
			    format0,
			    0);
	    break;
	  case 6:
	    ULSCH_dci          = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->cqi_req  = cqi_req;
	    
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_1_5MHz_FDD_t),
			    aggregation,
			    sizeof_DCI0_1_5MHz_FDD_t,
			    format0,
			    0);
	    break;
	  case 50:
	    ULSCH_dci          = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_10MHz_FDD_t *)ULSCH_dci)->cqi_req  = cqi_req;
	    
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_10MHz_FDD_t),
			    aggregation,
			    sizeof_DCI0_10MHz_FDD_t,
			    format0,
			    0);
	    break;
	  case 100:
	    ULSCH_dci          = eNB_mac_inst[Mod_id].UE_template[next_ue].ULSCH_DCI[harq_pid];
	    
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->type     = 0;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
	    ((DCI0_20MHz_FDD_t *)ULSCH_dci)->cqi_req  = cqi_req;
	    
	    add_ue_spec_dci(DCI_pdu,
			    ULSCH_dci,
			    rnti,
			    sizeof(DCI0_20MHz_FDD_t),
			    aggregation,
			    sizeof_DCI0_20MHz_FDD_t,
			    format0,
			    0);
	    break;

	  }
	}	       
	//#ifdef DEBUG_eNB_SCHEDULER
	//      dump_dci(mac_xface->lte_frame_parms,
	//	       &DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci-1]);
	//#endif
	add_ue_ulsch_info(Mod_id,
			  next_ue,
			  subframe,
			  S_UL_SCHEDULED);

	*nCCE = (*nCCE) + (1<<aggregation);
	*nCCE_available = mac_xface->get_nCCE_max(Mod_id) - *nCCE;
	//msg("[MAC][eNB %d][ULSCH Scheduler] Frame %d, subframe %d: Generated ULSCH DCI for next UE_id %d, format 0\n", Mod_id,frame,subframe,next_ue);

	//break; // leave loop after first UE is schedule (avoids m
      } // UE_is_to_be_scheduled
    } // UE is in PUSCH
  } // loop over UE_id
}



u32 allocate_prbs(unsigned char UE_id,unsigned char nb_rb, u32 *rballoc) {

  int i;
  u32 rballoc_dci=0;
  unsigned char nb_rb_alloc=0;

  for (i=0;i<(mac_xface->lte_frame_parms->N_RB_DL-2);i+=2) {
    if (((*rballoc>>i)&3)==0) {
      *rballoc |= (3<<i);
      rballoc_dci |= (1<<((12-i)>>1));
      nb_rb_alloc+=2;
    }
    if (nb_rb_alloc==nb_rb)
      return(rballoc_dci);
  }

  if ((mac_xface->lte_frame_parms->N_RB_DL&1)==1) {
    if ((*rballoc>>(mac_xface->lte_frame_parms->N_RB_DL-1)&1)==0) {
      *rballoc |= (1<<(mac_xface->lte_frame_parms->N_RB_DL-1));
      rballoc_dci |= 1;//(1<<(mac_xface->lte_frame_parms->N_RB_DL>>1));
    }
  }
  return(rballoc_dci);
}


u32 allocate_prbs_sub(int nb_rb, u8 *rballoc) {

  int check=0;//check1=0,check2=0;
  uint32_t rballoc_dci=0;
  //u8 number_of_subbands=13;

  LOG_D(MAC,"*****Check1RBALLOC****: %d%d%d%d (nb_rb %d,N_RBGS %d)\n",
	rballoc[3],rballoc[2],rballoc[1],rballoc[0],nb_rb,mac_xface->lte_frame_parms->N_RBGS);
  while((nb_rb >0) && (check < mac_xface->lte_frame_parms->N_RBGS)){
    //printf("rballoc[%d] %d\n",check,rballoc[check]);
    if(rballoc[check] == 1){
      rballoc_dci |= (1<<((mac_xface->lte_frame_parms->N_RBGS-1)-check));
      switch (mac_xface->lte_frame_parms->N_RB_DL) {
	case 6:
	  nb_rb--;
	case 25:
	  if ((check == mac_xface->lte_frame_parms->N_RBGS-1))
	    nb_rb--;
	  else
	    nb_rb-=2;
	  break;
	case 50:
	  if ((check == mac_xface->lte_frame_parms->N_RBGS-1))
	    nb_rb-=2;
	  else
	    nb_rb-=3;
	  break;
      case 100:
	nb_rb-=4;
	break;
      }
    }
    //printf("rb_alloc %x\n",rballoc_dci);
    check = check+1;
    //    check1 = check1+2;
  }
  // rballoc_dci = (rballoc_dci)&(0x1fff);
  LOG_D(MAC,"*********RBALLOC : %x\n",rballoc_dci);
  // exit(-1);
  return (rballoc_dci);
}


void fill_DLSCH_dci(unsigned char Mod_id,u32 frame, unsigned char subframe,u32 RBalloc,u8 RA_scheduled,int mbsfn_flag) {
  // loop over all allocated UEs and compute frequency allocations for PDSCH

  unsigned char UE_id,first_rb,nb_rb=3;
  u16 rnti;
  unsigned char vrb_map[100];
  u8 rballoc_sub[mac_xface->lte_frame_parms->N_RBGS];
  //u8 number_of_subbands=13;
  u32 rballoc=RBalloc;

  //  u32 test=0;
  unsigned char round;
  unsigned char harq_pid;
  void *DLSCH_dci=NULL;
  DCI_PDU *DCI_pdu= &eNB_mac_inst[Mod_id].DCI_pdu;
  int i;
  //  u8 status=0;

  void *BCCH_alloc_pdu=(void*)&eNB_mac_inst[Mod_id].BCCH_alloc_pdu;
  int size_bits,size_bytes;

  if (mbsfn_flag>0)
    return;

  // clear vrb_map
  memset(vrb_map,0,100);

  // SI DLSCH
  //  printf("BCCH check\n");
  if (eNB_mac_inst[Mod_id].bcch_active == 1) {
    eNB_mac_inst[Mod_id].bcch_active = 0;

    // randomize frequency allocation for SI
    first_rb = (unsigned char)(taus()%(mac_xface->lte_frame_parms->N_RB_DL-4));
    if (mac_xface->lte_frame_parms->frame_type == TDD) {

    }
    else {
      BCCH_alloc_pdu_fdd.rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
      rballoc |= mac_xface->get_rballoc(BCCH_alloc_pdu_fdd.vrb_type,BCCH_alloc_pdu_fdd.rballoc);
    }
 
    vrb_map[first_rb] = 1;
    vrb_map[first_rb+1] = 1;
    vrb_map[first_rb+2] = 1;
    vrb_map[first_rb+3] = 1;

    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      switch (mac_xface->lte_frame_parms->N_RB_DL) {
      case 6:
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->TPC = 1;
	rballoc |= mac_xface->get_rballoc(0,((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_1_5MHz_TDD_1_6_t),
		       2,
		       sizeof_DCI1A_1_5MHz_TDD_1_6_t,
		       format1A,0);
	break;
      case 25:
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->TPC = 1;
	rballoc |= mac_xface->get_rballoc(0,((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_5MHz_TDD_1_6_t),
		       2,
		       sizeof_DCI1A_5MHz_TDD_1_6_t,
		       format1A,0);
	break;
      case 50:
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->TPC = 1;
	rballoc |= mac_xface->get_rballoc(0,((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_10MHz_TDD_1_6_t),
		       2,
		       sizeof_DCI1A_10MHz_TDD_1_6_t,
		       format1A,0);
	break;
      case 100:
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->TPC = 1;
	rballoc |= mac_xface->get_rballoc(0,((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_20MHz_TDD_1_6_t),
		       2,
		       sizeof_DCI1A_20MHz_TDD_1_6_t,
		       format1A,0);
	break;
      }
    }
    else {
      switch (mac_xface->lte_frame_parms->N_RB_DL) {
      case 6:
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->TPC = 1;

	rballoc |= mac_xface->get_rballoc(0,((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_1_5MHz_FDD_t),
		       2,
		       sizeof_DCI1A_1_5MHz_FDD_t,
		       format1A,0);
	break;
      case 25:
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->TPC = 1;

	rballoc |= mac_xface->get_rballoc(0,((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_5MHz_FDD_t),
		       2,
		       sizeof_DCI1A_5MHz_FDD_t,
		       format1A,0);
	break;
      case 50:
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->TPC = 1;

	rballoc |= mac_xface->get_rballoc(0,((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_10MHz_FDD_t),
		       2,
		       sizeof_DCI1A_10MHz_FDD_t,
		       format1A,0);
	break;
      case 100:
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->type = 1;
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->vrb_type = 0;
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->ndi = 1;
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->rv = 1;
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->harq_pid = 0;
	((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->TPC = 1;

	rballoc |= mac_xface->get_rballoc(0,((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->rballoc);
	add_common_dci(DCI_pdu,
		       BCCH_alloc_pdu,
		       SI_RNTI,
		       sizeof(DCI1A_20MHz_FDD_t),
		       2,
		       sizeof_DCI1A_20MHz_FDD_t,
		       format1A,0);
	break;
      }
    }
  }
  if (RA_scheduled == 1) {
    for (i=0;i<NB_RA_PROC_MAX;i++) {

      if (eNB_mac_inst[Mod_id].RA_template[i].generate_rar == 1) {
	
	//FK: postponed to fill_rar
	//eNB_mac_inst[Mod_id].RA_template[i].generate_rar = 0;

	LOG_D(MAC,"[eNB %d] Frame %d, subframe %d: Generating RAR DCI (proc %d), RA_active %d format 1A (%d,%d))\n",
	      Mod_id,frame, subframe,i,
	      eNB_mac_inst[Mod_id].RA_template[i].RA_active,
	      eNB_mac_inst[Mod_id].RA_template[i].RA_dci_fmt1,
	      eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bits1);
	// randomize frequency allocation for RA
	while (1) {
	  first_rb = (unsigned char)(taus()%(mac_xface->lte_frame_parms->N_RB_DL-4));
	  if ((vrb_map[first_rb] != 1) && (vrb_map[first_rb+3] != 1))
	    break;
	}
	vrb_map[first_rb] = 1;
	vrb_map[first_rb+1] = 1;
	vrb_map[first_rb+2] = 1;
	vrb_map[first_rb+3] = 1;
	
	if (mac_xface->lte_frame_parms->frame_type == TDD) {
	  switch(mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 25:
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  

	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 50:
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  

	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 100:
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  

	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  default:
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  

	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  }
	}
	else {
	  switch(mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 25:
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 50:
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  case 100:
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->type=1; 
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rv=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->TPC=1;  
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
					      ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0])->rballoc);
	    break;
	  default:
	    break;
	  }
	}
	add_common_dci(DCI_pdu,
		       (void*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu1[0],
		       eNB_mac_inst[Mod_id].RA_template[i].RA_rnti,
		       eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bytes1,
		       2,
		       eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bits1,
		       eNB_mac_inst[Mod_id].RA_template[i].RA_dci_fmt1,
		       1);
	

	  
	LOG_D(MAC,"[eNB %d] Frame %d: Adding common dci for RA%d (RAR) RA_active %d\n",Mod_id,frame,i,
	      eNB_mac_inst[Mod_id].RA_template[i].RA_active);
      }
      if (eNB_mac_inst[Mod_id].RA_template[i].generate_Msg4_dci == 1) {
	
	// randomize frequency allocation for RA
	while (1) {
	  first_rb = (unsigned char)(taus()%(mac_xface->lte_frame_parms->N_RB_DL-4));
	  if ((vrb_map[first_rb] != 1) && (vrb_map[first_rb+3] != 1))
	    break;
	}
	vrb_map[first_rb] = 1;
	vrb_map[first_rb+1] = 1;
	vrb_map[first_rb+2] = 1;
	vrb_map[first_rb+3] = 1;

	if (mac_xface->lte_frame_parms->frame_type == TDD) {
	  switch (mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;      
	    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 25:
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 50:
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 100:
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  default:
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  }
	}
	else {
	  switch (mac_xface->lte_frame_parms->N_RB_DL) {

	  case 6:
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 25:
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 50:
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  case 100:
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  default:
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->type=1; 
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rv=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->TPC=1;  
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	    break;
	  }
	}
	
	add_ue_spec_dci(DCI_pdu,
			(void*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0],
			eNB_mac_inst[Mod_id].RA_template[i].rnti,
			eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bytes2,
			1,
			eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bits2,
			eNB_mac_inst[Mod_id].RA_template[i].RA_dci_fmt2,
			0);
	LOG_D(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: Adding ue specific dci (rnti %x) for Msg4\n",
	      Mod_id,frame,subframe,eNB_mac_inst[Mod_id].RA_template[i].rnti);
	eNB_mac_inst[Mod_id].RA_template[i].generate_Msg4_dci=0;
	
      }
      else if (eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4==1) {
	// check HARQ status and retransmit if necessary
	LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: Checking if Msg4 was acknowledged: \n",
	      Mod_id,frame,subframe);
	// Get candidate harq_pid from PHY
	mac_xface->get_ue_active_harq_pid(Mod_id,eNB_mac_inst[Mod_id].RA_template[i].rnti,subframe,&harq_pid,&round,0);
	if (round>0) {
	  // we have to schedule a retransmission
	  if (mac_xface->lte_frame_parms->frame_type == TDD) 
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->ndi=0;
	  else
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->ndi=0;
	  // randomize frequency allocation for RA
	  while (1) {
	    first_rb = (unsigned char)(taus()%(mac_xface->lte_frame_parms->N_RB_DL-4));
	    if ((vrb_map[first_rb] != 1) && (vrb_map[first_rb+3] != 1))
	      break;
	  }
	  vrb_map[first_rb] = 1;
	  vrb_map[first_rb+1] = 1;
	  vrb_map[first_rb+2] = 1;
	  vrb_map[first_rb+3] = 1;
	  if (mac_xface->lte_frame_parms->frame_type == TDD) {
	    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	  }
	  else {
	    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
	    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
					      ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0])->rballoc);
	  }
	  
	  add_ue_spec_dci(DCI_pdu,
			  (void*)&eNB_mac_inst[Mod_id].RA_template[i].RA_alloc_pdu2[0],
			  eNB_mac_inst[Mod_id].RA_template[i].rnti,
			  eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bytes2,
			  2,
			  eNB_mac_inst[Mod_id].RA_template[i].RA_dci_size_bits2,
			  eNB_mac_inst[Mod_id].RA_template[i].RA_dci_fmt2,
			  0);
	  LOG_W(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d: Msg4 not acknowledged, adding ue specific dci (rnti %x) for RA (Msg4 Retransmission)\n",
		Mod_id,frame,subframe,eNB_mac_inst[Mod_id].RA_template[i].rnti);
	}
	else {
	  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframe %d : Msg4 acknowledged\n",Mod_id,frame,subframe);
	  eNB_mac_inst[Mod_id].RA_template[i].wait_ack_Msg4=0;
	  eNB_mac_inst[Mod_id].RA_template[i].RA_active=0;
	}
      }
    }
  } // RA is scheduled in this subframe

  // UE specific DCIs
  for (UE_id=0;UE_id<NUMBER_OF_UE_MAX;UE_id++) {
    //printf("UE_id: %d => status %d\n",UE_id,eNB_dlsch_info[Mod_id][UE_id].status);
    if (eNB_dlsch_info[Mod_id][UE_id].status == S_DL_SCHEDULED) {

      // clear scheduling flag
      eNB_dlsch_info[Mod_id][UE_id].status = S_DL_WAITING;
      rnti = find_UE_RNTI(Mod_id,UE_id);
      mac_xface->get_ue_active_harq_pid(Mod_id,rnti,subframe,&harq_pid,&round,0);
      nb_rb = eNB_mac_inst[Mod_id].UE_template[UE_id].nb_rb[harq_pid];

      DLSCH_dci = (void *)eNB_mac_inst[Mod_id].UE_template[UE_id].DLSCH_DCI[harq_pid];


      /// Synchronizing rballoc with rballoc_sub
      for(i=0;i<mac_xface->lte_frame_parms->N_RBGS;i++){
	rballoc_sub[i] = eNB_mac_inst[Mod_id].UE_template[UE_id].rballoc_subband[harq_pid][i];
	if(rballoc_sub[i] == 1)  
	  rballoc |= (0x0001<<i);  // TO be FIXED!!!!!!
      }


      switch(mac_xface->get_transmission_mode(Mod_id,rnti)) {
      default:

      case 1:

      case 2:
	LOG_D(MAC,"[USER-PLANE DEFAULT DRB] Adding UE spec DCI for %d PRBS (%x) => ",nb_rb,rballoc);
	if (mac_xface->lte_frame_parms->frame_type == TDD) {
	  switch (mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes = sizeof(DCI1_1_5MHz_TDD_t);
	    size_bits  = sizeof_DCI1_1_5MHz_TDD_t;
	    break;
	  case 25:
	    ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes = sizeof(DCI1_5MHz_TDD_t);
	    size_bits  = sizeof_DCI1_5MHz_TDD_t;
	    break;
	  case 50:
	    ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes = sizeof(DCI1_10MHz_TDD_t);
	    size_bits  = sizeof_DCI1_10MHz_TDD_t;
	    break;
	  case 100:
	    ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes = sizeof(DCI1_20MHz_TDD_t);
	    size_bits  = sizeof_DCI1_20MHz_TDD_t;
	    break;
	  default:
	    ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes = sizeof(DCI1_5MHz_TDD_t);
	    size_bits  = sizeof_DCI1_5MHz_TDD_t;
	    break;
	  }


	}
	else {
	  switch(mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes=sizeof(DCI1_1_5MHz_FDD_t);
	    size_bits=sizeof_DCI1_1_5MHz_FDD_t;
	    break;
	  case 25:
	    ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes=sizeof(DCI1_5MHz_FDD_t);
	    size_bits=sizeof_DCI1_5MHz_FDD_t;
	    break;
	  case 50:
	    ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes=sizeof(DCI1_10MHz_FDD_t);
	    size_bits=sizeof_DCI1_10MHz_FDD_t;
	    break;
	  case 100:
	    ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes=sizeof(DCI1_20MHz_FDD_t);
	    size_bits=sizeof_DCI1_20MHz_FDD_t;
	    break;
	  default:
	    ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	    ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
	    size_bytes=sizeof(DCI1_5MHz_FDD_t);
	    size_bits=sizeof_DCI1_5MHz_FDD_t;
	    break;
	  }
	}

	add_ue_spec_dci(DCI_pdu,
			DLSCH_dci,
			rnti,
			size_bytes,
			process_ue_cqi (Mod_id,UE_id),//aggregation,
			size_bits,
			format1,
			0);
	
	break;
      case 4:

	//if (nb_rb>10) {
	// DCI format 2_2A_M10PRB can also be used for less than 10 PRB (it refers to the system bandwidth)
	((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rah = 0;
	add_ue_spec_dci(DCI_pdu,
			DLSCH_dci,
			rnti,
			sizeof(DCI2_5MHz_2A_M10PRB_TDD_t),
			process_ue_cqi (Mod_id,UE_id),//aggregation,
			sizeof_DCI2_5MHz_2A_M10PRB_TDD_t,
			format2_2A_M10PRB,
			0);
	/*}
	  else {
	  ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	  add_ue_spec_dci(DCI_pdu,
	  DLSCH_dci,
	  rnti,
	  sizeof(DCI2_5MHz_2A_L10PRB_TDD_t),
	  2,//aggregation,
	  sizeof_DCI2_5MHz_2A_L10PRB_TDD_t,
	  format2_2A_L10PRB);
	  }*/
	break;
      case 5:
	((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rah = 0;

	add_ue_spec_dci(DCI_pdu,
			DLSCH_dci,
			rnti,
			sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t),
			process_ue_cqi (Mod_id,UE_id),//aggregation,
			sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t,
			format1E_2A_M10PRB,
			0);
	break;

      case 6:
	((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
	((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rah = 0;
	
	add_ue_spec_dci(DCI_pdu,
			DLSCH_dci,
			rnti,
			sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t),
			process_ue_cqi (Mod_id,UE_id),//aggregation
			sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t,
			format1E_2A_M10PRB,
			0);
	break;

      }
    }


  }
}



void update_ul_dci(u8 Mod_id,u16 rnti,u8 dai) {

  DCI_PDU *DCI_pdu= &eNB_mac_inst[Mod_id].DCI_pdu;
  int i;
  DCI0_5MHz_TDD_1_6_t *ULSCH_dci;

  if (mac_xface->lte_frame_parms->frame_type == TDD) {
    for (i=0;i<DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci;i++) {
      ULSCH_dci = (DCI0_5MHz_TDD_1_6_t *)DCI_pdu->dci_alloc[i].dci_pdu;
      if ((DCI_pdu->dci_alloc[i].format == format0) && (DCI_pdu->dci_alloc[i].rnti == rnti))
	ULSCH_dci->dai = (dai-1)&3;
    }
  }
  //  printf("Update UL DCI: DAI %d\n",dai);
}

void schedule_ue_spec(unsigned char Mod_id,
		      u32 frame, 
		      unsigned char subframe,
		      u16 nb_rb_used0,
		      unsigned int *nCCE_used,
		      int mbsfn_flag) {

  unsigned char UE_id;
  unsigned char next_ue;
  unsigned char granted_UEs;
  u16 nCCE;
  unsigned char aggregation;
  mac_rlc_status_resp_t rlc_status;
  unsigned char header_len_dcch=0, header_len_dcch_tmp=0,header_len_dtch=0,header_len_dtch_tmp=0, ta_len=0;
  unsigned char sdu_lcids[11],offset,num_sdus=0;
  u16 nb_rb,nb_rb_temp,nb_available_rb,TBS,j,sdu_lengths[11],rnti,padding=0,post_padding=0;
  unsigned char dlsch_buffer[MAX_DLSCH_PAYLOAD_BYTES];
  unsigned char round=0;
  unsigned char harq_pid=0;
  void *DLSCH_dci;
  LTE_eNB_UE_stats* eNB_UE_stats;
  u16 sdu_length_total=0;
  //  unsigned char loop_count;
  unsigned char DAI;
  u16 i=0,ii=0,tpmi0=1;
  u8 dl_pow_off[NUMBER_OF_UE_MAX];
  unsigned char rballoc_sub_UE[NUMBER_OF_UE_MAX][N_RBGS_MAX];
//   unsigned char rballoc_sub[N_RBGS_MAX];
  u16 pre_nb_available_rbs[NUMBER_OF_UE_MAX];
  int mcs;
  //u8 number_of_subbands=13;
  u16 min_rb_unit;


  switch (mac_xface->lte_frame_parms->N_RB_DL) {
  case 6:
    min_rb_unit=1;
    break;
  case 25:
    min_rb_unit=2;
    break;
  case 50:
    min_rb_unit=3;
    break;
  case 100:
    min_rb_unit=4;
    break;
  default:
    min_rb_unit=2;
    break;
  }

  if (mbsfn_flag>0)
    return;

  //int **rballoc_sub = (int **)malloc(1792*sizeof(int *));
  granted_UEs = find_dlgranted_UEs(Mod_id);


  //weight = get_ue_weight(Mod_id,UE_id);
  aggregation = 1; // set to the maximum aggregation level


  /// Initialization for pre-processor
  for(i=0;i<NUMBER_OF_UE_MAX;i++){
    pre_nb_available_rbs[i] = 0;
    dl_pow_off[i] = 2;
    for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++){
//       rballoc_sub[j] = 0;
      rballoc_sub_UE[i][j] = 0;
    }
  }


  for (i = 0; i < NUMBER_OF_UE_MAX; i++) {
    PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[i].pre_nb_available_rbs = 0;
    for (j = 0; j < mac_xface->lte_frame_parms->N_RBGS; j++) {
      PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[i].rballoc_sub[j] = 0;
    }
  }


  // set current available nb_rb and nCCE to maximum
  nb_available_rb = mac_xface->lte_frame_parms->N_RB_DL - nb_rb_used0;
  nCCE = mac_xface->get_nCCE_max(Mod_id) - *nCCE_used;

  // store the goloabl enb stats
  eNB_mac_inst[Mod_id].eNB_stats.num_dlactive_UEs =  granted_UEs;
  eNB_mac_inst[Mod_id].eNB_stats.available_prbs =  nb_available_rb;
  eNB_mac_inst[Mod_id].eNB_stats.total_available_prbs +=  nb_available_rb;
  eNB_mac_inst[Mod_id].eNB_stats.available_ncces = nCCE; 
  eNB_mac_inst[Mod_id].eNB_stats.dlsch_bytes_tx=0;
  eNB_mac_inst[Mod_id].eNB_stats.dlsch_pdus_tx=0;


  /// CALLING Pre_Processor for downlink scheduling (Returns estimation of RBs required by each UE and the allocation on sub-band)
  dlsch_scheduler_pre_processor(Mod_id,
				frame,
				subframe,
				dl_pow_off,
				pre_nb_available_rbs,
				mac_xface->lte_frame_parms->N_RBGS,
				rballoc_sub_UE);


  for (UE_id=0;UE_id<granted_UEs;UE_id++) {
   
    rnti = find_UE_RNTI(Mod_id,UE_id);

    eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].crnti= rnti;
    eNB_mac_inst[Mod_id].eNB_UE_stats[UE_id].rrc_status=mac_get_rrc_status(Mod_id,1,UE_id);
    if (rnti==0) {
      LOG_E(MAC,"Cannot find rnti for UE_id %d (granted UEs %d)\n",UE_id,granted_UEs);
      mac_xface->macphy_exit("");//continue;
    }

    eNB_UE_stats = mac_xface->get_eNB_UE_stats(Mod_id,rnti);
    if (eNB_UE_stats==NULL)
      mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

    // Get candidate harq_pid from PHY
    mac_xface->get_ue_active_harq_pid(Mod_id,rnti,subframe,&harq_pid,&round,0);
    //    printf("Got harq_pid %d, round %d\n",harq_pid,round);


    nb_available_rb = pre_nb_available_rbs[UE_id];

    if ((nb_available_rb == 0) || (nCCE < (1<<aggregation))) {
      LOG_D(MAC,"UE %d: nb_availiable_rb exhausted (nb_rb_used %d, nb_available_rb %d, nCCE %d, aggregation %d)\n",
	    UE_id, nb_rb_used0, nb_available_rb, nCCE, aggregation);
      //if(mac_xface->get_transmission_mode(Mod_id,rnti)==5) 
      continue; //to next user (there might be rbs availiable for other UEs in TM5
      // else 
      //	break;
    }
    sdu_length_total=0;
    num_sdus=0;

    // get Round-Robin allocation
    next_ue = UE_id;//schedule_next_dlue(Mod_id,subframe); // next scheduled user
    // If nobody is left, exit while loop and go to next step
    if (next_ue == 255)
      break;

    if (mac_xface->lte_frame_parms->frame_type == TDD) { 
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 0:
	if ((subframe==0)||(subframe==1)||(subframe==3)||(subframe==5)||(subframe==6)||(subframe==8))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
      case 1:
	if ((subframe==0)||(subframe==4)||(subframe==5)||(subframe==9))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
	break;
      case 2:
	if ((subframe==4)||(subframe==5))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
	break;
      case 3:
	if ((subframe==5)||(subframe==7)||(subframe==9))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
	break;
      case 4:
	if ((subframe==0)||(subframe==6))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
	break;
      case 5:
	if (subframe==9)
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
	break;
      case 6:
	if ((subframe==0)||(subframe==1)||(subframe==5)||(subframe==6)||(subframe==9))
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI = 0;
      default:
	break;
      }
    }

    // This is an allocated UE_id
    rnti = find_UE_RNTI(Mod_id,next_ue);
    if (rnti==0)
      continue;

    eNB_UE_stats = mac_xface->get_eNB_UE_stats(Mod_id,rnti);

    if (eNB_UE_stats==NULL)
      mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

    //eNB_UE_stats->dlsch_mcs1 = openair_daq_vars.target_ue_dl_mcs;
    // int flag_LA=0;
    //printf("CQI %d\n",eNB_UE_stats->DL_cqi[0]);
    if (flag_LA==0){
     
      switch(eNB_UE_stats->DL_cqi[0]) {
      case 0:
	eNB_UE_stats->dlsch_mcs1 = 0;
	break;
      case 1:
	eNB_UE_stats->dlsch_mcs1 = 0;
	break;
      case 2:
	eNB_UE_stats->dlsch_mcs1 = 0;
	break;	
      case 3:
	eNB_UE_stats->dlsch_mcs1 = 2;
	break;
      case 4:
	eNB_UE_stats->dlsch_mcs1 = 4;
	break;
      case 5:
	eNB_UE_stats->dlsch_mcs1 = 6;
	break;
      case 6:
	eNB_UE_stats->dlsch_mcs1 = 8;
	break;
      case 7:
	eNB_UE_stats->dlsch_mcs1 = 11;
	break;
      case 8:
	eNB_UE_stats->dlsch_mcs1 = 13;
	break;
      case 9:
	eNB_UE_stats->dlsch_mcs1 = 16;
	break;
      case 10:
	eNB_UE_stats->dlsch_mcs1 = 18;
	break;
      case 11:
	eNB_UE_stats->dlsch_mcs1 = 20;
	break;
      case 12:
	eNB_UE_stats->dlsch_mcs1 = 22;
	break;
      case 13:
	eNB_UE_stats->dlsch_mcs1 = 22;//25
	break;
      case 14:
	eNB_UE_stats->dlsch_mcs1 = 22;//27
	break;
      case 15:
	eNB_UE_stats->dlsch_mcs1 = 22;//28
	break;
      default:
	LOG_E(MAC,"Invalid CQI");
	exit(-1);
      }
    }
    else {
	// begin CQI to MCS mapping
      if(mac_xface->get_transmission_mode(Mod_id,rnti)==1)
	eNB_UE_stats->dlsch_mcs1 = cqi_mcs[0][eNB_UE_stats->DL_cqi[0]];
      else
	if(mac_xface->get_transmission_mode(Mod_id,rnti)==2)
	  eNB_UE_stats->dlsch_mcs1 = cqi_mcs[1][eNB_UE_stats->DL_cqi[0]];
	else
	  if(mac_xface->get_transmission_mode(Mod_id,rnti)==6 || mac_xface->get_transmission_mode(Mod_id,rnti)==5)
	    eNB_UE_stats->dlsch_mcs1 = cqi_mcs[2][eNB_UE_stats->DL_cqi[0]];
      // end CQI Mapping 
      // if MUMIMO is enabled with two UEs then adjust the CQI and MCS mapping
      if(mac_xface->get_transmission_mode(Mod_id,rnti)==5 && dl_pow_off[UE_id]==0){
	snr_tm6 = cqi_snr[eNB_UE_stats->DL_cqi[0]];
	if (snr_tm6<snr_mcs[0])
	  eNB_UE_stats->dlsch_mcs1 = 0;
	else 
	  if(snr_tm6>snr_mcs[27])
	    eNB_UE_stats->dlsch_mcs1 = 27;
	  else
	    for (i=0;i<27;i++){
	      if(snr_tm6 > snr_mcs[i] && snr_tm6 < snr_mcs[i+1])
		eNB_UE_stats->dlsch_mcs1 = i;
	    }
      }
    }
    
    if(eNB_UE_stats->dlsch_mcs1>22)
      eNB_UE_stats->dlsch_mcs1=22;
 
    
    // for TM5, limit the MCS to 16QAM    


    // for EXMIMO, limit the MCS to 16QAM as well
#ifdef EXMIMO
    eNB_UE_stats->dlsch_mcs1 = cmin(eNB_UE_stats->dlsch_mcs1,16);
#endif    

    // Get candidate harq_pid from PHY
    mac_xface->get_ue_active_harq_pid(Mod_id,rnti,subframe,&harq_pid,&round,0);

    eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].harq_pid = harq_pid; 
    eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].harq_round = round; 
    // Note this code is for a specific DCI format
    DLSCH_dci = (void *)eNB_mac_inst[Mod_id].UE_template[next_ue].DLSCH_DCI[harq_pid];

    for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++){ // initializing the rb allocation indicator for each UE
      eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j] = 0;
    }
    // store stats 
    eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].dl_cqi= eNB_UE_stats->DL_cqi[0];
    
    if (round > 0) {
      if (mac_xface->lte_frame_parms->frame_type == TDD) {
	eNB_mac_inst[Mod_id].UE_template[next_ue].DAI++;
	LOG_D(MAC,"DAI update: subframe %d: UE %d, DAI %d\n",subframe,next_ue,eNB_mac_inst[Mod_id].UE_template[next_ue].DAI);
	
	update_ul_dci(Mod_id,rnti,eNB_mac_inst[Mod_id].UE_template[next_ue].DAI);
      }

      // get freq_allocation
      nb_rb = eNB_mac_inst[Mod_id].UE_template[next_ue].nb_rb[harq_pid];
      if (nb_rb <= nb_available_rb) {
	
	if(nb_rb == pre_nb_available_rbs[next_ue]){
	  for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++) // for indicating the rballoc for each sub-band
	    eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];}
	else
	  {
	    nb_rb_temp = nb_rb;
	    j = 0;
	    while((nb_rb_temp > 0) && (j<mac_xface->lte_frame_parms->N_RBGS)){
	      if(rballoc_sub_UE[next_ue][j] == 1){
		eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
		if((j == mac_xface->lte_frame_parms->N_RBGS-1) && 
		   ((mac_xface->lte_frame_parms->N_RB_DL == 25)||
		    (mac_xface->lte_frame_parms->N_RB_DL == 50)))
		  nb_rb_temp = nb_rb_temp - min_rb_unit+1;
		else
		  nb_rb_temp = nb_rb_temp - min_rb_unit;
	      }
	      j = j+1;
	    }
	  }

	nb_available_rb -= nb_rb;
	aggregation = process_ue_cqi(Mod_id,next_ue);
	nCCE-=(1<<aggregation); // adjust the remaining nCCE
	*nCCE_used += (1<<aggregation);


	PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].pre_nb_available_rbs = nb_rb;
	PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].dl_pow_off = dl_pow_off[next_ue];
	
	for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++)
	  PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].rballoc_sub[j] = eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j];

	switch (mac_xface->get_transmission_mode(Mod_id,rnti)) {
	case 1:
	case 2:
	default:
	  switch (mac_xface->lte_frame_parms->N_RB_DL) {
	  case 6:
	    if (mac_xface->lte_frame_parms->frame_type == TDD) {
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",Mod_id,harq_pid,round,(eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1),((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs);
	    }
	    else {
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",Mod_id,harq_pid,round,((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs);
	      
	    }
	    break;
	  case 25:
	    if (mac_xface->lte_frame_parms->frame_type == TDD) {
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",Mod_id,harq_pid,round,(eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1),((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs);
	    }
	    else {
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",Mod_id,harq_pid,round,((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs);
	      
	    }
	    break;
	  case 50:
	    if (mac_xface->lte_frame_parms->frame_type == TDD) {
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",Mod_id,harq_pid,round,(eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1),((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs);
	    }
	    else {
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",Mod_id,harq_pid,round,((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs);
	      
	    }
	    break;
	  case 100:
	    if (mac_xface->lte_frame_parms->frame_type == TDD) {
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",Mod_id,harq_pid,round,(eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1),((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs);
	    }
	    else {
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
	      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",Mod_id,harq_pid,round,((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs);
	      
	    }
	    break;
	  }
	  break;
	case 4:
	  //	  if (nb_rb>10) {
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi1 = 0;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  // }
	  //else {
	  //  ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 0;
	  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
	  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  // }
	  break;
	case 5:
	  // if(nb_rb>10){
	  //((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = eNB_UE_stats->DL_cqi[0]<<1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  if(dl_pow_off[next_ue] == 2)
	    dl_pow_off[next_ue] = 1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = dl_pow_off[next_ue];
	  // }
	  break;
	case 6:
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;//dl_pow_off[next_ue];
	  break;
	}

	add_ue_dlsch_info(Mod_id,
			  next_ue,
			  subframe,
			  S_DL_SCHEDULED);

	//eNB_UE_stats->dlsch_trials[round]++;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_retransmission+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].rbs_used_retx=nb_rb;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].total_rbs_used_retx+=nb_rb;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].ncce_used_retx=nCCE;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].dlsch_mcs2=eNB_UE_stats->dlsch_mcs1;
      }
      else { // don't schedule this UE, its retransmission takes more resources than we have
	
      }
    }
    else {  // This is a potentially new SDU opportunity

      // calculate mcs

      rlc_status.bytes_in_buffer = 0;
      // Now check RLC information to compute number of required RBs
      // get maximum TBS size for RLC request
      //TBS = mac_xface->get_TBS(eNB_UE_stats->DL_cqi[0]<<1,nb_available_rb);
      TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_available_rb);
      // check first for RLC data on DCCH
      // add the length for  all the control elements (timing adv, drx, etc) : header + payload  
      ta_len = ((eNB_UE_stats->timing_advance_update/4)!=0) ? 2 : 0;
      
      header_len_dcch = 2; // 2 bytes DCCH SDU subheader 

    
      rlc_status = mac_rlc_status_ind(Mod_id,frame,1,RLC_MBMS_NO, DCCH+(NB_RB_MAX*next_ue),
				      (TBS-ta_len-header_len_dcch)); // transport block set size

      sdu_lengths[0]=0;
      if (rlc_status.bytes_in_buffer > 0) {  // There is DCCH to transmit
	LOG_D(MAC,"[eNB %d] Frame %d, DL-DCCH->DLSCH, Requesting %d bytes from RLC (RRC message)\n",Mod_id,frame,TBS-header_len_dcch);
	sdu_lengths[0] += mac_rlc_data_req(Mod_id,frame,RLC_MBMS_NO,
					   DCCH+(NB_RB_MAX*next_ue),
					   (char *)&dlsch_buffer[sdu_lengths[0]]);

    	LOG_D(MAC,"[eNB %d][DCCH] Got %d bytes from RLC\n",Mod_id,sdu_lengths[0]);
	sdu_length_total = sdu_lengths[0];
	sdu_lcids[0] = DCCH;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_pdu_tx[DCCH]+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_bytes_tx[DCCH]+=sdu_lengths[0];
	num_sdus = 1;
#ifdef DEBUG_eNB_SCHEDULER
	LOG_T(MAC,"[eNB %d][DCCH] Got %d bytes :",Mod_id,sdu_lengths[0]);
	for (j=0;j<sdu_lengths[0];j++)
	  LOG_T(MAC,"%x ",dlsch_buffer[j]);
	LOG_T(MAC,"\n");
#endif
      }
      else {
	header_len_dcch = 0;
	sdu_length_total = 0;
      }

      // check for DCCH1 and update header information (assume 2 byte sub-header)
      rlc_status = mac_rlc_status_ind(Mod_id,frame,1,RLC_MBMS_NO, DCCH+1+(NB_RB_MAX*next_ue),
				      (TBS-ta_len-header_len_dcch-sdu_length_total)); // transport block set size less allocations for timing advance and
      // DCCH SDU

      if (rlc_status.bytes_in_buffer > 0) {
	LOG_D(MAC,"[eNB %d], Frame %d, DCCH1->DLSCH, Requesting %d bytes from RLC (RRC message)\n",
	      Mod_id,frame,TBS-header_len_dcch-sdu_length_total);
	sdu_lengths[num_sdus] += mac_rlc_data_req(Mod_id,frame,RLC_MBMS_NO,
						  DCCH+1+(NB_RB_MAX*next_ue),
						  (char *)&dlsch_buffer[sdu_lengths[0]]);
	sdu_lcids[num_sdus] = DCCH1;
	sdu_length_total += sdu_lengths[num_sdus];
	header_len_dcch += 2;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_pdu_tx[DCCH1]+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_bytes_tx[DCCH1]+=sdu_lengths[num_sdus];
	num_sdus++;
	LOG_D(MAC,"[eNB %d] Got %d bytes for DCCH from RLC\n",Mod_id,sdu_lengths[0]);
      }
      // check for DTCH and update header information
      // here we should loop over all possible DTCH

      header_len_dtch = 3; // 3 bytes DTCH SDU subheader

      LOG_D(MAC,"[eNB %d], Frame %d, DTCH->DLSCH, Checking RLC status (rab %d, tbs %d, len %d)\n",
	    Mod_id,frame,DTCH+(NB_RB_MAX*next_ue),TBS,
	    TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);

      rlc_status = mac_rlc_status_ind(Mod_id,frame,1,RLC_MBMS_NO, DTCH+(NB_RB_MAX*next_ue),
				      TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);

      if (rlc_status.bytes_in_buffer > 0) {
	
	LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB], Frame %d, DTCH->DLSCH, Requesting %d bytes from RLC (hdr len dtch %d)\n",
	      Mod_id,frame,TBS-header_len_dcch-sdu_length_total-header_len_dtch,header_len_dtch);
	sdu_lengths[num_sdus] = mac_rlc_data_req(Mod_id,frame,RLC_MBMS_NO,
						 DTCH+(NB_RB_MAX*next_ue),
						 (char*)&dlsch_buffer[sdu_length_total]);
	
	LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] Got %d bytes for DTCH %d \n",Mod_id,sdu_lengths[num_sdus],DTCH+(NB_RB_MAX*next_ue));
	sdu_lcids[num_sdus] = DTCH;
	sdu_length_total += sdu_lengths[num_sdus];
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_pdu_tx[DTCH]+=1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].num_bytes_tx[DTCH]+=sdu_lengths[num_sdus];
	if (sdu_lengths[num_sdus] < 128)
	  header_len_dtch=2;
	num_sdus++;
      }
      else {
	header_len_dtch = 0;
      }

      // there is a payload
      if (((sdu_length_total + header_len_dcch + header_len_dtch )> 0)) {

	// Now compute number of required RBs for total sdu length
	// Assume RAH format 2
	// adjust  header lengths
	header_len_dcch_tmp = header_len_dcch;
	header_len_dtch_tmp = header_len_dtch;
	if (header_len_dtch==0)
	  header_len_dcch = (header_len_dcch >0) ? 1 : header_len_dcch;  // remove length field
	else 
	  header_len_dtch = (header_len_dtch > 0) ? 1 :header_len_dtch;     // remove length field for the last SDU
	



	mcs = eNB_UE_stats->dlsch_mcs1;
	if (mcs==0) nb_rb = 4;   // don't let the TBS get too small
	else nb_rb=min_rb_unit;

	TBS = mac_xface->get_TBS_DL(mcs,nb_rb); 
	
	while (TBS < (sdu_length_total + header_len_dcch + header_len_dtch + ta_len))  {
	  nb_rb += min_rb_unit;  // 
	  if (nb_rb>nb_available_rb) { // if we've gone beyond the maximum number of RBs
	    // (can happen if N_RB_DL is odd)
	    TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_available_rb);
	    nb_rb = nb_available_rb;
	    break;
	  }
	  TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_rb);
	}

	if(nb_rb == pre_nb_available_rbs[next_ue])
	  for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++) // for indicating the rballoc for each sub-band
	    eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
	else
	  {
	    nb_rb_temp = nb_rb;
	    j = 0;
	    while((nb_rb_temp > 0) && (j<mac_xface->lte_frame_parms->N_RBGS)){
	      if(rballoc_sub_UE[next_ue][j] == 1){
		eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
		if ((j == mac_xface->lte_frame_parms->N_RBGS-1) && 
		    ((mac_xface->lte_frame_parms->N_RB_DL == 25)||
		     (mac_xface->lte_frame_parms->N_RB_DL == 50)))
		  nb_rb_temp = nb_rb_temp - min_rb_unit+1;
		else 
		  nb_rb_temp = nb_rb_temp - min_rb_unit;
	      }
	      j = j+1;
	    }
	  }
	
	PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].pre_nb_available_rbs = nb_rb;
	PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].dl_pow_off = dl_pow_off[next_ue];
	
	for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++)
	  PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[next_ue].rballoc_sub[j] = eNB_mac_inst[Mod_id].UE_template[next_ue].rballoc_subband[harq_pid][j];


	// decrease mcs until TBS falls below required length
	while ((TBS > (sdu_length_total + header_len_dcch + header_len_dtch + ta_len)) && (mcs>0)) {
	  mcs--;
	  TBS = mac_xface->get_TBS_DL(mcs,nb_rb);
	}

	// if we have decreased too much or we don't have enough RBs, increase MCS
	while ((TBS < (sdu_length_total + header_len_dcch + header_len_dtch + ta_len)) && ((( dl_pow_off[next_ue]>0) && (mcs<28)) || ( (dl_pow_off[next_ue]==0) && (mcs<=15)))) {
	  mcs++;
	  TBS = mac_xface->get_TBS_DL(mcs,nb_rb);
	}

	LOG_D(MAC,"dlsch_mcs before and after the rate matching = (%d, %d)\n",eNB_UE_stats->dlsch_mcs1, mcs);

#ifdef DEBUG_eNB_SCHEDULER
	LOG_D(MAC,"[eNB %d] Generated DLSCH header (mcs %d, TBS %d, nb_rb %d)\n",
	      Mod_id,mcs,TBS,nb_rb);
	// msg("[MAC][eNB ] Reminder of DLSCH with random data %d %d %d %d \n",
	//	TBS, sdu_length_total, offset, TBS-sdu_length_total-offset);
#endif

	if ((TBS - header_len_dcch - header_len_dtch - sdu_length_total - ta_len) <= 2) {
	  padding = (TBS - header_len_dcch - header_len_dtch - sdu_length_total - ta_len);
	  post_padding = 0;
	}
	else {
	  padding = 0;
	  // adjust the header len 
	  if (header_len_dtch==0)
	    header_len_dcch = header_len_dcch_tmp; 
	  else //if (( header_len_dcch==0)&&((header_len_dtch==1)||(header_len_dtch==2)))
	    header_len_dtch = header_len_dtch_tmp; 
	  
	  post_padding = TBS - sdu_length_total - header_len_dcch - header_len_dtch - ta_len ; // 1 is for the postpadding header 
	}


	offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)next_ue][0].payload[0],
				       // offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0],
				       num_sdus,              //num_sdus
				       sdu_lengths,  //
				       sdu_lcids,
				       255,                                   // no drx
				       eNB_UE_stats->timing_advance_update/4, // timing advance
				       NULL,                                  // contention res id
				       padding,                        
				       post_padding);
	//#ifdef DEBUG_eNB_SCHEDULER
	LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] Generate header : sdu_length_total %d, num_sdus %d, sdu_lengths[0] %d, sdu_lcids[0] %d => payload offset %d,timing advance value : %d, next_ue %d,padding %d,post_padding %d,(mcs %d, TBS %d, nb_rb %d),header_dcch %d, header_dtch %d\n",
	      Mod_id,sdu_length_total,num_sdus,sdu_lengths[0],sdu_lcids[0],offset,
	      ta_len,next_ue,padding,post_padding,mcs,TBS,nb_rb,header_len_dcch,header_len_dtch);
	//#endif
		      
	LOG_T(MAC,"[eNB %d] First 16 bytes of DLSCH : \n");
	for (i=0;i<16;i++)
	  LOG_T(MAC,"%x.",dlsch_buffer[i]);
	LOG_T(MAC,"\n");
	
	// cycle through SDUs and place in dlsch_buffer
	memcpy(&eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)next_ue][0].payload[0][offset],dlsch_buffer,sdu_length_total);
	// memcpy(&eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset],dcch_buffer,sdu_lengths[0]);

	// fill remainder of DLSCH with random data
	for (j=0;j<(TBS-sdu_length_total-offset);j++)
	  eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)next_ue][0].payload[0][offset+sdu_length_total+j] = (char)(taus()&0xff);
	//eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset+sdu_lengths[0]+j] = (char)(taus()&0xff);

#if defined(USER_MODE) && defined(OAI_EMU)
        /* Tracing of PDU is done on UE side */
	if (oai_emulation.info.opt_enabled)
            trace_pdu(1, (uint8_t *)eNB_mac_inst[Mod_id].DLSCH_pdu[(unsigned char)next_ue][0].payload[0],
                TBS, Mod_id, 3, find_UE_RNTI(Mod_id,next_ue),
                eNB_mac_inst[Mod_id].subframe,0,0);
	LOG_D(OPT,"[eNB %d][DLSCH] Frame %d  rnti %x  with size %d\n", 
	      Mod_id, frame, find_UE_RNTI(Mod_id,next_ue), TBS);
#endif

	aggregation = process_ue_cqi(Mod_id,next_ue);
	nCCE-=(1<<aggregation); // adjust the remaining nCCE
	*nCCE_used+=(1<<aggregation); // adjust the remaining nCCE
	eNB_mac_inst[Mod_id].UE_template[next_ue].nb_rb[harq_pid] = nb_rb;
	
	add_ue_dlsch_info(Mod_id,
			  next_ue,
			  subframe,
			  S_DL_SCHEDULED);
	// store stats 
	eNB_mac_inst[Mod_id].eNB_stats.dlsch_bytes_tx+=sdu_length_total;
	eNB_mac_inst[Mod_id].eNB_stats.dlsch_pdus_tx+=1;
      
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].rbs_used = nb_rb;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].total_rbs_used += nb_rb;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].ncce_used = nCCE;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].dlsch_mcs2=mcs;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].TBS = TBS;

	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].overhead_bytes= TBS- sdu_length_total;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].total_sdu_bytes+= sdu_length_total;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].total_pdu_bytes+= TBS;
	eNB_mac_inst[Mod_id].eNB_UE_stats[next_ue].total_num_pdus+=1;

	if (mac_xface->lte_frame_parms->frame_type == TDD) {
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI++;
	  //	printf("DAI update: subframe %d: UE %d, DAI %d\n",subframe,next_ue,eNB_mac_inst[Mod_id].UE_template[next_ue].DAI);
	  
	  update_ul_dci(Mod_id,rnti,eNB_mac_inst[Mod_id].UE_template[next_ue].DAI);
	}

	switch (mac_xface->get_transmission_mode(Mod_id,rnti)) {
	case 1:
	case 2:
	default:
	  if (mac_xface->lte_frame_parms->frame_type == TDD) {
	    switch (mac_xface->lte_frame_parms->N_RB_DL) {
	    case 6:
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      break;
	    case 25:
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      break;
	    case 50:
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv = 0;
	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      break;
	    case 100:
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv = 0;
	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      break;
	    default:
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	      break;
	    }
	  }
	  else {
	    switch (mac_xface->lte_frame_parms->N_RB_DL) {
	    case 6:
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
	      break;
	    case 25:
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
	      break;
	    case 50:
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv = 0;
	      break;
	    case 100:
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv = 0;
	      break;
	    default:
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1;
	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
	      break;
	    }
	  }
	  break;
	case 4:
	  //  if (nb_rb>10) {
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs1 = mcs;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi1 = 1;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
	  ((DCI2_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;

	  //}
	  /* else {
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->mcs1 = eNB_UE_stats->DL_cqi[0];
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 1;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	     }*/
	  break;
	case 5:

	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = mcs;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  if(dl_pow_off[next_ue] == 2)
	    dl_pow_off[next_ue] = 1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = dl_pow_off[next_ue];
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
	  break;
	case 6:
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = mcs;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;
	  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
	  break;
	}



      }
    
      else {  // There is no data from RLC or MAC header, so don't schedule

      }
    }
    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      DAI = (eNB_mac_inst[Mod_id].UE_template[next_ue].DAI-1)&3;
      LOG_D(MAC,"[eNB %d] Frame %d: DAI %d for UE %d\n",Mod_id,frame,DAI,next_ue);
      // Save DAI for Format 0 DCI
    
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 0:
	//      if ((subframe==0)||(subframe==1)||(subframe==5)||(subframe==6))
	break;
      case 1:
	switch (subframe) {
	case 1:
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[7] = DAI;
	  break;
	case 4:
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[8] = DAI;
	  break;
	case 6:
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[2] = DAI;
	  break;
	case 9:
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[3] = DAI;
	  break;
	}
      case 2:
	//      if ((subframe==3)||(subframe==8))
	//	eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul = DAI;
	break;
      case 3:
	if ((subframe==6)||(subframe==8)||(subframe==0)) {
	  LOG_D(MAC,"schedule_ue_spec: setting UL DAI to %d for subframe %d => %d\n",DAI,subframe, ((subframe+8)%10)>>1);
	  eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul[((subframe+8)%10)>>1] = DAI;
	}
	break;
      case 4:
	//      if ((subframe==8)||(subframe==9))
	//	eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul = DAI;
	break;
      case 5:
	//      if (subframe==8)
	//	eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul = DAI;
	break;
      case 6:
	//      if ((subframe==1)||(subframe==4)||(subframe==6)||(subframe==9))
	//	eNB_mac_inst[Mod_id].UE_template[next_ue].DAI_ul = DAI;
	break;
      default:
	break;
      }
    }
    //printf("MAC nCCE : %d\n",*nCCE_used);
  }
}

void eNB_dlsch_ulsch_scheduler(u8 Mod_id,u8 cooperation_flag, u32 frame, u8 subframe) {//, int calibration_flag) {

  unsigned char nprb=0;
  unsigned int nCCE=0;
  int mbsfn_status=0;
  u32 RBalloc=0;

  DCI_PDU *DCI_pdu= &eNB_mac_inst[Mod_id].DCI_pdu;
  //  LOG_D(MAC,"[eNB %d] Frame %d, Subframe %d, entering MAC scheduler\n",Mod_id, frame, subframe);

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ENB_DLSCH_ULSCH_SCHEDULER,1);

  // clear DCI and BCCH contents before scheduling
  DCI_pdu->Num_common_dci  = 0;
  DCI_pdu->Num_ue_spec_dci = 0;
  eNB_mac_inst[Mod_id].bcch_active = 0;

#ifdef Rel10
  eNB_mac_inst[Mod_id].mcch_active =0;
#endif

  eNB_mac_inst[Mod_id].frame = frame;
  eNB_mac_inst[Mod_id].subframe = subframe;

  //if (subframe%5 == 0)
#ifdef EXMIMO 
  //pdcp_run(frame, 1, 0, Mod_id);
 
  if (pthread_mutex_lock (&pdcp_mutex) != 0) {
    LOG_E(PDCP,"Cannot lock mutex\n");
    //return(-1);
  }
  else {
    pdcp_instance_cnt++;
    pthread_mutex_unlock(&pdcp_mutex);
        
    if (pdcp_instance_cnt == 0) {
      if (pthread_cond_signal(&pdcp_cond) != 0) {
	LOG_E(PDCP,"pthread_cond_signal unsuccessfull\n");
	//return(-1);
      }
    }
    else {
      LOG_W(PDCP,"PDCP thread busy!!! inst_cnt=%d\n",pdcp_instance_cnt);
    }
  }
  
#endif
#ifdef CELLULAR
  rrc_rx_tx(Mod_id, frame, 0, 0);
#endif

#ifdef Rel10
  if (eNB_mac_inst[Mod_id].MBMS_flag >0) {

    mbsfn_status = schedule_MBMS(Mod_id,frame,subframe);
  }
#endif

  switch (subframe) {
  case 0:
    // FDD/TDD Schedule Downlink RA transmissions (RA response, Msg4 Contention resolution)
    // Schedule ULSCH for FDD or subframe 4 (TDD config 0,3,6)
    // Schedule Normal DLSCH
    
    schedule_RA(Mod_id,frame,subframe,2,&nprb,&nCCE);

    if (mac_xface->lte_frame_parms->frame_type == FDD) {  //FDD
      // schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,4,&nCCE);//,calibration_flag);
    }
    else if  ((mac_xface->lte_frame_parms->tdd_config == TDD) || //TDD
	      (mac_xface->lte_frame_parms->tdd_config == 3) ||
	      (mac_xface->lte_frame_parms->tdd_config == 6))
      //schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,4,&nCCE);//,calibration_flag);

      
      // schedule_ue_spec(Mod_id,subframe,nprb,&nCCE,mbsfn_status);

      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,1,mbsfn_status);
    break;

  case 1:
    // TDD, schedule UL for subframe 7 (TDD config 0,1) / subframe 8 (TDD Config 6)
    // FDD, schedule normal UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == TDD) { // TDD
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 0:
      case 1:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,7,&nCCE);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      case 6:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,8,&nCCE);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      default:
	break;
      }
    }
    else {  //FDD
      //schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE);
      // schedule_ue_spec(Mod_id,subframe,nprb,&nCCE,mbsfn_status);
      // fill_DLSCH_dci(Mod_id,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 2:
    // TDD, nothing 
    // FDD, normal UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == FDD) {  //FDD
      // schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE);
      // schedule_ue_spec(Mod_id,subframe,nprb,&nCCE,mbsfn_status);
      // fill_DLSCH_dci(Mod_id,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 3:
    // TDD Config 2, ULSCH for subframe 7
    // TDD Config 2/5 normal DLSCH
    // FDD, normal UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 2: 
      	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,7,&nCCE);
      case 5: 
	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      default:
	break;
      }
    }
    else { //FDD
      //	schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE,calibration_flag);
      // schedule_ue_spec(Mod_id,subframe,0,0,mbsfn_status);
      // fill_DLSCH_dci(Mod_id,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 4:
    // TDD Config 1, ULSCH for subframe 8
    // TDD Config 1/2/4/5 DLSCH
    // FDD UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == 1) { // TDD
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 1:
	//        schedule_RA(Mod_id,frame,subframe,&nprb,&nCCE);
        schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,8,&nCCE);
      case 2:
      case 4:
      case 5:
        schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
        fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,1,mbsfn_status);
        break;
      default:
        break;
      }
    }
    else {
      if (mac_xface->lte_frame_parms->frame_type == FDD) {  //FDD
	//	schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE,calibration_flag);
	// schedule_ue_spec(Mod_id,subframe,nprb,&nCCE,mbsfn_status);
	// fill_DLSCH_dci(Mod_id,subframe,RBalloc,0,mbsfn_status);
      }
    }
    break;

  case 5:
    // TDD/FDD Schedule SI
    // TDD Config 0,6 ULSCH for subframes 9,3 resp.
    // TDD normal DLSCH
    // FDD normal UL/DLSCH
    schedule_SI(Mod_id,frame,&nprb,&nCCE);
    //schedule_RA(Mod_id,frame,subframe,5,&nprb,&nCCE);
    if ((mac_xface->lte_frame_parms->frame_type == FDD) ) {
      schedule_RA(Mod_id,frame,subframe,1,&nprb,&nCCE);
      //schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,1,mbsfn_status);

    }
    else if ((mac_xface->lte_frame_parms->tdd_config == 0) || // TDD Config 0
	     (mac_xface->lte_frame_parms->tdd_config == 6)) { // TDD Config 6
      //schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    else {
      //schedule_ue_spec(Mod_id,subframe,nprb,&nCCE,mbsfn_status);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 6:
    // TDD Config 0,1,6 ULSCH for subframes 2,3
    // TDD Config 3,4,5 Normal DLSCH
    // FDD normal ULSCH/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == TDD) { // TDD
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 0:
	break;
      case 1:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,2,&nCCE);
	//	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      case 6:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,3,&nCCE);
	//	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      case 5:
	schedule_RA(Mod_id,frame,subframe,2,&nprb,&nCCE);
	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,1,mbsfn_status);
	break;
      case 3:
      case 4:
	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
     
      default:
	break;
      }
    }
    else {  //FDD
      //	schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE,calibration_flag);
      schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    break;
 
  case 7:
    // TDD Config 3,4,5 Normal DLSCH
    // FDD Normal UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == TDD) { // TDD
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 3:
      case 4:
	//	  schedule_RA(Mod_id,frame,subframe,3,&nprb,&nCCE);  // 3 = Msg3 subframe, not
	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status); //1,mbsfn_status);
	break;
      case 5:
	schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      default:
	break;
      }
    }
    else {  //FDD
      //	schedule_ulsch(Mod_id,cooperation_flag,subframe,&nCCE,calibration_flag);
      schedule_ue_spec(Mod_id,frame,subframe,nprb,&nCCE,mbsfn_status);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 8:
    // TDD Config 2,3,4,5 ULSCH for subframe 2
    //
    // FDD Normal UL/DLSCH
    if (mac_xface->lte_frame_parms->frame_type == TDD) { // TDD
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 2:
      case 3:
      case 4:
      case 5:
	
	//	schedule_RA(Mod_id,subframe,&nprb,&nCCE);
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,2,&nCCE);
	schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      default:
	break;
      }
    }
    else {  //FDD
      schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,2,&nCCE);
      schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    break;

  case 9:
    // TDD Config 1,3,4,6 ULSCH for subframes 3,3,3,4
    if (mac_xface->lte_frame_parms->frame_type == TDD) {
      switch (mac_xface->lte_frame_parms->tdd_config) {
      case 1:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,3,&nCCE);
	schedule_RA(Mod_id,frame,subframe,7,&nprb,&nCCE);  // 7 = Msg3 subframe, not
	schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,1,mbsfn_status);
	break;
      case 3:
      case 4:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,3,&nCCE);
	schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      case 6:
	schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,4,&nCCE);
	//schedule_RA(Mod_id,frame,subframe,&nprb,&nCCE);
	schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      case 2:
      case 5:
	//schedule_RA(Mod_id,frame,subframe,&nprb,&nCCE);
	schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
	fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
	break;
      default:
	break;
      }
    }
    else {  //FDD
      schedule_ulsch(Mod_id,frame,cooperation_flag,subframe,3,&nCCE);
      schedule_ue_spec(Mod_id,frame,subframe,0,&nCCE,mbsfn_status);
      fill_DLSCH_dci(Mod_id,frame,subframe,RBalloc,0,mbsfn_status);
    }
    break;
    
  }

  DCI_pdu->nCCE = nCCE;
  LOG_D(MAC,"frame %d, subframe %d nCCE %d\n",frame,subframe,nCCE);

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ENB_DLSCH_ULSCH_SCHEDULER,0);
}
