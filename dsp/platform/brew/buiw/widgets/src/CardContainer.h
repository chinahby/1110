/*
  ========================================================================

  FILE:  CardContainer.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a "card-deck" style container.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __CARDCONTAINER_H_
#define __CARDCONTAINER_H_

#include "AEECardContainer.h"
#include "ContainerBase.h"

typedef struct CardContainer 
{
   ContainerBase     base;
   IShell *          piShell;
   WidgetNode *      pnActive;
   ModelListener     mlFocus;
   int               nFocusIndex;
   AEECallback       cbkFocus;
} CardContainer;


int     CardContainer_New       (ICardContainer **ppo, IShell *piShell, IModule *piModule);
void    CardContainer_Ctor      (CardContainer *me, AEEVTBL(ICardContainer) *pvt, 
                                 IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);

// IQI methods
int     CardContainer_QueryInterface(ICardContainer *po, AEECLSID id, void **ppo);
// ICardContainer methods
int     CardContainer_Insert     (ICardContainer *po, IWidget *piw, IWidget *piwBefore, const void *pDesc);
int     CardContainer_Remove     (ICardContainer *po, IWidget *piw);
int     CardContainer_Activate   (ICardContainer *po, IWidget *piw);

// IWidget method overrides

void    CardContainer_SetExtent  (IWidget *po, WExtent *pe);



#endif   //__CARDCONTAINER_H_

