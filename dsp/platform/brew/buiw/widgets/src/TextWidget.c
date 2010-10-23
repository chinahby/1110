/*======================================================
FILE: TextWidget.c

SERVICES: 
   IWidget implementation for text entry

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "WidgetBase.h"
#include "TextWidget.h"
#include "TextController.h"
#include "BitmapWidget.h"
#include "TextModel.h"
#include "ListWidget.h"
#include "ScrollWidget.h"
#include "BorderWidget.h"
#include "AEEFont.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"
#include "CaretWidget.h"
#include "AEEHFontOutline.h"

//#define DEBUG_WIDGETAPP    1

#define ME_FROM_WIDGET  TextWidget *me = (TextWidget*)po

#define MASK_NOALPHA 0xFFFFFF00


#define FARF_LINES      0
#define FARF_MEASURE    0
#define FARF_SCROLLPOS  0
#define FARF_UPDATE     0
#define FARF_JUMPWORD   0
#define FARF_DRAW       0
#define FARF_DRAWLINE   0
#define FARF_CARET      0


#ifndef FARF
#define FARF(x, p)  if (0 != FARF_##x) DBGPRINTF p 
#endif


#define MASKTIMEOUT  600


// forward decls
static boolean TextWidget_AdjustScrollPos(TextWidget *me, TextInfo *pInfo);


static __inline void TextWidget_Invalidate(TextWidget *me) 
{
   WidgetBase_InvalidateContent((IWidget*)me);
}

static __inline AECHAR *TextWidget_GetText(TextWidget *me, int *pnLen)
{
   ITextModel *piModel;

   if (IWIDGET_GetModel((IWidget*)me, AEEIID_TEXTMODEL, (IModel **)&piModel) == SUCCESS) {
      TextInfo textInfo;
      
      ITEXTMODEL_GetTextInfo(piModel, &textInfo);
      *pnLen = textInfo.cchText;
      ITEXTMODEL_Release(piModel);
      return (AECHAR*)textInfo.pwText;
   }
   return 0;
}

// returns the display height of all the wraped lines to for preferred width
static int TextWidget_WrapToPreferredWidth(TextWidget *me, int cxWidth)
{
   int cyHeight = 10;
   LayoutTextStyleInfo style;

   boolean bWrapNoBreak = (0 != (me->dwFlags & TWF_WRAPNOBREAK));

   if ((me->dwFlags & TWF_MULTILINE) || bWrapNoBreak) {
      int nLen = 0;
      AECHAR *pwText = TextWidget_GetText(me, &nLen);
      if (pwText && nLen) {
         me->fRewrap = 1;  // force rewrap, table no longer valid

         LayoutText_SetLineBreak(me->poLayoutText, 
               (boolean) ((me->dwFlags & TWF_MULTILINE) || (me->dwFlags & TWF_WRAPNOBREAK) ? 1 : 0),
               (boolean) ( me->dwFlags & TWF_WRAPNOBREAK ? 0 : 1 ) );

         LayoutText_DeleteText(me->poLayoutText, LAYOUTTEXT_MIN_TEXT_RANGE, LAYOUTTEXT_MAX_TEXT_RANGE);
         style.piFont = me->piFont;
         style.rgbBackground = RGB_NONE;
         style.rgbForeground = RGB_NONE;
         LayoutText_AddText   (me->poLayoutText, &style, pwText, nLen, LAYOUTTEXT_MIN_TEXT_RANGE);

         LayoutText_SetWidth     (me->poLayoutText, cxWidth);
         LayoutText_GetHeight    (me->poLayoutText, &cyHeight);
         LayoutText_GetLineCount (me->poLayoutText, &me->nLines);

      }
   }

   return cyHeight;
}

static __inline int TextWidget_CalcLineTbl(TextWidget *me)
{
   int nLen       = me->ti.cchText;
   AECHAR *pwText = (AECHAR *)me->ti.pwText;
   LayoutTextStyleInfo style;
   boolean bDrawEllipsis = FALSE;
   int ndx = (me->border.bActive != 0) + (2 * (me->border.bSelected != 0));
   RGBAVAL rgba;
   
   DynRGB_Get(&me->rgbText, ndx, &rgba);

   if (me->fRewrap) {
      TextWidget_WrapToPreferredWidth(me, me->border.rcClient.dx);
      me->fRewrap = 0;
   }

   LayoutText_SetLineBreak(me->poLayoutText, 
         (boolean) ((me->dwFlags & TWF_MULTILINE) || (me->dwFlags & TWF_WRAPNOBREAK) ? 1 : 0),
         (boolean) ( me->dwFlags & TWF_WRAPNOBREAK ? 0 : 1 ) );

   LayoutText_SetEllipsis(me->poLayoutText, bDrawEllipsis );

   LayoutText_DeleteText(me->poLayoutText, LAYOUTTEXT_MIN_TEXT_RANGE, LAYOUTTEXT_MAX_TEXT_RANGE);
   style.piFont = me->piFont;
   style.rgbBackground = RGB_NONE;
   style.rgbForeground = rgba & MASK_NOALPHA;

   LayoutText_AddText   (me->poLayoutText, &style, pwText, nLen, LAYOUTTEXT_MIN_TEXT_RANGE);

   LayoutText_SetWidth  (me->poLayoutText, me->border.rcClient.dx);

   return SUCCESS;   
}

static void TextWidget_MaskTimeout(TextWidget *me)
{
   me->nMaskChars = me->ti.cchText;
   TextWidget_Invalidate(me);
}

static void TextWidget_MTTimeout(TextWidget *me, ModelEvent *pev)
{
   if ((me->dwFlags & TWF_PASSWORD) &&
       pev->evCode == EVT_MDL_MODE_TIMEOUT) {
      TextWidget_MaskTimeout(me);
   }
}


static void TextWidget_ModelChanged(TextWidget *me, ModelEvent *pEvent)
{
   switch (pEvent->evCode) {
      
      case EVT_MDL_TEXT_CHANGE:

         ITEXTMODEL_GetTextInfo(me->piTextModel, &me->ti);

         me->nChangePos = (int)pEvent->dwParam;

         // if text changed at position 0, the entire text may have 
         // been replaced, so reset the scroll position. 
         // (the correct scroll pos will be recalculated at draw 
         // time if necessary)
         if (me->nChangePos == 0) {
            me->xScrollPos = 0;
         }

         if ( (me->dwFlags & TWF_PASSWORD) && (me->ti.cchText < me->nMaskChars) ) {
            me->nMaskChars = me->ti.cchText;
         }

         if ((me->dwFlags & TWF_MULTILINE) && (me->border.rcClient.dx > 0)) {
            me->posUpdate = MIN(me->posUpdate, (uint32)pEvent->dwParam);
            me->fRewrap = 1;
         }
         TextWidget_Invalidate(me);
         break;

      case EVT_MDL_TEXT_SELECT: {
         ITEXTMODEL_GetTextInfo(me->piTextModel, &me->ti);
         TextWidget_Invalidate(me);
         break;
      }
   }
}


static __inline void TextWidget_CalcVisibleLines(TextWidget *me)
{
   me->nVisibleLines = 0;

   if (me->cyChar > 0) {

      me->nVisibleLines = me->border.rcClient.dy / (me->cyChar + me->nLineGap);
      
      if (me->border.rcClient.dy % (me->cyChar + me->nLineGap) >= me->cyChar) {
         me->nVisibleLines++;
      }
   }

   me->nVisibleLines = MAX(me->nVisibleLines, 1);

   FARF(LINES, ("me->nVisibleLines=%d", me->nVisibleLines));
}


static void TextWidget_FontRecalc(TextWidget *me)
{
   AEEFontInfo fi;
   WExtent we;
      
   IFONT_GetInfo(me->piFont, &fi, sizeof(fi));
   me->cyChar = fi.nAscent + fi.nDescent;

   TextWidget_CalcVisibleLines(me);
   me->fRewrap = 1;
   me->posUpdate = (uint32)(-1);   // MAXINT

   // set the extent of the caret
   if (me->piCaret) {
      IWIDGET_GetPreferredExtent(me->piCaret, &we);
      we.height = me->cyChar;
      IWIDGET_SetExtent(me->piCaret, &we);
   }
}


static void TextWidget_FontChanged(TextWidget *me, ModelEvent *pEvent)
{
   IHFont *piHFont;

   if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
      // we're an IHFont and the size of our characters may have changed
      TextWidget_FontRecalc(me);
      RELEASEIF(piHFont);
   }

   TextWidget_Invalidate(me);
}


static void TextWidget_UseFont(TextWidget *me)
{
   LISTENER_Cancel(&me->mlFont);
   me->cyChar = 12;

   if (me->piFont) {
      
      // recalculate ...
      TextWidget_FontRecalc(me);

      // if font supports IModel, attach a listener for animated fonts
      {
         IModel *piModel = 0;
         if (SUCCESS == IFONT_QueryInterface(me->piFont, AEEIID_MODEL, (void**)&piModel)) {
            IMODEL_AddListenerEx(piModel, &me->mlFont, (PFNLISTENER)TextWidget_FontChanged, me);
            IMODEL_Release(piModel);
         }
      }
      TextWidget_Invalidate(me);
   }
}

static void TextWidget_SetFont(TextWidget *me, IFont *piFont)
{
   RELEASEIF(me->piFont);
   me->piFont = piFont;
   ADDREFIF(me->piFont);
   TextWidget_UseFont(me);
}

void TextWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;
   WExtent we;

   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   Border_CalcClientRect(&me->border);
   TextWidget_CalcVisibleLines(me);
   me->fRewrap = 1;
   me->posUpdate = (uint32)(-1);   // MAXINT 

   // set the extent of the caret
   if (me->piCaret) {
      IWIDGET_GetPreferredExtent(me->piCaret, &we);
      we.height = me->cyChar;
      IWIDGET_SetExtent(me->piCaret, &we);
   }
}

void TextWidget_GetExtent(IWidget *po, WExtent *pe)
{
   // Don't ask child, we control the extent
   WidgetBase_GetExtent((IWidget*)po, pe);
}

int TextWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;

   if (piModel != (IModel*)me->piTextModel) {

      RELEASEIF(me->piTextModel);
   
      if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_TEXTMODEL, (void **)&me->piTextModel)) {
         return ECLASSNOTSUPPORT;
      }

      WidgetBase_SetModel(po, piModel);
      IMODEL_AddListenerEx(piModel, &me->modelListener, (PFNLISTENER)TextWidget_ModelChanged, me);

      // force a text model change, esp. for the password logic
      {
         ModelEvent ev;
         ev.evCode   = EVT_MDL_TEXT_CHANGE;
         ev.dwParam  = 0;
         TextWidget_ModelChanged(me, &ev);
      }

      // update the textinfo from the textmodel
      ITEXTMODEL_GetTextInfo(me->piTextModel, &me->ti);

      // invalidate ourself to our container
      TextWidget_Invalidate(me);
   }

   return SUCCESS;
}

static int MemEnsure(void **ppMem, int *pcb, int cbEnsureSize)
{
   int nErr = 0;
   int cb = *pcb;

   if (cb < cbEnsureSize) {
      nErr = MemGrow(ppMem, pcb, cb, cbEnsureSize - cb);
   }

   return nErr;
}

static void TextWidget_PasswordFix(TextWidget *me, TextInfo *pInfo)
{
   if (me->dwFlags & TWF_PASSWORD) {
      if (SUCCESS == MemEnsure((void**)&me->pwMaskBuf, &me->cbMaskBuf, 
                               (pInfo->cchText+1) * sizeof(AECHAR))) {
         int i;
         AECHAR wchChar = (pInfo->cchText == me->nMaskChars) ? me->wchMask : pInfo->pwText[me->nChangePos];
         for (i=0; i < pInfo->cchText; i++) {
            me->pwMaskBuf[i] = (i == me->nChangePos) ? wchChar : me->wchMask;
         }
      } else {
         // realloc failed, don't let cchText exceed sizeof buffer
         pInfo->cchText = MIN(pInfo->cchText, (int)(me->cbMaskBuf / sizeof(AECHAR)));
         pInfo->nSelStart = MIN(pInfo->nSelStart, pInfo->cchText);
         pInfo->nSelEndCaret = MIN(pInfo->nSelEndCaret, pInfo->cchText);
      }
      pInfo->pwText = me->pwMaskBuf;
   }
}

static
boolean TextWidget_AdjustScrollPos(TextWidget *me, TextInfo *pInfo)
{
   int nCaret     = pInfo->nSelEndCaret;
   int nMaxLine   = MIN(me->nLines, me->nScrollLine + me->nVisibleLines);
   int nSaveScrollLine = me->nScrollLine;
   int nErr;

   int posMax = 0;     // maximum position in current view
   int posMin = 0;     // minimum position in current view

   if ( me->dwFlags & TWF_MULTILINE ) {
      nErr = LayoutText_GetLineStartIndex(me->poLayoutText, (uint16)(nMaxLine-1), &posMax);

      if (SUCCESS == nErr) {
         nErr = LayoutText_GetLineStartIndex(me->poLayoutText, (uint16)me->nScrollLine, &posMin);
      }

      // out-of-range case...
      if (nErr || (uint16)nCaret < posMin 
               || (uint16)nCaret >= posMax) {
         LayoutText_GetLineFromIndex(me->poLayoutText, nCaret, &me->nScrollLine);
         if ((uint16)nCaret >= posMax) {
            me->nScrollLine = MAX(0, me->nScrollLine - (me->nVisibleLines-1));
         }
      }

      FARF(UPDATE, ("nScrollLine=%d, nLines=%d, nVisibleLines=%d",
                    me->nScrollLine, me->nLines, me->nVisibleLines));

      return nSaveScrollLine != me->nScrollLine;
   }
   else {
      // single line
      // does caret show in the viewport?
      int xSaveScrollPos = me->xScrollPos;
      LayoutText_GetCaretScroll(me->poLayoutText, nCaret, xSaveScrollPos, &me->xScrollPos); 
      return FALSE;//nSaveScrollPos != me->nScrollPos;
   }

}


static 
void TextWidget_NotifyScrollEvent(TextWidget *me)
{
   if (me->base.piViewModel) {
      ScrollEvent se;
      se.base.evCode    = EVT_MDL_SCROLL_CHANGE;
      se.range          = me->nLines;
      se.visible        = me->nVisibleLines;
      se.position       = me->nScrollLine;
      IMODEL_Notify(me->base.piViewModel, SCROLLEVENT_TO_MODELEVENT(&se));
   }
}


static 
void TextWidget_NotifyCursorLineEvent(TextWidget *me)
{
   if (me->base.piViewModel) {

      int      nCaretLine;

      // send EVT_MDL_CARETLINE_CHANGE notification if cursor line changed
      LayoutText_GetLineFromIndex(me->poLayoutText, me->ti.nSelEndCaret, &nCaretLine);

      if (nCaretLine != me->nCurrentLine) {
         int nLines;
         ModelEvent ev;

         me->nCurrentLine = nCaretLine;

         LayoutText_GetLineCount(me->poLayoutText, &nLines);

         ev.evCode   = EVT_MDL_CARETLINE_CHANGE;
         ev.dwParam  = MAKELONG(nLines, me->nCurrentLine);
         IMODEL_Notify(me->base.piViewModel, &ev);
      }
   }
}

static
boolean TextWidget_GetTextRects(TextWidget* me, WidgetTextRects* wtr) {

   int nErr;
   boolean bScrolled = FALSE;
   uint32   nCount = 0;
   uint32   nAdjust;
   if ( 0 == wtr->flags && wtr->end >= wtr->start  ) {

      AEERect  rcChar;
      AEERect  rcIntersect;
      AEERect  rcLast;

      AEERect* pRects = wtr->pRects;
      uint32 nStart = MIN(wtr->start, wtr->end);
      uint32 nEnd   = MAX(wtr->end,   wtr->start);
      uint32 nIndex = nStart;

      SETAEERECT(&rcLast, -1,0,0,0);
      // adjust the scroll if needed 
      // (just like TextWidget_Draw does).
      bScrolled |= TextWidget_AdjustScrollPos(me, &me->ti);

      for ( ; nIndex <= nEnd; ++nIndex ) {
         nErr = LayoutText_GetCharacterLocation(me->poLayoutText, nIndex, &rcChar);
         if ( nErr != SUCCESS ) {
            continue;//break;
         }
         rcChar.dy = me->cyChar;

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

      // adjust for scroll and border position
      pRects = wtr->pRects;
      for ( nAdjust = 0; nAdjust < wtr->count && nAdjust < nCount; ++nAdjust) {
         pRects->x += me->border.rcClient.x;
         pRects->y += me->border.rcClient.y;

         if ( me->dwFlags & TWF_MULTILINE ) {
            // multi line scrolling
            pRects->y -= me->nScrollLine * (me->cyChar + me->nLineGap);
         }
         else {
            // single line scrolling
            pRects->x -= me->xScrollPos;
         }

         ++pRects;
      }
   }
   wtr->count = nCount;

   if (bScrolled) {
      CALLBACK_Resume(&me->cbkNotify, TextWidget_NotifyScrollEvent, me, me->piShell);
   }

   TextWidget_NotifyCursorLineEvent(me);
   return TRUE;
}


void TextWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   AEERect rc;
   AEERect rcClip;
   IDisplay *piDisplay = 0;
   int nLine, nDrawX, nDrawY;
   boolean bScrolled = FALSE;
   RGBVAL rgbTextFG;


   me->xDraw = x;
   me->yDraw = y;

   Border_Draw(&me->border, piCanvas, x, y);

   FARF(DRAW, ("Draw"));

   rc = me->border.rcClient;
   rc.x += x;
   rc.y += y;

   nDrawX = rc.x;
   nDrawY = rc.y;
    
   DynRGB_Get(&me->rgbText, (me->border.bActive) ? RGBINDEX_ACTIVE : RGBINDEX_INACTIVE, &rgbTextFG);


   ICANVAS_GetClipRect(piCanvas, &rcClip);
   if (IntersectRect(&rcClip, &rcClip, &rc) 
         && me->piTextModel
         && SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      // if we actually have text to draw...
      if (me->ti.pwText) {

         boolean bEllipsis;
        
         TextWidget_PasswordFix(me, &me->ti);

         TextWidget_CalcLineTbl(me);


         if ( !me->border.bActive && me->dwFlags & TWF_SHORTENTEXT ) {
            bEllipsis = TRUE;
         } 
         else {
            bEllipsis = FALSE;
            bScrolled = TextWidget_AdjustScrollPos(me, &me->ti);
            nLine = me->nScrollLine;


            if ( me->dwFlags & TWF_MULTILINE ) {
               // multi line scrolling
               nDrawY -= me->nScrollLine * (me->cyChar + me->nLineGap);
            }
            else {
               // single line scrolling
               nDrawX -= me->xScrollPos;
            }
         }

         
         LayoutText_SetEllipsis(me->poLayoutText, bEllipsis);


         LayoutText_Draw(me->poLayoutText, piDisplay, nDrawX, nDrawY, &rcClip, IDF_TEXT_TRANSPARENT, &rc, rgbTextFG);
         if ( me->ti.nSelEndCaret != me->ti.nSelStart ) {
            // highlight the selected text by redrawing it.
            WidgetTextRects wtr;
            wtr.count  = me->nSelRect;
            wtr.flags  = 0;
            wtr.start  = me->ti.nSelStart;
            wtr.end    = (me->ti.nSelEndCaret) ? (me->ti.nSelEndCaret-1) : 0;
            wtr.pRects = me->paSelRect;
            TextWidget_GetTextRects(me, &wtr);
            if ( wtr.count > (uint32) me->nSelRect ) {
               FREEIF(me->paSelRect);
               me->nSelRect  = wtr.count;
               me->paSelRect = MALLOC(sizeof(AEERect)*wtr.count);
               wtr.count  = me->nSelRect;
               wtr.pRects = me->paSelRect;
               TextWidget_GetTextRects(me, &wtr);
            }
            while ( wtr.count-- ) {
               AEERect* prcSelected = me->paSelRect + wtr.count;
               RGBVAL rgbSelectFG;
               int ndxFG = (me->border.bActive) ? RGBINDEX_SACTIVE : RGBINDEX_SINACTIVE;
               int ndxBG = (me->border.bActive) ? 1 : 0;
               prcSelected->x += x;
               prcSelected->y += y;
               BlendRect(piDisplay, prcSelected, me->rgbaSelected[ndxBG], RGBA_GETALPHA(me->rgbaSelected[ndxBG]));
      
               DynRGB_Get(&me->rgbText, ndxFG, &rgbSelectFG);
               LayoutText_Draw(me->poLayoutText, piDisplay, nDrawX, nDrawY, prcSelected, IDF_TEXT_TRANSPARENT, &rc, rgbSelectFG);

            }
         }
      }
   }
   RELEASEIF(piDisplay);

   // only draw caret if active and selection span is zero
   if (me->piCaret && me->border.bActive && me->ti.nSelStart == me->ti.nSelEndCaret) {
      if ( LayoutText_GetCaretInfo(me->poLayoutText, me->ti.nSelStart, &me->xCaret, &me->yCaret) == SUCCESS ) {
         me->xCaret += nDrawX - 3;
         me->yCaret += nDrawY;
         IWIDGET_Draw(me->piCaret, piCanvas, me->xCaret, me->yCaret);
      }
   }
         
   if (bScrolled) {
      CALLBACK_Resume(&me->cbkNotify, TextWidget_NotifyScrollEvent, me, me->piShell);
   }
   
   TextWidget_NotifyCursorLineEvent(me);

}


boolean TextWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}


void TextWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;

   WExtent weClient;

   int cx = me->cxHintWidth ? me->cxHintWidth : me->border.rcClient.dx;

   if (me->dwFlags & TWF_MULTILINE) {

      int height = MAX(0,me->nHintRows * (me->cyChar + me->nLineGap) - me->nLineGap);

      if (!height) {
         height = TextWidget_WrapToPreferredWidth(me, cx);
      }

      height = MAX(me->cyChar, height);

      weClient.height = height;
      weClient.width  = cx;
   
   } else {

      SETWEXTENT(&weClient, cx, me->cyChar);

      if (me->piFont && me->piTextModel) {
   
         // get text extent
         if (me->ti.pwText && me->ti.cchText) {
            int fits;
            IFONT_MeasureText(me->piFont, me->ti.pwText, me->ti.cchText, 
                              200, &fits, &weClient.width);

            if (me->piCaret) {  
               WExtent we;
               IWIDGET_GetPreferredExtent(me->piCaret, &we);
               weClient.width += we.width / 2;
            }
         }
      }
   }
   
   Border_CalcPreferredExtent(&me->border, pweOut, &weClient);
}


boolean TextWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   // PROP_TEXTCONTROLLER must be handled before forwarding the event
   // to the current IController.
   switch(evt) {
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_TEXTCONTROLLER) {
            *(IController **) dwParam = me->piController;
            ADDREFIF(me->piController);
            return TRUE;
         }
         if (wParam == PROP_CARETWIDGET) {
            *(IWidget **) dwParam = me->piCaret;
            ADDREFIF(me->piCaret);
            return TRUE;
         }
         break;
      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_TEXTCONTROLLER) {
            if (me->piController) {
               ICONTROLLER_SetWidget(me->piController, NULL);
            }
            RELEASEIF(me->piController);

            me->piController = (IController *) dwParam;

            ADDREFIF(me->piController);

            if (me->piController) {
               ICONTROLLER_SetWidget(me->piController, po);
            }
            return TRUE;
         }
         if (wParam == PROP_CARETWIDGET) {
            RELEASEIF(me->piCaret);
            me->piCaret = (IWidget *) dwParam;
            ADDREFIF(me->piCaret);
            if (me->piCaret) {
               WExtent we;
               IWIDGET_GetPreferredExtent(me->piCaret, &we);
               we.height = me->cyChar;
               IWIDGET_SetExtent(me->piCaret, &we);
            }
            TextWidget_Invalidate(me);
            return TRUE;
         }
         break;
      default:
         break;
   }



   
   // allow the installed controller to handle the event first...
   if (me->piController && ICONTROLLER_HandleEvent(me->piController, evt, wParam, dwParam)) {
      return TRUE;
   }
   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch(evt) {
   
      ////////////////////////////// 
      case EVT_WDG_SETPROPERTY:
         
         if (wParam == PROP_FGCOLOR) {
            DynRGB_Collapse(&me->rgbText, (RGBVAL)dwParam);
   
         } else if (wParam == PROP_TEXT_SELECTED_BGCOLOR) {
            me->rgbaSelected[0]              =
            me->rgbaSelected[1]              = (RGBVAL)dwParam;

         } else if (wParam == PROP_TEXT_SINACTIVE_BGCOLOR) {
            me->rgbaSelected[0]              = (RGBVAL)dwParam;

         } else if (wParam == PROP_TEXT_SACTIVE_BGCOLOR) {
            me->rgbaSelected[1]              = (RGBVAL)dwParam;

         } else if (wParam == PROP_SELECTED_FGCOLOR) {
            DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
            DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);

         } else if (wParam == PROP_SACTIVE_FGCOLOR) {
            DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
         
         } else if (wParam == PROP_SINACTIVE_FGCOLOR) {
            DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
         
         } else if (wParam == PROP_ACTIVE_FGCOLOR) {
            DynRGB_Set(&me->rgbText, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
         
         } else if (wParam == PROP_INACTIVE_FGCOLOR) {
            DynRGB_Set(&me->rgbText, RGBINDEX_INACTIVE, (RGBVAL)dwParam);

         } else if (wParam == PROP_FONT) {
            TextWidget_SetFont(me, (IFont*)dwParam);

         } else if (wParam == PROP_FONT_CLASS) {
            IFont *piFont;
            if (SUCCESS == ISHELL_CreateFont(me->piShell, (AEECLSID)dwParam, &piFont)) {
               TextWidget_SetFont(me, piFont);
               IFONT_Release(piFont);
            }

         } else if (wParam == PROP_FONT_OUTLINECOLOR) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               IHFONT_SetFontOutlineColor(piHFont, (RGBVAL)dwParam);
               RELEASEIF(piHFont);
               TextWidget_UseFont(me);
            } else {
               break;
            }

         } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               IHFONT_SetFontOutlineWidth(piHFont, dwParam);
               RELEASEIF(piHFont);
               TextWidget_UseFont(me);
            } else {
               break;
            }

         } else if (wParam == PROP_FLAGS) {
            if ((me->dwFlags ^ dwParam) & TWF_PASSWORD) {
               me->nMaskChars = me->ti.cchText;
            }
            me->dwFlags = dwParam;

         } else if (wParam == PROP_HINT_ROWS) {
            me->nHintRows = dwParam;
         
         } else if (wParam == PROP_HINT_WIDTH) {
            me->cxHintWidth = dwParam;
         
         } else if (wParam == PROP_PASSWORDMASKCHAR) {
            me->wchMask = (AECHAR)dwParam;

         } else if (wParam == PROP_PASSWORDMASKDELAY) {
            me->nMaskTimeout = (int16)dwParam;

         } else if (wParam == PROP_LINEGAP) {
            me->nLineGap = dwParam;
            LayoutText_SetLineGap(me->poLayoutText, me->nLineGap);         
            TextWidget_UseFont(me);

         } else {
            break;
         }
         TextWidget_Invalidate(me);
         return TRUE;
   
      
      ////////////////////////////// 
      case EVT_WDG_GETPROPERTY:
         
         if (wParam == PROP_FGCOLOR || wParam == PROP_ACTIVE_FGCOLOR) {
            DynRGB_Get(&me->rgbText, RGBINDEX_ACTIVE, (uint32*)dwParam);
         
         } else if (wParam == PROP_TEXT_SELECTED_BGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbaSelected[1];

         } else if (wParam == PROP_TEXT_SINACTIVE_BGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbaSelected[0];

         } else if (wParam == PROP_TEXT_SACTIVE_BGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbaSelected[1];

         } else if (wParam == PROP_SELECTED_FGCOLOR || wParam == PROP_SACTIVE_FGCOLOR) {
            DynRGB_Get(&me->rgbText, RGBINDEX_SACTIVE, (uint32*)dwParam);

         } else if (wParam == PROP_SINACTIVE_FGCOLOR) {
            DynRGB_Get(&me->rgbText, RGBINDEX_SINACTIVE, (uint32*)dwParam);
         
         } else if (wParam == PROP_INACTIVE_FGCOLOR) {
            DynRGB_Get(&me->rgbText, RGBINDEX_INACTIVE, (uint32*)dwParam);

         } else if (wParam == PROP_FONT) {
            *((IFont**)dwParam) = me->piFont;
            ADDREFIF(me->piFont);
         
         } else if (wParam == PROP_FONT_OUTLINECOLOR) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               IHFONT_GetFontOutlineColor(piHFont, (RGBVAL*)dwParam);
               RELEASEIF(piHFont);
            } else {
               break;
            }

         } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont))  {
               IHFONT_GetFontOutlineWidth(piHFont, (int*)dwParam);
               RELEASEIF(piHFont);
            } else {
               break;
            }

         } else if (wParam == PROP_FLAGS) {
            *((uint32*)dwParam) = me->dwFlags;

         } else if (wParam == PROP_ROWS) {
            *((uint32*)dwParam) = me->nVisibleLines;

         } else if (wParam == PROP_TEXTRECTS) {
            return TextWidget_GetTextRects(me, (WidgetTextRects*) dwParam);

         } else if (wParam == PROP_VIEWMODEL) {
            *(IModel**)dwParam = me->base.piViewModel;
            IMODEL_AddRef(me->base.piViewModel);
         
         } else if (wParam == PROP_CARETPOS) {
            *((uint32*)dwParam) = ( ((uint32)me->yCaret << 16) | (uint32)me->xCaret );

         } else if (wParam == PROP_PASSWORDMASKCHAR) {
            *((uint32*)dwParam) = (uint32)me->wchMask;

         } else if (wParam == PROP_PASSWORDMASKDELAY) {
            *((uint32*)dwParam) = (uint32)me->nMaskTimeout;

         } else if (wParam == PROP_LINEGAP) {
            *((uint32*)dwParam) = (uint32)me->nLineGap;

         } else if (wParam == PROP_STARTLINE ||
                    wParam == PROP_PREVLINE  ||
                    wParam == PROP_NEXTLINE) {
            
            int pos;
            int line;

            if (!(me->dwFlags & TWF_MULTILINE)) {
               break;
            }

            LayoutText_GetLineFromIndex(me->poLayoutText, *((int*)dwParam), &line);
            if (wParam == PROP_PREVLINE) {
               line--;
            } else if (wParam == PROP_NEXTLINE) {
               line++;
            }
            if (SUCCESS == LayoutText_GetLineStartIndex(me->poLayoutText, line, &pos)) {
               *((int*)dwParam) = pos;
            } else {
               return FALSE;
            }

         } else {
            break;
         }
         return TRUE;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}

void TextWidget_Dtor(TextWidget *me)
{
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Cancel(&me->mlMT);
   CALLBACK_Cancel(&me->cbkUpdate);
   CALLBACK_Cancel(&me->cbkNotify);
   CALLBACK_Cancel(&me->cbkMask);

   // textwidget specific cleanup
   LISTENER_Cancel(&me->mlFont);
   RELEASEIF(me->piFont);
   RELEASEIF(me->piTextModel);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piController);
   RELEASEIF(me->piCaret);

   FREEIF(me->pwMaskBuf);
   FREEIF(me->paSelRect);

   LayoutText_Delete(me->poLayoutText);

   Border_Dtor(&me->border);
   DynRGB_Dtor(&me->rgbText);
}

uint32 TextWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      TextWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}

void TextWidget_Ctor(TextWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(TextWidget_HandleEvent));

   pvt->Release            = TextWidget_Release;
   pvt->GetPreferredExtent = TextWidget_GetPreferredExtent;
   pvt->IntersectOpaque    = TextWidget_IntersectOpaque;
   pvt->Draw               = TextWidget_Draw;
   pvt->SetModel           = TextWidget_SetModel;
   pvt->SetExtent          = TextWidget_SetExtent;
   pvt->GetExtent          = TextWidget_GetExtent;
      
   me->piShell = piShell;
   ISHELL_AddRef(piShell);
   
   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, TRUE, &WBASE(me)->piViewModel);

   // setup the border & background properties
   me->border.padding.left     = 3;
   me->border.padding.right    = 2;
   me->border.padding.top      = 1;
   me->border.padding.bottom   = 1;

   Border_SetBGColor(&me->border, 0, RGBA_WHITE);

   me->cyChar      = 12;
   me->nLineGap    = 0;
   me->dwFlags     = 0;   
   me->xCaret      = 1;
   me->yCaret      = 4;

   DynRGB_Ctor(&me->rgbText, RGB_BLACK);
   DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, RGB_WHITE);
   DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, RGB_WHITE);

   me->rgbaSelected[0] = 
   me->rgbaSelected[1] = MAKE_RGBA(0,0,128,255);

   LayoutText_New(&me->poLayoutText, piShell);
   me->nLines = 1;

   me->nCurrentLine = -1;  // force EVT_MDL_CARETLINE_CHANGE to be sent initially

   me->wchMask = (AECHAR)'*';
   me->nMaskTimeout = MASKTIMEOUT;
}


int TextWidget_Initialize(TextWidget *me)
{
   int nErr = 0;
   ITextModel *piTextModel = 0;
   IModel *piViewModel = 0;

   nErr = ISHELL_CreateFont(me->piShell, AEECLSID_FONTSYSNORMAL, &me->piFont);

   if (!nErr) {
      nErr = TextModel_New(&piTextModel, me->base.piModule);
   }
   if (!nErr) {
      nErr = WidgetBase_GetViewModel(&me->base, &piViewModel);
   }
   if (!nErr) {
      IMODEL_AddListenerEx(piViewModel, &me->mlMT, (PFNLISTENER)TextWidget_MTTimeout, me);
   }
   if (!nErr) {
      nErr = TextWidget_SetModel((IWidget*)me, (IModel*)piTextModel);
   }

   // initialize the text model, so we at least have 1 null terminator.  This ensures caret drawing.   
   if (!nErr) {      
      
      // be sure to do this *after* we have set the text model (need the EVT_MDL_TEXT_CHANGE)
      nErr = ITEXTMODEL_ReplaceSel(piTextModel, NULL, 0);      
   }


   if (!nErr) {

      // try to instantiate the OEM caret
      if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_CARETWIDGET, 
                                           (void**)&me->piCaret)) {
         // otherwise, use the default one
         nErr = CaretWidget_New(&me->piCaret, me->piShell, me->base.piModule);
      }
   }

   if (!nErr) {

      TextWidget_UseFont(me);

      // create the TextController
      // try to instantiate the OEM text controller
      if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_TEXTCONTROLLER, 
                                           (void**)&me->piController)) {
         // otherwise, use the default one
         nErr = TextController_New(&me->piController, me->piShell, me->base.piModule);
      }
   }
   
   if (!nErr) {
      WExtent we;
      we.width  = 40;
      we.height = 16;
      TextWidget_SetExtent((IWidget*)&me->base, &we);

      ICONTROLLER_SetWidget(me->piController, (IWidget*)me);
   }

   RELEASEIF(piTextModel);
   RELEASEIF(piViewModel);

   return nErr;
}


int TextWidget_New(IWidget **ppo, ITextModel *piModel, IShell *piShell, IModule *piModule)
{
   int nErr = 0;
   TextWidget *me = MALLOCREC_VTBL(TextWidget, IWidget);
   
   *ppo = 0;
   
   if (!me) {
      return ENOMEMORY;
   }

   TextWidget_Ctor(me, GETVTBL(me,IWidget), piShell, piModule, 0);
      
   nErr = TextWidget_Initialize(me);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      TextWidget_Dtor(me);
      FREE(me);
   }

   return nErr;
}
