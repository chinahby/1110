/*=========================================================================
FILE:  SSOperations_Mod.c

SERVICES:  
    GSM1x SS Operations Module
  
GENERAL DESCRIPTION:
    Functions for GSM1x SS Operations module.
        
                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_AEEModGen.h"
#include "SSOperations_App.h"
#include "SSPromptApp.h"
#include "SSAgentApp.h" 
#include "SSOperations_ClassId.h"
#include "gsm1xsso_res.h"

#ifdef BREW_STATIC_APP
    #include "GSM1xPhone.h"
    #include "AEEGSM1xSig.h"
#endif


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

PFNMODENTRY SSOperationsMod_GetModInfo(
    IShell      *pIShell,
    AEECLSID   **ppClasses,
    AEEAppInfo **ppApps,
    uint16      *pnApps,
    uint16      *pwMinPriv
    );

static int SSOperationsMod_Load(
    IShell      *pIShell,
    void        *pHelperFuncs,
    IModule    **ppMod
    );

static int SSOperationsMod_CreateInstance(
    AEECLSID    ClsId,
    IShell     *pIShell,
    IModule    *pIModule,
    void      **ppObj
    );

static void SSOperationsMod_FreeResources(
    IModule    *pIModule
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

static const AEECLSID gSSOperationsClasses[] =
{
   0  // Must terminate list with 0
};

static const AEEAppInfo gSSOperationsApps[] = {
   {
      AEECLSID_SS_AGENT,   // Class ID
      0,                 // File name of resource file
      0,                 // String ID of app title
      0,                 // Bitmap ID of app icon 26x26
      0,                 // Bitmap ID of app icon LARGE
      0,                 // String ID of settings name
      0,                 
      0                  // Applet flags
   },
   {
      AEECLSID_SS_OPERATIONS,   // Class ID
      GSM1XSSO_RES_FILE, // File name of resource file
      SSO_APP_NAME_BASE, // Base ID for locating title, icon, etc
      0,                 
      0,                 
      0,                 
      0,                 
      0                  // Applet flags
   },
   {
      AEECLSID_SS_PROMPTAPP,   // Class ID
      0,                 // File name of resource file
      0,                 // String ID of app title
      0,                 // Bitmap ID of app icon 26x26
      0,                 // Bitmap ID of app icon LARGE
      0,                 // String ID of settings name
      0,                 
      0                  // Applet flags
   }
};

static AEECallback s_cbRegEvents;

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsMod_RegisterForEvents

DESCRIPTION:
    Registers for any events this module wishes to handle.
    This function is only defined when linking statically (BREW_STATIC_APP).
    In a dynamic applet, we register to these events by a MIF file.

PARAMETERS:
    pIShell [in]: Pointer to the IShell structure.

RETURN VALUE:
    None, although the function can fail.

=============================================================================*/
void SSOperationsMod_RegisterForEvents(IShell *pIShell)
{
    
    if (ISHELL_RegisterNotify(
            pIShell, 
            AEECLSID_SS_OPERATIONS,
            AEECLSID_IGSM1XSIGNOTIFIER, 
            ((GSM1xSIG_PTYPE_RIL3<<16) | NMASK_GSM1xSIG_PROTOCOL_TYPE)) 
            != AEE_SUCCESS)
    {
        return;
    }

    if (ISHELL_RegisterNotify(
            pIShell, 
            AEECLSID_SS_AGENT,
            AEECLSID_SHELL, 
            NMASK_SHELL_INIT) 
            != AEE_SUCCESS)
    {
        return;
    }

    if (ISHELL_RegisterNotify(
            pIShell, 
            AEECLSID_SS_AGENT,
            AEECLSID_PHONE, 
            NMASK_PH_CALL_INFO) 
            != AEE_SUCCESS)
    {
        return;
    }
    
}

/*=============================================================================
FUNCTION: SSOperationsMod_GetModInfo

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
PFNMODENTRY SSOperationsMod_GetModInfo(
    IShell      *pIShell,
    AEECLSID   **ppClasses,
    AEEAppInfo **ppApps,
    uint16      *pnApps,
    uint16      *pwMinPriv
    )
{
    //Instead of calling RegisterForevents directly, we will call call the 
    //resume, so that the SMS module will be created before registering 
    //for the events.
    CALLBACK_Init(&s_cbRegEvents, SSOperationsMod_RegisterForEvents, pIShell);
 
    ISHELL_Resume(pIShell,&s_cbRegEvents);

    *ppClasses = (AEECLSID*)gSSOperationsClasses;
    *ppApps = (AEEAppInfo*)gSSOperationsApps;
    *pnApps = sizeof(gSSOperationsApps) / sizeof(gSSOperationsApps[0]);

    return ((PFNMODENTRY)SSOperationsMod_Load);
}

/*=============================================================================
FUNCTION: SSOperationsMod_Load

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
static int SSOperationsMod_Load(
    IShell   *pIShell,
    void     *pHelperFuncs,
    IModule **ppMod
    )
{
    return SSOperations_AEEStaticMod_New(
              sizeof(AEEMod),
              pIShell,
              pHelperFuncs,
              ppMod,
              SSOperationsMod_CreateInstance,
              SSOperationsMod_FreeResources
              );
}

/*=============================================================================
FUNCTION: SSOperationsMod_FreeResources

DESCRIPTION:
    Optional function for a static module,
    That frees any resources in use by this module.
    Called by BREW when the static module is destroyed.

PARAMETERS:

RETURN VALUE:

=============================================================================*/
static void SSOperationsMod_FreeResources(IModule *pIModule)
{
}

#endif // BREW_STATIC_APP

/*=============================================================================
FUNCTION: SSOperationsMod_CreateInstance

DESCRIPTION:
    Create an instance of any of the classes 
    (including applets) that this module exports.

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
static int SSOperationsMod_CreateInstance(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    void   **ppObj
    )
{
    *ppObj = NULL;

    switch (ClsId)
    {
    case AEECLSID_SS_OPERATIONS:
        if(SSOperations_AEEApplet_New(
           sizeof(CSSOperationsApp),
           ClsId,
           pIShell,
           pIModule,
           (IApplet**)ppObj,
           (AEEHANDLER)SSOperationsApp_HandleEvent,
           (PFNFREEAPPDATA)SSOperationsApp_FreeAppData
           ) == FALSE)
        {
            return ENOMEMORY;
        }
   
        if (SSOperationsApp_InitAppData((CSSOperationsApp *)*ppObj) != SUCCESS) 
        {
          	IAPPLET_Release((IApplet*)*ppObj);
            *ppObj = NULL;
            return EFAILED;
        }
        break;

    case AEECLSID_SS_PROMPTAPP:
        if(SSOperations_AEEApplet_New(
           sizeof(CSSPromptApp),
           ClsId,
           pIShell,
           pIModule,
           (IApplet**)ppObj,
           (AEEHANDLER)SSPromptApp_HandleEvent,
           (PFNFREEAPPDATA)SSPromptApp_FreeAppData
           ) == FALSE)
        {
            return ENOMEMORY;
        }
        
        if (SSPromptApp_InitAppData((CSSPromptApp *)*ppObj) != SUCCESS) 
        {
          	IAPPLET_Release((IApplet*)*ppObj);
            *ppObj = NULL;
            return EFAILED;
        }
        break;

    case AEECLSID_SS_AGENT:
    {
        AEEMod* pMod = (AEEMod*)pIModule;
        if (pMod->pCSSAgentApp == NULL)
        {
            return EFAILED;
        }

        *ppObj = pMod->pCSSAgentApp;
        IAPPLET_AddRef((IApplet*) (pMod->pCSSAgentApp));
        break;
    }

    default:
        return EFAILED;
    }

    return(AEE_SUCCESS);
}

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION:
    This function is invoked while the app is being loaded. All Modules must 
    provide this function. In here, the module must verify the ClassID and then
    invoke the AEEApplet_New() function that has been provided in AEEAppGen.c.

    Note that this functions also serves all module classes, and 
    only exists when the module uses DYNAMIC linkage.

PARAMETERS:
    clsID [in]      - Specifies the ClassID of the applet which is being loaded
    pIShell [in]    - Contains pointer to the IShell interface. 
    pIModule [pin]  - Contains pointer to the IModule to which this app belongs
    ppObj [out]     - On return, *ppObj must point to a valid IApplet structure. 
                      Allocation of memory for this structure and initializing the 
                      base data members is done by AEEApplet_New().

RETURN VALUE:
  AEE_SUCCESS - If the app needs to be loaded, and AEEApplet_New() succeeded.
  Error Code  - If the app should not be loaded, or AEEApplet_New() failed. 

===========================================================================*/
#ifndef BREW_STATIC_APP

int AEEClsCreateInstance(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    void   **ppObj
    )
{
    return SSOperationsMod_CreateInstance(ClsId, pIShell, pIModule, ppObj);
}

#endif // !BREW_STATIC_APP
