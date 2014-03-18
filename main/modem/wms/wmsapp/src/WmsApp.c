/*=============================================================================

FILE: WMSApp.c

SERVICES: QCT UI WMS BREW Application.

GENERAL DESCRIPTION:
This applet is a SMS Application

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsApp.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/08   HN      Change SPRINTF to SNPRINTF to fix Klocwork warnings
07/29/08   PMD     Fixed Compile Error due to bad featuzrization
07/14/08   PMD     Redefine CFGI_MSG_DISABLE_DAEMONS
06/27/08   PMD     Cleaned up Feature Flags in Code
06/23/08   HN      Fixed Lint & Klocwork warnings
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
04/09/08   SM/PMD  Fixed Potential Buffer overruns
03/31/08   PMD     Added support for dropping CDMA Flash SMS Message
03/27/08   PMD     Fixed bc_mm_table.entries allocation and free
02/07/08   PMD     Replaced Deprecated Function Calls with newer equivalents
02/07/08   PMD     Added Special Handling for Cell Change Event with no Malloc
02/01/08   PMD     Fixed Debug Message Comment
01/31/08   PMD     Updated Copyright Information
01/29/08   SM      Added check for CFGI_MESSAGING_DAEMON Config Item
11/14/07   SM      Removed Messaging Daemon check for build stability
10/30/07   SM      Changed pfnCancel statements to CALLBACK_Cancel
10/25/07   SM      Adding check for CFGI_MESSAGING_DAEMON Config Item
09/29/07   PMD     Fixed Lint Warnings
08/30/07   PMD     Init Card in WMSAPP_Init() only for GW Mode
06/08/07   PMD     Removed check for CFGI_MESSAGING_DAEMON Config Item
06/06/07   PMD     Fixed Lint Errors
04/03/07   YZ      Start WMSAPP in Background using NMASK_SHELL_INIT
03/21/07   NP      Freeing struct memebers without checking if its NULL
                   causing kernel panic.
02/22/07   JAS     Featurize Annunciator references for uiOne HDK builds
01/31/07   PMD     Fixed Compile Error
                   Fixed Potential Memory Corruption Issues - MALLOC, STRCPY, STRCAT
01/26/07   HQ      Start WmsApp properly in uiOne HDK builds.
01/17/07   HQ      Updated copyright info for 2007.
12/06/06   PMD     Added support for Primary Client Set Event
12/04/06   PMD     Added checks for Null Pointer after MALLOC
12/01/06   NP      Removed AEE_Active() from WMSAPP_MsgCb().  Was returning FALSE
                   when other apps were taking UI and thus not allowing cb handling.
11/20/06   PMD     Appended 1 to length for Null Character in wms_ts_unpack_ascii
11/17/06   PMD     Initialized other in client_bd structure to 0
                   Replaced wms_ts_decode with IWMS_TsDecode API Call
09/12/06   PMD     Drop Messages with UDH Port Header
05/18/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
03/29/06   PMD     Send Suspend Event to Dialogs for Processing
02/09/06   PMD     Fixed Include File wmsapp.brh case
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes				  
12/20/05   PMD     Added support for WmsApp.brh File
10/19/05   VK      EMS Featurization fixes
10/17/05   PMD     Removed Retry Config Item Code
                   Fixed Feature Define Comment
07/26/05   PMD     Fix for Incompatible Message Mode on Alert Viewing.
07/26/05   Rex     Featurization fixes
07/15/05   PMD     Added Cell Change Type Debug Statement.
06/13/05   Rex     Added support for IWMSDIAG_CbDeleteAllResponse
06/10/05   VK      Fixed Secondary Display Annuciator Update
06/10/05   VK      Fixed compile errors
06/09/05   Rex     Added fixes for correct notification for voicemail
06/08/05   VK      Back to back Enhanced CB messages not received
06/06/05   Rex     Added fixes for voicemail
05/25/05   PMD     Added Null Pointer Checks for Broadcast Messages.
05/10/05   PMD     Fix for Correct Notification incase of Immediate Display.
05/04/05   PMD     Added support for Any Update Number is Change
03/31/05   PMD     Added support for Cell Change Type Event Information.
03/24/05   PMD     Corrected isUpdateable Logic for GW Broadcast Messages
03/22/05   Rex     Fixed WMSAPP_GetFileAttributeMN_CB callback
03/22/05   Rex     Fixed WMSAPP_GetFileAttributeMN_CB
03/21/05   Rex     Fixed memory free in vm and mn callback
03/11/05   PMD     Inserted F_SMS_LTK for LTK Compilation
03/11/05   PMD     Replaced F_GSM_CB by F_GWSMS_BROADCAST
03/09/05   Rex     Featurization fix for CPHS
03/08/05   PMD     Fixed Compile Warning
03/05/05   Rex     Added VMWI and Mailbox Number support
03/01/05   VK      Additional Changes to the test menu - access channel msg test
02/24/05   VK      Added Test menu support for checking access channel msg sizes
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
02/18/05   PMD     Check Return Value of ISHELL_GetActiveDialog().
02/01/05   PMD     Added Support for UMTS BMC Reading Preferences
02/01/05   PMD     Cleaned up Featurization around SetIdleMenuTextInBkgrnd.
01/31/05   PMD     Prevent WMSAPP from handling events before init.
01/27/05   PMD     Added IAnnunciator under !FEATURE_SMS_LTK
                   Get GSM CB Table and Pref only if mode = GW
01/17/05   VK      Removed Compiler warnings
01/17/05   VK      Added Secondary Display
01/17/05   PMD     Replaced FREEIF calls with WMSAPPU_FREE
01/12/05   Rex     Fixes for CPHS
01/07/05   PMD     Replaced uisnd_snd calls by WMSAPP_SoundAlert().
01/07/05   PMD     Support to Not Play IMelody / Beep Alerts during Voice / Video Calls
01/07/05   PMD     Added Feature FEATURE_SMS_LTK
11/16/04   PMD     Used New Config Item for GCF to set Retry Period.
11/12/04   Rex     Added fix for memory leaks with sms_msg_event_info_buf
11/04/04   PMD     Drop WAP Push Messages in WMSAPP
10/16/04   PMD     Seperated CDMA and GW Broadcast Message Notification.
10/08/04   PMD     Changed behavior for handling Default Template
10/06/04   Rex     Update message list for WMS_TAG_NONE
10/05/04   PMD     Fixed Compile Errors and Warnings
09/30/04   Rex     Featurization fix for IWMSDIAG
09/29/04   PMD     Release Instance of IFILEMGR to avoid Memory Leak
09/29/04   PMD     Replaced FREE with FREEIF to ensure we do not free Null Pointers
09/29/04   PMD     Fixed Display of Junk Characters in CB Message.
08/29/04   PMD     Added GSM CB Debug Messages
09/24/04   Rex     Fix for CPHS
09/23/04   Rex     Featurize SetIdleMenuTextInBkgrnd for compilation issues
09/22/04   Rex     Fixed compile error
09/21/04   Rex     Added support for CPHS
09/17/04   PMD     Changes to make WMSAPP a background App.
08/06/04   Rex     Checking for invalid cb msg index
08/05/04   Rex     Featurize IWMSDIAG
08/04/04   PMD     Added Extra Checking for Pointer Allocate and Free.
07/23/04   PMD     Display Sending Message only if Client is WMSAPP.
07/16/04   PMD     Using SetIdleMenuTextInBkgrnd() API to not go back to root screen
07/15/04   PMD     Added Handling of New Event for Duplicate CB Page
06/24/04   Rex     Fixed cb sms raw data return length
06/23/04   PMD     Call DisplayMsgDlg only if App is not suspended.
06/22/04   Rex     Added some checking in EVT_NOTIFY when receiving bad msg index
                   For EVT_NOTIFY, set dwParam to be AEENotify
06/22/04   VK      Added Support for displaying dialogue after message is saved.
06/11/04   PMD     Replaced wms function calls with IWMS
                   Featurize code under GSM_CB and IWMSDIAG
06/08/04   PMD     Added Support for Clearing Immediate Display on Deletion.
06/08/04   PMD     Clear Immediate Display on Deactivation of Cell Broadcast
05/27/04   Rex     Added support for IWMSDIAG interface
04/23/04   Rex     Fix memory leak problem in WMSAPP_CfgCb cell change event
04/22/04   PMD     Use IWMS_MsgGetCacheInfo API
04/22/04   PMD     Fixed Power up stuck up while reading CB Messages from EFS
04/21/04   PMD     Fix for storing Page Numbers in EFS Cache.
04/09/04   PMD     Fix for displaying Normal Display CB Message Notification.
04/08/04   PMD     Fix for early termination of CB Messages on detection of 2 '\r'
04/05/04   Rex     For GW CB immediate msg, convert text to ASCII prior to display
03/31/04   PMD/Rex Moved WMSAPPU_IsMsgActive and WMSAPPU_UpdateIcons.
03/31/04   Rex     Modify srv_id and lang fields to correspond with the
                   wmsapp_cb_msg_info_cache_s_type structure changes
03/24/04   PMD     Added support for FEATURE_WMSAPP_STORAGE
03/22/04   PMD     Handling for Update Number in CB RAM Messages
03/22/04   PMD     Changed Display handling by selectively calling SetIdleMenuText()
03/18/04   PMD     Fixes for Immediate Display Cell Broadcast Message
03/16/04   AP      Added support for the user app
03/09/04   PMD     Added Support for Primary Client
02/14/04   PMD     Added Support for Retry Settings
02/14/04   PMD     Fixed various bugs in WMSAPP_InitBroadcastMessages()
02/03/04   PMD     Cleaned up Lint Warnings in code
02/03/04   PMD     Support for maintaining WMSAPP Dialog Stack on Suspend
02/03/04   PMD     Removed FEATURE_SMS_SEGMENT
01/30/04   Rex     Fixed compile error by featurizing SetIdleMenuText
01/27/04   PMD     Added Support for Immediate Display Broadcast Message
01/27/04   PMD     Added support for Compose from other Apps
01/26/04   PMD     Added Segmentation Definitions
01/16/04   PMD     Used IWMS_MsgSetRetryPeriod for Retry
12/22/03   PMD     Fixed problem of Incoming Message and Send / Store
12/18/03   PMD     Removed the workaround for Brew 3.0 and removed CloseApplet
12/17/03   PMD     Added workaround for BREW 3.0 dropping CDMA Ready Event
12/03/03   PMD     Lint related changes for Saber
12/01/03   PMD     Removed RegisterMsgInfoCacheCb Function
11/21/03   PMD     Added GSM Cell Broadcast Support
11/14/03   AP      Check if app is suspended before creating NEW MT msg dialog.
                   Added Debug messages.
11/13/03   PMD     Added Support to directly read message from Alert Screen
11/10/03   PMD     Added new checks for voicecall and non-sms call using ICM
11/09/03   PMD     Added new functions to check for Display Alert and Sound Alert
11/08/03   PMD     Replaced wms functions by IWMS functions for Transparent MO
11/06/03   AP      Use selected auto DC send Service Option.
11/06/03   Rex     Status Report Display Changes
11/06/03   PMD     Changes for Brew 3.0
11/05/03   PMD     Additional Check (ui_is_in_call) for Beeping Alert
11/04/03   PMD     Added Support for CDMA and GW Default Templates using ICFG
11/03/03   PMD     Removed CDMA RAM Initialization in WMSAPP_Init.
                   Modified EVT_WMS_CMD_ERR handling if Suspended
11/03/03   PMD     Seperated the CDMA and GW Template List and Default Template
10/31/03   PMD     Added Alert Settings Check before displaying Alert
10/30/03   PMD     Added certain functions skipped during pervious merge
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
10/22/03   PMD     Removed WMSAPP_MsgInfoCacheCb
                   Added IWMS_RegisterMsgInfoCacheCb for registering MsgInfo
                   Cache Callback.
10/03/03   AP      Fixed handling of voicemail MT messages: 0 voicemails and
                   voicemail alert display.
09/24/03   PMD     Disable the Splash Screen on Launch of WMSAPP
09/23/03   PMD     Changed User Data in IWMS functions to be a pointer to pMe
09/19/03   PMD     Added handling for GW Domain Preference Cfg Event from WMS
                   Added Memory Full Event Alert even if App is Suspended.
                   Changes to Overwrite the notification with the latest one.
09/16/03   PMD     Fixed Compile warning for GW only builds
09/02/03   PMD     Enabled Splash Screen Display
08/28/03   AP      Handle new event for ack report.
08/15/03   PMD     Added Support and handling for Status Report Memory Status List
08/06/03   PMD     Fix for Status Report Deletion problem from WMSAPP
08/05/03   PMD     Changes to Get Mode preference (CDMA/GSM) in NV using IConfig
                   Removed WMSAPP_SetModeControl and dependency on SK UI.
07/29/03   AP      Added DC callback; Fixed included files to generate correct
                   dependencies; Changed annunciator to not depend on the mode;
                   Added support for auto connect DC to send message.
07/14/03   PMD     Changes to remove / suppress Lint Errors
07/11/03   PMD     Changes for Displaying Splash Screen on Launch
07/08/03   AP      Do not reset the cache after falling through into GW feature.
                   Initialize a flag for reply messages.
07/03/03   AP      Fixed msg info cache cb to copy to the cache correctly.
06/26/03   PMD     Added Support for display of Alpha ID for MO SMS Control
                   for send and submit report events
                   Added support for copy of Alpha ID for GW Template
                   Changes reqd for WMSAPPMN_DisplayMsgDlg function change
06/25/03   AP      Fixed memory useage for alpha id.
06/20/03   HQ      Check if message_mode==GW for dynamic alpha ID processing.
06/19/03   PMD     Copying the Alpha_ID from the wms_msg_events - read_template,
                   write_template, send and submit report
06/10/03   AP      Check if the app is suspended prior to displaying a
                   notification.
06/10/03   PMD     New Function Added in InitAppData to initialize
                   EMS Directory Structure
06/05/03   PMD     Support for dynamic alpha_id in gw_template
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
06/05/03   HQ      Changed FEATURE_WMSAPP to FEATURE_WMS_APP.
06/03/03   SUN     Placed the file under FEATURE_WMSAPP
06/02/03   PMD     Added pointer to IConfig in WMSAPP Structure
06/02/03   AP      Added auto disable for SMS calls.
05/29/03   PMD     Changed Message Send Display Strings
05/20/03   SUN     Changed structs to pointers in pMe
05/19/03   PMD     Initialized the Default Template Text
05/17/03   PMD     Fix for Default template to be last Template
05/09/03   SUN     Added support for Memory full, code cleanup,
                   code review changes
05/01/03   AP      Display msg info cache correctly if encoding is Unicode.
04/25/03   AP      Added null pointer checks.
04/03/03   SUN/AP  Clean up.
03/17/03   AP      Fixed msg cache info cb to use MALLOC for structure and
                   buffer declarations to avoid overflowing the wms_stack.
                   Set mode correctly when creating the idle sms menu dialog and
                   before updating icons.
03/10/03   AP      Forward events and display status of events only when
                   the app is not suspended.  Start the applet w/args.
03/04/03   AP      Free data from callbacks, handle subsequent MT alerts.
02/27/03   AP      Clean up.
02/24/03   SUN     Applet Should be Started using ISHELLApplet_StartArgs
                   Added AFLAG_HIDDEN
02/14/03   SUN     Inital Revision
===========================================================================*/
/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "comdef.h"
#include "AEEAppGen.h"        // AEEApplet structure and Applet services
#include "AEE_OEM.h"
#include "BREWVersion.h"

#if MIN_BREW_VERSION(3,1)
#include "AEE_OEMEvent.h"
#endif

#include "AEEText.h"

#ifdef FEATURE_WMS_APP
#include "msg.h"

#include "WmsApp.h"
#include "wmsapp.brh"
#include "WmsappMenu.h"
#include "WmsAppUtils.h"
#include "uixsnd.h"

#ifndef FEATURE_ICM
#include "uiscall.h"
#endif

#ifdef FEATURE_IWMSDIAG
#include "AEEWMSDIAG.h"
#endif

#include "OEMCFGI.h"
#ifdef FEATURE_UIONE_HDK
#ifndef CFGI_MSG_DISABLE_DAEMONS
#define CFGI_MSG_DISABLE_DAEMONS (0xA000 + 63)
#endif
#endif /* FEATURE_UIONE_HDK */


/*===========================================================================
                            MACROS   
===========================================================================*/
#define WMSAPP_RELATIVE_VALIDITY_IMMEDIATE          246


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
static boolean    WMSAPP_InitAppData(IApplet* po);



/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/

static const AEEAppInfo gaiWMSApp = {AEECLSID_WMSAPP,
  WMSAPP_RES_FILE,
  IDS_SMS_TITLE,
  IDB_SMS_TITLE,IDB_SMS_TITLE,0,0,AFLAG_STATIC| AFLAG_HIDDEN};

//
// SMS app can either be statically built into BREW or dynamically linked during run-time.
// If BREW_STATIC_APP is defined, then SMS app will be a static app.
// NOTE: Static apps can be built only by OEMs or Carriers. App developers can build dynamic apps only.
//

static boolean WMSAPP_HandleEvent(CWMSAPP *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#if defined(BREW_STATIC_APP)

int         WMSAPP_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
int         WMSAPP_Load(IShell *ps, void * pHelpers, IModule ** pMod);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

//===========================================================================



//===========================================================================
PFNMODENTRY CWMSAPP_GetModInfo(const IShell * ps, const AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,const uint16 * pwMinPriv)
{
  *pApps = (AEEAppInfo *)&gaiWMSApp;
  *pnApps = 1;
  return((PFNMODENTRY)WMSAPP_Load);
}

//===========================================================================
//
//===========================================================================
int WMSAPP_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
  return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,WMSAPP_CreateInstance,NULL));
}

#endif  //BREW_STATIC_APP

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_CfgCb

DESCRIPTION
  Callback function for all the CFG related events

DEPENDENCIES
  None

SIDE EFFECTS
  Posts the Event to itself

===========================================================================*/
void WMSAPP_CfgCb
(
wms_cfg_event_e_type         event,
wms_cfg_event_info_s_type    *pInfo
)
{
  wms_cfg_event_info_s_type *sms_cfg_event_info_buf=NULL;
  IShell *shell_ptr = AEE_GetShell();
  uint8 nRetVal; 

  MSG_HIGH("WMSAPP_CfgCb()",0,0,0);
  
  if(shell_ptr == NULL)
  {
    MSG_ERROR("WMSAPP: shell_ptr = NULL",0,0,0);
    return;
  }

  /* Special Handling for WMS_CFG_EVENT_CELL_CHANGE with no MALLOC */
  if (WMS_CFG_EVENT_CELL_CHANGE == event)
  {
     MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_CELL_CHANGE",0,0,0);
     nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_CFG_CELL_CHANGE,
                        0, (uint32)pInfo->cell_change_type);
     if(nRetVal == FALSE)
     {
       MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
     }

     /* Early Return */
     return;
  }

  sms_cfg_event_info_buf = sys_malloc(sizeof(wms_cfg_event_info_s_type));
  if(sms_cfg_event_info_buf == NULL)
  {
    MSG_ERROR("WMSAPP: cfg_event_buf = NULL",0,0,0);
    return;
  }

  (void)MEMCPY(sms_cfg_event_info_buf, pInfo, sizeof(wms_cfg_event_info_s_type));


  switch(event)
  {
    case WMS_CFG_EVENT_GW_READY:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_GW_READY",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_CFG_GW_READY,
                         0, (uint32) sms_cfg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_CFG_EVENT_CDMA_READY:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_CDMA_READY",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_CFG_CDMA_READY,
                         0, (uint32) sms_cfg_event_info_buf);

      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }

      break;

    case WMS_CFG_EVENT_ROUTES:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_ROUTES",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_CFG_ROUTES,
                         0, (uint32) sms_cfg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;


    case WMS_CFG_EVENT_MEMORY_STATUS:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_MEMORY_STATUS",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_CFG_MEMORY_STATUS,
                         0, (uint32) sms_cfg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_CFG_EVENT_MEMORY_FULL:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_MEMORY_FULL",0,0,0);
       nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_CFG_MEMORY_FULL,
                         0, (uint32) sms_cfg_event_info_buf);
       if(nRetVal == FALSE)
       {
         MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
       }
      break;

    case WMS_CFG_EVENT_MESSAGE_LIST:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_MESSAGE_LIST",0,0,0);
       nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_CFG_MESSAGE_LIST,
                         0, (uint32) sms_cfg_event_info_buf);
       if(nRetVal == FALSE)
       {
         MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
       }
       break;

    case WMS_CFG_EVENT_GW_DOMAIN_PREF:
    MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_GW_DOMAIN_PREF",0,0,0);
       nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_CFG_GW_DOMAIN_PREF,
                         0, (uint32) sms_cfg_event_info_buf);
       if(nRetVal == FALSE)
       {
         MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
       }
       break;

    case WMS_CFG_EVENT_PRIMARY_CLIENT_SET:
      MSG_HIGH("WMSAPP_CfgCb: Posting WMS_CFG_EVENT_PRIMARY_CLIENT_SET",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_CFG_PRIMARY_CLIENT_SET,
                        0, (uint32) sms_cfg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_CFG_EVENT_MEMORY_STATUS_SET:
    case WMS_CFG_EVENT_LINK_CONTROL:
      MSG_HIGH("WMSAPP_CfgCb: WMS_CFG_EVENT_LINK_CONTROL | WMS_CFG_EVENT_MEMORY_STATUS_SET",0,0,0);
      WMSAPPU_FREE(sms_cfg_event_info_buf);
      break;

    default:
      MSG_ERROR("Invalid cfg event %d", event, 0,0);
      WMSAPPU_FREE(sms_cfg_event_info_buf);
      break;
  }

} /* WMSAPP_CfgCb*/


/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_MsgCb

DESCRIPTION
  Callback function for all the MSG related events

DEPENDENCIES
  None

SIDE EFFECTS
  Posts the Event to itself

===========================================================================*/
void WMSAPP_MsgCb
(
wms_msg_event_e_type         event,
wms_msg_event_info_s_type    *pInfo,
boolean                      *shared
)
{
  wms_msg_event_info_s_type *sms_msg_event_info_buf=NULL;
  IShell *shell_ptr = AEE_GetShell();
  uint8 nRetVal; 

  MSG_HIGH("WMSAPP_MsgCb() with event: %d",event,0,0);
  
  if(shell_ptr == NULL)
  {
    MSG_ERROR("WMSAPP: shell_ptr = NULL",0,0,0);
    return;
  }

  sms_msg_event_info_buf = sys_malloc(sizeof(wms_msg_event_info_s_type));
  if(sms_msg_event_info_buf == NULL)
  {
    MSG_ERROR("WMSAPP: msg_event_buf = NULL",0,0,0);
    return;
  }

  (void)MEMCPY(sms_msg_event_info_buf, pInfo, sizeof(wms_msg_event_info_s_type));

  switch(event)
  {
    case WMS_MSG_EVENT_SEND:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_SEND !", 0,0,0);
      sms_msg_event_info_buf->status_info.alpha_id.data = (uint8 *)sys_malloc(WMS_ALPHA_ID_MAX);
      sms_msg_event_info_buf->status_info.alpha_id.len =
               pInfo->status_info.alpha_id.len;
     if ( (sms_msg_event_info_buf->status_info.alpha_id.data != NULL) 
          && (sms_msg_event_info_buf->status_info.alpha_id.len > 0) )
     {
       (void)MEMCPY(sms_msg_event_info_buf->status_info.alpha_id.data,
              pInfo->status_info.alpha_id.data,
              pInfo->status_info.alpha_id.len);
     }
       
     nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_SEND,
                         0, (uint32) sms_msg_event_info_buf);
     if(nRetVal == FALSE)
     {
       MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
     }
     break;

    case WMS_MSG_EVENT_ACK:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_ACK !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_ACK,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_READ:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_READ !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_MSG_READ,
                         0, (uint32) sms_msg_event_info_buf);
      break;

    case WMS_MSG_EVENT_WRITE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_WRITE !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_WRITE,
                         0, (uint32) sms_msg_event_info_buf);
      break;

    case WMS_MSG_EVENT_DELETE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DELETE !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_DELETE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_MODIFY_TAG:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_MODIFY_TAG !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_MSG_MODIFY_TAG,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_READ_TEMPLATE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_READ_TEMPLATE !", 0,0,0);
      if( pInfo->status_info.message.msg_hdr.message_mode==WMS_MESSAGE_MODE_GW)
      {
        sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data = (uint8 *)sys_malloc(WMS_ALPHA_ID_MAX);
        sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.len =
               pInfo->status_info.message.u.gw_template.alpha_id.len;
        if ( (sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data != NULL) 
             && (sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.len > 0) )
        {
          (void)MEMCPY(sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data,
               pInfo->status_info.message.u.gw_template.alpha_id.data,
               pInfo->status_info.message.u.gw_template.alpha_id.len);
        }
      }
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_READ_TEMPLATE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_WRITE_TEMPLATE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_WRITE_TEMPLATE !", 0,0,0);
      if( pInfo->status_info.message.msg_hdr.message_mode==WMS_MESSAGE_MODE_GW)
      {
        sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data = (uint8 *)sys_malloc(WMS_ALPHA_ID_MAX);
        sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.len =
               pInfo->status_info.message.u.gw_template.alpha_id.len;
        if ( (sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data != NULL) 
             && (sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.len > 0) )
        {
          (void)MEMCPY(sms_msg_event_info_buf->status_info.message.u.gw_template.alpha_id.data,
               pInfo->status_info.message.u.gw_template.alpha_id.data,
               pInfo->status_info.message.u.gw_template.alpha_id.len);
        }
      }
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_WRITE_TEMPLATE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_DELETE_TEMPLATE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DELETE_TEMPLATE !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_DELETE_TEMPLATE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_RECEIVED_MESSAGE !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_RECEIVED_MESSAGE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_SUBMIT_REPORT:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_SUBMIT_REPORT !", 0,0,0);
      sms_msg_event_info_buf->submit_report_info.alpha_id.data = (uint8 *)sys_malloc(WMS_ALPHA_ID_MAX);
      sms_msg_event_info_buf->submit_report_info.alpha_id.len =
               pInfo->submit_report_info.alpha_id.len;
      if ( (sms_msg_event_info_buf->submit_report_info.alpha_id.data != NULL) 
          && (sms_msg_event_info_buf->submit_report_info.alpha_id.len > 0) )
      {
        (void)MEMCPY(sms_msg_event_info_buf->submit_report_info.alpha_id.data,
               pInfo->submit_report_info.alpha_id.data,
               pInfo->submit_report_info.alpha_id.len);
      }
      nRetVal = ISHELL_PostEventEx(shell_ptr,EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_MSG_SUBMIT_REPORT,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_STATUS_REPORT:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_STATUS_REPORT !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_STATUS_REPORT,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_DELETE_ALL:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DELETE_ALL !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_DELETE_ALL,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_DELETE_TEMPLATE_ALL:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DELETE_TEMPLATE_ALL !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_DELETE_TEMPLATE_ALL,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_READ_STS_REPORT:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_READ_STS_REPORT !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_READ_STATUS_REPORT,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_DELETE_STS_REPORT:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DELETE_STS_REPORT !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP, EVT_WMS_MSG_DELETE_STATUS_REPORT,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_MSG_EVENT_ACK_REPORT:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_ACK_REPORT !", 0,0,0);
      WMSAPPU_FREE(sms_msg_event_info_buf);
      break;

    case WMS_MSG_EVENT_DUPLICATE_CB_PAGE:
      MSG_HIGH("WMSAPP_MsgCb: Posting WMS_MSG_EVENT_DUPLICATE_CB_PAGE !", 0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP, EVT_WMS_MSG_DUPLICATE_CB_PAGE,
                         0, (uint32) sms_msg_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    default:
      MSG_ERROR("Invalid msg event %d", event, 0,0);
      WMSAPPU_FREE(sms_msg_event_info_buf);
  } // switch


} /* WMSAPP_MsgCb() */

void WMSAPP_DcCb
(
  wms_dc_event_e_type         event,
  wms_dc_event_info_s_type    *info_ptr
)
{
  wms_dc_event_info_s_type *pDcEventInfoBuf = NULL;
  IShell *shell_ptr = AEE_GetShell();
  uint8 nRetVal;
  
  MSG_HIGH("WMSAPP_DcCb()",0,0,0);
  
  if(shell_ptr == NULL)
  {
    MSG_ERROR("WMSAPP: shell_ptr = NULL",0,0,0);
    return;
  }

  pDcEventInfoBuf = sys_malloc(sizeof(wms_dc_event_info_s_type));
  if(pDcEventInfoBuf == NULL)
  {
    MSG_ERROR("WMSAPP: dc event info buf = NULL", 0, 0, 0);
    return;
  }

  (void) MEMCPY(pDcEventInfoBuf, info_ptr, sizeof(wms_dc_event_info_s_type));

  switch(event)
  {
    case WMS_DC_EVENT_CONNECTED:
      MSG_HIGH("WMSAPP_DcCb():WMS_DC_EVENT_CONNECTED",0,0,0);
      if((pDcEventInfoBuf->so != WMS_DC_SO_6) &&
         (pDcEventInfoBuf->so != WMS_DC_SO_14))
      {
        MSG_MED("NOT a SMS call", 0, 0, 0);
      }
      else
      {
        nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                          EVT_WMS_DC_CONNECTED, 0, 0);
        if(nRetVal == FALSE)
        {
          MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
        }
      }
      WMSAPPU_FREE(pDcEventInfoBuf);
      break;

    case WMS_DC_EVENT_ABORTED:
      MSG_HIGH("WMSAPP_DcCb():WMS_DC_EVENT_ABORTED",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DC_ABORTED, 0, 0);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      WMSAPPU_FREE(pDcEventInfoBuf);
      break;

    case WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT:
      MSG_HIGH("WMSAPP_DcCb():WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DC_ENABLE_AUTO_DISCONNECT,
                         0, 0);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      WMSAPPU_FREE(pDcEventInfoBuf);
      break;

    case WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT:
      MSG_HIGH("WMSAPP_DcCb():WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC,AEECLSID_WMSAPP,
                         EVT_WMS_DC_DISABLE_AUTO_DISCONNECT,
                         0, 0);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      WMSAPPU_FREE(pDcEventInfoBuf);
      break;

    case WMS_DC_EVENT_INCOMING:
    case WMS_DC_EVENT_DISCONNECTED:
    case WMS_DC_EVENT_CONNECTING:
    default:
      WMSAPPU_FREE(pDcEventInfoBuf);
      break;
  }
} /* WMSAPP_DcCb */

/*=============================================================================
FUNCTION: WmsApp_GetSecText

DESCRIPTION: Gets the static control for the secondary display.  Creates
  the control if it does not already exist.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean:  TRUE if the text pointer is now available; FALSE otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean WmsApp_GetSecText(CWMSAPP *pMe)
{
  int nCreateStatus;
  AEERect  sStaticRect;        // Area where the Static object will display
  AEEDeviceInfo sDevInfo;      // Includes size of secondary LCD

  if (NULL != pMe->m_pStaticSecText)
  {
    // if the pointer already exists, just return TRUE
    return TRUE;
  }

  if (NULL == pMe->m_pIDisplay2)
  {
    // If we don't have a secondary display, return FALSE
    return FALSE;
  }

  // Create the static text object
  IDISPLAY_MakeDefault(pMe->m_pIDisplay2);
  (void) IDISPLAY_SetColor(pMe->m_pIDisplay2, CLR_USER_BACKGROUND, RGB_WHITE);

  // Create Static control for secondary display text
  nCreateStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                        (void **)&pMe->m_pStaticSecText);
  IDISPLAY_MakeDefault(pMe->a.m_pIDisplay);
  if ((CheckAEEReturnStatus(nCreateStatus) == FALSE) ||
      (pMe->m_pStaticSecText == NULL))
  {
     MSG_ERROR("Unable to create secondary text static control: %d",
               nCreateStatus, 0, 0);
     pMe->m_pStaticSecText = NULL;
     return FALSE;
  }
  else
  {
    /* We want to center the text in the middle of the screen, so
     * we need to set its display rectangle correctly.
     */
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &sDevInfo);
    sStaticRect.x = 0;
    sStaticRect.y = 0;
    sStaticRect.dx = (int16)(sDevInfo.cxAltScreen);
    sStaticRect.dy = (int16)(sDevInfo.cyAltScreen);
    ISTATIC_SetRect(pMe->m_pStaticSecText, &sStaticRect);
    ISTATIC_SetProperties(pMe->m_pStaticSecText, ST_CENTERTEXT | ST_MIDDLETEXT);
  }

  return TRUE;

}

/*=============================================================================
FUNCTION: WMSAPP_UpdateSecondaryDisplay

DESCRIPTION: Updates the secondary display based on the inputs

PARAMETERS:
  *pMe: WmsApp object pointer

RETURN VALUE:
  None

COMMENTS:
  This routine updates the display based on the m_SecDispayState member of the
  WmsApp structure.  As a result, that member should be updated as
  necessary before invoking this function.

SIDE EFFECTS:

SEE ALSO:


=============================================================================*/
void WMSAPP_UpdateSecondaryDisp(CWMSAPP *pMe)
{

  AECHAR      *pszSecText;               // Text to display on secondary
  AECHAR      *pszScratch;               // Scratchpad for constructing text
  int          nStrlen;                  // Scratch strlen value


  // If there's no secondary display
  if (NULL == pMe->m_pIDisplay2)
  {
    return;
  }

  // Make sure the text object exists
  if (! WmsApp_GetSecText(pMe))
  {
    MSG_ERROR("WMSAPP: No IStatic for secondary display strings", 0, 0, 0);
    return;
  }

  pszSecText = (AECHAR *)sys_malloc((NOTIFY_STRING_SEC_DISPLAY) *
                                sizeof(AECHAR));
  pszScratch = (AECHAR *)sys_malloc((NOTIFY_STRING_SEC_DISPLAY) *
                                  sizeof(AECHAR));

  if ((NULL == pszSecText) || (NULL == pszScratch))
  {
    WMSAPPU_FREE(pszSecText);
    WMSAPPU_FREE(pszScratch);
    MSG_ERROR("WMSAPP: No memory for secondary display strings", 0, 0, 0);
    return;
  }

  switch (pMe->m_secDisplayState)
  {
  case WMSAPP_NEW_MESSAGE:
      // Load the "New Message" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NEW_MESSAGE_SEC_DISPLAY, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NEW MESSAGE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NEW_VOICEMAIL:
      // Load the "New Voicemail" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NEW_VMN_SEC_DISPLAY, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY *
                                  sizeof(AECHAR)));

      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NEW VOICEMAIL: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NEW_VOICEMAIL_LINE2:
      // Load the "New Voicemail Line2" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NEW_VMN_LINE2, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY *
                                  sizeof(AECHAR)));

      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NEW VMN LINE2: Redrawing the secondary display", 0, 0, 0);
      break;


    case WMSAPP_MEMORY_FULL:
      // Load the "Memory Full" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_OUT_OF_MEMORY, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY *
                                  sizeof(AECHAR)));

      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: MEMORY FULL: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NEW_MESSAGE_BROADCAST:
      // Load the "New Broadcast Message" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NEW_MESSAGE_BROADCAST, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NEW BROADCAST MESSAGE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_SENDING_MESSAGE:
      // Load the "SENDING MESSAGE" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_MESSAGE_SENDING, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: SENDING MESSAGE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_MESSAGE_SENT:
      // Load the "SENDING MESSAGE" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_MESSAGE_SENT, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: SENDING MESSAGE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_MESSAGE_FAILED_LL:
      // Load the "MSG FAILED IN LL" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_MSG_FAILED_LL_ERROR, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: MSG FAILED IN LL: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NETWORK_NOT_READY:
      // Load the "NETWORK_NOT_READY" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NETWORK_NOT_READY, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NETWORK_NOT_READY: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_PHONE_NOT_READY:
      // Load the "PHONE_NOT_READY" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_PHONE_NOT_READY, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: PHONE_NOT_READY: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NO_TL_ACK:
      // Load the "NO_TL_ACK" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NO_TL_ACK, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: NO_TL_ACK: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_TL_ERROR:
      // Load the "NO_TL_ERROR" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_TL_ERROR, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: TL_ERROR: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_MSG_TOO_LARGE:
      // Load the "MSG_TOO_LARGE" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_MSG_TOO_LARGE, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
      MSG_HIGH("WMSAPP: MSG_TOO_LARGE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_NOT_ALLOWED_IN_ANALOG_MODE:
      // Load the "NOT_ALLOWED_IN_ANALOG_MODE" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_NOT_ALLOWED_IN_ANALOG_MODE, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
          MSG_HIGH("WMSAPP: NOT_ALLOWED_IN_ANALOG_MODE: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_CANNOT_SEND_BROADCAST_MESSAGE:
      // Load the "CANNOT_SEND_BROADCAST_MESSAGE" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_CANNOT_SEND_BROADCAST_MESSAGE, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
          MSG_HIGH("WMSAPP: CANNOT_SEND_BROADCAST_MESSAGE: Redrawing the secondary display", 0, 0, 0);
      break;

   case WMSAPP_CP_ERROR:
      // Load the "CP_ERROR" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_CP_ERROR, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
          MSG_HIGH("WMSAPP: CP_ERROR: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_RPT_RP_ERROR:
      // Load the "RPT_RP_ERROR" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_RP_ERROR, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
       MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
           MSG_HIGH("WMSAPP: RPT_RP_ERROR: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_TR1_TIMEOUT:
      // Load the "RPT_RP_ERROR" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_TR1_TIMEOUT, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
          MSG_HIGH("WMSAPP: TR1_TIMEOUT: Redrawing the secondary display", 0, 0, 0);
      break;

    case WMSAPP_TR2_TIMEOUT:
      // Load the "RPT_RP_ERROR" string
      nStrlen = ISHELL_LoadResString(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                 IDS_TR2_TIMEOUT, pszSecText,
                                 (NOTIFY_STRING_SEC_DISPLAY * sizeof(AECHAR)));
      MSG_HIGH("WMSAPP: LoadResString # of characters loaded: %d", nStrlen, 0, 0);
          MSG_HIGH("WMSAPP: TR2_TIMEOUT: Redrawing the secondary display", 0, 0, 0);
      break;

  }
  // String has been constructed, now display it
  ISTATIC_SetProperties(pMe->m_pStaticSecText, ST_CENTERTEXT | ST_MIDDLETEXT);
  (void) ISTATIC_SetText(pMe->m_pStaticSecText, NULL, pszSecText, AEE_FONT_NORMAL,
                  AEE_FONT_NORMAL);

  if( (pMe->m_secDisplayState == WMSAPP_NEW_MESSAGE )
   || (pMe->m_secDisplayState == WMSAPP_NEW_VOICEMAIL)
   || (pMe->m_secDisplayState == WMSAPP_NEW_VOICEMAIL_LINE2) 
   || (pMe->m_secDisplayState == WMSAPP_NEW_MESSAGE_BROADCAST) )
  { 
#ifdef FEATURE_ANNUNCIATOR
    (void)IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG, ANNUN_STATE_ON);
#if MIN_BREW_VERSION(3,1)
    if(IANNUNCIATOR_Redraw(pMe->m_pIAnn) != SUCCESS)
      MSG_HIGH("Could not draw the secondary display annunciator",0,0,0);
#endif
#endif /* FEATURE_ANNUNCIATOR */
  }

  (void)ISTATIC_Redraw(pMe->m_pStaticSecText);

  WMSAPPU_FREE(pszSecText);

  return;

}


/*===========================================================================
FUNCTION WMSAPP_SoundAlert

DESCRIPTION
  It obtains the current Alert selection and sounds the current
  Sound Alert if allowed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPP_SoundAlert
(
  CWMSAPP *pMe
)
{
  uint8 cfgVal;
  boolean inCall = FALSE;

  /* Beep Alert */
  (void)ICONFIG_GetItem(pMe->m_pIConfig,CFGI_SMS_ALERT_SOUND, &cfgVal, 1);
  if (cfgVal == OEMNV_SMS_ALERT_SOUND_ENABLE)
  {
    pMe->m_bBeepAlert = TRUE;
  }
  else
  {
    pMe->m_bBeepAlert = FALSE;
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

  /* Check for In Call for Sounding Alert */
#ifdef FEATURE_ICM
  if (AEECM_IS_VOICECALL_PRESENT(pMe->m_pICM))
  {
    inCall = TRUE;
  }
#else /* FEATURE_ICM */
  {
    uint8 i;
    /* Check for all the Calls - SMS or not */
    for (i=0; i<CM_CALL_ID_MAX; i++)
    {
      if (ui_co_is_in_call(i) == TRUE)
      {
        if (uiscall_get_call_type(i) != CM_CALL_TYPE_SMS)
        {
          inCall = TRUE;
        }
      }
    }
    //inCall = ui_is_in_call();
  }
#endif /* FEATURE_ICM */

#ifdef FEATURE_QVPHONE
  if (!inCall)
  {
    inCall = AEEVP_IS_VIDEOCALL_CONNECTED(pMe->m_pIVideoPhone);
  }
#endif /* FEATURE_QVPHONE */

  /* Sound Alert if correct settings and not in call */
  if ((pMe->m_bBeepAlert == TRUE) && (pMe->m_BeepVolume != 0) && (!inCall))
  {
    uisnd_snd( UI_ALERT_SND, SND_MESSAGE_ALERT );
  }

} /* WMSAPP_SoundAlert */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_ProcessStatus

DESCRIPTION
  Processes the report information received from WMS

DEPENDENCIES
  None

SIDE EFFECTS
  Message Status is Displayed

===========================================================================*/
boolean WMSAPP_ProcessStatus
(
  CWMSAPP *pMe,
  const wms_submit_report_info_s_type  *pRptInfo
)
{
  uint16 wMsgDisplay;
  char *text;

  switch(pRptInfo->report_status)
  {
    /* Success: submit_report_ack info is available for GSM/WCDMA
    */
    case WMS_RPT_OK:
#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */
      wMsgDisplay =  IDS_MESSAGE_SENT;
      pMe->m_secDisplayState = WMSAPP_MESSAGE_SENT;
      break;

    /* Lower layer errors;
    ** For GSM/WCDMA: cause_value = LL Cause, not available to clients.
    */
    case WMS_RPT_LL_ERROR:
      wMsgDisplay = IDS_MSG_FAILED_LL_ERROR;
      pMe->m_secDisplayState = WMSAPP_MESSAGE_FAILED_LL;
      break;

    case WMS_RPT_OUT_OF_RESOURCES:
    case WMS_RPT_SMR_NO_RESOURCES:
      wMsgDisplay = IDS_OUT_OF_MEMORY;
      pMe->m_secDisplayState = WMSAPP_MEMORY_FULL;
      break;


    case WMS_RPT_NETWORK_NOT_READY:
      wMsgDisplay = IDS_NETWORK_NOT_READY;
      pMe->m_secDisplayState = WMSAPP_NETWORK_NOT_READY;
      break;

    case WMS_RPT_PHONE_NOT_READY:
      wMsgDisplay = IDS_PHONE_NOT_READY;
      pMe->m_secDisplayState = WMSAPP_PHONE_NOT_READY;
      break;

    case WMS_RPT_NO_ACK:
      wMsgDisplay = IDS_NO_TL_ACK;
      pMe->m_secDisplayState = WMSAPP_NO_TL_ACK;
      break;

    case WMS_RPT_CDMA_TL_ERROR:
      wMsgDisplay = IDS_TL_ERROR;
      pMe->m_secDisplayState = WMSAPP_TL_ERROR;
      break;

    case WMS_RPT_ACCESS_TOO_LARGE:
      if(pMe->m_bAccessChannelTest) {
        wMsgDisplay = IDS_ACCESS_CHANNEL_TOO_LARGE;
        break;
      }
      if(pMe->m_bAutoDcSend == TRUE)
      {
        pMe->m_bDCOrig = TRUE;
        if (WMSAPPMN_DcConnect(pMe, pMe->m_AutoDcSendSO))
        {
          return TRUE;
        }
        /* Fall Through */
      }
      /* Fall through */

    case WMS_RPT_DC_TOO_LARGE:
      wMsgDisplay = IDS_MSG_TOO_LARGE;
      pMe->m_secDisplayState = WMSAPP_MSG_TOO_LARGE;
      break;

    case WMS_RPT_NOT_ALLOWED_IN_AMPS:
      wMsgDisplay = IDS_NOT_ALLOWED_IN_ANALOG_MODE;
      pMe->m_secDisplayState = WMSAPP_NOT_ALLOWED_IN_ANALOG_MODE;
      break;

    case WMS_RPT_CANNOT_SEND_BROADCAST:
      wMsgDisplay = IDS_CANNOT_SEND_BROADCAST_MESSAGE;
      pMe->m_secDisplayState = WMSAPP_CANNOT_SEND_BROADCAST_MESSAGE;
      break;

    case WMS_RPT_RP_ERROR:
      wMsgDisplay = IDS_RP_ERROR;
      pMe->m_secDisplayState = WMSAPP_RPT_RP_ERROR;
      break;

    case WMS_RPT_CP_ERROR:
      wMsgDisplay = IDS_CP_ERROR;
      pMe->m_secDisplayState = WMSAPP_CP_ERROR;
      break;

    case WMS_RPT_SMR_TR1M_TIMEOUT:
      wMsgDisplay = IDS_TR1_TIMEOUT;
      pMe->m_secDisplayState = WMSAPP_TR1_TIMEOUT;
      break;

    case WMS_RPT_SMR_TR2M_TIMEOUT:
      wMsgDisplay = IDS_TR2_TIMEOUT;
      pMe->m_secDisplayState = WMSAPP_TR2_TIMEOUT;
      break;

     /* Any other error */
    default:
      wMsgDisplay = IDS_MSG_FAILED;
      pMe->m_secDisplayState = WMSAPP_MSG_FAILED;
      break;
  }
#ifdef FEATURE_CDSMS
  /* Check if a dedicated traffic channel is up */
  if((pRptInfo->message_mode == WMS_MESSAGE_MODE_CDMA) &&
     pMe->m_bDCOrig && pMe->m_bAutoDcSend)
  {
    /* reset the flag */
    pMe->m_bDCOrig = FALSE;

    /* disconnect the traffic channel */
    (void)WMSAPPMN_DcDisconnect(pMe);
  }
#endif /* FEATURE_CDSMS */

  WMSAPP_SoundAlert(pMe);

  /* cause code display */
  if ( ( (pRptInfo->message_mode == WMS_MESSAGE_MODE_CDMA) && (pRptInfo->report_status == WMS_RPT_CDMA_TL_ERROR) ) ||
       ( (pRptInfo->message_mode == WMS_MESSAGE_MODE_GW) && (pRptInfo->report_status == WMS_RPT_CP_ERROR || pRptInfo->report_status == WMS_RPT_RP_ERROR) ) )
  {
    if (NULL != (text = sys_malloc(NOTIFY_STRING_LEN)))
    {
      if (pRptInfo->message_mode == WMS_MESSAGE_MODE_CDMA)
      {
         (void)STRLCPY(text, " Rejected by CDMA Network:\n", NOTIFY_STRING_LEN);
      }
      else if (pRptInfo->message_mode == WMS_MESSAGE_MODE_GW)
      {
         (void)STRLCPY(text, " Rejected by GSM/WCDMA Network:\n", NOTIFY_STRING_LEN);
      }

      (void)STRLCAT(text, WMSAPPU_CauseCodeEnumToString(pRptInfo), NOTIFY_STRING_LEN);

      if (!WMSAPPMN_DisplayMsgDlg(pMe, 0, text, -1))
      {
         MSG_ERROR("WMSAPPMN_DisplayMsgDlg Failed",0,0,0);
      }
      WMSAPPU_FREE(text);
    }
    WMSAPP_UpdateSecondaryDisp(pMe);
    return TRUE;
  }
  else
  {
    WMSAPP_UpdateSecondaryDisp(pMe);
    if (WMSAPPMN_DisplayMsgDlg(pMe, wMsgDisplay, NULL, DIALOG_DISPLAY_TIME_LONGER))
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_BcMmCb

DESCRIPTION
  Callback Function for Multimode SMS Broadcast Events

DEPENDENCIES
  None

SIDE EFFECTS
  Posts the Events to itself

===========================================================================*/
void WMSAPP_BcMmCb( wms_bc_mm_event_e_type       event,
                    wms_bc_mm_event_info_s_type  *pInfo )
{
  wms_bc_mm_event_info_s_type *sms_bc_mm_event_info_buf=NULL;
  IShell *shell_ptr = AEE_GetShell();
  uint8 nRetVal; 

  MSG_HIGH("WMSAPP_BcMmCb()",0,0,0);
 
  if(shell_ptr == NULL)
  {
    MSG_ERROR("WMSAPP: Shell_ptr = NULL in BcMmCb",0,0,0);
    return;
  }

  sms_bc_mm_event_info_buf = sys_malloc(sizeof(wms_bc_mm_event_info_s_type));
  if(sms_bc_mm_event_info_buf == NULL)
  {
    MSG_ERROR("WMSAPP: bc_mm_event_buf = NULL",0,0,0);
    return;
  }

  (void)MEMCPY(sms_bc_mm_event_info_buf, pInfo, sizeof(wms_bc_mm_event_info_s_type));

  MSG_HIGH("WMS APP got MM BC event %d", event, 0, 0);

  switch (event)
  {
    case WMS_BC_MM_EVENT_CONFIG:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_CONFIG",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_CONFIG, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_PREF:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_PREF",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_PREF, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_TABLE:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_TABLE",0,0,0);
      nRetVal = TRUE;
      if (pInfo->u.bc_mm_table.size > 0)
      {
        /* Copy over the Entries */
        sms_bc_mm_event_info_buf->u.bc_mm_table.entries =
          sys_malloc(pInfo->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));
        if (NULL != sms_bc_mm_event_info_buf->u.bc_mm_table.entries)
        {
          (void) MEMCPY (sms_bc_mm_event_info_buf->u.bc_mm_table.entries,
                         pInfo->u.bc_mm_table.entries,
                         pInfo->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));
        }
        else
        {
          nRetVal = FALSE;
        }
      }
      else
      {
         sms_bc_mm_event_info_buf->u.bc_mm_table.entries = NULL;
      }

      if(nRetVal == FALSE)
      {
        MSG_HIGH("Malloc Failed!", 0,0,0);
      }
      else
      {
        nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                            EVT_WMS_BC_MM_EVENT_TABLE, 0,
                            (uint32) sms_bc_mm_event_info_buf);
        if(nRetVal == FALSE)
        {
          MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
        }
      }
      break;

    case WMS_BC_MM_EVENT_SRV_IDS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_SRV_IDS ",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_SRV_IDS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_SRV_INFO:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_SRV_INFO",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_SRV_INFO, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_ADD_SRVS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_ADD_SRVS",0,0,0);
      nRetVal = TRUE;
      if (pInfo->u.bc_mm_table.size > 0)
      {
        /* Copy over the Entries */
        sms_bc_mm_event_info_buf->u.bc_mm_table.entries =
          sys_malloc(pInfo->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));
        if (NULL != sms_bc_mm_event_info_buf->u.bc_mm_table.entries)
        {
          (void) MEMCPY (sms_bc_mm_event_info_buf->u.bc_mm_table.entries,
                 pInfo->u.bc_mm_table.entries,
                 pInfo->u.bc_mm_table.size * sizeof(wms_bc_mm_service_info_s_type));
        }
        else
        {
          nRetVal = FALSE;
        }
      }
      else
      {
         sms_bc_mm_event_info_buf->u.bc_mm_table.entries = NULL;
      }

      if(nRetVal == FALSE)
      {
        MSG_HIGH("Malloc Failed!", 0,0,0);
      }
      else
      {
        nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_ADD_SRVS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
        if(nRetVal == FALSE)
        {
          MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
        }
      }
      break;

    case WMS_BC_MM_EVENT_DELETE_SRVS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_DELETE_SRVS",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_DELETE_SRVS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_SRV_UPDATED:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_SRV_UPDATED",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_SRV_UPDATED, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_SELECT_ALL_SRVS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_SELECT_ALL_SRVS",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_SELECT_ALL_SRVS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_DELETE_ALL_SRVS:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_DELETE_ALL_SRVS",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_DELETE_ALL_SRVS, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_READING_PREF:
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_READING_PREF",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_READING_PREF, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_ENABLE_FAILURE:  /* lower layer (CM & CP) failure */
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_ENABLE_FAILURE",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_ENABLE_FAILURE, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    case WMS_BC_MM_EVENT_DISABLE_FAILURE: /* lower layer (CM & CP) failure */
      MSG_HIGH("WMSAPP_BcMmCb(): WMS_BC_MM_EVENT_DISABLE_FAILURE",0,0,0);
      nRetVal = ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                         EVT_WMS_BC_MM_EVENT_DISABLE_FAILURE, 0,
                         (uint32) sms_bc_mm_event_info_buf);
      if(nRetVal == FALSE)
      {
        MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
      }
      break;

    default:
      MSG_HIGH("MM BC event not recognized!!!",0,0,0);
      WMSAPPU_FREE(sms_bc_mm_event_info_buf);
      break;
  }
}
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

/* <EJECT> */
/*===========================================================================

Function:  WMSAPP_InitEMSDirectory()

Description:
    This function initializes the EMS directories for
    Small Picture, Large Picure, Variable Picture,
    Small Animations, Large Animations, Predefined Animations,
    Predefined and User defined Melody

Parameters:
  Pointer to CWMSAPP (pMe)

Return Value:
  TRUE: Indicating that the EMS Directory was Initialized Properly
  FALSE: Failure in Initializing EMS Directory
==============================================================================*/
boolean WMSAPP_InitEMSDirectory
(
 const CWMSAPP *pMe
)
{
  char *dirName = "ems";
  IFileMgr *pIFileMgr;

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                           (void **) &pIFileMgr) != SUCCESS)
  {
    return FALSE;
  }

  // Instance of FileMgr created successfully
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/smallpictures";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/SmallPictures Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/largepictures";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/LargePictures Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/varpictures";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/VarPictures Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/smallanimations";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/SmallAnimations Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/largeanimations";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/LargeAnimations Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/predefanimations";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/PredefAnimations Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/melody";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/Melody Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  dirName = "ems/usermelody";
  if (IFILEMGR_Test( pIFileMgr, dirName) != SUCCESS)
  {
    // EMS/UserMelody Directory Not Present
    // Hence Initialize it
    if (IFILEMGR_MkDir( pIFileMgr, dirName) != SUCCESS)
    {
      // Initializing Failed
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }
  }

  (void)IFILEMGR_Release(pIFileMgr);
  return TRUE;
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

/*===========================================================================

Function:  WMSAPP_InitAppData()

Description:
    This function initializes the app specific data.

Prototype:
  void WMSAPP_InitAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CWMSAPP*, in this case).

Return Value:
  TRUE: If successful
  FALSE: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
boolean WMSAPP_InitAppData(IApplet* po)
{
  CWMSAPP *pMe = (CWMSAPP*) po;
  AEEDeviceInfo di;
  uint8 i=0;
  int nReturnStatus;
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#if defined(FEATURE_CDSMS) && defined(FEATURE_GWSMS)
#error code not present
#endif /*FEATURE_CDSMS && FEATURE_GWSMS */

  pMe->m_pSplashImage = NULL;     /* Initialize Splash Screen image */

  pMe->m_bSuspended = FALSE;      /* Initialize <suspend> to FALSE */
  pMe->m_bDisplayNewMsgAlert = FALSE;

#ifdef FEATURE_ANNUNCIATOR
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_ANNUNCIATOR,(void **) &pMe->m_pIAnn) != SUCCESS)
  {
    MSG_ERROR ("Annunciator Interface could not be created", 0, 0, 0);
    return FALSE;
  }
  MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::wmsapp2222:::",0,0,0);
  //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
#endif /* FEATURE_ANNUNCIATOR */


  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WMS,  (void **) &pMe->m_pwms) != SUCCESS)
  {
    MSG_ERROR ("IWMS could not be created", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_ICM
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLMANAGER,  (void **) &pMe->m_pICM) != SUCCESS)
  {
    MSG_ERROR ("ICM could not be created", 0, 0, 0);
    return FALSE;
  }
#endif /* FEATURE_ICM */

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG, (void **) &pMe->m_pIConfig) != SUCCESS)
  {
    MSG_ERROR ("IConfig could not be created", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_IWMSDIAG
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WMSDIAG, (void **) &pMe->m_pIWmsdiag) != SUCCESS)
  {
    MSG_ERROR ("IWMSDIAG could not be created", 0, 0, 0);
    return FALSE;
  }

  if (ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_WMSAPP, AEECLSID_WMSDIAG_NOTIFIER, NMASK_WMSDIAG_REQ) != SUCCESS)
  {
    MSG_ERROR("ISHELL_RegisterNotify failed!",0,0,0);
    return FALSE;
  }
#endif /* FEATURE_IWMSDIAG */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_QVPHONE
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VIDEOPHONE,  (void **) &pMe->m_pIVideoPhone) != SUCCESS)
  {
    MSG_ERROR ("IVideoPhone could not be created", 0, 0, 0);
    return FALSE;
  }
#endif /* FEATURE_QVPHONE */
  /* Initializing cxScreen and cyScreen */
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  pMe->m_cxScreen = di.cxScreen;
  pMe->m_cyScreen = di.cyScreen;


  /* initializing MenuFrame */
  SETAEERECT(&(pMe->m_MenuFrame),
             0,
             0,
             pMe->m_cxScreen,
             pMe->m_cyScreen);




  pMe->m_msgInfo.memStore = WMS_MEMORY_STORE_NONE;
#ifdef FEATURE_SMS_UDH 
  pMe->m_numofImages = 0;
#endif /* FEATURE_SMS_UDH */
  pMe->m_bEditMsg = FALSE;
  pMe->m_bReply = FALSE;
  pMe->m_bDisplaySplash = FALSE;
  pMe->m_bAutoDcSend = TRUE;
  pMe->m_bDCOrig = FALSE;
  pMe->m_bAutoDcDisconnect = TRUE;
  pMe->m_AutoDcSendSO = WMS_DC_SO_6;
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#if defined(FEATURE_CDSMS) && defined(FEATURE_GWSMS)
#error code not present
#elif defined(FEATURE_CDSMS)
  pMe->m_modeControl = WMSAPP_MODE_CDMA;
#else
  pMe->m_modeControl = WMSAPP_MODE_GW;
#endif /*FEATURE_CDSMS && FEATURE_GWSMS */

  pMe->m_bCDMAReady = FALSE;
  pMe->m_bGWReady   = FALSE;

  /* Register Callbacks */
  (void)IWMS_Init(pMe->m_pwms, WMS_CLIENT_TYPE_WMS_APP, &pMe->m_clientId);
  (void)IWMS_RegisterCfgCb(pMe->m_pwms, pMe->m_clientId, WMSAPP_CfgCb );
  (void)IWMS_RegisterMsgCb(pMe->m_pwms, pMe->m_clientId, WMSAPP_MsgCb );

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  (void)IWMS_RegisterBcMmCb(pMe->m_pwms, pMe->m_clientId, WMSAPP_BcMmCb);
#endif /*FEATURE_BROADCAST_SMS_MULTIMODE*/

#ifdef FEATURE_CDSMS
  (void)IWMS_RegisterDcCb(pMe->m_pwms, pMe->m_clientId, WMSAPP_DcCb);
#endif /* FEATURE_CDSMS */
  (void)IWMS_Activate(pMe->m_pwms,pMe->m_clientId);

  pMe->m_pMsgEventInfo = MALLOC(sizeof(wms_msg_event_info_s_type));

  pMe->m_pMO_template_msg = MALLOC(sizeof(wms_client_message_s_type));
  pMe->m_pClient_msg_mo   = MALLOC(sizeof(wms_client_message_s_type));
  pMe->m_pClient_msg      = MALLOC(sizeof(wms_client_message_s_type));
  pMe->m_pClient_ts       = MALLOC(sizeof(wms_client_ts_data_s_type));
  pMe->m_pClient_ts_mo    = MALLOC(sizeof(wms_client_ts_data_s_type));
  #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
  pMe->m_isslectkey  = FALSE;
  #endif
  /* Initialize other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  /* Initialize other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts_mo->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  pMe->m_AlphaID.len      = 0;
  pMe->m_AlphaID.data     = MALLOC(WMS_ALPHA_ID_MAX);
  (void)MEMSET(pMe->m_AlphaID.data, 0, WMS_ALPHA_ID_MAX);

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */


  for(i=0;i<(uint8)WMS_MEMORY_STORE_MAX;i++)
  {
    pMe->m_pSmsLists[i] = MALLOC(sizeof(wms_message_list_s_type));
    pMe->m_pMemStatuses[i] = MALLOC(sizeof(wms_memory_status_s_type));
  }

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */

  /*One Extra for VoiceMail*/
  pMe->m_pSmsLists[i] = MALLOC(sizeof(wms_message_list_s_type));
  pMe->m_pCDSMSTemplateList = MALLOC(sizeof(wms_message_list_s_type));
  pMe->m_pGWSMSTemplateList = MALLOC(sizeof(wms_message_list_s_type));

  (void)MEMSET(pMe->m_pCDSMSTemplateList, 0, sizeof(wms_message_list_s_type));
  (void)MEMSET(pMe->m_pGWSMSTemplateList, 0, sizeof(wms_message_list_s_type));

  /* Initialize Default Templates to NV till we get SIM and RUIM Template List Events */
  pMe->m_CDSMSDefaultTemplate = 0;  /* Pointing to NV Template */
  pMe->m_GWSMSDefaultTemplate = 0;  /* Pointing to NV Template */

  pMe->m_melody_trans_id  = MELODY_TRANS_ID;

  if (WMSAPP_InitEMSDirectory(pMe) == FALSE)
  {
    MSG_ERROR("EMS Directory Structure Failed to Initialize",0,0,0);
  }
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  /* Boolean to use pMe->m_pUserData or not */
  pMe->m_bUsepUserData = FALSE;

  (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_SMS_SEGMENT_SETTING, &pMe->m_segmentSetting, 1);

  pMe->m_pUserData = MALLOC(sizeof(wmsapp_user_data_s_type));

  pMe->m_pUserData->num_chars = 0;
#ifdef FEATURE_SMS_UDH 
  pMe->m_pUserData->num_headers = 0;

  pMe->m_pUserData->headers = NULL;
#endif /* FEATURE_SMS_UDH */
  pMe->m_pUserData->data = NULL;

  pMe->m_bComposeMessage = FALSE;

  pMe->m_bTestMode        = TRUE;

  /* Setting Primary Client */
  pMe->m_cbData.user_data = (void*)pMe;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));

#ifdef FEATURE_WMSAPP_STORAGE
  (void)IWMS_SetPrimaryClient(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, TRUE, TRUE);
#else
  (void)IWMS_SetPrimaryClient(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, TRUE, FALSE);
#endif /* FEATURE_WMSAPP_STORAGE */


  /* Create Secondary Display */
  pMe->m_pStaticSecText = NULL;

  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                        AEECLSID_DISPLAY2,
                                        (void **)&pMe->m_pIDisplay2);
  if (SUCCESS != nReturnStatus)
  {
    pMe->m_pIDisplay2 = NULL;
    MSG_HIGH("WMSAPP: Failed to initialize the secondary display",0,0,0);
  }

  pMe->m_bAccessChannelTest = FALSE;
  pMe->m_pAccessChannelMsg = MALLOC(sizeof(wms_client_message_s_type));

  /* set default values */
  pMe->m_mailboxRecordNumLine1 = 0x00;

  pMe->m_voicemailNumber = MALLOC(sizeof(wmsapp_voicemail_number_s_type));
  pMe->m_voicemailNumber->digit = NULL;
  pMe->m_voicemailNumber->num_digit = 0;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  return TRUE;
}

/*===========================================================================
Function:  WMSAPP_FreeAppData()

Description:
  This function frees the app data. This function is registered with the
  applet framework when the applet is created (inside AEEClsCreateInstance() function).
  This function is called by the app framework when the reference count of the
  applet reaches zero. This function must free all the app data that has been
  allocated by this app. For ex: if their is data that was remembered when
  the app is suspended and resumed, those data must be freed here.

Prototype:
  void WMSAPP_FreeAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CWMSAPP*, in this case).

Return Value:
  None

Comments:  None

Side Effects: None
==============================================================================*/
void WMSAPP_FreeAppData(IApplet* po)
{
  CWMSAPP * pMe = (CWMSAPP *)po;
  uint8 i = 0;

  if(pMe->m_pSplashImage != NULL)
  {     /* Release Splash image */
    (void)IIMAGE_Release(pMe->m_pSplashImage);
  }

  /* IWMS */
  if(pMe->m_pwms)
  {
    (void)IWMS_Release(pMe->m_pwms);
  }

#ifdef FEATURE_ICM
  /* ICM */
  if(pMe->m_pICM)
  {
    (void)ICM_Release(pMe->m_pICM);
  }
#endif /* FEATURE_ICM */


#ifdef FEATURE_ANNUNCIATOR
  if(pMe->m_pIAnn)
  {
    /* Release annunciator */
    (void)IANNUNCIATOR_Release(pMe->m_pIAnn);
  }
#endif /* FEATURE_ANNUNCIATOR */

  if (pMe->m_pIConfig)
  {
    (void)ICONFIG_Release(pMe->m_pIConfig);
  }

#ifdef FEATURE_QVPHONE
  /* IVideoPhone */
  if(pMe->m_pIVideoPhone)
  {
    (void)IVIDEOPHONE_Release(pMe->m_pIVideoPhone);
  }
#endif /* FEATURE_QVPHONE */


  WMSAPPU_FREE(pMe->m_pMsgEventInfo);

  WMSAPPU_FREE(pMe->m_pClient_msg);

  WMSAPPU_FREE(pMe->m_pClient_msg_mo);

  WMSAPPU_FREE(pMe->m_pMO_template_msg);

  WMSAPPU_FREE(pMe->m_pClient_ts);

  WMSAPPU_FREE(pMe->m_pClient_ts_mo);

  WMSAPPU_FREE(pMe->m_AlphaID.data);

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/

  for(i=0;i<(uint8)WMS_MEMORY_STORE_MAX;i++)
  {
    WMSAPPU_FREE(pMe->m_pSmsLists[i]);
    WMSAPPU_FREE(pMe->m_pMemStatuses[i]);
  }

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */

  /*One Extra for VoiceMail*/
  WMSAPPU_FREE(pMe->m_pSmsLists[i]);

  WMSAPPU_FREE(pMe->m_pCDSMSTemplateList);

  WMSAPPU_FREE(pMe->m_pGWSMSTemplateList);

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

  WMSAPPU_FREE(pMe->m_pUserData);

#ifdef FEATURE_IWMSDIAG
  if (pMe->m_pIWmsdiag)
  {
    (void)IWMSDIAG_Release(pMe->m_pIWmsdiag);
  }
#endif /* FEATURE_IWMSDIAG */

  //free secondary display
  if(pMe->m_pIDisplay2 != NULL) {
          (void)IDISPLAY_Release(pMe->m_pIDisplay2);
          pMe->m_pIDisplay2 = NULL;
  }

  WMSAPPU_FREE(pMe->m_pAccessChannelMsg);

  if ( pMe->m_voicemailNumber )
  {
    WMSAPPU_FREE(pMe->m_voicemailNumber->digit);
    WMSAPPU_FREE(pMe->m_voicemailNumber);
  }
  
#ifdef FEATURE_GWSMS  
#error code not present
#endif /* FEATURE_GWSMS  */

}

static boolean WMSAPP_IsMessagingDaemonDisabled(IShell *pIShell)
{
#ifdef CFGI_MSG_DISABLE_DAEMONS
  boolean cfgMDD = TRUE;
  int     nErr;
  IConfig *pIConfig;

  if(ISHELL_CreateInstance(pIShell, AEECLSID_CONFIG, (void**)&pIConfig) != SUCCESS)
  {
    MSG_ERROR ("IConfig could not be created", 0, 0, 0);
    return TRUE;
  }

  nErr=ICONFIG_GetItem(pIConfig, CFGI_MSG_DISABLE_DAEMONS, &cfgMDD, 1);
  if(AEE_SUCCESS != nErr)
  {
    MSG_ERROR("Failed to get Item CFGI_MSG_DISABLE_DAEMONS", 0, 0, 0);
  }

  ICONFIG_Release(pIConfig);
  return cfgMDD;
#else
  return TRUE;
#endif /* CFGI_MSG_DISABLE_DAEMONS */
}
/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time.

PROTOTYPE:
  int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppApplet must point to a valid IBase object.
  If the classID  represnts an applet, then ppObj must point to a valid AEEApplet structure.Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful.
  EFAILED: Error occurred while creating the class/app. In this case, the app/class will
  not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(BREW_STATIC_APP)
int WMSAPP_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
  *ppObj = NULL;

  //
  // Here a check is done to see if the ClsID is that of WMSAPP app.
  // The reason is if this module has more than one applets or classes, then this function is invoked
  // once for each applet or class. Checking here ensures that the correct IApplet or class object is
  // constructed.
  //
  if (!WMSAPP_IsMessagingDaemonDisabled(pIShell))
  {
    MSG_HIGH("Messaging Daemons Enabled - Failing WMSAPP Launch", 0, 0, 0);
    return EFAILED;
  }

  if(ClsId == AEECLSID_WMSAPP)
  {
    //Create the applet
    if(AEEApplet_New(sizeof(CWMSAPP), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)WMSAPP_HandleEvent,(PFNFREEAPPDATA)WMSAPP_FreeAppData))
    {
      //Initialize applet data
      if(WMSAPP_InitAppData((IApplet*)*ppObj))
      {
        //Data initialized successfully
        return(AEE_SUCCESS);
      }
      else
      {
        //Release the applet. This will free the memory allocated for the applet when
        (void)IAPPLET_Release(((IApplet*)*ppObj));
        *ppObj = NULL;
        return EFAILED;
      }

    }//AEEApplet_New

  }// ClsId == AEECLSID_WMSAPP

  return(EFAILED);
}

/****************************************************************************/
/*                          Functions                                */
/****************************************************************************/
/*===========================================================================

FUNCTION: WMSAPP_Init

DESCRIPTION
   During Initialization, WMSAPP sends a few commands to WMS to update the message
   lists, memory status

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  WMSAPP will receive events in response to all the commands
===========================================================================*/
void WMSAPP_Init
(
  CWMSAPP *pMe
)
{

  pMe->m_cbData.user_data = (void*)pMe;

  CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                (void*)&(pMe->m_cbData));


#ifdef FEATURE_CDSMS
  if( pMe->m_mode == WMS_MESSAGE_MODE_CDMA )
  {
    //IWMS_CfgGetMessageList( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_RAM_CDMA,WMS_TAG_NONE );

#ifdef FEATURE_CDSMS_RUIM
    (void)IWMS_CfgGetMessageList( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_RUIM,WMS_TAG_NONE );

#endif /* FEATURE_CDSMS_RUIM */
    (void)IWMS_CfgGetMessageList( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_NV_CDMA,WMS_TAG_NONE );

#ifdef FEATURE_CDSMS_RUIM
    (void)IWMS_CfgGetMessageList( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_RUIM,WMS_TAG_MO_TEMPLATE );

#endif /* FEATURE_CDSMS_RUIM */

  /* Get memory status
  */
  //(void)IWMS_CfgGetMemoryStatus( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_RAM_CDMA,WMS_TAG_NONE );

#ifdef FEATURE_CDSMS_RUIM
    (void)IWMS_CfgGetMemoryStatus( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_RUIM,WMS_TAG_NONE );

#endif /* FEATURE_CDSMS_RUIM */

    (void)IWMS_CfgGetMemoryStatus( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe,WMS_MEMORY_STORE_NV_CDMA,WMS_TAG_NONE );

#ifdef FEATURE_CDSMS_BROADCAST
    /*
    ** Initialize BC SMS configuration, preferences and service table.
    */
    if (IWMS_BcMmGetConfig(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,NULL, WMS_MESSAGE_MODE_CDMA)
                         != SUCCESS)
    {
      MSG_ERROR("wms_bc_mm_get_config(CDMA): cmd not sent to WMS", 0, 0, 0);
    }

    if (IWMS_BcMmGetPref(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,NULL,WMS_MESSAGE_MODE_CDMA)
                       != SUCCESS)
    {
      MSG_ERROR("wms_bc_mm_get_pref(CDMA): cmd not sent to WMS", 0, 0, 0);
    }

    if (IWMS_BcMmGetTable(pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,NULL,WMS_MESSAGE_MODE_CDMA)
                        != SUCCESS)
    {
      MSG_ERROR("wms_bc_mm_get_table(CDMA): cmd not sent to WMS", 0, 0, 0);
    }
#endif /* FEATURE_CDSMS_BROADCAST */

    /* Set up DC auto disconnect */
    if(IWMS_DcEnableAutoDisconnect(pMe->m_pwms, pMe->m_clientId,
                                  &pMe->m_callback, NULL,
                                  WMSAPP_AUTO_DISCONNECT_TIME)  != SUCCESS)
    {
      MSG_ERROR("dc enable auto disconnect: cmd not sent to WMS",0,0,0);
    }
    pMe->m_bCDMAReady = TRUE;
  }
#endif /* FEATURE_CDSMS */

  /* Get GW message lists
  */
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

 (void)IWMS_CfgGetRoutes( pMe->m_pwms,pMe->m_clientId,&pMe->m_callback,(void*)pMe);

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_UpdateMemoryStatus

DESCRIPTION
  Updates memory status

DEPENDENCIES
  None

SIDE EFFECTS
 None

===========================================================================*/
void WMSAPP_UpdateMemoryStatus
(
const CWMSAPP *pMe,
const wms_memory_status_s_type   *ptr
)
{
  /* Special Handling for Status Report */
  if ((ptr->mem_store == WMS_MEMORY_STORE_SIM)
      && (ptr->tag == WMS_TAG_STATUS_RPT))
  {
    (void)MEMCPY(pMe->m_pStatusReportMemStatus, ptr, sizeof(wms_memory_status_s_type));
  }
  else
  {
    /* Special Handling for RAM Messages if FEATURE_WMSAPP_STORAGE is defined */
#ifdef FEATURE_WMSAPP_STORAGE
    if (ptr->mem_store == WMS_MEMORY_STORE_RAM_GW)
    {
      if ((ptr->free_slots == 0) && (pMe->m_pMemStatuses[WMS_MEMORY_STORE_RAM_GW]->free_slots != 0))
      {
        /* Set Memory Full Flag */
        CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                      (void*)&(pMe->m_cbData));

        (void)IWMS_SetMemoryFull(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, TRUE);
      }
      else if ((ptr->free_slots != 0) && (pMe->m_pMemStatuses[WMS_MEMORY_STORE_RAM_GW]->free_slots == 0))
      {
        /* Reset the Memory Full Flag */
        CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY) WMSAPPMN_ProcessError,
                      (void*)&(pMe->m_cbData));

        (void)IWMS_SetMemoryFull(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, FALSE);
      }
    }
#endif /* FEATURE_WMSAPP_STORAGE */
    if (WMS_MEMORY_STORE_MAX > ptr->mem_store) { 
       (void)MEMCPY(pMe->m_pMemStatuses[ptr->mem_store],ptr,sizeof(wms_memory_status_s_type));
    }
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_UpdateMessageList

DESCRIPTION
  Updates message/template status

DEPENDENCIES
  WMSAPPU_UpdateIcons

SIDE EFFECTS
 None

===========================================================================*/
void WMSAPP_UpdateMessageList
(
CWMSAPP *pMe,
const wms_message_list_s_type   *list_ptr
)
{

  if(list_ptr->tag == WMS_TAG_MO_TEMPLATE)
  {
    if (list_ptr->mem_store == WMS_MEMORY_STORE_SIM)
    {
      /* SIM Template List has Arrived - Read Default Template */
      (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_SMS_GW_DEFAULT_TEMPLATE, &pMe->m_GWSMSDefaultTemplate, 1);

      (void)MEMCPY(pMe->m_pGWSMSTemplateList,list_ptr,sizeof(wms_message_list_s_type));

      /* Validate Default Template Value */
      if (pMe->m_GWSMSDefaultTemplate > pMe->m_pGWSMSTemplateList->len)
      {
        /* Set it to the Template List Length */
        pMe->m_GWSMSDefaultTemplate = (uint8)pMe->m_pGWSMSTemplateList->len;
        (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_GW_DEFAULT_TEMPLATE, &pMe->m_GWSMSDefaultTemplate, 1);
      }
    }
    else if (list_ptr->mem_store == WMS_MEMORY_STORE_RUIM)
    {
      /* RUIM Template List has Arrived - Read Default Template */
      (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_SMS_CDMA_DEFAULT_TEMPLATE, &pMe->m_CDSMSDefaultTemplate, 1);

      (void)MEMCPY(pMe->m_pCDSMSTemplateList,list_ptr,sizeof(wms_message_list_s_type));

      /* Validate Default Template Value */
      if (pMe->m_CDSMSDefaultTemplate > pMe->m_pCDSMSTemplateList->len)
      {
        /* Set it to the Template List Length */
        pMe->m_CDSMSDefaultTemplate = (uint8)pMe->m_pCDSMSTemplateList->len;
        (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_CDMA_DEFAULT_TEMPLATE, &pMe->m_CDSMSDefaultTemplate, 1);
      }
    }
  }
#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/
  else if (list_ptr->tag == WMS_TAG_NONE)
  {
     if ( WMS_MEMORY_STORE_MAX > list_ptr->mem_store) { 
       (void)MEMCPY(pMe->m_pSmsLists[list_ptr->mem_store],list_ptr,sizeof(wms_message_list_s_type));
    }
  }

  WMSAPPU_UpdateIcons(pMe);
}

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPP_UpdateRoutes

DESCRIPTION
  Updates message routes

DEPENDENCIES
  None

SIDE EFFECTS
 None

===========================================================================*/
void WMSAPP_UpdateRoutes
(
  CWMSAPP *pMe,
  const wms_routes_s_type *routes
)
{
   pMe->m_routes = *routes;
}

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

/*===========================================================================
FUNCTION WMSAPPMN_ProcessBcMmEvent

DESCRIPTION
  Processes the Multimode Broadcast Events

RETURN
  boolean, if event was processed correctly.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMN_ProcessBcMmEvent
(
 CWMSAPP * pMe,
 AEEEvent eCode,
 wms_bc_mm_event_info_s_type *event
)
{
  if (event->message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else
  {
#ifdef FEATURE_CDSMS_BROADCAST
    uint8 i=0;
    switch (eCode)
    {
      case EVT_WMS_BC_MM_EVENT_CONFIG:
        pMe->m_bcInfo.config = event->u.bc_config;
        break;

      case EVT_WMS_BC_MM_EVENT_PREF:
        pMe->m_bcInfo.pref = event->u.bc_pref;
        break;

      case EVT_WMS_BC_MM_EVENT_TABLE:
      case EVT_WMS_BC_MM_EVENT_ADD_SRVS:
      case EVT_WMS_BC_MM_EVENT_SELECT_ALL_SRVS:
      case EVT_WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS:
        pMe->m_bcInfo.tableSize = event->u.bc_mm_table.size;
        for(i=0; i<pMe->m_bcInfo.tableSize && i<WMS_BC_TABLE_MAX; i++)
        {
          pMe->m_bcInfo.entries[i].alert = event->u.bc_mm_table.entries[i].alert;
          (void) MEMCPY(pMe->m_bcInfo.entries[i].label, event->u.bc_mm_table.entries[i].label, WMS_BC_MM_SRV_LABEL_SIZE);
          pMe->m_bcInfo.entries[i].label_encoding = event->u.bc_mm_table.entries[i].label_encoding;
          pMe->m_bcInfo.entries[i].max_messages = event->u.bc_mm_table.entries[i].max_messages;
          pMe->m_bcInfo.entries[i].priority = event->u.bc_mm_table.entries[i].priority;
          pMe->m_bcInfo.entries[i].selected = event->u.bc_mm_table.entries[i].selected;
          pMe->m_bcInfo.entries[i].srv_id.bc_srv_id = event->u.bc_mm_table.entries[i].srv_id.bc_srv_id;
        }

        /* Sort the services per language */
        WMSAPPU_BCSortSvcs(pMe);
        WMSAPPU_FREE(event->u.bc_mm_table.entries);
        break;

      case EVT_WMS_BC_MM_EVENT_SRV_IDS:
      case EVT_WMS_BC_MM_EVENT_DELETE_SRVS:
        break;

      case EVT_WMS_BC_MM_EVENT_DELETE_ALL_SRVS:
        pMe->m_bcInfo.tableSize = 0;

        if(pMe->m_bSuspended == FALSE)
        {
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_ALL_SRVS_DELETED, NULL, SELECTIONVERIFICATIONTIME);
        }

        /* set the size of the sorted svcs to 0 */
        for(i=0; i<WMSAPP_MAX_LANGUAGES; i++)
        {
          pMe->m_bcSortedSvcs[i].size = 0;
        }
        break;

      case EVT_WMS_BC_MM_EVENT_SRV_INFO:
      case EVT_WMS_BC_MM_EVENT_SRV_UPDATED:
        for(i=0; i<pMe->m_bcInfo.tableSize && i<WMS_BC_TABLE_MAX; i++)
        {
          /* Find the updated service */
          if((pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.service ==
              event->u.bc_mm_srv_info.srv_id.bc_srv_id.service) &&
            (pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language ==
              event->u.bc_mm_srv_info.srv_id.bc_srv_id.language))
          {
            /* Found the updated BC service; assign and early break */
            pMe->m_bcInfo.entries[i].alert = event->u.bc_mm_srv_info.alert;
            (void) MEMCPY(pMe->m_bcInfo.entries[i].label, event->u.bc_mm_srv_info.label, WMS_BC_MM_SRV_LABEL_SIZE);
            pMe->m_bcInfo.entries[i].label_encoding = event->u.bc_mm_srv_info.label_encoding;
            pMe->m_bcInfo.entries[i].max_messages = event->u.bc_mm_srv_info.max_messages;
            pMe->m_bcInfo.entries[i].priority = event->u.bc_mm_srv_info.priority;
            pMe->m_bcInfo.entries[i].selected = event->u.bc_mm_srv_info.selected;
            pMe->m_bcInfo.entries[i].srv_id.bc_srv_id = event->u.bc_mm_srv_info.srv_id.bc_srv_id;
            break;
          }
        }

        if(pMe->m_bSuspended == FALSE)
        {
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_SRV_UPDATED, NULL, SELECTIONVERIFICATIONTIME);
        }
        break;

      case EVT_WMS_BC_MM_EVENT_ENABLE_FAILURE:  /* lower layer (CM & CP) failure */
      case EVT_WMS_BC_MM_EVENT_DISABLE_FAILURE: /* lower layer (CM & CP) failure */
        break;
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  return TRUE;
}

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

/*===========================================================================
FUNCTION CreateSMSMenuDlg

DESCRIPTION
  It obtains the current menu selection and create the next levelt dialog
  accordingly

DEPENDENCIES
  WMSAPP_SetModeControl

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateSMSMenuDlg(const CWMSAPP *pMe)
{
  int ret_status;

  if (pMe->m_bDisplaySplash && !pMe->m_bDisplayNewMsgAlert)
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                     IDD_SPLASH, NULL);
  }
  else
  {
    ret_status = ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                     IDD_MAINSMS, NULL);
  }

  if(ret_status == EFAILED)
  { /* Creation failed */
    return FALSE;
  }
  return TRUE; /* Created successfully */

} /* CreateSMSMenuDlg */

/*===========================================================================
FUNCTION WMSAPP_IsDisplayAlertAllowed

DESCRIPTION
  It obtains the current Alert selection and decides whether the current
  Alert is allowed to be displayed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPP_IsDisplayAlertAllowed
(
  CWMSAPP *pMe
)
{
  uint8 cfgVal;
  (void)ICONFIG_GetItem(pMe->m_pIConfig,CFGI_SMS_ALERT_DISPLAY, &cfgVal, 1);
  if (cfgVal == OEMNV_SMS_ALERT_DISPLAY_ENABLE)
  {
    pMe->m_bDisplayAlert = TRUE;
  }
  else
  {
    pMe->m_bDisplayAlert = FALSE;
  }

  return pMe->m_bDisplayAlert;
} /* WMSAPP_IsDisplayAlertAllowed */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION WMSAPP_SendMOAccessChannelMsg

DESCRIPTION
  Function to send CDMA Message on Access Channel

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPP_SendMOAccessChannelMsg(CWMSAPP *pMe,wms_cdma_message_s_type *index0, wms_cdma_message_s_type *incomingMsg){
  uint8 i = 0, numberOfChar;
  boolean check = TRUE;
  static uint8 incomingMsg_email_addr[WMS_CDMA_USER_DATA_MAX];
  static uint8 index0_email_addr[WMS_CDMA_USER_DATA_MAX];
  
  int length_incoming_addr = 0;
  int length_index0_addr = 0;
  wms_client_ts_data_s_type *clientTsData = MALLOC(sizeof(wms_client_ts_data_s_type));
  AECHAR   destAddr[WMS_SMS_UDL_MAX_7_BIT];
  static uint8 temp_buf[WMS_MAX_LEN];

  uint8 buf[WMS_MAX_LEN];

  if(clientTsData == NULL) 
  {
    MSG_HIGH("WMSAPPMN: MALLOC failed for clientTsData",0,0,0);
    return ;
  }

  if(index0->address.number_of_digits != incomingMsg->address.number_of_digits) 
  {
    WMSAPPU_FREE(clientTsData);
    return;
  }

  (void)IWMS_TsDecode(pMe->m_pwms, &index0->raw_ts, clientTsData);
  if(index0->address.digit_mode == WMS_DIGIT_MODE_4_BIT)
  {
    (void)IWMS_TsDtmfToAscii( pMe->m_pwms, index0->address.number_of_digits,
      (byte*)index0->address.digits,
      (byte*)pMe->m_buf);
    if(WMSAPPU_IsNonStdEmail(FALSE,
      (uint8*)pMe->m_buf,
      (uint8)index0->address.number_of_digits))
    {
      /* unpack the user data to get the email address */
      (void)IWMS_TsUnpackAscii( pMe->m_pwms, &clientTsData->u.cdma.user_data,
                                clientTsData->u.cdma.user_data.number_of_digits+1,
                                (byte *)temp_buf);
      (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
      length_index0_addr = WMSAPPU_GetNonStdEmail(temp_buf,
                                                  (uint8*)pMe->m_emailAddr.address);

      (void)STRTOWSTR((char *)pMe->m_emailAddr.address,
                        destAddr,
                        sizeof(destAddr));

      (void) MEMCPY(index0_email_addr, pMe->m_emailAddr.address, (uint8)length_index0_addr);
    }
    else
    {
      (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
    }

  }
  (void)IWMS_TsDecode(pMe->m_pwms, &incomingMsg->raw_ts, clientTsData);

  if(incomingMsg->address.digit_mode == WMS_DIGIT_MODE_4_BIT)
  {
    (void)IWMS_TsDtmfToAscii( pMe->m_pwms, incomingMsg->address.number_of_digits,
                              (byte*)incomingMsg->address.digits,
                              (byte*)pMe->m_buf);
    if(WMSAPPU_IsNonStdEmail(FALSE,
                            (uint8*)pMe->m_buf,
                            (uint8)incomingMsg->address.number_of_digits))
    {
      /* unpack the user data to get the email address */
      (void)IWMS_TsUnpackAscii( pMe->m_pwms, &clientTsData->u.cdma.user_data,
                                clientTsData->u.cdma.user_data.number_of_digits+1,
                                (byte *)temp_buf);
      (void)MEMSET(pMe->m_emailAddr.address,0,WMS_MAX_LEN);
      length_incoming_addr = WMSAPPU_GetNonStdEmail(temp_buf,
                                                   (uint8*)pMe->m_emailAddr.address);

      (void)STRTOWSTR((char *)pMe->m_emailAddr.address, destAddr, sizeof(destAddr));

      (void) MEMCPY(incomingMsg_email_addr,pMe->m_emailAddr.address, (uint32)length_incoming_addr);
    }
    else
    {
      (void)STRTOWSTR((char*)pMe->m_buf, destAddr, sizeof(destAddr));
    }

  }

  if(index0->address.number_of_digits == incomingMsg->address.number_of_digits
                  && length_index0_addr == length_incoming_addr
      && incomingMsg->address.number_of_digits <= WMS_ADDRESS_MAX )
  {
    for(i=0; i<index0->address.number_of_digits; i++){
      if(index0->address.digits[i] != incomingMsg->address.digits[i]) {
        check = FALSE;
      }
    }
    for(i=0; i<length_incoming_addr; i++){
      if(index0_email_addr[i] != incomingMsg_email_addr[i]){
        check = FALSE;
      }
    }
  }
  else {
    check = FALSE;
    MSG_HIGH("Access Channel Msg Test - incoming address does not match the stored address!!",0,0,0);
  }
  if(check){
     numberOfChar = (uint8) ((temp_buf[length_incoming_addr+2] - 48) *10 + temp_buf[length_incoming_addr+3]-48);
     (void) MEMCPY(&pMe->m_pClient_msg_mo->u.cdma_message.address, &incomingMsg->address, sizeof(wms_address_s_type));
     for (i=(uint8)length_incoming_addr; i<length_incoming_addr+numberOfChar; i++)
     {
       pMe->m_pClient_ts_mo->u.cdma.user_data.data[i] = '1';
     }

     pMe->m_pClient_ts_mo->u.cdma.user_data.data_len = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits=1;
     WMSAPPU_PutNonStdEmail(pMe->m_emailAddr.address, pMe,&pMe->m_pClient_ts_mo->u.cdma.user_data) ;
     (void)MEMCPY(buf,
               pMe->m_pClient_ts_mo->u.cdma.user_data.data,
               pMe->m_pClient_ts_mo->u.cdma.user_data.data_len);
     pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits =
          IWMS_TsPackAscii( pMe->m_pwms, (char *)buf,
                            (byte*)pMe->m_pClient_ts_mo->u.cdma.user_data.data,
                            (byte *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data_len,
                            (byte *)&pMe->m_pClient_ts_mo->u.cdma.user_data.padding_bits);

     for(i = 1; i <= temp_buf[length_incoming_addr+4] - 48; i++)
       (void)WMSAPPMN_PrepareCDMAMOMsgForAccessChannel(pMe, numberOfChar);
  }

  WMSAPPU_FREE(clientTsData);

  return;
}

/*=========================================================================
FUNCTION
  WMSAPP_CheckCDMAFlashSMSMessage

DESCRIPTION
  Check if a CDMA message is a Flash SMS Message.
  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is Flash SMS Message
  FALSE: Is Flash SMS Message

SIDE EFFECTS
  None

=========================================================================*/
static boolean WMSAPP_CheckCDMAFlashSMSMessage
(
  const wms_client_bd_s_type *pClientBD
)
{
  boolean ret_value = FALSE;

  MSG_MED("Function WMSAPP_CheckCDMAFlashSMSMessage entered", 0, 0, 0);

  if (NULL == pClientBD)
  {
    MSG_ERROR("Null Parameter Passed in WMSAPP_CheckCDMAFlashSMSMessage", 0, 0, 0);
    return ret_value;
  }

  if((pClientBD->mask & WMS_MASK_BD_VALID_REL)
     || (WMSAPP_RELATIVE_VALIDITY_IMMEDIATE == 
         wms_ts_encode_relative_time(&pClientBD->validity_relative)))
  {
    ret_value = TRUE;
  }
  else if ( (pClientBD->mask & WMS_MASK_BD_DISPLAY_MODE)
            && (WMS_DISPLAY_MODE_IMMEDIATE == pClientBD->display_mode) )
  {
    ret_value = TRUE;
  }

  MSG_MED("Function WMSAPP_CheckCDMAFlashSMSMessage result = %d", ret_value, 0, 0);
  return ret_value;

}

#endif /* FEATURE_CDSMS */

/*===========================================================================

FUNCTION WMSAPP_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean WMSAPP_HandleEvent(CWMSAPP * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet. It was initialized during the AppClsCreateInstance() function.

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
static boolean WMSAPP_HandleEvent(CWMSAPP * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
 IDialog * pIDialog;
 uint16 wCtlID;
 boolean isNotificationSet = FALSE;
 uint8 nRetVal; 

 if (pMe == NULL)
 {
   MSG_ERROR("WMSAPP Not Initialized, pMe = NULL", 0, 0, 0);
   return TRUE;
 }

 switch(eCode)
 {
   case EVT_APP_START_BACKGROUND:
      /* We're being launched in the background, so go to the background. */
      MSG_HIGH("Received EVT_APP_START_BACKGROUND.", 0, 0, 0);

      if (ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE) != SUCCESS)
      {
        MSG_ERROR("Unable to go background.", 0, 0, 0);
      }

      return (TRUE);

   case EVT_APP_START:   // Process Start event
     pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
     pMe->m_bSuspended = FALSE;

     if (pIDialog)
     {
       MSG_MED("Redrawing dialog",0,0,0);
       IDIALOG_Redraw(pIDialog);
     }
     else
     {
       MSG_HIGH("Starting WMSApp!",0,0,0);
       return CreateSMSMenuDlg(pMe);
     }
     return TRUE;
    
   case EVT_APP_RESUME:
      pMe->m_bSuspended = FALSE;
      MSG_MED("Resuming WMSApp!",0,0,0);
      if (ISHELL_GetActiveDialog(pMe->a.m_pIShell))
      {
        /* Send this Event to the Dialogs to Refresh themselves */
        if (!WMSAPPMN_CheckReturnDlg(pMe))
        {
          MSG_ERROR("WMSAPPMN_CheckReturnDlg Failed",0,0,0);
        }
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,dwParam);
      }
      else
      {
        return (CreateSMSMenuDlg(pMe));
      }
      return TRUE;

    case EVT_APP_STOP:     /* process STOP event */
     MSG_HIGH ("Got EVT_APP_STOP", 0, 0, 0);
     pMe->m_bSuspended = TRUE;
     {
        boolean *pb = (boolean *)dwParam;
        if(pb)
        {
          *pb = FALSE;  /* Set the app to background app */
        }
     }
     return(TRUE);

    case EVT_APP_SUSPEND:
     {
       AEESuspendInfo * pi = (AEESuspendInfo *)dwParam;
       MSG_MED("Suspending WMSApp!",0,0,0);
       if(pi)
       {
         /* Do not kill dialog stack */
         pi->bCloseDialogs = FALSE;
       }
       /* To do processing on Suspending WMSAPP */
       (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,dwParam);
       pMe->m_bSuspended = TRUE;
     }
     return(TRUE);

    case EVT_KEY_PRESS:
    case EVT_KEY_RELEASE:
      return TRUE;

    case EVT_KEY:             // Process key event
      return(TRUE);

    case EVT_COMMAND:          // Process menu command event
      return(TRUE);

    case EVT_COPYRIGHT_END:    // Copyright dialog ended
      return(TRUE);

    case EVT_DIALOG_INIT:
      if(wParam < ARR_SIZE(SMSDlgEventHandlers))
      { /* check for array bound */
        if(SMSDlgEventHandlers[wParam] != NULL)
        {
          IDIALOG_SetEventHandler((IDialog *) dwParam, SMSDlgEventHandlers[wParam], pMe);
          IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS );
          return TRUE;
        }
      }
      return FALSE;

    case EVT_WMS_CFG_GW_READY:
      pMe->m_mode = WMS_MESSAGE_MODE_GW;
      WMSAPPU_FREE(dwParam);
      WMSAPP_Init(pMe);
      return TRUE;

    case EVT_WMS_CFG_CDMA_READY:
      pMe->m_mode = WMS_MESSAGE_MODE_CDMA;
      WMSAPPU_FREE(dwParam);
      WMSAPP_Init(pMe);
      return TRUE;

    case EVT_WMS_MSG_SEND:
      if((!pMe->m_bSuspended)&&
        (((wms_msg_event_info_s_type *)dwParam)->status_info.client_id == WMS_CLIENT_TYPE_WMS_APP))
      {
#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */
          (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_MESSAGE_SENDING, NULL, SELECTIONVERIFICATIONTIME);
      }
      if (((wms_msg_event_info_s_type*)dwParam)->status_info.alpha_id.data)
      {
          WMSAPPU_FREE (((wms_msg_event_info_s_type*)dwParam)->status_info.alpha_id.data);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_MSG_ACK:
    case EVT_WMS_MSG_READ:
    case EVT_WMS_MSG_DELETE:
    case EVT_WMS_MSG_MODIFY_TAG:
    case EVT_WMS_MSG_READ_STATUS_REPORT:
    case EVT_WMS_MSG_DELETE_STATUS_REPORT:
      (void)MEMCPY((void*)pMe->m_pMsgEventInfo, (void*)dwParam, sizeof(wms_msg_event_info_s_type));
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pMsgEventInfo);
      }
      if(pMe->m_bAccessChannelTest)
      {
#ifdef FEATURE_CDSMS
        WMSAPP_SendMOAccessChannelMsg(pMe, &pMe->m_pMsgEventInfo->status_info.message.u.cdma_message, &pMe->m_pAccessChannelMsg->u.cdma_message);
#endif /* FEATURE_CDSMS */
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_MSG_READ_TEMPLATE:
    case EVT_WMS_MSG_WRITE_TEMPLATE:
      (void)MEMCPY((void*)pMe->m_pMsgEventInfo, (void*)dwParam, sizeof(wms_msg_event_info_s_type));
      if (((wms_msg_event_info_s_type*)dwParam)->status_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
      {
        pMe->m_AlphaID.len = ((wms_msg_event_info_s_type*)dwParam)->status_info.message.u.gw_template.alpha_id.len;
        if (pMe->m_AlphaID.len > 0)
        {
          (void)MEMSET(pMe->m_AlphaID.data, 0, WMS_ALPHA_ID_MAX);
          if (((wms_msg_event_info_s_type*)dwParam)->status_info.message.u.gw_template.alpha_id.data)
          {
            (void)MEMCPY (pMe->m_AlphaID.data,
                    (((wms_msg_event_info_s_type*)dwParam)->status_info.message.u.gw_template.alpha_id.data),
                    pMe->m_AlphaID.len);
          }
          else
            pMe->m_AlphaID.len = 0;
        }
        pMe->m_pMsgEventInfo->status_info.message.u.gw_template.alpha_id = pMe->m_AlphaID;
      }

      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pMsgEventInfo);
      }

      if( ((wms_msg_event_info_s_type*)dwParam)->status_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW )
      {
        if (((wms_msg_event_info_s_type*)dwParam)->status_info.message.u.gw_template.alpha_id.data)
        {
          WMSAPPU_FREE(((wms_msg_event_info_s_type*)dwParam)->status_info.message.u.gw_template.alpha_id.data);
        }
      }

      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_MSG_WRITE:
      (void)MEMCPY((void*)pMe->m_pMsgEventInfo, (void*)dwParam, sizeof(wms_msg_event_info_s_type));
      if(!pMe->m_bSuspended)
      {
        /* update the internal message index */
        pMe->m_pClient_msg->msg_hdr.index =
          (byte)pMe->m_pMsgEventInfo->status_info.message.msg_hdr.index;
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pMsgEventInfo);
        (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_MSG_SAVED, NULL, SELECTIONVERIFICATIONTIME);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_DLG_RESUME:
    case EVT_WMS_CONFIRM:
      return(WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,dwParam));

    case EVT_WMS_MSG_DELETE_ALL:
      (void)MEMCPY((void*)pMe->m_pMsgEventInfo, (void*)dwParam, sizeof(wms_msg_event_info_s_type));
      WMSAPPU_FREE(dwParam);
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_ALL_MSGS_DELETED, NULL, SELECTIONVERIFICATIONTIME);
      }
      return TRUE;

    case EVT_WMS_MSG_DELETE_TEMPLATE_ALL:
    case EVT_WMS_MSG_DELETE_TEMPLATE:
      (void)MEMCPY((void*)pMe->m_pMsgEventInfo, (void*)dwParam, sizeof(wms_msg_event_info_s_type));
      WMSAPPU_FREE(dwParam);
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_TEMPLATE_DELETED, NULL, SELECTIONVERIFICATIONTIME);
        return (WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pMsgEventInfo));
      }
      return TRUE;

    case EVT_WMS_CFG_ROUTES:
      WMSAPP_UpdateRoutes(pMe,&((wms_cfg_event_info_s_type *) dwParam)->routes);
      WMSAPPU_FREE(dwParam);
      return TRUE;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    case EVT_WMS_MSG_RECEIVED_MESSAGE:
      {
        wms_msg_event_info_s_type *info = ((wms_msg_event_info_s_type*)dwParam);
        boolean isActive = FALSE;

        MSG_MED("WMSApp received new message!",0,0,0);

        isNotificationSet = FALSE;

        if(pMe->m_bAccessChannelTest){
          if( IWMS_MsgRead(pMe->m_pwms,pMe->m_clientId, &pMe->m_callback, (void*)pMe,
            WMS_MEMORY_STORE_NV_CDMA, 0) == SUCCESS ) {
                (void) MEMCPY(&pMe->m_pAccessChannelMsg->u.cdma_message, &info->mt_message_info.message.u.cdma_message ,sizeof(wms_client_message_s_type ));
          }
        }

        if (IWMS_CfgCheckWapPushMsg(pMe->m_pwms, &info->mt_message_info.message) == TRUE)
        {
          /* Drop this Message since it is already going to be conusmed by BREW */
          MSG_HIGH("WAP Push Message Recieved and Dropped", 0, 0, 0);
          WMSAPPU_FREE(dwParam);
          return TRUE;
        }

        /* Check for Messages with UDH Port Header 
        ** Drop them since they will be Application Specific 
        */
        {
          boolean isUDHPortPresent = FALSE;
          wms_client_ts_data_s_type *clientTsDataPtr = NULL;
           
          if ((clientTsDataPtr = MALLOC(sizeof(wms_client_ts_data_s_type))) != NULL)
          {
            if (info->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
            {
#ifdef FEATURE_CDSMS
              /* Initialize other to 0 */
              (void)MEMSET( &clientTsDataPtr->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

              if (IWMS_TsDecode(pMe->m_pwms, &info->mt_message_info.message.u.cdma_message.raw_ts, clientTsDataPtr) == WMS_OK_S)
              {
                // Check for Flash SMS Message
                if (WMSAPP_CheckCDMAFlashSMSMessage(&clientTsDataPtr->u.cdma) == TRUE)
                {
                  /* Drop this Message since it is not supported */
                  MSG_HIGH("CDMA Flash SMS Message Recieved and Dropped", 0, 0, 0);
                  WMSAPPU_FREE(dwParam);
                  return TRUE;
                }

                if (clientTsDataPtr->u.cdma.message_id.udh_present)
                {
                  uint8 i=0;
                  for(i=0; i<clientTsDataPtr->u.cdma.user_data.num_headers; i++)
                  {
                    if ( (clientTsDataPtr->u.cdma.user_data.headers[i].header_id == WMS_UDH_PORT_8)
                          ||(clientTsDataPtr->u.cdma.user_data.headers[i].header_id == WMS_UDH_PORT_16) )
                    {
                        /* Is a Message with UDH Port */
                        isUDHPortPresent = TRUE;
                        break;
                    }
                  }
                }
              }
#endif /* FEATURE_CDSMS */
            }
            else if (info->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
            {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
            }  
            WMSAPPU_FREE(clientTsDataPtr);
          }

          if (isUDHPortPresent)
          {
            /* Drop this Message since it is already going to be conusmed by Application */
            MSG_HIGH("Message with UDH Port Header Recieved and Dropped", 0, 0, 0);
            WMSAPPU_FREE(dwParam);
            return TRUE;
          }
        } 

        if(info->mt_message_info.message.msg_hdr.message_mode ==
           WMS_MESSAGE_MODE_CDMA)
        {
          /* check if this is a voicemail message and num of messages is 0
          ** if it is then don't do notification.
          */
          if(IWMS_CfgCheckVoiceMail(pMe->m_pwms, WMS_MESSAGE_MODE_CDMA,
                           &info->mt_message_info.message.u.cdma_message.raw_ts,
                           &isActive,NULL) == TRUE)
          {
            /* check if there are active voicemails */
            if(!isActive)
            {
              /* This message is for no voicemails active, so don't notify */
              WMSAPPU_FREE(dwParam);
              return TRUE;
            }
            else
            {
              pMe->m_notify = WMSAPP_NOTIFY_NEW_VOICEMAIL;
              pMe->m_secDisplayState = WMSAPP_NEW_VOICEMAIL;
              isNotificationSet = TRUE;
            }
            /* Active voicemails will fall through for notification */
          }
          /* Not a voicemail...just fall through for notification */
        }
        else /* GW */
        {
          /* check if this is a voicemail message and num of messages is 0
          ** if it is then don't do notification.
          */
#ifdef FEATURE_GWSMS          
#error code not present
#endif /* FEATURE_GWSMS */          
        }
        /* Not a voicemail...just fall through for notification */
      }
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    /* case EVT_WMS_MSG_RECEIVED_MESSAGE: pre-processing above */
    case EVT_WMS_MSG_STATUS_REPORT:

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */
      if (isNotificationSet == FALSE)
      {
        pMe->m_notify = WMSAPP_NOTIFY_NEW_MESSAGE;
        pMe->m_secDisplayState = WMSAPP_NEW_MESSAGE;
        isNotificationSet = TRUE;
      }

      if(pMe->m_bSuspended)
      {
        if (WMSAPP_IsDisplayAlertAllowed(pMe) == TRUE)
        {
          if(ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_WMSAPP,"START WMS APP") == SUCCESS)
          {
            /* Set this flag to close the proper amount of dialogs later */
            pMe->m_bAlertOnly = TRUE;

            /* Set this flag so the idle menu will post a special event */
            pMe->m_bDisplayNewMsgAlert = TRUE;
            WMSAPP_UpdateSecondaryDisp(pMe);
          }
          else
          {
            MSG_HIGH("WmsApp failed to start. Storing message!!",0,0,0);
            return (WMSAPPMN_ProcessMTMessage(pMe, (wms_msg_event_info_s_type*)dwParam));

          }
        }
        WMSAPP_SoundAlert(pMe);
      }
      else
      {
        WMSAPP_SoundAlert(pMe);

        if (WMSAPP_IsDisplayAlertAllowed(pMe) == TRUE)
        {
          boolean check = TRUE;
          pMe->m_bAlertOnly = FALSE;
          if(WMSAPPMN_ProcessMTMessage(pMe, (wms_msg_event_info_s_type*)dwParam))
          {
            if ((pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell)) != NULL)
            {
              wCtlID = IDIALOG_GetID(pIDialog);
              if(wCtlID != IDD_NEW_MSG_RXD)
              {
                if (SUCCESS == ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                          IDD_NEW_MSG_RXD, NULL))
                {
                  check = TRUE;
                }
                else
                {
                  check = FALSE;
                }
              }
              else
              {
                WMSAPP_UpdateSecondaryDisp(pMe);
                /* Initialize the New MT Message Dialog to Indicate the Latest Notification */
                (void)WMSAPPMN_InitNewMtMsg(pMe);
              }
            }
            else
            {
              MSG_ERROR("ISHELL_GetActiveDialog Failed!", 0, 0, 0);
              check = FALSE;
            }
          }
          return check;
        }
      }

      return (WMSAPPMN_ProcessMTMessage(pMe, (wms_msg_event_info_s_type*)dwParam));

    case EVT_WMS_PROCESS_NEW_MSG:
      /* reset this flag for next time */
      pMe->m_bDisplayNewMsgAlert = FALSE;
      if(!pMe->m_bSuspended)
      {
        if ((pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell)) != NULL)
        {
          wCtlID = IDIALOG_GetID(pIDialog);

          if(wCtlID != IDD_NEW_MSG_RXD)
          {
            return((ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                      IDD_NEW_MSG_RXD, NULL) == SUCCESS));
          }
          else
          {
            /* Initialize the New MT Message Dialog to Indicate the
            ** Latest Notification
            */
            (void)WMSAPPMN_InitNewMtMsg(pMe);
          }
        }
        else
        {
          MSG_ERROR("ISHELL_GetActiveDialog Failed!", 0, 0, 0);
          return FALSE;
        }
      }
      else
      {
        MSG_HIGH("Should not be here! app is suspended",0,0,0);
      }
      return TRUE;

    case EVT_WMS_MSG_SUBMIT_REPORT:
      if((!pMe->m_bSuspended) &&
         (((wms_msg_event_info_s_type *)dwParam)->submit_report_info.client_id == WMS_CLIENT_TYPE_WMS_APP))
      {
        (void)WMSAPP_ProcessStatus(pMe, &((wms_msg_event_info_s_type *)dwParam)->submit_report_info);
      }

      if (((wms_msg_event_info_s_type*)dwParam)->submit_report_info.alpha_id.data)
      {
        WMSAPPU_FREE (((wms_msg_event_info_s_type*)dwParam)->submit_report_info.alpha_id.data);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_CFG_MEMORY_STATUS:
      WMSAPP_UpdateMemoryStatus(pMe,&((wms_cfg_event_info_s_type *)dwParam)->memory_status);
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pMemStatuses);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_CFG_MESSAGE_LIST:
      WMSAPP_UpdateMessageList(pMe,&((wms_cfg_event_info_s_type *) dwParam)->message_list);
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe,eCode,wParam,(uint32)pMe->m_pSmsLists);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_CFG_GW_DOMAIN_PREF:
      pMe->m_GWDomainPref = ((wms_cfg_event_info_s_type *) dwParam)->gw_domain_pref;
      WMSAPPU_FREE(dwParam);
      return TRUE;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

    case EVT_WMS_CFG_PRIMARY_CLIENT_SET:
      {
        wms_cfg_set_primary_client_s_type  set_primary_info;

        set_primary_info = ((wms_cfg_event_info_s_type *) dwParam)->set_primary;

        /* Re-register WMSAPP only if the Primary Client de-registers itself 
           i.e. set_primary_info.set_primary equals FALSE */
        if (!set_primary_info.set_primary)
        {
#ifdef FEATURE_WMSAPP_STORAGE
          (void)IWMS_SetPrimaryClient(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, TRUE, TRUE);
#else
          (void)IWMS_SetPrimaryClient(pMe->m_pwms, pMe->m_clientId, &pMe->m_callback, (void*)pMe, TRUE, FALSE);
#endif /* FEATURE_WMSAPP_STORAGE */
        }
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;

    case EVT_WMS_CFG_MEMORY_FULL:
    {
      boolean check = TRUE;
      MSG_HIGH("WMSApp received memory full!",0,0,0);
      pMe->m_notify = WMSAPP_NOTIFY_MEMORY_FULL;
      pMe->m_secDisplayState = WMSAPP_MEMORY_FULL;
      pMe->m_msgInfo.memStore = ((wms_cfg_event_info_s_type*)dwParam)->mem_store;
      if(pMe->m_bSuspended)
      {
        if (WMSAPP_IsDisplayAlertAllowed(pMe) == TRUE)
        {
          if(ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_WMSAPP,"START WMS APP") == SUCCESS)
          {
            /* Set this flag to close the proper amount of dialogs later */
            pMe->m_bAlertOnly = TRUE;

            /* Set this flag so the idle menu will post a special event */
            pMe->m_bDisplayNewMsgAlert = TRUE;
            WMSAPP_UpdateSecondaryDisp(pMe);
          }
          else
          {
            check = FALSE;
          }
        }
        if(check == TRUE)
        {
          WMSAPP_SoundAlert(pMe);
        }
      }
      else
      {
        WMSAPP_SoundAlert(pMe);

        if (WMSAPP_IsDisplayAlertAllowed(pMe) == TRUE)
        {
          pMe->m_bAlertOnly = FALSE;
          if ((pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell)) != NULL)
          {
            wCtlID = IDIALOG_GetID(pIDialog);
            if(wCtlID != IDD_NEW_MSG_RXD)
            {
              if (SUCCESS == ISHELL_CreateDialog(pMe->a.m_pIShell, WMSAPP_RES_FILE,
                                                IDD_NEW_MSG_RXD, NULL))
              {
                check = TRUE;
              }
              else
              {
                check = FALSE;
              }
            }
            else
            {
              WMSAPP_UpdateSecondaryDisp(pMe);
              /* Initialize the New MT Message Dialog to Indicate the Latest Notification */
              (void)WMSAPPMN_InitNewMtMsg(pMe);
            }
          }
          else
          {
            MSG_ERROR("ISHELL_GetActiveDialog Failed!", 0, 0, 0);
            check = FALSE;
          }
        }
      }
      WMSAPPU_FREE(dwParam);
      return check;
    }

    case EVT_WMS_CMD_ERR:
      {
        wmsapp_cmd_err_type *cmd_err = (wmsapp_cmd_err_type *) dwParam;

        if(cmd_err->cmd_err != WMS_CMD_ERR_NONE)
        {
          MSG_ERROR("WMSAPP Error cmd %d error %d!",
                     cmd_err->cmd,
                     cmd_err->cmd_err,
                     0
                   );

          if (!pMe->m_bSuspended)
          {
            (void)WMSAPPMN_DisplayError(pMe, cmd_err->cmd, cmd_err->cmd_err);
          }
        }
        WMSAPPU_FREE(cmd_err);
      }
      return TRUE;

    case EVT_WMS_DC_CONNECTED:
      if(pMe->m_bAutoDcSend && pMe->m_bDCOrig)
      {
        /* re-send the message */
        if((WMSAPPMN_SendMessage(pMe)) == FALSE)
        {
          MSG_HIGH("Send message failed", 0, 0, 0);
          /* reset the flag */
          pMe->m_bDCOrig = FALSE;

          /* disconnect dc */
          (void)WMSAPPMN_DcDisconnect(pMe);
        }
      }
      return TRUE;

    case EVT_WMS_DC_ABORTED:
      if(pMe->m_bDCOrig)
      {
        /* reset the flag */
        pMe->m_bDCOrig = FALSE;

        /* Display a message to inform the user the message could not be sent */
        (void)WMSAPPMN_DisplayMsgDlg(pMe, IDS_MSG_FAILED, NULL, SELECTIONVERIFICATIONTIME);
      }
      return TRUE;

    case EVT_WMS_DC_ENABLE_AUTO_DISCONNECT:
    case EVT_WMS_DC_DISABLE_AUTO_DISCONNECT:
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe, eCode, wParam, dwParam);
      }
      return TRUE;

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
    case EVT_WMS_BC_MM_EVENT_CONFIG:
    case EVT_WMS_BC_MM_EVENT_PREF:
    case EVT_WMS_BC_MM_EVENT_TABLE:
    case EVT_WMS_BC_MM_EVENT_SRV_IDS:
    case EVT_WMS_BC_MM_EVENT_SRV_INFO:
    case EVT_WMS_BC_MM_EVENT_SRV_UPDATED:
    case EVT_WMS_BC_MM_EVENT_ADD_SRVS:
    case EVT_WMS_BC_MM_EVENT_DELETE_SRVS:
    case EVT_WMS_BC_MM_EVENT_SELECT_ALL_SRVS:
    case EVT_WMS_BC_MM_EVENT_DELETE_ALL_SRVS:
    case EVT_WMS_BC_MM_EVENT_READING_PREF:
    case EVT_WMS_BC_MM_EVENT_ENABLE_FAILURE:
    case EVT_WMS_BC_MM_EVENT_DISABLE_FAILURE:
      (void)WMSAPPMN_ProcessBcMmEvent(pMe, eCode, ((wms_bc_mm_event_info_s_type *)dwParam));
      if(!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe, eCode, wParam, dwParam);
      }
      WMSAPPU_FREE(dwParam);
      return TRUE;
#endif /*FEATURE_BROADCAST_SMS_MULTIMODE*/

    case EVT_WMSAPP_MESSAGE_READ:
      if (!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe, eCode, wParam, dwParam);
      }
      break;

    case EVT_WMSAPP_MESSAGE_COMPOSE:
      if (!pMe->m_bSuspended)
      {
        (void)WMSAPPMN_SendEvtToDlg(pMe, eCode, wParam, dwParam);
      }
      else
      {
        /* Launch WMSAPP */
        if(ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_WMSAPP,"START WMS APP") == SUCCESS)
        {
          pMe->m_bAlertOnly = FALSE;
          pMe->m_bDisplayNewMsgAlert = FALSE;
        }
        else
        {
          WMSAPPU_FREE(dwParam);
          return FALSE;
        }
        nRetVal = ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_WMSAPP,
                            EVT_WMSAPP_MESSAGE_COMPOSE, wParam, dwParam);
        if(nRetVal == FALSE)
        {
          MSG_HIGH("ISHELL_PostEventEx - Event could not be posted!", 0,0,0);
        }
      }
      break;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    case EVT_NOTIFY:
    {
#ifdef FEATURE_IWMSDIAG
      AEEWMSDIAGNotifyInfo *EventInfo = (AEEWMSDIAGNotifyInfo *)(((AEENotify *)dwParam)->pData);
#endif /* FEATURE_IWMSDIAG */

      /* Handle shell init notifier */
      if ((((AEENotify *)dwParam)->cls == AEECLSID_SHELL) &&
               (((AEENotify *)dwParam)->dwMask & NMASK_SHELL_INIT))
      {
        if (NULL == pMe->a.m_pIShell)
        {
          MSG_ERROR("Failed to get shell pointer.", 0, 0, 0);
          return TRUE;
        }

        if (ISHELL_StartBackgroundApplet(pMe->a.m_pIShell,
                                         AEECLSID_WMSAPP, NULL) != SUCCESS)
        {
          MSG_ERROR("Failed to launch WMSAPP", 0, 0, 0);
        }
        return TRUE;
      }

#ifdef FEATURE_IWMSDIAG
      if (((AEENotify *)dwParam)->cls == AEECLSID_WMSDIAG_NOTIFIER)
      {
        switch (((AEENotify *)dwParam)->dwMask)
        {
          case NMASK_WMSDIAG_REQ:
            switch (EventInfo->cmd_id)
            {
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
  #endif /*FEATURE_GWSMS_BROADCAST*/
              default:
                break;
            }
            break; /* case NMASK_WMSDIAG_REQ */

          default:
            break;
        } /*switch (((AEENotify *)dwParam)->dwMask)*/
      }
#endif /* FEATURE_IWMSDIAG */

      return TRUE;
    } /* case EVT_NOTIFY */
    default:
      break;

  }

  return FALSE;

}/*WMSAPP_HandleEvent*/

#endif /*FEATURE_WMSAPP*/

/*========================End of File ================================================*/

