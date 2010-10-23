/*
  ========================================================================

  FILE:  GridContainer.h
  
  SERVICES:  

  GENERAL DESCRIPTION: A grid container uses a grid to order its
  children. It provides default focus movement behaviour to allow
  focus to move around the grid in the obvious way. 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __GRIDCONTAINER_H__
#define __GRIDCONTAINER_H__

#include "AEEGridContainer.h"
#include "ContainerBase.h"

typedef struct
{
   ContainerBase base;

   uint16 iRows, iCols;
   GridDescriptor *pRowDesc, *pColDesc;
   uint16 *pRowCurrMax, *pColCurrMax;
   uint8 fShrinkToFit: 1; // set if there are any shrink-to-fit rows/cols  
} GridContainer;

void     GridContainer_doLayout(GridContainer *me);
uint32   GridContainer_Release(IGridContainer *po);
int      GridContainer_New       (IGridContainer **ppo, IShell *piShell, IModule *piModule);
int      GridContainer_Construct (GridContainer *me, AEEVTBL(IGridContainer) *pvt, IModule *piModule,
                                  PFNHANDLER pfnDefHandler);

void     GridContainer_Ctor(GridContainer *me, AEEVTBL(IGridContainer) *pvt, IModule *piModule, 
                            PFNHANDLER pfnDefHandler);
void     GridContainer_Dtor(GridContainer *me);

int      GridContainer_Insert (IGridContainer *po, IWidget *piw,
                              IWidget *piwBefore);
int      GridContainer_Remove (IGridContainer *po, IWidget *piw);
void     GridContainer_Invalidate(IGridContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwParam);
void     GridContainer_GetPreferredExtent(IWidget *po, WExtent *pwe);
void     GridContainer_GetGridDescriptors(IGridContainer *po, GridDescriptor *pRowDesc, uint16 *iRows,
                                          GridDescriptor *pColDesc, uint16 *iCols);
int      GridContainer_SetGridDescriptors(IGridContainer *po, GridDescriptor *pRowDesc, uint16 *iRows,
                                          GridDescriptor *pColDesc, uint16 *iCols);
int      GridContainer_GetVisibility(IGridContainer *po, IWidget *piw, boolean *pbVisible);
int      GridContainer_SetVisibility(IGridContainer *po, IWidget *piw, IWidget *piwBefore, const boolean *pbVisible);

int      GridContainer_QueryInterface(IGridContainer *po, AEECLSID id, void **ppo);

#endif // __GRIDCONTAINER_H__
