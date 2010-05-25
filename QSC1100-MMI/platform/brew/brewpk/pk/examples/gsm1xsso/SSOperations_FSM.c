/*=============================================================================
FILE: SSOperations_FSM.c

SERVICES: Provides the GSM1x SS Operations Finite State Machine entry point.
          Interact with the MN and the CC layers of the state machine

GENERAL DESCRIPTION:
    SSO state machine is composed of the following sub SM:
    - MN which handles procedures that involves more than one call (e.g.
      accept waiting call).
    - CC Manager and CC instances. Every CC instance handles a single call.
    - Toggle SM which handles the procedure of toggling between two calls. 
      The Toggle SM is part of the MN layer.
    - Hold SM which handles the HOLD auxiliary states for a single call. The 
      Hold SM is part of every CC instance.
    - Multi Party SM which handles the MPTY auxiliary states for a single call.
      The MPTY SM is part of every CC instance.

    The FSM is the entry point for external events to the SSO state machine.
    External events include standard BREW events along with the SSOUI-FSM
    interface primitives.
    The events are processed by a HandleEvent function which, if needed, 
    routes them either to the MN or to the CC Manager. They are further 
    processed by HandleEvent functions of these sub SM.

    Internal events (on the MN-CC interface or MN/CC internal events) are sent
    using an event queue maintained by the FSM.
    After handling each external event, the FSM iterates through this queue and
    handles all events in it until the queue is empty.
    Every event handling may result in additional events inserted to the event
    queue.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    CSSOperationsFSM

INITIALIZATION AND SEQUENCING REQUIREMENTS:
    After creating an CSSOperationsFSM object, call SSOperationsFSM_Initialize
    to initialize it.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_FSM.h"
#include "SSOperations_ClassId.h"
#include "SSOperations_UserEvents.h"
#include "SSCallDatabase.h"
#include "SS_CommonMacros.h"
#include "SSOperations_CommonEventDefinitions.h"
#include "SSOperations_CommonMessageDefinitions.h"
#include "SSqueue.h"

#include "SSUtil.h"                         // SSUtil Class header
#include "SSMsg.h"                          // SS layer 3 messages
#include "SSAgentApp.h"
#include "SSPromptApp.h"
#define  SSOPERATIONS_RES_FILE GSM1XSSO_RES_FILE

#include "AEEGSM1xSig.h"
#include "OEMClassIDs.h"
#include "gsm1xsso_res.h"	    // Applet Resources

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// timeout for displaying SSO notifications while the app is not started
#define SSO_NOTIFICATIONS_PROMPT_TIMEOUT 3000

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION: SSOperationsFSM_New

DESCRIPTION: Create an FSM object

PARAMETERS:
    pIShell [in]       : Pointer to the IShell interface
    pCallDatabase [in] : Pointer to a valid CallDatabase instance

RETURN VALUE:
   Pointer to the created SSOperationsFSM or NULL on failure.

COMMENTS:
    in order to free SSOperationsFSM object, use SSOperationsFSM_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsFSM* SSOperationsFSM_New(
    IShell* pIShell,
    SSCallDatabase* pCallDatabase
    )
{
    CSSOperationsFSM* obj;

    obj = MALLOC(sizeof(CSSOperationsFSM));
    if (obj == NULL)
        return NULL;

    obj->m_pCallDatabase = pCallDatabase;
    obj->m_pIShell = pIShell;
    ISHELL_AddRef(pIShell);

    // create the SS utilities class
    if (ISHELL_CreateInstance(obj->m_pIShell, AEECLSID_SSUTIL, (void **)&obj->m_pSSUtilCls) != SUCCESS)
        return NULL;

    // get the IGSM1x interfaces
    if (ISHELL_CreateInstance(obj->m_pIShell, AEECLSID_IGSM1XSIG, (void **)&obj->m_pIGSM1xSig) != SUCCESS)
    {
        GSM1X_LOG_HIGH(obj->m_pSSUtilCls, "SSOperationsFSM_New: Can't create AEECLSID_IGSM1XSIG", 0, 0, 0);
        return NULL;
    }

    // Note: for non static applet, registration for incoming GSM1x signaling
    //       messages is done from the MIF file.
    //       for static applet, registration is done from the SSOperations Module.

    obj->m_AppStarted = FALSE;
    obj->m_pNotify = NULL;

    GSM1X_LOG_HIGH(obj->m_pSSUtilCls, "SSO:X:FSM_New", 0, 0, 0);

    return obj;
}

/*=============================================================================
FUNCTION: SSOperationsFSM_Initialize

DESCRIPTION: Initialize an FSM object.

PARAMETERS:
     pMe [in]: Pointer to the FSM object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.

COMMENTS:
    This function is only called upon EVT_APP_START (and not upon InitAppData).
    Until then, most of the data members in CSSOperationsFSM and in other
    sub SMs are not initialized.
=============================================================================*/
boolean SSOperationsFSM_Initialize( CSSOperationsFSM *pMe )
{
    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:E:FSM_Initialize", 0, 0, 0);

    pMe->m_pEventQueue = NULL;
    if ((pMe->m_pEventQueue = SSQueue_New()) == NULL)
        return FALSE;

    // create and initialize the CC Manager
    pMe->m_pCC = NULL;
    if ((pMe->m_pCC = SSOperationsCC_New(pMe->m_pIShell, pMe, pMe->m_pCallDatabase)) == NULL)
        return FALSE;

    if (SSOperationsCC_Initialize(pMe->m_pCC) == FALSE)
        return FALSE;

    // create and initialize the MN
    pMe->m_pMN = NULL;
    if ((pMe->m_pMN = SSOperationsMN_New(pMe->m_pIShell)) == NULL)
        return FALSE;

    if (SSOperationsMN_Initialize(pMe->m_pMN, pMe, pMe->m_pCallDatabase) == FALSE)
        return FALSE;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO:X:FSM_Initialize", 0, 0, 0);

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsFSM_Release

DESCRIPTION: Destroy an FSM object.

PARAMETERS:
     pMe [in]: Pointer to the FSM object to be destroyed.

RETURN VALUE:
    None

COMMENTS:
    The FSM does not maintain a reference counter. Calling Release always
    destroy the given object.
=============================================================================*/
void SSOperationsFSM_Release( CSSOperationsFSM *pMe )
{
    // release the SS utilities class
    if (pMe->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pMe->m_pSSUtilCls);
        pMe->m_pSSUtilCls = NULL;
    }
    
    if (pMe->m_pMN != NULL)
    {
        SSOperationsMN_Release(pMe->m_pMN);
        pMe->m_pMN = NULL;
    }

    if (pMe->m_pCC != NULL)
    {
        SSOperationsCC_Release(pMe->m_pCC);
        pMe->m_pCC = NULL;
    }

    if (pMe->m_pEventQueue != NULL)
    {
        SSQueue_Release(pMe->m_pEventQueue);
        pMe->m_pEventQueue = NULL;
    }

    // registration for IGSM1x notifications stay as is.

    // release the IGSM1xSig interface
    if (pMe->m_pIGSM1xSig != NULL)
    {
        IGSM1xSig_Release(pMe->m_pIGSM1xSig);
        pMe->m_pIGSM1xSig = NULL;
    }
    
    ISHELL_Release(pMe->m_pIShell);

    FREE(pMe);
}

/*=============================================================================
FUNCTION: SSOperationsFSM_SendSSMessage

DESCRIPTION:
    Send a GSM1x signaling message to the network using IGSM1xSig.
    Before sending it, the message is translated into external (packed) format.
   
PARAMETERS:
    pMe [in] : Pointer to the FSM object.
    msg [in] : GSM1x SS signaling message to send.

RETURN VALUE:
    None.
=============================================================================*/
void SSOperationsFSM_SendSSMessage(
    CSSOperationsFSM   *pMe,
    SSMsg_int_msg_type *msg
    )
{
    uint8                ext_msg_len;
    uint8                ext_msg[SSMSG_MSG_MAX_LENGTH];
    SSUtil_error_type    result;

    uint8 i;
    int gsm1xResult;
    AEEGSM1xSig_SignalingMessageType* outgoingMsg;

    // translate the message
    ext_msg_len = sizeof(ext_msg);
    result = ISSUTILCLS_Translate_int_to_ext(
        pMe->m_pSSUtilCls,
        msg,
        &ext_msg_len,
        ext_msg
        );
    assert(result == SSUTIL_SUCCESS);

    outgoingMsg = MALLOC(sizeof(AEEGSM1xSig_SignalingMessageType));
    assert (outgoingMsg != NULL);

    // if the packed message is too long, the signaling mechanism must be
    // enhanced.
    assert (ext_msg_len <= GSM1xSIG_SIGNALING_MAX);

    outgoingMsg->ProtocolDataLen = ext_msg_len;
    MEMCPY(outgoingMsg->ProtocolData, ext_msg, ext_msg_len);
    outgoingMsg->ProtocolRevision = 0;
    outgoingMsg->ProtocolType = GSM1xSIG_PTYPE_RIL3;

    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Sending signaling msg. len=%d", ext_msg_len, 0, 0);
    for (i=0; i<ext_msg_len; i++)
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: char%d=%d", i, ext_msg[i], 0);

    gsm1xResult = IGSM1xSig_SendSignalingMessage(pMe->m_pIGSM1xSig,outgoingMsg);
    if (gsm1xResult != SUCCESS)
        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Failed to send signaling msg. result=%d", gsm1xResult, 0, 0);

    FREE(outgoingMsg);
}

/*=============================================================================
FUNCTION: SSOperationsFSM_SendReleaseComplete

DESCRIPTION:
    Send a RELEASE COMPLETE message to the network.
    Actual sending is done using SSOperationsFSM_SendSSMessage.
   
PARAMETERS:
    pMe [in]    : Pointer to the FSM object.
    TI  [in]    : Transaction identifier to use for the outgoing message.
    cause_value : Cause value to use for the outgoing message.

RETURN VALUE:
    None.

SEE ALSO:
    SSOperationsFSM_SendSSMessage
=============================================================================*/
void SSOperationsFSM_SendReleaseComplete(
    CSSOperationsFSM   *pMe,
    uint8 TI,
    SSMsg_cause_value_type cause_value
    )
{
    SSMsg_int_msg_type *msg = 
        SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_RELEASE_COMPLETE);

    assert(msg != NULL);
    
    SSOMessage_FillHeader(msg, TI);
    
    msg->release_complete.cause.hdr.present = TRUE;
    msg->release_complete.cause.coding_standard = SSMSG_CODING_STANDARD_GSM;
    msg->release_complete.cause.location = SSMSG_LOCATION_USER;
    msg->release_complete.cause.diagnostic_len = 0;
    msg->release_complete.cause.cause_value = cause_value;
    
    SSOperationsFSM_SendSSMessage(pMe, msg);
    SSOMessage_FreeMsg(msg);
}

/*=============================================================================
FUNCTION: SSOperationsFSM_CheckIncomingTI

DESCRIPTION: 
    Check the validity of an incoming GSM1x signaling message. The validity
    check is based on the TI and the message type of the incoming message.
    A response, according to the relevant protocol specifications is sent to
    the network if needed.

PARAMETERS:
    pMe [in]      : Pointer to the FSM object.
    TI [in]       : the incoming TI to check.
    msg_type [in] : message type of the incoming message.

RETURN VALUE:
    TRUE  - the message was found valid, further processing is needed.
    FALSE - the message was found invalid and was handled by this function. no 
            further processing is needed.

COMMENTS:
    This function can be called either before or after the app was started.
=============================================================================*/
boolean SSOperationsFSM_CheckIncomingTI(
    CSSOperationsFSM    *pMe,
    uint8                TI,
    SSMsg_msg_type_type  msg_type
    )
{
    boolean IsTIOccupied;

    if (msg_type == SSMSG_MSG_TYPE_SETUP) 
    {
        if (GET_TI_FLAG(TI) == 1)
            // SETUP message with an invalid TI flag.
            // ignore the message.
            return FALSE;

        if (GET_TI_VALUE(TI) == 0x7)
        {
            // SETUP message with a TI value of '111'
            // send RELEASE COMPLETE
            SSOperationsFSM_SendReleaseComplete(pMe, 
                COMPOSE_TI(1, GET_TI_VALUE(TI)), 
                SSMSG_CAUSE_VALUE_INVALID_TRANSACTION_IDENTIFIER_VALUE);
            
            return FALSE;
        }
    }
    else    // not a SETUP message
    {
        if (GET_TI_VALUE(TI) == 0x7)
        {
            // not a SETUP message with a TI value of '111'
            // ignore the message
            return FALSE;
        }
    }

    // now, check the TI against other allocated TI's
    if (pMe->m_AppStarted)
    {
        // check the TI against the CallDatabase.
        SSCall_TInfo_Index_type dummyTInfoIndex;
        IsTIOccupied = SSCallDatabase_GetTInfoIndexByIncomingTI(pMe->m_pCallDatabase, TI, &dummyTInfoIndex);
    }
    else
    {
        SSCall_Origination_type  tmpType;

        IsTIOccupied = FALSE;

        if (SSAgentApp_GetFirstCallDetails(
            pMe->m_pIShell, 
            &tmpType, 
            NULL, 
            NULL,
            NULL,
            NULL) == TRUE)
        {
            // if a first call exist and it is an MT call, then it occupies 
            // TI value 0 (for TI flag 0). in this case, the incoming TI (which
            // also has TI flag 0) is not valid if it has value 0.
            if (tmpType == SSCall_MT)
                IsTIOccupied = (GET_TI_VALUE(TI) == 0);
        }
    }

    if (!IsTIOccupied)    // unallocated TI
    {
        // check for RELEASE COMPLETE message
        if (msg_type == SSMSG_MSG_TYPE_RELEASE_COMPLETE)
            // ignore the message
            return FALSE;
        else if (msg_type != SSMSG_MSG_TYPE_SETUP)
        {
            // not a RELEASE COMPLETE nor SETUP message.
            // send RELEASE COMPLETE
            SSOperationsFSM_SendReleaseComplete(pMe, 
                COMPOSE_TI(1, GET_TI_VALUE(TI)), 
                SSMSG_CAUSE_VALUE_INVALID_TRANSACTION_IDENTIFIER_VALUE);

            return FALSE;
        }
    }
    else // TI is already allocated
    {
        // check for SETUP message
        if (msg_type == SSMSG_MSG_TYPE_SETUP)
            // ignore the message
            return FALSE;
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsFSM_HandleEvent

DESCRIPTION: Entry point for external events to the FSM.

PARAMETERS:
   pMe [in]   : Pointer to the FSM object.           
   eCode [in] : Specifies the Event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsFSM_HandleEvent(
   CSSOperationsFSM *pMe,
   AEEEvent          eCode,
   uint16            wParam,
   uint32            dwParam
)
{
    AEENotify* pNotify;
    AEECLSID clsid;
    uint32 dwMask;
    boolean result;
    SSQueueEvent curEvent;

    switch(eCode)
    {
    case EVT_APP_START:
        pMe->m_AppStarted = TRUE;
        
        // check for a stored GSM1x signaling message
        if (pMe->m_pNotify != NULL)
        {
            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsFSM_HandleEvent: Found pNotify on EVT_APP_START", 0, 0, 0);

            // post it to ourselves
            SS_PostEvent(
               pMe->m_pIShell, 
               AEECLSID_SS_OPERATIONS, 
               EVT_NOTIFY, 
               0, 
               (uint32)(pMe->m_pNotify)
               );
        }

    // fall through

    case EVT_APP_RESUME:
        return TRUE;

    case EVT_APP_STOP:
    case EVT_APP_SUSPEND:
        return TRUE;

    case EVT_NOTIFY:
        // dwParam holds AEENotify struct
        pNotify = (AEENotify*)dwParam;
        clsid = pNotify->cls;
        dwMask = pNotify->dwMask;

        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSOperationsFSM_HandleEvent: Got EVT_NOTIFY", 0, 0, 0);

        switch (clsid)
        {
        case AEECLSID_IGSM1XSIGNOTIFIER:    // incoming GSM1x signaling message
            {
                SSEventData* eventData;
                AEEGSM1xSig_NotifyMessageType* incomingNotification;
                AEEGSM1xSig_SignalingMessageType* incomingMsg;
                SSMsg_int_msg_type* int_msg;        // allocated here and should be freed
                                                    // by the handler
                uint8 TI;
                SSUtil_error_type result;

                GSM1X_LOG_HIGH(
                    pMe->m_pSSUtilCls, 
                    "SSO: Got IGSM1xSigNotifier notification. dwMask=%d AppStarted=%d", 
                    dwMask,
                    pMe->m_AppStarted,
                    0);
                
                // check the notification mask
                if (GET_NOTIFIER_MASK(dwMask) != NMASK_GSM1xSIG_PROTOCOL_TYPE)
                    return FALSE;
                if (GET_NOTIFIER_VAL(dwMask) != GSM1xSIG_PTYPE_RIL3)
                    return FALSE;
                
                incomingNotification = (AEEGSM1xSig_NotifyMessageType*)(pNotify->pData);
                GSM1X_LOG_HIGH(
                    pMe->m_pSSUtilCls, 
                    "SSO: RIL3 msg details: msg type=%d", 
                    incomingNotification->messageType, 0, 0);
                
                if (incomingNotification->messageType != GSM1xSIG_SIGNALING)
                {
                    // GSM1xSIG_REJECT notifications are currently not 
                    // supported
                    return FALSE;
                }
                
                incomingMsg = &incomingNotification->msg.signalMessage;
                
                GSM1X_LOG_HIGH(
                    pMe->m_pSSUtilCls, 
                    "SSO: RIL3 msg details: rev=%d ptype=%d", 
                    incomingMsg->ProtocolRevision,
                    incomingMsg->ProtocolType, 0);
                
                // check protocol revision. currently only rev 0 is supported.
                if (incomingMsg->ProtocolRevision != 0)
                    return FALSE;
                
                // check protocol type. only RIL3 is applicable.
                // this should be an assert because we already checked the mask
                // for the correct protocol type.
                if (incomingMsg->ProtocolType != GSM1xSIG_PTYPE_RIL3)
                    return FALSE;
                
                // translate the incoming message
                if ((int_msg = MALLOC(sizeof(SSMsg_int_msg_type))) == NULL)
                {
                    assert(!"MALLOC failed");
                }

                if ((result = ISSUTILCLS_Translate_ext_to_int(
                                pMe->m_pSSUtilCls, 
                                incomingMsg->ProtocolDataLen, 
                                incomingMsg->ProtocolData,
                                int_msg)) != SSUTIL_SUCCESS)
                {
                    // failed to translate message. action is TBD
                    GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Translate_ext_to_int failed. result=%d", result, 0, 0);
                    
                    FREE(int_msg);
                    return TRUE;
                }

                TI = COMPOSE_TI(int_msg->gen_hdr.tiflag, int_msg->gen_hdr.tivalue);
                GSM1X_LOG_HIGH(
                    pMe->m_pSSUtilCls, 
                    "SSO: RIL3 msg details: pd=%d ti=%d msg_type=%d", 
                    int_msg->gen_hdr.pd,
                    TI,
                    int_msg->gen_hdr.msg_type);

                // accept only call dependent pd
                if (int_msg->gen_hdr.pd != SSMSG_PD_CC)
                {
                    FREE(int_msg);
                    return FALSE;
                }

                if (!pMe->m_AppStarted)
                {
                    if (SSOperationsFSM_CheckIncomingTI(
                        pMe, 
                        TI,
                        int_msg->gen_hdr.msg_type) == FALSE)
                    {
                        // message contains an invalid TI
                        GSM1X_LOG_HIGH(
                            pMe->m_pSSUtilCls, 
                            "SSO: DI with invalid TI=%d. msg type=%d", 
                            TI, 
                            int_msg->gen_hdr.msg_type, 
                            0);
                        FREE(int_msg);
                        return TRUE;
                    }

                    if (int_msg->gen_hdr.msg_type == SSMSG_MSG_TYPE_SETUP)
                    {
                         SSCall_Origination_type tmpType;

                         if (FALSE == SSAgentApp_GetFirstCallDetails(
                                          pMe->m_pIShell, 
                                          &tmpType, 
                                          NULL, 
                                          NULL,
                                          NULL,
                                          NULL
                                          ))
                         {
                             // if first call details are not available, then an error occurred,
                             // ignore the setup request ...
                             GSM1X_LOG_HIGH(
                                 pMe->m_pSSUtilCls, 
                                 "SSO: Setup Msg but no 1st Call Details!", 
                                 0, 0, 0
                                 );
                             FREE(int_msg);
                             return TRUE;
                         }
                    }
                    
                    // while we are in the background, we only care about 
                    // specific messages
                    switch (int_msg->gen_hdr.msg_type)
                    {
                    case SSMSG_MSG_TYPE_SETUP:
                        // a SETUP message means an incoming waiting call.
                        // we store the message in pMe->m_pNotify and then 
                        // StartApplet ourselves.When EVT_APP_START is received
                        // by the SSO, if something is stored in pMe->m_pNotify
                        // we will again, post it to ourselves. This time we will
                        // be in the foreground and handle the SETUP message.
                        {
                            AEEGSM1xSig_NotifyMessageType* tmpData;
                            
                            // MALLOC only for the first time
                            if (pMe->m_pNotify == NULL)
                            {
                                pMe->m_pNotify = MALLOC(sizeof(AEENotify));
                                pMe->m_pNotify->pData = MALLOC(sizeof(AEEGSM1xSig_NotifyMessageType));
                            }
                            
                            // store the message in pMe->m_pNotify
                            tmpData = pMe->m_pNotify->pData; // store pointer
                            MEMCPY(pMe->m_pNotify, pNotify, sizeof(AEENotify));
                            pMe->m_pNotify->pData = tmpData; // restore overwritten pointer
                            MEMCPY(pMe->m_pNotify->pData, pNotify->pData, sizeof(AEEGSM1xSig_NotifyMessageType));
                            
                            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Got SETUP message while not started", 0, 0, 0);
                            
                            // start ourselves with a special argument string that denotes 
                            // a start because of an incoming call. note that the dialog handler checks 
                            // for this string argument.
                            ISHELL_StartAppletArgs(pMe->m_pIShell, AEECLSID_SS_OPERATIONS, "#1");
                        }
                        break;

                    case SSMSG_MSG_TYPE_FACILITY:
                        // a FACILITY message means a notification from the 
                        // network. We use the SSPromptApp to display it.
                        {
                            AECHAR msg[SS_PROMPT_MAX_MSG_LENGTH];
                            SSMsg_component_invoke_type* invoke_comp;
                            int16 resId = 0;

                            GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Got FACILITY message while not started", 0, 0, 0);
                            // lets understand the FACILITY
                            if (int_msg->facility.facility.hdr.present == FALSE)
                                // no facility ie
                                break;
                            if ((int_msg->facility.facility.component.gen_hdr.present == FALSE) ||
                                (int_msg->facility.facility.component.gen_hdr.component_type != SSMSG_COMPONENT_TYPE_INVOKE))
                                // we only care about invoke component
                                break;

                            invoke_comp = &int_msg->facility.facility.component.invoke;
                            if (invoke_comp->call_on_hold_indicator.hdr.present == FALSE)
                                // we only care about call hold notifications
                                break;

                            switch(invoke_comp->call_on_hold_indicator.value)
                            {
                            case SSMSG_HOLD_INDICATOR_CALL_RETRIEVED:
                                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Got call retrieved notification", 0, 0, 0);

                                resId = IDS_UIMN_FACILITY_IND_CALL_RETRIEVED;
                                break;
                            case SSMSG_HOLD_INDICATOR_CALL_ON_HOLD:
                                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Got call on hold notification", 0, 0, 0);
                                
                                resId = IDS_UIMN_FACILITY_IND_CALL_ON_HOLD;
                                break;
                            default:
                                ASSERT_NOT_REACHABLE;
                            }

                            if (ISHELL_LoadResString(
                                pMe->m_pIShell,
                                SSOPERATIONS_RES_FILE,
                                resId,
                                msg,
                                sizeof(msg)) == 0)
                            {
                                GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Failed to load resource string. resId=%d", resId, 0, 0);
                            }
                            else
                            {
                                // display the notification on the UI
                                ISHELL_SendEvent(
                                    pMe->m_pIShell,
                                    AEECLSID_SS_PROMPTAPP,
                                    EVT_SS_PROMPT_UPDATE,
                                    SSO_NOTIFICATIONS_PROMPT_TIMEOUT,
                                    (uint32)msg);
                            }
                        }
                        break;
                    }

                    FREE(int_msg);
                    // applet is not started. no further processing.
                    return TRUE;
                }
                else
                {
                    // applet is already started.
                    // free any stored message
                    if (pMe->m_pNotify != NULL)
                    {
                        GSM1X_LOG_HIGH(pMe->m_pSSUtilCls, "SSO: Freeing pNotify", 0, 0, 0);
                        FREE(pMe->m_pNotify->pData);
                        FREE(pMe->m_pNotify);
                        pMe->m_pNotify = NULL;
                    }
                }

                // prepare a data indication event to be routed to the sub SM.
                eventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_DATA_IND);
                eventData->dataIndData.msg = int_msg;
                
                eCode = MNCC_DATA_INDICATION;
                wParam = 0;
                dwParam = (uint32)eventData;
            }
            break;
        }
        break;
    }

    // route the event to the sub SM
    // first send it to the MN. If the MN did not handle it, send it to the 
    // CC Manager.
    if (!SSOperationsMN_HandleEvent(pMe->m_pMN, eCode, wParam, dwParam) &&
        !SSOperationsCC_HandleEvent(pMe->m_pCC, eCode, wParam, dwParam))
        // if the event wasn't handled by the MN nor the CC, we can return 
        // without checking the event queue
        return FALSE;

    // process events from the queue
    while (SSQueue_GetFirst(pMe->m_pEventQueue, &curEvent) == TRUE)
    {
        // first send it to the MN. If the MN did not handle it, send it to the 
        // CC Manager.
        result = SSOperationsMN_HandleEvent(pMe->m_pMN, curEvent.evt, curEvent.wParam, curEvent.dwParam);
        if (result == FALSE)
            result = SSOperationsCC_HandleEvent(pMe->m_pCC, curEvent.evt, curEvent.wParam, curEvent.dwParam);

        // if the event wasn't handled by the MN nor the CC, we need to free
        // some memory for specific events
        if (result == FALSE)
        {
           if ( (curEvent.evt > EVT_SSO_PRIMITIVES_FIRST) && 
                (curEvent.evt < EVT_SSO_PRIMITIVES_LAST)
              )
           {
              SSOEvent_FreeEventData( (SSEventData*)(curEvent.dwParam) );
           }

           // some specific events are expected to occasionally not being 
           // handled by the sub SM. for these known events, we do nothing.
           // for other events, we do an assert so we can catch them.
           switch (curEvent.evt)
           {
               case MNCC_HOLD_REQ:
               case MNCC_RETRIEVE_REQ:
               case MNCC_DISC_REQ:
                  // do nothing ...
                  break;

               default:
                  assert(!"Unexpected Event");
           }
        }
    }
    return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsFSM_QueueEvent

DESCRIPTION: Queues an event onto the MN-CC queue. may be called from the
             MN or from the CC.

PARAMETERS:
   pMe   [in] : Pointer to the FSM object.
   eCode [in] : Specifies the Event to queue.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None
=============================================================================*/
void SSOperationsFSM_QueueEvent(
   CSSOperationsFSM *pMe,
   AEEEvent          eCode,
   uint16            wParam,
   uint32            dwParam
   )
{
    SSQueueEvent event;

    event.evt = eCode;
    event.wParam = wParam;
    event.dwParam = dwParam;

    if (SSQueue_Put(pMe->m_pEventQueue, &event) == FALSE)
        assert (!"SSQueue is full");
}

