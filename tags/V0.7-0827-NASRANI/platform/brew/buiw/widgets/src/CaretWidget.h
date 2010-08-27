/*======================================================
FILE: CaretWidget.h

SERVICES: 
   IWidget implementation for text caret

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#ifndef __CARETWIDGET_H__
#define __CARETWIDGET_H__


#include "AEECaretWidget.h"


typedef struct {
   WidgetBase		base;
   IDIB *			piDibCaret;
   RGBVAL         rgbFG;
   RGBVAL         rgbBG;
   IShell *       piShell;
   IWidget *      piw;

   int            nDir;
	int            nBlend;
	int            nInc;       // current increment
	int            nMin;       // minimum blend amount (max is always 100)
	int            msTimeOn;   // time in ms that caret is on full
	int            msFade;     // pulse interval
	int            msTime;
	AEECallback    cbk;
	IContainer *   piContainer;
	AEERect        rcCaret;
   flg            bStepEnabled:1;
   int            nPulseCount;
   int            nPulseRep;
   flg            bLastFrame:1;
} CaretWidget;


int      CaretWidget_New                  (IWidget **ppo, IShell *piShell, IModule *piModule);
void     CaretWidget_Ctor                 (CaretWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
void     CaretWidget_Dtor                 (CaretWidget *me);
boolean  CaretWidget_HandleEvent          (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32   CaretWidget_AddRef               (IWidget *po);
uint32   CaretWidget_Release              (IWidget *po);
void     CaretWidget_CreateCaret          (IWidget *po, IDisplay *piDisplay);
void     CaretWidget_Draw                 (IWidget *po, ICanvas *piCanvas, int x, int y);
void     CaretWidget_GetPreferredExtent   (IWidget *po, WExtent *pweOut);
void     CaretWidget_SetExtent            (IWidget *po, WExtent *pWExtent);
boolean  CaretWidget_IntersectOpaque      (IWidget *po, AEERect *prcOut, const AEERect *prcIn);

#endif   //__CARETWIDGET_H__