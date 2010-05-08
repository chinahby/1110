/*
  ============================================================================

FILE:  AEETU_Connect.c

SERVICES:  
        blocking socket connect call

GENERAL DESCRIPTION:
        AEEThread/IThread in BREW provides only the most elementary 
        thread functionality, and leaves implementation of blocking calls
        to an external library (this one).

        These functions can be implemented by anyone, since they use 
        the IThread object in a public way (no private hooks).  Hopefully
        these implementations save some time...

REVISION HISTORY: 
        Wed Oct 03 14:47:41 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/

#include "AEEThreadUtil.h"
#include "AEECallbackUtil.h"

int IThread_Connect(IThread *me, ISocket *piSock, INAddr a, INPort p)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);
   int nRv;

   AEECallback_Connect(pcb, piSock, a, p, &nRv);
   ITHREAD_Suspend(me);

   return nRv;
}



