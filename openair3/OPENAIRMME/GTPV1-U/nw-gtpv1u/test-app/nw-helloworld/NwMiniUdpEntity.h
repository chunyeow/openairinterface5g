/*----------------------------------------------------------------------------*
 *                                                                            *
 *            M I N I M A L I S T I C     U D P     E N T I T Y               *
 *                                                                            *
 *                    Copyright (C) 2010 Amit Chawre.                         *
 *                                                                            *
 *----------------------------------------------------------------------------*/


/** 
 * @file NwMiniUdpEntity.h
 * @brief This file contains example of a minimalistic ULP entity.
*/

#include <stdio.h>
#include <assert.h>
#include "NwEvt.h"
#include "NwLog.h"

#ifndef NW_ASSERT
#define NW_ASSERT assert
#endif 

#ifndef __NW_MINI_UDP_ENTITY_H__
#define __NW_MINI_UDP_ENTITY_H__

typedef struct
{
  NwU32T        hSocket;
  NwEventT      ev;
  NwGtpv1uStackHandleT hGtpv1uStack;
} NwMiniUdpEntityT;

#ifdef __cplusplus
extern "C" {
#endif

NwGtpv1uRcT nwMiniUdpInit(NwMiniUdpEntityT* thiz, NwGtpv1uStackHandleT hGtpv1uStack, NwU8T* ipAddr);

NwGtpv1uRcT nwMiniUdpDestroy(NwMiniUdpEntityT* thiz);

NwGtpv1uRcT nwMiniUdpDataReq(NwGtpv1uUdpHandleT udpHandle,
    NwU8T* dataBuf,
    NwU32T dataSize,
    NwU32T peerAddr,
    NwU32T peerPort);

#ifdef __cplusplus
}
#endif

#endif
