/*
  ========================================================================

  FILE:  WidgetContBase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines a widget/container base class for derivation.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __WIDGETCONTBASE_H__
#define __WIDGETCONTBASE_H__

#include "AEEBase.h"
#include "WidgetBase.h"
#include "AEEContainer.h"


typedef struct WidgetContBase WidgetContBase;

struct IContainer {
   AEEBASE_INHERIT(IContainer, WidgetContBase);
};

struct WidgetContBase {
   WidgetBase     base;
   IContainer     container;
   AEEVTBL(IContainer) vtContainer;
};


#define WCBASE(p)      ((WidgetContBase*)(void*)p)



void WidgetContBase_Ctor         (WidgetContBase *me, AEEVTBL(IWidget) *pvt, 
                                  IModule *piModule, PFNHANDLER pfnDefHandler);

void WidgetContBase_Invalidate   (IContainer *po, IWidget *piw, 
                                  const AEERect *prc, uint32 dwFlags);


int WidgetContBase_QueryInterface   (IWidget *po, AEECLSID clsid, void **ppo);

#endif   //__WIDGETCONTBASE_H__
