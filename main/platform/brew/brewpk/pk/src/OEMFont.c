/*===========================================================================
                              OEMFont.c

  This file contains sample source to the IFont interface.

  Please search for OEM-Note in the file for points of interest.

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
            
============================================================================*/

/*============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
when          who   what, where, why
-----------   ---   ---------------------------------------------------------
03 Dec 2007   bw    Initial Version
                    
============================================================================*/

#include "OEMFeatures.h"
#include "AEEFont.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "OEMFontData.h"                    // Font image definition

//
//---------------------   OEM Font define   -----------------------
//
#define  CH_NORMAL_HEIGHT       16          // Normal font height in pixel unit
#define  CH_NORMAL_WIDTH_PIXEL  16          // Normal font width in pixel unit

//bw: #define  CH_USER1_HEIGHT        12          // User1 font height in pixel unit
#define  CH_USER1_HEIGHT        15//bw:20080506 14          // User1 font height in pixel unit
#define  CH_USER1_WIDTH_PIXEL   12          // User1 font width in pixel unit


#ifdef FEATURE_LANG_THAI
//#define TONE_UP_START 0xE000
//#define TONE_START    0x0e48
//#define TONE_VOWEL_WIDTH        6
#define TOP_VOWEL_WIDTH         6
#define BOTTOM_VOWEL_WIDTH      5
#define TONE_WIDTH              6

#define AROUND_VOWEL_OFFSET     4
#define CHAR_NORMAL             0
//#define CHAR_TONE_VOWEL         1
#define CHAR_TOP_VOWEL          1
#define CHAR_BOTTOM_VOWEL       2
#define CHAR_TONE               3

#define CHAR_AROUND_VOWEL       2

#define BOTTOM_CON_OFFSET       79

static boolean isThaiFont(uint32 ch);
static boolean isTopVowel(uint32 ch);
static boolean isBottomVowel(uint32 ch);
static boolean isBottomConsonant(uint32 ch);
static boolean isTone(uint32 ch);
static boolean isAroundVowel(uint32 ch);
#endif  // FEATURE_LANG_THAI


#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
// For Arphic Layout Engine
#include "userdef.h"   //it will include aleuser.h
//for additonal lib
#include "alelib.h"
#include <stdio.h>
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

extern unsigned char gbFont_Arabic_Hebrew[];
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

//
//---------------------   OEM Font support  -----------------------
//

extern const VTBL(IBitmap) gOEMBitmapFuncs;

//
//---------------------   OEM Font function   -----------------------
//

// Draw font function
//
static int DrawTextEx(IFont *pMe, IBitmap *pDst, const AECHAR * pcText, 
                        int nChars,int x, int y, const AEERect * prcBackground, 
                        uint32 dwFlags, NativeColor clrText,NativeColor clrBack, 
                        NativeColor clrFrame);
static void DrawChar(IFont *pMe, byte *pBmp, int nPitch, const AECHAR *pcText, 
                        int nChars,int x, int xMin, int xMax, int sy, int oy, 
                        int dy, NativeColor clrText, NativeColor clrBack,
	                    boolean bTransparency, int *pOutWidth, boolean bBold);
// Large Bold font
//
static int DrawTextExLargeBold(IFont *pMe, IBitmap *pDst, const AECHAR * pcText, 
                                int nChars,int x, int y, const AEERect * prcBackground, 
                                uint32 dwFlags, NativeColor clrText,NativeColor clrBack, 
                                NativeColor clrFrame);
static void DrawCharLargeBold(byte *pBmp, int nPitch, const AECHAR *pcText, int nChars,
	                            int x, int xMin, int xMax, int sy, int oy, 
	                            int dy, NativeColor clrText, NativeColor clrBack,
	                            boolean bTransparency, int *pOutWidth);

static int    OEMFont_DrawTextLargeBold(IFont *pMe, IBitmap *pDst, int x, int y,
                                        const AECHAR *pcText, int nChars,NativeColor clrFg, 
                                        NativeColor clrBg,const AEERect *prcClip, uint32 dwFlags);
static int    OEMFont_MeasureTextLargeBold(IFont *pMe, const AECHAR *pcText, int nChars,
                                        int nMaxWidth, int * pnFits, int *pnPixels);

// font interface 
//
static uint32 OEMFont_AddRef(IFont *po);
static uint32 OEMFont_Release(IFont *po);
static int    OEMFont_QueryInterface(IFont *po, AEECLSID id, void **ppif);
static int    OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y,
                               const AECHAR *pcText, int nChars, NativeColor clrFg, 
                               NativeColor clrBg, const AEERect *prcClip, uint32 dwFlags);
static int    OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars,
                               int nMaxWidth, int * pnFits, int *pnPixels);
static int    OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize);

//
//---------------------   OEM Font structure  -----------------------
//
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo,
                                             OEMFont_MeasureTextCursorPos};
#else //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE 
static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo};
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
struct IFont 
{
   const AEEVTBL(IFont) *pvt;
   AEEFont     nFont;               // lower-level interfaces still use AEE_FONT_... IDs
   int         nHeight;             // Height of the font
};

// Three static singleton IFont's to represent the BREW-enumerated fonts:
//
IFont gFontLarge  = {&gOEMFontFuncs, AEE_FONT_LARGE, CH_NORMAL_HEIGHT};

// User-defined font
//
IFont gFont12Normal = {&gOEMFontFuncs, AEE_FONT_USER_1, CH_USER1_HEIGHT};

// User2 is large number font. Only support the dial number
IFont gFontLargeNum = {&gOEMFontFuncs, AEE_FONT_USER_2, BIG_NUM_HEIGHT};

//
//---------------------   OEM Font implement  -----------------------
//

int OEMFont_GetSystemFont(AEEFont nFont, IFont **ppif)
{
    switch (nFont)
    {
        case AEE_FONT_NORMAL:
            *ppif = &gFont12Normal;//gFontNormal; 
            break;
            
        case AEE_FONT_BOLD:
            *ppif = &gFont12Normal;//gFontBold;
            break;
            
        case AEE_FONT_LARGE:
            *ppif = &gFontLarge;
            break;
            
        case AEE_FONT_USER_1:
            *ppif = &gFont12Normal;
            break;
            
        case AEE_FONT_USER_2:
            *ppif = &gFontLargeNum;
            break;
            
        default:
            return EUNSUPPORTED;            
    }

    return SUCCESS;
}


static uint32 OEMFont_AddRef(IFont *pme)
{
   return 1;
}


static uint32 OEMFont_Release(IFont *pme)
{
   return 0;
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

/*===========================================================================
Function: OEMFont_DrawText

Description:
        _DrawText Interface
===========================================================================*/
static int
OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars,
                 NativeColor foreground, NativeColor background, const AEERect *prcClip, uint32 dwFlags)
{

    AEERect rca;
    NativeColor clrBack, clrText, clrFrame;
    AEERect * prcBackRect = (AEERect *) prcClip;
    int nWidth,nHeight;
    AEERect dirty_rc;
    
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if ( 0 != HebrewArabicLangInit() )
    {
       return EFAILED;
    }
#endif

    if (pMe->nFont == AEE_FONT_USER_2) // big number
    {
   	    return OEMFont_DrawTextLargeBold(pMe, pDst, x, y, pcText, nChars, foreground, background, prcClip, dwFlags);
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

    nHeight = pMe->nHeight;
    
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

/*===========================================================================
Function: OEMFont_GetInfo

===========================================================================*/
static int OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize)
{

    if (nSize != sizeof(AEEFontInfo))
    {
        return EUNSUPPORTED;
    }
    
    if (pInfo == NULL)
    {
   	    return EBADPARM;
    }
    
    // The ascent and descent value must be filled in by OEM
    switch(pMe->nFont)
    {
        case AEE_FONT_NORMAL:
            pInfo->nAscent  = CH_NORMAL_HEIGHT;
            pInfo->nDescent = 0;
            break;
            
        case AEE_FONT_BOLD:
            pInfo->nAscent  = CH_NORMAL_HEIGHT;
            pInfo->nDescent = 0;
            break;
        
        case AEE_FONT_LARGE:
            pInfo->nAscent  = CH_NORMAL_HEIGHT;
            pInfo->nDescent = 0;
            break;

        case AEE_FONT_USER_1:
            pInfo->nAscent  = 12;
            //pInfo->nDescent = 0;//bw
            pInfo->nDescent = 3;//bw:20080506 making height as 15
            break;

        case AEE_FONT_USER_2:
            pInfo->nAscent  = BIG_NUM_ASCENT;
            pInfo->nDescent = BIG_NUM_DESCENT;
            break;

        default:
            return EUNSUPPORTED;
    }
    
    return SUCCESS;
}

/*==============================================================================
函数:
    RbmpBiDiReOrder_WMS

说明:
    将文本按。

参数:
    wstrIn [in]: 传入待转换的字符串。
    wstrOut [out]: 返回转换后的字符串。
    nOutLen [in]: wstrOut 大小(单位: AECHAR)。

返回值:
    none

备注:
    请将此函数定义放在 gRbmpWorkBuffer 定义的文件中。
==============================================================================*/
void RbmpBiDiReOrder_WMS(AECHAR *wstrIn, 
                         AECHAR *wstrOut, 
                         int32 nInLen,
                         int32 ABaseLevel, 
                         int32 AReOrderOption)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    AECHAR *wstrRet;
    
    wstrRet = AleBiDiReOrder(wstrIn, wstrOut, ABaseLevel, AReOrderOption, nInLen, gAleWorkBuffer);
    if ((NULL == wstrRet) || (wstrIn == wstrRet))
    {
        (void)WSTRCPY(wstrOut, wstrIn);
    }
#else
    return;
#endif
}

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
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
      if ( (kret = RbmpReset (gRbmpWorkBuffer, sizeof (gRbmpWorkBuffer), gbFont_Arabic_Hebrew, 0 )) < 0 )
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
      *curx = xstart + cursorpos;
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
    
    nCharsContent = *(pcText + nChars);
    pText[nChars] = 0;
    
    /*when outside call,need to Init Arphic Layout Engine*/
    memset (&getfont_info, 0, sizeof (getfont_info));
    getfont_info. CodeType = 1;  /* use Unicode */
    getfont_info. String = (ALE_UINT16 *)pText;
    getfont_info. FontBuffer = NULL;
    getfont_info. FontMetrics = &fm;

    AleGetStringFont (&getfont_info, gAleWorkBuffer);
    pText[nChars] = nCharsContent;
    
    return ((fm.horiAdvance > 0)?(fm.horiAdvance):(0));
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

/*===========================================================================
Function: OEMFont_MeasureText

===========================================================================*/
static int
OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnCharFits, int *pnPixels)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   int nRealStrLen, nTotalWidth = 0;
   int nCharFitsTemp = 0;
   int nPixelsTemp = 0;

    if ( 0 != HebrewArabicLangInit() )
    {
       return EFAILED;
    }
#if 0
    if (pMe->nFont == AEE_FONT_USER_2)
    {
       return (OEMFont_MeasureTextLargeBold(pMe, pcText, nChars, nMaxWidth, pnCharFits, pnPixels));
    }
#endif
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
#else //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

    int nRealStrLen, nTotalWidth = 0;
    int nFits;
    word gbindex;
    AECHAR ch;

    if (pMe->nFont == AEE_FONT_USER_2)
    {
       return (OEMFont_MeasureTextLargeBold(pMe, pcText, nChars, nMaxWidth, pnCharFits, pnPixels));
    }

    // Let's perform some sanity checks first
    if (!pcText)
    {
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

        // from 0x0000 ~ 0x02C9 - variable size font
        // for others, fixed 16 pixels width
        {
            gbindex = gbIndex12[ch];
            nTotalWidth += gbWidth12[gbindex]+1; // left 1 pixel when display
        }
        
#ifdef FEATURE_LANG_THAI
        if (isAroundVowel(ch))   // 包元音
        {
            nTotalWidth = nTotalWidth - AROUND_VOWEL_OFFSET;// - 1;
        }
        
        else if (isTone(ch) ||isTopVowel(ch) ||isBottomVowel(ch) )   // 上元音，准上元音
        {
            nTotalWidth = nTotalWidth - gbWidth12[gbindex] - 1;
        }

#endif //#ifdef FEATURE_LANG_THAI
   
        if (nTotalWidth > nMaxWidth)
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
    
    return SUCCESS;
   
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
}

/*===========================================================================
Function: DrawTextEx

===========================================================================*/
static int DrawTextEx(IFont *pMe, IBitmap *pDst, const AECHAR * pcText, int nChars,
                      int x, int y, const AEERect * prcBackground, uint32 dwFlags, NativeColor clrText,
                      NativeColor clrBack, NativeColor clrFrame)
{
    int sx;
    int xMin, xMax, yMin, yMax;
    boolean bTransparent, bUnderline,bBold;
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
    
    dy = pMe->nHeight - oy;
    
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

	/*
    if(pMe->nFont == AEE_FONT_BOLD)
    {
        bBold =TRUE;
    }
    else
    {
       bBold=FALSE;
    }
    */
    bBold = FALSE;		//Add By zzg 2010_07_10
    
    
    if (dy > 0)
    {
         IDIB *pDIB;
         if ((result = IBITMAP_QueryInterface(pDst, AEECLSID_DIB, (void**)&pDIB)) != SUCCESS)
         {
             DBGPRINTF("IBITMAP_QueryInterface FAIL");
             return result;
         }
 
         DrawChar(pMe, pDIB->pBmp, pDIB->nPitch, pcText, nChars,
    	             x, xMin, xMax, sy, oy, dy, clrText, clrBack, bTransparent, &dispWidth, bBold);
 
         IDIB_Release(pDIB);
    }
    else if (bUnderline)
    {
        int nCharFits;
        OEMFont_MeasureText(pMe, pcText, nChars, xMax-xMin+1, &nCharFits, &dispWidth);
    }
    
    if (bUnderline)
    {
        result = IBITMAP_DrawHScanline(pDst, y + pMe->nHeight - 1, sx, sx+dispWidth-1, clrFrame, AEE_RO_COPY);
    }
    
    return result;
}

/*===========================================================================
Function: DrawChar

===========================================================================*/
static void DrawChar(IFont *pMe, byte *pBmp, int nPitch, const AECHAR *pcText, int nChars,
	int x, int xMin, int xMax, int sy, int oy, int dy, NativeColor clrText, NativeColor clrBack,
	boolean bTransparency, int *pOutWidth, boolean bBold)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   AleFontMetrics fm;   //metics of the entrie string
   int kmask, j, yend, sx, xend, i, xloop;
   byte m,n;
   unsigned char *p;
   word *dp,*dbase;
   char *pfont;
   AleGetStringFontInfo getfont_info;  //structure for retrieve font
   word cText, cBack;
   uint32 bmp_offset = 0;
   AECHAR nCharsContent = 0;
   AECHAR *pText = (AECHAR *)pcText;
   int16 nRealSize = WSTRLEN(pcText);

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
   
   //all the remain fields are unchanged
   AleGetStringFont (&getfont_info, gAleWorkBuffer);
   if(nRealSize > nChars)
   {
        pText[nChars] = nCharsContent;
   }
   
    xend = fm.horiAdvance;
    //yend = fm. outBufHeight;
#ifdef FEATURE_ARPHIC_ARABIC_M16X16P_FONT        
    yend = (dy > 16)?(16):(dy);
#else        
    yend = (dy > 14)?(14):(dy);
#endif        
    pfont = gFontDataBuffer;

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

    *pOutWidth = xloop;
    return;
#else // FEATURE_ARPHIC_LAYOUT_ENGINE

    int xSrc, i;
    byte xWidth=0, xWidthOrig=0, xxDisp, bit_mask, *sp, *pFontData;
    int bytes_per_row, dispWidth = 0;
    word *dp, *dpBase, cText, cBack, index;
    unsigned int y1;
    AECHAR ch;
    boolean bBoldFontBuf;
    int bmp_offset;
        
#ifdef FEATURE_LANG_THAI
    uint32  prechar      = 0;
    uint32  postchar     = 0;
    uint8   ThaiSpecChar = CHAR_NORMAL;
    boolean bOriTransparency = bTransparency;
    
#endif // FEATURE_LANG_THAI
    
    bmp_offset = sy * nPitch;
    
    cText = (word)(clrText & 0xFFFF);
    cBack = (word)(clrBack & 0xFFFF);
 
    nPitch >>= 1;
 
    for (i=0; i<nChars && pcText[i]; i++)
    {
        if (x > xMax)
        {
            break;
        }
        ch = pcText[i];
        bBoldFontBuf = FALSE;
             
#ifdef FEATURE_LANG_THAI
        postchar = pcText[i+1];
        if (isAroundVowel(ch))   // 包元音
        {
            x -= AROUND_VOWEL_OFFSET;
            ThaiSpecChar = CHAR_AROUND_VOWEL;
        }
        
        else if (isTopVowel(ch))   // 上元音，准上元音
        {
            x -= TOP_VOWEL_WIDTH;
            ThaiSpecChar = CHAR_TOP_VOWEL;
        }
        
        else if ( isBottomConsonant(ch) )
        {
            if ( isBottomVowel(postchar) )
            {
                ch = ch + BOTTOM_CON_OFFSET;
            }
        }
        
        else if (isBottomVowel(ch))   //下元音
        {
            x -= BOTTOM_VOWEL_WIDTH;
            ThaiSpecChar = CHAR_BOTTOM_VOWEL;
        }
        
        else if (isTone(ch))  //音调
        {
            x -= TONE_WIDTH;
            ThaiSpecChar = CHAR_TONE;

        }
        if ( isThaiFont (ch) )
        {
            bTransparency = TRUE;
        }
        prechar = ch;


#endif  // FEATURE_LANG_THAI   
 
        {
            index = gbIndex12[ch];
            xWidth = gbWidth12[index]+1; // left 1 pixel when display
            bytes_per_row=2; 
        }
        
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
        
        {
            //pFontData = (byte*)ChineseNormalFontBuf12 + (index*24) + oy * bytes_per_row;
            //if(dy>12) dy=12;
            pFontData = (byte*)ChineseNormalFontBuf12 + (index*28) + oy * bytes_per_row;
            if(dy>14) dy=14;
        }
        
        dp = dpBase = (word*)(pBmp + bmp_offset + (x<<1));

#ifdef FEATURE_LANG_THAI
        if (isBottomVowel(ch))   //下元音
        {
            dp = dpBase = dp + nPitch;
        }
#endif //#ifdef FEATURE_LANG_THAI

        y1 = dy;
        while (y1--)
 	    {
            bit_mask = 0x80 >> (xSrc & 0x07);
            sp = pFontData;
 
            sp += xSrc >> 3;
 
            if (bTransparency)
            {
                // draw only foreground color
                unsigned int x1 = xxDisp;
                while (x1--)
     	        {
   	                if (*sp & bit_mask)
   	                {
 	                    *dp = cText;
   	                }
    	            dp++;
 
                    if ((bit_mask >>= 1) == 0)
                    {
                        bit_mask = 0x80;
                        sp++;
                    }
                }  // for loop
            }
            else
            {
                unsigned int x1 = xxDisp;
                while (x1--)
     	        //for (x1=xSrc;x1<xSrc+xxDisp;x1++)
                {
 	                *dp++ = (*sp & bit_mask) ? cText : cBack;
 
 	                if ((bit_mask >>= 1) == 0)
 	                {
                        bit_mask = 0x80;
                        sp++;
                    }
     	        }  // for loop
            }

 	        pFontData += bytes_per_row;
            dp = dpBase += nPitch;
 	    }
 	    
#ifdef FEATURE_LANG_THAI
        bTransparency = bOriTransparency;
        if(CHAR_TOP_VOWEL == ThaiSpecChar)
        {
            ThaiSpecChar = CHAR_NORMAL;
            x += 1; // the width is 4, and now I subtract 6, so it offset 2 here.
        }
        else if(CHAR_BOTTOM_VOWEL == ThaiSpecChar)
        {
            ThaiSpecChar = CHAR_NORMAL;
            //x += 1;
        }
        else if(CHAR_TONE == ThaiSpecChar)
        {
            ThaiSpecChar = CHAR_NORMAL;
            x += 1;
        }
        else if(CHAR_AROUND_VOWEL == ThaiSpecChar)
        {
            ThaiSpecChar = CHAR_NORMAL;
            dispWidth += xWidth - AROUND_VOWEL_OFFSET;
        }
        else
#endif // FEATURE_LANG_THAI
        dispWidth += xWidth;
        x += xWidth;
    }
 
    *pOutWidth = dispWidth;
#endif //    FEATURE_ARPHIC_LAYOUT_ENGINE
}

/*===========================================================================
Function: Large Bold font

===========================================================================*/
static int
OEMFont_DrawTextLargeBold(IFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars,
                 NativeColor foreground, NativeColor background, const AEERect *prcClip, uint32 dwFlags)
{
    AEERect rca;
    NativeColor clrBack, clrText, clrFrame;
    AEERect * prcBackRect = (AEERect *) prcClip;

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
       	    return EUNSUPPORTED;
 
        prcClip = &rca;
        SETAEERECT(prcBackRect, 0, 0,  bitmapInfo.cx, bitmapInfo.cy);
    }
 
    // Text is horizontally aligned
    if (dwFlags & IDF_ALIGNHORZ_MASK)
    {
        int nWidth;
        OEMFont_MeasureTextLargeBold(pMe, pcText, nChars, prcBackRect->dx, NULL, &nWidth);
        x = prcBackRect->x;
 
        if (dwFlags & IDF_ALIGN_RIGHT)
            x += (prcBackRect->dx - nWidth);
        else if (dwFlags & IDF_ALIGN_CENTER)
            x += ((prcBackRect->dx - nWidth) >> 1);
    }
 
    // Text is vertically aligned
    if (dwFlags & IDF_ALIGNVERT_MASK)
    {
        AEEFontInfo fontInfo;
        int nHeight;
        OEMFont_GetInfo(pMe, &fontInfo, sizeof(AEEFontInfo));
        nHeight = fontInfo.nAscent + fontInfo.nDescent;
 
        y = prcBackRect->y;
 
        if(dwFlags & IDF_ALIGN_BOTTOM)
            y += (prcBackRect->dy - nHeight);
        else if(dwFlags & IDF_ALIGN_MIDDLE)
            y += ((prcBackRect->dy - nHeight) >> 1);
    }
 
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
        	return EUNSUPPORTED;
    }
 
    return DrawTextExLargeBold(pMe, pDst, pcText, nChars, x, y, prcBackRect, dwFlags, clrText, clrBack, clrFrame);
}

/*===========================================================================
Function: Measure Large Bold font

===========================================================================*/
static int
OEMFont_MeasureTextLargeBold(IFont *pMe, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnCharFits, int *pnPixels)
{
    int nRealStrLen = 0, nFits = 0, nTotalWidth = 0, nAddWidth = 0;
    
    // Let's perform some sanity checks first
    if (!pcText)
        return 0;
    
    nRealStrLen = WSTRLEN(pcText);
    
    if (nChars < 0 || nRealStrLen < nChars)
        nChars = nRealStrLen;
    
    if (nMaxWidth <= 0)
        nMaxWidth = 0x0FFFFFFF;
    
    if (!pnCharFits)
        pnCharFits = &nFits;
    
    for ((*pnCharFits) = 0; (*pnCharFits) < nChars; (*pnCharFits)++)
    {
        nAddWidth = BIG_NUM_WIDTH;
    
        if (nTotalWidth + nAddWidth > nMaxWidth)
            break;
        nTotalWidth += nAddWidth;
    }
    
    *pnPixels = nTotalWidth;
    
    return SUCCESS;
}

/*===========================================================================
Function: DrawTextExLargeBold

Description: 
        for Large Bold font
===========================================================================*/
static int DrawTextExLargeBold(IFont *pMe, IBitmap *pDst, const AECHAR * pcText, int nChars,
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
      sx = xMin;
   else
	   sx = x;

   // clip y coordinate
   if (y > yMax)
	   return SUCCESS;

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

   dy = BIG_NUM_HEIGHT - oy;  // display height

   if (dy <= 0)   
      return SUCCESS;

   if (dwFlags & IDF_TEXT_TRANSPARENT)
	   bTransparent = TRUE;
   else
	   bTransparent = FALSE;

   if (dwFlags & IDF_TEXT_UNDERLINE)  // last line will be overwritten by underline
   {
      bUnderline = TRUE;
	   dy --;
   }
   else
      bUnderline = FALSE;

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
            DBGPRINTF("IBITMAP_QueryInterface FAIL");
            return result;
        }

        DrawCharLargeBold(pDIB->pBmp, pDIB->nPitch, pcText, nChars,
   	             x, xMin, xMax, sy, oy, dy, clrText, clrBack, bTransparent, &dispWidth);

        IDIB_Release(pDIB);
   }
   else if (bUnderline)
   {
      int nCharFits, nPixels;

      dispWidth = OEMFont_MeasureTextLargeBold(pMe, pcText, nChars, xMax-xMin+1, &nCharFits, &nPixels);
   }

   if (bUnderline)
   {
      result = IBITMAP_DrawHScanline(pDst, y + CH_NORMAL_HEIGHT - 1, sx, sx+dispWidth-1, clrFrame, AEE_RO_COPY);
   }

   return result;
}

/*===========================================================================
Function: DrawCharLargeBold

Description: 
    Draw Large Dial String
===========================================================================*/
static void DrawCharLargeBold(byte *pBmp, int nPitch, const AECHAR *pcText, int nChars,
	int x, int xMin, int xMax, int sy, int oy, int dy, NativeColor clrText, NativeColor clrBack,
	boolean bTransparency, int *pOutWidth)
{
   int y1, xSrc, i;
   byte xWidth;
   byte *pFontData;
   int dispWidth = 0;
   word *dp, *dpBase;
   byte *sp;
   int x1;
   byte bit_mask;
   word cText, cBack;
   AECHAR ch;
   word index;

   cText = (word)(clrText & 0xFFFF);
   cBack = (word)(clrBack & 0xFFFF);

   for (i=0; i<nChars && pcText[i]; i++)
   {
      if (x > xMax)
         break;

      ch = pcText[i];

      if (ch>='0' && ch<='9')
         index = ch - '0';
      else if (ch=='#')
      	 index = 10;
      else if (ch=='*')
      	 index = 11;
      else if (ch=='P'||ch=='p')
      	 index = 12;
      else if (ch=='T'||ch=='t')
      	 index = 13;
      else if (ch=='+')
         index = 14;
      else if(ch==':')
         index = 15;
      else   // ' '  replace character
      	 index = 16;

	  // Clip x coordinate
      xWidth = BIG_NUM_WIDTH;

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
	     xWidth = xMax - x + 1;

      pFontData = (byte*)BigNumFontBuf + (index*BIG_NUM_BYTES_PER_CHAR) + oy * BIG_NUM_BYTES_PER_ROW;
      dp = dpBase = (word*)(pBmp + sy * nPitch + (x<<1));
	  for (y1=0;y1<dy;y1++)
	  {
         bit_mask = 0x80 >> (xSrc & 0x07);
         sp = pFontData;

         if (xSrc > 7)
            sp += xSrc >> 3;

         if (bTransparency)
         {
            // draw only foreground color
    	    for (x1=xSrc;x1<xSrc+xWidth;x1++)
    	    {
  	           if (*sp & bit_mask)
	              *dp = cText;

	           dp++;

               if ((bit_mask >>= 1) == 0)
               {
                  bit_mask = 0x80;
                  sp++;
               }
            }// for loop
         }
         else
         {
            for (x1=xSrc;x1<xSrc+xWidth;x1++)
            {
               if (*sp & bit_mask)
	              *dp++ = cText;
	           else
                  *dp++ = cBack;

	           if ((bit_mask >>= 1) == 0)
	           {
                  bit_mask = 0x80;
                  sp++;
               }
    	    }  // for loop
         }

	     pFontData += BIG_NUM_BYTES_PER_ROW;
         dp = dpBase += (nPitch >> 1);
	  }

	  dispWidth += xWidth;
      x += xWidth;
   }

   *pOutWidth = dispWidth;
}

#ifdef FEATURE_LANG_THAI

boolean isThaiFont(uint32 ch)
{
    if (ch >= 0x0E01 && ch <= 0x0E5B)
        return TRUE;
    else
        return FALSE;
}

boolean isTopVowel(uint32 ch)
{
    if (ch == 0x0E31 || ch == 0x0E34 || ch == 0x0E35 || ch == 0x0E36 || ch == 0x0E37   //上元音
     || ch == 0x0E47 || ch == 0x0E4c)   //准上元音 
        return TRUE;
    else
        return FALSE;
}

boolean isBottomVowel(uint32 ch)
{
    if (ch == 0x0E38 || ch == 0x0E39)   //下元音
        return TRUE;
    else
        return FALSE;
}

boolean isBottomConsonant(uint32 ch)
{
    if (ch == 0x0E0D || ch == 0x0E0E || ch == 0x0E0F || ch == 0x0E10 ) 
        return TRUE;
    else
        return FALSE;
}

boolean isTone(uint32 ch)
{
    if (ch == 0x0E48 || ch == 0x0E49 || ch == 0x0E4a || ch == 0x0E4b)  //音调
        return TRUE;
    else
        return FALSE;
}

boolean isAroundVowel(uint32 ch)
{
    if (ch == 0x0E33)  //包元音
        return TRUE;
    else
        return FALSE;
}
#endif  // FEATURE_LANG_THAI
