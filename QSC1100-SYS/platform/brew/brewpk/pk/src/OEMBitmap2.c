/*=====================================================================
FILE:  OEMBitmap2.c

SERVICES:  OEMBitmap reference implementation

DESCRIPTION:
   Reference IBitmap implementation for 2-bit display.  This file
   defines the macros required for the bit depth independent
   implementation in OEMBitmap_generic.h.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "OEMBitmapFuncs_priv.h"
#include "OEMBitmap_priv.h"

// The reference implementation for IBitmap is in OEMBitmap_generic.h,
// which is included from this file.  The macros defined in this file
// are used by OEMBitmap_generic.h.  Customizing this file for your
// implementation may be done in several steps:

// STEP 1:
// These macros must be defined correctly for a functional implementation.
// This implementation will be functional, but performance will be slow.

#define COLOR_DEPTH  2  // number of significant bits per pixel, returned in AEEBitmapInfo struct
#define PIXEL_SIZE   2  // total number of bits per pixel, including padding (DIB-compatible bitmaps only)
#define COLOR_SCHEME 0  // DIB color scheme (DIB-compatible bitmaps only)

#define PITCH(w)     ((((w) + 15) / 16) * 4)

// This allows QueryInterface to work for IDIB.  If your bitmap format cannot
// be represented as a DIB, do not define this.
#define DIB_COMPATIBLE

// Used to contruct function names. For example, OEMBitmap2_New.
#if !defined(FUNCTION_NAME_ID)
#define FUNCTION_NAME_ID 2
#endif

#define SETPIXEL(b, x, y, c) { \
   byte *pd = (b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) / 4; \
   *pd = (*pd & ~((uint8)(0xC0 >> ((x) % 4 * 2))) | ((uint8)(((c) & 3) << ((3 - ((x) % 4)) * 2)))); \
}

#define GETPIXEL(b, x, y) \
   ((NativeColor)((*((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) / 4) >> ((3 - ((x) % 4)) * 2)) & 3))

// This macro is only used by RGB_TO_NATIVE
#define LUMINANCE(r,g,b)  ((r)*77 + (g)*151 + (b)*28)

#define RGB_TO_NATIVE(r, g, b) \
   (LUMINANCE((r), (g), (b)) >> 14)

#define NATIVE_TO_RGB(c) \
   (0x55555500 * (int)((c) & 3))



// STEP 2:
// For improved performance, the following macros should be defined.
#ifndef BITMAP_DISABLE_OPT_STEP2
# define PIXELITER_SETUP(b,x,y,fg,bg,bRevY)                                     \
    int nShift_ = (3 - ((x)&3)) << 1;                                           \
    register uint32 bg_ = (uint32) ((bg) & 3) << nShift_;                       \
    register uint32 fg_ = (uint32) (bg_ ^ ((fg) << nShift_));                   \
    uint32 bgBase_ = bg_;                                                       \
    uint32 fgBase_ = fg_;                                                       \
    register uint32 msk_ =  ~((uint32)3 << nShift_);                            \
    uint32 mskBase_ = msk_;                                                     \
    int32  pitch_ = (bRevY) ? -(b)->m_dib.nPitch : (b)->m_dib.nPitch;           \
    register uint8 *pby_ = (b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) / 4; \
    uint8 *pbyBase_ = pby_

   // To select between two color values, we use "b; if (cond) a;" instead of "if
   // (cond) a; else b;" because the generated code is smaller.  Although both
   // versions could be optimized by the compiler to be equally fast, the larger
   // version has more branches and ends up interfering with the compiler's
   // register allocation of variables.
# define PIXELITER_BIT(bOn)     { register uint32 v = (uint32)(*pby_&msk_|bg_); if (bOn) v^=fg_; *pby_ = (uint8)v; }

   // set pixel to fg
# define PIXELITER_SETFG()      (*pby_ = (uint8)(*pby_ & msk_ | fg_))

# define PIXELITER_SET(val)     { if ((val) & 1) *pby_ = (uint8)(*pby_ | (~msk_ & 0x55555555)); else *pby_ = (uint8)(*pby_ & (msk_ | 0xAAAAAAAA)); \
                                  if ((val) & 2) *pby_ = (uint8)(*pby_ | (~msk_ & 0xAAAAAAAA)); else *pby_ = (uint8)(*pby_ & (msk_ | 0x55555555)); }

# define PIXELITER_GET()        (((*pby_ & ~msk_ & 0xAAAAAAAA) ? 0x2 : 0x0) | \
                                 ((*pby_ & ~msk_ & 0x55555555) ? 0x1 : 0x0))

# define PIXELITER_NEXTX()      { if (msk_ & 3) {              \
                                   msk_ >>=  2;                \
                                   fg_ >>= 2;                  \
                                   bg_ >>= 2;                  \
                                 } else {                      \
                                   msk_ = 0xFFFFFF3F;          \
                                   fg_ <<= 6;                  \
                                   bg_ <<= 6;                  \
                                   pby_ ++;                    \
                                 } }

# define PIXELITER_NEXTY()      {pby_ = (pbyBase_ += pitch_); msk_ = mskBase_; bg_ = bgBase_; fg_ = fgBase_;}

# define PIXELITER_SRC_SETUP(b,x,y,bRevY)                                          \
    register uint32 mskSrc_ = ((uint32)0xC0 >> ((x & 3) << 1));                    \
    uint32 mskSrcBase_ = mskSrc_;                                                  \
    int32  pitchSrc_ = (bRevY) ? -(b)->m_dib.nPitch : (b)->m_dib.nPitch;           \
    register uint8 *pbySrc_ = (b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) / 4; \
    uint8 *pbySrcBase_ = pbySrc_

# define PIXELITER_SRC_NEXTX()  { if (!(mskSrc_ & 1)) {        \
                                   mskSrc_ >>=  2;             \
                                 } else {                      \
                                   mskSrc_ = 0xC0;             \
                                   pbySrc_ ++;                 \
                                 } }

# define PIXELITER_SRC_NEXTY()  {pbySrc_ = (pbySrcBase_ += pitchSrc_); mskSrc_ = mskSrcBase_;}

# define PIXELITER_SRC_GET()    (((*pbySrc_ & mskSrc_ & 0xAAAAAAAA) ? 0x2 : 0x0) | \
                                 ((*pbySrc_ & mskSrc_ & 0x55555555) ? 0x1 : 0x0))
#endif //BITMAP_DISABLE_OPT_STEP2



// STEP 3:
// For bitmap formats with whole-byte pixels (such as 8) the following macros
// provide more optimization.
//#ifndef BITMAP_DISABLE_OPT_STEP3
#if 0
# define PIXELITER_SETAT(i,val)
# define PIXELITER_GETAT(i)
# define PIXELITER_ADDX(dx)
# define PIXELITER_SRC_GETAT(i)
# define PIXELITER_SRC_ADDX(dx)
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


#if 0
#define BLITDDB_COPY BlitDdb_Copy

// Assume all parameters are valid and in bounds.
static __inline void BlitDdb_Copy(OEMBitmap *pbmDst, OEMBitmap *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
{
}
#endif


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


#if 0
#define TRANSFORM_SIMPLE TransformSimple

static __inline int TransformSimple(OEMBitmap *pMe, int xDst, int yDst, IBitmap *pSrc, int xSrc, int ySrc,
                                    unsigned dxSrc, unsigned dySrc, uint16 unTransform, uint8 unComposite)
{
}
#endif


#endif //BITMAP_DISABLE_OPT_STEP4

// Include the generic implementation that will use the above definitions
#include "OEMBitmap_generic.h"
