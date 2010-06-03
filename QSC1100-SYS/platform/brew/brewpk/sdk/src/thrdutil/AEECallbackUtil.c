/*
  ============================================================================

FILE:  AEECallbackUtil.h

SERVICES:  
     making AEECallback easier to deal with, mapping shell/socket stuff to
     AEECallback

GENERAL DESCRIPTION:
     The following stubs create AEECallback-based versions of socket and timer
     calls:
     
        AEECallback_SetTimer
        AEECallback_Readable
        AEECallback_Writeable
        AEECallback_Connect
     
     AEECallback-based scheduling corresponds perfectly to a thread's
     needs.  The AEECallback record keeps track of which thing is
     scheduled so that only one thing is scheduled and whatever is
     scheduled can always be canceled readily.
     
     Given a AEECallback-based asynchronous function, we can usually
     construct a blocking threaded version with two lines of code:
     
          DoSomethingCBK( <args> , ITHREAD_GetResumeCallback(piThread));
          ITHREAD_Suspend(piThread);


REVISION HISTORY: 
        Wed Sep 05 13:31:26 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
               	       All Rights Reserved.
               	    QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================ */

#include "AEECallbackUtil.h" 

static void AEECallback_Setup(AEECallback *pcb, PFNCBCANCEL pfnCancel, 
                              void *pvCancel, PFNSCHEDNOTIFY pfnCancelNotify)
{
   CALLBACK_Cancel(pcb);
   pcb->pfnCancel   = pfnCancel;
   pcb->pCancelData = pvCancel;
   pcb->pReserved   = (void *)pfnCancelNotify;
}


// Fire callback in response to a non-AEECallback callback.
//
static void AEECallback_Fire(void *pvCxt)
{
   AEECallback *pcb = (AEECallback *)pvCxt;

   pcb->pfnCancel = 0;
   pcb->pfnNotify(pcb->pNotifyData);
}

static void AEECallback_CancelNotify(AEECallback *pcb)
{
   PFNSCHEDNOTIFY pfnCancelNotify = (PFNSCHEDNOTIFY)pcb->pReserved;

   pcb->pfnCancel = 0;
   pfnCancelNotify(pcb->pCancelData, AEECallback_Fire, pcb);
}

void AEECallback_SchedNotifyWait(AEECallback *pcb, void *pSchedNotifyObj, 
                                 PFNSCHEDNOTIFY pfnSched, 
                                 PFNSCHEDNOTIFY pfnCancel)
{
   AEECallback_Setup(pcb, AEECallback_CancelNotify, pSchedNotifyObj, pfnCancel);
   pfnSched(pSchedNotifyObj, AEECallback_Fire, pcb);
}


static void AEECallback_ConnectDone(void *pvCxt, int nError)
{
   AEECallback *pcb = (AEECallback *) pvCxt;

   *((int *)pcb->pmc) = nError;
   AEECallback_Fire(pcb);
}

void AEECallback_Connect(AEECallback *pcb, ISocket *piSock, 
                         INAddr a, INPort p, int *pnError)
{
   AEECallback_Setup(pcb, AEECallback_CancelNotify, piSock,
                     (PFNSCHEDNOTIFY)piSock->pvt->Cancel);

   pcb->pmc = (void *)pnError;
      
   ISOCKET_Connect(piSock, a, p, AEECallback_ConnectDone, pcb);
}


void AEECallback_SetTimer(AEECallback *pcb, IShell *piShell, int nMS)
{
   AEECallback_Setup(pcb, AEECallback_CancelNotify, piShell, 
                     (PFNSCHEDNOTIFY)piShell->pvt->CancelTimer);

   ISHELL_SetTimer(piShell, nMS, AEECallback_Fire, pcb); 
   // To do: handle failure of SetTimer() ?
}
