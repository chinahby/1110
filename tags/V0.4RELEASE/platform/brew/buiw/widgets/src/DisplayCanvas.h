/*
  ========================================================================

  FILE:  DisplayCanvas.h
  
  SERVICES:  

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __DISPLAYCANVAS_H__
#define __DISPLAYCANVAS_H__

#include "AEEDisplayCanvas.h"

typedef struct DisplayCanvas DisplayCanvas;

struct DisplayCanvas {
   const AEEVTBL(IDisplayCanvas) *pvt;
   uint32      nRefs;
   IModule *   piModule;
   IDisplay *  piDisplay;
};


uint32 DisplayCanvas_AddRef         (IDisplayCanvas *po);
uint32 DisplayCanvas_Release        (IDisplayCanvas *po);
int    DisplayCanvas_QueryInterface (IDisplayCanvas *po, AEECLSID clsid, void **ppNew);
int    DisplayCanvas_GetBitmap      (IDisplayCanvas *po, IBitmap **ppo);
int    DisplayCanvas_SetBitmap      (IDisplayCanvas *po, IBitmap *pib);
int    DisplayCanvas_SetClipRect    (IDisplayCanvas *po, const AEERect *prc);
int    DisplayCanvas_GetClipRect    (IDisplayCanvas *po, AEERect *prc);

void   DisplayCanvas_Ctor           (DisplayCanvas *me, AEEVTBL(IDisplayCanvas) *pvt, IModule *piModule);
void   DisplayCanvas_Dtor           (DisplayCanvas *me);
int    DisplayCanvas_New            (IDisplayCanvas **ppo, IModule *piModule);



#endif   //__DISPLAYCANVAS_H__

