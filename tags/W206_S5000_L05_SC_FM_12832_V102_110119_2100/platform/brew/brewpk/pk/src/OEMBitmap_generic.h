/*=====================================================================
FILE:  OEMBitmap_generic.h

SERVICES:  OEMBitmap reference implementation

DESCRIPTION:
   Bit depth independent reference IBitmap implementation.  The
   implementation in this file is built on the macros defined in
   OEMBitmapN.c, where N is the bit depth.  The file is included
   from OEMBitmapN.c.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/
#ifdef IMPLEMENT_ITRANSFORM
#include "OEMTransform_generic.h"
#endif



//
// Various macros for OEMBitmap
//

#undef ASSERT
#define ASSERT(x) if (!(x)) { DBGPRINTF("assertion failed ("__FILE__":%d): \"%s\"\n",__LINE__,#x); }

// must wrap CREATE_FUNC_NAME_EX so parameters are evaluated
#define CREATE_FUNC_NAME_EX(prefix, id, suffix) prefix##id##suffix
#define CREATE_FUNC_NAME(prefix, id, suffix) CREATE_FUNC_NAME_EX(prefix, id, suffix)

#define CLIP_TO_SRC(xDst,yDst,xSrc,ySrc,cxSrc,cySrc,cx,cy)                          \
do {                                                                                \
   if ((xSrc) < 0) {                                                                \
      (xDst) -= (xSrc);                                                             \
      (cx) += (xSrc);                                                               \
      (xSrc) = 0;                                                                   \
   }                                                                                \
   if ((ySrc) < 0) {                                                                \
      (yDst) -= (ySrc);                                                             \
      (cy) += (ySrc);                                                               \
      (ySrc) = 0;                                                                   \
   }                                                                                \
   if ((xSrc) + (cx) > (cxSrc)) {                                                   \
      (cx) = (cxSrc) - (xSrc);                                                      \
   }                                                                                \
   if ((ySrc) + (cy) > (cySrc)) {                                                   \
      (cy) = (cySrc) - (ySrc);                                                      \
   }                                                                                \
} while (0)

#define CLIP_TO_DST(xDst,yDst,cxDst,cyDst,xSrc,ySrc,cx,cy)                          \
do {                                                                                \
   if ((xDst) < 0) {                                                                \
      (xSrc) -= (xDst);                                                             \
      (cx) += (xDst);                                                               \
      (xDst) = 0;                                                                   \
   }                                                                                \
   if ((yDst) < 0) {                                                                \
      (ySrc) -= (yDst);                                                             \
      (cy) += (yDst);                                                               \
      (yDst) = 0;                                                                   \
   }                                                                                \
   if (cx > cxDst - xDst) {                                                         \
      cx = cxDst - xDst;                                                            \
   }                                                                                \
   if (cy > cyDst - yDst) {                                                         \
      cy = cyDst - yDst;                                                            \
   }                                                                                \
} while (0)

#define BLT_CLIP_AND_DIRTY(pMe,xDst,yDst,cxDst,cyDst,xSrc,ySrc,cxSrc,cySrc,cx,cy)   \
do {                                                                                \
   CLIP_TO_SRC((xDst), (yDst), (xSrc), (ySrc), (cxSrc), (cySrc), (cx), (cy));       \
   if (IS_OUTSIDE_DIRTY_RECT((pMe), (xDst), (yDst), (cx), (cy))) {                  \
      CLIP_TO_DST((xDst), (yDst), (cxDst), (cyDst), (xSrc), (ySrc), (cx), (cy));    \
      if (0 >= (cx) || 0 >= (cy)) {                                                 \
         break;                                                                     \
      }                                                                             \
      EXPAND_DIRTY_RECT((pMe), (xDst), (yDst), (cx), (cy));                         \
   }                                                                                \
} while (0)

#ifndef PIXEL_BUF_SIZE
#define PIXEL_BUF_SIZE(w,h) (PITCH(w) * (h))
#endif

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 OEMBitmap_AddRef(IBitmap *po);
static uint32 OEMBitmap_Release(IBitmap *po);
static int OEMBitmap_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew);
static NativeColor OEMBitmap_RGBToNative(IBitmap *po, RGBVAL rgb);
static RGBVAL OEMBitmap_NativeToRGB(IBitmap *po, NativeColor color);
static int OEMBitmap_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop);
static int OEMBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor);
static int OEMBitmap_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop);
static int OEMBitmap_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop);
static int OEMBitmap_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop);
static int OEMBitmap_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop);
static int OEMBitmap_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
static int OEMBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize);
static int OEMBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h);
static int OEMBitmap_SetTransparencyColor(IBitmap *po, NativeColor color);
static int OEMBitmap_GetTransparencyColor(IBitmap *po, NativeColor *pColor);

static int OEMBitmap_Update(IBitmapDev *po);
static boolean OEMBitmap_IsEnabled(IBitmapDev *po);
static int OEMBitmap_NotifyEnable(IBitmapDev *po, AEECallback *pcb);

static int OEMBitmap_GetRotation(IBitmapRot *po);

static int OEMBitmap_Enable(IBitmapCtl *po, boolean bOn);
static int OEMBitmap_Restrict(IBitmapCtl *po, AEERect *prc);
static int OEMBitmap_NotifyRelease(IBitmapCtl *po, AEECallback *pcb);
static AEEPoint OEMBitmap_AppToScreen(IBitmapCtl *po, AEEPoint p);
static AEEPoint OEMBitmap_ScreenToApp(IBitmapCtl *po, AEEPoint p);


static const VTBL(IBitmap) gOEMBitmapFuncs = {
   OEMBitmap_AddRef,
   OEMBitmap_Release,
   OEMBitmap_QueryInterface,
   OEMBitmap_RGBToNative,
   OEMBitmap_NativeToRGB,
   OEMBitmap_DrawPixel,
   OEMBitmap_GetPixel,
   OEMBitmap_SetPixels,
   OEMBitmap_DrawHScanline,
   OEMBitmap_FillRect,
   OEMBitmap_BltIn,
   OEMBitmap_BltOut,
   OEMBitmap_GetInfo,
   OEMBitmap_CreateCompatibleBitmap,
   OEMBitmap_SetTransparencyColor,
   OEMBitmap_GetTransparencyColor
};


static const VTBL(IBitmapCtl) gOEMBitmapCtlFuncs = {
   AEEBASE_AddRef(IBitmapCtl),             // AEEBase versions defer to OEMBitmap_xxx
   AEEBASE_Release(IBitmapCtl),
   AEEBASE_QueryInterface(IBitmapCtl),

   OEMBitmap_Enable,
   OEMBitmap_Restrict,
   OEMBitmap_NotifyRelease,
   OEMBitmap_AppToScreen,
   OEMBitmap_ScreenToApp
};


static const VTBL(IBitmapDev) gOEMBitmapDevFuncs = {
   AEEBASE_AddRef(IBitmapDev),            // AEEBase versions defer to OEMBitmap_xxx
   AEEBASE_Release(IBitmapDev),
   AEEBASE_QueryInterface(IBitmapDev),

   OEMBitmap_Update,
   OEMBitmap_IsEnabled,
   OEMBitmap_NotifyEnable
};


static const VTBL(IBitmapRot) gOEMBitmapRotFuncs = {
   AEEBASE_AddRef(IBitmapRot),
   AEEBASE_Release(IBitmapRot),
   AEEBASE_QueryInterface(IBitmapRot),

   OEMBitmap_GetRotation
};


// PixelType = smallest type that can hold all of a pixel

#if COLOR_DEPTH <= 8
typedef uint8  PixelType;
#elif COLOR_DEPTH <= 16
typedef uint16 PixelType;
#else
typedef uint32 PixelType;
#endif

// Palette map, for caching palette conversions.
OBJECT(PaletteMap) {
   const AEEVTBL(IQueryInterface) *pvt;
   uint32         m_uRefs;
   PixelType     *m_pColors;
   boolean        m_bDDB;
};

static uint32 PaletteMap_AddRef(IQueryInterface *po);
static uint32 PaletteMap_Release(IQueryInterface *po);
static int    PaletteMap_QueryInterface(IQueryInterface *pMe, AEECLSID clsid, void **ppNew);

static const VTBL(IQueryInterface) gPaletteMapFuncs = {
   PaletteMap_AddRef,
   PaletteMap_Release,
   PaletteMap_QueryInterface
};


int CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _New)
   (uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap)
{
   return CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _NewEx)(w, h, pBuf, NULL, NULL, ppIBitmap);
}


static void * OEMBitmap_Realloc(void * pOrig, uint32 dwSize)
{
   return(REALLOC(pOrig,dwSize));
}

int CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _NewEx)
   (uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn,
    IDisplayDev *pIDispDev, IBitmap **ppIBitmap)
{
   OEMBitmap *pMe = NULL;
   uint16 nPitch = PITCH(w);

   if (NULL == pfn) {
      pfn = OEMBitmap_Realloc;
   }

   if (NULL != pBuf) {

      pMe = pfn(NULL, sizeof(OEMBitmap));

   } else {

#ifdef FEATURE_GRAPHICS_GRP_SUPPORT
      // To support GRP rendering, bitmaps need to be 64-byte aligned
      // so 63 additional bytes of memory are allocated
      pMe = pfn(NULL, sizeof(OEMBitmap) + PIXEL_BUF_SIZE(w,h) + 63);
#else
      pMe = pfn(NULL, sizeof(OEMBitmap) + PIXEL_BUF_SIZE(w,h));
#endif      /* FEATURE_GRAPHICS_GRP_SUPPORT */

      pBuf = pMe+1;

#ifdef FEATURE_GRAPHICS_GRP_SUPPORT
      // Align pBuf to the next 64-byte address
      if ((((unsigned int)(pBuf)) & 0x3F) != 0) {
         pBuf = (void*)(((unsigned int)pBuf) + 64 - (((unsigned int)(pBuf)) & 0x3F));
      }
#endif      /* FEATURE_GRAPHICS_GRP_SUPPORT */
   }

   if ((OEMBitmap *)0 == pMe) {
      return ENOMEMORY;
   }

   // our realloc clears memory, other may not
   if (OEMBitmap_Realloc != pfn) {
      MEMSET(pMe, 0, sizeof(OEMBitmap));
   }

   /* cast to non-const */
   pMe->m_dib.pvt = (AEEVTBL(IBitmap) *)&gOEMBitmapFuncs;

   pMe->m_uRefs = 1;
   pMe->m_pfnRealloc = pfn;

   pMe->m_dib.nDepth = PIXEL_SIZE;
   pMe->m_dib.nColorScheme = COLOR_SCHEME;
   pMe->m_dib.cx = w;
   pMe->m_dib.cy = h;
   pMe->m_dib.nPitch = nPitch;
   pMe->m_dib.pBmp = (byte*)pBuf;

   pMe->m_nFullWidth = w;
   pMe->m_nFullHeight = h;

   // initially, entire bitmap is dirty
   pMe->m_xDirty = 0;
   pMe->m_yDirty = 0;
   pMe->m_x2Dirty = w;
   pMe->m_y2Dirty = h;

   // Initially, bitmap is in default position
   pMe->m_nRotation = 0;

   if (pIDispDev) {
      pMe->m_pDisplayDev = pIDispDev;
      IDISPLAYDEV_AddRef(pIDispDev);
   }

#ifdef IMPLEMENT_ITRANSFORM
   // Initialize ITransform interface
   AEEBASE_INIT(pMe, m_ITransform, &gOEMTransformFuncs);
#endif

   // Initialize IBitmapDev interface
   AEEBASE_INIT(pMe, m_IBitmapDev, &gOEMBitmapDevFuncs);

   // Initialize IBitmapRot interface
   AEEBASE_INIT(pMe, m_IBitmapRot, &gOEMBitmapRotFuncs);

   // Initialize IBitmapCtl interface
   AEEBASE_INIT(pMe, m_IBitmapCtl, &gOEMBitmapCtlFuncs);
   pMe->m_cxEffective = pMe->m_dib.cx; // initially enabled

   *ppIBitmap = OEMBITMAP_TO_IBITMAP(pMe);
   return SUCCESS;
}


int CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _New_Child)
   (IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap)
{
   int nErr;
   int nRotation;
   uint16 uWidth, uHeight;
   IBitmapDev *pIBitmapDev = NULL;
   OEMBitmap *pbmParent = NULL;

   nErr = IBITMAP_QueryInterface(pParent, AEEIID_OEMBITMAP3, (void**)&pbmParent);
   if (SUCCESS != nErr) {
      goto Error;
   }

   nErr = IBITMAP_QueryInterface(pParent, AEEIID_BITMAPDEV, (void**)&pIBitmapDev);
   if (SUCCESS != nErr) {
      goto Error;
   }

   // Use the original dimensions of the parent,
   // in case parent has been rotated
   nRotation = pbmParent->m_nRotation;
   if ((nRotation == 90) || (nRotation == 270)) {
      uWidth = pbmParent->m_dib.cy;
      uHeight = pbmParent->m_dib.cx;
   } else {
      uWidth = pbmParent->m_dib.cx;
      uHeight = pbmParent->m_dib.cy;
   }

   nErr = CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _NewEx)(
      uWidth, uHeight, pbmParent->m_dib.pBmp,
      pfn, NULL, ppIBitmap);
   if (SUCCESS != nErr) {
      goto Error;
   }

   (*(OEMBitmap**)ppIBitmap)->m_pParent = pParent;
   (*(OEMBitmap**)ppIBitmap)->m_pParentDev = pIBitmapDev;

   IBITMAP_AddRef(pParent);

   goto Done;

Error:
   if (NULL != pIBitmapDev) {
      IBITMAPDEV_Release(pIBitmapDev);
   }
Done:
   if (NULL != pbmParent) {
      IBITMAP_Release(pParent);
   }
   return nErr;
}


/************************************************************************
 ** H E L P E R   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static void CancelBitmapCallback(AEECallback *pcb)
{
   *(AEECallback**)(pcb->pCancelData) = pcb->pNext;
   if (pcb->pNext) {
      pcb->pNext->pCancelData = pcb->pCancelData;
   }
   pcb->pNext = 0;
   pcb->pfnCancel = 0;
   pcb->pCancelData = 0;
}


static __inline RopPixelSupports(AEERasterOp rop)
{
   return (rop == AEE_RO_COPY     ||
           rop == AEE_RO_XOR      ||
           rop == AEE_RO_NOT      ||
           rop == AEE_RO_OR       ||
           rop == AEE_RO_MERGENOT ||
           rop == AEE_RO_ANDNOT);
}


static __inline void RopPixel(OEMBitmap *pbmDst, unsigned x, unsigned y, NativeColor color, AEERasterOp rop)
{
   switch (rop) {
   case AEE_RO_COPY:
      SETPIXEL(pbmDst, x, y, color);
      break;
   case AEE_RO_XOR:
      SETPIXEL(pbmDst, x, y, GETPIXEL(pbmDst, x, y) ^ color);
      break;
   case AEE_RO_NOT:
      SETPIXEL(pbmDst, x, y, ~color);
      break;
   case AEE_RO_OR:
      SETPIXEL(pbmDst, x, y, GETPIXEL(pbmDst, x, y) | color);
      break;
   case AEE_RO_MERGENOT:
      SETPIXEL(pbmDst, x, y, GETPIXEL(pbmDst, x, y) | ~color);
      break;
   case AEE_RO_ANDNOT:
      SETPIXEL(pbmDst, x, y, GETPIXEL(pbmDst, x, y) & ~color);
      break;
   }
}


#ifndef PIXELITER_SETUP

// If no optimized versions are supplied, we define simple versions in terms
// of SETPIXEL/GETPIXEL for the blending functions below.

# define PIXELITER_SETUP(b,x,y,fg,bg,bRevY)                                                \
    int32  pidy_ = (bRevY) ? -1 : 1;                                                       \
    int32  xbase_ = x;                                                                     \
    int32  pix_ = x;                                                                       \
    int32  piy_ = y;                                                                       \
    OEMBitmap *pbmp_ = b;                                                                  \
    uint32 bg_ = (bg);                                                                     \
    uint32 fg_ = (fg);

# define PIXELITER_NEXTX()      { ++pix_; fg_ = fg_; bg_ = bg_; }

# define PIXELITER_NEXTY()      { piy_ += pidy_; pix_ = xbase_; fg_ = fg_; bg_ = bg_;}

# define PIXELITER_SET(val)     SETPIXEL(pbmp_, pix_, piy_, val)

# define PIXELITER_GET()        GETPIXEL(pbmp_, pix_, piy_)

#endif



//--------  ALPHA BLENDING
//
// These routines depend on BITSPREAD_... macros being defined appropriately
// for the bit-depth.
//

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


#if defined(BITSPREAD_EXTRABITS)

#ifndef ALPHACOLOR 
#define ALPHACOLOR AlphaColor
typedef uint32 AlphaColor;
#define ALPHACOLOR_SCALE(a,n)       (a) *= (n);
#define ALPHACOLOR_SUM(a,b)         (a) += (b);
#define ALPHACOLOR_ADDBIAS(a)       (a) += (BITSPREAD_BIAS);
#define ALPHACOLOR_RSHIFT(a, bits)  (a) >>= (bits);
#endif

# define BITSPREAD_MAXALPHA           (1 << BITSPREAD_EXTRABITS)

#ifndef BLENDPIXELRGB
#define BLENDPIXELRGB  BlendPixelRGB

#ifndef GEN_BITSPREAD_FROMRGB
#define GEN_BITSPREAD_FROMRGB(dst, r, g, b) dst = BITSPREAD_FROMRGB(r,g,b)
#endif
#ifndef GEN_BITSPREAD_FROMNATIVE
#define GEN_BITSPREAD_FROMNATIVE(dst, src)  dst = BITSPREAD_FROMNATIVE(src)
#endif

#ifndef GEN_BITSPREAD_TONATIVE
#define GEN_BITSPREAD_TONATIVE(dst, src)    dst = BITSPREAD_TONATIVE(src)
#endif

// Blend RGBA value with pixel at pbmDst[x,y]
//
static __inline void BlendPixelRGB(OEMBitmap *pbmDst, unsigned x, unsigned y, const byte *pRGBA)
{
   byte alpha = pRGBA[3];

   if (alpha != 0) {
      // not completely transparent
      if (alpha == 255) {
         // completely opaque
         SETPIXEL(pbmDst, x, y, RGB_TO_NATIVE(pRGBA[2], pRGBA[1], pRGBA[0]));
      } else {
         // blend
         AlphaColor uDest;
         AlphaColor uSrc;
         NativeColor clrDest = GETPIXEL(pbmDst, x, y);
         int f = BITSPREAD_SCALEALPHA(alpha);

         GEN_BITSPREAD_FROMNATIVE(uDest, clrDest);
         GEN_BITSPREAD_FROMRGB(uSrc, pRGBA[2], pRGBA[1], pRGBA[0]);

         ALPHACOLOR_SCALE(uSrc, f);
         ALPHACOLOR_SCALE(uDest, (BITSPREAD_MAXALPHA - f));
         ALPHACOLOR_SUM(uDest, uSrc);
         ALPHACOLOR_ADDBIAS(uDest);
         ALPHACOLOR_RSHIFT(uDest, BITSPREAD_EXTRABITS);

         GEN_BITSPREAD_TONATIVE(clrDest, uDest);

         SETPIXEL(pbmDst, x, y, clrDest);
      }
   }
}
#endif  // !defined(BLENDPIXELRGB)


#ifndef BLENDPIXELNATIVE
# define BLENDPIXELNATIVE   BlendPixelNative

// Blend RGBA value with pixel at pbmDst[x,y]
//
static __inline void BlendPixelNative(OEMBitmap *pbmDst, unsigned x, unsigned y, NativeColor nc)
{
   byte alpha = (byte) (nc >> COLOR_DEPTH);

   if (alpha != 0) {
      // not completely transparent
      if (alpha == 255) {
         // completely opaque
         SETPIXEL(pbmDst, x, y, nc);
      } else {
         // blend
         AlphaColor uDest;
         AlphaColor uSrc;
         NativeColor clrDest = GETPIXEL(pbmDst, x, y);
         int f = BITSPREAD_SCALEALPHA(alpha);

         GEN_BITSPREAD_FROMNATIVE(uDest, clrDest);
         GEN_BITSPREAD_FROMNATIVE(uSrc, (nc & ((1 << COLOR_DEPTH)-1)));

         ALPHACOLOR_SCALE(uSrc, f);
         ALPHACOLOR_SCALE(uDest, (BITSPREAD_MAXALPHA - f));
         ALPHACOLOR_SUM(uDest, uSrc);
         ALPHACOLOR_ADDBIAS(uDest);
         ALPHACOLOR_RSHIFT(uDest, BITSPREAD_EXTRABITS);

         GEN_BITSPREAD_TONATIVE(clrDest, uDest);

         SETPIXEL(pbmDst, x, y, clrDest);
      }
   }
}

#endif // !defined( BLENDPIXELNATIVE )


// AlphaMap : Holds values precomputed from palette for alpha blending purposes.

OBJECT(AlphaMap) {
   const AEEVTBL(IQueryInterface) *pvt;
   uint32         uRefs;
   byte *         pAlphas;
   // colors[] array starts here, followed by pAlphas[]
};

// AddRef / Release / QueryInterface are identical to those of PaletteMap
//
static const VTBL(IQueryInterface) gAlphaMapFuncs = {
   PaletteMap_AddRef,
   PaletteMap_Release,
   PaletteMap_QueryInterface
};
static __inline AlphaColor AlphaColor_FromRGBA(const byte *pRGBA, byte *pbyDestAlpha)
{
   int a = BITSPREAD_SCALEALPHA(pRGBA[3]);

   *pbyDestAlpha = BITSPREAD_MAXALPHA - a;
   if (a == BITSPREAD_MAXALPHA) {
      AlphaColor rv;
      // fully opqaue (src=max, dest=0) => store native result
      GEN_BITSPREAD_FROMRGB(rv, pRGBA[2], pRGBA[1], pRGBA[0]); 
      return rv;
   } else {
      AlphaColor rv;
      GEN_BITSPREAD_FROMRGB(rv, pRGBA[2], pRGBA[1], pRGBA[0]); 
      ALPHACOLOR_SCALE(rv, a); 
      ALPHACOLOR_ADDBIAS(rv);
      return rv;
   }
}


// For blending operations we use a different type of palette map.  Instead of
// holding a single native color per entry, it holds pre-computed values for
// scaled alpha and spreaded multiplied source data.
//
static __inline int InitAlphaMap(IDIB *pbmSrc)
{
   // if already created, nothing to do

   if (pbmSrc->pPaletteMap && 
       AEEGETPVTBL(pbmSrc->pPaletteMap, IQueryInterface) == &gAlphaMapFuncs) {
      return SUCCESS;
   }

   if ( BETWEEN(pbmSrc->nDepth, 1, 9) ) {

      // try to create alpha map

      int cntColors = (1 << pbmSrc->nDepth);
      int cntRGB = MIN(pbmSrc->cntRGB, cntColors);
      AlphaMap *pMap;
      AlphaColor *pColors;
      byte *pAlphas;
      int ndx;
      byte *pbyRGB = (byte*)pbmSrc->pRGB;

      IDIB_FlushPalette(pbmSrc);

      pMap = (AlphaMap*) MALLOC(sizeof(AlphaMap)
                                + sizeof(AlphaColor)*cntColors   // alphacolors
                                + sizeof(byte)*cntColors );      // alpha factors
      if (!pMap) {
         return ENOMEMORY;
      }

      pColors = (AlphaColor*) (pMap+1);
      pAlphas = (byte*) (pColors + cntColors);

      pMap->uRefs = 1;
      pMap->pvt = (AEEVTBL(IQueryInterface) *) &gAlphaMapFuncs;
      pMap->pAlphas = pAlphas;

      for (ndx = cntRGB; --ndx >= 0; ) {
         pColors[ndx] = AlphaColor_FromRGBA( pbyRGB + ndx*sizeof(uint32), &pAlphas[ndx]);
      }

      pbmSrc->pPaletteMap = (IQueryInterface*)pMap;

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// PIXELITER_BLEND : blend source pixel with current pixel
//
//  pa = array of alpha values
//  pclr = array of pre-computed AlphaColor values for source pixel
//
#define PIXELITER_BLEND(clr, pa, pclr) \
  {                                                             \
      int nDestAlpha = (pa)[clr];                               \
      if (nDestAlpha != BITSPREAD_MAXALPHA) {                   \
         NativeColor n;                                         \
         AlphaColor u = (pclr)[clr];                            \
         if (nDestAlpha != 0) {                                 \
            AlphaColor uDest;                                   \
            NativeColor ncDest = PIXELITER_GET();               \
            GEN_BITSPREAD_FROMNATIVE(uDest, ncDest);                \
            ALPHACOLOR_SCALE(uDest, nDestAlpha);                \
            ALPHACOLOR_SUM(u, uDest);                           \
            ALPHACOLOR_RSHIFT(u, BITSPREAD_EXTRABITS);          \
         }                                                      \
         GEN_BITSPREAD_TONATIVE(n, u);                              \
         PIXELITER_SET( n );                                    \
      }                                                         \
   }


// Blend 8-bit RGBA palette image into pbmDst.
//
// NOTE: Assumes alpha map is successfully initialized.
//
static __inline int BlendDib1(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                              int xSrc, int ySrc, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   AlphaMap *pmap = (AlphaMap*) pbmSrc->pPaletteMap;
   AlphaColor *pColors = (AlphaColor*) (pmap+1);
   byte *pAlphas = pmap->pAlphas;

   PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

   for (y = yDst; y < yDstMax; ++y) {
      byte *pby = pbyBase;
      byte by = *pby;
      byte byBit = 128 >> (xSrc & 7);
      int cntBits = 8 - (xSrc & 7);

      x = xDstMax - xDst;

      for (;;) {

         if (x < cntBits) {
            if (!x) {
               break;
            }
            // Make sure we run out of bits before running out of 'x'
            byBit >>= cntBits - x;
            by >>= cntBits - x;
            cntBits = x;
         }

         do {
            int clr = ( (byBit & by) ? 1 : 0);

            PIXELITER_BLEND(clr, pAlphas, pColors);

            PIXELITER_NEXTX();
            byBit >>= 1;
         } while (byBit);

         x -= cntBits;

         by = *++pby;
         byBit = 128;
         cntBits = 8;
      }
      PIXELITER_NEXTY();

      pbyBase += pbmSrc->nPitch;
   }

   return SUCCESS;
}


static __inline int BlendDib2(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                              int xSrc, int ySrc, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x,y;
   AlphaMap *pmap = (AlphaMap*) pbmSrc->pPaletteMap;
   AlphaColor *pColors = (AlphaColor*) (pmap+1);
   byte *pAlphas = pmap->pAlphas;

   PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

   for (y = yDst; y < yDstMax; ++y) {
      byte *pby = pbyBase;
      int nSrcBits = (3 - (xSrc & 3)) << 1;
      for (x = xDst; x < xDstMax; ++x) {
         uint8 c = (((unsigned)*pby) >> nSrcBits) & 3;

         PIXELITER_BLEND(c, pAlphas, pColors);

         PIXELITER_NEXTX();
         if (nSrcBits == 0) {
            nSrcBits = 6;
            ++pby;
         } else {
            nSrcBits -= 2;
         }
      }
      pby = pbyBase += pbmSrc->nPitch;
      PIXELITER_NEXTY();
   }

   return SUCCESS;
}


// Blend 8-bit RGBA palette image into pbmDst.
//
// NOTE: Assumes alpha map is successfully initialized.
//
static __inline int BlendDib8(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                              int xSrc, int ySrc, const byte *pbyBase)
{
   int cx, cy;
   AlphaMap *pmap = (AlphaMap*) pbmSrc->pPaletteMap;
   AlphaColor *pColors = (AlphaColor*) (pmap+1);
   register byte *pAlphas = pmap->pAlphas;
   const byte *pby = pbyBase;

   PIXELITER_SETUP(pbmDst, xDst+dx, yDst, 0, 0, FALSE);

   for (cy = dy; cy > 0; --cy) {
      for (cx = dx; --cx >= 0;) {
         PIXELITER_PREVX();
         PIXELITER_BLEND(pby[cx], pAlphas, pColors);
      }
      pby += pbmSrc->nPitch;
      PIXELITER_NEXTY();
   }

   return SUCCESS;
}


static __inline int BlendDib4(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                              int xSrc, int ySrc, const byte *pbyBase)
{
   int cx, cy;
   AlphaMap *pmap = (AlphaMap*) pbmSrc->pPaletteMap;
   AlphaColor *pColors = (AlphaColor*) (pmap+1);
   register byte *pAlphas = pmap->pAlphas;
   int nPitch = pbmSrc->nPitch;

   if (dx == 0)
      return SUCCESS;

   // odd pixel on left?
   if ((xSrc & 1)) {
      const byte *pby = pbyBase;
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);
      for (cy = dy; cy > 0; --cy) {
         PIXELITER_BLEND(*pby & 0x0F, pAlphas, pColors);
         PIXELITER_NEXTY();
         pby += nPitch;
      }
      ++pbyBase;
      ++xDst;
      --dx;
   }

   // odd pixel on right?
   if ((dx & 1) != 0) {
      const byte *pby = pbyBase + (dx>>1);
      PIXELITER_SETUP(pbmDst, xDst+dx-1, yDst, 0, 0, FALSE);
      for (cy = dy; cy > 0; --cy) {
         PIXELITER_BLEND(*pby >> 4, pAlphas, pColors);
         PIXELITER_NEXTY();
         pby += nPitch;
      }
   }

   // two pixels (one byte) at a time
   dx >>= 1;
   if (dx != 0) {
      const byte *pby = pbyBase;
      PIXELITER_SETUP(pbmDst, xDst+dx*2, yDst, 0, 0, FALSE);
      for (cy = dy; cy > 0; --cy) {
         for (cx = dx; --cx >= 0; ) {
            PIXELITER_PREVX();
            PIXELITER_BLEND(pby[cx] & 0x0F, pAlphas, pColors);
            PIXELITER_PREVX();
            PIXELITER_BLEND(pby[cx] >> 4, pAlphas, pColors);
         }
         pby += nPitch;
         PIXELITER_NEXTY();
      }
   }

   return SUCCESS;
}


static __inline int BlendDib888(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                                int xSrc, int ySrc, const byte *pbyBase)
{
   const uint8 *pby = pbyBase;
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int nStep = pbmSrc->nDepth / 8;
   int nAdjust = pbmSrc->nPitch - (dx * nStep);
   int x, y;

   for (y = yDst; y < yDstMax; ++y) {
      for (x = xDst; x < xDstMax; ++x) {
         BLENDPIXELRGB(pbmDst, x, y, pby);
         pby += nStep;
      }
      pby += nAdjust;
   }

   return SUCCESS;
}


#ifndef BLENDDIB
# define BLENDDIB  BlendDib

static __inline int BlendDib(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
{
   byte *pbyBase = pbmSrc->pBmp + (ySrc * pbmSrc->nPitch) + ((xSrc * pbmSrc->nDepth) >> 3);

   if (pbmSrc->cntRGB) {

      // palette-based bitmap

      int rv =  InitAlphaMap(pbmSrc);
      if (rv != SUCCESS) {
         return rv;
      }

      if (pbmSrc->nDepth == 1) {

         return BlendDib1(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, pbyBase);

      } else if (pbmSrc->nDepth == 2) {

         return BlendDib2(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, pbyBase);

      } else if (pbmSrc->nDepth == 4) {

         return BlendDib4(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, pbyBase);

      } else if (pbmSrc->nDepth == 8) {

         return BlendDib8(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, pbyBase);

      }

   } else if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_888 && pbmSrc->nDepth == 32) {

      return BlendDib888(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, pbyBase);

   }

   return EUNSUPPORTED;
}

#endif


#ifndef BLENDRECT
# define BLENDRECT BlendRect

// BlendRect
//
static int BlendRect(OEMBitmap *me, int x, int y, int cx, int cy, NativeColor color)
{
   AlphaColor uSrcProduct;
   uint32 uPixelMask = ((1 << COLOR_DEPTH) - 1);
   int nAlphaLo = BITSPREAD_SCALEALPHA((color >> COLOR_DEPTH) & 0xFF);
   register int nDestAlpha = BITSPREAD_MAXALPHA - nAlphaLo;

   PIXELITER_SETUP(me, x, y, color, 0, FALSE);
   GEN_BITSPREAD_FROMNATIVE(uSrcProduct, (color & uPixelMask)); 
   ALPHACOLOR_SCALE(uSrcProduct, nAlphaLo); 
   ALPHACOLOR_ADDBIAS(uSrcProduct);


   while (cy-- > 0) {
      int cnt = cx;
      while (cnt--) {
         NativeColor pixel;
         AlphaColor u;
         NativeColor nc = PIXELITER_GET();
         GEN_BITSPREAD_FROMNATIVE(u, nc);
         ALPHACOLOR_SCALE(u, nDestAlpha); 
         ALPHACOLOR_SUM(u, uSrcProduct);
         ALPHACOLOR_RSHIFT(u, BITSPREAD_EXTRABITS);
         GEN_BITSPREAD_TONATIVE(pixel, u);
         PIXELITER_SET(pixel);
         PIXELITER_NEXTX();
      }
      PIXELITER_NEXTY();
   }

   return SUCCESS;
}
#endif // BLENDRECT


#endif // defined(BITSPREAD_EXTRABITS)



static __inline int FillRect(OEMBitmap *pbmDst, int x, int y, int dx, int dy, NativeColor color, AEERasterOp rop)
{
#if defined(FILLRECT_COPY)
   if (rop == AEE_RO_COPY) {
      FILLRECT_COPY(pbmDst, x, y, dx, dy, color);
      return SUCCESS;
   }
#elif defined(PIXELITER_SETFG)
   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, x, y, color, 0, FALSE);
      while (dy-- > 0) {
         int cx = dx;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, color);
            PIXELITER_SETAT(1, color);
            PIXELITER_SETAT(2, color);
            PIXELITER_SETAT(3, color);
            PIXELITER_SETAT(4, color);
            PIXELITER_SETAT(5, color);
            PIXELITER_SETAT(6, color);
            PIXELITER_SETAT(7, color);
            cx -= 8;
            PIXELITER_ADDX(8);
         }
#endif //PIXELITER_SETAT
         while (cx--) {
            PIXELITER_SETFG();
            PIXELITER_NEXTX();
         }
         PIXELITER_NEXTY();
      }
      return SUCCESS;
   }
#endif //FILLRECT_COPY
#if defined(FILLRECT_XOR)
   if (rop == AEE_RO_XOR) {
      FILLRECT_XOR(pbmDst, x, y, dx, dy, color);
      return SUCCESS;
   }
#elif defined(PIXELITER_SETUP)
   if (rop == AEE_RO_XOR) {
      PIXELITER_SETUP(pbmDst, x, y, color, 0, FALSE);
      while (dy-- > 0) {
         int cx = dx;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, color ^ PIXELITER_GETAT(0));
            PIXELITER_SETAT(1, color ^ PIXELITER_GETAT(1));
            PIXELITER_SETAT(2, color ^ PIXELITER_GETAT(2));
            PIXELITER_SETAT(3, color ^ PIXELITER_GETAT(3));
            PIXELITER_SETAT(4, color ^ PIXELITER_GETAT(4));
            PIXELITER_SETAT(5, color ^ PIXELITER_GETAT(5));
            PIXELITER_SETAT(6, color ^ PIXELITER_GETAT(6));
            PIXELITER_SETAT(7, color ^ PIXELITER_GETAT(7));
            cx -= 8;
            PIXELITER_ADDX(8);
         }
#endif //PIXELITER_SETAT
         while (cx--) {
            PIXELITER_SET(color ^ PIXELITER_GET());
            PIXELITER_NEXTX();
         }
         PIXELITER_NEXTY();
      }
      return SUCCESS;
   }
#endif //FILLRECT_XOR

#ifdef BLENDRECT
   if (rop == AEE_RO_BLEND) {
      return BLENDRECT(pbmDst, x, y, dx, dy, color);
   }
#endif // BLENDRECT

   switch (rop) {
   case AEE_RO_COPY:
      while (dy--) {
         int cx = dx;
         while (cx--) {
            SETPIXEL(pbmDst, x + cx, y + dy, color);
         }
      }
      break;
   case AEE_RO_XOR:
      while (dy--) {
         int cx = dx;
         while (cx--) {
            SETPIXEL(pbmDst, x + cx, y + dy, GETPIXEL(pbmDst, x + cx, y + dy) ^ color);
         }
      }
      break;
   case AEE_RO_TRANSPARENT:
      return SUCCESS;
   default:
      return EUNSUPPORTED;
   }

   return SUCCESS;
}


static int BlitDdb(OEMBitmap *pbmDst, OEMBitmap *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc, AEERasterOp rop)
{
   boolean bRevX = FALSE;
   boolean bRevY = FALSE;

   if (
#if defined(BLITDDB_COPY)
       rop != AEE_RO_COPY &&
#endif
#if defined(BLITDDB_TRANSPARENT)
       rop != AEE_RO_TRANSPARENT &&
#endif
#if defined(BLITDDB_XOR)
       rop != AEE_RO_XOR &&
#endif
       pbmSrc == pbmDst) {
      // possible overlap
      if (ySrc < yDst && yDst - ySrc < dy &&
          ((xDst < xSrc && xDst + dx >= xSrc) ||
           (xDst >= xSrc && xDst < xSrc + dx))) {
         // overlap in the Y direction, reverse Y order to fix
         // This case must be fast, since it will be used a lot for vertical scrolling.
         bRevY = TRUE;
         yDst += dy - 1;
         ySrc += dy - 1;
      } else if (ySrc == yDst && xSrc < xDst && xDst - xSrc < dx) {
         // overlap in the X direction, reverse X order to fix
         // This case does not have to be fast.
         bRevX = TRUE;
         xDst += dx - 1;
         xSrc += dx - 1;
      }
   }

#if defined(BLITDDB_COPY)
   if (rop == AEE_RO_COPY) {
      BLITDDB_COPY(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
      return SUCCESS;
   }
#elif defined(PIXELITER_SRC_SETUP)
   if (rop == AEE_RO_COPY && !bRevX) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, bRevY);
      PIXELITER_SRC_SETUP(pbmSrc, xSrc, ySrc, bRevY);
      while (dy--) {
         int cx = dx;

#if defined(PIXELITER_SRC_GETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, PIXELITER_SRC_GETAT(0));
            PIXELITER_SETAT(1, PIXELITER_SRC_GETAT(1));
            PIXELITER_SETAT(2, PIXELITER_SRC_GETAT(2));
            PIXELITER_SETAT(3, PIXELITER_SRC_GETAT(3));
            PIXELITER_SETAT(4, PIXELITER_SRC_GETAT(4));
            PIXELITER_SETAT(5, PIXELITER_SRC_GETAT(5));
            PIXELITER_SETAT(6, PIXELITER_SRC_GETAT(6));
            PIXELITER_SETAT(7, PIXELITER_SRC_GETAT(7));
            cx -= 8;
            PIXELITER_ADDX(8);
            PIXELITER_SRC_ADDX(8);
         }
#endif

         while (cx--) {
            PIXELITER_SET(PIXELITER_SRC_GET());
            PIXELITER_NEXTX();
            PIXELITER_SRC_NEXTX();
         }
         PIXELITER_NEXTY();
         PIXELITER_SRC_NEXTY();
      }
      return SUCCESS;
   }
#endif
#if defined(BLITDDB_TRANSPARENT)
   if (rop == AEE_RO_TRANSPARENT) {
      BLITDDB_TRANSPARENT(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
      return SUCCESS;
   }
#elif defined(PIXELITER_SRC_SETUP)
   if (rop == AEE_RO_TRANSPARENT && !bRevX) {
      NativeColor ncTransparent = pbmSrc->m_dib.ncTransparent;
      NativeColor nc;
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, bRevY);
      PIXELITER_SRC_SETUP(pbmSrc, xSrc, ySrc, bRevY);
      while (dy--) {
         int cx = dx;

#if defined(PIXELITER_SRC_GETAT)
         while (cx >= 8) {
            nc = PIXELITER_SRC_GETAT(0);
            if (nc != ncTransparent)
               PIXELITER_SETAT(0, nc);
            nc = PIXELITER_SRC_GETAT(1);
            if (nc != ncTransparent)
               PIXELITER_SETAT(1, nc);
            nc = PIXELITER_SRC_GETAT(2);
            if (nc != ncTransparent)
               PIXELITER_SETAT(2, nc);
            nc = PIXELITER_SRC_GETAT(3);
            if (nc != ncTransparent)
               PIXELITER_SETAT(3, nc);
            nc = PIXELITER_SRC_GETAT(4);
            if (nc != ncTransparent)
               PIXELITER_SETAT(4, nc);
            nc = PIXELITER_SRC_GETAT(5);
            if (nc != ncTransparent)
               PIXELITER_SETAT(5, nc);
            nc = PIXELITER_SRC_GETAT(6);
            if (nc != ncTransparent)
               PIXELITER_SETAT(6, nc);
            nc = PIXELITER_SRC_GETAT(7);
            if (nc != ncTransparent)
               PIXELITER_SETAT(7, nc);
            cx -= 8;
            PIXELITER_ADDX(8);
            PIXELITER_SRC_ADDX(8);
         }
#endif

         while (cx--) {
            nc = PIXELITER_SRC_GET();
            if (nc != ncTransparent)
               PIXELITER_SET(nc);
            PIXELITER_NEXTX();
            PIXELITER_SRC_NEXTX();
         }
         PIXELITER_NEXTY();
         PIXELITER_SRC_NEXTY();
      }
      return SUCCESS;
   }
#endif
#if defined(BLITDDB_XOR)
   if (rop == AEE_RO_XOR) {
      BLITDDB_XOR(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
      return SUCCESS;
   }
#elif defined(PIXELITER_SRC_SETUP)
   if (rop == AEE_RO_XOR && !bRevX) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, bRevY);
      PIXELITER_SRC_SETUP(pbmSrc, xSrc, ySrc, bRevY);
      while (dy--) {
         int cx = dx;

#if defined(PIXELITER_SRC_GETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, PIXELITER_GETAT(0) ^ PIXELITER_SRC_GETAT(0));
            PIXELITER_SETAT(1, PIXELITER_GETAT(1) ^ PIXELITER_SRC_GETAT(1));
            PIXELITER_SETAT(2, PIXELITER_GETAT(2) ^ PIXELITER_SRC_GETAT(2));
            PIXELITER_SETAT(3, PIXELITER_GETAT(3) ^ PIXELITER_SRC_GETAT(3));
            PIXELITER_SETAT(4, PIXELITER_GETAT(4) ^ PIXELITER_SRC_GETAT(4));
            PIXELITER_SETAT(5, PIXELITER_GETAT(5) ^ PIXELITER_SRC_GETAT(5));
            PIXELITER_SETAT(6, PIXELITER_GETAT(6) ^ PIXELITER_SRC_GETAT(6));
            PIXELITER_SETAT(7, PIXELITER_GETAT(7) ^ PIXELITER_SRC_GETAT(7));
            cx -= 8;
            PIXELITER_ADDX(8);
            PIXELITER_SRC_ADDX(8);
         }
#endif

         while (cx--) {
            PIXELITER_SET(PIXELITER_GET() ^ PIXELITER_SRC_GET());
            PIXELITER_NEXTX();
            PIXELITER_SRC_NEXTX();
         }
         PIXELITER_NEXTY();
         PIXELITER_SRC_NEXTY();
      }
      return SUCCESS;
   }
#endif


   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {

      // note: does not support blend because native formats have no alpha channel

      int x = 0, y = 0;
      int xInc = 1, yInc = 1;
      int dx2;

      if (bRevX) {
         xInc = -1;
      }
      if (bRevY) {
         yInc = -1;
      }

      for (; dy > 0; dy--, y += yInc) {
         for (dx2 = dx, x = 0; dx2 > 0; dx2--, x += xInc) {
            if (rop == AEE_RO_TRANSPARENT) {
               NativeColor c = GETPIXEL(pbmSrc, xSrc + x, ySrc + y);
               if (c != pbmSrc->m_dib.ncTransparent) {
                  SETPIXEL(pbmDst, xDst + x, yDst + y, c);
               }
            } else {
               RopPixel(pbmDst, xDst + x, yDst + y, GETPIXEL(pbmSrc, xSrc + x, ySrc + y), rop);
            }
         }
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


#ifndef SCHEME332_TO_NATIVE   // this could be identity function

# define SCHEME332_TO_NATIVE(v) RGB_TO_NATIVE( (((v) & 0xE0) >> 3) * 9,  /* 6 bits is enough */ \
                                               ((v) & 0x1C) * 9,         \
                                               ((v) & 0x03) * 0x55 )
#endif


// Contruct a palette map if possible, unless DIB is DDB-compatible.
//
// The palette map contains an array of NativeColors that is allocated to a size
// of 1<<nDepth, regardless of the number of colors defined in the bitmap.
//
static __inline boolean InitPaletteMap(IDIB *pbmSrc, boolean *pbCompatible)
{
   // if already created, nothing to do

   if (pbmSrc->pPaletteMap && 
       AEEGETPVTBL(pbmSrc->pPaletteMap, IQueryInterface) == &gPaletteMapFuncs) {
      *pbCompatible = ((PaletteMap*)pbmSrc->pPaletteMap)->m_bDDB;
      return TRUE;
   }

   // if DDB-compatible scheme, no need to create palette map

#if defined(DIB_COMPATIBLE) && COLOR_SCHEME != 0
   if (pbmSrc->nDepth == PIXEL_SIZE && pbmSrc->nColorScheme == COLOR_SCHEME) {
      *pbCompatible = TRUE;
      return FALSE;
   }
#endif

   // can we construct a palette map for this?

   if ( (BETWEEN(pbmSrc->nDepth, 1, 9) &&
         (pbmSrc->cntRGB > 0 || pbmSrc->nColorScheme == IDIB_COLORSCHEME_332)) ) {

      int cntColors = (1 << pbmSrc->nDepth);
      int cntRGB = MIN(pbmSrc->cntRGB, cntColors);
      int ndx;
      PaletteMap *pMap;
      PixelType *pColors;

      IDIB_FlushPalette(pbmSrc);

      pMap = (PaletteMap*) MALLOC(sizeof(PaletteMap) + sizeof(PixelType) * cntColors);
      pColors = (PixelType*)(pMap + 1);

      if (!pMap) {
         *pbCompatible = FALSE;
         return FALSE;
      }

      pMap->m_pColors = pColors;
      pMap->m_uRefs = 1;
      pMap->pvt = (AEEVTBL(IQueryInterface) *) &gPaletteMapFuncs;

      pbmSrc->pPaletteMap = (IQueryInterface*)pMap;

      if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_332) {

         // construct from 3-3-2 bit fields
         for (ndx = 255; ndx >= 0; --ndx) {
            pColors[ndx] = (PixelType) SCHEME332_TO_NATIVE(ndx);
         }

      } else {

         uint32 *pRGB = pbmSrc->pRGB;

         for (ndx = cntRGB; --ndx >= 0; ) {
            uint32 rgb = pRGB[ndx];
            pColors[ndx] = (PixelType) RGB_TO_NATIVE((rgb >> 16) & 0xFF,
                                                     (rgb >> 8) & 0xFF,
                                                     rgb & 0xFF);
         }

#if defined(DIB_COMPATIBLE)

         if (pbmSrc->nDepth == PIXEL_SIZE) {
            // compatible => same depth && palette is identity function

            pMap->m_bDDB = TRUE;

            // note: check only those colors defined in the bitmap; others are undefined
            for (ndx = cntRGB; ndx >= 0; --ndx) {
               if (pColors[ndx] != (NativeColor)ndx) {
                  pMap->m_bDDB = FALSE;
                  break;
               }
            }
         }

#endif

      }

      *pbCompatible = pMap->m_bDDB;
      return TRUE;
   }

   *pbCompatible = FALSE;
   return FALSE;
}




// BlitDib1 : Copy from 1-bit DIB
//
static __inline int BlitDib1(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int yDstMax = yDst + dy;
   int x, y;
   uint32 rgb0;
   uint32 rgb1;
   PixelType aColors[2];

   rgb0 = pbmSrc->cntRGB ? pbmSrc->pRGB[0] : 0;
   rgb1 = pbmSrc->cntRGB > 1 ? pbmSrc->pRGB[1] : 0;
   aColors[0] = (PixelType) RGB_TO_NATIVE((rgb0 >> 16) & 0xFF,
                                          (rgb0 >> 8) & 0xFF,
                                          rgb0 & 0xFF);
   aColors[1] = (PixelType) RGB_TO_NATIVE((rgb1 >> 16) & 0xFF,
                                          (rgb1 >> 8) & 0xFF,
                                          rgb1 & 0xFF);

#if defined(PIXELITER_BIT)

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, aColors[1], aColors[0], FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         byte by = *pby;
         byte byBit = 128 >> (xSrc & 7);

         x = dx;
         for (;;) {
            PIXELITER_BIT(byBit & by);
            PIXELITER_NEXTX();

            if (--x == 0) {
               break;
            }

            byBit >>= 1;
            if (!byBit) {
               // get next byte of source
               if (x >= 8) {
                  do {
                     by = *++pby;
                     PIXELITER_BIT(by & 128);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 64);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 32);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 16);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 8);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 4);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 2);
                     PIXELITER_NEXTX();
                     PIXELITER_BIT(by & 1);
                     PIXELITER_NEXTX();
                     x -= 8;
                  } while (x >= 8);
                  if (!x) {
                     break;
                  }
               }
               by = *++pby;
               byBit = 128;
            }
         }
         PIXELITER_NEXTY();

         pbyBase += pbmSrc->nPitch;
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {
      unsigned byInv = pbmSrc->ncTransparent ? 0xFF : 0x00;
      PIXELITER_SETUP(pbmDst, xDst, yDst, aColors[byInv == 0], 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         byte by = *pby ^ byInv;
         byte byBit = 128 >> (xSrc & 7);
         int cntBits = 8 - (xSrc & 7);

         x = dx;

         for (;;) {

            if (x < cntBits) {
               if (!x) {
                  break;
               }
               // Make sure we run out of bits before running out of 'x'
               byBit >>= cntBits - x;
               by >>= cntBits - x;
               cntBits = x;
            }

            do {
               if (byBit & by) {
                  PIXELITER_SETFG();
               }
               PIXELITER_NEXTX();
               byBit >>= 1;
            } while (byBit);

            x -= cntBits;

            by = *++pby ^ byInv;
            byBit = 128;
            cntBits = 8;
         }
         PIXELITER_NEXTY();

         pbyBase += pbmSrc->nPitch;
      }

      return SUCCESS;
   }

#endif // PIXELITER_SETUP

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {
      byte tc = pbmSrc->ncTransparent ? 0xFF : 0; // expand transparent color to all bits

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         byte by = *pby;
         byte byBit = 1 << (7 - (xSrc & 7));
         int xDstMax = xDst + dx;

         for (x = xDst; x < xDstMax; ++x) {

            if (rop == AEE_RO_TRANSPARENT) {
               if ((by ^ tc) & byBit) {
                  SETPIXEL(pbmDst, x, y, ((byBit & by) ? aColors[1] : aColors[0]));
               }
            } else {
               RopPixel(pbmDst, x, y, ((byBit & by) ? aColors[1] : aColors[0]), rop);
            }

            byBit >>= 1;
            if (!byBit) {
               by = *++pby;
               byBit = 128;
            }
         }

         pbyBase += pbmSrc->nPitch;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib2 : Copy from 2-bit DIB
//
static __inline int BlitDib2(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x,y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;

#if defined(PIXELITER_SET)

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         int nSrcBits = (3 - (xSrc & 3)) << 1;
         for (x = xDst; x < xDstMax; ++x) {
            uint8 c = (((unsigned)*pby) >> nSrcBits) & 3;
            PIXELITER_SET(pColors[c]);
            PIXELITER_NEXTX();
            if (nSrcBits == 0) {
               nSrcBits = 6;
               ++pby;
            } else {
               nSrcBits -= 2;
            }
         }
         pby = pbyBase += pbmSrc->nPitch;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         int nSrcBits = (3 - (xSrc & 3)) << 1;
         for (x = xDst; x < xDstMax; ++x) {
            uint8 c = (((unsigned)*pby) >> nSrcBits) & 3;
            if (c != pbmSrc->ncTransparent) {
               PIXELITER_SET(pColors[c]);
            }
            PIXELITER_NEXTX();
            if (nSrcBits == 0) {
               nSrcBits = 6;
               ++pby;
            } else {
               nSrcBits -= 2;
            }
         }
         pby = pbyBase += pbmSrc->nPitch;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         int nSrcBits = (3 - (xSrc & 3)) << 1;
         for (x = xDst; x < xDstMax; ++x) {
            uint8 c = (((unsigned)*pby) >> nSrcBits) & 3;

            if (rop == AEE_RO_TRANSPARENT) {
               if (c != pbmSrc->ncTransparent) {
                  SETPIXEL(pbmDst, x, y, pColors[c]);
               }
            } else {
               RopPixel(pbmDst, x, y, pColors[c], rop);
            }

            if (nSrcBits == 0) {
               nSrcBits = 6;
               ++pby;
            } else {
               nSrcBits -= 2;
            }
         }

         pbyBase += pbmSrc->nPitch;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib4 : Copy from 4-bit DIB
//
static __inline int BlitDib4(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;

#if defined(PIXELITER_SET)

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         byte by = *pby++;
         boolean bOdd = (xSrc & 1);
         for (x = xDst; x < xDstMax; ++x) {
            if (bOdd) {
               PIXELITER_SET(pColors[by & 0x0F]);
               bOdd = 0;
               by = *pby++;
            } else {
               PIXELITER_SET(pColors[by >> 4]);
               bOdd = 1;
            }
            PIXELITER_NEXTX();
         }
         pbyBase += pbmSrc->nPitch;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         byte *pby = pbyBase;
         byte by = *pby++;
         boolean bOdd = (xSrc & 1);
         for (x = xDst; x < xDstMax; ++x) {
            unsigned c;
            if (bOdd) {
               c = by & 0x0F;
               bOdd = 0;
               by = *pby++;
            } else {
               c = by >> 4;
               bOdd = 1;
            }
            if (c != pbmSrc->ncTransparent) {
               PIXELITER_SET(pColors[c]);
            }
            PIXELITER_NEXTX();
         }
         pbyBase += pbmSrc->nPitch;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {
      for (y = yDst; y < yDstMax; ++y) {

         byte *pby = pbyBase;
         byte by = *pby++;
         boolean bOdd = (xSrc & 1);

         for (x = xDst; x < xDstMax; ++x) {
            if (bOdd) {
               if (rop == AEE_RO_TRANSPARENT) {
                  if ((uint8)(by & 0x0F) != (pbmSrc->ncTransparent & 0x0F)) {
                     SETPIXEL(pbmDst, x, y, pColors[by & 0x0F]);
                  }
               } else {
                  RopPixel(pbmDst, x, y, pColors[by & 0x0F], rop);
               }
               by = *pby++;
               bOdd = 0;
            } else {
               if (rop == AEE_RO_TRANSPARENT) {
                  if ((uint8)((by >> 4) & 0x0F) != pbmSrc->ncTransparent) {
                     SETPIXEL(pbmDst, x, y, pColors[by >> 4]);
                  }
               } else {
                  RopPixel(pbmDst, x, y, pColors[by >> 4], rop);
               }
               bOdd = 1;
            }
         }

         pbyBase += pbmSrc->nPitch;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib8 : Copy from 8-bit DIB
//
static __inline int BlitDib8(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;
   byte *pby = pbyBase;
   int   nAdjust = pbmSrc->nPitch - (xDstMax - xDst);

#if defined(PIXELITER_SET)

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, pColors[pby[0]]);
            PIXELITER_SETAT(1, pColors[pby[1]]);
            PIXELITER_SETAT(2, pColors[pby[2]]);
            PIXELITER_SETAT(3, pColors[pby[3]]);
            PIXELITER_SETAT(4, pColors[pby[4]]);
            PIXELITER_SETAT(5, pColors[pby[5]]);
            PIXELITER_SETAT(6, pColors[pby[6]]);
            PIXELITER_SETAT(7, pColors[pby[7]]);
            PIXELITER_ADDX(8);
            pby += 8;
            cx -= 8;
         }
#endif
         while (cx) {
            PIXELITER_SET(pColors[*pby]);
            PIXELITER_NEXTX();
            ++pby;
            --cx;
         }
         pby += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
         while (cx) {
            if (*pby != pbmSrc->ncTransparent) {
               PIXELITER_SET(pColors[*pby]);
            }
            PIXELITER_NEXTX();
            ++pby;
            --cx;
         }
         pby += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {

      for (y = yDst; y < yDstMax; ++y) {
         for (x = xDst; x < xDstMax; ++x) {
            if (rop == AEE_RO_TRANSPARENT) {
               if (*pby != (pbmSrc->ncTransparent)) {
                  SETPIXEL(pbmDst, x, y, pColors[*pby]);
               }
            } else {
               RopPixel(pbmDst, x, y, pColors[*pby], rop);
            }
            pby++;
         }
         pby += nAdjust;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib555 : Copy from 5-5-5 DIB
//
static __inline int BlitDib555(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;

#define SCHEME_555_TO_NATIVE(w) \
   RGB_TO_NATIVE((((uint16)(w) >> 10) & 0x1F) * 255 / 31, \
                 (((uint16)(w) >> 5) & 0x1F) * 255 / 31,  \
                 ((uint16)(w) & 0x1F) * 255 / 31)

   uint16 *pw = (uint16*)pbyBase;
   int nAdjust = pbmSrc->nPitch / 2 - (xDstMax - xDst);

   if (pbmSrc->nDepth != 16) {
      return EUNSUPPORTED;
   }

#if defined(PIXELITER_SET)
   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, SCHEME_555_TO_NATIVE(pw[0]));
            PIXELITER_SETAT(1, SCHEME_555_TO_NATIVE(pw[1]));
            PIXELITER_SETAT(2, SCHEME_555_TO_NATIVE(pw[2]));
            PIXELITER_SETAT(3, SCHEME_555_TO_NATIVE(pw[3]));
            PIXELITER_SETAT(4, SCHEME_555_TO_NATIVE(pw[4]));
            PIXELITER_SETAT(5, SCHEME_555_TO_NATIVE(pw[5]));
            PIXELITER_SETAT(6, SCHEME_555_TO_NATIVE(pw[6]));
            PIXELITER_SETAT(7, SCHEME_555_TO_NATIVE(pw[7]));
            PIXELITER_ADDX(8);
            pw += 8;
            cx -= 8;
         }
#endif
         while (cx) {
            PIXELITER_SET(SCHEME_555_TO_NATIVE(*pw));
            PIXELITER_NEXTX();
            ++pw;
            --cx;
         }
         pw += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {

      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
         while (cx) {
            if (*pw != pbmSrc->ncTransparent) {
               PIXELITER_SET(SCHEME_555_TO_NATIVE(*pw));
            }
            PIXELITER_NEXTX();
            ++pw;
            --cx;
         }
         pw += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {

      for (y = yDst; y < yDstMax; ++y) {
         for (x = xDst; x < xDstMax; ++x) {
            if (rop == AEE_RO_TRANSPARENT) {
               if (*pw != pbmSrc->ncTransparent) {
                  SETPIXEL(pbmDst, x, y, SCHEME_555_TO_NATIVE(*pw));
               }
            } else {
               RopPixel(pbmDst, x, y, SCHEME_555_TO_NATIVE(*pw), rop);
            }
            pw++;
         }
         pw += nAdjust;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib565 : Copy from 5-6-5 DIB
//
static __inline int BlitDib565(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;

#define SCHEME_565_TO_NATIVE(w) \
   RGB_TO_NATIVE((((uint16)(w) >> 11) & 0x1F) * 255 / 31, \
                 (((uint16)(w) >> 5) & 0x3F) * 255 / 63,  \
                 ((uint16)(w) & 0x1F) * 255 / 31)

   uint16 *pw = (uint16*)pbyBase;
   int   nAdjust = pbmSrc->nPitch / 2 - (xDstMax - xDst);

   if (pbmSrc->nDepth != 16) {
      return EUNSUPPORTED;
   }

#if defined(PIXELITER_SET)
   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, SCHEME_565_TO_NATIVE(pw[0]));
            PIXELITER_SETAT(1, SCHEME_565_TO_NATIVE(pw[1]));
            PIXELITER_SETAT(2, SCHEME_565_TO_NATIVE(pw[2]));
            PIXELITER_SETAT(3, SCHEME_565_TO_NATIVE(pw[3]));
            PIXELITER_SETAT(4, SCHEME_565_TO_NATIVE(pw[4]));
            PIXELITER_SETAT(5, SCHEME_565_TO_NATIVE(pw[5]));
            PIXELITER_SETAT(6, SCHEME_565_TO_NATIVE(pw[6]));
            PIXELITER_SETAT(7, SCHEME_565_TO_NATIVE(pw[7]));
            PIXELITER_ADDX(8);
            pw += 8;
            cx -= 8;
         }
#endif
         while (cx) {
            PIXELITER_SET(SCHEME_565_TO_NATIVE(*pw));
            PIXELITER_NEXTX();
            ++pw;
            --cx;
         }
         pw += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

   if (rop == AEE_RO_TRANSPARENT) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
         while (cx) {
            if (*pw != pbmSrc->ncTransparent) {
               PIXELITER_SET(SCHEME_565_TO_NATIVE(*pw));
            }
            PIXELITER_NEXTX();
            ++pw;
            --cx;
         }
         pw += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop) || rop == AEE_RO_TRANSPARENT) {

      for (y = yDst; y < yDstMax; ++y) {
         for (x = xDst; x < xDstMax; ++x) {
            if (rop == AEE_RO_TRANSPARENT) {
               if (*pw != pbmSrc->ncTransparent) {
                  SETPIXEL(pbmDst, x, y, SCHEME_565_TO_NATIVE(*pw));
               }
            } else {
               RopPixel(pbmDst, x, y, SCHEME_565_TO_NATIVE(*pw), rop);
            }
            pw++;
         }
         pw += nAdjust;
      }

      return SUCCESS;
   }

   return EUNSUPPORTED;
}


// BlitDib888 : Copy from 8-8-8 DIB
//
static __inline int BlitDib888(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   int nStep = pbmSrc->nDepth / 8;
   uint8 *pby = pbyBase;
   int nAdjust = pbmSrc->nPitch - ((xDstMax - xDst) * nStep);

   if (!(pbmSrc->nDepth == 24 || pbmSrc->nDepth == 32)) {
      return EUNSUPPORTED;
   }

#if defined(PIXELITER_SET)
   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
#if defined(PIXELITER_SETAT)
         while (cx >= 8) {
            PIXELITER_SETAT(0, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(1, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(2, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(3, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(4, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(5, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(6, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_SETAT(7, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            pby += nStep;
            PIXELITER_ADDX(8);
            cx -= 8;
         }
#endif
         while (cx) {
            PIXELITER_SET(RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            PIXELITER_NEXTX();
            pby += nStep;
            --cx;
         }
         pby += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;

   }

   if (rop == AEE_RO_TRANSPARENT) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
         while (cx) {
            // Need to look at pby byte-by-byte for 24-bit... Otherwise we
            // end up with un-aligned memory access.
            if ((unsigned)(((pby[2] << 16 | pby[1] << 8 | pby[0])) & 0xFFFFFF) != pbmSrc->ncTransparent) {
               PIXELITER_SET(RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            }
            PIXELITER_NEXTX();
            pby += nStep;
            --cx;
         }
         pby += nAdjust;
         PIXELITER_NEXTY();
      }

      return SUCCESS;
   }

#else

   if (rop == AEE_RO_TRANSPARENT) {
      NativeColor ncXP = pbmSrc->ncTransparent & 0xFFFFFF;

      for (y = yDst; y < yDstMax; ++y) {
         for (x = xDst; x < xDstMax; ++x) {
            if ((unsigned)(pby[2] << 16 | pby[1] << 8 | pby[0]) != ncXP) {
               SETPIXEL(pbmDst, x, y, RGB_TO_NATIVE(pby[2], pby[1], pby[0]));
            }
            pby += nStep;
         }
         pby += nAdjust;
      }

      return SUCCESS;
   }

#endif

   if (RopPixelSupports(rop)) {
      for (y = yDst; y < yDstMax; ++y) {
         for (x = xDst; x < xDstMax; ++x) {
            RopPixel(pbmDst, x, y, RGB_TO_NATIVE(pby[2], pby[1], pby[0]), rop);
            pby += nStep;
         }
         pby += nAdjust;
      }
      return SUCCESS;
   }

   return EUNSUPPORTED;
}


static int BlitDib(OEMBitmap *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc, AEERasterOp rop)
{
   boolean bCompatible, bMapped;
   byte *pbyBase;

#ifdef BLENDDIB
   if (rop == AEE_RO_BLEND) {
      return BLENDDIB(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
   }
#endif

   if (1 == pbmSrc->nDepth && pbmSrc->cntRGB > 0) {
      // palette map is probably wasteful in this case, so BlitDib1 doesn't
      // use one
      pbyBase = pbmSrc->pBmp + (ySrc * pbmSrc->nPitch) + ((xSrc * pbmSrc->nDepth) >> 3);
      return BlitDib1(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);
   }

   bMapped = InitPaletteMap(pbmSrc, &bCompatible);
   if (bCompatible) {
      //                      @@@@@@ arghhhh!!!!
      return BlitDdb(pbmDst, (OEMBitmap*)pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop);
   }

   pbyBase = pbmSrc->pBmp + (ySrc * pbmSrc->nPitch) + ((xSrc * pbmSrc->nDepth) >> 3);

   if (bMapped) {

      // Paletted bitmap

      if (pbmSrc->nDepth == 1) {

         return BlitDib1(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      } else if (pbmSrc->nDepth == 2) {

         return BlitDib2(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      } else if (pbmSrc->nDepth == 4) {

         return BlitDib4(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      } else if (pbmSrc->nDepth == 8) {

         return BlitDib8(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      }

   } else {

      // No palette

      if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_555) {

         return BlitDib555(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      } else if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_565) {

         return BlitDib565(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      } else if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_888) {

         return BlitDib888(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop, pbyBase);

      }

   }

   return EUNSUPPORTED;
}


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static uint32 OEMBitmap_AddRef(IBitmap *po)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   return ++(pMe->m_uRefs);
}


static uint32 OEMBitmap_Release(IBitmap *po)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   AEECallback *pcbList, *pcb;
   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   // fire release callbacks
   pcbList = pMe->m_pcbRelease;
   pMe->m_pcbRelease = NULL;
   if (pcbList) {
      pcbList->pCancelData = &pcbList;
   }

   while ((AEECallback*)0 != (pcb = pcbList)) {
      CALLBACK_Cancel(pcb);
      if ((PFNNOTIFY)0 != pcb->pfnNotify) {
         pcb->pfnNotify(pcb->pNotifyData);
      }
   }

   // cancel callbacks
   while ((AEECallback*)0 != (pcb = pMe->m_pcbEnable)) {
      IShell *piShell = AEE_GetShell();
	   if(piShell) {
		ISHELL_Resume(piShell, pcb);
	   }
   }

   // There should be no callbacks left at this point.
   ASSERT(NULL == pMe->m_pcbRelease);
   ASSERT(NULL == pMe->m_pcbEnable);

   if (pMe->m_dib.pPaletteMap) {
      IQI_Release(pMe->m_dib.pPaletteMap);
   }

   if (pMe->m_pDisplayDev) {
      IDISPLAYDEV_Release(pMe->m_pDisplayDev);
   }

   if (pMe->m_pParent) {
      IBITMAP_Release(pMe->m_pParent);
   }

   if (pMe->m_pParentDev) {
      IBITMAPDEV_Release(pMe->m_pParentDev);
   }

   // Ref count is zero. So, release memory associated with this object.
   pMe->m_pfnRealloc(pMe, 0);

   return 0;
}


static int OEMBitmap_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   if (clsid == AEECLSID_BITMAP || clsid == AEECLSID_QUERYINTERFACE || clsid == AEEIID_OEMBITMAP3) {
      // I am an IBitmap/IQueryInterface, too.
      *ppNew = (void*)pMe;
      OEMBitmap_AddRef(po);
      return SUCCESS;
#ifdef DIB_COMPATIBLE
   } else if (clsid == AEECLSID_DIB) {
      *ppNew = &pMe->m_dib;
      OEMBitmap_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEECLSID_DIB_20) {
      *ppNew = &pMe->m_dib;
      OEMBitmap_AddRef(po);

      // make entire bitmap permanently dirty
      pMe->m_bDisableDirty = TRUE;
      pMe->m_xDirty = 0;
      pMe->m_x2Dirty = pMe->m_cxEffective;
      pMe->m_yDirty = 0;
      pMe->m_y2Dirty = pMe->m_dib.cy;

      return SUCCESS;
#endif
   } else if (clsid == AEEIID_BITMAPDEV && (pMe->m_pParent || pMe->m_pDisplayDev)) {
      *ppNew = (void*) &pMe->m_IBitmapDev;
      OEMBitmap_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEEIID_BITMAPCTL || clsid == AEEIID_BITMAPCTL_20 || clsid == AEEIID_BITMAPCTL_30) {
      *ppNew = (void*) &pMe->m_IBitmapCtl;
      OEMBitmap_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEEIID_BITMAPROT) {
      *ppNew = (void*) &pMe->m_IBitmapRot;
      OEMBitmap_AddRef(po);
      return SUCCESS;
#ifdef IMPLEMENT_ITRANSFORM
   } else if (clsid == AEECLSID_TRANSFORM) {
      *ppNew = (void*)&pMe->m_ITransform;
      OEMBitmap_AddRef(po);
      return SUCCESS;
#endif
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}


static NativeColor OEMBitmap_RGBToNative(IBitmap *po, RGBVAL rgb)
{
   NativeColor nc = RGB_TO_NATIVE((rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF, rgb >> 24);

#ifdef BITSPREAD_SCALEALPHA
   // include alpha channel
   nc |= (rgb & 0xFF) << COLOR_DEPTH;
#endif // BITSPREAD_SCALEALPHA

   return nc;
}


static RGBVAL OEMBitmap_NativeToRGB(IBitmap *po, NativeColor color)
{

#ifdef BITSPREAD_SCALEALPHA
   // include alpha channel
   return NATIVE_TO_RGB(color & ((1<<COLOR_DEPTH)-1)) | ((color >> COLOR_DEPTH) & 0xFF);
#else
   return NATIVE_TO_RGB(color);
#endif // BITSPREAD_SCALEALPHA
}


static int OEMBitmap_DrawPixel(IBitmap *po, unsigned int x, unsigned int y, NativeColor color, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   if (((signed)x < pMe->m_xDirty) || ((signed)x >= pMe->m_x2Dirty) ||
       ((signed)y < pMe->m_yDirty) || ((signed)y >= pMe->m_y2Dirty)) {

      if (x >= pMe->m_cxEffective || y >= pMe->m_dib.cy) {
         return SUCCESS;
      }

      EXPAND_DIRTY_RECT(pMe, (signed)x, (signed)y, 1, 1);
   }

   switch (rop) {
   case AEE_RO_COPY:
      SETPIXEL(pMe, x, y, color);
      break;

   case AEE_RO_XOR:
      SETPIXEL(pMe, x, y, GETPIXEL(pMe, x, y) ^ color);
      break;

#ifdef BLENDPIXELNATIVE
   case AEE_RO_BLEND:
      BLENDPIXELNATIVE(pMe, x, y, color);
      break;
#endif // BLENDPIXELNATIVE

   case AEE_RO_TRANSPARENT:

      return SUCCESS;
   default:
      return EUNSUPPORTED;
   }

   return SUCCESS;
}


static int OEMBitmap_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   if (x >= pMe->m_cxEffective || y >= pMe->m_dib.cy || pColor == NULL) {
      return EBADPARM;
   }

   *pColor = GETPIXEL(pMe, x, y);

   return SUCCESS;
}


// SetPixels: Set multiple pixels
//
// Performance note:
//
//   Optimized first for the case where pixels are in the dirty rectangle, and
//   second for the case where pixels are outside the clip bounds.  Cases
//   where individual pixels expand the dirty rectangle are treated as very
//   rare to reduce code bloat and setup overhead.  A couple of
//   justifications:
//
//   1. Drawing of individual pixels will almost always be done atop a
//      background that get repainted for each frame update, in which case
//      the pixels never expand the dirty rect.
//
//   2. We can place an upper bound on the number of times per frame the
//      dirty rectangle can be expanded: width + height.  This is a tiny
//      number relative to the number of pixels on the screen.
//
static int OEMBitmap_SetPixels(IBitmap *po, unsigned cntPoints, AEEPoint *pPoints, NativeColor color, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   unsigned cnt = cntPoints;
   AEEPoint *pp = pPoints;
   unsigned xDirty = pMe->m_xDirty;
   unsigned yDirty = pMe->m_yDirty;
   unsigned dxDirty = pMe->m_x2Dirty - xDirty;
   unsigned dyDirty = pMe->m_y2Dirty - yDirty;
   unsigned xMax = pMe->m_cxEffective;
   unsigned yMax = pMe->m_dib.cy;
   boolean bExpandDirty = 0;

   if (((int) dxDirty) < 0) {
      dxDirty = 0;
   }
   if (((int) dyDirty) < 0) {
      dyDirty = 0;
   }

   // DIRTY_OR_CLIP: Continue if clipped, break if we need to expand the dirty rect
#define DIRTY_OR_CLIP()  \
      if (x - xDirty >= dxDirty || y - yDirty >= dyDirty) {   \
         if (x >= xMax || y >= yMax) {                        \
            continue;  /* clipped */                          \
         } else {                                             \
            bExpandDirty = 1;     /* expand dirty rect */     \
            xDirty = 0;                                       \
            dxDirty = pMe->m_cxEffective;                     \
            yDirty = 0;                                       \
            dyDirty = pMe->m_dib.cy;                          \
         }                                                    \
      }


   switch (rop) {

   case AEE_RO_COPY:
      for (; cnt; --cnt, ++pp) {
         unsigned x = pp->x;
         unsigned y = pp->y;
         DIRTY_OR_CLIP();
         SETPIXEL(pMe, x, y, color);
      }
      break;

   case AEE_RO_XOR:
      for (; cnt; --cnt, ++pp) {
         unsigned x = pp->x;
         unsigned y = pp->y;
         DIRTY_OR_CLIP();
         SETPIXEL(pMe, x, y, GETPIXEL(pMe, x, y) ^ color);
      }
      break;

#ifdef BITSPREAD_EXTRABITS
   case AEE_RO_BLEND: {
      AlphaColor uSrcProduct;
      uint32 uPixelMask = ((1 << COLOR_DEPTH) - 1);
      int nAlphaLo = BITSPREAD_SCALEALPHA((color >> COLOR_DEPTH) & 0xFF);
      register int nDestAlpha = BITSPREAD_MAXALPHA - nAlphaLo;
      GEN_BITSPREAD_FROMNATIVE(uSrcProduct, color & uPixelMask); 
      ALPHACOLOR_SCALE(uSrcProduct,nAlphaLo);
      ALPHACOLOR_ADDBIAS(uSrcProduct);

      for ( ; cnt; --cnt, ++pp) {
         NativeColor nc;
         AlphaColor u;
         unsigned x = pp->x;
         unsigned y = pp->y;
         DIRTY_OR_CLIP();

         nc = GETPIXEL(pMe, x, y);
         GEN_BITSPREAD_FROMNATIVE(u, nc);
         ALPHACOLOR_SCALE(u, nDestAlpha);
         ALPHACOLOR_SUM(u, uSrcProduct);
         ALPHACOLOR_RSHIFT(u, BITSPREAD_EXTRABITS);
         GEN_BITSPREAD_TONATIVE(nc, u);
         SETPIXEL(pMe, x, y, nc);
      }
      break;
   }
#endif // BITSPREAD_EXTRABITS
   default:
      return EUNSUPPORTED;
   }


   if (bExpandDirty) {
      unsigned x2Dirty = pMe->m_x2Dirty;
      unsigned y2Dirty = pMe->m_y2Dirty;
      xDirty = pMe->m_xDirty;
      yDirty = pMe->m_yDirty;

      for (cnt = cntPoints, pp = pPoints; cnt; --cnt, ++pp) {
         unsigned x = pp->x;
         unsigned y = pp->y;

         if (x < xMax && y < yMax) {
            if (x < xDirty) {
               xDirty = x;
            }
            if (x >= x2Dirty) {
               x2Dirty = x+1;
            }
            if (y < yDirty) {
               yDirty = y;
            }
            if (y >= y2Dirty) {
               y2Dirty = y+1;
            }
         }
      }

      pMe->m_xDirty = xDirty;
      pMe->m_yDirty = yDirty;
      pMe->m_x2Dirty = x2Dirty;
      pMe->m_y2Dirty = y2Dirty;
   }

   return SUCCESS;
}


static int OEMBitmap_DrawHScanline(IBitmap *po, unsigned yParm, unsigned xMinParm, unsigned xMaxParm, NativeColor color, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   int y = (int)yParm;
   int xMin = (int)xMinParm;
   int xMax = (int)xMaxParm;


   // compare to dirty rectangle
   if (IS_OUTSIDE_DIRTY_RECT(pMe, xMin, y, xMax - xMin + 1, 1)) {

      // clip to bitmap boundries
      if (xMin < 0) {
         xMin = 0;
      }
      if (xMax >= pMe->m_cxEffective) {
         xMax = pMe->m_cxEffective - 1;
      }
      if ((unsigned)y >= pMe->m_dib.cy || xMin > xMax) {
         // nothing to draw
         return SUCCESS;
      }

      // expand dirty rectangle
      EXPAND_DIRTY_RECT(pMe, xMin, y, xMax - xMin + 1, 1);

   } else if (xMin > xMax) {

      // nothing to draw
      return SUCCESS;

   }


   // draw scan line
   return FillRect(pMe, xMin, y, xMax - xMin + 1, 1, color, rop);
}


static int OEMBitmap_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   int16 x, y, dx, dy;

   x = pRect->x;
   y = pRect->y;
   dx = pRect->dx;
   dy = pRect->dy;

   // compare to dirty rectangle
   if (IS_OUTSIDE_DIRTY_RECT(pMe, x, y, dx, dy)) {

      // clip to bitmap boundries
      if (x < 0) {
         dx += x;
         x = 0;
      }
      if (y < 0) {
         dy += y;
         y = 0;
      }
      if (x + dx > pMe->m_cxEffective) {
         dx = pMe->m_cxEffective - x;
      }
      if (y + dy > pMe->m_dib.cy) {
         dy = pMe->m_dib.cy - y;
      }
      if (0 >= dx || 0 >= dy) {
         return SUCCESS;
      }

      EXPAND_DIRTY_RECT(pMe, x, y, dx, dy);

   } else if (0 >= dx || 0 >= dy) {

      return SUCCESS;

   }

   // draw rectangle
   return FillRect(pMe, x, y, dx, dy, color, rop);
}


static int OEMBitmap_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   IDIB *pdibSrc;

   if (AEEGETPVTBL(pSrc, IBitmap) == AEEGETPVTBL(po, IBitmap)) {
      OEMBitmap *pbmSrc = (OEMBitmap*)pSrc;

      BLT_CLIP_AND_DIRTY(pMe, xDst, yDst, pMe->m_cxEffective, pMe->m_dib.cy,
                         xSrc, ySrc, pbmSrc->m_dib.cx, pbmSrc->m_dib.cy, dx, dy);

      if (dx > 0 && dy > 0) {
         return BlitDdb(pMe, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop);
      }

      return SUCCESS;

   } else if (SUCCESS == IBITMAP_QueryInterface(pSrc, AEECLSID_DIB, (void**)&pdibSrc) ||
              SUCCESS == IBITMAP_QueryInterface(pSrc, AEECLSID_DIB_20, (void**)&pdibSrc)) {
      int nResult = SUCCESS;

      BLT_CLIP_AND_DIRTY(pMe, xDst, yDst, pMe->m_cxEffective, pMe->m_dib.cy,
                         xSrc, ySrc, pdibSrc->cx, pdibSrc->cy, dx, dy);

      // Do the blit if there is still anything to blit.
      if (dx > 0 && dy > 0) {
         nResult = BlitDib(pMe, pdibSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop);
      }

      IDIB_Release(pdibSrc);

      if (nResult == SUCCESS) {
         return SUCCESS;
      }

   }

   return IBITMAP_BltOut(pSrc, xDst, yDst, dx, dy, po, xSrc, ySrc, rop);
}


static int OEMBitmap_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
   OEMBitmap *pMe = (OEMBitmap*)po;
   IDIB *pdibDst = NULL;
   int nResult = EUNSUPPORTED;

   if (AEE_RO_COPY != rop) {
      goto Done;
   }

   if (SUCCESS == IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void**)&pdibDst) ||
       SUCCESS == IBITMAP_QueryInterface(pDst, AEECLSID_DIB_20, (void**)&pdibDst)) {

      if (32 != pdibDst->nDepth || IDIB_COLORSCHEME_888 != pdibDst->nColorScheme) {
         goto Done;
      }

      nResult = SUCCESS;

      CLIP_TO_SRC(xDst, yDst, xSrc, ySrc, pMe->m_dib.cx, pMe->m_dib.cy, dx, dy);
      CLIP_TO_DST(xDst, yDst, pdibDst->cx, pdibDst->cy, xSrc, ySrc, dx, dy);

      // Do the blit if there is still anything to blit.
      if (dx > 0 && dy > 0) {
         uint32 *pd = (uint32*)(pdibDst->pBmp + yDst * pdibDst->nPitch) + xDst;
         uint32 *pd2 = pd;
         int nPitch = pdibDst->nPitch / 4;
#if defined(PIXELITER_SRC_SETUP)
         int dx2;
         PIXELITER_SRC_SETUP(pMe, xSrc, ySrc, FALSE);

         while (dy--) {
            dx2 = dx;
            while (dx2--) {
               *(pd2++) = HTONL(NATIVE_TO_RGB(PIXELITER_SRC_GET()));
               PIXELITER_SRC_NEXTX();
            }
            pd2 = (pd += nPitch);
            PIXELITER_SRC_NEXTY();
         }
#else
         int x, y;

         for (y = ySrc; y < ySrc + dy; y++) {
            for (x = xSrc; x < xSrc + dx; x++) {
               *(pd2++) = HTONL(NATIVE_TO_RGB(GETPIXEL(pMe, x, y)));
            }
            pd2 = (pd += nPitch);
         }
#endif
      }

   }

Done:
   if (NULL != pdibDst) {
      IDIB_Release(pdibDst);
   }
   return nResult;
}


static int OEMBitmap_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   if (sizeof(AEEBitmapInfo) != nSize) {
      return EUNSUPPORTED;
   }

   pInfo->cx = pMe->m_dib.cx;
   pInfo->cy = pMe->m_dib.cy;
   pInfo->nDepth = COLOR_DEPTH;

   return SUCCESS;
}


static int OEMBitmap_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h)
{
   return CREATE_FUNC_NAME(OEMBitmap, FUNCTION_NAME_ID, _New)(w, h, 0, ppIBitmap);
}


static int OEMBitmap_SetTransparencyColor(IBitmap *po, NativeColor color)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   pMe->m_dib.ncTransparent = color;
   return SUCCESS;
}


static int OEMBitmap_GetTransparencyColor(IBitmap *po, NativeColor *pColor)
{
   OEMBitmap *pMe = (OEMBitmap*)po;

   if (pColor) {
      *pColor = pMe->m_dib.ncTransparent;
      return SUCCESS;
   }
   return EBADPARM;
}


/************************************************************************
 **   I B I T M A P D E V   I N T E R F A C E   F U N C T I O N S
 ************************************************************************/

static int OEMBitmap_Update(IBitmapDev *po)
{
   OEMBitmap *pMe = ((OEMBitmapDev*)po)->pMe;

   if (0 != pMe->m_cxEffective) {
      if (pMe->m_pDisplayDev) {
         AEERect rc;

         rc.x = pMe->m_xDirty;
         rc.y = pMe->m_yDirty;
         rc.dx = pMe->m_x2Dirty - pMe->m_xDirty;
         rc.dy = pMe->m_y2Dirty - pMe->m_yDirty;

         if (!pMe->m_bDisableDirty) {
            // reset dirty rectangle (make entire bitmap clean)
            pMe->m_xDirty = pMe->m_cxEffective;
            pMe->m_x2Dirty = 0;
            pMe->m_yDirty = pMe->m_dib.cy;
            pMe->m_y2Dirty = 0;
         }

         return IDISPLAYDEV_Update(pMe->m_pDisplayDev, OEMBITMAP_TO_IBITMAP(pMe), &rc);
      }

      if (pMe->m_pParent) {
         AEERect rc;

         rc.x = pMe->m_xDirty + pMe->m_xOrigin;
         rc.y = pMe->m_yDirty + pMe->m_yOrigin;
         rc.dx = pMe->m_x2Dirty - pMe->m_xDirty;
         rc.dy = pMe->m_y2Dirty - pMe->m_yDirty;

         // merge our dirty rectangle with our parent's
         IBITMAP_Invalidate(pMe->m_pParent, &rc);

         if (!pMe->m_bDisableDirty) {
            // reset dirty rectangle (make entire bitmap clean)
            pMe->m_xDirty = pMe->m_cxEffective;
            pMe->m_x2Dirty = 0;
            pMe->m_yDirty = pMe->m_dib.cy;
            pMe->m_y2Dirty = 0;
         }

         return IBITMAPDEV_Update(pMe->m_pParentDev);
      }
   }

   return SUCCESS;
}


static boolean OEMBitmap_IsEnabled(IBitmapDev *po)
{
   OEMBitmap *pMe = ((OEMBitmapDev*)po)->pMe;
   return (0 != pMe->m_cxEffective);
}


static int OEMBitmap_NotifyEnable(IBitmapDev *po, AEECallback *pcb)
{
   OEMBitmap *pMe = ((OEMBitmapDev*)po)->pMe;

   if (NULL == pcb) {
      return EBADPARM;
   }

   CALLBACK_Cancel(pcb);
   pcb->pfnCancel   = CancelBitmapCallback;
   pcb->pCancelData = &(pMe->m_pcbEnable);
   pcb->pNext = pMe->m_pcbEnable;
   pMe->m_pcbEnable = pcb;
   if (pcb->pNext) {
      pcb->pNext->pCancelData = &(pcb->pNext);
   }

   return SUCCESS;
}


/************************************************************************
 **   I B I T M A P R O T   I N T E R F A C E   F U N C T I O N S
 ************************************************************************/

static int OEMBitmap_GetRotation(IBitmapRot *po)
{
   OEMBitmap *pMe = ((OEMBitmapRot*)po)->pMe;
   return pMe->m_nRotation;
}


/************************************************************************
 **   I B I T M A P C T L   I N T E R F A C E   F U N C T I O N S
 ************************************************************************/

static int OEMBitmap_Enable(IBitmapCtl *po, boolean bOn)
{
   OEMBitmap *pMe = ((OEMBitmapCtl*)po)->pMe;
   AEECallback *pcbList, *pcb;

   if (bOn && !pMe->m_cxEffective) {

      pMe->m_cxEffective = pMe->m_dib.cx;

      // make entire bitmap dirty
      pMe->m_xDirty = 0;
      pMe->m_x2Dirty = pMe->m_dib.cx;
      pMe->m_yDirty = 0;
      pMe->m_y2Dirty = pMe->m_dib.cy;

   } else if (!bOn && pMe->m_cxEffective) {

      pMe->m_cxEffective = 0;
      pMe->m_x2Dirty = 0;

   } else {
      // state did not change
      return SUCCESS;
   }

   //
   // fire callbacks
   //

   // remove callbacks from main list, in case another callback is queued from one of
   // the callbacks we are about to fire
   pcbList = pMe->m_pcbEnable;
   pMe->m_pcbEnable = NULL;
   if (pcbList) {
      pcbList->pCancelData = &pcbList;
   }

   while ((AEECallback*)0 != (pcb = pcbList)) {
      CALLBACK_Cancel(pcb);
      if ((PFNNOTIFY)0 != pcb->pfnNotify) {
         pcb->pfnNotify(pcb->pNotifyData);
      }
   }

   return SUCCESS;
}


static int OEMBitmap_Restrict(IBitmapCtl *po, AEERect *prc)
{
   OEMBitmap *pMe = ((OEMBitmapCtl*)po)->pMe;
   int nErr = EUNSUPPORTED;

   if (prc == NULL) {
      return EBADPARM;
   }

#if defined(RESTRICT_REGION)
   // This will need to be implemented if format is not DIB compatible.
   nErr = RESTRICT_REGION(po->pMe, prc);
#elif (defined(DIB_COMPATIBLE) && (PIXEL_SIZE % 8 == 0))
   pMe->m_dib.pBmp += (pMe->m_dib.nPitch * (prc->y - pMe->m_yOrigin)) + ((prc->x - pMe->m_xOrigin) * (PIXEL_SIZE / 8));
   pMe->m_dib.cx = prc->dx;
   if (pMe->m_cxEffective) {
      pMe->m_cxEffective = prc->dx;
   }
   pMe->m_dib.cy = prc->dy;
   nErr = SUCCESS;
#elif (defined(DIB_COMPATIBLE) && (PIXEL_SIZE < 8) && (8 % PIXEL_SIZE == 0))
   // For efficiency, and to maintain DIB support, we will only support restrictions
   // when x falls on a byte boundry.
   if (prc->x % (8 / PIXEL_SIZE) == 0) {
      pMe->m_dib.pBmp += (pMe->m_dib.nPitch * (prc->y - pMe->m_yOrigin)) + ((prc->x - pMe->m_xOrigin) / (8 / PIXEL_SIZE));
      pMe->m_dib.cx = prc->dx;
      if (pMe->m_cxEffective) {
         pMe->m_cxEffective = prc->dx;
      }
      pMe->m_dib.cy = prc->dy;
      nErr = SUCCESS;
   }
#endif

   if (SUCCESS == nErr) {
      // make entire bitmap dirty
      pMe->m_xDirty = 0;
      pMe->m_x2Dirty = pMe->m_cxEffective;
      pMe->m_yDirty = 0;
      pMe->m_y2Dirty = pMe->m_dib.cy;

      // remember where our offset
      pMe->m_xOrigin = prc->x;
      pMe->m_yOrigin = prc->y;
   }

   return nErr;
}


static int OEMBitmap_NotifyRelease(IBitmapCtl *po, AEECallback *pcb)
{
   OEMBitmap *pMe = ((OEMBitmapCtl*)po)->pMe;

   if (NULL == pcb) {
      return EBADPARM;
   }

   CALLBACK_Cancel(pcb);
   pcb->pfnCancel   = CancelBitmapCallback;
   pcb->pCancelData = &(pMe->m_pcbRelease);
   pcb->pNext = pMe->m_pcbRelease;
   pMe->m_pcbRelease = pcb;
   if (pcb->pNext) {
      pcb->pNext->pCancelData = &(pcb->pNext);
   }

   return SUCCESS;
}


static AEEPoint OEMBitmap_AppToScreen(IBitmapCtl *po, AEEPoint p)
{
   OEMBitmap *pMe = ((OEMBitmapCtl*)po)->pMe;

   p.x += pMe->m_xOrigin;
   p.y += pMe->m_yOrigin;

   if (90 == pMe->m_nRotation) {
      int16 nTmp = p.x;
      p.x = p.y;
      p.y = pMe->m_nFullHeight - nTmp;
   } else if (180 == pMe->m_nRotation) {
      p.x = pMe->m_nFullWidth - p.x;
      p.y = pMe->m_nFullHeight - p.y;
   } else if (270 == pMe->m_nRotation) {
      int16 nTmp = p.x;
      p.x = pMe->m_nFullWidth - p.y;
      p.y = nTmp;
   }

   return p;
}


static AEEPoint OEMBitmap_ScreenToApp(IBitmapCtl *po, AEEPoint p)
{
   OEMBitmap *pMe = ((OEMBitmapCtl*)po)->pMe;

   if (90 == pMe->m_nRotation) {
      int16 nTmp = p.x;
      p.x = pMe->m_nFullHeight - p.y;
      p.y = nTmp;
   } else if (180 == pMe->m_nRotation) {
      p.x = pMe->m_nFullWidth - p.x;
      p.y = pMe->m_nFullHeight - p.y;
   } else if (270 == pMe->m_nRotation) {
      int16 nTmp = p.x;
      p.x = p.y;
      p.y = pMe->m_nFullWidth - nTmp;
   }

   p.x -= pMe->m_xOrigin;
   p.y -= pMe->m_yOrigin;

   return p;
}


/************************************************************************
 **   P A L E T T E   M A P   I M P L E M E N T A T I O N
 ************************************************************************/

static uint32 PaletteMap_AddRef(IQueryInterface *po)
{
   return (((PaletteMap*)po)->m_uRefs)++;
}


static uint32 PaletteMap_Release(IQueryInterface *po)
{
   if (--((PaletteMap*)po)->m_uRefs != 0) {
      return ((PaletteMap*)po)->m_uRefs;
   }

   // Ref count is zero. So, release memory associated with this object.
   FREE(po);

   return 0;
}


static int PaletteMap_QueryInterface(IQueryInterface *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_QUERYINTERFACE) {
      *ppNew = po;
      return SUCCESS;
   }

   *ppNew = 0;
   return EUNSUPPORTED;
}


