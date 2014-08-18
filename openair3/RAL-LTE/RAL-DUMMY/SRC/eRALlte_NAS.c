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

#include "eRALlte_proto.h"

#include "eRALlte_variables.h"
#include "eRALlte_mih_msg.h"
#include "nas_rg_netlink.h"

#include "MIH_C_Types.h"

/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

extern int g_sockd_nas;		// defined in eRALlte_main.c

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/* Read/write buffer used to receive/send messages from/to the NAS driver */
static char nas_rg_msg[NAS_RG_NETL_MAXLEN];

/* Request messages sent to the NAS driver */
static struct nas_rg_netl_request *nas_rg_request = (struct nas_rg_netl_request *) nas_rg_msg;

/* Reply message received from the NAS driver */
static struct nas_rg_netl_reply *nas_rg_reply = (struct nas_rg_netl_reply *) nas_rg_msg;

/* Functions used to write messages being sent to the NAS driver */
static int eRALlte_NAS_write_rb_establish_request(int mt_ix, int ch_ix);
static int eRALlte_NAS_write_rb_release_request(int mt_ix, int ch_ix);
static int eRALlte_NAS_write_cnx_status_request(int mt_ix);
static int eRALlte_NAS_write_rb_list_request(int mt_ix);

/* Functions used to read messages being received from the NAS driver */
static int eRALlte_NAS_read_rb_establish_reply(void);
static int eRALlte_NAS_read_rb_release_reply(void);
static int eRALlte_NAS_read_cnx_status_reply(void);
static int eRALlte_NAS_read_rb_list_reply(void);

static void imei2l2id(const uint8_t* imei, uint32_t* l2id);

static void _eRALlte_NAS_get_MTs_list(int n_mts, int n_rbs);
static int _eRALlte_NAS_update_MTs_list(int mt_ix, int ch_ix);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_get_MTs_list()                                **
 **                                                                        **
 ** Description: Gets the list of attached Mobile Terminals.               **
 **		 (Dummy list: Hard coded for testing purpose)              **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	DestIpv6Addr[][]                           **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
void eRALlte_NAS_get_MTs_list(void)
{
    /* Number of MTs = 2, Number of RBs = 1 */
    _eRALlte_NAS_get_MTs_list(2, 1); // DUMMY
}
static void _eRALlte_NAS_get_MTs_list(int n_mts, int n_rbs)
{
    struct ral_lte_channel *channel;
    int mt_ix, ch_ix;

    memcpy(ralpriv->plmn, DefaultPLMN, DEFAULT_PLMN_SIZE); // DUMMY
    ralpriv->curr_cellId = RAL_DEFAULT_CELL_ID; // DUMMY

    /* Number of MTs in the list */
    if (n_mts > RAL_MAX_MT) {
	/* Should not exceed RAL_MAX_MT */
	n_mts = RAL_MAX_MT;
    }
    for (mt_ix=0; mt_ix < n_mts; ++mt_ix)
    {
	ralpriv->mt[mt_ix].ue_id = mt_ix;
//      ralpriv->mt[mt_ix].ipv6_l2id[0]= 406787;
//      ralpriv->mt[mt_ix].ipv6_l2id[1]= 1351685248 + mt_ix;
	ralpriv->mt[mt_ix].num_class = 2;
	ralpriv->mt[mt_ix].nas_state = 0x0A;
	ralpriv->mt[mt_ix].mt_state = RB_CONNECTED;

	/* Number of Radio Bearers attached to that MT */
	if (n_rbs > RAL_MAX_RB) {
	    /* Should not exceed RAL_MAX_RB */
	    n_rbs = RAL_MAX_RB;
	}
	ralpriv->mt[mt_ix].num_rbs = n_rbs;
	for (ch_ix = 0; ch_ix < ralpriv->mt[mt_ix].num_rbs; ch_ix++) {
	    channel = &ralpriv->mt[mt_ix].radio_channel[ch_ix];
	    channel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
	    channel->rbId = RAL_DEFAULT_RAB_ID;
	    channel->RadioQoSclass = RAL_DEFAULT_RAB_QoS;
	    channel->dscpUL = 0;
	    channel->dscpDL = 0;
	    channel->nas_state = 16;
	    channel->status = NAS_CONNECTED;
	}
	ralpriv->num_connected_mts++;

	/* MT's IPv6 L2 identifier */
	if (mt_ix < ADDR_MAX) {
	    memcpy((char *) ralpriv->mt[mt_ix].ipv6_addr,
		   (char *) DestIpv6Addr[mt_ix], 16);
	    imei2l2id((uint8_t*) &DestIpv6Addr[mt_ix][8],
		      (uint32_t*) &ralpriv->mt[mt_ix].ipv6_l2id[0]);
	    DEBUG(" MT%d initialized : address %d %d\n", mt_ix,
		  ralpriv->mt[mt_ix].ipv6_l2id[0],
		  ralpriv->mt[mt_ix].ipv6_l2id[1]);
	}
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_update_MTs_list()                             **
 **                                                                        **
 ** Description: Updates the list of attached Mobile Terminals.            **
 **		 (Dummy list: Hard coded for testing purpose)              **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	The index of the MT that has been updated  **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
int eRALlte_NAS_update_MTs_list(void)
{
    /* MT index = 2, RB channel index = 0 */
    return _eRALlte_NAS_update_MTs_list(2, 0); // DUMMY
}
static int _eRALlte_NAS_update_MTs_list(int mt_ix, int ch_ix)
{
    struct ral_lte_channel *channel;

    /* MT index */
    if (mt_ix < RAL_MAX_MT) {
	ralpriv->mt[mt_ix].ue_id = mt_ix;
	ralpriv->mt[mt_ix].num_rbs = 1;
	ralpriv->mt[mt_ix].num_class = 2;
	ralpriv->mt[mt_ix].nas_state = 0x0A;
	ralpriv->mt[mt_ix].mt_state = RB_CONNECTED;
	/* Default RB channel */
	if (ch_ix < RAL_MAX_RB) {
	    channel = &ralpriv->mt[mt_ix].radio_channel[ch_ix];
	    channel->cnx_id = (RAL_MAX_RB_PER_UE*mt_ix)+ch_ix+1;
	    channel->rbId = RAL_DEFAULT_RAB_ID;
	    channel->RadioQoSclass = RAL_DEFAULT_RAB_QoS;
	    channel->dscpUL = 0;
	    channel->dscpDL = 0;
	    channel->nas_state = 16;
	    channel->status = NAS_CONNECTED;
	}
	ralpriv->num_connected_mts++;

	ralpriv->mt[mt_ix].ltid = ralpriv->pending_mt.ltid;

	imei2l2id((uint8_t*) &ralpriv->pending_mt.ipv6_addr[8],
		  (uint32_t*) &ralpriv->mt[mt_ix].ipv6_l2id[0]);
	DEBUG (" MT%d initialized : address %d %d\n", mt_ix,
	       ralpriv->mt[mt_ix].ipv6_l2id[0],
	       ralpriv->mt[mt_ix].ipv6_l2id[1]);
	return mt_ix;
    }
    return RAL_MAX_MT;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_process_message()                             **
 **                                                                        **
 ** Description: Processes messages received from the NAS sublayer.        **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	g_sockd_nas, nas_rg_msg                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the received message has been         **
 **				successfully processed; -1 otherwise.      **
 ** 	 	 Others:	                                           **
 **                                                                        **
 ***************************************************************************/
int eRALlte_NAS_process_message(void)
{
    int rc = 0;
    int n;

    n = recv(g_sockd_nas, nas_rg_msg, NAS_RG_NETL_MAXLEN, 0);
    if (n <= 0) {
        if (n < 0) {
	    ERR(" %s : recv() failed (%d)\n", __FUNCTION__, errno);
	}
        rc = -1;
    }
    else if (n != nas_rg_reply->length) {
	ERR(" %s : invalid message length %d (should be %d)", __FUNCTION__,
	    n, nas_rg_reply->length);
	rc = -1;
    }

    if (!rc) {
	rc = -1;

        switch (nas_rg_reply->type)
	{
	    /*
	     * Process Radio Bearer establish reply message
	     */
	    case NAS_RG_MSG_RB_ESTABLISH_REPLY:
		DEBUG(" NAS_RG_MSG_RB_ESTABLISH_REPLY received from NAS (%d)\n", nas_rg_reply->length);
#ifdef MSCGEN_PYTOOL
		NOTICE("[MSC_MSG][%s][nas][--- NAS_RG_MSG_RB_ESTABLISH_REPLY --->][%s]\n",
		       getTimeStamp4Log(), g_link_id);
#endif
		rc = eRALlte_NAS_read_rb_establish_reply();
		break;

	    /*
	     * Process Radio Bearer release reply message
	     */
	    case NAS_RG_MSG_RB_RELEASE_REPLY:
		DEBUG(" NAS_RG_MSG_RB_RELEASE_REPLY received from NAS (%d)\n", nas_rg_reply->length);
#ifdef MSCGEN_PYTOOL
		NOTICE("[MSC_MSG][%s][nas][--- NAS_RG_MSG_RB_RELEASE_REPLY --->][%s]\n",
		       getTimeStamp4Log(), g_link_id);
#endif
		rc = eRALlte_NAS_read_rb_release_reply();
		break;

	    case NAS_RG_MSG_MT_MCAST_JOIN_REP:
		DEBUG(" NAS_RG_MSG_MT_MCAST_JOIN_REP received ");
		break;

	    case NAS_RG_MSG_MT_MCAST_LEAVE_REP:
		DEBUG(" NAS_RG_MSG_MT_MCAST_LEAVE_REP received ");
		break;

	    /*
	     * Process connection status reply message
	     */
	    case NAS_RG_MSG_CNX_STATUS_REPLY:
		DEBUG(" NAS_RG_MSG_CNX_STATUS_REPLY received from NAS (%d)\n", nas_rg_reply->length);
#ifdef MSCGEN_PYTOOL
		NOTICE("[MSC_MSG][%s][nas][--- NAS_RG_MSG_CNX_STATUS_REPLY --->][%s]\n",
		       getTimeStamp4Log(), g_link_id);
#endif
		rc = eRALlte_NAS_read_cnx_status_reply();
		break;

	    /*
	     * Process Radio Bearer list reply message
	     */
	    case NAS_RG_MSG_RB_LIST_REPLY:
		DEBUG(" NAS_RG_MSG_RB_LIST_REPLY received from NAS (%d)\n", nas_rg_reply->length);
#ifdef MSCGEN_PYTOOL
		NOTICE("[MSC_MSG][%s][nas][--- NAS_RG_MSG_RB_LIST_REPLY --->][%s]\n",
		       getTimeStamp4Log(), g_link_id);
#endif
		rc = eRALlte_NAS_read_rb_list_reply();
		break;

	    case NAS_RG_MSG_STATISTIC_REPLY:
		DEBUG(" NAS_RG_MSG_STATISTIC_REPLY received\n");
		break;

	    case NAS_RG_MSG_MEASUREMENT_REPLY:
		DEBUG(" NAS_RG_MSG_MEASUREMENT_REPLY received\n");
		break;

	    default:
		WARNING(" %s : invalid message Type %d\n",
		     __FUNCTION__, nas_rg_reply->type);
		break;
	}

    }

    return rc;
}

#ifdef RAL_REALTIME
/****************************************************************************
 **                                                                        **
 ** Name:	 TQAL_process_NAS_message()                                **
 **                                                                        **
 ** Description: Sends messages to the NAS sublayer.                       **
 **                                                                        **
 ** Inputs:	 ioctl_obj:	ioctl object identifier                    **
 **		 ioctl_cmd:	ioctl command identifier                   **
 **		 mt_ix:		Mobile Terminal index                      **
 **		 ch_ix:		Radio Bearer channel index                 **
 ** 	 	 Others:	g_sockd_nas, nas_rg_msg, nas_rg_request    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the message has been successfully     **
 **				sent to the NAS driver; -1 otherwise.      **
 ** 	 	 Others:	nas_rg_msg                                 **
 **                                                                        **
 ***************************************************************************/
int TQAL_process_NAS_message(int ioctl_obj, int ioctl_cmd,
			     int mt_ix, int ch_ix)
{
    int rc = -1;
    char msg_name[64];

    memset(nas_rg_msg, 0, NAS_RG_NETL_MAXLEN);

    switch (ioctl_obj)
    {
	case IO_OBJ_STATS:
	    /*
	     * Statistics command
	     */
	    break;

	case IO_OBJ_CNX:
	    /*
	     * Connexion command
	     */
	    switch (ioctl_cmd)
	    {
		case IO_CMD_LIST:
		    strncpy(msg_name, "NAS_RG_MSG_CNX_STATUS_REQUEST", 64);
		    rc = eRALlte_NAS_write_cnx_status_request(mt_ix);
		    break;

		default:
		    ERR(" %s : invalid ioctl command %d for object %d\n",
			__FUNCTION__, ioctl_cmd, ioctl_obj);
		    break;
	    }
	    break;

	case IO_OBJ_RB:
	    /*
	     * Radio Bearer command
	     */
	    switch (ioctl_cmd)
	    {
		case IO_CMD_ADD:
		    strncpy(msg_name, "NAS_RG_MSG_RB_ESTABLISH_REQUEST", 64);
		    rc = eRALlte_NAS_write_rb_establish_request(mt_ix, ch_ix);
		    break;

		case IO_CMD_DEL:
		    strncpy(msg_name, "NAS_RG_MSG_RB_RELEASE_REQUEST", 64);
		    rc = eRALlte_NAS_write_rb_release_request(mt_ix, ch_ix);
		    break;

		case IO_CMD_LIST:
		    strncpy(msg_name, "NAS_RG_MSG_RB_LIST_REQUEST", 64);
		    rc = eRALlte_NAS_write_rb_list_request(mt_ix);
		    break;

		default:
		    ERR(" %s : invalid ioctl command %d for object %d\n",
			__FUNCTION__, ioctl_cmd, ioctl_obj);
		    break;
	    }
	    break;

	case IO_OBJ_MC:
	    /*
	     * Multicast command
	     */
	    switch (ioctl_cmd)
	    {
		case IO_CMD_ADD:
		    break;

		case IO_CMD_DEL:
		    break;

		default:
		    ERR(" %s : invalid ioctl command %d for object %d\n",
			__FUNCTION__, ioctl_cmd, ioctl_obj);
		    break;
	    }
	    break;

	default:
	    ERR(" %s : invalid ioctl object %d\n", __FUNCTION__, ioctl_obj);
	    break;
    }

    if (!rc) {
	rc = send(g_sockd_nas, nas_rg_msg, nas_rg_request->length, 0);
	if (rc < 0) {
	    ERR(": failed to send %s (%d)\n", msg_name, errno);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- %s ---x][nas]\n",
		   getTimeStamp4Log(), g_link_id, msg_name);
#endif
	}
	else {
	    DEBUG (" %s sent to NAS (%d)\n", msg_name, nas_rg_request->length);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- %s --->][nas]\n",
		   getTimeStamp4Log(), g_link_id, msg_name);
#endif
	}
    }

    return rc;
}
#endif

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_send_rb_establish_request()                   **
 **                                                                        **
 ** Description: Sends Radio Bearer establish request to the NAS driver.   **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 ch_ix:		Radio bearer channel index                 **
 ** 	 	 Others:	g_sockd_nas, nas_rg_msg, nas_rg_request    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	-1 if the request message has not been     **
 **				successfully sent to the NAS driver.       **
 **				Otherwise, the number of bytes actually    **
 **				sent to the NAS driver.                    **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int eRALlte_NAS_send_rb_establish_request(int mt_ix, int ch_ix)
{
    int rc = eRALlte_NAS_write_rb_establish_request(mt_ix, ch_ix);
    if (!rc) {
	rc = send(g_sockd_nas, nas_rg_msg, nas_rg_request->length, 0);
	if (rc < 0) {
	    ERR(": failed to send NAS_RG_MSG_RB_ESTABLISH_REQUEST (%d)\n", errno);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- NAS_RG_MSG_RB_ESTABLISH_REQUEST ---x][nas]\n", getTimeStamp4Log(), g_link_id);
#endif
	}
	else {
	    DEBUG (" NAS_RG_MSG_RB_ESTABLISH_REQUEST sent to NAS (%d)\n", nas_rg_request->length);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- NAS_RG_MSG_RB_ESTABLISH_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
#endif
	}
    }

    return rc;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_send_rb_release_request()                     **
 **                                                                        **
 ** Description: Sends Radio Bearer release request to the NAS driver.     **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 ch_ix:		Radio bearer channel index                 **
 ** 	 	 Others:	g_sockd_nas, nas_rg_msg, nas_rg_request    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	-1 if the request message has not been     **
 **				successfully sent to the NAS driver.       **
 **				Otherwise, the number of bytes actually    **
 **				sent to the NAS driver.                    **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
int  eRALlte_NAS_send_rb_release_request(int mt_ix, int ch_ix)
{
    int rc = eRALlte_NAS_write_rb_release_request(mt_ix, ch_ix);
    if (!rc) {
	rc = send(g_sockd_nas, nas_rg_msg, nas_rg_request->length, 0);
	if (rc < 0) {
	    ERR(": failed to send NAS_RG_MSG_RB_RELEASE_REQUEST (%d)\n", errno);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- NAS_RG_MSG_RB_RELEASE_REQUEST ---x][nas]\n", getTimeStamp4Log(), g_link_id);
#endif
	}
	else {
	    DEBUG (" NAS_RG_MSG_RB_RELEASE_REQUEST sent to NAS (%d)\n", nas_rg_request->length);
#ifdef MSCGEN_PYTOOL
	    NOTICE("[MSC_MSG][%s][%s][--- NAS_RG_MSG_RB_RELEASE_REQUEST --->][nas]\n", getTimeStamp4Log(), g_link_id);
#endif
	}
    }

    return rc;
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_write_rb_establish_request()                  **
 **                                                                        **
 ** Description: Writes Radio Bearer establish request to be sent to the   **
 **		 NAS driver.                                               **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 ch_ix:		Radio bearer channel index                 **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	Always returns 0                           **
 ** 	 	 Others:	nas_rg_request, ralpriv                    **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_write_rb_establish_request(int mt_ix, int ch_ix)
{
    struct ral_lte_channel *currChannel;
    struct nas_rg_msg_rb_establish_request* rb_est_req = &(nas_rg_request->tqalNASPrimitive.rb_est_req);

    nas_rg_request->type = NAS_RG_MSG_RB_ESTABLISH_REQUEST;
    nas_rg_request->length = sizeof(struct nas_rg_netl_hdr) + sizeof(struct nas_rg_msg_rb_establish_request);

    if (mt_ix == RAL_MAX_MT) {
        currChannel = &(ralpriv->mcast.radio_channel);
    } else {
        currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
    }

    rb_est_req->cnxid = currChannel->cnx_id;
    rb_est_req->RBParms.rbId = currChannel->rbId;
    rb_est_req->RBParms.QoSclass = currChannel->RadioQoSclass;
    rb_est_req->RBParms.dscp = currChannel->dscpDL;
    rb_est_req->mcast_flag = currChannel->multicast;

    ralpriv->pending_req_mt_ix = mt_ix;
    ralpriv->pending_req_ch_ix = ch_ix;

    return 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_read_rb_establish_reply()                     **
 **                                                                        **
 ** Description: Reads Radio Bearer establish reply received from the NAS  **
 **		 driver.                                                   **
 **		 Returns a Link_Up.indication to the MIH-F.                **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	nas_rg_reply                               **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the message type is valid,            **
 **				-1 otherwise.                              **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_read_rb_establish_reply(void)
{
    if (nas_rg_reply->type == NAS_RG_MSG_RB_ESTABLISH_REPLY)
    {
	struct ral_lte_channel *currChannel;
	struct nas_rg_msg_rb_establish_reply* rb_est_rep = &(nas_rg_reply->tqalNASPrimitive.rb_est_rep);
	int mt_ix, ch_ix;

	MIH_C_LINK_TUPLE_ID_T* ltid;
	int is_unicast = eRALlte_process_find_channel(rb_est_rep->cnxid,
						      &mt_ix, &ch_ix);
	/* Read received parameters */
	if (is_unicast) {
	    ralpriv->mt[mt_ix].ue_id = rb_est_rep->ue_id;
	    currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
	    ltid = &ralpriv->mt[mt_ix].ltid;
	} else {
	    currChannel = &(ralpriv->mcast.radio_channel);
	    ltid = &ralpriv->mcast.ltid;
	}

	/* Check channel status */
	if (rb_est_rep->result == NAS_CONNECTED) {
	    ralpriv->pending_req_status = MIH_C_STATUS_SUCCESS;
	    currChannel->cnx_id = rb_est_rep->cnxid;
	    currChannel->rbId = rb_est_rep->RBParms.rbId;
	    currChannel->RadioQoSclass = rb_est_rep->RBParms.QoSclass;
	    currChannel->dscpDL = rb_est_rep->RBParms.dscp;
	    currChannel->status = rb_est_rep->result;
	} else {
	    ralpriv->pending_req_status = MIH_C_STATUS_REJECTED;
	    /* Clean radio resources */
	    eRALlte_process_clean_channel(currChannel);
	}

	/* Send Link-Up.indication to the MIH-F */
	if (ralpriv->pending_req_status != MIH_C_STATUS_REJECTED) {
	    eRALlte_send_link_up_indication(&ralpriv->pending_req_transaction_id, ltid, NULL, NULL, NULL, NULL);
	}

	ralpriv->pending_req_mt_ix = -1;
	ralpriv->pending_req_ch_ix = -1;
	ralpriv->pending_req_transaction_id = 0;

	return 0;
    }
    return -1;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_write_rb_release_request()                    **
 **                                                                        **
 ** Description: Writes Radio Bearer release request to be sent to the NAS **
 **		 driver.                                                   **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 ch_ix:		Radio Bearer channel index                 **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	Always returns 0                           **
 ** 	 	 Others:	nas_rg_request                             **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_write_rb_release_request(int mt_ix, int ch_ix)
{
    struct ral_lte_channel *currChannel;
    struct nas_rg_msg_rb_release_request* rb_rel_req = &(nas_rg_request->tqalNASPrimitive.rb_rel_req);

    nas_rg_request->type = NAS_RG_MSG_RB_RELEASE_REQUEST;
    nas_rg_request->length = sizeof(struct nas_rg_netl_hdr) + sizeof(struct nas_rg_msg_rb_release_request);

    if (mt_ix == RAL_MAX_MT) {
        currChannel = &(ralpriv->mcast.radio_channel);
        rb_rel_req->ue_id = RAL_MAX_MT;
     } else {
        currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
        rb_rel_req->ue_id = ralpriv->mt[mt_ix].ue_id;
    }

    rb_rel_req->cnxid = currChannel->cnx_id;
    rb_rel_req->rbId  = currChannel->rbId;

    ralpriv->pending_req_mt_ix = mt_ix;
    ralpriv->pending_req_ch_ix = ch_ix;

    return 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_read_rb_release_reply()                       **
 **                                                                        **
 ** Description: Reads Radio Bearer release reply received from the NAS    **
 **		 driver.                                                   **
 **		 Returns a Link_Down.indication to the MIH-F.              **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	nas_rg_reply                               **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the message type is valid,            **
 **				-1 otherwise.                              **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_read_rb_release_reply(void)
{
    if (nas_rg_reply->type == NAS_RG_MSG_RB_RELEASE_REPLY)
    {
	struct ral_lte_channel *currChannel;
	struct nas_rg_msg_rb_release_reply* rb_rel_rep = &(nas_rg_reply->tqalNASPrimitive.rb_rel_rep);
	int mt_ix, ch_ix;
	MIH_C_LINK_DN_REASON_T reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;
	MIH_C_LINK_TUPLE_ID_T* ltid;

	int is_unicast = eRALlte_process_find_channel(rb_rel_rep->cnxid,
						      &mt_ix, &ch_ix);
	/* Read received parameters */
	if (is_unicast) {
	    assert (rb_rel_rep->ue_id == ralpriv->mt[mt_ix].ue_id);
	    currChannel = &(ralpriv->mt[mt_ix].radio_channel[ch_ix]);
	    ltid = &ralpriv->mt[mt_ix].ltid;
	} else {
	    assert (rb_rel_rep->ue_id == RAL_MAX_MT);
	    currChannel = &(ralpriv->mcast.radio_channel);
	    ltid = &ralpriv->mcast.ltid;
	}

	/* Check channel status */
	if (rb_rel_rep->result == NAS_DISCONNECTED) {
	    ralpriv->pending_req_status = MIH_C_STATUS_SUCCESS;
	} else {
	    ralpriv->pending_req_status = MIH_C_STATUS_REJECTED;
	}

	/* Clean radio resources */
	eRALlte_process_clean_channel(currChannel);

	/* Send Link-Down.indication to the MIH-F */
	if (ralpriv->pending_req_status != MIH_C_STATUS_REJECTED) {
	    eRALlte_send_link_down_indication(&ralpriv->pending_req_transaction_id, ltid, NULL, &reason_code);
	}

	ralpriv->pending_req_mt_ix = -1;
	ralpriv->pending_req_ch_ix = -1;
	ralpriv->pending_req_transaction_id = 0;

	return 0;
    }
    return -1;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_write_cnx_status_request()                    **
 **                                                                        **
 ** Description: Writes connection status request to be sent to the NAS    **
 **		 driver.                                                   **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	Always returns 0                           **
 ** 	 	 Others:	nas_rg_request                             **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_write_cnx_status_request(int mt_ix)
{
    struct nas_rg_msg_cnx_status_request* cnx_stat_req = &(nas_rg_request->tqalNASPrimitive.cnx_stat_req);

    nas_rg_request->type = NAS_RG_MSG_CNX_STATUS_REQUEST;
    nas_rg_request->length = sizeof(struct nas_rg_netl_hdr) + sizeof(struct nas_rg_msg_cnx_status_request);

    /* Connection status is requested for MT unicast only */
    assert (mt_ix < RAL_MAX_MT);
    cnx_stat_req->ue_id = ralpriv->mt[mt_ix].ue_id;
    return 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_read_cnx_status_reply()                       **
 **                                                                        **
 ** Description: Reads connection status reply received from the NAS       **
 **		 driver.                                                   **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	nas_rg_reply                               **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the message type is valid,            **
 **				-1 otherwise.                              **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_read_cnx_status_reply(void)
{
    if (nas_rg_reply->type == NAS_RG_MSG_CNX_STATUS_REPLY)
    {
	struct nas_rg_msg_cnx_status_reply* cnx_stat_rep = &(nas_rg_reply->tqalNASPrimitive.cnx_stat_rep);

	int mt_ix = cnx_stat_rep->ue_id;

	/* Connection status has been requested for MT unicast only */
	assert (mt_ix < RAL_MAX_MT);
	assert (mt_ix == ralpriv->mt[mt_ix].ue_id);

	/* Read MT connection establishment status */
	ralpriv->mt[mt_ix].mt_state = cnx_stat_rep->status; // TODO ???
	return 0;
    }
    return -1;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_write_rb_list_request()                       **
 **                                                                        **
 ** Description: Writes Radio Bearer list request to be sent to the NAS    **
 **		 driver.                                                   **
 **                                                                        **
 ** Inputs:	 mt_ix:		Mobile Terminal index                      **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	Always returns 0                           **
 ** 	 	 Others:	nas_rg_request                             **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_write_rb_list_request(int mt_ix)
{
    struct nas_rg_msg_rb_list_request* rb_list_req = &(nas_rg_request->tqalNASPrimitive.rb_list_req);

    nas_rg_request->type = NAS_RG_MSG_RB_LIST_REQUEST;
    nas_rg_request->length = sizeof(struct nas_rg_netl_hdr) + sizeof(struct nas_rg_msg_rb_list_request);

    /* Radio Bearer list is requested for MT unicast only */
    assert (mt_ix < RAL_MAX_MT);
    rb_list_req->ue_id = ralpriv->mt[mt_ix].ue_id;
    return 0;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_NAS_read_rb_list_reply()                          **
 **                                                                        **
 ** Description: Reads Radio Bearer list reply received from the NAS       **
 **		 driver.                                                   **
 **                                                                        **
 ** Inputs:	 None                                                      **
 ** 	 	 Others:	nas_rg_reply                               **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	0 if the message type is valid,            **
 **				-1 otherwise.                              **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ***************************************************************************/
static int eRALlte_NAS_read_rb_list_reply(void)
{
    if (nas_rg_reply->type == NAS_RG_MSG_RB_LIST_REPLY)
    {
	struct ral_lte_channel *currChannel;
	struct nas_rg_msg_rb_list_reply* rb_list_rep = &(nas_rg_reply->tqalNASPrimitive.rb_list_rep);

	int mt_ix = rb_list_rep->ue_id;
	int ch_ix;

	/* Radio Bearer list has been requested for MT unicast only */
	assert (mt_ix < RAL_MAX_MT);
	assert (mt_ix == ralpriv->mt[mt_ix].ue_id);

	/* Read Radio Bearer channel data */
	assert (rb_list_rep->num_rb <= NAS_RG_NETL_MAX_RABS);
	ralpriv->mt[mt_ix].num_rbs = rb_list_rep->num_rb;
	for (ch_ix = 0; ch_ix < ralpriv->mt[mt_ix].num_rbs; ch_ix++) {
	    currChannel = &ralpriv->mt[mt_ix].radio_channel[ch_ix];
	    currChannel->multicast = 0;
	    currChannel->rbId = rb_list_rep->RBList[ch_ix].rbId;
	    currChannel->RadioQoSclass = rb_list_rep->RBList[ch_ix].QoSclass;
	    currChannel->dscpUL = rb_list_rep->RBList[ch_ix].dscp; // TODO ???
	    currChannel->dscpDL = rb_list_rep->RBList[ch_ix].dscp; // TODO ???
	}
	return 0;
    }
    return -1;
}

/****************************************************************************
 **                                                                        **
 ** Name:	 imei2l2id()                                               **
 **                                                                        **
 ** Description: Converts IMEI to Layer 2 identifier                       **
 **                                                                        **
 ** Inputs:	 imei		IMEI identifier                            **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ** Outputs:	 l2id		Layer 2 identifier                         **
 **		 Return:	None                                       **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
static void imei2l2id(const uint8_t* imei, uint32_t* l2id)
{
    if ( !(imei) || !(l2id) ) {
	ERR(" %s : input parameter is NULL\n", __FUNCTION__);
	return;
    }
    l2id[0] = imei[0]+256 *(imei[1]+256*(imei[2]+256*(imei[3])));
    l2id[1] = imei[4]+256 *(imei[5]+256*(imei[6]+256*(imei[7])));
}

