/*
  ========================================================================

  FILE:  Form.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Base implementation of a Form

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#include "Form.h"
#include "AEERootForm.h"
#include "AEESoftkeyWidget.h"
#include "AEEImageWidget.h"
#include "AEEStaticWidget.h"

#include "AEEResFile.h"
#include "FormUtil.h"

#define ME_FROM_FORM Form *me = (Form *)po

uint32 Form_AddRef(IForm *po)
{
   ME_FROM_FORM;
   return ++me->nRefs;
}

uint32 Form_Release(IForm *po)
{
   ME_FROM_FORM;

   int nRefs = --me->nRefs;

   if (nRefs == 0) {
      // call free handler (if installed)
      HANDLERDESC_Free(&me->handler);
      // then dtor form
      Form_Dtor(me);
      FREE(me);
   }

   return nRefs;
}

int Form_QueryInterface(IForm *po, AEECLSID clsid, void **ppo)
{
   ME_FROM_FORM;

   if (clsid == AEEIID_FORM || clsid == AEEIID_HANDLER ||
       clsid == AEECLSID_QUERYINTERFACE) {
      *ppo = me;
      Form_AddRef(po);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}

void Form_Notify(Form *me, uint32 dwItemMask)
{
   if (me->piModel) {
      FormEvent fe;
      fe.base.evCode = EVT_MDL_FORM_CHANGE;
      fe.dwItemMask  = dwItemMask;
      fe.piForm = FORM_TO_IFORM(me);   // No AddRef because we control scope
      IMODEL_Notify(me->piModel, (ModelEvent*)&fe);
   }
}

static void Form_LoadTextNotify(Form *me, FormRes *pFormRes, AECHAR **ppDest, uint32 dwItemMask)
{
   // Store the old pointer so we can free it after notifications
   AECHAR *pOld = (ppDest) ? *ppDest : NULL;
   FormRes_GetText(pFormRes, FORM_SHELL(me), ppDest);
   Form_Notify(me, dwItemMask);

   // Only free old pointer if it is now different
   if (!ppDest || (pOld != *ppDest)) FREEIF(pOld);
}

static void Form_LoadImageNotify(Form *me, FormRes *pFormRes, IImage **ppDest, uint32 dwItemMask)
{
   // Store the old pointer so we can free it after notifications
   IImage *pOld = (ppDest) ? *ppDest : NULL;
   int nErr = FormRes_GetImage(pFormRes, FORM_SHELL(me), ppDest);   
   Form_Notify(me, dwItemMask);

   // Only free old pointer if it is now different
   if (!nErr || !ppDest || (pOld != *ppDest)) RELEASEIF(pOld);
}

static void Form_UpdatePos(Form *me)
{
   AEERect rc;

   // If we don't have a root form, not much we can do yet
   if (!FORM_ROOT(me)) {
      return;
   }
  
   if (FORM_WIDGET(me) && IFORM_GetProperty(FORM_TO_IFORM(me), FID_PREFRECT, (uint32)&rc) == SUCCESS) {
      IXYContainer *piContainer;
      WExtent extent;
      WidgetPos pos;
      
      // Set size
      SETWEXTENT(&extent, rc.dx, rc.dy);
      IWIDGET_SetExtent(FORM_WIDGET(me), &extent);

      // and position
      IROOTFORM_GetClientRect(FORM_ROOT(me), &piContainer, NULL);
      IXYCONTAINER_GetPos(piContainer, FORM_WIDGET(me), &pos);
      pos.x = rc.x;
      pos.y = rc.y;
      IXYCONTAINER_SetPos(piContainer, FORM_WIDGET(me), WIDGET_ZNORMAL, &pos);
      RELEASEIF(piContainer);
   }  
}

static __inline void Form_VerifyPos(Form *me) {

   // If we are visible and the client region has changed, update our extents
   if (FORM_VISIBLE(me) && FORM_SIZECHANGED(me)) {
      Form_UpdatePos(me);
      FORM_SIZECHANGED(me) = FALSE;
   }
}

static void Form_UpdateRootFormCB(Form *me, FormEvent *pEvent)
{
   if (pEvent && (pEvent->base.evCode == EVT_MDL_FORM_CHANGE) && (pEvent->dwItemMask & FORMITEM_CLIENT)) {
      FORM_SIZECHANGED(me) = TRUE;
      Form_VerifyPos(me);
   }
}

boolean Form_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_FORM;

   if (evt == EVT_FORM_POP && FORM_ROOT(me)) {
      IROOTFORM_RemoveForm(FORM_ROOT(me), po);
      return TRUE;
   } else if (evt == EVT_FORM_CANCEL) {
        return IFORM_HandleEvent(po, EVT_FORM_POP, 0, 0);
   } else if (evt == EVT_KEY) {

      if (wParam == AVK_CLR || (me->wCancelKey && (wParam == me->wCancelKey))) {
         return IFORM_HandleEvent(po, EVT_FORM_CANCEL, 0, 0);
      }

   } else if (evt == EVT_WDG_GETPROPERTY) {

      switch(wParam) {
   
         case WID_FORM:
            *(IWidget **)dwParam = me->piWidget;
            ADDREFIF(me->piWidget);
            return TRUE;
   
         case FID_SHELL:
            *(IShell **)dwParam = FORM_SHELL(me);
            ADDREFIF(FORM_SHELL(me));
            return TRUE;
         
         case FID_ROOT:
            *(IRootForm **)dwParam = FORM_ROOT(me);
            ADDREFIF(FORM_ROOT(me));
            return TRUE;
   
         case FID_THEME_BASENAME:
            *(const char **)dwParam = FORM_THEME_BASENAME(me);
            return TRUE;
   
         case FID_ACTIVE:
            *(boolean *)dwParam = FORM_ACTIVE(me);
            return TRUE;

         case FID_VISIBLE:
            *(boolean *)dwParam = FORM_VISIBLE(me);
            return TRUE;
   
         case FID_BACKGROUND:
            *(IImage**)dwParam = FORM_BGIMAGE(me);
            ADDREFIF(FORM_BGIMAGE(me));
            return TRUE;
   
         case FID_ISPOPUP:   
            *(boolean *)dwParam = FORM_ISPOPUP(me);
            return TRUE;
           
         case FID_TITLE:
            *(AECHAR**)dwParam = FORM_TITLETEXT(me);
            return TRUE;
 
         case FID_TITLEIMAGE:
            *(IImage **)dwParam = FORM_TITLEIMAGE(me);
            ADDREFIF(FORM_TITLEIMAGE(me));
            return TRUE;
        
         case FID_SOFTKEY1:
            *(AECHAR**)dwParam = FORM_SK1TEXT(me);
            return TRUE;

         case FID_SOFTKEY2:
            *(AECHAR**)dwParam = FORM_SK2TEXT(me);
            return TRUE;

         case FID_CANCELKEY:
            *(uint16*)dwParam = me->wCancelKey;
            return TRUE;

         case FID_FORMMODEL:
            // lazy allocate a model on demand
            if (!FORM_MODEL(me)) {
               if (ISHELL_CreateInstance(FORM_SHELL(me), AEECLSID_MODEL, (void **)&FORM_MODEL(me)) != 0) {
                  return FALSE;
               }
            }
            *(IModel**)dwParam = FORM_MODEL(me);
            IMODEL_AddRef(FORM_MODEL(me));
            return TRUE;

         case FID_PREFRECT:
            if (FORM_ROOT(me)) {
               IROOTFORM_GetClientRect(FORM_ROOT(me), NULL, (AEERect *)dwParam);
               return TRUE;
            } 
            return FALSE;
      }
   
   } else if (evt == EVT_WDG_SETPROPERTY) {
      switch (wParam) {
         case FID_FORMMODEL:
            RELEASEIF(FORM_MODEL(me));
            FORM_MODEL(me) = (IModel *)dwParam;
            ADDREFIF(FORM_MODEL(me));
            return TRUE;

         case WID_FORM:
            RELEASEIF(me->piWidget);
            me->piWidget = (IWidget*)dwParam;
            ADDREFIF(me->piWidget);
            Form_Notify(me, FORMITEM_WIDGET);
            return TRUE;
   
         case FID_ROOT:
            FORM_ROOT(me) = (IRootForm *)dwParam;
            LISTENER_Cancel(&me->mlRootForm);
            if (FORM_ROOT(me)) {
               IModel* piModel = NULL;

               // Since we are recieving a new rootform, update size-dependent components
               FORM_SIZECHANGED(me) = TRUE;
               Form_VerifyPos(me);

               // Register listener for future rootform updates               
               IFORM_GetFormModel(IROOTFORM_TO_IFORM(FORM_ROOT(me)), &piModel);
               if (piModel) {
                  IMODEL_AddListenerEx(piModel, &me->mlRootForm, (PFNLISTENER)Form_UpdateRootFormCB, me);
                  RELEASEIF(piModel);
               }
            }           
            return TRUE;
   
         case FID_ACTIVE:
            FORM_ACTIVE(me) = (boolean)dwParam;
            if (FORM_ACTIVE(me)) {
               Form_UpdatePos(me);
            }
            return TRUE;

         case FID_VISIBLE:
            FORM_VISIBLE(me) = (boolean)dwParam;     
            Form_VerifyPos(me);
            return TRUE;

         case FID_THEME_BASENAME:
            FREEIF(FORM_THEME_BASENAME(me));
            FORM_THEME_BASENAME(me) = STRDUP((const char *)dwParam);
            // Make sure strdup succeeded
            if  ((const char *)dwParam && !FORM_THEME_BASENAME(me))
               return FALSE;

            Form_Notify(me, FORMITEM_THEME_BASENAME);
            return TRUE;
   
         case FID_THEME:
            {
               IResFile *piResFile = 0;
               IImage *piImage = 0;
               
               if (SUCCESS == IROOTFORM_GetThemeFile(FORM_ROOT(me), &piResFile)) {
                  // Load background image
                  IRESFILE_GetNamedImage(piResFile, (void **)&piImage,  
                                         FORM_THEME_BASENAME(me), "Background.Image", 0);

                  // Apply form widget properties
                  IWIDGET_SetProperties(FORM_WIDGET(me), piResFile, FORM_THEME_BASENAME(me), "Properties", 0);
               }

               // Replace image if one was found
               if (piImage) {
                  RELEASEIF(FORM_BGIMAGE(me));
                  FORM_BGIMAGE(me) = piImage;
                  IIMAGE_AddRef(piImage);
               };
               
               RELEASEIF(piImage);
               RELEASEIF(piResFile);

               return TRUE;
            }
            
         case FID_BACKGROUND:
            Form_LoadImageNotify(me, (FormRes*)dwParam, &FORM_BGIMAGE(me), FORMITEM_BACKGROUND);
            return TRUE;

         case FID_TITLE:
            Form_LoadTextNotify(me, (FormRes*)dwParam, &FORM_TITLETEXT(me), FORMITEM_TITLE);
            return TRUE;

         case FID_TITLEIMAGE:
            Form_LoadImageNotify(me, (FormRes*)dwParam, &FORM_TITLEIMAGE(me), FORMITEM_TITLE);
            return TRUE;
   
         case FID_SOFTKEY1:
            Form_LoadTextNotify(me, (FormRes*)dwParam, &FORM_SK1TEXT(me), FORMITEM_SOFTKEY);
            return TRUE;
   
         case FID_SOFTKEY2:
            Form_LoadTextNotify(me, (FormRes*)dwParam, &FORM_SK2TEXT(me), FORMITEM_SOFTKEY);
            return TRUE;

         case FID_CANCELKEY:
            me->wCancelKey = (uint16)dwParam;
            return TRUE;
      }
   }

   return FALSE;   
}

static boolean Form_HookedHandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_FORM;
   return HANDLERDESC_Call(&me->handler, evt, wParam, dwParam);
}

void Form_SetHandler(IForm *po, HandlerDesc *phd)
{
   ME_FROM_FORM;

   // Reset on null descriptor
   if (!phd) {
      HANDLERDESC_Init(&me->handler, me->pfnDefHandler, me, 0);
   } else {
      HandlerDesc hdTemp = *phd;
      *phd = me->handler;
      me->handler = hdTemp;
   }
}


/////////////////////////////////////////////////////////////////
// Concrete implementation 
void Form_Ctor(Form *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
               IShell *piShell, PFNHANDLER pfnDefHandler)
{
   me->pvt = pvt;

   me->nRefs = 1;

   me->piModule = piModule;
   ADDREFIF(piModule);

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   pvt->AddRef         = Form_AddRef;
   pvt->Release        = Form_Release;
   pvt->QueryInterface = Form_QueryInterface;
   pvt->HandleEvent    = Form_HookedHandleEvent;
   pvt->SetHandler     = Form_SetHandler;

   me->pfnDefHandler = DEFHANDLER(Form_HandleEvent);
   HANDLERDESC_Init(&me->handler, me->pfnDefHandler, me, 0);

   IFORM_SetThemeBaseName((IForm *)me, "System");

}

void Form_Dtor(Form *me)
{
   LISTENER_Cancel(&me->mlRootForm);
   RELEASEIF(me->piModel);
   RELEASEIF(me->piModule);
   RELEASEIF(me->piWidget);
   RELEASEIF(me->piShell);   
   RELEASEIF(FORM_BGIMAGE(me));
   FREEIF(FORM_TITLETEXT(me));
   RELEASEIF(FORM_TITLEIMAGE(me));
   FREEIF(FORM_SK1TEXT(me));
   FREEIF(FORM_SK2TEXT(me));
   FREEIF(FORM_THEME_BASENAME(me));
}

int Form_New(IForm **ppo, IModule *piModule, IShell *piShell)
{
   Form *me = MALLOCREC_VTBL(Form, IForm);

   *ppo = (IForm *)me;
   
   if (!me)
      return ENOMEMORY;

   Form_Ctor(me, GETVTBL(me, IForm), piModule, piShell, NULL);

   return 0;
}

