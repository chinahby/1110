/*======================================================
FILE:       BorderWidget.c

SERVICES:   Widget that draws borders and/or backgrounds

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdLib.h"

#include "BorderWidget.h"
#include "AEEWProperties.h"
#include "wutil.h"



//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------

// type-checked conversions
static __inline BorderWidget *DECORATOR_TO_BORDERWIDGET(IDecorator *po) {
   return (BorderWidget*)po;
}
static __inline BorderWidget *CONTAINER_TO_BORDERWIDGET(IContainer *po) {
   return (BorderWidget*)po->pMe;
}

#define BORDER_FROM_DECORATOR BorderWidget *me = DECORATOR_TO_BORDERWIDGET(po)
#define BORDER_FROM_CONTAINER BorderWidget *me = CONTAINER_TO_BORDERWIDGET(po)


static __inline void BorderWidget_SetExtentFromChild(IDecorator *po)
{
   BORDER_FROM_DECORATOR;
   WExtent we, weChild;
   
   Decorator_GetExtent(po, &weChild);
   Border_CalcPreferredExtent(&me->border, &we, &weChild);
   WidgetBase_SetExtent(IDECORATOR_TO_IWIDGET(po), &we);
}


void BorderWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   BORDER_FROM_DECORATOR;
   WExtent ce;

   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);
   
   ce.width = me->border.rcClient.dx;
   ce.height = me->border.rcClient.dy;
   Decorator_SetExtent(po, &ce);
}

void BorderWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   BORDER_FROM_DECORATOR;
   WExtent we;

   Decorator_SetWidget(po, piChild);
   
   Decorator_GetExtent(po, &we);
   Border_CalcPreferredExtent(&me->border, &we, &we);

   WidgetBase_SetExtent((IWidget*)&me->base.base, &we);
   Border_CalcClientRect(&me->border);
}

void BorderWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   // Don't ask child, we control the extent
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pe);
}


void BorderWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   BORDER_FROM_DECORATOR;
   WExtent ce;

   Decorator_GetPreferredExtent(po, &ce);

   Border_CalcPreferredExtent(&me->border, pweOut, &ce);
}

void BorderWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   BORDER_FROM_DECORATOR;

   Border_Draw(&me->border, piCanvas, x, y);
   
   if (me->base.pChild) {
      AEERect rc, rcClip;
      rc = me->border.rcClient;
      rc.x += x;
      rc.y += y;
   
      ICANVAS_GetClipRect(piCanvas, &rcClip);
      IntersectRect(&rcClip, &rcClip, &rc);
      ICANVAS_SetClipRect(piCanvas, &rcClip);
   
      Decorator_Draw(po, piCanvas, rc.x, rc.y);
   }
}

boolean BorderWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, const AEERect *prcIn)
{
   BORDER_FROM_DECORATOR;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}

boolean BorderWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   BORDER_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave = *prcClient;

   // if we have a child, let it handle these focus events first
   if (me->base.pChild && 
       evt == EVT_WDG_HASFOCUS || evt == EVT_WDG_CANTAKEFOCUS) {
      return Decorator_HandleEvent(po, evt, wParam, dwParam);
   }

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         // Border extent has changed. Tell the child its extent again
         WExtent ce;
         SETWEXTENT(&ce, me->border.rcClient.dx, me->border.rcClient.dy);
         Decorator_SetExtent(po, &ce);
      }
      return TRUE;
   }

   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}

/////////////////////////////////////////////////////////////////
// Container methods

void BorderWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags)
{
   BORDER_FROM_CONTAINER;
   Decorator_ClientInvalidate((IDecorator*)me, &me->border.rcClient, prcIn, dwFlags);
}

int BorderWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   BORDER_FROM_CONTAINER;
   return Decorator_ClientLocate((IDecorator*)me, &me->border.rcClient, piw, ppRoot, prcRel);
}

/////////////////////////////////////////////////////////////////

uint32 BorderWidget_Release(IDecorator *po)
{
   BORDER_FROM_DECORATOR;

   if (me->base.base.nRefs == 1) {
      BorderWidget_Dtor(me);
   }
   return Decorator_Release(po);
}

void BorderWidget_Dtor(BorderWidget *me)
{
   Border_Dtor(&me->border);
   Decorator_Dtor(&me->base);
}

void BorderWidget_Ctor(BorderWidget *me, AEEVTBL(IDecorator) *pvt, 
                       IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(BorderWidget_HandleEvent));

   pvt->Release            = BorderWidget_Release;
   pvt->Draw               = BorderWidget_Draw;
   pvt->IntersectOpaque    = BorderWidget_IntersectOpaque;
   pvt->GetPreferredExtent = BorderWidget_GetPreferredExtent;
   pvt->GetExtent          = BorderWidget_GetExtent;
   pvt->SetExtent          = BorderWidget_SetExtent;
   pvt->SetWidget          = BorderWidget_SetWidget;

   me->base.vtContainer.Invalidate = BorderWidget_Invalidate;
   me->base.vtContainer.Locate = BorderWidget_Locate;
   
   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.base.extent, FALSE, &WBASE(me)->piViewModel);

   me->border.nWidth[RGBINDEX_ACTIVE]    = 1;
   me->border.nWidth[RGBINDEX_INACTIVE]  = 1;
}

int BorderWidget_New(IDecorator **ppo, IModule *piModule)
   {
   BorderWidget *me = MALLOCREC_VTBL(BorderWidget, IDecorator);

   if (!me)
      return ENOMEMORY;

   BorderWidget_Ctor(me, GETVTBL(me, IDecorator), piModule, 0);
   *ppo = BORDERWIDGET_TO_IDECORATOR(me);

   return 0;
}

/////////////////////////////////////////////////////////////////
// Utility functions

int IWidget_WrapInBorder(IWidget **ppioChild, IModule *piModule)
{
   int result;
   IDecorator *piBorder;

   result = BorderWidget_New(&piBorder, piModule);
   
   if (result == 0) {
      IDECORATOR_SetWidget(piBorder, *ppioChild);
      IWIDGET_Release(*ppioChild);
      *ppioChild = IDECORATOR_TO_IWIDGET(piBorder);
   }

   return result;
}

