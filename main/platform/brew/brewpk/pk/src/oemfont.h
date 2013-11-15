#ifndef OEMFONT_H_
#define OEMFONT_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
           Oemfont.h

GENERAL DESCRIPTION
  This file contains macros and definitions for font routines.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Copyright (c) 2005-2009 TeleEpoch Ltd.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
23/07/09   tyj      Initial revision
===========================================================================*/
#define FONTCHAR_COUNT 8192
//name rule: FONT + font type + size
#define FEATURE_USING_FONT_24//FONT_FRANKLIN_16
#define FEATURE_CN_UNICODE_FONT_LIB

#ifdef FEATURE_USING_FONT_20
#undef FONTCHAR_WIDTH
#undef FONTCHAR_HEIGHT
#define FONTCHAR_WIDTH          20
#define FONTCHAR_HEIGHT         20
#elif defined(FEATURE_USING_FONT_24)
#undef FONTCHAR_WIDTH
#undef FONTCHAR_HEIGHT
#define FONTCHAR_WIDTH          24
#define FONTCHAR_HEIGHT         24
#endif

#ifdef FEATURE_QTM12
#define FONT_NONAME_20
#endif

// lzhao for Q6
#if defined(FEATURE_CN_UNICODE_FONT_LIB)
#define CN_CHAR_START			0x4Ebe	//unicode of the beginning of chinese char
#define CN_CHAR_END				0x9FA5  //unicode of the end of chinese char
#define CN_CHAR_START_LINE		4		//starting line number(0-based index) for chinese char
#define FONT_PER_LINE      (6144/24) //width of original bitmap/ width of char

typedef struct __font_table {
	uint16 col;					//col number of the char specified by unicode_start in the font bmp
	uint16 row;					//row number of the char specified by unicode_start in the font bmp
	uint16 unicode_start;		//starting char in unicode
	uint16 unicode_end;			//ending char in unicode	
}font_table;

static font_table font_map[] =
{
	{
		0,
		CN_CHAR_START_LINE,
		CN_CHAR_START,
		CN_CHAR_END
	},

    {//zi stroke bihua/pianpang
		0,
		0,
		0,
		0
	},
};
#endif

typedef struct {
    const AEEVTBL(IFont) *pvt;
    uint32 uRefs;
    IDIB * pdib;
    IBitmap * pbitmap;
    const uint16 * pwGlyphs;
    int cntGlyphs;
    int wUndefGlyph;        // glyph to be used when character lies outside pwGlyphs[]
    int xWid;               // width of each full-width character
    int yHgt;               // height of each character
    int yDescent;           // descent of each character
    int nHalfChars;         // number of characters to treat as half-width
    AEECLSID cls; 
}OEMFont;

#define OEMFONT_Release(p) AEEGETPVTBL((p), IFont)->Release((p))

#endif // OEMFONT_H_

