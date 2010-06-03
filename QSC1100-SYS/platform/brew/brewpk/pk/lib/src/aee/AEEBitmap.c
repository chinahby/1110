/*======================================================
FILE:      AEEBitmap.c

SERVICES:  BREW DIB object

GENERAL DESCRIPTION:

    A DIB is an object that supports the IBitmap and IDIB interfaces.  This
    module implements the DIB class that BREW uses for bitmap resources and
    DIBs created via IDISPLAY_CreateDIBitmap().

PUBLIC CLASSES AND STATIC FUNCTIONS:

    See AEEBitmap_priv.h.

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "OEMHeap.h"
#include "AEEBitmap.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEBitmap_priv.h"

//------------------------------------
//--------  Helper functions  --------
//------------------------------------


#define ROUNDUP32(n)        (((n) + 31) & ~31)
#define ROUNDUP4(n)         (((n) + 3) & ~3)

#define ISUSHORT(v)         ( (int32) (uint16) (v) == (int32) (v) )
#define ISSHORT(v)          ( (int32) (int16) (v) == (int32) (v) )

// POINTEROFDOOM: We use this in cases where the pointer should not be
// dereferenced or freed.  We hope that reading or writing to this pointer
// will generate an exception on Windows.  We don't use NULL because:
//
//   1. NULL might act as a crutch for apps that really they should be
//   checking bounds or result codes elsewhere.
//
//   2. FREE(NULL) will not generate any errors, and fail to catch potentially
//   dangerous problem.
//
//   3. If an app does mistakenly write to this pointer or nearby memory, NULL
//   is not a particularly safe address.
//
#define POINTEROFDOOM     ((void*)0xFEFFFFFF)


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

// right now, all bit depth use the same functions, since these are mostly
// not implemented

static uint32 DIB_AddRef(IBitmap *po);
static uint32 DIB_Release(IBitmap *po);
static int DIB_QueryInterface(IBitmap *pMe, AEECLSID clsid, void **ppNew);
static NativeColor DIB_RGBToNative(IBitmap *pMe, RGBVAL rgb);
static RGBVAL      DIB_NativeToRGB(IBitmap *pMe, NativeColor clr);
static int DIB_DrawPixel(IBitmap *pMe, unsigned x, unsigned y, NativeColor color, AEERasterOp rop);
static int DIB_GetPixel(IBitmap *pMe, unsigned x, unsigned y, NativeColor *pColor);
static int DIB_SetPixels(IBitmap *pMe, unsigned int cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop);
static int DIB_DrawHScanline(IBitmap *pMe, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop);
static int DIB_FillRect(IBitmap *pMe, const AEERect *pRect, NativeColor color, AEERasterOp rop);
static int DIB_BltIn(IBitmap *pMe, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop);
static int DIB_BltOut(IBitmap *pMe, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop);
static int DIB_GetInfo(IBitmap *pMe, AEEBitmapInfo *pInfo, int nSize);
static int DIB_CreateCompatibleBitmap(IBitmap *pMe, IBitmap **ppIBitmap, uint16 w, uint16 h);
static int DIB_SetTransparencyColor(IBitmap *pMe, NativeColor color);
static int DIB_GetTransparencyColor(IBitmap *pMe, NativeColor *pColor);



static const VTBL(IBitmap) gDIBFuncs = {
   DIB_AddRef,
   DIB_Release,
   DIB_QueryInterface,
   DIB_RGBToNative,
   DIB_NativeToRGB,
   DIB_DrawPixel,
   DIB_GetPixel,
   DIB_SetPixels,
   DIB_DrawHScanline,
   DIB_FillRect,
   DIB_BltIn,
   DIB_BltOut,
   DIB_GetInfo,
   DIB_CreateCompatibleBitmap,
   DIB_SetTransparencyColor,
   DIB_GetTransparencyColor
};



#define STATE_HEADER  (0)
#define STATE_GAP     (1)
#define STATE_DATA    (2)


struct IBitmap {
   IDIB              m_bmp;     // we share the vtable with m_bmp (IDIB)
   uint32            m_uRefs;
   PFNREALLOC_DIB    m_pfnRealloc;
};


#define PALLOCREC_EX(pfn, type, extra)   ( (type *) ((pfn)(0, sizeof(type) + (extra))) )

static void * DIB_Realloc(void * pOrig, uint32 dwSize)
{
   return(REALLOC(pOrig,dwSize));
}

// Construct a DIB of arbitrary width, height, and pixel size.  Memory for
// pixel data and palette data is allocated (owned by the DIB object).
// Rows are padded for 32-bit alignment.
//
// On success:
//   return value = SUCCESS
//   ppIDIB = address of resulting IDIB pointer  [may NOT be null]
//
// On failure:
//   An error code is returned and &ppIDIB is unmodified.
//
// Note:
//
// - Any palette information (or pixel structure, i.e. 5-5-5 vs. 5-6-5) can be
//   adjusted after the bitmap is returned.
//
// - A top-down bitmap (positive pitch) is constructed.  This could be
//   reversed after construction by modifying the nPitch and pBmp members of
//   IDIB.
//
int
DIB_NewEx(int nPixelSize, int32 w, int32 h, int cntColors, PFNREALLOC_DIB pfnRealloc, IDIB **ppIDIB)
{
   IBitmap *pMe;
   int32 cbColors = cntColors * sizeof(uint32);
   int32 cbRow;

   if (!pfnRealloc)
      pfnRealloc = DIB_Realloc;

   *ppIDIB = NULL;

   cbRow = ROUNDUP32(w * nPixelSize) / 8;

   // Validate values and make sure they mean the same thing in their new 16-bit home
   
   if (nPixelSize < 0 ||
       cntColors < 0  ||
       !BETWEEN(w, 0, 0x10000) ||
       !BETWEEN(h, 0, 0x10000) ||
       !ISSHORT(cbRow))
      return EBADPARM;
   
   // We need enough memory for IDIB struct plus raw bitmap
   // plus 63 bytes so we can align pixel buffer on 64-byte boundry.
   pMe = PALLOCREC_EX(pfnRealloc, IBitmap, cbColors + (cbRow * h) + 63);
   if (pMe == NULL) {
      return ENOMEMORY;
   }

   MEMSET(pMe, 0, sizeof(*pMe));   // realloc() doesn't always zero memory

   pMe->m_bmp.pvt = (VTBL(IBitmap)*)&gDIBFuncs;    // we use m_bmp's vtable pointer

   pMe->m_uRefs = 1;

   pMe->m_pfnRealloc = pfnRealloc;

   pMe->m_bmp.nDepth = nPixelSize;
   pMe->m_bmp.cx = (uint16) w;
   pMe->m_bmp.cy = (uint16) h;
   pMe->m_bmp.nPitch = (int16) cbRow;

   // init palette
   pMe->m_bmp.cntRGB = cntColors;
   pMe->m_bmp.pRGB = (uint32*) (pMe+1);
   if (0 == cntColors)
      pMe->m_bmp.pRGB = POINTEROFDOOM;
   
   // init bitmap data
   if (0 == w * h) {
      pMe->m_bmp.pBmp = POINTEROFDOOM;
   } else {
      // align pointer on 64-byte boundry
      pMe->m_bmp.pBmp = (byte *)(((uint32)(pMe + 1) + cbColors + 63) & 0xFFFFFFC0);
   }

   *ppIDIB = (IDIB*)pMe;

   return SUCCESS;
}


int
DIB_New(int nPixelSize, int32 w, int32 h, int cntColors, IDIB **ppIDIB)
{
   return DIB_NewEx(nPixelSize, w, h, cntColors, 0, ppIDIB);
}


/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

//
// The following functions are used for all DIB formats
//

static uint32
DIB_AddRef(IBitmap *po)
{
   return ++po->m_uRefs;
}


static uint32
DIB_Release(IBitmap *po)
{
   if (--po->m_uRefs != 0) {
      return po->m_uRefs;
   }

   // Ref count is zero. So, release memory associated with this object.
   if (po->m_bmp.pPaletteMap != NULL) {
      IQI_Release(po->m_bmp.pPaletteMap);
   }

   po->m_pfnRealloc(po, 0);   // free po

   return 0;
}


static int
DIB_QueryInterface(IBitmap *pMe, AEECLSID clsid, void **ppNew)
{
   void *po = 0;
   
   if (clsid == AEECLSID_AEEDIB) {  // This will phase out.  It's here for backward compatibility.
      po = &(pMe->m_bmp.pPaletteMap);
   } 
   else if (clsid == AEECLSID_DIB || clsid == AEECLSID_DIB_20) {
      po = & pMe->m_bmp;
      DIB_AddRef(pMe);
   }
   else if (clsid == AEECLSID_BITMAP) {
      po = pMe;
      DIB_AddRef(pMe);
   }

   *ppNew = po;
   return (po != 0 ? SUCCESS : EUNSUPPORTED);
}



// Square error for a color component in p[] (cacluate Euclidean distance)
#define SQERROR(p, ndx, val)     ( (((byte*)p)[ndx] - val) * (((byte*)p)[ndx] - val))


static NativeColor
DIB_RGBToNative(IBitmap *pMe, RGBVAL rgb)
{
   NativeColor ncConvColor = 0;
   uint16 cntRGB        = pMe->m_bmp.cntRGB;
   uint8 nColorScheme   = pMe->m_bmp.nColorScheme;

   byte byRed = (byte) ((rgb << 16) >> 24);        // same as (rgb >> 8) & 0xFF, but faster in Thumb
   byte byGrn = (byte) ((rgb << 8) >> 24);         // same as (rgb >> 16) & 0xFF, but faster in Thumb
   byte byBlu = (byte) (rgb >> 24);

   // palette based color-mapping
   if (cntRGB) {

      uint32 nDelta = 0xffffffff;
      uint32 *pRGB = pMe->m_bmp.pRGB;
      uint32 *pRGBClosest = pRGB;
      uint32 *pRGBLimit = pRGB + cntRGB;

      // search through the palette for closest match
      for (; pRGB < pRGBLimit; pRGB++) {

         uint32 nDelta0;

         nDelta0  = SQERROR(pRGB, 0, byBlu);    // byte 0 in palette entry = blue
         nDelta0 += SQERROR(pRGB, 1, byGrn);    // byte 1 in palette entry = green
         nDelta0 += SQERROR(pRGB, 2, byRed);    // byte 2 in palette entry = red

         if (nDelta0 < nDelta) {
            nDelta = nDelta0;
            pRGBClosest  = pRGB;

            // perfect match, abort search
            if (nDelta == 0)
               break;
         }
      }
      
      ncConvColor = pRGBClosest - pMe->m_bmp.pRGB;
   } 
   // scheme based color-mapping
   else if (nColorScheme) {

      // construct NativeColor from RGBVAL components
      switch (nColorScheme) {
                                                      // color|assigned-to
         case IDIB_COLORSCHEME_332:                   // ------------------
            ncConvColor =  ((byBlu & 0xC0) >> 6)      // blue: bits 0-1
                        + ((byGrn & 0xE0) >> 3)       // green:bits 2-4
                        +  (byRed & 0xE0);            // red:  bits 5-7
            break;
         case IDIB_COLORSCHEME_444:
            ncConvColor = ((byBlu & 0xf0) >> 4)       // blue: bits 0-3
                        + ((byGrn & 0xf0))            // green:bits 4-7
                        + ((byRed & 0xf0) << 4);      // red:  bits 8-11
            break;
         case IDIB_COLORSCHEME_555:
            ncConvColor = ((byBlu & 0xf8) >> 3)     // blue: bits 0-4
                        + ((byGrn & 0xf8) << 2)     // green:bits 5-9
                        + ((byRed & 0xf8) << 7);    // red:  bits 10-14
            break;
         case IDIB_COLORSCHEME_565:
            ncConvColor = ((byBlu & 0xF8) >> 3)     // blue: bits 0-4
                        + ((byGrn & 0xFC) << 3)     // green:bits 5-10
                        + ((byRed & 0xF8) << 8);    // red:  bits 11-15
            break;
         case IDIB_COLORSCHEME_888:
            ncConvColor =  (byBlu & 0xff)           // blue: bits 0-7
                        + ((byGrn & 0xff) << 8)     // green:bits 8-15
                        + ((byRed & 0xff) << 16);   // red:  bits 16-23
            break;
      }
   }
   // otherwise empty color-mapping

   return ncConvColor;
}


static RGBVAL
DIB_NativeToRGB(IBitmap *pMe, NativeColor clr)
{
   if ((uint32)clr < (uint32)pMe->m_bmp.cntRGB) {

      // palette based color-mapping

      return (RGBVAL) NTOHL(pMe->m_bmp.pRGB[(int)clr]);

   } else {

      int r=0, g=0, b=0;

      // first mask and multiply to calculate ranges from 00xxxx through FFxxxx
      
      switch (pMe->m_bmp.nColorScheme) {
         case IDIB_COLORSCHEME_332:
            r = (clr & 0xE0) * (0x49 * 0x00400);    // >>6 == <<10, >>16
            g = (clr & 0x1C) * (0x49 * 0x02000);    // >>3
            b = (clr & 0x03) * (0x55 * 0x10000);    // >>0
            break;
            
         case IDIB_COLORSCHEME_444:
            return MAKE_RGB( ((clr & 0x0f00) >> 8),
                             ((clr & 0x00f0) >> 4),
                             ((clr & 0x000f))     ) * 0x11;
            
         case IDIB_COLORSCHEME_555:
            r = (clr & 0x7C00) * (0x21 * 0x0010);   // >>12
            g = (clr & 0x03E0) * (0x21 * 0x0200);   // >>7
            b = (clr & 0X001F) * (0x21 * 0x4000);   // >>2
            break;
            
         case IDIB_COLORSCHEME_565:
            r = (clr & 0xF800) * (0x21 * 0x0008);   // >>13
            g = (clr & 0x07E0) * (0x41 * 0x0080);   // >>9
            b = (clr & 0x001F) * (0x21 * 0x4000);   // >>2
            break;
            
         case IDIB_COLORSCHEME_888:
            return ( ((clr & 0xFF0000) >> 8)          // red
                     | ((clr & 0x00FF00) << 8)        // blue  
                     | ((clr & 0x00000FF) << 24));    // green
      }

      // then do all the shifting here
      
      r >>= 16;
      g >>= 16;
      b >>= 16;

      return MAKE_RGB(r, g, b);
   }
}


static int
DIB_DrawPixel(IBitmap *pMe, unsigned int x, unsigned int y, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int
DIB_GetPixel(IBitmap *pMe, unsigned x, unsigned y, NativeColor *pColor)
{
   return EUNSUPPORTED;
}


static int
DIB_SetPixels(IBitmap *pMe, unsigned int cnt, AEEPoint *pPoint, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int
DIB_DrawHScanline(IBitmap *pMe, unsigned y, unsigned xMin, unsigned xMax, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int
DIB_FillRect(IBitmap *pMe, const AEERect *pRect, NativeColor color, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int
DIB_BltIn(IBitmap *pMe, int xDst, int yDst, int dx, int dy, IBitmap *pSrc, int xSrc, int ySrc, AEERasterOp rop)
{
   return IBITMAP_BltOut(pSrc, xDst, yDst, dx, dy, pMe, xSrc, ySrc, rop);
}


static int
DIB_BltOut(IBitmap *pMe, int xDst, int yDst, int dx, int dy, IBitmap *pDst, int xSrc, int ySrc, AEERasterOp rop)
{
   return EUNSUPPORTED;
}


static int
DIB_GetInfo(IBitmap *pMe, AEEBitmapInfo *pInfo, int nSize)
{
   if (nSize != sizeof(*pInfo))
      return EUNSUPPORTED;
   
   pInfo->cx = pMe->m_bmp.cx;
   pInfo->cy = pMe->m_bmp.cy;
   pInfo->nDepth = pMe->m_bmp.nDepth;

   return SUCCESS;
}


static int
DIB_CreateCompatibleBitmap(IBitmap *pMe, IBitmap **ppIBitmap, uint16 w, uint16 h)
{
   int rv = DIB_NewEx(pMe->m_bmp.nDepth, w, h, pMe->m_bmp.cntRGB, NULL, (IDIB**)ppIBitmap);
   if (rv == SUCCESS) {
      MEMMOVE( (*ppIBitmap)->m_bmp.pRGB, pMe->m_bmp.pRGB, pMe->m_bmp.cntRGB*sizeof(uint32) );
      (*ppIBitmap)->m_bmp.nColorScheme = pMe->m_bmp.nColorScheme;
   }
   
   return rv;
}


static int
DIB_SetTransparencyColor(IBitmap *pMe, NativeColor color)
{
   pMe->m_bmp.ncTransparent = color;
   return SUCCESS;
}


static int
DIB_GetTransparencyColor(IBitmap *pMe, NativeColor *pColor)
{
   if (pColor) {
      *pColor = pMe->m_bmp.ncTransparent;
      return SUCCESS;
   }
   return EBADPARM;
}



#if defined(AEE_SIMULATOR) && defined(_DEBUG)

//----------------------------------------------------------------------
//-----  UNIT TESTS                                                -----
//----------------------------------------------------------------------


// Test RGBToNative() and NativeToRGB()
//
int AEEBitmapTest_RGBPal(void)
{
   IBitmap ib;
   static struct {
      byte b, g, r, a;        // follows Windows RGBQUAD format
   } a[] = {
      0,   0,   0,    0,
      255, 255, 255,  0,
      255, 0,   0,    0, 
      0,   255, 0,    0,
      0,   0,   255,  0, 
      255, 255, 0,    0,
      255, 0,   255,  0,
      0,   255, 255,  0,
      8,   5,   1,    0,
      1,   5,   8,    0,     // (0,4,9) after XOR
      0,   4,  11,    0,     // distance from (0,4,9) = 2 (Euclidean or axial)
      0,   0,   0,    0,     // DUPLICATE of index 0
   };

   NativeColor nc;
   int cnt = ARRAY_SIZE(a);
   int ii;

   ZEROAT(&ib);
   ib.m_bmp.pRGB = (uint32*)a;
   ib.m_bmp.cntRGB = cnt;

   for (ii = 0; ii < cnt; ++ii) {

      RGBVAL rgb, rgbDiff;
      NativeColor ncExpected = ii;

      if (ii == cnt-1) {
         ncExpected = 0;  // duplicate of 0
      }

      // look for exact match
      
      rgb = MAKE_RGB(a[ii].r, a[ii].g, a[ii].b);
      nc = DIB_RGBToNative(&ib, rgb);
      if (nc != ncExpected) {
         DBGPRINTF("AEEBitmapTest_RGBPal: FAIL A-%d", ii);
         return 1;
      }

      // look for approximate match (twiddle lowest bit)
      // table is constructed so that this should result in the same index using Euclidean distance
      
      rgbDiff = MAKE_RGB(a[ii].r ^ 1, a[ii].g ^ 1, a[ii].b ^ 1);  // distance = ~1.732 Euclidean, or 3 axial
      nc = DIB_RGBToNative(&ib, rgbDiff);
      if (nc != ncExpected) {
         DBGPRINTF("AEEBitmapTest_RGBPal: FAIL B-%d", ii);
         return 1;
      }

      rgbDiff = DIB_NativeToRGB(&ib, nc);
      if (rgb != rgbDiff) {
         DBGPRINTF("AEEBitmapTest_RGBPal: FAIL C-%d", ii);
         return 1;
      }
   }
   return 0;
}


#define TEST332(r,g,b,rr,rg,rb)   IDIB_COLORSCHEME_332,  ( (((r)&7) << 5) | (((g)&7) << 2) | ((b)&3) ),  MAKE_RGB(rr,rg,rb)
#define TEST444(r,g,b,rr,rg,rb)   IDIB_COLORSCHEME_444,  ( (((r)&0x0f) << 8) | (((g)&0x0f) << 4) | ((b)&0x0f) ),  MAKE_RGB(rr,rg,rb)
#define TEST555(r,g,b,rr,rg,rb)   IDIB_COLORSCHEME_555,  ( (((r)&0x1f) << 10) | (((g)&0x1f) << 5) | ((b)&0x1f) ),  MAKE_RGB(rr,rg,rb)
#define TEST565(r,g,b,rr,rg,rb)   IDIB_COLORSCHEME_565,  ( (((r)&0x1f) << 11) | (((g)&0x3f) << 5) | ((b)&0x1f) ),  MAKE_RGB(rr,rg,rb)
#define TEST888(r,g,b,rr,rg,rb)   IDIB_COLORSCHEME_888,  ( (((r)&0xff) << 16) | (((g)&0xff) << 8) | ((b)&0xff) ),  MAKE_RGB(rr,rg,rb)


// Tests NativeToRGB() and RGBToNative() for color schemes
//
int AEEBitmapTest_RGBScheme(void)
{
   IBitmap ib;
   static struct {
      int nScheme;
      NativeColor nc;
      RGBVAL rgb;
   } atv[] = {
      TEST332( 0x00, 0x00, 0x00,  0x00, 0x00, 0x00),   // black -> black
      TEST332( 0x07, 0x07, 0x03,  0xFF, 0xFF, 0xFF),   // white -> white
      TEST332( 0x01, 0x01, 0x01,  0x24, 0x24, 0x55),   // min bit of each native component
      TEST332( 0x04, 0x02, 0x01,  0x92, 0x49, 0x55),   // max bits, different for each element

      TEST444( 0x00, 0x00, 0x00,  0x00, 0x00, 0x00),
      TEST444( 0x0F, 0x0F, 0x0F,  0xFF, 0xFF, 0xFF),
      TEST444( 0x01, 0x01, 0x01,  0x11, 0x11, 0x11),
      TEST444( 0x09, 0x08, 0x04,  0x99, 0x88, 0x44),
         
      TEST555( 0x00, 0x00, 0x00,  0x00, 0x00, 0x00),
      TEST555( 0x1F, 0x1F, 0x1F,  0xFF, 0xFF, 0xFF),
      TEST555( 0x01, 0x01, 0x01,  0x08, 0x08, 0x08),
      TEST555( 0x10, 0x08, 0x04,  0x84, 0x42, 0x21),
         
      TEST565( 0x00, 0x00, 0x00,  0x00, 0x00, 0x00),
      TEST565( 0x1F, 0x3F, 0x1F,  0xFF, 0xFF, 0xFF),
      TEST565( 0x01, 0x01, 0x01,  0x08, 0x04, 0x08),
      TEST565( 0x08, 0x10, 0x04,  0x42, 0x41, 0x21),
      TEST565( 0x10, 0x08, 0x02,  0x84, 0x20, 0x10),
         
      TEST888( 0x00, 0x00, 0x00,  0x00, 0x00, 0x00),
      TEST888( 0xff, 0xff, 0xff,  0xFF, 0xFF, 0xFF),
      TEST888( 0x01, 0x01, 0x01,  0x01, 0x01, 0x01),
      TEST888( 0x81, 0x10, 0x02,  0x81, 0x10, 0x02),
   };
   RGBVAL rgb;
   NativeColor nc;
   int ii;

   ZEROAT(&ib);

   for (ii = 0; ii < ARRAY_SIZE(atv); ++ii) {

      // Convert native values to expected RGB values
         
      ib.m_bmp.nColorScheme = atv[ii].nScheme;
         
      rgb = DIB_NativeToRGB(&ib, atv[ii].nc);
      if (rgb != atv[ii].rgb) {
         DBGPRINTF("AEEBitmapTest_RGBScheme: FAIL A-%d", ii);
         return 1;
      }

      // Convert RGB back to native values
      
      // This leaves room for variation in RGBToNative() in color schemes.  We
      // ensure that the RGBVAL closest to a particular NC will convert to
      // that NC, but never checks inexact matches.  (That requirement isn't
      // yet defined.)
         
      nc = DIB_RGBToNative(&ib, rgb);
      if (nc != atv[ii].nc) {
         DBGPRINTF("AEEBitmapTest_RGBScheme: FAIL B-%d", ii);
         return 1;
      }
   }

   return 0;
}


void AEEBitmapTest(void)
{
   int errs = 0;
   
   errs += AEEBitmapTest_RGBPal();
   errs += AEEBitmapTest_RGBScheme();

   DBGPRINTF("AEEBitmapTest: %d failures", errs);
}

#endif

