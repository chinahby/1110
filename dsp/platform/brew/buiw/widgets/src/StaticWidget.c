/*======================================================
FILE:       StaticWidget.h

SERVICES:   Draws static text 

GENERAL DESCRIPTION:

 Display static text with a transparent.background or in a rectangle 
 filled with a background color. 

Reproduction and/or distribution of this file without the
written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "StaticWidget.h"
#include "wutil.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "AEEStdLib.h"
#include "AEEFont.h"
#include "AEEDisplayCanvas.h"
#include "AEEHFontOutline.h"

#define ME_FROM_WIDGET StaticWidget *me = (StaticWidget *)po

#define DEFAULT_SCROLLAMOUNT     3
#define DEFAULT_STARTDELAY       1000
#define DEFAULT_SCROLLDELAY      100
#define DEFAULT_ENDDELAY         1000
#define DEFAULT_REPEAT           0

static __inline void StaticWidget_Invalidate(StaticWidget *me)
{
   WidgetBase_InvalidateContent(STATICWIDGET_TO_IWIDGET(me));
}

static __inline AECHAR *StaticWidget_GetText(StaticWidget *me, int *pnLen)
{
   if ((IValueModel*)me->base.piModel) {
      AECHAR *pw = (AECHAR*)IVALUEMODEL_GetText((IValueModel*)me->base.piModel, pnLen);
      if (pw && pnLen && *pnLen < 0) {
         *pnLen = WSTRLEN(pw);
      }
      return pw;
   } else {
      return 0;
   }
}

// returns the height of all ines for the preferred width
static int StaticWidget_WrapToPreferredWidth(StaticWidget *me, int cxWidth)
{
   int cyHeight = 10;
   LayoutTextStyleInfo style;

   int nLen;
   AECHAR *pwText = StaticWidget_GetText(me, &nLen);

   me->fRewrap = 1;  // force rewrap, table no longer valid

   LayoutText_SetLineBreak (me->poLayoutText, 
      (boolean) ((me->dwFlags & SWF_WRAPTEXT) || (me->dwFlags & SWF_WRAPNOBREAK) ? 1 : 0),
      (boolean) ( me->dwFlags & SWF_WRAPNOBREAK ? 0 : 1 ) );

   LayoutText_SetTextAlignment(me->poLayoutText , me->dwDefaultAlignment);

   style.piFont = me->piFont;
   style.rgbBackground = RGB_NONE;
   style.rgbForeground = RGB_NONE;
   LayoutText_DeleteText(me->poLayoutText, LAYOUTTEXT_MIN_TEXT_RANGE, LAYOUTTEXT_MAX_TEXT_RANGE);
   if (pwText && nLen ) {
      LayoutText_AddText   (me->poLayoutText, &style, pwText, nLen, LAYOUTTEXT_MIN_TEXT_RANGE);
   }

   LayoutText_SetWidth  (me->poLayoutText, cxWidth);
   LayoutText_GetHeight (me->poLayoutText, &cyHeight);

   return cyHeight;
}

static __inline void StaticWidget_CalcLineTbl(StaticWidget *me)
{
   if (me->fRewrap) {
      StaticWidget_WrapToPreferredWidth(me, me->border.rcClient.dx);
      me->fRewrap = 0;
   }
}


static void StaticWidget_FontRecalc(StaticWidget *me)
{

   me->fRewrap = 1;  // force rewrap, load new font into LayoutText object

   if (me->piFont) {
      AEEFontInfo fi;
      int nLen;
      AECHAR *pwText = StaticWidget_GetText(me, &nLen);      

      IFONT_GetInfo(me->piFont, &fi, sizeof(fi));

      SETWEXTENT(&me->weFont, 0, fi.nAscent + fi.nDescent);

      // If we have some text, then the width can be calculated too
      if (pwText && nLen && me->piFont) {
         int nFit;
         IFONT_MeasureText(me->piFont, pwText, nLen, IFONT_MAXWIDTH, &nFit, &me->weFont.width);
      }
   }
   else
      SETWEXTENT(&me->weFont, 0, 12);    // some defaults (arbitrary height inherited)
}


static void StaticWidget_CancelScrolling(StaticWidget *me)
{
   me->nScrollOffset = 0; 
   CALLBACK_Cancel(&me->cbkScroll);
}


static void StaticWidget_ScrollCallback(IWidget *po)
{
   StaticWidget *me = (StaticWidget*)po;
   
   if (me->weFont.width > me->border.rcClient.dx) {
      uint32 dwStepFlags = 0;

      me->nScrollOffset += me->stProps.nScrollAmount;

      if (me->bScrollReset) {
   
         me->nScrollOffset = 0;
         me->nScrollDelay = me->stProps.nStartDelay;
         me->bScrollReset = FALSE;
         me->nCurStep = 0;

         if (me->stProps.nRepeat > 0 && me->stProps.nRepeat <= me->nScrollRepeat) {
            me->bAnimate = FALSE;
         }    
      } else if (me->nScrollOffset > (me->weFont.width - me->border.rcClient.dx)) {
   
         me->nScrollDelay = me->stProps.nEndDelay;
         me->bScrollReset = TRUE;
         dwStepFlags |= EVT_STEP_FINAL;

         if (me->stProps.nRepeat == 0 || (me->nScrollRepeat+1) < me->stProps.nRepeat) {
            dwStepFlags |= EVT_STEP_REPEAT;
         }
   
      } else {
         me->nScrollDelay = me->stProps.nScrollDelay;
      }
   
      WidgetBase_Invalidate(STATICWIDGET_TO_IWIDGET(me), ICIF_REDRAW);
      
      if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
         IMODEL_StepNotify(WBASE(me)->piViewModel, dwStepFlags, me->nCurStep, me->nScrollRepeat);
      }

      // advance step counters now that we've sent StepEvents out.
      if (me->bScrollReset) {
         ++me->nScrollRepeat;
      }
      else {
         ++me->nCurStep;
      }

      if (me->bAnimate) {
         ISHELL_SetTimerEx(me->piShell, me->nScrollDelay, &me->cbkScroll);
      }
   }
}

static void StaticWidget_ResetScrollText(StaticWidget *me)
{
   if (me->bAnimate)
   {
      me->bScrollReset = FALSE;
      me->nScrollDelay = me->stProps.nStartDelay;
      me->nScrollRepeat = 0;
      me->nCurStep = 0;
   
      StaticWidget_CancelScrolling(me);

      // start marquee timer only if needed ...
      if (me->weFont.width > me->border.rcClient.dx) {
         if (me->dwAnimFlags & AF_ENABLE_EVT_STEP) {
            IMODEL_StepNotify(WBASE(me)->piViewModel, EVT_STEP_PRE, (uint16)me->nCurStep++, (uint16)me->nScrollRepeat);
         }
         ISHELL_SetTimerEx(me->piShell, me->nScrollDelay, &me->cbkScroll);
      }
   }
}


static void StaticWidget_FontChanged(StaticWidget *me, ModelEvent *pEvent)
{
   IHFont *piHFont;

   if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
      // we're an IHFont and the size of our characters may have changed
      StaticWidget_FontRecalc(me);
      RELEASEIF(piHFont);
   }

   StaticWidget_ResetScrollText(me);
   StaticWidget_Invalidate(me);
}


static void StaticWidget_UseFont(StaticWidget *me)
{
   LISTENER_Cancel(&me->mlFont);

   if (me->piFont) {
      IModel *piModel = 0;
      
      // recalculate ...
      StaticWidget_FontRecalc(me);

      // if font supports IModel, attach a listener for animated fonts
      if (SUCCESS == IFONT_QueryInterface(me->piFont, AEEIID_MODEL, (void**)&piModel)) {
         IMODEL_AddListenerEx(piModel, &me->mlFont, (PFNLISTENER)StaticWidget_FontChanged, me);
         IMODEL_Release(piModel);
      }
      StaticWidget_Invalidate(me);
   }

}


void StaticWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;

   // adjust text if we're scrolling so it's in the same relative position
   if (me->nScrollOffset)
      me->nScrollOffset += (me->base.extent.width - pe->width);

   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   Border_CalcClientRect(&me->border);
   me->fRewrap = 1;
}

/*

   Conditions:                         Result:
   
   1) PROP_HINT_WIDTH is non-zero      Calculate height according to hint width
      SWF_WRAPTEXT is TRUE
      
   2) PROP_HINT_WIDTH is zero          Calculate height according to current extent width
      SWF_WRAPTEXT is TRUE
    
   3) PROP_ANIMATE is TRUE             Return current extent.  The widget has a means to draw
                                       text without resizing larger.
   
   4) SWF_NOSHORTENTEXT is FALSE       Return current extent.  The widget can indicate to the user
      (draw ellipsis)                  that it is not drawing all the text.  

   5) SWF_WRAPTEXT is FALSE            Returned height is for one line
                                       Returned width is calculated from current text
   

*/
void StaticWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;

   if ((me->dwFlags & SWF_WRAPTEXT) || (me->dwFlags & SWF_WRAPNOBREAK)) {
      WExtent weClient;  
      
      int rows = me->nHintRows;
      int cy = 0;
      int cx = me->cxHintWidth ? me->cxHintWidth : me->border.rcClient.dx;

      cx = MAX(10, cx);

      if (!rows) {
         cy = StaticWidget_WrapToPreferredWidth(me, cx);
      } 
      else {
         cy = rows * (me->weFont.height + me->nLineGap) - me->nLineGap;
      }

      cy = MAX(me->weFont.height, cy);

      weClient.height = cy;
      weClient.width  = cx;
      Border_CalcPreferredExtent(&me->border, pweOut, &weClient);    
   } else 
      Border_CalcPreferredExtent(&me->border, pweOut, &me->weFont);
}


void StaticWidget_DrawRectGetRect(IWidget *po, ICanvas *piCanvas, int x, int y, AEERect *prcOut)
{
   ME_FROM_WIDGET;
   Border_Draw(&me->border, piCanvas, x, y);
   *prcOut = me->border.rcClient;
   prcOut->x += x;
   prcOut->y += y;
}


void StaticWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;
   IDisplay *piDisplay = 0;
   uint32 dwFlags = me->dwFlags;
   boolean bDrawEllipsis;

   int nDrawOffset;
   int nTotalLen;
   AEERect rc;
   AECHAR *pwText = StaticWidget_GetText(me, &nTotalLen);
   StaticWidget_CalcLineTbl(me);
   StaticWidget_DrawRectGetRect(po, piCanvas, x, y, &rc);

   // Return if there is no text to draw, but only after above 
   //  rectangle drawing (for background)
   if (!pwText) {
      return;
   }

   // set the ellipsis state for this draw...
   if ( me->dwFlags & SWF_WRAPTEXT ) {
      bDrawEllipsis = ( me->dwFlags & (SWF_MULTILINESHORTEN&~SWF_WRAPTEXT) ) ? 1 : 0;
   }
   else {
      bDrawEllipsis = ( me->dwFlags & SWF_NOSHORTENTEXT || me->dwFlags & SWF_WRAPNOBREAK || me->nScrollOffset ) ? 0 : 1;
   }
   LayoutText_SetEllipsis(me->poLayoutText, bDrawEllipsis );

   // set the scroll offset, depending upon left-to-right or right-to-left drawing.
   LayoutText_GetScrollPosition(me->poLayoutText, me->nScrollOffset, &nDrawOffset);

   x = rc.x;
   y = rc.y;

   dwFlags = (dwFlags & SWF_FLAGSMASK) | IDF_TEXT_TRANSPARENT;

   // for marquee scrolling, clear horizontal alignment bits
   // but leave others intact
   if (me->bAnimate && nDrawOffset) {
      dwFlags &= ~IDF_ALIGNHORZ_MASK;
   }


   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      int ndx = (me->border.bActive != 0) + (2 * (me->border.bSelected != 0));
      RGBAVAL rgba;
      DynRGB_Get(&me->rgbText, ndx, &rgba);

      LayoutText_Draw(me->poLayoutText, piDisplay, x + nDrawOffset, y, NULL, dwFlags, &rc, rgba);
   }

   RELEASEIF(piDisplay);

}

// since we're transparent, return empty rect
boolean StaticWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}

#define SETIF(memb,var) \
   do { RELEASEIF(memb); (memb)=(var); ADDREFIF(memb); } while(0)

boolean StaticWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch (evt) {
   
   case EVT_WDG_SETPROPERTY:
      if (wParam == PROP_FONT) {
         if ((IFont*)dwParam != me->piFont) {
            SETIF(me->piFont, (IFont*)dwParam);
            StaticWidget_UseFont(me);
            me->fRewrap = 1;
         }

      } else if (wParam == PROP_FONT_CLASS) {
         IFont *piFont;
         if (SUCCESS == ISHELL_CreateFont(me->piShell, (AEECLSID)dwParam, &piFont)) {
            RELEASEIF(me->piFont);
            me->piFont = piFont;
            StaticWidget_UseFont(me);
            me->fRewrap = 1;
         }

      } else if (wParam == PROP_FONT_OUTLINECOLOR) {
         IHFont *piHFont;
         if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
            IHFONT_SetFontOutlineColor(piHFont, (RGBVAL)dwParam);
            RELEASEIF(piHFont);
            StaticWidget_UseFont(me);
            me->fRewrap = 1;
         } else {
            break;
         }

      } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
         IHFont *piHFont;
         if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
            IHFONT_SetFontOutlineWidth(piHFont, dwParam);
            RELEASEIF(piHFont);
            StaticWidget_UseFont(me);
            me->fRewrap = 1;
         } else {
            break;
         }      

      } else if (wParam == PROP_FLAGS) {
         me->fRewrap = 1;
         me->dwFlags = dwParam;
      
      } else if (wParam == PROP_DEFAULTTEXTALIGNMENT) {
         me->fRewrap = 1;
         me->dwDefaultAlignment = dwParam;
      
      } else if (wParam == PROP_FGCOLOR) {
         DynRGB_Collapse(&me->rgbText, (RGBVAL)dwParam);
      
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
      
      } else if (wParam == PROP_HINT_WIDTH) {
         me->cxHintWidth = (int)dwParam;

      } else if (wParam == PROP_HINT_ROWS) {
         me->nHintRows = (int)dwParam;

      } else if (wParam == PROP_LINEGAP) {
         me->nLineGap = (int)dwParam;
         LayoutText_SetLineGap(me->poLayoutText, me->nLineGap);         
      } else if (wParam == PROP_ANIMATE) {

         me->bAnimate = (boolean)dwParam;
         // if we turned off scrolling, we need to kill the callback
         if (!me->bAnimate) {
            StaticWidget_CancelScrolling(me);
            WidgetBase_Invalidate((IWidget*)me, ICIF_REDRAW);
         }
         StaticWidget_ResetScrollText(me);
         return TRUE;      // don't invalidate

      } else if (wParam == PROP_SCROLLTEXT && dwParam) {

         MEMCPY(&me->stProps, (ScrollText*)dwParam, sizeof(ScrollText));
         StaticWidget_ResetScrollText(me);
         return TRUE;      // don't invalidate

      } else if (wParam == PROP_ANIMATE_FLAGS) {
         me->dwAnimFlags = dwParam;
         return TRUE;
      } else
         break;
      StaticWidget_Invalidate(me);
      return TRUE;

   case EVT_WDG_GETPROPERTY: 
      if (wParam == PROP_FONT) {
         *(IFont **)dwParam = me->piFont;
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
         *(uint32 *)dwParam = me->dwFlags;

      } else if (wParam == PROP_DEFAULTTEXTALIGNMENT) {
         *(uint32 *)dwParam = me->dwDefaultAlignment;
      
      
      } else if (wParam == PROP_FGCOLOR || wParam == PROP_ACTIVE_FGCOLOR) {
         DynRGB_Get(&me->rgbText, RGBINDEX_ACTIVE, (uint32*)dwParam);
      
      } else if (wParam == PROP_SELECTED_FGCOLOR || wParam == PROP_SACTIVE_FGCOLOR) {
         DynRGB_Get(&me->rgbText, RGBINDEX_SACTIVE, (uint32*)dwParam);

      } else if (wParam == PROP_SINACTIVE_FGCOLOR) {
         DynRGB_Get(&me->rgbText, RGBINDEX_SINACTIVE, (uint32*)dwParam);
      
      } else if (wParam == PROP_INACTIVE_FGCOLOR) {
         DynRGB_Get(&me->rgbText, RGBINDEX_INACTIVE, (uint32*)dwParam);

      } else if (wParam == PROP_HINT_WIDTH) {
         *(int*)dwParam = me->cxHintWidth;

      } else if (wParam == PROP_HINT_ROWS) {
         *(int*)dwParam = me->nHintRows;

      } else if (wParam == PROP_LINEGAP) {
         *(int*)dwParam = me->nLineGap;

      } else if (wParam == PROP_ANIMATE) {
         *(int*)dwParam = me->bAnimate;
      }else if (wParam == PROP_ANIMATE_FLAGS) {
         *(uint32 *)dwParam = me->dwAnimFlags;
      } else
         break;
      return TRUE;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


static 
void StaticWidget_ModelChanged(StaticWidget *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_VALUE) {
      StaticWidget_FontRecalc(me);
      StaticWidget_ResetScrollText(me);
      StaticWidget_Invalidate(me);
      me->fRewrap = 1;
   }
}


int StaticWidget_SetModel(IWidget *po, IModel *piModel)
{
   IModel *piModelTemp = NULL;
   int nErr;

   ME_FROM_WIDGET;
   
   // are we allowing NULL models to be passed?
   if (!piModel) {
      return WidgetBase_SetModel(po, piModel);
   }

   nErr = IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&piModelTemp);
   RELEASEIF(piModelTemp);

   if (nErr != SUCCESS) {
      return nErr;
   }

   WidgetBase_SetModel(po, piModel);
   IMODEL_AddListenerEx(piModel, &me->modelListener, (PFNLISTENER)StaticWidget_ModelChanged, me);
   StaticWidget_Invalidate(me);

   return SUCCESS;
}

uint32 StaticWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      StaticWidget_Dtor(me);
   }      

   return WidgetBase_Release(po);
}

void StaticWidget_Ctor(StaticWidget *me, AEEVTBL(IWidget) *pvt, IModel *piModel, 
                       IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WExtent we;

   WidgetBase_Ctor(&me->base, pvt, piModule, 
                   DEFHANDLER(StaticWidget_HandleEvent));
   
   pvt->Release            = StaticWidget_Release;
   pvt->GetPreferredExtent = StaticWidget_GetPreferredExtent;
   pvt->SetExtent          = StaticWidget_SetExtent;
   pvt->Draw               = StaticWidget_Draw;
   pvt->IntersectOpaque    = StaticWidget_IntersectOpaque;
   pvt->SetModel           = StaticWidget_SetModel;

   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, FALSE, &WBASE(me)->piViewModel);
   DynRGB_Ctor(&me->rgbText, RGBA_BLACK);

   we.width  = 30;
   we.height = 12;
   Border_CalcPreferredExtent(&me->border, &we, &we);
   StaticWidget_SetExtent((IWidget*)&me->base, &we);
   
   // default to no border
   me->border.nWidth[0] = 
   me->border.nWidth[1] = 0;

   Border_SetBorderColor(&me->border, 0, RGBA_NONE);
   Border_SetBGColor(&me->border, 0, RGBA_NONE);
   
// other ctor stuff
   me->nLineGap = 0;

   me->dwFlags            = IDF_ALIGN_MIDDLE;

   me->dwDefaultAlignment = IDF_ALIGN_NONE;


   LayoutText_New(&me->poLayoutText, piShell);

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   ISHELL_CreateFont(piShell, AEECLSID_FONTSYSNORMAL, &me->piFont);
   StaticWidget_UseFont(me);

   StaticWidget_SetModel(STATICWIDGET_TO_IWIDGET(me), piModel);

   // initialize scrolling
   me->stProps.nScrollAmount = DEFAULT_SCROLLAMOUNT;
   me->stProps.nStartDelay = DEFAULT_STARTDELAY;
   me->stProps.nScrollDelay = DEFAULT_SCROLLDELAY;
   me->stProps.nEndDelay = DEFAULT_ENDDELAY;
   me->stProps.nRepeat = DEFAULT_REPEAT;

   me->dwAnimFlags = 0;

   CALLBACK_Init(&me->cbkScroll, StaticWidget_ScrollCallback, STATICWIDGET_TO_IWIDGET(me));
}

void StaticWidget_Dtor(StaticWidget *me)
{
   CALLBACK_Cancel(&me->cbkScroll);
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Cancel(&me->mlFont);
   RELEASEIF(me->piFont);
   RELEASEIF(me->piShell);
   LayoutText_Delete(me->poLayoutText);
   Border_Dtor(&me->border);
   DynRGB_Dtor(&me->rgbText);
}


int StaticWidget_New(IWidget **ppo, IModel *piModel, IShell *piShell, IModule *piModule)
{
   int nErr = SUCCESS;
   
   if (!piModel) {
      nErr = ValueModel_New((IValueModel**)&piModel, piModule);
   } else {
      IMODEL_AddRef(piModel);
   }

   if (nErr == SUCCESS) {
      
      StaticWidget *me = MALLOCREC_VTBL(StaticWidget, IWidget);

      if (me) {
         StaticWidget_Ctor(me, GETVTBL(me, IWidget), piModel, piShell, piModule, 0);
         *ppo = STATICWIDGET_TO_IWIDGET(me);
      } else {
         nErr = ENOMEMORY;
      }
   }

   RELEASEIF(piModel);

   return nErr;
}


