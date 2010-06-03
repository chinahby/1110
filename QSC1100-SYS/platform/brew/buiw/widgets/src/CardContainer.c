/*
  ========================================================================

  FILE:  CardContainer.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a "card-deck" style container.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "CardContainer.h"
#include "ModelBase.h"
#include "AEEWProperties.h"
#include "wutil.h"

#define ME_FROM_CONTAINER     CardContainer *me = (CardContainer*)po
#define ME_FROM_WIDGET        CardContainer *me = (CardContainer*)po->pMe

// forward decls
static int CardContainer_privActivate(ICardContainer *po, WidgetNode *pn);
static void CardContainer_NotifyFocus(CardContainer *me);


/////////////////////////////////////////////////////////////////////////////


static IWidget *CardContainer_WidgetAtIndex(CardContainer *me, int nIndex)
{
   int i = 0;
   WidgetNode *pn;

   WNODE_ENUM_REV(pn, &me->base.head) {
      if (i == nIndex) {
         return pn->piWidget;
      }
      i++;
   }

   return NULL;
}

static int WidgetNode_NewForCard(const void* pDesc, IWidget *piw, WidgetNode **ppo)
{
   AEERect rc;
   WExtent we;

   *ppo = MALLOCREC(WidgetNode);
   if (!*ppo) {
      return ENOMEMORY;
   }

   IWIDGET_GetExtent(piw, &we);
   SETAEERECT(&rc, 0, 0, we.width, we.height);

   // add it invisible by default
   WidgetNode_Ctor(*ppo, piw, &rc, 0);
   return SUCCESS;
}


void CardContainer_Dtor(CardContainer *me)
{
   LISTENER_Cancel(&me->mlFocus);
   CALLBACK_Cancel(&me->cbkFocus);
   RELEASEIF(me->piShell);
}


uint32 CardContainer_Release(ICardContainer *po)
{
   ME_FROM_CONTAINER;

   if (CBASE(me)->nRefs == 1) {
      CardContainer_Dtor(me);
   }

   return ContainerBase_Release((IContainer*)po);
}


int CardContainer_QueryInterface(ICardContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_CARDCONTAINER) {
      *ppo = po;
      ICONTAINER_AddRef((IContainer*)po);
      return SUCCESS;
   } 

   return ContainerBase_QueryInterface((IContainer*)po, id, ppo);
}


int CardContainer_Insert(ICardContainer *po, IWidget *piw, IWidget *piwBefore, const void *pDesc)
{
   ME_FROM_CONTAINER;

   int nErr = ContainerBase_Insert((IContainer*)po, piw, piwBefore, pDesc);
   
   if (!nErr) {
      // active this one if first one in deck
      if (!me->pnActive) {
         WidgetNode *pn;
         if (SUCCESS == ContainerBase_FindWidget(&me->base, piw, &pn, NULL, NULL)) {
            CardContainer_privActivate(po, pn);
         }
      }
      // if no current focus, set focus to this one
      if (!me->base.pFocus) {
         IWIDGET_MoveFocus(&me->base.widget, piw);
      }
   }
   
   return nErr;
}


int CardContainer_Remove(ICardContainer *po, IWidget *piw)
{
   ME_FROM_CONTAINER;

   IWidget *piwNext = 0;
   int nErr;
   WidgetNode *pn;
   
   nErr = ContainerBase_FindWidget(&me->base, piw, &pn, &piw, NULL);

   if (!nErr) {

      ContainerBase_Invalidate((IContainer*)po, piw, NULL, 0);

      if ( (me->pnActive && me->pnActive->piWidget == piw) ||
           (me->base.pFocus && me->base.pFocus->piWidget == piw) ) {
         piwNext = WidgetNode_GetWidget(&me->base.head, piw, 1, 1);
         me->pnActive = 0;    // forget active widget
      }

      ContainerBase_WidgetNodeRemove(&me->base, pn);

      if (piwNext && piwNext != piw) {
         CardContainer_Activate(po, piwNext);
      }
   }

   return nErr;
}


int CardContainer_Activate(ICardContainer *po, IWidget *piw)
{
   ME_FROM_CONTAINER;

   WidgetNode *pnActive = me->pnActive;
   int nIndexSave = me->nFocusIndex;
   WidgetNode *pn;
   int nIndex;
   int nErr;
   
   nErr = ContainerBase_FindWidget(&me->base, piw, &pn, &piw, &nIndex);

   if (!nErr) {
      nErr = CardContainer_privActivate(po, pn);
   }

   if (!nErr && pnActive != me->pnActive) {
      me->base.pFocus = 0;          // in case the widget can't take focus, we still want to activate.
      // attempt to move focus to newly activated widget
      IWIDGET_MoveFocus(&me->base.widget, me->pnActive->piWidget);
      me->nFocusIndex = nIndex;
   }

   if (!nErr && (nIndexSave != me->nFocusIndex)) {
      CardContainer_NotifyFocus(me);
   }

   return nErr;
}


static int CardContainer_privActivate(ICardContainer *po, WidgetNode *pn)
{
   ME_FROM_CONTAINER;
   
   int nErr = 0;

   if (pn != me->pnActive) {
      if (me->pnActive) {
         me->pnActive->fVisible = 0;
      }
      me->pnActive = pn;
      me->pnActive->fVisible = 1;
      IWIDGET_SetExtent(me->pnActive->piWidget, &me->base.extent);
      ContainerBase_Invalidate((IContainer*)me, me->pnActive->piWidget, NULL, 0);
   }

   return nErr;
}


IWidget* CardContainer_GetActive(ICardContainer *po)
{
   ME_FROM_CONTAINER;
   return me->pnActive ? me->pnActive->piWidget : NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Focus model stuff


static void CardContainer_onFocusChange(CardContainer *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_TAB_CHANGE) {
      IWidget *piw = CardContainer_WidgetAtIndex(me, (int)pEvent->dwParam);
      if (piw) {
         CardContainer_Activate((ICardContainer*)me, piw);
      }
   }
}

static void CardContainer_NotifyFocus(CardContainer *me)
{
   if (me->base.piViewModel) {
      ModelEvent ev;
      ev.evCode   = EVT_MDL_TAB_CHANGE;
      ev.dwParam  = me->nFocusIndex;
      IMODEL_Notify(me->base.piViewModel, &ev);
   }
}

// This implementation of SetViewModel only hooks a listener onto the 
// supplied view model. Our internal view model is used for notifications
static void CardContainer_SetViewModel(CardContainer *me, IModel *piModel)
{
   if (piModel) {
      IMODEL_AddListenerEx(piModel, &me->mlFocus, (PFNLISTENER)CardContainer_onFocusChange, me);
   }
}

/////////////////////////////////////////////////////////////////
// IWidget methods

static boolean CardContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;                  

   if (evt == EVT_WDG_MOVEFOCUS) {
      ContainerBase_DefHandleEvent(po, evt, wParam, dwParam);
      // if there still is a focus, activate the newly focused widget
      if (me->base.pFocus) {
         CardContainer_privActivate((ICardContainer*)me, me->base.pFocus);
      }
      return TRUE;
   }

   if (evt == EVT_WDG_GETPROPERTY) {
      if (wParam == PROP_VIEWMODEL) {
         if (SUCCESS == ContainerBase_GetViewModel(&me->base, (IModel**)dwParam)) {
            CALLBACK_Resume(&me->cbkFocus, CardContainer_NotifyFocus, me, me->piShell);
         }
         return TRUE;
      }
   }
   
   if (evt == EVT_WDG_SETPROPERTY) {
      if (wParam == PROP_VIEWMODEL) {
         CardContainer_SetViewModel(me, (IModel*)dwParam);
         return TRUE;
      }
   }

   return ContainerBase_DefHandleEvent(po, evt, wParam, dwParam);
}


void CardContainer_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;
   ContainerBase_SetExtent(po, pe);
   // set the extent of the active widget
   if (me->pnActive) {
      IWIDGET_SetExtent(me->pnActive->piWidget, &me->base.extent);
   }
}


/////////////////////////////////////////////////////////////////
// CardContainer creation

void CardContainer_Ctor(CardContainer *me, AEEVTBL(ICardContainer) *pvt, 
                        IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ContainerBase_Ctor(&me->base, (AEEVTBL(IContainer)*) pvt, piShell, piModule, 
                      pfnDefHandler ? pfnDefHandler : (PFNHANDLER)CardContainer_DefHandleEvent,
                      WidgetNode_NewForCard, 0/*DefDoLayout*/);

   // ICardContainer vtbl
   pvt->Release         = CardContainer_Release;
   pvt->QueryInterface  = CardContainer_QueryInterface;
   pvt->Insert          = CardContainer_Insert;
   pvt->Remove          = CardContainer_Remove;
   pvt->Activate        = CardContainer_Activate;
   pvt->GetActive       = CardContainer_GetActive;

   // IWidget vtbl
   me->base.vtWidget.SetExtent = CardContainer_SetExtent;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);
}


int CardContainer_New(ICardContainer **ppo, IShell *piShell, IModule *piModule)
{
   CardContainer *me = MALLOCREC_VTBL(CardContainer, ICardContainer);
   *ppo = (ICardContainer*) me;
   if (!me) {
      return ENOMEMORY;
   }
   CardContainer_Ctor(me, GETVTBL(me,ICardContainer), piShell, piModule, 0/*DefHandleEvent*/);
   return SUCCESS;
}

