/*
  ========================================================================

  FILE:  RootContainer.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of RootContainer

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __ROOTCONTAINER_H__
#define __ROOTCONTAINER_H__

#include "AEERootContainer.h"
#include "AEEDrawHandler.h"
#include "XYContainer.h"


typedef struct RootContainer RootContainer;


struct IDrawHandler {
   AEEBASE_INHERIT(IDrawHandler, RootContainer);
};

struct RootContainer {
   XYContainer             base;

   // we also support IDrawHandler
   IDrawHandler            idrawhandler;
   AEEVTBL(IDrawHandler)   vtDrawHandler;
   DrawHandlerDesc         drawdesc;

   int16                   xDisp;
   int16                   yDisp;
   AEERect                 rcInvalid;
   ICanvas *               piCanvas;
   IShell   *              piShell;
   AEECallback             cbDraw;
};


void    RootContainer_Dtor            (RootContainer *me);
int     RootContainer_New             (IRootContainer **ppo, IShell *piShell, IModule *piModule);
int     RootContainer_Construct       (RootContainer *me, AEEVTBL(IRootContainer) *pvt, 
                                       IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);

void    RootContainer_Invalidate      (IRootContainer *po, IWidget *piw, const AEERect *prc, uint32 dwFlags);
uint32  RootContainer_Release         (IRootContainer *po);
void    RootContainer_SetCanvas       (IRootContainer *po, ICanvas *piCanvas, const AEERect *prc);
int     RootContainer_GetCanvas       (IRootContainer *po, ICanvas **ppiCanvas, AEERect *prc);
boolean RootContainer_DefHandleEvent  (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
boolean RootContainer_IntersectOpaque (IWidget *po, AEERect *prcOut, const AEERect *prcIn);
int     RootContainer_QueryInterface  (IRootContainer *po, AEECLSID id, void **ppo);


#endif /* __ROOTCONTAINER_H__ */
