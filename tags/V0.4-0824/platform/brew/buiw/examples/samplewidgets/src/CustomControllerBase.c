/*
  ========================================================================

  FILE:  ControllerBase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation for controllers

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "ControllerBase.h"
#include "wutil.h"

#define DECLARE_ME  ControllerBase *me = (ControllerBase*)po


uint32 ControllerBase_Release(IController *po)
{
   DECLARE_ME;
   
   uint32 nRefs = --me->nRefs;

   if (nRefs == 0) {
      HANDLERDESC_Free(&me->hd);
      RELEASEIF(me->piModule);
      FREE(me);
   }
   return nRefs;
}


uint32 ControllerBase_AddRef(IController *po)
{
   DECLARE_ME;
   return ++me->nRefs;
}

int ControllerBase_QueryInterface(IController *po, AEECLSID clsid, void **ppo)
{
   if (clsid == AEECLSID_QUERYINTERFACE ||
       clsid == AEEIID_HANDLER ||
       clsid == AEEIID_CONTROLLER) {
      *ppo = (void*)po;
      ControllerBase_AddRef(po);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}


boolean ControllerBase_DefHandleEvent(IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   return FALSE;
}


static boolean ControllerBase_HandleEvent(IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECLARE_ME;
   return HANDLERDESC_Call(&me->hd, evt, wParam, dwParam);
}

void ControllerBase_SetHandler(IController *po, HandlerDesc *pDesc)
{
   DECLARE_ME;

   if (!pDesc) {
      HANDLERDESC_Init(&me->hd, me->pfnDefHandler, me, 0);
   } else {
      HandlerDesc hdOld = me->hd;
      me->hd = *pDesc;
      *pDesc = hdOld;
   }
}

int ControllerBase_SetWidget(IController *po, IWidget *piw)
{
   DECLARE_ME;

   me->piw = piw;  // don't AddRef !
   return SUCCESS;
}


void ControllerBase_Ctor(ControllerBase *me, AEEVTBL(IController) *pvt, 
                         IModule *piModule, PFNHANDLER pfnDefHandler)
{
   me->nRefs = 1;

   me->piModule = piModule;
   ADDREFIF(piModule);

   me->pvt = pvt;

   #define VTFUNC(name)   pvt->name = ControllerBase_##name
   VTFUNC( AddRef );
   VTFUNC( Release );
   VTFUNC( QueryInterface );
   VTFUNC( HandleEvent );
   VTFUNC( SetHandler );
   VTFUNC( SetWidget );

   me->pfnDefHandler = pfnDefHandler ? pfnDefHandler 
                                     : (PFNHANDLER)ControllerBase_DefHandleEvent;

   ControllerBase_SetHandler((IController*)me, NULL);
}





