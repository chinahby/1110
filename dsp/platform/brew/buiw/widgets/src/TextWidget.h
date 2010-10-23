/*======================================================
FILE: TextWidget.h

SERVICES: 
   IWidget implementation for text entry

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __TEXTWIDGET_H__
#define __TEXTWIDGET_H__

#include "AEETextWidget.h"
#include "AEETextModel.h"
#include "WidgetBase.h"
#include "LayoutText.h"
#include "Border.h"


typedef struct {
   WidgetBase     base;
   Border         border;
   ModelListener  modelListener;
   
   // interface ptrs
   IFont *        piFont;
   ModelListener  mlFont;        // model listener for animated fonts
   ITextModel *   piTextModel;
   IShell *       piShell;

   IController *  piController;  // installed controller
   
   // text/selection colors
   DynRGB         rgbText;       // text color, indexed by RGBINDEX_xxx (see Border.h)
   RGBVAL         rgbaSelected[2]; // selection BG color, inactive[0], active[1], (w/ alpha component)

   int            cyChar;        // height of char in current font
   int            nLineGap;      // line height adjustment
   int            nVisibleLines; // number of complete visible lines (dep. on cyChar and extent)
   int            xScrollPos;    // x offset of scroll position;
   uint32         dwFlags;       // text widget specific flags (TWF_xxx)
   int            xCaret;        // last x position of caret (in widget coords)
   int            yCaret;        // last y position of caret (in widget coords) 

   // multi-line mode stuff
   LayoutText*    poLayoutText;

   int            nScrollLine;   // line number at current scroll position
   int            nLines;        // calculated by oLineTbl
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
   int            nCurrentLine;

   IWidget *       piCaret;

   // password masking
   int16          nMaskChars;    // number of masked characters (password mode)
   int16          nMaskTimeout;
   AECHAR *       pwMaskBuf;     // mask buffer;
   int            cbMaskBuf;     // sizeof mask buffer;
   ModelListener  mlMT;          // model listener for multi-tap timeout
   AEECallback    cbkMask;       // callback for mask timer
   AECHAR         wchMask;       // character to use for masking

   int            nChangePos;    // position at which last change occurred
   TextInfo       ti;

   int            nSelRect;
   AEERect*       paSelRect;

} TextWidget;


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

