/*
  ========================================================================

  FILE:  LineTable.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Line table object for widgets that wrap text.
  methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEStdLib.h"
#include "wutil.h"
#include "LineTable.h"


#define TO_BYTES(l)  ((l) * sizeof(TLPos))
#define TO_LINES(b)  ((b) / sizeof(TLPos))
#define LINES(p)     TO_LINES((p)->cbUsed)


#define LINE_GRANULARITY      10


static __inline int TextLineTbl_Grow(TextLineTbl *me, int nMoreLines)
{
   return MemGrowEx((void**)&me->paPos, &me->cbAlloc, me->cbUsed, 
                    TO_BYTES(nMoreLines), TO_BYTES(LINE_GRANULARITY));
}

int TextLineTbl_GetLine(TextLineTbl *me, uint16 nLine, TLPos *pposOffset, int *pnLen)
{
   if (!me->nLines || !me->paPos || nLine >= me->nLines) {
      return EFAILED;
   }

   *pposOffset = me->paPos[nLine];
   *pnLen = (int) (me->paPos[nLine+1] - me->paPos[nLine]);

   return SUCCESS;
}


int TextLineTbl_GetLineFromPos(TextLineTbl *me, TLPos pos)
{
   int nLine;

   for (nLine=0; nLine < me->nLines ; nLine++) {
      if (pos >= me->paPos[nLine] && pos < me->paPos[nLine+1]) {
         return nLine;
      }
   }

   return MAX(0, nLine-1);   // return last line
}


/*
|| Measure text and break at nearest line break
||
*/
static int IFont_MeasureTextToLineBreak(IFont *piFont, const AECHAR *pwText, int cchText, 
                                        int cxWidth, int *pnFits, boolean bNoBreakOnSpace)
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
      
      } else if (!bNoBreakOnSpace) {
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


/*
|| TextLineTbl_WrapFill
|| 
|| Args:
|| 
|| TextLineTbl *     me                   TextLineTbl object pointer
|| IFont *           piFont               font to use for measure text
|| const AECHAR *    pwText               pointer to text to wrap
|| int               cchText              length of text
|| int               nUpdateFromLine      line number to start wrapping from (0 for all)
|| int               cxWrapWidth          wrap to this width in pixels
|| 
*/
int TextLineTbl_WrapFill(TextLineTbl *me, IFont *piFont, const AECHAR *pwText, 
                         int cchText, int nUpdateFromLine, int cxWrapWidth, 
                         boolean bNoBreakOnSpace)
{
   TLPos nPos = 0;
   int nLine;
   int nChars;
   int nErr = SUCCESS;
   boolean bLastNewline = 0;

   if (!me->paPos || nUpdateFromLine >= me->nLines) {
      nUpdateFromLine = 0;
   }

   // special case for filling with no text
   if (0 == nUpdateFromLine  && (!cchText || !pwText)) {
      me->nLines = 1;
      nErr = TextLineTbl_Grow(me, 2);
      if (!nErr) {
         me->paPos[0] = me->paPos[1] = 0;
      }
      return nErr;
   }

   bLastNewline = (pwText[cchText-1] == (AECHAR)'\n');

   nLine = nUpdateFromLine;
   me->cbUsed = TO_BYTES(nLine);

   nPos = (me->paPos) ? me->paPos[nLine] : 0;

   nChars = cchText - nPos;


   while (nChars > 0) {

      int nFits;
      IFont_MeasureTextToLineBreak(piFont, pwText + nPos, nChars, cxWrapWidth, 
                                   &nFits, bNoBreakOnSpace);

      // ensure space for new entry
      nErr = TextLineTbl_Grow(me, 1);
      if (nErr) {
         break;
      }

      me->paPos[nLine++] = nPos;     // note that first entry is unchanged
      me->cbUsed += sizeof(TLPos);
      nPos += nFits;
      nChars -= nFits;
   }

   // last entry in table is used to calc length of last line
   if (!nErr) {
      nErr = TextLineTbl_Grow(me, (bLastNewline ? 2 : 1));
      if (!nErr) {

         if (bLastNewline) {
            me->paPos[nLine++] = nPos;
            me->cbUsed += sizeof(TLPos);
         }

         me->paPos[nLine] = nPos;   
         me->cbUsed += sizeof(TLPos);
      }
   }

   me->nLines = nLine;

   return nErr;
}

void TextLineTbl_Dtor(TextLineTbl *me)
{
   FREEIF(me->paPos);
}

void TextLineTbl_Ctor(TextLineTbl *me)
{
   ZEROAT(me);
}

void TextLineTbl_Reset(TextLineTbl *me)
{
   TextLineTbl_Dtor(me);
   TextLineTbl_Ctor(me);
}

int TextLineTbl_New(TextLineTbl **ppo)
{
   TextLineTbl *me = MALLOCREC(TextLineTbl);
   if (!me) {
      return ENOMEMORY;
   }

   TextLineTbl_Ctor(me);
   return SUCCESS;
}


