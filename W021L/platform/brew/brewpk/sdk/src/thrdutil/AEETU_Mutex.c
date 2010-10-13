/*
  ============================================================================

FILE:  AEETU_Mutex.c

SERVICES:  
        <fill me in!>

GENERAL DESCRIPTION:
        <fill me in!>

REVISION HISTORY: 
        Fri Nov 16 17:50:18 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/

#include "AEEThreadUtil.h"

int IThread_MutexRelease(IThread *me, AEEMutex *pmt, IShell *piShell)
{
   if (me != pmt->pitHolding) {
      return EFAILED;
   }

   if ((AEECallback *)0 != pmt->pcbWaiting) {
      AEECallback *pcb = pmt->pcbWaiting;

      pmt->pcbWaiting = pcb->pNext;
      pcb->pfnCancel   = 0;  /* not in waiting list anymore */
    
      pmt->pitHolding = (IThread *)pcb->pReserved;
      ISHELL_Resume(piShell, pcb);

   } else {
      pmt->pitHolding = 0;
   }

   return SUCCESS;
}

static void AEEMutexGrabCancel(AEECallback *pcb)
{
   AEEMutex *me = (AEEMutex *)pcb->pCancelData;
   
   /* turn off cancel */
   pcb->pfnCancel = 0;

   {
      AEECallback **lfp;
      AEECallback  *p;
      for (lfp = &me->pcbWaiting; (AEECallback *)0 != (p = *lfp); 
           lfp = &p->pNext) {

         if (p == pcb) {
            *lfp = p->pNext;
            break;
         }
      }
   }
}

int IThread_MutexGrab(IThread *me, AEEMutex *pmt)
{
   if ((IThread *)0 != pmt->pitHolding) {
      AEECallback **lfp;
      AEECallback  *pcb = ITHREAD_GetResumeCBK(me);
      
      CALLBACK_Cancel(pcb);

      {
         AEECallback *p;
         for (lfp = &pmt->pcbWaiting; (AEECallback *)0 != (p = *lfp); 
              lfp = &p->pNext);
      }

      *lfp = pcb;

      pcb->pNext       = 0;
      pcb->pfnCancel   = AEEMutexGrabCancel;
      pcb->pCancelData = pmt;
      pcb->pReserved   = me;

      ITHREAD_Suspend(me);
   } else {
      pmt->pitHolding = me;
   }

   return SUCCESS;
}
