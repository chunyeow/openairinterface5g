/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRAL_action.c
 *
 * Version 0.1
 *
 * Date  07/03/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem MIH link action request messages processing
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description 
 *
 *****************************************************************************/
#define LTE_RAL_ENB
#define LTE_RAL_ENB_ACTION_C

#include <assert.h>
#include "lteRALenb.h"


/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

static MIH_C_LINK_ACTION_T g_link_action;

static const char* g_link_action_type_str[] = {
        "MIH_C_LINK_AC_TYPE_NONE",
        "MIH_C_LINK_AC_TYPE_LINK_DISCONNECT",
        "MIH_C_LINK_AC_TYPE_LINK_LOW_POWER",
        "MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN",
        "MIH_C_LINK_AC_TYPE_LINK_POWER_UP",
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
        "MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR",
        "MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES",
        "MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES"
#endif
};


/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_request()                                  **
 **                                                                        **
 ** Description: Processes the Link_Action.request message and sends a     **
 **   Link_Action.confirm message to the MIHF.                  **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received message            **
 **     Others: ralpriv                                    **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_link_action                              **
 **                                                                        **
 ***************************************************************************/
void eRAL_action_request(ral_enb_instance_t instanceP, MIH_C_Message_Link_Action_request_t* msgP)
{
    MIH_C_STATUS_T         status;
    MIH_C_LINK_AC_TYPE_T   link_action_type;
    MIH_C_LINK_AC_RESULT_T link_action_result;
    int link_action_done = 0;
    
    
    rrc_ral_connection_reconfiguration_req_t        connection_reconfiguration_req_t;
    MessageDef                              *message_p = NULL;

    message_p = itti_alloc_new_message (TASK_RAL_ENB, RRC_RAL_CONNECTION_RECONFIGURATION_REQ);
    memset(&connection_reconfiguration_req_t, 0, sizeof(rrc_ral_connection_reconfiguration_req_t));
     // copy transaction id
//     connection_reconfiguration_req_t.transaction_id      = msgP->header.transaction_id;
    
    
    memcpy(&g_link_action, &msgP->primitive.LinkAction, sizeof(MIH_C_LINK_ACTION_T));

    status                        = MIH_C_STATUS_SUCCESS;
    link_action_type              = msgP->primitive.LinkAction.link_ac_type;
    link_action_result            = MIH_C_LINK_AC_RESULT_SUCCESS;    

    /*
     * Read link action attributs
     * --------------------------
     */
//     if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN)
//     {
//         /*
//          * Link scan operation request - Not supported by the network side:
//          * No measurements
//          */
//         LOG_D(RAL_ENB, " ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN: REFUSED\n");
//         link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
//         eRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id,
//                 &status,
//                 NULL,
//                 &link_action_result);
//     }

//     if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN)
//     {
//         /*
//          * Link resource retain operation request - Not supported by the
//          * network side.
//          */
//         LOG_D(RAL_ENB, " ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN: REFUSED\n");
//         link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
//         eRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id,
//                 &status,
//                 NULL,
//                 &link_action_result);
//     }

//     if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ)
//     {
//         /*
//          * Data forward operation request - Not supported by the network side.
//          */
//         LOG_D(RAL_ENB, " ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ: REFUSED\n");
//         link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
//         eRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id,
//                 &status,
//                 NULL,
//                 &link_action_result);
//     }

    /*
     * Read link action request
     * ------------------------
     * Requested actions that are not supported or currently in progress
     * are refused.
     */
    if (eRAL_action_is_in_progress(instanceP, &status,
            &link_action_result,
            link_action_type))
    {
        LOG_D(RAL_ENB, " Link action request %s is not supported or currently in progress\n", g_link_action_type_str[link_action_type]);
    }
    else
    {
        LOG_D(RAL_ENB, " %s ACTION REQUESTED: %s\n", __FUNCTION__, g_link_action_type_str[link_action_type]);

        switch (link_action_type)
        {
            case MIH_C_LINK_AC_TYPE_NONE:
                LOG_D(RAL_ENB, " %s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_NONE: NO ACTION\n", __FUNCTION__);
                break;

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
            case MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR:
                /*
                 * Provide the mark and multicast configuration to be set up
                 * for the data flow
                 */
                g_enb_ral_obj[instanceP].pending_req_ac_result = eRAL_action_link_flow_attr(instanceP);
                link_action_done = 1;
                break;

            case MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES:
                /*
                 * Configure and activate a data flow
                 */
                g_enb_ral_obj[instanceP].pending_req_ac_result = eRAL_action_link_activate_resources(instanceP);
                link_action_done = 1;
                break;

            case MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES:
                /*
                 * Deactivate a previously activated data flow
                 */
                g_enb_ral_obj[instanceP].pending_req_ac_result = eRAL_action_link_deactivate_resources(instanceP);
                link_action_done = 1;
                break;
#endif // MIH_C_MEDIEVAL_EXTENSIONS

            case MIH_C_LINK_AC_TYPE_LINK_DISCONNECT:
                /*
                 * Disconnect the link connection directly
                 */
            case MIH_C_LINK_AC_TYPE_LINK_LOW_POWER:
                /*
                 * Cause the link to adjust its battery power level to be
                 * low power consumption - Not supported by the network side
                 */
            case MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN:
                /*
                 * Cause the link to power down and turn off the radio
                 * - Not supported by the network side
                 */
            case MIH_C_LINK_AC_TYPE_LINK_POWER_UP:
              LOG_D(RAL_ENB, "%s ACTION REQUESTED: MIH_C_LINK_AC_TYPE_LINK_POWER_UP\n", __FUNCTION__);
                if (g_enb_ral_obj[instanceP].mih_supported_link_action_list  & (1 << MIH_C_LINK_AC_TYPE_LINK_POWER_UP)) {
                    // Activation requested - check it is not already active
                    if(g_enb_ral_obj[instanceP].pending_req_action & MIH_C_LINK_AC_TYPE_LINK_POWER_UP) {
//                         if (g_enb_ral_obj[instanceP].state == CONNECTED) {
//                             LOG_D(RAL_ENB, "Activation requested, but interface already active ==> NO OP\n");
//                             mRAL_send_link_action_confirm(instanceP, &messageP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
//                         } else {
                            g_enb_ral_obj[instanceP].pending_req_action = g_enb_ral_obj[instanceP].pending_req_action | MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
//                             g_enb_ral_obj[instanceP].cell_id = g_enb_ral_obj[instanceP].meas_cell_id[0];  // Default cell #0 - Next, choose cell with best conditions
                            LOG_D(RAL_ENB, "Activation requested to NAS interface on cell %d\n", g_enb_ral_obj[instanceP].cell_id);
// RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_ADD, g_enb_ral_obj[instanceP].cell_id);
//                         }
                    } else {
                        g_enb_ral_obj[instanceP].pending_req_action |= MIH_C_LINK_AC_TYPE_LINK_POWER_UP;
                    /*    g_enb_ral_obj[instanceP].cell_id = g_enb_ral_obj[instanceP].meas_cell_id[0];*/ // Default cell #0 - Next, choose cell with best conditions
                        message_p = itti_alloc_new_message (TASK_RAL_ENB, RRC_RAL_CONNECTION_RECONFIGURATION_REQ);
                        memset(&connection_reconfiguration_req_t, 0, sizeof(rrc_ral_connection_reconfiguration_req_t));
//                         copy transaction id
                        connection_reconfiguration_req_t.transaction_id  = msgP->header.transaction_id;
                        
                        connection_reconfiguration_req_t.link_action.link_ac_type = msgP->primitive.LinkAction.link_ac_type;
                        connection_reconfiguration_req_t.link_action.link_ac_attr = msgP->primitive.LinkAction.link_ac_attr;
                        connection_reconfiguration_req_t.link_action.link_ac_param = msgP->primitive.LinkAction.link_ac_param;
                        
                        memcpy (&message_p->ittiMsg, (void *) &connection_reconfiguration_req_t, sizeof(rrc_ral_connection_reconfiguration_req_t));
                        itti_send_msg_to_task (TASK_RRC_ENB, instanceP, message_p);
                        
         /*               LOG_D(RAL_ENB, "Total data volume0 %d\n", (g_enb_ral_obj[instanceP].totalDataVolume[0]/1000));
                        LOG_D(RAL_ENB, "Total data volume1 %d\n", (g_enb_ral_obj[instanceP].totalDataVolume[1]/1000));
                        LOG_D(RAL_ENB, "RLC Buffer Occupancy 0  %d\n", (g_enb_ral_obj[instanceP].rlcBufferOccupancy[0]));
                        LOG_D(RAL_ENB, "RLC Buffer Occupancy 1 %d\n", (g_enb_ral_obj[instanceP].rlcBufferOccupancy[1]));
                 */       
                    }
                } else {
                    LOG_D(RAL_ENB, "[mRAL]: command POWER UP not available \n\n");
//                     link_action_result = MIH_C_LINK_AC_RESULT_INCAPABLE;
//                     mRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id, &status, &scan_response_set_list, &link_action_result);
                }
                break;

            default:
                LOG_E(RAL_ENB, "%s Invalid LinkAction.link_ac_type %d\n",
                        __FUNCTION__, link_action_type);
                status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
        }
    }

    /* Return link action confirmation to the MIH-F */
    if (link_action_done) {
        g_enb_ral_obj[instanceP].pending_req_transaction_id = msgP->header.transaction_id;
        g_enb_ral_obj[instanceP].pending_req_status = MIH_C_STATUS_SUCCESS;
        eRAL_send_link_action_confirm(instanceP, &g_enb_ral_obj[instanceP].pending_req_transaction_id,
                &g_enb_ral_obj[instanceP].pending_req_status,
                NULL,
                &g_enb_ral_obj[instanceP].pending_req_ac_result);
    }
    else if (status == MIH_C_STATUS_SUCCESS) {
        eRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                NULL,
                &link_action_result);
    }
    else {
        eRAL_send_link_action_confirm(instanceP, &msgP->header.transaction_id,
                &status,
                NULL,
                NULL);
    }

    g_enb_ral_obj[instanceP].pending_req_action = 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_save_flow_id()                             **
 **                                                                        **
 ** Description: Save connection identifier and data of the specified data **
 **   flow into the list of active data flows.                  **
 **                                                                        **
 ** Inputs:  flowId: The data flow identifier                   **
 **     cnxid:  The connection identifier                  **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: The index of the specified data flow in    **
 **    the list of active data flows.             **
 **    -1 if the list is full.                    **
 **   Others: None                                       **
 **                                                                        **
 ***************************************************************************/
int eRAL_action_save_flow_id(ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int cnxid){
    return eRAL_action_set_channel_id(instanceP, flowId, cnxid);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 ** Name:  eRAL_action_set_channel_id()                           **
 **                                                                        **
 ** Description: Set the Connection identifier and store data of the       **
 **   specified data flow in the list of active data flows.     **
 ** Inputs:  flowId: The data flow identifier                   **
 **     cnxid:  The connection identifier                  **
 ** Outputs:  None                                                      **
 **   Return: The index of the specified data flow in    **
 **    the list of active data flows.             **
 **    -1 if the list is full.                    **
 ***************************************************************************/
int eRAL_action_set_channel_id (ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int cnxid){
    char addr[128];
    //char port[8];
    int f_ix;

    assert(cnxid != 0);

    for (f_ix = 0; f_ix < ACTION_MAX_FLOW; f_ix++) {
        if (g_flows.flow[f_ix].cnxid > 0) continue;
        g_flows.flow[f_ix].cnxid = cnxid;
        g_flows.flow_id[f_ix] = (int)flowId;
        g_flows.n_flows += 1;

        // Modified Michelle
#warning "TO DO DestIpv6Addr ?"
        //memcpy((char*)&(DestIpv6Addr[0][16]), addr, 16);
        eRAL_process_mt_addr_to_l2id(&g_flows.flow[f_ix].addr[8],
                &g_flows.flow[f_ix].l2id[0]);

        /* MIH_C_TRANSPORT_ADDR_VALUE2String(&flowId->dest_addr.ip_addr.address, addr);
 memcpy((char*)&(g_flows.flow[f_ix].addr), addr, 16);
 eRAL_process_mt_addr_to_l2id(&g_flows.flow[f_ix].addr[8],
     &g_flows.flow[f_ix].l2id[0]);
 MIH_C_PORT2String(&flowId->dest_addr.port, port);
 g_flows.flow[f_ix].port = strtol(port, (char**) NULL, 16);
 g_flows.flow[f_ix].proto = flowId->transport_protocol;*/
        return f_ix;
    }
    return (-1);
}

/****************************************************************************
 ** Name:  eRAL_action_get_channel_id()                           **
 **                                                                        **
 ** Description: Returns the Connection identifier of the specified data   **
 **   flow.                                                     **
 ** Inputs:  flowId: The data flow identifier                   **
 **     Others: None                                       **
 ** Outputs:  cnxid:  The connection identifier allocated to the **
 **    specified data flow.                       **
 **   Return: The index of the specified data flow in    **
 **    the list of active data flows.             **
 **    -1 if no any connection identifier exists  **
 **    for the specified data flow.               **
 **   Others: None                                       **
 ***************************************************************************/
int eRAL_action_get_channel_id (ral_enb_instance_t instanceP, MIH_C_FLOW_ID_T* flowId, int* cnxid){
    //char addr[128];
    //char port[8];
    //unsigned int dp;
    int f_ix;

    //     MIH_C_TRANSPORT_ADDR_VALUE2String(&flowId->dest_addr.ip_addr.address, addr);
    //     MIH_C_PORT2String(&flowId->dest_addr.port, port);
    //     dp = strtol(port, (char**) NULL, 16);

    for (f_ix = 0; f_ix < ACTION_MAX_FLOW; f_ix++) {
        /* if (!eRAL_process_cmp_mt_addr((const char*)addr, (const char*)g_flows.flow[f_ix].l2id)) continue;
 if (g_flows.flow[f_ix].port != dp) continue;
 if (g_flows.flow[f_ix].proto != flowId->transport_protocol) continue;*/
        if (g_flows.flow_id[f_ix] != *flowId) continue;
        *cnxid = g_flows.flow[f_ix].cnxid;
        return f_ix;
    }
    return (-1);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_del_channel_id()                           **
 **                                                                        **
 ** Description: Remove the data flow stored at the specified index in the **
 **   list of active data flows.                                **
 **                                                                        **
 ** Inputs:  fix:  Index of the data flow in the list         **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: The number of the remaining data flows in  **
 **    the list.                                  **
 **    -1 if the specified index is out of the    **
 **    boundary of the list.                      **
 **   Others: None                                       **
 **                                                                        **
 ***************************************************************************/
int eRAL_action_del_channel_id (ral_enb_instance_t instanceP, int fix)
{
    if (fix < ACTION_MAX_FLOW) {
        g_flows.n_flows -= 1;
        memset (&g_flows.flow[fix], 0, sizeof (struct Data_flow));
        return g_flows.n_flows;
    }
    return (-1);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_is_in_progress()                          **
 **                                                                        **
 ** Description: Checks if an action request is supported and whether it   **
 **   is currently in progress.                                 **
 **                                                                        **
 ** Inputs:  action: Type of link action                        **
 **     Others: g_link_action, ralpriv                     **
 **                                                                        **
 ** Outputs:  status: MIH request status                         **
 **   ac_status: Action request status                      **
 **   Return: 1 if the request is not supported or is    **
 **    currently in progress and the resource is  **
 **    already in the required state. 0 otherwise.**
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
int eRAL_action_is_in_progress(ral_enb_instance_t instanceP, MIH_C_STATUS_T* status,
        MIH_C_LINK_AC_RESULT_T* ac_status,
        MIH_C_LINK_AC_TYPE_T action)
{
    /* Check whether the action link command is supported */
    if (!(g_enb_ral_obj[instanceP].mih_supported_link_command_list & MIH_C_BIT_LINK_ACTION)) {
        *status = MIH_C_STATUS_REJECTED;
         LOG_D(RAL_ENB,"THE LINK ACTION is not in the list\n");
        return 1;
    }
    *status = MIH_C_STATUS_SUCCESS;

    /* Check whether the action request is supported */
    if (g_enb_ral_obj[instanceP].mih_supported_link_action_list & (1 << action))
    {
        /* Check whether another action request is currently in progress */
        if ((g_enb_ral_obj[instanceP].pending_req_action) && (g_enb_ral_obj[instanceP].pending_req_action != action))
        {
            /* Another action request is in progress:
             * Do not process new request before completion of this one */
         LOG_D(RAL_ENB,"THE LINK ACTION is pending\n");
            *ac_status = MIH_C_LINK_AC_RESULT_REFUSED;
            return 1;
        }

        /* The action request is supported and no other request is in progress:
         * Go ahead and process the request */
        
         LOG_D(RAL_ENB,"THE LINK ACTION is not pending\n");
        return 0;
    }

         LOG_D(RAL_ENB,"THE LINK ACTION is not supported\n");
    /* The link action request is not supported */
    *ac_status = MIH_C_LINK_AC_RESULT_INCAPABLE;
    return 1;
}

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_link_flow_attr()                          **
 **                                                                        **
 ** Description: Processes the link flow attribut action request.          **
 **                                                                        **
 ** Inputs:  None                                                      **
 **     Others: g_link_action                              **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **    been successfully processed.               **
 **     Others: ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
MIH_C_LINK_AC_RESULT_T eRAL_action_link_flow_attr(ral_enb_instance_t instanceP)
//TODO
{
    MIH_C_FLOW_ATTRIBUTE_T *flow = &g_link_action.link_ac_param._union.flow_attribute;
    int mt_ix, ch_ix, f_ix;
    int cnxid;

    /* Get the connection identifier */
    f_ix = eRAL_action_get_channel_id(instanceP, &flow->flow_id, &cnxid);
    if (f_ix < 0) {
        LOG_D(RAL_ENB, " No RB allocated for this data flow\n");
        return MIH_C_LINK_AC_RESULT_REFUSED;
    }

    /* Get MT and RB channel identifiers */
    if (eRAL_process_find_channel(instanceP, cnxid, &mt_ix, &ch_ix) != 0) {
        /* Unicast data flow */
        LOG_D(RAL_ENB, " %s: Unicast MT's address = %s\n", __FUNCTION__,
                eRAL_process_mt_addr_to_string(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_addr));
    }
    else {
        /* Multicast data flow */
        LOG_D(RAL_ENB, " %s: Multicast MT's address = %s\n", __FUNCTION__,
                eRAL_process_mt_addr_to_string(g_enb_ral_obj[instanceP].mcast.mc_group_addr));
    }

    if (flow->choice_mark_qos) {
        LOG_D(RAL_ENB, " Mark QoS enabled\n");
        //TODO ???
    }

    if (flow->choice_mark_drop_eligibility) {
        LOG_D(RAL_ENB, " Mark drop eligibility enabled\n");
        //TODO ???
    }

    /* Link action successfully processed */
    g_enb_ral_obj[instanceP].pending_req_action = MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_link_activate_resources()                 **
 **                                                                        **
 ** Description: Processes the link activate resource action request.      **
 **                                                                        **
 ** Inputs:  None                                                      **
 **     Others: g_link_action                              **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **    been successfully processed.               **
 **     Others: ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
MIH_C_LINK_AC_RESULT_T eRAL_action_link_activate_resources(ral_enb_instance_t instanceP){
    //---------------------------------------------------------------------------
    MIH_C_RESOURCE_DESC_T *res = &g_link_action.link_ac_param._union.resource_desc;
    MIH_C_COS_T            classIdDL = 0;
    MIH_C_COS_T            classIdUL = 0;
    MIH_C_LINK_DATA_RATE_T resBitrateDL = 0;
    MIH_C_LINK_DATA_RATE_T resBitrateUL = 0;
    MIH_C_BOOLEAN_T        multicast = MIH_C_BOOLEAN_FALSE;

    /* TODO: To be initialized downlink/uplink */
    if (res->choice_qos) {
        MIH_C_QOS_T *qos = &(res->_union_qos.qos);
        if (qos->choice) {
            classIdDL = qos->_union.cos;
            classIdUL = qos->_union.cos;
        }
    }
    if (res->choice_link_data_rate) {
        resBitrateDL = res->_union_link_data_rate.link_data_rate;
        resBitrateUL = res->_union_link_data_rate.link_data_rate;
    }
    /* end TODO */

    if (res->choice_multicast_enable) {
        multicast = res->_union_multicast_enable.multicast_enable;
    }

    /*
     * Get the Care-of Address (MT address):
     * The destination address is contained into the MIH_C_FLOW_ID structure
     * within the MIH_C_RESOURCE_DESC parameters of the request.
     */
    char mt_addr[128];
    //original code: int len = MIH_C_TRANSPORT_ADDR_VALUE2String(&res->flow_id.dest_addr.ip_addr.address, mt_addr);

    MIH_C_TRANSPORT_ADDR_VALUE_T mihc_transport_addr_value;
    MIH_C_TRANSPORT_ADDR_VALUE_set(&mihc_transport_addr_value, (unsigned char*)"2001:660:382:14:335:600:8014:9150", strlen("2001:660:382:14:335:600:8014:9150"));

    int len = MIH_C_TRANSPORT_ADDR_VALUE2String(&mihc_transport_addr_value, mt_addr);
    //int len = MIH_C_TRANSPORT_ADDR_VALUE2String(&DestIpv6Addr[0][16], mt_addr);

    if ( (len > 0) && (len < 128)) {
        LOG_D(RAL_ENB, " %s: MT's address = %s\n", __FUNCTION__, eRAL_process_mt_addr_to_string((unsigned char*)mt_addr));
    }
    else {
        LOG_E(RAL_ENB, "%s : IP Address is NOT valid (len=%d)\n", __FUNCTION__, len);
        return MIH_C_LINK_AC_RESULT_FAILURE;
    }

#if 0
    char mt_addr[128];
    if (res->link_id.link_addr.choice == MIH_C_CHOICE_3GPP_3G_CELL_ID) {
        char link_addr[128];
        int len = MIH_C_LINK_ADDR2String(&res->link_id.link_addr, link_addr);
        if ( (len > 0) && (len <= 128) ) {
            int i;
            LOG_D(RAL_ENB, " %s : Link address = %s\n", __FUNCTION__, link_addr);
            for (i=0; i < len; i++) {
                if (link_addr[i] == '=') break;
            }
            strncpy(mt_addr, &link_addr[i+2], 16);
            LOG_D(RAL_ENB, " %s : MT address = %s\n", __FUNCTION__, mt_addr);
        } else {
            LOG_E(RAL_ENB, "%s : Link address is NOT valid (len=%d)\n", __FUNCTION__, len);
            return MIH_C_LINK_AC_RESULT_FAILURE;
        }
    }
    else {
        LOG_E(RAL_ENB, "%s : Link address is NOT valid (type=%d, not 3GPP_3G_CELL_ID)\n",
                __FUNCTION__, res->link_id.link_addr.choice);
        return MIH_C_LINK_AC_RESULT_FAILURE;
    }
#endif

    /*
     * If the resource has already been activated, check whether it is in the
     * required state.
     */
    struct ral_lte_channel *currChannel;
    int mt_ix, ch_ix, f_ix;
    int cnxid;

    /* Get the connection identifier */
    f_ix = eRAL_action_get_channel_id(instanceP, &res->flow_id, &cnxid);
    if (f_ix != -1) {
        /* Get MT and RB channel identifiers */
        if (eRAL_process_find_channel(instanceP, cnxid, &mt_ix, &ch_ix) != 0) {
            /* Unicast data flow */
            currChannel = &(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]);
        } else {
            /* Multicast data flow */
            assert(multicast == MIH_C_BOOLEAN_TRUE);
            currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
        }

        if (currChannel->status == RB_CONNECTED) {
            //if (currChannel->status == NAS_CONNECTED) {
            /* The resource is already in the required state */
            LOG_D(RAL_ENB, " Link action ACTIVATE_RESOURCES requested while link resource is ALREADY activated\n");
            return MIH_C_LINK_AC_RESULT_SUCCESS;
        }

        /* The resource is not in the required state:
         * Remove the connection identifier from the list of active resources
         * and process the action request again. */
        LOG_D(RAL_ENB, " Resource has been activated but is not in the ACTIVE state\n");
        (void) eRAL_action_del_channel_id(instanceP, f_ix);
    }

    MIH_C_LINK_TUPLE_ID_T *ltid;
    int is_ready_for_rb_establish = 1;

    if (multicast) {
        /*
         * Multicast data flow:
         */
        mt_ix = RAL_MAX_MT;
        ch_ix = 0;
        currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
        currChannel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+1;
        currChannel->rbId = RAL_DEFAULT_RAB_ID;
        currChannel->multicast = 1;
        memcpy((char*)&(g_enb_ral_obj[instanceP].mcast.mc_group_addr), mt_addr, 16);
        ltid = &(g_enb_ral_obj[instanceP].mcast.ltid);
    }
    else {
        /*
         * Unicast data flow
         */
        /* Get the list of MTs waiting for RB establishment */
#ifdef RAL_REALTIME
        // LG RAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_LIST, 0, 0);
        // LG RAL_process_NAS_message(IO_OBJ_RB, IO_CMD_LIST, 0, 0);
#endif
        /* Check if the MT is in the list */
        mt_ix = eRAL_process_find_mt_by_addr(instanceP, mt_addr);

        if ( (mt_ix < RAL_MAX_MT) &&
                (g_enb_ral_obj[instanceP].mt[mt_ix].mt_state == RB_CONNECTED)) {
            /* The MT is ready for RB establishment */
            ch_ix = eRAL_process_find_new_channel(instanceP, mt_ix);
            if (ch_ix == RAL_MAX_RB) {
                LOG_D(RAL_ENB, " No RB available in MT\n");
                return MIH_C_LINK_AC_RESULT_REFUSED;
            }
            currChannel = &(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]);
            currChannel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
            currChannel->rbId = RAL_DEFAULT_RAB_ID + ch_ix;
            currChannel->multicast = 0;
            LOG_D(RAL_ENB, " mt_ix %d, ch_ix %d, cnx_id %d, rbId %d\n",
                    mt_ix, ch_ix, currChannel->cnx_id, currChannel->rbId);
            memcpy((char *)&(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_addr), mt_addr, 16);
            ltid = &(g_enb_ral_obj[instanceP].mt[mt_ix].ltid);
        }
        else {
            /* The MT is NOT ready for RB establishment */
            is_ready_for_rb_establish = 0;
            int qos_is_valid = 0;

            LOG_D(RAL_ENB, " Mobile Terminal not ready - Storing request data\n");
            /* Check validity of QoS parameters */
            if ( (classIdDL < 64) && (classIdUL < 64) ) {
                if ( (resBitrateDL <= RAL_BITRATE_320k)
                        && (resBitrateUL <= RAL_BITRATE_320k) ) {
                    qos_is_valid = 1;
                }
                else if ( (resBitrateDL >= RAL_BITRATE_384k)
                        && (resBitrateDL <= RAL_BITRATE_440k)
                        && (resBitrateUL <= RAL_BITRATE_64k) ) {
                    qos_is_valid = 1;
                }
            }
            if (qos_is_valid) {
                LOG_D(RAL_ENB, " Received QoS parameters are valid\n");
            } else {
                LOG_D(RAL_ENB, " Received QoS parameters are NOT valid - Request will be rejected\n");
                return MIH_C_LINK_AC_RESULT_REFUSED;
            }

            currChannel = &(g_enb_ral_obj[instanceP].pending_mt.radio_channel[0]);
            currChannel->cnx_id = 0;
            currChannel->rbId = 0;
            currChannel->multicast = 0;
            memcpy((char *)&(g_enb_ral_obj[instanceP].pending_mt.ipv6_addr), mt_addr, 16);

            ltid = &(g_enb_ral_obj[instanceP].pending_mt.ltid);
            g_enb_ral_obj[instanceP].pending_req_flag = 1;

        } /* end MT not ready */

    } /* end unicast data flow */

    /* Save the current data flow identifier into the list of active data flows */
    if (currChannel->cnx_id != 0) {
        f_ix = eRAL_action_set_channel_id(instanceP, &res->flow_id, currChannel->cnx_id);
        if (f_ix < 0) {
            LOG_D(RAL_ENB, " No RB available\n");
            return MIH_C_LINK_AC_RESULT_REFUSED;
        }
    }
    else {
        /* The current request is pending waiting for RB establishment */
        f_ix = 0;
        g_enb_ral_obj[instanceP].pending_req_fid = res->flow_id;
    }
    /*
     * Store resource parameters
     */
    int i;
    for (i = 0; i < 2; i++) {
        //TODO: int dir = p->qos.value[i].direction;
        // TODO: To be initialized downlink/uplink
        currChannel->flowId[i] = f_ix;
        currChannel->classId[i] = classIdDL; // classIdUL
        currChannel->resBitrate[i] = resBitrateDL; // resBitrateUL
        //currChannel->meanBitrate[i] = p->qos.value[i].tspec.meanBitrate;
        //currChannel->bktDepth[i] = p->qos.value[i].tspec.bucketDepth;
        //currChannel->pkBitrate[i] = p->qos.value[i].tspec.peakBitrate;
        //currChannel->MTU[i] = p->qos.value[i].tspec.maximumTransmissionUnit;
        LOG_D(RAL_ENB, " qos value : DIR %d, flowId %d, classId %d, resBitrate %.1f\n", i , currChannel->flowId[i], currChannel->classId[i], currChannel->resBitrate[i]);
    }
    /* Store the link identifier */
    ltid->link_id = res->link_id;
    ltid->choice = MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR;
    ltid->_union.link_addr.choice = MIH_C_CHOICE_3GPP_3G_CELL_ID; // DUMMY
    ltid->_union.link_addr._union._3gpp_3g_cell_id.cell_id = RAL_DEFAULT_CELL_ID; // DUMMY

    /*
     * Setup Radio Bearer resources
     */
    if (is_ready_for_rb_establish) {
        /* Map QoS */
        int mapping_result = eRAL_process_map_qos(instanceP, mt_ix, ch_ix);
        if (mapping_result) {
            int rc;
#ifdef RAL_DUMMY
            rc = eRAL_NAS_send_rb_establish_request(mt_ix, ch_ix);
#endif
#ifdef RAL_REALTIME
            // LG rc = RAL_process_NAS_message(IO_OBJ_RB, IO_CMD_ADD, mt_ix, ch_ix);
#endif
            if (rc < 0) {
                /* Failed to send RB establishment request */
                return MIH_C_LINK_AC_RESULT_FAILURE;
            }
        } else {
            /* QoS mapping is not supported */
            return MIH_C_LINK_AC_RESULT_REFUSED;
        }
    }
    else {
        /* Wait for MT coming ready;
         * re-try to establish RB upon timer expiration */
#ifdef RAL_DUMMY
        g_enb_ral_obj[instanceP].pending_mt_timer = 5;
#endif
#ifdef RAL_REALTIME
        g_enb_ral_obj[instanceP].pending_mt_timer = 300;
#endif
    }

    /* Link action successfully processed */
    g_enb_ral_obj[instanceP].pending_req_action = MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_action_link_deactivate_resources()               **
 **                                                                        **
 ** Description: Processes the link deactivate resource action request.    **
 **                                                                        **
 ** Inputs:  None                                                      **
 **     Others: g_link_action                              **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **    been successfully processed.               **
 **     Others: ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
MIH_C_LINK_AC_RESULT_T eRAL_action_link_deactivate_resources(ral_enb_instance_t instanceP)
{
    MIH_C_RESOURCE_DESC_T  *res = &g_link_action.link_ac_param._union.resource_desc;
    struct ral_lte_channel *currChannel;
    int                     mt_ix, ch_ix, f_ix;
    int                     cnxid;

    /* Get the connection identifier */
    f_ix = eRAL_action_get_channel_id(instanceP, &res->flow_id, &cnxid);
    if (f_ix < 0) {
        LOG_D(RAL_ENB, " Link action DEACTIVATE_RESOURCES requested while link resource is NOT activated\n");
        return MIH_C_LINK_AC_RESULT_SUCCESS;
    }

    /* Get MT and RB channel identifiers */
    if (eRAL_process_find_channel(instanceP, cnxid, &mt_ix, &ch_ix) != 0) {
        /* Unicast data flow */
        currChannel = &(g_enb_ral_obj[instanceP].mt[mt_ix].radio_channel[ch_ix]);
        LOG_D(RAL_ENB, " %s: Unicast MT's address = %s\n", __FUNCTION__,
                eRAL_process_mt_addr_to_string(g_enb_ral_obj[instanceP].mt[mt_ix].ipv6_addr));
    }
    else {
        /* Multicast data flow */
        currChannel = &(g_enb_ral_obj[instanceP].mcast.radio_channel);
        LOG_D(RAL_ENB, " %s: Multicast MT's address = %s\n", __FUNCTION__,
                eRAL_process_mt_addr_to_string(g_enb_ral_obj[instanceP].mcast.mc_group_addr));
    }

    if (currChannel->status == NAS_DISCONNECTED) {
        /* The resource is already in the required state */
        LOG_D(RAL_ENB, " Link action request DEACTIVATE_RESOURCES is currently in progress\n");
        return MIH_C_LINK_AC_RESULT_SUCCESS;
    }

    /* The resource is not in the required state:
     * Remove the connection identifier from the list of active resources
     * and go ahead in the request processing. */
    (void) eRAL_action_del_channel_id(instanceP, f_ix);

    int rc;
#ifdef RAL_DUMMY
    rc = eRAL_NAS_send_rb_release_request(mt_ix, ch_ix);
#endif
#ifdef RAL_REALTIME
    // LG rc = RAL_process_NAS_message(IO_OBJ_RB, IO_CMD_DEL, mt_ix, ch_ix);
#endif

    if (rc < 0) {
        /* Failed to send RB release request */
        return MIH_C_LINK_AC_RESULT_FAILURE;
    }

    /* Link action successfully processed */
    g_enb_ral_obj[instanceP].pending_req_action = MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

#endif // MIH_C_MEDIEVAL_EXTENSIONS
