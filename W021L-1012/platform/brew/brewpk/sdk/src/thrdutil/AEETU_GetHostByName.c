/*
  ============================================================================

FILE:  AEETU_GetHostByName.c

SERVICES:  
        blocking gethostbyname call

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

int IThread_GetHostByName(IThread *me, INetMgr *piNet, AEEDNSResult *pRes, 
                          const char *pszHost)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);

   INETMGR_GetHostByName(piNet, pRes, pszHost, pcb);
   ITHREAD_Suspend(me);

   if (((AEEDNSResult *)0 != pRes) && 
       (pRes->nResult > 0) && 
       (pRes->nResult <= AEEDNSMAXADDRS)) {
      return pRes->nResult;
   } else {
      return 0;
   }
}



