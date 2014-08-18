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
#define MRALLTE_PARAMETERS_C
#include <assert.h>
#include "mRALlte_parameters.h"

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

