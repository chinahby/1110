/*
  ========================================================================

  FILE:  wbase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation for widgets
  
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "wbase.h"
#include "fbase.h"
#include "util.h"
#include "AEEWProperties.h"


//--------------------------------------------------------------------
//    IWidget Methods
//--------------------------------------------------------------------

#define DECLARE_ME    WidgetBase *me = ((WidgetBase*)po)


uint32 WidgetBase_AddRef(IWidget *po)
{
   DECLARE_ME;
   
   return ++me->nRefs;
}


uint32 WidgetBase_Release(IWidget *po)
{
   DECLARE_ME;
   
   uint32 nRefs = --me->nRefs;

   if (nRefs == 0) { 
      // Call free hook for handler cxt
      HANDLERDESC_Free(&me->hd);
      WidgetBase_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


int WidgetBase_QueryInterface(IWidget *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_QUERYINTERFACE ||
       clsid == AEEIID_HANDLER ||
       clsid == AEEIID_WIDGET) {
      // I am IWidget/IQueryInterface, too.
      *ppNew = (void*)po;
      WidgetBase_AddRef(po);
      return SUCCESS;
   }

   *ppNew = 0;
   return ECLASSNOTSUPPORT;
}

void WidgetBase_SetViewModel(WidgetBase *me, IModel *piViewModel)
{
   RELEASEIF(me->piViewModel);
   me->piViewModel = piViewModel;
   ADDREFIF(piViewModel);
}


int WidgetBase_GetViewModel(WidgetBase *me, IModel **ppiModel) 
{
   int nErr = 0;

   // if we've been requested to supply a view model,
   // and we don't have one, create it
   if (!me->piViewModel) {
      nErr = ModelBase_New(&me->piViewModel, me->piModule);
   }

   *ppiModel = me->piViewModel;
   
   // AddRef the returned view model
   if (me->piViewModel) {
      IMODEL_AddRef(me->piViewModel);
   }

   return nErr;
}


boolean WidgetBase_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECLARE_ME;

   if (evt == EVT_WDG_GETPROPERTY && wParam == PROP_VIEWMODEL) {
      WidgetBase_GetViewModel(me, (IModel**)dwParam);
      return TRUE;
   }

   return FALSE;
}

static boolean WidgetBase_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECLARE_ME;
   return HANDLERDESC_Call(&me->hd, evt, wParam, dwParam);
}

void WidgetBase_SetHandler(IWidget *po, HandlerDesc *phd)
{
   DECLARE_ME;

   // Reset on null descriptor
   if (!phd) {
      HANDLERDESC_Init(&me->hd, me->pfnDefHandler, me, 0);
   } else {
      HandlerDesc hdTemp = *phd;
      *phd = me->hd;
      me->hd = hdTemp;
   }
}

void WidgetBase_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   // use pwePreferred
   return;
}


void WidgetBase_GetExtent(IWidget *po, WExtent *pWExtent)
{
   DECLARE_ME;
   
   *pWExtent = me->extent;
}


void WidgetBase_SetExtent(IWidget *po, WExtent *pWExtent)
{
   DECLARE_ME;
   WExtent eOld = me->extent;

   me->extent = *pWExtent;

   if (me->piContainer) {

      AEERect rc;
      rc.x = 0;
      rc.y = 0;
      rc.dx = MAX(pWExtent->width, eOld.width);
      rc.dy = MAX(pWExtent->height, eOld.height);
      
      ICONTAINER_Invalidate(me->piContainer, po, &rc, ICIF_EXTENT);
   }

   if (me->piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_SETEXTENT;
      ev.dwParam  = (uint32)&me->extent;
      IMODEL_Notify(me->piViewModel, &ev);
   }
}


void WidgetBase_GetParent(IWidget *po, IContainer **ppwc)
{
   DECLARE_ME;
   *ppwc = me->piContainer;
   ADDREFIF(me->piContainer);
}


void WidgetBase_SetParent(IWidget *po, IContainer *pwc)
{
   DECLARE_ME;
   me->piContainer = pwc;
}


void WidgetBase_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   // nothing to draw
}



// Default IntersectOpqaue() assumes transparency
//
boolean WidgetBase_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   prcOut->dx = 0;
   prcOut->dy = 0;
   return FALSE;
}


// Alternative for fully-opaque widgets
//
boolean WidgetBase_IntersectOpaque_O(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   DECLARE_ME;
   AEERect rc;

   rc.x = 0;
   rc.y = 0;
   rc.dx = me->extent.width;
   rc.dy = me->extent.height;

   return IntersectRect(prcOut, prcIn, &rc);
}


int WidgetBase_GetModel(IWidget *po, AEECLSID clsId, IModel **ppiModel)
{
   DECLARE_ME;
   
   if (me->piModel) {
      return IQI_QueryInterface((IQueryInterface *)me->piModel, clsId, (void**)ppiModel);
   } else {
      *ppiModel = 0;
      return EFAILED;
   }
}


int WidgetBase_SetModel(IWidget *po, IModel *piModel)
{
   DECLARE_ME;
   
   RELEASEIF(me->piModel);
   me->piModel = piModel;
   ADDREFIF(piModel);

   if (me->piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_SETMODEL;
      ev.dwParam  = (uint32)me->piModel;
      IMODEL_Notify(me->piViewModel, &ev);
   }

   return SUCCESS;
}


//--------------------------------------------------------------------
//    WidgetBase Member Functions
//--------------------------------------------------------------------


void WidgetBase_InitVT(AEEVTBL(IWidget) *pvt)
{
   #define VTFUNC(name)        pvt->name = WidgetBase_##name

   VTFUNC( AddRef );
   VTFUNC( Release );
   VTFUNC( QueryInterface );
   VTFUNC( HandleEvent );
   VTFUNC( GetPreferredExtent );
   VTFUNC( GetExtent );
   VTFUNC( SetExtent );
   VTFUNC( GetParent );
   VTFUNC( SetParent );
   VTFUNC( Draw );
   VTFUNC( IntersectOpaque );
   VTFUNC( GetModel );
   VTFUNC( SetModel );
   VTFUNC( SetHandler );
}


// Construct a WidgetBase
//
// If pvt != NULL, the structure it points to is filled with function
// pointers.  This should be the case only when the caller has dynamically
// allocated the vtable
//
void WidgetBase_Ctor(WidgetBase *me, AEEVTBL(IWidget) *pvt, 
                     IModule *piModule, PFNHANDLER pfnDefHandler)
{
   me->nRefs = 1;

   me->piModule = piModule;
   ADDREFIF(piModule);

   if (pvt) {
      me->pvt = pvt;
      WidgetBase_InitVT(pvt);
   }

   me->pfnDefHandler = pfnDefHandler ? pfnDefHandler 
                                     : (PFNHANDLER)WidgetBase_DefHandleEvent;

   HANDLERDESC_Init(&me->hd, me->pfnDefHandler, me, 0);
}


void WidgetBase_Dtor(WidgetBase *me)
{
   RELEASEIF(me->piModel);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piViewModel);
}

// Util invalidation
void WidgetBase_InvalidateExtent(IWidget *po)
{
   WidgetBase_Invalidate(po, ICIF_EXTENT);
}

void WidgetBase_InvalidateContent(IWidget *po)
{
   WidgetBase_Invalidate(po, 0);
}

void WidgetBase_Invalidate(IWidget *po, uint32 icif)
{
   DECLARE_ME;
   AEERect invRect;
   
   if (me->piContainer) {
      SETAEERECT(&invRect, 0, 0, me->extent.width,
                 me->extent.height);
      
      ICONTAINER_Invalidate(me->piContainer, po, &invRect, icif);
   }   
}
   
