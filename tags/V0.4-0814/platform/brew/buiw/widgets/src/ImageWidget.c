/*======================================================
FILE:       ImageWidget.c

SERVICES:   Widget that draws images and/or backgrounds

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdLib.h"
#include "WidgetBase.h"
#include "ImageWidget.h"
#include "ModelBase.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"
#include "AEEBitmap.h"


#define ME_FROM_DECORATOR    ImageWidget *me = ((ImageWidget*)po)
#define WBASE(p)          ((WidgetBase *)p)

#define OFFSETRECT(prc, nx, ny)  (prc)->x+=(nx),(prc)->y+=(ny)

#ifndef IPARM_DISPLAY
#define IPARM_DISPLAY   8
#endif

#ifndef IPARM_SCALE  
#define IPARM_SCALE     12
#endif


// IBitmap to IImage adapter
int BitmapImage_New(IBitmap *piBitmap, IImage **ppi);


// Transparency Operations
#define kSourceOpaque      0
#define kSourceAlpha       1
#define kSourceTransparent 2


static int BilinearScale(IDisplay *piDisplay, IBitmap *pSrc, AEERect *prcSrc, IBitmap *pDest,  AEERect *prcDest, AEERect *prcClip, int dwTransOp);
static int NBitTo32BitLine(IDIB *pbmSrc, int ySrc, IDIB *pbmDst, int dwWidth, boolean bAlpha);

static void ImageWidget_GetImage(ImageWidget *me, IImage **ppo)
{
   IInterfaceModel *piModel = (IInterfaceModel*) WBASE(me)->piModel;

   if (piModel) {
      if (SUCCESS != IINTERFACEMODEL_GetIPtr(piModel, AEEIID_IMAGE, (void **)ppo)) {
         IBitmap *pib = 0;
         if (SUCCESS == IINTERFACEMODEL_GetIPtr(piModel, AEECLSID_BITMAP, (void **)&pib)) {
            (void) BitmapImage_New(pib, ppo);
         }
         RELEASEIF(pib);
      }
   }
}


static void ImageWidget_SetBitmapTranspColor(ImageWidget *me)
{
   IInterfaceModel *piModel = CAST(IInterfaceModel*, WBASE(me)->piModel);

   if (piModel) {
      IBitmap *pib = 0;
      if (SUCCESS == IINTERFACEMODEL_GetIPtr(piModel, AEECLSID_BITMAP, (void **)&pib)) {
         NativeColor nc = IBITMAP_RGBToNative(pib, me->rgbTransp);
         if(me->rgbTransp  != RGB_NONE) {
            (void) IBITMAP_SetTransparencyColor(pib, nc);
            if(me->image.pii) {
               IIMAGE_SetParm(me->image.pii, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
            }
         }
         else {
            if(me->image.pii) {
               IIMAGE_SetParm(me->image.pii, IPARM_ROP, AEE_RO_COPY, 0);
            }
         }
      }
      RELEASEIF(pib);
   }
}

static void ImageWidget_DoModelChanged(ImageWidget *me)
{
   IImage *pii = 0;
   ImageWidget_GetImage(me, &pii);
   ImageBase_SetImage(&me->image, pii);
   ImageWidget_SetBitmapTranspColor(me);
   // this code from the original ImageWidget should probably be removed
   // however, someone may be relying on this auto-size functionality
   if (pii && (WBASE(me)->extent.width == 0 
            || WBASE(me)->extent.height == 0)) {  

      AEEImageInfo aii;
      boolean bInval = 0;

      IIMAGE_GetInfo(pii, &aii);

      if (WBASE(me)->extent.width == 0) {
         WBASE(me)->extent.width = aii.cxFrame;
         if (me->dwFlags & IWF_HASSELECTIMAGE) {
            WBASE(me)->extent.width /= 2;
         }
         bInval = 1;
      }
      if (WBASE(me)->extent.height == 0) {
         WBASE(me)->extent.height = aii.cy;
         bInval = 1;
      }
   
      // Extent change
      if (bInval) {
         WidgetBase_InvalidateExtent((IWidget*)me);
      }
   }

   RELEASEIF(pii);
}


static void ImageWidget_ModelChanged(ImageWidget *me, ModelEvent *pEvent)
{
   ImageWidget_DoModelChanged(me);
}

void ImageWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   ME_FROM_DECORATOR;
   
   Decorator_GetPreferredExtent(po, pweOut);

   if (me->image.pii) {

      AEEImageInfo aii;
      IIMAGE_GetInfo(me->image.pii, &aii);

      // override image info's notion of image size because it's
      // broken in some earlier versions of BREW.  Use the values
      // we know to be correct from the last call to set the scale
      if (me->image.bScaled) {
         aii.cxFrame = me->image.sdx;
         aii.cy = me->image.sdy;
      }

      pweOut->width  = MAX((me->image.dx ? me->image.dx : aii.cxFrame), pweOut->width);
      pweOut->height = MAX((me->image.dy ? me->image.dy : aii.cy), pweOut->height);

      // If the bitmap has 2 images (selected, non selected) we need to divide the bitmap in half
      if (me->dwFlags & IWF_HASSELECTIMAGE) {
         pweOut->width /= 2;
      }
   } 

   Border_CalcPreferredExtent(&me->border, pweOut, pweOut);
}


void ImageWidget_SetExtent(IDecorator *po, WExtent *pweIn)
{
   ME_FROM_DECORATOR;
   WExtent ce;

   WidgetBase_SetExtent((IWidget*)&me->base.base, pweIn);
   Border_CalcClientRect(&me->border);
   SETWEXTENT(&ce, me->border.rcClient.dx, me->border.rcClient.dy);
   Decorator_SetExtent(po, &ce);
}


void ImageWidget_GetExtent(IDecorator *po, WExtent *pweOut)
{
   WidgetBase_GetExtent(IDECORATOR_TO_IWIDGET(po), pweOut);
}


boolean ImageWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_DECORATOR;

   boolean bSaveSel   = me->border.bSelected;
   AEERect *prcClient = &me->border.rcClient;
   AEERect rcSave     = *prcClient;


   // if we have a child, let it handle these focus events first
   if (me->base.pChild && 
       (evt == EVT_WDG_HASFOCUS || evt == EVT_WDG_CANTAKEFOCUS)) {
      return Decorator_HandleEvent(po, evt, wParam, dwParam);
   }

   // Deal with border properties first
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (rcSave.dx != prcClient->dx ||
          rcSave.dy != prcClient->dy) {
         // client rect changed set extent again to affect child 
         IWIDGET_SetExtent(IDECORATOR_TO_IWIDGET(po), &WBASE(me)->extent);
      }
      return TRUE;
   }

   if (evt == EVT_WDG_SETPROPERTY) {
      switch(wParam) {
      case PROP_FLAGS: {
         uint32 dwSave = me->dwFlags;

         me->dwFlags = dwParam;

         if ((dwSave & IWF_HASSELECTIMAGE) != (me->dwFlags & IWF_HASSELECTIMAGE)) {
            me->image.bHasSelectImage = (0 != (me->dwFlags & IWF_HASSELECTIMAGE));
         }
         if ((dwSave & IWF_TILED) != (me->dwFlags & IWF_TILED)) {
            me->image.bTiled = (0 != (me->dwFlags & IWF_TILED));
         }

         // strip out alignment flags and set in imagebase
         me->image.dwAlignment = me->dwFlags & IDF_ALIGN_MASK;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      }

      case PROP_SELECTED:
         if (bSaveSel != me->border.bSelected) {
            ImageBase_SetSelected(&me->image, me->border.bSelected);
         }
         return TRUE;

      case PROP_IMAGE_PARM:
         ImageBase_SetParm(&me->image, (ImageParm *)dwParam);
         return TRUE;

      case PROP_IMAGE_ANIMATE:
         ImageBase_Animate(&me->image, (boolean)dwParam);
         return TRUE;

      case PROP_TILED_BDT:
         me->image.bdt = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDB:
         me->image.bdb = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDL:
         me->image.bdl = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDR:
         me->image.bdr = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;

      case PROP_IMAGE_FRAME:
         return ImageBase_SetFrame(&me->image, (int)dwParam);

      case PROP_ANIMATE_FLAGS:
         me->image.dwAnimFlags = dwParam;
         return TRUE;
      
      case PROP_TRANSP_COLOR:
         me->rgbTransp = (RGBVAL)dwParam;
         ImageWidget_SetBitmapTranspColor(me);
         return TRUE;

      }

   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case PROP_FLAGS:
         *(uint32 *)dwParam = me->dwFlags;
         return TRUE;
      case PROP_IMAGE_ANIMATE:
         *(uint32 *)dwParam = me->image.bAnimated;
         return TRUE;
      case PROP_TILED_BDT:
         *(uint16 *)dwParam = me->image.bdt;
         return TRUE;
      case PROP_TILED_BDB:
         *(uint16 *)dwParam = me->image.bdb;
         return TRUE;
      case PROP_TILED_BDL:
         *(uint16 *)dwParam = me->image.bdl;
         return TRUE;
      case PROP_TILED_BDR:
         *(uint16 *)dwParam = me->image.bdr;
         return TRUE;
      case PROP_IMAGE_FRAME:
         *(int *)dwParam = me->image.nCurFrame - 1;   // internal frame count is 1-based
         return TRUE;
      case PROP_IMAGE_NFRAMES:
         // @@@ TODO fix for Cambridge (return 0 for 'unknown'
         // default frame count
         *(int*)dwParam = me->image.nFrames ? me->image.nFrames : 1;
         return TRUE;
      case PROP_ANIMATE_FLAGS:
         *(uint32*)dwParam = me->image.dwAnimFlags;
         return TRUE; 
      case PROP_TRANSP_COLOR:
           if (dwParam) {
               *(uint32 *)dwParam = (uint32)me->rgbTransp;
               return TRUE;
            }
           return FALSE;
      }
   }

   return Decorator_HandleEvent(po, evt, wParam, dwParam);
}


int ImageWidget_GetModel(IDecorator *po, AEECLSID clsid, IModel **ppo)
{
   ME_FROM_DECORATOR;

   if (WBASE(me)->piModel) {
      return IINTERFACEMODEL_QueryInterface((IInterfaceModel *)WBASE(me)->piModel, clsid, (void **)ppo);
   } else {
      *ppo = 0;
      return EFAILED;
   }
}

int ImageWidget_SetModel(IDecorator *po, IModel *piModel)
{
   ME_FROM_DECORATOR;

   IInterfaceModel *piim = NULL;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_INTERFACEMODEL, (void **)&piim)) {
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(IDECORATOR_TO_IWIDGET(po), (IModel*)piim);

   // Re-register listener
   IMODEL_AddListenerEx(WBASE(me)->piModel, &me->modelListener, 
                        (PFNLISTENER)ImageWidget_ModelChanged, me);

   ImageWidget_DoModelChanged(me);

   // Clean up
   RELEASEIF(piim);

   return SUCCESS;
}

void ImageWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;
   Border_Draw(&me->border, piCanvas, x, y);
   ImageBase_Draw(&me->image, piCanvas, 
                  x + me->border.rcClient.x, 
                  y + me->border.rcClient.y, 
                  me->border.rcClient.dx,
                  me->border.rcClient.dy);
   Decorator_Draw(po, piCanvas, x, y);
}


void ImageWidget_Dtor(ImageWidget *me)
{
   ImageBase_Dtor(&me->image);
   LISTENER_Cancel(&me->modelListener);
   Decorator_Dtor(&me->base);
   Border_Dtor(&me->border);
}


uint32 ImageWidget_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if (WBASE(me)->nRefs == 1) {
      ImageWidget_Dtor(me);
   }
   return Decorator_Release(po);
}


void ImageWidget_Ctor(ImageWidget *me, AEEVTBL(IDecorator) *pvt, IShell *piShell, 
                      IModule *piModule, PFNHANDLER pfnDefHandler)
{
   // construct base
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ImageWidget_HandleEvent));
   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &WBASE(me)->extent, FALSE, &WBASE(me)->piViewModel);

   // construct derived members
   pvt->Release            = ImageWidget_Release;
   pvt->Draw               = ImageWidget_Draw;
   pvt->GetPreferredExtent = ImageWidget_GetPreferredExtent;
   pvt->SetExtent          = ImageWidget_SetExtent;
   pvt->GetExtent          = ImageWidget_GetExtent;
   pvt->GetModel           = ImageWidget_GetModel;
   pvt->SetModel           = ImageWidget_SetModel;

   IWIDGET_SetBorderWidth(IMAGEWIDGET_TO_IWIDGET(me), 0);
   IWIDGET_SetBGColor(IMAGEWIDGET_TO_IWIDGET(me), RGBA_NONE);
   IWIDGET_SetPadding(IMAGEWIDGET_TO_IWIDGET(me), 0);

   ImageBase_Ctor(&me->image, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, 
                  &WBASE(me)->piViewModel);
}


int ImageWidget_Initialize(ImageWidget *me)
{
   int nErr;
   IModel *piModel = 0;

   nErr = InterfaceModel_New((IInterfaceModel**)&piModel, WBASE(me)->piModule);
   
   if (!nErr) {
      ImageWidget_SetModel((IDecorator*)me, piModel);
   }

   RELEASEIF(piModel);

   return nErr;
}


int ImageWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule)
{
   int nErr;
   ImageWidget *me = MALLOCREC_VTBL(ImageWidget, IDecorator);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   ImageWidget_Ctor(me, GETVTBL(me,IDecorator), piShell, piModule, 0);
   nErr = ImageWidget_Initialize(me);

   if (!nErr) {
      *ppo = (IDecorator*)me;
   } else {
      ImageWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// IBitmap -> IImage adapter


#define UNUSED(a) 


typedef struct BitmapImage {
   AEEVTBL(IImage)*  pvt;        // pointer to our vtbl
   uint32            nRefs;      // refcount
   IBitmap *         piBitmap;   // IBitmap to draw
   IDisplay *        piDisplay;  // cached IDisplay interface to use
   AEERasterOp       rop;        // ROP to use when drawing the IBitmap
   AEEBitmapInfo     bi;         // bitmap info
   AEEVTBL(IImage)   vt;         // our IImage vtbl
   int16             xOffset;    
   int16             yOffset;
   int16             dx;
   int16             dy;
   int16             dxScale;
   int16             dyScale;
} BitmapImage;


static void BitmapImage_Draw(IImage *po, int x, int y)
{
   IIMAGE_DrawFrame(po,-1,x,y);
}

static void BitmapImage_DrawFrame(IImage *po, int nFrame, int x, int y)
{
   UNUSED(nFrame)
   BitmapImage *me = (BitmapImage*)po;

   if (me->piDisplay) {

      if (me->dxScale != (int16)me->bi.cx || me->dyScale != (int16)me->bi.cy) {

         IBitmap *pbmDst = IDISPLAY_GetDestination(me->piDisplay);

         if (pbmDst) {

            AEERect rcSrc;
            AEERect rcDst;
            AEERect rcClip;
            int xFrame = 0;   // unused for now, will be used when we support frames

            int nTrans = kSourceOpaque;

            if (AEE_RO_TRANSPARENT == me->rop) {
               nTrans = kSourceTransparent;
            } else if (AEE_RO_BLEND == me->rop) {
               nTrans = kSourceAlpha;
            }

            rcSrc.x = me->xOffset + xFrame;
            rcSrc.y = me->yOffset;
            rcSrc.dx = (uint16)me->bi.cx;
            rcSrc.dy = (uint16)me->bi.cy;

            rcDst.x = x;
            rcDst.y = y;
            rcDst.dx = me->dxScale;
            rcDst.dy = me->dyScale;

            IDISPLAY_GetClipRect(me->piDisplay, &rcClip);

            (void) BilinearScale(me->piDisplay, me->piBitmap, &rcSrc, 
                                 pbmDst, &rcDst, &rcClip, nTrans);

            IBITMAP_Release(pbmDst);

         }

      } else {
         IDISPLAY_BitBlt(me->piDisplay, x, y, (int) me->dx,(int) me->dy,
                         me->piBitmap, (int) me->xOffset, (int) me->yOffset, me->rop);
      }
   }
}

static void BitmapImage_GetInfo(IImage *po, AEEImageInfo *paii)
{
   BitmapImage *me = (BitmapImage*)po;
   paii->cx          = (uint16) me->bi.cx;
   paii->cy          = (uint16) me->bi.cy;
   paii->nColors     = (uint16)(1 << me->bi.nDepth);
   paii->cxFrame     = paii->cx;
   paii->bAnimated   = 0;
}

static void BitmapImage_SetParm(IImage *po, int nParm, int p1, int p2)
{                                                      
   BitmapImage *me = (BitmapImage*)po;

   switch (nParm) {
      case IPARM_DISPLAY:
         RELEASEIF(me->piDisplay);
         me->piDisplay = (IDisplay *) p1;  
         ADDREFIF(me->piDisplay);
         break;
   
      case IPARM_ROP:
         me->rop = (AEERasterOp) p1;
         break;
   
      case IPARM_SIZE:
         me->dx = (int16) CONSTRAIN(p1, 0, (int)me->bi.cx);
         me->dy = (int16) CONSTRAIN(p2, 0, (int)me->bi.cy);
         break;
   
      case IPARM_OFFSET:
         me->xOffset = (int16)p1;
         me->yOffset = (int16)p2;
         break;

      case IPARM_SCALE:
         me->dxScale = (int16)p1;
         me->dyScale = (int16)p2;
         me->dx = (int16)p1;
         me->dy = (int16)p2;
         break;
   }
}


static void BitmapImage_Start(IImage *po, int x, int y)
{
   UNUSED((po,x,y))
}

static void BitmapImage_Stop(IImage *po)
{
   UNUSED(po)
}

static void BitmapImage_SetStream(IImage *po, IAStream *ps)
{
   UNUSED((po,ps))
}

static boolean BitmapImage_HandleEvent(IImage *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   UNUSED((po,evt,wParam,dwParam))
   return FALSE;
}

static void BitmapImage_Notify(IImage *po, PFNIMAGEINFO  pfn, void *pUser)
{
   UNUSED((po,pfn,pUser))
}

static uint32 BitmapImage_AddRef(IImage *po)
{
   BitmapImage *me = (BitmapImage*)po;
   return ++me->nRefs;
}

static uint32 BitmapImage_Release(IImage *po)
{
   BitmapImage *me = (BitmapImage*)po;

   if (--me->nRefs) {
      return me->nRefs;
   }

   RELEASEIF(me->piBitmap);
   RELEASEIF(me->piDisplay);
   FREE(me);
   return 0;
}

static void BitmapImage_SetBitmap(BitmapImage *me, IBitmap *piBitmap)
{
   if (piBitmap) {
      RELEASEIF(me->piBitmap);
      me->piBitmap = piBitmap;
      IBITMAP_AddRef(me->piBitmap);
      (void) IBITMAP_GetInfo(me->piBitmap, &me->bi, sizeof(me->bi));
      me->dxScale = me->dx = (int16) me->bi.cx;
      me->dyScale = me->dy = (int16) me->bi.cy;
      me->xOffset = 0;
      me->yOffset = 0;

   }
}

int BitmapImage_New(IBitmap *piBitmap, IImage **ppi)
{
   BitmapImage *me = 0;

   *ppi = 0;

   if (!piBitmap) {
      return EBADPARM;
   }

   // Allocate some memory for the object
   me = MALLOCREC(BitmapImage);
   if (!me) {
      return ENOMEMORY;
   }

   *ppi = (IImage*)me;

   me->nRefs   = 1;
   me->rop     = AEE_RO_COPY;

   // Init the IImage vtable
   me->pvt = &me->vt;
   me->vt.AddRef      = BitmapImage_AddRef; 
   me->vt.Release     = BitmapImage_Release; 
   me->vt.Draw        = BitmapImage_Draw; 
   me->vt.DrawFrame   = BitmapImage_DrawFrame; 
   me->vt.GetInfo     = BitmapImage_GetInfo; 
   me->vt.SetParm     = BitmapImage_SetParm; 
   me->vt.Start       = BitmapImage_Start; 
   me->vt.Stop        = BitmapImage_Stop; 
   me->vt.SetStream   = BitmapImage_SetStream; 
   me->vt.HandleEvent = BitmapImage_HandleEvent; 
   me->vt.Notify      = BitmapImage_Notify; 

   BitmapImage_SetBitmap(me, piBitmap);

   return SUCCESS;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Bilinear scaling


// BilinearRowPixelWeight 
// Used to store how each pixel in a row should be weighted
typedef struct {
   int dwSrcX;
   int p256;
   int p1256;
} BilinearRowPixelWeight;


// macros to extract r, g and b from IDIB_COLORSCHEME_888 DIBs
#define RGB_888_BLUE(rgb)   ((byte)((uint32)(rgb) & 0x000000FF))
#define RGB_888_GREEN(rgb)  ((byte)(((uint32)(rgb) & 0x0000FF00) >> 8))
#define RGB_888_RED(rgb)    ((byte)(((uint32)(rgb) & 0x00FF0000) >> 16))
#define RGB_888_ALPHA(rgb)  ((byte)(((uint32)(rgb) & 0xFF000000) >> 24))
   
// macro to combine pixels
#define BILINEAR_COMBINE(a,b,c,d,p,p1,q,q1) ((((a*p1 + b*p)*q1) + ((c*p1 + d*p)*q)) >> 16)

// macro to construct the alpha channel
#define BUILD_ALPHA_CHANNEL(ps,sp,pd,x) {       \
   if (sp != ps->ncTransparent)                 \
      ((uint32 *)(pd->pBmp))[x] |= 0xFF000000;  \
}


static int BilinearScale(IDisplay *piDisplay, IBitmap *pSrc, AEERect *prcSrc, IBitmap *pDest,  AEERect *prcDest, AEERect *prcClip, int dwTransOp)
{ 
   int dwResult = SUCCESS;          
   AEEBitmapInfo si,di;             // size of the source and destination bitmaps
   int i, j;                        // column and row iterators
   int dwSrcY;                      // current scanline
   int dwOldSrcY;                   // previous scanline
   int q256, q1256;                 // how to weight the pixels in two neighboring rows
   uint32 rgba,rgbb,rgbc,rgbd;      // rgb values of four pixels to combine
   uint32 t;                        // keeps track of which of the four pixels are transparent
   int rr, rg, rb, ra;              // combined r,g,b,a channels
   AEERect rcSrc, rcDest, rcClip;   // source, destination and clip rects
   AEERect rc;                      // for rect calculations
   AEERect rcValidSrc;              // the part of the source rect that overlaps the source bitmap
   int *pDestPixel;                 // the pixel in the destination that's being set
   IDIB *pSrcLine1 = NULL, *pSrcLine2 = NULL, *pSwap, *pDestDib = NULL, *pSrcDib = NULL;
   BilinearRowPixelWeight *pRowWeights = NULL, *pRowPixelPtr = NULL;
   AEERasterOp rop = AEE_RO_COPY;   // how to combine scaled image and destination
   boolean bSrcHasTransClr;         // true if the source image has a transparent color
   int dwDestStart;                 // the first destination pixel to compute 
   int dwDestEnd;                   // the last destination pixel to compute.

   //
   // Verify Parameters
   //

   if (!pSrc || !pDest)
      return (EBADPARM);

   // get the source DIB
   dwResult = IBITMAP_QueryInterface(pSrc, AEECLSID_DIB, (void **)&pSrcDib);
   if (dwResult != SUCCESS)
      return (dwResult);
  
   //
   // Setup: determine the the size of the source, how 
   // much the image is to be scaled by and how much of
   // the scaled image to actually compute and draw.
   //

   // determine the size of the source
   IBITMAP_GetInfo(pSrc, &si, sizeof(AEEBitmapInfo));
   if (prcSrc) {
      rcSrc = *prcSrc;
      SETAEERECT(&rc, 0, 0, si.cx, si.cy);
      IntersectRect(&rcValidSrc, prcSrc, &rc);
   } else {
      // scale the entire source image
      SETAEERECT(&rcSrc, 0, 0, si.cx, si.cy);
      rcValidSrc = rcSrc;
   }
   bSrcHasTransClr = (dwTransOp == kSourceTransparent);

   // determine the size of the destination
   IBITMAP_GetInfo(pDest, &di, sizeof(AEEBitmapInfo));
   if (prcDest) {
      rcDest = *prcDest;
   } else {
      // scale source to the size of the entire destination bitmap
      SETAEERECT(&rcDest, 0, 0, di.cx, di.cy);
   }

   // determine which part of the destination image we'll actually draw
   if (prcClip) {
      // draw the intersection of the destination , destination rect and clip rectangle
      SETAEERECT(&rcClip, 0, 0, di.cx, di.cy);
      IntersectRect(&rc, &rcClip, &rcDest);  
      IntersectRect(&rcClip, &rc, prcClip);
   } else {
      // draw the intersection of the destination rectangle and the destination bitmap
      SETAEERECT(&rc, 0, 0, di.cx, di.cy);
      IntersectRect(&rcClip, &rcDest, &rc);
   }

   // since we will be using rcSrc.dx/dy and rcDest.dx/dy as divisors,
   // make sure they are not zero
   rcSrc.dx  = MAX(1, rcSrc.dx);
   rcSrc.dy  = MAX(1, rcSrc.dy);
   rcDest.dx = MAX(1, rcDest.dx);
   rcDest.dy = MAX(1, rcDest.dy);

   // limit the amount of the destination that we'll draw to those pixels
   // that can be computed from valid source data.  That is, properly handle
   // the case where prcSrc does not lie completely within pSrc.

   // first pixel to compute
   dwDestStart = (((rcValidSrc.x - rcSrc.x) * rcDest.dx * 256) / rcSrc.dx) + (rcDest.x * 256);
   // use next full pixel
   if ((dwDestStart & 0xFF) != 0)
      dwDestStart += 0x100;
   dwDestStart >>= 8;

   // pixel immediately beyond the last pixel to compute
   // no need for precision here.
   dwDestEnd = ((((rcValidSrc.x + rcValidSrc.dx) - rcSrc.x) * rcDest.dx )/rcSrc.dx) + rcDest.x ;

   // make sure start and end fall within the rcClip
   dwDestStart = MAX(dwDestStart, rcClip.x);
   dwDestEnd = MIN(dwDestEnd, rcClip.x + rcClip.dx);

   //
   // Initialization: allocate the line buffers we use for
   // better performance, get the destination DIB and 
   // precompute how pixels in a row of the source should
   // be weighted for later combination.
   //

   // create 23 bit horizontal scan lines from the source


   dwResult = IDISPLAY_CreateDIBitmap(piDisplay, &pSrcLine1, 32, (uint16)(si.cx+1), 1);
   if (dwResult == SUCCESS) {
      pSrcLine1->nColorScheme = IDIB_COLORSCHEME_888;
      dwResult = IDISPLAY_CreateDIBitmap(piDisplay, &pSrcLine2, 32, (uint16)(si.cx+1), 1);
      if (dwResult == SUCCESS) {
         pSrcLine2->nColorScheme = IDIB_COLORSCHEME_888;

         // create a single row destination.  We'll blit it into the destination bitmaps when it's full.
         dwResult = IDISPLAY_CreateDIBitmap(piDisplay, &pDestDib, 32, rcClip.dx, 1);
         if (dwResult == SUCCESS) {
            pDestDib->nColorScheme = IDIB_COLORSCHEME_888;

            // if we're paying attention to transparency, be sure to blend the result.
            if ((dwTransOp == kSourceAlpha) || (dwTransOp == kSourceTransparent))
               rop = AEE_RO_BLEND;

            // Note, we could iterate from the left edge to the right edge of rcClip and
            // simply skip any pixels where dwSrcX falls outside of the rcValidSrc rect.
            // Precomputing those above saves us if statements in the inner loop below
            // and it saves us memory here, though.  Plus it's real cool.

            // precompute the row weights
            pRowWeights = (BilinearRowPixelWeight *)MALLOC((dwDestEnd-dwDestStart) * sizeof(BilinearRowPixelWeight));
            if ((pRowPixelPtr = pRowWeights)!=NULL) {
               for (j = dwDestStart; j < dwDestEnd; j++) {

                  // find the right column in the source and determine how to weight the surrounding pixels
                  pRowPixelPtr->dwSrcX = (256*rcSrc.x) + ((j - rcDest.x) * rcSrc.dx * 256)/rcDest.dx;
                  pRowPixelPtr->p256 = (pRowPixelPtr->dwSrcX)&0xFF;
                  pRowPixelPtr->p1256 = 256 - pRowPixelPtr->p256;
                  pRowPixelPtr->dwSrcX >>= 8;

                  pRowPixelPtr++;
               }
            } else
               dwResult = ENOMEMORY;
         }
      }
   }
   
   // invalidate the destination before we start.  This speeds things up slightly.      
   IBITMAP_Invalidate(pDest, &rcClip);

   //
   // Scale the image.
   //
       
   if (dwResult == SUCCESS)
   { 
      // make sure we start out by filling both line buffers
      dwOldSrcY = -2;

      for (i = rcClip.y; i < (rcClip.y + rcClip.dy); i++) {

         //
         // Get the next two lines of rgb data from the source image
         //

         // find the right row in the source this pixel should be computed from
         // and compute how the columns should be weighted
         dwSrcY = (256*rcSrc.y) + ((i - rcDest.y) * rcSrc.dy * 256)/rcDest.dy;
         q256 = dwSrcY&0xFF;
         q1256 = 256 - q256;
         dwSrcY >>= 8;

         // is this line outside of the intersection of the source bitmap and source rect?
         if ((dwSrcY < rcValidSrc.y) || (dwSrcY >= (rcValidSrc.y + rcValidSrc.dy)))
            continue;

         // update the line buffers with new data from the source image

         // do we need at least one new line?
         if (dwSrcY != dwOldSrcY) {

            // are we looking at the next line in the source?
            if (dwSrcY == (dwOldSrcY + 1)) {

               // then we can use the pSrcLine2 from last time as the first line.
               pSwap = pSrcLine1;
               pSrcLine1 = pSrcLine2;
               pSrcLine2 = pSwap;

            } else {

               // we'll need two new lines.  Get the first.
               NBitTo32BitLine(pSrcDib, dwSrcY, pSrcLine1, si.cx, bSrcHasTransClr);

            }

            // Get the next line from the source.
            if (dwSrcY+1 < (int)si.cy)
               NBitTo32BitLine(pSrcDib, dwSrcY+1, pSrcLine2, si.cx, bSrcHasTransClr);
            else // no more lines.  Reuse line 1.
               MEMCPY(pSrcLine2->pBmp, pSrcLine1->pBmp, (si.cx + 1)*sizeof(uint32));
         }

         // remember line number for next time through.
         dwOldSrcY = dwSrcY;
         
         //
         // Combine pixels from the source
         //

         // setup row ...
         pDestPixel = (int *)(pDestDib->pBmp);
         pRowPixelPtr = pRowWeights;

         if (dwTransOp == kSourceOpaque) {

            // no transparency.  We can ignore the alpha channel during the scale.
            for (j = dwDestStart; j < dwDestEnd; j++) {
                  
               // Get four source pixels from our line buffers
               rgba = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX];
               rgbb = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX+1];
               rgbc = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX];
               rgbd = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX+1];

               // compute the interpolated pixel values
               rr = BILINEAR_COMBINE(RGB_888_RED(rgba),RGB_888_RED(rgbb),RGB_888_RED(rgbc),RGB_888_RED(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               rg = BILINEAR_COMBINE(RGB_888_GREEN(rgba),RGB_888_GREEN(rgbb),RGB_888_GREEN(rgbc),RGB_888_GREEN(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               rb = BILINEAR_COMBINE(RGB_888_BLUE(rgba),RGB_888_BLUE(rgbb),RGB_888_BLUE(rgbc),RGB_888_BLUE(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
         
               *pDestPixel =  (rb & 0xff)           // blue: bits 0-7
                           + ((rg & 0xff) << 8)     // green:bits 8-15
                           + ((rr & 0xff) << 16);   // red:  bits 16-23

               // next pixel
               pDestPixel++;

               // next weight
               pRowPixelPtr++;
            }

         } else if (dwTransOp == kSourceAlpha) {
     
            // the source image has an alpha channel.  Simply combine all for channels, 
            // there's no special transparency color to worry about.

            for (j = dwDestStart; j < dwDestEnd; j++) {

               // Get four source pixels from our line buffers
               rgba = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX];
               rgbb = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX+1];
               rgbc = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX];
               rgbd = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX+1];

               // compute the interpolated pixel values
               rr = BILINEAR_COMBINE(RGB_888_RED(rgba),RGB_888_RED(rgbb),RGB_888_RED(rgbc),RGB_888_RED(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               rg = BILINEAR_COMBINE(RGB_888_GREEN(rgba),RGB_888_GREEN(rgbb),RGB_888_GREEN(rgbc),RGB_888_GREEN(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               rb = BILINEAR_COMBINE(RGB_888_BLUE(rgba),RGB_888_BLUE(rgbb),RGB_888_BLUE(rgbc),RGB_888_BLUE(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               ra = BILINEAR_COMBINE(RGB_888_ALPHA(rgba),RGB_888_ALPHA(rgbb),RGB_888_ALPHA(rgbc),RGB_888_ALPHA(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               
               // set the destination pixel
               *pDestPixel =  (rb & 0xff)           // blue: bits 0-7
                           + ((rg & 0xff) << 8)     // green:bits 8-15
                           + ((rr & 0xff) << 16)    // red:  bits 16-23
                           + ((ra & 0xff) << 24);   // a:    bits 24-31

               // next pixel
               pDestPixel++;

               // next weight
               pRowPixelPtr++;
            }

         } else if (dwTransOp == kSourceTransparent) {

            // the image may have transparent pixels.  Take the alpha channel into account
            for (j = dwDestStart; j < dwDestEnd; j++) {

               // Get four source pixels from our line buffers
               rgba = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX];
               rgbb = ((int *)(pSrcLine1->pBmp))[pRowPixelPtr->dwSrcX+1];
               rgbc = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX];
               rgbd = ((int *)(pSrcLine2->pBmp))[pRowPixelPtr->dwSrcX+1];
            
               // figure out if any of the pixels we're combining are completely transparent
               t = ((rgba&0xFF000000) >> 24) 
                 + ((rgbb&0xFF000000) >> 16)
                 + ((rgbc&0xFF000000) >> 8)
                 + ((rgbd&0xFF000000));

               // combine pixels
               if (t == 0x00000000) {
                  
                  // all of the pixels are completely transparent.  Easy.
                  *pDestPixel = 0x00FFFFFF; 

               } else {

                  // build alpha channel from all four pixels
                  ra = BILINEAR_COMBINE(RGB_888_ALPHA(rgba),RGB_888_ALPHA(rgbb),RGB_888_ALPHA(rgbc),RGB_888_ALPHA(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
               
                  if (t != 0xFFFFFFFF) {

                     // at least one of the pixels is transparent.
                     // use the closest non-transparent pixel instead of completely transparent ones.
                     if ((t&0x000000FF)==0)
                        rgba = (t&0x0000FF00) ? rgbb : ((t&0x00FF0000) ? rgbc : rgbd);
                     if ((t&0x0000FF00)==0)
                        rgbb = rgba;
                     if ((t&0x00FF0000)==0)
                        rgbc = rgba;
                     if ((t&0xFF000000)==0)
                        rgbd = rgba;
                         
                  }

                  // Combine r,g,b channels.  At this point none of the four pixels are completely transparent
                  rr = BILINEAR_COMBINE(RGB_888_RED(rgba),RGB_888_RED(rgbb),RGB_888_RED(rgbc),RGB_888_RED(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
                  rg = BILINEAR_COMBINE(RGB_888_GREEN(rgba),RGB_888_GREEN(rgbb),RGB_888_GREEN(rgbc),RGB_888_GREEN(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);
                  rb = BILINEAR_COMBINE(RGB_888_BLUE(rgba),RGB_888_BLUE(rgbb),RGB_888_BLUE(rgbc),RGB_888_BLUE(rgbd),pRowPixelPtr->p256,pRowPixelPtr->p1256,q256,q1256);

                  // set the destination pixel
                  *pDestPixel =  (rb & 0xff)           // blue: bits 0-7
                              + ((rg & 0xff) << 8)     // green:bits 8-15
                              + ((rr & 0xff) << 16)    // red:  bits 16-23
                              + ((ra & 0xff) << 24);   // a:    bits 24-31

               } 

               // next pixel
               pDestPixel++;

               // next weight
               pRowPixelPtr++;
            }
         }

         // we've completed a row.  blit it into the destination.
         IBITMAP_BltIn(pDest, dwDestStart, i, dwDestEnd - dwDestStart, 1, IDIB_TO_IBITMAP(pDestDib), 0, 0, rop);
      }
   }

   //
   // Cleanup 
   //
   
   if (pSrcLine1)
      IDIB_Release(pSrcLine1);
   if (pSrcLine2)
      IDIB_Release(pSrcLine2);
   if (pDestDib)
      IDIB_Release(pDestDib);
   if (pRowWeights)
      FREE(pRowWeights);
   if (pSrcDib)
      IDIB_Release(pSrcDib);

   return (dwResult);
}

/* NBitTo32BitLine
|| 
|| Description:
||    This routine copies a single horizontal line from the source DIB to the
||    destination DIB, converting the source pixels to 32 bit pixel data in
||    the process.
|| 
|| Prototype:
||    static int NBitTo32BitLine( 
||       IDIB *pbmSrc, 
||       int ySrc,
||       IDIB *pbmDst,
||       int dwWidth,
||       boolean bAlpha);
|| 
|| Parameters:
||     pbmSrc  - the source to read the line of n-bit line of pixels from
||     ySrc    - which horizontal line of the source to read
||     pbmDst  - a DIB dwWidth pixels wide and 1 pixel high for the result
||     dwWidth - the width in pixels of both the source and destination
||     bAlpha  - construct an alpha channel
|| 
|| Return Value:
||    SUCCESS if data was converted
||    EUNSUPPORTED if the source DIB wasn't understood
|| 
|| Comments:
||    IMPORTANT: This routine assumes that pbmSrc is at least dwWidth
||               pixels wide, and the pbmDest is at least dwWidth + 1 pixels
||               wide.  If they're not, this routine will crash.  
*/
static int NBitTo32BitLine(IDIB *pbmSrc, int ySrc, IDIB *pbmDst, int dwWidth, boolean bAlpha)
{
   int dwResult = SUCCESS;
   register int x;   
   byte *pbyBase = pbmSrc->pBmp + (ySrc * pbmSrc->nPitch);
   uint32 dwSrcPixel;

   if ((pbmSrc->nDepth == 32) && (pbmSrc->nColorScheme == IDIB_COLORSCHEME_888)) {
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) {
            ((uint32 *)(pbmDst->pBmp))[x] = ((uint32 *)(pbyBase))[x];

            // build the alpha channel  ..
            BUILD_ALPHA_CHANNEL(pbmSrc, ((uint32 *)(pbyBase))[x], pbmDst, x);
         }
      } else {
         // simple memcopy
         for (x = 0; x < dwWidth; x++)
            ((uint32 *)(pbmDst->pBmp))[x] = ((uint32 *)(pbyBase))[x];
      }
   } else if ((pbmSrc->nDepth == 24) && (pbmSrc->nColorScheme == IDIB_COLORSCHEME_888)) {
      // same as 32 only with more padding
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) {
            // convert source pixel to dest pixel format
            dwSrcPixel = *(pbyBase)            // blue: bits 0-7
                      + (*(pbyBase+1) << 8)    // green:bits 8-15
                      + (*(pbyBase+2)  << 16); // red:  bits 16-23

            ((uint32 *)(pbmDst->pBmp))[x] = dwSrcPixel;

            // build the alpha channel  ..
            BUILD_ALPHA_CHANNEL(pbmSrc, dwSrcPixel, pbmDst, x);

            // next source pixel
            pbyBase+=3;
         } 
      } else {
         for (x = 0; x < dwWidth; x++) {
             // convert source pixel to dest pixel format
            dwSrcPixel = *(pbyBase)            // blue: bits 0-7
                      + (*(pbyBase+1) << 8)    // green:bits 8-15
                      + (*(pbyBase+2)  << 16); // red:  bits 16-23

            ((uint32 *)(pbmDst->pBmp))[x] = dwSrcPixel;

            // next source pixel
            pbyBase+=3;
         }
      }
   } else if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_555) {
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = (((*((uint16 *)(pbyBase)) & 0x001F) * (0x21 * 0x4000) & 0x00FF0000) >> 16);
            ((uint32 *)(pbmDst->pBmp))[x] += (((*((uint16 *)(pbyBase)) & 0x03E0) * (0x21 * 0x0200) & 0x00FF0000) >> 8); 
            ((uint32 *)(pbmDst->pBmp))[x] += ((*((uint16 *)(pbyBase)) & 0x7C00) * (0x21 * 0x0010) & 0x00FF0000);         
         
            // build the lpha channel ..
            BUILD_ALPHA_CHANNEL(pbmSrc, *((uint16 *)(pbyBase)), pbmDst, x);
         
            // next source pixel
            pbyBase+=sizeof(uint16);
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = (((*((uint16 *)(pbyBase)) & 0x001F) * (0x21 * 0x4000) & 0x00FF0000) >> 16);
            ((uint32 *)(pbmDst->pBmp))[x] += (((*((uint16 *)(pbyBase)) & 0x03E0) * (0x21 * 0x0200) & 0x00FF0000) >> 8); 
            ((uint32 *)(pbmDst->pBmp))[x] += ((*((uint16 *)(pbyBase)) & 0x7C00) * (0x21 * 0x0010) & 0x00FF0000);         
         
            // next source pixel
            pbyBase+=sizeof(uint16);
         }
      }
   } else if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_565) {
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = (((*((uint16 *)(pbyBase)) & 0x001F) * (0x21 * 0x4000) & 0x00FF0000) >> 16);
            ((uint32 *)(pbmDst->pBmp))[x] += (((*((uint16 *)(pbyBase)) & 0x07E0) * (0x41 * 0x0080) & 0x00FF0000) >> 8);  
            ((uint32 *)(pbmDst->pBmp))[x] += ((*((uint16 *)(pbyBase)) & 0xF800) * (0x21 * 0x0008) & 0x00FF0000);   
         
            // build the lpha channel  ..
            BUILD_ALPHA_CHANNEL(pbmSrc, *((uint16 *)(pbyBase)), pbmDst, x);
         
            // next source pixel
            pbyBase+=sizeof(uint16);
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = (((*((uint16 *)(pbyBase)) & 0x001F) * (0x21 * 0x4000) & 0x00FF0000) >> 16);
            ((uint32 *)(pbmDst->pBmp))[x] += (((*((uint16 *)(pbyBase)) & 0x07E0) * (0x41 * 0x0080) & 0x00FF0000) >> 8);  
            ((uint32 *)(pbmDst->pBmp))[x] += ((*((uint16 *)(pbyBase)) & 0xF800) * (0x21 * 0x0008) & 0x00FF0000);   
         
            // next source pixel
            pbyBase+=sizeof(uint16);
         }
      }
   } else if ((pbmSrc->nDepth == 8) && (pbmSrc->cntRGB)) {
      // look up each byte in the color table
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 

            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase];
         
            // build the lpha channel  ..
            BUILD_ALPHA_CHANNEL(pbmSrc, *pbyBase, pbmDst, x);

            // next source pixel
            pbyBase++;
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 

            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase];

            // next source pixel
            pbyBase++;
         }
      }
   } else if (pbmSrc->nDepth == 4) {
      // look up each byte in the color table
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0xF0)>>4];
            BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)((*pbyBase&0xF0)>>4), pbmDst, x);

            if (++x < dwWidth) {
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase&0x0F];
               BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)(*pbyBase&0x0F), pbmDst, x);
            }
         
            // next pair of source pixels ...
            pbyBase++;
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0xF0)>>4];
            if (++x < dwWidth)
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase&0x0F]; 
         
            // next pair of source pixels ...
            pbyBase++;
         }
      }
    } else if (pbmSrc->nDepth == 2) {
      // look up each byte in the color table
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0xC0)>>6];
            BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)((*pbyBase&0xC0)>>6), pbmDst, x);
            if (++x < dwWidth) {
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0x30)>>4];
               BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)((*pbyBase&0x30)>>4), pbmDst, x);
            }
            if (++x < dwWidth) {
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0x0C)>>2];
               BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)((*pbyBase&0x0C)>>2), pbmDst, x);
            }
            if (++x < dwWidth) {
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase&0x03];
               BUILD_ALPHA_CHANNEL(pbmSrc, (uint32)((*pbyBase&0x0C)>>2), pbmDst, x);
            }

            // next four source pixels ...
            pbyBase++;
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 
            // convert source pixel to dest pixel format
            ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0xC0)>>6];
            if (++x < dwWidth)
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0x30)>>4];
            if (++x < dwWidth)
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[(*pbyBase&0x0C)>>2];
            if (++x < dwWidth) 
               ((uint32 *)(pbmDst->pBmp))[x] = pbmSrc->pRGB[*pbyBase&0x03];
         
            // next four source pixels ...
            pbyBase++;
         }
      }
   } else if (pbmSrc->nDepth == 1) {
      // Simple.  bit == 1 means white, bit == 0 means black
      if (bAlpha) {
         for (x = 0; x < dwWidth; x++) { 

            // convert source pixel to dest pixel format
            dwSrcPixel = ((*(pbmSrc->pBmp + pbmSrc->nPitch*ySrc + (x) / 8) >> (7 - ((x) % 8)))) & 1;
            ((uint32 *)(pbmDst->pBmp))[x] = dwSrcPixel & 1 ? 0x00FFFFFF : 0;

            // build the alpha channel ...
            BUILD_ALPHA_CHANNEL(pbmSrc, dwSrcPixel, pbmDst, x)
         }
      } else {
         for (x = 0; x < dwWidth; x++) { 

            // convert source pixel to dest pixel format
            dwSrcPixel = ((*(pbmSrc->pBmp + pbmSrc->nPitch*ySrc + (x) / 8) >> (7 - ((x) % 8)))) & 1;
            ((uint32 *)(pbmDst->pBmp))[x] = dwSrcPixel & 1 ? 0x00FFFFFF : 0;
         }
      }
   } else {
      // unsupported DIB format
      dwResult = EUNSUPPORTED;
   }                                                      

   // rightmost edge initialization
   ((uint32 *)(pbmDst->pBmp))[dwWidth] = ((uint32 *)(pbmDst->pBmp))[dwWidth-1];

   return dwResult;
}




