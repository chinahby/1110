/*
  ========================================================================

  FILE:  Dialog.c
  
  SERVICES:  

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "Dialog.h"
#include "FormUtil.h"

#include "AEERootForm.h"
#include "AEEResFile.h"

#include "AEEWProperties.h"
#include "AEETitleWidget.h"
#include "AEEBorderWidget.h"
#include "AEEImageWidget.h"
#include "AEEStaticWidget.h"
#include "AEEPropContainer.h"
#include "AEEProgressWidget.h"


#define ME_FROM_FORM Dialog *me = (Dialog *)po

#define DIALOG_PCT_WIDTH  80 // Minimum width (percent) of client area dialog should take up
#define DIALOG_PCT_HEIGHT 70 // Minimum height (percent) of client area dialog should take up

boolean Dialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_FORM;

   if (evt == EVT_FORM_CANCEL) {
      Popup_fadeRemoveBackdrop((Popup *)me);
      // fall through to popup to remove the dialog itself
   } else if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {

      case WID_TITLE:
         IWIDGET_GetProperty(me->piTitle, PROP_TITLEWIDGET, (uint32 *)dwParam);
         return TRUE;

      case WID_BACKGROUND:
         *(IWidget **)dwParam = me->piBackground;
         ADDREFIF(me->piBackground);
         return TRUE;
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {
      IWidget *piWidget = 0;

      switch(wParam) {

      case FID_TITLE:
         if (IFORM_GetTitleWidget(po, &piWidget) == 0) 
            IWIDGET_SetFormText(piWidget, FORM_SHELL(me), (FormRes *)dwParam);
         RELEASEIF(piWidget);
         // Fall through to popup
         break;

      case FID_THEME:
         {
            IResFile *piResFile = 0;

            if (SUCCESS == IROOTFORM_GetThemeFile(FORM_ROOT(me), &piResFile)) {
               IImage *piImage = 0;
               IRESFILE_GetNamedImage(piResFile, (void **)&piImage, FORM_THEME_BASENAME(me), "Dialog.Background.Image", 0);
               IFORM_SetBGImage(po, piImage);
               RELEASEIF(piImage);
            }
            
            if (SUCCESS == IFORM_GetBGWidget(po, &piWidget))
               IROOTFORM_ApplyWProps(FORM_ROOT(me), piWidget, FORM_THEME_BASENAME(me), "Dialog.Background.Properties", 0);
            RELEASEIF(piWidget);

            if (SUCCESS == IFORM_GetWidget(po, WID_FORM, &piWidget))
               IROOTFORM_ApplyWProps(FORM_ROOT(me), piWidget, FORM_THEME_BASENAME(me), "Dialog.Properties", 0);
            RELEASEIF(piWidget);

            if (SUCCESS == IFORM_GetTitleWidget(po, &piWidget))
               IROOTFORM_ApplyWProps(FORM_ROOT(me), piWidget, FORM_THEME_BASENAME(me), "Dialog.Title.Properties", 0);
            RELEASEIF(piWidget);
            
            RELEASEIF(piResFile);
         }
         // Fall through to popup
         break;

      case FID_BACKGROUND:
		 // Do a form notify only if there is success
         if(SUCCESS==IWIDGET_SetFormImage(me->piBackground, FORM_SHELL(me), (FormRes *)dwParam))
		 {
			Form_Notify(&(me->base).base, FORMITEM_BACKGROUND);
		 }
         return TRUE;

      case WID_FORM:
         // Our form widget is the title widget, replace with new widget ptr
         RELEASEIF(me->piTitle);
         me->piTitle = (IWidget*)dwParam;
         ADDREFIF(me->piTitle);
         // Fall through to popup, form
         break;
      }
   }  
         
   return Popup_HandleEvent(po, evt, wParam, dwParam);
}


void Dialog_Dtor(Dialog *me)
{
   RELEASEIF(me->piTitle);
   RELEASEIF(me->piBackground);
   Popup_Dtor(&me->base);
}

uint32 Dialog_Release(IForm *po)
{
   ME_FROM_FORM;

   if (FORM_NREFS(me) == 1) 
      Dialog_Dtor(me);

   return Form_Release(po);
}


int Dialog_Construct(Dialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                     IShell *piShell, PFNHANDLER pfnDefHandler)
{
   int result;
   IDecorator *piImage, *piTitle;

   result = Popup_Construct(&me->base, pvt, piModule, piShell, TRUE, DEFHANDLER(Dialog_HandleEvent));

   if (result == 0) {
      pvt->Release     = Dialog_Release;

      result = ISHELL_CreateInstance(piShell, AEECLSID_TITLEWIDGET, (void **)&piTitle);
   }

   if (result == 0) {
      // Keep for theming 
      me->piTitle = IDECORATOR_TO_IWIDGET(piTitle);
      
      // Set border width to zero - theme can change it if necessary
      IWIDGET_SetBorderWidth(me->piTitle, 0);
      result = ISHELL_CreateInstance(piShell, AEECLSID_IMAGEWIDGET, (void **)&piImage);
   }

   if (result == 0) {
      IWidget *piContent = 0;

      // Set some padding on the title
      IWIDGET_GetProperty(me->piTitle, PROP_TITLEWIDGET, (uint32 *)&piContent);
      IWIDGET_SetLeftPadding(piContent, 4);      
      RELEASEIF(piContent);

      // Set image as title content, don't release as it's the decorator for the popup (below)
      IDECORATOR_SetWidget(piTitle, IDECORATOR_TO_IWIDGET(piImage));

      // Set content of image to original content
      IDECORATOR_GetWidget(POPUP_DECORATOR(me), &piContent);
      IDECORATOR_SetWidget(piImage, piContent);
      RELEASEIF(piContent);

      // Replace default border widget of popup with our title widget
      RELEASEIF(FORM_WIDGET(me));
      FORM_WIDGET(me) = IDECORATOR_TO_IWIDGET(piTitle);
      ADDREFIF(FORM_WIDGET(me));
      IWIDGET_SetShadowOffset(FORM_WIDGET(me), 1); // Take on default popup shadow

      me->piBackground = IDECORATOR_TO_IWIDGET(piImage);
      IDECORATOR_AddRef(piImage);
      IWIDGET_SetBGColor(me->piBackground, RGBA_WHITE);
      IWIDGET_SetFlags(me->piBackground, IDF_ALIGN_RIGHT | IDF_ALIGN_TOP);

      // Replace the default decorator to image widget
      IDECORATOR_Release(POPUP_DECORATOR(me));
      POPUP_DECORATOR(me) = piImage;
   }

   if (result != 0)
      Dialog_Dtor(me);

   return result;
}


/////////////////////////////////////////////////////////////////
// Concrete implementation

int Dialog_New(IForm **ppo, IModule *piModule, IShell *piShell)
{
   Dialog *me = MALLOCREC_VTBL(Dialog, IForm);

   *ppo = (IForm *)me;

   if (!me) 
      return ENOMEMORY;
   
   return Dialog_Construct(me, GETVTBL(me, IForm), piModule, piShell, NULL);
}


/////////////////////////////////////////////////////////////////
// StaticDialog

#define STATIC_FROM_FORM StaticDialog *me = (StaticDialog *)po

boolean StaticDialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   STATIC_FROM_FORM;

   if (evt == EVT_WDG_GETPROPERTY) {
      
      switch(wParam) {
         case WID_STATIC:
            *(IWidget **)dwParam = me->piText;
            ADDREFIF(me->piText);
            return TRUE;
   
         case FID_PREFRECT:
            Form_SetPrefRect(po, DIALOG_PCT_WIDTH, DIALOG_PCT_HEIGHT, 50, 50, 
                             FALSE, FALSE, (AEERect*)dwParam);
            return TRUE;
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {
      
      switch (wParam)  {
         case FID_TEXT: {
            IWIDGET_SetFormText(me->piText, FORM_SHELL(me), (FormRes *)dwParam);
            return TRUE;
         }

         case FID_THEME: {
            // Because we know that we have text here, we need to apply the Dialog Content properties to the text.  
            IROOTFORM_ApplyWProps(FORM_ROOT(me), me->piText, FORM_THEME_BASENAME(me), "Dialog.Content.Properties", 0);
            break;
         }
      }
   }

   return Dialog_HandleEvent(po, evt, wParam, dwParam);
}

void StaticDialog_Dtor(StaticDialog *me)
{
   RELEASEIF(me->piText);
}

uint32 StaticDialog_Release(IForm *po)
{
   STATIC_FROM_FORM;
   
   if (FORM_NREFS(me) == 1) {
      StaticDialog_Dtor(me);
   }

   return Dialog_Release(po);
}

int StaticDialog_Construct(StaticDialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                           IShell *piShell, PFNHANDLER pfnDefHandler)
{
   int result;
   IImage *piImage = 0;
   IDecorator *piDecorator = 0;

   result = Dialog_Construct(&me->base, pvt, piModule, piShell, DEFHANDLER(StaticDialog_HandleEvent));

   if (result == 0) {
      pvt->Release     = StaticDialog_Release;

      result = ISHELL_CreateInstance(piShell, AEECLSID_STATICWIDGET,(void **) &me->piText);
   }
   
   if (result == 0) {
      IWIDGET_SetFlags(me->piText, SWF_WRAPTEXT | IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE | SWF_NOSHORTENTEXT);
      IWIDGET_SetLeftPadding(me->piText, 4);
      IWIDGET_SetRightPadding(me->piText, 4);
      IWIDGET_SetFGColor(me->piText, RGBA_BLACK);
      IWIDGET_SetBGColor(me->piText, RGBA_NONE);
      IWIDGET_SetBorderWidth(me->piText, 0);

      result = IFORM_GetDecoratorWidget(STATICDIALOG_TO_IFORM(me), &piDecorator);
   }

   if (result == 0)
      IDECORATOR_SetWidget(piDecorator, me->piText);

   RELEASEIF(piDecorator);
   RELEASEIF(piImage);

   return result;
}

int StaticDialog_New(IForm **ppo, IModule *piModule, IShell *piShell)
{
   StaticDialog *me = MALLOCREC_VTBL(StaticDialog, IForm);

   *ppo = (IForm *)me;

   if (!me) 
      return ENOMEMORY;

   return StaticDialog_Construct(me, GETVTBL(me, IForm), piModule, piShell, NULL);
}


/////////////////////////////////////////////////////////////////
// Utility dialogs

int ResDialog_New(IForm **ppo, IModule *piModule, IShell *piShell, char *baseName)
{
   int result;
   result = StaticDialog_New(ppo, piModule, piShell);
   if ((result == SUCCESS) && (NULL != baseName)) {
      IFORM_SetThemeBaseName(*ppo, baseName);
   }
   return result;
}


/////////////////////////////////////////////////////////////////
// Progress

#define PROGRESS_FROM_FORM ProgressDialog *me = (ProgressDialog *)po

boolean ProgressDialog_HandleEvent(IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   PROGRESS_FROM_FORM;
   IValueModel *piModel;

   if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case WID_PROGRESS:
         *(IWidget **)dwParam = me->piProgress;
         ADDREFIF(me->piProgress);
         return TRUE;
      }
   } else if (evt == EVT_WDG_SETPROPERTY) {
      switch(wParam) {
      case FID_PROGRESS:
         if (IWIDGET_GetModel(me->piProgress, AEEIID_VALUEMODEL, (IModel **)&piModel) == 0) {
            IVALUEMODEL_SetValue(piModel, (void *)dwParam, 0, 0);
            IVALUEMODEL_Release(piModel);
         }         
         return TRUE;
      case FID_THEME:
         {
           if (me->piProgress)
           {
               IROOTFORM_ApplyWProps(FORM_ROOT(me), me->piProgress, FORM_THEME_BASENAME(me), "Dialog.ProgressBar.Properties", 0);
           }

           // Fall through so dialog can take care of the rest
           break;
         }

      }
   }

   return StaticDialog_HandleEvent(po, evt, wParam, dwParam);
}

void ProgressDialog_Dtor(ProgressDialog *me)
{
   RELEASEIF(me->piProgress);
   StaticDialog_Dtor(&me->base);
}

uint32 ProgressDialog_Release(IForm *po)
{
   PROGRESS_FROM_FORM;

   if (FORM_NREFS(me) == 1) {
      ProgressDialog_Dtor(me);
   }

   return StaticDialog_Release(po);
}


int ProgressDialog_Construct(ProgressDialog *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                             IShell *piShell, PFNHANDLER pfnDefHandler)
{
   int result;
   IPropContainer *piContainer = 0;
   IDecorator *piDecorator = 0;
   IDecorator *pidBorder = 0;
   IWidget *piText = 0;

   result = StaticDialog_Construct(&me->base, pvt, piModule, piShell, DEFHANDLER(ProgressDialog_HandleEvent));
   
   if (result == 0) {
      pvt->Release      = ProgressDialog_Release;

      IFORM_SetThemeBaseName((IForm*)me, "System.ProgressDialog");

      result = IFORM_GetDecoratorWidget(PROGRESSDIALOG_TO_IFORM(me), &piDecorator);
   }
   
   if (result == 0) {
      IDECORATOR_GetWidget(piDecorator, &piText);

      result = ISHELL_CreateInstance(piShell, AEECLSID_PROPCONTAINER, (void **)&piContainer);
   }

   if (result == 0) {
      result = ISHELL_CreateInstance(piShell, AEECLSID_PROGRESSWIDGET, (void **)&me->piProgress);
   }
   
   if (result == 0) {
      result = ISHELL_CreateInstance(piShell, AEECLSID_BORDERWIDGET, (void **)&pidBorder);
   }

   if (result == 0) {
      WExtent extent;
      IWIDGET_GetPreferredExtent(me->piProgress, &extent);
      IWIDGET_SetExtent(me->piProgress, &extent);

      IWIDGET_SetBorderWidth((IWidget*)pidBorder, 0);
      IWIDGET_SetBGColor((IWidget*)pidBorder, RGBA_NONE);

      IWIDGET_SetTopPadding((IWidget*)pidBorder, 4);
      IWIDGET_SetBottomPadding((IWidget*)pidBorder, 4);
      IWIDGET_SetLeftPadding((IWidget*)pidBorder, 2);
      IWIDGET_SetRightPadding((IWidget*)pidBorder, 2);

      IDECORATOR_SetWidget(pidBorder, me->piProgress);

   }

   if (result == 0) {
      IWidget *piWidget = 0;
      WidgetProp prop;

      prop.bVisible = TRUE;
      prop.prop = 0;
      IPROPCONTAINER_Insert(piContainer, (IWidget*)pidBorder, WIDGET_ZNORMAL, &prop);

      prop.prop = 1;
      IPROPCONTAINER_Insert(piContainer, piText, WIDGET_ZNORMAL, &prop);

      IPROPCONTAINER_QueryInterface(piContainer, AEEIID_WIDGET, (void **)&piWidget);
      IDECORATOR_SetWidget(piDecorator, piWidget);

      RELEASEIF(piWidget);
   }
   
   RELEASEIF(piText);
   RELEASEIF(piDecorator);
   RELEASEIF(piContainer);
   RELEASEIF(pidBorder);
   
   return result;
}


int ProgressDialog_New(IForm **ppo, IModule *piModule, IShell *piShell)
{
   typedef struct ProgressDialogImpl {
      ProgressDialog base;
      AEEVTBL(IForm) vtForm;
   } ProgressDialogImpl;
   
   ProgressDialogImpl *me = MALLOCREC(ProgressDialogImpl);

   *ppo = (IForm *)me;

   if (!me)
      return ENOMEMORY;

   return ProgressDialog_Construct(&me->base, &me->vtForm, piModule, piShell, NULL);
}
