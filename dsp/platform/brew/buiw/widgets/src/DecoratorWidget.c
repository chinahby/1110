/*
  ========================================================================

  FILE:  DecoratorWidget.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a decorator base class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "DecoratorWidget.h"
#include "AEEBase.h"
#include "AEEWProperties.h"
#include "wutil.h"

#define ME_FROM_CONTAINER Decorator *me = (Decorator *)po->pMe
#define ME_FROM_DECORATOR Decorator *me = (Decorator *)po

////////////////////////////////////////////////////////////
// IWidget members


void Decorator_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_DECORATOR;

   if (me->pChild)
      IWIDGET_Draw(me->pChild, piCanvas, x, y);
}

boolean Decorator_IntersectOpaque(IDecorator *po, AEERect *prc, const AEERect *prcTest)
{
   ME_FROM_DECORATOR;

   if (me->pChild) {
      return IWIDGET_IntersectOpaque(me->pChild, prc, prcTest); 
   } else {
      // No child, transparent
      prc->dx = 0;
      prc->dy = 0;
      return FALSE;
   }
}


boolean Decorator_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_DECORATOR;

   if (evt == EVT_WDG_MOVEFOCUS && (IWidget*)dwParam == me->pChild) {
      return TRUE;
   }

   // pass Get/Set PROP_VISIBLE up to parent, but change queried widget to me
   if ((evt == EVT_WDG_GETPROPERTY || evt == EVT_WDG_SETPROPERTY)
         && wParam == PROP_VISIBLE 
         && ((WidgetVis*)dwParam)->piw == me->pChild) {

      IWidget *piwParent = 0;
      if (SUCCESS == IWIDGET_GetParentWidget((IWidget*)po, &piwParent)) {

         boolean bHandled;

         // ask the parent if we, not our child, is visible
         ((WidgetVis*)dwParam)->piw = (IWidget*)po;

         bHandled = IWIDGET_HandleEvent(piwParent, evt, wParam, dwParam);

         // restore the original child widget in struct
         ((WidgetVis*)dwParam)->piw = me->pChild;

         RELEASEIF(piwParent);
         return bHandled;
      }
   }

   if (me->pChild)
      return IWIDGET_HandleEvent(me->pChild, evt, wParam, dwParam);

   return WidgetBase_DefHandleEvent((IWidget*)po, evt, wParam, dwParam);
}


void Decorator_GetPreferredExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;

   if (me->pChild)
      IWIDGET_GetPreferredExtent(me->pChild, pe);
   else 
      SETWEXTENT(pe, 0, 0);
}


void Decorator_GetExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;

   if (me->pChild)
      IWIDGET_GetExtent(me->pChild, pe);
}


void Decorator_SetExtent(IDecorator *po, WExtent *pe)
{
   ME_FROM_DECORATOR;
   
   if (me->pChild)
      IWIDGET_SetExtent(me->pChild, pe);
}


void Decorator_SetHandler(IDecorator *po, HandlerDesc *phd)
{
   WidgetBase_SetHandler((IWidget*)po, phd);
}

uint32 Decorator_Release(IDecorator *po)
{
   ME_FROM_DECORATOR;

   if (me->base.nRefs == 1) {
      Decorator_Dtor(me);
   }

   return WidgetBase_Release(DECORATOR_TO_IWIDGET(me));
}


int Decorator_QueryInterface(IDecorator *po, AEECLSID clsid, void **ppo)
{
   ME_FROM_DECORATOR;
   
   if (SUCCESS == WidgetBase_QueryInterface(IDECORATOR_TO_IWIDGET(po), clsid, ppo)) {
      return SUCCESS;
   }

   if (clsid == AEEIID_CONTAINER) {
      *ppo = (void *)&me->container;
      IDECORATOR_AddRef(po);
      return SUCCESS;
   } else if (clsid == AEEIID_DECORATOR) {
      *ppo = po;
      IDECORATOR_AddRef(po);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}


////////////////////////////////////////////////////////////
// IDecorator members

void Decorator_SetWidget(IDecorator *po, IWidget *pChild)
{
   ME_FROM_DECORATOR;

   if (me->pChild)
      IWIDGET_Release(me->pChild);

   me->pChild = pChild;
   if (pChild) {
      IWIDGET_AddRef(pChild);
      IWIDGET_SetParent(pChild, &me->container);
   }
}

void Decorator_GetWidget(IDecorator *po, IWidget **ppo)
{
   ME_FROM_DECORATOR;
   
   *ppo = me->pChild;
   ADDREFIF(me->pChild);
}

////////////////////////////////////////////////////////////
// IContainer members


static void Container_Invalidate(IContainer *po, IWidget *pw, const AEERect *prc, uint32 dwFlags)
{
   ME_FROM_CONTAINER;

   if (me->base.piContainer)
      ICONTAINER_Invalidate(me->base.piContainer, DECORATOR_TO_IWIDGET(me), prc, dwFlags);
}

static int Container_Locate(IContainer *po, IWidget *pw, IContainer **ppic, AEERect *prc)
{
   ME_FROM_CONTAINER;

   if (me->base.piContainer && me->pChild == pw)
      return ICONTAINER_Locate(me->base.piContainer, DECORATOR_TO_IWIDGET(me), ppic, prc);

   return EFAILED;
}      

static int Container_Insert(IContainer *po, IWidget *piw, IWidget *piwb, const void *pDesc)
{
   ME_FROM_CONTAINER;

   Decorator_SetWidget((IDecorator *)me, piw);

   return SUCCESS;
}

static int Container_Remove(IContainer *po, IWidget *piw)
{
   ME_FROM_CONTAINER;
   
   if (!me->pChild)
      return EBADPARM;

   if ((piw == me->pChild) 
    || (piw == WIDGET_ZNORMAL)
    || (piw == WIDGET_ZTOPMOST)
    || (piw == WIDGET_ZBOTTOMMOST)) {
      Decorator_SetWidget((IDecorator *)me, NULL);
      return SUCCESS;
   }

   return EBADPARM;
}

static IWidget *Container_GetWidget(IContainer *po, IWidget *piwRef, boolean bNext, boolean bWrap)
{
   ME_FROM_CONTAINER;

   // When not wrapping we can't return a widget if the reference is
   // our child
   if (piwRef == me->pChild && !bWrap)
      return NULL;

   return me->pChild;
}

////////////////////////////////////////////////////////////
// Decorator util functions


void Decorator_Invalidate(IDecorator *po, const AEERect *prc, uint32 dwFlags)
{
   ME_FROM_DECORATOR;
   
   if (me->base.piContainer)
      ICONTAINER_Invalidate(me->base.piContainer, DECORATOR_TO_IWIDGET(me), prc, dwFlags);
}

int Decorator_Locate(IDecorator *po, IContainer **ppic, AEERect *prc)
{
   ME_FROM_DECORATOR;

   if (!me->base.piContainer)
      return EFAILED;

   return ICONTAINER_Locate(me->base.piContainer, DECORATOR_TO_IWIDGET(me), ppic, prc);
}


void Decorator_ClientInvalidate(IDecorator *po, const AEERect *prcClient, 
                                const AEERect *prcIn, uint32 dwFlags)
{
   AEERect rc = *prcClient;
   if (prcIn) {
      rc.x += prcIn->x;
      rc.y += prcIn->y;
      rc.dx = prcIn->dx;
      rc.dy = prcIn->dy;
   }
   Decorator_Invalidate(po, &rc, dwFlags);
}


int Decorator_ClientLocate(IDecorator *po, const AEERect *prcClient, IWidget *piw, 
                           IContainer **ppic, AEERect *prc)
{
   ME_FROM_DECORATOR;
   
   if (piw != me->pChild) {
      return EFAILED;
   }

   prc->x += prcClient->x;
   prc->y += prcClient->y;

   return Decorator_Locate(po, ppic, prc);
}

////////////////////////////////////////////////////////////
// DecoratorWidget specific functions

void Decorator_Dtor(Decorator *me) 
{
   RELEASEIF(me->pChild);
   WidgetBase_Dtor(&me->base);
}


void Decorator_Ctor(Decorator *me, AEEVTBL(IDecorator) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, (AEEVTBL(IWidget) *)pvt, piModule, DEFHANDLER(Decorator_HandleEvent));

   me->pChild = 0;

   // Widget functions
   #define VTW(name)     pvt->name = Decorator_##name
   
   VTW( Release );
   VTW( QueryInterface );
   VTW( GetPreferredExtent );
   VTW( GetExtent );
   VTW( SetExtent );
   VTW( Draw );
   VTW( IntersectOpaque );
   VTW( SetHandler );

   // Decorator functions
   VTW( SetWidget );
   VTW( GetWidget );
   
   // Container functions
   AEEBASE_INIT(me, container, &me->vtContainer);
   me->vtContainer.AddRef = AEEBASE_AddRef(IContainer);
   me->vtContainer.Release = AEEBASE_Release(IContainer);
   me->vtContainer.QueryInterface = AEEBASE_QueryInterface(IContainer);
   me->vtContainer.Invalidate = Container_Invalidate;
   me->vtContainer.Locate = Container_Locate;
   me->vtContainer.Insert = Container_Insert;
   me->vtContainer.Remove = Container_Remove;
   me->vtContainer.GetWidget = Container_GetWidget;
}
