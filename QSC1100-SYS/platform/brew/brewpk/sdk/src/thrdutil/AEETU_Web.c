/*
  ============================================================================

FILE:  AEETU_Web.h

SERVICES:  
        thread util header for IWeb interactions

GENERAL DESCRIPTION:
        AEEThread/IThread in BREW provides only the most elementary 
        thread functionality, and leaves implementation of blocking calls
        to an external library (this one).

        These functions can be implemented by anyone, since they use 
        the IThread object in a public way (no private hooks).  Hopefully
        these implementations save some time...


REVISION HISTORY: 
        Fri Oct  5 17:31:29 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#include "AEETU_Web.h"

void IThread_GetWebResponseV(IThread *me, IWeb *piWeb, IWebResp **ppiwresp,
                             const char *cpszUrl, WebOpt *awozList)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);
   
   IWEB_GetResponseV(piWeb,ppiwresp,pcb,cpszUrl,awozList);

   ITHREAD_Suspend(me);
}

void IThread_GetWebResponse(IThread *me, IWeb *piWeb, IWebResp **ppiwresp,
                            const char *cpszUrl, ... )
{
   WebOpt *awozList = (WebOpt *)(((char *)&cpszUrl) + sizeof(cpszUrl));
   
   IThread_GetWebResponseV(me, piWeb, ppiwresp, cpszUrl, awozList);
}



