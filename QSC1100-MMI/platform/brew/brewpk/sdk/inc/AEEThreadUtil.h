#ifndef AEETHREADUTIL_H
#define AEETHREADUTIL_H /* #ifndef AEETHREADUTIL_H */
/*============================================================================

FILE:  AEEThreadUtil.h

SERVICES:  
        blocking call IThread helper functions

GENERAL DESCRIPTION:
        AEEThread/IThread in BREW provides only the most elementary 
        thread functionality, and leaves implementation of blocking calls
        to an external library (this one).

        These functions can be implemented by anyone, since they use 
        the IThread object in a public way (no private hooks).  Hopefully
        these implementations save some time...

============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                    QUALCOMM Proprietary/GTDR

============================================================================*/
#include "AEEThread.h"
#include "AEEShell.h"
#include "AEENet.h"

/* 
   AEEMutex

   while this is a public declaration, it's mainly here to allow
   global declaration of an instance
   
   Please don't modify the contents of this structure directly, use
   the IThread_Mutex* APIs.
*/

typedef struct AEEMutex
{
   AEECallback *pcbWaiting;
   IThread     *pitHolding;

} AEEMutex;


typedef struct AEECondVar
{
   AEECallback *pcbWaiting;

} AEECondVar;


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

void IThread_Yield(IThread *me, IShell *piShell);

void IThread_Sleep(IThread *me, IShell *piShell, int nMS);

int IThread_Recv(IThread *me, ISocket *piSock, void *pBuf, int cbBuf);

int IThread_Send(IThread *me, ISocket *piSock, void *pBuf, int cbBuf);

int IThread_Connect(IThread *me, ISocket *piSock, INAddr a, INPort p);

int IThread_GetHostByName(IThread *me, INetMgr *piNet, AEEDNSResult *pRes, 
                          const char *pszHost);

ISocket *IThread_Socket(IThread *me, INetMgr *pmgr, NetSocket stype);

int IThread_MutexGrab(IThread *piThread, AEEMutex *pmt);
int IThread_MutexRelease(IThread *piThread, AEEMutex *pmt, IShell *piShell);

int IThread_CondVarTimedWait(IThread *piThread, AEECondVar *pcv, AEEMutex *pmt,
                             IShell *piShell, int nMsecs);

int AEECondVar_Signal(AEECondVar *pcv, IShell *piShell);
int AEECondVar_Broadcast(AEECondVar *pcv, IShell *piShell);




#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEETHREADUTIL_H */
