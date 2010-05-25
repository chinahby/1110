/*=========================================================================
FILE: SSAgentApp.c

SERVICES: Holds the SSUtil class singleton and keeps it alive

GENERAL DESCRIPTION:
    This is the declaration of the SSAgentApp applet.

    The SSAgentApp loads itself upon shell initialization by using the 
    NMASK_SHELL_INIT notification from IShell. After loading, SSAgentApp stays
    loaded and runs in the background.
    The reason for staying in the background are:
    - Keeping the SSUtil class singleton
    - Storing the first call details.

    For achieving that, we take the opportunity of handling an event
    notification, and "demand" to keep running in the background.

    The invoking event is an IPhone notification. To stay running, we add a 
    reference to the applet structure with IAPPLET_AddRef. As we finish 
    handling the IPhone notification, applet memory will not be released, 
    because of this additional reference.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    CSSAgentApp class

INITIALIZATION & SEQUENCING REQUIREMENTS:
    See SSAgentApp.h

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "AEEStdLib.h"
#include "SS_CommonMacros.h"
#include "SSAgentApp.h"

#include "GSM1xPhone.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

Function:  SSAgentApp_New()

Description:
    This function allocates applet memory, and initializes the app data.

Parameters:

Return Value:
    SUCCESS or EFAILED.

==============================================================================*/
int SSAgentApp_New(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    CSSAgentApp   **ppObj
    )
{
    
    if ((ClsId != AEECLSID_SS_AGENT)
        || (pIShell == NULL)
        || (pIModule == NULL)
        || (ppObj == NULL))
    {
        return EFAILED;
    }

    if(SSOperations_AEEApplet_New(
       sizeof(CSSAgentApp),
       ClsId,
       pIShell,
       pIModule,
       (IApplet**)ppObj,
       (AEEHANDLER)SSAgentApp_HandleEvent,
       (PFNFREEAPPDATA)SSAgentApp_FreeAppData
       ) == FALSE)
    {
        return ENOMEMORY;
    }
    
    if (SSAgentApp_InitAppData((CSSAgentApp *)*ppObj) != SUCCESS) 
    {
      	IAPPLET_Release((IApplet*)*ppObj);
        *ppObj = NULL;
        return EFAILED;
    }

    return SUCCESS;
}

/*===========================================================================

Function:  SSAgentApp_InitAppData()

Description:
    This function initializes the app specific data.

Parameters:
    pMe: Pointer to the applet structure. This is of the type defined
         to store applet data. (CSSAgentApp*, in this case).

Return Value:
    SUCCESS or EFAILED.

==============================================================================*/
int SSAgentApp_InitAppData(CSSAgentApp* pMe)
{
    if(!pMe)
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

    // AddRef ourselves so we'll live forever.
    //IAPPLET_AddRef((IApplet*)pMe);
    pMe->m_FirstCallDetailsExist = FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSAgentApp_InitAppData success!", 0, 0, 0);
    return SUCCESS;
}

/*===========================================================================

Function:  SSAgentApp_FreeAppData()

Description:
    This function frees the app data. This function is registered with the
    applet framework when the applet is created (inside AEEClsCreateInstance() function).
    This function is called by the app framework when the reference count of the 
    applet reaches zero. This function must free all the app data that has been
    allocated by this app. For ex: if their is data that was remembered when
    the app is suspended and resumed, those data must be freed here.

Parameters:
    pMe: Pointer to the applet structure. This is of the type defined
         to store applet data. (CSSAgentApp*, in this case).

Return Value:
    None

==============================================================================*/
void SSAgentApp_FreeAppData(CSSAgentApp* pMe)
{
    if (pMe->m_pSSUtilCls != NULL)
    {
        (void) ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }
}

/*===========================================================================

FUNCTION SSAgentApp_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function.

PARAMETERS:
    pMe: Pointer to the CSSAgentApp structure. 
         This structure contains information specific
         to this applet. It was initialized during the AEEClsCreateInstance() function.
    ecode: Specifies the Event sent to this applet
    wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
boolean SSAgentApp_HandleEvent(
    CSSAgentApp *pMe,
    AEEEvent     eCode, 
    uint16       wParam, 
    uint32       dwParam
    )
{
    switch (eCode) 
    {
    case EVT_APP_START:
        return TRUE;

    case EVT_APP_RESUME:
        return TRUE;

    case EVT_APP_STOP:
        return TRUE;

    case EVT_APP_SUSPEND:
        return TRUE;

    case EVT_USER_SWAP_LOG_FILES_BY_CALLING_LOG:
        // this log is a must! it causes the log files to switch 
        // and a new file to be opened in the SSAgent applet's context
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "***END_OF_LOG_FILE***", 0, 0, 0);
        return TRUE;

    case EVT_NOTIFY:
        {
            // dwParam holds AEENotify struct
            AEENotify *pNotify = (AEENotify*)dwParam;
            AEECLSID clsid = pNotify->cls;

            switch (clsid)
            {
                case AEECLSID_PHONE:
                {
                    #ifdef FEATURE_MULTIMODE_ARCH
                        cm_mm_call_info_s_type *callInfo = (cm_mm_call_info_s_type*)(pNotify->pData);
                    #else
                        cm_call_info_s_type *callInfo = (cm_call_info_s_type*)(pNotify->pData);
                    #endif

                    SSCall_Origination_type tmpOrigType = SSCall_MT;
                    SSCall_PI_type       tmpPi = SS_PI_ALLOWED;
                    boolean              callTypeAllowed;

                    // tmpBuf should be sizeof(callNotification->data.num.buf)+1
                    char tmpBuf[CM_MAX_NUMBER_CHARS+1];
                    uint32 len;

                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSAgentApp: EVT_NOTIFY from IPhone", 0, 0, 0);
                    
                    if ((callInfo->call_type != CM_CALL_TYPE_VOICE) &&
                        (callInfo->call_type != CM_CALL_TYPE_TEST))
                    {
		        return FALSE;
                    }

                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, 
                        "SSAgent:Got IPhone notification. call_type=%d",
                        callInfo->call_type,
                        0,0);

                    // The code below assumes the following: 
                    // 1. The applet is registered with IPhone for 
                    //    NMASK_PH_CALL_INFO

                    if ((callInfo->call_state != CM_CALL_STATE_INCOM) &&
                        (callInfo->call_state != CM_CALL_STATE_ORIG) &&
						(callInfo->call_state != CM_CALL_STATE_CONV))
                    {
                        // we only care for these call states
                        return TRUE;
                    }

                    // call type already checked above
					callTypeAllowed = TRUE;

                    // make the number a null terminated string
                    len = callInfo->num.len;
                    if (len >= sizeof(tmpBuf)) {
                        len = sizeof(tmpBuf)-1;
                    }

                    MEMCPY(tmpBuf, callInfo->num.buf, len);
                    tmpBuf[len] = '\0';

                    switch (callInfo->call_state)
                    {
                    case CM_CALL_STATE_ORIG:
                        tmpOrigType = SSCall_MO;
                        tmpPi = SS_PI_ALLOWED;
                        break;

                    case CM_CALL_STATE_INCOM:
                        tmpOrigType = SSCall_MT;
						tmpPi = SS_PI_UNAVAILABLE;
                        break;

					case CM_CALL_STATE_CONV:
						// m_FirstCallType should have already being set
						if (pMe->m_FirstCallType == SSCall_MO)
							return TRUE;

						tmpOrigType = SSCall_MT;
						switch (callInfo->num.pi)
						{
						case CAI_ALLOW_PI:
							/* Presentation allowed */
							tmpPi = SS_PI_ALLOWED;
							break;
						case CAI_RES_PI:
							/* Presentation restricted */
							tmpPi = SS_PI_RESTRICTED;
							break;
						case CAI_NOT_AVAIL_PI:
							/* Number not available */
							tmpPi = SS_PI_UNAVAILABLE;
							break;
						default:
							ASSERT_NOT_REACHABLE;
							return TRUE;
						}
						break;

                    default:
                        ASSERT_NOT_REACHABLE;
                        return TRUE;
                    
                    }

                    SSAgentApp_SetFirstCallDetails(
                        pMe->a.m_pIShell, tmpOrigType, tmpPi, tmpBuf,
                        callTypeAllowed, ISHELL_GetUpTimeMS(pMe->m_pShell));

                    return TRUE;
                }
                break;

                case AEECLSID_SHELL:    // incoming shell notification
                {
                    // this log is a must! it causes the first log file to be opened
                    // in the SSAgent applet's context
                    GSM1X_LOG_HIGH(
                        pMe->m_pSSUtilCls, 
                        "SSAgentApp:Received EVT_NOTIFY from Shell!", 0, 0, 0);

                    return TRUE;
                }
                break;

                default:
                    GSM1X_LOG_ERROR(
                        pMe->m_pSSUtilCls, 
                        "SSAgentApp:Unexpected EVT_NOTIFY. clsid=%d", clsid, 0, 0);

                    return TRUE;
            }
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSAgentApp_SetFirstCallDetails

DESCRIPTION: Store the first call details

PARAMETERS:
    pIShell [in]         : Pointer to the Shell interface.
    type [in]            : call type, MO or MT
    pi [in]              : presentation indicator
    callString [in]      : pointer to a string that contains the call's number. 
                           the string is copied into a local buffer.
    callTypeAllowed [in] : indicates if this call is a voice or test call. The
                           SSO can be activated only if the first call is of 
                           these call types.
    startTime [in]       : call start time.

RETURN VALUE:
    None
=============================================================================*/
void SSAgentApp_SetFirstCallDetails(
    IShell *pIShell,
    SSCall_Origination_type    type,
    SSCall_PI_type             pi,
    const char                *callString,
    boolean                    callTypeAllowed,
    uint32                     startTime
    )
{
    CSSAgentApp *pMe;

    if (ISHELL_CreateInstance(
            pIShell, AEECLSID_SS_AGENT, (void**)&pMe
            ) != SUCCESS)
        return;

    pMe->m_FirstCallType = type;
    pMe->m_FirstCallPi = pi;
    
    STRLCPY(
        pMe->m_FirstCallCallString,
        callString,
        sizeof(pMe->m_FirstCallCallString));
    
    pMe->m_FirstCallTypeAllowed = callTypeAllowed;
    pMe->m_FirstCallCallStartTime = startTime;
    
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, 
        "SSAgent:SetDetails:type=%d pi=%d string=%s", type, pi, (uint32)callString);
    
    pMe->m_FirstCallDetailsExist = TRUE;

    IAPPLET_Release((IApplet*)pMe);
}

/*=============================================================================
FUNCTION: SSAgentApp_GetFirstCallDetails

DESCRIPTION: 
    Get the first call details. 
    Any out parameter can be NULL to indicate that it should be ignored.

PARAMETERS:
    pIShell [in]          : Pointer to the Shell interface.
    type [out]            : returns the call type, MO or MT
    pi [out]              : returns the presentation indicator
    callString [out]      : points to the call's number. The memory for this
                            string is owned by the Call Database; the caller
                            must not change or free it.
    callTypeAllowed [out] : returns an indication if this call is a voice or
                            test call. The SSO can be activated only if the 
                            first call is of these call types.
    startTime [out]       : returns the call start time.

RETURN VALUE:
    TRUE if a first call details exists, otherwise FALSE.
=============================================================================*/
boolean SSAgentApp_GetFirstCallDetails(
    IShell *pIShell,
    SSCall_Origination_type   *type,
    SSCall_PI_type            *pi,
    const char                **pCallString,
    boolean                   *callTypeAllowed,
    uint32                    *startTime
    )
{
    CSSAgentApp *pMe;
    
    if (ISHELL_CreateInstance(
            pIShell, AEECLSID_SS_AGENT, (void**)&pMe) != SUCCESS)
        return FALSE;
    
    if (pMe->m_FirstCallDetailsExist == FALSE)
    {
        IAPPLET_Release((IApplet*)pMe);
        return FALSE;
    }

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
        "SSAgent:GetDetails:type=%d pi=%d string=%s",
        pMe->m_FirstCallType,
        pMe->m_FirstCallPi,
        (uint32)pMe->m_FirstCallCallString);

    if (type != NULL)
        *type = pMe->m_FirstCallType;
    if (pi != NULL)
        *pi = pMe->m_FirstCallPi;
    if (pCallString != NULL)
        *pCallString = pMe->m_FirstCallCallString;
    if (callTypeAllowed != NULL)
        *callTypeAllowed = pMe->m_FirstCallTypeAllowed;
    if (startTime != NULL)
        *startTime = pMe->m_FirstCallCallStartTime;

    IAPPLET_Release((IApplet*)pMe);
    
    return TRUE;
}
