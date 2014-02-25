/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRAL_thresholds.c
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
#define LTE_RAL_ENB
#define LTE_RAL_ENB_THRESHOLDS_C
#include <assert.h>
#include "lteRALenb.h"


/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_configure_thresholds_request()                             **
 **                                                                        **
 ** Description: Forwards the Link_Configure_Thresholds.request message    **
 **   to the RRC layer.                                                    **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received message                        **
 **     Others: ralpriv                                                    **
 **                                                                        **
 ** Outputs:  None                                                         **
 ** Return:   None                                                         **
 ** Others:   None                                                         **
 **                                                                        **
 ***************************************************************************/
void eRAL_configure_thresholds_request(ral_enb_instance_t instanceP, MIH_C_Message_Link_Configure_Thresholds_request_t* msgP)
{
    unsigned int                             index;
    unsigned int                             th_index;
    rrc_ral_configure_threshold_req_t        configure_threshold_req;
    MessageDef                              *message_p;

    message_p = itti_alloc_new_message (TASK_RAL_ENB, RRC_RAL_CONFIGURE_THRESHOLD_REQ);

    memset(&configure_threshold_req, 0, sizeof(rrc_ral_configure_threshold_req_t));

    // copy transaction id
    configure_threshold_req.transaction_id      = msgP->header.transaction_id;

    // configure_threshold_req.num_link_cfg_params = 0; // done
    for (index = 0; index < msgP->primitive.LinkConfigureParameterList_list.length; index++) {
        // copy link_param_type
        configure_threshold_req.link_cfg_params[index].link_param_type.choice = msgP->primitive.LinkConfigureParameterList_list.val[index].link_param_type.choice;
        switch (configure_threshold_req.link_cfg_params[index].link_param_type.choice) {
            case  RAL_LINK_PARAM_TYPE_CHOICE_GEN:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_gen,
                        &msgP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_gen,
                        sizeof(ral_link_param_gen_t));
                break;
            case  RAL_LINK_PARAM_TYPE_CHOICE_QOS:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_qos,
                        &msgP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_qos,
                        sizeof(ral_link_param_qos_t));
                break;
            case  RAL_LINK_PARAM_TYPE_CHOICE_LTE:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_lte,
                        &msgP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_lte,
                        sizeof(ral_link_param_lte_t));
                break;
            default:
                assert(1==0);
        }
        configure_threshold_req.num_link_cfg_params += 1;

        // copy choice
        configure_threshold_req.link_cfg_params[index].union_choice = msgP->primitive.LinkConfigureParameterList_list.val[index].choice;

        // copy _union
        switch (configure_threshold_req.link_cfg_params[index].union_choice) {
            case RAL_LINK_CFG_PARAM_CHOICE_TIMER_NULL:
                configure_threshold_req.link_cfg_params[index]._union.null_attr = 0;
                break;
            case RAL_LINK_CFG_PARAM_CHOICE_TIMER:
                configure_threshold_req.link_cfg_params[index]._union.timer_interval = msgP->primitive.LinkConfigureParameterList_list.val[index]._union.timer_interval;
            default:
                assert(1==0);
        }

        // copy th_action
        configure_threshold_req.link_cfg_params[index].th_action = msgP->primitive.LinkConfigureParameterList_list.val[index].th_action;

        // configure_threshold_req.link_cfg_params[index].num_thresholds = 0; // done
        for (th_index = 0; th_index < msgP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.length;th_index++) {
            configure_threshold_req.link_cfg_params[index].thresholds[th_index].threshold_val  = msgP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.val[th_index].threshold_val;
            configure_threshold_req.link_cfg_params[index].thresholds[th_index].threshold_xdir = msgP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.val[th_index].threshold_xdir;
            configure_threshold_req.link_cfg_params[index].num_thresholds += 1;
        }
    }


    memcpy (&message_p->ittiMsg, (void *) &configure_threshold_req, sizeof(rrc_ral_configure_threshold_req_t));
    itti_send_msg_to_task (TASK_RRC_ENB, instanceP, message_p);
}

//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_configure_threshold_conf(instance_t instance, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_STATUS_T                      status;
    // This parameter is not included if Status does not indicate “Success.”
    MIH_C_LINK_CFG_STATUS_LIST_T        link_cfg_status_list;
    unsigned int                        i;

    status = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).status;
    if (status == RAL_STATUS_SUCCESS) {
        link_cfg_status_list.length = 0;
        for (i = 0; i < RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).num_link_cfg_params; i++) {
            link_cfg_status_list.val[i].link_param_type.choice = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].link_param_type.choice;
            switch (link_cfg_status_list.val[i].link_param_type.choice) {
                case  RAL_LINK_PARAM_TYPE_CHOICE_GEN:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_gen,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].link_param_type._union.link_param_gen,
                            sizeof(ral_link_param_gen_t));
                    break;
                case  RAL_LINK_PARAM_TYPE_CHOICE_QOS:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_qos,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].link_param_type._union.link_param_qos,
                            sizeof(ral_link_param_qos_t));
                    break;
                case  RAL_LINK_PARAM_TYPE_CHOICE_LTE:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_lte,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].link_param_type._union.link_param_lte,
                            sizeof(ral_link_param_lte_t));
                    break;
                default:
                    assert(1==0);
            }
            link_cfg_status_list.val[i].threshold.threshold_val  = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].threshold.threshold_val;
            link_cfg_status_list.val[i].threshold.threshold_xdir = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].threshold.threshold_xdir;
            link_cfg_status_list.val[i].config_status            = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).cfg_status[i].config_status;
            link_cfg_status_list.length += 1;
        }
        eRAL_send_configure_thresholds_confirm(instance, &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).transaction_id, &status, &link_cfg_status_list);
    } else {
        eRAL_send_configure_thresholds_confirm(instance, &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_p).transaction_id, &status, NULL);
    }
}
