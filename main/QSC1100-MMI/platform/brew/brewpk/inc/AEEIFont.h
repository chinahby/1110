#ifndef  AEEIFONT_H
#define  AEEIFONT_H
/*=============================================================================
FILE:          AEEIFont.h

SERVICES:      IFont interface

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2002-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"
#include "AEEIBitmap.h"

#define AEEIID_IFont 0x1001022

// IFONT_WIDTHALL can be used for the nMaxWidth parameter to MeasureText to
// measure all characters.
//
#define IFONT_MAXWIDTH   ((int)(((unsigned int)-1) >> 1)) // MAXINT

// Flags for DrawText():
//
#define IFONT_DRAW_TRANSPARENT     0x00008000            // post-3.0

typedef struct {
   int16  nAscent;
   int16  nDescent;
} AEEFontInfo;

#if defined(FEATURE_ARPHIC_LAYOUT_ENGINE)
#define INHERIT_IFont(iname) \
   INHERIT_IQI(iname); \
   int   (*DrawText)               (iname *po, IBitmap *pDst, int x, int y,       \
                                    const AECHAR *pcText, int nChars,             \
                                    NativeColor fg, NativeColor bg,               \
                                    const AEERect *prcClip, uint32 dwFlags);      \
   int   (*MeasureText)            (iname *po, const AECHAR *pcText, int nChars,  \
                                    int nMaxWidth, int *pnFits, int *pnPixels);   \
   int   (*GetInfo)                (iname *po, AEEFontInfo *pinfo, int nSize)   \
   int   (*MeasureTextCursorPos) (IFont *pMe,  int x, const AECHAR *pcText, int nChars,\
                                            const AEERect *prcClip, int* curx, int LineCursor, uint32 dwFlags);
#else
#define INHERIT_IFont(iname) \
   INHERIT_IQI(iname); \
   int   (*DrawText)               (iname *po, IBitmap *pDst, int x, int y,       \
                                    const AECHAR *pcText, int nChars,             \
                                    NativeColor fg, NativeColor bg,               \
                                    const AEERect *prcClip, uint32 dwFlags);      \
   int   (*MeasureText)            (iname *po, const AECHAR *pcText, int nChars,  \
                                    int nMaxWidth, int *pnFits, int *pnPixels);   \
   int   (*GetInfo)                (iname *po, AEEFontInfo *pinfo, int nSize)   
#endif


AEEINTERFACE_DEFINE(IFont);

static __inline uint32 IFont_AddRef(IFont *p)
{
   return AEEGETPVTBL(p, IFont)->AddRef((p));
}
static __inline uint32 IFont_Release(IFont *p)
{
   return AEEGETPVTBL(p, IFont)->Release((p));
}
static __inline int IFont_QueryInterface(IFont *p, AEEIID i, void **o)
{
   return AEEGETPVTBL(p, IFont)->QueryInterface((p),(i),(o));
}
static __inline int IFont_DrawText(IFont *p, IBitmap *d, int x, int y, const AECHAR *t, int n, NativeColor fg, NativeColor bg, const AEERect *r, uint32 w)
{
   return AEEGETPVTBL(p, IFont)->DrawText((p),(d),(x),(y),(t),(n),(fg),(bg),(r),(w));
}
static __inline int IFont_MeasureText(IFont *p, const AECHAR *t, int c, int w, int *n, int *np)
{
   return AEEGETPVTBL(p, IFont)->MeasureText((p),(t),(c),(w),(n),(np));
}
static __inline int IFont_GetInfo(IFont *p, AEEFontInfo *i, int n)
{
   return AEEGETPVTBL(p, IFont)->GetInfo((p),(i),(n));
}

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
#define  IFONT_MeasureTextCursorPos(p,x,t,n,r,cx,lc,w)  AEEGETPVTBL((p),IFont)->MeasureTextCursorPos((p),(x),(t),(n),(r),(cx),(lc),(w))
#endif

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEEFontInfo

Description:

   This structure describes characteristics of a font.

   Character cell height and baseline offset are properties of a font; the
   values do not change from character to character within a font.

   Each character's graphical representation is bounded by a rectangle called
   a _character cell_.  The height of this cell is a property of the font, and
   cell widths can vary from character to character.  When text is drawn
   opaquely, each character's cell is drawn to a same-sized rectangle in the
   destination bitmap, in which the pixels are set to either the background
   color or the foreground color.  When a character is drawn transparently,
   only the foreground pixels are drawn, and pixels that would otherwise take
   on the background color are left unchanged.

   A font's baseline is important for readability when different fonts are
   used on the same line of text.  The baseline divides the character cell
   into descent and ascent areas.  The descent area contains character
   descenders -- portions of a glyph that extend below the bottom of most
   characters -- and any spacing included at the bottom of the character cell.
   When different fonts are used on the same line of text, knowledge of the
   location of the baseline within the character cell allows the application
   to align the baselines by adjusting the vertical positioning of the
   characters.

   This figure shows two adjacent character cells from a single font,
   illustrating how nAscent and nDescent relate to the baseline and the height
   of the font:
 ===pre>
                   _____  
      -----------   |
      ---@-------   |
      --@-@------   |
      -@---@---@-   | ascent = 7
      -@@@@@-----   |
      -@---@---@-   |
      -@---@---@- __|_________ baseline
      ---------@-   |
      -------@@--   | descent = 3
      ----------- __|__
  
      total height = ascent + descent = 7 + 3 = 10

===/pre>


Definition:
   typedef struct {
      int16  nAscent;
      int16  nDescent;
   } AEEFontInfo;

Members:
   nAscent: Maximum number of pixels that character cells extend above the baseline.~
   nDescent: Maximum number of pixels that character cells extend below the baseline.

Comments:
    This structure may be extended in the future by adding new fields to the end.

See Also:
   IFont_GetInfo()

===============================================================================
*/

/*
===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IFont

Description: 

   This interface provides functions for drawing and measuring text.

   Applications will not typically call IFont member functions directly.
   IDisplay provides a friendlier and more convenient interface to drawing
   text, with support for system colors and clipping state, as well as many
   features related to drawing text, such as underlining, centering, framing,
   and erasing backgrounds.  IFont's functions are stricter about arguments
   and less flexible in defaulting values.  IDisplay builds on IFont, any
   IFont can be passed to IDisplay for it to manage, so using IFont directly
   offers no increase in functionality.

   IFont is an interface with multiple implementations.  Some IFont classes
   might work only with a particular class of IBitmap, whereas others will
   work with any IBitmap that supports 1-bit blits.  By default, IDisplay uses
   IFont objects provided by the OEM.  These can differ in size and appearance
   from handset to handset, and some may not support offscreen bitmaps.

The following header file is required:~
AEEFont.h

===============================================================================

Function: IFont_AddRef()

This function is inherited from IQI_AddRef().

===============================================================================

Function: IFont_Release()

This function is inherited from IQI_Release().

===============================================================================

Function: IFont_QueryInterface()

This function is inherited from IQI_QueryInterface().

===============================================================================

Function: IFont_DrawText()

Description:

   This function draws text into a bitmap.

   x and y describe the placement of the top-left corner of the left-most
   character cell.

   The drawing operation will be limited to the rectangle *prcClip.  Any
   portions of the text that fall outside of this rectangle (or outside the
   bounds of the bitmap) will not be drawn.  Clipping affects whether or not
   pixels are drawn; clipping never affects where things are drawn.

   All parameters to IFont_DrawText() should be valid.  Invalid or special values
   are not interpreted to have special meanings, as in IDisplay.

   The dwFlag parameter consists of a set of bit flags that select certain
   options.  Currently, only IFONT_DRAW_TRANSPARENT is defined.  When
   IFONT_DRAW_TRANSPARENT is specified, only the foreground (the glyph) is
   drawn and the background is not filled.  Otherwise, the background area of
   each character cell is also drawn.  (Both background and foreground are
   subject to the specified clipping rectangle.)

Prototype:
   int IFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y,
                      const AECHAR *pcText, int nChars,
                      NativeColor foreground, NativeColor background,
                      const AEERect *prcClip, uint32 dwFlags);

Parameters:
   pMe        [in]: Pointer to IFont interface
   pDst       [in/out]: Pointer to the destination IBitmap
   x          [in]: x-coordinate of the left of the text
   y          [in]: y-coordinate of the top of the text
   pcText     [in]: pointer to characters to be drawn
   nChars     [in]: number of characters in pcText[] to be drawn
   foreground [in]: color to draw the text (NativeColor)
   background [in]: color to draw the background (NativeColor)
   prcClip    [in]: clipping rectangle in which the text
                    string must be drawn
   dwFlags    [in]: bitmask of flags that affect drawing

Return Value:

   SUCCESS: is returned if the function was able to draw the text string.
   EUNSUPPORTED: if the underlying IBitmap does not support operations required
   by the font.~
   Other errors may be returned (implementation dependent).

Comments: 

   Negative x and y values are legal and indicate a starting position to the
   left of or above the top of the bitmap.  In such cases, any portion of the
   text that extends into the bitmap (and into *prcClip) will be drawn.

   When the width or height of *prcClip is negative, nothing will be drawn.

   Note that foreground and background color values are NativeColor values,
   not RGBVALs.  These can be obtained by calling the destination bitmap's
   IBITMAP_RGBToNative() member function.

   Implementation note: For future compatibility, font implementations should
   quietly ignore flag bits other than IFONT_DRAW_TRANSPARENT (i.e. those not
   yet defined in this version of the header file).

   
Side Effects: 
   None.

Version:
   Introduced BREW Client 2.0

See Also:
   IBITMAP_RGBToNative()
   IFont_MeasureText()

===============================================================================

Function: IFont_MeasureText()

Description:

   This function measures the width of text.  It calculates the number of
   characters that will fit in the given maximum width as well as the number
   of pixels that those characters will take up.

   All parameters to MeasureText() must be valid.  Invalid or special values
   are _not_ interpreted to have special meanings, as in IDisplay.

Prototype:

   int IFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars,
                         int nMaxWidth, int *pnFits, int *pnPixels);

Parameters:

   pMe        [in]: Pointer to IFont interface.

   pcText     [in]: text string to be measured in pixels.  Zero-termination
                    is not recognized; nChars must be accurate.

   nChars     [in]: number of AECHARs to measure.  Zero indicates an empty string
                    (width 0).  Negative values may not be passed.

   nMaxWidth [in]: maximum available width in pixels.  Zero means zero.
                   Use IFONT_MAXWIDTH to measure all characters.

   pnFits    [out]: number of characters that can fit in the available width.
                    This must be a valid pointer (not NULL).

   pnPixels  [out]: total width of the characters that fit.  This must be a
                    valid pointer (not NULL).

Return Value:
   SUCCESS: if successful~
   Other implementation specific error codes

Comments: 
   None.

Side Effects: 
   None.

Version:
   Introduced BREW Client 2.0

See Also:
   IFont_GetInfo()

===============================================================================

Function: IFont_GetInfo()

Description:

   This function fill the AEEFontInfo structure with information about the font.

   The size of the structure is passed for backward compatibility.  The
   implementor should only fill the structure up to the specified size.
   If the size is bigger that than the size of AEEFontInfo, this function
   should return EUNSUPPORTED.

Prototype:
   
   int IFont_GetInfo(IFont *pMe, AEEFontInfo *pinfo, int nSize);

Parameters:

   pMe        [in]: Pointer to IFont interface.
   pinfo     [out]: Pointer to AEEFontInfo structure to fill.
   nSize      [in]: Size of structure to fill.

Return Value:

   SUCCESS: on success
   EUNSUPPORTED: if the version (determined by nSize) is not supported.

Comments:
   
   IFont_GetInfo() should *always* succeed when a valid 'pinfo' pointer
   is passed and nSize is equal to sizeof(AEEFontInfo).


   In future releases, other structures with different sizes may be supported.

Side Effects: 
   None.

Version:
   Introduced BREW Client 2.0

See Also:
   AEEFontInfo, IFont_MeasureText()

===============================================================================
*/

#endif /* AEEIFONT_H */
