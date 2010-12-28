/*
  ========================================================================

  FILE:                 TextLayout.c
  
  SERVICES:             ITextLayout

  GENERAL DESCRIPTION:  This file contains the implementation of the
                        TextLayout interface, an object that lays out and 
                        draws text.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "TextLayout.h"
#include "wutil.h"
#include "AEEFontMapModel.h"
#include "AEEHFont.h"
#include "AEETextModel.h"
#include "AEEDisplayCanvas.h"
#include "AEEComUtil.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// Line instance
typedef struct {

   int         nParagraphStart;   // character index of paragraph start

   int         nParagraphLength;  // number of characters in the paragraph

   int         dxDrawStart;       // x offset to beginning of this line,
                                  // used for true-type rendering

   int         dx;                // number of pixels in this line

   int         nLineStart;        // starting character index used for DrawText, 
                                  // for true-type rendering this is same of the paragraph start

   int         nLineLength;       // number of characters used for DrawText
                                  // for true-type rendering this is same of the paragraph start

   int         nStartIndex;       // starting character index of this line

   int         nEndIndex;         // ending character index of this line

   uint32      dwFlags;           // options for this line

} TextLayoutLine;

typedef struct {
   TextLayoutLine*        paLine;        // array of line info 
   int                    nCount;        // number of items e
   int                    cbAlloc;       // number of bytes allocated (in paLine)
} TextLayoutLines;


typedef struct {
   AECHAR*     pText;         // text array 
   int         nTextLen;      // number of characters in pText
   int         cbAlloc;       // number of bytes allocated (in pText)
   int         cbUsed;        // number of bytes used (in pText)
} TextLayoutCharacters;

// TextLayout
typedef struct 
{
   const AEEVTBL(ITextLayout)   *pvt;
   uint32                        dwRefs;

   IShell                       *piShell;
   IModule                      *piModule;

   int                           nHintWidth;
   int                           nHintRows;
   int                           nLineGap;

   IFont*                        piFont;
   IFontMapModel*                piFontMapModel;

   RGBAVAL                       rgbaText;
   RGBAVAL                       rgbaBG;

   RGBAVAL                       rgbaSelText;
   RGBAVAL                       rgbaSelBG;

   int                           nSelectionStart;
   int                           nSelectionEnd;

   IFontBidiUtil*                piFontBidiUtil;
   AEECLSID                      ciFontBidiUtil;
   
   AECHAR                        pawEllipsis[2];
   int                           nEllipsisdx;

   TextLayoutCharacters          text;
   TextLayoutLines               lines;

   WExtent                       weLayout;
   uint32                        dwLayoutFlags;

   int            nSelRect;
   AEERect*       paSelRect;

} TextLayout;

/*---------------------------------------------------------------------------------
      Globals
---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------
      Function Prototypes
---------------------------------------------------------------------------------*/

// 
// ITextLayout
//

static uint32  TextLayout_AddRef(ITextLayout *po);
static uint32  TextLayout_Release(ITextLayout *po);
static int     TextLayout_QueryInterface(ITextLayout *po, AEECLSID cls, void **ppo);
static boolean TextLayout_HandleEvent(ITextLayout *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
static int     TextLayout_GetPreferredExtent(ITextLayout *po, IModel *piModel, uint32 nLayoutFlags, WExtent *pwPreferredExtent);
static int     TextLayout_Layout(ITextLayout *po, IModel *piModel, WExtent *pWExtent, uint32 nLayoutFlags);
static int     TextLayout_Draw(ITextLayout *po, ICanvas *piCanvas, int dwX, int dwY);
static int     TextLayout_GetLayoutSize(ITextLayout *po, WExtent *pwe, int *pdwLineCount);
static int     TextLayout_GetCharLocation(ITextLayout *po, int dwIndex, AEERect *prcChar);
static int     TextLayout_GetLineFromIndex(ITextLayout *po, int dwIndex, int *pdwLine);
static int     TextLayout_GetLineStartIndex(ITextLayout *po, int dwLine,  int *pdwIndex);
static int     TextLayout_GetCaretInfo(ITextLayout *po, int dwIndex, AEERect *prcCaret);
static int     TextLayout_GetCaretAfterMove(ITextLayout *po, int dwCaret, int dwMovement, int *pdwMovedCaret);
static int     TextLayout_ComputeScrollOffset(ITextLayout *po, int dwTextWidth, int dwDisplayWidth, int dwScrollOffset);

// Local Routines
static int     TextLayout_Ctor(TextLayout *me, AEEVTBL(ITextLayout) *pvt, IShell *piShell, IModule *piModule);
static void    TextLayout_Dtor(TextLayout *me);
static int     TextLayout_AdjustUsingAlignment(TextLayout* me, TextLayoutLine* pLine, int x, uint32 dwFlags);
static int     TextLayout_GetFontHeight(TextLayout* me, int* nLineHeight);
static boolean TextLayout_GetLangDirection(TextLayout* me, boolean* pLeftToRight); 
static int     TextLayout_CacheText(TextLayout* me, const AECHAR *pwText, int nTextLen);
static int     TextLayout_MakeLines(TextLayout* me, const AECHAR* pwText, int nLen, const WExtent* we, uint32 dwFlags);
static int     TextLayout_GetTextRects(TextLayout* me, WidgetTextRects* wtr);

static int     TextLayout_SetFontPropertyBreakOnGlyph(TextLayout* me, IHFont* piHFont, uint32 dwFlags);

static int     IFont_MeasureTextToLineBreak(IFont *piFont, const AECHAR *pwText, int cchText, 
                                        int cxWidth, int *pnFits, boolean bBreakOnWord);

static const AECHAR * IModel_GetPlainText(IModel *piModel, int *pnLen, int* pnSelStart, int* pnSelEnd);

// other helpful macros
static __inline boolean TextLayout_IsParagraphBreak (TextLayout* me, const AECHAR c) {
   PARAM_NOT_REF(me); 
   return ((c == 0x000A) || (c == 0x2029) || c == 0x0 );
}

static __inline const AECHAR* TextLayout_NextCharacter    (TextLayout* me, const AECHAR* p) {
   return ++p;
}

// TextLayoutLine flags
#define TLLF_ELLIPSIS             (0x0001)
#define TLLF_RIGHT_TO_LEFT        (0x0002)
#define TLLF_USE_PIXEL            (0x0004)


/*---------------------------------------------------------------------------------

      ITextLayout

---------------------------------------------------------------------------------*/

static __inline ITextLayout *TEXTLAYOUT_TO_ITEXTLAYOUT(TextLayout *p) { 
   return (ITextLayout*) (void*) p; 
}

static __inline TextLayout *ITEXTLAYOUT_TO_TEXTLAYOUT(ITextLayout *p) { 
   return (TextLayout*) (void*) p; 
}

// TextLayout_AddRef
static uint32 TextLayout_AddRef(ITextLayout *po)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   return ++me->dwRefs;
}

// TextLayout_Release
static uint32 TextLayout_Release(ITextLayout *po)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   if (0 == --me->dwRefs) {
      TextLayout_Dtor(me);   
   }
   return me->dwRefs;
}

// TextLayout_QueryInterface
static int TextLayout_QueryInterface(ITextLayout *po, AEECLSID id, void **ppo)
{
   int nResult     = ECLASSNOTSUPPORT;
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);

   if (AEEIID_TEXTLAYOUT == id) { 
      *ppo = me;
      IQI_AddRef(*ppo);
      nResult = SUCCESS;
   } 

   return nResult;
}

static boolean TextLayout_GetProperty(TextLayout* me, uint16 wParam, uint32 dwParam) {

   boolean    bHandled = TRUE;

   switch ( wParam ) {
   case PROP_FONT:
      *(IFont **)dwParam = me->piFont;
      ADDREFIF(me->piFont);
      break;
   case PROP_FGCOLOR:
      *(RGBAVAL *)dwParam = me->rgbaText;
      break;
   case PROP_BGCOLOR:
      *(RGBAVAL *)dwParam = me->rgbaBG;
      break;
   case PROP_SELECTED_FGCOLOR:
      *(RGBAVAL *)dwParam = me->rgbaSelText;
      break;
   case PROP_SELECTED_BGCOLOR:
      *(RGBAVAL *)dwParam = me->rgbaSelBG;
      break;

   case PROP_HINT_WIDTH:
      *(int*)dwParam = me->nHintWidth;
      break;
   case PROP_HINT_ROWS:
      *(int*)dwParam = me->nHintRows;
      break;
   case PROP_LINEGAP:
      *(int*)dwParam = me->nLineGap;
      break;
   case PROP_FONTMAPMODEL:
      *(IFontMapModel **)dwParam = me->piFontMapModel;
      ADDREFIF(me->piFontMapModel);
      break;
      
   case PROP_TEXTRECTS:
      if ( SUCCESS == TextLayout_GetTextRects(me, (WidgetTextRects*) dwParam) ) {
         break;
      }
      bHandled = FALSE;
      break;

   case PROP_ELLIPSIS:
      *(AECHAR *)dwParam = me->pawEllipsis[0];
      break;

   default:
      bHandled = FALSE;
      break;
   }
   return bHandled;
}

static void TextLayout_SetFont(TextLayout* me, IFont* pFont) {

   RELEASEIF(me->piFont);
   me->piFont       = pFont;
   ADDREFIF(me->piFont);
   // create the assoicated FontBidiUtil
   if ( me->piFont ) {
      int nErr;
      IHFont *piHFont;
      AEECLSID  ciFontBidiUtil;
      nErr = IFONT_TO_IHFONT(me->piFont, &piHFont);
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

   me->nEllipsisdx = -1;

}

static boolean TextLayout_SetProperty(TextLayout* me, uint16 wParam, uint32 dwParam) {
   boolean    bHandled = TRUE;
   switch (wParam) {
   case PROP_FONT:
      TextLayout_SetFont(me, (IFont*) dwParam);
      break;
   case PROP_FGCOLOR:
      me->rgbaText     = (RGBAVAL)dwParam;
      break;
   case PROP_BGCOLOR:
      me->rgbaBG       = (RGBAVAL)dwParam;
      break;
   case PROP_SELECTED_FGCOLOR:
      me->rgbaSelText     = (RGBAVAL)dwParam;
      break;
   case PROP_SELECTED_BGCOLOR:
      me->rgbaSelBG       = (RGBAVAL)dwParam;
      break;
   case PROP_HINT_WIDTH:
      me->nHintWidth   = (int)dwParam;
      break;
   case PROP_HINT_ROWS:
      me->nHintRows    = (int)dwParam;
      break;
   case PROP_LINEGAP:
      me->nLineGap     = (int)dwParam; 
      break;
   case PROP_FONTMAPMODEL:
      RELEASEIF(me->piFontMapModel);
      me->piFontMapModel = (IFontMapModel *)dwParam;
      ADDREFIF(me->piFontMapModel);
      break;
   case PROP_ELLIPSIS:
      me->pawEllipsis[0]  = (AECHAR)dwParam;
      me->pawEllipsis[1]  = 0;
      me->nEllipsisdx = -1;
      break;
   default:
      bHandled = FALSE;
      break;
   }
   return bHandled;
}

// TextLayout_HandleEvent
static boolean TextLayout_HandleEvent(ITextLayout *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   boolean    bHandled = FALSE;

   switch(evt) {
      case EVT_WDG_GETPROPERTY:
         bHandled = TextLayout_GetProperty(me, wParam, dwParam);
         break;
      case EVT_WDG_SETPROPERTY:
         bHandled = TextLayout_SetProperty(me, wParam, dwParam);
         break;
      default:
         break;
   }

   return bHandled;
}

// TextLayout_GetPreferredExtent - return preferred extent of text, based on PROP_HINTWIDTH or PROP_HINTROWS
static int TextLayout_GetPreferredExtent(ITextLayout *po, IModel *piModel, uint32 nLayoutFlags, WExtent *pwPreferredExtent)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   int dwResult = SUCCESS;
   const AECHAR* pText;
   int   nTextLength;
   int   nFontHeight = 0;

   WExtent weClient;
   SETWEXTENT(&weClient,me->nHintWidth,0);

   if ( piModel ) {
      pText       = IModel_GetPlainText(piModel, &nTextLength, NULL, NULL);
   }
   else {
      pText       = 0;
      nTextLength = 0;
   }

   dwResult = TextLayout_MakeLines(me, pText, nTextLength, &weClient, nLayoutFlags & ~TEXTLAYOUT_FLAG_ELLIPSIFY);

   if ( SUCCESS == dwResult ) {
      dwResult = TextLayout_GetFontHeight(me, &nFontHeight);
   }


   if ( SUCCESS == dwResult ) {

      if (nLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE) {

         int height = 0;
         if ( me->nHintRows ) {
            height = MAX(0,me->nHintRows * (nFontHeight + me->nLineGap) - me->nLineGap);
         }
         else {
            height = me->lines.nCount * (nFontHeight + me->nLineGap) - me->nLineGap;
         }

         height = MAX(nFontHeight, height);

         weClient.height = height;
         weClient.width  = me->nHintWidth;
   
      } else {

         SETWEXTENT(&weClient, me->nHintWidth, nFontHeight);

         if ( me->piFont ) {
   
            // get text extent
            if (pText && nTextLength) {
               int fits;
               (void) IFONT_MeasureText(me->piFont, pText, nTextLength, 
                                 IFONT_MAXWIDTH, &fits, &weClient.width);

            }
         }
      }
   }
   if ( SUCCESS == dwResult ) {
      *pwPreferredExtent = weClient;
   }
   return dwResult;
}


// TextLayout_Layout - layout the text
static int TextLayout_Layout(ITextLayout *po, IModel *piModel, WExtent *pWExtent, uint32 dwLayoutFlags)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   int dwResult = SUCCESS;
   const AECHAR* pText;
   int   nTextLength;

   pText    = IModel_GetPlainText(piModel, &nTextLength, &me->nSelectionStart, &me->nSelectionEnd);

   dwResult = TextLayout_CacheText(me, pText, nTextLength);

   if ( SUCCESS == dwResult ) {
      dwResult = TextLayout_MakeLines(me, pText, nTextLength, pWExtent, dwLayoutFlags);
   }

   if ( SUCCESS == dwResult ) {
      // save to extent and layout flag..used in drawing
      me->weLayout      = *pWExtent;
      me->dwLayoutFlags = dwLayoutFlags;
   }


   return dwResult;
}


static void TextLayout_DrawLines(TextLayout* me, IDisplay* piDisplay, int x, int y, AEERect* prcClip)
{
   int yLineDraw;
   int nFontHeight;
   int nErr = SUCCESS;
   int nLineIndex = 0;

   nErr = TextLayout_GetFontHeight(me, &nFontHeight);
   if ( SUCCESS != nErr ) {
      return;
   }

   yLineDraw = y;

   while ( nErr == SUCCESS && nLineIndex < me->lines.nCount )
   {
      AEERect rcLine, rcLineClip, rcEllipsis;
      int     nLineLength;
      int     xDrawText;
      uint32  nLineFlags   = me->dwLayoutFlags;

      TextLayoutLine* pLine = me->lines.paLine + nLineIndex;

      rcLine.x    = (int16) x;
      rcLine.y    = (int16) yLineDraw;
      rcLine.dx   = (int16) pLine->dx;
      rcLine.dy   = (int16) nFontHeight;
      nLineLength = pLine->nLineLength;

      xDrawText   = x - pLine->dxDrawStart;
      
      SETAEERECT(&rcEllipsis,0,rcLine.y,0,rcLine.dy);

      if ( TLLF_ELLIPSIS & pLine->dwFlags ) {

         int fits;
         int dx;
         int drawdx;

         rcEllipsis.dx = (short) me->nEllipsisdx;
         rcLine.dx     = (int16) (me->weLayout.width - rcEllipsis.dx);

         // redraw the text using this layout width minus the ellipsis width
         nErr = IFONT_MeasureText(me->piFont,
            me->text.pText + pLine->nLineStart, 
            pLine->nParagraphStart + pLine->nParagraphLength - pLine->nLineStart,
            pLine->dxDrawStart + rcLine.dx,
            &fits,
            &dx);

         nLineLength = fits;
         rcLine.dx    = (int16) (dx - pLine->dxDrawStart);


         if( TLLF_RIGHT_TO_LEFT & pLine->dwFlags ) {

            // Right to Left text...
            // position ellipsis on the left side
            rcEllipsis.x = (int16) x;
            // position text after the ellipsis
            xDrawText += rcEllipsis.dx;
            rcLine.x  += rcEllipsis.dx;
            // check the width need to display this text
            nErr = IFONT_MeasureText(me->piFont,
               me->text.pText + pLine->nLineStart, 
               fits, 
               IFONT_MAXWIDTH,
               &fits,
               &drawdx);

            if ( drawdx > dx ) {
               xDrawText -= drawdx - dx;
            }

         }
         else {

            // Left to Right text...
            // position ellipsis on the right side
            rcEllipsis.x = (int16) (rcLine.x + rcLine.dx);
         }
      }

      if ( nLineFlags & IDF_ALIGN_LEFT ) {
         // already aligned left
      }
      else if ( rcLine.dx < me->weLayout.width ) {
         if ( nLineFlags & IDF_ALIGN_CENTER) {
            int16 xShift = int32toint16( (me->weLayout.width - rcEllipsis.dx - rcLine.dx) / 2);
            rcEllipsis.x += xShift;
            rcLine.x     += xShift;
            xDrawText    += xShift;
         }
         else if ( nLineFlags & IDF_ALIGN_RIGHT || 
                   pLine->dwFlags & TLLF_RIGHT_TO_LEFT ) {
            int16 xShift = int32toint16(me->weLayout.width - rcEllipsis.dx - rcLine.dx);
            rcEllipsis.x += xShift;
            rcLine.x     += xShift;
            xDrawText    += xShift;
         }
      }



      if ( TLLF_ELLIPSIS & pLine->dwFlags  ) {

         // draw the ellipsis first ...
         if ( IntersectRect(&rcEllipsis, &rcEllipsis, prcClip) ) {
            (void) IDISPLAY_DrawText(piDisplay, AEE_FONT_NORMAL, 
                     me->pawEllipsis, 
                     WSTRLEN(me->pawEllipsis),
                     rcEllipsis.x, yLineDraw, &rcEllipsis, nLineFlags & ~IDF_ALIGN_MASK);
         }

      }

      // draw the text
      if ( IntersectRect(&rcLineClip, &rcLine, prcClip) ) {

         (void) IDISPLAY_DrawText(piDisplay, AEE_FONT_NORMAL,
            me->text.pText + pLine->nLineStart, 
            nLineLength,
            xDrawText, yLineDraw, &rcLineClip, nLineFlags & ~IDF_ALIGN_MASK );
      }
      

      yLineDraw += nFontHeight + me->nLineGap;
      if ( yLineDraw > prcClip->y + prcClip->dy ) {
         break;
      }

      ++nLineIndex;
   }
  
}

// TextLayout_Draw - draw the text
static int TextLayout_Draw(ITextLayout *po, ICanvas *piCanvas, int x, int y)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   IHFont * piHFont = 0;
   int nErr = SUCCESS;

   IDisplay* piDisplay;
   AEERect rcClip;
   RGBVAL  rgbOriginal = 0;

   IFont* piFont = 0;

   if ( !me || !piCanvas ) {
      return EBADPARM;
   }

   if ( me->piFont == 0 || me->text.nTextLen == 0 ) {
      return SUCCESS;
   }
   
   nErr = ICANVAS_GetDisplay(piCanvas, &piDisplay);

   if ( SUCCESS == nErr ) {
      piFont = IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, me->piFont);
      nErr   = ICANVAS_GetClipRect(piCanvas, &rcClip);
   }

   if ( SUCCESS == nErr ) {

      rgbOriginal = IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, me->rgbaText & 0xFFFFFF00 );

      if ( IFONT_TO_IHFONT(me->piFont, &piHFont) == SUCCESS )
      {
         int nCurrentWidth = 0;
         int nDesiredWidth = me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE ? me->weLayout.width : 0x7FFF;
         int nPropertyErr  = SUCCESS;
         nPropertyErr = IHFONT_GetProperty(piHFont, PROP_FONTLINEWIDTH, (uint32*) &nCurrentWidth);
         if ( SUCCESS == nPropertyErr ) {
            if ( nCurrentWidth != nDesiredWidth ) {
               (void) IHFONT_SetProperty(piHFont, PROP_FONTLINEWIDTH, (uint32) nDesiredWidth );
            }
         }

         (void) TextLayout_SetFontPropertyBreakOnGlyph(me, piHFont, me->dwLayoutFlags);

         RELEASEIF(piHFont); 
      }
   }

   if ( SUCCESS == nErr ) {
      // draw the text
      TextLayout_DrawLines(me, piDisplay, x, y, &rcClip);
   }

   // redraw the selected text with selection color
   if ( SUCCESS == nErr ) {
      if ( me->nSelectionEnd != me->nSelectionStart ) {
         int nSelMin = MIN(me->nSelectionEnd , me->nSelectionStart);
         int nSelMax = MAX(me->nSelectionEnd , me->nSelectionStart);

         // highlight the selected text by redrawing it.
         WidgetTextRects wtr;
         wtr.count  = (uint32) me->nSelRect;
         wtr.flags  = 0;
         wtr.start  = (uint32) nSelMin;
         wtr.end    = (uint32) (nSelMax ? (nSelMax - 1) : 0);
         wtr.pRects = me->paSelRect;
         nErr = TextLayout_GetTextRects(me, &wtr);
         if ( SUCCESS == nErr && (wtr.count > (uint32) me->nSelRect) ) {
            FREEIF(me->paSelRect);
            me->nSelRect  = (int) wtr.count;
            me->paSelRect = MALLOC(sizeof(AEERect)*wtr.count);
            wtr.count  = (uint32) me->nSelRect;
            wtr.pRects = me->paSelRect;
            nErr = TextLayout_GetTextRects(me, &wtr);
         }

         if ( SUCCESS == nErr ) {
            (void) IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, (me->rgbaSelText & 0xFFFFFF00) );
            while ( wtr.count-- ) {
               AEERect* prcSelected = me->paSelRect + wtr.count;
               prcSelected->x += (int16) x;
               prcSelected->y += (int16) y;
               (void) BlendRect(piDisplay, prcSelected, me->rgbaSelBG, RGBA_GETALPHA(me->rgbaSelBG));
               TextLayout_DrawLines(me, piDisplay, x, y, prcSelected);
            }
         }
      }
   }

   if ( SUCCESS == nErr ) {
      (void) IDISPLAY_SetColor(piDisplay, CLR_USER_TEXT, rgbOriginal);
      (void) IDISPLAY_SetFont(piDisplay, AEE_FONT_NORMAL, piFont);
   }

   RELEASEIF(piDisplay);
   
   return nErr;
}


// TextLayout_GetLayoutSize - return the actual size of a layout
static int TextLayout_GetLayoutSize(ITextLayout *po, WExtent *pwe, int *pdwLineCount)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);

   int dwResult = SUCCESS;
   int nFontHeight;

   dwResult = TextLayout_GetFontHeight(me, &nFontHeight);

   if (SUCCESS == dwResult ) {

      if ( pdwLineCount ) {
         *pdwLineCount = me->lines.nCount;
      }

      if ( pwe ) {
         if ( me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE ) {
            pwe->width  = me->weLayout.width;
            pwe->height = MAX(0, me->lines.nCount * (nFontHeight + me->nLineGap) - me->nLineGap);
         }
         else {
            pwe->width  = me->lines.nCount ? me->lines.paLine->dx : 0;
            pwe->height = MAX(0, nFontHeight);
         }
      }
   }

   return dwResult;
}

static int TextLayout_GetTextRects(TextLayout* me, WidgetTextRects* wtr) {

   int nErr = SUCCESS;
   uint32   nCount = 0;
   AEERect* pRects = 0;

   if ( 0 == wtr->flags ) {
      AEERect  rcChar;
      AEERect  rcIntersect;
      AEERect  rcLast;

      uint32 nStart = MIN(wtr->start, wtr->end);
      uint32 nEnd   = MAX(wtr->end,   wtr->start);
      uint32 nIndex = nStart;

      pRects = wtr->pRects;
      SETAEERECT(&rcLast, -1,0,0,0);

      for ( ; nIndex <= nEnd; ++nIndex ) {
         nErr = TextLayout_GetCharLocation(TEXTLAYOUT_TO_ITEXTLAYOUT(me), (int) nIndex, &rcChar);
         if ( nErr != SUCCESS ) {
            nErr = SUCCESS;  // not a fatal error, just can't find the character.
            continue;
         }
         // consolidate the rect if it's intersects with the last one.
         if ( rcLast.dx > 0 ) {
            AEERect rcComp;
            SETAEERECT(&rcComp, rcLast.x-1, rcLast.y, rcLast.dx+2, rcLast.dy);
            if ( IntersectRect(&rcIntersect, &rcComp, &rcChar) ) {
               UnionRect(&rcLast,&rcLast,&rcChar);
               if ( nCount <= wtr->count ) {
                  *(pRects-1) = rcLast;
               }
               continue;
            }
         }

         rcLast = rcChar;
         ++nCount;
         if ( nCount <= wtr->count ) {
            *pRects = rcChar;
            ++pRects;
         }
      }
   }
   else {
      nErr = EFAILED;
   }

   wtr->count = nCount;
   return nErr;

}

// TextLayout_GetCharLocation - return the visual location a character
static int TextLayout_GetCharLocation(ITextLayout *po, int nIndex, AEERect *prcChar)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   int nErr = SUCCESS;

   // find the paragraph that this index is on.
   int nLineIndex = 0;
   int nIndexLine = -1;
   int nIndexPos  = -1;
   int y          = 0;
   int nLineHeight = 0;
   TextLayoutLine* pLastLine = 0;

   nErr = TextLayout_GetFontHeight(me, &nLineHeight);
   if ( SUCCESS != nErr ) {
      return nErr;
   }
   
   while ( nLineIndex < me->lines.nCount ) {

      TextLayoutLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nParagraphStart <= nIndex && pLine->nParagraphLength + pLine->nParagraphStart - 1 >= nIndex ) {
         nIndexLine = nLineIndex;
         break;
      }

      if ( pLastLine && nIndex < pLine->nParagraphStart ) {
         nIndexLine = nLineIndex - 1;
         if ( pLastLine->dwFlags & TLLF_RIGHT_TO_LEFT && pLastLine->dx < me->weLayout.width) {
            nIndexPos = me->weLayout.width - pLastLine->dx;
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
        nIndexLine == -1 && nIndex > pLastLine->nParagraphStart + pLastLine->nParagraphLength - 1  ) {
      nIndexLine = nLineIndex - 1;
      pLastLine = 0;
   }

   if ( nIndexLine != -1 && nIndexPos == -1 ) {
      // the index is located within this paragraph...find the dx
      TextLayoutLine* pLine = me->lines.paLine + nIndexLine;
      if ( pLastLine ) {
         y += nLineHeight + me->nLineGap;
      }

      if ( pLine->dwFlags & TLLF_USE_PIXEL ) {
         // lines are broken using pixels...
         AEERect rcGlyph;
         int     nCurrentIndex = nIndexLine;
         nErr = IFONTBIDIUTIL_CharLocation(
                        me->piFontBidiUtil, 
                        me->piFont, 
                        me->text.pText + pLine->nParagraphStart,
                        pLine->nParagraphLength,
                        nIndex - pLine->nParagraphStart,
                        &rcGlyph);
         // now, find the line the cursor is
         while ( nCurrentIndex < me->lines.nCount ) {
            if ( rcGlyph.x < pLine->dxDrawStart + pLine->dx) {
               rcGlyph.x -= (int16) pLine->dxDrawStart;
               if ( pLine->dwFlags & TLLF_RIGHT_TO_LEFT && ((me->weLayout.width > pLine->dx) || (me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE))) {
                  rcGlyph.x += (int16) (me->weLayout.width - pLine->dx);
               }
               rcGlyph.y  = (int16) y;
               rcGlyph.dy = (int16) nLineHeight;
               *prcChar = rcGlyph;
               return SUCCESS;
            }
            y += nLineHeight + me->nLineGap;
            ++pLine;
            ++nCurrentIndex;
         }
      } 
      else {
         // lines are broken using characters..find the line with this character.
         while ( nIndexLine < me->lines.nCount ) {
            if ( pLine->nLineStart <= nIndex && pLine->nLineStart + pLine->nLineLength >  nIndex ) {
               AEERect rcGlyph;
               nErr = IFONTBIDIUTIL_CharLocation(
                              me->piFontBidiUtil, 
                              me->piFont, 
                              me->text.pText + pLine->nLineStart,
                              pLine->nLineLength,
                              nIndex - pLine->nLineStart,
                              &rcGlyph);
         
               nIndexPos = rcGlyph.x;
               if ( pLine->dwFlags & TLLF_RIGHT_TO_LEFT && me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE ) {
                  rcGlyph.x += (int16) (me->weLayout.width - pLine->dx);
               }
               rcGlyph.y  = (int16) y;
               rcGlyph.dy = (int16) nLineHeight;
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


// TextLayout_GetLineFromIndex - return which line an in-memory character is on
static int TextLayout_GetLineFromIndex(ITextLayout *po, int nIndex, int *pnLine)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   // find the line that this index is on.
   int nLineIndex = 0;
   int nIndexLine = -1;
   while ( nLineIndex < me->lines.nCount ) {

      TextLayoutLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nStartIndex <= nIndex ) {
         nIndexLine = nLineIndex;
      }
      if ( pLine->nEndIndex >= nIndex ) {
         break;
      }
      ++nLineIndex;
   }

   if ( nIndexLine != -1 )
   {
      *pnLine = nIndexLine;
   } 
   else {
      *pnLine = 0;
   }

   return SUCCESS;
}

// TextLayout_GetLineStartIndex - return which in-memory character a line starts with
static int TextLayout_GetLineStartIndex(ITextLayout *po, int nLine, int *pnIndex)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   TextLayoutLine* pLine;

   if ( me->lines.nCount == 0 ) {
      return EFAILED;
   }

   if ( nLine < 0 || nLine >= me->lines.nCount ) {
      return EFAILED;
   }

   pLine = me->lines.paLine + nLine;
   *pnIndex = pLine->nStartIndex;
  
   return SUCCESS;

}

// TextLayout_GetCaretInfo - get caret size and position
static int TextLayout_GetCaretInfo(ITextLayout *po, int nIndex, AEERect *prcCaret)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   int nErr = SUCCESS;

   // find the line that this index is on.
   int nLineIndex = 0;
   int nIndexPos  = -1;
   int y          = 0;
   int nLineHeight = 0;

   TextLayoutLine* pLastLine    = 0;
   TextLayoutLine* pFoundLine   = 0;

   nErr = TextLayout_GetFontHeight(me, &nLineHeight);
   if ( SUCCESS != nErr ) {
      return nErr;
   }

   while ( nLineIndex < me->lines.nCount ) {

      TextLayoutLine* pLine = me->lines.paLine + nLineIndex;

      if ( pLine->nStartIndex <= nIndex && pLine->nEndIndex >= nIndex ) {
         // found the line!
         pFoundLine = pLine;
         break;
      }

      if ( pLastLine && nIndex < pLine->nStartIndex ) {
         // between the line
         pFoundLine = pLastLine;
         break;

      }
      if ( pLastLine ) {
         y += nLineHeight + me->nLineGap;
      }

      pLastLine = pLine;
      ++nLineIndex;
   }

   if ( 0 == pFoundLine && pLastLine ) {
      // didn't find the index...assume it's at the end.
      pFoundLine = pLastLine;
   }
   

   if ( pFoundLine ) {

      int dwCursorX;
      if ( nIndex > pFoundLine->nEndIndex ) {
         nIndex = pFoundLine->nEndIndex + 1;
      }
      else {
         // caret is somewhere on this line
         if ( pLastLine ) {
            y += nLineHeight + me->nLineGap;
         }
      }
      nErr = IFONTBIDIUTIL_CaretInfo(
                     me->piFontBidiUtil, 
                     me->piFont, 
                     me->text.pText + pFoundLine->nLineStart,
                     pFoundLine->nLineLength,
                     nIndex - pFoundLine->nLineStart,
                     &dwCursorX);
      if ( SUCCESS == nErr ) {
         int nAdjPos = TextLayout_AdjustUsingAlignment(me, pFoundLine, dwCursorX - pFoundLine->dxDrawStart, me->dwLayoutFlags);
         nIndexPos = MAX(0,nAdjPos);
      }
   }

   if ( SUCCESS == nErr ) {
      if ( -1 != nIndexPos ) {
         prcCaret->x  = (int16) nIndexPos;
         prcCaret->y  = (int16) y;
      } 
      else {
         // no text....position cursor at 0,0
         prcCaret->x  = 0;
         prcCaret->y  = 0;
      }
      prcCaret->dx = 1;
      prcCaret->dy = (int16) nLineHeight;
   }
   
   return nErr;
}


// TextLayout_GetCaretAfterMove - get the location of the caret after a move
static int TextLayout_GetCaretAfterMove(ITextLayout *po, int nCaret, int dwMovement, int *pnMovedCaret)
{
   TextLayout *me = ITEXTLAYOUT_TO_TEXTLAYOUT(po);
   int nErr = SUCCESS;

   switch (dwMovement) {
      case MOVEMENT_CHARACTER_LEFT:
      case MOVEMENT_CHARACTER_RIGHT:
         {

            // find the paragraph we're in
            int nIndex = 0;
            if ( nCaret < 0 ) {
               nCaret = 0;
            }
            if ( nCaret > me->text.nTextLen ) {
               nCaret = me->text.nTextLen;
            }
            while ( nIndex < me->lines.nCount ) {

               TextLayoutLine* pLine = me->lines.paLine + nIndex;
               if ( pLine->nParagraphStart <= nCaret && 
                    pLine->nParagraphLength + pLine->nParagraphStart >= nCaret ) {

                  int nParagraphCaret = nCaret - pLine->nParagraphStart;
                  //  We're in the paragraph to this index
                  nErr = IFONTBIDIUTIL_MoveCaretIndex(
                        me->piFontBidiUtil, 
                        me->piFont, 
                        me->text.pText + pLine->nParagraphStart,
                        pLine->nParagraphLength,
                        nParagraphCaret, (MOVEMENT_CHARACTER_LEFT == dwMovement) ? -1 : 1, &nParagraphCaret);
                  if ( SUCCESS == nErr ) {
                     nCaret = nParagraphCaret + pLine->nParagraphStart;
                  } else {
                     if ( pLine->dwFlags & TLLF_RIGHT_TO_LEFT && me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE ) {
                        if ( MOVEMENT_CHARACTER_RIGHT == dwMovement ) {
                           --nCaret;
                        }
                        if ( MOVEMENT_CHARACTER_LEFT  == dwMovement ) {
                           ++nCaret;
                        }
                     }
                     else {
                        if ( MOVEMENT_CHARACTER_RIGHT == dwMovement ) {
                           ++nCaret;
                        }
                        if ( MOVEMENT_CHARACTER_LEFT  == dwMovement ) {
                           --nCaret;
                        }
                     }
                     nErr = SUCCESS;
                  }
                  if ( nCaret < 0 ) {
                     nCaret = 0;
                  }
                  if ( nCaret > me->text.nTextLen ) {
                     nCaret = me->text.nTextLen;
                  }
                  break;

               }
               ++nIndex;
            }
         }
         break;
      case MOVEMENT_LINE_UP:
      case MOVEMENT_LINE_DOWN:
      case MOVEMENT_WORD_LEFT:
      case MOVEMENT_WORD_RIGHT:
      case MOVEMENT_LINE_END:
      case MOVEMENT_LINE_BEGIN:
      default:
         nErr = EFAILED;
   }
      
   if ( SUCCESS == nErr ) {
      *pnMovedCaret = nCaret;
   }
   return nErr;
}

static int TextLayout_ComputeScrollOffset(ITextLayout *po, int dwTextWidth, int dwDisplayWidth, int dwScrollOffset)
{
   TextLayout* me = (TextLayout*) (void*) po;

   if ( me->dwLayoutFlags & TEXTLAYOUT_FLAG_MULTILINE  ||
        me->dwLayoutFlags & TEXTLAYOUT_FLAG_ELLIPSIFY  ||
        dwDisplayWidth >= dwTextWidth ) {
      dwScrollOffset = 0;
   } 
   else {
      boolean bLTRLang;

      TextLayout_GetLangDirection(me, &bLTRLang);
      if ( !bLTRLang ) {
         dwScrollOffset = - (dwTextWidth - dwDisplayWidth) + dwScrollOffset; 
      }
      else {
         dwScrollOffset = - dwScrollOffset;
      }
   }
   return dwScrollOffset;
}

/*---------------------------------------------------------------------------------
      CreateInstance
---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    TextLayout_New  
   --- */
int TextLayout_New(ITextLayout **ppo, IShell *piShell, IModule *piModule)
{
   int dwResult = SUCCESS;
   TextLayout *me;

   // sanity check parameters
   if(!ppo) {
		return(EFAILED);
   }

   // initialize
   *ppo = NULL;

   // allocate memory
   me = MALLOCREC_VTBL(TextLayout, ITextLayout);
   if (!me) {
      dwResult = ENOMEMORY;
   }

   // construct ITextLayout
   if (dwResult == SUCCESS && me ) { 
      dwResult = TextLayout_Ctor(me, GETVTBL(me,ITextLayout), piShell, piModule);

      // return result
      if (dwResult == SUCCESS) {
         *ppo = TEXTLAYOUT_TO_ITEXTLAYOUT(me);
      } else {
         (void) TextLayout_Release(TEXTLAYOUT_TO_ITEXTLAYOUT(me));
      }
   }

   return dwResult;
}

/*---------------------------------------------------------------------------------

      Local Routines

---------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
    TextLayout_Ctor 
   --- */
static int TextLayout_Ctor(TextLayout *me, AEEVTBL(ITextLayout) *pvt, IShell *piShell, IModule *piModule)
{
   int dwResult = SUCCESS;

   me->pvt = pvt;

   pvt->AddRef             = TextLayout_AddRef;
   pvt->Release            = TextLayout_Release;
   pvt->QueryInterface     = TextLayout_QueryInterface;
   pvt->HandleEvent        = TextLayout_HandleEvent;
   pvt->GetPreferredExtent = TextLayout_GetPreferredExtent;
   pvt->Layout             = TextLayout_Layout;
   pvt->Draw               = TextLayout_Draw;
   pvt->GetLayoutSize      = TextLayout_GetLayoutSize;
   pvt->GetLineFromIndex   = TextLayout_GetLineFromIndex;
   pvt->GetLineStartIndex  = TextLayout_GetLineStartIndex;
   pvt->GetCaretInfo       = TextLayout_GetCaretInfo;
   pvt->GetCaretAfterMove  = TextLayout_GetCaretAfterMove;
   pvt->ComputeScrollOffset= TextLayout_ComputeScrollOffset;

   me->dwRefs = 1;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

   me->piModule = piModule;
   IMODULE_AddRef(me->piModule);

   return dwResult;
}

/* -----------------------------------------------------------------------
    HFont_Dtor
   --- */
static void TextLayout_Dtor(TextLayout *me)
{
   RELEASEIF(me->piModule);
   RELEASEIF(me->piShell);

   RELEASEIF(me->piFont);
   RELEASEIF(me->piFontBidiUtil);
   RELEASEIF(me->piFontMapModel);

   FREEIF(me->text.pText);
   FREEIF(me->lines.paLine);
   FREEIF(me->paSelRect);

   // free me
   FREE(me);
}


static int TextLayout_AdjustUsingAlignment(TextLayout* me, TextLayoutLine* pLine, int x, uint32 dwFlags ) {

   if ( dwFlags & TEXTLAYOUT_FLAG_MULTILINE ) {

      if ( pLine->dwFlags & TLLF_RIGHT_TO_LEFT ) {
         if ( pLine->dx > me->weLayout.width ) {
            // line is trim when its too long, so the x need to be adjusted
            // with this trimming, scrolling the start of the text into view
            x += me->weLayout.width - pLine->dx;
         }
         x = x + MAX(0, me->weLayout.width - pLine->dx);
      }
      else {
         x = MIN(me->weLayout.width, x);
      }
   }
   return x;
}


static int TextLayout_GetFontHeight(TextLayout* me, int* nFontHeight) {
   int nErr = SUCCESS;
   
   if ( me->piFont ) {
      AEEFontInfo fi;
      nErr = IFONT_GetInfo(me->piFont, &fi, sizeof(AEEFontInfo));
      if ( SUCCESS == nErr ) {
         *nFontHeight = fi.nAscent + fi.nDescent;
      }
   }
   else {
      *nFontHeight = 0;
   }
   return nErr;
}


static boolean TextLayout_GetLangDirection  (TextLayout* me, boolean* pLeftToRight) {

   if ( IFONTBIDIUTIL_Characterize(
               me->piFontBidiUtil, 
               me->text.pText, 
               me->text.nTextLen,
               0, 
               NULL) == BIDICHARTYPECAT_STRONGRIGHT) {
      *pLeftToRight = FALSE;
   } 
   else {
      *pLeftToRight = TRUE;
   }

   return SUCCESS;

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

   (void) IFONT_MeasureText(piFont, pwText, cchText, cxWidth, &fits, &cx);

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

static int TextLayout_CacheText (TextLayout* me, const AECHAR *pwText, int nTextLen)
{
   int   nErr     = SUCCESS;
   int   cbAlloc  = me->text.cbAlloc;
   int   cbUsed   = me->text.cbUsed;
   int   cbMore   = 0;

   // compute needed bytes, make sure to include the extra int32 zero terminator
   if (0 == me->text.cbUsed) {
      cbMore = (nTextLen + 1) * uint32toint32(sizeof(AECHAR));
   }
   else {
      cbMore = (nTextLen - me->text.nTextLen) * uint32toint32(sizeof(AECHAR));
   }

   if (cbMore > 0) {
      nErr = MemGrowEx((void**)&me->text.pText, &cbAlloc, cbUsed, cbMore, 48);
   }

   if (SUCCESS == nErr) {
      cbUsed += cbMore;

      if (NULL != pwText) {
         MEMCPY(me->text.pText, pwText, int32touint32(nTextLen) * sizeof(AECHAR));
      }
      me->text.pText[nTextLen] = '\0';

      me->text.cbAlloc  = cbAlloc;
      me->text.cbUsed   = cbUsed;
      me->text.nTextLen = nTextLen;
   }

   return nErr;
}

static int TextLayout_SetFontPropertyBreakOnGlyph(TextLayout* me, IHFont* piHFont, uint32 dwFlags) {

   int nPropertyErr  = SUCCESS;
   int nBreakOnGlyph = FALSE;

   nPropertyErr = IHFONT_GetProperty(piHFont, PROP_LINEBREAKONGLYPH, (uint32*) &nBreakOnGlyph);
   if ( SUCCESS == nPropertyErr ) {
      boolean bCurrentBreakOnGlyph = (boolean) nBreakOnGlyph;
      boolean bDesiredBreakOnGlyph = dwFlags & TEXTLAYOUT_FLAG_BREAKONWORD ? FALSE : TRUE;
      if ( bCurrentBreakOnGlyph != bDesiredBreakOnGlyph ) {
         nBreakOnGlyph = (boolean) bDesiredBreakOnGlyph;
         (void) IHFONT_SetProperty(piHFont, PROP_LINEBREAKONGLYPH, int32touint32(nBreakOnGlyph) );
      }
   }
   return SUCCESS;
}



static int   TextLayout_MakeLines(TextLayout* me, const AECHAR* pText, int nLen, const WExtent* pwe, uint32 dwFlags) {

   const   AECHAR* p;
   int     nErr      = SUCCESS;
   int     dxLineStart  = 0;
   int     dxLineEnd    = 0;
   int     dwStartIndex = 0;
   int     dwEndIndex   = 0;
   TextLayoutLine * pLine = 0;
   IHFont         * piHFont    = 0;
   boolean bUsePixel   = FALSE;
   int     dwFits      = 0;
   int     dwLineStart = 0;
   int     nFontHeight;
   int     nLayoutHeight;
   boolean bStopForEllipsis = FALSE;
   boolean bHardLineBreak   = TRUE;

   // reset line count to zero
   me->lines.nCount = 0;

   if ( 0 == me->piFont || 0 == pwe->width  ) {
      return SUCCESS;
   }

   // does this font support the LINEWIDTH property?
   // if so, then break the lines using pixels,
   // if not, then break the lines using characters.
   nErr = IFONT_TO_IHFONT(me->piFont, &piHFont);
   if ( nErr == SUCCESS )
   {
      uint32 dw;
      if ( IHFONT_GetProperty(piHFont, PROP_FONTLINEWIDTH, &dw) == SUCCESS ) {
         bUsePixel = TRUE;
      }

      (void) TextLayout_SetFontPropertyBreakOnGlyph(me, piHFont, dwFlags);

      RELEASEIF(piHFont);
   }
   else {
      // not really an error, so reset the error condition
      nErr = SUCCESS;
   }

   // get the font height, so we can compute the 
   // line to draw an ellipsis on.
   nErr = TextLayout_GetFontHeight(me, &nFontHeight);
   if ( SUCCESS != nErr ) {
      return nErr;
   }
   nLayoutHeight = 0;

   // break text into a paragraph.
   p = pText;

   if ( !p ) {
      return SUCCESS;
   }

   if ( nLen == -1 ) {
      nLen = WSTRLEN(p);
   }

   while ( pText && p && // null pointer check
      (( *p && p < pText + nLen && FALSE == bStopForEllipsis ) // not the end of text 
                                   || TRUE ==  bHardLineBreak) // or, create a trailing blank line from previous paragraph break 
                                                               ) {

      int start, end;

      // bForceLine is used to insure every paragraph
      // contains at least one line.  It forces a blank line
      // for paragragh without any text.
      boolean bForceLine = FALSE;

      uint32 nLineFlags = 0;

      start = p - pText;
      end   = start;

      bHardLineBreak = FALSE;

      if ( TEXTLAYOUT_FLAG_MULTILINE & dwFlags ) {
         // multiline, find paragraph break
         while ( p && *p && p < pText + nLen ) {
            boolean bParagraphBreak = TextLayout_IsParagraphBreak(me, *p );
            p = TextLayout_NextCharacter(me, p);

            if ( bParagraphBreak ) {
               bHardLineBreak = TRUE;
               break;
            }
            if ( ! (p && *p && p < pText + nLen ) ) {
               break;
            }
            ++end;
          }
      }
      else {
         // single line, one paragraph
         p   += nLen;
         end  = nLen - 1;
      }

      // create a blank line for this paragraph without any text
      if ( start == end ) {
         bForceLine = TRUE;
      }

      nLineFlags = 0;  // default to left

      // find if the paragraph string is LtoR or RtoL
      if ( IFONTBIDIUTIL_Characterize(
                  me->piFontBidiUtil, 
                  pText + start, 
                  end - start + 1, 
                  0, 
                  NULL)
            == BIDICHARTYPECAT_STRONGRIGHT) {
         nLineFlags = TLLF_RIGHT_TO_LEFT;
      } 
      
      dxLineStart  = 0;
      dwLineStart  = start;
      dwStartIndex = start;

      ///////////////////////////////////////
      // now break the paragraph into lines
      //////////////////////////////////////
      while ( SUCCESS == nErr && dwLineStart <= end && FALSE == bStopForEllipsis ) {


         // figure out where to break this line of text
         if ( dwFlags & TEXTLAYOUT_FLAG_MULTILINE ) {

            if ( bUsePixel ) {
               int nTextLen = end - dwLineStart + 1;
               if ( bHardLineBreak ) {
                  --nTextLen;
               }
               nErr = IFONTBIDIUTIL_Breakline(
                     me->piFontBidiUtil, 
                     me->piFont, 
                     pText + dwLineStart, 
                     nTextLen, 
                     dxLineStart, 
                     pwe->width,  
                     &dxLineEnd,
                     &dwFits);
               dwEndIndex = MAX(dwStartIndex, dwStartIndex + dwFits - 1);
               dwFits     = end - dwLineStart + 1;  // reset dwFits to the whole paragraph
            }
            else {
               (void) IFont_MeasureTextToLineBreak(
                        me->piFont, 
                        pText + dwLineStart, 
                        end - dwLineStart + 1, 
                        pwe->width,
                        &dwFits, 
                        (boolean) (dwFlags & TEXTLAYOUT_FLAG_BREAKONWORD ? 1 : 0));

               // measure the broken line.
               nErr = IFONT_MeasureText(me->piFont, 
                        pText + dwLineStart, 
                        dwFits, 
                        IFONT_MAXWIDTH, 
                        &dwFits,  
                        &dxLineEnd);

               dwEndIndex = MAX(dwStartIndex, dwStartIndex + dwFits - 1);

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
               nErr = IFONT_MeasureText(me->piFont, 
                  pText + start, 
                  end - start + 1, 
                  IFONT_MAXWIDTH, 
                  &dwFits,  
                  &dxLineEnd);
               dwFits     = end - start + 1;
               dwEndIndex = dwStartIndex + dwFits - 1;
            }
         }

         if ( dxLineEnd || bForceLine ) {


            if (  NULL != pLine &&
                 dwFlags & TEXTLAYOUT_FLAG_ELLIPSIFY  &&
                 nLayoutHeight + nFontHeight > pwe->height ) {
               // woo..this next line overflows the display area, 
               // so draw an ellipsis on the previous line
               pLine->dwFlags |= TLLF_ELLIPSIS;
               bStopForEllipsis = TRUE;
            }
            else 
            {


               // allocate room for this line
               nErr = MemGrowEx((void**)&me->lines.paLine, &me->lines.cbAlloc, 
                  (int) sizeof(TextLayoutLine) * me->lines.nCount, 
                  (int) sizeof(TextLayoutLine) * (me->lines.nCount + 1),
                  (int) sizeof(TextLayoutLine) * (me->lines.nCount ? 5 : 0 ));

               if ( nErr ) {
                  break;
               }

               pLine = me->lines.paLine + me->lines.nCount;
               ++me->lines.nCount;


               pLine->nParagraphStart  = start;
               pLine->nParagraphLength = end - start + 1;
               pLine->dwFlags          = nLineFlags;
               pLine->dxDrawStart      = dxLineStart;
               pLine->dx               = dxLineEnd - dxLineStart;
               pLine->nLineLength      = dwFits;
               pLine->nLineStart       = dwLineStart;
               pLine->nStartIndex      = dwStartIndex;
               pLine->nEndIndex        = dwEndIndex;

               if ( bUsePixel ) {
                  pLine->dwFlags      |= TLLF_USE_PIXEL;
                  dxLineStart          = dxLineEnd;
               }
               else {

                  if ( TEXTLAYOUT_FLAG_MULTILINE & dwFlags ) {
                     // strip trailing paragraph glyphs and whitespace
                     int    nLineLength = pLine->nLineLength;
                     const AECHAR *pw = pText + pLine->nLineStart + nLineLength - 1;
                     while ( nLineLength) {
                        if ( *pw ) {  // remove any null characters
                           if ( TEXTLAYOUT_FLAG_TRIMLINE & dwFlags ) {
                              if ( !W_ISWHITESPACE(*pw) ) {  // remove whitespace at end of line
                                 break;
                              }
                           }
                           else {
                              if ( !TextLayout_IsParagraphBreak(me, *pw ) ) {  // remove paragraph glyphs
                                 break;
                              }
                           }
                        }
                        nLineLength--;
                        pw--;
                     }
                     if ( nLineLength != pLine->nLineLength ) {
                        int fits;
                        int dx;
                        nErr = IFONT_MeasureText(me->piFont, 
                           pText + pLine->nLineStart, 
                           nLineLength,
                           IFONT_MAXWIDTH, 
                           &fits,  
                           &dx);
                        pLine->dx = (int16) dx;
                        pLine->nLineLength = fits;
                     }
                  }

                  // define the start of the next line
                  dwLineStart   += dwFits;
               }

               dwStartIndex   = dwEndIndex + 1;

               nLayoutHeight += nFontHeight + me->nLineGap;

            }
            bForceLine = FALSE;
         }
         else { // dwFits == 0, so we're at the end of the paragraph!
            break;
         }

      }
   }
   if ( SUCCESS == nErr  &&
        dwFlags & TEXTLAYOUT_FLAG_ELLIPSIFY && 
        pLine               ) {

      // for single-line layout, check if the line length
      if ( ! (dwFlags & TEXTLAYOUT_FLAG_MULTILINE)  ) {
         // is the right end of the line showing?
         if ( pLine->dx > pwe->width ) {
            pLine->dwFlags |= TLLF_ELLIPSIS;
         }
      }

      if ( pLine->dwFlags & TLLF_ELLIPSIS ) {
         // compute the width of this ellipsis
         if ( -1 == me->nEllipsisdx ) {
            int fits = 0;
            nErr = IFONT_MeasureText(me->piFont, 
               me->pawEllipsis, 
               WSTRLEN(me->pawEllipsis),
               IFONT_MAXWIDTH, 
               &fits,  
               &me->nEllipsisdx);
         }
      }
   }

   return nErr;
}

static const AECHAR * IModel_GetPlainText(IModel *piModel, int *pnLen, int* pnSelStart, int* pnSelEnd )
{
   ITextModel *pitm;
   IValueModel *pivm;
   TextInfo ti;
   const AECHAR *pwText = NULL;

   // initialize
   *pnLen = 0;
   if ( pnSelStart ) {
      *pnSelStart = 0;
   }
   if ( pnSelEnd ) {
      *pnSelEnd = 0;
   }

   if (SUCCESS == IMODEL_QueryInterface(piModel, AEEIID_TEXTMODEL, (void **)&pitm)) {
      // this is a text model.
      ITEXTMODEL_GetTextInfo(pitm, &ti);
      *pnLen = ti.cchText;
      pwText = ti.pwText;
      if ( pnSelStart ) {
         *pnSelStart = MIN(ti.nSelEndCaret,ti.nSelStart);
      }
      if ( pnSelEnd ) {
         *pnSelEnd = MAX(ti.nSelEndCaret,ti.nSelStart);
      }
      RELEASEIF(pitm);
   } else if (SUCCESS == IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&pivm)) {
      // this is a value model.
      pwText = (AECHAR*)IVALUEMODEL_GetText(pivm, pnLen);
      if ( pwText ) {
         if ( *pnLen < 0 ) {
            *pnLen = WSTRLEN(pwText);
         }
      }
      else {
         // no text, so make sure length is zero.
         *pnLen = 0;
      }
      RELEASEIF(pivm);
   }

   return pwText;
}

