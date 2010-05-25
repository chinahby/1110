/*
  ============================================================================

FILE:  AEETU_Sleep.c

SERVICES:  
        blocking sleep call

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

void IThread_Sleep(IThread *me,IShell *piShell, int nMS)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);

   ISHELL_SetTimerEx(piShell,nMS,pcb);

   ITHREAD_Suspend(me);
}


