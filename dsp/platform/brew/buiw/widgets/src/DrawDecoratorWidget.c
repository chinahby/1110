/*======================================================
FILE:       DrawDecoratorWidget.c

SERVICES:   Widget that draws borders and/or backgrounds

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdLib.h"

#include "DrawDecoratorWidget.h"
#include "AEEWProperties.h"
#include "wutil.h"



//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------


#define DRAWDECORATORWIDGET_FROM_DRAWDECORATOR DrawDecoratorWidget *me = (DrawDecoratorWidget *)po

uint32 DrawDecoratorWidget_Release(IDrawDecorator *po)
{
   DRAWDECORATORWIDGET_FROM_DRAWDECORATOR;

   if (1 == me->base.base.nRefs) {
      DrawDecoratorWidget_Dtor(me);
   }

   return WidgetBase_Release((IWidget *) po);
}


void DrawDecoratorWidget_GetExtent(IDrawDecorator *po, WExtent *pe)
{
   //
   // The DrawDecoratorWidget must manage its own extent.  Its base class,
   // DecoratorWidget, will ignore GetExtent/SetExtent if it does not have
   // a child widget.   We want the DrawDecoratorWidget to have an extent,
   // even if it has no child widget.
   //

   // Don't ask child, we control the extent
   WidgetBase_GetExtent(IDRAWDECORATOR_TO_IWIDGET(po), pe);
}


void DrawDecoratorWidget_SetExtent(IDrawDecorator *po, WExtent *pe)
{
   //
   // The DrawDecoratorWidget must manage its own extent.  Its base class,
   // DecoratorWidget, will ignore GetExtent/SetExtent if it does not have
   // a child widget.   We want the DrawDecoratorWidget to have an extent,
   // even if it has no child widget.
   //

   // Set our extent
   WidgetBase_SetExtent(IDRAWDECORATOR_TO_IWIDGET(po), pe);

   // Set our child's extent (if we have a child)
   Decorator_SetExtent(IDRAWDECORATOR_TO_IDECORATOR(po), pe);
}

void DrawDecoratorWidget_DefDraw(DrawDecoratorWidget *me, ICanvas *piCanvas, int x, int y)
{
   Decorator_Draw(IDRAWDECORATOR_TO_IDECORATOR(DRAWDECORATORWIDGET_TO_IDRAWDECORATOR(me)), piCanvas, x, y);
}

void DrawDecoratorWidget_Draw(IDrawDecorator *po, ICanvas *piCanvas, int x, int y)
{
   DRAWDECORATORWIDGET_FROM_DRAWDECORATOR;

   DRAWHANDLERDESC_Call(&me->drawHandler, piCanvas, x, y);
}


void DrawDecoratorWidget_SetDraw(IDrawDecorator *po, DrawHandlerDesc *pDesc)
{
   DRAWDECORATORWIDGET_FROM_DRAWDECORATOR;

   // Reset on null descriptor
   if (!pDesc) {
      DRAWHANDLERDESC_Init(&me->drawHandler, DrawDecoratorWidget_DefDraw, me, 0);
   } else {
      DrawHandlerDesc temp = *pDesc;
      *pDesc = me->drawHandler;
      me->drawHandler = temp;
   }
}

void DrawDecoratorWidget_SetWidget(IDrawDecorator *po, IWidget *piChild)
{
   WExtent we;

   Decorator_SetWidget((IDecorator*)po, piChild);
   Decorator_GetExtent((IDecorator*)po, &we);
   WidgetBase_SetExtent(IDRAWDECORATOR_TO_IWIDGET(po), &we);
}


void DrawDecoratorWidget_Ctor(DrawDecoratorWidget *me, AEEVTBL(IDrawDecorator) *pvt, 
                              IModule *piModule, PFNHANDLER pfnDefHandler)
{
   Decorator_Ctor(&me->base, (AEEVTBL(IDecorator) *) pvt, piModule, DEFHANDLER(Decorator_HandleEvent));

   pvt->Release   = DrawDecoratorWidget_Release;
   pvt->GetExtent = DrawDecoratorWidget_GetExtent;
   pvt->SetExtent = DrawDecoratorWidget_SetExtent;
   pvt->Draw      = DrawDecoratorWidget_Draw;
   pvt->SetDraw   = DrawDecoratorWidget_SetDraw;
   pvt->SetWidget = DrawDecoratorWidget_SetWidget;

   DRAWHANDLERDESC_Init(&me->drawHandler, DrawDecoratorWidget_DefDraw, me, 0);
}

void DrawDecoratorWidget_Dtor(DrawDecoratorWidget *me)
{
   // Call free hook for handler cxt
   DRAWHANDLERDESC_Free(&me->drawHandler);

   Decorator_Dtor(&me->base);
}

int DrawDecoratorWidget_New(IDrawDecorator **ppo, IModule *piModule)
{
   DrawDecoratorWidget *me = MALLOCREC_VTBL(DrawDecoratorWidget, IDrawDecorator);

   if (!me)
      return ENOMEMORY;

   DrawDecoratorWidget_Ctor(me, GETVTBL(me, IDrawDecorator), piModule, 0);
   *ppo = DRAWDECORATORWIDGET_TO_IDRAWDECORATOR(me);

   return 0;
}

