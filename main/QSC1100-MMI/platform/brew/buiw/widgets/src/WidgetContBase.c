/*
  ========================================================================

  FILE:  WidgetContBase.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a widget/container base class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "WidgetContBase.h"


#define ME_FROM_CONTAINER     WidgetContBase *me = po->pMe


void WidgetContBase_Invalidate(IContainer *po, IWidget *piw, 
                               const AEERect *prc, uint32 dwFlags)
{
   WidgetContBase *me = po->pMe;
   if (me->base.piContainer) {
     ICONTAINER_Invalidate(me->base.piContainer, (IWidget*)me, prc, dwFlags);
   }
}

static int WidgetContBase_Locate(IContainer *po, IWidget *pw, IContainer **ppic, AEERect *prc)
{
   return EUNSUPPORTED;
}

static int WidgetContBase_Insert(IContainer *po, IWidget *piw, IWidget *piwb, const void *pDesc)
{
   return EUNSUPPORTED;
}

static int WidgetContBase_Remove(IContainer *po, IWidget *piw)
{
   return EUNSUPPORTED;
}

static IWidget* WidgetContBase_GetWidget(IContainer *po, IWidget *piwRef, boolean bNext, boolean bWrap)
{
   return NULL;
}


int WidgetContBase_QueryInterface(IWidget *po, AEECLSID clsid, void **ppo)
{
   WidgetContBase *me = (WidgetContBase*) po;
   
   if (clsid == AEEIID_CONTAINER) {
      *ppo = (void *)&me->container;
      WidgetBase_AddRef((IWidget*)po);
      return SUCCESS;
   }

   return WidgetBase_QueryInterface(po, clsid, ppo);
}


void WidgetContBase_Ctor(WidgetContBase *me, AEEVTBL(IWidget) *pvt, 
                         IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, (AEEVTBL(IWidget) *)pvt, piModule, pfnDefHandler);

   // override QI for AEEIID_CONTAINER
   pvt->QueryInterface = WidgetContBase_QueryInterface;

   // Container functions
   AEEBASE_INIT(me, container, &me->vtContainer);
   // IQI methods defer to base IWidget vtbl
   me->vtContainer.AddRef           = AEEBASE_AddRef(IContainer);
   me->vtContainer.Release          = AEEBASE_Release(IContainer);
   me->vtContainer.QueryInterface   = AEEBASE_QueryInterface(IContainer);
   // default IContainer methods (most should be overriden in derived class)
   me->vtContainer.Invalidate       = WidgetContBase_Invalidate;
   me->vtContainer.Locate           = WidgetContBase_Locate;
   me->vtContainer.Insert           = WidgetContBase_Insert;
   me->vtContainer.Remove           = WidgetContBase_Remove;
   me->vtContainer.GetWidget        = WidgetContBase_GetWidget;


}



