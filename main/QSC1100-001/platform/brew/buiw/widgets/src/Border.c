/*======================================================
FILE:       Border.c

SERVICES:   Widget border drawing/managment

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEWProperties.h"
#include "AEEImage.h"
#include "Border.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"


/////////////////////////////////////////////////////////////////////////////
#define COLOR_INDEX(pb)  (((pb)->bActive ? 1 : 0) + (2 * ((pb)->bSelected ? 1 : 0)))

static void BackImage_ReplaceImage(BackImage *me, IImage *piImage);
/////////////////////////////////////////////////////////////////////////////
// BackImage (background image object)


static void BackImage_CtorZ(BackImage *me, IShell *piShell, IImage *piImage, PFNINVALIDATE pfnInval, void *pvInval, IModel **ppiViewModel)
{
   ImageBase_Ctor(&me->image, piShell, pfnInval, pvInval, ppiViewModel);
   BackImage_ReplaceImage(me, piImage);   // this also invalidates
}


static void BackImage_Dtor(BackImage *me)
{
   ImageBase_Dtor(&me->image);
}


static int BackImage_New(IImage *pii, IShell *piShell, PFNINVALIDATE pfnInval, void *pvInval, BackImage **ppo, IModel **piViewModel)
{
   int nErr = ENOMEMORY;

   *ppo = MALLOCREC(BackImage);
   if (*ppo) {
      nErr = SUCCESS;
   }

   if (!nErr) {
      BackImage_CtorZ(*ppo, piShell, pii, pfnInval, pvInval, piViewModel);
   }

   return nErr;
}


static void BackImage_Delete(BackImage *me)
{
   BackImage_Dtor(me);
   FREE(me);
}

static void BackImage_Invalidate(BackImage *me, uint32 dwFlags)
{
   if (me->image.pfnInval) {
      me->image.pfnInval(me->image.pvInval, dwFlags);
   }
}


static void BackImage_ReplaceImage(BackImage *me, IImage *piImage)
{
   ImageBase_SetImage(&me->image, piImage);
}


static void BackImage_Draw(BackImage *me, ICanvas *piCanvas, int x, int y, 
                           uint16 dxClient, uint16 dyClient, boolean bSelected)
{
   me->image.bSelected = bSelected;
   ImageBase_Draw(&me->image, piCanvas, x, y, dxClient, dyClient);
}


static boolean BackImage_HandleEvent(BackImage *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   if (evt == EVT_WDG_SETPROPERTY) {

      switch (wParam) {
      
         case PROP_BGIMAGE_FLAGS: {
            uint32 dwSave = me->dwFlags;

            me->dwFlags = dwParam;

            if ((dwSave & BGIF_HASSELECTIMAGE) != (me->dwFlags & BGIF_HASSELECTIMAGE)) {
               me->image.bHasSelectImage = (0 != (me->dwFlags & BGIF_HASSELECTIMAGE));
            }
            if ((dwSave & BGIF_TILED) != (me->dwFlags & BGIF_TILED)) {
               me->image.bTiled = (0 != (me->dwFlags & BGIF_TILED));
            }

            // strip out alignment flags and set in imagebase
            me->image.dwAlignment = me->dwFlags & IDF_ALIGN_MASK;
            BackImage_Invalidate(me, 0);
            return TRUE;
         }

         case PROP_BGIMAGE_PARM:
            ImageBase_SetParm(&me->image, (ImageParm *)dwParam);
            return TRUE;

         case PROP_BGIMAGE_ANIMATE:
            ImageBase_Animate(&me->image, (boolean)dwParam);
            return TRUE;

         case PROP_BGIMAGE_TILED_BORDER: {
            BGImageTiledBorder *ptb = (BGImageTiledBorder*)dwParam;
            me->image.bdl = ptb->left;
            me->image.bdt = ptb->top;
            me->image.bdr = ptb->right;
            me->image.bdb = ptb->bottom;
            BackImage_Invalidate(me, 0);
            return TRUE;
         }

         case PROP_BGIMAGE_FRAME:
            return ImageBase_SetFrame(&me->image, (int)dwParam);

         case PROP_BGIMAGE_ANIMATE_FLAGS:
            me->image.dwAnimFlags = dwParam;
            return TRUE;

         default: break;
      } // switch

   } else if (evt == EVT_WDG_GETPROPERTY) {

      switch (wParam) {

         case PROP_BGIMAGE_FLAGS:
            *(uint32 *)dwParam = me->dwFlags;
            return TRUE;

         case PROP_BGIMAGE_TILED_BORDER:
            // return individual values in a struct
            ((BGImageTiledBorder*)dwParam)->left   = me->image.bdl;
            ((BGImageTiledBorder*)dwParam)->top    = me->image.bdt;
            ((BGImageTiledBorder*)dwParam)->right  = me->image.bdr;
            ((BGImageTiledBorder*)dwParam)->bottom = me->image.bdb;
            return TRUE;
         
         case PROP_BGIMAGE_ANIMATE_FLAGS:
            *(uint32*)dwParam = me->image.dwAnimFlags;
            return TRUE;

         default: break;
      } // switch

   }

   return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// DynRGB

static int MemRealloc(void **ppMem, int cbNewSize)
{
   void *pMem = *ppMem;
   pMem = REALLOC(pMem, (uint32)cbNewSize);
   if (!pMem) {
      return ENOMEMORY;
   }
   *ppMem = pMem;
   return SUCCESS;
}

static int Widget_LoadPropsV(IResFile *po, uint32 **ppo, uint32 *psize, VaListPtrType args)
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

   if (!desc || !desc->piResFile || !desc->args || !desc->piWidget) {
      return EBADPARM;
   }

   nErr = Widget_LoadPropsV(desc->piResFile, &props, &size, desc->args);

   if (!nErr) {
      uint32 *tmp;
      size = size / sizeof(uint32);

      (void) IWIDGET_EnableLayout(desc->piWidget, FALSE);
      for (tmp = props, size = size / 2; size > 0; size--) {
         uint32 key = *tmp++;
         uint32 value = *tmp++;
         (void) IWIDGET_SetProperty(desc->piWidget, (uint16)key, value);
      }
      (void) IWIDGET_EnableLayout(desc->piWidget, TRUE);
      (void) IWIDGET_ForceLayout(desc->piWidget);
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
   (void) DynRGB_Get(me, RGBINDEX_SACTIVE, &rgbFill);

   // Zero out pointer for realloc, if necessary
   if (!me->nMaxIndex) me->u.prgb = 0;

   // Realloc and fill from start index to max index
   nErr = MemRealloc((void**)&me->u.prgb, (nMaxIndex+1) * (int)sizeof(RGBVAL));
   if (!nErr) {
      int i;
      for (i = me->nMaxIndex; i <= nMaxIndex; i++) {
         me->u.prgb[i] = rgbFill;
      }
      me->nMaxIndex = (uint8)nMaxIndex;
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
   (void) DynRGB_Get(me, n1, &rgb_1);
   (void) DynRGB_Get(me, n2, &rgb_2);
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

      nErr = MemRealloc((void**)&me->u.pshadow, (nIndex+1) * (int)sizeof(Shadow));
      if (!nErr) {
         int i;
         // when expanding the array, we don't want any uninitialized
         // values, so we propagate the last value to the newly
         // allocated elements
         for (i = me->nMaxIndex; i < nIndex; i++) {
            me->u.pshadow[i] = shadow;
         }
         me->nMaxIndex = (uint8)nIndex;
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

void Border_GetShadowOffsets(Border *me, int nIndex, int *pnOffsetX, int *pnOffsetY) 
{
   Shadow shadow;
   
   if (nIndex == -1) {
      nIndex = COLOR_INDEX(me);
   }
   (void) DynShadow_Get(&me->dynShadow, nIndex, &shadow);
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
      (void) DynShadow_Get(me, nIndex, &shadow);
      switch(field) {
      case DSF_RGBA:
         shadow.rgba = (RGBAVAL)val;
         break;
      case DSF_OFFSETX:
         shadow.nOffsetX = (char)val;
         break;
      case DSF_OFFSETY:
         shadow.nOffsetY = (char)val;
         break;
      default: break;
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
   return DynShadow_SetVal(me, nIndex, DSF_RGBA, (int)rgba);
}

/////////////////////////////////////////////////////////////////////////////
// Border

RGBVAL Border_GetCurrentBorderColor(Border *me) 
{
   RGBVAL rgb;
   (void) DynRGB_Get(&me->dynBorder, COLOR_INDEX(me), &rgb);
   return rgb;
}

RGBVAL Border_GetCurrentBGColor(Border *me) 
{
   RGBVAL rgb;
   (void) DynRGB_Get(&me->dynBG, COLOR_INDEX(me), &rgb);
   return rgb;
}

void Border_SetBorderColor(Border *me, int index, RGBVAL rgb)
{
   index = CONSTRAIN(index, 0, RGBINDEX_SACTIVE);
   (void) DynRGB_Set(&me->dynBorder, index, rgb);
}

void Border_SetBGColor(Border *me, int index, RGBVAL rgb)
{
   index = CONSTRAIN(index, 0, RGBINDEX_SACTIVE);
   (void) DynRGB_Set(&me->dynBG, index, rgb);
}

void Border_GetBorderColor(Border *me, int index, RGBVAL *prgb) 
{
   (void) DynRGB_Get(&me->dynBorder, index, prgb);
}

void Border_GetBGColor(Border *me, int index, RGBVAL *prgb) 
{
   (void) DynRGB_Get(&me->dynBG, index, prgb);
}


void Border_AdjustDisplayClipRect(Border *me, ICanvas *piCanvas, int x, int y)
{
   AEERect rcClip, rcClient;

   (void) ICANVAS_GetClipRect(piCanvas, &rcClip);

   SETAEERECT(&rcClient, x + me->rcClient.x, y + me->rcClient.y, 
              me->rcClient.dx, me->rcClient.dy);

   (void) IntersectRect(&rcClip, &rcClip, &rcClient);

   (void) ICANVAS_SetClipRect(piCanvas, &rcClip);
}


void Border_Draw(Border *me, ICanvas *piCanvas, int x, int y)
{
   AEERect rc;
   IDisplay *piDisplay = 0;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      int ndxW = (me->bActive != 0);   // index into me->nWidth
      int ndxC = COLOR_INDEX(me);      // index into me->rgbBorder & me->rgbBackground
      RGBVAL rgbaBorder, rgbaBG = RGBA_WHITE, rgbaBGE = RGBA_WHITE;
      Shadow shadow;
      boolean bHasGradient = me->nGradientStyle != GRADIENT_STYLE_NONE;
      AEERect rcSaveClip, rcClip;
      
      (void) DynShadow_Get(&me->dynShadow, ndxC, &shadow);
      (void) DynRGB_Get(&me->dynBorder, ndxC, &rgbaBorder);
      (void) DynRGB_Get(&me->dynBG, ndxC, &rgbaBG);
      if (bHasGradient) {
         (void) DynRGB_Get(&me->dynBGE, ndxC, &rgbaBGE);
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
                  rcShadow.x  = (int16) (x + ((shadow.nOffsetX < 0) ? 0 : shadow.nOffsetX));
                  rcShadow.dx = (int16) rc.dx;
                  rcShadow.y  = (int16) (y + ((shadow.nOffsetY < 0) ? 0 : rc.dy));
                  rcShadow.dy = (int16) ABS(shadow.nOffsetY);
                  (void) BlendRect(piDisplay, &rcShadow, shadow.rgba, RGBA_GETALPHA(shadow.rgba));
               }
               if (shadow.nOffsetX) {
                  rcShadow.x  = (int16) (x + ((shadow.nOffsetX < 0) ? 0 : rc.dx));
                  rcShadow.dx = (int16) ABS(shadow.nOffsetX);
                  rcShadow.y  = (int16) (y + ABS(shadow.nOffsetY));
                  rcShadow.dy = (int16) (rc.dy - ABS(shadow.nOffsetY));
                  (void) BlendRect(piDisplay, &rcShadow, shadow.rgba, RGBA_GETALPHA(shadow.rgba));
               }
            } else {
               // shadow for rounded or beveled borders
               int nThick;

               if (me->nStyle == BORDERSTYLE_ROUNDED) {
                  nThick = MAX(me->nRadius, me->nWidth[ndxW]);
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
      (void) IntersectRect(&rcClip, &rc, &rcSaveClip);
      IDISPLAY_SetClipRect(piDisplay, &rcClip);
      (void) ICANVAS_SetClipRect(piCanvas, &rcClip);

      if (me->pBackImage && me->pBackImage->image.pii && 
          (me->pBackImage->dwFlags & BGIF_IMAGEUNDER)) {

         BackImage_Draw(me->pBackImage, piCanvas, rc.x, rc.y, 
                        (uint16)me->rcClient.dx, (uint16)me->rcClient.dy, 
                        me->bSelected);
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
            (void) BlendRect(piDisplay, &rc, rgbaBG, RGBA_GETALPHA(rgbaBG));
         }
      }

      if (me->pBackImage && me->pBackImage->image.pii && 
          (0 == (me->pBackImage->dwFlags & BGIF_IMAGEUNDER))) {
         BackImage_Draw(me->pBackImage, piCanvas, rc.x, rc.y, 
                        (uint16)me->rcClient.dx, (uint16)me->rcClient.dy, me->bSelected);
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

void Border_CalcClientRect(Border *me) {
   // compute and set the client rect  
   Border_GetClientRect(me, &me->rcClient, me->pExtent);
}

void Border_GetClientRect(Border *me, AEERect* prcClient, const WExtent* pe )
{
   int nShadowOffsetX, nShadowOffsetY;

   // if changing the following caculation,
   // the identical code that computes this 
   // client width is in Static Widget 
   // GetPreferredExtent...modified that  code
   // when changing 
   Border_GetCurrentShadowOffsets(me, &nShadowOffsetX, &nShadowOffsetY);
   

   if ( prcClient ) {
      // setup client rect here...
      SETAEERECT(prcClient, 
                 (int)me->nWidth[1] + me->padding.left + (nShadowOffsetX < 0 ? ABS(nShadowOffsetX) : 0),
                 (int)me->nWidth[1] + me->padding.top + (nShadowOffsetY < 0 ? ABS(nShadowOffsetY) : 0),
                 MAX(pe->width - (me->nWidth[1] * 2) - me->padding.left - me->padding.right - ABS(nShadowOffsetX), 0),
                 MAX(pe->height - (me->nWidth[1] * 2) - me->padding.top - me->padding.bottom - ABS(nShadowOffsetY), 0));
   }

}

void Border_CalcExtent(Border *me)
{
   Border_CalcClientRect(me);
   Border_Invalidate(me, ICIF_EXTENT);
}


/* returns the preferred extent for a required client 
   extent (the area inside border+padding) 
*/
void Border_CalcPreferredExtent(Border *me, WExtent *pweOut, const WExtent *pweClientIn)
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
   
   (void) DynRGB_Get(&me->dynBG, ndxC, &rgbaBG);

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
      rc.dx -= (int16)ABS(nShadowOffsetX);
      rc.dy -= (int16)ABS(nShadowOffsetY);

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
               if (SUCCESS != BackImage_New((IImage*)dwParam, me->piShell, 
                                            me->pfnInval, me->pvInval, &me->pBackImage, 
                                            me->piViewModel)) {
                  return FALSE;
               }
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
         (void) DynRGB_Set(&me->dynBGE, RGBINDEX_SACTIVE, (RGBAVAL)dwParam);
         (void) DynRGB_Set(&me->dynBGE, RGBINDEX_SINACTIVE, (RGBAVAL)dwParam);
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
         (void) DynRGB_Set(&me->dynBGE, nIndex, rgba);
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
         me->nStyle = (uint8) MIN(BORDERSTYLE_MAXSTYLE, (int)dwParam);
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
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_ACTIVE_BORDERCOLOR:
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_INACTIVE_BORDERCOLOR:
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_INACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SACTIVE_BORDERCOLOR:
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SINACTIVE_BORDERCOLOR:
         (void) DynRGB_Set(&me->dynBorder, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
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
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_ACTIVE_BGCOLOR:
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_INACTIVE_BGCOLOR:
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_INACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SACTIVE_BGCOLOR:
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SINACTIVE_BGCOLOR:
         (void) DynRGB_Set(&me->dynBG, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;

      // background transparency
      case PROP_TRANSPARENCY:
      case PROP_BGTRANSPARENCY:
         DynRGB_SetAlpha(&me->dynBG, (uint8)(255 - (uint8)dwParam));
         goto invalidateAndHandle;

      // shadow offset
      case PROP_SHADOWOFFSET:
         (void) DynShadow_SetOffsetX(&me->dynShadow, -1, (int)dwParam);
         (void) DynShadow_SetOffsetY(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWOFFSETX:
         (void) DynShadow_SetOffsetX(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWOFFSETY:
         (void) DynShadow_SetOffsetY(&me->dynShadow, -1, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SHADOWCOLOR:
         (void) DynShadow_SetRGBA(&me->dynShadow, -1, (RGBAVAL)dwParam);
         goto invalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSET:
         (void) DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         (void) DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         (void) DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         (void) DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSETX:
         (void) DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         (void) DynShadow_SetOffsetX(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWOFFSETY:
         (void) DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SACTIVE, (int)dwParam);
         (void) DynShadow_SetOffsetY(&me->dynShadow, RGBINDEX_SINACTIVE, (int)dwParam);
         goto calcInvalidateAndHandle;
      case PROP_SELECTED_SHADOWCOLOR:
         (void) DynShadow_SetRGBA(&me->dynShadow, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         (void) DynShadow_SetRGBA(&me->dynShadow, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         goto invalidateAndHandle;

      // widget property lookups
      case PROP_APPLYWPROPS:
         (void) Widget_ApplyPropsV((WResPropDesc*)dwParam);
         return TRUE;

      calcInvalidateAndHandle:
         if (!me->bUpdateStop) Border_CalcExtent(me);
      invalidateAndHandle:
         if (!me->bUpdateStop) Border_Invalidate(me, ICIF_REDRAW);
         return TRUE;

      default: break;
      }
      break;


   case EVT_WDG_GETPROPERTY:
      
      switch(wParam) {

      case PROP_BGIMAGE:
         if (me->pBackImage && me->pBackImage->image.pii) {
            *((IImage**)dwParam) = me->pBackImage->image.pii;
            IIMAGE_AddRef(me->pBackImage->image.pii);
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
         (void) DynRGB_Get(&me->dynBG, RGBINDEX_ACTIVE, &rgba);
         *((uint32*)dwParam) = (uint32) (255 - (uint8)RGBA_GETALPHA(rgba));
         return TRUE;
      }
      case PROP_BORDERTRANSPARENCY: {
         RGBVAL rgba;
         (void) DynRGB_Get(&me->dynBorder, RGBINDEX_ACTIVE, &rgba);
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
         (void) DynRGB_Get(&me->dynBorder, RGBINDEX_ACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_INACTIVE_BORDERCOLOR:
         (void) DynRGB_Get(&me->dynBorder, RGBINDEX_INACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SELECTED_BORDERCOLOR:
      case PROP_SACTIVE_BORDERCOLOR:
         (void) DynRGB_Get(&me->dynBorder, RGBINDEX_SACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SINACTIVE_BORDERCOLOR:
         (void) DynRGB_Get(&me->dynBorder, RGBINDEX_SINACTIVE, (uint32*)dwParam);
         return TRUE;

      // background color
      case PROP_BGCOLOR:
      case PROP_ACTIVE_BGCOLOR:
         (void) DynRGB_Get(&me->dynBG, RGBINDEX_ACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_INACTIVE_BGCOLOR:
         (void) DynRGB_Get(&me->dynBG, RGBINDEX_INACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SELECTED_BGCOLOR:
      case PROP_SACTIVE_BGCOLOR:
         (void) DynRGB_Get(&me->dynBG, RGBINDEX_SACTIVE, (uint32*)dwParam);
         return TRUE;
      case PROP_SINACTIVE_BGCOLOR:
         (void) DynRGB_Get(&me->dynBG, RGBINDEX_SINACTIVE, (uint32*)dwParam);
         return TRUE;

      // shadow offsets
      case PROP_SHADOWOFFSETX:
      case PROP_SHADOWOFFSETY: {
         int x, y;
         Border_GetShadowOffsets(me, RGBINDEX_ACTIVE, &x, &y);
         *((uint32*)dwParam) = (uint32) ((wParam == PROP_SHADOWOFFSETX) ? x : y);
         return TRUE;
      }
      case PROP_SELECTED_SHADOWOFFSETX:
      case PROP_SELECTED_SHADOWOFFSETY: {
         int x, y;
         Border_GetShadowOffsets(me, RGBINDEX_SACTIVE, &x, &y);
         *((uint32*)dwParam) = (uint32) ((wParam == PROP_SELECTED_SHADOWOFFSETX) ? x : y);
         return TRUE;
      }

      // shadow color
      case PROP_SELECTED_SHADOWCOLOR:
      case PROP_SHADOWCOLOR: {
         Shadow shadow;
         (void) DynShadow_Get(&me->dynShadow, (wParam == PROP_SHADOWCOLOR ? RGBINDEX_ACTIVE : RGBINDEX_SACTIVE), &shadow);
         *((uint32*)dwParam) = (uint32)shadow.rgba;
         return TRUE;
      }

      default: break;

      } break;

   default: break;
   }

   // handle all of the background image properties
   if (me->pBackImage && BackImage_HandleEvent(me->pBackImage, evt, wParam, dwParam)) {
      return TRUE;
   }

   return FALSE;
}


void Border_CtorZ(Border *me, IShell *piShell, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel)
{
   me->piShell = piShell;
   ADDREFIF(me->piShell);

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

void Border_Ctor(Border *me, IShell *piShell, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel)
{
   ZEROAT(me);
   Border_CtorZ(me, piShell, pfnInval, pvInval, pExtent, bCanTakeFocus, piViewModel);
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
   RELEASEIF(me->piShell);
}
