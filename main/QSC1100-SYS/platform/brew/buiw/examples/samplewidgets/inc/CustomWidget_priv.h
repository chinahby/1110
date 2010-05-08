/*
  ========================================================================

  FILE:           CustomWidget_priv.h

  SERVICES:       Private CustomWidget declarations and definitions.  Used
                  internally by the CustomWidget module only.

  PUBLIC CLASSES: Custom Widget Class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CUSTOMWIDGET_PRIV_H__
#define __CUSTOMWIDGET_PRIV_H__

#include "CustomWidget.h"

int CustomWidget_New(IWidget **ppo, IShell *pIShell, IModule *piModule);

#endif // __CUSTOMWIDGET_PRIV_H__