/*
  ========================================================================

  FILE:  ImageWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of ImageWidget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _IMAGEWIDGET_H_
#define _IMAGEWIDGET_H_

#include "AEEImageWidget.h"
#include "DecoratorWidget.h"
#include "Border.h"
#include "ImageBase.h"


typedef struct ImageWidget {
   Decorator      base;
   ImageBase      image;
   Border         border;
   ModelListener  modelListener;
   uint32         dwFlags;
   RGBVAL         rgbTransp;
} ImageWidget;

// Safe up-cast
static __inline IWidget *IMAGEWIDGET_TO_IWIDGET(ImageWidget *me) { 
   return (IWidget *) me; 
}
static __inline IDecorator *IMAGEWIDGET_TO_IDECORATOR(ImageWidget *me) { 
   return (IDecorator *) me; 
}

int      ImageWidget_New         (IDecorator **ppo, IShell *piShell, IModule *piModule);
void     ImageWidget_Ctor        (ImageWidget *me, AEEVTBL(IDecorator) *pvt, IShell *piShell, IModule *piModule, 
                                  PFNHANDLER pfnDefHandler);
void     ImageWidget_Dtor        (ImageWidget *me);

int      ImageWidget_Initialize  (ImageWidget *me);

uint32   ImageWidget_Release     (IDecorator *po);
void     ImageWidget_SetExtent   (IDecorator *po, WExtent *pExtent);
void     ImageWidget_Draw        (IDecorator *po, ICanvas *piCanvas, int x, int y);
void     ImageWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut);
boolean  ImageWidget_HandleEvent (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
int      ImageWidget_SetModel    (IDecorator *po, IModel *piModel);


#endif // _IMAGEWIDGET_H_
