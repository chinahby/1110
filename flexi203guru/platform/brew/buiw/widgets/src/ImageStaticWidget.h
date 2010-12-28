/*
  ========================================================================

  FILE:  ImageStaticWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __IMAGESTATICWIDGET_H__
#define __IMAGESTATICWIDGET_H__

#include "AEEImageStaticWidget.h"
#include "PropContainer.h"


typedef struct {

   PropContainer     base;

   IWidget *         piImage;
   IWidget *         piLabel;
   uint32            dwFlags;
   HandlerDesc       handler;

   IValueModel *     pivm;
   ModelListener     modelListener;
} ImageStaticWidget;


int ImageStaticWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);

#endif /* __IMAGESTATICWIDGET_H__ */
