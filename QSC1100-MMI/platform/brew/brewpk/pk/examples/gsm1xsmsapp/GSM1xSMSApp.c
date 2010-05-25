/*===========================================================================
  FILE: SMSApp.c

  SERVICES: SMS Applet

  GENERAL DESCRIPTION:
            Application for handling the SMS messages

  PUBLIC CLASSES AND STATIC FUNCTIONS:
            class SMSApp

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES FOR MODULE
===========================================================================*/

#include "gsm1xsms_res.h"               // Applet Resource Header
#include "GSM1xSMSApp.h"           // Main Applet Header
#include "GSM1xSMSUtil.h"          // Applet Utilities Header
#include "GSM1xSMSMT.h"            // Mobile-Terminated Services Header
#include "GSM1xSMSMessageStore.h"  // Message Storeage Header
#include "AEEGSMSMS.h"             // IGSMSMS Interface Header
#include "AEEDisp.h"
#include "AEETAPI.h"

#include "SMSAgentApp.h"

/*===========================================================================
                           DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/
#define  ANNUN_INUSE        0x0002

#ifndef ANNUN_VMAIL
#define ANNUN_VMAIL ANNUN_NET_MSG
#endif

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

//-----------------------------------------------------------------------------
//                            Public Function Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                            Module Method Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                            Applet Method Declarations
//-----------------------------------------------------------------------------
// Event handling functions
static boolean GSM1xSMSApp_HandleEvent(IApplet *po, AEEEvent  eCode,
                                   uint16  wParam, uint32 dwParam);

//-----------------------------------------------------------------------------
//            Local function definitions (used only by this module)
//-----------------------------------------------------------------------------

// Helper function to update the SMS display annunciators.
static void GSM1xSMSApp_SetSMSDisplayAnnunciators(SMSApp *pMe,
                                              uint16 mask,
                                              uint16 val);

// Various applet event handlers
static boolean GSM1xSMSApp_HandleNotifyEvent(SMSApp *pMe, AEENotify *pN);

// Internal applet method declarations
static int GSM1xSMSApp_InitAppData(IApplet *po);
static void GSM1xSMSApp_FreeAppData(IApplet *po);

#if defined(BREW_STATIC_APP)
static void GSM1xSMSApp_RegisterForEvents(IShell *pMe);
#endif

// Applet main state machine processing method.
static void GSM1xSMSApp_RunFSM(SMSApp *pMe);

// Send message status callback function.
static void GSM1xSMSApp_MsgSendStatusCallBack(void *pUser);

static int GSM1xSMSApp_CreateMessage(SMSApp *pMe, GSMSMSMsg *pMsg, SMSMessageStruct *pMsgIn);

/*===========================================================================
                              GLOBAL DATA
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

#if defined(BREW_STATIC_APP)

static int CSMSAppModCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
static int CSMSAppModLoad(IShell *ps, void * pHelpers, IModule ** pMod);


/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

static const AEECLSID gSMSClasses[] =
{
   0  // Must terminate list with 0
};

static const AEEAppInfo gaiSMSApp[] = {
   {
      AEECLSID_SMS_AGENT,   // Class ID
      0,                 // File name of resource file
      0,                 // String ID of app title
      0,                 // Bitmap ID of app icon 26x26
      0,                 // Bitmap ID of app icon LARGE
      0,                 // String ID of settings name
      0,
      0                  // Applet flags
   },
   {
      AEECLSID_APP_GSM1XSMS,   // Class ID
      GSM1XSMS_RES_FILE, // File name of resource file
      STR_APP_SMS, // Base ID for locating title, icon, etc
      0,
      0,
      0,
      0,
      AFLAG_TOOL                  // Applet flags
   }
};

//----------------------------------------------------------
//   Public function definitions
//----------------------------------------------------------
PFNMODENTRY GSM1xSMSMod_GetModInfo
(
   IShell      *ps,
   AEECLSID   **ppClasses,
   AEEAppInfo **pApps,
   uint16      *pnApps,
   uint16      *pwMinPriv
)
{
   *ppClasses = (AEECLSID*)gSMSClasses;
   *pApps  = (AEEAppInfo *)gaiSMSApp;
   *pnApps = sizeof(gaiSMSApp) / sizeof(gaiSMSApp[0]);
   GSM1xSMSApp_RegisterForEvents(ps);
   return((PFNMODENTRY)CSMSAppModLoad);
}


static int CSMSAppModLoad
(
   IShell   *ps,
   void     *pHelpers,
   IModule **pMod
)
{
   int retval;

   retval = AEEStaticMod_New
      ((int16)(sizeof(AEEMod)),
       ps,
       pHelpers,
       pMod,
       CSMSAppModCreateInstance,
       NULL);
   return retval;
}
#endif  //BREW_STATIC_APP


#if defined(BREW_STATIC_APP)
int CSMSAppModCreateInstance
#else
int AEEClsCreateInstance
#endif
(
   AEECLSID   ClsId,
   IShell    *pIShell,
   IModule   *po,
   void     **ppObj
)
{
   boolean valid;

   if (!po || !pIShell)
   {
      return EFAILED;
   }

   *ppObj = NULL;
   if (ClsId == AEECLSID_APP_GSM1XSMS)
   {
      valid = AEEApplet_New
         (sizeof(SMSApp),
          ClsId,
          pIShell,
          po,
          (IApplet**)ppObj,
          (AEEHANDLER)GSM1xSMSApp_HandleEvent,
          (PFNFREEAPPDATA)GSM1xSMSApp_FreeAppData);

      if (valid)
      {
         GSM1xSMSApp_InitAppData((IApplet*)*ppObj);
         return(AEE_SUCCESS);
      }
   }
   else if (ClsId == AEECLSID_SMS_AGENT)
   {
	   valid = AEEApplet_New
         (sizeof(SMSAgentApp),
          ClsId,
          pIShell,
          po,
          (IApplet**)ppObj,
          (AEEHANDLER)SMSAgentApp_HandleEvent,
          (PFNFREEAPPDATA)SMSAgentApp_FreeAppData);

      if (valid)
      {
         //SMSAgentApp_InitAppData((IApplet*)*ppObj);
		DBGPRINTF("Calling SMSAgentApp_Init from CreateInstance");
		if (SMSAgentApp_InitAppData((IApplet*)*ppObj) != SUCCESS)
		{
			DBGPRINTF("GSM1xSMSApp: SMSAgentApp_InitAppData FAILED!");
      		IAPPLET_Release((IApplet*)*ppObj);
			*ppObj = NULL;
			return EFAILED;
		}
         return(AEE_SUCCESS);
      }
   }
   return(EFAILED);
}


//-----------------------------------------------------------------------------
//                            Applet Method Definitions
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled
    in this function.

PARAMETERS:
    po [in]: Pointer to the SMS Applet structure. This structure contains
             information specific to this applet.

    ecode [in]: Specifies the Event sent to this applet

    wParam, dwParam [in]: Event specific data.

RETURN VALUE:
    TRUE: If the app has processed the event.
    FALSE: If the app did not process the event.

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
static boolean GSM1xSMSApp_HandleEvent(IApplet     *po,
                                   AEEEvent     eCode,
                                   uint16       wParam,
                                   uint32       dwParam)
{
   SMSApp *pMe = (SMSApp*)po;
   AEEAppStart *as;

   switch(eCode)
   {
      case EVT_APP_START:
         ASSERT(dwParam != 0);
         as = (AEEAppStart*)dwParam;

         // Show the busy icon while starting up
         GSM1xSMSUtil_ShowBusyIcon(pMe->a.m_pIShell,
                      pMe->a.m_pIDisplay,
                      &as->rc,
                      FALSE);

         pMe->m_suspending = FALSE;

         pMe->m_rc = as->rc;

         // Reset the FSM based on whether it is a natural start or not.
         // If this is not a natural start then the applet state is already
         // set by the caller and we don't want to overwrite it here.
         if(pMe->m_bNaturalStart)
         {
            pMe->m_currState = STATE_INIT;
         }

         pMe->m_dlgResult = DLGRET_CREATE;

         (void)ISHELL_GetPrefs(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS, SMS_CFG_VERSION,
                    &pMe->m_cfg, sizeof(GSM1xSMSAppConfig));

         GSM1xSMSApp_RunFSM(pMe);  // Start the SMS state machine engine.
         return TRUE;

      case EVT_APP_STOP:
         // Cancel any pending callback
         CALLBACK_Cancel(&pMe->m_IGSMSMSCallback);
         pMe->m_bNaturalStart = TRUE;
         pMe->m_bNeedToMakeVoiceCall = FALSE;
         pMe->m_suspending = TRUE;

         // In case we've somehow missed it, stop any alerts.
         ISHELL_Beep(pMe->a.m_pIShell, BEEP_OFF, FALSE);

         // In case if the SMS applet is in a state where it is still receiving
         // incoming SMS messages, end the call right away.
         if(pMe->m_bReceiving == TRUE)
         {
            (void)IPHONE_EndCall(pMe->m_pIPhone);
         }

         // Save the preference and configuration data.
         GSM1xSMSMessageStore_StorePrefs(pMe);

         // Update the annunciators
         GSM1xSMSApp_UpdateAnnunciators(pMe);

         return TRUE;

      case EVT_APP_SUSPEND:
         pMe->m_suspending = TRUE;
         return TRUE;

      case EVT_APP_RESUME:
         ASSERT(dwParam != 0);
         as = (AEEAppStart*)dwParam;

         pMe->m_suspending = FALSE;

         pMe->m_rc = as->rc;

         GSM1xSMSApp_RunFSM(pMe); // Start the SMS state machine engine.
         return TRUE;

      case EVT_DIALOG_INIT:

         // Update the active dialog info in the one and only SMSApp object.
         GSM1xSMSDialogHandler_UpdateActiveDialogInfo(pMe,wParam, dwParam);

         // Route the received event to the current active dialog handler.
         return GSM1xSMSDialogHandler_RouteDialogEvent(pMe,eCode,wParam,dwParam);

      case EVT_DIALOG_START:
         // Route the received event to the current active dialog handler.
         return GSM1xSMSDialogHandler_RouteDialogEvent(pMe,eCode,wParam,dwParam);

      case EVT_DIALOG_END:
         if(wParam == 0)
         {
            return TRUE;
         }

         // Route the received event to the current active dialog handler.
         (void) GSM1xSMSDialogHandler_RouteDialogEvent(pMe,eCode,wParam,dwParam);

         pMe->m_activeDlg = NULL;
         pMe->m_activeDlgHandleEvent = NULL;

         // Don't run the FSM if we are suspending (EVT_APP_SUSPEND is
         // sent before EVT_DIALOG_END).  Wait until we resume...
         if(pMe->m_suspending == FALSE)
         {
            GSM1xSMSApp_RunFSM(pMe); // Start the SMS state machine engine.
         }
         return TRUE;

      case EVT_BUSY:
         // The applet is never 'busy'
         break;

      case EVT_NOTIFY:
         return GSM1xSMSApp_HandleNotifyEvent(pMe, (AEENotify*)dwParam);

      case EVT_SEND_STATUS:
         // One of the two states must be chosen (depending upon the status
         // of the message send operation)
         if(dwParam == IDS_MSG_SEND_SUCCESS)
         {
            GSMSMSStatusType newstatus = (pMe->m_bSRR) ?
               GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED : GSMSMS_STATUS_MO_SENT;

            // Check if save of Outbox message was successful
            if (pMe->m_bOutMsgSaveFailed == FALSE)
            {
                // Mark as sent as we are able to send this message successfully.
                (void)IGSMSMS_SetMessageStatus(pMe->m_pIGSMSMS,
                                           pMe->m_wOutgoingMessageID,
                                           newstatus,
                                           GSMSMS_STORE_NVRAM);

                pMe->m_bNeedNVRAMRefresh = TRUE;
            }

            // Update the state.
            MOVE_TO_STATE(STATE_MSG_SENT_OK);
         }
         else
         {
            // Update the failure status and transition to the next state.
            pMe->m_wSendFailureStatus = (int16) SMS_SUBMIT_REPORT_FAILURE;
            pMe->m_wSubmitReportFailure = (int16) dwParam;
            MOVE_TO_STATE(STATE_MSG_SENT_FAIL);
         }

         // Applet is already running. The state is already updated. Let's just
         // close the dialog. This makes the main handle event to resume the
         // state machine with the new state.
         CLOSE_DIALOG(DLGRET_CREATE);
         return TRUE;

	  case EVT_CALL_STATE:
            switch(wParam)
            {
               case CM_CALL_STATE_INCOM:
                  // call starting
                  pMe->m_wNumReceived = 0;

                  //(void)IPHONE_AnswerCall(pMe->m_pIPhone, CM_CALL_TYPE_SMS); // fix for 6100, pk-3.0.0.
                  GSM1xSMSApp_SetSMSDisplayAnnunciators( pMe, (uint16)ANNUN_INUSE,
                                                    (uint16)ANNUN_INUSE);
                  pMe->m_bReceiving = TRUE;
                  //bStartApp = TRUE;   // yes start the Applet
                  break;
               case CM_CALL_STATE_IDLE:
                  // call ending
                  if(pMe->m_bReceiving ==  TRUE)
                  {
                     (void)IPHONE_EndCall(pMe->m_pIPhone);
					pMe->m_bReceiving = FALSE;
					 // 3/5/03 - There seems to be an issue here where that if the
			         // message comes in as a SO6, the message is never able to be
					 // viewed, especially if it is a class0 message (it is just lost then)
					 // Therefore, we should switch to the message arrived state so that we
					 // can see the message and do what we need to do.
					// tempState = STATE_MSG_ARRIVED;
                     //tempState = STATE_NUMMSG_RECEIVED;
                     //bStartApp = TRUE;
                  }
                  GSM1xSMSApp_SetSMSDisplayAnnunciators(pMe,(uint16)ANNUN_INUSE,0);
                  break;
               case CM_CALL_STATE_ORIG:
                  GSM1xSMSApp_SetSMSDisplayAnnunciators(pMe, (uint16)ANNUN_INUSE,
                                                    (uint16)ANNUN_INUSE);
                  break;
               //case CM_CALL_STATE_CONV:	Don't need this...
               default:
                  // do nothing
                  break;
            }
		 return TRUE;

      default:
         // Route the received event to the current active dialog handler.
         return GSM1xSMSDialogHandler_RouteDialogEvent(pMe,eCode,wParam,dwParam);
   }

   return FALSE;
} // GSM1xSMSApp_HandleEvent()

//------------------------------------------------------------------------------
// Public Helper Function Definitions (Used by all other modules of the SMS)
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_AddAddresseesToMenu

DESCRIPTION:
    Add address from current message to the menu.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pIMenuMain [in/out] : Pointer to the menu control.

RETURN VALUE:
    None

COMMENTS:
    Queries the return addresses from the message referred
    by the m_wCurrMsgID.

    After the adressee information is extracted from the FROM field, the
    address will be added to the REPLY INFORMATION menu.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_AddAddresseeToMenu(SMSApp *pMe, IMenuCtl *pIMenuMain)
{
   if(!pMe)
   {
      return;
   }

   GSM1xSMSMessageStore_GetAddresseeAtIndex(pMe,
                                       pMe->m_eMBoxType,
                                       pMe->m_wCurrMsgID,
                                       pMe->m_strReplyInfoNum,
                                       MAX_REPLYINFO_CHARS);

   // Add these items with the radio button bitmaps so we can tell which
   // item was selected when we move to the main menu.
   (void)GSM1xSMSUtil_AddBitmapMenuItem(pMe,
                                   pIMenuMain,
                                   pMe->m_strReplyInfoNum,
                                   (uint16)(ADD_CMD_BASE),
                                   IDB_RADIOBUTTON_ON,
                                   AEE_FONT_NORMAL,
                                   0);
}

#ifdef GSM1XSMSAPP_UNIT_TEST
static void TestSendSubmitReport(SMSApp *pMe)
{
   uint8 idx = 0;
   byte data[255];
   byte test_ack = 0;

   if (test_ack)
   {
      data[idx++] = 3;     // RP_MT_ACK;

      data[idx++] = 0;     // RP_MR (set by InjectMessage)
      data[idx++] = 0x41;  // GSMSMS_USER_DATA_IEI
      data[idx++] = 9;     //
      data[idx++] = 1;     // GSMSMS_TL_SMS_SUBMIT_REPORT, UDHI
      data[idx++] = 0;     // Parameter Indicator
      data[idx++] = 32;    // SCTS
      data[idx++] = 112;
      data[idx++] = 146;
      data[idx++] = 1;
      data[idx++] = 83;
      data[idx++] = 0;
      data[idx++] = 0;
   }
   else
   {
      static byte tp_cause = 0x80;

      data[idx++] = 5;     // RP_MT_ERROR

      data[idx++] = 0;     // RP_MR (set by InjectMessage)
      data[idx++] = 1;     // len = 1 (not 2, Diagnostic byte not present)
      data[idx++] = 0;     // RP Cause (ignored)
      data[idx++] = 0x41;  // GSMSMS_USER_DATA_IEI
      data[idx++] = 9;     //
      data[idx++] = 1;     // GSMSMS_TL_SMS_SUBMIT_REPORT, UDHI
      data[idx++] = tp_cause; // TP_FCS
      data[idx++] = 0;     // Parameter Indicator
      data[idx++] = 32;    // SCTS
      data[idx++] = 112;
      data[idx++] = 146;
      data[idx++] = 1;
      data[idx++] = 83;
      data[idx++] = 0;
      data[idx++] = 0;
   }

   IGSMSMS_InjectMessage(pMe->m_pIGSMSMS, data, idx);
}
#endif
/*=============================================================================
FUNCTION: GSM1xSMSApp_StartSendingMessage

DESCRIPTION:
    Using the IPhone interface, this method will send the user message.
    Also, save message if it needs to be saved, move message to sent
    box if it was sent for first time.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    eSaveMove [in]: code to save, send only, or move

RETURN VALUE:
    None

COMMENTS:
   Depending upon the configuration (access or traffic or access/traffic), the
   message will be sent upon the access or traffic channel.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
int GSM1xSMSApp_StartSendingMessage(SMSApp *pMe, SMSOutboxOp eSaveMove)
{
   int  result = SUCCESS;
   GSMSMSMsg gsmMsg;

   if(!pMe)
   {
      return EFAILED;
   }

   pMe->m_bOutMsgSaveFailed = FALSE;

   // GSM does not require From field
   MEMSET(pMe->m_msOut.m_szFrom, 0, sizeof(pMe->m_msOut.m_szFrom));

   result = GSM1xSMSApp_CreateMessage(pMe, &gsmMsg, &pMe->m_msOut);
   if (result != AEE_GSMSMS_SUCCESS)
   {
      pMe->m_bOutMsgSaveFailed = TRUE;
   }
   else
   {
       if (eSaveMove == SMSOUTBOXOP_SAVE)
       {
          pMe->m_bSRR =  gsmMsg.msg.SMSSubmit.SRR;

          // Save to NVRAM
          pMe->m_wOutgoingMessageID = 0xFFFF;
          result = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS,
                                        &gsmMsg, GSMSMS_STORE_NVRAM,
                                        &pMe->m_wOutgoingMessageID);

          if (result != AEE_GSMSMS_SUCCESS)
          {
             // The outgoing message save operation is failed.
             // Let's set the SMS Applet object variable to indicate the same.
             // This is later used by the state handler to report a warning to
             // the user indicating that the save operation is failed.
             pMe->m_bOutMsgSaveFailed = TRUE;

             if (result == AEE_GSMSMS_ESTORE_FULL)
             {
                DBGPRINTF("GSM1x_Test:SM saved on NVRAM failed Cause NVRAM Full");
             }
          }
          else
          {
             pMe->m_bNeedNVRAMRefresh = TRUE;
          }
       }

       // Send the message
       CALLBACK_Init(&pMe->m_IGSMSMSCallback, GSM1xSMSApp_MsgSendStatusCallBack, pMe);
       pMe->m_wSendFailureStatus = 0;
       result = IGSMSMS_SendSMSSubmit(pMe->m_pIGSMSMS,
                                      &gsmMsg.msg.SMSSubmit,
                                      &pMe->m_IGSMSMSCallback,
                                      &pMe->m_IGSMSMSReport);

       if (result == AEE_GSMSMS_SUCCESS)
       {
          DBGPRINTF("GSM1x_Test: SMS_Submit Msg sent");
#ifdef GSM1XSMSAPP_UNIT_TEST
          // Enable this to test responses
          TestSendSubmitReport(pMe);
#endif
       }
   }

   return result;
}



/*=============================================================================
FUNCTION: GSM1xSMSApp_DecodeAddress

DESCRIPTION:
    Convert a GSM address to unicode

PARAMETERS:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_DecodeAddress(GSMSMSAddress const *inAddr, AECHAR *outAddr, int len)
{
   if (inAddr->ton == GSMSMS_TON_INTERNATIONAL_NUMBER)
   {
      outAddr[0] = (AECHAR)'+';
      STRTOWSTR((char *)inAddr->addr, &outAddr[1], len-sizeof(AECHAR));
   }
   else
   {
      STRTOWSTR((char *)inAddr->addr, &outAddr[0], len);
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_CreateMessage

DESCRIPTION:
    Create an SMS Submit message based on the current OUT box entry

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet object
    pMsg [out]: Pointer to the message to be filled in

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
static int GSM1xSMSApp_CreateMessage(SMSApp *pMe, GSMSMSMsg *pMsg, SMSMessageStruct *pMsgIn)
{
   byte encodeLen;
   int result;

   result = IGSMSMS_CreateDefaultMessage(pMe->m_pIGSMSMS,
          GSMSMS_MSG_SMS_SUBMIT, pMsg);
   if (result != AEE_GSMSMS_SUCCESS)
   {
      return result;
   }

   // Set the TP-MR
   if (IGSMSMS_SetTPMR(pMe->m_pIGSMSMS, ++pMe->m_TPMR) != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("Could not set TP-MR in EF-SMSS on SIM");
   }
   pMsg->msg.SMSSubmit.MR = pMe->m_TPMR;

   // If the SC addr field is set, use the supplied SC address (used for RP bit set)
   if (pMe->m_msOut.m_scAddr.addr[0] != '\0')
   {
      DBGPRINTF("RP was set, using supplied SC address");
      MEMCPY(&pMsg->msg.SMSSubmit.SCAddr,
             &pMe->m_msOut.m_scAddr,
             sizeof(pMsg->msg.SMSSubmit.SCAddr));
   }

   // Update Destination Address
   if (pMe->m_msOut.m_szTo[0] == (AECHAR)'+')
   {
      pMsg->msg.SMSSubmit.DA.ton = GSMSMS_TON_INTERNATIONAL_NUMBER;
      WSTR_TO_STR(&pMsgIn->m_szTo[1],
                  (char *)pMsg->msg.SMSSubmit.DA.addr,
                  sizeof(pMsg->msg.SMSSubmit.DA.addr));
   }
   else
   {
      pMsg->msg.SMSSubmit.DA.ton = GSMSMS_TON_NATIONAL_NUMBER;
      WSTR_TO_STR(&pMsgIn->m_szTo[0],
                  (char *)pMsg->msg.SMSSubmit.DA.addr,
                  sizeof(pMsg->msg.SMSSubmit.DA.addr));
   }
   pMsg->msg.SMSSubmit.DA.npi = GSMSMS_NPI_ISDN_TELEPHONE;

   pMsg->msg.SMSSubmit.UDL = WSTRLEN(pMsgIn->m_szMessage); // number of septets

   result = IGSMSMS_EncodeUserData(pMe->m_pIGSMSMS,
                          pMsgIn->m_szMessage,
                          pMsg->msg.SMSSubmit.UD,
                          sizeof(pMsg->msg.SMSSubmit.UD),
                          GSMSMS_ENCODING_7BIT,
                          &encodeLen);
   if (result != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_EncodeUserData failed");
      return result;
   }
   return result;
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_SaveMessageToOutbox

DESCRIPTION:
    Save the current message to the OUT box

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet object

RETURN VALUE:
    SUCCESS if we succeed and the error if we don't

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
int GSM1xSMSApp_SaveMessageToOutbox(SMSApp *pMe)
{
   GSMSMSMsg gsmMsg;
   int result;

   if(!pMe)
   {
      return AEE_GSMSMS_EFAILED;
   }

   pMe->m_bOutMsgSaveFailed = FALSE;

   result = GSM1xSMSApp_CreateMessage(pMe, &gsmMsg, &pMe->m_msOut);
   if (result != AEE_GSMSMS_SUCCESS)
   {
      pMe->m_bOutMsgSaveFailed = TRUE;
   }
   else
   {
       pMe->m_wOutgoingMessageID = 0xFFFF;
       result = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS,
                                     &gsmMsg,
                                     GSMSMS_STORE_NVRAM,
                                     &pMe->m_wOutgoingMessageID);

       if (result != AEE_GSMSMS_SUCCESS)
       {
          // The outgoing message save operation is failed.
          // Let's set the SMS Applet object variable to indicate the same.
          // This is later used by the state handler to report a warning to
          // the user indicating that the save operation is failed.
          pMe->m_bOutMsgSaveFailed = TRUE;

          if (result == AEE_GSMSMS_ESTORE_FULL)
          {
             DBGPRINTF("GSM1x_Test:SM saved on NVRAM failed Cause NVRAM Full");
          }
       }
       else
       {
          pMe->m_bNeedNVRAMRefresh = TRUE;
       }
   }

   return (result);
}

/*===========================================================================

FUNCTION GSM1xSMSApp_UpdateAnnunciators

DESCRIPTION
   This function updates the annunciator(s) based on the number of new SMS and
   Voice Mail messages.

PARAMETERS
   pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE
   None

COMMENTS
   Note that this function DOES NOT update the INUSE Annunciator.

SIDE EFFECTS
   Updates the annunciator, even if the applet is not active

SEE ALSO:

===========================================================================*/
void GSM1xSMSApp_UpdateAnnunciators (SMSApp * pMe)
{
   uint16 wAnnunciatorVal = 0;

   if(!pMe)
   {
      return;
   }

   // Set the new voicemail/message annunciators appropriately
   // TODO - check for unread MT SIM messages
   if(pMe->m_wNumInMsgsNew > 0 || pMe->m_wNumSIMMsgsNew > 0)
   {
      wAnnunciatorVal |= ANNUN_MSG;
   }

   if(pMe->m_cfg.m_wNumVMMsgs > 0)
   {
      wAnnunciatorVal |= ANNUN_VMAIL;
   }

   // NOTE: Currently, this LCD does not differentiate between the VMAIL and
   //       MSG icons - it sets the same icon.
   GSM1xSMSApp_SetSMSDisplayAnnunciators(pMe, ANNUN_VMAIL | ANNUN_MSG,
                                     wAnnunciatorVal);
}

/*===========================================================================

FUNCTION GSM1xSMSApp_UpdateMboxCounters

DESCRIPTION
   This function updates the message counters for the NVRAM, VM and SIM
   mailboxes.

PARAMETERS
   pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE
   None

COMMENTS
   Note that this function DOES NOT update the INUSE Annunciator.

SIDE EFFECTS
   Updates the annunciator, even if the applet is not active

SEE ALSO:

===========================================================================*/
void GSM1xSMSApp_UpdateMboxCounters(SMSApp *pMe)
{
   uint16  i;
   int     errType;
   GSMSMSStatusType status;

   DBGPRINTF("Called GSM1xSMSApp_UpdateMboxCounters");

   // Update NVRAM counters
   if (pMe->m_bNeedNVRAMRefresh)
   {
      pMe->m_wNumInMsgs = 0;
      pMe->m_wNumOutMsgs = 0;
      pMe->m_wNumInMsgsNew = 0;
      for(i=0; i<= pMe->m_wMaxNVMsgs;i++)
      {
         errType = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_NVRAM);
         if(errType == AEE_GSMSMS_SUCCESS)
         {
            switch (status)
            {
            case GSMSMS_STATUS_MT_READ:
               pMe->m_wNumInMsgs++;
               break;
            case GSMSMS_STATUS_MT_NOT_READ:
               pMe->m_wNumInMsgsNew++;
               pMe->m_wNumInMsgs++;
               break;
            case GSMSMS_STATUS_MO_NOT_SENT:
            case GSMSMS_STATUS_MO_SENT:
            case GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED:
            case GSMSMS_STATUS_MO_SENT_ST_NOT_STORED:
            case GSMSMS_STATUS_MO_SENT_ST_STORED:
               pMe->m_wNumOutMsgs++;
               break;
            default:
               break;
           }
        }
      }
      pMe->m_bNeedNVRAMRefresh = FALSE;
   }

   // Update SIM counters
   if ((pMe->m_IGSMSMSInit) && (!pMe->m_bSSTDisabled) && pMe->m_bNeedSIMRefresh)
   {
      pMe->m_wNumSIMMsgs = 0;
      pMe->m_wNumSIMMsgsNew = 0;
      for(i=0; i<= pMe->m_wMaxSIMMsgs;i++)
      {
         errType = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_SIM);
         if(errType == AEE_GSMSMS_SUCCESS)
         {
            switch (status)
            {
            case GSMSMS_STATUS_MT_NOT_READ:
               pMe->m_wNumSIMMsgsNew++;
              pMe->m_wNumSIMMsgs++;
               break;
            case GSMSMS_STATUS_MT_READ:
            case GSMSMS_STATUS_MO_SENT:
            case GSMSMS_STATUS_MO_NOT_SENT:
            case GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED:
            case GSMSMS_STATUS_MO_SENT_ST_NOT_STORED:
            case GSMSMS_STATUS_MO_SENT_ST_STORED:
               pMe->m_wNumSIMMsgs++;
               break;
            default:
               break;
            }
         }
         else if(errType == AEE_GSMSMS_EBUSY)
         {
            break;
         }
      }
      pMe->m_bNeedSIMRefresh = FALSE;
   }

   if (pMe->m_bNeedVMRefresh)
   {
      pMe->m_cfg.m_wNumVMMsgs = 0;
      errType = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, 0, &status,
                                         GSMSMS_STORE_NVRAM_VM);
      if(errType == AEE_GSMSMS_SUCCESS)
      {
         switch (status)
         {
         case GSMSMS_STATUS_MT_READ:
         case GSMSMS_STATUS_MT_NOT_READ:
            pMe->m_cfg.m_wNumVMMsgs = 1;
            break;
         default:
            break;
         }
      }
      pMe->m_bNeedVMRefresh = FALSE;
   }
   GSM1xSMSApp_UpdateAnnunciators(pMe);
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_UpdateMenuItemsForMailBox

DESCRIPTION:
    Show the screen which displays the contents of the current mailbox.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pIMenuMain [in/out]: Pointer to the menu control to which we have to add
                     all the messages.

RETURN VALUE:
    None

COMMENTS:
    This function is shared by the dialog handlers to show messages in the
    inbox, outbox and the SIM inbox and outbox.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_UpdateMenuItemsForMailBox(SMSApp *pMe, IMenuCtl *pIMenuMain)
{
   uint16  numMsgs;
   uint16  wID = SMS_MESSAGE_STORE_INDEX_NONE;
   uint16  dwItemsAdded = 0;
   uint16  i = 0;
   uint32 *pdwParams;

   // Temporary buffer that holds the information required to create
   // menu items for each database record.
   struct _MenuItemData {
      uint16  wIconID;
      uint32  dwEncodedMBoxAndID;
      AECHAR  pTitle[MAX_TITLE_LEN];
   } *menuItemData;

   if(!pMe)
   {
      return;
   }

   GSM1xSMSUtil_ShowBusyIcon(pMe->a.m_pIShell,
                pMe->a.m_pIDisplay,
                &pMe->m_rc,
                FALSE); // Set this parameter to FALSE so that the helper
                        // function will not clear the existing screen while
                        // showing an hour glass.

   GSM1xSMSApp_UpdateMboxCounters(pMe);

   if(pIMenuMain)
   {
      // Set the menu title to the appropriate mailbox.
      switch(pMe->m_eMBoxType)
      {
         case SMS_MB_INBOX:
            if (pMe->m_wNumInMsgs == 0)
            {
               return;
            }
            numMsgs = pMe->m_wNumInMsgs;
            // inbox will contain pages and messages
            IMENUCTL_SetProperties(pIMenuMain, MP_UNDERLINE_TITLE);
            (void)IMENUCTL_SetTitle(pIMenuMain, GSM1XSMS_RES_FILE,
                                    IDS_INBOX, NULL);

            break;
         case SMS_MB_OUTBOX:
            if (pMe->m_wNumOutMsgs == 0)
            {
               return;
            }
            numMsgs = pMe->m_wNumOutMsgs;
            IMENUCTL_SetProperties(pIMenuMain, MP_UNDERLINE_TITLE);
            (void)IMENUCTL_SetTitle(pIMenuMain, GSM1XSMS_RES_FILE,
                                    IDS_OUTBOX, NULL);
            break;
         case SMS_MB_SIM:
            if (pMe->m_wNumSIMMsgs == 0)
            {
               return;
            }
            numMsgs = pMe->m_wNumSIMMsgs;
            IMENUCTL_SetProperties(pIMenuMain, MP_UNDERLINE_TITLE);
            (void)IMENUCTL_SetTitle(pIMenuMain, GSM1XSMS_RES_FILE,
                                    IDS_SIM, NULL);
            break;
         default:
            // Not a recognized mailbox for this operation
            ASSERT_NOT_REACHABLE;
            return;
      }

      // Allocate memory for the arrays of params
      pdwParams = (uint32*)MALLOC(numMsgs * sizeof(uint32));

      // Allocate a temporary buffer to store all the required information
      // from each record to create the menu item for the record.  This
      // is done because we load the database from oldest to newest, but
      // we display the messages from newest to oldest.
      //
      // This allocation looks large, but it's really not that bad:
      //    sizeof(*menuItemData) is current 70 bytes and the
      //    number of entries is determined by the OEM in AEEGSMSMS.h.
      menuItemData = MALLOC(sizeof(*menuItemData) * numMsgs);
      if (NULL == menuItemData) {
         DBGPRINTF("Out of memory");
         return;
      }

      // Load each message from the database from oldest to newest, parse
      // the required information and store the result temporarily in the
      // menuItemData[] array.
      for (;;)
      {
         if ( (dwItemsAdded >= numMsgs) || (dwItemsAdded > MAX_MSG_CMDS)) {
            // At the end of the list
            break;
         }

         if (GSM1xSMSMessageStore_GetNextMessage(pMe, &wID, &pMe->m_msIn, FALSE) == FALSE)
         {
            DBGPRINTF("!bValidMsg");
            break;
         }

         // Update the title. The title field of the message structure
         // is already updated by the helper function GSM1xSMSUtil_ExtractTitle
         // (based on the mailbox type). If the mailbox is outbox then we
         // extract the title based on the "to" field. For all other types
         // of mail boxes we extract it based on the "from" field.

         // If the helper function GSM1xSMSUtil_ExtractTitle is not able to
         // extract the title then we use the message text as the title.
         if((pMe->m_msIn.m_szTitle)[0] == (AECHAR)'\0')
         {
            // Leave an extra space at the end of the title so that BREW
            // can add a NULL character.
            WSTRNCOPYN((AECHAR *) menuItemData[dwItemsAdded].pTitle,
                       sizeof(menuItemData[dwItemsAdded].pTitle) /
                                                                sizeof(AECHAR),
                       pMe->m_msIn.m_szMessage,
                       (sizeof(menuItemData[dwItemsAdded].pTitle) /
                                                          sizeof(AECHAR)) - 1);
         }
         else
         {
            // Update the title field of the messages.
            WSTRNCOPYN((AECHAR *) menuItemData[dwItemsAdded].pTitle,
                       sizeof(menuItemData[dwItemsAdded].pTitle) /
                                                                sizeof(AECHAR),
                       pMe->m_msIn.m_szTitle, -1);
         }

         if((pMe->m_eMBoxType == SMS_MB_INBOX) || (pMe->m_eMBoxType == SMS_MB_SIM))
         {
            // Set the Unread/Read Icon
            menuItemData[dwItemsAdded].wIconID = pMe->m_msIn.m_bNew
                                          ? IDB_INBOX_UNREAD : IDB_INBOX_READ;


            // Encode the box type and record ID in a 32 bit parameter
            menuItemData[dwItemsAdded].dwEncodedMBoxAndID
                = (uint32)ENCODE_MBOX_AND_MSG_ID((uint32)pMe->m_eMBoxType,
                                                 wID);
         }
         else
         {
            // Set the data parameter to be sent back
            menuItemData[dwItemsAdded].dwEncodedMBoxAndID
                     = (uint32)ENCODE_MBOX_AND_MSG_ID((uint32)pMe->m_eMBoxType,
                                                      wID);

            // Set the Saved/Sent Icon
            menuItemData[dwItemsAdded].wIconID = pMe->m_msIn.m_bNew
                                         ? IDB_OUTBOX_SAVED : IDB_OUTBOX_SENT;
         }
         dwItemsAdded++;
      }

      // Add each record to the menu in reverse order (newest to oldest)
      for(i = dwItemsAdded; i > 0; i--)
      {
         // save information about box type and ID
         pdwParams[i-1] = menuItemData[i-1].dwEncodedMBoxAndID;

         // Add the item, with bitmap, to the menu
         (void)GSM1xSMSUtil_AddBitmapMenuItem(pMe,
                                         pIMenuMain,
                                         (AECHAR *) menuItemData[i-1].pTitle,
                                         (uint16)(MSG_CMD_BASE + i - 1),
                                         menuItemData[i-1].wIconID,
                                         AEE_FONT_BOLD,
                                         menuItemData[i-1].dwEncodedMBoxAndID);
      }

      // Release the temporary menu buffer
      if (menuItemData) {
         FREE((void *) menuItemData);
      }

      // Remember the list of message IDs. But before we do this, release the
      // memory that we allocated before.
      if(pMe->m_pdwIDList)
      {
         FREE(pMe->m_pdwIDList);    // free old list
      }
      // Update the list pointer to keep track of the new list.
      pMe->m_pdwIDList = pdwParams;
      pMe->m_wIDListSize = dwItemsAdded;  // keep track of number of valid IDs

      // Clear the message struct.
      GSM1xSMSMessageStore_ResetMessageStruct(&pMe->m_msIn);
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_UpdateCurrMsgWithNextMsg

DESCRIPTION:
   Updates the current message id with the next message.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    TRUE : If there is a valid next message in the list
    FALSE : If there is no next message in the list and the current message is
            the last message.

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSApp_UpdateCurrMsgWithNextMsg(SMSApp *pMe)
{
   boolean         bValidMsg = FALSE;
   uint32         *pdwParam;
   uint32          param;
   uint16          wNextID;
   SMSMBoxType     wNextBox;
   uint16          wCurrID;
   int             i;

   if(!pMe || !pMe->m_pdwIDList)
   {
      return FALSE;
   }

   // find the current ID in our list of IDs
   pdwParam = pMe->m_pdwIDList;
   wNextID = 0;
   wNextBox = SMS_MB_NONE;
   wCurrID = pMe->m_wCurrMsgID;
   for(i = 0; i < pMe->m_wIDListSize; i++)
   {
      if((uint16)DECODE_MSG_ID(pdwParam[i]) == wCurrID)
      {
         if(i == 0)
         { // we are at the last message, return to mailbox screen
            return FALSE;
         }
         param = pdwParam[i-1];    // next ID
         wNextID = (uint16)DECODE_MSG_ID(param);
         wNextBox = (SMSMBoxType)DECODE_MBOX(param);
         break;
      }
   }

   if(i == pMe->m_wIDListSize)
   {
      return FALSE;
   }

   // Get the contents of the message specified by pMe->m_wCurrMsgID.
   bValidMsg = GSM1xSMSMessageStore_GetMessageByID(pMe, wNextBox, wNextID, &pMe->m_msIn, TRUE);
   if(bValidMsg)
   {
      pMe->m_wCurrMsgID = wNextID;
      pMe->m_eMBoxType = wNextBox;
      return TRUE;
   }

   // No more messages to show:
   return FALSE;
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_MoveSIMMsg

DESCRIPTION:
   Based on the SMS Applet m_wCurrMsgID and the bToSIM field, a message
   move operation is performed from Inbox to the SIM or vice versa.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure.
   bToSIM [in] : To indicate whether the move operation needs to be performed
                  from In box -> SIM or SIM -> In box.
RETURN VALUE:
   boolean : True to indicate that the store operation is success.
             False to indicate that the store operation is failed.

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSApp_MoveSIMMsg(SMSApp *pMe, boolean bToSIM)
{
   uint16  index = 0xFFFF; // used to indicate any_slot
   int     status;         // Status of storing the message
   GSMSMSStorageType readFrom, writeTo;

   if(bToSIM)
   {
      switch (pMe->m_eMBoxType)
      {
      case SMS_MB_INBOX:
      case SMS_MB_OUTBOX:
         readFrom = GSMSMS_STORE_NVRAM;
         writeTo = GSMSMS_STORE_SIM;
         break;
      default:
         return FALSE;
      }
   }
   else
   {
      switch (pMe->m_eMBoxType)
      {
      case SMS_MB_SIM:
         writeTo = GSMSMS_STORE_NVRAM;
         readFrom = GSMSMS_STORE_SIM;
         break;
      default:
         return FALSE;
      }
   }

   // If a message just arrived but the inbox is full then
   // the message id will be invalid which indicates the
   // message has not been saved in the inbox.  In this
   // case, store the message on the SIM instead of moving it
   if (bToSIM &&
       pMe->m_eMBoxType == SMS_MB_INBOX &&
       pMe->m_wCurrMsgID == 0xffff)
   {
      status = IGSMSMS_StoreMessage
         (pMe->m_pIGSMSMS,
          &pMe->m_decodedMsg,
          GSMSMS_STORE_SIM,
          &index);
      if (status == AEE_GSMSMS_SUCCESS)
      {
         IGSMSMS_SetMessageStatus
            (pMe->m_pIGSMSMS,
             index,
             GSMSMS_STATUS_MT_READ,
             GSMSMS_STORE_SIM);
      }
   }
   else
   {
      status = IGSMSMS_MoveMessage
         (pMe->m_pIGSMSMS,
          readFrom,
          pMe->m_wCurrMsgID,
          writeTo,
          &index);
   }
   if (status == AEE_GSMSMS_SUCCESS)
   {
      pMe->m_bNeedNVRAMRefresh = TRUE;
      pMe->m_bNeedSIMRefresh = TRUE;
   }
   return (status == AEE_GSMSMS_SUCCESS);
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_EraseOldMessagesFromInbox

DESCRIPTION:
    Erases all old messages (messages which are already read by the user)
    from the Inbox.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    None

COMMENTS:
    None

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_EraseOldMessagesFromInbox(SMSApp *pMe, boolean showBusyIcon)
{
   if(!pMe)
   {
      return;
   }

   if (showBusyIcon == TRUE)
   {
      GSM1xSMSUtil_ShowBusyIcon(pMe->a.m_pIShell,pMe->a.m_pIDisplay,&pMe->m_rc, FALSE);
   }

   GSM1xSMSMessageStore_EraseOldMessagesFromInbox(pMe);

}

//------------------------------------------------------------------------------
//   Local Function Definitions (used only in the SMSApp.c file)
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_SetSMSDisplayAnnunciators

DESCRIPTION:
    Updates the SMS display annunciators based on the mask and value.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    mask [in]:
    val [in]:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/

static void GSM1xSMSApp_SetSMSDisplayAnnunciators
(
   SMSApp * pMe,
   uint16 mask,
   uint16 val
)
{
   // The SMS display annunciators can be updated (from the idle applet and
   // also from the phone notification handler) irrespective of whether it is
   // the active applet or not. So, we can not rely on the m_pDisplay attribute
   // of the SMSApp object as it is set/updated only when the SMS applet is
   // the active applet.

   // However we can rely on the fact that the m_pShell of the SMSApp object is
   // already set when the applet is loaded/created.
   //
   // But if the m_pDisplay attribute is non-NULL (ie, like when the applet is
   // running), we will take advantage of that instead of creating a second
   // IDisplay interface.

   if (pMe->a.m_pIDisplay)
   {
      IDISPLAY_SetAnnunciators(pMe->a.m_pIDisplay, mask, val);

   }
   else
   {
      IDisplay *pDisplay = NULL;
      if (ISHELL_CreateInstance(pMe->a.m_pIShell,
                                AEECLSID_DISPLAY,
                                (void **)&pDisplay) == SUCCESS)
      {
         IDISPLAY_SetAnnunciators(pDisplay, mask, val);
         IDISPLAY_Release(pDisplay);
      }
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_HandleNotifyEvent

DESCRIPTION:
    Handle the case of a notify event.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pN [in]: Pointer to a AEENotify object

RETURN VALUE:
    boolean

COMMENTS:
   Handles all the notifications from the Phone (status updates, new incoming
   page/message/voice mail messages). Also saves the message to the inbox
   and updates the temporary state (based on the storage status). Finally
   checks to make sure that a state transition is required. If so, closes
   the current active dialog and resumes the state machine with the temporary
   state.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
static boolean GSM1xSMSApp_HandleNotifyEvent(SMSApp *pMe, AEENotify *pN)
{
   AEECLSID              clsid = pN->cls;
   uint32                dwMask = pN->dwMask;
   SMSState              tempState = STATE_NONE;
   boolean               bStartApp = FALSE;   // Do not start Applet

   if(!pMe)
   {
      return FALSE;
   }

   if(clsid == AEECLSID_TAPI)
   {
      // If TAPI status has changed
      if (GET_NOTIFIER_MASK(dwMask) == NMASK_TAPI_STATUS)
      {
         TAPIStatus *statusPtr;

         statusPtr = (TAPIStatus *)pN->pData;
         if (statusPtr->bDigital == TRUE)          // If service is available
         {
            pMe->m_serviceActive = TRUE;

            // Try a deferred check but recheck memory state
            if (pMe->m_SMMARetry)
            {
               GSM1xSMSApp_CheckIfSMMARequired(pMe, TRUE);
            }
         }
         else
         {
            pMe->m_serviceActive = FALSE;
         }
      }
   }
   else if(clsid == AEECLSID_IGSMSMSNOTIFIER)
   {
      if (dwMask & NMASK_GSMSMS_INIT)
      {
         // SIM interface is init and accessible, update counters and annunciators
         DBGPRINTF("Received NMASK_GSMSMS_INIT");
         pMe->m_IGSMSMSInit = TRUE;
         pMe->m_bNeedSIMRefresh = TRUE;
         GSM1xSMSApp_UpdateMboxCounters(pMe);

         // Refresh screen
         CLOSE_DIALOG(DLGRET_CREATE);

         // check SMMA
         GSM1xSMSApp_CheckIfSMMARequired(pMe, TRUE);

         // This event will never happen again so no need to deregister
      }
      else if(dwMask & NMASK_GSMSMS_MT_MESSAGE)
      {
         bStartApp = TRUE;   // yes start the Applet

         // GSM1xSMSMT_MTMsgHandler will set pMe->m_eMBoxType to the
         // required state if it returns TRUE
         if (GSM1xSMSMT_MTMsgHandler(pMe, (GSMSMSRawMsg *)pN->pData) != TRUE)
         {
            return TRUE;
         }

         if (pMe->m_eMBoxType == SMS_MB_VOICEMAIL)
         {
            tempState = STATE_VOICEMAIL;
         }
         else
         {
            tempState = STATE_MSG_ARRIVED;
         }
      }
   }
   else if(clsid == AEECLSID_PHONE)
   {
	   //we only care about NMASK_PH_PHONE_INFO notification from IPHONE.
	  if(dwMask & NMASK_PH_PHONE_INFO)
      {
         // If New messages are on SIM and SIM interface is not yet
         // initialized, the indicator will not show yet.
         GSM1xSMSApp_UpdateAnnunciators(pMe);

         // We are not interested in any events from the phone object except
         // the NMASK_PH_CALL_INFO.
		 // We don't care about any events from the phone object. -- PY
         (void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                      AEECLSID_APP_GSM1XSMS,
                                      AEECLSID_PHONE,
				  NULL);

      }
   }

   if(bStartApp)
   {
      //Update the annunciator bars
      GSM1xSMSApp_UpdateAnnunciators(pMe);

      // Update the state.
      MOVE_TO_STATE(tempState);

      // If this applet is not active, then call StartApplet to start it.
      if(ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_APP_GSM1XSMS)
      {
         // The SMS applet is not running and we need to run it to show an
         // incoming message screen to the user. But, before we do that we need
         // to set the m_bNaturalStart to false to indicate that it is not a
         // natural start.

         pMe->m_bNaturalStart = FALSE;
         (void)ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS);
      }
      else
      {
         // Applet is already running. The state is already updated. Let's just
         // close the dialog. This makes the main handle event resume the
         // state machine with the new state that we set right before this
         // 'if' block.
         CLOSE_DIALOG(DLGRET_CREATE);
      }
   }

   return TRUE;
}



/*=============================================================================
FUNCTION: GSM1xSMSApp_MsgSendStatusCallBack

DESCRIPTION:
   Callback function which is passed to the BREW and then later
   invoked by the BREW to notify the SMS send message action status
   to the applet.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    None

COMMENTS:
   Here in this function, we will only post an event to the applet. When the
   applet handles this event, depending upon the status, it will update the
   SMS state variable.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void GSM1xSMSApp_MsgSendStatusCallBack(void * pUser)
{
   uint32 status=0;
   SMSApp *pMe = (SMSApp*) pUser;
   GSMSMSMsg gsm;

   DBGPRINTF("GSM1x_Test: SMS_SubmitReport Msg Received");

   if (pMe->m_IGSMSMSReport.sendResult != AEE_GSMSMS_SUCCESS)
   {
      if (pMe->m_IGSMSMSReport.sendResult == AEE_GSMSMS_ETIMEDOUT)
      {
         // Indicate message
         status = IDS_MSG_SEND_TIMEOUT;
      }
      else
      {
         status = IDS_MSG_SEND_FAILED;
      }
   }
   else
   {
      if (IGSMSMS_DecodeMessage(pMe->m_pIGSMSMS, &pMe->m_IGSMSMSReport.reportMsg, &gsm) != SUCCESS)
      {
         status = IDS_MSG_SEND_BAD_RESPONSE;
      }
      else if (gsm.msgType == GSMSMS_MSG_SMS_SUBMIT_REPORT)
      {
         if (gsm.msg.SMSSubmitReport.FCS_present)
         {
            switch (gsm.msg.SMSSubmitReport.FCS)
            {
            case 0x80:
            case 0x81:
            case 0x82:
               status = IDS_MSG_SEND_PID_UNSUPPORTED;
               break;
            case 0x8F:
               status = IDS_MSG_SEND_PID_ERR;
               break;
            case 0x90:
               status = IDS_MSG_SEND_DCS_UNSUPPORTED;
               break;
            case 0x9F:
               status = IDS_MSG_SEND_DCS_ERR;
               break;
            case 0xA0:
            case 0xA1:
            case 0xAF:
               status = IDS_MSG_SEND_TP_COMMAND_ERR;
               break;
            case 0xB0:
               status = IDS_MSG_SEND_TPDU_ERR;
               break;
            case 0xC0:
               status = IDS_MSG_SEND_SC_BUSY;
               break;
            case 0xC1:
               status = IDS_MSG_SEND_SC_NO_SUB;
               break;
            case 0xC2:
               status = IDS_MSG_SEND_SC_SYS_FAIL;
               break;
            case 0xC3:
               status = IDS_MSG_SEND_SME_ADDR_ERR;
               break;
            case 0xC4:
               status = IDS_MSG_SEND_SME_BARRED;
               break;
            case 0xC5:
               status = IDS_MSG_SEND_DUPLICATE_MSG;
               break;
            case 0xC6:
               status = IDS_MSG_SEND_VPF_UNSUPPORTED;
               break;
            case 0xC7:
               status = IDS_MSG_SEND_VP_UNSUPPORTED;
               break;
            default:
               status = IDS_MSG_SEND_FAILED;
               break;
            }
         }
         else
         {
            status = IDS_MSG_SEND_SUCCESS;
         }
      }
      else
      {
         status = IDS_MSG_SEND_BAD_RESPONSE;
      }
   }

   (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                           EVT_SEND_STATUS, 0, (uint32)status);
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_InitAppData

DESCRIPTION:
      Initializes all the SMS applet structure data to their default values.

PARAMETERS:
       pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    EFAILED          : if failed to init the app
    SUCCESS          : if everything goes as expected

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int GSM1xSMSApp_InitAppData
(
   IApplet* po                                              // Application data
)

{
   SMSApp *pMe;                               // Pointer to application data
   int errType;

   pMe = (SMSApp *)po;
   if(!pMe)
   {
      return EFAILED;
   }

   pMe->m_deviceInfo.wStructSize = sizeof(pMe->m_deviceInfo);
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pMe->m_deviceInfo);

   pMe->m_prevState = STATE_NONE;
   pMe->m_currState = STATE_INIT;
   pMe->m_activeDlg = NULL;
   pMe->m_activeDlgHandleEvent = NULL;

   pMe->m_msgBox_IStatic = NULL;

   pMe->m_pIGSMSMS = NULL;
   pMe->m_pIPhone = NULL;

   pMe->m_serviceActive = FALSE;

   //  Create IGSM1x reference
   if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IGSMSMS,
                            (void **)&pMe->m_pIGSMSMS) != SUCCESS)
   {
      // Some thing went wrong and we are about to return. Do some cleanup
      // and release the resources, if they are allocated.
      DBGPRINTF("Failed to create AEECLSID_IGSMSMS");
      GSM1xSMSApp_FreeAppData(po);
      return EFAILED;
   }

   if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_PHONE,
                            (void **)&pMe->m_pIPhone) != SUCCESS)
   {
      // Some thing went wrong and we are about to return. Do some cleanup
      // and release the resources, if they are allocated.
      DBGPRINTF("Failed to create AEECLSID_PHONE");
      GSM1xSMSApp_FreeAppData(po);
      return EFAILED;
   }

   if (ISHELL_CreateInstance(pMe->a.m_pIShell,
                             AEECLSID_CONFIG,
                            (void **)&pMe->m_pConfig) != SUCCESS)
   {
      // Some thing went wrong and we are about to return. Do some cleanup
      // and release the resources, if they are allocated.
      DBGPRINTF("Failed to create AEECLSID_CONFIG");
      GSM1xSMSApp_FreeAppData(po);
      return FALSE;
   }

   // Initialize preferences.
   pMe->m_cfg.m_eMsgAlertPref = ALERT_NONE;

   // Initialize mailbox values.
   pMe->m_eMBoxType = SMS_MB_NONE;

   pMe->m_wCurrMsgID = 0;
   pMe->m_wNumReceived = 0;

   // Initialize the message structures.
   GSM1xSMSMessageStore_ResetMessageStruct(&pMe->m_msIn);
   GSM1xSMSMessageStore_ResetMessageStruct(&pMe->m_msOut);

   // Initialize the list of message IDs
   pMe->m_pdwIDList = NULL;
   pMe->m_wIDListSize = 0;

   // Initialize the resource id to display "Send To" title on the screen.
   pMe->m_nSendToTitleResId =  IDS_ST_SEND_TO_LABEL;


   // Initialize the key parameters (mainly) used by the various dialog handlers
   // and the state handlers.
   pMe->m_activeDlg = NULL;
   pMe->m_activeDlgHandleEvent = NULL;
   pMe->m_prevMenuItemSel = MAIN_MENUITEM_VM;
   pMe->m_mainMenuSel = MAIN_MENUITEM_VM;
   pMe->m_bNaturalStart = TRUE;
   pMe->m_eEraseOption = ERASEOPT_NONE;
   pMe->m_bDoNotOverwriteDlgResult = FALSE;
   pMe->m_bNeedToMakeVoiceCall = FALSE;
   pMe->m_bMoveToSIM = TRUE;
   pMe->m_bSendingMessage = TRUE;
   pMe->m_bOutMsgSaveFailed = FALSE;

   pMe->m_wSendFailureStatus = SUCCESS;

   MEMSET(&pMe->m_SMMACallback,0,sizeof(pMe->m_SMMACallback));
   MEMSET(&pMe->m_IGSMSMSCallback,0,sizeof(pMe->m_IGSMSMSCallback));
   MEMSET(&pMe->m_decodedMsg,0,sizeof(pMe->m_decodedMsg));
   pMe->m_decodedMsg.msgType = GSMSMS_MSG_SMS_UNKNOWN;

   pMe->m_bStorageFull = FALSE;
   pMe->m_SMMARetry = TRUE;

   pMe->m_testTag=1;

   errType = IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_NVRAM, &pMe->m_wMaxNVMsgs);
   if (errType != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_GetStoreSize failed for NVRAM");
      return FALSE;
   }

   pMe->m_bSSTDisabled = FALSE;
   errType = IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_SIM, &pMe->m_wMaxSIMMsgs);

   // Check if SMS is enabled in SIM service table
   if (errType == AEE_GSMSMS_ESST)
   {
      pMe->m_bSSTDisabled = TRUE;
      pMe->m_IGSMSMSInit = TRUE;
   }
   else if (errType != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_GetStoreSize failed for SIM");
      return FALSE;
   }
   DBGPRINTF("SMSApp m_bSSTDisabled = %d", pMe->m_bSSTDisabled);
   DBGPRINTF("SMSApp m_IGSMSMSInit = %d", pMe->m_IGSMSMSInit);

   // Do not bother checking SIM if SIM is disabled
   if (pMe->m_bSSTDisabled)
   {
      pMe->m_bNeedSIMRefresh = FALSE;
   }
   else
   {
      pMe->m_bNeedSIMRefresh = TRUE;
   }

   pMe->m_bNeedNVRAMRefresh = TRUE;
   pMe->m_bNeedVMRefresh = TRUE;



   // Set the TP-MR
   if (IGSMSMS_GetTPMR(pMe->m_pIGSMSMS, &pMe->m_TPMR) != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("Could not get TP-MR from EF-SMSS on SIM");
      pMe->m_TPMR = 0;
   }

   // Check if IGSMSMS is init
   if (IGSMSMS_IsInit(pMe->m_pIGSMSMS) == TRUE)
   {
      // SIM interface is init, update counters and annunciators
      DBGPRINTF("SMSApp IGSMSMS_IsInit = %d", 1);
      pMe->m_IGSMSMSInit = TRUE;
      pMe->m_bNeedSIMRefresh = TRUE;
      GSM1xSMSApp_CheckIfSMMARequired(pMe, TRUE);
   }
   else
   {
      pMe->m_IGSMSMSInit = FALSE;
      pMe->m_bNeedSIMRefresh = FALSE;
   }
   GSM1xSMSApp_UpdateMboxCounters(pMe);

   return SUCCESS;
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_CheckIfSMMARequired

DESCRIPTION:
      Checks whether a SMMA message must be sent from the phone. It checks
      once initially

PARAMETERS:
       pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    EFAILED          : if failed to init the app
    SUCCESS          : if everything goes as expected

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_CheckIfSMMARequired(SMSApp *pMe, boolean memCheck)
{
   uint16 i;
   int rv;
   GSMSMSStatusType status;
   boolean NVFull = FALSE;
   boolean SIMFull = FALSE;
   uint8   flag;

   DBGPRINTF("CheckIfSMMARequired m_IGSMSMSInit = %d", pMe->m_IGSMSMSInit);

   // No need to continue
   if ((!pMe->m_IGSMSMSInit) || (!pMe->m_serviceActive))
   {
      pMe->m_SMMARetry = TRUE;
      return;
   }

   // Unregister for with ITAPI
   if(ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS, AEECLSID_TAPI, 0))
   {
      DBGPRINTF("ISHELL_RegisterNotify deregister for NMASK_TAPI_STATUS failed");
   }

   if (pMe->m_bSSTDisabled)
   {
      pMe->m_SMMARetry = FALSE;
      return;
   }

   // We are finally checking...
   pMe->m_SMMARetry = FALSE;

   rv = IGSMSMS_GetMemoryCapExceededFlag(pMe->m_pIGSMSMS, &flag);

   // If flag was set check if SIM and NV still full
   if (rv == AEE_GSMSMS_SUCCESS && ((flag & 0x01) == 0))
   {
      for (i=0; memCheck && i< pMe->m_wMaxNVMsgs; i++)
      {
         rv = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_NVRAM);
         if (rv == AEE_GSMSMS_SUCCESS)
         {
            if (status == GSMSMS_STATUS_NONE)
            {
               break;
            }
         }
      }

      if (i >= pMe->m_wMaxNVMsgs)
      {
         NVFull = TRUE;
      }

      for (i=0; memCheck && NVFull == TRUE && i< pMe->m_wMaxSIMMsgs; i++)
      {
         rv = IGSMSMS_GetMessageStatus(pMe->m_pIGSMSMS, i, &status, GSMSMS_STORE_SIM);
         if (rv == AEE_GSMSMS_SUCCESS)
         {
            if (status == GSMSMS_STATUS_NONE)
            {
               break;
            }
         }
      }
      if (i >= pMe->m_wMaxSIMMsgs)
      {
         SIMFull = TRUE;
      }

      if ((!memCheck) || (!SIMFull) || (!NVFull))
      {
         // Send SMMA
         CALLBACK_Init(&pMe->m_SMMACallback, GSM1xSMSApp_SMMACallBack, pMe);
         DBGPRINTF("SMSApp: Sending SMMA");
         (void)IGSMSMS_SendMoreMemoryAvailable(pMe->m_pIGSMSMS,
                                               &pMe->m_SMMACallback,
                                               &pMe->m_SMMAReport);
      }
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_FreeAppData

DESCRIPTION:

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void GSM1xSMSApp_FreeAppData(IApplet *po)
{
   SMSApp *pMe;

   pMe = (SMSApp *)po;
   if(!pMe)
   {
      return;
   }

   if(pMe->m_pIGSMSMS)
   {
      IGSMSMS_Release(pMe->m_pIGSMSMS);
      pMe->m_pIGSMSMS = NULL;
   }

   if(pMe->m_pIPhone)
   {
      IPHONE_Release(pMe->m_pIPhone);
      pMe->m_pIPhone = NULL;
   }

   if(pMe->m_pdwIDList)
   {
      FREE(pMe->m_pdwIDList);
      pMe->m_pdwIDList = NULL;
   }

   if(pMe->m_msgBox_IStatic)
   {
      (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
      pMe->m_msgBox_IStatic = NULL;
   }

   if (pMe->m_pConfig)
   {
      ICONFIG_Release(pMe->m_pConfig);
      pMe->m_pConfig = NULL;
   }
}

#if defined(BREW_STATIC_APP)
/*=============================================================================
FUNCTION: GSM1xSMSApp_RegisterForEvents

DESCRIPTION:
     Registers for any events this applet wishes to handle. This function
     unregisters all old events with all pertinent classes before registering.

PARAMETERS:
    pMe [in]: Pointer to the Ishell structure.

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
static void GSM1xSMSApp_RegisterForEvents (IShell * pMe)
{
   uint32 dwMask;

   // Register with ITAPI
   if(ISHELL_RegisterNotify(pMe, AEECLSID_APP_GSM1XSMS, AEECLSID_TAPI, NMASK_TAPI_STATUS))
   {
      DBGPRINTF("ISHELL_RegisterNotify for NMASK_TAPI_STATUS failed");
      return;
   }

   // Register for SMS messages with IGSMSMS
   dwMask = NMASK_GSMSMS_INIT | NMASK_GSMSMS_MT_MESSAGE;
   if(ISHELL_RegisterNotify(pMe, AEECLSID_APP_GSM1XSMS, AEECLSID_IGSMSMSNOTIFIER, dwMask))
   {
      DBGPRINTF("ISHELL_RegisterNotify for NMASK_GSMSMS_MT_MESSAGE failed");
      return;
   }

   // SO6/14 call status
   // The NMASK_PH_PHONE_INFO is registered here. However, after we receive the
   // first notification, we update the SMS annunicator and unregister for this
   // event.

   // In case if any new phone events are added here, they should also be ADDED
   // in the GSM1xSMSApp_HandleNotifyEvent() function.

   //we only need to register for NMASK_PH_PHONE_INFO event.
   dwMask = NMASK_PH_PHONE_INFO;
   if(ISHELL_RegisterNotify(pMe, AEECLSID_APP_GSM1XSMS,
                            AEECLSID_PHONE, dwMask))
   {
      DBGPRINTF("ISHELL_RegisterNotify for NMASK_PH_PHONE_INFO failed");
      return;
   }

   //Register SMS Agent with IPHONE event NMASK_PH_CALL_INFO
   if (ISHELL_RegisterNotify(pMe, AEECLSID_SMS_AGENT,
                            AEECLSID_PHONE,NMASK_PH_CALL_INFO))
    {
	   DBGPRINTF("ISHELL_RegisterNotify for NMASK_PH_CALL_INFO failed");
       return;
    }

}
#endif

//-----------------------------------------------------------------------------
//                    Applet State Method Definitions
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_RunFSM

DESCRIPTION: Run the finite state machine

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void GSM1xSMSApp_RunFSM(SMSApp *pMe)
{
   NextFSMAction nextFSMAction = NFSMACTION_WAIT;
   for(;;)
   {
      nextFSMAction = GSM1xSMSStateMachine_ProcessState(pMe);

      // We do not want to complicate the SMS state machine and hence we added
      // one boolean to indicate whether the dlgResult should be overwritten
      // here or not. In some specific cases (see the state machine design)
      // this avoids the unnecessary complexity and eases the design.
      if(pMe->m_bDoNotOverwriteDlgResult)
      {
         // We allow only one iteration without touching this variable. So,
         // let's reset it back to indicate that we are going to overwrite the
         // result for all subsequent iterations.
         pMe->m_bDoNotOverwriteDlgResult = FALSE;
      }
      else
      {
         // Overwrite the dlgResult to its default value. The next dialog that
         // we are going to display will set and return the dlgResult based on
         // the users action.
         pMe->m_dlgResult = DLGRET_CREATE;
      }

      // If the state machine processing engine indicates that it created a
      // dialog and requesting us to exit this loop, then we quit.
      if(nextFSMAction == NFSMACTION_WAIT)
      {
         break;
      }
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_SMMACallBack

DESCRIPTION:
   Callback function which is passed to the BREW and then later
   invoked by the BREW to notify the App thatthe SMMA message
   was sent successfully.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    None

COMMENTS:
   Here in this function, we will only post an event to the applet. When the
   applet handles this event, depending upon the status, it will update the
   SMS state variable.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_SMMACallBack(void * pUser)
{
   SMSApp *pMe = (SMSApp*) pUser;

   if (pMe->m_SMMAReport.sendResult != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("SMSApp: SMMA ACK not received");
   }
   else
   {
      DBGPRINTF("SMSApp: SMMA ACK received");
   }
   // Unset the flag regardless of whether this failed or not
   (void)IGSMSMS_SetMemoryCapExceededFlag(pMe->m_pIGSMSMS, 0xFF);
}
