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
                    
============================================================================*/

#include "OEMFeatures.h"
#include "AEEFont.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "OEMFontData.h"                    // Font image definition

#include "err.h"                            // ERR debug
#include "NV_items.h"
#include "OEMCFGI.h"

//
//---------------------   OEM Font define   -----------------------
//
// ykmilan add for large font 10.01.13

#ifdef FEATURE_LANG_CHINESE

#ifdef FEATURE_FONT_16

#define  CH_NORMAL_HEIGHT       16         // Normal font height in pixel unit
#define  CH_NORMAL_WIDTH_PIXEL  16          // Normal font width in pixel unit
#define  CH_USER1_HEIGHT        16          // User1 font height in pixel unit
#define  CH_USER1_WIDTH_PIXEL   12          // User1 font width in pixel unit

#else // FEATURE_FONT_16

#define  CH_NORMAL_HEIGHT       16          // Normal font height in pixel unit
#define  CH_NORMAL_WIDTH_PIXEL  16          // Normal font width in pixel unit
#define  CH_USER1_HEIGHT        16          // User1 font height in pixel unit
#define  CH_USER1_WIDTH_PIXEL   12          // User1 font width in pixel unit

#endif // FEATURE_FONT_16

#elif defined FEATURE_ARABIA_SANMU_LAYOUT // FEATURE_LANG_CHINESE

#define  CH_NORMAL_HEIGHT       16         // Normal font height in pixel unit
#define  CH_NORMAL_WIDTH_PIXEL  14          // Normal font width in pixel unit
#define  CH_USER1_HEIGHT        16          // User1 font height in pixel unit
#define  CH_USER1_WIDTH_PIXEL   12          // User1 font width in pixel unit

#else

#define  CH_NORMAL_HEIGHT       14         // Normal font height in pixel unit
#define  CH_NORMAL_WIDTH_PIXEL  14          // Normal font width in pixel unit
#define  CH_USER1_HEIGHT        14          // User1 font height in pixel unit
#define  CH_USER1_WIDTH_PIXEL   12          // User1 font width in pixel unit

#endif // FEATURE_LANG_CHINESE

#ifdef FEATURE_LANG_THAI
//#define TONE_UP_START 0xE000
//#define TONE_START    0x0e48
//#define TONE_VOWEL_WIDTH        6
#define TOP_VOWEL_WIDTH         6
#define BOTTOM_VOWEL_WIDTH      5
#define TONE_WIDTH              6

#define AROUND_VOWEL_OFFSET     4//  modify for 0xe33
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

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
static int OEMFont_ChangeToArabia(const AECHAR *pcText, AECHAR *arText, int nChars);
#endif

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

#ifdef FEATURE_ARABIA_SANMU_LAYOUT

#define MAX_ARABIA_STRING_LENGTH   1024

typedef enum _ArabiaLayout
{
    ARABIA_LAST,
    ARABIA_FIRST,
    ARABIA_MIDDLE,
    ARABIA_ALONE,
    ARABIA_NONE
}ArabiaLayout;

typedef struct _pszAr
{
    
    AECHAR        sz;
    ArabiaLayout  type;
    
}pszAr;

const static uint16 Arbic_Position[][5]=/*last，first，midile，alone*/
{
       { 0xfe80, 0xfe80, 0xfe80, 0xfe80, 0x0621},/*0x0621*/
       { 0xfe82, 0xfe81, 0xfe82, 0xfe81, 0x0622},
       { 0xfe84, 0xfe83, 0xfe84, 0xfe83, 0x0623},
       { 0xfe86, 0xfe85, 0xfe86, 0xfe85, 0x0624},
       { 0xfe88, 0xfe87, 0xfe88, 0xfe87, 0x0625},
       { 0xfe8a, 0xfe8b, 0xfe8c, 0xfe89, 0x0626},/*26*/
       { 0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d, 0x0627},
       { 0xfe90, 0xfe91, 0xfe92, 0xfe8f, 0x0628},
       { 0xfe94, 0xfe93, 0xfe93, 0xfe93, 0x0629},
       { 0xfe96, 0xfe97, 0xfe98, 0xfe95, 0x062a},
       { 0xfe9a, 0xfe9b, 0xfe9c, 0xfe99, 0x062b},
       { 0xfe9e, 0xfe9f, 0xfea0, 0xfe9d, 0x062c},
       { 0xfea2, 0xfea3, 0xfea4, 0xfea1, 0x062d},
       { 0xfea6, 0xfea7, 0xfea8, 0xfea5, 0x062e},
       { 0xfeaa, 0xfea9, 0xfeaa, 0xfea9, 0x062f},
       { 0xfeac, 0xfeab, 0xfeac, 0xfeab, 0x0630},
       { 0xfeae, 0xfead, 0xfeae, 0xfead, 0x0631},
       { 0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf, 0x0632},
       { 0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1, 0x0633},
       { 0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5, 0x0634},
       { 0xfeba, 0xfebb, 0xfebc, 0xfeb9, 0xfeba},
       { 0xfebe, 0xfebf, 0xfec0, 0xfebd, 0xfebe},/*36*/
       { 0xfec2, 0xfec3, 0xfec4, 0xfec1, 0x0637},/*37*/
       { 0xfec6, 0xfec7, 0xfec8, 0xfec5, 0x0638},
       { 0xfeca, 0xfecb, 0xfecc, 0xfec9, 0x0639},
       { 0xfece, 0xfecf, 0xfed0, 0xfecd, 0x063a},
       { 0x63b, 0x63b, 0x63b, 0x63b, 0x063b},
       { 0x63c, 0x63c, 0x63c, 0x63c, 0x063c},
       { 0x63d, 0x63d, 0x63d, 0x63d, 0x063d},
       { 0x63e, 0x63e, 0x63e, 0x63e, 0x063e},
       { 0x63f, 0x63f, 0x63f, 0x63f, 0x063f},
       { 0x640, 0x640, 0x640, 0x640, 0x0640},/*'-'*/
       { 0xfed2, 0xfed3, 0xfed4, 0xfed1, 0x0641},
       { 0xfed6, 0xfed7, 0xfed8, 0xfed5, 0x0642},
       { 0xfeda, 0xfedb, 0xfedc, 0xfed9, 0x0643},
       { 0xfede, 0xfedf, 0xfee0, 0xfedd, 0x0644},
       { 0xfee2, 0xfee3, 0xfee4, 0xfee1, 0x0645},
       { 0xfee6, 0xfee7, 0xfee8, 0xfee5, 0x0646},
       { 0xfeea, 0xfeeb, 0xfeec, 0xfee9, 0x0647},
       { 0xfeee, 0xfeed, 0xfeee, 0xfeed, 0x0648},
       { 0xfef0, 0xfeef, 0xfef0, 0xfeef, 0x0649},
       {0xfef2, 0xfef3, 0xfef4, 0xfef1, 0x064a},/*0x64a*/
};

#define MAX_SET1_NUM  23
#define MAX_SET2_NUM  43

const static uint16 theSet1[MAX_SET1_NUM]={
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626};


const static uint16 theSet2[MAX_SET2_NUM]={
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626,
    0x627, 0x623, 0x625, 0x622, 0x62f, 0x630, 0x631, 0x632,
    0x648, 0x624, 0x629, 0x649, 0xfef5, 0xfef6, 0xfef7, 0xfef8,
    0xfef9, 0xfefa, 0xfefb, 0xfefc};

#endif // FEATURE_ARABIA_SANMU_LAYOUT


// Three static singleton IFont's to represent the BREW-enumerated fonts:
//
IFont gFontNormal = {&gOEMFontFuncs, AEE_FONT_NORMAL, CH_NORMAL_HEIGHT};
IFont gFontBold   = {&gOEMFontFuncs, AEE_FONT_BOLD, CH_NORMAL_HEIGHT};
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
	//MSG_FATAL("OEMFont_GetSystemFont.................",0,0,0);
   if (nFont == AEE_FONT_NORMAL)
      *ppif = &gFontNormal;
   else if (nFont == AEE_FONT_BOLD)
      *ppif = &gFontBold;
   else if (nFont == AEE_FONT_LARGE)
      *ppif = &gFontLarge;
   
   else if (nFont == AEE_FONT_USER_1) 
      *ppif = &gFont12Normal;

   else if (nFont == AEE_FONT_USER_2)
      *ppif = &gFontLargeNum;
      
   else
      return EUNSUPPORTED;

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

int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags)
{
    int nErr;
    IFont *pOldFont;
    IFont *pNewFont = NULL;
	AEEFont temp = AEE_FONT_NORMAL;
	//MSG_FATAL("GreyBitBrewFont_DrawText.................=%d",psz[1],0,0);

    OEMFont_GetSystemFont(temp, &pNewFont);
    pOldFont = IDISPLAY_SetFont(p, AEE_FONT_NORMAL, pNewFont);
    IDISPLAY_DrawText(p, AEE_FONT_NORMAL, psz, nl, x, y, prcb, flags);
    IDISPLAY_SetFont(p, AEE_FONT_NORMAL, pOldFont);
    //GreyBitBrewFont_Destory(pNewFont);
	
    return nErr;
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

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
/*===========================================================================
Function: OEMFont_ChangeToArabia

Description:
        _DrawText Interface
===========================================================================*/
static int OEMFont_ChangeToArabia(const AECHAR *pcText, AECHAR *arText, int nChars)
{
    pszAr   pszText[MAX_ARABIA_STRING_LENGTH] = {0};
    AECHAR  szTemp[MAX_ARABIA_STRING_LENGTH]  = {0};
    int     i = 0, j = 0;
    boolean bPrev = FALSE, bNext = FALSE;
    ArabiaLayout PreChar = ARABIA_NONE;
    ArabiaLayout CurChar = ARABIA_NONE;
    int     nLen = 0;
    int     nRelLen = 0;
    int     nStrLen = nChars;
    
    if(!pszText || !arText)
    {
        return EFAILED;
    }
    
    for(i = 0, j = 0; i < nChars; i++)
    {
        if(pcText[i] == 0x0644 &&
            (pcText[i+1] == 0x0622 || pcText[i+1] == 0x0623 || pcText[i+1] == 0x0625 || pcText[i+1] == 0x0627))
        {
            boolean blast = FALSE;
            int     nPrev = 0;
            
            if(i > 0)
            {
                for(nPrev = 0; nPrev < MAX_SET1_NUM; nPrev++)
                {
                    if(pcText[i - 1] == theSet1[nPrev])
                    {
                        blast = TRUE;
                    }

                }                
            }
            
            if(pcText[i + 1] == 0x0622)
            {
                if(blast)
                {
                    pszText[j].sz = 0xfef6;
                }
                else
                {
                    pszText[j].sz = 0xfef5;
                }
                
            }
            else if(pcText[i + 1] == 0x0623)
            {
                if(blast)
                {
                    pszText[j].sz = 0xfef8;
                }
                else
                {
                    pszText[j].sz = 0xfef7;
                }                
            }
            else if(pcText[i + 1] == 0x0625)
            {
                if(blast)
                {
                    pszText[j].sz = 0xfefa;
                }
                else
                {
                    pszText[j].sz = 0xfef9;
                }                
            }
            else if(pcText[i + 1] == 0x0627)
            {
                if(blast)
                {
                    pszText[j].sz = 0xfefc;
                }
                else
                {
                    pszText[j].sz = 0xfefb;
                }                
            }
            
            pszText[j].type  = ARABIA_NONE;
            i++;
        }
        else
        {
            pszText[j].sz    = pcText[i];
            pszText[j].type  = ARABIA_NONE;
        }
        j++;
        nRelLen = j;
    }

    i = 0;
    j = 0;
    nStrLen = nRelLen - 1;

    for(i = 0; i < nRelLen; i++)
    {
        PreChar = CurChar;
        
        // non arabia char
        if(((uint16)(pszText[i].sz) < 0x0621 || (uint16)(pszText[i].sz) > 0x064a) &&
            !((uint16)(pszText[i].sz) >= 0xfef5 && (uint16)(pszText[i].sz) <= 0xfefc))
        {
            CurChar = ARABIA_NONE;
        }
        else
        {
            CurChar = ARABIA_ALONE;
        }

        if(i == 0)
        {
            PreChar = CurChar;
        }

        if(CurChar != PreChar)  
        {
            // add pre char here
            int nTempLen= 0;
            int n       = 0;
                        
            nTempLen= WSTRLEN(szTemp);

            if(PreChar == ARABIA_ALONE)
            {
                // need reverse
                for(n = 0; n < nTempLen && nStrLen > 0; n++)
                {
                    arText[nStrLen--] = szTemp[n];
                }
            }
            else
            {
                // no need reverse
                for(n = 0; n < nTempLen && nStrLen > 0; n++)
                {
                    arText[nStrLen--] = szTemp[nTempLen - 1 - n];
                }
            }
            
            MEMSET(szTemp, 0, sizeof(szTemp));
            
            PreChar = CurChar;
        }

        {
            if(CurChar == ARABIA_ALONE)
            {
                for(j = 0; j < MAX_SET2_NUM; j++)
                {
                    if(pszText[i + 1].sz == theSet2[j])
                    {
                        bNext = TRUE;
                    }
                }

                for(j = 0; j < MAX_SET1_NUM; j++)
                {
                    if(pszText[i - 1].sz == theSet1[j])
                    {
                        bPrev = TRUE;
                    }

                }

                if(bNext && bPrev)
                {
                    pszText[i].type = ARABIA_MIDDLE;
                }
                else if(bNext)
                {
                    pszText[i].type = ARABIA_FIRST;
                }
                else if(bPrev)
                {
                    pszText[i].type = ARABIA_LAST;
                }

                bNext = FALSE;
                bPrev = FALSE;  

                nLen         = WSTRLEN(szTemp);

                if(pszText[i].sz >= 0xfef5 && pszText[i].sz <= 0xfefc)
                {
                    szTemp[nLen] = pszText[i].sz;
                }
                else
                {
                    szTemp[nLen] = Arbic_Position[(uint16)pszText[i].sz - 0x0621][pszText[i].type];
                }
            }
            else
            {
                nLen         = WSTRLEN(szTemp);
                szTemp[nLen] = pszText[i].sz;
            }
        }

        if(i == (nRelLen -1))
        {
            // add pre char here
            int nTempLen= 0;
            int n       = 0;
            
            
            nTempLen= WSTRLEN(szTemp);

            if(CurChar == ARABIA_ALONE)
            {
                // need reverse
                for(n = 0; n < nTempLen && nStrLen >= 0; n++)
                {
                    arText[nStrLen--] = szTemp[n];
                }
            }
            else
            {
                // no need reverse
                for(n = 0; n < nTempLen && nStrLen >= 0; n++)
                {
                    arText[nStrLen--] = szTemp[nTempLen - 1 - n];
                }
            }
            
            MEMSET(szTemp, 0, sizeof(szTemp));
            
            PreChar = CurChar;
        }
    }

    if(arText[0] == 0)
    {
        int t = 0;
        int s = 0;
        
        for(t = 0; t < nRelLen;t++)
        {
            if(arText[t])
            {
                arText[s] = arText[t];
                s++;
            }            
        }
        arText[s] = 0;
    }
    arText[WSTRLEN(arText)] = 0;

    //FREEIF(pszText);

    return SUCCESS;
}
#endif // FEATURE_ARABIC_SANMU_LAYOUT

/*===========================================================================
Function: OEMFont_DrawText

Description:
        _DrawText Interface
===========================================================================*/
int
OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars,
                 NativeColor foreground, NativeColor background, const AEERect *prcClip, uint32 dwFlags)
{

    AEERect rca;
    NativeColor clrBack, clrText, clrFrame;
    AEERect * prcBackRect = (AEERect *) prcClip;
    int nWidth,nHeight;
    AEERect dirty_rc;
    int nRet = SUCCESS;
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    AECHAR arText[MAX_ARABIA_STRING_LENGTH]={0};
    int    arnChars = 0;
#endif
    
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if ( 0 != HebrewArabicLangInit() )
    {
       return EFAILED;
    }
#endif

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    {
        OEMFont_ChangeToArabia(pcText, arText, nChars);
        arnChars = WSTRLEN(arText);
    }   
#endif

    if (pMe->nFont == AEE_FONT_USER_2) // big number
    {
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
        return OEMFont_DrawTextLargeBold(pMe, pDst, x, y, arText, arnChars, foreground, background, prcClip, dwFlags);
#else
   	    return OEMFont_DrawTextLargeBold(pMe, pDst, x, y, pcText, nChars, foreground, background, prcClip, dwFlags);
#endif // FEATURE_ARABIA_SANMU_LAYOUT
    }
    
    // If no text, return immediately
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    if(!pcText || !arText)
#else
    if (!pcText)
#endif        
    {
        return SUCCESS;
    }
    
    // Calculate the string length
    if (nChars < 0) 
    {
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
        arnChars = WSTRLEN(arText);
#else
        nChars = WSTRLEN(pcText);
#endif
    }
	//MSG_FATAL("nChars====%d,%d",nChars,pcText[0],0);

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
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    //OEMFont_MeasureText(pMe, arText, arnChars, 0, NULL, &nWidth);
#else
    OEMFont_MeasureText(pMe, pcText, nChars, 0, NULL, &nWidth);
#endif

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

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    nRet =  DrawTextEx(pMe, pDst, arText, arnChars, x, y, prcBackRect, dwFlags, clrText, clrBack, clrFrame);
    //FREEIF(arText);
#else
    nRet =  DrawTextEx(pMe, pDst, pcText, nChars, x, y, prcBackRect, dwFlags, clrText, clrBack, clrFrame);
#endif

    return nRet;

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
            pInfo->nAscent  = CH_USER1_HEIGHT;
            pInfo->nDescent = 0;
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
#if defined (FEATURE_ARPHIC_LAYOUT_ENGINE) && !defined (FEATURE_ARABIA_SANMU_LAYOUT)
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
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    AECHAR ArText[MAX_ARABIA_STRING_LENGTH]={0};
#endif

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    {
        OEMFont_ChangeToArabia(pcText, ArText, nChars);
    }   
#endif    

    if (pMe->nFont == AEE_FONT_USER_2)
    {
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
       return (OEMFont_MeasureTextLargeBold(pMe, ArText, nChars, nMaxWidth, pnCharFits, pnPixels));
#else
       return (OEMFont_MeasureTextLargeBold(pMe, pcText, nChars, nMaxWidth, pnCharFits, pnPixels));
#endif
    }

    // Let's perform some sanity checks first
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    if(!ArText)
#else
    if (!pcText)
#endif        
    {
        return SUCCESS;
    }

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
    nRealStrLen = WSTRLEN(ArText);
#else
    nRealStrLen = WSTRLEN(pcText);
#endif
 
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
#ifdef FEATURE_ARABIA_SANMU_LAYOUT
        ch= ArText[nFits];
#else
        ch = *pcText++;
#endif

        // from 0x0000 ~ 0x02C9 - variable size font
        // for others, fixed 16 pixels width
        {
            gbindex = gbIndex12[ch];

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
            // fe80 - feff 是阿拉伯字母变形的范围 变形的字母不加 1 个像素
            if((uint16)ch < 65152 || (uint16)ch > 65279)
            {
                nTotalWidth += gbWidth12[gbindex]+1;
            }
            else
            {
                nTotalWidth += gbWidth12[gbindex];
            }
#else            
            nTotalWidth += gbWidth12[gbindex]+1;
#endif              
            //nTotalWidth += gbWidth12[gbindex]+1; // left 1 pixel when display
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
            nTotalWidth = nTotalWidth - gbWidth12[gbindex];
            nFits--;
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

#ifdef FEATURE_ARABIA_SANMU_LAYOUT     // 这里把由于组合减少掉的补回来
    {
        int nAdd = 0;
        
        for(nAdd = 0; nAdd < nFits; nAdd++)
        {
            if(ArText[nAdd] >= 0xfef5 && ArText[nAdd] <= 0xfefc)
            {
                (*pnCharFits)++;
            }
        }
    }
#endif

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
    //MSG_FATAL("DrawTextEx.................%d",pcText[0],0,0);
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
    
    if(pMe->nFont == AEE_FONT_BOLD)
    {
        bBold =TRUE;
    }
    else
    {
       bBold=FALSE;
    }
    
    if (dy > 0)
    {
        IDIB    *pDIB;

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
#if defined (FEATURE_ARPHIC_LAYOUT_ENGINE) && !defined (FEATURE_ARABIA_SANMU_LAYOUT)
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
    yend = (dy > CH_USER1_HEIGHT)?(CH_USER1_HEIGHT):(dy);
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
    byte xWidth, xWidthOrig, xxDisp, bit_mask, *sp, *pFontData;
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
	//MSG_FATAL("DrawChar.....................nChars=%d",nChars,0,0);
 
    nPitch >>= 1;
 
    for (i=0; i<nChars && pcText[i]; i++)
    {
        if (x > xMax)
        {
            break;
        }
        ch = pcText[i];
		//MSG_FATAL("DrawChar.....................ch=%d",ch,0,0);
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
        	//MSG_FATAL("ch==========%d",ch,0,0);
            index = gbIndex12[ch];
			//MSG_FATAL("index==========%x",index,0,0);
            if ( 0x5537 == index  )
            {
                ERR("missing char array(%d)",ch,0,0);
            }

#ifdef FEATURE_ARABIA_SANMU_LAYOUT
            // fe80 - feff 是阿拉伯字母变形的范围 变形的字母不加 1 个像素
            if((uint16)ch < 65152 || (uint16)ch > 65279)
            {
                xWidth = gbWidth12[index]+1; // left 1 pixel when display
            }
            else
            {
                xWidth = gbWidth12[index];
            }
#else            
            
            xWidth = gbWidth12[index]+1; // left 1 pixel when display
#endif            
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
            //MSG_FATAL("DrawChar...........ChineseNormalFontBuf12..........",0,0,0);
            pFontData = (byte*)ChineseNormalFontBuf12 + (index*CH_NORMAL_HEIGHT*2) + oy * bytes_per_row;
            if(dy>CH_NORMAL_HEIGHT) dy=CH_NORMAL_HEIGHT;
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
	//MSG_FATAL("dispWidth=======%d",dispWidth,0,0);
 
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
