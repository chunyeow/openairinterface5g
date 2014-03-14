/*******************************************************************************
Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2014 Eurecom

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
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
/*
                                rlc_rrc.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier at eurecom dot fr
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
rlc_op_status_t rrc_rlc_config_asn1_req (module_id_t           enb_mod_idP,
                                         module_id_t           ue_mod_idP,
                                         frame_t               frameP,
                                         eNB_flag_t            enb_flagP,
                                         SRB_ToAddModList_t   *srb2add_listP,
                                         DRB_ToAddModList_t   *drb2add_listP,
                                         DRB_ToReleaseList_t  *drb2release_listP
#if defined(Rel10)
                                         ,PMCH_InfoList_r9_t*  pmch_InfoList_r9_pP
#endif
  ) {
  //-----------------------------------------------------------------------------
  rb_id_t                rb_id           = 0;
  logical_chan_id_t      lc_id           = 0;
  DRB_Identity_t         drb_id          = 0;
  DRB_Identity_t*        pdrb_id         = NULL;
  long int               cnt             = 0;
  SRB_ToAddMod_t        *srb_toaddmod_p  = NULL;
  DRB_ToAddMod_t        *drb_toaddmod_p  = NULL;
  rlc_mode_t             rlc_mode        = RLC_MODE_NONE;
#if defined(Rel10)
  int                        i, j;
  MBMS_SessionInfoList_r9_t *mbms_SessionInfoList_r9_p = NULL;
  MBMS_SessionInfo_r9_t     *MBMS_SessionInfo_p        = NULL;
  mbms_session_id_t          mbms_session_id;
  mbms_service_id_t          mch_id, mbms_service_id;
  DL_UM_RLC_t                dl_um_rlc;
#endif

  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] CONFIG REQ ASN1 \n",
          frameP,
          (enb_flagP) ? "eNB" : "UE",
          enb_mod_idP,
          ue_mod_idP);

#ifdef OAI_EMU
    if (enb_flagP) {
        AssertFatal ((enb_mod_idP >= oai_emulation.info.first_enb_local) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too low (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local);
        AssertFatal ((enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local)) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too high (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
        AssertFatal (ue_mod_idP  < NB_UE_INST,
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
    } else {
        AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
        AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
            "UE module id is too low (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local);
    }
#endif
  if (srb2add_listP != NULL) {
      for (cnt=0;cnt<srb2add_listP->list.count;cnt++) {
         rb_id = srb2add_listP->list.array[cnt]->srb_Identity;

         if (enb_flagP) {
             rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_id].mode;
         } else {
             rlc_mode = rlc_array_ue[ue_mod_idP][rb_id].mode;
         }
         LOG_D(RLC, "Adding SRB %d, rb_id %d\n",srb2add_listP->list.array[cnt]->srb_Identity,rb_id);
          srb_toaddmod_p = srb2add_listP->list.array[cnt];

          if (srb_toaddmod_p->rlc_Config) {
              switch (srb_toaddmod_p->rlc_Config->present) {
                  case SRB_ToAddMod__rlc_Config_PR_NOTHING:
                      break;
                  case SRB_ToAddMod__rlc_Config_PR_explicitValue:
                      switch (srb_toaddmod_p->rlc_Config->choice.explicitValue.present) {
                          case RLC_Config_PR_NOTHING:
                              break;
                          case RLC_Config_PR_am:
                              if (rlc_mode == RLC_MODE_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_id, rb_id, RLC_MODE_AM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_am_asn1 (
                                                 frameP,
                                                 enb_flagP,
                                                 enb_mod_idP,
                                                 ue_mod_idP,
                                                 &srb_toaddmod_p->rlc_Config->choice.explicitValue.choice.am,
                                                 rb_id,
                                                 SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (enb_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d AM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (enb_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Bi_Directional:
                              if (rlc_mode == RLC_MODE_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_id, rb_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          enb_flagP,
                                          MBMS_FLAG_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          UNUSED_PARAM_MBMS_SESSION_ID,
                                          UNUSED_PARAM_MBMS_SERVICE_ID,
                                          &srb_toaddmod_p->rlc_Config->choice.explicitValue.choice.um_Bi_Directional.ul_UM_RLC,
                                          &srb_toaddmod_p->rlc_Config->choice.explicitValue.choice.um_Bi_Directional.dl_UM_RLC,
                                          rb_id,
                                          SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (enb_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (enb_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Uni_Directional_UL:
                              if (rlc_mode == RLC_MODE_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_id, rb_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          enb_flagP,
                                          MBMS_FLAG_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          UNUSED_PARAM_MBMS_SESSION_ID,
                                          UNUSED_PARAM_MBMS_SERVICE_ID,
                                          &srb_toaddmod_p->rlc_Config->choice.explicitValue.choice.um_Uni_Directional_UL.ul_UM_RLC,
                                          NULL,
                                          rb_id, SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (enb_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (enb_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          case RLC_Config_PR_um_Uni_Directional_DL:
                              if (rlc_mode == RLC_MODE_NONE) {
                                  if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_id, rb_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                                      config_req_rlc_um_asn1(
                                          frameP,
                                          enb_flagP,
                                          MBMS_FLAG_NO,
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          UNUSED_PARAM_MBMS_SESSION_ID,
                                          UNUSED_PARAM_MBMS_SERVICE_ID,
                                          NULL,
                                          &srb_toaddmod_p->rlc_Config->choice.explicitValue.choice.um_Uni_Directional_DL.dl_UM_RLC,
                                          rb_id, SIGNALLING_RADIO_BEARER);
                                  } else {
                                      LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] ERROR IN ALLOCATING SRB %d \n",
                                              frameP,
                                              (enb_flagP) ? "eNB" : "UE",
                                              enb_mod_idP,
                                              ue_mod_idP,
                                              rb_id);
                                  }
                              } else {
                                  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                          frameP,
                                          (enb_flagP) ? "eNB" : "UE",
                                          enb_mod_idP,
                                          ue_mod_idP,
                                          rb_id);
                              }
                              break;
                          default:
                              LOG_E(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] UNKNOWN RLC CONFIG %d \n",
                                      frameP,
                                      (enb_flagP) ? "eNB" : "UE",
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      srb_toaddmod_p->rlc_Config->choice.explicitValue.present);
                              break;
                      }
                      break;
                  case SRB_ToAddMod__rlc_Config_PR_defaultValue:
#warning TO DO SRB_ToAddMod__rlc_Config_PR_defaultValue
                      if (rlc_mode == RLC_MODE_NONE) {
                          rrc_rlc_add_rlc   (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_id, rb_id, RLC_MODE_UM);
                          config_req_rlc_um_asn1(
                                      frameP,
                                      enb_flagP,
                                      MBMS_FLAG_NO,
                                      enb_mod_idP,
                                      ue_mod_idP,
                                      UNUSED_PARAM_MBMS_SESSION_ID,
                                      UNUSED_PARAM_MBMS_SERVICE_ID,
                                      NULL, // TO DO DEFAULT CONFIG
                                      NULL, // TO DO DEFAULT CONFIG
                                      rb_id, SIGNALLING_RADIO_BEARER);
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] SRB %d DEFAULT UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (enb_flagP) ? "eNB" : "UE",
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
          drb_toaddmod_p = drb2add_listP->list.array[cnt];

          drb_id = *drb_toaddmod_p->logicalChannelIdentity;//drb_toaddmod_p->drb_Identity;
          lc_id  = drb_id;
          if (enb_flagP) {
              rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][drb_id].mode;
          } else {
              rlc_mode = rlc_array_ue[ue_mod_idP][drb_id].mode;
          }
          LOG_D(RLC, "Adding DRB %d, rb_id %d\n",*drb_toaddmod_p->logicalChannelIdentity,drb_id);


          if (drb_toaddmod_p->rlc_Config) {

              switch (drb_toaddmod_p->rlc_Config->present) {
                  case RLC_Config_PR_NOTHING:
                      break;
                  case RLC_Config_PR_am:
                      if (rlc_mode == RLC_MODE_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, drb_id, lc_id, RLC_MODE_AM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_am_asn1 (
                                            frameP,
                                            enb_flagP,
                                            enb_mod_idP,
                                            ue_mod_idP,
                                            &drb_toaddmod_p->rlc_Config->choice.am,
                                            drb_id,
                                            RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d AM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (enb_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Bi_Directional:
                      if (rlc_mode == RLC_MODE_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, drb_id, lc_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  enb_flagP,
                                  MBMS_FLAG_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  UNUSED_PARAM_MBMS_SESSION_ID,
                                  UNUSED_PARAM_MBMS_SERVICE_ID,
                                  &drb_toaddmod_p->rlc_Config->choice.um_Bi_Directional.ul_UM_RLC,
                                  &drb_toaddmod_p->rlc_Config->choice.um_Bi_Directional.dl_UM_RLC,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (enb_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Uni_Directional_UL:
                      if (rlc_mode == RLC_MODE_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, drb_id, lc_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  enb_flagP,
                                  MBMS_FLAG_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  UNUSED_PARAM_MBMS_SESSION_ID,
                                  UNUSED_PARAM_MBMS_SERVICE_ID,
                                  &drb_toaddmod_p->rlc_Config->choice.um_Uni_Directional_UL.ul_UM_RLC,
                                  NULL,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (enb_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  case RLC_Config_PR_um_Uni_Directional_DL:
                      if (rlc_mode == RLC_MODE_NONE) {
                          if (rrc_rlc_add_rlc (enb_mod_idP, ue_mod_idP, frameP, enb_flagP, drb_id, lc_id, RLC_MODE_UM) == RLC_OP_STATUS_OK) {
                              config_req_rlc_um_asn1(
                                  frameP,
                                  enb_flagP,
                                  MBMS_FLAG_NO,
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  UNUSED_PARAM_MBMS_SESSION_ID,
                                  UNUSED_PARAM_MBMS_SERVICE_ID,
                                  NULL,
                                  &drb_toaddmod_p->rlc_Config->choice.um_Uni_Directional_DL.dl_UM_RLC,
                                  drb_id, RADIO_ACCESS_BEARER);
                          }
                      } else {
                          LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] DRB %d UM ALREADY CONFIGURED, TO DO MODIFY \n",
                                  frameP,
                                  (enb_flagP) ? "eNB" : "UE",
                                  enb_mod_idP,
                                  ue_mod_idP,
                                  drb_id);
                      }
                      break;
                  default:
                      LOG_W(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u][RB %u] unknown drb_toaddmod_p->rlc_Config->present \n",
                              frameP,
                              (enb_flagP) ? "eNB" : "UE",
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
          rrc_rlc_remove_rlc(enb_mod_idP, ue_mod_idP, frameP, enb_flagP, *pdrb_id);
      }
  }

#if defined(Rel10)

  if (pmch_InfoList_r9_pP != NULL) {
      for (i=0;i<pmch_InfoList_r9_pP->list.count;i++) {
#warning TO DO REMOVE MBMS RLC
          mbms_SessionInfoList_r9_p = &(pmch_InfoList_r9_pP->list.array[i]->mbms_SessionInfoList_r9);
          for (j=0;j<mbms_SessionInfoList_r9_p->list.count;j++) {
              MBMS_SessionInfo_p = mbms_SessionInfoList_r9_p->list.array[j];
              mbms_session_id    = MBMS_SessionInfo_p->sessionId_r9->buf[0];
              lc_id              = mbms_session_id;
              mbms_service_id    = MBMS_SessionInfo_p->tmgi_r9.serviceId_r9.buf[2]; //serviceId is 3-octet string
              mch_id             = mbms_service_id;

              // can set the mch_id = i
              if (enb_flagP) {
                rb_id =  (mbms_service_id * maxSessionPerPMCH ) + mbms_session_id + (maxDRB + 3) * MAX_MOBILES_PER_ENB; // 1
                rlc_mbms_lcid2service_session_id_eNB[enb_mod_idP][lc_id].service_id                     = mbms_service_id;
                rlc_mbms_lcid2service_session_id_eNB[enb_mod_idP][lc_id].session_id                     = mbms_session_id;
                rlc_mbms_array_eNB[enb_mod_idP][mbms_service_id][mbms_session_id].rb_id                 = rb_id;
                rlc_mbms_array_eNB[enb_mod_idP][mbms_service_id][mbms_session_id].instanciated_instance = TRUE;
                rlc_mbms_enb_set_lcid_by_rb_id(enb_mod_idP,rb_id,lc_id);
              } else {
                rb_id =  (mbms_service_id * maxSessionPerPMCH ) + mbms_session_id + (maxDRB + 3); // 15
                rlc_mbms_lcid2service_session_id_ue[ue_mod_idP][lc_id].service_id                    = mbms_service_id;
                rlc_mbms_lcid2service_session_id_ue[ue_mod_idP][lc_id].session_id                    = mbms_session_id;
                rlc_mbms_array_ue[ue_mod_idP][mbms_service_id][mbms_session_id].rb_id                 = rb_id;
                rlc_mbms_array_ue[ue_mod_idP][mbms_service_id][mbms_session_id].instanciated_instance = TRUE;
                rlc_mbms_ue_set_lcid_by_rb_id(ue_mod_idP,rb_id,lc_id);
              }
              LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] CONFIG REQ MBMS ASN1 LC ID %u RB ID %u SESSION ID %u SERVICE ID %u\n",
                    frameP,
                    (enb_flagP) ? "eNB" : "UE",
                    enb_mod_idP,
                    ue_mod_idP,
                    lc_id,
                    rb_id,
                    mbms_session_id,
                    mbms_service_id
                   );
              dl_um_rlc.sn_FieldLength = SN_FieldLength_size5;
              dl_um_rlc.t_Reordering   = T_Reordering_ms0;

              config_req_rlc_um_asn1 (
                    frameP,
                    enb_flagP,
                    MBMS_FLAG_YES,
                    enb_mod_idP,
                    ue_mod_idP,
                    mbms_session_id,
                    mbms_service_id,
                    NULL,
                    &dl_um_rlc,
                    rb_id,
                    RADIO_ACCESS_BEARER);
          }
      }
  }
#endif

  LOG_D(RLC, "[FRAME %5u][%s][RLC_RRC][MOD %u/%u] CONFIG REQ ASN1 END \n",
         frameP,
         (enb_flagP) ? "eNB" : "UE",
         enb_flagP,
         enb_mod_idP,
         ue_mod_idP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_tm (rlc_tm_entity_t *rlcP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_tm_cleanup(rlcP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_um (rlc_um_entity_t *rlcP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_um_cleanup(rlcP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t
rb_release_rlc_am (rlc_am_entity_t *rlcP, frame_t frameP, module_id_t enb_mod_idP, module_id_t ue_mod_idP)
{
//-----------------------------------------------------------------------------
  rlc_am_cleanup(rlcP,frameP);
  return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_remove_rlc   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, rb_id_t rb_idP) {
//-----------------------------------------------------------------------------
    logical_chan_id_t        lcid     = 0;
    rlc_mode_t       rlc_mode = RLC_MODE_NONE;
    rlc_op_status_t  status;

#ifdef OAI_EMU
    AssertFatal ((enb_mod_idP >= oai_emulation.info.first_enb_local) && (oai_emulation.info.nb_enb_local > 0),
        "eNB module id is too low (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local);
    AssertFatal (enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local),
        "eNB module id is too high (%u/%d)!\n",
        enb_mod_idP,
        oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
    if (enb_flagP) {
        AssertFatal (ue_mod_idP  < NB_UE_INST,
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
    } else {
        AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
        AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
            "UE module id is too low (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local);
    }
#endif

    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);

    if (enb_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
        switch (rlc_mode) {
        case RLC_MODE_AM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB AM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC AM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_am(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am, frameP, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        case RLC_MODE_TM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB TM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC TM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_tm(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        case RLC_MODE_UM:
            LOG_D(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB UM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.channel_id;
            AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC UM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid], rb_idP);
            lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
             status = rb_release_rlc_um(&rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um, enb_mod_idP, ue_mod_idP);
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation = 0;
            rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        default:
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] RELEASE RB mode %d\n",
                    frameP,
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
        case RLC_MODE_AM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB AM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC AM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_am(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.am, frameP, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        case RLC_MODE_TM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB TM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC TM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_tm(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        case RLC_MODE_UM:
            LOG_D(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB UM\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP);
            lcid = rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.channel_id;
            AssertFatal (lcid2rbid_ue[ue_mod_idP][lcid] == rb_idP, "Mismatch in RLC UM LC %u/RB %u mapping for RB %u\n", lcid, lcid2rbid_ue[ue_mod_idP][lcid], rb_idP);
            lcid2rbid_ue[ue_mod_idP][lcid] = RLC_RB_UNALLOCATED;
            status = rb_release_rlc_um(&rlc_array_ue[ue_mod_idP][rb_idP].rlc.um, enb_mod_idP, ue_mod_idP);
            rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation = 0;
            rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_NONE;
            break;
        default:
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] RELEASE RB mode %d\n",
                    frameP,
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
rlc_op_status_t rrc_rlc_add_rlc   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, rb_id_t rb_idP, logical_chan_id_t chan_idP, rlc_mode_t rlc_modeP) {
//-----------------------------------------------------------------------------
    rlc_mode_t       rlc_mode = RLC_MODE_NONE;
    unsigned int     allocation;

#ifdef OAI_EMU
    if (enb_flagP) {
        AssertFatal ((enb_mod_idP >= oai_emulation.info.first_enb_local) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too low (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local);
        AssertFatal ((enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local)) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too high (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
        AssertFatal (ue_mod_idP  < NB_UE_INST,
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
    } else {
        AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
        AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
            "UE module id is too low (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local);
    }
#endif
    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
    AssertFatal (chan_idP < RLC_MAX_LC, "LC id is too high (%u/%d)!\n", chan_idP, RLC_MAX_LC);

    if (enb_flagP) {
        rlc_mode = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode;
        if (rlc_mode != RLC_MODE_NONE) {
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB RB IS ALREADY ALLOCATED\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    chan_idP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
        AssertFatal (lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][chan_idP] == RLC_RB_UNALLOCATED, "Bad LC RB %u mapping in RLC layer, channel id %u already configured!\n", rb_idP, chan_idP);
        lcid2rbid_eNB[enb_mod_idP][ue_mod_idP][chan_idP] = rb_idP;
        switch (rlc_modeP) {
        case RLC_MODE_AM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.am.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_AM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM IS ALREADY ALLOCATED\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_MODE_TM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.tm.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_TM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM IS ALREADY ALLOCATED\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_MODE_UM:
            allocation = rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation;
            if (!(allocation)) {
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].rlc.um.allocation = 1;
                rlc_array_eNB[enb_mod_idP][ue_mod_idP][rb_idP].mode = RLC_MODE_UM;
                LOG_I(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM IS ALREADY ALLOCATED\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        default:
            LOG_E(RLC, "[Frame %05u][eNB][RLC_RRC][INST %u/%u][RB %u] %s BAD PARAMETER RLC MODE %d\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    __FUNCTION__,
                    rlc_modeP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
    } else {
        rlc_mode = rlc_array_ue[ue_mod_idP][rb_idP].mode;
        if (rlc_mode != RLC_MODE_NONE) {
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB RB IS ALREADY ALLOCATED\n",
                    frameP,
                    enb_mod_idP,
                    ue_mod_idP,
                    rb_idP,
                    chan_idP);
            return RLC_OP_STATUS_BAD_PARAMETER;
        }
        AssertFatal (lcid2rbid_ue[ue_mod_idP][chan_idP] == RLC_RB_UNALLOCATED, "Bad LC RB %u mapping in RLC layer, channel id %u already configured!\n", rb_idP, chan_idP);
        lcid2rbid_ue[ue_mod_idP][chan_idP] = rb_idP;
        switch (rlc_modeP) {
        case RLC_MODE_AM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.am.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_AM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB AM IS ALREADY ALLOCATED\n",
                    frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_MODE_TM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.tm.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_TM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB TM IS ALREADY ALLOCATED\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        case RLC_MODE_UM:
            allocation = rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation;
            if (!(allocation)) {
                rlc_array_ue[ue_mod_idP][rb_idP].rlc.um.allocation = 1;
                rlc_array_ue[ue_mod_idP][rb_idP].mode = RLC_MODE_UM;
                LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
            } else {
                LOG_D(RLC,"[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u][LCH Id %d] ADD RB UM IS ALREADY ALLOCATED\n",
                        frameP,
                        enb_mod_idP,
                        ue_mod_idP,
                        rb_idP,
                        chan_idP);
                return RLC_OP_STATUS_BAD_PARAMETER;
            }
            break;
        default:
            LOG_E(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] %s BAD PARAMETER RLC MODE %d\n",
                    frameP,
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
rlc_op_status_t rrc_rlc_config_req   (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, config_action_t actionP, rb_id_t rb_idP, rb_type_t rb_typeP, rlc_info_t rlc_infoP) {
//-----------------------------------------------------------------------------
    rlc_op_status_t status;

    LOG_D(RLC, "[FRAME %05u][%s][RLC][MOD %u/%u] CONFIG_REQ for Rab %u\n",
            frameP,
            (enb_flagP) ? "eNB" : "UE",
            enb_mod_idP,
            ue_mod_idP,
            rb_idP);

#ifdef OAI_EMU
    if (enb_flagP) {
        AssertFatal ((enb_mod_idP >= oai_emulation.info.first_enb_local) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too low (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local);
        AssertFatal ((enb_mod_idP < (oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local)) && (oai_emulation.info.nb_enb_local > 0),
            "eNB module id is too high (%u/%d)!\n",
            enb_mod_idP,
            oai_emulation.info.first_enb_local + oai_emulation.info.nb_enb_local);
        AssertFatal (ue_mod_idP  < NB_UE_INST,
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
    } else {
        AssertFatal (ue_mod_idP  < (oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local),
            "UE module id is too high (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local + oai_emulation.info.nb_ue_local);
        AssertFatal (ue_mod_idP  >= oai_emulation.info.first_ue_local,
            "UE module id is too low (%u/%d)!\n",
            ue_mod_idP,
            oai_emulation.info.first_ue_local);
    }
#endif
    AssertFatal (rb_idP < NB_RB_MAX, "RB id is too high (%u/%d)!\n", rb_idP, NB_RB_MAX);
    switch (actionP) {

        case CONFIG_ACTION_ADD:
            if ((status = rrc_rlc_add_rlc(enb_mod_idP, ue_mod_idP, frameP, enb_flagP, rb_idP, rb_idP, rlc_infoP.rlc_mode)) != RLC_OP_STATUS_OK) {
              return status;
            }
            // no break, fall to next case
        case CONFIG_ACTION_MODIFY:
            switch (rlc_infoP.rlc_mode) {
                case RLC_MODE_AM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB AM\n",
                            frameP,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);

                    config_req_rlc_am(
                      frameP,
                      enb_flagP,
                      enb_mod_idP,
                      ue_mod_idP,
                      &rlc_infoP.rlc.rlc_am_info,
                      rb_idP,
                      rb_typeP);
                    break;
                case RLC_MODE_UM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB UM\n",
                            frameP,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);
                    config_req_rlc_um(
                      frameP,
                      enb_flagP,
                      enb_mod_idP,
                      ue_mod_idP,
                      &rlc_infoP.rlc.rlc_um_info,
                      rb_idP,
                      rb_typeP);
                    break;
                case RLC_MODE_TM:
                    LOG_I(RLC, "[Frame %05u][UE][RLC_RRC][INST %u/%u][RB %u] MODIFY RB TM\n",
                            frameP,
                            enb_mod_idP,
                            ue_mod_idP,
                            rb_idP);
                    config_req_rlc_tm(
                      frameP,
                      enb_flagP,
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

        case CONFIG_ACTION_REMOVE:
            return rrc_rlc_remove_rlc(enb_mod_idP, ue_mod_idP, rb_idP, frameP, enb_flagP);
            break;
        default:
            return RLC_OP_STATUS_BAD_PARAMETER;
    }

    return RLC_OP_STATUS_OK;
}
//-----------------------------------------------------------------------------
rlc_op_status_t rrc_rlc_data_req     (module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t enb_flagP, MBMS_flag_t MBMS_flagP, rb_id_t rb_idP, mui_t muiP, confirm_t confirmP, sdu_size_t sdu_sizeP, char* sduP) {
//-----------------------------------------------------------------------------
  mem_block_t*   sdu;

  sdu = get_free_mem_block(sdu_sizeP);
  if (sdu != NULL) {
    memcpy (sdu->data, sduP, sdu_sizeP);
    return rlc_data_req(enb_mod_idP, ue_mod_idP, frameP, enb_flagP, MBMS_flagP, rb_idP, muiP, confirmP, sdu_sizeP, sdu);
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

