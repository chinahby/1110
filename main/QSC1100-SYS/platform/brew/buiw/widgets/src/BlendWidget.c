/*======================================================
FILE:       BlendWidget.c

SERVICES:   Draws another widget blended (faded) 

GENERAL DESCRIPTION:

 BlendWiget "wraps" another widget and serves as a semi-transparent version
 of the underlying widget.  After the BlendWidget is initialized, the
 underlying widget should not be manipulated directly.

 
Reproduction and/or distribution of this file without the
written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"

#include "BlendWidget.h"
#include "AEEWProperties.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"
#include "Bitmap32.h"


// Implementation notes:
//
// We inherit behavior and structure from Decorator, overriding HandleEvent
// to capture opacity changes, Draw to do blending and IntersectOpaque


//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------

#define ME_FROM_DECORATOR BlendWidget *me  = (BlendWidget *)po
#define DECORATOR_FROM_ME Decorator   *dec = (Decorator *)me
#define ME_FROM_CONTAINER BlendWidget *me  = (BlendWidget *)po->pMe

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

static __inline void BlendWidget_InvalidateContent(IDecorator *po)
{
   AEERect rc;
   WExtent childExtent;

   Decorator_GetExtent(po, &childExtent);
   SETAEERECT(&rc, 0, 0, childExtent.width, childExtent.height);
   Decorator_Invalidate(po, &rc, 0);
}

static __inline boolean BlendWidget_BufferReady(BlendWidget *me)
{
   if (me->pibBuffer) {
      WExtent we;
      AEEBitmapInfo bmpInfo;

      IBITMAP_GetInfo(me->pibBuffer, &bmpInfo, sizeof(bmpInfo));

      Decorator_GetExtent((IDecorator*)me, &we);
      
      // if the buffer is still the right size, we can at least try to reuse it
      if (bmpInfo.cx == (uint32)we.width && bmpInfo.cy == (uint32)we.height) {
         return !me->fInvalidBuffer;
      }
      else {      // the buffer no longer matches the extent so we have to create a new one
         RELEASEIF(me->pibBuffer);           // trash old buffer (new one created in draw)
         return FALSE;
      }
   }

   // no buffer, return false so we get a new one created during draw
   return FALSE;
}

boolean BlendWidget_IntersectOpaque(IDecorator *po, AEERect *prc, const AEERect *prcTest)
{
   ME_FROM_DECORATOR;
   
   // If not buffered and opacity is 255, use old code path
   if (!me->fIsBuffered && me->nOpacity == 255) {
      return Decorator_IntersectOpaque(po, prc, prcTest);
   } else {
      // If buffered in a 32bpp buffer, then we can't know if any pixels are transparent
      prc->dx = 0;
      prc->dy = 0;
      return FALSE;
   }
}

void BlendWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;

   IBitmap *pibScreen;
   AEERect rcClip, rcWidget, rcDraw;
   WExtent we;
   IDisplay *piDisplay = 0;

   if( me->nOpacity == 0)
      // Completely transparent
      return;

   if (SUCCESS != ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
      return;
   }

   // calculate size, position of where we will actually draw

   Decorator_GetExtent(po, &we);
   SETAEERECT(&rcWidget, x, y, we.width, we.height);
      
   IDISPLAY_GetClipRect(piDisplay, &rcClip);

   if (!IntersectRect(&rcDraw, &rcWidget, &rcClip)) {
      RELEASEIF(piDisplay);
      return;
   }

   pibScreen = IDISPLAY_GetDestination(piDisplay);
   if ( !pibScreen ) {
      RELEASEIF(piDisplay);
      return;
   }

//   DBGPRINTF("rcDraw=%d,%d,%d,%d, rcClip=%d,%d,%d,%d",
//             rcDraw.x, rcDraw.y, rcDraw.dx, rcDraw.dy,
//             rcClip.x, rcClip.y, rcClip.dx, rcClip.dy);

   if(!me->fIsBuffered) {     // single buffer
      if (me->nOpacity == 255) {
            Decorator_Draw(po, piCanvas, x, y);
      }
      else
      {
         // Old code
         IBitmap* pibDraw;
         if(SUCCESS == IBITMAP_CreateCompatibleBitmap(pibScreen, &pibDraw, rcDraw.dx, rcDraw.dy)) {

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
      }
   } else { // double buffering
      if (!BlendWidget_BufferReady(me)) {
         AEERect rect;
         
         // Allocate the new buffer or clean the old one
         if(!me->pibBuffer) {
            Bitmap32_New((uint16)we.width, (uint16)we.height, NULL, &me->pibBuffer);
         }
         else {
            rect.x = 0; rect.y = 0; rect.dx = we.width; rect.dy = we.height;
            IBITMAP_FillRect(me->pibBuffer, &rect, 0, AEE_RO_COPY);
         }

         if(me->pibBuffer) {            
            IDISPLAY_SetDestination(piDisplay, me->pibBuffer );
            IDISPLAY_SetClipRect(piDisplay, NULL);
   
            // Draw child offscreen with origin of 0,0
            Decorator_Draw(po, piCanvas, 0, 0);

            IDISPLAY_SetClipRect(piDisplay, &rcClip);
            IDISPLAY_SetDestination(piDisplay, pibScreen);
         }

         me->fInvalidBuffer = FALSE;
      }

      // blend the buffered widget onto the draw rectangle onto the screen, bearing in mind the 
      // widget position.
      if(me->pibBuffer) {
         Bitmap32_BltOutTransparent(me->pibBuffer, rcDraw.x, rcDraw.y, rcDraw.dx, rcDraw.dy,
            pibScreen, rcDraw.x - rcWidget.x, rcDraw.y - rcWidget.y, me->nOpacity);
      }
   }

   IBITMAP_Release(pibScreen);
   RELEASEIF(piDisplay);
}


static void BlendWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;
   WExtent we;

   // use the Decorator_GetExtent to ask the child properly
   Decorator_GetExtent(po, &we);

   // There is some reliance in widgets that a SetExtent will invalidate the
   // widget, so let's maintain that behavior but optimize it so we only redraw
   // in double-buffer mode when it's a true extent change
   if((we.width == pe->width && we.height == pe->height) &&
      (me->fIsBuffered && !me->fInvalidBuffer)) {
      BlendWidget_InvalidateContent(po);
      return;
   }
   else {
      me->fSetExtent = TRUE;              // Begin to block out invalidBuffer notifications
      Decorator_SetExtent(po, pe);        // update the child's extent
      me->fSetExtent = FALSE;             // Allow the invalidBuffer notifications

      // don't set the me->fInvalidBuffer here because we'll square it up when we get to 
      // draw to see if the WExtent and Bitmap32 extent really are different 
   }
}

static void BlendWidget_Invalidate(IContainer *po, IWidget *pw, const AEERect *prc, uint32 dwFlags)
{
   ME_FROM_CONTAINER;

   // while doing a SetExtent on the child and we're double buffered, don't allow 
   // invalidates to dirty the buffer
   if (!me->fSetExtent) {
      me->fInvalidBuffer = TRUE;
   }
   Decorator_Invalidate((IDecorator*)me, prc, dwFlags);
}

static void BlendWidget_SetWidget(IDecorator *po, IWidget *pChild)
{
   ME_FROM_DECORATOR;

   me->fInvalidBuffer = TRUE;
   Decorator_SetWidget(po, pChild);
}

boolean BlendWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_DECORATOR;

   switch(evt) {
   case EVT_WDG_SETPROPERTY:
      switch(wParam) {
      case PROP_TRANSPARENCY: 
         // Alpha is the inverse of transparency
         dwParam = 255 - (dwParam & 255);
         // Drop through to alpha handling
      case PROP_ALPHA: 
         me->nOpacity = (dwParam & 255);
         BlendWidget_InvalidateContent(po);
         return TRUE;
      case PROP_BUFFERED:
         if(!me->fIsBuffered) {
            me->fInvalidBuffer = TRUE;
         }
         me->fIsBuffered = (boolean)dwParam;
         // If we are no longer buffered, and we used to be, free the buffer.
         if(!me->fIsBuffered) {
            RELEASEIF(me->pibBuffer);
         }
         return TRUE;
      default:
         return Decorator_HandleEvent(po, evt, wParam, dwParam);
      }
   case EVT_WDG_GETPROPERTY:
      switch(wParam) {
      case PROP_TRANSPARENCY:
         *(uint32 *)dwParam = 255 - me->nOpacity;
         return TRUE;
      case PROP_ALPHA:
         *(uint32 *)dwParam = me->nOpacity;
         return TRUE;
      case PROP_BUFFERED:
         *(uint32 *)dwParam = me->fIsBuffered;
         return TRUE;
      default: 
         return Decorator_HandleEvent(po, evt, wParam, dwParam);
      }
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
   DECORATOR_FROM_ME;
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(BlendWidget_HandleEvent));

   // Override widget functions
   pvt->Draw            = BlendWidget_Draw;
   pvt->IntersectOpaque = BlendWidget_IntersectOpaque;
   pvt->Release         = BlendWidget_Release;
   pvt->SetExtent       = BlendWidget_SetExtent;
   pvt->SetWidget       = BlendWidget_SetWidget;

   WCBASE(dec)->vtContainer.Invalidate = BlendWidget_Invalidate;
   
   //BlendWidget_DerivedCtor(me);
   me->nOpacity = 255;
   me->pibBuffer = NULL;
   me->fIsBuffered = FALSE;
   me->fInvalidBuffer = TRUE;
   me->fSetExtent = FALSE;
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

uint32 BlendWidget_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if(WBASE(me)->nRefs == 1) {
      RELEASEIF(me->pibBuffer);
   }
   return Decorator_Release(po);
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

