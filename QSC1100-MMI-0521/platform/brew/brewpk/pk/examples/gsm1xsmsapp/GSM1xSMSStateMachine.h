#ifndef GSM1XSMSSTATEMACHINE_H
#define GSM1XSMSSTATEMACHINE_H

/*=============================================================================
FILE: SMSStateMachine.h

SERVICES: SMS State Machine Handlers.

GENERAL DESCRIPTION:
      This file defines all SMS state mahine handlers which are used
      by the SMSApp to bring up various SMS screens to the user. 

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated. All Rights Reserved.
    QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================

                              INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                              DEFINITIONS AND CONSTANTS

===========================================================================*/

// Check to make sure that whether we are in the receiving state 
// (STATE_MSG_RECEIVING) or not?
#define RECEIVING(pMe)           ((pMe->m_currState) == STATE_MSG_RECEIVING)

// This macro is used to update the previous state with the smsState and
// then update the smsState variable with the nextState.
#define MOVE_TO_STATE(nextState) {                                      \
                                    pMe->m_prevState = pMe->m_currState; \
                                    pMe->m_currState = nextState;        \
                                 }

/*===========================================================================

                              CLASS DEFINITIONS

===========================================================================*/

/*===========================================================================

                              TYPE DECLARATIONs

===========================================================================*/

struct SMSApp;

// SMS Applet states...
// NOTE: For each state definition added to this enum, there must be a
// corresponding state handler defined under gStateHandlers in SMSStateMachine.c
// In otherwords, this enum table must be synchronized with the gStateHandlers 
// table.
typedef enum SMSState
{
   STATE_NONE = 0,               // No state
   STATE_INIT,                   // Initial state.
   STATE_MAIN,                   // Main state
   STATE_VOICEMAIL,              // State to handle the voicemail screen
   STATE_VM_CALL,                // State to handle a call to the VM number
   STATE_INBOX,                  // Inbox state
   STATE_IN_MSG,                 // State to show a msg from the inbox
   STATE_IN_REPLY_INFO,          // State to show a reply info screen
   STATE_CREATE_MSG,             // Main create message state (allows the user
                                 // to edit "to" number.
   STATE_CREATE_MSG_TEXT,        // Create text message to allow the user to 
                                 // edit the outgoing SMS message.
   STATE_SEND_OPTIONS,           // State to handle the users send/save option.
   STATE_MSG_SEND,               // State to handle the message send operation.
   STATE_OUTBOX,                 // Outbox state
   STATE_OUT_MSG,                // State to show a message from the outbox
   STATE_MSG_SENT_OK,            // State to handle a successful msg sent case 
   STATE_MSG_SENT_FAIL,          // State to handle a send operation failure.

   STATE_SIM,                    // Main SIM state.
   STATE_SIM_MSG,                // State to show a message from the SIM box.
   STATE_MSG_MOVE,               // State to handle a message move operation
                                 // from the SIM -> Inbox and vice versa.

   STATE_ERASE_MSGS,             // State to handle the erase message operation
   STATE_MSG_ERASE,              // State to handle a single message erase 
                                 // operation
   STATE_MSG_SETTINGS,           // Message settings
   STATE_EDIT_SC_ADDR,           // Edit SC Address
   STATE_MS_VMNUM,               // State in which we show a VM number screen
                                 // to the user.
   STATE_MS_VMNUM_EDIT,          // State in which we let the user edit the 
                                 // voice mail number.
   STATE_MS_ALERT,               // State to let the user change alert settings
   STATE_MSG_ARRIVED,            // State to show the arrived incoming message
   STATE_MSG_REJECTED,           // State to show a warning screen to the user
                                 // indicating that the incoming message is 
                                 // rejected
   STATE_FULL_INBOX,             // State to show a serious warning message
                                 // to the user that the message box is full.
                                 // Currently, we don't handle this state.
                                 // We simply show a screen to the user stating
                                 // that the feature is not yet implemented.
   STATE_THRESHOLD_WARN,         // State to handle the SMS storage space 
                                 // reaching its threshold value (75% of the 
                                 // maximum value)
   STATE_SEND_PREFORMAT,         // State to send preformed messages.
   STATE_EXIT,                   // State where we close the applet.
   STATE_TEST                    // State where we test the applet.

} SMSState;

// A return value from the state handler to the main state processing function.
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSStateMachine_ProcessState

DESCRIPTION:
   Looking at the intenal static state handler table (by using the 
   pMe->m_SMSState as an index), this function routes the call to the state 
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

NextFSMAction GSM1xSMSStateMachine_ProcessState(struct SMSApp *pMe);

#endif // end GSM1XSMSSTATEMACHINE_H

