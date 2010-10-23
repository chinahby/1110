/*
  ========================================================================

  FILE:  RootForm.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Concrete implementation of RootForm methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#include "Form.h"

#include "AEESoftkeyWidget.h"
#include "AEEImageWidget.h"
#include "AEEStaticWidget.h"
#include "AEEImageStaticWidget.h"
#include "AEERootContainer.h"
#include "AEEWProperties.h"
#include "AEEVectorModel.h"

#include "AEEWeb.h"

#include "AEERootForm.h"
#include "AEEResFile.h"

#include "FormUtil.h"
#include "AEEDisplayCanvas.h"

#define FORMSTACK_MIN  10
#define FORMSTACK_GROW 2

/////////////////////////////////////////////////////////////////
// RootForm

typedef struct RootForm {
   Form              base;

   IRootContainer *  piContainer;
   AEERect           rcContainer;
   AEERect           rcClient;

   IVectorModel *    piForms;
   ModelListener     mlFormActive;
   ModelListener     mlFormTopmostNonPopup;

   IWidget *         piTitle;
   ImageStaticInfo   titleInfo;
   IWidget *         piSoftkeys;
   IWidget *         piBackground;

   IWidget *         piActiveWidget;  

   IResFile *        piThemeFile;
   const char *      themeFile;

   boolean           bStackChangePending;
} RootForm;

// Forward Declns
static void RootForm_StackChange(IRootForm *po);

#define DECL(c) c* me = (c *)po

static __inline IForm *ROOTFORM_TO_IFORM(RootForm *me) {
   return (IForm *)me;
}

static __inline Form *ROOTFORM_TO_FORM(RootForm *me) {
   return (Form *)me;
}

static __inline IRootForm *ROOTFORM_TO_IROOTFORM(RootForm *me) {
   return (IRootForm *)me;
}

static void RootForm_FreeFormEntry(IForm *po)
{
   IFORM_Release(po);
}

static void RootForm_UpdateClientArea(RootForm *me)
{
   WidgetPos pos;
   WExtent titleExtent, skExtent;
   AEERect rcClientOld = me->rcClient;

   if (me->piSoftkeys) {
      IWIDGET_GetExtent(me->piSoftkeys, &skExtent);
 
      // Adjust softkey position based on current height
      IROOTCONTAINER_GetPos(me->piContainer, me->piSoftkeys, &pos);
      pos.y = me->rcContainer.dy - skExtent.height;
      IROOTCONTAINER_SetPos(me->piContainer, me->piSoftkeys, WIDGET_ZNORMAL, &pos);
   } else {
      SETWEXTENT(&skExtent, 0, 0);
   }

   if (me->piTitle) {
      IWIDGET_GetExtent(me->piTitle, &titleExtent);
   } else {
      SETWEXTENT(&titleExtent, 0, 0);
   }
   
   // Calculate client area
   SETAEERECT(&me->rcClient, 0, titleExtent.height,
              me->rcContainer.dx,
              me->rcContainer.dy - skExtent.height - titleExtent.height);

   // Notify listeners of client area change if necessary
   if (MEMCMP(&rcClientOld, &me->rcClient, sizeof(AEERect))) {
      Form_Notify(ROOTFORM_TO_FORM(me), FORMITEM_CLIENT);   
   }
}


static void RootForm_UpdateTheme(RootForm *me, const char *baseName)
{
   WExtent wextent;

   BUIT_LOG("FORMS EVT: Update Theme Started for %s", baseName);

   if (!me->piThemeFile)
      return;

   if (me->piTitle) {
      IWIDGET_SetProperties(me->piTitle, me->piThemeFile, baseName, "Title", "Properties", 0);
      IWIDGET_GetPreferredExtent(me->piTitle, &wextent);
      wextent.width = me->rcContainer.dx;
      IWIDGET_SetExtent(me->piTitle, &wextent);
   }

   if (me->piSoftkeys) {
      IWIDGET_SetProperties(me->piSoftkeys, me->piThemeFile, baseName, "Softkeys", "Properties", 0);
      IWIDGET_GetPreferredExtent(me->piSoftkeys, &wextent);
      wextent.width = me->rcContainer.dx;
      IWIDGET_SetExtent(me->piSoftkeys, &wextent);
   }

   if (me->piBackground) {
      IWIDGET_SetProperties(me->piBackground, me->piThemeFile, baseName, "Background", "Properties", 0);
   }

   // Update client area since sizes may have changed
   RootForm_UpdateClientArea(me);

   BUIT_LOG("FORMS EVT: Update Theme Finished for %s", baseName);
}

// updates the rootform with the background image, softkey and 
// title text of the TOS (top of stack) form.
static void RootForm_Update(RootForm *me, uint32 dwItemMask, IForm* piForm)
{
   boolean bPopup = 0;

   // get form's popup flag
   bPopup = IFORM_GetIsPopup(piForm);

   // if the form's widget has changed, update the scroll model
   // for the scroll indicator in the softkey widget
   if (dwItemMask & FORMITEM_WIDGET) {
      
      IWidget *piWidget = NULL;
      // get form's widget
      IFORM_GetWidget(piForm, WID_FORM, &piWidget);

      // update the widget and the scroll model
      if (piWidget) {

         // if the active widget has been changed underneath us...
         
         if (me->piActiveWidget && piWidget != me->piActiveWidget) {
            // this block will only be executed when the form widget is changed
            // by the application logic while the form is active
            WidgetPos pos;
            WExtent we;
   
            IWIDGET_MoveFocus(FORM_WIDGET(me), (IWidget*)WIDGET_FOCUS_NONE);
   
            IWIDGET_GetExtent(me->piActiveWidget, &we);
            IWIDGET_SetExtent(piWidget, &we);
   
            // remove the previously active widget from the root container
            if (SUCCESS == IROOTCONTAINER_GetPos(me->piContainer, me->piActiveWidget, &pos)) {
               IROOTCONTAINER_Remove(me->piContainer, me->piActiveWidget);
            }
            
            // add the new widget to the root container
            IROOTCONTAINER_Insert(me->piContainer, piWidget, WIDGET_ZTOPMOST, &pos);
            // and remember it fondly
            RELEASEIF(me->piActiveWidget);
            me->piActiveWidget = piWidget;
            ADDREFIF(piWidget);

            // set focus to the new widget
            IWIDGET_MoveFocus(FORM_WIDGET(me), piWidget);
         
         } else if (!me->piActiveWidget) {
            me->piActiveWidget = piWidget;
            ADDREFIF(piWidget);
         }

      }

      RELEASEIF(piWidget);
   }


   // if the form's background image has changed...
   // if form is a popup, then retain the background image 
   // from the previous form
   if (dwItemMask & FORMITEM_BACKGROUND && me->piBackground && !bPopup) {      
      IImage *pii = NULL;
      
      // Try to grab the image from the new form.  
      IFORM_GetBGImage(piForm, &pii);

      // If non-existent, try defaulting to the root form
      if (!pii) IFORM_GetBGImage(ROOTFORM_TO_IFORM(me), &pii);
      
      // Apply the result (NULL or otherwise) to our background widget
      IWIDGET_SetImage(me->piBackground, pii);
      RELEASEIF(pii);
   }
   
   // if the form's title text has changed...  retain previous title
   // if we are a popup 

   if ((dwItemMask & FORMITEM_TITLE) && me->piTitle && !bPopup) {
      // Release image. Text is owned by form
      RELEASEIF(me->titleInfo.piImage);
      IFORM_GetTextPtr(piForm, FID_TITLE, &me->titleInfo.pwText);
      IFORM_GetTitleImage(piForm, &me->titleInfo.piImage);

      // Set title info
      IWIDGET_SetImageStaticInfo(me->piTitle, &me->titleInfo, 0);
   }

   // if the form's softkey text has changed...
   if ((dwItemMask & FORMITEM_SOFTKEY) && me->piSoftkeys) {

      IForm* piTopForm = IROOTFORM_GetTopForm(ROOTFORM_TO_IROOTFORM(me));

      AECHAR *pwsz = NULL;
      IWidget *piKey = NULL;

      if (piTopForm == piForm) {
         // set softkey 1 text
         IFORM_GetTextPtr(piForm, FID_SOFTKEY1, &pwsz);
         if (SUCCESS == IWIDGET_GetSoftkey(me->piSoftkeys, PROP_SOFTKEY1, &piKey)) {
            IWIDGET_SetText(piKey, pwsz, 0);
         }
         RELEASEIF(piKey);
   
         // set softkey 2 text
         IFORM_GetTextPtr(piForm, FID_SOFTKEY2, &pwsz);
         if (SUCCESS == IWIDGET_GetSoftkey(me->piSoftkeys, PROP_SOFTKEY2, &piKey)) {
            IWIDGET_SetText(piKey, pwsz, 0);
         }
      }
      RELEASEIF(piKey);
   }

   if ((dwItemMask & FORMITEM_THEME_BASENAME)) {
      char *baseName = 0;

      IFORM_GetThemeBaseName(piForm, &baseName);
      RootForm_UpdateTheme(me, baseName);
   }

}

// Move the specified widget to just under pifBefore's primary widget
static int RootForm_MoveWidget(RootForm *me, IWidget *piwFind, IForm *pifBefore)
{
   WidgetPos pos;   
   int nErr = SUCCESS;
   IWidget* piwBefore = NULL;

   // Retrieve pifBefore's widget for our move location
   if (!nErr) {
      nErr = IFORM_GetWidget(pifBefore, WID_FORM, &piwBefore);
   }

   // Retrieve current pos info 
   if (!nErr) {      
      nErr = IROOTCONTAINER_GetPos(me->piContainer, piwFind, &pos);
   }

   // Set new location
   if (!nErr) {
      nErr = IROOTCONTAINER_SetPos(me->piContainer, piwFind, piwBefore, &pos);
   }

   RELEASEIF(piwBefore);
   return nErr;
}

// Replace the specified widget with new widget and update rootform as necessary
// *piw will be replaced and set to piwNew / ADDREF'd if successful
static int RootForm_ReplaceWidget(RootForm *me, IWidget **piw, IWidget *piwNew, IWidget *piwBefore)
{
   int nErr = SUCCESS;
   WidgetPos  pos;

   // Early check for identical replacements
   if (*piw != piwNew) {

      // Get pos of piw
      if (*piw) {
         nErr = IROOTCONTAINER_GetPos(me->piContainer, *piw, &pos);      
      }

      // Insert the new widget (do this before removal in case piwBefore == *piw)
      if (!nErr && piwNew) {
         nErr = IROOTCONTAINER_Insert(me->piContainer, piwNew, piwBefore, &pos);
      }

      // Now, remove old widget
      if (!nErr && *piw) {
         nErr = IROOTCONTAINER_Remove(me->piContainer, *piw);
         IWIDGET_Release(*piw);
      }

      // Addref and return the new widget via piw
      if (nErr) {
         piwNew = NULL;      
      } 
      
      *piw = piwNew;
      ADDREFIF(*piw);   
   }

   // Do an update since extents may have changed
   RootForm_UpdateClientArea(me);

   return nErr;
}

static int RootForm_SetThemeName(RootForm *me, const char *themeFile)
{
   if (!me->piThemeFile)
      return EBADSTATE;
   
   FREEIF(me->themeFile);
   me->themeFile = STRDUP(themeFile);
   
   IRESFILE_Close(me->piThemeFile);
   if (themeFile)
      return IRESFILE_Open(me->piThemeFile, themeFile);
   else
      return SUCCESS;
}

static int RootForm_SetDisplay(RootForm *me, IDisplay *piDisplay)
{
   int nErr = SUCCESS;
   IDisplayCanvas *piCanvas = 0;

   nErr = ISHELL_CreateInstance(FORM_SHELL(me), AEECLSID_DISPLAYCANVAS, (void **)&piCanvas);
      
   if (!nErr) {
      WExtent extent;
      WidgetPos pos;
      

      IDISPLAY_SetClipRect(piDisplay, NULL); // reset the clipping rectangle
      IDISPLAY_GetClipRect(piDisplay, &me->rcContainer);
      SETAEERECT(&me->rcClient, 0, 0, me->rcContainer.dx, me->rcContainer.dy);

      IDISPLAYCANVAS_SetDisplay(piCanvas, piDisplay);
      IROOTCONTAINER_SetCanvas(me->piContainer, (ICanvas *)piCanvas, &me->rcContainer);

      if (me->piTitle) {
         // Set extent, title is already positioned at 0, 0
         IWIDGET_GetExtent(me->piTitle, &extent);
         extent.width = me->rcContainer.dx;
         IWIDGET_SetExtent(me->piTitle, &extent);
      }

      if (me->piBackground) {
         // Set extent, background is already positioned at 0, 0
         extent.width = me->rcContainer.dx;
         extent.height = me->rcContainer.dy;
         IWIDGET_SetExtent(me->piBackground, &extent);
      }

      if (me->piSoftkeys) {
         // Set extent
         IWIDGET_GetExtent(me->piSoftkeys, &extent);
         extent.width = me->rcContainer.dx;
         IWIDGET_SetExtent(me->piSoftkeys, &extent);
         // And position at bottom of screen
         IROOTCONTAINER_GetPos(me->piContainer, me->piSoftkeys, &pos);
         pos.y = me->rcContainer.dy - extent.height;
         IROOTCONTAINER_SetPos(me->piContainer, WIDGET_ZNORMAL, me->piSoftkeys, &pos);
      }
   }

   RELEASEIF(piCanvas);

   return nErr;
}


static void RootForm_ApplyTheme(RootForm *me)
{
   int nrForms, i;

   if (!me->piThemeFile) 
      return;
   
   nrForms = IVECTORMODEL_Size(me->piForms);
   for (i = 0; i < nrForms; i++) {
      IForm *piForm;
      char* pTheme = 0;
      IVECTORMODEL_GetAt(me->piForms, i, (void **)&piForm);
      
      IFORM_GetThemeBaseName(ROOTFORM_TO_IFORM(me), &pTheme);
      pTheme = (pTheme) ? pTheme : "(None)";
      
      BUIT_LOG("FORMS EVT: Apply Theme Started for %s", pTheme);
      
      IFORM_ApplyTheme(piForm);
      
      BUIT_LOG("FORMS EVT: Apply Theme Finished for %s", pTheme);
   }

   if (nrForms == 0) {
      char *baseName = 0;
      
      IFORM_GetThemeBaseName(ROOTFORM_TO_IFORM(me), &baseName);
      RootForm_UpdateTheme(me, baseName);
   }
}

boolean RootForm_HandleEvent(IRootForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECL(RootForm);

   if (FORM_WIDGET(me)
      && IWIDGET_HandleEvent(FORM_WIDGET(me), evt, wParam, dwParam))
      return TRUE;

   if (evt == EVT_WDG_GETPROPERTY) {
      switch(wParam) {
      case FID_THEME_FNAME:
         *(const char **)dwParam = me->themeFile;
         return TRUE;

      case FID_THEME_FILE:
         *(IResFile **)dwParam = me->piThemeFile;
         ADDREFIF(me->piThemeFile);
         return TRUE;

      case WID_TITLE:
         *(IWidget **)dwParam = me->piTitle;
         ADDREFIF(me->piTitle);
         return TRUE;

      case WID_SOFTKEYS:
         *(IWidget **)dwParam = me->piSoftkeys;
         ADDREFIF(me->piSoftkeys);
         return TRUE;

      case WID_BACKGROUND:
         *(IWidget **)dwParam = me->piBackground;
         ADDREFIF(me->piBackground);
         return TRUE;

      case WID_FORM:
         IROOTCONTAINER_QueryInterface(me->piContainer, AEEIID_WIDGET, (void **)dwParam);
         return TRUE;

      case WID_CONTAINER:
         *(IContainer **)dwParam = IROOTCONTAINER_TO_ICONTAINER(me->piContainer);
         ADDREFIF(me->piContainer);
         return TRUE;

      default:
         // Fall back on form
         return Form_HandleEvent(ROOTFORM_TO_IFORM(me), evt, wParam, dwParam);
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {
      IForm *piForm = 0;

      switch(wParam) {
      case FID_ACTIVE: {
         // Set our rootform state accordingly
         FORM_ACTIVE(&me->base) = (boolean)dwParam;
         
         if ((me->bStackChangePending) && FORM_ACTIVE(&me->base)) {
            // If we were activated and there is a stack change pending, execute it now
            RootForm_StackChange(po);
         } else {
            // If we are just being activated or deactivated (no stack change) do minimal update
            piForm = IROOTFORM_GetTopForm(po);
            // Activate or de-activate the top form, invalidate if necessary
            if (piForm) {               
               IFORM_SetProperty(piForm, FID_ACTIVE, dwParam);
            }
            if (FORM_ACTIVE(&me->base)) {
               IROOTCONTAINER_Invalidate(me->piContainer, 0, 0, 0);
            }
         }
         return TRUE;
      }         
      case FID_THEME:
         RootForm_ApplyTheme(me);
         return TRUE;

      case FID_THEME_FNAME:
         if (SUCCESS == RootForm_SetThemeName(me, (const char *)dwParam)) {
            RootForm_ApplyTheme(me);
            return TRUE;
         }
         return FALSE;

      case FID_BACKGROUND:
         // If we have a background widget, set the image into it
         if (me->piBackground) {
            IWIDGET_SetFormImage(me->piBackground, FORM_SHELL(me), (FormRes *)dwParam);
         }
         // Also load the image into our internal form, which will hold it as a default for other forms
         return Form_HandleEvent(ROOTFORM_TO_IFORM(me), evt, wParam, dwParam);

      case FID_DISPLAY:
         return SUCCESS == RootForm_SetDisplay(me, (IDisplay *)dwParam);

      case FID_WPROPS: {
         WPropDesc *pdesc = (WPropDesc *)dwParam;
         WResPropDesc wd;
                  
         wd.piResFile = me->piThemeFile;
         if (pdesc) {
            wd.args = pdesc->args;
            wd.piWidget = pdesc->piWidget;
         }
         return IWIDGET_SetProperty(pdesc->piWidget, PROP_APPLYWPROPS, (uint32)&wd);
      }

      case WID_TITLE:
         return (SUCCESS == RootForm_ReplaceWidget(me, &me->piTitle, (IWidget *)dwParam, me->piTitle));

      case WID_SOFTKEYS:
         return (SUCCESS == RootForm_ReplaceWidget(me, &me->piSoftkeys, (IWidget *)dwParam, me->piSoftkeys));

      case WID_BACKGROUND:
         return (SUCCESS == RootForm_ReplaceWidget(me, &me->piBackground, (IWidget *)dwParam, WIDGET_ZBOTTOMMOST));

      default:
         // Fall back on form
         return Form_HandleEvent(ROOTFORM_TO_IFORM(me), evt, wParam, dwParam);
      }
   }

   // Non get/set property events are sent on to the topmost form
   {
      IForm *piForm = IROOTFORM_GetTopForm(po);
      if (!piForm)
         return FALSE;
      else
         return IFORM_HandleEvent(piForm, evt, wParam, dwParam);
   }  
}


static void RootForm_UpdateActiveListenerCB(RootForm *me, FormEvent *pEvent)
{
   if (pEvent->base.evCode == EVT_MDL_FORM_CHANGE) {
      RootForm_Update(me, pEvent->dwItemMask, pEvent->piForm);
   }
}

static void RootForm_UpdateTopmostNonPopupListenerCB(RootForm *me, FormEvent *pEvent)
{
   uint32 dwItemMask = pEvent->dwItemMask & (FORMITEM_BACKGROUND | FORMITEM_TITLE | FORMITEM_SOFTKEY);

   if (pEvent->base.evCode == EVT_MDL_FORM_CHANGE && dwItemMask) {
      RootForm_Update(me, dwItemMask, pEvent->piForm);
   }
}

static void RootForm_ShowFormWidget(IRootForm *po, IForm *piForm, boolean bShow, boolean bFocus)
{
   DECL(RootForm);
   WidgetPos pos;
   IWidget *piWidget;
   boolean bVisible = FALSE;

   if (!piForm)
      return;

   IFORM_GetWidget(piForm, WID_FORM, &piWidget);
   
   if (!piWidget)
      return;

   // Set visibility on form (if different), then set widget pos
   if ((SUCCESS != IFORM_IsVisible(piForm, &bVisible)) || (bVisible != bShow)) {
         IFORM_SetProperty(piForm, FID_VISIBLE, bShow);
   }
   
   IROOTCONTAINER_GetPos(me->piContainer, piWidget, &pos);
   pos.bVisible = bShow;
   IROOTCONTAINER_SetPos(me->piContainer, piWidget, WIDGET_ZNORMAL, &pos);

   // and set focus to the widget
   if (bShow && bFocus) {
      IWIDGET_MoveFocus(FORM_WIDGET(me), piWidget);
   } else {
      IWIDGET_MoveFocus(FORM_WIDGET(me), WIDGET_FOCUS_NONE);
   }
   
   IWIDGET_Release(piWidget);
}


/** Activates a given form.  Previous form should have been
 deactivated before this is called with bActivate set to FALSE
 */
static void RootForm_ActivateForm(IRootForm *po, IForm *piForm, boolean bActivate)
{
   DECL(RootForm);

   if (!piForm)
      return;

   if (!bActivate) {
      if (IFORM_IsActive(piForm)) {
         IFORM_Deactivate(piForm);
         RELEASEIF(me->piActiveWidget);
      }      
   } else {
      RELEASEIF(me->piActiveWidget);         
      IFORM_GetWidget(piForm, WID_FORM, &me->piActiveWidget);
      // Then go update all the items except the forms widget as this is not the 
      // form updating its own widget. Need to update first since theme information
      // affect client area which affects form activation
      RootForm_Update(me, FORMITEM_ALL & ~FORMITEM_WIDGET, piForm);

      // Activate the form, if necessary
      if (!IFORM_IsActive(piForm)) {       
         IFORM_Activate(piForm);
      }      
   }
}

static int RootForm_GetFormIndex(RootForm *me, IForm **ppiForm) 
{
   IForm *piForm;
   int nrForms;

   nrForms = IVECTORMODEL_Size(me->piForms);

   if (nrForms > 0) {

      if (*ppiForm == FORM_LAST || *ppiForm == FORM_DEFAULT) {

         IVECTORMODEL_GetAt(me->piForms, nrForms - 1, (void **)ppiForm);
         return nrForms - 1;

      } else if (*ppiForm == FORM_FIRST) {

         IVECTORMODEL_GetAt(me->piForms, 0, (void **)ppiForm);
         return 0;

      } else {

         int i;
         for (i = 0; i < nrForms; i++) {
            IVECTORMODEL_GetAt(me->piForms, i, (void **)&piForm);
            if (piForm == *ppiForm)
               return i;
         }

      }
   }

   return -1;
}

static __inline int RootForm_GetFormInsertionIndex(RootForm *me, IForm **ppiForm)
{
   int delta;

   if (*ppiForm == FORM_FIRST)
      return 0;

   if (*ppiForm == FORM_LAST || *ppiForm == FORM_DEFAULT) {
      delta = 1;
   } else {
      delta = 0;
   }

   return RootForm_GetFormIndex(me, ppiForm) + delta;
}

static void RootForm_StackChange(IRootForm *po)
{
   DECL(RootForm);
   IForm* piTopForm = IROOTFORM_GetTopForm(po);
   
   // We need to cancel listeners regardless of activation state
   LISTENER_Cancel(&me->mlFormActive);
   LISTENER_Cancel(&me->mlFormTopmostNonPopup);

   // If we are currently deactivated, set the flag and return
   if (!FORM_ACTIVE(&me->base)) {
      me->bStackChangePending = TRUE;
      return;
   }   
   me->bStackChangePending = FALSE;   

   // If there are still forms on the stack, then we need to set up several things:
   //   1. The topmost form is the active form
   //   2. All other forms are not active
   //   3. The topmost form is being listened to via mlFormActive
   //   4. The topmost non-popup form is being listened to via mlFormTopmostNonPopup
   //   5. The topmost non-popup form and all popup forms on top of it are shown
   //   6. Forms below the topmost non-popup form are not shown
   //   7. The softkeys should be just below the topmost form's widget
   //   8. The title should be just below the topmost non-popup form's widget
   if (piTopForm) {
      boolean bFoundTopmostNonPopup = FALSE;
      IModel* piModel = NULL;
      IForm*  pif;

      // Logging stack change begin
      BUIT_LOG("FORMS EVT: Stack Change Starting...", 1);

      // Need to deal with the non-active forms first, then the active form
      for (pif = piTopForm; pif; pif = IROOTFORM_GetForm(po, pif, FALSE, FALSE)) {
         boolean bPopup = IFORM_GetIsPopup(pif);
        
         IFORM_GetFormModel(pif, &piModel);
         if (piModel) {
            if (pif != piTopForm) {                              
               RootForm_ShowFormWidget(po, pif, (boolean)(bFoundTopmostNonPopup? FALSE : TRUE), FALSE);
               if (IFORM_IsActive(pif)) {
                  RootForm_ActivateForm(po, pif, FALSE);
               }
            }

            if (!bPopup && !bFoundTopmostNonPopup) {
               bFoundTopmostNonPopup = TRUE;
               if (me->piTitle) {
                  RootForm_MoveWidget(me, me->piTitle, pif);
               }
               IMODEL_AddListenerEx(piModel, &me->mlFormTopmostNonPopup, (PFNLISTENER)RootForm_UpdateTopmostNonPopupListenerCB, me);
               if (pif != piTopForm) {
                  // Only update if not the topmost form since the
                  // Activate below applies theme again The topmost
                  // non-popup (but not the top!) influences the
                  // background, title ans associated themes
                  RootForm_Update(me, FORMITEM_BACKGROUND | FORMITEM_TITLE | FORMITEM_THEME_BASENAME, pif);
               }
            }
            RELEASEIF(piModel);
         }        
      }

      if (me->piSoftkeys) {
         RootForm_MoveWidget(me, me->piSoftkeys, piTopForm);
      }
      RootForm_ActivateForm(po, piTopForm, TRUE);
      RootForm_ShowFormWidget(po, piTopForm, TRUE, TRUE);
      IFORM_GetFormModel(piTopForm, &piModel);
      if (piModel)
         IMODEL_AddListenerEx(piModel, &me->mlFormActive, (PFNLISTENER)RootForm_UpdateActiveListenerCB, me);
      RELEASEIF(piModel);
      
      // Log that the form is about to be activated - all theme stuff has happened by now)
      BUIT_LOG("FORMS EVT: Stack Change Finished", 1);

   }

    // Notify change in stack
   Form_Notify(ROOTFORM_TO_FORM(me), FORMITEM_STACK);
}


int RootForm_InsertForm(IRootForm *po, IForm *piForm, IForm *pifBefore)
{
   DECL(RootForm);
   IWidget *piWidget = 0;
   IWidget *piwBefore = 0;
   IForm *pifCurrent;
   int nrForms, formIndex, nErr;

   if (!piForm)
      return EBADPARM;

   // Make sure we can insert, get the index we want to insert at
   formIndex = RootForm_GetFormInsertionIndex(me, &pifBefore);

   if (formIndex < 0)
      return EBADPARM;

   nrForms = IVECTORMODEL_Size(me->piForms);
   pifCurrent = IROOTFORM_GetTopForm(po);

   // Get widget to insert
   IFORM_GetWidget(piForm, WID_FORM, &piWidget);

   // Get widget insertion point. 
   if (formIndex == nrForms || !nrForms) {
      piwBefore = WIDGET_ZTOPMOST;
   } else if (pifBefore == FORM_FIRST) {
      if (me->piBackground != NULL) {
     
         // If we have a background widget, try to insert the form's widget
         // above the background widget
         piwBefore = IROOTCONTAINER_GetWidget(me->piContainer, me->piBackground, TRUE, FALSE);
         if (piwBefore) {
            // Add a reference, so it can be released below.
            IWIDGET_AddRef(piwBefore);
         }
      } 

      if (!piwBefore) {
         // No background widget, insert the form's widget at the bottom.
         piwBefore = WIDGET_ZBOTTOMMOST;
      }

   } else {
      // If we have a pifBefore with a backdrop, we need to take it into account for insertion
      if ((SUCCESS != IFORM_GetWidget(pifBefore, WID_BACKDROP, &piwBefore)) || !piwBefore) {
         IFORM_GetWidget(pifBefore, WID_FORM, &piwBefore);
      }
   }

   // Make sure we have space for the new form
   nErr = IVECTORMODEL_EnsureCapacity(me->piForms, MAX(FORMSTACK_MIN, nrForms + 1), FORMSTACK_GROW);

   // Now insert
   if (!nErr && piWidget && piwBefore) {
      WidgetPos pos;

      // Not really needed here since Activate does this to, but since
      // we need to give a position on insert we may as well do it
      // right
      pos.x = me->rcClient.x; 
      pos.y = me->rcClient.y;
      pos.bVisible = (piwBefore == WIDGET_ZTOPMOST);
      
      // Insert widget into widget stack
      nErr = IROOTCONTAINER_Insert(me->piContainer, piWidget, piwBefore, &pos);
   }

   if (!nErr) {
      char* pTheme = 0;

      // Add form to formstack
      IVECTORMODEL_InsertAt(me->piForms, formIndex, piForm);
      IFORM_AddRef(piForm);

      // Set rootform
      IFORM_SetProperty(piForm, FID_ROOT, (uint32)po);

      // Log info
      IFORM_GetThemeBaseName(ROOTFORM_TO_IFORM(me), &pTheme);
      pTheme = (pTheme) ? pTheme : "(None)";

      BUIT_LOG("FORMS EVT: Insert Set Theme Started for %s", pTheme);

      // Set theme on new form
      IFORM_ApplyTheme(piForm);

      BUIT_LOG("FORMS EVT: Insert Set Theme Finished for %s", pTheme);
      //RootForm_Update(me, FORMITEM_THEME, piForm);

      RootForm_StackChange(po);
   }

   RELEASEIF(piWidget);
   if (piwBefore != WIDGET_ZTOPMOST && piwBefore != WIDGET_ZBOTTOMMOST)
      RELEASEIF(piwBefore);
   return nErr;
}

int RootForm_RemoveForm(IRootForm *po, IForm *piForm)
{
   DECL(RootForm);
   IWidget *piWidget = 0;
   IForm *piF = 0;
   int nrForms = 0;
   int formIndex;
   boolean bOnlyPopups = 1;

   if (me->piForms)
      nrForms = IVECTORMODEL_Size(me->piForms);

   if (piForm == FORM_ALL) {
      while (nrForms > 0) {
         IROOTFORM_RemoveForm(po, FORM_LAST);
         nrForms = IVECTORMODEL_Size(me->piForms);
      }

   } else {
      formIndex = RootForm_GetFormIndex(me, &piForm);
      
      if (formIndex < 0)
         return EBADPARM;
      
      IFORM_GetWidget(piForm, WID_FORM, &piWidget);
      
      if (piWidget) {
         IROOTCONTAINER_Remove(me->piContainer, piWidget);
      }
      
      // Hide form widget
      RootForm_ShowFormWidget(po, piForm, FALSE, FALSE);
      // Deactivate form
      RootForm_ActivateForm(po, piForm, FALSE);
      // Tell it of rootform departure
      IFORM_SetProperty(piForm, FID_ROOT, 0);
      // Delete it from the stack
      IVECTORMODEL_DeleteAt(me->piForms, formIndex);

      RootForm_StackChange(po);

      RELEASEIF(piWidget);

      // Now many forms do we now have?
      nrForms = IVECTORMODEL_Size(me->piForms);
   }

   // Cycle through remaining forms to determine type
   for (piF = IROOTFORM_GetTopForm(po); piF && bOnlyPopups; piF = IROOTFORM_GetForm(po, piF, FALSE, FALSE))
   {
      bOnlyPopups &= IFORM_GetIsPopup(piF);
   }  


   if ((0 == nrForms) || bOnlyPopups)
   {
      // If we don't have any more forms, or the only forms we do have are popups, 
      // ensure the title has been cleaned (the title memory is owned by the last full screen form, 
      // which may no longer exist).
      if (me->piTitle) {
         // Release image. Text is owned by form
         RELEASEIF(me->titleInfo.piImage);
         me->titleInfo.pwText = NULL;

         // Set title info
         IWIDGET_SetImageStaticInfo(me->piTitle, &me->titleInfo, 0);
      }
   }

   if (0 == nrForms) {
      
      // There are no more forms, ensure the softkey labels
      // have been cleaned (the softkey memory is owned by the form, which may no 
      // longer exist).
      if (me->piSoftkeys) {
         IWidget *piKey = NULL;

         (void) IWIDGET_GetSoftkey(me->piSoftkeys, PROP_SOFTKEY1, &piKey);
         if (piKey) {
            IWIDGET_SetText(piKey, NULL, 0);
            IWIDGET_Release(piKey);
            piKey = NULL;
         }

         (void) IWIDGET_GetSoftkey(me->piSoftkeys, PROP_SOFTKEY2, &piKey);
         if (piKey) {
            IWIDGET_SetText(piKey, NULL, 0);
            IWIDGET_Release(piKey);
            piKey = NULL;
         }

      }
   } else {
      RootForm_Update(me, FORMITEM_THEME_BASENAME, IROOTFORM_GetTopForm(po));
   }

   return SUCCESS;
}


void RootForm_GetClientRect(IRootForm *po, IXYContainer **ppo, AEERect *rc)
{
   DECL(RootForm);

   if (rc) {
      *rc = me->rcClient;
   }

   if (ppo && me->piContainer) {
      *ppo = IROOTCONTAINER_TO_IXYCONTAINER(me->piContainer);
      IROOTCONTAINER_AddRef(me->piContainer);
   }
}

IForm *RootForm_GetForm(IRootForm *po, IForm *pifRef, boolean bNext, boolean bWrap)
{
   DECL(RootForm);
   IForm *piForm = 0;
   int nrForms, formIndex;
   
   nrForms = IVECTORMODEL_Size(me->piForms);

   if (pifRef == NULL) {
      formIndex = bNext ? 0 : nrForms - 1;
      IVECTORMODEL_GetAt(me->piForms, formIndex, (void **)&piForm);
      return piForm;
   }

   formIndex = RootForm_GetFormIndex(me, &pifRef);

   if (formIndex < 0)
      return NULL;

   formIndex += bNext ? 1 : -1;
   if (formIndex < 0) {
      formIndex = bWrap ? nrForms - 1 : -1;
   } else if (formIndex >= nrForms) {
      formIndex = bWrap ? 0 : - 1;
   }
   
   if (formIndex < 0)
      return NULL;

   IVECTORMODEL_GetAt(me->piForms, formIndex, (void **)&piForm);
   return piForm;
}

int RootForm_ResolveForm(IRootForm *po, char const *szFormUrl, IForm **ppiForm)
{
   DECL(RootForm);
   IWebUtil *piWebUtil = 0;
   AEECLSID formClsId;
   int result;
   UrlParts parts;
   char *path = 0;

   if (!ppiForm || !szFormUrl)
      return EBADPARM;

   // Assume failure
   *ppiForm = 0;

   // Parse the URL
   result = ISHELL_CreateInstance(FORM_SHELL(me), AEECLSID_WEBUTIL, (void **) &piWebUtil);

   if (result == 0) 
      result = IWEBUTIL_ParseUrl(piWebUtil, szFormUrl, &parts);

      // Check the scheme
   if (result == 0
       && (!UP_HASSCHM(&parts) || STRNCMP(parts.cpcSchm,FORM_URL_SCHEME,sizeof(FORM_URL_SCHEME)-1)))
       result = ESCHEMENOTSUPPORTED;

      // Do we have a path?
   if (result == 0
       && (!UP_HASPATH(&parts) || UP_PATHLEN(&parts) <= 0))
      result = ESCHEMENOTSUPPORTED;

   // Extract the path (we need it to be NULL terminated)
   if (result == 0 
       && 0 == (path = MALLOC(UP_PATHLEN(&parts)+1)))
      result = ENOMEMORY;

   if (result == 0) {
      STRNCPY(path, parts.cpcHost, UP_PATHLEN(&parts)+1);

      // Does a handler exist for this path, of type AEEIID_FORM?
      if (0 == (formClsId = ISHELL_GetHandler(FORM_SHELL(me), AEEIID_FORM, path)))
         // Nope...
         result = ESCHEMENOTSUPPORTED;
   }

   if (result == 0)
      // Got the actual class id, lets create the form
      result = ISHELL_CreateInstance(FORM_SHELL(me), formClsId, (void **) ppiForm);

   //
   // TODO: We could use IWEBUTIL_ParseFormFields() to parse parts.cpcSrch 
   //       for known Form properties and apply them here...

   RELEASEIF(piWebUtil);
   FREEIF(path);

   return result;
}

void RootForm_Dtor(RootForm *me)
{
   IROOTFORM_RemoveForm(ROOTFORM_TO_IROOTFORM(me), FORM_ALL);

   RELEASEIF(me->piTitle);
   RELEASEIF(me->piSoftkeys);
   RELEASEIF(me->piContainer);
   RELEASEIF(me->piBackground);
   RELEASEIF(me->titleInfo.piImage);
   RELEASEIF(me->piForms);
   RELEASEIF(me->piActiveWidget);
   RELEASEIF(me->piThemeFile);
   FREEIF(me->themeFile);

   Form_Dtor(&me->base);  
}

uint32 RootForm_Release(IRootForm *po)
{
   DECL(RootForm);
   
   if (FORM_NREFS(me) == 1)
      RootForm_Dtor(me);

   return Form_Release(IROOTFORM_TO_IFORM(po));
}

int RootForm_QueryInterface(IRootForm *po, AEECLSID clsid, void **ppo)
{
   if (clsid == AEEIID_ROOTFORM) {
      *ppo = po;
      Form_AddRef(IROOTFORM_TO_IFORM(po));
      return SUCCESS;
   }

   return Form_QueryInterface(IROOTFORM_TO_IFORM(po), clsid, ppo);
}

int RootForm_Construct(RootForm *me, AEEVTBL(IRootForm) *pvt, IModule *piModule, IShell *piShell)
{
   int result;
   WExtent extent;
   WidgetPos pos;
   IDisplay *piDisplay = 0;
   ICanvas *piCanvas = 0;

   Form_Ctor(&me->base, (AEEVTBL(IForm) *)pvt, piModule, piShell, 
             (PFNHANDLER)RootForm_HandleEvent);

   me->bStackChangePending = FALSE;
   FORM_ACTIVE(&me->base) = TRUE;

   pos.x = 0;
   pos.y = 0;
   pos.bVisible = TRUE;
   SETWEXTENT(&extent, 0, 0);

   // Form overrides
   pvt->Release         = RootForm_Release;
   pvt->QueryInterface  = RootForm_QueryInterface;
   // RootForm definitions
   pvt->InsertForm      = RootForm_InsertForm;
   pvt->RemoveForm      = RootForm_RemoveForm;
   pvt->GetClientRect   = RootForm_GetClientRect;
   pvt->GetForm         = RootForm_GetForm;
   pvt->ResolveForm     = RootForm_ResolveForm;

   result = ISHELL_CreateInstance(piShell, AEECLSID_VECTORMODEL, (void **)&me->piForms);

   if (result == 0) {
      IVECTORMODEL_SetPfnFree(me->piForms, (PFNNOTIFY)RootForm_FreeFormEntry);

      result = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void **)&piDisplay);
   }

   if (result == 0)
      result = ISHELL_CreateInstance(piShell, AEECLSID_ROOTCONTAINER, (void **)&me->piContainer);

   if (result == 0)
      result = IROOTCONTAINER_QueryInterface(me->piContainer, AEEIID_WIDGET, (void **)&me->base.piWidget);

   if (result == 0)
      result = ISHELL_CreateInstance(piShell, AEECLSID_RESFILE, (void **)&me->piThemeFile);

   if (result == 0)
      result = ISHELL_CreateInstance(piShell, AEECLSID_IMAGEWIDGET, (void **)&me->piBackground);

   if (result == 0) {
      IWIDGET_SetFlags(me->piBackground, IDF_ALIGN_RIGHT | IDF_ALIGN_BOTTOM);

      // Insert, extent will be fixed up in SetDisplay below
      result = IROOTCONTAINER_Insert(me->piContainer, me->piBackground, WIDGET_ZBOTTOMMOST, &pos);
   }

   if (result == 0)
      // Construct title
      result = ISHELL_CreateInstance(piShell, AEECLSID_IMAGESTATICWIDGET, (void **)&me->piTitle);

   if (result == 0) {
      extent.height = 15;
      // Set title font to bold by default.  Apps and themes can override it.
      IWIDGET_SetFontClass(me->piTitle, AEECLSID_FONTSYSBOLD);

      IWIDGET_SetShadowOffsetY(me->piTitle, 0);
      IWIDGET_SetBorderWidth(me->piTitle, 0);
      IWIDGET_SetExtent(me->piTitle, &extent);
      // Add to container
      result = IROOTCONTAINER_Insert(me->piContainer, me->piTitle, WIDGET_ZTOPMOST, &pos);
   }

   if (result == 0)
      // Construct Softkeys
      result = ISHELL_CreateInstance(piShell, AEECLSID_SOFTKEYWIDGET, (void **)&me->piSoftkeys);

   if (result == 0) {
      IWIDGET_SetShadowOffsetY(me->piSoftkeys, -1);
      IWIDGET_SetBorderWidth(me->piSoftkeys, 0);
      IWIDGET_SetExtent(me->piSoftkeys, &extent);
      IWIDGET_SetLeftPadding(me->piSoftkeys, 2);
      IWIDGET_SetRightPadding(me->piSoftkeys, 2);

      // Insert at 0, 0. Correct positioning will happen in SetDisplay
      result = IROOTCONTAINER_Insert(me->piContainer, me->piSoftkeys, WIDGET_ZTOPMOST, &pos);
   }

   if (result == 0)
      result = RootForm_SetDisplay(me, piDisplay);

   if (result == 0) {   
      char* pTheme = 0;
      IFORM_SetThemeBaseName(ROOTFORM_TO_IFORM(me), "Root");

      IFORM_GetThemeBaseName(ROOTFORM_TO_IFORM(me), &pTheme);
      pTheme = (pTheme) ? pTheme : "(None)";

      BUIT_LOG("FORMS EVT: Construct Set Theme Started for %s", pTheme);

      IROOTFORM_SetThemeFileName(ROOTFORM_TO_IROOTFORM(me), "theme.bar");

      BUIT_LOG("FORMS EVT: Construct Set Theme Finished for %s", pTheme);

   } else {
      RootForm_Dtor(me);
   }

   RELEASEIF(piDisplay);
   RELEASEIF(piCanvas);
   
   return result;
}


int RootForm_New(IRootForm **ppo, IModule *piModule, IShell *piShell)
{
   RootForm *me = MALLOCREC_VTBL(RootForm, IRootForm);
   int result;

   *ppo = (IRootForm *)me;

   if (!me)
      return ENOMEMORY;

   result = RootForm_Construct(me, GETVTBL(me, IRootForm), piModule, piShell);
   
   if (result != 0) {
      *ppo = NULL;
      FREE(me);
   }
   
   return result;
}


