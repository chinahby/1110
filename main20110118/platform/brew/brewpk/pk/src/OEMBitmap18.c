/*=====================================================================
FILE:  OEMBitmap18.c

SERVICES:  OEMBitmap reference implementation

DESCRIPTION:
   Reference IBitmap implementation for 18-bit display.  This file
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

static const uint8 a8to6[] = {0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 
                              0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 
                              0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 
                              0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x08, 
                              0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x0A, 
                              0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0C, 
                              0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 
                              0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 
                              0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 
                              0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x14, 
                              0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15, 
                              0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 
                              0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 
                              0x1A, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B, 0x1B, 0x1B, 
                              0x1C, 0x1C, 0x1C, 0x1C, 0x1D, 0x1D, 0x1D, 0x1D, 
                              0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 
                              0x20, 0x20, 0x20, 0x20, 0x21, 0x21, 0x21, 0x21, 
                              0x22, 0x22, 0x22, 0x22, 0x23, 0x23, 0x23, 0x23, 
                              0x24, 0x24, 0x24, 0x24, 0x25, 0x25, 0x25, 0x25, 
                              0x26, 0x26, 0x26, 0x26, 0x27, 0x27, 0x27, 0x27, 
                              0x28, 0x28, 0x28, 0x28, 0x29, 0x29, 0x29, 0x29, 
                              0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2B, 0x2B, 0x2B, 
                              0x2B, 0x2C, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 
                              0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 
                              0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 
                              0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 
                              0x33, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 
                              0x35, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 
                              0x37, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 
                              0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 
                              0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3D, 0x3D, 0x3D, 
                              0x3D, 0x3E, 0x3E, 0x3E, 0x3E, 0x3F, 0x3F, 0x3F};

static const uint8 a6to8[] = {0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
                              0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C,
                              0x40, 0x44, 0x48, 0x4C, 0x50, 0x55, 0x59, 0x5D,
                              0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
                              0x81, 0x85, 0x89, 0x8D, 0x91, 0x95, 0x99, 0x9D,
                              0xA1, 0xA5, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
                              0xC2, 0xC6, 0xCA, 0xCE, 0xD2, 0xD6, 0xDA, 0xDE,
                              0xE2, 0xE6, 0xEA, 0xEE, 0xF2, 0xF6, 0xFA, 0xFF};

// The reference implementation for IBitmap is in OEMBitmap_generic.h,
// which is included from this file.  The macros defined in this file
// are used by OEMBitmap_generic.h.  Customizing this file for your
// implementation may be done in several steps:

// STEP 1:
// These macros must be defined correctly for a functional implementation.
// This implementation will be functional, but performance will be slow.

#define COLOR_DEPTH  18 // number of significant bits per pixel, returned in AEEBitmapInfo struct
#define PIXEL_SIZE   32 // total number of bits per pixel, including padding (DIB-compatible bitmaps only)
#define COLOR_SCHEME IDIB_COLORSCHEME_666  // DIB color scheme (DIB-compatible bitmaps only)

#define PITCH(w)     ((w) * 4)

// This allows QueryInterface to work for IDIB.  If your bitmap format cannot
// be represented as a DIB, do not define this.
#define DIB_COMPATIBLE

// Used to contruct function names. For example, OEMBitmap16_New.
#if !defined(FUNCTION_NAME_ID)
#define FUNCTION_NAME_ID 18
#endif

#define SETPIXEL(b, x, y, c) \
   (*(uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4) = (c))

#define GETPIXEL(b, x, y) \
   ((NativeColor)(*(uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4)))

#define RGB_TO_NATIVE(r, g, b) \
   ((NativeColor)((a8to6[(r)] << 12) | (a8to6[(g)] << 6) | a8to6[(b)]))

#define NATIVE_TO_RGB(c) \
   ((RGBVAL)((a6to8[(c) >> 12] << 8) | (a6to8[((c) >> 6) & 0x3F] << 16) | (a6to8[(c) & 0x3F] << 24)))

//---- Blending macros: See OEMBitmap8.c for more info.

// An AlphaColor holds values computed from an RGBA that will act as the
// source of a blit.  This includes:
//
//   - factor for destination pixel: This is (255 - alpha) scaled down
//   - "spreaded" and multiplied source RGB
//   - For fully opaque pixels, in includes the result value in NativeColor form.
//
// Blending then requires only spreading of the destination, multiplication,
// addition, and despreading.  In the case of fully opaque pixels (nDestAlpha
// == 0), the AlphaColor holds a pre-computed native color, and only a
// simple copy is required.
//
// Typical spreaded, pre-multiplied value looks like this (for 5-6-5 colorscheme):
//
//    gggggggggggrrrrr rrrrr0bbbbbbbbbb    (5-6-5 colorscheme)
//    000000000000000g gggggrrrrrrbbbbb    (3-3-2 colorscheme)
//


//
#define BITSPREAD_MASKLOW               0x0003f03fU
#define BITSPREAD_MASKHIGH              0x0000003fU
#define BITSPREAD_BIASLOW               0x0
#define BITSPREAD_BIASHIGH              0x0
#define BITSPREAD_EXTRABITS             6
#define BITSPREAD_SCALEALPHA(a)         (((a)+2)>>2)

#define ALPHACOLOR AlphaColor
typedef struct {
   uint32  low;
   uint32  high;
} AlphaColor;

#define ALPHACOLOR_SCALE(a, n)            a.low *= (n); a.high *= (n);
#define ALPHACOLOR_SUM(a1, a2)            a1.low += a2.low; a1.high += a2.high; 
#define ALPHACOLOR_ADDBIAS(a)             a.low += BITSPREAD_BIASLOW; a.high += BITSPREAD_BIASHIGH;
#define ALPHACOLOR_RSHIFT(a, bits)        a.low >>= bits; a.high >>=(bits); 

#define BITSPREAD_FROMNATIVE_MASK(dst, src, ml, mh) \
   dst.low =  ((src) & (ml)); \
   dst.high = ((src) >> 6 & (mh)); 

#define BITSPREAD_TONATIVE_MASK(dst, src, ml, mh) \
   dst = (((src.low) & (ml)) | ((src.high) & (mh))<<6); 

#define GEN_BITSPREAD_FROMRGB(dst, r,g,b)\
   dst.low = (a8to6[(r)]<<12 | a8to6[(b)]) & BITSPREAD_MASKLOW; \
   dst.high = (a8to6[(g)]) & BITSPREAD_MASKHIGH;

#define GEN_BITSPREAD_FROMNATIVE(dst, src)       BITSPREAD_FROMNATIVE_MASK((dst), (src),BITSPREAD_MASKLOW,BITSPREAD_MASKHIGH )
#define GEN_BITSPREAD_TONATIVE(dst, src)         BITSPREAD_TONATIVE_MASK((dst), (src), BITSPREAD_MASKLOW, BITSPREAD_MASKHIGH )


// STEP 2:
// For improved performance, the following macros should be defined.
#ifndef BITMAP_DISABLE_OPT_STEP2
# define PIXELITER_SETUP(b,x,y,fg,bg,bRevY)                                                \
    int32  pitch_ = (bRevY) ? -((b)->m_dib.nPitch / 4) : (b)->m_dib.nPitch / 4;            \
    uint32 bg_ = (bg);                                                                     \
    uint32 fg_ = (fg);                                                                     \
    register uint32 *pdw_ = (uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4);\
    uint32 *pdwBase_ = pdw_

   // To select between two color values, we use "b; if (cond) a;" instead of "if
   // (cond) a; else b;" because the generated code is smaller.  Although both
   // versions could be optimized by the compiler to be equally fast, the larger
   // version has more branches and ends up interfering with the compiler's
   // register allocation of variables.
# define PIXELITER_BIT(bOn)     { register uint32 v = bg_; if (bOn) v=fg_; *pdw_ = v; }

   // set pixel to fg
# define PIXELITER_SETFG()      (*pdw_ = fg_)

   // fg_ = fg_; bg_ = bg_; to get rid of warnings - optimized out by compiler

# define PIXELITER_PREVX()      { --pdw_; }
# define PIXELITER_NEXTX()      { pdw_++; }

# define PIXELITER_NEXTY()      { pdw_ = (pdwBase_ += pitch_); fg_ = fg_; bg_ = bg_; }

# define PIXELITER_SET(val)     (*pdw_ = (val))

# define PIXELITER_GET()        (*pdw_)

# define PIXELITER_SRC_SETUP(b,x,y,bRevY)                                                     \
    int32  pitchSrc_ = (bRevY) ? -((b)->m_dib.nPitch / 4) : (b)->m_dib.nPitch / 4;            \
    register uint32 *pdwSrc_ = (uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4);\
    uint32 *pdwSrcBase_ = pdwSrc_

# define PIXELITER_SRC_NEXTX()  (pdwSrc_ ++)

# define PIXELITER_SRC_NEXTY()  (pdwSrc_ = (pdwSrcBase_ += pitchSrc_))

# define PIXELITER_SRC_GET()    (*pdwSrc_)

#endif //BITMAP_DISABLE_OPT_STEP2



// STEP 3:
// For bitmap formats with whole-byte pixels (such as 8) the following macros
// provide more optimization.
#ifndef BITMAP_DISABLE_OPT_STEP3
# define PIXELITER_SETAT(i,val) (pdw_[i] = (val))
# define PIXELITER_GETAT(i)     (pdw_[i])
# define PIXELITER_ADDX(dx)     (pdw_ += (dx))
# define PIXELITER_SRC_GETAT(i) (pdwSrc_[i])
# define PIXELITER_SRC_ADDX(dx) (pdwSrc_ += (dx))
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
   byte *pbDst = pbmDst->m_dib.pBmp + pbmDst->m_dib.nPitch * yDst + xDst * 4;
   int nPitchDst = pbmDst->m_dib.nPitch;
   byte *pbSrc = pbmSrc->m_dib.pBmp + pbmSrc->m_dib.nPitch * ySrc + xSrc * 4;
   int nPitchSrc = pbmSrc->m_dib.nPitch;
   int dxBytes = dx * 4;

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
      int nDstPitch = pMe->m_dib.nPitch / 4;
      int nSrcPitch = pdibSrc->nPitch / 4;
      int nSrcDownAdjust = nSrcPitch;
      int nSrcRightAdjust = 1;
      uint32 *pDst, *pDst2, *pSrc, *pSrc2;
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

      pSrc = ((uint32*)pdibSrc->pBmp) + xSrc + ySrc * nSrcDownAdjust;

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
      pDst = ((uint32*)pMe->m_dib.pBmp) + xDst + yDst * nDstPitch;

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
               uint32 pixel = *pSrc2;
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
