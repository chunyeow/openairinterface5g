/*! \file rrc_eNB_S1AP.h
 * \brief rrc S1AP procedures for eNB
 * \author Laurent Winckel
 * \date 2013
 * \version 1.0
 * \company Eurecom
 * \email: laurent.winckel@eurecom.fr and navid.nikaein@eurecom.fr
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
