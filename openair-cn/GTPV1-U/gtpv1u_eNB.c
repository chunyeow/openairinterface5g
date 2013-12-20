#include <stdio.h>
#include <errno.h>

#include "assertions.h"
#include "NwGtpv1u.h"
#include "NwGtpv1uMsg.h"
#include "NwLog.h"

#include "log.h"

#include "gtpv1u_eNB_defs.h"

#include "udp_primitives_client.h"
#include "UTIL/LOG/log.h"
#include "COMMON/platform_types.h"

#ifdef GTPU_IN_KERNEL
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

extern unsigned char pdcp_data_req(u8 eNB_id, u8 UE_id, u32_t frame, u8_t eNB_flag, rb_id_t rb_id, u32 muiP, u32 confirmP, \
    sdu_size_t sdu_buffer_size, unsigned char* sdu_buffer, u8 mode);


inline NwGtpv1uRcT gtpv1u_eNB_log_request(NwGtpv1uLogMgrHandleT hLogMgr,
        NwU32T                logLevel,
        NwCharT              *file,
        NwU32T                line,
        NwCharT              *logStr)
{
    logIt(GTPU, logLevel, "%s\n", logStr);
    return NW_GTPV1U_OK;
}

/* Callback called when a gtpv1u message arrived on UDP interface */
NwGtpv1uRcT gtpv1u_process_stack_req(
    NwGtpv1uUlpHandleT hUlp,
    NwGtpv1uUlpApiT *pUlpApi)
{
    int result;

    switch(pUlpApi->apiType) {
            /* Here there are two type of messages handled:
             * - T-PDU
             * - END-MARKER
             */
        case NW_GTPV1U_ULP_API_RECV_TPDU: {
            uint8_t      buffer[4096];
            uint32_t     buffer_len;

            /* Nw-gptv1u stack has processed a PDU. we can schedule it to PDCP
             * for transmission.
             */
            if (NW_GTPV1U_OK != nwGtpv1uMsgGetTpdu(pUlpApi->apiInfo.recvMsgInfo.hMsg,
                                                   buffer, &buffer_len)) {
                GTPU_ERROR("Error while retrieving T-PDU");
            }
            GTPU_DEBUG("Received T-PDU from gtpv1u stack %u with size %d",
                       pUlpApi->apiInfo.recvMsgInfo.teid, buffer_len);

            result = pdcp_data_req(0, // eNB_idx,
                                   0, // UE idx
                                   0, // frame
                                   1, // enb flag
                                   5, // rb id
                                   0, // mui
                                   0, // confirm
                                   buffer_len,
                                   buffer,
                                   1);
            AssertFatal (result == TRUE, "PDCP data request failed!\n");
        }
        break;
        default: {
            GTPU_ERROR("Received undefined UlpApi (%02x) from gtpv1u stack!\n",
                       pUlpApi->apiType);
        }
    }
    return NW_GTPV1U_OK;
}

NwGtpv1uRcT gtpv1u_eNB_udp_req(NwGtpv1uUdpHandleT udpHandle,
                               NwU8T             *dataBuf,
                               NwU32T             dataSize,
                               NwU32T             peerIpAddr,
                               NwU16T             peerPort)
{
    udp_data_t *udp_data_p;

    if (udpHandle == 0) {
        return NW_GTPV1U_FAILURE;
    }

    udp_data_p = (udp_data_t *)udpHandle;

    LOG_D(GTPU, "New udp req triggered with sd %d, data size %u\n",
          udp_data_p->sd, dataSize);

    if (udp_send_to(udp_data_p->sd, peerPort, peerIpAddr, dataBuf, dataSize) < 0) {
        return NW_GTPV1U_FAILURE;
    }

    return NW_GTPV1U_OK;
}

int data_recv_callback(uint16_t  port,
                       uint32_t  address,
                       uint8_t  *buffer,
                       uint32_t  length,
                       void     *arg_p)
{
    gtpv1u_data_t        *gtpv1u_data_p;

    if (arg_p == NULL) {
        return -1;
    }

    gtpv1u_data_p = (gtpv1u_data_t *)arg_p;

    return nwGtpv1uProcessUdpReq(gtpv1u_data_p->gtpv1u_stack,
    		buffer,
    		length,
    		port,
    		address);
}

int gtpv1u_create_tunnel_endpoint(gtpv1u_data_t *gtpv1u_data_p, uint8_t ue_id,
                                  uint8_t rab_id, char *sgw_ip_addr, uint16_t port)
{
    uint32_t                     teid;
    uint8_t                      max_attempt = 100;
    NwGtpv1uRcT                  rc;
    NwGtpv1uUlpApiT              ulp_req;
    struct gtpv1u_ue_data_s     *new_ue_p;
    struct gtpv1u_ue_data_s     *temp;
    struct gtpv1u_bearer_s      *bearer;
    hashtbl_rc_t                 hash_rc;

    if (rab_id > MAX_BEARERS_PER_UE) {
        LOG_E(GTPU, "Could not use rab_id %d > max %d\n",
              rab_id, MAX_BEARERS_PER_UE);
        return -1;
    }


    if ((hash_rc = hashtbl_get(gtpv1u_data_p->ue_mapping, (uint64_t)ue_id, (void**)&new_ue_p)) == HASH_TABLE_OK) {
        /* A context for this UE already exist in the tree, use it */
        /* We check that the tunnel is not already configured */
        if (new_ue_p->bearers[rab_id].state != BEARER_DOWN) {
            LOG_E(GTPU, "Cannot create new end-point over already existing tunnel\n");
            return -1;
        }
    } else {
        /* Context doesn't exist, create it */
        if (rab_id != 0) {
            /* UE should first establish Default bearer before trying to setup
             * additional bearers.
             */
            LOG_E(GTPU, "UE context is not known and rab_id != 0\n");
            return -1;
        }
        new_ue_p = calloc(1, sizeof(struct gtpv1u_ue_data_s));
        new_ue_p->ue_id = ue_id;

        hash_rc = hashtbl_insert(gtpv1u_data_p->ue_mapping, (uint64_t)ue_id, new_ue_p);

        if ((hash_rc != HASH_TABLE_OK) && (hash_rc != HASH_TABLE_INSERT_OVERWRITTEN_DATA)) {
            LOG_E(GTPU, "Failed to insert new UE context\n");
            free(new_ue_p);
            return -1;
        }
    }

    bearer = &new_ue_p->bearers[rab_id];

    /* Configure the bearer */
    bearer->state       = BEARER_IN_CONFIG;
    bearer->sgw_ip_addr = inet_addr(sgw_ip_addr);
    bearer->port        = port;

    /* Create the new stack api request */
    memset(&ulp_req, 0, sizeof(NwGtpv1uUlpApiT));
    ulp_req.apiType = NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT;

    /* Try to create new tunnel-endpoint.
     * If teid generated is already present in the stack, just peek another random
     * teid. This could be ok for small number of tunnel but more errors could be
     * thrown if we reached high number of tunnels.
     * TODO: find a solution for teid
     */
    do {
        /* Request for a new random TEID */
        teid = gtpv1u_new_teid();
        ulp_req.apiInfo.createTunnelEndPointInfo.teid = teid;

        rc = nwGtpv1uProcessUlpReq(gtpv1u_data_p->gtpv1u_stack, &ulp_req);

        if (rc == NW_GTPV1U_OK) {
//             LOG_D(GTPU, "Successfully created new tunnel endpoint for teid 0x%x\n",
//                   teid);
            bearer->teid_eNB = teid;
//             gtpv1u_initial_req(gtpv1u_data_p, teid, GTPV1U_UDP_PORT,
//                                inet_addr("192.168.56.101"));
            LOG_I(GTPU, "Created eNB tunnel endpoint %u for ue id %u, rab id %u\n", teid, ue_id, rab_id);
            return 0;
        } else {
            LOG_W(GTPU, "Teid %u already in use... %s\n",
                  teid, (max_attempt > 1) ? "Trying another one" : "Last chance");
        }
    } while(max_attempt-- && rc != NW_GTPV1U_OK);

    bearer->state = BEARER_DOWN;
    LOG_I(GTPU, "Failed to created eNB tunnel endpoint %u for ue id %u, rab id %u, bearer down\n", teid, ue_id, rab_id);

    return -1;
}

int gtpv1u_initial_req(gtpv1u_data_t *gtpv1u_data_p, uint32_t teid,
                       uint16_t port, uint32_t address)
{
    NwGtpv1uUlpApiT ulp_req;
    NwGtpv1uRcT     rc;

    memset(&ulp_req, 0, sizeof(NwGtpv1uUlpApiT));

    ulp_req.apiType = NW_GTPV1U_ULP_API_INITIAL_REQ;
    ulp_req.apiInfo.initialReqInfo.teid     = teid;
    ulp_req.apiInfo.initialReqInfo.peerPort = port;
    ulp_req.apiInfo.initialReqInfo.peerIp   = address;

    rc = nwGtpv1uProcessUlpReq(gtpv1u_data_p->gtpv1u_stack, &ulp_req);
    if (rc == NW_GTPV1U_OK) {
        LOG_D(GTPU, "Successfully sent initial req for teid %u\n", teid);
    } else {
        LOG_W(GTPU, "Could not send initial req for teid %u\n", teid);
    }
    return (rc == NW_GTPV1U_OK) ? 0 : -1;
}

int gtpv1u_new_data_req(gtpv1u_data_t *gtpv1u_data_p,
                        uint8_t ue_id, uint8_t rab_id,
                        uint8_t *buffer, uint32_t buf_len)
{
#ifdef GTPU_IN_KERNEL
    struct sockaddr_in          dummy_dest_addr;
    socklen_t                   socklen = sizeof(struct sockaddr_in);
#endif
    NwGtpv1uUlpApiT          stack_req;
    NwGtpv1uRcT              rc;
    struct gtpv1u_ue_data_s  ue;
    struct gtpv1u_ue_data_s *ue_inst_p;
    struct gtpv1u_bearer_s  *bearer_p;
    hashtbl_rc_t             hash_rc;

    memset(&ue, 0, sizeof(struct gtpv1u_ue_data_s));

    ue.ue_id = ue_id;

    assert(gtpv1u_data_p != NULL);
    assert(rab_id <= MAX_BEARERS_PER_UE);

    /* Check that UE context is present in ue map. */
    hash_rc = hashtbl_get(gtpv1u_data_p->ue_mapping, (uint64_t)ue_id, (void**)&ue_inst_p);

    if (hash_rc ==  HASH_TABLE_KEY_NOT_EXISTS ) {
        LOG_E(GTPU, "[UE %d] Trying to send data on non-existing UE context\n", ue_id);
        return -1;
    }

    bearer_p = &ue_inst_p->bearers[rab_id];

    /* Ensure the bearer in ready.
     * TODO: handle the cases where the bearer is in HANDOVER state.
     * In such case packets should be placed in FIFO.
     */
    if (bearer_p->state != BEARER_UP) {
        LOG_W(GTPU, "Trying to send data over bearer with state(%u) != BEARER_UP\n",
              bearer_p->state);
#warning  LG: HACK WHILE WAITING FOR NAS, normally return -1
        if (bearer_p->state != BEARER_IN_CONFIG)
        return -1;
    }
#ifdef GTPU_IN_KERNEL
    dummy_dest_addr.sin_family      = AF_INET;
    dummy_dest_addr.sin_port        = 5001;
    dummy_dest_addr.sin_addr.s_addr = inet_addr("178.179.180.181");
    if (sendto(gtpv1u_data_p->sock_desc[rab_id], (void *)buffer, buf_len, 0, (struct sockaddr *)&dummy_dest_addr, socklen) < 0) {
        LOG_E(GTPU, "Error during send to socket %d : (%s:%d)\n", gtpv1u_data_p->sock_desc[rab_id], strerror(errno), errno);
        return -1;
    } else {
        LOG_D(GTPU, "send to UDP socket %d, packet should be handled by iptables\n", gtpv1u_data_p->sock_desc[rab_id]);
    }
#else

    memset(&stack_req, 0, sizeof(NwGtpv1uUlpApiT));

    stack_req.apiType                   = NW_GTPV1U_ULP_API_SEND_TPDU;
    // LG HACK stack_req.apiInfo.sendtoInfo.teid   = bearer_p->teid_sgw;
    stack_req.apiInfo.sendtoInfo.teid   = 1;// LG HACK
    stack_req.apiInfo.sendtoInfo.ipAddr = bearer_p->sgw_ip_addr;

    rc = nwGtpv1uGpduMsgNew(gtpv1u_data_p->gtpv1u_stack,
            //bearer_p->teid_sgw,
            1, // LG FORCING 1 instead of bearer_p->teid_sgw
                            NW_FALSE,
                            gtpv1u_data_p->seq_num++,
                            buffer,
                            buf_len,
                            &(stack_req.apiInfo.sendtoInfo.hMsg));

    if (rc != NW_GTPV1U_OK) {
        LOG_E(GTPU, "nwGtpv1uGpduMsgNew failed: 0x%x\n", rc);
        return -1;
    }
    rc = nwGtpv1uProcessUlpReq(gtpv1u_data_p->gtpv1u_stack,
                               &stack_req);
    if (rc != NW_GTPV1U_OK) {
        LOG_E(GTPU, "nwGtpv1uProcessUlpReq failed: 0x%x\n", rc);
        return -1;
    }
    rc = nwGtpv1uMsgDelete(gtpv1u_data_p->gtpv1u_stack,
                           stack_req.apiInfo.sendtoInfo.hMsg);
    if (rc != NW_GTPV1U_OK) {
        LOG_E(GTPU, "nwGtpv1uMsgDelete failed: 0x%x\n", rc);
        return -1;
    }
#endif
    LOG_E(GTPU, "%s() return code OK\n", __FUNCTION__);
    return 0;
}

#ifdef GTPU_IN_KERNEL

#undef GTPV1U_PACKET_RX_RING
/// The number of frames in the ring
//  This number is not set in stone. Nor are block_size, block_nr or frame_size
#define CONF_RING_FRAMES          128

/// Offset of data from start of frame
#define PKT_OFFSET      (TPACKET_ALIGN(sizeof(struct tpacket_hdr)) + \
                         TPACKET_ALIGN(sizeof(struct sockaddr_ll)))

int gtpv1u_eNB_create_sockets(gtpv1u_data_t *gtpv1u_data_p)
{
    int                     value, mark;
    const int              *val_p=&value;
    struct ifreq            ifr;
#ifdef GTPV1U_PACKET_RX_RING
    struct tpacket_req      tp;
#endif
    int                     i;

    if (gtpv1u_data_p == NULL) {
        return -1;
    }

    GTPU_DEBUG("Creating socket for GTPV1U on %s if index %u\n", gtpv1u_data_p->interface_name, gtpv1u_data_p->interface_index);

    for (mark = 0; mark <= 15; mark++) {

        gtpv1u_data_p->sock_desc[mark] = socket( PF_INET , SOCK_DGRAM, 0);
        if (gtpv1u_data_p->sock_desc[mark] < 0) {
            GTPU_ERROR("Error during socket creation (%s:%d)\n",strerror(errno), errno);
            goto error;
        }

//      // socket options, tell the kernel we provide the IP structure
//      if(setsockopt(sgi_data_p->sd[rab_id], IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
//        {
//            SGI_IF_ERROR("Error during socket setsockopt IP_HDRINCL (%s:%d)\n", strerror(errno), errno);
//          goto error;
//      }
//
//
        // setting socket option to use MARK value
        //value = rab_id + SGI_MIN_EPS_BEARER_ID;
        value = mark;
        if (setsockopt (gtpv1u_data_p->sock_desc[mark], SOL_SOCKET, SO_MARK, val_p, sizeof (value)) < 0)
        {
            GTPU_ERROR("error notifying kernel about MARK");
            goto error;
        }
        GTPU_DEBUG("Created socket %d for rab_id %d (for any UE context)\n", gtpv1u_data_p->sock_desc[mark], value);

#ifdef SGI_PACKET_RX_RING
        // tell kernel to export data through mmap()ped ring
        tp.tp_block_size = CONF_RING_FRAMES * getpagesize();
        tp.tp_block_nr   = 1;
        tp.tp_frame_size = getpagesize();
        tp.tp_frame_nr   = CONF_RING_FRAMES;

        if (setsockopt(gtpv1u_data_p->sock_desc[mark], SOL_PACKET, PACKET_RX_RING, (void*) &tp, sizeof(tp))) {
        	GTPU_ERROR("setsockopt() ring\n");
            goto error;
        }

        // open ring
        gtpv1u_data_p->sock_mmap_ring[mark] = mmap(0, tp.tp_block_size * tp.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, sgi_data_p->sd[mark], 0);
        if (!gtpv1u_data_p->sock_mmap_ring[mark]) {
        	GTPU_ERROR("Failed to mmap socket (%s:%d)\n", strerror(errno), errno);
            goto error;
        }
        /* Setup our ringbuffer */
        gtpv1u_data_p->malloc_ring[mark] = malloc(tp.tp_frame_nr * sizeof(struct iovec));
        for(i=0; i<tp.tp_frame_nr; i++) {
        	gtpv1u_data_p->malloc_ring[mark][i].iov_base=(void *)((long)gtpv1u_data_p->sock_mmap_ring[mark])+(i*tp.tp_frame_size);
        	gtpv1u_data_p->malloc_ring[mark][i].iov_len=tp.tp_frame_size;
        }

#endif

        memset(&ifr, 0, sizeof(ifr));
        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), gtpv1u_data_p->interface_name);
        if (setsockopt (gtpv1u_data_p->sock_desc[mark], SOL_SOCKET, SO_BINDTODEVICE,(void *)&ifr, sizeof(ifr)) < 0)
        {
            GTPU_ERROR("error notifying kernel about MARK");
            goto error;
        }
        GTPU_DEBUG("Created socket %d for rab_id %d (for any UE context)\n", gtpv1u_data_p->sock_desc[mark], value);
    }
    return 0;
error:
    GTPU_ERROR("ERROR (%s)\n", strerror(errno));
    for (mark = 0; mark <= 15; mark++) {
        if (gtpv1u_data_p->sock_desc[mark] > 0) {
            close(gtpv1u_data_p->sock_desc[mark]);
        }
        gtpv1u_data_p->sock_desc[mark] = -1;
    }
    return -1;
}
#endif

int gtpv1u_eNB_init(gtpv1u_data_t *gtpv1u_data_p)
{
    NwGtpv1uRcT             rc;
    NwGtpv1uLogMgrEntityT   log;
    NwGtpv1uUdpEntityT      udp;
    NwGtpv1uUlpEntityT      ulp;

    if (gtpv1u_data_p == NULL) {
        return -1;
    }

    //(gtpv1u_data_p was allocated with calloc (zeroed))

//     GTPU_INFO("Initializing GTPU stack for eNB %u\n",
//               gtpv1u_data_p->eNB_id);

    /* Initialize UE tree */
    //RB_INIT(&gtpv1u_data_p->gtpv1u_ue_map_head);

#ifdef GTPU_IN_KERNEL
#warning hardcoded ENB GTPV1U interface name
    gtpv1u_data_p->interface_name = "upenb0";
    gtpv1u_data_p->interface_index = if_nametoindex(gtpv1u_data_p->interface_name);

    gtpv1u_eNB_create_sockets(gtpv1u_data_p);
#endif
    /* Initialize UE hashtable */
    gtpv1u_data_p->ue_mapping = hashtbl_create (256, NULL, NULL);
    if (gtpv1u_data_p->ue_mapping == NULL) {
        perror("hashtbl_create");
        GTPU_ERROR("Initializing TASK_GTPV1_U task interface: ERROR\n");
        return -1;
    }

    if (udp_create_connection(gtpv1u_data_p->ip_addr, GTPV1U_UDP_PORT,
                              &gtpv1u_data_p->udp_data, data_recv_callback, (void *)gtpv1u_data_p) < 0) {
        return -1;
    }

    /* Initializing GTPv1-U stack */
    if ((rc = nwGtpv1uInitialize(&gtpv1u_data_p->gtpv1u_stack)) != NW_GTPV1U_OK) {
        GTPU_ERROR("Failed to setup nwGtpv1u stack %x\n", rc);
        return -1;
    }

    /* Set up the log interface and register the log entity */
    log.logReqCallback = gtpv1u_eNB_log_request;

    if ((rc = nwGtpv1uSetLogMgrEntity(gtpv1u_data_p->gtpv1u_stack,
                                      &log)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetLogMgrEntity: %x\n", rc);
        return -1;
    }

    udp.hUdp = (NwGtpv1uUdpHandleT) &gtpv1u_data_p->udp_data;
    udp.udpDataReqCallback = gtpv1u_eNB_udp_req;

    if ((rc = nwGtpv1uSetUdpEntity(gtpv1u_data_p->gtpv1u_stack,
                                   &udp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUdpEntity: %x\n", rc);
        return -1;
    }

    /* Set the ULP API callback. Called once message have been processed by the
     * nw-gtpv1u stack.
     */
    ulp.ulpReqCallback = gtpv1u_process_stack_req;

    if ((rc = nwGtpv1uSetUlpEntity(gtpv1u_data_p->gtpv1u_stack,
                                   &ulp)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetUlpEntity: %x\n", rc);
        return -1;
    }

    if ((rc = nwGtpv1uSetLogLevel(gtpv1u_data_p->gtpv1u_stack,
                                  NW_LOG_LEVEL_DEBG)) != NW_GTPV1U_OK) {
        GTPU_ERROR("nwGtpv1uSetLogLevel: %x\n", rc);
        return -1;
    }

    gtpv1u_create_tunnel_endpoint(gtpv1u_data_p, 0, 0, "192.168.1.1", 2152);

//     GTPU_INFO("Initializing GTPU stack for eNB %u: DONE\n",
//               gtpv1u_data_p->eNB_id);

    return 0;
}
