/*----------------------------------------------------------------------------*
 *                                                                            *
 *            M I N I M A L I S T I C     U D P     E N T I T Y               *
 *                                                                            *
 *                    Copyright (C) 2010 Amit Chawre.                         *
 *                                                                            *
 *----------------------------------------------------------------------------*/


/** 
 * @file NwMiniUdpEntity.c
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
  NwGtpv2cStackHandleT hGtpv2cStack;
} NwGtpv2cNodeUdpT;

#ifdef __cplusplus
extern "C" {
#endif

NwRcT nwGtpv2cUdpInit(NwGtpv2cNodeUdpT* thiz, NwGtpv2cStackHandleT hGtpv2cStack, NwU8T* ipv4Addr);

NwRcT nwGtpv2cUdpDestroy(NwGtpv2cNodeUdpT* thiz);

NwRcT nwGtpv2cUdpDataReq(NwGtpv2cUdpHandleT udpHandle,
    NwU8T* dataBuf,
    NwU32T dataSize,
    NwU32T peerIp,
    NwU32T peerPort);

#ifdef __cplusplus
}
#endif

#endif
