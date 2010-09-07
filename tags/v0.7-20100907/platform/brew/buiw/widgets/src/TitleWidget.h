/*
  ========================================================================

  FILE:  TitleWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Decorator that wraps a title on top of another 
  widget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __TITLEWIDGET_H__
#define __TITLEWIDGET_H__

#include "AEETitleWidget.h"
#include "DecoratorWidget.h"
#include "Border.h"

typedef struct TitleWidget {
   Decorator base;
   
   Border   border;
   IWidget *piTitle;

   flg      fInLayout : 1;
} TitleWidget;

static __inline IDecorator *TITLEWIDGET_TO_IDECORATOR(TitleWidget *me) {
   return (IDecorator *)me;
}

static __inline IWidget *TITLEWIDGET_TO_IWIDGET(TitleWidget *me) {
   return (IWidget *)me;
}

int  TitleWidget_New  (IDecorator **ppo, IShell *piShell, IModule *piModule);
void TitleWidget_Ctor (TitleWidget *me, AEEVTBL(IDecorator) *pvt, 
                       IWidget *piTitle, IShell *piShell, IModule *piModule, 
                       PFNHANDLER pfnDefHandler);

// Widget methods
boolean TitleWidget_HandleEvent        (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
boolean TitleWidget_IntersectOpaque    (IDecorator *po, AEERect *prcOut, const AEERect *prcIn);
void    TitleWidget_Draw               (IDecorator *po, ICanvas *piCanvas, int x, int y);
void    TitleWidget_GetPreferredExtent (IDecorator *po, WExtent *pweOut);
void    TitleWidget_GetExtent          (IDecorator *po, WExtent *pe);
void    TitleWidget_SetExtent          (IDecorator *po, WExtent *pe);
void    TitleWidget_SetWidget          (IDecorator *po, IWidget *piWidget);
int     TitleWidget_QueryInterface     (IDecorator *po, AEECLSID clsid, void **ppo);

// Container methods
void TitleWidget_Invalidate (IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags);
int  TitleWidget_Locate     (IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel);


#endif /* __TITLEWIDGET_H__ */
