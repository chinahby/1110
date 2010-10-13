/*======================================================
FILE:       StaticWidget.h

SERVICES:   Draws static text 

GENERAL DESCRIPTION:

 Display static text with a transparent.background or in a rectangle 
 filled with a background color. 

Reproduction and/or distribution of this file without the
written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "StaticWidget.h"
#include "wutil.h"
#include "wlint.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "AEEStdLib.h"
#include "AEEFont.h"
#include "AEEDisplayCanvas.h"
#include "AEEHFontOutline.h"
#include "AEERichTextModel.h"
#include "AEETextModel.h"
#include "TextLayout.h"


#define DEFAULT_SCROLLAMOUNT     3
#define DEFAULT_STARTDELAY       1000
#define DEFAULT_SCROLLDELAY      100
#define DEFAULT_ENDDELAY         1000
#define DEFAULT_REPEAT           0

static __inline IWidget *STATICWIDGET_TO_IWIDGET(StaticWidget *p) { 
   return (IWidget*) p; 
}

static __inline StaticWidget *IWIDGET_TO_STATICWIDGET(IWidget *p) { 
   return (StaticWidget*) p; 
}

static void StaticWidget_ResetScrollText(StaticWidget *me);

static __inline AECHAR *StaticWidget_GetText(StaticWidget *me, int *pnLen)
{
   AECHAR *pwText = NULL;

   if (me->base.piModel) {
      IRichTextModel *pirtm;
      
      if (SUCCESS == IMODEL_QueryInterface(me->base.piModel, AEEIID_RICHTEXTMODEL, (void **)&pirtm)) {
         // the user has told us to use a RichTextModel.
         ITextModel *pitm;
         if (SUCCESS == IRICHTEXTMODEL_QueryInterface(pirtm, AEEIID_TEXTMODEL, (void**)&pitm)) {
            TextInfo ti;
            ITEXTMODEL_GetTextInfo(pitm, &ti);
            *pnLen = ti.cchText;
            pwText = (AECHAR *)(ti.pwText);
            RELEASEIF(pitm);
         }
         RELEASEIF(pirtm);
      } else {
         // default model is a ValueModel.
         pwText = (AECHAR*)IVALUEMODEL_GetText((IValueModel*)(void*)me->base.piModel, pnLen);
         if (pwText && pnLen && *pnLen < 0) {
            *pnLen = WSTRLEN(pwText);
         }
      }
   } 
   return pwText;
}

static __inline AECHAR StaticWidget_GetEllipsisChar(StaticWidget *me) {
   AECHAR awEllipsis = me->awEllipsis;
   if (0 == awEllipsis) {
      // return the character we'll be using as the ellipsis
      awEllipsis = IShell_GetEllipsis(me->piShell);
   }
   return awEllipsis;
}

static __inline boolean StaticWidget_NeedsTruncation(StaticWidget* me) {
   boolean bDrawEllipsis = FALSE;

   if (!me->bAnimate || !me->nScrollOffset) {
      if ( me->dwFlags & SWF_WRAPTEXT ) {
         bDrawEllipsis = ( me->dwFlags & (SWF_MULTILINESHORTEN&~SWF_WRAPTEXT) ) ? 1 : 0;
      }
      else {
         bDrawEllipsis = ( me->dwFlags & SWF_NOSHORTENTEXT || me->dwFlags & SWF_WRAPNOBREAK || me->nScrollOffset ) ? 0 : 1;
      }
   }
   return bDrawEllipsis;
}
  

static __inline uint32 StaticWidget_DetermineTextLayoutFlags(StaticWidget* me) {

   uint32 dwTextLayoutFlags = 0;
   boolean bAnimate = me->bAnimate && (me->weFont.width > me->border.rcClient.dx);

   // include the IDF_* flags
   dwTextLayoutFlags |= me->dwFlags & SWF_FLAGSMASK;  

   if (!bAnimate) {

      if (me->dwFlags & SWF_WRAPTEXT){
         dwTextLayoutFlags |= TEXTLAYOUT_FLAG_MULTILINE | TEXTLAYOUT_FLAG_TRIMLINE | TEXTLAYOUT_FLAG_BREAKONWORD;
      } 
      else if (me->dwFlags & SWF_WRAPNOBREAK) {
         dwTextLayoutFlags |= TEXTLAYOUT_FLAG_MULTILINE | TEXTLAYOUT_FLAG_TRIMLINE;
      }

   }

   // ellipsify if we're shortening text
   if ( StaticWidget_NeedsTruncation(me) ) {
      dwTextLayoutFlags |= TEXTLAYOUT_FLAG_ELLIPSIFY;
   }

   dwTextLayoutFlags |= IDF_TEXT_TRANSPARENT;

   if ( ! (me->dwFlags & IDF_ALIGNHORZ_MASK) ) {
      dwTextLayoutFlags |= me->dwDefaultAlignment;
   }

   // for marquee scrolling, clear horizontal alignment bits
   if (bAnimate) {
      dwTextLayoutFlags &= ~IDF_ALIGNHORZ_MASK;
   }

   return dwTextLayoutFlags;
}

static __inline void StaticWidget_Invalidate(StaticWidget *me)
{
   WidgetBase_InvalidateContent(STATICWIDGET_TO_IWIDGET(me));
}

static __inline int StaticWidget_LayoutText(StaticWidget *me)
{
   int dwResult = SUCCESS;
   uint32 dwLayoutFlags = StaticWidget_DetermineTextLayoutFlags(me);
   if ( dwLayoutFlags != me->dwLayoutFlags ) {
      me->dwLayoutFlags = dwLayoutFlags;
      me->fLayoutText   = 1;
   }

   if (me->fLayoutText) {
      int ndx;
      RGBAVAL rgba;
      WExtent we;

      // determine size of area to lay out text into
      we.height = me->border.rcClient.dy;
      we.width = me->border.rcClient.dx;

      // chose proper (default) text color
      ndx = ((me->border.bActive != 0) ? 1 : 0 ) + (2 * ((me->border.bSelected != 0) ? 1 : 0));
      DynRGB_Get(&me->rgbText, ndx, &rgba);
      ITEXTLAYOUT_SetFGColor(me->piTextLayout, rgba);

      // set proper ellipsis character
      (void) ITEXTLAYOUT_SetEllipsis(me->piTextLayout, StaticWidget_GetEllipsisChar(me));
      // layout the text
      dwResult = ITEXTLAYOUT_Layout(me->piTextLayout, me->base.piModel, &we, me->dwLayoutFlags);
      me->fLayoutText = 0;
   }

   return dwResult;
}

// returns the height of all ines for the preferred width
static int StaticWidget_WrapToPreferredWidth(StaticWidget *me, int cxWidth)
{
   int dwResult = SUCCESS;
   int dwLayoutFlags = 0;
   int cyHeight = 10;

   // determine layout flags
   // determine layout flags
   dwLayoutFlags = StaticWidget_DetermineTextLayoutFlags(me);

   // default font and colors
   if (SUCCESS == dwResult) {
      dwResult = ITEXTLAYOUT_SetBGColor(me->piTextLayout, RGB_NONE);
   }
   if (SUCCESS == dwResult) {
      dwResult = ITEXTLAYOUT_SetFGColor(me->piTextLayout, RGB_NONE);
   }


   // get preferred extent
   if (SUCCESS == dwResult) {
      WExtent we;
      dwResult = ITEXTLAYOUT_GetPreferredExtent(me->piTextLayout, me->base.piModel, dwLayoutFlags, &we);
      if (SUCCESS == dwResult) {
         cyHeight = we.height;
      }
      me->fLayoutText = TRUE;
   }

   return cyHeight;
}

static void StaticWidget_CancelScrolling(StaticWidget *me)
{
   me->nScrollOffset = 0; 
   me->bAdjustScroll = FALSE;
   CALLBACK_Cancel(&me->cbkScroll);
}

static void StaticWidget_FontRecalc(StaticWidget *me)
{
   int nDefaultWidth  = 0;
   int nDefaultHeight = 12;

   // default extent
   SETWEXTENT(&me->weFont, nDefaultWidth, nDefaultHeight);  

   // what's the layout's new preferred width?
   ITEXTLAYOUT_SetHintRows(me->piTextLayout, 1);
   ITEXTLAYOUT_GetPreferredExtent(me->piTextLayout, me->base.piModel, 0, &me->weFont);
   me->fLayoutText = TRUE;
   StaticWidget_ResetScrollText(me);

}



static void StaticWidget_ScrollCallback(IWidget *po)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);
         
   if (me->weFont.width > me->border.rcClient.dx) {
      uint32 dwStepFlags = 0;

      me->nScrollOffset += me->stProps.nScrollAmount;

      if (me->bScrollReset) {
   
		   me->bAdjustScroll = FALSE;
         me->nScrollOffset = 0;
         me->nScrollDelay = me->stProps.nStartDelay;
         me->bScrollReset = FALSE;
         me->nCurStep = 0;

         if (me->stProps.nRepeat > 0 && me->stProps.nRepeat <= me->nScrollRepeat) {
            me->bAnimate = FALSE;
         }    
      } else if (me->nScrollOffset > (me->weFont.width - me->border.rcClient.dx)) {
   
         me->bAdjustScroll = TRUE;
         me->nScrollDelay = me->stProps.nEndDelay;
         me->bScrollReset = TRUE;
         dwStepFlags |= EVT_STEP_FINAL;

         if (me->stProps.nRepeat == 0 || (me->nScrollRepeat+1) < me->stProps.nRepeat) {
            dwStepFlags |= EVT_STEP_REPEAT;
         }
   
      } else {
		   me->bAdjustScroll = TRUE;
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
         (void)ISHELL_SetTimerEx(me->piShell, me->nScrollDelay, &me->cbkScroll);
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
         (void) ISHELL_SetTimerEx(me->piShell, me->nScrollDelay, &me->cbkScroll);
      }
   }
}

static void StaticWidget_FontChanged(StaticWidget *me, ModelEvent *pEvent)
{
   PARAM_NOT_REF(pEvent)   
   StaticWidget_FontRecalc(me);
   StaticWidget_Invalidate(me);
}


static void StaticWidget_UseFont(StaticWidget *me)
{
   LISTENER_Cancel(&me->mlFont);

   (void) ITEXTLAYOUT_SetFont(me->piTextLayout, me->piFont);

   if (me->piFont) {
      IModel *piModel = 0;
      
      // if font supports IModel, attach a listener for animated fonts
      if (SUCCESS == IFONT_QueryInterface(me->piFont, AEEIID_MODEL, (void**)&piModel)) {
         (void) IMODEL_AddListenerEx(piModel, &me->mlFont, (PFNLISTENER)StaticWidget_FontChanged, me);
         IMODEL_Release(piModel);
      }
   }

   // recalculate ...
   StaticWidget_FontRecalc(me);
   StaticWidget_Invalidate(me);
}


static void StaticWidget_FontMapModelChanged(StaticWidget *me, ModelEvent *pEvent)
{
   PARAM_NOT_REF(pEvent)   
   StaticWidget_FontRecalc(me);
   StaticWidget_Invalidate(me);
}


static void StaticWidget_UseFontMapModel(StaticWidget *me)
{
   LISTENER_Cancel(&me->mlFontMapModel);
   if (me->piFontMapModel) {
      IMODEL_AddListenerEx((IModel *)me->piFontMapModel, &me->mlFontMapModel, (PFNLISTENER)StaticWidget_FontMapModelChanged, me);
   }
   StaticWidget_FontMapModelChanged(me, NULL);
}

void StaticWidget_SetExtent(IWidget *po, WExtent *pe)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);

   // adjust text if we're scrolling so it's in the same relative position
   if (me->bAdjustScroll)
      me->nScrollOffset += (me->base.extent.width - pe->width);

   WidgetBase_SetExtent((IWidget*)(void*)&me->base, pe);
   Border_CalcClientRect(&me->border);
   me->fLayoutText = 1;
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
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);

   if ((me->dwFlags & SWF_WRAPTEXT) || (me->dwFlags & SWF_WRAPNOBREAK)) {
      WExtent weClient;  
   
      int cy = 0;
      int cx = me->cxHintWidth ? me->cxHintWidth : me->border.rcClient.dx;
      
      (void) ITEXTLAYOUT_SetHintWidth(me->piTextLayout, cx);
      (void) ITEXTLAYOUT_SetHintRows(me->piTextLayout, me->nHintRows);


      cx = MAX(10, cx);

      if (me->cxHintWidth) {
         cy = StaticWidget_WrapToPreferredWidth(me, cx);
      } else {
         WExtent we; 
         uint32 dwLayoutFlags = StaticWidget_DetermineTextLayoutFlags(me);  
         if (SUCCESS == ITEXTLAYOUT_GetPreferredExtent(me->piTextLayout, me->base.piModel, dwLayoutFlags, &we)) {
            cx = we.width;
            cy = we.height;
         }
      }

      cy = MAX(me->weFont.height, cy);

      weClient.height = cy;
      weClient.width  = cx;
      Border_CalcPreferredExtent(&me->border, pweOut, &weClient);  
      me->fLayoutText = TRUE;
   } 
   else {
      Border_CalcPreferredExtent(&me->border, pweOut, &me->weFont);
   }
}


void StaticWidget_DrawRectGetRect(IWidget *po, ICanvas *piCanvas, int x, int y, AEERect *prcOut)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);
   Border_Draw(&me->border, piCanvas, x, y);
   *prcOut = me->border.rcClient;
   prcOut->x += (int16) x;
   prcOut->y += (int16) y;
}



void StaticWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);
   AEERect rc;
   AEERect rcCanvasClip;
   AEERect rcWidgetClip;
   int nTotalLen;
   AECHAR *pwText = StaticWidget_GetText(me, &nTotalLen);
   WExtent weLayout;

   // layout text if it's become dirty
   StaticWidget_LayoutText(me);

   // determine where we'll draw the text
   StaticWidget_DrawRectGetRect(po, piCanvas, x, y, &rc);

   // Return if there is no text to draw, but only after above 
   // rectangle drawing (for background)
   if (!pwText) {
      return;
   }

   // align the text within the widget
   x = rc.x;
   y = rc.y;
   
   ITEXTLAYOUT_GetLayoutSize(me->piTextLayout, &weLayout, NULL);
   if ( (!(me->dwFlags & SWF_WRAPTEXT)) || 
        (!(StaticWidget_NeedsTruncation(me) == TRUE && rc.dy < weLayout.height)) ) {
      
      // virt-align if single-line or multi-line and no ellipsis

      if (me->dwFlags & IDF_ALIGN_MIDDLE) {
         y += (rc.dy - weLayout.height) / 2;
      } else if (me->dwFlags & IDF_ALIGN_BOTTOM) {
         y +=  rc.dy - weLayout.height;
      }

   }

   // draw the layout
   ICANVAS_GetClipRect(piCanvas, &rcCanvasClip);
   if (IntersectRect(&rcWidgetClip, &rcCanvasClip, &rc)) {

      ICANVAS_SetClipRect(piCanvas, &rcWidgetClip);
      (void)ITEXTLAYOUT_Draw(me->piTextLayout, piCanvas, x + ITEXTLAYOUT_ComputeScrollOffset(me->piTextLayout, me->weFont.width, me->border.rcClient.dx, me->nScrollOffset), y);
      ICANVAS_SetClipRect(piCanvas, &rcCanvasClip);

   }

}

// since we're transparent, return empty rect
boolean StaticWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}


static void StaticWidget_SetTextLayoutProps(StaticWidget* me) 
{
   RGBAVAL rgba;

   ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_LINEGAP, (uint32) me->nLineGap);
   ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_FONT, (uint32) me->piFont);
   ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_HINT_ROWS, (uint32) me->nHintRows);
   ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_HINT_WIDTH, (uint32) me->cxHintWidth);
   ITEXTLAYOUT_SetProperty(me->piTextLayout, PROP_FONTMAPMODEL, (uint32) me->piFontMapModel);

   (void) DynRGB_Get(&me->rgbText, RGBINDEX_ACTIVE, &rgba);
   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_ACTIVE_FGCOLOR, (uint32) rgba);

   (void) DynRGB_Get(&me->rgbText, RGBINDEX_SACTIVE, &rgba);
   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_SACTIVE_FGCOLOR, (uint32) rgba);

   (void) DynRGB_Get(&me->rgbText, RGBINDEX_SINACTIVE, &rgba);
   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_SINACTIVE_FGCOLOR, (uint32) rgba);

   (void) DynRGB_Get(&me->rgbText, RGBINDEX_INACTIVE, &rgba);
   ITEXTLAYOUT_SetProperty (me->piTextLayout, PROP_INACTIVE_FGCOLOR, (uint32) rgba);

   me->fLayoutText = 1;
}

#define SETIF(memb,var) \
   do { RELEASEIF(memb); (memb)=(var); ADDREFIF(memb); } while(0)

boolean StaticWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      me->fLayoutText = 1;
      return TRUE;
   }

   switch (evt) {
   
      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_FONT) {
            if ((IFont*)dwParam != me->piFont) {
               SETIF(me->piFont, (IFont*)dwParam);
               StaticWidget_UseFont(me);
            }

         } else if (wParam == PROP_FONT_CLASS) {
            IFont *piFont;
            if (SUCCESS == ISHELL_CreateFont(me->piShell, (AEECLSID)dwParam, &piFont)) {
               RELEASEIF(me->piFont);
               me->piFont = piFont;
               StaticWidget_UseFont(me);
            }

         } else if (wParam == PROP_FONT_OUTLINECOLOR) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               (void) IHFONT_SetFontOutlineColor(piHFont, (RGBVAL)dwParam);
               RELEASEIF(piHFont);
               StaticWidget_UseFont(me);
            } else {
               break;
            }

         } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               (void) IHFONT_SetFontOutlineWidth(piHFont, (int) dwParam);
               RELEASEIF(piHFont);
               StaticWidget_UseFont(me);
            } else {
               break;
            }      

         } else if (wParam == PROP_FLAGS) {
            me->fLayoutText = 1;
            me->dwFlags = dwParam;
      
         } else if (wParam == PROP_DEFAULTTEXTALIGNMENT) {
            me->fLayoutText = 1;
            me->dwDefaultAlignment = dwParam;
      
         } else if (wParam == PROP_FGCOLOR) {
            DynRGB_Collapse(&me->rgbText, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_SELECTED_FGCOLOR) {
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_SACTIVE_FGCOLOR) {
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_SINACTIVE_FGCOLOR) {
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_SINACTIVE, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_ACTIVE_FGCOLOR) {
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_ACTIVE, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_INACTIVE_FGCOLOR) {
            (void) DynRGB_Set(&me->rgbText, RGBINDEX_INACTIVE, (RGBVAL)dwParam);
            me->fLayoutText = 1;

         } else if (wParam == PROP_HINT_WIDTH) {
            me->cxHintWidth = (int)dwParam;

         } else if (wParam == PROP_HINT_ROWS) {
            me->nHintRows = (int)dwParam;

         } else if (wParam == PROP_LINEGAP) {
            me->nLineGap = (int)dwParam;  
            me->fLayoutText = 1;

         } else if (wParam == PROP_ANIMATE) {
            me->bAnimate = (boolean)dwParam;
            me->fLayoutText = 1;
            // if we turned off scrolling, we need to kill the callback
            if (!me->bAnimate) {
               StaticWidget_CancelScrolling(me);
               WidgetBase_Invalidate((IWidget*)(void*)me, ICIF_REDRAW);
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

         } else if (wParam == PROP_TEXTLAYOUTCLASSID) {
            if ((AEECLSID)dwParam != me->clsIdTextLayout) {
               int            nErr           = SUCCESS;
               ITextLayout*   piTextLayout   = NULL;

               nErr = ISHELL_CreateInstance(me->piShell, (AEECLSID)dwParam, (void**)&piTextLayout);
               if ((SUCCESS == nErr) && (NULL != piTextLayout)) {
                  RELEASEIF(me->piTextLayout);

                  me->clsIdTextLayout  = (AEECLSID)dwParam;
                  me->piTextLayout     = piTextLayout;
                  piTextLayout         = NULL;

                  StaticWidget_SetTextLayoutProps(me);
                  StaticWidget_Invalidate(me);
                  return TRUE;
               }
               else {
                  RELEASEIF(piTextLayout);
                  return FALSE;
               }
            }
      
         } else if (wParam == PROP_FONTMAPMODEL) {
            RELEASEIF(me->piFontMapModel);
            me->piFontMapModel = (IFontMapModel *)dwParam;
            ADDREFIF(me->piFontMapModel);
            StaticWidget_UseFontMapModel(me);

         } else if (wParam == PROP_ELLIPSIS) {
            me->awEllipsis = (AECHAR)dwParam;
            me->fLayoutText = 1;
            StaticWidget_Invalidate(me);
            return TRUE;

         } else {
            break;

         }
         ITEXTLAYOUT_SetProperty(me->piTextLayout, wParam, dwParam);
         StaticWidget_Invalidate(me);
         return TRUE;

      case EVT_WDG_GETPROPERTY: 
         if (wParam == PROP_FONT) {
            *(IFont **)dwParam = me->piFont;
            ADDREFIF(me->piFont);
         } else if (wParam == PROP_FONT_OUTLINECOLOR) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
               (void) IHFONT_GetFontOutlineColor(piHFont, (RGBVAL*)dwParam);
               RELEASEIF(piHFont);
            } else {
               break;
            }
         } else if (wParam == PROP_FONT_OUTLINEWIDTH) {
            IHFont *piHFont;
            if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont))  {
               (void) IHFONT_GetFontOutlineWidth(piHFont, (int*)dwParam);
               RELEASEIF(piHFont);
            } else {
               break;
            }
         } else if (wParam == PROP_FLAGS) {
            *(uint32 *)dwParam = me->dwFlags;

         } else if (wParam == PROP_DEFAULTTEXTALIGNMENT) {
            *(uint32 *)dwParam = me->dwDefaultAlignment;
   
         } else if (wParam == PROP_FGCOLOR || wParam == PROP_ACTIVE_FGCOLOR) {
            (void) DynRGB_Get(&me->rgbText, RGBINDEX_ACTIVE, (uint32*)dwParam);
      
         } else if (wParam == PROP_SELECTED_FGCOLOR || wParam == PROP_SACTIVE_FGCOLOR) {
            (void) DynRGB_Get(&me->rgbText, RGBINDEX_SACTIVE, (uint32*)dwParam);

         } else if (wParam == PROP_SINACTIVE_FGCOLOR) {
            (void) DynRGB_Get(&me->rgbText, RGBINDEX_SINACTIVE, (uint32*)dwParam);
      
         } else if (wParam == PROP_INACTIVE_FGCOLOR) {
            (void) DynRGB_Get(&me->rgbText, RGBINDEX_INACTIVE, (uint32*)dwParam);

         } else if (wParam == PROP_HINT_WIDTH) {
            *(int*)dwParam = me->cxHintWidth;

         } else if (wParam == PROP_HINT_ROWS) {
            *(int*)dwParam = me->nHintRows;

         } else if (wParam == PROP_LINEGAP) {
            *(int*)dwParam = me->nLineGap;

         } else if (wParam == PROP_ANIMATE) {
            *(int*)dwParam = me->bAnimate;

         } else if (wParam == PROP_ANIMATE_FLAGS) {
            *(uint32 *)dwParam = me->dwAnimFlags;

         } else if (wParam == PROP_TEXTLAYOUTCLASSID) {
            *(AEECLSID *)dwParam = me->clsIdTextLayout;

         } else if (wParam == PROP_FONTMAPMODEL) {
            *(IFontMapModel **)dwParam = me->piFontMapModel;
            ADDREFIF(me->piFontMapModel);

         } else if (wParam == PROP_ELLIPSIS) {
            *(AECHAR *)dwParam = StaticWidget_GetEllipsisChar(me);

         } else {
            break;

         }
         return TRUE;

      default:
         break;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


static 
void StaticWidget_ModelChanged(StaticWidget *me, ModelEvent *pEvent)
{
   if ((pEvent->evCode == EVT_MDL_VALUE) || (pEvent->evCode == EVT_MDL_TEXT_CHANGE)) {
      StaticWidget_FontRecalc(me);
      StaticWidget_Invalidate(me);
   }
}


int StaticWidget_SetModel(IWidget *po, IModel *piModel)
{
   IModel *pim = NULL;
   int nErr;
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);
   
   // allow NULL model
   if (!piModel) {
      return WidgetBase_SetModel(po, piModel);
   }

   // is this a value model?
   nErr = IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&pim);
   if (nErr == SUCCESS) {
      // register for notifications from this model
      nErr = WidgetBase_SetModel(po, piModel);
      if ( SUCCESS == nErr ) {
         nErr = IMODEL_AddListenerEx(piModel, &me->modelListener, (PFNLISTENER)StaticWidget_ModelChanged, me);
      }
      RELEASEIF(pim);

      StaticWidget_FontRecalc(me);
      StaticWidget_Invalidate(me);
      return nErr;
   }

   // is this a RichTextModel?
   nErr = IMODEL_QueryInterface(piModel, AEEIID_RICHTEXTMODEL, (void **)&pim);
   if (nErr == SUCCESS) {
      ITextModel *pitm;
      // register for notifications from the TextModel
      nErr = IMODEL_QueryInterface(piModel, AEEIID_TEXTMODEL, (void **)&pitm);
      if (SUCCESS == nErr) {
         nErr = WidgetBase_SetModel(po, piModel);
         if ( SUCCESS == nErr ) {
            nErr = IMODEL_AddListenerEx((IModel *)pitm, &me->modelListener, (PFNLISTENER)StaticWidget_ModelChanged, me);
         }
         RELEASEIF(pitm);
      }
      RELEASEIF(pim);

      StaticWidget_FontRecalc(me);
      StaticWidget_Invalidate(me);
   }

   return nErr;
}

uint32 StaticWidget_Release(IWidget *po)
{
   StaticWidget* me = IWIDGET_TO_STATICWIDGET(po);

   if (me->base.nRefs == 1) {
      StaticWidget_Dtor(me);
   }      

   return WidgetBase_Release(po);
}

int StaticWidget_Ctor(StaticWidget *me, AEEVTBL(IWidget) *pvt, IModel *piModel, 
                       IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   int nErr;
   WExtent we;

   WidgetBase_Ctor(&me->base, pvt, piModule, 
                   DEFHANDLER(StaticWidget_HandleEvent));
   
   pvt->Release            = StaticWidget_Release;
   pvt->GetPreferredExtent = StaticWidget_GetPreferredExtent;
   pvt->SetExtent          = StaticWidget_SetExtent;
   pvt->Draw               = StaticWidget_Draw;
   pvt->IntersectOpaque    = StaticWidget_IntersectOpaque;
   pvt->SetModel           = StaticWidget_SetModel;

   Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, FALSE, &WBASE(me)->piViewModel);
   DynRGB_Ctor(&me->rgbText, RGBA_BLACK);

   we.width  = 30;
   we.height = 12;
   Border_CalcPreferredExtent(&me->border, &we, &we);
   StaticWidget_SetExtent((IWidget*)(void*)&me->base, &we);
   
   // default to no border
   me->border.nWidth[0] = 
   me->border.nWidth[1] = 0;

   Border_SetBorderColor(&me->border, 0, RGBA_NONE);
   Border_SetBGColor(&me->border, 0, RGBA_NONE);
   
// other ctor stuff
   me->nLineGap = 0;

   me->dwFlags            = IDF_ALIGN_MIDDLE;

   me->dwDefaultAlignment = IDF_ALIGN_NONE;

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   // create the default TextLayout object
   me->clsIdTextLayout = AEECLSID_TEXTLAYOUT;
   nErr = ISHELL_CreateInstance(me->piShell, me->clsIdTextLayout, (void **)&me->piTextLayout);
   if (SUCCESS == nErr) {

      (void) ISHELL_CreateFont(piShell, AEECLSID_FONTSYSNORMAL, &me->piFont);
      StaticWidget_UseFont(me);
      
      (void) StaticWidget_SetModel(STATICWIDGET_TO_IWIDGET(me), piModel);

      // initialize scrolling
      me->stProps.nScrollAmount = DEFAULT_SCROLLAMOUNT;
      me->stProps.nStartDelay = DEFAULT_STARTDELAY;
      me->stProps.nScrollDelay = DEFAULT_SCROLLDELAY;
      me->stProps.nEndDelay = DEFAULT_ENDDELAY;
      me->stProps.nRepeat = DEFAULT_REPEAT;

      me->dwAnimFlags = 0;

      CALLBACK_Init(&me->cbkScroll, StaticWidget_ScrollCallback, STATICWIDGET_TO_IWIDGET(me));
   }

   return nErr;
}

void StaticWidget_Dtor(StaticWidget *me)
{
   CALLBACK_Cancel(&me->cbkScroll);
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Cancel(&me->mlFont);
   LISTENER_Cancel(&me->mlFontMapModel);
   RELEASEIF(me->piFont);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piTextLayout);
   RELEASEIF(me->piFontMapModel);
   Border_Dtor(&me->border);
   DynRGB_Dtor(&me->rgbText);
}

int StaticWidget_New(IWidget **ppo, IModel *piModel, IShell *piShell, IModule *piModule)
{
   int nErr = SUCCESS;
   
   if (!piModel) {
      nErr = ValueModel_New((IValueModel**)(void**)&piModel, piModule);
   } else {
      IMODEL_AddRef(piModel);
   }

   if (nErr == SUCCESS) {
      
      StaticWidget *me = MALLOCREC_VTBL(StaticWidget, IWidget);

      if (me) {
         nErr = StaticWidget_Ctor(me, GETVTBL(me, IWidget), piModel, piShell, piModule, 0);
         if (SUCCESS == nErr) {
            *ppo = STATICWIDGET_TO_IWIDGET(me);
         } else {
            StaticWidget_Dtor(me);
         }
      } else {
         nErr = ENOMEMORY;
      }
   } //lint !e429 


   RELEASEIF(piModel);

   return nErr;
}


