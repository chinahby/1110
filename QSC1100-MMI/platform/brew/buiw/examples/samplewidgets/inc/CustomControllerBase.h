/*
  ========================================================================

  FILE:  ControllerBase.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation for controllers

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __CONTROLLERBASE_H__
#define __CONTROLLERBASE_H__


#include "AEEWidget.h"


typedef struct ControllerBase ControllerBase;
struct ControllerBase {
   AEEVTBL(IController) *  pvt;
   IModule *               piModule;
   uint32                  nRefs;
   IWidget *               piw;
   HandlerDesc             hd;
   PFNHANDLER              pfnDefHandler;
};


void     ControllerBase_Ctor           (ControllerBase *me, AEEVTBL(IController) *pvt, IModule *piModule, PFNHANDLER pfnHandler);
uint32   ControllerBase_AddRef         (IController *po);
uint32   ControllerBase_Release        (IController *po);
int      ControllerBase_QueryInterface (IController *po, AEECLSID id, void **ppo);
boolean  ControllerBase_DefHandleEvent (IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     ControllerBase_SetHandler     (IController *po, HandlerDesc *pDesc);
int      ControllerBase_SetWidget      (IController *po, IWidget *piWidget);


#endif   //__CONTROLLERBASE_H__





