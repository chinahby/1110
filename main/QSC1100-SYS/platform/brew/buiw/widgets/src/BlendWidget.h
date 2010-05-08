/*
  ========================================================================

  FILE:  BlendWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: BlendWidget blends its child into the background
  
  Properties:
  PROP_TRANSPARENCY
  PROP_ALPHA
  PROP_BUFFERED

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _BLENDWIDGET_H_
#define _BLENDWIDGET_H_

#include "AEEBlendWidget.h"
#include "AEEBitmap.h"

#include "DecoratorWidget.h"

typedef struct BlendWidget {
   Decorator   base;

   int nOpacity;
   IBitmap* pibBuffer;
   flg      fIsBuffered   : 1;
   flg      fInvalidBuffer: 1;
   flg      fSetExtent : 1;
} BlendWidget;


static __inline IDecorator *BLENDWIDGET_TO_IDECORATOR(BlendWidget *p) {
   return (IDecorator *)p;
}

static __inline IWidget *BLENDWIDGET_TO_IWIDGET(BlendWidget *p) {
   return (IWidget *)p;
}

int     BlendWidget_New         (IDecorator **ppiw, IModule *piModule);
void    BlendWidget_Ctor        (BlendWidget *me, AEEVTBL(IDecorator) *pvt, 
                                 IModule *piModule, PFNHANDLER pfnDefHandler);
uint32  BlendWidget_Release     (IDecorator *po);

boolean BlendWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
boolean BlendWidget_IntersectOpaque(IDecorator *po, AEERect *prc, const AEERect *prcTest);
void    BlendWidget_Draw        (IDecorator *po, ICanvas *piCanvas, int x, int y);

int IWidget_WrapInBlender(IWidget **ppioChild, IModule *piModule);

#endif // _BLENDWIDGET_H_
