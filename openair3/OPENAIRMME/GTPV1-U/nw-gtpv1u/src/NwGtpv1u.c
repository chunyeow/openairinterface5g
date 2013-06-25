/*----------------------------------------------------------------------------*
 *                                                                            *
 *                             n w - g t p v 2 u                              * 
 *    G P R S    T u n n e l i n g    P r o t o c o l   v 2 u   S t a c k     *
 *                                                                            *
 *                                                                            *
 * Copyright (c) 2010-2011 Amit Chawre                                        *
 * All rights reserved.                                                       *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions         *
 * are met:                                                                   *
 *                                                                            *
 * 1. Redistributions of source code must retain the above copyright          *
 *    notice, this list of conditions and the following disclaimer.           *
 * 2. Redistributions in binary form must reproduce the above copyright       *
 *    notice, this list of conditions and the following disclaimer in the     *
 *    documentation and/or other materials provided with the distribution.    *
 * 3. The name of the author may not be used to endorse or promote products   *
 *    derived from this software without specific prior written permission.   *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR       *
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.    *
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,           *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT   *
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY      *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT        *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF   *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NwTypes.h"
#include "NwUtils.h"
#include "NwGtpv1uError.h"
#include "NwGtpv1uPrivate.h"
#include "NwGtpv1uTunnelEndPoint.h"
#include "NwGtpv1uTrxn.h"
#include "NwGtpv1u.h"
#include "NwGtpv1uIe.h"
#include "NwGtpv1uLog.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*
 *                    P R I V A T E    F U N C T I O N S                    *
 *--------------------------------------------------------------------------*/

static void 
nwGtpv1uDisplayBanner( NwGtpv1uStackT* thiz)
{
  printf(" *----------------------------------------------------------------------------*\n");
  printf(" *                                                                            *\n");
  printf(" *                             n w - g t p v 2 u                              *\n");
  printf(" *    G P R S    T u n n e l i n g    P r o t o c o l   v 2 u   S t a c k     *\n");
  printf(" *                                                                            *\n");
  printf(" *                                                                            *\n");
  printf(" * Copyright (c) 2010-2011 Amit Chawre                                        *\n");
  printf(" * All rights reserved.                                                       *\n");
  printf(" *                                                                            *\n");
  printf(" * Redistribution and use in source and binary forms, with or without         *\n");
  printf(" * modification, are permitted provided that the following conditions         *\n");
  printf(" * are met:                                                                   *\n");
  printf(" *                                                                            *\n");
  printf(" * 1. Redistributions of source code must retain the above copyright          *\n");
  printf(" *    notice, this list of conditions and the following disclaimer.           *\n");
  printf(" * 2. Redistributions in binary form must reproduce the above copyright       *\n");
  printf(" *    notice, this list of conditions and the following disclaimer in the     *\n");
  printf(" *    documentation and/or other materials provided with the distribution.    *\n");
  printf(" * 3. The name of the author may not be used to endorse or promote products   *\n");
  printf(" *    derived from this software without specific prior written permission.   *\n");
  printf(" *                                                                            *\n");
  printf(" * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR       *\n");
  printf(" * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *\n");
  printf(" * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.    *\n");
  printf(" * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,           *\n");
  printf(" * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT   *\n");
  printf(" * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *\n");
  printf(" * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY      *\n");
  printf(" * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT        *\n");
  printf(" * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF   *\n");
  printf(" * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *\n");
  printf(" *----------------------------------------------------------------------------*\n\n");

}
/*---------------------------------------------------------------------------
 * RBTree Search Functions 
 *--------------------------------------------------------------------------*/

static inline NwS32T
nwGtpv1uCompareTeid(struct NwGtpv1uTunnelEndPoint* a, struct NwGtpv1uTunnelEndPoint* b);

static inline NwS32T
nwGtpv1uCompareSeqNum(struct NwGtpv1uTrxn* a, struct NwGtpv1uTrxn* b);

RB_PROTOTYPE(NwGtpv1uOutstandingTxSeqNumTrxnMap, NwGtpv1uTrxn, outstandingTxSeqNumMapRbtNode, nwGtpv1uCompareSeqNum)
RB_PROTOTYPE(NwGtpv1uOutstandingRxSeqNumTrxnMap, NwGtpv1uTrxn, outstandingRxSeqNumMapRbtNode, nwGtpv1uCompareSeqNum)
RB_PROTOTYPE(NwGtpv1uTunnelEndPointTMap, NwGtpv1uTunnelEndPoint, sessionMapRbtNode, nwGtpv1uCompareTeid)
RB_PROTOTYPE(NwGtpv1uTunnelEndPointIdentifierMap, NwGtpv1uTunnelEndPoint, sessionMapRbtNode, nwGtpv1uCompareTeid)

RB_GENERATE(NwGtpv1uOutstandingTxSeqNumTrxnMap, NwGtpv1uTrxn, outstandingTxSeqNumMapRbtNode, nwGtpv1uCompareSeqNum)
RB_GENERATE(NwGtpv1uOutstandingRxSeqNumTrxnMap, NwGtpv1uTrxn, outstandingRxSeqNumMapRbtNode, nwGtpv1uCompareSeqNum)
RB_GENERATE(NwGtpv1uTunnelEndPointTMap, NwGtpv1uTunnelEndPoint, sessionMapRbtNode, nwGtpv1uCompareTeid)
RB_GENERATE(NwGtpv1uTunnelEndPointIdentifierMap, NwGtpv1uTunnelEndPoint, sessionMapRbtNode, nwGtpv1uCompareTeid)

/**
  Comparator funtion for comparing two sessions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the 
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
 */

static inline NwS32T
nwGtpv1uCompareTeid(struct NwGtpv1uTunnelEndPoint* a, struct NwGtpv1uTunnelEndPoint* b)
{
  if(a->teid > b->teid)
    return 1;
  if(a->teid < b->teid)
    return -1;
  return 0;
}

/**
  Comparator funtion for comparing two sequence number transactions.

  @param[in] a: Pointer to session a.
  @param[in] b: Pointer to session b.
  @return  An integer greater than, equal to or less than zero according to whether the 
  object pointed to by a is greater than, equal to or less than the object pointed to by b.
 */

static inline NwS32T
nwGtpv1uCompareSeqNum(struct NwGtpv1uTrxn* a, struct NwGtpv1uTrxn* b)
{
  if(a->seqNum > b->seqNum)
    return 1;
  if(a->seqNum < b->seqNum)
    return -1;
  if(a->peerIp > b->peerIp) 
    return 1;
  if(a->peerIp < b->peerIp) 
    return -1;
  return 0;
}

/**
  Create and Send GTPU message.

  @param[in] thiz: Stack handle
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uCreateAndSendMsg( NwGtpv1uStackT* thiz, NwU32T peerIp, NwU16T peerPort, NwGtpv1uMsgT* pMsg)
{
  NwGtpv1uRcT rc;
  NwU8T* msgHdr;

  NW_ASSERT(thiz);
  NW_ASSERT(pMsg);

  NW_ENTER(thiz);

  msgHdr = pMsg->msgBuf;
  NW_ASSERT(msgHdr != NULL);

  *(msgHdr++)         = (pMsg->version << 5)            | 
    (pMsg->protocolType << 4)       | 
    (pMsg->extHdrFlag << 2)         | 
    (pMsg->seqNumFlag << 1)         | 
    (pMsg->npduNumFlag);

  *(msgHdr++)         = (pMsg->msgType);
  *((NwU16T*) msgHdr) = htons(pMsg->msgLen);
  msgHdr += 2;

  *((NwU32T*) msgHdr) = htonl(pMsg->teid);
  msgHdr += 4;

  if(pMsg->seqNumFlag || pMsg->extHdrFlag || pMsg->npduNumFlag)
  {
    *((NwU16T*) msgHdr) = (pMsg->seqNumFlag ? htons(pMsg->seqNum) : 0x0000);
    msgHdr += 2; 

    *((NwU8T*) msgHdr) = (pMsg->npduNumFlag ? htons(pMsg->npduNum) : 0x00);
    msgHdr++; 

    *((NwU8T*) msgHdr) = (pMsg->extHdrFlag ? htons(pMsg->nextExtHdrType) : 0x00);
    msgHdr++; 
  }

  rc = thiz->udp.udpDataReqCallback(thiz->udp.hUdp,
      pMsg->msgBuf,
      pMsg->msgLen,
      peerIp,
      peerPort);

  NW_LEAVE(thiz);
  return rc;
}

/**
 * Send GTPv1u Message Indication to ULP entity.
 *
 * @param[in] hGtpuStackHandle : Stack handle
 * @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uSendUlpMessageIndication( NW_IN NwGtpv1uStackT* thiz,
    NW_IN NwU32T  hUlpTrxn,
    NW_IN NwU32T  apiType,
    NW_IN NwU32T  msgType,
    NW_IN NwU32T  peerIp,
    NW_IN NwU16T  peerPort,
    NW_IN NwU8T  *pMsgBuf,
    NW_IN NwU16T  msgLength)
{
  NwGtpv1uRcT rc;
  NwGtpv1uUlpApiT ulpApi;

  NW_ENTER(thiz);

  ulpApi.apiType                        = apiType;
  ulpApi.apiInfo.recvMsgInfo.msgType    = msgType;
  ulpApi.apiInfo.recvMsgInfo.hUlpTrxn   = hUlpTrxn;
  ulpApi.apiInfo.recvMsgInfo.peerIp     = peerIp;
  ulpApi.apiInfo.recvMsgInfo.peerPort   = peerPort;

  if(pMsgBuf && msgLength)
  {
    rc = nwGtpv1uMsgFromBufferNew((NwGtpv1uStackHandleT)thiz, pMsgBuf, msgLength, &(ulpApi.apiInfo.recvMsgInfo.hMsg));
    NW_ASSERT(rc == NW_GTPV1U_OK);
  }

  rc = thiz->ulp.ulpReqCallback(thiz->ulp.hUlp, &ulpApi);
  NW_ASSERT(rc == NW_GTPV1U_OK);

  NW_LEAVE(thiz);

  return rc;
}

NwGtpv1uRcT
nwGtpv1uPeerRspTimeout(void* arg)
{
  NwGtpv1uRcT rc = NW_GTPV1U_OK;
  NwGtpv1uTrxnT* thiz;
  NwGtpv1uTimeoutInfoT *timeoutInfo = arg;

  printf("Retransmission timer expired\n");

  thiz = ((NwGtpv1uTrxnT*)timeoutInfo->timeoutArg);
  rc = thiz->pStack->udp.udpDataReqCallback(thiz->pStack->udp.hUdp,
      thiz->pMsg->msgBuf,
      thiz->pMsg->msgLen,
      thiz->peerIp,
      thiz->peerPort);

  if(thiz->maxRetries)
  {
    rc = thiz->pStack->tmrMgr.tmrStartCallback(thiz->pStack->tmrMgr.tmrMgrHandle, 5, 0, NW_GTPV1U_TMR_TYPE_ONE_SHOT, (void*)timeoutInfo, &thiz->hRspTmr);
    thiz->maxRetries--;
  }
  else
  {
    /* Inform session layer about path fialure */
    printf("Max retries over!\n");
  }
  return rc;
}

/*---------------------------------------------------------------------------
 * ULP API Processing Functions 
 *--------------------------------------------------------------------------*/

/**
  Process NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT Request from ULP entity.

  @param[in] hGtpuStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
NwGtpv1uCreateTunnelEndPoint( NW_IN  NwGtpv1uStackT* thiz,  
                    NW_IN  NwU32T teid, 
                    NW_IN  NwGtpv1uUlpSessionHandleT hUlpSession, 
                    NW_OUT NwGtpv1uStackSessionHandleT *phStackSession )
{
  NwGtpv1uRcT rc = NW_GTPV1U_OK;
  NwGtpv1uTunnelEndPointT* pTunnelEndPoint;
  NwGtpv1uTunnelEndPointT* pCollision;

  NW_ENTER(thiz);

  pTunnelEndPoint = nwGtpTunnelEndPointNew(thiz);

  if(pTunnelEndPoint)
  {

    pTunnelEndPoint->teid           = teid;
    pTunnelEndPoint->pStack         = thiz;
    pTunnelEndPoint->hUlpSession    = hUlpSession;

    pCollision = RB_INSERT(NwGtpv1uTunnelEndPointIdentifierMap, &(thiz->teidMap), pTunnelEndPoint);

    if(pCollision)
    {
      NW_LOG(thiz, NW_LOG_LEVEL_ERRO, "Tunnel end-point cannot be created for teid 0x%x. Tunnel already exists", teid);
      rc = nwGtpTunnelEndPointDestroy(thiz, pTunnelEndPoint);
      NW_ASSERT(rc == NW_GTPV1U_OK);
      *phStackSession = (NwGtpv1uStackSessionHandleT) 0; 
      NW_ASSERT(0);
      rc = NW_GTPV1U_FAILURE;
    }
    else
    {
      *phStackSession = (NwGtpv1uStackSessionHandleT) pTunnelEndPoint; 
      pTunnelEndPoint = RB_FIND(NwGtpv1uTunnelEndPointIdentifierMap, &(thiz->teidMap), pTunnelEndPoint);
      NW_ASSERT(pTunnelEndPoint);
      NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Tunnel end-point 0x%x creation successful for teid 0x%x",
             (unsigned int)pTunnelEndPoint, teid);
    }

  }
  else
  {
    *phStackSession = (NwGtpv1uStackSessionHandleT) 0; 
    rc = NW_GTPV1U_FAILURE;
  }

  NW_LEAVE(thiz);
  return rc;
}

/**
  Process NW_GTPV1U_ULP_API_DESTROY_TUNNEL_ENDPOINT Request from ULP entity.

  @param[in] hGtpuStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uDestroyTunnelEndPoint( NwGtpv1uStackT* thiz,  NW_IN NwGtpv1uUlpApiT *pUlpReq)
{
  NwGtpv1uRcT rc = NW_GTPV1U_OK;
  NwGtpv1uTunnelEndPointT *pRemovedTeid;

  if(pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle)
  {
    NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Destroying Tunnel end-point '%x'", pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle);
    pRemovedTeid = RB_REMOVE(NwGtpv1uTunnelEndPointIdentifierMap, &(thiz->teidMap), (NwGtpv1uTunnelEndPointT*)(pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle));

    NW_ASSERT(pRemovedTeid == (NwGtpv1uTunnelEndPointT*)(pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle));

    rc = nwGtpTunnelEndPointDestroy(thiz, (NwGtpv1uTunnelEndPointT*) pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle);
  }
  else
  {
    NW_LOG(thiz, NW_LOG_LEVEL_WARN, "Non-existent Tunnel end-point handle '%x'", pUlpReq->apiInfo.destroyTunnelEndPointInfo.hStackSessionHandle);
  }

  return rc;
}

/**
  Process NW_GTPV1U_ULP_API_INITIAL_REQ Request from ULP entity.

  @param[in] hGtpuStackHandle : Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uInitialReq( NW_IN NwGtpv1uStackT* thiz, NW_IN NwGtpv1uUlpApiT *pUlpReq)
{
  NwGtpv1uRcT rc;
  NwGtpv1uTrxnT *pTrxn;

  NW_ENTER(thiz);

  /* Create New Transaction */
  rc = nwGtpv1uTrxnNew(thiz, &pTrxn);

  if(pTrxn)
  {
    rc = nwGtpv1uTrxnCreateAndSendMsg(thiz,
        pTrxn,
        pUlpReq->apiInfo.initialReqInfo.peerIp,
        pUlpReq->apiInfo.initialReqInfo.peerPort,
        (NwGtpv1uMsgT*) pUlpReq->hMsg);

    if(rc == NW_GTPV1U_OK)
    {
      /* Insert into search tree */
      RB_INSERT(NwGtpv1uOutstandingTxSeqNumTrxnMap, &(thiz->outstandingTxSeqNumMap), pTrxn);
    }
    else
    {
      rc = nwGtpv1uTrxnDelete(&pTrxn);
      NW_ASSERT(rc == NW_GTPV1U_OK);
    }
  }

  NW_LEAVE(thiz);

  return rc;
}

/**
  Process NW_GTPV1U_ULP_API_SEND_TPDU Request from ULP entity.

  @param[in] thiz: Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uSendto( NwGtpv1uStackT* thiz,  NW_IN NwGtpv1uUlpApiT *pUlpReq)
{
  NwGtpv1uRcT rc;

  NW_ASSERT(thiz);
  NW_ENTER(thiz);

  (void) nwGtpv1uMsgSetTeid(pUlpReq->apiInfo.sendtoInfo.hMsg, pUlpReq->apiInfo.sendtoInfo.teid);

  rc = nwGtpv1uCreateAndSendMsg(thiz,
      pUlpReq->apiInfo.sendtoInfo.ipAddr,
      2152,
      (NwGtpv1uMsgT*) (NwGtpv1uMsgT*) pUlpReq->apiInfo.sendtoInfo.hMsg);

  NW_LEAVE(thiz);
  return rc;
}

/**
  Process GPDU from UDP entity.

  @param[in] thiz: Stack handle
  @param[in] pUlpReq : Pointer to Ulp Req.
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT
nwGtpv1uProcessGpdu( NwGtpv1uStackT* thiz, 
                     NW_IN NwU8T* gpdu,
                     NW_IN NwU32T gdpuLen,
                     NW_IN NwU32T peerIp)

{
  NwGtpv1uRcT rc;
  NwGtpv1uMsgHeaderT* msgHdr;
  NwGtpv1uTunnelEndPointT* pTunnelEndPoint;
  NwGtpv1uTunnelEndPointT tunnelEndPointKey;

  NW_ENTER(thiz);

  msgHdr = (NwGtpv1uMsgHeaderT*) gpdu;

  tunnelEndPointKey.teid = ntohl(msgHdr->teid);
  pTunnelEndPoint = RB_FIND(NwGtpv1uTunnelEndPointIdentifierMap, &(thiz->teidMap), &tunnelEndPointKey);

  if(pTunnelEndPoint)
  {
    NwGtpv1uMsgHandleT hMsg;

    rc = nwGtpv1uMsgFromBufferNew( (NwGtpv1uStackHandleT)thiz,
        (NwU8T*)gpdu,
        gdpuLen,
        &hMsg);

    if(NW_GTPV1U_OK == rc)
    {
      NwGtpv1uMsgT* pMsg = (NwGtpv1uMsgT*) hMsg;
      NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received T-PDU over tunnel end-point '%x' of size %u from "NW_IPV4_ADDR, ntohl(msgHdr->teid), pMsg->msgLen, NW_IPV4_ADDR_FORMAT((peerIp)));
      rc = nwGtpSessionSendMsgApiToUlpEntity(pTunnelEndPoint, pMsg);
    }
  }
  else
  {
    NW_LOG(thiz, NW_LOG_LEVEL_ERRO, "Received T-PDU over non-existent tunnel end-point '%x' from "NW_IPV4_ADDR, ntohl(msgHdr->teid), NW_IPV4_ADDR_FORMAT((peerIp)));
  }
  NW_LEAVE(thiz);

  return rc;
}

/**
  Handle Echo Request from Peer Entity.

  @param[in] thiz : Stack context 
  @return NW_GTPV1U_OK on success.
 */

static NwGtpv1uRcT 
nwGtpv1uHandleEchoReq(NW_IN NwGtpv1uStackT *thiz,
                    NW_IN NwU8T* msgBuf,
                    NW_IN NwU32T msgBufLen,
                    NW_IN NwU16T peerPort,
                    NW_IN NwU32T peerIp)
{
  NwGtpv1uRcT           rc;
  NwU16T                seqNum = 0;
  NwGtpv1uMsgHandleT    hMsg = 0;

  seqNum = ntohs(*(NwU16T*) (msgBuf + (((*msgBuf) & 0x02) ? 8 : 4)));

  /* Send Echo Response */

  rc = nwGtpv1uMsgNew( (NwGtpv1uStackHandleT)thiz,
      NW_TRUE,         /* SeqNum flag    */
      NW_FALSE,
      NW_FALSE,
      NW_GTP_ECHO_RSP,  /* Msg Type             */
      0x00000000U,      /* TEID                 */
      seqNum,           /* Seq Number           */
      0,
      0,
      (&hMsg));

  NW_ASSERT(NW_GTPV1U_OK == rc);

  /* 
   * The Restart Counter value in the Recovery information element shall 
   * not be used, i.e. it shall be set to zero by the sender and shall be 
   * ignored by the receiver. 
   */ 
  rc = nwGtpv1uMsgAddIeTV1(hMsg, NW_GTPV1U_IE_RECOVERY, 0x00);

  NW_LOG(thiz, NW_LOG_LEVEL_INFO, "Sending NW_GTP_ECHO_RSP message to %x:%x with seq %u", peerIp, peerPort, seqNum);

  rc = nwGtpv1uCreateAndSendMsg(thiz,
      peerIp,
      peerPort,
      (NwGtpv1uMsgT*) hMsg);

  rc = nwGtpv1uMsgDelete((NwGtpv1uStackHandleT)thiz, hMsg);
  NW_ASSERT(NW_GTPV1U_OK == rc);

  return rc;
}

/*--------------------------------------------------------------------------*
 *                     P U B L I C   F U N C T I O N S                      *
 *--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Constructor
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT
nwGtpv1uInitialize( NW_INOUT NwGtpv1uStackHandleT* hGtpuStackHandle)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz;

  thiz = (NwGtpv1uStackT*) malloc( sizeof(NwGtpv1uStackT));

  if(thiz)
  {
    thiz->id    = (NwU32T) thiz;
    thiz->seq   = (NwU16T) ((NwU32T)thiz) ;
    RB_INIT(&(thiz->outstandingTxSeqNumMap));
    RB_INIT(&(thiz->outstandingRxSeqNumMap));
    if (0 == 1)
        nwGtpv1uDisplayBanner(thiz);

    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }


  *hGtpuStackHandle = (NwGtpv1uStackHandleT) thiz;
  return rc;
}


/*---------------------------------------------------------------------------
 * Destructor
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT
nwGtpv1uFinalize( NW_IN  NwGtpv1uStackHandleT hGtpuStackHandle)
{
  NwGtpv1uRcT rc;
  if(hGtpuStackHandle)
  {
    free((void*)hGtpuStackHandle);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }
  return rc;
}


/*---------------------------------------------------------------------------
 * Configuration Get/Set
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT
nwGtpv1uSetUlpEntity( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                          NW_IN NwGtpv1uUlpEntityT* pUlpEntity)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  if(pUlpEntity)
  {
    thiz->ulp = *(pUlpEntity);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }

  return rc;
}


NwGtpv1uRcT
nwGtpv1uSetUdpEntity( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                          NW_IN NwGtpv1uUdpEntityT* pUdpEntity)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  if(pUdpEntity)
  {
    thiz->udp = *(pUdpEntity);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }

  return rc;
}

NwGtpv1uRcT
nwGtpv1uSetMemMgrEntity( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                               NW_IN NwGtpv1uMemMgrEntityT* pMemMgrEntity)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  if(pMemMgrEntity)
  {
    thiz->memMgr = *(pMemMgrEntity);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }

  return rc;
}


NwGtpv1uRcT
nwGtpv1uSetTimerMgrEntity( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                               NW_IN NwGtpv1uTimerMgrEntityT* pTmrMgrEntity)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  if(pTmrMgrEntity)
  {
    thiz->tmrMgr = *(pTmrMgrEntity);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }

  return rc;
}


NwGtpv1uRcT
nwGtpv1uSetLogMgrEntity( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                             NW_IN NwGtpv1uLogMgrEntityT* pLogMgrEntity)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;
 
  if(pLogMgrEntity)
  {
    thiz->logMgr = *(pLogMgrEntity);
    rc = NW_GTPV1U_OK;
  }
  else
  {
    rc = NW_GTPV1U_FAILURE;
  }
 return rc;
}

NwGtpv1uRcT
nwGtpv1uSetLogLevel( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                         NW_IN NwU32T logLevel)
{
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;
  thiz->logLevel = logLevel;
  return NW_GTPV1U_OK;
}

/*---------------------------------------------------------------------------
 * Process Request from Udp Layer
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT 
nwGtpv1uProcessUdpReq( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle, 
                    NW_IN NwU8T* udpData,
                    NW_IN NwU32T udpDataLen,
                    NW_IN NwU16T peerPort,
                    NW_IN NwU32T peerIp)
{
  NwGtpv1uRcT           rc;
  NwGtpv1uStackT*       thiz;
  NwU16T                msgType;

  rc = NW_GTPV1U_FAILURE;
  thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  NW_ASSERT(thiz);

  msgType = *((NwU8T*)(udpData + 1));

  switch(msgType)
  {
    case NW_GTP_ECHO_REQ:
      rc = nwGtpv1uHandleEchoReq( thiz, udpData, udpDataLen, peerPort, peerIp);
      break;

    case NW_GTP_ERROR_INDICATION:

      rc  = nwGtpv1uSendUlpMessageIndication( thiz,
          0,
          NW_GTPV1U_ULP_API_RECV_MSG,
          msgType,
          peerIp,
          peerPort,
          udpData,
          udpDataLen);

      NW_ASSERT(rc == NW_GTPV1U_OK);

      break;

    case NW_GTP_ECHO_RSP:
      rc = NW_GTPV1U_OK;
      break;

    case NW_GTP_GPDU:
      rc = nwGtpv1uProcessGpdu(thiz, udpData, udpDataLen, peerIp);
      break;

    default:
      NW_ASSERT(0);
      break;
  }

  NW_LEAVE(thiz);
  return rc;
}


/*---------------------------------------------------------------------------
 * Process Request from Upper Layer
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT
nwGtpv1uProcessUlpReq( NW_IN NwGtpv1uStackHandleT hGtpuStackHandle,
                    NW_IN NwGtpv1uUlpApiT *pUlpReq)
{
  NwGtpv1uRcT rc = NW_GTPV1U_FAILURE;
  NwGtpv1uStackT* thiz = (NwGtpv1uStackT*) hGtpuStackHandle;

  NW_ASSERT(thiz);
  NW_ASSERT(pUlpReq != NULL);

  NW_ENTER(thiz);

  switch(pUlpReq->apiType)
  {
    case NW_GTPV1U_ULP_API_CREATE_TUNNEL_ENDPOINT:
      {
        NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received create session req from ulp");
        rc = NwGtpv1uCreateTunnelEndPoint(thiz, 
            pUlpReq->apiInfo.createTunnelEndPointInfo.teid,
            pUlpReq->apiInfo.createTunnelEndPointInfo.hUlpSession,
            &(pUlpReq->apiInfo.createTunnelEndPointInfo.hStackSession));
      }
      break;

    case NW_GTPV1U_ULP_API_DESTROY_TUNNEL_ENDPOINT:
      {
        NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received destroy session req from ulp");
        rc = nwGtpv1uDestroyTunnelEndPoint(thiz,  pUlpReq);
      }
      break;

    case NW_GTPV1U_ULP_API_INITIAL_REQ:
      {
        NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received initial req from ulp");
        rc = nwGtpv1uInitialReq(thiz, pUlpReq);
      }
      break;

    case NW_GTPV1U_ULP_API_SEND_TPDU:
      {
        NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received send tpdu req from ulp");
        rc = nwGtpv1uSendto(thiz,  pUlpReq);
      }
      break;

    default:
      NW_LOG(thiz, NW_LOG_LEVEL_ERRO, "Unsupported API received from ulp");
      rc = NW_GTPV1U_FAILURE;
      break;
  }

  NW_LEAVE(thiz);

  return rc;
}

/*---------------------------------------------------------------------------
 * Process Timer timeout Request from Timer Manager
 *--------------------------------------------------------------------------*/

NwGtpv1uRcT
nwGtpv1uProcessTimeout(void* timeoutInfo)
{
  NwGtpv1uRcT rc;
  NwGtpv1uStackT* thiz;

  NW_ASSERT(timeoutInfo != NULL);

  thiz = (NwGtpv1uStackT*) (((NwGtpv1uTimeoutInfoT*) timeoutInfo)->hStack);

  NW_ASSERT(thiz != NULL);

  NW_ENTER(thiz);
  NW_LOG(thiz, NW_LOG_LEVEL_DEBG, "Received timeout event from ULP with timeoutInfo %x!", (unsigned int)timeoutInfo);

  rc = (((NwGtpv1uTimeoutInfoT*) timeoutInfo)->timeoutCallbackFunc) (timeoutInfo);

  NW_LEAVE(thiz);

  return rc;
}

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*
 *                      E N D     O F    F I L E                            *
 *--------------------------------------------------------------------------*/

