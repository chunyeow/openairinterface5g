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
/*! \file rrc_eNB_S1AP.c
 * \brief rrc S1AP procedures for eNB
 * \author Laurent Winckel and Navid Nikaein
 * \date 2013-2014
 * \version 1.0
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr
 */

#if defined(ENABLE_USE_MME)
# include "defs.h"
# include "extern.h"
# include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
# include "RRC/LITE/MESSAGES/asn1_msg.h"
# include "rrc_eNB_S1AP.h"

# if defined(ENABLE_ITTI)
#   include "asn1_conversions.h"
#   include "intertask_interface.h"
#   include "pdcp.h"
#   include "pdcp_primitives.h"
#   include "s1ap_eNB.h"
# else
#   include "../../S1AP/s1ap_eNB.h"
# endif

#if defined(ENABLE_SECURITY)
#   include "UTIL/OSA/osa_defs.h"
#endif

/* Value to indicate an invalid UE initial id */
static const uint16_t UE_INITIAL_ID_INVALID = 0;

/* Masks for S1AP Encryption algorithms, EEA0 is always supported (not coded) */
static const uint16_t S1AP_ENCRYPTION_EEA1_MASK = 0x8000;
static const uint16_t S1AP_ENCRYPTION_EEA2_MASK = 0x4000;

/* Masks for S1AP Integrity algorithms, EIA0 is always supported (not coded) */
static const uint16_t S1AP_INTEGRITY_EIA1_MASK = 0x8000;
static const uint16_t S1AP_INTEGRITY_EIA2_MASK = 0x4000;

#ifdef Rel10
# define INTEGRITY_ALGORITHM_NONE SecurityAlgorithmConfig__integrityProtAlgorithm_eia0_v920
#else
#ifdef EXMIMO_IOT
# define INTEGRITY_ALGORITHM_NONE SecurityAlgorithmConfig__integrityProtAlgorithm_eia2
#else
# define INTEGRITY_ALGORITHM_NONE SecurityAlgorithmConfig__integrityProtAlgorithm_reserved
#endif
#endif

# if defined(ENABLE_ITTI)
/*! \fn uint16_t get_next_ue_initial_id(uint8_t mod_id)
 *\brief provide an UE initial ID for S1AP initial communication.
 *\param mod_id Instance ID of eNB.
 *\return the UE initial ID.
 */
static uint16_t get_next_ue_initial_id(uint8_t mod_id) {
  static uint16_t ue_initial_id[NUMBER_OF_eNB_MAX];

  ue_initial_id[mod_id]++;

  /* Never use UE_INITIAL_ID_INVALID this is the invalid id! */
  if (ue_initial_id[mod_id] == UE_INITIAL_ID_INVALID) {
    ue_initial_id[mod_id]++;
  }

  return ue_initial_id[mod_id];
}

/*! \fn uint8_t get_UE_index_from_initial_id (uint8_t mod_id, uint16_t ue_initial_id)
 *\brief retrieve UE index in the eNB from the UE initial ID.
 *\param mod_id Instance ID of eNB.
 *\param ue_initial_id The UE initial ID sent to S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_initial_id(uint8_t mod_id, uint16_t ue_initial_id) {
  uint8_t ue_index;

  AssertFatal(mod_id < NB_eNB_INST, "eNB index invalid (%d/%d)!", mod_id, NB_eNB_INST);
  LOG_D(RRC, "[eNB %d] get_UE_index_from_initial_id: ue_initial_id %d\n", ue_initial_id);

  for (ue_index = 0; ue_index < NUMBER_OF_UE_MAX; ue_index++) {
    /* Check if this UE is in use */
    LOG_D(RRC, "[eNB %d][UE %d] UE rv 0x%" PRIx64 " %d\n", mod_id, ue_index,
          eNB_rrc_inst[mod_id].Info.UE_list[ue_index], eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id);

    if (eNB_rrc_inst[mod_id].Info.UE_list[ue_index] != 0) {
      /* Check if the initial id match */
      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id == ue_initial_id) {
        return ue_index;
      }
    }
  }
  return UE_INDEX_INVALID;
}

/*! \fn uint8_t get_UE_index_from_eNB_ue_s1ap_id(uint8_t mod_id, uint32_t eNB_ue_s1ap_id)
 *\brief retrieve UE index in the eNB from the eNB_ue_s1ap_id previously transmitted by S1AP.
 *\param mod_id Instance ID of eNB.
 *\param eNB_ue_s1ap_id The value sent by S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_eNB_ue_s1ap_id(uint8_t mod_id, uint32_t eNB_ue_s1ap_id) {
  uint8_t ue_index;

  AssertFatal(mod_id < NB_eNB_INST, "eNB index invalid (%d/%d)!", mod_id, NB_eNB_INST);
  LOG_D(RRC, "[eNB %d] get_UE_index_from_eNB_ue_s1ap_id: eNB_ue_s1ap_id %d\n", mod_id, eNB_ue_s1ap_id);

  for (ue_index = 0; ue_index < NUMBER_OF_UE_MAX; ue_index++) {
    /* Check if this UE is in use */
    LOG_D(RRC, "[eNB %d][UE %d] UE rv 0x%" PRIx64 " %d\n", mod_id, ue_index,
          eNB_rrc_inst[mod_id].Info.UE_list[ue_index], eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id);

    if (eNB_rrc_inst[mod_id].Info.UE_list[ue_index] != 0) {
      /* Check if the initial id match */
      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id == eNB_ue_s1ap_id) {
        return ue_index;
      }
    }
  }
  LOG_D(RRC,
      "[eNB %d] return UE_INDEX_INVALID for eNB_ue_s1ap_id %u\n",
      mod_id,
      eNB_ue_s1ap_id);
  return UE_INDEX_INVALID;
}

/*! \fn uint8_t get_UE_index_from_s1ap_ids(uint8_t mod_id, uint16_t ue_initial_id, uint32_t eNB_ue_s1ap_id)
 *\brief retrieve UE index in the eNB from the UE initial ID if not equal to UE_INDEX_INVALID or
 *\brief from the eNB_ue_s1ap_id previously transmitted by S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_initial_id The UE initial ID sent to S1AP.
 *\param eNB_ue_s1ap_id The value sent by S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_s1ap_ids(uint8_t mod_id, uint16_t ue_initial_id, uint32_t eNB_ue_s1ap_id) {
  uint8_t ue_index;

  if (ue_initial_id == UE_INITIAL_ID_INVALID) {
    /* If "ue_initial_id" is not set search if "eNB_ue_s1ap_id" is know by RRC */
    ue_index = get_UE_index_from_eNB_ue_s1ap_id (mod_id, eNB_ue_s1ap_id);
  }
  else {
    /* If "ue_initial_id" is set there is probably not yet an associated "eNB_ue_s1ap_id" with S1AP */
    ue_index = get_UE_index_from_initial_id (mod_id, ue_initial_id);
  }

  return ue_index;
}

/*! \fn e_SecurityAlgorithmConfig__cipheringAlgorithm rrc_eNB_select_ciphering(uint16_t algorithms)
 *\brief analyze available encryption algorithms bit mask and return the relevant one.
 *\param algorithms The bit mask of available algorithms received from S1AP.
 *\return the selected algorithm.
 */
static e_SecurityAlgorithmConfig__cipheringAlgorithm rrc_eNB_select_ciphering(uint16_t algorithms) {
  
#warning "Forced   return SecurityAlgorithmConfig__cipheringAlgorithm_eea0, to be deleted in future"
  return SecurityAlgorithmConfig__cipheringAlgorithm_eea0;

  if (algorithms & S1AP_ENCRYPTION_EEA2_MASK) {
    return SecurityAlgorithmConfig__cipheringAlgorithm_eea2;
  }

  if (algorithms & S1AP_ENCRYPTION_EEA1_MASK) {
    return SecurityAlgorithmConfig__cipheringAlgorithm_eea1;
  }

  return SecurityAlgorithmConfig__cipheringAlgorithm_eea0;
}

/*! \fn e_SecurityAlgorithmConfig__integrityProtAlgorithm rrc_eNB_select_integrity(uint16_t algorithms)
 *\brief analyze available integrity algorithms bit mask and return the relevant one.
 *\param algorithms The bit mask of available algorithms received from S1AP.
 *\return the selected algorithm.
 */
static e_SecurityAlgorithmConfig__integrityProtAlgorithm rrc_eNB_select_integrity(uint16_t algorithms) {
  
  if (algorithms & S1AP_INTEGRITY_EIA2_MASK) {
    return SecurityAlgorithmConfig__integrityProtAlgorithm_eia2;
  }

  if (algorithms & S1AP_INTEGRITY_EIA1_MASK) {
    return SecurityAlgorithmConfig__integrityProtAlgorithm_eia1;
  }

  return INTEGRITY_ALGORITHM_NONE;
}

/*! \fn int rrc_eNB_process_security (uint8_t mod_id, uint8_t ue_index, security_capabilities_t *security_capabilities)
 *\brief save and analyze available security algorithms bit mask and select relevant ones.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 *\param security_capabilities The security capabilities received from S1AP.
 *\return TRUE if at least one algorithm has been changed else FALSE.
 */
static int rrc_eNB_process_security (uint8_t mod_id, uint8_t ue_index, security_capabilities_t *security_capabilities) {
  int changed = FALSE;
  e_SecurityAlgorithmConfig__cipheringAlgorithm cipheringAlgorithm;
  e_SecurityAlgorithmConfig__integrityProtAlgorithm integrityProtAlgorithm;

  /* Save security parameters */
  eNB_rrc_inst[mod_id].Info.UE[ue_index].security_capabilities = *security_capabilities;

  // translation
  LOG_D(RRC,
      "[eNB %d] NAS security_capabilities.encryption_algorithms %u AS ciphering_algorithm %u NAS security_capabilities.integrity_algorithms %u AS integrity_algorithm %u\n",
      mod_id,
      eNB_rrc_inst[mod_id].Info.UE[ue_index].security_capabilities.encryption_algorithms,
      eNB_rrc_inst[mod_id].ciphering_algorithm[ue_index],
      eNB_rrc_inst[mod_id].Info.UE[ue_index].security_capabilities.integrity_algorithms,
      eNB_rrc_inst[mod_id].integrity_algorithm[ue_index]);
  /* Select relevant algorithms */
  cipheringAlgorithm = rrc_eNB_select_ciphering (eNB_rrc_inst[mod_id].Info.UE[ue_index].security_capabilities.encryption_algorithms);
  if (eNB_rrc_inst[mod_id].ciphering_algorithm[ue_index] != cipheringAlgorithm) {
    eNB_rrc_inst[mod_id].ciphering_algorithm[ue_index] = cipheringAlgorithm;
    changed = TRUE;
  }

  integrityProtAlgorithm = rrc_eNB_select_integrity (eNB_rrc_inst[mod_id].Info.UE[ue_index].security_capabilities.integrity_algorithms);
  if (eNB_rrc_inst[mod_id].integrity_algorithm[ue_index] != integrityProtAlgorithm) {
    eNB_rrc_inst[mod_id].integrity_algorithm[ue_index] = integrityProtAlgorithm;
    changed = TRUE;
  }

  LOG_I (RRC, "[eNB %d][UE %d] Selected security algorithms (%x): %x, %x, %s\n",
         mod_id, ue_index, security_capabilities, cipheringAlgorithm, integrityProtAlgorithm, changed ? "changed" : "same");

  return changed;
}

/*! \fn void process_eNB_security_key (uint8_t mod_id, uint8_t ue_index, uint8_t *security_key)
 *\brief save security key.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 *\param security_key The security key received from S1AP.
 */
static void process_eNB_security_key (uint8_t mod_id, uint8_t ue_index, uint8_t *security_key) {
#if defined(ENABLE_SECURITY)
  char ascii_buffer[65];
  uint8_t i;

  /* Saves the security key */
  memcpy (eNB_rrc_inst[mod_id].kenb[ue_index], security_key, SECURITY_KEY_LENGTH);

  for (i = 0; i < 32; i++) {
      sprintf(&ascii_buffer[2 * i], "%02X", eNB_rrc_inst[mod_id].kenb[ue_index][i]);
  }
  ascii_buffer[2 * i] = '\0';

  LOG_I (RRC, "[eNB %d][UE %d] Saved security key %s\n", mod_id, ue_index, ascii_buffer);
#endif
}


static void rrc_pdcp_config_security(uint8_t enb_mod_idP, uint8_t ue_mod_idP, uint8_t send_security_mode_command ){

#if defined(ENABLE_SECURITY)

  
  SRB_ToAddModList_t                 *SRB_configList = eNB_rrc_inst[enb_mod_idP].SRB_configList[ue_mod_idP];
  uint8_t                            *kRRCenc = NULL;
  uint8_t                            *kRRCint = NULL;
  uint8_t                            *kUPenc = NULL;
  pdcp_t                             *pdcp_p   = NULL;
  static int                         print_keys= 1;
  /* Derive the keys from kenb */
  if (SRB_configList != NULL) {
    derive_key_up_enc(eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP],
		      eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kUPenc);
  }
  
  derive_key_rrc_enc(eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP],
		     eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kRRCenc);
  derive_key_rrc_int(eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP],
		     eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP], &kRRCint);
  
#define DEBUG_SECURITY 1 
 
#if defined (DEBUG_SECURITY)
#define msg printf
  if (print_keys ==1 ) {
      print_keys =0;
      int i;
      msg("\nKeNB:");
      for(i = 0; i < 32; i++)
	msg("%02x", eNB_rrc_inst[enb_mod_idP].kenb[ue_mod_idP][i]);
      msg("\n");
            
      msg("\nKRRCenc:");
      for(i = 0; i < 32; i++)
	msg("%02x", kRRCenc[i]);
      msg("\n");
      
      msg("\nKRRCint:");
      for(i = 0; i < 32; i++)
	msg("%02x", kRRCint[i]);
      msg("\n");
      
    }
#endif //DEBUG_SECURITY


    pdcp_p = &pdcp_array_srb_eNB[enb_mod_idP][ue_mod_idP][DCCH-1];
    
    pdcp_config_set_security(pdcp_p,
        enb_mod_idP,
        ue_mod_idP,
        0,
        ENB_FLAG_YES,
        DCCH,
        DCCH+2,
        (send_security_mode_command == TRUE)  ?
            0 | (eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP] << 4) :
            (eNB_rrc_inst[enb_mod_idP].ciphering_algorithm[ue_mod_idP] )         |
            (eNB_rrc_inst[enb_mod_idP].integrity_algorithm[ue_mod_idP] << 4),
        kRRCenc,
        kRRCint,
        kUPenc);
#endif
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP(uint8_t mod_id, uint8_t ue_index) {
  eNB_RRC_UE_INFO *UE_info = &eNB_rrc_inst[mod_id].Info.UE[ue_index];
  MessageDef      *msg_p         = NULL;
  int e_rab;
  int e_rabs_done = 0;
  int e_rabs_failed = 0;

  msg_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_INITIAL_CONTEXT_SETUP_RESP);
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).eNB_ue_s1ap_id = UE_info->eNB_ue_s1ap_id;
  for (e_rab = 0; e_rab < UE_info->nb_of_e_rabs; e_rab++) {
    if (UE_info->e_rab[e_rab].status == E_RAB_STATUS_DONE) {
      e_rabs_done++;
      S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs[e_rab].e_rab_id = UE_info->e_rab[e_rab].param.e_rab_id;
      // TODO add other information from S1-U when it will be integrated
          S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs[e_rab].gtp_teid = UE_info->enb_gtp_teid[e_rab];
          S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs[e_rab].eNB_addr = UE_info->enb_gtp_addrs[e_rab];
          S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs[e_rab].eNB_addr.length = 4;
    }
    else {
      e_rabs_failed++;
      S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs_failed[e_rab].e_rab_id = UE_info->e_rab[e_rab].param.e_rab_id;
      // TODO add cause when it will be integrated
    }
  }
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).nb_of_e_rabs = e_rabs_done;
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).nb_of_e_rabs_failed = e_rabs_failed;

  itti_send_msg_to_task (TASK_S1AP, mod_id, msg_p);
}
# endif

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_UPLINK_NAS(uint8_t mod_id, uint8_t ue_index, UL_DCCH_Message_t *ul_dcch_msg) {
#if defined(ENABLE_ITTI)
  {
    ULInformationTransfer_t *ulInformationTransfer = &ul_dcch_msg->message.choice.c1.choice.ulInformationTransfer;

    if ((ulInformationTransfer->criticalExtensions.present == ULInformationTransfer__criticalExtensions_PR_c1)
        && (ulInformationTransfer->criticalExtensions.choice.c1.present
            == ULInformationTransfer__criticalExtensions__c1_PR_ulInformationTransfer_r8)
        && (ulInformationTransfer->criticalExtensions.choice.c1.choice.ulInformationTransfer_r8.dedicatedInfoType.present
            == ULInformationTransfer_r8_IEs__dedicatedInfoType_PR_dedicatedInfoNAS)) {
      /* This message hold a dedicated info NAS payload, forward it to NAS */
      struct ULInformationTransfer_r8_IEs__dedicatedInfoType *dedicatedInfoType =
          &ulInformationTransfer->criticalExtensions.choice.c1.choice.ulInformationTransfer_r8.dedicatedInfoType;
      uint32_t pdu_length;
      uint8_t *pdu_buffer;
      MessageDef *msg_p;

      pdu_length = dedicatedInfoType->choice.dedicatedInfoNAS.size;
      pdu_buffer = dedicatedInfoType->choice.dedicatedInfoNAS.buf;

      msg_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_UPLINK_NAS);
      S1AP_UPLINK_NAS (msg_p).eNB_ue_s1ap_id = eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id;
      S1AP_UPLINK_NAS (msg_p).nas_pdu.length = pdu_length;
      S1AP_UPLINK_NAS (msg_p).nas_pdu.buffer = pdu_buffer;

      itti_send_msg_to_task (TASK_S1AP, mod_id, msg_p);
    }
  }
#else
  {
    ULInformationTransfer_t *ulInformationTransfer;
    ulInformationTransfer =
    &ul_dcch_msg->message.choice.c1.choice.
    ulInformationTransfer;

    if (ulInformationTransfer->criticalExtensions.present ==
        ULInformationTransfer__criticalExtensions_PR_c1)
    {
      if (ulInformationTransfer->criticalExtensions.choice.c1.present ==
          ULInformationTransfer__criticalExtensions__c1_PR_ulInformationTransfer_r8)
      {

        ULInformationTransfer_r8_IEs_t
        *ulInformationTransferR8;
        ulInformationTransferR8 =
        &ulInformationTransfer->criticalExtensions.choice.
        c1.choice.ulInformationTransfer_r8;
        if (ulInformationTransferR8->dedicatedInfoType.
            present ==
            ULInformationTransfer_r8_IEs__dedicatedInfoType_PR_dedicatedInfoNAS)
        s1ap_eNB_new_data_request (mod_id, ue_index,
            ulInformationTransferR8->
            dedicatedInfoType.choice.
            dedicatedInfoNAS.buf,
            ulInformationTransferR8->
            dedicatedInfoType.choice.
            dedicatedInfoNAS.size);
      }
    }
  }
#endif
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_UE_CAPABILITIES_IND(uint8_t mod_id, uint8_t ue_index, UL_DCCH_Message_t *ul_dcch_msg) {
  UECapabilityInformation_t *ueCapabilityInformation = &ul_dcch_msg->message.choice.c1.choice.ueCapabilityInformation;

  if ((ueCapabilityInformation->criticalExtensions.present == UECapabilityInformation__criticalExtensions_PR_c1)
      && (ueCapabilityInformation->criticalExtensions.choice.c1.present
          == UECapabilityInformation__criticalExtensions__c1_PR_ueCapabilityInformation_r8)
      && (ueCapabilityInformation->criticalExtensions.choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList.list.count > 0)) {
        UE_CapabilityRAT_ContainerList_t *ue_CapabilityRAT_ContainerList = &ueCapabilityInformation->criticalExtensions.choice.c1.choice.ueCapabilityInformation_r8.ue_CapabilityRAT_ContainerList;
        MessageDef *msg_p;

        msg_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_UE_CAPABILITIES_IND);
        S1AP_UE_CAPABILITIES_IND (msg_p).eNB_ue_s1ap_id = eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id;
        S1AP_UE_CAPABILITIES_IND (msg_p).ue_radio_cap.length = ue_CapabilityRAT_ContainerList->list.array[0]->ueCapabilityRAT_Container.size;
        S1AP_UE_CAPABILITIES_IND (msg_p).ue_radio_cap.buffer = ue_CapabilityRAT_ContainerList->list.array[0]->ueCapabilityRAT_Container.buf;

        itti_send_msg_to_task (TASK_S1AP, mod_id, msg_p);

        if (ue_CapabilityRAT_ContainerList->list.count > 1) {
          LOG_W (RRC,"[eNB %d][UE %d] can only handle 1 UE capability RAT item for now (%d)\n", mod_id, ue_index,
                 ue_CapabilityRAT_ContainerList->list.count);
        }
  }
}

  /*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_NAS_FIRST_REQ(uint8_t mod_id, uint8_t ue_index,
                                     RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete) {
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_NAS_FIRST_REQ);
    memset(&message_p->ittiMsg.s1ap_nas_first_req, 0, sizeof(s1ap_nas_first_req_t));

    eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id = get_next_ue_initial_id (mod_id);
    S1AP_NAS_FIRST_REQ (message_p).ue_initial_id = eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id;

    /* Assume that cause is coded in the same way in RRC and S1ap, just check that the value is in S1ap range */
    AssertFatal(eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause < RRC_CAUSE_LAST,
                "Establishment cause invalid (%d/%d) for eNB %d!",
                eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause, RRC_CAUSE_LAST, mod_id);
    S1AP_NAS_FIRST_REQ (message_p).establishment_cause = eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause;

    /* Forward NAS message */S1AP_NAS_FIRST_REQ (message_p).nas_pdu.buffer =
        rrcConnectionSetupComplete->dedicatedInfoNAS.buf;
    S1AP_NAS_FIRST_REQ (message_p).nas_pdu.length = rrcConnectionSetupComplete->dedicatedInfoNAS.size;

    /* Fill UE identities with available information */
    {
      S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask = UE_IDENTITIES_NONE;

      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].Initialue_identity_s_TMSI.presence) {
        /* Fill s-TMSI */
        UE_S_TMSI *s_TMSI = &eNB_rrc_inst[mod_id].Info.UE[ue_index].Initialue_identity_s_TMSI;

        S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask |= UE_IDENTITIES_s_tmsi;
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.mme_code = s_TMSI->mme_code;
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.m_tmsi = s_TMSI->m_tmsi;
        LOG_I(S1AP, "[eNB %d] Build S1AP_NAS_FIRST_REQ with s_TMSI: MME code %u M-TMSI %u ue_index %d\n",
                mod_id,
                S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.mme_code,
                S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.m_tmsi,
                ue_index);
      }

      if (rrcConnectionSetupComplete->registeredMME != NULL) {
        /* Fill GUMMEI */
        struct RegisteredMME *r_mme = rrcConnectionSetupComplete->registeredMME;

        S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask |= UE_IDENTITIES_gummei;
        if (r_mme->plmn_Identity != NULL) {
          if ((r_mme->plmn_Identity->mcc != NULL) && (r_mme->plmn_Identity->mcc->list.count > 0)) {
            /* Use first indicated PLMN MCC if it is defined */
            S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mcc = *r_mme->plmn_Identity->mcc->list.array[0];
            LOG_I(S1AP, "[eNB %d] Build S1AP_NAS_FIRST_REQ adding in s_TMSI: GUMMEI MCC %u ue_index %d\n",
                    mod_id,
                    S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mcc,
                    ue_index);
          }
          if (r_mme->plmn_Identity->mnc.list.count > 0) {
            /* Use first indicated PLMN MNC if it is defined */
            S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mnc = *r_mme->plmn_Identity->mnc.list.array[0];
            LOG_I(S1AP, "[eNB %d] Build S1AP_NAS_FIRST_REQ adding in s_TMSI: GUMMEI MNC %u ue_index %d\n",
                    mod_id,
                    S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mnc,
                    ue_index);
          }
        }
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_code     = BIT_STRING_to_uint8 (&r_mme->mmec);
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_group_id = BIT_STRING_to_uint16 (&r_mme->mmegi);
        LOG_I(S1AP, "[eNB %d] Build S1AP_NAS_FIRST_REQ adding in s_TMSI: GUMMEI mme_code %u mme_group_id %u ue_index %d\n",
                mod_id,
                S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_code,
                S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_group_id,
                ue_index);
      }
    }
    itti_send_msg_to_task (TASK_S1AP, mod_id, message_p);
  }
#else
  {
    s1ap_eNB_new_data_request (mod_id, ue_index,
        rrcConnectionSetupComplete->dedicatedInfoNAS.
        buf,
        rrcConnectionSetupComplete->dedicatedInfoNAS.
        size);
  }
#endif
}

# if defined(ENABLE_ITTI)
/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_DOWNLINK_NAS(MessageDef *msg_p, const char *msg_name, instance_t instance, mui_t *rrc_eNB_mui) {
  uint16_t ue_initial_id;
  uint32_t eNB_ue_s1ap_id;
  uint8_t ue_index;
  uint32_t length;
  uint8_t *buffer;

  ue_initial_id = S1AP_DOWNLINK_NAS (msg_p).ue_initial_id;
  eNB_ue_s1ap_id = S1AP_DOWNLINK_NAS (msg_p).eNB_ue_s1ap_id;
  ue_index = get_UE_index_from_s1ap_ids (instance, ue_initial_id, eNB_ue_s1ap_id);

  LOG_I(RRC, "[eNB %d] Received %s: ue_initial_id %d, eNB_ue_s1ap_id %d, ue_index %d\n", instance, msg_name, ue_initial_id, eNB_ue_s1ap_id, ue_index);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
    MessageDef *msg_fail_p;

    LOG_W(RRC, "[eNB %d] In S1AP_DOWNLINK_NAS: unknown UE from S1AP ids (%d, %d)\n", instance, ue_initial_id, eNB_ue_s1ap_id);

    msg_fail_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_NAS_NON_DELIVERY_IND);
    S1AP_NAS_NON_DELIVERY_IND (msg_fail_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;
    S1AP_NAS_NON_DELIVERY_IND (msg_fail_p).nas_pdu.length = S1AP_DOWNLINK_NAS (msg_p).nas_pdu.length;
    S1AP_NAS_NON_DELIVERY_IND (msg_fail_p).nas_pdu.buffer = S1AP_DOWNLINK_NAS (msg_p).nas_pdu.buffer;

    // TODO add failure cause when defined!

    itti_send_msg_to_task (TASK_S1AP, instance, msg_fail_p);

    return (-1);
  }
  else {
    /* Is it the first income from S1AP ? */
    if (eNB_rrc_inst[instance].Info.UE[ue_index].eNB_ue_s1ap_id == 0) {
      eNB_rrc_inst[instance].Info.UE[ue_index].eNB_ue_s1ap_id = S1AP_DOWNLINK_NAS (msg_p).eNB_ue_s1ap_id;
    }
 
   /* Create message for PDCP (DLInformationTransfer_t) */
    length = do_DLInformationTransfer (instance, &buffer, rrc_eNB_get_next_transaction_identifier (instance),
                                       S1AP_DOWNLINK_NAS (msg_p).nas_pdu.length,
                                       S1AP_DOWNLINK_NAS (msg_p).nas_pdu.buffer);

#ifdef RRC_MSG_PRINT
    int i=0;
    LOG_F(RRC,"[MSG] RRC DL Information Transfer\n");
    for (i = 0; i < length; i++)
      LOG_F(RRC,"%02x ", ((uint8_t*)buffer)[i]);
    LOG_F(RRC,"\n");
#endif
    /* Transfer data to PDCP */
    pdcp_rrc_data_req (instance, ue_index, 0 /* TODO put frame number ! */, 1, DCCH, *rrc_eNB_mui++, 0,
                       length, buffer, 1);

    return (0);
  }
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance) {
  uint16_t               ue_initial_id;
  uint32_t               eNB_ue_s1ap_id;
  uint8_t                ue_index;
  MessageDef            *message_gtpv1u_p = NULL;

    ue_initial_id  = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).ue_initial_id;
    eNB_ue_s1ap_id = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id;
    ue_index       = get_UE_index_from_s1ap_ids (instance, ue_initial_id, eNB_ue_s1ap_id);

    LOG_I(RRC, "[eNB %d] Received %s: ue_initial_id %d, eNB_ue_s1ap_id %d, nb_of_e_rabs %d, ue_index %d\n",
        instance, msg_name, ue_initial_id, eNB_ue_s1ap_id, S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).nb_of_e_rabs, ue_index);

    if (ue_index == UE_INDEX_INVALID) {
        /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
        MessageDef *msg_fail_p = NULL;

        LOG_W(RRC, "[eNB %d] In S1AP_INITIAL_CONTEXT_SETUP_REQ: unknown UE from S1AP ids (%d, %d)\n", instance, ue_initial_id, eNB_ue_s1ap_id);

        msg_fail_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_INITIAL_CONTEXT_SETUP_FAIL);
        S1AP_INITIAL_CONTEXT_SETUP_FAIL (msg_fail_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;

        // TODO add failure cause when defined!

        itti_send_msg_to_task (TASK_S1AP, instance, msg_fail_p);

        return (-1);
    }
    else {

        eNB_rrc_inst[instance].Info.UE[ue_index].eNB_ue_s1ap_id = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id;

        /* Save e RAB information for later */
        {
            int i;

            message_gtpv1u_p = itti_alloc_new_message(TASK_S1AP, GTPV1U_ENB_CREATE_TUNNEL_REQ);
            memset(&message_gtpv1u_p->ittiMsg.Gtpv1uCreateTunnelReq, 0 , sizeof(gtpv1u_enb_create_tunnel_req_t));

            eNB_rrc_inst[instance].Info.UE[ue_index].nb_of_e_rabs = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).nb_of_e_rabs;
            for (i = 0; i < eNB_rrc_inst[instance].Info.UE[ue_index].nb_of_e_rabs; i++) {
                eNB_rrc_inst[instance].Info.UE[ue_index].e_rab[i].status = E_RAB_STATUS_NEW;
                eNB_rrc_inst[instance].Info.UE[ue_index].e_rab[i].param = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).e_rab_param[i];


                GTPV1U_ENB_CREATE_TUNNEL_REQ(message_gtpv1u_p).eps_bearer_id[i]       = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).e_rab_param[i].e_rab_id;
                GTPV1U_ENB_CREATE_TUNNEL_REQ(message_gtpv1u_p).sgw_S1u_teid[i]        = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).e_rab_param[i].gtp_teid;

                memcpy(&GTPV1U_ENB_CREATE_TUNNEL_REQ(message_gtpv1u_p).sgw_addr[i],
                    &S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).e_rab_param[i].sgw_addr,
                    sizeof(transport_layer_addr_t));
            }
            GTPV1U_ENB_CREATE_TUNNEL_REQ(message_gtpv1u_p).ue_index       = ue_index; // warning put zero above
            GTPV1U_ENB_CREATE_TUNNEL_REQ(message_gtpv1u_p).num_tunnels    = i;

            itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_gtpv1u_p);
        }

        /* TODO parameters yet to process ... */
        {
            S1AP_INITIAL_CONTEXT_SETUP_REQ(msg_p).ue_ambr;
        }

        rrc_eNB_process_security (instance, ue_index, &S1AP_INITIAL_CONTEXT_SETUP_REQ(msg_p).security_capabilities);

        process_eNB_security_key (instance, ue_index, S1AP_INITIAL_CONTEXT_SETUP_REQ(msg_p).security_key);

        {
          uint8_t send_security_mode_command = TRUE;

#ifndef EXMIMO_IOT
          if ((eNB_rrc_inst[instance].ciphering_algorithm[ue_index] == SecurityAlgorithmConfig__cipheringAlgorithm_eea0)
                && (eNB_rrc_inst[instance].integrity_algorithm[ue_index] == INTEGRITY_ALGORITHM_NONE)) {
                send_security_mode_command = FALSE;
          }
#endif
          rrc_pdcp_config_security(instance, ue_index,send_security_mode_command);

          if (send_security_mode_command) {

              rrc_eNB_generate_SecurityModeCommand (instance, 0 /* TODO put frame number ! */, ue_index);
              send_security_mode_command = FALSE;
              // apply ciphering after RRC security command mode
              rrc_pdcp_config_security(instance, ue_index,send_security_mode_command);
          }
          else {
              rrc_eNB_generate_UECapabilityEnquiry (instance, 0 /* TODO put frame number ! */, ue_index);
          }
        }
        return (0);
    }
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_UE_CTXT_MODIFICATION_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance) {
  uint32_t eNB_ue_s1ap_id;
  uint8_t ue_index;

  eNB_ue_s1ap_id = S1AP_UE_CTXT_MODIFICATION_REQ (msg_p).eNB_ue_s1ap_id;
  ue_index = get_UE_index_from_eNB_ue_s1ap_id (instance, eNB_ue_s1ap_id);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
    MessageDef *msg_fail_p;

    LOG_W(RRC, "[eNB %d] In S1AP_UE_CTXT_MODIFICATION_REQ: unknown UE from eNB_ue_s1ap_id (%d) for eNB %d\n", instance, eNB_ue_s1ap_id);

    msg_fail_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_UE_CTXT_MODIFICATION_FAIL);
    S1AP_UE_CTXT_MODIFICATION_FAIL (msg_fail_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;

    // TODO add failure cause when defined!

    itti_send_msg_to_task (TASK_S1AP, instance, msg_fail_p);

    return (-1);
  }
  else {

    /* TODO parameters yet to process ... */
    {
      if (S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).present & S1AP_UE_CONTEXT_MODIFICATION_UE_AMBR) {
        S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).ue_ambr;
      }
    }

    if (S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).present & S1AP_UE_CONTEXT_MODIFICATION_UE_SECU_CAP) {
      if (rrc_eNB_process_security (instance, ue_index, &S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).security_capabilities)) {
        /* transmit the new security parameters to UE */
        rrc_eNB_generate_SecurityModeCommand (instance, 0 /* TODO put frame number ! */, ue_index);
      }
    }

    if (S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).present & S1AP_UE_CONTEXT_MODIFICATION_SECURITY_KEY) {
      process_eNB_security_key (instance, ue_index, S1AP_UE_CTXT_MODIFICATION_REQ(msg_p).security_key);

      /* TODO reconfigure lower layers... */
    }

    /* Send the response */
    {
      MessageDef *msg_resp_p;

      msg_resp_p = itti_alloc_new_message(TASK_RRC_ENB, S1AP_UE_CTXT_MODIFICATION_RESP);
      S1AP_UE_CTXT_MODIFICATION_RESP(msg_resp_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;

      itti_send_msg_to_task(TASK_S1AP, instance, msg_resp_p);
    }

    return (0);
  }
}

/*------------------------------------------------------------------------------*/
#warning "LG Note this message is only from eNB to MME, proc to be deleted"
int rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_REQ (MessageDef *msg_p, const char *msg_name, instance_t instance) {
  uint32_t eNB_ue_s1ap_id;
  uint8_t ue_index;

  eNB_ue_s1ap_id = S1AP_UE_CONTEXT_RELEASE_REQ(msg_p).eNB_ue_s1ap_id;
  ue_index = get_UE_index_from_eNB_ue_s1ap_id(instance, eNB_ue_s1ap_id);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
    MessageDef *msg_fail_p;

    LOG_W(RRC, "[eNB %d] In S1AP_UE_CONTEXT_RELEASE_REQ: unknown UE from eNB_ue_s1ap_id (%d) for eNB %d\n", instance, eNB_ue_s1ap_id);

    msg_fail_p = itti_alloc_new_message(TASK_RRC_ENB, S1AP_UE_CONTEXT_RELEASE_RESP); /* TODO change message ID. */
    S1AP_UE_CONTEXT_RELEASE_RESP(msg_fail_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;

    // TODO add failure cause when defined!

    itti_send_msg_to_task(TASK_S1AP, instance, msg_fail_p);

    return (-1);
  }
  else {
    /* TODO release context. */

    /* Send the response */
    {
      MessageDef *msg_resp_p;

      msg_resp_p = itti_alloc_new_message(TASK_RRC_ENB, S1AP_UE_CONTEXT_RELEASE_RESP);
      S1AP_UE_CONTEXT_RELEASE_RESP(msg_resp_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;

      itti_send_msg_to_task(TASK_S1AP, instance, msg_resp_p);
    }

    return (0);
  }
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_COMMAND (MessageDef *msg_p, const char *msg_name, instance_t instance) {
  uint32_t eNB_ue_s1ap_id;
  uint8_t  ue_index;

  eNB_ue_s1ap_id = S1AP_UE_CONTEXT_RELEASE_COMMAND(msg_p).eNB_ue_s1ap_id;
  ue_index = get_UE_index_from_eNB_ue_s1ap_id(instance, eNB_ue_s1ap_id);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index */
    MessageDef *msg_complete_p;

    LOG_W(RRC,
            "[eNB %d] In S1AP_UE_CONTEXT_RELEASE_COMMAND: unknown UE from eNB_ue_s1ap_id (%d) for eNB %d\n",
            instance,
            eNB_ue_s1ap_id);

    msg_complete_p = itti_alloc_new_message(TASK_RRC_ENB, S1AP_UE_CONTEXT_RELEASE_COMPLETE);
    S1AP_UE_CONTEXT_RELEASE_COMPLETE(msg_complete_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;
    itti_send_msg_to_task(TASK_S1AP, instance, msg_complete_p);

    return (-1);
  }
  else {
    rrc_eNB_generate_RRCConnectionRelease(instance, 0 /*frame*/, ue_index);
      LOG_W(RRC,
              "[eNB %d] In S1AP_UE_CONTEXT_RELEASE_COMMAND: TODO call rrc_eNB_connection_release for eNB %d\n",
              instance,
              ue_index);
    {
        int      e_rab;
        int      mod_id = 0;
        eNB_RRC_UE_INFO *UE_info = &eNB_rrc_inst[mod_id].Info.UE[ue_index];
        MessageDef *msg_delete_tunnels_p = NULL;

        msg_delete_tunnels_p = itti_alloc_new_message(TASK_RRC_ENB, GTPV1U_ENB_DELETE_TUNNEL_REQ);
        memset(&GTPV1U_ENB_DELETE_TUNNEL_REQ(msg_delete_tunnels_p),
               0,
               sizeof(GTPV1U_ENB_DELETE_TUNNEL_REQ(msg_delete_tunnels_p)));

        // do not wait response
        GTPV1U_ENB_DELETE_TUNNEL_REQ(msg_delete_tunnels_p).ue_index = ue_index;
        for (e_rab = 0; e_rab < UE_info->nb_of_e_rabs; e_rab++) {
            GTPV1U_ENB_DELETE_TUNNEL_REQ(msg_delete_tunnels_p).eps_bearer_id[GTPV1U_ENB_DELETE_TUNNEL_REQ(msg_p).num_erab++] = UE_info->enb_gtp_ebi[e_rab];
            // erase data
            UE_info->enb_gtp_teid[e_rab] = 0;
            memset(&UE_info->enb_gtp_addrs[e_rab], 0, sizeof(UE_info->enb_gtp_addrs[e_rab]));
            UE_info->enb_gtp_ebi[e_rab]  = 0;
        }
        itti_send_msg_to_task(TASK_GTPV1_U, instance, msg_delete_tunnels_p);

        MessageDef *msg_complete_p = NULL;
        msg_complete_p = itti_alloc_new_message(TASK_RRC_ENB, S1AP_UE_CONTEXT_RELEASE_COMPLETE);
        S1AP_UE_CONTEXT_RELEASE_COMPLETE(msg_complete_p).eNB_ue_s1ap_id = eNB_ue_s1ap_id;
        itti_send_msg_to_task(TASK_S1AP, instance, msg_complete_p);
    }

    return (0);
  }
}


# endif /* defined(ENABLE_ITTI) */
#endif /* defined(ENABLE_USE_MME) */
