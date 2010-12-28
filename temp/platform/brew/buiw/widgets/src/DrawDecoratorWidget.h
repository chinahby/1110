/*
  ========================================================================

  FILE:  DrawDecoratorWidget.h
  
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
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _DRAWDECORATORWIDGET_H_
#define _DRAWDECORATORWIDGET_H_

#include "AEEDrawDecoratorWidget.h"
#include "DecoratorWidget.h"

typedef struct DrawDecoratorWidget {
   Decorator       base;

   DrawHandlerDesc drawHandler;
} DrawDecoratorWidget;


static __inline IDrawDecorator *DRAWDECORATORWIDGET_TO_IDRAWDECORATOR(DrawDecoratorWidget *p) {
   return (IDrawDecorator *)p;
}

static __inline IWidget *DRAWDECORATORWIDGET_TO_IWIDGET(DrawDecoratorWidget *p) {
   return (IWidget *)p;
}


int DrawDecoratorWidget_New  (IDrawDecorator **ppo, IModule *piModule);
void DrawDecoratorWidget_Ctor(DrawDecoratorWidget *me, AEEVTBL(IDrawDecorator) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler);
void DrawDecoratorWidget_Dtor(DrawDecoratorWidget *me);

void DrawDecoratorWidget_DefDraw(DrawDecoratorWidget *me, ICanvas *piCanvas, int x, int y);

// DrawDecoratorWidget methods
uint32 DrawDecoratorWidget_Release(IDrawDecorator *po);
void DrawDecoratorWidget_GetExtent(IDrawDecorator *po, WExtent *pe);
void DrawDecoratorWidget_SetExtent(IDrawDecorator *po, WExtent *pe);
void DrawDecoratorWidget_Draw(IDrawDecorator *po, ICanvas *piCanvas, int x, int y);
void DrawDecoratorWidget_SetDraw(IDrawDecorator *po, DrawHandlerDesc *pDesc);



#endif // _DRAWDECORATORWIDGET_H_
