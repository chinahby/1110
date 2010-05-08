/*===========================================================================
FILE: SSManagement_StateMachine.c

SERVICES: GSM1x SS Management State Machine (FSM)

GENERAL DESCRIPTION:
    All functions for the FSM - 
    Initialization, Destruction and Handling of Events.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    StateMachine_Initialize
    StateMachine_Destroy
    StateMachine_HandleEvent

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */

#include "AEEGSM1xSig.h"
#include "AEEGSM1xControl.h"
#include "GSM1xPhone.h"
#include "OEMClassIDs.h"

#include "SSManagement_App.h"
#include "SSManagement_StateMachine.h"
#include "ssmanagement.bid"	// Applet class ID
#include "SSManagement_UserEvent.h"
#include "SS_CommonMacros.h" 

#include "SSUtil.h"                         // SSUtil Class header
#include "SSMsg.h"                          // SS layer 3 messages

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/
// Time (in ms) to wait for an SS response
#define SS_RESPONSE_TIMEOUT 30000

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

// This macro is used to set a new state handler
#define MOVE_TO_STATE(newStateHandler) \
{                                      \
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:MOVE_TO_STATE: " #newStateHandler "\n", 0, 0, 0); \
    StateMachine_MoveToState(pMe, newStateHandler); \
}

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static boolean IdleState_Handler(
   CGSM1xSSManagementApp *pMe,
   AEEEvent               eCode,
   uint16                 wParam,
   uint32                 dwParam
);

static boolean WaitForResponseState_Handler(
   CGSM1xSSManagementApp *pMe,
   AEEEvent               eCode,
   uint16                 wParam,
   uint32                 dwParam
);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: StateMachine_HandleEvent

DESCRIPTION: handle a BREW event by the state machine

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.     
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
boolean StateMachine_HandleEvent(
    struct _CGSM1xSSManagementApp *pMe,
    AEEEvent                       eCode,
    uint16                         wParam,
    uint32                         dwParam
    )
{
    AEENotify* pNotify;
    AEECLSID clsid;
    uint32 dwMask;

   
    if (NULL == pMe->m_pCurrentStateHandler) 
    {
        return FALSE;
    }
    
    switch(eCode)
    {
    case EVT_NOTIFY:
        // dwParam holds AEENotify struct
        pNotify = (AEENotify*)dwParam;
        clsid = pNotify->cls;
        dwMask = pNotify->dwMask;

        switch (clsid)
        {
        case AEECLSID_PHONE:
            return pMe->m_pCurrentStateHandler(pMe, EVT_PHONE_STATUS_CHANGE, 0, 
                (uint32)pNotify->pData);

            break;
            
        case AEECLSID_IGSM1XSIGNOTIFIER:
            {
                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "StateMachine_HandleEvent: "
                    "Got IGSM1xSigNotifier notification. dwMask=%d AppStarted=%d", 
                    dwMask, pMe->m_AppStarted, 0);
                                
                // for AEECLSID_IGSM1XSIGNOTIFIER, this Applet only handles GSM1x Signaling.
                if (GET_NOTIFIER_MASK(dwMask) != NMASK_GSM1xSIG_PROTOCOL_TYPE)
                    return FALSE;
                
                if (GET_NOTIFIER_VAL(dwMask) != GSM1xSIG_PTYPE_RIL3)
                    return FALSE;
               
        		// currently, only handle these events after app is started.
	        	if (pMe->m_AppStarted == FALSE)
                    return FALSE;

                return pMe->m_pCurrentStateHandler(pMe, EVT_SS_RESPONSE, 0, 
                                                    (uint32)pNotify->pData);
            }
            break;

        case AEECLSID_IGSM1XCONTROLNOTIFIER:
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "StateMachine_HandleEvent: "
                "Got IGSM1xControlNotifier notification. dwMask=%d AppStarted=%d", 
                dwMask, pMe->m_AppStarted, 0);
            
            // check the notification mask
            if (GET_NOTIFIER_MASK(dwMask) == NMASK_GSM1xCONTROL_STATUS_CHANGE)
            {
                AEEGSM1xSig_Status newStatus = (uint32)(pNotify->pData);
                
                if ((newStatus == GSM1xSIG_INACTIVE) && (pMe->m_AppStarted))
                {
                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "StateMachine_HandleEvent: "
                        "Got GSM1xSIG_INACTIVE. Closing applet", 0, 0, 0);
                    ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                }
                return TRUE;
            }
            break;

        } // end of switch(clsid).
        break;
	}

    return pMe->m_pCurrentStateHandler(pMe, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: StateMachine_Initialize

DESCRIPTION: initialize state machine

PARAMETERS:
    pMe [in]: Pointer to the Applet structure.     

RETURN VALUE:
    TRUE for success, or FALSE for failure

=============================================================================*/
boolean StateMachine_Initialize( struct _CGSM1xSSManagementApp *pMe )
{
    pMe->m_pCurrentStateHandler = NULL;
    pMe->m_pSSUtilCls = NULL;
    pMe->m_pIGSM1xSig = NULL;

    // create the SS utilities class
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SSUTIL, 
                                (void **)&pMe->m_pSSUtilCls) != SUCCESS) 
    {                          
        return FALSE;
    }

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:E:StateMachine_Initialize", 0, 0, 0);

    // get a IGSM1xSig interface
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IGSM1XSIG, 
		(void **)&pMe->m_pIGSM1xSig) != SUCCESS)
    {
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM: Can't create IGSM1xSig", 0, 0, 0);
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:X:StateMachine_Initialize", 0, 0, 0);
        return FALSE;
    }

    // start in the Idle state
    pMe->m_pCurrentStateHandler = IdleState_Handler;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:X:StateMachine_Initialize", 0, 0, 0);
    return TRUE;
}

/*=============================================================================
FUNCTION: StateMachine_Destroy

DESCRIPTION: destroy state machine

PARAMETERS:
    pMe [in]: Pointer to the Applet structure.     

RETURN VALUE:
    None

=============================================================================*/
void StateMachine_Destroy( struct _CGSM1xSSManagementApp *pMe )
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:E:StateMachine_Destroy", 0, 0, 0);

    // release the SS utilities class
    if (pMe->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }

    // release the IGSM1x interface
    if (pMe->m_pIGSM1xSig != NULL)
    {
        IGSM1xSig_Release(pMe->m_pIGSM1xSig);
        pMe->m_pIGSM1xSig = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//   Local Function Definitions
///////////////////////////////////////////////////////////////////////////////

/*=============================================================================
FUNCTION: StateMachine_MoveToState

DESCRIPTION:
    Changes the current state.
    This function provides a partial OnEnter/OnExit mechanism:
    1. an ONEXIT event is sent to the current state.
    2. current state is changed to the new state.
    3. an ONENTER event is sent to the current (new) state.
    This mechanism is used mainly for setting and canceling timers.
   
PARAMETERS:
    pMe [in]             : Pointer to a CGSM1xSSManagementApp object.
    newStateHandler [in] : new state, identified by its state handler.

RETURN VALUE:
    None.
=============================================================================*/
static void StateMachine_MoveToState(
    CGSM1xSSManagementApp *pMe,
    STATE_EVENT_HANDLER    newStateHandler
    )
{
    if (pMe->m_pCurrentStateHandler != NULL) 
    {
        // OnExit actions
        pMe->m_pCurrentStateHandler(pMe, ONEXIT, 0, 0);
    }

    // Move to new state
    pMe->m_pCurrentStateHandler = newStateHandler;

    if (pMe->m_pCurrentStateHandler != NULL)
    {
        // OnEnter actions
        pMe->m_pCurrentStateHandler(pMe, ONENTER, 0, 0);
    }
}

/*=============================================================================
FUNCTION: StateMachineTimeoutCallback

DESCRIPTION:
   Callback function passed to the BREW function ISHELL_SetTimer. As a timer 
   set for a state expires, we post a timeout event to the state handler. 
   
PARAMETERS:
   pUserData: Assumed to be a pointer to the Applet Structure (CGSM1xSSManagementApp).
              It is dictated by BREW type PFNNOTIFY.

RETURN VALUE:
    None

=============================================================================*/
static void StateMachineTimeoutCallback(void *pUserData)
{
    CGSM1xSSManagementApp *pMe = (CGSM1xSSManagementApp*)pUserData;
    assert (pMe != NULL);

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:E:StateMachineTimeoutCallback", 0, 0, 0);

    // Post a message to the state handler
    (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                            EVT_STATE_MACHINE_TIMEOUT, 0, 0);

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:X:StateMachineTimeoutCallback", 0, 0, 0);
}

/*=============================================================================
FUNCTION: IdleState_Handler

DESCRIPTION:
   This is the Idle state event handler
   
PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.      
   eCode [in]: Specifies the Event
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the state handler has processed the event.
   FALSE:   If the state handler did not process the event.

=============================================================================*/
static boolean IdleState_Handler(
    CGSM1xSSManagementApp *pMe,
    AEEEvent               eCode,
    uint16                 wParam,
    uint32                 dwParam
    )
{
    const char* pNumber;

	#ifdef FEATURE_MULTIMODE_ARCH
	    const cm_mm_call_info_s_type* ci;
	#else
		const cm_call_info_s_type* ci;
	#endif

    long pNumberLength;

    switch(eCode)
    {
        case EVT_SS_REQUEST:
        {            
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:IdleState:Got EVT_SS_REQUEST", 0, 0, 0);

            // get the command string
            pNumber = (const char*)dwParam;
            pNumberLength = STRLEN(pNumber);

            if (pNumberLength > CM_MAX_DIALED_DIGITS)
            {
                DBGPRINTF("ERROR! CM_MAX_DIALED_DIGITS exceeded!");
                GSM1X_LOG_FATAL(
                   pMe->m_pSSUtilCls,
                   "SSM:call string is too big! len=%d > max=%d",
                   pNumberLength,
                   CM_MAX_DIALED_DIGITS,
                   0
                   );

                ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                return TRUE;
            }

            if (((ci = IPHONE_GetCallInfo(pMe->m_pIPhone)) == NULL) ||
                (ci->call_state == CM_CALL_STATE_IDLE))
            {
                if (IPHONE_MakeCall(pMe->m_pIPhone,
                         CM_CALL_TYPE_VOICE,        // Call type
                         CM_SRV_TYPE_AUTOMATIC,     // Service type
                         (word)0,                   // Service option
                         (const byte *) pNumber,    // Num to dial
                         (byte)STRLEN(pNumber),
                         CM_DIGIT_MODE_4BIT_DTMF,   // Num format                         
                         (const byte *)"",    // Alpha tag
                         (byte)0,
                         CM_OTASP_ACT_CODE_NONE) != SUCCESS)
                {
                    // notify Dialogs
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                            EVT_SS_OPER_END, SS_OPER_RES_CANT_INVOKE, 0);
                    return TRUE;
                }
            }
            else 
            {
                GSM1X_LOG_FATAL(pMe->m_pSSUtilCls, "SSM:Must be in Idle to send request!", 0, 0, 0);

                ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                return TRUE;
            }
            
            // start timer to wait for response
            CALLBACK_Init(&pMe->m_StateMachineTimerCB, StateMachineTimeoutCallback, pMe);

            (void) ISHELL_SetTimerEx(
                      pMe->a.m_pIShell,
                      SS_RESPONSE_TIMEOUT,
                      &pMe->m_StateMachineTimerCB
                      );

            ISSUTILCLS_IncCallIndependentCounter(pMe->m_pSSUtilCls);
            MOVE_TO_STATE( WaitForResponseState_Handler );
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: WaitForResponseState_Handler

DESCRIPTION:
   This is the WaitForResponse state event handler
   
PARAMETERS:
   pMe [in]: CGSM1xSSManagementApp object pointer.    
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the handler has processed the event.
   FALSE:   If the handler did not process the event.

=============================================================================*/
static boolean WaitForResponseState_Handler(
    CGSM1xSSManagementApp *pMe,
    AEEEvent               eCode,
    uint16                 wParam,
    uint32                 dwParam
    )
{
    switch(eCode)
    {
    case EVT_SS_RESPONSE:
	{
        AEEGSM1xSig_NotifyMessageType* incomingNotification;
        AEEGSM1xSig_SignalingMessageType* incomingMsg;
        SSMsg_int_msg_type* int_msg;        // allocated here and should be freed
                                            // by the handler
         
         incomingNotification = (AEEGSM1xSig_NotifyMessageType*)dwParam;
         GSM1X_LOG_HIGH(
             pMe->m_pSSUtilCls, 
             "RIL3 msg details: msg type=%d", 
             incomingNotification->messageType, 0, 0);
         
         if (incomingNotification->messageType != GSM1xSIG_SIGNALING)
         {
             // currently, not supporting GSM1xSIG_REJECT notifications
             return FALSE;
         }
         
         incomingMsg = &incomingNotification->msg.signalMessage;
                
         GSM1X_LOG_HIGH(
             pMe->m_pSSUtilCls, 
             "RIL3 msg details: rev=%d ptype=%d", 
             incomingMsg->ProtocolRevision,
             incomingMsg->ProtocolType, 0);
         
         // check protocol revision. currently only rev 0 is supported.
         if (incomingMsg->ProtocolRevision != 0)
             return FALSE;
         
         // check protocol type. only RIL3 is applicable.
         // actually we already checked the mask
         // for the correct protocol type.
         if (incomingMsg->ProtocolType != GSM1xSIG_PTYPE_RIL3)
             return FALSE;
  
  
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:WaitForResponseState:Got EVT_SS_RESPONSE", 0, 0, 0);
       
        // translate the response
        if ((int_msg = MALLOC(sizeof(SSMsg_int_msg_type))) == NULL)
        {
            GSM1X_LOG_FATAL(pMe->m_pSSUtilCls,
                "SSM:Message memory allocation failed!", 0, 0, 0);

            // notify Dialogs. error cause is actually "internal error".
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                    EVT_SS_OPER_END, SS_OPER_RES_NO_RESPONSE, 0);

            MOVE_TO_STATE( IdleState_Handler );
            return TRUE;
        }

       if ((ISSUTILCLS_Translate_ext_to_int(pMe->m_pSSUtilCls, 
					incomingMsg->ProtocolDataLen, 
                    incomingMsg->ProtocolData, int_msg)) != SSUTIL_SUCCESS)
		{
            GSM1X_LOG_FATAL(pMe->m_pSSUtilCls,
                "SSM:Message decoding failed!", 0, 0, 0);
      
            // notify Dialogs. error cause is actually "internal error".
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                    EVT_SS_OPER_END, SS_OPER_RES_NO_RESPONSE, 0);

            FREE(int_msg);
            MOVE_TO_STATE( IdleState_Handler );
            return TRUE;
        }
  
        // accept only call independant pd
        if (int_msg->gen_hdr.pd != SSMSG_PD_SS)
        {
            FREE(int_msg);
            return FALSE;
        }
        
        // response is o.k. check for a release complete message
        if (int_msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_RELEASE_COMPLETE)
        {
            // notify Dialogs with the response
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                EVT_SS_OPER_END, SS_OPER_RES_SUCCESS, (uint32)int_msg);
                     
            MOVE_TO_STATE( IdleState_Handler );
            return TRUE;
        }
        else
		{
            // ignore
            return TRUE;
		}
        break;
	}

    case EVT_PHONE_STATUS_CHANGE:
        {
			#ifdef FEATURE_MULTIMODE_ARCH
				cm_mm_call_info_s_type *callInfoNotification = (cm_mm_call_info_s_type*)dwParam;
			#else
				cm_call_info_s_type *callInfoNotification = (cm_call_info_s_type*)dwParam;
			#endif

            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls,
                "SSM:Got phone notification. call_state=%d", 
                callInfoNotification->call_state, 0, 0);

            if (callInfoNotification->call_state == CM_CALL_STATE_IDLE)
            {
                // call ended. terminate procedure. notify Dialogs
                (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                    EVT_SS_OPER_END, SS_OPER_RES_NO_RESPONSE, 0);
                
                //ISSUTILCLS_ResetCallIndependentCounter(pMe->m_pSSUtilCls);
                
                MOVE_TO_STATE( IdleState_Handler );
                return TRUE;
            }
        }

        break;

    case EVT_STATE_MACHINE_TIMEOUT:
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSM:WaitForResponseState:Got EVT_STATE_MACHINE_TIMEOUT", 0, 0, 0);

        // notify Dialogs
        (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_SS_MANAGEMENT,
                                EVT_SS_OPER_END, SS_OPER_RES_NO_RESPONSE, 0);

        MOVE_TO_STATE( IdleState_Handler );
        return TRUE;
        break;

    case ONEXIT:
        CALLBACK_Cancel(&pMe->m_StateMachineTimerCB); // cancel timer callback
        return TRUE;
        break;
    }
    
    return FALSE;
}
