/*
  ========================================================================

  FILE:  frame_mod.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ModWrapper for framewidget class(es)

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "FrameWidget.h"
#include "MediaFrameModel.h"
#include "CameraFrameModel.h"
#include "xmod.h"

#ifdef AEE_STATIC

// Need to rename AEEMod_Load() to prevent duplicate symbols
#define AEEMOD_LOAD           FrameMod_Load

// Need to rename our module's CreateInstance method to prevent duplicate
// symbols
#define AEEMOD_CREATEINSTANCE FrameMod_CreateInstance

// Inline a copy of xmodstub for our module
#include "xmodstub.c"

static const AEECLSID gWidgetExtClasses[] = {
   AEECLSID_FRAMEWIDGET, AEECLSID_MEDIAFRAMEMODEL, AEECLSID_CAMERAFRAMEMODEL, 0 
};
    
// Returns static module information
PFNMODENTRY FrameMod_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
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
   if (clsid == AEECLSID_FRAMEWIDGET) {
      return FrameWidget_New((IWidget**)ppo, piModule);
   } else if (clsid == AEECLSID_MEDIAFRAMEMODEL) {
      return MediaFrameModel_New((IMediaFrameModel**)ppo, piModule);
   } else if (clsid == AEECLSID_CAMERAFRAMEMODEL) {
      return CameraFrameModel_New((ICameraFrameModel**)ppo, piModule);
   }

   return ECLASSNOTSUPPORT;
}

