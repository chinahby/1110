/*
  ========================================================================

  FILE:  CursorWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of CursorWidget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CURSORWIDGET_H__
#define __CURSORWIDGET_H__

#include "AEECursorWidget.h"
#include "DecoratorWidget.h"
#include "BitmapWidget.h"

typedef struct {
   BitmapWidget   base;
   IShell *       piShell;
} CursorWidget;

static __inline IWidget *CURSORWIDGET_TO_IWIDGET(CursorWidget *me) {
   return (IWidget *)me;
}

int      CursorWidget_New         (IWidget **ppo, IShell *piShell, IModule *piModule);
void     CursorWidget_Ctor        (CursorWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                                   IModule *piModule, PFNHANDLER pfnDefHandler);
void     CursorWidget_Dtor       (CursorWidget *me);

boolean  CursorWidget_HandleEvent (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);


#endif /* __CURSORWIDGET_H__ */
