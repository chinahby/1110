/*
  ========================================================================

  FILE:  TabWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Definition for tabbed style decorator widget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __TABWIDGET_H__
#define __TABWIDGET_H__

#include "AEETabWidget.h"
#include "BorderWidget.h"
#include "AEEListModel.h"
#include "AEEWProperties.h"
#include "wutil.h"


typedef struct TabWidget {
   
   BorderWidget      base;

   IShell *          piShell;
   ModelListener     listener;

   int               cyTabHeight;
   int               nTabSpacingH;
   int               nTabSpacingV;

   AEECallback       cbkFocus;
   ModelListener     mlFocus;

   IWidget *         piwItem;    //an ImageStaticWidget by default
   AEERect           rcClip;
   AEERect           rcSelTab;

   WExtent           weTabA;     // active tab extent
   WExtent           weTabI;     // inactive tab extent

   int               nFocusPos;
   int               nScrollPos;
   int               nVisTabs;   // number of visible tabs
   int               nTabs;
   int               nTabWidthA;     // 
   int               nTabWidthI;     // 
   boolean           bScroll;
   uint32            dwFlags;

   TiledImage        tiArrow;    // tiled image for arrows

   IndexerDesc       idx;

} TabWidget;


int TabWidget_New(IDecorator **ppo, IShell *piShell, IModule *piModule);








#endif   //__TABWIDGET_H__
