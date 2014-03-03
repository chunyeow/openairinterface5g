/***************************************************************************
                         lteRALue_action.c  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

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
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALue_mih_execute.c
 * \brief Execution of MIH primitives in LTE-RAL-UE
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_UE
#define LTERALUE_ACTION_C
//-----------------------------------------------------------------------------
#include "lteRALue.h"
#include "LAYER2/MAC/extern.h"

//-----------------------------------------------------------------------------
void mRAL_action_request(ral_ue_instance_t instanceP, MIH_C_Message_Link_Action_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                         status;
    LIST(MIH_C_LINK_SCAN_RSP,              scan_response_set);
    MIH_C_LINK_AC_RESULT_T                 link_action_result;
    //unsigned int                           scan_index, meas_to_send;
    MessageDef                            *message_p = NULL;
    rrc_ral_connection_release_req_t       release_req;
    rrc_ral_connection_establishment_req_t connection_establishment_req;
    module_id_t                            mod_id = instanceP - NB_eNB_INST;

    status             = MIH_C_STATUS_SUCCESS;
    link_action_result = MIH_C_LINK_AC_RESULT_SUCCESS;
    scan_response_set_list.length = 0;

    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN) {
        //----------------------------------------------------
        // send a response to MIH-F or it will report an error.
        //----------------------------------------------------
        //link_action_result = MIH_C_LINK_AC_RESULT_SUCCESS;
        //mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);

        //----------------------------------------------------
        // Transmit request to RRC.
        //----------------------------------------------------
        message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_SCAN_REQ);
        RRC_RAL_SCAN_REQ(message_p).transaction_id  = messageP->header.transaction_id;
        itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);
    }
    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN) {
        // TO DO
        // The link will be disconnected but the resource for the link connection still remains so
        // reestablishing the link connection later can be more efficient.

    }
    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ) {
        // TO DO
        // This indication requires the buffered data at the old serving PoA entity to be forwarded
        // to the new target PoA entity in order to avoid data loss. This action can be taken imme-
        // diately after the old serving PoS receives MIH_N2N_HO_Commit response message
        // from the new target PoS, or the old serving PoS receives MIH_Net_HO_Commit
        // response message from the MN. This is not valid on UMTS link type.

    }

    // do not make actions if SCAN required
    if (( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN) == 0) {
        switch (messageP->primitive.LinkAction.link_ac_type) {
            case MIH_C_LINK_AC_TYPE_NONE:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_NONE: NO ACTION\n", __FUNCTION__);
                break;

            case MIH_C_LINK_AC_TYPE_LINK_DISCONNECT:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_DISCONNECT: NO ACTION\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_DISCONNECT) {
                    message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_CONNECTION_RELEASE_REQ);
                    memset(&release_req, 0, sizeof(rrc_ral_connection_release_req_t));
                    // copy transaction id
                    release_req.transaction_id  = messageP->header.transaction_id;
                    memcpy (&message_p->ittiMsg, (void *) &release_req, sizeof(rrc_ral_connection_release_req_t));
                    itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_LOW_POWER:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_LOW_POWER\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_LOW_POWER) {
                // TO DO
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN) {
                    if ( g_ue_ral_obj[mod_id].pending_req_action & MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN ) {
                        if (g_ue_ral_obj[mod_id].state == DISCONNECTED) {
                            LOG_D(RAL_UE, "Deactivation requested, but interface already inactive ==> NO OP\n");
                            mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                        } else {
                            g_ue_ral_obj[mod_id].pending_req_action = g_ue_ral_obj[mod_id].pending_req_action | MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN;
                            //Send immediatly a confirm, otherwise it will arrive to late and MIH-F will report a failure to the MIH-USER
                            mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, NULL, &link_action_result);

                            message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_CONNECTION_RELEASE_REQ);
                            memset(&release_req, 0, sizeof(rrc_ral_connection_release_req_t));
                            // copy transaction id
                            release_req.transaction_id  = messageP->header.transaction_id;
                            memcpy (&message_p->ittiMsg, (void *) &release_req, sizeof(rrc_ral_connection_release_req_t));
                            itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);
                            LOG_D(RAL_UE, "Deactivation requested to NAS interface\n");
//RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_DEL, g_ue_ral_obj[mod_id].cell_id);

                        }
                    } else {
                        g_ue_ral_obj[mod_id].pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN;
                        //Send immediatly a confirm, otherwise it will arrive to late and MIH-F will report a failure to the MIH-USER
                        mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, NULL, &link_action_result);

                        message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_CONNECTION_RELEASE_REQ);
                        memset(&release_req, 0, sizeof(rrc_ral_connection_release_req_t));
                        // copy transaction id
                        release_req.transaction_id  = messageP->header.transaction_id;
                        memcpy (&message_p->ittiMsg, (void *) &release_req, sizeof(rrc_ral_connection_release_req_t));
                        itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);
                        LOG_D(RAL_UE, "Deactivation requested to NAS interface\n");
//RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_DEL, g_ue_ral_obj[mod_id].cell_id);
                    }
                } else {
                    LOG_D(RAL_UE, " command POWER DOWN not available \n\n");
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, NULL, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_POWER_UP:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_POWER_UP\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_POWER_UP) {
                    // Activation requested - check it is not already active
                    if(g_ue_ral_obj[mod_id].pending_req_action & MIH_C_LINK_AC_TYPE_LINK_POWER_UP) {
                        if (g_ue_ral_obj[mod_id].state == CONNECTED) {
                            LOG_D(RAL_UE, "Activation requested, but interface already active ==> NO OP\n");
                            mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                        } else {
                            g_ue_ral_obj[mod_id].pending_req_action = g_ue_ral_obj[mod_id].pending_req_action | MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                            g_ue_ral_obj[mod_id].cell_id = g_ue_ral_obj[mod_id].meas_cell_id[0];  // Default cell #0 - Next, choose cell with best conditions
                            LOG_D(RAL_UE, "Activation requested to NAS interface on cell %d\n", g_ue_ral_obj[mod_id].cell_id);
//RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_ADD, g_ue_ral_obj[mod_id].cell_id);
                        }
                    } else {
                        g_ue_ral_obj[mod_id].pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                        g_ue_ral_obj[mod_id].cell_id = g_ue_ral_obj[mod_id].meas_cell_id[0]; // Default cell #0 - Next, choose cell with best conditions
                        message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_CONNECTION_ESTABLISHMENT_REQ);
                        memset(&connection_establishment_req, 0, sizeof(rrc_ral_connection_establishment_req_t));
                        // copy transaction id
                        connection_establishment_req.transaction_id  = messageP->header.transaction_id;
                        memcpy (&message_p->ittiMsg, (void *) &connection_establishment_req, sizeof(rrc_ral_connection_establishment_req_t));
                        itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);
                    }
                } else {
                    LOG_D(RAL_UE, "[mRAL]: command POWER UP not available \n\n");
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

/* LG KEEP  case MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR: NO ACTION\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES: NO ACTION\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES:
                LOG_D(RAL_UE, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES: NO ACTION\n", __FUNCTION__);
                if (g_ue_ral_obj[mod_id].mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;*/

            default:
                LOG_E(RAL_UE, "%s Invalid LinkAction.link_ac_type in MIH_C_Message_Link_Action_request\n", __FUNCTION__);
                status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
                mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
        }
    }
}

