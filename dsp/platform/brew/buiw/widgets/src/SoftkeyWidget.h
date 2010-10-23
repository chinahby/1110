/*
  ========================================================================

  FILE:  SoftkeyWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Softkey widget definition

  A SoftkeyLabel assigns labels to the physical softkeys on the device.
  It shows this in a horizontal strip preferrably at or near the physical
  soft keys. 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __SOFTKEYWIDGET_H__
#define __SOFTKEYWIDGET_H__

#include "AEESoftkeyWidget.h"
#include "AEEPropContainer.h"

typedef struct SoftkeyWidget {
   IPropContainer *piContainer;
   HandlerDesc handler;
} SoftkeyWidget;


int  SoftkeyWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);

#endif /* __SOFTKEYLABEL_H__ */
