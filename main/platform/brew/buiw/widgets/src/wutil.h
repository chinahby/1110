
#ifndef _WUTIL_H_
#define _WUTIL_H_

#include <stdarg.h>
#include "AEEDisp.h"
#include "AEEFontBidiUtil.h"
#include "AEEWidget.h"
#include "wlint.h"

void   DrawAndInsetBorder(IDisplay *piDisplay, AEERect *prc, int nThick, RGBVAL rgb);
void   DrawAndInsetBorderEx(IDisplay *piDisplay, AEERect *prc, int nInset, int nThick, RGBVAL rgb, uint8 nAlpha, boolean bBeveled);
void   DrawAndInsetRoundedBorderEx(IDisplay *piDisplay, AEERect *prc, int nInset, int nThick, int nRadius, RGBVAL rgb, uint8 nAlpha);

int     BlendRect(IDisplay *me, const AEERect *prc, RGBVAL rgb, int nAlpha);
int     BlendBlit(IBitmap *pibDest, const AEERect *prcDest, IBitmap *pibSrc, int xSrc, int ySrc, int nAlpha);
boolean IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2);
boolean SubtractRect(AEERect *prcResult, const AEERect *prcSrc, const AEERect *prcSubtract);
void    UnionRect(AEERect *prcResult, const AEERect *prcSrc1, const AEERect *prcSrc2);

// strip the alpha channel from an RGBAVAL
#define RGBA_STRIPALPHA(r) (r&0xFFFFFF00)

#define IDISPLAY_FrameRectEx(p,prc,clr) \
  IDISPLAY_DrawRect((p),(prc),(RGBA_STRIPALPHA(clr)),RGB_NONE,IDF_RECT_FRAME)

#define IDISPLAY_FrameFillRect(p,prc,cfrm,cfill) \
  IDISPLAY_DrawRect((p),(prc),(RGBA_STRIPALPHA(cfrm)),(RGBA_STRIPALPHA(cfill)),IDF_RECT_FRAME|IDF_RECT_FILL)

// extract RGB values as 8-bit fixed point                       
#ifndef EXTRACT_RGB_FP8
#define EXTRACT_RGB_FP8(rgb,r,g,b,a) \
   (a)=(((rgb) << 8) & 0xff00), (r)=((rgb) & 0xff00), (g)=(((rgb) >> 8) & 0xff00), (b)=(((rgb) >> 16) & 0xff00)
#endif

// make an RGBVAL from 8-bit fixed-point color channel values
#ifndef MAKE_RGB_FP8
#define MAKE_RGB_FP8(r,g,b) \
   (RGBVAL) ((uint32)(r) & 0xff00) + (((uint32)(g) & 0xff00) << 8) + (((uint32)(b) & 0xff00) << 16)
#endif

#ifndef AEECLSID_FONTSYSNORMAL
#define AEECLSID_FONTSYSNORMAL   0x01012786  // normal system font
#endif

#ifndef AEECLSID_FONTSYSLARGE
#define AEECLSID_FONTSYSLARGE    0x01012787  // large system font
#endif 

#ifndef AEECLSID_FONTSYSBOLD
#define AEECLSID_FONTSYSBOLD     0x01012788  // bold system font
#endif

#ifndef AEECLAID_FONTSMALL
#define AEECLAID_FONTSMALL       0x0101402f
#endif

#ifndef AEECLSID_FONTSYSITALIC
#define AEECLSID_FONTSYSITALIC      0x0101402c
#endif


#ifndef AEEIID_DIB
#define AEEIID_DIB               0x01001045 // dib
#endif


int IShell_GetResStringSize(IShell *piShell, const char *pszFile, uint16 idRes);

IFont *IDISPLAY_GetFont(IDisplay *piDisplay, AEEFont fontId);
int    ISHELL_CreateFont(IShell *piShell, AEECLSID fontClass, IFont **ppo);

void IDisplay_DrawRectangleEx(IDisplay *me, const AEERect *prc, RGBVAL rgbFrame, RGBVAL rgbFill, boolean bRounded);

void IImage_DrawClipped(IImage * pii, IDisplay *piDisplay, int x, int y, AEERect *prcImage, AEERect * prcClip);
void IImage_DrawImage(IImage *pii, IDisplay *piDisplay, int x, int y, AEERect *rect);
void IImage_DrawTiled(IImage *pii, IDisplay *piDisplay, int x, int y, AEERect *prcImage, AEERect *prcClip);

void IDisplay_DrawVLine(IDisplay *me, int x, int y, int nLen, int nThickness, RGBVAL rgb);
void IDisplay_DrawHLine(IDisplay *me, int x, int y, int nLen, int nThickness, RGBVAL rgb);
void IBitmap_SetPixelsEx(IBitmap *me, AEEPoint *pPoint, unsigned cnt, RGBVAL rgb);
void IDisplay_SetPixels(IDisplay *me, AEEPoint *pPoint, unsigned cnt, RGBVAL rgb);

void IDisplay_FillRectGradient(IDisplay *pDisplay, const AEERect *prc, RGBVAL rgbStart, RGBVAL rgbEnd, 
                               boolean bVertical, boolean bCenter, boolean bAlpha);


void IDisplay_FillRoundedRectGradient(IDisplay *piDisplay, const AEERect *prc, int nRadius, RGBVAL rgbStart, 
                                      RGBVAL rgbEnd, boolean bVertical, boolean bCenter, boolean bAlpha);
#define IDisplay_FillRoundedRect(pd,prc,r,rgb) IDisplay_FillRoundedRectGradient((pd),(prc),(r),(rgb),(rgb),TRUE,FALSE,FALSE);

void IDisplay_DrawStyledRectShadow(IDisplay *piDisplay, const AEERect *prc, const AEERect *prcShadowCaster, int nRadius, RGBVAL rgb, boolean bRounded);

AECHAR IShell_GetEllipsis(IShell *piShell);

void InitMonoDIBPalette(IDIB *piDIB, RGBVAL rgb);
int CreateMonoDIB(IDisplay *piDisplay, IDIB **ppiDIB, 
                  uint16 width, uint16 height, boolean bAllocData);


// tiled image drawing

typedef struct TiledImage {
   IBitmap *   piBitmap;      
   int         cxTile;        // calculated at image load time by dividing image width by nNumTiles
   int         cyTile;        // obtained at image load time with IImage_GetInfo
   int         nNumTiles;     // given by caller at load time
   uint32 *    pRGB;          // copy of palette (if required)
   RGBVAL      rgbLast;       // cached rgbOld for last call to ReplaceColor (performance enhancement)
   NativeColor ncLast;        // cached native color (performance)
   IDIB *      piDib;         // cached IDIB
} TiledImage;


#define TIM_NONE     0
#define TIM_IMAGE    1
#define TIM_BITMAP   2

int TiledImage_LoadResBitmap  (TiledImage *me, IShell *piShell, const char *pszResFile, uint16 idRes, int nNumTiles);
void TiledImage_SetBitmap     (TiledImage *me, IBitmap *piBitmap, int nNumTiles);
int TiledImage_DrawTile       (TiledImage *me, IDisplay *piDisplay, int x, int y, int nTile);
int TiledImage_DrawTileEx     (TiledImage *me, IDisplay *piDisplay, int x, int y, int nTile,
                               int dx, int dy, uint32 dwAlign);
int TiledImage_ReplaceColor   (TiledImage *me, RGBVAL rgbOld, RGBVAL rgbNew);
void TiledImage_Dtor          (TiledImage *me);

#define TILEDIMAGE_HASIMAGE(p)   ((p)->piBitmap != 0)


// misc macros
#define ISRECTEMPTY(prc)   (((prc)->dx <= 0) || ((prc)->dy <= 0))

#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**) (void *) &p)

static __inline void RELEASEPPIF(IBase **p) {
   if (*p) {
      IBASE_Release(*p);
      *p = 0;
   }
}
#endif


#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) (p)); } while(0)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)       (sizeof(a)/sizeof(a[0]))
#endif

#ifndef INFLATERECT
#define INFLATERECT(prc,nx,ny) \
   (prc)->x-=(nx),(prc)->y-=(ny),(prc)->dx+=((nx)*2),(prc)->dy+=((ny)*2)
#endif

#ifndef RELEASE_IF
#define RELEASE_IF(typ,ptr)   if ((ptr)) { typ##_Release((ptr)); } // ((ptr) && (typ##_Release((ptr)), ((ptr) = 0)) != 0)
#endif

#ifndef ADDREF_IF
#define ADDREF_IF(typ,ptr)    if ((ptr)) { typ##_AddRef((ptr)); } // ((ptr) && (typ##_AddRef((ptr))) != 0)
#endif

// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt)     MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)) )
#endif

// get pointer to vtbl from object allocated w/ MALLOCREC_VTBL
#ifndef GETVTBL
#define GETVTBL(po,vtt)             ((AEEVTBL(vtt)*)(void *)((po)+1))
#endif


#ifndef CALLBACK_Timer
#define CALLBACK_Timer(pcb,pfn,pv,ps,n) \
   do { CALLBACK_Init((pcb), (pfn), (pv)); ISHELL_SetTimerEx((ps),(n),(pcb)); } while (0)
#endif

#ifndef CALLBACK_Resume
#define CALLBACK_Resume(pcb,pfn,pv,ps) \
   do { CALLBACK_Init((pcb), (pfn), (pv)); ISHELL_Resume((ps), (pcb)); } while (0)
#endif

#ifndef MALLOCARRAY
#define MALLOCARRAY(type,size)      ((type*)MALLOC(sizeof(type)*(size)))
#endif


// Wide-char whitespace test for wrapping
#define WSPCCHAR            (AECHAR)('\040')

#define WCHARISIN(wch, cBtm, cTop)   \
   ( (AECHAR) ((wch) - (cBtm)) <= ((AECHAR) (cTop) - (cBtm)) )

#define W_ISWHITESPACE(wch)   (WCHARISIN((wch), (AECHAR)'\001', WSPCCHAR))

#define MAKELONG(hiword,loword)  (((uint16)(hiword)<<16)|((uint16)(loword)))
#define HIWORD(l)                (uint16)((l)>>16)
#define LOWORD(l)                (uint16)((l)&0xFFFF)


static __inline void IWidget_GetRect(IWidget *po, AEERect *prc) {
   WExtent we;
   IWIDGET_GetExtent(po, &we);
   SETAEERECT(prc, 0, 0, we.width, we.height);
}


/////////////////////////////////////////////////////////////////////////////
// MemGrow

int MemGrowEx(void **ppMem, int *pcb, int cbUsed, int cbMore, int cbAllocExtra);
int MemAppendEx(void **ppMem, int *pcb, int *pcbUsed, void *pAppend, int cbAppend, int cbAllocExtra);

static __inline 
int MemGrow(void **ppMem, int *pcb, int cbUsed, int cbMore) { 
   return MemGrowEx(ppMem, pcb, cbUsed, cbMore, 0); 
}

static __inline
int MemAppend(void **ppMem, int *pcb, int *pcbUsed, void *pAppend, int cbAppend) {
   return MemAppendEx(ppMem, pcb, pcbUsed, pAppend, cbAppend, 0);
}


/////////////////////////////////////////////////////////////////////////////
// Mem miscellaneous utils

int Mem_LoadResString(void **ppMem, int *pcb, int *pcbUsed, IShell *piShell, 
                      const char *pszResFile, uint16 idRes);

// "No Termination" version of Mem_LoadResString -- means that cbUsed does
// not count the terminating zero, even though it is still present. This is
// useful for concatenating resource strings.
static __inline int Mem_LoadResStringNT(void **ppMem, int *pcb, int *pcbUsed, IShell *piShell, 
                                            const char *pszResFile, uint16 idRes) {
   int nErr = Mem_LoadResString(ppMem, pcb, pcbUsed, piShell, pszResFile, idRes);
   *pcbUsed -= sizeof(AECHAR);
   return nErr;
}

/////////////////////////////////////////////////////////////////////////////
// Bidirectional Text Support

// Returns an IFontBidiUtil for bidirectional text support
IFontBidiUtil *GetFontBidiUtil(IShell *pIShell);

// determine the default alignment of a paragraph of text
uint32 DetermineTextAlignment(IFontBidiUtil *pIFontBidiUtil, AECHAR *pwText, int dwLen);

// should this text be right aligned?
#define TextIsRtoL(p,t,n) ((boolean)(p && (DetermineTextAlignment(p,(AECHAR *)t,n)==IDF_ALIGN_RIGHT)))

#endif // _WUTIL_H_

