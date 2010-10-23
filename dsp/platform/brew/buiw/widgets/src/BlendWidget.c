/*======================================================
FILE:       BlendWidget.c

SERVICES:   Draws another widget blended (faded) 

GENERAL DESCRIPTION:

 BlendWiget "wraps" another widget and serves as a semi-transparent version
 of the underlying widget.  After the BlendWidget is initialized, the
 underlying widget should not be manipulated directly.

 
Reproduction and/or distribution of this file without the
written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"

#include "BlendWidget.h"
#include "AEEWProperties.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"


// Implementation notes:
//
// We inherit behavior and structure from Decorator, overriding HandleEvent
// to capture opacity changes, Draw to do blending and IntersectOpaque


//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------

#define ME_FROM_DECORATOR BlendWidget *me = (BlendWidget *)po
#define ME_FROM_CONTAINER BlendWidget *me = (BlendWidget *)po->pMe


// Drawing:
//
//   Blending a widget involves the following steps:
//
//   1.  Create a new off-screen bitmap the size of the child widget
//   2.  Copy the destination pixels onto the off-screen bimap.  [see note]
//   3.  Draw the child widget onto the off-screen bitmap
//   4.  Blend the off-screen bitmap onto the screen, using BlendBlit().
//
//   Note: Step 2 is necessary only for the parts of the child widget that are
//   transparent.  Draw now supports transparent child widgets
//
//   Note: All the above steps could be further optimized by dealing only with
//   the clipped region, not the entire widget size.
//

boolean BlendWidget_IntersectOpaque(IDecorator *po, AEERect *prc, const AEERect *prcTest)
{
   ME_FROM_DECORATOR;
   
   if (me->nOpacity == 255) {
      return Decorator_IntersectOpaque(po, prc, prcTest);
   } else {
      prc->dx = 0;
      prc->dy = 0;
      return FALSE;
   }
}

void BlendWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;

   IBitmap *pibScreen, *pibDraw;
   AEERect rcClip, rcWidget, rcDraw;
   WExtent we;
   IDisplay *piDisplay = 0;

   if (SUCCESS != ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
      return;
   }

   // calculate size, position of where we will actually draw

   Decorator_GetExtent(po, &we);
   SETAEERECT(&rcWidget, x, y, we.width, we.height);
      
   IDISPLAY_GetClipRect(piDisplay, &rcClip);

   if (!IntersectRect(&rcDraw, &rcWidget, &rcClip)) {
      return;
   }

   pibScreen = IDISPLAY_GetDestination(piDisplay);
   if ( !pibScreen ) {
      return;
   }

//   DBGPRINTF("rcDraw=%d,%d,%d,%d, rcClip=%d,%d,%d,%d",
//             rcDraw.x, rcDraw.y, rcDraw.dx, rcDraw.dy,
//             rcClip.x, rcClip.y, rcClip.dx, rcClip.dy);
   if (me->nOpacity == 255) {
      Decorator_Draw(po, piCanvas, x, y);
   } else if (SUCCESS == IBITMAP_CreateCompatibleBitmap(pibScreen, &pibDraw, rcDraw.dx, rcDraw.dy)) {

      IDISPLAY_SetDestination(piDisplay, pibDraw);
      IDISPLAY_SetClipRect(piDisplay, NULL);          // "no clipping" = clip to bitmap bounds

      // check if wrapped widget is opaque
      {
         AEERect rcOpaque;
         AEERect rcResult;

         SETAEERECT(&rcOpaque, rcWidget.x - rcDraw.x, rcWidget.y - rcDraw.y,
                    rcDraw.dx, rcDraw.dy);

         // if not opaque intersection OR the intersecting rect is not 
         // the entire invalid region, means we have transparent child
         if (!BlendWidget_IntersectOpaque(po, &rcResult, &rcOpaque) 
             || rcOpaque.dx != rcResult.dx || rcOpaque.dy != rcResult.dy) {

            // not completely opaque, so copy in the background bits
            IBITMAP_BltIn(pibDraw, 0, 0, rcDraw.dx, rcDraw.dy, 
                          pibScreen, rcDraw.x, rcDraw.y, AEE_RO_COPY);
         }
      }

      // draw offscreen
      Decorator_Draw(po, piCanvas, rcWidget.x - rcDraw.x, rcWidget.y - rcDraw.y);

      IDISPLAY_SetClipRect(piDisplay, &rcClip);
      IDISPLAY_SetDestination(piDisplay, pibScreen);

      // blend onto screen
      BlendBlit(pibScreen, &rcDraw, pibDraw, 0, 0, me->nOpacity);

      IBITMAP_Release(pibDraw);
   }

   IBITMAP_Release(pibScreen);

   RELEASEIF(piDisplay);
}

static __inline void BlendWidget_InvalidateExtent(IDecorator *po)
{
   AEERect rc;
   WExtent childExtent;

   Decorator_GetExtent(po, &childExtent);
   SETAEERECT(&rc, 0, 0, childExtent.width, childExtent.height);
   Decorator_Invalidate(po, &rc, 0);
}


boolean BlendWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_DECORATOR;

   switch(evt) {
   case EVT_WDG_SETPROPERTY:
      switch(wParam) {
      case PROP_TRANSPARENCY: 
         me->nOpacity = 255 - (dwParam & 255);
         BlendWidget_InvalidateExtent(po);
         return TRUE;
      default:
         return Decorator_HandleEvent(po, evt, wParam, dwParam);
      } break;
   case EVT_WDG_GETPROPERTY:
      switch(wParam) {
      case PROP_TRANSPARENCY:
         *(uint32 *)dwParam = 255 - me->nOpacity;
         return TRUE;
      default: 
         return Decorator_HandleEvent(po, evt, wParam, dwParam);
      } break;
   }

   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}


//--------------------------------------------------------------------
//    BlendWidget-implementation
//--------------------------------------------------------------------

typedef struct BlendWidgetImpl {
   BlendWidget   base;

   AEEVTBL(IDecorator) vtDecorator;
} BlendWidgetImpl;


void BlendWidget_Ctor(BlendWidget *me, AEEVTBL(IDecorator) *pvt, 
                      IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(BlendWidget_HandleEvent));

   // Override widget functions
   pvt->Draw            = BlendWidget_Draw;
   pvt->IntersectOpaque = BlendWidget_IntersectOpaque;
   
   //BlendWidget_DerivedCtor(me);
   me->nOpacity = 255;
}

int BlendWidget_New(IDecorator **ppid, IModule *piModule)
{
   BlendWidgetImpl *me = MALLOCREC(BlendWidgetImpl);

   if (!me)
      return ENOMEMORY;

   // construct base
   BlendWidget_Ctor(&me->base, &me->vtDecorator, piModule, 0);
   
   
   *ppid = BLENDWIDGET_TO_IDECORATOR(&me->base);
   return SUCCESS;

}


//--------------------------------------------------------------------
//    Utility functions
//--------------------------------------------------------------------

int IWidget_WrapInBlender(IWidget **ppioChild, IModule *piModule)
{
   int result;
   IDecorator *piBorder;

   result = BlendWidget_New(&piBorder, piModule);
   
   if (result == 0) {
      IDECORATOR_SetWidget(piBorder, *ppioChild);
      IWIDGET_Release(*ppioChild);
      *ppioChild = IDECORATOR_TO_IWIDGET(piBorder);
   }

   return result;
}

