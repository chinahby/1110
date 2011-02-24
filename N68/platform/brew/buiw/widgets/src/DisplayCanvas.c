/*
  ========================================================================

  FILE:  DisplayCanvas.c
  
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
#include "AEEStdLib.h"
#include "wutil.h"
#include "DisplayCanvas.h"


#define DECL(name)      name *me = (name*)po


///////////////////////////////////////////////
// IQueryInterface


uint32 DisplayCanvas_AddRef(IDisplayCanvas *po)
{
   DECL(DisplayCanvas);
   return ++me->nRefs;
}

uint32 DisplayCanvas_Release(IDisplayCanvas *po)
{
   DECL(DisplayCanvas);
   
   uint32 nRefs = --me->nRefs;

   if (!nRefs) { 
      DisplayCanvas_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


int DisplayCanvas_QueryInterface(IDisplayCanvas *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_QUERYINTERFACE || 
         clsid == AEEIID_CANVAS         ||
         clsid == AEEIID_DISPLAYCANVAS) {

      *ppNew = (void*)po;
      DisplayCanvas_AddRef(po);
      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}


///////////////////////////////////////////////
// ICanvas

int DisplayCanvas_GetBitmap(IDisplayCanvas *po, IBitmap **ppo)
{
   DECL(DisplayCanvas);

   *ppo = 0;

   if (!me->piDisplay) {
      return EFAILED;
   }

   *ppo = IDISPLAY_GetDestination(me->piDisplay);
   return SUCCESS;
}


int DisplayCanvas_SetBitmap(IDisplayCanvas *po, IBitmap *pib)
{
   DECL(DisplayCanvas);

   if (!me->piDisplay) {
      return EFAILED;
   }
   return IDISPLAY_SetDestination(me->piDisplay, pib);
}


int DisplayCanvas_SetClipRect(IDisplayCanvas *po, const AEERect *prc)
{
   DECL(DisplayCanvas);

   if (!me->piDisplay) {
      return EFAILED;
   }
   
   IDISPLAY_SetClipRect(me->piDisplay, prc);
   return SUCCESS;
}

int DisplayCanvas_GetClipRect(IDisplayCanvas *po, AEERect *prc)
{
   DECL(DisplayCanvas);

   if (!me->piDisplay) {
      return EFAILED;
   }
   
   IDISPLAY_GetClipRect(me->piDisplay, prc);
   return SUCCESS;
}


///////////////////////////////////////////////
// IDisplayCanvas


int DisplayCanvas_GetDisplay(IDisplayCanvas *po, IDisplay **ppo)
{
   DECL(DisplayCanvas);
   *ppo = me->piDisplay;
   ADDREFIF(me->piDisplay);
   return me->piDisplay ? SUCCESS : EFAILED;
}


int DisplayCanvas_SetDisplay(IDisplayCanvas *po, IDisplay *piDisplay)
{
   DECL(DisplayCanvas);

   RELEASEIF(me->piDisplay);
   me->piDisplay = piDisplay;
   ADDREFIF(me->piDisplay);

   return SUCCESS;
}


///////////////////////////////////////////////
// DisplayCanvas - object ctor, dtor


void DisplayCanvas_Ctor(DisplayCanvas *me, AEEVTBL(IDisplayCanvas) *pvt, IModule *piModule)
{
#define VTFUNC(name)    pvt->name = DisplayCanvas_##name
   VTFUNC( AddRef );
   VTFUNC( Release );
   VTFUNC( QueryInterface );
   VTFUNC( GetBitmap );
   VTFUNC( SetBitmap );
   VTFUNC( SetClipRect );
   VTFUNC( GetClipRect );
   VTFUNC( GetDisplay );
   VTFUNC( SetDisplay );
#undef  VTFUNC

   me->pvt = pvt;
   me->nRefs = 1;

   me->piModule = piModule;
   ADDREFIF(piModule);
}


void DisplayCanvas_Dtor(DisplayCanvas *me)
{
   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piModule);
}


int DisplayCanvas_New(IDisplayCanvas **ppo, IModule *piModule)
{
   DisplayCanvas *me = MALLOCREC_VTBL(DisplayCanvas, IDisplayCanvas);

   *ppo = (IDisplayCanvas*)me;
   if (!me) {
      return ENOMEMORY;
   }

   DisplayCanvas_Ctor(me, GETVTBL(me,IDisplayCanvas), piModule);
   return SUCCESS;
}



