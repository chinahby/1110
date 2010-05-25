/*===========================================================================
FILE:  aeemenuctlutils.c

SERVICES:  aeemenuctlex util object Handlers

GENERAL DESCRIPTION: 

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

========================================================================== */

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //brewapps/bam2/HEAD/aeemenuctlex/aeemenuctlutils.c#2 $

===========================================================================*/
#include "aeemenuctlutils.h"
#include "AEEStdLib.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"

typedef enum {
	BIDICLASS_L,		// Left-to-Right 
	BIDICLASS_LRE,	   // Left-to-Right Embedding 
	BIDICLASS_LRO,	   // Left-to-Right Override 
	BIDICLASS_R,		// Right-to-Left 
	BIDICLASS_AL,		// Right-to-Left Arabic 
	BIDICLASS_RLE,	   // Right-to-Left Embedding 
	BIDICLASS_RLO,	   // Right-to-Left Override 
	BIDICLASS_PDF,	   // Pop Directional Format 
	BIDICLASS_EN,		// European Number 
	BIDICLASS_ES,		// European Number Separator 
	BIDICLASS_ET,		// European Number Terminator 
	BIDICLASS_AN,		// Arabic Number 
	BIDICLASS_CS,		// Common Number Separator 
	BIDICLASS_NSM,	   // Non-Spacing Mark 
	BIDICLASS_BN,		// Boundary Neutral 
	BIDICLASS_B,	   // Paragraph Separator 
	BIDICLASS_S,		// Segment Separator 
	BIDICLASS_WS,		// Whitespace 
	BIDICLASS_ON		// Other Neutrals 
} BIDECLASSEnum;

typedef struct _BidiTransitionTable {
	byte byType;
	AECHAR wStart;
} BidiTransitionTable;

#define BIDICLASS_SEARCH_SKEW 46
#define BIDITRANSITION_TABLE_SIZE   362

static const BidiTransitionTable TransitionTable[BIDITRANSITION_TABLE_SIZE+1] = { 
	BIDICLASS_BN,	0x0000,
	BIDICLASS_S,	0x0009,
	BIDICLASS_B,	0x000A,
	BIDICLASS_S,	0x000B,
	BIDICLASS_WS,	0x000C,
	BIDICLASS_B,	0x000D,
	BIDICLASS_BN,	0x000E,
	BIDICLASS_B,	0x001C,
	BIDICLASS_S,	0x001F,
	BIDICLASS_WS,	0x0020,
	BIDICLASS_ON,	0x0021,
	BIDICLASS_ET,	0x0023,
	BIDICLASS_ON,	0x0026,
	BIDICLASS_ET,	0x002B,
	BIDICLASS_CS,	0x002C,
	BIDICLASS_ET,	0x002D,
	BIDICLASS_CS,	0x002E,
	BIDICLASS_ES,	0x002F,
	BIDICLASS_EN,	0x0030,
	BIDICLASS_CS,	0x003A,
	BIDICLASS_ON,	0x003B,
	BIDICLASS_L,	0x0041,
	BIDICLASS_ON,	0x005B,
	BIDICLASS_L,	0x0061,
	BIDICLASS_ON,	0x007B,	
	BIDICLASS_BN,	0x007F,	
	BIDICLASS_B,	0x0085,
	BIDICLASS_BN,	0x0086,
	BIDICLASS_CS,	0x00A0,
	BIDICLASS_ON,	0x00A1,
	BIDICLASS_ET,	0x00A2,
	BIDICLASS_ON,	0x00A6,
	BIDICLASS_L,	0x00AA,
	BIDICLASS_ON,	0x00AB,
	BIDICLASS_ET,	0x00B0,
	BIDICLASS_EN,	0x00B2,
	BIDICLASS_ON,	0x00B4,
	BIDICLASS_L,	0x00B5,
	BIDICLASS_ON,	0x00B6,
	BIDICLASS_EN,	0x00B9,
	BIDICLASS_L,	0x00BA,
	BIDICLASS_ON,	0x00BB,
	BIDICLASS_L,	0x00C0,
	BIDICLASS_ON,	0x00D7,
	BIDICLASS_L,	0x00D8,
	BIDICLASS_ON,	0x00F7,
	BIDICLASS_L,	0x00F8,
	BIDICLASS_ON,	0x02B9,
	BIDICLASS_L,	0x02BB,
	BIDICLASS_ON,	0x02C2,
	BIDICLASS_L,	0x02D0,
	BIDICLASS_ON,	0x02D2,
	BIDICLASS_L,	0x02E0,
	BIDICLASS_ON,	0x02E5,
	BIDICLASS_L,	0x02EE,
	BIDICLASS_NSM,	0x0300,
	BIDICLASS_ON,	0x0374,
	BIDICLASS_L,	0x037A,
	BIDICLASS_ON,	0x037E,
	BIDICLASS_L,	0x0386,
	BIDICLASS_ON,	0x0387,
	BIDICLASS_L,	0x0388,
	BIDICLASS_NSM,	0x0483,
	BIDICLASS_L,	0x048C,
	BIDICLASS_ON,	0x058A,
	BIDICLASS_NSM,	0x0591,
	BIDICLASS_R,	0x05BE,
	BIDICLASS_NSM,	0x05BF,
	BIDICLASS_R,	0x05C0,
	BIDICLASS_NSM,	0x05C1,
	BIDICLASS_R,	0x05C3,
	BIDICLASS_NSM,	0x05C4,
	BIDICLASS_R,	0x05D0,
	BIDICLASS_CS,	0x060C,
	BIDICLASS_AL,	0x061B,
	BIDICLASS_NSM,	0x064B,
	BIDICLASS_AN,	0x0660,
	BIDICLASS_ET,	0x066A,
	BIDICLASS_AN,	0x066B,	
	BIDICLASS_AL,	0x066D,
	BIDICLASS_NSM,	0x0670,
	BIDICLASS_AL,	0x0671,
	BIDICLASS_NSM,	0x06D6,
	BIDICLASS_AL,	0x06E5,
	BIDICLASS_NSM,	0x06E7,
	BIDICLASS_ON,	0x06E9,
	BIDICLASS_NSM,	0x06EA,
	BIDICLASS_EN,	0x06F0,
	BIDICLASS_AL,	0x06FA,
	BIDICLASS_BN,	0x070F,
	BIDICLASS_AL,	0x0710,
	BIDICLASS_NSM,	0x0711,
	BIDICLASS_AL,	0x0712,
	BIDICLASS_NSM,	0x0730,
	BIDICLASS_AL,	0x0780,
	BIDICLASS_NSM,	0x07A6,
	BIDICLASS_L,	0x0903,
	BIDICLASS_NSM,	0x093C,
	BIDICLASS_L,	0x093D,
	BIDICLASS_NSM,	0x0941,
	BIDICLASS_L,	0x0949,
	BIDICLASS_NSM,	0x094D,
	BIDICLASS_L,	0x0950,
	BIDICLASS_NSM,	0x0951,
	BIDICLASS_L,	0x0958,
	BIDICLASS_NSM,	0x0962,
	BIDICLASS_L,	0x0964,
	BIDICLASS_NSM,	0x0981,
	BIDICLASS_L,	0x0982,
	BIDICLASS_NSM,	0x09BC,
	BIDICLASS_L,	0x09BE,
	BIDICLASS_NSM,	0x09C1,
	BIDICLASS_L,	0x09C7,
	BIDICLASS_NSM,	0x09CD,
	BIDICLASS_L,	0x09D7,
	BIDICLASS_NSM,	0x09E2,
	BIDICLASS_L,	0x09E6,
	BIDICLASS_ET,	0x09F2,	
	BIDICLASS_L,	0x09F4,
	BIDICLASS_NSM,	0x0A02,
	BIDICLASS_L,	0x0A05,
	BIDICLASS_NSM,	0x0A3C,
	BIDICLASS_L,	0x0A3E,
	BIDICLASS_NSM,	0x0A41,
	BIDICLASS_L,	0x0A59,
	BIDICLASS_NSM,	0x0A70,
	BIDICLASS_L,	0x0A72,
	BIDICLASS_NSM,	0x0A81,
	BIDICLASS_L,	0x0A83,
	BIDICLASS_NSM,	0x0ABC,
	BIDICLASS_L,	0x0ABD,
	BIDICLASS_NSM,	0x0AC1,
	BIDICLASS_L,	0x0AC9,
	BIDICLASS_NSM,	0x0ACD,
	BIDICLASS_L,	0x0AD0,
	BIDICLASS_NSM,	0x0B01,
	BIDICLASS_L,	0x0B02,
	BIDICLASS_NSM,	0x0B3C,
	BIDICLASS_L,	0x0B3D,
	BIDICLASS_NSM,	0x0B3F,
	BIDICLASS_L,	0x0B40,
	BIDICLASS_NSM,	0x0B41,
	BIDICLASS_L,	0x0B47,
	BIDICLASS_NSM,	0x0B4D,
	BIDICLASS_L,	0x0B57,
	BIDICLASS_NSM,	0x0B82,
	BIDICLASS_L,	0x0B83,
	BIDICLASS_NSM,	0x0BC0,
	BIDICLASS_L,	0x0BC1,
	BIDICLASS_NSM,	0x0BCD,
	BIDICLASS_L,	0x0BD7,	
	BIDICLASS_NSM,	0x0C3E,
	BIDICLASS_L,	0x0C41,
	BIDICLASS_NSM,	0x0C46,
	BIDICLASS_L,	0x0C60,
	BIDICLASS_NSM,	0x0CBF,
	BIDICLASS_L,	0x0CC0,
	BIDICLASS_NSM,	0x0CC6,
	BIDICLASS_L,	0x0CC7,
	BIDICLASS_NSM,	0x0CCC,	
	BIDICLASS_L,	0x0CD5,
	BIDICLASS_NSM,	0x0D41,	
	BIDICLASS_L,	0x0D46,
	BIDICLASS_NSM,	0x0D4D,
	BIDICLASS_L,	0x0D57,
	BIDICLASS_NSM,	0x0DCA,
	BIDICLASS_L,	0x0DCF,
	BIDICLASS_NSM,	0x0DD2,
	BIDICLASS_L,	0x0DD8,
	BIDICLASS_NSM,	0x0E31,
	BIDICLASS_L,	0x0E32,	
	BIDICLASS_NSM,	0x0E34,
	BIDICLASS_ET,	0x0E3F,
	BIDICLASS_L,	0x0E40,
	BIDICLASS_NSM,	0x0E47,
	BIDICLASS_L,	0x0E4F,
	BIDICLASS_NSM,	0x0EB1,
	BIDICLASS_L,	0x0EB2,	
	BIDICLASS_NSM,	0x0EB4,
	BIDICLASS_L,	0x0EBD,
	BIDICLASS_NSM,	0x0EC8,
	BIDICLASS_L,	0x0ED0,
	BIDICLASS_NSM,	0x0F18,
	BIDICLASS_L,	0x0F1A,
	BIDICLASS_NSM,	0x0F35,
	BIDICLASS_L,	0x0F36,
	BIDICLASS_NSM,	0x0F37,
	BIDICLASS_L,	0x0F38,
	BIDICLASS_NSM,	0x0F39,
	BIDICLASS_ON,	0x0F3A,
	BIDICLASS_L,	0x0F3E,
	BIDICLASS_NSM,	0x0F71,
	BIDICLASS_L,	0x0F7F,
	BIDICLASS_NSM,	0x0F80,
	BIDICLASS_L,	0x0F85,
	BIDICLASS_NSM,	0x0F86,	
	BIDICLASS_L,	0x0F88,
	BIDICLASS_NSM,	0x0F90,
	BIDICLASS_L,	0x0FBE,
	BIDICLASS_NSM,	0x0FC6,
	BIDICLASS_L,	0x0FC7,
	BIDICLASS_NSM,	0x102D,
	BIDICLASS_L,	0x1031,
	BIDICLASS_NSM,	0x1032,
	BIDICLASS_L,	0x1038,
	BIDICLASS_NSM,	0x1039,
	BIDICLASS_L,	0x1040,
	BIDICLASS_NSM,	0x1058,
	BIDICLASS_L,	0x10A0,
	BIDICLASS_WS,	0x1680,
	BIDICLASS_L,	0x1681,
	BIDICLASS_ON,	0x169B,	
	BIDICLASS_L,	0x16A0,
	BIDICLASS_NSM,	0x17B7,
	BIDICLASS_L,	0x17BE,
	BIDICLASS_NSM,	0x17C6,
	BIDICLASS_L,	0x17C7,
	BIDICLASS_NSM,	0x17C9,
	BIDICLASS_L,	0x17D4,
	BIDICLASS_ET,	0x17DB,
	BIDICLASS_L,	0x17DC,
	BIDICLASS_ON,	0x1800,
	BIDICLASS_BN,	0x180B,
	BIDICLASS_L,	0x1810,
	BIDICLASS_NSM,	0x18A9,
	BIDICLASS_L,	0x1E00,
	BIDICLASS_ON,	0x1FBD,
	BIDICLASS_L,	0x1FBE,
	BIDICLASS_ON,	0x1FBF,
	BIDICLASS_L,	0x1FC2,
	BIDICLASS_ON,	0x1FCD,
	BIDICLASS_L,	0x1FD0,
	BIDICLASS_ON,	0x1FDD,
	BIDICLASS_L,	0x1FE0,
	BIDICLASS_ON,	0x1FED,
	BIDICLASS_L,	0x1FF2,
	BIDICLASS_ON,	0x1FFD,
	BIDICLASS_WS,	0x2000,
	BIDICLASS_BN,	0x200B,
	BIDICLASS_L,	0x200E,
	BIDICLASS_R,	0x200F,
	BIDICLASS_ON,	0x2010,
	BIDICLASS_WS,	0x2028,
	BIDICLASS_B,	0x2029,
	BIDICLASS_LRE,	0x202A,
	BIDICLASS_RLE,	0x202B,
	BIDICLASS_PDF,	0x202C,
	BIDICLASS_LRO,	0x202D,
	BIDICLASS_RLO,	0x202E,
	BIDICLASS_WS,  0x202F,
	BIDICLASS_ET,  0x2030,
	BIDICLASS_ON,  0x2035,	
	BIDICLASS_BN,  0x206A,	
	BIDICLASS_EN,  0x2070,	
	BIDICLASS_ET,  0x207A,	
	BIDICLASS_ON,  0x207C,	
	BIDICLASS_L,	0x207F,
	BIDICLASS_EN,	0x2080,
	BIDICLASS_ET,	0x208A,
	BIDICLASS_ON,	0x208C,
	BIDICLASS_ET,	0x20A0,
	BIDICLASS_NSM,	0x20D0,
	BIDICLASS_ON,	0x2100,	
	BIDICLASS_L,	0x2102,
	BIDICLASS_ON,	0x2103,
	BIDICLASS_L,	0x2107,
	BIDICLASS_ON,	0x2108,
	BIDICLASS_L,	0x210A,
	BIDICLASS_ON,	0x2114,
	BIDICLASS_L,	0x2115,
	BIDICLASS_ON,	0x2116,	
	BIDICLASS_L,	0x2119,
	BIDICLASS_ON,	0x211E,
	BIDICLASS_L,	0x2124,
	BIDICLASS_ON,	0x2125,
	BIDICLASS_L,	0x2126,
	BIDICLASS_ON,	0x2127,
	BIDICLASS_L,	0x2128,
	BIDICLASS_ON,	0x2129,
	BIDICLASS_L,	0x212A,
	BIDICLASS_ET,	0x212E,
	BIDICLASS_L,	0x212F,
	BIDICLASS_ON,	0x2132,
	BIDICLASS_L,	0x2133,
	BIDICLASS_ON,	0x213A,
	BIDICLASS_L,	0x2160,
	BIDICLASS_ON,	0x2190,
	BIDICLASS_ET,	0x2212,
	BIDICLASS_ON,	0x2214,
	BIDICLASS_L,	0x2336,
	BIDICLASS_ON,	0x237B,
	BIDICLASS_L,	0x2395,
	BIDICLASS_ON,	0x2396,
	BIDICLASS_EN,	0x2460,
	BIDICLASS_L,	0x249C,
	BIDICLASS_EN,	0x24EA,
	BIDICLASS_ON,	0x2500,
	BIDICLASS_WS,	0x3000,
	BIDICLASS_ON,	0x3001,
	BIDICLASS_L,	0x3005,
	BIDICLASS_ON,	0x3008,
	BIDICLASS_L,	0x3021,
	BIDICLASS_NSM,	0x302A,
	BIDICLASS_ON,	0x3030,
	BIDICLASS_L,	0x3031,
	BIDICLASS_ON,	0x3036,
	BIDICLASS_L,	0x3038,	
	BIDICLASS_ON,	0x303E,	
	BIDICLASS_L,	0x3041,
	BIDICLASS_NSM,	0x3099,
	BIDICLASS_ON,	0x309B,	
	BIDICLASS_L,	0x309D,
	BIDICLASS_ON,	0x30FB,
	BIDICLASS_L,	0x30FC,
	BIDICLASS_ON,	0xA490,
	BIDICLASS_L,	0xAC00,
	BIDICLASS_R,	0xFB1D,
	BIDICLASS_NSM,	0xFB1E,
	BIDICLASS_R,	0xFB1F,
	BIDICLASS_ET,	0xFB29,
	BIDICLASS_R,	0xFB2A,	
	BIDICLASS_AL,	0xFB50,
	BIDICLASS_ON,	0xFD3E,	
	BIDICLASS_AL,	0xFD50,
	BIDICLASS_NSM,	0xFE20,
	BIDICLASS_ON,	0xFE30,
	BIDICLASS_CS,	0xFE50,
	BIDICLASS_ON,	0xFE51,
	BIDICLASS_CS,	0xFE52,
	BIDICLASS_ON,	0xFE54,
	BIDICLASS_CS,	0xFE55,
	BIDICLASS_ON,	0xFE56,
	BIDICLASS_ET,	0xFE5F,
	BIDICLASS_ON,	0xFE60,	
	BIDICLASS_ET,	0xFE62,	
	BIDICLASS_ON,	0xFE64,
	BIDICLASS_ET,	0xFE69,	
	BIDICLASS_ON,	0xFE6B,
	BIDICLASS_AL,	0xFE70,
	BIDICLASS_BN,	0xFEFF,
	BIDICLASS_ON,	0xFF01,	
	BIDICLASS_ET,	0xFF03,
	BIDICLASS_ON,	0xFF06,
	BIDICLASS_ET,	0xFF0B,
	BIDICLASS_CS,	0xFF0C,
	BIDICLASS_ET,	0xFF0D,
	BIDICLASS_CS,	0xFF0E,
	BIDICLASS_ES,	0xFF0F,
	BIDICLASS_EN,	0xFF10,
	BIDICLASS_CS,	0xFF1A,
	BIDICLASS_ON,	0xFF1B,	
	BIDICLASS_L,	0xFF21,
	BIDICLASS_ON,	0xFF3B,	
	BIDICLASS_L,	0xFF41,
	BIDICLASS_ON,	0xFF5B,
	BIDICLASS_L,	0xFF66,
	BIDICLASS_ET,	0xFFE0,
	BIDICLASS_ON,	0xFFE2,
	BIDICLASS_ET,	0xFFE5,
	BIDICLASS_ON,	0xFFE8,
	BIDICLASS_BN,	0xFFF9,
	BIDICLASS_ON,	0xFFFC,
	0,		         0	            // Table End.  Search routine counts on this sentinel.
};	

static BIDECLASSEnum UnicodeCharClass(AECHAR c);

/*===========================================================================

FUNCTION 
   ParagraphAlignment()

DESCRIPTION
   Given a paragraph of text, this routine determines if it will be rendered
   Left to Right or Right to Left, according to the Unicode 3.0 bidirectional
   display algorithm.

   This assumes, of course, that the text in the wide string passed in is
   Unicode.  So before we do anything, check the phone's default encoding
   from the DeviceInfo and make sure what we're looking at is Unicode.  If
   it's not  then don't run the alogirthm at all.  Just return the default
   left to right alignment.

DEPENDENCIES
  Assumes pszText and nLen describe a paragraph of text.

RETURN VALUE
   IDF_ALIGN_LEFT if the paragraph has an embedding level of 0, Left To Right
   FALSE if 1, Right to Left.

SIDE EFFECTS
    none
===========================================================================*/
uint32 ParagraphAlignment(CMenuCtl* pme, AECHAR *pszText, int nChars)
{
   AECHAR *pszSpot = pszText;
   AEEDeviceInfo di;
   
   // determine the handset's default ewncoding.  Once.
   if (pme->m_bHasTextEncodingInfo == FALSE) 
   {
      ISHELL_GetDeviceInfo(pme->m_pIShell, &di);
      pme->m_wTextEncoding = di.wEncoding;
      pme->m_bHasTextEncodingInfo = TRUE;
   }

   // return Left to Right if this handset doesn't do Unicode.
   switch (pme->m_wTextEncoding) {
      // only apply the Unicode Bidirectional Algorithm to Unicode characters.
      case AEE_ENC_UNICODE:
         break;     

      // Otherwise, we haven't been passed Unicode characters.  Punt.  Nicely.
      default:
         return (IDF_ALIGN_LEFT);
   }

   // determine this paragraph's base direction, which is the same as 
   // the directionality of the first strong character in the paragraph.
   while (pszSpot && (pszSpot < (pszText + nChars))) {
      switch (UnicodeCharClass(*pszSpot)) {
         // strong left (L)
         case BIDICLASS_L:
            return (IDF_ALIGN_LEFT);

         // strong right (R or AL)
         case BIDICLASS_R:
         case BIDICLASS_AL:
            return (IDF_ALIGN_RIGHT);
          
         // neither strong left or right
         default:
            break;
      }
      // check next character
      pszSpot++;
   }

   // Left to Right by default
   return IDF_ALIGN_LEFT;
}


/*===========================================================================
   Return the BIDICLASS (direction) of a unicode character.  Does a binary
   search of the TransitionTable for better performance.
===========================================================================*/
BIDECLASSEnum UnicodeCharClass(AECHAR c)
{
   int dwLow, dwHigh, dwMid;

   // set up search ...
   dwLow = 0;
   dwHigh = BIDITRANSITION_TABLE_SIZE;

   // Start binary search at the end of ASCII text, the most common
   dwMid = BIDICLASS_SEARCH_SKEW;       

   // this is guaranteed to find a match.
   while (1) {
      if ((c >= TransitionTable[dwMid].wStart)                                                           // is c at or beyond the current range?
         && ((c < TransitionTable[dwMid+1].wStart) || (TransitionTable[dwMid+1].wStart == 0)))           // and is it less than the next range or at the end of the table?
         return ((BIDECLASSEnum)TransitionTable[dwMid].byType);                                          // then we've found our match.
      else if (c < TransitionTable[dwMid].wStart)                                                        // is c less that the current range?
         dwHigh = dwMid -1;                                                                              // then look in everythinh before the current range.
      else if (c > TransitionTable[dwMid].wStart)                                                        // is c beyond the current range and the next?
         dwLow = dwMid + 1;                                                                              // then look in everything adter the current range

      // next chunk to search in ...
      dwMid = (dwLow + dwHigh) /2;  
   }
}

