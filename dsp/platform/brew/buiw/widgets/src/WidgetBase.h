/*
  ========================================================================

  FILE:  AEEWidgetBase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation for models

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEWIDGETBASE_H__
#define __AEEWIDGETBASE_H__

#include "AEEWidget.h"

typedef struct WidgetBase      WidgetBase;

struct WidgetBase {
   const AEEVTBL(IWidget) *pvt;

   int               nRefs;
   IModule *         piModule;

   WExtent           extent;
   IContainer *      piContainer;
   IModel *          piModel;

   HandlerDesc       hd;
   PFNHANDLER        pfnDefHandler;
   
   // view model
   IModel *          piViewModel;
};

#define WBASE(p) ((WidgetBase *)p)

// macro for conveniently assigning the pfnDefHandler in the xxxWidget_Ctor
#define DEFHANDLER(mpfn)      (pfnDefHandler ? pfnDefHandler : (PFNHANDLER)(mpfn))

extern void WidgetBase_InitVT(AEEVTBL(IWidget) *pvt);
extern void WidgetBase_Ctor(WidgetBase *me, AEEVTBL(IWidget) *pvt, 
                            IModule *piModule, PFNHANDLER pfnDefHandler);
extern void WidgetBase_Dtor(WidgetBase *me);

// IWidget functions:  Derived objects can delegate to these functions

uint32   WidgetBase_AddRef(IWidget *me);
uint32   WidgetBase_Release(IWidget *me);
int      WidgetBase_QueryInterface(IWidget *po, AEECLSID id, void **ppNew);

// note: WidgetBase_HandleEvent should not be overridden in the vtable.
//       Derived classes must supply a default handler in WidgetBase_Ctor

boolean  WidgetBase_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     WidgetBase_GetPreferredExtent(IWidget *po, WExtent *pweOut);
void     WidgetBase_GetExtent(IWidget *me, WExtent *pWExtent);
void     WidgetBase_SetExtent(IWidget *me, WExtent *pWExtent);
void     WidgetBase_GetParent(IWidget *me, IContainer **ppwc);
void     WidgetBase_SetParent(IWidget *me, IContainer *pwc);
void     WidgetBase_Draw(IWidget *me, ICanvas *piCanvas, int x, int y);
boolean  WidgetBase_IntersectOpaque(IWidget *me, AEERect *prcOut, const AEERect *prcIn);
int      WidgetBase_GetModel(IWidget *me, AEECLSID id, IModel **ppwm);
int      WidgetBase_SetModel(IWidget *me, IModel *pwm);
void     WidgetBase_SetHandler(IWidget *po, HandlerDesc *phd);
int      WidgetBase_GetViewModel(WidgetBase *me, IModel **ppiModel);
void     WidgetBase_SetViewModel(WidgetBase *me, IModel *piViewModel);

// Alternative IntersectOpqaue() for fully-opqaue widgets:
boolean  WidgetBase_IntersectOpaque_O(IWidget *po, AEERect *prcOut, const AEERect *prcIn);

// Util extent and content invalidation for widgetbase derived 

void WidgetBase_Invalidate(IWidget *po, uint32 icif);
void WidgetBase_InvalidateExtent(IWidget *po);
void WidgetBase_InvalidateContent(IWidget *po);


#endif /* __AEEWIDGETBASE_H__ */
