/***************************************************************************
                         lteRALue_mih_execute.c  -  description
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
#define MRAL_MODULE
#define MRALLTE_MIH_EXECUTE_C
#include <assert.h>
#include "lteRALue_mih_execute.h"
#include "lteRALue_variables.h"
#include "nas_ue_ioctl.h"
#include "lteRALue_proto.h"

//-----------------------------------------------------------------------------
void mRALlte_action_request(MIH_C_Message_Link_Action_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T status;
    LIST(MIH_C_LINK_SCAN_RSP, scan_response_set);
    MIH_C_LINK_AC_RESULT_T link_action_result;
    MIH_C_TRANSACTION_ID_T transaction_id;
    unsigned int scan_index, meas_to_send;


    memcpy(&g_link_action, &messageP->primitive.LinkAction, sizeof(MIH_C_LINK_ACTION_T));

    status = MIH_C_STATUS_SUCCESS;
    link_action_result = MIH_C_LINK_AC_RESULT_SUCCESS;
    scan_response_set_list.length = 0;
    meas_to_send = ralpriv->num_measures;
    #ifdef RAL_REALTIME
    meas_to_send = 1;  // MW-TEMP - For real Time, block reporting to one measure only
    #endif

    if ( messageP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN) {
        for (scan_index = 0; scan_index < meas_to_send; scan_index++) {
            //MIH_C_LINK_ADDR_T - MW-TEMP, set to DEFAULT_ADDRESS_eNB (should not be UE address here)
            scan_response_set_list.val[scan_index].link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
            //MIH_C_3GPP_ADDR_set(&scan_response_set_list.val[scan_index].link_addr._union._3gpp_addr, (u_int8_t*)&(ralpriv->ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));
            MIH_C_3GPP_ADDR_set(&scan_response_set_list.val[scan_index].link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_eNB, strlen(DEFAULT_ADDRESS_eNB));
            // MIH_C_NETWORK_ID_T
            MIH_C_NETWORK_ID_set(&scan_response_set_list.val[scan_index].network_id, (u_int8_t *)PREDEFINED_MIH_NETWORK_ID, strlen(PREDEFINED_MIH_NETWORK_ID));
            // MIH_C_SIG_STRENGTH_T

            scan_response_set_list.val[scan_index].sig_strength.choice = MIH_C_SIG_STRENGTH_CHOICE_PERCENTAGE;
            scan_response_set_list.val[scan_index].sig_strength._union.percentage = ralpriv->integrated_meas_level[scan_index];
            scan_response_set_list.length += 1;
        }
        transaction_id = messageP->header.transaction_id;

        mRALlte_send_link_action_confirm(&transaction_id, &status, &scan_response_set_list, &link_action_result);
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
				// TO DO
                } else {
                    link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
                    ralpriv->pending_req_status = 0;
                    mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            case MIH_C_LINK_AC_TYPE_LINK_LOW_POWER:
                DEBUG("%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_LOW_POWER\n", __FUNCTION__);
                if (ralpriv->mih_supported_action_list  & MIH_C_LINK_AC_TYPE_LINK_LOW_POWER) {
				// TO DO
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
                            RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_DEL, ralpriv->cell_id);
                            //Send immediatly a confirm, otherwise it will arrive to late and MIH-F will report a failure to the MIH-USER
                            mRALlte_send_link_action_confirm(&messageP->header.transaction_id, &status, NULL, &link_action_result);
                        }
                    } else {
                        ralpriv->pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN;
                        ralpriv->pending_req_status = 0;
                        ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                        DEBUG("Deactivation requested to NAS interface\n");
                        RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_DEL, ralpriv->cell_id);
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
                            ralpriv->cell_id = ralpriv->meas_cell_id[0];  // Default cell #0 - Next, choose cell with best conditions
                            DEBUG("Activation requested to NAS interface on cell %d\n", ralpriv->cell_id);
                            RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_ADD, ralpriv->cell_id);
                        }
                    } else {
                        ralpriv->pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                        ralpriv->pending_req_status = 0;
                        ralpriv->pending_req_transaction_id = messageP->header.transaction_id;
                        ralpriv->cell_id = ralpriv->meas_cell_id[0]; // Default cell #0 - Next, choose cell with best conditions
                        DEBUG("Activation requested to NAS interface on cell %d\n", ralpriv->cell_id);
                        RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_ADD, ralpriv->cell_id);
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

//-----------------------------------------------------------------------------
void mRALlte_get_parameters_request(MIH_C_Message_Link_Get_Parameters_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_PARAM_LIST_T             link_parameters_status_list;
    MIH_C_LINK_STATES_RSP_LIST_T        link_states_response_list;
    MIH_C_LINK_DESC_RSP_LIST_T          link_descriptors_response_list;
    unsigned int                        link_index;

    // SAVE REQUEST
    // MAY BE MERGE REQUESTS ?
    //memcpy(&g_link_cfg_param_thresholds_list, &messageP->primitive.LinkConfigureParameterList_list, sizeof(MIH_C_LINK_CFG_PARAM_LIST_T));

    status       = MIH_C_STATUS_SUCCESS;

    for (link_index = 0;
         link_index < messageP->primitive.LinkParametersRequest_list.length;
         link_index++) {

        //------------------------------------------------
        //  MIH_C_LINK_PARAM_LIST_T
        //------------------------------------------------
        memcpy(&link_parameters_status_list.val[link_index].link_param_type,
               &messageP->primitive.LinkParametersRequest_list.val[link_index],
               sizeof(MIH_C_LINK_PARAM_TYPE_T));

        switch (messageP->primitive.LinkParametersRequest_list.val[link_index].choice) {
            case MIH_C_LINK_PARAM_TYPE_CHOICE_GEN:
                link_parameters_status_list.val[link_index].choice = MIH_C_LINK_PARAM_CHOICE_LINK_PARAM_VAL;
                link_parameters_status_list.val[link_index]._union.link_param_val = MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH;
                break;
            case MIH_C_LINK_PARAM_TYPE_CHOICE_QOS:
                link_parameters_status_list.val[link_index].choice = MIH_C_LINK_PARAM_CHOICE_QOS_PARAM_VAL;
                link_parameters_status_list.val[link_index]._union.qos_param_val.choice = MIH_C_QOS_PARAM_VAL_CHOICE_AVG_PK_TX_DELAY;
                link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.length        = 2; //??
                link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[0].cos_id = 2; //??
                link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[0].value  = 20; //??
                link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[1].cos_id = 3; //??
                link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[2].value  = 50; //??
                break;
            case MIH_C_LINK_PARAM_TYPE_CHOICE_LTE:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_GG:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_EDGE:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_ETH:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_802_11:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_C2K:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_FDD:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_HRPD:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_802_16:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_802_20:
            case MIH_C_LINK_PARAM_TYPE_CHOICE_802_22:
            default:
                 ERR("%s TO DO CONTINUE PROCESSING LinkParametersRequest_list of \n", __FUNCTION__);
        }

        //------------------------------------------------
        //  MIH_C_LINK_STATES_RSP_LIST_T
        //------------------------------------------------
        if (messageP->primitive.LinkStatesRequest & MIH_C_BIT_LINK_STATES_REQ_OP_MODE) {
            link_states_response_list.val[link_index].choice         = 0;
            link_states_response_list.val[link_index]._union.op_mode = MIH_C_OPMODE_NORMAL_MODE;
        } else if (messageP->primitive.LinkStatesRequest & MIH_C_BIT_LINK_STATES_REQ_CHANNEL_ID) {
            link_states_response_list.val[link_index].choice            = 1;
            link_states_response_list.val[link_index]._union.channel_id = PREDEFINED_CHANNEL_ID;
        } else {
            ERR("%s Invalid LinkStatesRequest in MIH_C_Link_Get_Parameters_request\n", __FUNCTION__);
            // DEFAULT VALUES
            link_states_response_list.val[link_index].choice         = 0;
            link_states_response_list.val[link_index]._union.op_mode = MIH_C_OPMODE_NORMAL_MODE;
        }

        //------------------------------------------------
        // MIH_C_LINK_DESC_RSP_LIST_T
        //------------------------------------------------
        if (messageP->primitive.LinkDescriptorsRequest & MIH_C_BIT_NUMBER_OF_CLASSES_OF_SERVICE_SUPPORTED) {
            link_descriptors_response_list.val[link_index].choice         = 0;
            link_descriptors_response_list.val[link_index]._union.num_cos = PREDEFINED_CLASSES_SERVICE_SUPPORTED;
        } else if (messageP->primitive.LinkDescriptorsRequest & MIH_C_BIT_NUMBER_OF_QUEUES_SUPPORTED) {
            link_descriptors_response_list.val[link_index].choice            = 1;
            link_descriptors_response_list.val[link_index]._union.num_queue = PREDEFINED_QUEUES_SUPPORTED;
        } else {
            ERR("%s Invalid LinkDescriptorsRequest in MIH_C_Link_Get_Parameters_request\n", __FUNCTION__);
            // DEFAULT VALUES
            link_descriptors_response_list.val[link_index].choice         = 0;
            link_descriptors_response_list.val[link_index]._union.num_cos = PREDEFINED_CLASSES_SERVICE_SUPPORTED;
        }
    }
    link_parameters_status_list.length    = messageP->primitive.LinkParametersRequest_list.length;
    link_states_response_list.length      = messageP->primitive.LinkParametersRequest_list.length;
    link_descriptors_response_list.length = messageP->primitive.LinkParametersRequest_list.length;


    mRALte_send_get_parameters_confirm(&messageP->header.transaction_id,
                                             &status,
                                             &link_parameters_status_list,
                                             &link_states_response_list,
                                             &link_descriptors_response_list);
}

//-----------------------------------------------------------------------------
void mRALlte_subscribe_request(MIH_C_Message_Link_Event_Subscribe_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_EVENT_LIST_T             mih_subscribed_req_event_list;

    ralpriv->mih_subscribe_req_event_list |= (messageP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list);

    mih_subscribed_req_event_list = ralpriv->mih_subscribe_req_event_list & messageP->primitive.RequestedLinkEventList;

    status = MIH_C_STATUS_SUCCESS;

    mRALte_send_event_subscribe_confirm(&messageP->header.transaction_id,
                                        &status,
                                        &mih_subscribed_req_event_list);

}
//-----------------------------------------------------------------------------
void mRALlte_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_EVENT_LIST_T             mih_unsubscribed_req_event_list;
    MIH_C_LINK_EVENT_LIST_T             saved_req_event_list;

    saved_req_event_list           = ralpriv->mih_subscribe_req_event_list;

    ralpriv->mih_subscribe_req_event_list &= ((messageP->primitive.RequestedLinkEventList & ralpriv->mih_supported_link_event_list) ^
                                                messageP->primitive.RequestedLinkEventList);

    mih_unsubscribed_req_event_list = ralpriv->mih_subscribe_req_event_list ^ saved_req_event_list;

    status = MIH_C_STATUS_SUCCESS;

    mRALte_send_event_unsubscribe_confirm(&messageP->header.transaction_id,
                                        &status,
                                        &mih_unsubscribed_req_event_list);
}

//-----------------------------------------------------------------------------
void mRALlte_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* messageP) {
//-----------------------------------------------------------------------------
    MIH_C_STATUS_T                      status;
    MIH_C_LINK_CFG_STATUS_LIST_T        link_cfg_status_list;
    unsigned int                        threshold_index;
    unsigned int                        link_index;
    unsigned int                        result_index;

    // SAVE REQUEST
    // IT IS ASSUMED SINCE IT IS NOT CLEAR IN SPECs THAT THERE IS NO NEED TO MERGE CONFIGURE_THRESHOLDS_requests
    //memset(&ralpriv->mih_link_cfg_param_thresholds_list, 0, sizeof(MIH_C_LINK_CFG_PARAM_LIST_T));
    memcpy(&ralpriv->mih_link_cfg_param_thresholds_list, &messageP->primitive.LinkConfigureParameterList_list, sizeof(MIH_C_LINK_CFG_PARAM_LIST_T));

    status = MIH_C_STATUS_SUCCESS;

    result_index = 0;

    for (link_index = 0;
         link_index < messageP->primitive.LinkConfigureParameterList_list.length;
         link_index++) {

        ralpriv->active_mih_link_cfg_param_threshold[link_index] = MIH_C_BOOLEAN_TRUE;
        for (threshold_index = 0;
            threshold_index < messageP->primitive.LinkConfigureParameterList_list.val[link_index].threshold_list.length;
            threshold_index ++) {

            memcpy(&link_cfg_status_list.val[result_index].link_param_type,
            &messageP->primitive.LinkConfigureParameterList_list.val[link_index].link_param_type,
            sizeof(MIH_C_LINK_PARAM_TYPE_T));

            memcpy(&link_cfg_status_list.val[result_index].threshold,
            &messageP->primitive.LinkConfigureParameterList_list.val[link_index].threshold_list.val[threshold_index],
            sizeof(MIH_C_THRESHOLD_T));

             // NOW, ALWAYS SAY OK FOR PARAMETERS, BUT MAY BE WE WILL PUT MIH_C_BOOLEAN_FALSE in active_mih_link_cfg_param_threshold[link_index]
            link_cfg_status_list.val[result_index].config_status = MIH_C_CONFIG_STATUS_SUCCESS;

            result_index += 1;
        }
    }
    // Say following thresholds entries are not configured
    for (link_index = messageP->primitive.LinkConfigureParameterList_list.length;
         link_index < MIH_C_LINK_CFG_PARAM_LIST_LENGTH;
         link_index++) {
        ralpriv->active_mih_link_cfg_param_threshold[link_index] = MIH_C_BOOLEAN_FALSE;
    }
    link_cfg_status_list.length = result_index;

    mRALte_send_configure_thresholds_confirm(&messageP->header.transaction_id,&status, &link_cfg_status_list);
}
//-----------------------------------------------------------------------------
void mRALlte_check_thresholds_signal_strength(MIH_C_THRESHOLD_VAL_T new_valP, MIH_C_THRESHOLD_VAL_T old_valP) {
//-----------------------------------------------------------------------------

    unsigned int                        threshold_index, threshold_index_mov;
    unsigned int                        link_index;
    unsigned int                        buffer_index;
    MIH_C_THRESHOLD_VAL_T               threshold_val;
    MIH_C_THRESHOLD_XDIR_T              threshold_xdir;
    MIH_C_TH_ACTION_T                   th_action;
    LIST(MIH_C_LINK_PARAM_RPT, LinkParametersReportList);
    MIH_C_TRANSACTION_ID_T              transaction_id;
    MIH_C_LINK_TUPLE_ID_T               link_identifier;
    char                                buf[256];

    DEBUG("%s new_val %d old_val %d\n", __FUNCTION__, new_valP, old_valP);

    LinkParametersReportList_list.length = 0;
    for (link_index = 0;
         link_index < MIH_C_LINK_CFG_PARAM_LIST_LENGTH;
         link_index++) {

        if (ralpriv->active_mih_link_cfg_param_threshold[link_index] == MIH_C_BOOLEAN_TRUE) {

            if ( ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].link_param_type.choice == MIH_C_LINK_PARAM_TYPE_CHOICE_GEN) {

                th_action = ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].th_action;

                for (threshold_index = 0;
                    threshold_index < ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.length;
                    threshold_index ++) {

                    threshold_val  = ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index].threshold_val;
                    threshold_xdir = ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index].threshold_xdir;

                    switch (th_action) {
                        case MIH_C_SET_NORMAL_THRESHOLD:
                            if (((threshold_xdir == MIH_C_ABOVE_THRESHOLD) && (old_valP <= threshold_val) && (new_valP > threshold_val)) ||
                                ((threshold_xdir == MIH_C_BELOW_THRESHOLD) && (old_valP >= threshold_val) && (new_valP < threshold_val))) {

                                memset(buf, 0, 256);
                                buffer_index = sprintf(buf, "CROSSED NORMAL THRESHOLD VAL %d DIR ", threshold_val);
                                buffer_index = MIH_C_THRESHOLD_XDIR2String2(&threshold_xdir, &buf[buffer_index]);
                                buffer_index = sprintf(buf, " with VAL %d\n", new_valP);
                                NOTICE("%s", buf);

                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.link_param_type.choice = MIH_C_LINK_PARAM_TYPE_CHOICE_GEN;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.link_param_type._union.link_param_gen = MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.choice                = MIH_C_LINK_PARAM_CHOICE_LINK_PARAM_VAL;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param._union.link_param_val = new_valP;

                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].choice = MIH_C_LINK_PARAM_RPT_CHOICE_THRESHOLD;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length]._union.threshold.threshold_val  = threshold_val;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length]._union.threshold.threshold_xdir = threshold_xdir;
                                LinkParametersReportList_list.length = LinkParametersReportList_list.length + 1;
                            }
                            break;
                        case MIH_C_SET_ONE_SHOT_THRESHOLD:
                            if (((threshold_xdir == MIH_C_ABOVE_THRESHOLD) && (old_valP <= threshold_val) && (new_valP > threshold_val)) ||
                                ((threshold_xdir == MIH_C_BELOW_THRESHOLD) && (old_valP >= threshold_val) && (new_valP < threshold_val))) {

                                memset(buf, 0, 256);
                                buffer_index = sprintf(buf, "CROSSED ONE SHOT THRESHOLD VAL %d DIR ", threshold_val);
                                buffer_index = MIH_C_THRESHOLD_XDIR2String2(&threshold_xdir, &buf[buffer_index]);
                                buffer_index = sprintf(buf, " with VAL %d\n", new_valP);
                                NOTICE("%s", buf);

                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.link_param_type.choice = MIH_C_LINK_PARAM_TYPE_CHOICE_GEN;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.link_param_type._union.link_param_gen = MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param.choice                = MIH_C_LINK_PARAM_CHOICE_LINK_PARAM_VAL;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].link_param._union.link_param_val = new_valP;

                                LinkParametersReportList_list.val[LinkParametersReportList_list.length].choice = MIH_C_LINK_PARAM_RPT_CHOICE_THRESHOLD;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length]._union.threshold.threshold_val  = threshold_val;
                                LinkParametersReportList_list.val[LinkParametersReportList_list.length]._union.threshold.threshold_xdir = threshold_xdir;
                                LinkParametersReportList_list.length = LinkParametersReportList_list.length + 1;

                                // Remove this threshold: shift Thresholds
                                for (threshold_index_mov = threshold_index;
                                    threshold_index_mov < ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.length - 1;
                                    threshold_index_mov ++) {
                                    memcpy(&ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index_mov],
                                        &ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index_mov+1],
                                        sizeof(MIH_C_THRESHOLD_T));
                                }
                            }
                            break;
                        // may be not necessary here
                        case MIH_C_CANCEL_THRESHOLD:
                            // shift Thresholds
                            for (threshold_index_mov = threshold_index;
                                threshold_index_mov < ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.length - 1;
                                threshold_index_mov ++) {
                                memcpy(&ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index_mov],
                                       &ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.val[threshold_index_mov+1],
                                    sizeof(MIH_C_THRESHOLD_T));
                            }
                            ralpriv->mih_link_cfg_param_thresholds_list.val[link_index].threshold_list.length -= 1;
                            break;
                        default:
                            ERR("%s UNKNOWN TH ACTION FOUND, RETURN", __FUNCTION__);
                            return;
                    }
                }
            }
        }
    }
    if (LinkParametersReportList_list.length > 0) {
        transaction_id = MIH_C_get_new_transaction_id();

        link_identifier.link_id.link_type        = MIH_C_WIRELESS_UMTS;
        link_identifier.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
        MIH_C_3GPP_ADDR_set(&link_identifier.link_id.link_addr._union._3gpp_addr, (u_int8_t*)&(ralpriv->ipv6_l2id[0]), strlen(DEFAULT_ADDRESS_3GPP));
        link_identifier.choice                   = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

        mRALlte_send_link_parameters_report_indication(&transaction_id,  &link_identifier, &LinkParametersReportList_list);
    }
}

