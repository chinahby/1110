/*
  ========================================================================

  FILE:       SourceAStream.c

  SERVICES:   

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "SourceAStream.h"
#include "wutil.h"

/////////////////////////////////////////////////////////////////////////////
// IAStream wrapper around ISource

// undef for no throttline
///#define THROTTLE_TIME   200

#define DECLARE_ME   SourceAStream *me = (SourceAStream*)pif


typedef struct SourceAStream {
   IAStream          ias;
   ISource *         pis;
   int               nRef;
   AEECallback       cbk;
   PFNREADSTATUS     pfn;
   void *            pv;
   int32             nEndRv;      

   // timer for throttling reads
#if defined(THROTTLE_TIME)
   IShell *          piShell;
   flg               bReadThrottle;
#endif
} SourceAStream;

static void    SourceAStream_Delete(SourceAStream *me);
static uint32  SourceAStream_AddRef(IAStream *p);
static uint32  SourceAStream_Release(IAStream *p);
static void    SourceAStream_Readable(IAStream *p, PFNNOTIFY pfn, void *pcx);
static int32   SourceAStream_Read(IAStream *p, void *pc, uint32 cb);
static void    SourceAStream_Cancel(IAStream *p, PFNNOTIFY pfn, void *pcx);
#if defined(THROTTLE_TIME)
static void    SourceAStream_TimerCB(IAStream *p);
#endif


/////////////////////////////////////////////////////////////////////////////
// Isolate Static vs. Dynamic crap here

#if 0
//#if defined(AEE_STATIC)

/* 
|| Static version
*/
static int SourceAStream_NewVT(SourceAStream **ppo)
{
   static const IAStreamVtbl gvtSourceAStream = {
      SourceAStream_AddRef,
      SourceAStream_Release,
      SourceAStream_Readable,
      SourceAStream_Read,
      SourceAStream_Cancel,
   };
   
   SourceAStream *me = MALLOCREC(SourceAStream);
   *ppo = me;
   if (!me) {
      return ENOMEMORY;
   }

   me->ias.pvt = (IAStreamVtbl*)&gvtSourceAStream;
   return SUCCESS;
}

#else

/* 
|| Dynamic version
*/
static int SourceAStream_NewVT(SourceAStream **ppo)
{
   SourceAStream *me = MALLOCREC_VTBL(SourceAStream, IAStream);
   *ppo = me;
   if (!me) {
      return ENOMEMORY;
   }

   me->ias.pvt = GETVTBL(me, IAStream);

   me->ias.pvt->AddRef     = SourceAStream_AddRef;
   me->ias.pvt->Release    = SourceAStream_Release;
   me->ias.pvt->Readable   = SourceAStream_Readable;
   me->ias.pvt->Read       = SourceAStream_Read;
   me->ias.pvt->Cancel     = SourceAStream_Cancel;

   return SUCCESS;
}
#endif
/////////////////////////////////////////////////////////////////////////////


/* Renamed from SourceAStream_New because of linker conflicts */
IAStream *SourceAStream_Wrap(ISource *pis, PFNREADSTATUS pfn, void * pv, IShell *piShell)
{
   SourceAStream *me = 0;
   
   int nErr = SourceAStream_NewVT(&me);

   if (!nErr) {
      me->nRef    = 1;
      me->pis     = pis;
      me->pfn     = pfn;
      me->pv      = pv;
#if defined(THROTTLE_TIME)
      me->piShell = piShell;
#endif
      ISOURCE_AddRef(pis);
      return &me->ias;
   }

   return NULL;
}

static void SourceAStream_Delete(SourceAStream *me)
{
   CALLBACK_Cancel(&me->cbk);
#if defined(THROTTLE_TIME)
   ISHELL_CancelTimer(me->piShell, (PFNNOTIFY)SourceAStream_TimerCB, me);
#endif
   RELEASEIF(me->pis);
   FREE(me);
}

static uint32  SourceAStream_AddRef(IAStream *pif)
{
   DECLARE_ME;
   return ++me->nRef;
}

static uint32  SourceAStream_Release(IAStream *pif)
{
   DECLARE_ME;
   uint32 u = --me->nRef;
   if (u == 0) {
      SourceAStream_Delete(me);
   }
   return u;
}

#if defined(THROTTLE_TIME)

static void SourceAStream_TimerCB(IAStream *pif)
{
   DECLARE_ME;
   PFNNOTIFY pfn;
   me->bReadThrottle = 0;
   
   if ((PFNNOTIFY)0 != (pfn = me->cbk.pfnNotify)) {
      me->cbk.pfnNotify = 0;
      SourceAStream_Readable(pif, pfn, me->cbk.pNotifyData);
   }
}

static void SourceAStream_Readable(IAStream *pif, PFNNOTIFY pfn, void *pcx)
{ 
   DECLARE_ME;
   CALLBACK_Cancel(&me->cbk);

   me->cbk.pfnNotify   = pfn;
   me->cbk.pNotifyData = pcx;

   if ((PFNNOTIFY)0 == pfn) {
      return;
   }

   if ((ISource *)0 != me->pis) {
      if (me->bReadThrottle) {
         ISHELL_SetTimer(me->piShell, THROTTLE_TIME,
                         (PFNNOTIFY)SourceAStream_TimerCB, me);
      } else {
         ISOURCE_Readable(me->pis,&me->cbk);
      }
   } else {
      ISHELL_Resume(me->piShell,&me->cbk);
   }
}

#else

static void SourceAStream_Readable(IAStream *pif, PFNNOTIFY pfn, void *pcx)
{ 
   DECLARE_ME;
   CALLBACK_Cancel(&me->cbk);

   me->cbk.pfnNotify   = pfn;
   me->cbk.pNotifyData = pcx;

   if ((PFNNOTIFY)0 == pfn) {
      return;
   }
   
   ISOURCE_Readable(me->pis,&me->cbk);
}

#endif


static int32 SourceAStream_Read(IAStream *pif, void *pc, uint32 cb)
{
   DECLARE_ME;
   int32 rv;

   if (!me->pis) {
      return me->nEndRv;
   }

#if defined(THROTTLE_TIME)
   if (me->bReadThrottle) {
      return ISOURCE_WAIT;
   }
#endif

   rv = ISOURCE_Read(me->pis,pc,cb);
   if (me->pfn) {
//      BRIDLE_ASSERT_SAFE_PFN(me->pfn);
      me->pfn(me->pv, rv);
   }

#if defined(THROTTLE_TIME)
   // turn on throttling after every successful read
   me->bReadThrottle = 1;
#endif

   if (0 != cb && 0 == rv || -1 == rv) {
      RELEASEIF(me->pis);
      me->nEndRv = rv;
   }

   return rv;
}

static void SourceAStream_Cancel(IAStream *pif, PFNNOTIFY pfn, void *pcx)
{
   DECLARE_ME;
   CALLBACK_Cancel(&me->cbk);
   me->cbk.pfnNotify = 0;
}


// end IAStream wrapper

