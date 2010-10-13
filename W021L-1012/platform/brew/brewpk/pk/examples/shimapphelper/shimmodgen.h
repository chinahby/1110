#ifndef _SHIM_MOD_GEN_HEADER_ // {
#define _SHIM_MOD_GEN_HEADER_
/*============================================================================

FILE:  shimmodgen.h

SERVICES:  
        Extemely low memory allocation module generation for static applets.

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

// Allows creation of Static Module without an allocation.
extern int AEEStaticMod_NewNoAlloc(AEEMod * pMod, IShell *pIShell, void *ph,
                     PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF);

// Allows creation of static applet without allocations
extern boolean AEEStaticApplet_New(AEEApplet *pApp,
                                   AEECLSID clsID,
                                   IShell * pIShell,
                                   IModule * pIModule,
                                   AEEHANDLER pAppHandleEvent,
                                   PFNFREEAPPDATA pFreeAppData);


#endif // } _SHIM_MOD_GEN_HEADER_ 