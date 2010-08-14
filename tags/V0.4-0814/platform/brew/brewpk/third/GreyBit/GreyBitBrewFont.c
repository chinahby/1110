
#include "OEMFeatures.h"
#include "AEEFont.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEModTable.h"
#include "AEEStdErr.h"
#include "AEEFile.h"
#include "GreyBitType.h"
#include "OEMHeap.h"

#ifndef RELEASEIF
#define RELEASEIF(p) do { if (p) { IBASE_Release((IBase*)(p)); p = 0; } } while (0)
#endif

#define MMI_GREYBITTYPE_FONTS_PATH     AEEFS_SYS_DIR"/systemen.gvf"

#define BIGNUMBER_FONT_SIZE 24
#define NORMAL_FONT_SIZE    14
#define LARGE_FONT_SIZE     16


/* IDIB_COLORSCHEME_565 */
#define CGreyBit_COLORSCHEME565_GET_R(color)       ((unsigned short)((color) >> 11))
#define CGreyBit_COLORSCHEME565_GET_G(color)       ((unsigned short)((unsigned short)((color) << 5) >> 10))    /* (((color) >> 5) & 0x003F)                 */
#define CGreyBit_COLORSCHEME565_GET_B(color)       ((unsigned short)((unsigned short)((color) << 11) >> 11))   /* ((color) & 0x001F)                        */
#define CGreyBit_COLORSCHEME565_GET_RGB(r,g,b)     ((unsigned short)(((r) << 11) | ((g) << 5) | (b)))

static uint32 OEMFont_AddRef(IFont *po);
static uint32 OEMFont_Release(IFont *po);
static int    OEMFont_QueryInterface(IFont *po, AEECLSID id, void **ppif);
static int    OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y,
                               const AECHAR *pcText, int nChars, 
                               NativeColor clrFg, NativeColor clrBg, 
                               const AEERect *prcClip, uint32 dwFlags);
static int    OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars,
                                  int nMaxWidth, int * pnFits, int *pnPixels);
static int    OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize);

static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo};

struct IFont {
   const AEEVTBL(IFont) *pvt;
   uint32                dwRefs;
   int16                 wSize;
   boolean               bBold;
   boolean               bItalic;
   GBHANDLE              pLayout;
};

static IFont gFontNormal           = {&gOEMFontFuncs, 0, NORMAL_FONT_SIZE,   FALSE, FALSE, NULL};
static IFont gFontNormalBold       = {&gOEMFontFuncs, 0, NORMAL_FONT_SIZE,   FALSE, FALSE, NULL};
static IFont gFontLarge            = {&gOEMFontFuncs, 0, LARGE_FONT_SIZE,    FALSE, FALSE, NULL};
static IFont gFontBigNumber        = {&gOEMFontFuncs, 0, BIGNUMBER_FONT_SIZE,FALSE, FALSE, NULL};

static GBHANDLE g_pLibrary = NULL;
static GBHANDLE g_pLoader  = NULL;

void GreyBitBrewFont_Init(void)
{
    if(g_pLibrary == NULL){
        g_pLibrary = GreyBitType_Init();
    }
    
    if(g_pLibrary){
        if(g_pLoader == NULL){
            g_pLoader = GreyBitType_Loader_New(g_pLibrary, MMI_GREYBITTYPE_FONTS_PATH);
        }
    }
}

static void OEMFont_Create(IFont *pMe)
{
    if(g_pLoader && pMe->pLayout == NULL){
        pMe->pLayout = GreyBitType_Layout_New(g_pLoader, pMe->wSize, 8, pMe->bBold, pMe->bItalic);
    }
}

static void OEMFont_Destroy(IFont *pMe)
{
    if (pMe->pLayout)
	{
        GreyBitType_Layout_Done(pMe->pLayout);
        pMe->pLayout = NULL;
    }
}

void GreyBitBrewFont_Done(void)
{
	// Destroy All Font
	OEMFont_Destroy(&gFontNormal);
	OEMFont_Destroy(&gFontNormalBold);
	OEMFont_Destroy(&gFontLarge);
	OEMFont_Destroy(&gFontBigNumber);

    if(g_pLoader){
        GreyBitType_Loader_Done(g_pLoader);
        g_pLoader = NULL;
    }
    
    if(g_pLibrary){
        GreyBitType_Done(g_pLibrary);
        g_pLibrary = NULL;
    }
}

static uint32 OEMFont_AddRef(IFont *pMe)
{
    return ++pMe->dwRefs;
}


static uint32 OEMFont_Release(IFont *pMe)
{
    uint32 dwRefs = --pMe->dwRefs;
    
    if (0 == dwRefs) {
        OEMFont_Destroy(pMe);
    }
    return dwRefs;
}


static int OEMFont_QueryInterface(IFont *pMe, AEECLSID id, void **pvtbl)
{
    void *po = 0;
   
    if (id == AEECLSID_FONT) {
        IFONT_AddRef(pMe);
        po = (void*)pMe;
    }
    *pvtbl = po;
    return (po != 0 ? SUCCESS : ECLASSNOTSUPPORT);
}


static void DrawChar(IFont *pMe, byte *pBmp, int nPitch, const AECHAR *pcText, int nChars,
	int x, int xMin, int xMax, int sy, int oy, int dy, NativeColor clrText, NativeColor clrBack,
	boolean bTransparency, int *pOutWidth)
{
    int xSrc, i;
    byte xWidth, xWidthOrig, xxDisp, *sp, *pFontData;
    int bytes_per_row, dispWidth = 0;
    word *dp, *dpBase, cText, cBack;
    unsigned int y1;
    AECHAR ch;
    int bmp_offset;
    GB_Bitmap charBmp;
    int16 foreR, foreG, foreB, backR, backG, backB, diffR, diffG, diffB;
    bmp_offset = sy * nPitch;
    
    cText = (word)(clrText & 0xFFFF);
    cBack = (word)(clrBack & 0xFFFF);
 
    nPitch >>= 1;
    
    foreR = CGreyBit_COLORSCHEME565_GET_R(cText);
	foreG = CGreyBit_COLORSCHEME565_GET_G(cText);
	foreB = CGreyBit_COLORSCHEME565_GET_B(cText);
    backR = CGreyBit_COLORSCHEME565_GET_R(cBack);
    backG = CGreyBit_COLORSCHEME565_GET_G(cBack);
    backB = CGreyBit_COLORSCHEME565_GET_B(cBack);
    diffR = foreR-backR;
    diffG = foreG-backG;
    diffB = foreB-backB;
    
    for (i=0; i<nChars && pcText[i]; i++)
    {
        if (x > xMax)
        {
            break;
        }
        
	    ch = pcText[i];
		if(ch < ' ') continue;
		
		if(0 != GreyBitType_Layout_LoadChar(pMe->pLayout, ch, &charBmp))
	    {
	        continue;
	    }
	    
        xWidth = (byte)charBmp->width;
        bytes_per_row = charBmp->pitch;
        
 	    // Clip x coordinate
        xWidthOrig = xWidth;
 	    xSrc = 0;
        
   	    if (x < xMin)
  	    {
 		    if ((x+xWidth) < xMin)
 		    {
 			    x += xWidth;
 			    continue;
 		    }
 
 		    xSrc = xMin - x;
 		    xWidth -= xSrc;
 		    x = xMin;
   	    }
 	    else if ((x+xWidth) > xMax)
 	    {
 	        xWidth = xMax - x + 1;
 	    }
        
        xxDisp = (xWidth > xWidthOrig) ? xWidthOrig : xWidth;
        
        pFontData = charBmp->buffer + oy * bytes_per_row;
        dp = dpBase = (word*)(pBmp + bmp_offset + (x<<1));
        
        y1 = dy;
        if (bTransparency){
            while (y1--){
                unsigned int x1 = xxDisp;
                sp = pFontData;
                sp += xSrc;
                
                // draw only foreground color                
                while (x1--){
                    switch(*sp){
                    case 0:
                        break;
                    case 0xFF:
                        *dp = cText;
                        break;
                    default:
                        backR = CGreyBit_COLORSCHEME565_GET_R(*dp);
                        backG = CGreyBit_COLORSCHEME565_GET_G(*dp);
                        backB = CGreyBit_COLORSCHEME565_GET_B(*dp);
                        backR = (((foreR - backR) * (*sp))/256) + backR;
                        backG = (((foreG - backG) * (*sp))/256) + backG;
                        backB = (((foreB - backB) * (*sp))/256) + backB;
                        *dp = (unsigned short)CGreyBit_COLORSCHEME565_GET_RGB(backR, backG, backB);
                        break;
                    }
                    dp++;
                    sp++;
                }  // for loop            
                
                pFontData += bytes_per_row;
                dp = dpBase += nPitch;
            }
        }else{
            while (y1--){
                unsigned int x1 = xxDisp;
                sp = pFontData;
                sp += xSrc;
                
                while (x1--){
                    switch(*sp){
                    case 0:
                        *dp = cBack;
                        break;
                    case 0xFF:
                        *dp = cText;
                        break;
                    default:
                        foreR = ((diffR * (*sp))/256) + backR;
                        foreG = ((diffG * (*sp))/256) + backG;
                        foreB = ((diffB * (*sp))/256) + backB;
                        *dp = (unsigned short)CGreyBit_COLORSCHEME565_GET_RGB(foreR, foreG, foreB);
                        break;
                    }
                    dp++;
                    sp++;
                }  // for loop
                
                pFontData += bytes_per_row;
                dp = dpBase += nPitch;
            }
        }
        
 
 	  dispWidth += xWidth;
       x += xWidth;
    }
 
    *pOutWidth = dispWidth;
}

static int DrawTextEx(IFont *pMe, IBitmap *pDst, const AECHAR * pcText, int nChars,
                      int x, int y, const AEERect * prcBackground, uint32 dwFlags, NativeColor clrText,
                      NativeColor clrBack, NativeColor clrFrame)
{
    int sx;
    int xMin, xMax, yMin, yMax;
    boolean bTransparent, bUnderline;
    int sy, dy, oy;
    int dispWidth;
    int result = SUCCESS;
    
    xMin = prcBackground->x;
    xMax = prcBackground->x + prcBackground->dx - 1;
    yMin = prcBackground->y;
    yMax = prcBackground->y + prcBackground->dy - 1;
 
    if (x < xMin)
    {
        sx = xMin;
    }
    else
    {
 	    sx = x;
 	}
    
    // clip y coordinate
    if (y > yMax)
    {
 	    return SUCCESS;
 	}
 
    if (y < yMin)
    {
 	    sy = yMin;
 	    oy = yMin - y;
    }
    else
    {
 	    sy = y;
 	    oy = 0;
    }
    
    dy = pMe->wSize - oy;
    
    if (dy <= 0)   //
    {
       return SUCCESS;
    }
    
    if (dwFlags & IDF_TEXT_TRANSPARENT)
    {
 	    bTransparent = TRUE;
 	}
    else
    {
 	    bTransparent = FALSE;
 	}
 
    if (dwFlags & IDF_TEXT_UNDERLINE)  // last line will be overwritten by underline
    {
        bUnderline = TRUE;
 	    dy --;
    }
    else
    {
        bUnderline = FALSE;
    }
 
    if ((sy+dy) > yMax)
    {
 	    dy = yMax - sy + 1;
        bUnderline = FALSE;
    }
    
    if (dy > 0)
    {
         IDIB *pDIB;
         if ((result = IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void**)&pDIB)) != SUCCESS)
         {
             return result;
         }
         DrawChar(pMe, pDIB->pBmp, pDIB->nPitch, pcText, nChars,
    	          x, xMin, xMax, sy, oy, dy, clrText, clrBack, bTransparent, &dispWidth);
         IDIB_Release(pDIB);
    }
    else if (bUnderline)
    {
        int nCharFits;
        OEMFont_MeasureText(pMe, pcText, nChars, xMax-xMin+1, &nCharFits, &dispWidth);
    }
    
    if (bUnderline)
    {
        result = IBITMAP_DrawHScanline(pDst, y + pMe->wSize - 1, sx, sx+dispWidth-1, clrText, AEE_RO_COPY);
    }
    
    return result;
}

static int OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars,
                 NativeColor foreground, NativeColor background, const AEERect *prcClip, uint32 dwFlags)
{
    AEERect rca;
    NativeColor clrBack, clrText, clrFrame;
    AEERect * prcBackRect = (AEERect *) prcClip;
    int nWidth,nHeight;
    AEERect dirty_rc;
    
    if(pMe->pLayout == NULL){
        return EFAILED;
    }
    
    // If no text, return immediately
    if (!pcText)
    {
        return SUCCESS;
    }
    
    // Calculate the string length
    if (nChars < 0) 
    {
        nChars = WSTRLEN(pcText);
    }

    // If no background rect, the full rectangle is used
    if (!prcClip)
    {
        AEEBitmapInfo bitmapInfo;
        if (IBITMAP_GetInfo(pDst, &bitmapInfo, sizeof(AEEBitmapInfo))!=SUCCESS)
        {
      	    return EUNSUPPORTED;
      	}
        
        prcBackRect = &rca;
        prcClip = &rca;
        SETAEERECT(prcBackRect, 0, 0,  bitmapInfo.cx, bitmapInfo.cy);
    }
    
    // Get the text width and height
    OEMFont_MeasureText(pMe, pcText, nChars, 0, NULL, &nWidth);
    nHeight = pMe->wSize;
    
    // Text is horizontally aligned
    if (dwFlags & IDF_ALIGNHORZ_MASK)
    {
        x = prcBackRect->x;

        if (dwFlags & IDF_ALIGN_RIGHT)
        {
            x += (prcBackRect->dx - nWidth);
        }
        else if (dwFlags & IDF_ALIGN_CENTER)
        {
           x += ((prcBackRect->dx - nWidth) >> 1);
        }
	}
	
    // Text is vertically aligned
    if (dwFlags & IDF_ALIGNVERT_MASK)
    {
        y = prcBackRect->y;

        if(dwFlags & IDF_ALIGN_BOTTOM)
        {
            y += (prcBackRect->dy - nHeight);
        }
        else if(dwFlags & IDF_ALIGN_MIDDLE)
        {
            y += ((prcBackRect->dy - nHeight) >> 1);
        }
    }
    
    // check the x,y and rcclip
    if(x >(prcBackRect->x +prcBackRect->dx))
    {
        //Overflow the end x coordinate
        return SUCCESS;
    }
    
    if(y >(prcBackRect->y +prcBackRect->dy))
    {
        //Overflow the end y coordinate
        return SUCCESS;
    }
    
    if(prcBackRect->x > (x+ nWidth))
    {
        //Overflow the start x coordinate
        return SUCCESS;
    }
    
    if(prcBackRect->y > (y +nHeight))
    {
        //Overflow the start y coordinate
        return SUCCESS;
    }
    
	// set the dirty rect for text draw
	dirty_rc.x = ((x >prcBackRect->x)?x:prcBackRect->x);
	dirty_rc.y = ((y >prcBackRect->y)?y:prcBackRect->y);
    dirty_rc.dx= (prcBackRect->x + prcBackRect->dx) - dirty_rc.x;
    dirty_rc.dy= (prcBackRect->y + prcBackRect->dy) - dirty_rc.y;
    
    // Update the dirty rect by IBitmap
    (void)IBITMAP_FillRect(pDst, &dirty_rc, 0, AEE_RO_TRANSPARENT);
    
    // Set correct colors
    if (dwFlags & IDF_TEXT_INVERTED)
    {
        clrBack = foreground;
        clrText = background;
    }
    else
    {
        clrBack = background;
        clrText = foreground;
    }
    
    clrFrame = IBITMAP_RGBToNative(pDst, CLR_USER_FRAME);

    // If a rectangle flag is passed in, draw it
    if ((dwFlags & IDF_RECT_MASK) != IDF_RECT_NONE)
    {
        if (IBITMAP_FillRect(pDst, prcBackRect, clrFrame, AEE_RO_COPY) != SUCCESS)
        {
       	    return EUNSUPPORTED;
       	}
    }

    return DrawTextEx(pMe, pDst, pcText, nChars, x, y, prcBackRect, dwFlags, clrText, clrBack, clrFrame);
}

static int OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize)
{
    if (nSize != sizeof(AEEFontInfo))
        return EUNSUPPORTED;
        
    if (pInfo){
        pInfo->nAscent  = pMe->wSize;
	    pInfo->nDescent = 0;
	}else{
	    return EBADPARM;
	}
    return SUCCESS;
}


static int OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnCharFits, int *pnPixels)
{
    int nRet = SUCCESS;
    int nRealStrLen, nFits, nTotalWidth = 0;
    AECHAR ch;
    
    if(pMe->pLayout == NULL){
        return EFAILED;
    }
    
    // Let's perform some sanity checks first
    if (!pcText){
        return SUCCESS;
    }
    
    nRealStrLen = WSTRLEN(pcText);
    
    if (nChars < 0 || nRealStrLen < nChars)
    {
        nChars = nRealStrLen;
    }
    
    if (nMaxWidth <= 0)
    {
        nMaxWidth = 0x0FFFFFFF;
    }
    
    for (nFits = 0; nFits < nChars; nFits++)
    {
        ch = *pcText++;
        if(ch < ' ') {
            continue;
        }
        
	    nTotalWidth += GreyBitType_Layout_GetWidth(pMe->pLayout, ch);
        if (nTotalWidth >= nMaxWidth)
        {
            nTotalWidth = nMaxWidth;
            break;
        }
    }
    
    if (!pnCharFits)
    {
        pnCharFits = &nFits;
    }
    else
    {
        *pnCharFits = nFits;
    }
    
    *pnPixels = nTotalWidth;
    
    return nRet;
}

int GreyBitBrewFont_New(IShell *piShell, AEECLSID cls, void **ppif)
{
    IFont *pMe;
    switch(cls){
    case AEECLSID_FONTSYSNORMAL:
        pMe = &gFontNormal;
        break;
    case AEECLSID_FONTSYSBOLD:
        pMe = &gFontNormalBold;
        break;
    case AEECLSID_FONTSYSLARGE:
        pMe = &gFontLarge;
        break;
    case AEECLSID_FONTSYSBIGNUMBER:
        pMe = &gFontBigNumber;
        break;
    default:
        return AEE_ECLASSNOTSUPPORT;
    }
    
    OEMFont_Create(pMe);
	OEMFont_AddRef(pMe);
    *ppif = pMe;
    return AEE_SUCCESS;
}

int GreyBitBrewFont_Create(int nSize, void **ppif)
{
    IFont *pMe;
    
    if(ppif == NULL || nSize <= 0){
        return EBADPARM;
    }
    
    pMe = sys_malloc(sizeof(IFont));
    if(pMe == NULL){
        return ENOMEMORY;
    }
    
    pMe->pvt        = &gOEMFontFuncs;
    pMe->dwRefs     = 1;
    pMe->wSize      = nSize;
    pMe->bBold      = FALSE;
    pMe->bItalic    = FALSE;
    OEMFont_Create(pMe);
    *ppif = pMe;
    return AEE_SUCCESS;
}

int GreyBitBrewFont_Destory(IFont *pMe)
{
    if(pMe)
    {
        OEMFont_Destroy(pMe);
        sys_free(pMe);
    }
}

#include "AEEDisp.h"
int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags)
{
    int nErr;
    IFont *pOldFont;
    IFont *pNewFont = NULL;
    
    nErr = GreyBitBrewFont_Create(nSize, (void **)&pNewFont);
    if(SUCCESS != nErr)
    {
        return nErr;
    }
    
    pOldFont = IDISPLAY_SetFont(p, AEE_FONT_USER_1, pNewFont);
    nErr = IDISPLAY_DrawText(p, AEE_FONT_USER_1, psz, nl, x, y, prcb, flags);
    IDISPLAY_SetFont(p, AEE_FONT_USER_1, pOldFont);
    GreyBitBrewFont_Destory(pNewFont);
    return nErr;
}

