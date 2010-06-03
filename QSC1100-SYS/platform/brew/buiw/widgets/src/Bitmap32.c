/*=====================================================================
FILE:  Bitmap32.c

SERVICES:  Bitmap32 implementation

DESCRIPTION:
   IBitmap implementation for 32-bit display, ina  BGRA8888 format (i.e. identical to Brew-sty;le RGB) 

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "Bitmap32.h"
#include "wutil.h"


// Define macros for use by Bitmap_generic.h

#define SETPIXEL(b, x, y, c) \
   (*(uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4) = (c))

#define GETPIXEL(b, x, y) \
   ((NativeColor)(*(uint32*)((b)->m_dib.pBmp + (b)->m_dib.nPitch * (y) + (x) * 4)))

// Internal format is BGRA - which happens to be the same as Brew's RGBVAL type

// Force alpha to 255 if unspecified
#define RGB_TO_NATIVE(r, g, b) \
   ((NativeColor)((  0xff | (r) << 8) | ((g) << 16) | ((b) << 24)))

#define NATIVE_TO_RGB(c) \
   ((RGBVAL)(c))

// Reduce alpha from 0-255 to 0-32 scale.
#define REDUCEALPHA(a)    (((a)*32 + 144) >> 8)


// Setup an interator. 
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

// Advance one pixel across the line. pdw_ is a uint32, so this advances the correct amount.
# define PIXELITER_NEXTX()      { pdw_++; }

// Move up or down a line
// fg = fg_, bg = bg_ gets rid of warnings, and is optimised out by the compiler
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




// For bitmap formats with whole-byte pixels (such as 8) the following macros
// provide more optimization.
# define PIXELITER_SETAT(i,val) (pdw_[i] = (val))
# define PIXELITER_GETAT(i)     (pdw_[i])
# define PIXELITER_ADDX(dx)     (pdw_ += (dx))
# define PIXELITER_SRC_GETAT(i) (pdwSrc_[i])
# define PIXELITER_SRC_ADDX(dx) (pdwSrc_ += (dx))

// We can define these macros for optimal performance later

#if 0
#define FILLRECT_COPY FillRect_Copy

// Assume all parameters are valid and in bounds.
static __inline void FillRect_Copy(Bitmap32 *pbmDst, int x, int y, int dx, int dy, NativeColor color)
{
}
#endif


#if 0
#define FILLRECT_XOR FillRect_XOR

// Assume all parameters are valid and in bounds.
static __inline void FillRect_XOR(Bitmap32 *pbmDst, int x, int y, int dx, int dy, NativeColor color)
{
}
#endif


// Define our own BlitDdb_Copy method. This isn't essential.
#define BLITDDB_COPY BlitDdb_Copy
// Define our own BlendDib method
#define BLENDDIB BlendDib8888

// Macro used by BlendDib
// This routine uses BGRA byte order
#define BLENDPIXEL(srcCol, pDstCol) { \
   uint32  alphaSrc = srcCol & 0xff; \
   if(alphaSrc == 255) { \
      *pDstCol = srcCol; \
   } else  { \
      uint32 alphaDest = *pDstCol & 0xff;   \
      uint32 rbSrc = (srcCol & 0xff00ff00) >> 8; \
      uint32 gSrc = (srcCol & 0xff0000) >> 8; \
      uint32 rbDest = (*pDstCol & 0xff00ff00) >> 8; \
      uint32 gDest = (*pDstCol & 0xff0000) >> 8; \
      /* Blend red and blue */ \
      rbDest = ((rbSrc * 255) + (rbDest * (255 - alphaSrc))) & 0xff00ff00; \
                  \
      /* Blend green */ \
      gDest = ((gSrc * 255) + (gDest * (255 - alphaSrc))) & 0xff0000; \
                  \
      /* Calculate final alpha */ \
      alphaDest = (alphaSrc << 8) + alphaDest * (255 - alphaSrc); \
      alphaDest = alphaDest >> 8; \
      *pDstCol = rbDest | gDest | alphaDest; \
   } \
}     
       

// Assume all parameters are valid and in bounds.
static __inline void BlitDdb_Copy(Bitmap32 *pbmDst, Bitmap32 *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc)
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
      MEMMOVE(pbDst, pbSrc, dxBytes);
      pbDst += nPitchDst;
      pbSrc += nPitchSrc;
   }
}


static __inline int BlendDib8888Palettised(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                                           int xSrc, int ySrc)
{
   byte *pby= pbmSrc->pBmp + (ySrc * pbmSrc->nPitch) + ((xSrc * pbmSrc->nDepth) >> 3);
   int x, y;
   int nAdjust = pbmSrc->nPitch - dx;

   // We only support blitting from an 8 bit source
   if(pbmSrc->nDepth != 8)
   {
      DBGPRINTF("**** only support 8bpp palettised");
      return EUNSUPPORTED;
   }

   if(pbmSrc->cntRGB > 256)
   {
      DBGPRINTF("**** more than 256 palette entries not support");
      return EUNSUPPORTED;
   }

   for(y = 0; y < dy; ++y) {
      for(x = 0; x < dx; ++x) {   
         uint8 pEntry = *pby;
         // Get the src col, in BGRx format - would need to convert this on BE architecture.
         // The Brew documentation says that pRGB[pEntry] is BE. It isn't!!
         uint32 clrSrc = NTOHL(pbmSrc->pRGB[pEntry]);
         // Get dest color - we operate in BGRA, so that's easy
         uint32 clrDst = GETPIXEL(pbmDst, x + xDst, y + yDst);
         BLENDPIXEL(clrSrc, &clrDst);

         SETPIXEL(pbmDst, x + xDst, y + yDst, clrDst);
         pby++;
      }
      pby += nAdjust;
   }

   return SUCCESS;
}



static __inline int BlendDib8888Unpalettised(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                                             int xSrc, int ySrc)
{
   byte *pby= pbmSrc->pBmp + (ySrc * pbmSrc->nPitch) + ((xSrc * pbmSrc->nDepth) >> 3);
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int nAdjust = pbmSrc->nPitch - (dx * 4);
   int x, y;

   // Must be 32bpp
   if(pbmSrc->nDepth != 32)
      return EUNSUPPORTED;

   for (y = yDst; y < yDstMax; ++y) {
      for (x = xDst; x < xDstMax; ++x) {
         uint32 clrSrc = *(uint32*)pby;
         uint32 clrDst = GETPIXEL(pbmDst, x, y);
         
         BLENDPIXEL(clrSrc, &clrDst);

         SETPIXEL(pbmDst, x, y, clrDst);
         pby += 4;
      }
      pby += nAdjust;
   }

   return SUCCESS;
}

static __inline int BlendDib8888(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                                int xSrc, int ySrc)
{
   // Check for palettised
   if(pbmSrc->cntRGB != 0)
      return BlendDib8888Palettised(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
   else
      return BlendDib8888Unpalettised(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
}




/*=====================================================================
FILE:  Bitmap_generic.h

SERVICES:  Bitmap reference implementation

DESCRIPTION:
   Bit depth independent reference IBitmap implementation.  The
   implementation in this file is built on the macros defined in
   Bitmap32N.c, where N is the bit depth.  The file is included
   from BitmapN.c.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable

        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/


//
// Various macros for Bitmap
//

#undef ASSERT
#define ASSERT(x) if (!(x)) { DBGPRINTF("assertion failed ("__FILE__":%d): \"%s\"\n",__LINE__,#x); }


//
// Dirty rectangle macros
//

#define EXPAND_DIRTY_RECT(pMe, x, y, dx, dy)                                        \
do {                                                                                \
   (pMe)->m_xDirty  = MIN((pMe)->m_xDirty,  (x));                                   \
   (pMe)->m_x2Dirty = MAX((pMe)->m_x2Dirty, (x) + (dx));                            \
   (pMe)->m_yDirty  = MIN((pMe)->m_yDirty,  (y));                                   \
   (pMe)->m_y2Dirty = MAX((pMe)->m_y2Dirty, (y) + (dy));                            \
} while (0)

#define IS_OUTSIDE_DIRTY_RECT(pMe, x, y, dx, dy)                                    \
   ((x) < (pMe)->m_xDirty || (x) + (dx) > (pMe)->m_x2Dirty ||                       \
    (y) < (pMe)->m_yDirty || (y) + (dy) > (pMe)->m_y2Dirty)


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


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 Bitmap32_AddRef(IBitmap *po);
static uint32 Bitmap32_Release(IBitmap *po);
static int Bitmap32_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew);
static NativeColor Bitmap32_RGBToNative(IBitmap *po, RGBVAL rgb);
static RGBVAL Bitmap32_NativeToRGB(IBitmap *po, NativeColor color);
static int Bitmap32_DrawPixel(IBitmap *po, unsigned x, unsigned y, NativeColor color, AEERasterOp rop);
static int Bitmap32_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor);
static int Bitmap32_SetPixels(IBitmap *po, unsigned cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop);
static int Bitmap32_DrawHScanline(IBitmap *po, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop);
static int Bitmap32_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop);
static int Bitmap32_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop);
static int Bitmap32_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
static int Bitmap32_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize);
static int Bitmap32_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h);
static int Bitmap32_SetTransparencyColor(IBitmap *po, NativeColor color);
static int Bitmap32_GetTransparencyColor(IBitmap *po, NativeColor *pColor);



// PixelType = smallest type that can hold all of a pixel

typedef uint32 PixelType;


// Palette map, for caching palette conversions.
OBJECT(PaletteMap) {
   AEEVTBL(IQueryInterface) *pvt;
   uint32         m_uRefs;
   PixelType     *m_pColors;
   boolean        m_bDDB;
};

static uint32 PaletteMap_AddRef(IQueryInterface *po);
static uint32 PaletteMap_Release(IQueryInterface *po);
static int    PaletteMap_QueryInterface(IQueryInterface *pMe, AEECLSID clsid, void **ppNew);



int Bitmap32_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap)
{
   Bitmap32 *pMe = NULL;

   if (NULL != pBuf) {

      pMe = MALLOC(sizeof(Bitmap32));

   } else {

      pMe = MALLOC(sizeof(Bitmap32) + w * h * 4);
      pBuf = pMe+1;

   }

   if ((Bitmap32 *)0 == pMe) {
      return ENOMEMORY;
   }


   pMe->m_dib.pvt = MALLOC(sizeof(AEEVTBL(IBitmap)));
   pMe->m_dib.pvt->AddRef = Bitmap32_AddRef;
   pMe->m_dib.pvt->Release = Bitmap32_Release;
   pMe->m_dib.pvt->QueryInterface = Bitmap32_QueryInterface;
   pMe->m_dib.pvt->RGBToNative = Bitmap32_RGBToNative;
   pMe->m_dib.pvt->NativeToRGB = Bitmap32_NativeToRGB;
   pMe->m_dib.pvt->DrawPixel = Bitmap32_DrawPixel;
   pMe->m_dib.pvt->GetPixel = Bitmap32_GetPixel;
   pMe->m_dib.pvt->SetPixels = Bitmap32_SetPixels;
   pMe->m_dib.pvt->DrawHScanline = Bitmap32_DrawHScanline;
   pMe->m_dib.pvt->FillRect = Bitmap32_FillRect;
   pMe->m_dib.pvt->BltIn = Bitmap32_BltIn;
   pMe->m_dib.pvt->BltOut = Bitmap32_BltOut;
   pMe->m_dib.pvt->GetInfo = Bitmap32_GetInfo;
   pMe->m_dib.pvt->CreateCompatibleBitmap = Bitmap32_CreateCompatibleBitmap;
   pMe->m_dib.pvt->SetTransparencyColor = Bitmap32_SetTransparencyColor;
   pMe->m_dib.pvt->GetTransparencyColor = Bitmap32_GetTransparencyColor;

   pMe->m_uRefs = 1;

   pMe->m_dib.nDepth = 32;
   pMe->m_dib.nColorScheme = IDIB_COLORSCHEME_8888;
   pMe->m_dib.cx = w;
   pMe->m_dib.cy = h;
   // Calculate number of bytes per row
   pMe->m_dib.nPitch = w * 4;
   pMe->m_dib.pBmp = (byte*)pBuf;

   // initially, entire bitmap is dirty
   pMe->m_xDirty = 0;
   pMe->m_yDirty = 0;
   pMe->m_x2Dirty = w;
   pMe->m_y2Dirty = h;

   *ppIBitmap = BITMAP32_TO_IBITMAP(pMe);
   return SUCCESS;
}



/************************************************************************
 ** H E L P E R   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/



static __inline RopPixelSupports(AEERasterOp rop)
{
   return (rop == AEE_RO_COPY     ||
           rop == AEE_RO_XOR      ||
           rop == AEE_RO_NOT      ||
           rop == AEE_RO_OR       ||
           rop == AEE_RO_MERGENOT ||
           rop == AEE_RO_ANDNOT);
}


static __inline void RopPixel(Bitmap32 *pbmDst, unsigned x, unsigned y, NativeColor color, AEERasterOp rop)
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


//--------  ALPHA BLENDING
//
static __inline int FillRect(Bitmap32 *pbmDst, int x, int y, int dx, int dy, NativeColor color, AEERasterOp rop)
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


static int BlitDdb(Bitmap32 *pbmDst, Bitmap32 *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc, AEERasterOp rop)
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
   asdas
   if (rop == AEE_RO_TRANSPARENT) {
      BLITDDB_TRANSPARENT(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
      return SUCCESS;
   }
#else
   if (rop == AEE_RO_TRANSPARENT && !bRevX) {
      NativeColor ncTransparent = pbmSrc->m_dib.ncTransparent;
      NativeColor nc;
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, bRevY);
      PIXELITER_SRC_SETUP(pbmSrc, xSrc, ySrc, bRevY);
      while (dy--) {
         int cx = dx;

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
#else 
   if (rop == AEE_RO_XOR && !bRevX) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, bRevY);
      PIXELITER_SRC_SETUP(pbmSrc, xSrc, ySrc, bRevY);
      while (dy--) {
         int cx = dx;

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


# define SCHEME332_TO_NATIVE(v) RGB_TO_NATIVE( (((v) & 0xE0) >> 3) * 9,  /* 6 bits is enough */ \
                                               ((v) & 0x1C) * 9,         \
                                               ((v) & 0x03) * 0x55 )


// Contruct a palette map if possible, unless DIB is DDB-compatible.
//
// The palette map contains an array of NativeColors that is allocated to a size
// of 1<<nDepth, regardless of the number of colors defined in the bitmap.
//
static __inline boolean InitPaletteMap(IDIB *pbmSrc, boolean *pbCompatible)
{
   // if already created, nothing to do

   // This method used to compare the global vtable ptr. We can't do that, because we don't have a global vtable.
   // Instead, compare the QI ptr instead.
   if (pbmSrc->pPaletteMap
       && AEEGETPVTBL(pbmSrc->pPaletteMap, IQueryInterface)->QueryInterface == PaletteMap_QueryInterface) {
      *pbCompatible = ((PaletteMap*)pbmSrc->pPaletteMap)->m_bDDB;
      return TRUE;
   }

   // if DDB-compatible scheme, no need to create palette map
   if (pbmSrc->nDepth == 32 && pbmSrc->nColorScheme == IDIB_COLORSCHEME_8888) {
      *pbCompatible = TRUE;
      return FALSE;
   }

   // can we construct a palette map for this?

   if ( (BETWEEN(pbmSrc->nDepth, 1, 9) &&
         (pbmSrc->cntRGB > 0 || pbmSrc->nColorScheme == IDIB_COLORSCHEME_332)) ) {

      int cntColors = (1 << pbmSrc->nDepth);
      int cntRGB = MIN(pbmSrc->cntRGB, cntColors);
      int ndx;
      PaletteMap *pMap;

      IDIB_FlushPalette(pbmSrc);

      pMap = (PaletteMap*) MALLOCREC_EX(PaletteMap, sizeof(AEEVTBL(IQueryInterface)) + (sizeof(PixelType) * cntColors));

      if (!pMap) {
         *pbCompatible = FALSE;
         return FALSE;
      }

      pMap->pvt = GETVTBL(pMap, IQueryInterface);
      pMap->pvt->AddRef  = PaletteMap_AddRef;
      pMap->pvt->Release = PaletteMap_Release;
      pMap->pvt->QueryInterface = PaletteMap_QueryInterface;
    
      pMap->m_pColors = (PixelType*)(pMap->pvt + 1);
      pMap->m_uRefs = 1;

      pbmSrc->pPaletteMap = (IQueryInterface*)pMap;

      if (pbmSrc->nColorScheme == IDIB_COLORSCHEME_332) {

         // construct from 3-3-2 bit fields
         for (ndx = 255; ndx >= 0; --ndx) {
            pMap->m_pColors[ndx] = (PixelType) SCHEME332_TO_NATIVE(ndx);
         }

      } else {

         uint32 *pRGB = pbmSrc->pRGB;

         for (ndx = cntRGB; --ndx >= 0; ) {
            uint32 rgb = pRGB[ndx];
            pMap->m_pColors[ndx] = (PixelType) RGB_TO_NATIVE((rgb >> 16) & 0xFF,
                                                             (rgb >> 8) & 0xFF,
                                                              rgb & 0xFF);
         }
         if (pbmSrc->nDepth == 32) {
            // compatible => same depth && palette is identity function

            pMap->m_bDDB = TRUE;

            // note: check only those colors defined in the bitmap; others are undefined
            for (ndx = cntRGB; ndx >= 0; --ndx) {
               if (pMap->m_pColors[ndx] != (NativeColor)ndx) {
                  pMap->m_bDDB = FALSE;
                  break;
               }
            }
         }
      }

      *pbCompatible = pMap->m_bDDB;
      return TRUE;
   }

   *pbCompatible = FALSE;
   return FALSE;
}




// BlitDib1 : Copy from 1-bit DIB
//
static __inline int BlitDib1(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int yDstMax = yDst + dy;
   int x, y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, pColors[1], pColors[0], FALSE);

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
      PIXELITER_SETUP(pbmDst, xDst, yDst, pColors[byInv == 0], 0, FALSE);

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
                  SETPIXEL(pbmDst, x, y, ((byBit & by) ? pColors[1] : pColors[0]));
               }
            } else {
               RopPixel(pbmDst, x, y, ((byBit & by) ? pColors[1] : pColors[0]), rop);
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
static __inline int BlitDib2(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x,y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;


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
static __inline int BlitDib4(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;

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
static __inline int BlitDib8(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
                             int xSrc, int ySrc, AEERasterOp rop, byte *pbyBase)
{
   int xDstMax = xDst + dx;
   int yDstMax = yDst + dy;
   int x, y;
   PixelType *pColors = ((PaletteMap*)pbmSrc->pPaletteMap)->m_pColors;
   byte *pby = pbyBase;
   int   nAdjust = pbmSrc->nPitch - (xDstMax - xDst);


   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
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
static __inline int BlitDib555(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
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

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
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
static __inline int BlitDib565(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
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

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
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
static __inline int BlitDib888(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy,
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

   if (rop == AEE_RO_COPY) {
      PIXELITER_SETUP(pbmDst, xDst, yDst, 0, 0, FALSE);

      for (y = yDst; y < yDstMax; ++y) {
         int cx = xDstMax - xDst;
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


static int BlitDib(Bitmap32 *pbmDst, IDIB *pbmSrc, int xDst, int yDst, int dx, int dy, int xSrc, int ySrc, AEERasterOp rop)
{
   boolean bCompatible, bMapped;
   byte *pbyBase;

#ifdef BLENDDIB
   if (rop == AEE_RO_BLEND) {
      return BLENDDIB(pbmDst, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc);
   }
#endif

   bMapped = InitPaletteMap(pbmSrc, &bCompatible);
   if (bCompatible) {
      //                      @@@@@@ arghhhh!!!!
      return BlitDdb(pbmDst, (Bitmap32*)pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop);
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

static uint32 Bitmap32_AddRef(IBitmap *po)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   return ++(pMe->m_uRefs);
}


static uint32 Bitmap32_Release(IBitmap *po)
{
   Bitmap32 *pMe = (Bitmap32*)po;
   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }


   if (pMe->m_dib.pPaletteMap) {
      IQI_Release(pMe->m_dib.pPaletteMap);
   }


   // Ref count is zero. So, release memory associated with this object.
   FREE(pMe->m_dib.pvt);
   FREE(pMe);

   return 0;
}


static int Bitmap32_QueryInterface(IBitmap *po, AEECLSID clsid, void **ppNew)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   if (clsid == AEECLSID_BITMAP || clsid == AEECLSID_QUERYINTERFACE) {
      // I am an IBitmap/IQueryInterface.
      *ppNew = (void*)pMe;
      Bitmap32_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEECLSID_DIB) {
      *ppNew = &pMe->m_dib;
      Bitmap32_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEECLSID_DIB_20) {
      *ppNew = &pMe->m_dib;
      Bitmap32_AddRef(po);

      // make entire bitmap permanently dirty
      pMe->m_xDirty = 0;
      pMe->m_x2Dirty = pMe->m_dib.cx;
      pMe->m_yDirty = 0;
      pMe->m_y2Dirty = pMe->m_dib.cy;

      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}


static NativeColor Bitmap32_RGBToNative(IBitmap *po, RGBVAL rgb)
{
   // Take alpha channel to 255
   return (NativeColor)(rgb |0xff);
}


static RGBVAL Bitmap32_NativeToRGB(IBitmap *po, NativeColor color)
{
   return NATIVE_TO_RGB(color);
}


static int Bitmap32_DrawPixel(IBitmap *po, unsigned int x, unsigned int y, NativeColor color, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   if (((signed)x < pMe->m_xDirty) || ((signed)x >= pMe->m_x2Dirty) ||
       ((signed)y < pMe->m_yDirty) || ((signed)y >= pMe->m_y2Dirty)) {

      if (x >= pMe->m_dib.cx || y >= pMe->m_dib.cy) {
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

   case AEE_RO_BLEND:
      {
         NativeColor dstCol = GETPIXEL(pMe, x, y);
         BLENDPIXEL(color, &dstCol);
         SETPIXEL(pMe, x, y, dstCol);
      }
      break;

   case AEE_RO_TRANSPARENT:

      return SUCCESS;
   default:
      return EUNSUPPORTED;
   }

   return SUCCESS;
}


static int Bitmap32_GetPixel(IBitmap *po, unsigned x, unsigned y, NativeColor *pColor)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   if (x >= pMe->m_dib.cx || y >= pMe->m_dib.cy || pColor == NULL) {
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
static int Bitmap32_SetPixels(IBitmap *po, unsigned cntPoints, AEEPoint *pPoints, NativeColor color, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;
   unsigned cnt = cntPoints;
   AEEPoint *pp = pPoints;
   unsigned xDirty = pMe->m_xDirty;
   unsigned yDirty = pMe->m_yDirty;
   unsigned dxDirty = pMe->m_x2Dirty - xDirty;
   unsigned dyDirty = pMe->m_y2Dirty - yDirty;
   unsigned xMax = pMe->m_dib.cx;
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
            dxDirty = pMe->m_dib.cx;                     \
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


static int Bitmap32_DrawHScanline(IBitmap *po, unsigned yParm, unsigned xMinParm, unsigned xMaxParm, NativeColor color, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;
   int y = (int)yParm;
   int xMin = (int)xMinParm;
   int xMax = (int)xMaxParm;


   // compare to dirty rectangle
   if (IS_OUTSIDE_DIRTY_RECT(pMe, xMin, y, xMax - xMin + 1, 1)) {

      // clip to bitmap boundries
      if (xMin < 0) {
         xMin = 0;
      }
      if (xMax >= pMe->m_dib.cx) {
         xMax = pMe->m_dib.cx - 1;
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


static int Bitmap32_FillRect(IBitmap *po, const AEERect *pRect, NativeColor color, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;
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
      if (x + dx > pMe->m_dib.cx) {
         dx = pMe->m_dib.cx - x;
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


static int Bitmap32_BltIn(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;
   IDIB *pdibSrc;

   if (AEEGETPVTBL(pSrc, IBitmap) == AEEGETPVTBL(po, IBitmap)) {
      Bitmap32 *pbmSrc = (Bitmap32*)pSrc;

      BLT_CLIP_AND_DIRTY(pMe, xDst, yDst, pMe->m_dib.cx, pMe->m_dib.cy,
                         xSrc, ySrc, pbmSrc->m_dib.cx, pbmSrc->m_dib.cy, dx, dy);

      if (dx > 0 && dy > 0) {
         return BlitDdb(pMe, pbmSrc, xDst, yDst, dx, dy, xSrc, ySrc, rop);
      }

      return SUCCESS;

   } else if (SUCCESS == IBITMAP_QueryInterface(pSrc, AEECLSID_DIB, (void**)&pdibSrc) ||
              SUCCESS == IBITMAP_QueryInterface(pSrc, AEECLSID_DIB_20, (void**)&pdibSrc)) {
      int nResult = SUCCESS;

      BLT_CLIP_AND_DIRTY(pMe, xDst, yDst, pMe->m_dib.cx, pMe->m_dib.cy,
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


static int Bitmap32_BltOut(IBitmap *po, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
   Bitmap32 *pMe = (Bitmap32*)po;
   IDIB *pdibDst = NULL;
   int nResult = EUNSUPPORTED;

   if (AEE_RO_COPY != rop) {
      goto Done;
   }

   if (SUCCESS == IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void**)&pdibDst) ||
       SUCCESS == IBITMAP_QueryInterface(pDst, AEECLSID_DIB_20, (void**)&pdibDst)) {

      if (32 != pdibDst->nDepth || IDIB_COLORSCHEME_8888 != pdibDst->nColorScheme) {
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
         int dx2;
         PIXELITER_SRC_SETUP(pMe, xSrc, ySrc, FALSE);

         while (dy--) {
            dx2 = dx;
            while (dx2--) {
               *(pd2++) = NATIVE_TO_RGB(PIXELITER_SRC_GET());
               PIXELITER_SRC_NEXTX();
            }
            pd2 = (pd += nPitch);
            PIXELITER_SRC_NEXTY();
         }
      }

   }

Done:
   if (NULL != pdibDst) {
      IDIB_Release(pdibDst);
   }
   return nResult;
}


static int Bitmap32_GetInfo(IBitmap *po, AEEBitmapInfo *pInfo, int nSize)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   if (sizeof(AEEBitmapInfo) != nSize) {
      return EUNSUPPORTED;
   }

   pInfo->cx = pMe->m_dib.cx;
   pInfo->cy = pMe->m_dib.cy;
   pInfo->nDepth = 32;

   return SUCCESS;
}


static int Bitmap32_CreateCompatibleBitmap(IBitmap *po, IBitmap **ppIBitmap, uint16 w, uint16 h)
{
   return Bitmap32_New(w, h, 0, ppIBitmap);
}


static int Bitmap32_SetTransparencyColor(IBitmap *po, NativeColor color)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   pMe->m_dib.ncTransparent = color;
   return SUCCESS;
}


static int Bitmap32_GetTransparencyColor(IBitmap *po, NativeColor *pColor)
{
   Bitmap32 *pMe = (Bitmap32*)po;

   if (pColor) {
      *pColor = pMe->m_dib.ncTransparent;
      return SUCCESS;
   }
   return EBADPARM;
}


int Bitmap32_BltOutTransparent(IBitmap* pSrc, int xDst, int yDst, int aSrcW, int aSrcH, IBitmap* pDst, int xSrc, int ySrc, int opacity)
{
   // Clip the destination and source....
   AEEBitmapInfo info;
	IDIB* iDibSrc;
   IDIB* iDibDst;
   int   sourceWidth;
   unsigned int x, y;
   uint32* pBmp;


   IBITMAP_GetInfo(pSrc, &info, sizeof(AEEBitmapInfo));
   sourceWidth = info.cx;

   CLIP_TO_SRC(xDst, yDst, xSrc, ySrc, (uint16)info.cx, (uint16)info.cy, aSrcW, aSrcH);
   IBITMAP_GetInfo(pDst, &info, sizeof(AEEBitmapInfo));
   CLIP_TO_DST(xDst, yDst, (uint16)info.cx, (uint16)info.cy, xSrc, ySrc, aSrcW, aSrcH);
  

   

	// Get a DIB to use from the destination and source
	IBITMAP_QueryInterface( pDst, AEECLSID_DIB, (void**)&iDibDst);
	IBITMAP_QueryInterface( pSrc, AEECLSID_DIB, (void**)&iDibSrc);

   // Check the src type
   if (!iDibSrc || !iDibDst || (iDibSrc->nColorScheme != IDIB_COLORSCHEME_8888)) {
      return EUNSUPPORTED;
   }

   // we know the colorscheme is 8888, make sure same depth and use the optimized BlendBlit
   if (iDibDst->nColorScheme == iDibSrc->nColorScheme) {
      int nErr = 0;
      AEERect rect;
      
      rect.x = xDst;
      rect.y = yDst;
      rect.dx = aSrcW;
      rect.dy = aSrcH;
      nErr = BlendBlit(pDst, &rect, pSrc, xSrc, ySrc, opacity);

      RELEASEIF(iDibSrc);
      RELEASEIF(iDibDst);     

      return nErr;    
   }

    // Go through the x and y, and blend the sucker
	
    pBmp = (uint32*)iDibSrc->pBmp;
    for (y = 0; y < (unsigned int)aSrcH; ++y)
    {
        // Set the location in our source bitmap
        // NB we need to use the full sourceWidth as the
        // row stride and not the clipped one!
        // TODO : Check source reference to make sure it isn't out-of-bounds!
        uint32* tempSource = &pBmp[((y + ySrc) * sourceWidth) + xSrc];

        for (x = 0; x < (unsigned int)aSrcW; ++x)
        {
            // Get the next source pixel as RGBA8888
            uint32 srcColor = *tempSource++;

            
            // Skip loading the destination at all if the source weight is
            // zero (and hence we're leaving the destination pixel untouched).
            if ((srcColor & 0xff) != 0)
            {
                uint32 outPixel;

                // Get alpha
                uint32 srcAlpha = srcColor & 0xFF;                
                //uint16 rgb565;
                //uint16* pPixel;


                // Calculate the srcAlpha by multiplying by the opacity.
                // (srcAlpha + opacity) biases the end result so we end up on a scale of 0-256.
                srcAlpha = ((opacity * srcAlpha + ((srcAlpha + opacity) >> 1))) >> 8;
                
                if (srcAlpha == 0xff)
                {
                    outPixel = srcColor;
                }
                else 
                {
                    // Get the destination colour from RGB565
                    NativeColor nColor;
                    RGBVAL destColor;
                    IBITMAP_GetPixel( pDst,
                                      x + xDst,
                                      y + yDst,
                                      &nColor );
                    destColor = IBITMAP_NativeToRGB( pDst,
                                                     nColor );
 
                    // Format of RGBVAL (which is a uint32) is
                    // BBGGRRxx, as found in the definition of MAKE_RGB
                    // in the (crap) BREW documentation

#if 0
                    // ALGCOMMENT I claim this code is more accurate... (see ALGCOMMENT below)
                    if(srcAlpha == 0)
                    {
                       outPixel = destColor;
                    }
                    else
                    {
                       uint32 bias;
                       // Calculate the destination weight
                       uint32 dest_weight = 255 - srcAlpha;


                       // We don't need the bottom 8 bits of either dest or source
                       destColor >>= 8;
                       srcColor  >>= 8;

                       // We now have BGR888 values                    
                    
                       // Calculate the BR components - do green separately
                       bias = (srcColor & 0xff00ff)  | srcAlpha | srcAlpha << 16;
                       outPixel = (
                                   ((srcColor & 0xff00ff) * srcAlpha)
                                   + ((destColor & 0xff00ff) * dest_weight)
                                   + bias
                                   ) & 0xff00ff00;

                       // Add in the green component
                       bias = (srcColor & 0xff00) |  srcAlpha << 8;
                       outPixel |= (((srcColor & 0x00ff00) * srcAlpha)
                                    + ((destColor & 0x00ff00) * dest_weight)
                                    + bias
                                    ) & 0x00ff0000;
                    }

#else
                // ALGCOMMENT (see ALGCOMMENT above) But this is what BlendWidget does under the hood, 
                // and to maintain backwards compatibility in tests, we'll do the same
                    srcAlpha = REDUCEALPHA(srcAlpha);
                    if(srcAlpha == 0)
                    {
                       outPixel = destColor;
                    }
                    else
                    {

                       uint32 nAlphaLo = srcAlpha;
                       uint32 nBetaLo = 32 - nAlphaLo;
                       uint32 ubr, vbr, ug, vg;

                       // Bottom 8 bits not needed
                       srcColor >>= 8;
                       destColor >>= 8;

                       ubr = srcColor & 0xFF00FF;
                       ubr *= nAlphaLo;
 
                       vbr = destColor & 0xFF00FF;
                       vbr *= nBetaLo;

                       ubr += vbr + 0x00100010;

                       ug = srcColor & 0x00FF00;
                       ug *= nAlphaLo;
 
                       vg = destColor & 0x00FF00;
                       vg *= nBetaLo;

                       ug += vg + 0x00001000;

                       outPixel = (ubr << 3) & 0xFF00FF00;
                       outPixel |= (ug << 3) & 0x00FF0000;

                    }
#endif
               }
					// outPixel is BGR888x

               // Could optimise RGB565 by using the DIB directly 
               /*
               // Need to switch on 
      			// For blue, take the top 5 bits of BGR888X, and shift down 27 places so they become LSBs
					rgb565 = (uint16)(outPixel >> 27); 
		   		// For green, take the top 6 bits of the second 8 bits of BGR888, shift down 18 places,
					// then back left 5 places. AKA shift down 13 places and & with %111111 << 5
					rgb565 |= ((outPixel >> 13) & 0x7e0); 
					// For red, take the top 5 bits of the third 8 bits of BGR888, shift down 11 places, then
					// back left 11. AKA, don't shift,& with %11111 << 11
					rgb565 |= (outPixel & 0xf800); //red
               
					// Plot it
			   	pPixel = (uint16*)(iDibDst->pBmp + iDibDst->nPitch * (y+aDstY) + 2 * (x+aDstX));
               *pPixel = rgb565
               */

               // Generic solution for everyone
               IBITMAP_DrawPixel(pDst, x + xDst, y + yDst, IBITMAP_RGBToNative(pDst, (RGBVAL)outPixel), AEE_RO_COPY);
            }
         }
    }
	

	IDIB_Release(iDibSrc);
   IDIB_Release(iDibDst);
   return SUCCESS;
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


