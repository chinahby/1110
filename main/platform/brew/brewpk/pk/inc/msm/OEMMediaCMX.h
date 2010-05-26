#ifndef OEMMEDIACMX_H
#define OEMMEDIACMX_H
/*============================================================================
FILE:	OEMMediaCMX.h

SERVICES:  Internal interface for OEMMediaCMX.c

DESCRIPTION:
   This file is contains functinos for OEMMediaCMX.c that can be called
   from elsewhere in the OEM layer
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEEMedia.h"

#define MM_PARM_OEM_BASE             1        // Parms base used by IMedia
#define MM_PARM_OEM_USER_BASE        0x100    // Parms base for derived class

#define MM_PARM_OEM_CHANNEL          (MM_PARM_OEM_BASE + 1)  // [Get] Returns channel Id

//---------------------------------------------------------------------------
//    Function Declarations
//---------------------------------------------------------------------------

typedef void (*PFNMIDIOUTNOTIFY)(void * pObj, boolean bSuccess);

int CMIDIOut_Release(void);
int CMIDIOut_Open(byte nADECType, PFNMIDIOUTNOTIFY pfn, void * pObj, uint32 handle, boolean synchronous);

int CMediaMIDIOutQCP_GetMediaParmOEM(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
int CMediaMIDIOutQCP_AddCachedParam(IMedia * po, int nParmID, void * p1);
int CMediaMIDIOutQCP_CheckCachedParam(IMedia * po, int nParmID, void * p1);
void CMediaMIDIOutQCP_SaveParam(IMedia * po, int nParmID, void * p1);
void CMediaMIDIOutQCP_SetMediaEnv(IMedia * po, uint32 dwEnable, flg inEnv);
void CMediaMIDIOutQCP_FlushParamCache(IMedia * po);

#endif // #ifndef OEMMEDIACMX_H
