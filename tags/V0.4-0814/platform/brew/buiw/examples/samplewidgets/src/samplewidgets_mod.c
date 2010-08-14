/*======================================================
FILE:       samplewidgets_mod.c

SERVICES:   mod.c file for sample widgets creation

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "CustomCaretWidget_priv.h"
#include "CustomContainer_priv.h"
#include "CustomController_priv.h"
#include "CustomModel_priv.h"
#include "CustomWidget_priv.h"
#include "samplewidgets_util.h"
#include "xmod.h"

#ifdef AEE_STATIC

// Need to rename AEEMod_Load() to prevent duplicate symbols
#define AEEMOD_LOAD           WidgetMod_Load

// Need to rename our module's CreateInstance method to prevent duplicate
// symbols
#define AEEMOD_CREATEINSTANCE WidgetMod_CreateInstance

// Inline a copy of xmodstub for our module
#include "xmodstub.c"

// class ids this module exposes. 
static const AEECLSID      gWidgetExtClasses[] = {
   AEECLSID_CUSTOMCONTAINER, AEECLSID_CUSTOMCONTROLLER, AEECLSID_CUSTOMMODEL, AEECLSID_CUSTOMWIDGET, AEECLSID_CUSTOMCARETWIDGET, 0 
};
    
// Returns static module information
PFNMODENTRY WidgetMod_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = NULL;
   *pnApps = 0;
   *ppClasses = (AEECLSID*)gWidgetExtClasses;
   return((PFNMODENTRY)AEEMOD_LOAD);
}

#else 

// Select the default "xModule_CreateInstance" name for our module's
// CreateInstance method
#define AEEMOD_CREATEINSTANCE xModule_CreateInstance

#endif /* AEE_STATIC */


int AEEMOD_CREATEINSTANCE(IModule *piModule, IShell *piShell, AEECLSID clsid, void **ppo)
{
   int result;

   switch(clsid) {

      // Custom Container
      case AEECLSID_CUSTOMCONTAINER:
         result = CustomContainer_New((ICustomContainer **)ppo, piShell, piModule);
         break;

      // Custom Controller
      case AEECLSID_CUSTOMCONTROLLER:
         result = CustomController_New((IController **)ppo, piShell, piModule);
         break;

      // Custom Model
      case AEECLSID_CUSTOMMODEL:
         result = CustomModel_New((ICustomModel **)ppo, piShell, piModule);
         break;

      // Custom Widget
      case AEECLSID_CUSTOMWIDGET:
         result = CustomWidget_New((IWidget **)ppo, piShell, piModule);
         break;

      // Custom Caret Widget
      case AEECLSID_CUSTOMCARETWIDGET:
         result = CustomCaretWidget_New((IWidget **)ppo, piShell, piModule);
         break;
   
      // unrecognized class id
      default:
         result = ECLASSNOTSUPPORT;
   }
   
   return result;
}
