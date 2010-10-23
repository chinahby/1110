/*
  ========================================================================

  FILE:  LayoutText.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Line table object for widgets that wrap text.
  methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEStdLib.h"
#include "AEEFont.h"
#include "AEEHFont.h"
#include "wutil.h"
#include "LayoutText.h"


// LayoutText flags
#define LTF_MULTILINE     (0x0001)
#define LTF_DRAWELLIPSIS  (0x0002) 
#define LTF_BREAKONWORD   (0x0004) 

typedef AEERect LayoutRect;

// LayoutTextLine flags
#define LTLF_LAST_LINE            (0x0002)
#define LTLF_RIGHT_TO_LEFT        (0x0004)
#define LTLF_USE_PIXEL            (0x0008)


// Line instance
typedef struct {
   int         nParagraphStart;
   int         nParagraphLength;
   int         dxDrawStart;
   int         dx;
   int         nLineStart;
   int         nLineLength;
   uint32      nFlags;
} LayoutTextLine;

typedef struct {
   LayoutTextLine*        paLine;        // array of line info 
   int                    nCount;        // number of items e
   int                    cbAlloc;       // number of bytes allocated (in paLine)
} LayoutLines;



typedef struct {
   AECHAR*     pText;         // text array 
   int         nText;         // number of characters in pText
   int         cbAlloc;       // number of bytes allocated (in pText)
} LayoutCharacters;

struct _LayoutText {

   uint32              nFlags;
   uint32              nTextAlignment;
   LayoutCharacters    text;
   LayoutLines         lines;
   LayoutTextStyleInfo style;
   IFontBidiUtil*      piFontBidiUtil;
   AEECLSID            ciFontBidiUtil;
   IShell*             piShell;
   AECHAR              szEllipsis[2];


   boolean             bLayout;
   int                 nWidth;
   int                 nLineGap;

};


// other helpful macros
static boolean LayoutText_IsParagraphBreak (LayoutText* me, AECHAR c) {
   return ((c == 0x000A) || (c == 0x2029) || c == 0x0 );
}

static AECHAR* LayoutText_NextCharacter    (LayoutText* me, AECHAR* p) {
   return ++p;
}

static void LayoutText_DoLayout (LayoutText* me);



static int LayoutText_GetLineHeight(LayoutText* me, int* nLineHeight) {
   int nErr = SUCCESS;
   
   if ( me->style.piFont ) {
      AEEFontInfo fi;
      nErr = IFONT_GetInfo(me->style.piFont, &fi, sizeof(AEEFontInfo));
      if ( SUCCESS == nErr ) {
         *nLineHeight = fi.nAscent + fi.nDescent;
      }
   }
   else {
      *nLineHeight = 0;
   }
   return nErr;
}

int   LayoutText_SetLineBreak (LayoutText* me, boolean bMultiLine, boolean bBreakOnWord) {

   if ( bMultiLine ) {
      me->nFlags |= LTF_MULTILINE;
      if ( bBreakOnWord ) {
         me->nFlags |= LTF_BREAKONWORD;
      }
      else {
         me->nFlags &= ~LTF_BREAKONWORD;
      }
   } 
   else {
      me->nFlags &= ~(LTF_MULTILINE | LTF_BREAKONWORD);
   }

   me->bLayout = TRUE;

   return SUCCESS;

}

int   LayoutText_SetEllipsis (LayoutText* me, boolean bDrawEllipsis)
{
   if ( bDrawEllipsis ) {
      me->nFlags |= LTF_DRAWELLIPSIS;
   } 
   else {
      me->nFlags &= ~LTF_DRAWELLIPSIS;
   
   }
   return SUCCESS;
}

int  LayoutText_SetLineGap (LayoutText* me, int nLineGap )
{
   if ( nLineGap != me->nLineGap ) {
      me->nLineGap = nLineGap;
   }

   return SUCCESS;
}

int   LayoutText_SetTextAlignment (LayoutText* me, uint32 dwAlignment) {

   if ( dwAlignment != me->nTextAlignment ) {
      me->nTextAlignment = dwAlignment;
      me->bLayout        = TRUE;
   }

   return SUCCESS;
}

int   LayoutText_GetScrollPosition (LayoutText* me, int offset, int* x) {
   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }
   if ( me->nFlags & LTF_MULTILINE       ||
        me->nFlags & LTF_DRAWELLIPSIS    ||
        me->lines.nCount != 1            ||
        me->lines.paLine->dx <= me->nWidth ) {
      *x = 0;
   } 
   else {
      if ( me->lines.paLine->nFlags & LTLF_RIGHT_TO_LEFT ) {
         *x = - (me->lines.paLine->dx - me->nWidth) + offset; 
      }
      else {
         *x = - offset;
      }
   }
   return SUCCESS;
}


int   LayoutText_GetLineFromIndex  (LayoutText* me, int index, int* pLine) {
   int nErr = SUCCESS;

   // find the line that this index is on.
   int nLineIndex = 0;
   int nIndexLine = -1;
   int nIndexPos  = -1;
   LayoutTextLine* pLastLine = 0;

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }

   while ( nLineIndex < me->lines.nCount ) {

      LayoutTextLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nParagraphStart <= index && pLine->nParagraphLength + pLine->nParagraphStart - 1 >= index ) {
         nIndexLine = nLineIndex;
         break;
      }

      if ( pLastLine && index < pLine->nParagraphStart ) {
         nIndexLine = nLineIndex - 1;
         nIndexPos  = pLastLine->dx;
         break;
      }

      pLastLine = pLine;
      ++nLineIndex;
   }

   if ( pLastLine &&
        nIndexLine == -1 && index > pLastLine->nParagraphStart + pLastLine->nParagraphLength - 1 ) {
      nIndexLine = nLineIndex - 1;
      nIndexPos  = pLastLine->dx;
   }

   if ( nIndexLine != -1 && nIndexPos == -1 ) {
      // the index is located within this paragraph...find the line!
      LayoutTextLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nParagraphStart == index ) {
         // easy...we're at the beginning of this paragraph
         nIndexPos  = pLine->nFlags & LTLF_RIGHT_TO_LEFT ? me->nWidth : 0;
      }
      else {
         // we're somewhere in the middle of the paragraph
         if ( pLine->nFlags & LTLF_USE_PIXEL ) {
            // lines are broken using pixels...
            int     dwCursorX;
            nErr = IFONTBIDIUTIL_CaretInfo(
                           me->piFontBidiUtil, 
                           me->style.piFont, 
                           me->text.pText + pLine->nParagraphStart,
                           pLine->nParagraphLength,
                           index - pLine->nParagraphStart,
                           &dwCursorX);
            // now, find the line the cursor (and index) is on
            while ( 1 ) {
               if ( dwCursorX < pLine->dxDrawStart + pLine->dx) {
                  break;
               }
               ++pLine;
               ++nLineIndex;
            }
            nIndexPos = dwCursorX;
         } 
         else {
            // lines are broken using characters..find the line with this index.
            while ( nLineIndex < me->lines.nCount ) {
               LayoutTextLine* pLine = me->lines.paLine + nLineIndex;
               if ( pLine->nLineStart <= index && pLine->nLineStart + pLine->nLineLength > index ) {
                  break;
               }
               ++nLineIndex;
            }
         }
         nIndexLine = nLineIndex;
      }
   }


   if ( nLineIndex != -1 )
   {
      *pLine = nIndexLine;
   } 
   else {
      *pLine = 0;
   }

   return SUCCESS;
}


int   LayoutText_GetLineStartIndex (LayoutText* me, int line,  int* pIndex) {

   LayoutTextLine* pLine;

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }

   if ( me->lines.nCount == 0 ) {
      return EFAILED;
   }

   if ( line < 0 || line >= me->lines.nCount ) {
      return EFAILED;
   }

   pLine = me->lines.paLine + line;
   if ( pLine->nFlags & LTLF_USE_PIXEL ) {
      int index = 0;
      if ( pLine->dxDrawStart > 0 ) {

         int dx = pLine->dxDrawStart;
         if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT ) {
            dx += pLine->dx - 1;
         }

         IFONTBIDIUTIL_CharAt(
                           me->piFontBidiUtil,
                           me->style.piFont,
                           me->text.pText + pLine->nParagraphStart, 
                           pLine->nParagraphLength, 
                           dx, 
                           &index);
      }
      *pIndex = pLine->nParagraphStart + index;
   }
   else {
      *pIndex = pLine->nLineStart;
   }
  
   return SUCCESS;
}


int   LayoutText_GetCaretInfo (LayoutText* me, int index, int* pxCaret, int* pyCaret) {
   int nErr = SUCCESS;

   // find the line that this index is on.
   int nLineIndex = 0;
   int nIndexLine = -1;
   int nIndexPos  = -1;
   int y          = 0;
   int nLineHeight = 0;

   LayoutTextLine* pLastLine = 0;
   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }

   LayoutText_GetLineHeight(me, &nLineHeight);

   while ( nLineIndex < me->lines.nCount ) {

      LayoutTextLine* pLine = me->lines.paLine + nLineIndex;
      if ( pLine->nParagraphStart <= index && pLine->nParagraphLength + pLine->nParagraphStart - 1 >= index ) {
         //  We're in the paragraph to this index
         nIndexLine = nLineIndex;
         break;
      }

      if ( pLastLine && index < pLine->nParagraphStart ) {
         //  Between Lines nLineIndex-1 and nLineIndex
         nIndexLine = nLineIndex - 1;
         if ( pLastLine->nFlags & LTLF_RIGHT_TO_LEFT && pLastLine->dx < me->nWidth) {
            nIndexPos = me->nWidth - pLastLine->dx;
         }
         else {
            nIndexPos = pLastLine->dx;
         }

         break;
      }

      if ( pLastLine ) {
         y += nLineHeight + me->nLineGap;
      }

      pLastLine = pLine;
      ++nLineIndex;
   }

   if ( pLastLine &&
        nIndexLine == -1 && index > pLastLine->nParagraphStart + pLastLine->nParagraphLength - 1  ) {
      // At end of widget text
      nIndexLine = nLineIndex - 1;
      if ( pLastLine->nFlags & LTLF_RIGHT_TO_LEFT ) {
         nIndexPos = me->nWidth - pLastLine->dx;
      }
      else {
         nIndexPos = pLastLine->dx;
      }
   }
   else {

      if ( nIndexLine != -1 && nIndexPos == -1 ) {
         // the index is located within this paragraph...find the dx
         int     dwCursorX;
         LayoutTextLine* pLine = me->lines.paLine + nIndexLine;
         if ( pLastLine ) {
            y += nLineHeight + me->nLineGap;
         }

         if ( pLine->nFlags & LTLF_USE_PIXEL ) {
            // lines are broken using pixels...
            nErr = IFONTBIDIUTIL_CaretInfo(
                           me->piFontBidiUtil, 
                           me->style.piFont, 
                           me->text.pText + pLine->nParagraphStart,
                           pLine->nParagraphLength,
                           index - pLine->nParagraphStart,
                           &dwCursorX);
            // now, find the line the cursor is
            if ( SUCCESS == nErr ) {
               while ( 1 ) {
                  if ( dwCursorX < pLine->dxDrawStart + pLine->dx) {
                     dwCursorX -= pLine->dxDrawStart;
                     if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT && me->nFlags & LTF_MULTILINE ) {
                        dwCursorX += me->nWidth - pLine->dx;
                     }
                     break;
                  }

                  y += nLineHeight + me->nLineGap;
                  ++pLine;
               }
            }
            else { 
               return nErr; 
            }
            nIndexPos = dwCursorX;
         } 
         else {
            // lines are broken using characters..find the line with this character.
            while ( nIndexLine < me->lines.nCount ) {
               if ( pLine->nLineStart <= index && pLine->nLineStart + pLine->nLineLength - 1 >= index ) {
                  nErr = IFONTBIDIUTIL_CaretInfo(
                                 me->piFontBidiUtil, 
                                 me->style.piFont, 
                                 me->text.pText + pLine->nLineStart,
                                 pLine->nLineLength,
                                 index - pLine->nLineStart,
                                 &nIndexPos);
                  if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT && me->nFlags & LTF_MULTILINE ) {
                     nIndexPos += me->nWidth - pLine->dx;
                  }

                  break;
               }
               if ( ++nIndexLine >= me->lines.nCount ) {
                  nIndexPos = pLine->dx;
                  break;
               }
               y += nLineHeight + me->nLineGap;
               ++pLine;
            }
         }
      }
   }


   if ( nIndexPos != -1 )
   {
      *pxCaret  = nIndexPos;
      *pyCaret  = y;
   } 
   else {
      // no text....position cursor at 0,0
      *pxCaret  = 0;
      *pyCaret  = 0;
   }

   return SUCCESS;
}

int LayoutText_GetCaretScroll(LayoutText* me, int nCaret, int nScroll, int * pnScroll) {


   int x, y;
   int nMargin = 10;
   LayoutTextLine* pLine;

   *pnScroll = 0;

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }

   if ( me->nFlags & LTF_MULTILINE || me->lines.nCount != 1 ) {
      return TRUE;
   }


   pLine = me->lines.paLine;
   
   if ( pLine->dx < me->nWidth ) {
      return TRUE;
   }

   if ( LayoutText_GetCaretInfo(me, nCaret, &x, &y) == SUCCESS ) {

      if ( x > nScroll + me->nWidth - nMargin ) {
         *pnScroll = MIN( x - me->nWidth + nMargin, 
                          pLine->dx - me->nWidth );
      } 
      else if ( x < nScroll + nMargin ) {
         *pnScroll = MAX(0, nScroll - nMargin);
      }
      else {
         *pnScroll = nScroll;
      }
   }

   return TRUE;
}



/*
|| Measure text and break at nearest line break
||
*/
static int IFont_MeasureTextToLineBreak(IFont *piFont, const AECHAR *pwText, int cchText, 
                                        int cxWidth, int *pnFits, boolean bBreakOnWord)
{
   int cx = 0;
   int fits;
   const AECHAR *pw = pwText;

   IFONT_MeasureText(piFont, pwText, cchText, cxWidth, &fits, &cx);

   // scan for any hard-breaks
   {
      int nn = 0;
      for (; nn < fits; nn++) {
         if (*pw++ == '\n') {
            fits = nn;
            break;
         }
      }
   }

   if (fits < cchText) {

      pw = pwText + fits;

      // if we broke at white space, consider it (the whitespace)
      // to fit on the line also
      if (W_ISWHITESPACE(*pw)) {
         fits++;
      
      } else if (bBreakOnWord) {
      // if we broke at non-whitespace, scan back
      // until we find whitespace
         while (pw > pwText && !W_ISWHITESPACE(*(pw-1))) {
            pw--;
         }
         if (pw > pwText) {
            fits = pw-pwText;
         }
      }
   }

   *pnFits = (fits == 0) ? 1 : fits;
   return cx;
}

#define PROP_LINEWIDTH                 (PROP_SPECIAL + 11)  // line width for DrawText 

static int   LayoutText_MakeLines(LayoutText* me) {

   AECHAR* p;
   int     nErr;
   int  dxLineStart, dxLineEnd;
   LayoutTextLine* pLine = 0;
   IHFont * piHFont = 0;
   boolean bUsePixel = FALSE;
   int     dwFits, dwLineStart;
   int     nLen;


   // reset line count to zero
   me->lines.nCount = 0;

   if ( 0 == me->style.piFont ) {
      return SUCCESS;
   }
   // does this font support the LINEWIDTH property?
   // if so, then break the lines using pixels,
   // if not, then break the lines using characters.
   nErr = IFONT_TO_IHFONT(me->style.piFont, &piHFont);
   if ( nErr == SUCCESS )
   {
      uint32 dw;
      if ( IHFONT_GetProperty(piHFont, PROP_LINEWIDTH, &dw) == SUCCESS ) {
         bUsePixel = TRUE;
      }
      RELEASEIF(piHFont);
   }
   else {
      // not really an error, so reset the error condition
      nErr = SUCCESS;
   }


   // break text into a paragraph.
   p = me->text.pText;
   if ( !p ) {
      return SUCCESS;
   }

   nLen = me->text.nText;

   while ( *p ) {

      int start, end;

      int nLineFlags = 0;

      start = p - me->text.pText;
      end   = start;

      if ( LTF_MULTILINE & me->nFlags ) {
         // multiline, find paragraph break
         while ( 1 ) {
            p = LayoutText_NextCharacter(me, p);
            if ( !p || !*p ) {
               break;
            }
            if ( LayoutText_IsParagraphBreak(me, *p ) ) {
               p = LayoutText_NextCharacter(me, p);
               break;
            }
            else {
               ++end;
            }
         }
      }
      else {
         // single line, one paragraph
         p += end = WSTRLEN(p);
         --end;
      }

      nLineFlags = 0;// defaults to LEFT to RIGHT
      if ( me->nTextAlignment == IDF_ALIGN_NONE ) {

         // no alignment specified - use unicode characters.
         // find if the paragraph string is LtoR or RtoL
         if ( IFONTBIDIUTIL_Characterize(
                     me->piFontBidiUtil, 
                     me->text.
                     pText + start, 
                     end - start + 1, 
                     0, 
                     NULL)
               == BIDICHARTYPECAT_STRONGRIGHT) {
            nLineFlags = LTLF_RIGHT_TO_LEFT;
         }
      }
      else if ( me->nTextAlignment == IDF_ALIGN_RIGHT ) {
         nLineFlags = LTLF_RIGHT_TO_LEFT;
      }


      
      dxLineStart = 0;
      dwLineStart = start;

      ///////////////////////////////////////
      // now break the paragraph into lines
      //////////////////////////////////////
      while ( SUCCESS == nErr && dwLineStart <= end ) {


         // figure out where to break this line of text
         if ( me->nFlags & LTF_MULTILINE ) {

            if ( bUsePixel ) {
               nErr = IFONTBIDIUTIL_Breakline(
                     me->piFontBidiUtil, 
                     me->style.piFont, 
                     me->text.pText + dwLineStart, 
                     end - dwLineStart + 1, 
                     dxLineStart, 
                     me->nWidth,  
                     &dxLineEnd,
                     &dwFits);
               dwFits = end - dwLineStart + 1;
            }
            else {

               IFont_MeasureTextToLineBreak(
                        me->style.piFont, 
                        me->text.pText + dwLineStart, 
                        end - dwLineStart + 1, 
                        me->nWidth,
                        &dwFits, 
                        (boolean) (me->nFlags & LTF_BREAKONWORD ? 1 : 0));

               // measure the broken line.
               nErr = IFONT_MeasureText(me->style.piFont, 
                        me->text.pText + dwLineStart, 
                        dwFits, 
                        IFONT_MAXWIDTH, 
                        &dwFits,  
                        &dxLineEnd);

            }
         }
         else
         {
            // single line.
            if ( dxLineStart ) {
               // done.
               dxLineEnd = 0;
            }
            else {
               // measure the single line.
               nErr = IFONT_MeasureText(me->style.piFont, 
                  me->text.pText + start, 
                  end - start + 1, 
                  IFONT_MAXWIDTH, 
                  &dwFits,  
                  &dxLineEnd);
            }
         }

         if ( dxLineEnd ) {


            // allocate room for this line
            nErr = MemGrowEx((void**)&me->lines.paLine, &me->lines.cbAlloc, 
               sizeof(LayoutTextLine) * me->lines.nCount, 
               sizeof(LayoutTextLine) * (me->lines.nCount + 1),
               sizeof(LayoutTextLine) * (me->lines.nCount ? 5 : 0 ));

            if ( nErr ) {
               break;
            }

            pLine = me->lines.paLine + me->lines.nCount;
            ++me->lines.nCount;


            pLine->nParagraphStart  = start;
            pLine->nParagraphLength = end - start + 1;
            pLine->nFlags           = nLineFlags;


            pLine->dxDrawStart      = dxLineStart;
            pLine->dx               = dxLineEnd - dxLineStart;
            pLine->nLineLength      = dwFits;
            pLine->nLineStart       = dwLineStart;
            if ( bUsePixel ) {
               pLine->nFlags       |= LTLF_USE_PIXEL;
               dxLineStart          = dxLineEnd;
            }
            else {
               dwLineStart         += dwFits;
            }
            
         }
         else { // dwLineEnd == 0, so we're at the end of the paragraph!

            if ( pLine ) {
               pLine->nFlags |= nLineFlags | LTLF_LAST_LINE;
            }
            break;
         }
      }
   }

   return nErr;
}

static void LayoutText_DoLayout (LayoutText* me) {
   LayoutText_MakeLines(me);
   me->bLayout = FALSE;
}


int   LayoutText_SetWidth    (LayoutText* me, int width) {
   me->nWidth = width;
   me->bLayout = TRUE;
   return SUCCESS;
}

int   LayoutText_GetHeight   (LayoutText* me, int* pHeight) {

   int nErr = SUCCESS;
   int nLineHeight = 10;

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }
   
   if ( nErr == SUCCESS ) {
      nErr = LayoutText_GetLineHeight(me, &nLineHeight);
   }

   if ( nErr == SUCCESS ) {
      *pHeight = MAX(nLineHeight, ((me->nLineGap + nLineHeight) * me->lines.nCount ) - me->nLineGap);
   }
   
   return nErr;
}

int   LayoutText_GetLineCount (LayoutText* me, int* pCount) {

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }
   
   *pCount =  me->lines.nCount;

   return SUCCESS;
}


int   LayoutText_DeleteText  (LayoutText* me, int start, int end) {
   me->text.nText = 0;
   RELEASEIF(me->style.piFont);
   me->bLayout = TRUE;
   return SUCCESS;
}

int   LayoutText_AddText     (LayoutText* me, LayoutTextStyleInfo* pStyle, const AECHAR *pwText, int len, int insert ) {

   int nErr = SUCCESS;

   int cbAlloc = me->text.cbAlloc;
   int cbUsed  = (me->text.nText + 1) * sizeof(AECHAR);
   int cbNeeded;
   AECHAR* pEnd;
   
   if ( pwText ) {
   
      // Used text size is cch+1 (include zero terminator)
      cbNeeded = cbUsed + len  * sizeof(AECHAR);
      if ( cbNeeded < 0 ) {
         cbNeeded = 0;
      }

      nErr = MemGrowEx((void**)&me->text.pText, &cbAlloc, cbUsed, cbNeeded, 48);

      MEMCPY(me->text.pText, pwText, (len+1) * sizeof(AECHAR) );

      me->text.cbAlloc  = cbAlloc;
      me->text.nText   += len;
      pEnd = me->text.pText + len;
      *pEnd = 0;
   
      me->bLayout = TRUE;

   }

   if ( pStyle ) {

      RELEASEIF(me->style.piFont);
      MEMCPY(&me->style,pStyle,sizeof(me->style));
      IFONT_AddRef(me->style.piFont);

      // create the assoicated FontBidiUtil
      if ( me->style.piFont ) {
         int nErr;
         IHFont *piHFont;
         AEECLSID  ciFontBidiUtil;
         nErr = IFONT_TO_IHFONT(me->style.piFont, &piHFont);
         if ( nErr == SUCCESS ) {
            nErr = IHFONT_GetFontBidiUtilClassId(piHFont, &ciFontBidiUtil);
            if ( nErr == SUCCESS ) {
               if ( me->piFontBidiUtil == NULL || ciFontBidiUtil != me->ciFontBidiUtil ) {
                  RELEASEIF(me->piFontBidiUtil);
                  nErr = ISHELL_CreateInstance(me->piShell, ciFontBidiUtil, (void **)&me->piFontBidiUtil);
                  if ( nErr == SUCCESS ) {
                     me->ciFontBidiUtil = ciFontBidiUtil;
                  }
               }
            }
            RELEASEIF(piHFont);
         }
      }
      if ( me->piFontBidiUtil == NULL ) {
         me->piFontBidiUtil = GetFontBidiUtil(me->piShell);
         me->ciFontBidiUtil = 0;
      }

   }
   
   return nErr;
   
}


int   LayoutText_Draw        (LayoutText* me, IDisplay* piDisplay, 
                              int x, int y, AEERect* prcClip, 
                              uint32 nFlags, AEERect* prcWidget, RGBVAL rgbText  ) {
   int nErr = SUCCESS;
   int nLineIndex = 0;
   IHFont * piHFont = 0;
   int yLineDraw;
   int xLineDraw;
   int nLineHeight;
   int nHeight;

   AEEFontInfo fi;
   boolean bQuitDrawing = FALSE;
   AEERect rcClip;
   RGBVAL  rgbOriginal;

   IFont* piFont = 0;

   if ( !me || !piDisplay || !prcWidget ) {
      return EBADPARM;
   }

   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }
   
   if ( me->style.piFont == 0 || me->text.nText == 0 ) {
      return SUCCESS;
   }
   
   piFont = IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, me->style.piFont);

   if ( rgbText == RGB_NONE ) {
      rgbText = me->style.rgbForeground;
   }
   
   rgbOriginal = IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, rgbText & 0xFFFFFF00 );
   
   nErr = IFONT_GetInfo(me->style.piFont, &fi, sizeof(AEEFontInfo));
   nLineHeight = fi.nAscent + fi.nDescent;


   if ( prcClip ) {
      rcClip = *prcClip;
   } 
   else {
      rcClip = *prcWidget;
   }

   if ( IFONT_TO_IHFONT(me->style.piFont, &piHFont) == SUCCESS )
   {
      IHFONT_SetProperty(piHFont, PROP_LINEWIDTH, (uint32) me->nFlags & LTF_MULTILINE ? me->nWidth : 0x7FFF );
      RELEASEIF(piHFont);
   }

   LayoutText_GetHeight(me, &nHeight);

   if ( ! (me->nFlags & LTF_DRAWELLIPSIS && prcWidget->dy < nHeight) ) {

      if ( nFlags & IDF_ALIGN_MIDDLE ) {
         // center text in clip rect
         y += ( prcWidget->dy - nHeight) / 2;
      }
      else if ( nFlags & IDF_ALIGN_BOTTOM ) {
         y += ( prcWidget->dy - nHeight );
      }

   }

   yLineDraw = y;

   while ( !bQuitDrawing && nErr == SUCCESS && nLineIndex < me->lines.nCount )
   {
      AEERect rcLine, rcLineClip;
      int     nLineLength;
      int     xDrawText;
      boolean bEllipsis    = FALSE;

      LayoutTextLine* pLine = me->lines.paLine + nLineIndex;

      xLineDraw   = x;

      rcLine.x    = prcWidget->x;
      rcLine.y    = yLineDraw;
      rcLine.dx   = pLine->dx;
      rcLine.dy   = nLineHeight;
      nLineLength = pLine->nLineLength;

      if ( ! (pLine->nFlags & LTLF_USE_PIXEL ) ) {
         // strip trailing whitespace
         AECHAR *pw = me->text.pText + pLine->nLineStart + nLineLength - 1;
         while (W_ISWHITESPACE(*pw) && nLineLength) {
            nLineLength--;
            pw--;
         }
         if ( nLineLength != pLine->nLineLength ) {
            int fits;
            int x;
            nErr = IFONT_MeasureText(me->style.piFont, 
               me->text.pText + pLine->nLineStart, 
               nLineLength,
               IFONT_MAXWIDTH, 
               &fits,  
               &x);
            rcLine.dx = x;
         }
      }


      if ( nFlags & IDF_ALIGN_LEFT ) {
         // already aligned left
      }
      else if ( rcLine.dx < me->nWidth ) {
         if ( nFlags & IDF_ALIGN_CENTER) {
            rcLine.x  += ( me->nWidth - rcLine.dx) / 2;
            xLineDraw += ( me->nWidth - rcLine.dx) / 2;
         }
         else if ( nFlags & IDF_ALIGN_RIGHT) {
            rcLine.x  += me->nWidth - rcLine.dx;
            xLineDraw += me->nWidth - rcLine.dx;
         }
         else if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT ) {
            rcLine.x  += me->nWidth - rcLine.dx;
            xLineDraw += me->nWidth - rcLine.dx;
         } 
      }
      
      if ( rcLine.dx   > me->nWidth           && 
           pLine->nFlags & LTLF_RIGHT_TO_LEFT   &&
           me->nFlags    & LTF_DRAWELLIPSIS ) {
         rcLine.x  += me->nWidth - rcLine.dx;
         xLineDraw += me->nWidth - rcLine.dx;
      }


      if ( me->nFlags & LTF_DRAWELLIPSIS ) {

         // check if this current line is the last line that fits.
         if ( nLineIndex + 1 < me->lines.nCount ) {
            if ( (rcLine.y - y) + rcLine.dy + nLineHeight > prcWidget->dy ) {
               // this is not the last line and it's the last whole line to draw!
               bEllipsis = TRUE;
               bQuitDrawing = TRUE;
            }
         }

         // for single-line layout, check if the line length
         if ( !bEllipsis && !(me->nFlags & LTF_MULTILINE) ) {
            if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT ) {

               // is the left end of the line showing?
               if ( rcLine.x < prcWidget->x ) {
                  bEllipsis = TRUE;
               }
            }
            else {
               // is the right end of the line showing?
               if ( rcLine.dx + rcLine.x > prcWidget->dx + prcWidget->x ) {
                  bEllipsis = TRUE;
               }
            }
         }

      }

      xDrawText   = xLineDraw - pLine->dxDrawStart;

      if ( bEllipsis ) {

         int dummy;
         AEERect rcEllipsis = rcLine;
         int     dx;
         nErr = IFONT_MeasureText(me->style.piFont, 
            me->szEllipsis, 
            WSTRLEN(me->szEllipsis),
            IFONT_MAXWIDTH, 
            &dummy,  
            &dx);

         rcEllipsis.dx = (short) dx;

         if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT ) {
            // draw ellipsis on left
            int whitespace = rcLine.x - prcWidget->x;
            if ( whitespace >= rcEllipsis.dx ) {
               rcEllipsis.x = rcLine.x - rcEllipsis.dx;
            }
            else {
               int trim = rcEllipsis.dx - whitespace;
               rcLine.x  += trim;
               rcLine.dx -= trim;
               rcEllipsis.x = prcWidget->x;
            }

         } 
         else {
            // draw ellipsis on the right
            int whitespace = ( prcWidget->x + prcWidget->dx ) - ( rcLine.x + rcLine.dx );
            if ( whitespace >= rcEllipsis.dx  && pLine->nFlags & LTLF_USE_PIXEL ) {
               // there's room to draw elipsis after line of text.
               rcEllipsis.x = rcLine.x + rcLine.dx;
            }
            else {

               int fits;

               // trim line to make room for ellipsis
               int trim = MAX(0,rcEllipsis.dx - whitespace);
            
               rcLine.dx    -= trim;
               rcEllipsis.x  = rcLine.x + rcLine.dx;


               if ( ! ( pLine->nFlags & LTLF_USE_PIXEL ) ) {

                  rcLine.dx = prcWidget->dx - rcEllipsis.dx;

                  //shorten text using whole characters
                  nErr = IFONT_MeasureText(me->style.piFont,
                     me->text.pText + pLine->nLineStart, 
                     pLine->nParagraphStart + pLine->nParagraphLength - pLine->nLineStart,
                     pLine->dxDrawStart + rcLine.dx,
                     &fits,
                     &dx);

                  rcLine.dx    = dx - pLine->dxDrawStart;
                  rcEllipsis.x = rcLine.x + rcLine.dx;

                  nLineLength = fits;
               }
            }
         }

         if ( IntersectRect(&rcEllipsis, &rcEllipsis, &rcClip) ) {
            IDISPLAY_DrawText(piDisplay, AEE_FONT_NORMAL, 
                     me->szEllipsis, 
                     WSTRLEN(me->szEllipsis),
                     rcEllipsis.x, rcEllipsis.y, &rcEllipsis, nFlags);
         }

      }

      if ( IntersectRect(&rcLineClip, &rcLine, &rcClip) ) {
         IDISPLAY_DrawText(piDisplay, AEE_FONT_NORMAL,
            me->text.pText + pLine->nLineStart, 
            nLineLength,
            xDrawText, yLineDraw, &rcLineClip, nFlags);
      }
      

      yLineDraw += nLineHeight + me->nLineGap;
      if ( bQuitDrawing || yLineDraw > rcClip.y + rcClip.dy ) {
         break;
      }

      ++nLineIndex;
   }
  

   IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, rgbOriginal);
   IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, piFont);
   
   return nErr;
}


int   LayoutText_GetCharacterLocation(LayoutText* me, int index, AEERect* prcChar) {

   int nErr = SUCCESS;

   // find the paragraph that this index is on.
   int nLineIndex = 0;
   int nIndexLine = -1;
   int nIndexPos  = -1;
   int y          = 0;
   int nLineHeight = 0;
   LayoutTextLine* pLastLine = 0;
   if ( me->bLayout ) {
      LayoutText_DoLayout(me);
   }

   LayoutText_GetLineHeight(me, &nLineHeight);
   
   while ( nLineIndex < me->lines.nCount ) {

      LayoutTextLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nParagraphStart <= index && pLine->nParagraphLength + pLine->nParagraphStart - 1 >= index ) {
         nIndexLine = nLineIndex;
         break;
      }

      if ( pLastLine && index < pLine->nParagraphStart ) {
         nIndexLine = nLineIndex - 1;
         if ( pLastLine->nFlags & LTLF_RIGHT_TO_LEFT && pLastLine->dx < me->nWidth) {
            nIndexPos = me->nWidth - pLastLine->dx;
         }

         break;
      }

      if ( pLastLine ) {
         y += nLineHeight + me->nLineGap;
      }

      pLastLine = pLine;
      ++nLineIndex;
   }

   if ( pLastLine &&
        nIndexLine == -1 && index > pLastLine->nParagraphStart + pLastLine->nParagraphLength - 1  ) {
      nIndexLine = nLineIndex - 1;
      pLastLine = 0;
   }

   if ( nIndexLine != -1 && nIndexPos == -1 ) {
      // the index is located within this paragraph...find the dx
      LayoutTextLine* pLine = me->lines.paLine + nIndexLine;
      if ( pLastLine ) {
         y += nLineHeight + me->nLineGap;
      }

      if ( pLine->nFlags & LTLF_USE_PIXEL ) {
         // lines are broken using pixels...
         AEERect rcGlyph;
         nErr = IFONTBIDIUTIL_CharLocation(
                        me->piFontBidiUtil, 
                        me->style.piFont, 
                        me->text.pText + pLine->nParagraphStart,
                        pLine->nParagraphLength,
                        index - pLine->nParagraphStart,
                        &rcGlyph);
         // now, find the line the cursor is
         while ( 1 ) {
            if ( rcGlyph.x < pLine->dxDrawStart + pLine->dx) {
               rcGlyph.x -= pLine->dxDrawStart;
               if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT && me->nFlags & LTF_MULTILINE ) {
                  rcGlyph.x += me->nWidth - pLine->dx;
               }
               rcGlyph.y = y;
               *prcChar = rcGlyph;
               return SUCCESS;
            }
            y += nLineHeight + me->nLineGap;
            ++pLine;
         }
      } 
      else {
         // lines are broken using characters..find the line with this character.
         while ( nIndexLine < me->lines.nCount ) {
            if ( pLine->nLineStart <= index && pLine->nLineStart + pLine->nLineLength > index ) {
               AEERect rcGlyph;
               nErr = IFONTBIDIUTIL_CharLocation(
                              me->piFontBidiUtil, 
                              me->style.piFont, 
                              me->text.pText + pLine->nLineStart,
                              pLine->nLineLength,
                              index - pLine->nLineStart,
                              &rcGlyph);
         
               nIndexPos = rcGlyph.x;
               if ( pLine->nFlags & LTLF_RIGHT_TO_LEFT && me->nFlags & LTF_MULTILINE ) {
                  rcGlyph.x += me->nWidth - pLine->dx;
               }
               rcGlyph.y = y;
               *prcChar = rcGlyph;
               return SUCCESS;
            }
            if ( ++nIndexLine >= me->lines.nCount ) {
               break;
            }
            y += nLineHeight + me->nLineGap;
            ++pLine;
         }
      }
   }

   return EFAILED;
}


static void LayoutText_Dtor(LayoutText* me)
{
   FREEIF(me->lines.paLine);
   FREEIF(me->text.pText);

   RELEASEIF(me->piShell);
   RELEASEIF(me->style.piFont);
   RELEASEIF(me->piFontBidiUtil);

}

int LayoutText_Delete(LayoutText *me)
{
   LayoutText_Dtor(me);
   FREEIF(me);
   return SUCCESS;
}

static void LayoutText_Ctor(LayoutText *me, IShell* piShell)
{
   ZEROAT(me);
   me->bLayout = TRUE;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piFontBidiUtil = GetFontBidiUtil(piShell);
   me->szEllipsis[0]  = IShell_GetEllipsis(piShell);
   me->szEllipsis[1]  = 0;

}

int LayoutText_New(LayoutText **ppo, IShell* piShell)
{
   LayoutText *me = MALLOCREC(LayoutText);
   if (!me) {
      return ENOMEMORY;
   }

   LayoutText_Ctor(me, piShell);

   *ppo = me;

   return SUCCESS;
}


