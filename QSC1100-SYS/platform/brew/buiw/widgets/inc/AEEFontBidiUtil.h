/*
  ========================================================================

  FILE:           AEEFontBidiUtil.h

  SERVICES:       Bidirectional font utitlities

  PUBLIC CLASSES: IFontBidiUtil

  DESCRIPTION:    IFont utilities for measuring text, breaking lines and
                  determining appropriate cursor locations.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEFONTBIDIUTIL_H__
#define __AEEFONTBIDIUTIL_H__

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEDisp.h"
#include "AEEFont.h"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/

// uniquely identify the IFontBidiUtil interface.
#include "bid/AEEIID_FONTBIDIUTIL.BID"
#include "bid/AEECLSID_FONTBIDIUTIL.bid"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// error codes
#define EINVALIDRANGE   EFAILED

// BidiCharType - directional character type of Unicode characters
#define BIDICHARTYPE_L        0
#define BIDICHARTYPE_LRE      1
#define BIDICHARTYPE_LRO      2   
#define BIDICHARTYPE_R        3  
#define BIDICHARTYPE_AL       4
#define BIDICHARTYPE_RLE      5
#define BIDICHARTYPE_RLO      6
#define BIDICHARTYPE_PDF      7
#define BIDICHARTYPE_EN       8
#define BIDICHARTYPE_ES       9
#define BIDICHARTYPE_ET       10
#define BIDICHARTYPE_AN       11
#define BIDICHARTYPE_CS       12
#define BIDICHARTYPE_NSM      13
#define BIDICHARTYPE_BN       14
#define BIDICHARTYPE_B        15
#define BIDICHARTYPE_S        16
#define BIDICHARTYPE_WS       17
#define BIDICHARTYPE_ON       18
#define BIDICHARTYPE_AMBIGOUS 19

// BidiCharTypeCat - directional character type catagories
#define BIDICHARTYPECAT_STRONGLEFT     0
#define BIDICHARTYPECAT_STRONGRIGHT    1
#define BIDICHARTYPECAT_WEAK           2
#define BIDICHARTYPECAT_NEUTRAL        3

// BidiOrderTypes - used to indicate logical or visual character ordering
#define BIDI_LOGICAL_ORDER       0x00000000
#define BIDI_VISUAL_ORDER        0x00000001

// BidiCursorPositionHint - where to place the cursor
#define BCPH_RIGHT      0
#define BCPH_LEFT       1
#define BCPH_TOP        2
#define BCPH_BOTTOM     3
#define BCPH_AMBIGUOUS  4

// IFontBidiUtil class
#define INHERIT_IFontBidiUtil(iname)\
   INHERIT_IQueryInterface (iname); \
    int  (*Characterize)   (iname *po, const AECHAR *pwText, int dwLen, int dwIndex, int *pdwCharType); \
    int  (*Locate)         (iname *po, IDisplay *pIDisplay, AEEFont Font, const AECHAR *pwText, int dwLen, int dwStartIndex, int dwEndIndex, AEERect *prcBounds, int *pdwCursorHint); \
    int  (*NextDisplayChar)(iname *po, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset); \
    int  (*Breakline)       (iname *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwLineStart, int dwClip, int *pcxLineEnd, int *pdwFits ); \
    int  (*CharAt)          (iname *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwX, int *pdwCharIdx); \
    int  (*CharLocation)    (iname *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, AEERect *prcBounds); \
    int  (*CaretInfo)       (iname *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int* dwX ); \
    int  (*MoveCaretIndex)  (iname *po, IFont *pif, const AECHAR *pwText, int dwLen, int dwIndex, int dwOffset, int *pdwNextChar)

AEEINTERFACE_DEFINE(IFontBidiUtil);


#define  IFONTBIDIUTIL_AddRef(p)                      AEEGETPVTBL((p),IFontBidiUtil)->AddRef((p))
#define  IFONTBIDIUTIL_Release(p)                     AEEGETPVTBL((p),IFontBidiUtil)->Release((p))
#define  IFONTBIDIUTIL_QueryInterface(p,i,o)          AEEGETPVTBL((p),IFontBidiUtil)->QueryInterface((p),(i),(o))
#define  IFONTBIDIUTIL_Characterize(p,t,l,d,c)        AEEGETPVTBL((p),IFontBidiUtil)->Characterize((p),(t),(l),(d),(c))
#define  IFONTBIDIUTIL_Locate(p,d,f,t,l,s,e,r,h)      AEEGETPVTBL((p),IFontBidiUtil)->Locate((p),(d),(f),(t),(l),(s),(e),(r),(h))     
#define  IFONTBIDIUTIL_NextDisplayChar(p,t,l,i,o)     AEEGETPVTBL((p),IFontBidiUtil)->NextDisplayChar((p),(t),(l),(i),(o))
#define  IFONTBIDIUTIL_Breakline(p,f,t,l,s,c,x,d)     AEEGETPVTBL((p),IFontBidiUtil)->Breakline((p),(f),(t),(l),(s),(c),(x),(d))
#define  IFONTBIDIUTIL_CharAt(p,f,t,l,x,i)            AEEGETPVTBL((p),IFontBidiUtil)->CharAt((p),(f),(t),(l),(x),(i))      
#define  IFONTBIDIUTIL_CharLocation(p,f,t,l,i,x)      AEEGETPVTBL((p),IFontBidiUtil)->CharLocation((p),(f),(t),(l),(i),(x))     
#define  IFONTBIDIUTIL_MoveCaretIndex(p,f,t,l,i,o,c)  AEEGETPVTBL((p),IFontBidiUtil)->MoveCaretIndex((p),(f),(t),(l),(i),(o),(c))
#define  IFONTBIDIUTIL_CaretInfo(p,f,t,l,i,x)         AEEGETPVTBL((p),IFontBidiUtil)->CaretInfo((p),(f),(t),(l),(i),(x)) 


// other helpful macros
#define  IFONTBIDIUTIL_IsParagraphBreak(c) ((c == 0x000A) || (c == 0x2029))

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================

Bidirectional Character Types

Description:  
   Indicates the directional character type of a Unicode character.

Definition:
   BIDICHARTYPE_LRE        Left-to-Right Embedding 
   BIDICHARTYPE_LRO        Left-to-Right Override 
   BIDICHARTYPE_R          Right-to-Left 
   BIDICHARTYPE_AL         Right-to-Left Arabic 
   BIDICHARTYPE_RLE        Right-to-Left Embedding 
   BIDICHARTYPE_RLO        Right-to-Left Override
   BIDICHARTYPE_PDF        Pop Directional Format 
   BIDICHARTYPE_EN         European Number 
   BIDICHARTYPE_ES         European Number Separator 
   BIDICHARTYPE_ET         European Number Terminator 
   BIDICHARTYPE_AN         Arabic Number 
   BIDICHARTYPE_CS         Common Number Separator 
   BIDICHARTYPE_NSM        Non-Spacing Mark 
   BIDICHARTYPE_BN         Boundary Neutral 
   BIDICHARTYPE_B          Paragraph Separator 
   BIDICHARTYPE_S          Segment Separator 
   BIDICHARTYPE_WS         Whitespace 
   BIDICHARTYPE_ON         Other Neutrals 
   BIDICHARTYPE_AMBIGUOUS  Ambiguous

Comments:
   BIDICHARTYPE_AMBIGUOUS may be returned if a range of characters is
   being characterized and the characters within the range have different
   types.

See Also:
   IFONTBIDIUTIL_Characterize()

==============================================================================

Bidirectional Character Type Categories

Description:  
   Describes the default direction of one or more Unicode characters.  This is
   a generalization of the BidiCharType that labels characters as obviously
   left to right, right to left or neutral.

Definition:
   BIDICHARTYPECAT_STRONGLEFT   Strong Left-To-Right directional type
   BIDICHARTYPECAT_STRONGRIGHT  Strong Right-To-Left directional type
   BIDICHARTYPECAT_WEAK         Weak directional type 
   BIDICHARTYPECAT_NEUTRAL      Neutral directional type

Comments:
   This information can be used to determine the default direction of a
   series of Unicode characters and how they should be aligned on screen.

See Also:
   IFONTBIDIUTIL_Characterize()

==============================================================================

Bidirectional Order Types 

Description:  
   These options indicate how a character index should be interpreted.

   When a series of Unicode characters contains both left to right and
   right to left characters, the order of those characters in memory will
   differ from the order in which they're rendered on screen.

   The in-memory order of the characters is called the "logical order"
   and is identical to the order in which the characters would have been 
   entered by the user using a keypad, for example.

   When bidirectional text is displayed, the font implementation takes the 
   text in logical order.  It applies various algorithms on the characters 
   to lay them out into the proper order as defined by the Unicode standard
   then draws them on screen.  The order in which the characters appear on 
   screen is called the "visual order".

   When dealing with purely left to right text, the logical order and the
   visual order of the text are identical.  However, when dealing with
   bidirectional text, the logical and visual orders may differ significantly.
   Consider this simple example, where lower case characters represent
   left to right characters and upper case characters represent right to
   left characters:

===pre>
   The user enters:
   ABCD abcd

   The characters are stored in memory as:
   ABCD abcd

   The characters are displayed on screen as:
   abcd DCBA
===/pre>

   Certain IFontBidiUtil functions act on character indices.  When a function
   is able to interpret both logical and visual indices, it will allow the
   caller to specify which by passing one of the order types defined below.

Definition:
   BIDI_LOGICAL_ORDER   interpret the the index as an index into the logical 
                        order of the text

   BIDI_VISUAL_ORDER    interpret the the index as an index into the visual 
                        order of the text

Comments:
   If BIDI_LOGICAL_ORDER is specified in a call to IFONTBIDIUTIL_Locate(),
   the bounding box encompassing logical character dwStart to logical character
   dwEnd is returned.  Note, if this range represents a non-contiguois run
   of text on screen, an error will be returned.

   If BIDI_VISUAL_ORDER is specified in a call to IFONTBIDIUTIL_Locate(),
   the bounding box encompassing visual character dwStart to logical character
   dwEnd is returned.

See Also:
    IFONTBIDIUTIL_Locate()
    
==============================================================================

Cursor Position Hint

Description:  
   Indicates where the cusor should be placed in reference to the returned 
   character or chacter range.

Definition:
   BCPH_RIGHT      draw cursor to the right of the returned bounding rect
   BCPH_LEFT       draw cursor to the left of the returned bounding rect
   BCPH_TOP        draw cursor above the returned bounding rect
   BCPH_BOTTOM     draw cursor below the returned bounding rect
   BCPH_AMBIGUOUS  cursor location can't be determined 

Comments:
   BCPH_AMBIGUOUS may be returned if the character range in question is non-
   contiguous when displayed.

See Also:
    IFONTBIDIUTIL_Locate()

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

IFontBidiUtil Interface

Description: 
   This interface provides routines to perform various calculations on and 
   characterizations of Unicode characters that are drawn to the screen.
   Specifically, this interface provides a means to determine the direction 
   (Left-to-Right or Right-To-Left) of one or more Unicode characters,
   calculate where bidirectional characters are rendered on screen and
   map displayed characters back to characters in memory.

   These routines are necessary because of the added complexities of rendering 
   bidirectional text. The Unicode Standard prescribes a memory representation
   order know as logical order as well as an on screen representation known
   as display order.  For example, consider the following characters in memory
   and how they're displayed:
   
===pre>
   Logical Order                     Display Order
   (in memory)                       (on screen)

   a b c   A B C                     a b c   C B A
   0 1 2 3 4 5 6     
===/pre>
   
   When dealing with bidirectional Unicode text, the order of the text in memory 
   may differ greatly from the order of the text on screen.  In BREW, Unicode
   characters are passed to the font implementations in logical order.
   The font, in turn, applies the Unicode Bidirectional Display Alogirthm
   to the text to determine how it should be displayed then displays the
   characters properly.
   
   Developers writing text input controls, for example, may need to determine
   how blocks of text should be aligned, where individual characters end
   up on screen for caret placement and where in memory the next displayed
   character lies.  This interface provides these things.

   Note, this interface is intended for use on Unicode characters only.

Header File:
   AEEFontBidiUtil.h
   
==============================================================================
Function: IFONTBIDIUTIL_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: IFONTBIDIUTIL_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: IFONTBIDIUTIL_QueryInterface()

This function is inherited from IQI_QueryInterface().

===========================================================================

IFONTBIDIUTIL_Characterize()

Description:
   This function determines the direction of one or more Unicode 
   characters.  The direction of the text in question
   is returned as well as  the Unicode Bidirectional Character Type when
   appropriate.  IFONTBIDIUTIL_Characterize determines the direction or 
   Unicode characters according to the Bidirectional Rendering Algoritm 
   in the Unicode 3.0 spec.

   This routine can be used to determine the direction of a string of
   Unicode characters.  It can also be used to return the direction and
   actual Unicode character type of a single character when wStart and
   wEnd are equal.

   If this routine is passed a paragraph of Unicode text (that is, a block
   of text ending with a paragraph break mark), the BidiCharTypeCat returned
   can be used to determine the default direction of the paragraph.  If
   BIDICHARTYPECAT_STRONGLEFT is returned, the paragraph has a default
   direction of Left to Right and should be left aligned on screen.  If
   BIDICHARTYPECAT_STRONGRIGHT is returned, the paragraph has a default
   direction of Right to Left and should be right aligned on screen.

   This routine is generic and need not be implemented by the OEM.
   
Prototype:
   BidiCharTypeCat IFONTBIDIUTIL_Characterize(
      IFontBidiUtil *po, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwPos, 
      BidiCharType *pdwCharType);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  length of Unicode text
   dwPos       [in]  character we're interested in
   pdwCharType [out] the bidirectional character type of the character
===/pre>

Return Value:
   This routine returns a BidiCharTypeCat indicating the direction of the
   text passed in.

Comments:
   This routine is intended for Unicode text only.
  
Side Effects: 
   None

See Also:
   Bidirectional Character Type Categories
   Bidirectional Character Types

===========================================================================

IFONTBIDIUTIL_Locate()

Description:

   This function is no longer supported.

   Given an index and an optional range within a text string, return
   the screen position at which the character(s) will be rendered.  This 
   routine also returns where a cursor should be placed relative to the
   returned bounding box of the located characters.

   Applications that render text may at times require detailed information
   about where individual characters end up on screen.  This routine can
   be used to find the location of characters on screen given either 
   logical or visual indices.

   When a series of Unicode characters contains both left to right and
   right to left characters, the in memory order of those characters differs 
   from the order in which they're rendered on screen.  The in memory order
   of the characters is called the "logical order" and is identical to the
   order in which the characters would have been entered by the user.
   
   When bidirectional text is displayed in BREW, the font implementation is 
   given the text in logical order.  It applies the Unicode Bidirectional 
   Display Alogorithm (among other things) to the characters to determine 
   in which order they should be drawn on screen then draws them.  The order 
   in which the characters appear on screen is called the "visual order".

   When dealing with purely left to right text, the logical order and the
   visual order of the text are identical.  However, when dealing with
   bidirectional text, the logical and visual orders may differ significantly.
   Consider this simple example, where lower case characters represent
   left to right characters and upper case characters represent right to
   left characters:

===pre>
   The user enters:
   ABCD abcd

   The characters are stored in memory as:
   ABCD abcd

   The characters are displayed on screen as:
   abcd DCBA
===/pre>

   Note that the 'ABCD' in memory is rendered right to left, and the left to 
   right characters follow that string on its left.  The 1st logical character 
   ends up being the 9th visual character in this example.
   
   IFONTBIDIUTIL_Locate() provides applications a way to find out where any
   given character will be rendered by the font on screen.  The caller may
   pass in BIDI_LOGICAL_ORDER in dwOptions to request a run of characters 
   in the logical ordering be located, or BIDI_VISUAL_ORDER to request
   the bounding box of a run of characters on the display.


   BIDI_LOGICAL_ORDER

   When the caller specifies BIDI_LOGICAL_ORDER, IFONTBIDIUTIL_Locate()
   treats dwStart and dwEnd as indices into the text in logical order.
   Consider this example of setting the cursor position to the end of
   a block of bidirectional text.

===pre>
   Logical Order                     Display Order
   (in memory)                       (on screen)

   a b c   A B C                     a b c   C B A
   0 1 2 3 4 5 6                     
===/pre>

   Assume "C" was the last character entered and that the cursor needs to 
   be drawn immediately following that character.  The caller would need
   to know both where C is and, given the direction of that character, on
   which side of it to draw the cursor.  This routine can be used:

===pre>
   dwResult = IFONTBIDIUTIL_Locate(
               po, 
               pd, 
               pwText, 
               dwLen, 
               6, 
               6, 
               BIDI_LOGICAL_ORDER,
               prc, 
               pHint);
===/pre>

   dwLen = 7 is the length of the text, dwStart = dwEnd = 6 is the index
   into the in memory characters we're looking for.  On return, 
   prc contains the coordinates of where the "C" character is drawn.
   pHint would be set to BCPH_LEFT, indicating that the cursor should
   end up to the left of the "C", since "C" is a strong right character:

===pre>
   a b c   A B C|                     a b c  |C B A
   0 1 2 3 4 5 6 
===/pre>
             
   Note that this routine is unable to provide the location of characters
   that span multiple directions.  That is, the range of characters passed
   in must span characters of a single direction.  For example, if this routine 
   was asked about the character range 2,3,4, it would return an error:

===pre>
   a b c   A B C                     a b c   C B A
   0 1 2 3 4 5 6 
       ^^^^^                             ^^^^    ^
===pre/>

   dwResult = IFONTBIDIUTIL_Locate(po, pd, pwText, dwLen, 2, 4, prc, pHint)
   would return EINVALIDRANGE because the range of characters in
   question aren't rendered within the same area on screen.  pHint
   would be set to BCPH_AMBIGUOUS.  EINVALIDRANGE indicates that the
   characters in question span one or more direction changes.


   BIDI_VISUAL_ORDER

   When the caller specifies BIDI_VISUAL_ORDER, IFONTBIDIUTIL_Locate()
   treats dwStart and dwEnd as indices into the text in visual order.  That
   is, it interprets dwStart as the first on-screem character of the text
   run and dwEnd as the last on-screen character of the text run.  Consider
   this simplified example of drawing a single line of text in two different 
   colors:

===pre>
   Logical Order                     Display Order
   (in memory)                       (on screen)

   a b c   A B C                     a b c   C B A
   0 1 2 3 4 5 6                     0 1 2 3 4 5 6
===/pre>

   Assume we want to draw the left to right characters in blue, and the
   right to left characters in red.

   First we'd locate where the "abc " will end up on screen like this:

===pre>
   dwResult = IFONTBIDIUTIL_Locate(
               po, 
               pd, 
               pwText, 
               dwLen, 
               0, 
               3, 
               BIDI_VISUAL_ORDER,
               prc, 
               pHint);
===/pre>

   dwLen = 7 is the length of the text, dwStart 0 is the visual index
   of the "a" and dwEnd is the visual index of the space after the "c". 
   On return, prc contains the bounding box of the right to left
   characters.  We could then call IFONT_DrawText() with the entire text
   string, specify a blue text color and pass the bounding box as the clip 
   rect.  IFONT_DrawText() will lay out the text into visual order, then
   draw the "abc " in blue, and skip the "CBA" as those characters fall
   outside of the clip rectangle.

   A second call to IFONT_DrawText() could be invoked with the entire
   text again, a red text color, and a new clip rect based on the bounding
   box returned by the IFONTBIDIUTIL_Locate() call above.  It would lay
   out the text properly, clip the "abc " and draw only the "CBA".


   A generic imlementation of this routine is provided for reference.
   It is expected that the OEM implement this routine for their font,
   applying the same display algorithm as they do in their font.

Prototype:
   int IFONTBIDIUTIL_Locate(
      IFontBidiUtil *po, 
      IFont pif,
      const AECHAR *pwText, 
      int dwLen, 
      int dwStart, 
      int dwEnd,
      int dwOptions,
      AEERect *prcBounds, 
      int *pdwCursorHint);

Parameters:
===pre>
   po             [in]  pointer to the IFontBidiUtil interface object.  
   pif            [in]  the font being used to render the text
   pwText         [in]  pointer to Unicode text to display
   dwLen          [in]  total length of the paragraph of text.  If -1 is
                        specified, this routine calculates the length itself.
   dwStart        [in]  start of in memory charcter run
   dwEnd          [in]  end of in memory character run
   dwOptions      [in]  either BIDI_LOGICAL_ORDER, which tells this routine to
                        treat dwStart and dwEnd as logical indices, or
                        BIDI_VISUAL_ORDER, which tells this routine to teat
                        dwStart and dwEnd as visual indices.
   prcBounds      [in]  bounding rect of specified character range
   pdwCursorHint  [out] where the font expects the cursor to be placed
===/pre>

Return Value:
   SUCCESS if the screen position could be determined
   EINVALIDRANGE if the characters in question aren't display contiguously
   other error codes on failure

Comments: 
   An individual character can be located by setting dwStart and dwEnd to
   the same value.

Side Effects: 
   None

See Also:
   None

===========================================================================

IFONTBIDIUTIL_Breakline()

Description:
   Given a paragraph of text, the IFont object used to render the text and
   the visual clipping coordinate, return the visual pixel coordinate of
   an appropriate place to break the line. 

   Applications may use this routine in combination with IDISPLAY_DrawText()
   to break a paragraph of bidirectional text into lines and display them
   on screen.  The text is passed into this routine in logical order,
   laid out with respect to the various Unicode layout algorithms and then 
   broken into lines where the application requests.  This routine gives
   the application the ability to break the line according to the layout
   rules the IFont object understands.
   
Prototype:
   int IFONTBIDIUTIL_Breakline(
      IFontBidiUtil *po, 
      IFont *pif, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwLineStart, 
      int dwClip, 
      int *pdwLineEnd);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pif         [in]  pointer to the IFont object used to render the text
   pwText      [in]  pointer to the entire paragraph of text
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwLineStart [in]  x coordinate offset into the text where the line starts
   dwClip      [in]  number of pixel that define the width of the line
   pcxLineEnd  [out] on return, the x coordinate where the line breaks.
   pdwFits     [out] on return, a last character index of the line.
===/pre>

Return Value:
   SUCCESS - pdwLineEnd contains the best possible place to break this
             paragraph of text
   EFAILED - some error occured determining the line break

Comments:
   Breaking the text into lines requires that it be laid out properly, 
   so applications must  pass the entire paragraph of text to be broken 
   each time this routine is called.
  
Side Effects: 
   None

See Also:
   None

===========================================================================

IFONTBIDIUTIL_CharAt()

Description:
   This routine can be used to determine which logical character occurs
   at a specified pixel offset into the line.  Applications can use this
   information to move cursors and similar tasks.

   This routine takes the entire text of the paragraph being rendered and
   lays it out on a single line.  It then asks the underlying text layout
   engine to return the character index of the glyph that appears at the
   specified x location on that line.  In other words, this routine 
   translates from visual position to logical character index.

Prototype:
   int IFONTBIDIUTIL_CharAt(
      IFontBidiUtil *po, 
      IFont *pif, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwX, 
      int *pdwCharIdx);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pif         [in]  the font being used to render the text
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwX         [in]  horizontal pixel position of the logical characer
   pdwCharIdx  [out] the logical character at position dwX
===/pre>

Return Value:
   SUCCESS if logical character index could be determined
   error code otherwise

Comments: 
   This routine has to lay out the entire parapgrah to determine what
   character occurs at the given position.  Be sure to pass the entire
   paragraph text into this routine each time it's called!

Side Effects: 
   None

See Also:
   IFONTBIDIUTIL_Breakline()

===========================================================================

IFONTBIDIUTIL_NextDisplayChar()

Description:
   This function is no longer supported.

   Given an index into text in logical order, return an index into the 
   text in logical order of the character that is displayed <offset> 
   characters to the right or left.  That is, tell the caller which in 
   memory character is displayed to the right or left of any other 
   in memory character.

   When dealing with Text Input, for example, it is often necessary to 
   traverse the text in the order in which it's displayed on screen.  
   Consider the problem of  moving an insertion point through bidirectional 
   text.  The logical order differs from the display order, so simply 
   incrementing the insertion point through the in memory text does not 
   suffice. The caret would end up jumping around through the text on
   screen rather than moving smoothly to the left or right.  This routine 
   solves that problem by telling the caller which character in memory
   is displayed to the left or right of any other in memory character.  
   This allows the caller to move the caret through the displayed text one 
   or more characters at a time.

   IFONTBIDIUTIL_NextDisplayChar() takes text in logical order and an 
   index into that text representing the current insertion point.  It is
   also passed the offset indicating which displayed character the caller
   is interested in locating in memory, typically +/-1.  It returns an index 
   into the  text in logical order of that displayed charactter.

   Consider this example:

===pre>
   Logical Order                     Display Order
   (in memory)                       (on screen)

   a b c   A B|C                     a b c   C|B A
   0 1 2 3 4 5 6                     
===/pre>

   Assume a text input control has rendered the above characters and 
   needs to move the insertion point one character to the left.  Here,
   the caret location is stored as dwCaret = 5.  That is, the caret is
   currently located after pwText + 5. If the text control receives a 
   request to move the caret one character to the left on the display, it 
   would call this routine:

===pre>
   IFONTBIDIUTIL_NextDisplayChar(po, pwText, dwLen, 5, -1, &dwNextChar);
===/pre>

   This routine will determine which character is one character to the
   left of the charcter at index 5 ("B") and return the in memory index.  
   In this case, "C" is immediately to the left of the "B", and this routine 
   would return 6, since "C" is located at pwText + 6 in memory.

===pre>
   a b c   A B C|                    a b c  |C B A
   0 1 2 3 4 5 6                     
===/pre>

   A generic implementation of this routine is provided for reference, but
   it does not handle bidirectional text.  It is expected that the OEM
   will apply their knowledge of the internals of how their font renders
   text on screen to their own implementation of this routine.

Prototype:
   int IFONTBIDIUTIL_NextDisplayChar(
      IFontBidiUtil *po,
      const AECHAR *pwText, 
      int dwLen, 
      int dwIndex, 
      int dwOffset
      int *pdwNextChar);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwIndex     [in]  in memory index of current character
   dwOffset    [in]  display offset of next character  
   pdwNextChar [out] On successful return, an index into the in memory copy 
                     of the text pointing to the  character displayed <offset> 
                     characters to the right or left of the original
===/pre>

Return Value:
   SUCCESS if next character was successfully located
   failure code otherwise

Comments: 
   dwOffset may be set to either 1 or -1 to scan through displayed text
   one character at a time, although any other positive or negative may
   be used as well.

Side Effects: 
   None

See Also:
   IFONT_GetInfo()
===========================================================================

IFONTBIDIUTIL_CharLocation()

Description:
   This routine can be used to determine the given character's bounding
   rectangle where it is displayed.
   
   This routine takes the entire text of the paragraph being rendered and
   lays it out on a single line.  It then asks the underlying text layout
   engine to return the location of the character's glyph.
   
Prototype:
   int IFONTBIDIUTIL_CharLocation(
      IFontBidiUtil *po, 
      IFont *pif, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwIndex, 
      int *prcBounds);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pif         [in]  the font being used to render the text
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwIndex     [in]  in memory index of character
   prcBounds   [out] bounding rect of the character's glyph
===/pre>

Return Value:
   SUCCESS if the bounding rect of the index could be determined
   error code otherwise

Comments: 
   This routine has to lay out the entire paragraph to determine what
   character occurs at the given position.  Be sure to pass the entire
   paragraph text into this routine each time it's called!

Side Effects: 
   None


===========================================================================

IFONTBIDIUTIL_CaretInfo

Description:
   This routine can be used to determine the caret position of the given
   character index.

   This routine takes the entire text of the paragraph being rendered and
   lays it out on a single line.  It then asks the underlying text layout
   engine to return the caret position of the given character index.
   
Prototype:
   int IFONTBIDIUTIL_CaretInfo(
      IFontBidiUtil *po, 
      IFont *pif, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwIndex, 
      int *pdwX);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pif         [in]  the font being used to render the text
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwIndex     [in]  in memory index of character
   pdwX        [out] horizontal pixel position of the character's caret position.
   
===/pre>

Return Value:
   SUCCESS if logical character index could be determined
   error code otherwise

Comments: 
   This routine has to lay out the entire paragraph to determine the 
   character's caret position.  Be sure to pass the entire
   paragraph text into this routine each time it's called!

Side Effects: 
   None

See Also:

===========================================================================

IFONTBIDIUTIL_MoveCaretIndex

Description:
   This routine can be used to determine the character index after visually
   moving left or right. Applications can use this information to move cursors.

   This routine takes the entire text of the paragraph being rendered and
   lays it out on a single line.  It then asks the underlying text layout
   engine to return the character index after the caret moves.
   
Prototype:
   int IFONTBIDIUTIL_MoveCaretIndex(
      IFontBidiUtil *po, 
      IFont *pif, 
      const AECHAR *pwText, 
      int dwLen, 
      int dwIndex,
      int dwOffset,
      int *pdwNextChar);

Parameters:
===pre>
   po          [in]  pointer to the IFontBidiUtil interface object.  
   pif         [in]  the font being used to render the text
   pwText      [in]  pointer to Unicode text to display
   dwLen       [in]  total length of the paragraph of text.  If -1 is
                     specified, this routine calculates the length itself.
   dwIndex     [in]  character index of current cursor position.
   dwOffset    [in]  direction and length of move.  Negative numbers move the
                     cursor visually left.  Positive numbers move the cursor
                     visually right.
   pdwNextChar [out] the logical character index after the move
===/pre>

Return Value:
   SUCCESS if logical character index could be moved

Comments: 
   This routine has to lay out the entire paragraph to determine what
   characters location.  Be sure to pass the entire
   paragraph text into this routine each time it's called!

Side Effects: 
   None

See Also:


===========================================================================*/

#endif /* __AEEFONTBIDUTIL_H__ */
