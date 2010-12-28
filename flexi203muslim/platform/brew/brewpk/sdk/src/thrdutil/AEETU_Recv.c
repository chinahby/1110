/*
  ============================================================================

FILE:  AEETU_Read.c

SERVICES:  
        blocking socket recv call

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

int IThread_Recv(IThread *me, ISocket *piSock, void *pBuf, int cbBuf)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);
   int nRv;

   while ((nRv = ISOCKET_Read(piSock, pBuf, cbBuf)) == AEE_NET_WOULDBLOCK) {
      AEECallback_Readable(pcb, piSock);
      ITHREAD_Suspend(me);
   }

   return nRv;
}


