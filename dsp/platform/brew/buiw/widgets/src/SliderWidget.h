/*
  ========================================================================

  FILE:  SliderWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __SLIDERWIDGET_H__
#define __SLIDERWIDGET_H__


#include "AEESliderWidget.h"
#include "WidgetBase.h"
#include "Border.h"


typedef struct SliderWidget SliderWidget;

struct SliderWidget {
   WidgetBase     base;

   Border         border;     // used for slider main rectangle
   WExtent        bextent;    // border extent

   IShell *       piShell;
   uint16         nMaxPos;     // if nMaxPos == 5, valid nPos values are (0,1,2,3,4,5)
   int            cyFont;
   RGBVAL         rgbHandleBorder;
   boolean        bVert;

   ModelListener  ml;
};


int      SliderWidget_New              (IWidget **ppo, IShell *piShell, IModule *piModule);
int      SliderWidget_SetModel         (IWidget *po, IModel *piModel);
void     SliderWidget_GetPreferredExtent  (IWidget *po, WExtent *pweOut);
void     SliderWidget_SetExtent        (IWidget *po, WExtent *pe);
boolean  SliderWidget_HandleEvent      (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     SliderWidget_Dtor             (SliderWidget *me);
void     SliderWidget_Delete           (SliderWidget *me);
uint32   SliderWidget_Release          (IWidget *po);
int      SliderWidget_Construct        (SliderWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                                        IModule *piModule, PFNHANDLER pfnDefHandler);





#endif   /*__SLIDERWIDGET_H__*/


