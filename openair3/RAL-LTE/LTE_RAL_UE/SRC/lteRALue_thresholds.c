/***************************************************************************
                         ltmRALue_thresholds.c  -  description
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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file ltmRALue_thresholds.c
 * \brief
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_UE
#define LTERALUE_THRESHOLDS_C
//-----------------------------------------------------------------------------
#include "assertions.h"
#include "lteRALue.h"

extern unsigned char NB_eNB_INST;


/****************************************************************************
 **                                                                        **
 ** Name:  mRAL_configure_thresholds_request()                             **
 **                                                                        **
 ** Description: Forwards the Link_Configure_Thresholds.request message    **
 **   to the RRC layer.                                                    **
 **                                                                        **
 ** Inputs:  msg_pP:  Pointer to the received message                        **
 **     Others:                                                            **
 **                                                                        **
 ** Outputs:  None                                                         **
 ** Return:   None                                                         **
 ** Others:   None                                                         **
 **                                                                        **
 ***************************************************************************/
void mRAL_configure_thresholds_request(ral_ue_instance_t                                  instanceP,
                                       MIH_C_Message_Link_Configure_Thresholds_request_t* msg_pP)
{
    unsigned int                             index;
    unsigned int                             th_index;
    rrc_ral_configure_threshold_req_t        configure_threshold_req;
    MessageDef                              *message_p;
    MIH_C_STATUS_T                           status;
    MIH_C_STATUS_T                           global_status;
    int                                      result;
    MIH_C_LINK_CFG_STATUS_LIST_T             link_configure_status_list;

    MIH_C_LINK_CFG_STATUS_LIST_init(&link_configure_status_list);

    message_p = itti_alloc_new_message (TASK_RAL_UE, RRC_RAL_CONFIGURE_THRESHOLD_REQ);

    memset(&configure_threshold_req, 0, sizeof(rrc_ral_configure_threshold_req_t));

    // copy transaction id
    configure_threshold_req.transaction_id      = msg_pP->header.transaction_id;

    global_status = MIH_C_STATUS_SUCCESS;

    // configure_threshold_req.num_link_cfg_params = 0; // done
    for (index = 0; index < msg_pP->primitive.LinkConfigureParameterList_list.length; index++) {

        status = MIH_C_STATUS_SUCCESS;

        // copy link_param_type
        configure_threshold_req.link_cfg_params[index].link_param_type.choice = msg_pP->primitive.LinkConfigureParameterList_list.val[index].link_param_type.choice;
        switch (configure_threshold_req.link_cfg_params[index].link_param_type.choice) {
            case  RAL_LINK_PARAM_TYPE_CHOICE_GEN:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_gen,
                        &msg_pP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_gen,
                        sizeof(ral_link_param_gen_t));
                break;
            case  RAL_LINK_PARAM_TYPE_CHOICE_QOS:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_qos,
                        &msg_pP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_qos,
                        sizeof(ral_link_param_qos_t));
                break;
            case  RAL_LINK_PARAM_TYPE_CHOICE_LTE:
                memcpy(&configure_threshold_req.link_cfg_params[index].link_param_type._union.link_param_lte,
                        &msg_pP->primitive.LinkConfigureParameterList_list.val[index].link_param_type._union.link_param_lte,
                        sizeof(ral_link_param_lte_t));
                break;
            default:
                status        = MIH_C_STATUS_UNSPECIFIED_FAILURE;
                break;
        }
        // at first error, exit


        configure_threshold_req.num_link_cfg_params += 1;

        // copy choice
        configure_threshold_req.link_cfg_params[index].union_choice = msg_pP->primitive.LinkConfigureParameterList_list.val[index].choice;

        // copy _union
        switch (configure_threshold_req.link_cfg_params[index].union_choice) {
            case RAL_LINK_CFG_PARAM_CHOICE_TIMER_NULL:
                configure_threshold_req.link_cfg_params[index]._union.null_attr = 0;
                break;
            case RAL_LINK_CFG_PARAM_CHOICE_TIMER:
                configure_threshold_req.link_cfg_params[index]._union.timer_interval = msg_pP->primitive.LinkConfigureParameterList_list.val[index]._union.timer_interval;
                break;
            default:
                printf("ERROR RAL_UE, : mRAL_configure_thresholds_request unknown configure_threshold_req.link_cfg_params[index].union_choice %d\n",
                        configure_threshold_req.link_cfg_params[index].union_choice);
                status        = MIH_C_STATUS_UNSPECIFIED_FAILURE;
                break;
        }

        // copy th_action
        configure_threshold_req.link_cfg_params[index].th_action = msg_pP->primitive.LinkConfigureParameterList_list.val[index].th_action;

        // configure_threshold_req.link_cfg_params[index].num_thresholds = 0; // done
        for (th_index = 0; th_index < msg_pP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.length;th_index++) {
            configure_threshold_req.link_cfg_params[index].thresholds[th_index].threshold_val  = msg_pP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.val[th_index].threshold_val;
            configure_threshold_req.link_cfg_params[index].thresholds[th_index].threshold_xdir = msg_pP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.val[th_index].threshold_xdir;
            configure_threshold_req.link_cfg_params[index].num_thresholds += 1;

            // Fill ConfigureThreshold_confirm
            if (link_configure_status_list.length < MIH_C_LINK_CFG_STATUS_LIST_LENGTH) {
                memcpy(&link_configure_status_list.val[link_configure_status_list.length].link_param_type,
                        &configure_threshold_req.link_cfg_params[index].link_param_type,
                        sizeof(ral_link_param_type_t));

                memcpy(&link_configure_status_list.val[link_configure_status_list.length].threshold,
                        &msg_pP->primitive.LinkConfigureParameterList_list.val[index].threshold_list.val[th_index],
                        sizeof(ral_link_param_type_t));

                link_configure_status_list.val[link_configure_status_list.length].config_status = status;

                link_configure_status_list.length += 1;
            } else {
                LOG_E(RAL_UE, "MIH_C_LINK_CFG_STATUS_LIST overflow for send_configure_thresholds_confirm\n");
                global_status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
            }
        }
    }

    if (link_configure_status_list.length > 0) {
        memcpy (&message_p->ittiMsg, (void *) &configure_threshold_req, sizeof(rrc_ral_configure_threshold_req_t));
        itti_send_msg_to_task (TASK_RRC_UE, instanceP, message_p);

        mRAL_send_configure_thresholds_confirm(instanceP,
                &msg_pP->header.transaction_id,
                &global_status,
                &link_configure_status_list);
    } else {
        mRAL_send_configure_thresholds_confirm(instanceP,
                &msg_pP->header.transaction_id,
                &global_status,
                NULL);

        result = itti_free (ITTI_MSG_ORIGIN_ID(message_p), message_p);
        AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_configure_threshold_conf(ral_ue_instance_t instance, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_STATUS_T                      status;
    // This parameter is not included if Status does not indicate “Success.”
    MIH_C_LINK_CFG_STATUS_LIST_T        link_cfg_status_list;
    unsigned int                        i;

    status = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).status;
    if (status == RAL_STATUS_SUCCESS) {
        link_cfg_status_list.length = 0;
        for (i = 0; i < RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).num_link_cfg_params; i++) {
            link_cfg_status_list.val[i].link_param_type.choice = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].link_param_type.choice;
            switch (link_cfg_status_list.val[i].link_param_type.choice) {
                case  RAL_LINK_PARAM_TYPE_CHOICE_GEN:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_gen,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].link_param_type._union.link_param_gen,
                            sizeof(ral_link_param_gen_t));
                    break;
                case  RAL_LINK_PARAM_TYPE_CHOICE_QOS:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_qos,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].link_param_type._union.link_param_qos,
                            sizeof(ral_link_param_qos_t));
                    break;
                case  RAL_LINK_PARAM_TYPE_CHOICE_LTE:
                    memcpy(&link_cfg_status_list.val[i].link_param_type._union.link_param_lte,
                            &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].link_param_type._union.link_param_lte,
                            sizeof(ral_link_param_lte_t));
                    break;
                default:
                    assert(1==0);
            }
            link_cfg_status_list.val[i].threshold.threshold_val  = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].threshold.threshold_val;
            link_cfg_status_list.val[i].threshold.threshold_xdir = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].threshold.threshold_xdir;
            link_cfg_status_list.val[i].config_status            = RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).cfg_status[i].config_status;
            link_cfg_status_list.length += 1;
        }
        mRAL_send_configure_thresholds_confirm(instance, &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).transaction_id, &status, &link_cfg_status_list);
    } else {
        mRAL_send_configure_thresholds_confirm(instance, &RRC_RAL_CONFIGURE_THRESHOLD_CONF(msg_pP).transaction_id, &status, NULL);
    }
}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_measurement_report_indication(ral_ue_instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_TRANSACTION_ID_T           transaction_id;
    module_id_t                      mod_id;
    MIH_C_LINK_TUPLE_ID_T            link_tuple_id;
    LIST(MIH_C_LINK_PARAM_RPT,  link_parameters_report);

    mod_id                               = instanceP - NB_eNB_INST;
    transaction_id                       = g_ue_ral_obj[mod_id].transaction_id;
    g_ue_ral_obj[mod_id].transaction_id += 1;

    memset(&link_tuple_id, 0, sizeof(MIH_C_LINK_TUPLE_ID_T));
    link_tuple_id.link_id.link_type     = MIH_C_WIRELESS_LTE;
#ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
#else
    link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;

    // preserve byte order of plmn id
    memcpy(link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_ue_ral_obj[mod_id].plmn_id, 3);
    link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_ue_ral_obj[mod_id].cell_id;

    LOG_D(RAL_UE, "PLMN ID %d.%d.%d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);
    LOG_D(RAL_UE, "CELL ID %d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
#endif

    MIH_C_LINK_PARAM_RPT_LIST_init(&link_parameters_report_list);
    memcpy(&link_parameters_report_list.val[0].link_param,
            &RRC_RAL_MEASUREMENT_REPORT_IND(msg_pP).link_param,
            sizeof(MIH_C_LINK_PARAM_T));

    if (RRC_RAL_MEASUREMENT_REPORT_IND(msg_pP).threshold.threshold_xdir == RAL_NO_THRESHOLD) {
        link_parameters_report_list.val[0].choice = MIH_C_LINK_PARAM_RPT_CHOICE_NULL;
    } else {
        link_parameters_report_list.val[0].choice = MIH_C_LINK_PARAM_RPT_CHOICE_THRESHOLD;
        link_parameters_report_list.val[0]._union.threshold.threshold_val  = RRC_RAL_MEASUREMENT_REPORT_IND(msg_pP).threshold.threshold_val;
        link_parameters_report_list.val[0]._union.threshold.threshold_xdir = RRC_RAL_MEASUREMENT_REPORT_IND(msg_pP).threshold.threshold_xdir;
    }
    link_parameters_report_list.length += 1;

    mRAL_send_link_parameters_report_indication(instanceP,
                                               &transaction_id,
                                               &link_tuple_id,
                                               &link_parameters_report_list);
}
/*
 * typedef struct MIH_C_LINK_PARAM {
    MIH_C_LINK_PARAM_TYPE_T      link_param_type;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_LINK_PARAM_VAL_T   link_param_val;
        MIH_C_QOS_PARAM_VAL_T    qos_param_val;
    } _union;
} MIH_C_LINK_PARAM_T;

 * typedef struct MIH_C_LINK_PARAM_RPT {
    MIH_C_LINK_PARAM_T           link_param;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_THRESHOLD_T        threshold;
    } _union;
} MIH_C_LINK_PARAM_RPT_T;
#define MIH_C_LINK_PARAM_RPT_CHOICE_NULL      0
#define MIH_C_LINK_PARAM_RPT_CHOICE_THRESHOLD 1
 *
 */

