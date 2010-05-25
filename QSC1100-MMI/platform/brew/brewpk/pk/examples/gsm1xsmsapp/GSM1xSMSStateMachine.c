/*===========================================================================
  FILE: GSM1xSMSStateMachine.c

  SERVICES: SMS State Machine
 
  GENERAL DESCRIPTION:
      This file implements all SMS state mahine handlers which are used
      by the SMSApp to bring up various SMS screens to the user.             

  PUBLIC CLASSES AND STATIC FUNCTIONS:
            
  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gsm1xsms_res.h"          // Applet Resource Header
#include "GSM1xSMSApp.h"           // SMS Applet services and definitions
#include "GSM1xSMSUtil.h"          // SMS Applet helper functions
#include "GSM1xSMSMessageStore.h"  // Applet Message Store Helper Functions.
#include "AEETAPI.h"               // Used for MakeVoiceCall

/*===========================================================================

                           DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                              MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                              TYPE DECLARATIONs

===========================================================================*/

typedef NextFSMAction (*STATE_HANDLER)(SMSApp *pMe);

/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

// List of state handlers (all these are local functions and used only by the
// SMSStateMachine.c)
static NextFSMAction StateNoOperationHandler(SMSApp *pMe);
static NextFSMAction StateInitHandler(SMSApp *pMe);
static NextFSMAction StateMainHandler(SMSApp *pMe);
static NextFSMAction StateVoiceMailHandler(SMSApp *pMe);
static NextFSMAction StateVoiceMailCallHandler(SMSApp *pMe);
static NextFSMAction StateInBoxHandler(SMSApp *pMe);
static NextFSMAction StateInMsgHandler(SMSApp *pMe);
static NextFSMAction StateInReplyInfoHandler(SMSApp *pMe);
static NextFSMAction StateCreateMsgHandler(SMSApp *pMe);
static NextFSMAction StateCreateTextMsgHandler(SMSApp *pMe);
static NextFSMAction StateSendOptionsHandler(SMSApp *pMe);
static NextFSMAction StateMsgSendHandler(SMSApp *pMe);
static NextFSMAction StateOutboxHandler(SMSApp *pMe);
static NextFSMAction StateOutMsgHandler(SMSApp *pMe);
static NextFSMAction StateMsgSentOkHandler(SMSApp *pMe);
static NextFSMAction StateMsgSentFailHandler(SMSApp *pMe);

static NextFSMAction StateSIMHandler(SMSApp *pMe);
static NextFSMAction StateSIMMsgHandler(SMSApp *pMe);
static NextFSMAction StateMsgMoveHandler(SMSApp *pMe);

static NextFSMAction StateEraseMsgsHandler(SMSApp *pMe);
static NextFSMAction StateMsgEraseHandler(SMSApp *pMe);
static NextFSMAction StateMsgSettingsHandler(SMSApp *pMe);
static NextFSMAction StateMSVoiceMailNumberHandler(SMSApp *pMe);
static NextFSMAction StateMSVoiceMailNumberEditHandler(SMSApp *pMe);
static NextFSMAction StateMSAlertHandler(SMSApp *pMe);
static NextFSMAction StateMsgArrivedHandler(SMSApp *pMe);
static NextFSMAction StateMsgRejectedHandler(SMSApp *pMe);
static NextFSMAction StateThresholdWarningHandler(SMSApp *pMe);
static NextFSMAction StateSendPreformatHandler(SMSApp *pMe);
static NextFSMAction StateExitHandler(SMSApp *pMe);
static NextFSMAction StateEditSCAddrHandler(SMSApp *pMe);
static NextFSMAction StateTestHandler(SMSApp *pMe);

//------------------------------------------------------------------------------
//                            Public Function Definitions
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSStateMachine_ProcessState

DESCRIPTION: .
   Looking at the intenal static state handler table (by using the 
   pMe->m_smsState as an index), this function routes the call to the state 
   handler.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
NextFSMAction GSM1xSMSStateMachine_ProcessState(SMSApp *pMe)
{
   NextFSMAction retVal = NFSMACTION_WAIT;

   if(pMe)
   {
      STATE_HANDLER fcnPtr;

      switch (pMe->m_currState)
      {
         case STATE_NONE:              
            fcnPtr = StateNoOperationHandler;
            break;

         case STATE_INIT:              
            fcnPtr = StateInitHandler;
            break;

         case STATE_MAIN:              
            fcnPtr = StateMainHandler;
            break;

         case STATE_VOICEMAIL:         
            fcnPtr = StateVoiceMailHandler;
            break;
         
         case STATE_VM_CALL:           
            fcnPtr = StateVoiceMailCallHandler;
            break;

         case STATE_INBOX:                
            fcnPtr = StateInBoxHandler;
            break;
             
         case STATE_IN_MSG:            
            fcnPtr = StateInMsgHandler;
            break;
             
         case STATE_IN_REPLY_INFO:     
            fcnPtr = StateInReplyInfoHandler;
            break;

         case STATE_EDIT_SC_ADDR:        
            fcnPtr = StateEditSCAddrHandler;
            break;
        
         case STATE_CREATE_MSG:        
            fcnPtr = StateCreateMsgHandler;
            break;
        
         case STATE_CREATE_MSG_TEXT:   
            fcnPtr = StateCreateTextMsgHandler;
            break;

         case STATE_SEND_OPTIONS:      
            fcnPtr = StateSendOptionsHandler;
            break;

         case STATE_MSG_SEND:          
            fcnPtr = StateMsgSendHandler;
            break;

         case STATE_OUTBOX:            
            fcnPtr = StateOutboxHandler;
            break;
            
         case STATE_OUT_MSG:           
            fcnPtr = StateOutMsgHandler;
            break;
            
         case STATE_MSG_SENT_OK:       
            fcnPtr = StateMsgSentOkHandler;
            break;

         case STATE_MSG_SENT_FAIL:     
            fcnPtr = StateMsgSentFailHandler;
            break;

         case STATE_SIM:               
            fcnPtr = StateSIMHandler;
            break;
              
         case STATE_SIM_MSG:           
            fcnPtr = StateSIMMsgHandler;
            break;

         case STATE_MSG_MOVE:          
            fcnPtr = StateMsgMoveHandler;
            break;

   
         case STATE_ERASE_MSGS:        
            fcnPtr = StateEraseMsgsHandler;
            break;
        
         case STATE_MSG_ERASE:         
            fcnPtr = StateMsgEraseHandler;
            break;

         case STATE_MSG_SETTINGS:      
            fcnPtr = StateMsgSettingsHandler;
            break;
      
         case STATE_MS_VMNUM:          
            fcnPtr = StateMSVoiceMailNumberHandler;
            break;

         case STATE_MS_VMNUM_EDIT:     
            fcnPtr = StateMSVoiceMailNumberEditHandler;
            break;

         case STATE_MS_ALERT:          
            fcnPtr = StateMSAlertHandler;
            break;

         case STATE_MSG_ARRIVED:       
            fcnPtr = StateMsgArrivedHandler;
            break;

         case STATE_MSG_REJECTED:      
            fcnPtr = StateMsgRejectedHandler;
            break;

         case STATE_FULL_INBOX:        
            fcnPtr = StateNoOperationHandler;
            break;

         case STATE_THRESHOLD_WARN:    
            fcnPtr = StateThresholdWarningHandler;
            break;

         case STATE_SEND_PREFORMAT:    
            fcnPtr = StateSendPreformatHandler;
            break;

         case STATE_EXIT:              
            fcnPtr = StateExitHandler;
            break;

         case STATE_TEST:              
            fcnPtr = StateTestHandler;
            break;

         default:              
            ASSERT_NOT_REACHABLE;
            fcnPtr = StateNoOperationHandler;
            break;
      }
      retVal = fcnPtr(pMe);
   }
   return retVal;
}

//------------------------------------------------------------------------------
//   Local Function Definitions (used only in the SMSStateMachine.c file)
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: StateNoOperationHandler

DESCRIPTION: 
   Currently called in response to the STATE_NONE (or STATE_FULL_INBOX)
   state being set. This state handler is to show a screen to the user 
   indicating that (whatever :) feature is not yet implemented.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.

RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateNoOperationHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_NOT_IMPLEMENTED);
         return NFSMACTION_WAIT;
      
      // Right after the dialog is closed, we close the applet by moving to
      // STATE_EXIT.
      case DLGRET_MSGBOX_OK:
         MOVE_TO_STATE(STATE_EXIT);
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateInitHandler

DESCRIPTION: 
   Called in response to the STATE_INIT state being set.
   
   This state is the entry state into the SMS state machine engine.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateInitHandler(SMSApp *pMe)
{
   // Currently, I can not think of any thing to put under this state. But just
   // nice to have an init state from where we can (fresh) start :) so that 
   // in case if we need to add something in the future then it is going to be
   // easy.

   switch(pMe->m_dlgResult)
   {
   
   case DLGRET_OK:
   case DLGRET_CANCELED:
      MOVE_TO_STATE(STATE_EXIT); 
      return NFSMACTION_CONTINUE;

   case DLGRET_CREATE:
   default:

      MOVE_TO_STATE(STATE_MAIN);
      return NFSMACTION_CONTINUE;
      break;

   }
}

/*=============================================================================
FUNCTION: StateMainHandler

DESCRIPTION: 
   Called in response to the STATE_MAIN state being set.
   
   SMS Main state to bringup the SMS main menu to the user.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMainHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   // Resetting the item to zero so that jumping between the inbox/outbox and
   // the ruim boxes will not mess up the user highlighted menu item selection.
   pMe->m_prevMenuItemSel = 0 ; 
                  
   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
      case DLGRET_MSGBOX_OK: 
         // DLGRET_MSGBOX_OK: This is in response to the no message screen 
         // dialog. Right after we show a "no message in the inbox/outbox/ruim"
         // screen to the user and the dialog is closed, we come back here to
         // show the main screen. So, we treat it same as DLGRET_CREATE.
         
         // Unset all cache values.  There may be other times when it is
         // appropriate to do this, but it is clearly appropriate here.
         GSM1xSMSMessageStore_ResetMessageStruct(&pMe->m_msIn);
         GSM1xSMSMessageStore_ResetMessageStruct(&pMe->m_msOut);
         pMe->m_wCurrMsgID = 0;
         pMe->m_eEraseOption = ERASEOPT_NONE;
         pMe->m_bDoNotOverwriteDlgResult = FALSE;
         pMe->m_eMBoxType = SMS_MB_NONE;

         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MAIN);
         return NFSMACTION_WAIT;
      
      case DLGRET_MAIN_VM:                             
         // Check how many messages we have in the voice mail box. If we
         // don't have any messages then show "no message" screen to return to 
         // the main screen. If not, show the voice mail message.
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         if(pMe->m_cfg.m_wNumVMMsgs == 0)
         {
            // Show a no message screen to the user.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MB_NO_VOICEMAIL);
            return NFSMACTION_WAIT;
         }
         MOVE_TO_STATE(STATE_VOICEMAIL)
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MAIN_INBOX:
         // Check how many messages we have in the Inbox. If we don't have any 
         // messages then show "no message" screen to return to the main screen.
         // If not, bringup a menu including all the messages in the Inbox.
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         if(pMe->m_wNumInMsgs == 0)
         {
            // Show a no message screen to the user.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MB_NO_INBOX);
            return NFSMACTION_WAIT;
         }
         MOVE_TO_STATE(STATE_INBOX);
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MAIN_CMSG:
         // Create Message.
         pMe->m_nSendToTitleResId =  IDS_ST_SEND_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG)
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MAIN_SENDPREFORMAT:
         // Send preformed message.
         MOVE_TO_STATE(STATE_SEND_PREFORMAT)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_OUTBOX:
         // Check how many messages we have in the Outbox. If we don't have any 
         // messages then show "no message" screen to return to the main screen.
         // If not, bringup a menu including all the messages in the Outbox.
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         if(pMe->m_wNumOutMsgs == 0)
         {
            // Show a no message screen to the user.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MB_NO_OUTBOX);
            return NFSMACTION_WAIT;
         }
         MOVE_TO_STATE(STATE_OUTBOX)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_SIM:
         // If SMS is diabled in SST do not display
         if (pMe->m_bSSTDisabled)
         {
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SST_DISABLED_TEXT);
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;               
         }
         else if (pMe->m_IGSMSMSInit == FALSE)
         {
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_NOT_READY);
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;               
         }

         // Check how many messages we have in the SIM box. If we don't have 
         // any messages then show "no message" screen to return to the main 
         // screen. If not, bringup a menu including all the messages in the 
         // SIM box.
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         if(pMe->m_wNumSIMMsgs == 0)
         {
            // Show a no message screen to the user.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MB_NO_SIM);
            return NFSMACTION_WAIT;
         }
         MOVE_TO_STATE(STATE_SIM)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_ERASE:
         // Erase messages.
         MOVE_TO_STATE(STATE_ERASE_MSGS)
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MAIN_SETTINGS:
         // To change the message setting options.
         MOVE_TO_STATE(STATE_MSG_SETTINGS)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_SEND:
         // Originate a call to the voice mail number (if there is any).
         MOVE_TO_STATE(STATE_VM_CALL)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_TEST:
         // Test the SMSApp
         MOVE_TO_STATE(STATE_TEST)
         return NFSMACTION_CONTINUE;

      case DLGRET_MAIN_ABOUT:
         // Show the About screen
         GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_ABOUT_TEXT);
         return NFSMACTION_WAIT;

      case DLGRET_OK:
      case DLGRET_CANCELED:
         // About screen closed
         MOVE_TO_STATE(STATE_MAIN)
         return NFSMACTION_CONTINUE;

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateVoiceMailHandler

DESCRIPTION: 
   Called in response to the STATE_VOICEMAIL state being set.
   
   This state handler is to show the voice mail screen to the user with the
   number of voice mails pending for the user. The check for no voice mails is 
   already done in the main state.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateVoiceMailHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // At any time there can be only one voice mail message.
         // Get the first message from the voice mail box. 
         
         pMe->m_eMBoxType = SMS_MB_VOICEMAIL;
         if (GSM1xSMSMessageStore_GetMessageByID(pMe, pMe->m_eMBoxType, 0, &pMe->m_msIn, TRUE) == FALSE)
         {
            // Some thing went wrong. Transition to the main screen.
            DBGPRINTF("Voice Mail Message Read Operation Failed");
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;              
         }

         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_VOICEMAIL);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;              

      case DLGRET_VOICEMAIL_SEND:
      case DLGRET_VOICEMAIL_CALL:
         // While we are showing the voice mail message screen, if the user 
         // decides to call a number (or hit the send button) then we initiate
         // a call to the voice mail number by changing to the STATE_VM_CALL.
         // (The STATE_VM_CALL takes care of a scenario where there is no voice
         // mail number)
         MOVE_TO_STATE(STATE_VM_CALL);
         return NFSMACTION_CONTINUE;              

      case DLGRET_VOICEMAIL_RESET:
         // Reset the number of VM messages
         pMe->m_cfg.m_wNumVMMsgs = 0;

         // Update the annunciators (if needed)
         GSM1xSMSApp_UpdateAnnunciators(pMe);

         // Save number of messages to file system
         GSM1xSMSMessageStore_StorePrefs(pMe);

         // Erase the mailbox and then return to the main state.
         (void)GSM1xSMSMessageStore_EraseEntireMBox(pMe, SMS_MB_VOICEMAIL);
         pMe->m_bNeedVMRefresh = TRUE;
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;              

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateVoiceMailCallHandler

DESCRIPTION: 
   Called in response to the STATE_VM_CALL state being set.
   
   State handler to initiate a call to the voice mail number.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   Primarily, this state is set to initiate a call to the voice mail number.
   So, first we check to see whether the voice mail number is configured or 
   not. If not, then we bringup a screen prompting the user to edit the number.
   Once the number is entered, we go ahead and initiate a call. If the number
   is already configured then we skip this and initiate a call.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateVoiceMailCallHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
      {
         // Query the previously configured voicemail number from the
         // configuration database.
         AECHAR  voiceMailNum[MAX_PH_DIGITS];

         MEMCPY(&voiceMailNum[0], &pMe->m_cfg.m_szVoiceMailNum[0],
             sizeof(voiceMailNum));

         // Verify whether the number is configured or not. If yes, initiate a
         // call to the number.
         if(voiceMailNum[0] != (AECHAR)'\0')
         {
            ITAPI *pTAPI;
            char str[MAX_PH_DIGITS];

            WSTRTOSTR(voiceMailNum, &str[0], sizeof(str));

            if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_TAPI,
                                         (void **)&pTAPI))
            {
               // Initiate a call to the sender number.
               if (SUCCESS != ITAPI_MakeVoiceCall(pTAPI, str, AEECLSID_APP_GSM1XSMS))
               {
                  MOVE_TO_STATE(STATE_MAIN); 
               }
            }
            else
            {
               MOVE_TO_STATE(STATE_MAIN); 
            }
         }
         else
         {
            // No voice mail number is configured. 
            // Prompt the user to enter a voice mail number. Once he is done,
            // we need to make a call to the voice mail number.
            pMe->m_yesnoTimeout = FALSE;
            pMe->m_yesnoDialogTitle = IDS_VOICEMAIL_INFO;
            GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_YESNO);
         }
         return NFSMACTION_WAIT;
      }

      case DLGRET_CANCELED:
      case DLGRET_NO:   // If the user don't want to edit the voice mail number
                        // then we just transition to the main screen.
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;              

      case DLGRET_YES:
         // Once the number is changed, we need to initiate a call to the same.
         // We need to set this variable (m_bNeedToMakeVoiceCall) so that we 
         // can initiate a call in the STATE_MS_VMNUM_EDIT state.
         pMe->m_bNeedToMakeVoiceCall = TRUE;

         // The user decided to change the number. So, we simply transition 
         // to a state where we can show the voice mail edit box to the user.
         MOVE_TO_STATE(STATE_MS_VMNUM_EDIT);
         return NFSMACTION_CONTINUE;              

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateInBoxHandler

DESCRIPTION: 
   Called in response to the STATE_INBOX state being set.
   
   State handler invoked in response to the user selection of the 'Inbox' from 
   the SMS main menu.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateInBoxHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
          GSM1xSMSApp_UpdateMboxCounters(pMe);
		  if(pMe->m_wNumInMsgs == 0)
		  {
			  MOVE_TO_STATE(STATE_MAIN);
			  return NFSMACTION_CONTINUE;
		  }
         pMe->m_eMBoxType = SMS_MB_INBOX;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MAIL_BOX_LIST);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         // Before we go to the main state, let's check and make sure that
         // we haven't yet reached the threshold level (75% of the maximum
         // storage space). If so, then we go to STATE_THRESHOLD_WARN where we
         // show a warning screen to the user. From there we return to the 
         // main screen.

         MOVE_TO_STATE(STATE_MAIN);

         return NFSMACTION_CONTINUE;               
      
      case DLGRET_OK:
         // Get the message specified by pMe->m_wCurrMsgID and save it.
         if(GSM1xSMSMessageStore_GetMessageByID(pMe,pMe->m_eMBoxType,
            pMe->m_wCurrMsgID, &pMe->m_msIn, SET_READ))
         {
            MOVE_TO_STATE(STATE_IN_MSG);
         }
         else
         {
            // If there is any error then we return to the main state to show
            // the main SMS screen to the user.
            MOVE_TO_STATE(STATE_MAIN); 
         }
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_INBOX_SEND:
         // While we are showing the Inbox message menu, if the user hits a
         // send button then we bringup the "Reply Info" screen for the message
         // that the user selected.

         // Get the message specified by pMe->m_wCurrMsgID and save it.
         if(GSM1xSMSMessageStore_GetMessageByID(pMe,pMe->m_eMBoxType,
             pMe->m_wCurrMsgID, &pMe->m_msIn, SET_READ))
         {
            MOVE_TO_STATE(STATE_IN_REPLY_INFO);
         }
         else
         {
            // If there is any error then we return to the main state to show
            // the main SMS screen to the user.
            MOVE_TO_STATE(STATE_MAIN);
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateInMsgHandler

DESCRIPTION: 
   Called in response to the STATE_IN_MSG state being set.
   
   State handler invoked in response to the user selection of a message from 
   the 'Inbox'.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateInMsgHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_IN_MSG);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
      case DLGRET_MSGBOX_OK:
         MOVE_TO_STATE(STATE_INBOX);
         return NFSMACTION_CONTINUE;               

      case DLGRET_IN_NEXT:
         // Update the current message id with the next message and show the 
         // same dialog. if there is no next message then we bring up the inbox
         // screen.

         if(GSM1xSMSApp_UpdateCurrMsgWithNextMsg(pMe))
         {
            // Call to the GSM1xSMSApp_UpdateCurrMsgWithNextMsg() function updates
            // the current message id and also the m_msIn structure with all
            // the message information. We just have to change the state
            // so that the user can see this next message.
            MOVE_TO_STATE(STATE_IN_MSG); 
         }
         else
         {
            // If we are here, that means there are no more messages to traverse 
            // through the inbox message list. So, we return to the Inbox 
            // message list screen by setting the STATE_INBOX state.

            MOVE_TO_STATE(STATE_INBOX); 
         }
         return NFSMACTION_CONTINUE;

      case DLGRET_IN_ERASE:
         // Transition to the STATE_MSG_ERASE to start erasing the message.
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;               

      case DLGRET_IN_REPLY:
      {
         SMSMessageStruct *pMsg = NULL;

         // Make the Sender number as a default number on the
         // create message screen.
         pMe->m_msOut.m_szTo[0] = (AECHAR)'\0';
         pMe->m_msOut.m_szMessage[0] = (AECHAR)'\0';

         pMsg = MALLOC(sizeof(SMSMessageStruct));
         if (pMsg == NULL)
         {
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;               
         }

         if (pMe->m_eMBoxType != SMS_MB_MESSAGE && pMe->m_wCurrMsgID != 0xffff)
         {
             // Get the message we want to respond to
             (void)GSM1xSMSMessageStore_GetMessageByID(pMe,
                       pMe->m_eMBoxType,pMe->m_wCurrMsgID, 
                       pMsg, DONT_SET_READ);
         }
         else
         {
            MEMCPY(&pMsg->m_szFrom[0], &pMe->m_msIn.m_szFrom[0], sizeof(pMsg->m_szFrom));
            MEMCPY(&pMsg->m_szMessage[0], &pMe->m_msIn.m_szMessage[0], sizeof(pMsg->m_szMessage));
            MEMCPY(&pMsg->m_scAddr, &pMe->m_msIn.m_scAddr, sizeof(pMsg->m_scAddr));
         }

         // Add addressees from the specified InBox message "From" field to the
         // OutBox message "To" field.
         WSTRNCOPYN(pMe->m_msOut.m_szTo,
                    sizeof(pMe->m_msOut.m_szTo)/sizeof(AECHAR),
                    pMsg->m_szFrom,
                    -1);

         // The default text of the MO message is the original text of the MT 
         // message
         WSTRNCOPYN(pMe->m_msOut.m_szMessage, 
                    sizeof(pMe->m_msOut.m_szMessage)/sizeof(AECHAR),
                    pMsg->m_szMessage, 
                    -1);

         // Before we change the state, we need to initialize 
         // the data member of the SMSApp (pMe) to indicate that this a reply
         // message (this is to set a proper title of the "Create Message" 
         // text control)

         // copy the SC Address if RP bit set
         if (pMsg->m_scAddr.addr[0] != '\0')
         {
            MEMCPY(&pMe->m_msOut.m_scAddr, &pMsg->m_scAddr, sizeof(pMe->m_msOut.m_scAddr));
         }
         else
         {
            pMe->m_msOut.m_scAddr.addr[0] = '\0';
         }
         FREE(pMsg);
         pMe->m_nSendToTitleResId =  IDS_IN_REPLY_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;               

      }
      case DLGRET_IN_FORWARD:
      {
         SMSMessageStruct *pMsg = NULL;

         // While forwarding the message we don't want to default the number
         // to anything.
         pMe->m_msOut.m_szTo[0] = (AECHAR)'\0';
         pMe->m_msOut.m_szMessage[0] = (AECHAR)'\0';

         pMsg = MALLOC(sizeof(SMSMessageStruct));
         if (pMsg == NULL)
         {
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;               
         }

         // get the message we want to respond to
         (void)GSM1xSMSMessageStore_GetMessageByID(pMe,pMe->m_eMBoxType,
                   pMe->m_wCurrMsgID,pMsg,DONT_SET_READ);

         // We don't default the number and instead we just copy only the 
         // message. The default text of the MO message is the original text of 
         // the MT message
         WSTRNCOPYN(pMe->m_msOut.m_szMessage,
                    sizeof(pMe->m_msOut.m_szMessage)/sizeof(AECHAR),
                    pMsg->m_szMessage, 
                    -1);

         FREE(pMsg);
         // Before we change the state, we need to initialize 
         // the data member of the SMSApp (pMe) to indicate that this a 
         // forwarded message (this is to set a proper title of the 
         // "Create Message" text control)
         pMe->m_nSendToTitleResId =  IDS_FORWARD_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;               

      }
      case DLGRET_IN_SENDER:
         // If the user wants to call the sender then let's transition to the 
         // STATE_IN_REPLY_INFO to process the request.
         MOVE_TO_STATE(STATE_IN_REPLY_INFO);
         return NFSMACTION_CONTINUE;               

      case DLGRET_IN_MOVESIM:
         {
            uint16 count;

            // If SMS is diabled in SST do not move to SIM
            if (pMe->m_bSSTDisabled)
            {
               GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SST_DISABLED_TEXT);
               MOVE_TO_STATE(STATE_IN_MSG);
               return NFSMACTION_CONTINUE;               
            }

            // Before we move this message, let's make sure that there is enough
            // space in the SIM box to save one more message. If there is no space
            // then we warn the user that the move operation can not be performed.
            if (IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_SIM, &count) != AEE_GSMSMS_SUCCESS)
            {
                return NFSMACTION_CONTINUE;
            }
            if(pMe->m_wNumSIMMsgs < count)
            {
               // All the checks are passed. It is now time to move the message
               // from Inbox to the SIM. Update the boolean m_bMoveToSIM and
               // start moving the message.
               pMe->m_bMoveToSIM = TRUE; 
               MOVE_TO_STATE(STATE_MSG_MOVE);

               return NFSMACTION_CONTINUE;
            }
            else
            {
               // There is no space in the Inbox to move this message. 
               // We need to show a warning screen to the user that the 
               // message move can not be performed. Once this warning 
               // dialog screens ends, we return to
               // the main SIM screen.
               GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_SIM_FULL);
               return NFSMACTION_WAIT;
            }
         }

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateInReplyInfoHandler

DESCRIPTION: 
   Called in response to the STATE_IN_REPLY_INFO state being set.
   
   State handler invoked in response to the user selection of 'Reply' softkey
   from the Inbox message screen.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateInReplyInfoHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_REPLY_INFO);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         // This state can be entered three ways. The user may want to call the
         // sender by selecting a message either from the Inbox or the SIM box.
         // Or the user hit the send button while we are showing the newly 
         // arrived message on the screen.

         switch(pMe->m_prevState)
         {
            case STATE_INBOX: // user hit a send key from the inbox message list
               MOVE_TO_STATE(STATE_INBOX);
               break;
            
            case STATE_IN_MSG: // User selected a "sender" option (from Inbox)
               MOVE_TO_STATE(STATE_IN_MSG);
               break;

            case STATE_SIM_MSG: // User selected a "sender" option (from SIM)
               MOVE_TO_STATE(STATE_SIM_MSG);
               break;
            
            default:
               // In all other cases we transition to the main state.
               MOVE_TO_STATE(STATE_MAIN); 
               break;
         }
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_CALL:
         {
            ITAPI *pTAPI;

            if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_TAPI,
                                         (void **)&pTAPI))
            {
               char str[MAX_PH_DIGITS];

               WSTRTOSTR(pMe->m_strReplyInfoNum, &str[0], sizeof(str));

               // Initiate a call to the sender number.
               if (SUCCESS != ITAPI_MakeVoiceCall(pTAPI, 
                                                  str, 
                                                  AEECLSID_APP_GSM1XSMS))
               {
                  MOVE_TO_STATE(STATE_MAIN); 
               }
            }
            else
            {
               MOVE_TO_STATE(STATE_MAIN); 
            }

            return NFSMACTION_WAIT;
         }
      
      case DLGRET_REPLY:
         // Send a reply to the sender.
         WSTRNCOPYN(pMe->m_msOut.m_szTo,
                    sizeof(pMe->m_msOut.m_szTo)/sizeof(AECHAR), 
                    pMe->m_strReplyInfoNum, 
                    sizeof(pMe->m_strReplyInfoNum)/sizeof(AECHAR)); 
         MOVE_TO_STATE(STATE_CREATE_MSG_TEXT);
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}


/*=============================================================================
FUNCTION: StateCreateMsgHandler

DESCRIPTION: 
   Called in response to the STATE_CREATE_MSG state being set.
   
   State handler invoked in response to the user selection of 'Create Message'
   from the SMS main menu.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:
   If the m_pMultiEmailAddrStr pointer is not NULL then this method deallocates
   the memory allocated for this string and initializes the pointer to NULL.

SEE ALSO:

=============================================================================*/

static NextFSMAction StateCreateMsgHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
      {
         // Bring up the "Send To" screen.
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_CREATE_MSG);
         
         return NFSMACTION_WAIT;
      }

      case DLGRET_MSGBOX_OK:
         // We are done with showing the "MO SMS Not Supported" status window
         // to the user. Time to return to the SMS main menu.
         MOVE_TO_STATE(STATE_MAIN); 
         return NFSMACTION_CONTINUE;

      case DLGRET_CANCELED:
         // This state can be entered through multiple ways. The user may want 
         // to forward or send a reply back to the received incoming message. 
         // The other way is possible in cases where the user is trying to 
         // compose a new message (main menu -> create message action). Return
         // to the previous state from which we transitioned to this state. 

         switch(pMe->m_prevState)
         {
            case STATE_IN_MSG: 
               // We entered into this state when the user selected "forward"
               // or "reply" options from the inbox message menu. So, we 
               // return to the same screen.
               MOVE_TO_STATE(STATE_IN_MSG);
               break;
         
             case STATE_OUT_MSG: 
               // We entered into this state when the user selected "forward"
               // option from the outbox message menu.
               MOVE_TO_STATE(STATE_OUT_MSG);
               break;

            case STATE_SIM_MSG: 
               // We entered into this state when the user selected "forward"
               // option from the SIM message menu.
               MOVE_TO_STATE(STATE_SIM_MSG);
               break;
            
            case STATE_SEND_PREFORMAT:
               // We entered this state when the user decided to send a 
               // preformed message. For some reason, the user wants to go 
               // back to the same screen. 
               MOVE_TO_STATE(STATE_SEND_PREFORMAT);
               break;

            default:
               // In all other cases we transition to the main state.
               MOVE_TO_STATE(STATE_MAIN); 
               break;
         }
         return NFSMACTION_CONTINUE;               

      case DLGRET_CM_CONTINUE:
         // Check and make sure that the user entered some text for the outgoing
         // address. If there is no text then warn the user about it. Even 
         // after the warning, if the user decides to send this message then
         // we will advance to the next state where we prompt the user to enter
         // data.

         if(WSTRLEN(pMe->m_msOut.m_szTo) == 0)
         {
            pMe->m_yesnoTimeout = FALSE;
            pMe->m_yesnoDialogTitle = IDS_ST_NO_ADD;
            GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_YESNO);
            return NFSMACTION_WAIT;
         }

         // We simply transition to STATE_CREATE_MSG_TEXT so that we can 
         // bring up a text control to let the user enter an outgoing text
         MOVE_TO_STATE(STATE_CREATE_MSG_TEXT);

         return NFSMACTION_CONTINUE;

      case DLGRET_CM_CONTACTS:
         // FIXME Rewrite using IADDRBOOK
         //GSM1xSMSExtApp_GetAddresseeFromContactsApp(pMe);
         //IADDRBOOK_GetContact(...)

         // FIXME
         // We will set the same state again so that we can show the contact
         // number in the input text box.
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;           

      case DLGRET_YES:
         // Looks like the user decided to go ahead to send/save this message 
         // without entering any number. (For "no input address number" warning,
         // the user said "yes" !!!)
         MOVE_TO_STATE(STATE_CREATE_MSG_TEXT);
         return NFSMACTION_CONTINUE;

      case DLGRET_NO:
         // Great :) The user realized that he didn't enter any text for the 
         // outgoing number. We will bring up the same old create message
         // screen so that the user can enter the number.
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;           

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateCreateTextMsgHandler

DESCRIPTION: 
   Called in response to the STATE_CREATE_MSG_TEXT state being set.
   
   State handler invoked in response to the user selection of 'Continue' softkey
   from the 'Send To' screen.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateCreateTextMsgHandler(SMSApp *pMe)
{
   AEEDeviceInfo   di;

   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_CREATE_MSG_TEXT);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_CREATE_MSG);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;

      case DLGRET_CM_CONTINUE:
         // Mark the message as being new (i.e. unsent)
         pMe->m_msOut.m_bNew = TRUE;

         // Check and make sure that the user entered some text for the outgoing
         // message. If there is no text then warn the user about it. Even 
         // after the warning, if the user decides to send this message then
         // we will advance to the next state where we prompt the user to either
         // save or send this message.
         if(WSTRLEN(pMe->m_msOut.m_szMessage) == 0)
         {
            // In case if the user decided to save/send this message without
            // entering any text, we use the default values for the message
            // encoding(UNICODE) and language (UNSPECIFIED).
            pMe->m_yesnoTimeout = FALSE;
            pMe->m_yesnoDialogTitle = IDS_ST_NO_TEXT;
            GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_YESNO);
            return NFSMACTION_WAIT;
         }

         // We are about to send this message. It is time to set some of the 
         // key parameters of this outgoing message so that proper encoding 
         // can take place in the subsequent sections.
         ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
         if(di.dwLang == LNG_ENGLISH )
         {
            pMe->m_msOut.m_eEncoding = GSMSMS_ENCODING_7BIT;
         }
         else
         {
            pMe->m_msOut.m_eEncoding = GSMSMS_ENCODING_UCS2;
         }

         // Construct the title for an outgoing message using the helper 
         // function.
         GSM1xSMSUtil_ExtractTitle(pMe, SMS_MB_OUTBOX, &pMe->m_msOut);

         MOVE_TO_STATE(STATE_SEND_OPTIONS);
         return NFSMACTION_CONTINUE;

      case DLGRET_YES:
         // Looks like the user decided to go ahead to send this message without
         // entering any text. (For "no message text" warning, the 
         // user said "yes" !!!)
         MOVE_TO_STATE(STATE_SEND_OPTIONS);
         return NFSMACTION_CONTINUE;

      case DLGRET_NO:
         // Great :) The user realized that he didn't enter any text for the 
         // outgoing message. We will bringup the same old create message
         // text screen so that the user can enter the text.
         MOVE_TO_STATE(STATE_CREATE_MSG_TEXT);
         return NFSMACTION_CONTINUE;           

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateSendOptionsHandler

DESCRIPTION: 
   Called in response to the STATE_SEND_OPTIONS state being set.
   
   State handler invoked in response to the user selection of the 'Continue' 
   softkey from the outgoing message text screen. This is mainly to show the 
   'Message Options' screen to the user and let the user decide whether to send 
   the message or save it for later use.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateSendOptionsHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_SEND_OPTIONS);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_CREATE_MSG_TEXT);
         return NFSMACTION_CONTINUE;

      case DLGRET_OK:
         // We are done with showing a warning message to the user indicating
         // that we failed to save the messge.
                  
         // First check whether it is a natural start or not. If yes, transition
         // to the main state to show the SMS main menu.
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;

      case DLGRET_CM_SEND:
         // Before we do anything, let's check and make sure that both the
         // address and text fields are entered by the user. If at least one
         // field is present then we go ahead and process the request. Othewise
         // we fail the operation (by showing a warning screen to the user).
         if((WSTRLEN(pMe->m_msOut.m_szTo) != 0) || 
            (WSTRLEN(pMe->m_msOut.m_szMessage) != 0))
         {
            int rv;

            // At least one field is present. Time to send this message.
            rv = GSM1xSMSApp_StartSendingMessage(pMe, SMSOUTBOXOP_SAVE);
            if(rv == SUCCESS)
            {
               // So far so good. Display the message sending status screen to 
               // the user by transitioing to STATE_MSG_SEND state.
               MOVE_TO_STATE(STATE_MSG_SEND);
            }
            else
            {
               // Some thing went wrong with sending this message. Let the user
               // know about it.
               if (rv == AEE_GSMSMS_ENODEFAULTSCADDR)
               {
                  GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_NO_SC_ADDRESS);
               }
               else if (rv == AEE_GSMSMS_ENOSERVICE)
               {
                  pMe->m_wSendFailureStatus = SMS_NOT_GSM1X_MODE_FAILURE;
               }
               MOVE_TO_STATE(STATE_MSG_SENT_FAIL);
            }
            return NFSMACTION_CONTINUE;
         }
         
         // Both fields are not present. Ignore the user request.
         GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_NO_INFO_SEND_FAILED);
         return NFSMACTION_WAIT;

      case DLGRET_CM_SAVE:
         // Before we do anything, let's check and make sure that both the
         // address and text fields are entered by the user. If at least one
         // field is present then we go ahead and process the request. Othewise
         // we fail the operation (by showing a warning screen to the user).
         if((WSTRLEN(pMe->m_msOut.m_szTo) != 0) || 
            (WSTRLEN(pMe->m_msOut.m_szMessage) != 0))
         {


            // At least one field is present. Start processing the user request.
            // Save the message to the outbox and check to make sure that the 
            // save operation went through fine. If not, we will warn the user 
            // about the failure.
             switch(GSM1xSMSApp_SaveMessageToOutbox(pMe))
             {
                case AEE_GSMSMS_SUCCESS:
                    GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MSG_SAVE_SUCCESS);
                    break;
                case AEE_GSMSMS_ENODEFAULTSCADDR:
                    GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_NO_SC_ADDRESS);
                    break;
                default:
                    GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_SAVE_FAILED);
                    break;
             }
         }
         else
         {
            // Both fields are not present. Ignore the user request.
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_NO_INFO_SAVE_FAILED);
         }
         
         return NFSMACTION_WAIT;

      case DLGRET_MSGBOX_OK:
         // We are done with showing a successful completion of the save 
         // operation status to the user. 
         
         // First check whether it is a natural start or not. If yes, transition
         // to the main state to show the SMS main menu.
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgSendHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_SEND state being set.
   
   State handler invoked in response to the user selecting 'Send' from the 
   'Message Options' screen.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   This state transition is made right after we started sending the message.
   
   Once we are in this state and started showing this status message screen to
   the user, there is nothing else we can do. We just wait for the notification
   to come from the phone that changes the SMS state and closes this dialog.
   Until then we show the same status message to the user. However, we give an
   option to the user to end this call any time by clicking on the "END "
   softkey :)

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgSendHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         pMe->m_bSendingMessage = TRUE;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_SENT_RECEIVE_STATUS);
         return NFSMACTION_WAIT;
      
      case DLGRET_END:
      case DLGRET_CANCELED:
         // Looks like the user wants to end this call. 
         CALLBACK_Cancel(&pMe->m_IGSMSMSCallback);
         (void)IPHONE_EndCall(pMe->m_pIPhone);
         
         // In order to make the state transition, first check whether it is a 
         // natural start or not. If yes, transition to the main state to show 
         // the SMS main menu. If not, close this applet by moving to STATE_EXIT
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateOutboxHandler

DESCRIPTION: 
   Called in response to the STATE_OUTBOX state being set.
   
   State handler invoked in response to the user selecting 'Outbox' from the 
   SMS main menu.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateOutboxHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         pMe->m_eMBoxType = SMS_MB_OUTBOX;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MAIL_BOX_LIST);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_OK:
         // Get the message specified by pMe->m_wCurrMsgID and save it.
         if(GSM1xSMSMessageStore_GetMessageByID(pMe,pMe->m_eMBoxType,
             pMe->m_wCurrMsgID, &pMe->m_msOut,DONT_SET_READ))
         {
            // Save the message ID, should the user choose to send it
            pMe->m_wOutgoingMessageID = pMe->m_wCurrMsgID;

            MOVE_TO_STATE(STATE_OUT_MSG);
         }
         else
         {
            // If there is any error then we return to the main state to show
            // the main SMS screen to the user.
            MOVE_TO_STATE(STATE_MAIN);
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateOutMsgHandler

DESCRIPTION: 
   Called in response to the STATE_OUT_MSG state being set.
   
   State handler invoked in response to the user selection of a message from the
   Outbox list.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateOutMsgHandler(SMSApp *pMe)
{
   int result = SUCCESS;
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_OUT_MSG);
         return NFSMACTION_WAIT;
      
      case DLGRET_OUT_DONE:
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_OUTBOX);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_OUT_SEND:
         // Send this message right away.

         result = GSM1xSMSApp_StartSendingMessage(pMe,SMSOUTBOXOP_DONT_SAVE);
         if(result == SUCCESS)
         {
            // So far so good. Display the message sending status screen to the
            // user by transitioing to STATE_MSG_SEND state.
            MOVE_TO_STATE(STATE_MSG_SEND);
         }
         else
         {
            // Some thing went wrong with sending this message. Let the user
            // know about it.
            if (result == AEE_GSMSMS_ENODEFAULTSCADDR)
            {
               GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_MSG_NO_SC_ADDRESS);
               MOVE_TO_STATE(STATE_OUT_MSG);
               return NFSMACTION_CONTINUE;
            }
            else if (result == AEE_GSMSMS_ENOSERVICE)
            {
               pMe->m_wSendFailureStatus = SMS_NOT_GSM1X_MODE_FAILURE;
            }
            MOVE_TO_STATE(STATE_MSG_SENT_FAIL);
         }
         return NFSMACTION_CONTINUE;      

      case DLGRET_OUT_FORWARD:
         // Jump to the "Create Message" screen.
         // Before we change the state, we need to initialize 
         // the data member of the SMSApp (pMe) to indicate that this a 
         // forwarded message (this is to set a proper title of the 
         // "Create Message" text control)
         pMe->m_nSendToTitleResId =  IDS_FORWARD_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;               

      case DLGRET_OUT_MOVESIM:
         {
            uint16 count;

            // If SMS is diabled in SST do not move to SIM
            if (pMe->m_bSSTDisabled)
            {
               GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SST_DISABLED_TEXT);
               MOVE_TO_STATE(STATE_OUT_MSG);
               return NFSMACTION_CONTINUE;               
            }

            // Before we move this message, let's make sure that there is enough
            // space in the SIM box to save one more message. If there is no space
            // then we warn the user that the move operation can not be performed.
            if (IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_SIM, &count) != AEE_GSMSMS_SUCCESS)
            {
                return NFSMACTION_CONTINUE;
            }
            if(pMe->m_wNumSIMMsgs < count)
            {
               // All the checks are passed. It is now time to move the message
               // from Inbox to the SIM. Update the boolean m_bMoveToSIM and
               // start moving the message.
               pMe->m_bMoveToSIM = TRUE; 
               MOVE_TO_STATE(STATE_MSG_MOVE);

               return NFSMACTION_CONTINUE;
            }
            else
            {
               // There is no space in the Inbox to move this message. 
               // We need to show a warning screen to the user that the 
               // message move can not be performed. Once this warning 
               // dialog screens ends, we return to
               // the main SIM screen.
               GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_SIM_FULL);
               return NFSMACTION_WAIT;
            }
         }
	 
      case DLGRET_OUT_ERASE:
         // Looks like the user wants to erase this message. We will transition
         // to the STATE_MSG_ERASE where the message operation is performed.
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;      
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgSentOkHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_SENT_OK state being set.
   
   State handler to show a successful message sent status to the user.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   Upon receiving a successful "message sent" notification from the phone, the 
   HandleNotifyEvent function updates the state and closes the previous dialog.
   
   In this state we show the end result to the user and transition to the
   main state. 
   
   We could use the same state to show a bad status message screen (instead of
   the STATE_MSG_SENT_FAIL). But in case if we decided to do any special 
   handling for the bad status then it is going to be lot easier if we have a 
   separate state.
                  
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgSentOkHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // The message is sent successfully. Let's check to make sure that we
         // didn't fail to save the message to the outbox. If so, we need to
         // let the user know about it.
         if(pMe->m_bOutMsgSaveFailed)
         {
            // We need to show a warning screen to the user indicating that
            // we were failed to save the message (don't have enough space)
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SENT_OK_SAVE_FAILED);
         }
         else
         {
            // The message is sent successfully. Let the user know about it.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_MSG_SEND_SUCCESS);
         }
         return NFSMACTION_WAIT;
      
      case DLGRET_OK:
      case DLGRET_MSGBOX_OK:
         // We are done with showing the status to the user.
         // First check whether it is a natural start or not. If yes, transition
         // to the main state to show the SMS main menu.
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgSentFailHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_SENT_FAIL state being set.
   
   State handler to show a failed message sent operation status to the user.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   Upon receiving a failed "message sent" notification from the phone, the 
   HandleNotifyEvent function updates the state and closes the previous dialog.
   
   In this state we show the end result (fail) to the user and transition to the
   main state. 

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgSentFailHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_SEND_FAILURE);
         return NFSMACTION_WAIT;
      
      case DLGRET_MSGBOX_OK:
         // We are done with showing the "send failed" status to the user. 
         // First check whether it is a natural start or not. If yes, transition
         // to the main state to show the SMS main menu.
         if(pMe->m_bNaturalStart)
         {
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            // This is not a natural start. Simply close the applet by moving
            // to the state STATE_EXIT
            MOVE_TO_STATE(STATE_EXIT); 
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}


/*=============================================================================
FUNCTION: StateSIMHandler

DESCRIPTION: 
   Called in response to the STATE_SIM state being set.
   
   State handler invoked in response to the user selection of 'SIM' from the 
   SMS main menu.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateSIMHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         pMe->m_eMBoxType = SMS_MB_SIM;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MAIL_BOX_LIST);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_OK:
         // Get the message specified by pMe->m_wCurrMsgID and save it.
         if(GSM1xSMSMessageStore_GetMessageByID(pMe,pMe->m_eMBoxType,
             pMe->m_wCurrMsgID, &pMe->m_msIn, SET_READ))
         {
            MOVE_TO_STATE(STATE_SIM_MSG);
         }
         else
         {
            // If there is any error then we return to the main state to show
            // the main SMS screen to the user.
            MOVE_TO_STATE(STATE_MAIN); 
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateSIMMsgHandler

DESCRIPTION: 
   Called in response to the STATE_SIM_MSG state being set.
   
   State handler invoked in response to the user selection of a message from 
   the R-UIM box message list.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateSIMMsgHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_SIM_MSG);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
      case DLGRET_SIM_NEXT:
		 // Update the current message id with the next message and show the 
         // same dialog. if there is no next message then we bring up the inbox
         // screen.

         if(GSM1xSMSApp_UpdateCurrMsgWithNextMsg(pMe))
         {
            // Call to the GSM1xSMSApp_UpdateCurrMsgWithNextMsg() function updates
            // the current message id and also the m_msIn structure with all
            // the message information. We just have to change the state
            // so that the user can see this next message.
            MOVE_TO_STATE(STATE_SIM_MSG); 
         }
         else
         {
            // If we are here, that means there are no more messages to traverse 
            // through the inbox message list. So, we return to the Inbox 
            // message list screen by setting the STATE_INBOX state.

            MOVE_TO_STATE(STATE_SIM); 
         }
         return NFSMACTION_CONTINUE;

      case DLGRET_MSGBOX_OK:
         MOVE_TO_STATE(STATE_SIM);
         return NFSMACTION_CONTINUE;               

      case DLGRET_SIM_ERASE:
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;               

      case DLGRET_SIM_SENDER:
         // Save the message ID, should the user choose to send it
         pMe->m_wOutgoingMessageID = pMe->m_wCurrMsgID;

         MOVE_TO_STATE(STATE_IN_REPLY_INFO);
         return NFSMACTION_CONTINUE;  
		 
	  case DLGRET_SIM_REPLY:
      {

         // Make the Sender number as a default number on the
         // create message screen.
         pMe->m_msOut.m_szTo[0] = (AECHAR)'\0';
         pMe->m_msOut.m_szMessage[0] = (AECHAR)'\0';


         // Add addressees from the specified InBox message "From" field to the
         // OutBox message "To" field.
         WSTRNCOPYN(pMe->m_msOut.m_szTo,
                    sizeof(pMe->m_msOut.m_szTo)/sizeof(AECHAR),
                    pMe->m_msIn.m_szFrom,
                    -1);

         // The default text of the MO message is the original text of the MT 
         // message
         WSTRNCOPYN(pMe->m_msOut.m_szMessage, 
                    sizeof(pMe->m_msOut.m_szMessage)/sizeof(AECHAR),
                    pMe->m_msIn.m_szMessage, 
                    -1);

         // Before we change the state, we need to initialize 
         // the data member of the SMSApp (pMe) to indicate that this a reply
         // message (this is to set a proper title of the "Create Message" 
         // text control)

         // copy the SC Address if RP bit set
         if (pMe->m_msIn.m_scAddr.addr[0] != '\0')
         {
            MEMCPY(&pMe->m_msOut.m_scAddr, &pMe->m_msIn.m_scAddr, sizeof(pMe->m_msOut.m_scAddr));
         }
         else
         {
            pMe->m_msOut.m_scAddr.addr[0] = '\0';
         }
         pMe->m_nSendToTitleResId =  IDS_IN_REPLY_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;               

      }

      case DLGRET_SIM_FORWARD:
         // While forwarding the message we don't want to default the number
         // to anything.
         pMe->m_msOut.m_szTo[0] = (AECHAR)'\0';
         pMe->m_msOut.m_szMessage[0] = (AECHAR)'\0';

         // We don't default the number and instead we just copy only the 
         // message. The default text of the MO message is the original text of 
         // the MT message
         WSTRNCOPYN(pMe->m_msOut.m_szMessage,
                    sizeof(pMe->m_msOut.m_szMessage)/sizeof(AECHAR),
                    pMe->m_msIn.m_szMessage, 
                    -1);

         // Before we change the state, we need to initialize 
         // the data member of the SMSApp (pMe) to indicate that this a 
         // forwarded message (this is to set a proper title of the 
         // "Create Message" text control)
         pMe->m_nSendToTitleResId =  IDS_FORWARD_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE;               

      case DLGRET_SIM_MOVEIN:
       {
         uint16  numMsgs;
         int errType;

         // Before we move this message, let's make sure that there is enough
         // space in the Inbox to save one more message. If there is no space
         // then we warn the user that the move operation can not be performed.
         errType = IGSMSMS_GetStoreSize(pMe->m_pIGSMSMS, GSMSMS_STORE_NVRAM, &numMsgs);
         if (errType != AEE_GSMSMS_SUCCESS)
         {
            DBGPRINTF("IGSMSMS_GetStoreSize failed");
            return FALSE;
         }
         if(pMe->m_wNumInMsgs < numMsgs)
         {
            // We want to move a message from the SIM box to the Inbox. So, 
            // let's set a boolean to indicate the same.
            pMe->m_bMoveToSIM = FALSE; 
            MOVE_TO_STATE(STATE_MSG_MOVE);
            return NFSMACTION_CONTINUE;
         }
         else
         {
            // There is no space in the Inbox to move this message. We need to 
            // show a warning screen to the user that the message move can not
            // be performed. Once this warning dialog screens ends, we return to
            // the main SIM screen.
            GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_IN_FULL);
            return NFSMACTION_WAIT;
         }
       }
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgMoveHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_MOVE state being set.
   
   SMS state to handle a message move operation from Inbox -> SIM and 
   vice versa.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgMoveHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // We take care of the message move operation in the dialog handler.
         // Also, right after the move operation is complete, we show a status
         // screen to the user indicating that the operation is complete.
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MSG_MOVE);
         return NFSMACTION_WAIT;

      case DLGRET_OK:
         // The message move operation is complete. Now it is time to return 
         // to either the Inbox screen or the SIM screen (Depending upon
         // which state invoked this method).
         
         pMe->m_bDoNotOverwriteDlgResult = TRUE;

         if(pMe->m_prevState == STATE_IN_MSG)
         {
            // The previous state is set to STATE_IN_MSG (means that we just
            // completed a message move operation from the Inbox -> SIM box).
            // So, we will return to the main Inbox screen (via the main screen)
            // This smooth state transition takes care of a scenario where we
            // don't have any more messages (otherwise, we end up in displaying
            // a blank screen to the user). 

            // The flag to avoid the dialog result overwrite is already set.
            pMe->m_dlgResult = DLGRET_MAIN_INBOX;
         }
         else
         {
            // The previous state is set to STATE_SIM_MSG (means that we just
            // completed a message move operation from the Inbox -> SIM box).
            // So, we will return to the main SIM screen (via the main screen).
            // This smooth state transition takes care of a scenario where we
            // don't have any more messages (otherwise, we end up in displaying
            // a blank screen to the user).
            
            // The flag to avoid the dialog result overwrite is already set.
            pMe->m_dlgResult = DLGRET_MAIN_SIM;
         }

         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;               

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateEraseMsgsHandler

DESCRIPTION: 
   Called in response to the STATE_ERASE_MSGS state being set.
   
   SMS State handler invoked in response to the user selection of 
   'Erase Messages' from the SMS main menu.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateEraseMsgsHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_ERASE_MSGS);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
      case DLGRET_EM_NO:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;  
      
      case DLGRET_EM_INBOX:
         // Erase the entire inbox. 
         pMe->m_eEraseOption = ERASEOPT_ENTIRE_INBOX;
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;  
      
      case DLGRET_EM_OUTBOX:
         // Erase the entire outbox.
         pMe->m_eEraseOption = ERASEOPT_ENTIRE_OUTBOX;
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;  

      case DLGRET_EM_SIM:
         // Erase the entire SIM box.
         pMe->m_eEraseOption = ERASEOPT_ENTIRE_SIM;
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;  
      
      case DLGRET_ERASE_OLD_MSGS:
         // Erase all the old inbox messages
         pMe->m_eEraseOption = ERASEOPT_OLD_INBOX_MSGS;
         MOVE_TO_STATE(STATE_MSG_ERASE);
         return NFSMACTION_CONTINUE;  
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgEraseHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_ERASE state being set.
   
   Intermediate state used by Inbox, Outbox, R-UIM and the Erase Messages to 
   remove one/many messages.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgEraseHandler(SMSApp *pMe)
{
   SMSMBoxType    eMBoxType = SMS_MB_NONE;

   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         pMe->m_yesnoTimeout = FALSE;
         
         // We need to set the "yesno" dialog title based on the users action
         // (depending upon whether the user wants to delete the entire inbox,
         // outbox, SIM box or just a single message from one of these message
         // boxes). 

         // First we check to from which state we transitioned to this state.
         if(pMe->m_prevState == STATE_ERASE_MSGS)
         {
            // This must be a call from the "EraseMessages" (SMS main screen)
            switch(pMe->m_eEraseOption)
            {
               case ERASEOPT_ENTIRE_INBOX:
                  pMe->m_yesnoDialogTitle = IDS_ERASE_INBOX;
                  break;
               
               case ERASEOPT_ENTIRE_OUTBOX:
                  pMe->m_yesnoDialogTitle = IDS_ERASE_OUTBOX;
                  break;

               case ERASEOPT_ENTIRE_SIM:
                  pMe->m_yesnoDialogTitle = IDS_ERASE_SIM;
                  break;
               
               case ERASEOPT_OLD_INBOX_MSGS:
                  pMe->m_yesnoDialogTitle = IDS_ERASE_OLD_INBOX_MSGS;
                  break;

               default:
                  // Invalid option. Let's return to the main screen.
                  MOVE_TO_STATE(STATE_MAIN);
                  return NFSMACTION_CONTINUE;               
            }
         }
         else
         {
            // This must be a call from one of the mail boxes (inbox/outbox or
            // the SIM box). We really don't care about the mailbox as they 
            // have already updated the current message id (before even this 
            // function is invoked). 
            pMe->m_yesnoDialogTitle = IDS_EM_SINGLE_CONFIRM_MSG;
         }
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_YESNO);
         return NFSMACTION_WAIT;

      case DLGRET_NO:
      case DLGRET_CANCELED:
      {
         // Return to the previous screen.
         SMSState tempState = pMe->m_prevState;
         MOVE_TO_STATE(tempState);
         return NFSMACTION_CONTINUE;
      }
      
      case DLGRET_MSGBOX_OK:
         // Switch to the previous state. If the previous state is not the
         // STATE_ERASE_MSGS then we transition to the previous state through
         // the main state (STATE_MAIN). This indirect transition takes care 
         // of a scenario where the user deleted last message from the mail
         // box (if we just simply transition to the previous state there is
         // a good potential that we may end up in showing a blank message box 
         // screen).

         switch(pMe->m_prevState)
         {
            case STATE_ERASE_MSGS:
               MOVE_TO_STATE(STATE_ERASE_MSGS);
               return NFSMACTION_CONTINUE;

            case STATE_IN_MSG:
               pMe->m_dlgResult = DLGRET_MAIN_INBOX;
               pMe->m_bDoNotOverwriteDlgResult = TRUE;
               break;

            case STATE_OUT_MSG:
               pMe->m_dlgResult = DLGRET_MAIN_OUTBOX;
               pMe->m_bDoNotOverwriteDlgResult = TRUE;
               break;

            case STATE_SIM_MSG:
               pMe->m_dlgResult = DLGRET_MAIN_SIM;
               pMe->m_bDoNotOverwriteDlgResult = TRUE;
               break;

            default:
               DBGPRINTF("Invalid state transition from %d to STATE_MSG_ERASE state",pMe->m_prevState);
               break;               
         }

         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;

      case DLGRET_YES:
         // Set the last parameter to FALSE so that the helper function will 
         // not clear the existing screen while showing an hour glass.
         GSM1xSMSUtil_ShowBusyIcon(pMe->a.m_pIShell,pMe->a.m_pIDisplay,&pMe->m_rc, FALSE); 
         
         // The user really wants to delete a message (or entire message box).
         if(pMe->m_eMBoxType != SMS_MB_NONE)
         {
            // Erase the current message
            (void)GSM1xSMSMessageStore_EraseMessageFromMBox(pMe, pMe->m_eMBoxType,
                                                       pMe->m_wCurrMsgID);
            switch (pMe->m_eMBoxType)
            {
            case SMS_MB_INBOX:
            case SMS_MB_OUTBOX:
               pMe->m_bNeedNVRAMRefresh = TRUE;
               break;
            case SMS_MB_SIM:
               pMe->m_bNeedSIMRefresh = TRUE;
               break;
            default:
               break;
            }

            // The current message no longer exists
            pMe->m_wCurrMsgID = 0;
         }
         else
         {
            // This must be a call from the "EraseMessages" (SMS main screen)
            switch(pMe->m_eEraseOption)
            {
               case ERASEOPT_ENTIRE_INBOX:
                  eMBoxType = SMS_MB_INBOX;
                  pMe->m_bNeedNVRAMRefresh = TRUE;
                  break;
               
               case ERASEOPT_ENTIRE_OUTBOX:
                  eMBoxType = SMS_MB_OUTBOX;
                  pMe->m_bNeedNVRAMRefresh = TRUE;
                  break;

               case ERASEOPT_ENTIRE_SIM:
                  eMBoxType = SMS_MB_SIM;
                  pMe->m_bNeedSIMRefresh = TRUE;
                  break;
               
               case ERASEOPT_OLD_INBOX_MSGS:
                  pMe->m_bNeedNVRAMRefresh = TRUE;
                  break;

               default:
                  // Invalid option. Let's return to the main screen.
                  MOVE_TO_STATE(STATE_MAIN);
                  return NFSMACTION_CONTINUE;               
            }

            if(pMe->m_eEraseOption == ERASEOPT_OLD_INBOX_MSGS)
            {
               // Looks like the user wants to erase all old messages from the
               // Inbox.
               GSM1xSMSApp_EraseOldMessagesFromInbox(pMe, TRUE);
            }
            else
            {
               // Erase the entire mailbox
               (void)GSM1xSMSMessageStore_EraseEntireMBox(pMe, eMBoxType);
            }
         }

         // Update the annunciators (if needed)
         GSM1xSMSApp_UpdateAnnunciators(pMe);

         // Show an informative message to the user indicating that the message
         // erase operation is complete.
         GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_EM_ERASE_DONE_MSG);
         return NFSMACTION_WAIT;

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgSettingsHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_SETTINGS state being set.
   
   State handler invoked in response to the user selection of 'Message Settings'
   from the SMS main menu.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgSettingsHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MS);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_MS_VMNUM:
         // User wants to view/change the voice mail number.
         MOVE_TO_STATE(STATE_MS_VMNUM);
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MS_ALERT:
         // User wants to view/change the incoming message alert settings.
         MOVE_TO_STATE(STATE_MS_ALERT);
         return NFSMACTION_CONTINUE;

      case DLGRET_MS_EDIT_SC_ADDR:
         // If SMS is diabled in SST do not display
         if (pMe->m_bSSTDisabled)
         {
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SST_DISABLED_TEXT);
            MOVE_TO_STATE(STATE_MSG_SETTINGS);
            return NFSMACTION_CONTINUE;               
         }
         // To change the message setting options.
         MOVE_TO_STATE(STATE_EDIT_SC_ADDR)
         return NFSMACTION_CONTINUE;

      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMSVoiceMailNumberHandler

DESCRIPTION: 
   Called in response to the STATE_MS_VMNUM state being set.
   
   State handler invoked in response to the user selection of Voice Mail Number
   menu item from the 'Message Settings' screen.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMSVoiceMailNumberHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MS_VMNUM);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
      case DLGRET_MS_VMNUM_OK:
         MOVE_TO_STATE(STATE_MSG_SETTINGS);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_MS_VMNUM_EDIT:
         // If user wants to change the number then transition to the next
         // state where we allow him/her to do the same.
         MOVE_TO_STATE(STATE_MS_VMNUM_EDIT);
         return NFSMACTION_CONTINUE;
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMSVoiceMailNumberEditHandler

DESCRIPTION: 
   Calle in response to the STATE_MS_VMNUM_EDIT state being set.
   
   State handler invoked in response to the user selection of 'Edit' softkey 
   from the 'Voice Mail Number' screen.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   In this state, not only we allow the user to edit/add a voice mail number.
   We also check to see from where we transitioned to this state. If we 
   moved to this state from the "voice mail" option (STATE_VOICEMAIL or 
   STATE_MAIN) then we initiate a call to the voice mail number.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static NextFSMAction StateMSVoiceMailNumberEditHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MS_VMNUM_EDIT);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MS_VMNUM);
         return NFSMACTION_CONTINUE;
      
      case DLGRET_MS_VMNUM_EDIT_OK:
         // The dialog handler already updated the new voicemail number in the
         // configuration database. 
         
         // See how the user entered into this state (did he/she enter into this
         // state while editing the voicemail number or making a call to the 
         // voicemail number?).
         if(pMe->m_bNeedToMakeVoiceCall)
         {
            // This state transition is initiated from the Voice Mail menu 
            // option. Looks like the user entered a new voice mail number.
            // Let's transition back to the voice mail (call) state so that
            // we can make a call to the voice mail server.
            pMe->m_bNeedToMakeVoiceCall = FALSE;
            MOVE_TO_STATE(STATE_VM_CALL);
         }
         else
         {
            MOVE_TO_STATE(STATE_MSG_SETTINGS);
         }
         return NFSMACTION_CONTINUE;               
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMSAlertHandler

DESCRIPTION: 
   Called in response to the STATE_MS_ALERT state being set.
   
   State handler invoked in reponse to the user selection of 'Message Alert' 
   menu item from the 'Message Settings' screen.

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMSAlertHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MS_ALERT);
         return NFSMACTION_WAIT;
      
      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MSG_SETTINGS);
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_OK:
         // The dialog handler already updated the SMSApp configuration object
         // variables. All we have to do here is to save the message into
         // the configuration database.
         GSM1xSMSMessageStore_StorePrefs(pMe);
         MOVE_TO_STATE(STATE_MSG_SETTINGS);
         return NFSMACTION_CONTINUE;
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgArrivedHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_ARRIVED state being set.
   
   State handler to show an informative screen to the user indicating that a 
   new SMS message is received. 

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   Upon successfully receiving an incoming page/text/voicemail message, the
   HandleNotify function sets this state to display the message to the user.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgArrivedHandler(SMSApp *pMe)
{
   SMSState tempState;
   uint16 index;

   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MSG_ARRIVED);
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
      case DLGRET_MA_IGNORE:
         // Make the state change based on whether it is a natural start or not.
         tempState = pMe->m_prevState;
         if(pMe->m_bNaturalStart)
         {
            // The notification handler might have set this state twice (happens
            // in cases if the user didn't see the incoming message and we
            // received one more message). In case if we don't check what 
            // exactly the previous state is then there is a very good potential
            // that we may endup in showing the same screen again and again. 

            // Here is the scenario under which it may happen. The SMS applet
            // is not running and the user receives a message. The HandleNotify
            // sets the state to STATE_MSG_ARRIVED. And we start showing this
            // incoming message screen. The user is not looking at the screen 
            // and did not change the screen. User receives one more message
            // and the HandleNotify sets the state again to STATE_MSG_ARRIVED.
            // At this point both the m_prevState and the m_smsState becomes 
            // STATE_MSG_ARRIVED and moving to the m_prevState will cause 
            // the same incoming message to show up on the screen.

            // Having said all this :) we check for both the m_smsState and
            // m_prevState to make sure that they are not same. If they are same
            // then we close the applet. (In case if the user is composing any
            // message then he may lost the data. Actually, it is good to save 
            // the state in the HandleNotify and return to that state. But don't
            // want to add too much complexity to address this rare condition)

            if(pMe->m_currState == tempState)
            {
               MOVE_TO_STATE(STATE_EXIT);
            }
            else
            {
               GSM1xSMSApp_UpdateMboxCounters(pMe);
               MOVE_TO_STATE(tempState);
            }
         }
         else
         {
            MOVE_TO_STATE(STATE_EXIT);
         }
         return NFSMACTION_CONTINUE;               
      case DLGRET_MA_SAVE:
      case DLGRET_MA_VIEW:
		   // save has the same desired effect as view except that we want to
		   // save it first (this is only brought about by class 0 or class none
		   // messages that were desired to be saved)
         if(pMe->m_dlgResult == DLGRET_MA_SAVE)
         {
            int storeErr = AEE_GSMSMS_SUCCESS;

            //save the message because it is a class 0 or class none that is
            //desired to be saved
            index = 0xFFFF;
            storeErr = IGSMSMS_StoreMessage(pMe->m_pIGSMSMS, &pMe->m_decodedMsg, 
                                         GSMSMS_STORE_NVRAM, &index);

            if (storeErr == AEE_GSMSMS_ESTORE_FULL)
            {
               DBGPRINTF("GSM1x_Test:SM saved on NVRAM failed Cause NVRAM Full");
            }
            pMe->m_bNeedNVRAMRefresh = TRUE;
            pMe->m_wCurrMsgID = index;
         }
         if (!((pMe->m_wNumInMsgs+pMe->m_wNumOutMsgs) >= pMe->m_wMaxNVMsgs && 
             pMe->m_dlgResult == DLGRET_MA_VIEW && 
             (pMe->m_msIn.m_eClass == GSMSMS_MSG_CLASS_0 || 
              pMe->m_msIn.m_eClass == GSMSMS_MSG_CLASS_NONE)))
         {

            switch (pMe->m_eMBoxType)
            {
            case SMS_MB_INBOX:
            case SMS_MB_OUTBOX:
               pMe->m_bNeedNVRAMRefresh = TRUE;
               (void)IGSMSMS_SetMessageStatus(pMe->m_pIGSMSMS, 
                                           pMe->m_wCurrMsgID, 
                                           GSMSMS_STATUS_MT_READ, 
                                           GSMSMS_STORE_NVRAM);
               break;
            case SMS_MB_SIM:
               pMe->m_bNeedSIMRefresh = TRUE;
               (void)IGSMSMS_SetMessageStatus(pMe->m_pIGSMSMS, 
                                           pMe->m_wCurrMsgID, 
                                           GSMSMS_STATUS_MT_READ, 
                                           GSMSMS_STORE_SIM);
               break;
            default:
               break;
            }

         }

         if(pMe->m_eMBoxType == SMS_MB_VOICEMAIL)
         {
            // The received message is a voice mail message indicaiton. We
            // need to transition to the STATE_VM_MSG state where we can show
            // the voice mail message screen to the user.
            MOVE_TO_STATE(STATE_VOICEMAIL);
         }
         else if(pMe->m_eMBoxType == SMS_MB_SIM)
         {
            // The received message is a SIM message.
            MOVE_TO_STATE(STATE_SIM_MSG);
         }
         else
         {
            // The received message is not a voice mail message indication. It
            // must be a page/message. Let's transition to the STATE_IN_MSG 
            // so that we can show all the message information to the user.
            MOVE_TO_STATE(STATE_IN_MSG);
         }
         // Update annunciators
         GSM1xSMSApp_UpdateMboxCounters(pMe);
         return NFSMACTION_CONTINUE;             
      
      case DLGRET_MA_SEND:
         // While showing the incoming message to the user, the user hit the 
         // send key. So, depending on the type of incoming message, we either 
         // jump to the reply info screen or the voice mail screen.
         
         // But before doing anything, let's update the Current message id with
         // the received message id so that we can show the address information
         // on the screen.
         if(pMe->m_eMBoxType == SMS_MB_VOICEMAIL)
         {
            // Transition to a state where we can originate a call to the voice
            // mail number.
            MOVE_TO_STATE(STATE_VM_CALL);
         }
         else
         {
            MOVE_TO_STATE(STATE_IN_REPLY_INFO);
         }
         return NFSMACTION_CONTINUE;
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateMsgRejectedHandler

DESCRIPTION: 
   Called in response to the STATE_MSG_REJECTED state being set.
   
   State handler to show a warning screen to the user indicating that an
   incoming SMS message is rejected because of some internal failure (running
   out-of-memory or a message store operation failure are the most possible 
   causes).

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   If the AEETAPI layer experiences any problems or we failed in processing the
   received incoming message then we set this state to indicate that incoming
   message is rejected.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateMsgRejectedHandler(SMSApp *pMe)
{
   SMSState tempState;

   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // Update the pMe->m_msgBox_textId with proper warning message
         pMe->m_msgBox_textId = IDS_MSG_REJECTED_WARNING;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_ERASE_OLD_MSGS);
         return NFSMACTION_WAIT;
      
      case DLGRET_OK:
      case DLGRET_MSGBOX_OK:
      case DLGRET_CANCELED:
         // Make the state change based on whether it is a natural start or not.
         tempState = pMe->m_prevState;
         if(pMe->m_bNaturalStart)
         {
            // The notification handler might have set this state twice (happens
            // in cases if the user didn't see the warning message and we
            // received one more message). In case if we don't check what 
            // exactly the previous state is then there is a very good potential
            // that we may endup in showing the same screen again and again. 

            // Here is the scenario under which it may happen. The SMS applet
            // is not running and the user receives a message. The HandleNotify
            // sets the state to STATE_MSG_REJECTED. And we start showing this
            // warning message screen. The user is not looking at the screen 
            // and did not reset the message. User receives one more message
            // and the HandleNotify sets the state again to STATE_MSG_REJECTED.
            // At this point both the m_prevState and the m_smsState becomes 
            // STATE_MSG_REJECTED and moving to the m_prevState will cause 
            // the same warning message to show up on the screen.

            // Having said all this :) we check for both the m_smsState and
            // m_prevState to make sure that they are not same. If they are same
            // then we close the applet. (In case if the user is composing any
            // message then he may lost the data. Actually, it is good to save 
            // the state in the HandleNotify and return to that state. But don't
            // want to add too much complexity to address this rare condition)

            if(pMe->m_currState == tempState)
            {
               MOVE_TO_STATE(STATE_EXIT);
            }
            else
            {
               MOVE_TO_STATE(tempState);
            }
         }
         else
         {
            MOVE_TO_STATE(STATE_EXIT);
         }
         return NFSMACTION_CONTINUE;               
      
      case DLGRET_ERASE_OLD_MSGS:
         // Let's delete all old messages and then show a status window to the
         // user.
         GSM1xSMSApp_EraseOldMessagesFromInbox(pMe, TRUE);
         pMe->m_bNeedNVRAMRefresh = TRUE;

         // Show an informative message to the user indicating that the message
         // erase operation is complete.
         GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_EM_ERASE_DONE_MSG);
         return NFSMACTION_WAIT;
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateThresholdWarningHandler

DESCRIPTION: 
   Called in response to the STATE_THRESHOLD_WARN state being set.
   
   State handler to show an informative screen to the user indicating that the
   number of messages stored in the Inbox already reached the threshold level.
   This also gives an option to the user so that the user can delete all the old
   messages from the Inbox.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   When we are running low on the SMS storage space, the m_bSpaceShortage 
   attribute of the SMS Applet structure is set. And when the user is switching
   from the Inbox screen to the main SMS screen, we check the m_bSpaceShortage
   flag and if it is set then we transition to this state to show a warning 
   message to the user. Once the user accepts the warning, we transition to the
   STATE_MAIN.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateThresholdWarningHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // Update the pMe->m_msgBox_textId with proper warning message
         pMe->m_msgBox_textId = IDS_MSG_MEM_NEAR_CAPACITY_WARNING;
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_ERASE_OLD_MSGS);
         return NFSMACTION_WAIT;
      
      case DLGRET_OK:
      case DLGRET_MSGBOX_OK:
      case DLGRET_CANCELED:
         // We show this dialog only when we reached the threshold (75% of the 
         // maximum storage space) AND the user is exiting from the Inbox
         // and moving to the SMS main screen. Under this condition, a state
         // transition (STATE_INBOX -> STATE_MAIN) is not made and instead 
         // (STATE_INBOX -> STATE_THRESHOLD_WARN -> STATE_MAIN) will be made.
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE; 

      case DLGRET_ERASE_OLD_MSGS:
         // Let's delete all old messages and then show a status window to the
         // user.
         GSM1xSMSApp_EraseOldMessagesFromInbox(pMe, TRUE);
         pMe->m_bNeedNVRAMRefresh = TRUE;

         // Show an informative message to the user indicating that the message
         // erase operation is complete.
         GSM1xSMSDialogHandler_ShowMsgBox(pMe, IDS_EM_ERASE_DONE_MSG);
         return NFSMACTION_WAIT;
      
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateSendPreformatHandler

DESCRIPTION: 
   Called in response to the STATE_SEND_PREFORMAT state being set.
   
   State handler to show a dialog with all the preformed messages. The user can
   select a message from the list and later he/she can add a destination address
   and change the text (if needed).
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateSendPreformatHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // Show a dialog with all the preformed messages.
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_SEND_PREFORMAT);
         return NFSMACTION_WAIT;
      
      case DLGRET_OK:
         // The preformed text is alredy extracted into the destination (m_To)
         // buffer. All we have to do is to transition to the STATE_CREATE_MSG
         // state so that the user can enter a destination address.
         pMe->m_nSendToTitleResId =  IDS_ST_SEND_TO_LABEL;
         MOVE_TO_STATE(STATE_CREATE_MSG);
         return NFSMACTION_CONTINUE; 

      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE; 
         
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}


/*=============================================================================
FUNCTION: StateExitHandler

DESCRIPTION: 
   Called in response to the STATE_EXIT state being set.
   
   State handler invoked from various SMS states to gracefully close the SMS
   applet and return to the previous menu screen (it could be phone main menu 
   or Idle screen)

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:
   We transition to this state only when we want to close the applet.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static NextFSMAction StateExitHandler(SMSApp *pMe)
{
   // No matter what the dlgResult is set to, we just ignore it and close
   // the applet.

   (void) ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);

   return NFSMACTION_WAIT;
}

/*=============================================================================
FUNCTION: StateEditSCAddrHandler

DESCRIPTION: 
   Called in response to the STATE_EDIT_SC_ADDR state being set.
   
   State handler invoked in response to the user selection of 'Edit SC Addr'
   from the SMS main menu.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static NextFSMAction StateEditSCAddrHandler(SMSApp *pMe)
{
   GSMSMSAddress scAddr;

   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
      case DLGRET_OK: 
         // If SC address exists, set the contents to the text control.
         if (IGSMSMS_GetSCAddress(pMe->m_pIGSMSMS, &scAddr) != AEE_GSMSMS_SUCCESS)
         {
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_SST_DISABLED_TEXT);
            MOVE_TO_STATE(STATE_MAIN);
         }
         else
         {
            if (scAddr.addr[0] != '\0')
            {
               STRTOWSTR((char *)&scAddr.addr[0], &pMe->m_szSCAddr[0], 
                  sizeof(pMe->m_szSCAddr));
            }
            else
            {
               pMe->m_szSCAddr[0] = (AECHAR)'\0';
            }

            // Let's bring up the "Edit SC Addr" screen.
            GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_EDIT_SC_ADDR);
         }
         
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
         // This state can be entered through multiple ways. The user may want 
         // to forward or send a reply back to the received incoming message. 
         // The other way is possible in cases where the user is trying to 
         // compose a new message (main menu -> create message action). Return
         // to the previous state from which we transitioned to this state. 

         switch(pMe->m_prevState)
         {
            case STATE_IN_MSG: 
               // We entered into this state when the user selected "forward"
               // or "reply" options from the inbox message menu. So, we 
               // return to the same screen.
               MOVE_TO_STATE(STATE_IN_MSG);
               break;
         
             case STATE_OUT_MSG: 
               // We entered into this state when the user selected "forward"
               // option from the outbox message menu.
               MOVE_TO_STATE(STATE_OUT_MSG);
               break;

            case STATE_SIM_MSG: 
               // We entered into this state when the user selected "forward"
               // option from the SIM message menu.
               MOVE_TO_STATE(STATE_SIM_MSG);
               break;
            
            case STATE_SEND_PREFORMAT:
               // We entered this state when the user decided to send a 
               // preformed message. For some reason, the user wants to go 
               // back to the same screen. 
               MOVE_TO_STATE(STATE_SEND_PREFORMAT);
               break;

            default:
               // In all other cases we transition to the main state.
               MOVE_TO_STATE(STATE_MSG_SETTINGS); 
               break;
         }
         return NFSMACTION_CONTINUE;               

      case DLGRET_EDIT_SC_ADDR_CONTINUE:
      {
         GSMSMSAddress scAddr;

         // Check and make sure that the user entered some text for the SC
         // address. If there is no text then warn the user about it. 

         if(WSTRLEN(pMe->m_szSCAddr) == 0)
         {
            GSM1xSMSDialogHandler_ShowOKBox(pMe, IDS_NO_SC_ADDR_TEXT);
            return NFSMACTION_WAIT;
         }

         // Check to make sure that the user entered "+" to indicate
         // an international number
         if(pMe->m_szSCAddr[0] == '+')
         {
            scAddr.ton = GSMSMS_TON_INTERNATIONAL_NUMBER;
         }
         else
         {
            scAddr.ton = GSMSMS_TON_NATIONAL_NUMBER;
         }
         scAddr.npi = GSMSMS_NPI_ISDN_TELEPHONE;
         WSTRTOSTR(&pMe->m_szSCAddr[0], (char *)&scAddr.addr[0], sizeof(scAddr.addr));
         

         if (IGSMSMS_SetSCAddress(pMe->m_pIGSMSMS, &scAddr) != AEE_GSMSMS_SUCCESS)
         {
            // Some thing went wrong. Transition to the main screen.
            DBGPRINTF("Set SC Adddress Operation Failed");
            MOVE_TO_STATE(STATE_MAIN);
            return NFSMACTION_CONTINUE;              
         }
               
         switch(pMe->m_prevState)
         {
            case STATE_IN_MSG: 
            case STATE_OUT_MSG: 
            case STATE_SIM_MSG: 
            case STATE_SEND_PREFORMAT:
            case STATE_CREATE_MSG:
               // We entered into this state when the user selected "forward"
               // or "reply" options from the mailbox menu or from the send or
               // send preformatted message menus.
               MOVE_TO_STATE(STATE_CREATE_MSG);
               break;
         
            default:
               // In all other cases we transition to the main state.
               MOVE_TO_STATE(STATE_MAIN); 
               break;
         }
         return NFSMACTION_CONTINUE;
      }
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}


#define IEI_SPECIAL_SMS_MESSAGE_INDICATION 0x01 // GSM 3.40, 9.2.3.24

static void TestSendSMSDeliver(SMSApp *pMe, byte dcs, byte pid, boolean udhi);

/*=============================================================================
FUNCTION: StateTestHandler

DESCRIPTION: 
   Called in response to the STATE_TEST state being set.
   
   State handler invoked in response to the user selection of 'Test'
   from the SMS main menu.
   
PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure. This structure contains
              information specific to this applet.
   
RETURN VALUE:

   NFSMACTION_CONTINUE: To indicate that there are subsequent states to be 
                        followed and hence the state machine should not be 
                        stopped.
         
   NFSMACTION_WAIT: To indicate that the state machine can be suspended as we 
                    are going to show a dialog to the user.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static NextFSMAction StateTestHandler(SMSApp *pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_dlgResult)
   {
      case DLGRET_CREATE:
         // Let's bring up the "Test" screen.
         GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_TEST);
         
         return NFSMACTION_WAIT;

      case DLGRET_CANCELED:
         MOVE_TO_STATE(STATE_MAIN);
         return NFSMACTION_CONTINUE;               

      case DLGRET_TEST_CLASS0:
         // Send a Test SMS Deliver Message
         TestSendSMSDeliver(pMe, 0x10, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_CLASS1:
         // Send a Test SMS Deliver Message
         TestSendSMSDeliver(pMe, 0x11, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_CLASS2:
         // Send a Test SMS Deliver Message
         TestSendSMSDeliver(pMe, 0x12, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_CLASS_NONE:
         // Send a Test SMS Deliver Message
         TestSendSMSDeliver(pMe, 0, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_VM_PID:
         // Send a Test SMS Deliver Message (voicemail)
         TestSendSMSDeliver(pMe, 0, 0x5F, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_VM_DCS_STORE:
         // Send a Test SMS Deliver Message (voicemail)
         TestSendSMSDeliver(pMe, 0xD8, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_VM_DCS_DISCARD:
         // Send a Test SMS Deliver Message (voicemail)
         TestSendSMSDeliver(pMe, 0xC8, 0, FALSE);
         return NFSMACTION_CONTINUE;

      case DLGRET_TEST_VM_UDH:
         // Send a Test SMS Deliver Message (voicemail)
         TestSendSMSDeliver(pMe, 0, 0, TRUE);
         return NFSMACTION_CONTINUE;
      default:
         ASSERT_NOT_REACHABLE;
   }

   return NFSMACTION_WAIT;
}

static void TestSendSMSDeliver(SMSApp *pMe, byte dcs, byte pid, boolean udhi)
{
   AECHAR wstr[10];
   uint8 idx = 0;
   byte data[255];
   byte len, *pUDL;

   data[idx++] = 1;     // RP_MT_DATA;

   data[idx++] = 10;    // RP_MR
   data[idx++] = 5;     // RP-OA (len)
   data[idx++] = 0x91 ; // RP-OA (TON/NPI)
   data[idx++] = 0x55 ; // RP-OA digits
   data[idx++] = 0x55 ; // RP-OA digits
   data[idx++] = 0x55 ; // RP-OA digits
   data[idx++] = 0xF5 ; // RP-OA digits

   data[idx++] = 0 ;    // RP-DA (len)
   data[idx++] = 0;     // USER-DATA len

   data[idx++] = (uint8)(0 | (udhi << 6));     // MTI=TL_SMS_DELIVER, UDHI=udhi, MMS=0, RP=1, SRI=0

   data[idx++] = 7;     // TP-OA (len)
   data[idx++] = 0x91 ; // TP-OA (TON/NPI)
   data[idx++] = 0x55 ; // TP-OA digits
   data[idx++] = 0x55 ; // TP-OA digits
   data[idx++] = 0x55 ; // TP-OA digits
   data[idx++] = 0xF5 ; // TP-OA digits

   data[idx++] = pid;   // TP-PID 
   data[idx++] = dcs;   // TP-DCS 

   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS
   data[idx++] = 0 ;    // TP-SCTS

   // skip UDL
   pUDL = &data[idx++];

   if (udhi)
   {

      data[idx++] = 4;    // UDH len
      data[idx++] = IEI_SPECIAL_SMS_MESSAGE_INDICATION;
      data[idx++] = 2;    // IEI len
      data[idx++] = 0x80; // Store, VM Waiting
      data[idx++] = 3;    // Three messages waiting
   }

   wstr[0] = (AECHAR)'T';
   wstr[1] = (AECHAR)'S';
   wstr[2] = (AECHAR)'T';
   wstr[3] = (AECHAR)('0'+ (pMe->m_testTag++ % 10));
   wstr[4] = (AECHAR)'\0';

   if (IGSMSMS_EncodeUserData(pMe->m_pIGSMSMS, wstr, &data[idx], 
       12, GSMSMS_ENCODING_7BIT, &len) != AEE_GSMSMS_SUCCESS)
   {
      DBGPRINTF("IGSMSMS_EncodeUserData failed");
      return;
   }
   *pUDL = len+((uint8)udhi*5); // TP-UDL
   idx+=len;

   IGSMSMS_InjectMessage(pMe->m_pIGSMSMS, data, idx);
}

