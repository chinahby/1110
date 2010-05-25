/*======================================================
FILE:       BitmapWidget.c

SERVICES:   Widget that draws bitmaps in 

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdLib.h"

#include "AEEWProperties.h"
#include "BitmapWidget.h"
#include "AEEDisplayCanvas.h"
#include "wutil.h"



//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------


#define ME_FROM_WIDGET    BitmapWidget *me = ((BitmapWidget*)po)


static void BitmapWidget_SetTranspColor(BitmapWidget *me)
{
   if (me->piBitmap && me->rgbTransp != RGB_NONE) {
      NativeColor nc = IBITMAP_RGBToNative(me->piBitmap, me->rgbTransp);
      IBITMAP_SetTransparencyColor(me->piBitmap, nc);
   }
}

static __inline void BitmapWidget_Invalidate(BitmapWidget *me) 
{ 
   WidgetBase_InvalidateContent((IWidget*)me); 
}


static void BitmapWidget_ModelChanged(BitmapWidget *me, ModelEvent *pEvent)
{
   RELEASEIF(me->piBitmap);

   if (me->base.piModel &&
         (SUCCESS == IINTERFACEMODEL_GetIPtr((IInterfaceModel*)me->base.piModel, 
                                              AEECLSID_BITMAP, (void **)&me->piBitmap))) {
   
      AEEBitmapInfo abi;
      
      IBITMAP_GetInfo(me->piBitmap, &abi, sizeof(abi));
   
      // resize extent if zero.
      if (me->base.extent.width == 0 ||
          me->base.extent.height == 0) {
         WExtent we;

         we.width = me->base.extent.width == 0 ? abi.cx : me->base.extent.width;
         we.height = me->base.extent.height == 0 ? abi.cy : me->base.extent.height;
         WidgetBase_SetExtent((IWidget*)&me->base, &we);   
         Border_CalcClientRect(&me->border);
      }

      // apply transparency color...
      // me->rgbTransp applies to the widget
      // and is not restricted to the current bitmap
      BitmapWidget_SetTranspColor(me);
   }

   BitmapWidget_Invalidate(me);
}


boolean BitmapWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch(evt) {
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_TRANSP_COLOR) {
            if (dwParam) {
               *(uint32 *)dwParam = (uint32)me->rgbTransp;
            }
         } else if (wParam == PROP_FLAGS){
            if (dwParam) {
               *(uint32 *)dwParam = (uint32)me->dwFlags;
            }
         } else {
            break;
         }
         return TRUE;

      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_TRANSP_COLOR) {
            me->rgbTransp = (RGBVAL)dwParam;
            BitmapWidget_SetTranspColor(me);
      
         } else if (wParam == PROP_FLAGS ){
            me->dwFlags = dwParam;
         } else {
            break;
         }
         
         BitmapWidget_Invalidate(me);
         return TRUE;

   }
   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}



void BitmapWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;
   
   if (me->piBitmap) {
      AEEBitmapInfo abi;
      IBITMAP_GetInfo(me->piBitmap, &abi, sizeof(abi));
      pweOut->width  = abi.cx;
      pweOut->height = abi.cy;
   } else {
      pweOut->width  = 10;
      pweOut->height = 10;
   }

   Border_CalcPreferredExtent(&me->border, pweOut, pweOut);
}


void BitmapWidget_SetExtent(IWidget *po, WExtent *we)
{
   ME_FROM_WIDGET;
   WidgetBase_SetExtent((IWidget*)&me->base, we);
   Border_CalcClientRect(&me->border);
}


int BitmapWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;

   IInterfaceModel *piim = NULL;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_INTERFACEMODEL, (void **)&piim)) {
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(po, (IModel*)piim);
   RELEASEIF(piim);

   // Re-register listener
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Init(&me->modelListener, BitmapWidget_ModelChanged, me);
   IMODEL_AddListener(me->base.piModel, &me->modelListener);

   BitmapWidget_ModelChanged(me, NULL);
   return SUCCESS;
}


void BitmapWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   Border_Draw(&me->border, piCanvas, x, y);
   if (me->piBitmap) {
      AEERect  rcClip;
      IBitmap *pDstBitmap = NULL;

      ZEROAT(&rcClip);
      (void) ICANVAS_GetClipRect(piCanvas, &rcClip);

      (void) ICANVAS_GetBitmap(piCanvas, &pDstBitmap);

      if (pDstBitmap != NULL) {
         int ix = 0, iy = 0;       // offset for image to be drawn (within clipping rect)            
         int idx = 0, idy = 0;
         AEERect     rc, 
                     rcDest;
         AEERasterOp rop = (me->rgbTransp == RGB_NONE) ? AEE_RO_COPY : AEE_RO_TRANSPARENT;
         AEEBitmapInfo bi;
         
         IBITMAP_GetInfo(me->piBitmap, &bi, sizeof(bi));
        
         // Horizontal alignment - ensure extent is large enough to show alignment
         if (bi.cx < (uint32)me->border.rcClient.dx) {         
            idx = me->border.rcClient.dx - bi.cx;
            if (me->dwFlags & IDF_ALIGN_RIGHT)
               ix += me->border.rcClient.dx - bi.cx;
            else if (me->dwFlags & IDF_ALIGN_CENTER)
               ix += (me->border.rcClient.dx - bi.cx)/2;
         } 
         
         // Vertical alignment
         if (bi.cy < (uint32)me->border.rcClient.dy) {
            idy += me->border.rcClient.dy - bi.cy;
            if (me->dwFlags & IDF_ALIGN_BOTTOM)
               iy += me->border.rcClient.dy - bi.cy;                    
            else if (me->dwFlags & IDF_ALIGN_MIDDLE)
               iy += (me->border.rcClient.dy - bi.cy)/2;
         }

         SETAEERECT(&rcDest, x + me->border.rcClient.x + ix, y + me->border.rcClient.y + iy,
            me->border.rcClient.dx-idx, me->border.rcClient.dy-idy);

         if (IntersectRect(&rc, &rcDest, &rcClip)) {
            int xSrc = 0, ySrc = 0;   // bitmap coordinates of where the blt begins        

            // If we are clipped past our bitmap start, adjust source location and dimensions
            if (rc.x > rcDest.x) {
               xSrc += rc.x - rcDest.x;               
            }
            if (rc.y > rcDest.y) {
               ySrc += rc.y - rcDest.y;                              
            }


           (void) IBITMAP_BltIn(pDstBitmap, rc.x, rc.y, rc.dx, rc.dy,
                               me->piBitmap, xSrc, ySrc, 
                               rop);
         }
         IBITMAP_Release(pDstBitmap);
      }
   }
}


boolean BitmapWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return ((me->rgbTransp == RGB_NONE) 
            ? WidgetBase_IntersectOpaque_O(po, prcOut, prcIn) 
            : WidgetBase_IntersectOpaque(po, prcOut, prcIn));
}


void BitmapWidget_Dtor(BitmapWidget *me)
{
   RELEASEIF(me->piBitmap);
   WidgetBase_Dtor(&me->base);
   Border_Dtor(&me->border);
}

uint32 BitmapWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      BitmapWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}

void BitmapWidget_Ctor(BitmapWidget *me, AEEVTBL(IWidget) *pvt, 
                       IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   // contruct base
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(BitmapWidget_HandleEvent));

   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, FALSE, &WBASE(me)->piViewModel);

   // construct derived members
   pvt->Release            = BitmapWidget_Release;
   pvt->GetPreferredExtent = BitmapWidget_GetPreferredExtent;
   pvt->SetExtent          = BitmapWidget_SetExtent;
   pvt->Draw               = BitmapWidget_Draw;
   pvt->IntersectOpaque    = BitmapWidget_IntersectOpaque;
   pvt->SetModel           = BitmapWidget_SetModel;

   me->rgbTransp = RGB_NONE;

   IWIDGET_SetBorderWidth(BITMAPWIDGET_TO_IWIDGET(me), 0);
   IWIDGET_SetBGColor(BITMAPWIDGET_TO_IWIDGET(me), RGBA_NONE);
   IWIDGET_SetPadding(BITMAPWIDGET_TO_IWIDGET(me), 0);
}


int BitmapWidget_Initialize(BitmapWidget *me, IShell *piShell)
{
   int nErr;
   IModel *piModel = 0;

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_INTERFACEMODEL, (void**)&piModel);
   
   if (!nErr) {
      BitmapWidget_SetModel((IWidget*)me, piModel);
   }

   RELEASEIF(piModel);

   return nErr;
}


int BitmapWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int nErr;
   BitmapWidget *me = MALLOCREC_VTBL(BitmapWidget, IWidget);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   BitmapWidget_Ctor(me, GETVTBL(me,IWidget), piShell, piModule, 0);
   nErr = BitmapWidget_Initialize(me, piShell);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      BitmapWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}


