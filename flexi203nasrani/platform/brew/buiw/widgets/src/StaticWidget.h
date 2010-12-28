/*
  ========================================================================

  FILE:  StaticWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines a static, label-like widget
  
  Properties:

  PROP_FONT
  PROP_TEXTFLAGS
  PROP_FONT
  PROP_FGCOLOR
  PROP_BGCOLOR
  PROP_TRANSPARENCY

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __STATICWIDGET_H__
#define __STATICWIDGET_H__

#include "AEEStaticWidget.h"
#include "AEEWModel.h"
#include "AEEFontBidiUtil.h"
#include "WidgetBase.h"
#include "AEEFont.h"
#include "Border.h"
#include "AEEFontMapModel.h"
#include "AEETextLayout.h"

typedef struct StaticWidget {
   WidgetBase     base;
   ModelListener  modelListener;
   Border         border;
   IFont *        piFont;
   ModelListener  mlFont;              // model listener for animated fonts
   ModelListener  mlFontMapModel;      // model listener for FontMapModel
   WExtent        weFont;              // cached size of string, measured with font
   DynRGB         rgbText;
   WExtent        wePrefExtent;
   uint32         dwFlags;
   uint32         dwDefaultAlignment;
   flg            fLayoutText;         // layout the text
   int            cxHintWidth;
   int            nHintRows;
   int            nLineGap;
   IShell         *piShell;            // For font creation

   // text layout and drawing
   AEECLSID       clsIdTextLayout;     // class id of TextLayout object to use
   ITextLayout    *piTextLayout;       // TexyLayout to layout and draw text
   IFontMapModel  *piFontMapModel;     // FontMapModel to interpret text styles
   AECHAR         awEllipsis;          // AECHAR to use for ellipsis truncation

   uint32         dwLayoutFlags;       // flags used during the last layout
   // scrolling
   boolean        bAnimate;
   boolean        bScrollReset;
   boolean        bAdjustScroll;
   AEECallback    cbkScroll;           // Callback for scrolling
   int            nScrollOffset;       // Text offset 
   int            nScrollDelay;        // Scroll delay
   uint32         nScrollRepeat;       // Current repeat
   uint32         nCurStep;            // Current step
   ScrollText     stProps;             // ScrollText properties
   uint32         dwAnimFlags;         // Animation specific flags

} StaticWidget;


int      StaticWidget_New  (IWidget **ppo, IModel *piModel, IShell *piShell, IModule *piModule);
int      StaticWidget_Ctor (StaticWidget *me, AEEVTBL(IWidget) *pvt, IModel *piModel, IShell *piShell, 
                            IModule *piModule, PFNHANDLER pfnDefHandler);
void     StaticWidget_Dtor (StaticWidget *me);

// Overriden widget members in this implementation
uint32   StaticWidget_Release             (IWidget *po);
boolean  StaticWidget_HandleEvent         (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     StaticWidget_GetPreferredExtent  (IWidget *po, WExtent *pweOut);
void     StaticWidget_SetExtent           (IWidget *po, WExtent *pe);
void     StaticWidget_Draw                (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  StaticWidget_IntersectOpaque     (IWidget *po, AEERect *prcOut, const AEERect *prcIn);
int      StaticWidget_SetModel            (IWidget *po, IModel *piModel);

// Utility funcs for derived classes
void     StaticWidget_DrawText   (IWidget *po, IFont *piFont, const AECHAR *pwszText, int nLen, IDisplay *piDisplay, int x, int y, 
                                  AEERect *prcClip, uint32 dwFlags, boolean bAnimate);
void     StaticWidget_DrawRectGetRect(IWidget *po, ICanvas *piCanvas, int x, int y, AEERect *prcOut);
int      StaticWidget_DrawTextColor (IWidget *po, IFont *piFont, const AECHAR *pwszText, int nLen, IDisplay *piDisplay, int x, int y, 
                                     AEERect *prcClip, RGBVAL rgbText, uint32 dwFlags, boolean bAnimate);

#endif //__STATICWIDGET_H__

