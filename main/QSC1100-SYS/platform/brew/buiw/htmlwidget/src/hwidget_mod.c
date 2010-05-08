/*
  ========================================================================

  FILE:  hwidget_mod.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ModWrapper for HTML Widget class(es)

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "HtmlWidget.h"
#include "xmod.h"

#ifdef AEE_STATIC

// Need to rename AEEMod_Load() to prevent duplicate symbols
#define AEEMOD_LOAD           HWidgetMod_Load

// Need to rename our module's CreateInstance method to prevent duplicate
// symbols
#define AEEMOD_CREATEINSTANCE HWidgetMod_CreateInstance

// Inline a copy of xmodstub for our module
#include "xmodstub.c"

static const AEECLSID gWidgetExtClasses[] = {
   AEECLSID_HTMLWIDGET, 0 
};
    
// Returns static module information
PFNMODENTRY HWidgetMod_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = NULL;
   *pnApps = 0;
   *ppClasses = (AEECLSID*)gWidgetExtClasses;
   return((PFNMODENTRY)AEEMOD_LOAD);
}

#else /* AEE_STATIC */

// Select the default "xModule_CreateInstance" name for our module's
// CreateInstance method
#define AEEMOD_CREATEINSTANCE xModule_CreateInstance

#endif /* AEE_STATIC */


int AEEMOD_CREATEINSTANCE(IModule *piModule, IShell *piShell, AEECLSID clsid, void **ppo)
{
   if (clsid == AEECLSID_HTMLWIDGET) {
      return HtmlWidget_New((IWidget**)ppo, piShell, piModule);
   }
   return ECLASSNOTSUPPORT;
}

