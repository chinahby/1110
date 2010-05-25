/*=========================================================================
FILE: SSOperations.c

SERVICES: GSM1x SS Operations Applet

GENERAL DESCRIPTION:
	GSM1x SS Operations Applet Implementation


                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/


/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "GSM1xPhone.h"
#include "AEEGSM1xSig.h"
#include "AEEGSM1xControl.h"
#include "OEMClassIDs.h"

#include "SSOperations_App.h"
#include "SSOperations_ClassId.h"

#include "SSOperations_FSM.h"
#include "SSCallDatabase.h"
#include "SSOperations_DialogHandler.h"

#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonEventDefinitions.h"

#include "SS_CommonMacros.h"


/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

boolean SSOperationsApp_HandleEvent(
    CSSOperationsApp *pMe, 
    AEEEvent          eCode,
    uint16            wParam, 
    uint32            dwParam
    );

int SSOperationsApp_InitAppData(CSSOperationsApp *pMe);
void SSOperationsApp_FreeAppData(CSSOperationsApp* pMe);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

FUNCTION:  SSOperationsApp_InitAppData()

DESCRIPTION:
    This function initializes the app specific data.

PARAMETERS:
    pMe: Pointer to the applet structure.

RETURN VALUE:
    TRUE if successful, FALSE if failed

COMMENTS:
    The function does NOT free memory upon failure.
    Call SSOperationsApp_FreeAppData.
==============================================================================*/
int SSOperationsApp_InitAppData(CSSOperationsApp* pMe)
{
    if(!pMe)
        return EFAILED;

    pMe->m_AppStarted = FALSE;
    pMe->m_pSSUtilCls = NULL;
    pMe->m_pIGSM1xSig = NULL;

    pMe->m_pCallDatabase = SSCallDatabase_New(pMe->a.m_pIShell);

    if (pMe->m_pCallDatabase == NULL)
        return EFAILED;

    if (ISHELL_CreateInstance(
           pMe->a.m_pIShell,
           AEECLSID_SSUTIL,
           (void **)&pMe->m_pSSUtilCls
           )
           != SUCCESS)
    {
       return EFAILED;
    }

    pMe->m_pDialogHandler = 
       SSOperationsDialogHandler_New(
           pMe->a.m_pIShell, 
           pMe->m_pCallDatabase, 
           pMe->m_pSSUtilCls
           );

    if (pMe->m_pDialogHandler == NULL)
        return EFAILED;

    if (SUCCESS != ISHELL_CreateInstance(
                        pMe->a.m_pIShell, 
                        AEECLSID_IGSM1XSIG, 
                        (void**)&pMe->m_pIGSM1xSig
                        ))
    {
        return EFAILED;
    }

    pMe->m_pFSM = SSOperationsFSM_New(pMe->a.m_pIShell, pMe->m_pCallDatabase);

    if (pMe->m_pFSM == NULL)
        return EFAILED;

    return SUCCESS;
}

/*===========================================================================

FUNCTION:  SSOperationsApp_FreeAppData()

DESCRIPTION:
    This function frees the app data. This function is registered with the
    applet framework when the applet is created (inside AEEClsCreateInstance() function).
    This function is called by the app framework when the reference count of the 
    applet reaches zero. This function must free all the app data that has been
    allocated by this app. For ex: if their is data that was remembered when
    the app is suspended and resumed, those data must be freed here.

PARAMETERS:
    pMe: Pointer to the applet structure. This is of the type defined
        to store applet data. (CSSOperationsApp*, in this case).

RETURN VALUE:
    None

==============================================================================*/
void SSOperationsApp_FreeAppData(CSSOperationsApp* pMe)
{   
    if (pMe->m_pDialogHandler != NULL) 
    {
       SSOperationsDialogHandler_Release(pMe->m_pDialogHandler);
       pMe->m_pDialogHandler = NULL;
    }

    if (pMe->m_pFSM != NULL) 
    {
       SSOperationsFSM_Release(pMe->m_pFSM);
       pMe->m_pFSM = NULL;
    }

    if (pMe->m_pIGSM1xSig != NULL)
    {
        IGSM1xSig_Release(pMe->m_pIGSM1xSig);
        pMe->m_pIGSM1xSig = NULL;
    }

    if (pMe->m_pSSUtilCls != NULL)
    {
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_FreeAppData: Enter",
            0, 0, 0);
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }

    if (pMe->m_pCallDatabase != NULL) 
    {
        SSCallDatabase_Release(pMe->m_pCallDatabase);
        pMe->m_pCallDatabase = NULL;
    }
}   


/*===========================================================================

FUNCTION: SSOperationsApp_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled in this
    function.

PARAMETERS:
    pMe:    Pointer to the AEEApplet structure. This structure contains 
            information specific to this applet. It was initialized during 
            the AEEClsCreateInstance() function.
    eCode:  Specifies the Event sent to this applet
    wParam, dwParam: Event specific data.

RETURN VALUE:
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

===========================================================================*/

boolean SSOperationsApp_HandleEvent(
    CSSOperationsApp *pMe,
    AEEEvent          eCode, 
    uint16            wParam, 
    uint32            dwParam
    )
{
    int result;

    switch (eCode) 
    {
    case EVT_APP_START:
    {
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
            "Got EVT_APP_START", 0, 0, 0);

        // check GSM1x status
        if (IGSM1xSig_GetStatus(pMe->m_pIGSM1xSig) == GSM1xSIG_INACTIVE)
        {
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "GSM1x is inactive. Closing applet", 0, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;
        }
 
        // register for GSM1x status change notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_OPERATIONS,
                        AEECLSID_IGSM1XCONTROLNOTIFIER, 
                        NMASK_GSM1xCONTROL_STATUS_CHANGE)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "Register for NMASK_GSM1xCONTROL_STATUS_CHANGE failed, result=%d."
                "Closing applet", result, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE; 
        }

        // register for IPhone notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_OPERATIONS,
                        AEECLSID_PHONE, 
                        NMASK_PH_CALL_INFO)) 
                        != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(
                pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "Register for AEECLSID_PHONE failed, result=%d. Closing applet",
                result, 0, 0);
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE; 
        }

        pMe->m_AppStarted = TRUE;

        // initialize the Dialogs and the FSM modules.
        if (SSOperationsDialogHandler_Initialize(pMe->m_pDialogHandler) == FALSE)
        {
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;
        }
        
        if (SSOperationsFSM_Initialize(pMe->m_pFSM) == FALSE)
        {
            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;
        }

        // also send EVT_APP_START to both objects
        SSOperationsDialogHandler_HandleEvent(
            pMe->m_pDialogHandler, 
            eCode, 
            wParam, 
            dwParam
            );

        SSOperationsFSM_HandleEvent(pMe->m_pFSM, eCode, wParam, dwParam);

        return TRUE;
    }

    case EVT_APP_RESUME:

        // send EVT_APP_RESUME to both objects
        SSOperationsDialogHandler_HandleEvent(pMe->m_pDialogHandler, 
                                                eCode, wParam, dwParam);
        SSOperationsFSM_HandleEvent(pMe->m_pFSM, eCode, wParam, dwParam);

        return TRUE;

    case EVT_APP_STOP:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
            "Got EVT_APP_STOP", 0, 0, 0);
    
        // send EVT_APP_STOP to both objects
        if (pMe->m_pDialogHandler != NULL)
            SSOperationsDialogHandler_HandleEvent(
                pMe->m_pDialogHandler, 
                eCode, 
                wParam, 
                dwParam
                );

        if (pMe->m_pFSM != NULL)
            SSOperationsFSM_HandleEvent(pMe->m_pFSM, eCode, wParam, dwParam);

        // unregister for IPhone notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_OPERATIONS,
                        AEECLSID_PHONE, 
                        0)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(
                pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "Unregister for AEECLSID_PHONE failed, result=%d.", result, 0, 0);
        }

        // unregister for GSM1x status change notifications
        if ((result = ISHELL_RegisterNotify(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_OPERATIONS,
                        AEECLSID_IGSM1XCONTROLNOTIFIER, 
                        0)) != AEE_SUCCESS)
        { 
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "Unregister for NMASK_GSM1xCONTROL_STATUS_CHANGE failed, result=%d.",
                result, 0, 0);
        }

        return TRUE;

    case EVT_APP_SUSPEND:
        
        // send EVT_APP_SUSPEND to both objects
        if (pMe->m_pDialogHandler != NULL)
            SSOperationsDialogHandler_HandleEvent(pMe->m_pDialogHandler, 
                                            eCode, wParam, dwParam);

        if (pMe->m_pFSM != NULL)
            SSOperationsFSM_HandleEvent(pMe->m_pFSM, eCode, wParam, dwParam);

        return TRUE; // must handle EVT_APP_SUSPEND, or the applet will quit.

    case EVT_NOTIFY:
        {
            AEENotify           *pNotify;
            AEECLSID             clsid;
            uint32               dwMask;

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                "Got EVT_NOTIFY", 0, 0, 0);
            // dwParam holds AEENotify struct
            pNotify = (AEENotify*)dwParam;
            clsid = pNotify->cls;
            dwMask = pNotify->dwMask;
            
            switch (clsid)
            {
            case AEECLSID_PHONE:
            {
				#ifdef FEATURE_MULTIMODE_ARCH
					cm_mm_call_info_s_type *callInfo = (cm_mm_call_info_s_type*)(pNotify->pData);
				#else
					cm_call_info_s_type *callInfo = (cm_call_info_s_type*)(pNotify->pData);
				#endif

				if ((callInfo->call_type != CM_CALL_TYPE_VOICE) &&
					(callInfo->call_type != CM_CALL_TYPE_TEST))
				{
					return FALSE;
				}

                // handle CDMA call end
                if ((pMe->m_AppStarted) &&
                    (callInfo->call_state == CM_CALL_STATE_IDLE))
                {
                    SSEventData* pEventData;

                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                        "Got AEECLSID_PHONE event. Call is IDLE", 0, 0, 0);

                    // notify the UI
                    pEventData = SSOEvent_PrepareCallwiseMsgEvent(0, 0, NULL);
                    SS_PostEvent(
                        pMe->a.m_pIShell, 
                        AEECLSID_SS_OPERATIONS, 
                        UIMN_DROP_TCH_IND, 
                        0, 
                        (uint32)pEventData
                        );

                    return TRUE;
                }

                // don't deliver IPhone notifications to other components
                return TRUE;
            }
            break;

            case AEECLSID_IGSM1XCONTROLNOTIFIER:
                {
                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                        "Got IGSM1xControlNotifier notification. dwMask=%d AppStarted=%d",
                        dwMask, pMe->m_AppStarted, 0);
                    
                    // check the notification mask
                    if (GET_NOTIFIER_MASK(dwMask) == NMASK_GSM1xCONTROL_STATUS_CHANGE)
                    {
                        AEEGSM1xSig_Status newStatus = (uint32)(pNotify->pData);

                        if ((newStatus == GSM1xSIG_INACTIVE) && (pMe->m_AppStarted))
                        {
                            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsApp_HandleEvent: "
                                "Got GSM1xSIG_INACTIVE. Closing applet", 0, 0, 0);
                            ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                        }
                        return TRUE;
                    }
                }
                break;
            } // end of switch (EVT_NOTIFY clsid)
        }
        break;
    } // end of switch (eCode)

    // First send event to the dialog handler.
    if (FALSE == SSOperationsDialogHandler_HandleEvent(
                    pMe->m_pDialogHandler, eCode, wParam, dwParam))
    {
        // Send event to the state machine.
        if (FALSE == SSOperationsFSM_HandleEvent(pMe->m_pFSM, eCode, wParam, dwParam))
        {
            if ((eCode > EVT_SSO_PRIMITIVES_FIRST) && (eCode < EVT_SSO_PRIMITIVES_LAST))
            {
                SSOEvent_FreeEventData( (SSEventData*)dwParam );
            }
            return FALSE;
        }
    }

    return TRUE;
}
