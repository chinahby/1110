
/*
  ========================================================================

  FILE:  ScrollWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A Progress widget

  Properties:
  
  All Border properties

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __PROGRESSWIDGET_H__
#define __PROGRESSWIDGET_H__

#include "AEEProgressWidget.h"
#include "AEEWModel.h"
#include "WidgetBase.h"
#include "Border.h"

typedef struct ProgressWidget {
   WidgetBase     base;

   Border         border;
   RGBVAL         rgbProgress;
   IShell *       piShell;
   int            nIndeterminate;
   AEECallback    cbkAuto;
   int            cyFont;
   flg            bStepEnabled : 1;
   flg            bFinalStep : 1;
   int            nCurStep;
   int            nScrollRep;

   ModelListener modelListener;
} ProgressWidget;

static __inline IWidget *PROGRESSWIDGET_TO_IWIDGET(ProgressWidget *me) {
   return (IWidget *)me;
}

int  ProgressWidget_New              (IWidget **ppo, IValueModel *piModel, IShell *piShell, IModule *piModule);
void ProgressWidget_Ctor             (ProgressWidget *me, AEEVTBL(IWidget) *pvt, IValueModel *piModel, 
                                      IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);

boolean ProgressWidget_HandleEvent   (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void    ProgressWidget_Draw          (IWidget *po, ICanvas *piCanvas, int x, int y);
int     ProgressWidget_SetModel      (IWidget *po, IModel *piModel);
void    ProgressWidget_SetExtent     (IWidget *po, WExtent *pe);
int     ProgressWidget_QueryInterface(IWidget *po, AEECLSID clsid, void **ppo);
uint32  ProgressWidget_Release       (IWidget *po);


#endif /* __PROGRESSWIDGET_H__ */
