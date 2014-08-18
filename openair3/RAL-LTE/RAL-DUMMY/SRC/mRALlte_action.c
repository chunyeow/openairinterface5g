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

#define MRAL_MODULE
#define MRALLTE_ACTION_C
#include <assert.h>
#include "mRALlte_action.h"
#include "mRALlte_variables.h"
#include "nas_ue_ioctl.h"
#include "mRALlte_proto.h"

//-----------------------------------------------------------------------------
void mRALlte_action_request(MIH_C_Message_Link_Action_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    LIST(MIH_C_LINK_SCAN_RSP,           scan_response_set);
    MIH_C_LINK_AC_RESULT_T              link_action_result;
    MIH_C_TRANSACTION_ID_T              transaction_id;
    unsigned int                        scan_index;


    memcpy(&g_link_action, &messageP->primitive.LinkAction, sizeof(MIH_C_LINK_ACTION_T));

    status                        = MIH_C_STATUS_SUCCESS;
    link_action_result            = MIH_C_LINK_AC_RESULT_SUCCESS;
    scan_response_set_list.length = 0;

    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN) {
        for (scan_index = 0; scan_index < ralpriv->num_measures; scan_index++) {
            //MIH_C_LINK_ADDR_T
            scan_response_set_list.val[scan_index].link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
            MIH_C_3GPP_ADDR_set(&scan_response_set_list.val[scan_index].link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP, strlen(DEFAULT_ADDRESS_3GPP));
            // MIH_C_NETWORK_ID_T
            MIH_C_NETWORK_ID_set(&scan_response_set_list.val[scan_index].network_id, (u_int8_t *)PREDEFINED_MIH_NETWORK_ID, strlen(PREDEFINED_MIH_NETWORK_ID));
            // MIH_C_SIG_STRENGTH_T

            scan_response_set_list.val[scan_index].sig_strength.choice = MIH_C_SIG_STRENGTH_CHOICE_PERCENTAGE;
            scan_response_set_list.val[scan_index].sig_strength._union.percentage = ralpriv->integrated_meas_level[scan_index];
            scan_response_set_list.length += 1;
        }
        transaction_id = messageP->header.transaction_id;

        mRALlte_send_link_action_confirm(&transaction_id,
                                         &status,
                                         &scan_response_set_list,
                                         &link_action_result);

    }
    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN) {
        // TO DO
    }
    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ) {
        // TO DO
    }

    // do not make actions if SCAN required
    if (( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN) == 0) {
        switch (messageP->primitive.LinkAction.link_ac_type) {
            case MIH_C_LINK_AC_TYPE_NONE:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_NONE: NO ACTION\n", __FUNCTION__);
                break;

            case MIH_C_LINK_AC_TYPE_LINK_DISCONNECT:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_DISCONNECT: NO ACTION\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_DISCONNECT) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_LOW_POWER:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_LOW_POWER\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_LOW_POWER) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN) {
                    if ( ralpriv->pending_req_action & MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN ) {
                        if (ralpriv->state == DISCONNECTED) {
                            DEBUG("Deactivation requested, but interface already inactive ==> NO OP\n");
                            ralpriv->pending_req_status = 0;
                            mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                        } else {
                            ralpriv->pending_req_action = ralpriv->pending_req_action | MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN;
                            ralpriv->pending_req_status = 0;
                            ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                            DEBUG("Deactivation requested to NAS interface\n");
                            IAL_process_DNAS_message(IO_OBJ_CNX, IO_CMD_DEL, ralpriv->cell_id);
                            //Send immediatly a confirm, otherwise it will arrive to late and MIH-F will report a failure to the MIH-USER
                            mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, NULL, &link_action_result);
                        }
                    } else {
                        ralpriv->pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN;
                        ralpriv->pending_req_status = 0;
                        ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                        DEBUG("Deactivation requested to NAS interface\n");
                        IAL_process_DNAS_message(IO_OBJ_CNX, IO_CMD_DEL, ralpriv->cell_id);
                        //Send immediatly a confirm, otherwise it will arrive to late and MIH-F will report a failure to the MIH-USER
                        mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, NULL, &link_action_result);
                    }
                } else {
                    DEBUG ("[mRAL]: command POWER DOWN not available \n\n");
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, NULL, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_POWER_UP:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_POWER_UP\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_POWER_UP) {
                    // Activation requested - check it is not already active
                    if(ralpriv->pending_req_action & MIH_C_LINK_AC_TYPE_LINK_POWER_UP) {
                        if (ralpriv->state == CONNECTED) {
                            DEBUG("Activation requested, but interface already active ==> NO OP\n");
                            ralpriv->pending_req_status = 0;
                            mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                        } else {
                            ralpriv->pending_req_action = ralpriv->pending_req_action | MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                            ralpriv->pending_req_status = 0;
                            ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                            DEBUG("Activation requested to NAS interface\n");
                            IAL_process_DNAS_message(IO_OBJ_CNX, IO_CMD_ADD, ralpriv->cell_id);
                        }
                    } else {
                        ralpriv->pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                        ralpriv->pending_req_status = 0;
                        ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                        DEBUG("Activation requested to NAS interface\n");
                        IAL_process_DNAS_message(IO_OBJ_CNX, IO_CMD_ADD, ralpriv->cell_id);
                    }
                } else {
                    DEBUG ("[mRAL]: command POWER UP not available \n\n");
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR: NO ACTION\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES: NO ACTION\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES: NO ACTION\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES) {
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            default:
                ERR("%s Invalid LinkAction.link_ac_type in MIH_C_Message_Link_Action_request\n", __FUNCTION__);
                status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
                mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
        }
    }
}

