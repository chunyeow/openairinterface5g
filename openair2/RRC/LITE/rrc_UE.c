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
/*! \file rrc_UE.c
 * \brief rrc procedures for UE
 * \author Navid Nikaein and Raymond Knopp
 * \date 2011 - 2014
 * \version 1.0
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr and raymond.knopp@eurecom.fr
 */

#define RRC_UE
#define RRC_UE_C

#include "assertions.h"
#include "asn1_conversions.h"
#include "defs.h"
#include "PHY/TOOLS/dB_routines.h"
#include "extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
#include "COMMON/mac_rrc_primitives.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#ifndef CELLULAR
#include "RRC/LITE/MESSAGES/asn1_msg.h"
#endif
#include "RRCConnectionRequest.h"
#include "RRCConnectionReconfiguration.h"
#include "UL-CCCH-Message.h"
#include "DL-CCCH-Message.h"
#include "UL-DCCH-Message.h"
#include "DL-DCCH-Message.h"
#include "BCCH-DL-SCH-Message.h"
#ifdef Rel10
#include "MCCH-Message.h"
#endif
#include "MeasConfig.h"
#include "MeasGapConfig.h"
#include "MeasObjectEUTRA.h"
#include "TDD-Config.h"
#include "UECapabilityEnquiry.h"
#include "UE-CapabilityRequest.h"
#ifdef PHY_ABSTRACTION
#include "OCG.h"
#include "OCG_extern.h"
#endif
#ifdef USER_MODE
#include "RRC/NAS/nas_config.h"
#include "RRC/NAS/rb_config.h"
#endif
#ifdef ENABLE_RAL
#include "rrc_UE_ral.h"
#endif

#if defined(ENABLE_SECURITY)
# include "UTIL/OSA/osa_defs.h"
#endif

#include "pdcp.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#ifdef PHY_EMUL
extern EMULATION_VARS *Emul_vars;
#endif
extern eNB_MAC_INST *eNB_mac_inst;
extern UE_MAC_INST *UE_mac_inst;
#ifdef BIGPHYSAREA
extern void *bigphys_malloc(int);
#endif

//#define XER_PRINT

extern inline unsigned int taus(void);
extern int8_t dB_fixed2(uint32_t x,uint32_t y);

/*------------------------------------------------------------------------------*/
static Rrc_State_t rrc_get_state (module_id_t ue_mod_idP) {
  return UE_rrc_inst[ue_mod_idP].RrcState;
}

static Rrc_Sub_State_t rrc_get_sub_state (module_id_t ue_mod_idP) {
  return UE_rrc_inst[ue_mod_idP].RrcSubState;
}

static int rrc_set_state (module_id_t ue_mod_idP, Rrc_State_t state) {
  AssertFatal ((RRC_STATE_FIRST <= state) && (state <= RRC_STATE_LAST),
      "Invalid state %d!\n", state);

  if (UE_rrc_inst[ue_mod_idP].RrcState != state) {
      UE_rrc_inst[ue_mod_idP].RrcState = state;

#if defined(ENABLE_ITTI)
      {
        MessageDef *msg_p;

        msg_p = itti_alloc_new_message(TASK_RRC_UE, RRC_STATE_IND);
        RRC_STATE_IND(msg_p).state = UE_rrc_inst[ue_mod_idP].RrcState;
        RRC_STATE_IND(msg_p).sub_state = UE_rrc_inst[ue_mod_idP].RrcSubState;

        itti_send_msg_to_task(TASK_UNKNOWN, NB_eNB_INST + ue_mod_idP, msg_p);
      }
#endif
      return (1);
  }

  return (0);
}

static int rrc_set_sub_state (module_id_t ue_mod_idP, Rrc_Sub_State_t subState) {
#if (defined(ENABLE_ITTI) && (defined(ENABLE_USE_MME) || defined(ENABLE_RAL)))
  switch (UE_rrc_inst[ue_mod_idP].RrcState) {
  case RRC_STATE_INACTIVE:
    AssertFatal ((RRC_SUB_STATE_INACTIVE_FIRST <= subState) && (subState <= RRC_SUB_STATE_INACTIVE_LAST),
        "Invalid sub state %d for state %d!\n", subState, UE_rrc_inst[ue_mod_idP].RrcState);
    break;

  case RRC_STATE_IDLE:
    AssertFatal ((RRC_SUB_STATE_IDLE_FIRST <= subState) && (subState <= RRC_SUB_STATE_IDLE_LAST),
        "Invalid sub state %d for state %d!\n", subState, UE_rrc_inst[ue_mod_idP].RrcState);
    break;

  case RRC_STATE_CONNECTED:
    AssertFatal ((RRC_SUB_STATE_CONNECTED_FIRST <= subState) && (subState <= RRC_SUB_STATE_CONNECTED_LAST),
        "Invalid sub state %d for state %d!\n", subState, UE_rrc_inst[ue_mod_idP].RrcState);
    break;
  }
#endif

  if (UE_rrc_inst[ue_mod_idP].RrcSubState != subState) {
      UE_rrc_inst[ue_mod_idP].RrcSubState = subState;

#if defined(ENABLE_ITTI)
      {
        MessageDef *msg_p;

        msg_p = itti_alloc_new_message(TASK_RRC_UE, RRC_STATE_IND);
        RRC_STATE_IND(msg_p).state = UE_rrc_inst[ue_mod_idP].RrcState;
        RRC_STATE_IND(msg_p).sub_state = UE_rrc_inst[ue_mod_idP].RrcSubState;

        itti_send_msg_to_task(TASK_UNKNOWN, NB_eNB_INST + ue_mod_idP, msg_p);
      }
#endif
      return (1);
  }

  return (0);
}

/*------------------------------------------------------------------------------*/
void init_SI_UE(module_id_t ue_mod_idP,uint8_t eNB_index) {

  int i;


  UE_rrc_inst[ue_mod_idP].sizeof_SIB1[eNB_index] = 0;
  UE_rrc_inst[ue_mod_idP].sizeof_SI[eNB_index] = 0;

  UE_rrc_inst[ue_mod_idP].SIB1[eNB_index] = (uint8_t *)malloc16(32);
  UE_rrc_inst[ue_mod_idP].sib1[eNB_index] = (SystemInformationBlockType1_t *)malloc16(sizeof(SystemInformationBlockType1_t));
  UE_rrc_inst[ue_mod_idP].SI[eNB_index] = (uint8_t *)malloc16(64);

  for (i=0;i<NB_CNX_UE;i++) {
      UE_rrc_inst[ue_mod_idP].si[eNB_index][i] = (SystemInformation_t *)malloc16(sizeof(SystemInformation_t));
  }

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status = 0;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus = 0;
}

#ifdef Rel10
void init_MCCH_UE(module_id_t ue_mod_idP, uint8_t eNB_index) {
  int i;
  UE_rrc_inst[ue_mod_idP].sizeof_MCCH_MESSAGE[eNB_index] = 0;
  UE_rrc_inst[ue_mod_idP].MCCH_MESSAGE[eNB_index] = (uint8_t *)malloc16(32);
  UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index] = (MBSFNAreaConfiguration_r9_t *)malloc16(sizeof(MBSFNAreaConfiguration_r9_t));
  for (i=0; i<8;i++) // MAX MBSFN Area
    UE_rrc_inst[ue_mod_idP].Info[eNB_index].MCCHStatus[i] = 0;

}
#endif

static
void openair_rrc_lite_ue_init_security(module_id_t ue_mod_idP)
{
#if defined(ENABLE_SECURITY)
  //    uint8_t *kRRCenc;
  //    uint8_t *kRRCint;
  char ascii_buffer[65];
  uint8_t i;

  memset(UE_rrc_inst[ue_mod_idP].kenb, ue_mod_idP, 32);

  for (i = 0; i < 32; i++) {
      sprintf(&ascii_buffer[2 * i], "%02X", UE_rrc_inst[ue_mod_idP].kenb[i]);
  }

  LOG_T(RRC, "[OSA][UE %02d] kenb    = %s\n", ue_mod_idP, ascii_buffer);
#endif
}

/*------------------------------------------------------------------------------*/
char openair_rrc_lite_ue_init(module_id_t ue_mod_idP, unsigned char eNB_index){
  /*-----------------------------------------------------------------------------*/
  rrc_set_state (ue_mod_idP, RRC_STATE_INACTIVE);
  rrc_set_sub_state (ue_mod_idP, RRC_SUB_STATE_INACTIVE);

  LOG_D(RRC,"[UE %d] INIT State = RRC_IDLE (eNB %d)\n",ue_mod_idP,eNB_index);
  LOG_D(RRC,"[MSC_NEW][FRAME 00000][RRC_UE][MOD %02d][]\n", ue_mod_idP+NB_eNB_INST);
  LOG_D(RRC, "[MSC_NEW][FRAME 00000][IP][MOD %02d][]\n", ue_mod_idP+NB_eNB_INST);

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].State=RRC_IDLE;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].T300_active = 0;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].T304_active = 0;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].T310_active = 0;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].UE_index=0xffff;
  UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Active=0;
  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Active=0;
  UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Active=0;
  UE_rrc_inst[ue_mod_idP].HandoverInfoUe.measFlag=1;

  UE_rrc_inst[ue_mod_idP].ciphering_algorithm = SecurityAlgorithmConfig__cipheringAlgorithm_eea0;
#ifdef Rel10
  UE_rrc_inst[ue_mod_idP].integrity_algorithm = SecurityAlgorithmConfig__integrityProtAlgorithm_eia0_v920;
#else
  UE_rrc_inst[ue_mod_idP].integrity_algorithm = SecurityAlgorithmConfig__integrityProtAlgorithm_reserved;
#endif

  openair_rrc_lite_ue_init_security(ue_mod_idP);

  init_SI_UE(ue_mod_idP,eNB_index);
  LOG_D(RRC,"[UE %d] INIT: phy_sync_2_ch_ind\n", ue_mod_idP);

#ifndef NO_RRM
  send_msg(&S_rrc,msg_rrc_phy_synch_to_CH_ind(ue_mod_idP,eNB_index,UE_rrc_inst[ue_mod_idP].Mac_id));
#endif

#ifdef NO_RRM //init ch SRB0, SRB1 & BDTCH
  openair_rrc_on(ue_mod_idP,0);
#endif
#ifdef CBA 
  int j;
  for(j=0; j<NUM_MAX_CBA_GROUP; j++)
    UE_rrc_inst[ue_mod_idP].cba_rnti[j] = 0x0000;
  UE_rrc_inst[ue_mod_idP].num_active_cba_groups = 0;
#endif

  return 0;
}

/*------------------------------------------------------------------------------*/
void rrc_ue_generate_RRCConnectionRequest(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index){
  /*------------------------------------------------------------------------------*/

  uint8_t i=0,rv[6];

  if(UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.payload_size ==0){

      // Get RRCConnectionRequest, fill random for now
      // Generate random byte stream for contention resolution
      for (i=0;i<6;i++) {
#ifdef SMBV   
          // if SMBV is configured the contention resolution needs to be fix for the connection procedure to succeed
          rv[i]=i;
#else
          rv[i]=taus()&0xff;
#endif
          LOG_T(RRC,"%x.",rv[i]);
      }
      LOG_T(RRC,"\n");
      UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.payload_size = do_RRCConnectionRequest(ue_mod_idP, (uint8_t *)UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.Payload,rv);

      LOG_I(RRC,"[UE %d] : Frame %d, Logical Channel UL-CCCH (SRB0), Generating RRCConnectionRequest (bytes %d, eNB %d)\n",
          ue_mod_idP, frameP, UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.payload_size, eNB_index);

      for (i=0;i<UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.payload_size;i++) {
          LOG_T(RRC,"%x.",UE_rrc_inst[ue_mod_idP].Srb0[eNB_index].Tx_buffer.Payload[i]);
      }
      LOG_T(RRC,"\n");
      /*
      UE_rrc_inst[ue_mod_idP].Srb0[Idx].Tx_buffer.Payload[i] = taus()&0xff;

    UE_rrc_inst[ue_mod_idP].Srb0[Idx].Tx_buffer.payload_size =i;
       */

  }
}


mui_t rrc_mui=0;

/* NAS Attach request with IMSI */
static const char const nas_attach_req_imsi[] =
    {
        0x07, 0x41,
        /* EPS Mobile identity = IMSI */
        0x71, 0x08, 0x29, 0x80, 0x43, 0x21, 0x43, 0x65, 0x87,
        0xF9,
        /* End of EPS Mobile Identity */
        0x02, 0xE0, 0xE0, 0x00, 0x20, 0x02, 0x03,
        0xD0, 0x11, 0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00,
        0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0A, 0x00, 0x52, 0x12, 0xF2,
        0x01, 0x27, 0x11,
    };

/* NAS Attach request with GUTI */
static const char const nas_attach_req_guti[] =
    {
        0x07, 0x41,
        /* EPS Mobile identity = GUTI */
        0x71, 0x0B, 0xF6, 0x12, 0xF2, 0x01, 0x80, 0x00, 0x01, 0xE0, 0x00,
        0xDA, 0x1F,
        /* End of EPS Mobile Identity */
        0x02, 0xE0, 0xE0, 0x00, 0x20, 0x02, 0x03,
        0xD0, 0x11, 0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00,
        0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0A, 0x00, 0x52, 0x12, 0xF2,
        0x01, 0x27, 0x11,
    };

/*------------------------------------------------------------------------------*/
void rrc_ue_generate_RRCConnectionSetupComplete(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index, uint8_t Transaction_id){
  /*------------------------------------------------------------------------------*/

  uint8_t    buffer[100];
  uint8_t    size;
  const char * nas_msg;
  int   nas_msg_length;

#if defined(ENABLE_ITTI) && defined(ENABLE_USE_MME)
  nas_msg         = (char *) UE_rrc_inst[ue_mod_idP].initialNasMsg.data;
  nas_msg_length  = UE_rrc_inst[ue_mod_idP].initialNasMsg.length;
#else
  nas_msg         = nas_attach_req_imsi;
  nas_msg_length  = sizeof(nas_attach_req_imsi);
#endif

  size = do_RRCConnectionSetupComplete(ue_mod_idP, buffer, Transaction_id, nas_msg_length, nas_msg);

  LOG_I(RRC,"[UE %d][RAPROC] Frame %d : Logical Channel UL-DCCH (SRB1), Generating RRCConnectionSetupComplete (bytes%d, eNB %d)\n",
      ue_mod_idP,frameP, size, eNB_index);

  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (RRCConnectionSetupComplete to eNB %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
      frameP, ue_mod_idP+NB_eNB_INST, size, eNB_index, rrc_mui, ue_mod_idP+NB_eNB_INST, DCCH);

  //  rrc_rlc_data_req(ue_mod_idP+NB_eNB_INST,frameP, 0 ,DCCH,rrc_mui++,0,size,(char*)buffer);
  pdcp_rrc_data_req (eNB_index, ue_mod_idP, frameP, 0, DCCH, rrc_mui++, 0, size, buffer, 1);
}

void rrc_ue_generate_RRCConnectionReconfigurationComplete(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index, uint8_t Transaction_id) {

  uint8_t buffer[32], size;

  size = do_RRCConnectionReconfigurationComplete(ue_mod_idP, buffer, Transaction_id);

  LOG_I(RRC,"[UE %d] Frame %d : Logical Channel UL-DCCH (SRB1), Generating RRCConnectionReconfigurationComplete (bytes %d, eNB_index %d)\n",
      ue_mod_idP,frameP, size, eNB_index);
  LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- PDCP_DATA_REQ/%d Bytes (RRCConnectionReconfigurationComplete to eNB %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
      frameP, ue_mod_idP+NB_eNB_INST, size, eNB_index, rrc_mui, ue_mod_idP+NB_eNB_INST, DCCH);

  //rrc_rlc_data_req(ue_mod_idP+NB_eNB_INST,frameP, 0 ,DCCH,rrc_mui++,0,size,(char*)buffer);
  pdcp_rrc_data_req (eNB_index, ue_mod_idP, frameP, 0, DCCH, rrc_mui++, 0, size, buffer, 1);
}

/*------------------------------------------------------------------------------*/
int rrc_ue_decode_ccch(module_id_t ue_mod_idP, frame_t frameP, SRB_INFO *Srb_info, uint8_t eNB_index){
  /*------------------------------------------------------------------------------*/

  //DL_CCCH_Message_t dlccchmsg;
  DL_CCCH_Message_t *dl_ccch_msg=NULL;//&dlccchmsg;
  asn_dec_rval_t dec_rval;
  int rval=0;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_CCCH, VCD_FUNCTION_IN);

  //memset(dl_ccch_msg,0,sizeof(DL_CCCH_Message_t));

  //  LOG_D(RRC,"[UE %d] Decoding DL-CCCH message (%d bytes), State %d\n",ue_mod_idP,Srb_info->Rx_buffer.payload_size,
  //  UE_rrc_inst[ue_mod_idP].Info[eNB_index].State);

  dec_rval = uper_decode(NULL,
      &asn_DEF_DL_CCCH_Message,
      (void**)&dl_ccch_msg,
      (uint8_t*)Srb_info->Rx_buffer.Payload,
      100,0,0);

#ifdef XER_PRINT
  xer_fprint(stdout,&asn_DEF_DL_CCCH_Message,(void*)dl_ccch_msg);
#endif

#if defined(ENABLE_ITTI)
# if defined(DISABLE_ITTI_XER_PRINT)
  {
    MessageDef *msg_p;

    msg_p = itti_alloc_new_message (TASK_RRC_UE, RRC_DL_CCCH_MESSAGE);
    memcpy (&msg_p->ittiMsg, (void *) dl_ccch_msg, sizeof(RrcDlCcchMessage));

    itti_send_msg_to_task (TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
  }
# else
  {
    char        message_string[10000];
    size_t      message_string_size;

    if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_DL_CCCH_Message, (void *)dl_ccch_msg)) > 0)
      {
        MessageDef *msg_p;

        msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_DL_CCCH, message_string_size + sizeof (IttiMsgText));
        msg_p->ittiMsg.rrc_dl_ccch.size = message_string_size;
        memcpy(&msg_p->ittiMsg.rrc_dl_ccch.text, message_string, message_string_size);

        itti_send_msg_to_task(TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
      }
  }
# endif
#endif

  if ((dec_rval.code != RC_OK) && (dec_rval.consumed==0)) {
      LOG_E(RRC,"[UE %d] Frame %d : Failed to decode DL-CCCH-Message (%d bytes)\n",ue_mod_idP,dec_rval.consumed);
      vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_CCCH, VCD_FUNCTION_OUT);
      return -1;
  }

  if (dl_ccch_msg->message.present == DL_CCCH_MessageType_PR_c1) {

      if (UE_rrc_inst[ue_mod_idP].Info[eNB_index].State == RRC_SI_RECEIVED) {

          switch (dl_ccch_msg->message.choice.c1.present) {

          case DL_CCCH_MessageType__c1_PR_NOTHING:
            LOG_I(RRC, "[UE%d] Frame %d : Received PR_NOTHING on DL-CCCH-Message\n", ue_mod_idP, frameP);
            rval = 0;
            break;

          case DL_CCCH_MessageType__c1_PR_rrcConnectionReestablishment:
            LOG_D(RRC,
                "[MSC_MSG][FRAME %05d][MAC_UE][MOD %02d][][--- MAC_DATA_IND (rrcConnectionReestablishment ENB %d) --->][RRC_UE][MOD %02d][]\n", frameP, ue_mod_idP+NB_eNB_INST, eNB_index, ue_mod_idP+NB_eNB_INST);

            LOG_I(RRC,
                "[UE%d] Frame %d : Logical Channel DL-CCCH (SRB0), Received RRCConnectionReestablishment\n", ue_mod_idP, frameP);
            rval = 0;
            break;

          case DL_CCCH_MessageType__c1_PR_rrcConnectionReestablishmentReject:
            LOG_D(RRC,
                "[MSC_MSG][FRAME %05d][MAC_UE][MOD %02d][][--- MAC_DATA_IND (RRCConnectionReestablishmentReject ENB %d) --->][RRC_UE][MOD %02d][]\n", frameP, ue_mod_idP+NB_eNB_INST, eNB_index, ue_mod_idP+NB_eNB_INST);
            LOG_I(RRC,
                "[UE%d] Frame %d : Logical Channel DL-CCCH (SRB0), Received RRCConnectionReestablishmentReject\n", ue_mod_idP, frameP);
            rval = 0;
            break;

          case DL_CCCH_MessageType__c1_PR_rrcConnectionReject:
            LOG_D(RRC,
                "[MSC_MSG][FRAME %05d][MAC_UE][MOD %02d][][--- MAC_DATA_IND (rrcConnectionReject ENB %d) --->][RRC_UE][MOD %02d][]\n", frameP, ue_mod_idP+NB_eNB_INST, eNB_index, ue_mod_idP+NB_eNB_INST);

            LOG_I(RRC,
                "[UE%d] Frame %d : Logical Channel DL-CCCH (SRB0), Received RRCConnectionReject \n", ue_mod_idP, frameP);
            rval = 0;
            break;

          case DL_CCCH_MessageType__c1_PR_rrcConnectionSetup:
            LOG_D(RRC,
                "[MSC_MSG][FRAME %05d][MAC_UE][MOD %02d][][--- MAC_DATA_IND (rrcConnectionSetup ENB %d) --->][RRC_UE][MOD %02d][]\n", frameP, ue_mod_idP+NB_eNB_INST, eNB_index, ue_mod_idP+NB_eNB_INST);

            LOG_I(RRC,
                "[UE%d][RAPROC] Frame %d : Logical Channel DL-CCCH (SRB0), Received RRCConnectionSetup \n", ue_mod_idP, frameP);
            // Get configuration

            // Release T300 timer
            UE_rrc_inst[ue_mod_idP].Info[eNB_index].T300_active = 0;
            rrc_ue_process_radioResourceConfigDedicated(ue_mod_idP, frameP, eNB_index,
                &dl_ccch_msg->message.choice.c1.choice.rrcConnectionSetup.criticalExtensions.choice.c1.choice.rrcConnectionSetup_r8.radioResourceConfigDedicated);

            rrc_set_state (ue_mod_idP, RRC_STATE_CONNECTED);
            rrc_set_sub_state (ue_mod_idP, RRC_SUB_STATE_CONNECTED);
            rrc_ue_generate_RRCConnectionSetupComplete(ue_mod_idP, frameP, eNB_index, dl_ccch_msg->message.choice.c1.choice.rrcConnectionSetup.rrc_TransactionIdentifier);

            rval = 0;
            break;

          default:
            LOG_E(RRC, "[UE%d] Frame %d : Unknown message\n", ue_mod_idP, frameP);
            rval = -1;
            break;
          }
      }
  }

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_CCCH, VCD_FUNCTION_OUT);
  return rval;
}

int32_t rrc_ue_establish_srb1(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index, struct SRB_ToAddMod *SRB_config) {
  // add descriptor from RRC PDU

  uint8_t lchan_id = DCCH;

  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Active = 1;
  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Status = RADIO_CONFIG_OK;//RADIO CFG
  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Srb_info.Srb_id = 1;

  // copy default configuration for now
  //  memcpy(&UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Srb_info.Lchan_desc[0],&DCCH_LCHAN_DESC,LCHAN_DESC_SIZE);
  //  memcpy(&UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Srb_info.Lchan_desc[1],&DCCH_LCHAN_DESC,LCHAN_DESC_SIZE);


  LOG_I(RRC,"[UE %d], CONFIG_SRB1 %d corresponding to eNB_index %d\n", ue_mod_idP,lchan_id,eNB_index);

  //rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD, lchan_id,UNDEF_SECURITY_MODE);
  //  rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,lchan_id,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);

  //  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Srb_info.Tx_buffer.payload_size=DEFAULT_MEAS_IND_SIZE+1;


  return(0);
}

int32_t rrc_ue_establish_srb2(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index, struct SRB_ToAddMod *SRB_config) {
  // add descriptor from RRC PDU

  uint8_t lchan_id = DCCH1;

  UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Active = 1;
  UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Status = RADIO_CONFIG_OK;//RADIO CFG
  UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Srb_info.Srb_id = 2;

  // copy default configuration for now
  //  memcpy(&UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Srb_info.Lchan_desc[0],&DCCH_LCHAN_DESC,LCHAN_DESC_SIZE);
  //  memcpy(&UE_rrc_inst[ue_mod_idP].Srb2[eNB_index].Srb_info.Lchan_desc[1],&DCCH_LCHAN_DESC,LCHAN_DESC_SIZE);


  LOG_I(RRC,"[UE %d], CONFIG_SRB2 %d corresponding to eNB_index %d\n",ue_mod_idP,lchan_id,eNB_index);

  //rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD, lchan_id, UNDEF_SECURITY_MODE);
  //  rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,lchan_id,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);

  //  UE_rrc_inst[ue_mod_idP].Srb1[eNB_index].Srb_info.Tx_buffer.payload_size=DEFAULT_MEAS_IND_SIZE+1;


  return(0);
}

int32_t rrc_ue_establish_drb(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index,
    struct DRB_ToAddMod *DRB_config) { // add descriptor from RRC PDU
#ifdef NAS_NETLINK
  int oip_ifup=0,ip_addr_offset3=0,ip_addr_offset4=0;
#endif

  LOG_I(RRC,"[UE %d] Frame %d: processing RRCConnectionReconfiguration: reconfiguring DRB %ld/LCID %d\n",
      ue_mod_idP, frameP, DRB_config->drb_Identity, (int)*DRB_config->logicalChannelIdentity);
  /*
  rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD,
                             (eNB_index * NB_RB_MAX) + *DRB_config->logicalChannelIdentity, UNDEF_SECURITY_MODE);

 rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,
                    (eNB_index * NB_RB_MAX) + *DRB_config->logicalChannelIdentity,
                    RADIO_ACCESS_BEARER,Rlc_info_um);
   */
#ifdef NAS_NETLINK
#    ifdef OAI_EMU
  ip_addr_offset3 = oai_emulation.info.nb_enb_local;
  ip_addr_offset4 = NB_eNB_INST;
#    else
  ip_addr_offset3 = 0;
  ip_addr_offset4 = 8;
#    endif
#    if !defined(OAI_NW_DRIVER_TYPE_ETHERNET) && !defined(EXMIMO)
  LOG_I(OIP,"[UE %d] trying to bring up the OAI interface oai%d, IP 10.0.%d.%d\n", ue_mod_idP, ip_addr_offset3+ue_mod_idP,
      ip_addr_offset3+ue_mod_idP+1,ip_addr_offset4+ue_mod_idP+1);
  oip_ifup=nas_config(ip_addr_offset3+ue_mod_idP,   // interface_id
      ip_addr_offset3+ue_mod_idP+1, // third_octet
      ip_addr_offset4+ue_mod_idP+1); // fourth_octet
  if (oip_ifup == 0 ){ // interface is up --> send a config the DRB
#        ifdef OAI_EMU
      oai_emulation.info.oai_ifup[ue_mod_idP]=1;
#        endif
      LOG_I(OIP,"[UE %d] Config the oai%d to send/receive pkt on DRB %d to/from the protocol stack\n",
          ue_mod_idP,
          ip_addr_offset3+ue_mod_idP,
          (eNB_index * maxDRB) + DRB_config->drb_Identity);

      rb_conf_ipv4(0,//add
          ue_mod_idP,//cx align with the UE index
          ip_addr_offset3+ue_mod_idP,//inst num_enb+ue_index
          (eNB_index * maxDRB) + DRB_config->drb_Identity,//rb
          0,//dscp
          ipv4_address(ip_addr_offset3+ue_mod_idP+1,ip_addr_offset4+ue_mod_idP+1),//saddr
          ipv4_address(ip_addr_offset3+ue_mod_idP+1,eNB_index+1));//daddr
      LOG_D(RRC,"[UE %d] State = Attached (eNB %d)\n",ue_mod_idP,eNB_index);
  }
#    else
#        ifdef OAI_EMU
  oai_emulation.info.oai_ifup[ue_mod_idP]=1;
#        endif
#    endif
#endif

  return(0);
}


void  rrc_ue_process_measConfig(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index,MeasConfig_t *measConfig){

  // This is the procedure described in 36.331 Section 5.5.2.1
  int i;
  long ind;
  MeasObjectToAddMod_t *measObj;

  if (measConfig->measObjectToRemoveList != NULL) {
      for (i=0;i<measConfig->measObjectToRemoveList->list.count;i++) {
          ind   = *measConfig->measObjectToRemoveList->list.array[i];
          free(UE_rrc_inst[ue_mod_idP].MeasObj[eNB_index][ind-1]);
      }
  }
  if (measConfig->measObjectToAddModList != NULL) {
      LOG_D(RRC,"Measurement Object List is present\n");
      for (i=0;i<measConfig->measObjectToAddModList->list.count;i++) {
          measObj = measConfig->measObjectToAddModList->list.array[i];
          ind   = measConfig->measObjectToAddModList->list.array[i]->measObjectId;

          if (UE_rrc_inst[ue_mod_idP].MeasObj[eNB_index][ind-1]) {
              LOG_D(RRC,"Modifying measurement object %d\n",ind);
              memcpy((char*)UE_rrc_inst[ue_mod_idP].MeasObj[eNB_index][ind-1],
                  (char*)measObj,
                  sizeof(MeasObjectToAddMod_t));
          }
          else {
              LOG_I(RRC,"Adding measurement object %d\n",ind);
              if (measObj->measObject.present == MeasObjectToAddMod__measObject_PR_measObjectEUTRA) {
                  LOG_I(RRC,"EUTRA Measurement : carrierFreq %d, allowedMeasBandwidth %d,presenceAntennaPort1 %d, neighCellConfig %d\n",
                      measObj->measObject.choice.measObjectEUTRA.carrierFreq,
                      measObj->measObject.choice.measObjectEUTRA.allowedMeasBandwidth,
                      measObj->measObject.choice.measObjectEUTRA.presenceAntennaPort1,
                      measObj->measObject.choice.measObjectEUTRA.neighCellConfig.buf[0]);
                  UE_rrc_inst[ue_mod_idP].MeasObj[eNB_index][ind-1]=measObj;
              }
          }
      }
      rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
          (RadioResourceConfigCommonSIB_t *)NULL,
          (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
			 (SCellToAddMod_r10_t *)NULL,
	  //struct PhysicalConfigDedicatedSCell_r10 *physicalConfigDedicatedSCell_r10,
#endif
          UE_rrc_inst[ue_mod_idP].MeasObj[eNB_index],
          (MAC_MainConfig_t *)NULL,
          0,
          (struct LogicalChannelConfig *)NULL,
          (MeasGapConfig_t *)NULL,
          (TDD_Config_t *)NULL,
          (MobilityControlInfo_t *)NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL
#ifdef Rel10
          ,
          0,
          (MBSFN_AreaInfoList_r9_t *)NULL,
          (PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
          ,
          0,
          0
#endif
      );
  }
  if (measConfig->reportConfigToRemoveList != NULL) {
      for (i=0;i<measConfig->reportConfigToRemoveList->list.count;i++) {
          ind   = *measConfig->reportConfigToRemoveList->list.array[i];
          free(UE_rrc_inst[ue_mod_idP].ReportConfig[eNB_index][ind-1]);
      }
  }
  if (measConfig->reportConfigToAddModList != NULL) {
      LOG_I(RRC,"Report Configuration List is present\n");
      for (i=0;i<measConfig->reportConfigToAddModList->list.count;i++) {
          ind   = measConfig->reportConfigToAddModList->list.array[i]->reportConfigId;
          if (UE_rrc_inst[ue_mod_idP].ReportConfig[eNB_index][ind-1]) {
              LOG_I(RRC,"Modifying Report Configuration %d\n",ind-1);
              memcpy((char*)UE_rrc_inst[ue_mod_idP].ReportConfig[eNB_index][ind-1],
                  (char*)measConfig->reportConfigToAddModList->list.array[i],
                  sizeof(ReportConfigToAddMod_t));
          }
          else {
              LOG_D(RRC,"Adding Report Configuration %d %p \n",ind-1,measConfig->reportConfigToAddModList->list.array[i]);
              UE_rrc_inst[ue_mod_idP].ReportConfig[eNB_index][ind-1] = measConfig->reportConfigToAddModList->list.array[i];
          }
      }
  }

  if (measConfig->quantityConfig != NULL) {
      if (UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]) {
          LOG_D(RRC,"Modifying Quantity Configuration \n");
          memcpy((char*)UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index],
              (char*)measConfig->quantityConfig,
              sizeof(QuantityConfig_t));
      }
      else {
          LOG_D(RRC,"Adding Quantity configuration\n");
          UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index] = measConfig->quantityConfig;
      }
  }

  if (measConfig->measIdToRemoveList != NULL) {
      for (i=0;i<measConfig->measIdToRemoveList->list.count;i++) {
          ind   = *measConfig->measIdToRemoveList->list.array[i];
          free(UE_rrc_inst[ue_mod_idP].MeasId[eNB_index][ind-1]);
      }
  }

  if (measConfig->measIdToAddModList != NULL) {
      for (i=0;i<measConfig->measIdToAddModList->list.count;i++) {
          ind   = measConfig->measIdToAddModList->list.array[i]->measId;
          if (UE_rrc_inst[ue_mod_idP].MeasId[eNB_index][ind-1]) {
              LOG_D(RRC,"Modifying Measurement ID %d\n",ind-1);
              memcpy((char*)UE_rrc_inst[ue_mod_idP].MeasId[eNB_index][ind-1],
                  (char*)measConfig->measIdToAddModList->list.array[i],
                  sizeof(MeasIdToAddMod_t));
          }
          else {
              LOG_D(RRC,"Adding Measurement ID %d %p\n",ind-1,measConfig->measIdToAddModList->list.array[i]);
              UE_rrc_inst[ue_mod_idP].MeasId[eNB_index][ind-1] = measConfig->measIdToAddModList->list.array[i];
          }
      }
  }

  if (measConfig->measGapConfig !=NULL) {
      if (UE_rrc_inst[ue_mod_idP].measGapConfig[eNB_index]) {
          memcpy((char*)UE_rrc_inst[ue_mod_idP].measGapConfig[eNB_index],
              (char*)measConfig->measGapConfig,
              sizeof(MeasGapConfig_t));
      }
      else {
          UE_rrc_inst[ue_mod_idP].measGapConfig[eNB_index] = measConfig->measGapConfig;
      }
  }

  if (measConfig->quantityConfig != NULL) {
      if (UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]) {
          LOG_I(RRC,"Modifying Quantity Configuration \n");
          memcpy((char*)UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index],
              (char*)measConfig->quantityConfig,
              sizeof(QuantityConfig_t));
      }
      else {
          LOG_I(RRC,"Adding Quantity configuration\n");
          UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index] = measConfig->quantityConfig;
      }

      UE_rrc_inst[ue_mod_idP].filter_coeff_rsrp = 1./pow(2,(*UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]->quantityConfigEUTRA->filterCoefficientRSRP)/4);
      UE_rrc_inst[ue_mod_idP].filter_coeff_rsrq = 1./pow(2,(*UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]->quantityConfigEUTRA->filterCoefficientRSRQ)/4);

      LOG_I(RRC,"[UE %d] set rsrp-coeff for eNB %d: %d rsrq-coeff: %d rsrp_factor: %f rsrq_factor: %f \n",
          ue_mod_idP, eNB_index, // UE_rrc_inst[ue_mod_idP].Info[eNB_index].UE_index,
          *UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]->quantityConfigEUTRA->filterCoefficientRSRP,
          *UE_rrc_inst[ue_mod_idP].QuantityConfig[eNB_index]->quantityConfigEUTRA->filterCoefficientRSRQ,
          UE_rrc_inst[ue_mod_idP].filter_coeff_rsrp, UE_rrc_inst[ue_mod_idP].filter_coeff_rsrp,
          UE_rrc_inst[ue_mod_idP].filter_coeff_rsrp, UE_rrc_inst[ue_mod_idP].filter_coeff_rsrq);
  }

  if (measConfig->s_Measure != NULL) {
      UE_rrc_inst[ue_mod_idP].s_measure = *measConfig->s_Measure;
  }

  if (measConfig->speedStatePars != NULL) {
      if (UE_rrc_inst[ue_mod_idP].speedStatePars)
        memcpy((char*)UE_rrc_inst[ue_mod_idP].speedStatePars,(char*)measConfig->speedStatePars,sizeof(struct MeasConfig__speedStatePars));
      else
        UE_rrc_inst[ue_mod_idP].speedStatePars = measConfig->speedStatePars;
      LOG_I(RRC,"[UE %d] Configuring mobility optimization params for UE %d \n",
          ue_mod_idP,UE_rrc_inst[ue_mod_idP].Info[0].UE_index);
  }
}

void    
rrc_ue_process_radioResourceConfigDedicated(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index,
    RadioResourceConfigDedicated_t *radioResourceConfigDedicated) {

  long SRB_id,DRB_id;
  int i,cnt;
  LogicalChannelConfig_t *SRB1_logicalChannelConfig,*SRB2_logicalChannelConfig;
#ifdef CBA  
  uint8_t cba_found = 0;
  uint16_t cba_RNTI;
#endif 

  // Save physicalConfigDedicated if present
  if (radioResourceConfigDedicated->physicalConfigDedicated) {
      if (UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index]) {
          memcpy((char*)UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index],(char*)radioResourceConfigDedicated->physicalConfigDedicated,
              sizeof(struct PhysicalConfigDedicated));

      }
      else {
          UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index] = radioResourceConfigDedicated->physicalConfigDedicated;
      }
  }
  // Apply macMainConfig if present
  if (radioResourceConfigDedicated->mac_MainConfig) {
      if (radioResourceConfigDedicated->mac_MainConfig->present == RadioResourceConfigDedicated__mac_MainConfig_PR_explicitValue) {
          if (UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index]) {
              memcpy((char*)UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index],(char*)&radioResourceConfigDedicated->mac_MainConfig->choice.explicitValue,
                  sizeof(MAC_MainConfig_t));
          }
          else
            UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index] = &radioResourceConfigDedicated->mac_MainConfig->choice.explicitValue;
      }
  }

  // Apply spsConfig if present
  if (radioResourceConfigDedicated->sps_Config) {
      if (UE_rrc_inst[ue_mod_idP].sps_Config[eNB_index]) {
          memcpy(UE_rrc_inst[ue_mod_idP].sps_Config[eNB_index],radioResourceConfigDedicated->sps_Config,
              sizeof(struct SPS_Config));
      }
      else {
          UE_rrc_inst[ue_mod_idP].sps_Config[eNB_index] = radioResourceConfigDedicated->sps_Config;
      }
  }
#ifdef CBA
  if (radioResourceConfigDedicated->cba_RNTI_vlola) {
      cba_RNTI = (uint16_t) (((radioResourceConfigDedicated->cba_RNTI_vlola->buf[1]&0xff) << 8) |
          (radioResourceConfigDedicated->cba_RNTI_vlola->buf[0]&0xff));
      for (i=0 ; i< NUM_MAX_CBA_GROUP; i++) {
          if (UE_rrc_inst[ue_mod_idP].cba_rnti[i] == cba_RNTI ) {
              cba_found=1;
              break;
          } else if (UE_rrc_inst[ue_mod_idP].cba_rnti[i] == 0 )
            break;
      }
      if (cba_found==0) {
          UE_rrc_inst[ue_mod_idP].num_active_cba_groups++;
          UE_rrc_inst[ue_mod_idP].cba_rnti[i]=cba_RNTI;
          LOG_D(RRC, "[UE %d] Frame %d: radioResourceConfigDedicated reveived CBA_RNTI = %x for group %d from eNB %d \n",
              ue_mod_idP,frameP, UE_rrc_inst[ue_mod_idP].cba_rnti[i], i, eNB_index);
      }
  }
#endif 
  // Establish SRBs if present
  // loop through SRBToAddModList
  if (radioResourceConfigDedicated->srb_ToAddModList) {
      uint8_t *kRRCenc = NULL;
      uint8_t *kRRCint = NULL;

#if defined(ENABLE_SECURITY)
      derive_key_rrc_enc(UE_rrc_inst[ue_mod_idP].ciphering_algorithm,
          UE_rrc_inst[ue_mod_idP].kenb, &kRRCenc);
      derive_key_rrc_int(UE_rrc_inst[ue_mod_idP].integrity_algorithm,
          UE_rrc_inst[ue_mod_idP].kenb, &kRRCint);
#endif

// Refresh SRBs
      rrc_pdcp_config_asn1_req(eNB_index,ue_mod_idP,frameP,0,
          radioResourceConfigDedicated->srb_ToAddModList,
          (DRB_ToAddModList_t*)NULL,
          (DRB_ToReleaseList_t*)NULL,
          UE_rrc_inst[ue_mod_idP].ciphering_algorithm |
          (UE_rrc_inst[ue_mod_idP].integrity_algorithm << 4),
          kRRCenc,
          kRRCint,
          NULL
#ifdef Rel10
,(PMCH_InfoList_r9_t *)NULL
#endif
      );

      // Refresh SRBs
      rrc_rlc_config_asn1_req(eNB_index,ue_mod_idP,frameP,0,
          radioResourceConfigDedicated->srb_ToAddModList,
          (DRB_ToAddModList_t*)NULL,
          (DRB_ToReleaseList_t*)NULL
#ifdef Rel10
          ,(PMCH_InfoList_r9_t *)NULL
#endif
      );

#ifdef ENABLE_RAL
	// first msg that includes srb config	
	UE_rrc_inst[ue_mod_idP].num_srb=radioResourceConfigDedicated->srb_ToAddModList->list.count;
#endif 

      for (cnt=0;cnt<radioResourceConfigDedicated->srb_ToAddModList->list.count;cnt++) {
	//	connection_reestablishment_ind.num_srb+=1;
          SRB_id = radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt]->srb_Identity;
          LOG_D(RRC,"[UE %d]: Frame %d SRB config cnt %d (SRB%ld)\n",ue_mod_idP,frameP,cnt,SRB_id);
          if (SRB_id == 1) {
              if (UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index]) {
                  memcpy(UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index],radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt],
                      sizeof(struct SRB_ToAddMod));
              }
              else {
                  UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index] = radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt];

                  rrc_ue_establish_srb1(ue_mod_idP,frameP,eNB_index,radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt]);
                  if (UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index]->logicalChannelConfig) {
                      if (UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index]->logicalChannelConfig->present == SRB_ToAddMod__logicalChannelConfig_PR_explicitValue) {
                          SRB1_logicalChannelConfig = &UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index]->logicalChannelConfig->choice.explicitValue;
                      }
                      else {
                          SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
                      }
                  }
                  else {
                      SRB1_logicalChannelConfig = &SRB1_logicalChannelConfig_defaultValue;
                  }

                  LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ  (SRB1 eNB %d) --->][MAC_UE][MOD %02d][]\n",
                      frameP, ue_mod_idP, eNB_index, ue_mod_idP);
                  rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
                      (RadioResourceConfigCommonSIB_t *)NULL,
                      UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index],
#ifdef Rel10
				     (SCellToAddMod_r10_t *)NULL,
		       //struct PhysicalConfigDedicatedSCell_r10 *physicalConfigDedicatedSCell_r10,
#endif
                      (MeasObjectToAddMod_t **)NULL,
                      UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index],
                      1,
                      SRB1_logicalChannelConfig,
                      (MeasGapConfig_t *)NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
#ifdef Rel10	       
                      ,
                      0,
                      (MBSFN_AreaInfoList_r9_t *)NULL,
                      (PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
                      ,
                      0,
                      0
#endif
                  );
              }
          }
          else {
              if (UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index]) {
                  memcpy(UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index],radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt],
                      sizeof(struct SRB_ToAddMod));
              }
              else {

                  UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index] = radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt];

                  rrc_ue_establish_srb2(ue_mod_idP,frameP,eNB_index,radioResourceConfigDedicated->srb_ToAddModList->list.array[cnt]);
                  if (UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index]->logicalChannelConfig) {
                      if (UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index]->logicalChannelConfig->present == SRB_ToAddMod__logicalChannelConfig_PR_explicitValue){
                          LOG_I(RRC,"Applying Explicit SRB2 logicalChannelConfig\n");
                          SRB2_logicalChannelConfig = &UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index]->logicalChannelConfig->choice.explicitValue;
                      }
                      else {
                          LOG_I(RRC,"Applying default SRB2 logicalChannelConfig\n");
                          SRB2_logicalChannelConfig = &SRB2_logicalChannelConfig_defaultValue;
                      }
                  }
                  else {
                      SRB2_logicalChannelConfig = &SRB2_logicalChannelConfig_defaultValue;
                  }

                  LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ  (SRB2 eNB %d) --->][MAC_UE][MOD %02d][]\n",
                      frameP, ue_mod_idP, eNB_index, ue_mod_idP);
                  rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
                      (RadioResourceConfigCommonSIB_t *)NULL,
                      UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index],
#ifdef Rel10
				     (SCellToAddMod_r10_t *)NULL,
		       //struct PhysicalConfigDedicatedSCell_r10 *physicalConfigDedicatedSCell_r10,
#endif
                      (MeasObjectToAddMod_t **)NULL,
                      UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index],
                      2,
                      SRB2_logicalChannelConfig,
                      UE_rrc_inst[ue_mod_idP].measGapConfig[eNB_index],
                      (TDD_Config_t *)NULL,
                      (MobilityControlInfo_t *)NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
#ifdef Rel10
,
0,
(MBSFN_AreaInfoList_r9_t *)NULL,
(PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
,
0,
0
#endif
                  );
              }
          }
      }
  }

  // Establish DRBs if present
  if (radioResourceConfigDedicated->drb_ToAddModList) {
      uint8_t *kUPenc = NULL;

#if defined(ENABLE_SECURITY)
      derive_key_up_enc(UE_rrc_inst[ue_mod_idP].integrity_algorithm,
          UE_rrc_inst[ue_mod_idP].kenb, &kUPenc);
#endif

      // Refresh DRBs
      rrc_pdcp_config_asn1_req(eNB_index, ue_mod_idP,frameP,0,
          (SRB_ToAddModList_t*)NULL,
          radioResourceConfigDedicated->drb_ToAddModList,
          (DRB_ToReleaseList_t*)NULL,
          UE_rrc_inst[ue_mod_idP].ciphering_algorithm |
          (UE_rrc_inst[ue_mod_idP].integrity_algorithm << 4),
          NULL,
          NULL,
          kUPenc
#ifdef Rel10
,(PMCH_InfoList_r9_t *)NULL
#endif
      );

      // Refresh DRBs
      rrc_rlc_config_asn1_req(eNB_index,ue_mod_idP,frameP,0,
          (SRB_ToAddModList_t*)NULL,
          radioResourceConfigDedicated->drb_ToAddModList,
          (DRB_ToReleaseList_t*)NULL
#ifdef Rel10
          ,(PMCH_InfoList_r9_t *)NULL
#endif
      );
      for (i=0;i<radioResourceConfigDedicated->drb_ToAddModList->list.count;i++) {
          DRB_id   = radioResourceConfigDedicated->drb_ToAddModList->list.array[i]->drb_Identity-1;
          if (UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][DRB_id]) {
              memcpy(UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][DRB_id],radioResourceConfigDedicated->drb_ToAddModList->list.array[i],
                  sizeof(struct DRB_ToAddMod));
          }
          else {
              UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][DRB_id] = radioResourceConfigDedicated->drb_ToAddModList->list.array[i];

              rrc_ue_establish_drb(ue_mod_idP,frameP,eNB_index,radioResourceConfigDedicated->drb_ToAddModList->list.array[i]);
              // MAC/PHY Configuration
              LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (DRB %d eNB %d) --->][MAC_UE][MOD %02d][]\n",
                  frameP, ue_mod_idP, radioResourceConfigDedicated->drb_ToAddModList->list.array[i]->drb_Identity, eNB_index, ue_mod_idP);
              rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
                  (RadioResourceConfigCommonSIB_t *)NULL,
                  UE_rrc_inst[ue_mod_idP].physicalConfigDedicated[eNB_index],
#ifdef Rel10
				 (SCellToAddMod_r10_t *)NULL,
		       //struct PhysicalConfigDedicatedSCell_r10 *physicalConfigDedicatedSCell_r10,
#endif
                  (MeasObjectToAddMod_t **)NULL,
                  UE_rrc_inst[ue_mod_idP].mac_MainConfig[eNB_index],
                  *UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][DRB_id]->logicalChannelIdentity,
                  UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][DRB_id]->logicalChannelConfig,
                  UE_rrc_inst[ue_mod_idP].measGapConfig[eNB_index],
                  (TDD_Config_t*)NULL,
                  (MobilityControlInfo_t *)NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL
#ifdef Rel10
,
0,
(MBSFN_AreaInfoList_r9_t *)NULL,
(PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
,
UE_rrc_inst[ue_mod_idP].num_active_cba_groups, //
UE_rrc_inst[ue_mod_idP].cba_rnti[0]
#endif
              );

          }
      }
  }

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].State = RRC_CONNECTED;
  LOG_I(RRC,"[UE %d] State = RRC_CONNECTED (eNB %d)\n",ue_mod_idP,eNB_index);


}

void rrc_ue_process_securityModeCommand(uint8_t ue_mod_idP, frame_t frameP,SecurityModeCommand_t *securityModeCommand,uint8_t eNB_index) {

  asn_enc_rval_t enc_rval;

  UL_DCCH_Message_t ul_dcch_msg;
  // SecurityModeCommand_t SecurityModeCommand;
  uint8_t buffer[200];
  int i, securityMode;

  LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1 (DL-DCCH), Processing securityModeCommand (eNB %d)\n",
      ue_mod_idP,frameP,eNB_index);

  switch (securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.cipheringAlgorithm){
  case SecurityAlgorithmConfig__cipheringAlgorithm_eea0:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea0\n",ue_mod_idP);
    securityMode= SecurityAlgorithmConfig__cipheringAlgorithm_eea0;
    break;
  case SecurityAlgorithmConfig__cipheringAlgorithm_eea1:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea1\n",ue_mod_idP);
    securityMode= SecurityAlgorithmConfig__cipheringAlgorithm_eea1;
    break;
  case SecurityAlgorithmConfig__cipheringAlgorithm_eea2:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea2\n",ue_mod_idP);
    securityMode = SecurityAlgorithmConfig__cipheringAlgorithm_eea2;
    break;
  default:
    LOG_I(RRC,"[UE %d] Security algorithm is set to none\n",ue_mod_idP);
    securityMode = SecurityAlgorithmConfig__cipheringAlgorithm_spare1;
    break;
  }
  switch (securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.integrityProtAlgorithm){
  case SecurityAlgorithmConfig__integrityProtAlgorithm_eia1:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to eia1\n",ue_mod_idP);
    securityMode |= 1 << 5;
    break;
  case SecurityAlgorithmConfig__integrityProtAlgorithm_eia2:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to eia2\n",ue_mod_idP);
    securityMode |= 1 << 6;
    break;
  default:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to none\n",ue_mod_idP);
    securityMode |= 0x70 ;
    break;
  }
  LOG_D(RRC,"[UE %d] security mode is %x \n",ue_mod_idP, securityMode);

  /* Store the parameters received */
  UE_rrc_inst[ue_mod_idP].ciphering_algorithm = securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.cipheringAlgorithm;
  UE_rrc_inst[ue_mod_idP].integrity_algorithm = securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.integrityProtAlgorithm;

  memset((void *)&ul_dcch_msg,0,sizeof(UL_DCCH_Message_t));
  //memset((void *)&SecurityModeCommand,0,sizeof(SecurityModeCommand_t));

  ul_dcch_msg.message.present           = UL_DCCH_MessageType_PR_c1;
  if (securityMode >= NO_SECURITY_MODE)
    ul_dcch_msg.message.choice.c1.present = UL_DCCH_MessageType__c1_PR_securityModeComplete;
  else 
    ul_dcch_msg.message.choice.c1.present = UL_DCCH_MessageType__c1_PR_securityModeFailure;

  if (securityModeCommand->criticalExtensions.present == SecurityModeCommand__criticalExtensions_PR_c1) {
      if (securityModeCommand->criticalExtensions.choice.c1.present == SecurityModeCommand__criticalExtensions__c1_PR_securityModeCommand_r8) {

          ul_dcch_msg.message.choice.c1.choice.securityModeComplete.rrc_TransactionIdentifier = securityModeCommand->rrc_TransactionIdentifier;
          ul_dcch_msg.message.choice.c1.choice.securityModeComplete.criticalExtensions.present = SecurityModeCommand__criticalExtensions_PR_c1;
          ul_dcch_msg.message.choice.c1.choice.securityModeComplete.criticalExtensions.choice.securityModeComplete_r8.nonCriticalExtension =NULL;

          LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1 (DL-DCCH), encoding securityModeComplete (eNB %d)\n",
              ue_mod_idP,frameP,eNB_index);

          enc_rval = uper_encode_to_buffer(&asn_DEF_UL_DCCH_Message,
              (void*)&ul_dcch_msg,
              buffer,
              100);
          AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %jd)!\n",
              enc_rval.failed_type->name, enc_rval.encoded);

#ifdef XER_PRINT
          xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, (void*)&ul_dcch_msg);
#endif	  

#if defined(ENABLE_ITTI)
# if !defined(DISABLE_XER_SPRINT)
          {
            char        message_string[20000];
            size_t      message_string_size;

            if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_DCCH_Message, (void *) &ul_dcch_msg)) > 0)
              {
                MessageDef *msg_p;

                msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_UL_DCCH, message_string_size + sizeof (IttiMsgText));
                msg_p->ittiMsg.rrc_ul_dcch.size = message_string_size;
                memcpy(&msg_p->ittiMsg.rrc_ul_dcch.text, message_string, message_string_size);

                itti_send_msg_to_task(TASK_UNKNOWN, NB_eNB_INST + ue_mod_idP, msg_p);
              }
          }
# endif
#endif

#ifdef USER_MODE
          LOG_D(RRC, "securityModeComplete Encoded %d bits (%d bytes)\n", enc_rval.encoded, (enc_rval.encoded+7)/8);
#endif
          for (i = 0; i < (enc_rval.encoded + 7) / 8; i++)
            LOG_T(RRC, "%02x.", buffer[i]);
          LOG_T(RRC, "\n");
          pdcp_rrc_data_req (eNB_index, ue_mod_idP, frameP, 0, DCCH, rrc_mui++, 0, (enc_rval.encoded + 7) / 8, buffer, 1);
      }
  }

}
void rrc_ue_process_ueCapabilityEnquiry(uint8_t ue_mod_idP, frame_t frameP,UECapabilityEnquiry_t *UECapabilityEnquiry,uint8_t eNB_index) {

  asn_enc_rval_t enc_rval;

  UL_DCCH_Message_t ul_dcch_msg;


  UE_CapabilityRAT_Container_t ue_CapabilityRAT_Container;

  uint8_t buffer[200];
  int i;

  LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1 (DL-DCCH), Processing UECapabilityEnquiry (eNB %d)\n",
      ue_mod_idP,frameP,eNB_index);


  memset((void *)&ul_dcch_msg,0,sizeof(UL_DCCH_Message_t));
  memset((void *)&ue_CapabilityRAT_Container,0,sizeof(UE_CapabilityRAT_Container_t));

  ul_dcch_msg.message.present           = UL_DCCH_MessageType_PR_c1;
  ul_dcch_msg.message.choice.c1.present = UL_DCCH_MessageType__c1_PR_ueCapabilityInformation;
  ul_dcch_msg.message.choice.c1.choice.ueCapabilityInformation.rrc_TransactionIdentifier = UECapabilityEnquiry->rrc_TransactionIdentifier;

  ue_CapabilityRAT_Container.rat_Type = RAT_Type_eutra;
  OCTET_STRING_fromBuf(&ue_CapabilityRAT_Container.ueCapabilityRAT_Container,
      (const char*)UE_rrc_inst[ue_mod_idP].UECapability,
      UE_rrc_inst[ue_mod_idP].UECapability_size);
  //  ue_CapabilityRAT_Container.ueCapabilityRAT_Container.buf  = UE_rrc_inst[ue_mod_idP].UECapability;
  // ue_CapabilityRAT_Container.ueCapabilityRAT_Container.size = UE_rrc_inst[ue_mod_idP].UECapability_size;


  if (UECapabilityEnquiry->criticalExtensions.present == UECapabilityEnquiry__criticalExtensions_PR_c1) {
      if (UECapabilityEnquiry->criticalExtensions.choice.c1.present == UECapabilityEnquiry__criticalExtensions__c1_PR_ueCapabilityEnquiry_r8) {
          ul_dcch_msg.message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.present           = UECapabilityInformation__criticalExtensions_PR_c1;
          ul_dcch_msg.message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.choice.c1.present = UECapabilityInformation__criticalExtensions__c1_PR_ueCapabilityInformation_r8;
          ul_dcch_msg.message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList.list.count=0;

          for (i=0;i<UECapabilityEnquiry->criticalExtensions.choice.c1.choice.ueCapabilityEnquiry_r8.ue_CapabilityRequest.list.count;i++) {

              if (*UECapabilityEnquiry->criticalExtensions.choice.c1.choice.ueCapabilityEnquiry_r8.ue_CapabilityRequest.list.array[i]
                                                                                                                                   == RAT_Type_eutra) {
                  ASN_SEQUENCE_ADD(
                      &ul_dcch_msg.message.choice.c1.choice.ueCapabilityInformation.criticalExtensions.choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList.list,
                      &ue_CapabilityRAT_Container);

                  enc_rval = uper_encode_to_buffer(&asn_DEF_UL_DCCH_Message, (void*) &ul_dcch_msg, buffer, 100);
                  AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %jd)!\n",
                      enc_rval.failed_type->name, enc_rval.encoded);

#ifdef XER_PRINT
                  xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, (void*)&ul_dcch_msg);
#endif

#if defined(ENABLE_ITTI)
# if !defined(DISABLE_XER_SPRINT)
                  {
                    char        message_string[20000];
                    size_t      message_string_size;

                    if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_DCCH_Message, (void *) &ul_dcch_msg)) > 0)
                      {
                        MessageDef *msg_p;

                        msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_UL_DCCH, message_string_size + sizeof (IttiMsgText));
                        msg_p->ittiMsg.rrc_ul_dcch.size = message_string_size;
                        memcpy(&msg_p->ittiMsg.rrc_ul_dcch.text, message_string, message_string_size);

                        itti_send_msg_to_task(TASK_UNKNOWN, NB_eNB_INST + ue_mod_idP, msg_p);
                      }
                  }
# endif
#endif

#ifdef USER_MODE
                  LOG_D(RRC,"UECapabilityInformation Encoded %d bits (%d bytes)\n",enc_rval.encoded,(enc_rval.encoded+7)/8);
#endif
                  for (i = 0; i < (enc_rval.encoded + 7) / 8; i++)
                    LOG_T(RRC, "%02x.", buffer[i]);
                  LOG_T(RRC, "\n");
                  pdcp_rrc_data_req (eNB_index, ue_mod_idP, frameP, 0, DCCH, rrc_mui++, 0, (enc_rval.encoded + 7) / 8, buffer, 1);
              }
          }
      }
  }
}


void rrc_ue_process_rrcConnectionReconfiguration(module_id_t ue_mod_idP, frame_t frameP,
    RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,
    uint8_t eNB_index) {

  LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1 (DL-DCCH), Processing RRCConnectionReconfiguration (eNB %d)\n",
      ue_mod_idP,frameP,eNB_index);
  if (rrcConnectionReconfiguration->criticalExtensions.present == RRCConnectionReconfiguration__criticalExtensions_PR_c1) {
      if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.present ==
          RRCConnectionReconfiguration__criticalExtensions__c1_PR_rrcConnectionReconfiguration_r8) {
          RRCConnectionReconfiguration_r8_IEs_t *rrcConnectionReconfiguration_r8 = &rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8;

          if (rrcConnectionReconfiguration_r8->mobilityControlInfo) {
              LOG_I(RRC,"Mobility Control Information is present\n");
              rrc_ue_process_mobilityControlInfo(ue_mod_idP, eNB_index, frameP, rrcConnectionReconfiguration_r8->mobilityControlInfo);
          }
          if (rrcConnectionReconfiguration_r8->measConfig != NULL) {
              LOG_I(RRC,"Measurement Configuration is present\n");
              rrc_ue_process_measConfig(ue_mod_idP,frameP, eNB_index,
                  rrcConnectionReconfiguration_r8->measConfig);
          }
          if (rrcConnectionReconfiguration_r8->radioResourceConfigDedicated) {
              LOG_I(RRC,"Radio Resource Configuration is present\n");
              rrc_ue_process_radioResourceConfigDedicated(ue_mod_idP,frameP,eNB_index, rrcConnectionReconfiguration_r8->radioResourceConfigDedicated);
          }

#if defined(ENABLE_ITTI)
          /* Check if there is dedicated NAS information to forward to NAS */
          if (rrcConnectionReconfiguration_r8->dedicatedInfoNASList != NULL) {
              int list_count;
              uint32_t pdu_length;
              uint8_t *pdu_buffer;
              MessageDef *msg_p;

              for (list_count = 0; list_count < rrcConnectionReconfiguration_r8->dedicatedInfoNASList->list.count; list_count++) {
                  pdu_length = rrcConnectionReconfiguration_r8->dedicatedInfoNASList->list.array[list_count]->size;
                  pdu_buffer = rrcConnectionReconfiguration_r8->dedicatedInfoNASList->list.array[list_count]->buf;

                  msg_p = itti_alloc_new_message(TASK_RRC_UE, NAS_CONN_ESTABLI_CNF);
                  NAS_CONN_ESTABLI_CNF(msg_p).errCode = AS_SUCCESS;
                  NAS_CONN_ESTABLI_CNF(msg_p).nasMsg.length = pdu_length;
                  NAS_CONN_ESTABLI_CNF(msg_p).nasMsg.data = pdu_buffer;

                  itti_send_msg_to_task(TASK_NAS_UE, ue_mod_idP, msg_p);
              }

              free (rrcConnectionReconfiguration_r8->dedicatedInfoNASList);
          }
#ifdef ENABLE_RAL
          {
            MessageDef                                 *message_ral_p = NULL;
            rrc_ral_connection_reestablishment_ind_t    connection_reestablishment_ind;
            int                                         i;

            message_ral_p = itti_alloc_new_message (TASK_RRC_UE, RRC_RAL_CONNECTION_REESTABLISHMENT_IND);
            memset(&connection_reestablishment_ind, 0, sizeof(rrc_ral_connection_reestablishment_ind_t));
            // TO DO ral_si_ind.plmn_id        = 0;
            connection_reestablishment_ind.ue_id            = ue_mod_idP;
            if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList != NULL) {
                connection_reestablishment_ind.num_drb      = rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count;

                for (i=0;(i<rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count) && (i < maxDRB);i++) {
                    // why minus 1 in RRC code for drb_identity ?
                    connection_reestablishment_ind.drb_id[i]   = rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.array[i]->drb_Identity;
                }
            } else {
                connection_reestablishment_ind.num_drb      = 0;
            }
            if (rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList != NULL) {
                connection_reestablishment_ind.num_srb      = rrcConnectionReconfiguration->criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList->list.count + UE_rrc_inst[ue_mod_idP].num_srb;
		
            } else {
 	              connection_reestablishment_ind.num_srb      += UE_rrc_inst[ue_mod_idP].num_srb;
            }
	   if (connection_reestablishment_ind.num_srb > 2) // fixme: only 2 srbs can exist, adjust the value
		connection_reestablishment_ind.num_srb =2;
		
            memcpy (&message_ral_p->ittiMsg, (void *) &connection_reestablishment_ind, sizeof(rrc_ral_connection_reestablishment_ind_t));
            //#warning "ue_mod_idP ? for instance ? => YES"
            LOG_I(RRC, "Sending RRC_RAL_CONNECTION_REESTABLISHMENT_IND to mRAL\n");
            itti_send_msg_to_task (TASK_RAL_UE, ue_mod_idP + NB_eNB_INST, message_ral_p);
          }
#endif
#endif
      } // c1 present
  } // critical extensions present
}

/* 36.331, 5.3.5.4      Reception of an RRCConnectionReconfiguration including the mobilityControlInfo by the UE (handover) */
void rrc_ue_process_mobilityControlInfo(uint8_t eNB_index, uint8_t UE_id, frame_t frameP, struct MobilityControlInfo *mobilityControlInfo)
{
  module_id_t ue_mod_idP = UE_id;
  /*
  DRB_ToReleaseList_t*  drb2release_list;
  DRB_Identity_t *lcid;
   */
  LOG_N(RRC,"Note: This function needs some updates\n");
  if(UE_rrc_inst[UE_id].Info[eNB_index].T310_active == 1)
    UE_rrc_inst[UE_id].Info[eNB_index].T310_active = 0;
  UE_rrc_inst[UE_id].Info[eNB_index].T304_active = 1;
  UE_rrc_inst[UE_id].Info[eNB_index].T304_cnt = T304[mobilityControlInfo->t304];

  /*
  drb2release_list = CALLOC (1, sizeof (*drb2release_list));
  lcid= CALLOC (1, sizeof (DRB_Identity_t)); // long
  for (*lcid=0;*lcid<NB_RB_MAX;*lcid++)
  {
    ASN_SEQUENCE_ADD (&(drb2release_list)->list,lcid);
  }
   */

  //Removing SRB1 and SRB2 and DRB0
  LOG_N(RRC,"[UE %d] : Update needed for rrc_pdcp_config_req (deprecated) and rrc_rlc_config_req commands(deprecated)\n", UE_id);
  rrc_pdcp_config_req (eNB_index, UE_id, frameP, ENB_FLAG_NO, SRB_FLAG_YES, CONFIG_ACTION_REMOVE, DCCH,UNDEF_SECURITY_MODE);
  rrc_rlc_config_req(eNB_index, ue_mod_idP,frameP,ENB_FLAG_NO, SRB_FLAG_YES, MBMS_FLAG_NO, CONFIG_ACTION_REMOVE,ue_mod_idP+DCCH,Rlc_info_am_config);

  rrc_pdcp_config_req (eNB_index, UE_id, frameP, ENB_FLAG_NO, SRB_FLAG_YES, CONFIG_ACTION_REMOVE, DCCH1,UNDEF_SECURITY_MODE);
  rrc_rlc_config_req(eNB_index, ue_mod_idP,frameP,ENB_FLAG_NO, SRB_FLAG_YES,CONFIG_ACTION_REMOVE, MBMS_FLAG_NO,ue_mod_idP+DCCH1,Rlc_info_am_config);

  rrc_pdcp_config_req (eNB_index, UE_id, frameP, ENB_FLAG_NO, SRB_FLAG_NO, CONFIG_ACTION_REMOVE, DTCH,UNDEF_SECURITY_MODE);
  rrc_rlc_config_req(eNB_index, ue_mod_idP,frameP,ENB_FLAG_NO, SRB_FLAG_NO,CONFIG_ACTION_REMOVE, MBMS_FLAG_NO,ue_mod_idP+DTCH,Rlc_info_um);
  /*
  rrc_pdcp_config_asn1_req(NB_eNB_INST+ue_mod_idP,frameP, 0,eNB_index,
			   NULL, // SRB_ToAddModList
			   NULL, // DRB_ToAddModList
			   drb2release_list,
			   0, // security mode
			   NULL, // key rrc encryption
			   NULL, // key rrc integrity
			   NULL // key encryption
#ifdef Rel10
			   ,NULL
#endif
			   );

  rrc_rlc_config_asn1_req(NB_eNB_INST+ue_mod_idP, frameP,0,eNB_index,
			  NULL,// SRB_ToAddModList
			  NULL,// DRB_ToAddModList
			  drb2release_list // DRB_ToReleaseList
#ifdef Rel10
			  ,NULL
#endif 
			  );
   */


  //A little cleanup at RRC...
  //Copying current queue config to free RRC index
  /*
    memcpy((void *)UE_rrc_inst[ue_mod_idP].SRB1_config[~(7<<eNB_index)],(void *)UE_rrc_inst[ue_mod_idP].SRB1_config[7<<eNB_index],sizeof(SRB_ToAddMod_t));
    memcpy((void *)UE_rrc_inst[ue_mod_idP].SRB2_config[~(7<<eNB_index)],(void *)UE_rrc_inst[ue_mod_idP].SRB2_config[7<<eNB_index],sizeof(SRB_ToAddMod_t));
    memcpy((void *)UE_rrc_inst[ue_mod_idP].DRB_config[~(7<<eNB_index)][0],(void *)UE_rrc_inst[ue_mod_idP].DRB_config[7<<eNB_index][0],sizeof(DRB_ToAddMod_t));
   */
  /*
  LOG_N(RRC,"Not sure if Freeing the current queue config works properly: Fix me\n");
  free((void *)&UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index]);
  free((void *)&UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index]);
  free((void *)&UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][0]);

  UE_rrc_inst[ue_mod_idP].SRB1_config[eNB_index] = NULL;
  UE_rrc_inst[ue_mod_idP].SRB2_config[eNB_index] = NULL;
  UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index][0] = NULL;
   */
  //Synchronisation to DL of target cell
  LOG_D(RRC,"HO: Reset PDCP and RLC for configured RBs.. \n[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ  (SRB2 eNB %d) --->][MAC_UE][MOD %02d][]\n",
      frameP, ue_mod_idP, eNB_index, ue_mod_idP);

  // Reset MAC and configure PHY
  rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
      (RadioResourceConfigCommonSIB_t *)NULL,
      (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
      (MeasObjectToAddMod_t **)NULL,
      (MAC_MainConfig_t *)NULL,
      0,
      (struct LogicalChannelConfig *)NULL,
      (MeasGapConfig_t *)NULL,
      (TDD_Config_t *)NULL,
      mobilityControlInfo,
      (uint8_t *)NULL,
      (uint16_t *)NULL,
      NULL,
      NULL,
      NULL,
      NULL
#ifdef Rel10
      ,0,
      (MBSFN_AreaInfoList_r9_t *)NULL,
      (PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
      ,0,
      0
#endif
  );

  // Re-establish PDCP for all RBs that are established
  // rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD, ue_mod_idP+DCCH);
  // rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD, ue_mod_idP+DCCH1);
  // rrc_pdcp_config_req (ue_mod_idP+NB_eNB_INST, frameP, 0, CONFIG_ACTION_ADD, ue_mod_idP+DTCH);


  // Re-establish RLC for all RBs that are established
  // rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,ue_mod_idP+DCCH,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
  // rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,ue_mod_idP+DCCH1,SIGNALLING_RADIO_BEARER,Rlc_info_am_config);
  // rrc_rlc_config_req(ue_mod_idP+NB_eNB_INST,frameP,0,CONFIG_ACTION_ADD,ue_mod_idP+DTCH,RADIO_ACCESS_BEARER,Rlc_info_um);

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].State = RRC_SI_RECEIVED;

}
void rrc_detach_from_eNB(module_id_t ue_mod_idP,uint8_t eNB_index) {
  //UE_rrc_inst[ue_mod_idP].DRB_config[eNB_index]
}

/*------------------------------------------------------------------------------------------*/
void  rrc_ue_decode_dcch(module_id_t ue_mod_idP, frame_t frameP,uint8_t Srb_id, uint8_t *Buffer,uint8_t eNB_index){
  /*------------------------------------------------------------------------------------------*/

  //DL_DCCH_Message_t dldcchmsg;
  DL_DCCH_Message_t *dl_dcch_msg=NULL;//&dldcchmsg;
  //  asn_dec_rval_t dec_rval;
  // int i;
  uint8_t target_eNB_index=0xFF;
#if defined(ENABLE_ITTI)
  MessageDef *msg_p;
#endif

  if (Srb_id != 1) {
      LOG_E(RRC,"[UE %d] Frame %d: Received message on DL-DCCH (SRB%d), should not have ...\n",
          ue_mod_idP, frameP, Srb_id);
      return;
  }

  //memset(dl_dcch_msg,0,sizeof(DL_DCCH_Message_t));

  // decode messages
  //  LOG_D(RRC,"[UE %d] Decoding DL-DCCH message\n",ue_mod_idP);
  /*
  for (i=0;i<30;i++)
    LOG_T(RRC,"%x.",Buffer[i]);
  LOG_T(RRC, "\n");
   */
  uper_decode(NULL,
      &asn_DEF_DL_DCCH_Message,
      (void**)&dl_dcch_msg,
      (uint8_t*)Buffer,
      RRC_BUF_SIZE,0,0);

#ifdef XER_PRINT
  xer_fprint(stdout,&asn_DEF_DL_DCCH_Message,(void*)dl_dcch_msg);
#endif

#if defined(ENABLE_ITTI)
# if defined(DISABLE_ITTI_XER_PRINT)
  {
    msg_p = itti_alloc_new_message (TASK_RRC_UE, RRC_DL_DCCH_MESSAGE);
    memcpy (&msg_p->ittiMsg, (void *) dl_dcch_msg, sizeof(RrcDlDcchMessage));

    itti_send_msg_to_task (TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
  }
# else
  {
    char        message_string[30000];
    size_t      message_string_size;

    if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_DL_DCCH_Message, (void *)dl_dcch_msg)) > 0)
      {
        msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_DL_DCCH, message_string_size + sizeof (IttiMsgText));
        msg_p->ittiMsg.rrc_dl_dcch.size = message_string_size;
        memcpy(&msg_p->ittiMsg.rrc_dl_dcch.text, message_string, message_string_size);

        itti_send_msg_to_task(TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
      }
  }
# endif
#endif

  if (dl_dcch_msg->message.present == DL_DCCH_MessageType_PR_c1) {

      if (UE_rrc_inst[ue_mod_idP].Info[eNB_index].State >= RRC_CONNECTED) {

          switch (dl_dcch_msg->message.choice.c1.present) {

          case DL_DCCH_MessageType__c1_PR_NOTHING:
            LOG_I(RRC, "[UE %d] Frame %d : Received PR_NOTHING on DL-DCCH-Message\n", ue_mod_idP, frameP);
            return;

          case DL_DCCH_MessageType__c1_PR_csfbParametersResponseCDMA2000:
            break;

          case DL_DCCH_MessageType__c1_PR_dlInformationTransfer: {
#if defined(ENABLE_ITTI)
            DLInformationTransfer_t *dlInformationTransfer = &dl_dcch_msg->message.choice.c1.choice.dlInformationTransfer;

            if ((dlInformationTransfer->criticalExtensions.present == DLInformationTransfer__criticalExtensions_PR_c1)
                && (dlInformationTransfer->criticalExtensions.choice.c1.present
                    == DLInformationTransfer__criticalExtensions__c1_PR_dlInformationTransfer_r8)
                    && (dlInformationTransfer->criticalExtensions.choice.c1.choice.dlInformationTransfer_r8.dedicatedInfoType.present
                        == DLInformationTransfer_r8_IEs__dedicatedInfoType_PR_dedicatedInfoNAS)) {
                /* This message hold a dedicated info NAS payload, forward it to NAS */
                struct DLInformationTransfer_r8_IEs__dedicatedInfoType *dedicatedInfoType =
                    &dlInformationTransfer->criticalExtensions.choice.c1.choice.dlInformationTransfer_r8.dedicatedInfoType;
                uint32_t pdu_length;
                uint8_t *pdu_buffer;
                MessageDef *msg_p;

                pdu_length = dedicatedInfoType->choice.dedicatedInfoNAS.size;
                pdu_buffer = dedicatedInfoType->choice.dedicatedInfoNAS.buf;

                msg_p = itti_alloc_new_message(TASK_RRC_UE, NAS_DOWNLINK_DATA_IND);
                NAS_DOWNLINK_DATA_IND(msg_p).UEid = ue_mod_idP; // TODO set the UEid to something else ?
                NAS_DOWNLINK_DATA_IND(msg_p).nasMsg.length = pdu_length;
                NAS_DOWNLINK_DATA_IND(msg_p).nasMsg.data = pdu_buffer;

                itti_send_msg_to_task(TASK_NAS_UE, ue_mod_idP + NB_eNB_INST, msg_p);
            }
#endif
            break;
          }

          case DL_DCCH_MessageType__c1_PR_handoverFromEUTRAPreparationRequest:
            break;

          case DL_DCCH_MessageType__c1_PR_mobilityFromEUTRACommand:
            break;

          case DL_DCCH_MessageType__c1_PR_rrcConnectionReconfiguration:
            // first check if mobilityControlInfo  is present
            if (dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.mobilityControlInfo
                != NULL) {
                /* 36.331, 5.3.5.4 Reception of an RRCConnectionReconfiguration including the mobilityControlInfo by the UE (handover)*/
                if (UE_rrc_inst[ue_mod_idP].HandoverInfoUe.targetCellId
                    != dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.mobilityControlInfo->targetPhysCellId) {
                    LOG_W(RRC,
                        "[UE %d] Frame %d: Handover target (%d) is different from RSRP measured target (%d)..\n", ue_mod_idP, frameP, dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.mobilityControlInfo->targetPhysCellId, UE_rrc_inst[ue_mod_idP].HandoverInfoUe.targetCellId);
                    return;
                }
                else
                  if ((target_eNB_index = get_adjacent_cell_mod_id(UE_rrc_inst[ue_mod_idP].HandoverInfoUe.targetCellId))
                      == 0xFF) {
                      LOG_W(RRC,
                          "[UE %d] Frame %d: ue_mod_idP of the target eNB not found, check the network topology\n", ue_mod_idP, frameP);
                      return;
                  }
                  else {
                      LOG_I(RRC,
                          "[UE% d] Frame %d: Received rrcConnectionReconfiguration with mobilityControlInfo \n", ue_mod_idP, frameP);
                      UE_rrc_inst[ue_mod_idP].HandoverInfoUe.measFlag = 1; // Ready to send more MeasReports if required
                  }
            }
            rrc_ue_process_rrcConnectionReconfiguration(ue_mod_idP, frameP,
                &dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration, eNB_index);
            if (target_eNB_index != 0xFF) {
                rrc_ue_generate_RRCConnectionReconfigurationComplete(ue_mod_idP, frameP, target_eNB_index,
                    dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.rrc_TransactionIdentifier);
                UE_rrc_inst[ue_mod_idP].Info[eNB_index].State = RRC_HO_EXECUTION;
                UE_rrc_inst[ue_mod_idP].Info[target_eNB_index].State = RRC_RECONFIGURED;
                LOG_I(RRC, "[UE %d] State = RRC_RECONFIGURED during HO (eNB %d)\n", ue_mod_idP, target_eNB_index);
#if defined(ENABLE_ITTI)
#ifdef ENABLE_RAL
                {
                  MessageDef                                 *message_ral_p = NULL;
                  rrc_ral_connection_reconfiguration_ho_ind_t connection_reconfiguration_ho_ind;
                  int                                         i;

                  message_ral_p = itti_alloc_new_message (TASK_RRC_UE, RRC_RAL_CONNECTION_RECONFIGURATION_HO_IND);
                  memset(&connection_reconfiguration_ho_ind, 0, sizeof(rrc_ral_connection_reconfiguration_ho_ind_t));
                  connection_reconfiguration_ho_ind.ue_id = ue_mod_idP;
                  if (dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList != NULL) {
                      connection_reconfiguration_ho_ind.num_drb      = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count;

                      for (i=0;(i<dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count) && (i < maxDRB);i++) {
                          // why minus 1 in RRC code for drb_identity ?
                          connection_reconfiguration_ho_ind.drb_id[i]   = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.array[i]->drb_Identity;
                      }
                  } else {
                      connection_reconfiguration_ho_ind.num_drb      = 0;
                  }
                  if (dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList != NULL) {
                      connection_reconfiguration_ho_ind.num_srb      = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList->list.count + UE_rrc_inst[ue_mod_idP].num_srb;
                  } else {
                      connection_reconfiguration_ho_ind.num_srb      += UE_rrc_inst[ue_mod_idP].num_srb;
                  }
		  if (connection_reconfiguration_ho_ind.num_srb > 2 )
         		connection_reconfiguration_ho_ind.num_srb =2;
                  memcpy (&message_ral_p->ittiMsg, (void *) &connection_reconfiguration_ho_ind, sizeof(rrc_ral_connection_reconfiguration_ho_ind_t));
                  //#warning "ue_mod_idP ? for instance ? => YES"
                  LOG_I(RRC, "Sending RRC_RAL_CONNECTION_RECONFIGURATION_HO_IND to mRAL\n");
                  itti_send_msg_to_task (TASK_RAL_UE, ue_mod_idP + NB_eNB_INST, message_ral_p);
                }
#endif
#endif
            }
            else {
                rrc_ue_generate_RRCConnectionReconfigurationComplete(ue_mod_idP, frameP, eNB_index,
                    dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.rrc_TransactionIdentifier);
                UE_rrc_inst[ue_mod_idP].Info[eNB_index].State = RRC_RECONFIGURED;
                LOG_I(RRC, "[UE %d] State = RRC_RECONFIGURED (eNB %d)\n", ue_mod_idP, eNB_index);
#if defined(ENABLE_ITTI)
#ifdef ENABLE_RAL
                {
                  MessageDef                                 *message_ral_p = NULL;
                  rrc_ral_connection_reconfiguration_ind_t    connection_reconfiguration_ind;
                  int                                         i;

                  message_ral_p = itti_alloc_new_message (TASK_RRC_UE, RRC_RAL_CONNECTION_RECONFIGURATION_IND);
                  memset(&connection_reconfiguration_ind, 0, sizeof(rrc_ral_connection_reconfiguration_ind_t));
                  connection_reconfiguration_ind.ue_id = ue_mod_idP;
                  if (dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList != NULL) {
                      connection_reconfiguration_ind.num_drb      = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count;

                      for (i=0;(i<dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.count) && (i < maxDRB);i++) {
                          // why minus 1 in RRC code for drb_identity ?
                          connection_reconfiguration_ind.drb_id[i]   = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->drb_ToAddModList->list.array[i]->drb_Identity;
                      }
                  } else {
                      connection_reconfiguration_ind.num_drb      = 0;
                  }
                  if (dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList != NULL) {
                      connection_reconfiguration_ind.num_srb      = dl_dcch_msg->message.choice.c1.choice.rrcConnectionReconfiguration.criticalExtensions.choice.c1.choice.rrcConnectionReconfiguration_r8.radioResourceConfigDedicated->srb_ToAddModList->list.count + UE_rrc_inst[ue_mod_idP].num_srb;
                  } else {
                      connection_reconfiguration_ind.num_srb      +=UE_rrc_inst[ue_mod_idP].num_srb;
                  }
		  if (connection_reconfiguration_ind.num_srb > 2 )
			connection_reconfiguration_ind.num_srb =2;
                  memcpy (&message_ral_p->ittiMsg, (void *) &connection_reconfiguration_ind, sizeof(rrc_ral_connection_reconfiguration_ind_t));
                  //#warning "ue_mod_idP ? for instance ? => YES"
                  LOG_I(RRC, "Sending RRC_RAL_CONNECTION_RECONFIGURATION_IND to mRAL\n");
                  itti_send_msg_to_task (TASK_RAL_UE, ue_mod_idP + NB_eNB_INST, message_ral_p);
                }
#endif
#endif

            }
            break;

          case DL_DCCH_MessageType__c1_PR_rrcConnectionRelease:
#if defined(ENABLE_ITTI)
            msg_p = itti_alloc_new_message(TASK_RRC_UE, NAS_CONN_RELEASE_IND);
            if ((dl_dcch_msg->message.choice.c1.choice.rrcConnectionRelease.criticalExtensions.present
                == RRCConnectionRelease__criticalExtensions_PR_c1)
                && (dl_dcch_msg->message.choice.c1.choice.rrcConnectionRelease.criticalExtensions.choice.c1.present
                    == RRCConnectionRelease__criticalExtensions__c1_PR_rrcConnectionRelease_r8)) {
                NAS_CONN_RELEASE_IND(msg_p).cause =
                    dl_dcch_msg->message.choice.c1.choice.rrcConnectionRelease.criticalExtensions.choice.c1.choice.rrcConnectionRelease_r8.releaseCause;
            }

            itti_send_msg_to_task(TASK_NAS_UE, ue_mod_idP + NB_eNB_INST, msg_p);
#if defined(ENABLE_RAL)
            msg_p = itti_alloc_new_message(TASK_RRC_UE, RRC_RAL_CONNECTION_RELEASE_IND);
            RRC_RAL_CONNECTION_RELEASE_IND(msg_p).ue_id = ue_mod_idP;
            itti_send_msg_to_task(TASK_RAL_UE, ue_mod_idP + NB_eNB_INST, msg_p);
#endif
#endif
            break;

          case DL_DCCH_MessageType__c1_PR_securityModeCommand:
            LOG_I(RRC, "[UE %d] Received securityModeCommand (eNB %d)\n", ue_mod_idP, eNB_index);
            rrc_ue_process_securityModeCommand(ue_mod_idP, frameP, &dl_dcch_msg->message.choice.c1.choice.securityModeCommand,
                eNB_index);
            break;

          case DL_DCCH_MessageType__c1_PR_ueCapabilityEnquiry:
            LOG_I(RRC, "[UE %d] Received Capability Enquiry (eNB %d)\n", ue_mod_idP, eNB_index);
            rrc_ue_process_ueCapabilityEnquiry(ue_mod_idP, frameP, &dl_dcch_msg->message.choice.c1.choice.ueCapabilityEnquiry,
                eNB_index);
            break;

          case DL_DCCH_MessageType__c1_PR_counterCheck:
            break;

#ifdef Rel10
          case DL_DCCH_MessageType__c1_PR_ueInformationRequest_r9:
            break;
          case DL_DCCH_MessageType__c1_PR_loggedMeasurementConfiguration_r10:
            break;
          case DL_DCCH_MessageType__c1_PR_rnReconfiguration_r10:
            break;
#endif

          case DL_DCCH_MessageType__c1_PR_spare1:
          case DL_DCCH_MessageType__c1_PR_spare2:
          case DL_DCCH_MessageType__c1_PR_spare3:
          case DL_DCCH_MessageType__c1_PR_spare4:
            break;

          default:
            break;
          }
      }
  }
#ifndef NO_RRM
  send_msg(&S_rrc,msg_rrc_end_scan_req(ue_mod_idP,eNB_index));
#endif
}

const char siWindowLength[7][5] = {"1ms\0","2ms\0","5ms\0","10ms\0","15ms\0","20ms\0","40ms\0"};
const char siWindowLength_int[7] = {1,2,5,10,15,20,40};

const char SIBType[16][6] ={"SIB3\0","SIB4\0","SIB5\0","SIB6\0","SIB7\0","SIB8\0","SIB9\0","SIB10\0","SIB11\0","SIB12\0","SIB13\0","Sp2\0","Sp3\0","Sp4\0"};
const char SIBPeriod[7][7]= {"80ms\0","160ms\0","320ms\0","640ms\0","1280ms\0","2560ms\0","5120ms\0"};
int siPeriod_int[7] = {80,160,320,640,1280,2560,5120};

int decode_BCCH_DLSCH_Message(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index,uint8_t *Sdu,uint8_t Sdu_len, uint8_t rsrq, uint8_t rsrp) {

  //BCCH_DL_SCH_Message_t bcch_message;
  BCCH_DL_SCH_Message_t *bcch_message=NULL;//_ptr=&bcch_message;
  SystemInformationBlockType1_t **sib1=&UE_rrc_inst[ue_mod_idP].sib1[eNB_index];
  SystemInformation_t **si=UE_rrc_inst[ue_mod_idP].si[eNB_index];
  asn_dec_rval_t dec_rval;
  uint32_t si_window;//, sib1_decoded=0, si_decoded=0;
  // int i;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_BCCH, VCD_FUNCTION_IN);


  if ((UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status == 1) &&
      (UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus == 1)) {
      // Avoid decoding to prevent memory bloating
      vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_BCCH, VCD_FUNCTION_OUT);
      return 0;
  } 
  else {

      rrc_set_sub_state (ue_mod_idP, RRC_SUB_STATE_IDLE_RECEIVING_SIB);

      //memset(&bcch_message,0,sizeof(BCCH_DL_SCH_Message_t));
      //  LOG_D(RRC,"[UE %d] Decoding DL_BCCH_DLSCH_Message\n",ue_mod_idP)
      /*
    for (i=0;i<Sdu_len;i++)
      printf("%x.",Sdu[i]);
      printf("\n");*/
      dec_rval = uper_decode_complete(NULL,
          &asn_DEF_BCCH_DL_SCH_Message,
          (void **)&bcch_message,
          (const void *)Sdu,
          Sdu_len);//,0,0);

      if ((dec_rval.code != RC_OK) && (dec_rval.consumed==0)) {
          LOG_E(RRC,"[UE %d] Failed to decode BCCH_DLSCH_MESSAGE (%d bits)\n",ue_mod_idP,dec_rval.consumed);
          //free the memory
          SEQUENCE_free(&asn_DEF_BCCH_DL_SCH_Message, (void*)bcch_message, 1);
          vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_BCCH, VCD_FUNCTION_OUT);
          return -1;
      }
      //  xer_fprint(stdout,  &asn_DEF_BCCH_DL_SCH_Message, (void*)&bcch_message);

#if defined(ENABLE_ITTI)
# if defined(DISABLE_ITTI_XER_PRINT)
      {
        MessageDef *msg_p;

        msg_p = itti_alloc_new_message (TASK_RRC_UE, RRC_DL_BCCH_MESSAGE);
        memcpy (&msg_p->ittiMsg, (void *) bcch_message, sizeof(RrcDlBcchMessage));

        itti_send_msg_to_task (TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
      }
# else
      {
        char        message_string[15000];
        size_t      message_string_size;

        if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_BCCH_DL_SCH_Message, (void *)bcch_message)) > 0)
          {
            MessageDef *msg_p;

            msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_DL_BCCH, message_string_size + sizeof (IttiMsgText));
            msg_p->ittiMsg.rrc_dl_bcch.size = message_string_size;
            memcpy(&msg_p->ittiMsg.rrc_dl_bcch.text, message_string, message_string_size);

            itti_send_msg_to_task(TASK_UNKNOWN, ue_mod_idP + NB_eNB_INST, msg_p);
          }
      }
# endif
#endif

      if (bcch_message->message.present == BCCH_DL_SCH_MessageType_PR_c1) {
          switch (bcch_message->message.choice.c1.present) {
          case BCCH_DL_SCH_MessageType__c1_PR_systemInformationBlockType1:
            if ((frameP %2) == 0) {
                if (UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status == 0) {
                    memcpy((void*)*sib1,
                        (void*)&bcch_message->message.choice.c1.choice.systemInformationBlockType1,
                        sizeof(SystemInformationBlockType1_t));
                    LOG_D(RRC,"[UE %d] Decoding First SIB1\n",ue_mod_idP);
                    decode_SIB1(ue_mod_idP, eNB_index, rsrq, rsrp);
                    //mac_xface->macphy_exit("after decode_SIB1");
                }
            }
            break;

          case BCCH_DL_SCH_MessageType__c1_PR_systemInformation:
            if ((UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status == 1) &&
                (UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus == 0)) {
                //                                                if ((frameP %8) == 1) {  // check only in odd frames for SI
                si_window = (frameP%(UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIperiod/10))/(UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIwindowsize/10);
                memcpy((void*)si[si_window],
                    (void*)&bcch_message->message.choice.c1.choice.systemInformation,
                    sizeof(SystemInformation_t));
                LOG_D(RRC,"[UE %d] Decoding SI for frameP %d, si_window %d\n",ue_mod_idP,frameP,si_window);
                decode_SI(ue_mod_idP,frameP,eNB_index,si_window);
                //mac_xface->macphy_exit("after decode_SI");

                //                                }
            }
            break;

          case BCCH_DL_SCH_MessageType__c1_PR_NOTHING:
          default:
            break;
          }
      }
  }

  if ((rrc_get_sub_state(ue_mod_idP) == RRC_SUB_STATE_IDLE_SIB_COMPLETE)
# if defined(ENABLE_USE_MME)
      && (UE_rrc_inst[ue_mod_idP].initialNasMsg.data != NULL)
#endif
  ) {
      rrc_ue_generate_RRCConnectionRequest(ue_mod_idP, frameP, 0);
      LOG_I(RRC, "not sending connection request\n");

      rrc_set_sub_state (ue_mod_idP, RRC_SUB_STATE_IDLE_CONNECTING);
  }

  /*  if ((UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status == 1) &&
      (UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus == 1) && (frameP >= ue_mod_idP * 20 + 10))
      SEQUENCE_free(&asn_DEF_BCCH_DL_SCH_Message, (void*)bcch_message, 0);*/
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_BCCH, VCD_FUNCTION_OUT);

  return 0;
}


int decode_SIB1(module_id_t ue_mod_idP,uint8_t eNB_index, uint8_t rsrq, uint8_t rsrp) {
  SystemInformationBlockType1_t **sib1=&UE_rrc_inst[ue_mod_idP].sib1[eNB_index];
  int i;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SIB1, VCD_FUNCTION_IN);

  LOG_D(RRC,"[UE %d] : Dumping SIB 1\n",ue_mod_idP);

  //  xer_fprint(stdout,&asn_DEF_SystemInformationBlockType1, (void*)*sib1);

  LOG_D(RRC,"cellAccessRelatedInfo.cellIdentity : %x.%x.%x.%x\n",
      (*sib1)->cellAccessRelatedInfo.cellIdentity.buf[0],
      (*sib1)->cellAccessRelatedInfo.cellIdentity.buf[1],
      (*sib1)->cellAccessRelatedInfo.cellIdentity.buf[2],
      (*sib1)->cellAccessRelatedInfo.cellIdentity.buf[3]);

  LOG_D(RRC,"cellSelectionInfo.q_RxLevMin       : %d\n",(int)(*sib1)->cellSelectionInfo.q_RxLevMin);
  LOG_D(RRC,"freqBandIndicator                  : %d\n",(int)(*sib1)->freqBandIndicator);
  LOG_D(RRC,"siWindowLength                     : %s\n",siWindowLength[(*sib1)->si_WindowLength]);
  if ((*sib1)->schedulingInfoList.list.count>0) {
      for (i=0;i<(*sib1)->schedulingInfoList.list.count;i++) {
          LOG_D(RRC,"siSchedulingInfoPeriod[%d]          : %s\n",i,SIBPeriod[(int)(*sib1)->schedulingInfoList.list.array[i]->si_Periodicity]);
          if ((*sib1)->schedulingInfoList.list.array[i]->sib_MappingInfo.list.count>0)
            LOG_D(RRC,"siSchedulingInfoSIBType[%d]         : %s\n",i,SIBType[(int)(*(*sib1)->schedulingInfoList.list.array[i]->sib_MappingInfo.list.array[0])]);
          else {
              LOG_W(RRC,"mapping list %d is null\n",i);
          }
      }
  }
  else {
      LOG_E(RRC,"siSchedulingInfoPeriod[0]          : PROBLEM!!!\n");
      return -1;
  }

  if ((*sib1)->tdd_Config) {
      LOG_D(RRC,"TDD subframe assignment            : %d\n",(int)(*sib1)->tdd_Config->subframeAssignment);
      LOG_D(RRC,"S-Subframe Config                  : %d\n",(int)(*sib1)->tdd_Config->specialSubframePatterns);
  }

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIperiod     = siPeriod_int[(*sib1)->schedulingInfoList.list.array[0]->si_Periodicity];
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIwindowsize = siWindowLength_int[(*sib1)->si_WindowLength];
  LOG_D(RRC, "[MSC_MSG][FRAME unknown][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (SIB1 params eNB %d) --->][MAC_UE][MOD %02d][]\n",
      ue_mod_idP, eNB_index, ue_mod_idP);

  rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
      (RadioResourceConfigCommonSIB_t *)NULL,
      (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
      (MeasObjectToAddMod_t **)NULL,
      (MAC_MainConfig_t *)NULL,
      0,
      (struct LogicalChannelConfig *)NULL,
      (MeasGapConfig_t *)NULL,
      UE_rrc_inst[ue_mod_idP].sib1[eNB_index]->tdd_Config,
      (MobilityControlInfo_t *) NULL,
      &UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIwindowsize,
      &UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIperiod,
      NULL,
      NULL,
      NULL,
      (MBSFN_SubframeConfigList_t *)NULL
#ifdef Rel10
      ,0,
      (MBSFN_AreaInfoList_r9_t *)NULL,
      (PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
      ,
      0,
      0
#endif
  );

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1Status = 1;
  UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIB1systemInfoValueTag = (*sib1)->systemInfoValueTag;

#if defined(ENABLE_ITTI) && defined(ENABLE_USE_MME)
  {
    int cell_valid = 0;

    if ((*sib1)->cellAccessRelatedInfo.cellBarred == SystemInformationBlockType1__cellAccessRelatedInfo__cellBarred_notBarred) {
        /* Cell is not barred */
        int plmn;
        int plmn_number;

        plmn_number = (*sib1)->cellAccessRelatedInfo.plmn_IdentityList.list.count;

        /* Compare requested PLMN and PLMNs from SIB1*/
        for (plmn = 0; plmn < plmn_number; plmn++) {
            PLMN_Identity_t *plmn_Identity;

            plmn_Identity = &(*sib1)->cellAccessRelatedInfo.plmn_IdentityList.list.array[plmn]->plmn_Identity;
            if (((plmn_Identity->mcc == NULL)
                ||
                ((UE_rrc_inst[ue_mod_idP].plmnID.MCCdigit1 == *(plmn_Identity->mcc->list.array[0])) &&
                    (UE_rrc_inst[ue_mod_idP].plmnID.MCCdigit2 == *(plmn_Identity->mcc->list.array[1])) &&
                    (UE_rrc_inst[ue_mod_idP].plmnID.MCCdigit3 == *(plmn_Identity->mcc->list.array[2]))))
                    &&
                    (UE_rrc_inst[ue_mod_idP].plmnID.MNCdigit1 == *(plmn_Identity->mnc.list.array[0])) &&
                    (UE_rrc_inst[ue_mod_idP].plmnID.MNCdigit2 == *(plmn_Identity->mnc.list.array[1])) &&
                    (((UE_rrc_inst[ue_mod_idP].plmnID.MNCdigit3 == 0xf) && (plmn_Identity->mnc.list.count == 2))
                        ||
                        (UE_rrc_inst[ue_mod_idP].plmnID.MNCdigit3 == *(plmn_Identity->mnc.list.array[2])))) {
                /* PLMN match, send a confirmation to NAS */
                MessageDef  *msg_p;

                msg_p = itti_alloc_new_message(TASK_RRC_UE, NAS_CELL_SELECTION_CNF);
                NAS_CELL_SELECTION_CNF (msg_p).errCode = AS_SUCCESS;
                NAS_CELL_SELECTION_CNF (msg_p).cellID = BIT_STRING_to_uint32(&(*sib1)->cellAccessRelatedInfo.cellIdentity);
                NAS_CELL_SELECTION_CNF (msg_p).tac = BIT_STRING_to_uint16(&(*sib1)->cellAccessRelatedInfo.trackingAreaCode);
                NAS_CELL_SELECTION_CNF (msg_p).rat = 0xFF;
                NAS_CELL_SELECTION_CNF (msg_p).rsrq = rsrq;
                NAS_CELL_SELECTION_CNF (msg_p).rsrp = rsrp;

                itti_send_msg_to_task(TASK_NAS_UE, ue_mod_idP + NB_eNB_INST, msg_p);
                cell_valid = 1;
                break;
            }
        }
    }

    if (cell_valid == 0)
      {
        /* Cell can not be used, ask PHY to try the next one */
        MessageDef  *msg_p;

        msg_p = itti_alloc_new_message(TASK_RRC_UE, PHY_FIND_NEXT_CELL_REQ);

        itti_send_msg_to_task(TASK_PHY_UE, ue_mod_idP + NB_eNB_INST, msg_p);
      }
  }
#endif

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SIB1, VCD_FUNCTION_OUT);
  return 0;

}


void dump_sib2(SystemInformationBlockType2_t *sib2) {

  LOG_D(RRC,"radioResourceConfigCommon.rach_ConfigCommon.preambleInfo.numberOfRA_Preambles : %ld\n",
      sib2->radioResourceConfigCommon.rach_ConfigCommon.preambleInfo.numberOfRA_Preambles);

  //  if (radioResourceConfigCommon.rach_ConfigCommon.preambleInfo.preamblesGroupAConfig)
  //msg("radioResourceConfigCommon.rach_ConfigCommon.preambleInfo.preamblesGroupAConfig ",sib2->radioResourceConfigCommon.rach_ConfigCommon.preambleInfo.preamblesGroupAConfig = NULL;

  LOG_D(RRC,"[UE]radioResourceConfigCommon.rach_ConfigCommon.powerRampingParameters.powerRampingStep : %ld\n",sib2->radioResourceConfigCommon.rach_ConfigCommon.powerRampingParameters.powerRampingStep);

  LOG_D(RRC,"[UE]radioResourceConfigCommon.rach_ConfigCommon.powerRampingParameters.preambleInitialReceivedTargetPower : %ld\n",sib2->radioResourceConfigCommon.rach_ConfigCommon.powerRampingParameters.preambleInitialReceivedTargetPower);

  LOG_D(RRC,"radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.preambleTransMax  : %ld\n",sib2->radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.preambleTransMax);

  LOG_D(RRC,"radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.ra_ResponseWindowSize : %ld\n",sib2->radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.ra_ResponseWindowSize);

  LOG_D(RRC,"radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.mac_ContentionResolutionTimer : %ld\n",sib2->radioResourceConfigCommon.rach_ConfigCommon.ra_SupervisionInfo.mac_ContentionResolutionTimer);

  LOG_D(RRC,"radioResourceConfigCommon.rach_ConfigCommon.maxHARQ_Msg3Tx : %ld\n",
      sib2->radioResourceConfigCommon.rach_ConfigCommon.maxHARQ_Msg3Tx);

  LOG_D(RRC,"radioResourceConfigCommon.prach_Config.rootSequenceIndex : %ld\n",sib2->radioResourceConfigCommon.prach_Config.rootSequenceIndex);
  LOG_D(RRC,"radioResourceConfigCommon.prach_Config.prach_ConfigInfo.prach_ConfigIndex : %ld\n",sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.prach_ConfigIndex);
  LOG_D(RRC,"radioResourceConfigCommon.prach_Config.prach_ConfigInfo.highSpeedFlag : %d\n",  (int)sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.highSpeedFlag);
  LOG_D(RRC,"radioResourceConfigCommon.prach_Config.prach_ConfigInfo.zeroCorrelationZoneConfig : %ld\n",  sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.zeroCorrelationZoneConfig);
  LOG_D(RRC,"radioResourceConfigCommon.prach_Config.prach_ConfigInfo.prach_FreqOffset %ld\n",  sib2->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.prach_FreqOffset);

  // PDSCH-Config
  LOG_D(RRC,"radioResourceConfigCommon.pdsch_ConfigCommon.referenceSignalPower  : %ld\n",sib2->radioResourceConfigCommon.pdsch_ConfigCommon.referenceSignalPower);
  LOG_D(RRC,"radioResourceConfigCommon.pdsch_ConfigCommon.p_b : %ld\n",sib2->radioResourceConfigCommon.pdsch_ConfigCommon.p_b);

  // PUSCH-Config
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.n_SB  : %ld\n",sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.n_SB);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.hoppingMode  : %ld\n",sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.hoppingMode);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.pusch_HoppingOffset : %ld\n",sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.pusch_HoppingOffset);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.enable64QAM : %d\n",(int)sib2->radioResourceConfigCommon.pusch_ConfigCommon.pusch_ConfigBasic.enable64QAM);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupHoppingEnabled : %d\n",(int)sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupHoppingEnabled);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH : %ld\n",sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled : %d\n",(int)sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled);
  LOG_D(RRC,"radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.cyclicShift : %ld\n",sib2->radioResourceConfigCommon.pusch_ConfigCommon.ul_ReferenceSignalsPUSCH.cyclicShift);

  // PUCCH-Config

  LOG_D(RRC,"radioResourceConfigCommon.pucch_ConfigCommon.deltaPUCCH_Shift : %ld\n",sib2->radioResourceConfigCommon.pucch_ConfigCommon.deltaPUCCH_Shift);
  LOG_D(RRC,"radioResourceConfigCommon.pucch_ConfigCommon.nRB_CQI : %ld\n",sib2->radioResourceConfigCommon.pucch_ConfigCommon.nRB_CQI);
  LOG_D(RRC,"radioResourceConfigCommon.pucch_ConfigCommon.nCS_AN : %ld\n",sib2->radioResourceConfigCommon.pucch_ConfigCommon.nCS_AN);
  LOG_D(RRC,"radioResourceConfigCommon.pucch_ConfigCommon.n1PUCCH_AN : %ld\n",sib2->radioResourceConfigCommon.pucch_ConfigCommon.n1PUCCH_AN);

  LOG_D(RRC,"radioResourceConfigCommon.soundingRS_UL_ConfigCommon.present : %d\n",sib2-> radioResourceConfigCommon.soundingRS_UL_ConfigCommon.present);


  // uplinkPowerControlCommon

  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.p0_NominalPUSCH : %ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.p0_NominalPUSCH);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.alpha : %ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.alpha);

  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.p0_NominalPUCCH : %ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.p0_NominalPUCCH);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format1 : %ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format1);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format1b :%ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format1b);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2  :%ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2a :%ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2a);
  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2b :%ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaFList_PUCCH.deltaF_PUCCH_Format2b);

  LOG_D(RRC,"radioResourceConfigCommon.uplinkPowerControlCommon.deltaPreambleMsg3 : %ld\n",sib2->radioResourceConfigCommon.uplinkPowerControlCommon.deltaPreambleMsg3);

  LOG_D(RRC,"radioResourceConfigCommon.ul_CyclicPrefixLength : %ld\n", sib2->radioResourceConfigCommon.ul_CyclicPrefixLength);

  LOG_D(RRC,"ue_TimersAndConstants.t300 : %ld\n", sib2->ue_TimersAndConstants.t300);
  LOG_D(RRC,"ue_TimersAndConstants.t301 : %ld\n", sib2->ue_TimersAndConstants.t301);
  LOG_D(RRC,"ue_TimersAndConstants.t310 : %ld\n", sib2->ue_TimersAndConstants.t310);
  LOG_D(RRC,"ue_TimersAndConstants.n310 : %ld\n", sib2->ue_TimersAndConstants.n310);
  LOG_D(RRC,"ue_TimersAndConstants.t311 : %ld\n", sib2->ue_TimersAndConstants.t311);
  LOG_D(RRC,"ue_TimersAndConstants.n311 : %ld\n", sib2->ue_TimersAndConstants.n311);

  LOG_D(RRC,"freqInfo.additionalSpectrumEmission : %ld\n",sib2->freqInfo.additionalSpectrumEmission);
  LOG_D(RRC,"freqInfo.ul_CarrierFreq : %p\n", sib2->freqInfo.ul_CarrierFreq);
  LOG_D(RRC,"freqInfo.ul_Bandwidth : %p\n", sib2->freqInfo.ul_Bandwidth);
  LOG_D(RRC,"mbsfn_SubframeConfigList : %p\n", sib2->mbsfn_SubframeConfigList);
  LOG_D(RRC,"timeAlignmentTimerCommon : %ld\n", sib2->timeAlignmentTimerCommon);
}

void dump_sib3(SystemInformationBlockType3_t *sib3) {

}

#ifdef Rel10
void dump_sib13(SystemInformationBlockType13_r9_t *sib13) {

  LOG_D(RRC,"[RRC][UE] Dumping SIB13\n");
  LOG_D(RRC,"[RRC][UE] dumping sib13 second time\n");
  LOG_D(RRC,"[RRC][UE] NotificationRepetitionCoeff-r9 : %ld\n", sib13->notificationConfig_r9.notificationRepetitionCoeff_r9);
  LOG_D(RRC,"[RRC][UE] NotificationOffset-r9 : %d\n", (int)sib13->notificationConfig_r9.notificationOffset_r9);
  LOG_D(RRC,"[RRC][UE] NotificationSF-Index-r9 : %d\n", (int)sib13->notificationConfig_r9.notificationSF_Index_r9);

}
#endif

//const char SIBPeriod[7][7]= {"80ms\0","160ms\0","320ms\0","640ms\0","1280ms\0","2560ms\0","5120ms\0"};
int decode_SI(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index,uint8_t si_window) {

  SystemInformation_t **si=&UE_rrc_inst[ue_mod_idP].si[eNB_index][si_window];
  int i;
  struct SystemInformation_r8_IEs_sib_TypeAndInfo_Member *typeandinfo;

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SI  , VCD_FUNCTION_IN);

  // Dump contents
  if ((*si)->criticalExtensions.present==SystemInformation__criticalExtensions_PR_systemInformation_r8) {
      LOG_D(RRC,"(*si)->criticalExtensions.choice.systemInformation_r8.sib_TypeAndInfo.list.count %d\n",
          (*si)->criticalExtensions.choice.systemInformation_r8.sib_TypeAndInfo.list.count);
  }
  else {
      LOG_D(RRC,"[UE] Unknown criticalExtension version (not Rel8)\n");
      return -1;
  }

  for (i=0;i<(*si)->criticalExtensions.choice.systemInformation_r8.sib_TypeAndInfo.list.count;i++) {
      LOG_D(RRC,"SI count %d\n",i);
      typeandinfo=(*si)->criticalExtensions.choice.systemInformation_r8.sib_TypeAndInfo.list.array[i];

      switch(typeandinfo->present) {
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib2:
        UE_rrc_inst[ue_mod_idP].sib2[eNB_index] = &typeandinfo->choice.sib2;
        LOG_D(RRC,"[UE %d] Frame %d Found SIB2 from eNB %d\n",ue_mod_idP,frameP,eNB_index);
        dump_sib2(UE_rrc_inst[ue_mod_idP].sib2[eNB_index]);
        LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (SIB2 params  eNB %d) --->][MAC_UE][MOD %02d][]\n",
            frameP, ue_mod_idP, eNB_index, ue_mod_idP);
        rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
            &UE_rrc_inst[ue_mod_idP].sib2[eNB_index]->radioResourceConfigCommon,
            (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
  	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif			   
            (MeasObjectToAddMod_t **)NULL,
            (MAC_MainConfig_t *)NULL,
            0,
            (struct LogicalChannelConfig *)NULL,
            (MeasGapConfig_t *)NULL,
            (TDD_Config_t *)NULL,
            (MobilityControlInfo_t *)NULL,
            NULL,
            NULL,
            UE_rrc_inst[ue_mod_idP].sib2[eNB_index]->freqInfo.ul_CarrierFreq,
            UE_rrc_inst[ue_mod_idP].sib2[eNB_index]->freqInfo.ul_Bandwidth,
            &UE_rrc_inst[ue_mod_idP].sib2[eNB_index]->freqInfo.additionalSpectrumEmission,
            UE_rrc_inst[ue_mod_idP].sib2[eNB_index]->mbsfn_SubframeConfigList
#ifdef Rel10
,0,
(MBSFN_AreaInfoList_r9_t *)NULL,
(PMCH_InfoList_r9_t *)NULL
#endif
#ifdef CBA
,0,
0
#endif
        );
        UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus = 1;
        // After SI is received, prepare RRCConnectionRequest
#ifdef Rel10
        if (UE_rrc_inst[ue_mod_idP].MBMS_flag < 3) // see -Q option
#endif
#if !(defined(ENABLE_ITTI) && defined(ENABLE_USE_MME))
          rrc_ue_generate_RRCConnectionRequest(ue_mod_idP,frameP,eNB_index);
        LOG_I(RRC, "not sending connection request\n");
#endif

        if (UE_rrc_inst[ue_mod_idP].Info[eNB_index].State == RRC_IDLE) {
            LOG_I(RRC,"[UE %d] Received SIB1/SIB2/SIB3 Switching to RRC_SI_RECEIVED\n",ue_mod_idP);
            UE_rrc_inst[ue_mod_idP].Info[eNB_index].State = RRC_SI_RECEIVED;
#ifdef ENABLE_RAL
            {
              MessageDef                            *message_ral_p = NULL;
              rrc_ral_system_information_ind_t       ral_si_ind;

              message_ral_p = itti_alloc_new_message (TASK_RRC_UE, RRC_RAL_SYSTEM_INFORMATION_IND);
              memset(&ral_si_ind, 0, sizeof(rrc_ral_system_information_ind_t));
              ral_si_ind.plmn_id.MCCdigit2 = '0';
              ral_si_ind.plmn_id.MCCdigit1 = '2';
              ral_si_ind.plmn_id.MNCdigit3 = '0';
              ral_si_ind.plmn_id.MCCdigit3 = '8';
              ral_si_ind.plmn_id.MNCdigit2 = '9';
              ral_si_ind.plmn_id.MNCdigit1 = '9';
              ral_si_ind.cell_id        = 1;
              ral_si_ind.dbm            = 0;
              //ral_si_ind.dbm            = fifo_dump_emos_UE.PHY_measurements->rx_rssi_dBm[eNB_index];
              // TO DO
              ral_si_ind.sinr           = 0;
              //ral_si_ind.sinr           = fifo_dump_emos_UE.PHY_measurements->subband_cqi_dB[eNB_index][phy_vars_ue->lte_frame_parms.nb_antennas_rx][0];
              // TO DO
              ral_si_ind.link_data_rate = 0;
              memcpy (&message_ral_p->ittiMsg, (void *) &ral_si_ind, sizeof(rrc_ral_system_information_ind_t));
#warning "ue_mod_idP ? for instance ?"
              itti_send_msg_to_task (TASK_RAL_UE, ue_mod_idP + NB_eNB_INST, message_ral_p);
            }
#endif
        }
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib3:
        UE_rrc_inst[ue_mod_idP].sib3[eNB_index] = &typeandinfo->choice.sib3;
        LOG_I(RRC,"[UE %d] Frame %d Found SIB3 from eNB %d\n",ue_mod_idP,frameP,eNB_index);
        dump_sib3(UE_rrc_inst[ue_mod_idP].sib3[eNB_index]);
        UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus = 1;

        rrc_set_sub_state (ue_mod_idP, RRC_SUB_STATE_IDLE_SIB_COMPLETE);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib4:
        UE_rrc_inst[ue_mod_idP].sib4[eNB_index] = &typeandinfo->choice.sib4;
        LOG_I(RRC,"[UE %d] Frame %d Found SIB4 from eNB %d\n",ue_mod_idP,frameP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib5:
        UE_rrc_inst[ue_mod_idP].sib5[eNB_index] = &typeandinfo->choice.sib5;
        LOG_I(RRC,"[UE %d] Found SIB5 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib6:
        UE_rrc_inst[ue_mod_idP].sib6[eNB_index] = &typeandinfo->choice.sib6;
        LOG_I(RRC,"[UE %d] Found SIB6 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib7:
        UE_rrc_inst[ue_mod_idP].sib7[eNB_index] = &typeandinfo->choice.sib7;
        LOG_I(RRC,"[UE %d] Found SIB7 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib8:
        UE_rrc_inst[ue_mod_idP].sib8[eNB_index] = &typeandinfo->choice.sib8;
        LOG_I(RRC,"[UE %d] Found SIB8 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib9:
        UE_rrc_inst[ue_mod_idP].sib9[eNB_index] = &typeandinfo->choice.sib9;
        LOG_I(RRC,"[UE %d] Found SIB9 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib10:
        UE_rrc_inst[ue_mod_idP].sib10[eNB_index] = &typeandinfo->choice.sib10;
        LOG_I(RRC,"[UE %d] Found SIB10 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib11:
        UE_rrc_inst[ue_mod_idP].sib11[eNB_index] = &typeandinfo->choice.sib11;
        LOG_I(RRC,"[UE %d] Found SIB11 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
#ifdef Rel10
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib12_v920:
        UE_rrc_inst[ue_mod_idP].sib12[eNB_index] = &typeandinfo->choice.sib12_v920;
        LOG_I(RRC,"[RRC][UE %d] Found SIB12 from eNB %d\n",ue_mod_idP,eNB_index);
        break;
      case SystemInformation_r8_IEs_sib_TypeAndInfo_Member_PR_sib13_v920:
        UE_rrc_inst[ue_mod_idP].sib13[eNB_index] = &typeandinfo->choice.sib13_v920;
        LOG_I(RRC,"[RRC][UE %d] Found SIB13 from eNB %d\n",ue_mod_idP,eNB_index);
        dump_sib13(UE_rrc_inst[ue_mod_idP].sib13[eNB_index]);
        // adding here function to store necessary parameters for using in decode_MCCH_Message + maybe transfer to PHY layer
        LOG_D(RRC, "[MSC_MSG][FRAME %05d][RRC_UE][MOD %02d][][--- MAC_CONFIG_REQ (SIB13 params eNB %d) --->][MAC_UE][MOD %02d][]\n",
            frameP, ue_mod_idP, eNB_index, ue_mod_idP);
        rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
            (RadioResourceConfigCommonSIB_t *)NULL,
            (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
            (MeasObjectToAddMod_t **)NULL,
            (MAC_MainConfig_t *)NULL,
            0,
            (struct LogicalChannelConfig *)NULL,
            (MeasGapConfig_t *)NULL,
            (TDD_Config_t *)NULL,
            (MobilityControlInfo_t *)NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            (MBSFN_SubframeConfigList_t *)NULL
            ,0,
            &UE_rrc_inst[ue_mod_idP].sib13[eNB_index]->mbsfn_AreaInfoList_r9,
            (PMCH_InfoList_r9_t *)NULL
#ifdef CBA
,0,
0
#endif
        );
        UE_rrc_inst[ue_mod_idP].Info[eNB_index].SIStatus = 1;
        break;
#endif
      default:
        break;
      }

  }

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SI  , VCD_FUNCTION_OUT);
  return 0;
}

// layer 3 filtering of RSRP (EUTRA) measurements: 36.331, Sec. 5.5.3.2
void ue_meas_filtering(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index){
  float a  = UE_rrc_inst[ue_mod_idP].filter_coeff_rsrp; // 'a' in 36.331 Sec. 5.5.3.2
  float a1 = UE_rrc_inst[ue_mod_idP].filter_coeff_rsrq;
  //float rsrp_db, rsrq_db;
  uint8_t    eNB_offset;

  if(UE_rrc_inst[ue_mod_idP].QuantityConfig[0] != NULL) { // Only consider 1 serving cell (index: 0)
      if (UE_rrc_inst[ue_mod_idP].QuantityConfig[0]->quantityConfigEUTRA != NULL) {
          if(UE_rrc_inst[ue_mod_idP].QuantityConfig[0]->quantityConfigEUTRA->filterCoefficientRSRP != NULL) {
              for (eNB_offset = 0;eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0);eNB_offset++) {
                  //filter_factor = 1/power(2,*UE_rrc_inst[ue_mod_idP].QuantityConfig[0]->quantityConfigEUTRA->filterCoefficientRSRP/4);
                  // LOG_N(RRC,"[UE %d] Frame %d : check proper operation in abstraction mode rsrp (%d), rx gain (%d) N_RB_DL (%d)\n",
                  //	ue_mod_idP,frameP,mac_xface->get_RSRP(ue_mod_idP,0,eNB_offset),mac_xface->get_rx_total_gain_dB(ue_mod_idP,0),mac_xface->lte_frame_parms->N_RB_DL);
                  UE_rrc_inst[ue_mod_idP].rsrp_db[eNB_offset] = (dB_fixed_times10(mac_xface->get_RSRP(ue_mod_idP,0,eNB_offset))/10.0)-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0)-dB_fixed(mac_xface->lte_frame_parms->N_RB_DL*12);
                  UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset] = (1.0-a)*UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset] +  a*UE_rrc_inst[ue_mod_idP].rsrp_db[eNB_offset];
                  //mac_xface->set_RSRP_filtered(ue_mod_idP,eNB_offset,UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]);


                  //LOG_D(RRC,"RSRP_total_dB: %3.2f \n",(dB_fixed_times10(mac_xface->get_RSRP(ue_mod_idP,0,eNB_offset))/10.0)-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0)-dB_fixed(mac_xface->lte_frame_parms->N_RB_DL*12));

                  LOG_D(RRC,"RSRP_dBm: %3.2f \n",(dB_fixed_times10(mac_xface->get_RSRP(ue_mod_idP,0,eNB_offset))/10.0));
                  LOG_D(RRC,"gain_loss_dB: %d \n",mac_xface->get_rx_total_gain_dB(ue_mod_idP,0));
                  LOG_D(RRC,"gain_fixed_dB: %d \n",dB_fixed(mac_xface->lte_frame_parms->N_RB_DL*12));
                  LOG_D(PHY,"[UE %d] Frame %d, RRC Measurements => rssi %3.1f dBm (digital: %3.1f dB)\n",
              			ue_mod_idP, frameP,	10*log10(mac_xface->get_RSSI(ue_mod_idP,0))-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0),10*log10(mac_xface->get_RSSI(ue_mod_idP,0)));
                  LOG_D(RRC,"[UE %d] Frame %d: Meas RSRP: eNB_offset: %d rsrp_coef: %3.1f filter_coef: %d before L3 filtering: rsrp: %3.1f after L3 filtering: rsrp: %3.1f \n ",
                      ue_mod_idP, frameP, eNB_offset,a,
                      *UE_rrc_inst->QuantityConfig[0]->quantityConfigEUTRA->filterCoefficientRSRP,
                      UE_rrc_inst[ue_mod_idP].rsrp_db[eNB_offset],
                      UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]);
              }
          }
      }
      else {
          for (eNB_offset = 0;eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0);eNB_offset++) {
              UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]= mac_xface->get_RSRP(ue_mod_idP,0,eNB_offset);
              // phy_vars_ue->PHY_measurements.rsrp_filtered[eNB_offset]=UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset];
              //mac_xface->set_RSRP_filtered(ue_mod_idP,eNB_offset,UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]);
          }
      }
      if (UE_rrc_inst[ue_mod_idP].QuantityConfig[0]->quantityConfigEUTRA != NULL) {
          if(UE_rrc_inst[ue_mod_idP].QuantityConfig[0]->quantityConfigEUTRA->filterCoefficientRSRQ != NULL) {
              for (eNB_offset = 0;eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0);eNB_offset++) {
                  // LOG_N(RRC,"[UE %d] Frame %d : check if this operation workes properly in abstraction mode\n",ue_mod_idP,frameP);
                  UE_rrc_inst[ue_mod_idP].rsrq_db[eNB_offset] = (10*log10(mac_xface->get_RSRQ(ue_mod_idP,0,eNB_offset)))-20;
                  UE_rrc_inst[ue_mod_idP].rsrq_db_filtered[eNB_offset]=(1-a1)*UE_rrc_inst[ue_mod_idP].rsrq_db_filtered[eNB_offset] + a1 *UE_rrc_inst[ue_mod_idP].rsrq_db[eNB_offset];
                  //mac_xface->set_RSRP_filtered(ue_mod_idP,eNB_offset,UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]);
                  /*
          LOG_D(RRC,"[UE %d] Frame %d: Meas RSRQ: eNB_offset: %d rsrq_coef: %3.2f filter_coef: %d before L3 filtering: rsrq: %3.1f after L3 filtering: rsrq: %3.1f \n ",
            ue_mod_idP,frameP,eNB_offset,a1,
                   *UE_rrc_inst->QuantityConfig[0]->quantityConfigEUTRA->filterCoefficientRSRQ,
            mac_xface->get_RSRQ(ue_mod_idP,0,eNB_offset),
            UE_rrc_inst[ue_mod_idP].rsrq_db[eNB_offset],
            UE_rrc_inst[ue_mod_idP].rsrq_db_filtered[eNB_offset]);
                   */
              }
          }
      }
      else{
          for (eNB_offset = 0;eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0);eNB_offset++) {
              UE_rrc_inst[ue_mod_idP].rsrq_db_filtered[eNB_offset]= mac_xface->get_RSRQ(ue_mod_idP,0,eNB_offset);
          }
      }
  }
}

//Below routine implements Measurement Reporting procedure from 36.331 Section 5.5.5
void rrc_ue_generate_MeasurementReport(module_id_t eNB_id, module_id_t UE_id, frame_t frameP) {

  uint8_t             buffer[32], size;
  uint8_t             i;
  uint8_t             target_eNB_offset;
  MeasId_t         measId;
  PhysCellId_t     cellId, targetCellId;
  long             rsrp_t,rsrq_t;
  long             rsrp_s,rsrq_s;
  long             nElem, nElem1;
  float            rsrp_filtered, rsrq_filtered;
  static frame_t   pframe=0;
  int              result;



  nElem = 98;
  nElem1 = 35;
  target_eNB_offset = UE_rrc_inst[UE_id].Info[0].handoverTarget; // eNB_offset of target eNB: used to obtain the mod_id of target eNB

  for (i=0;i<MAX_MEAS_ID;i++) {
      if (UE_rrc_inst[UE_id].measReportList[0][i] != NULL) {
          measId = UE_rrc_inst[UE_id].measReportList[0][i]->measId;

          // Note: Values in the meas report have to be the mapped values...to implement binary search for LUT
          rsrp_filtered = UE_rrc_inst[UE_id].rsrp_db_filtered[eNB_id];//nid_cell];
          rsrp_s = binary_search_float(RSRP_meas_mapping,nElem, rsrp_filtered);

          rsrq_filtered = UE_rrc_inst[UE_id].rsrq_db_filtered[eNB_id];//nid_cell]; //RSRQ of serving cell
          rsrq_s = binary_search_float(RSRQ_meas_mapping,nElem1,rsrq_filtered);//mapped RSRQ of serving cell

          LOG_D(RRC,"[UE %d] Frame %d: source eNB %d :rsrp_s: %ld rsrq_s: %ld rsrp_filtered: %f rsrq_filtered: %f \n",
              UE_id, frameP, eNB_id, rsrp_s,rsrq_s,rsrp_filtered,rsrq_filtered);

          rsrp_t = binary_search_float(RSRP_meas_mapping,nElem,UE_rrc_inst[UE_id].rsrp_db_filtered[target_eNB_offset]); //RSRP of target cell
          rsrq_t = binary_search_float(RSRQ_meas_mapping,nElem1,UE_rrc_inst[UE_id].rsrq_db_filtered[target_eNB_offset]); //RSRQ of target cell

          LOG_D(RRC,"[UE %d] Frame %d: target eNB %d :rsrp_t: %ld rsrq_t: %ld rsrp_filtered: %f rsrq_filtered: %f \n",
              UE_id, frameP, target_eNB_offset, rsrp_t,rsrq_t,UE_rrc_inst[UE_id].rsrp_db_filtered[target_eNB_offset],UE_rrc_inst[UE_id].rsrq_db_filtered[target_eNB_offset]);

          //  if (measFlag == 1) {
          cellId = get_adjacent_cell_id(UE_id, eNB_id); //PhycellId of serving cell
          targetCellId = UE_rrc_inst[UE_id].HandoverInfoUe.targetCellId ;//get_adjacent_cell_id(ue_mod_idP,target_eNB_offset); //PhycellId of target cell

          if (pframe!=frameP){
              pframe=frameP;
              LOG_D(RRC, "[UE %d] Frame %d: doing MeasReport: servingCell(%d) targetCell(%d) rsrp_s(%ld) rsrq_s(%ld) rsrp_t(%ld) rsrq_t(%ld) \n",
                  UE_id, frameP, cellId,targetCellId,rsrp_s,rsrq_s,rsrp_t,rsrq_t);
	      size = do_MeasurementReport(UE_id, buffer,measId,targetCellId,rsrp_s,rsrq_s,rsrp_t,rsrq_t);
              
              LOG_I(RRC, "[UE %d] Frame %d : Generating Measurement Report for eNB %d\n", UE_id, frameP, eNB_id);
              LOG_D(RLC, "[MSC_MSG][FRAME %05d][RRC_UE][UE %02d][][--- PDCP_DATA_REQ/%d Bytes (MeasurementReport to eNB %d MUI %d) --->][PDCP][MOD %02d][RB %02d]\n",
                  frameP, UE_id, size, eNB_id, rrc_mui, eNB_id, DCCH);
              result = pdcp_data_req(eNB_id, UE_id, frameP, ENB_FLAG_NO,  SRB_FLAG_YES, DCCH, rrc_mui++, 0, size, buffer, PDCP_TRANSMISSION_MODE_DATA);
              AssertFatal (result == TRUE, "PDCP data request failed!\n");
              //LOG_D(RRC, "[UE %d] Frame %d Sending MeasReport (%d bytes) through DCCH%d to PDCP \n",ue_mod_idP,frameP, size, DCCH);
          }
          //          measFlag = 0; //re-setting measFlag so that no more MeasReports are sent in this frameP
          //          }
      }
  }
}

// Measurement report triggering, described in 36.331 Section 5.5.4.1: called periodically 
void ue_measurement_report_triggering(module_id_t ue_mod_idP, frame_t frameP,uint8_t eNB_index) {
  uint8_t               i,j;
  Hysteresis_t     hys;
  TimeToTrigger_t  ttt_ms;
  Q_OffsetRange_t  ofn;
  Q_OffsetRange_t  ocn;
  Q_OffsetRange_t  ofs = 0;
  Q_OffsetRange_t  ocs = 0;
  long             a3_offset;
  MeasObjectId_t   measObjId;
  ReportConfigId_t reportConfigId;

  for(i=0 ; i<NB_CNX_UE ; i++) {
      for(j=0 ; j<MAX_MEAS_ID ; j++) {
          if(UE_rrc_inst[ue_mod_idP].MeasId[i][j] != NULL) {
              measObjId = UE_rrc_inst[ue_mod_idP].MeasId[i][j]->measObjectId;
              reportConfigId = UE_rrc_inst[ue_mod_idP].MeasId[i][j]->reportConfigId;
              if( /*UE_rrc_inst[ue_mod_idP].MeasId[i][j] != NULL && */ UE_rrc_inst[ue_mod_idP].MeasObj[i][measObjId-1] != NULL) {
                  if(UE_rrc_inst[ue_mod_idP].MeasObj[i][measObjId-1]->measObject.present == MeasObjectToAddMod__measObject_PR_measObjectEUTRA) {
                      /* consider any neighboring cell detected on the associated frequency to be
                       * applicable when the concerned cell is not included in the blackCellsToAddModList
                       * defined within the VarMeasConfig for this measId */
                      //    LOG_I(RRC,"event %d %d %p \n", measObjId,reportConfigId, UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]);
                      if((UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1] != NULL) &&
                          (UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.present==ReportConfigToAddMod__reportConfig_PR_reportConfigEUTRA) &&
                          (UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.choice.reportConfigEUTRA.triggerType.present == ReportConfigEUTRA__triggerType_PR_event)) {
                          hys = UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.hysteresis;
                          //LOG_N(RRC,"[UE%d] Frame %d Check below lines for segfault :), Fix me \n",ue_mod_idP, frameP);
                          ttt_ms = timeToTrigger_ms[UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.timeToTrigger];
                          // Freq specific offset of neighbor cell freq
                          ofn = 5;//((UE_rrc_inst[ue_mod_idP].MeasObj[i][measObjId-1]->measObject.choice.measObjectEUTRA.offsetFreq != NULL) ?
                             // *UE_rrc_inst[ue_mod_idP].MeasObj[i][measObjId-1]->measObject.choice.measObjectEUTRA.offsetFreq : 15); //  /* 15 is the Default */
                          // cellIndividualOffset of neighbor cell - not defined yet
                          ocn = 0;
                          a3_offset = UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.choice.eventA3.a3_Offset;

                          switch (UE_rrc_inst[ue_mod_idP].ReportConfig[i][reportConfigId-1]->reportConfig.choice.reportConfigEUTRA.triggerType.choice.event.eventId.present) {
                          case ReportConfigEUTRA__triggerType__event__eventId_PR_eventA1:
                            LOG_D(RRC,"[UE %d] Frame %d : A1 event: check if serving becomes better than threshold\n",ue_mod_idP, frameP);
                            break;
                          case ReportConfigEUTRA__triggerType__event__eventId_PR_eventA2:
                            LOG_D(RRC,"[UE %d] Frame %d : A2 event, check if serving becomes worse than a threshold\n",ue_mod_idP, frameP);
                            break;
                          case ReportConfigEUTRA__triggerType__event__eventId_PR_eventA3:
                            LOG_D(RRC,"[UE %d] Frame %d : A3 event: check if a neighboring cell becomes offset better than serving to trigger a measurement event \n",ue_mod_idP, frameP);
                            if ((check_trigger_meas_event(ue_mod_idP,frameP,eNB_index,i,j,ofn,ocn,hys,ofs,ocs,a3_offset,ttt_ms)) &&
                                (UE_rrc_inst[ue_mod_idP].Info[0].State >= RRC_CONNECTED) &&
                                (UE_rrc_inst[ue_mod_idP].Info[0].T304_active == 0 )      &&
                                (UE_rrc_inst[ue_mod_idP].HandoverInfoUe.measFlag == 1)) {
                                //trigger measurement reporting procedure (36.331, section 5.5.5)
                                if (UE_rrc_inst[ue_mod_idP].measReportList[i][j] == NULL) {
                                    UE_rrc_inst[ue_mod_idP].measReportList[i][j] = malloc(sizeof(MEAS_REPORT_LIST));
                                }
                                UE_rrc_inst[ue_mod_idP].measReportList[i][j]->measId = UE_rrc_inst[ue_mod_idP].MeasId[i][j]->measId;
                                UE_rrc_inst[ue_mod_idP].measReportList[i][j]->numberOfReportsSent = 0;
                                rrc_ue_generate_MeasurementReport(eNB_index, ue_mod_idP,frameP);
                                UE_rrc_inst[ue_mod_idP].HandoverInfoUe.measFlag = 1;
                                LOG_I(RRC,"[UE %d] Frame %d: A3 event detected, state: %d \n", ue_mod_idP, frameP, UE_rrc_inst[ue_mod_idP].Info[0].State);
                            }
                            else {
                                if(UE_rrc_inst[ue_mod_idP].measReportList[i][j] != NULL){
                                    free(UE_rrc_inst[ue_mod_idP].measReportList[i][j]);
                                }
                                UE_rrc_inst[ue_mod_idP].measReportList[i][j] = NULL;
                            }
                            break;
                          case ReportConfigEUTRA__triggerType__event__eventId_PR_eventA4:
                            LOG_D(RRC,"[UE %d] Frame %d : received an A4 event, neighbor becomes offset better than a threshold\n",ue_mod_idP, frameP);
                            break;
                          case ReportConfigEUTRA__triggerType__event__eventId_PR_eventA5:
                            LOG_D(RRC,"[UE %d] Frame %d: received an A5 event, serving becomes worse than threshold 1 and neighbor becomes better than threshold 2\n",ue_mod_idP, frameP);
                            break;
                          default:
                            LOG_D(RRC,"Invalid ReportConfigEUTRA__triggerType__event__eventId: %d",
                                UE_rrc_inst[ue_mod_idP].ReportConfig[i][j]->reportConfig.choice.reportConfigEUTRA.triggerType.present);
                            break;
                          }
                      }
                  }
              }
          }
      }
  }
}

//check_trigger_meas_event(ue_mod_idP, frameP, eNB_index, i,j,ofn,ocn,hys,ofs,ocs,a3_offset,ttt_ms)
uint8_t check_trigger_meas_event(module_id_t ue_mod_idP,frame_t frameP, uint8_t eNB_index, uint8_t ue_cnx_index, uint8_t meas_index,
    Q_OffsetRange_t ofn, Q_OffsetRange_t ocn, Hysteresis_t hys,
    Q_OffsetRange_t ofs, Q_OffsetRange_t ocs, long a3_offset, TimeToTrigger_t ttt) {
  uint8_t eNB_offset;
  uint8_t currentCellIndex = mac_xface->lte_frame_parms->Nid_cell;
  uint8_t tmp_offset;
  LOG_I(RRC,"[UE %d] ofn(%d) ocn(%d) hys(%d) ofs(%d) ocs(%d) a3_offset(%d) ttt(%d) rssi %3.1f\n",
	ue_mod_idP, 
	ofn,ocn,hys,ofs,ocs,a3_offset,ttt,
	10*log10(mac_xface->get_RSSI(ue_mod_idP,0))-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0));
  for (eNB_offset = 0;eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0);eNB_offset++) {
  //for (eNB_offset = 1;(eNB_offset<1+mac_xface->get_n_adj_cells(ue_mod_idP,0));eNB_offset++) {
      /* RHS: Verify that idx 0 corresponds to currentCellIndex in rsrp array */
	  if((eNB_offset!=eNB_index)&&(eNB_offset<NB_eNB_INST)){
	    if(eNB_offset<eNB_index){
	      tmp_offset = eNB_offset;
	    }
	    else {
	      tmp_offset = eNB_offset-1;
	    }
	    if(UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_offset]+ofn+ocn-hys > UE_rrc_inst[ue_mod_idP].rsrp_db_filtered[eNB_index]+ofs+ocs-1/*+a3_offset*/) {
	      UE_rrc_inst->measTimer[ue_cnx_index][meas_index][tmp_offset] += 2; //Called every subframe = 2ms
	      LOG_D(RRC,"[UE %d] Frame %d: Entry measTimer[%d][%d][%d]: %d currentCell: %d betterCell: %d \n",
		    ue_mod_idP, frameP, ue_cnx_index,meas_index,tmp_offset,UE_rrc_inst->measTimer[ue_cnx_index][meas_index][tmp_offset],currentCellIndex,eNB_offset);
	    }
	    else {
	      UE_rrc_inst->measTimer[ue_cnx_index][meas_index][tmp_offset] = 0; //Exit condition: Resetting the measurement timer
	      LOG_D(RRC,"[UE %d] Frame %d: Exit measTimer[%d][%d][%d]: %d currentCell: %d betterCell: %d \n",
		    ue_mod_idP, frameP, ue_cnx_index,meas_index,tmp_offset,UE_rrc_inst->measTimer[ue_cnx_index][meas_index][tmp_offset],currentCellIndex,eNB_offset);
	    }
	    if (UE_rrc_inst->measTimer[ue_cnx_index][meas_index][tmp_offset] >= ttt) {
	      UE_rrc_inst->HandoverInfoUe.targetCellId = get_adjacent_cell_id(ue_mod_idP,tmp_offset); //WARNING!!!...check this!
	      LOG_D(RRC,"[UE %d] Frame %d eNB %d: Handover triggered: targetCellId: %d currentCellId: %d eNB_offset: %d rsrp source: %3.1f rsrp target: %3.1f\n", \
		    ue_mod_idP, frameP, eNB_index,
		    UE_rrc_inst->HandoverInfoUe.targetCellId,ue_cnx_index,eNB_offset,
		    (dB_fixed_times10(UE_rrc_inst[ue_mod_idP].rsrp_db[0])/10.0)-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0)-dB_fixed(mac_xface->lte_frame_parms->N_RB_DL*12),
		    (dB_fixed_times10(UE_rrc_inst[ue_mod_idP].rsrp_db[eNB_offset])/10.0)-mac_xface->get_rx_total_gain_dB(ue_mod_idP,0)-dB_fixed(mac_xface->lte_frame_parms->N_RB_DL*12));
	      UE_rrc_inst->Info[0].handoverTarget = eNB_offset;
	      //LOG_D(RRC,"PHY_ID: %d \n",UE_rrc_inst->HandoverInfoUe.targetCellId);
	      return 1;
	    }
	    // else{
	    //	LOG_D(RRC,"Condition does not hold\n");
	    // }
	  }
  }
  return 0;
}

#ifdef Rel10
int decode_MCCH_Message(module_id_t ue_mod_idP, frame_t frameP, uint8_t eNB_index, uint8_t *Sdu, uint8_t Sdu_len,uint8_t mbsfn_sync_area) {

  MCCH_Message_t *mcch=NULL;
  MBSFNAreaConfiguration_r9_t **mcch_message=&UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index];
  asn_dec_rval_t dec_rval;

  if (UE_rrc_inst[ue_mod_idP].Info[eNB_index].MCCHStatus[mbsfn_sync_area] == 1) {
      LOG_D(RRC,"[UE %d] Frame %d: MCCH MESSAGE for MBSFN sync area %d has been already received!\n",
          ue_mod_idP, frameP, mbsfn_sync_area);
      return 0; // avoid decoding to prevent memory bloating
  }
  else {
      dec_rval = uper_decode_complete(NULL,
          &asn_DEF_MCCH_Message,
          (void **)&mcch,
          (const void *)Sdu,
          Sdu_len);
      if ((dec_rval.code != RC_OK) && (dec_rval.consumed==0)) {
          LOG_E(RRC,"[UE %d] Failed to decode MCCH__MESSAGE (%d bits)\n",ue_mod_idP,dec_rval.consumed);
          //free the memory
          SEQUENCE_free(&asn_DEF_MCCH_Message, (void*)mcch, 1);
          return -1;
      }
#ifdef XER_PRINT
      xer_fprint(stdout, &asn_DEF_MCCH_Message, (void*)mcch);
#endif

      if (mcch->message.present == MCCH_MessageType_PR_c1) {
          LOG_D(RRC,"[UE %d] Found mcch message \n",ue_mod_idP);
          if(mcch->message.choice.c1.present == MCCH_MessageType__c1_PR_mbsfnAreaConfiguration_r9) {
              /*
        memcpy((void*)*mcch_message,
               (void*)&mcch->message.choice.c1.choice.mbsfnAreaConfiguration_r9,
               sizeof(MBSFNAreaConfiguration_r9_t)); */
              *mcch_message = &mcch->message.choice.c1.choice.mbsfnAreaConfiguration_r9;
              LOG_I(RRC,"[UE %d] Frame %d : Found MBSFNAreaConfiguration from eNB %d \n",ue_mod_idP, frameP, eNB_index);
              decode_MBSFNAreaConfiguration(ue_mod_idP,eNB_index,frameP, mbsfn_sync_area);

          }
      }
  }
  return 0;
}

void decode_MBSFNAreaConfiguration(module_id_t ue_mod_idP, uint8_t eNB_index, frame_t frameP,uint8_t mbsfn_sync_area) {
  LOG_D(RRC,"[UE %d] Frame %d : Number of MCH(s) in the MBSFN Sync Area %d  is %d\n", 
      ue_mod_idP, frameP, mbsfn_sync_area, UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index]->pmch_InfoList_r9.list.count);
  //  store to MAC/PHY necessary parameters for receiving MTCHs
  rrc_mac_config_req(ue_mod_idP,ENB_FLAG_NO,0,eNB_index,
      (RadioResourceConfigCommonSIB_t *)NULL,
      (struct PhysicalConfigDedicated *)NULL,
#ifdef Rel10
	(SCellToAddMod_r10_t *)NULL,
	//(struct PhysicalConfigDedicatedSCell_r10 *)NULL,
#endif
      (MeasObjectToAddMod_t **)NULL,
      (MAC_MainConfig_t *)NULL,
      0,
      (struct LogicalChannelConfig *)NULL,
      (MeasGapConfig_t *)NULL,
      (TDD_Config_t *)NULL,
      (MobilityControlInfo_t *)NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      (MBSFN_SubframeConfigList_t *)NULL
#ifdef Rel10	       
,
0,
(MBSFN_AreaInfoList_r9_t *)NULL,
&UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index]->pmch_InfoList_r9
#endif
#ifdef CBA
,
0,
0
#endif
  );

  UE_rrc_inst[ue_mod_idP].Info[eNB_index].MCCHStatus[mbsfn_sync_area] = 1;

  // Config Radio Bearer for MBMS user data (similar way to configure for eNB side in init_MBMS function)
  rrc_pdcp_config_asn1_req(eNB_index, ue_mod_idP, frameP, 0,
      NULL, // SRB_ToAddModList
      NULL, // DRB_ToAddModList
      (DRB_ToReleaseList_t*)NULL,
      0, // security mode
      NULL, // key rrc encryption
      NULL, // key rrc integrity
      NULL // key encryption
#ifdef Rel10
      ,&(UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index]->pmch_InfoList_r9)
#endif
  );

  rrc_rlc_config_asn1_req(eNB_index, ue_mod_idP, frameP,0,
      NULL,// SRB_ToAddModList
      NULL,// DRB_ToAddModList
      NULL,// DRB_ToReleaseList
#ifdef Rel10
      &(UE_rrc_inst[ue_mod_idP].mcch_message[eNB_index]->pmch_InfoList_r9)
#endif 
  );
  // */

}

#endif // rel10

#ifndef USER_MODE
EXPORT_SYMBOL(Rlc_info_am_config);
#endif

#if defined(ENABLE_ITTI)
void *rrc_ue_task(void *args_p) {
  MessageDef   *msg_p;
  const char   *msg_name;
  instance_t    instance;
  unsigned int  ue_mod_id;
  int           result;
  SRB_INFO     *srb_info_p;

  itti_mark_task_ready (TASK_RRC_UE);

  while(1) {
      // Wait for a message
      itti_receive_msg (TASK_RRC_UE, &msg_p);

      msg_name = ITTI_MSG_NAME (msg_p);
      instance = ITTI_MSG_INSTANCE (msg_p);
      ue_mod_id = instance - NB_eNB_INST;

      switch (ITTI_MSG_ID(msg_p)) {
      case TERMINATE_MESSAGE:
        itti_exit_task ();
        break;

      case MESSAGE_TEST:
        LOG_I(RRC, "[UE %d] Received %s\n", ue_mod_id, msg_name);
        break;

        /* MAC messages */
      case RRC_MAC_IN_SYNC_IND:
        LOG_D(RRC, "[UE %d] Received %s: frameP %d, eNB %d\n", ue_mod_id, msg_name,
            RRC_MAC_IN_SYNC_IND (msg_p).frame, RRC_MAC_IN_SYNC_IND (msg_p).enb_index);

        UE_rrc_inst[ue_mod_id].Info[RRC_MAC_IN_SYNC_IND (msg_p).enb_index].N310_cnt = 0;
        if (UE_rrc_inst[ue_mod_id].Info[RRC_MAC_IN_SYNC_IND (msg_p).enb_index].T310_active == 1)
          UE_rrc_inst[ue_mod_id].Info[RRC_MAC_IN_SYNC_IND (msg_p).enb_index].N311_cnt++;
        break;

      case RRC_MAC_OUT_OF_SYNC_IND:
        LOG_I(RRC, "[UE %d] Received %s: frameP %d, eNB %d\n", ue_mod_id, msg_name,
            RRC_MAC_OUT_OF_SYNC_IND (msg_p).frame, RRC_MAC_OUT_OF_SYNC_IND (msg_p).enb_index);

        UE_rrc_inst[ue_mod_id].Info[RRC_MAC_OUT_OF_SYNC_IND (msg_p).enb_index].N310_cnt ++;
        break;

      case RRC_MAC_BCCH_DATA_IND:
        LOG_D(RRC, "[UE %d] Received %s: frameP %d, eNB %d\n", ue_mod_id, msg_name,
            RRC_MAC_BCCH_DATA_IND (msg_p).frame, RRC_MAC_BCCH_DATA_IND (msg_p).enb_index);

        decode_BCCH_DLSCH_Message (ue_mod_id, RRC_MAC_BCCH_DATA_IND (msg_p).frame,
            RRC_MAC_BCCH_DATA_IND (msg_p).enb_index, RRC_MAC_BCCH_DATA_IND (msg_p).sdu,
            RRC_MAC_BCCH_DATA_IND (msg_p).sdu_size,
            RRC_MAC_BCCH_DATA_IND (msg_p).rsrq, RRC_MAC_BCCH_DATA_IND (msg_p).rsrp);
        break;

      case RRC_MAC_CCCH_DATA_CNF:
        LOG_I(RRC, "[UE %d] Received %s: eNB %d\n", ue_mod_id, msg_name,
            RRC_MAC_CCCH_DATA_CNF (msg_p).enb_index);

        // reset the tx buffer to indicate RRC that ccch was successfully transmitted (for example if contention resolution succeeds)
        UE_rrc_inst[ue_mod_id].Srb0[RRC_MAC_CCCH_DATA_CNF (msg_p).enb_index].Tx_buffer.payload_size = 0;
        break;

      case RRC_MAC_CCCH_DATA_IND:
        LOG_I(RRC, "[UE %d] Received %s: frameP %d, eNB %d\n", ue_mod_id, msg_name,
            RRC_MAC_CCCH_DATA_IND (msg_p).frame, RRC_MAC_CCCH_DATA_IND (msg_p).enb_index);

        srb_info_p = &UE_rrc_inst[ue_mod_id].Srb0[RRC_MAC_CCCH_DATA_IND (msg_p).enb_index];

        memcpy (srb_info_p->Rx_buffer.Payload, RRC_MAC_CCCH_DATA_IND (msg_p).sdu,
            RRC_MAC_CCCH_DATA_IND (msg_p).sdu_size);
        srb_info_p->Rx_buffer.payload_size = RRC_MAC_CCCH_DATA_IND (msg_p).sdu_size;
        rrc_ue_decode_ccch (ue_mod_id, RRC_MAC_CCCH_DATA_IND (msg_p).frame, srb_info_p,
            RRC_MAC_CCCH_DATA_IND (msg_p).enb_index);
        break;

# ifdef Rel10
      case RRC_MAC_MCCH_DATA_IND:
        LOG_I(RRC, "[UE %d] Received %s: frameP %d, eNB %d, mbsfn SA %d\n", ue_mod_id, msg_name,
            RRC_MAC_MCCH_DATA_IND (msg_p).frame, RRC_MAC_MCCH_DATA_IND (msg_p).enb_index, RRC_MAC_MCCH_DATA_IND (msg_p).mbsfn_sync_area);

        decode_MCCH_Message (ue_mod_id, RRC_MAC_MCCH_DATA_IND (msg_p).frame, RRC_MAC_MCCH_DATA_IND (msg_p).enb_index,
            RRC_MAC_MCCH_DATA_IND (msg_p).sdu, RRC_MAC_MCCH_DATA_IND (msg_p).sdu_size,
            RRC_MAC_MCCH_DATA_IND (msg_p).mbsfn_sync_area);
        break;
# endif

/* PDCP messages */
      case RRC_DCCH_DATA_IND:
        LOG_I(RRC, "[UE %d] Received %s: frameP %d, DCCH %d, eNB %d\n", ue_mod_id, msg_name,
            RRC_DCCH_DATA_IND (msg_p).frame, RRC_DCCH_DATA_IND (msg_p).dcch_index, RRC_DCCH_DATA_IND (msg_p).eNB_index);

        rrc_ue_decode_dcch (ue_mod_id, RRC_DCCH_DATA_IND (msg_p).frame,
            RRC_DCCH_DATA_IND (msg_p).dcch_index, RRC_DCCH_DATA_IND (msg_p).sdu_p,
            RRC_DCCH_DATA_IND (msg_p).eNB_index);

        // Message buffer has been processed, free it now.
        result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), RRC_DCCH_DATA_IND (msg_p).sdu_p);
        AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
        break;

# if defined(ENABLE_USE_MME)
        /* NAS messages */
      case NAS_CELL_SELECTION_REQ:
        ue_mod_id = 0; /* TODO force ue_mod_id to first UE, NAS UE not virtualized yet */

        LOG_I(RRC, "[UE %d] Received %s: state %d, plmnID %d, rat %x\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id),
            NAS_CELL_SELECTION_REQ (msg_p).plmnID, NAS_CELL_SELECTION_REQ (msg_p).rat);

        if (rrc_get_state(ue_mod_id) == RRC_STATE_INACTIVE) {
            // have a look at MAC/main.c void dl_phy_sync_success(...)
            openair_rrc_ue_init(ue_mod_id,0);
        }

        /* Save cell selection criterion */
        {
          UE_rrc_inst[ue_mod_id].plmnID = NAS_CELL_SELECTION_REQ (msg_p).plmnID;
          UE_rrc_inst[ue_mod_id].rat = NAS_CELL_SELECTION_REQ (msg_p).rat;
        }

        switch (rrc_get_state(ue_mod_id)) {
        case RRC_STATE_INACTIVE:
          {
            /* Need to first activate lower layers */
            MessageDef *message_p;

            message_p = itti_alloc_new_message(TASK_RRC_UE, ACTIVATE_MESSAGE);

            itti_send_msg_to_task(TASK_L2L1, NB_eNB_INST + ue_mod_id, message_p);

            rrc_set_state (ue_mod_id, RRC_STATE_IDLE);
            /* Fall through to next case */
          }

        case RRC_STATE_IDLE:
          {
            /* Ask to layer 1 to find a cell matching the criterion */
            MessageDef *message_p;

            message_p = itti_alloc_new_message(TASK_RRC_UE, PHY_FIND_CELL_REQ);

            PHY_FIND_CELL_REQ (message_p).earfcn_start = 1;
            PHY_FIND_CELL_REQ (message_p).earfcn_end = 1;

            itti_send_msg_to_task(TASK_PHY_UE, NB_eNB_INST + ue_mod_id, message_p);
            rrc_set_sub_state (ue_mod_id, RRC_SUB_STATE_IDLE_SEARCHING);

            break;
          }

        case RRC_STATE_CONNECTED:
          /* should not happen */
          LOG_E(RRC, "[UE %d] request %s in RRC state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
          break;

        default:
          LOG_C(RRC, "[UE %d] Invalid RRC state %d\n", ue_mod_id, rrc_get_state(ue_mod_id));
          break;
        }
        break;

        case NAS_CONN_ESTABLI_REQ:
          LOG_I(RRC, "[UE %d] Received %s: cause %d, type %d, s_tmsi %d, plmnID %d\n", ue_mod_id, msg_name, NAS_CONN_ESTABLI_REQ (msg_p).cause,
              NAS_CONN_ESTABLI_REQ (msg_p).type, NAS_CONN_ESTABLI_REQ (msg_p).s_tmsi, NAS_CONN_ESTABLI_REQ (msg_p).plmnID);

          UE_rrc_inst[ue_mod_id].initialNasMsg = NAS_CONN_ESTABLI_REQ (msg_p).initialNasMsg;

          switch (rrc_get_state(ue_mod_id)) {
          case RRC_STATE_IDLE:
            {
              if (rrc_get_sub_state(ue_mod_id) == RRC_SUB_STATE_IDLE_SIB_COMPLETE)
                {
                  rrc_ue_generate_RRCConnectionRequest(ue_mod_id, 0 /* TODO put frameP number ! */, 0);
                  LOG_I(RRC, "not sending connection request\n");

                  rrc_set_sub_state (ue_mod_id, RRC_SUB_STATE_IDLE_CONNECTING);
                }
              break;
            }

          case RRC_STATE_INACTIVE:
          case RRC_STATE_CONNECTED:
            /* should not happen */
            LOG_E(RRC, "[UE %d] request %s in RRC state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
            break;

          default:
            LOG_C(RRC, "[UE %d] Invalid RRC state %d\n", ue_mod_id, rrc_get_state(ue_mod_id));
            break;
          }
          break;

          case NAS_UPLINK_DATA_REQ:
            {
              uint32_t length;
              uint8_t *buffer;

              LOG_I(RRC, "[UE %d] Received %s: UEid %d\n", ue_mod_id, msg_name, NAS_UPLINK_DATA_REQ (msg_p).UEid);

              /* Create message for PDCP (ULInformationTransfer_t) */
              length = do_ULInformationTransfer(&buffer, NAS_UPLINK_DATA_REQ (msg_p).nasMsg.length, NAS_UPLINK_DATA_REQ (msg_p).nasMsg.data);

              /* Transfer data to PDCP */
              pdcp_rrc_data_req (0, ue_mod_id, 0 /* TODO put frameP number ! */, 0, DCCH, rrc_mui++, 0, length, buffer, 1);
              break;
            }
# endif

# if defined(ENABLE_RAL)
          case RRC_RAL_SCAN_REQ:
            LOG_I(RRC, "[UE %d] Received %s: state %d\n", ue_mod_id, msg_name);

            switch (rrc_get_state(ue_mod_id)) {
            case RRC_STATE_INACTIVE:
              {
                /* Need to first activate lower layers */
                MessageDef *message_p;

                message_p = itti_alloc_new_message(TASK_RRC_UE, ACTIVATE_MESSAGE);

                itti_send_msg_to_task(TASK_L2L1, instance, message_p);

                rrc_set_state (ue_mod_id, RRC_STATE_IDLE);
                /* Fall through to next case */
              }

            case RRC_STATE_IDLE:
              {
                if (rrc_get_sub_state(ue_mod_id) != RRC_SUB_STATE_IDLE_SEARCHING) {
                    /* Ask to layer 1 to find a cell matching the criterion */
                    MessageDef *message_p;

                    message_p = itti_alloc_new_message(TASK_RRC_UE, PHY_FIND_CELL_REQ);

                    rrc_set_sub_state (ue_mod_id, RRC_SUB_STATE_IDLE_SEARCHING);

                    PHY_FIND_CELL_REQ (message_p).transaction_id = RRC_RAL_SCAN_REQ (msg_p).transaction_id;
                    PHY_FIND_CELL_REQ (message_p).earfcn_start   = 1;
                    PHY_FIND_CELL_REQ (message_p).earfcn_end     = 1; //44

                    itti_send_msg_to_task(TASK_PHY_UE, instance, message_p);
                }
                break;
              }

            case RRC_STATE_CONNECTED:
              /* should not happen */
              LOG_E(RRC, "[UE %d] request %s in RRC state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
              break;

            default:
              LOG_C(RRC, "[UE %d] Invalid RRC state %d\n", ue_mod_id, rrc_get_state(ue_mod_id));
              break;
            }
            break;

            case PHY_FIND_CELL_IND:
              LOG_I(RRC, "[UE %d] Received %s: state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
              switch (rrc_get_state(ue_mod_id)) {
              case RRC_STATE_IDLE:
                switch (rrc_get_sub_state(ue_mod_id)) {
                case RRC_SUB_STATE_IDLE_SEARCHING:
                  {
                    MessageDef *message_p;
                    int         i;

                    message_p = itti_alloc_new_message(TASK_RRC_UE, RRC_RAL_SCAN_CONF);

                    RRC_RAL_SCAN_CONF (message_p).transaction_id = PHY_FIND_CELL_IND(msg_p).transaction_id;
                    RRC_RAL_SCAN_CONF (message_p).num_scan_resp  = PHY_FIND_CELL_IND(msg_p).cell_nb;
                    for (i = 0 ; i < PHY_FIND_CELL_IND(msg_p).cell_nb; i++) {
                        // TO DO
                        memset(&RRC_RAL_SCAN_CONF (message_p).link_scan_resp[i].link_addr,  0, sizeof(ral_link_addr_t));
                        // TO DO
                        memset(&RRC_RAL_SCAN_CONF (message_p).link_scan_resp[i].network_id, 0, sizeof(ral_network_id_t));

                        RRC_RAL_SCAN_CONF (message_p).link_scan_resp[i].sig_strength.choice     = RAL_SIG_STRENGTH_CHOICE_DBM;
                        RRC_RAL_SCAN_CONF (message_p).link_scan_resp[i].sig_strength._union.dbm = PHY_FIND_CELL_IND(msg_p).cells[i].rsrp;
                    }

                    rrc_set_sub_state (ue_mod_id, RRC_SUB_STATE_IDLE);

                    itti_send_msg_to_task(TASK_RAL_UE, instance, message_p);
                    break;
                  }

                default:
                  LOG_C(RRC, "[UE %d] Invalid RRC state %d substate %d\n",
                      ue_mod_id,
                      rrc_get_state(ue_mod_id),
                      rrc_get_sub_state(ue_mod_id));
                }
                break;

                case RRC_STATE_INACTIVE:
                case RRC_STATE_CONNECTED:
                  /* should not happen */
                  LOG_E(RRC, "[UE %d] indication %s in RRC state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
                  break;

                default:
                  LOG_C(RRC, "[UE %d] Invalid RRC state %d\n", ue_mod_id, rrc_get_state(ue_mod_id));
                  break;
              }
              break; // PHY_FIND_CELL_IND

              case PHY_MEAS_REPORT_IND:
                {
                  MessageDef *message_p;
                  message_p = itti_alloc_new_message(TASK_RRC_UE, RRC_RAL_MEASUREMENT_REPORT_IND);

                  memcpy(&RRC_RAL_MEASUREMENT_REPORT_IND (message_p).threshold,
                      &PHY_MEAS_REPORT_IND(msg_p).threshold,
                      sizeof(RRC_RAL_MEASUREMENT_REPORT_IND (message_p).threshold));

                  memcpy(&RRC_RAL_MEASUREMENT_REPORT_IND (message_p).link_param,
                      &PHY_MEAS_REPORT_IND(msg_p).link_param,
                      sizeof(RRC_RAL_MEASUREMENT_REPORT_IND (message_p).link_param));
		
		  LOG_I(RRC, "[UE %d] PHY_MEAS_REPORT_IN: sending msg %s to %s \n", ue_mod_id, "RRC_RAL_MEASUREMENT_REPORT_IND", "TASK_RAL_UE");
                  itti_send_msg_to_task(TASK_RAL_UE, instance, message_p);
                  break;
                }

              case RRC_RAL_CONFIGURE_THRESHOLD_REQ:
                rrc_ue_ral_handle_configure_threshold_request(ue_mod_id, msg_p);
                break;

              case RRC_RAL_CONNECTION_ESTABLISHMENT_REQ:
                LOG_I(RRC, "[UE %d] Received %s\n", ue_mod_id, msg_name);
                switch (rrc_get_state(ue_mod_id)) {
                case RRC_STATE_IDLE:
                  {
                    if (rrc_get_sub_state(ue_mod_id) == RRC_SUB_STATE_IDLE_SIB_COMPLETE)
                      {
                        rrc_ue_generate_RRCConnectionRequest(ue_mod_id, 0 /* TODO put frameP number ! */, 0);
                        LOG_I(RRC, "not sending connection request\n");

                        rrc_set_sub_state (ue_mod_id, RRC_SUB_STATE_IDLE_CONNECTING);
                      }
                    break;
                  }

                case RRC_STATE_INACTIVE:
                case RRC_STATE_CONNECTED:
                  /* should not happen */
                  LOG_E(RRC, "[UE %d] request %s in RRC state %d\n", ue_mod_id, msg_name, rrc_get_state(ue_mod_id));
                  break;

                default:
                  LOG_C(RRC, "[UE %d] Invalid RRC state %d\n", ue_mod_id, rrc_get_state(ue_mod_id));
                  break;
                }
                break;

                case RRC_RAL_CONNECTION_RELEASE_REQ:
                  ue_mod_id = 0; /* TODO force ue_mod_id to first UE, NAS UE not virtualized yet */
                  LOG_I(RRC, "[UE %d] Received %s\n", ue_mod_id, msg_name);
                  break;
#endif

                default:
                  LOG_E(RRC, "[UE %d] Received unexpected message %s\n", ue_mod_id, msg_name);
                  break;
      }

      result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
      AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
      AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
      msg_p = NULL;
  }
}
#endif
