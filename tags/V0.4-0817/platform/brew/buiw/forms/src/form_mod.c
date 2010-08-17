/*
  ========================================================================

  FILE:  form_mod.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ModWrapper for Form classes

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "RootForm.h"
#include "Popup.h"
#include "Dialog.h"
#include "ListForm.h"

#include "xmod.h"


#ifdef AEE_STATIC

// Need to rename AEEMod_Load() to prevent duplicate symbols
#define AEEMOD_LOAD           FormsMod_Load

// Need to rename our module's CreateInstance method to prevent duplicate
// symbols
#define AEEMOD_CREATEINSTANCE FormsMod_CreateInstance

// Inline a copy of xmodstub for our module
#include "xmodstub.c"

static const AEECLSID      gFormsExtClasses[] = {
   AEECLSID_FORM, AEECLSID_ROOTFORM, AEECLSID_POPUPFORM, AEECLSID_DIALOGFORM,
   AEECLSID_POPUPMENUFORM, AEECLSID_LISTFORM, AEECLSID_INFODIALOG, AEECLSID_ERRORDIALOG,
   AEECLSID_WARNDIALOG, AEECLSID_PROGRESSDIALOG, AEECLSID_LISTFORM_SCROLL, 
   AEECLSID_POPUPMENUFORM_SCROLL, 0 
};
 
// Returns static module information
PFNMODENTRY FormsMod_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = NULL;
   *pnApps = 0;
   *ppClasses = (AEECLSID*)gFormsExtClasses;
   return((PFNMODENTRY)AEEMOD_LOAD);
}

#else 

// Select the default "xModule_CreateInstance" name for our module's
// CreateInstance method
#define AEEMOD_CREATEINSTANCE xModule_CreateInstance

#endif /* AEE_STATIC */



int AEEMOD_CREATEINSTANCE(IModule *module, IShell *piShell, AEECLSID clsid, void **ppo)
{
   switch (clsid) {

      case AEECLSID_FORM:
         return Form_New((IForm **)ppo, module, piShell);

      case AEECLSID_ROOTFORM:
         return RootForm_New((IRootForm **)ppo, module, piShell);

      case AEECLSID_POPUPFORM:
         return Popup_New((IForm **)ppo, module, piShell);
      
      case AEECLSID_DIALOGFORM:
         return Dialog_New((IForm **)ppo, module, piShell);
      
      case AEECLSID_POPUPMENUFORM:
         return PopupMenu_New((IPopupMenu **)ppo, module, piShell, 0);
      
      case AEECLSID_POPUPMENUFORM_SCROLL:
         return PopupMenu_New((IPopupMenu **)ppo, module, piShell, 1);

      case AEECLSID_LISTFORM:
         return ListForm_New((IForm **)ppo, module, piShell, 0);

      case AEECLSID_LISTFORM_SCROLL:
         return ListForm_New((IForm **)ppo, module, piShell, 1);
      
      case AEECLSID_INFODIALOG:
         return ResDialog_New((IForm **)ppo, module, piShell, THEME_INFO_BASENAME);
      
      case AEECLSID_ERRORDIALOG:
         return ResDialog_New((IForm **)ppo, module, piShell, THEME_ERROR_BASENAME);
      
      case AEECLSID_WARNDIALOG:
         return ResDialog_New((IForm **)ppo, module, piShell, THEME_WARN_BASENAME);
      
      case AEECLSID_PROGRESSDIALOG:
         return ProgressDialog_New((IForm **)ppo, module, piShell);     
   }

   return ECLASSNOTSUPPORT;
}
