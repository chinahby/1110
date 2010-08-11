/*
  ============================================================================

FILE:  AEETU_Yield.c

SERVICES:  
        blocking yeild call

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

void IThread_Yield(IThread *me, IShell *piShell)
{
   AEECallback *pcb = ITHREAD_GetResumeCBK(me);

   ISHELL_Resume(piShell, pcb);
   ITHREAD_Suspend(me);
}


