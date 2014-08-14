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
/*! \file rrc_eNB_S1AP.h
 * \brief rrc S1AP procedures for eNB
 * \author Laurent Winckel and Navid Nikaein
 * \date 2013
 * \version 1.0
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr
 */

#ifndef RRC_ENB_S1AP_H_
#define RRC_ENB_S1AP_H_

# if defined(ENABLE_USE_MME)

#include "UL-DCCH-Message.h"

/* Up link procedures */

#   if defined(ENABLE_ITTI)
/*! \fn void rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP(uint8_t mod_id, uint8_t ue_index)
 *\brief create a S1AP_INITIAL_CONTEXT_SETUP_RESP for S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 */
void rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP(uint8_t Mod_id, uint8_t UE_index);
#   endif

/*! \fn void rrc_eNB_send_S1AP_UPLINK_NAS(uint8_t Mod_id, uint8_t UE_index, UL_DCCH_Message_t *ul_dcch_msg)
 *\brief create a S1AP_UPLINK_NAS to transfer a NAS message to S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 *\param ul_dcch_msg The message receive by RRC holding the NAS message.
 */
void rrc_eNB_send_S1AP_UPLINK_NAS(uint8_t Mod_id, uint8_t UE_index, UL_DCCH_Message_t *ul_dcch_msg);

/*! \fn void rrc_eNB_send_S1AP_UE_CAPABILITIES_IND(uint8_t Mod_id, uint8_t UE_index, UL_DCCH_Message_t *ul_dcch_msg)
 *\brief create a S1AP_UE_CAPABILITIES_IND to transfer a NAS message to S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 *\param ul_dcch_msg The message receive by RRC holding the NAS message.
 */
void rrc_eNB_send_S1AP_UE_CAPABILITIES_IND(uint8_t mod_id, uint8_t ue_index, UL_DCCH_Message_t *ul_dcch_msg);

/*! \fn rrc_eNB_send_S1AP_NAS_FIRST_REQ(uint8_t Mod_id, uint8_t UE_index, RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete)
 *\brief create a S1AP_NAS_FIRST_REQ to indicate that RRC has completed its first connection setup to S1AP.
 *\brief eventually forward a NAS message to S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_index Instance ID of UE in the eNB.
 *\param rrcConnectionSetupComplete The message receive by RRC that may hold the NAS message.
 */
void rrc_eNB_send_S1AP_NAS_FIRST_REQ(uint8_t Mod_id, uint8_t UE_index,
                                     RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete);

/* Down link procedures */

#   if defined(ENABLE_ITTI)
/*! \fn rrc_eNB_process_S1AP_DOWNLINK_NAS(MessageDef *msg_p, const char *msg_name, instance_t instance, mui_t *rrc_eNB_mui)
 *\brief process a S1AP_DOWNLINK_NAS message received from S1AP and transfer the embedded NAS message to UE.
 *\param msg_p Message received by RRC.
 *\param msg_name Message name.
 *\param instance Message instance.
 *\param rrc_eNB_mui Counter for lower level message identification.
 *\return 0 when successful, -1 if the UE index can not be retrieved.
 */
int rrc_eNB_process_S1AP_DOWNLINK_NAS(MessageDef *msg_p, const char *msg_name, instance_t instance, mui_t *rrc_eNB_mui);

/*! \fn rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance)
 *\brief process a S1AP_INITIAL_CONTEXT_SETUP_REQ message received from S1AP.
 *\param msg_p Message received by RRC.
 *\param msg_name Message name.
 *\param instance Message instance.
 *\return 0 when successful, -1 if the UE index can not be retrieved.
 */
int rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance);

/*! \fn rrc_eNB_process_S1AP_UE_CTXT_MODIFICATION_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance)
 *\brief process a S1AP_UE_CTXT_MODIFICATION_REQ message received from S1AP.
 *\param msg_p Message received by RRC.
 *\param msg_name Message name.
 *\param instance Message instance.
 *\return 0 when successful, -1 if the UE index can not be retrieved.
 */
int rrc_eNB_process_S1AP_UE_CTXT_MODIFICATION_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance);

/*! \fn rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance)
 *\brief process a S1AP_UE_CONTEXT_RELEASE_REQ message received from S1AP.
 *\param msg_p Message received by RRC.
 *\param msg_name Message name.
 *\param instance Message instance.
 *\return 0 when successful, -1 if the UE index can not be retrieved.
 */
int rrc_eNB_process_S1AP_UE_CONTEXT_RELEASE_REQ (MessageDef *msg_p, const char *msg_name, instance_t instance);

#   endif
# endif /* defined(ENABLE_USE_MME) */
#endif /* RRC_ENB_S1AP_H_ */
