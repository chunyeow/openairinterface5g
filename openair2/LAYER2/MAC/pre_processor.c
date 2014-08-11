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
/*! \file pre_processor.c
 * \brief eNB scheduler preprocessing fuction prior to scheduling 
 * \author Navid Nikaein and Ankit Bhamri
 * \date 2013 - 2014
 * \email navid.nikaein@eurecom.fr
 * \version 1.0
 * @ingroup _mac

 */


#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/proto.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"
#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"



#define DEBUG_eNB_SCHEDULER 1
#define DEBUG_HEADER_PARSING 1
//#define DEBUG_PACKET_TRACE 1

//#define ICIC 0

/*
  #ifndef USER_MODE
  #define msg debug_msg
  #endif
*/

extern inline unsigned int taus(void);



// This function stores the downlink buffer for all the logical channels 
void store_dlsch_buffer (module_id_t Mod_id,
                         frame_t     frameP,
                         sub_frame_t subframeP){

  int                   UE_id,i;
  rnti_t                rnti;
  mac_rlc_status_resp_t rlc_status;
  UE_list_t             *UE_list = &eNB_mac_inst[Mod_id].UE_list;
  UE_TEMPLATE           *UE_template;

  for (UE_id=UE_list->head;UE_id>=0;UE_id=UE_list->next[UE_id]){

    UE_template = &UE_list->UE_template[UE_PCCID(Mod_id,UE_id)][UE_id];
    
  // clear logical channel interface variables
    UE_template->dl_buffer_total = 0;
    UE_template->dl_pdus_total = 0;
    for(i=0;i< MAX_NUM_LCID; i++) {
      UE_template->dl_buffer_info[i]=0;
      UE_template->dl_pdus_in_buffer[i]=0;
      UE_template->dl_buffer_head_sdu_creation_time[i]=0;
      UE_template->dl_buffer_head_sdu_remaining_size_to_send[i]=0;
    }
 
    rnti = UE_RNTI(Mod_id,UE_id);
      
    for(i=0;i< MAX_NUM_LCID; i++){ // loop over all the logical channels
      
      rlc_status = mac_rlc_status_ind(Mod_id,UE_id, frameP,ENB_FLAG_YES,MBMS_FLAG_NO,i,0 );
      UE_template->dl_buffer_info[i] = rlc_status.bytes_in_buffer; //storing the dlsch buffer for each logical channel
      UE_template->dl_pdus_in_buffer[i] = rlc_status.pdus_in_buffer;
      UE_template->dl_buffer_head_sdu_creation_time[i] = rlc_status.head_sdu_creation_time ;
      UE_template->dl_buffer_head_sdu_remaining_size_to_send[i] = rlc_status.head_sdu_remaining_size_to_send;
      UE_template->dl_buffer_head_sdu_is_segmented[i] = rlc_status.head_sdu_is_segmented;
      UE_template->dl_buffer_total = UE_template->dl_buffer_total + UE_template->dl_buffer_info[i];//storing the total dlsch buffer
      UE_template->dl_pdus_total += UE_template->dl_pdus_in_buffer[i];
      
#ifdef DEBUG_eNB_SCHEDULER
      /* note for dl_buffer_head_sdu_remaining_size_to_send[i] :
       * 0 if head SDU has not been segmented (yet), else remaining size not already segmented and sent
       */
      if (UE_template->dl_buffer_info[i]>0)
	LOG_D(MAC,"[eNB %d] Frame %d Subframe %d : RLC status for UE %d in LCID%d: total of %d pdus and size %d, head sdu queuing time %d, remaining size %d, is segmeneted %d \n",
	      Mod_id, frameP, subframeP, UE_id,
	      i, UE_template->dl_pdus_in_buffer[i],UE_template->dl_buffer_info[i],
	      UE_template->dl_buffer_head_sdu_creation_time[i],
	      UE_template->dl_buffer_head_sdu_remaining_size_to_send[i],
	      UE_template->dl_buffer_head_sdu_is_segmented[i]
	      );
#endif
      
    }
#ifdef DEBUG_eNB_SCHEDULER
    if ( UE_template->dl_buffer_total>0)
      LOG_D(MAC,"[eNB %d] Frame %d Subframe %d : RLC status for UE %d : total DL buffer size %d and total number of pdu %d \n",
	    Mod_id, frameP, subframeP, UE_id,
	    UE_template->dl_buffer_total,
	    UE_template->dl_pdus_total
	    );
#endif   
  }
}


// This function returns the estimated number of RBs required by each UE for downlink scheduling
void assign_rbs_required (module_id_t Mod_id,
			  frame_t     frameP,
			  sub_frame_t subframe,
			  uint16_t    nb_rbs_required[MAX_NUM_CCs][NUMBER_OF_UE_MAX],
			  int         min_rb_unit[MAX_NUM_CCs]){


  rnti_t           rnti;
  uint16_t         TBS = 0;
  LTE_eNB_UE_stats *eNB_UE_stats[MAX_NUM_CCs];
  int              UE_id,n,i,j,CC_id,pCCid,tmp;
  UE_list_t        *UE_list = &eNB_mac_inst[Mod_id].UE_list;


  // clear rb allocations across all CC_ids

  for (UE_id=UE_list->head;UE_id>=0;UE_id=UE_list->next[UE_id]){

    pCCid = UE_PCCID(Mod_id,UE_id);

    //update CQI information across component carriers
    for (n=0;n<UE_list->numactiveCCs[UE_id];n++) {

      CC_id = UE_list->ordered_CCids[n][UE_id];

      eNB_UE_stats[CC_id] = mac_xface->get_eNB_UE_stats(Mod_id,CC_id,rnti);	

      switch(eNB_UE_stats[CC_id]->DL_cqi[0]) {
      case 0:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 0;
	break;
      case 1:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 0;
	break;
      case 2:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 0;
	break;
      case 3:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 2;
	break;
      case 4:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 4;
	break;
      case 5:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 6;
	break;
      case 6:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 8;
	break;
      case 7:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 11;
	break;
      case 8:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 13;
	break;
      case 9:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 16;
	break;
      case 10:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 18;
	break;
      case 11:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 20;
	break;
      case 12:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 22;
	break;
      case 13:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 25;
	break;
      case 14:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 27;
	break;
      case 15:
	eNB_UE_stats[CC_id]->dlsch_mcs1 = 28;
	break;
      default:
	LOG_E(MAC,"preprocessor.c assign_rbs_required(): Invalid CQI %d, should not happen",eNB_UE_stats[CC_id]->DL_cqi[0]);
	exit(-1);
      }
    }

    // provide the list of CCs sorted according to MCS
    for (i=0;i<UE_list->numactiveCCs[UE_id];i++) {
      for (j=i+1;j<UE_list->numactiveCCs[UE_id];j++) {
	if (eNB_UE_stats[UE_list->ordered_CCids[i][UE_id]]->dlsch_mcs1 > 
	    eNB_UE_stats[UE_list->ordered_CCids[j][UE_id]]->dlsch_mcs1) {
	  tmp = UE_list->ordered_CCids[i][UE_id];
	  UE_list->ordered_CCids[i][UE_id] = UE_list->ordered_CCids[j][UE_id];
	  UE_list->ordered_CCids[j][UE_id] = tmp;
	}
      }
    }
  
  

    if ((mac_get_rrc_status(Mod_id,1,UE_id) < RRC_RECONFIGURED)){  // If we still don't have a default radio bearer
      nb_rbs_required[pCCid][UE_id] = PHY_vars_eNB_g[Mod_id][pCCid]->lte_frame_parms.N_RB_DL;
      continue;
    }

    if (UE_list->UE_template[UE_id]->dl_buffer_total> 0) {
      for (i=0;i<UE_list->numactiveCCs[UE_id];i++) {
	CC_id = UE_list->ordered_CCids[i][UE_id];

	if (eNB_UE_stats[CC_id]->dlsch_mcs1==0) nb_rbs_required[CC_id][UE_id] = 4;  // don't let the TBS get too small
	else nb_rbs_required[CC_id][UE_id] = min_rb_unit[CC_id];
	
	TBS = mac_xface->get_TBS_DL(eNB_UE_stats[CC_id]->dlsch_mcs1,nb_rbs_required[CC_id][UE_id]);
	
	while (TBS < UE_list->UE_template[UE_id]->dl_buffer_total)  {
	  nb_rbs_required[CC_id][UE_id] += min_rb_unit[CC_id];
	  if (nb_rbs_required[CC_id][UE_id]>PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL) {
	    TBS = mac_xface->get_TBS_DL(eNB_UE_stats[CC_id]->dlsch_mcs1,PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL);
	    nb_rbs_required[CC_id][UE_id] = PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL;// calculating required number of RBs for each UE
	    break;
	  }
	  TBS = mac_xface->get_TBS_DL(eNB_UE_stats[CC_id]->dlsch_mcs1,nb_rbs_required[CC_id][UE_id]);
	}
      }
    }
  }
}



// This function scans all CC_ids for a particular UE to find the maximum round index of its HARQ processes

int maxround(module_id_t Mod_id,uint16_t rnti,sub_frame_t subframe) {

  uint8_t round,round_max=0,harq_pid;
  int CC_id;

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++){
    mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframe,&harq_pid,&round,0);
    if (round > round_max)
      round_max = round;
  }

}

// This function scans all CC_ids for a particular UE to find the maximum DL CQI

int maxcqi(module_id_t Mod_id,uint16_t rnti) {

  LTE_eNB_UE_stats *eNB_UE_stats = NULL;
  int CC_id;
  int CQI = 0;

  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++){
    eNB_UE_stats = mac_xface->get_eNB_UE_stats(Mod_id,CC_id,rnti);
    if (eNB_UE_stats->DL_cqi[0] > CQI)
      CQI = eNB_UE_stats->DL_cqi[0];
  }

  return(CQI);
}



// This fuction sorts the UE in order their dlsch buffer and CQI
void sort_UEs (module_id_t Mod_id,
	       sub_frame_t subframe) {


  int               UE_id1,UE_id2;
  int               pCC_id1,pCC_id2;
  int               cqi1,cqi2,round1,round2;
  int               i=0,ii=0,j=0;
  rnti_t            rnti1,rnti2;

  UE_list_t *UE_list = &eNB_mac_inst[Mod_id].UE_list;

  for (i=UE_list->head;i>=0;i=UE_list->next[i]) {

    rnti1 = UE_RNTI(Mod_id,i);
    if(rnti1 == 0)
      continue;

    UE_id1  = i;
    pCC_id1 = UE_PCCID(Mod_id,UE_id1);
    cqi1    = maxcqi(Mod_id,rnti1); //
    round1  = maxround(Mod_id,rnti1,subframe);  


    for(ii=UE_list->next[i];ii>=0;ii=UE_list->next[ii]){

      UE_id2 = ii;
      rnti2 = UE_RNTI(Mod_id,UE_id2);
      if(rnti2 == 0)
	continue;

      cqi2    = maxcqi(Mod_id,rnti2);
      round2  = maxround(Mod_id,rnti2,subframe);  //mac_xface->get_ue_active_harq_pid(Mod_id,rnti2,subframe,&harq_pid2,&round2,0);
      pCC_id2 = UE_PCCID(Mod_id,UE_id2);

      if(round2 > round1){  // Check first if one of the UEs has an active HARQ process which needs service and swap order
	swap_UEs(UE_list,UE_id1,UE_id2);
      }
      else if (round2 == round1){
	// RK->NN : I guess this is for fairness in the scheduling. This doesn't make sense unless all UEs have the same configuration of logical channels.  This should be done on the sum of all information that has to be sent.  And still it wouldn't ensure fairness.  It should be based on throughput seen by each UE or maybe using the head_sdu_creation_time, i.e. swap UEs if one is waiting longer for service.
	//	for(j=0;j<MAX_NUM_LCID;j++){
	//	  if (eNB_mac_inst[Mod_id][pCC_id1].UE_template[UE_id1].dl_buffer_info[j] < 
	//      eNB_mac_inst[Mod_id][pCC_id2].UE_template[UE_id2].dl_buffer_info[j]){
	  if (UE_list->UE_template[pCC_id1][UE_id1].dl_buffer_total < 
	      UE_list->UE_template[pCC_id2][UE_id2].dl_buffer_total){
	    swap_UEs(UE_list,UE_id1,UE_id2);
	    //	    break;
	  }
	  else if (cqi1 < cqi2){
	    swap_UEs(UE_list,UE_id1,UE_id2);
	  }

	    
	    //	    if((j == MAX_NUM_LCID-1))
	    //	    {
	
	      /* The goal is to sort by priority.
	       * We use the priority of DTCH logical
	       * channel.
	       */
	      /*if(eNB_mac_inst[Mod_id][CC_id].UE_sched_ctrl[UE_id1].priority[3]<eNB_mac_inst[Mod_id][CC_id].UE_sched_ctrl[UE_id2].priority[3])
		{
		UE_id_sorted[i] = UE_id2;
		UE_id_sorted[ii] = UE_id1;
		} //if the priority is the same then sort by CQI
		else if(eNB_mac_inst[Mod_id][CC_id].UE_sched_ctrl[UE_id1].priority[3]==eNB_mac_inst[Mod_id][CC_id].UE_sched_ctrl[UE_id2].priority[3]){*/                     
	    

	      //}
	    //	    }
      }
    }
  }
}




// This function assigns pre-available RBS to each UE in specified sub-bands before scheduling is done
void dlsch_scheduler_pre_processor (module_id_t   Mod_id,
				    frame_t       frameP,
				    sub_frame_t   subframeP,
				    uint8_t       dl_pow_off[MAX_NUM_CCs][NUMBER_OF_UE_MAX],
				    uint16_t      pre_nb_available_rbs[MAX_NUM_CCs][NUMBER_OF_UE_MAX],
				    int           N_RBGS[MAX_NUM_CCs],
				    unsigned char rballoc_sub_UE[MAX_NUM_CCs][NUMBER_OF_UE_MAX][N_RBGS_MAX],
				    int *mbsfn_flag){

  unsigned char rballoc_sub[MAX_NUM_CCs][13],harq_pid=0,harq_pid1=0,harq_pid2=0,round=0,round1=0,round2=0,total_ue_count[MAX_NUM_CCs];
  unsigned char MIMO_mode_indicator[MAX_NUM_CCs][13];
  int                     UE_id, UE_id2, i;
  uint16_t                ii,j;
  uint16_t                nb_rbs_required[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  uint16_t                nb_rbs_required_remaining[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  uint16_t                nb_rbs_required_remaining_1[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  uint16_t                i1,i2,i3,r1=0;
  uint16_t                average_rbs_per_user[MAX_NUM_CCs];
  rnti_t             rnti,rnti1,rnti2;
  LTE_eNB_UE_stats  *eNB_UE_stats1 = NULL;
  LTE_eNB_UE_stats  *eNB_UE_stats2 = NULL;
  int                min_rb_unit[MAX_NUM_CCs];

  int CC_id;
  UE_list_t *UE_list = &eNB_mac_inst[Mod_id].UE_list;


  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {

    if (mbsfn_flag[CC_id]>0)  // If this CC is allocated for MBSFN skip it here
      continue;

    switch (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL) {
    case 6:
      min_rb_unit[CC_id]=1;
      break;
    case 25:
      min_rb_unit[CC_id]=2;
      break;
    case 50:
      min_rb_unit[CC_id]=3;
      break;
    case 100:
      min_rb_unit[CC_id]=4;
      break;
    default:
      min_rb_unit[CC_id]=2;
      break;
    }

    // Initialize scheduling information for all active UEs
    for (i=UE_list->head;i>=0;i=UE_list->next[i]) {
      UE_id = i;

      nb_rbs_required[CC_id][UE_id] = 0;
      dl_pow_off[CC_id][UE_id]  =2;
      pre_nb_available_rbs[CC_id][UE_id] = 0;
      nb_rbs_required_remaining[CC_id][UE_id] = 0;
      for(j=0;j<N_RBGS[CC_id];j++)
	{
	  MIMO_mode_indicator[CC_id][j] = 2;
	  rballoc_sub[CC_id][j] = 0;
	  rballoc_sub_UE[CC_id][UE_id][j] = 0;
	}
    }
  }

  // Store the DLSCH buffer for each logical channel
  store_dlsch_buffer (Mod_id,frameP,subframeP);

  // Calculate the number of RBs required by each UE on the basis of logical channel's buffer
  assign_rbs_required (Mod_id,frameP,subframeP,nb_rbs_required,min_rb_unit);

  // Sorts the user on the basis of dlsch logical channel buffer and CQI
  sort_UEs (Mod_id,subframeP);


  // loop over all active UEs
  for (i=UE_list->head;i>=0;i=UE_list->next[i]) {

    UE_id = i;


    rnti = UE_RNTI(Mod_id,UE_id);
    if(rnti == 0)
      continue;

    for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
      CC_id = UE_list->ordered_CCids[ii][UE_id];

      total_ue_count[CC_id]=0;
      average_rbs_per_user[CC_id]=0;


      mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframeP,&harq_pid,&round,0);
      if(round>0)
	nb_rbs_required[CC_id][UE_id] = UE_list->UE_template[CC_id][UE_id].nb_rb[harq_pid];
      //nb_rbs_required_remaining[UE_id] = nb_rbs_required[UE_id];
      if (nb_rbs_required[CC_id][UE_id] > 0) {
	total_ue_count[CC_id] = total_ue_count[CC_id] + 1;
      }
      
  // hypotetical assignement
  /*
   * If schedule is enabled and if the priority of the UEs is modified
   * The average rbs per logical channel per user will depend on the level of  
   * priority. Concerning the hypothetical assignement, we should assign more 
   * rbs to prioritized users. Maybe, we can do a mapping between the
   * average rbs per user and the level of priority or multiply the average rbs 
   * per user by a coefficient which represents the degree of priority.
   */


      if((total_ue_count[CC_id] > 0) && ( min_rb_unit[CC_id] * total_ue_count[CC_id] <= PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL ) )
	average_rbs_per_user[CC_id] = (uint16_t) ceil(PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL/total_ue_count[CC_id]);
      else 
	average_rbs_per_user[CC_id] = min_rb_unit[CC_id];
    }
  }

  for(i=UE_list->head;i>=0;i=UE_list->next[i]){
    for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
      CC_id = UE_list->ordered_CCids[ii][UE_id];
      // control channel
      if (mac_get_rrc_status(Mod_id,1,i) < RRC_RECONFIGURED)
	nb_rbs_required_remaining_1[CC_id][i] = nb_rbs_required[CC_id][i];
      else
	nb_rbs_required_remaining_1[CC_id][i] = cmin(average_rbs_per_user[CC_id],nb_rbs_required[CC_id][i]);
    }
  }


  //Allocation to UEs is done in 2 rounds,
  // 1st round: average number of RBs allocated to each UE
  // 2nd round: remaining RBs are allocated to high priority UEs
  for(r1=0;r1<2;r1++){ 

    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];
 
	if(r1 == 0)
	  nb_rbs_required_remaining[CC_id][i] = nb_rbs_required_remaining_1[CC_id][i];
	else  // rb required based only on the buffer - rb allloctaed in the 1st round + extra reaming rb form the 1st round
	  nb_rbs_required_remaining[CC_id][i] = nb_rbs_required[CC_id][i]-nb_rbs_required_remaining_1[CC_id][i]+nb_rbs_required_remaining[CC_id][i];
      }
    }
    // retransmission in control channels
    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      UE_id = i;
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];
	

	rnti = UE_RNTI(Mod_id,UE_id);
	if(rnti == 0)
	  continue;
	mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframeP,&harq_pid,&round,0);
	
	if ((mac_get_rrc_status(Mod_id,1,UE_id) < RRC_RECONFIGURED) && (round >0)) {
	  for(j=0;j<N_RBGS[CC_id];j++){
	    
	    if((rballoc_sub[CC_id][j] == 0) && (rballoc_sub_UE[CC_id][UE_id][j] == 0) && (nb_rbs_required_remaining[UE_id]>0)){
	      
	      rballoc_sub[CC_id][j] = 1;
	      rballoc_sub_UE[CC_id][UE_id][j] = 1;
	      
	      MIMO_mode_indicator[CC_id][j] = 1;
	      
	      if(mac_xface->get_transmission_mode(Mod_id,CC_id,rnti)==5)
		dl_pow_off[CC_id][UE_id] = 1;
	      // if the total rb is odd
	      if ((j == N_RBGS[CC_id]-1) &&
		  ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25)||
		   (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))) {
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id]+1;
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id] - 1;
	      }
	      else {
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id];
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id];
	      }
	    }
	  }
	}
      }
    }


    // retransmission in data channels

    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      UE_id = i;
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];

	rnti = UE_RNTI(Mod_id,UE_id);
	if(rnti == 0)
	  continue;
	mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframeP,&harq_pid,&round,0);
	
	if ((mac_get_rrc_status(Mod_id,1,UE_id) >= RRC_RECONFIGURED) && (round > 0)) {
	  
	  for(j=0;j<N_RBGS[CC_id];j++){
	    
	    if((rballoc_sub[CC_id][j] == 0) && (rballoc_sub_UE[CC_id][UE_id][j] == 0) && (nb_rbs_required_remaining[UE_id]>0)){
	      
	      rballoc_sub[CC_id][j] = 1;
	      rballoc_sub_UE[CC_id][UE_id][j] = 1;
	      
	      MIMO_mode_indicator[CC_id][j] = 1;
	      
	      if(mac_xface->get_transmission_mode(Mod_id,CC_id,rnti)==5)
		dl_pow_off[CC_id][UE_id] = 1;
	      
	      if((j == N_RBGS[CC_id]-1) &&
		 ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25)||
		  (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))){
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id] + 1;
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id] - 1;
	      }
	      else {
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id];
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id];
	      }
	    }
	  }
	}
      }
    }
      
    // control channel in the 1st transmission

    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      UE_id = i;
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];

	
	rnti = UE_RNTI(Mod_id,UE_id);
	if(rnti == 0)
	  continue;
	mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframeP,&harq_pid,&round,0);
	
	if ((mac_get_rrc_status(Mod_id,1,UE_id) < RRC_RECONFIGURED) && (round == 0)) {
	  
	  
	  
	  
	  for(j=0;j<N_RBGS[CC_id];j++){
	    
	    if((rballoc_sub[CC_id][j] == 0) && (rballoc_sub_UE[CC_id][UE_id][j] == 0) && (nb_rbs_required_remaining[CC_id][UE_id]>0)){
	      
	      rballoc_sub[CC_id][j] = 1;
	      rballoc_sub_UE[CC_id][UE_id][j] = 1;
	      
	      MIMO_mode_indicator[CC_id][j] = 1;
	      
	      if(mac_xface->get_transmission_mode(Mod_id,CC_id,rnti)==5)
		dl_pow_off[CC_id][UE_id] = 1;
	      
	      if((j == N_RBGS[CC_id]-1) &&
		 ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25)||
		  (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))){
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id] + 1;
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id] - 1;
	      }
	      else {
		nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id];
		pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id];
	      }
	    }
	  }
	}
      }
    }


    // data chanel TM5
    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      UE_id = i;
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];


	rnti1 = UE_RNTI(Mod_id,UE_id);
	if(rnti1 == 0)
	  continue;
	
	eNB_UE_stats1 = mac_xface->get_eNB_UE_stats(Mod_id,CC_id,rnti1);
	
	mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti1,subframeP,&harq_pid1,&round1,0);
	
	if ((mac_get_rrc_status(Mod_id,1,UE_id) >= RRC_RECONFIGURED) && (round1==0) && (mac_xface->get_transmission_mode(Mod_id,CC_id,rnti1)==5) && (dl_pow_off[CC_id][UE_id] != 1)) {
	  
	  
	  for(j=0;j<N_RBGS[CC_id];j+=2){
	    
	    if((((j == (N_RBGS[CC_id]-1))&& (rballoc_sub[CC_id][j] == 0) && (rballoc_sub_UE[CC_id][UE_id][j] == 0)) || ((j < (N_RBGS[CC_id]-1)) && (rballoc_sub[CC_id][j+1] == 0) && (rballoc_sub_UE[CC_id][UE_id][j+1] == 0))) && (nb_rbs_required_remaining[CC_id][UE_id]>0)){
	      
	      for (ii = UE_list->next[i+1];ii >=0;ii=UE_list->next[ii]) {
		
		UE_id2 = ii;
		rnti2 = UE_RNTI(Mod_id,UE_id2);
		if(rnti2 == 0)
		  continue;
		
		eNB_UE_stats2 = mac_xface->get_eNB_UE_stats(Mod_id,CC_id,rnti2);
		mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti2,subframeP,&harq_pid2,&round2,0);
		
		if ((mac_get_rrc_status(Mod_id,1,UE_id2) >= RRC_RECONFIGURED) && (round2==0) && (mac_xface->get_transmission_mode(Mod_id,CC_id,rnti2)==5) && (dl_pow_off[CC_id][UE_id2] != 1)) {
		  
		  if((((j == (N_RBGS[CC_id]-1)) && (rballoc_sub_UE[CC_id][UE_id2][j] == 0)) || ((j < (N_RBGS[CC_id]-1)) && (rballoc_sub_UE[CC_id][UE_id2][j+1] == 0))) && (nb_rbs_required_remaining[CC_id][UE_id2]>0)){
		    
		    if((((eNB_UE_stats2->DL_pmi_single^eNB_UE_stats1->DL_pmi_single)<<(14-j))&0xc000)== 0x4000){ //MU-MIMO only for 25 RBs configuration
		      
		      rballoc_sub[CC_id][j] = 1;
		      rballoc_sub_UE[CC_id][UE_id][j] = 1;
		      rballoc_sub_UE[CC_id][UE_id2][j] = 1;
		      MIMO_mode_indicator[CC_id][j] = 0;
		      
		      if (j< N_RBGS[CC_id]-1) {
			rballoc_sub[CC_id][j+1] = 1;
			rballoc_sub_UE[CC_id][UE_id][j+1] = 1;
			rballoc_sub_UE[CC_id][UE_id2][j+1] = 1;
			MIMO_mode_indicator[CC_id][j+1] = 0;
		      }
		      
		      dl_pow_off[CC_id][UE_id] = 0;
		      dl_pow_off[CC_id][UE_id2] = 0;
		      
		      
		      
		      
		      if ((j == N_RBGS[CC_id]-1) &&
			  ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25) ||
			   (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))){
			nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id]+1;
			pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id]-1;
			nb_rbs_required_remaining[CC_id][UE_id2] = nb_rbs_required_remaining[CC_id][UE_id2] - min_rb_unit[CC_id]+1;
			pre_nb_available_rbs[CC_id][UE_id2] = pre_nb_available_rbs[CC_id][UE_id2] + min_rb_unit[CC_id]-1;
		      }
		      else {
			nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - 4;
			pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + 4;
			nb_rbs_required_remaining[CC_id][UE_id2] = nb_rbs_required_remaining[CC_id][UE_id2] - 4;
			pre_nb_available_rbs[CC_id][UE_id2] = pre_nb_available_rbs[CC_id][UE_id2] + 4;
		      }
		      break;
		    }
		  }
		}
	      }
	      
	    }
	  }
	}
      }
    }
    // data channel for all TM

    for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
      for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
	CC_id = UE_list->ordered_CCids[ii][UE_id];
	UE_id = i;

	rnti = UE_RNTI(Mod_id,UE_id);
	if (rnti == 0)
	  continue;
	
	mac_xface->get_ue_active_harq_pid(Mod_id,CC_id,rnti,subframeP,&harq_pid,&round,0);
	
	if ((mac_get_rrc_status(Mod_id,1,UE_id) >= RRC_RECONFIGURED) && (round==0)) {
	  
	  
	  for(j=0;j<N_RBGS[CC_id];j++){
	    
	    if((rballoc_sub[CC_id][j] == 0) && (rballoc_sub_UE[CC_id][UE_id][j] == 0) && (nb_rbs_required_remaining[CC_id][UE_id]>0)){
	      
	      
	      switch (mac_xface->get_transmission_mode(Mod_id,CC_id,rnti)) {
	      case 1:
	      case 2:
	      case 4:
	      case 6:
		rballoc_sub[CC_id][j] = 1;
		rballoc_sub_UE[CC_id][UE_id][j] = 1;
		
		MIMO_mode_indicator[CC_id][j] = 1;
		
		if((j == N_RBGS[CC_id]-1) &&
		   ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25)||
		    (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))){
		  nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id]+1;
		  pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] +min_rb_unit[CC_id]-1;
		}
		else {
		  nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id];
		  pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id];
		}
		
		break;
	      case 5:
		if (dl_pow_off[CC_id][UE_id] != 0){
		  
		  dl_pow_off[CC_id][UE_id] = 1;
		  
		  rballoc_sub[CC_id][j] = 1;
		  rballoc_sub_UE[CC_id][UE_id][j] = 1;
		  
		  MIMO_mode_indicator[CC_id][j] = 1;
		  
		  if((j == N_RBGS[CC_id]-1) &&
		     ((PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 25)||
		      (PHY_vars_eNB_g[Mod_id][CC_id]->lte_frame_parms.N_RB_DL == 50))){
		    nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id]+1;
		    pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id]-1;
		  }
		  else {
		    nb_rbs_required_remaining[CC_id][UE_id] = nb_rbs_required_remaining[CC_id][UE_id] - min_rb_unit[CC_id];
		    pre_nb_available_rbs[CC_id][UE_id] = pre_nb_available_rbs[CC_id][UE_id] + min_rb_unit[CC_id];
		  }
		}
		break;
	      default:
		break;
	      }
	    }
	  }
	}
      }
    }
  }

  // This has to be revisited!!!!
  for (CC_id=0;CC_id<MAX_NUM_CCs;CC_id++) {
    i1=0;
    i2=0;
    i3=0;
    for (j=0;j<N_RBGS[CC_id];j++){
      if(MIMO_mode_indicator[CC_id][j] == 2)
	i1 = i1+1;
      else if(MIMO_mode_indicator[CC_id][j] == 1)
	i2 = i2+1;
      else if(MIMO_mode_indicator[CC_id][j] == 0)
	i3 = i3+1;
    }
    
    
    if((i1 < N_RBGS[CC_id]) && (i2>0) && (i3==0))
      PHY_vars_eNB_g[Mod_id][CC_id]->check_for_SUMIMO_transmissions = PHY_vars_eNB_g[Mod_id][CC_id]->check_for_SUMIMO_transmissions + 1;
    
    if(i3 == N_RBGS[CC_id] && i1==0 && i2==0)
      PHY_vars_eNB_g[Mod_id][CC_id]->FULL_MUMIMO_transmissions = PHY_vars_eNB_g[Mod_id][CC_id]->FULL_MUMIMO_transmissions + 1;
    
    if((i1 < N_RBGS[CC_id]) && (i3 > 0))
      PHY_vars_eNB_g[Mod_id][CC_id]->check_for_MUMIMO_transmissions = PHY_vars_eNB_g[Mod_id][CC_id]->check_for_MUMIMO_transmissions + 1;
    
    PHY_vars_eNB_g[Mod_id][CC_id]->check_for_total_transmissions = PHY_vars_eNB_g[Mod_id][CC_id]->check_for_total_transmissions + 1;
    
    
  }

  for(i=UE_list->head; i>=0;i=UE_list->next[i]) {
    for (ii=0;ii<UE_num_active_CC(UE_list,UE_id);ii++) {
      CC_id = UE_list->ordered_CCids[ii][UE_id];
      UE_id = i;
      //PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[UE_id].dl_pow_off = dl_pow_off[UE_id];
      LOG_D(MAC,"******************Scheduling Information for UE%d ************************\n",UE_id);
      LOG_D(MAC,"dl power offset UE%d = %d \n",UE_id,dl_pow_off[CC_id][UE_id]);
      LOG_D(MAC,"***********RB Alloc for every subband for UE%d ***********\n",UE_id);
      for(j=0;j<N_RBGS[CC_id];j++){
	//PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[UE_id].rballoc_sub[i] = rballoc_sub_UE[CC_id][UE_id][i];
	LOG_D(MAC,"RB Alloc for UE%d and Subband%d = %d\n",UE_id,j,rballoc_sub_UE[CC_id][UE_id][j]);
      }
      //PHY_vars_eNB_g[Mod_id]->mu_mimo_mode[UE_id].pre_nb_available_rbs = pre_nb_available_rbs[CC_id][UE_id];
      LOG_D(MAC,"Total RBs allocated for UE%d = %d\n",UE_id,pre_nb_available_rbs[CC_id][UE_id]);
    }
  }
}

