/*=================================================================================
FILE:          FontBidiUtil.c

SERVICES:      Bidirectional Font specific utilities.

PUBLIC CLASSES:IFontBidiUtil

DESCRIPTION:   This file describes an interface of several bidirectional font 
               utilities.  A reference implementation of this interface is provided.  
               It is assumed the OEM will create a version that supports their
               right to left fonts.

                Copyright © 1999-2006 QUALCOMM Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEFontBidiUtil.h"
#include "AEEShell.h"
#include "wutil.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// FontBidiUtil
typedef struct 
{
   const AEEVTBL(IFontBidiUtil) *pvt;
   uint32  m_nRefs;
} FontBidiUtil;

// BidiTransitionTable
typedef struct _BidiTransitionTable {
	byte byType;
	AECHAR wStart;
} BidiTransitionTable;


/*---------------------------------------------------------------------------------
      Globals
---------------------------------------------------------------------------------*/

// where to start char type binary search
#define BIDICHARTYPE_SEARCH_SKEW 46

// the directional type of each Unicode character
#define BIDITRANSITION_TABLE_SIZE   362
static const BidiTransitionTable TransitionTable[BIDITRANSITION_TABLE_SIZE+1] = { 
BIDICHARTYPE_BN,  0x0000, 	BIDICHARTYPE_S,   0x0009, 	BIDICHARTYPE_B,   0x000A, 	BIDICHARTYPE_S,   0x000B, 	BIDICHARTYPE_WS,  0x000C, 	BIDICHARTYPE_B,   0x000D, 	BIDICHARTYPE_BN,  0x000E, 	
BIDICHARTYPE_B,   0x001C, 	BIDICHARTYPE_S,   0x001F, 	BIDICHARTYPE_WS,  0x0020, 	BIDICHARTYPE_ON,  0x0021, 	BIDICHARTYPE_ET,  0x0023, 	BIDICHARTYPE_ON,  0x0026, 	BIDICHARTYPE_ET,  0x002B, 	
BIDICHARTYPE_CS,  0x002C, 	BIDICHARTYPE_ET,  0x002D, 	BIDICHARTYPE_CS,  0x002E, 	BIDICHARTYPE_ES,  0x002F, 	BIDICHARTYPE_EN,  0x0030, 	BIDICHARTYPE_CS,  0x003A, 	BIDICHARTYPE_ON,  0x003B, 	
BIDICHARTYPE_L,   0x0041, 	BIDICHARTYPE_ON,  0x005B, 	BIDICHARTYPE_L,   0x0061, 	BIDICHARTYPE_ON,  0x007B, 	BIDICHARTYPE_BN,  0x007F, 	BIDICHARTYPE_B,   0x0085, 	BIDICHARTYPE_BN,  0x0086, 	
BIDICHARTYPE_CS,  0x00A0, 	BIDICHARTYPE_ON,  0x00A1, 	BIDICHARTYPE_ET,  0x00A2, 	BIDICHARTYPE_ON,  0x00A6, 	BIDICHARTYPE_L,   0x00AA, 	BIDICHARTYPE_ON,  0x00AB, 	BIDICHARTYPE_ET,  0x00B0, 	
BIDICHARTYPE_EN,  0x00B2, 	BIDICHARTYPE_ON,  0x00B4, 	BIDICHARTYPE_L,   0x00B5, 	BIDICHARTYPE_ON,  0x00B6, 	BIDICHARTYPE_EN,  0x00B9, 	BIDICHARTYPE_L,   0x00BA, 	BIDICHARTYPE_ON,  0x00BB, 	
BIDICHARTYPE_L,   0x00C0, 	BIDICHARTYPE_ON,  0x00D7, 	BIDICHARTYPE_L,   0x00D8, 	BIDICHARTYPE_ON,  0x00F7, 	BIDICHARTYPE_L,   0x00F8, 	BIDICHARTYPE_ON,  0x02B9, 	BIDICHARTYPE_L,   0x02BB, 	
BIDICHARTYPE_ON,  0x02C2, 	BIDICHARTYPE_L,   0x02D0, 	BIDICHARTYPE_ON,  0x02D2, 	BIDICHARTYPE_L,   0x02E0, 	BIDICHARTYPE_ON,  0x02E5, 	BIDICHARTYPE_L,   0x02EE, 	BIDICHARTYPE_NSM, 0x0300, 	
BIDICHARTYPE_ON,  0x0374, 	BIDICHARTYPE_L,   0x037A, 	BIDICHARTYPE_ON,  0x037E, 	BIDICHARTYPE_L,   0x0386, 	BIDICHARTYPE_ON,  0x0387, 	BIDICHARTYPE_L,   0x0388, 	BIDICHARTYPE_NSM, 0x0483, 	
BIDICHARTYPE_L,   0x048C, 	BIDICHARTYPE_ON,  0x058A, 	BIDICHARTYPE_NSM, 0x0591, 	BIDICHARTYPE_R,   0x05BE, 	BIDICHARTYPE_NSM, 0x05BF, 	BIDICHARTYPE_R,   0x05C0, 	BIDICHARTYPE_NSM, 0x05C1, 	
BIDICHARTYPE_R,   0x05C3, 	BIDICHARTYPE_NSM, 0x05C4, 	BIDICHARTYPE_R,   0x05D0, 	BIDICHARTYPE_CS,  0x060C, 	BIDICHARTYPE_AL,  0x061B, 	BIDICHARTYPE_NSM, 0x064B, 	BIDICHARTYPE_AN,  0x0660, 	
BIDICHARTYPE_ET,  0x066A, 	BIDICHARTYPE_AN,  0x066B, 	BIDICHARTYPE_AL,  0x066D, 	BIDICHARTYPE_NSM, 0x0670, 	BIDICHARTYPE_AL,  0x0671, 	BIDICHARTYPE_NSM, 0x06D6, 	BIDICHARTYPE_AL,  0x06E5, 	
BIDICHARTYPE_NSM, 0x06E7, 	BIDICHARTYPE_ON,  0x06E9, 	BIDICHARTYPE_NSM, 0x06EA, 	BIDICHARTYPE_EN,  0x06F0, 	BIDICHARTYPE_AL,  0x06FA, 	BIDICHARTYPE_BN,  0x070F, 	BIDICHARTYPE_AL,  0x0710, 	
BIDICHARTYPE_NSM, 0x0711, 	BIDICHARTYPE_AL,  0x0712, 	BIDICHARTYPE_NSM, 0x0730, 	BIDICHARTYPE_AL,  0x0780, 	BIDICHARTYPE_NSM, 0x07A6, 	BIDICHARTYPE_L,   0x0903, 	BIDICHARTYPE_NSM, 0x093C, 	
BIDICHARTYPE_L,   0x093D, 	BIDICHARTYPE_NSM, 0x0941, 	BIDICHARTYPE_L,   0x0949, 	BIDICHARTYPE_NSM, 0x094D, 	BIDICHARTYPE_L,   0x0950, 	BIDICHARTYPE_NSM, 0x0951, 	BIDICHARTYPE_L,   0x0958, 	
BIDICHARTYPE_NSM, 0x0962, 	BIDICHARTYPE_L,   0x0964, 	BIDICHARTYPE_NSM, 0x0981, 	BIDICHARTYPE_L,   0x0982, 	BIDICHARTYPE_NSM, 0x09BC, 	BIDICHARTYPE_L,   0x09BE, 	BIDICHARTYPE_NSM, 0x09C1, 	
BIDICHARTYPE_L,   0x09C7, 	BIDICHARTYPE_NSM, 0x09CD, 	BIDICHARTYPE_L,   0x09D7, 	BIDICHARTYPE_NSM, 0x09E2, 	BIDICHARTYPE_L,   0x09E6, 	BIDICHARTYPE_ET,  0x09F2, 	BIDICHARTYPE_L,   0x09F4, 	
BIDICHARTYPE_NSM, 0x0A02, 	BIDICHARTYPE_L,   0x0A05, 	BIDICHARTYPE_NSM, 0x0A3C, 	BIDICHARTYPE_L,   0x0A3E, 	BIDICHARTYPE_NSM, 0x0A41, 	BIDICHARTYPE_L,   0x0A59, 	BIDICHARTYPE_NSM, 0x0A70, 	
BIDICHARTYPE_L,   0x0A72, 	BIDICHARTYPE_NSM, 0x0A81, 	BIDICHARTYPE_L,   0x0A83, 	BIDICHARTYPE_NSM, 0x0ABC, 	BIDICHARTYPE_L,   0x0ABD, 	BIDICHARTYPE_NSM, 0x0AC1, 	BIDICHARTYPE_L,   0x0AC9, 	
BIDICHARTYPE_NSM, 0x0ACD, 	BIDICHARTYPE_L,   0x0AD0, 	BIDICHARTYPE_NSM, 0x0B01, 	BIDICHARTYPE_L,   0x0B02, 	BIDICHARTYPE_NSM, 0x0B3C, 	BIDICHARTYPE_L,   0x0B3D, 	BIDICHARTYPE_NSM, 0x0B3F, 	
BIDICHARTYPE_L,   0x0B40, 	BIDICHARTYPE_NSM, 0x0B41, 	BIDICHARTYPE_L,   0x0B47, 	BIDICHARTYPE_NSM, 0x0B4D, 	BIDICHARTYPE_L,   0x0B57, 	BIDICHARTYPE_NSM, 0x0B82, 	BIDICHARTYPE_L,   0x0B83, 	
BIDICHARTYPE_NSM, 0x0BC0, 	BIDICHARTYPE_L,   0x0BC1, 	BIDICHARTYPE_NSM, 0x0BCD, 	BIDICHARTYPE_L,   0x0BD7, 	BIDICHARTYPE_NSM, 0x0C3E, 	BIDICHARTYPE_L,   0x0C41, 	BIDICHARTYPE_NSM, 0x0C46, 	
BIDICHARTYPE_L,   0x0C60, 	BIDICHARTYPE_NSM, 0x0CBF, 	BIDICHARTYPE_L,   0x0CC0, 	BIDICHARTYPE_NSM, 0x0CC6, 	BIDICHARTYPE_L,   0x0CC7, 	BIDICHARTYPE_NSM, 0x0CCC, 	BIDICHARTYPE_L,   0x0CD5, 	
BIDICHARTYPE_NSM, 0x0D41, 	BIDICHARTYPE_L,   0x0D46, 	BIDICHARTYPE_NSM, 0x0D4D, 	BIDICHARTYPE_L,   0x0D57, 	BIDICHARTYPE_NSM, 0x0DCA, 	BIDICHARTYPE_L,   0x0DCF, 	BIDICHARTYPE_NSM, 0x0DD2, 	
BIDICHARTYPE_L,   0x0DD8, 	BIDICHARTYPE_NSM, 0x0E31, 	BIDICHARTYPE_L,   0x0E32,  BIDICHARTYPE_NSM, 0x0E34, 	BIDICHARTYPE_ET,  0x0E3F, 	BIDICHARTYPE_L,   0x0E40, 	BIDICHARTYPE_NSM, 0x0E47, 	
BIDICHARTYPE_L,   0x0E4F, 	BIDICHARTYPE_NSM, 0x0EB1, 	BIDICHARTYPE_L,   0x0EB2, 	BIDICHARTYPE_NSM, 0x0EB4, 	BIDICHARTYPE_L,   0x0EBD, 	BIDICHARTYPE_NSM, 0x0EC8, 	BIDICHARTYPE_L,   0x0ED0, 	
BIDICHARTYPE_NSM, 0x0F18, 	BIDICHARTYPE_L,   0x0F1A, 	BIDICHARTYPE_NSM, 0x0F35, 	BIDICHARTYPE_L,   0x0F36, 	BIDICHARTYPE_NSM, 0x0F37, 	BIDICHARTYPE_L,   0x0F38, 	BIDICHARTYPE_NSM, 0x0F39, 	
BIDICHARTYPE_ON,  0x0F3A, 	BIDICHARTYPE_L,   0x0F3E, 	BIDICHARTYPE_NSM, 0x0F71, 	BIDICHARTYPE_L,   0x0F7F, 	BIDICHARTYPE_NSM, 0x0F80, 	BIDICHARTYPE_L,   0x0F85, 	BIDICHARTYPE_NSM, 0x0F86, 	
BIDICHARTYPE_L,   0x0F88, 	BIDICHARTYPE_NSM, 0x0F90, 	BIDICHARTYPE_L,   0x0FBE, 	BIDICHARTYPE_NSM, 0x0FC6, 	BIDICHARTYPE_L,   0x0FC7, 	BIDICHARTYPE_NSM, 0x102D, 	BIDICHARTYPE_L,   0x1031, 	
BIDICHARTYPE_NSM, 0x1032, 	BIDICHARTYPE_L,   0x1038, 	BIDICHARTYPE_NSM, 0x1039, 	BIDICHARTYPE_L,   0x1040, 	BIDICHARTYPE_NSM, 0x1058, 	BIDICHARTYPE_L,   0x10A0, 	BIDICHARTYPE_WS,  0x1680, 	
BIDICHARTYPE_L,   0x1681, 	BIDICHARTYPE_ON,  0x169B, 	BIDICHARTYPE_L,   0x16A0,	BIDICHARTYPE_NSM, 0x17B7, 	BIDICHARTYPE_L,   0x17BE, 	BIDICHARTYPE_NSM, 0x17C6, 	BIDICHARTYPE_L,   0x17C7, 	
BIDICHARTYPE_NSM, 0x17C9, 	BIDICHARTYPE_L,   0x17D4, 	BIDICHARTYPE_ET,  0x17DB, 	BIDICHARTYPE_L,   0x17DC, 	BIDICHARTYPE_ON,  0x1800, 	BIDICHARTYPE_BN,  0x180B, 	BIDICHARTYPE_L,   0x1810, 	
BIDICHARTYPE_NSM, 0x18A9, 	BIDICHARTYPE_L,   0x1E00, 	BIDICHARTYPE_ON,  0x1FBD, 	BIDICHARTYPE_L,   0x1FBE, 	BIDICHARTYPE_ON,  0x1FBF, 	BIDICHARTYPE_L,   0x1FC2, 	BIDICHARTYPE_ON,  0x1FCD, 	
BIDICHARTYPE_L,   0x1FD0, 	BIDICHARTYPE_ON,  0x1FDD, 	BIDICHARTYPE_L,   0x1FE0, 	BIDICHARTYPE_ON,  0x1FED, 	BIDICHARTYPE_L,   0x1FF2, 	BIDICHARTYPE_ON,  0x1FFD, 	BIDICHARTYPE_WS,  0x2000, 	
BIDICHARTYPE_BN,  0x200B, 	BIDICHARTYPE_L,   0x200E, 	BIDICHARTYPE_R,   0x200F, 	BIDICHARTYPE_ON,  0x2010, 	BIDICHARTYPE_WS,  0x2028, 	BIDICHARTYPE_B,   0x2029, 	BIDICHARTYPE_LRE, 0x202A, 	
BIDICHARTYPE_RLE, 0x202B, 	BIDICHARTYPE_PDF, 0x202C, 	BIDICHARTYPE_LRO, 0x202D, 	BIDICHARTYPE_RLO, 0x202E, 	BIDICHARTYPE_WS,  0x202F, 	BIDICHARTYPE_ET,  0x2030, 	BIDICHARTYPE_ON,  0x2035, 	
BIDICHARTYPE_BN,  0x206A, 	BIDICHARTYPE_EN,  0x2070, 	BIDICHARTYPE_ET,  0x207A, 	BIDICHARTYPE_ON,  0x207C, 	BIDICHARTYPE_L,   0x207F, 	BIDICHARTYPE_EN,  0x2080, 	BIDICHARTYPE_ET,  0x208A, 	
BIDICHARTYPE_ON,  0x208C, 	BIDICHARTYPE_ET,  0x20A0, 	BIDICHARTYPE_NSM, 0x20D0, 	BIDICHARTYPE_ON,  0x2100, 	BIDICHARTYPE_L,   0x2102, 	BIDICHARTYPE_ON,  0x2103, 	BIDICHARTYPE_L,   0x2107, 	
BIDICHARTYPE_ON,  0x2108, 	BIDICHARTYPE_L,   0x210A, 	BIDICHARTYPE_ON,  0x2114, 	BIDICHARTYPE_L,   0x2115, 	BIDICHARTYPE_ON,  0x2116, 	BIDICHARTYPE_L,   0x2119, 	BIDICHARTYPE_ON,  0x211E, 	
BIDICHARTYPE_L,   0x2124, 	BIDICHARTYPE_ON,  0x2125, 	BIDICHARTYPE_L,   0x2126, 	BIDICHARTYPE_ON,  0x2127, 	BIDICHARTYPE_L,   0x2128, 	BIDICHARTYPE_ON,  0x2129, 	BIDICHARTYPE_L,   0x212A, 	
BIDICHARTYPE_ET,  0x212E, 	BIDICHARTYPE_L,   0x212F, 	BIDICHARTYPE_ON,  0x2132, 	BIDICHARTYPE_L,   0x2133, 	BIDICHARTYPE_ON,  0x213A, 	BIDICHARTYPE_L,   0x2160, 	BIDICHARTYPE_ON,  0x2190, 	
BIDICHARTYPE_ET,  0x2212, 	BIDICHARTYPE_ON,  0x2214, 	BIDICHARTYPE_L,   0x2336, 	BIDICHARTYPE_ON,  0x237B, 	BIDICHARTYPE_L,   0x2395, 	BIDICHARTYPE_ON,  0x2396, 	BIDICHARTYPE_EN,  0x2460, 	
BIDICHARTYPE_L,   0x249C, 	BIDICHARTYPE_EN,  0x24EA, 	BIDICHARTYPE_ON,  0x2500, 	BIDICHARTYPE_WS,  0x3000, 	BIDICHARTYPE_ON,  0x3001, 	BIDICHARTYPE_L,   0x3005, 	BIDICHARTYPE_ON,  0x3008, 	
BIDICHARTYPE_L,   0x3021, 	BIDICHARTYPE_NSM, 0x302A, 	BIDICHARTYPE_ON,  0x3030, 	BIDICHARTYPE_L,   0x3031, 	BIDICHARTYPE_ON,  0x3036, 	BIDICHARTYPE_L,   0x3038, 	BIDICHARTYPE_ON,  0x303E, 	
BIDICHARTYPE_L,   0x3041, 	BIDICHARTYPE_NSM, 0x3099, 	BIDICHARTYPE_ON,  0x309B, 	BIDICHARTYPE_L,   0x309D, 	BIDICHARTYPE_ON,  0x30FB, 	BIDICHARTYPE_L,   0x30FC, 	BIDICHARTYPE_ON,  0xA490, 	
BIDICHARTYPE_L,   0xAC00, 	BIDICHARTYPE_R,   0xFB1D, 	BIDICHARTYPE_NSM, 0xFB1E, 	BIDICHARTYPE_R,   0xFB1F, 	BIDICHARTYPE_ET,  0xFB29, 	BIDICHARTYPE_R,   0xFB2A, 	BIDICHARTYPE_AL,  0xFB50, 	
BIDICHARTYPE_ON,  0xFD3E, 	BIDICHARTYPE_AL,  0xFD50, 	BIDICHARTYPE_NSM, 0xFE20, 	BIDICHARTYPE_ON,  0xFE30, 	BIDICHARTYPE_CS,  0xFE50, 	BIDICHARTYPE_ON,  0xFE51, 	BIDICHARTYPE_CS,  0xFE52, 	
BIDICHARTYPE_ON,  0xFE54, 	BIDICHARTYPE_CS,  0xFE55, 	BIDICHARTYPE_ON,  0xFE56, 	BIDICHARTYPE_ET,  0xFE5F, 	BIDICHARTYPE_ON,  0xFE60, 	BIDICHARTYPE_ET,  0xFE62, 	BIDICHARTYPE_ON,  0xFE64, 	
BIDICHARTYPE_ET,  0xFE69, 	BIDICHARTYPE_ON,  0xFE6B, 	BIDICHARTYPE_AL,  0xFE70, 	BIDICHARTYPE_BN,  0xFEFF, 	BIDICHARTYPE_ON,  0xFF01, 	BIDICHARTYPE_ET,  0xFF03, 	BIDICHARTYPE_ON,  0xFF06, 	
BIDICHARTYPE_ET,  0xFF0B, 	BIDICHARTYPE_CS,  0xFF0C, 	BIDICHARTYPE_ET,  0xFF0D, 	BIDICHARTYPE_CS,  0xFF0E, 	BIDICHARTYPE_ES,  0xFF0F, 	BIDICHARTYPE_EN,  0xFF10, 	BIDICHARTYPE_CS,  0xFF1A, 	
BIDICHARTYPE_ON,  0xFF1B, 	BIDICHARTYPE_L,   0xFF21, 	BIDICHARTYPE_ON,  0xFF3B, 	BIDICHARTYPE_L,   0xFF41, 	BIDICHARTYPE_ON,  0xFF5B, 	BIDICHARTYPE_L,   0xFF66, 	BIDICHARTYPE_ET,  0xFFE0, 	
BIDICHARTYPE_ON,  0xFFE2, 	BIDICHARTYPE_ET,  0xFFE5, 	BIDICHARTYPE_ON,  0xFFE8, 	BIDICHARTYPE_BN,  0xFFF9, 	BIDICHARTYPE_ON,  0xFFFC, 
0x0000  // Table End.  Search routine counts on this sentinel.
};	

/*---------------------------------------------------------------------------------
      Function Prototypes
---------------------------------------------------------------------------------*/

static uint32  FontBidiUtil_AddRef(IFontBidiUtil* po);
static uint32  FontBidiUtil_Release(IFontBidiUtil* po);
static int     FontBidiUtil_QueryInterface(IFontBidiUtil *pMe, AEECLSID id, void **ppo); 
static int     FontBidiUtil_Characterize(IFontBidiUtil *po, const AECHAR *pwText, int dwLen, int dwPos, int *pdwCharType);
static int     FontBidiUtil_Locate(IFontBidiUtil *po, IDisplay *pIDisplay, AEEFont Font, const AECHAR *pwText, int dwLen, int dwStartIndex, int dwEndIndex, AEERect *prcBounds, int *pdwCursorHint); 
static int     FontBidiUtil_NextDisplayChar(IFontBidiUtil *po, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset); 
static int     FontBidiUtil_Breakline(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwLineStart, int dwClip, int *pdwLineEnd, int *pdwFits);
static int     FontBidiUtil_CharLocation(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, AEERect *prcBounds);
static int     FontBidiUtil_CharAt(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwX, int *pdwCharIdx);
static int     FontBidiUtil_CaretInfo(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int *pdwX);
static int     FontBidiUtil_MoveCaretIndex(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset, int *pdwNextChar);

// Local routines
static int UnicodeCharClass(AECHAR c);

/*---------------------------------------------------------------------------------
      IBidiFontUtil
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 FontBidiUtil_AddRef(IFontBidiUtil* po)
{
   FontBidiUtil *pMe = (FontBidiUtil *)po;
   return ++pMe->m_nRefs;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static uint32 FontBidiUtil_Release(IFontBidiUtil* po)
{
   FontBidiUtil *pMe = (FontBidiUtil *)po;

    if (--pMe->m_nRefs)
      return pMe->m_nRefs;

   FREE(pMe);
   return 0;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int FontBidiUtil_QueryInterface(IFontBidiUtil *po, AEECLSID id, void **ppo)
{
   FontBidiUtil *pMe = (FontBidiUtil *)po;

   if ((id == AEEIID_FONTBIDIUTIL) || (id == AEECLSID_FONTBIDIUTIL_1)) 
   {
      *ppo = pMe;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } 
   else 
   {
      return ECLASSNOTSUPPORT;
   }
}
 
/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int FontBidiUtil_Characterize(IFontBidiUtil *po, const AECHAR *pwText, int dwLen, int dwPos, int *pdwCharType)
{
   const AECHAR *pwSpot = pwText;
   int dwDirCat = BIDICHARTYPECAT_NEUTRAL;
   int dwCharType;

   // validate params
   if (pwText == NULL) {
      return -1;
   }

   // ensure nChars
   if (dwLen == -1)
      dwLen = WSTRLEN(pwText);

   // initialize pdwCharType
   if (pdwCharType)
      *pdwCharType = BIDICHARTYPE_AMBIGOUS;

   // scan through characters
   while ((dwDirCat != BIDICHARTYPECAT_STRONGRIGHT)      // the text direction has
       && (dwDirCat != BIDICHARTYPECAT_STRONGLEFT)       // not yet been determined
       && pwSpot                                         // there's text to scan
       && (pwSpot < (pwText + dwLen)))                   // we're not at the end
   {
      // determine the class of this character
      dwCharType = UnicodeCharClass(*pwSpot);

      // determine the direction of this series of characters.  The 
      // direction is determined by the first occurence of a strong
      // left or right character.
      switch (dwCharType)
      {
         // strong left
         case BIDICHARTYPE_L:
         case BIDICHARTYPE_LRE:
         case BIDICHARTYPE_LRO:
            dwDirCat = BIDICHARTYPECAT_STRONGLEFT;
            break;

         // strong right
         case BIDICHARTYPE_R:
         case BIDICHARTYPE_AL:
         case BIDICHARTYPE_RLE:
         case BIDICHARTYPE_RLO:
            dwDirCat = BIDICHARTYPECAT_STRONGRIGHT;
            break;

         // weak
         case BIDICHARTYPE_PDF:
         case BIDICHARTYPE_EN:
         case BIDICHARTYPE_ES:
         case BIDICHARTYPE_ET:
         case BIDICHARTYPE_AN:
         case BIDICHARTYPE_CS:
         case BIDICHARTYPE_NSM:
         case BIDICHARTYPE_BN:
            dwDirCat = BIDICHARTYPECAT_WEAK;
            break;

         // neutral
         case BIDICHARTYPE_B:
         case BIDICHARTYPE_S:
         case BIDICHARTYPE_WS:
         case BIDICHARTYPE_ON:
         default:
            dwDirCat = BIDICHARTYPECAT_NEUTRAL;
            break;
      }

      // return the character type if appropriate
      if (pdwCharType && (dwLen == 1))
         *pdwCharType = dwCharType;

      // check next character
      pwSpot++;
   }
   return dwDirCat;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */

static int     FontBidiUtil_Locate(IFontBidiUtil *po, IDisplay *pIDisplay, AEEFont Font, const AECHAR *pwText, int dwLen, int dwStartIndex, int dwEndIndex, AEERect *prcBounds, int *pdwCursorHint)
{
   return EUNSUPPORTED;
}

static int     FontBidiUtil_NextDisplayChar(IFontBidiUtil *po, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset)
{
   return EUNSUPPORTED;
}

static int FontBidiUtil_Breakline(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwLineStart, int dwClip, int *pcxLineEnd, int *pdwFits )
{

   int dwResult;
   int cx = 0;
   int fits;
   const AECHAR *pw;
   const AECHAR* pwLineStart;
   int linefits;
   int dwStart;
   int cxResult = 0;
   int fitsResult = 0;

   // find the start character for this line.
   dwResult = IFONT_MeasureText(pif, pwText, dwLen, dwLineStart, &dwStart, &cx);
   pwLineStart  = pwText + dwStart;
   dwLen  -= dwStart;
   if ( dwLen > 0 ) {


      // find the end character for this line
      dwResult = IFONT_MeasureText(pif, pwLineStart, dwLen, dwClip, &fits, &cx);

      linefits = fits;

      if (dwResult == SUCCESS) {

         // scan for any hard-breaks
         {
            int nn = 0;
            pw = pwLineStart;
            for (; nn < fits; nn++) {
               if (*pw++ == '\n') {
                  fits = nn;
                  break;
               }
            }
         }

         if (fits < dwLen) {

            pw = pwLineStart + fits;

            // if we broke at white space, consider it (the whitespace)
            // to fit on the line also
            if (W_ISWHITESPACE(*pw)) {
               fits++;
   
            } else {
            // if we broke at non-whitespace, scan back
            // until we find whitespace
               while (pw > pwLineStart && !W_ISWHITESPACE(*(pw-1))) {
                  pw--;
               }
               if (pw > pwLineStart) {
                  fits = pw-pwLineStart;
               }
            }
         }

         if ( fits != linefits ) {
            int dummy;
            dwResult = IFONT_MeasureText(pif, pwLineStart, fits, 0x7FFF, &dummy, &cx);
         }


         cxResult = dwLineStart + cx;
         fitsResult  = fits;
      }
   }
   if ( dwResult == SUCCESS ) {
      if ( pcxLineEnd ) {
         *pcxLineEnd = cxResult;
      }
      if ( pdwFits ) {
         *pdwFits = fitsResult;
      }
   }


   return dwResult;
}


/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int FontBidiUtil_CharLocation(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, AEERect *prcBounds)
{
   /* Reference Implementation

      This routine should be modified by the OEM to take the display order into account!
   */

   // this reference implementaion works on uni-directional text only.
  
   int dwResult;
   int nFits, x;
   int dx = 0;

   // validate params
   if (pif == NULL || pwText == NULL) {
      return -1;
   }

   // calculate length if needed
   if (dwLen == -1)
      dwLen = WSTRLEN(pwText);

   // verify parameters
   if ((dwIndex < 0) 
    || (dwIndex >= dwLen) )
    return (EINVALIDRANGE);

   // where does this character start?
   dwResult = IFONT_MeasureText(pif, pwText, dwIndex, IFONT_MAXWIDTH, &nFits, &x);
   if (dwResult == SUCCESS) {

      // how wide is this range of characters?
      dwResult = IFONT_MeasureText(pif, pwText+dwIndex, 1, IFONT_MAXWIDTH, &nFits, &dx);
      if (dwResult == SUCCESS) {

         AEEFontInfo fi;
         IFONT_GetInfo(pif, &fi, sizeof(fi));
         // return the area that this character will be rendered
         SETAEERECT(prcBounds, x, 0, dx, 0);//fi.nAscent + fi.nDescent);
      }
   }

   return dwResult;
}

/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int FontBidiUtil_CharAt(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwX, int *pdwCharIdx)
{
   int dwResult;
   int nFits, nPixels;

   /* Reference Implementation

      This routine should be modified by the OEM to take the display order into account!
   */

   // this reference implementaion works on uni-directional, bitmap text only.

   // return which character (pdwCharIdx) occurs at dwX pixels into the line.  

   // verify parameters
   if (!pif || !pwText || !pdwCharIdx) {
      return EBADPARM;
   }

   // how many characters fit up until dwX?
   dwResult = IFONT_MeasureText(pif, pwText, dwLen, dwX, &nFits, &nPixels);
   if (dwResult == SUCCESS) {
      
      // The nFits'th character fits on this line.
      *pdwCharIdx = nFits;

      // exact fit?
      if (nPixels != dwX) {
         
         // part of the next character fits as well.
         (*pdwCharIdx)++;
      }
   }

   return dwResult;
}


static int FontBidiUtil_CaretInfo(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int *pdwX)
{
   int dwResult;
   int nFits, nPixels;

   /* Reference Implementation

      This routine should be modified by the OEM to take the display order into account!
   */

   // this reference implementaion works on uni-directional, bitmap text only.

   // verify parameters
   if (!pif || !pwText || !pdwX || dwLen < 0 ) {
      return EBADPARM;
   }

   // how many pixels to the beginning of this character?
   dwResult = IFONT_MeasureText(pif, pwText, dwIndex, IFONT_MAXWIDTH, &nFits, &nPixels);
   if (dwResult == SUCCESS) {
      // The nFits'th character fits on this line.
      *pdwX = nPixels;
   }
   else {
      *pdwX = 0;
   }

   return dwResult;
}



/* -----------------------------------------------------------------------
    Public function - see interface definition      
   --- */
static int FontBidiUtil_MoveCaretIndex(IFontBidiUtil *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset, int *pdwNextChar)
{
   /* Reference Implementation

      This routine should be modified by the OEM to take the display order into account!
   */

   // validate params
   if (pwText == NULL || pdwNextChar == NULL) {
      return EBADPARM;
   }

   // this reference implementaion works on uni-directional text only.
   *pdwNextChar = dwIndex + dwOffset;

   return SUCCESS;
}

/*---------------------------------------------------------------------------------
      CreateInstance
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    FontBidiUtil_New  
    
    Note: The reference implementation of IFontBidiUtil only knows about
    Unicode characters.  If the default encoding of the device isn't
    Unicode, this class won't work as expected.

    The OEM implementation of IFontBidiUtil most likely will work for
    only Unicode data as well.
   --- */
int FontBidiUtil_New(IShell *pIShell, void **ppObj)
{
   FontBidiUtil *pMe;
   //AEEDeviceInfo di;

   // This version, like most OEM implementations, only knows about Unicode characters.  
   // If we're not a unicode device, punt.  This object won't work as expected.
   //ISHELL_GetDeviceInfo(pIShell, &di);
   //if (di.wEncoding != AEE_ENC_UNICODE)
   //   return(EUNSUPPORTED);

   if(!ppObj)
		return(EFAILED);

   *ppObj = NULL;

   pMe = MALLOCREC_EX(FontBidiUtil,sizeof(AEEVTBL(IFontBidiUtil)));
   if(pMe) 
   {
		AEEVTBL(IFontBidiUtil) *pFuncs;
     
      // setup VTBL
		pMe->m_nRefs = 1;

      pFuncs = (AEEVTBL(IFontBidiUtil) *)((byte *)pMe + sizeof(FontBidiUtil));

      pFuncs->AddRef          = FontBidiUtil_AddRef;
      pFuncs->Release         = FontBidiUtil_Release;
      pFuncs->QueryInterface  = FontBidiUtil_QueryInterface;
		pFuncs->Characterize    = FontBidiUtil_Characterize;
		pFuncs->Locate          = FontBidiUtil_Locate;
		pFuncs->NextDisplayChar = FontBidiUtil_NextDisplayChar;
		pFuncs->Breakline       = FontBidiUtil_Breakline;
		pFuncs->CharAt          = FontBidiUtil_CharAt;
		pFuncs->CharLocation    = FontBidiUtil_CharLocation;
		pFuncs->MoveCaretIndex  = FontBidiUtil_MoveCaretIndex;
      pFuncs->CaretInfo       = FontBidiUtil_CaretInfo;

      pMe->pvt = pFuncs;

      // return new object
      *ppObj = (void *)pMe;
      return (SUCCESS);
	}

	return ENOMEMORY;
}

/*---------------------------------------------------------------------------------
      Local Routines
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
   UnicodeCharClass    

   Return the direction of a unicode character.  Does a binary search of the 
   Transition Table for better performance.
   --- */
static int UnicodeCharClass(AECHAR c)
{
   int dwLow, dwHigh, dwMid;

   // set up search ...
   dwLow = 0;
   dwHigh = BIDITRANSITION_TABLE_SIZE;

   // Start binary search at the end of ASCII text, the most common
   dwMid = BIDICHARTYPE_SEARCH_SKEW;       

   // this is guaranteed to find a match.
   while (1) {
      if ((c >= TransitionTable[dwMid].wStart)                                                  // is c at or beyond the current range?
         && ((c < TransitionTable[dwMid+1].wStart) || (TransitionTable[dwMid+1].wStart == 0)))  // and is it less than the next range or at the end of the table?
         return (TransitionTable[dwMid].byType);                                                // then we've found our match.
      else if (c < TransitionTable[dwMid].wStart)                                               // is c less that the current range?
         dwHigh = dwMid -1;                                                                     // then look in everythinh before the current range.
      else if (c > TransitionTable[dwMid].wStart)                                               // is c beyond the current range and the next?
         dwLow = dwMid + 1;                                                                     // then look in everything adter the current range

      // next chunk to search in ...
      dwMid = (dwLow + dwHigh) /2;  
   }
}
