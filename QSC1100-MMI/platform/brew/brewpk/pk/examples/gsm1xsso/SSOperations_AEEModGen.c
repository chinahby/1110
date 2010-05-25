/*===========================================================================

FILE: SSOperations_AEEModGen.c // singleton-change

SERVICES: Sample IModule implementation file

DESCRIPTION
  This file contains sample implementation of the IModule interface
  
  This file can be used for developing dynamic apps as well as static apps. Static
  apps are the apps that will be statically linked with the phone image by the OEM.
  To develop a static app, the following steps need to be done:
  1. Provide a GetModInfo() function
  2. Provide a Load() function
  3. Provide a CreateInstance() function (which is identical to the AEEClsCreateInstance()
  function that must be provided by a dynamic module.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


          Copyright © 1999-2002 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
  ========================================================================== */


/*==========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
  ========================================================================== */
#include "AEEHeap.h"    // Memory allocation Routines
#include "AEEStdLib.h"  // AEE Standard C library functions
#include "SSOperations_AEEModGen.h"  // singleton-change // Contains Sample AEEModule declarations
#include "SSOperations_AEEAppGen.h" 
#include "SSAgent.bid" // singleton-change

/*==========================================================================

                      PUBLIC DATA DECLARATIONS

  ========================================================================== */

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

/*
  || Functions to be provided by the module developers.
*/

static uint32     AEEMod_AddRef(IModule *po);
static uint32     AEEMod_Release(IModule *po);
static int        AEEMod_CreateInstance(IModule *po,IShell *pIShell,
                                        AEECLSID ClsId, void **ppObj);
static void       AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

// singleton-change - Method to actually create the instance
int SSAgentApp_New(AEECLSID clsID, IShell   *ps, IModule  *pIModule, CSSAgentApp **ppObj);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/
#ifdef AEE_SIMULATOR
   // IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
   //   SDK ONLY! This variable should NOT BE:
   //
   //      (1) overwritten 
   //      (2) USED DIRECTLY by BREW SDK users. 
   //
   //  g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,
AEEHelperFuncs *g_pvtAEEStdLibEntry;
#endif

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

/* =========================================================================
Function:  AEEMod_Load()

Description:
   This is the module load function. It is invoked when the module is being
   loaded. It creates an instance of the AEEMod structure and initializes
   the data members appropriately.

   If running under the AEE Simulator, the function is prefixed with the tag
   __declspec(dllexport) so that this function is exported by the App DLL.
   This function shall be invoked from within the AEE.
   
Prototype:
   boolean AEEMod_Load(IShell *pIShell, char *psz, IModule **ppMod)

Parameters:
   piShell: Pointer to IShell interface
   psz:     String containing name of the resource file for module
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
               structure

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:
   This function must be the first function in the first module for 
   it to work on the handset!

Side Effects: 
   None

============================================================================*/
#ifndef BREW_STATIC_APP

#ifdef AEE_SIMULATOR
__declspec(dllexport)
#endif
    int AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
{
   // Invoke helper function to do the actual loading.
   return SSOperations_AEEStaticMod_New(sizeof(AEEMod),pIShell,ph,ppMod,NULL,NULL);
}

#endif // BREW_STATIC_APP

/*===========================================================================

Function:  AEEStaticMod_New()

Description:
   This is the module load function for static modules. It is directly 
   invoked by static modules when the static module is being loaded. 
   It also serves as a helper function for dynamic modules. 
   It creates an instance of the AEEMod structure and initializes the data
   members appropriately. The only difference between this function and the
   one used for dynamic mods is that this function takes an additional 
   parameter that specifies the CreateInstance function of the static module.

Prototype:
   boolean AEEStaticMod_New(int nSize, IShell *pIShell, void *ph, 
                            IModule **ppMod,PFNMODCREATEINST pfn)

Parameters:
   nSize: Specifies the size (in bytes) of the structure to be allocated for 
           the module.  Typically, this is the size of the AEEMod structure
   piShell: Pointer to IShell interface
   ph:  Not used on phone. In SDK, this is a pointer to the AEEHelperFuncs 
           structure. 
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
           structure
   pfn: Pointer to the modules' CreateInstance() function

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
int SSOperations_AEEStaticMod_New(int16 nSize, IShell *pIShell, void *ph, IModule **ppMod,
                     PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
{
   AEEMod *pMe = NULL;
   VTBL(IModule) *modFuncs;

   if (!ppMod || !pIShell) {
      return EFAILED;
   }

   *ppMod = NULL;
  
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

   //Allocate memory for the AEEMod object

   if (nSize < sizeof(AEEMod)) {
      nSize += sizeof(AEEMod);
   }

   if (NULL == (pMe = (AEEMod *)MALLOC(nSize + sizeof(IModuleVtbl)))) {
      return ENOMEMORY;
   }
  
   // Allocate the vtbl and initialize it. Note that the modules and apps 
   // must not have any static data. Hence, we need to allocate the vtbl as 
   // well.

   modFuncs = (IModuleVtbl *)((byte *)pMe + nSize);

   // Initialize individual entries in the VTBL
   modFuncs->AddRef         = AEEMod_AddRef;
   modFuncs->Release        = AEEMod_Release;
   modFuncs->CreateInstance = AEEMod_CreateInstance;
   modFuncs->FreeResources  = AEEMod_FreeResources;


   // initialize the vtable
   INIT_VTBL(pMe, IModule, *modFuncs);

   // initialize the data members

   // Store address of Module's CreateInstance function
   pMe->pfnModCrInst = pfnMC;

   // Store Address of Module's FreeData function
   pMe->pfnModFreeData = pfnMF;

   pMe->m_nRefs = 1;
   pMe->m_pIShell = pIShell;

   pMe->pCSSAgentApp = NULL; // singleton-change
   SSAgentApp_New(AEECLSID_SS_AGENT, pIShell, (IModule*)pMe, &(pMe->pCSSAgentApp)); // singleton-change

   // Set the pointer in the parameter
   *ppMod = (IModule*)pMe;

   return SUCCESS;
}

/*===========================================================================

Function:  AEEMod_AddRef()

Description:
   This function increases the referecne count for the AEEMod object

Prototype:
   uint32 AEEMod_AddRef(IModule *po)

Parameters:
   po: Pointer to IModule interface whos reference count is to be incremented

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static uint32 AEEMod_AddRef(IModule *po)
{
   return (++((AEEMod *)po)->m_nRefs);
}


/*===========================================================================

Function:  AEEMod_Release()

Description:
   This function decreases the referecne count for the IModule object. If
   the reference count reaches zero, it does the required cleanup

Prototype:
   uint32 AEEMod_Release(IModule *po)

Parameters:
   po: Pointer to the IModule interface whose reference count needs to be
     decremented.

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static uint32 AEEMod_Release(IModule *po)
{
   AEEMod *pMe = (AEEMod *)po;

   if (--pMe->m_nRefs != 0) {
      return pMe->m_nRefs;
   }

   // Ref count is zero. So, release memory associated with this object.

   // singleton-change - release the singleton instance.
   // Applet release will also free applet data.
   if (pMe->pCSSAgentApp != NULL)
      IAPPLET_Release((IApplet*)(pMe->pCSSAgentApp));

   // First, release user-specific data if any
   // Invoke User's FreeData function if they have registered
   if (pMe->pfnModFreeData) {
      pMe->pfnModFreeData(po);
   }

   //Free the object itself
   FREE_VTBL(pMe, IModule);
   FREE(pMe);
  
   return 0;
}


/*===========================================================================

Function:  AEEMod_CreateInstance()

Description:
   This function is invoked so as to create the individual classes belonging
   to this module. It is invoked once for each class this module contains. 
   This function invokes the AEEClsCreateInstance() function that the module
   implementer must provide.

Prototype:
   int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,AEECLSID ClsId,void **ppObj)

Parameters:
   pIModule: [in]: Pointer to this module
   pIShell: [in]: Pointer to the AEE Shell interface
   ClsID:   [in]: ClassId of the class that needs to be created. 
   ppobj:   [out]: If ClsID points to an applet, then *ppobj must point to a 
            valid structure that implemetns the IApplet interface. If ClsID 
            points to a class that is not an applet, *ppobj must point to a 
            valid IBase*.

Return Value:
   SUCCESS: If successful
   Error code: If failed

Comments:  None

Side Effects: None

==============================================================================*/
static int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,
                                 AEECLSID ClsId,void **ppObj)
{
   AEEMod    *pme = (AEEMod *)pIModule;
   int        nErr = 0;

   // For a dynamic module, they must supply the AEEClsCreateInstance() 
   //   function. Hence, invoke it. For a static app, they will have 
   //   registered the create Instance function. Invoke it.
   if (pme->pfnModCrInst) {
      nErr = pme->pfnModCrInst(ClsId, pIShell, pIModule, ppObj);
#if !defined(AEE_STATIC)
   } else {
      nErr = AEEClsCreateInstance(ClsId, pIShell, pIModule, ppObj);
#endif
   }

   return nErr;
}

/*===========================================================================

Function:  AEEMod_FreeResources()

Description:
   This function is invoked so that the module can free any of the resources
   that it has loaded.

Prototype:
   void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)

Parameters:
   po: [in]: Pointer to this module
   ph: [in]: Pointer to the AEE IHeap interface
   pfm: Pointer to the resource file

Return Value:
   None

Comments:  None

Side Effects: None

==============================================================================*/
static void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)
{
   (void)po,(void)ph,(void)pfm; /* all unused */
}



/*========================End of File ================================================*/
