/*=============================================================================

FILE: WMSAppMenu.c

SERVICES: QCT UI WMS BREW Application.

GENERAL DESCRIPTION:
This applet is a SMS Application

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:



(c) COPYRIGHT 2002, 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsAppMenu.c#15 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   HN      Fix mem crash due to insufficient mem allocation for Alpha ID
08/29/08   HN      Fix Klocwork warnings by updating code to use new WMSAPPU_ITOA()
08/07/07   HN      Fixed Lint warnings
06/27/08   PMD     Cleaned up Feature Flags in Code
06/23/08   HN      Fixed Lint & Klocwork warnings
06/23/08   PMD     Mainlined FEATURE_CDSMS_IS637B in code
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/19/08   PMD     Fixed Lint Warnings
02/07/08   PMD     Replaced Deprecated Function Calls with newer equivalents
02/04/08   PMD     Replaced STRNCPY with STRLCPY
01/31/08   PMD     Updated Copyright Information
01/31/08   SJ      Added support for ISHELL_SendURL for calling in uiOne
01/14/07   PMD     Fixed Potential Null Dereference in TestExtendedObject()
09/29/07   PMD     Plus code dialing enhancement to only strip out first plus
08/21/07   PMD     Fixed High Severity Lint Warnings
08/21/07   PMD     Added WMSAPPMN_WmsappModeToWmsMessageMode utility
06/08/07   PMD     Fix for checking Null Ptr before using pIMenu, pISKMenu
06/06/07   PMD     Fixed Lint Errors
03/19/07   PMD     Fixed Critical Lint Warnings in WMSAPP_GetMsgListResID()
03/15/07   PMD     Fixed Memory Corruption with ITEXTCTL_GetText()
02/15/07   HQ      Fixed compiler warnings related to SMS_UDH.
01/31/07   PMD     Added Command Error String for WMS_CMD_ERR_NO_NETWORK
01/31/07   PMD     Fixed Buffer Overrun Vulnerability - GetMsgListResID()
01/17/07   HQ      Mainlined 637A.
12/04/06   PMD     Replaced all instances of STRNCPY with STRLCPY
11/28/06   PMD     Fixed Insufficient Memory Allocation causing Memory Corruption
11/20/06   PMD     Appended 1 to length for Null Character in wms_ts_unpack_ascii
11/17/06   PMD     Initialized other in client_bd structure to 0
10/31/06   PMD     Fixed Compile Warnings on 7k Targets
10/20/06   PMD     Fix for supporting EMS send from the Composer.
10/17/06   PMD     Removed check for < 10000 in WMSAPPMN_CheckNewValidServiceInfo()
09/14/06   NP      App was not resetting values correctly after sending EMS 
                   so that’s why it was including user data header in the message. 
                   Set user_data_header_present to false for GW messages 
05/03/06   PMD     Close Applet if Quitting from Alert Menu if bAlertOnly is TRUE
05/02/06   HQ      Added null pointer check in WMSAPPMN_SendCDMAReadAck().
04/27/06   HQ      Mainlined FEATURE_SMS_RETRY.
04/25/06   PMD     Fix GetMenuCtl to return for SC Address Dialogs
04/21/06   PMD     Removed Pending Queue Commands.
04/21/06   HQ      Added support for CDMA Read Ack message.
03/29/06   HKM     Fix for deleting characters
03/29/06   PMD     Fix to store number on App Suspension.
02/09/06   PMD     Fix for Limiting the Number of Characters while Composing
02/09/06   PMD     Fixed Include File wmsapp.brh case
02/06/06   PMD     Fixed Compile Warnings
01/30/06   PMD     Changed Retry Period from uint16 to uint32
01/26/06   PMD     Fixed a Compile Error arising due to a Code Merge.
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes
12/20/05   PMD     Added Support for CS Only and PS Only GW Domain Preference.
12/20/05   PMD     Added support for WmsApp.brh File
11/20/05   PMD     Initialized abs_time in InitAbsValDlg()
10/31/05   PMD     Fixed Reply displaying Junk Character.
10/20/05   PMD     Mainlined FEATURE_SMS_TEST
10/19/05   VK      EMS Featurization fixes
10/10/05   PMD     Added support for Retry Settings Dialog
                   Fixed Reject Duplicates = FALSE.
09/13/05   PMD     Fixed Test Menu Destination Address Issue.
09/13/05   PMD     Remove Continuous Send Handling for GW Mode.
09/08/05   PMD     Used OEMNV_SMS_RETRY_PERIOD for SMS Retry Period.
08/18/05   Rex     Fix compile error 
08/02/05   PMD     Setting Text to Null for Composer Screen
07/28/05   Rex     Fixed WMSAPPMN_AddSoftkeyMenu
07/26/05   Rex     Featurization fixes
07/25/05   PMD     Fix Multiple Display Issues on Raven.
06/28/05   PMD     Fixed Truncating of Test Menu Destination Address
06/06/05   Rex     Added fixes for voicemail
05/04/05   PMD     Added support for Any Update Number is Change
04/21/05   VK      Compiler Errors
04/14/05   VK      Set Memory Full - cause code for TRANSFER_ONLY messages.
04/12/05   VK      Added test menu items - Message Tests
04/12/05   VK      Added test menu - Send All - CDMA
04/11/05   Rex     Fixed cdma unicode message display
03/22/05   PMD     Added code to refresh New MT Msg Dialog
03/11/05   PMD     Inserted F_SMS_LTK for LTK Compilation
03/09/05   Rex     Fixed WMSAPPMN_SaveVoicemailToSIM
03/08/05   PMD     Fixed Compile Warning.
03/05/05   Rex     Added VMWI and Mailbox Number support
03/01/05   VK      Additional Changes to the test menu - access channel msg test
02/24/05   VK      Added Test menu support for checking access channel msg sizes
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
02/01/05   PMD     Added Support for UMTS BMC Reading Preferences
01/31/05   Rex     Fix for WMSAPPMN_StoreText
01/21/05   VK      Stop animation if any is attached to the message
01/18/05   VK      Fixed store Options in CDMA - RUIM.
01/18/05   VK      Fixed Secondary Display for MO notifications
01/17/05   VK      Fixed store Options in CDMA - only Phone Memory.
01/17/05   VK      Added Secondary Display Support
01/17/05   PMD     Replaced FREEIF calls with WMSAPPU_FREE
01/11/05   PMD     Fixed Compile Warnings
12/06/04   Rex     Fixed Message Errors
12/02/04   VK      Fixed Compiler Error
11/30/04   PMD     Removed Forcing Encoding to Octet for CDSMS UDH
                   Convert to Unicode for GSM Composer
11/17/04   VK      Unicode Support.
                   Updated the EMS test menu display.
11/16/04   PMD     Added Support for WEMT Teleservice ID.
                   Added Support for GSM 7-bit Default in CDMA DCS.
                   Added Support for DCS Encoding in GW Template Options.
11/04/04   PMD     Fixed the GW Email Text Box size.
10/28/04   PMD     Changed WMSAPPMN_InitNewMtMsg Implementation.
10/16/04   PMD     Support for seperate CDMA and GW Broadcast Message Notification.
10/16/04   PMD     Changes for WMSAPPU_DeleteBroadcastMessage prototype change
10/13/04   PMD     Changed all filenames to Lower Case
                   Release IImage pointer after use.
10/12/04   Rex     Fixed wmsapp test menu extended object msg
10/08/04   PMD     Fix to clear screen on entering Compose Dialog.
10/01/04   PMD     Fixed Freeing of a const char array in WMSAPPMN_GetDirName
09/29/04   PMD     Added Failure Displays on basis of Encode Failures
09/29/04   PMD     Release Instance of IFILEMGR to avoid Memory Leak
09/29/04   PMD     Replaced FREE with FREEIF to ensure we do not free Null Pointers
09/21/04   Rex     Added support for CPHS
09/03/04   Rex     BREW 3.1 related changes
09/01/04   Rex     Added extended object test menu
08/30/04   Rex     Fixed double occurrences of memory free
08/04/04   PMD     Modified InitCBServices() Code and checkbox handling.
07/28/04   PMD     Blank Destination Address Dialog if Email is selected.
                   Added Segment Setting to GetMenuCtl().
07/23/04   PMD     Added Segmentation Settings to Settings Dialog.
                   Setting the Use pUserData Flag to FALSE in case of Failure.
07/19/04   PMD     Cancel any pending timers in New MT Msg Dialog.
                   Change for not returning to Message Dialog.
                   Setting GW Address Max Size to 20 digits.
07/07/04   PMD     Changes for modified Function Prototype - WMSAPPMD_ConvertPicToEMS()
06/23/04   VK      Fix for returning the error messages in WMSAPPMN_MsgRead
                   to the UI.
06/22/04   Rex     Clean up potential occurrences of memory leaks
06/04/04   Rex     Merged changes for IMenuCtl
06/02/04   Rex     Changed class to msg_class to fix C++ compilation issue.
05/18/04   HQ      Fixed compilation error.
04/22/04   PMD     Use IWMS_MsgGetCacheInfo API
04/21/04   PMD     Fix for Directly reading from Broadcast Alert Screen
04/21/04   PMD     Fix for Not Highlighting Broadcast Messages on going up / down
04/08/04   PMD     Fix for display of Commands and thier Error Codes on UI
                   Fixed Memory Corruption in DisplayError() Function
                   Added support for Delete All CB Services
03/31/04   PMD/Rex Block Reading Messages from an Empty Inbox.
                   Fix for Status Report Crash due to Incorrect Feature
03/18/04   Rex     Added displaying page number / total pages for CB msg list
03/24/04   PMD     Added support for FEATURE_WMSAPP_STORAGE
                   Fixed Compile Warning
03/22/04   PMD     Added Support for Delete All Broadcast Messages
03/22/04   PMD     Modified Test App Phone Memory Handling
03/19/04   PMD     Added SMS Retry Settings Menu Item and its settings
03/16/04   AP      Added support for the user app
03/11/04   ak      Fixes message deletion problem while viewing a message
03/09/04   PMD     Changed Negative Ack to Positive Ack
                   Removed Compile Warning
03/05/04   Rex     Remove use of ASSERT statements
02/20/04   ak      Added Message Type Submit for forwarded SMS messages
                   Keep the message id for replies same as in original msg
02/14/04   Rex     Remove compile warning in code
02/14/04   PMD     Added Support for Retry Settings
02/14/04   PMD     Fixed various Display Issues with GSM Cell Broadcast
02/04/04   ak      packing/unpacking problem fixed
02/03/04   PMD     Cleaned up Lint Warnings in code
02/03/04   PMD     Handling of EVT_APP_RESUME for certain dialogs
02/03/04   PMD     Removed FEATURE_SMS_SEGMENT
02/02/04   Rex     End mode select dialog first when a mode is not ready
                   before displaying error msg
01/29/04   Rex     Changed abs_time to get relative contiguous values
                   (month, day, hour, timezone)
                   Modified AbsValDlgEventHandler for checking valid values in
                   BCD rather in integer
01/29/04   Rex     Fixed mobile stuck in mode switch between CDMA and GW
01/28/04   PMD     Added support for Alphanumeric Number Type
01/27/04   PMD     Added support for Compose from other Apps
01/26/04   PMD     Renamed and moved ITOA to WMSAPPU_ITOA
01/26/04   PMD     Added Test Menus to Segment Large Messages
01/26/04   PMD     Added New Dialog and Handling for Segmentation Settings
01/26/04   ak      Copy the correct memory store before sending the message
01/26/04   Rex     Display error msg when app is in different mode than incoming MT msg
01/13/04   HQ      Don't store MT CDMA message if it is duplicate.
01/06/04   Rex     Changed function WMSAPPMN_DisplayMsgDlg to accomodate dialog display time
12/22/03   PMD     Fixed problem of Incoming Message and Send / Store
12/11/03   PMD     Fixed Problem with Invalid Callback.
12/05/03   AP      Check reference counter to not be 0 before processing command
                   error.
12/03/03   PMD     Lint related changes for Saber
11/21/03   PMD     Added GSM Cell Broadcast Support
11/18/03   PMD     Fixed Incorrect Display of Main SMS Menu Options on Mode Switch
11/17/03   PMD     Added Support for EMS Test Menus to pick up SC Address
                   from Template
11/13/03   PMD     Added Support to directly Read from Alert Screen
                   Fixed GW Email RFC822 bugs with header addition.
11/08/03   PMD     Replaced wms functions to IWMS functions for Transparent MO
11/06/03   AP      Set service option for auto DC send.
11/06/03   Rex     Status Report Display Changes
11/05/03   PMD     Added function WMSAPPMN_CallNumber
11/04/03   PMD     Added Support for CDMA and GW Default Templates using ICFG
11/04/03   PMD     Added Support for + and * key in Voicemail Number Dialog
11/03/03   PMD     Seperated the CDMA and GW Template List and Default Template
10/31/03   PMD     Added New Alert Settings Dialog; Cleaned up Settings Dialog
10/31/03   PMD     Fixed Compile Warning for ITOA, Fixed broken Dialog Links
10/30/03   PMD     Validity Enhancements, Modifications and Cleanup
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
                   Init num_of_headers to 0 when starting composer.
10/22/03   PMD     Replaced wms_ts_* by IWMS_Ts* (Transport Service Functions)
10/06/03   PMD     Restoration of NV Template to Default Values on Deletion
10/06/03   PMD     Added 5, 10, 15 minutes in Relative Validity Dialogs
10/03/03   AP      Fixed handling of voicemail MT messages: 0 voicemails and
                   voicemail alert display.
10/06/03   PMD     Fix for Alpha ID being a Null Pointer while trying to
                   create a new template
10/03/03   PMD     Fix for PID and Message Class not being set while
                   creating / modifying a template.
10/03/03   PMD     Fix for Broadcast to be blocked in GW Mode
10/01/03   PMD     Changes to Support more than 10 predefined Animations
10/01/03   tml     Removed UI dependencies
09/25/03   PMD     Added Handling for more test menus - Clear SMMA and
                   Reset Message Reference Number
09/23/03   PMD     Changed User Data in IWMS functions to be a pointer to pMe
09/19/03   PMD     Added Handling for GW Domain Preference Dialog.
                   Changes to Notification Dialog Initialization based on
                   notify type.
09/17/03   AP      Fixed compile warnings for GW only builds.
09/16/03   PMD     Fixed Compile Errors and Warnings for GW only Builds
                   - Featurization problems
09/05/03   PMD     Added IIMAGE_Release to Free Memory in ImageDlgEventHandler
09/02/03   PMD     Fixed Number of bitmaps for Splash Screen.
08/28/03   AP      Handle transfer only route correctly for CDMA.
08/26/03   PMD     Fix for Memory store being corrupted on saving message from
                   one memory store to another
08/26/03   PMD     Fix for NV, Phone Memory showing Status Report SoftKey
08/21/03   AP      Don't display animations on notification display.
08/07/03   PMD     Put FEATURE_BREW back in the code
                   Changes for clearing the Status Report Screen on Deletion
                   Changes for going to the main menu on Reading an Incoming
                   Message from the Notification screen
                   Some more enhancements for + sign in Display Address
08/06/03   PMD     Removed featurization - FEATURE_BREW
08/06/03   PMD     Changes to Store Mode preference (CDMA/GSM) in NV using IConfig
                   Changes to BOLD Memory store on reception of new message
                   Changes to Display the Reply Address in the Dest Addr
                   Dialog the first time.
                   Changes to store GW Class 0 message even if mode = CDMA
                   Placed OEM_TM_DIALED_DIGITS and AEE_TM_DIALED_DIGITS under
                   FEATURE_BREW
08/04/03   SLY     Changed AEE_TM_DIALED_DIGITS to OEM_TM_DIALED_DIGITS
07/30/03   AP      Fixed display of address.
07/29/03   AP      Added mode select in settings.  Changed sms title to be mode
                   specific.  Added enable/disable of auto DC connect and auto
                   DC send.
07/17/03   PMD     Changes to Display + Sign for International Numbers
07/14/03   PMD     Changes to remove / suppress Lint Errors
07/11/03   PMD     Changes to Display About Screen
                   Changes to Display Splash Screen on Launch
07/09/03   AP      Don't copy SC address from the template if replying.
07/08/03   PMD     Changes to Enter and Support '+' sign for International Addresses
07/03/03   PMD     Changes to Handle wms_cmd_err_busy
07/02/03   PMD     Changes in WMSAPPMN_InitIdleMenuDlg to solve unnecessary Bold issue
                   Renamed WMSPPMN_InitIdleMenuDlg to WMSAPPMN_InitIdleMenuDlg
                   Added Soft Key and Support to Store Messages from Test Menu given
                   the number of characters
06/27/03   PMD     Changed function WMSAPPMN_DisplayMsgDlg to accomodate text
                   Changed the Alpha ID use in StoreAlphaTag
                   Changes reqd for WMSAPPMN_DisplayMsgDlg function change
06/25/03   AP      Fixed memory useage for alpha id.
06/19/03   PMD     Message Routes now being properly displayed and set
                   both for CDMA and GW. Added logic to block Mem_store
                   display if user has not selected the "Store" option
06/17/03   PMD     Set mem_store = none for Test EMS messages
06/13/03   PMD     Added work around for Template Refresh
06/11/03   AP      Fixed compile warning.
06/10/03   AP      Added null pointer check in WMSAPPMN_DisplayMsgDlg.
06/10/03   SUN     The resource ID array should be used to get the menu item
                   for the message list display. Fixed Style text creation
06/05/03   PMD     Fixed Display Problems with Animations remaining on screen
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
06/02/03   PMD     Usage of NV item for storing / retrieving Voice Mail Number
06/02/03   AP      Pass pMe when connecting a SMS call.
05/29/03   PMD     Fix for proper storing, calling and display of
                   VoiceMail Number for both GSM and CDMA
05/27/03   PMD     Added Support for Predefined Animations and User Defined Melody
                   Changed handling of Predefined Melody
                   Added MelodyDlgEventHandler for user-defined and predefined sound
05/22/03   SUN     Fixed Display of User data with Style Text
05/20/03   SUN     Changed structs to pointers in pMe
05/19/03   PMD     Support for Imelody Dialog from Compose Screen
                   Selection of Default Template with Indication on Screen
                   Animations are stopped before going to next Dialog
                   Set Routes Display Dialog
                   Support for highlighting current Template while on SoftKeyMenu
05/14/03   PMD     Fixed Destination Address display in WMSAPPMN_DisplayAddress
                   Change in TestMelody to play predef sound number 255.
05/14/03   HQ      Fixed prototype of WMSAPPMN_DisplayMsgDlg() for CDMA SMS.
05/14/03   PMD     Dest. Addr  type for Canned Messages set to Numeric
05/13/03   PMD     Fixed Destination Address Formatting issues on being
                   switched back from Compose screen.
05/12/03   PMD     Fixed Canned Message String to 7 Characters
05/12/03   PMD     Fixed Test Menu Message To Display
05/09/03   SUN     Added support for class summary, converted all strings to
                   resources,added support for GW reply, fixed style text
                   formatting, code cleanup
05/01/03   AP      Fixed handling of replying then clearing to go back to the
                   main SMS dialog.  Display brief message if trying to delete
                   card memory, but the build does not support this memory.
04/08/03   SUN     Fixed Template Handling
04/03/03   SUN/AP  Clean up.
03/26/03   AP      Fixed settings to update cfg_route to RUIM.
03/17/03   AP      Fixed warning. Clean up.
                   Deleted setting mode in idle dlg event handler.
03/12/03   AP      Fixed changing dcs preference and converting the user data
                   to the new encoding type.
03/10/03   SUN     Added support for Memory Usage
                   Fixed display for Canned Messages and Other Info
                   Fixed Message List Display
                   Fixed Teleservice option on Preferences screen
                   Fixed erase from outbox options
                   Clean up
03/06/03   AP      Fixed replying to a MT message.
03/04/03   AP      Add test menu support, fix composing and sending of messages,
                   add voice mail options.
02/27/03   AP      Clean up.
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "comdef.h"
#include "BREWVersion.h"
#include "AEEAppGen.h"        // AEEApplet structure and Applet services

#ifdef FEATURE_WMS_APP
// AEE Services used by app
#include "AEEStdLib.h"        // AEE Stb Lib Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEFile.h"          // AEE File Manager Services
#include "AEEImageCtl.h"      // AEE ImageCtl Services
#include "AEEText.h"
#include "AEEBitmap.h"
#include "msg.h"

#include "WmsappMenu.h"
#include "WmsApp.h"
#include "WmsAppUtils.h"
#include "wmsapp.brh"
#include "WmsappMedia.h"
#include "OEMCFGI.h"
#ifdef FEATURE_IMELODY
  #include "cmx.h"
#endif /*FEATURE_IMELODY*/

#include "AppComFunc.h"
#include "uixsnd.h"
#if MIN_BREW_VERSION(3,1)
#define OEM_TM_DIALED_DIGITS      AEE_TM_DIALED_DIGITS
#endif

#ifdef FEATURE_CDSMS_BROADCAST
void WMSAPPMN_BCSetAllSvcs(CWMSAPP *pMe, int language);
void WMSAPPMN_BCResetAllSvcs(CWMSAPP *pMe, int language);
boolean WMSAPPMN_InitBCServices(CWMSAPP *pMe);
#endif /* FEATURE_CDSMS_BROADCAST */

boolean WMSAPPMN_EndNDlgs(CWMSAPP *pMe, int n);
boolean WMSAPPMN_GetCheckBoxImage(IMenuCtl   *pm, uint16 listId);
boolean WMSAPPMN_InitMenuSoftKey(CWMSAPP *pMe);

#ifdef FEATURE_CDSMS
static boolean WMSAPPMN_DisableAutoDisconnect(CWMSAPP *pMe);
static boolean WMSAPPMN_EnableAutoDisconnect(CWMSAPP *pMe, uint32  timeout);
#endif /* FEATURE_CDSMS */

#define defaultTxtLen 10
#define WMS_SPLASH_ANIM_NUM_BITMAPS 6
#define SPLASH_TIME_IN_MILLISECONDS 1500

typedef struct
{
  uint16 wItemID;
  uint16 wDialogID;
} SMSNextDlgIDTable;

/* Prototype */
boolean  WMSAPPMN_EndDlg(CWMSAPP *pMe);

static const SMSNextDlgIDTable LookupNextCDMASelItemDlgID[] = {

  /* Core Menu selection */
  {IDL_SMS_VOICEMAIL,              IDD_VIEW_MESSAGE_INFO},    /*1*/
  {IDL_SIM,                        IDD_CARD_MESSAGE_ACCESS},  /*2*/
  {IDL_NV,                         IDD_NV_MESSAGE_ACCESS},    /*3*/
  {IDL_RAM,                        IDD_TEMP_MESSAGE_ACCESS},  /*4*/
  {IDL_COMPOSE,                    IDD_MESSAGE_TYPE},         /*5*/
  {IDL_SETTINGS,                   IDD_SETTINGS},             /*6*/
  {IDL_ROUTE,                      IDD_ROUTES},               /*7*/
  {IDL_MEMORY_STORE,               IDD_MEMORY_STORE},         /*8*/
  {IDL_TEMPLATES,                  IDD_TEMPLATE_LIST},        /*12*/
  {IDL_VOICEMAIL_NUM,              IDD_VOICEMAIL_NUMBER},     /*13*/
  {IDL_MSG_ROUTES,                 IDD_MEMORY_STORE},         /*14*/
  {IDL_CLASS0,                     IDD_ROUTE_SETTINGS},       /*15*/
  {IDL_CLASS1,                     IDD_ROUTE_SETTINGS},       /*16*/
  {IDL_CLASS2,                     IDD_ROUTE_SETTINGS},       /*17*/
  {IDL_CLASS3,                     IDD_ROUTE_SETTINGS},       /*18*/
  {IDL_CLASSNONE,                  IDD_ROUTE_SETTINGS},       /*19*/
  {IDL_BROADCAST,                  IDD_BC_CONFIG},            /*25*/
  {IDL_SK_OUTBOX_OPTIONS_SAVE,     IDD_MEMORY_STORE},         /*26*/
  {IDL_REJ_DUP,                    IDD_REJECT_DUP},           /*28*/
  {IDL_VALIDITY,                   IDD_VALIDITY},             /*29*/
  {IDL_REPLY_PATH,                 IDD_REPLY_PATH},           /*30*/
  {IDL_PID,                        IDD_PID},                  /*32*/
  {IDL_DCS,                        IDD_DCS},                  /*33*/
  {IDL_ENCODING,                   IDD_ENCODING_TYPES},       /*36*/
  {IDL_VALIDITY_FORMAT_RELATIVE,   IDD_REL_VALIDITY},         /*41*/
  {IDL_VALIDITY_FORMAT_ABSOLUTE,   IDD_ABS_VALIDITY},         /*42*/
  {IDL_SK_TEMPLATE_ALPHA_TAG_DONE, IDD_DESTADDR_TYPE},        /*57*/
  {IDL_GW_TMPLT_PID,               IDD_PID},                  /*62*/
  {IDL_GW_TMPLT_DCS,               IDD_DCS},                  /*63*/
  {IDL_GW_TMPLT_VALIDITY,          IDD_REL_VALIDITY},         /*64*/
  {IDL_SK_OUTBOX_OPTIONS_INFO,     IDD_OTHERINFO},            /*67*/
  {IDL_OUTBOX_TELESRV,             IDD_TELESERVICE},          /*68*/
  {IDL_OUTBOX_REPLY_OPTNS,         IDD_REPLY_OPTIONS},        /*70*/
  {IDL_OUTBOX_DCS,                 IDD_CDMA_DCS},             /*71*/
  {IDL_OUTBOX_VALID_REL,           IDD_REL_VALIDITY},         /*73*/
  {IDL_OUTBOX_VALID_ABS,           IDD_ABS_VALIDITY},         /*74*/
  {IDL_OUTBOX_DEFER_REL,           IDD_REL_VALIDITY},         /*75*/
  {IDL_OUTBOX_DEFER_ABS,           IDD_ABS_VALIDITY},         /*76*/
  {IDL_OUTBOX_PRIORITY,            IDD_PRIORITY},             /*77*/
  {IDL_OUTBOX_PRIVACY,             IDD_PRIVACY},              /*78*/
  {IDL_TEMPLATE_DCS_ALPHABET_NEXT, IDD_TEMPLATE_LIST},        /*116*/
  {IDL_OUTBOX_LANGUAGE,            IDD_LANGUAGE},             /*121*/
  {IDL_OUTBOX_CALLBACK,            IDD_MESSAGE_TYPE},         /*122*/
  {IDL_OUTBOX_DISPLAY_MODE,        IDD_DISPLAY_MODE},         /*123*/
  {IDL_OUTBOX_ALERT,               IDD_ALERT_MODE_637A},      /*124*/
  {IDL_SMALL_PICTURE,              IDD_VIEW_SMALL_PICTURES},  /*134*/
  {IDL_LARGE_PICTURE,              IDD_VIEW_LARGE_PICTURES},  /*135*/
  {IDL_SMALL_ANIMATION,            IDD_VIEW_SMALL_ANIM},      /*136*/
  {IDL_LARGE_ANIMATION,            IDD_VIEW_LARGE_ANIM},      /*137*/
  {IDL_MELODY,                     IDD_LIST_MELODY},          /*138*/
  {IDL_VAR_PICTURE,                IDD_VIEW_VAR_PICTURES},    /*139*/
  {IDL_SK_DESTADDR_DONE,           IDD_COMPOSE_MSG},          /*145*/
  {IDL_MSG_PREF,                   IDD_CDMA_OUTBOX_PREF},     /*150*/
  {IDL_MSG_OTHERINFO,              IDD_OTHERINFO},            /*152*/
  {IDL_MSG_STORE,                  IDD_MEMORY_STORE},         /*153*/
  {IDL_SK_ATTACH,                  IDD_LIST_ATTACHMENTS},     /*162*/
  {IDL_SK_NEW_TMPLT,               IDD_TEMPLATE_ALPHA_TAG},   /*168*/
  {IDL_SK_EDIT_TMPLT,              IDD_CDMA_TEMPLATE_OPTIONS},/*170*/
  {IDL_TEST_MENU,                  IDD_TEST},                 /*171*/
//  {IDL_ABOUT,                      IDD_ABOUT},                /*172*/
  {IDL_SK_INBOX_FWD,               IDD_DESTADDR},             /*187*/
  {IDL_SK_INBOX_CALL,              IDD_CALL},                 /*188*/
  {IDL_SK_INBOX_SAVE,              IDD_MEMORY_STORE},         /*189*/
  {IDL_SK_INBOX_OTHERINFO,         IDD_OTHERINFO},            /*190*/
  {IDL_SK_MESSAGE_INFO_NEXT,       IDD_OUTBOX_VIEW},          /*223*/
  {IDL_MSG_TYPE_PHONE_NUMBER,      IDD_DESTADDR},             /*227*/
  {IDL_MSG_TYPE_EMAIL_ADDRESS,     IDD_DESTADDR},             /*228*/
  {IDL_MSG_TYPE_NON_STD_EMAIL,     IDD_DESTADDR},             /*229*/
  {IDL_BC_EDIT_TABLE,              IDD_BC_EDIT_TABLE},        /*8037*/
  {IDL_C_TMPLT_TELESRV,            IDD_TELESERVICE},          /*8015*/
  {IDL_C_TMPLT_DEST_ADDR,          IDD_MESSAGE_TYPE},         /*8016*/
  {IDL_C_TMPLT_REPLY_OPTS,         IDD_REPLY_OPTIONS},        /*8017*/
  {IDL_C_TMPLT_DCS,                IDD_CDMA_DCS},             /*8018*/
  {IDL_C_TMPLT_VALID_REL,          IDD_REL_VALIDITY},         /*8019*/
  {IDL_C_TMPLT_VALID_ABS,          IDD_ABS_VALIDITY},         /*8020*/
  {IDL_C_TMPLT_DEFER_REL,          IDD_REL_VALIDITY},         /*8021*/
  {IDL_C_TMPLT_DEFER_ABS,          IDD_ABS_VALIDITY},         /*8022*/
  {IDL_C_TMPLT_PRIORITY,           IDD_PRIORITY},             /*8023*/
  {IDL_C_TMPLT_PRIVACY,            IDD_PRIVACY},              /*8024*/
  {IDL_C_TMPLT_LANGUAGE,           IDD_LANGUAGE},             /*8025*/
  {IDL_C_TMPLT_CALLBACK,           IDD_MESSAGE_TYPE},         /*8026*/
  {IDL_C_TMPLT_USER_DATA,          IDD_SIMPLE_COMPOSE},       /*8027*/
  {IDL_BC_ENGLISH,                 IDD_BC_SERVICES},          /*8045*/
  {IDL_BC_SPANISH,                 IDD_BC_SERVICES},          /*8046*/
  {IDL_BC_FRENCH,                  IDD_BC_SERVICES},          /*8047*/
  {IDL_BC_JAPANESE,                IDD_BC_SERVICES},          /*8048*/
  {IDL_SK_BC_SERVICES,             IDD_BC_SERVICES},          /*8049*/
  {IDL_SK_BC_PRIORITY,             IDD_PRIORITY},             /*8051*/
  {IDL_SK_BC_PRI,                  IDD_PRIORITY},             /*8056*/
  {IDL_SK_BC_SRV_DEL,              IDD_CONFIRM},              /*8058*/
  {IDL_SK_BC_EDIT_LABEL,           IDD_SIMPLE_COMPOSE},       /*8059*/
  {IDL_SK_BC_MAX_MSGS,             IDD_BC_MAX_MESSAGES},      /*8060*/
 // {IDL_SK_BC_ALERT_MODE,           IDD_ALERT_MODE_637A},    /*8061*/
  {IDL_SK_BC_ADD_NEW_SRV,          IDD_BC_ADD_SVC},           /*8062*/
  {IDL_ABS_VAL_YEAR,               IDD_VALIDITY_YEAR},        /*8077*/
  {IDL_ABS_VAL_MONTH,              IDD_VALIDITY_MONTH},       /*8078*/
  {IDL_ABS_VAL_DAY,                IDD_VALIDITY_DAY},         /*8079*/
  {IDL_ABS_VAL_HOUR,               IDD_VALIDITY_HOUR},        /*8080*/
  {IDL_ABS_VAL_MINUTE,             IDD_VALIDITY_MINUTE},      /*8081*/
  {IDL_C_TMPLT_ALERT_MODE,         IDD_ALERT_MODE_637A},      /*8174*/
  {IDL_C_TMPLT_DISPLAY_MODE,       IDD_DISPLAY_MODE},         /*8175*/
  {IDL_BC_TABLE_SET_PRIORITY_ALL_SVCS, IDD_PRIORITY},         /*8181*/
  {IDL_BC_KOREAN,                  IDD_BC_SERVICES},          /*8194*/
  {IDL_BC_CHINESE,                 IDD_BC_SERVICES},          /*8195*/
  {IDL_BC_HEBREW,                  IDD_BC_SERVICES},          /*8196*/
  {IDL_BC_UNSPECIFIED,             IDD_BC_SERVICES},          /*8197*/
  {IDL_BC_CONFIG_1,                IDD_BC_CONFIG},            /*8057*/
  {IDL_BC_PREF_1,                  IDD_BC_PREF},              /*8160*/
  {IDL_BC_TABLE_1,                 IDD_BC_TABLE},             /*8161*/
  {IDL_BC_CONFIG_2,                IDD_BC_CONFIG},            /*8178*/
  {IDL_BC_PREF_2,                  IDD_BC_PREF},              /*8179*/
  {IDL_BC_TABLE_2,                 IDD_BC_TABLE},             /*8180*/
  {IDL_BC_TABLE_EDIT,              IDD_BC_EDIT_TABLE},        /*8181*/
  {IDL_BC_CONFIG_3,                IDD_BC_CONFIG},            /*8186*/
  {IDL_BC_PREF_3,                  IDD_BC_PREF},              /*8187*/
  {IDL_BC_TABLE_3,                 IDD_BC_TABLE},             /*8188*/
  {IDL_SK_INBOX_REPLY,             IDD_MESSAGE_TYPE},         /*8248*/
  {IDL_EMS_TEST_MENU,              IDD_EMS_TEST},             /*8560*/
  {IDL_CDMA_TEST_MENU,             IDD_CDMA_TEST},            /*8562*/
  {IDL_SK_TEST_DONE,               IDD_CDMA_TEST},            /*8254*/
  {IDL_BC_ADD_SVC_SRV_ID,          IDD_BC_AVAILABLE_SVCS},    /*8566*/
  {IDL_BC_ADD_SVC_LANGUAGE,        IDD_LANGUAGE},             /*8567*/
  {IDL_BC_ADD_SVC_LABEL,           IDD_SIMPLE_COMPOSE},       /*8568*/
  {IDL_BC_ADD_SVC_PRIORITY,        IDD_PRIORITY},             /*8569*/
  {IDL_GW_TMPLT_ENCODING,          IDD_ENCODING_TYPES},       /*8594*/
  {IDL_PREDEF_ANIMATION,           IDD_VIEW_PREDEF_ANIM},     /*8637*/
  {IDL_USER_MELODY,                IDD_LIST_USER_MELODY},     /*8638*/
  {IDL_SK_TEST_STORE_MSG,          IDD_MEMORY_STORE},         /*8652*/
  {IDL_MODE_SELECT,                IDD_MODE_SELECT},          /*8658*/
  {IDL_REL_VAL_USER_DEFINED,       IDD_REL_VAL_USER_DEFINED}, /*8842*/
  {IDL_AUTO_MEM_STORE,             IDD_AUTO_MEM_STORE},       /*8827*/
  {IDL_ALERT_SETTINGS,             IDD_ALERT_SETTINGS},       /*8862*/
  {IDL_AUTO_DC_SEND_SO,            IDD_AUTO_DC_SEND_SO},      /*8866*/
  {IDL_SK_ABOUT,                   IDD_ABOUT},                /*8867*/
  {IDL_SK_CONTINUOUS_SEND,         IDD_INTERVAL},             /*26*/
  {IDL_SEGMENT_SETTING,            IDD_SEGMENT_SETTING},      /*8869*/
  {IDL_RETRY_SETTING,              IDD_RETRY_SETTINGS},       /*8905*/
};

static const SMSNextDlgIDTable LookupNextGWSelItemDlgID[] = {

  /* Core Menu selection */
  {IDL_SMS_VOICEMAIL,              IDD_VIEW_MESSAGE_INFO},    /*1*/
  {IDL_SIM,                        IDD_CARD_MESSAGE_ACCESS},  /*2*/
  {IDL_NV,                         IDD_NV_MESSAGE_ACCESS},    /*3*/
  {IDL_RAM,                        IDD_TEMP_MESSAGE_ACCESS},  /*4*/
  {IDL_COMPOSE,                    IDD_MESSAGE_TYPE},         /*5*/
  {IDL_SETTINGS,                   IDD_SETTINGS},             /*6*/
  {IDL_ROUTE,                      IDD_ROUTES},               /*7*/
  {IDL_MEMORY_STORE,               IDD_MEMORY_STORE},         /*8*/
  {IDL_TEMPLATES,                  IDD_TEMPLATE_LIST},        /*12*/
  {IDL_VOICEMAIL_NUM,              IDD_VOICEMAIL_NUMBER},     /*13*/
  {IDL_MSG_ROUTES,                 IDD_MSG_CLASSES},          /*14*/
  {IDL_CLASS0,                     IDD_ROUTE_SETTINGS},       /*15*/
  {IDL_CLASS1,                     IDD_ROUTE_SETTINGS},       /*16*/
  {IDL_CLASS2,                     IDD_ROUTE_SETTINGS},       /*17*/
  {IDL_CLASS3,                     IDD_ROUTE_SETTINGS},       /*18*/
  {IDL_CLASSNONE,                  IDD_ROUTE_SETTINGS},       /*19*/
  {IDL_BROADCAST,                  IDD_CB_SETTINGS},          /*25*/
  {IDL_SK_OUTBOX_OPTIONS_SAVE,     IDD_MEMORY_STORE},         /*26*/
  {IDL_REJ_DUP,                    IDD_REJECT_DUP},           /*28*/
  {IDL_VALIDITY,                   IDD_VALIDITY},             /*29*/
  {IDL_REPLY_PATH,                 IDD_REPLY_PATH},           /*30*/
  {IDL_PID,                        IDD_PID},                  /*32*/
  {IDL_DCS,                        IDD_DCS},                  /*33*/
  {IDL_ENCODING,                   IDD_ENCODING_TYPES},       /*36*/
  {IDL_VALIDITY_FORMAT_RELATIVE,   IDD_REL_VALIDITY},         /*41*/
  {IDL_VALIDITY_FORMAT_ABSOLUTE,   IDD_ABS_VALIDITY},         /*42*/
  {IDL_GW_TMPLT_ALPHA_TAG,         IDD_TEMPLATE_ALPHA_TAG},   /*59*/
  {IDL_GW_TMPLT_DEST_ADDR,         IDD_EDIT_DEST_ADDR},       /*60*/
  {IDL_GW_TMPLT_SC_ADDR,           IDD_EDIT_SC_ADDR},         /*61*/
  {IDL_GW_TMPLT_PID,               IDD_PID},                  /*62*/
  {IDL_GW_TMPLT_DCS,               IDD_DCS},                  /*63*/
  {IDL_GW_TMPLT_VALIDITY,          IDD_REL_VALIDITY},         /*64*/
  {IDL_SK_OUTBOX_OPTIONS_INFO,     IDD_OTHERINFO},            /*67*/
  {IDL_OUTBOX_TELESRV,             IDD_TELESERVICE},          /*68*/
  {IDL_OUTBOX_REPLY_OPTNS,         IDD_REPLY_OPTIONS},        /*70*/
  {IDL_OUTBOX_DCS,                 IDD_CDMA_DCS},             /*71*/
  {IDL_OUTBOX_VALID_REL,           IDD_REL_VALIDITY},         /*73*/
  {IDL_OUTBOX_VALID_ABS,           IDD_ABS_VALIDITY},         /*74*/
  {IDL_OUTBOX_DEFER_REL,           IDD_REL_VALIDITY},         /*75*/
  {IDL_OUTBOX_DEFER_ABS,           IDD_ABS_VALIDITY},         /*76*/
  {IDL_OUTBOX_PRIORITY,            IDD_PRIORITY},             /*77*/
  {IDL_OUTBOX_PRIVACY,             IDD_PRIVACY},              /*78*/
  {IDL_TEMPLATE_DCS_ALPHABET_NEXT, IDD_TEMPLATE_LIST},        /*116*/
  {IDL_OUTBOX_LANGUAGE,            IDD_LANGUAGE},             /*121*/
  {IDL_OUTBOX_CALLBACK,            IDD_MESSAGE_TYPE},         /*122*/
  {IDL_OUTBOX_DISPLAY_MODE,        IDD_DISPLAY_MODE},         /*123*/
  {IDL_OUTBOX_ALERT,               IDD_ALERT_MODE_637A},      /*124*/
  {IDL_SMALL_PICTURE,              IDD_VIEW_SMALL_PICTURES},  /*134*/
  {IDL_LARGE_PICTURE,              IDD_VIEW_LARGE_PICTURES},  /*135*/
  {IDL_SMALL_ANIMATION,            IDD_VIEW_SMALL_ANIM},      /*136*/
  {IDL_LARGE_ANIMATION,            IDD_VIEW_LARGE_ANIM},      /*137*/
  {IDL_MELODY,                     IDD_LIST_MELODY},          /*138*/
  {IDL_VAR_PICTURE,                IDD_VIEW_VAR_PICTURES},    /*139*/
  {IDL_SK_DESTADDR_DONE,           IDD_COMPOSE_MSG},          /*145*/
  {IDL_MSG_PREF,                   IDD_GW_OUTBOX_PREF},       /*150*/
  {IDL_MSG_OTHERINFO,              IDD_OTHERINFO},            /*152*/
  {IDL_MSG_STORE,                  IDD_MEMORY_STORE},         /*153*/
  {IDL_SK_ATTACH,                  IDD_LIST_ATTACHMENTS},     /*162*/
  {IDL_SK_NEW_TMPLT,               IDD_TEMPLATE_ALPHA_TAG},   /*168*/
  {IDL_SK_EDIT_TMPLT,              IDD_GW_TEMPLATE_OPTIONS},  /*170*/
  {IDL_TEST_MENU,                  IDD_TEST},                 /*171*/
//  {IDL_ABOUT,                      IDD_ABOUT},                /*172*/
  {IDL_SK_INBOX_FWD,               IDD_DESTADDR},             /*187*/
  {IDL_SK_INBOX_CALL,              IDD_CALL},                 /*188*/
  {IDL_SK_INBOX_SAVE,              IDD_MEMORY_STORE},         /*189*/
  {IDL_SK_INBOX_OTHERINFO,         IDD_OTHERINFO},            /*190*/
  {IDL_SK_MESSAGE_INFO_NEXT,       IDD_OUTBOX_VIEW},          /*223*/
  {IDL_MSG_TYPE_PHONE_NUMBER,      IDD_DESTADDR},             /*227*/
  {IDL_MSG_TYPE_EMAIL_ADDRESS,     IDD_DESTADDR},             /*228*/
  {IDL_MSG_TYPE_NON_STD_EMAIL,     IDD_DESTADDR},             /*229*/
  {IDL_BC_EDIT_TABLE,              IDD_BC_EDIT_TABLE},        /*8037*/
  {IDL_SK_BC_SERVICES,             IDD_BC_SERVICES},          /*8049*/
  {IDL_SK_BC_PRIORITY,             IDD_PRIORITY},             /*8051*/
  {IDL_ABS_VAL_YEAR,               IDD_VALIDITY_YEAR},        /*8077*/
  {IDL_ABS_VAL_MONTH,              IDD_VALIDITY_MONTH},       /*8078*/
  {IDL_ABS_VAL_DAY,                IDD_VALIDITY_DAY},         /*8079*/
  {IDL_ABS_VAL_HOUR,               IDD_VALIDITY_HOUR},        /*8080*/
  {IDL_ABS_VAL_MINUTE,             IDD_VALIDITY_MINUTE},      /*8081*/
  {IDL_ABS_VAL_TIMEZONE,           IDD_VALIDITY_TIMEZONE},    /*8082*/
  {IDL_DEST_ADDR_NUMBER,           IDD_DESTADDR},             /*8234*/
  {IDL_DEST_ADDR_TYPE,             IDD_DESTADDR_TYPE},        /*8235*/
  {IDL_DEST_ADDR_PLAN,             IDD_DESTADDR_PLAN},        /*8236*/
  {IDL_SC_ADDR_NUMBER,             IDD_DESTADDR},             /*8237*/
  {IDL_SC_ADDR_TYPE,               IDD_SCADDR_TYPE},          /*8238*/
  {IDL_SC_ADDR_PLAN,               IDD_SCADDR_PLAN},          /*8239*/
  {IDL_SK_OUTBOX_OPTIONS_COMMAND,  IDD_MSG_COMMAND},          /*8247*/
  {IDL_SK_INBOX_REPLY,             IDD_MESSAGE_TYPE},         /*8248*/
  {IDL_EMS_TEST_MENU,              IDD_EMS_TEST},             /*8560*/
  {IDL_GW_TEST_MENU,               IDD_GW_TEST},              /*8561*/
  {IDL_GW_TMPLT_ENCODING,          IDD_ENCODING_TYPES},       /*8594*/
  {IDL_CLASS_SUMMARY,              IDD_DCS_SUMMARY},          /*8605*/
  {IDL_PREDEF_ANIMATION,           IDD_VIEW_PREDEF_ANIM},     /*8637*/
  {IDL_USER_MELODY,                IDD_LIST_USER_MELODY},     /*8638*/
  {IDL_SK_TEST_STORE_MSG,          IDD_MEMORY_STORE},         /*8652*/
  {IDL_MODE_SELECT,                IDD_MODE_SELECT},          /*8658*/
  {IDL_CB_SERVICE_LIST,            IDD_CB_SERVICES},          /*8660*/
  {IDL_BROADCAST_MSG,              IDD_BROADCAST_MESSAGE_ACCESS}, /*166*/
  {IDL_SK_CB_READ,                 IDD_BROADCAST_MESSAGE},    /*8709*/
  {IDL_SK_CB_MSG_INFO,             IDD_BROADCAST_MESSAGE_OTHER_INFO}, /*8717*/
  {IDL_DOMAIN_PREF,                IDD_DOMAIN_PREF},          /*8680*/
  {IDL_AUTO_MEM_STORE,             IDD_AUTO_MEM_STORE},       /*8827*/
  {IDL_REL_VAL_USER_DEFINED,       IDD_REL_VAL_USER_DEFINED}, /*8842*/
  {IDL_ALERT_SETTINGS,             IDD_ALERT_SETTINGS},       /*8862*/
  {IDL_SK_ABOUT,                   IDD_ABOUT},                /*8867*/
  {IDL_SEGMENT_SETTING,            IDD_SEGMENT_SETTING},      /*8869*/
  {IDL_VOICEMAIL_NUMBER_LINE2,     IDD_VOICEMAIL_NUMBER},     /*8848*/
  {IDL_RETRY_SETTING,              IDD_RETRY_SETTINGS},       /*8905*/
};

typedef struct
{
  uint16 wDialogID;
  uint16 wCtrlID;
} SMSMenuCtlIDTable;

static const SMSMenuCtlIDTable SMSLookupMenuCtlID[] = {
  {IDD_MAINSMS,                 IDC_MAIN_MENU},               /*1*/
  {IDD_SETTINGS,                IDC_MENU_SETTINGS},           /*2*/
  {IDD_MSG_CLASSES,             IDC_MSG_CLASSES},             /*3*/
  {IDD_ROUTES,                  IDC_ROUTES},                  /*4*/
  {IDD_ROUTE_SETTINGS,          IDC_ROUTE_SETTINGS},          /*5*/
  {IDD_MEMORY_STORE,            IDC_MEMORY_STORE},            /*6*/
  {IDD_DESTADDR,                IDC_DESTADDR},                /*7*/
  {IDD_MESSAGE_TYPE,            IDC_MESSAGE_TYPE},            /*8*/
  {IDD_TEMPLATE_LIST,           IDC_TEMPLATE},                /*9*/
  {IDD_GW_OUTBOX_PREF,          IDC_GW_MENU_OUTBOX_PREF},     /*10*/
  {IDD_REJECT_DUP,              IDC_REJECT_DUP},              /*11*/
  {IDD_VALIDITY,                IDC_VALIDITY_FORMATS},        /*12*/
  {IDD_REPLY_PATH,              IDC_REPLY_PATH},              /*13*/
  {IDD_PID,                     IDC_PID_TYPES},               /*14*/
  {IDD_DCS,                     IDC_DCS_CLASSES},             /*15*/
  {IDD_REL_VAL_USER_DEFINED,    IDC_REL_VAL_USER_DEFINED},    /*16*/
  {IDD_VALIDITY_TIMEZONE,       IDC_MENU_TIMEZONE},           /*17*/
  {IDD_GW_TEMPLATE_OPTIONS,     IDC_GW_TMPLT_OPTIONS},        /*18*/
  {IDD_OTHERINFO,               IDC_STATIC_OTHERINFO},        /*19*/
  {IDD_OUTBOX_OPTIONS,          IDC_OUTBOX_OPTIONS},          /*20*/
  {IDD_TEMPLATE_ALPHA_TAG,      IDC_TEMPLATE_ALPHA_TAG},      /*21*/
  {IDD_DESTADDR_TYPE,           IDC_DESTADDR_TYPE},           /*22*/
  {IDD_DESTADDR_PLAN,           IDC_DESTADDR_PLAN},           /*23*/
  {IDD_TEMPLATE_DESTADDR,       IDC_TEMPLATE_DESTADDR},       /*24*/
  {IDD_SCADDR_TYPE,             IDC_SCADDR_TYPE},             /*25*/
  {IDD_SCADDR_PLAN,             IDC_SCADDR_PLAN},             /*26*/
  {IDD_TEMPLATE_SCADDR,         IDC_TEMPLATE_SCADDR},         /*27*/
  {IDD_LIST_USER_MELODY,        IDC_MENU_USER_MELODY},        /*28*/
  {IDD_VIEW_PREDEF_ANIM,        IDC_MENU_LIST_PREDEF_ANIM},   /*29*/
  {IDD_TEMPLATE_DCS_ALPHABET,   IDC_TEMPLATE_DCS_ALPHABET},   /*30*/
  {IDD_ENCODING_TYPES,          IDC_MENU_ENCODING_TYPES},     /*31*/
  {IDD_CARD_MESSAGE_ACCESS,     IDC_CARD_MESSAGE_LIST},       /*32*/
  {IDD_INBOX_VIEW,              IDC_SK_INBOX},                /*33*/
  {IDD_LIST_ATTACHMENTS        ,IDC_MENU_CATEGORIES},         /*35*/
  {IDD_VIEW_SMALL_PICTURES     ,IDC_MENU_LIST_SMALL_PICT},    /*36*/
  {IDD_OUTBOX_VIEW             ,IDC_SK_OUTBOX_OPTNS},         /*37*/
  {IDD_LIST_MELODY             ,IDC_MENU_MELODY},             /*38*/
  {IDD_CALL                    ,IDC_CALL_NUMBER},             /*39*/
  {IDD_EMS_TEST                ,IDC_EMS_TEST},                /*40*/
  {IDD_VIEW_MESSAGE_INFO       ,IDC_SK_MESSAGE_INFO_NEXT},    /*41*/
  {IDD_VIEW_TMPLT              ,IDC_STATIC_VIEW_TMPLT},       /*42*/
  {IDD_VIEW_LARGE_PICTURES     ,IDC_MENU_LIST_LARGE_PICT},    /*43*/
  {IDD_VIEW_VAR_PICTURES       ,IDC_MENU_LIST_VAR_PICT},      /*44*/
  {IDD_VIEW_SMALL_ANIM         ,IDC_MENU_LIST_SMALL_ANIM},    /*45*/
  {IDD_VIEW_LARGE_ANIM         ,IDC_MENU_LIST_LARGE_ANIM},    /*46*/
  {IDD_NV_MESSAGE_ACCESS       ,IDC_NV_MESSAGE_LIST},         /*47*/
  {IDD_TEMP_MESSAGE_ACCESS     ,IDC_TEMP_MESSAGE_LIST},       /*48*/
  {IDD_CDMA_OUTBOX_PREF        ,IDC_CDMA_OUTBOX_PREF_MENU},   /*49*/
  {IDD_TELESERVICE             ,IDC_MENU_TELESERVICE},        /*50*/
  {IDD_STATUS_REPORT           ,IDC_STATIC_STATUS_REPORT},    /*51*/
  {IDD_MSG_COMMAND             ,IDC_MENU_MSG_COMMAND},        /*52*/
  {IDD_CDMA_DCS                ,IDC_MENU_CDMA_DCS},           /*53*/
  {IDD_PRIORITY                ,IDC_MENU_PRIORITY},           /*54*/
  {IDD_PRIVACY                 ,IDC_MENU_PRIVACY},            /*55*/
  {IDD_LANGUAGE                ,IDC_MENU_LANGUAGE},           /*56*/
  {IDD_MESSAGE                 ,IDC_STATIC_MSG},              /*57*/
  {IDD_BC_SMS                  ,IDC_MENU_BC_SMS},             /*58*/
  {IDD_BC_EDIT_TABLE           ,IDC_MENU_BC_EDIT_TABLE},      /*59*/
  {IDD_CDMA_TEMPLATE_OPTIONS   ,IDC_MENU_CDMA_TEMPLATE_OPTNS},/*60*/
  {IDD_BC_SERVICES             ,IDC_MENU_BC_SERVICES},        /*61*/
  {IDD_SIMPLE_COMPOSE          ,IDC_SIMPLE_COMPOSE},          /*62*/
  {IDD_REL_VALIDITY            ,IDC_MENU_RELATIVE_VALIDITY},  /*63*/
  {IDD_ABS_VALIDITY            ,IDC_MENU_ABS_VALIDITY},       /*64*/
  {IDD_VALIDITY_YEAR           ,IDC_MENU_VALIDITY_YEAR},      /*65*/
  {IDD_VALIDITY_MONTH          ,IDC_MENU_VALIDITY_MONTH},     /*66*/
  {IDD_VALIDITY_DAY            ,IDC_MENU_VAL_DAY},            /*67*/
  {IDD_VALIDITY_HOUR           ,IDC_MENU_VAL_HOUR},           /*68*/
  {IDD_VALIDITY_MINUTE         ,IDC_MENU_VAL_MINUTE},         /*69*/
  {IDD_REPLY_OPTIONS           ,IDC_MENU_REPLY_OPTIONS},      /*70*/
  {IDD_DISPLAY_MODE            ,IDC_MENU_DISPLAY_MODE},       /*71*/
  {IDD_ALERT_MODE_637A         ,IDC_MENU_ALERT_MODE},         /*72*/
  {IDD_BC_CONFIG               ,IDC_STATIC_BC_CONFIG},        /*73*/
  {IDD_BC_PREF                 ,IDC_MENU_BC_PREF},            /*74*/
  {IDD_BC_TABLE                ,IDC_MENU_BC_TABLE_OPTIONS},   /*75*/
  {IDD_EDIT_DEST_ADDR          ,IDC_MENU_EDIT_DEST_ADDR},     /*79*/
  {IDD_EDIT_SC_ADDR            ,IDC_MENU_EDIT_SC_ADDR},       /*80*/
  {IDD_CDMA_TEST               ,IDC_CDMA_TEST},               /*81*/
  {IDD_GW_TEST                 ,IDC_GW_TEST},                 /*82*/
  {IDD_TEST                    ,IDC_TEST},                    /*83*/
  {IDD_TEST_ADDRESS            ,IDC_TEST_ADDRESS},            /*84*/
  {IDD_TEST_NUMBER_OF_CHARS    ,IDC_TEST_NUMBER_OF_CHARS},    /*85*/
  {IDD_BC_ADD_SVC              ,IDC_MENU_BC_ADD_SVC},         /*87*/
  {IDD_BC_AVAILABLE_SVCS       ,IDC_MENU_BC_AVAILABLE_SVCS},  /*88*/
  {IDD_VOICEMAIL_NUMBER        ,IDC_VOICEMAIL_NUMBER},        /*89*/
  {IDD_USAGE                   ,IDC_STATIC_USAGE},            /*90*/
  {IDD_DCS_SUMMARY             ,IDC_STATIC_DCS_SUMMARY},      /*91*/
  {IDD_MODE_SELECT             ,IDC_MENU_MODE_SELECT},        /*94*/
  {IDD_CB_SETTINGS             ,IDC_CB_SETTINGS},             /*95*/
  {IDD_CB_SERVICES             ,IDC_CB_SERVICE_LIST},         /*96*/
  {IDD_CB_SERVICE_INFO         ,IDC_CB_SERVICE_EDIT_SEL},     /*97*/
  {IDD_BROADCAST_MESSAGE_ACCESS,IDC_BROADCAST_MSG_LIST},      /*98*/
  {IDD_BROADCAST_MESSAGE       ,IDC_ST_BROADCAST_MESSAGE},    /*99*/
  {IDD_BROADCAST_MESSAGE_OTHER_INFO, IDC_ST_BROADCAST_MESSAGE_OTHER_INFO}, /*100*/
  {IDD_DOMAIN_PREF             ,IDC_DOMAIN_PREF},             /*101*/
  {IDD_AUTO_MEM_STORE          ,IDC_MENU_AUTO_MEM_STORE},     /*102*/
  {IDD_ALERT_SETTINGS          ,IDC_ALERT_SETTINGS},          /*103*/
  {IDD_AUTO_DC_SEND_SO         ,IDC_MENU_AUTO_DC_SEND_SO},    /*104*/
  {IDD_SEGMENT_SETTING         ,IDC_SEGMENT_SETTING},         /*105*/
  {IDD_RETRY_SETTINGS          ,IDC_RETRY_SETTING},           /*115*/
};

typedef struct
{
  uint16 wDialogID;
  uint16 wSKCtrlID;
} SMSSKCtlIDTable;

static const SMSSKCtlIDTable SMSLookupSKCtlID[] = {
  {IDD_MAINSMS,                    IDC_SK_ABOUT},                      /*1*/
  {IDD_SETTINGS,                   IDC_SK_SETTINGS_DONE},              /*2*/
  {IDD_DESTADDR,                   IDC_SK_DESTADDR},                   /*7*/
  {IDD_TEMPLATE_LIST,              IDC_SK_TMPLT_OPTIONS},              /*9*/
  {IDD_GW_OUTBOX_PREF,             IDC_SK_GW_OUTBOX_OPTIONS},          /*10*/
  {IDD_REL_VAL_USER_DEFINED,       IDC_SK_REL_VAL_USER_DEFINED},       /*16*/
  {IDD_GW_TEMPLATE_OPTIONS,        IDC_SK_GW_TMPLT_OPTIONS},           /*18*/
  {IDD_OUTBOX_OPTIONS,             IDC_SK_OUTBOX_OPTIONS_DONE},        /*20*/
  {IDD_TEMPLATE_ALPHA_TAG,         IDC_SK_TEMPLATE_ALPHA_TAG},         /*22*/
  {IDD_LIST_USER_MELODY,           IDC_SK_USER_MELODY_OPTIONS},        /*28*/
  {IDD_VIEW_PREDEF_ANIM,           IDC_SK_SELECT_PREDEF_ANIM},         /*29*/
  {IDD_TEMPLATE_DCS_ALPHABET,      IDC_SK_TEMPLATE_DCS_ALPHABET_NEXT}, /*31*/
  {IDD_CARD_MESSAGE_ACCESS,        IDC_SK_CARD_MESSAGE_ACCESS},        /*32*/
  {IDD_COMPOSE_MSG,                IDC_SK_COMPOSE},                    /*34*/
  {IDD_VIEW_SMALL_PICTURES,        IDC_SK_SELECT_SMALL_PICT},          /*36*/
  {IDD_LIST_MELODY,                IDC_SK_MELODY_OPTIONS},             /*38*/
  {IDD_VIEW_TMPLT,                 IDC_SK_TMPLT_OPTNS},                /*42*/
  {IDD_VIEW_LARGE_PICTURES,        IDC_SK_SELECT_LARGE_PICT},          /*43*/
  {IDD_VIEW_VAR_PICTURES,          IDC_SK_SELECT_VAR_PICT},            /*44*/
  {IDD_VIEW_SMALL_ANIM,            IDC_SK_SELECT_SMALL_ANIM},          /*45*/
  {IDD_VIEW_LARGE_ANIM,            IDC_SK_SELECT_LARGE_ANIM},          /*46*/
  {IDD_NV_MESSAGE_ACCESS,          IDC_SK_NV_MESSAGE_ACCESS},          /*47*/
  {IDD_TEMP_MESSAGE_ACCESS,        IDC_SK_TEMP_MESSAGE_ACCESS},        /*48*/
  {IDD_CDMA_OUTBOX_PREF,           IDC_SK_CDMA_OUTBOX},                /*49*/
  {IDD_STATUS_REPORT,              IDC_SK_STATUS_REPORT},              /*51*/
  {IDD_BC_SMS,                     IDC_SK_BC_SMS},                     /*58*/
  {IDD_BC_EDIT_TABLE,              IDC_SK_BC_EDIT_TABLE},              /*59*/
  {IDD_CDMA_TEMPLATE_OPTIONS,      IDC_SK_C_TMPLT_OPTIONS},            /*60*/
  {IDD_BC_SERVICES,                IDC_SK_BC_SRV},                     /*61*/
  {IDD_REPLY_OPTIONS,              IDC_SK_REPLY_OPTIONS},              /*70*/
  /* */
  {IDD_BC_CONFIG,                  IDC_SK_BC_CONFIG},                  /*73*/
  {IDD_BC_PREF,                    IDC_SK_BC_PREF},                    /*74*/
  {IDD_BC_TABLE,                   IDC_SK_BC_TABLE_OPTIONS},           /*75*/
  {IDD_CONFIRM,                    IDC_SK_CONFIRM},                    /*76*/
  {IDD_EDIT_DEST_ADDR,             IDC_SK_MENU_EDIT_DESTADDR},         /*79*/
  {IDD_EDIT_SC_ADDR,               IDC_SK_MENU_EDIT_SCADDR},           /*80*/
  {IDD_TEST_ADDRESS,               IDC_SK_TEST_DONE},                  /*84*/
  {IDD_TEST_NUMBER_OF_CHARS,       IDC_SK_TEST_SEND_MSG},              /*85*/
  {IDD_BC_SVC_DISPLAY,             IDC_SK_BC_SVC_DISPLAY},             /*86*/
  {IDD_BC_ADD_SVC,                 IDC_SK_BC_ADD_SVC},                 /*87*/
  {IDD_VOICEMAIL_NUMBER,           IDC_SK_VOICEMAIL},                  /*89*/
  {IDD_USAGE,                      IDC_SK_USAGE},                      /*90*/
  {IDD_ABOUT,                      IDC_SK_ABOUT_DONE},                 /*92*/
  {IDD_CB_SETTINGS,                IDC_SK_CB_DONE},                    /*95*/
  {IDD_CB_SERVICES,                IDC_SK_CB_SERVICE},                 /*96*/
  {IDD_CB_SERVICE_INFO,            IDC_SK_CB_SERVICE_EDIT},            /*97*/
  {IDD_BROADCAST_MESSAGE_ACCESS,   IDC_SK_BROADCAST_MESSAGE_ACCESS},   /*98*/
  {IDD_BROADCAST_MESSAGE,          IDC_SK_BROADCAST_MESSAGE},          /*99*/
  {IDD_BROADCAST_MESSAGE_OTHER_INFO,IDC_SK_BROADCAST_MESSAGE_OTHER_INFO},/*100*/
  {IDD_COMPOSE_MENU,               IDC_SK_COMPOSE_MENU},                /*105*/
  {IDD_AUTO_STATUS,                IDC_SK_AUTO_STATUS},                 /*106*/
  {IDD_AUTO_OTHER_INFO,            IDC_SK_AUTO_OTHER_INFO},             /*107*/
  {IDD_AUTO_VOICEMAIL,             IDC_SK_AUTO_VOICEMAIL},              /*108*/
  {IDD_AUTO_REQUEST_VOICEMAIL_NUMBER, IDC_SK_AUTO_REQUEST_VOICEMAIL_NUMBER}, /*110*/
};

typedef enum
{
  WMSAPP_MSG_TYPE_PHONE_NUM,
  WMSAPP_MSG_TYPE_EMAIL,
  WMSAPP_MSG_TYPE_NON_STD_EMAIL
}wmsapp_msg_e_type;

static const int WMSAPP_NV_MSG[] =
 {
   IDL_NV_MSG_1, IDL_NV_MSG_2, IDL_NV_MSG_3, IDL_NV_MSG_4, IDL_NV_MSG_5, IDL_NV_MSG_6, IDL_NV_MSG_7, IDL_NV_MSG_8, IDL_NV_MSG_9, IDL_NV_MSG_10,
   IDL_NV_MSG_11,IDL_NV_MSG_12,IDL_NV_MSG_13,IDL_NV_MSG_14,IDL_NV_MSG_15,IDL_NV_MSG_16,IDL_NV_MSG_17,IDL_NV_MSG_18,IDL_NV_MSG_19,IDL_NV_MSG_20,
   IDL_NV_MSG_21,IDL_NV_MSG_22,IDL_NV_MSG_23,IDL_NV_MSG_24,IDL_NV_MSG_25,IDL_NV_MSG_26,IDL_NV_MSG_27,IDL_NV_MSG_28,IDL_NV_MSG_29,IDL_NV_MSG_30,
   IDL_NV_MSG_31,IDL_NV_MSG_32,IDL_NV_MSG_33,IDL_NV_MSG_34,IDL_NV_MSG_35,IDL_NV_MSG_36,IDL_NV_MSG_37,IDL_NV_MSG_38,IDL_NV_MSG_39,IDL_NV_MSG_40,
   IDL_NV_MSG_41,IDL_NV_MSG_42,IDL_NV_MSG_43,IDL_NV_MSG_44,IDL_NV_MSG_45,IDL_NV_MSG_46,IDL_NV_MSG_47,IDL_NV_MSG_48,IDL_NV_MSG_49,IDL_NV_MSG_50,
   IDL_NV_MSG_51,IDL_NV_MSG_52,IDL_NV_MSG_53,IDL_NV_MSG_54,IDL_NV_MSG_55,IDL_NV_MSG_56,IDL_NV_MSG_57,IDL_NV_MSG_58,IDL_NV_MSG_59,IDL_NV_MSG_60,
   IDL_NV_MSG_61,IDL_NV_MSG_62,IDL_NV_MSG_63,IDL_NV_MSG_64,IDL_NV_MSG_65,IDL_NV_MSG_66,IDL_NV_MSG_67,IDL_NV_MSG_68,IDL_NV_MSG_69,IDL_NV_MSG_70,
   IDL_NV_MSG_71,IDL_NV_MSG_72,IDL_NV_MSG_73,IDL_NV_MSG_74,IDL_NV_MSG_75,IDL_NV_MSG_76,IDL_NV_MSG_77,IDL_NV_MSG_78,IDL_NV_MSG_79,IDL_NV_MSG_80,
   IDL_NV_MSG_81,IDL_NV_MSG_82,IDL_NV_MSG_83,IDL_NV_MSG_84,IDL_NV_MSG_85,IDL_NV_MSG_86,IDL_NV_MSG_87,IDL_NV_MSG_88,IDL_NV_MSG_89,IDL_NV_MSG_90,
   IDL_NV_MSG_91,IDL_NV_MSG_92,IDL_NV_MSG_93,IDL_NV_MSG_94,IDL_NV_MSG_95,IDL_NV_MSG_96,IDL_NV_MSG_97,IDL_NV_MSG_98,IDL_NV_MSG_99,IDL_NV_MSG_100,
 };

static const int WMSAPP_CARD_MSG[] =
 {
   IDL_CARD_MSG_1, IDL_CARD_MSG_2, IDL_CARD_MSG_3, IDL_CARD_MSG_4, IDL_CARD_MSG_5, IDL_CARD_MSG_6, IDL_CARD_MSG_7, IDL_CARD_MSG_8, IDL_CARD_MSG_9, IDL_CARD_MSG_10,
   IDL_CARD_MSG_11,IDL_CARD_MSG_12,IDL_CARD_MSG_13,IDL_CARD_MSG_14,IDL_CARD_MSG_15,IDL_CARD_MSG_16,IDL_CARD_MSG_17,IDL_CARD_MSG_18,IDL_CARD_MSG_19,IDL_CARD_MSG_20,
   IDL_CARD_MSG_21,IDL_CARD_MSG_22,IDL_CARD_MSG_23,IDL_CARD_MSG_24,IDL_CARD_MSG_25,IDL_CARD_MSG_26,IDL_CARD_MSG_27,IDL_CARD_MSG_28,IDL_CARD_MSG_29,IDL_CARD_MSG_30,
   IDL_CARD_MSG_31,IDL_CARD_MSG_32,IDL_CARD_MSG_33,IDL_CARD_MSG_34,IDL_CARD_MSG_35,IDL_CARD_MSG_36,IDL_CARD_MSG_37,IDL_CARD_MSG_38,IDL_CARD_MSG_39,IDL_CARD_MSG_40,
   IDL_CARD_MSG_41,IDL_CARD_MSG_42,IDL_CARD_MSG_43,IDL_CARD_MSG_44,IDL_CARD_MSG_45,IDL_CARD_MSG_46,IDL_CARD_MSG_47,IDL_CARD_MSG_48,IDL_CARD_MSG_49,IDL_CARD_MSG_50,
   IDL_CARD_MSG_51,IDL_CARD_MSG_52,IDL_CARD_MSG_53,IDL_CARD_MSG_54,IDL_CARD_MSG_55,IDL_CARD_MSG_56,IDL_CARD_MSG_57,IDL_CARD_MSG_58,IDL_CARD_MSG_59,IDL_CARD_MSG_60,
   IDL_CARD_MSG_61,IDL_CARD_MSG_62,IDL_CARD_MSG_63,IDL_CARD_MSG_64,IDL_CARD_MSG_65,IDL_CARD_MSG_66,IDL_CARD_MSG_67,IDL_CARD_MSG_68,IDL_CARD_MSG_69,IDL_CARD_MSG_70,
   IDL_CARD_MSG_71,IDL_CARD_MSG_72,IDL_CARD_MSG_73,IDL_CARD_MSG_74,IDL_CARD_MSG_75,IDL_CARD_MSG_76,IDL_CARD_MSG_77,IDL_CARD_MSG_78,IDL_CARD_MSG_79,IDL_CARD_MSG_80,
   IDL_CARD_MSG_81,IDL_CARD_MSG_82,IDL_CARD_MSG_83,IDL_CARD_MSG_84,IDL_CARD_MSG_85,IDL_CARD_MSG_86,IDL_CARD_MSG_87,IDL_CARD_MSG_88,IDL_CARD_MSG_89,IDL_CARD_MSG_90,
   IDL_CARD_MSG_91,IDL_CARD_MSG_92,IDL_CARD_MSG_93,IDL_CARD_MSG_94,IDL_CARD_MSG_95,IDL_CARD_MSG_96,IDL_CARD_MSG_97,IDL_CARD_MSG_98,IDL_CARD_MSG_99,IDL_CARD_MSG_100,
 };

static const int WMSAPP_TEMP_MSG[] =
 {
   IDL_TEMP_MSG_1, IDL_TEMP_MSG_2, IDL_TEMP_MSG_3, IDL_TEMP_MSG_4, IDL_TEMP_MSG_5, IDL_TEMP_MSG_6, IDL_TEMP_MSG_7, IDL_TEMP_MSG_8, IDL_TEMP_MSG_9, IDL_TEMP_MSG_10,
   IDL_TEMP_MSG_11,IDL_TEMP_MSG_12,IDL_TEMP_MSG_13,IDL_TEMP_MSG_14,IDL_TEMP_MSG_15,IDL_TEMP_MSG_16,IDL_TEMP_MSG_17,IDL_TEMP_MSG_18,IDL_TEMP_MSG_19,IDL_TEMP_MSG_20,
   IDL_TEMP_MSG_21,IDL_TEMP_MSG_22,IDL_TEMP_MSG_23,IDL_TEMP_MSG_24,IDL_TEMP_MSG_25,IDL_TEMP_MSG_26,IDL_TEMP_MSG_27,IDL_TEMP_MSG_28,IDL_TEMP_MSG_29,IDL_TEMP_MSG_30,
   IDL_TEMP_MSG_31,IDL_TEMP_MSG_32,IDL_TEMP_MSG_33,IDL_TEMP_MSG_34,IDL_TEMP_MSG_35,IDL_TEMP_MSG_36,IDL_TEMP_MSG_37,IDL_TEMP_MSG_38,IDL_TEMP_MSG_39,IDL_TEMP_MSG_40,
   IDL_TEMP_MSG_41,IDL_TEMP_MSG_42,IDL_TEMP_MSG_43,IDL_TEMP_MSG_44,IDL_TEMP_MSG_45,IDL_TEMP_MSG_46,IDL_TEMP_MSG_47,IDL_TEMP_MSG_48,IDL_TEMP_MSG_49,IDL_TEMP_MSG_50,
   IDL_TEMP_MSG_51,IDL_TEMP_MSG_52,IDL_TEMP_MSG_53,IDL_TEMP_MSG_54,IDL_TEMP_MSG_55,IDL_TEMP_MSG_56,IDL_TEMP_MSG_57,IDL_TEMP_MSG_58,IDL_TEMP_MSG_59,IDL_TEMP_MSG_60,
   IDL_TEMP_MSG_61,IDL_TEMP_MSG_62,IDL_TEMP_MSG_63,IDL_TEMP_MSG_64,IDL_TEMP_MSG_65,IDL_TEMP_MSG_66,IDL_TEMP_MSG_67,IDL_TEMP_MSG_68,IDL_TEMP_MSG_69,IDL_TEMP_MSG_70,
   IDL_TEMP_MSG_71,IDL_TEMP_MSG_72,IDL_TEMP_MSG_73,IDL_TEMP_MSG_74,IDL_TEMP_MSG_75,IDL_TEMP_MSG_76,IDL_TEMP_MSG_77,IDL_TEMP_MSG_78,IDL_TEMP_MSG_79,IDL_TEMP_MSG_80,
   IDL_TEMP_MSG_81,IDL_TEMP_MSG_82,IDL_TEMP_MSG_83,IDL_TEMP_MSG_84,IDL_TEMP_MSG_85,IDL_TEMP_MSG_86,IDL_TEMP_MSG_87,IDL_TEMP_MSG_88,IDL_TEMP_MSG_89,IDL_TEMP_MSG_90,
   IDL_TEMP_MSG_91,IDL_TEMP_MSG_92,IDL_TEMP_MSG_93,IDL_TEMP_MSG_94,IDL_TEMP_MSG_95,IDL_TEMP_MSG_96,IDL_TEMP_MSG_97,IDL_TEMP_MSG_98,IDL_TEMP_MSG_99,IDL_TEMP_MSG_100,
 };

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_CDSMS_BROADCAST
static const uint16 bcServItems[] =
 {
   IDL_BC_SRV_0, IDL_BC_SRV_1, IDL_BC_SRV_2, IDL_BC_SRV_3, IDL_BC_SRV_4,
   IDL_BC_SRV_5, IDL_BC_SRV_6, IDL_BC_SRV_7, IDL_BC_SRV_8, IDL_BC_SRV_9,
   IDL_BC_SRV_10, IDL_BC_SRV_11, IDL_BC_SRV_12, IDL_BC_SRV_13,
   IDL_BC_SRV_14, IDL_BC_SRV_15, IDL_BC_SRV_16, IDL_BC_SRV_17,
   IDL_BC_SRV_18, IDL_BC_SRV_19, IDL_BC_SRV_20, IDL_BC_SRV_21,
   IDL_BC_SRV_22, IDL_BC_SRV_23, IDL_BC_SRV_24, IDL_BC_SRV_25,
   IDL_BC_SRV_26, IDL_BC_SRV_27, IDL_BC_SRV_28, IDL_BC_SRV_29,
   IDL_BC_SRV_30, IDL_BC_SRV_31,
 };
#endif /* FEATURE_CDSMS_BROADCAST */

#define SERVITEMS_SIZE 32
typedef struct
{
  wms_cmd_id_e_type cmd;
  uint16 wResID;
} WMSAPPCmdTable;

typedef struct
{
  wms_cmd_err_e_type cmd_err;
  uint16 wResID;
} WMSAPPCmdErrTable;

static const WMSAPPCmdTable WMSAPPLookupCmd[] = {
  {WMS_CMD_CFG_SET_ROUTES,              IDS_CMD_SET_ROUTES},
  {WMS_CMD_CFG_GET_ROUTES,              IDS_CMD_GET_ROUTES},
  {WMS_CMD_CFG_GET_MEMORY_STATUS,       IDS_CMD_GET_MEM_STATUS},
  {WMS_CMD_CFG_GET_MESSAGE_LIST,        IDS_CMD_GET_MSG_LIST},
  {WMS_CMD_CFG_SET_GW_DOMAIN_PREF,      IDS_CMD_SET_GW_DOMAIN},
  {WMS_CMD_CFG_GET_GW_DOMAIN_PREF,      IDS_CMD_GET_GW_DOMAIN},

  /* Message group commands
  */
  {WMS_CMD_MSG_SEND,                    IDS_CMD_MSG_SEND},
  {WMS_CMD_MSG_ACK,                     IDS_CMD_MSG_ACK},
  {WMS_CMD_MSG_READ,                    IDS_CMD_MSG_READ},
  {WMS_CMD_MSG_WRITE,                   IDS_CMD_MSG_WRITE},
  {WMS_CMD_MSG_DELETE,                  IDS_CMD_MSG_DELETE},
  {WMS_CMD_MSG_DELETE_ALL,              IDS_CMD_MSG_DELETE_ALL},
  {WMS_CMD_MSG_MODIFY_TAG,              IDS_CMD_MSG_MODIFY_TAG},
  {WMS_CMD_MSG_READ_TEMPLATE,           IDS_CMD_READ_TEMPLATE},
  {WMS_CMD_MSG_WRITE_TEMPLATE,          IDS_CMD_WRITE_TEMPLATE},
  {WMS_CMD_MSG_DELETE_TEMPLATE,         IDS_CMD_DELETE_TEMPLATE},
  {WMS_CMD_MSG_DELETE_TEMPLATE_ALL,     IDS_CMD_DELETE_TEMPLATE_ALL},

  {WMS_CMD_MSG_READ_STS_REPORT,         IDS_CMD_READ_STS_REPORT},
  {WMS_CMD_MSG_WRITE_STS_REPORT,        IDS_CMD_WRITE_STS_REPORT},
  {WMS_CMD_MSG_DELETE_STS_REPORT,       IDS_CMD_DELETE_STS_REPORT},
  {WMS_CMD_MSG_DELETE_STS_REPORT_ALL,   IDS_CMD_DELETE_STS_REPORT_ALL},

  /* DC commands
  */
  {WMS_CMD_DC_CONNECT,                  IDS_CMD_DC_CONNECT},
  {WMS_CMD_DC_DISCONNECT,               IDS_CMD_DC_DISCONNECT},
  {WMS_CMD_DC_ENABLE_AUTO_DISCONNECT,   IDS_CMD_ENABLE_AUTO_DISCONNECT},
  {WMS_CMD_DC_DISABLE_AUTO_DISCONNECT,  IDS_CMD_DISABLE_AUTO_DISCONNECT},

  {WMS_CMD_BC_MM_GET_CONFIG,            IDS_CMD_BC_MM_GET_CONFIG},
  {WMS_CMD_BC_MM_GET_PREF,              IDS_CMD_BC_MM_GET_PREF},
  {WMS_CMD_BC_MM_SET_PREF,              IDS_CMD_BC_MM_SET_PREF},
  {WMS_CMD_BC_MM_GET_TABLE,             IDS_CMD_BC_MM_GET_TABLE},
  {WMS_CMD_BC_MM_SELECT_SRV,            IDS_CMD_BC_MM_SELECT_SRV},
  {WMS_CMD_BC_MM_GET_ALL_SRV_IDS,       IDS_CMD_BC_MM_GET_ALL_SRV_IDS},
  {WMS_CMD_BC_MM_GET_SRV_INFO,          IDS_CMD_BC_MM_GET_SRV_INFO},
  {WMS_CMD_BC_MM_ADD_SRV,               IDS_CMD_BC_MM_ADD_SRV},
  {WMS_CMD_BC_MM_DELETE_SRV,            IDS_CMD_BC_MM_DELETE_SRV},
  {WMS_CMD_BC_MM_CHANGE_SRV_INFO,       IDS_CMD_BC_MM_CHANGE_SRV_INFO},
  {WMS_CMD_BC_MM_DELETE_ALL_SERVICES,   IDS_CMD_BC_MM_DELETE_ALL_SERVICES},
  {WMS_CMD_BC_MM_SELECT_ALL_SERVICES,   IDS_CMD_BC_MM_SELECT_ALL_SERVICES},
  {WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES,IDS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES},

  {WMS_CMD_DBG_RESET_TL_SEQ_NUM,        IDS_CMD_BC_DBG_RESET_TL_SEQ_NUM},

  {WMS_CMD_MSG_MC_MT_MSG_E,             IDS_CMD_MC_MT_MSG},
  {WMS_CMD_MSG_MC_MO_STATUS_E,          IDS_CMD_MC_MO_STATUS},
  {WMS_CMD_MSG_MC_STATE_CHANGED_E,      IDS_CMD_MC_STATE_CHANGED},
  {WMS_CMD_MSG_MC_MWI_E,                IDS_CMD_MC_MWI},
  {WMS_CMD_MSG_GSTK_EVT,                IDS_CMD_MC_GSTK},

  /* Internal: events from CM
  */
  {WMS_CMD_DC_CALL_INCOM_E,             IDS_CMD_DC_CALL_INCOMING},
  {WMS_CMD_DC_CALL_CONNECT_E,           IDS_CMD_DC_CALL_CONNECT},
  {WMS_CMD_DC_CALL_END_E,               IDS_CMD_DC_CALL_END},
  {WMS_CMD_DC_CALL_ERROR_E,             IDS_CMD_DC_CALL_ERROR},

  {WMS_CMD_CM_BC_ENABLE_FAILURE_E,      IDS_CMD_BC_ENABLE_FAILURE},
  {WMS_CMD_CM_START_FULL_SERVICE_E,     IDS_CMD_START_FULL_SERVICE},
  {WMS_CMD_CM_ONLINE_E,                 IDS_CMD_ONLINE},

  {WMS_CMD_GSDI_ASYNC_CB,               IDS_CMD_GSDI_ASYNC_CB},
  {WMS_CMD_BC_SCPT_DATA,                IDS_CMD_BC_SCPT_DATA},
};



/* Command error:
** the command status reported in the command callback
*/
static const WMSAPPCmdErrTable WMSAPPLookupCmdErr[] = {
  {WMS_CMD_ERR_CLIENT_ID,             IDS_CMD_ERR_INVALID_CLIENTID},
  {WMS_CMD_ERR_NO_RESOURCE,           IDS_CMD_ERR_NO_RESOURCES},
  {WMS_CMD_ERR_BUSY,                  IDS_CMD_ERR_BUSY},
  {WMS_CMD_ERR_NO_NETWORK,            IDS_CMD_ERR_NO_RESOURCES},
  {WMS_CMD_ERR_CFG_NV_WRITE,          IDS_CMD_ERR_NV_WRITE},
  {WMS_CMD_ERR_CFG_NV_READ,           IDS_CMD_ERR_NV_READ},
  {WMS_CMD_ERR_CFG_ROUTE,             IDS_CMD_ERR_CFG_ROUTE},
  {WMS_CMD_ERR_CFG_MEM_STORE,         IDS_CMD_ERR_CFG_MEM_STORE},
  {WMS_CMD_ERR_CFG_MSG_CLASS,         IDS_CMD_ERR_CFG_MSG_CLASS},
  {WMS_CMD_ERR_MSG_MEMORY_STORE,      IDS_CMD_ERR_MEMORY_STORE},
  {WMS_CMD_ERR_MSG_INDEX,             IDS_CMD_ERR_INDEX},
  {WMS_CMD_ERR_MSG_EMPTY_MESSAGE,     IDS_CMD_ERR_EMPTY_MESSAGE},
  {WMS_CMD_ERR_MSG_TAG,               IDS_CMD_ERR_MSG_TAG},
  {WMS_CMD_ERR_MSG_ENCODE,            IDS_CMD_ERR_MSG_ENCODE},
  {WMS_CMD_ERR_MSG_DECODE,            IDS_CMD_ERR_MSG_DECODE},
  {WMS_CMD_ERR_MSG_SEND_MODE,         IDS_CMD_ERR_SEND_MODE},
  {WMS_CMD_ERR_MSG_WRITE_MODE,        IDS_CMD_ERR_WRITE_MODE},
  {WMS_CMD_ERR_MSG_MEMORY_FULL,       IDS_CMD_ERR_MEMORY_FULL},
  {WMS_CMD_ERR_MSG_RAM_FULL,          IDS_CMD_ERR_TEMP_MEM_FULL},
  {WMS_CMD_ERR_MSG_SIM_FULL,          IDS_CMD_ERR_CARD_FULL},
  {WMS_CMD_ERR_MSG_NV_FULL,           IDS_CMD_ERR_PHONE_MEM_FULL},
  {WMS_CMD_ERR_MSG_TRANSACTION_ID,    IDS_CMD_ERR_TRANSACTION_ID},
  {WMS_CMD_ERR_MSG_SIM_WRITE,         IDS_CMD_ERR_CARD_WRITE},
  {WMS_CMD_ERR_MSG_SIM_READ,          IDS_CMD_ERR_CARD_READ},
  {WMS_CMD_ERR_MSG_SIM_DELETE,        IDS_CMD_ERR_CARD_DELETE},
  {WMS_CMD_ERR_MSG_RUIM_WRITE,        IDS_CMD_ERR_CARD_WRITE},
  {WMS_CMD_ERR_MSG_RUIM_READ,         IDS_CMD_ERR_CARD_READ},
  {WMS_CMD_ERR_MSG_RUIM_DELETE,       IDS_CMD_ERR_CARD_DELETE},
  {WMS_CMD_ERR_MSG_NV_WRITE,          IDS_CMD_ERR_PHONE_MEM_WRITE},
  {WMS_CMD_ERR_MSG_NV_READ,           IDS_CMD_ERR_PHONE_MEM_READ},
  {WMS_CMD_ERR_MSG_NV_DELETE,         IDS_CMD_ERR_PHONE_MEM_DELETE},
  {WMS_CMD_ERR_MSG_TPDU_TYPE,         IDS_CMD_ERR_TPDU_TYPE},
  {WMS_CMD_ERR_MSG_FORMAT,            IDS_CMD_ERR_MSG_FORMAT},
  {WMS_CMD_ERR_MSG_NO_MO_MSG,         IDS_CMD_ERR_NO_MO_MSG},
  {WMS_CMD_ERR_BC_BAD_PREF,           IDS_CMD_ERR_BC_BAD_PREF},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE,    IDS_CMD_ERR_BC_CANNOT_ACTIVATE},
  {WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL,IDS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL},
  {WMS_CMD_ERR_BC_NV_WRITE,           IDS_CMD_ERR_BC_PHONE_MEM_WRITE},
  {WMS_CMD_ERR_BC_BAD_SRV_ID,         IDS_CMD_ERR_BC_BAD_SRV_ID},
  {WMS_CMD_ERR_BC_TABLE_FULL,         IDS_CMD_ERR_BC_TABLE_FULL},
  {WMS_CMD_ERR_BC_DUPLICATE_SRV,      IDS_CMD_ERR_BC_DUPLICATE_SRV},
  {WMS_CMD_ERR_BC_SIM_WRITE,          IDS_CMD_ERR_BC_SIM_WRITE},
  {WMS_CMD_ERR_BC_SIM_READ,           IDS_CMD_ERR_BC_SIM_READ},
  {WMS_CMD_ERR_DC_BAD_STATE,          IDS_CMD_ERR_DC_BAD_STATE},
  {WMS_CMD_ERR_DC_BAD_CALL_ID,        IDS_CMD_ERR_DC_BAD_CALL_ID},
};

/* Function Prototypes */
static void WMSAPPMN_GetCheckBoxItems
(
  CWMSAPP *pMe
);

/****************************************************************************/
/*                          Static functions                                */
/****************************************************************************/

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPMN_wmsMessageModeToWmsappMode

DESCRIPTION
  Converts wms_message_mode_e_type to wmsapp_mode_e_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static wms_message_mode_e_type WMSAPPMN_WmsappModeToWmsMessageMode
(
  wmsapp_mode_e_type   wmsapp_mode
)
{
  switch (wmsapp_mode)
  {
    case WMSAPP_MODE_CDMA:
      return WMS_MESSAGE_MODE_CDMA;

    case WMSAPP_MODE_GW:
      return WMS_MESSAGE_MODE_GW;

    default:
      MSG_ERROR("Invalid Wmsapp Mode",0,0,0);
      return WMS_MESSAGE_MODE_CDMA;
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPMN_StoreMessage

DESCRIPTION
  Stores the message in the right mode and the correct Box Type
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMN_StoreMessage
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));


  /*ifthe msg is editted and stored in the same mem store, then it has to  be replaced*/
  if(pMe->m_msg_type == WMSAPP_MSG_TYPE_EDIT &&
     pMe->m_pClient_msg->msg_hdr.mem_store == pMe->m_msgInfo.memStore)
  {
    if (IWMS_MsgWrite(pMe->m_pwms, pMe->m_clientId,&pMe->m_callback,
                          (void*)pMe,WMS_WRITE_MODE_REPLACE,
                          pMe->m_pClient_msg_mo)==SUCCESS)
    {
      return TRUE;
    }
  }
  else
  {
    if(pMe->m_msgInfo.boxType == WMSAPP_INBOX)
    {
      if(IWMS_MsgWrite( pMe->m_pwms,pMe->m_clientId,
                        &pMe->m_callback,(void*)pMe,WMS_WRITE_MODE_INSERT,
                        pMe->m_pClient_msg) == SUCCESS)
      {
        return TRUE;
      }
    }
    else
    {
      if (IWMS_MsgWrite( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,
                             (void*)pMe,WMS_WRITE_MODE_INSERT,
                             pMe->m_pClient_msg_mo) == SUCCESS)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION WMSAPPMN_AckMessage

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMN_AckMessage
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if(IWMS_MsgAck(pMe->m_pwms, pMe->m_clientId,
                     &pMe->m_callback, (void*)pMe,
                     &pMe->m_ackInfo) == SUCCESS)
  {
    return TRUE;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPMN_EraseMessage

DESCRIPTION
  Stores the message in the right mode and the correct Box Type
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_EraseMessage
(
CWMSAPP *pMe
)
{

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  /* If the msg is editted and stored in the same mem store,
  ** then it has to  be replaced.
  */
  if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NONE)
  {
    return FALSE;
  }
  else
  {
    if (IWMS_MsgDelete(pMe->m_pwms,pMe->m_clientId, &pMe->m_callback,
                           (void*)pMe,pMe->m_msgInfo.memStore,
                           pMe->m_msgInfo.smsIndex) == SUCCESS)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/
/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPMN_SendMessage

DESCRIPTION
  Stores the message in the right mode and the correct Box Type
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_SendMessage
(
CWMSAPP *pMe
)
{
  boolean retVal = FALSE;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NONE)
  {
    if(IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE,
                         pMe->m_pClient_msg_mo) == SUCCESS)
    {
      retVal = TRUE;
    }
  }
  else
  {
    pMe->m_pClient_msg_mo->msg_hdr.index = pMe->m_msgInfo.smsIndex;
    pMe->m_pClient_msg_mo->msg_hdr.mem_store = pMe->m_msgInfo.memStore;

    if(IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, WMS_SEND_MODE_MEMORY_STORE,
                         pMe->m_pClient_msg_mo) ==SUCCESS)
    {
      retVal = TRUE;
    }
  }
  return retVal;
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPMN_SetRoutes

DESCRIPTION
  Stores the routes set by the user.
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_SetRoutes
(
CWMSAPP *pMe
)
{

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if(IWMS_CfgSetRoutes(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                           (void*)pMe, &pMe->m_routes) == SUCCESS)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION WMSAPPMN_StoreAlphaTag

DESCRIPTION
  Extracts the Number from the Text dialog and saves it for later use

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_StoreAlphaTag
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitIdleMenuDlg

DESCRIPTION
  Initializes the Main Menu Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitIdleMenuDlg
(
  CWMSAPP *pMe

)
{
  IMenuCtl* pICurrentMenu;
  IDialog *pIDialog;
  uint8 i;
  CtlAddItem pai;

  /*Get the information about the current dialog*/
  pIDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_MAIN_MENU);

  if(pICurrentMenu == NULL)
  {
     return FALSE;
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);

  (void)IMENUCTL_DeleteAll(pICurrentMenu);

  /* Initialize the CtlAddItem pai */
  pai.pText = NULL;
  pai.pImage = NULL;
  pai.pszResImage = WMSAPP_RES_FILE;
  pai.pszResText = WMSAPP_RES_FILE;
  pai.dwData = 0;

  if(pMe->m_bTestMode == TRUE)
  {
    /*Find out if there are any new messages in any memory store*/
    /*If there are any new messages, then the menu item should be BOLD*/
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
       #if 0
      (void)IMENUCTL_SetTitle(pICurrentMenu, WMSAPP_RES_FILE,
                        IDS_SMS_APP_CDMA, NULL);
	  #else
	  {
		AECHAR WTitle[40] = {0};
		(void)ISHELL_LoadResString(pMe->m_pShell,
                        WMSAPP_RES_FILE,                                
                        IDS_SMS_APP_CDMA,
                        WTitle,
                        sizeof(WTitle));
		IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
	   }
	   #endif
      /*VOICEMAIL*/
      pai.wItemID = IDL_SMS_VOICEMAIL;
      pai.wText = IDS_SMS_VOICEMAIL;
      pai.wImage = IDB_VOICEMAIL;
      if(pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->voice_mail_active)
      {
        pai.wFont = AEE_FONT_BOLD;
      }
      else
      {
        pai.wFont = AEE_FONT_NORMAL;
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

      /*RAM_CDMA*/
      /*
      pai.wItemID = IDL_RAM;
      pai.wText = IDS_SMS_RAM;
      pai.wImage = IDB_TEMP_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_RAM_CDMA]->len;i++)
      {
        if(pMe->m_pSmsLists[WMS_MEMORY_STORE_RAM_CDMA]->tags[i] == WMS_TAG_MT_NOT_READ)
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);
      */

  #ifdef FEATURE_CDSMS_RUIM
      /* RUIM CARD */
      pai.wItemID = IDL_SIM;
      pai.wText = IDS_SMS_SIM;
      pai.wImage = IDB_CARD_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_RUIM]->len;i++)
      {
        if(pMe->m_pSmsLists[WMS_MEMORY_STORE_RUIM]->tags[i] == WMS_TAG_MT_NOT_READ)
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);
  #endif /* FEATURE_CDSMS_RUIM */

      /* NV - PHONE MEMORY */
      pai.wItemID = IDL_NV;
      pai.wText = IDS_SMS_NV;
      pai.wImage = IDB_PHONE_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->len;i++)
      {
        if((pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->tags[i] == WMS_TAG_MT_NOT_READ) &&
           (pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->voice_mail_index != i))
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);
    }
    else //GW
    {
      #if 0
      (void)IMENUCTL_SetTitle(pICurrentMenu, WMSAPP_RES_FILE,
                        IDS_SMS_APP_GW, NULL);
	  #else
		  {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_SMS_APP_GW,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		   }
		   #endif

      /*VOICEMAIL*/
      pai.wItemID = IDL_SMS_VOICEMAIL;
      pai.wText = IDS_SMS_VOICEMAIL;
      pai.wImage = IDB_VOICEMAIL;
      if(pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->voice_mail_active)
      {
        pai.wFont = AEE_FONT_BOLD;
      }
      else
      {
        pai.wFont = AEE_FONT_NORMAL;
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

      /* RAM - TEMP MEMORY */
      pai.wItemID = IDL_RAM;
      pai.wText = IDS_SMS_RAM;
      pai.wImage = IDB_TEMP_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_RAM_GW]->len;i++)
      {
        if(pMe->m_pSmsLists[WMS_MEMORY_STORE_RAM_GW]->tags[i] == WMS_TAG_MT_NOT_READ)
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

      /* SIM CARD MEMORY */
      pai.wItemID = IDL_SIM;
      pai.wText = IDS_SMS_SIM;
      pai.wImage = IDB_CARD_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->len;i++)
      {
        if((pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->tags[i] == WMS_TAG_MT_NOT_READ) &&
           (pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->voice_mail_index != i))
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

      /* NV - PHONE MEMORY */
      pai.wItemID = IDL_NV;
      pai.wText = IDS_SMS_NV;
      pai.wImage = IDB_PHONE_MEMORY;
      pai.wFont = AEE_FONT_NORMAL;
      for(i=0;i<pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_GW]->len;i++)
      {
        if(pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_GW]->tags[i] == WMS_TAG_MT_NOT_READ)
        {
          pai.wFont = AEE_FONT_BOLD;
          break;
        }
      }
      (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
    }

    /* COMPOSE */
    pai.wItemID = IDL_COMPOSE;
    pai.wText = IDS_COMPOSE;
    pai.wImage = IDB_COMPOSE;
    pai.wFont = AEE_FONT_NORMAL;
    (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

    /* SETTINGS */
    pai.wItemID = IDL_SETTINGS;
    pai.wText = IDS_SMS_SETTINGS;
    pai.wImage = IDB_SETTINGS;
    pai.wFont = AEE_FONT_NORMAL;
    (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

    /* TEST MENU */
    pai.wItemID = IDL_TEST_MENU;
    pai.wText = IDS_SMS_TESTMENU;
    pai.wImage = IDB_TEST;
    pai.wFont = AEE_FONT_NORMAL;
    (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);
  }

  /* COMPOSE */
  pai.wItemID = IDL_COMPOSE;
  pai.wText = IDS_COMPOSE;
  pai.wImage = IDB_COMPOSE;
  pai.wFont = AEE_FONT_NORMAL;
  (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

  /* SETTINGS */
  pai.wItemID = IDL_SETTINGS;
  pai.wText = IDS_SMS_SETTINGS;
  pai.wImage = IDB_SETTINGS;
  pai.wFont = AEE_FONT_NORMAL;
  (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);

  if(pMe->m_bTestMode == TRUE)
  {
    /* TEST MENU */
    pai.wItemID = IDL_TEST_MENU;
    pai.wText = IDS_SMS_TESTMENU;
    pai.wImage = IDB_TEST;
    pai.wFont = AEE_FONT_NORMAL;
    (void)IMENUCTL_AddItemEx(pICurrentMenu, &pai);
  }

  (void)WMSAPPMN_InitMenuSoftKey(pMe);

  /* To Refresh the Menu Control */
  (void)IMENUCTL_Redraw(pICurrentMenu);
  /* Check if an alert started the app */
  if(pMe->m_bDisplayNewMsgAlert)
  {
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                       EVT_WMS_PROCESS_NEW_MSG, 0, 0);
  }
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitMsgInfoDlg

DESCRIPTION
  Initializes the Msg Info Dlg

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_InitMsgInfoDlg
(
  CWMSAPP *pMe
)
{

   //IMenuCtl* pIMenu;
   //IDialog *pIDialog;
   int ret_status;

   ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                         IDD_VIEW_MESSAGE_INFO, NULL);
   if(ret_status ==EFAILED)
    return FALSE;


    //pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

   //IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   //pIMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,IDC_SK_MESSAGE_INFO_NEXT);

   return WMSAPPMD_DisplayMessageInfo(pMe);

   //(void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenu);

   //return TRUE;

}

/*===========================================================================
FUNCTION WMSAPPMN_GetNumber

DESCRIPTION
  Extracts the Number from the Text dialog and saves it for later use

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_GetNumber(CWMSAPP *pMe)
{
  IDialog * pIDialog;           // Dialog to be opened
  ITextCtl * pIDestAddr  ;      // Pointer to text box for data entry
  AECHAR *destAddr;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  uint32 dwProps;
  uint32 length=0;

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  pIDestAddr = (ITextCtl * )IDIALOG_GetControl(pIDialog,IDC_DESTADDR);
  if(!pIDestAddr)
  {
    return FALSE;
  }

  dwProps = ITEXTCTL_GetProperties(pIDestAddr);

  dwProps &= ~TP_NODRAW;

  ITEXTCTL_SetProperties(pIDestAddr, dwProps);
  (void)ITEXTCTL_Redraw(pIDestAddr);

  destAddr = MALLOC(WMS_SMS_UDL_MAX_7_BIT);

  if (destAddr == NULL) {
    return FALSE;
  }


  switch(pMe->m_addrType)
  {
    case WMSAPP_MSG_TYPE_PHONE_NUM:
      /* Setting the Text Input Mode to Numeric */

      (void)ITEXTCTL_SetInputMode (pIDestAddr, OEM_TM_DIALED_DIGITS);
      if(pMe->m_modeControl == WMSAPP_MODE_GW)
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/
      }
      else
      {
#ifdef FEATURE_CDSMS
        if (pMe->m_pClient_msg_mo->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_8_BIT &&
            pMe->m_pClient_msg_mo->u.cdma_message.address.number_type == WMS_NUMBER_INTERNATIONAL &&
            pMe->m_pClient_msg_mo->u.cdma_message.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK)
        {
          pMe->m_buf[0]='+';
          (void)MEMCPY(pMe->m_buf+1, pMe->m_pClient_msg_mo->u.cdma_message.address.digits,
                 pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits);
          /* increment num of digits to add '+' */
          pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits++;
        }
        else
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits,
                                    (uint8*)pMe->m_pClient_msg_mo->u.cdma_message.address.digits,
                                    (uint8*)pMe->m_buf);
        }
        (void)STRTOWSTR((char *)pMe->m_buf,
                   destAddr,
                   sizeof(AECHAR) * (pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits +1));
        length = pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits;

#endif /*FEATURE_CDSMS*/
      }
      break;

    case WMSAPP_MSG_TYPE_EMAIL:
      if(pMe->m_modeControl == WMSAPP_MODE_GW)
      {
        (void)STRTOWSTR((char *)pMe->m_emailAddr.address, destAddr, sizeof(AECHAR) * (pMe->m_emailAddr.len +1));
        length = pMe->m_emailAddr.len;
      }
      else
      {
#ifdef FEATURE_CDSMS
        (void)STRTOWSTR((char *)pMe->m_pClient_msg_mo->u.cdma_message.address.digits, destAddr,
                  sizeof(AECHAR) * (pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits +1));
        length = pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits;
#endif
      }
      /* Setting the Text Input Mode to Alphabets */
      #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
      (void)ITEXTCTL_SetInputMode (pIDestAddr, AEE_TM_RAPID);
      #else
      (void)ITEXTCTL_SetInputMode (pIDestAddr, AEE_TM_LETTERS);
      #endif
      break;

#ifdef FEATURE_CDSMS
    case WMSAPP_MSG_TYPE_NON_STD_EMAIL:
      /* Get email address from the buffer */
      (void)STRTOWSTR((char *)pMe->m_emailAddr.address, destAddr, (sizeof(AECHAR) * (pMe->m_emailAddr.len) +1));
      length = pMe->m_emailAddr.len;

      /* Setting the Text Input Mode to Alphabets */
      #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
      (void)ITEXTCTL_SetInputMode (pIDestAddr, AEE_TM_RAPID);
      #else
      (void)ITEXTCTL_SetInputMode (pIDestAddr, AEE_TM_LETTERS);
      #endif
      break;
#endif /* FEATURE_CDSMS */
    default:
      break;
  }
  (void)ITEXTCTL_SetText (pIDestAddr, destAddr, WMS_SMS_UDL_MAX_7_BIT);
  ITEXTCTL_SetActive(pIDestAddr,TRUE);
  ITEXTCTL_SetCursorPos(pIDestAddr, (int32)length);
  WMSAPPU_FREE(destAddr);
  return TRUE;
}
/*===========================================================================
FUNCTION WMSAPPMN_StoreNumber

DESCRIPTION
  Extracts the Number from the Text dialog and saves it for later use

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_StoreNumber(CWMSAPP *pMe)
{
  IDialog * pIDialog;           // Dialog to be opened
  ITextCtl * pIDestAddr  ;      // Pointer to text box for data entry
  int   length;                 // Temporary length
  AECHAR *destAddr;
  wms_address_s_type    newAddress;
  boolean is_international_address;
  uint8 loopCounter, digitCounter = 0;

  ZEROAT(&newAddress);

#ifdef FEATURE_CDSMS
  /* Initialize the non std email flag to FALSE */
  pMe->m_emailAddr.isNonStdEmail = FALSE;
#endif /* FEATURE_CDSMS */

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  pIDestAddr = (ITextCtl * )IDIALOG_GetControl(pIDialog,IDC_DESTADDR);

  if(!pIDestAddr)
  {
    return FALSE;
  }

  destAddr = MALLOC((WMS_ADDRESS_MAX+1)*sizeof(AECHAR));

  if (destAddr == NULL) {
    return FALSE;
  }

  (void)ITEXTCTL_GetText (pIDestAddr, destAddr, WMS_ADDRESS_MAX);

  (void)ITEXTCTL_SetText (pIDestAddr, NULL, 0);

  /* Calculate the length of destination Address string */
  length = WSTRLEN(destAddr);

  is_international_address = FALSE;

  switch(pMe->m_addrType)
  {
    case WMSAPP_MSG_TYPE_PHONE_NUM:

      (void)WSTRTOSTR(destAddr,
                (char*)newAddress.digits,
                length+1);

      if (ITEXTCTL_GetInputMode(pIDestAddr, NULL) == OEM_TM_DIALED_DIGITS)
      {
        /* Check for + symbol and to remove it
          by setting the Number Type to International */
        if ((char)newAddress.digits[0] == '+')
        {
          /* First digit is +, hence International Number Type */
          is_international_address = TRUE;
        }

        /* To Strip out only the all the first + signs in the Addr */
        for (loopCounter=0; loopCounter < length; loopCounter++)
        {
          if (((char)newAddress.digits[loopCounter] != '+')
              ||(loopCounter != 0)) 
          {
            /* Keep it in newAddress */
            newAddress.digits[digitCounter] = newAddress.digits[loopCounter];
            digitCounter++;
          }
        }
        newAddress.number_of_digits = digitCounter;
      }
      else
      {
        newAddress.number_of_digits = (uint8)length;
      }



      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        newAddress.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
        newAddress.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
        if (is_international_address)
        {
          newAddress.digit_mode  = WMS_DIGIT_MODE_8_BIT;
          newAddress.number_type = WMS_NUMBER_INTERNATIONAL;
        }
        else
        {
          if(pMe->m_bTestMode)
          {
            (void)MEMSET(pMe->m_buf, 0, sizeof(pMe->m_buf));
            (void)MEMCPY(pMe->m_buf, newAddress.digits, digitCounter);
            (void)MEMSET(newAddress.digits, 0, sizeof(newAddress.digits));
            (void)IWMS_TsAsciiToDtmf( pMe->m_pwms, (char *)pMe->m_buf,
                                      (uint8 *)newAddress.digits);
          }
          newAddress.number_type = WMS_NUMBER_UNKNOWN;
          newAddress.digit_mode = WMS_DIGIT_MODE_4_BIT;
        }
#endif /* FEATURE_CDSMS */
      }
      break;

    case WMSAPP_MSG_TYPE_EMAIL:
      newAddress.number_of_digits = (uint8)length;

      (void)WSTRTOSTR(destAddr,
                (char *)newAddress.digits,
                length+1);

      newAddress.digit_mode = WMS_DIGIT_MODE_8_BIT;

#ifdef FEATURE_CDSMS
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
        newAddress.number_mode = WMS_NUMBER_MODE_DATA_NETWORK;
        newAddress.number_type = WMS_NUMBER_INTERNET_EMAIL;

        /* This field is not used for 8-bit address types */
        newAddress.number_plan = WMS_NUMBER_PLAN_UNKNOWN;
      }
#endif /*FEATURE_CDSMS*/
      break;

#ifdef FEATURE_CDSMS
    case WMSAPP_MSG_TYPE_NON_STD_EMAIL:
      newAddress.digit_mode = WMS_DIGIT_MODE_4_BIT;
      newAddress.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
      newAddress.number_type = WMS_NUMBER_UNKNOWN;
      newAddress.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
      if(pMe->m_bTestMode)
      {
        (void)IWMS_TsAsciiToDtmf( pMe->m_pwms,
                                  "6245", (uint8 *)newAddress.digits);
      }
      else
      {
        (void)MEMCPY(newAddress.digits, "6245", 4);
      }
      newAddress.number_of_digits = 4;

      /* Store in email buffer for later use */
      (void)WSTRTOSTR(destAddr, (char *)pMe->m_emailAddr.address, length+1);
      pMe->m_emailAddr.isNonStdEmail = TRUE;
      pMe->m_emailAddr.len = (uint8)length;
      break;
#endif /* FEATURE_CDSMS */
    default:
      break;
  }

/*-----------------------------TEMPLATE PROCESSING------------------------------------------------------------*/
  if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
  {
    if(pMe->m_modeControl == WMSAPP_MODE_GW)
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    }
    else
    {
#ifdef FEATURE_CDSMS
      if(pMe->m_currPref == IDL_C_TMPLT_DEST_ADDR)
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |=
          WMS_CDMA_TEMPLATE_MASK_DEST_ADDR;

        /* Check if we currently have a non-std email address in the template */
        if(WMSAPPU_IsNonStdEmail(TRUE, pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                                 pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits))
        {
          WMSAPPU_RemoveNonStdEmail(pMe, &pMe->m_pMO_template_msg->u.cdma_template.client_bd);

          /* clear appropriate masks if needed */
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &=
              ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }

        /* Special handling of non-std email */
        if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_NON_STD_EMAIL)
        {
          if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
             (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
          {
            WMSAPPU_PutNonStdEmail(pMe->m_emailAddr.address, pMe,
                                   &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data);

            (void)MEMCPY((uint8 *)pMe->m_buf,
                   pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
                   pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len);
          }
          else
          {
            (void)WSTRTOSTR(destAddr,
                      (char *)pMe->m_buf,
                      length+1);

            /* Set the appropriate masks */
            pMe->m_pMO_template_msg->u.cdma_template.mask |=
              WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |=
              WMS_MASK_BD_USER_DATA;
            pMe->m_buf[length] = 0;
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding =
              WMS_ENCODING_ASCII;

          }
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits =
            IWMS_TsPackAscii( pMe->m_pwms, (char *)pMe->m_buf,
                              (uint8 *)pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
                              (uint8 *)&pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len,
                              (uint8 *)&pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.padding_bits);
        }

        /* Assign the new address */
        pMe->m_pMO_template_msg->u.cdma_template.address = newAddress;
      }
      else if(pMe->m_currPref == IDL_C_TMPLT_CALLBACK)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback = newAddress;
        pMe->m_pMO_template_msg->u.cdma_template.mask |=
          WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |=
          WMS_MASK_BD_CALLBACK;
      }
#endif /* FEATURE_CDSMS */
    }
    WMSAPPU_FREE(destAddr);
    if(pMe->m_currPref == IDL_GW_TMPLT_DEST_ADDR ||
       pMe->m_currPref == IDL_GW_TMPLT_SC_ADDR)
    {
      return (WMSAPPMN_EndNDlgs(pMe,1));
    }
    return (WMSAPPMN_EndNDlgs(pMe,2));
  }
/*-------------------------SMS Messages--------------------------------------------*/
  else
  {
    if(pMe->m_modeControl == WMSAPP_MODE_GW)
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    }
    else
    {
#ifdef FEATURE_CDSMS
      if(pMe->m_currPref == IDL_OUTBOX_CALLBACK)
      {
        pMe->m_pClient_ts_mo->u.cdma.callback = newAddress;
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_CALLBACK;
        WMSAPPU_FREE(destAddr);
        return(WMSAPPMN_EndNDlgs(pMe,2));
      }
      else /* Dest Addr for a message */
      {
        pMe->m_pClient_msg_mo->u.cdma_message.address = newAddress;
        /* Set up for entering text */
        if(pMe->m_msg_type == WMSAPP_MSG_TYPE_NEW)
        {

          (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.user_data,0,
               sizeof(wms_cdma_user_data_s_type));
          pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers =0;
        }
      }
#endif /* FEATURE_CDSMS */
    }

    if(pMe->m_msg_type == WMSAPP_MSG_TYPE_CANNED)
    {
      (void)MEMCPY(pMe->m_pClient_ts, pMe->m_pClient_ts_mo, sizeof(wms_client_ts_data_s_type));
      (void)MEMCPY(pMe->m_pClient_msg, pMe->m_pClient_msg_mo, sizeof(wms_client_message_s_type));
      if (pMe->m_modeControl == WMSAPP_MODE_GW)
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      if (pMe->m_bUsepUserData == TRUE)
      {
#ifdef FEATURE_SMS_UDH
        uint8 i, numSegments = 0;
        wmsapp_user_data_s_type *segments = NULL;

        /* Message to be Segmented */
        if (!WMSAPPU_SplitUserData(pMe, pMe->m_pUserData, &numSegments, &segments))
        {
          MSG_ERROR("Segmentation Failed",0,0,0);
          if (!WMSAPPMN_EndDlg(pMe))
          {
            MSG_ERROR("WMSAPPMN_EndDlg Failed!",0,0,0);
          }
          if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, "Segmentation Error", SELECTIONVERIFICATIONTIME))
          {
            MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed!",0,0,0);
          }
        }
        else
        {
          for ( i=0 ; i <numSegments ; i++)
          {
            if (pMe->m_modeControl == WMSAPP_MODE_GW)
            {
              /* GW */
              pMe->m_pClient_ts->u.gw_pp.u.submit.user_data_header_present = TRUE;
              pMe->m_pClient_ts->u.gw_pp.u.submit.dcs.alphabet = (wms_gw_alphabet_e_type)segments[i].encoding;
              pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.num_headers = segments[i].num_headers;
              (void)MEMCPY(pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.headers, segments[i].headers, segments[i].num_headers * sizeof(wms_udh_s_type));
              if (segments[i].encoding == WMSAPP_ENCODING_16_BIT)
              {
                pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len = (uint16)(segments[i].num_chars * 2);
              }
              else
              {
                pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len = segments[i].num_chars;
              }

              (void)MEMCPY(pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_data, segments[i].data, pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len);

              if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts, &pMe->m_pClient_msg->u.gw_message.raw_ts_data)
                          != SUCCESS)
              {
                MSG_ERROR("WMS App encode ts failed! for segment %d",i+1,0,0);
              }
              else
              {
                if (pMe->m_bStoreMsg == FALSE)
                {
                  if (IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                  (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE,
                                  pMe->m_pClient_msg) != SUCCESS)
                  {
                    MSG_ERROR("WMS App Send Segment Failed for segment %d",i+1,0,0);
                  }
                  else
                  {
                    MSG_MED("WMS App Segment %d was sent successfully",i+1,0,0);
                  }
                }
                else
                {
                  pMe->m_pClient_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_GW;
                  if(IWMS_MsgWrite( pMe->m_pwms,pMe->m_clientId,
                          &pMe->m_callback,(void*)pMe,WMS_WRITE_MODE_INSERT,
                          pMe->m_pClient_msg) != SUCCESS)
                  {
                    MSG_ERROR("WMS App Store Segment Failed for segment %d",i+1,0,0);
                  }
                  else
                  {
                    MSG_MED("WMS App Segment %d was stored successfully",i+1,0,0);
                  }
                }
              }
            }
            else
            {
              /* CDMA */
              pMe->m_pClient_ts->u.cdma.user_data.num_headers = segments[i].num_headers;
              switch(segments[i].encoding)
              {
                case WMSAPP_ENCODING_7_BIT:
                  pMe->m_pClient_ts->u.cdma.user_data.encoding = WMS_ENCODING_GSM_7_BIT_DEFAULT;
                  break;

                case WMSAPP_ENCODING_8_BIT:
                  pMe->m_pClient_ts->u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
                  break;

                case WMSAPP_ENCODING_16_BIT:
                  pMe->m_pClient_ts->u.cdma.user_data.encoding = WMS_ENCODING_UNICODE;
                  break;
              }
              (void)MEMCPY(pMe->m_pClient_ts->u.cdma.user_data.headers, segments[i].headers, segments[i].num_headers * sizeof(wms_udh_s_type));

              if (segments[i].encoding == WMSAPP_ENCODING_16_BIT)
              {
                pMe->m_pClient_ts->u.cdma.user_data.data_len = (uint8)(segments[i].num_chars * 2);
                pMe->m_pClient_ts->u.cdma.user_data.number_of_digits = (uint8)(segments[i].num_chars);
              }
              else
              {
                pMe->m_pClient_ts->u.cdma.user_data.data_len = (uint8)(segments[i].num_chars);
                pMe->m_pClient_ts->u.cdma.user_data.number_of_digits = (uint8)(segments[i].num_chars);
              }

              (void)MEMCPY(pMe->m_pClient_ts->u.cdma.user_data.data, segments[i].data, pMe->m_pClient_ts->u.cdma.user_data.data_len);

              /* Initialize other to 0 */
              (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );
             
              if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts, &pMe->m_pClient_msg->u.cdma_message.raw_ts)
                          != SUCCESS)
              {
                MSG_ERROR("WMS App encode ts failed! for segment %d",i+1,0,0);
              }
              if (pMe->m_bStoreMsg == FALSE)
              {
                if (IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE,
                                pMe->m_pClient_msg) != SUCCESS)
                {
                  MSG_ERROR("WMS App Send Segment Failed for segment %d",i+1,0,0);
                }
                else
                {
                  MSG_MED("WMS App Segment %d was sent successfully",i+1,0,0);
                }
              }
              else
              {
                pMe->m_pClient_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
                if(IWMS_MsgWrite( pMe->m_pwms,pMe->m_clientId,
                        &pMe->m_callback,(void*)pMe,WMS_WRITE_MODE_INSERT,
                        pMe->m_pClient_msg) != SUCCESS)
                {
                    MSG_ERROR("WMS App Store Segment Failed for segment %d",i+1,0,0);
                }
                else
                {
                  MSG_MED("WMS App Segment %d was stored successfully",i+1,0,0);
                }
              }
            }
            if (segments[i].data)
            {
              WMSAPPU_FREE(segments[i].data);
              segments[i].data = NULL;
            }
            if (segments[i].headers)
            {
              WMSAPPU_FREE(segments[i].headers);
              segments[i].headers = NULL;
            }
          }
          if (!WMSAPPMN_EndDlg(pMe))
          {
            MSG_ERROR("WMSAPPMN_EndDlg Failed!",0,0,0);
          }
          if (pMe->m_bStoreMsg == FALSE)
          {
            if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, "Sending Segments", SELECTIONVERIFICATIONTIME))
            {
              MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed!",0,0,0);
            }
          }
          else
          {
            if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, "Storing Segments", SELECTIONVERIFICATIONTIME))
            {
              MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed!",0,0,0);
            }
          }
          if (segments)
          {
            WMSAPPU_FREE(segments);
          }
        }

        if (pMe->m_pUserData->data)
        {
          WMSAPPU_FREE(pMe->m_pUserData->data);
          pMe->m_pUserData->data = NULL;
        }
        if (pMe->m_pUserData->headers)
        {
          WMSAPPU_FREE(pMe->m_pUserData->headers);
          pMe->m_pUserData->headers = NULL;
        }

        /* Setting the Flag to False */
        pMe->m_bUsepUserData = FALSE;

        WMSAPPU_FREE(destAddr);
#endif /* FEATURE_SMS_UDH */
        return TRUE;
      }
      else if (pMe->m_bsegmentEO == TRUE)
      {
#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
        uint8 i, j=0, pos = 0, numSegments;
        /* each segment comprises of a concat_16 and an extended object */
        numSegments = pMe->m_pUserData->num_headers / 2;

        for (i=0; i < numSegments; i++)
        {
          if (pMe->m_modeControl == WMSAPP_MODE_GW)
          {
            /* GW */
            pMe->m_pClient_ts->u.gw_pp.u.submit.user_data_header_present = TRUE;
            pMe->m_pClient_ts->u.gw_pp.u.submit.dcs.alphabet = (wms_gw_alphabet_e_type)pMe->m_pUserData->encoding;
            /* 1st header for concat_16, 2nd header for extended object */
            pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.num_headers = 2;

            pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.headers[0].header_id = WMS_UDH_CONCAT_16;
            (void)MEMCPY(&pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.headers[0].u.concat_16, &pMe->m_pUserData->headers[j++].u.concat_16, sizeof(wms_udh_concat_16_s_type)+1);

            pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.headers[1].header_id = WMS_UDH_EXTENDED_OBJECT;
            (void)MEMCPY(&pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.headers[1].u.eo, &pMe->m_pUserData->headers[j++].u.eo, sizeof(wms_udh_eo_s_type)+1);

            /* total user data = 45, 15 uint8s for each segment (3 segments total) */
            pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len = pMe->m_pUserData->num_chars / numSegments;
            (void)MEMCPY(pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_data, pMe->m_pUserData->data + pos, pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len);
            pos += (uint8)pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len;

            if (IWMS_TsEncode(pMe->m_pwms, pMe->m_pClient_ts, &pMe->m_pClient_msg->u.gw_message.raw_ts_data) != SUCCESS)
            {
              MSG_ERROR("WMSAPP encoding failed for extended object segment number %d",i+1,0,0);
            }
            else
            {
              if (IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE, pMe->m_pClient_msg) != SUCCESS)
              {
                MSG_ERROR("WMSAPP send extended object segment failed for segment number %d",i+1,0,0);
              }
              else
              {
                MSG_MED("WMSAPP send extended object was sent successfully for segment number %d",i+1,0,0);
              }
            }
          }
          else
          {
            /* CDMA */
            pMe->m_pClient_ts->u.cdma.user_data.encoding = (wms_user_data_encoding_e_type)pMe->m_pUserData->encoding;
            pMe->m_pClient_ts->u.cdma.user_data.num_headers = 2;

            pMe->m_pClient_ts->u.cdma.user_data.headers[0].header_id = WMS_UDH_CONCAT_16;
            (void)MEMCPY(&pMe->m_pClient_ts->u.cdma.user_data.headers[0].u.concat_16, &pMe->m_pUserData->headers[j++].u.concat_16, sizeof(wms_udh_concat_16_s_type)+1);

            pMe->m_pClient_ts->u.cdma.user_data.headers[1].header_id = WMS_UDH_EXTENDED_OBJECT;
            (void)MEMCPY(&pMe->m_pClient_ts->u.cdma.user_data.headers[1].u.eo, &pMe->m_pUserData->headers[j++].u.eo, sizeof(wms_udh_eo_s_type)+1);

            /* total user data = 45, 15 uint8s for each segment (3 segments total) */
            pMe->m_pClient_ts->u.cdma.user_data.data_len = (uint8)(pMe->m_pUserData->num_chars / numSegments);
            (void)MEMCPY(pMe->m_pClient_ts->u.cdma.user_data.data, pMe->m_pUserData->data + pos, pMe->m_pClient_ts->u.gw_pp.u.submit.user_data.sm_len);
            pos += pMe->m_pClient_ts->u.cdma.user_data.data_len;

            /* Initialize other to 0 */
            (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

            if (IWMS_TsEncode(pMe->m_pwms, pMe->m_pClient_ts, &pMe->m_pClient_msg->u.cdma_message.raw_ts) != SUCCESS)
            {
              MSG_ERROR("WMSAPP encoding failed for extended object segment number %d",i+1,0,0);
            }
            else
            {
              if (IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE, pMe->m_pClient_msg) != SUCCESS)
              {
                MSG_ERROR("WMSAPP send extended object segment failed for segment number %d",i+1,0,0);
              }
              else
              {
                MSG_MED("WMSAPP send extended object was sent successfully for segment number %d",i+1,0,0);
              }
            }
          }
        }

        if (pMe->m_pUserData->data)
        {
          WMSAPPU_FREE(pMe->m_pUserData->data);
          pMe->m_pUserData->data = NULL;
        }
        if (pMe->m_pUserData->headers)
        {
          WMSAPPU_FREE(pMe->m_pUserData->headers);
          pMe->m_pUserData->headers = NULL;
        }

        /* Setting the Flag to False */
        pMe->m_bsegmentEO = FALSE;
#endif /* FEATURE_SMS_UDH */
      }
      else
      {
        if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
        {
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                   IDD_OUTBOX_VIEW, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
        }
        else if(pMe->m_msgInfo.boxType == WMSAPP_INBOX)
        {
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                          IDD_INBOX_VIEW, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
        }
        (void) WMSAPPMD_Display(pMe);
      }
    }
    else
    {
      if(ISHELL_CreateDialog(pMe->a.m_pIShell,WMSAPP_RES_FILE,
                           IDD_COMPOSE_MSG, NULL)!= SUCCESS)
      {
        WMSAPPU_FREE(destAddr);
        return FALSE; /* ERROR */
      }
    }

  }
  WMSAPPU_FREE(destAddr);

  {
    uint32 dwProps = ITEXTCTL_GetProperties(pIDestAddr);
    ITEXTCTL_SetProperties(pIDestAddr, dwProps|TP_NODRAW);
  }

  return TRUE;
}/*WMSAPPMN_StoreNumber*/

/*===========================================================================
FUNCTION WMSAPPMN_ProcessError

DESCRIPTION
  Used by WMS to send errors generated during a command

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMN_ProcessError
(
wms_cmd_id_e_type        cmd,
/* which command's status being reported
*/
void                    *user_data,
wms_cmd_err_e_type       cmd_err
/* the status of the command
*/
)
{
  CWMSAPP * pMe = (CWMSAPP *) (user_data);
  if((cmd_err != WMS_CMD_ERR_NONE) && (pMe != NULL) && (pMe->a.m_nRefs))
  {

    wmsapp_cmd_err_type *wmsapp_cmd_error = MALLOC(sizeof(wmsapp_cmd_err_type));

    if(wmsapp_cmd_error == NULL)
    {
      return FALSE;
    }
    wmsapp_cmd_error->cmd = cmd;
    wmsapp_cmd_error->cmd_err = cmd_err;
    wmsapp_cmd_error->user_data = user_data;

    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                       EVT_WMS_CMD_ERR, 0, (uint32)wmsapp_cmd_error);
  }

  return TRUE;
}

boolean WMSAPPMN_ProcessCardError
(
  void      *pNotifyData
)
{
  CWMSAPP * pMe = (CWMSAPP *)pNotifyData;

  if (pMe->m_pCardCmdVMWriteStatus->nCmdStatus != SUCCESS)
  {
    MSG_ERROR("WMSAPP: Writing to ICARD failed!",0,0,0);
    (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Writing to ICARD failed!", SELECTIONVERIFICATIONTIME);
    WMSAPPU_FREE(pMe->m_pCardCmdVMWriteStatus);
    return FALSE;
  }

  WMSAPPU_FREE(pMe->m_pCardCmdVMWriteStatus);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_GetTemplateListResID

DESCRIPTION
  Gets the Resource ID for the template list

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
uint16 WMSAPPMN_GetTemplateListResID
(
  int index
)
{
  switch(index)
  {
    case 0:
     return IDL_TEMPLATE_1;

    case 1:
      return IDL_TEMPLATE_2;

    case 2:
      return IDL_TEMPLATE_3;

    case 3:
      return IDL_TEMPLATE_4;

    case 4:
      return IDL_TEMPLATE_5;

    case 5:
      return IDL_TEMPLATE_6;

    case 6:
      return IDL_TEMPLATE_7;

    case 7:
      return IDL_TEMPLATE_8;

    case 8:
      return IDL_TEMPLATE_9;

    case 9:
      return IDL_TEMPLATE_10;

    default:
      return 0;
  }
}
/*===========================================================================
FUNCTION WMSAPPMN_GetListIndex

DESCRIPTION
  Gets the index from the resource ids

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMN_GetListIndex
(
  CWMSAPP *pMe,
  uint16 wItemID
)
{
  uint8 i=0;

  if(pMe->m_msgInfo.boxType != WMSAPP_TEMPLATE)
  {
    for(i=0;i<100;i++)
    {
      if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NV_CDMA ||
         pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NV_GW)
      {
        if(wItemID == WMSAPP_NV_MSG[i])
          return i+1;
      }

      if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RAM_CDMA ||
         pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RAM_GW)
      {
        if(wItemID == WMSAPP_TEMP_MSG[i])
          return i+1;
      }
      if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_SIM ||
         pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RUIM)
      {
        if(wItemID == WMSAPP_CARD_MSG[i])
          return i+1;
      }
    }
    return  0;
  }
  switch(wItemID)
  {
    case IDL_TEMPLATE_1:
      return 0;
    case IDL_TEMPLATE_2:
      return 1;
    case IDL_TEMPLATE_3:
      return 2;
    case IDL_TEMPLATE_4:
      return 3;
    case IDL_TEMPLATE_5:
      return 4;
    case IDL_TEMPLATE_6:
      return 5;
    case IDL_TEMPLATE_7:
      return 6;
    case IDL_TEMPLATE_8:
      return 7;
    case IDL_TEMPLATE_9:
      return 8;
    case IDL_TEMPLATE_10:
      return 9;

    default:
      return 0;
  }
}

/*===========================================================================
FUNCTION WMSAPPMN_GetMelodyListResID

DESCRIPTION
  Gets the Resource ID for the Melody list
  Returns the appropriate ResourceID based on whether it was
  Predefined Melody or Not (passed as boolean parameter)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
uint16 WMSAPPMN_GetMelodyListResID
(
  int index,
  boolean preDefined
)
{
  switch(index)
  {
    case 0:
      if (preDefined)
        return IDL_MELODY_CHIMES_HIGH;
      else
        return IDL_MELODY_1;

    case 1:
      if (preDefined)
        return IDL_MELODY_CHIMES_LOW;
      else
        return IDL_MELODY_2;

    case 2:
      if (preDefined)
        return IDL_MELODY_DING;
      else
        return IDL_MELODY_3;

    case 3:
      if (preDefined)
        return IDL_MELODY_TADA;
      else
        return IDL_MELODY_4;

    case 4:
      if (preDefined)
        return IDL_MELODY_NOTIFY;
      else
        return IDL_MELODY_5;

    case 5:
      if (preDefined)
        return IDL_MELODY_DRUM;
      else
        return IDL_MELODY_6;

    case 6:
      if (preDefined)
        return IDL_MELODY_CLAPS;
      else
        return IDL_MELODY_7;

    case 7:
      if (preDefined)
        return IDL_MELODY_FANFAR;
      else
        return IDL_MELODY_8;

    case 8:
      if (preDefined)
        return IDL_MELODY_CHORD_HIGH;
      else
        return IDL_MELODY_9;

    case 9:
      if (preDefined)
        return IDL_MELODY_CHORD_LOW;
      else
        return IDL_MELODY_10;

    default:
      return 0;
  }
} /* WMSAPPMN_GetMelodyListResID */

#ifdef FEATURE_SMS_UDH
/*===========================================================================
FUNCTION WMSAPPMN_GetMelodyListIndex

DESCRIPTION
  Gets the index from the Melody resource ids

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMN_GetMelodyListIndex
(
  uint16 wItemID
)
{
  switch(wItemID)
  {
    case IDL_MELODY_CHIMES_HIGH:
    case IDL_MELODY_1:
      return 0;

    case IDL_MELODY_CHIMES_LOW:
    case IDL_MELODY_2:
      return 1;

    case IDL_MELODY_DING:
    case IDL_MELODY_3:
      return 2;

    case IDL_MELODY_TADA:
    case IDL_MELODY_4:
      return 3;

    case IDL_MELODY_NOTIFY:
    case IDL_MELODY_5:
      return 4;

    case IDL_MELODY_DRUM:
    case IDL_MELODY_6:
      return 5;

    case IDL_MELODY_CLAPS:
    case IDL_MELODY_7:
      return 6;

    case IDL_MELODY_FANFAR:
    case IDL_MELODY_8:
      return 7;

    case IDL_MELODY_CHORD_HIGH:
    case IDL_MELODY_9:
      return 8;

    case IDL_MELODY_CHORD_LOW:
    case IDL_MELODY_10:
      return 9;

    default:
      return 0;
  }
} /* WMSAPPMN_GetMelodyListIndex */
#endif /*FEATURE_SMS_UDH */


#ifndef FEATURE_SMS_EXTENDED
/*===========================================================================
FUNCTION WMSAPPMN_GetPreDefAnimListResID

DESCRIPTION
  Gets the Resource ID for the Predefined animation list

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
uint16 WMSAPPMN_GetPreDefAnimListResID
(
  int index
)
{
  switch(index)
  {
    case 0:
      return IDL_PREDEF_ANIM_1;

    case 1:
      return IDL_PREDEF_ANIM_2;

    case 2:
      return IDL_PREDEF_ANIM_3;

    case 3:
      return IDL_PREDEF_ANIM_4;

    case 4:
      return IDL_PREDEF_ANIM_5;

    case 5:
      return IDL_PREDEF_ANIM_6;

    case 6:
      return IDL_PREDEF_ANIM_7;

    case 7:
      return IDL_PREDEF_ANIM_8;

    case 8:
      return IDL_PREDEF_ANIM_9;

    case 9:
      return IDL_PREDEF_ANIM_10;

    case 10:
      return IDL_PREDEF_ANIM_11;

    case 11:
      return IDL_PREDEF_ANIM_12;

    case 12:
      return IDL_PREDEF_ANIM_13;

    case 13:
      return IDL_PREDEF_ANIM_14;

    case 14:
      return IDL_PREDEF_ANIM_15;

    case 15:
      return IDL_PREDEF_ANIM_16;

    default:
      return 0;
  }
} /* WMSAPPMN_GetPreDefAnimListResID */
#endif /* !FEATURE_SMS_EXTENDED */

#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
/*===========================================================================
FUNCTION WMSAPPMN_GetPreDefAnimListIndex

DESCRIPTION
  Gets the index from the resource ids

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMN_GetPreDefAnimListIndex
(
  uint16 wItemID
)
{
  switch(wItemID)
  {
    case IDL_PREDEF_ANIM_1:
      return 0;

    case IDL_PREDEF_ANIM_2:
      return 1;

    case IDL_PREDEF_ANIM_3:
      return 2;

    case IDL_PREDEF_ANIM_4:
      return 3;

    case IDL_PREDEF_ANIM_5:
      return 4;

    case IDL_PREDEF_ANIM_6:
      return 5;

    case IDL_PREDEF_ANIM_7:
      return 6;

    case IDL_PREDEF_ANIM_8:
      return 7;

    case IDL_PREDEF_ANIM_9:
      return 8;

    case IDL_PREDEF_ANIM_10:
      return 9;

    case IDL_PREDEF_ANIM_11:
      return 10;

    case IDL_PREDEF_ANIM_12:
      return 11;

    case IDL_PREDEF_ANIM_13:
      return 12;

    case IDL_PREDEF_ANIM_14:
      return 13;

    case IDL_PREDEF_ANIM_15:
      return 14;

    default:
      return 0;
  }
} /* WMSAPPMN_GetPreDefAnimListIndex */
#endif /* FEATURE_SMS_UDH && !FEATURE_SMS_EXTENDED */

/*===========================================================================
FUNCTION WMSAPPMN_ReadTemplate

DESCRIPTION
  Given the index number of the template, this function reads the template

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_ReadTemplate
(
  CWMSAPP *pMe,
  uint32 index
)
{

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if (index == (uint32)WMS_DUMMY_MESSAGE_INDEX)
  {
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
      (void)IWMS_MsgReadTemplate(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,
                           (void*)pMe,WMS_MEMORY_STORE_NV_CDMA,
                           (uint32)WMS_DUMMY_MESSAGE_INDEX );
    }
    else if(pMe->m_modeControl == WMSAPP_MODE_GW)
    {
      (void)IWMS_MsgReadTemplate(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,
                           (void*)pMe,WMS_MEMORY_STORE_NV_GW,
                           (uint32)WMS_DUMMY_MESSAGE_INDEX );
    }
  }
  else
  {
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS_RUIM
      (void)IWMS_MsgReadTemplate(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,
                           (void*)pMe,WMS_MEMORY_STORE_RUIM, index );
#endif /* FEATURE_CDSMS_RUIM */
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    }
  }
}

/*===========================================================================
FUNCTION WMSAPPMN_ReadDefaultTemplate

DESCRIPTION
  Reads the default Template

DEPENDENCIES
  WMSAPPMN_ReadTemplate

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_ReadDefaultTemplate
(
 CWMSAPP *pMe
)
{
  int index=0;

  wmsapp_box_type boxType = pMe->m_msgInfo.boxType;
  pMe->m_msgInfo.boxType = WMSAPP_TEMPLATE;

  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    if (pMe->m_CDSMSDefaultTemplate > pMe->m_pCDSMSTemplateList->len)
    {
      pMe->m_CDSMSDefaultTemplate = (uint8)pMe->m_pCDSMSTemplateList->len;
      (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_CDMA_DEFAULT_TEMPLATE, &pMe->m_CDSMSDefaultTemplate, 1);
    }
    index = pMe->m_CDSMSDefaultTemplate;
  }
  else
  {
    if (pMe->m_GWSMSDefaultTemplate > pMe->m_pGWSMSTemplateList->len)
    {
      pMe->m_GWSMSDefaultTemplate = (uint8)pMe->m_pGWSMSTemplateList->len;
      (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_GW_DEFAULT_TEMPLATE, &pMe->m_GWSMSDefaultTemplate, 1);
    }
    index = pMe->m_GWSMSDefaultTemplate;
  }

  pMe->m_msgInfo.boxType = boxType;

  if(index !=0)
  {
    if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
      WMSAPPMN_ReadTemplate(pMe, pMe->m_pCDSMSTemplateList->indices[index-1]);
    }
    else
    {
      WMSAPPMN_ReadTemplate(pMe, pMe->m_pGWSMSTemplateList->indices[index-1]);
    }
  }
  else
  {
    WMSAPPMN_ReadTemplate(pMe, (uint32)WMS_DUMMY_MESSAGE_INDEX);
  }
}

/*===========================================================================
FUNCTION WMSAPPMN_DeleteTemplate

DESCRIPTION
  Deletes the Template

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_DeleteTemplate
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if(pMe->m_modeControl == WMSAPP_MODE_GW)
  {
    (void)IWMS_MsgDeleteTemplate(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,
      (void*)pMe,WMS_MEMORY_STORE_SIM, pMe->m_msgInfo.smsIndex);
  }
  else
  {
    (void)IWMS_MsgDeleteTemplate(pMe->m_pwms,pMe->m_clientId, &pMe->m_callback,
                             (void*)pMe,WMS_MEMORY_STORE_RUIM, pMe->m_msgInfo.smsIndex);
  }
}

/*===========================================================================
FUNCTION WMSAPPMN_WriteDefaultNVTemplate

DESCRIPTION
  Writes Default Values to the NV Template

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_WriteDefaultNVTemplate
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
              (void*)&(pMe->m_cbData));

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    /* CDMA NV Template */
    pMe->m_pMO_template_msg->msg_hdr.index = (uint32)WMS_DUMMY_MESSAGE_INDEX;
    pMe->m_pMO_template_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
    pMe->m_pMO_template_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

    /* Writing Default Values fo NV Template */

    /* Teleservice */
    pMe->m_pMO_template_msg->u.cdma_template.teleservice = WMS_TELESERVICE_CMT_95;

    /* Mask */
    pMe->m_pMO_template_msg->u.cdma_template.mask = 0;

    (void)IWMS_MsgWriteTemplate(pMe->m_pwms, pMe->m_clientId,&pMe->m_callback,
             (void*)pMe, WMS_WRITE_MODE_REPLACE, pMe->m_pMO_template_msg);
  }
  else
  {
    /* GW NV Template */
    pMe->m_pMO_template_msg->msg_hdr.index = (uint32)WMS_DUMMY_MESSAGE_INDEX;
    pMe->m_pMO_template_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_GW;
    pMe->m_pMO_template_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;

    /* Writing Default Values fo NV Template */

    /* Alpha ID */
    (void)MEMSET(pMe->m_AlphaID.data, 0, WMS_ALPHA_ID_MAX);
    pMe->m_AlphaID.len = 0;
    pMe->m_pMO_template_msg->u.gw_template.alpha_id = pMe->m_AlphaID;

    /* Mask */
    pMe->m_pMO_template_msg->u.gw_template.mask = WMS_GW_TEMPLATE_MASK_SC_ADDR
                                                  | WMS_GW_TEMPLATE_MASK_DEST_ADDR
                                                  | WMS_GW_TEMPLATE_MASK_PID
                                                  | WMS_GW_TEMPLATE_MASK_DCS
                                                  | WMS_GW_TEMPLATE_MASK_VALIDITY;

    /* Destination Address */
    pMe->m_pMO_template_msg->u.gw_template.dest_addr.number_of_digits = 0;
    pMe->m_pMO_template_msg->u.gw_template.dest_addr.number_type = WMS_NUMBER_INTERNATIONAL;
    pMe->m_pMO_template_msg->u.gw_template.dest_addr.number_plan = WMS_NUMBER_PLAN_TELEPHONY;

    /* SC Address */
    pMe->m_pMO_template_msg->u.gw_template.sc_addr.number_of_digits = 1;
    pMe->m_pMO_template_msg->u.gw_template.sc_addr.number_type = WMS_NUMBER_INTERNATIONAL;
    pMe->m_pMO_template_msg->u.gw_template.sc_addr.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    pMe->m_pMO_template_msg->u.gw_template.sc_addr.digits[0] = 7;

    /* PID */
    pMe->m_pMO_template_msg->u.gw_template.pid = WMS_PID_DEFAULT;

    /* DCS */
    pMe->m_pMO_template_msg->u.gw_template.dcs.is_compressed = FALSE;
    pMe->m_pMO_template_msg->u.gw_template.dcs.alphabet      = WMS_GW_ALPHABET_7_BIT_DEFAULT;
    pMe->m_pMO_template_msg->u.gw_template.dcs.msg_waiting   = WMS_GW_MSG_WAITING_NONE;
    pMe->m_pMO_template_msg->u.gw_template.dcs.msg_class         = WMS_MESSAGE_CLASS_NONE;

    /* Relative Validity */
    memset(&pMe->m_pMO_template_msg->u.gw_template.relative_validity, 0, sizeof(wms_timestamp_s_type));
    pMe->m_pMO_template_msg->u.gw_template.relative_validity.day = 1;

    (void)IWMS_MsgWriteTemplate(pMe->m_pwms, pMe->m_clientId,&pMe->m_callback,
             (void*)pMe, WMS_WRITE_MODE_REPLACE, pMe->m_pMO_template_msg);
  }
}

/*===========================================================================
FUNCTION WMSAPPMN_SaveSettings

DESCRIPTION
  Saves the Template/Message Settings

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_SaveSettings
(
  CWMSAPP *pMe
)
{
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wDlgID = IDIALOG_GetID(pIDialog);

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));


  switch(wDlgID)
  {
    /*It is a Template*/
    case IDD_CDMA_TEMPLATE_OPTIONS:
    case IDD_GW_TEMPLATE_OPTIONS:
      if (IWMS_MsgWriteTemplate(pMe->m_pwms, pMe->m_clientId,&pMe->m_callback,
                            (void*)pMe,pMe->m_msgInfo.write_mode ,pMe->m_pMO_template_msg) == SUCCESS)
      {
        return TRUE;
      }
      break;
    /*It is a message*/
    case IDD_CDMA_OUTBOX_PREF:
    case IDD_GW_OUTBOX_PREF:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
        /* Initialize other to 0 */
        (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

        if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts) != SUCCESS)
        {
          MSG_ERROR("Encoding failed!!",0,0,0);

          (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "Encoding Failure!", SELECTIONVERIFICATIONTIME);

          return FALSE;
        }
      }
      else
      {
        if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.gw_message.raw_ts_data) != SUCCESS)
        {
          MSG_ERROR("Encoding failed!!",0,0,0);

          (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "Encoding Failure!", SELECTIONVERIFICATIONTIME);

          return FALSE;
        }
      }
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      return TRUE;

    default:
      return FALSE;
  }

  return FALSE;
}


#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION WMSAPPMN_BCDeleteAllSvcs

DESCRIPTION
  Deletes All BC Services

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCDeleteAllSvcs
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));
  (void)IWMS_BcMmDeleteAllServices(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                           (void*)pMe, WMS_MESSAGE_MODE_CDMA);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCDeleteAllService

DESCRIPTION
  Deletes One BC Service

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCDeleteService
(
  CWMSAPP *pMe
)
{
  wms_bc_mm_srv_id_type bcMmSrvId;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  bcMmSrvId.bc_srv_id = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].srv_id.bc_srv_id;
  (void)IWMS_BcMmDeleteServices(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe,
                                WMS_MESSAGE_MODE_CDMA, 1, &bcMmSrvId);

  (void)IWMS_BcMmGetTable(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,NULL,WMS_MESSAGE_MODE_CDMA);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCSetPref

DESCRIPTION
  Sets the BC Preferences

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCSetPref
(
  CWMSAPP *pMe,
  wms_bc_pref_e_type bcPref
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_BcMmSetPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, WMS_MESSAGE_MODE_CDMA, bcPref);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCSetPriority

DESCRIPTION
  Sets a prioirty for BC Services

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCSetPriority
(
  CWMSAPP *pMe,
  wms_priority_e_type priority
)
{
  wms_bc_mm_service_info_s_type bcMmSrvInfo;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  /* Update the Service Information */
  bcMmSrvInfo.alert = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].alert;
  (void)MEMCPY(bcMmSrvInfo.label, pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].label, WMS_BC_MM_SRV_LABEL_SIZE);
  bcMmSrvInfo.label_encoding = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].label_encoding;
  bcMmSrvInfo.max_messages = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].max_messages;
  bcMmSrvInfo.selected = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].selected;
  bcMmSrvInfo.srv_id.bc_srv_id = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].srv_id.bc_srv_id;

  /* Update Priority */
  bcMmSrvInfo.priority = priority;

  (void)IWMS_BcMmChangeServiceInfo(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                               (void*)pMe,WMS_MESSAGE_MODE_CDMA, &bcMmSrvInfo);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCChnageLabel

DESCRIPTION
  Changes the label for the BC Services

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCChangeLabel
(
  CWMSAPP *pMe
)
{
  wms_bc_mm_service_info_s_type bcMmSrvInfo;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  /* Update the Service Information */
  bcMmSrvInfo.alert = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].alert;
  (void)MEMCPY(bcMmSrvInfo.label, pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].label, WMS_BC_MM_SRV_LABEL_SIZE);
  bcMmSrvInfo.label_encoding = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].label_encoding;
  bcMmSrvInfo.max_messages = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].max_messages;
  bcMmSrvInfo.priority = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].priority;
  bcMmSrvInfo.selected = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].selected;
  bcMmSrvInfo.srv_id.bc_srv_id = pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].srv_id.bc_srv_id;

  (void)IWMS_BcMmChangeServiceInfo(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                            (void*)pMe, WMS_MESSAGE_MODE_CDMA, &bcMmSrvInfo);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCSetPriorityForAll

DESCRIPTION
  Sets the Priority for all the BC Services

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCSetPriorityForAll
(
  CWMSAPP *pMe,
  wms_priority_e_type priority
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_BcMmSetPriorityAllServices(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                  (void*)pMe, WMS_MESSAGE_MODE_CDMA, priority);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCAddService

DESCRIPTION
  Adds a new BC Service

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCAddService
(
  CWMSAPP *pMe,
  wms_bc_mm_service_info_s_type *svc_info
)
{
  wms_bc_mm_service_info_s_type bcMmSrvInfo;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  /* Update the Service Information */
  bcMmSrvInfo.alert = svc_info->alert;
  (void)MEMCPY(bcMmSrvInfo.label, svc_info->label, WMS_BC_MM_SRV_LABEL_SIZE);
  bcMmSrvInfo.label_encoding = svc_info->label_encoding;
  bcMmSrvInfo.max_messages = svc_info->max_messages;
  bcMmSrvInfo.priority = svc_info->priority;
  bcMmSrvInfo.selected = svc_info->selected;
  bcMmSrvInfo.srv_id.bc_srv_id = svc_info->srv_id.bc_srv_id;

  (void)IWMS_BcMmAddServices(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                        (void*)pMe, WMS_MESSAGE_MODE_CDMA, 1, &bcMmSrvInfo);
}

/*===========================================================================
FUNCTION WMSAPPMN_BCSelectService

DESCRIPTION
  Selects a BC Service

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_BCSelectService
(
  CWMSAPP *pMe,
  wms_bc_mm_service_info_s_type *svc_info
)
{
  wms_bc_mm_srv_id_type bcMmSrvId;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  bcMmSrvId.bc_srv_id = svc_info->srv_id.bc_srv_id;
  (void)IWMS_BcMmSelectService(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe,
                               WMS_MESSAGE_MODE_CDMA, &bcMmSrvId, svc_info->selected);
}

#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
FUNCTION WMSAPPMN_FILL_DEFAULT_VALUES

DESCRIPTION
  Extracts all the information from the default template and stores it into
  the message. If a Template option is not enabled, then the hard coded default
  values are stored in the message

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_FillDefaultValues
(
CWMSAPP *pMe
)
{
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;
    pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
    if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA)
    {
      pMe->m_pClient_ts_mo->u.cdma.mask = pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask;
    }
    else
    {
      pMe->m_pClient_ts_mo->u.cdma.mask = 0;
    }

    if(pMe->m_msg_type == WMSAPP_MSG_TYPE_REPLY)
    {
      /* for replies, message id remains the same as received message */
      pMe->m_pClient_ts_mo->u.cdma.message_id.type = WMS_BD_TYPE_USER_ACK;
    }
    else
    {
      /* for forwards and new messages */
      pMe->m_pClient_ts_mo->u.cdma.message_id.type   = WMS_BD_TYPE_SUBMIT;
      pMe->m_pClient_ts_mo->u.cdma.message_id.id_number = 0xFFFF;
    }
    pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_MSG_ID;
    pMe->m_pClient_ts_mo->u.cdma.num_messages      = 1;

    if(pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_91 &&
      pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CPT_95 &&
      pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_95 &&
      pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_95 &&
      pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_WEMT)
    {
      pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
    }
    else
    {
      pMe->m_pClient_msg_mo->u.cdma_message.teleservice =
      pMe->m_pMO_template_msg->u.cdma_template.teleservice;
    }

    if(pMe->m_pMO_template_msg->u.cdma_template.mask &
      WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION)
    {
      pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = TRUE;
    }
    else
    {
      pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
    }

    if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS)
    {
      pMe->m_pClient_ts_mo->u.cdma.user_data.encoding =
        pMe->m_pMO_template_msg->u.cdma_template.dcs;
    }
    else
    {
      pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_ASCII;
    }

    if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA)
    {
      /* Encoding for user data */
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
      {
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding    = pMe->m_pMO_template_msg->u.cdma_template.dcs;
      }
      else if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA)
      {
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding =
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding    = WMS_ENCODING_ASCII;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY)
      {
        pMe->m_pClient_ts_mo->u.cdma.priority = pMe->m_pMO_template_msg->u.cdma_template.client_bd.priority;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.priority = WMS_PRIORITY_NORMAL;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIVACY)
      {
        pMe->m_pClient_ts_mo->u.cdma.privacy = pMe->m_pMO_template_msg->u.cdma_template.client_bd.privacy;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.privacy = WMS_PRIVACY_NORMAL;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_REPLY_OPTION)
      {
        pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested =
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested;
        pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested =
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested = FALSE;
        pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested = FALSE;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_LANGUAGE)
      {
        pMe->m_pClient_ts_mo->u.cdma.language = pMe->m_pMO_template_msg->u.cdma_template.client_bd.language;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.language = WMS_LANGUAGE_UNSPECIFIED;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_CALLBACK)
      {
        memcpy((void*)&pMe->m_pClient_ts_mo->u.cdma.callback,
               (void*)&pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback,
               sizeof(wms_address_s_type));
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.callback.digit_mode   = WMS_DIGIT_MODE_4_BIT;
        pMe->m_pClient_ts_mo->u.cdma.callback.number_plan  = WMS_NUMBER_PLAN_TELEPHONY;
        pMe->m_pClient_ts_mo->u.cdma.callback.number_type  = WMS_NUMBER_UNKNOWN;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_VALID_REL)
      {
        pMe->m_pClient_ts_mo->u.cdma.validity_relative =
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative;
      }
      else
      {
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_relative, 0, sizeof(wms_timestamp_s_type));
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_VALID_ABS)
      {
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute =
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute;
      }
      else
      {
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_absolute, 0, sizeof(wms_timestamp_s_type));
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_REL)
      {
        pMe->m_pClient_ts_mo->u.cdma.deferred_relative =
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative;
      }
      else
      {
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_relative, 0, sizeof(wms_timestamp_s_type));
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_ABS)
      {
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute =
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute;
      }
      else
      {
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_absolute, 0, sizeof(wms_timestamp_s_type));
      }


      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DISPLAY_MODE)
      {
        pMe->m_pClient_ts_mo->u.cdma.display_mode =
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.display_mode;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.display_mode = WMS_DISPLAY_MODE_DEFAULT;
      }

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_ALERT)
      {
        pMe->m_pClient_ts_mo->u.cdma.alert_mode =
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.alert_mode;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.alert_mode = WMS_ALERT_MODE_DEFAULT;
      }
    }
    else
    {
      /* Some default values if bearer data mask is not set, this may be useful
      ** if we edit a message to prevent wrong data being packed.
      */
      pMe->m_pClient_ts_mo->u.cdma.priority = WMS_PRIORITY_NORMAL;
      pMe->m_pClient_ts_mo->u.cdma.privacy = WMS_PRIVACY_NORMAL;
      pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested = FALSE;
      pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested = FALSE;
      pMe->m_pClient_ts_mo->u.cdma.language = WMS_LANGUAGE_UNSPECIFIED;
      pMe->m_pClient_ts_mo->u.cdma.callback.digit_mode   = WMS_DIGIT_MODE_4_BIT;
      pMe->m_pClient_ts_mo->u.cdma.callback.number_plan  = WMS_NUMBER_PLAN_TELEPHONY;
      pMe->m_pClient_ts_mo->u.cdma.callback.number_type  = WMS_NUMBER_UNKNOWN;
      pMe->m_pClient_ts_mo->u.cdma.validity_relative.day = 0;
      pMe->m_pClient_ts_mo->u.cdma.validity_relative.hour= 0;
      pMe->m_pClient_ts_mo->u.cdma.validity_relative.minute = 0;
      pMe->m_pClient_ts_mo->u.cdma.display_mode = WMS_DISPLAY_MODE_DEFAULT;
    }

#endif /*FEATURE_CDSMS*/
  }
  else if ( pMe->m_modeControl == WMSAPP_MODE_GW )
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

}

/*===========================================================================
FUNCTION WMSAPPMN_GETDLGMENUCTLID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetDlgMenuCtlID
(
 CWMSAPP *pMe,
 IDialog* pIActiveDialog
)
{
  uint16 wDlgID;
  int i;
  int nCount;

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  if(pMe->m_bTestMode)
  {
    nCount = ARR_SIZE(SMSLookupMenuCtlID);
    for(i = 0; i< nCount; i++)
    {
      if(SMSLookupMenuCtlID[i].wDialogID == wDlgID)
      {
        return SMSLookupMenuCtlID[i].wCtrlID;
      }
    }
  }
  return 0; /* error */
} /* WMSAPPMN_GetDlgMenuCtlID */

/*===========================================================================
FUNCTION WMSAPPMN_DisplayAddress

DESCRIPTION
  Displays the destination address retreived from IDD_DEST_ADDR Dialog

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
int  WMSAPPMN_DisplayAddress
(
 CWMSAPP * pMe
 //uint32 dw
)
{
  IDialog * pIDialog;           // Dialog to be opened
  ITextCtl * pIDestAddr  ;      // Pointer to text box for data entry
  AECHAR   destAddr[WMS_SMS_UDL_MAX_7_BIT];
  static uint8 temp_buf[WMS_MAX_LEN];
  uint16 wCtlID;

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(!pIDialog)
  {
    return FALSE;
  }

  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIDestAddr   = (ITextCtl * )IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  if(pIDestAddr == NULL)
  {
    return FALSE;
  }

  /* Initialize contents of destAddr and global buffer */
  (void)MEMSET(destAddr, 0, sizeof(destAddr));
  (void)MEMSET(temp_buf,0,sizeof(temp_buf));

  (void)MEMSET(pMe->m_buf, 0, sizeof(pMe->m_buf));

/*----------------------------------------------------------------------------*/
/* TEMPLATE processing                                                        */
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {

      if(pMe->m_currPref == IDL_C_TMPLT_DEST_ADDR)
      {
        if(pMe->m_pMO_template_msg->u.cdma_template.mask &
           WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
        {
          if(pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
          {
              (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits,
                              (uint8*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                              (uint8*)pMe->m_buf);

            if(WMSAPPU_IsNonStdEmail(FALSE, (uint8*)pMe->m_buf,
                                     (uint8)pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits))
            {
              if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_NON_STD_EMAIL)
              {
                /* unpack the user data to get the email address */
                (void)IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data,
                                    pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                                    (uint8 *)temp_buf);

                (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
                (void)WMSAPPU_GetNonStdEmail(temp_buf,
                                       (uint8*)pMe->m_emailAddr.address);

                (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                          destAddr,
                          sizeof(destAddr));
              }
            }
            else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM)
            {
              (void)STRTOWSTR((char *)pMe->m_buf, destAddr,
                        sizeof(destAddr));
            }
          }
          else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_EMAIL)
          {
            (void)STRTOWSTR((char *)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                      destAddr,
                      sizeof(AECHAR)*(pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits+1));
          }
          else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM &&
                  pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_8_BIT &&
                  pMe->m_pMO_template_msg->u.cdma_template.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
                  pMe->m_pMO_template_msg->u.cdma_template.address.number_type == WMS_NUMBER_INTERNATIONAL)
          {
            destAddr[0] = '+';

            /* copy the buffer to the address pointer */
            (void)STRTOWSTR((char *)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                            destAddr+1,
                            sizeof(AECHAR) * (pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits+1));
          }
        }
      }
      else if(pMe->m_currPref == IDL_C_TMPLT_CALLBACK)
      {
        if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
           (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_CALLBACK))
        {
          if((pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digit_mode ==
            WMS_DIGIT_MODE_4_BIT) && (pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM))
          {
              (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits,
                              (uint8 *)pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digits,
                              (uint8 *)pMe->m_buf);
          }
          else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_EMAIL)
          {
            (void)MEMCPY(pMe->m_buf, pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digits,
                   pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits);
          }
          else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM &&
                  pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digit_mode == WMS_DIGIT_MODE_8_BIT &&
                  pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_type == WMS_NUMBER_INTERNATIONAL)
          {
            pMe->m_buf[0] = '+';
            (void)MEMCPY(pMe->m_buf+1, pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digits,
                         pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits);
            /* increment the number of digits. */
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits++;

          }
          (void)STRTOWSTR((char *)pMe->m_buf, destAddr, sizeof(destAddr));
        }
      }

      (void)ITEXTCTL_SetText(pIDestAddr, destAddr, sizeof(destAddr));
      ITEXTCTL_SetCursorPos(pIDestAddr, sizeof(destAddr));
      /* Early return to not process for other box types */

      return TRUE;
    }
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

  }

/*----------------------------------------------------------------------------*/

  /*If the message is not a new message then retrieve message */
  if (pMe->m_msg_type != WMSAPP_MSG_TYPE_NEW && pMe->m_msg_type != WMSAPP_MSG_TYPE_CANNED)
  {
    if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
      pMe->m_pClient_msg->u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;

      /* Initialize other to 0 */
      (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

      if (IWMS_TsDecode( pMe->m_pwms, &pMe->m_pClient_msg->u.cdma_message.raw_ts,
                         pMe->m_pClient_ts_mo ) != (uint8)WMS_OK_S )
      {
        MSG_ERROR("Decoding failed!",0,0,0);
        return FALSE;
      }
#endif /*FEATURE_CDSMS*/
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
    }
  }

#ifdef FEATURE_CDSMS
/*----------------------------------------------------------------------------*/
/* Callback processing                                                        */

  /* Check if this is for modifying the callback */
  if(pMe->m_currPref == IDL_OUTBOX_CALLBACK)
  {
    if(pMe->m_msg_type != WMSAPP_MSG_TYPE_NEW && pMe->m_msg_type != WMSAPP_MSG_TYPE_CANNED)
    {
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_CALLBACK)
      {
        if(pMe->m_pClient_ts_mo->u.cdma.callback.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
            (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits,
                            (uint8 *)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                            (uint8 *)pMe->m_buf);

          (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
        }
        else
        {
          if (pMe->m_pClient_ts_mo->u.cdma.callback.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
              pMe->m_pClient_ts_mo->u.cdma.callback.number_type == WMS_NUMBER_INTERNATIONAL)
          {
            destAddr[0] = '+';
            (void)STRTOWSTR((char*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                    (destAddr+1),
                    sizeof(AECHAR)*(pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
          }
          else
          {
            (void)STRTOWSTR((char*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                    destAddr,
                    sizeof(AECHAR)*(pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
          }
        }
      }
    }
    else  /* This is for a new message, check the template */
    {
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_CALLBACK))
      {
        if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits,
                            (uint8 *)pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digits,
                            (uint8 *)pMe->m_buf);

          (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
        }
        else
        {
          if (pMe->m_pClient_ts_mo->u.cdma.callback.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
              pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_type == WMS_NUMBER_INTERNATIONAL)
          {
            destAddr[0] = '+';
            (void)STRTOWSTR((char*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                      (destAddr+1),
                      sizeof(AECHAR)*(pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
          }
          else
          {
            (void)STRTOWSTR((char*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                    destAddr,
                    sizeof(AECHAR)*(pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
          }
        }
      }
    }
    (void)ITEXTCTL_SetText(pIDestAddr, destAddr, sizeof(destAddr));
    ITEXTCTL_SetCursorPos(pIDestAddr, sizeof(destAddr));
    return TRUE;
  }
#endif /* FEATURE_CDSMS */

/*----------------------------------------------------------------------------*/
/* Destination address processing                                             */

  switch (pMe->m_msg_type)
  {
    case WMSAPP_MSG_TYPE_REPLY:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        /* Check for a callback number, if it is not present then use
        ** the address of the MT message */
        if (pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_CALLBACK)
        {
          if(pMe->m_pClient_ts_mo->u.cdma.callback.digit_mode == WMS_DIGIT_MODE_4_BIT)
          {
            (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits,
                                (uint8*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                                (uint8*)pMe->m_buf);

            (void)STRTOWSTR((char*)pMe->m_buf, destAddr,
                            sizeof(AECHAR) * (pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
          }
          else /* 8-bit */
          {
            if(pMe->m_pClient_ts_mo->u.cdma.callback.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
               pMe->m_pClient_ts_mo->u.cdma.callback.number_type == WMS_NUMBER_INTERNATIONAL)
            {
              destAddr[0] = '+';
              pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits++;
              (void)STRTOWSTR((char*)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                        (destAddr+1),
                        sizeof(AECHAR) * (pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
            }
            else
            {
              /* Copy the e-mail callback address */
              (void)STRTOWSTR((char *)pMe->m_pClient_ts_mo->u.cdma.callback.digits,
                        destAddr,
                        sizeof(AECHAR)*(pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits+1));
            }
          }
        }
        else /* Put received dest addr as the new dest addr */
        {
          if(pMe->m_pClient_msg->u.cdma_message.address.digit_mode ==
             WMS_DIGIT_MODE_4_BIT)
          {
              (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pClient_msg->u.cdma_message.address.number_of_digits,
                              (uint8*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                              (uint8*)pMe->m_buf);

            if(WMSAPPU_IsNonStdEmail(FALSE,
                                (uint8*)pMe->m_buf,
                                (uint8)pMe->m_pClient_msg->u.cdma_message.address.number_of_digits))
            {
              /* unpack the user data to get the email address */
              (void)IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pClient_ts_mo->u.cdma.user_data,
                                  pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits+1,
                                 (uint8 *)temp_buf);

              (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
              (void)WMSAPPU_GetNonStdEmail(temp_buf,
                                    (uint8*)pMe->m_emailAddr.address);

              (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                        destAddr,
                        sizeof(destAddr));
            }
            else
            {
              (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
            }
          }
          else
          {
            if(pMe->m_pClient_msg->u.cdma_message.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
               pMe->m_pClient_msg->u.cdma_message.address.number_type == WMS_NUMBER_INTERNATIONAL)
            {
              destAddr[0] = '+';
              pMe->m_pClient_msg->u.cdma_message.address.number_of_digits++;
              (void)STRTOWSTR((char*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                        (destAddr+1),
                        sizeof(AECHAR)*(pMe->m_pClient_msg->u.cdma_message.address.number_of_digits+1));
            }
            else
            {
              (void)STRTOWSTR((char*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                        destAddr,
                        sizeof(AECHAR)*(pMe->m_pClient_msg->u.cdma_message.address.number_of_digits+1));
            }
          }
        }
#endif /*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
      break;

    case WMSAPP_MSG_TYPE_FWD:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if (pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
        {
          if(pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
          {
              (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits,
                              (uint8*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                              (uint8*)pMe->m_buf);

            if(WMSAPPU_IsNonStdEmail(FALSE, pMe->m_buf,
                                     pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits))
            {
                /* unpack the user data to get the email address */
                (void)IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data,
                                    pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                                    (uint8 *)temp_buf);

                (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
                (void)WMSAPPU_GetNonStdEmail(temp_buf,
                                      (uint8*)pMe->m_emailAddr.address);

                (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                          destAddr,
                          sizeof(destAddr));

            }
            else
            {
              (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
            }
          }
          else /* 8-bit */
          {
            if(pMe->m_pMO_template_msg->u.cdma_template.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
               pMe->m_pMO_template_msg->u.cdma_template.address.number_type == WMS_NUMBER_INTERNATIONAL)
            {
              destAddr[0] = '+';
              pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits++;
              (void)STRTOWSTR((char*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                        (destAddr+1),
                        sizeof(AECHAR)*(pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits+1));
            }
            else
            {
              (void)STRTOWSTR((char*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                      destAddr,
                      sizeof(AECHAR)*(pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits+1));
            }
          }
        }
#endif/*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      /* just forwarding, thus no mem store */
      pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
      break;

  case WMSAPP_MSG_TYPE_NEW:
  case WMSAPP_MSG_TYPE_CANNED:
    pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
      if (pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
      {
        if(pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits,
                            (uint8*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                            (uint8*)pMe->m_buf);
          if(WMSAPPU_IsNonStdEmail(FALSE, (uint8*)pMe->m_buf,
                                   (uint8)pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits))
          {
            if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_NON_STD_EMAIL)
            {
              /* unpack the user data to get the email address */
              (void)IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data,
                                  pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                                  (uint8 *)temp_buf);

              (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
              (void)WMSAPPU_GetNonStdEmail(temp_buf,
                                     (uint8*)pMe->m_emailAddr.address);

              (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                        destAddr,
                        sizeof(destAddr));
            }
          }
          else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM)
          {
            (void)STRTOWSTR((char *)pMe->m_buf, destAddr, sizeof(destAddr));
          }
        }
        else if((pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode == WMS_DIGIT_MODE_8_BIT) &&
                (pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_EMAIL))
        {
          (void)STRTOWSTR((char *)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                    destAddr,
                    sizeof(AECHAR) * (pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits + 1));
        }
        else if(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM &&
                pMe->m_pMO_template_msg->u.cdma_template.address.number_type == WMS_NUMBER_INTERNATIONAL &&
                pMe->m_pMO_template_msg->u.cdma_template.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK)
        {
          destAddr[0] = '+';
          (void)STRTOWSTR((char*)pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                          destAddr+1,
                          sizeof(AECHAR) * (pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits + 1));
        }
      }
#endif /*FEATURE_CDSMS*/
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    }
    break;


    case WMSAPP_MSG_TYPE_EDIT:
      /*Edit . This message has to be replaced, do not change memory store*/
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_pClient_msg->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, pMe->m_pClient_msg->u.cdma_message.address.number_of_digits,
                              (uint8*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                              (uint8*)pMe->m_buf);
          if(WMSAPPU_IsNonStdEmail(FALSE, (uint8*)pMe->m_buf,
                                   (uint8)pMe->m_pClient_msg->u.cdma_message.address.number_of_digits))
          {
            /* unpack the user data to get the email address */
            (void)IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pClient_ts_mo->u.cdma.user_data,
                                 pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits+1,
                                 (uint8 *)temp_buf);

            (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
            (void)WMSAPPU_GetNonStdEmail(temp_buf,
                                   (uint8*)pMe->m_emailAddr.address);

            (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                      destAddr,
                      sizeof(destAddr));

          }
        }
        else /* 8-bit */
        {
          if (pMe->m_pClient_msg->u.cdma_message.address.number_mode ==
              WMS_NUMBER_MODE_NONE_DATA_NETWORK                      &&
              pMe->m_pClient_msg->u.cdma_message.address.number_type ==
              WMS_NUMBER_INTERNATIONAL)
          {
            destAddr[0] = '+';
            pMe->m_pClient_msg->u.cdma_message.address.number_of_digits++;
            (void)STRTOWSTR((char*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                      destAddr+1,
                      sizeof(AECHAR)*(pMe->m_pClient_msg->u.cdma_message.address.number_of_digits+1));
          }
          else
          {
            (void)STRTOWSTR((char*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                      destAddr,
                      sizeof(AECHAR)*(pMe->m_pClient_msg->u.cdma_message.address.number_of_digits+1));
          }
        }
#endif /*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    default:
      /* Should not come here */
      MSG_HIGH("Unkown message type!!!",0,0,0);
      break;
  }

  if ((pMe->m_msg_type == WMSAPP_MSG_TYPE_NEW)
      || (pMe->m_msg_type == WMSAPP_MSG_TYPE_FWD)
      || (pMe->m_msg_type == WMSAPP_MSG_TYPE_EDIT)
      || (pMe->m_msg_type == WMSAPP_MSG_TYPE_CANNED)
      || (pMe->m_msg_type == WMSAPP_MSG_TYPE_REPLY)
     )
  {
    pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
  }

  (void)ITEXTCTL_SetText (pIDestAddr, destAddr, sizeof(destAddr));
  ITEXTCTL_SetCursorPos(pIDestAddr, sizeof(destAddr));
  (void)ITEXTCTL_Redraw(pIDestAddr);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_SetDestAddrText

DESCRIPTION
  Sets the Destination Address Text to whatever is provided in ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
==========================================================================*/
boolean WMSAPPMN_SetDestAddrText
(
  CWMSAPP *pMe,
  char *string
)
{
  IDialog * pIDialog;
  ITextCtl * pIDestAddr;
  AECHAR   destAddr[WMS_SMS_UDL_MAX_7_BIT];
  uint16 wCtlID;

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(!pIDialog)
  {
    return FALSE;
  }

  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  pIDestAddr   = (ITextCtl * )IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  if(pIDestAddr == NULL)
  {
    return FALSE;
  }

  (void)MEMSET(destAddr, 0, sizeof(destAddr));
  (void)STRTOWSTR(string, destAddr, sizeof(destAddr));

  (void)ITEXTCTL_SetText (pIDestAddr, destAddr, sizeof(destAddr));
  ITEXTCTL_SetCursorPos(pIDestAddr, sizeof(destAddr));
  (void)ITEXTCTL_Redraw(pIDestAddr);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_StoreText

DESCRIPTION
  While creating the information, extracts all of the information from the
  template before giving the user the option to modify them

DEPENDENCIES
  None

SIDE EFFECTS
  None
==========================================================================*/
boolean WMSAPPMN_StoreText
(
  CWMSAPP *pMe
)
{
  /* If editting, need to remember the index */
  if (pMe->m_msg_type != WMSAPP_MSG_TYPE_EDIT)
  {
      pMe->m_msgInfo.smsIndex  = 0;
  }

  switch (pMe->m_msg_type)
  {
    case WMSAPP_MSG_TYPE_REPLY:
      WMSAPPMN_FillDefaultValues(pMe);
      break;
    case WMSAPP_MSG_TYPE_NEW:
    case WMSAPP_MSG_TYPE_FWD:
      WMSAPPMN_FillDefaultValues(pMe);
      break;
    default:
      break;
  }

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;

    pMe->m_pClient_ts_mo->u.cdma.user_data.data_len=
      pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;

#ifdef FEATURE_SMS_UDH
    /* Force the encoding if we have UDH */
    if(pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers > 0)
    {
      // pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
      pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = TRUE;
    }
#endif /* FEATURE_SMS_UDH */

    if(pMe->m_emailAddr.isNonStdEmail)
    {
      if((pMe->m_msg_type == WMSAPP_MSG_TYPE_NEW) ||
         !(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_USER_DATA))
      {
        /* Assign an encoding initially as OCTET */
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
      }
      WMSAPPU_PutNonStdEmail(pMe->m_emailAddr.address, pMe,
                             &pMe->m_pClient_ts_mo->u.cdma.user_data);
    }

    if (pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_ASCII ||
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_IA5)
    {
      uint8 buf[WMS_MAX_LEN];
      (void)MEMSET(buf, 0, WMS_MAX_LEN);
      (void)MEMCPY(buf, pMe->m_pClient_ts_mo->u.cdma.user_data.data,
             pMe->m_pClient_ts_mo->u.cdma.user_data.data_len);
      (void)MEMSET(pMe->m_pClient_ts_mo->u.cdma.user_data.data, 0, WMS_CDMA_USER_DATA_MAX);

      pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits =
          IWMS_TsPackAscii( pMe->m_pwms, (char *)buf,
                            (uint8 *)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                            (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data_len,
                            (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits);
      }
    else
    {

      /* make sure padding bits equals 0 for these encoding schemes */
      pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits = 0;

      if(pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE)
      {
        WMSAPPU_ConvertToUnicode(&pMe->m_pClient_ts_mo->u.cdma);
      }
    }


    if(pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits > 0 || pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers != 0)
    {
      pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
    }
    else
    {
      pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_USER_DATA;
    }

    /* Initialize other to 0 */
    (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

    if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts ) != SUCCESS)
    {
      MSG_ERROR("TS encoding error",0,0,0);

      (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "Encoding Failure!", SELECTIONVERIFICATIONTIME);

      return FALSE;
    }

    /* Prepare client msg*/
    pMe->m_pClient_msg_mo->msg_hdr.mem_store    = pMe->m_msgInfo.memStore;
    pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;

    if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
    {
      pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
    }
    else
    {
      pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MT_NOT_READ;
    }

    if(pMe->m_msg_type == WMSAPP_MSG_TYPE_EDIT)
    {
      if(pMe->m_pClient_msg_mo->msg_hdr.mem_store != (wms_memory_store_e_type)0)
      {
        if (WMSAPPMN_StoreMessage(pMe) != SUCCESS)
        {
          return FALSE;
        }
      }
    }
#endif /* FEATURE_CDSMS */
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }
  return TRUE;
}

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION WMSAPPMN_ConvertCDMADcs

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ConvertCDMADcs
(
  CWMSAPP *pMe,
  wms_user_data_encoding_e_type dcs
)
{
  uint8 *tempData = NULL;

  /* first check if a new selection was chosen, if not then no more processing
  ** is needed at this time.
  */
  if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
  {
    if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS) &&
       (dcs == pMe->m_pMO_template_msg->u.cdma_template.dcs))
    {
      return FALSE;
    }
  }
  else
  {
    if((pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_USER_DATA) &&
       (dcs == pMe->m_pClient_ts_mo->u.cdma.user_data.encoding))
    {
      return FALSE;
    }
  }

  if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
  {
    /* First check if this has a non std email address, if it does then the
    ** dcs can not be changed (must remain ASCII).
    */
    if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR) &&
       (WMSAPPU_IsNonStdEmail(TRUE, pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                              pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits)))
    {
      return FALSE;
    }

    /* Convert the user data if the message contains text */
    if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
       (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
    {
      /* check if we have to pack the data according to the new dcs value */
      if ( dcs == WMS_ENCODING_ASCII || dcs == WMS_ENCODING_IA5)
      {
        if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding != WMS_ENCODING_ASCII &&
           pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding != WMS_ENCODING_IA5)
        {
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_UNICODE)
          {
            WMSAPPU_ConvertFromUnicode(&pMe->m_pMO_template_msg->u.cdma_template.client_bd);
          }
          tempData = MALLOC(WMS_MAX_LEN);
          if (tempData == NULL) {
            return FALSE;
          }

          (void)MEMSET(tempData, 0, sizeof(tempData));

          (void)MEMCPY(tempData, pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
                 pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len);

          /* pack the data */
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits =
            IWMS_TsPackAscii( pMe->m_pwms, (char *)tempData, (uint8 *)pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
                              (uint8 *)&pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len,
                              (uint8 *)&pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.padding_bits);
          WMSAPPU_FREE(tempData);
        }
      }
      else if (pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_ASCII ||
               pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_IA5)
      {
        tempData = MALLOC(WMS_MAX_LEN);
        if (tempData == NULL) {
          return FALSE;
        }

        (void)MEMSET(tempData, 0, sizeof(tempData));

        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len =
          IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data,
                            pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                            (uint8 *)tempData);

        (void)MEMCPY(pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
               tempData,
               pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len);

        if(dcs == WMS_ENCODING_UNICODE)
        {
          WMSAPPU_ConvertToUnicode(&pMe->m_pMO_template_msg->u.cdma_template.client_bd);
        }
        /* reset the padding bits to 0 for these encoding schemes */
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.padding_bits = 0;
        WMSAPPU_FREE(tempData);
      }
      else if(dcs == WMS_ENCODING_UNICODE &&
              ((pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_OCTET)
              ||(pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT)))
      {
        WMSAPPU_ConvertToUnicode(&pMe->m_pMO_template_msg->u.cdma_template.client_bd);
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.padding_bits = 0;
      }

      /* Synchronize the user data encoding with the chosen DCS value */
      pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding = dcs;
    }
  }
  else
  {
    /* First check if this has a non std email address, if it does then the
    ** dcs can not be changed (must remain ASCII).
    */
    if(WMSAPPU_IsNonStdEmail(TRUE, pMe->m_pClient_msg_mo->u.cdma_message.address.digits,
                             pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits))
    {
      return FALSE;
    }

    /* Convert the user data if the message contains text */
    if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_USER_DATA)
    {
      /* check if we have to pack the data according to the new dcs value */
      if ( dcs == WMS_ENCODING_ASCII || dcs == WMS_ENCODING_IA5)
      {
        if(pMe->m_pClient_ts_mo->u.cdma.user_data.encoding != WMS_ENCODING_ASCII &&
           pMe->m_pClient_ts_mo->u.cdma.user_data.encoding != WMS_ENCODING_IA5)
        {
          if(pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE)
          {
            WMSAPPU_ConvertFromUnicode(&pMe->m_pClient_ts_mo->u.cdma);
          }
          tempData = MALLOC(WMS_MAX_LEN);
          if (tempData == NULL) {
            return FALSE;
          }

          (void)MEMSET(tempData, 0, sizeof(tempData));

          (void)MEMCPY(tempData, pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                 pMe->m_pClient_ts_mo->u.cdma.user_data.data_len);

          /* pack the data */
          pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits =
            IWMS_TsPackAscii( pMe->m_pwms, (char *)tempData, (uint8 *)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                              (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data_len,
                              (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits);
          WMSAPPU_FREE(tempData);
        }
      }
      else if (pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_ASCII ||
               pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_IA5)
      {
        tempData = MALLOC(WMS_MAX_LEN);
        if (tempData == NULL) {
          return FALSE;
        }

        (void)MEMSET(tempData, 0, sizeof(tempData));

        pMe->m_pClient_ts_mo->u.cdma.user_data.data_len =
          IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pClient_ts_mo->u.cdma.user_data,
                            pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits+1,
                            (uint8 *)tempData);

        (void)MEMCPY(pMe->m_pClient_ts_mo->u.cdma.user_data.data,
               tempData,
               pMe->m_pClient_ts_mo->u.cdma.user_data.data_len);

        if(dcs == WMS_ENCODING_UNICODE)
        {
          WMSAPPU_ConvertToUnicode(&pMe->m_pClient_ts_mo->u.cdma);
        }
        /* reset the padding bits to 0 for these encoding schemes */
        pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits = 0;
        WMSAPPU_FREE(tempData);
      }
      else if(dcs == WMS_ENCODING_UNICODE &&
              ((pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_OCTET)
              ||(pMe->m_pClient_ts_mo->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT)))
      {
        WMSAPPU_ConvertToUnicode(&pMe->m_pClient_ts_mo->u.cdma);
        pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits = 0;
      }
    }
    else
    {
      /* No user data so no need to change the encoding */
      return FALSE;
    }
  }

  return TRUE;
}
#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION WMSAPPMN_STOPIMAGES

DESCRIPTION
  The function is used to stop all images (especially Animations)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static void WMSAPPMN_StopImages(CWMSAPP *pMe)
{
#ifdef FEATURE_SMS_UDH
  int i;
  // To Stop all Animations before showing up next Dialog
  for(i=0;i< pMe->m_numofImages ;i++)
  {
    IIMAGE_Stop(pMe->m_pCurImage[i]);
    (void)IIMAGE_Release(pMe->m_pCurImage[i]);
    pMe->m_pCurImage[i] = NULL;
  }
  if (pMe->m_pSelImage != NULL)
  {
    IIMAGE_Stop(pMe->m_pSelImage);
    (void)IIMAGE_Release(pMe->m_pSelImage);
    pMe->m_pSelImage = NULL;
  }
  pMe->m_numofImages = 0;
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);
#endif /* FEATURE_SMS_UDH */
} /* WMSAPPMN_StopImages */


/*===========================================================================
FUNCTION WMSAPPMN_GETNEXTSMSSELITEMDLGID

DESCRIPTION
  The function is used when a menu control selection is being made.
  It returns the next dialog ID that corresponds to the item selected

DEPENDENCIES
  Note: This function should ONLY be called from a menu dialog which
    will create another dialog

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetNextSelItemDlgID(CWMSAPP *pMe, uint16 wItemID)
{
  int i;
  int nCount;

  if(wItemID == 0) return 0;

  if(pMe->m_bTestMode)
  {
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
      nCount = ARR_SIZE(LookupNextCDMASelItemDlgID);
      for(i = 0; i< nCount; i++)
      {
        if(LookupNextCDMASelItemDlgID[i].wItemID == wItemID)
        {
          return LookupNextCDMASelItemDlgID[i].wDialogID;
        }
      }
    }
    else
    {
      nCount = ARR_SIZE(LookupNextGWSelItemDlgID);
      for(i = 0; i< nCount; i++)
      {
        if(LookupNextGWSelItemDlgID[i].wItemID == wItemID)
        {
          return LookupNextGWSelItemDlgID[i].wDialogID;
        }
      }
    }
  }
  return 0; /* error */
} /* GetNextSelectedItemDlgID */




/*===========================================================================
FUNCTION WMSAPPMN_GETDLGSKCTLID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetDlgSKCtlID(CWMSAPP *pMe, IDialog* pIActiveDialog)
{
  uint16 wDlgID;
  int i;
  int nCount;

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  nCount = ARR_SIZE(SMSLookupSKCtlID);
  for(i = 0; i< nCount; i++)
  {
    if(SMSLookupSKCtlID[i].wDialogID == wDlgID)
    {
      return SMSLookupSKCtlID[i].wSKCtrlID;
    }
  }
  return 0; /* error */
} /* WMSAPPMN_GetDlgSKCtlID */

/*===========================================================================
FUNCTION WMSAPPMN_GetWMSCMDResID

DESCRIPTION
  This function matches the Cmd with a corresponding res ID

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetWMSCMDResID(CWMSAPP *pMe, wms_cmd_id_e_type cmd)
{
  int i;
  int nCount;

  nCount = ARR_SIZE(WMSAPPLookupCmd);
  for(i = 0; i< nCount; i++)
  {
    if(WMSAPPLookupCmd[i].cmd == cmd)
    {
      return WMSAPPLookupCmd[i].wResID;
    }
  }
  return 0; /* error */
} /* WMSAPPMN_GetWMSCMDResID */

/*===========================================================================
FUNCTION WMSAPPMN_GetWMSCMDResID

DESCRIPTION
  This function matches the Cmd with a corresponding res ID

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetWMSCMDErrResID(CWMSAPP *pMe, wms_cmd_err_e_type cmdErr)
{
  int i;
  int nCount;

  nCount = ARR_SIZE(WMSAPPLookupCmdErr);
  for(i = 0; i< nCount; i++)
  {
    if(WMSAPPLookupCmdErr[i].cmd_err == cmdErr)
    {
      return WMSAPPLookupCmdErr[i].wResID;
    }
  }
  return 0; /* error */
} /* WMSAPPMN_GetWMSCMDResID */
/*===========================================================================
FUNCTION WMSAPPMN_ShowImage

DESCRIPTION
  Displays the image on the Static Control

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_ShowImage(CWMSAPP *pMe, IStatic *pIstatic,char *pszImageFile)
{
#ifdef FEATURE_SMS_UDH
  IImage *       pImage = ISHELL_LoadImage(pMe->a.m_pIShell, pszImageFile);
  if(pImage)
  {
    /*Current image is the m_pSelImage*/
    if (pMe->m_pSelImage)
    {
      IIMAGE_Stop(pMe->m_pSelImage);
      (void)IIMAGE_Release(pMe->m_pSelImage);
      pMe->m_pSelImage = NULL;
    }
    (void)ISTATIC_SetText(pIstatic, NULL,(AECHAR *)pImage,AEE_FONT_NORMAL, AEE_FONT_NORMAL);
    (void)ISTATIC_Redraw(pIstatic);
    pMe->m_pSelImage = pImage;
  }
#endif /* FEATURE_SMS_UDH */
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_ShowAnim

DESCRIPTION
  Displays the animation on the Static Control

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_ShowAnim(CWMSAPP *pMe, IStatic *pIstatic,char *pszImageFile)
{
#ifdef FEATURE_SMS_UDH 
  IImage *       pImage = ISHELL_LoadImage(pMe->a.m_pIShell, pszImageFile);
  if(pImage)
  {
    if (pMe->m_pSelImage)
    {
      IIMAGE_Stop(pMe->m_pSelImage);
      (void)IIMAGE_Release(pMe->m_pSelImage);
      pMe->m_pSelImage = NULL;
    }
    pMe->m_pSelImage = pImage;
    IIMAGE_SetFrameCount(pImage, 4);
    IIMAGE_SetAnimationRate(pImage, 250);
    IIMAGE_Start(pImage,10,10);

  }
#endif /* FEATURE_SMS_UDH */
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_GetRoutes

DESCRIPTION
  Gets the routes information from WMS

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_GetRoutes
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));
  (void)IWMS_CfgGetRoutes(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                     (void*)pMe);
  return TRUE;
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION WMSAPPMN_SaveVoiceMailNum

DESCRIPTION
  Saves the Voicemail number (for eg. *86)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_SaveVoiceMailNum
(
  CWMSAPP *pMe
)
{
  AECHAR *vm;
  IDialog* pIDialog;
  ITextCtl* pITextCtl;
  uint16 wTextCtlID;
#ifdef FEATURE_GWSMS
#error code not present
#endif

  pIDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
    return FALSE;

  wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  if(wTextCtlID == 0)
    return FALSE;

  pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog, (int16)wTextCtlID);
  if(!pITextCtl)
  {
    return FALSE;
  }

  vm = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
  if (vm == NULL) {
    return FALSE;
  }

  if(!(ITEXTCTL_GetText(pITextCtl, vm, WMS_MAX_LEN)))
  {
    WMSAPPU_FREE(vm);
    return FALSE;
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif

  if(pMe->m_bTestMode)
  {
#ifdef FEATURE_GWSMS  
#error code not present
#endif /* FEATURE_GWSMS    */
  }
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_OriginateVoiceCall

DESCRIPTION
  Calls the Given Number

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMN_OriginateVoiceCall(IShell *pIShell, char *num)
{
#ifdef FEATURE_UIONE_HDK
  char *pURLPrefix = "tel:Call?Number=";
  uint8 uURLPrefixLength = STRLEN(pURLPrefix);

  char *pURL = MALLOC( uURLPrefixLength + STRLEN(num) + 1 );
  if (pURL)
  {
    MEMSET(pURL, 0, sizeof(pURL));
    STRLCPY(pURL, pURLPrefix, uURLPrefixLength+1);
    STRLCPY(pURL+uURLPrefixLength, num, STRLEN(num)+1);

    if (ISHELL_SendURL(pIShell, pURL))
    {
      WMSAPPU_FREE(pURL);
      return SUCCESS;
    }
    WMSAPPU_FREE(pURL);
  }
  return EFAILED;
#else
  return OriginateVoiceCall(num);
#endif /* FEATURE_UIONE_HDK */
}

/*===========================================================================
FUNCTION WMSAPPMN_CallVoiceMailNum

DESCRIPTION
  Calls the Voicemail number (for eg. *86)
  by posing event to CoreApp

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_CallVoiceMailNum
(
  CWMSAPP *pMe
)
{
  AECHAR *vm;
  uint16 wDlgID;
  IDialog* pIDialog;
  ITextCtl* pITextCtl;
  uint16 wTextCtlID;
  char *vm_num;
  uint8 length;

  pIDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
    return FALSE;

  wDlgID = IDIALOG_GetID(pIDialog);

  vm = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
  if (vm == NULL) {
    return FALSE;
  }

  if (wDlgID == IDD_VOICEMAIL_NUMBER)
  {
    wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
    if(wTextCtlID == 0)
    {
      WMSAPPU_FREE(vm);
      return FALSE;
    }

    pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);

    if(!pITextCtl)
    {
      WMSAPPU_FREE(vm);
      return FALSE;
    }

    if(!(ITEXTCTL_GetText(pITextCtl, vm, WMS_MAX_LEN)))
    {
      WMSAPPU_FREE(vm);
      return FALSE;
    }
  }
  else if (wDlgID == IDD_INBOX_VIEW)
  {
    if(pMe->m_bTestMode)
    {
#ifdef FEATURE_GWSMS     
#error code not present
#endif /* FEATURE_GWSMS */      
      {
        (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_VOICEMAIL_NUMBER, vm, WMS_MAX_LEN);
      }
    }
  }
  length = (uint8)WSTRLEN(vm);

  vm_num = MALLOC(length+1);
  if (vm_num == NULL) {
    return FALSE;
  }

  (void)WSTRTOSTR(vm, vm_num, length+1);
  vm_num[length] = '\0';

  WMSAPPU_FREE(vm);

  (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
  if (WMSAPPMN_OriginateVoiceCall(pMe->a.m_pIShell, vm_num) == SUCCESS)
  {
    WMSAPPU_FREE(vm_num);
    return TRUE;
  }
  WMSAPPU_FREE (vm_num);
  return FALSE;
}

/*===========================================================================
FUNCTION WMSAPPMN_CallNumber

DESCRIPTION
  Calls the Number present in the Incoming SMS
  by posting event to CoreApp

  Preference of Dial Number
  1. MT CDMA Message
      a. Callback Number (If Present and not Email)
      b. Voicemail Number (If Voicemail Message)
      b. Origination address (If Valid and not Email)
  2. MT GSM / WCDMA Message
      a. Voicemail Number (If Voicemail Message)
      b. Origination address (If Valid and not Email)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_CallNumber
(
  CWMSAPP *pMe
)
{
  boolean isVoiceMail = FALSE;
  boolean validCallback = FALSE;
  boolean validOrigAddress = FALSE;
  char dialNumber[WMS_ADDRESS_MAX];

  (void)MEMSET(dialNumber, 0, WMS_ADDRESS_MAX);

  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {

    /* CDMA MT Message */

    /* Check if Callback is Present */
    if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_CALLBACK)
    {
      if (pMe->m_pClient_ts->u.cdma.callback.number_of_digits > 0)
      {
        if (pMe->m_pClient_ts->u.cdma.callback.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, (uint8)pMe->m_pClient_ts->u.cdma.callback.number_of_digits,
                                    (uint8 *)pMe->m_pClient_ts->u.cdma.callback.digits,
                                    (uint8 *)dialNumber);

#ifdef FEATURE_CDSMS
          if (STRCMP(dialNumber,"6245") == 0)
          {
            /* String Match; Non Std Email */
            (void)MEMSET(dialNumber, 0, WMS_ADDRESS_MAX);
          }
          else
#endif /* FEATURE_CDSMS */
          {
            validCallback = TRUE;
          }
        }
      }
    }

    /* Check for Voicemail if Present */
    if (!validCallback)
    {
      isVoiceMail = IWMS_CfgCheckVoiceMail(pMe->m_pwms, WMS_MESSAGE_MODE_CDMA,
                          &pMe->m_pClient_msg->u.cdma_message.raw_ts,
                          NULL, NULL);
      if (isVoiceMail == TRUE)
      {
        /* This is a Voicemail Message */
        (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_VOICEMAIL_NUMBER, dialNumber, WMS_ADDRESS_MAX);
        if (WMSAPPMN_CallVoiceMailNum(pMe))
        {
          return (TRUE);
        }
        else
        {
          return (FALSE);
        }
      }
    }

    /* Check for Origination Address if Present */
    if (!validCallback)
    {
      /* Check the Origination Address */
      if (pMe->m_pClient_msg->u.cdma_message.address.number_of_digits > 0)
      {
        if (pMe->m_pClient_msg->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, (uint8)pMe->m_pClient_msg->u.cdma_message.address.number_of_digits,
                                    (uint8 *)pMe->m_pClient_msg->u.cdma_message.address.digits,
                                    (uint8 *)dialNumber);

#ifdef FEATURE_CDSMS
          if (STRCMP(dialNumber,"6245") == 0)
          {
            /* String Match; Non Std Email */
            (void)MEMSET(dialNumber, 0, WMS_ADDRESS_MAX);
          }
          else
#endif /* FEATURE_CDSMS */
          {
            validOrigAddress = TRUE;
          }
        }
      }
    }
  }
  else /* GW */
  {
    /* GW MT Message */

    /* Check for Voicemail */
    isVoiceMail = IWMS_CfgCheckVoiceMail(pMe->m_pwms, WMS_MESSAGE_MODE_GW,
                          &pMe->m_pClient_msg->u.gw_message.raw_ts_data,
                          NULL, NULL);
    if (isVoiceMail == TRUE)
    {
      /* This is a Voicemail Message */
      (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_VOICEMAIL_NUMBER, dialNumber, WMS_ADDRESS_MAX);
      if (WMSAPPMN_CallVoiceMailNum(pMe))
      {
        return (TRUE);
      }
      else
      {
        return (FALSE);
      }
    }

    /* Check for Valid Origination Address */

    if (pMe->m_pClient_ts->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER)
    {
      if (pMe->m_pClient_ts->u.gw_pp.u.deliver.address.number_of_digits > 0)
      {
        if (pMe->m_pClient_ts->u.gw_pp.u.deliver.address.number_type == WMS_NUMBER_INTERNATIONAL)
        {
          dialNumber[0] = '+';

          (void)IWMS_TsBcdToAscii( pMe->m_pwms, (uint8*)pMe->m_pClient_ts->u.gw_pp.u.deliver.address.digits,
                              pMe->m_pClient_ts->u.gw_pp.u.deliver.address.number_of_digits,
                              (uint8 *)(dialNumber+1));

          validOrigAddress = TRUE;
        }
        else
        {
          (void)IWMS_TsBcdToAscii( pMe->m_pwms, (uint8*)pMe->m_pClient_ts->u.gw_pp.u.deliver.address.digits,
                              pMe->m_pClient_ts->u.gw_pp.u.deliver.address.number_of_digits,
                              (uint8 *)dialNumber);

          validOrigAddress = TRUE;
        }
      }
    }
  }

  if (validCallback || validOrigAddress)
  {
    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    if (WMSAPPMN_OriginateVoiceCall(pMe->a.m_pIShell, dialNumber) == SUCCESS)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*=============================================================================
FUNCTION:  WMSAPPMN_SetCheckBoxImage

DESCRIPTION: Sets Check Boxes on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void WMSAPPMN_SetCheckBoxImage(IMenuCtl   *pm,
                               uint16      listId,
                               boolean     bSel)
{
   CtlAddItem cai;

   (void)MEMSET(&cai, 0, sizeof(cai));

   (void)IMENUCTL_GetItem(pm, listId, &cai);

   cai.pszResImage = WMSAPP_RES_FILE;
   cai.wImage      = bSel ? IDB_CHECK_YES : IDB_CHECK_NO;
   cai.pImage      = NULL;

   (void)IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
   if (bSel) {
     IMENUCTL_SetSel (pm, listId);
   }
   IMENUCTL_SetStyle(pm, NULL, NULL);
   (void)IMENUCTL_Redraw (pm);
}

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

/*===========================================================================
FUNCTION WMSAPPMN_InitMenuSoftKey

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitMenuSoftKey(CWMSAPP *pMe)
{
  IDialog*  pIDialog;
  IMenuCtl* pIMenuCtl;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif
  uint16 wSKCtrlID;
  uint16 wMenuCtrlID;


  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
  
  if(pIDialog == NULL)
  { /* error */

    return FALSE;
  }
  
  wMenuCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  if(wMenuCtrlID == 0)
    return FALSE;

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wMenuCtrlID);
  if(!pIMenuCtl)
  { /* a menu control existed for this dialog */
    return FALSE;
  }

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenuCtl);

  wSKCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);
  if(wSKCtrlID == 0)
    return FALSE;

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wSKCtrlID);
  if(!pIMenuCtl)
  { /* a menu control existed for this dialog */
    return FALSE;
  }

  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);

  (void)IMENUCTL_Redraw(pIMenuCtl);

  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_CREATEMENUSELECTEDITEMDLG

DESCRIPTION
  It obtains the current menu selection and create the next levelt dialog
  accordingly

DEPENDENCIES
  GetDlgMenuCtlID, GetNextSeletedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_CreateMenuSelectedItemDlg(CWMSAPP *pMe)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  uint16 wDlgID;
  uint16 wItemID;
  IDialog* pIActiveDialog;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL)
  { /* error */

    return FALSE;
  }

  /* Get the menu control */
  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0)
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16)wCtrlID);

  if(pICurrentMenu)
  { /* a menu control existed for this dialog */
    /* Get the selected item */
    wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

    switch(wItemID)
    {
      case IDL_COMPOSE:
        pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
        pMe->m_msg_type = WMSAPP_MSG_TYPE_NEW;
        pMe->m_currPref        = WMSAPP_NO_PREF;
        break;

      case IDL_SETTINGS:
        pMe->m_msgInfo.boxType = WMSAPP_NONE;
        pMe->m_currPref = IDL_SETTINGS;
        break;

      case IDL_NV:
        if(pMe->m_modeControl == WMSAPP_MODE_GW)
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        }
        else
        {
#ifdef FEATURE_CDSMS
          pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA;
#endif /* FEATURE_CDSMS */
        }
        break;

      case IDL_SIM:
        if(pMe->m_modeControl == WMSAPP_MODE_GW)
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        }
        else
        {
#ifdef FEATURE_CDSMS
          pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_RUIM;
#endif /* FEATURE_CDSMS */
        }
        break;

      case IDL_SMS_VOICEMAIL:
        if(pMe->m_modeControl == WMSAPP_MODE_GW)
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        }
        else
        {
#ifdef FEATURE_CDSMS
          pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA;
#endif /* FEATURE_CDSMS */
        }
        break;

      case IDL_RAM:
        if(pMe->m_modeControl == WMSAPP_MODE_GW)
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
        }
        else
        {
#ifdef FEATURE_CDSMS
          /* RAM is not currently supported in CDMA */
          wItemID =0;
#endif /* FEATURE_CDSMS */
        }
        break;

      case IDL_MSG_ROUTES:
        if(pMe->m_modeControl != WMSAPP_MODE_GW)
        {
#ifdef FEATURE_CDSMS
          pMe->m_curClass = WMS_MESSAGE_CLASS_CDMA;
          pMe->m_msgInfo.boxType = WMSAPP_NONE;

          /* In CDMA, on choosing the menu item "Msg Routes"
             create a new screen with "Memory Store" for storing to NV or RUIM
             and "Msg Routes" to be able Discard or Transfer or Transfer and Ack or Store
             an incoming message.
          */
          pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          if(pIActiveDialog == NULL)
          {
            /*error*/
            return FALSE;
          }

          wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIActiveDialog);
          pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16)wCtrlID);
          /* Delete the contents and add the appropriate options for message type */
          if(IMENUCTL_DeleteAll(pICurrentMenu) != TRUE)
          {
            MSG_HIGH("Message type list delete all failed!",0,0,0);
            return FALSE;
          }
          (void)IMENUCTL_Redraw(pICurrentMenu);

          if(IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_MEM_STORE,
                          IDL_MEMORY_STORE, NULL, 0) != TRUE)
          {
            MSG_HIGH("Memory Store could not be added to menu!",0,0,0);
            return FALSE;
          }
          if(IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_ROUTES,
                          IDL_ROUTE, NULL, 0) != TRUE)
          {
            MSG_HIGH("Memory Route could not be added to menu!",0,0,0);
            return FALSE;
          }
		   #if 0
          (void)IMENUCTL_SetTitle(pICurrentMenu,WMSAPP_RES_FILE,IDS_ROUTES,NULL);
		  #else
		  {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_ROUTES,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		   }
		   #endif
          (void)IMENUCTL_Redraw(pICurrentMenu);
          return TRUE;


#endif /* FEATURE_CDSMS */
        }
        break;

      case IDL_CLASS0:
      case IDL_CLASS1:
      case IDL_CLASS2:
      case IDL_CLASS3:
      case IDL_CLASSNONE:
        pMe->m_currPref = wItemID;
        pMe->m_msgInfo.boxType = WMSAPP_NONE;
        break;

      case IDL_MODE_SELECT:
#if !defined(FEATURE_CDSMS) || !defined(FEATURE_GWSMS)
        (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_FEATURE_NOT_AVAILABLE, NULL, SELECTIONVERIFICATIONTIME);
        return FALSE;
#else /* !FEATURE_CDSMS || !FEATURE_GWSMS */
#error code not present
#endif

      case IDL_CB_ACTIVATE:
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/
        return TRUE;

      case IDL_CB_SILENT_ALERT:
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/
        return TRUE;

      case IDL_CB_AUTO_DELETE:
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/
        return TRUE;

      case IDL_CB_LANG_FILTER:
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/
        return TRUE;

        case IDL_CB_READING_ADVISED:
#ifdef FEATURE_UMTS_BMC
#error code not present
#endif /*FEATURE_UMTS_BMC*/
        return TRUE;

        case IDL_CB_READING_OPTIONAL:
#ifdef FEATURE_UMTS_BMC
#error code not present
#endif /*FEATURE_UMTS_BMC*/
        return TRUE;

        case IDL_CB_UPDATE_CHANGE:
#ifdef FEATURE_UMTS_BMC
#error code not present
#endif /*FEATURE_UMTS_BMC*/
        return TRUE;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

      default:
        break;
    }
    /* Get next dialog ID */
    WMSAPPMN_StopImages(pMe);
    wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, wItemID);
    if(wDlgID == 0)
    { /* error */
      return FALSE;
    }
  }
  else
  { /* error */
    return FALSE;
  }

  /* Create next dialog */
  if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                         wDlgID, NULL) == EFAILED)
  { /* Creation failed */

    return FALSE;
  }
  return TRUE; /* Created successfully */
} /* CreateSMSMenuSelectedItemDlg */

/*===========================================================================
FUNCTION WMSAPPMN_CreateSKSelectedDlg

DESCRIPTION
  From the SK, create  a new dialog

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_CreateSKSelectedDlg
(
  CWMSAPP *pMe
)
{
   uint16 wMenuCtrlID,wSoftKeyCtrlID,wDlgID;
   IDialog* pIDialog;
   IMenuCtl* pICurrentMenu;

   pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

   wMenuCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
   pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wMenuCtrlID);

   if(IMENUCTL_IsActive(pICurrentMenu))
   {
      if (WMSAPPMN_CreateMenuSelectedItemDlg(pMe))
      {
        return TRUE;
      }
      else
      {
        MSG_ERROR("Menu Selected Item Dialog Could not be created",0,0,0);
        return FALSE;
      }
   }

   wSoftKeyCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

   if(!wSoftKeyCtrlID)
   {
      return FALSE;
   }

   pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wSoftKeyCtrlID);

   if(!pICurrentMenu)
   {
      return FALSE;
   }

   WMSAPPMN_StopImages(pMe);
   wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, IMENUCTL_GetSel(pICurrentMenu));

   if(wDlgID == 0)
   { /* error */
      return FALSE;
   }

   if(wDlgID == IDD_TEMPLATE_ALPHA_TAG)
   {
      if (WMSAPPMN_StoreAlphaTag(pMe))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
   }


   if(IMENUCTL_IsActive(pICurrentMenu))
   {
     if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                            wDlgID, NULL) == EFAILED)
     { /* Creation failed */

        return FALSE;
     }
   }

   return FALSE;
}


/*===========================================================================
FUNCTION WMSAPPMN_DISPLAYMENUDLG

DESCRIPTION
  Bring the program to the Idle menu screen display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_DisplayMenuDlg(CWMSAPP *pMe, uint16 dlg)
{
  IDialog *pIActiveDialog;
  uint16 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL)
  { /* error */
    return FALSE;
  }
  wDlgID = IDIALOG_GetID(pIActiveDialog);
  while(wDlgID != dlg)
  { /* The program is still not at the Idle menu screen display */
    (void)ISHELL_EndDialog(pMe->a.m_pIShell); /* end this dialog */
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);  /* Get the next active dlg */
    if(pIActiveDialog == NULL)
    { /* error */
      return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);
  }
  return TRUE;
}


/*===========================================================================
FUNCTION WMSAPPMN_WMSAPPMN_AddSoftkeyMenu

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_AddSoftkeyMenu
(
CWMSAPP *pMe,
IDialog* pIDialog,
uint16 wCtrlID
)
{

  IMenuCtl* pISoftKeyMenu;
  
  pISoftKeyMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wCtrlID);

  if(!pISoftKeyMenu)
  {
    return FALSE;
  }

/*IMENUCTL_SetProperties(pISoftKeyMenu, MP_MULTI_SEL);*/
/*IMENUCTL_SetProperties(pISoftKeyMenu, MP_MAXSOFTKEYITEMS);*/


  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISoftKeyMenu);
  (void)IMENUCTL_SetActive(pISoftKeyMenu, FALSE);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitStaticDlg

DESCRIPTION
  Set the static dialog title, content and style

DEPENDENCIES
  SetStaticDlgTitle, SetStaticDlgContent, SetGeneralStaticTextStyle,
  SetStaticTextCtlSize

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_InitStaticDlg(CWMSAPP *pMe)
{
  IStatic* pIStatic;
  uint16 wtitle = 0;
  AECHAR *psContent;
  uint16 wCtlID;
  IMenuCtl* pISKMenu;
  AECHAR psTitle[TITLE_BUFFER];
  //uint32 dwProps;

  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wDlgID = IDIALOG_GetID(pIDialog);

  psContent = MALLOC(sizeof(AECHAR) * TEXT_BUFFER);
  if (psContent == NULL) {
    return FALSE;
  }

  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  if(pIStatic == NULL)
  {/*error*/
    WMSAPPU_FREE(psContent);
    return FALSE;
  }
 /*Enable Auto Scroll*/
  //dwProps = ISTATIC_GetProperties(pIStatic);
  //dwProps &= ~ST_NOSCROLL;
  //dwProps |= ST_ENABLE_HLGHT;
  //(void)ISTATIC_SetProperties(pIStatic, dwProps);
  //ISTATIC_Redraw(pIStatic);

  wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);

  pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  if(pISKMenu)
  {
     (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
  }

  switch(wDlgID)
  {
    case IDD_OTHERINFO:
      /* Set the title and content */
      wtitle = IDS_OTHER_INFORMATION;
      if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        (void)MEMCPY(pMe->m_pClient_ts, pMe->m_pClient_ts_mo, sizeof(wms_client_ts_data_s_type));
        (void)MEMCPY(pMe->m_pClient_msg, pMe->m_pClient_msg_mo, sizeof(wms_client_message_s_type));
      }
      WMSAPPMD_DisplayOtherInfo(pMe,psContent);
      break;

    case IDD_VIEW_TMPLT:
      wtitle = IDS_TEMPLATE;
      WMSAPPMD_DisplayTemplate(pMe,psContent,pMe->m_pMO_template_msg);
      break;

    case IDD_USAGE:
      wtitle = IDS_USAGE;
      (void)WMSAPPMD_DisplayMemoryUsage(pMe,psContent);
      if (pMe->m_msgInfo.memStore != WMS_MEMORY_STORE_SIM)
      {
        (void)IMENUCTL_DeleteItem(pISKMenu, IDL_SK_STATUS_REPORT);
        
      }
      break;

    case IDD_DCS_SUMMARY:
      wtitle  = IDS_CLASS_SUMMARY;
      (void)WMSAPPMD_DisplayClassSummary(pMe,psContent);
      break;

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    default:
      WMSAPPU_FREE(psContent);
      return FALSE;
  }
  (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                        WMSAPP_RES_FILE,
                        wtitle,
                        psTitle,
                        TEXT_BUFFER*sizeof(AECHAR));

  (void)ISTATIC_SetText(pIStatic, psTitle, psContent, AEE_FONT_BOLD, AEE_FONT_NORMAL);
  (void)ISTATIC_Redraw(pIStatic);

  WMSAPPU_FREE(psContent);
  return TRUE;
} /*End WMSAPPMN_InitStaticDlg */

/*===========================================================================
FUNCTION WMSAPP_GetMsgListResID

DESCRIPTION
  Gets the Resource ID from the message list

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPP_GetMsgListResID
(
  CWMSAPP *pMe,
  int index
)
{
  if(pMe == NULL)
  {
    MSG_HIGH("pMe is NULL",0,0,0);
    return FALSE;
  }
  if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NV_GW ||
     pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_NV_CDMA)
  {
    if (index < ARR_SIZE(WMSAPP_NV_MSG))
    {
      return (uint16)WMSAPP_NV_MSG[index];
    }
    else
    {
      return 0;
    }
  }
  if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RAM_GW ||
     pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RAM_CDMA)
  {
    if (index < ARR_SIZE(WMSAPP_TEMP_MSG))
    {
      return (uint16)WMSAPP_TEMP_MSG[index];
    }
    else
    {
      return 0;
    }
  }
  if(pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_RUIM ||
     pMe->m_msgInfo.memStore == WMS_MEMORY_STORE_SIM)
  {
    if (index < ARR_SIZE(WMSAPP_CARD_MSG))
    {
      return (uint16)WMSAPP_CARD_MSG[index];
    }
    else
    {
      return 0;
    }
  }

  return 0;
}


/*===========================================================================
FUNCTION WMSAPPMN_SetMsgListDisplay

DESCRIPTION
  Sets the Display for the list of messages

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_SetMsgListDisplay
(
  CWMSAPP *pMe,
  wmsapp_box_type box_type
)
{

  uint16 wCtlID, wItemID;
  IMenuCtl* pIMenu;
  IMenuCtl* pSKMenu;
  int i;
  int count;
  CtlAddItem pai;
  AEERect rc;
  
  IDialog* pIDialog;
  uint16 wDlgID;
  AECHAR text[sizeof(AECHAR) * (WMS_MSG_INFO_CACHE_SIZE+2)];
  wms_message_number_type   msgIndex = (uint32)WMS_DUMMY_MESSAGE_INDEX;
  wms_message_tag_e_type    msgTag = WMS_TAG_NONE;
  wms_memory_store_e_type   cacheMemStore = WMS_MEMORY_STORE_NONE;

  if(pMe == NULL)
  {
    MSG_HIGH("pMe is NULL",0,0,0);
    return FALSE;
  }  /* Get the Controls and the Dialog */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wDlgID = IDIALOG_GetID(pIDialog);

  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if(wCtlID == 0)
  {
    return FALSE;
  }

  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  if(pIMenu == NULL)
  {
    return FALSE;
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  /*INBOX or OUTBOX*/
  switch(pMe->m_msgInfo.memStore)
  {
    case WMS_MEMORY_STORE_RAM_GW:
    case WMS_MEMORY_STORE_RAM_CDMA:
      if(box_type == WMSAPP_INBOX)
      {
        #if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_TEMP_MEMORY_INBOX,NULL);
		#else
		 {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_TEMP_MEMORY_INBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		#endif
      }
      else
      {
        #if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_TEMP_MEMORY_OUTBOX,NULL);
		#else
		 {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_TEMP_MEMORY_OUTBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		 #endif
      }
      break;

    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_SIM:
      if(box_type == WMSAPP_INBOX)
      {
        #if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_CARD_MEMORY_INBOX,NULL);
		#else
		 {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_CARD_MEMORY_INBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		 #endif
      }
      else if(box_type == WMSAPP_OUTBOX)
      {
         #if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_CARD_MEMORY_OUTBOX,NULL);
		 #else
		 {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_CARD_MEMORY_OUTBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		 #endif
      }
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/
      break;


    case WMS_MEMORY_STORE_NV_CDMA:
    case WMS_MEMORY_STORE_NV_GW:
      if(box_type == WMSAPP_INBOX)
      {
      	#if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_PHONE_MEMORY_INBOX,NULL);
		#else
		{
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_PHONE_MEMORY_INBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		#endif
      }
      else
      {
      	#if 0
        (void)IMENUCTL_SetTitle(pIMenu,WMSAPP_RES_FILE,IDS_PHONE_MEMORY_OUTBOX,NULL);
		#else
		{
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            IDS_PHONE_MEMORY_OUTBOX,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 		}
		#endif
      }
      break;

    default:
      break;

  }

  /* Get the Number of Messages */
  count = WMSAPPU_GetMsgCount(pMe, box_type, pMe->m_msgInfo.memStore);


  /*If there are no Messages in the list, Display "No Messages" */
  if(count ==0)
  {
    wItemID = WMSAPP_GetMsgListResID(pMe,0);

    if(wItemID == 0)
      return FALSE;

    /*Delete all the list items*/
    (void)IMENUCTL_DeleteAll(pIMenu);

    (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,
                     IDS_NO_MESSAGES, wItemID, NULL, 1);

    (void)IMENUCTL_GetRect(pIMenu, &rc);
    (void)IMENUCTL_SetRect(pIMenu, &rc);

    //IMENUCTL_Redraw(pIMenu);
    //return TRUE;
  }
  else
  {
    /*List the messages*/
    for(i=0;i<count;i++)
    {
      char* list_text = MALLOC(WMS_MSG_INFO_CACHE_SIZE+2);      
      int index;
      int scrLine =0;

      if (list_text == NULL) {
        return FALSE;
      }

      (void)MEMSET(list_text,0,WMS_MSG_INFO_CACHE_SIZE+2);
      /*Get the resource ID and add the item to the menu*/
      if(i>=IMENUCTL_GetItemCount(pIMenu))
      {
        wItemID = WMSAPP_GetMsgListResID(pMe,i);
        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,
                               IDS_NO_MESSAGES, wItemID, NULL, 0);
      }
      else
        wItemID = IMENUCTL_GetItemID(pIMenu, i);

      /*From the List Item ID, get the scrl line number*/
      scrLine = WMSAPPMN_GetListIndex(pMe, wItemID);

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/

      if(box_type == WMSAPP_INBOX || box_type == WMSAPP_OUTBOX)
      {
        /*from the Scroll Line, get the index in smsLists for the correct box*/
        index = (int)WMSAPPU_MapIndex(pMe, pMe->m_msgInfo.memStore, scrLine);

        if(pMe->m_bTestMode)
        {
          cacheMemStore = pMe->m_msgInfo.memStore;
        }
        /*The index of the element is retrieved from this function instead of the
        index of the message because, smsLists stores other items as well */

        msgIndex = pMe->m_pSmsLists[cacheMemStore]->indices[index];
        msgTag = pMe->m_pSmsLists[cacheMemStore]->tags[index];
      }

      if(pMe->m_bTestMode)
      {
        list_text[0] = (uint8)msgIndex/10+'0';
        list_text[1] = (uint8)msgIndex%10+'0';

        /* display text in the message list */
#if defined(FEATURE_GWSMS_CACHE)  || defined(FEATURE_CDSMS)
        if(IWMS_MsgGetCacheInfo(  pMe->m_pwms,
                                  cacheMemStore,
                                  msgIndex,
                                  msgTag,
                                  (uint8*)list_text+2) == WMS_OK_S)
        {
          list_text[2] = ' ';
        }
        else
        {
          MSG_HIGH("Could not get info from WMS msg cache",0,0,0);
        }
        (void)STRTOWSTR(list_text,text, sizeof(AECHAR) * (WMS_MSG_INFO_CACHE_SIZE+2));
#endif /*FEATURE_GWSMS_CACHE*/
      }
      (void)IMENUCTL_SetItemText(pIMenu,wItemID,WMSAPP_RES_FILE,0,text);
      (void)IMENUCTL_GetItem(pIMenu,wItemID,&pai);
      /*If the message has not been read, it is BOLD*/
      if(msgTag == WMS_TAG_MT_NOT_READ  ||
         msgTag == WMS_TAG_MO_NOT_SENT   )
      {
        pai.pszResImage = pai.pszResText=WMSAPP_RES_FILE;
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu, wItemID, MSIF_FONT, &pai);
      }
      else
      {
        pai.pszResImage = pai.pszResText=WMSAPP_RES_FILE;
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu, wItemID, MSIF_FONT, &pai);
      }

      WMSAPPU_FREE(list_text);
    }

    /*Delete all the extra items from the menu*/
    while(i<IMENUCTL_GetItemCount(pIMenu))
    {
      wItemID = IMENUCTL_GetItemID(pIMenu, i);
      (void)IMENUCTL_DeleteItem(pIMenu,wItemID);
    }
  }

  (void)WMSAPPMN_InitMenuSoftKey(pMe);

  /* Set the softkeys appropriately */
  if(wDlgID == IDD_NV_MESSAGE_ACCESS)
  {
    pSKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_SK_NV_MESSAGE_ACCESS);


    if(pMe->m_bTestMode)
    {
      boolean changeSelection = TRUE;
      switch (box_type)
      {
        case WMSAPP_INBOX:
          if (IMENUCTL_GetSel(pSKMenu) == IDL_SK_NV_INBOX)
            changeSelection = FALSE;
          break;

        case WMSAPP_OUTBOX:
          if (IMENUCTL_GetSel(pSKMenu) == IDL_SK_NV_OUTBOX)
            changeSelection = FALSE;
          break;

        default:
          break;
      }
      if (changeSelection == TRUE)
      {
        (void)IMENUCTL_DeleteAll(pSKMenu);
        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_INBOX,
                        IDL_SK_NV_INBOX, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_OUTBOX,
                        IDL_SK_NV_OUTBOX, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_USAGE,
                        IDL_SK_NV_USAGE, NULL, 0);

        switch (box_type)
        {
          case WMSAPP_INBOX:
            IMENUCTL_SetSel(pSKMenu, IDL_SK_NV_INBOX);
            break;

          case WMSAPP_OUTBOX:
            IMENUCTL_SetSel(pSKMenu, IDL_SK_NV_OUTBOX);
            break;

          default:
            break;
        }
      }
    }
    else
    {
      (void)IMENUCTL_DeleteAll(pSKMenu);

      if(pMe->m_msgInfo.boxType == WMSAPP_INBOX ||
         pMe->m_msgInfo.boxType == WMSAPP_BROADCAST)
      {
        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_READ,
                         IDL_SK_AUTO_INBOX_READ, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_DELETE,
                         IDL_SK_AUTO_INBOX_DELETE, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_DONE,
                         IDL_SK_AUTO_INBOX_DONE, NULL, 0);
      }
      else if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_READ,
                         IDL_SK_AUTO_OUTBOX_READ, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_SEND,
                         IDL_SK_AUTO_OUTBOX_SEND, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_STATUS,
                         IDL_SK_AUTO_OUTBOX_STATUS, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_DELETE,
                         IDL_SK_AUTO_OUTBOX_DELETE, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_INFO,
                         IDL_SK_AUTO_OUTBOX_OTHER_INFO, NULL, 0);

        (void)IMENUCTL_AddItem(pSKMenu, WMSAPP_RES_FILE, IDS_DONE,
                         IDL_SK_AUTO_OUTBOX_DONE, NULL, 0);
      }
    }
  }
  (void)IMENUCTL_Redraw(pIMenu);

  return TRUE;
}

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

/*===========================================================================
FUNCTION WMSAPPMN_InitTemplate

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitTemplate
(
CWMSAPP *pMe
)
{
  AECHAR *defaultTxt;
  CtlAddItem pai;
  AECHAR* text;

  int i=1;
  uint16 wItemID;
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID =  WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  IMenuCtl *pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  (void)IMENUCTL_DeleteAll(pIMenu);


  defaultTxt = MALLOC(sizeof(AECHAR) * defaultTxtLen);

  if(defaultTxt == NULL)
    return FALSE;

  (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                        WMSAPP_RES_FILE,
                        IDS_DEFAULT_BRACKET,
                        defaultTxt,
                        defaultTxtLen*sizeof(AECHAR));

  text = MALLOC(sizeof(AECHAR)*WMS_MSG_INFO_CACHE_SIZE );

  if(text == NULL)
  {
    WMSAPPU_FREE(defaultTxt);
    return FALSE;
  }

  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    if (pMe->m_CDSMSDefaultTemplate > (uint8)pMe->m_pCDSMSTemplateList->len)
    {
      pMe->m_CDSMSDefaultTemplate = pMe->m_pCDSMSTemplateList->len;
      (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_CDMA_DEFAULT_TEMPLATE, &pMe->m_CDSMSDefaultTemplate, 1);
    }
    for(i=0;i<=pMe->m_pCDSMSTemplateList->len;i++)
    {
      char* list_text;
      int offset = 0;
      char *memType;

      wItemID = WMSAPPMN_GetTemplateListResID(i);

      // First item in the list is NV template
      if(i >0)
      {
        list_text = MALLOC(WMS_MSG_INFO_CACHE_SIZE );

        if(list_text == NULL)
        {
          WMSAPPU_FREE(defaultTxt);
          WMSAPPU_FREE(text);
          return FALSE;
        }

        memType = "RUIM CARD ";
        (void)STRLCPY(list_text, memType, WMS_MSG_INFO_CACHE_SIZE);
        offset = (int)STRLEN(memType);

        list_text[offset++] = (uint8)pMe->m_pCDSMSTemplateList->indices[i-1]/10 + '0';
        list_text[offset++] = (uint8)pMe->m_pCDSMSTemplateList->indices[i-1]%10 + '0';

        list_text[offset++] = '\0';

        (void)STRTOWSTR((char*)list_text,text, (int)((int)sizeof(AECHAR) * offset));

        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,IDS_NO_MESSAGES,
                        wItemID, NULL, 0);



        WMSAPPU_FREE(list_text);
      }
      else
      {
        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,IDS_NV_CONFIG,
                        wItemID, NULL, 0);

        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                            WMSAPP_RES_FILE,
                            IDS_NV_CONFIG,
                            text,
                            10* sizeof(AECHAR));
      }

      if(pMe->m_CDSMSDefaultTemplate == i)
      {
        (void)WSTRLCAT (text, defaultTxt, WMS_MSG_INFO_CACHE_SIZE);
      }

      (void)IMENUCTL_SetItemText (pIMenu, wItemID, NULL, 0, text);
      (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
      pai.wFont = AEE_FONT_NORMAL;
      (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
    }
  }
  else
  {
    if (pMe->m_GWSMSDefaultTemplate > pMe->m_pGWSMSTemplateList->len)
    {
      pMe->m_GWSMSDefaultTemplate = pMe->m_pGWSMSTemplateList->len;
      (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_GW_DEFAULT_TEMPLATE, &pMe->m_GWSMSDefaultTemplate, 1);
    }
    for(i=0;i<=pMe->m_pGWSMSTemplateList->len;i++)
    {
      char* list_text;
      int offset = 0;
      char *memType;

      wItemID = WMSAPPMN_GetTemplateListResID(i);

      // First item in the list is NV template
      if(i >0)
      {
        list_text = MALLOC(WMS_MSG_INFO_CACHE_SIZE );

        if(list_text == NULL)
        {
          WMSAPPU_FREE(defaultTxt);
          WMSAPPU_FREE(text);
          return FALSE;
        }

        memType = "SIM CARD ";
        (void)STRLCPY(list_text,memType,WMS_MSG_INFO_CACHE_SIZE);
        offset = (int)STRLEN(memType);

        list_text[offset++] = (uint8)pMe->m_pGWSMSTemplateList->indices[i-1]/10 + '0';
        list_text[offset++] = (uint8)pMe->m_pGWSMSTemplateList->indices[i-1]%10 + '0';

        list_text[offset++] = '\0';

        (void)STRTOWSTR((char*)list_text,text, (int)((int)sizeof(AECHAR) * offset));

        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,IDS_NO_MESSAGES,
                        wItemID, NULL, 0);



        WMSAPPU_FREE(list_text);
      }
      else
      {
        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,IDS_NV_CONFIG,
                        wItemID, NULL, 0);

        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                            WMSAPP_RES_FILE,
                            IDS_NV_CONFIG,
                            text,
                            10* sizeof(AECHAR));
      }

      if(pMe->m_GWSMSDefaultTemplate == i)
      {
        (void)WSTRLCAT (text, defaultTxt, WMS_MSG_INFO_CACHE_SIZE);
      }

      (void)IMENUCTL_SetItemText (pIMenu, wItemID, NULL, 0, text);
      (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
      pai.wFont = AEE_FONT_NORMAL;
      (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);

    }
  }

  (void)WMSAPPMN_InitMenuSoftKey(pMe);
  (void)IMENUCTL_Redraw(pIMenu);
  WMSAPPU_FREE(defaultTxt);
  WMSAPPU_FREE(text);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitAbsValDlg

DESCRIPTION
  Handles Absolute Validity Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitAbsValDlg
(
CWMSAPP *pMe
)
{
  IDialog *pIDialog;
  uint16 wCtrlID;
  IMenuCtl* pICurrentMenu;
  wms_timestamp_s_type abs_time;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if(wCtrlID == 0)
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  if (pICurrentMenu == NULL) /* error checking */
  {
    MSG_ERROR("Menu Control is NULL",0,0,0);
    return FALSE;
  }

  /* Initialize abs_time to zero */
  (void)MEMSET(&abs_time, 0, sizeof(wms_timestamp_s_type));

  /* Getting the Absolute Time */
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {
      if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
      {
        abs_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute;
      }
      else /* deferred abs */
      {
        abs_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute;
      }
    }
    else
    {
      if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
      {
        abs_time = pMe->m_pClient_ts_mo->u.cdma.validity_absolute;
      }
      else /* deferred abs */
      {
        abs_time = pMe->m_pClient_ts_mo->u.cdma.deferred_absolute;
      }
    }
#endif /* FEATURE_CDSMS */
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  /* Year */
  /* Range Checking as per WMSAPP Resource File */
  if ( (WMSAPPU_ConvertBCDToInt(abs_time.year) >= 3) && (WMSAPPU_ConvertBCDToInt(abs_time.year) <= 7) )
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_YEAR, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_YEAR, FALSE);
  }

  /* Month */
  /* Range Checking as per WMSAPP Resource File */
  if ( (WMSAPPU_ConvertBCDToInt(abs_time.month) >= 1) && (WMSAPPU_ConvertBCDToInt(abs_time.month) <= 12) )
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_MONTH, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_MONTH, FALSE);
  }

  /* Day */
  /* Range Checking as per WMSAPP Resource File */
  if ( (WMSAPPU_ConvertBCDToInt(abs_time.day) >= 1) && (WMSAPPU_ConvertBCDToInt(abs_time.day) <= 31) )
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_DAY, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_DAY, FALSE);
  }

  /* Hour */
  /* Range Checking as per WMSAPP Resource File */
  if (abs_time.month <= 23)
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_HOUR, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_HOUR, FALSE);
  }

  /* Minute */
  /* Range Checking as per WMSAPP Resource File */
  if (  (WMSAPPU_ConvertBCDToInt(abs_time.minute) == 10)
     || (WMSAPPU_ConvertBCDToInt(abs_time.minute) == 20)
     || (WMSAPPU_ConvertBCDToInt(abs_time.minute) == 30)
     || (WMSAPPU_ConvertBCDToInt(abs_time.minute) == 40)
     || (WMSAPPU_ConvertBCDToInt(abs_time.minute) == 50) )
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_MINUTE, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_MINUTE, FALSE);
  }

  /* Time Zone */
  /* Range Checking as per WMSAPP Resource File */
  /* Introduce another condition for GW Only */
  if ((abs_time.timezone >= -11) && (abs_time.timezone <= 11)
      && (pMe->m_modeControl == WMSAPP_MODE_GW))
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_TIMEZONE, TRUE);
  }
  else
  {
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ABS_VAL_TIMEZONE, FALSE);
  }
  
  /* Delete the Timezone Menu Item if mode is not GW */
  if (pMe->m_modeControl != WMSAPP_MODE_GW)
  {
    (void)IMENUCTL_DeleteItem(pICurrentMenu, IDL_ABS_VAL_TIMEZONE);
  }

  /* remove certain items based on featurization */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitSettingsDlg

DESCRIPTION
  Initializes the Settings Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitSettingsDlg
(
  CWMSAPP *pMe
)
{
  IDialog *pIDialog;
  uint16 wCtrlID;
  IMenuCtl* pICurrentMenu;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if(wCtrlID == 0)
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
  
  if (pICurrentMenu == NULL) /* error checking */
  {
    MSG_ERROR("Menu Control is NULL",0,0,0);
    return FALSE;
  }

  (void)IMENUCTL_DeleteAll(pICurrentMenu);

  if(pMe->m_bTestMode)
  {
    /* Templates */
    (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_TEMPLATES,
                     IDL_TEMPLATES, NULL, 0);

    /* Voicemail */
    
    // Voicemail menu enabled only for CDMA mode.  For GW, voicemail and 
    // voicemail line 2 are incomplete features and hence not being shown
    // in WMS App Menu.  If implemented, a new menu would need to be added
    // here for Alternate Line 2 Service also.  It would need to read/write
    // MBDN(Mailbox Directory Number) from SIM/NV as required.
    if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
       (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_VOICEMAILNUM,
                     IDL_VOICEMAIL_NUM, NULL, 0);
    }
    
    /* Msg routes */
    (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_ROUTES,
                     IDL_MSG_ROUTES, NULL, 0);

    /* SMS Retry Setting */
    (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_RETRY_SETTINGS,
                     IDL_RETRY_SETTING, NULL, 0);

#ifdef FEATURE_CDSMS_BROADCAST
    /* Broadcast */
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
      (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_BROADCAST,
                       IDL_BROADCAST, NULL, 0);
    }
#endif /* FEATURE_CDSMS_BROADCAST */
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#if defined(FEATURE_GWSMS) && defined(FEATURE_CDSMS)
#error code not present
#endif /* FEATURE_GWSMS && FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    /* Alert Settings */
    (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_ALERT_SETTINGS,
                     IDL_ALERT_SETTINGS, NULL, 0);

    /* Segmentation Settings */
    (void)IMENUCTL_AddItem(pICurrentMenu, WMSAPP_RES_FILE, IDS_SEGMENT_SETTING,
                     IDL_SEGMENT_SETTING, NULL, 0);
  }

  WMSAPPMN_GetCheckBoxItems(pMe);
  (void)WMSAPPMN_InitMenuSoftKey(pMe);
  /* remove certain items based on featurization */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);

  if(pMe->m_bTestMode)
  {
    IMENUCTL_SetFocus(pICurrentMenu, IDL_TEMPLATES);
  }

  /* To Refresh the Menu Control */
  (void)IMENUCTL_Redraw(pICurrentMenu);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitAlertSettingsDlg

DESCRIPTION
  Initialize the Alert Settings Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitAlertSettingsDlg
(
  CWMSAPP *pMe
)
{
  IDialog *pIDialog;
  uint16 wCtrlID;
  IMenuCtl* pICurrentMenu;
  CtlAddItem pai;
  uint8 cfgVal;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if(wCtrlID == 0)
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  if (pICurrentMenu == NULL) /* error checking */
  {
    MSG_ERROR("Menu Control is NULL",0,0,0);
    return FALSE;
  }

  /* Initialize the various fields based on the User Settings */

  /* Display Alert */
  (void)ICONFIG_GetItem(pMe->m_pIConfig,CFGI_SMS_ALERT_DISPLAY, &cfgVal, 1);
  if (cfgVal == OEMNV_SMS_ALERT_DISPLAY_ENABLE)
  {
    pMe->m_bDisplayAlert = TRUE;
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ALERT_DISPLAY, TRUE);
  }
  else
  {
    pMe->m_bDisplayAlert = FALSE;
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ALERT_DISPLAY, FALSE);
  }

  /* Beep Alert */
  (void)ICONFIG_GetItem(pMe->m_pIConfig,CFGI_SMS_ALERT_SOUND, &cfgVal, 1);
  if (cfgVal == OEMNV_SMS_ALERT_SOUND_ENABLE)
  {
    pMe->m_bBeepAlert = TRUE;
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ALERT_BEEP, TRUE);
  }
  else
  {
    pMe->m_bBeepAlert = FALSE;
    WMSAPPMN_SetCheckBoxImage(pICurrentMenu, IDL_ALERT_BEEP, FALSE);
  }

  /* Beep Volume */
  (void)ICONFIG_GetItem(pMe->m_pIConfig,CFGI_SMS_ALERT_VOLUME, &cfgVal, 1);
  if (cfgVal > 4)
  {
    /* Default it to value 2 */
    cfgVal = 2;
    (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_VOLUME, &cfgVal, 1);
  }
  pMe->m_BeepVolume = cfgVal;

  (void)IMENUCTL_GetItem(pICurrentMenu, IDL_ALERT_VOLUME, &pai);

  switch (pMe->m_BeepVolume)
  {
    case 0:
      pai.wImage = IDB_VOLUME_0;
      break;

    case 1:
      pai.wImage = IDB_VOLUME_1;
      break;

    case 2:
      pai.wImage = IDB_VOLUME_2;
      break;

    case 3:
      pai.wImage = IDB_VOLUME_3;
      break;

    case 4:
      pai.wImage = IDB_VOLUME_4;
      break;

    default:
      pai.wImage = IDB_VOLUME_2;
      break;
  }
  pai.pszResImage = pai.pszResText = WMSAPP_RES_FILE;
  pai.pImage = NULL;
  (void)IMENUCTL_SetItem(pICurrentMenu, IDL_ALERT_VOLUME, MSIF_IMAGE, &pai);

  /* Refresh the Menu */
  (void)IMENUCTL_Redraw(pICurrentMenu);

  /* remove certain items based on featurization */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;
}


/*===========================================================================
FUNCTION WMSAPPMN_CheckReturnDlg

DESCRIPTION
  End a SMS dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Ends the Current Active Dialog
===========================================================================*/
boolean WMSAPPMN_CheckReturnDlg(CWMSAPP *pMe)
{
  IDialog * pIActiveDialog;
  uint16 wDlgID;


  if(pMe->m_pStaticSecText != NULL)
  {
    (void)ISTATIC_Release(pMe->m_pStaticSecText);
    pMe->m_pStaticSecText = NULL;
  }


  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL)
    return TRUE;

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  switch(wDlgID)
  {
    case IDD_OUTBOX_VIEW:
    case IDD_INBOX_VIEW:
    case IDD_COMPOSE_MSG:
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DLG_RESUME, 0, 0);
      break;

    case IDD_VIEW_MESSAGE_INFO:
      if (WMSAPPU_GetMsgCount(pMe, pMe->m_msgInfo.boxType, pMe->m_msgInfo.memStore) != 0) {
         (void)WMSAPPMD_DisplayMessageInfo(pMe);
      } else
      {
         (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                    EVT_WMS_DLG_RESUME, 0, 0);
      }


      break;

    case IDD_VIEW_TMPLT:
    case IDD_USAGE:
      (void)WMSAPPMN_InitStaticDlg(pMe);
      break;

    case IDD_DESTADDR:
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DLG_RESUME, 0, 0);
      break;

#ifdef FEATURE_CDSMS_BROADCAST
    case IDD_BC_SERVICES:
      (void)WMSAPPMN_InitBCServices(pMe);
      break;
#endif /* FEATURE_CDSMS_BROADCAST_ */

    case IDD_REL_VALIDITY:
    case IDD_VALIDITY:
      if(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }

    case IDD_ABS_VALIDITY:
      return (WMSAPPMN_InitAbsValDlg(pMe));

    case IDD_MAINSMS:
      return (WMSAPPMN_InitIdleMenuDlg(pMe));

    case IDD_SETTINGS:
      return (WMSAPPMN_InitSettingsDlg(pMe));

    case IDD_NV_MESSAGE_ACCESS:
    case IDD_TEMP_MESSAGE_ACCESS:
    case IDD_CARD_MESSAGE_ACCESS:
      return (WMSAPPMN_SetMsgListDisplay(pMe, pMe->m_msgInfo.boxType));

    case IDD_TEMPLATE_LIST:
      return (WMSAPPMN_InitTemplate(pMe));

    case IDD_CDMA_TEST:
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DLG_RESUME, 0, 0);
      break;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    case IDD_MESSAGE:
      /* Dont want to display this Dialog on Returning back */
      (void)WMSAPPMN_EndDlg(pMe);
      break;

    case IDD_NEW_MSG_RXD:
      (void)WMSAPPMN_InitNewMtMsg(pMe);
      break;

    default:
      break;
  }
  return TRUE;

} /*WMSAPPMN_CheckReturnDlg*/


/*===========================================================================
FUNCTION WMSAPPMN_EndNDlgs

DESCRIPTION
  End a SMS dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Ends the Current Active Dialog
===========================================================================*/
boolean  WMSAPPMN_EndNDlgs(CWMSAPP *pMe, int n)
{
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  int i=0;

  uisnd_snd_stop();
  if(pMe->m_pStaticSecText != NULL)
  {
    (void)ISTATIC_Release(pMe->m_pStaticSecText);
    pMe->m_pStaticSecText = NULL;
  }
  for (i=0;i<n;i++)
  {
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIActiveDialog == NULL)
    { /* error checking */
      return FALSE;
    }

    if(pIActiveDialog != NULL)
    {
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
    }
  }

  /*Close All Previous Message Dlgs*/
  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL)
  {
    return TRUE;
  }
  wDlgID = IDIALOG_GetID(pIActiveDialog);

  while(wDlgID == IDD_MESSAGE)
  {
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIActiveDialog == NULL)
    {
      return TRUE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);

  }

  WMSAPPMN_StopImages(pMe);
  (void)WMSAPPMN_CheckReturnDlg(pMe);
  return TRUE;

} /*WMSAPPMN_EndNDlgs*/


/*===========================================================================
FUNCTION WMSAPPMN_EndDlg

DESCRIPTION
  End a SMS dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Ends the Current Active Dialog
===========================================================================*/
boolean  WMSAPPMN_EndDlg(CWMSAPP *pMe)
{
  return (WMSAPPMN_EndNDlgs(pMe,1));
} /*WMSAPPMN_EndDlg*/

/*===========================================================================
FUNCTION WMSAPPMN_EndMsgDlg

DESCRIPTION
  End a SMS Msg (Popup) dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Ends the Current Active Dialog if it is a Popup (Msg) Dialog
===========================================================================*/
boolean  WMSAPPMN_EndMsgDlg(CWMSAPP *pMe)
{
  uint16 wDlgID = 0;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL)
  {
    return FALSE;
  }

  if(pMe->m_pStaticSecText != NULL)
  {
    (void)ISTATIC_Release(pMe->m_pStaticSecText);
    pMe->m_pStaticSecText = NULL;
  }

  wDlgID = IDIALOG_GetID(pIDialog);

  if (wDlgID == IDD_MESSAGE)
  {
    return (WMSAPPMN_EndNDlgs(pMe,1));
  }
  return TRUE;
} /*WMSAPPMN_EndMsgDlg*/

/*===========================================================================
FUNCTION WMSAPPMN_DisplayMsgDlg

DESCRIPTION
  Create a message dialog with the approprite Resource ID and Dialog Display
  Time (in milliseconds)
  If Resource ID = 0, displays the char string passed to it.

USAGE
  WMSAPPMN_DisplayMsgDlg(pMe, <Valid Resource ID>, NULL, <Display Time> )
  or
  WMSAPPMN_DisplayMsgDlg(pMe, 0, <Valid Char String>, <Display Time> )

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_DisplayMsgDlg
(
  CWMSAPP *pMe,
  uint16 wResID,
  char *text,
  int displayTimeMSec
)
{
  int retVal;
  IStatic* pIStatic;
  IDialog* pIDialog;
  AECHAR *psContents;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL)
  {
    MSG_HIGH("Null pointer in WMSAPPMN_DisplayMsgDlg",0,0,0);
    return FALSE;
  }
  
  retVal = ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE, IDD_MESSAGE, NULL);

  if(retVal != EFAILED && displayTimeMSec > 0)
  {
    /* Reset idle Timer, since we are successful in creating the message display */
    /*(void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);*/
    retVal = ISHELL_SetTimer(pMe->a.m_pIShell,
                             displayTimeMSec,
                             (PFNNOTIFY) WMSAPPMN_EndMsgDlg,
                             (uint32*) pMe);
  }

  psContents = MALLOC(TEXT_BUFFER *sizeof(AECHAR));

  if(psContents == NULL)
  {
    return FALSE;
  }

  if (wResID > 0)
  {
    (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                          WMSAPP_RES_FILE,
                          wResID,
                          psContents,
                          TEXT_BUFFER*sizeof(AECHAR));
  }
  else
  {
    (void)STRTOWSTR(text, psContents,TEXT_BUFFER*sizeof(AECHAR));
  }

  WMSAPPMN_StopImages(pMe);
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pIStatic = (IStatic *) IDIALOG_GetControl(pIDialog, IDC_STATIC_MSG);
  (void)ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
  retVal = ISTATIC_SetText(pIStatic, NULL, psContents, AEE_FONT_BOLD, AEE_FONT_BOLD);
  (void)ISTATIC_Redraw(pIStatic);

  WMSAPPU_FREE(psContents);

  if(retVal == SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /*WMSAPPMN_DisplayMsgDlg*/

/*===========================================================================
FUNCTION WMSAPPMN_DisplayError

DESCRIPTION
  Displays Error Msgs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMN_DisplayError
(
  CWMSAPP *pMe,
  wms_cmd_id_e_type cmd,
  wms_cmd_err_e_type cmd_err
)
{
  AECHAR *psBuffer = MALLOC(TITLE_BUFFER * sizeof(AECHAR));
  AECHAR *psContents = MALLOC(TEXT_BUFFER * sizeof(AECHAR));
  uint16 retVal;
  IDialog* pIDialog;
  IStatic * pIStatic;
  uint16 wResID;


  if(psContents == NULL || psBuffer == NULL)
  {
    return FALSE;
  }

  wResID = WMSAPPMN_GetWMSCMDResID(pMe,cmd);

  if (wResID == 0)
  {
    WMSAPPU_FREE(psContents);
    WMSAPPU_FREE(psBuffer);
    return FALSE;
  }

  (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                        WMSAPP_RES_FILE,
                        wResID,
                        psBuffer,
                        TITLE_BUFFER*sizeof(AECHAR));


  (void)WSTRLCAT(psContents, psBuffer, TITLE_BUFFER);
  (void)STRTOWSTR("\n", psBuffer, sizeof(psBuffer));
  (void)WSTRLCAT(psContents, psBuffer, TITLE_BUFFER);

  wResID = WMSAPPMN_GetWMSCMDErrResID(pMe,cmd_err);

  if (wResID == 0)
  {
    WMSAPPU_FREE(psContents);
    WMSAPPU_FREE(psBuffer);
    return FALSE;
  }

  (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                        WMSAPP_RES_FILE,
                        wResID,
                        psBuffer,
                        TITLE_BUFFER*sizeof(AECHAR));

  (void)WSTRLCAT(psContents, psBuffer, TITLE_BUFFER);

  if(pMe->a.m_pIShell == NULL)
  {
    WMSAPPU_FREE(psContents);
    WMSAPPU_FREE(psBuffer);
    return FALSE;
  }

  retVal = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE, IDD_MESSAGE, NULL);

  if(retVal != EFAILED)
  {
    /* Reset idle Timer, since we are successful in creating the message display */
    /* (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);*/
    retVal = (uint16)ISHELL_SetTimer(pMe->a.m_pIShell,
                             (int)SELECTION_TIME_IN_SECONDS*1000,
                             (PFNNOTIFY) WMSAPPMN_EndDlg,
                             (uint32*) pMe);


    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    pIStatic = (IStatic *) IDIALOG_GetControl(pIDialog, IDC_STATIC_MSG);

    (void)ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
    retVal = ISTATIC_SetText(pIStatic, NULL, psContents, AEE_FONT_BOLD, AEE_FONT_BOLD);
    (void)ISTATIC_Redraw(pIStatic);
  }

  WMSAPPU_FREE(psContents);
  WMSAPPU_FREE(psBuffer);

  if(retVal == SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION WMSAPPMN_InitDestAddr

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean InitDestAddr(CWMSAPP *pMe, uint32 dw)
{
  ITextCtl* pITextCtl;
  IDialog* pIDialog;
  uint16 wTextCtlID;
  uint16 wSKCtlID;
  IMenuCtl* pISoftKeyMenu;

  pIDialog = (IDialog*)dw;

  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
  
  wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  if(wTextCtlID == 0)
    return FALSE;

  pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);

  if(!pITextCtl)
  {
    return FALSE;
  }

  wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);

  if(wSKCtlID == 0)
    return FALSE;

  pISoftKeyMenu = (IMenuCtl *)IDIALOG_GetControl(pIDialog, (int16)wSKCtlID);
  
  if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
  {
    if(!IMENUCTL_DeleteAll(pISoftKeyMenu))
    {
      return FALSE;
    }
  }
  else
  {
    if(pMe->m_bTestMode)
    {
      if(!WMSAPPMN_AddSoftkeyMenu(pMe,pIDialog,wSKCtlID))
      {
        return FALSE;
      }
      if(!IMENUCTL_DeleteItem(pISoftKeyMenu,IDL_SK_DESTADDR_PHONE_NUM))
      {
        return FALSE;
      }
      if(!IMENUCTL_DeleteItem(pISoftKeyMenu,IDL_SK_DESTADDR_EMAIL))
      {
        return FALSE;
      }
    }
  }

  switch(pMe->m_addrType)
  {
    case WMSAPP_MSG_TYPE_PHONE_NUM:
      (void)ITEXTCTL_SetInputMode(pITextCtl, OEM_TM_DIALED_DIGITS);
      break;
    case WMSAPP_MSG_TYPE_EMAIL:
    case WMSAPP_MSG_TYPE_NON_STD_EMAIL:
    	#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
      	(void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_RAPID);
      	#else
      	(void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_LETTERS);
      	#endif
      break;

    default:
      break;
  }

  if(pMe->m_modeControl == WMSAPP_MODE_GW)
  {
    if (pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM)
    {
      /* Setting Max Input size for Text Control to WMS_GW_ADDRESS_MAX digits */
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_GW_ADDRESS_MAX);
    }
    else if (pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_EMAIL)
    {
      /* Setting Max Input size for Text Control to WMS_ADDRESS_MAX digits */
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_ADDRESS_MAX);
    }
    else
    {
      MSG_ERROR("Invalid Address Format for GW Mode", 0, 0, 0);
    }
  }
  else
  {
    if ((pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_PHONE_NUM)
        ||(pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_EMAIL))
    {
      /* Setting Max Input size for Text Control to WMS_ADDRESS_MAX digits */
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_ADDRESS_MAX);
    }
    else if (pMe->m_addrType == (uint16)WMSAPP_MSG_TYPE_NON_STD_EMAIL)
    {
      /* Setting Max Input size for Text Control to WMS_MAX_LEN digits */
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_MAX_LEN);
    }
  }
  (void)ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
  ITEXTCTL_SetActive(pITextCtl, TRUE);
  (void)ITEXTCTL_Redraw(pITextCtl);
  return TRUE;
}


/*===========================================================================
FUNCTION WMSAPPMN_InitTextSoftKey

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitTextSoftKey(CWMSAPP *pMe, AEETextInputMode wMode)
{
  IDialog*  pIDialog;
  ITextCtl* pITextCtl;
  uint16 wTextCtlID;
  uint16 wSKCtlID;
  IMenuCtl* pISoftKeyMenu;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
  
  if(pIDialog == NULL)
  { /* error */

    return FALSE;
  }

  //WMSAPPMD_SetScreen(pMe);

  wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  if(wTextCtlID == 0)
    return FALSE;

  pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);

  if(!pITextCtl)
  {
    return FALSE;
  }

  (void)ITEXTCTL_SetText(pITextCtl, NULL, 0);

  if(IDIALOG_GetID(pIDialog) == IDD_VOICEMAIL_NUMBER)
  {
    AECHAR *vm;
    if(pMe->m_bTestMode)
    {
      vm = MALLOC(WMS_MAX_LEN);

      if (vm == NULL) {
        return FALSE;
      }

#ifdef FEATURE_GWSMS
#error code not present
#endif      
      {
        if (pMe->m_voicemailNumber->digit == NULL) {
          return FALSE;
        }

        (void)STRTOWSTR((char *)pMe->m_voicemailNumber->digit, vm, WMS_MAX_LEN * sizeof(AECHAR));
      }
      (void)ITEXTCTL_SetText(pITextCtl, vm, WSTRLEN(vm));
      WMSAPPU_FREE(vm);
    }
  }
  else if (IDIALOG_GetID(pIDialog) == IDD_TEMPLATE_ALPHA_TAG)
  {
    AECHAR *vm;
    vm = MALLOC(WMS_ALPHA_ID_MAX * sizeof(AECHAR));

    if (vm == NULL) {
      return FALSE;
    }

    if ((pMe->m_pMO_template_msg->u.gw_template.alpha_id.data == NULL)
        &&(pMe->m_pMO_template_msg->u.gw_template.alpha_id.len == 0))
    {
      /* Assign it to pMe->m_AlphaID */
      pMe->m_pMO_template_msg->u.gw_template.alpha_id = pMe->m_AlphaID;
    }

    /* To Null Terminate */
    pMe->m_pMO_template_msg->u.gw_template.alpha_id.data[pMe->m_pMO_template_msg->u.gw_template.alpha_id.len] = 0;

    if (pMe->m_pMO_template_msg->u.gw_template.alpha_id.data == NULL) {
      return FALSE;
    }

    (void)STRTOWSTR((char *)pMe->m_pMO_template_msg->u.gw_template.alpha_id.data, vm, WMS_ALPHA_ID_MAX * sizeof(AECHAR));
    (void)ITEXTCTL_SetText(pITextCtl, vm, WSTRLEN(vm));
    WMSAPPU_FREE(vm);
  }

  wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);
  pISoftKeyMenu = (IMenuCtl *)IDIALOG_GetControl(pIDialog, (int16)wSKCtlID);

  if(wSKCtlID == 0 || pISoftKeyMenu == 0)
    return FALSE;

  if(!WMSAPPMN_AddSoftkeyMenu(pMe,pIDialog,wSKCtlID))
  {
    return FALSE;
  }

  if(pMe->m_bTestMode)
  {
    if(IDIALOG_GetID(pIDialog) == IDD_VOICEMAIL_NUMBER)
    {
      /* delete the soft keys for auto */
      if(!IMENUCTL_DeleteItem(pISoftKeyMenu,IDL_SK_VOICEMAIL_AUTO_SUB_1))
      {
        return FALSE;
      }
      if(!IMENUCTL_DeleteItem(pISoftKeyMenu,IDL_SK_VOICEMAIL_AUTO_SUB_2))
      {
        return FALSE;
      }
    }
  }

  if(IDIALOG_GetID(pIDialog) == IDD_TEMPLATE_ALPHA_TAG)
  {
    (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_ALPHA_ID_MAX);
  }
  else
  {
    if(pMe->m_modeControl == WMSAPP_MODE_GW)
    {
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_GW_ADDRESS_MAX);
    }
    else
    {
      (void)ITEXTCTL_SetMaxSize(pITextCtl, WMS_ADDRESS_MAX);
    }
  }

  ITEXTCTL_SetActive(pITextCtl, FALSE);

  /* Setting Text Input Mode */
  (void)ITEXTCTL_SetInputMode(pITextCtl, wMode);

  (void)ITEXTCTL_Redraw(pITextCtl);


  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitComposeMsg

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitComposeMsg
(
CWMSAPP *pMe

)
{
  IDialog*  pIDialog;
  AEERect rect;
  AECHAR pDummyText[2] = {' ','\0'};
  ITextCtl *pIText;

  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
  IDISPLAY_Update( pMe->a.m_pIDisplay );

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
  { /* error */

    return FALSE;
  }

  pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, IDC_MSG_TEXT);

  if(pIText == NULL)
    return FALSE;

  if (pMe->m_modeControl == WMSAPP_MODE_GW)
  {
    // Set maximum text size supported by the text control object
    (void)ITEXTCTL_SetMaxSize(pIText, WMS_GW_USER_DATA_MAX);
  }
  else /* for CDMA */
  {
    // Set maximum text size supported by the text control object
    (void)ITEXTCTL_SetMaxSize(pIText, WMS_CDMA_USER_DATA_MAX);
  }

  // Setting Text to NULL
  (void)ITEXTCTL_SetText(pIText, NULL, 0);
  (void)ITEXTCTL_SetTitle(pIText, NULL, 0, pDummyText);

  // Set the text input rect just off the screen to the north
  SETAEERECT(&rect, 0, 0, pMe->m_cxScreen, 15);

  (void)ITEXTCTL_SetRect(pIText, &(rect));

  // Set properties to have the text control object have a frame and to have a multi-line text edit space.
  (void)ITEXTCTL_SetProperties(pIText,  TP_FRAME | TP_MULTILINE);

  pMe->m_dispInfo.curX = START_POSITION;
  pMe->m_dispInfo.curY = LINE_HEIGHT;
  pMe->m_dispInfo.curLine =0;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] = LINE_HEIGHT;
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curX;

#ifdef FEATURE_CDSMS
  /* No EMS options if using non-std email */
  if(pMe->m_emailAddr.isNonStdEmail)
  {
    IMenuCtl* pISoftKeyMenu;

    pISoftKeyMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,IDC_SK_COMPOSE);

    if(!IMENUCTL_DeleteAll(pISoftKeyMenu))
    {
      return FALSE;
    }
  }
  else
#endif /* FEATURE_CDSMS */
  {
    if(!WMSAPPMN_AddSoftkeyMenu(pMe,pIDialog,IDC_SK_COMPOSE))
      return FALSE;

    (void)ITEXTCTL_SetSoftKeyMenu(pIText, NULL);
  }

  // Activate the text control.
  ITEXTCTL_SetActive(pIText, TRUE);

  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits = 0;
    pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  pMe->m_curFont = AEE_FONT_NORMAL;
  pMe->m_curFlags = IDF_TEXT_TRANSPARENT;
  pMe->m_bEditMsg = TRUE;
  pMe->m_bReply = FALSE;

  WMSAPPMD_SetScreen(pMe);

  return TRUE;
}




/*===========================================================================
FUNCTION WMSAPPMN_InitMenuDlg

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitMenuDlg
(
CWMSAPP *pMe,
IDialog *pIDialog
)
{

  uint16 wCtrlID;
  IMenuCtl* pICurrentMenu;

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if(wCtrlID == 0)
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  if (pICurrentMenu == NULL) /* error checking */
  {
    MSG_ERROR("Menu Control is NULL",0,0,0);
    return FALSE;
  }

  /* remove certain items based on featurization */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
   return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitRoutesDlg

DESCRIPTION
  Initialize Routes Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitRoutesDlg
(
CWMSAPP *pMe,
IDialog *pIDialog
)
{

  uint16 wCtrlID;
  IMenuCtl* pICurrentMenu;
  uint16 title;

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  if (wCtrlID == 0 )
  {
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  if (pICurrentMenu == NULL)
  {
    return FALSE;
  }

  /* remove certain items based on featurization */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);

  switch(pMe->m_currPref)
  {
    case IDL_CLASS0:
      pMe->m_curClass = WMS_MESSAGE_CLASS_0;
      title = IDS_CLASS0;
      break;

    case IDL_CLASS1:
      pMe->m_curClass = WMS_MESSAGE_CLASS_1;
      title = IDS_CLASS1;
      break;

    case IDL_CLASS2:
      pMe->m_curClass = WMS_MESSAGE_CLASS_2;
      title = IDS_CLASS2;
      break;

    case IDL_CLASS3:
      pMe->m_curClass = WMS_MESSAGE_CLASS_3;
      title = IDS_CLASS3;
      break;

    case IDL_CLASSNONE:
    default:
      pMe->m_curClass = WMS_MESSAGE_CLASS_NONE;
      title = IDS_CLASSNONE;
      break;
  }

  (void)WMSAPPMN_GetRoutes(pMe);
  #if 0
  (void)IMENUCTL_SetTitle(pICurrentMenu,WMSAPP_RES_FILE,title,NULL);
  #else
  {
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                            WMSAPP_RES_FILE,                                
                            title,
                            WTitle,
                            sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
 }
  #endif
  return TRUE;
}

static void WMSAPPMN_GetCheckBoxItems
(
  CWMSAPP *pMe
)
{

  IMenuCtl *pICurrentMenu;
  uint16 wCtlID;

  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  uint16 wDlgID = IDIALOG_GetID(pIDialog);

  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);

  pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  switch(wDlgID)
  {
#ifdef FEATURE_CDSMS
    case IDD_CDMA_OUTBOX_PREF:
    {
      (void)WMSAPPMN_GetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_TELESRV);
      WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_TELESRV,TRUE);

      if(pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested)
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_TLACK,TRUE);
      else
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_TLACK,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_REPLY_OPTION)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_REPLY_OPTNS,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_REPLY_OPTNS,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_USER_DATA)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DCS,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DCS,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_VALID_REL)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_VALID_REL,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_VALID_REL,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_VALID_ABS)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_VALID_ABS,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_VALID_ABS,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DEFER_REL)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DEFER_REL,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DEFER_REL,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DEFER_ABS)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DEFER_ABS,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DEFER_ABS,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_PRIORITY)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_PRIORITY,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_PRIORITY,TRUE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_PRIVACY)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_PRIVACY,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_PRIVACY,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_LANGUAGE)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_LANGUAGE,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_LANGUAGE,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_CALLBACK)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_CALLBACK,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_CALLBACK,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DISPLAY_MODE,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_DISPLAY_MODE,FALSE);

      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_ALERT)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_ALERT,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_OUTBOX_ALERT,FALSE);

    }
    break;

    case IDD_CDMA_TEMPLATE_OPTIONS:
    {
      if(pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_91 &&
         pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CPT_95 &&
         pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_95 &&
         pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_CMT_95 &&
         pMe->m_pMO_template_msg->u.cdma_template.teleservice != WMS_TELESERVICE_WEMT)
      {
           pMe->m_pMO_template_msg->u.cdma_template.teleservice = WMS_TELESERVICE_CMT_91;
      }
      WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_TELESRV,TRUE);

      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEST_ADDR,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEST_ADDR,FALSE);


      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_REPLY_OPTION))
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_REPLY_OPTS,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_REPLY_OPTS,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS)
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DCS,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DCS,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_VALID_REL)
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_VALID_REL,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_VALID_REL,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
             (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_VALID_ABS))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_VALID_ABS,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_VALID_ABS,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
            (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_REL))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEFER_REL,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEFER_REL,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_ABS))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEFER_ABS,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DEFER_ABS,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY)
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_PRIORITY,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_PRIORITY,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIVACY)
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_PRIVACY,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_PRIVACY,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_LANGUAGE)
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_LANGUAGE,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_LANGUAGE,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_CALLBACK)
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_CALLBACK,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_CALLBACK,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
            (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_USER_DATA,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_USER_DATA,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DISPLAY_MODE))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DISPLAY_MODE,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_DISPLAY_MODE,FALSE);

      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_ALERT))
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_ALERT_MODE,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_ALERT_MODE,FALSE);

      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION)
      {
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_TL_ACK,TRUE);
      }
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_C_TMPLT_TL_ACK,FALSE);

    }
    break;

#ifdef FEATURE_CDSMS_BROADCAST
    case IDD_BC_PREF:
    {
      if(pMe->m_bcInfo.pref == WMS_BC_PREF_ACTIVATE_ALL)
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_ACTIVATE_ALL,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_ACTIVATE_ALL,FALSE);

      if(pMe->m_bcInfo.pref == WMS_BC_PREF_ACTIVATE_TABLE)
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_ACTIVATE_TABLE,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_ACTIVATE_TABLE,FALSE);

      if(pMe->m_bcInfo.pref == WMS_BC_PREF_DEACTIVATE)
         WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_DEACTIVATE,TRUE);
      else
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_BC_PREF_DEACTIVATE,FALSE);
    }
    break;
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /*FEATURE_CDSMS*/
#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/

#ifdef FEATURE_CDSMS
    case IDD_CDMA_TEST:
      if(pMe->m_bAutoDcDisconnect == TRUE)
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_AUTO_DISCONNECT,TRUE);
      }
      else
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_AUTO_DISCONNECT,FALSE);
      }

      if(pMe->m_bAutoDcSend == TRUE)
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_AUTO_DC_SEND,TRUE);
      }
      else
      {
        WMSAPPMN_SetCheckBoxImage(pICurrentMenu,IDL_AUTO_DC_SEND,FALSE);
      }
      break;
#endif /* FEATURE_CDSMS */

    default:
      break;
  }
}





/*=============================================================================
FUNCTION:  WMSAPPMN_GetCheckBoxImage

DESCRIPTION: Gets Check Boxes on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean WMSAPPMN_GetCheckBoxImage(IMenuCtl   *pm, uint16 listId)
{
   CtlAddItem cai;

   cai.pszResImage = WMSAPP_RES_FILE;
   (void)IMENUCTL_GetItem(pm, listId, &cai);

   if(cai.wImage ==  IDB_CHECK_YES)
   {
     return TRUE;
   }

   return FALSE;
}

static boolean WMSAPPMN_SetCheckBoxItems
(
  CWMSAPP *pMe,
  boolean isToggle
)
{
  IDialog *pIDialog;
  IMenuCtl* pIMenu;
  uint16 wItemID;
  boolean option = TRUE;

  pIDialog  = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog));

  wItemID = IMENUCTL_GetSel(pIMenu);

  /* reset current preference being modified */
  pMe->m_currPref = WMSAPP_NO_PREF;

  switch(wItemID)
  {
#ifdef FEATURE_CDSMS
    case IDL_OUTBOX_TELESRV:
      option = TRUE;
      break;

    case IDL_OUTBOX_TLACK:
      if(pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested)
      {
        pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
        option = FALSE;
      }
      else
      {
        pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = TRUE;
      }
      break;

    case IDL_OUTBOX_REPLY_OPTNS:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_REPLY_OPTION)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_REPLY_OPTION;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_REPLY_OPTION;
        pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested = FALSE;
        pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested = FALSE;
      }
      break;

    case IDL_OUTBOX_DCS:
      /* DCS is dependant on the user data, if there is user data than
      ** toggling should not do anything
      */
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_USER_DATA)
      {
         option = TRUE;
      }
      else
      {
         option = FALSE;
      }
      break;

    case IDL_OUTBOX_VALID_REL:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_VALID_REL )
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_VALID_REL;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_VALID_REL;
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_relative, 0,
               sizeof(wms_timestamp_s_type));
      }
      break;

    case IDL_OUTBOX_VALID_ABS:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_VALID_ABS)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_VALID_ABS;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_VALID_ABS;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.day    = 0;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.hour   = 0;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.minute = 0;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.month  = 1;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.second = 0;
        pMe->m_pClient_ts_mo->u.cdma.validity_absolute.year   = 0x04;
      }
      break;

    case IDL_OUTBOX_DEFER_REL:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DEFER_REL)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_DEFER_REL;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_DEFER_REL;
        (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_relative, 0,
               sizeof(wms_timestamp_s_type));
      }
      break;

    case IDL_OUTBOX_DEFER_ABS:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DEFER_ABS)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_DEFER_ABS;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_DEFER_ABS;
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_VALID_ABS;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.day    = 0;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.hour   = 0;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.minute = 0;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.month  = 1;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.second = 0;
        pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.year   = 0x04;
      }
      break;

    case IDL_OUTBOX_PRIORITY:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_PRIORITY)
      {
        if(isToggle)
        {
          option= FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_PRIORITY;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_PRIORITY;
        pMe->m_pClient_ts_mo->u.cdma.priority = WMS_PRIORITY_NORMAL;
      }
      break;

    case IDL_OUTBOX_PRIVACY:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_PRIVACY)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_PRIVACY;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_PRIVACY;
        pMe->m_pClient_ts_mo->u.cdma.privacy = WMS_PRIVACY_NORMAL;
      }
      break;

    case IDL_OUTBOX_LANGUAGE:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_LANGUAGE)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_LANGUAGE;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_LANGUAGE;
        pMe->m_pClient_ts_mo->u.cdma.language = WMS_LANGUAGE_UNSPECIFIED;
      }
      break;

    case IDL_OUTBOX_CALLBACK:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_CALLBACK)
      {
        if(isToggle)
        {
          option= FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_CALLBACK;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_CALLBACK;
        pMe->m_pClient_ts_mo->u.cdma.callback.digit_mode = WMS_DIGIT_MODE_4_BIT;
        pMe->m_pClient_ts_mo->u.cdma.callback.number_plan =
          WMS_NUMBER_PLAN_TELEPHONY;
        pMe->m_pClient_ts_mo->u.cdma.callback.number_type = WMS_NUMBER_UNKNOWN;
        pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits = 0;
      }
      break;

    case IDL_OUTBOX_DISPLAY_MODE:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_DISPLAY_MODE;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_DISPLAY_MODE;
        pMe->m_pClient_ts_mo->u.cdma.display_mode = WMS_DISPLAY_MODE_DEFAULT;
      }
      break;

    case IDL_OUTBOX_ALERT:
      if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_ALERT)
      {
        if(isToggle)
        {
          option= FALSE;
          pMe->m_pClient_ts_mo->u.cdma.mask &= ~WMS_MASK_BD_ALERT;
        }
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_ALERT;
        pMe->m_pClient_ts_mo->u.cdma.alert_mode = WMS_ALERT_MODE_DEFAULT;
      }
      break;

    case IDL_C_TMPLT_TELESRV:
      pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
      break;

    case IDL_C_TMPLT_DEST_ADDR:
      if (pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_DEST_ADDR;
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_DEST_ADDR;

        /* Temporary...this should actually invoke the user to enter an address */
        pMe->m_pMO_template_msg->u.cdma_template.address.digit_mode =
          WMS_DIGIT_MODE_4_BIT;
        pMe->m_pMO_template_msg->u.cdma_template.address.number_plan =
          WMS_NUMBER_PLAN_TELEPHONY;
        pMe->m_pMO_template_msg->u.cdma_template.address.number_type =
          WMS_NUMBER_UNKNOWN;
        pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits = 0;
      }
      break;

    case IDL_C_TMPLT_REPLY_OPTS:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_REPLY_OPTION) )
      {
        if(isToggle)
        {
          option = FALSE;
          /* Synchronize the bearer data */
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_REPLY_OPTION;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_REPLY_OPTION;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested = FALSE;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested = FALSE;
      }
      break;

    case IDL_C_TMPLT_TL_ACK:
      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION)
      {
        if(isToggle)
        {
          option= FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION;
          pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION;
        pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = TRUE;
      }
      break;

    case IDL_C_TMPLT_DCS:
      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_DCS;
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_DCS;
        pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_ASCII;
      }
      break;

    case IDL_C_TMPLT_VALID_REL:
      if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_VALIDITY)
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_VALIDITY;
          /* Synchronize the bearer data */
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_VALID_REL;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_VALIDITY;
        (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.relative_validity, 0,
               sizeof(wms_timestamp_s_type));

        /* Synchronize the bearer data */
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |=
          WMS_MASK_BD_VALID_REL;
        (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative,
               0, sizeof(wms_timestamp_s_type));
      }
      break;

    case IDL_C_TMPLT_VALID_ABS:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_VALID_ABS) )
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_VALID_ABS;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_VALID_ABS;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.day    = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.hour   = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.minute = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.month  = 1;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.second = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.year   = 0x04;
      }
      break;

    case IDL_C_TMPLT_DEFER_REL:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_REL))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_DEFER_REL;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_DEFER_REL;
        (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative,
               0,sizeof(wms_timestamp_s_type));
      }
      break;

    case IDL_C_TMPLT_DEFER_ABS:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DEFER_ABS))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_DEFER_ABS;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_DEFER_ABS;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.day    = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.hour   = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.minute = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.month  = 1;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.second = 0;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.year   = 0x04;
      }
      break;

    case IDL_C_TMPLT_PRIORITY:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIORITY))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_PRIORITY;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_PRIORITY;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.priority = WMS_PRIORITY_NORMAL;
      }
      break;


    case IDL_C_TMPLT_PRIVACY:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_PRIVACY) )
      {
        if(isToggle)
        {
          option=  FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_PRIVACY;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_PRIVACY;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.priority = WMS_PRIORITY_NORMAL;
      }
      break;

    case IDL_C_TMPLT_LANGUAGE:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_LANGUAGE))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_LANGUAGE;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_LANGUAGE;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.language = WMS_LANGUAGE_UNSPECIFIED;
      }
      break;

    case IDL_C_TMPLT_CALLBACK:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_CALLBACK))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_CALLBACK;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_CALLBACK;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.digit_mode =WMS_DIGIT_MODE_4_BIT;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_type = WMS_NUMBER_UNKNOWN;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.callback.number_of_digits =0;
      }
      break;

    case IDL_C_TMPLT_USER_DATA:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_USER_DATA;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_USER_DATA;

        /* Fill in default user data, check if temaplate dcs is available. */
        if(pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS)
        {
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding =
            pMe->m_pMO_template_msg->u.cdma_template.dcs;
        }
        else
        {
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding =
            WMS_ENCODING_ASCII;
        }

        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len = 0;
      }
      break;

    case IDL_C_TMPLT_ALERT_MODE:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_ALERT))
      {
        if(isToggle)
        {
          option= FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_ALERT;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_ALERT;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.alert_mode = WMS_ALERT_MODE_DEFAULT;
      }
      break;

    case IDL_C_TMPLT_DISPLAY_MODE:
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_DISPLAY_MODE))
      {
        if(isToggle)
        {
          option = FALSE;
          pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask &= ~WMS_MASK_BD_DISPLAY_MODE;
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask == 0)
          {
            pMe->m_pMO_template_msg->u.cdma_template.mask &= ~WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
          }
        }
      }
      else
      {
        pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_DISPLAY_MODE;
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.display_mode =
          WMS_DISPLAY_MODE_DEFAULT;
      }
      break;

#ifdef FEATURE_CDSMS_BROADCAST
    case IDL_BC_ENGLISH:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_ENGLISH);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_ENGLISH);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_UNSPECIFIED:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_UNSPECIFIED);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_UNSPECIFIED);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option= FALSE;
          break;
      }
      break;

    case IDL_BC_FRENCH:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_FRENCH);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_FRENCH);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_SPANISH:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_SPANISH);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_SPANISH);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option= FALSE;
          break;
      }
      break;

    case IDL_BC_JAPANESE:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_JAPANESE);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_JAPANESE);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_KOREAN:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_KOREAN);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_KOREAN);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_CHINESE:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_CHINESE);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_CHINESE);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].status)
      {
        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_HEBREW:
      if(pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].status != WMSAPP_BC_ALL_ACTIVE)
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].status = WMSAPP_BC_ALL_ACTIVE;
        WMSAPPMN_BCSetAllSvcs(pMe, (int)WMS_LANGUAGE_HEBREW);
      }
      else
      {
        pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].status = WMSAPP_BC_NONE_ACTIVE;
        WMSAPPMN_BCResetAllSvcs(pMe, (int)WMS_LANGUAGE_HEBREW);
      }
      switch(pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].status)
      {

        case WMSAPP_BC_NONE_ACTIVE:
        default:
          option = FALSE;
          break;
      }
      break;

    case IDL_BC_SRV_0:
    case IDL_BC_SRV_1:
    case IDL_BC_SRV_2:
    case IDL_BC_SRV_3:
    case IDL_BC_SRV_4:
    case IDL_BC_SRV_5:
    case IDL_BC_SRV_6:
    case IDL_BC_SRV_7:
    case IDL_BC_SRV_8:
    case IDL_BC_SRV_9:
    case IDL_BC_SRV_10:
    case IDL_BC_SRV_11:
    case IDL_BC_SRV_12:
    case IDL_BC_SRV_13:
    case IDL_BC_SRV_14:
    case IDL_BC_SRV_15:
    case IDL_BC_SRV_16:
    case IDL_BC_SRV_17:
    case IDL_BC_SRV_18:
    case IDL_BC_SRV_19:
    case IDL_BC_SRV_20:
    case IDL_BC_SRV_21:
    case IDL_BC_SRV_22:
    case IDL_BC_SRV_23:
    case IDL_BC_SRV_24:
    case IDL_BC_SRV_25:
    case IDL_BC_SRV_26:
    case IDL_BC_SRV_27:
    case IDL_BC_SRV_28:
    case IDL_BC_SRV_29:
    case IDL_BC_SRV_30:
    case IDL_BC_SRV_31:
      if(WMSAPPMN_GetCheckBoxImage(pIMenu,wItemID))
      {
        option = FALSE;
      }
      break;

#endif /* FEATURE_CDSMS_BROADCAST */

    case IDL_AUTO_DISCONNECT:
      if(pMe->m_bAutoDcDisconnect == TRUE)
      {
        pMe->m_bAutoDcDisconnect = FALSE;
        (void)WMSAPPMN_DisableAutoDisconnect(pMe);
        option = FALSE;
      }
      else
      {
        pMe->m_bAutoDcDisconnect = TRUE;
        (void)WMSAPPMN_EnableAutoDisconnect(pMe, WMSAPP_AUTO_DISCONNECT_TIME);
      }
      break;

    case IDL_AUTO_DC_SEND:
      if(pMe->m_bAutoDcSend == TRUE)
      {
        pMe->m_bAutoDcSend = FALSE;
        option = FALSE;
      }
      else
      {
        pMe->m_bAutoDcSend = TRUE;
      }
      break;

    case IDL_ACCESS_CHANNEL_MSG:
      if(pMe->m_bAccessChannelTest == TRUE)
      {
        pMe->m_bAccessChannelTest = FALSE;
        option = FALSE;
      }
      else
      {
        pMe->m_bAccessChannelTest = TRUE;
      }
      break;

#endif /*FEATURE_CDSMS*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

    default:
     break;
  }

  /* Set the current preference being modified */
  pMe->m_currPref = wItemID;
  WMSAPPMN_SetCheckBoxImage(pIMenu,wItemID,option);

  return TRUE;

}

static int WMSAPPMN_ReadMsg
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                      (void*)&(pMe->m_cbData));

  return IWMS_MsgRead(pMe->m_pwms,pMe->m_clientId, &pMe->m_callback, (void*)pMe,
               pMe->m_msgInfo.memStore, pMe->m_msgInfo.smsIndex);
}

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

static boolean WMSAPPMN_MapNReadMsg
(
  CWMSAPP *pMe,
  uint16 wItemID
)
{
  int curItem = WMSAPPMN_GetListIndex(pMe, wItemID);
  wms_message_index_type msg_index;
  uint8 index =0;

  if(curItem == 0) return FALSE;

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

  if ( WMS_MEMORY_STORE_MAX <= pMe->m_msgInfo.memStore ) 
  {
     return FALSE;
  }
  if(pMe->m_bTestMode && pMe->m_pSmsLists[pMe->m_msgInfo.memStore]->len == 0)
  {
    return FALSE;
  }

  msg_index = WMSAPPU_MapIndex(pMe, pMe->m_msgInfo.memStore, curItem);
  if (msg_index != WMS_DUMMY_MESSAGE_INDEX)
  {
    index = (uint8)msg_index;
    pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pSmsLists[pMe->m_msgInfo.memStore]->indices[index];
    return WMSAPPMN_ReadMsg(pMe);
  }
  return TRUE;

}

static boolean WMSAPPMN_ViewMsg
(
  CWMSAPP *pMe,
  wms_msg_event_info_s_type *pInfo
)
{
  if(pMe->m_bTestMode)
  {
    pMe->m_msgInfo.memStore = pInfo->status_info.message.msg_hdr.mem_store;

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/

    (void)MEMCPY(pMe->m_pClient_msg, &pInfo->status_info.message, sizeof(wms_client_message_s_type));

    return (WMSAPPMN_InitMsgInfoDlg(pMe));
  }

  return FALSE;
}


static boolean WMSAPPMN_ViewTemplate
(
  CWMSAPP *pMe,
  wms_msg_event_info_s_type *pInfo
)
{


  (void)MEMCPY(pMe->m_pMO_template_msg, &pInfo->status_info.message, sizeof(wms_client_message_s_type));

  pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pMO_template_msg->msg_hdr.index;

  if (ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_VIEW_TMPLT, NULL) != SUCCESS)
    return FALSE;

  return TRUE;
}

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION WMSAPPMN_InitBCConfig

DESCRIPTION
  Initialize the BC Config dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitBCConfig
(
CWMSAPP *pMe
)
{
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  AECHAR *bcConfig = MALLOC(sizeof(AECHAR)*50);
  AECHAR *title = MALLOC(sizeof(AECHAR)*50);
  IMenuCtl *pISKMenu;
  IStatic *pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, IDC_STATIC_CONFIG);
  pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_SK_BC_CONFIG);

  if (bcConfig == NULL || title == NULL) {
    return FALSE;
  }

  switch(pMe->m_bcInfo.config)
  {
    case WMS_BC_CONFIG_ALLOW_TABLE:
      (void)STRTOWSTR("Allow Table Only", bcConfig, sizeof("Allow Table Only")*sizeof(AECHAR));
      break;

    case WMS_BC_CONFIG_ALLOW_ALL:
      (void)STRTOWSTR("Allow all Services", bcConfig, sizeof("Allow all Services")*sizeof(AECHAR));
      break;

    case WMS_BC_CONFIG_DISALLOW:
    default:
      (void)STRTOWSTR("Disallow All Services", bcConfig, sizeof("Disallow All Service") *sizeof(AECHAR));
      break;
  }
  (void)STRTOWSTR("Current BC SMS Configuration", title, sizeof("Current BC SMS Configuration")*sizeof(AECHAR));
  /* Set the proper text in the static text control */
  (void)ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_CENTERTITLE | ST_MIDDLETEXT | ST_UNDERLINE);
  (void)ISTATIC_SetText(pIStatic, title, bcConfig, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  WMSAPPU_FREE(title);
  WMSAPPU_FREE(bcConfig);

  if(pISKMenu)
  {
     (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
  }
  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_BC_CONFIG);

  if (pISKMenu)
  {
    IMENUCTL_SetFocus(pISKMenu, IDL_BC_CONFIG_1);
    (void)IMENUCTL_Redraw(pISKMenu);
  }

  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitBCPref

DESCRIPTION
  Initialize the BC Pref dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitBCPref
(
CWMSAPP *pMe
)
{
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_MENU_BC_PREF);
  IMenuCtl *pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_SK_BC_PREF);

  if(pIMenu == NULL || pISKMenu == NULL)
  {
    return FALSE;
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);
  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISKMenu);

  /* Set the current selection of BC pref */
  WMSAPPMN_GetCheckBoxItems(pMe);

  /* Set the focus on the SK Menu and Pref SK */
  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_BC_PREF);
  IMENUCTL_SetFocus(pISKMenu, IDL_BC_PREF_2);
  (void)IMENUCTL_Redraw(pISKMenu);

  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitBCTableOptions

DESCRIPTION
  Initialize the BC Table Options dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitBCTableOptions
(
  CWMSAPP *pMe
)
{
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_MENU_BC_TABLE_OPTIONS);
  IMenuCtl *pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_SK_BC_TABLE_OPTIONS);

  if(pIMenu == NULL || pISKMenu == NULL)
  {
    return FALSE;
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);
  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISKMenu);

  /* Set the focus on the SK Menu and Table SK */
  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_BC_TABLE_OPTIONS);
  IMENUCTL_SetFocus(pISKMenu, IDL_BC_TABLE_3);
  (void)IMENUCTL_Redraw(pISKMenu);

  return TRUE;
}

void WMSAPPMN_BCSetAllSvcs
(
  CWMSAPP *pMe,
  int     language
)
{
  int i=0;

  if(language == WMSAPP_BC_ALL_LANGUAGES)
  {
    /* Change selected to TRUE for all services */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      pMe->m_bcInfo.entries[i].selected = TRUE;
    }
  }
  else
  {
    /* Traverse the entries and modify selected for a particular language. */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == (wms_language_e_type)language)
      {
        pMe->m_bcInfo.entries[i].selected = TRUE;
      }
    }
  }
}

void WMSAPPMN_BCResetAllSvcs
(
  CWMSAPP *pMe,
  int     language
)
{
  int i=0;

  if(language == WMSAPP_BC_ALL_LANGUAGES)
  {
    /* Change selected to TRUE for all services */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      pMe->m_bcInfo.entries[i].selected = FALSE;
    }
  }
  else
  {
    /* Traverse the entries and modify selected for a particular language. */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == (wms_language_e_type)language)
      {
        pMe->m_bcInfo.entries[i].selected = FALSE;
      }
    }
  }
}

void WMSAPPMN_BCSetAllSvcPriority
(
  CWMSAPP *pMe,
  int     language,
  wms_priority_e_type priority
)
{
  int i=0;

  if(language == WMSAPP_BC_ALL_LANGUAGES)
  {
    /* Change selected to TRUE for all services */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      pMe->m_bcInfo.entries[i].priority = priority;
    }
  }
  else
  {
    /* Traverse the entries and modify selected for a particular language. */
    for(i=0; i<pMe->m_bcInfo.tableSize; i++)
    {
      if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == (wms_language_e_type)language)
      {
        pMe->m_bcInfo.entries[i].priority = priority;
      }
    }
  }
}

static uint16 WMSAPPMN_GetBCEditResID
(
  uint8 index
)
{
  uint16 languageItem[] =
    { IDL_BC_UNSPECIFIED, IDL_BC_ENGLISH, IDL_BC_FRENCH, IDL_BC_SPANISH,
      IDL_BC_JAPANESE, IDL_BC_KOREAN, IDL_BC_CHINESE, IDL_BC_HEBREW };

  if(index < WMSAPP_MAX_LANGUAGES)
  {
    return(languageItem[index]);
  }

  return 0;
}

static uint8 WMSAPPMN_GetBCLanguageIndex
(
  uint16 language
)
{
  uint8 i=0;
  uint16 languageItems[] =
    { IDL_BC_UNSPECIFIED, IDL_BC_ENGLISH, IDL_BC_FRENCH, IDL_BC_SPANISH,
      IDL_BC_JAPANESE, IDL_BC_KOREAN, IDL_BC_CHINESE, IDL_BC_HEBREW };

  for(i=0;i<WMSAPP_MAX_LANGUAGES;i++)
  {
    if(language == languageItems[i])
    {
      return i;
    }
  }
  return 0;
}

static uint16 WMSAPPMN_GetBCEditStringID
(
  uint8 index
)
{
  uint16 langString[] =
    { IDS_UNSPECIFIED, IDS_ENGLISH, IDS_FRENCH, IDS_SPANISH, IDS_JAPANESE,
      IDS_KOREAN, IDS_CHINESE, IDS_HEBREW };

  if(index < WMSAPP_MAX_LANGUAGES)
  {
    return(langString[index]);
  }

  MSG_HIGH("Language String not found!",0,0,0);
  return 0;
}

boolean WMSAPPMN_InitBCEdit
(
  CWMSAPP *pMe
)
{
  uint8   i;
  uint16  nItemID;
  uint16  wResID;
  CtlAddItem pai;
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_MENU_BC_EDIT_TABLE);

  /* Delete all the menu items */
  (void)IMENUCTL_DeleteAll(pIMenu);

  /* Set the correct languages */
  for(i=0; i<WMSAPP_MAX_LANGUAGES; i++)
  {
    if(pMe->m_bcSortedSvcs[i].size > 0)
    {
      if(i>=IMENUCTL_GetItemCount(pIMenu))
      {
        /* Add the item to the menu */
        nItemID = WMSAPPMN_GetBCEditResID(i);
        wResID = WMSAPPMN_GetBCEditStringID(i);

        /* Use the i as the language indicator */
        (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE, wResID, nItemID, NULL, (uint32)i);
        if(IMENUCTL_GetItem(pIMenu, nItemID, &pai))
        {
          pai.pszResImage = pai.pszResText = WMSAPP_RES_FILE;
          pai.pImage = NULL;
          switch(pMe->m_bcSortedSvcs[i].status)
          {
            case WMSAPP_BC_ALL_ACTIVE:
              pai.wImage = IDB_CHECK_YES;
              break;

            case WMSAPP_BC_SOME_ACTIVE:
              pai.wImage = IDB_CHECK_YES;
              break;

            case WMSAPP_BC_NONE_ACTIVE:
            default:
              pai.wImage = IDB_CHECK_NO;
              break;
          }
          (void)IMENUCTL_SetItem(pIMenu, nItemID, MSIF_IMAGE, &pai);
        }
      }
    }
  }
  (void)IMENUCTL_Redraw(pIMenu);
  return TRUE;
}

#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
FUNCTION WMSAPPMN_UPDATERADIOBUTTONIMAGES

DESCRIPTION
  Update the radio button images to reflect the newly selected item

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_UpdateRadioButtonImages(CWMSAPP *pMe, IMenuCtl* pIMenu, uint16 wSelectedItemID)
{

  /* uncheck previously checked item */
  InitMenuIcons(pIMenu);

  /* check newly selected item */
  SetMenuIcon(pIMenu,wSelectedItemID,TRUE);

}
/*===========================================================================
FUNCTION WMSAPPMN_SaveSetting

DESCRIPTION
  Convert resource ID to setting through various SetNVXXXSetting or
  SetXXXSetting functions

DEPENDENCIES
  Various SetNVXXXSetting and SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMN_SaveSetting(CWMSAPP *pMe, uint16 wSelectItemID)
{
  IDialog *pIDialog;
  IMenuCtl *pIMenu;
  uint32   data;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pIMenu   = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog));
  if(!(IMENUCTL_GetItemData(pIMenu, wSelectItemID, &data)))
  {
    MSG_HIGH("Item Data is not valid!",0,0,0);
  }

  /* Appropriate masks should already be set for particular messages, so just
  ** assign the selected value.
  */

  switch(wSelectItemID)
  {

#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
    case IDL_ROUTE_DISCARD:
      if (WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass ) {
         return FALSE;
      }
      pMe->m_routes.pp_routes[pMe->m_curClass].route = WMS_ROUTE_DISCARD;
      pMe->m_routes.pp_routes[pMe->m_curClass].mem_store = WMS_MEMORY_STORE_NONE;
      if (WMSAPPMN_SetRoutes(pMe))
        return TRUE;
      return FALSE;

    case IDL_ROUTE_STORE:
    
      if (WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass ) {
         return FALSE;
      }
      pMe->m_routes.pp_routes[pMe->m_curClass].route = WMS_ROUTE_STORE_AND_NOTIFY;
      if (pMe->m_routes.pp_routes[pMe->m_curClass].mem_store == WMS_MEMORY_STORE_NONE)
        pMe->m_routes.pp_routes[pMe->m_curClass].mem_store = WMS_MEMORY_STORE_NV_GW;
      if (WMSAPPMN_SetRoutes(pMe))
        return TRUE;
      return FALSE;

    case IDL_ROUTE_TRANSFER_ONLY:
      
      if (WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass ) {
         return FALSE;
      }
      pMe->m_routes.pp_routes[pMe->m_curClass].route = WMS_ROUTE_TRANSFER_ONLY;
      pMe->m_routes.pp_routes[pMe->m_curClass].mem_store = WMS_MEMORY_STORE_NONE;
      if (WMSAPPMN_SetRoutes(pMe))
        return TRUE;
      return FALSE;

    case IDL_ROUTE_TRANSFER_AND_ACK:
     
      if (WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass ) {
         return FALSE;
      }
      pMe->m_routes.pp_routes[pMe->m_curClass].route = WMS_ROUTE_TRANSFER_AND_ACK;
      pMe->m_routes.pp_routes[pMe->m_curClass].mem_store = WMS_MEMORY_STORE_NONE;
      if (WMSAPPMN_SetRoutes(pMe))
        return TRUE;
      return FALSE;

    case IDL_ROUTE_NONE:
      if (WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass ) {
         return FALSE;
      }
      pMe->m_routes.pp_routes[pMe->m_curClass].route = WMS_ROUTE_NO_CHANGE;
      pMe->m_routes.pp_routes[pMe->m_curClass].mem_store = WMS_MEMORY_STORE_NONE;
      if (WMSAPPMN_SetRoutes(pMe))
        return TRUE;
      return FALSE;

    case IDL_MEM_STORE_RAM:
      if (pMe->m_modeControl == WMSAPP_MODE_GW)
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

      }
      return(WMSAPPMN_StoreMessage(pMe));

    case IDL_MEM_STORE_CARD:
      if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS_RUIM
        /* Only save mem store if we are not in settings */
        if(pMe->m_msgInfo.boxType != WMSAPP_NONE)
        {
          if (pMe->m_msgInfo.boxType == WMSAPP_INBOX)
          {
            /* Inbox */
            pMe->m_pClient_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
          }
          else
          {
            /* Outbox */
            pMe->m_pClient_msg_mo->msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
          }
          /*pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_RUIM;*/
        }
        else
        {
          pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = WMS_MEMORY_STORE_RUIM;
          pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_STORE_AND_NOTIFY;
          if (WMSAPPMN_SetRoutes(pMe))
            return TRUE;
          return FALSE;
        }
#else
        return TRUE;
#endif /*FEATURE_CDSMS_RUIM*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
      }
      return WMSAPPMN_StoreMessage(pMe);

    case IDL_MEM_STORE_NV:
      if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        //pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA;

        /* Only encode if we are not in settings */
        if(pMe->m_msgInfo.boxType != WMSAPP_NONE)
        {
          if (pMe->m_msgInfo.boxType == WMSAPP_INBOX)
          {
            /* Inbox */
            pMe->m_pClient_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
          }
          else
          {
            /* Outbox */
            pMe->m_pClient_msg_mo->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
          }
          /* pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA; */
        }
        else
        {
          pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = WMS_MEMORY_STORE_NV_CDMA;
          pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_STORE_AND_NOTIFY;
          if (WMSAPPMN_SetRoutes(pMe))
            return TRUE;
          return FALSE;
        }
#endif /*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
      }
      return(WMSAPPMN_StoreMessage(pMe));


#ifdef FEATURE_CDSMS
    /* Teleservice */
    case IDL_TLSRV_CMT91:
    case IDL_TLSRV_CPT:
    case IDL_TLSRV_CMT95:
    case IDL_TLSRV_VMN:
    case IDL_TLSRV_WEMT:
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.teleservice = (wms_teleservice_e_type)data;
      }
      else
      {
        pMe->m_pClient_msg_mo->u.cdma_message.teleservice = (wms_teleservice_e_type)data;
      }
      break;

    /* Encoding */
    case IDL_DCS_ASCII:
    case IDL_DCS_OCTET:
    case IDL_DCS_IA5:
    case IDL_DCS_UNICODE:
    case IDL_DCS_GSM_7_BIT:
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        if(WMSAPPMN_ConvertCDMADcs(pMe, (wms_user_data_encoding_e_type)data))
        {
          pMe->m_pMO_template_msg->u.cdma_template.dcs = (wms_user_data_encoding_e_type)data;
        }
      }
      else
      {
        if(WMSAPPMN_ConvertCDMADcs(pMe, (wms_user_data_encoding_e_type)data))
        {
          pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = (wms_user_data_encoding_e_type)data;
        }
      }
      break;

    /* Priority */
    case IDL_PRTY_NORMAL:
    case IDL_PRTY_INTERACTIVE:
    case IDL_PRTY_URGENT:
    case IDL_PRTY_EMERGENCY:
#ifdef FEATURE_CDSMS_BROADCAST
      if((pMe->m_currPref == WMSAPP_NO_PREF) &&
         (pMe->m_bcCurrIndex == WMSAPP_BC_NO_INDEX))
      {
        pMe->m_bcNewSvc.priority = (wms_priority_e_type)data;
      }
      else
#endif /* FEATURE_CDSMS_BROADCAST */

      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.priority = (wms_priority_e_type)data;
      }
      else if(pMe->m_msgInfo.boxType == WMSAPP_INBOX || pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        pMe->m_pClient_ts_mo->u.cdma.priority = (wms_priority_e_type)data;
      }
#ifdef FEATURE_CDSMS_BROADCAST
      else if(pMe->m_svcPriority == WMSAPP_BC_SINGLE_SVC)
      {
        WMSAPPMN_BCSetPriority(pMe, (wms_priority_e_type)data);
      }
      else
      {
        WMSAPPMN_BCSetPriorityForAll(pMe, (wms_priority_e_type)data);
      }
#endif /* FEATURE_CDSMS_BROADCAST */
      break;

    /* Privacy */
    case IDL_PRV_NORMAL:
    case IDL_PRV_RESTRICT:
    case IDL_PRV_CONFIDENTIAL:
    case IDL_PRV_SECRET:
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.privacy = (wms_privacy_e_type)data;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.privacy = (wms_privacy_e_type)data;
      }
      break;

    /* Language */
    case IDL_LANG_ENGLISH:
    case IDL_LANG_FRENCH:
    case IDL_LANG_SPANISH:
    case IDL_LANG_JAPANESE:
    case IDL_LANG_KOREAN:
    case IDL_LANG_CHINESE:
    case IDL_LANG_HEBREW:
#ifdef FEATURE_CDSMS_BROADCAST
      if((pMe->m_currPref == WMSAPP_NO_PREF) &&
         (pMe->m_bcCurrIndex == WMSAPP_BC_NO_INDEX))
      {
        pMe->m_bcNewSvc.srv_id.bc_srv_id.language = (wms_language_e_type)data;
      }
      else
#endif /* FEATURE_CDSMS_BROADCAST */
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.language = (wms_language_e_type)data;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.language = (wms_language_e_type)data;
      }
      break;
#endif /* FEATURE_CDSMS */

    /* Absolute validity */
    case IDL_VAL_YEAR_ONE:
    case IDL_VAL_YEAR_TWO:
    case IDL_VAL_YEAR_THREE:
    case IDL_VAL_YEAR_FOUR:
    case IDL_VAL_YEAR_FIVE:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.year = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.year = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
          {
            pMe->m_pClient_ts_mo->u.cdma.validity_absolute.year = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.year = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_VAL_MONTH_JANUARY:
    case IDL_VAL_MONTH_FEBRUARY:
    case IDL_VAL_MONTH_MARCH:
    case IDL_VAL_MONTH_APRIL:
    case IDL_VAL_MONTH_MAY:
    case IDL_VAL_MONTH_JUNE:
    case IDL_VAL_MONTH_JULY:
    case IDL_VAL_MONTH_AUGUST:
    case IDL_VAL_MONTH_SEPTEMBER:
    case IDL_VAL_MONTH_OCTOBER:
    case IDL_VAL_MONTH_NOVEMBER:
    case IDL_VAL_MONTH_DECEMBER:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.month = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.month = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
          {
            pMe->m_pClient_ts_mo->u.cdma.validity_absolute.month = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.month = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_VAL_DAY_ONE:
    case IDL_VAL_DAY_TWO:
    case IDL_VAL_DAY_THREE:
    case IDL_VAL_DAY_FOUR:
    case IDL_VAL_DAY_FIVE:
    case IDL_VAL_DAY_SIX:
    case IDL_VAL_DAY_SEVEN:
    case IDL_VAL_DAY_EIGHT:
    case IDL_VAL_DAY_NINE:
    case IDL_VAL_DAY_TEN:
    case IDL_VAL_DAY_ELEVEN:
    case IDL_VAL_DAY_TWELVE:
    case IDL_VAL_DAY_THIRTEEN:
    case IDL_VAL_DAY_FOURTEEN:
    case IDL_VAL_DAY_FIFTEEN:
    case IDL_VAL_DAY_SIXTEEN:
    case IDL_VAL_DAY_SEVENTEEN:
    case IDL_VAL_DAY_EIGHTEEN:
    case IDL_VAL_DAY_NINETEEN:
    case IDL_VAL_DAY_TWENTY:
    case IDL_VAL_DAY_TWENTY_ONE:
    case IDL_VAL_DAY_TWENTY_TWO:
    case IDL_VAL_DAY_TWENTY_THREE:
    case IDL_VAL_DAY_TWENTY_FOUR:
    case IDL_VAL_DAY_TWENTY_FIVE:
    case IDL_VAL_DAY_TWENTY_SIX:
    case IDL_VAL_DAY_TWENTY_SEVEN:
    case IDL_VAL_DAY_TWENTY_EIGHT:
    case IDL_VAL_DAY_TWENTY_NINE:
    case IDL_VAL_DAY_THIRTY:
    case IDL_VAL_DAY_THIRTY_ONE:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.day = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.day = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
          {
            pMe->m_pClient_ts_mo->u.cdma.validity_absolute.day = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.day = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_VAL_HOUR_ZERO:
    case IDL_VAL_HOUR_ONE:
    case IDL_VAL_HOUR_TWO:
    case IDL_VAL_HOUR_THREE:
    case IDL_VAL_HOUR_FOUR:
    case IDL_VAL_HOUR_FIVE:
    case IDL_VAL_HOUR_SIX:
    case IDL_VAL_HOUR_SEVENT:
    case IDL_VAL_HOUR_EIGHT:
    case IDL_VAL_HOUR_NINE:
    case IDL_VAL_HOUR_TEN:
    case IDL_VAL_HOUR_ELEVEN:
    case IDL_VAL_HOUR_TWELVE:
    case IDL_VAL_HOUR_THIRTEEN:
    case IDL_VAL_HOUR_FOURTEEN:
    case IDL_VAL_HOUR_FIFTEEN:
    case IDL_VAL_HOUR_SIXTEEN:
    case IDL_VAL_HOUR_SEVENTEEN:
    case IDL_VAL_HOUR_EIGHTEEN:
    case IDL_VAL_HOUR_NINETEEN:
    case IDL_VAL_HOUR_TWENTY:
    case IDL_VAL_HOUR_TWENTY_ONE:
    case IDL_VAL_HOUR_TWENTY_TWO:
    case IDL_VAL_HOUR_TWENTY_THREE:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.hour = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.hour = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
          {
            pMe->m_pClient_ts_mo->u.cdma.validity_absolute.hour = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.hour = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_VAL_MIN_TEN:
    case IDL_VAL_MIN_TWENTY:
    case IDL_VAL_MIN_THIRTY:
    case IDL_VAL_MIN_FORTY:
    case IDL_VAL_MIN_FIFTY:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute.minute = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute.minute = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
          {
            pMe->m_pClient_ts_mo->u.cdma.validity_absolute.minute = (uint8)data;
          }
          else /* deferred abs */
          {
            pMe->m_pClient_ts_mo->u.cdma.deferred_absolute.minute = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_VAL_TIMEZONE_MINUS_12:
    case IDL_VAL_TIMEZONE_MINUS_11:
    case IDL_VAL_TIMEZONE_MINUS_10:
    case IDL_VAL_TIMEZONE_MINUS_9:
    case IDL_VAL_TIMEZONE_MINUS_8:
    case IDL_VAL_TIMEZONE_MINUS_7:
    case IDL_VAL_TIMEZONE_MINUS_6:
    case IDL_VAL_TIMEZONE_MINUS_5:
    case IDL_VAL_TIMEZONE_MINUS_4:
    case IDL_VAL_TIMEZONE_MINUS_3:
    case IDL_VAL_TIMEZONE_MINUS_2:
    case IDL_VAL_TIMEZONE_MINUS_1:
    case IDL_VAL_TIMEZONE_0:
    case IDL_VAL_TIMEZONE_PLUS_1:
    case IDL_VAL_TIMEZONE_PLUS_2:
    case IDL_VAL_TIMEZONE_PLUS_3:
    case IDL_VAL_TIMEZONE_PLUS_4:
    case IDL_VAL_TIMEZONE_PLUS_5:
    case IDL_VAL_TIMEZONE_PLUS_6:
    case IDL_VAL_TIMEZONE_PLUS_7:
    case IDL_VAL_TIMEZONE_PLUS_8:
    case IDL_VAL_TIMEZONE_PLUS_9:
    case IDL_VAL_TIMEZONE_PLUS_10:
    case IDL_VAL_TIMEZONE_PLUS_11:
    case IDL_VAL_TIMEZONE_PLUS_12:
      if(pMe->m_modeControl == WMSAPP_MODE_GW)
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

#ifdef FEATURE_CDSMS

    case IDL_ALERT_MODE_DEFAULT:
    case IDL_ALERT_MODE_LOW_PRI:
    case IDL_ALERT_MODE_MED_PRI:
    case IDL_ALERT_MODE_HIGH_PRI:
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.alert_mode = (wms_alert_mode_e_type)data;
      }
      else if(pMe->m_currPref != WMSAPP_NO_PREF)
      {
        pMe->m_pClient_ts_mo->u.cdma.alert_mode = (wms_alert_mode_e_type)data;
      }
      break;

    case IDL_DISP_MODE_IMMEDIATE:
    case IDL_DISP_MODE_DEFAULT:
    case IDL_DISP_MODE_USER_INVOKE:
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.display_mode = (wms_display_mode_e_type)data;
      }
      else
      {
        pMe->m_pClient_ts_mo->u.cdma.display_mode = (wms_display_mode_e_type)data;
      }
      break;

#ifdef FEATURE_CDSMS_BROADCAST
    case IDL_BC_MAX_MSG_1:
    case IDL_BC_MAX_MSG_2:
    case IDL_BC_MAX_MSG_3:
    case IDL_BC_MAX_MSG_4:
    case IDL_BC_MAX_MSG_5:
      pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].max_messages = (uint8)data;
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_CDSMS_BROADCAST
    case IDL_BC_SVC_ID_EMERGENCY:
    case IDL_BC_SVC_ID_ADMIN:
    case IDL_BC_SVC_ID_MAINTENANCE:
    case IDL_BC_SVC_ID_NEWS_LOC:
    case IDL_BC_SVC_ID_NEWS_REG:
    case IDL_BC_SVC_ID_NEWS_NAT:
    case IDL_BC_SVC_ID_NEWS_INT:
    case IDL_BC_SVC_ID_WEATHER:
    case IDL_BC_SVC_ID_TRAFFIC:
    case IDL_BC_SVC_ID_AIRPORT_SCHED:
    case IDL_BC_SVC_ID_RESTAURANTS:
    case IDL_BC_SVC_ID_LODGING:
    case IDL_BC_SVC_ID_RETAIL:
    case IDL_BC_SVC_ID_ADS:
    case IDL_BC_SVC_ID_STOCK:
    case IDL_BC_SVC_ID_JOBS:
    case IDL_BC_SVC_ID_MEDICAL:
    case IDL_BC_SVC_ID_TECH_NEWS:
    case IDL_BC_SVC_ID_MULTI:
      pMe->m_bcNewSvc.srv_id.bc_srv_id.service = (wms_service_e_type)data;
      break;
#endif /* FEATURE_CDSMS_BROADCAST */

    case IDL_AUTO_DC_SEND_SO_6:
    case IDL_AUTO_DC_SEND_SO_14:
    case IDL_AUTO_DC_SEND_SO_AUTO:
      pMe->m_AutoDcSendSO = (wms_dc_so_e_type)data;
      break;

#endif /*FEATURE_CDSMS*/

    /* Relative Validity */
    case IDL_REL_VAL_FIVE_MINUTES:
    case IDL_REL_VAL_TEN_MINUTES:
    case IDL_REL_VAL_FIFTEEN_MINUTES:
    case IDL_REL_VAL_THIRTY_MINUTES:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.relative_validity, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.relative_validity.minute = (uint8)data;

            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative.minute = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative.minute = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.validity_relative.minute = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.deferred_relative.minute = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }

      break;

    case IDL_REL_VAL_ONE_HOUR:
    case IDL_REL_VAL_TWO_HOURS:
    case IDL_REL_VAL_THREE_HOURS:
    case IDL_REL_VAL_SIX_HOURS:
    case IDL_REL_VAL_TWELVE_HOURS:
      if(pMe->m_modeControl == WMSAPP_MODE_GW)
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      else
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.relative_validity, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.relative_validity.hour = (uint8)data;

            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative.hour = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative.hour = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.validity_relative.hour = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.deferred_relative.hour = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      break;

    case IDL_REL_VAL_ONE_DAY:
    case IDL_REL_VAL_TWO_DAYS:
    case IDL_REL_VAL_THREE_DAYS:
    case IDL_REL_VAL_ONE_WEEK:
    case IDL_REL_VAL_TWO_WEEKS:
    case IDL_REL_VAL_THREE_WEEKS:
    case IDL_REL_VAL_FOUR_WEEKS:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.relative_validity, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.relative_validity.day = (uint8)data;

            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative.day = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative.day = (uint8)data;
          }
        }
        else
        {
          if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.validity_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.validity_relative.day = (uint8)data;
          }
          else /* deferred rel */
          {
            (void)MEMSET(&pMe->m_pClient_ts_mo->u.cdma.deferred_relative, 0, sizeof(wms_timestamp_s_type));
            pMe->m_pClient_ts_mo->u.cdma.deferred_relative.day = (uint8)data;
          }
        }
#endif /* FEATURE_CDSMS */
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      }
      break;

    case IDL_MODE_CDMA:
      {
        if (pMe->m_bCDMAReady)
        {
          uint8 mode = OEMNV_SMS_MODE_CDMA;
          pMe->m_modeControl = WMSAPP_MODE_CDMA;
          (void)ICONFIG_SetItem(pMe->m_pIConfig,CFGI_SMS_MODE, &mode, 1);
        }
        else
        {
          if (!WMSAPPMN_EndDlg(pMe))
          {
            MSG_ERROR("WMSAPPMN_EndDlg Failed!",0,0,0);
          }
          if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, "Phone Not Ready for CDMA Mode", -1))
          {
            MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed!",0,0,0);
          }
        }
      }
      break;

    case IDL_MODE_GW:
      {
        if (pMe->m_bGWReady)
        {
          uint8 mode = OEMNV_SMS_MODE_GW;
          pMe->m_modeControl = WMSAPP_MODE_GW;
          (void)ICONFIG_SetItem(pMe->m_pIConfig,CFGI_SMS_MODE, &mode, 1);
        }
        else
        {
          if (!WMSAPPMN_EndDlg(pMe))
          {
            MSG_ERROR("WMSAPPMN_EndDlg Failed!",0,0,0);
          }
          if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, "Phone Not Ready for GW Mode", -1))
          {
            MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed!",0,0,0);
          }
        }
      }
      break;

    case IDL_CS_PREF:
      {
        (void)IWMS_CfgSetGWDomainPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_GW_DOMAIN_PREF_CS_PREFERRED);
      }
      break;

    case IDL_PS_PREF:
      {
        (void)IWMS_CfgSetGWDomainPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_GW_DOMAIN_PREF_PS_PREFERRED);
      }
      break;

    case IDL_CS_ONLY:
      {
        (void)IWMS_CfgSetGWDomainPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_GW_DOMAIN_PREF_CS_ONLY);
      }
      break;

    case IDL_PS_ONLY:
      {
        (void)IWMS_CfgSetGWDomainPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_GW_DOMAIN_PREF_PS_ONLY);
      }
      break;

    case IDL_NO_SEGMENT:
    case IDL_8_BIT_SEGMENT:
    case IDL_16_BIT_SEGMENT:
      pMe->m_segmentSetting = (wmsapp_segment_setting_e_type)data;
      (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_SEGMENT_SETTING, &pMe->m_segmentSetting, 1);
      break;

    case IDL_NO_RETRY:
    case IDL_RETRY_30:
    case IDL_RETRY_60:
    case IDL_RETRY_120:
    case IDL_RETRY_240:
      pMe->m_retryPeriod = (uint32)data;
      (void)IWMS_MsgSetRetryPeriod(pMe->m_pwms, pMe->m_clientId, pMe->m_retryPeriod);
      break;

    default: /* error */
      return FALSE;
  }
  return FALSE;
} /* WMSAPPMN_SaveSetting */

/*===========================================================================
FUNCTION WMSAPPMN_UPDATESETTING

DESCRIPTION
  Update menu setting static variable and NV values if the selected Item is
  NV related

DEPENDENCIES
  TranslateResourceIDToSetting, UpdateSMSRadioButtonImages

SIDE EFFECTS
  None
===========================================================================*/
static void WMSAPPMN_UpdateSetting(CWMSAPP *pMe, uint16 wSelectedItemID, IMenuCtl* pICurrentMenu)
{

  if(pICurrentMenu)
  {  /* if current menu control is avaliable */

    /* Update the radio button images */
    WMSAPPMN_UpdateRadioButtonImages(pMe, pICurrentMenu, wSelectedItemID);

    /*
    Translate the selected resource ID to NV setting, the corresponding
    SetNVXXXSetting function
    */
    (void)WMSAPPMN_SaveSetting(pMe, wSelectedItemID);


    return;
  }
} /* UpdateSetting */



/*===========================================================================
FUNCTION WMSAPPMN_GETCURRENTSETTING


  Get the current setting for the menu item.  For non-NV items, the settings will
  be translated to Resource ID immediately; for NV items, the function will
  get the information from NV first and then translate to corresponding resource ID

DEPENDENCIES
  TranslateSettingToResourceID

SIDE EFFECTS
  None
===========================================================================*/
static uint16 WMSAPPMN_GetCurrentSetting(CWMSAPP * pMe, uint16 wDlgID)
{
  uint16 wItemID=0;

  switch(wDlgID)
  {
    /***************************************************************/
    /* Non NV items: get from the static variables */
    //***************************************************************/
    case IDD_ROUTES:
      /* translate to corresponding resource ID */
 
      if ( WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass) {
            return 0;
         }
      switch(pMe->m_routes.pp_routes[pMe->m_curClass].route)
      {
        case WMS_ROUTE_DISCARD:
          wItemID = IDL_ROUTE_DISCARD;
          break;

        case WMS_ROUTE_TRANSFER_AND_ACK:
          wItemID = IDL_ROUTE_TRANSFER_AND_ACK;
          break;

        case WMS_ROUTE_TRANSFER_ONLY:
          wItemID = IDL_ROUTE_TRANSFER_ONLY;
          break;

        case WMS_ROUTE_STORE_AND_NOTIFY:
          wItemID = IDL_ROUTE_STORE;
          break;

        default:
          wItemID = 0;
          break;
      }
      return wItemID;

    case IDD_MEMORY_STORE:
      if ( WMS_MESSAGE_CLASS_MAX <= pMe->m_curClass) {
         return 0;
      }
      if (pMe->m_msgInfo.boxType == WMSAPP_NONE)
      {
        // In Settings Menu
        switch(pMe->m_routes.pp_routes[pMe->m_curClass].mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_RAM_CDMA:
            wItemID = IDL_MEM_STORE_RAM;
            break;
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_RUIM:
            wItemID = IDL_MEM_STORE_CARD;
            break;
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_NV_CDMA:
            wItemID= IDL_MEM_STORE_NV;
            break;
          case WMS_MEMORY_STORE_NONE:
            if (pMe->m_modeControl == WMSAPP_MODE_GW)
            {
              // To block Dialog for Mem_store
              // Only in case of GW since
              // user has to select "Store" from route options
              wItemID = 0;
            }
            else
            {
              // For CDMA
              wItemID = IDL_MEM_STORE_NV;
            }
            break;
          default:
            wItemID = IDL_MEM_STORE_NV;
        }
      }
      else
      {
        switch(pMe->m_msgInfo.memStore)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_RAM_CDMA:
            wItemID = IDL_MEM_STORE_RAM;
            break;
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_RUIM:
            wItemID = IDL_MEM_STORE_CARD;
            break;
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_NV_CDMA:
            wItemID= IDL_MEM_STORE_NV;
            break;
          default:
            wItemID = IDL_MEM_STORE_NV;
        }
      }
      return wItemID;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#if (defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS))
      case IDD_REL_VALIDITY:
        {
          boolean userDefinedValidity = FALSE;
          uint8 numValidityVals = 0;
          wms_timestamp_s_type relative_time;

          if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
          {
            if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
            {
              if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
              {
                pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative
                  = pMe->m_pMO_template_msg->u.cdma_template.relative_validity;
                relative_time = pMe->m_pMO_template_msg->u.cdma_template.relative_validity;
              }
              else /* Deferred Relative */
              {
                relative_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative;
              }
            }
            else /* OUTBOX */
            {
              if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
              {
                relative_time = pMe->m_pClient_ts_mo->u.cdma.validity_relative;
              }
              else /* deferred rel */
              {
                relative_time = pMe->m_pClient_ts_mo->u.cdma.deferred_relative;
              }
            }
          }
          else if (pMe->m_modeControl == WMSAPP_MODE_GW)
          {
            if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
            {
              relative_time = pMe->m_pMO_template_msg->u.gw_template.relative_validity;
            }
            else
            {
              relative_time = pMe->m_pClient_ts_mo->u.gw_pp.u.submit.validity.u.time;
            }
          }
          else {
             ZEROAT(&relative_time);
          }
          if (relative_time.year >0)
          {
            wItemID = IDL_REL_VAL_USER_DEFINED;
            userDefinedValidity = TRUE;
            numValidityVals++;
          }
          if(relative_time.month > 1)
          {
              wItemID = IDL_REL_VAL_USER_DEFINED;
              userDefinedValidity = TRUE;
              numValidityVals++;
          }

          if(!userDefinedValidity && relative_time.day == 1)
          {
              wItemID = IDL_REL_VAL_ONE_DAY;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 2)
          {
              wItemID = IDL_REL_VAL_TWO_DAYS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 3)
          {
              wItemID = IDL_REL_VAL_THREE_DAYS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 7)
          {
              wItemID = IDL_REL_VAL_ONE_WEEK;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 14)
          {
              wItemID = IDL_REL_VAL_TWO_WEEKS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 21)
          {
              wItemID = IDL_REL_VAL_THREE_WEEKS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day == 28)
          {
              wItemID = IDL_REL_VAL_FOUR_WEEKS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.day !=0)
          {
              wItemID = IDL_REL_VAL_USER_DEFINED;
              userDefinedValidity = TRUE;
              numValidityVals++;
          }

          if(!userDefinedValidity && relative_time.hour ==1)
          {
              wItemID = IDL_REL_VAL_ONE_HOUR;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.hour ==2)
          {
              wItemID = IDL_REL_VAL_TWO_HOURS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.hour ==3)
          {
              wItemID = IDL_REL_VAL_THREE_HOURS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.hour ==6)
          {
              wItemID = IDL_REL_VAL_SIX_HOURS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.hour ==12)
          {
              wItemID = IDL_REL_VAL_TWELVE_HOURS;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.hour !=0)
          {
              wItemID = IDL_REL_VAL_USER_DEFINED;
              userDefinedValidity = TRUE;
              numValidityVals++;
          }

          if(!userDefinedValidity && relative_time.minute == 5)
          {
              wItemID = IDL_REL_VAL_FIVE_MINUTES;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.minute == 10)
          {
              wItemID = IDL_REL_VAL_TEN_MINUTES;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.minute == 15)
          {
              wItemID = IDL_REL_VAL_FIFTEEN_MINUTES;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.minute == 30)
          {
              wItemID = IDL_REL_VAL_THIRTY_MINUTES;
              userDefinedValidity = FALSE;
              numValidityVals++;
          }
          else if(!userDefinedValidity && relative_time.minute !=0)
          {
              wItemID = IDL_REL_VAL_USER_DEFINED;
              userDefinedValidity = TRUE;
              numValidityVals++;
          }

          if(!userDefinedValidity && relative_time.second !=0)
          {
              wItemID = IDL_REL_VAL_USER_DEFINED;
              userDefinedValidity = TRUE;
              numValidityVals++;
          }
          if(numValidityVals > 1 )
          {
             wItemID = IDL_REL_VAL_USER_DEFINED;
          }

          if(wItemID == 0)
            wItemID = IDL_REL_VAL_FIVE_MINUTES; /*default value*/

          return wItemID;
        }

    case IDD_VALIDITY_YEAR:
    case IDD_VALIDITY_MONTH:
    case IDD_VALIDITY_DAY:
    case IDD_VALIDITY_HOUR:
    case IDD_VALIDITY_MINUTE:
    case IDD_VALIDITY_TIMEZONE:
      {
        wms_timestamp_s_type abs_time;

        ZEROAT(&abs_time);

        if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
        {
  #ifdef FEATURE_CDSMS
          if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
          {
            if(pMe->m_currPref == IDL_C_TMPLT_VALID_ABS)
            {
              abs_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_absolute;
            }
            else /* deferred abs */
            {
              abs_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_absolute;
            }
          }
          else
          {
            if(pMe->m_currPref == IDL_OUTBOX_VALID_ABS)
            {
              abs_time = pMe->m_pClient_ts_mo->u.cdma.validity_absolute;
            }
            else /* deferred abs */
            {
              abs_time = pMe->m_pClient_ts_mo->u.cdma.deferred_absolute;
            }
          }
  #endif /* FEATURE_CDSMS */
        }
        else
        {
  #ifdef FEATURE_GWSMS
#error code not present
  #endif /* FEATURE_GWSMS */
        }

        /* Year */
        if (wDlgID == IDD_VALIDITY_YEAR)
        {
          switch(abs_time.year)
          {
            case 03:
              return IDL_VAL_YEAR_ONE;

            case 04:
              return IDL_VAL_YEAR_TWO;

            case 05:
              return IDL_VAL_YEAR_THREE;

            case 06:
              return IDL_VAL_YEAR_FOUR;

            case 07:
              return IDL_VAL_YEAR_FIVE;

            default:
              return 0;
          }
        }

        /* Month */
        if (wDlgID == IDD_VALIDITY_MONTH)
        {
          return (IDL_VAL_MONTH_JANUARY - 1 + WMSAPPU_ConvertBCDToInt(abs_time.month) );
        }

        /* Day */
        if (wDlgID == IDD_VALIDITY_DAY)
        {
          /* Since ID are contiguous */
          return (IDL_VAL_DAY_ONE - 1 + WMSAPPU_ConvertBCDToInt(abs_time.day) );
        }

        /* Hour */
        if (wDlgID == IDD_VALIDITY_HOUR)
        {
          /* Since ID are contiguous */
          return (IDL_VAL_HOUR_ZERO + WMSAPPU_ConvertBCDToInt(abs_time.hour) );
        }

        /* Minute */
        if (wDlgID == IDD_VALIDITY_MINUTE)
        {
          switch(abs_time.minute)
          {
            case 10:
              return IDL_VAL_MIN_TEN;

            case 20:
              return IDL_VAL_MIN_TWENTY;

            case 30:
              return IDL_VAL_MIN_THIRTY;

            case 40:
              return IDL_VAL_MIN_FORTY;

            case 50:
              return IDL_VAL_MIN_FIFTY;

            default:
              return 0;
          }
        }

        /* Time Zone */
        if (wDlgID == IDD_VALIDITY_TIMEZONE)
        {
          /* Since ID are contiguous */
          return (IDL_VAL_TIMEZONE_0 + abs_time.timezone);
          
        }
        
      }

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    case IDD_TELESERVICE:
      {
        wms_teleservice_e_type teleservice = WMS_TELESERVICE_CMT_95;

        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          teleservice = pMe->m_pMO_template_msg->u.cdma_template.teleservice;
        }
        else
        {
          teleservice = pMe->m_pClient_msg_mo->u.cdma_message.teleservice;
        }

        switch(teleservice)
        {
          case WMS_TELESERVICE_CMT_91:
            return IDL_TLSRV_CMT91;
          case WMS_TELESERVICE_CPT_95:
            return IDL_TLSRV_CPT;
          case WMS_TELESERVICE_CMT_95:
            return IDL_TLSRV_CMT95;
          case WMS_TELESERVICE_VMN_95:
            return IDL_TLSRV_VMN;
          case WMS_TELESERVICE_WEMT:
            return IDL_TLSRV_WEMT;
          default:
            return 0;
        }
      }


    case IDD_CDMA_DCS:
      {
        wms_user_data_encoding_e_type dcs = WMS_ENCODING_ASCII;

        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          dcs = pMe->m_pMO_template_msg->u.cdma_template.dcs;
        }
        else
        {
          dcs = pMe->m_pClient_ts_mo->u.cdma.user_data.encoding;
        }

        switch(dcs)
        {
          case WMS_ENCODING_ASCII:
            return IDL_DCS_ASCII;
          case WMS_ENCODING_OCTET:
            return IDL_DCS_OCTET;
          case WMS_ENCODING_IA5:
            return IDL_DCS_IA5;
//        case WMS_ENCODING_IS91EP:
//          return IDL_DCS_IS91EP;
          case WMS_ENCODING_UNICODE:
            return IDL_DCS_UNICODE;
          case WMS_ENCODING_GSM_7_BIT_DEFAULT:
            return IDL_DCS_GSM_7_BIT;
          default:
            return 0;
        }
      }

    case IDD_PRIORITY:
      {
        wms_priority_e_type priority = WMS_PRIORITY_NORMAL;

        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          priority = pMe->m_pMO_template_msg->u.cdma_template.client_bd.priority;
        }
        else
        {
          priority = pMe->m_pClient_ts_mo->u.cdma.priority;
        }

        switch(priority)
        {
          case WMS_PRIORITY_NORMAL:
            return IDL_PRTY_NORMAL;
          case WMS_PRIORITY_INTERACTIVE:
            return IDL_PRTY_INTERACTIVE;
          case WMS_PRIORITY_URGENT:
            return IDL_PRTY_URGENT;
          case WMS_PRIORITY_EMERGENCY:
            return IDL_PRTY_EMERGENCY;
          default:
            return 0;
        }
      }

    case IDD_PRIVACY:
      {
        wms_privacy_e_type  privacy = WMS_PRIVACY_NORMAL;

        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          privacy = pMe->m_pMO_template_msg->u.cdma_template.client_bd.privacy;
        }
        else
        {
          privacy = pMe->m_pClient_ts_mo->u.cdma.privacy;
        }

        switch(privacy)
        {
          case WMS_PRIVACY_NORMAL:
            return IDL_PRV_NORMAL;
          case WMS_PRIVACY_RESTRICTED:
            return IDL_PRV_RESTRICT;
          case WMS_PRIVACY_CONFIDENTIAL:
            return IDL_PRV_CONFIDENTIAL;
          case WMS_PRIVACY_SECRET:
            return IDL_PRV_SECRET;
          default:
            return 0;
        }
      }

    case IDD_LANGUAGE:
      {
        wms_language_e_type lang = WMS_LANGUAGE_UNSPECIFIED;

        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          lang = pMe->m_pMO_template_msg->u.cdma_template.client_bd.language;
        }
        else
        {
          lang = pMe->m_pClient_ts_mo->u.cdma.language;
        }

        switch(lang)
        {
          case WMS_LANGUAGE_ENGLISH:
            return IDL_LANG_ENGLISH;
          case WMS_LANGUAGE_FRENCH:
            return IDL_LANG_FRENCH;
          case WMS_LANGUAGE_SPANISH:
            return IDL_LANG_SPANISH;
          case WMS_LANGUAGE_JAPANESE:
            return IDL_LANG_JAPANESE;
          case WMS_LANGUAGE_KOREAN:
            return IDL_LANG_KOREAN;
          case WMS_LANGUAGE_CHINESE:
            return IDL_LANG_CHINESE;
          case WMS_LANGUAGE_HEBREW:
            return IDL_LANG_HEBREW;
          case  WMS_LANGUAGE_UNSPECIFIED:
          default:
            return IDL_LANG_UNSPECIFIED;
        }
      }

    case IDD_AUTO_DC_SEND_SO:
      switch(pMe->m_AutoDcSendSO)
      {
        case WMS_DC_SO_6:
          return IDL_AUTO_DC_SEND_SO_6;

        case WMS_DC_SO_14:
          return IDL_AUTO_DC_SEND_SO_14;

        case WMS_DC_SO_AUTO:
        default:
          return IDL_AUTO_DC_SEND_SO_AUTO;
      }

#endif /*FEATURE_CDSMS*/
    case IDD_MODE_SELECT:
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
        return IDL_MODE_CDMA;
      }
      else
      {
        return IDL_MODE_GW;
      }

    case IDD_DOMAIN_PREF:
      {
        switch (pMe->m_GWDomainPref)
        {
          case WMS_GW_DOMAIN_PREF_CS_PREFERRED:
          return IDL_CS_PREF;
          
          case WMS_GW_DOMAIN_PREF_PS_PREFERRED:
          return IDL_PS_PREF;

          case WMS_GW_DOMAIN_PREF_CS_ONLY:
            return IDL_CS_ONLY;

          case WMS_GW_DOMAIN_PREF_PS_ONLY:
            return IDL_PS_ONLY;

          default:
            pMe->m_GWDomainPref = WMS_GW_DOMAIN_PREF_CS_PREFERRED;
            (void)IWMS_CfgSetGWDomainPref(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe, WMS_GW_DOMAIN_PREF_CS_PREFERRED);
            return IDL_CS_PREF;
        }
      }

    case IDD_SEGMENT_SETTING:
      switch(pMe->m_segmentSetting)
      {
        case WMSAPP_NO_SEGMENT:
          wItemID = IDL_NO_SEGMENT;
          break;

        case WMSAPP_USE_CONCAT_8:
          wItemID = IDL_8_BIT_SEGMENT;
          break;

        case WMSAPP_USE_CONCAT_16:
          wItemID = IDL_16_BIT_SEGMENT;
          break;

        default:
          pMe->m_segmentSetting = WMSAPP_NO_SEGMENT;
          (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_SEGMENT_SETTING, &pMe->m_segmentSetting, 1);
          wItemID = IDL_NO_SEGMENT;
          break;
      }
      return wItemID;

    case IDD_RETRY_SETTINGS:
      pMe->m_retryPeriod = 0;
      (void)IWMS_MsgGetRetryPeriod(pMe->m_pwms, pMe->m_clientId, (uint32*)&pMe->m_retryPeriod);
      switch(pMe->m_retryPeriod)
      {
        case 0:
          wItemID = IDL_NO_RETRY;
          break;

        case 30:
          wItemID = IDL_RETRY_30;
          break;

        case 60:
          wItemID = IDL_RETRY_60;
          break;

        case 120:
          wItemID = IDL_RETRY_120;
          break;

        case 240:
          wItemID = IDL_RETRY_240;
          break;

        default:
          pMe->m_retryPeriod = 0;
          (void)IWMS_MsgSetRetryPeriod(pMe->m_pwms, pMe->m_clientId, pMe->m_retryPeriod);
          wItemID = IDL_NO_RETRY;
          break;
      }
      return wItemID;


    default:
      /* translate the static variable to IDL_XXX */
      return 0;
  }
} /* WMSAPPMN_GetCurrentSetting */

/*===========================================================================
FUNCTION WMSAPPMN_INITRADIOBUTTONDLG

DESCRIPTION
  This function update the radio button menu item list based on featurization,
  update the radio button for the selected item based on NV value

DEPENDENCIES
  WMSAPPMN_GetDlgMenuCtlID,  WMSAPPMN_GetCurrentSetting,
  WMSAPPMN_SetRadioButtonImage, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitRadioButtonDlg(CWMSAPP *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;


  if(pIActiveDialog != NULL)
  {
    /* Get the control ID */
    wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIActiveDialog);
    if(wCtrlID == 0)
    { /* error */
      return FALSE;
    }

    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16)wCtrlID);

    if(pICurrentMenu)
    {
      /* get the current setting from static variable or NV */
      wItemID = WMSAPPMN_GetCurrentSetting(pMe, wDlgID);

#ifdef FEATURE_CDSMS
      /* get the current setting from static variable or NV */
      wItemID = WMSAPPMN_GetCurrentSetting(pMe, wDlgID);
      if(wCtrlID == IDC_MEMORY_STORE){
        (void)IMENUCTL_DeleteItem(pICurrentMenu, IDL_MEM_STORE_RAM);
#ifndef FEATURE_CDSMS_RUIM
        (void)IMENUCTL_DeleteItem(pICurrentMenu, IDL_MEM_STORE_CARD);
#endif /* FEATURE_CDSMS_RUIM */
      }
#endif /* FEATURE_CDSMS */

      if(wItemID == 0)
      {
        if ((wDlgID == IDD_MEMORY_STORE)||(wDlgID == IDD_ROUTES))
        {
          // wItemID = 0 signifies mem_store = NONE
          // Hence Blocking Dialog
          return FALSE;
        }
        wItemID = IMENUCTL_GetItemID(pICurrentMenu,0);
      }


      InitMenuIcons(pICurrentMenu);
      /* update the radio button for the currently selected option */
      SetMenuIcon(pICurrentMenu, wItemID,TRUE);

      /* Set the menu style */
      SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      return TRUE;
    }
    return FALSE; /* error since there doesn't exist a menu control */
  }
  else
  { /* active dialog == NULL  ---->  error */
    return FALSE;
  }
} /* InitRadioButtonDlg */

/*===========================================================================
FUNCTION WMSAPPMN_SetStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_SetStaticTextStyle(IStatic *pIStaticText)
{
  (void)ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
  ISTATIC_SetActive(pIStaticText, TRUE);
} /*End SetGeneralStaticTextStyle */



/*===========================================================================
FUNCTION WMSAPPMN_GetSmallPictRes

DESCRIPTION
   This function gets the RESOURCE ID of MM ringer

DEPENDENCIES
 None

SIDE EFFECTS
  None
===========================================================================*/

static int WMSAPPMN_GetResourceFile(uint16 wDlgID, int item)
{
#ifndef FEATURE_SMS_EXTENDED
  switch(wDlgID)
  {
    case IDD_VIEW_SMALL_PICTURES:
      switch(item)
      {
        case 0:
          return IDL_SMALL_PICTURE1;
        case 1:
          return IDL_SMALL_PICTURE2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_LARGE_PICTURES:
      switch(item)
      {
        case 0:
          return IDL_LARGE_PICTURE1;
        case 1:
          return IDL_LARGE_PICTURE2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_VAR_PICTURES:
      switch(item)
      {
        case 0:
          return IDL_VAR_PICTURE1;
        case 1:
          return IDL_VAR_PICTURE2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_SMALL_ANIM:
      switch(item)
      {
        case 0:
          return IDL_SMALL_ANIM1;
        case 1:
          return IDL_SMALL_ANIM2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_LARGE_ANIM:
      switch(item)
      {
        case 0:
          return IDL_LARGE_ANIM1;
        case 1:
          return IDL_LARGE_ANIM2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_PREDEF_ANIM:
      return WMSAPPMN_GetPreDefAnimListResID(item);

    default:
      break;
  }
#endif /* !FEATURE_SMS_EXTENDED */
  return 0;
}

char *WMSAPPMN_GetDirName
(
  CWMSAPP *pMe,
  uint16 wDlgID
)
{
  char *pszDir=MALLOC(MAX_FILE_NAME);

  if(pszDir == NULL)
  {
    return NULL;
  }

  switch(wDlgID)
  {
    case IDD_VIEW_SMALL_PICTURES:
      (void)STRLCPY(pszDir, "ems/smallpictures", MAX_FILE_NAME);
      break;

    case IDD_VIEW_LARGE_PICTURES:
      (void)STRLCPY(pszDir, "ems/largepictures", MAX_FILE_NAME);
      break;

    case IDD_VIEW_VAR_PICTURES:
      (void)STRLCPY(pszDir, "ems/varpictures", MAX_FILE_NAME);
      break;

    case IDD_VIEW_SMALL_ANIM:
      (void)STRLCPY(pszDir, "ems/smallanimations", MAX_FILE_NAME);
      break;

    case IDD_VIEW_LARGE_ANIM:
      (void)STRLCPY(pszDir, "ems/largeanimations", MAX_FILE_NAME);
      break;

    case IDD_VIEW_PREDEF_ANIM:
      (void)STRLCPY(pszDir, "ems/predefanimations", MAX_FILE_NAME);
      break;

    case IDD_LIST_USER_MELODY:
      (void)STRLCPY(pszDir, "ems/usermelody", MAX_FILE_NAME);
      break;

    default:
      break;
  }

  return pszDir;
}

#ifdef FEATURE_SMS_UDH
static int WMSAPPMN_GetItemPos
(
uint16 wDlgID,
uint16 itemID
)
{
  switch(wDlgID)
  {
    case IDD_VIEW_SMALL_PICTURES:
      switch(itemID)
      {
        case IDL_SMALL_PICTURE1:
          return 1;
        case IDL_SMALL_PICTURE2:
          return 2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_LARGE_PICTURES:
      switch(itemID)
      {
        case IDL_LARGE_PICTURE1:
          return 1;
        case IDL_LARGE_PICTURE2:
          return 2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_VAR_PICTURES:
      switch(itemID)
      {
        case IDL_VAR_PICTURE1:
          return 1;
        case IDL_VAR_PICTURE2:
          return 2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_SMALL_ANIM:
      switch(itemID)
      {
        case IDL_SMALL_ANIM1:
          return 1;
        case IDL_SMALL_ANIM2:
          return 2;
        default:
          break;
      }
      return 0;

    case IDD_VIEW_LARGE_ANIM:
      switch(itemID)
      {
        case IDL_LARGE_ANIM1:
          return 1;
        case IDL_LARGE_ANIM2:
          return 2;
        default:
          break;
      }
      return 0;

    case IDD_LIST_USER_MELODY:
      return (WMSAPPMN_GetMelodyListIndex(itemID)+1);

    default:
      break;
  }
  return 0;
}
#endif /* FEATURE_SMS_UDH */

#ifdef FEATURE_SMS_UDH
static char* WMSAPPMN_GetFileList
(
CWMSAPP *pMe,
char* dirName,
uint16 itemVal
)
{
  IFileMgr* pIFileMgr;
  FileInfo fileInfo;
  int i=1;
  char *fileName = MALLOC(MAX_FILE_NAME);


  if(fileName == NULL)
    return NULL;

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
  {
    WMSAPPU_FREE(fileName);
    return NULL;
  }

  if(EFAILED == IFILEMGR_EnumInit(pIFileMgr, dirName, FALSE))
  {
    (void)IFILEMGR_Release(pIFileMgr);
    WMSAPPU_FREE(fileName);
    return NULL;
  }

  while(IFILEMGR_EnumNext(pIFileMgr, &fileInfo) && i<= itemVal)
  {
    if(i == itemVal)
    {
      (void)STRLCPY(fileName,fileInfo.szName,MAX_FILE_NAME);
      (void)IFILEMGR_Release(pIFileMgr);
      return fileName;
    }
    i++;
  }
  //Clean up
  (void)IFILEMGR_Release(pIFileMgr);
  return NULL;

}
#endif /* FEATURE_SMS_UDH */

#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
static int WMSAPPMN_GetSelectionIndex
(
  CWMSAPP *pMe,
  uint16 wDlgID,
  IDialog* pIDialog
)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  //Get The Dialog and the Menu Control
  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
  if(pICurrentMenu == NULL)
  {
    return 0;
  }

  if (wDlgID == IDD_VIEW_PREDEF_ANIM)
  {
    // Predefined Animation
    return (WMSAPPMN_GetPreDefAnimListIndex(IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu)));
  }
  else
  {
    // Predefined Sound or user defined Sound
    return (WMSAPPMN_GetMelodyListIndex(IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu)));
  }
}
#endif /* FEATURE_SMS_UDH && !FEATURE_SMS_EXTENDED */

#ifdef FEATURE_SMS_UDH
static char* WMSAPPMN_GetFileName
(
CWMSAPP *pMe,
uint16 wDlgID,
IDialog* pIDialog
)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  char *pszDir;
  uint16 itemVal;
  char *pszFile;


  //Get The Dialog and the Menu Control

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  if(pICurrentMenu == NULL)
  {
    return NULL;
  }

  // Get the Selected Item
  itemVal = (uint16)WMSAPPMN_GetItemPos(wDlgID, IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu));

  //Get the File Name
  pszDir = WMSAPPMN_GetDirName(pMe, wDlgID);

  if(pszDir == NULL)
  {
     return NULL;
  }

  pszFile = WMSAPPMN_GetFileList(pMe, pszDir,itemVal);
  WMSAPPU_FREE(pszDir);
  return pszFile;
}
#endif /* FEATURE_SMS_UDH */

static boolean WMSAPPMN_GetImageList
(
 CWMSAPP *pMe
)
{
  FileInfo fileInfo;
  uint16 wDlgID,wItemID =0,wIMenuID = 0, wImageID;
  AECHAR      *szBuf;
  IFileMgr *pIFileMgr = NULL;
  char *pszDir;
  IDialog *pIDialog;
  IMenuCtl *pIMenu;
  IStatic *pIStatic;
  uint16 imageNum;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wDlgID = IDIALOG_GetID(pIDialog);

  wIMenuID  = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wIMenuID);

  switch(wDlgID)
  {
    case IDD_VIEW_SMALL_PICTURES:
      pszDir = "ems/smallpictures\0";
      wImageID = IDC_VIEW_SMALL_PICT;
      break;

    case IDD_VIEW_LARGE_PICTURES:
      pszDir = "ems/largepictures\0";
      wImageID = IDC_VIEW_LARGE_PICT;
      break;

    case IDD_VIEW_VAR_PICTURES:
      pszDir = "ems/varpictures\0";
      wImageID = IDC_VIEW_VAR_PICT;
      break;

    case  IDD_VIEW_SMALL_ANIM:
      pszDir = "ems/smallanimations\0";
      wImageID = IDC_VIEW_SMALL_ANIM;
      break;

    case IDD_VIEW_LARGE_ANIM:
      pszDir = "ems/largeanimations\0";
      wImageID = IDC_VIEW_LARGE_ANIM;
      break;

    case IDD_VIEW_PREDEF_ANIM:
      pszDir = "ems/predefanimations\0";
      wImageID = IDC_VIEW_PREDEF_ANIM;
      break;

    default: /*None of the Above Dialogs*/
      return FALSE;
  }

  pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, (int16)wImageID);

  imageNum = IMENUCTL_GetSel(pIMenu);

  if (imageNum == 0)
  {
    /* Menu Control Not Initialized */
    imageNum = (uint16)WMSAPPMN_GetResourceFile(wDlgID, 0);
  }

  szBuf = MALLOC(MAX_FILE_NAME * sizeof(AECHAR));

  if(szBuf == NULL)
    return FALSE;

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
  {
    WMSAPPU_FREE(szBuf);
    return FALSE;
  }

  (void)ISTATIC_SetProperties(pIStatic, ST_ICONTEXT);

  if (IFILEMGR_EnumInit(pIFileMgr, pszDir, FALSE)  == EFAILED)
  {
      WMSAPPU_FREE(szBuf);
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
  }

  /* Hack for proper UI Display */
  if (wDlgID != IDD_VIEW_PREDEF_ANIM)
  {
    (void)IMENUCTL_DeleteAll ( pIMenu );
  }
  
  while(IFILEMGR_EnumNext(pIFileMgr, &fileInfo))
  {
    char *   szName;
    uint16   itemtoAdd;

    szName = STRRCHR(fileInfo.szName, (int)DIRECTORY_CHAR);

    if(szName)
      szName++;
    else
      szName = (char *)fileInfo.szName;

    itemtoAdd = (uint16)WMSAPPMN_GetResourceFile(wDlgID, wItemID);
    if(itemtoAdd ==0)
    {
      WMSAPPU_FREE(szBuf);
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
    if(itemtoAdd == imageNum)
    {
      IMENUCTL_SetSel(pIMenu,itemtoAdd);

      if(wDlgID == IDD_VIEW_SMALL_PICTURES ||
         wDlgID == IDD_VIEW_LARGE_PICTURES ||
         wDlgID == IDD_VIEW_VAR_PICTURES)
      {
        (void)WMSAPPMN_ShowImage(pMe,pIStatic, fileInfo.szName);
      }
      else
      {
        (void)WMSAPPMN_ShowAnim(pMe,pIStatic, fileInfo.szName);
      }
    }
    if (wDlgID != IDD_VIEW_PREDEF_ANIM)
    {
      (void)STRTOWSTR(szName, szBuf, MAX_FILE_NAME);
      (void)IMENUCTL_AddItem(pIMenu, NULL, 0, (uint16)itemtoAdd, szBuf, 0);
    }
    wItemID++;

  }

  WMSAPPU_FREE(szBuf);
  (void)IFILEMGR_Release(pIFileMgr);

  return TRUE;
}

#ifdef FEATURE_SMS_UDH
static void WMSAPPMN_CreateNewStyleFormatHeader
(
 CWMSAPP *pMe
)
{
  uint8 nHeaders;
  wms_udh_s_type *pUdh = NULL;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    nHeaders = pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers;

    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders].header_id = WMS_UDH_TEXT_FORMATING;
    nHeaders++;
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers++;
    pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = TRUE;
    pUdh= &(pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders-1]);

    pUdh->u.text_formating.start_position
       = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;
#endif
    }
    else /*GWSMS*/
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif
    }
    pUdh->u.text_formating.text_formatting_length = 0;

    if(pMe->m_styleMask & WMSAPP_STYLE_FORMAT_BOLD)/*Bold was set*/
    {
      pUdh->u.text_formating.style_bold = TRUE;
    }
    else
    {
      pUdh->u.text_formating.style_bold = FALSE;
    }

    if(pMe->m_styleMask & WMSAPP_STYLE_FORMAT_ITALIC)/*Italic was set*/
    {
      pUdh->u.text_formating.style_italic = TRUE;
    }
    else
    {
      pUdh->u.text_formating.style_italic = FALSE;
    }

    if(pMe->m_styleMask & WMSAPP_STYLE_FORMAT_UNDERLINE)/*Italic was set*/
    {
      pUdh->u.text_formating.style_underlined = TRUE;
    }
    else
    {
      pUdh->u.text_formating.style_underlined = FALSE;
    }

    if(pMe->m_styleMask & WMSAPP_STYLE_FORMAT_SIZE)/*Italic was set*/
    {
      pUdh->u.text_formating.font_size = WMS_UDH_FONT_LARGE;
    }
    else
    {
      pUdh->u.text_formating.font_size = WMS_UDH_FONT_NORMAL;
    }

    pUdh->u.text_formating.alignment_type = WMS_UDH_DEFAULT_ALIGNMENT;
}
#endif /* FEATURE_SMS_UDH */

static boolean WMSAPPMN_SetUserDataStyleText
(
  CWMSAPP *pMe,
  boolean setTextFormat
)
{
#ifdef FEATURE_SMS_UDH
  /*The data represents the current situation of the softkey.
    if the data is 1 it implies that the  soft key has been depressed and the style format is turned on*/

  /*If the Data is 1*/
  /*-----------------*/
  /*1.Check if the current header is Text Formatting. End the Current header before proceeding
    2.if the formatting length of the current header is 0, then this header should be removed from
    the user data, it has no contents
    3.The Bitmask is updated
    4.The boolean styleFormat is set.
    5.A new header is added
    6.The start position of the header is set to the number of characters in the user data
    7.The formatting length is set to 0.*/

  /*If data is 0*/
  /*-------------*/
  /*1.The bitmask is updated*/

  /*if the Bitmask is 0,
    1.StyleFormat header is set to FALSE
    2.if the formatting length of the current header is 0, then this header should be removed from
    the user data, it has no contents
  */

  /*If bitmask still has some values then it implies that some style format is still on
    but it has to be a new header.
    1.Check the formatting length of the header. If zero, then do not do anything, exit
    2.Else Create a new header
    3.Set the Start position
    4.Set the formatting length to 0*/

  wms_udh_s_type *pUdh = NULL;
  int nHeaders =0;
  uint8 totalUDLength =0 ;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    nHeaders = pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers;
    if(nHeaders !=0)
    {
      pUdh= &(pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders-1]);
      totalUDLength = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;
    }
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  if(setTextFormat)
  {
    /*If the Current Header is Text Formatting , then close it*/
    if(nHeaders != 0 && pUdh && pUdh->header_id == WMS_UDH_TEXT_FORMATING)
    {
      pUdh->u.text_formating.text_formatting_length =
         totalUDLength - pUdh->u.text_formating.start_position;

      /*Remove header if extra headers*/
      if(pUdh->u.text_formating.text_formatting_length == 0)
      {
        /*remove header*/
        nHeaders--;
        /**If no more headers, then set udh_present to FALSE*/
        if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
        {
#ifdef FEATURE_CDSMS
          if(nHeaders == 0)
          {
            pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
          }
          pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = (uint8)nHeaders;
#endif
        }
        else
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif
        }
      }
    }
    /*Create a new header*/
    if(nHeaders < WMS_MAX_UD_HEADERS)
    {
      WMSAPPMN_CreateNewStyleFormatHeader(pMe);
    }
  }
  else if (pUdh)/*Some/All Style(s) was turned off*/
  {
    /*Set the formatting length*/
      pUdh->u.text_formating.text_formatting_length =
       totalUDLength - pUdh->u.text_formating.start_position;

    if(pMe->m_styleMask == 0)
    {
      if(pUdh->u.text_formating.text_formatting_length == 0)
      {
        /*remove header*/
        nHeaders--;
        /**If no more headers, then set udh_present to FALSE*/
        if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
        {
#ifdef FEATURE_CDSMS
          if(nHeaders == 0)
          {
            pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
          }
          pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = (uint8)nHeaders;
#endif
        }
        else
        {
#ifdef FEATURE_GWSMS
#error code not present
#endif
        }
      }
    }
    else /*Style mask is still present*/
    {
      /*Check Formating length before creating new header*/
      if(pUdh->u.text_formating.text_formatting_length != 0)
      {
        /*Create new Header*/
        if(nHeaders < WMS_MAX_UD_HEADERS)
        {
          WMSAPPMN_CreateNewStyleFormatHeader(pMe);
        }
      }
    }
  }
#endif /* FEATURE_SMS_UDH */
 return TRUE;
}


static boolean WMSAPPMN_SetUserDataImage
(
CWMSAPP *pMe
)
{
  boolean retVal = TRUE;
#ifdef FEATURE_SMS_UDH
  IDialog *pIDialog;
  uint16 wDlgID;

  int nHeaders = 0;
  wms_udh_s_type *pUdh=NULL;
  boolean picture = FALSE;
  int  user_data_length=0;
  char *fileName =NULL;

  wms_client_ts_data_s_type *pOrigTsData = NULL;

  if ((pOrigTsData = MALLOC(sizeof(wms_client_ts_data_s_type))) == NULL)
  {
    /* Memory Allocation Failed */
    return FALSE;
  }

  (void)MEMCPY(pOrigTsData, pMe->m_pClient_ts_mo, sizeof(wms_client_ts_data_s_type));

  pIDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wDlgID = IDIALOG_GetID(pIDialog);


  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    nHeaders = pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers;
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders];
    user_data_length = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  /* If the Previous UDH was Text Formating, then its length field
     should be updated */
  if(pUdh->header_id == WMS_UDH_TEXT_FORMATING &&
     pUdh->u.text_formating.text_formatting_length == 0)
  {
    pUdh->u.text_formating.text_formatting_length
      =  (uint8)user_data_length - pUdh->u.text_formating.start_position;

    /*Update pUdh*/
    if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
      pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders];
#endif/*FEATURE_CDSMS*/
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/
    }
  }

  if ((wDlgID != IDD_LIST_MELODY)&&(wDlgID != IDD_VIEW_PREDEF_ANIM))
  {
    // Predefined Melody or Animation does not use File for filling header
    fileName = WMSAPPMN_GetFileName(pMe,wDlgID, pIDialog);
    if(fileName  == NULL)
    {
      return FALSE;
    }
  }

  switch(wDlgID)
  {
#ifndef FEATURE_SMS_EXTENDED
    case IDD_VIEW_SMALL_PICTURES:
      pUdh->header_id = WMS_UDH_SMALL_PICTURE;

      /*The UDH Starts after the current user data length*/
      pUdh->u.small_picture.position =  (uint8)user_data_length;

      (void)WMSAPPMD_ConvertPicToEMS(pMe, fileName, pUdh);

      picture = TRUE;
      break;

    case IDD_VIEW_LARGE_PICTURES:
      pUdh->header_id = WMS_UDH_LARGE_PICTURE;

      /*The UDH Starts after the current user data length*/
      pUdh->u.large_picture.position = (uint8)user_data_length;

      (void)WMSAPPMD_ConvertPicToEMS(pMe, fileName, pUdh);

      picture = TRUE;
      break;

    case IDD_VIEW_VAR_PICTURES:
      pUdh->header_id = WMS_UDH_VAR_PICTURE;
      pUdh->u.var_picture.position = (uint8)user_data_length;

      (void)WMSAPPMD_ConvertPicToEMS(pMe, fileName, pUdh);

      picture = TRUE;
      break;

    case IDD_VIEW_SMALL_ANIM:
      pUdh->header_id = WMS_UDH_SMALL_ANIM;

      pUdh->u.small_anim.position = (uint8)user_data_length;

      (void)WMSAPPMD_ConvertSmallAnimToEMS(pMe,
                                     (pUdh->u.small_anim.data),
                                     fileName
                                    );
      picture = TRUE;
      break;

    case IDD_VIEW_LARGE_ANIM:
      pUdh->header_id = WMS_UDH_LARGE_ANIM;

      pUdh->u.large_anim.position = (uint8)user_data_length;

      (void)WMSAPPMD_ConvertLargeAnimToEMS(pMe,
                                     (pUdh->u.large_anim.data),
                                     fileName
                                    );
      picture = TRUE;
      break;

    case IDD_VIEW_PREDEF_ANIM:
      pUdh->header_id = WMS_UDH_PRE_DEF_ANIM;

      pUdh->u.pre_def_anim.position = (uint8)user_data_length;

      // Get this from Menu Ctl
      pUdh->u.pre_def_anim.animation_number = (uint8)WMSAPPMN_GetSelectionIndex(pMe,wDlgID, pIDialog);

      picture = TRUE;
      break;

    case IDD_LIST_MELODY:
      // Predefined Melody
      pUdh->header_id = WMS_UDH_PRE_DEF_SOUND;

      pUdh->u.pre_def_sound.position = (uint8)user_data_length;

      // Get this from Menu Ctl
      pUdh->u.pre_def_sound.snd_number = (uint8)WMSAPPMN_GetSelectionIndex(pMe,wDlgID, pIDialog);

      picture = TRUE;
      break;

    case IDD_LIST_USER_MELODY:
      // User Defined Melody
      pUdh->header_id = WMS_UDH_USER_DEF_SOUND;

      pUdh->u.user_def_sound.position = (uint8)user_data_length;

      pUdh->u.user_def_sound.data_length = (uint8)WMSAPPMD_ConvertUserMelodyToEMS(pMe,
                                           (pUdh->u.user_def_sound.user_def_sound),
                                           fileName);

      picture = TRUE;
      break;
#endif /* !FEATURE_SMS_EXTENDED */

    default:
      break;
  }
  
  if(picture == TRUE)
  {
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
      (void)MEMCPY(&pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders],
            pUdh,
            sizeof(wms_udh_s_type));

      pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;

      pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = (uint8)nHeaders+1;
      pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = TRUE;
      pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_USER_DATA;

      /* Initialized other to 0 */
      (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

      if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts ) != SUCCESS)
      {
        MSG_ERROR("TS Encoding failed!",0,0,0);
        retVal = FALSE;

        /* Revert back to Original Data */
        (void)MEMCPY(pMe->m_pClient_ts_mo, pOrigTsData, sizeof(wms_client_ts_data_s_type));
      }
#endif /*FEATURE_CDSMS*/
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
    }
  }
  else
  {
    retVal = FALSE;
  }

  WMSAPPU_FREE(pOrigTsData);
  WMSAPPU_FREE(fileName);
#endif /* FEATURE_SMS_UDH */

  return retVal;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitUserRelValDlg

DESCRIPTION
  Set the User Defined Relative Validity

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_InitUserRelValDlg(CWMSAPP *pMe)
{
  IDialog  * pIDialog;
  IMenuCtl * pIMenuCtl;
  ITextCtl * pITextCtl;
  uint16     wTextCtlID,wSKCtrlID;
  wms_timestamp_s_type relative_time;
  uint8      time_encoded;

  /* Dialog pointer */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  /* Text Control */
  wTextCtlID  = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16)wTextCtlID);

  /* Switch to Appropriate value based on Preferences */
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {
      if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative
          = pMe->m_pMO_template_msg->u.cdma_template.relative_validity;
        relative_time = pMe->m_pMO_template_msg->u.cdma_template.relative_validity;
      }
      else /* Deferred Relative */
      {
        relative_time = pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative;
      }
    }
    else /* OUTBOX */
    {
      if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
      {
        relative_time = pMe->m_pClient_ts_mo->u.cdma.validity_relative;
      }
      else /* deferred rel */
      {
        relative_time = pMe->m_pClient_ts_mo->u.cdma.deferred_relative;
      }
    }
  }
  else if (pMe->m_modeControl == WMSAPP_MODE_GW)
  {
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {
      relative_time = pMe->m_pMO_template_msg->u.gw_template.relative_validity;
    }
    else
    {
      relative_time = pMe->m_pClient_ts_mo->u.gw_pp.u.submit.validity.u.time;
    }
  }

  /* Encode the Value into an Integer */
  time_encoded = IWMS_TsEncodeRelativeTime(pMe->m_pwms, &relative_time);

  /* Display the Integer */
  if (pITextCtl)
  {
    AECHAR textString[14];
    char tempBuf[14];
    uint8 offset=0;

    /* Format the Time */
    if ((relative_time.year == 0)
      &&(relative_time.month == 0)
      &&(relative_time.day == 0)
      &&(relative_time.hour == 0)
      &&(relative_time.minute == 0))
    {
      /* All Null TimeStamp; Invalid */
      tempBuf[0] = '\0';
    }
    else
    {
      (void)WMSAPPU_ITOA(time_encoded, tempBuf, STRLEN(tempBuf) - offset, 10);
    }
    offset = (uint8)STRLEN(tempBuf);
    (void)STRTOWSTR(tempBuf, textString, sizeof(AECHAR)*(offset+1));
    (void)ITEXTCTL_SetText(pITextCtl, textString, -1);
    ITEXTCTL_SetCursorPos(pITextCtl, WSTRLEN(textString));
    (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
  }

  /* Soft Key Control */
  wSKCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);
  if(wSKCtrlID == 0)
  {
    return FALSE;
  }

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wSKCtrlID);
  if(!pIMenuCtl)
  {
    /* a menu control existed for this dialog */
    return FALSE;
  }
  /* To Remove the Multitap Soft Key inserted by Text Control */
  (void)IMENUCTL_DeleteAll(pIMenuCtl);

  /* Insert Soft Key for Done */
  (void)IMENUCTL_AddItem(pIMenuCtl, WMSAPP_RES_FILE, IDS_DONE, IDC_SK_REL_VAL_USER_DEFINED, NULL, 0);

  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);

  return TRUE;
} /* WMSAPPMN_InitUserRelValDlg */

/*===========================================================================
FUNCTION WMSAPPMN_StoreUserRelValDlg

DESCRIPTION
  Store the User Defined Relative Validity

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_StoreUserRelValDlg(CWMSAPP *pMe)
{
  IDialog  * pIDialog;
  ITextCtl * pITextCtl;
  uint16     wTextCtlID;
  wms_timestamp_s_type relative_time;
  uint8      time_encoded;

  AECHAR *tempBuf;
  char    tempStr[WMS_ADDRESS_MAX];
  int     tempVal;

  /* Dialog pointer */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  /* Text Control */
  wTextCtlID  = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16)wTextCtlID);

  pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  /* Encoded Relative Validity */
  pITextCtl = (ITextCtl *)IDIALOG_GetControl(pIDialog, (int16)wTextCtlID);
  tempBuf = ITEXTCTL_GetTextPtr(pITextCtl);
  (void)WSTRTOSTR(tempBuf, tempStr, WSTRLEN(tempBuf)+1);
  tempVal = ATOI(tempStr);
  if (tempVal >= 0 && tempVal <= 255)
  {
    time_encoded = (uint8)tempVal;
  }
  else
  {
    /* Time is out of Bounds */
    return FALSE;
  }

  /* Decode the TimeStamp */
  (void)IWMS_TsDecodeRelativeTime(pMe->m_pwms, time_encoded, &relative_time);

  /* Switch to Appropriate value based on Preferences */
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {
      if(pMe->m_currPref == IDL_C_TMPLT_VALID_REL)
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.validity_relative = relative_time;
        pMe->m_pMO_template_msg->u.cdma_template.relative_validity = relative_time;
      }
      else /* Deferred Relative */
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.deferred_relative = relative_time;
      }
    }
    else /* OUTBOX */
    {
      if(pMe->m_currPref == IDL_OUTBOX_VALID_REL)
      {
        pMe->m_pClient_ts_mo->u.cdma.validity_relative = relative_time;
      }
      else /* deferred rel */
      {
        pMe->m_pClient_ts_mo->u.cdma.deferred_relative = relative_time;
      }
    }
  }
  else if (pMe->m_modeControl == WMSAPP_MODE_GW)
  {
    if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
    {
      pMe->m_pMO_template_msg->u.gw_template.relative_validity = relative_time;
    }
    else
    {
      pMe->m_pClient_ts_mo->u.gw_pp.u.submit.validity.u.time = relative_time;
    }
  }
  return TRUE;
} /* WMSAPPMN_StoreUserRelValDlg */

/*===========================================================================
FUNCTION WMSAPPMN_InitViewImgDlg

DESCRIPTION
  Set the static dialog title, content and style

DEPENDENCIES
  SetStaticDlgTitle, SetStaticDlgContent, SetGeneralStaticTextStyle,
  SetStaticTextCtlSize

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMN_InitViewImgDlg(CWMSAPP *pMe)
{
  IDialog * pIDialog;
  IMenuCtl *pIMenu, *pISKMenu;
  uint16 wIMenuID,wSKCtlID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);


  wIMenuID  = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wIMenuID);

  if(!WMSAPPMN_GetImageList(pMe))
  {
    return FALSE;
  }

  if (NULL == pIMenu)
  {
     return FALSE;
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  //Add Sk to the List
  wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);

  pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wSKCtlID);
  if (NULL == pISKMenu)
  {
     return FALSE;
  }

  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISKMenu);

  return TRUE;

} /*End WMSAPPMN_InitViewImgDlg */

/*===========================================================================
FUNCTION WMSAPPMN_InitMelodyDlg

DESCRIPTION
  Handles the Initialization of the Melody Dialog
  for user defined melody stored in EFS

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitMelodyDlg
(
  CWMSAPP *pMe
)
{
  char *pszDir;
  FileInfo fileInfo;
  IFileMgr *pIFileMgr = NULL;
  IDialog * pIDialog;
  IMenuCtl * pIMenu;
  uint16 wDlgID, wIMenuID = 0, wItemID = 0, wSKCtlID;
  AECHAR *szBuf;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wDlgID = IDIALOG_GetID(pIDialog);

  wIMenuID  = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wIMenuID);

  pszDir = "ems/usermelody\0";

  szBuf = MALLOC(MAX_FILE_NAME * sizeof(AECHAR));

  if(szBuf == NULL)
    return FALSE;

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
  {
    WMSAPPU_FREE(szBuf);
    return FALSE;
  }

  if (IFILEMGR_EnumInit(pIFileMgr, pszDir, FALSE)  == EFAILED)
  {
      WMSAPPU_FREE(szBuf);
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
  }

  while(IFILEMGR_EnumNext(pIFileMgr, &fileInfo))
  {
    char *   szName;
    uint16   itemtoAdd;

    szName = STRRCHR(fileInfo.szName, (int)DIRECTORY_CHAR);

    if(szName)
      szName++;
    else
      szName = (char *)fileInfo.szName;

    // False: Not predefined Melody
    itemtoAdd = WMSAPPMN_GetMelodyListResID(wItemID, FALSE);

    (void)STRTOWSTR(szName, szBuf, MAX_FILE_NAME);
    (void)IMENUCTL_SetItemText(pIMenu, (uint16)itemtoAdd, WMSAPP_RES_FILE, 0, szBuf);
    wItemID++;
  }

  WMSAPPU_FREE(szBuf);
  (void)IFILEMGR_Release(pIFileMgr);


  while(wItemID <= IMENUCTL_GetItemCount(pIMenu))
  {
    (void)IMENUCTL_DeleteItem(pIMenu, (uint16)WMSAPPMN_GetResourceFile(wDlgID, wItemID));
    wItemID++;
  }

  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenu);

  //Add Sk to the List
  wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wSKCtlID);

  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenu);

  return TRUE;
} /* WMSAPPMN_InitMelodyDlg */

#ifdef FEATURE_SMS_UDH

static void WMSAPP_TestInitEms
(
CWMSAPP *pMe
)
{

  pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
  pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    /* Initialize display information for scrolling */
    pMe->m_pClient_ts_mo->format                                 = WMS_FORMAT_CDMA;
    pMe->m_pClient_ts_mo->u.cdma.display_mode                        = WMS_DISPLAY_MODE_DEFAULT;
    pMe->m_pClient_ts_mo->u.cdma.mask = 0;;

    pMe->m_pClient_ts_mo->u.cdma.message_id.type                     = WMS_BD_TYPE_SUBMIT;
    pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present              = TRUE;
    pMe->m_pClient_ts_mo->u.cdma.mask                                = WMS_MASK_BD_MSG_ID;
    pMe->m_pClient_ts_mo->u.cdma.message_id.id_number                = 0xFFFF;
    pMe->m_pClient_ts_mo->u.cdma.num_messages                        = 1;
    pMe->m_pClient_msg_mo->u.cdma_message.is_mo                       = TRUE;
    pMe->m_pClient_msg_mo->u.cdma_message.subaddress.number_of_digits = 0;
    pMe->m_pClient_msg_mo->u.cdma_message.teleservice                 = WMS_TELESERVICE_WEMT;
    pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested         = FALSE;
    pMe->m_pClient_msg_mo->u.cdma_message.is_service_present          = FALSE;
    pMe->m_pClient_msg_mo->u.cdma_message.service                     = WMS_SRV_UNKOWN;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digit_mode          = WMS_DIGIT_MODE_4_BIT;
    pMe->m_pClient_msg_mo->u.cdma_message.address.number_plan         = WMS_NUMBER_PLAN_TELEPHONY;
    pMe->m_pClient_msg_mo->u.cdma_message.address.number_type         = WMS_NUMBER_UNKNOWN;
    pMe->m_pClient_ts_mo->u.cdma.user_data.encoding                  = WMS_ENCODING_GSM_7_BIT_DEFAULT;

    /*Address digits*/
    pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits           =10;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[0]                  =8;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[1]                  =5;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[2]                  =8;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[3]                  =6;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[4]                  =5;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[5]                  =1;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[6]                  =1;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[7]                  =5;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[8]                  =4;
    pMe->m_pClient_msg_mo->u.cdma_message.address.digits[9]                  =8;


    /*memcpy((uint8 *)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
          (char *)"(*) Fixed MT message testing! This is an automatically generated message.",
          73);*/
    (void)MEMCPY((uint8 *)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
          (char *)"SMS&EMS",
          STRLEN("SMS&EMS"));

    //pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits = 73;
    pMe->m_pClient_ts_mo->u.cdma.user_data.data_len = (uint8)STRLEN("SMS&EMS");;
    //pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits = 73;
    pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits = (uint8)STRLEN("SMS&EMS");
    /*  (void)IWMS_TsPackAscii( pMe->m_pwms, (char *)"abcdefg@qualcomm.com (*) Fixed MT message testing! no message can be sent at this time.  please resend at a later time or contact customer service.We apologis",
                          (uint8 *)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                          (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data_len,
                          (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits);*/

    pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_USER_DATA;

    pMe->m_pClient_ts_mo->u.cdma.mask |= WMS_MASK_BD_CALLBACK;

    pMe->m_pClient_ts_mo->u.cdma.callback.number_of_digits = 10;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[0]        =8;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[1]        =5;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[2]        =8;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[3]        =6;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[4]        =5;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[5]        =1;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[6]        =1;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[7]        =5;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[8]        =4;
    pMe->m_pClient_ts_mo->u.cdma.callback.digits[9]        =8;

    pMe->m_pClient_ts_mo->u.cdma.callback.number_plan      = WMS_NUMBER_PLAN_TELEPHONY;
    pMe->m_pClient_ts_mo->u.cdma.callback.number_type      = WMS_NUMBER_UNKNOWN;
  }
  else
  {
    /* Initialize display information for scrolling */
    pMe->m_pClient_ts_mo->format                                     = WMS_FORMAT_GW_PP;
    pMe->m_pClient_ts_mo->u.gw_pp.tpdu_type                          = WMS_TPDU_SUBMIT;

    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.message_reference         = 0xFF;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digit_mode        = WMS_DIGIT_MODE_4_BIT;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.number_plan       = WMS_NUMBER_PLAN_TELEPHONY;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.number_type       = WMS_NUMBER_INTERNATIONAL;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.reject_duplicates         = FALSE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.dcs.alphabet              = WMS_GW_ALPHABET_7_BIT_DEFAULT;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.dcs.is_compressed         = FALSE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.dcs.msg_waiting           = WMS_GW_MSG_WAITING_NONE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.dcs.msg_class                 = WMS_MESSAGE_CLASS_1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.pid                       = WMS_PID_DEFAULT;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.reply_path_present        = FALSE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.status_report_enabled     = FALSE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.validity.format           = WMS_GW_VALIDITY_NONE;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data_header_present  = TRUE;

    /*Address digits*/
    /*Address digits*/
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.number_of_digits = 10;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[0]        = 8;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[1]        = 5;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[2]        = 8;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[3]        = 6;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[4]        = 5;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[5]        = 1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[6]        = 1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[7]        = 5;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.address.digits[8]        = 4;


    (void)MEMCPY((uint8 *)pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.sm_data,
          (char *)"SMS&EMS",
          7);
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.sm_len = 7;

    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 0;
  }

  pMe->m_msg_type = WMSAPP_MSG_TYPE_CANNED;
  pMe->m_addrType = WMSAPP_MSG_TYPE_PHONE_NUM;

}

static boolean WMSAPP_TestFillSmallPic
(
CWMSAPP *pMe,
int n
)
{
  int j=0;

  wms_udh_s_type *pUdh=NULL;
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[n];
  }
  else
  {
    pUdh = &pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[n];
  }

  if(pUdh == NULL)
  {
    return FALSE;
  }

  (void)MEMSET (pUdh->u.small_picture.data,0x00,WMS_UDH_SMALL_PIC_SIZE);

  pUdh->header_id  = WMS_UDH_SMALL_PICTURE;
  pUdh->u.small_picture.position = 7;
  pUdh->u.small_picture.data[j++]  = 0x00;
  pUdh->u.small_picture.data[j++]  = 0x00;
  pUdh->u.small_picture.data[j++]  = 0x01;
  pUdh->u.small_picture.data[j++]  = 0x80;
  pUdh->u.small_picture.data[j++]  = 0x03;
  pUdh->u.small_picture.data[j++]  = 0xC0;
  pUdh->u.small_picture.data[j++]  = 0x07;
  pUdh->u.small_picture.data[j++]  = 0xF0;
  pUdh->u.small_picture.data[j++]  = 0x0F;
  pUdh->u.small_picture.data[j++]  = 0xF8;
  pUdh->u.small_picture.data[j++]  = 0x1F;
  pUdh->u.small_picture.data[j++]  = 0xFC;
  pUdh->u.small_picture.data[j++]  = 0x3F;
  pUdh->u.small_picture.data[j++]  = 0xFE;
  pUdh->u.small_picture.data[j++]  = 0x7F;
  pUdh->u.small_picture.data[j++]  = 0xFF;
  return TRUE;
}

static boolean WMSAPP_TestFillLargePic
(
CWMSAPP *pMe,
int n
)
{
  int j=0;

  wms_udh_s_type *pUdh=NULL;
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[n];
  }
  else
  {
      pUdh = &pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[n];
  }

  if(pUdh == NULL) return FALSE;

  pUdh->header_id  = WMS_UDH_LARGE_PICTURE;
  pUdh->u.large_picture.position = 7;

  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x10;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x10;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x11;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x88;
  pUdh->u.large_picture.data[j++]  = 0x09;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x90;
  pUdh->u.large_picture.data[j++]  = 0x06;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x60;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x06;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x60;
  pUdh->u.large_picture.data[j++]  = 0x09;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x90;
  pUdh->u.large_picture.data[j++]  = 0x11;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x88;
  pUdh->u.large_picture.data[j++]  = 0x10;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x10;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0xff;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x08;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x20;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x04;
  pUdh->u.large_picture.data[j++]  = 0x40;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x02;
  pUdh->u.large_picture.data[j++]  = 0x80;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x01;
  pUdh->u.large_picture.data[j++]  = 0x00;
  pUdh->u.large_picture.data[j++]  = 0x00;
  return TRUE;
}

static boolean WMSAPP_TestFillExtendedObject
(
  CWMSAPP *pMe,
  int n,
  int segment_total
)
{
  int j=0;
  static boolean firstSegment;
  wms_udh_s_type *pUdh=NULL;

  pUdh = &pMe->m_pUserData->headers[n];

  if(pUdh == NULL)
  {
    return FALSE;
  }

  /* fill up the concat16 headers which are even number headers - n = 0, 2, 4 */
  if (n % 2 == 0)
  {
    pUdh->header_id = WMS_UDH_CONCAT_16;
    pUdh->u.concat_16.msg_ref = 1;

    /* first segment */
    if (n == 0)
    {
      pUdh->u.concat_16.seq_num = 1;
      firstSegment = TRUE;
    }
    else
    {
      pUdh->u.concat_16.seq_num = n/2 + 1;
      firstSegment = FALSE;
    }
    pUdh->u.concat_16.total_sm = segment_total;
  }
  else
  {
    (void)MEMSET(pUdh->u.eo.content.data, 0x00, WMS_UDH_EO_DATA_SEGMENT_MAX);

    pUdh->header_id = WMS_UDH_EXTENDED_OBJECT;

    if (firstSegment == TRUE)
    {
      pUdh->u.eo.content.length = 5;  /* max value is 124 */
      pUdh->u.eo.first_segment = TRUE;
      pUdh->u.eo.reference = 1;
      pUdh->u.eo.length = segment_total*5;
      pUdh->u.eo.control = 0;
      pUdh->u.eo.type = WMS_UDH_EO_VCARD;
      pUdh->u.eo.position = 0;
    }
    else
    {
      pUdh->u.eo.content.length = 5;  /* max value is 131 */
      pUdh->u.eo.first_segment = FALSE;
    }

    /* currently, set some dummy values for extended object data content */
    pUdh->u.eo.content.data[j++] = 1;
    pUdh->u.eo.content.data[j++] = 2;
    pUdh->u.eo.content.data[j++] = 3;
    pUdh->u.eo.content.data[j++] = 4;
    pUdh->u.eo.content.data[j++] = 5;
  }

  return TRUE;
}

static boolean WMSAPP_TestFillVarPic
(
  CWMSAPP *pMe,
  int n
)
{
  int j=0;
  wms_udh_s_type *pUdh=NULL;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[n];
  }
  else
  {
      pUdh = &pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[n];
  }

  if(pUdh == NULL) return FALSE;

  pUdh->header_id  = WMS_UDH_VAR_PICTURE;
  pUdh->u.var_picture.position = 7;
  pUdh->u.var_picture.width  = 40;
  pUdh->u.var_picture.height  = 7;

  pUdh->u.var_picture.data[j++]  = 0xf8;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;

  pUdh->u.var_picture.data[j++]  = 0x8a;
  pUdh->u.var_picture.data[j++]  = 0x92;
  pUdh->u.var_picture.data[j++]  = 0x3b;
  pUdh->u.var_picture.data[j++]  = 0xa2;
  pUdh->u.var_picture.data[j++]  = 0x20;

  pUdh->u.var_picture.data[j++]  = 0x8a;
  pUdh->u.var_picture.data[j++]  = 0xba;
  pUdh->u.var_picture.data[j++]  = 0x22;
  pUdh->u.var_picture.data[j++]  = 0xb7;
  pUdh->u.var_picture.data[j++]  = 0x60;

  pUdh->u.var_picture.data[j++]  = 0x8a;
  pUdh->u.var_picture.data[j++]  = 0xaa;
  pUdh->u.var_picture.data[j++]  = 0x22;
  pUdh->u.var_picture.data[j++]  = 0xaa;
  pUdh->u.var_picture.data[j++]  = 0xa0;

  pUdh->u.var_picture.data[j++]  = 0x9b;
  pUdh->u.var_picture.data[j++]  = 0xab;
  pUdh->u.var_picture.data[j++]  = 0xbb;
  pUdh->u.var_picture.data[j++]  = 0xa2;
  pUdh->u.var_picture.data[j++]  = 0x20;

  pUdh->u.var_picture.data[j++]  = 0xf8;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;

  pUdh->u.var_picture.data[j++]  = 0x10;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  pUdh->u.var_picture.data[j++]  = 0x00;
  return TRUE;
}


static boolean WMSAPP_TestFillLargeAnim
(
  CWMSAPP *pMe,
  int n
)
{
  int j=0;
  int bitmap_pos =0;

  wms_udh_s_type *pUdh=NULL;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[n];
  }
  else
  {
      pUdh = &pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[n];
  }

  if(pUdh == NULL) return FALSE;

  pUdh->header_id  = WMS_UDH_LARGE_ANIM;
  pUdh->u.large_anim.position = 7;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x11;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x10;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0xFC;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x3F;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x10;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  j=0;
  bitmap_pos++;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x20;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x04;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0xF8;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x1F;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x08;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x04;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  j=0;
  bitmap_pos++;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x02;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x20;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x04;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0xF0;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x0F;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x20;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x04;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x02;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  j=0;
  bitmap_pos++;


  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x02;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0xE0;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x07;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x02;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x00;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x03;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x80;

  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x05;
  pUdh->u.large_anim.data[bitmap_pos][j++] = 0x40;

  return TRUE;
}

static boolean WMSAPP_TestFillSmallAnim
(
  CWMSAPP *pMe,
  int n
)
{
  int j=0;
  int bitmap_pos=0;
  wms_udh_s_type *pUdh=NULL;

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[n];
  }
  else
  {
    pUdh = &pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[n];
  }

  if(pUdh == NULL) return FALSE;

  pUdh->header_id  = WMS_UDH_SMALL_ANIM;
  pUdh->u.small_anim.position = 7;

  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;

  bitmap_pos++;
  j=0;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x01;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x02;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x04;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x08;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x20;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x80;

  bitmap_pos++;
  j=0;

  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0xff;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x00;

  bitmap_pos++;
  j=0;

  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x80;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x40;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x20;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x10;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x08;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x04;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x02;
  pUdh->u.small_anim.data[bitmap_pos][j++] = 0x01;

  return TRUE;
}

static void WMSAPP_TestEndEms
(
  CWMSAPP *pMe
)
{
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    /* Initialized other to 0 */
    (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

    if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts ) != WMS_OK_S)
    {
      MSG_ERROR("Encoding failed!",0,0,0);
    }

    /* Prepare client msg*/
    pMe->m_pClient_msg_mo->msg_hdr.mem_store    = pMe->m_msgInfo.memStore;
    pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;
  }
  else
  {
    if(IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.gw_message.raw_ts_data ) != SUCCESS)
    {
      MSG_ERROR("Encoding failed!",0,0,0);
    }

    /* Prepare client msg*/
    pMe->m_pClient_msg_mo->msg_hdr.mem_store                        = pMe->m_msgInfo.memStore;
    pMe->m_pClient_msg_mo->msg_hdr.message_mode                     = WMS_MESSAGE_MODE_GW;
    pMe->m_pClient_msg_mo->u.gw_message.is_broadcast                = FALSE;
    pMe->m_pClient_msg_mo->u.gw_message.sc_address.digit_mode       = WMS_DIGIT_MODE_4_BIT;
    pMe->m_pClient_msg_mo->u.gw_message.sc_address.number_plan      = WMS_NUMBER_PLAN_TELEPHONY;
    pMe->m_pClient_msg_mo->u.gw_message.sc_address.number_type      = WMS_NUMBER_INTERNATIONAL;
    pMe->m_pClient_msg_mo->u.gw_message.sc_address.number_of_digits = 0;
  }

  if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
    pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
  else
    pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MT_NOT_READ;

}

static boolean WMSAPP_TestSmallPicture(CWMSAPP* pMe)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
  }

  if (WMSAPP_TestFillSmallPic(pMe,0))
  {
    WMSAPP_TestEndEms(pMe);

    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                     IDD_DESTADDR, NULL);
  }

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;

}


static boolean WMSAPP_TestLargePicture(CWMSAPP* pMe)
{

  uint16 ret_status = EFAILED;
  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
  }

  if(WMSAPP_TestFillLargePic(pMe,0))
  {
    WMSAPP_TestEndEms(pMe);

    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);

  }
  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestExtendedObject(CWMSAPP* pMe)
{

  uint16 ret_status = EFAILED;

  /* user data for the mesage */
  char *text = "This is a segmented extended object message! ";

  WMSAPP_TestInitEms(pMe);
  pMe->m_bsegmentEO = TRUE;

  
  if (pMe->m_pUserData != NULL)
  {
    WMSAPPU_FREE(pMe->m_pUserData->data);
    pMe->m_pUserData->data = NULL;
  

    pMe->m_pUserData->num_chars = (uint16)(STRLEN(text));
    pMe->m_pUserData->data = MALLOC(pMe->m_pUserData->num_chars);
    if (pMe->m_pUserData->data == NULL) {
      return FALSE;
    }

    (void)MEMCPY (pMe->m_pUserData->data, text, pMe->m_pUserData->num_chars);

    pMe->m_pUserData->encoding = WMSAPP_ENCODING_7_BIT;
    pMe->m_pUserData->num_headers = 6;

    if (pMe->m_pUserData->headers != NULL)
    {
      WMSAPPU_FREE(pMe->m_pUserData->headers);
      pMe->m_pUserData->headers = NULL;
      pMe->m_pUserData->headers = MALLOC(sizeof(wms_udh_s_type)*pMe->m_pUserData->num_headers);
      if (pMe->m_pUserData->headers == NULL) {
        return FALSE;
      }
    }
  }
  /* 3 segments in total, 2 headers per segment - concat 16 + extended object */
  if(WMSAPP_TestFillExtendedObject(pMe,0, 3) &&
     WMSAPP_TestFillExtendedObject(pMe,1, 3) &&
     WMSAPP_TestFillExtendedObject(pMe,2, 3) &&
     WMSAPP_TestFillExtendedObject(pMe,3, 3) &&
     WMSAPP_TestFillExtendedObject(pMe,4, 3) &&
     WMSAPP_TestFillExtendedObject(pMe,5, 3) )
  {
    WMSAPP_TestEndEms(pMe);
    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);
  }
  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}


static boolean WMSAPP_TestVarPicture(CWMSAPP *pMe)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
  }
  else
  {
   pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
  }

  if(WMSAPP_TestFillVarPic(pMe,0))
  {
    WMSAPP_TestEndEms(pMe);


    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);
  }

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestLargeAnim
(
  CWMSAPP *pMe
)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
  }

  if(WMSAPP_TestFillLargeAnim(pMe,0))
  {
    WMSAPP_TestEndEms(pMe);
    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);

  }
  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestSmallAnim
(
  CWMSAPP *pMe
)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
  }

  if(WMSAPP_TestFillSmallAnim(pMe,0))
  {
    WMSAPP_TestEndEms(pMe);
    ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);
  }

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestPredefAnim
(
  CWMSAPP *pMe
)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].header_id = WMS_UDH_PRE_DEF_ANIM;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.pre_def_anim.position =7;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.pre_def_anim.animation_number = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].header_id = WMS_UDH_PRE_DEF_ANIM;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.pre_def_anim.position =7;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.pre_def_anim.animation_number = 1;
  }

  WMSAPP_TestEndEms(pMe);

  ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestMiscEms
(
CWMSAPP *pMe
)
{
  uint16 ret_status = EFAILED;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 3;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 3;
  }

  if(WMSAPP_TestFillSmallAnim(pMe,0))
  {
     if(WMSAPP_TestFillVarPic(pMe,1))
     {
       if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
       {
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].header_id                       = WMS_UDH_TEXT_FORMATING;
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].u.text_formating.style_bold     = TRUE;
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].u.text_formating.font_size      =WMS_UDH_FONT_LARGE;
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].u.text_formating.alignment_type = WMS_UDH_DEFAULT_ALIGNMENT;
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].u.text_formating.start_position = 5;
         pMe->m_pClient_ts_mo->u.cdma.user_data.headers[2].u.text_formating.text_formatting_length = 3;
       }
       else
       {
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].header_id                   = WMS_UDH_TEXT_FORMATING;
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].u.text_formating.style_bold = TRUE;
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].u.text_formating.font_size  =WMS_UDH_FONT_LARGE;
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].u.text_formating.alignment_type
                                                                                              = WMS_UDH_DEFAULT_ALIGNMENT;
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].u.text_formating.start_position = 5;
         pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[2].u.text_formating.text_formatting_length = 3;
       }

       WMSAPP_TestEndEms(pMe);
       ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);
     }
  }

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestSegmentEms
(
  CWMSAPP *pMe
)
{
  uint8 i;
  uint16 ret_status = EFAILED;

  char *text = "This is a test message to verify segmentation. It will have all the possible headers in between with position in spaces of 10";

  /* Initialize the Client TS Structure */
  WMSAPP_TestInitEms(pMe);

  pMe->m_bUsepUserData = TRUE;

  /* Fill in the Compose Message Structure */
  pMe->m_pUserData->num_chars = (uint16)(STRLEN(text));
  if (pMe->m_pUserData->data != NULL)
  {
    WMSAPPU_FREE(pMe->m_pUserData->data);
    pMe->m_pUserData->data = NULL;
  }
  pMe->m_pUserData->data = MALLOC(pMe->m_pUserData->num_chars);
  if (pMe->m_pUserData->data == NULL) {
    return FALSE;
  }

  (void)MEMCPY (pMe->m_pUserData->data, text, pMe->m_pUserData->num_chars);

  pMe->m_pUserData->encoding = WMSAPP_ENCODING_7_BIT;
  pMe->m_pUserData->num_headers = 8;

  if (pMe->m_pUserData->headers != NULL)
  {
    WMSAPPU_FREE(pMe->m_pUserData->headers);
    pMe->m_pUserData->headers = NULL;
  }
  pMe->m_pUserData->headers = MALLOC(sizeof(wms_udh_s_type)*pMe->m_pUserData->num_headers);

  if (pMe->m_pUserData->headers == NULL) {
    return FALSE;
  }

  /* Fill up the headers */
  for (i=0; i<8; i++)
  {
    uint8 j,k;
    pMe->m_pUserData->headers[i].header_id = WMS_UDH_LARGE_ANIM;
    pMe->m_pUserData->headers[i].u.large_anim.position = (uint8)((i+1)*10);
    for (j=0; j<4; j++)
    {
      for (k=0; k<32; k++)
      {
        if ((j % 2)== 0)
        {
          pMe->m_pUserData->headers[i].u.large_anim.data[j][k] = 0x00;
        }
        else
        {
          pMe->m_pUserData->headers[i].u.large_anim.data[j][k] = 0xFF;
        }
      }
    }
  }
  /* Convert to Client Msg */
  WMSAPP_TestEndEms(pMe);
  ret_status = (uint16)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);
  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestMelody
(
CWMSAPP *pMe
)
{
  uint32 ret_status;

  WMSAPP_TestInitEms(pMe);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].header_id = WMS_UDH_PRE_DEF_SOUND;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.pre_def_sound.position =7;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.pre_def_sound.snd_number = 1;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].header_id = WMS_UDH_PRE_DEF_SOUND;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.pre_def_sound.position =7;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.pre_def_sound.snd_number = 1;
  }

  WMSAPP_TestEndEms(pMe);

  ret_status = (uint32)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

static boolean WMSAPP_TestUserMelody
(
CWMSAPP *pMe
)
{
  int size = MELODY_SIZE;
  IFileMgr *pIFilemgr;
  IFile *pIFile;
  uint32 ret_status;
  char *fileName = "ems/usermelody/test.imy";

  WMSAPP_TestInitEms(pMe);

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFilemgr) != SUCCESS)
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFilemgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].header_id = WMS_UDH_USER_DEF_SOUND;
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.user_def_sound.position =7;
    size = IFILE_Read(pIFile, pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.user_def_sound.user_def_sound, (uint32)size);
    pMe->m_pClient_ts_mo->u.cdma.user_data.headers[0].u.user_def_sound.data_length = (uint8)size;
  }
  else
  {
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.num_headers = 1;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].header_id = WMS_UDH_USER_DEF_SOUND;
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.user_def_sound.position =7;
    size = IFILE_Read(pIFile, pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.user_def_sound.user_def_sound, (uint32)size);
    pMe->m_pClient_ts_mo->u.gw_pp.u.submit.user_data.headers[0].u.user_def_sound.data_length = (uint8)size;
  }

  (void)IFILE_Release(pIFile);
  (void)IFILEMGR_Release(pIFilemgr);

  WMSAPP_TestEndEms(pMe);

  ret_status = (uint32)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_DESTADDR, NULL);

  if(ret_status ==EFAILED)
    return FALSE;

  return TRUE;
}

#endif /*FEATURE_SMS_UDH*/

static boolean WMSAPPMN_ResetSeqNum
(
 CWMSAPP *pMe
)
{
#ifdef FEATURE_CDSMS
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

   (void)IWMS_DbgResetTLSeqNum(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe);
#endif /*FEATURE_CDSMS*/

   return TRUE;
}

boolean WMSAPPMN_DcConnect
(
  CWMSAPP *pMe,
  wms_dc_so_e_type so
)
{
#ifdef FEATURE_CDSMS
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_DcConnect(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback, (void *)pMe,
                 so);
#endif
  return TRUE;
}

boolean WMSAPPMN_DcDisconnect
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_CDSMS
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_DcDisconnect(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback, (void *)pMe);
#endif
  return TRUE;
}

#ifdef FEATURE_CDSMS
static boolean WMSAPPMN_EnableAutoDisconnect
(
  CWMSAPP *pMe,
  uint32  timeout
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_DcEnableAutoDisconnect(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                              (void*)pMe, timeout);
  return TRUE;
}

static boolean WMSAPPMN_DisableAutoDisconnect
(
  CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_DcDisableAutoDisconnect(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                              (void*)pMe);
  return TRUE;
}
#endif /* FEATURE_CDSMS */

boolean WMSAPPMN_DeleteAllMsgs
(
 CWMSAPP *pMe,
 wms_memory_store_e_type mem_store
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  if(mem_store != WMS_MEMORY_STORE_NONE)
  {
    (void)IWMS_MsgDeleteAll(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, mem_store, WMS_TAG_NONE);
  }
  else /*Delete all for GW*/
  {
    (void)IWMS_MsgDeleteAll(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, WMS_MEMORY_STORE_NV_GW, WMS_TAG_NONE);

    (void)IWMS_MsgDeleteAll(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, WMS_MEMORY_STORE_RAM_GW, WMS_TAG_NONE);
    (void)IWMS_MsgDeleteAll(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                         (void*)pMe, WMS_MEMORY_STORE_SIM, WMS_TAG_NONE);
  }

  return TRUE;
}

boolean WMSAPPMN_DeleteAllTemplates
(
 CWMSAPP *pMe,
 wms_memory_store_e_type mem_store
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  (void)IWMS_MsgDeleteAllTemplate(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                            (void*)pMe, mem_store);
  return TRUE;
}

boolean WMSAPPMN_SetTestRoutes
(
 CWMSAPP *pMe
)
{
  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_0].route     = WMS_ROUTE_TRANSFER_AND_ACK;
  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_0].mem_store = WMS_MEMORY_STORE_NONE;


  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_1].route     = WMS_ROUTE_STORE_AND_NOTIFY;
  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_1].mem_store = WMS_MEMORY_STORE_NV_GW;


  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_2].route     = WMS_ROUTE_STORE_AND_NOTIFY;
  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_2].mem_store = WMS_MEMORY_STORE_SIM;


  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_3].route     = WMS_ROUTE_STORE_AND_NOTIFY;
  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_3].mem_store = WMS_MEMORY_STORE_NV_GW;


  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_NONE].route     = WMS_ROUTE_STORE_AND_NOTIFY;
  pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_NONE].mem_store = WMS_MEMORY_STORE_NV_GW;

  (void)IWMS_CfgSetRoutes(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                    (void*)pMe, &pMe->m_routes);
  return TRUE;
}


boolean WMSAPPMN_SendAllMsgs
(
  CWMSAPP *pMe
)
{
  int i,j;

  wms_client_message_s_type   msg;

  for (i = 0; i<WMS_MEMORY_STORE_MAX;i++)
  {
    pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
    pMe->m_msgInfo.memStore = (wms_memory_store_e_type)i;

    for (j = 0; j<pMe->m_pSmsLists[i]->len ; j++ )
    {
      if ( pMe->m_pSmsLists[i]->tags[j] == WMS_TAG_MO_NOT_SENT )
      {
        msg.msg_hdr.index       = pMe->m_pSmsLists[i]->indices[j];
        msg.msg_hdr.mem_store   = (wms_memory_store_e_type)i;

        CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
              (void*)&(pMe->m_cbData));

        (void)IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                    (void*)pMe, WMS_SEND_MODE_MEMORY_STORE,
                    &msg);


      }
    }
  }
  return TRUE;
}


boolean WMSAPPMN_PrepareGWMOMsg
(
  CWMSAPP *pMe,
  uint8 numberOfChars
)
{

#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  return TRUE;
}

boolean WMSAPPMN_PrepareCDMAMOMsg
(
  CWMSAPP *pMe,
  uint8 numberOfChars
)
{
#ifdef FEATURE_CDSMS
    uint8 i;
  uint8 buf[WMS_MAX_LEN];
      (void)IWMS_TsAsciiToDtmf( pMe->m_pwms, (char *)pMe->m_pClient_msg_mo->u.cdma_message.address.digits,
                    (uint8 *)pMe->m_pClient_msg_mo->u.cdma_message.address.digits);

  pMe->m_pClient_msg_mo->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;

  /* build the message */
  pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
  pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_service_present = FALSE;
  pMe->m_pClient_msg_mo->u.cdma_message.subaddress.number_of_digits = 0;
  pMe->m_pClient_msg_mo->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;

  pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;
  pMe->m_pClient_ts_mo->u.cdma.mask = WMS_MASK_BD_MSG_ID    |
                                    WMS_MASK_BD_USER_DATA;
  pMe->m_pClient_ts_mo->u.cdma.message_id.id_number = 0xFFFF;
  pMe->m_pClient_ts_mo->u.cdma.message_id.type = WMS_BD_TYPE_SUBMIT;
  pMe->m_pClient_ts_mo->u.cdma.num_messages = 1;

  pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_ASCII;
  pMe->m_pClient_ts_mo->u.cdma.user_data.data_len = (uint8)numberOfChars;
  pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits = (uint8)numberOfChars;

  /* fill the user data with 1's */
  for (i=0; i<numberOfChars; i++)
  {
    buf[i] = '1';
  }

  (void)IWMS_TsPackAscii( pMe->m_pwms, (char *)buf,
                         (uint8*)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                         (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data_len,
                         (uint8 *)&pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits);

  /* Initialized other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  if (IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts)
              != SUCCESS)
  {
    MSG_ERROR(" Encoding test msg failed!", 0,0,0);
    return FALSE;
  }

  pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;

#endif /*FEATURE_CDSMS*/
  return TRUE;
}

boolean WMSAPPMN_PrepareCDMAMTMsg
(
  CWMSAPP *pMe,
  uint8 numberOfChars,
  wms_msg_event_info_s_type * msg_info
)
{
#ifdef FEATURE_CDSMS
    uint8 i;

    if(msg_info == NULL)
      return FALSE;

  /* build the message */
  msg_info->mt_message_info.message.u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
  msg_info->mt_message_info.message.u.cdma_message.is_mo = FALSE;
  msg_info->mt_message_info.message.u.cdma_message.is_tl_ack_requested = FALSE;
  msg_info->mt_message_info.message.u.cdma_message.is_service_present = FALSE;

  msg_info->mt_message_info.message.msg_hdr.tag = WMS_TAG_MT_NOT_READ;
  msg_info->mt_message_info.route = WMS_ROUTE_TRANSFER_ONLY;

  msg_info->mt_message_info.message.u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
  msg_info->mt_message_info.message.u.cdma_message.raw_ts.tpdu_type = WMS_TPDU_DELIVER;

  msg_info->mt_message_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_info->mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
  msg_info->mt_message_info.transaction_id = 90;

  msg_info->mt_message_info.message.u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;

  /* build the message */
  pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_WEMT;
  pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_service_present = FALSE;
  pMe->m_pClient_msg_mo->u.cdma_message.subaddress.number_of_digits = 0;
  pMe->m_pClient_msg_mo->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
  pMe->m_pClient_msg_mo->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;

  pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;
  pMe->m_pClient_ts_mo->u.cdma.mask = WMS_MASK_BD_MSG_ID    |
                                    WMS_MASK_BD_USER_DATA;
  pMe->m_pClient_ts_mo->u.cdma.message_id.id_number = 0xFFFF;
  pMe->m_pClient_ts_mo->u.cdma.message_id.type = WMS_BD_TYPE_DELIVER;
  pMe->m_pClient_ts_mo->u.cdma.num_messages = 1;

  pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_ASCII;
  pMe->m_pClient_ts_mo->u.cdma.user_data.data_len = (uint8)numberOfChars;

  /* fill the user data with 1's */
  for (i=0; i<numberOfChars; i++)
  {
    pMe->m_pClient_ts_mo->u.cdma.user_data.data[i] = '1';
  }

  /* Initialized other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  if (IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &msg_info->mt_message_info.message.u.cdma_message.raw_ts)
               != SUCCESS)
  {
    MSG_ERROR(" Encoding test msg failed!", 0,0,0);
    return FALSE;
  }

  pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;


  WMSAPPU_FREE(msg_info);

#endif /*FEATURE_CDSMS*/
  return TRUE;
}


boolean WMSAPPMN_PrepareCDMAMOMsgForAccessChannel
(
  CWMSAPP *pMe,
  uint8 numberOfChars
)
{
#ifdef FEATURE_CDSMS

    pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA;
    pMe->m_msgInfo.smsIndex = 0;

  /* build the message */
  pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
  pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
  pMe->m_pClient_msg_mo->u.cdma_message.is_service_present = FALSE;

  pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;
  pMe->m_pClient_ts_mo->u.cdma.mask = WMS_MASK_BD_MSG_ID    |
                                    WMS_MASK_BD_USER_DATA;
  pMe->m_pClient_ts_mo->u.cdma.message_id.id_number = 0xFFFF;
  pMe->m_pClient_ts_mo->u.cdma.message_id.type = WMS_BD_TYPE_SUBMIT;
  pMe->m_pClient_ts_mo->u.cdma.num_messages = 1;

  pMe->m_pClient_ts_mo->u.cdma.user_data.encoding = WMS_ENCODING_ASCII;

  /* Initialized other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  if (IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, &pMe->m_pClient_msg_mo->u.cdma_message.raw_ts)
              != SUCCESS)
  {
    MSG_ERROR(" Encoding test msg failed!", 0,0,0);
    return FALSE;
  }

  pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
  pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;

#endif /*FEATURE_CDSMS*/
  (void)WMSAPPMN_SendMessage(pMe);
  return TRUE;
}


void WMSAPPMN_ModifyVolume
(
  CWMSAPP *pMe,
  uint16 key
)
{
  boolean VolumeChange = FALSE;

  /* Check for Invalid combinations at boundary */

  if ((pMe->m_BeepVolume == 0) && (key == AVK_LEFT))
  {
    /* Cannot Decrement Volume Further */
  }
  else if ((pMe->m_BeepVolume == 4) && (key == AVK_RIGHT))
  {
    /* Cannot Increment Volume Further */
  }
  else
  {

    switch(key)
    {
      case AVK_LEFT:
        pMe->m_BeepVolume--;
        break;

      case AVK_RIGHT:
        pMe->m_BeepVolume++;
        break;

      case AVK_SELECT:
        /* Goes Incrementing in a circular fashion */
        if (pMe->m_BeepVolume == 4)
          pMe->m_BeepVolume = 0;
        else
          pMe->m_BeepVolume++;
        break;

      default:
        break;
    }
    VolumeChange = TRUE;
  }

  if (VolumeChange == TRUE)
  {
    /* Update IConfig */
    uint8 cfgVal = pMe->m_BeepVolume;
    (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_VOLUME, &cfgVal, 1);

    /* Update Display */
    (void)WMSAPPMN_InitAlertSettingsDlg(pMe);
  }
}

boolean WMSAPPMN_SendEvtToDlg
(
  CWMSAPP *pMe,
  AEEEvent eCode,
  uint16 wParam,
  uint32 dwParam
)
{
  IDialog *pIDialog;
  uint16 wDlgID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  /* there can be no Active dialog at this point! */

  if(pIDialog)
  {  /* if there is an existing dialog, pass the event to the dialog */
    wDlgID = IDIALOG_GetID(pIDialog);

    if (wDlgID >= ARR_SIZE(SMSDlgEventHandlers) || wDlgID == 0)
    {
      return FALSE;
    }

    if(wDlgID == 0)
    {
      return FALSE; /* error */
    }

    /* find the appropriate event handler */
    if(wDlgID < ARR_SIZE(SMSDlgEventHandlers))
    { /* check for array bound */
      if(SMSDlgEventHandlers[wDlgID] != NULL)
      {
        if(SMSDlgEventHandlers[wDlgID](pMe, eCode, wParam, dwParam))
        {
          /* event handled by dialog event handler */
          return TRUE;
        }
        else
        { /* dialog didn't handle this event, require app to handle it */
          /* require handling by app's event handler */
          return FALSE;
        }
      }
    }
    return FALSE; /* error */
  }
  else
  {
    return FALSE;
  }
}
/*-----------------------------------EVENT HANDLERS-------------------------------*/

/*===========================================================================
FUNCTION WMSAPPMN_IdleDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_IdleDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;

#ifdef FEATURE_GWSMS
#error code not present
#endif

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    //(void)ISHELL_CancelTimer(pMe->a.m_pIShell, DisplaySMSIdleMenuDlg, pMe);
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      return (WMSAPPMN_InitIdleMenuDlg(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_KEY_PRESS:
    case EVT_KEY_RELEASE:
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      if(w == IDL_SMS_VOICEMAIL)
      {
        if(pMe->m_bTestMode)
        {
          if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
          {
            if(pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->voice_mail_active)
            {
              pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NV_CDMA;
              pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->voice_mail_index;
              (void)WMSAPPMN_ReadMsg(pMe);
            }
          }
          else
          {
            if(pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->voice_mail_active)
            {
              pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_SIM;
              pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->voice_mail_index;
              (void)WMSAPPMN_ReadMsg(pMe);
            }
          }
          return TRUE;
        }
      }
#ifdef FEATURE_GWSMS      
#error code not present
#endif /* FEATURE_GWSMS */
      else if(w == IDL_SK_ABOUT)
      {
        return(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_ABOUT, NULL));
      }
      (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      return TRUE;

    case EVT_DIALOG_END:
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE;

    case EVT_WMS_MSG_READ: /*VoiceMail Read Event*/
      pMe->m_msgInfo.boxType = WMSAPP_INBOX;
      return (WMSAPPMN_ViewMsg(pMe, (wms_msg_event_info_s_type *)dw));

    case EVT_WMSAPP_MESSAGE_READ:
      {
        uint16 wDlgID=0;

        if (w == 0)
        {
          /* Read Normal PP Messages */
          pMe->m_msgInfo.memStore = ((wms_client_msg_hdr_s_type *)dw)->mem_store;

          switch(pMe->m_msgInfo.memStore)
          {
            case WMS_MEMORY_STORE_RAM_GW:
              wDlgID = IDD_TEMP_MESSAGE_ACCESS;
              break;

            case WMS_MEMORY_STORE_SIM:
            case WMS_MEMORY_STORE_RUIM:
              wDlgID = IDD_CARD_MESSAGE_ACCESS;
              break;

            case WMS_MEMORY_STORE_NV_GW:
            case WMS_MEMORY_STORE_NV_CDMA:
              wDlgID = IDD_NV_MESSAGE_ACCESS;
              break;

            default:
              break;
          }
        }
        else
        {
          /* Read Broadcast SMS Messages */
          wDlgID = IDD_BROADCAST_MESSAGE_ACCESS;
        }

        if(ISHELL_CreateDialog (pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                wDlgID, NULL) == EFAILED)
        {
          /* Creation failed */
          return FALSE;
        }
        (void)ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                            EVT_WMSAPP_MESSAGE_READ, w, dw);
      }
      return TRUE;

    case EVT_WMSAPP_MESSAGE_COMPOSE:
      pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
      pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
      pMe->m_msg_type        = WMSAPP_MSG_TYPE_NEW;
      pMe->m_currPref        = WMSAPP_NO_PREF;
      if(ISHELL_CreateDialog (pMe->a.m_pIShell, WMSAPP_RES_FILE,
                              IDD_MESSAGE_TYPE, NULL) == EFAILED)
      {
        /* Creation failed */
        return FALSE;
      }
      (void)ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                          EVT_WMSAPP_MESSAGE_COMPOSE, w, dw);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_IdleDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_MenuDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;



  switch(evt)
  {

    case EVT_DIALOG_START:
      return(WMSAPPMN_InitMenuDlg(pMe,(IDialog*)dw));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        result = (SUCCESS == ISHELL_EndDialog(pMe->a.m_pIShell));
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return result;
      }
      if(w == AVK_SELECT)
      {
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;


    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_MenuDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_SettingsDlgEventHandler

DESCRIPTION
  Handles all the Settings Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_SettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitSettingsDlg(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)WMSAPPMN_EndDlg(pMe);
      }
      return TRUE; /* Digit keys should be ignored */

    case EVT_COMMAND:
      switch(w)
      {
        case IDL_USE_AUTO_SELECT:
          (void)WMSAPPMN_SetCheckBoxItems(pMe, FALSE);
          break;

        default:
          (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
          break;
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_SettingsDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_AlertSettingsDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_AlertSettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  IMenuCtl *pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog,(int16)wCtlID);

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitAlertSettingsDlg(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)WMSAPPMN_EndDlg(pMe);
      }
      if ((w == AVK_LEFT)||(w == AVK_RIGHT)||(w == AVK_SELECT))
      {
        if (IMENUCTL_GetSel(pIMenu) == IDL_ALERT_VOLUME)
        {
          WMSAPPMN_ModifyVolume(pMe, w);
        }
      }
      return TRUE; /* Digit keys should be ignored */

    case EVT_COMMAND:
      /* Add Code here for Handling various events */
      switch(IMENUCTL_GetSel(pIMenu))
      {
        case IDL_ALERT_DISPLAY:
          if (pMe->m_bDisplayAlert == TRUE)
          {
            uint8 cfgVal = OEMNV_SMS_ALERT_DISPLAY_DISABLE;
            (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_DISPLAY, &cfgVal, 1);
            pMe->m_bDisplayAlert = FALSE;
          }
          else
          {
            uint8 cfgVal = OEMNV_SMS_ALERT_DISPLAY_ENABLE;
            (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_DISPLAY, &cfgVal, 1);
            pMe->m_bDisplayAlert = TRUE;
          }
          /* Update Display */
          (void)WMSAPPMN_InitAlertSettingsDlg(pMe);
          break;

        case IDL_ALERT_BEEP:
          if (pMe->m_bBeepAlert == TRUE)
          {
            uint8 cfgVal = OEMNV_SMS_ALERT_SOUND_DISABLE;
            (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_SOUND, &cfgVal, 1);
            pMe->m_bBeepAlert = FALSE;
          }
          else
          {
            uint8 cfgVal = OEMNV_SMS_ALERT_SOUND_ENABLE;
            (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_ALERT_SOUND, &cfgVal, 1);
            pMe->m_bBeepAlert = TRUE;
          }
          /* Update Display */
          (void)WMSAPPMN_InitAlertSettingsDlg(pMe);
          break;

        case IDL_ALERT_VOLUME:
          WMSAPPMN_ModifyVolume(pMe, AVK_SELECT);
          break;

        default:
          break;
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_AlertSettingsDlgEventHandler */



/*===========================================================================
FUNCTION WMSAPPMN_AbsValDlgEventHandler

DESCRIPTION
  Handle Absolute Validity Menu Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_AbsValDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;

  switch(evt)
  {

    case EVT_DIALOG_START:
      return(WMSAPPMN_InitAbsValDlg(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        return (WMSAPPMN_EndDlg(pMe));
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_AbsValDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_MsgAccessDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MsgAccessDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;
  IMenuCtl* pIMenu;
  IDialog *pIDialog;
  uint16 wCtlID;
  uint16 wItemID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)wCtlID);
  

  switch(evt)
  {

    case EVT_DIALOG_START:
      if(pMe->m_bTestMode)
      {
        pMe->m_msgInfo.boxType = WMSAPP_INBOX;
      }
      //(void)WMSAPPMN_InitMenuSoftKey(pMe);
      return (WMSAPPMN_SetMsgListDisplay(pMe, pMe->m_msgInfo.boxType));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return result;
      }
      if(w == AVK_SELECT)
      {
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      {
        IMenuCtl *pIMenuMsgList;

        if(pMe->m_bTestMode)
        {
          wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);

          pIMenuMsgList = (IMenuCtl*) IDIALOG_GetControl(pIDialog,
                                                         (int16)wCtlID);

          wItemID = IMENUCTL_GetSel(pIMenuMsgList);

          return (WMSAPPMN_MapNReadMsg(pMe, wItemID));
        }
        return TRUE;
      }


    case EVT_CTL_SEL_CHANGED:
      if(pMe->m_bTestMode)
      {
        wItemID = IMENUCTL_GetSel(pIMenu);

        switch(wItemID)
        {
          case IDL_SK_CARD_INBOX:
          case IDL_SK_NV_INBOX:
          case IDL_SK_TEMP_INBOX:
            pMe->m_msgInfo.boxType= WMSAPP_INBOX;
            break;

          case IDL_SK_CARD_OUTBOX:
          case IDL_SK_NV_OUTBOX:
          case IDL_SK_TEMP_OUTBOX:
            pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
            break;

          case IDL_SK_CARD_STATUS_RPT:
            pMe->m_msgInfo.boxType = WMSAPP_STATUS_REPORT;
            break;

          case IDL_SK_CARD_USAGE:
          case IDL_SK_NV_USAGE:
          case IDL_SK_TEMP_USAGE:
            if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                               IDD_USAGE, NULL) == EFAILED)
            {
              /* Creation failed */
              return FALSE;
            }
            return TRUE;

          default:
            break;
        }
        (void)WMSAPPMN_SetMsgListDisplay(pMe,pMe->m_msgInfo.boxType);
      }
      return TRUE;

    case EVT_WMS_MSG_READ:
    case EVT_WMS_MSG_READ_STATUS_REPORT:
      return (WMSAPPMN_ViewMsg(pMe, (wms_msg_event_info_s_type *)dw));

    case EVT_DIALOG_END:
      return TRUE;

    case EVT_WMS_CFG_MESSAGE_LIST:
      return (WMSAPPMN_SetMsgListDisplay(pMe, pMe->m_msgInfo.boxType));

    case EVT_WMS_DLG_RESUME:
      switch(pMe->m_msgInfo.memStore)
      {
        case WMS_MEMORY_STORE_RAM_CDMA:
        case WMS_MEMORY_STORE_RAM_GW:
          if (pMe->m_msgInfo.boxType == WMSAPP_INBOX)
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_TEMP_INBOX);
          }
          else
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_TEMP_OUTBOX);
          }
          break;

        case WMS_MEMORY_STORE_SIM:
        case WMS_MEMORY_STORE_RUIM:
          if (pMe->m_msgInfo.boxType == WMSAPP_INBOX)
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_CARD_INBOX);
          }
          else
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_CARD_OUTBOX);
          }
          break;

        case WMS_MEMORY_STORE_NV_CDMA:
        case WMS_MEMORY_STORE_NV_GW:
          if (pMe->m_msgInfo.boxType == WMSAPP_INBOX)
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_NV_INBOX);
          }
          else
          {
            IMENUCTL_SetSel(pIMenu, IDL_SK_NV_OUTBOX);
          }
          break;

        default:
          break;
      }
      return TRUE;

    case EVT_WMSAPP_MESSAGE_READ:
      pMe->m_msgInfo.memStore = ((wms_client_msg_hdr_s_type*)dw)->mem_store;
      pMe->m_msgInfo.smsIndex = (uint8)(((wms_client_msg_hdr_s_type*)dw)->index);
      (void)WMSAPPMN_ReadMsg(pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_MsgAccessDlgEventHandler */


boolean WMSAPPMN_InitMsgType(CWMSAPP *pMe)
{
  IDialog* pIDialog;
  uint16 wCtrlID;
  IMenuCtl* pIMenu;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL)
  {
    /*error*/
    return FALSE;
  }

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  /* Delete the contents and add the appropriate options for message type */
  if(IMENUCTL_DeleteAll(pIMenu) != TRUE)
  {
    MSG_HIGH("Message type list delete all failed!",0,0,0);
    return FALSE;
  }
  (void)IMENUCTL_Redraw(pIMenu);


  if(pMe->m_msg_type != WMSAPP_MSG_TYPE_REPLY)
  {
    /* Add phone number */
    if(IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE, IDS_MESSAGE_TYPE_PHONE,
                        IDL_MSG_TYPE_PHONE_NUMBER, NULL, 0) != TRUE)
    {
      MSG_HIGH("Phone number could not be added to menu!",0,0,0);
      return FALSE;
    }

    if((pMe->m_modeControl == WMSAPP_MODE_GW)
       || (pMe->m_modeControl == WMSAPP_MODE_CDMA)
      )
    {
      /* Add e-mail address */
      if(IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE, IDS_MESSAGE_TYPE_EMAIL,
                          IDL_MSG_TYPE_EMAIL_ADDRESS, NULL, 0) != TRUE)
      {
        MSG_HIGH("E-mail could not be added to menu!",0,0,0);
        return FALSE;
      }
    }

#ifdef FEATURE_CDSMS
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
      if((pMe->m_currPref != IDL_C_TMPLT_CALLBACK) &&
         (pMe->m_currPref != IDL_OUTBOX_CALLBACK))
      {
        /* Add non-std email address */
        if(IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,
                            IDS_MESSAGE_TYPE_NON_STD_EMAIL,
                            IDL_MSG_TYPE_NON_STD_EMAIL, NULL, 0) != TRUE)
        {
          MSG_HIGH("Non-std e-mail could not be added to menu!",0,0,0);
          return FALSE;
        }
      }
    }
#endif /* FEATURE_CDSMS */
  }

  else
  {
    uint16 wResID = 0, nItemID =0;
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
    /* special handling of reply; only show the addr type of the message
    ** replying to.
    */

    switch(pMe->m_pClient_msg->u.cdma_message.address.digit_mode)
    {
      case WMS_DIGIT_MODE_8_BIT:
        if(pMe->m_pClient_msg->u.cdma_message.address.number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK &&
           pMe->m_pClient_msg->u.cdma_message.address.number_type == WMS_NUMBER_INTERNATIONAL)
        {
          wResID = IDS_MESSAGE_TYPE_PHONE;
          nItemID = IDL_MSG_TYPE_PHONE_NUMBER;
        }
        else
        {
          wResID = IDS_MESSAGE_TYPE_EMAIL;
          nItemID = IDL_MSG_TYPE_EMAIL_ADDRESS;
        }
        break;

      case WMS_DIGIT_MODE_4_BIT:
      default:
#ifdef FEATURE_CDSMS
        /* first check if this is a non-std email address */
        if(WMSAPPU_IsNonStdEmail(TRUE,
                                 pMe->m_pClient_msg->u.cdma_message.address.digits,
                                 pMe->m_pClient_msg->u.cdma_message.address.number_of_digits))
        {
          wResID = IDS_MESSAGE_TYPE_NON_STD_EMAIL;
          nItemID = IDL_MSG_TYPE_NON_STD_EMAIL;
        }
        else
#endif /* FEATURE_CDSMS */
        {
          wResID = IDS_MESSAGE_TYPE_PHONE;
          nItemID = IDL_MSG_TYPE_PHONE_NUMBER;
        }
        break;
    }
    if(IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE,
                        wResID,
                        nItemID, NULL, 0) != TRUE)
    {
      MSG_HIGH("Addr type could not be added for REPLY!",0,0,0);
      return FALSE;
    }
#endif /* FEATURE_CDSMS */
    }
    else //FEATURE_GWSMS
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif
    }
  }

  return TRUE;
}
/*===========================================================================
FUNCTION WMSAPPMN_MsgTypeDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MsgTypeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;
  IDialog* pIDialog;
  uint16 wCtrlID;
  IMenuCtl* pIMenu;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL)
  {
    /*error*/
    return FALSE;
  }

  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

  switch(evt)
  {

    case EVT_DIALOG_START:
      if(!(WMSAPPMN_InitMsgType(pMe)))
      {
        return FALSE;
      }
      return(WMSAPPMN_InitMenuDlg(pMe,(IDialog*)dw));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        /* If the message type is reply, then go back to the main menu, to
        ** re-synchronize the app.
        */
        if(pMe->m_msg_type == WMSAPP_MSG_TYPE_REPLY)
        {
          return(WMSAPPMN_DisplayMenuDlg(pMe, IDD_MAINSMS));
        }

        result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
        (void)WMSAPPMN_CheckReturnDlg(pMe);

        return result;
      }
      if(w == AVK_SELECT)
      {
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      {
        uint16 wItemID;
        uint16 wDlgID;

        wItemID = IMENUCTL_GetSel(pIMenu);

        if(wItemID == 0)
        {
          MSG_HIGH("Invalid item ID from menu control!",0,0,0);
        }

        switch(wItemID)
        {
          case IDL_MSG_TYPE_PHONE_NUMBER:
            pMe->m_addrType = WMSAPP_MSG_TYPE_PHONE_NUM;
            break;
          case IDL_MSG_TYPE_EMAIL_ADDRESS:
            pMe->m_addrType = WMSAPP_MSG_TYPE_EMAIL;
            break;
          case IDL_MSG_TYPE_NON_STD_EMAIL:
            pMe->m_addrType = WMSAPP_MSG_TYPE_NON_STD_EMAIL;
            break;
          default:
            break;
        }
        WMSAPPMN_StopImages(pMe);
        wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, wItemID);

        if (wDlgID == 0)
        {
          return FALSE;
        }

        if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                               wDlgID, NULL) == EFAILED)
        { /* Creation failed */

          return FALSE;
        }

      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    case EVT_WMSAPP_MESSAGE_COMPOSE:
      if ((boolean)w == TRUE)
      {
        /* Is an Email Address */
        pMe->m_addrType = WMSAPP_MSG_TYPE_EMAIL;
      }
      else
      {
        /* NOT an Email Address */
        pMe->m_addrType = WMSAPP_MSG_TYPE_PHONE_NUM;
      }
      if(ISHELL_CreateDialog (pMe->a.m_pIShell, WMSAPP_RES_FILE,
                              IDD_DESTADDR, NULL) == EFAILED)
      {
        /* Creation failed */
        return FALSE;
      }
      pMe->m_bComposeMessage = TRUE;
      pMe->m_composeAddress = (char *)dw; /* Pointer assignment */
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSMsgTypeDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_ComposeDlgEventHandler

DESCRIPTION
  handle the compose screen dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ComposeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wDlgID;
  uint16 wItemID;
  IDialog* pIDialog;
  boolean result;
#ifdef FEATURE_SMS_UDH
  uint16 i;
#endif
  ITextCtl* pIText;
  boolean setTextFormat = FALSE;

  uint16 pos;
  AECHAR *text;
  char realData[2];

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
    return FALSE;

  pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, IDC_MSG_TEXT);
  switch(evt)
  {

    case EVT_DIALOG_START:
      /* init the num of headers */
      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers = 0;
        pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
#endif /*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
      }

      pMe->m_dispInfo.length = 0;
      pMe->m_dispInfo.handleEvent = FALSE;
      pMe->m_dispInfo.isSpace = FALSE;
      pMe->m_dispInfo.isCaps = FALSE;
      return(WMSAPPMN_InitComposeMsg(pMe));
      /* no menu control ---> error */

    case EVT_KEY:

      switch(w)
      {
        case AVK_CLR:
          result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          return result;

        case AVK_SELECT:
          pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, IDC_MSG_TEXT);
          pMe->m_bEditMsg = FALSE;

#ifdef FEATURE_SMS_UDH
          //Stop animation if any is attached to the message
          for(i = 0; i<WMS_MAX_UD_HEADERS; i++) {
            if(pMe->m_pCurImage[i] != NULL)
              IIMAGE_Stop(pMe->m_pCurImage[i]);
          }
#endif /* FEATURE_SMS_UDH */
          if(pMe->m_styleMask != WMSAPP_STYLE_FORMAT_NONE)
          {
            pMe->m_styleMask = WMSAPP_STYLE_FORMAT_NONE;
            (void)WMSAPPMN_SetUserDataStyleText(pMe,FALSE);
          }

          ITEXTCTL_SetActive(pIText,FALSE);

          if (WMSAPPMN_StoreText(pMe) == TRUE)
          {
            if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                  IDD_OUTBOX_OPTIONS, NULL) == EFAILED)
            { /* Creation failed */
              return FALSE;
            }
          }
          return TRUE;

        default:
          break;
      }
      return TRUE; // Digit keys should be ignored*/

   case EVT_COMMAND:
      pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,IDC_SK_COMPOSE);
      if(!IMENUCTL_IsActive(pICurrentMenu))
        return FALSE;

      wItemID = IMENUCTL_GetSel(pICurrentMenu);
      switch(wItemID)
      {
        case IDL_SK_CAPS:
          if(pMe->m_dispInfo.isCaps)
          {
            pMe->m_dispInfo.isCaps = FALSE;
          }
          else
          {
            pMe->m_dispInfo.isCaps = TRUE;
          }
          return TRUE;

        case IDL_SK_BOLD:
          if(pMe->m_curFont == AEE_FONT_BOLD)/*Key was set, it is now being turned off*/
          {
            pMe->m_styleMask &= ~WMSAPP_STYLE_FORMAT_BOLD;
            setTextFormat =FALSE;
            pMe->m_curFont = AEE_FONT_NORMAL;
          }
          else/*Bold is set*/
          {
            pMe->m_curFont = AEE_FONT_BOLD;
            pMe->m_styleMask |= WMSAPP_STYLE_FORMAT_BOLD;
            setTextFormat =TRUE;
          }
          break;

        case IDL_SK_ITALIC:
          if(pMe->m_curFlags & IDF_TEXT_INVERTED) //Key has been turned off
          {
            pMe->m_curFlags &= ~IDF_TEXT_INVERTED;
            pMe->m_styleMask &= ~WMSAPP_STYLE_FORMAT_ITALIC;
            setTextFormat =FALSE;
          }
          else /*Italic is set*/
          {
            pMe->m_curFlags |= IDF_TEXT_INVERTED;
            pMe->m_styleMask |= WMSAPP_STYLE_FORMAT_ITALIC;
            setTextFormat =TRUE;
          }
          break;

        case IDL_SK_STRIKETHROUGH:
          // No Support
          return TRUE;

        case IDL_SK_UNDERLINE:
          if(pMe->m_curFlags & IDF_TEXT_UNDERLINE) //Key has been turned on
          {
            pMe->m_curFlags &= ~IDF_TEXT_UNDERLINE;
            pMe->m_styleMask &= ~WMSAPP_STYLE_FORMAT_UNDERLINE;
            setTextFormat =FALSE;
          }
          else/*Underline is set*/
          {
            pMe->m_curFlags |= IDF_TEXT_UNDERLINE;
            pMe->m_styleMask |= WMSAPP_STYLE_FORMAT_UNDERLINE;
            setTextFormat =TRUE;
          }
          break;

        case IDL_SK_FONT_SIZE: /*Only Normal and Large sizes are supported by BREW*/
          if(pMe->m_curFont == AEE_FONT_LARGE)
          {
            pMe->m_curFont = AEE_FONT_NORMAL;
            pMe->m_styleMask &= ~WMSAPP_STYLE_FORMAT_SIZE;
            setTextFormat =FALSE;
          }
          else /*Large is set*/
          {
            pMe->m_curFont = AEE_FONT_LARGE;
            pMe->m_styleMask |= WMSAPP_STYLE_FORMAT_SIZE;
            setTextFormat =TRUE;
          }
          break;

        case IDL_SK_LEFT_ALIGN:
          // No Support
          break;

        case IDL_SK_RIGHT_ALIGN:
          // No Support
          break;

        case IDL_SK_CENTER_ALIGN:
          // No Support
          break;


        case IDL_SK_ATTACH:
#ifdef FEATURE_SMS_UDH
          WMSAPPMN_StopImages(pMe);
#endif /* FEATURE_SMS_UDH */
          wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, wItemID);

          if(wDlgID == 0)
          { /* error */
            return FALSE;
          }

          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 wDlgID, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
          IMENUCTL_SetSelEx(pICurrentMenu, wItemID,FALSE);
          return TRUE;

        default:
          break;
      }

      //Some Style Text was Selected.
      (void)WMSAPPMN_SetUserDataStyleText(pMe,setTextFormat);
      return TRUE;

    case EVT_DIALOG_END:
      WMSAPPMN_StopImages(pMe);
      pMe->m_bEditMsg = FALSE;
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;

    /* Refresh */
    case EVT_WMS_DLG_RESUME:
      WMSAPPMD_SetScreen(pMe);
      pMe->m_bEditMsg = TRUE;
      (void)MEMCPY(pMe->m_pClient_ts,pMe->m_pClient_ts_mo,sizeof(wms_client_ts_data_s_type));
      (void)WMSAPPMD_Display(pMe);
      /* User pressed back key so unpack the data again only for CDMA */
      if ( pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
        #ifdef FEATURE_CDSMS
        uint8   temp_data[ WMS_CDMA_USER_DATA_MAX ];

        if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_USER_DATA)
        {
          if(pMe->m_pClient_ts->u.cdma.user_data.encoding == WMS_ENCODING_IA5 ||
             pMe->m_pClient_ts->u.cdma.user_data.encoding == WMS_ENCODING_ASCII)
          {
            pMe->m_pClient_ts->u.cdma.user_data.data_len = IWMS_TsUnpackAscii(pMe->m_pwms,
                                                               &pMe->m_pClient_ts->u.cdma.user_data,
                                                               pMe->m_pClient_ts->u.cdma.user_data.number_of_digits+1,
                                                               (uint8 *)temp_data);

            (void)MEMCPY(pMe->m_pClient_ts->u.cdma.user_data.data, temp_data, pMe->m_pClient_ts->u.cdma.user_data.data_len);
            (void)MEMCPY(pMe->m_pClient_ts_mo,pMe->m_pClient_ts,sizeof(wms_client_ts_data_s_type));
          }
          if (pMe->m_pClient_ts->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE)
          {
            WMSAPPU_ConvertFromUnicode( &pMe->m_pClient_ts->u.cdma);
            (void)MEMCPY(pMe->m_pClient_ts_mo,pMe->m_pClient_ts,sizeof(wms_client_ts_data_s_type));
          }
        } /* end pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_USER_DATA */
        #endif
      } /* end outer if */

#ifdef FEATURE_GWSMS
#error code not present
#endif
      return TRUE;

    case EVT_KEY_PRESS:
    case EVT_KEY_RELEASE:
      /*For every key that is pressed, we get two events
      We need to handle both the events
      The keycode is determined from the hidden textControl
      The algorithm to determine the keycode is based on the following example -

      if "2" is pressed
      EVT               wParam         KeyCode
      EVT_KEY_PRESSED   AVK_2          A
      EVT_KEY_RELEASED  INVALID        a

      if "2" is pressed again
      EVT               wParam         KeyCode
      EVT_KEY_PRESS     AVK_2          a
      EVT_KEY_RELEASE   INVALID        b

      Now if "3" is pressed
      EVT               wParam         KeyCode
      EVT_KEY_PRESS     AVK_3          A
      EVT_KEY_RELEASE   INVALID        d

      "#" and "*" are special cases that we have to handle. They impact the text control
      But they behave a little differently.

      if "#" is pressed
      EVT               wParam         KeyCode
      EVT_KEY_PRESS     AVK_POUND      A
      EVT_KEY_RELEASE   INVALID        " "

      if "*" is pressed
      EVT               wParam         KeyCode
      EVT_KEY_PRESS     AVK_STAR       A
      EVT_KEY_RELEASE   INVALID        A

      Therefore, the keycode can be detremined only when the key is released*/

      //MSG_ERROR("evt = %x, w= %x , dw= %x",evt,w,dw);
      //MSG_ERROR("evt = %x, w= %x , dw= %x",evt,w,dw);

    // Switching between Controls. Dialog should not handle it
      if(!ITEXTCTL_IsActive(pIText))
      {
          return FALSE;
      }

      if(w== AVK_UP || w == AVK_DOWN || w == AVK_SELECT || w == AVK_LEFT
         || w == AVK_RIGHT)
      {
        pMe->m_dispInfo.handleEvent = FALSE;
        return FALSE; /*Do not handle these keys*/
      }

      /*A new key is presed*/
      if(evt == EVT_KEY_PRESS && !pMe->m_dispInfo.handleEvent)
      {
        /*Tell the screen to handle the Next KEY_RELEASE event*/
        pMe->m_dispInfo.handleEvent = TRUE;
      }

      if(w == AVK_STAR)/*caps_lock*/
      {
        /*Do not handle the KEY*/
        pMe->m_dispInfo.handleEvent = FALSE;
      }

      if(w == AVK_POUND || w==AVK_0)
      {
        /*It is a space*/
        pMe->m_dispInfo.isSpace = TRUE;
      }

      /*Get the Cursor position. The Cursor position does not change if the same key is pressed a few times*/
      pos = (uint16)ITEXTCTL_GetCursorPos(pIText);

      if(w==AVK_CLR && pMe->m_dispInfo.handleEvent)
      {
        int rv;

        /*Erase the last character*/
        rv = WMSAPPMD_EraseDisplay(pMe, pos, -1);
        if(rv)
        {
          /*The key event has been handled*/
          pMe->m_dispInfo.handleEvent = FALSE;
          if (pMe->m_dispInfo.length > 0) 
          {
            pMe->m_dispInfo.length--;
          }
          return TRUE;
        }
        return FALSE;
      }

      /*Get the text from the TextCTL*/
      text = ITEXTCTL_GetTextPtr(pIText);
      (void)WSTRTOSTR(text+pos, (char *)realData, 2);
      //MSG_ERROR("Char is %c at pos %d",realData[0],pos,0);


      /*This loop is exceuted only for key releases because, this is when we get the key code.
        Also, the handleEvent boolean has to be checked. In some case , for eg if a "*" is pressed, we do not
        want to do anything*/



      if (evt == EVT_KEY_RELEASE && text != NULL && pMe->m_dispInfo.handleEvent)
      {
        /*If it is a space, then send the space character to the display function*/
        if(pMe->m_dispInfo.isSpace)
        {
          /*Not sure */
          pos--;
          pMe->m_dispInfo.isSpace = FALSE;
        }


        if(pMe->m_dispInfo.isCaps)
        {
          if(text[pos] >= 0x61 && text[pos] <= 0x7A)
          {
            text[pos] -= 0x20;
          }
        }

        /*Display the character*/

        (void)WMSAPPMD_DisplayWhileComposing(pMe, text+pos,
                                        (boolean)(pos-pMe->m_dispInfo.length));
        /*Rememeber the last position*/
        pMe->m_dispInfo.length = pos;

      }
      return TRUE;

    default:
      return FALSE;
   }

} /* WMSAPPMN_ComposeDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_AccessChannelMsgCb

DESCRIPTION
  Callback is called to compose and send a user-defined number of
  messages with a user-defined number of characters for every few
  minutes (user-defined) in a given time period (user-defined).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_AccessChannelMsgCb(void *pMe){
  int i;
  CWMSAPP *pUser = (CWMSAPP*)pMe;

  if(pUser->AccessChMsgSendContinuouslyFor > 0){

    /* Total number of message to be sent over the time period */
    pUser->AccessChMsgSendContinuouslyFor--;

    /* Number of message to be sent everytime */
    for(i = 0; i<pUser->AccessChMsgNumberOfMsgs; i++){

      /* prepare the CDMA message to be sent with the user-defined of characters.
         No TL ack and ASCII encoding
      */
      (void)WMSAPPMN_PrepareCDMAMOMsg(pUser,pUser->AccessChMsgNumberOfChars);

      /* Sending the Message after preparing it */
      (void)WMSAPPMN_SendMessage(pMe);
    }

    /* the interval is defined in minutes */
    (void)ISHELL_SetTimer(pUser->a.m_pIShell,pUser->AccessChMsgInterval*1000, WMSAPPMN_AccessChannelMsgCb, pUser);
  }


}


/*===========================================================================
FUNCTION WMSAPPMN_MenuSoftkeyDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MenuSoftkeyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  uint16 wDlgID;
  IDialog* pIDialog;
  boolean result;
  uint16 wMenuCtrID;
  uint16 wSoftKeyCtrlID;
  IMenuCtl* pICurrentMenu;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wDlgID = IDIALOG_GetID(pIDialog);

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitMenuSoftKey(pMe));
      /* no menu control ---> error */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          (void)WMSAPPMN_CheckReturnDlg(pMe);
          return result;

        case AVK_DOWN:

          wSoftKeyCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

          (void)IDIALOG_SetFocus(pIDialog, (int16)wSoftKeyCtrlID);
          return TRUE;

        case AVK_UP:

          wMenuCtrID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
          (void)IDIALOG_SetFocus(pIDialog, (int16)wMenuCtrID);
          return TRUE;

        default:
          break;
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      wMenuCtrID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
      pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wMenuCtrID);

      if(IMENUCTL_IsActive(pICurrentMenu))
      {
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
        return TRUE;
      }

      wSoftKeyCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

      if(!wSoftKeyCtrlID)
      {
        return FALSE;
      }

      pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wSoftKeyCtrlID);

      if(!pICurrentMenu)
      {
        return FALSE;
      }

      WMSAPPMN_StopImages(pMe);
      wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, IMENUCTL_GetSel(pICurrentMenu));

      if(wDlgID == 0)
      { /* error */
        (void)WMSAPPMN_EndNDlgs(pMe,1);
        return TRUE;
      }

      if(IMENUCTL_IsActive(pICurrentMenu))
      {
        if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                               wDlgID, NULL) == EFAILED)
        { /* Creation failed */

          return FALSE;
        }

      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_MenuSoftkeyDlgEventHandler */


/*===========================================================================
FUNCTION WMSAPPMN_TextSoftKeyDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_TextSoftKeyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  uint16 wDlgID;
  IDialog* pIDialog;
  boolean result;
  uint16 wMenuCtrID;
  uint16 wSoftKeyCtrlID;
  AEETextInputMode wMode = AEE_TM_NUMBERS;
  ITextCtl* pITextCtl=NULL;
  uint16 wTextCtlID;
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wDlgID = IDIALOG_GetID(pIDialog);

  if(wDlgID == IDD_VOICEMAIL_NUMBER)
  {
    wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
    pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);
    if(pITextCtl == 0)
    {
      return FALSE;
    }
  }

  switch(evt)
  {

    case EVT_DIALOG_START:
      if(wDlgID == IDD_TEMPLATE_ALPHA_TAG)
      {
      	#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
      	wMode = AEE_TM_RAPID;
      	#else
        wMode = AEE_TM_LETTERS;
        #endif
      }
      else if (wDlgID == IDD_VOICEMAIL_NUMBER)
      {
        wMode = OEM_TM_DIALED_DIGITS;
      }
      return(WMSAPPMN_InitTextSoftKey(pMe,wMode));
      /* no menu control ---> error */



    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          return result;

        case AVK_DOWN:
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          wSoftKeyCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

          (void)IDIALOG_SetFocus(pIDialog, (int16)wSoftKeyCtrlID);
          return TRUE;

        case AVK_UP:
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          wMenuCtrID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
          (void)IDIALOG_SetFocus(pIDialog, (int16)wMenuCtrID);
          return TRUE;
		 case AVK_CAMERA:
    	 #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
		 {
			nv_item_type	  SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select ==AVK_SEND_TWO)
			{
				if(wDlgID == IDD_VOICEMAIL_NUMBER)
          		{
            		(void)WMSAPPMN_CallVoiceMailNum(pMe);
				}
			}
			return TRUE;
		}
		#endif
		break;

        case AVK_SEND:
        #if defined(FEATURE_VERSION_C01) 
		{
			nv_item_type	SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select==AVK_SEND_TWO)
			{
				return TRUE;
			}
		}
		#endif
          if(wDlgID == IDD_VOICEMAIL_NUMBER)
          {
            (void)WMSAPPMN_CallVoiceMailNum(pMe);
          }
          return TRUE;

        default:
          break;
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      switch(w)
      {
        case IDL_SK_VOICEMAIL_CALL:
          (void)WMSAPPMN_CallVoiceMailNum(pMe);
          break;

        case IDL_SK_VOICEMAIL_SAVE:
          (void)WMSAPPMN_SaveVoiceMailNum(pMe);
          (void)WMSAPPMN_EndDlg(pMe);
          break;

        case IDL_SK_VOICEMAIL_DONE:
          (void)WMSAPPMN_EndDlg(pMe);
          break;

        case IDL_SK_TEMPLATE_ALPHA_TAG_DONE:
          (void)WMSAPPMN_StoreAlphaTag(pMe);
          break;

        default:
          (void)WMSAPPMN_CreateSKSelectedDlg(pMe);
      }

      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_TextSoftKeyDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_TestMenuDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_TestMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
#ifdef FEATURE_CDSMS
  uint16 wDlgID;
#endif
  IDialog* pIDialog;
  boolean result;
  uint16 wTextCtlID;
  ITextCtl* pITextCtl;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
  {
    /*error*/
   return FALSE;
  }
  wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
  wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  if(wTextCtlID == 0)
    return FALSE;

  pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);

  if(!pITextCtl)
  {
    return FALSE;
  }



  switch(evt)
  {

    case EVT_DIALOG_START:
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, (IDialog*)dw);

      if(wCtrlID == 0)
      {
        return FALSE;
      }
      pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtrlID);

      if(pICurrentMenu)
      {
        /* remove certain items based on featurization */
        WMSAPPMD_SetScreen(pMe);
        SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
        WMSAPPMN_FillDefaultValues(pMe);

#ifdef FEATURE_CDSMS
        wDlgID = IDIALOG_GetID(pIDialog);
        if(wDlgID == IDD_CDMA_TEST)
        {
          WMSAPPMN_GetCheckBoxItems(pMe);
        }
#endif /* FEATURE_CDSMS */
        return TRUE;
      }
      return FALSE; /* no menu control ---> error */


    case EVT_KEY:
      if(w == AVK_CLR)
      {
        result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
        return result;

      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      switch(w)
      {
#ifdef FEATURE_SMS_UDH
        case IDL_EMS_TEST_SMALL_PIC:
          (void)WMSAPP_TestSmallPicture(pMe);
          return TRUE;

        case IDL_EMS_TEST_LARGE_PIC:
          (void)WMSAPP_TestLargePicture(pMe);
          return TRUE;

        case IDL_EMS_TEST_VAR_PIC:
          (void)WMSAPP_TestVarPicture(pMe);
          return TRUE;

        case IDL_EMS_TEST_LARGE_ANIM:
          (void)WMSAPP_TestLargeAnim(pMe);
          return TRUE;

        case IDL_EMS_TEST_SMALL_ANIM:
          (void)WMSAPP_TestSmallAnim(pMe);
          return TRUE;

        case IDL_EMS_TEST_PREDEF_ANIM:
          (void)WMSAPP_TestPredefAnim(pMe);
          return TRUE;

        case IDL_EMS_TEST_USER_MELODY:
          (void)WMSAPP_TestUserMelody(pMe);
          return TRUE;

        case IDL_EMS_TEST_MELODY:
          (void)WMSAPP_TestMelody(pMe);
          return TRUE;

        case IDL_EMS_TEST_MAX_MSG:
          (void)WMSAPP_TestMiscEms(pMe);
          return TRUE;

        case IDL_EMS_TEST_SEND_SEGMENT:
          pMe->m_bStoreMsg = FALSE;
          (void)WMSAPP_TestSegmentEms(pMe);
          return TRUE;

        case IDL_EMS_TEST_STORE_SEGMENT:
          pMe->m_bStoreMsg = TRUE;
          (void)WMSAPP_TestSegmentEms(pMe);
          return TRUE;

        case IDL_EMS_TEST_EXTENDED_OBJECT:
          (void)WMSAPP_TestExtendedObject(pMe);
          return TRUE;
#endif /* FEATURE_SMS_UDH */

        case IDL_CDMA_TEST_ORIG_SMS_CALL:
          (void)WMSAPPMN_DcConnect(pMe, WMS_DC_SO_AUTO);
          return TRUE;

        case IDL_CDMA_TEST_ORIG_SO6:
          (void)WMSAPPMN_DcConnect(pMe, WMS_DC_SO_6);
          return TRUE;

        case IDL_CDMA_TEST_ORIG_SO14:
          (void)WMSAPPMN_DcConnect(pMe, WMS_DC_SO_14);
          return TRUE;

        case IDL_CDMA_TEST_DELETE_ALL_PHONE:
          (void)WMSAPPMN_DeleteAllMsgs(pMe,WMS_MEMORY_STORE_NV_CDMA );
          return TRUE;

        case IDL_CDMA_TEST_DELETE_ALL_CARD:
#ifdef FEATURE_CDSMS_RUIM
          (void)WMSAPPMN_DeleteAllMsgs(pMe,WMS_MEMORY_STORE_RUIM);
#else
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_FEATURE_NOT_AVAILABLE, NULL, SELECTIONVERIFICATIONTIME);

#endif /* FEATURE_CDSMS_RUIM */
          return TRUE;

        case IDL_CDMA_TEST_RESET_SEQ_NUM:
          (void)WMSAPPMN_ResetSeqNum(pMe);
          return TRUE;


        case IDL_AUTO_DISCONNECT:
        case IDL_AUTO_DC_SEND:
        case IDL_ACCESS_CHANNEL_MSG:
          (void)WMSAPPMN_SetCheckBoxItems(pMe, FALSE);
          return TRUE;

        case IDL_AUTO_DC_SEND_SO:
          return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));

        case IDL_CDMA_TEST_SEND_MO_MSG:
        case IDL_GW_TEST_SEND_MO_MSG:
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_TEST_ADDRESS, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
          return TRUE;


        case IDL_ACCESS_CHANNEL_MSG_TEST_MT:
         pMe->m_bAccessChannelTest = TRUE;
         pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         (void)WMSAPPMN_PrepareCDMAMTMsg(pMe, 3, (wms_msg_event_info_s_type *)dw);
         return TRUE;

        /* Message Tests:
           Store a message in slot 0 of the phone. The address of this message should match the incoming
           message and we want to reply with a user-defined number of characters that is
           a part of the incoming message.
           The incoming message format:
           <numberOfCharacters><numberOfMessages>
           NumberOfCharacters - two numbers
           numberOfMessage - this will be the number of messages that will be sent if the incoming address
                             is the same as the address of the message stored in slot 0.
        */
        case IDL_ACCESS_CHANNEL_MSG_TEST:
         pMe->m_bAccessChannelTest = TRUE;
         pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
         (void)WMSAPPMN_InitMsgType(pMe);
         (void)IMENUCTL_Redraw(pICurrentMenu);
         return TRUE;

       /* Send all messages in the Outbox - CDMA mode */
       case IDL_CDMA_SEND_ALL_MO:
          (void)WMSAPPMN_SendAllMsgs(pMe);
          break;

       case IDL_MSG_TYPE_PHONE_NUMBER:
          pMe->m_addrType = WMSAPP_MSG_TYPE_PHONE_NUM;
          (void)ITEXTCTL_SetInputMode(pITextCtl, OEM_TM_DIALED_DIGITS);
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                  IDD_TEST_ADDRESS, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
          return TRUE;
      case IDL_MSG_TYPE_EMAIL_ADDRESS:
      case IDL_MSG_TYPE_NON_STD_EMAIL:
          pMe->m_addrType = WMSAPP_MSG_TYPE_EMAIL;
          (void)ITEXTCTL_Reset(pITextCtl);
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                  IDD_TEST_ADDRESS, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          if(pIDialog == NULL)
          {
            /*error*/
          return FALSE;
          }
          wCtrlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
          pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
          wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
          if(wTextCtlID == 0)
            return FALSE;

          pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);

          if(!pITextCtl)
          {
            return FALSE;
          }
		  #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
		  (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_RAPID);
		  #else
          (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_LETTERS);
          #endif
          (void)ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
          ITEXTCTL_SetActive(pITextCtl, TRUE);
          (void)ITEXTCTL_Redraw(pITextCtl);

          return TRUE;





        case IDL_GW_TEST_SET_ROUTES:
          (void)WMSAPPMN_SetTestRoutes(pMe);
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_MESSAGE_ROUTES_SET, NULL, SELECTIONVERIFICATIONTIME);
          return TRUE;

        case IDL_GW_TEST_SEND_ALL:
          (void)WMSAPPMN_SendAllMsgs(pMe);
          break;

        case IDL_GW_TEST_DELETE_ALL_MSGS:
          (void)WMSAPPMN_DeleteAllMsgs(pMe,WMS_MEMORY_STORE_NONE);
          return TRUE;

        case IDL_GW_TEST_DELETE_ALL_TMPLTS:
          (void)WMSAPPMN_DeleteAllTemplates(pMe,WMS_MEMORY_STORE_SIM);
          break;

        case IDL_GW_TEST_CLEAR_SMMA:
          (void)IWMS_DbgClearSMMAFlag(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                                (void*)pMe);
          break;

        case IDL_GW_TEST_RESET_REF:
          (void)IWMS_DbgSetMsgRefNum(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback,
                               (void*)pMe);
          break;

        default:
          break;
      }
      return TRUE;

    case EVT_WMS_DLG_RESUME:
      WMSAPPMN_GetCheckBoxItems(pMe);
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_TestMenuDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_TestSendMsgEvtHandler

DESCRIPTION
  Sends Canned MO Msg in CDMA/GW Mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_TestSendMsgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  uint16 wDlgID;
  IDialog* pIDialog;
  char *numberOfChars, *interval, *timePeriod, *numberOfMsgs;
  ITextCtl  *pITextCtl = NULL;
  AECHAR *text;
  char *destAddress;
  uint8 loopCounter, digitCounter;
  uint8 length;
  uint16 wTextCtlID;
  IMenuCtl* pISKMenu;
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wTextCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);
  pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,(int16)wTextCtlID);
  wDlgID = IDIALOG_GetID(pIDialog);

  switch(evt)
  {

    case EVT_DIALOG_START:
      if(WMSAPPMN_InitTextSoftKey(pMe,AEE_TM_NUMBERS))
      {
        if(pMe->m_modeControl == WMSAPP_MODE_GW && wDlgID == IDD_TEST_ADDRESS)
        {
          WMSAPPMN_ReadDefaultTemplate(pMe);
        }
        pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;

        /* Remove Continuous Send Soft Key for GW Mode */
        if(pMe->m_modeControl == WMSAPP_MODE_GW && wDlgID == IDD_TEST_NUMBER_OF_CHARS)
        {
          pISKMenu = (IMenuCtl*)IDIALOG_GetControl( pIDialog, IDC_SK_TEST_SEND_MSG);
          (void)IMENUCTL_DeleteItem(pISKMenu, IDL_SK_CONTINUOUS_SEND);
        }
      }
      return TRUE;

      /* no menu control ---> error */

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      switch(wDlgID)
      {
        case IDD_TEST_ADDRESS:
          /*Get the Destination Address*/
          pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,IDC_TEST_ADDRESS);
          if(!pITextCtl)
          {
            return FALSE;
          }
          
          (void)ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl)|TP_NODRAW);
          
          text = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
          destAddress = MALLOC(WMS_MAX_LEN);
          if ((text == NULL) || (destAddress == NULL))
          {
            return FALSE;
          }

          /* Get the text from the control */
          if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
          {
            WMSAPPU_FREE(text);
            return FALSE;
          }

          length = (uint8)WSTRLEN(text);
          if(length > 1)
          {
            boolean is_international_address = FALSE;

            (void)WSTRTOSTR(text, destAddress, length +1);

            if (destAddress[0] == '+')
            {
              is_international_address = TRUE;
            }

            /* To Strip out only the all the first + signs in the Addr */
            digitCounter = 0;
            for (loopCounter=0; loopCounter < length; loopCounter++)
            {
              if ((destAddress[loopCounter] != '+')
                  ||(loopCounter != 0) ) 
              {
                /* Keep it in destAddress */
                destAddress[digitCounter] = destAddress[loopCounter];
                digitCounter++;
              }
            }
            length = digitCounter;
          
            if(pMe->m_modeControl == WMSAPP_MODE_GW)
            {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
            }
            else
            {
#ifdef FEATURE_CDSMS
              (void)MEMCPY( pMe->m_pClient_msg_mo->u.cdma_message.address.digits, destAddress, length+1 );
              pMe->m_pClient_msg_mo->u.cdma_message.address.number_of_digits = length;
              if (is_international_address)
              {
                pMe->m_pClient_msg_mo->u.cdma_message.address.number_type = WMS_NUMBER_INTERNATIONAL;
              }
#endif /*FEATURE_CDSMS*/
            }
          }

          WMSAPPU_FREE(text);
          WMSAPPU_FREE(destAddress);

          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_TEST_NUMBER_OF_CHARS, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }

          return TRUE;

        case IDD_TEST_NUMBER_OF_CHARS:
          /*Get the Number of Charcaters*/
          pITextCtl = (ITextCtl * )IDIALOG_GetControl(pIDialog,IDC_TEST_NUMBER_OF_CHARS);
          if(!pITextCtl)
          {
            return FALSE;
          }
          (void)ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl)|TP_NODRAW);
          text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
          numberOfChars = MALLOC(WMS_MAX_LEN+1);

          if (text == NULL || numberOfChars == NULL)
          {
            WMSAPPU_FREE(text);
            WMSAPPU_FREE(numberOfChars);
            return FALSE;
          }

          /* Get the text from the control */
          if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
          {
            WMSAPPU_FREE(text);
            WMSAPPU_FREE(numberOfChars);
            return FALSE;
          }

          (void)WSTRTOSTR(text, (char *)numberOfChars, WMS_MAX_LEN);
          WMSAPPU_FREE(text);

          switch (w)
          {
            /* Message Tests: Send a user-defined of messages for every given number of minutes
               over a time period over the Access Channel.
            */
             case IDL_SK_INTERVAL_DONE:
               /* Get the interval - amount of time we have to wait to
                  send the next bunch of messages
               */
               text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
               interval = MALLOC(WMS_MAX_LEN+1);

              if (text == NULL )
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(interval);
                return FALSE;
              }

              /* Get the text from the control */
              if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(interval);
                return FALSE;
              }

              (void)WSTRTOSTR(text, (char *)interval, WMS_MAX_LEN);
              pMe->AccessChMsgInterval = ATOI(interval);
              WMSAPPU_FREE(interval);
              WMSAPPU_FREE(text);

              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_TEST_NUMBER_OF_CHARS, NULL) == EFAILED)
              { /* Creation failed */
                return FALSE;
              }

              pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              (void)ITEXTCTL_Reset(pITextCtl);

              if(!ITEXTCTL_SetTitle(pITextCtl, WMSAPP_RES_FILE,
                            IDS_TIME_PERIOD, NULL))
              {
                return FALSE;
              }
              pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
              (void)IMENUCTL_DeleteAll(pISKMenu);
              (void)IMENUCTL_AddItem(pISKMenu, WMSAPP_RES_FILE,IDS_DONE , IDL_SK_TIME_PERIOD_DONE, NULL, 0);
              (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
              (void)ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
              ITEXTCTL_SetActive(pITextCtl, TRUE);
              (void)ITEXTCTL_Redraw(pITextCtl);
              (void)IMENUCTL_Redraw(pISKMenu);

              return TRUE;


             /* the number of message we have to send everytime */
             case IDL_SK_NUMBER_OF_MSGS_DONE:
               text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
               numberOfMsgs = MALLOC(WMS_MAX_LEN+1);

               if (text == NULL || numberOfMsgs == NULL)
               {
                 WMSAPPU_FREE(text);
                 WMSAPPU_FREE(numberOfMsgs);
                 return FALSE;
               }

               /* Get the text from the control */
               if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
               {
                 WMSAPPU_FREE(text);
                 WMSAPPU_FREE(numberOfMsgs);
                 return FALSE;
               }

               (void)WSTRTOSTR(text, (char *)numberOfMsgs, WMS_MAX_LEN);
               /* the number of characters in the message */
               pMe->AccessChMsgNumberOfMsgs = ATOI(numberOfMsgs);
               WMSAPPU_FREE(numberOfMsgs);
               WMSAPPU_FREE(text);


               /* Calculate the number of messages to be sent in the time period */
               /* The interval is in minutes. time period is defined in hours */
               if(pMe->AccessChMsgInterval/60 != 0)
               {
                 pMe->AccessChMsgSendContinuouslyFor = (pMe->AccessChMsgTimePeriod*60)/(pMe->AccessChMsgInterval/60);
               }
               else
               {
                 pMe->AccessChMsgSendContinuouslyFor = (pMe->AccessChMsgTimePeriod*60);
               }

               (void)ISHELL_SetTimer(pMe->a.m_pIShell, pMe->AccessChMsgInterval/60*1000, WMSAPPMN_AccessChannelMsgCb, pMe);
               break;

             /* The time period we have to send the messages for */
             case IDL_SK_TIME_PERIOD_DONE:
               text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
               timePeriod = MALLOC(WMS_MAX_LEN+1);

               if (text == NULL || timePeriod == NULL)
               {
                 WMSAPPU_FREE(text);
                 WMSAPPU_FREE(timePeriod);
                 return FALSE;
               }

               /* Get the text from the control */
               if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
               {
                 WMSAPPU_FREE(text);
                 WMSAPPU_FREE(timePeriod);
                 return FALSE;
               }

               (void)WSTRTOSTR(text, (char *)timePeriod, WMS_MAX_LEN);
               pMe->AccessChMsgTimePeriod = ATOI(timePeriod);
               WMSAPPU_FREE(text);
               WMSAPPU_FREE(timePeriod);
               (void)ISHELL_EndDialog(pMe->a.m_pIShell);
               if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_TEST_NUMBER_OF_CHARS, NULL) == EFAILED)
               { /* Creation failed */
                 return FALSE;
               }

               pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
               (void)ITEXTCTL_Reset(pITextCtl);
               if(!ITEXTCTL_SetTitle(pITextCtl, WMSAPP_RES_FILE,
                            IDS_NUMBER_OF_MSGS, NULL))
               {
                 return FALSE;
               }
               pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
               (void)IMENUCTL_DeleteAll(pISKMenu);
               (void)IMENUCTL_AddItem(pISKMenu, WMSAPP_RES_FILE,IDS_DONE , IDL_SK_NUMBER_OF_MSGS_DONE, NULL, 0);
               (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
               (void)ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
               ITEXTCTL_SetActive(pITextCtl, TRUE);
               (void)ITEXTCTL_Redraw(pITextCtl);
               (void)IMENUCTL_Redraw(pISKMenu);
               return TRUE;

            case IDL_SK_TEST_STORE_MSG:
              /* the number of characters in the message */
              text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));

              if (text == NULL || numberOfChars == NULL)
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              /* Get the text from the control */
              if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              (void)WSTRTOSTR(text, (char *)numberOfChars, WMS_MAX_LEN);
              pMe->AccessChMsgNumberOfChars = ATOI(numberOfChars);
              WMSAPPU_FREE(text);

              /* Compose and store the message with the given number of characters */
              if(pMe->m_modeControl == WMSAPP_MODE_GW)
              {
                (void)WMSAPPMN_PrepareGWMOMsg(pMe,(uint8)ATOI(numberOfChars));
              }
              else
              {
                (void)WMSAPPMN_PrepareCDMAMOMsg(pMe,(uint8)ATOI(numberOfChars));
              }
              (void)WMSAPPMN_CreateSKSelectedDlg(pMe);
              break;

            case IDL_SK_TEST_SEND_MSG:

              /* Send a message with a given number of characters */
              text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
              if (text == NULL || numberOfChars == NULL)
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              /* Get the text from the control - number of characters in the message */
              if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              (void)WSTRTOSTR(text, (char *)numberOfChars, WMS_MAX_LEN);
              pMe->AccessChMsgNumberOfChars = ATOI(numberOfChars);
              WMSAPPU_FREE(text);

              if(pMe->m_modeControl == WMSAPP_MODE_GW)
              {
                (void)WMSAPPMN_PrepareGWMOMsg(pMe,(uint8)ATOI(numberOfChars));
              }
              else
              {
                (void)WMSAPPMN_PrepareCDMAMOMsg(pMe,(uint8)ATOI(numberOfChars));
              }

              // Sending the Message after preparing it
              (void)WMSAPPMN_SendMessage(pMe);
              break;

            /* To be able to send messages continuously every few minutes over a time period */
            case IDL_SK_CONTINUOUS_SEND:
              if (pMe->m_modeControl == WMSAPP_MODE_GW)
              {
                /* Dont do any processing */
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }
              text     = MALLOC((WMS_MAX_LEN+1)*sizeof(AECHAR));
              if (text == NULL || numberOfChars == NULL)
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              /* Get the text from the control */
              if(!(ITEXTCTL_GetText(pITextCtl, text, WMS_MAX_LEN)))
              {
                WMSAPPU_FREE(text);
                WMSAPPU_FREE(numberOfChars);
                return FALSE;
              }

              (void)WSTRTOSTR(text, (char *)numberOfChars, WMS_MAX_LEN);

              /* the number of characters the message should contain */
              pMe->AccessChMsgNumberOfChars = ATOI(numberOfChars);

              WMSAPPU_FREE(text);
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_TEST_NUMBER_OF_CHARS, NULL) == EFAILED)
              { /* Creation failed */
                return FALSE;
              }

              pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              (void)ITEXTCTL_Reset(pITextCtl);

              if(!ITEXTCTL_SetTitle(pITextCtl, WMSAPP_RES_FILE,
                            IDS_INTERVAL, NULL))
              {
                return FALSE;
              }
              pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
              (void)IMENUCTL_DeleteAll(pISKMenu);
              (void)IMENUCTL_AddItem(pISKMenu, WMSAPP_RES_FILE,IDS_DONE , IDL_SK_INTERVAL_DONE, NULL, 0);
              (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
              (void)ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
              ITEXTCTL_SetActive(pITextCtl, TRUE);
              (void)ITEXTCTL_Redraw(pITextCtl);
              (void)IMENUCTL_Redraw(pISKMenu);
              break;

            default:
              // Since our focus is on the Text Control
              // Shift Focus to SoftKey Control
              (void)IDIALOG_SetFocus(pIDialog, IDC_SK_TEST_SEND_MSG);
              break;
          }
          WMSAPPU_FREE(numberOfChars);
          break;

        default:
          break;
      }
      return TRUE;

    case EVT_WMS_MSG_READ_TEMPLATE:
      {
        wms_msg_event_info_s_type *sms_msg_event_info_buf;
        sms_msg_event_info_buf = (wms_msg_event_info_s_type *) dw;

        /* Assign the global template to the template that was read */
        (void)MEMCPY(pMe->m_pMO_template_msg, &sms_msg_event_info_buf->status_info.message,
                sizeof(wms_client_message_s_type));
        (void)WMSAPPMN_DisplayAddress(pMe);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_TestSendMsgEvtHandler */
/*===========================================================================
FUNCTION WMSAPPMN_StaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  InitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_StaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IMenuCtl* pISKMenu;
  IDialog *pIDialog;
  uint16 wItemID;
  uint16 wDlgID;
  IStatic *pIStatic;
  uint32 dwProps;
  AEERect rc;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
  pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog));

  switch(evt)
  {
    case EVT_DIALOG_START:
      return WMSAPPMN_InitStaticDlg(pMe);

    case EVT_WMS_MSG_DELETE_STATUS_REPORT:
      (void)WMSAPPMN_EndDlg(pMe);
      (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_MESSAGE_DELETED, NULL, SELECTIONVERIFICATIONTIME);
      return TRUE;

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)WMSAPPMN_EndDlg(pMe);
      }
      return TRUE;

    /*Switch to the SK Ctl if Static Ctl is Active*/
    case EVT_KEY_PRESS:
      if(pISKMenu != NULL) /*Only for Dialogs with SK control*/
      {
        switch(w)
        {
          case AVK_LEFT:
          case AVK_RIGHT:
            if(!IMENUCTL_IsActive(pISKMenu))
            {
              ISTATIC_SetActive(pIStatic,FALSE);
              (void)IMENUCTL_SetActive(pISKMenu, TRUE);
            }
            return TRUE;

          /*Switch to the static Ctl if SK is active*/
          case AVK_UP:
          case AVK_DOWN:
            /*If the static Control is not scrollable, do nothing*/
            if(!ISTATIC_IsScrollable(pIStatic))
            {
              return FALSE;
            }
            if(!ISTATIC_IsActive(pIStatic))
            {
              (void)IMENUCTL_SetActive(pISKMenu,FALSE);
              ISTATIC_SetActive(pIStatic,TRUE);
            }
            return TRUE;

          default:
            break;
        }
      }


      return TRUE;

    case EVT_COMMAND:

      wItemID  = IMENUCTL_GetSel(pISKMenu);

      switch(wItemID)
      {
        case IDL_SK_ERASE_TEMPLATE:
          if ((pMe->m_pMO_template_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_SIM)
              ||(pMe->m_pMO_template_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_RUIM))
          {
            /* A Card Template */
            WMSAPPMN_DeleteTemplate(pMe);
          }
          else if ((pMe->m_pMO_template_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA)
              ||(pMe->m_pMO_template_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_GW))
          {
            /* NV Template */
            /* Erasing an NV Template is same as writing Default Values;
               since we can never erase an NV Template */
            WMSAPPMN_WriteDefaultNVTemplate(pMe);
          }
          (void)WMSAPPMN_EndDlg(pMe);
          break;

        case IDL_SK_EDIT_TMPLT:
          WMSAPPMN_StopImages(pMe);
          wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, wItemID);
          if(wDlgID == 0)
          { /* error */
            return FALSE;
          }
          pMe->m_msgInfo.write_mode = WMS_WRITE_MODE_REPLACE;

          /*Disable suto Scroll*/
          dwProps = ISTATIC_GetProperties(pIStatic);
          dwProps |= ST_NOSCROLL;
          (void)ISTATIC_SetProperties(pIStatic, dwProps);
          ISTATIC_GetRect(pIStatic, &rc);
          ISTATIC_SetRect(pIStatic, &rc);
          (void)ISTATIC_Redraw(pIStatic);

          if(ISHELL_CreateDialog(pMe->a.m_pIShell,WMSAPP_RES_FILE,wDlgID, NULL)!= SUCCESS)
                 return FALSE;  /* ERROR */
          return TRUE;

        case IDL_SK_TMPLT_DONE:
        case IDL_SK_STATUS_REPORT_DONE:
          (void)WMSAPPMN_EndDlg(pMe);
          return TRUE;

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

        default:
          return FALSE;
      }

      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_StaticDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_StartIdleMenu

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_StartIdleMenu
(
  CWMSAPP *pMe
)
{
  uint32 ret_status;
  (void)ISHELL_EndDialog(pMe->a.m_pIShell);

  ret_status = (uint32)ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                   IDD_MAINSMS, NULL);
  if(ret_status == EFAILED)
  { /* Creation failed */
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMN_InitImageDlg

DESCRIPTION
  Handles the initialization of Image Dialog
  Returns True if Image Dialog properly initialized

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_InitImageDlg
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_SMS_UDH 
  IImage *pIImage;
  IImageCtl *pIImageCtl = NULL;
  IMenuCtl *pISKMenu = NULL;
  IDialog *pIDialog;
  uint16 wDlgID;
  AEERect rc;
  AEEImageInfo ImageInfo;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wDlgID = IDIALOG_GetID(pIDialog);

  if (wDlgID == IDD_ABOUT)
  {
    pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
    pIImageCtl = (IImageCtl*)IDIALOG_GetControl(pIDialog, IDC_IMAGE_ABOUT);
  }
  else if (wDlgID == IDD_SPLASH)
  {
    pIImageCtl = (IImageCtl*)IDIALOG_GetControl(pIDialog, IDC_SPLASH);
  }

  switch (wDlgID)
  {
    case IDD_ABOUT:
    {
      if(pISKMenu)
      {
        (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
        (void)IMENUCTL_SetActive(pISKMenu, TRUE);
      }
      pIImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                    WMSAPP_RES_FILE,
                                    IDB_ABOUT);
      if(pIImage)
      {
        IIMAGE_GetInfo(pIImage, &ImageInfo);
        rc.x = (int16)((pMe->m_cxScreen - ImageInfo.cx)/2);
        rc.y = (int16)((pMe->m_cyScreen - ImageInfo.cy)/2);
        rc.dx = (int16)ImageInfo.cx;
        rc.dy = (int16)ImageInfo.cy;
        pMe->m_pSelImage = pIImage;

        if (pIImageCtl)
        {
          IIMAGECTL_SetRect(pIImageCtl, &rc);
          IIMAGECTL_SetImage(pIImageCtl, pMe->m_pSelImage);
          IIMAGECTL_SetActive(pIImageCtl, FALSE);

          if (!IIMAGECTL_Redraw(pIImageCtl))
          {
            MSG_ERROR("IImageCtl Falied to Redraw in WMSAPPMN_ImageDlgEventHandler",0,0,0);
          }
        }
      }
      (void)IDIALOG_SetFocus(pIDialog, (int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
      break;
    }
    case IDD_SPLASH:
    {
      pIImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                    WMSAPP_RES_FILE,
                                    IDB_SPLASH);
      if(pIImage)
      {
        IIMAGE_SetFrameCount(pIImage, WMS_SPLASH_ANIM_NUM_BITMAPS);
        IIMAGE_SetAnimationRate(pIImage, 250);

        IIMAGE_GetInfo(pIImage, &ImageInfo);
        pMe->m_pSelImage = pIImage;

        // Since it is made up of frames
        ImageInfo.cx = ImageInfo.cx / WMS_SPLASH_ANIM_NUM_BITMAPS;
        rc.x = (int16)((pMe->m_cxScreen - ImageInfo.cx)/2);
        rc.y = (int16)((pMe->m_cyScreen - ImageInfo.cy)/2);
        rc.dx = (int16)ImageInfo.cx;
        rc.dy = (int16)ImageInfo.cy;
        IIMAGE_Start(pMe->m_pSelImage, rc.x, rc.y);

        if (pIImageCtl)
        {
          IIMAGECTL_SetRect(pIImageCtl, &rc);
          IIMAGECTL_SetImage(pIImageCtl, pMe->m_pSelImage);
          IIMAGECTL_SetActive(pIImageCtl, TRUE);

          (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                          (int) SPLASH_TIME_IN_MILLISECONDS,
                          (PFNNOTIFY) WMSAPPMN_StartIdleMenu,
                          (uint32*) pMe);

          if (!IIMAGECTL_Redraw(pIImageCtl))
          {
            MSG_ERROR("IImageCtl Falied to Redraw in WMSAPPMN_ImageDlgEventHandler",0,0,0);
          }
        }
      }
      break;
    }
    default:
      break;
  }
#endif /* FEATURE_SMS_UDH */
}


/*===========================================================================
FUNCTION WMSAPPMN_ImageDlgEventHandler

DESCRIPTION
  Handles Image Dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ImageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IDialog *pIDialog;
  uint16 wDlgID=0;
  uint16 wSKCtlID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog)
  {
    wDlgID = IDIALOG_GetID(pIDialog);
  }
  else
  {
    if (evt != EVT_DIALOG_END)
    {
      MSG_ERROR("ISHELL_GetActiveDialog returns NULL in ImageDlgEventHandler",0,0,0);
      return FALSE;
    }
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      WMSAPPMN_InitImageDlg(pMe);
      return TRUE;

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE;

    /*Switch to the SK Ctl if Image Ctl is Active*/
    case EVT_KEY_PRESS:
      switch (wDlgID)
      {
        case IDD_ABOUT:
        {
          switch(w)
          {
            /*Switch to the SK if not active*/
            case AVK_LEFT:
            case AVK_RIGHT:
            case AVK_UP:
            case AVK_DOWN:
              if (pIDialog)
              {
                wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
                (void)IDIALOG_SetFocus(pIDialog, (int16)wSKCtlID);
              }
              break;
            default:
              break;
          }
          break;
        }
        case IDD_SPLASH:
        {
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)WMSAPPMN_StartIdleMenu, (uint32 *)pMe);
          (void)WMSAPPMN_StartIdleMenu(pMe);
          break;
        }
        default:
          break;
      }
      return TRUE;

    /*Switch to the SK Ctl if Image Ctl is Active*/
    case EVT_KEY_RELEASE:
      switch (wDlgID)
      {
        case IDD_ABOUT:
        {
          if (pIDialog)
          {
            wSKCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
            (void)IDIALOG_SetFocus(pIDialog, (int16)wSKCtlID);
          }
          break;
        }
        default:
          break;
      }
      return TRUE;

    case EVT_COMMAND:
      switch (wDlgID)
      {
        case IDD_ABOUT:
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          break;
        }
        case IDD_SPLASH:
        {
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)WMSAPPMN_StartIdleMenu, (uint32 *)pMe);
          (void)WMSAPPMN_StartIdleMenu(pMe);
          break;
        }
        default:
          break;
      }
      return TRUE;

    case EVT_DIALOG_END:
#ifdef FEATURE_SMS_UDH 
      if (pMe->m_pSelImage != NULL)
      {
        IIMAGE_Stop(pMe->m_pSelImage);
        (void)IIMAGE_Release(pMe->m_pSelImage);
        pMe->m_pSelImage = NULL;
      }
#endif /* FEATURE_SMS_UDH */
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_ImageDlgEventHandler */

/*===========================================================================
FUNCTION GetMenuCtl

DESCRIPTION
  Determine the current active MenuCtl.  This function assumes that there is
  only 1 MenuCtl per dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static IMenuCtl *GetMenuCtl( CWMSAPP *pMe )
{
  IMenuCtl *pMenuCtl = NULL;
  IDialog  *pIActiveDialog;

  if( NULL != (pIActiveDialog = ISHELL_GetActiveDialog( pMe->a.m_pIShell )) )
  {
    uint16   DialogID;

    if( 0 != (DialogID = IDIALOG_GetID( pIActiveDialog )) )
    {
      int16    ControlID = 0;

      switch( DialogID )
      {
        case IDD_ROUTES:
          ControlID = IDC_ROUTES;
          break;

        case IDD_MEMORY_STORE:
          ControlID = IDC_MEMORY_STORE;
          break;

        case IDD_REJECT_DUP:
          ControlID = IDC_REJECT_DUP;
          break;

        case IDD_REPLY_PATH:
          ControlID = IDC_REPLY_PATH;
          break;

        case IDD_PID:
          ControlID = IDC_PID_TYPES;
          break;

        case IDD_DCS:
          ControlID = IDC_DCS_CLASSES;
          break;

        case IDD_VALIDITY_TIMEZONE:
          ControlID = IDC_MENU_TIMEZONE;
          break;

        case IDD_DESTADDR_TYPE:
          ControlID = IDC_DESTADDR_TYPE;
          break;

        case IDD_DESTADDR_PLAN:
          ControlID = IDC_DESTADDR_PLAN;
          break;

        case IDD_SCADDR_TYPE:
          ControlID = IDC_SCADDR_TYPE;
          break;

        case IDD_SCADDR_PLAN:
          ControlID = IDC_SCADDR_PLAN;
          break;

        case IDD_ENCODING_TYPES:
          ControlID = IDC_MENU_ENCODING_TYPES;
          break;

        case IDD_TELESERVICE:
          ControlID = IDC_MENU_TELESERVICE;
          break;

        case IDD_MSG_COMMAND:
          ControlID = IDC_MENU_MSG_COMMAND;
          break;

        case IDD_CDMA_DCS:
          ControlID = IDC_MENU_CDMA_DCS;
          break;

        case IDD_PRIORITY:
          ControlID = IDC_MENU_PRIORITY;
          break;

        case IDD_LANGUAGE:
          ControlID = IDC_MENU_LANGUAGE;
          break;

        case IDD_REL_VALIDITY:
          ControlID = IDC_MENU_RELATIVE_VALIDITY;
          break;

        case IDD_VALIDITY_YEAR:
          ControlID = IDC_MENU_VALIDITY_YEAR;
          break;

        case IDD_VALIDITY_MONTH:
          ControlID = IDC_MENU_VALIDITY_MONTH;
          break;

        case IDD_VALIDITY_DAY:
          ControlID = IDC_MENU_VAL_DAY;
          break;

        case IDD_VALIDITY_HOUR:
          ControlID = IDC_MENU_VAL_HOUR;
          break;

        case IDD_VALIDITY_MINUTE:
          ControlID = IDC_MENU_VAL_MINUTE;
          break;

        case IDD_DISPLAY_MODE:
          ControlID = IDC_MENU_DISPLAY_MODE;
          break;

        case IDD_ALERT_MODE_637A:
          ControlID = IDC_MENU_ALERT_MODE;
          break;

        case IDD_BC_MAX_MESSAGES:
          ControlID = IDC_MENU_BC_MAX_MESSAGES;
          break;

        case IDD_BC_AVAILABLE_SVCS:
          ControlID = IDC_MENU_BC_AVAILABLE_SVCS;
          break;

        case IDD_MODE_SELECT:
          ControlID = IDC_MENU_MODE_SELECT;
          break;

        case IDD_DOMAIN_PREF:
          ControlID = IDC_DOMAIN_PREF;
          break;

        case IDD_AUTO_MEM_STORE:
          ControlID = IDC_MENU_AUTO_MEM_STORE;
          break;

        case IDD_AUTO_DC_SEND_SO:
          ControlID = IDC_MENU_AUTO_DC_SEND_SO;
          break;

        case IDD_SEGMENT_SETTING:
          ControlID = IDC_SEGMENT_SETTING;
          break;

        case IDD_RETRY_SETTINGS:
          ControlID = IDC_RETRY_SETTING;
          break;
      }

      if( 0 != ControlID )
      {
          pMenuCtl = (IMenuCtl *) IDIALOG_GetControl( pIActiveDialog, ControlID );
      }
    }
  }

  return( pMenuCtl );
}


/*===========================================================================
FUNCTION WMSAPPMN_RadioBtnDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_RadioBtnDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  
  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell,  (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
  
    /* reset the idle screen timer */
    (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                     WMSAPPAPPTIMEOUT,
                     (PFNNOTIFY) WMSAPPMN_EndDlg,
                     (uint32*) pMe );
  }

  switch(evt)
  {

    case EVT_DIALOG_START:
      return (WMSAPPMN_InitRadioButtonDlg(pMe, w, (IDialog*)dw));


    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }

      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:

      WMSAPPMN_UpdateSetting(pMe, (uint16)w, GetMenuCtl(pMe) );

      if (w == IDL_REL_VAL_USER_DEFINED)
      {
        /* Goto the Next Dialog to Enter the User Defined Relative Validity */
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      }
      else
      {
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);

        /* Let the update image be shown for 2 seconds before going back to
        the previous screen */
        (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                        SELECTIONVERIFICATIONTIME,
                        (PFNNOTIFY) WMSAPPMN_EndDlg,
                        (uint32*) pMe);
      }
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_RadioBtnDlgEventHandler */



/*===========================================================================
FUNCTION WMSAPPMN_ValidityFormatDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ValidityFormatDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  
  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell,  (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
    
    /* reset the idle screen timer */
    (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                     WMSAPPAPPTIMEOUT,
                     (PFNNOTIFY) WMSAPPMN_EndDlg,
                     (uint32*) pMe );
  }


  switch(evt)
  {

    case EVT_DIALOG_START:
      return WMSAPPMN_InitRadioButtonDlg(pMe, w, (IDialog*)dw);


    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }

      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);

      {
        IDialog   *pIActiveDialog;

        if( NULL != (pIActiveDialog = ISHELL_GetActiveDialog( pMe->a.m_pIShell )) )
        {
          IMenuCtl *pMenuCtl;

          if( NULL != (pMenuCtl = (IMenuCtl *) IDIALOG_GetControl( pIActiveDialog, IDC_VALIDITY_FORMATS )) )
          {
            WMSAPPMN_UpdateSetting( pMe, (uint16)w, pMenuCtl );
          }
        }
      }

      if(w != IDL_VALIDITY_FORMAT_NONE)
      {
        return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));
      }
      else
      {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                      SELECTIONVERIFICATIONTIME,
                      (PFNNOTIFY) WMSAPPMN_EndDlg,
                      (uint32*) pMe);
      }

      return TRUE;


    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_ValidityFormatDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_DestAddrDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_DestAddrDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;
  IDialog* pIDialog;
  uint16 wSoftKeyCtrlID;
  uint16 wMenuCtrID;
  wms_msg_event_info_s_type *sms_msg_event_info_buf;


  switch(evt)
  {
    case EVT_DIALOG_START:
      (void)InitDestAddr(pMe,dw);
      if(pMe->m_msgInfo.boxType != WMSAPP_TEMPLATE)
      {
        WMSAPPMN_ReadDefaultTemplate(pMe);
      }
      else if(pMe->m_bTestMode)
      {
        /* The template was already read, just display */
        (void)WMSAPPMN_DisplayAddress(pMe);
      }
      return TRUE;

    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          return result;

        case AVK_DOWN:
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          wSoftKeyCtrlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

          (void)IDIALOG_SetFocus(pIDialog, (int16)wSoftKeyCtrlID);
          return TRUE;

        case AVK_UP:
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          wMenuCtrID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
          (void)IDIALOG_SetFocus(pIDialog, (int16)wMenuCtrID);
          return TRUE;

        case AVK_SELECT:
          (void)WMSAPPMN_StoreNumber(pMe);
          return TRUE;

        default:
          break;
      }
      return TRUE; // Digit keys should be ignored


    case EVT_COMMAND:
      if(pMe->m_bTestMode)
      {
        /* if the message is canned, only store the number and go to
        ** the view screen
        */
        (void)WMSAPPMN_StoreNumber(pMe);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    case EVT_WMS_MSG_READ_TEMPLATE:
      sms_msg_event_info_buf = (wms_msg_event_info_s_type *) dw;

      /* Assign the global template to the template that was read */
      (void)MEMCPY(pMe->m_pMO_template_msg, &sms_msg_event_info_buf->status_info.message,
            sizeof(wms_client_message_s_type));

      if (pMe->m_bComposeMessage)
      {
        /* Set the Text to the given Email / Phone Number */
        if (!WMSAPPMN_SetDestAddrText(pMe, (char *)pMe->m_composeAddress))
        {
          MSG_ERROR("WMSAPPMN_SetDestAddrText Failed!",0,0,0);
        }
        (void)WMSAPPMN_StoreNumber(pMe);
        WMSAPPU_FREE(pMe->m_composeAddress);
        pMe->m_bComposeMessage = FALSE;
      }
      else
      {
        (void)WMSAPPMN_DisplayAddress(pMe);
      }
      return TRUE;

    case EVT_WMS_DLG_RESUME:
      (void)WMSAPPMN_GetNumber(pMe);
      return TRUE;

    case EVT_APP_SUSPEND:
      (void)WMSAPPMN_StoreNumber(pMe);
      (void)ISHELL_EndDialog(pMe->a.m_pIShell); /* End the Compose dialog */
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_DestAddrDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_UserRelValDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_UserRelValEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;

  switch(evt)
  {
    case EVT_DIALOG_START:
      (void)WMSAPPMN_InitUserRelValDlg(pMe);
      return TRUE;

    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          return(WMSAPPMN_EndDlg(pMe));

        default:
          break;
      }
      return TRUE;

    case EVT_COMMAND:
      if (WMSAPPMN_StoreUserRelValDlg(pMe))
      {
        /* Valid Encoded Value */
        return(WMSAPPMN_EndDlg(pMe));
      }
      else
      {
        /* Invalid Encoded Value */
        (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Encoded TimeStamp Value", SELECTIONVERIFICATIONTIME);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_UserRelValDlgEventHandler */

#ifdef FEATURE_IMELODY
void WMSAPPMN_StopEventCb
(
  cmx_status_type status,
  const void *client_data
  
)
{

}
#endif

/*===========================================================================
FUNCTION WMSAPPMN_ViewImageDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ViewImageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    //(void)ISHELL_CancelTimer(pMe->a.m_pIShell, DisplaySMSIdleMenuDlg, pMe);
  }


  switch(evt)
  {
    case EVT_DIALOG_START:

      return WMSAPPMN_InitViewImgDlg(pMe);


    case EVT_KEY:
      if(w == AVK_CLR)
        return(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
      return TRUE;

    case EVT_CTL_SEL_CHANGED:
      return (WMSAPPMN_GetImageList(pMe));


    case EVT_COMMAND:
      if (WMSAPPMN_SetUserDataImage(pMe) == TRUE)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      else
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "User Data has Exceeded Capacity", SELECTIONVERIFICATIONTIME);
      }
      return TRUE;

    case EVT_DIALOG_END:
      {
        IDialog *pIDialog;
        WMSAPPMN_StopImages(pMe);

#ifdef FEATURE_IMELODY
        cmx_audfmt_stop((cmx_cb_func_ptr_type) WMSAPPMN_StopEventCb,
                        (void *)&pMe->m_melody_trans_id);
#endif

        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

        if(IDIALOG_GetID(pIDialog) == IDD_LIST_ATTACHMENTS)
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;


  }


} /* WMSAPPMN_ViewImageDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_ViewSMSDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ViewSMSDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IMenuCtl *pIMenu = NULL;
  IDialog* pIDialog;

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    //(void)ISHELL_CancelTimer(pMe->a.m_pIShell, DisplaySMSIdleMenuDlg, pMe);
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      //WMSAPPMD_Display(pMe);

      if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        pIMenu = (IMenuCtl * )IDIALOG_GetControl((IDialog*)dw,IDC_SK_OUTBOX_OPTNS);
        (void)IMENUCTL_DeleteItem(pIMenu, IDL_SK_OUTBOX_OPTIONS_EDIT);
        (void)IMENUCTL_DeleteItem(pIMenu, IDL_SK_OUTBOX_OPTIONS_STATUS);
      }
      else if(pMe->m_msgInfo.boxType == WMSAPP_INBOX)
      {
        pIMenu = (IMenuCtl * )IDIALOG_GetControl((IDialog*)dw,IDC_SK_INBOX);
      }
      if(pIMenu == NULL)
      {
        return FALSE;
      }
      SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

      return TRUE;


    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_CheckReturnDlg(pMe);

      }
      else if (w == AVK_SEND)
      {
      	#if defined(FEATURE_VERSION_C01) 
		{
			nv_item_type	SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select==AVK_SEND_TWO)
			{
				return TRUE;
			}
		}
		#endif
        if (WMSAPPMN_CallNumber(pMe) == FALSE)
        {
          if (!pMe->m_bSuspended)
          {
            (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Callback Number present", SELECTIONVERIFICATIONTIME);
          }
        }
      }
	  else if (w == AVK_CAMERA)
	  {

    	 #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
		 {
			nv_item_type	  SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select ==AVK_SEND_TWO)
			{
				 if (WMSAPPMN_CallNumber(pMe) == FALSE)
        		{
          			if (!pMe->m_bSuspended)
          			{
            			(void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Callback Number present", SELECTIONVERIFICATIONTIME);
          			}
        		}
			}
	      }
	     #endif
	  }
      return TRUE;

    case EVT_COMMAND:
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        pIMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,IDC_SK_OUTBOX_OPTNS);
      }
      else if(pMe->m_msgInfo.boxType == WMSAPP_INBOX)
      {
        pIMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,IDC_SK_INBOX);
      }
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

      if (pIMenu)
      {
        if(IMENUCTL_IsActive(pIMenu))
        {
          uint16 wItemID = IMENUCTL_GetSel(pIMenu);

          switch(wItemID)
          {
            case IDL_SK_INBOX_CALL:
              if (WMSAPPMN_CallNumber(pMe) == FALSE)
              {
                if (!pMe->m_bSuspended)
                {
                  (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Callback Number present", SELECTIONVERIFICATIONTIME);
                }
              }
              return TRUE;

            case IDL_SK_OUTBOX_OPTIONS_SAVE:
            case IDL_SK_INBOX_SAVE:
              return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));

            case IDL_SK_OUTBOX_OPTIONS_ERASE:
            case IDL_SK_INBOX_ERASE:
              return(WMSAPPMN_EraseMessage(pMe));

            case IDL_SK_OUTBOX_OPTIONS_SEND:
              return(WMSAPPMN_SendMessage(pMe));

            case IDL_SK_OUTBOX_OPTIONS_EDIT:
              return FALSE;

            case IDL_SK_OUTBOX_DONE:
            case IDL_SK_INBOX_DONE:
              if(pMe->m_msg_type == WMSAPP_MSG_TYPE_CANNED)
              {
                (void)WMSAPPMN_EndNDlgs(pMe,3);
              }
              else
              {
                if(pMe->m_bTestMode)
                {
                  (void)WMSAPPMN_EndNDlgs(pMe,2);
                }
              }
              return TRUE;

            case IDL_SK_INBOX_OTHERINFO:
              if(pMe->m_bTestMode)
              {
                return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));
              }

            case IDL_SK_INBOX_REPLY:
              pMe->m_msg_type = WMSAPP_MSG_TYPE_REPLY;
              return(WMSAPPMN_CreateMenuSelectedItemDlg(pMe));

            case IDL_SK_INBOX_FWD:
              pMe->m_msg_type = WMSAPP_MSG_TYPE_FWD;
              break;

            default:
              break;
          }
        }
      }
      return(WMSAPPMN_CreateMenuSelectedItemDlg(pMe));

    case EVT_DIALOG_END:
      {
        WMSAPPMN_StopImages(pMe);
      }
      return TRUE;

    case EVT_WMS_MSG_DELETE:
      (void)WMSAPPMN_EndNDlgs(pMe,2);
      if (WMSAPPMN_DisplayMsgDlg(pMe, IDS_MESSAGE_DELETED, NULL, SELECTIONVERIFICATIONTIME) == SUCCESS)
        return TRUE;

      return FALSE;

    case EVT_WMS_DLG_RESUME:
      if (WMSAPPU_GetMsgCount(pMe, pMe->m_msgInfo.boxType, pMe->m_msgInfo.memStore) == 0 ) {
         (void)WMSAPPMN_EndNDlgs(pMe,2);
      }
      else
      {
         (void)WMSAPPMD_Display(pMe);
      }



      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_ViewSMSDlgEventHandler */


/*===========================================================================
FUNCTION WMSAPPMN_SendCDMAReadAck

DESCRIPTION
  Send CDMA Read Ack message if requested by the sender

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_SendCDMAReadAck
(
  void *pUser
)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  
  if( pMe == NULL )
  {
    MSG_ERROR("Null pointer!",0,0,0);
    return;
  }

  if( pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_REPLY_OPTION
      &&
      pMe->m_pClient_msg->msg_hdr.tag == WMS_TAG_MT_NOT_READ
      &&
      pMe->m_pClient_ts->u.cdma.reply_option.read_ack_requested == TRUE )
  {
    /* build the message */
    pMe->m_pClient_msg_mo->u.cdma_message.is_mo = TRUE;
    pMe->m_pClient_msg_mo->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;

    /* Use the original address from MT SMS:
    */
    pMe->m_pClient_msg_mo->u.cdma_message.address =
            pMe->m_pClient_msg->u.cdma_message.address;

    pMe->m_pClient_msg_mo->u.cdma_message.is_tl_ack_requested = FALSE;
    pMe->m_pClient_msg_mo->u.cdma_message.is_service_present = FALSE;

    pMe->m_pClient_ts_mo->format = WMS_FORMAT_CDMA;
    pMe->m_pClient_ts_mo->u.cdma.mask = WMS_MASK_BD_MSG_ID;

    /* Use the same message ID number from the Inbox message:
    */
    pMe->m_pClient_ts_mo->u.cdma.message_id.id_number = 
              pMe->m_pClient_ts->u.cdma.message_id.id_number;

    pMe->m_pClient_ts_mo->u.cdma.message_id.type = WMS_BD_TYPE_READ_ACK;

    /* Initialized other to 0 */
    (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

    if (IWMS_TsEncode( pMe->m_pwms, pMe->m_pClient_ts_mo, & pMe->m_pClient_msg_mo->u.cdma_message.raw_ts)
              != SUCCESS)
    {
      MSG_ERROR(" Encoding test msg failed!", 0,0,0);
    }
    else
    {
      pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
      pMe->m_pClient_msg_mo->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
      pMe->m_pClient_msg_mo->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      pMe->m_pClient_msg_mo->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;

      if( IWMS_MsgSend(pMe->m_pwms, pMe->m_clientId, NULL,
                         (void*)pMe, WMS_SEND_MODE_CLIENT_MESSAGE,
                         pMe->m_pClient_msg_mo) != SUCCESS)
      {
        MSG_ERROR("Sending Read Ack failed", 0,0,0);
      }
    }
  }
} /* WMSAPPMN_SendCDMAReadAck() */

/*===========================================================================
FUNCTION WMSAPPMN_ViewMsgInfoDlgEvtHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ViewMsgInfoDlgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    //(void)ISHELL_CancelTimer(pMe->a.m_pIShell, DisplaySMSIdleMenuDlg, pMe);
  }

  switch(evt)
  {

    case EVT_DIALOG_START:

      {
        IMenuCtl* pIMenu;

        pIMenu = (IMenuCtl * )IDIALOG_GetControl((IDialog*)dw,IDC_SK_MESSAGE_INFO_NEXT);

        if (NULL != pIMenu)
        {
          (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenu);
        }
        return TRUE;
      }



    case EVT_KEY:
      if(w == AVK_CLR)
      {
        return(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
      }
      else if (w == AVK_SEND)
      {
      	#if defined(FEATURE_VERSION_C01) 
		{
			nv_item_type	SimChoice;
			OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
			if(SimChoice.sim_select==AVK_SEND_TWO)
			{
				return TRUE;
			}
		}
		#endif
        if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
        {
    #ifdef FEATURE_CDSMS
          /* Initialize other to 0 */
          (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

          if(IWMS_TsDecode( pMe->m_pwms, &(pMe->m_pClient_msg->u.cdma_message.raw_ts), pMe->m_pClient_ts) != SUCCESS)
          {
            MSG_ERROR("Decoding failed!",0,0,0);
          }
    #endif /*FEATURE_CDSMS*/
        }
        else
        {
    #ifdef FEATURE_GWSMS
#error code not present
    #endif /*FEATURE_GWSMS*/
        }

        if (WMSAPPMN_CallNumber(pMe) == FALSE)
        {
          if (!pMe->m_bSuspended)
          {
            (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Callback Number present", SELECTIONVERIFICATIONTIME);
          }
        }
      }
      else if (w == AVK_CAMERA)
      {
	      #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
			 {
				nv_item_type	  SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select ==AVK_SEND_TWO)
				{
					if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
			        {
			    #ifdef FEATURE_CDSMS
			          /* Initialize other to 0 */
			          (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

			          if(IWMS_TsDecode( pMe->m_pwms, &(pMe->m_pClient_msg->u.cdma_message.raw_ts), pMe->m_pClient_ts) != SUCCESS)
			          {
			            MSG_ERROR("Decoding failed!",0,0,0);
			          }
			    #endif /*FEATURE_CDSMS*/
			        }
			        else
			        {
			    #ifdef FEATURE_GWSMS
#error code not present
			    #endif /*FEATURE_GWSMS*/
			        }

			        if (WMSAPPMN_CallNumber(pMe) == FALSE)
			        {
			          if (!pMe->m_bSuspended)
			          {
			            (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Invalid Callback Number present", SELECTIONVERIFICATIONTIME);
			          }
			        }
				}
			}
		  #endif
	  }
      return TRUE;

    case EVT_COMMAND:
      /*(void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);*/

      if(pMe->m_msgInfo.boxType == WMSAPP_OUTBOX)
      {
        if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                         IDD_OUTBOX_VIEW, NULL) == EFAILED)
        { /* Creation failed */

          return FALSE;
        }
      }
      else if(pMe->m_msgInfo.boxType == WMSAPP_INBOX)
      {
        if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                         IDD_INBOX_VIEW, NULL) == EFAILED)
        { /* Creation failed */

          return FALSE;
        }

      }

      if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
        /* Initialize other to 0 */
        (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );
      
        if(IWMS_TsDecode( pMe->m_pwms, &(pMe->m_pClient_msg->u.cdma_message.raw_ts), pMe->m_pClient_ts) != SUCCESS)
        {
          MSG_ERROR("Decoding failed!",0,0,0);
        }
        else
        {
          /* For CDMA Inbox msg, if READ ACK is requested, send the READ ACK */
          if( pMe->m_msgInfo.boxType == WMSAPP_INBOX )
          {
            WMSAPPMN_SendCDMAReadAck( pMe );
          }
        }
#endif /*FEATURE_CDSMS*/
      }
      else
      {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
      }

      if(WMS_IS_MO(pMe->m_pClient_msg->msg_hdr.tag))
      {
        /*copy the buffer into client_ts_mo and client_msg_mo*/
        (void)MEMCPY(pMe->m_pClient_msg_mo,pMe->m_pClient_msg, sizeof(wms_client_message_s_type));
        (void)MEMCPY(pMe->m_pClient_ts_mo,pMe->m_pClient_ts, sizeof(wms_client_ts_data_s_type));
      }
      return (WMSAPPMD_Display(pMe));


    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;

    case EVT_WMS_DLG_RESUME:
       if (WMSAPPU_GetMsgCount(pMe, pMe->m_msgInfo.boxType, pMe->m_msgInfo.memStore) == 0)
       {
          (void)WMSAPPMN_EndNDlgs(pMe,2);
       }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_ViewMsgInfoDlgEvtHandler */

/*===========================================================================
FUNCTION WMSAPPMN_OutboxOptionsDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_OutboxOptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wItemID;
  IDialog* pIDialog;
  uint16 wMenuCtrID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitMenuSoftKey(pMe));
      /* no menu control ---> error */

    case EVT_KEY:
      if(w== AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_CheckReturnDlg(pMe);
      }
      return TRUE;

    case EVT_COMMAND:
      {

        wMenuCtrID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);

        pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wMenuCtrID);

        if(!IMENUCTL_IsActive(pICurrentMenu))
        {

          wMenuCtrID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);

          pICurrentMenu = (IMenuCtl * )IDIALOG_GetControl(pIDialog,(int16)wMenuCtrID);

          if(!IMENUCTL_IsActive(pICurrentMenu))
            return FALSE;
        }

        wItemID = IMENUCTL_GetSel(pICurrentMenu);

        switch(wItemID)
        {
          case IDL_SEND_MSG:
            return(WMSAPPMN_SendMessage(pMe));

          case IDL_ERASE_MSG:
            return(WMSAPPMN_EraseMessage(pMe));

          case IDL_EDIT_MSG:
            return TRUE;

          case IDL_MSG_PREF:
          case IDL_MSG_OTHERINFO:
          case IDL_MSG_STORE:
            (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
            return TRUE;

          case IDL_SK_OUTBOX_OPTIONS_DONE:
            (void)WMSAPPMN_DisplayMenuDlg(pMe, IDD_MAINSMS);
            return TRUE;

          default:
            break;
        }
      }
      return TRUE;

     case EVT_WMS_MSG_DELETE:
       (void)WMSAPPMN_EndNDlgs(pMe,4);

      if (WMSAPPMN_DisplayMsgDlg(pMe, IDS_MESSAGE_DELETED, NULL, SELECTIONVERIFICATIONTIME) == SUCCESS)
        return TRUE;

      return FALSE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_OutboxOptionsDlgEventHandler */


/*===========================================================================
FUNCTION WMSAPPMN_MsgDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MsgDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IStatic * pIStatic;
  IDialog * pIDialog;
  uint16 wCtlID;

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  
  if (pMe == NULL)
  {
    return FALSE;
  }

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell,  (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
  
    /* reset the idle screen timer */
    (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                     WMSAPPAPPTIMEOUT,
                     (PFNNOTIFY) WMSAPPMN_EndDlg,
                     (uint32*) pMe );
  }

  switch(evt)
  {
    case EVT_DIALOG_START:

      wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,(IDialog*)dw);
      pIStatic = (IStatic*) IDIALOG_GetControl((IDialog*)dw, (int16)wCtlID);
      WMSAPPMN_SetStaticTextStyle(pIStatic);
      return TRUE;

    case EVT_KEY:

      if(w == AVK_END)
      { /* go all the way back to idle screen */
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if(pIDialog == NULL)
        {
          return FALSE; /* error */
        }
        
        /* ending the message NOW -> cancel end message timer */
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
        (void)WMSAPPMN_EndDlg(pMe);

        return TRUE;
      }

      switch(w)
      {
        /* digit keys */
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
          /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
          /* send key */
        case AVK_SEND:
        #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
        case AVK_CAMERA:
        #endif
          /* clear key */
        case AVK_CLR:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          /* these key events will make the dialog go away and also cancel
          ** the timer if this action is taken before the timer is being
          ** triggered.
          */
           #if defined(FEATURE_VERSION_C01) 
			{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select==AVK_SEND_TWO && w == AVK_SEND)
				{
					return TRUE;
				}
			}
			#endif
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
          (void)WMSAPPMN_EndDlg(pMe);
          return TRUE;

        default:
          return FALSE; /* let core app handles these events */
      }

    case EVT_COMMAND:
      return TRUE;

    case EVT_DIALOG_END:
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
} /*WMSAPPMN_MsgDlgEventHandler*/

/*===========================================================================
FUNCTION WMSAPPMN_PrefDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_PrefDlgEventHandler
(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IDialog *pIDialog;
  uint16 wCtlID;
  CtlAddItem pai;
  IMenuCtl* pIMenu;

  CWMSAPP * pMe = (CWMSAPP*) pUser;

  if (pMe == NULL)
  {
    return FALSE;
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      (void)WMSAPPMN_InitMenuSoftKey(pMe);
      WMSAPPMN_GetCheckBoxItems(pMe);
      return TRUE;

    case EVT_KEY:
      if((w == AVK_END) || (w == AVK_CLR))
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return TRUE;
      }
      return TRUE;

    case EVT_WMS_MSG_WRITE_TEMPLATE:
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      //(void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;


    case EVT_COMMAND:
      pIDialog  = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
      pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

      if(IMENUCTL_IsActive(pIMenu))
      {
        if(wCtlID == IDC_MENU_CDMA_TEMPLATE_OPTNS ||
           wCtlID == IDC_GW_TMPLT_OPTIONS)
        {
          pMe->m_msgInfo.boxType = WMSAPP_TEMPLATE;
        }
        else
        {
          pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
        }
        /* Update the menu before going to the next dialog */
        (void)WMSAPPMN_SetCheckBoxItems(pMe, FALSE);

        return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));
      }
      // Look for the Soft Keys
      wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
      pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

      // Toggle Soft Key
      if(IMENUCTL_GetItemID(pIMenu, 0) == IMENUCTL_GetSel(pIMenu))
      {
        return (WMSAPPMN_SetCheckBoxItems(pMe, TRUE));
      }
      // Save Soft Key
      if(IMENUCTL_GetItemID(pIMenu, 1) == IMENUCTL_GetSel(pIMenu))
      {
        (void)WMSAPPMN_SaveSettings(pMe);
      }

      return TRUE;

    case EVT_KEY_PRESS:
      pIDialog  = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
      pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

      if(w == AVK_LEFT || w == AVK_RIGHT)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);
      }

      if(w == AVK_UP || w == AVK_DOWN)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
      }

      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
} /*WMSAPPMN_PrefDlgEventHandler*/

/*===========================================================================
FUNCTION WMSAPPMN_TmpltDlgEventHandler

DESCRIPTION
  Handle the template viewing dialog events.

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_TmpltDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;
  IMenuCtl* pIMenu;
  IMenuCtl* pISKMenu;
  IDialog *pIDialog;
  uint8 index;
  CtlAddItem pai;
  uint16 wSelectedItemID;
  uint8 defaultTemplate;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog));


  switch(evt)
  {
    case EVT_DIALOG_START:
      pMe->m_msgInfo.boxType = WMSAPP_TEMPLATE;
      return(WMSAPPMN_InitTemplate(pMe));

    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          result = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          (void)WMSAPPMN_CheckReturnDlg(pMe);
          return result;
        default:
          break;
      }
      return TRUE;

    case EVT_COMMAND:
      if(IMENUCTL_IsActive(pIMenu))
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);

        // Setting current selection to Bold
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu, wItemID, MSIF_FONT, &pai);

        (void)IMENUCTL_Redraw(pIMenu);

        index = (uint8)WMSAPPMN_GetListIndex(pMe, wItemID);

        if(index ==0)
        {
          WMSAPPMN_ReadTemplate(pMe,(uint32)WMS_DUMMY_MESSAGE_INDEX);
        }
        else
        {
          if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
          {
            WMSAPPMN_ReadTemplate(pMe,pMe->m_pCDSMSTemplateList->indices[index-1]);
          }
          else
          {
            WMSAPPMN_ReadTemplate(pMe,pMe->m_pGWSMSTemplateList->indices[index-1]);
          }
        }

        pMe->m_msgInfo.write_mode = WMS_WRITE_MODE_REPLACE;
      }
      else // Soft Key is Active
      {
        switch(IMENUCTL_GetSel(pISKMenu))
        {
          case IDL_SK_NEW_TMPLT:
            // Create Separate Dialogs for GW and CDMA.

            (void)MEMSET(pMe->m_pMO_template_msg,0,sizeof(wms_client_message_s_type));

            if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
            {
              if(ISHELL_CreateDialog(pMe->a.m_pIShell,WMSAPP_RES_FILE,IDD_CDMA_TEMPLATE_OPTIONS, NULL)!= SUCCESS)
                 return FALSE;  /* ERROR */
              pMe->m_pMO_template_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
            }
            else
            {
              if(ISHELL_CreateDialog(pMe->a.m_pIShell,WMSAPP_RES_FILE,IDD_GW_TEMPLATE_OPTIONS, NULL)!= SUCCESS)
                 return FALSE;  /* ERROR */
              pMe->m_pMO_template_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_SIM;
            }
            pMe->m_msgInfo.write_mode = WMS_WRITE_MODE_INSERT;

            break;

          case IDL_SK_ERASE_TMPLT:
            index = (uint8)WMSAPPMN_GetListIndex(pMe,IMENUCTL_GetSel(pIMenu));
            if(index != 0)
            {
              if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
              {
                /* A RUIM Card Template */
                pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pCDSMSTemplateList->indices[index-1];
              }
              else
              {
                /* A SIM Card Template */
                pMe->m_msgInfo.smsIndex = (uint8)pMe->m_pGWSMSTemplateList->indices[index-1];
              }
              WMSAPPMN_DeleteTemplate(pMe);
            }
            else
            {
              /* NV Template */
              /* Erasing an NV Template is same as writing Default Values;
                 since we can never erase an NV Template */
              WMSAPPMN_WriteDefaultNVTemplate(pMe);
            }
            break;

          case IDL_SK_DEFAULT_TMPLT:
            wSelectedItemID = IMENUCTL_GetSel(pIMenu);
            defaultTemplate = (uint8)WMSAPPMN_GetListIndex(pMe, wSelectedItemID);
            if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
            {
              if (defaultTemplate != pMe->m_CDSMSDefaultTemplate)
              {
                pMe->m_CDSMSDefaultTemplate = defaultTemplate;
                (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_CDMA_DEFAULT_TEMPLATE, &pMe->m_CDSMSDefaultTemplate, 1);
                (void)WMSAPPMN_InitTemplate(pMe);
              }
            }
            else
            {
              if (defaultTemplate != pMe->m_GWSMSDefaultTemplate)
              {
                pMe->m_GWSMSDefaultTemplate = defaultTemplate;
                (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_GW_DEFAULT_TEMPLATE, &pMe->m_GWSMSDefaultTemplate, 1);
                (void)WMSAPPMN_InitTemplate(pMe);
              }
            }
            return TRUE;

          default:
            break;
        }
      }

      return TRUE;


    case EVT_WMS_MSG_READ_TEMPLATE:
      return (WMSAPPMN_ViewTemplate(pMe, (wms_msg_event_info_s_type *)dw));


    case EVT_DIALOG_END:
      return TRUE;

    case EVT_KEY_PRESS:
      if(w == AVK_LEFT || w == AVK_RIGHT)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);
      }

      if(w == AVK_UP || w == AVK_DOWN)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
      }

    return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_TmpltDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_MelodyDlgEventHandler

DESCRIPTION
  Handles the selectiona dn play of Imelody tunes from the
  Imelody Attach Screen

RETURN
  boolean if the evt was handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MelodyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  boolean result;
  IMenuCtl* pIMenu;
  IMenuCtl* pISKMenu;
  IDialog *pIDialog;
  CtlAddItem pai;
  uint16 wDlgID;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog));
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog));

  wDlgID = IDIALOG_GetID(pIDialog);

  switch(evt)
  {
    case EVT_DIALOG_START:
      if (wDlgID == IDD_LIST_MELODY)
        return(WMSAPPMN_InitMenuDlg(pMe,(IDialog*)dw));
      else
        return(WMSAPPMN_InitMelodyDlg(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        result = (SUCCESS == ISHELL_EndDialog(pMe->a.m_pIShell));
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return result;
      }
      return TRUE;

    case EVT_COMMAND:
      if(IMENUCTL_IsActive(pIMenu))
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);

        // Setting current selection to Bold
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu, wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);

        // Insert the Appropriate Imelody item in Message Header
        // Then return to the Message Dialog
        if (WMSAPPMN_SetUserDataImage(pMe) == TRUE)
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        }
        else
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "User Data has Exceeded Capacity", SELECTIONVERIFICATIONTIME);
        }
        return TRUE;

      }
      else // Soft Key is Active
      {
        switch(IMENUCTL_GetSel(pISKMenu))
        {
          case IDL_SK_MELODY_SELECT:
          case IDL_SK_USER_MELODY_SELECT:
            if (WMSAPPMN_SetUserDataImage(pMe) == TRUE)
            {
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
            }
            else
            {
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              (void)WMSAPPMN_DisplayMsgDlg( pMe, 0, "User Data has Exceeded Capacity", SELECTIONVERIFICATIONTIME);
            }
            return TRUE;

          case IDL_SK_MELODY_PLAY:
          case IDL_SK_USER_MELODY_PLAY:
#ifdef FEATURE_IMELODY
            // Play the appropriate Imelody
            (void)WMSAPPMD_PlayMelody(pMe, wDlgID, (uint16)WMSAPPMN_GetMelodyListIndex(IMENUCTL_GetSel(pIMenu)));
#endif /* FEATURE_IMELODY */
            break;

          case IDL_SK_MELODY_DONE:
          case IDL_SK_USER_MELODY_DONE:
            result = (SUCCESS ==ISHELL_EndDialog(pMe->a.m_pIShell));
            (void)WMSAPPMN_CheckReturnDlg(pMe);
            return result;

          default:
            break;
        }
      }
      return TRUE;

    case EVT_DIALOG_END:
      {
        WMSAPPMN_StopImages(pMe);
#ifdef FEATURE_IMELODY
        cmx_audfmt_stop((cmx_cb_func_ptr_type) WMSAPPMN_StopEventCb,
                        (void *)&pMe->m_melody_trans_id);
#endif
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

        if(IDIALOG_GetID(pIDialog) == IDD_LIST_ATTACHMENTS)
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE;

    case EVT_KEY_PRESS:
      if(w == AVK_LEFT || w == AVK_RIGHT)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);
      }
      if(w == AVK_UP || w == AVK_DOWN)
      {
        uint16 wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
      }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
#else
  return TRUE;
#endif /*FEATURE_SMS_UDH && !FEATURE_SMS_EXTENDED*/
} /* WMSAPPMN_MelodyDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_BCDlgEventHandler

DESCRIPTION
  Handle 1st level of BC dialogs.

RETURN
  boolean if the evt was handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_BCDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS_BROADCAST
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  //boolean result;
  IMenuCtl* pIMenu;
  IMenuCtl* pISKMenu;
  IDialog *pIDialog;
  uint16 wItemID;
  uint16 SKCtlID;
  uint16 nextDlgID;
  uint16 currDlgID;
  uint32 itemData=0;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  SKCtlID  = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
  currDlgID = WMSAPPMN_GetDlgMenuCtlID(pMe, pIDialog);

  pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)SKCtlID);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)currDlgID);
  wItemID  = IDIALOG_GetID(pIDialog);

  if((pISKMenu == NULL || pIMenu == NULL) && wItemID != IDD_BC_CONFIG)
  {
    return FALSE;
  }

  switch(evt)
  {

    case EVT_DIALOG_START:
      /* Which dialog to display */
      switch(wItemID)
      {
        case IDD_BC_CONFIG:
          return WMSAPPMN_InitBCConfig(pMe);

        case IDD_BC_PREF:
          return WMSAPPMN_InitBCPref(pMe);

        case IDD_BC_TABLE:
          return WMSAPPMN_InitBCTableOptions(pMe);

        default:
          MSG_ERROR("Invalid BC Dlg!",0,0,0);
          return FALSE;
      }

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        return(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
      }
      return TRUE;

    case EVT_COMMAND:
      if(pISKMenu && IMENUCTL_IsActive(pISKMenu))
      {
        if(w == IDL_BC_DONE_1 || w == IDL_BC_DONE_2 || w == IDL_BC_DONE_3)
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        }
        else
        {
          /* create the next dialog */
          WMSAPPMN_StopImages(pMe);
          nextDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, w);

          /* End the current dialog to avoid filling the dialog stack, order of
          ** events is important here.
          */
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);

          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 nextDlgID, NULL) == EFAILED)
          { /* Creation failed */
            return FALSE;
          }
        }
      }
      else if(pIMenu && IMENUCTL_IsActive(pIMenu))
      {
        if(wItemID == IDD_BC_PREF)
        {
          (void)IMENUCTL_GetItemData(pIMenu, w, &itemData);
          WMSAPPMN_BCSetPref(pMe, (wms_bc_pref_e_type)itemData);
        }
        else if(wItemID == IDD_BC_TABLE)
        {
          /* switch on the selection */
          switch(w)
          {
            case IDL_BC_TABLE_EDIT:
            case IDL_BC_TABLE_SET_PRIORITY_ALL_SVCS:
              if(w == IDL_BC_TABLE_SET_PRIORITY_ALL_SVCS)
              {
                /* set this value appropriately to update priority correctly */
                pMe->m_svcPriority = WMSAPP_BC_ALL_LANGUAGES;
              }

              /* create the next dialog */
              WMSAPPMN_StopImages(pMe);
              nextDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe, w);

              if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                     nextDlgID, NULL) == EFAILED)
              { /* Creation failed */
                return FALSE;
              }
              break;

            case IDL_BC_TABLE_RESET_SVCS:
              WMSAPPMN_BCResetAllSvcs(pMe, WMSAPP_BC_ALL_LANGUAGES);
              (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_ALL_SRVS_RESET, NULL, SELECTIONVERIFICATIONTIME);
              break;

            case IDL_BC_TABLE_SET_SVCS:
              WMSAPPMN_BCSetAllSvcs(pMe, WMSAPP_BC_ALL_LANGUAGES);
              (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_ALL_SRVS_SET, NULL, SELECTIONVERIFICATIONTIME);
              break;

            case IDL_BC_TABLE_DELETE_ALL_SVCS:
              if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                     IDD_CONFIRM, NULL) == EFAILED)
              { /* Creation failed */
                return FALSE;
              }
              break;



            default:
              MSG_ERROR("Invalid BC Table option!",0,0,0);
              return FALSE;
          }
        }
      }
      return TRUE;

    case EVT_WMS_CONFIRM:
      if(dw == WMSAPP_CONFIRM_YES)
      {
        WMSAPPMN_BCDeleteAllSvcs(pMe);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
} /* WMSAPPMN_BCDlgEventHandler() */


/*===========================================================================
FUNCTION WMSAPPMN_BCEditDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_BCEditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS_BROADCAST
  CWMSAPP *pMe = (CWMSAPP *)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  IMenuCtl* pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  IMenuCtl* pISKMenu = NULL;
  uint16 wItemID, wDlgID;
  CtlAddItem pai;

  if (pMe == NULL)
  {
    return FALSE;
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      (void)WMSAPPMN_InitMenuSoftKey(pMe);
      return(WMSAPPMN_InitBCEdit(pMe));

    case EVT_KEY:
      if((w == AVK_END) || (w == AVK_CLR))
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return TRUE;
      }
      return TRUE;

    case EVT_COMMAND:
      if(IMENUCTL_IsActive(pIMenu))
      {
        pMe->m_currPref = IMENUCTL_GetSel(pIMenu);
        /* Get the current language */
        pMe->m_currLang = WMSAPPMN_GetBCLanguageIndex(pMe->m_currPref);

        return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));
      }

      wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
      pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
      wItemID = IMENUCTL_GetSel(pISKMenu);
      // Look for the Soft Keys
      switch (w)
      {
        case IDL_SK_BC_SERVICES:
        case IDL_SK_BC_PRIORITY:
          if(w == IDL_SK_BC_PRIORITY)
          {
            uint16 tempItem;
            /* Need to set the priority for a particular language */
            tempItem = IMENUCTL_GetSel(pIMenu);
            (void)IMENUCTL_GetItemData(pIMenu, tempItem, (uint32*)&pMe->m_svcPriority);
          }
          else
          {
            /* IDL_SK_BC_SERVICES */
            pMe->m_currPref = IMENUCTL_GetSel(pIMenu);
            /* Get the current language */
            pMe->m_currLang = WMSAPPMN_GetBCLanguageIndex(pMe->m_currPref);
          }
          WMSAPPMN_StopImages(pMe);
          wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe,wItemID);
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 wDlgID,NULL) == EFAILED)
          {
            return FALSE;
          }
          return TRUE;

        case IDL_SK_BC_TOGGLE:
          return (WMSAPPMN_SetCheckBoxItems(pMe, TRUE));

        default:
          break;
      }
      return TRUE;

    case EVT_KEY_PRESS:
      if(w == AVK_LEFT || w == AVK_RIGHT)
      {
        wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);
      }

      if(w == AVK_UP || w == AVK_DOWN)
      {
        wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
      }
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
} /*WMSAPPMN_BCEditDlgEventHandler*/

#ifdef FEATURE_CDSMS_BROADCAST

static uint16 WMSAPPMN_GetBCServItemID
(
  uint8 index
)
{
  return bcServItems[index];
}

boolean WMSAPPMN_InitBCServices
(
  CWMSAPP *pMe
)
{
  AECHAR  *label;
  uint8   i;
  uint16  nItemID;
  CtlAddItem pai;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  IMenuCtl* pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  /* Clear the menu items */
  (void)IMENUCTL_DeleteAll(pIMenu);

  /* currLang should be set prior to entering */
  for(i=0; i<pMe->m_bcSortedSvcs[pMe->m_currLang].size; i++)
  {
    if (i >= SERVITEMS_SIZE || i >= (WMS_BC_TABLE_MAX/WMSAPP_MAX_LANGUAGES))
    {
      return FALSE;
    }

    label = MALLOC(sizeof(AECHAR)*30);

    if (label == NULL) {
      return FALSE;
    }

    (void)STRTOWSTR(pMe->m_bcInfo.entries[pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[i]].label,
              label,
              (int)((int)sizeof(AECHAR)*(STRLEN(pMe->m_bcInfo.entries[pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[i]].label))+1));    
    nItemID = WMSAPPMN_GetBCServItemID(i);

    /* Add the item */
    if(i>=IMENUCTL_GetItemCount(pIMenu))
    {
      /* use the index as the item data to differentiate later */
      (void)IMENUCTL_AddItem(pIMenu, WMSAPP_RES_FILE, IDS_NO_MESSAGES,
                       nItemID, NULL, (uint32)i);

      /* Now set the label */
      (void)IMENUCTL_SetItemText(pIMenu, nItemID, WMSAPP_RES_FILE, 0, label);

      /* Finally set the correct check box item */
      if(IMENUCTL_GetItem(pIMenu, nItemID, &pai))
      {
        pai.pszResImage = pai.pszResText = WMSAPP_RES_FILE;
        pai.pImage = NULL;
        if ((pMe->m_currLang < WMSAPP_MAX_LANGUAGES) &&
            (pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[i] < WMS_BC_TABLE_MAX) &&
            (pMe->m_bcInfo.entries[pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[i]].selected))
        {
          pai.wImage = IDB_CHECK_YES;
        }
        else
        {
          pai.wImage = IDB_CHECK_NO;
        }
        (void)IMENUCTL_SetItem(pIMenu, nItemID, MSIF_IMAGE, &pai);
      }
    }

    WMSAPPU_FREE(label);
  }
  (void)IMENUCTL_Redraw(pIMenu);
  return TRUE;
}
#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
FUNCTION WMSAPPMN_BCServicesDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_BCServicesDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS_BROADCAST

  CWMSAPP *pMe = (CWMSAPP *)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  IMenuCtl* pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  IMenuCtl* pISKMenu;
  uint16 wItemID, wDlgID;
  uint32 itemData = 0;
  uint16 currSel = 0;
  CtlAddItem pai;

  switch(evt)
  {
    case EVT_DIALOG_START:
      (void)WMSAPPMN_InitMenuSoftKey(pMe);
      return(WMSAPPMN_InitBCServices(pMe));

    case EVT_KEY:
      if((w == AVK_END) || (w == AVK_CLR))
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
      return TRUE;

    case EVT_COMMAND:
      if(IMENUCTL_IsActive(pIMenu))
      {
        pMe->m_currPref = IMENUCTL_GetSel(pIMenu);
        /* get the item data from the menu ctl */
        (void)IMENUCTL_GetItemData(pIMenu, pMe->m_currPref, &pMe->m_bcCurrIndex);

        /* Display the service data */
        if (ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                    IDD_BC_SVC_DISPLAY, NULL) == SUCCESS)
        {
          return TRUE;
        }
        else
          return FALSE;
      }
      // Look for the Soft Keys
      wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
      pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
      wItemID = IMENUCTL_GetSel(pISKMenu);
      currSel = IMENUCTL_GetSel(pIMenu);
      /* get the item data from the menu ctl */
      (void)IMENUCTL_GetItemData(pIMenu, currSel, &itemData);

      /* Assign the currIndex */
      if(wItemID != IDL_SK_BC_ADD_NEW_SRV)
      {
        pMe->m_bcCurrIndex = pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[(uint8)itemData];
      }

      /* Some options may use currPref as notification that this is BC */
      pMe->m_currPref = WMSAPP_NO_PREF;

      switch (wItemID)
      {
        case IDL_SK_BC_PRI:
        case IDL_SK_BC_SRV_DEL:
        case IDL_SK_BC_EDIT_LABEL:
          if(wItemID == IDL_SK_BC_PRI)
          {
            pMe->m_svcPriority = WMSAPP_BC_SINGLE_SVC;
          }
          WMSAPPMN_StopImages(pMe);
          wDlgID = WMSAPPMN_GetNextSelItemDlgID(pMe,wItemID);
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 wDlgID,NULL) == EFAILED)
          {
            return FALSE;
          }
          return TRUE;

        case IDL_SK_BC_SRV_TOGGLE:
          if(pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].selected)
          {
            pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].selected = FALSE;
          }
          else
          {
            pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].selected = TRUE;
          }
          WMSAPPMN_BCSelectService(pMe,
                                   &pMe->m_bcInfo.entries[pMe->m_bcCurrIndex]);
          return (WMSAPPMN_SetCheckBoxItems(pMe, TRUE));

        case IDL_SK_BC_ADD_NEW_SRV:
          if(ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDD_BC_ADD_SVC,NULL) == EFAILED)
          {
            return FALSE;
          }
          return TRUE;

        case IDL_SK_BC_MAX_MSGS:
        case IDL_SK_BC_ALERT_MODE:
        default:
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_FEATURE_NOT_AVAILABLE, NULL, SELECTIONVERIFICATIONTIME);
          break;

      }
      return TRUE;

    case EVT_KEY_PRESS:
      if(w == AVK_LEFT || w == AVK_RIGHT)
      {
        wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_BOLD;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
        (void)IMENUCTL_Redraw(pIMenu);
      }

      if(w == AVK_UP || w == AVK_DOWN)
      {
        wItemID = IMENUCTL_GetSel(pIMenu);
        (void)IMENUCTL_GetItem(pIMenu, wItemID, &pai);
        pai.wFont = AEE_FONT_NORMAL;
        (void)IMENUCTL_SetItem(pIMenu,wItemID, MSIF_FONT, &pai);
      }
      return TRUE;

    case EVT_WMS_CONFIRM:
      if(dw == WMSAPP_CONFIRM_YES)
      {
        WMSAPPMN_BCDeleteService(pMe);
      }
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
}/*WMSAPPMN_BCServicesDlgEventHandler*/

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION WMSAPPMN_InitBCSvcDisplay

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitBCSvcDisplay
(
  CWMSAPP *pMe
)
{
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IStatic* pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog,
                                                     IDC_STATIC_BC_SVC_DISPLAY);
  uint16 wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
  IMenuCtl* pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  char  *buf;
  AECHAR *pText;
  AECHAR *pTitle;
  int    offset=0;
  uint32 bcTableIndex;
  uint8   *svcId = (uint8*)("SERVICE ID: 0x");
  uint8   *sel   = (uint8*)("SELECTED:   ");

  if(pIStatic == NULL || pISKMenu == NULL)
  {
    return FALSE;
  }

  bcTableIndex = pMe->m_bcSortedSvcs[pMe->m_currLang].entryIndex[pMe->m_bcCurrIndex];

  /* Set the title of the service data */
  buf = MALLOC(100);
  pTitle   = MALLOC(sizeof(AECHAR) * WMS_BC_MM_SRV_LABEL_SIZE);
  pText = MALLOC(sizeof(AECHAR) * 100);

  if (buf == NULL || pTitle == NULL || pText == NULL) {
    return FALSE;
  }

  (void)MEMCPY((char*)buf,
         pMe->m_bcInfo.entries[bcTableIndex].label,
         (STRLEN((char*)pMe->m_bcInfo.entries[bcTableIndex].label)+1));
  (void)STRTOWSTR((char*)buf, pTitle,
            (int)((int)sizeof(AECHAR) * (STRLEN((char*)pMe->m_bcInfo.entries[bcTableIndex].label)+1)));

  /* Set the data of the service */
  (void)MEMSET(buf, 0, 100);


  /* Service ID */
  buf[offset++] = '\n';
  (void)MEMCPY(buf+offset, svcId, STRLEN((char*)svcId));
  offset += (int)STRLEN((char*)svcId);

  buf[offset++] = WMSAPPU_HexChar((uint8)(pMe->m_bcInfo.entries[bcTableIndex].srv_id.bc_srv_id.service/16));
  buf[offset++] = WMSAPPU_HexChar((uint8)(pMe->m_bcInfo.entries[bcTableIndex].srv_id.bc_srv_id.service%16));
  buf[offset++] = '\n';

  /* Service Language */
  offset += WMSAPPU_FormatLanguage(pMe->m_bcInfo.entries[bcTableIndex].srv_id.bc_srv_id.language,
                                   (uint8*)buf+offset);
  buf[offset++] = '\n';

  /* is service selected */
  (void)MEMCPY(buf+offset, sel, STRLEN((char*)sel));
  offset += (int)STRLEN((char*)sel);

  if(pMe->m_bcInfo.entries[bcTableIndex].selected)
  {
    buf[offset++] = 'T';
  }
  else
  {
    buf[offset++] = 'F';
  }
  buf[offset++] = '\n';

  /* priority */
  offset += WMSAPPU_FormatPriority(pMe->m_bcInfo.entries[bcTableIndex].priority,
                                   (uint8*)buf+offset);

  offset++;
  (void)STRTOWSTR(buf, pText, (int)((int)sizeof(AECHAR)*offset));

  (void)ISTATIC_SetText(pIStatic, pTitle, pText, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  /* Display the softkeys */
  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_BC_SVC_DISPLAY);
  IMENUCTL_SetFocus(pISKMenu, IDL_SK_BC_SVC_DISPLAY_DONE);
  (void)IMENUCTL_Redraw(pISKMenu);

  WMSAPPU_FREE(buf);
  WMSAPPU_FREE(pText);
  WMSAPPU_FREE(pTitle);
  return TRUE;
}
#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
FUNCTION  WMSAPPMN_BCAddServiceEvtHandler

DESCRIPTION
  Handles evts for the dialog to add a BC service.

RETURN
  boolean, if evts are handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_BCServiceDisplayEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS_BROADCAST
  CWMSAPP *pMe = (CWMSAPP *)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
  IMenuCtl* pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);

  if(pISKMenu == NULL)
  {
    return FALSE;
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitBCSvcDisplay(pMe));

    case EVT_KEY:
      if((w == AVK_END) || (w == AVK_CLR))
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
      return TRUE;

    case EVT_COMMAND:
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      //(void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
}/*WMSAPPMN_BCAddServiceEvtHandler*/

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION  WMSAPPMN_InitBCAddService

DESCRIPTION
  Initialized the BC add service dialog.

RETURN
  boolean, if initialization was successful.

DEPENDENCIES
  None

SIDE EFFECTS
  Default values are set in pMe->m_bcNewSvc.
===========================================================================*/
boolean WMSAPPMN_InitBCAddService
(
  CWMSAPP *pMe
)
{
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  IMenuCtl* pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  IMenuCtl *pISKMenu;
  wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe, pIDialog);
  pISKMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  if(pIMenu == NULL)
  {
    return FALSE;
  }

  /* Set some default values for the new svc */
  pMe->m_bcNewSvc.srv_id.bc_srv_id.service = WMS_SRV_UNKOWN;
  pMe->m_bcNewSvc.srv_id.bc_srv_id.language = WMS_LANGUAGE_ENGLISH;
  (void)MEMCPY(pMe->m_bcNewSvc.label, "New Svc", STRLEN("New Svc"));
  pMe->m_bcNewSvc.priority = WMS_PRIORITY_NORMAL;
  pMe->m_bcNewSvc.selected = TRUE;

  /* Display the menu and softkeys. */
  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);
  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISKMenu);

  return TRUE;
} /* WMSAPPMN_InitBCAddService() */
#endif /* FEATURE_CDSMS_BROADCAST */

/*===========================================================================
FUNCTION  WMSAPPMN_BCAddServiceEvtHandler

DESCRIPTION
  Handles evts for the dialog to add a BC service.

RETURN
  boolean, if evts are handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_BCAddServiceEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS_BROADCAST
  CWMSAPP *pMe = (CWMSAPP *)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  uint16 wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  IMenuCtl* pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16)wCtlID);
  uint16 wItemID;

  if(pIMenu == NULL)
  {
    return FALSE;
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      return(WMSAPPMN_InitBCAddService(pMe));

    case EVT_KEY:
      if((w == AVK_END) || (w == AVK_CLR))
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
      return TRUE;

    case EVT_COMMAND:
      /* Check if DONE SK was selected */
      if(!IMENUCTL_IsActive(pIMenu))
      {
        /* done with adding a new service */
        WMSAPPMN_BCAddService(pMe, &pMe->m_bcNewSvc);
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }

      /* A menu item was selected */
      wItemID = IMENUCTL_GetSel(pIMenu);

      switch(wItemID)
      {
        case IDL_BC_ADD_SVC_SRV_ID:
        case IDL_BC_ADD_SVC_LANGUAGE:
        case IDL_BC_ADD_SVC_LABEL:
        case IDL_BC_ADD_SVC_PRIORITY:
          /* Signifies this is for a new service */
          pMe->m_bcCurrIndex = WMSAPP_BC_NO_INDEX;
          break;

        case IDL_BC_ADD_SVC_ALERT:
        case IDL_BC_ADD_SVC_MAX_MSGS:
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_FEATURE_NOT_AVAILABLE, NULL, SELECTIONVERIFICATIONTIME);
          return TRUE;

        default:
          MSG_HIGH("Invalid menu item selected",0,0,0);
          break;
      }
      return (WMSAPPMN_CreateMenuSelectedItemDlg(pMe));

    case EVT_DIALOG_END:
      /* Might have to refresh the dialog going back to */
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return TRUE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
} /* WMSAPPMN_BCAddServiceEvtHandler() */

/*===========================================================================
FUNCTION  WMSAPPMN_InitSimpleText

DESCRIPTION
  Initialize the simple text dialog.  Sets text and cursor if needed.

RETURN
  boolean, if initialization was successful.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitSimpleText(CWMSAPP *pMe)
{
  IDialog  *pIDialog;
  ITextCtl *pIText;
  AECHAR *text;
  uint8  *unpackBuf;
#ifdef FEATURE_CDSMS
  uint8  *textBuf = NULL;
  uint8  length = 0;
#endif /* FEATURE_CDSMS */

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(!pIDialog)
  {
    return FALSE;
  }

  pIText = (ITextCtl *)IDIALOG_GetControl(pIDialog, IDC_SIMPLE_COMPOSE);
  if(pIText == NULL)
  {
    return FALSE;
  }
  /* Initialize the text control text to null */
  (void)ITEXTCTL_SetText(pIText, NULL, 0);

  text = MALLOC(WMS_MAX_LEN);
  unpackBuf = MALLOC(WMS_MAX_LEN);

  if(text == NULL || unpackBuf == NULL)
  {
    WMSAPPU_FREE(text);
    WMSAPPU_FREE(unpackBuf);
    return FALSE;
  }

  (void)MEMSET(text, 0, sizeof(text));
  (void)MEMSET(unpackBuf, 0, sizeof(unpackBuf));

#ifdef FEATURE_CDSMS
  /* template processing for user data */
  if(pMe->m_currPref == IDL_C_TMPLT_USER_DATA)
  {
    if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
       (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA))
    {
      /* Display the user data in the template, unpack data accordingly */
      if((pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_ASCII) ||
         (pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_IA5))
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len =
          IWMS_TsUnpackAscii( pMe->m_pwms, &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data,
                              (uint8)pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits+1,
                              (uint8 *)unpackBuf);
      }
      /* First check if the template contains a non std email address, we
      ** don't want to display the non-std email address
      */
      if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR) &&
         (WMSAPPU_IsNonStdEmail(TRUE, pMe->m_pMO_template_msg->u.cdma_template.address.digits,
                                pMe->m_pMO_template_msg->u.cdma_template.address.number_of_digits)))
      {
        /* Set the encoding as OCTET */
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding =
          WMS_ENCODING_OCTET;

        /* Extract the text and store the email addr in the email buffer */
        length = pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len;
        pMe->m_emailAddr.len = WMSAPPU_GetNonStdEmail(unpackBuf, pMe->m_emailAddr.address);
        pMe->m_emailAddr.isNonStdEmail = TRUE;

        textBuf = MALLOC(WMS_MAX_LEN);
        if(textBuf == NULL)
        {
          return FALSE;
        }

        (void)MEMCPY(textBuf, unpackBuf+pMe->m_emailAddr.len, length-pMe->m_emailAddr.len);
        (void)MEMSET(unpackBuf, 0, sizeof(unpackBuf));
        (void)MEMCPY(unpackBuf, textBuf, length-pMe->m_emailAddr.len);
        WMSAPPU_FREE(textBuf);
      }
      else
      {
        pMe->m_emailAddr.isNonStdEmail = FALSE;
      }
    }
  }
#endif /*FEATURE_CDSMS*/
  (void)STRTOWSTR((char *)unpackBuf, text, sizeof(text));
  /* set the text to the text control */

  (void)ITEXTCTL_SetText(pIText, text, sizeof(text));
  ITEXTCTL_SetCursorPos(pIText, sizeof(text));
  WMSAPPU_FREE(text);
  WMSAPPU_FREE(unpackBuf);
  return TRUE;
} /* WMSAPPMN_InitSimpleText() */

/*===========================================================================
FUNCTION  WMSAPPMN_PutSimpleText

DESCRIPTION
  Put the text entered in the text control in the appropriate pMe structure.

RETURN
  boolean if updating a structure of pMe was successful.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_PutSimpleText(CWMSAPP *pMe)
{
  IDialog  *pIDialog;
  ITextCtl *pIText;
  AECHAR *text;
  uint8  *textBuf;

  pIDialog = (IDialog *)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(!pIDialog)
  {
    return FALSE;
  }

  pIText = (ITextCtl *)IDIALOG_GetControl(pIDialog, IDC_SIMPLE_COMPOSE);
  if(pIText == NULL)
  {
    return FALSE;
  }

  text     = MALLOC((sizeof(AECHAR))*(WMS_MAX_LEN+1));
  textBuf  = MALLOC(WMS_MAX_LEN+1);

  if (text == NULL || textBuf == NULL) {
    return FALSE;
  }

  /* Get the text from the control */
  if(!(ITEXTCTL_GetText(pIText, text, sizeof(AECHAR)*WMS_MAX_LEN)))
  {
    WMSAPPU_FREE(text);
    WMSAPPU_FREE(textBuf);
    return FALSE;
  }

#ifdef FEATURE_CDSMS_BROADCAST
  if(pMe->m_currPref == WMSAPP_NO_PREF)
  {
    /* This text control was used to change a label */
    (void)WSTRTOSTR(text, (char *)textBuf, WMS_MAX_LEN);
    if(STRLEN((char*)textBuf)+1 > WMS_BC_MM_SRV_LABEL_SIZE)
    {
      (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_LABEL_TOO_LONG, NULL, SELECTIONVERIFICATIONTIME);
    }
    else
    {
      if(pMe->m_bcCurrIndex != WMSAPP_BC_NO_INDEX)
      {
        (void)MEMCPY(pMe->m_bcInfo.entries[pMe->m_bcCurrIndex].label, textBuf,
               STRLEN((char*)textBuf)+1);
      }
      else
      {
        /* Clear the default label or previous label */
        (void)MEMSET(pMe->m_bcNewSvc.label, 0, WMS_BC_MM_SRV_LABEL_SIZE);
        (void)MEMCPY(pMe->m_bcNewSvc.label, textBuf,
               STRLEN((char*)textBuf)+1);
      }
    }
    WMSAPPU_FREE(text);
    WMSAPPU_FREE(textBuf);
    return TRUE;
  }
#endif /* FEATURE_CDSMS_BROADCAST */

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    (void)WSTRTOSTR(text, (char *)textBuf, WMS_MAX_LEN);

    pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len =
      pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits =
      (uint8)STRLEN((char*)textBuf);

    if(pMe->m_currPref == IDL_C_TMPLT_USER_DATA)
    {
      if(pMe->m_emailAddr.isNonStdEmail)
      {
        /* put the email address first then the text */


        WMSAPPU_PutNonStdEmail(pMe->m_emailAddr.address, pMe,
                               &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data);

        /* reset the textBuf */
        (void)MEMSET(textBuf, 0, sizeof(textBuf));

        /* copy the user data into the textBuf before packing */
        (void)MEMCPY(textBuf,
               pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
               pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len);

      }

      /* check if DCS is available in the template, otherwise keep the text as OCTET */
      if(((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_DCS) &&
         ((pMe->m_pMO_template_msg->u.cdma_template.dcs == WMS_ENCODING_ASCII) ||
          (pMe->m_pMO_template_msg->u.cdma_template.dcs == WMS_ENCODING_IA5))) ||
         ((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
          (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_USER_DATA) &&
          ((pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_ASCII) ||
           (pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.encoding == WMS_ENCODING_IA5)))
         || (pMe->m_emailAddr.isNonStdEmail)
        )
      {
        pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.number_of_digits =
          IWMS_TsPackAscii( pMe->m_pwms, (char *)textBuf,
                            pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
                            &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len,
                            &pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.padding_bits);
      }
      else
      {
        /* Keep the encoding as 8-bit */
        (void)MEMCPY(pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data,
               textBuf, pMe->m_pMO_template_msg->u.cdma_template.client_bd.user_data.data_len);
      }
      pMe->m_pMO_template_msg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
      pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask |= WMS_MASK_BD_USER_DATA;
    }
#endif /*FEATURE_CDSMS*/
  }

  WMSAPPU_FREE(text);
  WMSAPPU_FREE(textBuf);
  return TRUE;
} /* WMSAPPMN_PutSimpleText() */

/*===========================================================================
FUNCTION  WMSAPPMN_SimpleTextDlgEventHandler

DESCRIPTION
  Handle the reply text input dialog events.

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_SimpleTextDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP *pMe = (CWMSAPP*)pUser;
  
  switch(evt)
  {

    case EVT_DIALOG_START:
      /* Do any initialization of the text input control if needed */
      return(WMSAPPMN_InitSimpleText(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
      else if(w == AVK_SELECT)
      {
        /* Done with inputting, update the correct structure of pMe */
        if(WMSAPPMN_PutSimpleText(pMe))
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        }
      }
      return TRUE;

    case EVT_COMMAND:
      return TRUE;

    case EVT_DIALOG_END:
      /* Check if we got here by modifying a preference or not */
      if(pMe->m_currPref == WMSAPP_NO_PREF)
      {
        /* go back to the appropriate dialog and refresh if needed */
        (void)WMSAPPMN_CheckReturnDlg(pMe);
      }
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_SimpleTextDlgEventHandler() */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION  WMSAPPMN_UpdateReplyOptions

DESCRIPTION
  Update the reply options dialog by checking yes or no, depending on the
  value of replyOption.

RETURN
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMN_UpdateReplyOptions
(
  CWMSAPP *pMe,
  uint16  replyOption,
  boolean value
)
{
  IMenuCtl  *pIMenu;
  IDialog   *pIDialog;
  uint16    wCtlID;
  CtlAddItem pai;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  wCtlID = WMSAPPMN_GetDlgMenuCtlID(pMe,pIDialog);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)wCtlID);

  /* Set the check boxes appropriately */

  (void)IMENUCTL_GetItem(pIMenu, (uint16)replyOption, &pai);
  pai.pszResImage = pai.pszResText = WMSAPP_RES_FILE;
  pai.pImage = NULL;
  if(value)
  {
    pai.wImage = IDB_CHECK_YES;
  }
  else
  {
    pai.wImage = IDB_CHECK_NO;
  }
  (void)IMENUCTL_SetItem(pIMenu, (uint16)replyOption, MSIF_IMAGE, &pai);

  (void)IMENUCTL_Redraw(pIMenu);
} /* WMSAPPMN_UpdateReplyOptions() */
#endif /*FEATURE_CDSMS*/

/*===========================================================================
FUNCTION  WMSAPPMN_ReplyOptionsDlgEventHandler

DESCRIPTION
  Handle the reply options dialog events.

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ReplyOptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_CDSMS
  CWMSAPP *pMe = (CWMSAPP*)pUser;
  
  switch(evt)
  {

    case EVT_DIALOG_START:
      (void)WMSAPPMN_InitMenuSoftKey(pMe);
      /* processing for a template */
      if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
      {
        /* Set the initial values of the template */
        if((pMe->m_pMO_template_msg->u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
           (pMe->m_pMO_template_msg->u.cdma_template.client_bd.mask & WMS_MASK_BD_REPLY_OPTION))
        {
          WMSAPPMN_UpdateReplyOptions(pMe, IDL_DELIVERY_ACK,
                                      pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested);
          WMSAPPMN_UpdateReplyOptions(pMe, IDL_USER_ACK,
                                      pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested);
        }
      }
      else
      {
        /* processing for a MO message */
        if(pMe->m_pClient_ts_mo->u.cdma.mask & WMS_MASK_BD_REPLY_OPTION)
        {
          WMSAPPMN_UpdateReplyOptions(pMe, IDL_DELIVERY_ACK,
                                      pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested);
          WMSAPPMN_UpdateReplyOptions(pMe, IDL_USER_ACK,
                                      pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested);
        }
      }

      return(TRUE);

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
      else if(w == AVK_SELECT)
      {
        /* Put the text in the template user data */
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE;

    case EVT_COMMAND:
      /* Delivery ack processing */
      if(w == IDL_DELIVERY_ACK)
      {
        /* template */
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          /* toggle the pMe structure */
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested = FALSE;
          }
          else
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested = TRUE;
          }
          /* check/uncheck the reply options dlg. */
          WMSAPPMN_UpdateReplyOptions(pMe, w,
                                      pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.delivery_ack_requested);
        }
        else /* MO message */
        {
          /* toggle the pMe structure */
          if(pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested)
          {
            pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested = FALSE;
          }
          else
          {
            pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested = TRUE;
          }
          /* check/uncheck the reply options dlg. */
          WMSAPPMN_UpdateReplyOptions(pMe, w,
                                      pMe->m_pClient_ts_mo->u.cdma.reply_option.delivery_ack_requested);
        }
      }
      /* User ack processing */
      else if(w == IDL_USER_ACK)
      {
        /* template */
        if(pMe->m_msgInfo.boxType == WMSAPP_TEMPLATE)
        {
          /* toggle the pMe structure */
          if(pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested)
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested = FALSE;
          }
          else
          {
            pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested = TRUE;
          }
          /* check/uncheck the reply options dlg. */
          WMSAPPMN_UpdateReplyOptions(pMe, w,
                                      pMe->m_pMO_template_msg->u.cdma_template.client_bd.reply_option.user_ack_requested);
        }
        else  /* MO message */
        {
          /* toggle the pMe structure */
          if(pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested)
          {
            pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested = FALSE;
          }
          else
          {
            pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested = TRUE;
          }
          /* check/uncheck the reply options dlg. */
          WMSAPPMN_UpdateReplyOptions(pMe, w,
                                      pMe->m_pClient_ts_mo->u.cdma.reply_option.user_ack_requested);
        }
      }
      /* DONE softkey, finished with selection */
      else if(w == IDL_SK_REPLY_OPTIONS_DONE)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
#else
  return TRUE;
#endif /* FEATURE_CDSMS */
} /* WMSAPPMN_ReplyOptionsDlgEventHandler() */

/*===========================================================================
FUNCTION  WMSAPPMN_InitConfirm

DESCRIPTION
  Initialize the confirmation dialog and display it.

RETURN
  boolean if the dialog was initialized properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitConfirm(CWMSAPP *pMe)
{
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IStatic *pIStatic = (IStatic *)IDIALOG_GetControl(pIDialog, IDC_STATIC_CONFIRM);
  IMenuCtl *pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,IDC_SK_CONFIRM);

  /* Set the text properties */
  (void)ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
  ISTATIC_SetFont(pIStatic, AEE_FONT_BOLD, AEE_FONT_BOLD);

  if(pISKMenu)
  {
     (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
  }
  else
  {
    return FALSE;
  }

  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_CONFIRM);
  IMENUCTL_SetFocus(pISKMenu, IDL_CONFIRM_NO);
  (void)IMENUCTL_Redraw(pISKMenu);

  return TRUE;
} /* WMSAPPMN_InitConfirm() */

/*===========================================================================
FUNCTION  WMSAPPMN_ConfirmDlgEventHandler

DESCRIPTION
  Handle the confirmation dialog events. "Are you sure?"

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ConfirmDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP *pMe = (CWMSAPP*)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,IDC_SK_CONFIRM);
  static int response = WMSAPP_CONFIRM_NONE;

  switch(evt)
  {

    case EVT_DIALOG_START:
      return (WMSAPPMN_InitConfirm(pMe));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        response = WMSAPP_CONFIRM_NONE;
        return(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
      }
      return TRUE;

    case EVT_COMMAND:
      (void)IMENUCTL_GetItemData(pISKMenu, w, (uint32*)&response);
      return(ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);

    case EVT_DIALOG_END:
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_CONFIRM, 0, (uint32)response);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_ConfirmDlgEventHandler() */

/*===========================================================================
FUNCTION WMSAPPMN_ProcessMTMessage

DESCRIPTION
  Processes the received MT message if needed.

RETURN
  boolean, if message was processed correctly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ProcessMTMessage
(
  CWMSAPP *pMe,
  wms_msg_event_info_s_type  *evt
)
{
  if ((evt->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
          && (evt->mt_message_info.message.u.gw_message.is_broadcast == TRUE))
  {
    /* GSM Cell broadcast Message: Already handled */
    WMSAPPU_FREE(evt);
    return TRUE;
  }
  /* copy the message into the internal structure */
  (void)MEMCPY(pMe->m_pClient_msg, &evt->mt_message_info.message,
    sizeof(wms_client_message_s_type));

  if (pMe->m_pClient_msg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    /* Check the route, if it's TRANSFER AND ACK or TRANSFER ONLY,
    ** then store the message in NV.
    */
    if(evt->mt_message_info.route == WMS_ROUTE_TRANSFER_AND_ACK ||
       evt->mt_message_info.route == WMS_ROUTE_TRANSFER_ONLY)
    {
      wmsapp_box_type tempBoxType = pMe->m_msgInfo.boxType;

      /* Ack the message if required */
      if(evt->mt_message_info.route == WMS_ROUTE_TRANSFER_ONLY)
      {
        pMe->m_ackInfo.message_mode = WMS_MESSAGE_MODE_CDMA;
        pMe->m_ackInfo.transaction_id = evt->mt_message_info.transaction_id;
        if (pMe->m_pMemStatuses[WMS_MEMORY_STORE_NV_CDMA]->free_slots == 0)
        {
          /* Since the memory is full. Set the cause code and ack the message */
          pMe->m_ackInfo.u.cdma.tl_status = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;
          pMe->m_ackInfo.u.cdma.error_class = WMS_ERROR_TEMP;

          /* Finally ack the message */
          (void)WMSAPPMN_AckMessage(pMe);
          WMSAPPU_FREE(evt);
          return FALSE;
        }
        else
        {
          pMe->m_ackInfo.u.cdma.error_class = WMS_ERROR_NONE;
          (void)WMSAPPMN_AckMessage(pMe);
        }
      }

      if( IWMS_CfgCheckCDMADuplicate( pMe->m_pwms,
                                      & evt->mt_message_info.message ) == TRUE )
      {
        /* This is a duplicate message, don't store it */
        WMSAPPU_FREE(evt);
        return FALSE; /* Don't display alert for the dup */
      }

      /* set some variables before storing the message */
      pMe->m_msgInfo.boxType = WMSAPP_INBOX;

      pMe->m_pClient_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;

      /* Now store the message */
      (void)WMSAPPMN_StoreMessage(pMe);

      pMe->m_msgInfo.boxType = tempBoxType;
    }
#endif /* FEATURE_CDSMS */
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  WMSAPPU_FREE(evt);
  return TRUE;
} /* WMSAPPMN_ProcessMTMessage() */

/*===========================================================================
FUNCTION WMSAPPMN_InitNewMtMsg

DESCRIPTION
  Initialize the display of the new mt msg alert dialog.

RETURN
  boolean, if the dialog was initialized properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_InitNewMtMsg
(
  CWMSAPP *pMe
)
{
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,IDC_SK_NEW_MSG_RXD);
  IStatic *pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, IDC_STATIC_NEW_MSG_RXD);
  char    *text = MALLOC(NOTIFY_STRING_LEN);
  AECHAR  *message = MALLOC(sizeof(AECHAR)*NOTIFY_STRING_LEN);

  if((pIStatic == NULL) || (pISKMenu == NULL) || (text == NULL) || (message == NULL))
  {
    MSG_ERROR("Memory Cannot be Allocated in InitNewMtMsg", 0, 0, 0);
    WMSAPPU_FREE(text);
    WMSAPPU_FREE(message);
    return FALSE;
  }

  (void)MEMSET(text, 0, NOTIFY_STRING_LEN);
  (void)MEMSET(message, 0, sizeof(AECHAR)*NOTIFY_STRING_LEN);

  if (pMe->m_notify == WMSAPP_NOTIFY_NEW_MESSAGE)
  {
    switch(pMe->m_pClient_msg->msg_hdr.mem_store)
    {
      case WMS_MEMORY_STORE_RAM_CDMA:
        (void)STRLCPY(text, "New Message in CDMA Temporary Memory!", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_RAM_GW:
        (void)STRLCPY(text, "New Message in GW Temporary Memory!", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_RUIM:
        (void)STRLCPY(text, "New Message in the CDMA Card!", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_SIM:
        (void)STRLCPY(text, "New Message in the GW Card!", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_NV_CDMA:
        (void)STRLCPY(text, "New Message in CDMA Phone Memory!", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_NV_GW:
      default:
        (void)STRLCPY(text, "New Message in GW Phone Memory!", NOTIFY_STRING_LEN);
        break;
    }
  }
  else if (pMe->m_notify == WMSAPP_NOTIFY_MEMORY_FULL)
  {
    /* Display the correct alert */
    switch(pMe->m_msgInfo.memStore)
    {
      case WMS_MEMORY_STORE_RAM_CDMA:
        (void)STRLCPY(text, "CDMA Temporary Memory is FULL", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_RAM_GW:
        (void)STRLCPY(text, "GW Temporary Memory is FULL", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_RUIM:
        (void)STRLCPY(text, "CDMA Card Memory is FULL", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_SIM:
        (void)STRLCPY(text, "GW Card Memory is FULL", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_NV_CDMA:
        (void)STRLCPY(text, "CDMA Phone Memory is FULL", NOTIFY_STRING_LEN);
        break;

      case WMS_MEMORY_STORE_NV_GW:
      default:
        (void)STRLCPY(text, "GW Phone Memory is FULL", NOTIFY_STRING_LEN);
        break;
    }
  }
  else if (pMe->m_notify == WMSAPP_NOTIFY_NEW_GW_BROADCAST_MESSAGE)
  {
    (void)STRLCPY(text, "New GSM Cell Broadcast Message has arrived", NOTIFY_STRING_LEN);
  }
  else if (pMe->m_notify == WMSAPP_NOTIFY_NEW_VOICEMAIL)
  {
    (void)STRLCPY(text, "New Voicemail Message", NOTIFY_STRING_LEN);
  }
  else if (pMe->m_notify == WMSAPP_NOTIFY_NEW_VOICEMAIL_LINE2)
  {
    (void)STRLCPY(text, "New Voicemail Message (Line 2)", NOTIFY_STRING_LEN);
  }
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */
  else
  {
    MSG_ERROR("No Text to be Displayed in InitNewMtMsg", 0, 0, 0);
    WMSAPPU_FREE(text);
    WMSAPPU_FREE(message);
    return FALSE;
  }

  (void)STRTOWSTR(text, message, (STRLEN(text)+1)*sizeof(AECHAR));
  (void)ISTATIC_SetProperties(pIStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
  (void)ISTATIC_SetText(pIStatic, NULL, message, AEE_FONT_BOLD, AEE_FONT_BOLD);

  WMSAPPU_FREE(text);
  WMSAPPU_FREE(message);

  (void)IMENUCTL_DeleteAll(pISKMenu);

  (void)IMENUCTL_AddItem(pISKMenu, WMSAPP_RES_FILE, IDS_QUIT, IDL_SK_MSG_RXD_QUIT, NULL, 0);

  /* Don't Display the READ softkey */
  if ((pMe->m_notify != WMSAPP_NOTIFY_MEMORY_FULL) && (pMe->m_notify != WMSAPP_NOTIFY_NEW_STATUS_REPORT_NOSTORE))
  {
    (void)IMENUCTL_AddItem(pISKMenu, WMSAPP_RES_FILE, IDS_READ, IDL_SK_MSG_RXD_READ, NULL, 0);
  }

  /* Display the softkeys */
  (void)SetDefaultSoftkeyLook(pMe->a.m_pIShell,pISKMenu);
  (void)IDIALOG_SetFocus(pIDialog, IDC_SK_NEW_MSG_RXD);

  if ((pMe->m_notify != WMSAPP_NOTIFY_MEMORY_FULL) && (pMe->m_notify != WMSAPP_NOTIFY_NEW_STATUS_REPORT_NOSTORE))
  {
    IMENUCTL_SetFocus(pISKMenu, IDL_SK_MSG_RXD_READ);
  }
  else
  {
    IMENUCTL_SetFocus(pISKMenu, IDL_SK_MSG_RXD_QUIT);
  }

  IDIALOG_Redraw(pIDialog);

  return TRUE;
} /* WMSAPPMN_InitNewMtMsg() */

/*===========================================================================
FUNCTION WMSAPPMN_NewMtMsgDlgEventHandler

DESCRIPTION
  Handle the new message alert dialog events.

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_NewMtMsgDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP *pMe = (CWMSAPP*)pUser;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl *pISKMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,IDC_SK_NEW_MSG_RXD);
  uint16 select=0;
  boolean retVal;

  if(pMe->m_pStaticSecText != NULL)
  {
    (void)ISTATIC_Release(pMe->m_pStaticSecText);
    pMe->m_pStaticSecText = NULL;
  }


  switch(evt)
  {

    case EVT_DIALOG_START:
      /* Cancel any previous pending timers */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return (WMSAPPMN_InitNewMtMsg(pMe));

    case EVT_KEY:
      if((w == (uint16)AVK_END) || (w == (uint16)AVK_CLR))
      {
        if(pMe->m_bAlertOnly == TRUE)
        {
          /* reset the flag */
          pMe->m_bAlertOnly = FALSE;
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;
        }
        else
        {
          retVal = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          (void)WMSAPPMN_CheckReturnDlg(pMe);
          return retVal;
        }
      }
      return TRUE;

    case EVT_COMMAND:
      /*Stop The Alerts*/
      uisnd_snd_stop();
      select = IMENUCTL_GetSel(pISKMenu);
      if(select == IDL_SK_MSG_RXD_READ)
      {
        /* Simply end all the open WMSAPP Dialogs
           and return to the Main SMS menu dialog */
        (void)WMSAPPMN_DisplayMenuDlg(pMe, IDD_MAINSMS);

        (void)WMSAPPMN_CheckReturnDlg(pMe);

        if (pMe->m_notify != WMSAPP_NOTIFY_NEW_GW_BROADCAST_MESSAGE)
        {
          if (WMSAPPMN_WmsappModeToWmsMessageMode(pMe->m_modeControl) == pMe->m_pClient_msg->msg_hdr.message_mode)
          {
            (void)ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                              EVT_WMSAPP_MESSAGE_READ, 0, (uint32)&pMe->m_pClient_msg->msg_hdr);
          }
          else
          {
            (void)WMSAPPMN_DisplayMsgDlg(pMe, 0, "Incompatible Message Mode", SELECTIONVERIFICATIONTIME);
          }
        }
        else
        {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
        }
        return TRUE;
      }
      else
      {
        if(pMe->m_bAlertOnly == TRUE)
        {
          /* reset the flag */
          pMe->m_bAlertOnly = FALSE;
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return TRUE;
        }
        else
        {
          retVal = (ISHELL_EndDialog(pMe->a.m_pIShell) == SUCCESS);
          (void)WMSAPPMN_CheckReturnDlg(pMe);
          return retVal;
        }
      }

    case EVT_DIALOG_END:
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_NewMtMsgDlgEventHandler() */

/*===========================================================================
FUNCTION WMSAPPMN_MemUsageDlgEvtHandler

DESCRIPTION
  Handle the memory useage dialog events.

RETURN
  boolean if evts were handled and/or handled properly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_MemUsageDlgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;

  IMenuCtl* pIMenu = NULL;
  uint16 wItemID,wCtlID;
  IDialog* pIDialog;

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell,  (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
 
    /* reset the idle screen timer */
    (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                     WMSAPPAPPTIMEOUT,
                     (PFNNOTIFY) WMSAPPMN_EndDlg,
                     (uint32*) pMe );
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL) return FALSE;
  wCtlID = WMSAPPMN_GetDlgSKCtlID(pMe,pIDialog);
  if(wCtlID == 0) return FALSE;
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog,(int16)wCtlID);
  if(pIMenu == NULL) return FALSE;

  switch(evt)
  {

    case EVT_DIALOG_START:
      if (WMSAPPMN_InitStaticDlg(pMe))
      {
        IMENUCTL_SetSel(pIMenu, IDL_SK_USAGE);
        return TRUE;
      }
      return FALSE;


    case EVT_KEY:
    case EVT_KEY_PRESS:
      switch(w)
      {
        case AVK_CLR:
         (void)WMSAPPMN_EndNDlgs(pMe,1);
         return TRUE;

        case AVK_UP:
        case AVK_DOWN:
          (void)IDIALOG_SetFocus(pIDialog, (int16)wCtlID);

          return TRUE;
        default:
          break;
      }
      return TRUE; // Digit keys should be ignored

    case EVT_CTL_SEL_CHANGED:
      wItemID = IMENUCTL_GetSel(pIMenu);

      switch(wItemID)
      {
        case IDL_SK_USAGE_INBOX:
          pMe->m_msgInfo.boxType= WMSAPP_INBOX;
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DLG_RESUME, 0, 0);
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          break;

        case IDL_SK_USAGE_OUTBOX:
          pMe->m_msgInfo.boxType = WMSAPP_OUTBOX;
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DLG_RESUME, 0, 0);
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
           break;

        default:
          return FALSE;
      }
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WMSAPPMN_EndDlg, pMe);
      return TRUE;


    case EVT_DIALOG_END:
       return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }

} /* WMSAPPMN_MemUsageDlgEvtHandler() */
/*===========================================================================
FUNCTION WMSAPPMN_RoutesDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_RoutesDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CWMSAPP * pMe = (CWMSAPP*) pUser;
  
  switch(evt)
  {
    case EVT_DIALOG_START:
      return (WMSAPPMN_InitRoutesDlg(pMe,(IDialog*)dw));

    case EVT_KEY:
      if(w == AVK_CLR)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)WMSAPPMN_CheckReturnDlg(pMe);
        return TRUE;
      }
      if(w == AVK_SELECT)
      {
        (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      }
      return TRUE; // Digit keys should be ignored

    case EVT_COMMAND:
      (void)WMSAPPMN_CreateMenuSelectedItemDlg(pMe);
      return TRUE;

    case EVT_DIALOG_END:
      (void)WMSAPPMN_CheckReturnDlg(pMe);
      return TRUE;


    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* WMSAPPMN_RoutesDlgEventHandler */

/*===========================================================================
FUNCTION WMSAPPMN_CBMenuDlgEventHandler

DESCRIPTION
  Handle Cell Broadcast Idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_CBMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif /* FEATURE_GWSMS_BROADCAST */
} /* WMSAPPMN_CBMenuDlgEventHandler */


/*===========================================================================
FUNCTION WMSAPPMN_ServiceEditDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ServiceEditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#else
  return FALSE;
#endif /* FEATURE_GWSMS_BROADCAST */
} /* WMSAPPMN_ServiceEditDlgEventHandler */

/*========================End of File ================================================*/
#endif /*FEATURE_WMS_APP*/

