/*===========================================================================

FILE: catapp.c

SERVICES: Sample Card Application Toolkit (CAT) applet showing usage of
          BREW ISTK interfaces.

DESCRIPTION
  This file contains the implementation of a CatApp applet using the
  AEE ISTK interface services.
  CAT app

  Demonstrates usage of
  (1) ISTK API

       Copyright © 2000-2009 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

===========================================================================*/
/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/cat/rel/07H1_2/src/catapp.c#13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/09   xz      Fix issue of unable to send error TR of SELECT ITEM
06/03/09   sg      Changed display text to sustain until user MMI action
05/14/09   xz      Change DTMF Pause timer to 3200 ms
05/05/09   yb      Fixed issue of help information in MENU SELECTION cmd
04/15/09   xz      Fix issue of duration TLV in DISPLAY TEXT cmd
03/17/09   xz      Fix crash when GET INPUT timer expires
03/04/09   yb      Added support for sending L3 data in response for various
                   SS and USSD operations.
02/13/09   tml     Do not simulate select until diag sends an explicit select
                   key for menu related dialogs
02/25/09   yb      Added support to skip FDN check in Call Control
01/28/09   xz      Fix issue of sending DTMF pause
01/08/09   tml     Add handling for END key for get input/inkey and select item
                   Add more debug messages
01/08/09  tml     Merge fix for CatApp automation setup call YES key handling
11/25/08   sk      Allow Get Inkey to send duration object in the terminal
                   response when user does not provide an input.
11/24/08   js      Fixed error fatal for select menu automation
11/18/08   js      Fixed text buffer copy logic for catapp automation
10/30/08   js      Fixed featurization for automation changes.
10/29/08   js      CatApp automation changes
09/25/08   yb      Fixed lint error
09/15/08   yb      Added support for parallel BIP and Voice Calls
08/29/08   yb      Fixed lint error
07/22/08   js      Fix in DiagReadDisplayMenu() - initialized memory block
                   Fix in DiagSelectItem() - corrected format specifier
06/23/08   yb      Changed the feature name from FEATURE_APP_TEST_AUTOMATION
                   to FEATURE_CATAPP_TEST_AUTOMATION
06/23/08   yb      Added a check for null ptr for inetmgr instance before
                   registering for net events
05/02/08   sk      Fixed comment typo and tpdu related error for send sms
03/28/07   sk      Fix for support of Null alpha for send SS, USSD, DTMF,
                   Set Up Idle Text for HDK UI
02/29/08   yb      Corrected position of MSG_HIGH while getting class ID from
                   URL for IDLE MODE TEXT
02/27/08   yb      Fixed Send SS and Send USSD response in case ISuppSvc is
                   not supported
02/13/08   yb      Fixing Non Adherence of coding standards for SCM guidelines
01/03/08   sk      Release InetMgr instance for an Opened Channel only after
                   receiving successful pdp context deactivation event
12/18/07   yb      Fixed Compilation issue due to feature
                   FEATURE_APP_TEST_AUTOMATION
11/27/07   yb      Replaced the deprecated functions with new functions
                   Removed Lint error introduced by FEATURE_UIONE_HDK
11/21/07   sk      Added UIONE support for Send DTMF, Send SS,
                   Send USSD alpha
11/20/07   sk      Added support for Idle Screen Event for UIONE
                   Changed support for Lang Notification, User Activity Event
                   to listen to BREW events
11/19/07   sk      Send appropriate TR when 93 00 is returned for mo sms
                   control for send sms
10/16/07   sk      Fix for Send SMS - MO SMS ctrl
10/03/07   sk      Added support for setup idle text for uiOne
09/13/07   sk      Changed SetIdleMenuText()to SetIdleMenuTextInBkgrnd()
08/29/07   sk      Corrected additional info enums
08/21/07   sk      CatApp will no longer exit if ISuppsvc creation fails.
08/20/07   sk      Added display text support for normal text for uiOne
08/12/07   sk      Fixed Backward move problem for Get Input when there is
                   a default text present
05/22/07   sun     Updated to account for changes to IPDP interface
05/07/07   nk      Featurized Battery Support
03/22/07   sk      Set suspended flag to TRUE on receiving
                   EVT_APP_START_BACKGROUND
03/14/07   sun     Handle EVT_KEY_PRESS and End Key explicitly in
                   CataApp_Handlevent to close the app on an END KEY.
02/26/07   sk      Removed reference to IAnnuciator
02/21/07   sun     Start Catapp in Background using NMASK_SHELL_INIT
01/30/07   sk      Added changes for ISuppSvc_StartSSWithOpts.
                   Added support for SS null alpha string.
01/16/07   sk      Fixed support for sending DTMF while processing set up call
                   request from card.
01/15/07   sk      Revert changes for ISuppSvc_StartSSWithOpts
12/22/06   sk      Close dialog if app is in suspended mode and after TR is sent
                   upon user timeout as no End proactive session may come in.
12/15/06   alb     Changed prototype of ISUPPSVC_StartSSWithOpts.
12/14/06   sk      Support for ss cfu - interrogate
11/13/06   nk      Added support for selecting end key in diag select item
                   function
11/09/06   nk      Featurized proactive command event being sent under
                   automation feature and added sending the event after
                   getinput dialog created.  Added payload to be sent with diag
                   events.
11/03/06   sk      Fixed compilation warning.
10/25/06   sk      Set linger time for sockets to fix 30 sec delay
                   in PDP context deactivation.
10/24/06   sk      Fixed End Proactive session issue when app is in suspended
                   mode.
10/23/06   sk      Fixed SMS issue to not send TR to the card when user
                   presses any key during the display of alpha.
                   Added check for TR flag when NW error is received for SMS
10/04/06   sun     Fixed Compile Error
10/02/06   sk      Fixed memory issue
09/27/06   nk      Added WriteText Diag Function
09/19/06   sk      Fixed typo error
09/13/06   nk      Fixed ReadDisplayText timing issue
09/13/06   sk      Changed Feature name
09/13/06   sk      Corrected AEESUPPSVC_EVENT_DATA field name.
09/12/06   sk      Removed lint errors for 1X.
09/11/06   sk      Fixed lint error
09/11/06   nk      Added ReadDisplayMenu and SelectItem diag command.
09/11/06   sk      Fixed compilation warnings and lint errors.
                   Featurized support for send ss - cfu
09/07/06   sk      Support for Send SS - CFU
08/17/06   sk      Corrected comments, Removed warning for 1x.
08/09/06   jk      Replaced call to non-exported wms function for dual-proc.
                   Fixed lint errors
07/27/06   sk      Fixed compile error
07/25/06   sk      Added messages,setup menu refresh after adding, deleting
                   menu items, close channel cnf needs to be sent after PDP Context
                   activation confirmation from NW, link drop channel status
                   needs to be made available as free slot
06/27/06   jk      Added featurization so that 7500 can continue to use old Sound
                   interface
06/29/06   sk      Fixed lint error
06/26/06   sk      Changes to not allow data available event when the receive
                   data buffer has not been previously read by the card.
                   Fixed set up menu, next action identifier value 00.
06/26/06   nk      Fixed truncated text in Read Display and ensured num AEChars
                   returned instead of total number of bytes
06/14/06   sk      Fixed compilation problems.
06/12/06   nk      Added support for Read Display Text IDIAG Automation
06/09/06   sk      Fixed UCS2 issue related to get input
05/31/06   jk      1x CCAT Support for SMS
05/31/06   sk      Fixed lint errors
05/25/06   sk      Fixed send data issues.
                   Fixed Receive data issues.
                   Fixed binding of socket to port during Open Ch.
05/21/06   sk      Fixed lint errors
05/20/06   sk      Fixed compilation issue.
05/19/06   sk      Removed ds header file.
05/19/06   sk      Lint fixes.
05/18/06   sk      Re-architecture for supporting multple PDP  context
                   Lint fixes.
                   Open Channel fix for IP address.
05/06/06   tml     Fixed compilation
05/03/06   sk      Fixed Lint Errors
                   Fixed problem in Get Channel Status
                   Fixed send data - no of bytes issue
04/24/06   sk      Corrected dcs for Send SMS for "packing required".
04/21/06   sk      Undo changes in catapp_cm_call_cmd_orig().
04/19/06   sk      Fixed check in catapp_cm_call_cmd_orig() for accomodating
                   max dialled strings.
04/04/06   sk      Added support for variable timeout and immediate digit
                   response for get inkey cmd.
04/04/06   sk      Added support for duration object in display txt cmd.
                   Added support for battery status.
03/20/06   sk      Added check for cause values from ICM
03/16/06   sk      Fixed Added support for tone and sound
                   for playtone cmd.
                   Added support for new event
                   AEECM_EVENT_ALL_CALLS_ENDED.
                   Fixed Lint errors.
03/13/06   sk      Fixed potential data abort problem
02/16/05   sk      Corrected additional info fields in
                   catapp_access_conditions_satified() when ME is
                   busy on a call
                   Lint fixes
           tml     Added support for various end status for event
                   AEECM_EVENT_CALL_END from ICM
12/23/05   sk      Corrected Null ptr check for Channel Status
12/06/05   sk      Modified mask for ss error code and added shifting of bytes.
12/05/05   sk      Added support for a field in SuppSvcOpt
                   to inform SuppSvc if the USSD request is from
                   the card or from the user.
                   Added checks for returning only Return result
                   error codes for USSD additional info.
11/15/05   sk      Lint fixes
11/03/05   sk      Added check for ucs2 string in SendCommandResponse
                   Copied length field in case NMASK_SS_COMPLETE.
10/27/05   sk      Changed additional info value for USSD return error to reflect
                   error codes being sent by the network.
09/07/05   sk      Changed conditions for checking channels closed by catapp or by
                   the network.
08/26/05   sk      Added check for buffer size in open channel cmd
                   Added result check AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION
                   to SendCommandResponse().
08/24/05   sk      Added fix for data abort problem in function
                   CatApp_GetChannelStatus()
08/15/05   tml     Fixed Unicode Item display
08/10/05   sk      Modified coversion of IP address to IPV4 format
                   in CatApp_PopulateOpenChTable().
                   Added USC2 support for NMASK_SS_COMPLETE.
07/14/05   sk      Fixed array problems
07/13/05   sk      Fixes for close channel and get channel status
06/07/05   tml     Fixed potential data abort
05/11/05   sst     Lint fixes
05/06/05   sk      Send Channel Data Bug fixes.
                   Support for display of Second alpha identifier
                   for SET_UP_CALL Proactive command.
04/06/05   tml     Open Channel connection fixes
04/04/05   tml     Open Channel Bug fixes
03/30/05   sk      Added support for Unicode.
02/24/05   tml     Only malloc for WMS data structure if the event is
                   MESSAGE_SENT in the wmseventcb
01/24/05    jk     Fixed compiler warning.
01/12/05   tml     Used AEESTK in USSD dcs values
01/11/05   tml     Fixed send SMS screen display and idle mode text
01/10/05  sk/tml   Added USSD, get channel status and open channel GCF fixes
01/07/05   sun     compilation fixes
12/10/04   sun     Featurized ISUPPSVC
12/13/04   sk      Fixed the freeing of the pointer
                   pMe->m_pProactiveCommand.
                   Also introduced checks for active calls before
                   an open channel command.
12/08/04   sk      Added definiton for function CatApp_getChannelStatus().
                   Changed case AEESTK_GET_CH_STATUS_REQ in
                   SendCommandResponse() to collect status of all
                   'opened' and 'dropped' channels.
12/01/04   tml     Fixed Get Inkey Yes/No terminal response
12/02/04   tml     Fixed compilation fix
11/23/04  sk/tml   Added SAT/USAT data support
11/05/04   tml     Fixed case sensitivity issue for include file
10/27/04   sun     Freeup memory on error
10/22/04   tml     Allow DTMF display to be done by Dialer app
10/20/04   tml     Fixed potential abort issue in PRL
10/01/04   tml     Added support for help in setup menu and get input issue
                   fix
10/01/04   tml     Fixed Send DTMF display issue
09/24/04   tml     Added length to get input and get inkey terminal response
09/21/04   ss      Made ISTK changes based on QIS comments
09/08/04   tml     Removed ring tone alert in Setup call confirmation and
                   fixed get input terminal response issue
09/07/04   sw      Fixed catapp init failure issue
09/06/04   sw      Fixed compilation warning
09/05/04   gw      Fixed compilation issue with CDMA target
08/25/04   tml     fixed clearing or closing the catapp issue
08/25/04   tml     Fixed select item select not clear screen, icon terminal
                   response
08/29/04   tml     Merged in BREW 3.0 Input mode changes
08/18/04   tml     Fixed select item and main menu end key issue
08/17/04   tml     Reverted back to 30 second time out value
08/16/04   tml     Added fixes for 11.10 failures
08/04/04   tml     Change user no response timer to 5 seconds and add support
                   for softkey menu navigation for select item and setup menu
08/03/04   tml     Fixed default item, help info request issues
07/15/04   tml     Added provide local language support
07/06/04   tml     Handle alpha identifier padded with 0xFF display.  Fixed
                   Setup Menu GCF failure
06/24/04   tml     Fixed CGF cal control
06/18/04   ram     SS and USSD functionality ISuppSvc.
06/15/04   tml     Fixed compilation issue with BREW 3.0
06/14/04   tml     Allowed User CLR to go back to Setup Menu
06/10/04   tml     Added supports for Profile download version
05/26/04   tml     Added supports for language notification and send DTMF
04/23/04   tml     Fixed compilation warning
03/02/04   tml     Changes ERR to MSG_ERROR
02/24/04   tml     Fixed Send SMS packing required issue
02/06/04   wli     Featurization for CDMA only environment
01/19/04   tml     Added ICM functions back
01/19/04   tml     Fixed compilation problem, backed out ICM changes
01/16/03   ram     Replaced cm functionality by ICM.
12/23/03   tml     Fixed null pointer issue
12/16/03   BPW     Changes from UK field test
11/13/03   ram     Fixed ICM initialziation.
10/23/03   PA      Integrated with latest ICM
10/09/03   tml     Mode change command handling
09/30/03   tml     ICM initial support
09/15/03   tml     GCF fixes
09/03/03   tml     Fixes for memory issue and backward move in get input
08/06/03   AP      Use OEM_TM_DIALED_DIGITS for FEATURE_BREW.
07/18/03   tml     Changed Get Input dialog so that it has only text control
07/08/03   tml     Fixed get input error
07/01/03   tml     Changed TM_NUMBERS to DIALLED_DIGITS
06/24/03   tml     Added null pointer check for display text
06/04/03   tml     Fixed null text item display and setup call ringer tone fix
06/03/03   AP      Don't de-reference pointer when posting MESSAGE_SENT.
06/03/03   AP      Added include of cmph.h and fixed memory leak for
                   sms_msg_event_info struct.
06/03/03   AP      Deleted "include" for oemcm.h.
05/20/03   AP      Fixed display text dialog after pressing a key for specific
                   immediate response displays.
04/28/03   tml     Fixed Select Item and Setup Menu lists
04/24/03   tml     increased number of select items and setup menu
04/23/03   AP      included customer.h
04/22/03   tml/AP  Added support for UTK; fixes for display text w/immediate
                   response and added EVT_CAT_CM_CALL_EVENT_ERR.
03/19/03   tml     Get input and get inkey fix
03/14/03   tml     Update Call control and MO SMS control terminal profile
                   bits to 1
03/13/03   tml     Select Item and Setup menu memory issue fixed, play tone
                   enum match
03/06/03   tml     Update enum names
02/27/03   tml     Bug fixes for USSD
02/25/03   tml     Added Send SS support,
02/22/03   tml     EVT_SUSPEND and StartAppletArgs changes
                   Changed ERROR to ERROR_TONE to avoid conflict with atcop
02/19/03   tml     Send SMS bug fixes
02/14/03   tml     Changed call end API
02/11/03   tml     Memory and various bug fixes
02/05/03   tml     Changed cat app title text
02/04/03   AT      Added call support, redid indentation, made it pass lint.
12/16/02   aaa     Initial Version

===========================================================================*/


/*lint -e740 */
/* This lint error occurs because of how BREW objects are defined and   */
/* dereferenced. Lint says the cast of the ISTK * pointer to a          */
/* ISTKVtbl ** (within a macro) is sucpicious. For now, turn the error  */
/* off so lint can be run without errors.                               */

/*==========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
============================================================================ */

#include "intconv.h"
#include "customer.h"

#include "BREWVersion.h"
#ifdef FEATURE_APP_CATAPP
#include "comdef.h"
#include "AEEAppGen.h"        /* AEEApplet structure and Applet services */

#include "AppComFunc.h"
#include "catapp.bid"
#include "catapp_res.h"
#include "catapp.h"
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
  #include "OEMCFGI.h"
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK)
#include "AEEBatt.h"
#endif /* #if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK) */

#include "OEMFeatures.h"
#ifdef FEATURE_BREW_3_0
#include "OEMHeap.h"
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

#ifdef FEATURE_UIONE_HDK
#include "AEENativeNotifier.h"
#endif /* FEATURE_UIONE_HDK */

/* AEE Services used by app */
/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#define USER_TIMEOUT          30000     /* timeout value for user action */
#define DEFAULT_TONE_TIMEOUT  1000
#define CATAPP_PAUSE_TIME     3200      /* for DTMF */

/* for menu settings */
#define GET_INKEY_INPUT_SIZE 1
#define LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG 16
#define NUM_OF_MILLISECONDS_IN_MINUTE 60000
#define NUM_OF_MILLISECONDS_IN_SECOND 1000
#define NUM_OF_MILLISECONDS_IN_TENTH_SECOND 100
#define NUMBER_OF_MILLISECONDS_IN_ONE_MILLI_SECOND 1

#ifdef FEATURE_CAT_REL6
#define CATAPP_NUM_OF_TSEC_IN_SEC                  10
#define CATAPP_NUM_OF_SECONDS_IN_MINUTE            60
#endif
/* CATAPP_MAX_IP_ADDR4_LEN: includes 1 for null byte + 15 for max len for IP addr string */
#define CATAPP_MAX_IP_ADDR4_LEN                    16

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#ifdef FEATURE_SS_L3_DATA
#define CATAPP_MAX_BYTE_SIZE                       255
#endif /* #ifdef FEATURE_SS_L3_DATA */

byte catapp_default_lang[AEESTK_LANG_CODE_LEN] = {0x65, 0x6e};

/* Resource ID for Select Item list */
static const uint16 SelectItemDialogResID[] =
{    IDL_SELECT_ITEM_1,
     IDL_SELECT_ITEM_2,
     IDL_SELECT_ITEM_3,
     IDL_SELECT_ITEM_4,
     IDL_SELECT_ITEM_5,
     IDL_SELECT_ITEM_6,
     IDL_SELECT_ITEM_7,
     IDL_SELECT_ITEM_8,
     IDL_SELECT_ITEM_9,
     IDL_SELECT_ITEM_10,
     IDL_SELECT_ITEM_11,
     IDL_SELECT_ITEM_12,
     IDL_SELECT_ITEM_13,
     IDL_SELECT_ITEM_14,
     IDL_SELECT_ITEM_15,
     IDL_SELECT_ITEM_16,
     IDL_SELECT_ITEM_17,
     IDL_SELECT_ITEM_18,
     IDL_SELECT_ITEM_19,
     IDL_SELECT_ITEM_20,
     IDL_SELECT_ITEM_21,
     IDL_SELECT_ITEM_22,
     IDL_SELECT_ITEM_23,
     IDL_SELECT_ITEM_24,
     IDL_SELECT_ITEM_25,
     IDL_SELECT_ITEM_26,
     IDL_SELECT_ITEM_27,
     IDL_SELECT_ITEM_28,
     IDL_SELECT_ITEM_29,
     IDL_SELECT_ITEM_30
};

/* Resource ID for Select Item list */
static const uint16 SelectItemSKDialogResID[] =
{    IDL_SK_SELECT_ITEM_1,
     IDL_SK_SELECT_ITEM_2,
     IDL_SK_SELECT_ITEM_3,
     IDL_SK_SELECT_ITEM_4,
     IDL_SK_SELECT_ITEM_5,
     IDL_SK_SELECT_ITEM_6,
     IDL_SK_SELECT_ITEM_7,
     IDL_SK_SELECT_ITEM_8,
     IDL_SK_SELECT_ITEM_9,
     IDL_SK_SELECT_ITEM_10,
     IDL_SK_SELECT_ITEM_11,
     IDL_SK_SELECT_ITEM_12,
     IDL_SK_SELECT_ITEM_13,
     IDL_SK_SELECT_ITEM_14,
     IDL_SK_SELECT_ITEM_15,
     IDL_SK_SELECT_ITEM_16,
     IDL_SK_SELECT_ITEM_17,
     IDL_SK_SELECT_ITEM_18,
     IDL_SK_SELECT_ITEM_19,
     IDL_SK_SELECT_ITEM_20,
     IDL_SK_SELECT_ITEM_21,
     IDL_SK_SELECT_ITEM_22,
     IDL_SK_SELECT_ITEM_23,
     IDL_SK_SELECT_ITEM_24,
     IDL_SK_SELECT_ITEM_25,
     IDL_SK_SELECT_ITEM_26,
     IDL_SK_SELECT_ITEM_27,
     IDL_SK_SELECT_ITEM_28,
     IDL_SK_SELECT_ITEM_29,
     IDL_SK_SELECT_ITEM_30
};

/* Resource ID for Main Menu Item list */
static const uint16 MainMenuItemResID[] =
{    IDL_MENU_1,
     IDL_MENU_2,
     IDL_MENU_3,
     IDL_MENU_4,
     IDL_MENU_5,
     IDL_MENU_6,
     IDL_MENU_7,
     IDL_MENU_8,
     IDL_MENU_9,
     IDL_MENU_10,
     IDL_MENU_11,
     IDL_MENU_12,
     IDL_MENU_13,
     IDL_MENU_14,
     IDL_MENU_15,
     IDL_MENU_16,
     IDL_MENU_17,
     IDL_MENU_18,
     IDL_MENU_19,
     IDL_MENU_20,
     IDL_MENU_21,
     IDL_MENU_22,
     IDL_MENU_23,
     IDL_MENU_24,
     IDL_MENU_25,
     IDL_MENU_26,
     IDL_MENU_27,
     IDL_MENU_28,
     IDL_MENU_29,
     IDL_MENU_30
};

/* Resource ID for Main Menu Item list */
static const uint16 MainMenuSKItemResID[] =
{    IDL_SK_MENU_1,
     IDL_SK_MENU_2,
     IDL_SK_MENU_3,
     IDL_SK_MENU_4,
     IDL_SK_MENU_5,
     IDL_SK_MENU_6,
     IDL_SK_MENU_7,
     IDL_SK_MENU_8,
     IDL_SK_MENU_9,
     IDL_SK_MENU_10,
     IDL_SK_MENU_11,
     IDL_SK_MENU_12,
     IDL_SK_MENU_13,
     IDL_SK_MENU_14,
     IDL_SK_MENU_15,
     IDL_SK_MENU_16,
     IDL_SK_MENU_17,
     IDL_SK_MENU_18,
     IDL_SK_MENU_19,
     IDL_SK_MENU_20,
     IDL_SK_MENU_21,
     IDL_SK_MENU_22,
     IDL_SK_MENU_23,
     IDL_SK_MENU_24,
     IDL_SK_MENU_25,
     IDL_SK_MENU_26,
     IDL_SK_MENU_27,
     IDL_SK_MENU_28,
     IDL_SK_MENU_29,
     IDL_SK_MENU_30
};

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
int CCatApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,
                           void ** ppObj);
static boolean    CCatApp_HandleEvent(IApplet * pi, AEEEvent eCode,
                                      uint16 wParam, uint32 dwParam);
void CCatApp_FreeAppData(IApplet* po);

boolean DisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean SpcDisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean MorecharEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean MainMenuEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean GetInputEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean SelectItemEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean UserInputSKEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

void SendCommandResponse(CCatApp *pMe, AEESTKResult result,
                          AEESTKAdditionalInfo *additional_info);
void CatApp_SendDTMF(CCatApp *pMe);
void CatApp_EndNonMainDialog(CCatApp *pMe, boolean bCloseApp);
static int32 Catapp_RegisterForNetEvent(CCatApp *pMe,CatApp_CommandRespType **ppCommandResp,
                                        int index);
void catapp_wmseventcb(wms_msg_event_e_type event, const wms_msg_event_info_s_type *pInfo,
                              const boolean *shared);
void catapp_wms_callback (wms_cmd_id_e_type cmd, const void *user_data,
                          wms_cmd_err_e_type cmd_err);

static void CatApp_ReceiveDataRetryCb(void *p);
static void CatApp_SendDataRetryCb(void *p);
static void CatApp_CleanupSocketInfo(CCatApp *pMe, int index, boolean bAll);
static int CatApp_UDPStartConnect(CCatApp *pMe, int index);
static int CatApp_TCPStartConnect(CCatApp *pMe, int index);
static void CatApp_TCPConnected(void *p, int nErr);
static int32 CatApp_GetChannelStatus(const CCatApp *pMe,boolean value,
                              AEESTKGetChStatusResponse *chRespTable);
static int catapp_ConvertLocalAddrToIpv4Format(
  AEESTKPacketDataAddressType sLocalAddress,
  char                       *IPAddr);
#if (defined FEATURE_CAT_REL6 || !(defined FEATURE_CATAPP_OLD_SOUND_INTERFACE))
static int catapp_ConvertToMsec(
  AEESTKDuration minDurationOfDisplay,
  int32          *msec);
#endif /* FEATURE_CAT_REL6 || !(defined FEATURE_CATAPP_OLD_SOUND_INTERFACE) */
static int CatApp_BindSocket(CatApp_OpenChInfo openChInfo);
#ifdef FEATURE_CAT_REL6
#ifndef FEATURE_UIONE_HDK
static AEESTKBatteryStatusType CatApp_processChargerState(
  IBatt *po,AEEChargerStatus charger_state);
static AEESTKBatteryStatusType CatApp_processBatteryLevel(
  AEEBattLevel batt_level);
#endif /* #ifndef FEATURE_UIONE_HDK */
static void CatApp_SetHideSelectionSoftkeyLook(
  IShell *ps, IMenuCtl *pm);
#endif /* #ifdef FEATURE_CAT_REL6 */

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /*FEATURE_CATAPP_TEST_AUTOMATION */

boolean CreateCatAppDlg(CCatApp *pMe);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_UIONE_HDK
int CatApp_FetchURLResourceString(
    CCatApp *pMe,
    char    *pTmpStr,
    int      destSize,
    uint16   nResId
);
int CatApp_GetClassIdfromURL(CCatApp *pMe, char *pszURL, AEECLSID *pIdleAppClassId);
void CatApp_DeviceInfoChangeNotifCb(void *pData);
int CatApp_Send_URI_To_App(
  CCatApp                *pMe,
  AEESTKProactiveCmdType  cmdType,
  AECHAR                 *pWStr
);
int CatApp_AllocAndConvertWSTRTOSTR(AECHAR *pwStr, char**ppStr, int32 *pDestSize);
#endif /* FEATURE_UIONE_HDK */

/******************************************************************************/
/*********        Data and prototypes needed for Settings Menu           ******/
/******************************************************************************/

/* Dialog Event Handler function pointer structure */
typedef boolean (*PFNDLGHANDLER) (void *pUser, AEEEvent evt,
                                 uint16 w, uint32 dw );

static const PFNDLGHANDLER CatDlgEventHandlers[] =
{
   NULL,
   MainMenuEventHandler,      /* 1 IDD_MAIN_CAT */
   DisplayTextEventHandler,   /* 2 IDD_DISPLAY_TEXT */
   GetInputEventHandler,      /* 3 IDD_GET_INPUT */
   SelectItemEventHandler,    /* 4 IDD_SELECT_ITEM */
   MorecharEventHandler,      /* 5 IDD_GET_INPUT_ERROR */
   UserInputSKEventHandler,   /* 6 IDD_SKUSER_INPUT */
   MainMenuEventHandler,      /* 7 IDD_SK_MAIN_CAT */
   SelectItemEventHandler,    /* 8 IDD_SK_SELECT_ITEM */
   SpcDisplayTextEventHandler /* IDD_DISPLAY_CLOSE_UPON_EVENT */
};

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


/* Cat App can either be statically built into BREW or dynamically linked  */
/* during run-time.  If AEE_STATIC is defined, then Cat app will be a      */
/* static app.                                                             */
/* NOTE: Static apps can be built only by OEMs or Carriers. App developers */
/* can build dynamic apps only.                                            */
#if defined(AEE_STATIC)

int  CCatApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);

/* Constant Data... */
/*lint -e528 gaiCatApp not referenced */
static const AEEAppInfo    gaiCatApp = {AEECLSID_CATAPP,
                                        CATAPP_RES_FILE,
                                        IDS_TITLE_STRING,
                                        IDB_TITLE,
                                        IDB_TITLE,
                                        0,
                                        0,
                                        AFLAG_STATIC | AFLAG_HIDDEN};
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION: CatApp_PopulateCatAppCommandResponse

DESCRIPTION
  This function is used by catapp to populate the catapp command response pointer.

PROTOTYPE:
 void CatApp_PopulateCatAppCommandResponse(CatApp_CommandRespType **pResp,
                                           AEESTKResult Result,
                                           AEESTKAdditionalInfo ExtraInfo
                                           );
PARAMETERS:
   CatApp_CommandRespType **pResp  - CATAPP command response pointer
   AEESTKResult Result             - The result structure returned by AEE to CATAPP
   AEESTKAdditionalInfo ExtraInfo  - Additional informationn returned by AEE to CATAPP

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_PopulateCatAppCommandResponse(CatApp_CommandRespType **pResp, AEESTKResult Result, AEESTKAdditionalInfo ExtraInfo)
{
  if (pResp == NULL) {
    MSG_ERROR("Pass in null pResp for PopulateCatAppCommandresponse", 0, 0, 0);
    return;
  }
  *pResp = MALLOC(sizeof(CatApp_CommandRespType));
  if (*pResp == NULL) {
    MSG_ERROR("Unable to Alloc Memory", 0, 0, 0);
    return;
  }
  (*pResp)->AdditionalInfo.nLength = ExtraInfo.nLength;
  (void)MEMCPY((*pResp)->AdditionalInfo.additionalInfo, ExtraInfo.additionalInfo, ExtraInfo.nLength);
  (*pResp)->Result = Result;
}

/*===========================================================================
FUNCTION: CatApp_FindChSocketIndex

DESCRIPTION
  This function is used to find the index in the Open Channel Info table
  given the channel status

PROTOTYPE:
  int CatApp_FindChSocketIndex(CCatApp *pMe, int *index, CatApp_ChannelStatus Status);

PARAMETERS:
  CCatApp *pMe                  - pointer to the Open Channel Info Table
  int *index                    - address if the index that needs to be determined.
  CatApp_ChannelStatus Status   - Channel Status.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS - 0  in case the index could be found.
  EFAILED - 1  in case the index could not be found.

SIDE EFFECTS
  none
===========================================================================*/

int CatApp_FindChSocketIndex(CCatApp *pMe, int *index, CatApp_ChannelStatus Status)
{
  int i = 0;

  if (pMe == NULL)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }
  if (index == NULL)
  {
    MSG_ERROR("index ERR:NULL",0,0,0);
    return EFAILED;
  }

  for (i= 0; i< CATAPP_MAX_GPRS_CH; i++) {
    if (pMe->m_sOpenChInfo[i].status == Status) {
      *index = i;
      /* also assign the index in the pMe struct */
      pMe->m_sOpenChInfo[i].nIndex = i;
      return SUCCESS;
    }
  }
  MSG_HIGH("CatApp_FindChSocketIndex: Returning EFAILED",0,0,0);
  return EFAILED;
} /* CatApp_FindChSocketIndex */

/*===========================================================================
FUNCTION: CatApp_FindChSocketIndexFromChID

DESCRIPTION
  This function is used to find the index in the Open Channel Info table
  given the channel ID

PROTOTYPE:
 int CatApp_FindChSocketIndexFromChID(CCatApp *pMe, int *index, int ch_id);

PARAMETERS:
  CCatApp *pMe                  - pointer to the Open Channel Info Table
  int *index                    - address if the index that needs to be determined.
  int ch_id                     - Channel ID.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS - 0  in case the index could be found.
  EFAILED - 1  in case the index could not be found.

SIDE EFFECTS
  none
===========================================================================*/
int CatApp_FindChSocketIndexFromChID(CCatApp *pMe, int *index, int ch_id)
{
  int i = 0;
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }
  if(!index)
  {
    MSG_ERROR("index ERR:NULL",0,0,0);
    return EFAILED;
  }

  for (i= 0; i< CATAPP_MAX_GPRS_CH; i++) {
    if (pMe->m_sOpenChInfo[i].nChID == ch_id) {
      *index = i;
      /* also assign the index in the pMe struct */
      pMe->m_sOpenChInfo[i].nIndex = i;
      return SUCCESS;
    }
  }
  return EFAILED;
} /* CatApp_FindChSocketIndexFromChID */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
/*===========================================================================
FUNCTION: CatApp_IsOpenGPRSChReq

DESCRIPTION
  This function returns a TRUE if the proactive request that ISTK sends to the
  registered client is AEESTK_OPEN_CH_REQ and if the bearer type is GPRS/DEFAULT.

PROTOTYPE:
 static boolean CatApp_IsOpenGPRSChReq(CCatApp *pMe);

PARAMETERS:
  CCatApp *pMe -

DEPENDENCIES
  None.

RETURN VALUE
  static boolean
    FALSE   - 0
    TRUE    - 1

SIDE EFFECTS
  none
===========================================================================*/
static boolean CatApp_IsOpenGPRSChReq(const CCatApp *pMe)
{
  AEESTKOpenChReq *pOpenChReq;

  if (pMe == NULL) {
    return FALSE;
  }
  pOpenChReq = pMe->m_pProactiveCommand;

  if (pMe->cmdHdr.nCommandID != AEESTK_OPEN_CH_REQ) {
    return FALSE;
  }
  switch (pOpenChReq->sBearer.BearerType) {
  case AEESTK_BEARER_CSD:
    return FALSE;
  case AEESTK_BEARER_GPRS:
  case AEESTK_BEARER_DEFAULT:
    return TRUE;
  default:
    return FALSE;
  }
} /* CatApp_IsOpenGPRSChReq */

/*===========================================================================
FUNCTION: CatApp_IsMETransportRequire

DESCRIPTION
  This function returns a TRUE if the proactive request ISTK sends to the
  registered client is AEESTK_OPEN_CH_REQ and if the transport prtocol in
  the request is AEESTK_PKT_DATA_TCP.

PROTOTYPE
 static boolean CatApp_IsMETransportRequire(CCatApp *pMe);

PARAMETERS:
  CCatApp *pMe -

DEPENDENCIES
  None.

RETURN VALUE
  static boolean
    FALSE   - 0
    TRUE    - 1

SIDE EFFECTS
  none
===========================================================================*/
static boolean CatApp_IsMETransportRequire(const CCatApp *pMe)
{
  AEESTKOpenChReq *pOpenChReq;

  if (pMe == NULL) {
    return FALSE;
  }

  pOpenChReq = pMe->m_pProactiveCommand;
  if (pMe->cmdHdr.nCommandID != AEESTK_OPEN_CH_REQ) {
    return FALSE;
  }
  switch (pOpenChReq->sTransportLevel.TransportProtocol) {
  case AEESTK_PKT_DATA_UDP:
  case AEESTK_PKT_DATA_TCP:
    return TRUE;
  default:
    return FALSE;
  }
} /* CatApp_IsMETransportRequire */

/*===========================================================================
FUNCTION: CatApp_DetermineTransportType

DESCRIPTION
  This function determines the tranport protocol type TCP/UDP in
  the Open channel Proactive command request.


PROTOTYPE:
  static int CatApp_DetermineTransportType(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe -
  int index    - index

DEPENDENCIES
  None.

RETURN VALUE
  static int
    SUCCESS   - 0  if the transport protocol could be determined.
    EFAILED   - 1  if the transport protocol could not be determined.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_DetermineTransportType(CCatApp *pMe, int index)
{
  AEESTKOpenChReq *pOpenChReq;

  if (pMe == NULL) {
    return EFAILED;
  }
  pOpenChReq = pMe->m_pProactiveCommand;

  if(pOpenChReq == NULL)
  {
    return EFAILED;
  }

  if (pMe->cmdHdr.nCommandID != AEESTK_OPEN_CH_REQ) {
    return EFAILED;
  }
  switch (pOpenChReq->sTransportLevel.TransportProtocol) {
  case AEESTK_PKT_DATA_UDP:
    pMe->m_sOpenChInfo[index].bIsTCP = FALSE;
    break;
  case AEESTK_PKT_DATA_TCP:
    pMe->m_sOpenChInfo[index].bIsTCP = TRUE;
    break;
  default:
    return EFAILED;
  }
  pMe->m_sOpenChInfo[index].dwPortNum =
      HTONS(uint32touint16(pOpenChReq->sTransportLevel.dwPortNumber));
  return SUCCESS;

}  /*CatApp_DetermineTransportType */

/*===========================================================================
FUNCTION: CatApp_PopulateOpenChTable

DESCRIPTION
  This function populates information into the Open channel Table.
  NOTE :- This function does not offer support for the 'CSD' bearer type.


PROTOTYPE:
  static int CatApp_PopulateOpenChTable(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe -
  int index    - index

DEPENDENCIES
  None.

RETURN VALUE
  static int
    SUCCESS   - 0  if the table could be populated with the 'Open Channel' information.
    EFAILED   - 1  if the table could not be populated with the 'Open Channel' information.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_PopulateOpenChTable(CCatApp *pMe, int index)
{
  AEESTKOpenChReq *pOpenChReq;
  char            *pIPAddr = NULL;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }
  pOpenChReq = pMe->m_pProactiveCommand;

  if (pOpenChReq == NULL) {
    return EFAILED;
  }
  if (pMe->cmdHdr.nCommandID != AEESTK_OPEN_CH_REQ) {
    return EFAILED;
  }

  pMe->m_sOpenChInfo[index].nChID = index+1;
  pMe->m_CurrChId = pMe->m_sOpenChInfo[index].nChID;
  pMe->m_sOpenChInfo[index].bOnDemandLinkEst = pOpenChReq->bOnDemandLinkEst;
  switch (pOpenChReq->sBearer.BearerType) {
  case AEESTK_BEARER_GPRS:
    pMe->m_sOpenChInfo[index].sGPRSBearer.bPresent = TRUE;
    (void)MEMCPY(&pMe->m_sOpenChInfo[index].sGPRSBearer.Bearer, &pOpenChReq->sBearer.BearerParams.BearerGPRS,
           sizeof(AEESTKBearerGPRSParamType));
    break;
  case AEESTK_BEARER_DEFAULT:
    pMe->m_sOpenChInfo[index].sGPRSBearer.bPresent = FALSE;
    (void)MEMSET(&pMe->m_sOpenChInfo[index].sGPRSBearer.Bearer, 0x00,
      sizeof(AEESTKBearerGPRSParamType));
    break;
  default:
    MSG_HIGH("In CatApp_PopulateOpenChTable: Bearer type 0x%x",
             pOpenChReq->sBearer.BearerType,0,0);
    return EFAILED;
  }

  /* Port number and transport protocol type info */
  if (CatApp_DetermineTransportType(pMe, index) != SUCCESS) {
    return EFAILED;
  }

  if (pOpenChReq->nBufferSize <= 0) {
    return EFAILED;
  }

  if(pOpenChReq->nBufferSize > CATAPP_GPRS_MAX_BUFFER_SIZE)
  {
    pMe->m_sOpenChInfo[index].nBufferSize = CATAPP_GPRS_MAX_BUFFER_SIZE;
    pMe->m_sOpenChInfo[index].bOpen_ch_param_modified = TRUE;
  }
  else
  {
    pMe->m_sOpenChInfo[index].nBufferSize = pOpenChReq->nBufferSize;
    pMe->m_sOpenChInfo[index].bOpen_ch_param_modified = FALSE;
  }

  /* Free data before malloc to ensure there is no "dangling" pointer */
  CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pRecvBuffer);

  /* allocate buffer size */
  pMe->m_sOpenChInfo[index].pRecvBuffer = MALLOC(int32touint32(pMe->m_sOpenChInfo[index].nBufferSize)*sizeof(uint8));

  if (pMe->m_sOpenChInfo[index].pRecvBuffer == NULL) {
    return EFAILED;
  }

  /* Convert Local IP to INADDR */
  if (pOpenChReq->sLocalAddress.nPktAddressLength > 0) {
    pIPAddr = MALLOC(CATAPP_MAX_IP_ADDR4_LEN * sizeof(uint8));
    if (pIPAddr == NULL) {
      MSG_ERROR("Local IP Addr Null Ptr", 0, 0, 0);
      return EFAILED;
    }
    /* Set last byte to null terminated */
    (void)MEMSET(pIPAddr, 0x00, CATAPP_MAX_IP_ADDR4_LEN * sizeof(uint8));

    if(pOpenChReq->sLocalAddress.PktAddressType == AEESTK_PKT_DATA_IPV4_ADDRESS)
    {
      if(catapp_ConvertLocalAddrToIpv4Format(pOpenChReq->sLocalAddress, pIPAddr) != SUCCESS)
      {
        MSG_ERROR("Local IP address conversion to IPV4 format failed", 0, 0, 0);
        FREE(pIPAddr);
        pIPAddr = NULL;
        return EFAILED;
      }
    }

    /* Convert to INAddr */
    if(!INET_ATON(pIPAddr, &pMe->m_sOpenChInfo[index].sIPAddress)) {
      MSG_ERROR("INet_ATON failed", 0, 0, 0);
      FREE(pIPAddr);
      pIPAddr = NULL;
      return EFAILED;
    }
  }
  CATAPP_FREEIF(pIPAddr);

  if (pOpenChReq->sAccessPointName.nLength > 0) {
    pMe->m_sOpenChInfo[index].sAPN = MALLOC(pOpenChReq->sAccessPointName.nLength * sizeof(AECHAR));
    if (pMe->m_sOpenChInfo[index].sAPN == NULL) {
      MSG_ERROR("APN Null pointer", 0, 0, 0);
      return EFAILED;
    }
    (void)MEMCPY(pMe->m_sOpenChInfo[index].sAPN,
           pOpenChReq->sAccessPointName.pwszText,
           sizeof(AECHAR)*pOpenChReq->sAccessPointName.nLength);
  }

  /* Convert data Dest IP to INADDR */
  if (pOpenChReq->sDataDestAddress.nPktAddressLength > 0) {
    pIPAddr = MALLOC(CATAPP_MAX_IP_ADDR4_LEN  * sizeof(uint8));
    if (pIPAddr == NULL) {
      MSG_ERROR("Data Dest Addr Null Ptr", 0, 0, 0);
      return EFAILED;
    }
    /* Set last byte to null terminated */
    (void)MEMSET(pIPAddr, 0x00, CATAPP_MAX_IP_ADDR4_LEN * sizeof(uint8));

    if(pOpenChReq->sDataDestAddress.PktAddressType == AEESTK_PKT_DATA_IPV4_ADDRESS)
    {
      if(catapp_ConvertLocalAddrToIpv4Format(pOpenChReq->sDataDestAddress, pIPAddr) != SUCCESS)
      {
        MSG_ERROR("Local IP address conversion to IPV4 format failed", 0, 0, 0);
        FREE(pIPAddr);
        pIPAddr = NULL;
        return EFAILED;
      }
    }

    /* Convert to INAddr */
    if(!INET_ATON(pIPAddr, &pMe->m_sOpenChInfo[index].sDataDestAddress)) {
      MSG_ERROR("INet_ATON failed", 0, 0, 0);
      FREE(pIPAddr);
      pIPAddr = NULL;
      return EFAILED;
    }
  }
  CATAPP_FREEIF(pIPAddr);

  /* For the first time after Open Channel , this flag will be TRUE always */
  pMe->m_sOpenChInfo[index].bWasEmpty = TRUE;
  MSG_HIGH("bWasEmpty Flg is set to TRUE ox%x",
                pMe->m_sOpenChInfo[index].bWasEmpty,0,0);
  pMe->m_sOpenChInfo[index].status = CATAPP_CH_OPEN_WAIT;
  return SUCCESS;
} /* CatApp_PopulateOpenChTable */

/*===========================================================================
FUNCTION: CatApp_ReleaseNetMgrInst

DESCRIPTION
  This function is called to release the net mgr instance if this is not NULL.

PROTOTYPE:
  static void CatApp_ReleaseNetMgrInst(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe - pointer to CCatApp object
  int index    - index into open channel table.

DEPENDENCIES
  None.

RETURN VALUE
  static void

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_ReleaseNetMgrInst(CCatApp *pMe, int index)
{
  MSG_HIGH("In CatApp_ReleaseNetMgrInst index -- 0x%x",index,0,0);
  if(!pMe)
  {
    MSG_HIGH("pMe is NULL",0,0,0);
    return;
  }
  if (index >= CATAPP_MAX_GPRS_CH)
  {
    MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
    return;
  }
  if (pMe->m_sOpenChInfo[index].pINetMgr)
  {
    (void)INETMGR_Release(pMe->m_sOpenChInfo[index].pINetMgr);
    pMe->m_sOpenChInfo[index].pINetMgr = NULL;
  }
}

/*===========================================================================
FUNCTION: CatApp_ReleaseSocket

DESCRIPTION
  This function is called to release socket related to a particular channel id
  if this is not already NULL.

PROTOTYPE:
  static void CatApp_ReleaseSocket(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe - pointer to CCatApp object
  int index    - index into open channel table.

DEPENDENCIES
  None.

RETURN VALUE
  static void

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_ReleaseSocket(CCatApp *pMe, int index)
{
  if(!pMe)
  {
    MSG_HIGH("pMe is NULL",0,0,0);
    return;
  }
  if (index >= CATAPP_MAX_GPRS_CH)
  {
    MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
    return;
  }
  if (pMe->m_sOpenChInfo[index].pISocket != NULL)
  {
    (void)ISOCKET_Release(pMe->m_sOpenChInfo[index].pISocket);
    pMe->m_sOpenChInfo[index].pISocket = NULL;
  }
}

/*===========================================================================
FUNCTION: CatApp_CleanupSocketInfo

DESCRIPTION
  This function is called to release memory  corresponding to the socket being closed.
  NOTE :- If the socket is closed by the network, release only the data buffer
  but retain the channel Id and the channel status.


PROTOTYPE:
  static void CatApp_CleanupSocketInfo(CCatApp *pMe, int index, boolean bAll);

PARAMETERS:
  CCatApp *pMe -
  int index    -
  boolean bAll -

DEPENDENCIES
  None.

RETURN VALUE
  static void

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_CleanupSocketInfo(CCatApp *pMe, int index, boolean bAll)
{
  MSG_HIGH("In CatApp_CleanupSocketInfo",0,0,0);
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }
  pMe->m_sOpenChInfo[index].bIsTCP = FALSE;
  pMe->m_sOpenChInfo[index].sIPAddress = 0;
  if (bAll) {
    pMe->m_sOpenChInfo[index].status = CATAPP_CH_CLOSED;
  }
  else {
    pMe->m_sOpenChInfo[index].status = CATAPP_CH_DROPPED;
  }
  CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pRecvBuffer);
  pMe->m_sOpenChInfo[index].nCurrRecvDataSize = 0;
  pMe->m_sOpenChInfo[index].bMoreDataToRecv = FALSE;
  /* During cleanup, this flag is set to TRUE as the buffer is emptied */
  pMe->m_sOpenChInfo[index].bWasEmpty = TRUE;
  pMe->m_sOpenChInfo[index].nBufferSize = 0;
  pMe->m_sOpenChInfo[index].nByteDataSent = 0;
  pMe->m_sOpenChInfo[index].nIndex = 0;
  pMe->m_sOpenChInfo[index].nSendDataBufSize = 0;
  CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pSendBuffer);
  pMe->m_sOpenChInfo[index].bOnDemandLinkEst = FALSE;
  pMe->m_sOpenChInfo[index].dwPortNum = 0;
  pMe->m_sOpenChInfo[index].nBufferSize = 0;
  CATAPP_FREEIF(pMe->m_sOpenChInfo[index].sAPN);
  pMe->m_sOpenChInfo[index].sDataDestAddress = 0;
  pMe->m_sOpenChInfo[index].sGPRSBearer.bPresent = 0;
  (void)MEMSET(&pMe->m_sOpenChInfo[index].sGPRSBearer.Bearer, 0x00, sizeof(AEESTKBearerGPRSParamType));
  pMe->m_CurrChId = 0;
}

/*===========================================================================
FUNCTION: CatApp_CopyReceiveDataBuf

DESCRIPTION
  This function is used to copy the received data buffer.

PROTOTYPE:
  static int CatApp_CopyReceiveDataBuf(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe
  int index

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0 if the copying is successful
    EFAILED - 1 if the pointer to receive data req is a NULL pointer.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_CopyReceiveDataBuf(CCatApp *pMe, int index)
{
  AEESTKReceiveDataReq *pReceiveDataReq;
  uint8                *pNewRecvBuf = NULL;
  AEESTKResult         nResult = AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY;

  MSG_HIGH("In CatApp_CopyReceiveDataBuf",0,0,0);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }

  MSG_HIGH("nCurrRecvDataSize is 0x%x, bMoreDataToRecv Flg is 0x%x",
            pMe->m_sOpenChInfo[index].nCurrRecvDataSize,
            pMe->m_sOpenChInfo[index].bMoreDataToRecv,0);
  pReceiveDataReq = (AEESTKReceiveDataReq*)pMe->m_pProactiveCommand;

  CATAPP_FREEIF(pMe->m_RecvDataRsp.pData);

  (void)MEMSET(&pMe->m_RecvDataRsp, 0x00, sizeof(AEESTKReceiveDataResponse));

  if (pMe->m_sOpenChInfo[index].nCurrRecvDataSize >= pReceiveDataReq->nChDataLength)
  {
    /* More data or equal number of data is presented in the buffer
       Equal number of data is handled in this case so that the COMMAND_PERFORMED_SUCCESSFULLY
       instead of missing information is sent to the card */
    if (pReceiveDataReq->nChDataLength > CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE) {
      /* Check if the data requested is greater than 1 APDU can
         sent, if so, limit the data sent to the card to the max APDU len */
      pReceiveDataReq->nChDataLength = CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE;
    }
    pMe->m_RecvDataRsp.nLength = pReceiveDataReq->nChDataLength;
    pMe->m_RecvDataRsp.nChDataRemainingLen = pMe->m_sOpenChInfo[index].nCurrRecvDataSize
                                             - pMe->m_RecvDataRsp.nLength;
    /* Whenver the curr recv datasize is greater than what the card requests, this flg is set
       to FALSE as the recv buffer is not empty when another set of new data arrives
    */
    pMe->m_sOpenChInfo[index].bWasEmpty  = FALSE;

  }
  else
  { /* requested data size is greater than to data */
    if (pMe->m_sOpenChInfo[index].nCurrRecvDataSize > CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE)
    {
      /* Check if the data requested is greater than 1 APDU can
         sent, if so, limit the data sent to the card to the max APDU len */
      pMe->m_RecvDataRsp.nLength = CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE;
      /* Whenver the curr recv datasize is greater than the MAX APDU Size, this flg is set
          to FALSE as the recv buffer is not empty when another set of new data arrives
      */
      pMe->m_sOpenChInfo[index].bWasEmpty  = FALSE;
    }
    else
    {
      pMe->m_RecvDataRsp.nLength = pMe->m_sOpenChInfo[index].nCurrRecvDataSize;
      /* Whenver the curr recv datasize is lesser than the MAX APDU Size, this flg is set
       to TRUE as the recv buffer is empty when another set of new data arrives
      */
      pMe->m_sOpenChInfo[index].bWasEmpty  = TRUE;
    }
    pMe->m_RecvDataRsp.nChDataRemainingLen = pMe->m_sOpenChInfo[index].nCurrRecvDataSize
                                             - pMe->m_RecvDataRsp.nLength;
    if(pMe->m_RecvDataRsp.nChDataRemainingLen == 0) {
      nResult = AEESTK_RESULT_COMMAND_PERFORMED_WITH_MISSING_INFORMATION;
    }
  }
  if (pMe->m_RecvDataRsp.nLength > 0) {
    /* Free data before malloc to ensure there is no "dangling" pointer */
    CATAPP_FREEIF(pMe->m_RecvDataRsp.pData);
    pMe->m_RecvDataRsp.pData = MALLOC(int32touint32(pMe->m_RecvDataRsp.nLength));
    if (pMe->m_RecvDataRsp.pData == NULL) {
      return EFAILED;
    }
    /* copy data over */
    (void)MEMCPY(pMe->m_RecvDataRsp.pData, pMe->m_sOpenChInfo[index].pRecvBuffer, int32touint32(pMe->m_RecvDataRsp.nLength));
  }
  if (pMe->m_RecvDataRsp.nChDataRemainingLen > 0)
  { /* more data in the buffer, shift the data */
    pNewRecvBuf = MALLOC(int32touint32(pMe->m_RecvDataRsp.nChDataRemainingLen));
    if (pNewRecvBuf == NULL) {
      return EFAILED;
    }
    (void)MEMCPY(pNewRecvBuf, &pMe->m_sOpenChInfo[index].pRecvBuffer[pMe->m_RecvDataRsp.nLength],
      int32touint32(pMe->m_RecvDataRsp.nChDataRemainingLen));
    (void)MEMSET(pMe->m_sOpenChInfo[index].pRecvBuffer, 0x00, int32touint32(pMe->m_sOpenChInfo[index].nCurrRecvDataSize));
    (void)MEMCPY(pMe->m_sOpenChInfo[index].pRecvBuffer, pNewRecvBuf, int32touint32(pMe->m_RecvDataRsp.nChDataRemainingLen));
    FREE(pNewRecvBuf);
    pNewRecvBuf = NULL;
    /* Update the new recv data size */
    pMe->m_sOpenChInfo[index].nCurrRecvDataSize = pMe->m_RecvDataRsp.nChDataRemainingLen;
    MSG_HIGH("nCurrRecvDataSize is 0x%x",pMe->m_sOpenChInfo[index].nCurrRecvDataSize,0,0);
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_RECEIVE_DATA_SUCCESS,
                         int32touint16(index),
                         (uint32)(int32)nResult);
  }
  else {/* no more data, sent all to the card */
    (void)MEMSET(pMe->m_sOpenChInfo[index].pRecvBuffer, 0x00, int32touint32(pMe->m_sOpenChInfo[index].nCurrRecvDataSize));
    /* Update the new recv data size */
    pMe->m_sOpenChInfo[index].nCurrRecvDataSize = pMe->m_RecvDataRsp.nChDataRemainingLen;
    MSG_HIGH("nCurrRecvDataSize is 0x%x",pMe->m_sOpenChInfo[index].nCurrRecvDataSize,0,0);
    /* no more data from the previous data read */
    if (!(pMe->m_sOpenChInfo[index].bMoreDataToRecv))
    {
      /* no more data from the previous data read */
      pMe->m_sOpenChInfo[index].bWasEmpty = TRUE;
      MSG_HIGH("bWasEmpty Flg is set to TRUE ox%x",
                pMe->m_sOpenChInfo[index].bWasEmpty,0,0);
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_RECEIVE_DATA_SUCCESS,
                         int32touint16(index),
                         (uint32)(int32)nResult);
    }
  }
  /* Post an event to attempt more ISOCKET_Read
     1. The bMoreDataToRecv flg may have been set to TRUE
                             OR
     2. ReceiveDataRetryCb may have been invoked when the catapp Receive
        data buffer was not empty. So read socket once more to see if there
        are new bytes
  */
  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CATAPP_RECEIVE_DATA,
                           int32touint16(index),
                           (uint32)&pMe->m_sOpenChInfo[index]);

  return SUCCESS;

} /* CatApp_CopyReceiveDataBuf */

/*===========================================================================
FUNCTION: CatApp_ReceiveDataRetryCb

DESCRIPTION
  This function is a callback function for ReceiveData retry.

PROTOTYPE:
  static void CatApp_ReceiveDataRetryCb(void *p);

PARAMETERS:
  void *p -  p is the CatApp_OpenChInfo Pointer type

DEPENDENCIES
  None

RETURN VALUE
  static void

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_ReceiveDataRetryCb(void *p)
{
  /* p is the CatApp_OpenChInfo Pointer type */
  IShell *pIShell = AEE_GetShell();
  CatApp_OpenChInfo *pOpenChInfo = (CatApp_OpenChInfo*)p;
  if ((pIShell == NULL) || (p == NULL)) {
    MSG_ERROR("In CatApp_ReceiveDataRetryCb: pointer becomes null", 0, 0, 0);
    return;
  }
  if((pOpenChInfo->bWasEmpty == TRUE) && (pOpenChInfo->bMoreDataToRecv == FALSE))
  {
    MSG_HIGH("Posting event EVT_CATAPP_RECEIVE_DATA",0,0,0);
    (void)ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_RECEIVE_DATA, 0xFF,
                             (uint32)p);
  }
}

/*===========================================================================
FUNCTION: CatApp_UDPReceiveData

DESCRIPTION
  This function is used to get data from a UDP socket.

PROTOTYPE:
  static int CatApp_UDPReceiveData(CCatApp *pMe, int index, int *pnRead);

PARAMETERS:
  CCatApp *pMe -
  int index    -
  int *pnRead  -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0 if receive data is successful
    EFAILED - 1 receive data failed .

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_UDPReceiveData(const CCatApp *pMe, int index, int *pnRead)
{

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }

  if(!pnRead)
  {
    MSG_ERROR("pnRead ERR:NULL",0,0,0);
    return EFAILED;
  }

  if (pMe->m_sOpenChInfo[index].nCurrRecvDataSize >= pMe->m_sOpenChInfo[index].nBufferSize) {
    /* no more room to read */
    return EFAILED;
  }
  /* Save data to the Rx Buffer */
  *pnRead = ISOCKET_RecvFrom(pMe->m_sOpenChInfo[index].pISocket,
    (byte *)&pMe->m_sOpenChInfo[index].pRecvBuffer[pMe->m_sOpenChInfo[index].nCurrRecvDataSize],
    (uint16)(pMe->m_sOpenChInfo[index].nBufferSize-pMe->m_sOpenChInfo[index].nCurrRecvDataSize),0,0,0);

  return SUCCESS;
} /* CatApp_UDPReceiveData */

/*===========================================================================
FUNCTION: CatApp_TCPReceiveData

DESCRIPTION
  This function is used to get data from a TCP socket.

PROTOTYPE:
  static int CatApp_TCPReceiveData(CCatApp * pMe, int index, int *pnRead);

PARAMETERS:
  CCatApp *pMe
  int index
  int *pnRead

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0 if receive data is successful
    EFAILED - 1 receive data failed .

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_TCPReceiveData(const CCatApp *pMe, int index, int *pnRead)
{
  if (pMe->m_sOpenChInfo[index].nCurrRecvDataSize >= pMe->m_sOpenChInfo[index].nBufferSize) {
    /* no more room to read */
    return EFAILED;
  }
  if (pnRead == NULL) {
    return EFAILED;
  }

  *pnRead = ISOCKET_Read(pMe->m_sOpenChInfo[index].pISocket,
                      (byte *)&pMe->m_sOpenChInfo[index].pRecvBuffer[pMe->m_sOpenChInfo[index].nCurrRecvDataSize],
                      (uint16)(pMe->m_sOpenChInfo[index].nBufferSize-pMe->m_sOpenChInfo[index].nCurrRecvDataSize));

  return SUCCESS;
} /* CatApp_TCPReceiveData */

/*===========================================================================
FUNCTION: CatApp_ReceiveData

DESCRIPTION
  This function is used to get data from a socket.This function calls either
  CatApp_TCPReceiveData or CatApp_UDPReceiveData depending on the transport
  protocol.

PROTOTYPE:
  static int CatApp_ReceiveData(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe
  int index

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0 if receive data is successful
    EFAILED - 1 receive data failed .

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_ReceiveData(CCatApp *pMe, int index)
{
  int                nRead;
  int                nReturnStatus;
  AEESTKResult       nResult = AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY;

  MSG_HIGH("In CatApp_ReceiveData",0,0,0);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }

  if (pMe->m_sOpenChInfo[index].pISocket == NULL) {
    MSG_ERROR("Socket is NULL", 0, 0, 0);
    return EFAILED;
  }
  if (pMe->m_sOpenChInfo[index].status != CATAPP_CH_OPENED) {
    MSG_ERROR("Socket is not opened", 0, 0, 0);
    return EFAILED;
  }

  if (pMe->m_sOpenChInfo[index].bIsTCP) {
    nReturnStatus = CatApp_TCPReceiveData(pMe, index, &nRead);
  }
  else {
    nReturnStatus = CatApp_UDPReceiveData(pMe, index, &nRead);
  }

  if(nReturnStatus != SUCCESS) {
    MSG_ERROR("Receive Data Failed, 0x%x", nReturnStatus, 0, 0);
    return nReturnStatus;
  }

  if (nRead == AEE_NET_ERROR) {
    MSG_ERROR("Receive Failed: Error 0x%x",
              ISOCKET_GetLastError(pMe->m_sOpenChInfo[index].pISocket),
              0, 0);
    return EFAILED;
  }
  MSG_HIGH("bWasEmpty Flg is 0x%x,nCurrRecvDataSize is 0x%x,bMoreDataToRecv Flg is 0x%x",
            pMe->m_sOpenChInfo[index].bWasEmpty,
            pMe->m_sOpenChInfo[index].nCurrRecvDataSize,
            pMe->m_sOpenChInfo[index].bMoreDataToRecv);

  if (nRead == AEE_NET_WOULDBLOCK) {
    MSG_HIGH("nRead = AEE_NET_WOULDBLOCK ", 0, 0, 0);
    /* No more data to be received for this section either  */
    /* MoreDataToRecv is only updated either when the buff max out or when AEEWOULDBLOCK happens */
    pMe->m_sOpenChInfo[index].bMoreDataToRecv = FALSE;
    /* Send Data Available event */
    if ((pMe->m_sOpenChInfo[index].bWasEmpty) &&
        (pMe->m_sOpenChInfo[index].nCurrRecvDataSize > 0))
    {
      MSG_HIGH("Posting data available event...", 0, 0, 0);
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                      EVT_CATAPP_DATA_AVAILABLE, int32touint16(index),
                          0);
    }
    ISOCKET_Readable(pMe->m_sOpenChInfo[index].pISocket,
                     (PFNNOTIFY)CatApp_ReceiveDataRetryCb,
                     &pMe->m_sOpenChInfo[index]
                     );
    if (!pMe->term_resp_sent && (pMe->cmdHdr.nCommandID == AEESTK_RECEIVE_DATA_REQ))
    {
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_RECEIVE_DATA_SUCCESS,
                         int32touint16(index),
                         (uint32)(int32)nResult);
    }
    if(pMe->m_sOpenChInfo[index].nCurrRecvDataSize == 0)
    {
      pMe->m_sOpenChInfo[index].bWasEmpty = TRUE;
    }
    return SUCCESS;
  }

  if (nRead == (pMe->m_sOpenChInfo[index].nBufferSize-pMe->m_sOpenChInfo[index].nCurrRecvDataSize))
  {
    /* Max out on the data buffer */
    /* MoreDataToRecv is only updated either when the buff max out or when AEEWOULDBLOCK happens */
    MSG_HIGH("Received 0x%x bytes ",nRead,0,0);
    pMe->m_sOpenChInfo[index].bMoreDataToRecv = TRUE;
    MSG_HIGH("bMoreDataToRecv Flg is set to TRUE 0x%x",
              pMe->m_sOpenChInfo[index].bMoreDataToRecv,0,0);
    pMe->m_sOpenChInfo[index].nCurrRecvDataSize += nRead;
    if ((pMe->m_sOpenChInfo[index].bWasEmpty) &&
        (pMe->m_sOpenChInfo[index].nCurrRecvDataSize > 0))
    {
      MSG_HIGH("Posting data available event...", 0, 0, 0);
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_DATA_AVAILABLE, int32touint16(index),
                         0);
    }
  }
  else {
    /* did not max out buffer yet, do another read,
       i.e., stay in this loop until either
       1) buffer is full
       2) no more data  */
    MSG_HIGH("Received 0x%x bytes ",nRead,0,0);
    pMe->m_sOpenChInfo[index].nCurrRecvDataSize += nRead;
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_RECEIVE_DATA, int32touint16(index),
                         (uint32)&pMe->m_sOpenChInfo[index]);
  }

  MSG_HIGH("bWasEmpty Flg is 0x%x,nCurrRecvDataSize is 0x%x,bMoreDataToRecv Flg is 0x%x",
            pMe->m_sOpenChInfo[index].bWasEmpty,
            pMe->m_sOpenChInfo[index].nCurrRecvDataSize,
            pMe->m_sOpenChInfo[index].bMoreDataToRecv);

  return SUCCESS;
}

/*===========================================================================
FUNCTION: CatApp_SendDataRetryCb

DESCRIPTION
  This function is the callback function for SendDataRetry

PROTOTYPE:
  static void CatApp_SendDataRetryCb(void *p);

PARAMETERS:
  void *p - p is the CatApp_OpenChInfo Pointer type

DEPENDENCIES
  None

RETURN VALUE
  static void

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_SendDataRetryCb(void *p)
{
  /* p is the CatApp_OpenChInfo Pointer type */
  IShell *pIShell = AEE_GetShell();
  if ((pIShell == NULL) || (p == NULL)) {
    MSG_ERROR("In CatApp_ReceiveDataRetryCb : pointer becomes null", 0, 0, 0);
    return;
  }
  (void)ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                           EVT_CATAPP_SEND_DATA, 0xFF, (uint32)p);
}

/*===========================================================================
FUNCTION: CatApp_UDPSendData

DESCRIPTION
  This function is used to do a send data through a UDP socket.

PROTOTYPE:
  static int CatApp_UDPSendData(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe -
  int index    -

DEPENDENCIES
  None

RETURN VALUE
  static int - number of bytes sent.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_UDPSendData(const CCatApp *pMe, int index)
{
  int nSent = 0;
  nSent  = ISOCKET_SendTo(pMe->m_sOpenChInfo[index].pISocket,
                        (byte*)pMe->m_sOpenChInfo[index].pSendBuffer,
                        (uint16)pMe->m_sOpenChInfo[index].nSendDataBufSize,
                        0,
                        pMe->m_sOpenChInfo[index].sDataDestAddress,
                        pMe->m_sOpenChInfo[index].dwPortNum);
  MSG_HIGH("In CatApp_UDPSendData, sent 0x%x bytes",nSent,0,0);
  return nSent;
}

/*===========================================================================
FUNCTION: CatApp_TCPSendData

DESCRIPTION
  This function is used to do a send data through a TCP socket.

PROTOTYPE:
  static int CatApp_TCPSendData(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe -
  int index    -

DEPENDENCIES
  None

RETURN VALUE
  static int - number of bytes sent.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_TCPSendData(const CCatApp * pMe, int index)
{
  int                nSent = 0;
  nSent = ISOCKET_Write(pMe->m_sOpenChInfo[index].pISocket,
                        pMe->m_sOpenChInfo[index].pSendBuffer,
                        (uint16)pMe->m_sOpenChInfo[index].nSendDataBufSize);
  MSG_HIGH("In CatApp_TCPSendData, sent 0x%x bytes",nSent,0,0);
  return nSent;
} /* CatApp_TCPSendData */

/*===========================================================================
FUNCTION: CatApp_SendData

DESCRIPTION
  This function is used to do a send data through a socket.This function calls
  either CatApp_TCPSendData or CatApp_UDPSendData depending on the transport
  protocol used.

PROTOTYPE:
  static int CatApp_TCPSendData(CCatApp *pMe, int index);

PARAMETERS:
  CCatApp *pMe -
  int index    -

DEPENDENCIES
  None

RETURN VALUE
  static int - number of bytes sent.

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_SendData(CCatApp *pMe, int index)
{

  int nSent = 0;
  uint8 *pNewSendBuf = NULL;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }

  if (pMe->m_sOpenChInfo[index].pISocket == NULL) {
    MSG_ERROR("Socket is NULL", 0, 0, 0);
    return EFAILED;
  }
  if (pMe->m_sOpenChInfo[index].status != CATAPP_CH_OPENED) {
    MSG_ERROR("Socket is not opened", 0, 0, 0);
    return EFAILED;
  }

  if (pMe->m_sOpenChInfo[index].bIsTCP) {
    nSent = CatApp_TCPSendData(pMe, index);
  }
  else {
    nSent = CatApp_UDPSendData(pMe, index);
  }

  /* Can't send data at this moment, register callback so that we can retry
     in the later time */
  if (nSent == AEE_NET_WOULDBLOCK) {
    MSG_HIGH("AEE_NET_WOULDBLOCK trying to send again..", 0, 0, 0);
    ISOCKET_Writeable(pMe->m_sOpenChInfo[index].pISocket,
      (PFNNOTIFY)CatApp_SendDataRetryCb, &pMe->m_sOpenChInfo[index]);
    return SUCCESS;
  }
  if (nSent == AEE_NET_ERROR) {
    MSG_ERROR("Send Failed: Error 0x%x", ISOCKET_GetLastError(pMe->m_sOpenChInfo[index].pISocket), 0, 0);
    return EFAILED;
  }
  /* Sent something */
  if (nSent >= pMe->m_sOpenChInfo[index].nSendDataBufSize) {/* sent more and same amount of data
                                                                as located in the buffer */
    /* Done sending */
    pMe->m_sOpenChInfo[index].nSendDataBufSize = 0;
    CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pSendBuffer);
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_SEND_DATA_SUCCESS, int32touint16(index), 0);
  }
  else {
    /* sent only part of the data */
    pNewSendBuf = MALLOC(int32touint32(pMe->m_sOpenChInfo[index].nSendDataBufSize - nSent));
    if (pNewSendBuf == NULL) {
      /* Send data failed */
      MSG_HIGH("pNewSendBuf is NULL",0,0,0);
      return EFAILED;
    }
    (void)MEMCPY(pNewSendBuf, &pMe->m_sOpenChInfo[index].pSendBuffer[nSent],
           int32touint32(pMe->m_sOpenChInfo[index].nSendDataBufSize - nSent));
    pMe->m_sOpenChInfo[index].nSendDataBufSize -= nSent;
    CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pSendBuffer);
    pMe->m_sOpenChInfo[index].pSendBuffer = pNewSendBuf;
    /* Require to send everything now */
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_SEND_DATA, int32touint16(index),
                         (uint32)&pMe->m_sOpenChInfo[index]);
  }

  MSG_HIGH("sending complete...", 0, 0, 0);
  return SUCCESS;
}

/*===========================================================================
FUNCTION: CatApp_UDPConnected

DESCRIPTION

PROTOTYPE:
  static int CatApp_UDPConnected(void *p) ;

PARAMETERS:
  CCatApp *pMe -
  int index    -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_UDPConnected(void *p)
{
  int nErr = 0;
  IShell *pIShell = AEE_GetShell();
  CatApp_OpenChInfo *pOpenChInfo = (CatApp_OpenChInfo*)p;
  if ((pIShell == NULL) || (pOpenChInfo == NULL)) {
    // BAD!!!!!!
    MSG_ERROR("pointer becomes null", 0, 0, 0);
    return;
  }
  nErr = ISOCKET_Realize(pOpenChInfo->pISocket);
  switch (nErr) {
    case AEE_NET_WOULDBLOCK:   /* not ready */
      MSG_HIGH("CatApp_UDPConnected :AEE_NET_WOULDBLOCK ",0,0,0);
      ISOCKET_Readable(pOpenChInfo->pISocket,
                       (PFNNOTIFY)CatApp_UDPConnected,
                       pOpenChInfo);
      break;
    case AEE_NET_SUCCESS:      /* network is up */
      MSG_HIGH("CatApp_UDPConnected :AEE_NET_SUCCESS ",0,0,0);
      pOpenChInfo->status = CATAPP_CH_OPENED;
      (void)ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(pOpenChInfo->nIndex), 0);
      break;
    case AEE_NET_ERROR:
      MSG_HIGH("CatApp_UDPConnected :AEE_NET_ERROR ",0,0,0);
      pOpenChInfo->status = CATAPP_CH_OPENED;
      (void)ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pOpenChInfo->nIndex), 0);
      break; // handle error here
    default: break;  // some ISOCKET_* return something else for SUCCESS
  }
}

/*===========================================================================
FUNCTION: CatApp_UDPStartConnect

DESCRIPTION

PROTOTYPE:
  static int CatApp_UDPStartConnect(CCatApp *pMe, int index) ;

PARAMETERS:
  CCatApp *pMe -
  int index    -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_UDPStartConnect(CCatApp *pMe, int index)
{
  int nErr;
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }

  nErr = ISOCKET_Realize(pMe->m_sOpenChInfo[index].pISocket);

  switch (nErr)
  {
  case AEE_NET_SUCCESS:
    MSG_HIGH("connecting, 0x%x...\n", index, 0, 0);

    pMe->m_sOpenChInfo[index].status = CATAPP_CH_OPENED;
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(index), 0);
    break;
  case AEE_NET_WOULDBLOCK:
    MSG_HIGH("WOULDBLOCK: err: 0x%x, index 0x%x",
        ISOCKET_GetLastError(pMe->m_sOpenChInfo[index].pISocket), index, 0);
    ISOCKET_Readable(pMe->m_sOpenChInfo[index].pISocket,
                     (PFNNOTIFY)CatApp_UDPConnected,
                     &pMe->m_sOpenChInfo[index]);
    break;
  default:
    MSG_ERROR("Connect Failed: Error 0x%x, index 0x%x",
          ISOCKET_GetLastError(pMe->m_sOpenChInfo[index].pISocket), index, 0);
    /* Terminal response is sent because of the return EFAILED value */
    return EFAILED;
  }
  return SUCCESS;
} /* CatApp_UDPStartConnect */

/*===========================================================================
FUNCTION: CatApp_TCPConnected

DESCRIPTION

PROTOTYPE:
  static void CatApp_TCPConnected(void *p, int nErr) ;

PARAMETERS:
  void *p  -
  int nErr -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0
    EFAILED - 1

SIDE EFFECTS
  none
===========================================================================*/
static void CatApp_TCPConnected(void *p, int nErr)
{
  IShell *pIShell = AEE_GetShell();
  CatApp_OpenChInfo * pOpenInfo = (CatApp_OpenChInfo *)p;
  CatApp_CommandRespType *pCommandResp;
  AEESTKAdditionalInfo additional_info;

  MSG_HIGH("In CatApp_TCPConnected",0,0,0);

  if ((pIShell == NULL) || (pOpenInfo == NULL)) {
    // BAD!!!!!!
    MSG_ERROR("pointer becomes null", 0, 0, 0);
    return;
  }
  additional_info.nLength = 0;
  (void)MEMSET(additional_info.additionalInfo, 0, AEESTK_MAX_ADDITIONAL_INFO);

  if ((pOpenInfo->status != CATAPP_CH_OPEN_WAIT) && (pOpenInfo->status != CATAPP_CH_OPENED)) {
    MSG_ERROR("Can't find pending open wait ch", 0, 0, 0);
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_CH_ID_INVALID;
    CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                        additional_info);
    (void)ISHELL_PostEventEx( pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                        EVT_CATAPP_OPEN_CH_FAILED, 0xFF,
                        (uint32)pCommandResp);
  }

  if ((nErr == AEE_NET_SUCCESS) || (nErr == AEE_NET_EISCONN)){
    pOpenInfo->status = CATAPP_CH_OPENED;
    (void)ISHELL_PostEventEx(pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
      EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(pOpenInfo->nIndex), 0);
  }
  else {
    MSG_ERROR("Connect Failed: Error %d\n", nErr, 0, 0);
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_CH_CLOSED;
    CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                        additional_info);
    (void)ISHELL_PostEventEx( pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                        EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pOpenInfo->nIndex),
                        (uint32)pCommandResp);
  }
} /* CatApp_TCPConnected */

/*===========================================================================
FUNCTION: CatApp_TCPStartConnect

DESCRIPTION

PROTOTYPE:
  static int CatApp_TCPStartConnect(CCatApp *pMe, int index) ;

PARAMETERS:
  CCatApp *pMe   -
  int index      -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0
    EFAILED - 1

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_TCPStartConnect(CCatApp *pMe, int index)
{
  int nErr;

  nErr = ISOCKET_Connect(pMe->m_sOpenChInfo[index].pISocket,
                        pMe->m_sOpenChInfo[index].sDataDestAddress,
                        pMe->m_sOpenChInfo[index].dwPortNum,
                        CatApp_TCPConnected,
                        (void*)(&pMe->m_sOpenChInfo[index]));

  if (nErr != AEE_NET_SUCCESS) {
    MSG_ERROR("Connect Failed: Error 0x%x", ISOCKET_GetLastError(pMe->m_sOpenChInfo[index].pISocket), 0, 0);
    return EFAILED;
  } else {
    MSG_ERROR("CatApp_TCPStartConnect : connecting...\n", 0, 0, 0);
    return SUCCESS;
  }
} /* CatApp_TCPStartConnect */


/*===========================================================================
FUNCTION: CatApp_OpenSocket

DESCRIPTION
  This function is used to open a socket depending on whether the transport protocol
  is a TCP or a UDP socket.

PROTOTYPE:
  static int CatApp_OpenSocket(CCatApp *pMe, int *ch_id);

PARAMETERS:
  CCatApp *pMe -
  int *ch_id   -

DEPENDENCIES
  None

RETURN VALUE
  static int
    SUCCESS - 0
    EFAILED - 1

SIDE EFFECTS
  none
===========================================================================*/
static int CatApp_OpenSocket(CCatApp *pMe, int index, const int *ch_id)
{
  NetSocket  sSocketType;
  int        nErr;
  int        i;
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return EFAILED;
  }
  /* This section needs clean up. Index passed to this function is always valid whether it is
     an Open Channel or a Send Data Req. There is no need to find index using channel id or an empty
     index if there is no ch id (an in Open Ch Req).
     For Open Ch Req, the empty ch id is found in Evt Open Ch. For Send Data Req, the index
     is found in AEESTK_SEND_DATA_REQ using ch id sent by the card
  */
  if (ch_id == NULL) {
    /* find next avaliable channel */
    nErr = CatApp_FindChSocketIndex(pMe, &i, CATAPP_CH_CLOSED);
    if (nErr != SUCCESS)
    {
      MSG_ERROR("Can't find empty ch", 0, 0, 0);
      return EFAILED;
    }
  }
  else {
    nErr = CatApp_FindChSocketIndexFromChID(pMe, &i, *ch_id);
    if (nErr != SUCCESS) {
      MSG_ERROR("Can't find matching ch id", 0, 0, 0);
      return EFAILED;
    }
  }

  if (pMe->m_sOpenChInfo[i].bIsTCP) {
    MSG_HIGH("Channel %d has a TCP socket", i, 0, 0);
    sSocketType =  AEE_SOCK_STREAM;
  }
  else {
    MSG_HIGH("Channel %d has a UDP socket", i, 0, 0);
    sSocketType = AEE_SOCK_DGRAM;
  }
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

  pMe->m_sOpenChInfo[i].pISocket = INETMGR_OpenSocket(pMe->m_sOpenChInfo[index].pINetMgr, sSocketType);

  if (pMe->m_sOpenChInfo[i].pISocket == NULL) {
    MSG_ERROR("OpenSocket at index -0x%x Failed", i, 0, 0);
    return EFAILED;
  }

  pMe->m_sOpenChInfo[i].status = CATAPP_CH_OPEN_WAIT;

  if (pMe->m_sOpenChInfo[i].sIPAddress == 0) {
    /* Card did not provide IP address, use default */
    pMe->m_sOpenChInfo[i].sIPAddress = INETMGR_GetMyIPAddr(pMe->m_sOpenChInfo[index].pINetMgr);
  }

  if (pMe->m_sOpenChInfo[i].bIsTCP) {
    return CatApp_TCPStartConnect(pMe, i);
  }
  else {
    return CatApp_UDPStartConnect(pMe, i);
  }
  /* Success event is being sent in CatApp_UDPStartConnect or CatApp_TCPConnected */
  /* Failure event is required to be called from the calling function due to the async
     property in TCP */
}

/*===========================================================================

FUNCTION: catapp_hold_all_calls

DESCRIPTION
  This function is a placeholder in case future implementations do not
  automatically put all calls on hold.  Right now, this function simply
  returns TRUE, when the new call comes up, all others are on hold.

PROTOTYPE:
  boolean catapp_hold_all_calls(AEESTKAdditionalInfo  *additional_info)

PARAMETERS:
   additional_info: [in/out]: If we can't put calls on hold, this may hold
                              the reason.  For now, there is no reason since
                              we also say OK.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
boolean catapp_hold_all_calls(AEESTKAdditionalInfo  *additional_info)
{
  if(!additional_info)
  {
    MSG_ERROR("additional_info ERR:NULL",0,0,0);
    return FALSE;
  }
  additional_info->nLength = 0;
  return TRUE;
}

/*===========================================================================

FUNCTION: catapp_end_all_calls

DESCRIPTION
  This function is called before we originate the new call when we need to
  end all prior calls.  It runs through the list of in use call_ids and
  builds the list of call_ids to end.  It then calls cm_mm_call_cmd_end to
  release these calls.

PROTOTYPE:
  boolean catapp_end_all_calls(CCatApp *pMe,
                               AEESTKAdditionalInfo *additional_info)

PARAMETERS:
  pMe:             [in]:     Data structure containing our client_id.
  additional_info: [in/out]: If there is an error, this can say why.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
boolean catapp_end_all_calls(const CCatApp * pMe,
    const AEESTKAdditionalInfo  *additional_info)
{
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }
  if(!additional_info)
  {
    MSG_ERROR("additional_info ERR:NULL",0,0,0);
    return FALSE;
  }
  return CheckAEEReturnStatus(ICM_EndAllCalls(pMe->m_pICM));
} /*lint !e715 */


/*===========================================================================

FUNCTION: catapp_cm_call_cmd_orig

DESCRIPTION
  This function is called to originate the new call.  It takes the list of
  parameters given by GSTK and builds the structures used by CM.  It then
  calls cm_mm_call_cmd_orig and away the call goes.

PROTOTYPE:
  boolean catapp_cm_call_cmd_orig(CCatApp *pMe,
      AEESTKAdditionalInfo  *additional_info)

PARAMETERS:
  pMe:             [in]:     Data structure containing our client_id as
                             well as our command parameters.
  additional_info: [in/out]: If there is an error, this can say why.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
boolean catapp_cm_call_cmd_orig(CCatApp *pMe,
    AEESTKAdditionalInfo  *additional_info)
{
  int                  i = 0;            /* Loop variables */
  int                  j = 0;            /* Loop variables */
  boolean              start_dtmf = FALSE;
  //int                  dtmf_digit = 0;
  uint8               *digit_ptr = NULL;                           /* Flag to parse digits/DTMFs */
  uint8                num_buf[AEECM_MAX_DIAL_STRING+1];       /* Number buf going to CM.    */
  int                  digit_count = -1;                      /* Number of digits in num_buf */
  AEECMCallType        nCallType;                    /* Type of call (Voice/Emergency)*/
  AECHAR              *pCalledNumber = NULL;                      /* Called Number for ICM*/
  AECHAR              *pszSubAddress = NULL;
  AEECMServiceType srv_opt;                 /* Service option to origianate call*/
  AEESTKSetupCallReq  *pSetupCallReq;
  ICallOpts           *pICallOpts = NULL;
  CallOpt              call_option;
  boolean              bRetVal;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  if(!additional_info)
  {
    MSG_ERROR("additional_info ERR:NULL",0,0,0);
    return FALSE;
  }

  pSetupCallReq = (AEESTKSetupCallReq*) pMe->m_pProactiveCommand;

  pMe->m_PendingCallSetUp = FALSE;

  /* initialize info */
  additional_info->nLength = 0;             /* No additional info for this */
  (void) MEMSET( num_buf, 0, sizeof(num_buf));    /* No dialed number yet. */
  (void) MEMSET( pMe->dtmf_digits, 0, sizeof(pMe->dtmf_digits));    /* No dialed number yet. */

  /* Are they calling internationally?  If so, tell CM. */
  if (pSetupCallReq->address.TON == AEECM_TON_INTERNATIONAL)
    num_buf[j++] = '+';


  /* Run through the list of digits, build the number. */
  for (i = 0, digit_ptr = &num_buf[j];
       (j < AEECM_MAX_DIAL_STRING) &&
       (i < pSetupCallReq->address.nLength);
       i++)
  {
    switch (pSetupCallReq->address.address[i])
    {
      /* Digits are treated as ASCII, */
      case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
      case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
        *digit_ptr++ = '0' + pSetupCallReq->address.address[i];
        if (start_dtmf) {/* dtmf */
          //dtmf_digit++;
        }
        else {
          j++;
        }
        break;

      /* 0x0a is the * key */
      case 0x0A:
        *digit_ptr++ = '*';
        if (start_dtmf) {  /* dtmf */
          //dtmf_digit++;
        }
        else {
          j++;
        }
        break;
      /* 0x0b is the # key */
      case 0x0B:
        *digit_ptr++ = '#';
        if (start_dtmf) {  /* dtmf */
          //dtmf_digit++;
        }
        else {
          j++;
        }
        break;
      /* 0x0c marks the end of the dialed digits,
       * additional digits are DTMFs, switch the
       * digit_ptr to point into the dtmf_digits
       * array and remember how many in num_buf
       * Second or subsequent 0x0c in DTMFs meaning a pause
       * */
      case 0x0C:
        if (!start_dtmf) {
          /* save digit count */
         digit_count = j;
         start_dtmf = TRUE;
         digit_ptr = &pMe->dtmf_digits[0];
        }
        else {
          /* copy "pause" */
          *digit_ptr++ = 0xFF;
        }
        break;
      /* The rest are really errors. */
      case 0x0D:
      case 0x0E:
      case 0x0F:
      default:
        MSG_ERROR("Unexpected value %d in dial string at offset %d.",
            pSetupCallReq->address.address[i], i, 0);
        break;
    } /* switch (pMe->m_pProactiveCommand->cmd... */
  }
  if (digit_count < 0) digit_count = j;


  pCalledNumber = (AECHAR*) MALLOC(sizeof(AECHAR)*int32touint32(digit_count+1));

  if(pCalledNumber == NULL)
  {
    MSG_ERROR("CatApp:Malloc Failed", 0, 0, 0);
    return FALSE;
  }
  (void) STRTOWSTR((char*) num_buf, pCalledNumber, (int) sizeof(AECHAR)*(digit_count+1));

   nCallType = AEECM_CALL_TYPE_VOICE;


  /* Need a place to store the capability config. */
#if 0
  if (pSetupCallReq->capabilityConfig.nLength > 0)
  {
    /* Store the CCP into the appropriate CM feature. */
    pSetupCallReq->capabilityConfig.ccp;
  }
#endif

  if (ISHELL_CreateInstance (pMe->a.m_pIShell, AEECLSID_CALLOPTS,(void **) &pICallOpts) != SUCCESS) {
    MSG_ERROR("Could not get instance of ICallOpts", 0, 0, 0);
    FREE(pCalledNumber);
    pCalledNumber = NULL;
    return FALSE;
  }

  /* Subaddress call options*/
  if (pSetupCallReq->destinationSubAddress.nLength > 0)
  {
    pszSubAddress = (AECHAR*) MALLOC((int)sizeof(AECHAR)*
      (pSetupCallReq->destinationSubAddress.nLength+1));
    if(pszSubAddress == NULL)
    {
      MSG_ERROR("CatApp:Malloc Failed", 0, 0, 0);
      FREE(pCalledNumber);
      pCalledNumber = NULL;
      (void)ICallOpts_Release (pICallOpts);
      pICallOpts = NULL;
      return FALSE;
    }
    (void) STRTOWSTR((char*) pSetupCallReq->destinationSubAddress.subaddress,
              pszSubAddress,
              (int)sizeof(AECHAR)*(pSetupCallReq->destinationSubAddress.nLength+1));

    call_option.nId = AEECM_CALLOPT_CALLED_SUBADDR;
    call_option.pVal = pszSubAddress ;

    if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
      MSG_ERROR("Could not set call options", 0, 0, 0);
      (void)ICallOpts_Release (pICallOpts);
      pICallOpts = NULL;
      FREE(pszSubAddress);
      pszSubAddress = NULL;
      FREE(pCalledNumber);
      pCalledNumber = NULL;
      return FALSE;
    }
    call_option.nId = AEECM_CALLOPT_SRV_OPT;
    srv_opt = AEECM_SRV_TYPE_AUTOMATIC;
    call_option.pVal = &srv_opt;

    if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
      MSG_ERROR("Could not set call options", 0, 0, 0);
      (void)ICallOpts_Release (pICallOpts);
      pICallOpts = NULL;
      FREE(pszSubAddress);
      pszSubAddress = NULL;
      FREE(pCalledNumber);
      pCalledNumber = NULL;
      return FALSE;
    }
  }
#if defined(FEATURE_GSTK_FDN_CC_SUPPORT) && defined(CM_API_CC_MOD_PARAM)
  MSG_HIGH(" Skipping FDN check ",0,0,0);
  bRetVal = CheckAEEReturnStatus(ICM_OriginateCall_SkipFDNChk(pMe->m_pICM,
                                    nCallType,
                                    pCalledNumber,
                                    pMe->call_cnf_str,
                                    pICallOpts,
                                    &pMe->cat_call_id
                                   ));
#else /* defined(FEATURE_GSTK_FDN_CC_SUPPORT) && defined(CM_API_CC_MOD_PARAM) */
  bRetVal = CheckAEEReturnStatus(ICM_OriginateCall(pMe->m_pICM,
                                    nCallType,
                                    pCalledNumber,
                                    pMe->call_cnf_str,
                                    pICallOpts,
                                    &pMe->cat_call_id
                                   ));
#endif /* defined(FEATURE_GSTK_FDN_CC_SUPPORT) && defined(CM_API_CC_MOD_PARAM) */
  (void)ICallOpts_Release (pICallOpts);
  pICallOpts = NULL;
  CATAPP_FREEIF(pszSubAddress);
  FREE(pCalledNumber);
  pCalledNumber = NULL;
  return bRetVal;

}


/*===========================================================================

FUNCTION: catapp_cm_call_cmd_end

DESCRIPTION
  This function is called when we need to end the current call.  It uses
  the call_id in the pMe variable to figure out which to end.

PROTOTYPE:
  boolean catapp_cm_call_cmd_end(CCatApp *pMe)

PARAMETERS:
  pMe:             [in]:     Data structure containing our client_id as
                             well as our command parameters.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
boolean catapp_cm_call_cmd_end(const CCatApp * pMe)
{
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }
  return CheckAEEReturnStatus(ICM_EndCall(pMe->m_pICM, pMe->cat_call_id));
}


/*===========================================================================

FUNCTION: catapp_access_conditions_satified

DESCRIPTION
  This function is called when we need to end the current call.  It determines
  based on the current state of the phone if we can make this call or must reject
  it.


PROTOTYPE:
  boolean catapp_access_conditions_satified(CCatApp pMe,
      gstk_call_setup_req_enum_type setup_requirement,
      AEESTKAdditionalInfo  *additional_info)

PARAMETERS:
  setup_requirement:   [in]:  The condition under which to make this call.
  additional_info: [in/out]:  If there is a reason, set it here.


DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
boolean catapp_access_conditions_satified(const CCatApp *pMe,
    AEESTKCallSetupReqType setup_requirement,
    AEESTKAdditionalInfo  *additional_info)
{
  boolean isCallPresentFlg = FALSE;
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }
  if(!additional_info)
  {
    MSG_ERROR("additional_info ERR:NULL",0,0,0);
    return FALSE;
  }

  additional_info->nLength = 0;

  switch (setup_requirement)
  {
    case AEESTK_NO_OTHER_CALLS:
      /* Ask CM if there are other calls, if not, then we can
       * do this requirement.  If there is a call, they asked
       * for us to reject their request.
       */
        isCallPresentFlg = AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM);
        if(isCallPresentFlg == TRUE)
        {
          additional_info->nLength = 1;
          additional_info->additionalInfo[0] = AEESTK_ME_CURRENTLY_BUSY_ON_CALL;
          return FALSE;
        }
        else
        {
            return TRUE;
        }
    case AEESTK_HOLD_ACTIVE_CALLS:
    case AEESTK_DISCONNECT_ACTIVE_CALLS:
      /* We can do these actions when asked. */
      return TRUE;

    default:
      MSG_ERROR("Unrecognized setup requirement %d", setup_requirement, 0, 0);
      return FALSE;
  }
}

/*===========================================================================

FUNCTION CatApp_ProcessDTMF

DESCRIPTION
    This function converts the DTMF string from BCD format, and start the
    sending DTMF process

PROTOTYPE:
  static void CatApp_ProcessDTMF(CCatApp *pMe)

PARAMETERS:
    pMe: CCatApp structure

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_ProcessDTMF(CCatApp *pMe)
{
  uint32                 nCallIDSize;
  byte                   byLower;
  byte                   byHigher;
  AEESTKAdditionalInfo   additional_info;
  byte                   *tempDtmfString = NULL;
  int                    i, j            = 0;
  int                    tempDtmfLen     = 0;
  AEESTKSendDTMFReq      *pSendDTMFReq;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }
  pSendDTMFReq = (AEESTKSendDTMFReq*) pMe->m_pProactiveCommand;

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  /* Get the number of call IDs */
  nCallIDSize = ICM_GetActiveCallIDs(pMe->m_pICM,
                       AEECM_CALL_TYPE_NONE,
                       AEECM_CALL_STATE_CONV,
                       NULL,
                       0);
  if (nCallIDSize == 0) {
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_NOT_IN_SPEECH_CALL;
    SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
    return;
  }

  /* bcd len * 2 + 1 for null terminated data*/
  tempDtmfLen = pSendDTMFReq->nLength * 2 + 1;
  tempDtmfString = MALLOC(int32touint32(tempDtmfLen));

  if (tempDtmfString == NULL)
  {
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
    return;
  }
  (void)MEMSET(tempDtmfString, 0x00, int32touint32(tempDtmfLen));

  /* Set the DTMF */
  for (i = 0; i< pSendDTMFReq->nLength; i++) {
    byLower = pSendDTMFReq->pDTMFString[i] & 0x0F;
    byHigher = (pSendDTMFReq->pDTMFString[i] & 0xF0) >> 4;
    switch (byLower) {
    case 0x0A:
      tempDtmfString[j++] = 0x2A;
      break;
    case 0x0B:
      tempDtmfString[j++] = 0x23;
      break;
    case 0x0C:
      tempDtmfString[j++] = 0xFF;
      break;
    case 0x0D:
    case 0x0E:
    case 0x0F:
      break;
    default:
      tempDtmfString[j++] = 0x30 + byLower;
      break;
    }
    switch (byHigher) {
    case 0x0A:
      tempDtmfString[j++] = 0x2A;
      break;
    case 0x0B:
      tempDtmfString[j++] = 0x23;
      break;
    case 0x0C:
      // TODO pause
      tempDtmfString[j++] = 0xFF;
      break;
    case 0x0D:
    case 0x0E:
    case 0x0F:
      break;
    default:
      tempDtmfString[j++] = 0x30 + byHigher;
      break;
    }
  }

  tempDtmfLen = uint32toint32(STRLEN((char*)tempDtmfString) + 1);
  if (tempDtmfLen <= 1) {
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
    OEM_Free(tempDtmfString);
    tempDtmfString = NULL;
    return;
  }

  /* Free data before malloc to ensure there is no "dangling" pointer */
  CATAPP_FREEIF(pMe->sDTMFDigit);
  pMe->sDTMFDigit = MALLOC(int32touint32(tempDtmfLen));
  if (pMe->sDTMFDigit == NULL) {
    additional_info.nLength = 1;
    additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
    OEM_Free(tempDtmfString);
    tempDtmfString = NULL;
    return;
  }
  (void)MEMSET(pMe->sDTMFDigit, 0x00, int32touint32(tempDtmfLen));

  pMe->byCurrDTMF = 0;
  pMe->byNumDTMFDigits = int32touint8(tempDtmfLen - 1);

  /* memcpy only the digit, the last byte is set to null terminated value in
     the memset already */
  (void)MEMCPY(pMe->sDTMFDigit, tempDtmfString, pMe->byNumDTMFDigits);

  OEM_Free(tempDtmfString);
  tempDtmfString = NULL;
  CatApp_SendDTMF(pMe);
} /* CatApp_ProcessDTMF */

/*===========================================================================

FUNCTION CatApp_SendDTMF

DESCRIPTION
    This function gets the active call IDs and send the DTMF to the parties

PROTOTYPE:
  static void CatApp_SendDTMF(CCatApp *pMe)

PARAMETERS:
    pMe: CCatApp structure

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_SendDTMF(CCatApp *pMe)
{
  AEECMCallID                         sCallID;
  uint32                              nCallIDSize;
  AEESTKAdditionalInfo                additional_info;
  AECHAR                              *tempDtmfString = NULL;
  int                                 i;
  int                                 nReturnStatus;

  MSG_HIGH("In CatApp_SendDTMF",0,0,0);

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }

  if (pMe->sDTMFDigit == NULL)
  {
    MSG_ERROR("pMe->sDTMFDigit ERR:NULL",0,0,0);
    return;
  }

  /* Get the number of call IDs */
  nCallIDSize = ICM_GetActiveCallIDs(pMe->m_pICM,
                       AEECM_CALL_TYPE_NONE,
                       AEECM_CALL_STATE_CONV,
                       NULL,
                       0);

  MSG_HIGH("nCallIDSize = 0x%x", nCallIDSize, 0, 0);

  if (nCallIDSize == 0)
  {
    if(pMe->cmdHdr.nCommandID == AEESTK_SEND_DTMF_REQ)
    {
      additional_info.nLength = 1;
      additional_info.additionalInfo[0] = AEESTK_NOT_IN_SPEECH_CALL;
      SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
    }
    OEM_Free(pMe->sDTMFDigit);
    pMe->sDTMFDigit = NULL;
    pMe->byNumDTMFDigits = 0;
    pMe->byCurrDTMF = 0;
    return;
  }

  /* Get the 1 call ID */
  (void)ICM_GetActiveCallIDs(pMe->m_pICM,
                       AEECM_CALL_TYPE_NONE,
                       AEECM_CALL_STATE_CONV,
                       &sCallID,
                       1);

  /* find any pause in the DTMF string? */
  for (i = pMe->byCurrDTMF; i< pMe->byNumDTMFDigits; i++)
  {
    MSG_HIGH("pMe->sDTMFDigit[%d] = 0x%x", i, pMe->sDTMFDigit[i], 0);
    if (pMe->sDTMFDigit[i] == 0xFF) {
      break;
    }
  }

  i -= pMe->byCurrDTMF;

  /* If i == 0 => there is a pause, do not allocate any new memory, and only continue with the pause timer */
  if ( i > 0) {
    tempDtmfString = (AECHAR*)MALLOC(sizeof(AECHAR)*int32touint32(i+1));

    if (tempDtmfString == NULL)
    {
      MSG_ERROR("No memory for DTMF String Allocation", 0, 0, 0);
      if(pMe->cmdHdr.nCommandID == AEESTK_SEND_DTMF_REQ)
      {
        additional_info.nLength = 1;
        additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
      }
      OEM_Free(pMe->sDTMFDigit);
      pMe->sDTMFDigit = NULL;
      pMe->byNumDTMFDigits = 0;
      pMe->byCurrDTMF = 0;
      return;
    }

    (void)STRTOWSTR((char*)(&pMe->sDTMFDigit[pMe->byCurrDTMF]), tempDtmfString, uint32toint32(sizeof(AECHAR) * int32touint32(i+1)));

    nReturnStatus = ICM_BurstDTMF(pMe->m_pICM, sCallID, AEECM_DTMF_ON_95,  AEECM_DTMF_OFF_60, tempDtmfString);
    if (nReturnStatus != SUCCESS)
    {
      if(pMe->cmdHdr.nCommandID == AEESTK_SEND_DTMF_REQ)
      {
        additional_info.nLength = 1;
        additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additional_info);
      }
      OEM_Free(pMe->sDTMFDigit);
      pMe->sDTMFDigit = NULL;
      pMe->byNumDTMFDigits = 0;
      pMe->byCurrDTMF = 0;
      OEM_Free(tempDtmfString);
      tempDtmfString = NULL;
      return;
    }
  }

  if (i != (pMe->byNumDTMFDigits - pMe->byCurrDTMF))
  {
    /* encounter a pause */
    MSG_HIGH("Encountered a pause while sending DTMF",0,0,0);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, CATAPP_PAUSE_TIME,
            (PFNNOTIFY) CatApp_SendDTMF, (uint32*) pMe);
    /* Update the currentDtmf location for the next one if there are more */
    pMe->byCurrDTMF += int32touint8(i+1);
  }
  else
  {
    /* finished the process, send terminal response */
    if(pMe->cmdHdr.nCommandID == AEESTK_SEND_DTMF_REQ)
    {
      if (pMe->m_icon_present)
      {
        SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                            &additional_info);
      }
      else
      {
        SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
      }
    }
    OEM_Free(pMe->sDTMFDigit);
    pMe->sDTMFDigit = NULL;
    pMe->byNumDTMFDigits = 0;
    pMe->byCurrDTMF = 0;
  }

  if (tempDtmfString != NULL)
  {
    OEM_Free(tempDtmfString);
    tempDtmfString = NULL;
  }
} /* CatApp_SendDTMF */

/*===========================================================================
FUNCTION CatApp_EndNonMainDialog

DESCRIPTION
    This function ends the dialog if the currently active dialog is
    not the set up menu dialog

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_EndNonMainDialog(CCatApp *pMe, boolean bCloseApp)
{
  IDialog *pIDialog = NULL;
  uint16   wDlgID;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIDialog);
    if ((wDlgID != IDD_MAIN_CAT) && (wDlgID != IDD_SK_MAIN_CAT))
    {
      /*End the dialog */
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      /*Get the next dialog */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIDialog != NULL)
      {
        wDlgID = IDIALOG_GetID(pIDialog);
        if ((wDlgID != IDD_MAIN_CAT) && (wDlgID != IDD_SK_MAIN_CAT))
        {
            /*Still not the CatApp dialog, exit catapp, there should be at
              most 2 levels in CatApp, the setup menu and the latest display */

          if (bCloseApp)
            (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);

        }
        else
        {
          if(pMe->setupmenuupdate)
          {
            /*End the dialog and re-create it for any updates*/
            (void)ISHELL_EndDialog(pMe->a.m_pIShell);
            (void)CreateCatAppDlg(pMe);
          }
        }
      }
      else {
        /* Not running in CatApp content */
        if (bCloseApp)
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      }
    }
  }
} /* CatApp_EndNonMainDialog */

/*=========================================================================== */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*=========================================================================== */
/*lint -esym(714,CCatApp_GetModInfo) not referenced */
/*lint -e714 CCatApp_Load not referenced */
PFNMODENTRY CCatApp_GetModInfo(
   /*lint -esym(715,ps) */
   /*lint -esym(715,ppClasses) */
   /*lint -esym(715,pwMinPriv) */
  /*lint -e528 gaiCatApp not referenced */
  /*lint -e613 gaiCatApp not referenced */
    const IShell * ps, const AEECLSID * const *ppClasses,
    AEEAppInfo ** pApps, uint16 * pnApps, const uint16 * pwMinPriv)
{
   *pApps = (AEEAppInfo *)&gaiCatApp;
   *pnApps = 1;
   return((PFNMODENTRY)CCatApp_Load);
}
/*lint +e528 gaiCatApp not referenced */
/*lint +e613 gaiCatApp not referenced */


/*=========================================================================== */
/*                                                                            */
/*=========================================================================== */
int CCatApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)), ps, pHelpers, pMod,
                           CCatApp_CreateInstance, NULL));
}

#endif  /*AEE_STATIC */
/*lint +e714 CCatApp_Load not referenced */



/*===========================================================================

FUNCTION: SendTimerExpired

DESCRIPTION
  This function is invoked when the ME is expecting a response from the user
  but the user has not entered any input.

PROTOTYPE:
 void SendTimerExpired(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.


DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sends a Terminal Response to GSTK indicating user inactivity
===========================================================================*/
void SendTimerExpired(CCatApp *pMe)
{
  AEESTKAdditionalInfo           additional_info;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  pMe->m_bDisplayingText = FALSE;

  if(!pMe->term_resp_sent)
  {
    SendCommandResponse(pMe, AEESTK_RESULT_NO_RESPONSE_FROM_USER, &additional_info);
    if(pMe->m_bSuspended == TRUE)
    {
      /* End Proactive Session flag is checked on app resume. If this flag is set,
         all non main catapp stale dialogs are closed.
      */
      pMe->m_EndProactiveSession = CATAPP_NON_CARD_END_PROACTIVE_SESSION;
    }
  }
}

void SendClearTimerExpired(CCatApp *pMe)
{
  AEESTKAdditionalInfo           additional_info;
  AEESTKDisplayTextReq          *pDisplayTextReq = NULL;

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }

  if(!pMe->term_resp_sent)
  {
    pMe->m_bDisplayingText = FALSE;
    pDisplayTextReq = (AEESTKDisplayTextReq*) pMe->m_pProactiveCommand;
    if (NULL == pDisplayTextReq)
    {
      MSG_ERROR("pDisplayTextReq is null!", 0, 0, 0);
      return;
    }

    if (pMe->cmdHdr.nCommandID == AEESTK_DISPLAY_TEXT_REQ &&
        pDisplayTextReq->bUserControl)
    {
      /* SIM expects user to clear msg but user activity timer expires */
      SendCommandResponse(
        pMe,
        AEESTK_RESULT_NO_RESPONSE_FROM_USER,
        &additional_info);
    }
    else
    {
      if (pMe->m_icon_present)
      {
        SendCommandResponse(
          pMe,
          AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
          &additional_info);
      }
      else
      {
        SendCommandResponse(
          pMe,
          AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY,
          &additional_info);
      }
    }

  }
  else
  {
    /* end the dialog since we get here when a terminal response has already
       been sent, i.e., there might not be another proactive command/end of
       proactive session from the SIM/USIM/RUIM */
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
  }
}

/*===========================================================================

FUNCTION: SendCommandResponse

DESCRIPTION
  This function is invoked when the ME is expecting a response from the user
  but the user has not entered anything.

PROTOTYPE:
 void SendTimerExpired(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains information specific
             to this applet. It was initialized during the AppCreateInstance() function.
  result: [in]: The result to be passed to GSTK.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sends a Terminal Response to GSTK passing in the result
===========================================================================*/
void SendCommandResponse(CCatApp *pMe, AEESTKResult result,
  AEESTKAdditionalInfo           *pAdditional_info )
{
  int                        nReturnStatus;
  uint8                      extra_param_length   = 0;
  void                      *pExtraParam = NULL;
  AEESTKGetInputResponse     getInputRsp;
  AEESTKGetChStatusResponse  ChStatus[CATAPP_MAX_GPRS_CH];
  AEESTKOpenChResponse       OpenCh;
  AEESTKInkeyResponse        InkeyResp;
  int32                      bByteRemaining       = 0;
  int                        index                = 0xFF;
  int32                      cnt                  = 0;
  AEESTKUSSDResponse         USSDRsp;

  if(!pMe)
  {
    MSG_ERROR("SendCommandResponse: pMe ERR:NULL",0,0,0);
    return;
  }

  MSG_HIGH("CatApp Sending Terminal Response :result - %d, command - %d",
           result, pMe->cmdHdr.nCommandID, 0);
  if((result == AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY)
    || (result == AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED)
    || (result == AEESTK_RESULT_COMMAND_PERFORMED_WITH_MISSING_INFORMATION)
    || (result == AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION)
#ifdef FEATURE_CAT_REL6
    || (result == AEESTK_RESULT_NO_RESPONSE_FROM_USER)
#endif /* FEATURE_CAT_REL6 */
 )
  {
    switch(pMe->cmdHdr.nCommandID)
    {
      case AEESTK_SEND_SS_REQ:
        if(pMe->m_pISuppSvcOpts)
        {
          (void)ISUPPSVCOpts_Release(pMe->m_pISuppSvcOpts);
          pMe->m_pISuppSvcOpts = NULL;
        }
        break;
      case AEESTK_SEND_USSD_REQ:
        (void)MEMSET(&USSDRsp, 0x00, sizeof(AEESTKUSSDResponse));
        USSDRsp.data       = pMe->m_USSDStringRsp;
        USSDRsp.nDCSFromNW = pMe->m_USSDDcs;
        pExtraParam        = &USSDRsp;
        if ((pMe->m_USSDStringRsp.nLength > 0) && (pMe->m_USSDStringRsp.pwszText != NULL))
        {
          if(pMe->m_USSDDcs == (uint8)AEESTK_UCS2_DCS)
          {
            extra_param_length
              = uint32touint8(int32touint32(sizeof(AEESTKUSSDResponse))) + uint32touint8(pMe->m_USSDStringRsp.nLength);
          }
          else
          {
            extra_param_length
              = uint32touint8(int32touint32(sizeof(AEESTKUSSDResponse))) +
                uint32touint8(int32touint32(WSTRLEN(pMe->m_USSDStringRsp.pwszText)));
          }
        }
        else {
          extra_param_length = (uint8)sizeof(AEESTKUSSDResponse);
        }
        if(pMe->m_pISuppSvcOpts)
        {
          (void)ISUPPSVCOpts_Release(pMe->m_pISuppSvcOpts);
          pMe->m_pISuppSvcOpts = NULL;
        }
        break;

      case AEESTK_GET_INKEY_REQ:
        {
          AEESTKGetInkeyReq *pGetInkeyReq = (AEESTKGetInkeyReq*) pMe->m_pProactiveCommand;
          if (pMe->m_pProactiveCommand == NULL)
          {
            MSG_ERROR("Proactive command buffer for AEESTK_GET_INKEY_REQ data is null", 0, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }
          InkeyResp.ResponseType = pMe->m_InkeyRspType;
          InkeyResp.pszText = pMe->userinput;
          /* Intialize TotalDisplayDuration object */
          (void)MEMSET(&InkeyResp.TotalDisplayDuration, 0x00, sizeof(AEESTKDuration));
#ifdef FEATURE_CAT_REL6
          if(pGetInkeyReq->variableTimeout.bPresent == TRUE)
          {
            AEESTKDuration endExecTime;

            endExecTime.nLength  = uint32touint8(GETUPTIMEMS() / NUM_OF_MILLISECONDS_IN_SECOND);
            endExecTime.nUnits   = AEESTK_DURATION_SECONDS;
            InkeyResp.TotalDisplayDuration.nLength =
              endExecTime.nLength - pMe->m_StartofExecTime.nLength;

            InkeyResp.TotalDisplayDuration.bPresent = TRUE;
            InkeyResp.TotalDisplayDuration.nUnits   = pGetInkeyReq->variableTimeout.nUnits;
            switch(pGetInkeyReq->variableTimeout.nUnits)
            {
              case AEESTK_DURATION_MINUTES:
                InkeyResp.TotalDisplayDuration.nLength = (1/CATAPP_NUM_OF_SECONDS_IN_MINUTE) *
                  InkeyResp.TotalDisplayDuration.nLength;
                break;
              case AEESTK_DURATION_SECONDS:
                break;
              case AEESTK_DURATION_TENTHS_OF_SECONDS:
                InkeyResp.TotalDisplayDuration.nLength = int32touint8(CATAPP_NUM_OF_TSEC_IN_SEC *
                  InkeyResp.TotalDisplayDuration.nLength);
                break;
              default:
                MSG_HIGH("Send Command Response: Unknown time unit from card",0,0,0);
                result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
                break;
            }
            if(result == AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND)
            {
              MSG_HIGH("Send Command Response: ME Unable to process command",0,0,0);
                break;
            }
          }
#endif /* #ifdef FEATURE_CAT_REL6 */

          pExtraParam = &InkeyResp;

          if(result != AEESTK_RESULT_NO_RESPONSE_FROM_USER)
          {
            if(pGetInkeyReq->responseFormat == AEESTK_YES_NO
#ifdef FEATURE_CAT_REL6
               ||
               pGetInkeyReq->responseFormat == AEESTK_YES_NO_AND_IMM_DGT_RSP
#endif /* #ifdef FEATURE_CAT_REL6 */
              )
            {
              extra_param_length = int32touint8(sizeof(AEESTKInkeyResponse));
            }
            else
            {
              extra_param_length = int32touint8(sizeof(AEESTKInkeyResponse)) +
                                   uint32touint8(int32touint32(WSTRLEN(pMe->userinput)));
            }
          }
          else
            extra_param_length = int32touint8(sizeof(AEESTKInkeyResponse));
        }
        break;

      case AEESTK_GET_INPUT_REQ:
        {
          if (result != AEESTK_RESULT_NO_RESPONSE_FROM_USER)
          {
            getInputRsp.pString.nLength  = int32touint32(WSTRLEN(pMe->userinput));
            getInputRsp.pString.pwszText = pMe->userinput;
            getInputRsp.dcs              = pMe->m_GetInputRspFormat;
            pExtraParam                  = &getInputRsp;
          }
        }
        break;

      case AEESTK_SELECT_ITEM_REQ:
        {
          if (result != AEESTK_RESULT_NO_RESPONSE_FROM_USER)
          {
            pExtraParam = &pMe->chosenitem;
            extra_param_length = int32touint8(sizeof(pMe->chosenitem));
          }
        }
        break;

      case AEESTK_PROVIDE_BATTERY_STATUS_REQ:
#if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK)
        pExtraParam = &pMe->m_BatteryStatusRsp;
        extra_param_length = int32touint8(sizeof(pMe->m_BatteryStatusRsp));
#else
        /* This is as good as the default case */
        MSG_HIGH("AEESTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported ",0,0,0);
#endif /* #ifdef (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK) */
        break;

      case AEESTK_PROVIDE_LANG_INFO_REQ:
        pExtraParam = pMe->m_lang;
        extra_param_length = int32touint8(sizeof(AEESTK_LANG_CODE_LEN));
        break;

      case AEESTK_OPEN_CH_REQ:
        {
          nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pMe->m_CurrChId);
          if (nReturnStatus != SUCCESS) {
            /* ch already occupied */
            /* no more room */
            MSG_ERROR("Can't find any matching ch 0x%x", pMe->m_CurrChId, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }

          OpenCh.nChID = pMe->m_sOpenChInfo[index].nChID;
          if (pMe->m_sOpenChInfo[index].status == CATAPP_CH_OPENED) {
            OpenCh.bIsLinkEst = TRUE;
          }
          else {
            OpenCh.bIsLinkEst = FALSE;
          }
          OpenCh.ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
          OpenCh.nBufferSize = pMe->m_sOpenChInfo[index].nBufferSize;
          OpenCh.BearerDescription.BearerType = AEESTK_BEARER_GPRS;
          if (pMe->m_sOpenChInfo[index].sGPRSBearer.bPresent) {
            (void)MEMCPY(&OpenCh.BearerDescription.BearerParams.BearerGPRS,
              &pMe->m_sOpenChInfo[index].sGPRSBearer.Bearer,
              sizeof(AEESTKBearerGPRSParamType));
          }

          pExtraParam = &OpenCh;
          extra_param_length = uint32touint8(sizeof(AEESTKOpenChResponse));
        }
        break;

      case AEESTK_SEND_DATA_REQ:
        {
          AEESTKSendDataReq *pSendDataReq = (AEESTKSendDataReq*) pMe->m_pProactiveCommand;
          if (pMe->m_pProactiveCommand == NULL)
          {
            MSG_ERROR("Proactive command buffer for send data is null", 0, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }
          nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pSendDataReq->nChID);
          if (nReturnStatus != SUCCESS) {
            /* ch already occupied */
            /* no more room */
            MSG_ERROR("Can't find any matching ch 0x%x", pSendDataReq->nChID, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }
          bByteRemaining = pMe->m_sOpenChInfo[index].nBufferSize - pMe->m_sOpenChInfo[index].nSendDataBufSize;
          /* Buffer size not needed any more -  so clear it */
          if(bByteRemaining <= 0)
          {
            pMe->m_sOpenChInfo[index].nSendDataBufSize = 0;
            bByteRemaining = 0xFF;
          }
          else if(bByteRemaining > 255)
          {
            bByteRemaining = 0xFF;
          }
          pExtraParam = &bByteRemaining;
          extra_param_length = int32touint8(sizeof(int32));
        }
        break;
      case AEESTK_RECEIVE_DATA_REQ:
        {
          AEESTKReceiveDataReq *pReceiveDataReq = (AEESTKReceiveDataReq*) pMe->m_pProactiveCommand;
          MSG_HIGH("In SendCommandResponse AEESTK_RECEIVE_DATA_REQ",0,0,0);
          if (pMe->m_pProactiveCommand == NULL)
          {
            MSG_ERROR("Proactive command buffer for receive data is null", 0, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }
          nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pReceiveDataReq->nChID);
          if (nReturnStatus != SUCCESS) {
            /* ch already occupied */
            /* no more room */
            MSG_ERROR("Can't find any matching ch 0x%x", pReceiveDataReq->nChID, 0, 0);
            result = AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            break;
          }
          pExtraParam = &pMe->m_RecvDataRsp;
          extra_param_length = uint32touint8(int32touint32(pMe->m_RecvDataRsp.nLength) + sizeof(AEESTKReceiveDataResponse));
        }
        break;
      case AEESTK_GET_CH_STATUS_REQ:
        {
          cnt = CatApp_GetChannelStatus(pMe,TRUE,ChStatus);
          result = AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY;
          pExtraParam = ChStatus;
          extra_param_length = uint32touint8(int32touint32(cnt) * sizeof(AEESTKGetChStatusResponse));
        }
        break;

      default:
        break;
    }
  } /* if result not successful  */
  else if(result == AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER)
  {
    if (pMe->cmdHdr.nCommandID == AEESTK_SELECT_ITEM_REQ)
    {
      pExtraParam = &pMe->chosenitem;
      extra_param_length = sizeof(pMe->chosenitem);
    }
  }

  /* For Open Ch command, require to add buffer size and bearer gprs even if
     it is a failure */
  if (pMe->cmdHdr.nCommandID == AEESTK_OPEN_CH_REQ)
  {
    /* a separate handling is required here since in the above
       succses handling, if an index is not found, the result will
       be set to fail */
    switch (result) {
    case AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY:
    case AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
    case AEESTK_RESULT_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
    case AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION:
      break;
    default:
      /* data that doesn't matter */
      OpenCh.nChID = 0x0;
      OpenCh.bIsLinkEst = FALSE;
      OpenCh.ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
      if (pMe->m_pProactiveCommand == NULL) {
        MSG_ERROR("pProactiveCommand ptr has been freed", 0, 0, 0);
      }
      else {
        AEESTKOpenChReq *pOpenChReq = (AEESTKOpenChReq*) pMe->m_pProactiveCommand;
        /* populating buffer size and bearer description */
        OpenCh.nBufferSize = pOpenChReq->nBufferSize;
        (void)MEMCPY(&OpenCh.BearerDescription.BearerType, &pOpenChReq->sBearer,
               sizeof(AEESTKBearerDescriptionType));
        pExtraParam = &OpenCh;
        extra_param_length = sizeof(AEESTKOpenChResponse);
      }
      break;
    }
  }

  /* send the terminal response */
  MSG_LOW("Sending Terminal Response %u ",result,0,0);
  if(pMe->cmdHdr.nCommandID != AEESTK_GET_INPUT_REQ)
  {
    nReturnStatus = ISTK_SendCommandResponse(pMe->m_pISTK,
          pMe->cmdHdr,
          result,
          pAdditional_info,
          pExtraParam,
          extra_param_length);
  }
  else
  {
    /* Enhanced to support the new response structure related to Get Input */
    nReturnStatus = ISTK_SendCommandResponseEx(pMe->m_pISTK,
          pMe->cmdHdr,
          result,
          pAdditional_info,
          pExtraParam);
  }

  if (nReturnStatus == SUCCESS)
  {
    MSG_LOW("Terminal Response sent successfully",0,0,0);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  }
  else
  {
    MSG_ERROR("Catapp: Terminal Response sending error 0x%x",nReturnStatus,0,0);
  }

  /* cancel the timer for user timeout */
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendTimerExpired, pMe);
  if((pMe->cmdHdr.nCommandID != AEESTK_DISPLAY_TEXT_REQ) ||
     (!((AEESTKDisplayTextReq*)pMe->m_pProactiveCommand)->bImmediateRespReq))
  {
    /* This is a either not a display txt req or a display text req requiring no iimediate response */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendClearTimerExpired, pMe);
  }
  pMe->term_resp_sent = TRUE;
  pMe->m_icon_present = FALSE;

  /* free the data */
  switch(pMe->cmdHdr.nCommandID)
  {
    case AEESTK_GET_INKEY_REQ:
    case AEESTK_GET_INPUT_REQ:
      CATAPP_FREEIF(pMe->userinput);
      break;
    case AEESTK_SEND_DATA_REQ:
      if (index < CATAPP_MAX_GPRS_CH) {
        pMe->m_sOpenChInfo[index].nByteDataSent = 0;
      }
      break;
    case AEESTK_RECEIVE_DATA_REQ:
      CATAPP_FREEIF(pMe->m_RecvDataRsp.pData);
      break;
    case AEESTK_SEND_USSD_REQ:
      CATAPP_FREEIF(pMe->m_USSDStringRsp.pwszText);
      break;
    default:
      break;
  }

  if(pAdditional_info)
  {
    /* Free the additional info memory that is dynamically allocated for Send SS only */
    CATAPP_FREEIF(pAdditional_info->pAdditionalInfoData);
  }

  pMe->cmdHdr.nCommandID = (AEESTKProactiveCmdType)0;
  pMe->cmdHdr.nCommandNumber = 0;
  pMe->m_CurrChId = 0;
  /* Free the pointer for the proactive command */
  CATAPP_FREEIF(pMe->m_pProactiveCommand); // Its a void pointer anyway.
}

/*===========================================================================

FUNCTION: CloseMoreCharDialog

DESCRIPTION
  This function is invoked when the timer expires and the dialog that is
  displayed when the user enters inapprorpriate number of characters needs
  to be ended

PROTOTYPE:
 void CloseMoreCharDialog(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Closes the Dialogue which displays that more characters need to be entered
===========================================================================*/
void CloseMoreCharDialog(CCatApp *pMe)
{
  MSG_LOW("Closing More Char Dialog",0,0,0);
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }
  (void)ISHELL_EndDialog(pMe->a.m_pIShell);
  (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT,
      (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
}

/*===========================================================================
Function:  CatApp_SearchISocketIndex()

Description:
  This function searches for the index in the open channel info table for a
  particular socket.

Prototype:
  static int CatApp_SearchISocketIndex(CCatApp *pMe, int *index, ISocket *pISocket);

Parameters:
  CCatApp *pMe
  int *index
  ISocket *pISocket

Return Value:
  SUCCESS  : If successful
  EFAILED  : IF failed

Comments:  None

Side Effects: None

==============================================================================*/
static int CatApp_SearchISocketIndex(const CCatApp *pMe, int *index, const ISocket *pISocket)
{
  int i=0;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }
  if(!pISocket)
  {
    MSG_ERROR("pISocket ERR:NULL",0,0,0);
    return FALSE;
  }
  if(!index)
  {
    MSG_ERROR("index ERR:NULL",0,0,0);
    return FALSE;
  }

  for (i = 0; i < CATAPP_MAX_GPRS_CH; i++) {
    if (pMe->m_sOpenChInfo[i].pISocket == pISocket) {
      *index = i;
      return SUCCESS;
    }
  }
  return EFAILED;
} /* CatApp_SearchISocketIndex */

/*===========================================================================
Function:  CatApp_NetEvent

Description:


Prototype:
  static void CatApp_NetEvent(void *p, NetMgrEvent evt, uint32 dwData);

Parameters:
  void *p         -
  NetMgrEvent evt -
  uint32 dwData   -

Return Value:
  static void

Comments:  None

Side Effects: None

==============================================================================*/
static void CatApp_NetEvent(void *p, NetMgrEvent evt, uint32 dwData)
{
  CatApp_OpenChInfo       *pChInfo                = (CatApp_OpenChInfo *)p;
  CCatApp                 *pMe                    = NULL;
  int                      index                  = 0xFF;
  int                      nReturnStatus          = SUCCESS;
  AEESTKAdditionalInfo     additional_info;
  CatApp_CommandRespType  *pCommandResp           = NULL;

  MSG_HIGH("In CatAppNetEvent with event %d and dwData %d",evt,dwData,0);

  if (pChInfo == NULL)
  {
    MSG_HIGH("pChInfo ptr is NULL",0,0,0);
    return;
  }

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  pMe = (CCatApp *)pChInfo->nParentAddress;

  if (pMe == NULL)
  {
    MSG_HIGH("pMe ptr is NULL",0,0,0);
    return;
  }

  MSG_HIGH("evt is %d d",evt,0,0);

  switch(evt) {
  case NE_PPP:
    MSG_HIGH("NE_PPP is received",0,0,0);
    if(dwData == NET_PPP_CLOSED)
    {
      MSG_HIGH("dwData  NET_PPP_CLOSED is received",0,0,0);
      /* Socket here may not be valid if ISocket_Release() has been used
         Hence search index in Open Channel table based on channel id
      */
      nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pChInfo->nChID);
      if (nReturnStatus != SUCCESS)
      {
        MSG_ERROR("Unable to locate Socket Ptr", 0, 0, 0);
        return;
      }
      if (index >= CATAPP_MAX_GPRS_CH) {
        MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
        return;
      }
      /* Clean up other info */
      if (pMe->m_sOpenChInfo[index].status == CATAPP_CH_OPENED)
      {
        /* May be received for link dropped event from NW
           May be received for PDP cpntext Deactivation when card ends call with ISocket_Release
           with NE_SO_CLOSED event being the first event
           When user end call, DS releases PDP context first before Socket Release is done by Catapp
        */
        if(!pMe->term_resp_sent && (pMe->cmdHdr.nCommandID == AEESTK_CLOSE_CH_REQ))
        {
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_CLOSE_CH_SUCCESS, int32touint16(index),
                                   0);
           CatApp_ReleaseNetMgrInst(pMe,index);
           CatApp_CleanupSocketInfo(pMe, index, FALSE);
           pMe->m_sOpenChInfo[index].status = CATAPP_CH_CLOSED;
        }
        else if(!pMe->term_resp_sent && (pMe->cmdHdr.nCommandID == AEESTK_SEND_DATA_REQ))
        {
          /* Link dropped or Channel close --> BIP error with channel close */
          additional_info.nLength = 1;
          additional_info.additionalInfo[0] = AEESTK_CH_CLOSED;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additional_info);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_SEND_DATA_FAILED, 0xFF, (uint32)pCommandResp);
        }
        CatApp_ReleaseSocket(pMe,index);
        CatApp_ReleaseNetMgrInst(pMe,index);
        if(pMe->cmdHdr.nCommandID != AEESTK_CLOSE_CH_REQ)
        {
          /* CatApp_CleanUpSocketInfo should not be called for Receive Data Req from card */
          /* May need to consider user end or network caused link drop */
          pMe->m_sOpenChInfo[index].status = CATAPP_CH_DROPPED;
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_CH_STATUS_CHANGED, int32touint16(index), 0);
        }
      }
    }
    return;
  case NE_SO_CONNECTING:
    nReturnStatus = CatApp_SearchISocketIndex(pMe, &index, (ISocket*)dwData);
    if (nReturnStatus != SUCCESS) {
      MSG_ERROR("Unable to locate Socket Ptr", 0, 0, 0);
      return;
    }
    if (index >= CATAPP_MAX_GPRS_CH) {
      MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
      return;
    }
    pMe->m_sOpenChInfo[index].status = CATAPP_CH_OPEN_WAIT;
    return;

  case NE_SO_OPENING:
  case NE_SO_CONNECTED:
    nReturnStatus = CatApp_SearchISocketIndex(pMe, &index, (ISocket*)dwData);
    if (nReturnStatus != SUCCESS) {
      MSG_ERROR("Unable to locate Socket Ptr", 0, 0, 0);
      return;
    }
    if (index >= CATAPP_MAX_GPRS_CH) {
      MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
      return;
    }
    pMe->m_sOpenChInfo[index].status = CATAPP_CH_OPENED;
    /* may require to post OPEN_CH_SUCCESS from here */
    return;

  case NE_SO_CLOSING:
    nReturnStatus = CatApp_SearchISocketIndex(pMe, &index, (ISocket*)dwData);
    if (nReturnStatus != SUCCESS) {
      MSG_ERROR("Unable to locate Socket Ptr", 0, 0, 0);
      return;
    }
    if (index >= CATAPP_MAX_GPRS_CH) {
      MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
      return;
    }
    pMe->m_sOpenChInfo[index].status = CATAPP_CH_OPENED;
    return;
  case NE_SO_CLOSED:
    if (pChInfo->nChID >= CATAPP_MAX_GPRS_CH)
    {
      MSG_ERROR("Invalid Open Ch Table Index", 0, 0, 0);
      return;
    }
    if (CatApp_FindChSocketIndexFromChID(pMe, &index, pChInfo->nChID) != SUCCESS)
    {
      MSG_ERROR("Cannot locate index from Open Ch Id in Open Ch Table", 0, 0, 0);
      return;
    }
    /* In case of Channel Open Wait state, if Close Channel Req comes in from the card, send Terminal
        Response as no NET_PPP_CLOSED will be received as there was no PDP context activated
    */
    if(pChInfo->status == CATAPP_CH_OPEN_WAIT)
    {
      if(!pMe->term_resp_sent && (pMe->cmdHdr.nCommandID == AEESTK_CLOSE_CH_REQ))
      {
        (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                 EVT_CATAPP_CLOSE_CH_SUCCESS, int32touint16(index),
                                 0);
      }
      CatApp_CleanupSocketInfo(pMe, index, TRUE);
      CatApp_ReleaseSocket(pMe,index);
      CatApp_ReleaseNetMgrInst(pMe,index);
    }
    else
    {
      MSG_ERROR("Unable to locate Channel Id with Open Wait status", 0, 0, 0);
    }
    return;
  default:
    break;
  }
}


/*===========================================================================

Function:  CCatApp_InitAppData()

Description:
        This function initializes the app specific data.

Prototype:
    void CCatApp_InitAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
    to store applet data. (CCatApp*, in this case).

Return Value:
    TRUE: If successful
    FALSE: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
static boolean CCatApp_InitAppData(IApplet* po)
{
  CCatApp *    pMe = (CCatApp*)((void*)po);
  int               nAscent, nDescent;
  wms_client_err_e_type   client_err;
  AEEDeviceInfo *   pdi;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif
  int nReturnStatus;
  int i = 0;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  /* Get screen pixel count */
  pdi = MALLOC(sizeof(AEEDeviceInfo));
  if (!pdi)
    return (FALSE);

  /*Get Device Info */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, pdi);
  pMe->m_cxWidth = pdi->cxScreen;
  pMe->m_cyHeight = pdi->cyScreen;
  pMe->m_nColorDepth = pdi->nColorDepth;
  FREE(pdi);
  pdi = NULL;
  pMe->m_bExit = FALSE;
  /* set Display Settings */
  (void)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_LARGE, &nAscent,
      &nDescent);
  pMe->m_nLChSize = (uint16) (nAscent + nDescent);
  (void)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &nAscent,
      &nDescent);
  pMe->m_nNChSize = (uint16) (nAscent + nDescent);

  pMe->m_pISuppSvc = NULL;
  pMe->m_pISTK = NULL;
  pMe->m_pICM = NULL;
#ifdef FEATURE_UIONE_HDK
  pMe->m_bIdleScreenDisplayed = FALSE;
#endif /* FEATURE_UIONE_HDK */

  /* Intialize INetMgr and channel status*/
  for(i=0;i<CATAPP_MAX_GPRS_CH ;i++)
  {
    pMe->m_sOpenChInfo[i].pINetMgr = NULL;
    pMe->m_sOpenChInfo[i].status = CATAPP_CH_CLOSED;
  }

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

  /*Create an instance of ISTK */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STK,
                                        (void **)&pMe->m_pISTK);
  if ((nReturnStatus != SUCCESS) || (pMe->m_pISTK == NULL))
  {
    MSG_ERROR("Unable to create for ISTK, 0x%x", nReturnStatus, 0, 0);
    CCatApp_FreeAppData((IApplet*)pMe);
    return (FALSE);
  }

  /*Register notification with ISTK */
  nReturnStatus = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                        AEECLSID_CATAPP,
                                        AEECLSID_STKNOTIFIER,
                                        (NMASK_STKNOTIFIER_PROACTIVE_CMD |
                                         NMASK_STKNOTIFIER_ENVELOPE_CMD));
  if (nReturnStatus != SUCCESS)
  {
    MSG_ERROR("Unable to register for ISTK notifications 0x%x", nReturnStatus, 0, 0);
    CCatApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  /*Create an instance for ICM */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CM,
                                        (void **)&pMe->m_pICM);
  if ((nReturnStatus != SUCCESS) || (pMe->m_pICM == NULL))
  {
    MSG_ERROR("Could not Create ICM 0x%x", nReturnStatus, 0, 0);
    CCatApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  /* register with ICM */
  nReturnStatus = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                        AEECLSID_CATAPP,
                                        AEECLSID_CM_NOTIFIER,
                                        (NMASK_CM_VOICE_CALL| NMASK_CM_DATA_CALL |
                                         NMASK_CM_TEST_CALL | NMASK_CM_OTHER_CALL));
  if (nReturnStatus != SUCCESS)
  {
    MSG_ERROR("Unable to register for ICM notifications 0x%x", nReturnStatus, 0, 0);
    CCatApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

#ifdef FEATURE_UIONE_HDK
  /* register with PhoneApp */
  nReturnStatus = ISHELL_RegisterNotify(pMe->a.m_pIShell,
                                        AEECLSID_CATAPP,
                                        AEECLSID_NATIVENOTIFIER,
                                        NMASK_NATIVENOTIFIER_ALL);
  if (nReturnStatus != SUCCESS)
  {
    MSG_ERROR("Unable to register for Native Notifier notifications 0x%x", nReturnStatus, 0, 0);
    CCatApp_FreeAppData((IApplet*)pMe);
    return FALSE;
  }

  /* Initialize callback for device info change notification - specifically for language notification */
  CALLBACK_Init(&pMe->m_LangUserCb, (PFNNOTIFY)CatApp_DeviceInfoChangeNotifCb,
                (void*)pMe);

  /* Register callback for device info change notification - specifically for language notification */
  ISHELL_RegisterSystemCallback(pMe->a.m_pIShell,
                                &pMe->m_LangUserCb,
                                AEE_SCB_DEVICE_INFO_CHANGED);


#endif /* FEATURE_UIONE_HDK */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)*/

  pMe->m_lang[0] = 0x65;
  pMe->m_lang[1] = 0x6E;

  pMe->m_bSuspended = FALSE;
  pMe->numItemsInMenu = 0;
  pMe->m_bSetupMenuHelp = FALSE;
  pMe->m_bDisplayingText = FALSE;
  pMe->m_PendingCallSetUp = FALSE;
  pMe->m_pProactiveCommand = NULL;
  pMe->nOpId = (uint8) -1;

  pMe->m_bSKMainMenu = FALSE;
  /* Initialize WMS */
  /* initialize the cat app as wms client */
  client_err = wms_client_init(WMS_CLIENT_TYPE_CAT,  &pMe->wms_id );
  if (client_err != WMS_CLIENT_ERR_NONE)
  {
     MSG_ERROR("wms_client_init returned %d", client_err, 0, 0);
     CCatApp_FreeAppData((IApplet*)pMe);
     return FALSE;
  }

  client_err = wms_client_activate( pMe->wms_id );
  if (client_err != WMS_CLIENT_ERR_NONE)
  {
     MSG_ERROR("wms_client_activate returned %d", client_err, 0, 0);
     CCatApp_FreeAppData((IApplet*)pMe);
     return FALSE;
  }
  client_err = wms_client_reg_msg_cb(pMe->wms_id,
                                     (wms_msg_event_cb_type)catapp_wmseventcb);
  if (client_err != WMS_CLIENT_ERR_NONE)
  {
     MSG_ERROR("wms_client_reg_msg_cb returned %d", client_err, 0, 0);
     CCatApp_FreeAppData((IApplet*)pMe);
     return FALSE;
  }

  /* Intialize Proactive End Session Flag to None */
  pMe->m_EndProactiveSession = CATAPP_END_PROACTIVE_SESSION_NONE;

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif     /* FEATURE_CATAPP_TEST_AUTOMATION */

  return (TRUE);
}

/*===========================================================================

FUNCTION: CreateDisplayTextDlg

DESCRIPTION
  This function is invoked when the Display Text Dialog needs to be created.
  The Display Text Dialog is used for the following proactive Commands:
  GSTK_DISPLAY_TEXT_REQ
  GSTK_PLAY_TONE_REQ
  GSTK_SEND_SMS_REQ

PROTOTYPE:
 boolean CreateDisplayTextDlg(CCatApp *pMe, uint16 wId)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates the Display Text Dialog and makes it the currently active dialog
===========================================================================*/
boolean CreateDisplayTextDlg(const CCatApp * pMe, uint16 wId)
{
  int ret_status;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                            wId, NULL);

  if (ret_status == EFAILED) { /* Creation failed */
    return (FALSE);
  }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  return (TRUE); /* Created successfully */
}

/*===========================================================================

FUNCTION: CreateDisplayTextDlg

DESCRIPTION
  This function is invoked when the user has entered invalid number of
  characters when the Get Input or Get Inkey command comes in. This function
  creates a new dialog and displays text to user indicating that invalid
  number of characters were entered.

  This Dialog is used in the following proactive Commands:
  GSTK_GET_INPUT_REQ

PROTOTYPE:
 boolean CreateMoreCharsReqd(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates a new Dialog and makes it the currently active dialog
===========================================================================*/
boolean CreateMoreCharsReqd(const CCatApp * pMe)
{
  int ret_status;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                            IDD_GET_INPUT_ERROR, NULL);

  if (ret_status == EFAILED) { /* Creation failed */
    return (FALSE);
  }

  return (TRUE); /* Created successfully */
}


/*===========================================================================

FUNCTION: CreateMenuDlg

DESCRIPTION
  This function is invoked when the Select Item proactive command comes in
  This dialog displays a list of items in a menu
  This dialog is used in the following proactive Commands:
  GSTK_SELECT_ITEM_REQ

PROTOTYPE:
 boolean CreateMenuDlg(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates a new Dialog and makes it the currently active dialog
===========================================================================*/
boolean CreateMenuDlg(const CCatApp *pMe)
{
  int ret_status;
  AEESTKSelectItemReq *pSelectItemReq;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  pSelectItemReq = (AEESTKSelectItemReq*) pMe->m_pProactiveCommand;

  if(pSelectItemReq->bSoftkeySelection)
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                          IDD_SK_SELECT_ITEM, NULL);
  }
  else
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                              IDD_SELECT_ITEM, NULL);
  }
  if (ret_status == EFAILED)
  { /* Creation failed */
    return (FALSE);
  }

  return (TRUE); /* Created successfully */
}


/*===========================================================================

FUNCTION: CreateUserSKDlg

DESCRIPTION
  This function is invoked when a dialog needs to be created which requires
  a user input where the user needs to key in an input.
  This function is used in the following proactive Commands:
  GSTK_GET_INKEY_REQ
  GSTK_SETUP_CALL_REQ



PROTOTYPE:
 boolean CreateUserSKDlg(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.
  pIMenu: [in]: Pointer to the menu whose style needs to be set.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sets the properties for dialog depending on what the proactive command
  requested
===========================================================================*/
boolean CreateUserSKDlg(CCatApp *pMe)
{
  IDialog  *pIActiveDialog = NULL;
  AECHAR   *pszText        = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic  *pIStaticText   = NULL;
  boolean   bReturnStatus  = TRUE;
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL) {
    return (FALSE); /* error */
  }
  else
  {
    /* initialize the controls */
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                                                IDC_SK_SKUSER_INPUT);
    pIStaticText = (IStatic*)IDIALOG_GetControl(pIActiveDialog,
                                                IDC_STATIC_SKUSER_INPUT);

    if(pISoftKeyMenu && pIStaticText)
    {
      /* title */
      switch(pMe->cmdHdr.nCommandID) {
      case AEESTK_GET_INKEY_REQ:
        pszText =
          ((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->textString.pwszText;
        break;

      case AEESTK_SETUP_CALL_REQ:
        pszText =
          ((AEESTKSetupCallReq*)(pMe->m_pProactiveCommand))->userCnfAlpha.pwszText;
        break;

      case AEESTK_OPEN_CH_REQ:
        pszText =
          ((AEESTKOpenChReq*)(pMe->m_pProactiveCommand))->sAlpha.pwszText;
        break;

      default:
        break;
      }

      if (pszText == NULL) {
        /* Nothing to be displayed should not have come here in the first place! */
        MSG_ERROR("Nothing to be Display", 0, 0, 0);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
        return FALSE;
      }

      bReturnStatus = ISTATIC_SetText(pIStaticText, NULL, pszText,
            AEE_FONT_BOLD, AEE_FONT_BOLD);
      if (bReturnStatus != TRUE)
      {
        MSG_ERROR("Could not Set Text - Error %d", bReturnStatus, 0, 0);
      }
      (void)ISTATIC_Redraw(pIStaticText);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      /* remove all the item in softkey menu */
      (void)IMENUCTL_DeleteAll(pISoftKeyMenu);
      (void)IMENUCTL_AddItem(pISoftKeyMenu, CATAPP_RES_FILE, IDS_CATAPP_YES,
        IDL_SKUSER_INPUT_YES,NULL, 0);
      (void)IMENUCTL_AddItem(pISoftKeyMenu, CATAPP_RES_FILE, IDS_CATAPP_NO,
        IDL_SKUSER_INPUT_NO,NULL, 0);

      /* set the string in the control */
#ifdef FEATURE_CAT_REL6
      if (pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)
      {
        if((((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
              AEESTK_YES_NO_AND_IMM_DGT_RSP))
        {
          CatApp_SetHideSelectionSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
        }
        else
        {
          /* This case is a simple yes/no response */
          SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
        }
      }
      else
#endif /* FEATURE_CAT_REL6 */
      {
        /* Set this default for all other commands */
        SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
      }
      /* Set textctl properties */
      (void)IMENUCTL_Redraw(pISoftKeyMenu);

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      return (TRUE);

    }/* if(pISoftKeyMenu && pIStaticText) */
  }/* pIactiveDialog == NULL */
  return (FALSE); /* error */
}

/*===========================================================================

FUNCTION: CreateGetInputDlg

DESCRIPTION
  This function is invoked when a dialog needs to be created which requires
  a user input where the user needs to key in an input.
  This function is used in the following proactive Commands:
  GSTK_GET_INPUT_REQ
  GSTK_GET_INKEY_REQ


PROTOTYPE:
 boolean CreateGetInputDlg(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.
  pIMenu: [in]: Pointer to the menu whose style needs to be set.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sets the properties for dialog depending on what the proactive command
  requested
===========================================================================*/
boolean CreateGetInputDlg(CCatApp *pMe)
{
  IDialog  *pIActiveDialog = NULL;
  AECHAR   *szText         = NULL;
  AECHAR   *pszText        = NULL;
  ITextCtl *pIInputEdit    = NULL;
  boolean   bReturnStatus;
  uint32    nTextLen       = 0;
  uint32    nCopiedLen     = 0;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL) {
    return (FALSE); /* error */
  }
  else
  {
    /* initialize the controls */
    pIInputEdit = (ITextCtl*)IDIALOG_GetControl(pIActiveDialog,
                                                IDC_GET_INPUT_EDIT);

    if(pIInputEdit)
    {
      /* title */
      switch(pMe->cmdHdr.nCommandID) {
      case AEESTK_GET_INKEY_REQ:

        pszText =
          ((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->textString.pwszText;
        nTextLen = (pszText!= NULL)? int32touint32(WSTRLEN(pszText)) : 0;
        break;

      case AEESTK_GET_INPUT_REQ:
        pszText =
          ((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->textString.pwszText;
        nTextLen = (pszText!= NULL)? int32touint32(WSTRLEN(pszText)) : 0;
        break;

      default:
        break;
      }

      /* set the string in the control */
      szText = MALLOC((nTextLen+1) * sizeof(AECHAR));
      if (szText == NULL)
      {
        MSG_ERROR("szText ERR:NULL",0,0,0);
        return FALSE;
      }
      if (pszText != NULL)
      {
        nCopiedLen = WSTRLCPY(szText, pszText, (nTextLen+1));
        if(nCopiedLen != nTextLen)
        {
          MSG_HIGH("Incorrect Length for Get Input DIAG %d",
                    nTextLen,0, 0);
        }

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      }
      else
      {
        szText[0] = 0x00;
      }
      bReturnStatus = ITEXTCTL_SetTitle(pIInputEdit, NULL, 0, szText);

      if (bReturnStatus != TRUE)
      {
        MSG_ERROR("Could not set title - returned %d", bReturnStatus, 0, 0);
      }

      switch (pMe->cmdHdr.nCommandID) {

      case AEESTK_GET_INKEY_REQ:
        /* if get inkey proactive command comes in, set the max input */
        /* size to 1 */
        ITEXTCTL_SetMaxSize(pIInputEdit, GET_INKEY_INPUT_SIZE);

        switch (((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat)
        {
        case AEESTK_NUMERICAL_ONLY:
          /* set proprties for text control */
          ITEXTCTL_SetProperties(pIInputEdit, TP_FRAME | TP_MULTILINE );
          /* if card specifies numbers only input */
#if MIN_BREW_VERSION(3,1)
          (void)ITEXTCTL_SetInputMode(pIInputEdit, AEE_TM_DIALED_DIGITS);
#else
          ITEXTCTL_SetInputMode(pIInputEdit, OEM_TM_DIALED_DIGITS);
#endif /* FEATURE_BREW */
          break;
        case AEESTK_YES_NO:
          return FALSE;
        case AEESTK_IMMEDIATE_DIGIT_RSP:
#ifdef FEATURE_CAT_REL6
          return TRUE;
#else
          MSG_HIGH("AEESTK_IMMEDIATE_DIGIT_RSP: FEATURE_CAT_REL6 not supported",0,0,0);
          return FALSE;
#endif
        default:
          /* set proprties for text control */
          ITEXTCTL_SetProperties(pIInputEdit, TP_FRAME | TP_MULTILINE );
          /* if card does not specify numbers only input */
          (void)ITEXTCTL_SetInputMode(pIInputEdit, AEE_TM_LETTERS);
          break;
        }
        break;

      case AEESTK_GET_INPUT_REQ:
        /* set proprties for text control */
        ITEXTCTL_SetProperties(pIInputEdit, TP_FRAME | TP_MULTILINE );
        switch (((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->responseFormat) {
        case AEESTK_NUMERICAL_ONLY:
          if (((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->bShowUserInput ==
            FALSE) {
            ITEXTCTL_SetProperties(pIInputEdit,
                                     TP_FRAME | TP_MULTILINE |TP_PASSWORD );
          }
          /* if card specifies numbers only input */
#if MIN_BREW_VERSION(3,1)
          (void)ITEXTCTL_SetInputMode(pIInputEdit, AEE_TM_DIALED_DIGITS);
#else
          ITEXTCTL_SetInputMode(pIInputEdit, OEM_TM_DIALED_DIGITS);
#endif /* FEATURE_BREW */
          break;

        default:
          /* if card does not specify numbers only input */
          (void)ITEXTCTL_SetInputMode(pIInputEdit, AEE_TM_LETTERS);
          break;
        }
        /*Set max input size for control as specified by card */
        ITEXTCTL_SetMaxSize(pIInputEdit,
             ((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->nMaxUserInput);

        /* if default text is specified by the card,
         * set that in text control dialog */
        if(((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->defaultText.pwszText != NULL)
        {
          pszText =
              ((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->defaultText.pwszText;

          nTextLen = (pszText!= NULL)? int32touint32(WSTRLEN(pszText)) : 0;

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
          /* Free previous memory allocation for szText */
          CATAPP_FREE(szText);
          szText = MALLOC((nTextLen+1) * sizeof(AECHAR));
          if (szText == NULL)
          {
            MSG_ERROR("szText ERR:NULL",0,0,0);
            return FALSE;
          }

          (void) MEMSET(szText, 0x00, (nTextLen+1)*sizeof(AECHAR));

          nCopiedLen = WSTRLCPY(szText, pszText, (nTextLen+1));
          if(nCopiedLen != nTextLen)
          {
            MSG_HIGH("Incorrect Length for Get Input DIAG %d",
                      nTextLen,0, 0);
          }
          (void)ITEXTCTL_SetText(pIInputEdit, szText, (int) (nTextLen+1));
        }
        break;
      default:
        break;
      }

      /* set the dialog focus to this current dialog */
      CATAPP_FREE(szText);

      /* Set textctl properties */
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      return (TRUE);

    }/* if(pIStaticText && pIInputEdit && pISoftKeyMenu) */

  }/* pIactiveDialog ==NULL */
  return (FALSE); /* error */
}


/*===========================================================================

FUNCTION: CreateSelectItemDlg

DESCRIPTION
  This function is invoked when a proactive command comes in and a
  dialog needs to be created which requires a user input where the user
  needs to select an item from a menu.
  This function is used in the following proactive Commands:
  GSTK_SELECT_ITEM_REQ


PROTOTYPE:
  boolean CreateSelectItemDlg(CCatApp *pMe, IDialog* pIActiveDialog, uint16 wDlgID)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
  information specific to this applet. It was initialized during the
  AppCreateInstance() function.
  pIActiveDialog: [in]: Pointer to the currently active dialog.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sets the properties for dialog depending on what the proactive command
  requested and adds the items to the menu
===========================================================================*/
/*lint -e818 pMe could be constant */
boolean CreateSelectItemDlg(CCatApp *pMe, IDialog* pIActiveDialog, uint16 wDlgID)
{
  IMenuCtl          *pICurrentMenu   = NULL; /* softkey menu */
  boolean           bReturnStatus    = TRUE;
  int               i                = 0;
  uint32            nTextLen         = 0;
  AECHAR            *szText          = NULL;
  const uint16      *pItemListID     = NULL;
  IStatic           *pIStaticText    = NULL;
  uint32            nCopiedLen       = 0;

  AEESTKSelectItemReq *pSelectItemReq;

  static char displaytext[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =    " (Display Text)";
  static char getinput[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =       " (Get Input)   ";
  static char selectitem[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =     " (Select Item) ";
  static char setupcall[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =      " (Setup Call)  ";
  static char sendss[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]    =      " (Send SS)     ";
  static char sendussd[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]  =      " (Send USSD)   ";
  static char sendsms[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]   =      " (Send SMS)    ";
  static char launchbrowser[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =  " (Launch Brwsr)";
  static char playtone[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =       " (Play Tone)   ";
  static char getinkey[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =       " (Get Inkey)   ";
  static char setupmenu[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =      " (Setup Menu)  ";
  static char setupidletext[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]=   " (SetupIdleTxt)";
  static char endofproactive[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] = " (EndProactive)";
  static char refresh[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]        = " (Refresh)     ";
  static char moretime[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =       " (More Time)   ";
  static char pollinterval[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =   " (PollInterval)";
  static char setupeventlist[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] = " (SetupEvtList)";
  static char provideloci[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG] =    " (Local Info)  ";
  static char runatcmd[LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG]     =   " (Run AT Cmd)  ";
  AECHAR                          NextActionText[CATAPP_BUFF_SIZE];

  (void) MEMSET(NextActionText, 0x00, CATAPP_BUFF_SIZE);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  pSelectItemReq = (AEESTKSelectItemReq*) pMe->m_pProactiveCommand;

  if (pIActiveDialog == NULL)
  {
    return (FALSE); /* error */
  }
  switch (wDlgID)
  {
  case IDD_SELECT_ITEM:
    /* get the menu control */
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,IDC_SELECT_ITEM_MENU);
    pItemListID = SelectItemDialogResID;
    break;
  case IDD_SK_SELECT_ITEM:
    /* get the menu control */
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,IDC_SK_SELECT_ITEM_MENU);
    pIStaticText = (IStatic*) IDIALOG_GetControl(pIActiveDialog, IDC_SK_STATIC_SELECT_ITEM_TITLE);
    pItemListID = SelectItemSKDialogResID;
    break;
  default:
    return FALSE;
  }

  if (pICurrentMenu == NULL)
  {
    return FALSE;
  }

  if(pSelectItemReq->title.pwszText != NULL)
  { /* if a title is specified set the title */
    nTextLen = int32touint32(WSTRLEN(pSelectItemReq->title.pwszText));
    szText = MALLOC((nTextLen+1) * sizeof(AECHAR));
    if (szText == NULL)
    {
      MSG_ERROR("szText ERR:NULL",0,0,0);
      return FALSE;
    }
    (void) MEMSET(szText, 0x00, (nTextLen+1)*sizeof(AECHAR));
    nCopiedLen = WSTRLCPY(szText,pSelectItemReq->title.pwszText, (nTextLen+1));
    if(nCopiedLen != nTextLen)
    {
      MSG_HIGH("Incorrect Length for Select item title %d", nTextLen, 0, 0);
    }

    if (wDlgID == IDD_SELECT_ITEM )
    {
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, NULL, 0, szText);
    }
    else
    {
      if (pIStaticText == NULL)
      {
        CATAPP_FREE(szText);
        return FALSE;
      }
      bReturnStatus = ISTATIC_SetText(pIStaticText, NULL, szText, AEE_FONT_BOLD,
                                      AEE_FONT_BOLD);
    }
    if (bReturnStatus != TRUE)
    {
      MSG_ERROR("Could not set title return value %d", bReturnStatus, 0,0);
    }
    CATAPP_FREE(szText);
  }

  bReturnStatus = IMENUCTL_DeleteAll(pICurrentMenu);
  if (bReturnStatus!=TRUE)
  {
    MSG_ERROR("Could not DeleteAll, returned %d", bReturnStatus, 0, 0);
    return FALSE;
  }
  for (i=0; i<pSelectItemReq->nNumItems && i< MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU; i++)
  {
    if((pSelectItemReq->nextActionIndicator.nNumItems >0) &&
        (i < pSelectItemReq->nextActionIndicator.nNumItems))
    {
      /* question here: */
      /*I assume that if a next action identifier is provided, it is provided
        * for all menu items */
      /* is this true or not? */
      /* append the next action identifier to the string to be displayed on
        * menu */
      /* LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG: includes 1 for null byte + 15
        * for max len for Next Action Identifier */
      nTextLen = int32touint32(WSTRLEN(pSelectItemReq->items[i].pwszText) +
                               LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG);

      /* copy the item string    */
      szText = MALLOC((nTextLen) * sizeof(AECHAR));
      if (szText == NULL)
      {
        MSG_ERROR("szText ERR:NULL",0,0,0);
        return FALSE;
      }
      (void) MEMSET(szText, 0x00, (nTextLen)*sizeof(AECHAR));
      nCopiedLen = WSTRLCPY(szText, pSelectItemReq->items[i].pwszText, nTextLen );
      if(nCopiedLen != (nTextLen - LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG))
      {
        MSG_HIGH("Incorrect Length for Select Item %d",
                 (nTextLen - LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG),0, 0);
      }

      switch (pSelectItemReq->nextActionIndicator.pNextActionList[i])
      {
        case AEESTK_CMD_STK_DISPLAY_TEXT:
          (void) STRTOWSTR(displaytext, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_GET_INPUT:
          (void) STRTOWSTR(getinput, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SELECT_ITEM:
          (void) STRTOWSTR(selectitem, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_REFRESH:
          (void) STRTOWSTR(refresh, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_MORE_TIME:
          (void) STRTOWSTR(moretime, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_POLL_INTERVAL:
          (void) STRTOWSTR(pollinterval, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SET_UP_EVENT_LIST:
          (void) STRTOWSTR(setupeventlist, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SET_UP_CALL:
          (void) STRTOWSTR(setupcall, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SEND_SS:
          (void) STRTOWSTR(sendss, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_USSD:
          (void) STRTOWSTR(sendussd, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SEND_SHORT_MESSAGE:
          (void) STRTOWSTR(sendsms, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_LAUNCH_BROWSER:
          (void) STRTOWSTR(launchbrowser, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_PLAY_TONE:
          (void) STRTOWSTR(playtone, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_GET_INKEY:
          (void) STRTOWSTR(getinkey, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SET_UP_MENU:
          (void) STRTOWSTR(setupmenu, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_PROVIDE_LOCAL_INFO:
          (void) STRTOWSTR(provideloci, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
          (void) STRTOWSTR(setupidletext, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_RUN_AT_COMMAND:
          (void) STRTOWSTR(runatcmd, NextActionText, CATAPP_BUFF_SIZE);
          break;

        case AEESTK_CMD_STK_END_OF_PROACTIVE_SES:
          (void) STRTOWSTR(endofproactive, NextActionText, CATAPP_BUFF_SIZE);
          break;

        default:
          MSG_ERROR("Unexpected next step %d",
                pSelectItemReq->nextActionIndicator.pNextActionList[i],
                0, 0);
          break;
      }
      nCopiedLen = WSTRLCAT(szText, NextActionText, nTextLen);
      if(nCopiedLen != (nTextLen - 1))
      {
        MSG_HIGH("Incorrect Length for Select Item and Next Action Text %d",
                 (nTextLen-1),0, 0);
      }
    }
    else
    {
      /* no next item identifier */
      if(pSelectItemReq->items[i].pwszText != NULL) {

        nTextLen = int32touint32(WSTRLEN(pSelectItemReq->items[i].pwszText));
        szText = MALLOC((nTextLen+1) * sizeof(AECHAR));
        if(szText != NULL)
        {
          /* copy the item string    */
          (void) MEMSET(szText, 0x00, (nTextLen+1)*sizeof(AECHAR));
          nCopiedLen = WSTRLCPY(szText, pSelectItemReq->items[i].pwszText,
                                (nTextLen+1));
          if(nCopiedLen != nTextLen)
          {
            MSG_HIGH("Incorrect Length for NO next item identifier %d",
                      nTextLen,0, 0);
          }
        }
      }
      else { /* application requests NULL text display */
        nTextLen = 1;
        szText = MALLOC((nTextLen+1) * sizeof(AECHAR));
        if(szText != NULL)
        {
          (void) STRTOWSTR(" ",
                          szText,
                          (int) ((nTextLen) * sizeof(AECHAR)));
        }
      }

    }

    /* add the item to the menu */

    if(szText != NULL) {
      bReturnStatus = IMENUCTL_AddItem(pICurrentMenu, NULL, 0,
          pItemListID[i], szText,
          pSelectItemReq->items[i].nLength);
      if (bReturnStatus != TRUE)
      {
        FREE(szText);
        szText = NULL;
        MSG_ERROR("Could not Add Item, returned %d", bReturnStatus, 0, 0);
        return FALSE;
      }
      /* Set default */
      if (pSelectItemReq->nDefaultItem == pSelectItemReq->items[i].nLength) {
          IMENUCTL_SetSel(pICurrentMenu, pItemListID[i]);
      }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION  */

      FREE(szText);
      szText = NULL;
    }

  } /* for i=0 -> pMe->m_pProactiveCommand->cmd.select_item_pro_cmd_req.num_items */
    /* set standard menu style */
  if (wDlgID == IDD_SELECT_ITEM)
  {
    SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  }
  else
  {
    SetDefaultSoftkeyLook(pMe->a.m_pIShell, pICurrentMenu);
  }

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION  */

  MSG_HIGH("Presentation Style = 0x%x", pSelectItemReq->presentation, 0, 0);
  return (TRUE);
}
/*lint !e818 */

/*===========================================================================

FUNCTION: CreateMainMenuDlg

DESCRIPTION
  This function is invoked when the user starts the Cat App from the main menu.
  It is used to create the Main Menu dialog. This function is only called if a
  setup menu proactive command has already come in


PROTOTYPE:
 boolean CreateMainMenuDlg(CCatApp *pMe, IDialog* pIActiveDialog, uint16 wDlgID)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
  information specific to this applet. It was initialized during the
  AppCreateInstance() function.
  pIActiveDialog: [in]: Pointer to the currently active dialog.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Sets the properties for dialog and displays the items specified in the last
  setup menu proactive command
===========================================================================*/
boolean CreateMainMenuDlg(const CCatApp *pMe, IDialog* pIActiveDialog, uint16 wDlgID)
{
  IMenuCtl *pICurrentMenu = NULL;
  boolean   bReturnStatus;
  uint32    i = 0;
  IStatic  *pIStaticText = NULL;
  AEERect   MenuRect;
  AEERect   StaticRect;
  const uint16 *pItemListID = NULL;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  if (pIActiveDialog == NULL) {
    return FALSE; /* error */
  }
  switch (wDlgID)
  {
  case IDD_MAIN_CAT:
    /* get the menu control */
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,IDC_CATMAIN);
    pIStaticText = (IStatic*) IDIALOG_GetControl(pIActiveDialog, IDC_CATNOMENU);
    pItemListID = MainMenuItemResID;
    break;
  case IDD_SK_MAIN_CAT:
    /* get the menu control */
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,IDC_SK_CATMAIN);
    pIStaticText = (IStatic*) IDIALOG_GetControl(pIActiveDialog, IDC_SK_CATTITLE);
    pItemListID = MainMenuSKItemResID;
    break;
  default:
    return FALSE;
  }

  if((pICurrentMenu == NULL) || (pIStaticText == NULL)) {
    return FALSE;
  }


  if(pMe->numItemsInMenu == 0)
  {
    /* if no setup menu command has come in yet */
    IMENUCTL_GetRect(pICurrentMenu, &MenuRect);
    ISTATIC_GetRect(pIStaticText, &StaticRect);
    StaticRect.y = 0;
    StaticRect.dy = (int16) (pMe->m_cyHeight);
    MenuRect.x = -1;
    MenuRect.y = -1;
    MenuRect.dx = 0;
    MenuRect.dy = 0;
    ISTATIC_SetRect (pIStaticText, &StaticRect);
    IMENUCTL_SetRect(pICurrentMenu, &MenuRect);
    ISTATIC_SetActive(pIStaticText, TRUE);
    return TRUE;
  }
  /* if a setup menu command has come in. */
  if (pMe->setupmenutitlepresent)
  {
    if (wDlgID == IDD_MAIN_CAT) {
      /* set the title if title was given by the card */
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, NULL, 0, pMe->setupmenutitle);
    }
    else {
      /* set the text that came in from GSTK */
      bReturnStatus = ISTATIC_SetText(pIStaticText, NULL, pMe->setupmenutitle, AEE_FONT_BOLD,
                                        AEE_FONT_BOLD);
    }

    if (bReturnStatus != TRUE){
      MSG_ERROR("Could not set title in main menu", 0, 0, 0);
      return FALSE;
    }
  }
  /*Delete all the items from the menu */
  bReturnStatus = IMENUCTL_DeleteAll(pICurrentMenu);
  if (bReturnStatus != TRUE)
  {
    MSG_ERROR("Could not Delete All, returned %d", bReturnStatus, 0, 0);
    return FALSE;
  }

  for (i=0; i < pMe->numItemsInMenu && i < MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU; i++)
  {
    bReturnStatus = IMENUCTL_AddItem(pICurrentMenu, NULL, 0,
    pItemListID[i], pMe->StaticMenu[i], pMe->menuitemid[i]);
    if (bReturnStatus != TRUE)
    {
      MSG_ERROR("Could not Add Item, returned %d", bReturnStatus, 0, 0);
      return FALSE;
    }
            /* Set default */
    if (pMe->default_setupmenu_item == pMe->menuitemid[i]) {
        IMENUCTL_SetSel(pICurrentMenu, pItemListID[i]);
    }
  }
  /* set standard menu style */
  if (wDlgID == IDD_MAIN_CAT) {
    SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  }
  else {
    SetDefaultSoftkeyLook(pMe->a.m_pIShell, pICurrentMenu);
  }

  return (TRUE);

}

/*===========================================================================
Function:  CCatApp_FreeAppData()

Description:
    This function frees the app data. This function is registered with the
    applet framework when the applet is created
    (inside AEEClsCreateInstance() function).

    This function is called by the app framework when the reference count of the
    applet reaches zero. This function must free all the app data that has been
    allocated by this app. For ex: if their is data that was remembered when
    the app is suspended and resumed, those data must be freed here.

Prototype:
    void CCatApp_FreeAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
    to store applet data. (CCatApp*, in this case).

Return Value:
    None

Comments:  None

Side Effects: None
==============================================================================*/
void CCatApp_FreeAppData(IApplet* po)
{
  CCatApp * pMe = (CCatApp *)((void*)po);
  int i = 0;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }

  if (pMe->m_pISTK)
  {
    (void)ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_CATAPP, AEECLSID_STKNOTIFIER, 0);
    (void)ISTK_Release(pMe->m_pISTK);
    pMe->m_pISTK = NULL;
  }

  if(pMe->m_pICM)
  {
    (void)ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_CATAPP, AEECLSID_CM_NOTIFIER, 0);
    (void)ICM_Release(pMe->m_pICM);
    pMe->m_pICM = NULL;
  }

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

  for (i = 0; i< CATAPP_MAX_GPRS_CH; i++) {
    CatApp_CleanupSocketInfo(pMe, i, TRUE);
    CatApp_ReleaseSocket(pMe,i);
    CatApp_ReleaseNetMgrInst(pMe,i);
  }

#ifdef FEATURE_UIONE_HDK
  CALLBACK_Cancel((AEECallback*)&pMe->m_LangUserCb);
#endif /* FEATURE_UIONE_HDK */
}

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must
    provide this function. Ensure to retain the same name and parameters for
    this function.

    In here, the module must verify the ClassID and then invoke the
    AEEApplet_New() function that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specifici
   initialization. In this example, a generic structure is provided so that
   app developers need not change app specific initialization section every time.

PROTOTYPE:
    int AEEClsCreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po,
                             void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell interface.

    pIModule: pin]: Contains pointer to the IModule interface to the current
                    module to which this app belongs

    ppObj: [out]: On return, *ppApplet must point to a valid IBase object.

    If the classID  represnts an applet, then ppObj must point to a valid
    AEEApplet structure.  Allocation of memory for this structure and initializing
    the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful.
  EFAILED: Error occurred while creating the class/app. In this case,
           the app/class will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int CCatApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
  if(ppObj == NULL)
  {
    return EFAILED;
  }

  *ppObj = NULL;

  /* Here a check is done to see if the ClsID is that of Cat app.          */
  /* The reason is if this module has more than one applets or classes,    */
  /* then this function is invoked                                         */
  /* once for each applet or class. Checking here ensures that the correct */
  /* IApplet or class object is constructed.                               */
  if(ClsId == AEECLSID_CATAPP)
  {
    /*Create the applet */
    if(AEEApplet_New(sizeof(CCatApp), ClsId, pIShell,po,(IApplet**)ppObj,
          (AEEHANDLER)CCatApp_HandleEvent,(PFNFREEAPPDATA)CCatApp_FreeAppData))
    {
      /*Initialize applet data */
      if(CCatApp_InitAppData((IApplet*)*ppObj))
      {
        /*Data initialized successfully */
        return(AEE_SUCCESS);
      }
    }/*AEEApplet_New */
    /*Release the applet. This will free the memory allocated for the applet */
    (void)IAPPLET_Release(((IApplet*)*ppObj));
    *ppObj = NULL;
    return EFAILED;
  }/* ClsId == AEECLSID_CATAPP */

  return(EFAILED);
}


/*===========================================================================

FUNCTION: DisplayTextEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Display Text Dialog.
    The Display Text Dialog is used in the following proactive commands:
    GSTK_DISPLAY_TEXT_REQ
    GSTK_PLAY_TONE_REQ
    GSTK_SEND_SMS_REQ

PROTOTYPE:
    boolean DisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean DisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCatApp                *pMe                 = (CCatApp*) pUser;
  IStatic                *pIStaticText        = NULL;
  AECHAR                 *pszText             = NULL;
  boolean                 bReturnStatus       = TRUE;
  int                     nReturnStatus       = 0;
  AEESTKAdditionalInfo    additional_info;
  CatApp_CommandRespType *pCommandResp        = NULL;
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

  MSG_HIGH("DisplayTextEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

   /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND
 #ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
 #endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    )
  {
    /* No Timer required as timeout should not send a TR to the card
       i.e User did not respond is not a valid response for these commands
       For DISPLAY TEXT cmd, we start timer in CCatApp_HandleEvent()
    */
    if ((pMe->cmdHdr.nCommandID != AEESTK_PLAY_TONE_REQ) &&
        (pMe->cmdHdr.nCommandID != AEESTK_CLOSE_CH_REQ)  &&
        (pMe->cmdHdr.nCommandID != AEESTK_SEND_SMS_REQ) &&
        (pMe->cmdHdr.nCommandID != AEESTK_DISPLAY_TEXT_REQ)
       )
    {
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendTimerExpired, pMe);
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, USER_TIMEOUT,
          (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
    }
  }

  switch (evt)
  {
    case EVT_DIALOG_START:

      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_DISP_TEXT);
      if(pIStaticText)
      {
        ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
        /* get the text that came in from gstk */
        switch(pMe->cmdHdr.nCommandID)
        {
        case AEESTK_DISPLAY_TEXT_REQ:
          pszText = ((AEESTKDisplayTextReq*)(pMe->m_pProactiveCommand))->textString.pwszText;
          break;
        case AEESTK_PLAY_TONE_REQ:
          pszText = ((AEESTKPlayToneReq*)(pMe->m_pProactiveCommand))->alpha.pwszText;
          break;
        case AEESTK_SEND_SMS_REQ:
          pszText = ((AEESTKSendSMSReq*)(pMe->m_pProactiveCommand))->alpha.pwszText;
          break;
        case AEESTK_SEND_DTMF_REQ:
          pszText = ((AEESTKSendDTMFReq*)(pMe->m_pProactiveCommand))->alpha.pwszText;
          break;
        case AEESTK_SEND_DATA_REQ:
          pszText = ((AEESTKSendDataReq*)(pMe->m_pProactiveCommand))->sAlpha.pwszText;
          break;
        case AEESTK_RECEIVE_DATA_REQ:
          pszText = ((AEESTKReceiveDataReq*)(pMe->m_pProactiveCommand))->sAlpha.pwszText;
          break;
        case AEESTK_CLOSE_CH_REQ:
          pszText = ((AEESTKCloseChReq*)(pMe->m_pProactiveCommand))->sAlpha.pwszText;
          break;
        case AEESTK_OPEN_CH_REQ:
          pszText = MALLOC(sizeof(AECHAR)*50);
          if (pszText == NULL) {
            return FALSE;
          }
          nReturnStatus = ISHELL_LoadResString(pMe->a.m_pIShell, CATAPP_RES_FILE, IDS_OPENING_CH,
                    pszText, 50*sizeof(AECHAR));

          if (nReturnStatus == 0) {
            MSG_ERROR("LoadResString failed for Open Channel Req",0,0,0);
            FREE(pszText);
            pszText = NULL;
            return FALSE;
          }
          break;

        default:
          return FALSE;
        }

        if(pszText == NULL) {
          MSG_ERROR("displaying null alpha should not be here!", 0, 0, 0);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
          return FALSE;
        }

        /* set the text that came in from GSTK */
        bReturnStatus = ISTATIC_SetText(pIStaticText, NULL, pszText, AEE_FONT_BOLD,
                                        AEE_FONT_BOLD);
        if (bReturnStatus != TRUE)
        {
          MSG_ERROR("Could not Set Text, returned %d", bReturnStatus, 0, 0);
        }
        /* set this dialog as active dialog */
        ISTATIC_SetActive(pIStaticText, TRUE);

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

        if(pMe->cmdHdr.nCommandID == AEESTK_DISPLAY_TEXT_REQ)
        {
#ifdef FEATURE_UIM_DEBUG_PRL_UPDATE
          /* Check if this text is for a contatenated version of a prl update, if
          ** so make sure the bImmediateRespReq flag is set to true.
          */
          pszText = ((AEESTKDisplayTextReq*)(pMe->m_pProactiveCommand))->textString.pwszText;
          if (pszText != NULL) {
            char *precvText = NULL;
            precvText = MALLOC(int32touint32(WSTRLEN(pszText)+1));
            if (precvText != NULL) {
              (void)MEMSET(precvText, 0x00, int32touint32(WSTRLEN(pszText)+1));
              (void)WSTRTOSTR(pszText, precvText, WSTRLEN(pszText)+1);
              if((STRSTR(precvText, "PRL Update")) &&
                !(STRSTR(precvText, "PRL Update Successful")))
              {
                ((AEESTKDisplayTextReq*)(pMe->m_pProactiveCommand))->bImmediateRespReq = TRUE;
              }
            }
            CATAPP_FREEIF(precvText);
          }
#endif /* FEATURE_UIM_DEBUG_PRL_UPDATE */
          if(((AEESTKDisplayTextReq*)pMe->m_pProactiveCommand)->bImmediateRespReq)
          {
            if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                    &additional_info);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
            }

            pMe->m_bDisplayingText = TRUE;
          }

        }
        else if (pMe->cmdHdr.nCommandID == AEESTK_OPEN_CH_REQ)
        {
          FREE(pszText);
          pszText = NULL;
        }
        return (TRUE);
      }
      else
      {
        return (FALSE);
      }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY:
      if(pMe->m_bDisplayingText && pMe->term_resp_sent)
      {
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
        if((ISHELL_GetActiveDialog(pMe->a.m_pIShell)) == NULL)
        {
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        }
      }

      switch(w)
      {
        case AVK_CLR:
          /* we got a clear key, so send Backward move in session as Terminal Response */
          if (pMe->term_resp_sent == FALSE ) {
            switch(pMe->cmdHdr.nCommandID)
            {

            case AEESTK_SEND_DATA_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_RECEIVE_DATA_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_RECEIVE_DATA_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_CLOSE_CH_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_CLOSE_CH_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_SEND_SMS_REQ:
              /* Backward move requested by user is not applicable to Send SMS cmd */
              break;
            default:
              SendCommandResponse(pMe,
                  AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                  &additional_info);
            }
          }
          return (TRUE);

        case AVK_END:
          /* we got the End Key so send SIM_SESSION TERMINATED BY User with TR */
          if(pMe->term_resp_sent == FALSE)
          {
            switch(pMe->cmdHdr.nCommandID)
            {

            case AEESTK_SEND_DATA_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_RECEIVE_DATA_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_RECEIVE_DATA_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_CLOSE_CH_REQ:
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                                                   additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_CLOSE_CH_FAILED, 0xFF,
                            (uint32)pCommandResp);
              break;
            case AEESTK_SEND_SMS_REQ:
              /* Backward move requested by user is not applicable to Send SMS cmd */
              break;
            default:
              SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                  &additional_info);
            }
          }
          return (TRUE);

        default:
        /* any other key, if immediate response is not required then send TR */
        if(pMe->cmdHdr.nCommandID != AEESTK_SEND_SMS_REQ)
        {
          if( pMe->term_resp_sent == FALSE)
          {
            if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                  &additional_info);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
            }
          }
        }
        return (TRUE);
      }

    case EVT_DIALOG_END:
      if(pMe->cmdHdr.nCommandID != AEESTK_SEND_SMS_REQ)
      {
        if(pMe->m_bDisplayingText)
        {
          /* this was an immediate response */
          pMe->m_bDisplayingText = FALSE;
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendClearTimerExpired, pMe);
        }
        else if ( pMe->term_resp_sent == FALSE )
        {
          SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                              &additional_info);
        }
      }
      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      return (FALSE);
  }
} /* DisplayTextEventHandler */

/*===========================================================================

FUNCTION: SpcDisplayTextEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Display Text Dialog.
    The Display Text Dialog is used in the following proactive commands:
    GSTK_SET_UP_CALL_REQ
PROTOTYPE:
    boolean SpcDisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean SpcDisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCatApp                *pMe = (CCatApp*) pUser;
  IStatic                *pIStaticText = NULL;
  AECHAR                 *pszText = NULL;
  boolean                 bReturnStatus = TRUE;
  AEESTKAdditionalInfo    additional_info;

  MSG_HIGH("SpcDisplayTextEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  switch (evt)
  {
    case EVT_DIALOG_START:

      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_DISPLAY_CLOSE_UPON_EVENT);
      if(pIStaticText)
      {
        ISTATIC_SetProperties(
          pIStaticText,
          ST_MIDDLETEXT | ST_UNDERLINE | ST_CENTERTITLE | ST_CENTERTEXT | ST_NOSCROLL);
        /* get the text that came in from gstk */
        switch(pMe->cmdHdr.nCommandID)
        {
          case AEESTK_SETUP_CALL_REQ:
            pszText = pMe->call_cnf_str;
            break;
          default:
            return FALSE;
        }
        /* set the text that came in from GSTK */
        bReturnStatus =
          ISTATIC_SetText(pIStaticText, NULL, pszText, AEE_FONT_BOLD,
                          AEE_FONT_BOLD);
        if (bReturnStatus != TRUE)
        {
          MSG_ERROR("Could not Set Text, returned %d", bReturnStatus, 0, 0);
        }
        /* set this dialog as active dialog */
        ISTATIC_SetActive(pIStaticText, TRUE);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      }
      break;
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
          if((ISHELL_GetActiveDialog(pMe->a.m_pIShell)) == NULL)
          {
            (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          }
          break;

        case AVK_END:
          /* we got the End Key so send SIM_SESSION TERMINATED BY User with TR */
          if(pMe->term_resp_sent == FALSE)
          {
              SendCommandResponse(
                pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                &additional_info);
          }
          return (TRUE);

        default:
          return (TRUE);
      }
      break;

    case EVT_DIALOG_END:
      if ( pMe->term_resp_sent == FALSE )
      {
        SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
            &additional_info);
      }
      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      break;
  }
  return (FALSE);
} /* SpcDisplayTextEventHandler */

/*===========================================================================

FUNCTION: MoreCharEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Dialog that is displayed
    when the user enters invalid number of characters
    This dialog used in the following proactive commands:
    GSTK_GET_INPUT_REQ

PROTOTYPE:
    boolean DisplayTextEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/

boolean MorecharEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
   /*lint -esym(715,w) */
{
  CCatApp * pMe = (CCatApp*) pUser;
  IStatic * pIStaticText = NULL;

  MSG_HIGH("MorecharEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    )
  {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendTimerExpired, pMe);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired,
        (uint32*) pMe);
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
      /*Display the Text */
      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw,
                                                  IDC_MORE_CHARS_REQD);
      if(pIStaticText)
      {
        ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
        ISTATIC_SetActive(pIStaticText, TRUE);
        return (TRUE);
      }
      else
      {
        return (FALSE);
      }

    case EVT_DIALOG_END:
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CloseMoreCharDialog, pMe);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SendTimerExpired, pMe);
      (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired,
          (uint32*) pMe);
      return (TRUE);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY_PRESS:
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CloseMoreCharDialog, pMe);
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      /* Need to evaluate if an event needs to be sent here
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
      FREEIF(pMe->m_DisplayTextBuffer.pwszText);
      DBGEVENT(EVT_CATAPP_NO_DISPLAY, AEECLSID_CATAPP);
#endif FEATURE_CATAPP_TEST_AUTOMATION */
      return TRUE;

    default:
      return (FALSE);
  }
}


/*===========================================================================

FUNCTION: catapp_wms_callback

DESCRIPTION
        This function is the command callback function for WMS.

PROTOTYPE:
         void catapp_wms_callback( wms_cmd_id_e_type cmd, void *user_data,
                                   wms_cmd_err_e_type cmd_err)

PARAMETERS:
        cmd: [in]:command whose status is being reported

        user_data: [in]: Event specific user data

        cmd_err: [in]: the status of the command


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void catapp_wms_callback (wms_cmd_id_e_type cmd, const void *user_data,
                          wms_cmd_err_e_type cmd_err)
   /*lint -esym(715,cmd) */
{
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL)
    return;

  MSG_LOW("In WMS message CALLBACK",0,0,0);
  if (cmd == WMS_CMD_MSG_SEND)
  {
    if(cmd_err != WMS_CMD_ERR_NONE)
    {
      /* if the message was unsuccessfully sent, post an event to CAt app to */
      /* sent a error TR to GSTK */
      (void)ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_SMS_MESSAGE_FAILED, 0,
                            0);

    }
  }
  /*if no error was recieved its okay, wait for event callback to be called */
} /*lint !e715 */


/*===========================================================================

FUNCTION: catapp_wmseventcb

DESCRIPTION
    This function is the event callback function for WMS.

PROTOTYPE:
      void catapp_wmseventcb(wms_msg_event_e_type event,
      wms_msg_event_info_s_type *pInfo, boolean *shared)

PARAMETERS:
    event: [in]:command whose status is being reported

    pInfo: [in]: WMS info buffer

    shared: [in]: ????


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void catapp_wmseventcb(wms_msg_event_e_type event, const wms_msg_event_info_s_type *pInfo,
                       const boolean *shared)
{
  IShell *shell_ptr = AEE_GetShell();
  wms_msg_event_info_s_type *sms_msg_event_info_buf;
  boolean status = TRUE;
  MSG_LOW("In WMS message event CALLBACK",0,0,0);
  if (shell_ptr == NULL)
    return;

  if (pInfo == NULL)
    return;

  switch( event )
  {
    case WMS_MSG_EVENT_SUBMIT_REPORT:
      sms_msg_event_info_buf = MALLOC(sizeof(wms_msg_event_info_s_type));
      if (sms_msg_event_info_buf == NULL) {
        MSG_ERROR("catapp_wmseventcb MALLOC failed", 0, 0, 0);
        return;
      }
      (void) MEMCPY(sms_msg_event_info_buf, pInfo,
                    sizeof(wms_msg_event_info_s_type));

      /* if the message was successfully sent, post an event to CAt app to */
      /* sent a success TR to GSTK */
      status = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_SMS_MESSAGE_SENT, 0,
                         (uint32) sms_msg_event_info_buf);
      if (!status) {
        MSG_ERROR("Post EVT_CATAPP_SMS_MESSAGE_SENT failed", 0, 0, 0);
        FREE(sms_msg_event_info_buf);
        sms_msg_event_info_buf = NULL;
      }
      break;
    default:
      MSG_MED("Unhandled event %d by cat app", event, 0, 0);
      break;
  }
} /*lint !e715 */

/*===========================================================================

FUNCTION: PackandSendSMSMessage

DESCRIPTION
    This function is invoked when Send SMS proactive command has come in. It is
    used to send an SMS message to the network

PROTOTYPE:
     void PackandSendSMSMessage (CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
  information specific to this applet. It was initialized during the
  AppCreateInstance() function.
  pIActiveDialog: [in]: Pointer to the currently active dialog.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
 The SMS message is sent to the network
===========================================================================*/
void PackandSendSMSMessage (CCatApp *pMe)
{

  AEECMSSInfo ss_info;
  AEESTKSendSMSReq *pSendSMSReq = NULL;

  if (pMe == NULL)
  {
     MSG_ERROR("Could not call PackandSendSMSMessage", 0, 0, 0);
     return;
  }
  pSendSMSReq = (AEESTKSendSMSReq*) pMe->m_pProactiveCommand;

  CATAPP_FREEIF(pMe->m_psMessageToWms);

  pMe->m_psMessageToWms = MALLOC(sizeof(wms_client_message_s_type));
  if(pMe->m_psMessageToWms != NULL) {
    (void)MEMSET(pMe->m_psMessageToWms, 0x00, sizeof(wms_client_message_s_type));

    (void)ICM_GetSSInfo(pMe->m_pICM, &ss_info, sizeof(AEECMSSInfo));

    if (((ss_info.sys_mode == AEECM_SYS_MODE_GSM) ||
        (ss_info.sys_mode == AEECM_SYS_MODE_WCDMA))
        && pSendSMSReq->bIsCDMASMS == FALSE)
    {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /*F_WCDMA || GSM */
    }
    /* CDMA_SEND_SMS and mode is CDMA */
    else if ((ss_info.sys_mode == AEECM_SYS_MODE_CDMA)
         && pSendSMSReq->bIsCDMASMS == TRUE)
    {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      if(wms_ts_cdma_OTA2cl(pSendSMSReq->smsTPDU.pTPDU,
        (uint16)pSendSMSReq->smsTPDU.nLength,
        pMe->m_psMessageToWms) != WMS_OK_S)
        {
          MSG_ERROR("Conversion from OTA to cl failed",0,0,0);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_SMS_MESSAGE_SENT, 0,
                                   0);
        }
#ifdef FEATURE_CCAT
        else if(pSendSMSReq->bPackingRequired)
        {
          /* OTA conversion to WMS format ok, pack if necessary */
          if(wms_ts_pack_cdma_user_data(
            &pMe->m_psMessageToWms->u.cdma_message.raw_ts) != WMS_OK_S)
          {
            MSG_ERROR("Packing Message failed",0,0,0);
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,
                                     AEECLSID_CATAPP,
                                     EVT_CATAPP_SMS_MESSAGE_SENT, 0, 0);
          }
        }
#endif /* FEATURE_CCAT */
#endif /*F_CDMA_800 || F_CDMA_1900 */
    }
    /* Mode and command mis-match */
    else
    {
      (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_SMS_MESSAGE_SENT, 0,
                       0);
      return;
    }


    /*  ERR_FATAL("TS encoding error",0,0,0); */
    (void) wms_msg_send(pMe->wms_id, (wms_cmd_cb_type)catapp_wms_callback, (void *) pMe,
                        WMS_SEND_MODE_CLIENT_MESSAGE, pMe->m_psMessageToWms);
  }
  else {
    /* can't allocate message memory buffer */
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_SMS_MESSAGE_SENT, 0,
                       0);
  }
}


/*===========================================================================

FUNCTION: UserInputSKEventHandler

DESCRIPTION
    This function is invoked when an event occurs in a message that required
    the user to enter Yes/No Softkey selection
    This Dialog is used in the following proactive commands:
    GSTK_GET_INKEY_REQ (Yes/No)
    GSTK_SETUP_CALL_REQ (Confirmation)

PROTOTYPE:
    boolean UserInputSKEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean UserInputSKEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AEESTKAdditionalInfo                  additional_info;
  CCatApp * pMe                         = (CCatApp*) pUser;
  CatApp_CommandRespType  *pCommandResp = NULL;

  MSG_HIGH("UserInputSKEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    )
  {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SendTimerExpired, pMe);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired,
        (uint32*) pMe);
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
      return CreateUserSKDlg(pMe);

    case EVT_COMMAND:
      switch (pMe->cmdHdr.nCommandID)
      {
      case AEESTK_GET_INKEY_REQ:
#ifdef FEATURE_CAT_REL6
        if((((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat !=
                AEESTK_YES_NO_AND_IMM_DGT_RSP))
#endif /* #ifdef FEATURE_CAT_REL6 */
        {
          switch (w) {
          case IDL_SKUSER_INPUT_YES:
            pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_YES;
            CATAPP_FREEIF(pMe->userinput);
            MSG_LOW("GOT YES INPUT FROM USER",0,0,0);
            break;

          case IDL_SKUSER_INPUT_NO:
            pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_NO;
            CATAPP_FREEIF(pMe->userinput);
            MSG_LOW("GOT NO INPUT FROM USER",0,0,0);
            break;

          default:
            MSG_ERROR("EVT_COMMAND: Unexpected IDL %d", w, 0, 0);
            break;
          }
          if (pMe->m_icon_present)
          {
            SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                &additional_info);
          }
          else
          {
            SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
          }
        }
        return (TRUE);

      case AEESTK_SETUP_CALL_REQ: /* set up call */
        /* if call setup requirement is set to something other than
         * NO_OTHER_CALLS, then reject the proactive command */
        switch (((AEESTKSetupCallReq*)(pMe->m_pProactiveCommand))->callSetupRequirement)
        {
          case AEESTK_NO_OTHER_CALLS:
            break;                /* Nothing to do in this case. */
          case AEESTK_HOLD_ACTIVE_CALLS:
            if (catapp_hold_all_calls(&additional_info) == FALSE)
            {
              SendCommandResponse(pMe,
                  AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES, &additional_info);
              return (TRUE);
            }
            break;
          case AEESTK_DISCONNECT_ACTIVE_CALLS:
            if (catapp_end_all_calls(pMe, &additional_info) == FALSE)
            {
              SendCommandResponse(pMe,
                 AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES, &additional_info);
              return (TRUE);
            }
            break;
          case AEESTK_MAX_CALL_SETUP_REQ_TYPE:
            break;
        }
        /* if the user accepted the phone call, send a TR indicating
         * SUCCESSFUL completion */
        /* this is a temporary. More implementation is needed here */
        if(w == IDL_SKUSER_INPUT_YES)
        {
          if(((AEESTKSetupCallReq*)(pMe->m_pProactiveCommand))->callSetupRequirement != AEESTK_DISCONNECT_ACTIVE_CALLS)
          {
            /* Cancel timer since we are going to steup a call*/
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendTimerExpired, pMe);
            if (catapp_cm_call_cmd_orig(pMe, &additional_info) == FALSE)
            {
              /* Call failed. */
              SendCommandResponse(pMe,
                AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                &additional_info);
            }
          }
          else
          {
            /* Wait for AEECM_ALL_CALLS_ENDED event to set up a call*/
            pMe->m_PendingCallSetUp = TRUE;
          }
        }
        else
        {
          /* if user did not accept the call, then send a TR indicating
           * that user did not accept the call */
          pMe->m_PendingCallSetUp = FALSE;
          SendCommandResponse(pMe,
              AEESTK_RESULT_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST,
              &additional_info);
        }
        return (TRUE);

      case AEESTK_OPEN_CH_REQ:
        if(w == IDL_SKUSER_INPUT_YES)
        {
          /* Open the channel */
          /* Create an open Ch dialog to indicate to user that it is opening */
          if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT))
          {
            additional_info.nLength = 1;
            additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                        additional_info);
            /* m_CurrChId is used because we get here because of during Open Ch Req */
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
              EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pMe->m_CurrChId),
              (uint32)pCommandResp);
          }
          else {
            int index = 0;

            if (CatApp_FindChSocketIndexFromChID(pMe, &index, pMe->m_CurrChId) != SUCCESS) {
              /* Able found the ID in the table */
              MSG_ERROR("Channel ID 0x%x already occupies an element in the table",
                        pMe->m_CurrChId, 0, 0);
              additional_info.nLength = 1;
              additional_info.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                        additional_info);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                    EVT_CATAPP_OPEN_CH_FAILED, 0xFF, (uint32)pCommandResp);
              return TRUE;
            }
            if (!pMe->m_sOpenChInfo[index].bOnDemandLinkEst) {
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH, int32touint16(index), 0);
            }
            else {
              MSG_HIGH("Open Channel On Demand Success ", 0, 0, 0);
              /* Clean up Socket Info */
              /* m_CurrChId is used because we get here because of during Open Ch Req */
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                    EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(index), 0);
            }
          }
          return TRUE;
        }
        else
        {
          /* if user did not accept the open ch request, then send a TR indicating
           * that user did not accept the call */
          /* Open the channel */
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST,
                                        additional_info);
          /* m_CurrChId is used because we get here because of during Open Ch Req */
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pMe->m_CurrChId),
                             (uint32)pCommandResp);
          return TRUE;
        }


      default:
        return FALSE;
      }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY:
      switch(w)
      {
      case AVK_1:
#ifdef FEATURE_CAT_REL6
        if (pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)
        {
          if((((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
                AEESTK_YES_NO_AND_IMM_DGT_RSP))

          {
            pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_YES;
            MSG_HIGH("GOT YES INPUT FROM USER",0,0,0);
            if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                  &additional_info);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
            }
          }
        }
#endif /*  #ifdef FEATURE_CAT_REL6 */
        return (TRUE);
      case AVK_2:
#ifdef FEATURE_CAT_REL6
        if (pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)
        {
          if((((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
                AEESTK_YES_NO_AND_IMM_DGT_RSP))
          {
            pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_NO;
            MSG_HIGH("GOT NO INPUT FROM USER",0,0,0);
            if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                  &additional_info);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
            }
          }
        }
#endif /* #ifdef FEATURE_CAT_REL6 */
        return (TRUE);
      /* if user presses clear key need to send TR indicating backward move,
       * if the text control is empty */
      case AVK_CLR:
        if (pMe->term_resp_sent == FALSE) {
          if (pMe->cmdHdr.nCommandID == AEESTK_OPEN_CH_REQ) {
            /* m_CurrChId is used because we get here because of during Open Ch Req */
            CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                        AEESTK_RESULT_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE,
                                        additional_info);
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pMe->m_CurrChId),
                              (uint32)pCommandResp);
          }
          else {
            SendCommandResponse(pMe,
                AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                &additional_info);
          }
        }

        return (TRUE);

      case AVK_POUND:
        /* if help information is provided by card then send a TR indicating
         * help requested */
        if(pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)
        {
          /* get inkey */
          if(!(pMe->term_resp_sent) &&
              ((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->bHelpAvaliable)
          {
            SendCommandResponse(pMe, AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER,
                &additional_info);
          }
          return (TRUE);
        }
        return TRUE;

      default:
        return TRUE;
      }

    case EVT_DIALOG_END:
      if (pMe->term_resp_sent == FALSE)
      {
        if (pMe->cmdHdr.nCommandID == AEESTK_SETUP_CALL_REQ) {
          if(!pMe->m_bSuspended) {
            return TRUE;
          }
        }
        if (pMe->cmdHdr.nCommandID == AEESTK_OPEN_CH_REQ) {
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                   AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
                                                   additional_info);
          /* m_CurrChId is used because we get here because of during Open Ch Req */
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_OPEN_CH_FAILED, int32touint16(pMe->m_CurrChId),
                              (uint32)pCommandResp);
        }
        else {
          SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
              &additional_info);
        }
      }

      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      return (FALSE);
  }
} /*lint !e715 */


/*===========================================================================

FUNCTION: GetInputEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Get Input Dialog.
    The Get Input Dialog is used in the following proactive commands:
    GSTK_GET_INPUT_REQ
    GSTK_GET_INKEY_REQ
    GSTK_SETUP_CALL_REQ

PROTOTYPE:
    boolean GetInputEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean GetInputEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
    /*lint -esym(715,dw) */
{
  AEESTKAdditionalInfo           additional_info;
  AECHAR                         NULLSTRING='\0';
  CCatApp                      * pMe = (CCatApp*) pUser;
  IDialog                      * pIActiveDialog = NULL;
  ITextCtl                     * pInputEdit = NULL;
  uint32                         nTextLen = 0;

  MSG_HIGH("GetInputEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
     )
  {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SendTimerExpired, pMe);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired,
        (uint32*) pMe);
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
      return CreateGetInputDlg(pMe);

    case EVT_KEY:
      return TRUE;
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY_PRESS:
      switch(w)
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
#ifdef FEATURE_CAT_REL6
          if (!(pMe->term_resp_sent))
          {
            if(((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
               AEESTK_IMMEDIATE_DIGIT_RSP)
            {
              pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_SINGLE_CHAR;
              pMe->userinput = MALLOC(sizeof(AECHAR));
              pMe->userinput = (AECHAR *)&w;
              if (pMe->m_icon_present)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                      &additional_info);
              }
              else
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
              }
              return (TRUE);
            }
          }
#endif /* #ifdef FEATURE_CAT_REL6 */
          return (FALSE);
        case AVK_SELECT:
          MSG_HIGH("GOT SELECT KEY",0,0,0);
#ifdef FEATURE_CAT_REL6
          if((pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ) &&
            (((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
               AEESTK_IMMEDIATE_DIGIT_RSP))
          {
              /* Do not let BREW handle this key */
              return TRUE;
          }
#endif /* #ifdef FEATURE_CAT_REL6 */
          pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

          if (!pMe->term_resp_sent) {
            if(pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ) {
              pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_SINGLE_CHAR;
              nTextLen = GET_INKEY_INPUT_SIZE;
            }
            else { /* Get Input */
              /* Get input proactive command */
              nTextLen = ((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->nMaxUserInput;
            }

            pInputEdit = (ITextCtl *) IDIALOG_GetControl(pIActiveDialog,
                                                           IDC_GET_INPUT_EDIT);

            /* Free data before malloc to ensure there is no "dangling" pointer */
            CATAPP_FREEIF(pMe->userinput);
            /* if get inkey command has come in, but user will key in input */
            pMe->userinput = MALLOC((nTextLen+1) * sizeof(AECHAR));

            (void)MEMSET(pMe->userinput, 0x00, nTextLen+1);
            if(nTextLen==0)
            {
              /* if maximum user input size is 0 */
              pMe->userinput[0] = NULLSTRING;
              if (pMe->m_icon_present)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                      &additional_info);
              }
              else
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
              }
            }
            else {
              if(ITEXTCTL_GetText(pInputEdit, pMe->userinput,
                                      (int) (nTextLen+1)))
              {
                if ((pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ) ||
                    /* if got the input from the user */
                    ((pMe->cmdHdr.nCommandID == AEESTK_GET_INPUT_REQ)
                      && (WSTRLEN(pMe->userinput) >=
                      ((AEESTKGetInputReq*)pMe->m_pProactiveCommand)->nMinUserInput)))
                {
                   /* if the input is of valid size  */
                   MSG_LOW("Sending TR COMMAND_PERFORMED_SUCCESSFULLY",0,0,0);
                  if (pMe->m_icon_present)
                  {
                    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                          &additional_info);
                  }
                  else
                  {
                    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
                  }
                }
                else
                {
                  /* if input is not valid size, tell the user */
                  (void) CreateMoreCharsReqd(pMe);
                  (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                      (PFNNOTIFY) SendTimerExpired, pMe);
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT,
                      (PFNNOTIFY) CloseMoreCharDialog, (uint32*) pMe);
                }
              } /*ITEXTCTL_GETTEXT */
              else { /* user presses select with input */
                /* check if minimum user input is 0 or it is get inkey req */
                if(((pMe->cmdHdr.nCommandID == AEESTK_GET_INPUT_REQ) &&
                    (((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->nMinUserInput == 0)) ||
                   (pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)) {
                  pMe->userinput[0] = NULLSTRING;
                  if (pMe->m_icon_present)
                  {
                    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                          &additional_info);
                  }
                  else
                  {
                    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
                  }
                }
                else {
                  /* if input is not valid size, tell the user */
                  (void) CreateMoreCharsReqd(pMe);
                  (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                    (PFNNOTIFY) SendTimerExpired, pMe);
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT,
                    (PFNNOTIFY) CloseMoreCharDialog, (uint32*) pMe);
                }
              }
            }
          } /* if term resp not sent */

          return TRUE;

        /* if user presses clear key need to send TR indicating backward move,
         * if the text control is empty */
        case AVK_CLR:
          pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          pInputEdit = (ITextCtl*)  IDIALOG_GetControl(pIActiveDialog,
              IDC_GET_INPUT_EDIT);

          if (pMe->cmdHdr.nCommandID == AEESTK_GET_INPUT_REQ) {
            /* Free data before malloc to ensure there is no "dangling" pointer */
            CATAPP_FREEIF(pMe->userinput);
            if (((AEESTKGetInputReq*)(pMe->m_pProactiveCommand))->nMaxUserInput == 0) {
              pMe->userinput = MALLOC(sizeof(AECHAR));
            }
            else {
              pMe->userinput = MALLOC(
                ((AEESTKGetInputReq*)pMe->m_pProactiveCommand)->nMaxUserInput *
                sizeof(AECHAR));
            }
          }
          else if (pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ) {
            /* Free data before malloc to ensure there is no "dangling" pointer */
            CATAPP_FREEIF(pMe->userinput);
            pMe->userinput = MALLOC(sizeof(AECHAR));
          }
          if (pMe->userinput == NULL) {
            MSG_ERROR("Can't obtain memory: pMe->userinput NULL", 0, 0, 0);
            return TRUE;
          }
          if (ITEXTCTL_GetText(pInputEdit, pMe->userinput,
                (((AEESTKGetInputReq*)pMe->m_pProactiveCommand))->nMaxUserInput))
          {
            ITEXTCTL_SetActive(pInputEdit, TRUE);
            ITEXTCTL_SetCursorPos(pInputEdit,TC_CURSOREND);
            /* check to make sure that text control does not have any user
             * input, if so then treat the clear key */
            /* as normal, otherwise send a TR */
            if ((WSTRLEN(pMe->userinput) ==0) &&
                (pMe->term_resp_sent == FALSE))
            {
              SendCommandResponse(pMe,
                  AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                  &additional_info);
            }
          }
          else
          {
            if (pMe->term_resp_sent == FALSE)
              SendCommandResponse(pMe,
                  AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER,
                  &additional_info);
          }
          return (TRUE);

        case AVK_POUND:
          if (!(pMe->term_resp_sent))
          {
            if(pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ)
            {
              /* if help information is provided by card then send a TR indicating
               * help requested */
              /* get inkey */
              if(((AEESTKGetInkeyReq*)pMe->m_pProactiveCommand)->bHelpAvaliable)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER,
                    &additional_info);
                return TRUE;
              }
#ifdef FEATURE_CAT_REL6
              else if(((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
                   AEESTK_IMMEDIATE_DIGIT_RSP)
              {
                pMe->m_InkeyRspType = AEESTK_INKEY_INPUT_IS_SINGLE_CHAR;
                pMe->userinput = MALLOC(sizeof(AECHAR));
                pMe->userinput = (AECHAR *)&w;
                if (pMe->m_icon_present)
                {
                  SendCommandResponse(pMe,
                                      AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                      &additional_info);
                  return (TRUE);
                }
                else
                {
                  SendCommandResponse(pMe,
                                      AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY,
                                      &additional_info);
                  return (TRUE);
                }
              }
#endif /* #ifdef FEATURE_CAT_REL6 */
            }
            else if (pMe->cmdHdr.nCommandID == AEESTK_GET_INPUT_REQ)
            {/* get input */
              if (((AEESTKGetInputReq*)pMe->m_pProactiveCommand)->bHelpAvaliable)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER,
                    &additional_info);
                return TRUE;
              }
            }
          }
          return FALSE;

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

        default:
#ifdef FEATURE_CAT_REL6
          if((pMe->cmdHdr.nCommandID == AEESTK_GET_INKEY_REQ) &&
              (((AEESTKGetInkeyReq*)(pMe->m_pProactiveCommand))->responseFormat ==
               AEESTK_IMMEDIATE_DIGIT_RSP))
          {
            /* Do not let BREW handle this key */
            return TRUE;
          }
#endif /* #ifdef FEATURE_CAT_REL6 */
          break;

      }
      return (FALSE);

    case EVT_COMMAND:
      return (TRUE);

    case EVT_DIALOG_END:
      if (pMe->term_resp_sent == FALSE)
      {
        SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER,
            &additional_info);
      }

      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      return (FALSE);
  }
}

/*===========================================================================

FUNCTION: SelectItemEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Select ITem Dialog.
    The Select Item Dialog is used in the following proactive commands:
    GSTK_SELECT_ITEM_REQ

PROTOTYPE:
    boolean SelectItemEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean SelectItemEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCatApp * pMe = (CCatApp*) pUser;
  IMenuCtl* pICurrentMenu = NULL;
  CtlAddItem itemfrommenu;
  AEESTKAdditionalInfo           additional_info;
  IDialog *pIActiveDialog = NULL;
  uint16   wDlgID = 0;

  MSG_HIGH("SelectItemEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  if (evt != EVT_DIALOG_END)
  {
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIActiveDialog == 0) {
      MSG_ERROR("Null Active Dlg", 0, 0, 0);
      return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);

    switch (wDlgID) {
    case IDD_SELECT_ITEM:
      pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                      IDC_SELECT_ITEM_MENU);
      break;

    case IDD_SK_SELECT_ITEM:
      pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                      IDC_SK_SELECT_ITEM_MENU);
      break;

    default:
      MSG_ERROR("Invalid Dialog ID: 0x%x", wDlgID, 0, 0);
      return FALSE;
    }

    if (pICurrentMenu == NULL) {
      MSG_ERROR("pICurrentMenu = NULL", 0, 0, 0);
      return FALSE;
    }
  }
  /* We are in the idle menu, so cancel the DisplaySMSIdleMenuDlg timer */
  if (evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
      )
  {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SendTimerExpired, pMe);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired,
        (uint32*) pMe);
  }

  switch (evt)
  {
    case EVT_DIALOG_START:
      return CreateSelectItemDlg(pMe, pIActiveDialog, wDlgID);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY:
      if (w != (uint16)AVK_SELECT)
      {
        switch(w)
        {
          case AVK_CLR:
            if(pMe->term_resp_sent == FALSE)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER, &additional_info);
            }
            return (TRUE);

          case AVK_DOWN:
          case AVK_UP:

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
          case AVK_LEFT:
          case AVK_RIGHT:
          case AVK_SEND:
          case AVK_MESSAGE:
          case AVK_MENU:
          case AVK_MUTE:
            return (TRUE);
          case AVK_POUND:
            if (pMe->term_resp_sent == FALSE &&
                ((AEESTKSelectItemReq*)pMe->m_pProactiveCommand)->bHelpAvaliable == TRUE)
            {
              if (pICurrentMenu == NULL) { // Make lint happy
                MSG_ERROR("SelectItemEventHandler: AVK_POUND: pICurrentMenu Null", 0, 0, 0);
                return FALSE;
              }
              w = IMENUCTL_GetSel(pICurrentMenu);
              (void)IMENUCTL_GetItem(pICurrentMenu, w, &itemfrommenu);
              pMe->chosenitem = (uint16) itemfrommenu.dwData;
              SendCommandResponse(pMe, AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER, &additional_info);
            }
            return (TRUE);

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

          default:
            return (TRUE);
        }
      }
      return (TRUE);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_COMMAND:
      if(!pMe->term_resp_sent)
      {
        if (pICurrentMenu == NULL) { // Make lint happy
          MSG_ERROR("EVT_COMMAND or EVT_CATAPP_DIAG_SELECT_MENU_ITEM_KEY: pICurrentMenu=NULL",
                    0,0,0);
          return FALSE;
        }
        (void)IMENUCTL_GetItem(pICurrentMenu, w, &itemfrommenu);
        pMe->chosenitem = (uint16) itemfrommenu.dwData;
        if (pMe->m_icon_present)
        {
          SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                &additional_info);
        }
        else
        {
          SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
        }

      }
      return (TRUE);

    case EVT_DIALOG_END:
      if(pMe->term_resp_sent == FALSE)
      {
        SendCommandResponse(pMe, AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER, &additional_info);
      }
      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      return (FALSE);
  }
}

/*===========================================================================

FUNCTION: CreateTextDlg

DESCRIPTION
  This function is invoked when the Get Input Dialog needs to be created.
  The Get Input Dialog is used for the following proactive Commands:
  GSTK_GET_INPUT_REQ
  GSTK_GET_INKEY_REQ


PROTOTYPE:
 static boolean CreateTextDlg(CCatApp* pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates the Get Input Dialog and makes it the currently active dialog
===========================================================================*/
static boolean CreateTextDlg(const CCatApp *pMe)
{
  int nReturnStatus;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
      IDD_GET_INPUT, NULL);

  if (nReturnStatus == EFAILED) { /* Creation failed */
    return (FALSE);
  }
  return (TRUE);

}

/*===========================================================================

FUNCTION: CreateSKInputDlg

DESCRIPTION
  This function is invoked when the Get Input Dialog needs to be created.
  The Get Input Dialog is used for the following proactive Commands:
  GSTK_SETUP_CALL_REQ
  GSTK_GET_INKEY_REQ


PROTOTYPE:
 static boolean CreateSKInputDlg(CCatApp* pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates the Get Input Dialog and makes it the currently active dialog
===========================================================================*/
static boolean CreateSKInputDlg(const CCatApp *pMe)
{
  int nReturnStatus;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
      IDD_SKUSER_INPUT, NULL);

  if (nReturnStatus == EFAILED) { /* Creation failed */
    return (FALSE);
  }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  return (TRUE);

}
#ifndef FEATURE_CATAPP_OLD_SOUND_INTERFACE
/*===========================================================================

FUNCTION: TurnoffSound

DESCRIPTION
  This function is a callback that is invoked when a play sound proactive command
  has come in and the sound has to end
  This function is used in the following proactive commands:
  GSTK_PLAY_TONE_REQ


PROTOTYPE:
 void TurnoffSound(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Stops the currently playing sound, and sends a TR to GSTK
===========================================================================*/
void TurnoffSound(CCatApp *pMe)
{
  AEESTKAdditionalInfo           additional_info;
  additional_info.nLength = 0;

  MSG_LOW("CALL BACK SOUND CALLED",0,0,0);
  uisnd_snd_stop ();

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }
  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  if(pMe->term_resp_sent == FALSE && pMe->play_tone_displayed == TRUE)
  {
    if (pMe->m_icon_present)
    {
      SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                            &additional_info);
    }
    else
    {
      SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
    }

  }
  else if(pMe->term_resp_sent == FALSE && pMe->play_tone_displayed == FALSE)
  {
    /* if tone was played but no screen was displayed  */
    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION, &additional_info);
  }
}
#endif /* FEATURE_CATAPP_OLD_SOUND_INTERFACE */
/*===========================================================================

FUNCTION: TurnoffTone

DESCRIPTION
  This function is a callback that is invoked when a play tone proactive command
  has come in and the tone has to end
  This function is used in the following proactive commands:
  GSTK_PLAY_TONE_REQ


PROTOTYPE:
 void TurnoffTone(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Stops the currently playing tone, and sends a TR to GSTK
===========================================================================*/
void TurnoffTone(CCatApp *pMe)
{
  AEESTKAdditionalInfo           additional_info;

  /* Intialize additional Info */
  (void)MEMSET(&additional_info, 0x00, sizeof(additional_info));

  MSG_LOW("CALL BACK SOUND CALLED",0,0,0);
  uisnd_tone_stop ();

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return;
  }
  if(pMe->term_resp_sent == FALSE && pMe->play_tone_displayed == TRUE)
  {
    if (pMe->m_icon_present)
    {
      SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                            &additional_info);
    }
    else
    {
      SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additional_info);
    }

  }
  else if(pMe->term_resp_sent == FALSE && pMe->play_tone_displayed == FALSE)
  {
    /* if tone was played but no screen was displayed  */
    SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION, &additional_info);
  }
}

/*===========================================================================

FUNCTION: MainMenuEventHandler

DESCRIPTION
    This function is invoked when an event occurs in the Main Cat App Dialog.

PROTOTYPE:
    boolean MainMenuEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)

PARAMETERS:
    pUser: [in]: User Data passed in

    evt: [in]: Event which has triggered

    w: [in]: Parameters associated with event evt

    dw: [in]: Any additional parameters associated with event evt

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the dialog handled the event
  FALSE: If the dialog did not handle the event

SIDE EFFECTS
  none
===========================================================================*/
boolean MainMenuEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCatApp * pMe = (CCatApp*) pUser;
  IMenuCtl* pICurrentMenu = NULL;

  CtlAddItem itemfrommenu;
  IDialog *pIActiveDialog = NULL;
  uint16   wDlgID = 0;

  MSG_HIGH("MainMenuEventHandler evt 0x%x, w 0x%x, dw 0x%x", evt, w, dw);

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  if (evt != EVT_DIALOG_END)
  {
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIActiveDialog == 0) {
      MSG_ERROR("Null Active Dlg", 0, 0, 0);
      return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIActiveDialog);

    switch (wDlgID) {
    case IDD_MAIN_CAT:
      pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                      IDC_CATMAIN);
      break;

    case IDD_SK_MAIN_CAT:
      pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                      IDC_SK_CATMAIN);
      break;

    default:
      MSG_ERROR("Invalid Dialog ID: 0x%x", wDlgID, 0, 0);
      return FALSE;
    }

    if (pICurrentMenu == NULL) {
      return FALSE;
    }
  }
  switch (evt)
  {
    case EVT_DIALOG_START:
      return CreateMainMenuDlg(pMe, pIActiveDialog, wDlgID);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_KEY:
      switch(w)
      {
        case AVK_CLR:
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return (TRUE);
        case AVK_POUND:
          if(pMe->numItemsInMenu == 0)
          {
            MSG_HIGH("MainMenuEventHandler: AVK_POUND, pMe->numItemsInMenu = 0", 0, 0, 0);
            return (TRUE);
          }
          else
          {
            if (pMe->m_bSetupMenuHelp) {
              uint16 wTransactionID;

              if (pICurrentMenu == NULL) { // Make lint happy
                MSG_ERROR("MainMenuEventHandler: AVK_POUND, pICurrentMenu Null", 0, 0, 0);
                return FALSE;
              }
              w = IMENUCTL_GetSel(pICurrentMenu);
              (void)IMENUCTL_GetItem(pICurrentMenu, w, &itemfrommenu);
              (void)ISTK_SendEnvelopeMenuSelCommand(pMe->m_pISTK, &wTransactionID,
                  (uint8) itemfrommenu.dwData, TRUE);
            }
            return (TRUE);
          }
        default:
          break;
      }
      return (TRUE);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    case EVT_COMMAND:
      if(pMe->numItemsInMenu == 0)
      {
        return (TRUE);
      }
      else
      {
        uint16 wTransactionID;

        if (pICurrentMenu == NULL) { // Make lint happy
          return FALSE;
        }
        (void)IMENUCTL_GetItem(pICurrentMenu, w, &itemfrommenu);
        (void)ISTK_SendEnvelopeMenuSelCommand(pMe->m_pISTK, &wTransactionID,
                                        (uint8) itemfrommenu.dwData, pMe->m_bSetupMenuHelp);
        return (TRUE);
      }

    case EVT_DIALOG_END:
      return (TRUE);

    default: /* Other events that have not been handled by the handler */
      return (FALSE);
  }
}

/*===========================================================================

FUNCTION: CreateCatAppDlg

DESCRIPTION
  This function is invoked when the Cat App starts up or resumes. It creates
  the main Cat App Dialog


PROTOTYPE:
 boolean CreateCatAppDlg(CCatApp *pMe)

PARAMETERS:
  pMe: [in]: Pointer to the AEEApplet structure. This structure contains
             information specific to this applet. It was initialized during
             the AppCreateInstance() function.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  Creates the Cat App Dialog and makes it the currently active dialog
===========================================================================*/
boolean CreateCatAppDlg(CCatApp *pMe)
{
  int ret_status;

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }
  if (pMe->m_bSKMainMenu)
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                            IDD_SK_MAIN_CAT, NULL);
  }
  else
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, CATAPP_RES_FILE,
                            IDD_MAIN_CAT, NULL);
  }

  if (ret_status != SUCCESS) { /* Creation failed */
    MSG_ERROR("Can't Create CatApp", 0, 0, 0);
    return FALSE;
  }
  pMe->setupmenuupdate = FALSE;
  return TRUE; /* Created successfully */

} /* CreateCatAppDlg */

#ifdef FEATURE_CATAPP_OLD_SOUND_INTERFACE
/*===========================================================================

FUNCTION: GetSound

DESCRIPTION
  This function is invoked when the play tone command comes in, and the sound
  needs to be get which is mapped to a particular tone.


PROTOTYPE:
 uisnd_snd_type GetSound(gstk_tone_enum_type tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 the tone mapped to the incoming tone on this ME.

SIDE EFFECTS
 none
===========================================================================*/
uisnd_snd_type GetSound(AEESTKToneType tone)
{
  switch(tone)

  {
    case AEESTK_DIAL_TONE:
    case AEESTK_CALLED_SUBSCRIBER_BUSY:
    case AEESTK_CONGESTION:
    case AEESTK_RADIO_PATH_ACK:
    case AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
    case AEESTK_ERROR_TONE:
    case AEESTK_CALL_WAITING_TONE:
      return UI_CALL_EAR_SND;
    case AEESTK_RINGING_TONE:
      return UI_ALERT_SND;
    case AEESTK_GENERAL_BEEP:
    case AEESTK_POSITIVE_ACK_TONE:
    case AEESTK_NEGATIVE_ACK_TONE:
      return UI_EARBEEP_SND;
    default:
      return UI_CALL_EAR_SND;
  }
}

/*===========================================================================

FUNCTION: GetTone

DESCRIPTION
  This function is invoked when the play tone command comes in, and the tone
  needs to be get which is mapped to a particular tone.


PROTOTYPE:
 snd_tone_type GetTone(gstk_tone_enum_type tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 the tone mapped to the incoming tone on this ME.

SIDE EFFECTS
 none
===========================================================================*/
snd_tone_type GetTone(AEESTKToneType tone)
{
  switch(tone)
  {
    case AEESTK_DIAL_TONE:
      return (snd_tone_type)SND_DIAL_TONE_TONE;

    case AEESTK_CALLED_SUBSCRIBER_BUSY:
      return (snd_tone_type)SND_BUSY;

    case AEESTK_CONGESTION:
      return (snd_tone_type)SND_BUSY;

    case AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
      return (snd_tone_type)SND_ERR;

    case AEESTK_ERROR_TONE:
      return (snd_tone_type)SND_ERR;

    case AEESTK_CALL_WAITING_TONE:
      return (snd_tone_type)SND_CALL_WT_TONE;

    case AEESTK_RINGING_TONE:
      return (snd_tone_type)SND_RING_A;

    case AEESTK_GENERAL_BEEP:
    case AEESTK_POSITIVE_ACK_TONE:
    case AEESTK_NEGATIVE_ACK_TONE:
    case AEESTK_RADIO_PATH_ACK:
       return (snd_tone_type)SND_2ND;
    default:
       return (snd_tone_type)SND_ERR;
  }
}
#else
/*===========================================================================
FUNCTION: IsSoundNotTone

DESCRIPTION
  This function is invoked when the play tone command comes in, and the gstk tone
  needs to be mapped to a particular tone.


PROTOTYPE:
 boolean IsSoundNotTone(AEESTKToneType tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 TRUE if gstk tone maps to an snd sound, FALSE if it maps to an snd tone.

SIDE EFFECTS
 none
===========================================================================*/
boolean IsSoundNotTone(AEESTKToneType tone)
{
  switch(tone)

  {
    case AEESTK_RINGING_TONE:
    case AEESTK_DIAL_TONE:
    case AEESTK_CALLED_SUBSCRIBER_BUSY:
    case AEESTK_CONGESTION:
    case AEESTK_RADIO_PATH_ACK:
    case AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
    case AEESTK_ERROR_TONE:
    case AEESTK_CALL_WAITING_TONE:
      return TRUE;
    case AEESTK_GENERAL_BEEP:
    case AEESTK_POSITIVE_ACK_TONE:
    case AEESTK_NEGATIVE_ACK_TONE:
      return FALSE;
    default:
      return FALSE;
  }
}

/*===========================================================================

FUNCTION: GetSoundType

DESCRIPTION
  This function is invoked when an AEESTK_PLAY_TONE_REQ event comes in, and
  the gstk tone needs to be mapped to a particular snd sound type.


PROTOTYPE:
 uisnd_snd_type GetSoundType(AEESTKToneType tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 the sound type mapped to the incoming tone on this ME.

SIDE EFFECTS
 none
===========================================================================*/
uisnd_snd_type GetSoundType(AEESTKToneType tone)
{
  switch(tone)

  {
    case AEESTK_DIAL_TONE:
    case AEESTK_CALLED_SUBSCRIBER_BUSY:
    case AEESTK_CONGESTION:
    case AEESTK_RADIO_PATH_ACK:
    case AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
    case AEESTK_ERROR_TONE:
    case AEESTK_CALL_WAITING_TONE:
      return UI_CALL_EAR_SND;
    case AEESTK_RINGING_TONE:
      return UI_ALERT_SND;
    case AEESTK_GENERAL_BEEP:
    case AEESTK_POSITIVE_ACK_TONE:
    case AEESTK_NEGATIVE_ACK_TONE:
      return UI_EARBEEP_SND;
    default:
      return UI_CALL_EAR_SND;
  }
}

/*===========================================================================

FUNCTION: GetSound

DESCRIPTION
  This function is invoked when an AEESTK_PLAY_TONE_REQ event comes in, and
  the gstk tone needs to be get which is mapped to a particular snd sound.


PROTOTYPE:
 uisnd_sound_id_type GetSound(AEESTKToneType tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 the sound mapped to the incoming tone on this ME.

SIDE EFFECTS
 none
===========================================================================*/
snd_sound_id_type GetSound(AEESTKToneType tone)
{
  switch(tone)
  {
    case AEESTK_DIAL_TONE:
#ifdef FEATURE_CEPT
      return (snd_sound_id_type)SND_CEPT_DIAL_TONE;
#else
      return (snd_sound_id_type)SND_DIAL_TONE ;
#endif

    case AEESTK_CALLED_SUBSCRIBER_BUSY:
#ifdef FEATURE_CEPT
      return (snd_sound_id_type)SND_SUBSCRIBER_BUSY;
#else
      return (snd_sound_id_type)SND_BUSY_ALERT;
#endif

    case AEESTK_CONGESTION:
#ifdef FEATURE_CEPT
      return (snd_sound_id_type)SND_CONGESTION;
#else
      return (snd_sound_id_type)SND_REORDER;
#endif

    case AEESTK_RADIO_PATH_ACK:
      return (snd_sound_id_type)SND_RADIO_PATH_ACK;

    case AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
      return (snd_sound_id_type)SND_RADIO_PATH_NOT_AVAIL;

    case AEESTK_ERROR_TONE:
      return (snd_sound_id_type)SND_ERROR_INFO;

    case AEESTK_CALL_WAITING_TONE:
#ifdef FEATURE_CEPT
      return (snd_sound_id_type)SND_CEPT_CALL_WAITING;
#else
      return (snd_sound_id_type)SND_CALL_WAITING;
#endif

    case AEESTK_RINGING_TONE:
#ifdef FEATURE_CEPT
      return (snd_sound_id_type)SND_CEPT_RING;
#else
      return (snd_sound_id_type)SND_RING_BACK;
#endif

    default:
      return (snd_sound_id_type)SND_WAKEUP;
  }
}


/*===========================================================================

FUNCTION: GetTone

DESCRIPTION
  This function is invoked when an AEESTK_PLAY_TONE_REQ event comes in, and
  the gstk tone needs to be get which is mapped to a particular snd tone.


PROTOTYPE:
 snd_tone_type GetTone(AEESTKToneType tone)

PARAMETERS:
  tone: [in]: Tone whose mapping needs to be fetched

DEPENDENCIES
  none

RETURN VALUE
 the tone mapped to the incoming tone on this ME.

SIDE EFFECTS
 none
===========================================================================*/
snd_tone_type GetTone(AEESTKToneType tone)
{
  switch(tone)
  {
    case AEESTK_GENERAL_BEEP:
    case AEESTK_POSITIVE_ACK_TONE:
    case AEESTK_NEGATIVE_ACK_TONE:
    case AEESTK_RADIO_PATH_ACK:
       return (snd_tone_type)SND_2ND;
    default:
       return (snd_tone_type)SND_ERR;
  }
}
#endif /* !FEATURE_CATAPP_OLD_SOUND_INTERFACE */

/*===========================================================================

FUNCTION CCatApp_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled
    in this function. All APPs must supply an Event Handler.

PROTOTYPE:
  static boolean CCatApp_HandleEvent(IApplet * pi, AEEEvent eCode,
                                     uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains
        information specific to this applet. It was initialized during
        the AppClsCreateInstance() function.

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
static boolean CCatApp_HandleEvent(IApplet * pi, AEEEvent eCode,
                                   uint16 wParam, uint32 dwParam)
{
  AEESTKAdditionalInfo           additionalInfo;
  int                            nReturnStatus = SUCCESS;
  uint32                         nTextLen   = 0;
  uint32                         nCopiedLen = 0;
  CCatApp                       *pMe = (CCatApp *)((void*)pi);
  IDialog                       *pIActiveDialog = NULL;
  uint16                         wDlgID = 0;
  int                            i = 0;
  int                            counter=0;
  static char displaytext[] =    " (Display Text)";
  static char getinput[] =       " (Get Input)   ";
  static char selectitem[] =     " (Select Item) ";
  static char setupcall[] =      " (Setup Call)  ";
  static char sendss[]    =      " (Send SS)     ";
  static char sendussd[]  =      " (Send USSD)   ";
  static char sendsms[]   =      " (Send SMS)    ";
  static char launchbrowser[] =  " (Launch Brwsr)";
  static char playtone[] =       " (Play Tone)   ";
  static char getinkey[] =       " (Get Inkey)   ";
  static char setupmenu[] =      " (Setup Menu)  ";
  static char setupidletext[]=   " (SetupIdleTxt)";
  static char endofproactive[] = " (EndProactive)";
  static char refresh[]        = " (Refresh)     ";
  static char moretime[] =       " (More Time)   ";
  static char pollinterval[] =   " (PollInterval)";
  static char setupeventlist[] = " (SetupEvtList)";
  static char provideloci[] =    " (Local Info)  ";
  static char runatcmd[]     =   " (Run AT Cmd)  ";
  CatApp_CommandRespType        *pCommandResp = NULL;
  int32 cnt = 0;
  AECHAR                          NextActionText[CATAPP_BUFF_SIZE];
#ifdef FEATURE_CATAPP_OLD_SOUND_INTERFACE
  int                            numtomultiply = 0;
#else /* FEATURE_CATAPP_OLD_SOUND_INTERFACE */
  int32                          duration = 0;
#endif

  (void) MEMSET(NextActionText, 0x00, CATAPP_BUFF_SIZE);

  /* Intialize additional Info */
  (void)MEMSET(&additionalInfo, 0x00, sizeof(additionalInfo));

  if(!pMe)
  {
    MSG_ERROR("pMe ERR:NULL",0,0,0);
    return FALSE;
  }

  switch ( eCode )
  {
    case EVT_APP_START_BACKGROUND:
      /* We're being launched in the background, so go to the background. */
      MSG_HIGH("CatApp:EVT_APP_START_BACKGROUND.", 0, 0, 0);
      /* Set suspended flag to TRUE in order to redraw the dialog when
         any notification is received */
      pMe->m_bSuspended = TRUE;
      return (TRUE);

    case EVT_APP_START:   /* Process Start event */
       MSG_HIGH("CatApp:EVT_APP_START.", 0, 0, 0);
       pMe->m_bSuspended = FALSE;
       pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
       if (pIActiveDialog) {
         IDIALOG_Redraw(pIActiveDialog);
       }
       else {
          (void) CreateCatAppDlg(pMe);
       }
       return (TRUE);

    case EVT_APP_RESUME:
      MSG_HIGH("CatApp:EVT_APP_RESUME", 0, 0, 0);
      pMe->m_bSuspended = FALSE;
      pIActiveDialog    = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog)
      {
        /* Close App if End Proactive Session is from card */
        if(pMe->m_EndProactiveSession == CATAPP_CARD_END_PROACTIVE_SESSION)
        {
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
        }
        /* Close all dialogs except main dialog if End Proactive Session is recv but
           not from card */
        else if(pMe->m_EndProactiveSession == CATAPP_NON_CARD_END_PROACTIVE_SESSION)
        {
          CatApp_EndNonMainDialog(pMe, TRUE);
        }
        /* No Proactive End session was received, so redraw the dialogs */
        else
        {
          IDIALOG_Redraw(pIActiveDialog);
        }
      }
      else
      {
        (void) CreateCatAppDlg(pMe);
      }
      pMe->m_EndProactiveSession = CATAPP_END_PROACTIVE_SESSION_NONE;
      return (TRUE);

    case EVT_APP_STOP:        /* process STOP event */
      MSG_HIGH("CatApp:EVT_APP_STOP", 0, 0, 0);
      pMe->m_bSuspended = TRUE;
      if (!pMe->m_bExit)
      {
        boolean *pb = (boolean *)dwParam;
        if(pb)
          *pb = FALSE;  /* Set the app to background app */
      }
      else
      {
        CCatApp_FreeAppData((IApplet*)pMe);
      }
      return (TRUE);

    case EVT_APP_SUSPEND:
      {
        AEESuspendInfo * info = (AEESuspendInfo *)dwParam;
        MSG_HIGH ("CatApp:EVT_APP_SUSPEND", 0, 0, 0);

        if(info)
        {
          info->bCloseDialogs = FALSE;
        }

        pMe->m_bSuspended = TRUE;

        return TRUE;
      }



    case EVT_DIALOG_INIT:
      if (wParam >= ARR_SIZE(CatDlgEventHandlers))
      {
        MSG_ERROR("wParam out of range", 0, 0, 0);
        return FALSE;
      }
      if(wParam < ARR_SIZE(CatDlgEventHandlers))
      { /* check for array bound */
        if (CatDlgEventHandlers[wParam] != NULL)
        {
          IDIALOG_SetEventHandler((IDialog *) dwParam,
                             CatDlgEventHandlers[wParam], pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
          return (TRUE);
        }
      }
      return (FALSE);

    case EVT_USER:
    case EVT_COMMAND:          /* Process menu command event */
      return TRUE;

    case EVT_KEY:              /* Process key event */
      switch(wParam)
      {
        case AVK_CLR:
          if (pMe->origination_sent && !pMe->term_resp_sent)
          {
            (void) catapp_cm_call_cmd_end(pMe);
            SendCommandResponse(pMe,
                AEESTK_RESULT_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE,
                &additionalInfo);
          }
          break;
        default:
          break;
      }
      return (TRUE);

    case EVT_KEY_PRESS:
      switch(wParam)
      {
        case AVK_END:
         (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          break;
        default:
          break;
      }
      return TRUE;

    case EVT_KEY_HOOK:
      MSG_HIGH("Got KEY HOOK EVENT 0x%x",wParam,0,0);
      switch ((AVKType) wParam)
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
        case AVK_POWER:
        case AVK_END:
        case AVK_SEND:
        case AVK_CLR:
        case AVK_UP:
        case AVK_DOWN:
        case AVK_VOLUME_UP:
        case AVK_VOLUME_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        case AVK_STAR:
        case AVK_POUND:
          if (pMe->m_pISTK) {
            /* Set user activity */
             MSG_HIGH("Setting User Activity",0,0,0);
            (void)ISTK_UserActivity(pMe->m_pISTK);
          }
          break;
        default:
          break;
      }
      return FALSE;

    case EVT_CAT_USER_KEY_EVENT:
      if(pMe->m_pISTK) {
        (void)ISTK_UserActivity(pMe->m_pISTK);
      }
      return TRUE;

    case EVT_CATAPP_CH_STATUS_CHANGED:
      {
        /* Channel Status Changed */
        AEESTKGetChStatusResponse  sChStatus;
        sChStatus.nChID = pMe->m_sOpenChInfo[wParam].nChID;
        switch (pMe->m_sOpenChInfo[wParam].status) {
        case CATAPP_CH_OPEN_WAIT:
        case CATAPP_CH_OPENED:
          MSG_ERROR("Channel 0x%x not dropped 0x%x, shouldn't rec'v the Ch Status Change Evt",
            wParam, pMe->m_sOpenChInfo[wParam].status, 0);
          break;
        case CATAPP_CH_CLOSED:
        case CATAPP_CH_DROPPED:
          sChStatus.bIsLinkEst = FALSE;
          sChStatus.ChStatusAdditionalInfo = AEESTK_CH_STATUS_LINK_DROPPED;
          break;
        default:
          break;
        }
        (void)ISTK_ChannelStatusChange(pMe->m_pISTK, sChStatus);
      }
      return TRUE;

    case EVT_CATAPP_DATA_AVAILABLE:
      MSG_HIGH("In EVT_CATAPP_DATA_AVAILABLE",0,0,0);
      if (!pMe->term_resp_sent && (pMe->cmdHdr.nCommandID == AEESTK_RECEIVE_DATA_REQ)) {
        /* Update the Terminal Response remaining data len */
        pMe->m_RecvDataRsp.nChDataRemainingLen += pMe->m_sOpenChInfo[wParam].nCurrRecvDataSize;
        (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_RECEIVE_DATA_SUCCESS, wParam,
                         0);
        /* as a result of replenishing the data buffer */
      }
      else {
        /* Data available processing */
        AEESTKDataAvailNotify  sDataAvail;
        int status = 0;
        MSG_HIGH("Sending Data Available Envelope",0,0,0);

        sDataAvail.nChLen = pMe->m_sOpenChInfo[wParam].nCurrRecvDataSize;
        switch (pMe->m_sOpenChInfo[wParam].status) {
        case CATAPP_CH_OPEN_WAIT:
          sDataAvail.bIsLinkEst = FALSE;
          sDataAvail.ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
          break;
        case CATAPP_CH_OPENED:
          sDataAvail.bIsLinkEst = TRUE;
          sDataAvail.ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
          break;
        case CATAPP_CH_DROPPED:
          sDataAvail.bIsLinkEst = FALSE;
          sDataAvail.ChStatusAdditionalInfo = AEESTK_CH_STATUS_LINK_DROPPED;
          break;
        default:
          break;
        }
        if(pMe->m_sOpenChInfo[wParam].status == CATAPP_CH_CLOSED)
        {
          MSG_HIGH("Did not expect to send Data Available on Closed channel",0,0,0);
          return TRUE;
        }
        sDataAvail.nChID = pMe->m_sOpenChInfo[wParam].nChID;
        status = ISTK_DataAvailable(pMe->m_pISTK, sDataAvail);
        if(status == EFAILED)
        {
          MSG_HIGH("ISTK Data Available FAILED",0,0,0);
        }
        else
        {
          /*After the card has been notified of the new data that has arrived,
            set the bWasEmpty flg to FALSE to disallow any other data available events
            till the card has completely read the receive data buffer.
          */
          pMe->m_sOpenChInfo[wParam].bWasEmpty = FALSE;
        }
      }
      return TRUE;

    case EVT_CATAPP_RECEIVE_DATA_SUCCESS:
      MSG_HIGH("In EVT_CATAPP_RECEIVE_DATA_SUCCESS",0,0,0);
      if (!pMe->term_resp_sent) {
        additionalInfo.nLength = 0;
        if ((AEESTKResult)dwParam == AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY) {
          if (pMe->m_icon_present)
          {
            dwParam = (uint32)AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED;
          }
          else
          {
            dwParam = (uint32)AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY;
          }
        }
        SendCommandResponse(pMe, (AEESTKResult)dwParam, &additionalInfo);
      }
      return TRUE;

    case EVT_CATAPP_SEND_DATA_SUCCESS:
    case EVT_CATAPP_CLOSE_CH_SUCCESS:
      MSG_HIGH("In EVT_CATAPP_CLOSE_CH_SUCCESS / EVT_CATAPP_SEND_DATA_SUCCESS 0x%x",eCode,0,0);
      if (!pMe->term_resp_sent) {
        additionalInfo.nLength = 0;
        if (pMe->m_icon_present)
        {
          SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                &additionalInfo);
        }
        else
        {
          SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
        }
      }
      return TRUE;

    case EVT_CATAPP_RECEIVE_DATA:
      {
        CatApp_OpenChInfo *pOpenChInfo = (CatApp_OpenChInfo*)dwParam;
        MSG_HIGH("In EVT_CATAPP_RECEIVE_DATA",0,0,0);
        /* require PostEvent to pass in correct index wParam value */
        if (pOpenChInfo == NULL) {
            /* Invalid Index and pointer */
            MSG_ERROR("Invalid Index 0x%x", wParam, 0, 0);
            return FALSE;
        }
        if (pOpenChInfo->status != CATAPP_CH_OPENED) {
          MSG_ERROR("Socket is not opened anymore", 0, 0, 0);
          pOpenChInfo->bMoreDataToRecv = FALSE;
          MSG_HIGH("bMoreDataToRecv Flg is set to FALSE 0x%x",
                    pOpenChInfo->bMoreDataToRecv,0,0);
          return FALSE;
        }
        /* find the index, receive the data */
        nReturnStatus = CatApp_ReceiveData(pMe, pOpenChInfo->nIndex);
        if (nReturnStatus != SUCCESS) {
          MSG_ERROR("Socket Receive Data Failed, 0x%x", nReturnStatus, 0, 0);
        }
      }
      return TRUE;

    case EVT_CATAPP_SEND_DATA:
    {
        CatApp_OpenChInfo *pOpenChInfo = (CatApp_OpenChInfo*)dwParam;
        MSG_HIGH("CatApp_SendData request on Ch Id 0x%x",pOpenChInfo->nChID,0,0);
        if (pOpenChInfo == NULL) {
          /* Invalid Index and pointer */
          MSG_ERROR("Invalid Index 0x%x", wParam, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);

          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                            EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                            (uint32)pCommandResp);
          return TRUE;

        }
        if (pOpenChInfo->status != CATAPP_CH_OPENED) {
          MSG_ERROR("Socket is not opened anymore", 0, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additionalInfo);

          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_SEND_DATA_FAILED, int32touint16(pOpenChInfo->nIndex),
                              (uint32)pCommandResp);
          return TRUE;
        }
        /* find the index, send the data */
        nReturnStatus = CatApp_SendData(pMe, pOpenChInfo->nIndex);
        if (nReturnStatus != SUCCESS) {
          MSG_HIGH("CatApp_SendData FAILED",0,0,0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);

          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_SEND_DATA_FAILED, int32touint16(pOpenChInfo->nIndex),
                              (uint32)pCommandResp);
        }
      }
      return TRUE;

    case EVT_CATAPP_CLOSE_CH:
      MSG_HIGH("In EVT_CATAPP_CLOSE_CH",0,0,0);
      if (wParam < CATAPP_MAX_GPRS_CH) {
        switch (pMe->m_sOpenChInfo[wParam].status) {
        case CATAPP_CH_OPEN_WAIT:
        case CATAPP_CH_OPENED:
          /* find the index, send the data */
          MSG_HIGH("Closing channel that has status CATAPP_CH_OPENED",0,0,0);
          CatApp_ReleaseSocket(pMe,wParam);
          /* InetMgr instance will be released once pdp context has been deactivated
             successfully */
          /* Evt Close ch success will be posted after receiving PDP context deactivated confirmation */
          break;
        case CATAPP_CH_DROPPED:
          MSG_HIGH("Closing channel that has status CATAPP_CH_DROPPED",0,0,0);
          CatApp_ReleaseSocket(pMe,wParam);
          CatApp_ReleaseNetMgrInst(pMe,wParam);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_CLOSE_CH_SUCCESS, wParam,
                                   0);
          break;
        case CATAPP_CH_CLOSED:
          /* Send Close Channel terminal response immediately if channel is already closed */
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_CLOSE_CH_SUCCESS, wParam,
                                   0);
          break;
        default:
          MSG_HIGH("status default : No channel to close",0,0,0);
          break;
        }
      }
      else {
        MSG_ERROR("Invalid Index 0x%x", wParam, 0, 0);
      }
      return TRUE;

    case EVT_CATAPP_CLOSE_CH_FAILED:
    case EVT_CATAPP_GET_CH_STATUS_RESULT:
    case EVT_CATAPP_RECEIVE_DATA_FAILED:
    case EVT_CATAPP_SEND_DATA_FAILED:
      {
      MSG_HIGH("In EVT_CATAPP_FAILED",0,0,0);
      pCommandResp = (CatApp_CommandRespType*)dwParam;
      if (!pMe->term_resp_sent) {
        if (pCommandResp == NULL) {
          MSG_ERROR("pCommandResp result info is NULL", 0, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
        }
        else {
          switch((AEESTKResult)pCommandResp->Result) {
          case AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER:
          case AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER:
            if (eCode == EVT_CATAPP_SEND_DATA_FAILED) {
              int index =0;
              AEESTKSendDataReq *pSendDataReq = (AEESTKSendDataReq*) pMe->m_pProactiveCommand;

              if (CatApp_FindChSocketIndexFromChID(pMe, &index, pSendDataReq->nChID) != SUCCESS) {
                /* ch not present */
                MSG_ERROR("Channel ID 0x%x not presented in the table",
                          pSendDataReq->nChID, 0, 0);
                return TRUE;
              }
              ISOCKET_Cancel(pMe->m_sOpenChInfo[index].pISocket,
                            CatApp_SendDataRetryCb,
                            (void*)&pMe->m_sOpenChInfo[index]);
              ISOCKET_Cancel(pMe->m_sOpenChInfo[index].pISocket,
                            CatApp_ReceiveDataRetryCb,
                            (void*)&pMe->m_sOpenChInfo[index]);
            }
            break;
          default:
            break;
          }
          SendCommandResponse(pMe, (AEESTKResult)pCommandResp->Result, &pCommandResp->AdditionalInfo);
        }
      }
      CATAPP_FREEIF(pCommandResp);
      return TRUE;
      }
    case EVT_CATAPP_OPEN_CH_FAILED:
      {
        MSG_HIGH("In EVT_CATAPP_OPEN_CH_FAILED",0,0,0);
        pCommandResp = (CatApp_CommandRespType*)dwParam;
        if(pMe->cmdHdr.nCommandID == AEESTK_OPEN_CH_REQ) {
          if (wParam < CATAPP_MAX_GPRS_CH) {
            CatApp_ReleaseSocket(pMe,wParam);
            CatApp_ReleaseNetMgrInst(pMe,wParam);
            CatApp_CleanupSocketInfo(pMe, wParam, TRUE);
          }
          else {
            /* No valid Index was found, no need to perform any table cleanup */
          }
        }

        if (!pMe->term_resp_sent) {
          if (pCommandResp == NULL) {
            MSG_ERROR("pCommandResp result info is NULL", 0, 0, 0);
            additionalInfo.nLength = 1;
            additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
          }
          else {
            SendCommandResponse(pMe, pCommandResp->Result, &pCommandResp->AdditionalInfo);
          }
          CATAPP_FREEIF(pCommandResp);
        }
        return TRUE;
      }

    case EVT_CATAPP_PROCESS_RECEIVE_DATA_CMD:
      {
        int index = 0;
        AEESTKReceiveDataReq *pReceiveDataReq = (AEESTKReceiveDataReq*) pMe->m_pProactiveCommand;
        MSG_HIGH("In EVT_CATAPP_PROCESS_RECEIVE_DATA_CMD",0,0,0);
        if (wParam >= CATAPP_MAX_GPRS_CH) {
          nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pReceiveDataReq->nChID);
          if (nReturnStatus != SUCCESS) {
            /* ch already occupied */
            /* no more room */
            MSG_ERROR("Can't find any matching ch 0x%x", pReceiveDataReq->nChID, 0, 0);
            additionalInfo.nLength = 1;
            additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
            CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                 AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                 additionalInfo);
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_RECEIVE_DATA_FAILED, 0xFF,
                              (uint32)pCommandResp);
            return TRUE;
          }
        }
        else {
          index = (int)wParam;
        }
        if (pMe->m_sOpenChInfo[index].status != CATAPP_CH_OPENED) {
          MSG_HIGH("Channel not open anymore",0,0,0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additionalInfo);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_RECEIVE_DATA_FAILED, int32touint16(index),
                              (uint32)pCommandResp);
        }

        /* Get the data */
        nReturnStatus = CatApp_CopyReceiveDataBuf(pMe, index);
        if (nReturnStatus != SUCCESS) {
          /* ch already occupied */
          /* no more room */
          MSG_ERROR("CatApp_CopyReceiveDataBuf FAILED Can't find any matching ch 0x%x",
                     pReceiveDataReq->nChID, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_RECEIVE_DATA_FAILED, int32touint16(index),
                              (uint32)pCommandResp);
          return TRUE;
        }
        /* Success is sent in CopyReceiveDataBuf if
           1) there are more data in the recv buff after copying to the AEESTKReceiveDataResponse
           2) there is no more data and no pending data in the recv buff after copying to the
              AEESTKReceiveDataResponse
          Else, Success is sent after CatApp_ReceiveData sends the EVT_DATA_AVABILABLE event.
          In the EVT_DATA_AVAILABLE event, a Data Avaliable envelope will be sent only if
          there is no pending terminal response of RECEIVE_DATA and the SIM has requested the
          data available info. Else, a SUCCESS for the receive data proactive command will be sent */
      }
      return TRUE;

    case EVT_CATAPP_OPEN_CH_SUCCESS:
      {
        int index = (int)wParam;
        MSG_HIGH("In EVT_CATAPP_OPEN_CH_SUCCESS",0,0,0);

        if (index >= CATAPP_MAX_GPRS_CH)
        {
          MSG_ERROR("Invalid Index 0x%x", index, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_FAILED, 0xFF,
                             (uint32)pCommandResp);
          return TRUE;
        }

        /* Bind the socket to a port
           Limitation: Currently this always binds a card sent port to a socket
           There is no dynamic binding of a socket to a port
           May need to change it later
        */
        nReturnStatus = CatApp_BindSocket(pMe->m_sOpenChInfo[index]);

        /* register for net event */
        pMe->m_sOpenChInfo[index].nParentAddress = (int32)pMe;
        pMe->m_sOpenChInfo[index].nSendDataBufSize = 0;
        pMe->m_sOpenChInfo[index].nCurrRecvDataSize = 0;
        if(Catapp_RegisterForNetEvent(pMe, &pCommandResp, index) != SUCCESS)
        {
          return TRUE;
        }

        /* Register to receive events when data is available in a socket */
        if(pMe->m_sOpenChInfo[index].pISocket != NULL)
        {
          ISOCKET_Readable(pMe->m_sOpenChInfo[index].pISocket,
                           (PFNNOTIFY)CatApp_ReceiveDataRetryCb,
                           &pMe->m_sOpenChInfo[index]);
        }

        switch(pMe->cmdHdr.nCommandID)
        {
        case AEESTK_OPEN_CH_REQ:
        {
          if (!pMe->term_resp_sent)
          {
            if(pMe->m_sOpenChInfo[index].bOpen_ch_param_modified == TRUE)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION,
                                    &additionalInfo);
            }
            else if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                    &additionalInfo);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
            }
          }
          return TRUE;
        }
        case AEESTK_SEND_DATA_REQ:
        {
          AEESTKSendDataReq *pSendDataReq = (AEESTKSendDataReq*) pMe->m_pProactiveCommand;
          nReturnStatus = CatApp_FindChSocketIndexFromChID(pMe, &index, pSendDataReq->nChID);
          if (nReturnStatus != SUCCESS)
          {
            /* ch already occupied */
            /* no more room */
            MSG_ERROR("Can't find any matching ch 0x%x", pSendDataReq->nChID, 0, 0);
            additionalInfo.nLength = 1;
            additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
            CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                additionalInfo);
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                              (uint32)pCommandResp);
            return TRUE;
          }
          /* Start sending the data */
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
            EVT_CATAPP_SEND_DATA, int32touint16(index), (uint32)(&pMe->m_sOpenChInfo[index]));

          return TRUE;
        }
        default:
          MSG_ERROR("OPEN_CH_SUCCESS in other event 0x%x", pMe->cmdHdr.nCommandID, 0, 0);
          return TRUE;
        }
      }

    case EVT_CATAPP_OPEN_CH:
      {
        /* Open CH event expected a valid wParam for index */
        int index = (int)wParam;
        if (index >= CATAPP_MAX_GPRS_CH) {
          MSG_ERROR("Invalid Index 0x%x", index, 0, 0);
          additionalInfo.nLength = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_FAILED, 0xFF,
                             (uint32)pCommandResp);
          return TRUE;
        }
        MSG_HIGH("Channel[0x%x] status is 0x%x",index,pMe->m_sOpenChInfo[index].status,0);
        if((pMe->m_sOpenChInfo[index].pINetMgr == NULL) &&
           (pMe->m_sOpenChInfo[index].status != CATAPP_CH_OPENED))
        {
          /* INet */
          nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_NET,
                                                (void**)(&pMe->m_sOpenChInfo[index].pINetMgr));
          if (nReturnStatus != SUCCESS)
          {
            MSG_ERROR("Could not Create Net Manager %d", nReturnStatus, 0, 0);
            additionalInfo.nLength = 1;
            additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                 AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                                 additionalInfo);
            (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                     EVT_CATAPP_OPEN_CH_FAILED, int32touint16(index),
                                     (uint32)pCommandResp);
          }
        }

        /* find the index, check to see if channel needs to be opened immediately or not */
        if ((pMe->m_sOpenChInfo[index].pISocket == NULL) &&
            (pMe->m_sOpenChInfo[index].status != CATAPP_CH_OPENED))
        {
            /* Has not opened the socket yet, e.g.,
               1) during first OpenCh non OnDemand Open
               2) during first SendData with OnDemand Open Ch request previously */
            nReturnStatus = CatApp_OpenSocket(pMe, index, (int*)&pMe->m_sOpenChInfo[index].nChID);
            if (nReturnStatus != SUCCESS) {
              MSG_HIGH("CatApp_OpenSocket failed ",0,0,0);
              additionalInfo.nLength = 1;
              if(pMe->cmdHdr.nCommandID == AEESTK_SEND_DATA_REQ)
              {
                additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
              }
              else
              {
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
              }
              CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                               EVT_CATAPP_OPEN_CH_FAILED, int32touint16(index),
                               (uint32)pCommandResp);
            }
        }
        else {
          /* done */
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(index), 0);
          return TRUE;
        }
      }
      return TRUE;


    case EVT_CATAPP_SMS_MESSAGE_FAILED:
      CATAPP_FREEIF(pMe->m_psMessageToWms);
      if(pMe->term_resp_sent == FALSE)
      {
        /*if we got an error while processing this command, send a TR to GSTK */
        SendCommandResponse(pMe, AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                            &additionalInfo);
      }
      return TRUE;

    case EVT_CATAPP_SMS_MESSAGE_SENT:
      CATAPP_FREEIF(pMe->m_psMessageToWms);

      if(dwParam == 0) {
        /*if we got an error while processing this command, send a TR to GSTK */
        SendCommandResponse(pMe, AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
      }
      else {
        /* First check if this status is due to a message sent by cat app.
        */
        if(((wms_msg_event_info_s_type*)dwParam)->submit_report_info.client_id == (wms_client_type_e_type)pMe->wms_id)
        {
          switch(((wms_msg_event_info_s_type*)dwParam)->submit_report_info.report_status)
          {
          case WMS_RPT_OK:
             /*SMS message has been sent, this event recieved from wms event callback */
              if (pMe->m_icon_present)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                    &additionalInfo);
              }
              else
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
              }
              break;
          case WMS_RPT_MO_CONTROL_DISALLOW:
              /* Call control did not allow mo sms send */
              additionalInfo.nLength = 1;
              additionalInfo.additionalInfo[0] = AEESTK_ACTION_NOT_ALLOWED;
              SendCommandResponse(pMe, AEESTK_RESULT_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM,
                                  &additionalInfo);
              break;
          case WMS_RPT_MO_CONTROL_ERROR:
              /* Call control error */
              additionalInfo.nLength = 1;
              additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
              SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
              break;
          default:
              /*SMS message has been sent, this event recieved from wms event callback */
              SendCommandResponse(pMe, AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                  &additionalInfo);
              break;
          }
        }
        FREE((void*)dwParam);
        dwParam = 0;
      }
      return (TRUE);

    case EVT_NOTIFY:
    {

      AEENotify *pNotifyInfo = (AEENotify*) dwParam;
      IShell *pIShell;

      if (pNotifyInfo == NULL)
      {
        return FALSE;
      }
#ifdef FEATURE_UIONE_HDK
      if (((AEENotify *) dwParam)->cls == AEECLSID_NATIVENOTIFIER)
      {
        if(pNotifyInfo->dwMask == NOTIFY_MASK_NATIVE_IDLE_EVENT) //MASK == PROACTIVE_CMD
        {
          pMe->m_bIdleScreenDisplayed = TRUE;
          /* Sending idle screen envelope */
          (void)ISTK_IdleScreen(pMe->m_pISTK);
        }
        else if(pNotifyInfo->dwMask == NOTIFY_MASK_NATIVE_NONIDLE_EVENT)
        {
          pMe->m_bIdleScreenDisplayed = FALSE;
        }
      }
      else if (pNotifyInfo->cls == AEECLSID_STKNOTIFIER)
#else /* FEATURE_UIONE_HDK */
      if (pNotifyInfo->cls == AEECLSID_STKNOTIFIER)
#endif /* FEATURE_UIONE_HDK */
      {
        if(pNotifyInfo->dwMask == NMASK_STKNOTIFIER_PROACTIVE_CMD) //MASK == PROACTIVE_CMD
        {
          AEESTKNotifierData* pNotifyData = (AEESTKNotifierData*) (pNotifyInfo->pData);
          AEESTKProactiveCmdType commandID = (AEESTKProactiveCmdType) (pNotifyData->dwTransactionID & 0xFFFF);

          if (pNotifyData == NULL)
          {
            MSG_ERROR("Null ISTK Notify Data", 0, 0, 0);
            return FALSE;
          }

          CATAPP_FREEIF(pMe->m_pProactiveCommand);
          if(!pNotifyData->dwCmdDataSize)
          {
            MSG_ERROR("returned size 0", 0, 0, 0);
            return FALSE;
          }
          else
          {
            pMe->m_pProactiveCommand = MALLOC(pNotifyData->dwCmdDataSize);
            if(!pMe->m_pProactiveCommand)
            {
             MSG_ERROR("Malloc Failed", 0, 0, 0);
             return FALSE;
            }
            (void)ISTK_GetCmdData(pMe->m_pISTK, pNotifyData->dwTransactionID, pMe->m_pProactiveCommand, &pNotifyData->dwCmdDataSize);
          }
          /* Initilize the proactive end session flag to NONE */
          pMe->m_EndProactiveSession = CATAPP_END_PROACTIVE_SESSION_NONE;
          switch(commandID)//Command ID
          {

          case AEESTK_SETUP_CALL_REQ:
            {
              AEESTKSetupCallReq *pSetupCallReq = (AEESTKSetupCallReq*) pMe->m_pProactiveCommand;
              int                 nLen = 0;
              pMe->m_PendingCallSetUp = FALSE;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for SETUP Call is null", 0, 0, 0);
                return TRUE;
              }

              pMe->cmdHdr = pSetupCallReq->cmdHdr;

              /* Save call confirmation text */
              if ((pSetupCallReq->setupCallDisplayAlpha.nLength > 0) &&
                  (pSetupCallReq->setupCallDisplayAlpha.pwszText != NULL))
              {
                (void)MEMSET(pMe->call_cnf_str, 0x00, sizeof(pMe->call_cnf_str));
                 nLen = uint32toint32(pSetupCallReq->setupCallDisplayAlpha.nLength) + 1;
                 if (nLen > AEECM_MAX_DIAL_STRING+1)
                 {
                   nLen = AEECM_MAX_DIAL_STRING+1;
                 }
                 (void)WSTRLCPY(pMe->call_cnf_str,pSetupCallReq->setupCallDisplayAlpha.pwszText,
                                int32touint32(nLen));
              }
              if((pSetupCallReq->userCnfIcon.bPresent) ||( pSetupCallReq->setupCallDisplayIcon.bPresent ))
              {
                pMe->m_icon_present = TRUE;
              }
              if (!catapp_access_conditions_satified(pMe,
                   pSetupCallReq->callSetupRequirement,
                   &additionalInfo))
              {
                pMe->term_resp_sent = FALSE;

                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                    &additionalInfo);
              }
              else
              {
                if((pSetupCallReq->userCnfAlpha.pwszText == NULL) ||
                   (WSTRLEN(pSetupCallReq->userCnfAlpha.pwszText) == 0))
                {
                  pMe->term_resp_sent = FALSE;

                  /* No user confirmation is necessary, start the orig processing */
                  return(catapp_cm_call_cmd_orig(pMe, &additionalInfo));
                }
                /*Create Setup call confirmation dialog */
                else
                {
                  if (pMe->m_bSuspended)
                  {
                    (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                  }

                  CatApp_EndNonMainDialog(pMe, FALSE);
                  pMe->term_resp_sent = FALSE;

                  if(!CreateSKInputDlg(pMe))
                  {
                    additionalInfo.nLength=1;
                    additionalInfo.additionalInfo[0]= AEESTK_SCREEN_IS_BUSY;
                    SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                        &additionalInfo);
                  }
                }
              }
            }
            return (TRUE);


          case AEESTK_GET_INKEY_REQ:
            {
              AEESTKGetInkeyReq *pGetInkeyReq = (AEESTKGetInkeyReq*) pMe->m_pProactiveCommand;
              int32 nTimeInMsec = 0;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for get inkey is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pGetInkeyReq->cmdHdr;
              if (pMe->m_bSuspended)
              {
                (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
              }
              /*Get Inkey proactive command */
              CatApp_EndNonMainDialog(pMe, FALSE);
              pMe->term_resp_sent = FALSE;
              if(pGetInkeyReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              if (pGetInkeyReq->responseFormat == AEESTK_YES_NO
#ifdef FEATURE_CAT_REL6
                  ||
                  pGetInkeyReq->responseFormat == AEESTK_YES_NO_AND_IMM_DGT_RSP
#endif /* #ifdef FEATURE_CAT_REL6 */
              )
              {
                if (!CreateSKInputDlg(pMe))
                {
                  SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                      &additionalInfo);
                }
              }
              else
              {
                if (!CreateTextDlg(pMe))
                {
                  SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                      &additionalInfo);
                }
              }
#ifdef FEATURE_CAT_REL6
              /* If Variable Timeout object is specified by the card,
                 set timeout to be value specified by the card ELSE
                 Variable timeout is at the discretion of the ME
              */
              if(pGetInkeyReq->variableTimeout.bPresent)
              {
                /* Set the start time to calculate the total exection time later */
                pMe->m_StartofExecTime.bPresent = TRUE;
                pMe->m_StartofExecTime.nLength  = uint32touint8(GETUPTIMEMS() / NUM_OF_MILLISECONDS_IN_SECOND);
                pMe->m_StartofExecTime.nUnits   = AEESTK_DURATION_SECONDS;
                if(catapp_ConvertToMsec(
                      pGetInkeyReq->variableTimeout,
                      &nTimeInMsec) == EFAILED)
                {
                  nTimeInMsec = USER_TIMEOUT;
                }
              }
              else
#endif /* #ifdef FEATURE_CAT_REL6 */
              {
                nTimeInMsec = USER_TIMEOUT;
              }
              (void)ISHELL_SetTimer(pMe->a.m_pIShell, nTimeInMsec,
                    (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
            }
            return (TRUE);

          case AEESTK_GET_INPUT_REQ:
            {
              AEESTKGetInputReq *pGetInputReq = (AEESTKGetInputReq*) pMe->m_pProactiveCommand;
              pMe->m_GetInputRspFormat        = pGetInputReq->responseFormat;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for get input is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pGetInputReq->cmdHdr;
              if (pMe->m_bSuspended)
              {
                (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
              }
              /*Get Input proactive command */
              CatApp_EndNonMainDialog(pMe, FALSE);
              pMe->term_resp_sent = FALSE;
              if(pGetInputReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              if (!CreateTextDlg(pMe))
              {
                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                    &additionalInfo);

              }
              (void)ISHELL_SetTimer(pMe->a.m_pIShell, USER_TIMEOUT,
                                    (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
            }
            return TRUE;

          case AEESTK_PLAY_TONE_REQ:
            {
#ifndef FEATURE_CATAPP_OLD_SOUND_INTERFACE
              boolean bPlaySoundNotTone;
#endif
              AEESTKPlayToneReq *pPlayToneReq = (AEESTKPlayToneReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL)
              {
                MSG_ERROR("Proactive command buffer for play tone is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pPlayToneReq->cmdHdr;

              /*Play Tone proactive command has come in */
              pMe->play_tone_displayed = TRUE;
              /* pMe->term_resp_sent = FALSE; */
              if(pPlayToneReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              if((pPlayToneReq->alpha.pwszText != NULL) &&
                 (WSTRLEN(pPlayToneReq->alpha.pwszText)> 0))
              {/* whether to display the screen or not */
                if (pMe->m_bSuspended)
                {
                  (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }
                CatApp_EndNonMainDialog(pMe, FALSE);
                pMe->term_resp_sent = FALSE;
                if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT))
                {
                  pMe->play_tone_displayed = FALSE;
                }
              }
              else
              { /* no alpha */
                pMe->term_resp_sent = FALSE;
              }
#ifdef FEATURE_CATAPP_OLD_SOUND_INTERFACE
              uisnd_tone(
                GetSound(pPlayToneReq->nTone),
                GetTone(pPlayToneReq->nTone), 0);
              if (pPlayToneReq->duration.bPresent)
              {
                switch (pPlayToneReq->duration.nUnits)
                {
                  case AEESTK_DURATION_SECONDS:
                    numtomultiply = NUM_OF_MILLISECONDS_IN_SECOND;
                    break;
                  case AEESTK_DURATION_MINUTES:
                    numtomultiply = NUM_OF_MILLISECONDS_IN_MINUTE;
                    break;
                  case AEESTK_DURATION_TENTHS_OF_SECONDS:
                    numtomultiply = NUM_OF_MILLISECONDS_IN_TENTH_SECOND;
                    break;
                  default:
                    numtomultiply = NUMBER_OF_MILLISECONDS_IN_ONE_MILLI_SECOND;
                }
                (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                    pPlayToneReq->duration.nLength * numtomultiply,
                    (PFNNOTIFY) TurnoffTone,
                    (uint32*) pMe);
              }
              else
              { /* no timer specified so set it to default LENGTH */
                (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                  DEFAULT_TONE_TIMEOUT,
                  (PFNNOTIFY) TurnoffTone, (uint32*) pMe);
              }
#else
              bPlaySoundNotTone = IsSoundNotTone( pPlayToneReq->nTone);
              if (bPlaySoundNotTone)
              {
                uisnd_snd(GetSoundType(pPlayToneReq->nTone),
                          GetSound(pPlayToneReq->nTone));
              }
              else
              {
                uisnd_tone(GetSoundType(pPlayToneReq->nTone),
                           GetTone(pPlayToneReq->nTone), 0);
              }
              if (pPlayToneReq->duration.bPresent)
              {
                if(catapp_ConvertToMsec(pPlayToneReq->duration,&duration) == EFAILED)
                {
                  duration = pPlayToneReq->duration.nLength *
                             NUMBER_OF_MILLISECONDS_IN_ONE_MILLI_SECOND;
                }
                if (bPlaySoundNotTone)
                {
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                                        duration,
                                        (PFNNOTIFY) TurnoffSound,
                                        (uint32*) pMe);
                }
                else
                {
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                                        duration,
                                        (PFNNOTIFY) TurnoffTone,
                                        (uint32*) pMe);
                }
              }
              else
              {
                /* no timer specified so set it to default LENGTH */
                if (bPlaySoundNotTone)
                {
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                                        DEFAULT_TONE_TIMEOUT,
                                        (PFNNOTIFY) TurnoffSound,
                                        (uint32*) pMe);
                }
                else
                {
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                                        DEFAULT_TONE_TIMEOUT,
                                        (PFNNOTIFY) TurnoffTone, (uint32*) pMe);
                }
              }
#endif /* !FEATURE_CATAPP_OLD_SOUND_INTERFACE */
            }
            return (TRUE);

          case AEESTK_SELECT_ITEM_REQ:
            {
              AEESTKSelectItemReq *pSelectItemReq = (AEESTKSelectItemReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for select item is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pSelectItemReq->cmdHdr;

              if (pMe->m_bSuspended) {
                (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
              }
              CatApp_EndNonMainDialog(pMe, FALSE);
              pMe->term_resp_sent = FALSE;
              if(pSelectItemReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              /* check for num of items */
              if(pSelectItemReq->nNumItems > MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES,
                                  &additionalInfo);
              }
              else
              {
                (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                    USER_TIMEOUT, (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
                if(!CreateMenuDlg(pMe))
                {
                  SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
                }
              }
            }
            return (TRUE);

          case AEESTK_SEND_SMS_REQ:
            {
              AEESTKSendSMSReq *pSendSMSReq = (AEESTKSendSMSReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for send sms is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pSendSMSReq->cmdHdr;
              if(pSendSMSReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              PackandSendSMSMessage(pMe);
              if((pSendSMSReq->alpha.pwszText != NULL) &&
                 (WSTRLEN(pSendSMSReq->alpha.pwszText)> 0 ))
              {
                if (pMe->m_bSuspended)
                {
                 (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }
                CatApp_EndNonMainDialog(pMe, FALSE);
                pMe->term_resp_sent = FALSE;
                if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT))
                {
                  MSG_ERROR("Unable to create display text dlg", 0, 0, 0);
                }
              }
              else
              {
                pMe->term_resp_sent = FALSE;
              }
            }
            return (TRUE);

          case AEESTK_SETUP_MENU_REQ:
            {
              AEESTKSetupMenuReq *pSetupMenuReq = (AEESTKSetupMenuReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for setup menu is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pSetupMenuReq->cmdHdr;
              /*Setup menu proactive command has come in */
              pMe->term_resp_sent = FALSE;
              pMe->setupmenuupdate = TRUE;

              /* free the memory since we are going to receive new menu! */
              CATAPP_FREEIF(pMe->menuitemid);

              for(i = 0; i< MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU; i++) {
                CATAPP_FREEIF(pMe->StaticMenu[i]);
              }
              CATAPP_FREEIF(pMe->setupmenutitle);

              /* initialize to FALSE */
              pMe->m_bSKMainMenu = FALSE;
              if(pSetupMenuReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              /* Check if first item text is null, if so, remove the menu! */
              if(pSetupMenuReq->items[0].pwszText == NULL) {
                pMe->numItemsInMenu = 0;
                pMe->setupmenutitlepresent = FALSE;
                pMe->default_setupmenu_item = 0xFF;
                pMe->m_bSetupMenuHelp = FALSE;
              }
              else
              { /* new menu! */

                pMe->numItemsInMenu = pSetupMenuReq->nItems;
                if (pMe->numItemsInMenu > MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU)
                {
                  pMe->numItemsInMenu = 0;
                  /*Send Terminal Response indicating successful completion of Proactive command */
                  SendCommandResponse(pMe,
                                      AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES,
                                      &additionalInfo);
                  return (TRUE);
                }
                pMe->default_setupmenu_item = pSetupMenuReq->nDefaultItem;

                /* setup if the help is available or not */
                pMe->m_bSetupMenuHelp = pSetupMenuReq->bHelp;

                /* copy the items to pMe structure */
                pMe->menuitemid = MALLOC(pMe->numItemsInMenu * sizeof(int));

                for (i=0; i<pSetupMenuReq->nItems; i++)
                {
                  if((pSetupMenuReq->nextActionIndicator.nNumItems>0) &&
                    (i < pSetupMenuReq->nextActionIndicator.nNumItems))
                  {
                    /* if next action identifier is provided append that to the setup menu item */
                    nTextLen = int32touint32(WSTRLEN(pSetupMenuReq->items[i].pwszText) +
                                             LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG);
                    /* copy the item string    */

                    pMe->StaticMenu[counter] = MALLOC((nTextLen) * sizeof(AECHAR));
                    if (pMe->StaticMenu[counter] == NULL)
                    {
                      MSG_ERROR("pMe->StaticMenu[counter] ERR:NULL",0,0,0);
                      return (FALSE);
                    }
                    (void) MEMSET(pMe->StaticMenu[counter], 0x00, (nTextLen) * sizeof(AECHAR));

                    nCopiedLen = WSTRLCPY(pMe->StaticMenu[counter], pSetupMenuReq->items[i].pwszText,
                                          nTextLen);
                    if(nCopiedLen != (nTextLen - LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG))
                    {
                      MSG_HIGH("Incorrect Length for setup menu %d",
                               (nTextLen - LENGTH_OF_NEXT_ACTION_IDENTIFIER_TAG),0, 0);
                    }
                    switch (pSetupMenuReq->nextActionIndicator.pNextActionList[counter])
                    {
                    case AEESTK_CMD_STK_DISPLAY_TEXT:
                      (void) STRTOWSTR(displaytext, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_GET_INPUT:
                      (void) STRTOWSTR(getinput, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SELECT_ITEM:
                      (void) STRTOWSTR(selectitem, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_REFRESH:
                      (void) STRTOWSTR(refresh, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_MORE_TIME:
                      (void) STRTOWSTR(moretime, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_POLL_INTERVAL:
                      (void) STRTOWSTR(pollinterval, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SET_UP_EVENT_LIST:
                      (void) STRTOWSTR(setupeventlist, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SET_UP_CALL:
                      (void) STRTOWSTR(setupcall, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SEND_SS:
                      (void) STRTOWSTR(sendss, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_USSD:
                      (void) STRTOWSTR(sendussd, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SEND_SHORT_MESSAGE:
                      (void) STRTOWSTR(sendsms, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_LAUNCH_BROWSER:
                      (void) STRTOWSTR(launchbrowser, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_PLAY_TONE:
                      (void) STRTOWSTR(playtone, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_GET_INKEY:
                      (void) STRTOWSTR(getinkey, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SET_UP_MENU:
                      (void) STRTOWSTR(setupmenu, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_PROVIDE_LOCAL_INFO:
                      (void) STRTOWSTR(provideloci, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
                      (void) STRTOWSTR(setupidletext, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_RUN_AT_COMMAND:
                      (void) STRTOWSTR(runatcmd, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    case AEESTK_CMD_STK_END_OF_PROACTIVE_SES:
                      (void) STRTOWSTR(endofproactive, NextActionText, CATAPP_BUFF_SIZE);
                      break;

                    default:
                      MSG_ERROR("Unexpected next step %d",
                          pSetupMenuReq->nextActionIndicator.pNextActionList[counter],
                          0, 0);
                      break;
                    }
                    nCopiedLen = WSTRLCAT(pMe->StaticMenu[counter], NextActionText,
                                          nTextLen);
                     if(nCopiedLen != (nTextLen - 1))
                     {
                       MSG_HIGH("Incorrect Length for setup menu and Next Action Text %d",
                                (nTextLen - 1),0, 0);
                     }
                  }
                  else
                  {
                    /* no next item identifier */
                    if(pSetupMenuReq->items[i].pwszText!= NULL)
                    {
                      nTextLen = int32touint32(WSTRLEN(pSetupMenuReq->items[i].pwszText));
                      /* copy the item string    */
                      pMe->StaticMenu[counter] = MALLOC((nTextLen + 1) * sizeof(AECHAR));
                      if (pMe->StaticMenu[counter] != NULL)
                      {
                        (void) MEMSET(pMe->StaticMenu[counter], 0x00, (nTextLen+1)*sizeof(AECHAR));
                        nCopiedLen = WSTRLCPY(pMe->StaticMenu[counter],
                                              pSetupMenuReq->items[i].pwszText,
                                              (nTextLen + 1));
                        if(nCopiedLen != nTextLen)
                        {
                          MSG_HIGH("Incorrect Length for NO setup menu next item %d",
                                    nTextLen,0, 0);
                        }
                      }
                    }
                  }

                  /* copy the item id */
                  pMe->menuitemid[counter] =
                  pSetupMenuReq->items[i].nLength;
                  counter ++;
                }
                nTextLen = 0;
                if(pSetupMenuReq->title.pwszText != NULL)
                {/* if title is provided, copy that to pMe structure */
                  nTextLen = int32touint32(WSTRLEN(pSetupMenuReq->title.pwszText));
                  pMe->setupmenutitle = MALLOC( (nTextLen +1 )* sizeof(AECHAR));
                  if(pMe->setupmenutitle == NULL)
                  {
                    MSG_ERROR("pMe->setupmenutitle ERR:NULL",0,0,0);
                    return FALSE;
                  }
                  pMe->setupmenutitlepresent = TRUE;
                  nCopiedLen = WSTRLCPY(pMe->setupmenutitle,
                                pSetupMenuReq->title.pwszText,(nTextLen +1));
                  if(nCopiedLen != nTextLen)
                  {
                    MSG_HIGH("Incorrect Length for setup menu title %d",
                              nTextLen,0, 0);
                  }
                }
                pMe->m_bSKMainMenu = pSetupMenuReq->bSoftkeySelection;
              } /* there is new menu! */

              /*Send Terminal Response indicating successful completion of Proactive command */
              if (pMe->m_icon_present)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                      &additionalInfo);
              }
              else
              {
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
              }
              return (TRUE);
            }

          case AEESTK_MORE_TIME_REQ:
            {
              AEESTKMoreTimeReq *pMoreTimeReq = (AEESTKMoreTimeReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for more time is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pMoreTimeReq->cmdHdr;

              (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SendTimerExpired, pMe);
              (void)ISHELL_SetTimer(pMe->a.m_pIShell, USER_TIMEOUT,
                  (PFNNOTIFY) SendTimerExpired, (uint32*) pMe);
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
            }
            return (TRUE);

          case AEESTK_UPDATE_IDLE_TEXT_REQ:
            {
              AEESTKSetupIdleModeTextReq *pIdleTextReq = (AEESTKSetupIdleModeTextReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for idle text is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pIdleTextReq->cmdHdr;
              pMe->term_resp_sent = FALSE;
              if(pIdleTextReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              nTextLen = 0;
#ifndef FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED
#ifdef FEATURE_UIONE_HDK
              if(CatApp_Send_URI_To_App(
                   pMe,
                   AEESTK_UPDATE_IDLE_TEXT_REQ,
                   pIdleTextReq->string.pwszText) != SUCCESS)
#else /* FEATURE_UIONE_HDK */
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              if (!SetIdleMenuText(pIdleTextReq->string.pwszText))
#endif /* FEATURE_UIONE_HDK */
              {
                /* Set up idle mode text failed */
                SendCommandResponse(
                  pMe,
                  AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
                return (TRUE);
              }

              if (pMe->m_icon_present)
              {
                SendCommandResponse(
                  pMe,
                  AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                  &additionalInfo);
              }
              else
              {
                SendCommandResponse(
                  pMe,
                  AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY,
                  &additionalInfo);
              }
#else
              SendCommandResponse(
                pMe,
                AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                &additionalInfo);
#endif /* FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED */
            }
            return (TRUE);

          case AEESTK_DISPLAY_TEXT_REQ:
            {
              AEESTKDisplayTextReq *pDisplayTextReq = (AEESTKDisplayTextReq*) pMe->m_pProactiveCommand;
#ifdef FEATURE_CAT_REL6
              int32 nTimeInMsec = 0;
#endif /* #ifdef FEATURE_CAT_REL6 */
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for display text is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pDisplayTextReq->cmdHdr;
              if (!pDisplayTextReq->bHighPriority) {
#ifdef FEATURE_UIONE_HDK
                if((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CATAPP) &&
                   (pMe->m_bIdleScreenDisplayed == FALSE))
#else /* #ifdef FEATURE_UIONE_HDK */
                if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CATAPP) &&
                  (ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP))
#endif /* FEATURE_UIONE_HDK */
                {
                  pMe->term_resp_sent = FALSE;
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] =  AEESTK_SCREEN_IS_BUSY;
                  SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
                  return TRUE;
                }
              }
              if (pMe->m_bSuspended)
              {
                (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
              }

              CatApp_EndNonMainDialog(pMe, FALSE);
              pMe->term_resp_sent = FALSE;
              if(pDisplayTextReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
#ifdef FEATURE_CAT_REL6
              /* If Duration object is specified by the card,
                 set min duration of time ELSE
                 Duration of display is at the discretion of the ME
              */
              if(pDisplayTextReq->bDurationOfDisplay.bPresent)
              {
                if(catapp_ConvertToMsec(
                     pDisplayTextReq->bDurationOfDisplay,
                     &nTimeInMsec) == EFAILED)
                {
                  nTimeInMsec = USER_TIMEOUT;
                }
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, nTimeInMsec,
                                      (PFNNOTIFY) SendClearTimerExpired,
                                      (uint32*) pMe);
              }
              else
#endif /* FEATURE_CAT_REL6 */
              {
                if(!pDisplayTextReq->bUserControl)
                {
                  /* Duration of display is at the discretion of the ME */
                  (void)ISHELL_SetTimer(pMe->a.m_pIShell,  USER_TIMEOUT,
                                        (PFNNOTIFY) SendClearTimerExpired,
                                        (uint32*) pMe);
                }
              }

              if(! CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT) && pMe->term_resp_sent == FALSE)
              {
                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, &additionalInfo);
              }
            }
            return TRUE;
          case AEESTK_PROVIDE_BATTERY_STATUS_REQ:
#if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK)
            {
              IBatt *pIBatt = NULL;
              AEEChargerStatus charger_state;
              int result;
              AEESTKProvideBatteryStatusReq *pProvideBattStatusReq =
                (AEESTKProvideBatteryStatusReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL)
              {
                MSG_ERROR("Proactive command buffer for provide batt status is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr= pProvideBattStatusReq->cmdHdr;

              pMe->term_resp_sent = FALSE;

              result = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_BATT,
                                  (void **)&pIBatt);
              if(result != SUCCESS)
              {
                MSG_ERROR("Could not get instance of IBatt", 0, 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                    &additionalInfo);
                return TRUE;
              }
              charger_state = IBATT_GetChargerStatus(pIBatt);
              pMe->m_BatteryStatusRsp = CatApp_processChargerState(pIBatt,charger_state);

              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY,
                                  &additionalInfo);
              (void)IBATT_Release(pIBatt);
            }
            return TRUE;
#else
            return FALSE;
#endif /* #if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK) */


          case AEESTK_END_PROACTIVE_CMD_SES:
            {
              AEESTKEndProactiveCmdReq *pEndReq = (AEESTKEndProactiveCmdReq*)
                pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL)
              {
                MSG_ERROR("Proactive command buffer for end proactive is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pEndReq->cmdHdr;
              if(pMe->m_bSuspended)
              {
                if(pEndReq->bEndFromCard)
                {
                  pMe->m_EndProactiveSession = CATAPP_CARD_END_PROACTIVE_SESSION;
                }
                else
                {
                  pMe->m_EndProactiveSession = CATAPP_NON_CARD_END_PROACTIVE_SESSION;
                }
              }
              if (!pMe->m_bSuspended && !pMe->m_bDisplayingText)
              {
                if(pEndReq->bEndFromCard)
                {
                  (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                }
                else
                {
                  CatApp_EndNonMainDialog(pMe, TRUE);
                }
              }
            }
            return TRUE;

          case AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR:
            /* clear out the setup menu info */
            /* free the memory! */
            CATAPP_FREEIF(pMe->menuitemid);

            for(i = 0; i< MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU; i++) {
              CATAPP_FREEIF(pMe->StaticMenu[i]);
            }
            CATAPP_FREEIF(pMe->setupmenutitle);

            /* Check if first item text is null, if so, remove the menu! */
            pMe->numItemsInMenu = 0;
            pMe->setupmenutitlepresent = FALSE;

            if (!pMe->m_bSuspended && !pMe->m_bDisplayingText) {
              (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            }
            return TRUE;

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)*/

            case AEESTK_PROVIDE_LANG_INFO_REQ:
              {
                AEESTKProvideLangReq *pProvideLangReq = (AEESTKProvideLangReq*) pMe->m_pProactiveCommand;
                if (pMe->m_pProactiveCommand == NULL) {
                  MSG_ERROR("Proactive command buffer for provide lang is null", 0, 0, 0);
                  return TRUE;
                }
                pMe->cmdHdr= pProvideLangReq->cmdHdr;

                pMe->term_resp_sent = FALSE;

                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
              }
              break;
            case AEESTK_LANG_NOTIFICATION_REQ:
            {
              AEESTKLangInfoReq *pLangInfoReq = (AEESTKLangInfoReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for lang notificaton is null", 0, 0, 0);
                return TRUE;
              }

              pMe->cmdHdr= pLangInfoReq->cmdHdr;

              if (pLangInfoReq->bSpecLangRequest)
              {
                (void)MEMCPY(pMe->m_lang, pLangInfoReq->nLangCode, AEESTK_LANG_CODE_LEN);
              }
              else
              {
                (void)MEMCPY(pMe->m_lang, catapp_default_lang, AEESTK_LANG_CODE_LEN);
              }
              MSG_HIGH("Current SAT Lang %c, %c", pMe->m_lang[0], pMe->m_lang[1], 0);
              pMe->term_resp_sent = FALSE;

              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
            }
            return (TRUE); /* error */

            case AEESTK_SEND_DTMF_REQ:
            {
              AEESTKSendDTMFReq *pSendDTMFReq = (AEESTKSendDTMFReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for send dtmf is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pSendDTMFReq->cmdHdr;

              pMe->term_resp_sent = FALSE;
              if(pSendDTMFReq->icon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
              if((pSendDTMFReq->alpha.pwszText != NULL) &&
                 WSTRLEN(pSendDTMFReq->alpha.pwszText) > 0)
              {/* whether to display the screen or not */
#ifdef FEATURE_UIONE_HDK
                if(CatApp_Send_URI_To_App(
                     pMe,
                     AEESTK_SEND_DTMF_REQ,
                     pSendDTMFReq->alpha.pwszText) != SUCCESS)
#else /* FEATURE_UIONE_HDK */
#ifndef FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
                if(DisplayString(pSendDTMFReq->alpha.pwszText) != SUCCESS)
#endif /* FEATURE_IDLE_MENU_TEXT_AND_DISPLAY_STRING_NOT_SUPPORTED */
#endif /* FEATURE_UIONE_HDK */
                {
                  MSG_HIGH("Not able to display DTMF Alpha  ", 0, 0, 0);
                  additionalInfo.nLength           = 1;
                  additionalInfo.additionalInfo[0] =
                    AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  SendCommandResponse(
                    pMe,
                    AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                    &additionalInfo);
                  return TRUE;
                }
              }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              if (pSendDTMFReq->nLength > 0)
              {
                CatApp_ProcessDTMF(pMe);
              }
              else
              {
                MSG_HIGH("DTMF Length = 0 ", 0, 0, 0);
                if (pMe->m_icon_present)
                {
                  SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                        &additionalInfo);
                }
                else
                {
                  SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
                }
              }
            }
            return TRUE;

            case AEESTK_OPEN_CH_REQ:
            {
              AEESTKOpenChReq *pOpenChReq =  (AEESTKOpenChReq*) pMe->m_pProactiveCommand;
              int              index      = 0;
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
              int             status_iconfig_getitem = 0;
              IConfig         *p_Config              = NULL;
              boolean         cs_ps_parallel         = FALSE;
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */
              MSG_HIGH("In case AEESTK_OPEN_CH_REQ",0,0,0);
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for open ch is null", 0, 0, 0);
                return TRUE;
              }

              pMe->cmdHdr = pOpenChReq->cmdHdr;

              if(pOpenChReq->sIcon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
              if (ISHELL_CreateInstance (pMe->a.m_pIShell, AEECLSID_CONFIG,
                                          (void **) &p_Config) != SUCCESS)
              {
                MSG_ERROR("Could not get instance of IConfig", 0, 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                    &additionalInfo);
                return TRUE;
              }
              status_iconfig_getitem = ICONFIG_GetItem(p_Config,CFGI_TOOLKIT_CS_PS_PARALLEL,
                                                        &cs_ps_parallel,sizeof(cs_ps_parallel));
              if(p_Config)
                (void)ICONFIG_Release(p_Config);
              if (status_iconfig_getitem!= SUCCESS)
              {
                MSG_ERROR("Error retrieving cs ps parallel nv item, status_iconfig_getitem = 0x%x",
                           status_iconfig_getitem,0,0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                      &additionalInfo);
                return TRUE;
              }
              if(!cs_ps_parallel)
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */
              {
                MSG_HIGH("cs ps parallel is set to 0",0,0,0);
                if (AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM))
                {
                  MSG_HIGH("On going call during AEESTK_OPEN_CH_REQ",0,0,0);
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
                  SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                      &additionalInfo);
                  return TRUE;
                }
              }
              /* Check if it is CS */
              if (!CatApp_IsOpenGPRSChReq(pMe)) {
                pMe->term_resp_sent = FALSE;
                /* do not support CSD */
                MSG_HIGH("Do Not support CSD Open Ch", 0, 0, 0);
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES,
                                               additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH_FAILED, 0xFF, (uint32)pCommandResp);
                return TRUE;
              }
              /* Check if SIM/ME interface transport level is required */
              if (!CatApp_IsMETransportRequire(pMe)) {
                pMe->term_resp_sent = FALSE;
                /* do not support SIM as transport level */
                MSG_HIGH("Do Not support SIM handles transport Open Ch", 0, 0, 0);
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES,
                                               additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH_FAILED, 0xFF, (uint32)pCommandResp);
                return TRUE;
              }
              else {
                /* find next available slot in the table */
                if (CatApp_FindChSocketIndex(pMe, &index, CATAPP_CH_CLOSED) != SUCCESS)
                {
                  if((CatApp_FindChSocketIndex(pMe, &index, CATAPP_CH_DROPPED) != SUCCESS))
                  {
                    pMe->term_resp_sent = FALSE;
                    /* no more room */
                    MSG_ERROR("No more Room for Open Ch", 0, 0, 0);
                    additionalInfo.nLength = 1;
                    additionalInfo.additionalInfo[0] = AEESTK_CH_UNAVAILABLE;
                    CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additionalInfo);
                    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                         EVT_CATAPP_OPEN_CH_FAILED, 0xFF, (uint32)pCommandResp);
                    return TRUE;
                  }
                }
                if (CatApp_PopulateOpenChTable(pMe, index) != SUCCESS) {
                  MSG_ERROR("CatApp_PopulateOpenChTable failed ", 0, 0, 0);
                  pMe->term_resp_sent = FALSE;
                  /* Clean up Socket Info */
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH_FAILED, int32touint16(index), (uint32)pCommandResp);
                  return TRUE;
                }
              }

              if((pOpenChReq->sAlpha.pwszText != NULL) &&
                WSTRLEN(pOpenChReq->sAlpha.pwszText) > 0)
              {

                if (pMe->m_bSuspended)
                {
                  (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }

                CatApp_EndNonMainDialog(pMe, FALSE);

                pMe->term_resp_sent = FALSE;

                if(! CreateSKInputDlg(pMe) && pMe->term_resp_sent == FALSE)
                {
                  MSG_HIGH("Open Channel Confirmation Not able to display ", 0, 0, 0);
                  /* Clean up Socket Info */
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH_FAILED, int32touint16(index), (uint32)pCommandResp);
                  return TRUE;
                }
              }
              else {
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
                pMe->term_resp_sent = FALSE;
                /* no user confirmation */
                if (!pMe->m_sOpenChInfo[index].bOnDemandLinkEst) {
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                      EVT_CATAPP_OPEN_CH, int32touint16(index), 0);
                }
                else {
                  MSG_HIGH("Open Channel On Demand Success ", 0, 0, 0);
                  /* Clean up Socket Info */
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_OPEN_CH_SUCCESS, int32touint16(index), 0);
                }
              }
            }
            return TRUE;

          case AEESTK_SEND_DATA_REQ:
            {
              AEESTKSendDataReq *pSendDataReq = (AEESTKSendDataReq*) pMe->m_pProactiveCommand;
              uint8 *pNewSendBuf = NULL;
              int                index;
              MSG_HIGH("In AEESTK_SEND_DATA_REQ",0,0,0);
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for send data is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pSendDataReq->cmdHdr;

              if(pSendDataReq->sIcon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }

              /* Check if the ch is not already opened */
              if (CatApp_FindChSocketIndexFromChID(pMe, &index, pSendDataReq->nChID) != SUCCESS) {
                  pMe->term_resp_sent = FALSE;
                  /* ch not present */
                  MSG_ERROR("Channel ID 0x%x not present in the table",
                            pSendDataReq->nChID, 0, 0);
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                       AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                       additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                           EVT_CATAPP_SEND_DATA_FAILED, 0xFF, (uint32)pCommandResp);
                  return TRUE;
              }
              if(pMe->m_sOpenChInfo[index].status == CATAPP_CH_DROPPED)
              {
                pMe->term_resp_sent = FALSE;
                /* ch not present */
                MSG_ERROR("Channel ID 0x%x closed/dropped in the table",
                           pSendDataReq->nChID, 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                               additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                         EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                                        (uint32)pCommandResp);
                CatApp_ReleaseSocket(pMe,index);
                CatApp_ReleaseNetMgrInst(pMe,index);
                return TRUE;
              }
              if(pMe->m_sOpenChInfo[index].status == CATAPP_CH_CLOSED)
              {
                pMe->term_resp_sent = FALSE;
                /* ch not present */
                MSG_ERROR("Channel ID 0x%x closed",pSendDataReq->nChID, 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                     AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                     additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                         EVT_CATAPP_SEND_DATA_FAILED, 0xFF, (uint32)pCommandResp);
                return TRUE;
              }
              if((pSendDataReq->sAlpha.pwszText != NULL) &&
                WSTRLEN(pSendDataReq->sAlpha.pwszText) > 0)
              {
                if (pMe->m_bSuspended)
                {
                  (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }

                CatApp_EndNonMainDialog(pMe, FALSE);

                pMe->term_resp_sent = FALSE;
                if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT) && pMe->term_resp_sent == FALSE)
                {
                  MSG_HIGH("Send Channel alpha not able to display ", 0, 0, 0);
                  /* Clean up Socket Info */
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_SEND_DATA_FAILED, int32touint16(index), (uint32)pCommandResp);
                  return TRUE;
                }
              }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              pMe->term_resp_sent = FALSE;
              /* Check if existing send buffer data size + the newly requested size is bigger than the
                 Tx buffer size */
              if ((pMe->m_sOpenChInfo[index].nSendDataBufSize + pSendDataReq->sChData.nLength)
                   > pMe->m_sOpenChInfo[index].nBufferSize )
              {
                /* can't proceed with this command now due to limitation in size */
                MSG_ERROR("Can't sent due to Tx 0x%x size limitation", pMe->m_sOpenChInfo[index].nBufferSize, 0, 0);
                MSG_ERROR("Req sent size: 0x%x, existing size: 0x%x",
                  pSendDataReq->sChData.nLength, pMe->m_sOpenChInfo[index].nSendDataBufSize, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_BUFFER_SIZE_UNAVAILABLE;
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                              AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                              additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                      EVT_CATAPP_SEND_DATA_FAILED, 0xFF, (uint32)pCommandResp);
                return TRUE;
              }
              /* Copy data into sent buffer */
              pNewSendBuf = MALLOC(int32touint32(pMe->m_sOpenChInfo[index].nSendDataBufSize + pSendDataReq->sChData.nLength));
              if (pNewSendBuf == NULL) {
                /* Send data failed */
                /* memory allocat err */
                MSG_ERROR("Malloc of 0x%x Send Buf failed",
                  (pSendDataReq->sChData.nLength+pMe->m_sOpenChInfo[index].nSendDataBufSize), 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                              AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                              additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                      EVT_CATAPP_SEND_DATA_FAILED, 0xFF, (uint32)pCommandResp);
                return TRUE;
              }
              if (pMe->m_sOpenChInfo[index].pSendBuffer != NULL) {
                (void)MEMCPY(pNewSendBuf, pMe->m_sOpenChInfo[index].pSendBuffer,
                    int32touint32(pMe->m_sOpenChInfo[index].nSendDataBufSize));
              }
              /* Copy the new data */
              (void)MEMCPY(&pNewSendBuf[pMe->m_sOpenChInfo[index].nSendDataBufSize], pSendDataReq->sChData.pData,
                int32touint32(pSendDataReq->sChData.nLength));
              pMe->m_sOpenChInfo[index].nSendDataBufSize += pSendDataReq->sChData.nLength;
              CATAPP_FREEIF(pMe->m_sOpenChInfo[index].pSendBuffer);
              pMe->m_sOpenChInfo[index].pSendBuffer = pNewSendBuf;

              /* Start sending the data */
              if (pSendDataReq->bSendDataImmediately) {
                if ((pMe->m_sOpenChInfo[index].status == CATAPP_CH_OPEN_WAIT) &&
                    (pMe->m_sOpenChInfo[index].bOnDemandLinkEst)) {
                  /* Need to start socket first since the channel has not been opened and
                    it was a OnDemandLinkeEstablish request */
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                        EVT_CATAPP_OPEN_CH, int32touint16(index), 0);
                }
                else {
                  /* Channel already opened due to eitehr
                    1) Open Channel not On Demand request
                    2) The second and subsequent Send Data Request for a On Demand Open Ch request */
                  /* Require to send everything now */
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                    EVT_CATAPP_SEND_DATA, int32touint16(index), (uint32)&pMe->m_sOpenChInfo[index]);
                }
              }
              else {
                /* "Send" successful */
                /* The actual send will be performed when a "Send immediate" is received */
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                  EVT_CATAPP_SEND_DATA_SUCCESS, int32touint16(index), 0);
              }
            }
            return TRUE;

          case AEESTK_RECEIVE_DATA_REQ:
            {
              AEESTKReceiveDataReq *pReceiveDataReq = (AEESTKReceiveDataReq*) pMe->m_pProactiveCommand;
              int                   index           = 0;
              MSG_HIGH("In AEESTK_RECEIVE_DATA_REQ",0,0,0);
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for receive data is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pReceiveDataReq->cmdHdr;

              if(pReceiveDataReq->sIcon.bPresent)
              {
                pMe->m_icon_present = TRUE;
              }

              /* Check if the ch is not already opened */
              if (CatApp_FindChSocketIndexFromChID(pMe, &index, pReceiveDataReq->nChID) != SUCCESS) {
                  pMe->term_resp_sent = FALSE;
                  /* ch not present */
                  MSG_ERROR("Channel ID 0x%x not presented in the table",
                            pReceiveDataReq->nChID, 0, 0);
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                              additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_RECEIVE_DATA_FAILED, 0xFF, (uint32)pCommandResp);
                  return TRUE;
              }
              if((pReceiveDataReq->sAlpha.pwszText != NULL) &&
                WSTRLEN(pReceiveDataReq->sAlpha.pwszText) > 0)
              {
                if (pMe->m_bSuspended)
                {
                  (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }

                CatApp_EndNonMainDialog(pMe, FALSE);
                pMe->term_resp_sent = FALSE;

                if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT) && pMe->term_resp_sent == FALSE)
                {
                  MSG_HIGH("Receive Channel alpha not able to display ", 0, 0, 0);
                  /* Clean up Socket Info */
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                              additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_RECEIVE_DATA_FAILED, int32touint16(index), (uint32)pCommandResp);
                  return TRUE;
                }
              }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              pMe->term_resp_sent = FALSE;
              /* Start receiving data */
              (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                    EVT_CATAPP_PROCESS_RECEIVE_DATA_CMD, int32touint16(index),
                                    0);
            }
            return TRUE;

          case AEESTK_GET_CH_STATUS_REQ:
            {
              AEESTKGetChStatusReq *pGetChStatusReq = (AEESTKGetChStatusReq*) pMe->m_pProactiveCommand;
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for get ch status is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pGetChStatusReq->cmdHdr;

              pMe->term_resp_sent = FALSE;
              /* Check if there are any channels that are open or dropped */
              cnt = CatApp_GetChannelStatus(pMe,FALSE,NULL);
              if(cnt)
              {
                additionalInfo.nLength = 0;
                SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY,
                                    &additionalInfo);
              }
              else
              {
                MSG_ERROR("Cnt of Channel Status can never be zero!!!",0,0,0);
              }
            }
            return TRUE;

          case AEESTK_CLOSE_CH_REQ:
            {
              AEESTKCloseChReq *pCloseChReq = (AEESTKCloseChReq*) pMe->m_pProactiveCommand;
              int               index;
              MSG_HIGH("In AEESTK_CLOSE_CH_REQ",0,0,0);
              if (pMe->m_pProactiveCommand == NULL) {
                MSG_ERROR("Proactive command buffer for close ch is null", 0, 0, 0);
                return TRUE;
              }
              pMe->cmdHdr = pCloseChReq->cmdHdr;

              /* Check if the ch is not already opened */
              if (CatApp_FindChSocketIndexFromChID(pMe, &index, pCloseChReq->nChID) != SUCCESS) {
                  pMe->term_resp_sent = FALSE;
                  /* ch not present */
                  MSG_ERROR("CatApp_FindChSocketIndexFromChID failed ,ChID 0x%x not present in the table",
                            pCloseChReq->nChID, 0, 0);
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_CH_ID_INVALID;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                      AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                      additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_CLOSE_CH_FAILED, 0xFF, (uint32)pCommandResp);
                  return TRUE;
              }
              if((pCloseChReq->sAlpha.pwszText != NULL) &&
                WSTRLEN(pCloseChReq->sAlpha.pwszText) > 0)
              {
                if (pMe->m_bSuspended)
                {
                  (void)ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP, "Start Cat App");
                }

                CatApp_EndNonMainDialog(pMe, FALSE);
                pMe->term_resp_sent = FALSE;
                if(!CreateDisplayTextDlg(pMe, IDD_DISPLAY_TEXT) && pMe->term_resp_sent == FALSE)
                {
                  MSG_HIGH("Close Channel alpha not able to display ", 0, 0, 0);
                  /* Clean up Socket Info */
                  additionalInfo.nLength = 1;
                  additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                  CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                      AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                                        additionalInfo);
                  (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_CLOSE_CH_FAILED, int32touint16(index), (uint32)pCommandResp);
                  return TRUE;
                }
              }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              pMe->term_resp_sent = FALSE;
              switch (pMe->m_sOpenChInfo[index].status) {
              /* According to 11.14 spec, Dropped, Opened Channels must return success
                 when Card attempts to close them. Already closed channels should return an error
              */
              case CATAPP_CH_DROPPED:
              case CATAPP_CH_OPEN_WAIT:
              case CATAPP_CH_OPENED:
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                       EVT_CATAPP_CLOSE_CH, int32touint16(index), 0);
                break;
              case CATAPP_CH_CLOSED:
                /* Channel not opened! */
                MSG_ERROR("Posting EVT_CATAPP_CLOSE_CH_FAILED:Socket is closed already", 0, 0, 0);
                additionalInfo.nLength = 1;
                additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
                CatApp_PopulateCatAppCommandResponse(&pCommandResp,
                                                     AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                     additionalInfo);
                (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                         EVT_CATAPP_CLOSE_CH_FAILED, int32touint16(index),
                                         (uint32)pCommandResp);
                break;
              }
            }
            return TRUE;


            default:
              pMe->cmdHdr = *((AEESTKProactiveCmdHeader*) pMe->m_pProactiveCommand);
              MSG_HIGH("CatApp : Unhandled EVT_NOTIFY  :%d", pMe->cmdHdr.nCommandID, 0, 0);
              return FALSE;

          }
        }
        else if(wParam == NMASK_STKNOTIFIER_ENVELOPE_CMD)
        {
          //AEESTKEnvelopeCmd *pCommand = (AEESTKEnvelopeCmd*) ((AEENotify *)dwParam)->pData;
        }
      }
      else if (((AEENotify *) dwParam)->cls == AEECLSID_CM_NOTIFIER)
      {
        AEECMNotifyInfo *EventInfo = ((AEENotify *)dwParam)->pData;
        if(EventInfo->event != AEECM_EVENT_CALL_ALL_CALLS_ENDED)
        {
          if(!ICM_IsMyCall(pMe->m_pICM, EventInfo->event_data.call.call_id))
          {
            return FALSE;
          }
        }


        switch(EventInfo->event)
        {

        case AEECM_EVENT_CALL_CONNECT:
          MSG_MED("CatApp: AEECM_EVT_CALL_CONNECT", 0, 0, 0);

          /* Send any DTMF if present before releasing the ptrs in SendCommandResponse() */
          if (STRLEN((char *) pMe->dtmf_digits) > 0)
          {
            int tempLen = uint32toint32(STRLEN((char *) pMe->dtmf_digits) + 1);

            /* Free data before malloc to ensure there is no "dangling" pointer */
            CATAPP_FREEIF(pMe->sDTMFDigit);
            pMe->sDTMFDigit = MALLOC(int32touint32(tempLen));

            if (pMe->sDTMFDigit != NULL)
            {
              (void)MEMSET(pMe->sDTMFDigit, 0x00, int32touint32(tempLen));
              pMe->byCurrDTMF      = 0;
              /* The actual number of digits excludes the null terminated byte */
              pMe->byNumDTMFDigits = int32touint8(tempLen - 1);

              /* memcpy only the digit, the last byte is set to null terminated value in
                 the memset already */
              (void)MEMCPY(pMe->sDTMFDigit, pMe->dtmf_digits, pMe->byNumDTMFDigits);
              CatApp_SendDTMF(pMe);
            }
          }

          if (pMe->origination_sent && !pMe->term_resp_sent)
          {
            additionalInfo.nLength = 0;
            if (pMe->m_icon_present)
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED,
                                    &additionalInfo);
            }
            else
            {
              SendCommandResponse(pMe, AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY, &additionalInfo);
            }
          }
          if(pMe->cmdHdr.nCommandID == AEESTK_SETUP_CALL_REQ)
          {
            pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            if (pIActiveDialog)
            {
              wDlgID = IDIALOG_GetID(pIActiveDialog);
              if (wDlgID == IDD_DISPLAY_CLOSE_UPON_EVENT)
              {
                (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
              }
              else if (wDlgID == IDD_SKUSER_INPUT)
              {
                (void)ISHELL_EndDialog(pMe->a.m_pIShell);
                /* todo need to evaluate, since the user confirmation screen will be removed, and then we go to dialer app's display
                  will the script care?
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
                  FREEIF(pMe->m_DisplayTextBuffer.pwszText);
                  DBGEVENT(EVT_CATAPP_NO_DISPLAY, AEECLSID_CATAPP);
#endif FEATURE_CATAPP_TEST_AUTOMATION */
              }
            }
          }
          pMe->origination_sent = FALSE;
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          return (TRUE);

        case AEECM_EVENT_CALL_ORIG:
          MSG_MED("CatApp:AEECM_EVT_CALL_ORIG", 0, 0, 0);
          uisnd_snd_stop();

          pMe->origination_sent = TRUE;
          return (TRUE);

        case AEECM_EVENT_CALL_ALL_CALLS_ENDED:
          if(pMe->m_PendingCallSetUp == TRUE)
          {
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)SendTimerExpired, pMe);
            if (catapp_cm_call_cmd_orig(pMe, &additionalInfo) == FALSE)
            {
              if(pMe->term_resp_sent == FALSE)
              {
                /* Call failed. */
                SendCommandResponse(pMe,
                  AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
              }
            }
          }
          return (TRUE);

        case AEECM_EVENT_CALL_END:
          MSG_MED("CatApp:AEECM_EVT_CALL_END", 0, 0, 0);
          MSG_HIGH("CatAPP:CallEND Status :%d, Additional Info :%d",
            (unsigned int)(EventInfo->event_data.call.end_status), EventInfo->event_data.call.end_additional_info ,0 );

          uisnd_snd_stop();
          if ((pMe->origination_sent) && (!pMe->term_resp_sent))
          {
            switch ((AEECMEndStatus)EventInfo->event_data.call.end_status)
            {
            case AEECM_CALL_END_CLIENT_END:
              additionalInfo.nLength = 0;
              SendCommandResponse(pMe, AEESTK_RESULT_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE,
                  &additionalInfo);
              break;
            case AEECM_CALL_END_NETWORK_END:
            case AEECM_CALL_END_LL_CAUSE:
              additionalInfo.nLength = 1;
              /* End additonal info could be mapped to 4.08 spec */
              /* Does not consider CDMA cause value               */
              if(EventInfo->event_data.call.end_additional_info != AEECM_END_UNAVAILABLE)
              {
                additionalInfo.additionalInfo[0] = (AEESTKAddionalInfoCode)(0x80 |
                    ((uint8)EventInfo->event_data.call.end_additional_info & 0xFF));
              }
              else
              {
                additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
              }
              SendCommandResponse(pMe, AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
              break;
            case AEECM_CALL_END_OFFLINE:
            default:
              additionalInfo.nLength = 0;
              SendCommandResponse(pMe, AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
              break;
            }
          }

          if(pMe->cmdHdr.nCommandID  == AEESTK_SETUP_CALL_REQ)
          {
              pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              if (pIActiveDialog)
              {
                  wDlgID = IDIALOG_GetID(pIActiveDialog);
                  if (wDlgID == IDD_DISPLAY_CLOSE_UPON_EVENT)
                  {
                      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
                  }
              }
          }
          pMe->origination_sent = FALSE;
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);

          pMe->origination_sent = FALSE;
          return (TRUE);

        case AEECM_EVENT_CALL_ERROR:
          MSG_MED("CatApp:AEECM_EVT_CALL_ERR", 0, 0, 0);
          if((EventInfo->event_data.call.call_error == AEECM_CALL_ERROR_NONE))
          {
            return FALSE;
          }
          uisnd_snd_stop();
          if (!pMe->term_resp_sent)
          {
            if (EventInfo->event_data.call.call_error == AEECM_CALL_ERROR_CALL_CONTROL_REJECTED)
            {
              additionalInfo.nLength = 1;
              additionalInfo.additionalInfo[0] = AEESTK_SCREEN_IS_BUSY;
              SendCommandResponse(pMe,
                  AEESTK_RESULT_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM,
                  &additionalInfo);
            }
            else
            {
              additionalInfo.nLength = 0;
              SendCommandResponse(pMe, AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                  &additionalInfo);
            }
          }
          if(pMe->cmdHdr.nCommandID == AEESTK_SETUP_CALL_REQ)
          {

              pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              if (pIActiveDialog)
              {
                wDlgID = IDIALOG_GetID(pIActiveDialog);
                if (wDlgID == IDD_DISPLAY_CLOSE_UPON_EVENT)
                {
                  (void)ISHELL_EndDialog(pMe->a.m_pIShell);
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
                }
              }
          }
          pMe->origination_sent = FALSE;
          (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
          pMe->origination_sent = FALSE;
          return (TRUE);

        default:
          return FALSE;
        }
      }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif
      /* Handle shell init notifier */
      else if ((pNotifyInfo->cls == AEECLSID_SHELL) &&
               (pNotifyInfo->dwMask & NMASK_SHELL_INIT))
      {
        pIShell = AEE_GetShell();
        if (pIShell == NULL)
        {
          MSG_ERROR("Failed to get shell pointer.", 0, 0, 0);
          return TRUE;
        }

        if (ISHELL_StartBackgroundApplet(pIShell,
                                         AEECLSID_CATAPP, NULL) != SUCCESS)
        {
          MSG_MED("Failed to launch Catapp", 0, 0, 0);
        }
#ifdef FEATURE_UIONE_HDK
        pMe->m_bIdleScreenDisplayed = TRUE;
#endif /* FEATURE_UIONE_HDK */
        return TRUE;
      }
      else
      {
        return FALSE;
      }
      break;
    }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
    default:
      break;
  }
  return (FALSE);
}
/*===========================================================================
FUNCTION CatApp_getChannelStatus

DESCRIPTION
  This function checks the Channel table maintained by pMe to learn the channel
  status for open and dropped channels.

PARAMETERS
  CCatApp*pMe   - Catapp table pointer
  boolean value - if set to FALSE, no need to populate status information.
                  if set to TRUE collect status as well as no of channels
                  for which status is available..
  AEESTKGetChStatusResponse *chRespTable - Channel status info table.

DEPENDENCIES
  None

RETURN VALUE
  int32

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int32 CatApp_GetChannelStatus(
  const CCatApp *pMe,
  boolean value,
  AEESTKGetChStatusResponse *chRespTable
)
{
  int32 i = 0;
  int32 cnt = 0;

  if(!pMe)
  {
      MSG_ERROR("pMe ptr NULL ptr",0,0,0);
      return 0;
  }

  if((value == TRUE) && (!chRespTable))
  {
      MSG_ERROR("chRespTable ptr NULL ptr",0,0,0);
      return 0;
  }

  while(i < CATAPP_MAX_GPRS_CH)
  {
     switch (pMe->m_sOpenChInfo[i].status)
     {
       case CATAPP_CH_DROPPED:
         /* if value is TRUE, we need to populate the chRespTable passed in as
            a paramater to this function.
            If value is FALSE, we just have to return the count of the number of
            open and dropped channels.
         */
         if(value && (chRespTable != NULL))
         {
           chRespTable[cnt].nChID  = pMe->m_sOpenChInfo[i].nChID;
           chRespTable[cnt].bIsLinkEst = FALSE;
           chRespTable[cnt].ChStatusAdditionalInfo = AEESTK_CH_STATUS_LINK_DROPPED;
         }
         cnt++;
         break;
       case CATAPP_CH_OPENED:
         if(value && (chRespTable != NULL))
         {
           chRespTable[cnt].nChID  = pMe->m_sOpenChInfo[i].nChID;
           chRespTable[cnt].bIsLinkEst = TRUE;
           chRespTable[cnt].ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
         }
         cnt++;
         break;
       default:
         break;
     } /*  switch (pMe->m_sOpenChInfo[index].status) */
     i++;
  } /* while(i < GSTK_CHANNEL_ID_MAX_VALUE) */
  if (cnt == 0)
  {
    if(value && (chRespTable != NULL))
    {
      chRespTable[cnt].nChID   = 0x00;
      chRespTable[cnt].bIsLinkEst =  FALSE;
      chRespTable[cnt].ChStatusAdditionalInfo = AEESTK_CH_STATUS_NO_FURTHER_INFO;
    }
    cnt = 1;
  }
  return (cnt);
}

/*===========================================================================
FUNCTION: catapp_ConvertLocalAddrToIpv4Format

DESCRIPTION
  This function converts the address bytes to a string in ipv4 format


PROTOTYPE:
  int catapp_ConvertLocalAddrToIpv4Format(
    AEESTKPacketDataAddressType sLocalAddress,
    char                       *IPAddr);

PARAMETERS:
  sLocalAddress - local address bytes
  IPAddr        - ipv4 string

DEPENDENCIES
  None.

RETURN VALUE
  static int
    SUCCESS   - 0  if the conversion was successful
    EFAILED   - 1  if the conversion was not successful.

SIDE EFFECTS
  none
===========================================================================*/
static int catapp_ConvertLocalAddrToIpv4Format(AEESTKPacketDataAddressType sLocalAddress,
                                             char                       *pIPAddr)
{
  int i = 0;
  char tmpStr[4];
  if(sLocalAddress.PktAddressType != AEESTK_PKT_DATA_IPV4_ADDRESS)
  {
    MSG_ERROR("Local IP address conversion to pIPAddr format failed", 0, 0, 0);
    return EFAILED;
  }
  if(pIPAddr == NULL)
  {
    MSG_ERROR("IP Addr Null Ptr", 0, 0, 0);
    return EFAILED;
  }

  for(i=0;i < sLocalAddress.nPktAddressLength;i++ )
  {
    (void)MEMSET(tmpStr, 0x00, 4*sizeof(char));
    (void) SNPRINTF(tmpStr, 4*sizeof(char), "%d", sLocalAddress.pPktAddress[i]);
    (void) STRLCAT(pIPAddr, tmpStr, CATAPP_MAX_IP_ADDR4_LEN);
    if(i != (sLocalAddress.nPktAddressLength - 1))
    {
      (void) STRLCAT(pIPAddr, ".", CATAPP_MAX_IP_ADDR4_LEN);
    }
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION: Catapp_RegisterForNetEvent

DESCRIPTION: Function to register for Net Events with INetMgr

PARAMETERS:
  pMe              pointer to the Applet member structure (CCatApp*)
  **ppCommandResp  pointer to Catapp command response type(will get
                   allocated in CatApp_PopulateCatAppCommandResponse())
  index            index of the channel in the table

RETURN:
  SUCCESS if successful
  EFAILED if unsuccessful

SIDE EFFECTS:
  If the registraion is successful, Catapp will be receving net events
  from inetmgr
===========================================================================*/
static int32 Catapp_RegisterForNetEvent(
  CCatApp                       *pMe,
  CatApp_CommandRespType        **ppCommandResp,
  int                           index
)
{
  AEESTKAdditionalInfo           additionalInfo = {0};
  int                            nReturnStatus  = SUCCESS;
  if(pMe == NULL)
  {
    MSG_ERROR("Invalid NULL pointer",0,0,0);
    return EFAILED;
  }
  if (ppCommandResp == NULL)
  {
    MSG_ERROR("Invalid NULL ppCommandResp",0,0,0);
    return EFAILED;
  }
  if((index < 0) || (index >=  CATAPP_MAX_GPRS_CH))
  {
    MSG_ERROR("Invalid Open Ch Table Index %d", index, 0, 0);
    additionalInfo.nLength           = 1;
    additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    CatApp_PopulateCatAppCommandResponse(ppCommandResp,
                                         AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                         additionalInfo);
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                             EVT_CATAPP_OPEN_CH_FAILED, 0xFF,
                             (uint32)(*ppCommandResp));
    return EFAILED;
  }
  if(pMe->m_sOpenChInfo[index].pINetMgr != NULL)
  {
    nReturnStatus = INETMGR_OnEvent(pMe->m_sOpenChInfo[index].pINetMgr, CatApp_NetEvent,
                                    &(pMe->m_sOpenChInfo[index]), TRUE);
  }
  if ((nReturnStatus != SUCCESS) || (pMe->m_sOpenChInfo[index].pINetMgr == NULL))
  {
    switch(pMe->cmdHdr.nCommandID)
    {
      case AEESTK_OPEN_CH_REQ:
        if(!pMe->m_sOpenChInfo[index].bOnDemandLinkEst)
        {
          MSG_ERROR("Could not Register Net Manager Evt for OPEN_CH_REQ %d", nReturnStatus, 0, 0);
          additionalInfo.nLength           = 1;
          additionalInfo.additionalInfo[0] = AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          CatApp_PopulateCatAppCommandResponse(ppCommandResp,
                                               AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                               additionalInfo);
          (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                   EVT_CATAPP_OPEN_CH_FAILED, 0xFF,
                                   (uint32)(*ppCommandResp));
          return EFAILED;
        }
        break;
      case AEESTK_SEND_DATA_REQ:
        MSG_ERROR("Could not Register Net Manager Evt for SEND_DATA_REQ %d", nReturnStatus, 0, 0);
        additionalInfo.nLength           = 1;
        additionalInfo.additionalInfo[0] = AEESTK_CH_CLOSED;
        CatApp_PopulateCatAppCommandResponse(ppCommandResp,
                                             AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                             additionalInfo);
        (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                                 EVT_CATAPP_SEND_DATA_FAILED, 0xFF,
                                 (uint32)(*ppCommandResp));
        return EFAILED;
      default:
        MSG_ERROR("OPEN_CH_SUCCESS in other event 0x%x", pMe->cmdHdr.nCommandID, 0, 0);
        return EFAILED;
    }
  }
  MSG_HIGH("Register for NetMgr events %d", nReturnStatus, 0, 0);
  return SUCCESS;
}/* Catapp_RegisterForNetEvent */

#if (defined FEATURE_CAT_REL6 || !(defined FEATURE_CATAPP_OLD_SOUND_INTERFACE))
/*===========================================================================
FUNCTION: catapp_ConvertToMsec(AEESTKDuration minDurationOfDisplay,
                                  int32 msec)

DESCRIPTION
  This function converts time in min, sec or tenth of sec to a msec format


PROTOTYPE:
  int catapp_ConvertToMsec(AEESTKDuration minDurationOfDisplay,
                                  int32 msec)

PARAMETERS:
  minDurationOfDisplay - time object containing length and unit of time
  msec                 - time in msec format

DEPENDENCIES
  None.

RETURN VALUE
  static int
    SUCCESS   - 0  if the conversion was successful
    EFAILED   - 1  if the conversion was not successful.

SIDE EFFECTS
  none
===========================================================================*/
static int catapp_ConvertToMsec(AEESTKDuration minDurationOfDisplay,
                                  int32 * msec)
{
    if(msec == NULL)
    {
      MSG_ERROR("Null ptr in catapp_ConvertToMsec()",0,0,0);
      return EFAILED;
    }
    switch(minDurationOfDisplay.nUnits)
    {
      case AEESTK_DURATION_MINUTES:
        *msec = minDurationOfDisplay.nLength * NUM_OF_MILLISECONDS_IN_MINUTE;
      break;
      case AEESTK_DURATION_SECONDS:
        *msec = minDurationOfDisplay.nLength * NUM_OF_MILLISECONDS_IN_SECOND;
      break;
      case AEESTK_DURATION_TENTHS_OF_SECONDS:
        *msec = minDurationOfDisplay.nLength * NUM_OF_MILLISECONDS_IN_TENTH_SECOND;
      break;
      default:
        MSG_ERROR("CATAPP Error:TIME UNIT not Supported",0,0,0);
        return (EFAILED);
    }
  return (SUCCESS);
}
#endif /*FEATURE_CAT_REL6 || !(defined FEATURE_CATAPP_OLD_SOUND_INTERFACE) */

#ifdef FEATURE_CAT_REL6
#ifndef FEATURE_UIONE_HDK
/*===========================================================================
FUNCTION: CatApp_processBatteryLevel(AEEBattLevel pLevel)

DESCRIPTION
  This function processes the battery level


PROTOTYPE:
  int CatApp_processBatteryLevel(AEEBattLevel pLevel)

PARAMETERS:
  AEEBattLevel batt_level - battery level

DEPENDENCIES
  None.

RETURN VALUE
  AEESTKBatteryStatusType
    AEESTK_BATTERY_LEVEL_UNKNOWN
    AEESTK_BATTERY_LEVEL_NA
    AEESTK_BATTERY_LEVEL_0
    AEESTK_BATTERY_LEVEL_1
    AEESTK_BATTERY_LEVEL_2
    AEESTK_BATTERY_LEVEL_3
    AEESTK_BATTERY_LEVEL_4
    AEESTK_BATTERY_LEVEL_NA

SIDE EFFECTS
  none
===========================================================================*/
AEESTKBatteryStatusType CatApp_processBatteryLevel(AEEBattLevel batt_level)
{
    switch(batt_level.level)
    {
    case 0xFF:
        return AEESTK_BATTERY_LEVEL_UNKNOWN;
    case 0xFE:
        return AEESTK_BATTERY_LEVEL_NA;
    case 0:
        return AEESTK_BATTERY_LEVEL_0;
    case 1:
        return AEESTK_BATTERY_LEVEL_1;
    case 2:
        return AEESTK_BATTERY_LEVEL_2;
    case 3:
        return AEESTK_BATTERY_LEVEL_3;
    case 4:
        return AEESTK_BATTERY_LEVEL_4;
    default:
        return AEESTK_BATTERY_LEVEL_NA;
    }
}
/*===========================================================================
FUNCTION: CatApp_processChargerState

DESCRIPTION
  This function processes the charger state given by IBATT_GetChargerStatus


PROTOTYPE:
  AEESTKBatteryStatusType CatApp_processChargerState(IBatt *po, AEEChargerStatus charger_state)

PARAMETERS:
  IBatt *po                        - Battery object
  AEEChargerStatus charger_state   - charger state

DEPENDENCIES
  None.

RETURN VALUE
  AEESTKBatteryStatusType
    AEESTK_BATTERY_LEVEL_UNKNOWN
    AEESTK_BATTERY_LEVEL_NA
    AEESTK_BATTERY_LEVEL_0
    AEESTK_BATTERY_LEVEL_1
    AEESTK_BATTERY_LEVEL_2
    AEESTK_BATTERY_LEVEL_3
    AEESTK_BATTERY_LEVEL_4
    AEESTK_BATTERY_LEVEL_NA


SIDE EFFECTS
  none
===========================================================================*/
AEESTKBatteryStatusType CatApp_processChargerState(
    IBatt *po,
    AEEChargerStatus charger_state)
{
  AEEBattLevel     batt_level;

  switch(charger_state)
  {
    case AEECHG_STATUS_NO_BATT:
    case AEECHG_STATUS_UNKNOWN_BATT:      /* Charger found an unrecognized battery  */
    case AEECHG_STATUS_NO_CHARGE_NO_BATT: /*Charger is weak and no Battery present  */
    case AEECHG_STATUS_DEAD_BATT:         /* Charger found a dead battery.*/
      return AEESTK_BATTERY_LEVEL_NA;

    case AEECHG_STATUS_NO_CHARGE:         /* Nothing's wrong.  Just haven't started */
    case AEECHG_STATUS_NO_CHARGER:        /* Charger isn't working now              */
    case AEECHG_STATUS_CANNOT_CHARGE:     /* Cannot charge for the moment           */
    case AEECHG_STATUS_INIT:              /* Initialized state                      */
      (void)IBATT_GetBattStatus(po, &batt_level);

      return CatApp_processBatteryLevel(batt_level);

    case AEECHG_STATUS_CHARGING:          /*Battery is being charged                */
      return AEESTK_BATTERY_LEVEL_UNKNOWN;

    case AEECHG_STATUS_FULLY_CHARGE:      /* Battery is fully charged               */
      return AEESTK_BATTERY_LEVEL_4;

    case AEECHG_STATUS_OVERVOLTAGE:
    default:
      return AEESTK_BATTERY_LEVEL_UNKNOWN;
  }
}
#endif /* FEATURE_UIONE_HDK */

/*===========================================================================

FUNCTION CatApp_SetHideSelectionSoftkeyLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void CatApp_SetHideSelectionSoftkeyLook(IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle sstyle, ustyle;
  AEEMenuColors col;

  if(ps == NULL)
  {
    MSG_HIGH("IShell ptr ps is NULL",0,0,0);
    return;
  }
  if(pm == NULL)
  {
    MSG_HIGH("IMenuCtl ptr pm is NULL",0,0,0);
    return;
  }

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &ustyle, &sstyle);


  /* selection style */
  sstyle.ft = AEE_FT_NONE;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pm, &ustyle, &sstyle);

  /* colors */
  col.cSelText = RGB_BLACK;
  col.cText    = RGB_BLACK;
  col.wMask    = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT | MC_TEXT;
  col.cBack    = MAKE_RGB(255, 255, 204);
  col.cSelBack = MAKE_RGB(255, 255, 204);

  IMENUCTL_SetColors(pm, &col);
}
#endif /* #ifdef FEATURE_CAT_REL6 */

/*===========================================================================

FUNCTION CatApp_BindSocket

DESCRIPTION
  Binds a specified socket to the reserved port specified by the card

PROTOTYPE:
  void CatApp_BindSocket(ISocket * pISocket, INAddr a, uint16 wPort)

PARAMETERS:
  ISocket * pISocket - socket pointer
  INAddr a           - local IP addr
  uint16 wPort       - port specified by the card in network byte order

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/
int CatApp_BindSocket(CatApp_OpenChInfo openChInfo)
{
  int status = 0;
  MSG_HIGH("In CatApp_BindSocket",0,0,0);
  if((openChInfo.status != CATAPP_CH_OPENED) ||
     (openChInfo.pISocket == NULL))
  {
    MSG_HIGH("Socket of Channel 0x%x to port 0x%x failed",openChInfo.nChID,NTOHL(openChInfo.dwPortNum),0);
    return EFAILED;
  }
  status = ISOCKET_Bind(openChInfo.pISocket,AEE_INADDR_ANY,openChInfo.dwPortNum);

  switch(status)
  {
  case AEE_NET_SUCCESS:
    MSG_HIGH("bind of channel 0x%x successful with port 0x%x ...\n", openChInfo.nChID,NTOHL(openChInfo.dwPortNum),0);;
    return SUCCESS;

  case AEE_NET_WOULDBLOCK:
    MSG_HIGH("Case AEE_NET_WOULDBLOCK: err: 0x%x, nChId 0x%x",
              ISOCKET_GetLastError(openChInfo.pISocket), openChInfo.nChID, 0);

    /* Not sure if we want to do a n ISocket_writeable to try to bind once again in
       a failure scenario
    */
    ISOCKET_Writeable(openChInfo.pISocket,(PFNNOTIFY)CatApp_BindSocket,&openChInfo);
    return SUCCESS;

  default: /* AEE_NET_ERROR */
    MSG_HIGH("Case Default: Socket of Channel 0x%x to port 0x%x failed",
              openChInfo.nChID,NTOHL(openChInfo.dwPortNum),0);
    return EFAILED;
  }
}
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_UIONE_HDK
/*=========================================================================
FUNCTION
  CatApp_FetchURLResourceString

DESCRIPTION
  Get URL string from the Resource File

DEPENDENCIES
  None

RETURN VALUE
  EFAILED - if Error
  SUCCESS - if Success

SIDE EFFECTS
  Populate the destination string pTmpStr with the char* equivalent of
  the resource string.
=========================================================================*/
int CatApp_FetchURLResourceString(
  CCatApp *pMe,
  char    *pTmpStr,
  int      destSize,
  uint16   nResId
)
{
  AECHAR     *pResStr      = NULL;
  uint32      nBufSize     = 0;

  if(NULL == pMe)
  {
    MSG_ERROR("pMe is null", 0, 0, 0);
    return EFAILED;
  }
  if(NULL == pTmpStr)
  {
    MSG_ERROR("pTmpStr is null", 0, 0, 0);
    return EFAILED;
  }

  /* MALLOC Resource String */
  if (NULL == ISHELL_LoadResDataEx(pMe->a.m_pIShell,
                                   CATAPP_RES_FILE,
                                   nResId,
                                   RESTYPE_STRING,
                                   (void*) -1,
                                   &nBufSize))
  {
    MSG_ERROR("ISHELL_LoadResDataEx Error:Unable to fetch ResStr size",0,0,0);
    return EFAILED;
  }

  /* Read the URL Resource string */
  pResStr = (AECHAR*)MALLOC((nBufSize+CATAPP_ADDITIONAL_NULL_CHAR)
                             * sizeof(AECHAR));
  if(NULL == pResStr)
  {
    MSG_ERROR("pResStr is null", 0, 0, 0);
    return EFAILED;
  }
  (void)MEMSET((void*)pResStr,
                0x00,
                (nBufSize + CATAPP_ADDITIONAL_NULL_CHAR) *
                sizeof(AECHAR));

  if(ISHELL_LoadResString(pMe->a.m_pIShell,
                          CATAPP_RES_FILE,
                          nResId,
                          pResStr,
                          uint32toint16(nBufSize*sizeof(AECHAR))) == 0)
  {
    FREE(pResStr);
    MSG_ERROR("ISHELL_LoadResString failed", 0, 0, 0);
    return EFAILED;
  }

  (void)MEMSET((void*)pTmpStr, 0x00, int32touint32(destSize) * sizeof(char));

  if( (uint32)destSize < nBufSize)
  {
    MSG_HIGH("Dest Size 0x%x < resource string size 0x%x",destSize,nBufSize,0);
  }

  /* Convert the resource string from AECHAR  to char */
  (void)WSTRTOSTR(pResStr,
                  pTmpStr,
                  destSize);
  FREE(pResStr);
  return SUCCESS;
}

/*=========================================================================
FUNCTION
  CatApp_GetClassIdfromURL

DESCRIPTION
  Get class id from URL

DEPENDENCIES
  None

RETURN VALUE
  EFAILED - if Error
  SUCCESS - if Success

SIDE EFFECTS
  Populate the class id pointer
=========================================================================*/
int CatApp_GetClassIdfromURL(
  CCatApp *pMe,
  char *pszURL,
  AEECLSID *pIdleAppClassId
)
{
  char *pTempURLString      = NULL;
  char *pAppNameDelimiter   = NULL;

  if(NULL == pMe)
  {
    MSG_ERROR("pMe is null", 0, 0, 0);
    return EFAILED;
  }
  if(NULL == pszURL)
  {
    MSG_ERROR("pszURL is null", 0, 0, 0);
    return EFAILED;
  }
  if(NULL == pIdleAppClassId)
  {
    MSG_ERROR("pIdleAppClassId is null", 0, 0, 0);
    return EFAILED;
  }
  pTempURLString = STRDUP(pszURL);
  if(NULL == pTempURLString)
  {
    MSG_ERROR("pTempURLString is null", 0, 0, 0);
    return EFAILED;
  }
  pAppNameDelimiter = STRCHR(pTempURLString,':');
  /* Send ISHELL_GetHandler a URL that doesn't have a ':' after the app name.*/
  if(pAppNameDelimiter)
  {
    *pAppNameDelimiter = 0;
  }
  /* To get the Class ID of IImage */
  *pIdleAppClassId = ISHELL_GetHandler(pMe->a.m_pIShell,
                                     AEECLSID_APP,
                                     pTempURLString);

  if(0 == *pIdleAppClassId)
  {
    MSG_HIGH("ISHELL_GetHandler returned App ID 0x%x is null", *pIdleAppClassId, 0, 0);
    return EFAILED;
  }
  return SUCCESS;
}


/*=========================================================================
FUNCTION
  CatApp_DeviceInfoChangeNotifCb

DESCRIPTION
  Callback for receiving device change notifications

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Send Language change indication envelope is language has changed
=========================================================================*/
void CatApp_DeviceInfoChangeNotifCb(void *pData)
{
  CCatApp                      * pMe     = (CCatApp *)pData;
  AEEDeviceInfo                * pDi     = NULL;
  int                          nRetVal   = SUCCESS;
  AEESTKLangSelectionResponse  sLangSel;
  uint8                        nLangSel[AEESTK_LANG_CODE_LEN];

  if(NULL == pMe)
  {
    MSG_ERROR("pMe is NULL",0,0,0);
    return;
  }

   /* Allocate memory for device info */
  pDi = MALLOC(sizeof(AEEDeviceInfo));
  if (NULL == pDi)
  {
    MSG_ERROR("pDi is NULL",0,0,0);
    return;
  }

  /*Get Device Info */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell, pDi);
  /* pDi->dwLang  is of type AEE Language Type.
    The last two bytes of pDi->dwLang give the langauge info */
  nLangSel[0] = pDi->dwLang & 0xFF;
  nLangSel[1] = (pDi->dwLang >> CATAPP_SHIFT_BY_ONE_BYTE ) & 0xFF;
  if((pMe->m_currLang[0] != nLangSel[0]) || (pMe->m_currLang[1] != nLangSel[1]))
  {
    sLangSel.len       = AEESTK_LANG_CODE_LEN;
    sLangSel.sLangCode = nLangSel;
    nRetVal            = ISTK_LangSelection(pMe->m_pISTK, &sLangSel);
    if (nRetVal != SUCCESS)
    {
      MSG_HIGH("Failed ISTK notification, %d", nRetVal, 0, 0);
      FREE(pDi);
      return;
    }
    /* store the language sent as envelope to the card */
    (void)MEMCPY(pMe->m_currLang, nLangSel, sizeof(uint8)*AEESTK_LANG_CODE_LEN);
  }
  FREE(pDi);
}


/*=========================================================================
FUNCTION
  CatApp_Send_URI_To_App

DESCRIPTION
  Function to send URI to HDK App.

DEPENDENCIES
  None

RETURN VALUE
  EFAILED - if sending URI FAILED
  SUCCESS - if sending URI was a SUCCESS

SIDE EFFECTS

=========================================================================*/
int CatApp_Send_URI_To_App(
  CCatApp                *pMe,
  AEESTKProactiveCmdType cmdType,
  AECHAR                 *pWStr
)
{
  AEECLSID         telClassId;
  char            *pStr                = NULL;
  char            *pszURL              = NULL;
  int32            strLen              = 0;
  uint16           nResId              = 0;
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  if(NULL == pMe)
  {
    MSG_ERROR("pMe is NULL",0,0,0);
    return EFAILED;
  }

  /* pWStr can be NULL indication a NULL alpha from the card */


  switch(cmdType)
  {
  case AEESTK_UPDATE_IDLE_TEXT_REQ:
    nResId = IDS_HDK_URL_IDLE_STR;
    break;
  case AEESTK_SEND_DTMF_REQ:
  case AEESTK_SEND_SS_REQ:
  case AEESTK_SEND_USSD_REQ:
    nResId = IDS_HDK_URL_TEL_STR;
    break;
  default:
    break;
  }

  /* convert "alpha ID from card" from AECHAR* to char* */
  if(EFAILED == CatApp_AllocAndConvertWSTRTOSTR(pWStr,&pStr, &strLen))
  {
    MSG_ERROR("CatApp_AllocAndConvertWSTRTOSTR() failed ", 0, 0, 0);
    return EFAILED;
  }
  /* Allocate memory to compose the URL */
  pszURL = (char*)MALLOC(int32touint32(strLen+CATAPP_URL_STANDARD_STRING_LEN));
  if(NULL == pszURL)
  {
    MSG_ERROR("pszURL is null in proactive command", 0, 0, 0);
    CATAPP_FREEIF(pStr);
    return EFAILED;
  }
  (void)MEMSET(pszURL,
               0x00,
               int32touint32(strLen+CATAPP_URL_STANDARD_STRING_LEN));

  /* Fetch URL string  */
  if(EFAILED == CatApp_FetchURLResourceString(pMe,
                                              pszURL,
                                              CATAPP_URL_STANDARD_STRING_LEN,
                                              nResId))
  {
    MSG_ERROR("CatApp_FetchURLResourceString FAILED", 0, 0, 0);
    FREEIF(pStr);
    FREE(pszURL);
    return EFAILED;
  }
  if(pStr)
  {
    (void)STRLCAT(pszURL,
                  pStr,
                  int32touint32(strLen+CATAPP_URL_STANDARD_STRING_LEN));
  }

  /* Get class Id of the tel App */
  if(EFAILED == CatApp_GetClassIdfromURL(pMe,pszURL, &telClassId))
  {
    FREEIF(pStr);
    FREE(pszURL);
    MSG_ERROR("CatApp_GetClassIdfromURL() FAILED", 0, 0, 0);
    return EFAILED;
  }
#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
  /* Send Event to the tel App */
  if(ISHELL_SendEvent(pMe->a.m_pIShell,
                      telClassId,
                      EVT_APP_POST_URL,
                      0,(uint32)pszURL) == FALSE)
  {
    FREEIF(pStr);
    FREE(pszURL);
    MSG_ERROR("ISHELL_SendEvent() FAILED", 0, 0, 0);
    return EFAILED;
  }
  FREEIF(pStr);
  FREE(pszURL);
  return SUCCESS;
}


/*=========================================================================
FUNCTION
  CatApp_AllocAndConvertWSTRTOSTR

DESCRIPTION
  Convert AECHAR string to char string

DEPENDENCIES
  None

RETURN VALUE
  EFAILED - if Error
  SUCCESS - if success

COMMENTS:
  Caller of function needs to release memory allocated for *ppStr

SIDE EFFECTS
  Allocate memory for the char * destination string and its size pointer
=========================================================================*/
int CatApp_AllocAndConvertWSTRTOSTR(AECHAR *pwStr, char **ppStr, int32 *pDestSize)
{
  int strLen = 0;

  if(NULL == pDestSize)
  {
    MSG_ERROR("pDestSize is null", 0, 0, 0);
    return EFAILED;
  }
  if(NULL == ppStr)
  {
    MSG_ERROR("ppStr is null", 0, 0, 0);
    return EFAILED;
  }

  /* pwStr can be NULL */

  *ppStr      = NULL;
  *pDestSize = 0;
  if(pwStr)
  {
    strLen = WSTRLEN(pwStr);
    *ppStr  = MALLOC(int32touint32(strLen+CATAPP_ADDITIONAL_NULL_CHAR));
    if(NULL == *ppStr)
    {
      MSG_ERROR("pStr is null", 0, 0, 0);
      return EFAILED;
    }
    (void)MEMSET(*ppStr,
                 0x00,
                 int32touint32(strLen+CATAPP_ADDITIONAL_NULL_CHAR));

    /* Convert the string from card from widestring to string */
    (void)WSTRTOSTR(pwStr,
                    *ppStr,
                    strLen+CATAPP_ADDITIONAL_NULL_CHAR);
    *pDestSize = strLen+CATAPP_ADDITIONAL_NULL_CHAR;
  }
  return SUCCESS;
}
#endif /* FEATURE_UIONE_HDK */
#endif /* FEATURE_APP_CATAPP */
/*========================End of File ================================================*/
