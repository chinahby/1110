/*
  ========================================================================

  FILE:  DecoratorWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines the decorator base interface and class. 

  A decorator is a container that wraps a single widget to which it 
  defers all drawing and events. The implementation is an abstract base
  class to be re-used by other widget implementations

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#ifndef __DECORATORWIDGET_H__
#define __DECORATORWIDGET_H__

#include "AEEDecorator.h"
#include "AEEBase.h"
#include "WidgetBase.h"

typedef struct Decorator Decorator;

struct IContainer {
   AEEBASE_INHERIT(IContainer, Decorator);
};

struct Decorator {
   WidgetBase base;

   IContainer container;
   AEEVTBL(IContainer) vtContainer;

   IWidget *pChild;
};

static __inline IWidget *DECORATOR_TO_IWIDGET(Decorator *p) {
   return (IWidget *)p;
}

void Decorator_Ctor(Decorator *me, AEEVTBL(IDecorator) *pvt, IModule *piModule, 
                    PFNHANDLER pfnDefHandler);

void Decorator_Dtor(Decorator *me);

// IContainer methods
void Decorator_Invalidate  (IDecorator *po, const AEERect *prc, uint32 dwFlags);
int  Decorator_Locate      (IDecorator *po, IContainer **ppic, AEERect *prc);
void Decorator_ClientInvalidate(IDecorator *po, const AEERect *prcClient, 
                                const AEERect *prcIn, uint32 dwFlags);
int Decorator_ClientLocate (IDecorator *po, const AEERect *prcClient, IWidget *piw, 
                            IContainer **ppic, AEERect *prc);


// IWidget methods
uint32  Decorator_Release           (IDecorator *po);
int     Decorator_QueryInterface    (IDecorator *po, AEECLSID clsid, void **ppo);
void    Decorator_Draw              (IDecorator *po, ICanvas *piCanvas, int x, int y);
boolean Decorator_IntersectOpaque   (IDecorator *po, AEERect *prc, const AEERect *prcTest);
boolean Decorator_HandleEvent       (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void    Decorator_GetPreferredExtent(IDecorator *po, WExtent *pe);
void    Decorator_GetExtent         (IDecorator *po, WExtent *pe);
void    Decorator_SetExtent         (IDecorator *po, WExtent *pe);
void    Decorator_GetController     (IDecorator *po, IController **ppoc);
int     Decorator_SetController     (IDecorator *po, IController *pc);

// IDecorator specific methods
void Decorator_SetWidget(IDecorator *po, IWidget *pChild);
void Decorator_GetWidget(IDecorator *po, IWidget **ppo);

#endif /* __DECORATORWIDGET_H__ */
