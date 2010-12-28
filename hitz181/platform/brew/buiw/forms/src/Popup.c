/*
  ========================================================================

  FILE:  Popup.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Popup implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "Popup.h"
#include "FormUtil.h"
#include "AEERootForm.h"
#include "AEEResFile.h"

#include "AEEWProperties.h"
#include "AEEImageStaticWidget.h"
#include "AEEBorderWidget.h"
#include "AEEListWidget.h"
#include "AEEListModel.h"
#include "AEEArrayModel.h"
#include "AEEMenuModel.h"
#include "AEEBlendWidget.h"
#include "AEEScrollWidget.h"


// wrap a widget in a decorator 
// and then replace it with the decorator
static __inline void WrapWidget(IWidget **ppiw, IDecorator *pid) {
   
   IDECORATOR_SetWidget(pid, *ppiw);
   RELEASEIF(*ppiw);
   *ppiw = (IWidget*)pid;
}


#define ME_FROM_FORM Popup *me = (Popup *)po

void Popup_Dtor(Popup *me);

uint32 Popup_Release(IForm *po)
{
   ME_FROM_FORM;

   if (FORM_NREFS(me) == 1) 
      Popup_Dtor(me);

   return Form_Release(po);
}

static void Popup_fadeComplete(Popup *me)
{   
   Popup_removeBackdrop((Popup *)me);
   IFORM_Release(POPUP_TO_IFORM(me));
}

void Popup_fadeRemoveBackdrop(Popup *me)
{
   FaderDesc fader;
   
   if (me->piBackFader && POPUP_BACKDROP(me) && !ITRANSITION_IsRunning(me->piBackFader)) {
      int trans;
      // Addref form to make sure the transition callback
      // can still refer to me.  The form is released in
      // fadeComplete
      IFORM_AddRef(POPUP_TO_IFORM(me));
      
      IWIDGET_GetTransparency(POPUP_BACKDROP(me), &trans);
      FADERDESC_Init(&fader, POPUP_BACKDROP(me), trans, 255);
      ITRANSITION_StartFader(me->piBackFader, &fader, POPUP_FADEMS(me), (PFNNOTIFY)Popup_fadeComplete, me);
   } else if (!me->piBackFader && POPUP_BACKDROP(me)) {
      Popup_removeBackdrop((Popup *)me);
   }
}

void Popup_removeBackdrop(Popup *me) 
{
   if (!FORM_ROOT(me)) {
      return;
   }

   // Remove backdrop
   if (me->piBackdrop) {
      IXYContainer *piContainer;
      AEERect rc;

      IROOTFORM_GetClientRect(FORM_ROOT(me), &piContainer, &rc);
      IXYCONTAINER_Remove(piContainer, me->piBackdrop);
      IXYCONTAINER_Release(piContainer);      
   }
}

static void Popup_addBackdrop(Popup *me)
{
   AEERect rc;
   WidgetPos pos;
   WExtent extent;
   IXYContainer *piContainer;

   if (!FORM_ROOT(me)) {
      return;
   }

   IROOTFORM_GetClientRect(FORM_ROOT(me), &piContainer, &rc);

   // Only change these properties the first time, as doing an insert of the backdrop
   // subsequent times will cause the backdrop to be in the container multiple times
   if (IXYCONTAINER_GetWidget(piContainer, me->piBackdrop, TRUE, TRUE) == NULL) {
      pos.bVisible = TRUE;
      
      if (me->piBackdrop) {
         // Set backdrop size and position
         SETWEXTENT(&extent, rc.x + rc.dx, rc.y + rc.dy);
         IWIDGET_SetExtent(me->piBackdrop, &extent);

         pos.x = 0;
         pos.y = 0;

         IXYCONTAINER_Insert(piContainer, me->piBackdrop, FORM_WIDGET(me), &pos);
      }
   }

   IXYCONTAINER_Release(piContainer);
}

boolean Popup_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

   ME_FROM_FORM;

   if (evt == EVT_FORM_CANCEL) {
      Popup_fadeRemoveBackdrop((Popup *)me);
      
   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case WID_DECORATOR:
         *(IDecorator **)dwParam = me->piDecorator;
         ADDREFIF(me->piDecorator);
         return TRUE;

      case WID_BACKDROP:
         *(IWidget **)dwParam = me->piBackdrop;
         ADDREFIF(me->piBackdrop);
         return TRUE;

      case FID_FADEMS:
         *(uint32 *)dwParam = POPUP_FADEMS(me);
         return TRUE;
      }
   } else if (evt == EVT_WDG_SETPROPERTY) {
      switch(wParam) {

      case FID_ACTIVE:
         if ((boolean)dwParam)
            Popup_addBackdrop(me);
         else if (!me->piBackFader || !ITRANSITION_IsRunning(me->piBackFader))
            // only remove backdrop if the transition is not running
            Popup_removeBackdrop(me);
         // fall through to form
         break;

      case FID_ROOT:
         // If root wants to be removed we don't release if 
         // the transition is active.
         if (dwParam == 0 && me->piBackFader && ITRANSITION_IsRunning(me->piBackFader))
            return TRUE;
         // fall through to form
         break;

      case FID_THEME:
         if (me->piBackdrop) 
            IROOTFORM_ApplyWProps(FORM_ROOT(me), me->piBackdrop, FORM_THEME_BASENAME(me), "Backdrop.Properties", 0);
         // fall through to form
         break;

      case FID_FADEMS:
         POPUP_FADEMS(me) = dwParam;
         return TRUE;

      case WID_BACKDROP:
         if (FORM_ACTIVE(me))
            Popup_removeBackdrop(me);
         RELEASEIF(me->piBackdrop);
         me->piBackdrop = (IWidget *) dwParam;
         ADDREFIF(me->piBackdrop);
         if (FORM_ACTIVE(me))
            Popup_addBackdrop(me);
         return TRUE;
      }
   }

   return Form_HandleEvent(po, evt, wParam, dwParam);
}

int Popup_Construct(Popup *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                    IShell *piShell, boolean bBackdrop, PFNHANDLER pfnDefHandler)
{
   int result;

   Form_Ctor(&me->base, (AEEVTBL(IForm) *)pvt, piModule, piShell, DEFHANDLER(Popup_HandleEvent));

   pvt->Release     = Popup_Release;

   FORM_ISPOPUP(me) = 1;
   POPUP_FADEMS(me) = 200; // common fade out time 

   result = ISHELL_CreateInstance(piShell, AEECLSID_BORDERWIDGET, (void **)&me->piDecorator);

   if (result == 0) {
      FORM_WIDGET(me) = IDECORATOR_TO_IWIDGET(me->piDecorator);
      IWIDGET_SetBorderWidth(FORM_WIDGET(me), 0);
      IWIDGET_SetShadowOffset(FORM_WIDGET(me), 1);
      // Addref again to reflect double reference
      IDECORATOR_AddRef(me->piDecorator);
   }

   // always try create a backfader so we'll fade backdrops if possible
   if (result == SUCCESS) 
      ISHELL_CreateInstance(piShell, AEECLSID_FADER, (void **)&POPUP_BACKFADER(me));

   if (bBackdrop && result == SUCCESS) {
      result = ISHELL_CreateInstance(piShell, AEECLSID_BORDERWIDGET, (void **)&POPUP_BACKDROP(me));

      if (result == SUCCESS) {
         IWIDGET_SetBorderWidth(POPUP_BACKDROP(me), 0);
         
         // By default the backdrop will be light gray and fades out in 200ms
         IWIDGET_SetBGColor(POPUP_BACKDROP(me), MAKE_RGB(192,192,192));
         IWIDGET_SetTransparency(POPUP_BACKDROP(me), BACKDROP_TRANS);
      }      
   }

   if (result != 0)  
      Popup_Dtor(me);
   
   return result;
}


void Popup_Dtor(Popup *me)
{
   RELEASEIF(me->piDecorator);
   RELEASEIF(me->piBackdrop);
   RELEASEIF(me->piBackFader);
   
   Form_Dtor(&me->base);
}

/////////////////////////////////////////////////////////////////
// Concrete implementation

int Popup_New(IForm **ppo, IModule *piModule, IShell *piShell)
{
   typedef struct PopupImpl {
      Popup base;
      AEEVTBL(IForm) vtForm;
   } PopupImpl;

   PopupImpl *me = MALLOCREC(PopupImpl);

   *ppo = (IForm *)me;

   if (!me)
      return ENOMEMORY;

   return Popup_Construct(&me->base, &me->vtForm, piModule, piShell, FALSE, NULL);
}

/////////////////////////////////////////////////////////////////
// PopupMenu


#define MAKE_ME(cls)    cls *me = (cls*)po

#define MENU_BOTTOM_PAD  2

#define MENU_PCT_WIDTH  80 // Minimum width (percent) of client area menu should take
#define MENU_PCT_HEIGHT 70 // Minimum height (percent) of client area menu should take


static __inline int PopupMenu_GetMenuModel(PopupMenu *me, IMenuModel **ppo)
{
   return IWIDGET_GetModel(me->piList, AEEIID_MENUMODEL, (IModel **)ppo);
}

int PopupMenu_LoadMenuItem(IPopupMenu *po, const char *pszResFile, uint16 idResText, uint16 idMenuCmd, uint16 flags)
{
   MAKE_ME(PopupMenu);
   AECHAR *pItem = NULL;
   IMenuModel *piMenuModel = 0;
   int nErr;

   nErr = ISHELL_LoadResStringEx(FORM_SHELL(me), pszResFile, (uint16)idResText, &pItem);
   
   if (!nErr) 
      nErr = PopupMenu_GetMenuModel(me, &piMenuModel);

   if (!nErr)
      IMENUMODEL_Add(piMenuModel, pItem, idMenuCmd, flags);

   RELEASEIF(piMenuModel);
   return nErr;
}

int PopupMenu_SetMenuItemFlags(IPopupMenu *po, uint16 idCmd, uint16 flags, boolean bSet)
{
   MAKE_ME(PopupMenu);
   IMenuModel *piMenuModel = 0;
   int nErr;

   nErr = PopupMenu_GetMenuModel(me, &piMenuModel);

   if (!nErr)
      IMENUMODEL_SetItemFlags(piMenuModel, idCmd, flags, bSet);

   RELEASEIF(piMenuModel);
   return nErr;
}



static void PopupMenu_OptionSelected(PopupMenu *me, ModelEvent *pEvent)
{
   if (pEvent->evCode == EVT_MDL_FOCUS_SELECT) {
      IMenuModel *piMenuModel = 0;
      uint16 idCmd;

      // if valid pos and handler installed
      if (me->pfnSelect 
          && SUCCESS == PopupMenu_GetMenuModel(me, &piMenuModel)
          && SUCCESS == IMENUMODEL_GetAt(piMenuModel, (uint16)pEvent->dwParam, MMF_ENABLED, 0, 0, &idCmd)) {
         me->pfnSelect(me->pvSelect, idCmd);
      }

      RELEASEIF(piMenuModel);
   }
}

static void PopupMenu_onCancelComplete(IPopupMenu *po)
{
   IFORM_Release((IForm*)po);
   IFORM_HandleEvent((IForm*)po, EVT_FORM_POP, 0, 0);
}

int PopupMenu_QueryInterface(IPopupMenu *po, AEECLSID clsid, void **ppo)
{
   MAKE_ME(PopupMenu);

   if (clsid == AEEIID_POPUPMENUFORM) {
      *ppo = po;
      IPOPUPMENU_AddRef(po);
      return SUCCESS;
   }

   return Form_QueryInterface(POPUPMENU_TO_IFORM(me), clsid, ppo);
}

boolean PopupMenu_HandleEvent(IPopupMenu *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   MAKE_ME(PopupMenu);

   if (evt == EVT_FORM_CANCEL) {
      if (me->piFader && !ITRANSITION_IsRunning(me->piFader)) {
         FaderDesc fader;
         FADERDESC_Init(&fader, FORM_WIDGET(me), 0, 255);
      
          // Addref the form to make sure it survives the transition
         IFORM_AddRef((IForm*)po);
         ITRANSITION_StartFader(me->piFader, &fader, POPUP_FADEMS(me), (PFNNOTIFY)PopupMenu_onCancelComplete, po);    
      } else if (!me->piFader) {
         IFORM_HandleEvent((IForm*)po, EVT_FORM_POP, 0, 0);
      }
      return TRUE;       
   } else if (evt == EVT_WDG_GETPROPERTY) {

      switch(wParam) {

      case WID_LIST:
         *(IWidget**)dwParam = me->piList;
         ADDREFIF(me->piList);
         return TRUE;

      case WID_LISTITEM:
         *(IWidget**)dwParam = me->piItem;
         ADDREFIF(me->piItem);
         return TRUE;

      case WID_SCROLLBAR:
         *(IWidget**)dwParam = me->piScrollbar;
         ADDREFIF(me->piScrollbar);
         return TRUE;
         
      case FID_MENUMODEL:
         return SUCCESS == PopupMenu_GetMenuModel(me, (IMenuModel **)dwParam);

      case FID_LISTMODEL:
         return (SUCCESS == IWIDGET_GetModel(me->piList, AEEIID_LISTMODEL, (IModel **)dwParam));

      case FID_ARRAYMODEL:
         return (SUCCESS == IWIDGET_GetModel(me->piList, AEEIID_ARRAYMODEL, (IModel **)dwParam));
   
      case FID_VIEWMODEL:
         return (SUCCESS == IWIDGET_GetViewModel(me->piList, (IModel **)dwParam));

      case FID_PREFRECT:
         Form_SetPrefRect((IForm*)po, MENU_PCT_WIDTH, 0, 50, -(MENU_BOTTOM_PAD), 
                          FALSE, TRUE, (AEERect*)dwParam);
         return TRUE;
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {

      switch(wParam) {
         
      case WID_STATIC:
         if (me->piItem) {
            IWIDGET_SetImageStaticStaticWidget(me->piItem, (IWidget*)dwParam);
         }
         return TRUE;

      case WID_LIST:
         RELEASEIF(me->piList);
         me->piList = (IWidget *)dwParam;
         ADDREFIF(me->piList);
         // Re-register with viewmodel for selection
         if (me->piList) {
            IModel *piModel = 0;
            if (IWIDGET_GetViewModel(me->piList, (IModel**)&piModel) == 0) {
               LISTENER_Cancel(&me->ml);
               IMODEL_AddListenerEx(piModel, &me->ml, (PFNLISTENER)PopupMenu_OptionSelected, me);
               IMODEL_Release(piModel);
            }
         }
         return TRUE;

      case WID_LISTITEM: 
         RELEASEIF(me->piItem);
         me->piItem = (IWidget*)dwParam;
         ADDREFIF(me->piItem);

         IDECORATOR_SetWidget((IDecorator*)me->piList, me->piItem);
         // Set the new itemheight of the list
         if (me->piItem) {
            WExtent we;
            IWIDGET_GetPreferredExtent(me->piItem, &we);
            IWIDGET_SetItemHeight(me->piList, we.height);
         }
         return TRUE;

      case WID_SCROLLBAR:
         RELEASEIF(me->piScrollbar);
         me->piScrollbar = (IWidget *)dwParam;
         ADDREFIF(me->piScrollbar);
         return TRUE;

      case FID_SELECTHANDLER: {
         SelectHandler *psh = (SelectHandler*)dwParam;
         me->pfnSelect = psh->pfn;
         me->pvSelect = psh->pv;
         return TRUE;
      }

      case FID_LISTMODEL:
      case FID_ARRAYMODEL:
      case FID_MENUMODEL:
         IWIDGET_SetModel(me->piList, (IModel*)dwParam);
         return TRUE;
   
      case FID_THEME:
         {
            IResFile *piThemeFile;
            WExtent we;

            if (SUCCESS == IROOTFORM_GetThemeFile(FORM_ROOT(me), &piThemeFile)) {
               Theme_ApplyListProps(piThemeFile, me->piList, me->piItem, FORM_THEME_BASENAME(me));
               IWIDGET_SetProperties(me->piScrollbar, piThemeFile, FORM_THEME_BASENAME(me), "List.Scrollbar.Properties", 0);
            }
               
            IWIDGET_GetPreferredExtent(me->piItem, &we);
            IWIDGET_SetItemHeight(me->piList, we.height);
            RELEASEIF(piThemeFile);
         }
         // Fall through to popup
         break;

      case FID_ROOT:
         // On first attach, set index to 0
         if (dwParam) {
            IWIDGET_SetFocusIndex(FORM_WIDGET(me), 0);
         }
         // Fall through to popup
         break;
            
      case FID_ACTIVE:
         if ((boolean)dwParam) {
            // Set transparency
            IWIDGET_SetTransparency(FORM_WIDGET(me), 0);
         }
         // Fall through to popup
         break;
      }
   
   } else if (evt == EVT_KEY && wParam == AVK_SOFT1) {
      return IWIDGET_HandleEvent(me->piList, EVT_KEY, AVK_SELECT, 0);
   }

   return Popup_HandleEvent((IForm*)po, evt, wParam, dwParam);
}


void PopupMenu_Indexer(PopupMenu *me, int nIndex, boolean bSelected)
{
   uint16 flags;
   int    result;
   IMenuModel *piMenuModel = 0;

   result = PopupMenu_GetMenuModel(me, &piMenuModel);

   if (SUCCESS == result) 
      result = IMENUMODEL_GetAt(piMenuModel, (uint16) nIndex, MENUITEM_ALL, NULL, &flags, NULL);

   if (SUCCESS == result) {
      uint32 miFlags = 0;

      (void) IWIDGET_GetFlags(me->piItem, &miFlags);

      if (flags & MMF_SUBMENU) {
         miFlags |= MMF_SUBMENU;
      } else {
         miFlags &= ~MMF_SUBMENU;
      }

      if (flags & MMF_SEPARATOR) {
         miFlags |= MMF_SEPARATOR;
      } else {
         miFlags &= ~MMF_SEPARATOR;
      }

      (void) IWIDGET_SetFlags(me->piItem, miFlags);
   }

   RELEASEIF(piMenuModel);
}


static void PopupMenu_FreeMenuItem(void *pItem)
{
   FREEIF(pItem);
}

void PopupMenu_Dtor(PopupMenu *me)
{
   LISTENER_Cancel(&me->ml);

   RELEASEIF(me->piList);
   RELEASEIF(me->piItem);
   RELEASEIF(me->piFader);

   Popup_Dtor(&me->base);
}

uint32 PopupMenu_Release(IPopupMenu *po) 
{
   MAKE_ME(PopupMenu);
   if (FORM_NREFS(me) == 1)
      PopupMenu_Dtor(me);
   
   return Popup_Release((IForm*)po);
}


int PopupMenu_Construct(PopupMenu *me, AEEVTBL(IPopupMenu) *pvt, IModule *piModule, 
                        IShell *piShell, boolean bScrollbar, PFNHANDLER pfnDefHandler)
{
   int result;
   IDecorator *piBlender = 0;
   IWidget *piMenu = 0;
   IMenuModel *piMenuModel = 0;

   result = Popup_Construct(&me->base, (AEEVTBL(IForm)*)pvt, piModule, 
                            piShell, TRUE, DEFHANDLER(PopupMenu_HandleEvent));

   if (result == 0) {

#define VTF(name)   pvt->name = PopupMenu_##name

      VTF(LoadMenuItem);
      VTF(SetMenuItemFlags);
      VTF(Release);
      VTF(QueryInterface);

      result = ISHELL_CreateInstance(piShell, AEECLSID_MENUMODEL, (void **)&piMenuModel);
   }

   if (result == 0) {
      IMENUMODEL_SetPfnFree(piMenuModel, PopupMenu_FreeMenuItem);

      result = ISHELL_CreateInstance(piShell, AEECLSID_LISTWIDGET, (void **)&me->piList);
   }

   if (result == 0) {
      piMenu = me->piList;
      IWIDGET_AddRef(piMenu);
   }

   if (result == 0) {
      me->indexer.pCtx = me;
      me->indexer.pfnIndexer = (PFNINDEXER)PopupMenu_Indexer;

      result = IWIDGET_SetProperty(me->piList, PROP_INDEXER, (uint32) &me->indexer);
   }

   if (result == 0) {
      IModel *piModel;

      IWIDGET_SetModel(me->piList, IMENUMODEL_TO_IMODEL(piMenuModel));

      if (IWIDGET_GetViewModel(me->piList, (IModel**)&piModel) == 0) {
         IMODEL_AddListenerEx(piModel, &me->ml, (PFNLISTENER)PopupMenu_OptionSelected, me);
         IMODEL_Release(piModel);
      }

      IWIDGET_SetBGColor(me->piList, RGBA_WHITE);
      IWIDGET_SetBorderColor(me->piList, RGBA_BLACK);
      IWIDGET_SetBorderWidth(me->piList, 1);
      
      result = ISHELL_CreateInstance(piShell, AEECLSID_STATICWIDGET, (void **)&me->piItem);
   }


   if (result == 0) {
      IWIDGET_SetFGColor(me->piItem, RGBA_BLACK);
      IWIDGET_SetBGColor(me->piItem, RGBA_NONE);
      IWIDGET_SetSelectedBGColor(me->piItem, MAKE_RGBA(0xC0, 0xC0, 0xC0, 0xFF));
      IDECORATOR_SetWidget((IDecorator*)me->piList, me->piItem);
   }

   if ((result == 0) && bScrollbar) {
      result = ISHELL_CreateInstance(piShell, AEECLSID_SCROLLBARWIDGET, (void **)&me->piScrollbar);
      if (result == 0) {
         WrapWidget(&piMenu, (IDecorator*)me->piScrollbar);
      }
   }

   if (result == 0) {
      IDECORATOR_SetWidget(POPUP_DECORATOR(me), piMenu);
   }

   // Wrap in blender for fadeout transition
   if (result == 0) {
      result = ISHELL_CreateInstance(piShell, AEECLSID_BLENDWIDGET, (void **)&piBlender);
   }

   // Try and create the transition itself, if fader is supported
   if (result == 0) {
      ISHELL_CreateInstance(piShell, AEECLSID_FADER, (void **)&me->piFader);
   }

   if (result == 0) {
      WrapWidget(&FORM_WIDGET(me), piBlender);
   }

   if (result == 0) {
      result = IFORM_SetThemeBaseName((IForm *)me, "System.PopupMenu");
   }

   RELEASEIF(piMenu);
   RELEASEIF(piMenuModel);
   return result;
}

int PopupMenu_New(IPopupMenu **ppo, IModule *piModule, IShell *piShell, boolean bScrollbar)
{
   PopupMenu *me = MALLOCREC_VTBL(PopupMenu, IPopupMenu);

   *ppo = (IPopupMenu*) me;

   if (!me)
      return ENOMEMORY;

   return PopupMenu_Construct(me, GETVTBL(me, IPopupMenu), piModule, piShell, bScrollbar, NULL);   
}

