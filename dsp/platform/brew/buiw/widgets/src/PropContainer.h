/*
  ========================================================================

  FILE:  PropContainer.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Defines a proportional container

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __PROPCONTAINER_H__
#define __PROPCONTAINER_H__

#include "AEEPropContainer.h"
#include "ContainerBase.h"

typedef struct PropContainer 
{
   ContainerBase base;

   flg fVertical : 1;
} PropContainer;

int     PropContainer_New       (IPropContainer **ppo, IModule *piModule);
void    PropContainer_Ctor      (PropContainer *me, AEEVTBL(IPropContainer) *pvt, 
                                 IModule *piModule, PFNHANDLER pfnDefHandler);

int     PropContainer_SetProp    (IPropContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetProp *prop);
int     PropContainer_GetProp    (IPropContainer *po, IWidget *piw, WidgetProp *prop);
int     PropContainer_SetPadding (IPropContainer *po, IWidget *piw, int32 dwBefore, int32 dwAfter);
int     PropContainer_GetPadding (IPropContainer *po, IWidget *piw, int32 *pdwBefore, int32 *pdwAfter);
boolean PropContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

#endif /* __PROPCONTAINER_H__ */
