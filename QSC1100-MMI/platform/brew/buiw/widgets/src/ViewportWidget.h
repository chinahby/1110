/*
  ========================================================================

  FILE:  ViewportWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a decorator widget that
                       provides a 'viewport' into another widget.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __VIEWPORTWIDGET_H__
#define __VIEWPORTWIDGET_H__

#include "AEEViewportWidget.h"

#include "DecoratorWidget.h"
#include "Border.h"

typedef struct ViewportWidget 
{
   Decorator      base;

   Border         border;
   int            xOrigin;
   int            yOrigin;

   int            xMax;    // limit of xOrigin
   int            yMax;    // limit of yOrigin

   int            nInc;

   WExtent        weChild;    // extent of the child
   
   IShell *       piShell;
   AEECallback    cbkView;

   uint32         dwFlags;
   uint8          nLayout;
   boolean        bInCalcXY;

} ViewportWidget;


int ViewportWidget_New   (IDecorator **ppo, IShell *piShell, IModule *piModule);
void ViewportWidget_Ctor (ViewportWidget *me, AEEVTBL(IDecorator) *pvt, 
                          IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
void ViewportWidget_Invalidate(IContainer *po, IWidget *piw, const AEERect *prc, uint32 dwFlags);


// Utility functions
int IWidget_WrapInViewport(IWidget **ppioChild, IShell *piShell, IModule *piModule);

#endif   //__VIEWPORTWIDGET_H__

