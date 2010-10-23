/*
  ========================================================================

  FILE:  RootContainer.c
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "AEEStdLib.h"
#include "AEEWProperties.h"
#include "RootContainer.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"

/////////////////////////////////////////////////////////////////
// RootContainer

#define ROOT_FROM_CONTAINER      RootContainer *me = (RootContainer *)po
#define ROOT_FROM_INTERFACE      RootContainer *me = (RootContainer *)po->pMe
#define ROOT_FROM_WIDGET         ROOT_FROM_INTERFACE

#if (defined BREW_BUILD) && (BREW_BUILD < 145)

// version-indepedent function
static boolean IDisplay_IsEnabled(IDisplay *me)
{
#ifndef IDISPLAY_IsEnabled
#define IDISPLAY_IsEnabled(p) \
    ((boolean (*)(IDisplay *)) (((uint32 *)AEEGETPVTBL(p,IDisplay))[22]))(p)
#endif

   return (0x02010000 <= GETAEEVERSION(0,0,0)) ? IDISPLAY_IsEnabled(me) : TRUE;
}

#endif


static void RootContainer_onDraw(RootContainer *me)
{
   IDisplay *piDisplay = 0;

   if (me->piCanvas 
         && SUCCESS == ICANVAS_GetDisplay(me->piCanvas, &piDisplay) 
         && IDisplay_IsEnabled(piDisplay)) {

      AEERect rcDraw;

      SETAEERECT(&rcDraw, me->xDisp + me->rcInvalid.x, me->yDisp + me->rcInvalid.y, me->rcInvalid.dx, me->rcInvalid.dy);
      ICANVAS_SetClipRect(me->piCanvas, &rcDraw);

      DRAWHANDLERDESC_Call(&me->drawdesc, me->piCanvas, me->xDisp, me->yDisp);

      IDISPLAY_UpdateEx(piDisplay,FALSE);
      SETAEERECT(&me->rcInvalid, 0, 0, 0, 0);
   }
   RELEASEIF(piDisplay);
}


static void RootContainer_DefDraw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   IWIDGET_Draw(po, piCanvas, x, y);
}


void RootContainer_SetCanvas(IRootContainer *po, ICanvas *piCanvas, const AEERect *prc)
{
   ROOT_FROM_CONTAINER;
   WExtent we;

   RELEASEIF(me->piCanvas);
   me->piCanvas = piCanvas;
   ADDREFIF(me->piCanvas);

   we.width  = prc->dx;
   we.height = prc->dy;
   ContainerBase_SetExtent(&me->base.base.widget, &we);

   me->xDisp = prc->x;
   me->yDisp = prc->y;

   RootContainer_Invalidate(po, NULL, NULL, 0);
}


int RootContainer_GetCanvas(IRootContainer *po, ICanvas **ppiCanvas, AEERect *prc)
{
   ROOT_FROM_CONTAINER;
   WExtent we;

   *ppiCanvas = me->piCanvas;

   if (!*ppiCanvas) {
      return EFAILED;
   }
   
   ContainerBase_GetExtent(&me->base.base.widget, &we);
   prc->x  = me->xDisp;
   prc->y  = me->yDisp;
   prc->dx = we.width;
   prc->dy = we.height;

   ICANVAS_AddRef(*ppiCanvas);
   return SUCCESS;
}


uint32 RootContainer_Release(IRootContainer *po)
{
   ROOT_FROM_CONTAINER;
   if (CBASE(me)->nRefs == 1) {
      RootContainer_Dtor(me);
   }

   return ContainerBase_Release(IROOTCONTAINER_TO_ICONTAINER(po));
}


int RootContainer_QueryInterface(IRootContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_ROOTCONTAINER || id == AEEIID_ROOTCONTAINER_1) {
      *ppo = po;
      ICONTAINER_AddRef((IContainer*)po);
      return SUCCESS;

   } else if (id == AEEIID_DRAWHANDLER) {
      ROOT_FROM_CONTAINER;
      *ppo = &me->idrawhandler;
      ICONTAINER_AddRef((IContainer*)po);
      return SUCCESS;
   }

   return XYContainer_QueryInterface((IXYContainer*)po, id, ppo);
}


void RootContainer_SetDraw(IDrawHandler *po, DrawHandlerDesc *pDesc)
{
   ROOT_FROM_INTERFACE;

   // Reset on null descriptor
   if (!pDesc) {
      DRAWHANDLERDESC_Init(&me->drawdesc, RootContainer_DefDraw, &me->base.base.widget, 0);
   } else {
      DrawHandlerDesc temp = *pDesc;
      *pDesc = me->drawdesc;
      me->drawdesc = temp;
   }
}

void RootContainer_Dtor(RootContainer *me)
{
   RELEASEIF(me->piShell);
   RELEASEIF(me->piCanvas);
   CALLBACK_Cancel(&me->cbDraw);

   ContainerBase_Dtor(CBASE(me));
}


void RootContainer_Invalidate(IRootContainer *po, IWidget *piw, const AEERect *prc, uint32 dwFlags)
{
   ROOT_FROM_CONTAINER;
   AEERect rcNotify;

   if (piw) {

      if (!ContainerBase_CalcInvalidRect(&me->base.base, piw, prc, dwFlags, &rcNotify)) {
         return;
      }

      if (ISRECTEMPTY(&me->rcInvalid)) {
         me->rcInvalid = rcNotify;
      } else {
         UnionRect(&me->rcInvalid, &me->rcInvalid, &rcNotify);
      }

   } else {
      SETAEERECT(&me->rcInvalid, 0, 0, CBASE(me)->extent.width, CBASE(me)->extent.height);
      rcNotify = me->rcInvalid;
   }

   // If the invalidation rect exists and on the downsweep
   if (!ISRECTEMPTY(&me->rcInvalid) && me->piShell) {

      // if a view model is present, notify listener(s)
      // of invalid rectangle
      if (me->base.base.piViewModel) {
         ModelEvent  mev;
         mev.evCode  = EVT_MDL_ROOTINVAL;
         mev.dwParam = (uint32)(void*)&rcNotify;
         IMODEL_Notify(me->base.base.piViewModel, &mev);
      }

      // this uses SetTimerEx(0) instead of ISHELL_Resume because it has more priority than resume.
      CALLBACK_Init(&me->cbDraw, RootContainer_onDraw, me);
      ISHELL_SetTimerEx(me->piShell, 0, &me->cbDraw);
   }
}


static void RootContainer_InvalidateMe(IRootContainer *po, uint32 dwFlags)
{
   RootContainer_Invalidate(po, NULL, NULL, dwFlags);
}


void RootContainer_doLayout(ContainerBase *me, WidgetNode *pNode, AEERect *prcInvalid)
{
   RootContainer_Invalidate((IRootContainer*)me, (pNode ? pNode->piWidget : NULL), NULL, 0);
}


void RootContainer_Ctor(RootContainer *me, AEEVTBL(IRootContainer) *pvt, IShell *piShell, 
                        IModule *piModule, PFNHANDLER pfnDefHandler)
{
   XYContainer_Ctor(&me->base, (AEEVTBL(IXYContainer) *)pvt, piModule, 
                    pfnDefHandler, RootContainer_doLayout);

   // Border object in root container needs to have a different
   // Invalidate method, namely, RootContainer_InvalidateMe()
   Border_SetPfnInval(&me->base.base.border, (PFNINVALIDATE)RootContainer_InvalidateMe, me);

   // IRootContainer vtbl
   pvt->SetCanvas       = RootContainer_SetCanvas;
   pvt->Release         = RootContainer_Release;
   pvt->QueryInterface  = RootContainer_QueryInterface;
   pvt->Invalidate      = RootContainer_Invalidate;
   pvt->GetCanvas       = RootContainer_GetCanvas;

   // IDrawHandler vtbl
   AEEBASE_INIT(me, idrawhandler, &me->vtDrawHandler);
   me->vtDrawHandler.AddRef         = AEEBASE_AddRef(IDrawHandler);
   me->vtDrawHandler.Release        = AEEBASE_Release(IDrawHandler);
   me->vtDrawHandler.QueryInterface = AEEBASE_QueryInterface(IDrawHandler);
   me->vtDrawHandler.SetDraw        = RootContainer_SetDraw;

   DRAWHANDLERDESC_Init(&me->drawdesc, RootContainer_DefDraw, &me->base.base.widget, 0);

   CBASE(me)->border.bActive = 1;  // we're root, we're always active

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   DynRGB_Ctor(&CBASE(me)->border.dynBG, RGBA_WHITE);
}

int RootContainer_New(IRootContainer **ppo, IShell *piShell, IModule *piModule)
{
   RootContainer *me = MALLOCREC_VTBL(RootContainer,IRootContainer);
   
   *ppo = (IRootContainer *)me;

   if (!me) {
      return ENOMEMORY;
   }

   RootContainer_Ctor(me, GETVTBL(me,IRootContainer), piShell, piModule, 0);
   return SUCCESS;
}
