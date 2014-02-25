/*
                                rlc_rrc.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMlAIL   : Lionel.Gauthier@eurecom.fr
*/

#define RLC_RRC_C
#include "rlc.h"
#include "rlc_am.h"
#include "rlc_um.h"
#include "rlc_tm.h"
#include "UTIL/LOG/log.h"
#ifdef OAI_EMU
#include "UTIL/OCG/OCG_vars.h"
#endif
#include "RLC-Config.h"
#include "DRB-ToAddMod.h"
#include "DRB-ToAddModList.h"
#include "SRB-ToAddMod.h"
#include "SRB-ToAddModList.h"
#include "DL-UM-RLC.h"
#ifdef Rel10
#include "PMCH-InfoList-r9.h"
#endif

#include "LAYER2/MAC/extern.h"
#include "assertions.h"
//-----------------------------------------------------------------------------
#ifdef Rel10
rlc_op_status_t rrc_rlc_config_asn1_req (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, SRB_ToAddModList_t* srb2add_listP, DRB_ToAddModList_t* drb2add_listP, DRB_ToReleaseList_t*  drb2release_listP, PMCH_InfoList_r9_t *pmch_info_listP) {
#else
rlc_op_status_t rrc_rlc_config_asn1_req (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, SRB_ToAddModList_t* srb2add_listP, DRB_ToAddModList_t* drb2add_listP, DRB_ToReleaseList_t*  drb2release_listP) {
#endif//-----------------------------------------------------------------------------
  rb_id_t                rb_id        = 0;
  logical_chan_id_t              lc_id        = 0;
  DRB_Identity_t         drb_id       = 0;
  DRB_Identity_t*        pdrb_id      = NULL;
  long int               cnt          = 0;
  SRB_ToAddMod_t        *srb_toaddmod = NULL;
  DRB_ToAddMod_t        *drb_toaddmod = NULL;
  rlc_mode_t             rlc_mode     = RLC_NONE;
#ifdef Rel10
  long int               cnt2            = 0;
  //  long int               mrb_id          = 0;
  long int               mbms_service_id = 0;
  // long int               mbms_session_id = 0;
  PMCH_Info_r9_t*        pmch_info_r9    = NULL;
  MBMS_SessionInfo_r9_t* mbms_session    = NULL;
  rlc_op_status_t        rlc_status      = RLC_OP_STATUS_OK;
  DL_UM_RLC_t            dl_um_rlc;
#endif
  
  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] CONFIG REQ ASN1 \n",
          frameP,
          (eNB_flagP) ? "eNB" : "UE",
          enb_mod_idP,
          ue_mod_idP);

#ifdef OAI_EMU
  AssertFatal (enb_mod_idP >= oai_emulation.info.first_enb_local,
      "eNB module id is too low (%u/%d)!\n",
      enb_mod_idP,
      oai_emulation.info.first_enb_local);
  AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
      "eNB module id is too high (%u/%d)!\n",
      enb_mod_idP,
      oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
  AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
      "UE module id is too low (%u/%d)!\n",
      ue_mod_idP,
      oai_emulation.info.first_ue_local);
  AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
      "UE module id is too high (%u/%d)!\n",
      ue_mod_idP,
      oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
#endif
  if (srb2add_listP != NULL) {
      for (cnt=0;cnt<srb2add_listP->list.count;cnt++) {
         rb_id = srb2add_listP->list.array[cnt]->srb_Identity;

         if (eNB_flagP) {
             rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_id].mode;
         } else {
             rlc_mode = rlc_array_ue[ue_mod_idP][rb_id].mode;
         }
         LOG_D(RLC, "Adding SRB %d, rb_id %d\n",srb2add_listP->list.array[cnt]->srb_Identity,rb_id);
          srb_toaddmod = srb2add_listP->list.array[cnt];

          if (srb_toaddmod->rlc_Config) {
              switch (srb_toaddmod->rlc_Config->present) {
                  case SRB_ToAddMod__rlc_Config_PR_NOTHING:
                      break;
                  case SRB_ToAddMod__rlc_Config_PR_explicitValue:
                      switch (srb_toaddmod->rlc_Config->choice.explicitValue.present) {
                          case RLC_Config_PR_NOTHING:
                              break;
                          case RLC_Config_PR_am:
                              if (rlc_mode == RLC_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, rb_id, rb_id, RLC_AM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_am_asn1 (
                                                 frameP, 
                                                 eNB_flagP,
                                                 enb_mod_idP,
                                                 ue_mod_idP,
                                                 &srb_toaddmod->rlc_Config->choice.explicitValue.choice.am, 
                                                 rb_id, 
                                                 SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (eNB_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d AM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (eNB_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Bi_Directional:
                              if (rlc_mode == RLC_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, rb_id, rb_id, RLC_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          eNB_flagP,
                                          RLC_MBMS_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          &srb_toaddmod->rlc_Config->choice.explicitValue.choice.um_Bi_Directional.ul_UM_RLC,
                                          &srb_toaddmod->rlc_Config->choice.explicitValue.choice.um_Bi_Directional.dl_UM_RLC,
                                          rb_id,
                                          SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (eNB_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (eNB_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Uni_Directional_UL:
                              if (rlc_mode == RLC_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, rb_id, rb_id, RLC_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          eNB_flagP,
                                          RLC_MBMS_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          &srb_toaddmod->rlc_Config->choice.explicitValue.choice.um_Uni_Directional_UL.ul_UM_RLC,
                                          NULL,
                                          rb_id, SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (eNB_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (eNB_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Uni_Directional_DL:
                              if (rlc_mode == RLC_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, rb_id, rb_id, RLC_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          eNB_flagP,
                                          RLC_MBMS_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          NULL,
                                          &srb_toaddmod->rlc_Config->choice.explicitValue.choice.um_Uni_Directional_DL.dl_UM_RLC,
                                          rb_id, SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (eNB_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (eNB_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          default:
                              LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] UNKNOWN RLC CONFIG %d \n",
                                      frameP,
                                      (eNB_flagP) ? "eNB" : "UE",
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      srb_toaddmod->rlc_Config->choice.explicitValue.present);
                              break;
                      }
                      break;
                  case SRB_ToAddMod__rlc_Config_PR_defaultValue:
#warning TO DO SRB_ToAddMod__rlc_Config_PR_defaultValue
                      if (rlc_mode == RLC_NONE) {
                          rrc_rlc_add_rlc   (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, rb_id, rb_id, RLC_UM);
                          config_req_rlc_um_asn1(
                                      frameP,
                                      eNB_flagP,
                                      RLC_MBMS_NO,
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      NULL, // TO DO DEFAULT CONFIG
                                      NULL, // TO DO DEFAULT CONFIG
                                      rb_id, SIGNALLING_RADIO_BEARER);
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d DEFAULT UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (eNB_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  rb_id);
                      }
                      break;
                  default:;
              }
          }
      }
  }
  if (drb2add_listP != NULL) {
      for (cnt=0;cnt<drb2add_listP->list.count;cnt++) {
          drb_toaddmod = drb2add_listP->list.array[cnt];

          drb_id = *drb_toaddmod->logicalChannelIdentity;//drb_toaddmod->drb_Identity;
          lc_id  = drb_id;
          if (eNB_flagP) {
              rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][drb_id].mode;
          } else {
              rlc_mode = rlc_array_ue[ue_mod_idP][drb_id].mode;
          }
          LOG_D(RLC, "Adding DRB %d, rb_id %d\n",*drb_toaddmod->logicalChannelIdentity,drb_id);
          

          if (drb_toaddmod->rlc_Config) {

              switch (drb_toaddmod->rlc_Config->present) {
                  case RLC_Config_PR_NOTHING:
                      break;
                  case RLC_Config_PR_am:
                      if (rlc_mode == RLC_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, drb_id, lc_id, RLC_AM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_am_asn1 (
                                            frameP,
                                            eNB_flagP,
                                            enb_mod_idP,
                                            ue_mod_idP,
                                            &drb_toaddmod->rlc_Config->choice.am,
                                            drb_id,
                                            RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d AM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (eNB_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Bi_Directional:
                      if (rlc_mode == RLC_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, drb_id, lc_id, RLC_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  eNB_flagP,
                                  RLC_MBMS_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  &drb_toaddmod->rlc_Config->choice.um_Bi_Directional.ul_UM_RLC,
                                  &drb_toaddmod->rlc_Config->choice.um_Bi_Directional.dl_UM_RLC,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (eNB_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Uni_Directional_UL:
                      if (rlc_mode == RLC_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, drb_id, lc_id, RLC_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  eNB_flagP,
                                  RLC_MBMS_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  &drb_toaddmod->rlc_Config->choice.um_Uni_Directional_UL.ul_UM_RLC,
                                  NULL,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (eNB_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Uni_Directional_DL:
                      if (rlc_mode == RLC_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, drb_id, lc_id, RLC_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  eNB_flagP,
                                  RLC_MBMS_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  NULL,
                                  &drb_toaddmod->rlc_Config->choice.um_Uni_Directional_DL.dl_UM_RLC,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (eNB_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  default:
                      LOG_W(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u][RB %u] unknown drb_toaddmod->rlc_Config->present \n",
                              frameP,
                              (eNB_flagP) ? "eNB" : "UE",
                              enb_mod_idP,
                              ue_mod_idP,
                              drb_id);
              }
          }
      }
  }
  if (drb2release_listP != NULL) {
    for (cnt=0;cnt<drb2release_listP->list.count;cnt++) {
      pdrb_id = drb2release_listP->list.array[cnt];
      rrc_rlc_remove_rlc(enb_mod_idP, ue_mod_idP, frameP, eNB_flagP, *pdrb_id);
    }
  }

#ifdef Rel10
  if (pmch_info_listP != NULL) {

      LOG_I(RLC,"[%s %d] Config RLC instance for MBMS\n", (eNB_flagP) ? "eNB" : "UE", (eNB_flagP) ? module_idP : module_idP - NB_eNB_INST);

      for (cnt=0;cnt<pmch_info_listP->list.count;cnt++) {
          pmch_info_r9 = pmch_info_listP->list.array[cnt];
      
          for (cnt2=0;cnt2<pmch_info_r9->mbms_SessionInfoList_r9.list.count;cnt2++) {
              mbms_session = pmch_info_r9->mbms_SessionInfoList_r9.list.array[cnt2];

              if (mbms_session->logicalChannelIdentity_r9 > 0) {

                  //  lc_id = (NUMBER_OF_UE_MAX*NB_RB_MAX) + mbms_session->logicalChannelIdentity_r9;
                  //   test this one and tell Lionel
                  if (eNB_flagP) {
                      /* SR: (maxDRB = 11 + 3) * 16 = 224... */
                      lc_id = mbms_session->logicalChannelIdentity_r9 + (maxDRB + 3) * MAX_MOBILES_PER_RG;
                  } else {
                      lc_id = mbms_session->logicalChannelIdentity_r9 + (maxDRB + 3);
                  }

                  /*
                  if (mbms_session->sessionId_r9 != NULL) {
                      mbms_session_id = mbms_session->sessionId_r9->buf[0];
                  } else {
                     mbms_session_id = mbms_session->logicalChannelIdentity_r9;
                  }
                  */
                  mbms_service_id = mbms_session->tmgi_r9.serviceId_r9.buf[2];// can use the pmch_index, here is the value 'cnt'
                  rb_id = (mbms_service_id * maxSessionPerPMCH) + lc_id;

                  if (rlc[module_idP].m_rlc_pointer[rb_id].rlc_type == RLC_NONE) {
                      rlc_status = rrc_rlc_add_rlc (module_idP, frameP, rb_id, lc_id, RLC_UM);
                      if (rlc_status != RLC_OP_STATUS_OK ) {
                        LOG_D(RLC, "[RLC_RRC] COULD NOT ALLOCATE RLC UM INSTANCE\n");
                        continue;//? return rlc_status;
                      }
                  } else if (rlc[module_idP].m_rlc_pointer[rb_id].rlc_type != RLC_UM) {
                      LOG_E(RLC, "[RLC_RRC] MBMS ERROR IN CONFIG, RLC FOUND ALREADY CONFIGURED FOR MBMS BEARER IS NOT UM\n");
                      continue;
                  }
                  dl_um_rlc.sn_FieldLength = SN_FieldLength_size5;
                  dl_um_rlc.t_Reordering = T_Reordering_ms0;

                  config_req_rlc_um_asn1 (
                        frameP,
                        eNB_flagP,
                        RLC_MBMS_YES,
                        enb_mod_idP,
                        ue_mod_idP,
                        NULL,
                        &dl_um_rlc,
                        rb_id,
                        RADIO_ACCESS_BEARER);
              } else {
                  LOG_D(RLC, "[RLC_RRC] Invalid LogicalChannelIdentity for MTCH --- Value 0 is reserved for MCCH\n");
                  lc_id = -1;
              }
          }
      }
  }
#endif
  
  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] CONFIG REQ ASN1 END \n",
         frameP,
         (eNB_flagP) ? "eNB" : "UE",
         eNB_flagP,
         enb_mod_idP,
         ue_mod_idP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_tm (struct rlc_tm_entity *rlcP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_tm_cleanup(rlcP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_um (struct rlc_um_entity *rlcP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_um_cleanup(rlcP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_am (struct rlc_am_entity *rlcP, frame_t frameP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_am_cleanup(rlcP,frame);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_remove_rlc   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t rb_idP) {
//-----------------------------------------------------------------------------
    logical_chan_id_t        lcid     = 0;
    rlc_mode_t       rlc_mode = RLC_NONE;
    rlc_op_status_t  status;

#ifdef OAI_EMU
    AssertFatal (enb_mod_idP >= oai_emulation.info.first_enb_local,
        "eNB module id is too low (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local);
    AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
        "eNB module id is too high (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
    AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
        "UE module id is too low (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local);
    AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
        "UE module id is too high (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
#endif

    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);

    if (eNB_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
        switch (rlc_mode) {
        case RLC_AM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB AM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC AM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_am(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am, frame, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        case RLC_TM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB TM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC TM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_tm(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        case RLC_UM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB UM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC UM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
             status = rb_release_rlc_um(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        default:
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB mode %d\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    rlc_mode);
            return RLC_OP_STATUS_BAD_PARAMETER;
            break;
        }
    } else {
        rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
        switch (rlc_mode) {
        case RLC_AM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB AM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC AM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_am(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.am, frame, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        case RLC_TM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB TM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC TM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_tm(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        case RLC_UM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB UM\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC UM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_um(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.um, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_NONE;
            break;
        default:
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB mode %d\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    rlc_mode);
            return RLC_OP_STATUS_BAD_PARAMETER;
            break;
        }
    }
    return status;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_add_rlc   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t rb_idP, logical_chan_id_t chan_idP, rlc_mode_t rlc_modeP) {
//-----------------------------------------------------------------------------
    rlc_mode_t       rlc_mode = RLC_NONE;
    unsigned int     allocation;
    
#ifdef OAI_EMU
    AssertFatal (enb_mod_idP >= oai_emulation.info.first_enb_local,
        "eNB module id is too low (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local);
    AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
        "eNB module id is too high (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
    AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
        "UE module id is too low (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local);
    AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
        "UE module id is too high (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
#endif
    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
    AssertFatal (chan_idP < RLC_MAX_LC, "LC id is too high (%u/%d)!\n", chan_idP, RLC_MAX_LC);

    if (eNB_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
        if (rlc_mode != RLC_NONE) {
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB RB IS ALREADY ALLOCATED\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    chan_idP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
        AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][chan_idP] == RLC_RB_UNALLOCATED, "Bad LC RB %u mapping in RLC layer, channel id %u already configured!\n", rb_idP, chan_idP);
        lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][chan_idP] = rb_idP;
        switch (rlc_modeP) {
        case RLC_AM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_AM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM IS ALREADY ALLOCATED\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_TM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_TM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM IS ALREADY ALLOCATED\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_UM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_UM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM IS ALREADY ALLOCATED\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        default:
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] %s BAD PARAMETER RLC MODE %d\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    __FUNCTION__,
                    rlc_modeP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
    } else {
        rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
        if (rlc_mode != RLC_NONE) {
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB RB IS ALREADY ALLOCATED\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    chan_idP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
        AssertFatal (lcid2rbid_ue[ue_mod_idP][chan_idP] == RLC_RB_UNALLOCATED, "Bad LC RB %u mapping in RLC layer, channel id %u already configured!\n", rb_idP, chan_idP);
        lcid2rbid_ue[ue_mod_idP][chan_idP] = rb_idP;
        switch (rlc_modeP) {
        case RLC_AM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_AM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM IS ALREADY ALLOCATED\n",
                    frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_TM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_TM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM IS ALREADY ALLOCATED\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_UM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_UM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM IS ALREADY ALLOCATED\n",
                        frame,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        default:
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] %s BAD PARAMETER RLC MODE %d\n",
                    frame,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    __FUNCTION__,
                    rlc_modeP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
    }
    return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_config_req   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, config_action_t actionP, rb_id_t rb_idP, rb_type_t rb_typeP, rlc_info_t rlc_infoP) {
//-----------------------------------------------------------------------------
    rlc_op_status_t status;

    LOG_D(RLC, "[FRAME %05u][%s][RLC][MOD %u/%u] CONFIG_REQ for Rab %u\n",
            frame,
            (eNB_flagP) ? "eNB" : "UE",
            enb_mod_idP,
            ue_mod_idP,
            rb_idP);

#ifdef OAI_EMU
    AssertFatal (enb_mod_idP >= oai_emulation.info.first_enb_local,
        "eNB module id is too low (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local);
    AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
        "eNB module id is too high (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
    AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
        "UE module id is too low (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local);
    AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
        "UE module id is too high (%u/%d)!\n",
        ue_mod_idP,
        oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
#endif
    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
#warning TO DO rrc_rlc_config_req

    switch (actionP) {

        case ACTION_ADD:
            if ((status = rrc_rlc_add_rlc(enb_mod_idP, ue_mod_idP, frame, eNB_flagP, rb_idP, rb_idP, rlc_infoP.rlc_mode)) != RLC_OP_STATUS_OK) {
              return status;
            }
        case ACTION_MODIFY:
            switch (rlc_infoP.rlc_mode) {
                case RLC_AM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB AM\n",
                            frame,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);

                    config_req_rlc_am(
                      frame,
                      eNB_flagP,
                      enb_mod_idP,
                      ue_mod_idP,
                      &rlc_infoP.rlc.rlc_am_info,
                      rb_idP,
                      rb_typeP);
                    break;
                case RLC_UM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB UM\n",
                            frame,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);
                    config_req_rlc_um(
                      frame,
                      eNB_flagP,
                      enb_mod_idP,
                      ue_mod_idP,
                      &rlc_infoP.rlc.rlc_um_info,
                      rb_idP,
                      rb_typeP);
                    break;
                case RLC_TM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB TM\n",
                            frame,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);
                    config_req_rlc_tm(
                      frame,
                      eNB_flagP,
                      enb_mod_idP,
                      ue_mod_idP,
                      &rlc_infoP.rlc.rlc_tm_info,
                      rb_idP,
                      rb_typeP);
                    break;
                default:
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;

        case ACTION_REMOVE:
            return rrc_rlc_remove_rlc(enb_mod_idP, ue_mod_idP, rb_idP, frame, eNB_flagP);
            break;
        default:
            return RLC_OP_STATUS_BAD_PARAMETER;
    }

    return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_data_req     (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, MBMS_flag_t MBMS_flagP, rb_id_t rb_idP, mui_t muiP, confirm_t confirmP, sdu_size_t sdu_sizeP, char* sduP) {
//-----------------------------------------------------------------------------
  mem_block_t*   sdu;

  sdu = get_free_mem_block(sdu_sizeP);
  if (sdu != NULL) {
    memcpy (sdu->data, sduP, sdu_sizeP);
    return rlc_data_req(enb_mod_idP, ue_mod_idP, frame, eNB_flagP, MBMS_flagP, rb_idP, muiP, confirmP, sdu_sizeP, sdu);
  } else {
    return RLC_OP_STATUS_INTERNAL_ERROR;
  }
}

//-----------------------------------------------------------------------------
void rrc_rlc_register_rrc (rrc_data_ind_cb_t rrc_data_indP, rrc_data_conf_cb_t rrc_data_confP) {
//-----------------------------------------------------------------------------
   rlc_rrc_data_ind  = rrc_data_indP;
   rlc_rrc_data_conf = rrc_data_confP;
}

