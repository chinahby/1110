 /*
  ========================================================================

  FILE:  ScrollWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A Scrollbar decorator for any widget that responds
  to the property PROP_SCROLLMODEL. 

  Can also be used seperately without a child widget by setting property
  PROP_SCROLLMODEL

  Properties:

  PROP_SCROLLMODEL (Set)
  PROP_FGCOLOR - For color of scroll handle
  PROP_BORDERCOLOR - For Border and arrow color

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __SCROLLWIDGET_H__
#define __SCROLLWIDGET_H__

#include "AEEScrollWidget.h"
#include "AEEWModel.h"
#include "DecoratorWidget.h"
#include "Border.h"
#include "wutil.h"


typedef struct SbArrows {
   TiledImage     tiVArrows;
   TiledImage     tiHArrows;
   IShell *       piShell;
} SbArrows;


typedef struct {
   uint16      range;
   uint16      visible;
   uint16      position;
} ScrollInfo;

typedef struct ScrollbarWidget {
   Decorator      base;
   IShell *       piShell;
   ModelListener  scrollListener;
   uint8          nScrollStyle;   // SCROLL_HORZ, SCROLL_VERT, SCROLL_BOTH
   ScrollInfo     scroll[2];     // [0] == vertical, [1] == horizontal

   Border         border;
   RGBVAL         rgbBorder[2];
   RGBVAL         rgbHandle[2];
   RGBVAL         rgbFill;

   int8           nHandleWidth;
   int8           nPad;
   int8           nEndPad; // padding at ends of scrollbars when arrows aren't visible

   SbArrows       arrows;
   uint32         dwFlags;

   int            focusIndex;
   int            numItems;

} ScrollbarWidget;

static __inline IDecorator *SCROLLBARWIDGET_TO_IDECORATOR(ScrollbarWidget *me) {
   return (IDecorator *)me;
}

static __inline IWidget *SCROLLBARWIDGET_TO_IWIDGET(ScrollbarWidget *me) {
   return (IWidget *)me;
}

int      ScrollbarWidget_New           (IDecorator **ppo, IShell *piShell, IModule *piModule);
void     ScrollbarWidget_Ctor          (ScrollbarWidget *me, AEEVTBL(IDecorator) *pvt, 
                                        IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
boolean  ScrollbarWidget_HandleEvent   (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     ScrollbarWidget_Draw          (IDecorator *po, ICanvas *piCanvas, int x, int y);
void     ScrollbarWidget_SetExtent     (IDecorator *po, WExtent *pe);
void     ScrollbarWidget_GetExtent     (IDecorator *po, WExtent *pe);
uint32   ScrollbarWidget_Release       (IDecorator *po);


/////////////////////////////////////////////////////////////////
// Utility functions

int IWidget_WrapInScrollbar(IWidget **ppioChild, IShell *piShell, IModule *piModule);


/////////////////////////////////////////////////////////////////
// ScrollIndicatorWidget

typedef struct ScrollIndicatorWidget {
   Decorator      base;
   IShell *       piShell;
   ModelListener  scrollListener;
   uint8          nScrollStyle;   // SCROLL_HORZ, SCROLL_VERT, SCROLL_BOTH
   ScrollInfo     scroll[2];      // scroll[0] == vertical, scroll[1] == horizontal

   Border         border;
   RGBVAL         rgbIndicator[2];
   int            nPad;

   SbArrows       arrows;

} ScrollIndicatorWidget;


static __inline IDecorator *SCROLLINDICATORWIDGET_TO_IDECORATOR(ScrollIndicatorWidget *me) { 
   return (IDecorator *)me; 
}

static __inline IWidget *SCROLLINDICATORWIDGET_TO_IWIDGET(ScrollIndicatorWidget *me) { 
   return (IWidget *)me; 
}


void     ScrollIndicatorWidget_Ctor               (ScrollIndicatorWidget *me, AEEVTBL(IDecorator) *pvt, 
                                                   IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
int      ScrollIndicatorWidget_New                (IDecorator **ppo, IShell *piShell, IModule *piModule);
void     ScrollIndicatorWidget_Draw               (IDecorator *po, ICanvas *piCanvas, int x, int y);
boolean  ScrollIndicatorWidget_HandleEvent        (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     ScrollIndicatorWidget_GetPreferredExtent (IDecorator *po, WExtent *pweOut);





#endif /* __SCROLLWIDGET_H__ */
