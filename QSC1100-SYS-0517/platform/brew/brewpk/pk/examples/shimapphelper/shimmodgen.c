/*============================================================================

FILE:  shimmodgen.c

SERVICES:  
        Implementation for extemely low memory allocation module generation 
        for static applets.

GENERAL DESCRIPTION:
        Allows static applets structure to be held within ROM space.

REVISION HISTORY: 

============================================================================

               Copyright © 2004 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "shimmodgen.h"

// Static Module declarations...
static uint32     AEEStaticMod_AddRef(IModule *po);
static uint32     AEEStaticMod_Release(IModule *po);
static int        AEEStaticMod_CreateInstance(IModule *po,IShell *pIShell,
                                        AEECLSID ClsId, void **ppObj);
static void       AEEStaticMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

static void       AEEStaticMod_CleanStruct(AEEMod * pm);

static const VTBL(IModule)  gsStaticModuleFunc =   {AEEStaticMod_AddRef,
                                                    AEEStaticMod_Release,
                                                    AEEStaticMod_CreateInstance,
                                                    AEEStaticMod_FreeResources};

static uint32     AEEStaticApplet_AddRef(IApplet * po);
static uint32     AEEStaticApplet_Release(IApplet * po);
static boolean    AEEStaticApplet_HandleEvent(IApplet * po, AEEEvent evt, 
                                              uint16 wParam, uint32 dwParam);
static void       AEEStaticApplet_CleanStruct(AEEApplet * pa);

static const VTBL(IApplet) gsAppletFuncs =   {AEEStaticApplet_AddRef,
                                              AEEStaticApplet_Release,
                                              AEEStaticApplet_HandleEvent};

// BEGIN Static Module implementations...
#ifdef AEE_SIMULATOR
#include "AEEStdLib.h"
   // IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
   //   SDK ONLY! This variable should NOT BE:
   //
   //      (1) overwritten 
   //      (2) USED DIRECTLY by BREW SDK users. 
   //
   //  g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,
AEEHelperFuncs *g_pvtAEEStdLibEntry;
#endif

int AEEStaticMod_NewNoAlloc(AEEMod * pMod, IShell *pIShell, void *ph,
                     PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
{
   if( !pMod || !pIShell || !pfnMC ){
      return EFAILED;
   }
   AEEStaticMod_CleanStruct(pMod);
  
#ifdef AEE_SIMULATOR
   // IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
   //   SDK ONLY! This variable should NOT BE:
   //
   //      (1) overwritten 
   //      (2) USED DIRECTLY by BREW SDK users. 
   //
   //  g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,
   //   DO NOT REMOVE the next five lines.
   if (!ph) {
      return EFAILED;
   } else {
      g_pvtAEEStdLibEntry = (AEEHelperFuncs *)ph;
   }
#endif

   INIT_VTBL(pMod, IModule, gsStaticModuleFunc);
   pMod->pfnModCrInst   = pfnMC;
   pMod->pfnModFreeData = pfnMF;

   pMod->m_nRefs        = 1;
   pMod->m_pIShell      = pIShell;
   ISHELL_AddRef(pIShell);

   return SUCCESS;
}

static uint32 AEEStaticMod_AddRef(IModule *po)
{
   return (++((AEEMod *)po)->m_nRefs);
}


static uint32 AEEStaticMod_Release(IModule *po)
{
   AEEMod *pMe = (AEEMod *)po;

   if (--pMe->m_nRefs != 0) {
      return pMe->m_nRefs;
   }

   if (pMe->pfnModFreeData) {
      pMe->pfnModFreeData(po);
   }
   ISHELL_Release(pMe->m_pIShell);
   AEEStaticMod_CleanStruct(pMe);
  
   return 0;
}

static int AEEStaticMod_CreateInstance(IModule *pIModule,IShell *pIShell,
                                 AEECLSID ClsId,void **ppObj)
{
   AEEMod * pme   = (AEEMod *)pIModule;
   int      nErr  = EFAILED;

   if( pme->pfnModCrInst ){
      nErr = pme->pfnModCrInst(ClsId, pIShell, pIModule, ppObj);
   }

   return nErr;
}
static void AEEStaticMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)
{
   (void)po,(void)ph,(void)pfm; /* all unused */
}
// Static Module helper fn
static void AEEStaticMod_CleanStruct(AEEMod * pm)
{
   pm->m_nRefs             = 0;
   pm->m_pIShell           = 0;
   pm->pfnModCrInst        = NULL;
   pm->pfnModFreeData      = NULL;
   GET_PVTBL(pm, IModule)  = NULL;
}
// END static module implementations

// BEGIN static applet implementations
static uint32  AEEStaticApplet_AddRef(IApplet * po)
{
   return ++(((AEEApplet *)po)->m_nRefs);
}

static uint32 AEEStaticApplet_Release(IApplet * po)
{
   AEEApplet * pme = (AEEApplet *)po;

   if( --pme->m_nRefs ){
      return pme->m_nRefs;
   }

   if( pme->pFreeAppData ){
      pme->pFreeAppData(po);
   }
   if( pme->m_pIShell ){
      ISHELL_Release(pme->m_pIShell);
   }
   if( pme->m_pIModule ){
      IMODULE_Release(pme->m_pIModule);
   }
   if( pme->m_pIDisplay ){
      IDISPLAY_Release(pme->m_pIDisplay);
   }

   AEEStaticApplet_CleanStruct(pme);

   return 0;
}
boolean  AEEStaticApplet_HandleEvent(IApplet * po, AEEEvent evt, uint16 wParam, 
                               uint32 dwParam)
{
   return ((AEEApplet *)po)->pAppHandleEvent(po, evt,wParam,dwParam);
}

boolean AEEStaticApplet_New(AEEApplet *pApp,
                            AEECLSID clsID,
                            IShell * pIShell,
                            IModule * pIModule,
                            AEEHANDLER pAppHandleEvent,
                            PFNFREEAPPDATA pFreeAppData)
{
   if( !pApp ){
      return FALSE;
   }
   if( !pIShell || !pIModule ){
      return FALSE;
   }

   INIT_VTBL(pApp, IApplet, gsAppletFuncs);

   pApp->m_nRefs           = 1;
   pApp->m_pIShell         = pIShell;
   pApp->m_pIModule        = pIModule;
   pApp->m_pIDisplay       = NULL;
   pApp->clsID             = clsID;
   pApp->pAppHandleEvent   = pAppHandleEvent;
   pApp->pFreeAppData      = pFreeAppData;

   ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY, (void **)&pApp->m_pIDisplay);

   if( !pApp->m_pIDisplay ){
      AEEStaticApplet_CleanStruct(pApp);
      return FALSE;
   }
   ISHELL_AddRef(pIShell);
   IMODULE_AddRef(pIModule);
  
   return TRUE;
}
// Static Module helper fn

static void AEEStaticApplet_CleanStruct(AEEApplet * pa)
{
   pa->clsID               = 0;
   pa->m_nRefs             = 0;
   pa->m_pIShell           = NULL;
   pa->m_pIModule          = NULL;
   pa->m_pIDisplay         = NULL;
   pa->pAppHandleEvent     = NULL;
   pa->pFreeAppData        = NULL;
   GET_PVTBL(pa, IApplet)  = NULL;
}
// END Static Applet helper fn