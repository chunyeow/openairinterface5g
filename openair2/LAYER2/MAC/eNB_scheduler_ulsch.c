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
 * \brief procedures related to UE on the ULSCH transport channel
 * \author Navid Nikaein and Raymond Knopp
 * \date 2010 - 2014
 * \email: navid.nikaein@eurecom.fr
 * \version 0.5
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


void add_ue_ulsch_info(module_id_t module_idP, module_id_t ue_mod_idP, sub_frame_t subframeP, UE_ULSCH_STATUS status){

  eNB_ulsch_info[module_idP][ue_mod_idP].rnti             = find_UE_RNTI(module_idP,ue_mod_idP);
  eNB_ulsch_info[module_idP][ue_mod_idP].subframe         = subframeP;
  eNB_ulsch_info[module_idP][ue_mod_idP].status           = status;

  eNB_ulsch_info[module_idP][ue_mod_idP].serving_num++;

}

module_id_t schedule_next_ulue(module_id_t module_idP, module_id_t ue_mod_idP, sub_frame_t subframeP){

  module_id_t next_ue;

  // first phase: scheduling for ACK
  switch (subframeP) {
  // scheduling for subframeP 2: for scheduled user during subframeP 5 and 6
  case 8:
    if  ((eNB_dlsch_info[module_idP][ue_mod_idP].status == S_DL_SCHEDULED) &&
        (eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 5 || eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 6)){
        // set the downlink status
        eNB_dlsch_info[module_idP][ue_mod_idP].status = S_DL_BUFFERED;
        return ue_mod_idP;
    }
    break;
    // scheduling for subframeP 3: for scheduled user during subframeP 7 and 8
  case 9:
    if  ((eNB_dlsch_info[module_idP][ue_mod_idP].status == S_DL_SCHEDULED) &&
        (eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 7 || eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 8)){
        eNB_dlsch_info[module_idP][ue_mod_idP].status = S_DL_BUFFERED;
        return ue_mod_idP;
    }
    break;
    // scheduling UL subframeP 4: for scheduled user during subframeP 9 and 0
  case 0 :
    if  ((eNB_dlsch_info[module_idP][ue_mod_idP].status == S_DL_SCHEDULED) &&
        (eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 9 || eNB_dlsch_info[module_idP][ue_mod_idP].subframe == 0)){
        eNB_dlsch_info[module_idP][ue_mod_idP].status = S_DL_BUFFERED;
        return ue_mod_idP;
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
		exit(-1);
	      }
          }
      }
  }
  *num_ce = num_ces;
  *num_sdu = num_sdus;

  return(mac_header_ptr);
}


void schedule_ulsch(module_id_t module_idP, frame_t frameP,unsigned char cooperation_flag,sub_frame_t subframeP, unsigned char sched_subframe,unsigned int *nCCE) {//,int calibration_flag) {

  start_meas(&eNB_mac_inst[module_idP].schedule_ulsch);
  uint8_t           granted_UEs;
  unsigned int nCCE_available;
  uint16_t first_rb=1,i;

  granted_UEs = find_ulgranted_UEs(module_idP);
  nCCE_available = mac_xface->get_nCCE_max(module_idP) - *nCCE;

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
      if ((eNB_mac_inst[module_idP].RA_template[i].RA_active == TRUE) &&
          (eNB_mac_inst[module_idP].RA_template[i].generate_rar == 0) &&
          (eNB_mac_inst[module_idP].RA_template[i].Msg3_subframe == sched_subframe)) {
          first_rb++;
          break;
      }
  }

  schedule_ulsch_rnti(module_idP, cooperation_flag, frameP, subframeP, sched_subframe, granted_UEs, nCCE, &nCCE_available, &first_rb);

#ifdef CBA
  if ((eNB_mac_inst[module_idP].num_active_cba_groups > 0) && (*nCCE == 0))
    schedule_ulsch_cba_rnti(module_idP, cooperation_flag, frameP, subframeP, sched_subframe, granted_UEs, nCCE, &nCCE_available, &first_rb);
#endif
  stop_meas(&eNB_mac_inst[module_idP].schedule_ulsch);

}



void schedule_ulsch_rnti(module_id_t   module_idP,
                         unsigned char cooperation_flag,
                         frame_t       frameP,
                         sub_frame_t   subframeP,
                         unsigned char sched_subframe,
                         uint8_t            granted_UEs,
                         unsigned int *nCCE,
                         unsigned int *nCCE_available,
                         uint16_t          *first_rb){
  module_id_t             ue_mod_id        = -1;
  module_id_t             next_ue        = -1;
  unsigned char      aggregation    = 2;
  rnti_t             rnti           = -1;
  uint8_t                 round          = 0;
  uint8_t                 harq_pid       = 0;
  void              *ULSCH_dci      = NULL;
  LTE_eNB_UE_stats  *eNB_UE_stats   = NULL;
  DCI_PDU           *DCI_pdu        = &eNB_mac_inst[module_idP].DCI_pdu;
  uint8_t                 status         = 0;
  uint8_t                 rb_table_index = -1;
  uint16_t                TBS,i;
  uint32_t                buffer_occupancy;
  uint32_t                tmp_bsr;
  uint32_t                cqi_req,cshift,ndi,mcs,rballoc;

  for (ue_mod_id=0;ue_mod_id<granted_UEs && (*nCCE_available > (1<<aggregation));ue_mod_id++) {

      //    msg("[MAC][eNB] subframeP %d: checking UE_id %d\n",subframeP,UE_id);
      next_ue = ue_mod_id;     // find next ue to schedule
      //    msg("[MAC][eNB] subframeP %d: next ue %d\n",subframeP,next_ue);
      rnti = find_UE_RNTI(module_idP,next_ue); // radio network temp id is obtained
      //    msg("[MAC][eNB] subframeP %d: rnti %x\n",subframeP,rnti);

      if (rnti==0) // if so, go to next UE
        continue;

      eNB_UE_stats = mac_xface->get_eNB_UE_stats(module_idP,rnti);
      if (eNB_UE_stats==NULL)
        mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");

      LOG_I(MAC,"[eNB %d] Scheduler Frame %d, subframeP %d, nCCE %d: Checking ULSCH next UE_id %d mode id %d (rnti %x,mode %s), format 0\n",
          module_idP,frameP,subframeP,*nCCE,next_ue,module_idP, rnti,mode_string[eNB_UE_stats->mode]);

      if (eNB_UE_stats->mode == PUSCH) { // ue has a ulsch channel
          int8_t ret;
          // Get candidate harq_pid from PHY
          ret = mac_xface->get_ue_active_harq_pid(module_idP,rnti,subframeP,&harq_pid,&round,1);
          LOG_I(MAC,"Got harq_pid %d, round %d, next_ue %d\n",harq_pid,round,next_ue);

          /* [SR] 01/07/13: Don't schedule UE if we cannot get harq pid */
#ifndef EXMIMO_IOT
          if ((((UE_is_to_be_scheduled(module_idP,ue_mod_id)>0)) || (round>0) || ((frameP%10)==0)) && (ret == 0))
            // if there is information on bsr of DCCH, DTCH or if there is UL_SR, or if there is a packet to retransmit, or we want to schedule a periodic feedback every 10 frames
#else
            if (round==0)
#endif
              {
                LOG_D(MAC,"[eNB %d][PUSCH %x] Frame %d subframeP %d Scheduling UE %d (SR %d)\n",
                    module_idP,rnti,frameP,subframeP,ue_mod_id,
                    eNB_mac_inst[module_idP].UE_template[ue_mod_id].ul_SR);

                // reset the scheduling request
                eNB_mac_inst[module_idP].UE_template[ue_mod_id].ul_SR = 0;

                aggregation = process_ue_cqi(module_idP,next_ue); // =2 by default!!
                //    msg("[MAC][eNB] subframeP %d: aggregation %d\n",subframeP,aggregation);

                status = mac_get_rrc_status(module_idP,1,next_ue);

                if (status < RRC_CONNECTED)
                  cqi_req = 0;
                else
                  cqi_req = 1;


                if (round > 0) {
                    ndi = eNB_mac_inst[module_idP].UE_template[ue_mod_id].oldNDI_UL[harq_pid];
                    mcs = (round&3) + 28; //not correct for round==4!
                }
                else {
                    ndi = 1-eNB_mac_inst[module_idP].UE_template[ue_mod_id].oldNDI_UL[harq_pid];
                    eNB_mac_inst[module_idP].UE_template[ue_mod_id].oldNDI_UL[harq_pid]=ndi;
                    mcs = openair_daq_vars.target_ue_ul_mcs;
                }

                LOG_D(MAC,"[eNB %d] ULSCH scheduler: Ndi %d, mcs %d\n",module_idP,ndi,mcs);

                if((cooperation_flag > 0) && (next_ue == 1)) { // Allocation on same set of RBs
                    // RIV:resource indication value // function in openair1/PHY/LTE_TRANSPORT/dci_tools.c
                    rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
                        ((next_ue-1)*4),//openair_daq_vars.ue_ul_nb_rb),
                        4);//openair_daq_vars.ue_ul_nb_rb);
                }
                else if ((round==0) && (mcs < 29)) {
                    rb_table_index = 1;
                    TBS = mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
                    buffer_occupancy = ((eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0]  == 0) &&
                        (eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1]  == 0) &&
                        (eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2]  == 0) &&
                        (eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3] == 0))?
                            BSR_TABLE[10] :   // This is when we've received SR and buffers are fully served
                            BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0]]+
                            BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1]]+
                            BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2]]+
                            BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3]];  // This is when remaining data in UE buffers (even if SR is triggered)

                    LOG_D(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframeP %d Scheduled UE, BSR for LCGID0 %d, LCGID1 %d, LCGID2 %d LCGID3 %d, BO %d\n",
                        module_idP,
                        ue_mod_id,
                        rnti,
                        frameP,
                        subframeP,
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0],
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1],
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2],
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3],
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
                        module_idP,ue_mod_id,rnti,frameP,subframeP,mcs,
                        *first_rb,rb_table[rb_table_index],
                        rb_table_index,mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]),
                        harq_pid);

                    rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
                        *first_rb,
                        rb_table[rb_table_index]);//openair_daq_vars.ue_ul_nb_rb);

                    *first_rb+=rb_table[rb_table_index];  // increment for next UE allocation
                    eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb_ul[harq_pid] = rb_table[rb_table_index]; //store for possible retransmission

                    buffer_occupancy -= mac_xface->get_TBS_UL(mcs,rb_table[rb_table_index]);
                    i = bytes_to_bsr_index((int32_t)buffer_occupancy);

                    // Adjust BSR entries for LCGIDs
                    if (i>0) {
                        if (eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0] <= i) {
                            tmp_bsr = BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0]];
                            eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0] = 0;
                            if (BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1]] <= (buffer_occupancy-tmp_bsr)) {
                                tmp_bsr += BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1]];
                                eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1] = 0;
                                if (BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2]] <= (buffer_occupancy-tmp_bsr)) {
                                    tmp_bsr += BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2]];
                                    eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2] = 0;
                                    if (BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3]] <= (buffer_occupancy-tmp_bsr)) {
                                        tmp_bsr += BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3]];
                                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3] = 0;
                                    } else {
                                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3] = bytes_to_bsr_index((int32_t)BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3]] - ((int32_t)buffer_occupancy - (int32_t)tmp_bsr));
                                    }
                                }
                                else {
                                    eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2] = bytes_to_bsr_index((int32_t)BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2]] - ((int32_t)buffer_occupancy -(int32_t)tmp_bsr));
                                }
                            }
                            else {
                                eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1] = bytes_to_bsr_index((int32_t)BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1]] - (int32_t)buffer_occupancy);
                            }
                        }
                        else {
                            eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0] = bytes_to_bsr_index((int32_t)BSR_TABLE[eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0]] - (int32_t)buffer_occupancy);
                        }
                    }
                    else {  // we have flushed all buffers so clear bsr
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID0] = 0;
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID1] = 0;
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID2] = 0;
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].bsr_info[LCGID3] = 0;
                    }


                } // ndi==1
                else { //we schedule a retransmission
                    LOG_I(MAC,"[eNB %d][PUSCH %d/%x] Frame %d subframeP %d Scheduled UE retransmission (mcs %d, first rb %d, nb_rb %d, TBS %d, harq_pid %d)\n",
                        module_idP,ue_mod_id,rnti,frameP,subframeP,mcs,
                        *first_rb,eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb_ul[harq_pid],
                        mac_xface->get_TBS_UL(mcs,eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb_ul[harq_pid]),
                        harq_pid);

                    rballoc = mac_xface->computeRIV(mac_xface->lte_frame_parms->N_RB_UL,
                        *first_rb,
                        eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb_ul[harq_pid]);

                    *first_rb+=eNB_mac_inst[module_idP].UE_template[ue_mod_id].nb_rb_ul[harq_pid];  // increment for next UE allocation
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
                      ULSCH_dci = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
                      ((DCI0_1_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[sched_subframe];
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
                      ULSCH_dci = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
                      ((DCI0_5MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[sched_subframe];
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
                      ULSCH_dci = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[sched_subframe];
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
                      ULSCH_dci = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->type     = 0;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->hopping  = 0;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->rballoc  = rballoc;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->mcs      = mcs;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->ndi      = ndi;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->TPC      = 1;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->cshift   = cshift;
                      ((DCI0_10MHz_TDD_1_6_t *)ULSCH_dci)->padding  = 0;
                      ((DCI0_20MHz_TDD_1_6_t *)ULSCH_dci)->dai      = eNB_mac_inst[module_idP].UE_template[next_ue].DAI_ul[sched_subframe];
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

                      ULSCH_dci          = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

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
                      ULSCH_dci          = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

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
                      ULSCH_dci          = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

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
                      ULSCH_dci          = eNB_mac_inst[module_idP].UE_template[next_ue].ULSCH_DCI[harq_pid];

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
                    next_ue,
                    subframeP,
                    S_UL_SCHEDULED);

                *nCCE = (*nCCE) + (1<<aggregation);
                *nCCE_available = mac_xface->get_nCCE_max(module_idP) - *nCCE;
                //msg("[MAC][eNB %d][ULSCH Scheduler] Frame %d, subframeP %d: Generated ULSCH DCI for next UE_id %d, format 0\n", module_idP,frameP,subframeP,next_ue);

                //break; // leave loop after first UE is schedule (avoids m
              } // UE_is_to_be_scheduled
      } // UE is in PUSCH
  } // loop over UE_id
}

uint8_t find_ulgranted_UEs(module_id_t module_idP){

  // all active users should be granted
  return(find_active_UEs(module_idP));
}

unsigned char *get_dlsch_sdu(module_id_t module_idP, frame_t frameP, rnti_t rntiP, uint8_t TBindex) {

  module_id_t ue_mod_id;

  if (rntiP==SI_RNTI) {
      LOG_D(MAC,"[eNB %d] Frame %d Get DLSCH sdu for BCCH \n",module_idP,frameP);

      return((unsigned char *)&eNB_mac_inst[module_idP].BCCH_pdu.payload[0]);
  }
  else if ((ue_mod_id = find_UE_id(module_idP,rntiP)) != UE_INDEX_INVALID ){
    LOG_D(MAC,"[eNB %d] Frame %d:  Get DLSCH sdu for rnti %x => UE_id %d\n",module_idP,frameP,rntiP,ue_mod_id);
    return((unsigned char *)&eNB_mac_inst[module_idP].DLSCH_pdu[ue_mod_id][TBindex].payload[0]);
  } else {
    LOG_E(MAC,"[eNB %d] Frame %d: UE with RNTI %x does not exist\n", module_idP,frameP,rntiP);
    return NULL;
  }

}

#ifdef CBA
void schedule_ulsch_cba_rnti(module_id_t module_idP, unsigned char cooperation_flag, frame_t frameP, sub_frame_t subframeP, unsigned char sched_subframe, uint8_t granted_UEs, unsigned int *nCCE, unsigned int *nCCE_available, uint16_t *first_rb){

  DCI0_5MHz_TDD_1_6_t *ULSCH_dci_tdd16;
  DCI0_5MHz_FDD_t *ULSCH_dci_fdd;
  DCI_PDU *DCI_pdu= &eNB_mac_inst[module_idP].DCI_pdu;

  uint8_t rb_table_index=0, aggregation=2;
  uint32_t rballoc;
  uint8_t cba_group, cba_resources;
  uint8_t required_rbs[NUM_MAX_CBA_GROUP], weight[NUM_MAX_CBA_GROUP], num_cba_resources[NUM_MAX_CBA_GROUP];
  uint8_t available_rbs= ceil(mac_xface->lte_frame_parms->N_RB_UL-1-*first_rb);
  uint8_t remaining_rbs= available_rbs;
  uint8_t allocated_rbs;
  // We compute the weight of each group and initialize some variables
  for (cba_group=0;cba_group<eNB_mac_inst[module_idP].num_active_cba_groups;cba_group++) {
      // UEs in PUSCH with traffic
      weight[cba_group] = find_num_active_UEs_in_cbagroup(module_idP, cba_group);
      required_rbs[cba_group] = 0;
      num_cba_resources[cba_group]=0;
  }
  //LOG_D(MAC, "[eNB ] CBA granted ues are %d\n",granted_UEs );
  for (cba_group=0;cba_group<eNB_mac_inst[module_idP].num_active_cba_groups  && (*nCCE_available > (1<<aggregation));cba_group++) {
      if (remaining_rbs <= 0 )
        break;
      // If the group needs some resource
      if ((weight[cba_group] > 0) && eNB_mac_inst[module_idP].cba_rnti[cba_group] != 0){
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

          while ((*nCCE) + (1<<aggregation) * num_cba_resources[cba_group] > *nCCE_available)
            num_cba_resources[cba_group]--;

          LOG_N(MAC,"[eNB %d] Frame %d, subframeP %d: cba group %d weight/granted_ues %d/%d available/required rb (%d/%d), num resources %d->1 (*scaled down*) \n",
              module_idP, frameP, subframeP, cba_group,
              weight[cba_group], granted_UEs, available_rbs,required_rbs[cba_group],
              num_cba_resources[cba_group]);

          num_cba_resources[cba_group]=1;

      }
  }
  // phase 2
  for (cba_group=0;cba_group<eNB_mac_inst[module_idP].num_active_cba_groups;cba_group++) {
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
              module_idP, frameP, subframeP, cba_group,eNB_mac_inst[module_idP].cba_rnti[cba_group],
              available_rbs, required_rbs[cba_group], allocated_rbs, remaining_rbs,rballoc,
              *nCCE_available,*nCCE);


          if (mac_xface->lte_frame_parms->frame_type == TDD) {
              ULSCH_dci_tdd16 = (DCI0_5MHz_TDD_1_6_t *)eNB_mac_inst[module_idP].UE_template[cba_group].ULSCH_DCI[0];

              ULSCH_dci_tdd16->type     = 0;
              ULSCH_dci_tdd16->hopping  = 0;
              ULSCH_dci_tdd16->rballoc  = rballoc;
              ULSCH_dci_tdd16->mcs      = 2;
              ULSCH_dci_tdd16->ndi      = 1;
              ULSCH_dci_tdd16->TPC      = 1;
              ULSCH_dci_tdd16->cshift   = cba_group;
              ULSCH_dci_tdd16->dai      = eNB_mac_inst[module_idP].UE_template[cba_group].DAI_ul[sched_subframe];
              ULSCH_dci_tdd16->cqi_req  = 1;

              //add_ue_spec_dci
              add_common_dci(DCI_pdu,
                  ULSCH_dci_tdd16,
                  eNB_mac_inst[module_idP].cba_rnti[cba_group],
                  sizeof(DCI0_5MHz_TDD_1_6_t),
                  aggregation,
                  sizeof_DCI0_5MHz_TDD_1_6_t,
                  format0,
                  0);
          }
          else {
              ULSCH_dci_fdd           = (DCI0_5MHz_FDD_t *)eNB_mac_inst[module_idP].UE_template[cba_group].ULSCH_DCI[0];

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
                  eNB_mac_inst[module_idP].cba_rnti[cba_group],
                  sizeof(DCI0_5MHz_FDD_t),
                  aggregation,
                  sizeof_DCI0_5MHz_FDD_t,
                  format0,
                  0);
          }
          *nCCE = (*nCCE) + (1<<aggregation) * num_cba_resources[cba_group];
          *nCCE_available = mac_xface->get_nCCE_max(module_idP) - *nCCE;
          //      break;// for the moment only schedule one
      }
  }
}
#endif
