
#include "OEMFeatures.h"
#include "AEEFont.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEModTable.h"
#include "AEEStdErr.h"
#include "AEEFile.h"
#include "GreyBitType.h"
#include "OEMHeap.h"
#include "AEE_OEMFile.h"

#ifndef RELEASEIF
#define RELEASEIF(p) do { if (p) { IBASE_Release((IBase*)(p)); p = 0; } } while (0)
#endif

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE   //add by yangdecai
// For Arphic Layout Engine
#include "userdef.h"   //it will include aleuser.h
//for additonal lib
#include "alelib.h"
#include "AEEShell.h"
#include "AEE_OEM.h"

#define __RBMP_PURE_MEMORY_VERSION__

#define SCREENWIDTH (512)
#define DEFAULT_BACKGROUND_COL   MAKE_RGB(255, 255, 255)
/* WARNING: because Windows DIB need to be 32-bit aligned,
   please keep this width 32's multiples. (The Layout Engine
   does not have such limit).
 */
 
#define SCREENBYTEWIDTH ((SCREENWIDTH+7)/8)
#define SCREENHEIGHT 48
#define MAXLINESIZE 120
char gFontDataBuffer [SCREENBYTEWIDTH*SCREENHEIGHT];  //font data of Line []

char gRbmpWorkBuffer [2048]={0,};//3072
char *gAleWorkBuffer;

int gLineTotal;       // total number of characters in Line []
int gLineCursor;     // cursor position, 0 means at first character

ALE_UINT16 gLayoutEngineBiDiBuffer [(MAXLINESIZE+1)*3]={0,};  // buffer for layout engine
ALE_UINT16 gLineCharPos [MAXLINESIZE+1];   //position of each char (used by layout engine)
const unsigned char gbFont_Arabic_Hebrew[] = {
#include "FontDB//Arabic_Hebrew_14x14p.i"
};

#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif
int32 HebrewArabicLangInit(void);
static void ArphicLineCursorMeasure (const AECHAR *pcText, int nChars,
	                                                        int x, int xMax, uint32 dwFlags, int* curx,int LineCursor);
static int ArphicMeasureNChars(const AECHAR *pcText, int nChars);
static int OEMFont_MeasureTextCursorPos(IFont *pMe, int x, const AECHAR *pcText, int nChars, 
                                                        const AEERect *prcClip, int* curx, int LineCursor, uint32 dwFlags);
#endif

#define MMI_GREYBITTYPE_FONTS_PATH     AEEFS_SYS_DIR"systemfont.gvf"

#if defined(FEATURE_DISP_320X240) || defined(FEATURE_DISP_240X320)
#define BIGNUMBER_FONT_SIZE 48 
#define NORMAL_FONT_SIZE    24 
#define LARGE_FONT_SIZE     28 
#define SMALL_FONT_SIZE     4

#elif defined(FEATURE_DISP_220X176) || defined(FEATURE_DISP_176X220)
#if defined(FEATURE_VERSION_1110W516)
#define BIGNUMBER_FONT_SIZE 54 
#else
#define BIGNUMBER_FONT_SIZE 40 
#endif
#ifdef FEATURE_VERSION_C310
#define NORMAL_FONT_SIZE    22
#else
#define NORMAL_FONT_SIZE    20
#endif
#ifdef FEATURE_DISP_128X160
#define LARGE_FONT_SIZE     24 
#else
#define LARGE_FONT_SIZE     28 
#endif
#define SMALL_FONT_SIZE     4

#else
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define BIGNUMBER_FONT_SIZE 42
#else
#define BIGNUMBER_FONT_SIZE 32
#endif

#if defined(FEATURE_LANG_THAI)||defined(FEATURE_LANG_MYANMAR)
#define NORMAL_FONT_SIZE    16
#else
#define NORMAL_FONT_SIZE    15
#endif
#define LARGE_FONT_SIZE     20
#define SMALL_FONT_SIZE     4

#endif


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

#if defined(FEATURE_ARPHIC_LAYOUT_ENGINE)
static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo,
                                             OEMFont_MeasureTextCursorPos};

#else
static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo};
#endif
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
static IFont gFontSmall            = {&gOEMFontFuncs, 0, SMALL_FONT_SIZE,    FALSE, FALSE, NULL};
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
	OEMFont_Destroy(&gFontSmall);

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

static boolean GetCombineOffSet(IFont *pMe, uint16 ch1, uint16 ch2, int *pOff)
{
    boolean bRet = FALSE;
    
    // Hindi
    switch(ch2){
    case 0x093F:
    case 0x094E:
        if(pOff)
        {
            bRet = TRUE;
            *pOff = -(GreyBitType_Layout_GetWidth(pMe->pLayout, ch2)*3)/5;
        }
        break;
    default:
        break;
    }

    // Hebrew
    //switch()
    return bRet;
}
#ifndef FEATURE_DISP_128X160
#define NEW_SIZE  17
#define MATH_FACTOR_BIT         10
#define SCALE_V_ONLY 
#endif
//#include "err.h"
static void DrawChar(IFont *pMe, byte *pBmp, int nPitch, const AECHAR *pcText, int nChars,
	int x, int xMin, int xMax, int sy, int oy, int dy, NativeColor clrText, NativeColor clrBack,
	boolean bTransparency, int *pOutWidth)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if(pMe->wSize != BIGNUMBER_FONT_SIZE)
    {
        AleFontMetrics fm;   //metics of the entrie string
        int kmask, j, yend, sx, xend, i, xloop,ii=0,jj=0;
        byte m,n;
        unsigned char *p;
        word *dp,*dbase;
        char *pfont;
        AleGetStringFontInfo getfont_info;  //structure for retrieve font
        word cText, cBack;
        uint32 bmp_offset = 0;
        AECHAR nCharsContent = 0;
        AECHAR *pText = (AECHAR *)pcText,pTemp=NULL;
        int16 nRealSize = WSTRLEN(pcText);
#ifndef FEATURE_DISP_128X160
			   //第一步, 进行参数合法性检测
		//宽度缩放比
		uint32 fScalex =0;
		uint32 fScaley =0;
		//指向目标数据
		uint16* pbyDst = NULL;
		uint16* pbySrc = NULL;
		uint32 xx=0,yy=0,xx_last=0;
	//	uint16 Position[100] = {0};
#endif
        if(nRealSize > nChars)
        {
            nCharsContent = *(pcText + nChars - 1 + 1);
            pText[nChars] = 0;
        }
       
        /* current line being edited
          ---------------------------------------- */
        memset (&getfont_info, 0, sizeof (getfont_info));
        getfont_info. CodeType = 1;  /* use Unicode */
        getfont_info. String = (ALE_UINT16 *)pText;
        getfont_info. FontMetrics = &fm;
        getfont_info. FontBuffer = gFontDataBuffer;
        getfont_info. BufferRowByteTotal = SCREENBYTEWIDTH;
        getfont_info. BufferColumnByteTotal = SCREENHEIGHT;
        //getfont_info. CharPosition = Position;
		
        //all the remain fields are unchanged
        AleGetStringFont (&getfont_info, gAleWorkBuffer);		
		
		//MSG_FATAL("position %d %d %d",Position[0],Position[1],Position[2]);
		//MSG_FATAL("position %d %d %d",Position[3],Position[4],Position[5]);
		
        if(nRealSize > nChars)
        {
            pText[nChars] = nCharsContent;
        }
       
        xend = fm.horiAdvance;
        if(xend == 0)
        {
            goto FONT_NO_EXIST;
        }
        //yend = fm. outBufHeight;
#ifdef FEATURE_ARPHIC_ARABIC_M16X16P_FONT        
        yend = (dy > 16)?(16):(dy);
#else        
        yend = (dy > 14)?(14):(dy);
#endif        
        pfont = gFontDataBuffer;
#ifndef FEATURE_DISP_128X160
		fScalex =(yend<<MATH_FACTOR_BIT) / NEW_SIZE;
        fScaley =(yend<<MATH_FACTOR_BIT) / NEW_SIZE;
#ifndef SCALE_V_ONLY			
		xend = fScalex*NEW_SIZE/14;
#endif
#endif
        if(x < xMin)
        {
            m = (xMin - x) / 8;
            n = (xMin - x) % 8;
            sx = xMin;
            if((x + xend) > xMax)
            {
                xloop = xMax - xMin;
            }
            else
            {
                xloop = xend - (xMin - x);
            }
        }
        else if((x + xend) > xMax)
        {
            m = n = 0;
            sx = x;
            xloop = xMax - x;
        }
        else
        {
            m = n = 0;
            sx = x;
            xloop = xend;
        }
		
        cText = (word)(clrText & 0xFFFF);
        cBack = (word)(clrBack & 0xFFFF);
        
        bmp_offset = sy*nPitch;
        dp = dbase = (word*)(pBmp + bmp_offset + (sx<<1));
		
#ifndef FEATURE_DISP_128X160
		//MSG_FATAL("xloop=%d bTransparency=%d EngCount=%d",xloop,bTransparency,EngCount);
        for ( j = 0; j < NEW_SIZE; j++ )
        {        	
			yy = (j * fScaley)>>MATH_FACTOR_BIT;
			
            if (bTransparency)
            {
                for ( xx=0,xx_last=0,p = (unsigned char*) (pfont + m + oy*SCREENBYTEWIDTH + yy*SCREENBYTEWIDTH), 
                        dp = dbase + j*(nPitch>>1),kmask = (0x80>>n), i = 0;i < xloop;i++)
                {    
#ifdef SCALE_V_ONLY							
					xx = i;
#else						
					xx = (i * fScalex)>>MATH_FACTOR_BIT;
#endif
					
					if(xx_last!=xx)
					{
						xx_last=xx;
	                    kmask >>= 1;
					}
					
					if ( !(kmask) )
                    {
                        kmask = 0x80;
                        p++;
                    }
					
                    if ( (*p) & (kmask) )
                    {
                        *dp = cText;  //this pixel contains font
                    }
                    dp++;


                }
            }
            else
            {
                for ( xx=0,xx_last=0,p = (unsigned char*) (pfont + m +  oy*SCREENBYTEWIDTH + yy*SCREENBYTEWIDTH), 
					dp = dbase + j*(nPitch>>1),kmask = (0x80>>n), i = 0;i < xloop;i++)
                {
                	
#ifdef SCALE_V_ONLY							
					xx = i;
#else						
					xx = (i * fScalex)>>MATH_FACTOR_BIT;
#endif
					if(xx_last!=xx)
					{
						xx_last=xx;
	                    kmask >>= 1;
					}
					
					if ( !(kmask) )
                    {
                        kmask = 0x80;
                        p++;
                    }
					
                    *dp++ = (*p & (kmask) )?(cText):(cBack);
   
                }
            }
        }
#else
        for ( j = 0; j < yend; j++ )
        {
            if (bTransparency)
            {
                for ( p = (unsigned char*) (pfont + m + oy*SCREENBYTEWIDTH + j*SCREENBYTEWIDTH), 
                       dp = dbase + j*(nPitch>>1), kmask = (0x80>>n), i = 0;         i < xloop;         i++)
                {
                    if ( (*p) & kmask )
                    {
                        *dp = cText;  //this pixel contains font
                    }
                    dp++;
                    
                    kmask >>= 1;
                    if ( !kmask )
                    {
                        kmask = 0x80;
                        p++;
                    }
                }
            }
            else
            {
                for ( p = (unsigned char*) (pfont + m +  oy*SCREENBYTEWIDTH + j*SCREENBYTEWIDTH), 
                         dp = dbase + j*(nPitch>>1), kmask = (0x80>>n), i = 0;        i < xloop;       i++)
                {
                    *dp++ = (*p & kmask)?(cText):(cBack);
                    
                    kmask >>= 1;
                    if ( !kmask )
                    {
                        kmask = 0x80;
                        p++;
                    }
                }
            }
        }

#endif		
        *pOutWidth = xloop;
        return;
    }
    else
    {
FONT_NO_EXIST:
#endif
{
    int xSrc, i;
    byte xWidth, xWidthOrig, xxDisp, *sp, *pFontData;
    int bytes_per_row, dispWidth = 0;
    word *dp, *dpBase, cText, cBack;
    unsigned int y1;
    AECHAR ch,ch1;
    int bmp_offset;
    GB_Bitmap charBmp;
    int16 foreR, foreG, foreB, backR, backG, backB, diffR, diffG, diffB;
    int xCombOff = 0;
    
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
	    ch = pcText[i];
		if(ch < ' ') continue;
        
        if(xCombOff == 0)
        {
            ch1 = (i+1)<nChars?pcText[i+1]:0;
            if(GetCombineOffSet(pMe, ch, ch1, &xCombOff))
            {
                ch  = ch1;
                ch1 = pcText[i];
            }
            
            if(0 != GreyBitType_Layout_LoadChar(pMe->pLayout, ch, &charBmp))
    	    {
    	        continue;
    	    }
        }
        else
        {
    		if(0 != GreyBitType_Layout_LoadChar(pMe->pLayout, ch1, &charBmp))
    	    {
                xCombOff = 0;
    	        continue;
    	    }
            
            charBmp->horioff += xCombOff;
            xCombOff = 0;
        }
        
        xWidth = (byte)charBmp->width;
        bytes_per_row = charBmp->pitch;
        
 	    // Clip x coordinate
        xWidthOrig = xWidth;
        x += charBmp->horioff;
 	    xSrc = 0;
        
        if (x > xMax)
        {
            break;
        }
        else if (x < xMin)
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
                        backR = (((foreR - backR) * (*sp)) >> 8) + backR;
                        backG = (((foreG - backG) * (*sp)) >> 8) + backG;
                        backB = (((foreB - backB) * (*sp)) >> 8) + backB;
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
            if(charBmp->horioff < 0) //如泰语等语言
            {
                while (y1--){
                    unsigned int x1 = -charBmp->horioff;
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
                            backR = (((foreR - backR) * (*sp)) >> 8) + backR;
                            backG = (((foreG - backG) * (*sp)) >> 8) + backG;
                            backB = (((foreB - backB) * (*sp)) >> 8) + backB;
                            *dp = (unsigned short)CGreyBit_COLORSCHEME565_GET_RGB(backR, backG, backB);
                            break;
                        }
                        dp++;
                        sp++;
                    }  // for loop            
                    
                    backR = CGreyBit_COLORSCHEME565_GET_R(cBack);
                    backG = CGreyBit_COLORSCHEME565_GET_G(cBack);
                    backB = CGreyBit_COLORSCHEME565_GET_B(cBack);
                    x1 = xxDisp+charBmp->horioff;
                    while (x1--){
                        switch(*sp){
                        case 0:
                            *dp = cBack;
                            break;
                        case 0xFF:
                            *dp = cText;
                            break;
                        default:
                            foreR = ((diffR * (*sp)) >> 8) + backR;
                            foreG = ((diffG * (*sp)) >> 8) + backG;
                            foreB = ((diffB * (*sp)) >> 8) + backB;
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
            else
            {
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
                            foreR = ((diffR * (*sp)) >> 8) + backR;
                            foreG = ((diffG * (*sp)) >> 8) + backG;
                            foreB = ((diffB * (*sp)) >> 8) + backB;
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
        }
        
 	    dispWidth += xWidth;
        x += xWidth;
    }
 
    *pOutWidth = dispWidth;  
}
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    }
#endif
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
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if ( 0 != HebrewArabicLangInit() )
    {
       return EFAILED;
    }
#endif    
    
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
    nHeight = pMe->wSize;
    
    // Text is horizontally aligned
    if (dwFlags & IDF_ALIGNHORZ_MASK)
    {
        OEMFont_MeasureText(pMe, pcText, nChars, 0, NULL, &nWidth);
        
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
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if(pMe->wSize != BIGNUMBER_FONT_SIZE)
    {
        int nRealStrLen, nTotalWidth = 0;
        int nCharFitsTemp = 0;
        int nPixelsTemp = 0;
    
        if ( 0 != HebrewArabicLangInit() )
        {
           return EFAILED;
        }
        
        // Let's perform some sanity checks first
        if (!pcText)
        {
            return SUCCESS;
        }
        
        if (!pnCharFits)
        {
            pnCharFits = &nCharFitsTemp;
        }
        if(!pnPixels)
        {
            pnPixels = &nPixelsTemp;
        }
    
        nRealStrLen = WSTRLEN(pcText);
    
        if ((nChars < 0 || nRealStrLen <= nChars) && (nMaxWidth < 0))
        {
            nTotalWidth = ArphicMeasureNChars(pcText, nRealStrLen);
            *pnPixels = nTotalWidth;
            *pnCharFits = nRealStrLen;
            
            return SUCCESS;
        }
    
        if((nRealStrLen > nChars) && (nMaxWidth < 0) )
        {
            nTotalWidth = ArphicMeasureNChars(pcText, nChars);
            *pnPixels = nTotalWidth;
            *pnCharFits = nChars;
            return SUCCESS;
        }
        
        if (nChars < 0 || nRealStrLen < nChars)
        {
            nChars = nRealStrLen;
        }
        if (nMaxWidth <= 0)
        {
            nMaxWidth = 0x0FFFFFFF;
        }
        
        if(0 == nChars)
        {
            *pnPixels = 0;
            *pnCharFits = 0;
            return SUCCESS;
        }
    
        if(nMaxWidth >= (nTotalWidth = ArphicMeasureNChars(pcText, nChars)))
        {
            *pnPixels = nTotalWidth;
            *pnCharFits = nChars;
            return SUCCESS;
        }
    
        {
           int start = 0;
           int end = nChars;
           int mid;
           
           while(start <= end)
           {
                mid = (start + end)/2;
                if(-1 != (nTotalWidth = ArphicMeasureNChars(pcText, mid)))
                {
                    // 4,7 is OK, 2 not OK
                    if(ABS(nTotalWidth - nMaxWidth) <= 4)
                    {
                        if(nTotalWidth <= nMaxWidth)
                        {
                            *pnPixels = nTotalWidth;
                            *pnCharFits = mid;
                        }
                        else
                        {
                            *pnCharFits = mid - 1;
                            *pnPixels = nMaxWidth;
                        }
                        return SUCCESS;
                    }
                    else if(nTotalWidth > nMaxWidth)
                    {
                        end = mid -1;
                    }
                    else
                    {
                        start = mid + 1;
                    }
                }
                else
                {
                    return -1;
                }
             }//end while
             
            nTotalWidth = ArphicMeasureNChars(pcText, (start + end)/2);
            *pnPixels = (nTotalWidth > 0)?(nTotalWidth):(0);
            *pnCharFits = (start + end)/2;
        }
    
        return SUCCESS;
    }
    else
    {
#endif
    int nRet = SUCCESS;
    int nRealStrLen, nFits, nTotalWidth = 0;
    AECHAR ch,ch1;
    int xCombOff = 0;
    
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

        if(xCombOff == 0)
        {
            ch1 = (nFits+1)<nChars?(*(pcText+1)):0;
            if(GetCombineOffSet(pMe, ch, ch1, &xCombOff))
            {
                ch  = ch1;
                ch1 = *pcText;
            }
            
            nTotalWidth += GreyBitType_Layout_GetWidth(pMe->pLayout, ch) + xCombOff;
        }
        else
        {
    		nTotalWidth += GreyBitType_Layout_GetWidth(pMe->pLayout, ch1);
            xCombOff = 0;
        }
        
	    //nTotalWidth += GreyBitType_Layout_GetWidth(pMe->pLayout, ch);
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

    if(pnPixels)
    {
        *pnPixels = nTotalWidth;
    }
    
    return nRet;
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    }
#endif    
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
    case AEECLAID_FONTSMALL:
    	pMe = &gFontSmall;
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
    return 0;
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

int GreyBitBrewFont_MeasureText(IDisplay *p, int nSize, const AECHAR *psz)
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
    nErr = IDISPLAY_MeasureText(p, AEE_FONT_USER_1, psz);
    IDISPLAY_SetFont(p, AEE_FONT_USER_1, pOldFont);
    GreyBitBrewFont_Destory(pNewFont);
    return nErr;
}

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
void Hebrew_ARbmpGetDataProc(void *RbmpInstance, ALE_UINT32 rbmOffs, void *dataBuf, ALE_UINT16 dataLen)
{
    //DBGPRINTF("Hebrew_ARbmpGetDataProc %d %d",rbmOffs,dataLen);
    MEMCPY(dataBuf, &gbFont_Arabic_Hebrew[rbmOffs],dataLen);
}

/*==============================================================================
函数:
    HebrewArabicLangInit

说明:
    初始化Ale

参数:

返回值:
    none

==============================================================================*/
int32 HebrewArabicLangInit(void)
{
   static boolean staArabicInitFlag = FALSE;
   ALE_INT16 AleTextDirType = ALE_TEXTDIR_ARABIC_SMART;
   int32 kret = -1;

   if ( !staArabicInitFlag )
   {
      if ( (kret = RbmpReset (gRbmpWorkBuffer, sizeof (gRbmpWorkBuffer), NULL, Hebrew_ARbmpGetDataProc )) < 0 )
      {
          DBGPRINTF("Insufficient memory for font data.Initializing Error");
          return kret;
      }
      //get layout engine's working buffer from bitmap font engine
      gAleWorkBuffer = RbmpGetAleWorkBuffer (gRbmpWorkBuffer);

      //Enable Bi-Directional Text Support
      /*
       ALE_TEXTDIR_DEFAULT (0, default): Always Left-to-right, no right-to-left
       ALE_TEXTDIR_ENGLISH_MAJOR (1): default left-to-right; right-to-left for Arabic
       ALE_TEXTDIR_ARABIC_MAJOR (2): default right-to-left, left-to-right for English
       ALE_TEXTDIR_ARABIC_SMART (3): default right-to-left, left-to-right for English;
       If no right-to-left language in text, all line is left-to-right.
       set type to 1-3 if you use right-to-left text
      */
      AleSetTextDirection (AleTextDirType, gAleWorkBuffer) ;      
      //Set Bi-Directional Text Buffer buffer
      AleSetBiDiReorderBuffer (gLayoutEngineBiDiBuffer, sizeof (gLayoutEngineBiDiBuffer), gAleWorkBuffer);
   
      // init gLineCharPos
      memset (gLineCharPos, 0, sizeof (gLineCharPos));
      
      staArabicInitFlag = TRUE;
   }

   return 0;
}

/*===========================================================================
Function: ArphicLineCursorMeasure

Description:
        Text Cursor Interface
        int LineCursor /in/     // cursor position, 0 means at first character
        int* cursorx   /out/     // return the pixel
===========================================================================*/
static void ArphicLineCursorMeasure (const AECHAR *pcText, int nChars,
	int x, int xMax, uint32 dwFlags, int* curx,int LineCursor)
{
   AleFontMetrics       fm;   //metics of the entrie string
   int                  cursorpos;           //cursor's position relatived to displayed text
   int                  cursordirection;    //text direction at cursor
   int                  xstart;               //start position in x to display string
   AleGetStringFontInfo getfont_info;  //structure for retrieve font
   AECHAR nCharsContent = 0;
   AECHAR *tempText = (AECHAR *)pcText;
   int16 nRealSize = WSTRLEN(pcText);

    if(NULL == pcText)
    {
        return;
    }
    if(NULL == curx)
    {
        return;
    }
   
   if(nRealSize > nChars)
   {
       nCharsContent = *(pcText + nChars - 1 + 1);
       tempText[nChars] = 0;
   }
   /* current line being edited
      ---------------------------------------- */
   memset (&getfont_info, 0, sizeof (getfont_info));
   memset((char *)gLineCharPos, 0, sizeof(gLineCharPos));
   getfont_info. CodeType = 1;  /* use Unicode */
   getfont_info. String = tempText;
   getfont_info. FontBuffer = gFontDataBuffer;
   getfont_info. FontMetrics = &fm;
   
   getfont_info. BufferRowByteTotal = SCREENBYTEWIDTH;
   getfont_info. BufferColumnByteTotal = SCREENHEIGHT;
   getfont_info. CharPosition = gLineCharPos;
   
   //all the remain fields are unchanged
   AleGetStringFont (&getfont_info, gAleWorkBuffer);
   if(nRealSize > nChars)
   {
        tempText[nChars] = nCharsContent;
   }
   
   xstart = x;
   /*
   caculate cursor
   */
   if ( LineCursor >= 0 )
   {
      if ( LineCursor >= nChars )  //cursor at end of line
      {
         if ( nChars > 0 )
         {
             //get text direction at last character
             cursordirection = AleGetDisplayDirection (nChars, gAleWorkBuffer);
             if ( cursordirection % 2 == 0 )   //last char is left-to-right
               cursorpos = AleGetDisplayRight (nChars, gLineCharPos, gAleWorkBuffer);
             else  // right-to-left
               cursorpos = AleGetDisplayLeft (nChars, gLineCharPos, gAleWorkBuffer);
         }
         else  //no character at all
         {
            if (dwFlags & IDF_ALIGN_RIGHT)
              cursorpos = 0; //Arabic layout
            else
              cursorpos = 0; 
         }
      }
      else  //cursor is inside string
      {
          /* New version: we put cursor at "previous character"(where cursor is after)'s side to next character
             This will be more intuitive during editing. */
          if ( LineCursor == 0 )  /* Cursor at line's beginning */
          {
             if (dwFlags & IDF_ALIGN_RIGHT)
                cursorpos = fm.horiAdvance;//fm.outBufWidth; //Arabic layout
             else
                cursorpos = 0;  //normal layout
          }
          else
          {
             cursordirection = AleGetDisplayDirection (LineCursor, gAleWorkBuffer);
             if ( cursordirection % 2 == 0 )   //last char is left-to-right
                cursorpos = AleGetDisplayRight (LineCursor, gLineCharPos, gAleWorkBuffer);
             else  //char on cursor is right-to-left
                cursorpos = AleGetDisplayLeft (LineCursor, gLineCharPos, gAleWorkBuffer);
          }
      }

      //decide cursor's real position
#ifdef FEATURE_DISP_128X160
      *curx = xstart + cursorpos;
#else
#ifdef SCALE_V_ONLY      
      *curx = (xstart + cursorpos);
#else
	  *curx = (xstart + cursorpos)*NEW_SIZE/14;
#endif
#endif
      if ( *curx < 0 )
         *curx = 0;
      else if ( *curx >= xMax )
         *curx = xMax;
   }
   
}

/*===========================================================================
Function: ArphicMeasureNChars

===========================================================================*/
static int
ArphicMeasureNChars(const AECHAR *pcText, int nChars)
{
    AECHAR nCharsContent = 0;
    AECHAR *pText = (AECHAR *)pcText;
    int16 nRealSize = WSTRLEN(pcText);
    AleFontMetrics fm;   //metics of the entrie string
    AleGetStringFontInfo getfont_info;  //structure for retrieve font

    if((NULL == pcText) || (WSTRLEN(pcText) < nChars) 
                    || (nChars < 0))
    {
        return -1;
    }
    
    if(0 == nChars)
    {
        return 0;
    }

    if(nRealSize > nChars)
    {
        nCharsContent = *(pcText + nChars);
        pText[nChars] = 0;
    }
        
    /*when outside call,need to Init Arphic Layout Engine*/
    memset (&getfont_info, 0, sizeof (getfont_info));
    getfont_info. CodeType = 1;  /* use Unicode */
    getfont_info. String = (ALE_UINT16 *)pText;
    getfont_info. FontBuffer = NULL;
    getfont_info. FontMetrics = &fm;

    AleGetStringFont (&getfont_info, gAleWorkBuffer);
    if(nRealSize > nChars)
    {
        pText[nChars] = nCharsContent;
    }
#ifdef FEATURE_DISP_128X160
    return ((fm.horiAdvance > 0)?(fm.horiAdvance):(0));
#else
#ifdef SCALE_V_ONLY    
    return ((fm.horiAdvance > 0)?(fm.horiAdvance):(0));
#else
	return ((fm.horiAdvance > 0)?(fm.horiAdvance*NEW_SIZE/14):(0));
#endif
#endif
}

/*===========================================================================
Function: OEMFont_MeasureTextCursorPos
         just for measure text line cursor position in pixels
Description:
        int LineCursor     // cursor position, 0 means at first character
===========================================================================*/
static int OEMFont_MeasureTextCursorPos(IFont *pMe,  int x, const AECHAR *pcText, int nChars,
                 const AEERect *prcClip, int* curx, int LineCursor, uint32 dwFlags)
{
    int          nWidth;
    int          xMax;
    
    if(!prcClip)
    {
        return EMEMPTR;
    }
    // If no text, return immediately
    if (!pcText)
    {
        return EMEMPTR;
    }
    
    if ( 0 != HebrewArabicLangInit() )
    {
       return EFAILED;
    }
    // Calculate the string length
    if (nChars < 0) 
    {
        nChars = WSTRLEN(pcText);
    }

    // Get the text width and height
    OEMFont_MeasureText(pMe, pcText, nChars, 0, NULL, &nWidth);
    
    // Text is horizontally aligned
    if (dwFlags & IDF_ALIGNHORZ_MASK)
    {
        x = prcClip->x;

        if (dwFlags & IDF_ALIGN_RIGHT)
        {
            x += (prcClip->dx - nWidth);
        }
        else if (dwFlags & IDF_ALIGN_CENTER)
        {
           x += ((prcClip->dx - nWidth) >> 1);
        }
    }
	
    // check the x,y and rcclip
    if(x >(prcClip->x +prcClip->dx))
    {
        //Overflow the end x coordinate
        return SUCCESS;
    }

    if(prcClip->x > (x+ nWidth))
    {
        //Overflow the start x coordinate
        return SUCCESS;
    }

    xMax = prcClip->x + prcClip->dx - 1;
    ArphicLineCursorMeasure (pcText, nChars, x, xMax, dwFlags, curx, LineCursor);

    return SUCCESS;
}

#endif // FEATURE_ARPHIC_LAYOUT_ENGINE

