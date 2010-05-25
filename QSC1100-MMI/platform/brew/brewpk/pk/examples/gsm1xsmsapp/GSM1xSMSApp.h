#ifndef GSM1XSMSApp_H
#define GSM1XSMSApp_H

/*=============================================================================
FILE: SMSApp.h

SERVICES: SMS Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
ISMSApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated. All Rights Reserved.
    QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES FOR MODULE
===========================================================================*/

#include "GSM1xSMSApp.bid"

#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEHeap.h"          // AEE Memory Allocation Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEFile.h"          // AEE File Services

#include "GSM1xPhone.h"         // AEE Phone services

#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AEEAppGen.h"        // AEEApplet Declaration
#include "AEEGSMSMS.h"        // GSM1x SMS Interface

#include "AEEConfig.h"

#include "GSM1xSMSStateMachine.h"  // SMS State Machine Services
#include "GSM1xSMSDialogHandler.h" // SMS Dialog Event Handlers

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#if 0
#define GSM1XSMSAPP_UNIT_TEST 1
#endif

#define MAX_TO_CHARS                   64       // Maximum addressee AECHARs
#define MAX_FROM_CHARS                 32       // Maximum addressee AECHARs
#define MAX_REPLYINFO_CHARS            32       // Maximum reply info AECHARs
#define MAX_TITLE_LEN                  32       // Maximum title AECHARs
#define MAX_MSG_LEN                    256      // Maximum message AECHARs
#define MAX_MSG_LEN_IN_CHARS           (MAX_MSG_LEN * sizeof(AECHAR))
#define AECHAR_512_BUF_SIZE            512      // 512 AECHAR buffer size.
#define AECHAR_512_BUF_SIZE_MINUS_ONE  (AECHAR_512_BUF_SIZE-1) 
#define AECHAR_256_BUF_SIZE            256      // 256 AECHAR buffer size.
#define AECHAR_256_BUF_SIZE_MINUS_ONE  (AECHAR_256_BUF_SIZE-1) 
#define AECHAR_128_BUF_SIZE            128      // 128 AECHAR buffer size.
#define AECHAR_64_BUF_SIZE             64       // 64 AECHAR buffer size.

#define MAX_PH_DIGITS      32       // Maximum phone number digits

#define MSG_CMD_BASE       5000     // Message id base--also used as command id
#define MAX_MSG_CMDS       1000

#define ADD_CMD_BASE       6000     // Address id base--also used as command id
#define MAX_ADD_CMDS       1000

#define SMS_CFG_VERSION       5  // Update the config version whenever new 
                                 // parameters are added to the SMSApp 
                                 // configuration structure.
#define SMS_CFG_FILENAME     "sms_prefs.dat"  // File to store preferences

// Math macros
#define ENCODE_MBOX_AND_MSG_ID(b, i)   (((b) << 16) | (i))
#define DECODE_MBOX(dw)                (((dw) >> 16) & 0x00ff)
#define DECODE_MSG_ID(dw)              ((dw) & 0x00ff)

// See the comment at the top of EVT_DIALOG_START in
// GSM1xSMSApp_HandlerMsgBoxDialogEvent() for details.
#define EVT_WAIT_FOR_REDRAW (EVT_USER+1)

// This event is sent to a dialog when the dialog timer runs out.
// (wParam = dwParam = 0)
#define EVT_DIALOG_TIMEOUT (EVT_USER+2)

// This event is used by the Send Status callback function to let the applet 
// know about the send status.
#define EVT_SEND_STATUS (EVT_USER+4)

#define EVT_CALL_STATE (EVT_USER+5)

// Send operation failure status values
#define SMS_ADDRESS_TRANSLATION_FAILURE_S    0x01
#define SMS_SMS_ORIGINATION_DENIED_S         0x61
#define SMS_SMS_NOT_SUPPORTED_S              0x64
#define SMS_SUBMIT_REPORT_FAILURE            0xED
#define SMS_NOT_GSM1X_MODE_FAILURE           0xEE
/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// A Note about the Mailboxes ...
// There are only 4 physical mailboxes created for the SMS. And they are Voice 
// Mail box, Inbox, Outbox, and the SIM box (applies if and only if the SIM
// feature is turned on). The logical mail boxes SMS_MB_PAGE, SMS_MB_MESSAGE 
// are indirectly mapped to the SMS_MB_INBOX. And the logical mail box 
// SMS_MB_SENT_OUTBOX is mapped to the SMS_MB_OUTBOX. 
typedef enum SMSMBoxType
{
   SMS_MB_NONE,         // No mailbox
   SMS_MB_VOICEMAIL,    // Voice Mail mailbox
   SMS_MB_INBOX,        // InBox
   SMS_MB_OUTBOX,       // Outbox
   SMS_MB_SENT_OUTBOX,  // Sent Outbox
   SMS_MB_PAGE,         // Pages
   SMS_MB_MESSAGE,      // Messages
   SMS_MB_SIM,          // SIM messages
   SMS_MB_MAX
} SMSMBoxType;

// Alert preferences
// This enum contains various alerts that the user can set for an incoming SMS
// messages.
typedef enum SMSAlertPref
{
   ALERT_NONE,          // None.
   ALERT_VIB_ONCE,      // Vibrate once
   ALERT_VIB_REMIND,    // Vibrate and remind
   ALERT_BEEP_ONCE,     // Beep once
   ALERT_BEEP_REMIND    // Beep and remind
} SMSAlertPref;

// SMS Message Erase options
// We let the user to choose one of these options to erase an entire inbox,
// outbox, SIM or old Inbox.
typedef enum SMSEraseOption
{
   ERASEOPT_NONE,          // None
   ERASEOPT_ENTIRE_INBOX,  // Erase the entire inbox
   ERASEOPT_ENTIRE_OUTBOX, // Erase the entire outbox
   ERASEOPT_ENTIRE_SIM,   // Erase the entire SIM box
   ERASEOPT_OLD_INBOX_MSGS // Erase all the old Inbox messages which are 
                           // already read by the user.
}SMSEraseOption;

// SMSApp configuration structure.
// This structure carries all the user configuration data (alert preferences
// for an incoming SMS messages, number of voice mail messages, and the voice 
// mail number).
typedef struct GSM1xSMSAppConfig
{
   SMSAlertPref    m_eMsgAlertPref; // Msg alert preference
   uint16          m_wNumVMMsgs;    // Num of VM messages on the system
   AECHAR          m_szVoiceMailNum[MAX_PH_DIGITS]; // Voicemail number
} GSM1xSMSAppConfig;

// SMS Applet message data structure to carry all the fields of a message stored
// in Inbox or Outbox or in the SIM.
typedef struct SMSMessageStruct
{
   AECHAR              m_szFrom[MAX_FROM_CHARS];  // Callback number
   AECHAR              m_szTo[MAX_TO_CHARS];      // Outgoing number
   AECHAR              m_szTitle[MAX_TITLE_LEN];  // Title
   AECHAR              m_szMessage[MAX_MSG_LEN];  // Message body
   uint32              m_dwTimeStamp;                  
   uint32              m_dwNumMsgs;               
   SMSMBoxType         m_dwMBox;                  
   GSMSMSAddress       m_scAddr;
   GSMSMSEncodingType  m_eEncoding;
   GSMSMSMsgClassType  m_eClass;
   boolean             m_bNew;    // Unread message (if the mailbox is Inbox)
                                  // Message not sent (if the mailbox is Outbox)
                                  // Doesn't apply to the SIM and Voice Mail box
} SMSMessageStruct;

#if defined(AEE_SIMULATOR)
#ifdef _MSC_VER
   #pragma pack(push,1)
#endif
#endif

// SMS applet class (contains all the key fields required to bring up various
// SMS screens)
typedef struct SMSApp
{
   AEEApplet      a;                            // Standard applet information
   AEERect        m_rc;                         // Rectangle for the applet
   AEEDeviceInfo  m_deviceInfo;                 // Device Information

   IGSMSMS        *m_pIGSMSMS;                 // GSMSMS
   IPhone         *m_pIPhone;                  // Phone
   IConfig        *m_pConfig;                  // IConfig interface

   // Previous applet state
   SMSState m_prevState;

   // Current applet state
   SMSState m_currState;

   // Result code returned when the active dialog closes
   DLGRetValue   m_dlgResult;

   // Pointer to the currently active dialog
   IDialog *m_activeDlg;

   // Event handler for the active dialog
   DIALOG_EVENTHANDLER m_activeDlgHandleEvent;

   // To save the menu selection for the previously active menu
   // Used to restore the menu selection while switching back to the previous
   // screen.
   uint16 m_prevMenuItemSel;
   uint16 m_mainMenuSel;

   // The resource id that the YesNo dialog box uses for it's title
   // (this should be set before creating a YesNo dialog box)
   uint16 m_yesnoDialogTitle;

   // True if the yesno dialog should timeout after YESNO_TIMEOUT (currently
   // set to 6 seconds in the SMSDialogHandler.c file) 
   boolean  m_yesnoTimeout;

   // Pointer to the ISTATIC control used by the MsgBox dialog
   IStatic *m_msgBox_IStatic;

   // String resource to use for the Msgbox text
   uint16 m_msgBox_textId;

   // True if EVT_SUSPEND as been received
   boolean m_suspending;

   // Mailbox members
   SMSMBoxType     m_eMBoxType;                    // Current mailbox type
   uint16          m_wCurrMsgID;                   // ID of curr message

   // Arrived message mailbox members
   uint16          m_wNumReceived;                 // Number of received messages thru SO6/14

   // Erasing message(s) member
   SMSEraseOption  m_eEraseOption;                 // ID of erase cmd chosen

   // Message related data members
   SMSMessageStruct m_msIn;                        // Current in message
   SMSMessageStruct m_msOut;                       // Current out message

   GSMSMSMsg        m_decodedMsg;                  // Decoded incoming message

   uint32         *m_pdwIDList;     // Current list of IDs of displayed msg list
   uint16          m_wIDListSize;   // Number of valid IDs in IDList

   // Configuration data
   GSM1xSMSAppConfig       m_cfg;             // Configuration data saved on file 
                                         // system

   // To indicate which option ("Reply To" or "Forward To" is requesting the 
   // "Send To" screen to appear on the display.
   uint16          m_nSendToTitleResId;
   uint16          m_wOutgoingMessageID;   // Record ID of outgoing
   boolean         m_bSRR;                 // Message has Status Report Request set

   // Mailbox statistics
   uint16          m_wNumInMsgs;           // Num msgs in inbox total
   uint16          m_wNumInMsgsNew;        // Num msgs in inbox that are new
   uint16          m_wNumOutMsgs;          // Num msgs in outbox total
   uint16          m_wNumSIMMsgs;          // Num msgs in SIM total
   uint16          m_wNumSIMMsgsNew;       // Num msgs in SIM that are new

   uint16          m_wMaxNVMsgs;           // Used to indicate the store size of the NVRAM
   uint16          m_wMaxSIMMsgs;          // Used to indicate the store size of the SIM

   // Buffer used by the Reply Info dialog and the correponding states.
   AECHAR          m_strReplyInfoNum[MAX_REPLYINFO_CHARS];          

   // Send failure status.
   int16          m_wSendFailureStatus; 
   int16          m_wSubmitReportFailure;     // IDS of string to display for TP-FCS value

   boolean         m_bNaturalStart;           // Is it a natural start?        
   boolean         m_bDoNotOverwriteDlgResult;// Overwrite the dialog result?
   boolean         m_bNeedToMakeVoiceCall;    // Do we need to make a voice call?
   boolean         m_bMoveToSIM;              // Move the selected msg to SIM?
   boolean         m_bSendingMessage;         // Are we currently sending a msg?
   boolean         m_bOutMsgSaveFailed;       // Out msg save operation failed?

   AECHAR          m_szSCAddr[MAX_TO_CHARS];  // Used to pass data from dialog to state handler

   AEECallback      m_IGSMSMSCallback;        // Used to indicate submit report has arrived
   AEECallback      m_SMMACallback;           // Used to indicate SMMA ACK has arrived
   GSMSMSSendReport m_IGSMSMSReport;          // Holds the submit report message
   GSMSMSSMMAReport m_SMMAReport;             // Holds the SMMA report message

   boolean          m_bStorageFull;           // Indicates NV and SIM are full

   boolean          m_bNeedNVRAMRefresh;      // Indicates Inbox/Outbox needs 
                                              // to be updated

   boolean          m_bNeedSIMRefresh;        // Indicates SIM list needs to 
                                              // be updated

   boolean          m_bNeedVMRefresh;         // Indicates VM needs to be 
                                              // updated

   boolean          m_IGSMSMSInit;            // Indicates IGSMSMS interface is ready

   boolean          m_bSSTDisabled;           // Indicates SMS access on SIM 
                                              // has been disabled 
                                              // (in SIM service table)

   boolean          m_serviceActive;          // Phone has digital service
   uint8            m_TPMR;                   // TP-MR from EF-SMSS on SIM
   boolean          m_SMMARetry;              // Flag to retry when service
                                              // becomes available
   byte             m_testTag;                // Used for unit testing

   boolean			m_bReceiving;			  // Used as an indicator for SO6/14 state

} SMSApp;

#ifdef _MSC_VER
   #pragma pack(pop)
#endif

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSApp_AddAddresseeToMenu 

DESCRIPTION:
    Add addressee from current message to the menu.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pIMenuMain [in/out] : Pointer to the Menu control.
    
RETURN VALUE:
    None

COMMENTS:
    This is broken out into its own function primarily because there is no
    way to change a menu item's bitmap.  When an addressee is selected, we
    need a way to indicate which item was selected when we make the softkeys
    menu active.  We do this by deleting the addressee from the menu and
    reading them with the appropriate bitmap.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_AddAddresseeToMenu(SMSApp *pMe, IMenuCtl *pIMenuMain);


void GSM1xSMSApp_GetAddresseeAtIndex
(
   SMSApp *pMe, 
   SMSMBoxType eMBoxType, 
   uint16 wMsgID, 
   AECHAR *pszBuf, 
   uint16 wBufSize
);

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
    int : SUCCESS to indicate that the initial send operation went through 
          without any failures.

COMMENTS:
   Depending upon the configuration (access or traffic or access/traffic), the
   message will be sent upon the access or traffic channel.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
int GSM1xSMSApp_StartSendingMessage(SMSApp *pMe, SMSOutboxOp eSaveMove);

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
void GSM1xSMSApp_DecodeAddress(const GSMSMSAddress *inAddr, AECHAR *outAddr, int len);

/*=============================================================================
FUNCTION: GSM1xSMSApp_SaveMessageToOutbox

DESCRIPTION:
    Save the current message to the OUT box.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet object

RETURN VALUE:
    SUCCESS if we succeed and the error code if we don't

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
int GSM1xSMSApp_SaveMessageToOutbox(SMSApp *pMe);

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
void GSM1xSMSApp_UpdateAnnunciators (SMSApp *pMe);

/*=============================================================================
FUNCTION: GSM1xSMSApp_UpdateMenuItemsForMailBox

DESCRIPTION:
    Show the screen which displays the contents of the current mailbox.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pIMenuMain [in/out] : Pointer to the menu control.

RETURN VALUE:
    None

COMMENTS:
    This function is shared by the dialog handlers to show messages in the 
    inbox, outbox and the SIM box.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_UpdateMenuItemsForMailBox(SMSApp *pMe, IMenuCtl *pIMenuMain);

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
boolean GSM1xSMSApp_UpdateCurrMsgWithNextMsg(SMSApp *pMe) ;

/*=============================================================================
FUNCTION: GSM1xSMSApp_MoveSIMMsg

DESCRIPTION:
   Based on the SMS Applet m_wCurrMsgID and the bToSIM field, a message 
   move operation is performed from Inbox to the SIM or vice versa.      
   
PARAMETERS:
   pMe [in]    : Pointer to the SMS Applet structure.
   bToSIM [in] : To indicate whether the move operation needs to be performed
                  from Inbox -> SIM or SIM -> Inbox.
RETURN VALUE:
   boolean : True to indicate that the store operation is success. 
             False to indicate that the store operation is failed.

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSApp_MoveSIMMsg(SMSApp *pMe, boolean bToSIM) ;

/*=============================================================================
FUNCTION: GSM1xSMSApp_EraseOldMessagesFromInbox

DESCRIPTION:
    Erases all old messages (messages which are already read by the user)
    from the Inbox.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    showBusyIcon [in] : boolean indicating whether a busy hour glass should be
                        shown to the user or not.

RETURN VALUE:
    None

COMMENTS:
    None
    
SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
void GSM1xSMSApp_EraseOldMessagesFromInbox(SMSApp *pMe, boolean showBusyIcon);

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
void GSM1xSMSApp_UpdateMboxCounters(SMSApp *pMe);
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
void GSM1xSMSApp_SMMACallBack(void * pUser);

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
void GSM1xSMSApp_CheckIfSMMARequired(SMSApp *pMe, boolean memCheck);
#endif // end GSM1XSMSApp_H
