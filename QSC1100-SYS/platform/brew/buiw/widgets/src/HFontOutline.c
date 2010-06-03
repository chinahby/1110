/*
  ========================================================================

  FILE:     HFontOutline.c
  
  SERVICES: Extended IFont object that implements HandleEvent.
            Used to render text with a glow effect.

  GENERAL DESCRIPTION: 

            Extended IFont object that implements HandleEvent.
            Currently implementation allows IFont, outline width and 
            outline color ttributes to be get/set.  Resulting IHFont
            can be used as a regular IFont to render text with a 
            glow effect.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "HFontOutline.h"
#include "ModelBase.h"
#include "wutil.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// HFont
typedef struct 
{
   const AEEVTBL(IHFont)  *pvt;
   
   uint32                  dwRefs;

   IModel                 *piModel;       // a model to notify user of font changes

   IShell                 *piShell;
   IModule                *piModule;

   IFont                  *piFont;        // the font that will be outlined
   int                     dwWidth;       // the width of the outline
   RGBVAL                  rgb;           // the outline color

   IBitmap                *piBitmap;      // character slot
   AEEBitmapInfo           bmpInfo;       // bitmap size
   AEERect                 rcBmp;         // bitmap rect
} HFont;

/*---------------------------------------------------------------------------------
      Globals
---------------------------------------------------------------------------------*/

// bit masks for native color
static const uint32 ncBitMasks[4] = { 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };

/*---------------------------------------------------------------------------------
      Function Prototypes
---------------------------------------------------------------------------------*/

// 
// IHFont
//
static uint32	         HFont_AddRef(IHFont *po);
static uint32	         HFont_Release(IHFont *po);
static int              HFont_QueryInterface(IHFont *pMe, AEECLSID cls, void **ppo);
static int              HFont_DrawText(IHFont *po, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars, 
                                       NativeColor fg, NativeColor bg, const AEERect *prcClip, uint32 dwFlags);
static int              HFont_MeasureText(IHFont *po, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnFits, int *pnPixels);
static int              HFont_GetInfo(IHFont *po, AEEFontInfo *pinfo, int nSize);
static boolean          HFont_HandleEvent(IHFont *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

// Local Routines
static void HFont_Dtor(HFont *pMe);
static int  HFont_Ctor(HFont *pMe, AEEVTBL(IHFont) *pvt, IShell *piShell, IModule *piModule);

// outline blitters
typedef void (*PFNBLITWITHOUTLINE)(IDIB *pSrcDIB, AEERect *prcGlyph, int dwWidth, IDIB *pDestDIB, 
                                   int x, int y, AEERect *prcDest, NativeColor glyphBG, NativeColor ol);
static void HFont_Blit16(IDIB *pSrcDIB, AEERect *prcGlyph, int dwWidth, IDIB *pDestDIB, 
                         int x, int y, AEERect *prcDest, NativeColor glyphBG, NativeColor ol);
static void HFont_BlitOther(IDIB *pSrcDIB, AEERect *prcGlyph, int dwWidth, IDIB *pDestDIB, 
                            int x, int y, AEERect *prcDest, NativeColor glyphBG, NativeColor ol);

// algorithm for single and double pixel outlines
static int HFont_Draw1or2PixelOutline(HFont *pMe, 
                                        IBitmap *pDst, 
                                        int x, 
                                        int y, 
                                        const AECHAR *pcText, 
                                        int nChars, 
                                        NativeColor fg, 
                                        NativeColor bg, 
                                        const AEERect *prcClip, 
                                        uint32 dwFlags);

// draw outlined text
static int HFont_DrawNPixelOutline(HFont *pMe, 
                             IBitmap *pDst, 
                             IDIB *pDstDIB, 
                             PFNBLITWITHOUTLINE pfnBlit,
                             int x, 
                             int y, 
                             const AECHAR *pcText, 
                             int nChars, 
                             NativeColor fg, 
                             NativeColor bg, 
                             const AEERect *prcClip, 
                             uint32 dwFlags);

// Notify through our model of font change
static __inline void HFont_NotifyStrokeChange(HFont *pMe) {
   if (pMe->piModel) {
      ModelEvent mev;
      mev.evCode  = EVT_MDL_VALUE;
      IMODEL_Notify(pMe->piModel, &mev);
   }
}

// define this if we have to.  Needed for BUIW 1.2.2.
#ifndef RGBA_STRIPALPHA
// strip the alpha channel from an RGBAVAL
#define RGBA_STRIPALPHA(r) (r&0xFFFFFF00)
#endif

/*---------------------------------------------------------------------------------


      IHFont


---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 HFont_AddRef(IHFont* po)
{
   HFont *pMe = (HFont *)po;
   return ++pMe->dwRefs;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 HFont_Release(IHFont* po)
{
   HFont *pMe = (HFont *)po;

    if (--pMe->dwRefs)
      return pMe->dwRefs;

   HFont_Dtor(pMe);

   FREE(pMe);
   return(0);
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int HFont_QueryInterface(IHFont *po, AEECLSID cls, void **ppo)
{
   HFont *pMe = (HFont *)po;

   if (cls == AEEIID_MODEL) {
      *ppo = pMe->piModel;
      IMODEL_AddRef(*ppo);
      return SUCCESS;
   }

   if (cls == AEEIID_HFONT) {
      *ppo = pMe;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } else {
      return ECLASSNOTSUPPORT;
   }
}

/* -----------------------------------------------------------------------
    HFont_DrawText - IFONT_DrawText() equivalent             
   --- */
static int HFont_DrawText(IHFont *po, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars, 
                          NativeColor fg, NativeColor bg, const AEERect *prcClip, uint32 dwFlags)
{
   HFont *pMe = (HFont *)po;
   int dwResult;
   IDIB *pDstDIB = NULL;
   PFNBLITWITHOUTLINE pfnBlit;

   // make sure we're properly set up
   if (!pMe->piFont) {
      return EBADSTATE;
   }

    // get the destination DIB ...
   dwResult = IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void **)&pDstDIB);
   if (dwResult == SUCCESS) {

      // trim off alpha from foreground and background colors.
      // we have to do this becuase BUIT only knows about RGBA.
      fg &= ncBitMasks[(pDstDIB->nDepth - 1)/8];
      bg &= ncBitMasks[(pDstDIB->nDepth - 1)/8];

      // Optimization for NULL outline color or 0 outline width
      if ((pMe->rgb == RGBA_NONE) || (pMe->dwWidth == 0)) {

         // NULL outline color or 0 width outline specified. 
         // Draw text without glow effect but with glow spacing.
         dwResult = IFONT_DrawText(pMe->piFont, pDst, x + pMe->dwWidth, y + pMe->dwWidth, pcText, nChars, fg, bg, prcClip, dwFlags);

      } else {

         // Outline color and outline width specified. 
         // Draw text with glow effect and glow spacing.
   
         // determine which blit function to use ...
         if (pMe->dwWidth <= 2) {
      
            // draw single or double pixel outline using differnt algorithm ...
            dwResult = HFont_Draw1or2PixelOutline(pMe, pDst, x, y, pcText, nChars, fg, bg, prcClip, dwFlags);

         } else {

            // pick outline blit function ...
            switch (pDstDIB->nDepth)
            {
               // 16 bit optimized blit
               case 16:
                  pfnBlit = HFont_Blit16;
                  break;

               // other depths
               default:
                  pfnBlit = HFont_BlitOther;
                  break;
            }

            // ... and draw and n-pixel outline ...
            dwResult = HFont_DrawNPixelOutline(pMe, pDst, pDstDIB, pfnBlit, x, y, pcText, nChars, fg, bg, prcClip, dwFlags);
         }
      }
   }

   // cleanup
   RELEASEIF(pDstDIB);

   return (dwResult);
}

/* -----------------------------------------------------------------------
    HFont_MeasureText - IFONT_MeasureText() equivalent      
   --- */
static int HFont_MeasureText(IHFont *po, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnFits, int *pnPixels)
{
   HFont *pMe = (HFont *)po;
   int dwResult = SUCCESS;

   // make sure we're properly set up
   if (!pMe->piFont) {
      return EBADSTATE;
   }

   // sanity check parameters
   if (pnFits && pnPixels) {

      // outline must be drawn, so subtract it from the max width
      nMaxWidth -= 2 * pMe->dwWidth;
      // measure the text ...
      dwResult = IFONT_MeasureText(pMe->piFont, pcText, nChars, nMaxWidth, pnFits, pnPixels);

      // add outline width to final pixel count
      if (nChars) {
         *pnPixels += 2 * pMe->dwWidth;
      }

   } else {
      // caller supplied bad parameters
      dwResult = EBADPARM;
   }

   return dwResult;
}

/* -----------------------------------------------------------------------
   HFont_GetInfo - IFONT_GetInfo() equivalent    
   --- */
static int HFont_GetInfo(IHFont *po, AEEFontInfo *pinfo, int nSize)
{
   HFont *pMe = (HFont *)po;
   int dwResult = SUCCESS; 
   
   // make sure we're properly set up
   if (!pMe->piFont) {
      return EBADSTATE;
   }

   dwResult = IFONT_GetInfo(pMe->piFont, pinfo, nSize);

   pinfo->nAscent += pMe->dwWidth;
   pinfo->nDescent += pMe->dwWidth;

   return dwResult;
}

/* -----------------------------------------------------------------------
    HFont_HandleEvent
   --- */
static boolean HFont_HandleEvent(IHFont *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   HFont *pMe = (HFont *)po;

   switch(evt) {
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_FONT) {
            *(IFont **) dwParam = pMe->piFont;
            ADDREFIF(pMe->piFont);
            return TRUE;
         }
         if (wParam == PROP_FONT_OUTLINEWIDTH) {
            *((int *)dwParam) = pMe->dwWidth;
            return TRUE;
         }
         if (wParam == PROP_FONT_OUTLINECOLOR) {
            *((RGBVAL *)dwParam) = pMe->rgb;
            return TRUE;
         }
         break;
      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_FONT_CLASS) {
            IFont *piFont;
            if (SUCCESS == ISHELL_CreateFont(pMe->piShell, (AEECLSID)dwParam, &piFont)) {
               RELEASEIF(pMe->piFont);
               pMe->piFont = (IFont *)piFont;
            } else {
               return FALSE;
            }

         } else if (wParam == PROP_FONT) {
            RELEASEIF(pMe->piFont);
            pMe->piFont = (IFont *)dwParam;
            ADDREFIF(pMe->piFont);

         } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
            if ((int)dwParam >= 0) {
               pMe->dwWidth = (int)dwParam;
            } else {
               return FALSE;
            }

         } else if (wParam == PROP_FONT_OUTLINECOLOR) {
            pMe->rgb = (RGBVAL)dwParam;
         
         } else {
            break;
         }

         HFont_NotifyStrokeChange(pMe);
         return TRUE;

      default:
         break;
   }

   return FALSE;
}
/*---------------------------------------------------------------------------------
      CreateInstance
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    HFontOutline_New  
   --- */
int HFontOutline_New(IHFont **ppo, IShell *piShell, IModule *piModule)
{
   int dwResult = SUCCESS;
   HFont *pMe;

   // sanity check parameters
   if(!ppo) {
		return(EFAILED);
   }

   // initialize
   *ppo = NULL;

   // allocate memory
   pMe = MALLOCREC_VTBL(HFont, IHFont);
   if (!pMe) {
      dwResult = ENOMEMORY;
   }

   // construct IHFont
   if (dwResult == SUCCESS) { 
      dwResult = HFont_Ctor(pMe, GETVTBL(pMe,IHFont), piShell, piModule);

      // return result
      if (dwResult == SUCCESS) {
         *ppo = (IHFont*) pMe;
      } else {
         HFont_Release((IHFont *)pMe);
      }
   }

   return dwResult;
}

/*---------------------------------------------------------------------------------


      Local Routines


---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    HFont_Ctor 
   --- */
static int HFont_Ctor(HFont *pMe, AEEVTBL(IHFont) *pvt, IShell *piShell, IModule *piModule)
{
   int dwResult = SUCCESS;

   pMe->pvt = pvt;

   pvt->AddRef          = HFont_AddRef;
   pvt->Release         = HFont_Release;
   pvt->QueryInterface  = HFont_QueryInterface;
   pvt->DrawText        = HFont_DrawText;
   pvt->MeasureText     = HFont_MeasureText;
   pvt->GetInfo         = HFont_GetInfo;
   pvt->HandleEvent     = HFont_HandleEvent;

   pMe->dwRefs = 1;

   pMe->piShell = piShell;
   ISHELL_AddRef(pMe->piShell);

   pMe->piModule = piModule;
   if (pMe->piModule) {
      IMODULE_AddRef(pMe->piModule);
   }

   dwResult = ModelBase_New(&pMe->piModel, pMe->piModule);
   if (SUCCESS == dwResult) {
      // defaults
      pMe->dwWidth = 1;
      pMe->rgb = RGB_WHITE;
   }

   return dwResult;
}

/* -----------------------------------------------------------------------
    HFont_Dtor
   --- */
static void HFont_Dtor(HFont *pMe)
{
   RELEASEIF(pMe->piFont);
   RELEASEIF(pMe->piBitmap);
   RELEASEIF(pMe->piModel);
   RELEASEIF(pMe->piModule);
   RELEASEIF(pMe->piShell);
}

/* -----------------------------------------------------------------------
    HFont_Draw1or2PixelOutline, optimized routine for one or two pixel  
    outlines.  Fast for devices that don't have an optimized blit as well.
   --- */
static int HFont_Draw1or2PixelOutline(HFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars, 
                                        NativeColor fg, NativeColor bg, const AEERect *prcClip, uint32 dwFlags)
{
   int dwResult = SUCCESS; 
   AEEFontInfo fi;
   int nOriginalFits, nOriginalPixels;
   AEERect rcLine, rcDestGlyph, rcIntersection;   
   NativeColor outlineColor = IBITMAP_RGBToNative(pDst, RGBA_STRIPALPHA(pMe->rgb));
   NativeColor glyphBG = IBITMAP_RGBToNative(pDst, RGBA_STRIPALPHA(RGB_WHITE));
   int dwSourceX, dwSourceY;

   // how tall are these characters?
   dwResult = IFONT_GetInfo(pMe->piFont, &fi, sizeof(AEEFontInfo));
   if (dwResult == SUCCESS) {
               
      //
      // draw the character into a temporary bitmap
      //

      // figure out how big this line of characters is
      dwResult = IFONT_MeasureText(pMe->piFont, pcText, nChars, IFONT_MAXWIDTH, &nOriginalFits, &nOriginalPixels);
      if (dwResult == SUCCESS) {

         // determine this size of this line of characters
         SETAEERECT(&rcLine, 
                    1, 
                    1, 
                    nOriginalPixels + 2*pMe->dwWidth, 
                    fi.nAscent + fi.nDescent + 2*pMe->dwWidth);

         // determine how much of this line of characters fits on screen
         SETAEERECT(&rcDestGlyph, 
                    x, 
                    y, 
                    rcLine.dx, 
                    rcLine.dy);

         if (prcClip) {
            if (!IntersectRect(&rcIntersection, &rcDestGlyph, prcClip)) {
               // no intersection.  we're done.
               return SUCCESS;
            }
         }

         // will this line fit in our interal temp bitmap?  Note, the bitmap is going to be big 
         // enough for the entire outlined line as well as single pixel border all the way around to 
         // speed up blitting later
         if (!pMe->piBitmap || 
            ((uint32)(rcLine.dx + 2) > pMe->bmpInfo.cx) 
            || ((uint32)(rcLine.dy + 2) > pMe->bmpInfo.cy)) {

            // nope, it won't.  Create a new one.
            RELEASEIF(pMe->piBitmap);
            pMe->bmpInfo.cx = rcLine.dx + 2;
            pMe->bmpInfo.cy = rcLine.dy + 2;
            dwResult = IBITMAP_CreateCompatibleBitmap(pDst, &pMe->piBitmap, (uint16)pMe->bmpInfo.cx, (uint16)pMe->bmpInfo.cy);
            SETAEERECT(&pMe->rcBmp, 0, 0, pMe->bmpInfo.cx, pMe->bmpInfo.cy);
         }

         // determine an appropriate background color for this temporary glyph bitmap
         // white is best, unless the outline color is white as well ...
         if (glyphBG == outlineColor) {
            glyphBG = IBITMAP_RGBToNative(pDst, RGBA_STRIPALPHA(RGB_BLACK));
         }
         
         // draw the characters into the temporary bitmap in the outline color
         if (dwResult == SUCCESS) {    
            IBITMAP_FillRect(pMe->piBitmap, &pMe->rcBmp, glyphBG, AEE_RO_COPY);
            IBITMAP_SetTransparencyColor(pMe->piBitmap, glyphBG);
            dwResult = IFONT_DrawText(pMe->piFont, pMe->piBitmap, pMe->dwWidth + 1, pMe->dwWidth + 1, pcText, nOriginalFits, outlineColor, glyphBG, &pMe->rcBmp, dwFlags);
         }

         //
         // now blit the characters to the destination, adding a glow as we go
         //

         if (dwResult == SUCCESS) {
         
            // first, fill the destination with the background color
            if (0==(dwFlags&IDF_TEXT_TRANSPARENT)) {
               IBITMAP_FillRect(pDst, &rcIntersection, bg, AEE_RO_COPY);
            }

            // blit the outline four times to make a single pixel outline ...
            dwSourceX = rcIntersection.x - rcDestGlyph.x + 1;
            dwSourceY = rcIntersection.y - rcDestGlyph.y + 1;

            // blit the text drawn in the outline color enough times around
            // destination to produce a 1 or 2 pixel outline.
            if (pMe->dwWidth == 1) {

               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX+1, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY+1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX-1, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY-1, AEE_RO_TRANSPARENT);

            } else if (pMe->dwWidth == 2) {

               // first pixel
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX+1, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX+1, dwSourceY+1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY+1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX-1, dwSourceY+1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX-1, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX-1, dwSourceY-1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY-1, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX+1, dwSourceY-1, AEE_RO_TRANSPARENT);

               // second pixel
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX+2, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY+2, AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX-2, dwSourceY,   AEE_RO_TRANSPARENT);
               IBITMAP_BltIn(pDst, rcIntersection.x, rcIntersection.y, rcIntersection.dx, rcIntersection.dy, pMe->piBitmap, dwSourceX,   dwSourceY-2, AEE_RO_TRANSPARENT);
            }

            // now draw the text ...
            IFONT_DrawText(pMe->piFont, pDst, pMe->dwWidth + x, pMe->dwWidth + y, pcText, nOriginalFits, fg, glyphBG, prcClip, IDF_TEXT_TRANSPARENT);
         }
      }  

      // invalidate
      if (!prcClip) {
         SETAEERECT(&rcDestGlyph, x, y, rcIntersection.dx, fi.nAscent + fi.nDescent + 2*pMe->dwWidth);
         prcClip = &rcDestGlyph;
      }
      IBITMAP_Invalidate(pDst, prcClip);
   }

   return dwResult;
}

/* -----------------------------------------------------------------------
    HFont_DrawNPixelOutline - draws text with an n-pixel outline            
   --- */
static int HFont_DrawNPixelOutline(HFont *pMe, IBitmap *pDst, IDIB *pDstDIB, PFNBLITWITHOUTLINE pfnBlit, int x, int y, 
                             const AECHAR *pcText, int nChars, NativeColor fg, NativeColor bg, const AEERect *prcClip, 
                             uint32 dwFlags)
{
   int dwResult = SUCCESS; 
   AEEFontInfo fi;
   int nOriginalFits, nOriginalPixels;
   AEERect rcLine, rcDestGlyph, rcIntersection;   
   NativeColor outlineColor = IBITMAP_RGBToNative(pDst, RGBA_STRIPALPHA(pMe->rgb));
   IDIB *psdib = NULL;
   NativeColor glyphBG = IBITMAP_RGBToNative(pDst, RGBA_STRIPALPHA(RGB_WHITE));

   // how tall are these characters?
   dwResult = IFONT_GetInfo(pMe->piFont, &fi, sizeof(AEEFontInfo));
   if (dwResult == SUCCESS) {
               
      //
      // draw the character into a temporary bitmap
      //

      // figure out how big this line of characters is
      dwResult = IFONT_MeasureText(pMe->piFont, pcText, nChars, IFONT_MAXWIDTH, &nOriginalFits, &nOriginalPixels);
      if (dwResult == SUCCESS) {

         // determine this size of this line of characters
         SETAEERECT(&rcLine, 
                    1, 
                    1, 
                    nOriginalPixels + 2*pMe->dwWidth, 
                    fi.nAscent + fi.nDescent + 2*pMe->dwWidth);

         // determine how much of this line of characters fits on screen
         SETAEERECT(&rcDestGlyph, 
                    x, 
                    y, 
                    rcLine.dx, 
                    rcLine.dy);

         if (prcClip) {
            if (!IntersectRect(&rcIntersection, &rcDestGlyph, prcClip)) {
               // no intersection.  we're done.
               return SUCCESS;
            }
         }

         // will this line fit in our interal temp bitmap?  Note, the bitmap is going to be big 
         // enough for the entire outlined line as well as single pixel border all the way around to 
         // speed up blitting later
         if (!pMe->piBitmap || 
            ((uint32)(rcLine.dx + 2) > pMe->bmpInfo.cx) 
            || ((uint32)(rcLine.dy + 2) > pMe->bmpInfo.cy)) {

            // nope, it won't.  Create a new one.
            RELEASEIF(pMe->piBitmap);
            RELEASEIF(psdib);
            pMe->bmpInfo.cx = rcLine.dx + 2;
            pMe->bmpInfo.cy = rcLine.dy + 2;
            dwResult = IBITMAP_CreateCompatibleBitmap(pDst, &pMe->piBitmap, (uint16)pMe->bmpInfo.cx, (uint16)pMe->bmpInfo.cy);
            SETAEERECT(&pMe->rcBmp, 0, 0, pMe->bmpInfo.cx, pMe->bmpInfo.cy);
         }

         // get the source DIB
         if ((dwResult == SUCCESS) && (psdib == NULL)) {
            dwResult = IBITMAP_QueryInterface(pMe->piBitmap, AEECLSID_DIB, (void**)&psdib);

            if (dwResult == SUCCESS) {
               // determine an appropriate background color for this temporary glyph bitmap
               // white is best, unless the fg color is white as well ...
               if (glyphBG == fg) {
                  glyphBG = IBITMAP_RGBToNative(pMe->piBitmap, RGBA_STRIPALPHA(RGB_BLACK));
               }
            }
         }
         
         // draw the characters into the temporary bitmap
         if (dwResult == SUCCESS) {    
            IBITMAP_FillRect(pMe->piBitmap, &pMe->rcBmp, glyphBG, AEE_RO_COPY);
            dwResult = IFONT_DrawText(pMe->piFont, pMe->piBitmap, pMe->dwWidth + 1, pMe->dwWidth + 1, pcText, nOriginalFits, fg, glyphBG, &pMe->rcBmp, dwFlags);
         }

         //
         // now blit the characters to the destination, adding a glow as we go
         //

         if (dwResult == SUCCESS) {
         
            // first, fill the destination with the background color
            if (0==(dwFlags&IDF_TEXT_TRANSPARENT)) {
               IBITMAP_FillRect(pDst, &rcIntersection, bg, AEE_RO_COPY);
            }

            // blit the character to the screen
            pfnBlit(psdib, &rcLine, pMe->dwWidth, pDstDIB, x, y, &rcIntersection, glyphBG, outlineColor);
         }
      }

      // invalidate
      if (!prcClip) {
         SETAEERECT(&rcDestGlyph, x, y, rcIntersection.dx, fi.nAscent + fi.nDescent + 2*pMe->dwWidth);
         prcClip = &rcDestGlyph;
      }
      IBITMAP_Invalidate(pDst, prcClip);
   }

   // release
   RELEASEIF(psdib);

   return dwResult;
}

/* -----------------------------------------------------------------------
    HFont_Blit, 16 bit version
   --- */

static void HFont_Blit16(IDIB *pSrcDIB, AEERect *prcGlyph, int dwOutlineWidth, IDIB *pDestDIB, 
                         int x, int y, AEERect *prcDest, NativeColor glyphBG, NativeColor ol)
{
   int col, row, k, dwWidth;
   byte *pbySrc, *pbyDest;
   int dwDestX, dwDestY;
    
   // iterate over entire source glyph ...
   for (row = prcGlyph->y; row < prcGlyph->y+prcGlyph->dy; row++) {
      for (col = prcGlyph->x; col < prcGlyph->x+prcGlyph->dx; col++) {
      
         // for each glyph pixel ...
         pbySrc = pSrcDIB->pBmp + (row * pSrcDIB->nPitch) + (col * sizeof(uint16));
         if (*((uint16 *)(pbySrc)) != (uint16)glyphBG) {
         
            // blit the glyph pixel to the destination if it falls
            // within the clipping region
            dwDestX = x+col-1;
            dwDestY = y+row-1;
            if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
             && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
               pbyDest = pDestDIB->pBmp + dwDestY * pDestDIB->nPitch + dwDestX * sizeof(uint16);
               *((uint16 *)(pbyDest)) = *((uint16 *)(pbySrc));
            }

            // outline this pixel once per outline width
            for (dwWidth = 1; dwWidth <= dwOutlineWidth; dwWidth++) {

               // outline it if the outline doesn't step on another glyph pixel
               for (k = 0; k < dwWidth; k++) {

                  // South East
                  dwDestX = x+col-1+dwWidth-k;
                  dwDestY = y+row-1+k;
                   if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                    && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     pbySrc = pSrcDIB->pBmp + (row+k) * pSrcDIB->nPitch + (col+dwWidth-k) * sizeof(uint16);
                     if (*((uint16 *)(pbySrc)) == (uint16)glyphBG) {
                       pbyDest = pDestDIB->pBmp + dwDestY * pDestDIB->nPitch + dwDestX * sizeof(uint16);
                       *((uint16 *)(pbyDest)) = (uint16)ol;
                     }
                  }
                  
                  // South West
                  dwDestX = x+col-1-k;
                  dwDestY = y+row-1+dwWidth-k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     pbySrc = pSrcDIB->pBmp + (row+dwWidth-k) * pSrcDIB->nPitch + (col-k) * sizeof(uint16);
                     if (*((uint16 *)(pbySrc)) == (uint16)glyphBG) {
                       pbyDest = pDestDIB->pBmp + dwDestY * pDestDIB->nPitch + dwDestX * sizeof(uint16);
                       *((uint16 *)(pbyDest)) = (uint16)ol;
                     }
                  }
                 
                  // North West
                  dwDestX = x+col-1-dwWidth+k;
                  dwDestY = y+row-1-k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     pbySrc = pSrcDIB->pBmp + (row-k) * pSrcDIB->nPitch + (col-dwWidth+k) * sizeof(uint16);
                     if (*((uint16 *)(pbySrc)) == (uint16)glyphBG) {
                       pbyDest = pDestDIB->pBmp + dwDestY * pDestDIB->nPitch + dwDestX * sizeof(uint16);
                       *((uint16 *)(pbyDest)) = (uint16)ol;
                     }
                  }
        
                  // North East
                  dwDestX = x+col-1+k;
                  dwDestY = y+row-1-dwWidth+k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     pbySrc = pSrcDIB->pBmp + (row-dwWidth+k) * pSrcDIB->nPitch + (col+k) * sizeof(uint16);
                     if (*((uint16 *)(pbySrc)) == (uint16)glyphBG) {
                       pbyDest = pDestDIB->pBmp + dwDestY * pDestDIB->nPitch + dwDestX * sizeof(uint16);
                       *((uint16 *)(pbyDest)) = (uint16)ol;
                     }
                  }
               }
            }
         }
      }
   }
}

/* -----------------------------------------------------------------------
    HFont_BlitOther, GetPixel/SetPixel version.  
    Slow, but works for all bit depths
   --- */
static void HFont_BlitOther(IDIB *pSrcDIB, AEERect *prcGlyph, int dwOutlineWidth, IDIB *pDestDIB, 
                            int x, int y,  AEERect *prcDest, NativeColor glyphBG, NativeColor ol)
{
   int col, row, k, dwWidth;
   NativeColor srcPixel;
   int dwDestX, dwDestY;

   // iterate over entire source glyph ...
   for (row = prcGlyph->y; row < prcGlyph->y+prcGlyph->dy; row++) {
      for (col = prcGlyph->x; col < prcGlyph->x+prcGlyph->dx; col++) {
     
          // for each glyph pixel ...
         IBITMAP_GetPixel(IDIB_TO_IBITMAP(pSrcDIB), col, row, &srcPixel);
         if (srcPixel != glyphBG) {
         
            // blit the glyph pixel to the destination if it falls
            // within the clipping region
            dwDestX = x+col-1;
            dwDestY = y+row-1;
            if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
             && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
               IBITMAP_DrawPixel(IDIB_TO_IBITMAP(pDestDIB), dwDestX, dwDestY, srcPixel, AEE_RO_COPY);
            }

            // outline this pixel once per outline width
            for (dwWidth = 1; dwWidth <= dwOutlineWidth; dwWidth++) {

               // outline it if the outline doesn't step on another glyph pixel
               for (k = 0; k < dwWidth; k++) {

                  // South East
                  dwDestX = x+col-1+dwWidth-k;
                  dwDestY = y+row-1+k;
                   if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                    && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     IBITMAP_GetPixel(IDIB_TO_IBITMAP(pSrcDIB), col+dwWidth-k, row+k, &srcPixel);
                     if (srcPixel == glyphBG) {
                       IBITMAP_DrawPixel(IDIB_TO_IBITMAP(pDestDIB), dwDestX, dwDestY, ol, AEE_RO_COPY);
                     }
                  }
                  
                  // South West
                  dwDestX = x+col-1-k;
                  dwDestY = y+row-1+dwWidth-k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     IBITMAP_GetPixel(IDIB_TO_IBITMAP(pSrcDIB), col-k, row+dwWidth-k, &srcPixel);
                     if (srcPixel == glyphBG) {
                       IBITMAP_DrawPixel(IDIB_TO_IBITMAP(pDestDIB), dwDestX, dwDestY, ol, AEE_RO_COPY);
                     }
                  }
                
                  // North West
                  dwDestX = x+col-1-dwWidth+k;
                  dwDestY = y+row-1-k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     IBITMAP_GetPixel(IDIB_TO_IBITMAP(pSrcDIB), col-dwWidth+k, row-k, &srcPixel);
                     if (srcPixel == glyphBG) {
                       IBITMAP_DrawPixel(IDIB_TO_IBITMAP(pDestDIB), dwDestX, dwDestY, ol, AEE_RO_COPY);
                     }
                  }
        
                  // North East
                  dwDestX = x+col-1+k;
                  dwDestY = y+row-1-dwWidth+k;
                  if ((dwDestX >= prcDest->x) && (dwDestX < (prcDest->x + prcDest->dx)) 
                   && (dwDestY >= prcDest->y) && (dwDestY < (prcDest->y + prcDest->dy))) {
                     IBITMAP_GetPixel(IDIB_TO_IBITMAP(pSrcDIB), col+k, row-dwWidth+k, &srcPixel);
                     if (srcPixel == glyphBG) {
                       IBITMAP_DrawPixel(IDIB_TO_IBITMAP(pDestDIB), dwDestX, dwDestY, ol, AEE_RO_COPY);
                     }
                  }
               }
            }
         }
      }
   }
}

