/*
  ========================================================================

  FILE:  XYContainer.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines a container that positions it's widgets
  using x,y,z coordinates

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __XYCONTAINER_H__
#define __XYCONTAINER_H__

#include "ContainerBase.h"
#include "AEEXYContainer.h"
#include "AEERootContainer.h"

typedef struct XYContainer 
{
   ContainerBase base;
} XYContainer;

int      XYContainer_New            (IXYContainer **ppo, IShell *piShell, IModule *piModule);
void     XYContainer_Ctor           (XYContainer *me, AEEVTBL(IXYContainer) *pvt, 
                                     IShell *piShell, IModule *piModule, 
                                     PFNHANDLER pfnDefHandler, PFNLAYOUT pfnLayout);
int      XYContainer_QueryInterface (IXYContainer *po, AEECLSID id, void **ppo);
int      XYContainer_GetPos         (IXYContainer *po, IWidget *piw, WidgetPos *pos);
int      XYContainer_SetPos         (IXYContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetPos *pos);

#endif /* __XYCONTAINER_H__ */
