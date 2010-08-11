/*
  ========================================================================

  FILE:  ImageBase.c
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of ImageBase

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEDisplayCanvas.h"
#include "AEEContainer.h"
#include "AEEWProperties.h"
#include "ImageBase.h"
#include "wutil.h"

#define DEF_ANIMATION_RATE    150

#ifndef IPARM_DISPLAY
#define IPARM_DISPLAY   8
#endif

#ifndef IPARM_SCALE  
#define IPARM_SCALE     12
#endif

#ifndef IPARM_GETNFRAMES
#define IPARM_GETNFRAMES      0x0103b027
#endif


#ifndef IPARM_GETFRAMEDELAY
#define IPARM_GETFRAMEDELAY   0x0103b753
#endif


#ifndef IPARM_EXTFUNC
#define IPARM_EXTFUNC   13

// structure for IPARM_EXTFUNC
typedef struct {
   AEECLSID id;
   void    *pBuf;
   int      nBufSize;
} AEEImageExtFunc;

static __inline int IIMAGE_ExtFunc(IImage *me, AEECLSID id, void *pBuf, int nBufSize)
{
   AEEImageExtFunc parm;
   int nReturn = EUNSUPPORTED;

   parm.id = id;
   parm.pBuf = pBuf;
   parm.nBufSize = nBufSize;

   IIMAGE_SetParm(me, IPARM_EXTFUNC, (int)&parm, (int)&nReturn);
   return nReturn;
}

#endif   // IPARM_EXTFUNC

#ifndef AEECLSID_IIMAGE_GETORIGINALDIMENSIONS
#define AEECLSID_IIMAGE_GETORIGINALDIMENSIONS 0x0106d4e8

typedef struct {
   int cx;
   int cy;
} AEEImageDimensions;

/* IIMAGE_GetOriginalDimensions
*/
static __inline int IIMAGE_GetOriginalDimensions(IImage *me, AEEImageDimensions *pDimensions) {
   return IIMAGE_ExtFunc(me, AEECLSID_IIMAGE_GETORIGINALDIMENSIONS, pDimensions, sizeof(*pDimensions));
}

#endif   // AEECLSID_IIMAGE_GETORIGINALDIMENSIONS

#define SAFEDIV(dividend,divisor) \
   ((dividend) / (((divisor)==0) ? 1 : (divisor)))


static void ImageBase_AnimateStep(ImageBase *me);


/////////////////////////////////////////////////////////////////////////////
// Local helpers

/* IIMAGE_GetNFrames
*/
static __inline int IIMAGE_GetNFrames(IImage *me, int *pnFrames) {
   return IIMAGE_ExtFunc(me, IPARM_GETNFRAMES, pnFrames, sizeof(*pnFrames));
}


/* IIMAGE_GetFrameDelay
*/
static __inline int IIMAGE_GetFrameDelay(IImage *me, int nFrame, int *pnDelay) {
   int nFrameDelay = nFrame;
   int nErr = IIMAGE_ExtFunc(me, IPARM_GETFRAMEDELAY, &nFrameDelay, sizeof(nFrameDelay));
   if (!nErr) {
      *pnDelay = nFrameDelay;
   }
   return nErr;
}

/* ImageBase_Invalidate 
*/
static __inline void ImageBase_Invalidate(ImageBase *me, uint32 dwFlags)
{
   if (me->pfnInval) {
      me->pfnInval(me->pvInval, dwFlags);
   }
}


/////////////////////////////////////////////////////////////////////////////
// animation


static int ImageBase_GetFrameDelay(ImageBase *me, int nFrame)
{
   int nDelay = 0;
   if (me->bVarFrameDelay 
         && SUCCESS == IIMAGE_GetFrameDelay(me->pii, nFrame, &nDelay)) {
      return nDelay * 10;
   }
   // otherwise, return default rate
   return me->nRate;
}


/* ImageBase_ScheduleFrameTimer 
*/
static void ImageBase_ScheduleFrameTimer(ImageBase *me)
{
   int nDelay;
   if(((me->dwAnimFlags & AF_ANIMATE_ONCE) && (me->nCurStep == me->nFrames)) ||
         (1 == me->nFrames)) {
      return;
   }
   nDelay = ImageBase_GetFrameDelay(me, me->nCurFrame-1);
   CALLBACK_Timer(&me->cbk, (PFNNOTIFY)ImageBase_AnimateStep, me, me->piShell, nDelay);
}


/* ImageBase_AnimateStep
*/
static void ImageBase_AnimateStep(ImageBase *me)
{
   uint32 dwNotifyFlags = 0;

   ImageBase_Invalidate(me, ICIF_REDRAW);

   if (!me->bExternalAnim) {

      me->nCurFrame++;
   
      if (me->nCurFrame >= me->nFrames+1) {
         me->nCurFrame = 1;
      }
   
      if (me->nCurStep == me->nFrames-1) {
         dwNotifyFlags = EVT_STEP_FINAL;
         if (0 == (me->dwAnimFlags & AF_ANIMATE_ONCE)) {
            dwNotifyFlags |= EVT_STEP_REPEAT;
         }
      }
   }

   if (*me->ppiViewModel && (me->dwAnimFlags & AF_ENABLE_EVT_STEP)) {
      IMODEL_StepNotify(*me->ppiViewModel, dwNotifyFlags, me->nCurStep, me->nCurRep);
   }

   if (dwNotifyFlags & EVT_STEP_FINAL) {
      if (me->dwAnimFlags & AF_ANIMATE_ONCE) {
         me->nCurStep = me->nFrames; //done with the animation.
         return;
      }
      me->nCurStep = 0;
      me->nCurRep++;
   } else {
      me->nCurStep++;
   }
}


/* ImageBase_Animate 
*/
void ImageBase_Animate(ImageBase *me, boolean bAnim)
{
   uint32 dwNotifyFlags = 0;
   me->bAnimated = bAnim;

   if (!bAnim) {     // turn off

      if (me->piAnimImage) {     // if already animating...

         // if doing external animation, stop it
         if (me->bExternalAnim) {
            IIMAGE_Stop(me->piAnimImage);
            IIMAGE_SetParm(me->piAnimImage, IPARM_REDRAW, 0, 0);
         }

         // Release cache
         RELEASEIF(me->piAnimImage);
         me->nCurStep = me->nCurRep = 0;

         if (me->nCurFrame != me->nSaveCurFrame) {
            me->nCurFrame = me->nSaveCurFrame;
            ImageBase_Invalidate(me, ICIF_REDRAW);
         }
         CALLBACK_Cancel(&me->cbk);
      }
   } 


   // turn on, if image is present
   if (bAnim && me->pii) {   

      // we allow 'restarts' of currently animating images because
      // we should be able to change from AnimateOnceThenStop mode
      // to continuous mode just by changing the flag, then restarting
      // without an intervening stop...
      // so Release here if we're already animating:
      RELEASEIF(me->piAnimImage);

      // Cache model image since we can't store any other state to
      // know when the image or model has changed
      me->piAnimImage = me->pii;
      IIMAGE_AddRef(me->piAnimImage);

      me->nCurStep = me->nCurRep = 0;
      // always start at first frame
      me->nCurFrame = 1;      // we need to initialize it if not previously set

      ImageBase_Invalidate(me, ICIF_REDRAW);

      if (*me->ppiViewModel && (me->dwAnimFlags & AF_ENABLE_EVT_STEP)) {
         dwNotifyFlags = EVT_STEP_PRE;
         if (1 == me->nFrames)
         {
            dwNotifyFlags |= EVT_STEP_FINAL;
         }
         IMODEL_StepNotify(*me->ppiViewModel, dwNotifyFlags, 
               (uint32)me->nCurStep, (uint16)me->nCurRep);
      }

      me->nCurStep++;

      me->bExternalAnim = !me->bGetNFrames && me->bInfoAnimated && !me->bFramesSet;

      // if our IImage is a BCI and NFrames has not been set
      // by the application, they we let the IImage do the animation
      // directly
      if (me->bExternalAnim) {
         IIMAGE_SetParm(me->piAnimImage, IPARM_REDRAW, (int)ImageBase_AnimateStep, (int)me);
         IIMAGE_Start(me->piAnimImage, 0, 0);
      }

   }
}

/////////////////////////////////////////////////////////////////////////////

/* ImageBase_SetSelected
*/
void ImageBase_SetSelected(ImageBase *me, boolean bSelected)
{
   me->bSelected = bSelected;
   if (me->bHasSelectImage) {
      me->nCurFrame = bSelected ? 1 : 2;
      ImageBase_Invalidate(me, 0);
   }
}


/* ImageBase_SetParm
*/
void ImageBase_SetParm(ImageBase *me, ImageParm *pParm)
{
   if (pParm->parm == IPARM_RATE) {
      me->nRate = pParm->arg1;
      // override the frame delays contained in the image (GIF only)
      me->bVarFrameDelay = 0;
   
   } else if (pParm->parm == IPARM_OFFSET) {
      me->x = pParm->arg1;
      me->y = pParm->arg2;
   
   } else if (pParm->parm == IPARM_SIZE) {
      me->dx = pParm->arg1;
      me->dy = pParm->arg2;
   
   } else if (pParm->parm == IPARM_NFRAMES) {
   
      // restart animations on frame count change.  This insures that
      // animation continues even if the previous animated image only
      // had a single frame.  (CR44466)
      if (me->nFrames != pParm->arg1) {
         RELEASEIF(me->piAnimImage);
      }
   
      me->nFrames    = pParm->arg1;
      me->nCurFrame  = (me->nFrames > 0) ? 1 : 0;
      me->bFramesSet = 1;
   
   } else if (pParm->parm == IPARM_SCALE) {
   
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

   if (me->pii) { 
      IIMAGE_SetParm(me->pii, pParm->parm, pParm->arg1, pParm->arg2);
      ImageBase_Invalidate(me, 0);
   }
}


/* ImageBase_SetFrame
*/
boolean ImageBase_SetFrame(ImageBase *me, int nFrame)
{
   if (me->pii && nFrame < me->nFrames) {
      ImageBase_Animate(me, 0);
      me->nCurFrame = nFrame + 1;  // internal current frame is 1 based
      me->nSaveCurFrame = me->nCurFrame;
      ImageBase_Invalidate(me, 0);
      return TRUE;
   }
   return FALSE;
}


/* ImageBase_SetImage
*/
void ImageBase_SetImage(ImageBase *me, IImage *pii)
{
   // cleanup animation
   ImageBase_Animate(me, 0);

   me->bFramesSet = 0;
   me->bGetNFrames = 0;
   me->bVarFrameDelay = 0;
   me->bInfoAnimated = 0;
   me->bExternalAnim = 0;

   RELEASEIF(me->pii);
   me->pii = pii;
   ADDREFIF(me->pii);

   if (me->pii) {

      AEEImageInfo aii;
      AEEImageDimensions aid;

      IIMAGE_GetInfo(me->pii, &aii);
      me->bInfoAnimated = aii.bAnimated;

      // GIF images have an internal notion of frames, set by the 
      // image data.  The GIF viewer extension will report this with
      // IPARM_GETNFRAMES.  Other viewers will fail with this IPARM.
      if (SUCCESS == IIMAGE_GetNFrames(me->pii, &me->nFrames)) {

         // if we can get the frame delay for frame zero, then
         // variable frame delays are supported
         int nTemp;
         me->bVarFrameDelay = (SUCCESS == IIMAGE_GetFrameDelay(me->pii, 0, &nTemp));
         me->bGetNFrames = 1;

      } else {
         me->nFrames = SAFEDIV(aii.cx, aii.cxFrame);
      }

      if (me->nFrames < 1) {
         me->nFrames = 1;
      }

      // store original image size in case it gets scaled later
      if (IIMAGE_GetOriginalDimensions(me->pii, &aid) == SUCCESS) {

         me->odx = aid.cx;
         me->ody = aid.cy;     

         if (me->nFrames > 1) {
            me->sdx = aii.cxFrame;
            me->sdy = aii.cy;
            me->bScaled = FALSE;
         } else {
            // Update scale setting in case the image dimensions differ from the original
            me->sdx = aii.cxFrame;
            me->sdy = aii.cy;
            if ((me->sdx != me->odx) || (me->sdy != me->ody)) {
               // remember that this image has been scaled from its original size.
               // we need to keep this bit of information around because the
               // number of image frames may change and the frame may be scaled
               // the the original size again.  We'd, like, totally mistake the
               // image as being at its original size if we didn't remember this.
               me->bScaled = TRUE;
            } 
            else {
               // scaled back to original size.  Just like nothing ever happend.
               me->bScaled = FALSE;
            }
            IIMAGE_SetParm(me->pii, IPARM_SCALE, me->sdx, me->sdy);
         }
      } else {
         me->odx = me->sdx = aii.cxFrame;
         me->ody = me->sdy = aii.cy;
         me->bScaled = FALSE;
      }


      // Turn off animation
      ImageBase_Animate(me, FALSE);
   
      // Reset current frame
      me->nCurFrame = (me->nFrames > 0) ? 1 : 0;
   
      // If animate flag is set, and new image can be animated, restart animation
      // cxFrame should be defined, but double check, then calculate nFrames based on the value
      if (me->bAnimated && me->pii && aii.cxFrame) {
         // now start animation
         ImageBase_Animate(me, TRUE);
      }
   }

   // invalidate for a content change
   ImageBase_Invalidate(me, 0);
}

/* ImageBase_DrawTiled 
*/
static void ImageBase_DrawTiled(ImageBase *me, IDisplay *piDisplay, 
                                int x, int y, int dxClient, int dyClient,
                                AEERect *prcImage)
{
   AEERect rcRegion, rcClip;
   int rcdx = dxClient;
   int rcdy = dyClient;
      
   // center tiled
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y + me->bdt, 
              prcImage->dx - (me->bdl+me->bdr), prcImage->dy - (me->bdt+me->bdb));   

   SETAEERECT(&rcClip, x + me->bdl, y + me->bdt, 
              rcdx  - (me->bdl + me->bdr), rcdy - (me->bdt + me->bdb));  
   IImage_DrawTiled(me->pii, piDisplay, x + me->bdl, y + me->bdt, &rcRegion, &rcClip);   
   
   // simple tiling is on, save some work.
   if (me->bdt == 0 && me->bdb == 0 && me->bdl == 0 && me->bdr == 0) {
      return;
   }
   
   // upper left corner
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y, me->bdl, me->bdt);
   IImage_DrawImage(me->pii, piDisplay, x, y, &rcRegion);
   
   // left border   
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y + me->bdt, me->bdl, prcImage->dy - (me->bdt+me->bdb));       
   SETAEERECT(&rcClip, x, y + me->bdt, me->bdl, rcdy - (me->bdt+me->bdb));   
   IImage_DrawTiled(me->pii, piDisplay, x, y + me->bdt, &rcRegion, &rcClip);        
      
   // top border   
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y, prcImage->dx - (me->bdl+me->bdr), me->bdt);       
   SETAEERECT(&rcClip, x + me->bdl, y, rcdx - (me->bdl+me->bdr), me->bdt);   
   IImage_DrawTiled(me->pii, piDisplay, x + me->bdl, y, &rcRegion, &rcClip);        
   
   // right border  
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + me->bdt, me->bdr, prcImage->dy - (me->bdt+me->bdb));        
   SETAEERECT(&rcClip, x + (rcdx-me->bdr), y+me->bdt, me->bdr, rcdy - (me->bdt+me->bdb));   
   IImage_DrawTiled(me->pii, piDisplay, x + rcdx-me->bdr, y + me->bdt, &rcRegion, &rcClip);     
    
   // bottom border  
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y + (prcImage->dy-me->bdb), prcImage->dx-(me->bdl+me->bdr), me->bdb);       
   SETAEERECT(&rcClip, x + me->bdl, y + (rcdy - me->bdb), rcdx - (me->bdl+me->bdr), me->bdb);   
   IImage_DrawTiled(me->pii, piDisplay, x + me->bdl, y + (rcdy - me->bdb), &rcRegion, &rcClip);        
   
   // upper right corner
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y, me->bdr, me->bdt);
   IImage_DrawImage(me->pii, piDisplay, x + (rcdx-me->bdr), y, &rcRegion);
   
   // lower right corner
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + (prcImage->dy - me->bdb), me->bdr, me->bdb);
   IImage_DrawImage(me->pii, piDisplay, x + (rcdx-me->bdr), y + (rcdy-me->bdb), &rcRegion);

   // lower left corner
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y + (prcImage->dy-me->bdb), me->bdl, me->bdb);
   IImage_DrawImage(me->pii, piDisplay, x, y + (rcdy-me->bdb), &rcRegion);   
}


/* ImageBase_Draw
*/
void ImageBase_Draw(ImageBase *me, ICanvas *piCanvas, int x, int y, 
                    int dxClient, int dyClient)
{
   if (me->pii) {
      AEERect rcClip;   // The clip rectangle - anythig outside this will not be drawn
      AEERect rcSel;    // The size of the image selection to be drawn
      int width;
      int height;
      IDisplay *piDisplay;
      AEERect rcClipOld;   // original clip rect
      AEEImageInfo aii;


      // constrain drawing to the area of the screen within the 
      // widget's border or to the current clip rect, whichever
      // is smaller.
      SETAEERECT(&rcClip, x, y, dxClient, dyClient);
      ICANVAS_GetClipRect(piCanvas, &rcClipOld);
      if (!IntersectRect(&rcClip, &rcClip, &rcClipOld)) {
         return; // Clipping rectangle null - nothing to draw.
      }
      ICANVAS_SetClipRect(piCanvas, &rcClip);

      IIMAGE_GetInfo(me->pii, &aii);

      // override image info's notion of image size because it's
      // broken in some earlier versions of BREW.  Use the values
      // we know to be correct from the last call to set the scale
      if (me->bScaled) {
         aii.cxFrame = me->sdx;
         aii.cy = me->sdy;
      }

      // Adjust frame if necessary - bitmap has 2 images
      if (me->bHasSelectImage) {
         aii.cxFrame /= 2;
      }

      width  = me->dx ? me->dx : aii.cxFrame;
      height = me->dy ? me->dy : aii.cy;

      if (!me->bTiled) {
         // Horizontal alignment
         if (me->dwAlignment & IDF_ALIGN_RIGHT) {
            x += dxClient - width;
         } else if (me->dwAlignment & IDF_ALIGN_CENTER) {
            x += (dxClient - width) / 2;
         }

         // Vertical alignment
         if (me->dwAlignment & IDF_ALIGN_BOTTOM) {
            y += dyClient - height;
         } else if (me->dwAlignment & IDF_ALIGN_MIDDLE) {
            y += (dyClient - height) / 2;
         }
      }

      if (me->bHasSelectImage && me->bSelected) {
         SETAEERECT(&rcSel, aii.cxFrame + me->x, me->y, width, height);
      } else if (!me->bGetNFrames && (me->nFrames > 1) && me->nCurFrame) {
         // select the right part of the image to draw.  Note,
         // how we do this depends on whether the image has
         // been scaled or not.
         if (me->bScaled) {
            rcSel.x = SAFEDIV(((me->x + ((me->nCurFrame - 1) * aii.cxFrame)) * SAFEDIV(me->odx,me->nFrames)), me->sdx);
            rcSel.y = me->y;
            rcSel.dx = MIN(width, aii.cxFrame - me->x);
            rcSel.dy = height;
         } else {
            rcSel.x = me->x + ((me->nCurFrame - 1) * SAFEDIV(aii.cx,me->nFrames));
            rcSel.y = me->y;
            rcSel.dx = MIN(width, SAFEDIV(aii.cx,me->nFrames) - me->x);
            rcSel.dy = height;
         }

      } else {
         SETAEERECT(&rcSel, me->x, me->y, width, height);
      }

      if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {
         if (me->bTiled) { 
            ImageBase_DrawTiled(me, piDisplay, x, y, dxClient, dyClient, &rcSel);

         } else if (me->bScaled) {
            // draw the scaled image and rely on the display clip rect 
            // to cause only the right frame/portion to be drawn.
            IIMAGE_SetParm(me->pii, IPARM_DISPLAY, (int)piDisplay, 0);
            IIMAGE_SetOffset(me->pii, rcSel.x, rcSel.y);
            IIMAGE_SetDrawSize(me->pii, width, height);
            if(me->bInfoAnimated) {
               IIMAGE_DrawFrame(me->pii, me->nCurFrame-1, x, y);
            }
            else {
               IIMAGE_DrawFrame(me->pii, -1, x, y);
            }

         } else if ((!me->bAnimated && me->bExternalAnim && me->bInfoAnimated) || 
                   (!me->bExternalAnim && 
                   (me->bGetNFrames || (me->bInfoAnimated && me->piAnimImage)))) {
            // for GIF or BCI viewers, we must draw the frame with 
            // the actual frame number.
            // Same is the case for ImageStrips, where IIMAGE_SetFrameCount was
            // called before image is set on the widget.
            IIMAGE_SetParm(me->pii, IPARM_DISPLAY, (int)piDisplay, 0);
            IIMAGE_DrawFrame(me->pii, me->nCurFrame-1, x, y);

         } 
         else {
            IImage_DrawClipped(me->pii, piDisplay, x, y, &rcSel, &rcClip);
         }
         IDISPLAY_Release(piDisplay);
      }

      // restore the clip rect
      ICANVAS_SetClipRect(piCanvas, &rcClipOld);
      // set next timer callback after the current frame is drawn
      if (!me->bExternalAnim && me->bAnimated) {
         ImageBase_ScheduleFrameTimer(me);
      }

   }
   
}


/* ImageBase_Dtor
*/
void ImageBase_Dtor(ImageBase *me)
{
   ImageBase_Animate(me, 0);

   RELEASEIF(me->pii);
   RELEASEIF(me->piAnimImage);
   RELEASEIF(me->piShell);
}


/* ImageBase_Ctor
*/
void ImageBase_Ctor(ImageBase *me, IShell *piShell, PFNINVALIDATE pfnInval, 
                           void *pvInval, IModel **ppiViewModel)
{
   me->piShell = piShell;
   ADDREFIF(me->piShell);

   me->pfnInval = pfnInval;
   me->pvInval  = pvInval;

   me->nRate        = DEF_ANIMATION_RATE;
   me->dwAlignment  = IDF_ALIGN_TOP | IDF_ALIGN_LEFT;
   me->ppiViewModel = ppiViewModel;
}







