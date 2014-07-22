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
 * \author  Navid Nikaein and Raymond Knopp
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
#include "LAYER2/MAC/extern.h"

#include "LAYER2/MAC/proto.h"
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


void init_ue_sched_info(void){
  module_id_t i,j;
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



unsigned char get_ue_weight(module_id_t module_idP, module_id_t ue_mod_idP){

  return(eNB_dlsch_info[module_idP][ue_mod_idP].weight);

}

DCI_PDU *get_dci_sdu(module_id_t module_idP, frame_t frameP, sub_frame_t subframeP) {

  return(&eNB_mac_inst[module_idP].DCI_pdu);

}

module_id_t find_UE_id(module_id_t module_idP, rnti_t rnti) {

  module_id_t ue_mod_id;

  for (ue_mod_id=0;ue_mod_id<NUMBER_OF_UE_MAX;ue_mod_id++) {
    //   if (mac_get_rrc_status(module_idP,1,ue_mod_id) >= RRC_CONNECTED) {
      if (eNB_mac_inst[module_idP].UE_template[ue_mod_id].rnti==rnti) {
	return(ue_mod_id);
      }
  }
  return(module_id_t)(-1);

}



rnti_t find_UE_RNTI(module_id_t module_idP, module_id_t ue_mod_idP) {

  return (eNB_mac_inst[module_idP].UE_template[ue_mod_idP].rnti);

}
boolean_t is_UE_active(module_id_t module_idP, module_id_t ue_mod_idP ){
  if (eNB_mac_inst[module_idP].UE_template[ue_mod_idP].rnti !=0 )
    return TRUE;
  else
    return FALSE ;
}
uint8_t find_active_UEs(module_id_t module_idP){

  module_id_t        ue_mod_id      = 0;
  rnti_t        rnti         = 0;
  uint8_t            nb_active_ue = 0;

  for (ue_mod_id=0;ue_mod_id<NUMBER_OF_UE_MAX;ue_mod_id++) {

      if (((rnti=eNB_mac_inst[module_idP].UE_template[ue_mod_id].rnti) !=0)&&(eNB_mac_inst[module_idP].UE_template[ue_mod_id].ul_active==TRUE)){

          if (mac_xface->get_eNB_UE_stats(module_idP,rnti) != NULL){ // check at the phy enb_ue state for this rnti
              nb_active_ue++;
          }
          else { // this ue is removed at the phy => remove it at the mac as well
              mac_remove_ue(module_idP, ue_mod_id);
          }
      }
  }
  return(nb_active_ue);
}



// get aggregatiob form phy for a give UE
unsigned char process_ue_cqi (module_id_t module_idP, module_id_t ue_mod_idP) {
  unsigned char aggregation=2;
  // check the MCS and SNR and set the aggregation accordingly
  return aggregation;
}
#ifdef CBA
uint8_t find_num_active_UEs_in_cbagroup(module_id_t module_idP, unsigned char group_id){

  module_id_t    UE_id;
  rnti_t    rnti;
  unsigned char nb_ue_in_pusch=0;
  LTE_eNB_UE_stats* eNB_UE_stats;

  for (UE_id=group_id;UE_id<NUMBER_OF_UE_MAX;UE_id+=eNB_mac_inst[module_idP].num_active_cba_groups) {

      if (((rnti=eNB_mac_inst[module_idP].UE_template[UE_id].rnti) !=0) &&
          (eNB_mac_inst[module_idP].UE_template[UE_id].ul_active==TRUE)    &&
          (mac_get_rrc_status(module_idP,1,UE_id) > RRC_CONNECTED)){
          //  && (UE_is_to_be_scheduled(module_idP,UE_id)))
          // check at the phy enb_ue state for this rnti
          if ((eNB_UE_stats= mac_xface->get_eNB_UE_stats(module_idP,rnti)) != NULL){
              if ((eNB_UE_stats->mode == PUSCH) && (UE_is_to_be_scheduled(module_idP,UE_id) == 0)){
                  nb_ue_in_pusch++;
              }
          }
      }
  }
  return(nb_ue_in_pusch);
}
#endif
int8_t add_new_ue(module_id_t enb_mod_idP, rnti_t rntiP) {
  module_id_t ue_mod_id;
  int         j;

  for (ue_mod_id=0;ue_mod_id<NUMBER_OF_UE_MAX;ue_mod_id++) {
      if (eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].rnti == 0) {
          eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].rnti = rntiP;
          for (j=0;j<8;j++) {
              eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].oldNDI[j]    = 0;
              eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].oldNDI_UL[j] = 0;
          }
          eNB_ulsch_info[enb_mod_idP][ue_mod_id].status = S_UL_WAITING;
          eNB_dlsch_info[enb_mod_idP][ue_mod_id].status = S_UL_WAITING;
          LOG_D(MAC,"[eNB] Add UE_id %d : rnti %x\n",ue_mod_id,eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].rnti);
          return((int8_t)ue_mod_id);
      }
  }
  return(-1);
}

int8_t mac_remove_ue(module_id_t enb_mod_idP, module_id_t ue_mod_idP) {

  LOG_I(MAC,"Removing UE %d (rnti %x)\n",ue_mod_idP,eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].rnti);

  // clear all remaining pending transmissions
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].bsr_info[LCGID0]  = 0;
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].bsr_info[LCGID1]  = 0;
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].bsr_info[LCGID2]  = 0;
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].bsr_info[LCGID3]  = 0;

  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].ul_SR             = 0;
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].rnti              = 0;
  eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_idP].ul_active         = FALSE;
  eNB_ulsch_info[enb_mod_idP][ue_mod_idP].rnti                        = 0;
  eNB_ulsch_info[enb_mod_idP][ue_mod_idP].status                      = S_UL_NONE;
  eNB_dlsch_info[enb_mod_idP][ue_mod_idP].rnti                        = 0;
  eNB_dlsch_info[enb_mod_idP][ue_mod_idP].status                      = S_DL_NONE;

  rrc_eNB_free_UE_index(enb_mod_idP,ue_mod_idP);

  return(1);
}




void SR_indication(module_id_t enb_mod_idP, frame_t frameP, rnti_t rntiP, sub_frame_t subframeP) {

  module_id_t ue_mod_id = find_UE_id(enb_mod_idP, rntiP);
  
  if (ue_mod_id  != UE_INDEX_INVALID ) {
      LOG_D(MAC,"[eNB %d][SR %x] Frame %d subframeP %d Signaling SR for UE %d \n",enb_mod_idP,rntiP,frameP,subframeP, ue_mod_id);
      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].ul_SR = 1;
      eNB_mac_inst[enb_mod_idP].UE_template[ue_mod_id].ul_active = TRUE;
  } else {
    //     AssertFatal(0, "find_UE_id(%u,rnti %d) not found", enb_mod_idP, rntiP);
    AssertError(0, 0, "Frame %d: find_UE_id(%u,rnti %d) not found\n", frameP, enb_mod_idP, rntiP);
  }
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
  uint8_t first_element=0,last_size=0,i;
  uint8_t mac_header_control_elements[2*num_sdus],*ce_ptr;

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
    rnti_t rnti,
    unsigned char dci_size_bytes,
    unsigned char aggregation,
    unsigned char dci_size_bits,
    unsigned char dci_fmt,
    uint8_t ra_flag) {

  memcpy(&DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].dci_pdu[0],pdu,dci_size_bytes);
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].dci_length = dci_size_bits;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].L          = aggregation;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].rnti       = rnti;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].format     = dci_fmt;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci].ra_flag    = ra_flag;


  DCI_pdu->Num_common_dci++;
  LOG_D(MAC,"add common dci format %d for rnti %d \n",dci_fmt,rnti);
}

void add_ue_spec_dci(DCI_PDU *DCI_pdu,void *pdu,rnti_t rnti,unsigned char dci_size_bytes,unsigned char aggregation,unsigned char dci_size_bits,unsigned char dci_fmt,uint8_t ra_flag) {

  memcpy(&DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].dci_pdu[0],pdu,dci_size_bytes);
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].dci_length = dci_size_bits;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].L          = aggregation;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].rnti       = rnti;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].format     = dci_fmt;
  DCI_pdu->dci_alloc[DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci].ra_flag    = ra_flag;

  DCI_pdu->Num_ue_spec_dci++;
}





// This has to be updated to include BSR information
uint8_t UE_is_to_be_scheduled(module_id_t module_idP,uint8_t UE_id) {


  //  LOG_D(MAC,"[eNB %d][PUSCH] Frame %d subframeP %d Scheduling UE %d\n",module_idP,rnti,frameP,subframeP,
  //	UE_id);

  if ((eNB_mac_inst[module_idP].UE_template[UE_id].bsr_info[LCGID0]>0) ||
      (eNB_mac_inst[module_idP].UE_template[UE_id].bsr_info[LCGID1]>0) ||
      (eNB_mac_inst[module_idP].UE_template[UE_id].bsr_info[LCGID2]>0) ||
      (eNB_mac_inst[module_idP].UE_template[UE_id].bsr_info[LCGID3]>0) ||
      (eNB_mac_inst[module_idP].UE_template[UE_id].ul_SR>0)) // uplink scheduling request
    return(1);
  else
    return(0);
}




uint32_t allocate_prbs(module_id_t ue_mod_idP,unsigned char nb_rb, uint32_t *rballoc) {

  int i;
  uint32_t rballoc_dci=0;
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


uint32_t allocate_prbs_sub(int nb_rb, uint8_t *rballoc) {

  int check=0;//check1=0,check2=0;
  uint32_t rballoc_dci=0;
  //uint8_t number_of_subbands=13;

  LOG_T(MAC,"*****Check1RBALLOC****: %d%d%d%d (nb_rb %d,N_RBGS %d)\n",
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
  LOG_T(MAC,"*********RBALLOC : %x\n",rballoc_dci);
  // exit(-1);
  return (rballoc_dci);
}




void update_ul_dci(module_id_t module_idP,rnti_t rnti,uint8_t dai) {

  DCI_PDU             *DCI_pdu   = &eNB_mac_inst[module_idP].DCI_pdu;
  int                  i;
  DCI0_5MHz_TDD_1_6_t *ULSCH_dci = NULL;;

  if (mac_xface->lte_frame_parms->frame_type == TDD) {
      for (i=0;i<DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci;i++) {
          ULSCH_dci = (DCI0_5MHz_TDD_1_6_t *)DCI_pdu->dci_alloc[i].dci_pdu;
          if ((DCI_pdu->dci_alloc[i].format == format0) && (DCI_pdu->dci_alloc[i].rnti == rnti))
            ULSCH_dci->dai = (dai-1)&3;
      }
  }
  //  printf("Update UL DCI: DAI %d\n",dai);
}

