/*=============================================================================
FILE: SSOperations_DialogHandler.c

SERVICES: GSM1x SS Operations Dialog Handlers

GENERAL DESCRIPTION:
      This file defines the SS Operations dialog component, which is responsible
      for handling the various User interface Dialog screen.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===============================================================================
                        INCLUDE FILES
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEETAPI.h"

#include "SSOperations_DialogHandler.h"
#include "SSOperations_DialogPopupHandler.h"
#include "SS_CommonMacros.h"
#include "SSOperations_DialogCommon.h"
#include "SSOperations_UserEvents.h"
#include "SSAgentApp.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/
#define TESTING

/*===========================================================================
                    MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                    TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                    FUNCTION DECLARATIONS
===========================================================================*/

static boolean SSOperationsDialogHandler_RouteDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static boolean SSOperationsDialogHandler_IsPhoneStateValid(
   CSSOperationsDialogHandler *pSelf
   );

static boolean SSOperationsDialogHandler_IsSSOPrimitiveEvent(
    AEEEvent eCode
    );

static boolean SSOperationsDialogHandler_IsTimerEvent(
    AEEEvent eCode
    );

static boolean SSOperationsDialogHandler_RouteDisplayDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static boolean SSOperationsDialogHandler_RouteNonDisplayDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

static void SSOperationsDialogHandler_ShowAppletDialog(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialogHandler_ProcessAppStartData(
    CSSOperationsDialogHandler *pSelf,
    AEEAppStart                *appStartData
    );

static boolean SSOperationsDialogHandler_LoadInstances(
    CSSOperationsDialogHandler *pSelf
    );

static void SSOperationsDialogHandler_InitMembers(
    CSSOperationsDialogHandler *pSelf
    );

static void SoundCallbackFunction( 
    void           *pUserData, 
    AEESoundCmd     eCBType, 
    AEESoundStatus  eStatus, 
    uint32          dwParam 
    );

/*#ifdef AEE_SIMULATOR
    static void SSOperationsDialogHandler_AddDummyCall1(
        CSSOperationsDialogHandler *pSelf
        );

    static void SSOperationsDialogHandler_AddDummyCall2(
        CSSOperationsDialogHandler *pSelf
        );
#else
*/
    static boolean SSOperationsDialogHandler_AddFirstCall(
        CSSOperationsDialogHandler *pSelf
        );
//#endif


/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleEvent

DESCRIPTION: handle a BREW event by the dialog handler

PARAMETERS:
   pSelf [in]: Pointer to the Applet structure.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SSOperationsDialogHandler_HandleEvent(
   CSSOperationsDialogHandler *pSelf,
   AEEEvent                    eCode,
   uint16                      wParam,
   uint32                      dwParam
)
{
   switch (eCode) 
   {
   case EVT_DIALOG_INIT:
      pSelf->m_DialogID = wParam;
      pSelf->m_pDialog  = (IDialog*)dwParam;
      return SSOperationsDialogHandler_RouteDialogEvent(pSelf, eCode, wParam, dwParam);

   case EVT_DIALOG_START:
      return SSOperationsDialogHandler_RouteDialogEvent(pSelf, eCode, wParam, dwParam);
    
   case EVT_DIALOG_END:
      if (0 == wParam) return TRUE;
      assert(wParam == pSelf->m_DialogID);

      (void) SSOperationsDialogHandler_RouteDialogEvent(pSelf, eCode, wParam, dwParam);
      pSelf->m_DialogID = 0;
      pSelf->m_pDialog = NULL;

      // invalidate the dialog's display handler
      pSelf->m_pDisplayDialogEventHandler = NULL;
        
      // Don't show dialog if we are suspending (EVT_APP_SUSPEND is sent before EVT_DIALOG_END).  
      // Wait until we resume...
      if (FALSE == pSelf->m_bSuspended)
      {
         SSOperationsDialogHandler_ShowAppletDialog(pSelf);
      }
      return TRUE;

   case EVT_APP_START:
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:Started!", 0, 0, 0);
      SSOperationsDialogHandler_ProcessAppStartData(pSelf, (AEEAppStart*)dwParam);
      SSOperationsDialogHandler_ShowAppletDialog(pSelf);
      return TRUE;

   case EVT_APP_STOP:
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:Stopped!", 0, 0, 0);
      // allow current event handler to handle shutdown - mainly end the call ...
      // routing this event gets the phone 'stuck' ...

      // The following line is a hack. It seems that at least in the emulator 
      // after EVT_APP_STOP, an EVT_DIALOG_END event is received! this hack
      // makes sure that a new dialog will not be opened in the meantime.
      pSelf->m_bSuspended = TRUE;

      // active dialog will be closed 'auto-magically'
      return TRUE;

   case EVT_APP_SUSPEND:
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:Suspended!", 0, 0, 0);
      pSelf->m_bSuspended = TRUE;
      return TRUE;

   case EVT_APP_RESUME:
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:Resumed!", 0, 0, 0);
      // During the suspend something may have drawn to the screen, so
      // redraw based on what state we were in last
      pSelf->m_bSuspended = FALSE;
      SSOperationsDialogHandler_ShowAppletDialog(pSelf);
      return TRUE;
   }

   // Route the received event to the current active dialog handler.
   return SSOperationsDialogHandler_RouteDialogEvent(pSelf, eCode, wParam, dwParam);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_New

DESCRIPTION: Create a dialog handler instance

PARAMETERS:
   pIShell [in]: Pointer to the IShell instance 
   pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
   Pointer to CSSOperationsDialogHandler.

=============================================================================*/
CSSOperationsDialogHandler* SSOperationsDialogHandler_New(
   IShell         *pIShell,
   SSCallDatabase *pCallDatabase,
   ISSUtilCls     *pSSUtilCls
   )
{
   CSSOperationsDialogHandler* obj;

   obj = MALLOC(sizeof(CSSOperationsDialogHandler));
   if (obj == NULL)
     return NULL;

   obj->m_pCallDatabase = pCallDatabase;
   obj->m_pShell = pIShell;
   ISHELL_AddRef(pIShell);

   obj->m_pSSUtilCls = pSSUtilCls;
   ISSUTILCLS_AddRef(pSSUtilCls);

   return obj;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_Initialize

DESCRIPTION: Initialize the dialog handler instance

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure

RETURN VALUE:
   TRUE if initialization succeeded, FALSE otherwise.

COMMENTS:
    *** IMPORTANT ***
    This function is only called upon EVT_APP_START.
    Until then, most of the data members in CSSOperationsDialogHandler
    are just set to NULL.

=============================================================================*/
boolean SSOperationsDialogHandler_Initialize(
   CSSOperationsDialogHandler *pSelf
   )
{
   boolean        success;
   AEESoundInfo   soundInfo;

   SSOperationsDialogHandler_InitMembers(pSelf);

   success = SSOperationsDialogHandler_LoadInstances(pSelf);
   if (!success)
   {
      GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO:DialogHandler_LoadInstances Failed!",0,0,0);
      return FALSE;
   }

   if (SSOperationsDialogHandler_IsPhoneStateValid(pSelf))
   {
/*
#ifdef AEE_SIMULATOR
      SSOperationsDialogHandler_AddDummyCall1(pSelf);
      //SSOperationsDialogHandler_AddDummyCall2(pSelf);
#else
*/
      success = SSOperationsDialogHandler_AddFirstCall(pSelf);
      if (!success) return FALSE;
//#endif
   }
   else
   {
      pSelf->m_pDialogSetupHandler = SSOperationsDialog_SetupInvalidPhoneStateDialog;
      return TRUE;
   }

   pSelf->m_pDialogSetupHandler = SSOperationsDialog_SetupEntryMenuDialog;
    
   ////////////////////////////
   // initialize sound device: 
   ////////////////////////////
   (void) ISOUND_Get(pSelf->m_pSound, &soundInfo);

   soundInfo.eDevice = AEE_SOUND_DEVICE_CURRENT;
   //soundInfo.eMethod = AEE_SOUND_METHOD_RING;
   //soundInfo.eAPath = AEE_SOUND_APATH_LOCAL;

   soundInfo.eMethod = AEE_SOUND_METHOD_VOICE;
   soundInfo.eAPath = AEE_SOUND_APATH_BOTH;
   soundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
   //soundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

   // ISOUND_Set: sets the device attributes of ISound interface 
   // object. The device attributes include Device, Method, APath, 
   // Ear-piece Mute Control and Mic Mute Control.
   ISOUND_Set(pSelf->m_pSound, &soundInfo);

   // The audio device is not connected at this point.
   // A call the ISOUND_SetDevice function will connect the
   // device. 
   //
   // ISOUND_SetDevice: commands ISound to connect to a specific audio 
   // device. There is always one, and only one, audio device 
   // connected at any particular time.
   ISOUND_SetDevice(pSelf->m_pSound);

   ISOUND_RegisterNotify(pSelf->m_pSound, &SoundCallbackFunction, (void *)pSelf);

   // ISOUND_GetVolume: This function retrieves the current volume 
   // used for the device/method pair.
   // Note: The volume level will be sent back to the client via 
   //       the callback function pointer. The result includes 
   //       the status as well as the current volume level.
   ISOUND_GetVolume (pSelf->m_pSound);

   return TRUE;
}

/*=============================================================================
FUNCTION:SSOperationsDialogHandler_Release

DESCRIPTION: Release a dialog handler instance

PARAMETERS:
    dialogData [in]: Pointer to a valid dialog data instance

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_Release(
   CSSOperationsDialogHandler* pSelf
   )
{
    // release instances
    if (pSelf->m_pPopupIStatic != NULL)
    {
        (void) ISTATIC_Release(pSelf->m_pPopupIStatic);
        pSelf->m_pPopupIStatic = NULL;
    }

    if (pSelf->m_pNumberScroller != NULL)
    {
        IStaticScroller_Destroy(pSelf->m_pNumberScroller);
        pSelf->m_pNumberScroller = NULL;
    }

    if (pSelf->m_pNumberCtl != NULL)
    {
        (void) ISTATIC_Release(pSelf->m_pNumberCtl);
        pSelf->m_pNumberCtl = NULL;
    }

    if (pSelf->m_pNameScroller != NULL)
    {
        IStaticScroller_Destroy(pSelf->m_pNameScroller);
        pSelf->m_pNameScroller = NULL;
    }

    if (pSelf->m_pNameCtl != NULL)
    {
        (void) ISTATIC_Release(pSelf->m_pNameCtl);
        pSelf->m_pNameCtl = NULL;
    }

    if (pSelf->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pSelf->m_pDisplay);
        pSelf->m_pDisplay = NULL;
    }

    if (pSelf->m_pSSUtilCls != NULL)
    {
        ISSUTILCLS_Release(pSelf->m_pSSUtilCls);
        pSelf->m_pSSUtilCls = NULL;
    }

    if (pSelf->m_pSound != NULL)
    {
        ISOUND_Release(pSelf->m_pSound);
        pSelf->m_pSound = NULL;
    }

    if (pSelf->m_pAddressBook != NULL)
    {
        IADDRBOOK_Release(pSelf->m_pAddressBook);
        pSelf->m_pAddressBook = NULL;
    }

    if (pSelf->m_pPhone != NULL) 
    {
        IPHONE_Release(pSelf->m_pPhone);
        pSelf->m_pPhone = NULL;
    }

    if (pSelf->m_pShell != NULL)
    {
        ISHELL_Release(pSelf->m_pShell);
        pSelf->m_pShell = NULL;
    }

    FREE(pSelf);
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_IsPhoneStateValid

DESCRIPTION: Check if the phone state is valid for the SSO applet

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure

RETURN VALUE:
   TRUE if phone state valid, FALSE otherwise.

=============================================================================*/
boolean SSOperationsDialogHandler_IsPhoneStateValid(
   CSSOperationsDialogHandler *pSelf
   )
{
//#ifndef AEE_SIMULATOR
    boolean callTypeAllowed;
	#ifdef FEATURE_MULTIMODE_ARCH
		const cm_mm_call_info_s_type* ci = IPHONE_GetCallInfo(pSelf->m_pPhone);
	#else
		const cm_call_info_s_type* ci = IPHONE_GetCallInfo(pSelf->m_pPhone);
	#endif

    // no state information - probably idle state, assume applet should not run
    if (ci == NULL) 
    {
		DBGPRINTF("SSOperations_DialogHandler; ci == NULL!!!");
        GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO *probably* not in Conversation!",0,0,0);
        return FALSE;
    }

    // if phone is NOT in conversation don't run applet
    if (ci->call_state != CM_CALL_STATE_CONV)
    {
		DBGPRINTF("SSOperations_DialogHandler; SSO not in conversation!");
        GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO not in Conversation!",0,0,0);
        return FALSE;
    }

    if (SSAgentApp_GetFirstCallDetails(
           pSelf->m_pShell,
           NULL,
           NULL,
           NULL,
           &callTypeAllowed,
           NULL) == FALSE)
    {
		DBGPRINTF("SSOperations_DialogHandler; SSO no first call details!");
        GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO no first call details!",0,0,0);
        return FALSE;
    }

    return callTypeAllowed;

/*
#else
   return TRUE;
#endif
*/
}

/*===========================================================================

FUNCTION: SoundCallbackFunction

DESCRIPTION:
   This is the callback function that is registered with ISOUND_RegisterNotify, 
   in order to receive ISound information. Function prototype is dictated by
   BREW type PFNSOUNDSTATUS.

PARAMETERS:
   eCBType: Type of ISound callback.
   eStatus: Status contained in callback. The status depends on the 
              eCBType.
   pUserData: Our CSSOperationsDialogHandler.
   dwParam: Additional info specific to callback type and status type

RETURN VALUE
  none

SIDE EFFECTS
  updates the 

===========================================================================*/
static void SoundCallbackFunction( 
    void               *pUserData, 
    AEESoundCmd         eCBType, 
    AEESoundStatus      eStatus, 
    uint32              dwParam 
    )
{
   CSSOperationsDialogHandler *pSelf = (CSSOperationsDialogHandler*)pUserData;
   AEESoundCmdData            *pData = (AEESoundCmdData*) dwParam;

   assert(pSelf != NULL);

   switch (eStatus)
   {
      case AEE_SOUND_SUCCESS:
      {
         // callback was invoked as a result of the GetVolume function
         if (eCBType == AEE_SOUND_VOLUME_CB)
         {
            uint32 normalizedVolume;

            assert(pData != NULL);            

            normalizedVolume = (pData->wVolume*SS_MAX_VOLUME)/AEE_MAX_VOLUME;
            assert((int32)normalizedVolume >= SS_1ST_VOLUME);
            assert(normalizedVolume <= SS_MAX_VOLUME);

            pSelf->m_CallVolume = (SS_CallVolume) normalizedVolume;
            GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:SoundCallbackFunction Vol=%d",normalizedVolume,0,0);

            // Deregister the notify function. Do this first so that
            // the subsequent ISOUND operations will not queue any
            // callbacks.
            ISOUND_RegisterNotify(pSelf->m_pSound, NULL, NULL);
         }
      }
      break;

      case AEE_SOUND_FAILURE:
        {
            GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:SoundCallbackFunction Failure",0,0,0);

            // callback was invoked as a result of the GetVolume function
            if (eCBType == AEE_SOUND_VOLUME_CB)
            {
               pSelf->m_CallVolume = 0;
            }
        }
      break;

      default:
         assert(!"Unknown ISOUND status");
         break;
   }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_AddDummyCall1

DESCRIPTION: 
    add a dummy call to the call database.
    For testing in the simulator.

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure.           

RETURN VALUE:
   None

=============================================================================*/
/*#ifdef AEE_SIMULATOR

static void SSOperationsDialogHandler_AddDummyCall1(
    CSSOperationsDialogHandler *pSelf
    )
{
    SSCall_TInfo_type tempCallTInfo;
    boolean           result;

    tempCallTInfo.callStartTime = ISHELL_GetUpTimeMS(pSelf->m_pShell);
    tempCallTInfo.callOrigType = SSCall_MT;
    tempCallTInfo.callStringPI = SS_PI_ALLOWED;

    STRTOWSTR(
        "123456789123456789123456789", 
        tempCallTInfo.callString, 
        sizeof(tempCallTInfo.callString)
        );

    STRTOWSTR(
        "",
        //"DummyContact1 with a LONG name!",
        tempCallTInfo.callName, 
        sizeof(tempCallTInfo.callName)
        );
    
    if (tempCallTInfo.callOrigType == SSCall_MO)
    {
        result = SSCallDatabase_AllocateMoTi(pSelf->m_pCallDatabase, 
                                             &tempCallTInfo.TIValue);
        assert(result == TRUE);
    }
    else
    {
        tempCallTInfo.TIValue = 0;
    }

    tempCallTInfo.callStatus = SS_CALL_STATUS_ACTIVE;
    tempCallTInfo.callState.mainState = SSCallState_ACTIVE;
    tempCallTInfo.callState.holdState = SSCallHoldState_IDLE;
    tempCallTInfo.callState.mptyState = SSCallMPTYState_IDLE;
    
    result = SSCallDatabase_AddCall(
                pSelf->m_pCallDatabase,
                &tempCallTInfo,
                &pSelf->m_IncomingDialogCallIndex,
                NULL
                );
    assert(result == TRUE);
}

#endif // AEE_SIMULATOR
*/

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_AddDummyCall2

DESCRIPTION: add a dummy call to the call database

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure.           

RETURN VALUE:
   None

=============================================================================*/

/*#ifdef AEE_SIMULATOR

static void SSOperationsDialogHandler_AddDummyCall2(
    CSSOperationsDialogHandler *pSelf
    )
{
    SSCall_TInfo_type tempCallTInfo;
    SSCall_Info_type *pAllocatedCallInfo;
    boolean           result;

    tempCallTInfo.callStartTime = ISHELL_GetUpTimeMS(pSelf->m_pShell) - 1000;
    tempCallTInfo.callOrigType = SSCall_MT;
    tempCallTInfo.callStringPI = SS_PI_RESTRICTED;
    tempCallTInfo.TIValue = 1;
    STRTOWSTR("555-555-555", tempCallTInfo.callString, sizeof(tempCallTInfo.callString));
    STRTOWSTR("DummyContact2", tempCallTInfo.callName, sizeof(tempCallTInfo.callName));

    tempCallTInfo.callStatus = 0xff; // init to a known dummy value
    tempCallTInfo.callState.mainState = SSCallState_ACTIVE;
    tempCallTInfo.callState.holdState = SSCallHoldState_CALL_HELD;
    tempCallTInfo.callState.mptyState = SSCallMPTYState_IDLE;

    result = SSCallDatabase_AddCall(
                pSelf->m_pCallDatabase,
                &tempCallTInfo,
                &pSelf->m_IncomingDialogCallIndex,
                NULL
                );
    assert(result == TRUE);

    result= SSCallDatabase_GetCallInfo(
        pSelf->m_pCallDatabase,
        pSelf->m_IncomingDialogCallIndex,
        &pAllocatedCallInfo
        );
    assert(result == TRUE);

    pAllocatedCallInfo->isHeld = TRUE;
}
#endif  // AEE_SIMULATOR
*/

//#ifndef AEE_SIMULATOR
/*=============================================================================
FUNCTION: SSOperationsDialogHandler_AddFirstCall

DESCRIPTION: add the idle call data

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure.           

RETURN VALUE:
   None

=============================================================================*/
static boolean SSOperationsDialogHandler_AddFirstCall(
    CSSOperationsDialogHandler *pSelf
    )
{
    SSCall_TInfo_type        tempCallTInfo;
    boolean                  result;
    SSCall_Origination_type  tmpOrigType;
    SSCall_PI_type           tmpPi;
    const char              *pTmpCallString;
    boolean                  tmpCallTypeAllowed;
    uint32                   tmpStartTime;

    GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:E:DialogHandler_AddFirstCall", 0, 0, 0);

    result = SSAgentApp_GetFirstCallDetails(
                pSelf->m_pShell, 
                &tmpOrigType, 
                &tmpPi, 
                &pTmpCallString,
                &tmpCallTypeAllowed,
                &tmpStartTime
                );
    assert (result == TRUE);

    GSM1X_LOG_HIGH(
       pSelf->m_pSSUtilCls,
       "AddFirstCall:type=%d pi=%d string=%s",
       tmpOrigType,
       tmpPi,
       (const uint32)pTmpCallString
       );

    tempCallTInfo.callStartTime = tmpStartTime;
    tempCallTInfo.callOrigType = tmpOrigType;
    tempCallTInfo.callStringPI = tmpPi;

    STRTOWSTR(
       pTmpCallString, 
       tempCallTInfo.callString, 
       sizeof(tempCallTInfo.callString)
       );

    SSOperationsDialog_ProcessCallNumberAndName(
        pSelf,
        &tempCallTInfo
        );
    
    if (tempCallTInfo.callOrigType == SSCall_MO)
    {
        result = SSCallDatabase_AllocateMoTi(
                    pSelf->m_pCallDatabase, 
                    &tempCallTInfo.TIValue
                    );
        assert (result == TRUE);
    }
    else
    {
        tempCallTInfo.TIValue = 0;
    }
    
    tempCallTInfo.callStatus = SS_CALL_STATUS_ACTIVE;
    tempCallTInfo.callState.mainState = SSCallState_ACTIVE;
    tempCallTInfo.callState.holdState = SSCallHoldState_IDLE;
    tempCallTInfo.callState.mptyState = SSCallMPTYState_IDLE;
    
    result = SSCallDatabase_AddCall(
                pSelf->m_pCallDatabase,
                &tempCallTInfo,
                &pSelf->m_IncomingDialogCallIndex,
                NULL
                );
    assert (result == TRUE);

    GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:X:DialogHandler_AddFirstCall", 0, 0, 0);
    return TRUE;
}
//#endif // !AEE_SIMULATOR

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_InitMembers

DESCRIPTION: initialize dialog handler member data

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure.           

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialogHandler_InitMembers(
    CSSOperationsDialogHandler *pSelf
    )
{
   pSelf->m_bSuspended = FALSE;

   pSelf->m_pDisplayDialogEventHandler = NULL;
   pSelf->m_pNonDisplayEventHandler = NULL;
   pSelf->m_pDialogSetupHandler = NULL;

   pSelf->m_pVolumeDialogPrevSetupHandler = NULL;
   pSelf->m_pAboutDialogPrevSetupHandler = NULL;
   pSelf->m_pIncomingDialogPrevSetupHandler = NULL;
   pSelf->m_pEndingCallDialogPrevSetupHandler = NULL;

   pSelf->m_pPopupIStatic = NULL;
   pSelf->m_PopupStatus = SS_NO_POPUP;
   pSelf->m_OverwritePopupDialog = FALSE;
   pSelf->m_pPopupNonDisplayEventHandler = NULL;
   pSelf->m_pPopupText = NULL;

   pSelf->m_PopupGeneralDataValid = FALSE;
   pSelf->m_PopupGeneralText[0] = '\0';

   pSelf->m_Popup1DataValid = FALSE;
   pSelf->m_Popup1Text[0] = '\0';
   pSelf->m_Popup1CallIndex = 0;

   pSelf->m_Popup2DataValid = FALSE;
   pSelf->m_Popup2Text[0] = '\0';
   pSelf->m_Popup1CallIndex = 0;

   pSelf->m_DialogID = 0;
   pSelf->m_pDialog = NULL;

   pSelf->m_MPTYCallString[0] = (AECHAR) '\0';
   pSelf->m_CallVolume = SS_1ST_VOLUME;

   pSelf->m_DialString[0] = '\0';

}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_LoadInstances

DESCRIPTION: load dialog handler's member data instances

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure.           

COMMENTS:
    This function should only be called during initialization.
    If it returns FALSE, memory should be cleaned up.

RETURN VALUE:
    TRUE on success, FALSE on failure.

=============================================================================*/
static boolean SSOperationsDialogHandler_LoadInstances(
    CSSOperationsDialogHandler *pSelf
    )
{
    pSelf->m_pSound = NULL;
    pSelf->m_pAddressBook = NULL;
    pSelf->m_pNumberCtl = NULL;
    pSelf->m_pNameCtl = NULL;
    pSelf->m_pPhone = NULL;

    // Create Sound instance
    if (AEE_SUCCESS != ISHELL_CreateInstance(pSelf->m_pShell,
                                             AEECLSID_SOUND,
                                             (void **)&pSelf->m_pSound))
    {
        return FALSE;
    }


    // Create Address Book instance
    if (AEE_SUCCESS != ISHELL_CreateInstance(pSelf->m_pShell,
                                             AEECLSID_ADDRBOOK,
                                             (void **)&pSelf->m_pAddressBook))
    {
        return FALSE;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pSelf->m_pShell,
                                             AEECLSID_STATIC,
                                             (void**)&pSelf->m_pNumberCtl))
    {
        return FALSE;
    }

    pSelf->m_pNumberScroller = IStaticScroller_New(pSelf->m_pShell);
    if (pSelf->m_pNumberScroller == NULL)
    {
        return FALSE;
    }

    if (AEE_SUCCESS != ISHELL_CreateInstance(pSelf->m_pShell,
                                             AEECLSID_STATIC,
                                             (void**)&pSelf->m_pNameCtl))
    {
        return FALSE;
    }

    pSelf->m_pNameScroller = IStaticScroller_New(pSelf->m_pShell);
    if (pSelf->m_pNumberScroller == NULL)
    {
        return FALSE;
    }

    // Create IPhone instance
    if (AEE_SUCCESS != ISHELL_CreateInstance(pSelf->m_pShell,
                                             AEECLSID_PHONE,
                                             (void **)&pSelf->m_pPhone))
    {
        return FALSE;
    }

   return TRUE;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_RouteDialogEvent

DESCRIPTION: 
    Routes a BREW event to the active dialog's event handler.
    Note that if no handler processes the event, SS Operations
    main event handler will pass it to the SS Operations FSM 
    event handler, and eventually also free memory if needed.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialogHandler_RouteDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    boolean eventHandled = FALSE;

    if (SSOperationsDialogHandler_IsSSOPrimitiveEvent(eCode))
    {
        return SSOperationsDialogHandler_RouteNonDisplayDialogEvent(
                    pSelf, eCode, wParam, dwParam);
    }    

    if (SSOperationsDialogHandler_IsTimerEvent(eCode))
    {    
        eventHandled = SSOperationsDialogHandler_RouteNonDisplayDialogEvent(
                          pSelf, eCode, wParam, dwParam);

        if (FALSE == eventHandled)
        {
            eventHandled = SSOperationsDialogHandler_RouteDisplayDialogEvent(
                                pSelf, eCode, wParam, dwParam);
        }

        return eventHandled;
    }
    else
    {
        return SSOperationsDialogHandler_RouteDisplayDialogEvent(
                    pSelf, eCode, wParam, dwParam);
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_IsSSOPrimitiveEvent

DESCRIPTION: Checks if an event is a displayed dialog event or not

PARAMETERS:
    eCode [in]: BREW event code

RETURN VALUE:
   boolean: TRUE if the event is a displayed dialog event, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialogHandler_IsSSOPrimitiveEvent(
    AEEEvent eCode
    )
{
    if (eCode > EVT_SSO_PRIMITIVES_FIRST && 
        eCode < EVT_SSO_PRIMITIVES_LAST)
    {
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_IsTimerEvent

DESCRIPTION: Checks if an event is a displayed dialog event or not

PARAMETERS:
    eCode [in]: BREW event code

RETURN VALUE:
   boolean: TRUE if the event is a displayed dialog event, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialogHandler_IsTimerEvent(
    AEEEvent eCode
    )
{
    switch (eCode)
    {
        case EVT_USER_DIALOG_TIMEOUT:
        case EVT_POPUP_DIALOG_TIMEOUT:
           return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_RouteDisplayDialogEvent

DESCRIPTION: Routes a BREW event to the active dialog's event handler

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialogHandler_RouteDisplayDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    if (NULL != pSelf->m_pDisplayDialogEventHandler)
    {
        return pSelf->m_pDisplayDialogEventHandler(pSelf, eCode, wParam, dwParam);
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_RouteNonDisplayDialogEvent

DESCRIPTION: Routes a BREW event to the active dialog's event handler

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    eCode [in]: BREW event code
    wParam [in]:
    dwParam [in]:

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialogHandler_RouteNonDisplayDialogEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    boolean eventHandled = FALSE;

    if (NULL != pSelf->m_pNonDisplayEventHandler)
    {
        eventHandled = pSelf->m_pNonDisplayEventHandler(pSelf, eCode, wParam, dwParam);
    }

    // pass event to the pop-up handler
    if (FALSE == eventHandled)
    {
        eventHandled = SSOperationsDialogHandler_HandlePopupEvents(
                          pSelf, 
                          eCode, 
                          wParam, 
                          dwParam
                          );
    }

    return eventHandled;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_ShowAppletDialog

DESCRIPTION: setup and display the applet's dialog
             
PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer

RETURN VALUE:
   None

=============================================================================*/
static void SSOperationsDialogHandler_ShowAppletDialog(
    CSSOperationsDialogHandler *pSelf
    )
{
    switch (pSelf->m_PopupStatus)
    {
        case SS_NO_POPUP:
            assert(pSelf->m_pDialogSetupHandler != NULL);
            pSelf->m_pDialogSetupHandler(pSelf, TRUE);
            break;

        case SS_1ST_POPUP_ACTIVE:
            assert(pSelf->m_Popup1DataValid == TRUE);
            pSelf->m_pPopupText = pSelf->m_Popup1Text;
            SSOperationsDialog_SetupPopupDialog(pSelf);
            break;

        case SS_2ND_POPUP_ACTIVE:
            assert(pSelf->m_Popup2DataValid == TRUE);
            pSelf->m_pPopupText = pSelf->m_Popup2Text;
            SSOperationsDialog_SetupPopupDialog(pSelf);
            break;

        case SS_GENERAL_POPUP_ACTIVE:
            assert(pSelf->m_PopupGeneralDataValid == TRUE);
            pSelf->m_pPopupText = pSelf->m_PopupGeneralText;
            SSOperationsDialog_SetupPopupDialog(pSelf);
            break;

        default:
            assert(!"unknown SS_PopupStatus value");
    }    
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_UpdateScreenData

DESCRIPTION: update the stored screen data using the applet's start data.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pAppStartData [in]: pointer to an AEEAppStart structure

RETURN VALUE:
   pointer to a SSCallDatabase_CallInfo_type.

=============================================================================*/
static void SSOperationsDialogHandler_UpdateScreenData(
    CSSOperationsDialogHandler *pSelf,
    AEEAppStart                *pAppStartData
    )
{
    assert(pAppStartData != NULL);

    if (pSelf->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pSelf->m_pDisplay);
        pSelf->m_pDisplay = NULL;
    }
  
    pSelf->m_pDisplay = pAppStartData->pDisplay;
    (void) IDISPLAY_AddRef(pSelf->m_pDisplay);

    IDISPLAY_ClearScreen(pSelf->m_pDisplay);

    pSelf->m_Rect = pAppStartData->rc;

    // Save text line height for current font
    pSelf->m_LineHeight = IDISPLAY_GetFontMetrics(
                             pSelf->m_pDisplay,
                             AEE_FONT_NORMAL,
                             NULL,
                             NULL
                             );
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_ProcessAppStartData

DESCRIPTION: process the applet's start data information.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   *pAppStartData [in]: pointer to an AEEAppStart structure

RETURN VALUE:
   pointer to a SSCallDatabase_CallInfo_type.

=============================================================================*/
static void SSOperationsDialogHandler_ProcessAppStartData(
    CSSOperationsDialogHandler *pSelf,
    AEEAppStart                *pAppStartData
    )
{
    assert(pAppStartData != NULL);

    SSOperationsDialogHandler_UpdateScreenData(pSelf, pAppStartData);

    if (pAppStartData->pszArgs != NULL)
    {
        if (STRCMP("#1", pAppStartData->pszArgs) == 0)
        {
            pSelf->m_pDialogSetupHandler = 
                SSOperationsDialog_SetupWaitForIncomingCallDialog;
        }
     }

}
