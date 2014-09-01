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

/*! \file eNB_scheduler_ulsch.c
 * \brief eNB procedures for the ULSCH transport channel
 * \author Navid Nikaein and Raymond Knopp
 * \date 2010 - 2014
 * \email: navid.nikaein@eurecom.fr
 * \version 1.0
 * @ingroup _mac

 */

#include "assertions.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/proto.h"
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

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#define ENABLE_MAC_PAYLOAD_DEBUG
#define DEBUG_eNB_SCHEDULER 1


// This table holds the allowable PRB sizes for ULSCH transmissions
uint8_t rb_table[33] = {1,2,3,4,5,6,8,9,10,12,15,16,18,20,24,25,27,30,32,36,40,45,48,50,54,60,72,75,80,81,90,96,100};

void rx_sdu(module_id_t enb_mod_idP,int CC_id,frame_t frameP,rnti_t rntiP,uint8_t *sdu, uint16_t sdu_len) {

  unsigned char  rx_ces[MAX_NUM_CE],num_ce,num_sdu,i,*payload_ptr;
  unsigned char  rx_lcids[NB_RB_MAX];
  unsigned short rx_lengths[NB_RB_MAX];
  int    UE_id = find_UE_id(enb_mod_idP,rntiP);
  int ii,j;
  eNB_MAC_INST *eNB = &eNB_mac_inst[enb_mod_idP];
  UE_list_t *UE_list= &eNB->UE_list;

  start_meas(&eNB->rx_ulsch_sdu);
  
  if ((UE_id >  NUMBER_OF_UE_MAX) || (UE_id == -1)  )
    for(ii=0; ii<NB_RB_MAX; ii++) rx_lengths[ii] = 0;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,1);

  LOG_D(MAC,"[eNB %d] Received ULSCH sdu from PHY (rnti %x, UE_id %d), parsing header\n",enb_mod_idP,rntiP,UE_id);
  payload_ptr = parse_ulsch_header(sdu,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,sdu_len);

  // control element
  for (i=0;i<num_ce;i++) {

    switch (rx_ces[i]) { // implement and process BSR + CRNTI +
    case POWER_HEADROOM:
      if (UE_id != UE_INDEX_INVALID ){
	UE_list->UE_template[CC_id][UE_id].phr_info =  (payload_ptr[0] & 0x3f);// - PHR_MAPPING_OFFSET;
	LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received PHR PH = %d (db)\n", rx_ces[i], UE_list->UE_template[CC_id][UE_id].phr_info);
      }
      payload_ptr+=sizeof(POWER_HEADROOM_CMD);
      break;
    case CRNTI:
      LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received CRNTI %d \n", rx_ces[i], payload_ptr[0]);
      payload_ptr+=1;
      break;
    case TRUNCATED_BSR:
    case SHORT_BSR: {
      if (UE_id  != UE_INDEX_INVALID ){
	uint8_t lcgid;
	lcgid = (payload_ptr[0] >> 6);
	LOG_D(MAC, "[eNB] MAC CE_LCID %d : Received short BSR LCGID = %u bsr = %d\n",
	      rx_ces[i], lcgid, payload_ptr[0] & 0x3f);
	UE_list->UE_template[CC_id][UE_id].bsr_info[lcgid] = (payload_ptr[0] & 0x3f);
      }
      payload_ptr += 1;//sizeof(SHORT_BSR); // fixme
    } break;
    case LONG_BSR:
      if (UE_id  != UE_INDEX_INVALID ){
	UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID0] = ((payload_ptr[0] & 0xFC) >> 2);
	UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID1] =
	  ((payload_ptr[0] & 0x03) << 4) | ((payload_ptr[1] & 0xF0) >> 4);
	UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID2] =
	  ((payload_ptr[1] & 0x0F) << 2) | ((payload_ptr[2] & 0xC0) >> 6);
	UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID3] = (payload_ptr[2] & 0x3F);
	LOG_D(MAC, "[eNB] MAC CE_LCID %d: Received long BSR LCGID0 = %u LCGID1 = "
	      "%u LCGID2 = %u LCGID3 = %u\n",
	      rx_ces[i],
	      UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID0],
	      UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID1],
	      UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID2],
	      UE_list->UE_template[CC_id][UE_id].bsr_info[LCGID3]);
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
	      eNB->common_channels[CC_id].RA_template[ii].rnti, rntiP,
	      eNB->common_channels[CC_id].RA_template[ii].RA_active);
	
	if ((eNB->common_channels[CC_id].RA_template[ii].rnti==rntiP) &&
	    (eNB->common_channels[CC_id].RA_template[ii].RA_active==TRUE)) {
	  
          //payload_ptr = parse_ulsch_header(msg3,&num_ce,&num_sdu,rx_ces,rx_lcids,rx_lengths,msg3_len);
	  
	  if (UE_id < 0) {
	    memcpy(&eNB->common_channels[CC_id].RA_template[ii].cont_res_id[0],payload_ptr,6);
	    LOG_I(MAC,"[eNB %d][RAPROC] Frame %d CCCH: Received RRCConnectionRequest: length %d, offset %d\n",
                  enb_mod_idP,frameP,rx_lengths[ii],payload_ptr-sdu);
	    if ((UE_id=add_new_ue(enb_mod_idP,CC_id,eNB->common_channels[CC_id].RA_template[ii].rnti)) == -1 )
	      mac_xface->macphy_exit("[MAC][eNB] Max user count reached\n");
	    else 
	      LOG_I(MAC,"[eNB %d][RAPROC] Frame %d Added user with rnti %x => UE %d\n",
		    enb_mod_idP,frameP,eNB->common_channels[CC_id].RA_template[ii].rnti,UE_id);
	  } else {
	    LOG_I(MAC,"[eNB %d][RAPROC] Frame %d CCCH: Received RRCConnectionReestablishment from UE %d: length %d, offset %d\n",
		  enb_mod_idP,frameP,UE_id,rx_lengths[ii],payload_ptr-sdu);
	  }
	  
	  if (Is_rrc_registered == 1)
	    mac_rrc_data_ind(enb_mod_idP,frameP,CCCH,(uint8_t *)payload_ptr,rx_lengths[ii],1,enb_mod_idP,0);
	  
	  
          if (num_ce >0) {  // handle msg3 which is not RRCConnectionRequest
	    //	process_ra_message(msg3,num_ce,rx_lcids,rx_ces);
	  }
	  
	  eNB->common_channels[CC_id].RA_template[ii].generate_Msg4 = 1;
	  eNB->common_channels[CC_id].RA_template[ii].wait_ack_Msg4 = 0;
	  
	  
	} // if process is active
	
      } // loop on RA processes
      
      break;
    case  DCCH : 
    case DCCH1 :
      //      if(eNB_mac_inst[module_idP][CC_id].Dcch_lchan[UE_id].Active==1){
      
#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
      for (j=0;j<32;j++)
	LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n");
#endif
      
      //  This check is just to make sure we didn't get a bogus SDU length, to be removed ...
      if (rx_lengths[i]<CCCH_PAYLOAD_SIZE_MAX) {
	LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DCCH, received %d bytes form UE %d on LCID %d(%d) \n",
	      enb_mod_idP,frameP, rx_lengths[i], UE_id, rx_lcids[i], rx_lcids[i]);
	
	mac_rlc_data_ind(enb_mod_idP,UE_id, frameP,ENB_FLAG_YES,MBMS_FLAG_NO,
			 rx_lcids[i],
			 (char *)payload_ptr,
			 rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_rx[rx_lcids[i]]+=1;
	UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];
	
      }
      //      }
      break;
    case DTCH: // default DRB 
      //      if(eNB_mac_inst[module_idP][CC_id].Dcch_lchan[UE_id].Active==1){
	
#if defined(ENABLE_MAC_PAYLOAD_DEBUG)
      LOG_T(MAC,"offset: %d\n",(unsigned char)((unsigned char*)payload_ptr-sdu));
      for (j=0;j<32;j++)
	LOG_T(MAC,"%x ",payload_ptr[j]);
      LOG_T(MAC,"\n");
#endif
      
      LOG_D(MAC,"[eNB %d] Frame %d : ULSCH -> UL-DTCH, received %d bytes from UE %d for lcid %d (%d)\n",
	    enb_mod_idP,frameP, rx_lengths[i], UE_id,rx_lcids[i],rx_lcids[i]);
      
      if ((rx_lengths[i] <SCH_PAYLOAD_SIZE_MAX) &&  (rx_lengths[i] > 0) ) {   // MAX SIZE OF transport block
	mac_rlc_data_ind(enb_mod_idP,UE_id, frameP,ENB_FLAG_YES,MBMS_FLAG_NO,
			 DTCH,
			 (char *)payload_ptr,
			 rx_lengths[i],
			 1,
			 NULL);//(unsigned int*)crc_status);
	UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_rx[rx_lcids[i]]+=1;
	UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_rx[rx_lcids[i]]+=rx_lengths[i];
	  
      }
      //      }
      break;
    default :  //if (rx_lcids[i] >= DTCH) {
      UE_list->eNB_UE_stats[CC_id][UE_id].num_errors_rx+=1;
      LOG_E(MAC,"[eNB %d] received unsupported or unknown LCID %d from UE %d ", rx_lcids[i], UE_id);
      break;
    }
    payload_ptr+=rx_lengths[i];
    
  }

  UE_list->eNB_UE_stats[CC_id][UE_id].total_pdu_bytes_rx+=sdu_len;
  UE_list->eNB_UE_stats[CC_id][UE_id].total_num_pdus_rx+=1;
  
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,0);
  stop_meas(&eNB->rx_ulsch_sdu);
}


uint32_t bytes_to_bsr_index(int32_t nbytes) {

  uint32_t i=0;

  if (nbytes<0)
    return(0);

  while ((i<BSR_TABLE_SIZE)&&
	 (BSR_TABLE[i]<=nbytes)){
    i++;
  }
  return(i-1);
}


void add_ue_ulsch_info(module_id_t module_idP, int CC_id, int UE_id, sub_frame_t subframeP, UE_ULSCH_STATUS status){

  eNB_ulsch_info[module_idP][UE_id].rnti             = UE_RNTI(module_idP,UE_id);
  eNB_ulsch_info[module_idP][UE_id].subframe         = subframeP;
  eNB_ulsch_info[module_idP][UE_id].status           = status;

  eNB_ulsch_info[module_idP][UE_id].serving_num++;

}

// This seems not to be used anymore
/*
int schedule_next_ulue(module_id_t module_idP, int UE_id, sub_frame_t subframeP){

  int next_ue;

  // first phase: scheduling for ACK
  switch (subframeP) {
    // scheduling for subframeP 2: for scheduled user during subframeP 5 and 6
  case 8:
    if  ((eNB_dlsch_info[module_idP][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[module_idP][UE_id].subframe == 5 || eNB_dlsch_info[module_idP][UE_id].subframe == 6)){
      // set the downlink status
      eNB_dlsch_info[module_idP][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
    // scheduling for subframeP 3: for scheduled user during subframeP 7 and 8
  case 9:
    if  ((eNB_dlsch_info[module_idP][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[module_idP][UE_id].subframe == 7 || eNB_dlsch_info[module_idP][UE_id].subframe == 8)){
      eNB_dlsch_info[module_idP][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
    // scheduling UL subframeP 4: for scheduled user during subframeP 9 and 0
  case 0 :
    if  ((eNB_dlsch_info[module_idP][UE_id].status == S_DL_SCHEDULED) &&
	 (eNB_dlsch_info[module_idP][UE_id].subframe == 9 || eNB_dlsch_info[module_idP][UE_id].subframe == 0)){
      eNB_dlsch_info[module_idP][UE_id].status = S_DL_BUFFERED;
      return UE_id;
    }
    break;
  default:
    break;
  }

  // second phase
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ ){

    if  (eNB_ulsch_info[module_idP][next_ue].status == S_UL_WAITING )
      return next_ue;
    else if (eNB_ulsch_info[module_idP][next_ue].status == S_UL_SCHEDULED){
      eNB_ulsch_info[module_idP][next_ue].status = S_UL_BUFFERED;
    }
  }
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ ){
    if (eNB_ulsch_info[module_idP][next_ue].status != S_UL_NONE )// do this just for active UEs
      eNB_ulsch_info[module_idP][next_ue].status = S_UL_WAITING;
  }
  next_ue = 0;
  return next_ue;

}
*/





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
	  ce_len+=3;
	else if (lcid==CRNTI)
	  ce_len+=2;
	else if ((lcid==POWER_HEADROOM) || (lcid==TRUNCATED_BSR)|| (lcid== SHORT_BSR))
	  ce_len++;
	else {
	  LOG_E(MAC,"unknown CE %d \n", lcid);
	  mac_xface->macphy_exit("unknown CE");
	}
      }
    }
  }
  *num_ce = num_ces;
  *num_sdu = num_sdus;

  return(mac_header_ptr);
}


void schedule_ulsch(module_id_t module_idP, frame_t frameP,unsigned char cooperation_flag,sub_frame_t subframeP, unsigned char sched_subframe,unsigned int *nCCE) {//,int calibration_flag) {


  unsigned int nCCE_available[MAX_NUM_CCs];
  uint16_t first_rb[MAX_NUM_CCs],i;
  int CC_id;
  eNB_MAC_INST *eNB=&eNB_mac_inst[module_idP];

  start_meas(&eNB->schedule_ulsch);


  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    
    first_rb[CC_id] = 1;
    nCCE_available[CC_id] = mac_xface->get_nCCE_max(module_idP,CC_id) - nCCE[CC_id];
    
    // UE data info;
    // check which UE has data to transmit
    // function to decide the scheduling
    // e.g. scheduling_rslt = Greedy(granted_UEs, nb_RB)
    
    // default function for default scheduling
    //
    
    // output of scheduling, the UE numbers in RBs, where it is in the code???
    // check if RA (Msg3) is active in this subframeP, if so skip the PRBs used for Msg3
    // Msg3 is using 1 PRB so we need to increase first_rb accordingly
    // not sure about the break (can there be more than 1 active RA procedure?)
    
    for (i=0;i<NB_RA_PROC_MAX;i++) {
      if ((eNB->common_channels[CC_id].RA_template[i].RA_active == TRUE) &&
          (eNB->common_channels[CC_id].RA_template[i].generate_rar == 0) &&
          (eNB->common_channels[CC_id].RA_template[i].Msg3_subframe == sched_subframe)) {
	first_rb[CC_id]++;
	break;
      }
    }
  }   

  schedule_ulsch_rnti(module_idP, cooperation_flag, frameP, subframeP, sched_subframe, nCCE, nCCE_available, first_rb);
    
#ifdef CBA
  schedule_ulsch_cba_rnti(module_idP, cooperation_flag, frameP, subframeP, sched_subframe, granted_UEs, nCCE, &nCCE_available, &first_rb);
#endif


  stop_meas(&eNB->schedule_ulsch);
  
}



void schedule_ulsch_rnti(module_id_t   module_idP,
                         unsigned char cooperation_flag,
                         frame_t       frameP,
                         sub_frame_t   subframeP,
                         unsigned char sched_subframe,
                         unsigned int *nCCE,
                         unsigned int *nCCE_available,
                         uint16_t          *first_rb){

  int             UE_id;
  unsigned char      aggregation    = 2;
  rnti_t             rnti           = -1;
  uint8_t                 round          = 0;
  uint8_t                 harq_pid       = 0;
  void              *ULSCH_dci      = NULL;
  LTE_eNB_UE_stats  *eNB_UE_stats   = NULL;
  DCI_PDU           *DCI_pdu; 
  uint8_t                 status         = 0;
  uint8_t                 rb_table_index = -1;
  uint16_t                TBS,i;
  uint32_t                buffer_occupancy;
  uint32_t                tmp_bsr;
  uint32_t                cqi_req,cshift,ndi,mcs,rballoc;

  int n,CC_id;
  eNB_MAC_INST      *eNB=&eNB_mac_inst[module_idP];
  UE_list_t         *UE_list=&eNB->UE_list;
  UE_TEMPLATE       *UE_template;

  int rvidx_tab[4] = {0,3,1,2};


  // loop over all active UEs
  for (UE_id=UE_list->head;(UE_id>=0) && (*nCCE_available > (1<<aggregation));UE_id=UE_list->next[UE_id]) {
    rnti = UE_RNTI(module_idP,UE_id); // radio network temp id is obtained
    if (rnti==0) // if so, go to next UE
      continue;

    // loop over all active UL CC_ids for this UE
    for (n=0;n<UE_list->numactiveULCCs[UE_id];n++) {
      // This is the actual CC_id in the list
      CC_id = UE_list->ordered_ULCCids[n][UE_id];

      DCI_pdu = &eNB->common_channels[CC_id].DCI_pdu;
      UE_template = &UE_list->UE_template[CC_id][UE_id];


      eNB_UE_stats = mac_xface->get_eNB_UE_stats(module_idP,CC_id,rnti);
      if (eNB_UE_stats==NULL)
        mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

      LOG_I(MAC,"[eNB %d] Scheduler Frame %d, subframeP %d, nCCE %d: Checking ULSCH next UE_id %d mode id %d (rnti %x,mode %s), format 0\n",
	    module_idP,frameP,subframeP,*nCCE,UE_id,module_idP, rnti,mode_string[eNB_UE_stats->mode]);

      if (eNB_UE_stats->mode == PUSCH) { // ue has a ulsch channel
	int8_t ret;
	// Get candidate harq_pid from PHY
	ret = mac_xface->get_ue_active_harq_pid(module_idP,CC_id,rnti,frameP,subframeP,&harq_pid,&round,1);
	LOG_I(MAC,"Got harq_pid %d, round %d, UE_id %d (UE_to_be_scheduled %d)\n",harq_pid,round,UE_id,
	      UE_is_to_be_scheduled(module_idP,CC_id,UE_id));

	/* [SR] 01/07/13: Don't schedule UE if we cannot get harq pid */
	//#ifndef EXMIMO_IOT
	//	if ((((UE_is_to_be_scheduled(module_idP,CC_id,UE_id)>0)) || (round>0) || ((frameP%10)==0)) && (ret == 0))
	  // if there is information on bsr of DCCH, DTCH or if there is UL_SR, or if there is a packet to retransmit, or we want to schedule a periodic feedback every 10 frames
	//#else
	  if (round==0)
	    //#endif
	    {
	      LOG_D(MAC,"[eNB %d][PUSCH %d] Frame %d subframeP %d Scheduling UE %d round %d (SR %d)\n",
                    module_idP,harq_pid,frameP,subframeP,UE_id,round,
                    UE_template->ul_SR);

	      // reset the scheduling request
	      UE_template->ul_SR = 0;

	      aggregation = process_ue_cqi(module_idP,UE_id); // =2 by default!!
	      //    msg("[MAC][eNB] subframeP %d: aggregation %d\n",subframeP,aggregation);

	      status = mac_get_rrc_status(module_idP,1,UE_id);

	      if (status < RRC_CONNECTED)
		cqi_req = 0;
	      else
		cqi_req = 1;



	      if (round > 0) {
		ndi = UE_template->oldNDI_UL[harq_pid];
		if ((round&3)==0)
		  mcs = openair_daq_vars.target_ue_ul_mcs;
		else
		  mcs = rvidx_tab[round&3] + 28; //not correct for round==4!
	      }
	      else {
		ndi = 1-UE_template->oldNDI_UL[harq_pid];
		UE_template->oldNDI_UL[harq_pid]=ndi;
		mcs = openair_daq_vars.target_ue_ul_mcs;
	      }

	      LOG_D(MAC,"[eNB %d] ULSCH scheduler: Ndi %d, mcs %d\n",module_idP,ndi,mcs);

	      /*	      if((cooperation_flag > 0) && (UE_id == 1)) { // Allocation on same set of RBs
		// RIV:resource indication value // function in openair1/PHY/LTE_TRANSPORT/dci_tools.c
		rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
						((UE_id-1)*4),//openair_daq_vars.ue_ul_nb_rb),
						4);//openair_daq_vars.ue_ul_nb_rb);
						}*/

	      if (round==0) {
		rb_table_index = 1;
		TBS = mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
		buffer_occupancy = ((UE_template->bsr_info[LCGID0]  == 0) &&
				    (UE_template->bsr_info[LCGID1]  == 0) &&
				    (UE_template->bsr_info[LCGID2]  == 0) &&
				    (UE_template->bsr_info[LCGID3] == 0))?
		  BSR_TABLE[11] :   // This is when we've received SR and buffers are fully served
		  BSR_TABLE[UE_template->bsr_info[LCGID0]]+
		  BSR_TABLE[UE_template->bsr_info[LCGID1]]+
		  BSR_TABLE[UE_template->bsr_info[LCGID2]]+
		  BSR_TABLE[UE_template->bsr_info[LCGID3]];  // This is when remaining data in UE buffers (even if SR is triggered)

		LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframeP %d Scheduled UE, BSR for LCGID0 %d, LCGID1 %d, LCGID2 %d LCGID3 %d, BO %d\n",
		      module_idP,
		      UE_id,
		      rnti,
		      frameP,
		      subframeP,
		      UE_template->bsr_info[LCGID0],
		      UE_template->bsr_info[LCGID1],
		      UE_template->bsr_info[LCGID2],
		      UE_template->bsr_info[LCGID3],
		      buffer_occupancy);

		while ((TBS < buffer_occupancy) &&
		       rb_table[rb_table_index]<(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb)){
		  // continue until we've exhauster the UEs request or the total number of available PRBs
		  /*	    LOG_I(MAC,"[eNB %d][PUSCH %x] Frame %d subframeP %d Scheduled UE (rb_table_index %d => TBS %d)\n",
			    module_idP,rnti,frameP,subframeP,
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

		LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframeP %d Scheduled UE (mcs %d, first rb %d, nb_rb %d, rb_table_index %d, TBS %d, harq_pid %d)\n",
		      module_idP,harq_pid,rnti,frameP,subframeP,mcs,
		      *first_rb,rb_table[rb_table_index],
		      rb_table_index,mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]),
		      harq_pid);

		rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
						*first_rb,
						rb_table[rb_table_index]);//openair_daq_vars.ue_ul_nb_rb);

		*first_rb+=rb_table[rb_table_index];  // increment for next UE allocation
		
		UE_template->nb_rb_ul[harq_pid] = rb_table[rb_table_index]; //store for possible retransmission

		buffer_occupancy -= mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
		i = bytes_to_bsr_index((int32_t)buffer_occupancy);

		// Adjust BSR entries for LCGIDs
		if (i>0) {
		  if (UE_template->bsr_info[LCGID0] <= i) {
		    tmp_bsr = BSR_TABLE[UE_template->bsr_info[LCGID0]];
		    UE_template->bsr_info[LCGID0] = 0;
		    if (BSR_TABLE[UE_template->bsr_info[LCGID1]] <= (buffer_occupancy-tmp_bsr)) {
		      tmp_bsr += BSR_TABLE[UE_template->bsr_info[LCGID1]];
		      UE_template->bsr_info[LCGID1] = 0;
		      if (BSR_TABLE[UE_template->bsr_info[LCGID2]] <= (buffer_occupancy-tmp_bsr)) {
			tmp_bsr += BSR_TABLE[UE_template->bsr_info[LCGID2]];
			UE_template->bsr_info[LCGID2] = 0;
			if (BSR_TABLE[UE_template->bsr_info[LCGID3]] <= (buffer_occupancy-tmp_bsr)) {
			  tmp_bsr += BSR_TABLE[UE_template->bsr_info[LCGID3]];
			  UE_template->bsr_info[LCGID3] = 0;
			} else {
			  UE_template->bsr_info[LCGID3] = bytes_to_bsr_index((int32_t)BSR_TABLE[UE_template->bsr_info[LCGID3]] - ((int32_t)buffer_occupancy - (int32_t)tmp_bsr));
			}
		      }
		      else {
			UE_template->bsr_info[LCGID2] = bytes_to_bsr_index((int32_t)BSR_TABLE[UE_template->bsr_info[LCGID2]] - ((int32_t)buffer_occupancy -(int32_t)tmp_bsr));
		      }
		    }
		    else {
		      UE_template->bsr_info[LCGID1] = bytes_to_bsr_index((int32_t)BSR_TABLE[UE_template->bsr_info[LCGID1]] - (int32_t)buffer_occupancy);
		    }
		  }
		  else {
		    UE_template->bsr_info[LCGID0] = bytes_to_bsr_index((int32_t)BSR_TABLE[UE_template->bsr_info[LCGID0]] - (int32_t)buffer_occupancy);
		  }
		}
		else {  // we have flushed all buffers so clear bsr
		  UE_template->bsr_info[LCGID0] = 0;
		  UE_template->bsr_info[LCGID1] = 0;
		  UE_template->bsr_info[LCGID2] = 0;
		  UE_template->bsr_info[LCGID3] = 0;
		}



	      } // ndi==1
	      else { //we schedule a retransmission
		LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframeP %d Scheduled UE retransmission (mcs %d, first rb %d, nb_rb %d, TBS %d, harq_pid %d)\n",
		      module_idP,UE_id,rnti,frameP,subframeP,mcs,
		      *first_rb,UE_template->nb_rb_ul[harq_pid],
		      mac_xface->get_TBS_UL(mcs,UE_template->nb_rb_ul[harq_pid]),
		      harq_pid);
		
		rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
						*first_rb,
						UE_template->nb_rb_ul[harq_pid]);
		*first_rb+=UE_template->nb_rb_ul[harq_pid];  // increment for next UE allocation
	      }

	      // Cyclic shift for DM RS
	      if(cooperation_flag == 2) {
		if(UE_id == 1)// For Distriibuted Alamouti, cyclic shift applied to 2nd UE
		  cshift = 1;
		else
		  cshift = 0;
	      }
	      else
		cshift = 0;// values from 0 to 7 can be used for mapping the cyclic shift (36.211 , Table 5.5.2.1.1-1)

	      if (mac_xface->lte_frame_parms->frame_type == TDD) {
		switch (mac_xface->lte_frame_parms->N_RB_UL) {
		case 6:
		  ULSCH_dci = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
		  ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = UE_template->DAI_ul[sched_subframe];
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
		  ULSCH_dci = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
		  ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = UE_template->DAI_ul[sched_subframe];
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
		  ULSCH_dci = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->dai      = UE_template->DAI_ul[sched_subframe];
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
		  ULSCH_dci = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
		  ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->dai      = UE_template->DAI_ul[sched_subframe];
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

		  ULSCH_dci          = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_5MHz_FDD_t *)ULSCH_dci)->padding  = 0;
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
		  ULSCH_dci          = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->cshift   = cshift;
		  ((DCI0_1_5MHz_FDD_t *)ULSCH_dci)->padding  = 0;
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
		  ULSCH_dci          = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_10MHz_FDD_t *)ULSCH_dci)->padding  = 0;
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
		  ULSCH_dci          = UE_template->ULSCH_DCI[harq_pid];

		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->type     = 0;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->hopping  = 0;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->rballoc  = rballoc;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->mcs      = mcs;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->ndi      = ndi;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->TPC      = 1;
		  ((DCI0_20MHz_FDD_t *)ULSCH_dci)->padding  = 0;
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
	      add_ue_ulsch_info(module_idP,
				CC_id,
				UE_id,
				subframeP,
				S_UL_SCHEDULED);

	      nCCE[CC_id] = nCCE[CC_id] + (1<<aggregation);
	      nCCE_available[CC_id] = mac_xface->get_nCCE_max(module_idP,CC_id) - nCCE[CC_id];
	      //msg("[MAC][eNB %d][ULSCH Scheduler] Frame %d, subframeP %d: Generated ULSCH DCI for next UE_id %d, format 0\n", module_idP,frameP,subframeP,UE_id);

	      //break; // leave loop after first UE is schedule (avoids m
	    } // UE_is_to_be_scheduled
      } // UE is in PUSCH
    } // loop of CC_id
  } // loop over UE_id
}

#ifdef CBA
void schedule_ulsch_cba_rnti(module_id_t module_idP, unsigned char cooperation_flag, frame_t frameP, sub_frame_t subframeP, unsigned char sched_subframe, uint8_t granted_UEs, unsigned int *nCCE, unsigned int *nCCE_available, uint16_t *first_rb){

  DCI0_5MHz_TDD_1_6_t *ULSCH_dci_tdd16;
  DCI0_5MHz_FDD_t *ULSCH_dci_fdd;
  DCI_PDU *DCI_pdu;

  uint8_t rb_table_index=0, aggregation=2;
  uint32_t rballoc;
  uint8_t cba_group, cba_resources;
  uint8_t required_rbs[NUM_MAX_CBA_GROUP], weight[NUM_MAX_CBA_GROUP], num_cba_resources[NUM_MAX_CBA_GROUP];
  uint8_t available_rbs= ceil(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb);
  uint8_t remaining_rbs= available_rbs;
  uint8_t allocated_rbs;
  // We compute the weight of each group and initialize some variables

  

  for (cba_group=0;cba_group<eNB_mac_inst[module_idP][CC_id].num_active_cba_groups;cba_group++) {
    // UEs in PUSCH with traffic
    weight[cba_group] = find_num_active_UEs_in_cbagroup(module_idP, cba_group);
    required_rbs[cba_group] = 0;
    num_cba_resources[cba_group]=0;
  }
  //LOG_D(MAC, "[eNB ] CBA granted ues are %d\n",granted_UEs );

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {

    if ((eNB_mac_inst[module_idP][CC_id].num_active_cba_groups > 0) && (nCCE[CC_id] == 0)) {
      DCI_pdu = &eNB_mac_inst[module_idP][CC_id].DCI_pdu;

      for (cba_group=0;cba_group<eNB_mac_inst[module_idP][CC_id].num_active_cba_groups  && (nCCE_available[CC_id] > (1<<aggregation));cba_group++) {
	if (remaining_rbs <= 0 )
	  break;
	// If the group needs some resource
	if ((weight[cba_group] > 0) && eNB_mac_inst[module_idP][CC_id].cba_rnti[cba_group] != 0){
	  // to be refined in case of : granted_UEs >> weight[cba_group]*available_rbs
	  required_rbs[cba_group] = (uint8_t)ceil((weight[cba_group]*available_rbs)/granted_UEs);

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
	    num_cba_resources[cba_group]=(uint8_t)ceil(weight[cba_group]/2.0);
	  else
	    num_cba_resources[cba_group]=(uint8_t)ceil(required_rbs[cba_group]/2.0);

	  while (nCCE[CC_id] + (1<<aggregation) * num_cba_resources[cba_group] > nCCE_available[CC_id])
	    num_cba_resources[cba_group]--;

	  LOG_N(MAC,"[eNB %d] Frame %d, subframeP %d: cba group %d weight/granted_ues %d/%d available/required rb (%d/%d), num resources %d->1 (*scaled down*) \n",
		module_idP, frameP, subframeP, cba_group,
		weight[cba_group], granted_UEs, available_rbs,required_rbs[cba_group],
		num_cba_resources[cba_group]);

	  num_cba_resources[cba_group]=1;

	}
      }
      // phase 2
      for (cba_group=0;cba_group<eNB_mac_inst[module_idP][CC_id].num_active_cba_groups;cba_group++) {
	for (cba_resources=0; cba_resources < num_cba_resources[cba_group]; cba_resources++){
	  rb_table_index =0;
	  // check if there was an allocation for this group in the 1st phase
	  if (required_rbs[cba_group] == 0 )
	    continue;

	  while (rb_table[rb_table_index] < (uint8_t) ceil(required_rbs[cba_group] / num_cba_resources[cba_group]) )
	    rb_table_index++;

	  while (rb_table[rb_table_index] > remaining_rbs )
	    rb_table_index--;

	  remaining_rbs-=rb_table[rb_table_index];
	  allocated_rbs=rb_table[rb_table_index];

	  rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
					  *first_rb,
					  rb_table[rb_table_index]);

	  *first_rb+=rb_table[rb_table_index];
	  LOG_D(MAC,"[eNB %d] Frame %d, subframeP %d: CBA %d rnti %x, total/required/allocated/remaining rbs (%d/%d/%d/%d), rballoc %d, nCCE (%d/%d)\n",
		module_idP, frameP, subframeP, cba_group,eNB_mac_inst[module_idP][CC_id].cba_rnti[cba_group],
		available_rbs, required_rbs[cba_group], allocated_rbs, remaining_rbs,rballoc,
		nCCE_available[CC_id],nCCE[CC_id]);


	  if (mac_xface->lte_frame_parms->frame_type == TDD) {
	    ULSCH_dci_tdd16 = (DCI0_5MHz_TDD_1_6_t *)UE_list->UE_template[CC_id][cba_group].ULSCH_DCI[0];

	    ULSCH_dci_tdd16->type     = 0;
	    ULSCH_dci_tdd16->hopping  = 0;
	    ULSCH_dci_tdd16->rballoc  = rballoc;
	    ULSCH_dci_tdd16->mcs      = 2;
	    ULSCH_dci_tdd16->ndi      = 1;
	    ULSCH_dci_tdd16->TPC      = 1;
	    ULSCH_dci_tdd16->cshift   = cba_group;
	    ULSCH_dci_tdd16->dai      = UE_list->UE_template[CC_id][cba_group].DAI_ul[sched_subframe];
	    ULSCH_dci_tdd16->cqi_req  = 1;

	    //add_ue_spec_dci
	    add_common_dci(DCI_pdu,
			   ULSCH_dci_tdd16,
			   eNB_mac_inst[module_idP][CC_id].cba_rnti[cba_group],
			   sizeof(DCI0_5MHz_TDD_1_6_t),
			   aggregation,
			   sizeof_DCI0_5MHz_TDD_1_6_t,
			   format0,
			   0);
	  }
	  else {
	    ULSCH_dci_fdd           = (DCI0_5MHz_FDD_t *)UE_list->UE_template[CC_id][cba_group].ULSCH_DCI[0];

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
			   eNB_mac_inst[module_idP][CC_id].cba_rnti[cba_group],
			   sizeof(DCI0_5MHz_FDD_t),
			   aggregation,
			   sizeof_DCI0_5MHz_FDD_t,
			   format0,
			   0);
	  }
	  nCCE[CC_id] = nCCE[CC_id] + (1<<aggregation) * num_cba_resources[cba_group];
	  nCCE_available[CC_id] = mac_xface->get_nCCE_max(module_idP,CC_id) - nCCE[CC_id];
	  //      break;// for the moment only schedule one
	}
      }
    }
  }
}
#endif
