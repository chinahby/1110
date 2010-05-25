/*======================================================
FILE: TextWidget.h

SERVICES: 
   IWidget implementation for text entry

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __TEXTWIDGET_H__
#define __TEXTWIDGET_H__

#include "AEETextWidget.h"
#include "AEETextModel.h"
#include "AEEFontMapModel.h"
#include "AEETextLayout.h"
#include "WidgetContBase.h"
#include "Border.h"


typedef struct TextWidget TextWidget;

struct TextWidget{
   WidgetContBase base;
   Border         border;
   ModelListener  modelListener;

   // interface ptrs
   IFont *        piFont;
   ModelListener  mlFont;        // model listener for animated fonts
   ModelListener  mlFontMapModel;// model listener for FontMapModel
   ITextModel *   piTextModel;
   IShell *       piShell;

   ITextModel *   piPasswordModel;  // model used to store password mask

   IController *  piController;  // installed controller
   
   // text/selection colors
   DynRGB         rgbText;       // text color, indexed by RGBINDEX_xxx (see Border.h)
   RGBVAL         rgbsText[2];   // text selection FG color, inactive[0], active[1]
   RGBVAL         rgbaSelected[2]; // selection BG color, inactive[0], active[1], (w/ alpha component)

   int            cyChar;        // height of char in current font
   int            nLineGap;      // line height adjustment
   int            nVisibleLines; // number of complete visible lines (dep. on cyChar and extent)
   int            xScrollPos;    // x offset of scroll position;
   int            yScrollPos;    // y offset of scroll position - used for rich text
   uint32         dwFlags;       // text widget specific flags (TWF_xxx)

   int            xCaret;        // last x position of caret (in widget coords)
   int            yCaret;        // last y position of caret (in widget coords) 

   ITextLayout*   piTextLayout;
   AEECLSID       clsidTextLayout;

   boolean        fLayoutText;
   RGBAVAL        rgbaLayoutFG;
   RGBAVAL        rgbaLayoutSelFG;
   RGBAVAL        rgbaLayoutSelBG;
   uint32         dwLayoutFlags;
   AECHAR         awEllipsis;    // AECHAR to use for ellipsis when shortening text

   IFontMapModel* piFontMapModel;

   ScrollEvent    se;            // last scrollevent raised

   int            nLines;        // number of lines in layout
   int            nScrollLine;   // line number at current scroll position
   uint32         posUpdate;     // accumulated, lowest update position sent by model
   boolean        fRewrap;

   AEECallback    cbkUpdate;     // callback used to update the line table
   AEECallback    cbkNotify;     // callback used to notify of scroll events
                                 // (to avoid problem when sending notify directly from _Draw)
   AEERect        rcCaret;
   int            xDraw;
   int            yDraw;

   int            nHintRows;
   int            cxHintWidth;
   uint32         dwCaretLineParam;

   IWidget *       piCaret;

   // password masking
   int16          nMaskChars;      // number of masked characters (password mode)
   int16          nMaskTimeout;
   AECHAR *       pwMaskBuf;       // mask buffer;
   int            cbMaskBuf;       // sizeof mask buffer;
   ModelListener  mlViewListener;  // model listener for multi-tap timeout
   AEECallback    cbkMask;         // callback for mask timer
   AECHAR         wchMask;         // character to use for masking

   int            nChangePos;      // position at which last change occurred
   TextInfo       ti;

};


int      TextWidget_New    (IWidget **ppo, ITextModel *piValueModel, IShell *piShell, 
                            IModule *piModule);

void     TextWidget_Ctor   (TextWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, 
                            IModule *piModule, PFNHANDLER pfnDefHandler);

int      TextWidget_Initialize(TextWidget *me);

uint32   TextWidget_Release            (IWidget *po);
boolean  TextWidget_HandleEvent        (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     TextWidget_GetPreferredExtent (IWidget *po, WExtent *pweOut);
void     TextWidget_SetExtent          (IWidget *po, WExtent *pe);
void     TextWidget_Draw               (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  TextWidget_IntersectOpaque    (IWidget *po, AEERect *prcOut, const AEERect *prcIn);
int      TextWidget_SetModel           (IWidget *po, IModel *piModel);
      

#endif   //__TEXTWIDGET_H__

