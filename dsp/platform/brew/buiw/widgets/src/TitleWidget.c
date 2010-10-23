/*
  ========================================================================

  FILE:  TitleWidget.c
  
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

#include "TitleWidget.h"
#include "StaticWidget.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"

static __inline TitleWidget *DECORATOR_TO_TITLEWIDGET(IDecorator *po) {
   return (TitleWidget*)po;
}
static __inline TitleWidget *CONTAINER_TO_TITLEWIDGET(IContainer *po) {
   return (TitleWidget*)po->pMe;
}

#define TITLE_FROM_DECORATOR TitleWidget *me = DECORATOR_TO_TITLEWIDGET(po)
#define TITLE_FROM_CONTAINER TitleWidget *me = CONTAINER_TO_TITLEWIDGET(po)
#define WBASE(p)             ((WidgetBase *)p)

void TitleWidget_Dtor(TitleWidget *me);

uint32 TitleWidget_Release(IDecorator *po)
{
   TITLE_FROM_DECORATOR;

   if (WBASE(me)->nRefs == 1)
      TitleWidget_Dtor(me);

   return Decorator_Release(po);
}

// Returns overlapping / surrounding width and of static widget and overall border
static void TitleWidget_TitleOverlap(IDecorator *po, int *pnOverlap, int *pnSurround)
{
   TITLE_FROM_DECORATOR;   
   if (me->piTitle) {
      int nTitleBorder = 0;
      int nBorder = 0;

      // Subtract lesser of the y border widths 
      // to account for sharing the same top border
      IWIDGET_GetBorderWidth(me->piTitle, &nTitleBorder);
      IWIDGET_GetBorderWidth(IDECORATOR_TO_IWIDGET(po), &nBorder);

      if(pnOverlap) {
         *pnOverlap = MIN(nBorder, nTitleBorder);
      }
      if (pnSurround) {
         *pnSurround = MAX(0, nBorder-nTitleBorder);
      }
   }
}

void TitleWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   TITLE_FROM_DECORATOR;
   WExtent ce;

   if (me->fInLayout)
      return;
   
   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);
   ce.width = ce.height = 0;
   
   if (me->piTitle) {
      int nShadowX, nShadowY, nSurround, nOverlap, nMaxDX;
      Border_GetCurrentShadowOffsets(&me->border, &nShadowX, &nShadowY);
      TitleWidget_TitleOverlap(po, &nOverlap, &nSurround);
      IWIDGET_GetExtent(me->piTitle, &ce);
      
      // Resize title width if needed
      nMaxDX = MAX(0, pe->width - (ABS(nShadowX) + 2*nSurround));   
      if (ce.width != nMaxDX) {
         ce.width = nMaxDX;
         me->fInLayout = 1;
         IWIDGET_SetExtent(me->piTitle, &ce);
         me->fInLayout = 0;
      }      

      ce.height = MAX(0, ce.height-nOverlap);
      // Adjust client rect to account for title
      me->border.rcClient.y  += ce.height;
      me->border.rcClient.dy = MAX(0, me->border.rcClient.dy - ce.height);

   }

   SETWEXTENT(&ce, me->border.rcClient.dx, me->border.rcClient.dy);
   Decorator_SetExtent(po, &ce);
}

void TitleWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   TITLE_FROM_DECORATOR;

   Decorator_SetWidget(po, piChild);
   TitleWidget_SetExtent(po, &WBASE(me)->extent);
}

void TitleWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   // Don't ask child, we control the extent
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pe);
}

void TitleWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   TITLE_FROM_DECORATOR;      

   // You would think we'd want to call GetPreferredExtent on the
   // title widget, but we actually want to keep it at its current
   // height.  It's the child item that the title decorates that
   // we want to size to.
   Decorator_GetPreferredExtent(po, pweOut);
   Border_CalcPreferredExtent(&me->border, pweOut, pweOut);

   // Factor title (minus shared border) to height and width if necessary
   if (me->piTitle) {
      WExtent ce;
      int nShadowX, nShadowY, nOverlap, nSurround;      
      IWIDGET_GetExtent(me->piTitle, &ce);
      Border_GetCurrentShadowOffsets(&me->border, &nShadowX, &nShadowY);
      TitleWidget_TitleOverlap(po, &nOverlap, &nSurround);
      
      ce.height -= MIN(ce.height, nOverlap);     
      ce.width += ABS(nShadowX) + 2*nSurround;

      pweOut->height += ce.height;      
      pweOut->width = MAX(ce.width, pweOut->width);       
   }   
}

void TitleWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   TITLE_FROM_DECORATOR;
   AEERect rcClip, rcDisp, rcDraw;

   // Draw border
   Border_Draw(&me->border, piCanvas, x, y);

   // Get the current clip rect BEFORE drawing the title widget, as it
   // might modify the clip rect and not be kind enough to restore it
   // to the original value.
   ICANVAS_GetClipRect(piCanvas, &rcDisp);

   // Draw title with minimal clip rect
   if (me->piTitle) {
      WExtent ce;      
      int nMaxDY, nTitleDY = 0;
      int nShadowX, nShadowY, nBorder, nOverlap, nSurround = 0;      
      Border_GetCurrentShadowOffsets(&me->border, &nShadowX, &nShadowY);
      IWIDGET_GetBorderWidth(IDECORATOR_TO_IWIDGET(po), &nBorder);
      IWIDGET_GetExtent(me->piTitle, &ce);
      TitleWidget_TitleOverlap(po, &nOverlap, &nSurround);

      // Title clip height (subtract border overlap)
      nTitleDY = MAX(0, ce.height - nOverlap);

      // Max clip height base extent minus border / shadow
      nMaxDY = MAX(0, me->base.base.extent.height - (2*nBorder+ABS(nShadowY)));

      // Zero shadows if they dont affect draw pos (negative shadow => positive offset)
      nShadowX = ABS(MIN(nShadowX, 0));
      nShadowY = ABS(MIN(nShadowY, 0));

      // Set title clipping region inside border, outside padding
      rcClip.x  = x + nShadowX + nBorder;
      rcClip.y  = y + nShadowY + nBorder;
      rcClip.dx = me->border.rcClient.dx + me->border.padding.left + me->border.padding.right;
      rcClip.dy = MIN(nTitleDY, nMaxDY);

      // Set drawing coordinates past shadow and surrounding border
      if (IntersectRect(&rcClip, &rcClip, &rcDisp)) {
         int nTitleX = x + nShadowX + nSurround;
         int nTitleY = y + nShadowY + nSurround;
         ICANVAS_SetClipRect(piCanvas, &rcClip);
         IWIDGET_Draw(me->piTitle, piCanvas, nTitleX, nTitleY);
      }
   }

   // Draw client region 
   SETAEERECT(&rcDraw, x + me->border.rcClient.x, y + me->border.rcClient.y, 
           me->border.rcClient.dx, me->border.rcClient.dy);   
   if (IntersectRect(&rcClip, &rcDisp, &rcDraw)) {          
      ICANVAS_SetClipRect(piCanvas, &rcClip);
      Decorator_Draw(po, piCanvas, rcDraw.x, rcDraw.y);
   }
}

boolean TitleWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, const AEERect *prcIn)
{
/*    TITLE_FROM_DECORATOR; */
/*    WExtent ce; */
/*    AEERect rc; */

   prcOut->dx = prcOut->dy = 0;
   return FALSE;
}

boolean TitleWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   TITLE_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave     = *prcClient;

   // if we have a child, let it handle these focus events first
   if (me->base.pChild && 
       evt == EVT_WDG_HASFOCUS || evt == EVT_WDG_CANTAKEFOCUS) {
      return Decorator_HandleEvent(po, evt, wParam, dwParam);
   }

   // Deal with border properties first
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
      }
      return TRUE;
   }

   if (evt == EVT_WDG_GETPROPERTY) {
      if (wParam == PROP_TITLEWIDGET) {
         *((IWidget **)dwParam) = me->piTitle;
         ADDREFIF(me->piTitle);
         return TRUE;
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {
      if (wParam == PROP_TITLEWIDGET) {
   
         if (me->piTitle) {
            // Release the old title
            IWIDGET_SetParent(me->piTitle, NULL);
            IWIDGET_Release(me->piTitle);
         }
      
         // Setup the new title
         me->piTitle = (IWidget *) dwParam;
      
         if (me->piTitle) {
            IWIDGET_AddRef(me->piTitle);
            IWIDGET_SetParent(me->piTitle, &me->base.container);
         }

         return TRUE;
      }
   
   }

   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}

/////////////////////////////////////////////////////////////////
// Container methods

void TitleWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags)
{
   TITLE_FROM_CONTAINER;

   if (piw == me->base.pChild) {
      Decorator_ClientInvalidate(TITLEWIDGET_TO_IDECORATOR(me), &me->border.rcClient, prcIn, dwFlags);

   } else if (piw == me->piTitle) {
      AEERect rc;
      if (prcIn) {
         rc = *prcIn;
      } else {
         IWidget_GetRect(me->piTitle, &rc);
      }
      Decorator_Invalidate(TITLEWIDGET_TO_IDECORATOR(me), &rc, dwFlags);
   }
}

int TitleWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   TITLE_FROM_CONTAINER;
   return Decorator_ClientLocate((IDecorator*)me, &me->border.rcClient, piw, ppRoot, prcRel);
}

/////////////////////////////////////////////////////////////////


void TitleWidget_Ctor(TitleWidget *me, AEEVTBL(IDecorator) *pvt, IWidget *piTitle, 
                      IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(TitleWidget_HandleEvent));

   pvt->Release            = TitleWidget_Release;
   pvt->Draw               = TitleWidget_Draw;
   pvt->IntersectOpaque    = TitleWidget_IntersectOpaque;
   pvt->GetPreferredExtent = TitleWidget_GetPreferredExtent;
   pvt->GetExtent          = TitleWidget_GetExtent;
   pvt->SetExtent          = TitleWidget_SetExtent;
   pvt->SetWidget          = TitleWidget_SetWidget;

   me->base.vtContainer.Invalidate = TitleWidget_Invalidate;
   me->base.vtContainer.Locate = TitleWidget_Locate;

   me->piTitle = piTitle;
   if (me->piTitle) {
      IWIDGET_AddRef(piTitle);
      IWIDGET_SetParent(piTitle, &me->base.container);
   }

   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.base.extent, FALSE, &WBASE(me)->piViewModel);
   IWIDGET_SetBorderWidth(TITLEWIDGET_TO_IWIDGET(me), 1);
   IWIDGET_SetBorderColor(TITLEWIDGET_TO_IWIDGET(me), RGBA_BLACK);
}

void TitleWidget_Dtor(TitleWidget *me)
{
   RELEASEIF(me->piTitle);
   Border_Dtor(&me->border);
}

/////////////////////////////////////////////////////////////////
// Concrete Implementation

int TitleWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   TitleWidget *me = MALLOCREC_VTBL(TitleWidget, IDecorator);
   IWidget *piTitle = 0;
   IFont *piFont = 0;
   int result;

   *ppo = TITLEWIDGET_TO_IDECORATOR(me);

   if (!me)
      result = ENOMEMORY;
   else 
      result = StaticWidget_New(&piTitle, 0, piShell, piModule);

   if (result == 0) {
      IWIDGET_SetFlags(piTitle, IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE);

      result = ISHELL_CreateFont(piShell, AEECLSID_FONTSYSBOLD, &piFont);
   }

   if (result == 0)
      IWIDGET_SetFont(piTitle, piFont);

   if (result == 0)
      TitleWidget_Ctor(me, GETVTBL(me, IDecorator), piTitle, piModule, 0);

   if (result != 0)
      FREE(me);

   RELEASEIF(piTitle);
   RELEASEIF(piFont);
   
   return result;
}



