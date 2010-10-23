/*======================================================
FILE:       Border.c

SERVICES:   Widget border drawing/managment

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEWProperties.h"
#include "AEEImage.h"
#include "Border.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"


/////////////////////////////////////////////////////////////////////////////
#define COLOR_INDEX(pb)  (((pb)->bActive != 0) + (2 * ((pb)->bSelected != 0)))

static void BackImage_ReplaceImage(BackImage *me, IImage *piImage);
/////////////////////////////////////////////////////////////////////////////
// BackImage (background image object)


static void BackImage_CtorZ(BackImage *me, IImage *piImage, PFNINVALIDATE pfnInval, void *pvInval, IModel **piViewModel)
{
   AEEImageInfo aii;

   IIMAGE_GetInfo(piImage, &aii);
   me->odx = me->sdx = aii.cxFrame;
   me->ody = me->sdy = aii.cy;
   me->bScaled = FALSE;
   me->nCurStep = me->nCurRep = 0;
   
   me->pfnInval = pfnInval;
   me->pvInval  = pvInval;

   me->piViewModel = piViewModel;

   me->dwAnimFlags = 0;

   BackImage_ReplaceImage(me, piImage);   // this also invalidates
}


static void BackImage_Dtor(BackImage *me)
{
   RELEASEIF(me->piImage);
}


static int BackImage_New(IImage *pii, PFNINVALIDATE pfnInval, void *pvInval, BackImage **ppo, IModel **piViewModel)
{
   int nErr = ENOMEMORY;

   *ppo = MALLOCREC(BackImage);
   if (*ppo) {
      nErr = SUCCESS;
   }

   if (!nErr) {
      BackImage_CtorZ(*ppo, pii, pfnInval, pvInval, piViewModel);
   }

   return nErr;
}


static void BackImage_Delete(BackImage *me)
{
   BackImage_Dtor(me);
   FREE(me);
}


static __inline void BackImage_Invalidate(BackImage *me) {
   if (me->pfnInval) {
      me->pfnInval(me->pvInval, ICIF_REDRAW);
      
      // if the view model from WidgetBase or ContainerBase has been created...
      if (*me->piViewModel && (me->dwAnimFlags & AF_ENABLE_EVT_STEP)) {
         if (me->nCurStep == (uint32)(me->nFrames-1)) {
            IMODEL_StepNotify(*me->piViewModel, EVT_STEP_BG|EVT_STEP_FINAL|EVT_STEP_REPEAT, me->nCurStep, me->nCurRep++);
            me->nCurStep = 0;
         }
         else {
            IMODEL_StepNotify(*me->piViewModel, EVT_STEP_BG, me->nCurStep++, me->nCurRep);
         }         
      }
   }
}


static void BackImage_Animate(BackImage *me, boolean bOn)
{
   if (bOn) {

      // Already animating?
      if (me->bAnimating) 
         return;

      // save animation state
      me->bAnimating = me->piImage != NULL;

      // Based on above, if cache is set we can start animation
      if (me->bAnimating) {
         me->nCurStep = me->nCurRep = 0;

         // Set the redraw callback
         IIMAGE_SetParm(me->piImage, IPARM_REDRAW, (int)BackImage_Invalidate, (int)me);

         if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
            IMODEL_StepNotify(*me->piViewModel, EVT_STEP_BG|EVT_STEP_PRE, me->nCurStep++, me->nCurRep);
         }

         // and start animation
         IIMAGE_Start(me->piImage, 0, 0);
      } 

   } else if (me->bAnimating) {

      // stop drawing
      IIMAGE_Stop(me->piImage);

      // Unset the redraw callback
      IIMAGE_SetParm(me->piImage, IPARM_REDRAW, 0, 0);

      me->bAnimating = 0;
      me->nCurStep = me->nCurRep = 0;
   }

}


static void BackImage_ReplaceImage(BackImage *me, IImage *piImage)
{
   AEEImageInfo aii;

   BackImage_Animate(me, 0);

   RELEASEIF(me->piImage);
   me->piImage = piImage;
   ADDREFIF(me->piImage);

   // remember original size for scaling purposes later ...
   if (piImage) {
      IIMAGE_GetInfo(piImage, &aii);
   } else {
      aii.cxFrame = aii.cy = 0;
   }
   me->odx = me->sdx = aii.cxFrame;
   me->ody = me->sdy = aii.cy;
   me->bScaled = FALSE;
   //me->nFrames = aii.cx/aii.cxFrame;

   BackImage_Invalidate(me);
}


static void BackImage_DrawTiled(BackImage *me, IDisplay *piDisplay, int x, int y, 
                                AEERect *prcImage, uint16 dxClient, uint16 dyClient)
{
   AEERect rcRegion, rcClip;
      
   // center tiled
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y + me->bdt, prcImage->dx - (me->bdl+me->bdr), prcImage->dy - (me->bdt+me->bdb));   
   SETAEERECT(&rcClip, x + me->bdl, y + me->bdt, dxClient  - (me->bdl + me->bdr), dyClient - (me->bdt + me->bdb));  
   IImage_DrawTiled(me->piImage, piDisplay, x + me->bdl, y + me->bdt, &rcRegion, &rcClip);   
   
   // simple tiling is on, save some work.
   if (me->bdt == 0 && me->bdb == 0 && me->bdl == 0 && me->bdr == 0) {
      return;
   }
   
   // upper left corner
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y, me->bdl, me->bdt);
   IImage_DrawImage(me->piImage, piDisplay, x, y, &rcRegion);
   
   // left border   
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y + me->bdt, me->bdl, prcImage->dy - (me->bdt+me->bdb));       
   SETAEERECT(&rcClip, x, y + me->bdt, me->bdl, dyClient - (me->bdt+me->bdb));   
   IImage_DrawTiled(me->piImage, piDisplay, x, y + me->bdt, &rcRegion, &rcClip);        
      
   // top border   
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y, prcImage->dx - (me->bdl+me->bdr), me->bdt);       
   SETAEERECT(&rcClip, x + me->bdl, y, dxClient - (me->bdl+me->bdr), me->bdt);   
   IImage_DrawTiled(me->piImage, piDisplay, x + me->bdl, y, &rcRegion, &rcClip);        
   
   // right border  
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + me->bdt, me->bdr, prcImage->dy - (me->bdt+me->bdb));        
   SETAEERECT(&rcClip, x + (dxClient-me->bdr), y+me->bdt, me->bdr, dyClient - (me->bdt+me->bdb));   
   IImage_DrawTiled(me->piImage, piDisplay, x + dxClient-me->bdr, y + me->bdt, &rcRegion, &rcClip);     
    
   // bottom border  
   SETAEERECT(&rcRegion, prcImage->x + me->bdl, prcImage->y + (prcImage->dy-me->bdb), prcImage->dx-(me->bdl+me->bdr), me->bdb);       
   SETAEERECT(&rcClip, x + me->bdl, y + (dyClient - me->bdb), dxClient - (me->bdl+me->bdr), me->bdb);   
   IImage_DrawTiled(me->piImage, piDisplay, x + me->bdl, y + (dyClient - me->bdb), &rcRegion, &rcClip);        
   
   // upper right corner
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y, me->bdr, me->bdt);
   IImage_DrawImage(me->piImage, piDisplay, x + (dxClient-me->bdr), y, &rcRegion);
   
   // lower right corner
   SETAEERECT(&rcRegion, prcImage->x + (prcImage->dx-me->bdr), prcImage->y + (prcImage->dy - me->bdb), me->bdr, me->bdb);
   IImage_DrawImage(me->piImage, piDisplay, x + (dxClient-me->bdr), y + (dyClient-me->bdb), &rcRegion);

   // lower left corner
   SETAEERECT(&rcRegion, prcImage->x, prcImage->y + (prcImage->dy-me->bdb), me->bdl, me->bdb);
   IImage_DrawImage(me->piImage, piDisplay, x, y + (dyClient-me->bdb), &rcRegion);   
}




static void BackImage_Draw(BackImage *me, IDisplay *piDisplay, int x, int y, 
                           uint16 dxClient, uint16 dyClient, boolean bSelected)
{
   if (me->piImage) {
      AEERect rcClip;      // The clip rectangle - anything outside this will not be drawn
      AEERect rcSel;       // The size of the image selection to be drawn
      AEEImageInfo aii;    // image info struct
      int ix = x;          // x offset for image to be drawn (within clipping rect)
      int iy = y;          // y offset for image to be drawn    
      int width;
      int height;

      IIMAGE_GetInfo(me->piImage, &aii);      

      // override image info's notion of image size because it's
      // broken in some earlier versions of BREW.  Use the values
      // we know to be correct from the last call to set the scale
      if (me->bScaled) {
         aii.cxFrame = me->sdx;
         aii.cy = me->sdy;
      } 

      // Adjust frame if necessary - bitmap has 2 images
      if (me->dwFlags & BGIF_HASSELECTIMAGE) {
         aii.cxFrame /= 2;
      }

      width  = me->dx ? me->dx : aii.cxFrame;
      height = me->dy ? me->dy : aii.cy;

      if (!(me->dwFlags & BGIF_TILED)) {

         // Horizontal alignment
         if (me->dwFlags & IDF_ALIGN_RIGHT) {
            ix += dxClient - width;

         } else if (me->dwFlags & IDF_ALIGN_CENTER) {
            ix += (dxClient - width) / 2;
         }

         // Vertical alignment
         if (me->dwFlags & IDF_ALIGN_BOTTOM) {
            iy += dyClient - height;

         } else if (me->dwFlags & IDF_ALIGN_MIDDLE) {
            iy += (dyClient - height) / 2;
         }
      }

      // Important note:  If this clip rect is changed by some future code, 
      // be sure to set the display clip rect right before drawing. Scaled 
      // images are drawn to screen assuming the display cliprect  is set to 
      // exactly the portion of the screen that needs to be updated.
      IDISPLAY_GetClipRect(piDisplay, &rcClip);

      // IImage expects pre-scaled offsets, so adjust them 
      // here if the image was scaled up beyond our clip rect.
      if (me->odx && (me->odx < aii.cxFrame) && (width > (me->x + rcClip.dx))) {
         ix = (ix * me->odx)/aii.cxFrame;
      }
      if (me->ody && (me->ody < aii.cy) && (height > (me->y + rcClip.dy))) {
         iy = (iy * me->ody)/aii.cy;
      }

      if ((me->dwFlags & BGIF_HASSELECTIMAGE) && bSelected) {
         SETAEERECT(&rcSel, aii.cxFrame + me->x, me->y, width, height);

      } else if ((me->nFrames > 1) && me->nCurFrame) {

         // select the right part of the image to draw ...
         // Note,  how we do this depends on whether the 
         // image has been scaled or not.
         if (me->bScaled) {
            rcSel.x = ((me->x + ((me->nCurFrame - 1) * aii.cxFrame)) * (me->odx/me->nFrames)) / me->sdx;
            rcSel.y = me->y;
            rcSel.dx = MIN(width, (me->nCurFrame * aii.cxFrame) - me->x);
            rcSel.dy = height;
         } else {
            rcSel.x = me->x + ((me->nCurFrame - 1) * (aii.cx/me->nFrames));
            rcSel.y = me->y;
            rcSel.dx = MIN(width, (me->nCurFrame * (aii.cx/me->nFrames)) - me->x);
            rcSel.dy = height;
         }

      } else {
         SETAEERECT(&rcSel, me->x, me->y, width, height);
      }

      // For scaled images, tell our draw routine to draw the entire image, 
      // relying on the display clip rect to make sure that only the invalid
      // part of the image is drawn.  We can't rely on our draw routines
      // below since they use IPARM_OFFSET, which expects pre-scaled
      // offsets and is inadequate for drawing a portion of a scaled image.
      if (me->bScaled) {
         // tell our routine to draw the entire image.  The current
         // display clip rect will limit drawing to only the invalid 
         // part of the image.
         SETAEERECT(&rcClip, x, y, dxClient, dyClient);
      }

      if (me->dwFlags & BGIF_TILED) { 
         BackImage_DrawTiled(me, piDisplay, ix, iy, &rcSel, dxClient, dyClient);
      } else {
         IImage_DrawClipped(me->piImage, piDisplay, ix, iy, &rcSel, &rcClip);
      }
   }
}


static boolean BackImage_HandleEvent(BackImage *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   if (evt == EVT_WDG_SETPROPERTY) {

      switch (wParam) {
      
         case PROP_BGIMAGE_FLAGS:
            me->dwFlags = dwParam;
            BackImage_Invalidate(me);
            return TRUE;

         case PROP_BGIMAGE_PARM: {
            ImageParm *pParm = (ImageParm*)dwParam;

            if (pParm->parm == IPARM_OFFSET) {
               me->x = pParm->arg1;
               me->y = pParm->arg2;

            } else if (pParm->parm == IPARM_SIZE) {
               me->dx = pParm->arg1;
               me->dy = pParm->arg2;

            } else if (pParm->parm == IPARM_NFRAMES) {
               me->nFrames = pParm->arg1;
               me->nCurFrame = 0;
            
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

            if (me->piImage) {
               IIMAGE_SetParm(me->piImage, pParm->parm, pParm->arg1, pParm->arg2);
               BackImage_Invalidate(me);
               return TRUE;
            }
         } break;


         case PROP_BGIMAGE_ANIMATE:
            if (me->piImage) {
               BackImage_Animate(me, (boolean)dwParam);
               return TRUE;
            }
            break;

         case PROP_BGIMAGE_TILED_BORDER:
            if (me->piImage) {
               BGImageTiledBorder *ptb = (BGImageTiledBorder*)dwParam;
               me->bdl = ptb->left;
               me->bdt = ptb->top;
               me->bdr = ptb->right;
               me->bdb = ptb->bottom;
               BackImage_Invalidate(me);
               return TRUE;
            }
            break;

         case PROP_BGIMAGE_FRAME:
            if (me->piImage) {
               AEEImageInfo ii;
               IIMAGE_GetInfo(me->piImage, &ii);
   
               // get total number of frames if not yet specified   
               if (me->nFrames == 0) {
                  if (ii.cxFrame) {
                     me->nFrames = ii.cx/ii.cxFrame;
                  } else {
                     me->nFrames = 1;
                  }
               }
   
               if ((int)dwParam < me->nFrames) {
                  me->nCurFrame = (int)dwParam + 1;  // internal current frame is 1 based
                  BackImage_Animate(me, 0);
                  BackImage_Invalidate(me);
                  return TRUE;
               }
            }
            break;
         case PROP_BGIMAGE_ANIMATE_FLAGS:
            me->dwAnimFlags = dwParam;
            return TRUE;
      } // switch

   } else if (evt == EVT_WDG_GETPROPERTY) {

      switch (wParam) {

         case PROP_BGIMAGE_FLAGS:
            *(uint32 *)dwParam = me->dwFlags;
            return TRUE;

         case PROP_BGIMAGE_TILED_BORDER:
            // return individual values in a struct
            ((BGImageTiledBorder*)dwParam)->left   = me->bdl;
            ((BGImageTiledBorder*)dwParam)->top    = me->bdt;
            ((BGImageTiledBorder*)dwParam)->right  = me->bdr;
            ((BGImageTiledBorder*)dwParam)->bottom = me->bdb;
            return TRUE;
         
         case PROP_BGIMAGE_ANIMATE_FLAGS:
            *(uint32*)dwParam = me->dwAnimFlags;
            return TRUE;

      } // switch

   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// DynRGB

static int MemRealloc(void **ppMem, int cbNewSize)
{
   void *pMem = *ppMem;
   pMem = REALLOC(pMem, cbNewSize);
   if (!pMem) {
      return ENOMEMORY;
   }
   *ppMem = pMem;
   return SUCCESS;
}

static int Widget_LoadPropsV(IResFile *po, uint32 **ppo, uint32 *psize, va_list* args)
{
   int nErr;
   
   nErr = IRESFILE_GetNamedV(po, RESTYPE_WPROPS, (void *)-1, psize, args);
   if (!nErr) {
      *ppo = (uint32 *)MALLOC(*psize);
      if (!*ppo)
         nErr = ENOMEMORY;         
      }   

   if (!nErr)
   {
      nErr = IRESFILE_GetNamedV(po, RESTYPE_WPROPS, *ppo, psize, args);
   }


   return nErr;
}

int Widget_ApplyPropsV(WResPropDesc* desc)
{
   int nErr;
   uint32 size;
   uint32 *props = 0;

  if (!desc || !desc->piResFile || !desc->args || !desc->piWidget)
      return EBADPARM;

   nErr = Widget_LoadPropsV(desc->piResFile, &props, &size, desc->args);

   if (!nErr) {
      uint32 *tmp;
      size = size / sizeof(uint32);

      IWIDGET_EnableLayout(desc->piWidget, FALSE);
      for (tmp = props, size = size / 2; size > 0; size--) {
         uint32 key = *tmp++;
         uint32 value = *tmp++;
         IWIDGET_SetProperty(desc->piWidget, (uint16)key, value);
      }
      IWIDGET_EnableLayout(desc->piWidget, TRUE);
      IWIDGET_ForceLayout(desc->piWidget);
   }
   
   FREEIF(props);   
   return nErr;
}

void DynRGB_Ctor(DynRGB *me, RGBVAL rgbDef)
{
   me->nMaxIndex     = 0;
   me->bDominantSet  = 0;
   me->u.rgb         = rgbDef;
}

void DynRGB_Dtor(DynRGB *me)
{
   if (me->nMaxIndex > 0) {
      FREE(me->u.prgb);
   }
}

void DynRGB_Collapse(DynRGB *me, RGBVAL rgb)
{
   // collapse is called when a dominant color that takes up all 4 slots is set.
   DynRGB_Dtor(me);
   me->nMaxIndex = 0;
   me->bDominantSet = 1;
   me->u.rgb = rgb;
}

static int DynRGB_Expand(DynRGB *me, int nMaxIndex)
{
   int nErr = SUCCESS;
   RGBVAL rgbFill;

   // Get the fill color, always the last set color.
   DynRGB_Get(me, RGBINDEX_SACTIVE, &rgbFill);

   // Zero out pointer for realloc, if necessary
   if (!me->nMaxIndex) me->u.prgb = 0;

   // Realloc and fill from start index to max index
   nErr = MemRealloc((void**)&me->u.prgb, (nMaxIndex+1) * sizeof(RGBVAL));
   if (!nErr) {
      int i;
      for (i = me->nMaxIndex; i <= nMaxIndex; i++) {
         me->u.prgb[i] = rgbFill;
      }
      me->nMaxIndex = nMaxIndex;
   }
   return nErr;
}

int DynRGB_Set(DynRGB *me, int nIndex, RGBVAL rgb)
{
   int nErr = SUCCESS;   

   if (nIndex > RGBINDEX_SACTIVE) {
      return EBADPARM;
   }

   if (me->bDominantSet && (me->nMaxIndex < RGBINDEX_SACTIVE)) {                  
      // If a dominant was set, allocate the full set, fill all values in
      nErr = DynRGB_Expand(me, RGBINDEX_SACTIVE);
   } 
   else if (!me->bDominantSet && (nIndex > me->nMaxIndex)) {
      // If a dominant wasn't set, allocate just enough for the new value, fill missing values in
      nErr = DynRGB_Expand(me, nIndex);
   }

   // Set the new value
   if (!nErr) {
      if (!me->nMaxIndex) {
         me->u.rgb = rgb;
      } else {
         me->u.prgb[nIndex] = rgb;
      }
   }

   return nErr;
}


// DynRGB_SetAlpha() sets the alpha value for all rgb's in array
static void DynRGB_SetAlpha(DynRGB *me, uint8 nAlpha)
{
   if (!me->nMaxIndex) {
      me->u.rgb &= 0xFFFFFF00;
      me->u.rgb |= nAlpha;
   } else {
      int i;
      for (i=0; i < me->nMaxIndex; i++) {
         me->u.prgb[i] &= 0xFFFFFF00;
         me->u.prgb[i] |= nAlpha;
      }
   }
}


boolean DynRGB_Get(DynRGB *me, int nIndex, RGBVAL *prgb)
{
   boolean bib = 1;  // in-bounds
   if (nIndex > me->nMaxIndex) {
      nIndex = me->nMaxIndex;
      bib = 0;
   }
   *prgb = !me->nMaxIndex ? me->u.rgb : me->u.prgb[nIndex];
   return bib;
}


// returns true if equal
boolean DynRGB_Compare(DynRGB *me, int n1, int n2)
{
   RGBVAL rgb_1, rgb_2;
   DynRGB_Get(me, n1, &rgb_1);
   DynRGB_Get(me, n2, &rgb_2);
   return (rgb_1 == rgb_2);
}


/////////////////////////////////////////////////////////////////////////////
// Shadow

static __inline void DynShadow_Dtor(DynShadow *me)
{
   if (me->nMaxIndex > 0) {
      FREE(me->u.pshadow);
   }
}

static boolean DynShadow_Get(DynShadow *me, int nIndex, Shadow *pshadow)
{
   boolean bib = 1; // in bounds
   if (nIndex > me->nMaxIndex) {
      nIndex = me->nMaxIndex;
      bib = 0;
   };
   *pshadow = !me->nMaxIndex ? me->u.shadow : me->u.pshadow[nIndex];
   return bib;
}

static int DynShadow_Set(DynShadow *me, int nIndex, Shadow *pshadow)
{
   int nErr = SUCCESS;

   if (nIndex > RGBINDEX_SACTIVE) {
      return EBADPARM;
   }
   
   // grow the array if we need more space
   if (nIndex > me->nMaxIndex) {

      Shadow shadow;

      if (!me->nMaxIndex) {
         shadow = me->u.shadow;
         me->u.pshadow = 0;   // make sure u.pshadow is 0 cuz we're gonna pass it to realloc
      } else {
         shadow = me->u.pshadow[me->nMaxIndex];
      }

      nErr = MemRealloc((void**)&me->u.pshadow, (nIndex+1) * sizeof(Shadow));
      if (!nErr) {
         int i;
         // when expanding the array, we don't want any uninitialized
         // values, so we propagate the last value to the newly
         // allocated elements
         for (i = me->nMaxIndex; i < nIndex; i++) {
            me->u.pshadow[i] = shadow;
         }
         me->nMaxIndex = nIndex;
      }
   }

   // set the new value
   if (!nErr) {
      if (!me->nMaxIndex) {
         me->u.shadow = *pshadow;
      } else {
         me->u.pshadow[nIndex] = *pshadow;
      }
   }

   return nErr;
}

void Border_GetShadowOffsets(Border *me, int nIndex, int *pnOffsetX, int *pnOffsetY) {
   Shadow shadow;
   
   if (nIndex == -1) 
      nIndex = COLOR_INDEX(me);
   DynShadow_Get(&me->dynShadow, nIndex, &shadow);
   *pnOffsetX = shadow.nOffsetX;
   *pnOffsetY = shadow.nOffsetY;
}

#define DSF_RGBA    0
#define DSF_OFFSETX 1
#define DSF_OFFSETY 2

static int DynShadow_SetVal(DynShadow *me, int nIndex, int field, int val)
{
   Shadow shadow;
   int result = SUCCESS;

   if (nIndex == -1) {
      for (nIndex = 0; result == SUCCESS && nIndex <= RGBINDEX_SACTIVE; nIndex++)
         result = DynShadow_SetVal(me, nIndex, field, val);
   } else {
      DynShadow_Get(me, nIndex, &shadow);
      switch(field) {
      case DSF_RGBA:
         shadow.rgba = (RGBAVAL)val;
         break;
      case DSF_OFFSETX:
         shadow.nOffsetX = val;
         break;
      case DSF_OFFSETY:
         shadow.nOffsetY = val;
         break;
      }
      result = DynShadow_Set(me, nIndex, &shadow);
   }
   return result;
}

static __inline int DynShadow_SetOffsetX(DynShadow *me, int nIndex, int nOffsetX)
{
   return DynShadow_SetVal(me, nIndex, DSF_OFFSETX, nOffsetX);
}

static __inline int DynShadow_SetOffsetY(DynShadow *me, int nIndex, int nOffsetY)
{
   return DynShadow_SetVal(me, nIndex, DSF_OFFSETY, nOffsetY);
}

static __inline int DynShadow_SetRGBA(DynShadow *me, int nIndex, RGBAVAL rgba)
{
   return DynShadow_SetVal(me, nIndex, DSF_RGBA, rgba);
}

/////////////////////////////////////////////////////////////////////////////
// Border

RGBVAL Border_GetCurrentBorderColor(Border *me) 
{
   RGBVAL rgb;
   DynRGB_Get(&me->dynBorder, COLOR_INDEX(me), &rgb);
   return rgb;
}

RGBVAL Border_GetCurrentBGColor(Border *me) 
{
   RGBVAL rgb;
   DynRGB_Get(&me->dynBG, COLOR_INDEX(me), &rgb);
   return rgb;
}

void Border_SetBorderColor(Border *me, int index, RGBVAL rgb)
{
   index = CONSTRAIN(index, 0, RGBINDEX_SACTIVE);
   DynRGB_Set(&me->dynBorder, index, rgb);
}

void Border_SetBGColor(Border *me, int index, RGBVAL rgb)
{
   index = CONSTRAIN(index, 0, RGBINDEX_SACTIVE);
   DynRGB_Set(&me->dynBG, index, rgb);
}

void Border_GetBorderColor(Border *me, int index, RGBVAL *prgb) 
{
   DynRGB_Get(&me->dynBorder, index, prgb);
}

void Border_GetBGColor(Border *me, int index, RGBVAL *prgb) 
{
   DynRGB_Get(&me->dynBG, index, prgb);
}


void Border_AdjustDisplayClipRect(Border *me, ICanvas *piCanvas, int x, int y)
{
   AEERect rcClip, rcClient;

   ICANVAS_GetClipRect(piCanvas, &rcClip);

   SETAEERECT(&rcClient, x + me->rcClient.x, y + me->rcClient.y, 
              me->rcClient.dx, me->rcClient.dy);

   IntersectRect(&rcClip, &rcClip, &rcClient);

   ICANVAS_SetClipRect(piCanvas, &rcClip);
}


void Border_Draw(Border *me, ICanvas *piCanvas, int x, int y)
{
   AEERect rc;
   IDisplay *piDisplay = 0;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      int ndxW = (me->bActive != 0);   // index into me->nWidth
      int ndxC = COLOR_INDEX(me);      // index into me->rgbBorder & me->rgbBackground
      RGBVAL rgbaBorder, rgbaBG, rgbaBGE;
      Shadow shadow;
      boolean bHasGradient = me->nGradientStyle != GRADIENT_STYLE_NONE;
      AEERect rcSaveClip, rcClip;
      
      DynShadow_Get(&me->dynShadow, ndxC, &shadow);
      DynRGB_Get(&me->dynBorder, ndxC, &rgbaBorder);
      DynRGB_Get(&me->dynBG, ndxC, &rgbaBG);
      if (bHasGradient) {
         DynRGB_Get(&me->dynBGE, ndxC, &rgbaBGE);
      }

      SETAEERECT(&rc, x, y, me->pExtent->width, me->pExtent->height);
   
      // Shadow struct is dynamically allocated
      if (shadow.nOffsetX|| shadow.nOffsetY) {
         
         AEERect rcShadow;
   
         // inset rc to allow for shadow dimensions
         if (shadow.nOffsetX < 0) {
            rc.x -= shadow.nOffsetX;
         }
         if (shadow.nOffsetY < 0) {
            rc.y -= shadow.nOffsetY;
         }
         rc.dx -= ABS(shadow.nOffsetX);
         rc.dy -= ABS(shadow.nOffsetY);

         if (RGBA_GETALPHA(shadow.rgba) > 0) {
            if (me->nStyle == BORDERSTYLE_NORMAL) {
               if (shadow.nOffsetY) {
                  rcShadow.x  = x + ((shadow.nOffsetX < 0) ? 0 : shadow.nOffsetX);
                  rcShadow.dx = rc.dx;
                  rcShadow.y  = y + ((shadow.nOffsetY < 0) ? 0 : rc.dy);
                  rcShadow.dy = ABS(shadow.nOffsetY);
                  BlendRect(piDisplay, &rcShadow, shadow.rgba, RGBA_GETALPHA(shadow.rgba));
               }
               if (shadow.nOffsetX) {
                  rcShadow.x  = x + ((shadow.nOffsetX < 0) ? 0 : rc.dx);
                  rcShadow.dx = ABS(shadow.nOffsetX);
                  rcShadow.y  = y + ABS(shadow.nOffsetY);
                  rcShadow.dy = rc.dy - ABS(shadow.nOffsetY);
                  BlendRect(piDisplay, &rcShadow, shadow.rgba, RGBA_GETALPHA(shadow.rgba));
               }
            } else {
               // shadow for rounded or beveled borders
               int nThick;

               if (me->nStyle == BORDERSTYLE_ROUNDED) {
                  nThick = MAX(0,MAX(me->nRadius,me->nWidth[ndxW]));
               } else {
                  nThick = me->nWidth[ndxW];
               }

               SETAEERECT(&rcShadow, rc.x + shadow.nOffsetX, rc.y + shadow.nOffsetY, rc.dx, rc.dy);
               IDisplay_DrawStyledRectShadow(piDisplay, 
                                             &rcShadow, 
                                             &rc, 
                                             nThick, 
                                             shadow.rgba, 
                                             (boolean)(me->nStyle == BORDERSTYLE_ROUNDED));
            }
         }
      }
   
      if (me->nStyle == BORDERSTYLE_ROUNDED) {
         DrawAndInsetRoundedBorderEx(piDisplay, &rc, 
                                     (int)me->nWidth[RGBINDEX_ACTIVE],
                                     (int)me->nWidth[ndxW],
                                     (int)me->nRadius, 
                                     rgbaBorder, (uint8)RGBA_GETALPHA(rgbaBorder));
      } else {
         DrawAndInsetBorderEx(piDisplay, &rc, 
                              (int)me->nWidth[RGBINDEX_ACTIVE], 
                              (int)me->nWidth[ndxW], 
                              rgbaBorder, (uint8)RGBA_GETALPHA(rgbaBorder), 
                              (boolean)(me->nStyle == BORDERSTYLE_BEVELED));
      }

      IDISPLAY_GetClipRect(piDisplay, &rcSaveClip);
      IntersectRect(&rcClip, &rc, &rcSaveClip);
      IDISPLAY_SetClipRect(piDisplay, &rcClip);

      if (me->pBackImage && me->pBackImage->piImage && 
          (me->pBackImage->dwFlags & BGIF_IMAGEUNDER)) {
         BackImage_Draw(me->pBackImage, piDisplay, rc.x, rc.y, 
                        me->rcClient.dx, me->rcClient.dy, me->bSelected);
      }

      // draw background (solid or gradient)
      if (bHasGradient && (rgbaBGE != rgbaBG)) {
         // gradient fill the background
         boolean bVert = (boolean)(me->nGradientStyle == GRADIENT_STYLE_VERT || 
                                   me->nGradientStyle == GRADIENT_STYLE_CENTERVERT);
         
         boolean bCenter = (boolean)(me->nGradientStyle == GRADIENT_STYLE_CENTERVERT || 
                                     me->nGradientStyle == GRADIENT_STYLE_CENTERHORZ);
         
         if (me->nStyle == BORDERSTYLE_ROUNDED) {
            // fill a rounded rect with the gradient
            IDisplay_FillRoundedRectGradient(piDisplay, &rc,
                                             MAX(0,(me->nRadius - me->nWidth[ndxW])), 
                                             rgbaBG, rgbaBGE,bVert, bCenter, 1);
         } else {
            // fill a simple rect with the gradient
            IDisplay_FillRectGradient(piDisplay, &rc, rgbaBG, rgbaBGE, bVert, bCenter, 1);
         }

      } else if (RGBA_GETALPHA(rgbaBG) > 0) {
         // draw background solid color (alpha blended)
         if (me->nStyle == BORDERSTYLE_ROUNDED) { 
            // fill a rounded rect with the solid background color
            IDisplay_FillRoundedRect(piDisplay, &rc, MAX(0,(me->nRadius - me->nWidth[ndxW])), rgbaBG);
         } else {
            // fill a simple rect with the solid background color
            BlendRect(piDisplay, &rc, rgbaBG, RGBA_GETALPHA(rgbaBG));
         }
      }

      if (me->pBackImage && me->pBackImage->piImage && 
          (0 == (me->pBackImage->dwFlags & BGIF_IMAGEUNDER))) {
         BackImage_Draw(me->pBackImage, piDisplay, rc.x, rc.y, 
                        me->rcClient.dx, me->rcClient.dy, me->bSelected);
      }

      IDISPLAY_SetClipRect(piDisplay, &rcSaveClip);
   }

   RELEASEIF(piDisplay);
}

static void Border_Invalidate(Border *me, uint32 dwFlags)
{
   if (me->pfnInval) {
      me->pfnInval(me->pvInval, dwFlags);
   }
}


void Border_CalcClientRect(Border *me)
{
   int nShadowOffsetX, nShadowOffsetY;
   WExtent *pe = me->pExtent;
   
   Border_GetCurrentShadowOffsets(me, &nShadowOffsetX, &nShadowOffsetY);
   
   // setup client rect here...
   SETAEERECT(&me->rcClient, 
              (int)me->nWidth[1] + me->padding.left + (nShadowOffsetX < 0 ? ABS(nShadowOffsetX) : 0),
              (int)me->nWidth[1] + me->padding.top + (nShadowOffsetY < 0 ? ABS(nShadowOffsetY) : 0),
              MAX(pe->width - (me->nWidth[1] * 2) - me->padding.left - me->padding.right - ABS(nShadowOffsetX), 0),
              MAX(pe->height - (me->nWidth[1] * 2) - me->padding.top - me->padding.bottom - ABS(nShadowOffsetY), 0));
}


void Border_CalcExtent(Border *me)
{
   Border_CalcClientRect(me);
   Border_Invalidate(me, ICIF_EXTENT);
}


/* returns the preferred extent for a required client 
   extent (the area inside border+padding) 
*/
void Border_CalcPreferredExtent(Border *me, WExtent *pweOut, WExtent *pweClientIn)
{
   int nShadowOffsetX, nShadowOffsetY;
   Border_GetCurrentShadowOffsets(me, &nShadowOffsetX, &nShadowOffsetY);
   
   pweOut->width = (me->nWidth[1] * 2) + pweClientIn->width + 
                     me->padding.left + me->padding.right + ABS(nShadowOffsetX);

   pweOut->height = (me->nWidth[1] * 2) + pweClientIn->height + 
                     me->padding.top + me->padding.bottom + ABS(nShadowOffsetY);
}



boolean Border_IntersectOpaque(Border *me, AEERect *prcOut, const AEERect *prcIn)
{
   AEERect rc;
   int ndxC = COLOR_INDEX(me);
   RGBVAL rgbaBG;
   
   DynRGB_Get(&me->dynBG, ndxC, &rgbaBG);

   // if opaque center.. 
   if (RGBA_GETALPHA(rgbaBG) == 255) {

      int nShadowOffsetX, nShadowOffsetY;
      int nIndent;

      SETAEERECT(&rc, 0, 0, me->pExtent->width, me->pExtent->height);

      nIndent = (int)me->nWidth[RGBINDEX_ACTIVE] - (int)me->nWidth[RGBINDEX_INACTIVE];
      if (!me->bActive && nIndent) {
         INFLATERECT(&rc, -nIndent, -nIndent);
      }

      Border_GetCurrentShadowOffsets(me, &nShadowOffsetX, &nShadowOffsetY);

      // adjust for the border
      rc.dx -= ABS(nShadowOffsetX);
      rc.dy -= ABS(nShadowOffsetY);

      return IntersectRect(prcOut, prcIn, &rc);
   } 

   prcOut->dx = prcOut->dy = 0;
   return FALSE;
}

boolean Border_HandleEvent(Border *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch(evt) {

   case EVT_WDG_SETLAYOUT: {
      if (wParam == 2) {
         Border_CalcExtent(me);
      } else {
         me->bUpdateStop = (wParam == 0) ? TRUE : FALSE;
      }
      return FALSE; // Container base needs the event as well
   }

   case EVT_WDG_SETFOCUS: {
      boolean bOldActive = me->bActive;
      me->bActive = (wParam != 0);

//      DBGPRINTF("me=%ld, me->bActive=%d", (unsigned long)me, (int)me->bActive);

      if (bOldActive != me->bActive && 
          (me->nWidth[0] != me->nWidth[1]
           || (!me->bSelected && DynRGB_Compare(&me->dynBorder, 0, 1))
           || (me->bSelected && DynRGB_Compare(&me->dynBorder, 2, 3))
           || (!me->bSelected && DynRGB_Compare(&me->dynBG, 0, 1))
           || (me->bSelected && DynRGB_Compare(&me->dynBG, 2, 3)) )) {
         Border_Invalidate(me, ICIF_REDRAW);
      }
      return FALSE;
   }
   
   case EVT_WDG_HASFOCUS:
      *((boolean*)dwParam) = (boolean) me->bActive;
      return TRUE;

   case EVT_WDG_CANTAKEFOCUS:
      *((boolean *)dwParam) = (boolean) me->bCanTakeFocus;
      return TRUE;

   case EVT_WDG_SETPROPERTY:
      
      switch(wParam) {

      case PROP_BGIMAGE:
         if (dwParam) {
            if (me->pBackImage) {
               BackImage_ReplaceImage(me->pBackImage, (IImage*)dwParam);
            } else {
               BackImage_New((IImage*)dwParam, me->pfnInval, me->pvInval, &me->pBackImage, me->piViewModel);
            }

         // else 0==dwParam, delete BackImage
         } else if (me->pBackImage) {  
            BackImage_Delete(me->pBackImage);
            me->pBackImage = 0;
            Border_Invalidate(me, ICIF_REDRAW);
         }
         return TRUE;

      // gradient style
      case PROP_GRADIENT_STYLE: 
         me->nGradientStyle = (uint8)dwParam;
         goto invalidateAndHandle;

      // gradient
      case PROP_GRADIENT:
         DynRGB_Collapse(&me->dynBGE, (RGBAVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SELECTED_GRADIENT:
         DynRGB_Set(&me->dynBGE, RGBINDEX_SACTIVE, (RGBAVAL)dwParam);
         DynRGB_Set(&me->dynBGE, RGBINDEX_SINACTIVE, (RGBAVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_ACTIVE_GRADIENT:
      case PROP_INACTIVE_GRADIENT:
      case PROP_SACTIVE_GRADIENT:
      case PROP_SINACTIVE_GRADIENT: {
         RGBAVAL rgba = (RGBAVAL)dwParam;
         int nIndex = 
            (wParam == PROP_ACTIVE_GRADIENT)   ? RGBINDEX_ACTIVE   :
            (wParam == PROP_INACTIVE_GRADIENT) ? RGBINDEX_INACTIVE :
            (wParam == PROP_SACTIVE_GRADIENT)  ? RGBINDEX_SACTIVE  :
            RGBINDEX_SINACTIVE;
         DynRGB_Set(&me->dynBGE, nIndex, rgba);
         goto invalidateAndHandle;
      } 

      // selected
      case PROP_SELECTED:
         me->bSelected = (boolean)dwParam;
         if (!me->bUpdateStop) Border_Invalidate(me, ICIF_REDRAW);

         // NOTE: Do not return TRUE here!  If the widget we contain
         //       is a container widget that contains other borders, we want
         //       the child borders to know about PROP_SELECTED as well
         return FALSE;

      // border style
      case PROP_BORDERSTYLE:
         me->nStyle = MIN(BORDERSTYLE_MAXSTYLE, (int)dwParam);
         goto invalidateAndHandle;

      // border padding
      case PROP_PADDING:
         me->padding.left =
            me->padding.right =
            me->padding.top = 
            me->padding.bottom = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_LEFT_PAD:
         me->padding.left = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_RIGHT_PAD:
         me->padding.right = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_TOP_PAD:
         me->padding.top = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_BOTTOM_PAD:
         me->padding.bottom = (uint8)dwParam;
         goto calcInvalidateAndHandle;

      // border width
      case PROP_BORDERWIDTH:
         //   nWidth  = border thickness
         me->nWidth[0] = me->nWidth[1] = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_ACTIVE_BORDERWIDTH:
         me->nWidth[RGBINDEX_ACTIVE] = (uint8)dwParam;
         goto calcInvalidateAndHandle;
      case PROP_INACTIVE_BORDERWIDTH:
         me->nWidth[RGBINDEX_INACTIVE] = MIN(me->nWidth[RGBINDEX_ACTIVE], (uint8)dwParam);
         goto calcInvalidateAndHandle;

      // border radius (rounded rectangles)
      case PROP_BORDERRADIUS:
         me->nRadius = (uint8)dwParam;
         goto calcInvalidateAndHandle;

      // border color
      case PROP_BORDERCOLOR:
         DynRGB_Collapse(&me->dynBorder, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SELECTED_BORDERCOLOR:
         DynRGB_Set(&me->dynBorder, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         DynRGB_Set(&me->dynBorder, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_ACTIVE_BORDERCOLOR:
         DynRGB_Set(&me->dynBorder, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_INACTIVE_BORDERCOLOR:
         DynRGB_Set(&me->dynBorder, RGBINDEX_INACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SACTIVE_BORDERCOLOR:
         DynRGB_Set(&me->dynBorder, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SINACTIVE_BORDERCOLOR:
         DynRGB_Set(&me->dynBorder, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;

     // border transparency
     case PROP_BORDERTRANSPARENCY:
         DynRGB_SetAlpha(&me->dynBorder, (uint8)(255 - (uint8)dwParam));
         goto invalidateAndHandle;


      // background color
      case PROP_BGCOLOR:
         DynRGB_Collapse(&me->dynBG, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SELECTED_BGCOLOR:
         DynRGB_Set(&me->dynBG, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         DynRGB_Set(&me->dynBG, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_ACTIVE_BGCOLOR:
         DynRGB_Set(&me->dynBG, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_INACTIVE_BGCOLOR:
         DynRGB_Set(&me->dynBG, RGBINDEX_INACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SACTIVE_BGCOLOR:
         DynRGB_Set(&me->dynBG, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SINACTIVE_BGCOLOR:
         DynRGB_Set(&me->dynBG, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;

      // background transparency
      case PROP_TRANSPARENCY:
      case PROP_BGTRANSPARENCY:
         DynRGB_SetAlpha(&me->dynBG, (uint8)(255 - (uint8)dwParam));
         goto invalidateAndHandle;

      // shadow offset
      case PROP_SHADOWOFFSET:
         DynShadow_SetOffsetX(&me->dynShadow, -1, (int)dwParam);
         DynShadow_SetOffsetY(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWOFFSETX:
         DynShadow_SetOffsetX(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWOFFSETY:
         DynShadow_SetOffsetY(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWCOLOR:
         DynShadow_SetRGBA(&me->dynShadow, -1, (RGBAVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSET:
         DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSETX:
         DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSETY:
         DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWCOLOR:
         DynShadow_SetRGBA(&me->dynShadow, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         DynShadow_SetRGBA(&me->dynShadow, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;

      // widget property lookups
      case PROP_APPLYWPROPS:
         Widget_ApplyPropsV((WResPropDesc*)dwParam);
         return TRUE;

      calcInvalidateAndHandle:
         if (!me->bUpdateStop) Border_CalcExtent(me);
      invalidateAndHandle:
         if (!me->bUpdateStop) Border_Invalidate(me, ICIF_REDRAW);
         return TRUE;
      }
      break;


   case EVT_WDG_GETPROPERTY:
      
      switch(wParam) {

      case PROP_BGIMAGE:
         if (me->pBackImage && me->pBackImage->piImage) {
            *((IImage**)dwParam) = me->pBackImage->piImage;
            IIMAGE_AddRef(me->pBackImage->piImage);
         } else {
            *((IImage**)dwParam) = 0;
         }
         return TRUE;

      case PROP_CLIENTRECT:
         *((AEERect*)dwParam) = me->rcClient;
         return TRUE;

      // selected
      case PROP_SELECTED:
         *((uint32*)dwParam) = (uint32) me->bSelected;
         return TRUE;

      // style
      case PROP_BORDERSTYLE:
         *((uint32*)dwParam) = (uint32) me->nStyle;
         return TRUE;

      // padding
      case PROP_LEFT_PAD:
         *((uint32*)dwParam) = (uint32) me->padding.left;
         return TRUE;
      case PROP_RIGHT_PAD:
         *((uint32*)dwParam) = (uint32) me->padding.right;
         return TRUE;
      case PROP_TOP_PAD:
         *((uint32*)dwParam) = (uint32) me->padding.top;
         return TRUE;
      case PROP_BOTTOM_PAD:
         *((uint32*)dwParam) = (uint32) me->padding.bottom;
         return TRUE;

      // transparency
      case PROP_TRANSPARENCY:
      case PROP_BGTRANSPARENCY: {
         RGBVAL rgba;
         DynRGB_Get(&me->dynBG, RGBINDEX_ACTIVE, &rgba);
         *((uint32*)dwParam) = (uint32) (255 - (uint8)RGBA_GETALPHA(rgba));
         return TRUE;
      }
      case PROP_BORDERTRANSPARENCY: {
         RGBVAL rgba;
         DynRGB_Get(&me->dynBorder, RGBINDEX_ACTIVE, &rgba);
         *((uint32*)dwParam) = (uint32) (255 - (uint8)RGBA_GETALPHA(rgba));
         return TRUE;
      }
         
      // border width
      case PROP_BORDERWIDTH:
      case PROP_ACTIVE_BORDERWIDTH:
         *((uint32*)dwParam) = (uint32) me->nWidth[RGBINDEX_ACTIVE];
         return TRUE;
      case PROP_INACTIVE_BORDERWIDTH:
         *((uint32*)dwParam) = (uint32) me->nWidth[RGBINDEX_INACTIVE];
         return TRUE;

      // border radius (rounded rectangles)
      case PROP_BORDERRADIUS:
         *((uint32*)dwParam) = me->nRadius;
         return TRUE;

      // border color
      case PROP_BORDERCOLOR:
      case PROP_ACTIVE_BORDERCOLOR:
         DynRGB_Get(&me->dynBorder, RGBINDEX_ACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_INACTIVE_BORDERCOLOR:
         DynRGB_Get(&me->dynBorder, RGBINDEX_INACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SELECTED_BORDERCOLOR:
      case PROP_SACTIVE_BORDERCOLOR:
         DynRGB_Get(&me->dynBorder, RGBINDEX_SACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SINACTIVE_BORDERCOLOR:
         DynRGB_Get(&me->dynBorder, RGBINDEX_SINACTIVE, (uint32*)dwParam);
         return TRUE;

      // background color
      case PROP_BGCOLOR:
      case PROP_ACTIVE_BGCOLOR:
         DynRGB_Get(&me->dynBG, RGBINDEX_ACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_INACTIVE_BGCOLOR:
         DynRGB_Get(&me->dynBG, RGBINDEX_INACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SELECTED_BGCOLOR:
      case PROP_SACTIVE_BGCOLOR:
         DynRGB_Get(&me->dynBG, RGBINDEX_SACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SINACTIVE_BGCOLOR:
         DynRGB_Get(&me->dynBG, RGBINDEX_SINACTIVE, (uint32*)dwParam);
         return TRUE;

      // shadow offsets
      case PROP_SHADOWOFFSETX:
      case PROP_SHADOWOFFSETY: {
         int x, y;
         Border_GetShadowOffsets(me, RGBINDEX_ACTIVE, &x, &y);
         *((uint32*)dwParam) = (uint32) (wParam == PROP_SHADOWOFFSETX) ? x : y;
         return TRUE;
      }
      case PROP_SELECTED_SHADOWOFFSETX:
      case PROP_SELECTED_SHADOWOFFSETY: {
         int x, y;
         Border_GetShadowOffsets(me, RGBINDEX_SACTIVE, &x, &y);
         *((uint32*)dwParam) = (uint32) (wParam == PROP_SELECTED_SHADOWOFFSETX) ? x : y;
         return TRUE;
      }

      // shadow color
      case PROP_SELECTED_SHADOWCOLOR:
      case PROP_SHADOWCOLOR: {
         Shadow shadow;
         DynShadow_Get(&me->dynShadow, (wParam == PROP_SHADOWCOLOR ? RGBINDEX_ACTIVE : RGBINDEX_SACTIVE), &shadow);
         *((uint32*)dwParam) = (uint32)shadow.rgba;
         return TRUE;
      }

      } break;
   }

   // handle all of the background image properties
   if (me->pBackImage && BackImage_HandleEvent(me->pBackImage, evt, wParam, dwParam)) {
      return TRUE;
   }

   return FALSE;
}


void Border_CtorZ(Border *me, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel)
{
   me->pfnInval = pfnInval; 
   me->pvInval  = pvInval;
   me->pExtent  = pExtent;

   me->nWidth[RGBINDEX_INACTIVE]   = 1;           // single pixel inactive border
   me->nWidth[RGBINDEX_ACTIVE]     = 2;           // double wide active border
   
   DynRGB_Ctor(&me->dynBorder, RGBA_BLACK);
   DynRGB_Ctor(&me->dynBG, RGBA_NONE);
   DynRGB_Ctor(&me->dynBGE, RGBA_NONE);

   me->bCanTakeFocus  = bCanTakeFocus;
   me->nStyle         = BORDERSTYLE_NORMAL;
   me->nGradientStyle = GRADIENT_STYLE_NONE;
   me->bUpdateStop    = FALSE;
   me->piViewModel = piViewModel;
}

void Border_Ctor(Border *me, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel)
{
   ZEROAT(me);
   Border_CtorZ(me, pfnInval, pvInval, pExtent, bCanTakeFocus, piViewModel);
}

void Border_Dtor(Border *me)
{
   DynRGB_Dtor(&me->dynBorder);
   DynRGB_Dtor(&me->dynBG);
   DynRGB_Dtor(&me->dynBGE);
   DynShadow_Dtor(&me->dynShadow);
   if (me->pBackImage) {
      BackImage_Delete(me->pBackImage);
   }
}
