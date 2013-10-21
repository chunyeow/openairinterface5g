/*----------------------------------------------------------------------------*
 *                                                                            *
 *                              n w - g t p v 2 c                             * 
 *    G P R S   T u n n e l i n g    P r o t o c o l   v 2 c    S t a c k     *
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
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#include "NwTypes.h"
#include "NwLog.h"
#include "NwUtils.h"
#include "NwGtpv2cLog.h"
#include "NwGtpv2c.h"
#include "NwGtpv2cPrivate.h"
#include "NwGtpv2cIe.h"
#include "NwGtpv2cMsg.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*
 *                     P R I V A T E     F U N C T I O N S                    *
 *----------------------------------------------------------------------------*/

static NwGtpv2cMsgT* gpGtpv2cMsgPool = NULL;

/*----------------------------------------------------------------------------*
 *                       P U B L I C   F U N C T I O N S                      *
 *----------------------------------------------------------------------------*/

NwRcT
nwGtpv2cMsgNew( NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
                NW_IN NwU8T     teidPresent,
                NW_IN NwU8T     msgType,
                NW_IN NwU32T    teid,
                NW_IN NwU32T    seqNum,
                NW_OUT NwGtpv2cMsgHandleT *phMsg)
{
  NwGtpv2cStackT* pStack = (NwGtpv2cStackT*) hGtpcStackHandle;
  NwGtpv2cMsgT *pMsg;

  NW_ASSERT(pStack);

  if(gpGtpv2cMsgPool)
  {
    pMsg = gpGtpv2cMsgPool;
    gpGtpv2cMsgPool = gpGtpv2cMsgPool->next;
  }
  else
  {
    NW_GTPV2C_MALLOC(pStack, sizeof(NwGtpv2cMsgT), pMsg, NwGtpv2cMsgT*);
  }

  if(pMsg)
  {
    pMsg->version       = NW_GTP_VERSION;
    pMsg->teidPresent   = teidPresent;
    pMsg->msgType       = msgType;
    pMsg->teid          = teid;
    pMsg->seqNum        = seqNum;
    pMsg->msgLen        = (NW_GTPV2C_EPC_SPECIFIC_HEADER_SIZE - (teidPresent ? 0 : 4));

    pMsg->groupedIeEncodeStack.top = 0;
    pMsg->hStack        = hGtpcStackHandle;

    *phMsg = (NwGtpv2cMsgHandleT) pMsg;
    NW_LOG(pStack, NW_LOG_LEVEL_DEBG, "Created message %p!", pMsg);
    return NW_OK;
  }

  return NW_FAILURE;
}

NwRcT
nwGtpv2cMsgFromBufferNew( NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
                         NW_IN NwU8T* pBuf,
                         NW_IN NwU32T bufLen,
                         NW_OUT NwGtpv2cMsgHandleT *phMsg)
{
  NwGtpv2cStackT* pStack = (NwGtpv2cStackT*) hGtpcStackHandle;
  NwGtpv2cMsgT *pMsg;

  NW_ASSERT(pStack);

  if(gpGtpv2cMsgPool)
  {
    pMsg = gpGtpv2cMsgPool;
    gpGtpv2cMsgPool = gpGtpv2cMsgPool->next;
  }
  else
  {
    NW_GTPV2C_MALLOC(pStack, sizeof(NwGtpv2cMsgT), pMsg, NwGtpv2cMsgT*);
  }

  if(pMsg)
  {
    *phMsg = (NwGtpv2cMsgHandleT) pMsg;
    memcpy(pMsg->msgBuf, pBuf, bufLen);
    pMsg->msgLen = bufLen;

    pMsg->version       = ((*pBuf) & 0xE0) >> 5;
    pMsg->teidPresent   = ((*pBuf) & 0x08) >> 3;
    pBuf++;

    pMsg->msgType       = *(pBuf);
    pBuf += 3;

    if(pMsg->teidPresent)
    {
      pMsg->teid          = ntohl(*((NwU32T*)(pBuf)));
      pBuf += 4;
    }

    memcpy(((NwU8T*)&pMsg->seqNum) + 1, pBuf, 3);
    pMsg->seqNum  = ntohl(pMsg->seqNum);

    pMsg->hStack        = hGtpcStackHandle;
    NW_LOG(pStack, NW_LOG_LEVEL_DEBG, "Created message %p!", pMsg);
    return NW_OK;
  }
  return NW_FAILURE;
}

NwRcT
nwGtpv2cMsgDelete( NW_IN NwGtpv2cStackHandleT hGtpcStackHandle,
                   NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cStackT* pStack = (NwGtpv2cStackT*) hGtpcStackHandle;
  NW_LOG(pStack, NW_LOG_LEVEL_DEBG, "Purging message %"PRIxPTR"!", hMsg);

  ((NwGtpv2cMsgT*)hMsg)->next = gpGtpv2cMsgPool;
  gpGtpv2cMsgPool = (NwGtpv2cMsgT*) hMsg;

  return NW_OK;
}

 /**
  * Set TEID for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  * @param[in] teid: TEID value.
  */

NwRcT
nwGtpv2cMsgSetTeid(NW_IN NwGtpv2cMsgHandleT hMsg, NwU32T teid)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  thiz->teid = teid; 
  return NW_OK;
}

 /**
  * Set TEID present flag for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  * @param[in] teidPesent: Flag boolean value.
  */

NwRcT
nwGtpv2cMsgSetTeidPresent(NW_IN NwGtpv2cMsgHandleT hMsg, NwBoolT teidPresent)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  thiz->teidPresent = teidPresent; 
  return NW_OK;
}

 /**
  * Set sequence for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  * @param[in] seqNum: Flag boolean value.
  */

NwRcT
nwGtpv2cMsgSetSeqNumber(NW_IN NwGtpv2cMsgHandleT hMsg, NwU32T seqNum)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  thiz->seqNum = seqNum; 
  return NW_OK;
}

 /**
  * Get TEID present for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  */

NwU32T
nwGtpv2cMsgGetTeid(NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  return (thiz->teid); 
}

 /**
  * Get TEID present for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  */

NwBoolT
nwGtpv2cMsgGetTeidPresent(NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  return (thiz->teidPresent); 
}

 /**
  * Get sequence number for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  */

NwU32T
nwGtpv2cMsgGetSeqNumber(NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  return (thiz->seqNum);
}

 /**
  * Get msg type for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  */

NwU32T
nwGtpv2cMsgGetMsgType(NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  return (thiz->msgType);
}

 /**
  * Get msg type for gtpv2c message.
  *
  * @param[in] hMsg : Message handle.
  */

NwU32T
nwGtpv2cMsgGetLength(NW_IN NwGtpv2cMsgHandleT hMsg)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  return (thiz->msgLen);
}


NwRcT
nwGtpv2cMsgAddIeTV1(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       type,
              NW_IN NwU8T       instance,
              NW_IN NwU8T       value)
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv1T *pIe;

  pIe = (NwGtpv2cIeTv1T*) (pMsg->msgBuf + pMsg->msgLen);

  pIe->t        = type;
  pIe->l        = htons(0x0001);
  pIe->i        = instance & 0x00ff;
  pIe->v        = value;

  pMsg->msgLen += sizeof(NwGtpv2cIeTv1T);

  return NW_OK;
}

NwRcT
nwGtpv2cMsgAddIeTV2(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       type,
              NW_IN NwU8T       instance,
              NW_IN NwU16T      value)
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv2T *pIe;

  pIe = (NwGtpv2cIeTv2T*) (pMsg->msgBuf + pMsg->msgLen);

  pIe->t        = type;
  pIe->l        = htons(0x0002);
  pIe->i        = instance & 0x00ff;
  pIe->v        = htons(value);

  pMsg->msgLen += sizeof(NwGtpv2cIeTv2T);

  return NW_OK;
}

NwRcT
nwGtpv2cMsgAddIeTV4(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       type,
              NW_IN NwU8T       instance,
              NW_IN NwU32T      value)
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv4T *pIe;

  pIe = (NwGtpv2cIeTv4T*) (pMsg->msgBuf + pMsg->msgLen);

  pIe->t        = type;
  pIe->l        = htons(0x0004);
  pIe->i        = instance & 0x00ff;
  pIe->v        = htonl(value);

  pMsg->msgLen += sizeof(NwGtpv2cIeTv4T);

  return NW_OK;
}

NwRcT
nwGtpv2cMsgAddIe(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       type,
              NW_IN NwU16T      length,
              NW_IN NwU8T       instance,
              NW_IN NwU8T*      pVal)
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;

  pIe = (NwGtpv2cIeTlvT*) (pMsg->msgBuf + pMsg->msgLen);

  pIe->t        = type;
  pIe->l        = htons(length);
  pIe->i        = instance & 0x00ff;

  memcpy(((NwU8T*)pIe) + 4, pVal, length);
  pMsg->msgLen += (4 + length);

  return NW_OK;
}

NwRcT
nwGtpv2cMsgGroupedIeStart(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       type,
              NW_IN NwU8T       instance)
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;

  pIe = (NwGtpv2cIeTlvT*) (pMsg->msgBuf + pMsg->msgLen);

  pIe->t        = type;
  pIe->i        = instance & 0x00ff;
  pMsg->msgLen += (4);
  pIe->l        = (pMsg->msgLen);

  NW_ASSERT(pMsg->groupedIeEncodeStack.top < NW_GTPV2C_MAX_GROUPED_IE_DEPTH);

  pMsg->groupedIeEncodeStack.pIe[pMsg->groupedIeEncodeStack.top] = pIe;
  pMsg->groupedIeEncodeStack.top++;

  return NW_OK;
}

NwRcT
nwGtpv2cMsgGroupedIeEnd(NW_IN NwGtpv2cMsgHandleT hMsg) 
{
  NwGtpv2cMsgT *pMsg = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;
  
  NW_ASSERT(pMsg->groupedIeEncodeStack.top > 0);

  pMsg->groupedIeEncodeStack.top--;
  pIe = pMsg->groupedIeEncodeStack.pIe[pMsg->groupedIeEncodeStack.top];

  pIe->l       = htons(pMsg->msgLen - pIe->l);

  return NW_OK;
}

NwRcT
nwGtpv2cMsgAddIeCause(NW_IN NwGtpv2cMsgHandleT hMsg, 
                      NW_IN NwU8T instance,
                      NW_IN NwU8T causeValue,
                      NW_IN NwU8T bitFlags,
                      NW_IN NwU8T offendingIeType,
                      NW_IN NwU8T offendingIeInstance)
{
  NwU8T causeBuf[8];

  causeBuf[0] = causeValue;
  causeBuf[1] = bitFlags;

  if(offendingIeType)
  {
    causeBuf[2] = offendingIeType;
    causeBuf[3] = 0;
    causeBuf[4] = 0;
    causeBuf[5] = (offendingIeInstance & 0x0f);
  }

  return (nwGtpv2cMsgAddIe(hMsg, NW_GTPV2C_IE_CAUSE, (offendingIeType? 6 : 2), instance, causeBuf));
}

NwRcT
nwGtpv2cMsgAddIeFteid(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN NwU8T       instance,
              NW_IN NwU8T       ifType,
              NW_IN NwU32T      teidOrGreKey,
              NW_IN NwU32T      ipv4Addr,
              NW_IN NwU8T*      pIpv6Addr)
{

  NwU8T fteidBuf[32];
  NwU8T *pFteidBuf = fteidBuf;

  fteidBuf[0] = (ifType & 0x1F);
  pFteidBuf++;

  *((NwU32T*)(pFteidBuf)) = htonl((teidOrGreKey)); 
  pFteidBuf += 4;

  if(ipv4Addr)
  {
    fteidBuf[0] |= (0x01 << 7);
    *((NwU32T*)(pFteidBuf)) = htonl(ipv4Addr);
    pFteidBuf += 4;
  }

  if(pIpv6Addr)
  {
    fteidBuf[0] |= (0x01 << 6);
    memcpy((pFteidBuf), pIpv6Addr, 16);
    pFteidBuf += 16;
  }
  return (nwGtpv2cMsgAddIe(hMsg, NW_GTPV2C_IE_FTEID, (pFteidBuf - fteidBuf), instance, fteidBuf));
}


NwBoolT
nwGtpv2cMsgIsIePresent(NW_IN NwGtpv2cMsgHandleT hMsg, 
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  if((NwGtpv2cIeTv1T*) thiz->pIe[type][instance])
    return NW_TRUE; 
  return NW_FALSE; 
}

NwRcT
nwGtpv2cMsgGetIeTV1(NW_IN NwGtpv2cMsgHandleT hMsg, 
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_OUT NwU8T* pVal)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv1T *pIe; 
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTv1T*) thiz->pIe[type][instance]; 
    if(ntohs(pIe->l) != 0x01)
      return NW_GTPV2C_IE_INCORRECT;

    if(pVal) *pVal = pIe->v;
    return NW_OK;
  }
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeTV2( NW_IN NwGtpv2cMsgHandleT hMsg,
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_OUT NwU16T* pVal)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv2T *pIe; 
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTv2T*) thiz->pIe[type][instance]; 
    if(ntohs(pIe->l) != 0x02)
      return NW_GTPV2C_IE_INCORRECT;

    if(pVal) *pVal = ntohs(pIe->v);
    return NW_OK;
  }
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeTV4( NW_IN NwGtpv2cMsgHandleT hMsg,
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_OUT NwU32T* pVal)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv4T *pIe; 
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTv4T*) thiz->pIe[type][instance]; 
    if(ntohs(pIe->l) != 0x04)
      return NW_GTPV2C_IE_INCORRECT;

    if(pVal) *pVal = ntohl(pIe->v);
    return NW_OK;
  }
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeTV8( NW_IN NwGtpv2cMsgHandleT hMsg,
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_OUT NwU64T* pVal)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTv8T *pIe; 
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTv8T*) thiz->pIe[type][instance]; 
    if(ntohs(pIe->l) != 0x08)
      return NW_GTPV2C_IE_INCORRECT;

    if(pVal) *pVal = NW_NTOHLL((pIe->v));
    return NW_OK;
  }
  NW_LOG(thiz->hStack, NW_LOG_LEVEL_ERRO, "Cannot retrieve IE of type %u instance %u !", type, instance);
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeTlv( NW_IN NwGtpv2cMsgHandleT hMsg,
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_IN NwU16T maxLen,
                        NW_OUT NwU8T* pVal,
                        NW_OUT NwU16T* pLen)
{
  NwGtpv2cMsgT *thiz  = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTlvT*) thiz->pIe[type][instance]; 
    if(ntohs(pIe->l) <= maxLen)
    {
      if(pVal) memcpy(pVal, ((NwU8T*) pIe) + 4, ntohs(pIe->l));
      if(pLen) *pLen = ntohs(pIe->l);
      return NW_OK;
    }
  }
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeTlvP( NW_IN NwGtpv2cMsgHandleT hMsg,
                        NW_IN NwU8T type,
                        NW_IN NwU8T instance,
                        NW_OUT NwU8T** ppVal,
                        NW_OUT NwU16T* pLen)
{
  NwGtpv2cMsgT *thiz  = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;
  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[type][instance])
  {
    pIe = (NwGtpv2cIeTlvT*) thiz->pIe[type][instance]; 
    if(ppVal) *ppVal = ((NwU8T*) pIe) + 4;
    if(pLen)  *pLen  = ntohs(pIe->l);
    return NW_OK;
  }
  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeCause(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN  NwU8T       instance,
              NW_OUT NwU8T*      causeValue,
              NW_OUT NwU8T*      flags,
              NW_OUT NwU8T*      offendingIeType,
              NW_OUT NwU8T*      offendingIeInstance)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;

  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);
  if(thiz->isIeValid[NW_GTPV2C_IE_CAUSE][instance])
  {
    pIe = (NwGtpv2cIeTlvT*) thiz->pIe[NW_GTPV2C_IE_CAUSE][instance]; 
    *causeValue = *((NwU8T*)(((NwU8T*)pIe) + 4));
    *flags      = *((NwU8T*)(((NwU8T*)pIe) + 5));
    if(pIe->l == 6)
    {
      *offendingIeType    = *((NwU8T*)(((NwU8T*)pIe) + 6));
      *offendingIeType    = *((NwU8T*)(((NwU8T*)pIe) + 8));
    }
    return NW_OK;
  }

  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgGetIeFteid(NW_IN NwGtpv2cMsgHandleT hMsg, 
              NW_IN  NwU8T       instance,
              NW_OUT NwU8T*      ifType,
              NW_OUT NwU32T*     teidOrGreKey,
              NW_OUT NwU32T*     ipv4Addr,
              NW_OUT NwU8T*      pIpv6Addr)
{
  NwGtpv2cMsgT *thiz = (NwGtpv2cMsgT*) hMsg;
  NwGtpv2cIeTlvT *pIe;

  NW_ASSERT(instance <= NW_GTPV2C_IE_INSTANCE_MAXIMUM);

  if(thiz->isIeValid[NW_GTPV2C_IE_FTEID][instance])
  {
    pIe = (NwGtpv2cIeTlvT*) thiz->pIe[NW_GTPV2C_IE_FTEID][instance]; 
    NwU8T flags;
    NwU8T* pIeValue     = ((NwU8T*) pIe) + 4;
    flags               = (*pIeValue) & 0xE0;
    *ifType             = (*pIeValue) & 0x1F;
    pIeValue += 1;

    *teidOrGreKey       = ntohl(*((NwU32T*)(pIeValue))); 
    pIeValue += 4;

    if(flags & 0x80)
    {
      *ipv4Addr           = ntohl(*((NwU32T*)(pIeValue)));
      pIeValue += 4;
    }
    return NW_OK;
  }

  return NW_GTPV2C_IE_MISSING;
}

NwRcT
nwGtpv2cMsgHexDump(NwGtpv2cMsgHandleT hMsg, FILE* fp)
{

  NwGtpv2cMsgT* pMsg = (NwGtpv2cMsgT*) hMsg;
  NwU8T* data = pMsg->msgBuf;
  NwU32T size = pMsg->msgLen;

  unsigned char *p = (unsigned char*)data;
  unsigned char c;
  int n;
  char bytestr[4] = {0};
  char addrstr[10] = {0};
  char hexstr[ 16*3 + 5] = {0};
  char charstr[16*1 + 5] = {0};
  fprintf((FILE*)fp, "\n");
  for(n=1;n<=size;n++) {
    if (n%16 == 1) {
      /* store address for this line */
      snprintf(addrstr, sizeof(addrstr), "%.4lx",
          ((unsigned long)p-(unsigned long)data) );
    }

    c = *p;
    if (isalnum(c) == 0) {
      c = '.';
    }

    /* store hex str (for left side) */
    snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
    strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

    /* store char str (for right side) */
    snprintf(bytestr, sizeof(bytestr), "%c", c);
    strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);
    if(n%16 == 0) {
      /* line completed */
      fprintf((FILE*)fp, "[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
      hexstr[0] = 0;
      charstr[0] = 0;
    } else if(n%8 == 0) {
      /* half line: add whitespaces */
      strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
      strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
    }
    p++; /* next byte */
  }

  if (strlen(hexstr) > 0) {
    /* print rest of buffer if not empty */
    fprintf((FILE*)fp, "[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);

  }
  fprintf((FILE*)fp, "\n");

  return NW_OK;
}

#ifdef __cplusplus
}
#endif



/*--------------------------------------------------------------------------*
 *                          E N D   O F   F I L E                           * 
 *--------------------------------------------------------------------------*/

