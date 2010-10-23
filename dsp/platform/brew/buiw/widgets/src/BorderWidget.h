/*
  ========================================================================

  FILE:  BorderWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: General border decorator

  Understood properties:
  PROP_TRANSPARENCY
  PROP_BORDERWIDTH
  PROP_BORDERRADIUS
  PROP_BGCOLOR
  PROP_BORDERCOLOR
  PROP_PADDING
  PROP_*_PAD

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _BORDERWIDGET_H_
#define _BORDERWIDGET_H_

#include "AEEBorderWidget.h"
#include "DecoratorWidget.h"
#include "Border.h"

typedef struct BorderWidget {
   Decorator   base;

   Border      border;
} BorderWidget;


static __inline IDecorator *BORDERWIDGET_TO_IDECORATOR(BorderWidget *p) {
   return (IDecorator *)p;
}

static __inline IWidget *BORDERWIDGET_TO_IWIDGET(BorderWidget *p) {
   return (IWidget *)p;
}

int  BorderWidget_New  (IDecorator **ppo, IModule *piModule);
void BorderWidget_Ctor (BorderWidget *me, AEEVTBL(IDecorator) *pvt, 
                        IModule *piModule, PFNHANDLER pfnDefHandler);
void BorderWidget_Dtor (BorderWidget *me);

// Widget methods
uint32   BorderWidget_Release        (IDecorator *po);
boolean  BorderWidget_HandleEvent    (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
boolean  BorderWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, const AEERect *prcIn);
void BorderWidget_Draw              (IDecorator *po, ICanvas *piCanvas, int x, int y);
void BorderWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut);
void BorderWidget_GetExtent         (IDecorator *po, WExtent *pe);
void BorderWidget_SetExtent         (IDecorator *po, WExtent *pe);
void BorderWidget_SetWidget         (IDecorator *po, IWidget *piWidget);

// Container methods
void BorderWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prcIn, uint32 dwFlags);
int BorderWidget_Locate      (IContainer *po, IWidget *piw, IContainer **ppRoot, AEERect *prcRel);



/////////////////////////////////////////////////////////////////
// Utility functions

int IWidget_WrapInBorder(IWidget **ppioChild, IModule *piModule);

#endif // _BORDERWIDGET_H_
