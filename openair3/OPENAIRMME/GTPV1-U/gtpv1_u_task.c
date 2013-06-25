/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "mme_config.h"
#include "gtpv1_u.h"
#include "NwGtpv1u.h"
#include "NwGtpv1uMsg.h"
#include "NwLog.h"
#include "timer.h"
#include "intertask_interface.h"

#ifndef GTPU_DEBUG
# define GTPU_DEBUG(x, args...) do { fprintf(stdout, "[GTPU][D]"x, ##args); } \
    while(0)
#endif
#ifndef GTPU_ERROR
# define GTPU_ERROR(x, args...) do { fprintf(stdout, "[GTPU][E]"x, ##args); } \
    while(0)
#endif

static NwGtpv1uStackHandleT gtpv1u_stack = 0;
static pthread_t gtpv1u_task_thread;

static int gtpv1_u_send_init_udp(uint16_t port_number);
static int gtpv1u_create_s1u_tunnel(void);
static void* gtpv1u_thread(void *args);

NwGtpv1uRcT gtpv1_u_send_udp_msg(
    NwGtpv1uUdpHandleT udpHandle,
    NwU8T* dataBuf,
    NwU32T dataSize,
    NwU32T peerIpAddr,
    NwU32T peerPort);

NwGtpv1uRcT gtpv1_u_log_request(
    NwGtpv1uLogMgrHandleT hLogMgr,
    NwU32T logLevel,
    NwCharT* file,
    NwU32T line,
    NwCharT* logStr);

NwGtpv1uRcT gtpv1u_process_stack_req(
    NwGtpv1uUlpHandleT hUlp,
    NwGtpv1uUlpApiT *pUlpApi);

static int gtpv1_u_send_init_udp(uint16_t port_number) {
    // Create and alloc new message
    MessageDef *message_p;

    message_p = alloc_new_message(TASK_GTPV1_U, TASK_UDP, UDP_INIT);
    if (message_p == NULL) return -1;

    message_p->msg.udpInit.port = port_number;
    message_p->msg.udpInit.address = "0.0.0.0"; //ANY address

    return send_msg_to_task(TASK_UDP, message_p);
}

NwGtpv1uRcT gtpv1_u_log_request(NwGtpv1uLogMgrHandleT hLogMgr,
                                NwU32T logLevel,
                                NwCharT* file,
                                NwU32T line,
                                NwCharT* logStr) {

    GTPU_DEBUG("%s\n", logStr);
    return NW_GTPV1U_OK;
}

NwGtpv1uRcT gtpv1_u_send_udp_msg(
    NwGtpv1uUdpHandleT udpHandle,
    NwU8T* dataBuf,
    NwU32T dataSize,
    NwU32T peerIpAddr,
    NwU32T peerPort) {

    // Create and alloc new message
    MessageDef *message_p;
    UdpDataReq *udpDataReq_p;

    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = UDP_DATA_REQ;
    message_p->originTaskId = TASK_GTPV1_U;
    message_p->destinationTaskId = TASK_UDP;

    udpDataReq_p = &message_p->msg.udpDataReq;

    udpDataReq_p->peer_address  = peerIpAddr;
    udpDataReq_p->peer_port     = peerPort;
    udpDataReq_p->buffer        = dataBuf;
    udpDataReq_p->buffer_length = dataSize;

    return send_msg_to_task(TASK_UDP, message_p);
}

/* Callback called when a gtpv1_u message arrived on UDP interface */
NwGtpv1uRcT gtpv1u_process_stack_req(
    NwGtpv1uUlpHandleT hUlp,
    NwGtpv1uUlpApiT *pUlpApi) {

    switch(pUlpApi->apiType)
    {
        /* Here there are two type of messages handled:
         * - T-PDU
         * - END-MARKER
         */
        case NW_GTPV1U_ULP_API_RECV_TPDU:
        {
            NwU8T dataBuf[4096];
            NwU32T dataSize;
            if (NW_GTPV1U_OK != nwGtpv1uMsgGetTpdu(pUlpApi->apiInfo.recvMsgInfo.hMsg, dataBuf, &dataSize)) {
                GTPU_ERROR("Error while retrieving T-PDU");
            }
            GTPU_DEBUG("Received TPDU from gtpv1u stack %u!", pUlpApi->apiInfo.recvMsgInfo.teid);
        } break;
        default:
            GTPU_ERROR("Received undefined UlpApi (%02x) from gtpv1u stack!\n", pUlpApi->apiType);
    }
    return NW_GTPV1U_OK;
}

static int gtpv1u_create_s1u_tunnel(void) {
    /* Create a new nw-gtpv1-u stack req using API */
    NwGtpv1uUlpApiT stack_req;
    /* Local tunnel end-point identifier */
    static uint32_t s1u_teid = 1;
    int ret = 0;

    MessageDef *message_p;

    memset(&stack_req, 0, sizeof(NwGtpv1uUlpApiT));

    stack_req.apiType = NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT;
    stack_req.apiInfo.createTunnelEndPointInfo.teid          = s1u_teid;
    stack_req.apiInfo.createTunnelEndPointInfo.hUlpSession   = 0;
    stack_req.apiInfo.createTunnelEndPointInfo.hStackSession = 0;

    if (nwGtpv1uProcessUlpReq(gtpv1u_stack, &stack_req) != NW_GTPV1U_OK) {
        ret = -1;
    }

    message_p = alloc_new_message(TASK_GTPV1_U, TASK_SGW_LITE, SGW_S1U_ENDPOINT_CREATED);

    message_p->msg.sgwS1UEndpointCreated.teid = s1u_teid;
    message_p->msg.sgwS1UEndpointCreated.status = ret;

    ret = send_msg_to_task(TASK_SGW_LITE, message_p);

    s1u_teid++;
    return ret;
}

static NwGtpv1uRcT gtpv1u_start_timer_wrapper(
    NwGtpv1uTimerMgrHandleT tmrMgrHandle,
    NwU32T                  timeoutSec,
    NwU32T                  timeoutUsec,
    NwU32T                  tmrType,
    void*                   timeoutArg,
    NwGtpv1uTimerHandleT*   hTmr) {

    NwGtpv1uRcT rc = NW_GTPV1U_OK;
    long        timer_id;

    timer_setup(timeoutSec, timeoutUsec, TASK_GTPV1_U, TIMER_ONE_SHOT, &timer_id);

    return rc;
}

static NwGtpv1uRcT gtpv1u_stop_timer_wrapper(
    NwGtpv1uTimerMgrHandleT tmrMgrHandle,
    NwGtpv1uTimerHandleT hTmr) {

    NwGtpv1uRcT rc = NW_GTPV1U_OK;

    return rc;
}

static void* gtpv1u_thread(void *args) {
    while(1) {
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        MessageDef *received_message_p;
        receive_msg(TASK_GTPV1_U, &received_message_p);
        assert(received_message_p != NULL);
        switch(received_message_p->messageId) {
            case S1U_CREATE_TUNNEL:
            {
                long timer_id;

                gtpv1u_create_s1u_tunnel();
            } break;
            case GTPV1U_DATA_REQ:
            {
                Gtpv1uDataReq *gtpv1u_data_req_p;
                gtpv1u_data_req_p = &received_message_p->msg.gtpv1uDataReq;
                nwGtpv1uProcessUdpReq(gtpv1u_stack, gtpv1u_data_req_p->buffer,
                                      gtpv1u_data_req_p->buffer_length,
                                      gtpv1u_data_req_p->port,
                                      gtpv1u_data_req_p->peer_addr);
                free(gtpv1u_data_req_p->buffer);
            } break;
            default:
            {
                GTPU_ERROR("Unkwnon message ID %d\n", received_message_p->messageId);
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int gtpv1_u_init(const mme_config_t *mme_config) {
    int                     ret;
    NwGtpv1uRcT             rc;
    NwGtpv1uUlpEntityT      ulp;
    NwGtpv1uUdpEntityT      udp;
    NwGtpv1uLogMgrEntityT   log;
    NwGtpv1uTimerMgrEntityT tmr;

    GTPU_DEBUG("Initializing GTPV1U interface\n");

    ret = gtpv1_u_send_init_udp(mme_config->gtpv1_u_config.port_number);
    if (ret < 0) {
        return ret;
    }

    /* Initializing GTPv1-U stack */
    if ((rc = nwGtpv1uInitialize(&gtpv1u_stack)) != NW_GTPV1U_OK) {
        GTPU_ERROR("Failed to setup nwGtpv1u stack %x\n", rc);
        return -1;
    }

    if ((rc = nwGtpv1uSetLogLevel(gtpv1u_stack, NW_LOG_LEVEL_DEBG)) != NW_GTPV1U_OK) {
        GTPU_ERROR("Failed to setup loglevel for stack %x\n", rc);
        return -1;
    }

    /* Set the ULP API callback. Called once message have been processed by the
     * nw-gtpv1u stack.
     */
    ulp.ulpReqCallback = gtpv1u_process_stack_req;

    if ((rc = nwGtpv1uSetUlpEntity(gtpv1u_stack, &ulp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUlpEntity: %x", rc);
        return -1;
    }

    /* nw-gtpv1u stack requires an udp callback to send data over UDP.
     * We provide a wrapper to UDP task.
     */
    udp.udpDataReqCallback = gtpv1_u_send_udp_msg;

    if ((rc = nwGtpv1uSetUdpEntity(gtpv1u_stack, &udp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUdpEntity: %x", rc);
        return -1;
    }

    log.logReqCallback = gtpv1_u_log_request;

    if ((rc = nwGtpv1uSetLogMgrEntity(gtpv1u_stack, &log)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetLogMgrEntity: %x", rc);
        return -1;
    }

    /* Timer interface is more complicated as both wrappers doesn't send a message
     * to the timer task but call the timer API functions start/stop timer.
     */
    tmr.tmrMgrHandle     = 0;
    tmr.tmrStartCallback = gtpv1u_start_timer_wrapper;
    tmr.tmrStopCallback  = gtpv1u_stop_timer_wrapper;

    if ((rc = nwGtpv1uSetTimerMgrEntity(gtpv1u_stack, &tmr)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetTimerMgrEntity: %x", rc);
        return -1;
    }

    if (pthread_create(&gtpv1u_task_thread, NULL, &gtpv1u_thread, NULL) < 0) {
        GTPU_ERROR("s1ap phtread_create: %s", strerror(errno));
        return -1;
    }

    GTPU_DEBUG("Initializing GTPV1U interface: DONE\n");

    return ret;
}
