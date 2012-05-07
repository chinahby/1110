/*======================================================
FILE: TextWidget.c

SERVICES: 
   IWidget implementation for text entry

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "TextController.h"
#include "BitmapWidget.h"
#include "TextModel.h"
#include "AEEFont.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"
#include "CaretWidget.h"
#include "AEEHFontOutline.h"
#include "AEEFontMapModel.h"
#include "AEETextLayout.h"
#include "TextLayout.h"
#include "TextWidget.h"

#include "bid/AEECLSID_TEXTLAYOUT.BID"


//#define DEBUG_WIDGETAPP    1

#define ME_FROM_CONTAINER  TextWidget *me = (TextWidget*)po->pMe
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
static void TextWidget_AdjustScrollPos(TextWidget *me, TextInfo *pInfo);
static void TextWidget_NotifyEventCallback(TextWidget *me);


static __inline void TextWidget_Invalidate(TextWidget *me) 
{
   WidgetBase_InvalidateContent((IWidget*)me);
}

static __inline void TextWidget_SetLayoutFlags  (TextWidget* me) {
   uint32 dwLayoutFlags = 0;
 
   if ( me->dwFlags & TWF_MULTILINE ){
      if ( me->dwFlags & TWF_WRAPNOBREAK ) {
         dwLayoutFlags = TEXTLAYOUT_FLAG_MULTILINE;
      } 
      else {
         dwLayoutFlags = TEXTLAYOUT_FLAG_MULTILINE| TEXTLAYOUT_FLAG_BREAKONWORD;
      }
   }

   if ( me->dwFlags & TWF_SHORTENTEXT && FALSE == me->border.bActive ) {
      dwLayoutFlags |= TEXTLAYOUT_FLAG_ELLIPSIFY;
   }

   if ( dwLayoutFlags != me->dwLayoutFlags ) {
      me->dwLayoutFlags = dwLayoutFlags;
      me->fLayoutText   = TRUE;
   }
}

static __inline void TextWidget_SetLayoutColor(TextWidget* me, RGBAVAL rgbaFG,  RGBAVAL rgbaSelFG, RGBAVAL rgbaSelBG ) {
   if ( rgbaFG != me->rgbaLayoutFG ) {
      me->rgbaLayoutFG = rgbaFG;
      me->fLayoutText  = TRUE;
   }
   if ( rgbaSelBG != me->rgbaLayoutSelBG ) {
      me->rgbaLayoutSelBG = rgbaSelBG;
      me->fLayoutText  = TRUE;
   }
   if ( rgbaSelFG != me->rgbaLayoutSelFG ) {
      me->rgbaLayoutSelFG = rgbaSelFG;
      me->fLayoutText  = TRUE;
   }
}

static __inline AECHAR TextWidget_GetEllipsisChar(TextWidget *me) {
   AECHAR awEllipsis = me->awEllipsis;
   if (0 == awEllipsis) {
      // return the character we'll be using as the ellipsis
      awEllipsis = IShell_GetEllipsis(me->piShell);
   }
   return awEllipsis;
}

static void TextWidget_InitScrollEvent(TextWidget* me, ScrollEvent* pse) {

   ZEROAT(pse);
   pse->base.evCode    = EVT_MDL_SCROLL_CHANGE;
   pse->range          = (uint16) me->nLines;
   pse->visible        = (uint16) me->nVisibleLines;
   pse->position       = (uint16) me->nScrollLine;
   pse->bVertical      = TRUE;
}

static void TextWidget_InitCaretLineParam(TextWidget* me, uint32* pdwParam) {
   if ( pdwParam ) {
      int nCaretLine;
      (void) ITEXTLAYOUT_GetLineFromIndex(me->piTextLayout, me->ti.nSelEndCaret, &nCaretLine);
      *pdwParam = MAKELONG(me->nLines, nCaretLine);
   }
}

static __inline void TextWidget_QueueCaretScrollEvent(TextWidget* me) {

   boolean bCallback = FALSE;
   uint32 dwCaretLineParam;

   if ( me->dwFlags & TWF_MULTILINE ) {
      ScrollEvent se;
      TextWidget_InitScrollEvent(me, &se);
      if ( MEMCMP(&se, &me->se, sizeof(ScrollEvent)) != 0 ) {
         bCallback = TRUE;
      }
   }

   TextWidget_InitCaretLineParam(me, &dwCaretLineParam);
   if (me->dwCaretLineParam != dwCaretLineParam) {
      bCallback = TRUE;
   }

   if ( bCallback ) {
      CALLBACK_Resume(&me->cbkNotify, TextWidget_NotifyEventCallback, me, me->piShell);
   }
}

static void TextWidget_NotifyEventCallback(TextWidget *me)
{
   if (me->base.base.piViewModel) {

      // send EVT_MDL_SCROLL_CHANGE notification if scroll has changed
      ScrollEvent se;
      uint32      dwParam;
      TextWidget_InitScrollEvent(me, &se);
      if ( MEMCMP(&me->se ,&se, sizeof(ScrollEvent)) ) {
         me->se = se;
         IMODEL_Notify(me->base.base.piViewModel, SCROLLEVENT_TO_MODELEVENT(&se));
      }

      // send EVT_MDL_CARETLINE_CHANGE notification if cursor line changed
      TextWidget_InitCaretLineParam(me, &dwParam);
      if ( dwParam != me->dwCaretLineParam  ) {
         ModelEvent ev;
         me->dwCaretLineParam = dwParam;
         ev.evCode   = EVT_MDL_CARETLINE_CHANGE;
         ev.dwParam  = dwParam;
         IMODEL_Notify(me->base.base.piViewModel, &ev);
      }
   }
}

static __inline void TextWidget_LayoutText(TextWidget* me, IModel* piModel) {

   if ( me->fLayoutText ) {
      WExtent weLayout;
      WExtent we;

      // determine size of area to lay out text into
      weLayout.height = me->border.rcClient.dy;
      weLayout.width  = me->border.rcClient.dx;

      ITEXTLAYOUT_SetFGColor(me->piTextLayout, me->rgbaLayoutFG);
      ITEXTLAYOUT_SetSelectedFGColor(me->piTextLayout, me->rgbaLayoutSelFG);
      ITEXTLAYOUT_SetSelectedBGColor(me->piTextLayout, me->rgbaLayoutSelBG);
      ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_FONTMAPMODEL, (uint32) me->piFontMapModel);
      (void) ITEXTLAYOUT_SetEllipsis(me->piTextLayout, TextWidget_GetEllipsisChar(me));

      ITEXTLAYOUT_Layout(me->piTextLayout, piModel, &weLayout, me->dwLayoutFlags | IDF_TEXT_TRANSPARENT );

      (void) ITEXTLAYOUT_GetLayoutSize(me->piTextLayout, &we, &me->nLines);

      me->fLayoutText = FALSE;
   }
}

static void TextWidget_MaskTimeout(TextWidget *me)
{
   me->nMaskChars = me->ti.cchText;
   TextWidget_Invalidate(me);
}

static void TextWidget_TextViewModelListener(TextWidget *me, ModelEvent *pEvent)
{
   if ((me->dwFlags & TWF_PASSWORD) &&
      pEvent->evCode == EVT_MDL_MODE_TIMEOUT) {
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
            me->yScrollPos = 0;
         }

         if ( (me->dwFlags & TWF_PASSWORD) && (me->ti.cchText < me->nMaskChars) ) {
            me->nMaskChars = me->ti.cchText;
         }

         if ((me->dwFlags & TWF_MULTILINE) && (me->border.rcClient.dx > 0)) {
            me->posUpdate = MIN(me->posUpdate, (uint32)pEvent->dwParam);
            me->fRewrap = 1;
         }

         me->fLayoutText = TRUE;
         TextWidget_Invalidate(me);
         break;

      case EVT_MDL_TEXT_SELECT: {
         ITEXTMODEL_GetTextInfo(me->piTextModel, &me->ti);

         me->fLayoutText = TRUE;

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

   me->yScrollPos  = 0;
   me->fLayoutText = TRUE;

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
      
   // recalculate ...
   TextWidget_FontRecalc(me);

   if (me->piFont) {
      // if font supports IModel, attach a listener for animated fonts
      {
         IModel *piModel = 0;
         if (SUCCESS == IFONT_QueryInterface(me->piFont, AEEIID_MODEL, (void**)&piModel)) {
            IMODEL_AddListenerEx(piModel, &me->mlFont, (PFNLISTENER)TextWidget_FontChanged, me);
            IMODEL_Release(piModel);
         }
      }
   }

   TextWidget_Invalidate(me);

   ITEXTLAYOUT_SetFont(me->piTextLayout, me->piFont);
}


static void TextWidget_FontMapModelChanged(TextWidget *me, ModelEvent *pEvent)
{
   PARAM_NOT_REF(pEvent)  
   me->fLayoutText = TRUE;
   TextWidget_Invalidate(me);
}


static void TextWidget_UseFontMapModel(TextWidget *me)
{
   LISTENER_Cancel(&me->mlFontMapModel);
   if (me->piFontMapModel) {
      IMODEL_AddListenerEx((IModel *)me->piFontMapModel, &me->mlFontMapModel, (PFNLISTENER)TextWidget_FontMapModelChanged, me);
   }
   TextWidget_FontMapModelChanged(me, NULL);
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

   WidgetBase_SetExtent((IWidget*)WBASE(me), pe);
   Border_CalcClientRect(&me->border);

   TextWidget_CalcVisibleLines(me);

   me->posUpdate = (uint32)(-1);   // MAXINT 

   // set the extent of the caret
   if (me->piCaret) {
      IWIDGET_GetPreferredExtent(me->piCaret, &we);
      we.height = me->cyChar;
      IWIDGET_SetExtent(me->piCaret, &we);
   }

   me->fLayoutText = TRUE;
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

      me->fLayoutText = TRUE;
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

static IModel* TextWidget_PasswordFix(TextWidget *me, TextInfo *pInfo)
{
   IModel* piModel = 0;
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

      // now load this info into the password model
      if ( 0 == me->piPasswordModel ) {
         (void) TextModel_New(&me->piPasswordModel, WBASE(me)->piModule);
      }
      if ( me->piPasswordModel ) {
         ITEXTMODEL_SetSel(me->piPasswordModel, 0, -1);
         ITEXTMODEL_ReplaceSel(me->piPasswordModel, pInfo->pwText, pInfo->cchText);
         ITEXTMODEL_SetSel(me->piPasswordModel, pInfo->nSelStart,pInfo->nSelEndCaret);
      }
      piModel = (IModel*) me->piPasswordModel;
      ADDREFIF(piModel);

   }

   return piModel;
}


static void TextWidget_AdjustScrollPos(TextWidget *me, TextInfo *pInfo)
{
   int nCaret     = pInfo->nSelEndCaret;
   int nMaxLine   = 0;
   int nErr;

   WExtent weLayout;
   int     nLineCount;
   int     nMargin = 10;


   int posMax = 0;     // maximum position in current view
   int posMin = 0;     // minimum position in current view
   
   if ( me->nScrollLine + me->nVisibleLines > me->nLines ) {
      me->nScrollLine = MAX(0, me->nLines - me->nVisibleLines );
   }
   nMaxLine   = MIN(me->nLines, me->nScrollLine + me->nVisibleLines);


   if ( me->dwFlags & TWF_MULTILINE ) {
      nErr = ITEXTLAYOUT_GetLineStartIndex(me->piTextLayout, (uint16)(nMaxLine-1), &posMax);

      if (SUCCESS == nErr) {
         nErr = ITEXTLAYOUT_GetLineStartIndex(me->piTextLayout, (uint16)(me->nScrollLine), &posMin);
      }

      // out-of-range case...
      if (nErr || (uint16)nCaret < posMin 
               || (uint16)nCaret >= posMax) {
         ITEXTLAYOUT_GetLineFromIndex(me->piTextLayout, nCaret, &me->nScrollLine);
         if ((uint16)nCaret >= posMax) {
            me->nScrollLine = MAX(0, me->nScrollLine - (me->nVisibleLines-1));
         }
      }

      nErr = ITEXTLAYOUT_GetLayoutSize(me->piTextLayout, &weLayout, &nLineCount);
   
      if ( me->piFontMapModel ) {
         // if there's a fontmapmodel, then the lines may have variable height,
         // so use the caret to scroll the y coordinate.
         if ( nLineCount > 1 && weLayout.height > me->border.rcClient.dy ) {

            AEERect rcCaret;
            if ( ITEXTLAYOUT_GetCaretInfo(me->piTextLayout, nCaret, &rcCaret) == SUCCESS ) {

               if ( rcCaret.y + rcCaret.dy > me->yScrollPos + me->border.rcClient.dy - nMargin ) {
                  me->yScrollPos = MIN( rcCaret.y + rcCaret.dy - me->border.rcClient.dy + nMargin, 
                                        weLayout.height - me->border.rcClient.dy );
               } 
               else if ( rcCaret.y < me->yScrollPos + nMargin ) {
                  me->yScrollPos = MAX(0, rcCaret.y - nMargin);
               }
            
               me->yScrollPos = MIN(me->yScrollPos,weLayout.height - me->border.rcClient.dy);
            }

         
            else {
               me->yScrollPos = 0;
            }
         }
         else {
            me->yScrollPos = 0;
         }
      }
      else {
         // no fontmapmodel, so revert back to the original scrool method.
         me->yScrollPos = me->nScrollLine * (me->cyChar + me->nLineGap);
      }
      
   }
   else {
      // single line
      // does caret show in the viewport?
      if ( me->dwFlags & TWF_SHORTENTEXT && me->border.bActive == FALSE) {
         me->xScrollPos = 0;
      }
      else {

         nErr = ITEXTLAYOUT_GetLayoutSize(me->piTextLayout, &weLayout, &nLineCount);
   
         if ( nLineCount == 1 && weLayout.width > me->border.rcClient.dx ) {

            AEERect rcCaret;
            if ( ITEXTLAYOUT_GetCaretInfo(me->piTextLayout, nCaret, &rcCaret) == SUCCESS ) {

               if ( rcCaret.x > me->xScrollPos + me->border.rcClient.dx - nMargin ) {
                  me->xScrollPos = MIN( rcCaret.x - me->border.rcClient.dx + nMargin, 
                                   weLayout.width - me->border.rcClient.dx );
               } 
               else if ( rcCaret.x < me->xScrollPos + nMargin ) {
                  me->xScrollPos = MAX(0, rcCaret.x - nMargin);
               }
            }
         }
         else {
            me->xScrollPos = 0;
         }
      }
   }

}



static
boolean TextWidget_GetTextRects(TextWidget* me, WidgetTextRects* wtr) {

   int nErr;
   uint32   nAdjust;
   AEERect* pRects = 0;

   uint32   nOriginalCount = wtr->count;

   TextWidget_LayoutText(me, WBASE(me)->piModel);

   TextWidget_AdjustScrollPos(me, &me->ti);
   nErr = ITEXTLAYOUT_GetProperty(me->piTextLayout, PROP_TEXTRECTS, (uint32*) wtr);

   if ( SUCCESS == nErr ) {
      pRects = wtr->pRects;
      for ( nAdjust = 0; nAdjust < wtr->count && nAdjust < nOriginalCount; ++nAdjust) {

	 pRects->x += me->border.rcClient.x;
	 pRects->y += me->border.rcClient.y;

	 if ( me->dwFlags & TWF_MULTILINE ) {
	    // multi line scrolling
	    pRects->y -= me->yScrollPos;

	 }
	 else {
	    // single line scrolling
	    pRects->x -= me->xScrollPos;
	 }

	 ++pRects;
      }
   }

   if ( SUCCESS == nErr ) {
      TextWidget_QueueCaretScrollEvent(me);
   }

   return ( SUCCESS == nErr ) ? TRUE : FALSE;
}

static __inline int TextWidget_GetCaretInfo    (TextWidget* me, int* pxCaret, int* pyCaret) {
   int nResult = SUCCESS;
   AEERect rcCaret;
   nResult = ITEXTLAYOUT_GetCaretInfo(me->piTextLayout,  me->ti.nSelStart, &rcCaret);
   if ( SUCCESS == nResult )
   {
      *pxCaret = rcCaret.x;
      *pyCaret = rcCaret.y;
   }
   return nResult;
}



void TextWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;

   AEERect rc;
   AEERect rcClip;
   IDisplay *piDisplay = 0;
   int nDrawX, nDrawY;
   RGBAVAL rgbaTextFG;
   AEERect rcOriginalClip;

   FARF(DRAW, ("Draw"));

   me->xDraw = x;
   me->yDraw = y;

   Border_Draw(&me->border, piCanvas, x, y);
   ICANVAS_GetClipRect(piCanvas, &rcClip);
   rcOriginalClip = rcClip;

   rc = me->border.rcClient;
   rc.x += x;
   rc.y += y;

   if (IntersectRect(&rcClip, &rcClip, &rc) 
         && me->piTextModel
         && SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      // if we actually have text to draw...
      if (me->ti.pwText) {

         IModel* piPasswordModel;

         RGBAVAL rgbaSelectBG = me->rgbaSelected[(me->border.bActive) ? 1 : 0];
         RGBAVAL rgbaSelectFG = me->rgbsText[(me->border.bActive) ? 1 : 0];

         //(void) DynRGB_Get(&me->rgbText, (me->border.bActive) ? RGBINDEX_SACTIVE : RGBINDEX_SINACTIVE, &rgbaSelectFG);
         (void) DynRGB_Get(&me->rgbText, (me->border.bActive) ? RGBINDEX_ACTIVE  : RGBINDEX_INACTIVE,  &rgbaTextFG);

         TextWidget_SetLayoutColor(me, rgbaTextFG, rgbaSelectFG, rgbaSelectBG );
         TextWidget_SetLayoutFlags(me);

         piPasswordModel = TextWidget_PasswordFix(me, &me->ti);

         TextWidget_LayoutText(me, piPasswordModel ? piPasswordModel : WBASE(me)->piModel );

         RELEASEIF(piPasswordModel);

         nDrawX = rc.x;
         nDrawY = rc.y;

         if ( !me->border.bActive && me->dwFlags & TWF_SHORTENTEXT ) {
            ;
         } 
         else {
            TextWidget_AdjustScrollPos(me, &me->ti);

            if ( me->dwFlags & TWF_MULTILINE ) {
               // multi line scrolling
               nDrawY -= me->yScrollPos;
            }
            else {
               // single line scrolling
               nDrawX -= me->xScrollPos;
            }
         }


         // set the clip rect to the border client area.
         (void) ICANVAS_SetClipRect(piCanvas, &rcClip);
         (void) ITEXTLAYOUT_Draw(me->piTextLayout, piCanvas, nDrawX, nDrawY);
         (void) ICANVAS_SetClipRect(piCanvas, &rcOriginalClip);

         // only draw caret if active and selection span is zero
         if (me->piCaret && me->border.bActive && me->ti.nSelStart == me->ti.nSelEndCaret && rc.dx > 0 ) {
            int xCaret = 0;
            int yCaret = 0;
            if ( TextWidget_GetCaretInfo(me, &xCaret, &yCaret) == SUCCESS ) {
               xCaret += nDrawX - 3;
               yCaret += nDrawY;
               IWIDGET_Draw(me->piCaret, piCanvas, xCaret, yCaret);
            }
            me->xCaret = xCaret;
            me->yCaret = yCaret;
         }

      }
   }
   
   RELEASEIF(piDisplay);

   TextWidget_QueueCaretScrollEvent(me);
}


boolean TextWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}



void TextWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;
   int dwResult = SUCCESS;
   WExtent weClient;
   weClient.height = 12;
   weClient.width  = 10;

   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_HINT_ROWS,    (uint32) me->nHintRows);
   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_HINT_WIDTH,   
      (uint32) me->cxHintWidth ? me->cxHintWidth : me->border.rcClient.dx );

   TextWidget_SetLayoutFlags(me);

   dwResult = ITEXTLAYOUT_GetPreferredExtent(me->piTextLayout, WBASE(me)->piModel, me->dwLayoutFlags, &weClient);

   if ( SUCCESS == dwResult ) {
      if ( !(me->dwFlags & TWF_MULTILINE) && me->piCaret && me->ti.pwText && me->ti.cchText ) {  
	 WExtent we;
	 IWIDGET_GetPreferredExtent(me->piCaret, &we);
	 weClient.width += we.width / 2;
      }
   }

   Border_CalcPreferredExtent(&me->border, pweOut, &weClient);
}


int TextWidget_CreateTextLayout(TextWidget* me, AEECLSID clsidTextLayout) {

   int nResult = SUCCESS;

   me->clsidTextLayout = clsidTextLayout;
   RELEASEIF(me->piTextLayout);

   if ( me->clsidTextLayout ) {

      nResult = ISHELL_CreateInstance(me->piShell, me->clsidTextLayout, (void**) &me->piTextLayout);
      
      if ( SUCCESS == nResult ) {


         ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_LINEGAP,      (uint32) me->nLineGap);
         ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_FONT,         (uint32) me->piFont);
         ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_FONTMAPMODEL, (uint32) me->piFontMapModel);
      }
   }

   me->fLayoutText = TRUE;
   return nResult;
}

static __inline TextWidget_SetTextLayoutProperty(TextWidget* me, uint16 wParam, uint32 dwParam) {
   if ( me->piTextLayout ) { 
      me->fLayoutText = TRUE;
      ITEXTLAYOUT_SetProperty(me->piTextLayout, wParam, dwParam);
   }
}

static void Container_Invalidate(IContainer *po, IWidget *piw, 
                                 const AEERect *prc, uint32 dwFlags)
{
   TextWidget *me = (TextWidget*)po->pMe;
   AEERect rc;
   
   if (prc) {
      rc = *prc;
   } else {
      SETAEERECT(&rc, 0,0,0,0);
      if (piw) {
         WExtent we;
         IWIDGET_GetExtent(piw, &we);
         rc.dx = we.width;
         rc.dy = we.height;
      }
   }
   
   rc.x += me->border.rcClient.x;
   rc.y += me->border.rcClient.y;
   
   WidgetContBase_Invalidate(po, piw, &rc, dwFlags);
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
            IContainer *pic = NULL;
            *(IWidget **) dwParam = me->piCaret;
            ADDREFIF(me->piCaret);
            IWIDGET_QueryInterface(po, AEEIID_CONTAINER, (void**)&pic);
            IWIDGET_SetParent(me->piCaret, pic);
            RELEASEIF(pic);
            return TRUE;
         }
         if (wParam == PROP_TEXTLAYOUTCLASSID) {
            *(AEECLSID*)dwParam = me->clsidTextLayout;
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
         if (wParam == PROP_TEXTLAYOUTCLASSID) {
            TextWidget_CreateTextLayout(me, (AEECLSID) dwParam);
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
      me->fLayoutText = TRUE;
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

         } else if(wParam == PROP_TEXT_SELECTED_FGCOLOR) {
            me->rgbsText[0]              =
            me->rgbsText[1]              = (RGBVAL)dwParam;

         } else if(wParam == PROP_TEXT_SINACTIVE_FGCOLOR) {
            me->rgbsText[0] = (RGBVAL)dwParam;

         } else if(wParam == PROP_TEXT_SACTIVE_FGCOLOR) {
            me->rgbsText[1] = (RGBVAL)dwParam;

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
            TextWidget_SetLayoutFlags(me);

         } else if (wParam == PROP_HINT_ROWS) {
            me->nHintRows = dwParam;
            TextWidget_SetTextLayoutProperty(me, wParam, dwParam);

         } else if (wParam == PROP_HINT_WIDTH) {
            me->cxHintWidth = dwParam;
            TextWidget_SetTextLayoutProperty(me, wParam, dwParam);
         
         } else if (wParam == PROP_PASSWORDMASKCHAR) {
            me->wchMask = (AECHAR)dwParam;

         } else if (wParam == PROP_PASSWORDMASKDELAY) {
            me->nMaskTimeout = (int16)dwParam;

         } else if (wParam == PROP_LINEGAP) {
            me->nLineGap = dwParam;
            TextWidget_SetTextLayoutProperty(me, wParam, dwParam);
            TextWidget_UseFont(me);

         } else if (wParam == PROP_FONTMAPMODEL) {
            RELEASEIF(me->piFontMapModel);
            me->piFontMapModel = (IFontMapModel *) dwParam;
            ADDREFIF(me->piFontMapModel);
            TextWidget_UseFontMapModel(me);

         } else if (wParam == PROP_ELLIPSIS) {
            me->awEllipsis = (AECHAR)dwParam;
            me->fLayoutText = 1;

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

         } else if (wParam == PROP_TEXT_SELECTED_FGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbsText[1];

         } else if (wParam == PROP_TEXT_SINACTIVE_FGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbsText[0];

         } else if (wParam == PROP_TEXT_SACTIVE_FGCOLOR) {
            *(RGBVAL *)dwParam = me->rgbsText[1];

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
            TextWidget_GetTextRects(me, (WidgetTextRects*) dwParam);
         
         } else if (wParam == PROP_VIEWMODEL) {
            *(IModel**)dwParam = WBASE(me)->piViewModel;
            IMODEL_AddRef(WBASE(me)->piViewModel);
         
         } else if (wParam == PROP_CARETPOS) {
            //int x = 0, y = 0;
            //TextWidget_GetCaretInfo(me, &x, &y);
            //*((uint32*)dwParam) = ( ((uint32)y << 16) | (uint32)x );
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
            
            TextWidget_LayoutText(me, WBASE(me)->piModel);

            ITEXTLAYOUT_GetLineFromIndex(me->piTextLayout, *((int*)dwParam), &line);
            if (wParam == PROP_PREVLINE) {
               line--;
            } else if (wParam == PROP_NEXTLINE) {
               line++;
            }
            if (SUCCESS == ITEXTLAYOUT_GetLineStartIndex(me->piTextLayout, line, &pos)) {
               *((int*)dwParam) = pos;
            } else {
               return FALSE;
            }

         } else if (wParam == PROP_CURSOR_MOVE) {

            CursorMovementType* pCmt = (CursorMovementType*) dwParam;
            if ( 0 == pCmt->flags ) {

               TextWidget_LayoutText(me, WBASE(me)->piModel);

               if ( SUCCESS != ITEXTLAYOUT_GetCaretAfterMove(me->piTextLayout, pCmt->nIndex, pCmt->dwMovement, &pCmt->nMoveIndex) ) {
                  break;
               }
            }
            else
               break;
       
         } else if (wParam == PROP_FONTMAPMODEL) {
            *(IFontMapModel**)dwParam = me->piFontMapModel;
            ADDREFIF(me->piFontMapModel);

         } else if (wParam == PROP_ELLIPSIS) {
            *(AECHAR *)dwParam = TextWidget_GetEllipsisChar(me);

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
   LISTENER_Cancel(&me->mlViewListener);

   CALLBACK_Cancel(&me->cbkUpdate);
   CALLBACK_Cancel(&me->cbkNotify);
   CALLBACK_Cancel(&me->cbkMask);

   // textwidget specific cleanup
   LISTENER_Cancel(&me->mlFont);
   LISTENER_Cancel(&me->mlFontMapModel);
   RELEASEIF(me->piFont);
   RELEASEIF(me->piTextModel);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piController);
   RELEASEIF(me->piCaret);
   RELEASEIF(me->piTextLayout);
   RELEASEIF(me->piFontMapModel);
   RELEASEIF(me->piPasswordModel);

   FREEIF(me->pwMaskBuf);

   Border_Dtor(&me->border);
   DynRGB_Dtor(&me->rgbText);
}

uint32 TextWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (WBASE(me)->nRefs == 1) {
      TextWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}

void TextWidget_Ctor(TextWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetContBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(TextWidget_HandleEvent));

   pvt->Release            = TextWidget_Release;
   pvt->GetPreferredExtent = TextWidget_GetPreferredExtent;
   pvt->IntersectOpaque    = TextWidget_IntersectOpaque;
   pvt->Draw               = TextWidget_Draw;
   pvt->SetModel           = TextWidget_SetModel;
   pvt->SetExtent          = TextWidget_SetExtent;
   pvt->GetExtent          = TextWidget_GetExtent;
   
   // override IContainer functions
   me->base.vtContainer.Invalidate = Container_Invalidate;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);
   
   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &WBASE(me)->extent, TRUE, &WBASE(me)->piViewModel);

   // setup the border & background properties
   me->border.padding.left     = 3;
   me->border.padding.right    = 2;
   me->border.padding.top      = 1;
   me->border.padding.bottom   = 1;

   Border_SetBGColor(&me->border, 0, RGBA_WHITE);

   me->cyChar      = 12;
   me->nLineGap    = 0;
   me->dwFlags     = 0;   

   DynRGB_Ctor(&me->rgbText, RGB_BLACK);
   DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, RGB_WHITE);
   DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, RGB_WHITE);

   me->rgbaSelected[0] = 
   me->rgbaSelected[1] = MAKE_RGBA(0,0,128,255);

   me->rgbsText[0] = me->rgbsText[1] = RGB_WHITE;

   me->dwCaretLineParam = (uint32) -1;  // force EVT_MDL_CARETLINE_CHANGE to be sent initially

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
      nErr = TextModel_New(&piTextModel, WBASE(me)->piModule);
   }

   if (!nErr) {
      nErr = WidgetBase_GetViewModel(WBASE(me), &piViewModel);
   }

   if (!nErr) {
      IMODEL_AddListenerEx(piViewModel, &me->mlViewListener, (PFNLISTENER)TextWidget_TextViewModelListener, me);
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
      // try to instantiate the OEM Text Layout
      if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_TEXTLAYOUT, 
                                           (void**)&me->piTextLayout)) {
         // otherwise, use the default one
         nErr = TextLayout_New(&me->piTextLayout, me->piShell, WBASE(me)->piModule);
      }
   }

   if (!nErr) {

      // try to instantiate the OEM caret
      if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_CARETWIDGET, 
                                           (void**)&me->piCaret)) {
         // otherwise, use the default one
         nErr = CaretWidget_New(&me->piCaret, me->piShell, WBASE(me)->piModule);
      }
   }

   if (!nErr) {

      TextWidget_UseFont(me);

      // create the TextController
      // try to instantiate the OEM text controller
      if (SUCCESS != ISHELL_CreateInstance(me->piShell, AEECLSID_TEXTCONTROLLER, 
                                           (void**)&me->piController)) {
         // otherwise, use the default one
         nErr = TextController_New(&me->piController, me->piShell, WBASE(me)->piModule);
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
