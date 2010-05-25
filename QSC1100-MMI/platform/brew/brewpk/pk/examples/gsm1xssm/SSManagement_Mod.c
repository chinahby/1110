/*=========================================================================
FILE:  SSManagement_Mod.c

SERVICES:  
    GSM1x SS Management Module
  
GENERAL DESCRIPTION:
    Functions for GSM1x SS Management module.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSManagement_App.h" 
#include "gsm1xssm_res.h"


/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/


/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

#ifdef BREW_STATIC_APP

PFNMODENTRY 
SSManagementMod_GetModInfo(
    IShell      *pIShell,
    AEECLSID   **ppClasses,
    AEEAppInfo **ppApps,
    uint16      *pnApps,
    uint16      *pwMinPriv);

static int
SSManagementMod_Load(
    IShell      *pIShell,
    void        *pHelperFuncs,
    IModule    **ppMod);

static int 
SSManagementMod_CreateInstance(
    AEECLSID    ClsId,
    IShell     *pIShell,
    IModule    *pIModule,
    void      **ppObj);

static void 
SSManagementMod_FreeResources(
    IModule    *pIModule);

#endif // BREW_STATIC_APP


/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

#ifdef BREW_STATIC_APP

static const AEECLSID gSSManagementClasses[] =
{
   0  // Must terminate list with 0
};

static const AEEAppInfo gSSManagementApps[] = {
   {
      AEECLSID_SS_MANAGEMENT,   // Class ID
      GSM1XSSM_RES_FILE,        // File name of resource file
      SSM_APP_NAME_BASE,        // Base ID for locating title, icon, etc
      0,                 
      0,                 
      0,                 
      0,                 
      0                  // Applet flags
   }
};


#endif // BREW_STATIC_APP

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSManagementMod_GetModInfo

DESCRIPTION:
    Mandatory function for a static module.
    Returns module information in given variables.

PARAMETERS:
    pIShell [in]:       Pointer to IShell instance
    ppClasses [out]:    Returns a pointer to the exported class table, 
                        or NULL if none
    pApps [out]:        Returns a pointer to the exported applet class table, 
                        cannot be NULL
    pnApps [out]:       Returns how many elements in pApps table

RETURN VALUE:
    A function pointer to the module load entry point. Must not be NULL.
=============================================================================*/

#ifdef BREW_STATIC_APP

PFNMODENTRY SSManagementMod_GetModInfo(
    IShell      *pIShell,
    AEECLSID   **ppClasses,
    AEEAppInfo **ppApps,
    uint16      *pnApps,
    uint16      *pwMinPriv)
{
    *ppClasses = (AEECLSID*)gSSManagementClasses;
    *ppApps = (AEEAppInfo*)gSSManagementApps;
    *pnApps = sizeof(gSSManagementApps) / sizeof(gSSManagementApps[0]);

    return ((PFNMODENTRY)SSManagementMod_Load);
}

#endif // BREW_STATIC_APP

/*=============================================================================
FUNCTION: SSManagementMod_Load

DESCRIPTION:
    Mandatory function for a static module.
    Called by BREW to create and initialize the module.

PARAMETERS:
    pIShell:        Pointer to IShell interface
    pHelperFuncs:   Not used on phone. In SDK, this is a pointer to the 
                    AEEHelperFuncs structure. 
    ppMod [out]:    Returns created module

RETURN VALUE:
   SUCCESS if successful, Error code if failed

=============================================================================*/

#ifdef BREW_STATIC_APP

static int SSManagementMod_Load(
    IShell   *pIShell,
    void     *pHelperFuncs,
    IModule **ppMod)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                         pIShell,
                         pHelperFuncs,
                         ppMod,
                         SSManagementMod_CreateInstance,
                         SSManagementMod_FreeResources);
}

#endif // BREW_STATIC_APP

/*=============================================================================
FUNCTION: SSManagementMod_FreeResources

DESCRIPTION:
    Optional function for a static module,
    That frees any resources in use by this module.
    Called by BREW when the static module is destroyed.

PARAMETERS:

RETURN VALUE:

=============================================================================*/
#ifdef BREW_STATIC_APP

static void SSManagementMod_FreeResources(IModule  *pIModule)
{
}

#endif // BREW_STATIC_APP

/*=============================================================================
FUNCTION: SSManagementMod_CreateInstance

DESCRIPTION:
    Create an instance of any of the classes 
    (including applets) that this module exports.
    As SSManagement Module currently only has an Applet Class,
    we simply create it, without checking the given class id.

    For a static module, this function is invoked by BREW.
    For a dynamic module, it is called by AEEClsCreateInstance.

PARAMETERS:
    ClsId:          ID of class to create
    pIShell:        Pointer to the shell
    pIModule:       Pointer to this module
    ppObj [out]:    Returned new instance

RETURN VALUE:
    SUCCESS if successful, Error code if failed

=============================================================================*/

static int SSManagementMod_CreateInstance(AEECLSID ClsId,
                                    IShell  *pIShell,
                                    IModule *pIModule,
                                    void   **ppObj)
{
   *ppObj = NULL;

   return CGSM1xSSManagementApp_New(pIShell, pIModule, 
                                    (CGSM1xSSManagementApp**)ppObj);
}


/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
    For a DYNAMIC module, this function creates an instance of any of the 
    classes (including applets) that this module exports.

PARAMETERS:
	clsID [in]:     ClassID of the object to create
    pIShell [in]:   Contains pointer to the IShell object. 
    pIModule pin]:  Contains pointer to this Module.
    ppObj [out]:    Returned new instance

RETURN VALUE
    SUCCESS if successful, Error code if failed

===========================================================================*/
#ifndef BREW_STATIC_APP

int AEEClsCreateInstance(
        AEECLSID ClsId,
        IShell *pIShell, 
        IModule *pIModule, 
        void **ppObj)
{
    return SSManagementMod_CreateInstance(ClsId, pIShell, pIModule, ppObj);
}

#endif // not BREW_STATIC_APP