/*======================================================
FILE:       ImageWidget.c

SERVICES:   Widget that draws images and/or backgrounds

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdLib.h"
#include "WidgetBase.h"
#include "ImageWidget.h"
#include "ModelBase.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"




//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------


#define ME_FROM_DECORATOR    ImageWidget *me = ((ImageWidget*)po)
#define WBASE(p)          ((WidgetBase *)p)


#define OFFSETRECT(prc, nx, ny)  (prc)->x+=(nx),(prc)->y+=(ny)


static __inline IImage *ImageWidget_GetImage(ImageWidget *me)
{
   IImage *pii = NULL;
   IModel *piModel = WBASE(me)->piModel;

   if (piModel) {
      IINTERFACEMODEL_GetIPtr((IInterfaceModel*)piModel, AEEIID_IMAGE, (void **)&pii);
   }
   return pii;
}

static void ImageWidget_AnimateStep(ImageWidget *me)
{
   WidgetBase_Invalidate((IWidget*)me, ICIF_REDRAW);
   
   if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
      if (me->nCurStep == (uint32)(me->nFrames-1)) {
         IMODEL_StepNotify(WBASE(me)->piViewModel, EVT_STEP_FINAL|EVT_STEP_REPEAT, me->nCurStep, me->nCurRep++);
         me->nCurStep = 0;
      }
      else {
         IMODEL_StepNotify(WBASE(me)->piViewModel, 0, me->nCurStep++, me->nCurRep);
      }
   }   
}

static void ImageWidget_Animate(ImageWidget *me, boolean bAnim)
{
   if (bAnim) {
      // Already animating?
      if (me->piAnimImage) 
         return;

      // Cache model image since we can't store any other state to
      // know when the image or model has changed
      me->piAnimImage = ImageWidget_GetImage(me);
   } else if (me->piAnimImage) {
      // stop drawing
      IIMAGE_Stop(me->piAnimImage);
      // Unset the redraw callback
      IIMAGE_SetParm(me->piAnimImage, IPARM_REDRAW, 0, 0);
      // Release cache
      RELEASEIF(me->piAnimImage);
      me->nCurStep = me->nCurRep = 0;
   }

   // Based on above, if cache is set we can start animation
   if (me->piAnimImage) {
      me->nCurStep = me->nCurRep = 0;
      // Set the redraw callback
      IIMAGE_SetParm(me->piAnimImage, IPARM_REDRAW, (int)ImageWidget_AnimateStep, (int)me);
      
      if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
         IMODEL_StepNotify(WBASE(me)->piViewModel, EVT_STEP_PRE, (uint16)me->nCurStep++, (uint16)me->nCurRep);
      }
      
      // and start animation
      IIMAGE_Start(me->piAnimImage, 0, 0);
   } 
}


static void ImageWidget_ModelChanged(ImageWidget *me, ModelEvent *pEvent)
{
   IImage *piImage = ImageWidget_GetImage(me);
   AEEImageInfo aii;

   if (piImage) {
       IIMAGE_GetInfo(piImage, &aii);
      


      if (WBASE(me)->extent.width == 0) {
         WBASE(me)->extent.width = aii.cxFrame;
         if (me->dwFlags & IWF_HASSELECTIMAGE) {
            WBASE(me)->extent.width /= 2;
         }
         // Extent change
         WidgetBase_InvalidateExtent(IMAGEWIDGET_TO_IWIDGET(me));
      }
      if (WBASE(me)->extent.height == 0) {
         WBASE(me)->extent.height = aii.cy;
         // Extent change
         WidgetBase_InvalidateExtent(IMAGEWIDGET_TO_IWIDGET(me));
      }

      // store original image size in case it gets scaled later
      me->odx = me->sdx = aii.cxFrame;
      me->ody = me->sdy = aii.cy;
      me->bScaled = FALSE;
   }

   // Turn of animation
   ImageWidget_Animate(me, FALSE);

   // Reset current frame
   me->nCurFrame = 0;

   // And then a content change
   WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));

   // If animate flag is set, and new image can be animated, restart animation
   // cxFrame should be defined, but double check, then calculate nFrames based on the value
   if (me->bAnimated && piImage && aii.cxFrame) {
      me->nFrames = aii.cx/aii.cxFrame;
      // now start animation
      ImageWidget_Animate(me, TRUE);
   }

   RELEASEIF(piImage);
}

void ImageWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut)
{
   ME_FROM_DECORATOR;
   
   IImage *piImage = ImageWidget_GetImage(me);

   Decorator_GetPreferredExtent(po, pweOut);

   if (piImage) {
      AEEImageInfo aii;
      IIMAGE_GetInfo(piImage, &aii);

      // override image info's notion of image size because it's
      // broken in some earlier versions of BREW.  Use the values
      // we know to be correct from the last call to set the scale
      if (me->bScaled) {
         aii.cxFrame = me->sdx;
         aii.cy = me->sdy;
      }

      pweOut->width  = MAX(me->dx?me->dx:aii.cxFrame, pweOut->width);
      pweOut->height = MAX(me->dy?me->dy:aii.cy, pweOut->height);

      // If the bitmap has 2 images (selected, non selected) we need to divide the bitmap in half
      if (me->dwFlags & IWF_HASSELECTIMAGE) {
         pweOut->width /= 2;
      }
   } 

   Border_CalcPreferredExtent(&me->border, pweOut, pweOut);
   RELEASEIF(piImage);
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
      case PROP_FLAGS:
         me->dwFlags = dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;

      case PROP_SELECTED:
         if ((me->dwFlags & IWF_HASSELECTIMAGE) && (bSaveSel != me->border.bSelected)) {
            WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         }
         return TRUE;

      case PROP_IMAGE_PARM:
         {
            IImage *piImage = ImageWidget_GetImage(me);
            ImageParm *pParm = (ImageParm *)dwParam;

            if (pParm->parm == IPARM_OFFSET) {
               me->x = pParm->arg1;
               me->y = pParm->arg2;
            }

            if (pParm->parm == IPARM_SIZE) {
               me->dx = pParm->arg1;
               me->dy = pParm->arg2;
            } 

            if (pParm->parm == IPARM_NFRAMES) {
               
               // restart animations on frame count change.  This insures that
               // animation continues even if the previous animated image only
               // had a single frame.  (CR44466)
               if (me->nFrames != pParm->arg1) {
                  RELEASEIF(me->piAnimImage);
               }

               me->nFrames = pParm->arg1;
               me->nCurFrame = 0;
            }

            if (pParm->parm == IPARM_SCALE) {
               me->sdx = pParm->arg1;
               me->sdy = pParm->arg2;
               if ((me->sdx != me->odx) || (me->sdy != me->ody)) {
                  // remember that this image has been scaled from its original size.
                  // we need to keep this bit of information around because the
                  // number of image frames may change and the frame may be scaled
                  // the the original size again.  We'd, like, totally mistake the
                  // image as being at its original size if we didn't remember this.
                  me->bScaled = TRUE;
               } else {
                  // scaled back to original size.  Just like nothing ever happend.
                  me->bScaled = FALSE;
               }
            }

            if (piImage) { 
               IIMAGE_SetParm(piImage, pParm->parm, pParm->arg1, pParm->arg2);
               WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
            }
            RELEASEIF(piImage);
            return TRUE;
         }
      case PROP_IMAGE_ANIMATE:
         me->bAnimated = (boolean)dwParam;
         ImageWidget_Animate(me, (boolean)dwParam);
         return TRUE;
      case PROP_TILED_BDT:
         me->bdt = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDB:
         me->bdb = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDL:
         me->bdl = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_TILED_BDR:
         me->bdr = (uint16)dwParam;
         WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
         return TRUE;
      case PROP_IMAGE_FRAME:
         {
            IImage *piImage = ImageWidget_GetImage(me);
            AEEImageInfo ii;
            if (piImage) {
               IIMAGE_GetInfo(piImage, &ii);
               RELEASEIF(piImage);
               
               // get total number of frames if not yet specified   
               if (me->nFrames == 0) {
                  if (ii.cxFrame) {
                     me->nFrames = ii.cx/ii.cxFrame;
                  } else {
                     me->nFrames = 1;
                  }
               }
               
               if ((int)dwParam < me->nFrames) {
                  me->nCurFrame = (int)dwParam + 1;  // interal current frame is 1 based
                  ImageWidget_Animate(me, FALSE);
                  WidgetBase_InvalidateContent(IMAGEWIDGET_TO_IWIDGET(me));
                  return TRUE;
               }
            }
            break;
         }
      case PROP_ANIMATE_FLAGS:
         me->dwAnimFlags = dwParam;
         return TRUE;
      }
      
   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case PROP_FLAGS:
         *(uint32 *)dwParam = me->dwFlags;
         return TRUE;
      case PROP_IMAGE_ANIMATE:
         //*(uint32 *)dwParam = (me->piAnimImage != NULL);
         *(uint32 *)dwParam = me->bAnimated;
         return TRUE;
      case PROP_TILED_BDT:
         *(uint16 *)dwParam = me->bdt;
         return TRUE;
      case PROP_TILED_BDB:
         *(uint16 *)dwParam = me->bdb;
         return TRUE;
      case PROP_TILED_BDL:
         *(uint16 *)dwParam = me->bdl;
         return TRUE;
      case PROP_TILED_BDR:
         *(uint16 *)dwParam = me->bdr;
         return TRUE;
      case PROP_IMAGE_FRAME:
         *(int *)dwParam = me->nCurFrame - 1;   // internal frame count is 1-based
         return TRUE;
      case PROP_IMAGE_NFRAMES:
         {
            IImage *piImage = ImageWidget_GetImage(me);
            AEEImageInfo ii;

            // default frame count
            *(int *)dwParam  = 1;

            // is an image loaded in the widget?
            if (piImage) {
               IIMAGE_GetInfo(piImage, &ii);
               RELEASEIF(piImage);

               if (ii.cxFrame) {
                  // Return the number of frames the image has.
                  *(int *)dwParam  = ii.cx/ii.cxFrame;
               }
            } else if (me->nFrames) {
               // no image is loaded.  Return the last frame count
               *(int *)dwParam  = me->nFrames;
            }

            return TRUE;
         }
      case PROP_ANIMATE_FLAGS:
         *(uint32*)dwParam = me->dwAnimFlags;
         return TRUE; 
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
   IImage *piImage = ImageWidget_GetImage(me);
   AEEImageInfo aii;

   if (piImage) {
       IIMAGE_GetInfo(piImage, &aii);
   }

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_INTERFACEMODEL, (void **)&piim)) {
      RELEASEIF(piImage);
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(IDECORATOR_TO_IWIDGET(po), (IModel*)piim);

   // Re-register listener
   IMODEL_AddListenerEx(WBASE(me)->piModel, &me->modelListener, (PFNLISTENER)ImageWidget_ModelChanged, me);

   // Turn of animation
   ImageWidget_Animate(me, FALSE);

   // Reset the current frame as well.
   me->nCurFrame = 0;

   // And invalidate content
   WidgetBase_InvalidateContent(IDECORATOR_TO_IWIDGET(po));

   // If animate flag is set, and new image can be animated, restart animation
   // cxFrame should be defined, but double check, then calculate nFrames based on the value
   if (me->bAnimated && piImage && aii.cxFrame) {
      me->nFrames = aii.cx/aii.cxFrame;
      // now start animation
      ImageWidget_Animate(me, TRUE);
   }

   // Clean up
   RELEASEIF(piim);
   RELEASEIF(piImage);

   return SUCCESS;
}


static void ImageWidget_DrawTiled(ImageWidget *me, IImage *pii, IDisplay *piDisplay, int x, int y, AEERect *prcImage, AEERect *prcClip)
{
   AEERect tempRegion, tempClip;
   int rcdx = me->border.rcClient.dx;
   int rcdy = me->border.rcClient.dy;
      
   // center tiled
   SETAEERECT(&tempRegion, prcImage->x + me->bdl, prcImage->y + me->bdt, prcImage->dx - (me->bdl+me->bdr), prcImage->dy - (me->bdt+me->bdb));   
   SETAEERECT(&tempClip, x + me->bdl, y + me->bdt, rcdx  - (me->bdl + me->bdr), rcdy - (me->bdt + me->bdb));  
   IImage_DrawTiled(pii, piDisplay, x + me->bdl, y + me->bdt, &tempRegion, &tempClip);   
   
   // simple tiling is on, save some work.
   if(me->bdt == 0 && me->bdb == 0 && me->bdl == 0 && me->bdr == 0)
      return;
   
   // upper left corner
   SETAEERECT(&tempRegion, prcImage->x, prcImage->y, me->bdl, me->bdt);
   IImage_DrawImage(pii, piDisplay, x, y, &tempRegion);
   
   // left border   
   SETAEERECT(&tempRegion, prcImage->x, prcImage->y + me->bdt, me->bdl, prcImage->dy - (me->bdt+me->bdb));       
   SETAEERECT(&tempClip, x, y + me->bdt, me->bdl, rcdy - (me->bdt+me->bdb));   
   IImage_DrawTiled(pii, piDisplay, x, y + me->bdt, &tempRegion, &tempClip);        
      
   // top border   
   SETAEERECT(&tempRegion, prcImage->x + me->bdl, prcImage->y, prcImage->dx - (me->bdl+me->bdr), me->bdt);       
   SETAEERECT(&tempClip, x + me->bdl, y, rcdx - (me->bdl+me->bdr), me->bdt);   
   IImage_DrawTiled(pii, piDisplay, x + me->bdl, y, &tempRegion, &tempClip);        
   
   // right border  
   SETAEERECT(&tempRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + me->bdt, me->bdr, prcImage->dy - (me->bdt+me->bdb));        
   SETAEERECT(&tempClip, x + (rcdx-me->bdr), y+me->bdt, me->bdr, rcdy - (me->bdt+me->bdb));   
   IImage_DrawTiled(pii, piDisplay, x + rcdx-me->bdr, y + me->bdt, &tempRegion, &tempClip);     
    
   // bottom border  
   SETAEERECT(&tempRegion, prcImage->x + me->bdl, prcImage->y + (prcImage->dy-me->bdb), prcImage->dx-(me->bdl+me->bdr), me->bdb);       
   SETAEERECT(&tempClip, x + me->bdl, y + (rcdy - me->bdb), rcdx - (me->bdl+me->bdr), me->bdb);   
   IImage_DrawTiled(pii, piDisplay, x + me->bdl, y + (rcdy - me->bdb), &tempRegion, &tempClip);        
   
   // upper right corner
   SETAEERECT(&tempRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y, me->bdr, me->bdt);
   IImage_DrawImage(pii, piDisplay, x + (rcdx-me->bdr), y, &tempRegion);
   
   // lower right corner
   SETAEERECT(&tempRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + (prcImage->dy - me->bdb), me->bdr, me->bdb);
   IImage_DrawImage(pii, piDisplay, x + (rcdx-me->bdr), y + (rcdy-me->bdb), &tempRegion);

   // lower left corner
   SETAEERECT(&tempRegion, prcImage->x, prcImage->y + (prcImage->dy-me->bdb), me->bdl, me->bdb);
   IImage_DrawImage(pii, piDisplay, x, y + (rcdy-me->bdb), &tempRegion);   
}


void ImageWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;
   IImage *piImage = ImageWidget_GetImage(me);

   Border_Draw(&me->border, piCanvas, x, y);
   if (piImage) {
      AEERect rcClip;   // The clip rectangle - anythig outside this will not be drawn
      AEERect rcSel;    // The size of the image selection to be drawn
      AEEImageInfo aii; // image info struct
      int ix;           // x offset for image to be drawn
      int iy;           // y offset for image to be drawn    
      int width;
      int height;
      IDisplay *piDisplay;
      AEERect rcClipOld;   // original clip rect

      // draw image with its origin at the inside corner of
      // the border and padding
      ix = x + me->border.rcClient.x;
      iy = y + me->border.rcClient.y;

      // constrain drawing to the area of the screen within the 
      // widget's border or to the current clip rect, whichever
      // is smaller.
      SETAEERECT(&rcClip, ix, iy, me->border.rcClient.dx, me->border.rcClient.dy);
      ICANVAS_GetClipRect(piCanvas, &rcClipOld);
      IntersectRect(&rcClip, &rcClip, &rcClipOld);
      ICANVAS_SetClipRect(piCanvas, &rcClip);

      // get image size
      IIMAGE_GetInfo(piImage, &aii);    

      // override image info's notion of image size because it's
      // broken in some earlier versions of BREW.  Use the values
      // we know to be correct from the last call to set the scale
      if (me->bScaled) {
         aii.cxFrame = me->sdx;
         aii.cy = me->sdy;
      }

      // Adjust frame if necessary - bitmap has 2 images
      if (me->dwFlags & IWF_HASSELECTIMAGE) {
         aii.cxFrame /= 2;
      }

      width = me->dx?me->dx:aii.cxFrame;
      height = me->dy?me->dy:aii.cy;

      if (!(me->dwFlags &IWF_TILED)) {
         // Horizontal alignment
         if (me->dwFlags & IDF_ALIGN_RIGHT) {
            ix += me->border.rcClient.dx - width;
         } else if (me->dwFlags & IDF_ALIGN_CENTER) {
            ix += (me->border.rcClient.dx - width) / 2;
         }

         // Vertical alignment
         if (me->dwFlags & IDF_ALIGN_BOTTOM) {
            iy += me->border.rcClient.dy - height;
         } else if (me->dwFlags & IDF_ALIGN_MIDDLE) {
            iy += (me->border.rcClient.dy - height) / 2;
         }
      }

      if ((me->dwFlags & IWF_HASSELECTIMAGE) && me->border.bSelected) {
         SETAEERECT(&rcSel, aii.cxFrame + me->x, me->y, width, height);
      } else if ((me->nFrames > 1) && me->nCurFrame) {

         // select the right part of the image to draw.  Note,
         // how we do this depends on whether the image has
         // been scaled or not.
         if (me->bScaled) {
            rcSel.x = ((me->x + ((me->nCurFrame - 1) * aii.cxFrame)) * (me->odx/me->nFrames)) / me->sdx;
            rcSel.y = me->y;
            rcSel.dx = MIN(width, aii.cxFrame - me->x);
            rcSel.dy = height;
         } else {
            rcSel.x = me->x + ((me->nCurFrame - 1) * (aii.cx/me->nFrames));
            rcSel.y = me->y;
            rcSel.dx = MIN(width, (aii.cx/me->nFrames) - me->x);
            rcSel.dy = height;
         }

      } else {
         SETAEERECT(&rcSel, me->x, me->y, width, height);
      }

      if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
         if (me->dwFlags & IWF_TILED) { 
            ImageWidget_DrawTiled(me, piImage, piDisplay, ix, iy, &rcSel, &rcClip);
         } else if (me->bScaled) {
            // draw the scaled image and rely on the display clip rect 
            // to cause only the right frame/portion to be drawn.
#ifndef IPARM_DISPLAY
#define IPARM_DISPLAY   8
#endif
            IIMAGE_SetParm(piImage, IPARM_DISPLAY, (int)piDisplay, 0);
            IIMAGE_SetOffset(piImage, rcSel.x, rcSel.y);
            IIMAGE_SetDrawSize(piImage, width, height);
            IIMAGE_DrawFrame(piImage, -1, ix, iy);
         } else {
            IImage_DrawClipped(piImage, piDisplay, ix, iy, &rcSel, &rcClip);
         }
         IDISPLAY_Release(piDisplay);
      }

      // restore the clip rect
      ICANVAS_SetClipRect(piCanvas, &rcClipOld);
   }
   
   Decorator_Draw(po, piCanvas, x, y);

   RELEASEIF(piImage);
}


void ImageWidget_Dtor(ImageWidget *me)
{
   ImageWidget_Animate(me, FALSE);
   LISTENER_Cancel(&me->modelListener);
   Decorator_Dtor(&me->base);
   Border_Dtor(&me->border);
}


uint32 ImageWidget_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if (me->base.base.nRefs == 1) {
      ImageWidget_Dtor(me);
   }
   return Decorator_Release(po);
}


void ImageWidget_Ctor(ImageWidget *me, AEEVTBL(IDecorator) *pvt, IModule *piModule, 
                      PFNHANDLER pfnDefHandler)
{
   // construct base
   Decorator_Ctor(&me->base, pvt, piModule, DEFHANDLER(ImageWidget_HandleEvent));
   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.base.extent, FALSE, &WBASE(me)->piViewModel);

   // construct derived members
   pvt->Release            = ImageWidget_Release;
   pvt->Draw               = ImageWidget_Draw;
   pvt->GetPreferredExtent = ImageWidget_GetPreferredExtent;
   pvt->GetExtent          = ImageWidget_GetExtent;
   pvt->SetExtent          = ImageWidget_SetExtent;
   pvt->GetModel           = ImageWidget_GetModel;
   pvt->SetModel           = ImageWidget_SetModel;

   IWIDGET_SetBorderWidth(IMAGEWIDGET_TO_IWIDGET(me), 0);
   IWIDGET_SetBGColor(IMAGEWIDGET_TO_IWIDGET(me), RGBA_NONE);
   IWIDGET_SetPadding(IMAGEWIDGET_TO_IWIDGET(me), 0);
//   IWIDGET_SetTransparency(IMAGEWIDGET_TO_IWIDGET(me), 0);

   me->bAnimated = FALSE; // animation turned off by default
   me->dwFlags = IDF_ALIGN_TOP | IDF_ALIGN_LEFT;
   me->bdt = me->bdb = me->bdl = me->bdr = 0;         //simple tiling by default
   me->nCurStep = me->nCurRep = 0;
   me->dwAnimFlags = 0;
}


int ImageWidget_Initialize(ImageWidget *me)
{
   int nErr;
   IModel *piModel = 0;

   nErr = InterfaceModel_New((IInterfaceModel**)&piModel, me->base.base.piModule);
   
   if (!nErr) {
      ImageWidget_SetModel((IDecorator*)me, piModel);
   }

   RELEASEIF(piModel);

   return nErr;
}


int ImageWidget_New(IDecorator **ppo, IModule *piModule)
{
   int nErr;
   ImageWidget *me = MALLOCREC_VTBL(ImageWidget, IDecorator);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   ImageWidget_Ctor(me, GETVTBL(me,IDecorator), piModule, 0);
   nErr = ImageWidget_Initialize(me);

   if (!nErr) {
      *ppo = (IDecorator*)me;
   } else {
      ImageWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}



