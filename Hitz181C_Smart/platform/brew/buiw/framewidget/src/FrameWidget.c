/*======================================================
FILE: FrameWidget.c

SERVICES: 
   IWidget implementation for the frame widget

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "AEEDisplayCanvas.h"
#include "AEEFrameModel.h"
#include "AEEDirectMode.h"
#include "WidgetBase.h"
#include "FrameWidget.h"
#include "wutil.h"
#include "AEEIID_FRAMEMODEL.BID"
#include "AEERootContainer.h"

#define ME_FROM_WIDGET  FrameWidget *me = (FrameWidget*)po
#define MYMODEL(p)      (IFrameModel*)((p)->base.piModel)


typedef struct {

   WidgetBase     base;
   ModelListener  ml;
   IBitmap *      piFrame;
   RGBVAL         rgbBackground;
   uint32         dwAnimFlags;   
   int            nStepCount;
} FrameWidget;


// Prototypes
void     FrameWidget_Ctor               (FrameWidget *me, AEEVTBL(IWidget) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler);
uint32   FrameWidget_Release            (IWidget *po);
boolean  FrameWidget_HandleEvent        (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     FrameWidget_GetPreferredExtent (IWidget *po, WExtent *pweOut);
void     FrameWidget_SetExtent          (IWidget *po, WExtent *pe);
void     FrameWidget_Draw               (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  FrameWidget_IntersectOpaque    (IWidget *po, AEERect *prcOut, const AEERect *prcIn);
int      FrameWidget_SetModel           (IWidget *po, IModel *piModel);


static int FrameWidget_SetDirectMode(FrameWidget *me, int nDirect);
static int FrameWidget_GetDirectMode(FrameWidget *me, int* pnDirect);
static __inline void FrameWidget_Invalidate(FrameWidget *me) {
   WidgetBase_Invalidate((IWidget*)me, ICIF_REDRAW);
}


static void FrameWidget_ModelChanged(FrameWidget *me, ModelEvent *pEvent)
{
   
   if (EVT_FRAMEMDL_FRAME == pEvent->evCode) {      
      IBitmap * piNewFrame = NULL;
      IBitmap * piOldFrame = me->piFrame;
      IFRAMEMODEL_GetFrame(MYMODEL(me), &piNewFrame);
      me->piFrame = piNewFrame;
      RELEASEIF(piOldFrame);

      // Invalidate even if there's no frame, in case we need to clear the display
      FrameWidget_Invalidate(me);

      // if EVT_STEP events are enabled, send the event
      if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
         IMODEL_StepNotify(WBASE(me)->piViewModel, 0, me->nStepCount++, 0);
      }

   }
}


void FrameWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;

   if (MYMODEL(me)) {
      IFRAMEMODEL_SetSize(MYMODEL(me), pe->width, pe->height);
   }

   WidgetBase_SetExtent(po, pe);
}


int FrameWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;
   IModel *piModelTemp = 0;
   int nErr;
   IBitmap *piNewFrame = NULL;
   IBitmap *piOldFrame = me->piFrame;

   if (!piModel) {
	   RELEASEIF(me->piFrame);
	   LISTENER_Cancel(&me->ml);
      (void)WidgetBase_SetModel(po, piModel);
      FrameWidget_Invalidate(me);
      return SUCCESS;
   } 

   nErr = IMODEL_QueryInterface(piModel, AEEIID_FRAMEMODEL, (void **)&piModelTemp);
   RELEASEIF(piModelTemp);

   if (!nErr) {

      WidgetBase_SetModel(po, piModel);
      IMODEL_AddListenerEx(piModel, &me->ml, (PFNLISTENER)FrameWidget_ModelChanged, me);

      if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
         me->nStepCount = 0;
         IMODEL_StepNotify(WBASE(me)->piViewModel, EVT_STEP_PRE, me->nStepCount, 0);
      }

      if (MYMODEL(me)) {
         IFRAMEMODEL_SetSize(MYMODEL(me), me->base.extent.width, me->base.extent.height);
      }



      // We could pass in the current frame pointer instead, but that
      // risks a potential FrameModel implementation that might
      // release/free it without us knowing. To be safe we'll just pass
      // a NULL pointer and assign the new frame pointer ourselves.
      (void)IFRAMEMODEL_GetFrame(MYMODEL(me), &piNewFrame);
      me->piFrame = piNewFrame;
      RELEASEIF(piOldFrame);
      FrameWidget_Invalidate(me);
   }

   return nErr;
}


void FrameWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   IBitmap *pibDest = 0;
   
   if (SUCCESS == ICANVAS_GetBitmap(piCanvas, &pibDest)) {

      AEERect rcClip;

      if (SUCCESS != ICANVAS_GetClipRect(piCanvas, &rcClip)) {
         SETAEERECT(&rcClip, 0, 0, me->base.extent.width, me->base.extent.height);
      }

      // Fill background
      if (me->rgbBackground != RGB_NONE) {

         AEERect rc;
         SETAEERECT(&rc, x, y, me->base.extent.width, me->base.extent.height);
         IntersectRect(&rc, &rc, &rcClip);
         IBITMAP_FillRect(pibDest, &rc, IBITMAP_RGBToNative(pibDest, me->rgbBackground), AEE_RO_COPY);
      }

      // Draw image
      if (me->piFrame) {

         AEERect rc;
         AEEBitmapInfo bi;
         int cx, cy;

         IBITMAP_GetInfo(me->piFrame, &bi, sizeof(AEEBitmapInfo));

         cx = MIN((int)bi.cx, me->base.extent.width);
         cy = MIN((int)bi.cy, me->base.extent.height);

         // Center bitmap
         if ((int)bi.cx < me->base.extent.width) {
            x += (me->base.extent.width - (int)bi.cx) / 2;
         }
         if ((int)bi.cy < me->base.extent.height) {
            y += (me->base.extent.height - (int)bi.cy) / 2;
         }

         SETAEERECT(&rc, x, y, cx, cy);
         IntersectRect(&rc, &rc, &rcClip);

         IBITMAP_BltIn(pibDest, rc.x, rc.y, rc.dx, rc.dy, me->piFrame, 0, 0, AEE_RO_COPY);
      }
   }

   RELEASEIF(pibDest);
}


void FrameWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{   
   ME_FROM_WIDGET;

   // ask the IFrameModel to supply the extent
   // because we don't have a clue!
   if (MYMODEL(me)) {
      int cx, cy;
      IFRAMEMODEL_GetSize(MYMODEL(me), &cx, &cy);
      pweOut->width = cx;
      pweOut->height = cy;
   }
}

//Get the rectangle that should be used to render the frame
static int FrameWidget_GetFrameRect(FrameWidget *me, AEERect *prc)
{
   int nErr = 0;
   IContainer *piContainer = 0;
   WExtent we;
   IWidget *piWidget = 0;
   IRootContainer *piRootContainer = 0;
   SETAEERECT(prc,0,0,0,0);
   //Locate self and return  the rect 
   if(!me->base.piContainer) {
      nErr = EFAILED;
   }
   if(!nErr) {
      nErr = ICONTAINER_Locate(me->base.piContainer, (IWidget*) me, 
          (IContainer**) &piContainer, prc);
   }
   if(!nErr && piContainer) {
      nErr = ICONTAINER_QueryInterface(piContainer, AEEIID_ROOTCONTAINER, (void**) &piRootContainer);
   }
   if(!nErr && piRootContainer) {
      nErr = IROOTCONTAINER_QueryInterface(piRootContainer, AEEIID_WIDGET, (void**) &piWidget);
   }
   if(!nErr && piWidget) {
      //Ignore return value for PROP_SCREEN - we do not care if RC handles it or not.
      (void) IWIDGET_GetPropertyEx(piWidget, PROPEX_SCREEN,
            sizeof(*prc), (void*) prc);
   }
   if(!nErr && MYMODEL(me)) {
      WExtent wePref;
      int cx, cy;
      IWIDGET_GetExtent((IWidget*) me, &we);
      IWIDGET_GetPreferredExtent(CAST(IWidget*, me), &wePref);
      cx = MIN(we.width, wePref.width);
      cy = MIN(we.height, wePref.height);
      if(wePref.width < we.width) {
         prc->x+= (int16) (we.width - wePref.width)/2;
      }
      if(wePref.height < we.height) {
         prc->y+= (int16) (we.height - wePref.height)/2;
      }
      prc->dx = (int16) cx;
      prc->dy = (int16) cy;

   }
   
   RELEASEIF(piContainer);
   RELEASEIF(piWidget);
   RELEASEIF(piRootContainer);
   return nErr;
   
}


static int FrameWidget_SetDirectMode(FrameWidget *me, int nDirect)
{
   int nErr = SUCCESS;
   IDirectMode *piDirect = 0;
   AEERect rc;
   if(!MYMODEL(me)) {
      return EFAILED;
   }
   nErr = FrameWidget_GetFrameRect(me, &rc);
   if (!nErr) {
      (void) IFRAMEMODEL_QueryInterface(MYMODEL(me), AEEIID_DIRECTMODE, (void**) &piDirect);
      if(piDirect) {
         nErr = IDIRECTMODE_SetDirectMode(piDirect, nDirect, &rc);
      }
   }
   RELEASEIF(piDirect);
   return nErr;

}
static int FrameWidget_GetDirectMode(FrameWidget *me, int* pnDirect)
{
   IDirectMode *piDirect = 0;
   int nErr = SUCCESS;
   if(!pnDirect || !MYMODEL(me))
      return EFAILED;
   
   nErr = IFRAMEMODEL_QueryInterface(MYMODEL(me), AEEIID_DIRECTMODE, (void**) &piDirect);
   if(piDirect) {
      nErr = IDIRECTMODE_GetDirectMode(piDirect, pnDirect, 0);
   }
   RELEASEIF(piDirect);
   return nErr;
}

boolean FrameWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   switch (evt) {

      case EVT_WDG_GETPROPERTY:
         
         if (wParam == PROP_BGCOLOR) {
            *(uint32 *)dwParam = me->rgbBackground;
            return TRUE;

         } else if (wParam == PROP_ANIMATE_FLAGS) {
            *(uint32 *)dwParam = me->dwAnimFlags;
            return TRUE;

         } else if ( wParam == PROP_EX) {
            WidgetPropEx *pPropEx = (WidgetPropEx*) dwParam;
            if( PROPEX_DIRECT == pPropEx->nPropId) {
               return SUCCESS == FrameWidget_GetDirectMode(me, (int*) pPropEx->pUser);
            }
            
         }
         break;

      case EVT_WDG_SETPROPERTY:

         if (wParam == PROP_BGCOLOR) {
            me->rgbBackground = (RGBVAL)dwParam;
            FrameWidget_Invalidate(me);
            return TRUE;

         } else if (wParam == PROP_ANIMATE_FLAGS) {
            me->dwAnimFlags = dwParam;
            return TRUE;
         } else if (wParam == PROP_EX) {
            WidgetPropEx *pPropEx = (WidgetPropEx*) dwParam;
            if( PROPEX_DIRECT == pPropEx->nPropId) {
            int nDirect = *((int*) pPropEx->pUser);
               return SUCCESS == FrameWidget_SetDirectMode(me, nDirect);
            }
         }
         break;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


void FrameWidget_Dtor(FrameWidget *me)
{
   LISTENER_Cancel(&me->ml);
   RELEASEIF(me->piFrame);
}


uint32 FrameWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      FrameWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}


void FrameWidget_Ctor(FrameWidget *me, AEEVTBL(IWidget) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(FrameWidget_HandleEvent));

   pvt->Release            = FrameWidget_Release;
   pvt->GetPreferredExtent = FrameWidget_GetPreferredExtent;
   pvt->IntersectOpaque    = WidgetBase_IntersectOpaque_O;
   pvt->Draw               = FrameWidget_Draw;
   pvt->SetModel           = FrameWidget_SetModel;
   pvt->SetExtent          = FrameWidget_SetExtent;

   me->rgbBackground = RGB_NONE;
}


int FrameWidget_New(IWidget **ppo, IModule *piModule)
{
   int nErr = 0;
   FrameWidget *me = MALLOCREC_VTBL(FrameWidget, IWidget);
   
   *ppo = 0;
   
   if (!me) {
      return ENOMEMORY;
   }

   FrameWidget_Ctor(me, GETVTBL(me,IWidget), piModule, 0);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      FrameWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}



