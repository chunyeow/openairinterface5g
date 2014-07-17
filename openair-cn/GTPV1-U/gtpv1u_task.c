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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
/*! \file gtpv1u_task.c
* \brief
* \author Sebastien ROUX, Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "mme_config.h"

#include "assertions.h"
#include "intertask_interface.h"
#include "timer.h"

#include "gtpv1u.h"
#include "NwGtpv1u.h"
#include "NwGtpv1uMsg.h"
#include "NwLog.h"
#include "gtpv1u_sgw_defs.h"

//static NwGtpv1uStackHandleT gtpv1u_stack = 0;
static gtpv1u_data_t        gtpv1u_sgw_data;

static int gtpv1u_send_init_udp(uint16_t port_number);
static int gtpv1u_create_s1u_tunnel(Gtpv1uCreateTunnelReq *create_tunnel_reqP);
static int gtpv1u_delete_s1u_tunnel(Teid_t context_teidP, Teid_t S1U_teidP);
static int gtpv1u_update_s1u_tunnel(Gtpv1uUpdateTunnelReq *reqP);
static void *gtpv1u_thread(void *args);

NwGtpv1uRcT gtpv1u_send_udp_msg(
    NwGtpv1uUdpHandleT udpHandle,
    NwU8T *buffer,
    NwU32T buffer_len,
    NwU32T peerIpAddr,
    NwU32T peerPort);

NwGtpv1uRcT gtpv1u_log_request(
    NwGtpv1uLogMgrHandleT hLogMgr,
    NwU32T logLevel,
    NwCharT *file,
    NwU32T line,
    NwCharT *logStr);

NwGtpv1uRcT gtpv1u_process_stack_req(
    NwGtpv1uUlpHandleT hUlp,
    NwGtpv1uUlpApiT *pUlpApi);

static int gtpv1u_send_init_udp(uint16_t port_number)
{
    // Create and alloc new message
    MessageDef *message_p;
    struct in_addr addr;

    message_p = itti_alloc_new_message(TASK_GTPV1_U, UDP_INIT);
    if (message_p == NULL) {
        return -1;
    }

    UDP_INIT(message_p).port = port_number;
    //LG UDP_INIT(message_p).address = "0.0.0.0"; //ANY address

    addr.s_addr = gtpv1u_sgw_data.sgw_ip_address_for_S1u_S12_S4_up;
    UDP_INIT(message_p).address = inet_ntoa(addr);
    GTPU_DEBUG("Tx UDP_INIT IP addr %s\n", UDP_INIT(message_p).address);

    return itti_send_msg_to_task(TASK_UDP, INSTANCE_DEFAULT, message_p);
}

NwGtpv1uRcT gtpv1u_log_request(NwGtpv1uLogMgrHandleT hLogMgr,
                               NwU32T logLevel,
                               NwCharT *file,
                               NwU32T line,
                               NwCharT *logStr)
{
    GTPU_DEBUG("%s\n", logStr);
    return NW_GTPV1U_OK;
}

NwGtpv1uRcT gtpv1u_send_udp_msg(
    NwGtpv1uUdpHandleT udpHandle,
    NwU8T *buffer,
    NwU32T buffer_len,
    NwU32T peerIpAddr,
    NwU32T peerPort)
{
    // Create and alloc new message
    MessageDef     *message_p;
    udp_data_req_t *udp_data_req_p;

    message_p = itti_alloc_new_message(TASK_GTPV1_U, UDP_DATA_REQ);

    udp_data_req_p = &message_p->ittiMsg.udp_data_req;

    udp_data_req_p->peer_address  = peerIpAddr;
    udp_data_req_p->peer_port     = peerPort;
    udp_data_req_p->buffer        = buffer;
    udp_data_req_p->buffer_length = buffer_len;

    return itti_send_msg_to_task(TASK_UDP, INSTANCE_DEFAULT, message_p);
}

/* Callback called when a gtpv1u message arrived on UDP interface */
NwGtpv1uRcT gtpv1u_process_stack_req(
    NwGtpv1uUlpHandleT hUlp,
    NwGtpv1uUlpApiT *pUlpApi)
{
    switch(pUlpApi->apiType) {
            /* Here there are two type of messages handled:
             * - T-PDU
             * - END-MARKER
             */
        case NW_GTPV1U_ULP_API_RECV_TPDU: {
            uint8_t              buffer[4096];
            uint32_t             buffer_len;
            MessageDef          *message_p;
            Gtpv1uTunnelDataInd *data_ind_p;

            /* Nw-gptv1u stack has processed a PDU. we can forward it to IPV4
             * task for transmission.
             */
            if (NW_GTPV1U_OK != nwGtpv1uMsgGetTpdu(pUlpApi->apiInfo.recvMsgInfo.hMsg,
                buffer, (NwU32T *)&buffer_len)) {
                GTPU_ERROR("Error while retrieving T-PDU");
            }
            GTPU_DEBUG("Received TPDU from gtpv1u stack %u with size %d", pUlpApi->apiInfo.recvMsgInfo.teid, buffer_len);

            message_p = itti_alloc_new_message(TASK_GTPV1_U, GTPV1U_TUNNEL_DATA_IND);
            if (message_p == NULL) {
                return -1;
            }
            data_ind_p                       = &message_p->ittiMsg.gtpv1uTunnelDataInd;
            data_ind_p->buffer               = malloc(sizeof(uint8_t) * buffer_len);
            data_ind_p->local_S1u_teid       = pUlpApi->apiInfo.recvMsgInfo.teid;
            if (data_ind_p->buffer == NULL) {
                GTPU_ERROR("Failed to allocate new buffer\n");
                itti_free(ITTI_MSG_ORIGIN_ID(message_p), message_p);
                message_p = NULL;
            } else {
                memcpy(data_ind_p->buffer, buffer, buffer_len);
                data_ind_p->length = buffer_len;
                if (itti_send_msg_to_task(TASK_FW_IP, INSTANCE_DEFAULT, message_p) < 0) {
                    GTPU_ERROR("Failed to send message to task\n");
                    itti_free(ITTI_MSG_ORIGIN_ID(message_p), message_p);
                    message_p = NULL;
                }
            }
        }
        break;

        case NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT:{
        }
        break;

        default: {
            GTPU_ERROR("Received undefined UlpApi (%02x) from gtpv1u stack!\n",
                       pUlpApi->apiType);
        }
    }
    return NW_GTPV1U_OK;
}

static int gtpv1u_create_s1u_tunnel(Gtpv1uCreateTunnelReq *create_tunnel_reqP)
{
    /* Create a new nw-gtpv1-u stack req using API */
    NwGtpv1uUlpApiT          stack_req;
    NwGtpv1uRcT              rc;
    /* Local tunnel end-point identifier */
    uint32_t                 s1u_teid             = 0;
    gtpv1u_teid2enb_info_t  *gtpv1u_teid2enb_info = NULL;
    MessageDef              *message_p            = NULL;
    hashtable_rc_t           hash_rc;

    GTPU_DEBUG("Rx GTPV1U_CREATE_TUNNEL_REQ Context %d\n", create_tunnel_reqP->context_teid);
    memset(&stack_req, 0, sizeof(NwGtpv1uUlpApiT));

    stack_req.apiType = NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT;

    do {
        s1u_teid = gtpv1u_new_teid();
        GTPU_DEBUG("gtpv1u_create_s1u_tunnel() %u\n", s1u_teid);
        stack_req.apiInfo.createTunnelEndPointInfo.teid          = s1u_teid;
        stack_req.apiInfo.createTunnelEndPointInfo.hUlpSession   = 0;
        stack_req.apiInfo.createTunnelEndPointInfo.hStackSession = 0;

        rc = nwGtpv1uProcessUlpReq(gtpv1u_sgw_data.gtpv1u_stack, &stack_req);
        GTPU_DEBUG(".\n");
    } while (rc != NW_GTPV1U_OK);

    gtpv1u_teid2enb_info = malloc (sizeof(gtpv1u_teid2enb_info_t));
    memset(gtpv1u_teid2enb_info, 0, sizeof(gtpv1u_teid2enb_info_t));
    gtpv1u_teid2enb_info->state       = BEARER_IN_CONFIG;

//#warning !!! hack because missing modify session request, so force enb address
//    gtpv1u_teid2enb_info->enb_ip_addr.pdn_type = IPv4;
//    gtpv1u_teid2enb_info->enb_ip_addr.address.ipv4_address[0] = 192;
//    gtpv1u_teid2enb_info->enb_ip_addr.address.ipv4_address[1] = 168;
//    gtpv1u_teid2enb_info->enb_ip_addr.address.ipv4_address[2] = 1;
//    gtpv1u_teid2enb_info->enb_ip_addr.address.ipv4_address[3] = 2;


    message_p = itti_alloc_new_message(TASK_GTPV1_U, GTPV1U_CREATE_TUNNEL_RESP);
    message_p->ittiMsg.gtpv1uCreateTunnelResp.S1u_teid      = s1u_teid;
    message_p->ittiMsg.gtpv1uCreateTunnelResp.context_teid  = create_tunnel_reqP->context_teid;
    message_p->ittiMsg.gtpv1uCreateTunnelResp.eps_bearer_id = create_tunnel_reqP->eps_bearer_id;

    hash_rc = hashtable_is_key_exists(gtpv1u_sgw_data.S1U_mapping, s1u_teid);

    if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
        hash_rc = hashtable_insert(gtpv1u_sgw_data.S1U_mapping, s1u_teid, gtpv1u_teid2enb_info);
        message_p->ittiMsg.gtpv1uCreateTunnelResp.status       = 0;
    } else {
        message_p->ittiMsg.gtpv1uCreateTunnelResp.status       = 0xFF;
    }

    GTPU_DEBUG("Tx GTPV1U_CREATE_TUNNEL_RESP Context %u teid %u eps bearer id %u status %d\n",
                message_p->ittiMsg.gtpv1uCreateTunnelResp.context_teid,
                message_p->ittiMsg.gtpv1uCreateTunnelResp.S1u_teid,
                message_p->ittiMsg.gtpv1uCreateTunnelResp.eps_bearer_id,
                message_p->ittiMsg.gtpv1uCreateTunnelResp.status);
    return itti_send_msg_to_task(TASK_SPGW_APP, INSTANCE_DEFAULT, message_p);
}



static int gtpv1u_delete_s1u_tunnel(Teid_t context_teidP, Teid_t S1U_teidP)
{
    /* Local tunnel end-point identifier */
    MessageDef              *message_p;

    GTPU_DEBUG("Rx GTPV1U_DELETE_TUNNEL Context %u S1U teid %u\n", context_teidP, S1U_teidP);
    message_p = itti_alloc_new_message(TASK_GTPV1_U, GTPV1U_DELETE_TUNNEL_RESP);

    message_p->ittiMsg.gtpv1uDeleteTunnelResp.S1u_teid     = S1U_teidP;
    message_p->ittiMsg.gtpv1uDeleteTunnelResp.context_teid = context_teidP;

    if (hashtable_remove(gtpv1u_sgw_data.S1U_mapping, S1U_teidP) == HASH_TABLE_OK ) {
        message_p->ittiMsg.gtpv1uDeleteTunnelResp.status       = 0;
    } else {
        message_p->ittiMsg.gtpv1uDeleteTunnelResp.status       = -1;
    }
    GTPU_DEBUG("Tx SGW_S1U_ENDPOINT_CREATED Context %u teid %u status %d\n", context_teidP, S1U_teidP, message_p->ittiMsg.gtpv1uDeleteTunnelResp.status);
    return itti_send_msg_to_task(TASK_SPGW_APP, INSTANCE_DEFAULT, message_p);
}


static int gtpv1u_update_s1u_tunnel(Gtpv1uUpdateTunnelReq *reqP)
{
    hashtable_rc_t           hash_rc;
    gtpv1u_teid2enb_info_t  *gtpv1u_teid2enb_info;
    MessageDef              *message_p;

    GTPU_DEBUG("Rx GTPV1U_UPDATE_TUNNEL_REQ Context %u, S-GW S1U teid %u, eNB S1U teid %u\n",
        reqP->context_teid,
        reqP->sgw_S1u_teid,
        reqP->enb_S1u_teid);
    message_p = itti_alloc_new_message(TASK_GTPV1_U, GTPV1U_UPDATE_TUNNEL_RESP);

    hash_rc = hashtable_get(gtpv1u_sgw_data.S1U_mapping, reqP->sgw_S1u_teid, (void**)&gtpv1u_teid2enb_info);

    if (hash_rc == HASH_TABLE_OK) {
        gtpv1u_teid2enb_info->teid_enb    = reqP->enb_S1u_teid;
        gtpv1u_teid2enb_info->enb_ip_addr = reqP->enb_ip_address_for_S1u;
        gtpv1u_teid2enb_info->state       = BEARER_UP;
        gtpv1u_teid2enb_info->port        = GTPV1U_UDP_PORT;
        message_p->ittiMsg.gtpv1uUpdateTunnelResp.status = 0;           ///< Status (Failed = 0xFF or Success = 0x0)
    } else {
        GTPU_ERROR("Mapping not found\n");
        message_p->ittiMsg.gtpv1uUpdateTunnelResp.status = 0xFF;           ///< Status (Failed = 0xFF or Success = 0x0)
    }
    message_p->ittiMsg.gtpv1uUpdateTunnelResp.context_teid  = reqP->context_teid;
    message_p->ittiMsg.gtpv1uUpdateTunnelResp.sgw_S1u_teid  = reqP->sgw_S1u_teid;
    message_p->ittiMsg.gtpv1uUpdateTunnelResp.enb_S1u_teid  = reqP->enb_S1u_teid;
    message_p->ittiMsg.gtpv1uUpdateTunnelResp.eps_bearer_id = reqP->eps_bearer_id;
    return itti_send_msg_to_task(TASK_SPGW_APP, INSTANCE_DEFAULT, message_p);
}


static NwGtpv1uRcT gtpv1u_start_timer_wrapper(
    NwGtpv1uTimerMgrHandleT tmrMgrHandle,
    NwU32T                  timeoutSec,
    NwU32T                  timeoutUsec,
    NwU32T                  tmrType,
    void                   *timeoutArg,
    NwGtpv1uTimerHandleT   *hTmr)
{

    NwGtpv1uRcT rc = NW_GTPV1U_OK;
    long        timer_id;

    if (tmrType == NW_GTPV1U_TMR_TYPE_ONE_SHOT) {
        timer_setup(timeoutSec,
                    timeoutUsec,
                    TASK_GTPV1_U,
                    INSTANCE_DEFAULT,
                    TIMER_ONE_SHOT,
                    timeoutArg,
                    &timer_id);
    } else {
        timer_setup(timeoutSec,
                    timeoutUsec,
                    TASK_GTPV1_U,
                    INSTANCE_DEFAULT,
                    TIMER_PERIODIC,
                    timeoutArg,
                    &timer_id);
    }

    return rc;
}

static NwGtpv1uRcT gtpv1u_stop_timer_wrapper(
    NwGtpv1uTimerMgrHandleT tmrMgrHandle,
    NwGtpv1uTimerHandleT hTmr)
{

    NwGtpv1uRcT rc = NW_GTPV1U_OK;

    return rc;
}

static void *gtpv1u_thread(void *args)
{
    itti_mark_task_ready(TASK_GTPV1_U);
    while(1) {
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        MessageDef *received_message_p = NULL;
        itti_receive_msg(TASK_GTPV1_U, &received_message_p);
        DevAssert(received_message_p != NULL);

        switch (ITTI_MSG_ID(received_message_p))
        {
            case GTPV1U_CREATE_TUNNEL_REQ: {
                gtpv1u_create_s1u_tunnel(&received_message_p->ittiMsg.gtpv1uCreateTunnelReq);
            }
            break;

            case GTPV1U_DELETE_TUNNEL_REQ: {
                gtpv1u_delete_s1u_tunnel(received_message_p->ittiMsg.gtpv1uDeleteTunnelReq.context_teid, received_message_p->ittiMsg.gtpv1uDeleteTunnelReq.S1u_teid);
            }
            break;

            case GTPV1U_UPDATE_TUNNEL_REQ: {
                gtpv1u_update_s1u_tunnel(&received_message_p->ittiMsg.gtpv1uUpdateTunnelReq);
            }
            break;

            // DATA COMING FROM UDP
            case UDP_DATA_IND: {
                udp_data_ind_t *udp_data_ind_p;
                udp_data_ind_p = &received_message_p->ittiMsg.udp_data_ind;
                nwGtpv1uProcessUdpReq(gtpv1u_sgw_data.gtpv1u_stack,
                                      udp_data_ind_p->buffer,
                                      udp_data_ind_p->buffer_length,
                                      udp_data_ind_p->peer_port,
                                      udp_data_ind_p->peer_address);
                free(udp_data_ind_p->buffer);
            }
            break;

            // DATA TO BE SENT TO UDP
            case GTPV1U_TUNNEL_DATA_REQ: {
                Gtpv1uTunnelDataReq     *data_req_p = NULL;
                NwGtpv1uUlpApiT          stack_req;
                NwGtpv1uRcT              rc;
                hashtable_rc_t           hash_rc;
                gtpv1u_teid2enb_info_t  *gtpv1u_teid2enb_info = NULL;

                data_req_p = &received_message_p->ittiMsg.gtpv1uTunnelDataReq;
                //ipv4_send_data(ipv4_data_p->sd, data_ind_p->buffer, data_ind_p->length);

                memset(&stack_req, 0, sizeof(NwGtpv1uUlpApiT));

                /*
                 * typedef struct
                {
                NW_IN    NwU32T                       teid;
                NW_IN    NwU32T                       ipAddr;
                NW_IN    NwU8T                        flags;
                NW_IN    NwGtpv1uMsgHandleT           hMsg;
                } NwGtpv1uSendtoInfoT;*/

                hash_rc = hashtable_get(gtpv1u_sgw_data.S1U_mapping, (uint64_t)data_req_p->local_S1u_teid, (void**)&gtpv1u_teid2enb_info);

                if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
                    GTPU_ERROR("nwGtpv1uProcessUlpReq failed: while getting teid %u in hashtable S1U_mapping\n", data_req_p->local_S1u_teid);
                } else {
                    stack_req.apiType                   = NW_GTPV1U_ULP_API_SEND_TPDU;
                    stack_req.apiInfo.sendtoInfo.teid   = data_req_p->local_S1u_teid;
                    BUFFER_TO_NwU32T(gtpv1u_teid2enb_info->enb_ip_addr.address.ipv4_address, stack_req.apiInfo.sendtoInfo.ipAddr);

       /*nwGtpv1uGpduMsgNew( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                    NW_IN NwU32T    teid,
                    NW_IN NwU8T     seqNumFlag,
                    NW_IN NwU16T    seqNum,
                    NW_IN NwU8T    *tpdu,
                    NW_IN NwU16T    tpduLength,
                    NW_OUT NwGtpv1uMsgHandleT *phMsg)*/
                    rc = nwGtpv1uGpduMsgNew(gtpv1u_sgw_data.gtpv1u_stack,
                                        00,// TO DO bearer_p->port, but not needed when looking at processing
                                        NW_FALSE,
                                        gtpv1u_sgw_data.seq_num++,
                                        data_req_p->buffer,
                                        data_req_p->length,
                                        &(stack_req.apiInfo.sendtoInfo.hMsg));

                    if (rc != NW_GTPV1U_OK) {
                        GTPU_ERROR("nwGtpv1uGpduMsgNew failed: 0x%x\n", rc);
                    } else {
                        rc = nwGtpv1uProcessUlpReq(gtpv1u_sgw_data.gtpv1u_stack, &stack_req);
                        if (rc != NW_GTPV1U_OK) {
                            GTPU_ERROR("nwGtpv1uProcessUlpReq failed: 0x%x\n", rc);
                        }
                        rc = nwGtpv1uMsgDelete(gtpv1u_sgw_data.gtpv1u_stack,
                                    stack_req.apiInfo.sendtoInfo.hMsg);
                        if (rc != NW_GTPV1U_OK) {
                            GTPU_ERROR("nwGtpv1uMsgDelete failed: 0x%x\n", rc);
                        }
                    }
                }
                /* Buffer is no longer needed, free it */
                free(data_req_p->buffer);
            }
            break;
            case TERMINATE_MESSAGE: {
                itti_exit_task();
            } break;
            case TIMER_HAS_EXPIRED:
                nwGtpv1uProcessTimeout(&received_message_p->ittiMsg.timer_has_expired.arg);
                break;
            default: {
                GTPU_ERROR("Unkwnon message ID %d:%s\n",
                           ITTI_MSG_ID(received_message_p),
                           ITTI_MSG_NAME(received_message_p));
            }
            break;
        }
        itti_free(ITTI_MSG_ORIGIN_ID(received_message_p), received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int gtpv1u_init(const mme_config_t *mme_config_p)
{
    int                     ret;
    NwGtpv1uRcT             rc;
    NwGtpv1uUlpEntityT      ulp;
    NwGtpv1uUdpEntityT      udp;
    NwGtpv1uLogMgrEntityT   log;
    NwGtpv1uTimerMgrEntityT tmr;

    GTPU_DEBUG("Initializing GTPV1U interface\n");

    memset(&gtpv1u_sgw_data, 0, sizeof(gtpv1u_sgw_data));

    gtpv1u_sgw_data.S1U_mapping = hashtable_create (8192, NULL, NULL);
    if (gtpv1u_sgw_data.S1U_mapping == NULL) {
        perror("hashtable_create");
        GTPU_ERROR("Initializing TASK_GTPV1_U task interface: ERROR\n");
        return -1;
    }

    gtpv1u_sgw_data.sgw_ip_address_for_S1u_S12_S4_up = mme_config_p->ipv4.sgw_ip_address_for_S1u_S12_S4_up;

    /* Initializing GTPv1-U stack */
    if ((rc = nwGtpv1uInitialize(&gtpv1u_sgw_data.gtpv1u_stack)) != NW_GTPV1U_OK) {
        GTPU_ERROR("Failed to setup nwGtpv1u stack %x\n", rc);
        return -1;
    }

    if ((rc = nwGtpv1uSetLogLevel(gtpv1u_sgw_data.gtpv1u_stack,
                                  NW_LOG_LEVEL_DEBG)) != NW_GTPV1U_OK) {
        GTPU_ERROR("Failed to setup loglevel for stack %x\n", rc);
        return -1;
    }

    /* Set the ULP API callback. Called once message have been processed by the
     * nw-gtpv1u stack.
     */
    ulp.ulpReqCallback = gtpv1u_process_stack_req;

    if ((rc = nwGtpv1uSetUlpEntity(gtpv1u_sgw_data.gtpv1u_stack, &ulp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUlpEntity: %x", rc);
        return -1;
    }

    /* nw-gtpv1u stack requires an udp callback to send data over UDP.
     * We provide a wrapper to UDP task.
     */
    udp.udpDataReqCallback = gtpv1u_send_udp_msg;

    if ((rc = nwGtpv1uSetUdpEntity(gtpv1u_sgw_data.gtpv1u_stack, &udp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUdpEntity: %x", rc);
        return -1;
    }

    log.logReqCallback = gtpv1u_log_request;

    if ((rc = nwGtpv1uSetLogMgrEntity(gtpv1u_sgw_data.gtpv1u_stack, &log)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetLogMgrEntity: %x", rc);
        return -1;
    }

    /* Timer interface is more complicated as both wrappers doesn't send a message
     * to the timer task but call the timer API functions start/stop timer.
     */
    tmr.tmrMgrHandle     = 0;
    tmr.tmrStartCallback = gtpv1u_start_timer_wrapper;
    tmr.tmrStopCallback  = gtpv1u_stop_timer_wrapper;

    if ((rc = nwGtpv1uSetTimerMgrEntity(gtpv1u_sgw_data.gtpv1u_stack, &tmr)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetTimerMgrEntity: %x", rc);
        return -1;
    }

    if (itti_create_task(TASK_GTPV1_U, &gtpv1u_thread, NULL) < 0) {
        GTPU_ERROR("gtpv1u phtread_create: %s", strerror(errno));
        return -1;
    }

    ret = gtpv1u_send_init_udp(mme_config_p->gtpv1u_config.port_number);
    if (ret < 0) {
        return ret;
    }

    GTPU_DEBUG("Initializing GTPV1U interface: DONE\n");

    return ret;
}
