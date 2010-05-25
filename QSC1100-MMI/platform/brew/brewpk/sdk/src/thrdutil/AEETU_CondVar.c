#include "AEEThreadUtil.h"
#include "AEEStdLib.h"

static void AEECondVarTimedWaitCancel(AEECallback *pcb)
{
   AEECondVar *me = (AEECondVar *)pcb->pCancelData;

   /* turn off cancel */
   pcb->pfnCancel = 0;

   {
      AEECallback  *p;
      AEECallback **lfp;

      for (lfp = &me->pcbWaiting; (AEECallback *)0 != (p = *lfp); 
           lfp = &p->pNext) {

         if (p == pcb) {
            *lfp = p->pNext;
            break;
         }
      }
   }
}

static void AEECondVarTimedWaitTimeout(void *p)
{
   AEECallback *pcb = (AEECallback *)p;

   AEECondVarTimedWaitCancel(pcb);

   /* set the return value */
   *(int *)pcb->pmc = EEXPIRED;

   /* resume the thread, we're already in a timer 
      ('bout as high as we can get on the stack) */
   pcb->pfnNotify(pcb->pNotifyData);
}

int IThread_CondVarTimedWait(IThread *me, AEECondVar *pcv, AEEMutex *pmt,
                             IShell *piShell, int nMsecs)
{
   AEECallback **lfp;
   AEECallback  *pcb = ITHREAD_GetResumeCBK(me);
   int           nRv = SUCCESS;
   AEECallback   cbTimeout;

   ZEROAT(&cbTimeout);

   CALLBACK_Cancel(pcb);

   {
      AEECallback *p;
      for (lfp = &pcv->pcbWaiting; (AEECallback *)0 != (p = *lfp); 
           lfp = &p->pNext);
   }

   *lfp = pcb;

   pcb->pNext       = 0;
   pcb->pfnCancel   = AEECondVarTimedWaitCancel;
   pcb->pCancelData = pcv;
   pcb->pmc         = &nRv;

   if (nMsecs != -1) {
      CALLBACK_Init(&cbTimeout,AEECondVarTimedWaitTimeout,pcb);
      ISHELL_SetTimerEx(piShell,nMsecs,&cbTimeout);
   }

   IThread_MutexRelease(me,pmt,piShell);

   ITHREAD_Suspend(me);

   CALLBACK_Cancel(&cbTimeout);

   IThread_MutexGrab(me,pmt);
   
   return nRv;
}

int AEECondVar_Signal(AEECondVar *me, IShell *piShell)
{
   AEECallback *pcb;

   if ((AEECallback *)0 != (pcb = me->pcbWaiting)) {
      ISHELL_Resume(piShell, pcb);
   }

   return SUCCESS;
}

int AEECondVar_Broadcast(AEECondVar *me, IShell *piShell)
{
   AEECallback *pcb;
   
   if ((AEECallback *)0 != (pcb = me->pcbWaiting)) {
      while ((AEECallback *)0 != (pcb = me->pcbWaiting)) {
         ISHELL_Resume(piShell, pcb);
      }
   }

   return SUCCESS;
}
