/*=============================================================================
FILE: SSManagement_App.c

SERVICES: 
	GSM1x SS Management Applet

GENERAL DESCRIPTION:
	GSM1x SS Management Applet Implementation

PUBLIC CLASSES AND STATIC FUNCTIONS:
    CGSM1xSSManagementApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/


/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */

#include "SSManagement_App.h"
#include "SSManagement_DialogHandler.h"
#include "SSManagement_StateMachine.h"
#include "SS_CommonMacros.h"

#include "ssmanagement.bid"	   // Applet class ID

#include "AEEGSM1xSig.h" 
#include "AEEGSM1xControl.h" 
#include "OEMClassIDs.h" 

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

static boolean GSM1xSSManagementApp_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam,
    uint32                dwParam
    );

static int GSM1xSSManagementApp_InitAppData(CGSM1xSSManagementApp *pMe);
static void GSM1xSSManagementApp_FreeAppData(CGSM1xSSManagementApp *pMe);


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: CGSM1xSSManagementApp_New

DESCRIPTION:   
   This function is invoked when the app is being loaded. It creates a new 
   instance of the CGSM1xSSManagementApp structure for this applet.

PARAMETERS:
   pIShell [in]:    Contains a valid pointer to the AEE IShell interface
   pIModule [in]:   Contains a valid pointer to the Applet's module 
   ppObj [out] :    Address of a pointer. On return, *ppobj points to a 
                    valid AEEApplet structure, allocated by this function.
RETURN VALUE:
  AEE_SUCCESS:  If the app needs to be loaded and if AEEApplet_New() invocation was
               successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
           AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

=============================================================================*/
int CGSM1xSSManagementApp_New(
    IShell                *pIShell,
    IModule               *pIModule,
    CGSM1xSSManagementApp **ppObj
    )
{
	CGSM1xSSManagementApp *pMe = NULL;

    if(AEEApplet_New(
            sizeof(CGSM1xSSManagementApp), 
			AEECLSID_SS_MANAGEMENT, 
            pIShell,pIModule,
            (IApplet**)(&pMe),
			(AEEHANDLER)GSM1xSSManagementApp_HandleEvent,
			(PFNFREEAPPDATA)GSM1xSSManagementApp_FreeAppData
            )
         == FALSE)
    {
         return (EFAILED);
    }

	*ppObj = pMe;

    if (GSM1xSSManagementApp_InitAppData(pMe) != SUCCESS)
    {    
        IAPPLET_Release((IApplet*)*ppObj);
        *ppObj = NULL;        
        return EFAILED;
    }

    return SUCCESS;
}

/*=============================================================================
FUNCTION: GSM1xSSManagementApp_InitAppData

DESCRIPTION:
    This function initializes the applet specific data.

PARAMETERS:
	pMe: Pointer to the CGSM1xSSManagementApp applet structure

RETURN VALUE:
	SUCCESS: If successful
	EFAILED: IF failed

=============================================================================*/
static int GSM1xSSManagementApp_InitAppData(CGSM1xSSManagementApp *pMe)
{
	AEEDeviceInfo di;

    pMe->m_WasDialogHandlerInitCalled = FALSE;
    pMe->m_WasStateMachineInitCalled = FALSE;
	
    if(!pMe)
        return EFAILED;

    pMe->m_pIPhone = NULL;

    // Create Phone instance
    if (AEE_SUCCESS != ISHELL_CreateInstance(
                          pMe->a.m_pIShell, 
                          AEECLSID_PHONE, 
                          (void **)&pMe->m_pIPhone)
                          )
    {
        return EFAILED;
    }
    
    pMe->m_WasDialogHandlerInitCalled = TRUE;
    DialogHandler_Initialize(pMe);

    pMe->m_WasStateMachineInitCalled = TRUE;
    if (StateMachine_Initialize(pMe) == FALSE)
    {
        return EFAILED;
    }

    // Initialize other data members
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
    SETAEERECT(&(pMe->m_cMenuFrame), 0, 0, di.cxScreen, di.cyScreen);

    pMe->m_bSuspended = FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, 
        "GSM1xSSManagementApp_InitAppData: Success\n", 0, 0, 0);
    return SUCCESS;
}

/*=============================================================================
FUNCTION: GSM1xSSManagementApp_FreeAppData

DESCRIPTION:
	This function frees the app data. This function is registered with the
	applet framework when the applet is created (inside AEEClsCreateInstance() function).
	This function is called by the app framework when the reference count of the 
	applet reaches zero. This function must free all the app data that has been
	allocated by this app. 

PARAMETERS:
	pMe: Pointer to the CGSM1xSSManagementApp applet structure. 

RETURN VALUE:
	None

=============================================================================*/
static void GSM1xSSManagementApp_FreeAppData(CGSM1xSSManagementApp *pMe)
{
    if (pMe->m_WasStateMachineInitCalled) StateMachine_Destroy(pMe);
    if (pMe->m_WasDialogHandlerInitCalled) DialogHandler_Destroy(pMe);

    // release the IPhone interface
    if (pMe->m_pIPhone != NULL)
    {
        IPHONE_Release(pMe->m_pIPhone);
        pMe->m_pIPhone = NULL;
    }
}

/*===========================================================================

FUNCTION GSM1xSSManagementApp_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function.

PARAMETERS:
    pMe: Pointer to the AEEApplet structure. 
	eCode: Specifies the Event sent to this applet
    wParam, dwParam: Event specific data.

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

===========================================================================*/
static boolean GSM1xSSManagementApp_HandleEvent(
    CGSM1xSSManagementApp *pMe,
    AEEEvent              eCode, 
    uint16                wParam, 
    uint32                dwParam
    )
{ 
    int result;

	switch (eCode) 
	{
    case EVT_APP_START:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSMHandleEvent: "
            "Got EVT_APP_START", 0, 0, 0);

        // check GSM1x status
        if (IGSM1xSig_GetStatus(pMe->m_pIGSM1xSig) == GSM1xSIG_INACTIVE)
        {
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagementApp_HandleEvent: "
                "GSM1x is inactive. Closing applet", 0, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;
        }

        // register for GSM1x status change notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_IGSM1XCONTROLNOTIFIER, 
                        NMASK_GSM1xCONTROL_STATUS_CHANGE)
                        ) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagementApp_HandleEvent: "
                "Register for AEECLSID_IGSM1XCONTROLNOTIFIER failed, result=%d."
                "Closing applet", result, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE; 
        }

        // register for IPhone notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_PHONE, 
                        NMASK_PH_CALL_INFO)) 
                        != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(
                pMe->m_pSSUtilCls, "GSM1xSSManagmentApp_HandleEvent: "
                "Register for AEECLSID_PHONE failed, result=%d. Closing applet",
                result, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE; 
        }

        // register for GSM1x signalling message notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_IGSM1XSIGNOTIFIER, 
                        (GSM1xSIG_PTYPE_RIL3<<16) | NMASK_GSM1xSIG_PROTOCOL_TYPE)) 
                        != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagmentApp_HandleEvent: "
                "Register for AEECLSID_IGSM1XSIGNOTIFIER failed, result=%d."
                "Closing applet", result, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE; 
        }

 	    pMe->m_AppStarted=TRUE;
        break;

	case EVT_APP_STOP:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSMHandleEvent: "
                      "Got EVT_APP_STOP", 0, 0, 0);
		pMe->m_AppStarted=FALSE;

        // unregister for IPhone notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_PHONE, 
                        0)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagmentApp_HandleEvent: "
                "Unregister for AEECLSID_PHONE failed, result=%d.", result, 0, 0);
        }

        // unregister for GSM1x signalling message notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_IGSM1XSIGNOTIFIER,
                        0)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagmentApp_HandleEvent: "
                "Unregister for AEECLSID_IGSM1XSIGNOTIFIER failed, result=%d",
                result, 0, 0);
        }

        // unregister for GSM1x status change notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_MANAGEMENT,
                        AEECLSID_IGSM1XCONTROLNOTIFIER, 
                        0)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "GSM1xSSManagmentApp_HandleEvent: "
                "Unregister for AEECLSID_IGSM1XCONTROLNOTIFIER failed, result=%d",
                result, 0, 0);
        }
        break;

	case EVT_APP_SUSPEND:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSMHandleEvent: "
                      "Got EVT_APP_SUSPEND", 0, 0, 0);
		// Got to handle the suspend event, otherwise the app will quit on a suspend.
        // active dialog will be closed auto-magically
		pMe->m_bSuspended = TRUE;
		break;

	case EVT_APP_RESUME:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSMHandleEvent: "
                      "Got EVT_APP_RESUME", 0, 0, 0);
		pMe->m_bSuspended = FALSE;
        break;

	} // end of switch(eCode)

    // Routing events to SSM parts - first try the UI, then the FSM.
    if ((DialogHandler_HandleEvent(pMe, eCode, wParam, dwParam) == TRUE) ||
	    (StateMachine_HandleEvent(pMe, eCode, wParam, dwParam) == TRUE))
    {	
	    return TRUE;
    }
	
	return FALSE;	
}

