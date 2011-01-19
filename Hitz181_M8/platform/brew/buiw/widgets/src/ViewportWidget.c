/*
  ========================================================================

  FILE:  ViewportWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a decorator widget that
                       provides a 'viewport' into another widget.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "ViewportWidget.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "wutil.h"

static __inline ViewportWidget *DECORATOR_TO_VIEWPORTWIDGET(IDecorator *po) {
   return (ViewportWidget*)po;
}
static __inline ViewportWidget *CONTAINER_TO_VIEWPORTWIDGET(IContainer *po) {
   return (ViewportWidget*)po->pMe;
}

#define ME_FROM_DECORATOR  ViewportWidget *me = DECORATOR_TO_VIEWPORTWIDGET(po)
#define ME_FROM_CONTAINER  ViewportWidget *me = CONTAINER_TO_VIEWPORTWIDGET(po) 

void ViewportWidget_Dtor(ViewportWidget *me);

/////////////////////////////////////////////////////////////////////////////
// misc methods

static void ViewportWidget_SendScrollEvent(ViewportWidget *me, boolean bVert)
{
   if (WBASE(me)->piViewModel) {

      ScrollEvent se;
   
      se.base.evCode  = EVT_MDL_SCROLL_CHANGE;
      se.bVertical    = bVert;
   
      if (bVert) {
         se.range        = me->weChild.height;
         se.visible      = me->border.rcClient.dy;
         se.position     = me->yOrigin;
      } else {
         se.range        = me->weChild.width;
         se.visible      = me->border.rcClient.dx;
         se.position     = me->xOrigin;
      }

      IMODEL_Notify(WBASE(me)->piViewModel, (ModelEvent*)&se);
   }
}


static __inline void ViewportWidget_InvalidateViewport(IDecorator *po)
{
   ME_FROM_DECORATOR;

   Decorator_Invalidate(po, &me->border.rcClient, 0);
}

static void ViewportWidget_NotifyScroll(ViewportWidget *me)
{
   ViewportWidget_SendScrollEvent(me, 0);
   ViewportWidget_SendScrollEvent(me, 1);
}


/////////////////////////////////////////////////////////////////////////////
// IWidget methods

uint32 ViewportWidget_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if (WBASE(me)->nRefs == 1) {
      ViewportWidget_Dtor(me);
   }
   return Decorator_Release(po);
}

static void ViewportWidget_CalcMaxXY(IDecorator *po)
{
   ME_FROM_DECORATOR;

   WExtent we;

   me->bInCalcXY = 1;
   
   if (me->base.pChild) {
      WExtent currentExtent;
      IWIDGET_GetExtent(me->base.pChild, &currentExtent);   // store current extent
      MEMCPY(&we, &currentExtent, sizeof(WExtent));

      // calculate child widget's extent
      if (me->nLayout == LAYOUT_VERT) {

         if (we.width > me->border.rcClient.dx || (me->dwFlags & VWF_SIZETOFIT)) {

            IWIDGET_SetHintWidth(me->base.pChild, me->border.rcClient.dx);
            Decorator_GetPreferredExtent(po, &we);
            we.width = me->border.rcClient.dx;

            if (we.height < me->border.rcClient.dy && (me->dwFlags & VWF_SIZETOFIT)) {
               we.height = me->border.rcClient.dy;
            }
         }
   
      } else if (me->nLayout == LAYOUT_HORZ) {

         if (we.height > me->border.rcClient.dy || (me->dwFlags & VWF_SIZETOFIT)) {

            if (we.width < me->border.rcClient.dx && (me->dwFlags & VWF_SIZETOFIT)) {
               IWIDGET_SetHintWidth(me->base.pChild, me->border.rcClient.dx);
            }

            Decorator_GetPreferredExtent(po, &we);
            we.height = me->border.rcClient.dy;
         }
      } else  { // LAYOUT_BOTH

         if (me->dwFlags & VWF_SIZETOFIT) {

            if (we.width < me->border.rcClient.dx) {
               IWIDGET_SetHintWidth(me->base.pChild, me->border.rcClient.dx);
            }

            Decorator_GetPreferredExtent(po, &we);

            if (we.height < me->border.rcClient.dy) {
               we.height = me->border.rcClient.dy;
            }
         }
      }

      // if our extent changed, then call SetExtent on the child.
      if (we.height != currentExtent.height ||
          we.width != currentExtent.width)
         Decorator_SetExtent(po, &we);
   }

   Decorator_GetExtent(po, &we);

   me->weChild = we;

   me->xMax = MAX(0, me->weChild.width - me->border.rcClient.dx);
   me->yMax = MAX(0, me->weChild.height - me->border.rcClient.dy);

   me->yOrigin = CONSTRAIN(me->yOrigin, 0, me->yMax);
   me->xOrigin = CONSTRAIN(me->xOrigin, 0, me->xMax);

   ViewportWidget_NotifyScroll(me);

   me->bInCalcXY = 0;
}

void ViewportWidget_SetExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;
   WidgetBase_SetExtent((IWidget*)&me->base.base, pe);
   Border_CalcClientRect(&me->border);

   if (!me->bInCalcXY) {
      ViewportWidget_CalcMaxXY(po);
   }
}

void ViewportWidget_SetWidget(IDecorator *po, IWidget *piChild)
{
   Decorator_SetWidget(po, piChild);

   if (piChild) {
      ViewportWidget_CalcMaxXY(po);
   }
}

void ViewportWidget_GetExtent(IDecorator *po, WExtent *pe)
{
   WidgetBase_GetExtent((IWidget*)po, pe);
}

void ViewportWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   ME_FROM_DECORATOR;
   
   WExtent e;
   Decorator_GetPreferredExtent(po, &e);
   Border_CalcPreferredExtent(&me->border, pweOut, &e);
}


boolean ViewportWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_DECORATOR;

   boolean bIntersect = Border_IntersectOpaque(&me->border, prcOut, prcIn);
   
   if (!bIntersect && me->base.pChild) {
      AEERect rcIn = *prcIn;
      rcIn.x -= me->xOrigin;
      rcIn.y -= me->yOrigin;
      if (IntersectRect(&rcIn, &rcIn, &me->border.rcClient)) {
         bIntersect = IWIDGET_IntersectOpaque(me->base.pChild, prcOut, &rcIn);
      }
   }

   return bIntersect;
}


void ViewportWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;

   // Draw border
   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);
   
   Decorator_Draw(po, piCanvas, 
                  x + me->border.rcClient.x - me->xOrigin, 
                  y + me->border.rcClient.y - me->yOrigin);
}

boolean ViewportWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_DECORATOR;

   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave     = *prcClient;

   // Deal with border properties first
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (!me->bInCalcXY && (rcSave.dx != prcClient->dx ||
                            rcSave.dy != prcClient->dy)) {
         ViewportWidget_CalcMaxXY(po);
      }
      return TRUE;
   }

   switch (evt) {
      case EVT_KEY:
         if (wParam == AVK_UP || wParam == AVK_DOWN ||
             wParam == AVK_LEFT || wParam == AVK_RIGHT) {
   
            int nInc = me->nInc * ((wParam == AVK_UP || wParam == AVK_LEFT) ? -1 : 1);
      
            int xSave = me->xOrigin;
            int ySave = me->yOrigin;
      
            if (wParam == AVK_UP || wParam == AVK_DOWN) {
               me->yOrigin += nInc;
            } else { // AVK_LEFT, AVK_RIGHT
               me->xOrigin += nInc;
            }
            me->yOrigin = CONSTRAIN(me->yOrigin, 0, me->yMax);
            me->xOrigin = CONSTRAIN(me->xOrigin, 0, me->xMax);
      
            if (xSave != me->xOrigin || ySave != me->yOrigin) {
               ViewportWidget_InvalidateViewport(po);
               ViewportWidget_SendScrollEvent(me, (boolean)(ySave != me->yOrigin));
               return TRUE;
            }
         }
         break;
   
      case EVT_WDG_SETPROPERTY:
         switch (wParam) {

            case PROP_FLAGS:
               me->dwFlags = dwParam;
               return TRUE;

            case PROP_LAYOUTSTYLE:
               me->nLayout = (uint8)dwParam;
               return TRUE;

            case PROP_INCREMENT:
               me->nInc = (int)dwParam;
               return TRUE;
            
            case PROP_SCROLLREQ:
               ViewportWidget_SendScrollEvent(me, 0);
               ViewportWidget_SendScrollEvent(me, 1);
               return TRUE;
            
            case PROP_OFFSETX: {
               int xSave = me->xOrigin;
               me->xOrigin = CONSTRAIN((int)dwParam, 0, me->xMax);
               if (xSave != me->xOrigin) {
                  ViewportWidget_InvalidateViewport(po);
                  ViewportWidget_SendScrollEvent(me, 0);
               }
               return TRUE;
            }
      
            case PROP_OFFSETY: {
               int ySave = me->yOrigin;
               me->yOrigin = CONSTRAIN((int)dwParam, 0, me->yMax);
               if (ySave != me->yOrigin) {
                  ViewportWidget_InvalidateViewport(po);
                  ViewportWidget_SendScrollEvent(me, 1);
               }
               return TRUE;
            }
         }
         break;
   
      case EVT_WDG_GETPROPERTY:
         switch  (wParam) {
            case PROP_VIEWMODEL:
               if (SUCCESS == WidgetBase_GetViewModel(WBASE(me), (IModel **)dwParam)) {
                  CALLBACK_Resume(&me->cbkView, ViewportWidget_NotifyScroll, me, me->piShell);
               }
               return TRUE;

            case PROP_INCREMENT:
               *(int *)dwParam = me->nInc;
               return TRUE;

            case PROP_OFFSETX:
               *(int *)dwParam = me->xOrigin;
               return TRUE;

            case PROP_OFFSETY:
               *(int *)dwParam = me->yOrigin;
               return TRUE;

            case PROP_LAYOUTSTYLE:
               *(int*)dwParam = (int)me->nLayout;
               return TRUE;

            case PROP_FLAGS:
               *(uint32*)dwParam = me->dwFlags;
               return TRUE;

         }
         break;
   }

   if(Decorator_HandleEvent(po, evt, wParam, dwParam)) {
      return TRUE;
   }

   return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Container methods

void ViewportWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcInvalid, uint32 dwFlags)
{
   ME_FROM_CONTAINER;
   
   // if the extent changed and we're not already in CalcMaxXY
   if (!me->bInCalcXY && (dwFlags & ICIF_EXTENT)) {
      ViewportWidget_CalcMaxXY((IDecorator*)me);
   }

   if (!prcInvalid) {
      Decorator_Invalidate((IDecorator*)me, NULL, dwFlags);
   
   } else {

      AEERect rcInvalid = *prcInvalid;
      AEERect rcViewport = me->border.rcClient;

      rcInvalid.x -= me->xOrigin;
      rcInvalid.y -= me->yOrigin;
      
      // We only need to invalidate if rcInvalid falls within the
      // viewport.  Check to see if it does, but remember that
      // rcInvalid is specified in viewport coordinates!
      rcViewport.x = rcViewport.y = 0;

      if (IntersectRect(&rcInvalid, &rcInvalid, &rcViewport)) {
         Decorator_ClientInvalidate((IDecorator*)me, &me->border.rcClient, &rcInvalid, dwFlags);
      }
   }
}


int ViewportWidget_Locate(IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel)
{
   ME_FROM_CONTAINER;
   int nErr = Decorator_ClientLocate((IDecorator*)me, &me->border.rcClient, piw, ppRoot, prcRel);
   if (!nErr) {
      prcRel->x -= me->xOrigin;
      prcRel->y -= me->yOrigin;
   }
   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// ctor/dtor

void ViewportWidget_Ctor(ViewportWidget *me, AEEVTBL(IDecorator) *pvt, 
                         IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ViewportWidget_HandleEvent));

   pvt->Release            = ViewportWidget_Release;
   pvt->Draw               = ViewportWidget_Draw;
   pvt->GetPreferredExtent = ViewportWidget_GetPreferredExtent;
   pvt->GetExtent          = ViewportWidget_GetExtent;
   pvt->SetExtent          = ViewportWidget_SetExtent;
   pvt->SetWidget          = ViewportWidget_SetWidget;
   pvt->IntersectOpaque    = ViewportWidget_IntersectOpaque;

   WCBASE(me)->vtContainer.Invalidate  = ViewportWidget_Invalidate;
   WCBASE(me)->vtContainer.Locate      = ViewportWidget_Locate;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &WBASE(me)->extent, TRUE, &WBASE(me)->piViewModel);
   IWIDGET_SetBorderWidth((IWidget*)me, 0);
   IWIDGET_SetBorderColor((IWidget*)me, RGBA_NONE);
   IWIDGET_SetBGColor((IWidget*)me, RGBA_NONE);

   me->xOrigin = 0;
   me->yOrigin = 0;
   me->xMax    = 0;
   me->yMax    = 0;
   me->nInc    = 1;
   me->dwFlags = 0;

   me->nLayout = LAYOUT_BOTH;
}


void ViewportWidget_Dtor(ViewportWidget *me)
{
   CALLBACK_Cancel(&me->cbkView);
   RELEASEIF(me->piShell);
   Border_Dtor(&me->border);
}

/////////////////////////////////////////////////////////////////////////////
// New

int ViewportWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   ViewportWidget *me = MALLOCREC_VTBL(ViewportWidget, IDecorator);
   
   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   ViewportWidget_Ctor(me, GETVTBL(me,IDecorator), piShell, piModule, 0);
   *ppo = (IDecorator*)me;
   return SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Utility functions

int IWidget_WrapInViewport(IWidget **ppioChild, IShell *piShell, IModule *piModule)
{
   int nErr;
   IDecorator *piDecorator;

   nErr = ViewportWidget_New(&piDecorator, piShell, piModule);
   
   if (!nErr) {
      IDECORATOR_SetWidget(piDecorator, *ppioChild);
      IWIDGET_Release(*ppioChild);
      *ppioChild = IDECORATOR_TO_IWIDGET(piDecorator);
   }

   return nErr;
}




