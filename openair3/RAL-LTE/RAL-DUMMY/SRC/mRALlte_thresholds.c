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
#define MRALLTE_THRESHOLDS_C
#include <assert.h>
#include "mRALlte_thresholds.h"
#include "mRALlte_variables.h"

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
        MIH_C_3GPP_ADDR_set(&link_identifier.link_id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP, strlen(DEFAULT_ADDRESS_3GPP));
        link_identifier.choice                   = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

        mRALlte_send_link_parameters_report_indication(&transaction_id,
                                                       &link_identifier,
                                                       &LinkParametersReportList_list);
    }
}