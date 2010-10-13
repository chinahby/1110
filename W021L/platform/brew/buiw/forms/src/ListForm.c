/*
  ========================================================================

  FILE:  ListForm.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a list form

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ListForm.h"
#include "FormUtil.h"
#include "AEEResFile.h"
#include "AEERootForm.h"
#include "AEEListModel.h"
#include "AEEScrollWidget.h"

#include "AEEListWidget.h"
#include "AEEWProperties.h"

#define ME_FROM_FORM ListForm *me = (ListForm *)po;

static void ListForm_UseListWidget(ListForm *me, IWidget *piList);


boolean ListForm_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_FORM;

   if (evt == EVT_WDG_GETPROPERTY) {
      
      switch(wParam) {
      
      case WID_LIST:
         *(IWidget**)dwParam = me->piList;
         ADDREFIF(me->piList);
         return TRUE;
      case WID_LISTITEM:
         IDECORATOR_GetWidget((IDecorator*)me->piList, (IWidget **)dwParam);
         return TRUE;
      case WID_DECORATOR:
         *(IDecorator **)dwParam = (IDecorator*)me->piList;
         ADDREFIF(me->piList);
         return TRUE;
      case WID_SCROLLBAR:
         *(IWidget **)dwParam = me->piScrollbar;
         ADDREFIF(me->piScrollbar);
         return TRUE;

      case FID_VIEWMODEL:
         IWIDGET_GetViewModel(me->piList, (IModel **)dwParam);
         return TRUE;
      case FID_LISTMODEL:
         (void) IWIDGET_GetModel(me->piList, AEEIID_LISTMODEL, (IModel **) dwParam);
         return TRUE;
      case FID_LISTINDEX:
         IWIDGET_GetFocusIndex(me->piList, (int*)dwParam);
         return TRUE;
      }
   } else if (evt == EVT_WDG_SETPROPERTY) {
      WExtent extent;

      switch(wParam) {
         
      case WID_LIST:
         RELEASEIF(me->piList);
         me->piList = (IWidget*)dwParam;
         if (me->piList) {
            ListForm_UseListWidget(me, me->piList);
            IWIDGET_AddRef(me->piList);
         }
         return TRUE;

      case WID_LISTITEM:
         IDECORATOR_SetWidget((IDecorator*)me->piList, (IWidget *)dwParam);
         if (dwParam) {
            IWIDGET_GetPreferredExtent((IWidget *)dwParam, &extent);
            IWIDGET_SetItemHeight(me->piList, extent.height);
         }
         return TRUE;

      case WID_SCROLLBAR:
         RELEASEIF(me->piScrollbar);
         me->piScrollbar = (IWidget *)dwParam;
         ADDREFIF(me->piScrollbar);
         return TRUE;

      case FID_LISTMODEL:
         IWIDGET_SetModel(me->piList, (IModel *)dwParam);
         return TRUE;

      case FID_LISTINDEX:
         IWIDGET_SetFocusIndex(me->piList, dwParam);
         return TRUE;

      case FID_SELECTHANDLER: {
         SelectHandler *psh = (SelectHandler*)dwParam;
         me->pfnSelect = psh->pfn;
         me->pvSelect = psh->pv;
         return TRUE;
      }

      case FID_THEME:
         {
            IWidget *piListItem = 0;
            IResFile *piThemeFile = 0;

            if (me->piList)
               IDECORATOR_GetWidget((IDecorator*)me->piList, &piListItem);

            if (SUCCESS == IROOTFORM_GetThemeFile(FORM_ROOT(me), &piThemeFile)) {
               if (me->piScrollbar) 
                  IWIDGET_SetProperties(me->piScrollbar, piThemeFile, FORM_THEME_BASENAME(me), "List.Scrollbar.Properties", 0);

               if (piListItem) {
                  Theme_ApplyListProps(piThemeFile, me->piList, piListItem, FORM_THEME_BASENAME(me));
                  IWIDGET_GetPreferredExtent(piListItem, &extent);
                  IWIDGET_SetItemHeight(me->piList, extent.height);
               }
            }
            
            RELEASEIF(piListItem);
            RELEASEIF(piThemeFile);
            break; // fall through
         }
      }
   }

   return Form_HandleEvent(po, evt, wParam, dwParam);
}

void ListForm_Dtor(ListForm *me)
{
   LISTENER_Cancel(&me->selectListener);
   RELEASEIF(me->piList);
   RELEASEIF(me->piScrollbar);
   Form_Dtor(&me->base);
}

uint32 ListForm_Release(IForm *po)
{
   ME_FROM_FORM;

   if (FORM_NREFS(me) == 1) {
      ListForm_Dtor(me);
   }

   return Form_Release(po);
}

static void ListForm_ItemSelected(ListForm *me, ModelEvent *pe)
{
   if (pe->evCode == EVT_MDL_FOCUS_SELECT) {
      if (me->pfnSelect)
         me->pfnSelect(me->pvSelect, pe->dwParam);
   }
}

static void ListForm_UseListWidget(ListForm *me, IWidget *piList)
{
   if (piList) {
      IModel *pimFocus = NULL;
      IWIDGET_GetViewModel(piList, (IModel **)&pimFocus);
      if (pimFocus) {
         IMODEL_AddListenerEx(pimFocus, &me->selectListener, (PFNLISTENER)ListForm_ItemSelected, me);
         IMODEL_Release(pimFocus);
      }
   }
}

void ListForm_Ctor(ListForm *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                   IShell *piShell, IWidget *piList, IWidget *piScrollbar, 
                   IWidget *piFormWidget, PFNHANDLER pfnDefHandler)
{
   Form_Ctor(&me->base, pvt, piModule, piShell, DEFHANDLER(ListForm_HandleEvent));
   
   pvt->Release = ListForm_Release;

   IFORM_SetWidget(LISTFORM_TO_IFORM(me), WID_FORM, piFormWidget);
   IFORM_SetWidget(LISTFORM_TO_IFORM(me), WID_LIST, piList);
   IFORM_SetWidget(LISTFORM_TO_IFORM(me), WID_SCROLLBAR, piScrollbar);

   IWIDGET_SetBorderWidth(piList, 0);
   IWIDGET_SetFlags(piList, LWF_WRAP);
}


int ListForm_New(IForm **ppo, IModule *piModule, IShell *piShell, boolean bScrollbar)
{
   int nErr = SUCCESS;
   IWidget *piList = 0;
   IDecorator *piScrollbar = 0;
   IWidget *piFormWidget = 0;

   ListForm *me = MALLOCREC_VTBL(ListForm, IForm);
   if (!me) {
      nErr = ENOMEMORY;
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_LISTWIDGET, (void **)&piList);
   }

   if (!nErr && bScrollbar) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_SCROLLBARWIDGET, (void **)&piScrollbar);
      if (!nErr) {
         IWIDGET_SetFlags((IWidget*)piScrollbar, SBWF_NOAUTOHIDE);
         IDECORATOR_SetWidget(piScrollbar, piList);
         piFormWidget = (IWidget*)piScrollbar;
      }
   }

   if (!nErr) {
      if (!piFormWidget) {
         piFormWidget = piList;
      }
      IWIDGET_AddRef(piFormWidget); // extra refcount cuz 2 pointers to same object
      ListForm_Ctor(me, GETVTBL(me,IForm), piModule, piShell, piList, (IWidget *)piScrollbar, piFormWidget, NULL);
   } else {
      FREEIF(me);
   }

   *ppo = (IForm*)me;

   RELEASEIF(piList);
   RELEASEIF(piScrollbar);
   RELEASEIF(piFormWidget);

   return nErr;
}
