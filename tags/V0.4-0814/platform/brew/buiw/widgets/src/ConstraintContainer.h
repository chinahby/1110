/*
  ========================================================================

  FILE:  ConstraintContainer.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A Constraint container maintains a series of 
  constraints on its children. 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CONSTRAINTCONTAINER_H__
#define __CONSTRAINTCONTAINER_H__

#include "AEEConstraintContainer.h"
#include "ContainerBase.h"

typedef struct ConstraintContainer 
{
   ContainerBase base;
} ConstraintContainer;


int     ConstraintContainer_New       (IConstraintContainer **ppo, IShell *piShell, IModule *piModule);
int     ConstraintContainer_Construct (ConstraintContainer *me, AEEVTBL(IConstraintContainer) *pvt, 
                                       IModule *piModule, PFNHANDLER pfnDefHandler);

void ConstraintContainer_Ctor(ConstraintContainer *me, AEEVTBL(IConstraintContainer) *pvt, 
                              IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);

int     ConstraintContainer_SetConstraint(IConstraintContainer *po, IWidget *piw, IWidget *piwBefore, const WidgetConstraint *pwc);
int     ConstraintContainer_GetConstraint(IConstraintContainer *po, IWidget *piw, WidgetConstraint *pwc);
void    ConstraintContainer_GetPreferredExtent(IWidget *po, WExtent *pwe);

#endif /* __CONSTRAINTCONTAINER_H__ */
