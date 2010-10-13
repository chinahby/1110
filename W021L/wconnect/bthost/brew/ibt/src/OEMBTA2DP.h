#ifndef OEMBTA2DP_H
#define OEMBTA2DP_H

/*======================================================
FILE:  OEMBTA2DP.h

SERVICES:  Implementation of IBTA2DP Source.

GENERAL DESCRIPTION:
   Implements IBTA2DP Source

===============================================================================
        Copyright © 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.

 =========================================================================== */

#include "AEEStdLib.h"
#include "AEEBTDef.h"
#include "AEEIBTA2DP.h"

//=============================================================================
// 
//
//=============================================================================

#define OEMBTA2DP_MAX_BDA_LEN    6

typedef struct OEMBTA2DP OEMBTA2DP;
typedef struct {
   uint32   result;
} OEMBTA2DP_EnableType;

typedef struct {
   uint32   result;
} OEMBTA2DP_DisableType;

typedef struct {
   BDAddress    bdAddr;
} OEMBTA2DP_ConType;

typedef struct {
   BDAddress    bdAddr;
   uint32   result;
} OEMBTA2DP_ConFailedType;

typedef struct {
   BDAddress    bdAddr;
   uint32   result;
} OEMBTA2DP_DisconType;

typedef struct {
  BDAddress      bdAddr;
  boolean    bLocalInit;
} OEMBTA2DP_StartType;

typedef struct {
  BDAddress      bdAddr;
  boolean    bLocalInit;
} OEMBTA2DP_SuspendType;

typedef struct {
   BDAddress      bdAddr;
   boolean    bLocalInit;
} OEMBTA2DP_OpenType;

typedef struct {
   BDAddress      bdAddr;
   boolean    bLocalInit;
} OEMBTA2DP_CloseType;

typedef struct {
   uint8    event;
   union
   {
      OEMBTA2DP_EnableType    evtEnbl;
      OEMBTA2DP_DisableType   evtDsbl;
      OEMBTA2DP_ConType       evtConn;
      OEMBTA2DP_ConFailedType evtConFail;
      OEMBTA2DP_DisconType    evtDisc;
      OEMBTA2DP_StartType     evtStart;
      OEMBTA2DP_SuspendType   evtSuspend;
      OEMBTA2DP_OpenType      evtOpen;
      OEMBTA2DP_CloseType     evtClose;
   }u;
}OEMBTA2DPEvent;
//=============================================================================
// 
//
//=============================================================================

typedef void (*PFNA2DPEVTCB)( void            *cbData,
                              OEMBTA2DPEvent  *pEvt);

//=============================================================================
// 
//
//=============================================================================
int OEMBTA2DP_Init(OEMBTA2DP **pOem, PFNA2DPEVTCB cb, void *data);
int OEMBTA2DP_Enable(OEMBTA2DP *pOem);
int OEMBTA2DP_Disable(OEMBTA2DP *me);
int OEMBTA2DP_Connect(OEMBTA2DP *me, const BDAddress *pBDAddr);
int OEMBTA2DP_Disconnect(OEMBTA2DP *pOem, const BDAddress *pBDAddr);
int OEMBTA2DP_SetDevice(OEMBTA2DP *me, const BDAddress *pBDAddr);
int OEMBTA2DP_Release(OEMBTA2DP *me);
int OEMBTA2DP_Config(OEMBTA2DP *me, const BDAddress *pBDAddr, BTA2DPConfig config);

void OEMBTA2DP_StateDisconnected(OEMBTA2DP *me);
void OEMBTA2DP_StateConnected(OEMBTA2DP *me);
void OEMBTA2DP_StateEnabled(OEMBTA2DP *me);
void OEMBTA2DP_StateConnectFailed(OEMBTA2DP *me);
#endif
