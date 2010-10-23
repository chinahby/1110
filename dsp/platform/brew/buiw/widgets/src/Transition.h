/*
  ========================================================================

  FILE:  Transition.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of transitions

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "AEETransition.h"

typedef struct Transition
{
   const AEEVTBL(ITransition) *pvt;

   uint32      nRefs;
   IModule *   piModule;

   AEECLSID    clsid;
   IWidget *   piWidget;

   long        totalTime;
   long        startTime;
   long        elapsedTime;

   AEECallback timer;
   IShell *    piShell;
   
   PFNNOTIFY   pfnStep;

   PFNNOTIFY   pfnDone;
   void *      pfnCxt;   
   AEECallback cbkStop;
} Transition;

void Transition_Ctor         (Transition *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule);
void Transition_Dtor         (Transition *me);

uint32  Transition_AddRef        (ITransition *po);
uint32  Transition_Release       (ITransition *po);
int     Transition_QueryInterface(ITransition *po, AEECLSID clsid, void **ppNew);
void    Transition_Start         (ITransition *po, TransitionDesc *pDesc, long msecs, PFNNOTIFY pfnDone, void *pfnCxt);
void    Transition_Stop          (ITransition *po);
boolean Transition_IsRunning     (ITransition *po);

static __inline ITransition *TRANSITION_TO_ITRANSITION(Transition *me) {
   return (ITransition *)me;
}


typedef struct
{
   Transition base;

   uint8 start;
   uint8 end;
} Fader;

void Fader_Start(ITransition *po, TransitionDesc *pDesc, long msecs, PFNNOTIFY pfnDone, void *pCxt);
void Fader_Ctor(Fader *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule);
int  Fader_New(ITransition **ppo, IShell *piShell, IModule *piModule);

static __inline Transition *FADER_TO_TRANSITION(Fader *me) {
   return (Transition *)me;
}

typedef struct
{
   Transition base;

   IXYContainer *piContainer;
   struct {
      int16 x;
      int16 y;
   } start;
   struct {
      int16 x;
      int16 y;
   } end;

} Mover;

void Mover_Ctor(Mover *me, AEEVTBL(ITransition) *pvt, AEECLSID clsid, IShell *piShell, IModule *piModule);
void Mover_Dtor(Mover *me);
int  Mover_New(ITransition **ppo, IShell *piShell, IModule *piModule);

static __inline Transition *MOVER_TO_TRANSITION(Mover *me) {
   return (Transition *)me;
}


#endif /* __TRANSITION_H__ */
