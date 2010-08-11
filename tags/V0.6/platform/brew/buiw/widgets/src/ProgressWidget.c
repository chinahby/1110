/*
  ========================================================================

  FILE:  ProgressWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ProgressWidget implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ProgressWidget.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"

#define ME_FROM_WIDGET ProgressWidget *me = (ProgressWidget *)po

#define DEFAULT_PROGRESS_COLOR MAKE_RGB(192, 192, 192);


uint32 ProgressWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      LISTENER_Cancel(&me->modelListener);
      ISHELL_Release(me->piShell);
      CALLBACK_Cancel(&me->cbkAuto);
      Border_Dtor(&me->border);
   }

   return WidgetBase_Release(po);
}

void ProgressWidget_MakeProgress(ProgressWidget *me)
{
   me->nIndeterminate++;
   WidgetBase_Invalidate((IWidget*)me, ICIF_REDRAW);

   if (me->bStepEnabled) {
      IMODEL_StepNotify(WBASE(me)->piViewModel, (me->bFinalStep ? EVT_STEP_FINAL : 0) | EVT_STEP_REPEAT, me->nCurStep, me->nScrollRep);
   }

   if (me->bFinalStep) {
      ++me->nScrollRep;
      me->nCurStep = 0;
      me->bFinalStep = FALSE;
   }
   else {
      ++me->nCurStep;
   }
}

void ProgressWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   IDisplay *piDisplay = 0;

   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      AEERect rc;
      int val = 100;

      if (me->base.piModel) {
         val = (int)IVALUEMODEL_GetValue((IValueModel *)me->base.piModel,NULL);
         val = MIN(100, val);
      }
   
      if (val < 0) {
   
#define SEGMENTS  3
#define STEPS     7

         int dx = me->border.rcClient.dx;
         int dxSeg = dx / STEPS;
         int pos = me->nIndeterminate % (STEPS + SEGMENTS+1) - SEGMENTS;
         int xOffset = pos * dxSeg;
         int nGap = MAX(1, me->cyFont / 20);
         int i;

         SETAEERECT(&rc, x + me->border.rcClient.x + xOffset, y + me->border.rcClient.y, 
                    dxSeg-nGap, me->border.rcClient.dy);

         for (i=0; i < SEGMENTS; i++) {
            IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbProgress), IDF_RECT_FILL);
            rc.x += dxSeg;
         }
   
         if (pos == STEPS){
            me->bFinalStep = TRUE;
         }

         CALLBACK_Timer(&me->cbkAuto, ProgressWidget_MakeProgress, me, me->piShell, 150);

      } else {

         CALLBACK_Cancel(&me->cbkAuto);
   
         SETAEERECT(&rc, x + me->border.rcClient.x, y + me->border.rcClient.y, 
                    (me->border.rcClient.dx * val) / 100,
                    me->border.rcClient.dy);
         IDISPLAY_DrawRect(piDisplay, &rc, RGB_NONE, RGBA_STRIPALPHA(me->rgbProgress), IDF_RECT_FILL);
      }
   
      IDISPLAY_Release(piDisplay);
   }
}

void ProgressWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;

   pweOut->height = me->cyFont  / 2;
   pweOut->width = (((pweOut->height * 10) / STEPS) * STEPS) - 1;

   Border_CalcPreferredExtent(&me->border, pweOut, pweOut);
}

void ProgressWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;
   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   Border_CalcClientRect(&me->border);
}

int ProgressWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;
   IValueModel *piValueModel;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, 
                                        (void **)&piValueModel)) {
      // Not a valuemodel
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(po, piModel);
   IVALUEMODEL_Release(piValueModel);

   // Re-register listener
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Init(&me->modelListener, WidgetBase_InvalidateContent, po);
   IMODEL_AddListener(piModel, &me->modelListener);

   WidgetBase_InvalidateExtent(po);

   return 0;
   
}

boolean ProgressWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }
   
   switch(evt) {
   case EVT_WDG_SETPROPERTY:
      switch(wParam) {
      case PROP_FGCOLOR:
         me->rgbProgress = (RGBVAL)dwParam;
         WidgetBase_InvalidateExtent(po);
         return TRUE;
      case PROP_ANIMATE_FLAGS:
         me->bStepEnabled = (dwParam & AF_ENABLE_EVT_STEP);
         return TRUE;
      }

   case EVT_WDG_GETPROPERTY:
      switch(wParam) {
      case PROP_FGCOLOR:
         *(RGBVAL *)dwParam = me->rgbProgress;
         return TRUE;
      case PROP_ANIMATE_FLAGS:
         if (dwParam) {
            *((uint32*)dwParam) = 0;
            if (me->bStepEnabled) {
               *((uint32*)dwParam) |= AF_ENABLE_EVT_STEP;
            }
            return TRUE;
         }
      }
   }
      
   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}
   


void ProgressWidget_Ctor(ProgressWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, 
                         IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(ProgressWidget_HandleEvent));

   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, FALSE, &WBASE(me)->piViewModel);

   pvt->Release             = ProgressWidget_Release;
   pvt->Draw                = ProgressWidget_Draw;
   pvt->GetPreferredExtent  = ProgressWidget_GetPreferredExtent;
   pvt->SetExtent           = ProgressWidget_SetExtent;
   pvt->SetModel            = ProgressWidget_SetModel;

   me->rgbProgress = DEFAULT_PROGRESS_COLOR;
   IWIDGET_SetPadding(PROGRESSWIDGET_TO_IWIDGET(me), 1);

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);
   
   me->cyFont = 10;
   // get normal font size to base preferred extent upon
   {
      IFont *piFont = 0;
      if (SUCCESS == ISHELL_CreateFont(me->piShell, AEECLSID_FONTSYSNORMAL, &piFont)) {
         AEEFontInfo ai;
         IFONT_GetInfo(piFont, &ai, sizeof(ai));
         me->cyFont = ai.nAscent + ai.nDescent;
         IFONT_Release(piFont);
      }
   }

   me->bFinalStep = FALSE;

   ProgressWidget_SetModel(PROGRESSWIDGET_TO_IWIDGET(me), IVALUEMODEL_TO_IMODEL(piModel));
}


/////////////////////////////////////////////////////////////////
// Concrete Implementation

int ProgressWidget_New(IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule)
{
   int result = 0;
   ProgressWidget *me = MALLOCREC_VTBL(ProgressWidget, IWidget);

   if (!me)
      result = ENOMEMORY;

   if (result == 0) {
      if (piModel == 0) 
         result = ValueModel_New(&piModel, piModule);
      else
         IVALUEMODEL_AddRef(piModel);
   }

   if (result == 0) {
      ProgressWidget_Ctor(me, GETVTBL(me, IWidget), piModel, piShell, piModule, 0);
      *ppo = PROGRESSWIDGET_TO_IWIDGET(me);
   } else {
      *ppo = 0;
      FREEIF(me);
   }

   RELEASEIF(piModel);

   return result;
}


