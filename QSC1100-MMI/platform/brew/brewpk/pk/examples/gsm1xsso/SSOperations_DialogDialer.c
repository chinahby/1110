/*===========================================================================
FILE: SSOperations_DialogDialer.c

SERVICES: SS Operations Dialog for dialing a number

GENERAL DESCRIPTION:
    
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AutomationLogging.h"

#include "SSOperations_DialogCommon.h"
#include "SSOperations_UserEvents.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_CommonMessageDefinitions.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

// Max number if digits the user can enter in the dialing dialog
// when in a call.
#define MAX_DIALED_DIGITS_IN_CALL   64

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static void SSOperationsDialog_DisplayDialerSoftkeyBar(
    CSSOperationsDialogHandler *pSelf
    );

static SSEventData* SSOperationsDialog_CreateSetupMsgEventData(
    CSSOperationsDialogHandler *pSelf
    );

static SSMsg_msg_setup_type* SSOperationsDialog_CreateSetupMsg(
    AECHAR *pDialString
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_DialerHandleEvent

DESCRIPTION: Dialer dialog event handler (IDD_DIALER)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_DialerHandleEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   ITextCtl *pDialStringCtl = (ITextCtl*)
       IDIALOG_GetControl(
       pSelf->m_pDialog,
       IDDC_DIALER_DIALSTRING
       );

   switch (eCode) 
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         int       len;
         AEERect   rc;
         IMenuCtl *sk;

         SSOperationsDialog_DisplayDialerSoftkeyBar(pSelf);

         // Adjust the TextCtl rectangle so it doesn't overlap the softkey
         sk = (IMenuCtl*)
             IDIALOG_GetControl(
                 pSelf->m_pDialog, 
                 IDDC_DIALER_SOFTKEY
                 );
         IMENUCTL_GetRect(sk, &rc);
         rc.dy = rc.y;
         rc.y = 0;
         ITEXTCTL_SetRect(pDialStringCtl, &rc);

         ITEXTCTL_SetMaxSize(pDialStringCtl, MAX_DIALED_DIGITS_IN_CALL); 

         // Do we need to initialize the TextCtl with a value?
         len = WSTRLEN(pSelf->m_DialString);
         if (len > 0) 
         {
            (void) ITEXTCTL_SetText(pDialStringCtl,
                                    pSelf->m_DialString,
                                    -1);
         }

         // Disassociate the softkey menu from the dial-string text control
         ITEXTCTL_SetSoftKeyMenu(pDialStringCtl, NULL);

         // Set initial input mode
         (void) ITEXTCTL_SetInputMode(pDialStringCtl, AEE_TM_NUMBERS);

         // Align number to bottom right
         ITEXTCTL_SetProperties(pDialStringCtl, TP_MULTILINE);

         // Set focus to text control (soft key bar is default)
         (void) IDIALOG_SetFocus(pSelf->m_pDialog, IDDC_DIALER_SOFTKEY);

         return TRUE;
      }

      case EVT_DIALOG_END:
         (void) ITEXTCTL_GetText(pDialStringCtl,
                                 pSelf->m_DialString,
                                 sizeof(pSelf->m_DialString)/sizeof(AECHAR));
         return TRUE;

   case EVT_COMMAND:
      switch (wParam) 
      {
         case IDS_CANCEL:
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
            return TRUE;

         case IDS_SK_OPTIONS:
            return TRUE;
      }
      break;

   case EVT_KEY_PRESS:
      switch ((AVKType)wParam) 
      {
         case AVK_UP:
         case AVK_VOLUME_UP:            
            SET_VOLUME_PREV_DIALOG_SETUP_TO_CURRENT;
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallVolumeDialog );
            return TRUE;

         case AVK_DOWN:
         case AVK_VOLUME_DOWN:
            SET_VOLUME_PREV_DIALOG_SETUP_TO_CURRENT;
            SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallVolumeDialog );            
            return TRUE;
      }
      break;

   case EVT_KEY_RELEASE:

#ifdef PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC
      switch ((AVKType)wParam) 
      {
         case AVK_0:
         case AVK_1:
         case AVK_2:
         case AVK_3:
         case AVK_4:
         case AVK_5:
         case AVK_6:
         case AVK_7:
         case AVK_8:
         case AVK_9:
         case AVK_STAR:
         case AVK_POUND:
             // stop continuous DTMF
             (void) IPHONE_StopDTMF(pSelf->m_pPhone,
                                    0);
             break;

         default:
             break;
      }
#endif //PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC
      break;

   case EVT_KEY:
      
      // for a key to be included in the dialed string, 
      // concatenate the typed key to the dialed string,
      // and then update the display.
      switch ((AVKType)wParam) 
      {
         case AVK_0:
         case AVK_1:
         case AVK_2:
         case AVK_3:
         case AVK_4:
         case AVK_5:
         case AVK_6:
         case AVK_7:
         case AVK_8:
         case AVK_9:
         case AVK_STAR:
         case AVK_POUND:
         {
            int    len;
            char   szStr[2];
            AECHAR wStr[2];

            szStr[0] = SSOperationsDialog_AVKType2ASCII((AVKType)wParam);
            szStr[1] = '\0';

            STRTOWSTR(szStr, wStr, sizeof(wStr));

            (void) ITEXTCTL_GetText(pDialStringCtl,
                                    pSelf->m_DialString,
                                    sizeof(pSelf->m_DialString)/sizeof(AECHAR));

            len = WSTRLEN(pSelf->m_DialString);
            if (len < (int)( ARRAY_SIZE(pSelf->m_DialString) - 1)) 
               {
               (void)WSTRCPY(&pSelf->m_DialString[len],
                             wStr);
            }

            (void) ITEXTCTL_SetText(pDialStringCtl,
                                    pSelf->m_DialString,
                                    -1);

#ifdef PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC
            // start continuous DTMF
            (void) IPHONE_StartDTMF(pSelf->m_pPhone,
                                    *(byte*)szStr);

            /*
            (void) IPHONE_BurstDTMF(pSelf->m_pPhone,
                                    SS_DTMF_ON_150,
                                    SS_DTMF_OFF_100,
                                    (byte*)szStr,
                                    1);
            */
#endif //PHONE_DOES_NOT_AUTOMATICALLY_SEND_DTMF_WHILE_IN_TRAFFIC

            SSOperationsDialog_DisplayDialerSoftkeyBar(pSelf);
            
            IDISPLAY_UpdateEx(pSelf->m_pDisplay, FALSE);

            return TRUE;
         }

         // if there is a number displayed, 'clear' means delete (backspace).
         // otherwise, the it means going to previous dialog.
         case AVK_CLR:
         {
            int len;

            (void) ITEXTCTL_GetText(pDialStringCtl,
                                    pSelf->m_DialString,
                                    sizeof(pSelf->m_DialString)/sizeof(AECHAR));

            len = WSTRLEN(pSelf->m_DialString);

            if (len <= 1) 
               {
               // Clearing the last digit exits the dialog
               pSelf->m_DialString[0] = 0;
               (void) ITEXTCTL_SetText(pDialStringCtl,
                                       pSelf->m_DialString,
                                       -1);

               SETUP_NEW_UI_DIALOG( SSOperationsDialog_SetupCallDetailsDialog );
               return TRUE;
            }

            pSelf->m_DialString[len-1] = 0;
            (void) ITEXTCTL_SetText(pDialStringCtl,
                                    pSelf->m_DialString,
                                    -1);

            SSOperationsDialog_DisplayDialerSoftkeyBar(pSelf);
            
            // Draw it now!
            IDISPLAY_UpdateEx(pSelf->m_pDisplay, FALSE);
            
            return TRUE;
         }

         // user tries to setup a new call, calling the typed number.
         case AVK_SEND:
         {
            SSCall_TInfo_type  callTInfo;
            SSEventData       *pEventData;
            boolean            success;

            (void) ITEXTCTL_GetText(
                      pDialStringCtl,
                      pSelf->m_DialString,
                      sizeof(pSelf->m_DialString)/sizeof(AECHAR)
                      );

            // Currently, we do not support holding an MPTY call.
            // This section will be removed when we do support it.
            {
               SSCall_Info_type  *pCallInfo;
               SSCall_Index_type  callIndex;
               boolean            success;

               for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
               {
                  success = SSCallDatabase_GetCallInfo(
                               pSelf->m_pCallDatabase, 
                               callIndex, 
                               &pCallInfo
                               );

                  if (success)
                  {
                      // an additional MO call setup when a MPTY call already
                      // exists is not supported in the current release!!!
                      if (pCallInfo->isMPTY) 
                      {
                          GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls, "MO call setup while MPTY call exists is not supported!",0,0,0);
                          return TRUE;
                      }
                  }
               }
            }
            
            pEventData = SSOperationsDialog_CreateSetupMsgEventData(pSelf);
            if (NULL == pEventData)
            {
               return TRUE;
            }

            success = SSCallDatabase_AllocateMoTi(
                         pSelf->m_pCallDatabase, 
                         &callTInfo.TIValue
                         );
            if (!success)
            {
               SSOEvent_FreeEventData(pEventData);
               GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, 
                    "SSO Dialer:MO TI alloc failed!",0,0,0);
               return TRUE;
            }

            callTInfo.callStartTime = ISHELL_GetUpTimeMS(pMe->m_pIShell);
            callTInfo.callOrigType = SSCall_MO;
            callTInfo.callStringPI = SS_PI_ALLOWED;

            callTInfo.callStatus = SS_CALL_STATUS_SETUP;

            WSTRLCPY(
               callTInfo.callString,
               pSelf->m_DialString,
               sizeof(callTInfo.callString)/sizeof(AECHAR)
               );

            // try and find out the call's name
            SSOperationsDialog_DoNumberLookup(
                pSelf->m_pAddressBook,
                callTInfo.callString,
                callTInfo.callName,
                sizeof(callTInfo.callName)
                );
          
            success = SSCallDatabase_AddCall(
                         pSelf->m_pCallDatabase,
                         &callTInfo,
                         &pSelf->m_OriginatingSecondCallDialogCallIndex,
                         NULL
                         );
        
            if (!success)
            {
               SSCallDatabase_DeallocateMoTi(
                  pSelf->m_pCallDatabase,
                  callTInfo.TIValue
                  );

               SSOEvent_FreeEventData(pEventData);
               GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO Dialer:Add MO Call failed!",0,0,0);
               return TRUE;
            }

            // after call entry was added, set the event data's call index
            pEventData->callwiseMsgData.callIndex1 = 
                pSelf->m_OriginatingSecondCallDialogCallIndex;

            AUTOMATION_LOG_WSTR("Setup:%s", pSelf->m_DialString);
            SETUP_NEW_UI_DIALOG( 
                SSOperationsDialog_SetupOriginatingSecondCallDialog 
                );

            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      UIMN_SETUP_REQ,
                      0,
                      (uint32)pEventData
                      );

            return TRUE;
         }
         break;
      }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_DialerHandleNonDisplayEvent

DESCRIPTION: Dialer dialog non dialog event handler (IDD_DIALER)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_DialerHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
   switch (eCode) 
   {
      case UIMN_SETUP_IND:
      {
          SSOperationsDialog_MoveToTheIncomingDialog(
              pSelf, 
              (SSEventData*)dwParam
              );
          return TRUE;
      }
   }

   return FALSE;
}

/*===========================================================================
FUNCTION: SSOperationsDialog_SetupDialerDialog

DESCRIPTION:
    This function creates the Call Dialer dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupDialerDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( SSOperationsDialog_DialerHandleEvent );
        SHOW_DIALOG( IDD_DIALER );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_DialerHandleNonDisplayEvent 
        );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_CreateSetupMsgEventData

DESCRIPTION: Create a setup message, using the dialing string

PARAMETERS:
   *pSelf: Pointer to the app object

RETURN VALUE:
    None

=============================================================================*/
static SSEventData* SSOperationsDialog_CreateSetupMsgEventData(
    CSSOperationsDialogHandler *pSelf
    )
{
    SSEventData *pEventData = 
       SSOEvent_AllocateEventData(SS_EVENT_DATA_CALLWISE_MSG);
    if (pEventData == NULL)
    {            
        GSM1X_LOG_ERROR(pSelf->m_pSSUtilCls, "SSO Dialer:failed msg alloc!",0,0,0);
        return NULL;
    }

    pEventData->callwiseMsgData.msg = (SSMsg_int_msg_type *) 
        SSOperationsDialog_CreateSetupMsg(pSelf->m_DialString);

    if (NULL == pEventData->callwiseMsgData.msg)
    {
        SSOEvent_FreeEventData(pEventData);
        return NULL;
    }

    return pEventData;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_CreateSetupMsg

DESCRIPTION: 
    Create a message for call setup, using the dialing string.

PARAMETERS:
   *pDialString: pointer to a valid dial string

RETURN VALUE:
    None

=============================================================================*/
static SSMsg_msg_setup_type* SSOperationsDialog_CreateSetupMsg(
    AECHAR *pDialString
    )
{
    SSMsg_msg_setup_type *pMsg;
    char                  numberString[SSMSG_NUMBER_FIELD_MAX_LENGTH+1];

    assert(pDialString != NULL);

    pMsg = (SSMsg_msg_setup_type*)SSOMessage_AllocateMsg(SSMSG_MSG_TYPE_SETUP);
    if (pMsg == NULL) 
    {
        DBGPRINTF("SSO Dialer:Setup Msg alloc failed!");
        return pMsg;
    }

    pMsg->bearer_capability_1.hdr.present = TRUE;
    pMsg->bearer_capability_1.contents_len = 1;
    pMsg->bearer_capability_1.contents[0] = 0xA0;

    WSTRTOSTR(
       pDialString,
       numberString,
       sizeof(numberString)
       );
    assert(sizeof(numberString) > sizeof(pMsg->called_party_bcd_number.number));

    pMsg->called_party_bcd_number.hdr.present = TRUE;
    pMsg->called_party_bcd_number.np_id = SSMSG_NP_ID_ISDN_TELEPHONY;
    pMsg->called_party_bcd_number.ton = SSMSG_TON_NATIONAL_NUMBER;
    pMsg->called_party_bcd_number.number_len = STRLEN(numberString);
    MEMCPY(
       pMsg->called_party_bcd_number.number,
       numberString,
       sizeof(pMsg->called_party_bcd_number.number)
       );

    pMsg->called_party_subaddress.hdr.present = FALSE;
    pMsg->calling_party_bcd_number.hdr.present = FALSE;
    pMsg->calling_party_subaddress.hdr.present = FALSE;
    pMsg->cause_of_no_cli.hdr.present = FALSE;
    pMsg->signal.hdr.present = FALSE;

    return pMsg;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_DisplayDialerSoftkeyBar

DESCRIPTION: Used by the Dialer dialog to display its softkey

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
 
RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SSOperationsDialog_DisplayDialerSoftkeyBar(
    CSSOperationsDialogHandler *pSelf
    )
{
    IMenuCtl   *pSKMenu;
    CtlAddItem  ai;

    assert(IDD_DIALER == pSelf->m_DialogID);

    pSKMenu = (IMenuCtl*)IDIALOG_GetControl(pSelf->m_pDialog,
                                            IDDC_DIALER_SOFTKEY);

    // NOTE: This also serves to remove the "Multi-tap" key that automatically
    // appears with the creation of an ITextCtl
    (void) IMENUCTL_DeleteAll(pSKMenu);

    ai.pText = NULL;
    ai.pImage = NULL;
    ai.pszResImage = SSOPERATIONS_RES_FILE;
    ai.pszResText = SSOPERATIONS_RES_FILE;
    ai.dwData = NULL;
    ai.wFont = 0;

    SSOperations_SetDefaultSoftkeyLook(pSelf->m_pShell, pSKMenu);

    (void) IMENUCTL_Redraw(pSKMenu);
}
