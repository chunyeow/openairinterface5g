/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_thresholds.c
 *
 * Version 0.1
 *
 * Date  07/03/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem 
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description 
 *
 *****************************************************************************/

#include "lteRALenb_thresholds.h"

#include "lteRALenb_mih_msg.h"
#include "lteRALenb_variables.h"
#include "lteRALenb_constants.h"
#include "lteRALenb_proto.h"

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRALlte_configure_thresholds_request()                    **
 **                                                                        **
 ** Description: Processes the Link_Configure_Thresholds.request message   **
 **   and sends a Link_Configure_Thresholds.confirm message to  **
 **   the MIHF.                                                 **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received message            **
 **     Others: ralpriv                                    **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
void eRALlte_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_SUCCESS;
    unsigned int index;
    //unsigned int temp_polling_interval;

    DEBUG("\n");
    //DEBUG(" Configure thresholds request only returns success status to MIH_USER\n");

    // SAVE REQUEST
    memcpy(&ralpriv->mih_link_cfg_param_thresholds_list, &msgP->primitive.LinkConfigureParameterList_list, sizeof(MIH_C_LINK_CFG_PARAM_LIST_T));

    for (index = 0; index < ralpriv->mih_link_cfg_param_thresholds_list.length; index++) {
        ralpriv->active_mih_link_cfg_param_threshold[index] = MIH_C_BOOLEAN_TRUE;
        if  ( ralpriv->mih_link_cfg_param_thresholds_list.val[index].th_action== MIH_C_SET_NORMAL_THRESHOLD){
          ralpriv->measures_triggered_flag = RAL_TRUE;
          // read period 
          ralpriv->requested_period = ralpriv->mih_link_cfg_param_thresholds_list.val[index]._union.timer_interval;
          ralpriv->meas_polling_interval = 1 + ((ralpriv->requested_period *1000)/ MIH_C_RADIO_POLLING_INTERVAL_MICRO_SECONDS);
          //DEBUG(" Polling interval %d\n\n\n", temp_polling_interval);
          ralpriv->meas_polling_counter = 1;
          break;
        } else if  ( ralpriv->mih_link_cfg_param_thresholds_list.val[index].th_action == MIH_C_CANCEL_THRESHOLD){
          ralpriv->measures_triggered_flag = RAL_FALSE;
          // read period 
          ralpriv->requested_period = 0;
          ralpriv->meas_polling_interval = RAL_DEFAULT_MEAS_POLLING_INTERVAL;
          ralpriv->meas_polling_counter = 1;
          break;
        }
    }
    DEBUG(" Measurement values configured : Measures active %d, Requested period %d, Polling interval %d\n\n", 
          ralpriv->measures_triggered_flag, ralpriv->requested_period, ralpriv->meas_polling_interval);

    eRALlte_send_configure_thresholds_confirm(&msgP->header.transaction_id, &status, NULL);

/*    MIH_C_STATUS_T                      status;
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

    eRALte_send_configure_thresholds_confirm(&messageP->header.transaction_id,&status, &link_cfg_status_list);
*/
}

