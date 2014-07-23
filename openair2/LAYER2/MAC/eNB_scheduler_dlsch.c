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

/*! \file eNB_scheduler_dlsch.c
 * \brief procedures related to eNB for the DLSCH transport channel
 * \author  Navid Nikaein and Raymond Knopp
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

extern inline unsigned int taus(void);


// function for determining which active ue should be granted resources in downlink based on CQI, SI, and BSR
uint8_t find_dlgranted_UEs(module_id_t module_idP){

  // all active users should be granted
  return(find_active_UEs(module_idP));
}

void add_ue_dlsch_info(module_id_t module_idP, module_id_t ue_mod_idP, sub_frame_t subframeP, UE_DLSCH_STATUS status){

  eNB_dlsch_info[module_idP][ue_mod_idP].rnti             = find_UE_RNTI(module_idP,ue_mod_idP);
  //  eNB_dlsch_info[module_idP][ue_mod_idP].weight           = weight;
  eNB_dlsch_info[module_idP][ue_mod_idP].subframe         = subframeP;
  eNB_dlsch_info[module_idP][ue_mod_idP].status           = status;

  eNB_dlsch_info[module_idP][ue_mod_idP].serving_num++;

}

module_id_t schedule_next_dlue(module_id_t module_idP, sub_frame_t subframeP){

  module_id_t next_ue;

  for (next_ue=0; next_ue < NUMBER_OF_UE_MAX; next_ue++ ){
      if  (eNB_dlsch_info[module_idP][next_ue].status == S_DL_WAITING)
        return next_ue;
  }
  for (next_ue=0; next_ue <NUMBER_OF_UE_MAX; next_ue++ )
    if  (eNB_dlsch_info[module_idP][next_ue].status == S_DL_BUFFERED) {
        eNB_dlsch_info[module_idP][next_ue].status = S_DL_WAITING;
    }
  // next_ue = -1;
  return (module_id_t)(-1);//next_ue;

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
  uint8_t first_element=0,last_size=0,i;
  uint8_t mac_header_control_elements[16],*ce_ptr;

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

  //msg("After subheaders %d\n",(uint8_t*)mac_header_ptr - mac_header);

  if ((ce_ptr-mac_header_control_elements) > 0) {
      // printf("Copying %d bytes for control elements\n",ce_ptr-mac_header_control_elements);
      memcpy((void*)mac_header_ptr,mac_header_control_elements,ce_ptr-mac_header_control_elements);
      mac_header_ptr+=(unsigned char)(ce_ptr-mac_header_control_elements);
  }
  //msg("After CEs %d\n",(uint8_t*)mac_header_ptr - mac_header);

  return((unsigned char*)mac_header_ptr - mac_header);

}


void schedule_ue_spec(module_id_t   module_idP,
                      frame_t       frameP,
                      sub_frame_t   subframeP,
                      uint16_t           nb_rb_used0,
                      unsigned int *nCCE_used,
                      int           mbsfn_flag) {

  module_id_t           ue_mod_id          = -1;
  module_id_t           next_ue          = -1;
  unsigned char         granted_UEs      = 0;
  uint16_t                   nCCE;
  unsigned char         aggregation;
  mac_rlc_status_resp_t rlc_status;
  unsigned char         header_len_dcch=0, header_len_dcch_tmp=0,header_len_dtch=0,header_len_dtch_tmp=0, ta_len=0;
  unsigned char         sdu_lcids[11],offset,num_sdus=0;
  uint16_t                   nb_rb,nb_rb_temp,nb_available_rb,TBS,j,sdu_lengths[11],rnti,padding=0,post_padding=0;
  unsigned char         dlsch_buffer[MAX_DLSCH_PAYLOAD_BYTES];
  unsigned char         round            = 0;
  unsigned char         harq_pid         = 0;
  void                 *DLSCH_dci        = NULL;
  LTE_eNB_UE_stats     *eNB_UE_stats     = NULL;
  uint16_t                   sdu_length_total = 0;
  unsigned char         DAI;
  uint16_t                   i                = 0;
  uint8_t                    dl_pow_off[NUMBER_OF_UE_MAX];
  unsigned char         rballoc_sub_UE[NUMBER_OF_UE_MAX][N_RBGS_MAX];
  uint16_t                   pre_nb_available_rbs[NUMBER_OF_UE_MAX];
  int                   mcs;
  uint16_t                   min_rb_unit;
  short                 ta_update        = 0;

  if (mbsfn_flag>0)
    return;

  reset_meas(&eNB_mac_inst[module_idP].schedule_dlsch);

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

  //int **rballoc_sub = (int **)malloc(1792*sizeof(int *));
  granted_UEs = find_dlgranted_UEs(module_idP);


  //weight = get_ue_weight(module_idP,UE_id);
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
      PHY_vars_eNB_g[module_idP]->mu_mimo_mode[i].pre_nb_available_rbs = 0;
      for (j = 0; j < mac_xface->lte_frame_parms->N_RBGS; j++) {
          PHY_vars_eNB_g[module_idP]->mu_mimo_mode[i].rballoc_sub[j] = 0;
      }
  }


  // set current available nb_rb and nCCE to maximum
  nb_available_rb = mac_xface->lte_frame_parms->N_RB_DL - nb_rb_used0;
  nCCE = mac_xface->get_nCCE_max(module_idP) - *nCCE_used;

  // store the goloabl enb stats
  eNB_mac_inst[module_idP].eNB_stats.num_dlactive_UEs =  granted_UEs;
  eNB_mac_inst[module_idP].eNB_stats.available_prbs =  nb_available_rb;
  eNB_mac_inst[module_idP].eNB_stats.total_available_prbs +=  nb_available_rb;
  eNB_mac_inst[module_idP].eNB_stats.available_ncces = nCCE;
  eNB_mac_inst[module_idP].eNB_stats.dlsch_bytes_tx=0;
  eNB_mac_inst[module_idP].eNB_stats.dlsch_pdus_tx=0;


  /// CALLING Pre_Processor for downlink scheduling (Returns estimation of RBs required by each UE and the allocation on sub-band)
  start_meas(&eNB_mac_inst[module_idP].schedule_dlsch_preprocessor);
  dlsch_scheduler_pre_processor(module_idP,
				frameP,
				subframeP,
				dl_pow_off,
				pre_nb_available_rbs,
				mac_xface->lte_frame_parms->N_RBGS,
				rballoc_sub_UE);
  stop_meas(&eNB_mac_inst[module_idP].schedule_dlsch_preprocessor);

  for (ue_mod_id=0;ue_mod_id<granted_UEs;ue_mod_id++) {

      rnti = find_UE_RNTI(module_idP,ue_mod_id);

      eNB_mac_inst[module_idP].eNB_UE_stats[ue_mod_id].crnti= rnti;
      eNB_mac_inst[module_idP].eNB_UE_stats[ue_mod_id].rrc_status=mac_get_rrc_status(module_idP,1,ue_mod_id);
      if (rnti==0) {
          LOG_E(MAC,"Cannot find rnti for UE_id %d (granted UEs %d)\n",ue_mod_id,granted_UEs);
          mac_xface->macphy_exit("Cannot find rnti for UE_id");//continue;
      }

      eNB_UE_stats = mac_xface->get_eNB_UE_stats(module_idP,rnti);
      if (eNB_UE_stats==NULL)
        mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

      // Get candidate harq_pid from PHY
      mac_xface->get_ue_active_harq_pid(module_idP,rnti,subframeP,&harq_pid,&round,0);
      //    printf("Got harq_pid %d, round %d\n",harq_pid,round);


      nb_available_rb = pre_nb_available_rbs[ue_mod_id];

      if ((nb_available_rb == 0) || (nCCE < (1<<aggregation))) {
          LOG_D(MAC,"UE %d: nb_availiable_rb exhausted (nb_rb_used %d, nb_available_rb %d, nCCE %d, aggregation %d)\n",
              ue_mod_id, nb_rb_used0, nb_available_rb, nCCE, aggregation);
          //if(mac_xface->get_transmission_mode(module_idP,rnti)==5)
          continue; //to next user (there might be rbs availiable for other UEs in TM5
          // else
          //	break;
      }
      sdu_length_total=0;
      num_sdus=0;

      // get Round-Robin allocation
      next_ue = ue_mod_id;//schedule_next_dlue(module_idP,subframeP); // next scheduled user
      // If nobody is left, exit while loop and go to next step
      if (next_ue == 255)
        break;

      if (mac_xface->lte_frame_parms->frame_type == TDD) {
          switch (mac_xface->lte_frame_parms->tdd_config) {
          case 0:
            if ((subframeP==0)||(subframeP==1)||(subframeP==3)||(subframeP==5)||(subframeP==6)||(subframeP==8))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
          case 1:
            if ((subframeP==0)||(subframeP==4)||(subframeP==5)||(subframeP==9))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
            break;
          case 2:
            if ((subframeP==4)||(subframeP==5))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
            break;
          case 3:
            if ((subframeP==5)||(subframeP==7)||(subframeP==9))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
            break;
          case 4:
            if ((subframeP==0)||(subframeP==6))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
            break;
          case 5:
            if (subframeP==9)
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
            break;
          case 6:
            if ((subframeP==0)||(subframeP==1)||(subframeP==5)||(subframeP==6)||(subframeP==9))
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI = 0;
          default:
            break;
          }
      }

      // This is an allocated UE_id
      rnti = find_UE_RNTI(module_idP,next_ue);
      if (rnti==0)
        continue;

      eNB_UE_stats = mac_xface->get_eNB_UE_stats(module_idP,rnti);

      if (eNB_UE_stats==NULL)
        mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

      if (openair_daq_vars.target_ue_dl_mcs <= 0) {
	eNB_UE_stats->dlsch_mcs1 = cqi_to_mcs[eNB_UE_stats->DL_cqi[0]];
	LOG_T(MAC,"CQI %d\n",eNB_UE_stats->DL_cqi[0]);
      }
      else
	eNB_UE_stats->dlsch_mcs1 = openair_daq_vars.target_ue_dl_mcs;


      if(eNB_UE_stats->dlsch_mcs1>22)
        eNB_UE_stats->dlsch_mcs1=22;


      // for TM5, limit the MCS to 16QAM


      // for EXMIMO, limit the MCS to 16QAM as well
#ifdef EXMIMO
      eNB_UE_stats->dlsch_mcs1 = cmin(eNB_UE_stats->dlsch_mcs1,16);
#endif

      // Get candidate harq_pid from PHY
      mac_xface->get_ue_active_harq_pid(module_idP,rnti,subframeP,&harq_pid,&round,0);

      eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].harq_pid = harq_pid;
      eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].harq_round = round;
      // Note this code is for a specific DCI format
      DLSCH_dci = (void *)eNB_mac_inst[module_idP].UE_template[next_ue].DLSCH_DCI[harq_pid];

      for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++){ // initializing the rb allocation indicator for each UE
          eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j] = 0;
      }
      // store stats
      eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].dl_cqi= eNB_UE_stats->DL_cqi[0];

      if (round > 0) {
          if (mac_xface->lte_frame_parms->frame_type == TDD) {
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI++;
              LOG_D(MAC,"DAI update: subframeP %d: UE %d, DAI %d\n",subframeP,next_ue,eNB_mac_inst[module_idP].UE_template[next_ue].DAI);

              update_ul_dci(module_idP,rnti,eNB_mac_inst[module_idP].UE_template[next_ue].DAI);
          }

          // get freq_allocation
          nb_rb = eNB_mac_inst[module_idP].UE_template[next_ue].nb_rb[harq_pid];
          if (nb_rb <= nb_available_rb) {

              if(nb_rb == pre_nb_available_rbs[next_ue]){
                  for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++) // for indicating the rballoc for each sub-band
                    eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];}
              else
                {
                  nb_rb_temp = nb_rb;
                  j = 0;
                  while((nb_rb_temp > 0) && (j<mac_xface->lte_frame_parms->N_RBGS)){
                      if(rballoc_sub_UE[next_ue][j] == 1){
                          eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
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
              aggregation = process_ue_cqi(module_idP,next_ue);
              nCCE-=(1<<aggregation); // adjust the remaining nCCE
              *nCCE_used += (1<<aggregation);


              PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].pre_nb_available_rbs = nb_rb;
              PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].dl_pow_off = dl_pow_off[next_ue];

              for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++)
                PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].rballoc_sub[j] = eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j];

              switch (mac_xface->get_transmission_mode(module_idP,rnti)) {
              case 1:
              case 2:
              default:
                switch (mac_xface->lte_frame_parms->N_RB_DL) {
                case 6:
                  if (mac_xface->lte_frame_parms->frame_type == TDD) {
                      //	      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",module_idP,harq_pid,round,(eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1),((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs);
                  }
                  else {
                      //	      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",module_idP,harq_pid,round,((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs);

                  }
                  break;
                case 25:
                  if (mac_xface->lte_frame_parms->frame_type == TDD) {
                      //	      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",module_idP,harq_pid,round,(eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1),((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs);
                  }
                  else {
                      //	      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",module_idP,harq_pid,round,((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs);

                  }
                  break;
                case 50:
                  if (mac_xface->lte_frame_parms->frame_type == TDD) {
                      //	      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",module_idP,harq_pid,round,(eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1),((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs);
                  }
                  else {
                      //	      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",module_idP,harq_pid,round,((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs);

                  }
                  break;
                case 100:
                  if (mac_xface->lte_frame_parms->frame_type == TDD) {
                      //	      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, dai %d, mcs %d\n",module_idP,harq_pid,round,(eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1),((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs);
                  }
                  else {
                      //	      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                      LOG_D(MAC,"[eNB %d] Retransmission : harq_pid %d, round %d, mcs %d\n",module_idP,harq_pid,round,((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs);

                  }
                  break;
                }
                break;
                case 4:
                  //	  if (nb_rb>10) {
                  ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 0;
                  ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                  ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                  // }
                  //else {
                  //  ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 0;
                  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
                  // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                  // }
                  break;
                case 5:
                  // if(nb_rb>10){
                  //((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = eNB_UE_stats->DL_cqi[0]<<1;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                  if(dl_pow_off[next_ue] == 2)
                    dl_pow_off[next_ue] = 1;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = dl_pow_off[next_ue];
                  // }
                  break;
                case 6:
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                  ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;//dl_pow_off[next_ue];
                  break;
              }

              add_ue_dlsch_info(module_idP,
                  next_ue,
                  subframeP,
                  S_DL_SCHEDULED);

              //eNB_UE_stats->dlsch_trials[round]++;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_retransmission+=1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].rbs_used_retx=nb_rb;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].total_rbs_used_retx+=nb_rb;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].ncce_used_retx=nCCE;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].dlsch_mcs2=eNB_UE_stats->dlsch_mcs1;
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
#ifndef EXMIMO_IOT
          ta_len = ((eNB_UE_stats->timing_advance_update/4)!=0) ? 2 : 0;
#else
          ta_len = 0;
#endif

          header_len_dcch = 2; // 2 bytes DCCH SDU subheader


          rlc_status = mac_rlc_status_ind(
              module_idP,
              ue_mod_id,
              frameP,
              ENB_FLAG_YES,
              MBMS_FLAG_NO,
              DCCH,
              (TBS-ta_len-header_len_dcch)); // transport block set size

          sdu_lengths[0]=0;
          if (rlc_status.bytes_in_buffer > 0) {  // There is DCCH to transmit
              LOG_D(MAC,"[eNB %d] Frame %d, DL-DCCH->DLSCH, Requesting %d bytes from RLC (RRC message)\n",module_idP,frameP,TBS-header_len_dcch);
              sdu_lengths[0] += mac_rlc_data_req(
                  module_idP,
                  ue_mod_id,
                  frameP,
                  ENB_FLAG_YES,
                  MBMS_FLAG_NO,
                  DCCH,
                  (char *)&dlsch_buffer[sdu_lengths[0]]);

              LOG_D(MAC,"[eNB %d][DCCH] Got %d bytes from RLC\n",module_idP,sdu_lengths[0]);
              sdu_length_total = sdu_lengths[0];
              sdu_lcids[0] = DCCH;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_pdu_tx[DCCH]+=1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_bytes_tx[DCCH]+=sdu_lengths[0];
              num_sdus = 1;
#ifdef DEBUG_eNB_SCHEDULER
              LOG_T(MAC,"[eNB %d][DCCH] Got %d bytes :",module_idP,sdu_lengths[0]);
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
          rlc_status = mac_rlc_status_ind(
              module_idP,
              ue_mod_id,
              frameP,
              ENB_FLAG_YES,
              MBMS_FLAG_NO,
              DCCH+1,
              (TBS-ta_len-header_len_dcch-sdu_length_total)); // transport block set size less allocations for timing advance and
          // DCCH SDU

          if (rlc_status.bytes_in_buffer > 0) {
              LOG_D(MAC,"[eNB %d], Frame %d, DCCH1->DLSCH, Requesting %d bytes from RLC (RRC message)\n",
                            module_idP,frameP,TBS-header_len_dcch-sdu_length_total);
              sdu_lengths[num_sdus] += mac_rlc_data_req(
                  module_idP,
                  ue_mod_id,
                  frameP,
                  ENB_FLAG_YES,
                  MBMS_FLAG_NO,
                  DCCH+1,
                  (char *)&dlsch_buffer[sdu_lengths[0]]);

              sdu_lcids[num_sdus] = DCCH1;
              sdu_length_total += sdu_lengths[num_sdus];
              header_len_dcch += 2;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_pdu_tx[DCCH1]+=1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_bytes_tx[DCCH1]+=sdu_lengths[num_sdus];
              num_sdus++;
              LOG_D(MAC,"[eNB %d] Got %d bytes for DCCH from RLC\n",module_idP,sdu_lengths[0]);
          }
          // check for DTCH and update header information
          // here we should loop over all possible DTCH

          header_len_dtch = 3; // 3 bytes DTCH SDU subheader

          LOG_D(MAC,"[eNB %d], Frame %d, DTCH->DLSCH, Checking RLC status (rab %d, tbs %d, len %d)\n",
              module_idP,frameP,DTCH,TBS,
              TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);

          rlc_status = mac_rlc_status_ind(
              module_idP,
              ue_mod_id,
              frameP,
              ENB_FLAG_YES,
              MBMS_FLAG_NO,
              DTCH,
              TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);

          if (rlc_status.bytes_in_buffer > 0) {

              LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB], Frame %d, DTCH->DLSCH, Requesting %d bytes from RLC (hdr len dtch %d)\n",
                  module_idP,frameP,TBS-header_len_dcch-sdu_length_total-header_len_dtch,header_len_dtch);
              sdu_lengths[num_sdus] = mac_rlc_data_req(
                  module_idP,
                  ue_mod_id,
                  frameP,
                  ENB_FLAG_YES,
                  MBMS_FLAG_NO,
                  DTCH,
                  (char*)&dlsch_buffer[sdu_length_total]);

              LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] Got %d bytes for DTCH %d \n",module_idP,sdu_lengths[num_sdus],DTCH);
              sdu_lcids[num_sdus] = DTCH;
              sdu_length_total += sdu_lengths[num_sdus];
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_pdu_tx[DTCH]+=1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].num_bytes_tx[DTCH]+=sdu_lengths[num_sdus];
              if (sdu_lengths[num_sdus] < 128) {
                  header_len_dtch=2;
              }
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
              if (header_len_dtch==0) {
                header_len_dcch = (header_len_dcch >0) ? 1 : header_len_dcch;  // remove length field
              } else {
                header_len_dtch = (header_len_dtch > 0) ? 1 :header_len_dtch;     // remove length field for the last SDU
              }


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

              if(nb_rb == pre_nb_available_rbs[next_ue]) {
                for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++) {// for indicating the rballoc for each sub-band
                  eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
                }
              } else
                {
                  nb_rb_temp = nb_rb;
                  j = 0;
                  while((nb_rb_temp > 0) && (j<mac_xface->lte_frame_parms->N_RBGS)){
                      if(rballoc_sub_UE[next_ue][j] == 1){
                          eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j] = rballoc_sub_UE[next_ue][j];
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

              PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].pre_nb_available_rbs = nb_rb;
              PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].dl_pow_off = dl_pow_off[next_ue];

              for(j=0;j<mac_xface->lte_frame_parms->N_RBGS;j++)
                PHY_vars_eNB_g[module_idP]->mu_mimo_mode[next_ue].rballoc_sub[j] = eNB_mac_inst[module_idP].UE_template[next_ue].rballoc_subband[harq_pid][j];


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
                  module_idP,mcs,TBS,nb_rb);
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
#ifndef EXMIMO_IOT
              ta_update = eNB_UE_stats->timing_advance_update/4;
#else
              ta_update = 0;
#endif

              offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[module_idP].DLSCH_pdu[(module_id_t)next_ue][0].payload[0],
                  // offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0],
                  num_sdus,              //num_sdus
                  sdu_lengths,  //
                  sdu_lcids,
                  255,                                   // no drx
                  ta_update, // timing advance
                  NULL,                                  // contention res id
                  padding,
                  post_padding);
              //#ifdef DEBUG_eNB_SCHEDULER
              LOG_I(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] Generate header : sdu_length_total %d, num_sdus %d, sdu_lengths[0] %d, sdu_lcids[0] %d => payload offset %d,timing advance value : %d, next_ue %d,padding %d,post_padding %d,(mcs %d, TBS %d, nb_rb %d),header_dcch %d, header_dtch %d\n",
                  module_idP,sdu_length_total,num_sdus,sdu_lengths[0],sdu_lcids[0],offset,
                  ta_len,next_ue,padding,post_padding,mcs,TBS,nb_rb,header_len_dcch,header_len_dtch);
              //#endif

              LOG_T(MAC,"[eNB %d] First 16 bytes of DLSCH : \n");
              for (i=0;i<16;i++)
                LOG_T(MAC,"%x.",dlsch_buffer[i]);
              LOG_T(MAC,"\n");

              // cycle through SDUs and place in dlsch_buffer
              memcpy(&eNB_mac_inst[module_idP].DLSCH_pdu[(module_id_t)next_ue][0].payload[0][offset],dlsch_buffer,sdu_length_total);
              // memcpy(&eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset],dcch_buffer,sdu_lengths[0]);

              // fill remainder of DLSCH with random data
              for (j=0;j<(TBS-sdu_length_total-offset);j++)
                eNB_mac_inst[module_idP].DLSCH_pdu[(module_id_t)next_ue][0].payload[0][offset+sdu_length_total+j] = (char)(taus()&0xff);
              //eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset+sdu_lengths[0]+j] = (char)(taus()&0xff);

#if defined(USER_MODE) && defined(OAI_EMU)
              /* Tracing of PDU is done on UE side */
              if (oai_emulation.info.opt_enabled)
                trace_pdu(1, (uint8_t *)eNB_mac_inst[module_idP].DLSCH_pdu[(module_id_t)next_ue][0].payload[0],
                    TBS, module_idP, 3, find_UE_RNTI(module_idP,next_ue),
                    eNB_mac_inst[module_idP].subframe,0,0);
              LOG_D(OPT,"[eNB %d][DLSCH] Frame %d  rnti %x  with size %d\n",
                  module_idP, frameP, find_UE_RNTI(module_idP,next_ue), TBS);
#endif

              aggregation = process_ue_cqi(module_idP,next_ue);
              nCCE-=(1<<aggregation); // adjust the remaining nCCE
              *nCCE_used+=(1<<aggregation); // adjust the remaining nCCE
              eNB_mac_inst[module_idP].UE_template[next_ue].nb_rb[harq_pid] = nb_rb;

              add_ue_dlsch_info(module_idP,
                  next_ue,
                  subframeP,
                  S_DL_SCHEDULED);
              // store stats
              eNB_mac_inst[module_idP].eNB_stats.dlsch_bytes_tx+=sdu_length_total;
              eNB_mac_inst[module_idP].eNB_stats.dlsch_pdus_tx+=1;

              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].rbs_used = nb_rb;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].total_rbs_used += nb_rb;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].ncce_used = nCCE;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].dlsch_mcs2=mcs;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].TBS = TBS;

              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].overhead_bytes= TBS- sdu_length_total;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].total_sdu_bytes+= sdu_length_total;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].total_pdu_bytes+= TBS;
              eNB_mac_inst[module_idP].eNB_UE_stats[next_ue].total_num_pdus+=1;

              if (mac_xface->lte_frame_parms->frame_type == TDD) {
                  eNB_mac_inst[module_idP].UE_template[next_ue].DAI++;
                  //	printf("DAI update: subframeP %d: UE %d, DAI %d\n",subframeP,next_ue,eNB_mac_inst[module_idP].UE_template[next_ue].DAI);
#warning only for 5MHz channel 
                  update_ul_dci(module_idP,rnti,eNB_mac_inst[module_idP].UE_template[next_ue].DAI);
              }

              switch (mac_xface->get_transmission_mode(module_idP,rnti)) {
              case 1:
              case 2:
              default:
                if (mac_xface->lte_frame_parms->frame_type == TDD) {
                    switch (mac_xface->lte_frame_parms->N_RB_DL) {
                    case 6:
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      break;
                    case 25:
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      LOG_D(MAC,"Format1 DCI: harq_pid %d, ndi %d\n",harq_pid,((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi);
                      break;
                    case 50:
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv = 0;
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      break;
                    case 100:
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv = 0;
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      break;
                    default:
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                      break;
                    }
                }
                else {
                    switch (mac_xface->lte_frame_parms->N_RB_DL) {
                    case 6:
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                      break;
                    case 25:
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                      break;
                    case 50:
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv = 0;
                      break;
                    case 100:
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv = 0;
                      break;
                    default:
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
                      ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                      break;
                    }
                }
                break;
              case 4:
                //  if (nb_rb>10) {
                ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1;
                ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rv1 = round&3;
                ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;

                //}
                /* else {
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->mcs1 = eNB_UE_stats->DL_cqi[0];
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 1;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
	     ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
	     }*/
                break;
              case 5:

                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 1;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
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
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;
                ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
                break;
              }
              // Toggle NDI for next time
              LOG_D(MAC,"Frame %d, subframeP %d: Toggling Format1 NDI for UE %d (rnti %x/%d) oldNDI %d\n",frameP,subframeP,next_ue,
                  eNB_mac_inst[module_idP].UE_template[next_ue].rnti,harq_pid,eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid]);
              eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid]=1-eNB_mac_inst[module_idP].UE_template[next_ue].oldNDI[harq_pid];
          }

          else {  // There is no data from RLC or MAC header, so don't schedule

          }
      }
      if (mac_xface->lte_frame_parms->frame_type == TDD) {
          DAI = (eNB_mac_inst[module_idP].UE_template[next_ue].DAI-1)&3;
          LOG_D(MAC,"[eNB %d] Frame %d: DAI %d for UE %d\n",module_idP,frameP,DAI,next_ue);
          // Save DAI for Format 0 DCI

          switch (mac_xface->lte_frame_parms->tdd_config) {
          case 0:
            //      if ((subframeP==0)||(subframeP==1)||(subframeP==5)||(subframeP==6))
            break;
          case 1:
            switch (subframeP) {
            case 1:
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[7] = DAI;
              break;
            case 4:
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[8] = DAI;
              break;
            case 6:
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[2] = DAI;
              break;
            case 9:
              eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[3] = DAI;
              break;
            }
            case 2:
              //      if ((subframeP==3)||(subframeP==8))
              //	eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul = DAI;
              break;
            case 3:
              if ((subframeP==6)||(subframeP==8)||(subframeP==0)) {
                  LOG_D(MAC,"schedule_ue_spec: setting UL DAI to %d for subframeP %d => %d\n",DAI,subframeP, ((subframeP+8)%10)>>1);
                  eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[((subframeP+8)%10)>>1] = DAI;
              }
              break;
            case 4:
              //      if ((subframeP==8)||(subframeP==9))
              //	eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul = DAI;
              break;
            case 5:
              //      if (subframeP==8)
              //	eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul = DAI;
              break;
            case 6:
              //      if ((subframeP==1)||(subframeP==4)||(subframeP==6)||(subframeP==9))
              //	eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul = DAI;
              break;
            default:
              break;
          }
      }
      //printf("MAC nCCE : %d\n",*nCCE_used);
  }
  stop_meas(&eNB_mac_inst[module_idP].schedule_dlsch);
}

void fill_DLSCH_dci(module_id_t module_idP,frame_t frameP, sub_frame_t subframeP,uint32_t RBalloc,uint8_t RA_scheduled,int mbsfn_flag) {

  // loop over all allocated UEs and compute frequency allocations for PDSCH
  module_id_t   ue_mod_id = -1;
  uint8_t            first_rb,nb_rb=3;
  rnti_t        rnti;
  unsigned char vrb_map[100];
  uint8_t            rballoc_sub[mac_xface->lte_frame_parms->N_RBGS];
  //uint8_t number_of_subbands=13;
  uint32_t           rballoc = RBalloc;

  unsigned char round;
  unsigned char harq_pid;
  void         *DLSCH_dci=NULL;
  DCI_PDU      *DCI_pdu= &eNB_mac_inst[module_idP].DCI_pdu;
  int           i;
  void         *BCCH_alloc_pdu=(void*)&eNB_mac_inst[module_idP].BCCH_alloc_pdu;
  int           size_bits,size_bytes;

  if (mbsfn_flag>0)
    return;

  start_meas(&eNB_mac_inst[module_idP].fill_DLSCH_dci);

  // clear vrb_map
  memset(vrb_map,0,100);

  // SI DLSCH
  //  printf("BCCH check\n");
  if (eNB_mac_inst[module_idP].bcch_active == 1) {
      eNB_mac_inst[module_idP].bcch_active = 0;
      LOG_D(MAC,"[eNB %d] Frame %d subframeP %d: BCCH active\n", module_idP, frameP, subframeP);
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
            ((DCI1A_1_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->padding = 0;
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
            ((DCI1A_5MHz_TDD_1_6_t*)BCCH_alloc_pdu)->padding = 0;
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
            ((DCI1A_10MHz_TDD_1_6_t*)BCCH_alloc_pdu)->padding = 0;
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
            ((DCI1A_20MHz_TDD_1_6_t*)BCCH_alloc_pdu)->padding = 0;
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
            ((DCI1A_1_5MHz_FDD_t*)BCCH_alloc_pdu)->padding = 0;

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
            ((DCI1A_5MHz_FDD_t*)BCCH_alloc_pdu)->padding = 0;

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
            ((DCI1A_10MHz_FDD_t*)BCCH_alloc_pdu)->padding = 0;

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
            ((DCI1A_20MHz_FDD_t*)BCCH_alloc_pdu)->padding = 0;

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

          if (eNB_mac_inst[module_idP].RA_template[i].generate_rar == 1) {

              //FK: postponed to fill_rar
              //eNB_mac_inst[module_idP].RA_template[i].generate_rar = 0;

              LOG_D(MAC,"[eNB %d] Frame %d, subframeP %d: Generating RAR DCI (proc %d), RA_active %d format 1A (%d,%d))\n",
                  module_idP,frameP, subframeP,i,
                  eNB_mac_inst[module_idP].RA_template[i].RA_active,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_fmt1,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bits1);
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
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 25:
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 50:
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 100:
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  default:
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  }
              }
              else {
                  switch(mac_xface->lte_frame_parms->N_RB_DL) {
                  case 6:
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 25:
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 50:
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  case 100:
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->type=1;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->ndi=1;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rv=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->mcs=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->harq_pid=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->TPC=1;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->padding=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->vrb_type,
                        ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0])->rballoc);
                    break;
                  default:
                    break;
                  }
              }
              add_common_dci(DCI_pdu,
                  (void*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu1[0],
                  eNB_mac_inst[module_idP].RA_template[i].RA_rnti,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bytes1,
                  2,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bits1,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_fmt1,
                  1);



              LOG_D(MAC,"[eNB %d] Frame %d: Adding common dci for RA%d (RAR) RA_active %d\n",module_idP,frameP,i,
                  eNB_mac_inst[module_idP].RA_template[i].RA_active);
          }
          if (eNB_mac_inst[module_idP].RA_template[i].generate_Msg4_dci == 1) {

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
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_1_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 25:
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 50:
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_10MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 100:
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_20MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  default:
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  }
              }
              else {
                  switch (mac_xface->lte_frame_parms->N_RB_DL) {

                  case 6:
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_1_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 25:
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 50:
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_10MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  case 100:
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->padding=0;
                    ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_20MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  default:
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->type=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rv=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->harq_pid=0;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->TPC=1;
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc= mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_DL,first_rb,4);
                    rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                        ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                    break;
                  }
              }

              add_ue_spec_dci(DCI_pdu,
                  (void*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0],
                  eNB_mac_inst[module_idP].RA_template[i].rnti,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bytes2,
                  1,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bits2,
                  eNB_mac_inst[module_idP].RA_template[i].RA_dci_fmt2,
                  0);
              LOG_D(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: Adding ue specific dci (rnti %x) for Msg4\n",
                  module_idP,frameP,subframeP,eNB_mac_inst[module_idP].RA_template[i].rnti);
              eNB_mac_inst[module_idP].RA_template[i].generate_Msg4_dci=0;

          }
          else if (eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4==1) {
              // check HARQ status and retransmit if necessary
              LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: Checking if Msg4 was acknowledged: \n",
                  module_idP,frameP,subframeP);
              // Get candidate harq_pid from PHY
              mac_xface->get_ue_active_harq_pid(module_idP,eNB_mac_inst[module_idP].RA_template[i].rnti,subframeP,&harq_pid,&round,0);
              if (round>0) {
                  // we have to schedule a retransmission
                  if (mac_xface->lte_frame_parms->frame_type == TDD)
                    ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->ndi=1;
                  else
                    ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->ndi=1;
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
                      ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
                      rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                          ((DCI1A_5MHz_TDD_1_6_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                  }
                  else {
                      ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,first_rb,4);
                      rballoc |= mac_xface->get_rballoc(((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->vrb_type,
                          ((DCI1A_5MHz_FDD_t*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0])->rballoc);
                  }

                  add_ue_spec_dci(DCI_pdu,
                      (void*)&eNB_mac_inst[module_idP].RA_template[i].RA_alloc_pdu2[0],
                      eNB_mac_inst[module_idP].RA_template[i].rnti,
                      eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bytes2,
                      2,
                      eNB_mac_inst[module_idP].RA_template[i].RA_dci_size_bits2,
                      eNB_mac_inst[module_idP].RA_template[i].RA_dci_fmt2,
                      0);
                  LOG_W(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d: Msg4 not acknowledged, adding ue specific dci (rnti %x) for RA (Msg4 Retransmission)\n",
                      module_idP,frameP,subframeP,eNB_mac_inst[module_idP].RA_template[i].rnti);
              }
              else {
                  LOG_I(MAC,"[eNB %d][RAPROC] Frame %d, subframeP %d : Msg4 acknowledged\n",module_idP,frameP,subframeP);
                  eNB_mac_inst[module_idP].RA_template[i].wait_ack_Msg4=0;
                  eNB_mac_inst[module_idP].RA_template[i].RA_active=FALSE;
              }
          }
      }
  } // RA is scheduled in this subframeP

  // UE specific DCIs
  for (ue_mod_id=0;ue_mod_id<NUMBER_OF_UE_MAX;ue_mod_id++) {
      //printf("UE_id: %d => status %d\n",UE_id,eNB_dlsch_info[module_idP][UE_id].status);
      if (eNB_dlsch_info[module_idP][ue_mod_id].status == S_DL_SCHEDULED) {

          // clear scheduling flag
          eNB_dlsch_info[module_idP][ue_mod_id].status = S_DL_WAITING;
          rnti = find_UE_RNTI(module_idP,ue_mod_id);
          mac_xface->get_ue_active_harq_pid(module_idP,rnti,subframeP,&harq_pid,&round,0);
          nb_rb = eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb[harq_pid];

          DLSCH_dci = (void *)eNB_mac_inst[module_idP].UE_template[ue_mod_id].DLSCH_DCI[harq_pid];


          /// Synchronizing rballoc with rballoc_sub
          for(i=0;i<mac_xface->lte_frame_parms->N_RBGS;i++){
              rballoc_sub[i] = eNB_mac_inst[module_idP].UE_template[ue_mod_id].rballoc_subband[harq_pid][i];
              if(rballoc_sub[i] == 1)
                rballoc |= (0x0001<<i);  // TO be FIXED!!!!!!
          }


          switch(mac_xface->get_transmission_mode(module_idP,rnti)) {
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
                process_ue_cqi (module_idP,ue_mod_id),//aggregation,
                size_bits,
                format1,
                0);

            break;
          case 4:

            //if (nb_rb>10) {
            // DCI format 2_2A
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
            add_ue_spec_dci(DCI_pdu,
                DLSCH_dci,
                rnti,
                sizeof(DCI2_5MHz_2A_TDD_t),
                process_ue_cqi (module_idP,ue_mod_id),//aggregation,
                sizeof_DCI2_5MHz_2A_TDD_t,
                format2,
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
                process_ue_cqi (module_idP,ue_mod_id),//aggregation,
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
                process_ue_cqi (module_idP,ue_mod_id),//aggregation
                sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t,
                format1E_2A_M10PRB,
                0);
            break;

          }
      }


  }
  stop_meas(&eNB_mac_inst[module_idP].fill_DLSCH_dci);
}

