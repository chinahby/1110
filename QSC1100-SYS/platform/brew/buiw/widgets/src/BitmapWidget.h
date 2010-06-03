 /*======================================================
FILE:       BitmapWidget.h

SERVICES:   Widget that draws bitmaps in 

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __BITMAPWIDGET_H__
#define __BITMAPWIDGET_H__

#include "AEEBitmapWidget.h"

#include "WidgetBase.h"
#include "Border.h"

typedef struct BitmapWidget BitmapWidget;

struct BitmapWidget {
   WidgetBase     base;
   ModelListener  modelListener;
   IBitmap *      piBitmap;
   RGBVAL         rgbTransp;
   uint32         dwFlags;
   boolean        bActive;
   Border         border;
};

static __inline IWidget *BITMAPWIDGET_TO_IWIDGET(BitmapWidget *me) { 
   return (IWidget *) me; 
}

void     BitmapWidget_GetPreferredExtent  (IWidget *po, WExtent *pweOut);
void     BitmapWidget_SetExtent           (IWidget *po, WExtent *we);
int      BitmapWidget_SetModel            (IWidget *po, IModel *piModel);
void     BitmapWidget_Draw                (IWidget *po, ICanvas *piCanvas, int x, int y);
boolean  BitmapWidget_IntersectOpaque     (IWidget *po, AEERect *prcOut, const AEERect *prcIn);
uint32   BitmapWidget_Release             (IWidget *po);
boolean  BitmapWidget_HandleEvent         (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     BitmapWidget_Ctor                (BitmapWidget *me, AEEVTBL(IWidget) *pvt, 
                                           IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
void     BitmapWidget_Dtor                (BitmapWidget *me);
int      BitmapWidget_Initialize          (BitmapWidget *me, IShell *piShell);
int      BitmapWidget_New                 (IWidget **ppo, IShell *piShell,IModule *piModule);


#endif   //__BITMAPWIDGET_H__

