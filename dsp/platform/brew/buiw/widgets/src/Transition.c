/*
  ========================================================================

  FILE:  Transition.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Transition implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "Transition.h"
#include "AEEWProperties.h"
#include "wutil.h"

#define TIMERPERIOD 30

#define DECLARE_ME(c) c *me = (c *)po

static __inline void Transition_Schedule(Transition *me)
{
   if (!CALLBACK_IsQueued(&me->timer))
      ISHELL_SetTimerEx(me->piShell, MIN(TIMERPERIOD, me->totalTime), &me->timer);
}

static void Transition_CbkStop(Transition *me)
{
   ITRANSITION_Stop((ITransition *)me);
   if (me->pfnDone)
      me->pfnDone(me->pfnCxt);
}
static void Transition_Step(ITransition *po)
{
   DECLARE_ME(Transition);
   long now = GETUPTIMEMS();

   me->elapsedTime = now - me->startTime;

   //
   // Invoke the transition step function before checking if 
   // the transition has exceeded its totalTime.  
   //
   // This means that one transition step will occur after
   // totalTime.  This will ensure that the transition will
   // ALWAYS complete (Mover always moves the Widget to 
   // the final position, Fader always sets the Widget's final
   // transparency)
   //
   if (me->pfnStep) 
      me->pfnStep(me);

   if (me->elapsedTime >= me->totalTime) {
      //defer calling of this.  RootContainer uses a cbk to draw the widgets
      //after invalidation.  Ergo, we must wait until after the RootContainer
      //redraws itself, else the last step won't be visible if the widget is a 
      //temporary one that gets removed when the transition is done
      CALLBACK_Resume(&me->cbkStop,Transition_CbkStop,me,me->piShell);
      return;
   } else {
      Transition_Schedule(me);
   }
}

void Transition_Start(ITransition *po, TransitionDesc *pDesc, long msecs, PFNNOTIFY pfnDone, void *pfnCxt)
{
   DECLARE_ME(Transition);

   if (Transition_IsRunning(po))
      Transition_Stop(po);

   RELEASEIF(me->piWidget);
   me->piWidget = pDesc->piWidget;
   ADDREFIF(me->piWidget);

   me->pfnDone   = pfnDone;
   me->pfnCxt    = pfnCxt;
   me->totalTime = msecs;

   me->startTime = GETUPTIMEMS();
   Transition_Step(po);
}

void Transition_Stop(ITransition *po)
{
   DECLARE_ME(Transition);
   CALLBACK_Cancel(&me->timer);
   CALLBACK_Cancel(&me->cbkStop);
}

boolean Transition_IsRunning(ITransition *po)
{
   DECLARE_ME(Transition);
   return CALLBACK_IsQueued(&me->timer) || CALLBACK_IsQueued(&me->cbkStop);
}


void Transition_Ctor(Transition *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);
   
   me->piModule = piModule;
   ADDREFIF(piModule);

   me->pvt = pvt;
   me->clsid = clsid;
   me->nRefs = 1;

   pvt->AddRef         = Transition_AddRef;
   pvt->Release        = Transition_Release;
   pvt->QueryInterface = Transition_QueryInterface;
   pvt->Start          = Transition_Start;
   pvt->Stop           = Transition_Stop;
   pvt->IsRunning      = Transition_IsRunning;

   CALLBACK_Init(&me->timer, Transition_Step, me);
}

void Transition_Dtor(Transition *me)
{
   Transition_Stop(TRANSITION_TO_ITRANSITION(me));

   RELEASEIF(me->piWidget);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piModule);
}

uint32 Transition_AddRef(ITransition *po)
{
   DECLARE_ME(Transition);
   
   return ++me->nRefs;
}

uint32 Transition_Release(ITransition *po)
{
   DECLARE_ME(Transition);
   
   uint32 nRefs = --me->nRefs;

   if (nRefs == 0) {
      Transition_Dtor(me);
      FREE(me);
   }
   return nRefs;
}

int Transition_QueryInterface(ITransition *po, AEECLSID clsid, void **ppNew)
{
   DECLARE_ME(Transition);

   if (clsid == AEECLSID_QUERYINTERFACE ||
       clsid == AEEIID_TRANSITION ||
       clsid == me->clsid) {

      *ppNew = (void*)po;
      Transition_AddRef(po);
      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}



/////////////////////////////////////////////////////////////////
// Fader 

#define DECLARE_FADER Fader *me = (Fader *)po

static void Fader_Step(Fader *me)
{
   int range = me->end - me->start;
   int val;

   if (me->base.elapsedTime >= me->base.totalTime)
      val = me->end;
   else
      val = me->start + (range * me->base.elapsedTime) / me->base.totalTime;

   IWIDGET_SetProperty(me->base.piWidget, PROP_TRANSPARENCY, val);
}

void Fader_Start(ITransition *po, TransitionDesc *pDesc, long msecs, PFNNOTIFY pfnDone, void *pfnCxt)
{
   DECLARE_FADER;
   FaderDesc *pd = (FaderDesc *)pDesc;

   me->start = pd->start;
   me->end = pd->end;
   
   Transition_Start(po, pDesc, msecs, pfnDone, pfnCxt);
}

void Fader_Ctor(Fader *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule)
{
   Transition_Ctor(&me->base, pvt, clsid, piShell, piModule);

   pvt->Start = Fader_Start;

   me->base.pfnStep = (PFNNOTIFY)Fader_Step;
}

int Fader_New(ITransition **ppo, IShell *piShell, IModule *piModule)
{
   Fader *me = MALLOCREC_VTBL(Fader, ITransition);

   if (!me) {
      return ENOMEMORY;
   }

   *ppo = (ITransition *)me;

   Fader_Ctor(me, GETVTBL(me, ITransition), AEECLSID_FADER, piShell, piModule);

   return SUCCESS;
}



/////////////////////////////////////////////////////////////////
// Mover 

static void Mover_Step(Mover *me)
{
   int xrange = me->end.x - me->start.x;
   int yrange = me->end.y - me->start.y;

   WidgetPos pos;

   IXYCONTAINER_GetPos(me->piContainer, me->base.piWidget, &pos);

   if (me->base.elapsedTime >= me->base.totalTime) {
      pos.x = me->end.x;
      pos.y = me->end.y;
   } else {
      pos.x = me->start.x + (xrange * me->base.elapsedTime) / me->base.totalTime;
      pos.y = me->start.y + (yrange * me->base.elapsedTime) / me->base.totalTime;
   }

   IXYCONTAINER_SetPos(me->piContainer, me->base.piWidget, WIDGET_ZNORMAL, &pos);
}

void Mover_Start(ITransition *po, TransitionDesc *pDesc, long msecs, PFNNOTIFY pfnDone, void *pfnCxt)
{
   DECLARE_ME(Mover);
   MoverDesc *pd = (MoverDesc *)pDesc;

   me->start.x = pd->start.x;
   me->start.y = pd->start.y;
   me->end.x   = pd->end.x;
   me->end.y   = pd->end.y;

   me->piContainer = pd->piContainer;
   IXYCONTAINER_AddRef(me->piContainer);
   
   Transition_Start(po, pDesc, msecs, pfnDone, pfnCxt);   
}

uint32 Mover_Release(ITransition *po)
{
   DECLARE_ME(Mover);

   if (me->base.nRefs == 1) {
      Mover_Dtor(me);
   }

   return Transition_Release(po);
}

void Mover_Ctor(Mover *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule)
{
   Transition_Ctor(&me->base, pvt, clsid, piShell, piModule);

   pvt->Start   = Mover_Start;
   pvt->Release = Mover_Release;

   me->base.pfnStep = (PFNNOTIFY)Mover_Step;
}


void Mover_Dtor(Mover *me)
{
   RELEASEIF(me->piContainer);
   Transition_Dtor(&me->base);
}

int Mover_New(ITransition **ppo, IShell *piShell, IModule *piModule)
{
   Mover *me = MALLOCREC_VTBL(Mover, ITransition);

   if (!me) {
      return ENOMEMORY;
   }

   *ppo = (ITransition *)me;

   Mover_Ctor(me, GETVTBL(me, ITransition), AEECLSID_MOVER, piShell, piModule);

   return SUCCESS;
}
