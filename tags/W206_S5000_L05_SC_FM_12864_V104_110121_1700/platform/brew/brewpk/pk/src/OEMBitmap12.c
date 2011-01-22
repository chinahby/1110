/*=====================================================================
FILE:  OEMBitmap12.c

SERVICES:  OEMBitmap reference implementation

DESCRIPTION:
   Reference IBitmap implementation for 12-bit display.  This file
   defines the macros required for the bit depth independent
   implementation in OEMBitmap_generic.h.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/
#include "AEEStdLib.h"
#include "OEMStdLib.h"
#include "AEE_OEM.h"
#include "OEMBitmapFuncs_priv.h"
#include "OEMBitmap_priv.h"

static const uint8 a8to4[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                              0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
                              0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
                              0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
                              0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
                              0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 
                              0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
                              0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 
                              0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
                              0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
                              0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
                              0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 
                              0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 
                              0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 
                              0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
                              0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 
                              0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
                              0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
                              0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
                              0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
                              0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 
                              0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 
                              0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 
                              0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 
                              0x0B, 0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 
                              0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 
                              0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 
                              0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 
                              0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 
                              0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 
                              0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 
                              0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};

static const uint8 a4to8[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
                              0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// The reference implementation for IBitmap is in OEMBitmap_generic.h,
// which is included from this file.  The macros defined in this file
// are used by OEMBitmap_generic.h.  Customizing this file for your
// implementation may be done in several steps:

// STEP 1:
// These macros must be defined correctly for a functional implementation.
// This implementation will be functional, but performance will be slow.

#define COLOR_DEPTH  12 // number of significant bits per pixel, returned in AEEBitmapInfo struct
#define PIXEL_SIZE   16 // total number of bits per pixel, including padding (DIB-compatible bitmaps only)
#define COLOR_SCHEME IDIB_COLORSCHEME_444  // DIB color scheme (DIB-compatible bitmaps only)

#define PITCH(w)     ((((w) + 1) / 2) * 4)

// This allows QueryInterface to work for IDIB.  If your bitmap format cannot
// be represented as a DIB, do not define this.
#define DIB_COMPATIBLE

// Used to contruct function names. For example, OEMBitmap16_New.
#if !defined(FUNCTION_NAME_ID)
#define FUNCTION_NAME_ID 12
#endif

#define SETPIXEL(b, x, y, c) \
   (*(uint16*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 2) = (uint16)((c) & 0xFFFF))

#define GETPIXEL(b, x, y) \
   ((NativeColor)(*(uint16*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 2)))

#define RGB_TO_NATIVE(r, g, b) \
   ((NativeColor)((a8to4[(r)] << 8) | (a8to4[(g)] << 4) | a8to4[(b)]))

#define NATIVE_TO_RGB(c) \
   ((RGBVAL)((a4to8[(c) >> 8] << 8) | (a4to8[((c) >> 4) & 0xF] << 16) | (a4to8[(c) & 0xF] << 24)))


//---- Blending macros: See OEMBitmap8.c for more info.
//

#define BITSPREAD_MASK                  0x0F0F0FU
#define BITSPREAD_BIAS                  0x080808U
#define BITSPREAD_EXTRABITS             4
#define BITSPREAD_SCALEALPHA(a)         (((a) + 8) >> 4)
#define BITSPREAD_FROMNATIVE_MASK(nc,m) (((nc) | (nc) << 12) & (m))
#define BITSPREAD_TONATIVE_MASK(u,m)    ((((u) & (m)) | ((u) & (m)) >> 12))
#define BITSPREAD_FROMRGB(r,g,b)        ((NativeColor)((a8to4[(r)] << 8) | (a8to4[(g)] << 16) | a8to4[(b)]))

#define BITSPREAD_FROMNATIVE(nc)        BITSPREAD_FROMNATIVE_MASK((nc),BITSPREAD_MASK)
#define BITSPREAD_TONATIVE(nc)          BITSPREAD_TONATIVE_MASK((nc),BITSPREAD_MASK)


// STEP 2:
// For improved performance, the following macros should be defined.
#ifndef BITMAP_DISABLE_OPT_STEP2
# define PIXELITER_SETUP(b,x,y,fg,bg,bRevY)                                                \
    int32  pitch_ = (bRevY) ? -((b)->m_dib.nPitch / 2) : (b)->m_dib.nPitch / 2;            \
    uint32 bg_ = (bg);                                                                     \
    uint32 fg_ = (fg);                                                                     \
    register uint16 *pw_ = (uint16*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 2); \
    uint16 *pwBase_ = pw_

   // To select between two color values, we use "b; if (cond) a;" instead of "if
   // (cond) a; else b;" because the generated code is smaller.  Although both
   // versions could be optimized by the compiler to be equally fast, the larger
   // version has more branches and ends up interfering with the compiler's
   // register allocation of variables.
# define PIXELITER_BIT(bOn)     { register uint32 v = bg_; if (bOn) v=fg_; *pw_ = (uint16)v; }

   // set pixel to fg
# define PIXELITER_SETFG()      (*pw_ = (uint16)fg_)

   // fg_ = fg_; bg_ = bg_; to get rid of warnings - optimized out by compiler
# define PIXELITER_NEXTX()      { pw_++; }

# define PIXELITER_PREVX()      { --pw_; }

# define PIXELITER_NEXTY()      { pw_ = (pwBase_ += pitch_); fg_ = fg_; bg_ = bg_; }

# define PIXELITER_SET(val)     (*pw_ = (uint16)(val))

# define PIXELITER_GET()        (*pw_)

# define PIXELITER_SRC_SETUP(b,x,y,bRevY)                                                     \
    int32  pitchSrc_ = (bRevY) ? -((b)->m_dib.nPitch / 2) : (b)->m_dib.nPitch / 2;            \
    register uint16 *pwSrc_ = (uint16*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 2); \
    uint16 *pwSrcBase_ = pwSrc_

# define PIXELITER_SRC_NEXTX()  (pwSrc_ ++)

# define PIXELITER_SRC_NEXTY()  (pwSrc_ = (pwSrcBase_ += pitchSrc_))

# define PIXELITER_SRC_GET()    (*pwSrc_)

#endif //BITMAP_DISABLE_OPT_STEP2



// STEP 3:
// For bitmap formats with whole-byte pixels (such as 8) the following macros
// provide more optimization.
#ifndef BITMAP_DISABLE_OPT_STEP3
# define PIXELITER_SETAT(i,val) (pw_[i] = (uint16)(val))
# define PIXELITER_GETAT(i)     (pw_[i])
# define PIXELITER_ADDX(dx)     (pw_ += (dx))
# define PIXELITER_SRC_GETAT(i) (pwSrc_[i])
# define PIXELITER_SRC_ADDX(dx) (pwSrc_ += (dx))
#endif //BITMAP_DISABLE_OPT_STEP3

// STEP 4:
// Finally, the following functions should be implementated to provide optimal
// preformance for important cases.
#ifndef BITMAP_DISABLE_OPT_STEP4

#if 0
#define FILLRECT_COPY FillRect_Copy

// Assume all parameters are valid and in bounds.
static __inline void FillRect_Copy(OEMBitmap *pbmDst, int x, int y, int dx, int dy, NativeColor color)
{
}
#endif


#if 0
#define FILLRECT_XOR FillRect_XOR

// Assume all parameters are valid and in bounds.
static __inline void FillRect_XOR(OEMBitmap *pbmDst, int x, int y, int dx, int dy, NativeColor color)
{
}
#endif


#define BLITDDB_COPY BlitDdb_Copy

// Assume all parameters are valid and in bounds.
static __inline void BlitDdb_Copy(OEMBitmap *pbmDst, OEMBitmap *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
{
   byte *pbDst = pbmDst->m_dib.pBmp + pbmDst->m_dib.nPitch * yDst + xDst * 2;
   int nPitchDst = pbmDst->m_dib.nPitch;
   byte *pbSrc = pbmSrc->m_dib.pBmp + pbmSrc->m_dib.nPitch * ySrc + xSrc * 2;
   int nPitchSrc = pbmSrc->m_dib.nPitch;
   int dxBytes = dx * 2;

   if (pbmSrc == pbmDst && ySrc < yDst && yDst - ySrc < dy) {
      // overlap in the Y direction, reverse Y order to fix
      pbDst += nPitchDst * (dy - 1);
      pbSrc += nPitchSrc * (dy - 1);
      nPitchDst = -nPitchDst;
      nPitchSrc = -nPitchSrc;
   }

   while (dy--) {
      OEM_memmove(pbDst, pbSrc, dxBytes);
      pbDst += nPitchDst;
      pbSrc += nPitchSrc;
   }
}


#if 0
#define BLITDDB_TRANSPARENT BlitDdb_Transparent

// Assume all parameters are valid and in bounds.
static __inline void BlitDdb_Transparent(OEMBitmap *pbmDst, OEMBitmap *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
{
}
#endif


#if 0
#define BLITDDB_XOR BlitDdb_XOR

// Assume all parameters are valid and in bounds.
static __inline void BlitDdb_XOR(OEMBitmap *pbmDst, OEMBitmap *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
{
}
#endif


#if 0
#define TRANSFORM_COMPLEX TransformComplex

static __inline int TransformComplex(OEMBitmap *pMe, int xDst, int yDst, IBitmap *pSrc, int xSrc, int ySrc,
                                     unsigned dxSrc, unsigned dySrc,
                                     const AEETransformMatrix *pMatrixTransform, uint8 unComposite)
{
}
#endif


#define TRANSFORM_SIMPLE TransformSimple

static __inline int TransformSimple(OEMBitmap *pMe, int xDst, int yDst, IBitmap *pbmSrc, int xSrc, int ySrc,
                                    unsigned dxSrc, unsigned dySrc, uint16 unTransform, uint8 unComposite)
{
   IDIB *pdibSrc = NULL;
   int   nResult = SUCCESS;
   
   if (unTransform == 0) {
      // no transformation, call standard bitblt routine because it
      // should be faster
      IBitmap *pibMe = OEMBITMAP_TO_IBITMAP(pMe);
      return IBITMAP_BltIn(pibMe, xDst, yDst, dxSrc, dySrc, pbmSrc, xSrc, ySrc,
                           unComposite == COMPOSITE_KEYCOLOR ? AEE_RO_TRANSPARENT : AEE_RO_COPY);
   }

   if (IBITMAP_QueryInterface(pbmSrc, AEECLSID_DIB, (void**)&pdibSrc) != SUCCESS &&
       IBITMAP_QueryInterface(pbmSrc, AEECLSID_DIB_20, (void**)&pdibSrc) != SUCCESS) {
      return EUNSUPPORTED;
   }

   if (pdibSrc->nDepth == PIXEL_SIZE && pdibSrc->nColorScheme == COLOR_SCHEME) {
      int dx = dxSrc;
      int dy = dySrc;

      int i, j;
      int nDstPitch = pMe->m_dib.nPitch / 2;
      int nSrcPitch = pdibSrc->nPitch / 2;
      int nSrcDownAdjust = nSrcPitch;
      int nSrcRightAdjust = 1;
      uint16 *pDst, *pDst2, *pSrc, *pSrc2;
      NativeColor ncTransparent = pdibSrc->ncTransparent;
      int nScaleNum = 1;
      int nScaleDen = 1;
      int xNum, yNum;
      int nTmp;

      // Clip to source
      if (xSrc < 0) {
         dx += xSrc;
         xSrc = 0;
      }
      if (ySrc < 0) {
         dy += ySrc;
         ySrc = 0;
      }
      if (xSrc + dx > pdibSrc->cx) {
         dx = pdibSrc->cx - xSrc;
      }
      if (ySrc + dy > pdibSrc->cy) {
         dy = pdibSrc->cy - ySrc;
      }

      pSrc = ((uint16*)pdibSrc->pBmp) + xSrc + ySrc * nSrcDownAdjust;

      // Here's where we take care of the rotation and reflaction
      // raster ops.  We handle these be changing the order in
      // which we iterate through the pixels in the source bitmap.

      if (unTransform & 0x04) {
         // reflect over x axis
         pSrc += nSrcDownAdjust * (dySrc - 1);
         nSrcDownAdjust = -nSrcDownAdjust;
      }

      switch (unTransform & 0x03) {
      case 0x00:
         // no rotation
         break;
      case 0x01:
         // rotate 90 degrees counter clockwise
         pSrc += dxSrc - 1;
         nSrcRightAdjust = nSrcDownAdjust;
         nSrcDownAdjust = -1;
         nTmp = (dx - dy) / 2;
         xDst += nTmp;
         yDst -= nTmp;
         nTmp = dx;
         dx = dy;
         dy = dxSrc;
         break;
      case 0x02:
         // rotate 180 degrees counter clockwise
         pSrc += nSrcDownAdjust * (dySrc - 1) + dxSrc - 1;
         nSrcRightAdjust = -1;
         nSrcDownAdjust = -nSrcDownAdjust;
         break;
      case 0x03:
         // rotate 270 degrees counter clockwise
         pSrc += nSrcDownAdjust * (dySrc - 1);
         nSrcRightAdjust = -nSrcDownAdjust;
         nSrcDownAdjust = 1;
         nTmp = (dx - dy) / 2;
         xDst += nTmp;
         yDst -= nTmp;
         nTmp = dx;
         dx = dy;
         dy = dxSrc;
         break;
      }

      // Here's where we take care of the scaling transformations.
      // This involves adjusting the destination rectangle, and setting
      // a scaling numerator and denominator, which we use to figure out
      // how many pixels to skip in each iteration for the destination
      // and source bitmaps.
      
      switch (unTransform & 0x38) {
      case 0x00:
         // no scaling
         break;
      case 0x08:
         // 2X
         nScaleNum = 2;
         xDst -= dx / 2;
         yDst -= dy / 2;
         dy <<= 1;
         dx <<= 1;
         break;
      case 0x10:
         // 4X
         nScaleNum = 4;
         xDst -= (dx / 2) * 3;
         yDst -= (dy / 2) * 3;
         dy <<= 2;
         dx <<= 2;
         break;
      case 0x18:
         // 8X
         nScaleNum = 8;
         xDst -= (dx / 2) * 7;
         yDst -= (dy / 2) * 7;
         dy <<= 3;
         dx <<= 3;
         break;
      case 0x28:
         // 1/8
         nScaleDen = 8;
         xDst += (dx * 7) >> 4;
         yDst += (dy * 7) >> 4;
         dy >>= 3;
         dx >>= 3;
         break;
      case 0x30:
         // 1/4
         nScaleDen = 4;
         xDst += (dx * 3) >> 3;
         yDst += (dy * 3) >> 3;
         dy >>= 2;
         dx >>= 2;
         break;
      case 0x38:
         // 1/2
         nScaleDen = 2;
         xDst += dx >> 2;
         yDst += dy >> 2;
         dy >>= 1;
         dx >>= 1;
         break;
      }

      // Now that we know the destination coordinates, we calculate where
      // to start in the destination bitmap.
      pDst = ((uint16*)pMe->m_dib.pBmp) + xDst + yDst * nDstPitch;

      // Clip to destination
      if (IS_OUTSIDE_DIRTY_RECT(pMe, xDst, yDst, dx, dy)) {
         if (xDst < 0) {
            pDst -= xDst;
            pSrc += (((-xDst) * nScaleDen) / nScaleNum) * nSrcRightAdjust;
            dx += xDst;
            xDst = 0;
         }
         if (yDst < 0) {
            pDst -= yDst * nDstPitch;
            pSrc += (((-yDst) * nScaleDen) / nScaleNum) * nSrcDownAdjust;
            dy += yDst;
            yDst = 0;
         }
         if (xDst + dx > pMe->m_dib.cx) {
            dx = pMe->m_dib.cx - xDst;
         }
         if (yDst + dy > pMe->m_dib.cy) {
            dy = pMe->m_dib.cy - yDst;
         }

         if (0 >= dx || 0 >= dy) {
            goto Done;
         }
         EXPAND_DIRTY_RECT(pMe, xDst, yDst, dx, dy);
      } else if (0 >= dx || 0 >= dy) {
         goto Done;
      }

      pDst2 = pDst;
      pSrc2 = pSrc;

      nSrcRightAdjust *= nScaleDen;
      nSrcDownAdjust *= nScaleDen;
      
      yNum = 0;

      // This is where the pixels are actually copied

      if (unComposite != COMPOSITE_KEYCOLOR) {

         // Opaque blit.  Don't have to worry about key color.

         // Iterate through destination pixels
         for (j = dy - 1; j >= 0; j--) {
            xNum = 0;
            for (i = dx - 1; i >= 0; i--) {
               *(pDst2++) = *pSrc2;
               xNum++;
               if (xNum == nScaleNum) {
                  xNum -= nScaleNum;
                  pSrc2 += nSrcRightAdjust;
               }
            }
            pDst2 = pDst += nDstPitch;
            yNum++;
            if (yNum == nScaleNum) {
               yNum -= nScaleNum;
               pSrc += nSrcDownAdjust;
            }
            pSrc2 = pSrc;
         }

      } else {

         // Transparent blit.  Must compare each pixel in source
         // to key color.

         // Iterate through destination pixels
         for (j = dy - 1; j >= 0; j--) {
            xNum = 0;
            for (i = dx - 1; i >= 0; i--) {
               uint16 pixel = *pSrc2;
               if (pixel != ncTransparent) {
                  *pDst2 = pixel;
               }
               pDst2++;
               xNum++;
               if (xNum == nScaleNum) {
                  xNum -= nScaleNum;
                  pSrc2 += nSrcRightAdjust;
               }
            }
            pDst2 = pDst += nDstPitch;
            yNum++;
            if (yNum == nScaleNum) {
               yNum -= nScaleNum;
               pSrc += nSrcDownAdjust;
            }
            pSrc2 = pSrc;
         }

      }

      goto Done;
   }

   nResult = EUNSUPPORTED;

Done:
   if ((IDIB*)0 != pdibSrc) {
      IDIB_Release(pdibSrc);
   }

   return nResult;
}


#endif //BITMAP_DISABLE_OPT_STEP4

// Include the generic implementation that will use the above definitions
#include "OEMBitmap_generic.h"
