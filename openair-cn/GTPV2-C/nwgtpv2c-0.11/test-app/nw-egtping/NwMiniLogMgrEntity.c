/*----------------------------------------------------------------------------*
 *                                                                            *
 *         M I N I M A L I S T I C    L O G M G R     E N T I T Y             *
 *                                                                            *
 *                    Copyright (C) 2010 Amit Chawre.                         *
 *                                                                            *
 *----------------------------------------------------------------------------*/

/** 
 * @file hello-world.c
 * @brief This file contains example of a minimalistic log manager entity.
*/

#include <stdio.h>
#include <assert.h>
#include "NwEvt.h"
#include "NwTypes.h"
#include "NwError.h"
#include "NwLog.h"
#include "NwGtpv2c.h"

#include "NwMiniLogMgrEntity.h"

#ifdef __cplusplus 
extern "C" {
#endif

static
NwCharT* gLogLevelStr[] = {"EMER", "ALER", "CRIT",  "ERRO", "WARN", "NOTI", "INFO", "DEBG"};

NwMiniLogMgrT __gLogMgr;


/*---------------------------------------------------------------------------
 * Public functions
 *--------------------------------------------------------------------------*/

NwMiniLogMgrT* nwMiniLogMgrGetInstance()
{
  return &(__gLogMgr);
}

NwRcT nwMiniLogMgrInit(NwMiniLogMgrT* thiz, NwU32T logLevel )
{
  thiz->logLevel = logLevel;
  return NW_OK;
}

NwRcT nwMiniLogMgrSetLogLevel(NwMiniLogMgrT* thiz, NwU32T logLevel)
{
  thiz->logLevel = logLevel;
}

NwRcT nwMiniLogMgrLogRequest (NwGtpv2cLogMgrHandleT hLogMgr,
    NwU32T logLevel,
    NwCharT* file,
    NwU32T line,
    NwCharT* logStr)
{
  NwMiniLogMgrT* thiz = (NwMiniLogMgrT*) hLogMgr;
  if(thiz->logLevel >= logLevel)
    //printf("NWEGTPSTK  %s - %s <%s,%u>\n", gLogLevelStr[logLevel], logStr, basename(file), line);
  return NW_OK;
}

#ifdef __cplusplus 
}
#endif

