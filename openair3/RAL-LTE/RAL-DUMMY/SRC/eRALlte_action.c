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

#include <assert.h>

#include "eRALlte_action.h"

#include "eRALlte_mih_msg.h"
#include "eRALlte_variables.h"
#include "eRALlte_constants.h"
#include "eRALlte_proto.h"

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

static int _eRALlte_action_is_in_progress(MIH_C_STATUS_T* status,
					  MIH_C_LINK_AC_RESULT_T* ac_status,
					  MIH_C_LINK_AC_TYPE_T action);

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_flow_attr(void);
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_activate_resources(void);
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_deactivate_resources(void);

/*
 * ---------------------------------------------------------------------------
 * Flow identifier management:
 * Radio Bearer data flows are identified by a source address, a destination
 * address and a port number for a particular IP transport protocol (UDP,
 * TCP). A private data structure is used to map upper-layer flow identifiers
 * to lower-layer RB channel identifiers. It is handled by private functions.
 * ---------------------------------------------------------------------------
 */
/* Structure of the destination data flow */
struct Data_flow {
    unsigned char addr[16];	// IP address
    unsigned int l2id[2];	// L2 identifier
    unsigned int  port;		// IP port identifier
    int proto;			// IP protocol
    int cnxid;			// Data flow identifier
};
typedef struct {
    int n_flows;
#define ACTION_MAX_FLOW		((RAL_MAX_MT)*(RAL_MAX_RB))
    struct Data_flow flow [ACTION_MAX_FLOW];
    int flow_id [ACTION_MAX_FLOW];  //added TEMP MW 23/05/13
} eRALlte_action_DataFlowList_t;

static eRALlte_action_DataFlowList_t g_flows = {};

static int _eRALlte_action_set_channel_id (MIH_C_FLOW_ID_T* flowId, int cnxid);
static int _eRALlte_action_get_channel_id (MIH_C_FLOW_ID_T* flowId, int* cnxid);
static int _eRALlte_action_del_channel_id (int fix);
#endif // MIH_C_MEDIEVAL_EXTENSIONS

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_action_request()                                  **
 **                                                                        **
 ** Description: Processes the Link_Action.request message and sends a     **
 **		 Link_Action.confirm message to the MIHF.                  **
 **                                                                        **
 ** Inputs:	 msgP:		Pointer to the received message            **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	g_link_action                              **
 **                                                                        **
 ***************************************************************************/
void eRALlte_action_request(MIH_C_Message_Link_Action_request_t* msgP)
{
    MIH_C_STATUS_T         status;
    MIH_C_LINK_AC_TYPE_T   link_action_type;
    MIH_C_LINK_AC_RESULT_T link_action_result;
    int link_action_done = 0;

    memcpy(&g_link_action, &msgP->primitive.LinkAction, sizeof(MIH_C_LINK_ACTION_T));

    status                        = MIH_C_STATUS_SUCCESS;
    link_action_type              = msgP->primitive.LinkAction.link_ac_type;
    link_action_result            = MIH_C_LINK_AC_RESULT_SUCCESS;    

    /*
     * Read link action attributs
     * --------------------------
     */
    if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN)
    {
	/*
	 * Link scan operation request - Not supported by the network side:
	 * No measurements
	 */
	DEBUG(" ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN: REFUSED\n");
	link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
	eRALlte_send_link_action_confirm(&msgP->header.transaction_id,
					 &status,
					 NULL,
					 &link_action_result);
    }

    if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN)
    {
	/*
	 * Link resource retain operation request - Not supported by the
	 * network side.
	 */
	DEBUG(" ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN: REFUSED\n");
	link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
	eRALlte_send_link_action_confirm(&msgP->header.transaction_id,
					 &status,
					 NULL,
					 &link_action_result);
    }

    if (msgP->primitive.LinkAction.link_ac_attr & MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ)
    {
	/*
	 * Data forward operation request - Not supported by the network side.
	 */
	DEBUG(" ACTION ATTRIBUTE MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ: REFUSED\n");
	link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
	eRALlte_send_link_action_confirm(&msgP->header.transaction_id,
					 &status,
					 NULL,
					 &link_action_result);
    }

    /*
     * Read link action request
     * ------------------------
     * Requested actions that are not supported or currently in progress
     * are refused.
     */
    if (_eRALlte_action_is_in_progress(&status,
				       &link_action_result,
				       link_action_type))
    {
	DEBUG(" Link action request %s is not supported or currently in progress\n", g_link_action_type_str[link_action_type]);
    }
    else
    {
	DEBUG(" %s ACTION REQUESTED: %s\n", __FUNCTION__, g_link_action_type_str[link_action_type]);

	switch (link_action_type)
	{
	    case MIH_C_LINK_AC_TYPE_NONE:
		DEBUG("  NO ACTION\n");
		break;

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
	    case MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR:
		/*
		 * Provide the mark and multicast configuration to be set up
		 * for the data flow
		 */
		ralpriv->pending_req_ac_result = _eRALlte_action_link_flow_attr();
		link_action_done = 1;
		break;

	    case MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES:
		/*
		 * Configure and activate a data flow
		 */
		ralpriv->pending_req_ac_result = _eRALlte_action_link_activate_resources();
		link_action_done = 1;
		break;

	    case MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES:
		/*
		 * Deactivate a previously activated data flow
		 */
		ralpriv->pending_req_ac_result = _eRALlte_action_link_deactivate_resources();
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
		/*
		 * Cause the link to power up and establish L2 connectivity.
		 * For UMTS link type, power up lower layers and establish
		 * PDP context - Not supported by the network side
		 */
		DEBUG("  REFUSED\n");
		link_action_result = MIH_C_LINK_AC_RESULT_REFUSED;
		break;

	    default:
		ERR(" %s Invalid LinkAction.link_ac_type %d\n",
		    __FUNCTION__, link_action_type);
		status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
	}
    }

    /* Return link action confirmation to the MIH-F */
    if (link_action_done) {
	ralpriv->pending_req_transaction_id = msgP->header.transaction_id;
	ralpriv->pending_req_status = MIH_C_STATUS_SUCCESS;
	eRALlte_send_link_action_confirm(&ralpriv->pending_req_transaction_id,
					 &ralpriv->pending_req_status,
					 NULL,
					 &ralpriv->pending_req_ac_result);
    }
    else if (status == MIH_C_STATUS_SUCCESS) {
	eRALlte_send_link_action_confirm(&msgP->header.transaction_id,
					 &status,
					 NULL,
					 &link_action_result);
    }
    else {
	eRALlte_send_link_action_confirm(&msgP->header.transaction_id,
					 &status,
					 NULL,
					 NULL);
    }

    ralpriv->pending_req_action = 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_action_save_flow_id()                             **
 **                                                                        **
 ** Description: Save connection identifier and data of the specified data **
 **		 flow into the list of active data flows.                  **
 **                                                                        **
 ** Inputs:	 flowId:	The data flow identifier                   **
 ** 	 	 cnxid:		The connection identifier                  **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	The index of the specified data flow in    **
 **				the list of active data flows.             **
 **				-1 if the list is full.                    **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
extern int eRALlte_action_save_flow_id(MIH_C_FLOW_ID_T* flowId, int cnxid){
    return _eRALlte_action_set_channel_id(flowId, cnxid);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 ** Name:	 eRALlte_action_set_channel_id()                           **
 **                                                                        **
 ** Description: Set the Connection identifier and store data of the       **
 **		 specified data flow in the list of active data flows.     **
 ** Inputs:	 flowId:	The data flow identifier                   **
 ** 	 	 cnxid:		The connection identifier                  **
 ** Outputs:	 None                                                      **
 **		 Return:	The index of the specified data flow in    **
 **				the list of active data flows.             **
 **				-1 if the list is full.                    **
 ***************************************************************************/
static int _eRALlte_action_set_channel_id (MIH_C_FLOW_ID_T* flowId, int cnxid){
    char addr[128];
    //char port[8];
    int f_ix;

    assert(cnxid != 0);

    for (f_ix = 0; f_ix < ACTION_MAX_FLOW; f_ix++) {
	if (g_flows.flow[f_ix].cnxid > 0) continue;
	g_flows.flow[f_ix].cnxid = cnxid;
	g_flows.flow_id[f_ix] = (int) flowId;
	g_flows.n_flows += 1;

        // Modified Michelle
	memcpy((char*)&(DestIpv6Addr[0][16]), addr, 16);
	eRALlte_process_mt_addr_to_l2id(&g_flows.flow[f_ix].addr[8],
					&g_flows.flow[f_ix].l2id[0]);

/*	MIH_C_TRANSPORT_ADDR_VALUE2String(&flowId->dest_addr.ip_addr.address, addr);
	memcpy((char*)&(g_flows.flow[f_ix].addr), addr, 16);
	eRALlte_process_mt_addr_to_l2id(&g_flows.flow[f_ix].addr[8],
					&g_flows.flow[f_ix].l2id[0]);
	MIH_C_PORT2String(&flowId->dest_addr.port, port);
	g_flows.flow[f_ix].port = strtol(port, (char**) NULL, 16);
	g_flows.flow[f_ix].proto = flowId->transport_protocol;*/
	return f_ix;
    }
    return (-1);
}

/****************************************************************************
 ** Name:	 eRALlte_action_get_channel_id()                           **
 **                                                                        **
 ** Description: Returns the Connection identifier of the specified data   **
 **		 flow.                                                     **
 ** Inputs:	 flowId:	The data flow identifier                   **
 ** 	 	 Others:	None                                       **
  ** Outputs:	 cnxid:		The connection identifier allocated to the **
 **				specified data flow.                       **
 **		 Return:	The index of the specified data flow in    **
 **				the list of active data flows.             **
 **				-1 if no any connection identifier exists  **
 **				for the specified data flow.               **
 **		 Others:	None                                       **
 ***************************************************************************/
static int _eRALlte_action_get_channel_id (MIH_C_FLOW_ID_T* flowId, int* cnxid){
    //char addr[128];
    //char port[8];
    //unsigned int dp;
    int f_ix;

//     MIH_C_TRANSPORT_ADDR_VALUE2String(&flowId->dest_addr.ip_addr.address, addr);
//     MIH_C_PORT2String(&flowId->dest_addr.port, port);
//     dp = strtol(port, (char**) NULL, 16);

    for (f_ix = 0; f_ix < ACTION_MAX_FLOW; f_ix++) {
/*	if (!eRALlte_process_cmp_mt_addr((const char*)addr, (const char*)g_flows.flow[f_ix].l2id)) continue;
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
 ** Name:	 eRALlte_action_del_channel_id()                           **
 **                                                                        **
 ** Description: Remove the data flow stored at the specified index in the **
 **		 list of active data flows.                                **
 **                                                                        **
 ** Inputs:	 fix:		Index of the data flow in the list         **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	The number of the remaining data flows in  **
 **				the list.                                  **
 **				-1 if the specified index is out of the    **
 **				boundary of the list.                      **
 **		 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _eRALlte_action_del_channel_id (int fix)
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
 ** Name:	 _eRALlte_action_is_in_progress()                          **
 **                                                                        **
 ** Description: Checks if an action request is supported and whether it   **
 **		 is currently in progress.                                 **
 **                                                                        **
 ** Inputs:	 action:	Type of link action                        **
 ** 	 	 Others:	g_link_action, ralpriv                     **
 **                                                                        **
 ** Outputs:	 status:	MIH request status                         **
 **		 ac_status:	Action request status                      **
 **		 Return:	1 if the request is not supported or is    **
 **				currently in progress and the resource is  **
 **				already in the required state. 0 otherwise.**
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static int _eRALlte_action_is_in_progress(MIH_C_STATUS_T* status,
					  MIH_C_LINK_AC_RESULT_T* ac_status,
					  MIH_C_LINK_AC_TYPE_T action)
{
    /* Check whether the action link command is supported */
    if (!(ralpriv->mih_supported_link_command_list & MIH_C_BIT_LINK_ACTION)) {
	*status = MIH_C_STATUS_REJECTED;
	return 1;
    }
    *status = MIH_C_STATUS_SUCCESS;

    /* Check whether the action request is supported */
    if (ralpriv->mih_supported_link_action_list & (1 << action))
    {
	/* Check whether another action request is currently in progress */
	if ((ralpriv->pending_req_action) && (ralpriv->pending_req_action != action))
	{
	    /* Another action request is in progress:
	     * Do not process new request before completion of this one */
	    *ac_status = MIH_C_LINK_AC_RESULT_REFUSED;
	    return 1;	    
	}

	/* The action request is supported and no other request is in progress:
	 * Go ahead and process the request */
	return 0;
    }

    /* The link action request is not supported */
    *ac_status = MIH_C_LINK_AC_RESULT_INCAPABLE;
    return 1;
}

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
/****************************************************************************
 **                                                                        **
 ** Name:	 _eRALlte_action_link_flow_attr()                          **
 **                                                                        **
 ** Description: Processes the link flow attribut action request.          **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	g_link_action                              **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **				been successfully processed.               **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_flow_attr(void)
//TODO
{
    MIH_C_FLOW_ATTRIBUTE_T *flow = &g_link_action.link_ac_param._union.flow_attribute;
    int mt_ix, ch_ix, f_ix;
    int cnxid;

    /* Get the connection identifier */
    f_ix = _eRALlte_action_get_channel_id(&flow->flow_id, &cnxid);
    if (f_ix < 0) {
	DEBUG(" No RB allocated for this data flow\n");
	return MIH_C_LINK_AC_RESULT_REFUSED;
    }

    /* Get MT and RB channel identifiers */
    if (eRALlte_process_find_channel(cnxid, &mt_ix, &ch_ix) != 0) {
	/* Unicast data flow */
	DEBUG (" %s: Unicast MT's address = %s\n", __FUNCTION__,
	       eRALlte_process_mt_addr_to_string(ralpriv->mt[mt_ix].ipv6_addr));
    }
    else {
	/* Multicast data flow */
	DEBUG (" %s: Multicast MT's address = %s\n", __FUNCTION__,
	       eRALlte_process_mt_addr_to_string(ralpriv->mcast.mc_group_addr));
    }

    if (flow->choice_mark_qos) {
	DEBUG(" Mark QoS enabled\n");
	//TODO ???
    }

    if (flow->choice_mark_drop_eligibility) {
	DEBUG(" Mark drop eligibility enabled\n");
	//TODO ???
    }

    /* Link action successfully processed */
    ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _eRALlte_action_link_activate_resources()                 **
 **                                                                        **
 ** Description: Processes the link activate resource action request.      **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	g_link_action                              **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **				been successfully processed.               **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_activate_resources(void)
{
    MIH_C_RESOURCE_DESC_T *res = &g_link_action.link_ac_param._union.resource_desc;
    MIH_C_COS_T classIdDL = 0;
    MIH_C_COS_T classIdUL = 0;
    MIH_C_LINK_DATA_RATE_T resBitrateDL = 0;
    MIH_C_LINK_DATA_RATE_T resBitrateUL = 0;
    MIH_C_BOOLEAN_T multicast = MIH_C_BOOLEAN_FALSE;

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
	DEBUG (" %s: MT's address = %s\n", __FUNCTION__, eRALlte_process_mt_addr_to_string((unsigned char*)mt_addr));
    }
    else {
	ERR (" %s : IP Address is NOT valid (len=%d)\n", __FUNCTION__, len);
	return MIH_C_LINK_AC_RESULT_FAILURE;
    }

#if 0
    char mt_addr[128];
    if (res->link_id.link_addr.choice == MIH_C_CHOICE_3GPP_3G_CELL_ID) {
	char link_addr[128];
	int len = MIH_C_LINK_ADDR2String(&res->link_id.link_addr, link_addr);
	if ( (len > 0) && (len <= 128) ) {
	    int i;
	    DEBUG (" %s : Link address = %s\n", __FUNCTION__, link_addr);
	    for (i=0; i < len; i++) {
		if (link_addr[i] == '=') break;
	    }
	    strncpy(mt_addr, &link_addr[i+2], 16);
	    DEBUG (" %s : MT address = %s\n", __FUNCTION__, mt_addr);
	} else {
	    ERR (" %s : Link address is NOT valid (len=%d)\n", __FUNCTION__, len);
	    return MIH_C_LINK_AC_RESULT_FAILURE;
	}
    }
    else {
	ERR (" %s : Link address is NOT valid (type=%d, not 3GPP_3G_CELL_ID)\n",
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
    f_ix = _eRALlte_action_get_channel_id(&res->flow_id, &cnxid);
    if (f_ix != -1) {
	/* Get MT and RB channel identifiers */
	if (eRALlte_process_find_channel(cnxid, &mt_ix, &ch_ix) != 0) {
	    /* Unicast data flow */
	    currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
	} else {
	    /* Multicast data flow */
	    assert(multicast == MIH_C_BOOLEAN_TRUE);
	    currChannel = &(ralpriv->mcast.radio_channel);
	}

	if (currChannel->status == NAS_CONNECTED) {
	    /* The resource is already in the required state */
	    DEBUG(" Link action ACTIVATE_RESOURCES requested while link resource is ALREADY activated\n");
	    return MIH_C_LINK_AC_RESULT_SUCCESS;
	}

	/* The resource is not in the required state:
	 * Remove the connection identifier from the list of active resources
	 * and process the action request again. */
	DEBUG(" Resource has been activated but is not in the ACTIVE state\n");
	(void) _eRALlte_action_del_channel_id(f_ix);
    }

    MIH_C_LINK_TUPLE_ID_T *ltid;
    int is_ready_for_rb_establish = 1;

    if (multicast) {
	/*
	 * Multicast data flow:
	 */
	mt_ix = RAL_MAX_MT;
	ch_ix = 0;
	currChannel = &(ralpriv->mcast.radio_channel);
	currChannel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+1;
	currChannel->rbId = RAL_DEFAULT_RAB_ID;
	currChannel->multicast = 1;
	memcpy((char*)&(ralpriv->mcast.mc_group_addr), mt_addr, 16);
	ltid = &(ralpriv->mcast.ltid);
    }
    else {
	/*
	 * Unicast data flow
	 */
	/* Get the list of MTs waiting for RB establishment */
        #ifdef RAL_REALTIME
        TQAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_LIST, 0, 0);
        TQAL_process_NAS_message(IO_OBJ_RB, IO_CMD_LIST, 0, 0);
        #endif
	/* Check if the MT is in the list */
	mt_ix = eRALlte_process_find_mt_by_addr(mt_addr);

	if ( (mt_ix < RAL_MAX_MT) &&
	     (ralpriv->mt[mt_ix].mt_state == RB_CONNECTED)) {
	    /* The MT is ready for RB establishment */
	    ch_ix = eRALlte_process_find_new_channel(mt_ix);
	    if (ch_ix == RAL_MAX_RB) {
		DEBUG(" No RB available in MT\n");
		return MIH_C_LINK_AC_RESULT_REFUSED;
	    }
	    currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
	    currChannel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
	    currChannel->rbId = RAL_DEFAULT_RAB_ID + ch_ix;
	    currChannel->multicast = 0;
	    DEBUG(" mt_ix %d, ch_ix %d, cnx_id %d, rbId %d\n",
		  mt_ix, ch_ix, currChannel->cnx_id, currChannel->rbId);
	    memcpy((char *)&(ralpriv->mt[mt_ix].ipv6_addr), mt_addr, 16);
	    ltid = &(ralpriv->mt[mt_ix].ltid);
	}
	else {
	    /* The MT is NOT ready for RB establishment */
	    is_ready_for_rb_establish = 0;
	    int qos_is_valid = 0;

	    DEBUG(" Mobile Terminal not ready - Storing request data\n");
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
		DEBUG(" Received QoS parameters are valid\n");
	    } else {
		DEBUG(" Received QoS parameters are NOT valid - Request will be rejected\n");
		return MIH_C_LINK_AC_RESULT_REFUSED;
	    }

	    currChannel = &(ralpriv->pending_mt.radio_channel[0]);
	    currChannel->cnx_id = 0;
	    currChannel->rbId = 0;
	    currChannel->multicast = 0;
	    memcpy((char *)&(ralpriv->pending_mt.ipv6_addr), mt_addr, 16);

	    ltid = &(ralpriv->pending_mt.ltid);
	    ralpriv->pending_req_flag = 1;

	} /* end MT not ready */

    } /* end unicast data flow */

    /* Save the current data flow identifier into the list of active data flows */
    if (currChannel->cnx_id != 0) {
	f_ix = _eRALlte_action_set_channel_id(&res->flow_id, currChannel->cnx_id);
	if (f_ix < 0) {
	    DEBUG(" No RB available\n");
	    return MIH_C_LINK_AC_RESULT_REFUSED;
	}
    }
    else {
	/* The current request is pending waiting for RB establishment */
	f_ix = 0;
	ralpriv->pending_req_fid = res->flow_id;
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
	DEBUG(" qos value : DIR %d, flowId %d, classId %d, resBitrate %.1f\n", i , currChannel->flowId[i], currChannel->classId[i], currChannel->resBitrate[i]);
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
	int mapping_result = eRALlte_process_map_qos(mt_ix, ch_ix);
	if (mapping_result) {
	    int rc;
#ifdef RAL_DUMMY
	    rc = eRALlte_NAS_send_rb_establish_request(mt_ix, ch_ix);
#endif
#ifdef RAL_REALTIME
	    rc = TQAL_process_NAS_message(IO_OBJ_RB, IO_CMD_ADD, mt_ix, ch_ix);
#endif
	    if (rc < 0) {
		/* Failed to send RB establishment request */
		return MIH_C_LINK_AC_RESULT_FAILURE;
	    }
	}
	else {
	    /* QoS mapping is not supported */
	    return MIH_C_LINK_AC_RESULT_REFUSED;
	}
    }
    else {
	/* Wait for MT coming ready;
	 * re-try to establish RB upon timer expiration */
#ifdef RAL_DUMMY
	ralpriv->pending_mt_timer = 5;
#endif
#ifdef RAL_REALTIME
	ralpriv->pending_mt_timer = 300;
#endif
    }

    /* Link action successfully processed */
    ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 _eRALlte_action_link_deactivate_resources()               **
 **                                                                        **
 ** Description: Processes the link deactivate resource action request.    **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	g_link_action                              **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	MIH_C_LINK_AC_RESULT_SUCCESS if action has **
 **				been successfully processed.               **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static MIH_C_LINK_AC_RESULT_T _eRALlte_action_link_deactivate_resources(void)
{
    MIH_C_RESOURCE_DESC_T *res = &g_link_action.link_ac_param._union.resource_desc;
    struct ral_lte_channel *currChannel;
    int mt_ix, ch_ix, f_ix;
    int cnxid;

    /* Get the connection identifier */
    f_ix = _eRALlte_action_get_channel_id(&res->flow_id, &cnxid);
    if (f_ix < 0) {
	DEBUG(" Link action DEACTIVATE_RESOURCES requested while link resource is NOT activated\n");
	return MIH_C_LINK_AC_RESULT_SUCCESS;
    }

    /* Get MT and RB channel identifiers */
    if (eRALlte_process_find_channel(cnxid, &mt_ix, &ch_ix) != 0) {
	/* Unicast data flow */
	currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
	DEBUG (" %s: Unicast MT's address = %s\n", __FUNCTION__,
	       eRALlte_process_mt_addr_to_string(ralpriv->mt[mt_ix].ipv6_addr));
    }
    else {
	/* Multicast data flow */
	currChannel = &(ralpriv->mcast.radio_channel);
	DEBUG (" %s: Multicast MT's address = %s\n", __FUNCTION__,
	       eRALlte_process_mt_addr_to_string(ralpriv->mcast.mc_group_addr));
    }

    if (currChannel->status == NAS_DISCONNECTED) {
	/* The resource is already in the required state */
	DEBUG(" Link action request DEACTIVATE_RESOURCES is currently in progress\n");
	return MIH_C_LINK_AC_RESULT_SUCCESS;
    }

    /* The resource is not in the required state:
     * Remove the connection identifier from the list of active resources
     * and go ahead in the request processing. */
    (void) _eRALlte_action_del_channel_id(f_ix);

    int rc;
#ifdef RAL_DUMMY
    rc = eRALlte_NAS_send_rb_release_request(mt_ix, ch_ix);
#endif
#ifdef RAL_REALTIME
    rc = TQAL_process_NAS_message(IO_OBJ_RB, IO_CMD_DEL, mt_ix, ch_ix);
#endif

    if (rc < 0) {
	/* Failed to send RB release request */
	return MIH_C_LINK_AC_RESULT_FAILURE;
    }

    /* Link action successfully processed */
    ralpriv->pending_req_action = MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES;
    return MIH_C_LINK_AC_RESULT_SUCCESS;
}

#endif // MIH_C_MEDIEVAL_EXTENSIONS
