/*===========================================================================

FILE: CoreMenu.c

DESCRIPTION
   This file contains the Core app's menu related functions.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by CoreApp.c.

       Copyright ?2002 - 2008 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreMenu.c#322 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/08   clm     Let alarm clock handle alarm annunciator if FEATURE_ALARMCLOCK_APP is defined.
                   Add Alarm clock app icon to the 3GUI main menu
03/26/08   cvs     Remove deprecated functions
03/11/07   cvs     Remove potential infinite loop
03/07/08   cvs     Remove secure clock menu in CDMA mode, not user clock menu.
10/29/07   sg      Support for setting the BREW Secure clock
10/24/07   cvs     Force PIN dialog to be created immediately.
09/28/07   clm     Add PLMN name get functions.
09/24/07   snb     Add option to exit from USB MTP mode.
07/24/07   jks     Featurized support for SND_DEVICE_BT_A2DP_SCO_HEADSET
07/16/07   jks     Added featurization for SND_DEVICE_SDAC
04/10/07   ypmw    Fix the day light saving time adjustment
03/13/07   ypmw    Add 5 sec option for LCD display and backlight
03/07/07   jas     Reimplement background images with overlays.
                   Support VGA background images.
02/07/07   cvs     Set Time menu depends on FEATURE_USRSYSCLOCK
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/16/06   cvs     Add debug messages
12/14/06   ypmw    Optimize the UI utilization time for AEECM_OPRT_MODE_NET_TEST_GW 
                   event
12/12/06   RT      Fixed compile warning
12/11/06   RT      Removed SIO configuration related event handlers and functions
12/06/06   cvs     Fix lint issues
12/06/06   RT      Included CoreService.h to fix compiler warnings
11/29/06   ypmw    Fix wlan compiler warnings
11/14/06   ypmw    Fix wlan compiler warnings
11/09/06   ypmw    Fix the WIFI Security setting errors
11/09/06   ypmw    Fix the compiler warning
11/01/06   ypmw    Add SSID for start adhoc network
10/18/06   cvs     Fix compile errors
10/09/06   ypmw    Add multi-port for data services
10/03/06   cvs     Add support for MBN
09/22/06   jas     Cleaning up some logic errors
09/20/06   jas     Setting the band value correctly on WLAN manual scan
09/19/06   jas     Adding band value to AEECMWLANPref structure.
09/19/06   ypmw    Modified plmn network scan dialog
09/18/06   cvs     Fix compiler warnings
09/14/06   sj      Fixed compiler error when FEATURE_MMGSDI_PERSONALIZATION
                   is not defined.
09/13/06   sj      Fix handling of card events in perso and LPM mode
09/13/06   jas     Fixing a compiler warning
09/08/06   jas     Removing menus that were related to the NetSettings app
09/06/06   ak      Memory Leak removal for Network Selection
09/01/06   ak      Cleanup whitespaces from earlier checkins
08/23/06   jks     Added support for WLAN Adhoc networks
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added menu to set IncallMixing state
08/08/06   joe     NetSettings related changes.
07/25/06   jas     Fixing compiler warnings
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
07/12/06   jks     Fixed compile warnings
06/15/06   cvs     Fix Klocwork errors
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
05/31/06   RI      Added support for IMS VideoShare.
05/12/06   cvs     update featurizations
05/08/06   jks     Added HDR+WLAN, WCDMA+WLAN, GSM+WLAN, and WCDMA+GSM+WLAN to network mode menu
05/04/06   jas     Porting display mapping code from 7500
05/02/06   jas     Removing the background image outside CoreApp
03/27/06   ng      Featurized VT app.
03/13/06   jks     Added support for WLAN scan mode selection
03/14/06   jks     Removed WLAN profile_id (obsolete)
02/28/06   cvs     Add support for MBDN
02/15/06   pm      Hook up the Java icon with the new JAM
02/13/06   jas     Added GCF support for language selection
02/09/06   jks     Removed WWAN from SIO data port menu
01/24/06   jks     Fixed WLAN tech to WLAN tech preference translation
01/24/06   jas     Added support for WLAN channel value of 0 (any channel)
01/09/06   jas     Fixed typo in WCDMA band classes
01/05/06   jas     Added support for new WCDMA band classes
12/20/05   ak      Fixed Compilation issues
12/16/05   jks     Added security support for WLAN manual scans
12/05/05   jks     Removed WPA submenus, added DynamicMenuDlgEventHandler and
                   RefreshWLANSettingsMenu function, added new menu functionality
                   for manual vs auto WLAN network scan
11/07/05   ak      Do not allow phone operations for blocked pin case
10/21/05   RI      Updated the NITZ menu
10/20/05   sun     Removed support for BT-SAP
10/13/05   jks     Removed IP Version menu item
10/10/05   jks     Added new menu WLAN General Configs, added edit WLAN Max Tx Power
10/07/05   RI      Fixed network time menu item.
10/03/05   RI      Updated the GetTimeOfDay() function.
09/30/05   cvs     Fix EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED when idle dialog
                   is not the current dialog.
09/23/05   sun     Do not change the network setting when the user aborts Manual PLMN Selection
09/05/05   RI      Added a menu item for setting the user time using MM info
08/31/05   sun     Added support for BT-SAP
08/03/05   jas     Name change for WEP key sizes
08/08/05   aps     Fixed NV volume level lookup for sound devices
07/27/05   AT      Fixing compile errors
07/25/05   sun     Added support for Idle Screen notification
07/05/05   jas     Add more statistics to WLAN Status screen
07/05/05   ak      Merge MEID Support from Jaguar Branch
06/30/05   jas     Migrate ICM_SetSubscriptionStatus call from IWIFI
06/28/05   cvs     Add menu to select sound device
06/23/05   jas     Changing WLAN TLS security to TLS Certificat security
06/07/05   sun     Added support for CCBS
05/27/05   jas     Adding WLAN network selection mode select dialog
05/13/05   jas     Updating WLAN preferences to match ICM changes
05/02/05   jas     Change annunciator field name to match interface
04/29/05   ak      Adding 3DUI App Changes
04/28/05   jas     Case fixes for Linux
04/27/05   jas     Making some of the WLAN security menus more table-driven
04/21/05   jas     Adding WPA WLAN security options
04/19/05   jas     Hexadecimal range checking added for WEP key inputs
04/19/05   jas     Fixing WLAN featurization problem
04/13/05   sun     Privacy Menu option stripped for UMTS
04/08/05   jas     Reworked authentication menus
03/30/05   jas     Fixed workaround for handling of band prefs in WLAN Select menu
03/24/05   jas     Filled in all wlan pref fields in manual system selection
03/23/05   jas     Support AEECM_WLAN_AUTH_NONE in CoreApp_FillWLANAuthInfo
03/21/05   jas     Adding WLAN statistics to WLAN status dialog
03/21/05   jas     Workaround for handling of band prefs in WLAN Select menu
03/17/05   ak      Fixed Featurization Issue for GSM builds
03/15/05   jas     Display error msg if WLAN selection fails
03/14/05   jas     Fixing merge error in WLAN code
03/14/05   cvs     For Dual slot, use CDMA+HDR for cdma application
03/07/05   jas     Converting menu tables to use 64-bit values for set/get
03/04/05   jas     Get size of SSID field for WLAN with sizeof
03/03/05   cvs     Strip all WCDMA band prefs
03/03/05   jas     Fixing string lengths for WLAN params
02/22/05   jas     Adding more WLAN settings menus
02/17/05   cvs     Fix error handling in MSISDN numbers
02/17/05   PA      Support for displaying MSISDN numbers in MyInfo
02/16/05   sun     Added support for Enhanced Network Selection
02/14/05   cvs     dualslot: don't bring up select tech dialog if it
                   can be inhibited and no card apps changed
02/14/05   cvs     Fix ui lockups resulting from race condition
                   in dualslot, where switching slots with same tech
02/11/05   jas     COREMENU_OV_DISABLE needed to operate on a locked image
02/08/05   jas     Change COREMENU_OV_DISABLE to only disable images
                   that CoreApp created.
02/03/05   tjc/jas Support for full-screen mode
01/17/05   cvs     Remove Time/Date menu if !FEATURE_PMIC_RTC
01/06/05   cvs     Remove burst beep length if not a 1x target
01/04/05   cvs     Fix garbage in MyInfo/PLMN.  Fix compiler warnings.
01/03/05   cvs     Added DATA WWAN menu to Port Map menu
12/21/04   cvs     Add menus to wakeup 1x and HDR
12/13/04   ssc     Changed include file name case to match the Linux names
                   of the files. Mismatches in the case break AMSS builds
                   on Linux servers.
12/09/04   cvs     Add menu to avoid current system, remove time menu if CDMA
12/08/04   sun     Fixed IMenuCtl pointers
12/07/04   sun     Added new LCD off timer
12/06/04   BW      Added LAN menu and related submenu
11/23/04   ak      Added a Menu Item for HDR+CDMA only
11/22/04   sun     Added support for USB Charging
11/17/04   sun     Use CFGI item for Beep Length
11/12/04   ank,cvs Dualslot changes from 6300
11/11/04   sun     Added new menu option for LCD Off
11/09/04   cvs     Fix compiler warnings
10/28/04   jas     Updated feature defines
10/13/04   PA      Fix PRL info
10/07/04   cjr     Updated COREMENU_OV_DISABLE() Macro
10/06/04   sun     Fixed Manual PLMN Search screens
09/22/04   cvs     Add Contrast menu, lint fixes
09/20/04   sun     Check for nulls
09/14/04   cjr     IDisplay_Update overlay registration
09/14/04   sun     Fix the index for PLMN list display
09/10/04   jas     Added BREW version guards to the App Mgr class id fix
09/08/04   jas     Incorporated BREW 3.1 fixes for App Mgr class id
09/03/04   tml     Added pin 2 verification for line switching
09/02/04   tml     Added CPHS support and compilation fix and added revision
                   history comment
08/16/04   cjr     Added overlay transparency operation
07/27/04   sun     Start Dialer if any call is present
07/20/04   cjr     Added background and annunciator overlay support through MDP
07/08/04   SUN     Do not set to auto in DisplayPLMNListAgain
06/30/04   PA      Fix for data abort in restrict dialog due to change in
                   IMenuCtl implementation.
06/28/04   jas     Added support for dual-display targets
06/08/04   tml     Allow security submenu to be created
06/07/04   tml     Added personalization support
06/07/04   tml     Fixed compilation
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
06/01/04   sun     Delete unnecessary items
05/26/04   ak      IMenuCtl Changes
05/25/04   sun     Fixed Compile Warning
05/18/04   RI      Changed MM_RINGER_DIR to "ringers" (instead of "ringer")
05/13/04   AT      Added thin_ui changes.
05/13/04   sun     Added LPM Changes and added new Band Prefs
05/07/04   sun     Fixed TTY, Separated Game Mode from LPM
04/29/04   sun     Made plmn info modifications
04/16/04   sun     Made Backlight/kepad light modifications
04/14/04   sun     Added new menu item to set VR
03/31/04   ram     Changes for not clearing Idle Menu Text on End Key
03/19/04   sun     Added changes for DTMF
02/03/04   ram     Featurized items under Network menu.
02/04/04   is      Background gpsApp support
01/29/04   tml     Added NEW_ICARD feature
01/29/04   sun     Display the Service domain correctly on Myinfo
12/22/03   tml     ICARD API changes
01/07/04   ram     Add Multimedia Volume setting
01/05/04   tml     Featurized slots options in Restrict menu
12/10/03   ram     Removed lint errors.
12/10/03   sun     Check the return result for ui_get_nv
12/02/03   kar     Backed out change to end all calls from Core
12/02/03   kar     Fixed compile error
12/02/03   ram     Fix compile erros.
11/31/03   BPW     Do not handle auto repeat key events in BREW 3.0
11/26/03   BPW     End key press on idle menu now ends all calls
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/20/03   ram     Added HDR_ABORT_PPP support.
11/19/03   kar     Removed SKUI icon, added PLMN ID display
11/19/03   BPW     Fixed core to lauch dialer on end key press if connected call
11/11/03   ram     Added Game-Mode and removed PS-Attach/Detach/Any   .
11/11/03   tml     Added Pref Slot support
11/05/03   tml     Merged in changes for Bluetooth app
11/05/03   NNG     Added support for VT.
11/03/03   kar     Fixed End Key issue and compilation error fix
11/03/03   ram     Added PLMN selection menu.
10/31/03   kar     Band pref changes
10/29/03   ram     Changes AEECLSID_DATASTATSAPP to AEECLSID_DATASTATS
10/09/03   sun     Fixed Compile Error
09/30/03   tml     Compilation fixes
10/01/03   eam     Made Java menu item start up external AMS app if supported
09/30/03   tml     Reflected changes in ICM API
09/26/03   kar     Fixed compile errors
09/23/03   kar     Launch recent calls app on SEND key
09/22/03   kar     Fixed featurization
09/21/03   tml     ICM support
09/22/03   BPW     Changed the way Dialer App starts up.
08/29/03   ram     Added support for DataStats App.
08/08/03   tml     Added Dual Slot support
08/09/03   kar     Fixed phone number display for MM targets
08/08/03   sun     Added suppport for checkboxes for Band pref menu
08/05/03   kar     Set time changes
07/31/03   kar     Changes to support BREW
07/16/03   tml     enable/disable pin1/pin2 cleanup
07/15/03   tml     Security clean up
07/11/03   bpw     Added Dialer app functionality and removed numedit.
06/19/03   kar     Fixed compile warning
06/10/03   kar     Handle EVT_KEY_PRESS rather than EVT_KEY for * and #
06/09/03   AT      Allowed support for # and * in 1X as well as GSM/WCDMA.
06/06/03   AT      Added support for dial digits and supplementary services.
06/03/03   ram     Fixed stack corruption in LTK
05/28/03   kar     Include wapapp.bid rather than header file
05/27/03   kar     Fixed auto-retry, network menu error messages
05/20/03   kar     Fixed IMSI display
05/14/03   tml     PIN Change modification
05/08/03   kar     System menu changes
05/03/03   tml     Added NV Lock code support
04/29/03   kar     Restored CellOnly/PCS only menus
04/29/03   kar     Fixed JCDMA compile error
04/24/03   kar     Support for recent calls application
04/18/03   kar     Fixed GW compile errors and fixed memory leak
04/17/03   PA/AT   Lookup number in Speed Dial List before calling
04/14/03   kar     Code cleanup
04/04/03    lz     Removed the code to tear down data call in Idle menu.
04/02/03   kar     Code cleanup
04/01/03   kar     Code cleanup
03/28/03   lz      Tear down data call if UI active
03/14/03   kar     Removed network menu for GW builds
03/14/03   kar     Fixed error fatal on selecting network menu for GW targets
03/11/03   kar     End numedit dlg on call orig.Goto SKUI if END pressed and in call
03/05/03   NNG     Featurized Force Mode menu under FEATURE_UI_FORCE_CALL.
02/25/03   tml     Changed Cat App start applet to start applet Arg
02/24/03   kar     Fixed compile error on cell only builds
02/21/03   kar     Code cleanup
02/18/03   AP      Added support for FEATURE_WMS_APP.
02/13/03   NNG     Don't restart timer when settings dialog under EVT_DIALOG_END.
02/11/03   kar     Fixed compile warning
02/10/03   kar     Added support for numedit dialog
02/05/03   kar     Support for new icons
02/03/03   kar     Fixed compiler warnings
02/03/03   ic      In InitStaticDlg() force the static control to redraw
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
                   Added various new look up tables.
01/30/03   kar     Removed PM1000 code
01/28/03   tml     GSM/WCDMA compiler fixes + add CAT App
01/28/03   kar     Changes for AppComFunc API
01/07/03   kar     Use AppComFunc API
12/31/02   kar     Reflected variable name change for banner string
12/10/02   lz      Added support for WAP Browser.
12/03/02   tml     Added offline check before displaying rtre configuration
                   menu
11/20/02   kar     RTRE change
11/15/02   pm      Added support for Contact app.
11/13/02   NNG     Added support for GPS app.
11/13/02   kar     RTRE Config menu added
11/08/02   aaa     Fixed Compile Errors
10/28/02   tml     RTRE Security changes
10/14/02   tml     code review changes
10/11/02   kar     Fixed compile error for dual mode builds
10/03/02   tml     Code cleanup for Security module + Fixes for RTRE
10/03/02   kar     Fixed compiler warnings
10/02/02   kar     Code cleanup
09/20/02   kar     Code indentation
09/20/02   aaa     Support for time display at my info screen
09/19/02   tml     Added NV lock code support in case of no FEATURE_MMGSDI
                   or if FEATURE_UIM_RUN_TIME_ENABLE is defined
                   Fixed enable/disable pin issue
09/19/02   kar     Moved DMSSinclude files to CoreOldUI.h
09/18/02   kar     Support for MM ringer volume
09/17/02   kar     Removed unused variables
09/17/02   kar     Malloced the MMRingerFile ptr
09/17/02   kar     Code cleanup
09/16/02   AAA     Added Multimedia Ringer support
09/16/02   kar     Fixed compile warnings
09/12/02   kar     Fixed featurization
09/11/02 att/tml   Modificatons for ICARD changed
09/11/02   tml     Featurized out MMC Download
09/10/02   kar     Allow security menu access for non card targets
09/09/02  js/tml   Disable PINLock for PIN2 in non WCDMA technology
                   Disable PIN Change menu based on whether PIN1 or PIN2 have
                   been disabled or not
                   Enhanced EnablePINDlg event handler so that success/failure
                   messages will be displayed
09/03/02   kar     Fixed typo
08/23/02   att     Fixed Disable PIN issue while entering security menu
08/22/02   kar     Renamed variables to comply with guidelines
08/21/02  js/tml   Add Code support for ICARD integration
08/13/02   kar     Core menu module
===========================================================================*/


/*===============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "OEMFeatures.h"
#include "comdef.h"
#include "AEEStdLib.h"    /* contain FREE function */
#include "AEEAppGen.h"    /* Applet helper file */
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEEText.h"
#include "AEEDate.h"
#include "AEETime.h"
#include "AEEFile.h"          // AEE File Manager Services
#include "AEEMedia.h"
#include "AEEMediaUtil.h"     // AEE Media Utility functions
                //for multimedia ringers
#include "AEENet.h"
#include "AppComFunc.h"
#include "AEEStdLib.h"

#include "CoreApp_res.h"
#include "CoreApp_qvga_res.h"
#include "CoreMenu.h"
#include "CoreService.h"
#include "CoreSecurity.h"
#include "MediaPlayer.bid"      /* For Media Player Class Id */
#include "CoreUtils.h"
#include "core_ui.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"

#include "disp.h"     /*To change contrast*/

#ifdef FEATURE_APP_CONTACT
#include "ContApp.h"
#endif

#ifdef FEATURE_APP_DIALER
#include "DialerApp.h"
#endif

#ifdef FEATURE_APP_DATASTATS
#include "DATASTATSAPP.BID"
#endif

#ifdef FEATURE_WMS_APP
#include "WmsApp.h"
#endif /* FEATURE_WMS_APP */

#ifdef FEATURE_GPSAPP
#include "gpsApp.bid"
extern const char gpsApp_StartUpStr[];
#endif

#ifdef FEATURE_UI_3D_APP
#include "ui3d.bid"
#endif

#ifdef FEATURE_APP_WAP
#include "WAPApp.BID"
#endif

#ifdef FEATURE_APP_CATAPP
#include "catapp.bid"
#endif

#ifdef FEATURE_APP_FLDDBG
#include "FLDDBGAPP.BID"
#endif

#ifdef FEATURE_APP_QCAMERA
#include "qcamera.bid"
#endif

#ifdef FEATURE_NETSETTINGS_APP
#include "NetSettings.bid"
#endif

#ifdef FEATURE_AUXSETTINGS_APP
#include "AUXSettings.bid"
#endif

#ifdef FEATURE_APP_QCAMCORDER
#include "Qcamcorder.bid"
#endif

#ifdef FEATURE_JAVA
   #include "java_config.h"
   #ifdef FEATURE_JAVA_APP_MANAGER
      #include "JavaAppManager.bid"
      #define AEECLSID_JAVA_AMS AEECLSID_JAVAAPPMANAGER
   #else
      #ifdef FEATURE_JAVA_AMS_APP
         #include "ams.bid"
         #define AEECLSID_JAVA_AMS AEECLSID_AMS
      #else
         #include "java.bid"
         #define AEECLSID_JAVA_AMS AEECLSID_JAVA
      #endif
   #endif
#endif

#ifdef FEATURE_APP_RECENTCALLS
#include "recentcalls.bid"
#include "recentcalls.h"
#endif

#ifdef FEATURE_QVPHONE_APP
#include "QVIDEOPHONE.BID"
#endif /* FEATURE_QVPHONE_APP */

#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp.bid"
#endif

#if defined(FEATURE_APP_MMS)
#include "MMSApp.BID"
#endif

#ifdef FEATURE_SD20
#include "sd.h"
#endif

#ifdef FEATURE_AUXSETTINGS_APP
#define AUX_StartSIOCONFIGURATIONS "SIOCONFIGURATIONS"
#define AUX_StartWLAN "WLAN"
#define AUX_StartUSBDEVSW "USBDEVSW"
#define AUX_StartSECTIME "SECURETIME"
#endif

#include "AppComFunc.h"

// DMSS includes
#include "sdevmenu.h"
#include "sdevmap.h"
#include "rdevmap.h"
#include "mobile.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#include "ui_base.h"
#include "uiutils.h"

#ifdef FEATURE_IMS
#include "IMS_SETTINGS.BID"
#ifdef FEATURE_IMS_VIDEOSHARE
#include "IMS_VIDEOSHARE.BID"
#endif
#endif // FEATURE_IMS

#ifdef FEATURE_ALARMCLOCK_APP
#include "AlarmClock.bid"
#endif

#if defined(FEATURE_FMRADIO_APP)
#include "fmradioapp.brh"
#include "FMRADIOAPP.BID"
#include "FMRadioApp.h"
#endif

//lint -save -e740
#define TEXT_BUFFER_SIZE  400   // Size of buffer used to hold text strings
#define TITLE_BUFFER_SIZE  100
#define TC_BUFFER_SIZE 20
#define NUMBER_BUFFER_SIZE 32   // Max. dial string
#define MAX_PLMN_LIST_ITEMS 20 // Max. number of items in the PLMN List.
#define MAX_STR_SIZE 50

#define MIN_VAL(a, b) (((a) <= (b)) ? (a) : (b))

/* Valid action types */
#define LAUNCH_DIALOG 0
#define LAUNCH_APPLET 1
#define MENU_SETTING  2

/* WLAN Channel values
 * CHANNEL_BUFFER_SIZE is the UI buffer text size
 * CORE_WLAN_CHANNEL_ANY is the value used in the UI for "any channel";
 *   manipulating this value is easier than manipulating the AEEWIFI
 *   value for "any".
 */
#define CHANNEL_BUFFER_SIZE     2
#define CORE_WLAN_CHANNEL_ANY   0

typedef uint64 (GetFnPtrType)(CCoreApp*);
typedef boolean (SetFnPtrType)(CCoreApp*, uint64);

typedef struct {
  uint16 wDialogID;     /* Current dialog ID */
  uint16 wControlID;    /* Control ID in current dialog */
  uint16 wItemID;       /* Item ID in the current control */
  uint16 wActionType;   /* Type of action on selection */
  uint64 nActionData;   /* DlgID, AppletID or Setting value depending on action type */
  /* The following two parameters should be replaced by config item ID after moving to IConfig */
  GetFnPtrType   *getfnPtr;     /* Function pointer for get method - valid for menu settings */
  SetFnPtrType   *setfnPtr;     /* Function pointer for set method - valid for menu settings */
} MenuDefinitionType;

static const MenuDefinitionType MenuActionTbl[] = {
  /* Main menu */
#ifdef FEATURE_APP_CONTACT
  {IDD_CORE, IDC_ICONV_CORE, IDL_CONTACT, LAUNCH_APPLET, AEECLSID_CONTACT_APP, NULL, NULL },
#endif
#ifdef FEATURE_WMS_APP
  {IDD_CORE, IDC_ICONV_CORE, IDL_WMS, LAUNCH_APPLET, AEECLSID_WMSAPP, NULL, NULL },
#endif
#ifdef FEATURE_APP_RECENTCALLS
  {IDD_CORE, IDC_ICONV_CORE, IDL_RECENTCALL, LAUNCH_APPLET, AEECLSID_RECENTCALLSAPP, NULL, NULL},
#endif
  {IDD_CORE, IDC_ICONV_CORE, IDL_MY_INFO, LAUNCH_DIALOG, IDD_MYINFO, NULL, NULL},
  {IDD_CORE, IDC_ICONV_CORE, IDL_SETTINGS, LAUNCH_DIALOG, IDD_SETTINGS, NULL, NULL},
  {IDD_CORE, IDC_ICONV_CORE, IDL_MEDIAPLAYER, LAUNCH_APPLET, AEECLSID_MEDIAPLAYER, NULL, NULL},
#ifdef FEATURE_APP_WAP
  {IDD_CORE, IDC_ICONV_CORE, IDL_BROWSER, LAUNCH_APPLET, AEECLSID_BROWSER, NULL, NULL},
#endif
#if MIN_BREW_VERSION(3,0)
  {IDD_CORE, IDC_ICONV_CORE, IDL_APPMANAGER, LAUNCH_APPLET, AEECLSID_APPMANAGER, NULL, NULL},
#else
  {IDD_CORE, IDC_ICONV_CORE, IDL_APPMANAGER, LAUNCH_APPLET, AEECLSID_APPMGR_BID, NULL, NULL},
#endif
#ifdef FEATURE_APP_GPSONE
  {IDD_CORE, IDC_ICONV_CORE, IDL_GPS, LAUNCH_APPLET, AEECLSID_GPSONE, NULL, NULL},
#endif
#ifdef FEATURE_APP_CAT
  {IDD_CORE, IDC_ICONV_CORE, IDL_CATAPP, LAUNCH_APPLET, AEECLSID_CATAPP, NULL, NULL},
#endif
#ifdef FEATURE_APP_FLDDBG
  {IDD_CORE, IDC_ICONV_CORE, IDL_DEBUG, LAUNCH_APPLET, AEECLSID_FIELDDEBUGAPP, NULL, NULL},
#endif
#ifdef FEATURE_JAVA
  {IDD_CORE, IDC_ICONV_CORE, IDL_JAVA, LAUNCH_APPLET, AEECLSID_JAVA_AMS, NULL, NULL},
#endif
#ifdef FEATURE_APP_QCAMERA
  {IDD_CORE, IDC_ICONV_CORE, IDL_QCAMERA, LAUNCH_APPLET, AEECLSID_QCAMERA, NULL, NULL},
#endif
#ifdef FEATURE_APP_QCAMCORDER
  {IDD_CORE, IDC_ICONV_CORE, IDL_QCAMCORDER, LAUNCH_APPLET, AEECLSID_QCAMCORDER, NULL, NULL},
#endif
#ifdef FEATURE_QVPHONE_APP
  {IDD_CORE, IDC_ICONV_CORE, IDL_VIDEOPHONE, LAUNCH_APPLET, AEECLSID_QVIDEOPHONE, NULL, NULL},
#endif /* FEATURE_QVPHONE_APP */
#ifdef FEATURE_APP_DIALER
  {IDD_CORE, IDC_ICONV_CORE, IDL_DIALER, LAUNCH_APPLET, AEECLSID_DIALER, NULL, NULL },
#endif

#ifdef FEATURE_APP_DATASTATS
  {IDD_CORE, IDC_ICONV_CORE, IDL_DATASTATS, LAUNCH_APPLET, AEECLSID_DATASTATS, NULL, NULL },
#endif
#ifdef FEATURE_APP_BLUETOOTH
  {IDD_CORE, IDC_ICONV_CORE, IDL_BLUETOOTH, LAUNCH_APPLET, AEECLSID_BLUETOOTH_APP, NULL, NULL},
#endif
#if defined(FEATURE_APP_MMS)
  {IDD_CORE, IDC_ICONV_CORE, IDL_MMS, LAUNCH_APPLET, AEECLSID_TMT_MMS_APP, NULL, NULL},
#endif

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
   {IDD_CORE, IDC_ICONV_CORE, IDL_IMS_VIDEOSHARE, LAUNCH_APPLET, AEECLSID_IMS_VIDEOSHARE, NULL, NULL},
#endif
   {IDD_CORE, IDC_ICONV_CORE, IDL_IMS_SETTINGS, LAUNCH_APPLET, AEECLSID_IMS_SETTINGS, NULL, NULL},
#endif

#ifdef FEATURE_ALARMCLOCK_APP
   {IDD_CORE, IDC_ICONV_CORE, IDL_ALARMCLOCK, LAUNCH_APPLET, AEECLSID_ALARMCLOCK, NULL, NULL},
#endif

#ifdef FEATURE_FMRADIO_APP
  {IDD_CORE, IDC_ICONV_CORE, IDL_FMRADIO, LAUNCH_APPLET, AEECLSID_FMRADIOAPP, NULL, NULL},
#endif

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  {IDD_SELECT_CARD, IDC_SELECT_CARD, IDL_SELECT_CARD1, MENU_SETTING,
   COREAPP_CARD1, GetCards, NULL},
  {IDD_SELECT_CARD, IDC_SELECT_CARD, IDL_SELECT_CARD2, MENU_SETTING,
   COREAPP_CARD2, GetCards, NULL},
  {IDD_SELECT_CARD1, IDC_MENU_SELECT_CARD1, IDL_MENU_SELECT_CARD1, MENU_SETTING,
   COREAPP_CARD1, GetCards, NULL},
  {IDD_SELECT_TECH_FOR_BOTH_CARDS, IDC_SELECT_TECHNOLOGY, IDL_SK_SELECT_DONE, LAUNCH_DIALOG,
   IDD_CORE, NULL, NULL},
  {IDD_SELECT_TECH_FOR_BOTH_CARDS, IDC_SK_SELECT_TECHNOLOGY, IDL_SK_DEFAULT, LAUNCH_DIALOG,
   IDD_CORE, NULL, NULL},
  {IDD_SELECT_TECH_FOR_CARD_1, IDC_SELECT_TECH_CARD_1, NULL, NULL,NULL, NULL, NULL},
  {IDD_SELECT_TECH_FOR_CARD_2, IDC_SELECT_TECH_CARD_2, NULL, NULL,NULL, NULL, NULL},
#endif

  /* Settings menu Related */
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_SOUND, LAUNCH_DIALOG, IDD_SOUND, NULL, NULL},
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_DISPLAY,LAUNCH_DIALOG, IDD_DISPLAY, NULL, NULL},
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_SECURITY, LAUNCH_DIALOG, IDD_ENTERNVCODE, NULL, NULL},
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_EXTRAS, LAUNCH_DIALOG, IDD_EXTRAS, NULL, NULL},
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_KEYGUARD, LAUNCH_DIALOG, IDD_KEYGUARD, NULL, NULL},
#ifdef FEATURE_NETSETTINGS_APP
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_NETWORK, LAUNCH_APPLET, AEECLSID_NETSETTINGS, NULL, NULL},
#endif
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_CPHS, LAUNCH_DIALOG, IDD_CPHS, NULL, NULL},
#ifdef FEATURE_AUXSETTINGS_APP 
#ifdef FEATURE_WLAN
  {IDD_SETTINGS, IDC_MENU_SETTINGS, IDL_WLAN, LAUNCH_APPLET, AEECLSID_AUXSETTINGS, NULL, NULL},
#endif
#endif

  /* Sounds menu Related */
  {IDD_SOUND, IDC_MENU_SOUND, IDL_VOLUME, LAUNCH_DIALOG, IDD_VOLUME, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_RINGTYPE, LAUNCH_DIALOG, IDD_RINGTYPE, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_DTMFLEN, LAUNCH_DIALOG, IDD_DTMFLENGTH, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_SMARTSOUND, LAUNCH_DIALOG, IDD_SMARTSOUND, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_SERVICEALERT, LAUNCH_DIALOG, IDD_SERVICEALERT, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_SILENCEALL, LAUNCH_DIALOG, IDD_SILENCEALL, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_MUTE, LAUNCH_DIALOG, IDD_MUTE, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_TTY, LAUNCH_DIALOG, IDD_TTY, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_SOUND_DEVICE, LAUNCH_DIALOG, IDD_SOUND_DEVICE_DIALOG, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_INCALL_MIXING, LAUNCH_DIALOG, IDD_INCALL_MIXING, NULL, NULL},
  {IDD_SOUND, IDC_MENU_SOUND, IDL_SIDETONE, LAUNCH_DIALOG, IDD_SIDETONE, NULL, NULL},

  /* Sidetone Menu */
  {IDD_SIDETONE, IDC_MENU_SIDETONE, IDL_SIDETONE_ON, MENU_SETTING,
   SND_SIDETONE_NORMAL_OPERATION, GetSidetone, SetSidetone},
  {IDD_SIDETONE, IDC_MENU_SIDETONE, IDL_SIDETONE_OFF, MENU_SETTING, FALSE,
   SND_SIDETONE_FORCE_DISABLED, SetSidetone},

  /* Volume menu related */
  {IDD_VOLUME, IDC_MENU_VOLUME, IDL_RINGERVOL, LAUNCH_DIALOG, IDD_RINGERVOLUME, NULL, NULL},
  {IDD_VOLUME, IDC_MENU_VOLUME, IDL_KEYBEEPVOL, LAUNCH_DIALOG, IDD_KEYBEEPVOLUME, NULL, NULL},
  {IDD_VOLUME, IDC_MENU_VOLUME, IDL_EARPIECEVOL, LAUNCH_DIALOG, IDD_EARPIECEVOLUME, NULL, NULL},
  {IDD_VOLUME, IDC_MENU_VOLUME, IDL_MESSAGEVOL, LAUNCH_DIALOG, IDD_MESSAGEVOLUME, NULL, NULL},
  {IDD_VOLUME, IDC_MENU_VOLUME, IDL_MMVOL, LAUNCH_DIALOG, IDD_RINGERVOLUME, NULL, NULL},

  /* Ringer volume related */
  {IDD_RINGERVOLUME, IDC_MENU_RINGERVOLUME, IDL_RINGERVOLUME_OFF, MENU_SETTING,
   UISND_VOL_OFF, GetRingerVolume, SetRingerVolume},
  {IDD_RINGERVOLUME, IDC_MENU_RINGERVOLUME, IDL_RINGERVOLUME_1, MENU_SETTING,
   UISND_1ST_VOL, GetRingerVolume, SetRingerVolume},
  {IDD_RINGERVOLUME, IDC_MENU_RINGERVOLUME, IDL_RINGERVOLUME_2, MENU_SETTING,
   UISND_2ND_VOL, GetRingerVolume, SetRingerVolume},
  {IDD_RINGERVOLUME, IDC_MENU_RINGERVOLUME, IDL_RINGERVOLUME_3, MENU_SETTING,
   UISND_3RD_VOL, GetRingerVolume, SetRingerVolume},
  {IDD_RINGERVOLUME, IDC_MENU_RINGERVOLUME, IDL_RINGERVOLUME_4, MENU_SETTING,
   UISND_4TH_VOL, GetRingerVolume, SetRingerVolume},

  /* Keybeep volume related */
  {IDD_KEYBEEPVOLUME, IDC_MENU_KEYBEEPVOLUME, IDL_KEYBEEPVOLUME_OFF, MENU_SETTING,
   UISND_VOL_OFF, GetKeyBeepVolume, SetKeyBeepVolume},
  {IDD_KEYBEEPVOLUME, IDC_MENU_KEYBEEPVOLUME, IDL_KEYBEEPVOLUME_1, MENU_SETTING,
   UISND_1ST_VOL, GetKeyBeepVolume, SetKeyBeepVolume},
  {IDD_KEYBEEPVOLUME, IDC_MENU_KEYBEEPVOLUME, IDL_KEYBEEPVOLUME_2, MENU_SETTING,
   UISND_2ND_VOL, GetKeyBeepVolume, SetKeyBeepVolume},
  {IDD_KEYBEEPVOLUME, IDC_MENU_KEYBEEPVOLUME, IDL_KEYBEEPVOLUME_3, MENU_SETTING,
   UISND_3RD_VOL, GetKeyBeepVolume, SetKeyBeepVolume},
  {IDD_KEYBEEPVOLUME, IDC_MENU_KEYBEEPVOLUME, IDL_KEYBEEPVOLUME_4, MENU_SETTING,
   UISND_4TH_VOL, GetKeyBeepVolume, SetKeyBeepVolume},

  /* Earpiece volume related */
  {IDD_EARPIECEVOLUME, IDC_MENU_EARPIECEVOLUME, IDL_EARPIECEVOLUME_1, MENU_SETTING,
   UISND_1ST_VOL, GetEarpieceVolume, SetEarpieceVolume},
  {IDD_EARPIECEVOLUME, IDC_MENU_EARPIECEVOLUME, IDL_EARPIECEVOLUME_2, MENU_SETTING,
   UISND_2ND_VOL, GetEarpieceVolume, SetEarpieceVolume},
  {IDD_EARPIECEVOLUME, IDC_MENU_EARPIECEVOLUME, IDL_EARPIECEVOLUME_3, MENU_SETTING,
   UISND_3RD_VOL, GetEarpieceVolume, SetEarpieceVolume},
  {IDD_EARPIECEVOLUME, IDC_MENU_EARPIECEVOLUME, IDL_EARPIECEVOLUME_4, MENU_SETTING,
   UISND_4TH_VOL, GetEarpieceVolume, SetEarpieceVolume},

  /* Message volume related */
  {IDD_MESSAGEVOLUME, IDC_MENU_MESSAGEVOLUME, IDL_MESSAGEVOLUME_OFF, MENU_SETTING,
   UISND_VOL_OFF, GetMessageVolume, SetMessageVolume},
  {IDD_MESSAGEVOLUME, IDC_MENU_MESSAGEVOLUME, IDL_MESSAGEVOLUME_1, MENU_SETTING,
   UISND_1ST_VOL, GetMessageVolume, SetMessageVolume},
  {IDD_MESSAGEVOLUME, IDC_MENU_MESSAGEVOLUME, IDL_MESSAGEVOLUME_2, MENU_SETTING,
   UISND_2ND_VOL, GetMessageVolume, SetMessageVolume},
  {IDD_MESSAGEVOLUME, IDC_MENU_MESSAGEVOLUME, IDL_MESSAGEVOLUME_3, MENU_SETTING,
   UISND_3RD_VOL, GetMessageVolume, SetMessageVolume},
  {IDD_MESSAGEVOLUME, IDC_MENU_MESSAGEVOLUME, IDL_MESSAGEVOLUME_4, MENU_SETTING,
   UISND_4TH_VOL, GetMessageVolume, SetMessageVolume},

   /* Ringer type related */
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_STANDARD, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_1, MENU_SETTING,
   UISND_RING_1, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_2, MENU_SETTING,
   UISND_RING_2, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_3, MENU_SETTING,
   UISND_RING_3, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_4, MENU_SETTING,
   UISND_RING_4, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_5, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_6, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_7, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_8, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_9, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_10, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_11, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_12, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_13, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_14, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_15, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_16, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_17, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_18, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},
  {IDD_RINGTYPE, IDC_MENU_RINGTYPE, IDL_RINGTYPE_19, MENU_SETTING,
   UISND_RING_NORMAL, GetRingerType, SetRingerType},

   /* Beep length */
  {IDD_DTMFLENGTH, IDC_MENU_DTMFLENGTH, IDL_DTMFLENGTH_BURST, MENU_SETTING,
   OEMNV_DTMFBURST, GetBeepLength, SetBeepLength},
  {IDD_DTMFLENGTH, IDC_MENU_DTMFLENGTH, IDL_DTMFLENGTH_CONT, MENU_SETTING,
   OEMNV_DTMFCONT, GetBeepLength, SetBeepLength},
  {IDD_DTMFLENGTH, IDC_MENU_DTMFLENGTH, IDL_DTMFLENGTH_OFF, MENU_SETTING,
   OEMNV_DTMFOFF, GetBeepLength, SetBeepLength},


  /* Smart Sound */
  {IDD_SMARTSOUND, IDC_MENU_SMARTSOUND, IDL_SMARTSOUND_ON, MENU_SETTING,
   TRUE, GetSmartSound, SetSmartSound},
  {IDD_SMARTSOUND, IDC_MENU_SMARTSOUND, IDL_SMARTSOUND_OFF, MENU_SETTING,
   FALSE, GetSmartSound, SetSmartSound},

  /* Service Alert */
  {IDD_SERVICEALERT, IDC_MENU_SERVICEALERT, IDL_SERVICEALERT_ON, MENU_SETTING,
   TRUE, GetServiceAlert, SetServiceAlert},
  {IDD_SERVICEALERT, IDC_MENU_SERVICEALERT, IDL_SERVICEALERT_OFF, MENU_SETTING,
   FALSE, GetServiceAlert, SetServiceAlert},

  /* Silence All */
  {IDD_SILENCEALL, IDC_MENU_SILENCEALL, IDL_SILENCEALL_ON, MENU_SETTING,
   TRUE, GetSilenceAll, SetSilenceAll},
  {IDD_SILENCEALL, IDC_MENU_SILENCEALL, IDL_SILENCEALL_OFF, MENU_SETTING,
   FALSE, GetSilenceAll, SetSilenceAll},

  /* Mute */
  {IDD_MUTE, IDC_MENU_MUTE, IDL_MUTE_ON, MENU_SETTING,
   TRUE, GetMute, SetMute},
  {IDD_MUTE, IDC_MENU_MUTE, IDL_MUTE_OFF, MENU_SETTING,
   FALSE, GetMute, SetMute},

  /* InCall Mixing */
  {IDD_INCALL_MIXING, IDC_INCALL_MIXING, IDL_INCALL_MIXING_ON, MENU_SETTING,
   TRUE, GetIncallMixing, SetIncallMixing},
  {IDD_INCALL_MIXING, IDC_INCALL_MIXING, IDL_INCALL_MIXING_OFF, MENU_SETTING,
   FALSE, GetIncallMixing, SetIncallMixing},

#ifdef FEATURE_TTY
  /* TTY */
  {IDD_TTY, IDC_MENU_TTY, IDL_TTY_FULL, MENU_SETTING,
   OEMNV_TTY_FULL, GetTTY, SetTTY},
  {IDD_TTY, IDC_MENU_TTY, IDL_TTY_WITH_TALK, MENU_SETTING,
   OEMNV_TTY_TALK, GetTTY, SetTTY},
  {IDD_TTY, IDC_MENU_TTY, IDL_TTY_WITH_HEAR, MENU_SETTING,
   OEMNV_TTY_HEAR, GetTTY, SetTTY},
  {IDD_TTY, IDC_MENU_TTY, IDL_TTY_OFF, MENU_SETTING,
   OEMNV_TTY_OFF, GetTTY, SetTTY},
#endif

  /* Sound Device */
#ifdef FEATURE_TTY
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_TTY_HEADSET,
    MENU_SETTING, SND_DEVICE_TTY_HEADSET, GetSoundDevice, SetSoundDevice},
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_TTY_HCO,
    MENU_SETTING, SND_DEVICE_TTY_HCO, GetSoundDevice, SetSoundDevice},
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_TTY_VCO,
    MENU_SETTING, SND_DEVICE_TTY_VCO, GetSoundDevice, SetSoundDevice},
#endif
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_STEREO_HEADSET,
    MENU_SETTING, SND_DEVICE_STEREO_HEADSET, GetSoundDevice, SetSoundDevice},
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_HEADSET,
    MENU_SETTING, SND_DEVICE_HEADSET, GetSoundDevice, SetSoundDevice},
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_HFK,
    MENU_SETTING, SND_DEVICE_HFK, GetSoundDevice, SetSoundDevice},
#ifdef FEATURE_USB_CARKIT
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_USB_HFK,
    MENU_SETTING, SND_DEVICE_USB, GetSoundDevice, SetSoundDevice},
#endif
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_STEREO_DAC,
    MENU_SETTING, SND_DEVICE_SDAC, GetSoundDevice, SetSoundDevice},
#endif
#ifdef FEATURE_APP_BLUETOOTH
#ifdef FEATURE_BT_EXTPF_AV_A2DP_SCO
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_BT_A2DP_SCO,
    MENU_SETTING, SND_DEVICE_BT_A2DP_SCO_HEADSET, GetSoundDevice, SetSoundDevice},
#endif /* FEATURE_BT_EXTPF_AV */
#ifdef FEATURE_BT_AG
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_BT_HEADSET,
    MENU_SETTING, SND_DEVICE_BT_HEADSET, GetSoundDevice, SetSoundDevice},
#endif /* FEATURE_BT_AG */
#ifdef FEATURE_BT_EXTPF_AV
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_BT_A2DP,
    MENU_SETTING, SND_DEVICE_BT_A2DP_HEADSET, GetSoundDevice, SetSoundDevice},
#endif /* FEATURE_BT_EXTPF_AV */
#endif /* FEATURE_APP_BLUETOOTH */
#ifdef FEATURE_SPEAKER_PHONE
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_SPEAKERPHONE,
    MENU_SETTING, SND_DEVICE_SPEAKER_PHONE, GetSoundDevice, SetSoundDevice},
#endif /* FEATURE_SPEAKER_PHONE */
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_HANDSET,
    MENU_SETTING, SND_DEVICE_HANDSET, GetSoundDevice, SetSoundDevice},
  {IDD_SOUND_DEVICE_DIALOG, IDC_MENU_SOUND_DEVICE, IDL_SOUNDDEV_AUTOSELECT,
    MENU_SETTING, SND_DEVICE_MAX, GetSoundDevice, SetSoundDevice},



  /* Display menu */
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_BACKLIGHT, LAUNCH_DIALOG, IDD_BACKLIGHT, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_BANNER, LAUNCH_DIALOG, IDD_BANNER_VIEW, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_AUTOHYPHEN, LAUNCH_DIALOG, IDD_AUTOHYPHEN, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_SETTIME, LAUNCH_DIALOG, IDD_SETTIME_VIEW, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_SETSECURETIME, LAUNCH_APPLET, AEECLSID_AUXSETTINGS, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_SETLCD, LAUNCH_DIALOG, IDD_LCD, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_CONTRAST, LAUNCH_DIALOG, IDD_CONTRAST, NULL, NULL},
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_DISP_DISABLE_BG_IMAGE, LAUNCH_DIALOG, IDD_DISABLE_BG_IMAGE, NULL, NULL},
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_LANGUAGE_SELECT, LAUNCH_DIALOG, IDD_LANGUAGE_SELECT, NULL, NULL},
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#if defined(FEATURE_DISPLAY_CONTENT_MAP)
  {IDD_DISPLAY, IDC_MENU_DISPLAY, IDL_DISPLAY_MAP, LAUNCH_DIALOG, IDD_DISPLAY_MAPPING, NULL, NULL},
#endif /* defined(FEATURE_DISPLAY_CONTENT_MAP) */

    /* Backlight Menu Selection */
  {IDD_BACKLIGHT, IDC_MENU_BACKLIGHT, IDL_DURATION, LAUNCH_DIALOG, IDD_DURATION, NULL, NULL},
  {IDD_BACKLIGHT, IDC_MENU_BACKLIGHT, IDL_INTENSITY, LAUNCH_DIALOG, IDD_INTENSITY, NULL, NULL},

  /* Duration Menu */
  {IDD_DURATION, IDC_MENU_DURATION, IDL_DURATION_ALWAYSON, MENU_SETTING,
   UI_BKLT_TIME_MAX, GetBackLightDuration, SetBackLightDuration},
  {IDD_DURATION, IDC_MENU_DURATION, IDL_DURATION_5, MENU_SETTING,
   5, GetBackLightDuration, SetBackLightDuration},
  {IDD_DURATION, IDC_MENU_DURATION, IDL_DURATION_10, MENU_SETTING,
   10, GetBackLightDuration, SetBackLightDuration},
  {IDD_DURATION, IDC_MENU_DURATION, IDL_DURATION_30, MENU_SETTING,
   30, GetBackLightDuration, SetBackLightDuration},
  {IDD_DURATION, IDC_MENU_DURATION, IDL_DURATION_ALWAYSOFF, MENU_SETTING,
   UI_BKLT_TIME_NONE, GetBackLightDuration, SetBackLightDuration},

#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
  /* Intensity Menu */
  {IDD_INTENSITY, IDC_MENU_INTENSITY, IDL_INTENSITY_0, MENU_SETTING,
   LCD_BACKLIGHT_LVL_0, GetBackLightIntensity, SetBackLightIntensity},
  {IDD_INTENSITY, IDC_MENU_INTENSITY, IDL_INTENSITY_1, MENU_SETTING,
   LCD_BACKLIGHT_LVL_1, GetBackLightIntensity, SetBackLightIntensity},
  {IDD_INTENSITY, IDC_MENU_INTENSITY, IDL_INTENSITY_2, MENU_SETTING,
   LCD_BACKLIGHT_LVL_2, GetBackLightIntensity, SetBackLightIntensity},
  {IDD_INTENSITY, IDC_MENU_INTENSITY, IDL_INTENSITY_3, MENU_SETTING,
   LCD_BACKLIGHT_LVL_3, GetBackLightIntensity, SetBackLightIntensity},
  {IDD_INTENSITY, IDC_MENU_INTENSITY, IDL_INTENSITY_4, MENU_SETTING,
   LCD_BACKLIGHT_LVL_4, GetBackLightIntensity, SetBackLightIntensity},
#endif

  /* Autohyphen Menu */
  {IDD_AUTOHYPHEN, IDC_MENU_AUTOHYPHEN, IDL_AUTOHYPHEN_ON, MENU_SETTING,
   TRUE, GetAutoHyphen, SetAutoHyphen},
  {IDD_AUTOHYPHEN, IDC_MENU_AUTOHYPHEN, IDL_AUTOHYPHEN_OFF, MENU_SETTING,
   FALSE, GetAutoHyphen, SetAutoHyphen},

    /* Security Menu Selection */
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_RESTRICT, LAUNCH_DIALOG, IDD_RESTRICT, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_RESTORESETTINGS, LAUNCH_DIALOG, IDD_RESTORE_SETTINGS, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_PINLOCK, LAUNCH_DIALOG, IDD_PINLOCK, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_PINCHANGE, LAUNCH_DIALOG, IDD_PINCHANGE, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_RTRE_CONFIG, LAUNCH_DIALOG, IDD_RTRE_CONFIG, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_USER_SLOT_PREF, LAUNCH_DIALOG, IDD_USER_SLOT_PREF, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_PERSO_FEATURE_IND, LAUNCH_DIALOG, IDD_PERSO_FEATURE_INDICATION, NULL, NULL},
  {IDD_SECURITY, IDC_MENU_SECURITY, IDL_PERSO_FEATURE_DATA, LAUNCH_DIALOG, IDD_PERSO_FEATURE_DATA, NULL, NULL},

    /* PIN Lock Menu Selection */
  {IDD_PINLOCK, IDC_MENU_PINLOCK, IDL_PINLOCK_1, LAUNCH_DIALOG, IDD_PINLOCK1, NULL, NULL},
  {IDD_PINLOCK, IDC_MENU_PINLOCK, IDL_PINLOCK_2, LAUNCH_DIALOG, IDD_PINLOCK2, NULL, NULL},
  {IDD_PINLOCK, IDC_MENU_PINLOCK, IDL_PINLOCK_NV, LAUNCH_DIALOG, IDD_NVLOCK, NULL, NULL},
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  {IDD_PINLOCK, IDC_MENU_PINLOCK, IDL_PINLOCK_1_SLOT2, LAUNCH_DIALOG, IDD_PINLOCK1_SLOT2, NULL, NULL},
  {IDD_PINLOCK, IDC_MENU_PINLOCK, IDL_PINLOCK_2_SLOT2, LAUNCH_DIALOG, IDD_PINLOCK2_SLOT2, NULL, NULL},
#endif

   /* Restrict */
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_EFS, MENU_SETTING,
   COREUTILS_EFS, GetRestrict, SetRestrict},
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_SPEED, MENU_SETTING,
   COREUTILS_SPEED_DIAL, GetRestrict, SetRestrict},
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_ADN_SLOT1, MENU_SETTING,
   COREUTILS_ADN_SLOT1, GetRestrict, SetRestrict},
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_ADN_SLOT2, MENU_SETTING,
   COREUTILS_ADN_SLOT2, GetRestrict, SetRestrict},
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_FDN_SLOT1, MENU_SETTING,
   COREUTILS_FDN_SLOT1, GetRestrict, SetRestrict},
  {IDD_RESTRICT, IDC_MENU_RESTRICT, IDL_RESTRICT_FDN_SLOT2, MENU_SETTING,
   COREUTILS_FDN_SLOT2, GetRestrict, SetRestrict},

  /* Pin lock 1 */
  {IDD_PINLOCK1, IDC_MENU_PINLOCK1, IDL_PINLOCK1_ON, MENU_SETTING,
   TRUE, GetPINLock1, SetPINLock1},
  {IDD_PINLOCK1, IDC_MENU_PINLOCK1, IDL_PINLOCK1_OFF, MENU_SETTING,
   FALSE, GetPINLock1, SetPINLock1},

  /* Pin Lock 2 */
  {IDD_PINLOCK2, IDC_MENU_PINLOCK2, IDL_PINLOCK2_ON, MENU_SETTING,
   TRUE, GetPINLock2, SetPINLock2},
  {IDD_PINLOCK2, IDC_MENU_PINLOCK2, IDL_PINLOCK2_OFF, MENU_SETTING,
   FALSE, GetPINLock2, SetPINLock2},

  /* NV Lock */
  {IDD_NVLOCK, IDC_MENU_NVLOCK, IDL_NVLOCK_ON, MENU_SETTING,
   TRUE, GetNVLock, SetNVLock},
  {IDD_NVLOCK, IDC_MENU_NVLOCK, IDL_NVLOCK_OFF, MENU_SETTING,
   FALSE, GetNVLock, SetNVLock},

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Pin lock 1 Slot 2 */
  {IDD_PINLOCK1_SLOT2, IDC_MENU_PINLOCK1_SLOT2, IDL_PINLOCK1_SLOT2_ON, MENU_SETTING,
   TRUE, GetPINLock1Slot2, SetPINLock1Slot2},
  {IDD_PINLOCK1_SLOT2, IDC_MENU_PINLOCK1_SLOT2, IDL_PINLOCK1_SLOT2_OFF, MENU_SETTING,
   FALSE, GetPINLock1Slot2, SetPINLock1Slot2},

  /* Pin Lock 2 */
  {IDD_PINLOCK2_SLOT2, IDC_MENU_PINLOCK2_SLOT2, IDL_PINLOCK2_SLOT2_ON, MENU_SETTING,
   TRUE, GetPINLock2Slot2, SetPINLock2Slot2},
  {IDD_PINLOCK2_SLOT2, IDC_MENU_PINLOCK2_SLOT2, IDL_PINLOCK2_SLOT2_OFF, MENU_SETTING,
   FALSE, GetPINLock2Slot2, SetPINLock2Slot2},

#endif

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* RTRE Config */
  {IDD_RTRE_CONFIG, IDC_MENU_RTRE_CONFIG, IDL_RTRE_SIM, MENU_SETTING,
   NV_RTRE_CONFIG_RUIM_ONLY, GetRTREConfig, SetRTREConfig},
  {IDD_RTRE_CONFIG, IDC_MENU_RTRE_CONFIG, IDL_RTRE_NV, MENU_SETTING,
   NV_RTRE_CONFIG_NV_ONLY, GetRTREConfig, SetRTREConfig},
  {IDD_RTRE_CONFIG, IDC_MENU_RTRE_CONFIG, IDL_RTRE_SIMNV, MENU_SETTING,
   NV_RTRE_CONFIG_RUIM_OR_DROP_BACK, GetRTREConfig, SetRTREConfig},
#endif

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   /* user slot pref */
  {IDD_USER_SLOT_PREF, IDC_MENU_USER_SLOT_PREF, IDL_USER_SLOT_PREF_GSM, LAUNCH_DIALOG,
   IDD_USER_SLOT_PREF_GSM, NULL, NULL},
  {IDD_USER_SLOT_PREF, IDC_MENU_USER_SLOT_PREF, IDL_USER_SLOT_PREF_CDMA, LAUNCH_DIALOG,
   IDD_USER_SLOT_PREF_CDMA, NULL, NULL},


  {IDD_USER_SLOT_PREF_GSM, IDC_MENU_USER_SLOT_PREF_GSM, IDL_USER_SLOT_PREF_GSM_1, MENU_SETTING,
   AEECARD_SLOT1, GetUserSlotPrefGSM, SetUserSlotPrefGSM},
  {IDD_USER_SLOT_PREF_GSM, IDC_MENU_USER_SLOT_PREF_GSM, IDL_USER_SLOT_PREF_GSM_2, MENU_SETTING,
   AEECARD_SLOT2, GetUserSlotPrefGSM, SetUserSlotPrefGSM},
  {IDD_USER_SLOT_PREF_GSM, IDC_MENU_USER_SLOT_PREF_GSM, IDL_USER_SLOT_PREF_GSM_NONE, MENU_SETTING,
  AEECARD_SLOT_NONE, GetUserSlotPrefGSM, SetUserSlotPrefGSM},

  {IDD_USER_SLOT_PREF_CDMA, IDC_MENU_USER_SLOT_PREF_CDMA, IDL_USER_SLOT_PREF_CDMA1, MENU_SETTING,
   AEECARD_SLOT1, GetUserSlotPrefCDMA, SetUserSlotPrefCDMA},
  {IDD_USER_SLOT_PREF_CDMA, IDC_MENU_USER_SLOT_PREF_CDMA, IDL_USER_SLOT_PREF_CDMA2, MENU_SETTING,
   AEECARD_SLOT2, GetUserSlotPrefCDMA, SetUserSlotPrefCDMA},
  {IDD_USER_SLOT_PREF_CDMA, IDC_MENU_USER_SLOT_PREF_CDMA, IDL_USER_SLOT_PREF_CDMANONE, MENU_SETTING,
   AEECARD_SLOT_NONE, GetUserSlotPrefCDMA, SetUserSlotPrefCDMA},
#endif

#ifdef FEATURE_MMGSDI_PERSONALIZATION
   {IDD_PERSO_FEATURE_DATA, IDC_MENU_PERSO_FEATURE_DATA, IDL_NW_PERSO_DATA, LAUNCH_DIALOG, IDD_ENTER_NW_PERSO_DATA, NULL, NULL},
   {IDD_PERSO_FEATURE_DATA, IDC_MENU_PERSO_FEATURE_DATA, IDL_NW_SUB_PERSO_DATA, LAUNCH_DIALOG, IDD_ENTER_NW_SUB_PERSO_DATA, NULL, NULL},
   {IDD_PERSO_FEATURE_DATA, IDC_MENU_PERSO_FEATURE_DATA, IDL_SP_PERSO_DATA, LAUNCH_DIALOG, IDD_ENTER_SP_PERSO_DATA, NULL, NULL},
   {IDD_PERSO_FEATURE_DATA, IDC_MENU_PERSO_FEATURE_DATA, IDL_CP_PERSO_DATA, LAUNCH_DIALOG, IDD_ENTER_CP_PERSO_DATA, NULL, NULL},
   {IDD_PERSO_FEATURE_DATA, IDC_MENU_PERSO_FEATURE_DATA, IDL_SIM_PERSO_DATA, LAUNCH_DIALOG, IDD_ENTER_SIM_PERSO_DATA, NULL, NULL},
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

    /* Extras Menu Selection */
#ifdef FEATURE_AUXSETTINGS_APP
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_SIOCONFIGURATION, LAUNCH_APPLET, AEECLSID_AUXSETTINGS, NULL, NULL},
#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_USBDEVICESWITCH, LAUNCH_APPLET, AEECLSID_AUXSETTINGS, NULL, NULL},
#endif
#endif // FEATURE_AUXSETTINGS_APP
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_AUTOANSWER, LAUNCH_DIALOG, IDD_AUTOANSWER, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_AUTORETRY, LAUNCH_DIALOG, IDD_AUTORETRY, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_ONETOUCH, LAUNCH_DIALOG, IDD_ONETOUCH, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_PRIVACY, LAUNCH_DIALOG, IDD_PRIVACY, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_VR, LAUNCH_DIALOG, IDD_VR, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_GAME_MODE, LAUNCH_DIALOG, IDD_GAME_MODE, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_CUG, LAUNCH_DIALOG, IDD_CUG, NULL, NULL},
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_CCBS, LAUNCH_DIALOG, IDD_AUTO_ACCEPT_RECALL, NULL, NULL},
#ifdef FEATURE_USRSYSCLOCK
  {IDD_EXTRAS, IDC_MENU_EXTRAS, IDL_NITZ, LAUNCH_DIALOG, IDD_NITZ, NULL, NULL},
#endif // FEATURE_USRSYSCLOCK

  {IDD_AUTO_ACCEPT_RECALL, IDC_CCBS_AUTO_ACCEPT, IDL_CCBS_ACCEPT, MENU_SETTING, OEM_CCBS_ACCEPT,
  GetCCBSSetting, SetCCBSSetting},
  {IDD_AUTO_ACCEPT_RECALL, IDC_CCBS_AUTO_ACCEPT, IDL_CCBS_REJECT, MENU_SETTING, OEM_CCBS_REJECT,
  GetCCBSSetting, SetCCBSSetting},
  {IDD_AUTO_ACCEPT_RECALL, IDC_CCBS_AUTO_ACCEPT, IDL_CCBS_HOLD_N_ACCEPT, MENU_SETTING, OEM_CCBS_HOLD_N_ACCEPT,
  GetCCBSSetting, SetCCBSSetting},
  {IDD_AUTO_ACCEPT_RECALL, IDC_CCBS_AUTO_ACCEPT, IDL_CCBS_REL_N_ACCEPT, MENU_SETTING, OEM_CCBS_RELEASE_N_ACCEPT,
  GetCCBSSetting, SetCCBSSetting},
  {IDD_AUTO_ACCEPT_RECALL, IDC_CCBS_AUTO_ACCEPT, IDL_CCBS_NONE, MENU_SETTING, OEM_CCBS_NONE,
  GetCCBSSetting, SetCCBSSetting},

  /* Auto answer */
  {IDD_AUTOANSWER, IDC_MENU_AUTOANSWER, IDL_AUTOANSWER_ON, MENU_SETTING,
   TRUE, GetAutoAnswer, SetAutoAnswer},
  {IDD_AUTOANSWER, IDC_MENU_AUTOANSWER, IDL_AUTOANSWER_OFF, MENU_SETTING,
   FALSE, GetAutoAnswer, SetAutoAnswer},

  /* Auto Retry */
  {IDD_AUTORETRY, IDC_MENU_AUTORETRY, IDL_AUTORETRY_OFF, MENU_SETTING,
   0, GetAutoRetry, SetAutoRetry},
  {IDD_AUTORETRY, IDC_MENU_AUTORETRY, IDL_AUTORETRY_10, MENU_SETTING,
   1, GetAutoRetry, SetAutoRetry},
  {IDD_AUTORETRY, IDC_MENU_AUTORETRY, IDL_AUTORETRY_30, MENU_SETTING,
   3, GetAutoRetry, SetAutoRetry},
  {IDD_AUTORETRY, IDC_MENU_AUTORETRY, IDL_AUTORETRY_60, MENU_SETTING,
   6, GetAutoRetry, SetAutoRetry},

  /* One touch */
  {IDD_ONETOUCH, IDC_MENU_ONETOUCH, IDL_ONETOUCH_ON, MENU_SETTING,
   TRUE, GetOneTouch, SetOneTouch},
  {IDD_ONETOUCH, IDC_MENU_ONETOUCH, IDL_ONETOUCH_OFF, MENU_SETTING,
   FALSE, GetOneTouch, SetOneTouch},

  /* Privacy */
  {IDD_PRIVACY, IDC_MENU_PRIVACY, IDL_PRIVACY_ENHANCED, MENU_SETTING,
   TRUE, GetPrivacy, SetPrivacy},
  {IDD_PRIVACY, IDC_MENU_PRIVACY, IDL_PRIVACY_STANDARD, MENU_SETTING,
   FALSE, GetPrivacy, SetPrivacy},

   {IDD_VR, IDC_VR, IDL_VR_ON, MENU_SETTING,  TRUE, GetVR, SetVR},
   {IDD_VR, IDC_VR, IDL_VR_OFF, MENU_SETTING, FALSE, GetVR, SetVR},

#ifdef FEATURE_USRSYSCLOCK
   /* NITZ menu */

  {IDD_NITZ, IDC_MENU_NITZ, IDL_NETWORK_TIME, LAUNCH_DIALOG, IDD_NETWORK_TIME, NULL, NULL},
  {IDD_NITZ, IDC_MENU_NITZ, IDL_DELETE_NITZ_PLMN, LAUNCH_DIALOG, IDD_DELETE_NITZ_PLMN,NULL, NULL},

   /* User time set from network info */
  {IDD_NETWORK_TIME, IDC_MENU_NETWORK_TIME, IDL_NETWORK_TIME_ENABLE, MENU_SETTING,
   TRUE, GetNetworkTime, SetNetworkTime},
  {IDD_NETWORK_TIME, IDC_MENU_NETWORK_TIME, IDL_NETWORK_TIME_DISABLE, MENU_SETTING,
   FALSE, GetNetworkTime, SetNetworkTime},

    /* Delete NITZ PLMN info */
  {IDD_DELETE_NITZ_PLMN, IDC_MENU_DELETE_NITZ_PLMN, IDL_DELETE_NITZ_PLMN_ON, MENU_SETTING,
   TRUE, GetDeleteNITZPLMN, SetDeleteNITZPLMN},
  {IDD_DELETE_NITZ_PLMN, IDC_MENU_DELETE_NITZ_PLMN, IDL_DELETE_NITZ_PLMN_OFF, MENU_SETTING,
   FALSE, GetDeleteNITZPLMN, SetDeleteNITZPLMN},

#endif // FEATURE_USRSYSCLOCK

  /* Keyguard Menu */
  {IDD_KEYGUARD, IDC_MENU_KEYGUARD, IDL_GUARDNOW, MENU_SETTING,
   0, GetKeyguard, SetKeyguard},
  {IDD_KEYGUARD, IDC_MENU_KEYGUARD, IDL_15SAUTOGUARD, MENU_SETTING,
   15, GetKeyguard, SetKeyguard},
  {IDD_KEYGUARD, IDC_MENU_KEYGUARD, IDL_30SAUTOGUARD, MENU_SETTING,
   30, GetKeyguard, SetKeyguard},
  {IDD_KEYGUARD, IDC_MENU_KEYGUARD, IDL_1MAUTOGUARD, MENU_SETTING,
   60, GetKeyguard, SetKeyguard},

   /* Game Mode Menu */
  {IDD_GAME_MODE, IDC_MENU_GAME_MODE, IDL_GAME_MODE_ON, MENU_SETTING,
   TRUE, GetGameMode, SetGameMode},
  {IDD_GAME_MODE, IDC_MENU_GAME_MODE, IDL_GAME_MODE_OFF, MENU_SETTING,
   FALSE, GetGameMode, SetGameMode},

  /* CUG Menu*/
  {IDD_CUG, IDC_MENU_CUG, IDL_CUG_ENABLE, LAUNCH_DIALOG, IDD_CUG_ENABLE, NULL, NULL},
  {IDD_CUG, IDC_MENU_CUG, IDL_CUG_GROUP_ID, LAUNCH_DIALOG, IDD_CUG_GROUP_ID, NULL, NULL},
  {IDD_CUG, IDC_MENU_CUG, IDL_CUG_PREF_GROUP, LAUNCH_DIALOG, IDD_CUG_PREF_GROUP, NULL, NULL},
  {IDD_CUG, IDC_MENU_CUG, IDL_CUG_OUT_ACCESS, LAUNCH_DIALOG, IDD_CUG_OUT_ACCESS, NULL, NULL},

   /* LCD On/Off Menu */
  {IDD_LCD, IDC_LCD, IDL_LCD_5_SEC, MENU_SETTING, COREAPP_LCD_OFF_TIME_5_SEC, GetLCDValue, SetLCDValue},
  {IDD_LCD, IDC_LCD, IDL_LCD_10_SEC, MENU_SETTING, COREAPP_LCD_OFF_TIME_10_SEC, GetLCDValue, SetLCDValue},
  {IDD_LCD, IDC_LCD, IDL_LCD_30_SEC, MENU_SETTING, COREAPP_LCD_OFF_TIME_30_SEC, GetLCDValue, SetLCDValue},
  {IDD_LCD, IDC_LCD, IDL_LCD_2_MIN, MENU_SETTING, COREAPP_LCD_OFF_TIME_2_MIN, GetLCDValue, SetLCDValue},
  {IDD_LCD, IDC_LCD, IDL_LCD_10_MIN, MENU_SETTING, COREAPP_LCD_OFF_TIME_10_MIN, GetLCDValue, SetLCDValue},
  {IDD_LCD, IDC_LCD, IDL_LCD_ALWAYS_ON, MENU_SETTING, 0, GetLCDValue, SetLCDValue},

  /* CUG Enable */
  {IDD_CUG_ENABLE, IDC_MENU_CUG_ENABLE, IDL_CUG_ENABLE_ON, MENU_SETTING,
    TRUE , GetCUGEnable, SetCUGEnable},
  {IDD_CUG_ENABLE, IDC_MENU_CUG_ENABLE, IDL_CUG_ENABLE_OFF, MENU_SETTING,
    FALSE , GetCUGEnable, SetCUGEnable},

   /* CUG Preferred Group */
  {IDD_CUG_PREF_GROUP, IDC_MENU_CUG_PREF_GROUP, IDL_CUG_PREF_GROUP_SUPRESS, MENU_SETTING,
    TRUE , GetCUGPrefGroup, SetCUGPrefGroup},
  {IDD_CUG_PREF_GROUP, IDC_MENU_CUG_PREF_GROUP, IDL_CUG_PREF_GROUP_NO_SUPRESS, MENU_SETTING,
    FALSE , GetCUGPrefGroup, SetCUGPrefGroup},

   /* CUG Out Access */
   {IDD_CUG_OUT_ACCESS, IDC_MENU_CUG_OUT_ACCESS, IDL_CUG_OUT_ACCESS_SUPRESS, MENU_SETTING,
    TRUE , GetCUGOutAccess, SetCUGOutAccess},
    {IDD_CUG_OUT_ACCESS, IDC_MENU_CUG_OUT_ACCESS, IDL_CUG_OUT_ACCESS_NO_SUPRESS, MENU_SETTING,
    FALSE , GetCUGOutAccess, SetCUGOutAccess},

#ifdef FEATURE_ALS
 //  {IDD_CPHS, IDC_MENU_SETTINGS, IDL_CSP, LAUNCH_DIALOG, IDD_CSP, NULL, NULL},
   {IDD_CPHS, IDC_MENU_CPHS, IDL_ALS, LAUNCH_DIALOG, IDD_ALS, NULL, NULL},

   {IDD_ALS, IDC_MENU_ALS, IDL_ALS_LINE_SWITCHING, LAUNCH_DIALOG, IDD_ALS_LINE_SWITCHING, NULL, NULL},
   {IDD_ALS, IDC_MENU_ALS, IDL_ALS_LINE_SELECTION, LAUNCH_DIALOG, IDD_ALS_LINE_SELECTION, NULL, NULL},

   {IDD_ALS_LINE_SWITCHING, IDC_MENU_ALS_LINE_SWITCHING, IDL_ALS_LINE_SWITCH_FORBID, MENU_SETTING,
    AEECM_ALS_LINE_SWITCHING_DISABLED , GetALSLineSwitch, SetALSLineSwitch},
   {IDD_ALS_LINE_SWITCHING, IDC_MENU_ALS_LINE_SWITCHING, IDL_ALS_LINE_SWITCH_ALLOW, MENU_SETTING,
    AEECM_ALS_LINE_SWITCHING_ENABLED , GetALSLineSwitch, SetALSLineSwitch},

   {IDD_ALS_LINE_SELECTION, IDC_MENU_ALS_LINE_SELECTION, IDL_LINE_SELECTION_LINE1, MENU_SETTING,
    AEECM_ALS_LINE_01 , GetALSLineSelected, SetALSLineSelected},
   {IDD_ALS_LINE_SELECTION, IDC_MENU_ALS_LINE_SELECTION, IDL_LINE_SELECTION_LINE2, MENU_SETTING,
    AEECM_ALS_LINE_02 , GetALSLineSelected, SetALSLineSelected},
#endif /*FEATURE_ALS */

  {IDD_DISABLE_BG_IMAGE, IDL_DISABLE_BG_IMAGE, IDC_DISABLE_BG_IMAGE_YES,
   MENU_SETTING, TRUE, GetDisableBgImage, SetDisableBgImage},
  {IDD_DISABLE_BG_IMAGE, IDL_DISABLE_BG_IMAGE, IDC_DISABLE_BG_IMAGE_NO,
   MENU_SETTING, FALSE, GetDisableBgImage, SetDisableBgImage},

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
  {IDD_LANGUAGE_SELECT, IDC_MENU_LANGUAGE_SELECT, IDL_LANG_SEL_ENG,
   MENU_SETTING, COREAPP_LANG_SEL_ENG,
   GetLanguageSelection, SetLanguageSelection},
  {IDD_LANGUAGE_SELECT, IDC_MENU_LANGUAGE_SELECT, IDL_LANG_SEL_GER,
   MENU_SETTING, COREAPP_LANG_SEL_GER,
   GetLanguageSelection, SetLanguageSelection},
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#if defined(FEATURE_DISPLAY_CONTENT_MAP)
  /* Display mapping */
  {IDD_DISPLAY_MAPPING, IDC_MENU_DISPLAY_MAPPING, IDL_DISP_MAP_1,
    LAUNCH_DIALOG, IDD_DISPLAY_MAP_1, NULL, NULL},
  {IDD_DISPLAY_MAPPING, IDC_MENU_DISPLAY_MAPPING, IDL_DISP_MAP_2,
    LAUNCH_DIALOG, IDD_DISPLAY_MAP_2, NULL, NULL},
  {IDD_DISPLAY_MAPPING, IDC_MENU_DISPLAY_MAPPING, IDL_DISP_MAP_3,
    LAUNCH_DIALOG, IDD_DISPLAY_MAP_3, NULL, NULL},
  {IDD_DISPLAY_MAPPING, IDC_MENU_DISPLAY_MAPPING, IDL_DISP_MAP_4,
    LAUNCH_DIALOG, IDD_DISPLAY_MAP_4, NULL, NULL},

  {IDD_DISPLAY_MAP_1, IDC_MENU_DISP_MAP_1, IDL_DISP_MAP_1_PRI,
    MENU_SETTING, COREAPP_LOGICAL_DISP_PRI, GetDisplay1Mapping, SetDisplay1Mapping},
  {IDD_DISPLAY_MAP_1, IDC_MENU_DISP_MAP_1, IDL_DISP_MAP_1_SEC,
    MENU_SETTING, COREAPP_LOGICAL_DISP_SEC, GetDisplay1Mapping, SetDisplay1Mapping},
  {IDD_DISPLAY_MAP_1, IDC_MENU_DISP_MAP_1, IDL_DISP_MAP_1_NONE,
    MENU_SETTING, COREAPP_LOGICAL_DISP_NONE, GetDisplay1Mapping, SetDisplay1Mapping},

  {IDD_DISPLAY_MAP_2, IDC_MENU_DISP_MAP_2, IDL_DISP_MAP_2_PRI,
    MENU_SETTING, COREAPP_LOGICAL_DISP_PRI, GetDisplay2Mapping, SetDisplay2Mapping},
  {IDD_DISPLAY_MAP_2, IDC_MENU_DISP_MAP_2, IDL_DISP_MAP_2_SEC,
    MENU_SETTING, COREAPP_LOGICAL_DISP_SEC, GetDisplay2Mapping, SetDisplay2Mapping},
  {IDD_DISPLAY_MAP_2, IDC_MENU_DISP_MAP_2, IDL_DISP_MAP_2_NONE,
    MENU_SETTING, COREAPP_LOGICAL_DISP_NONE, GetDisplay2Mapping, SetDisplay2Mapping},

  {IDD_DISPLAY_MAP_3, IDC_MENU_DISP_MAP_3, IDL_DISP_MAP_3_PRI,
    MENU_SETTING, COREAPP_LOGICAL_DISP_PRI, GetDisplay3Mapping, SetDisplay3Mapping},
  {IDD_DISPLAY_MAP_3, IDC_MENU_DISP_MAP_3, IDL_DISP_MAP_3_SEC,
    MENU_SETTING, COREAPP_LOGICAL_DISP_SEC, GetDisplay3Mapping, SetDisplay3Mapping},
  {IDD_DISPLAY_MAP_3, IDC_MENU_DISP_MAP_3, IDL_DISP_MAP_3_NONE,
    MENU_SETTING, COREAPP_LOGICAL_DISP_NONE, GetDisplay3Mapping, SetDisplay3Mapping},

  {IDD_DISPLAY_MAP_4, IDC_MENU_DISP_MAP_4, IDL_DISP_MAP_4_PRI,
    MENU_SETTING, COREAPP_LOGICAL_DISP_PRI, GetDisplay4Mapping, SetDisplay4Mapping},
  {IDD_DISPLAY_MAP_4, IDC_MENU_DISP_MAP_4, IDL_DISP_MAP_4_SEC,
    MENU_SETTING, COREAPP_LOGICAL_DISP_SEC, GetDisplay4Mapping, SetDisplay4Mapping},
  {IDD_DISPLAY_MAP_4, IDC_MENU_DISP_MAP_4, IDL_DISP_MAP_4_NONE,
    MENU_SETTING, COREAPP_LOGICAL_DISP_NONE, GetDisplay4Mapping, SetDisplay4Mapping},
#endif /*  defined(FEATURE_DISPLAY_CONTENT_MAP) */

};

#define TIMEDONE 0
#define DATEDONE 1

typedef struct {
   uint16 wDialogID;
   uint16 wEditDialogID;
} EditCtlIDTable;

static const EditCtlIDTable LookupEditDlgID[] = {

  { IDD_BANNER_VIEW,                 IDD_BANNER_EDIT                 },
  { IDD_SETTIME_VIEW,                IDD_SETTIME_EDIT                },
};

typedef struct {
   uint16 wDialogID;
   uint16 wDialogTitle;
   uint16 wCtrlID;
   uint16 wSoftkeyID;
} StaticDlgDefinitionType;

static const StaticDlgDefinitionType StaticDlgs[] = {
  { IDD_BANNER_VIEW, IDS_BANNER, IDC_STATIC_BANNER, IDC_SK_BANNER_VIEW },
  { IDD_SETTIME_VIEW, IDS_TIMEDATEVIEW, IDC_STATIC_SETTIME_VIEW, IDC_SK_SETTIME_VIEW },
  { IDD_RESTORE_SETTINGS, IDS_EMPTY_STRING , IDC_STATIC_RESTORESETTINGS, IDC_SK_RESTORESETTINGS },
  { IDD_MYINFO, IDS_MY_INFO, IDC_STATIC_INFO, IDC_SK_MYINFO },
  { IDD_CARD_1_ERROR,  IDS_CARD_1,IDC_STATIC_CARD1_ERROR, IDC_SK_CARD1_ERR},
  { IDD_CARD_2_ERROR,  IDS_CARD_2,IDC_STATIC_CARD2_ERROR, IDC_SK_CARD2_ERR},

   // Not static Dialog
  { IDD_SELECT_TECH_FOR_CARD_1,  IDS_SELECT_TECHNOLOGY, IDC_SELECT_TECH_CARD_1, IDC_SK_SELECT_TECH_1  } ,
  { IDD_SELECT_TECH_FOR_CARD_2,  IDS_SELECT_TECHNOLOGY, IDC_SELECT_TECH_CARD_2, IDC_SK_SELECT_TECH_2  } ,
  { IDD_SELECT_TECH_FOR_BOTH_CARDS,  IDS_SELECT_TECHNOLOGY, IDC_SELECT_TECHNOLOGY, IDC_SK_SELECT_TECHNOLOGY  }
};

/* contrast windows stuff */
#define CONTRAST_STEP     1

static void UpdateContrast(CCoreApp * pMe, int value);
static boolean    InitContrastDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog);
/* end contrast window stuff */


#ifdef FEATURE_MMGSDI
static AEECardGenericCmdStatus sFDNActionStatus;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
void DetermineOperSlotForDefault(CCoreApp *pMe, uint32 *newSlotGSM, uint32 *newSlotCDMA,
                                 boolean *gsm, boolean *cdma);
void DetermineNewGSMCDMAOperSlot(CCoreApp *pMe, uint32 *newSlotGSM, uint32 *newSlotCDMA,
                                 boolean *gsm, boolean *cdma);
int UpdateSlotPref(CCoreApp *pMe, boolean gsm, boolean cdma, uint32 newSlotGSM, uint32 newSlotCDMA);
int DetermineDualSlotState(CCoreApp *pMe, uint16 *wDlgID, boolean bAllowBypass);
int StartNVCodeVerificationProcess(CCoreApp *pMe, uint8 lock_state);
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

int StartPINVerificationProcess(CCoreApp *pMe, uint8 byPinID);
int  DetermineSingleSlotState(CCoreApp *pMe, uint16 *wDlgID, boolean bAllowBypass);
#ifdef FEATURE_MMGSDI_PERSONALIZATION
void  DeterminePersoDialogID(CCoreApp *pMe, uint16 *wDlgID);
#endif
#endif

/****************************************************************************

                          Static functions

****************************************************************************/
#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION DisableFDN_cb

DESCRIPTION

  DisableFDN_cb is called in response to ICARD_DisableFDNSlot completing, and
  will check the status the action.

  Fail:    post EVT_RESTRICT_UPDATE_FDN_FAIL event to coreapp
  Success: post EVT_RESTRICT_UPDATE_FDN_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  DisableFDN_cb(void *pNotifyData)
{
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the FDN complete successfully? */
  if (sFDNActionStatus.nCmdStatus == SUCCESS) {

    if (sFDNActionStatus.nSlotID == AEECARD_SLOT1) {
      pMe->m_wRestrict &= ~COREUTILS_FDN_SLOT1;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    else {
      pMe->m_wRestrict &= ~COREUTILS_FDN_SLOT2;
    }
#endif
    /* Disable FDN success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_RESTRICT_UPDATE_FDN_SUCCESS, (unsigned)sFDNActionStatus.nSlotID);

  } else { /* Disable FDN fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_RESTRICT_UPDATE_FDN_FAIL, (unsigned)sFDNActionStatus.nSlotID);
  }
  return;
}

/*===========================================================================
FUNCTION EnableFDN_cb

DESCRIPTION

  EnableFDN_cb is called in response to ICARD_EnableFDNSlot completing, and
  will check the status the action.

  Fail:    post EVT_RESTRICT_UPDATE_FDN_FAIL event to coreapp
  Success: post EVT_RESTRICT_UPDATE_FDN_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  EnableFDN_cb(void *pNotifyData)
{
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the PIN entered by the user match the PIN on the ID card? */
  if (sFDNActionStatus.nCmdStatus == SUCCESS) {

    if (sFDNActionStatus.nSlotID == AEECARD_SLOT1) {
      pMe->m_wRestrict |= COREUTILS_FDN_SLOT1;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    else {
      pMe->m_wRestrict |= COREUTILS_FDN_SLOT2;
    }
#endif
    /* enable FDN success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_RESTRICT_UPDATE_FDN_SUCCESS, (unsigned)sFDNActionStatus.nSlotID);

  } else { /* enable FDN fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_RESTRICT_UPDATE_FDN_FAIL, (unsigned)sFDNActionStatus.nSlotID);
  }
  return;
}
#endif /* FEATURE_MMGSDI */
/*===========================================================================
FUNCTION ReadSetting

DESCRIPTION
  Read setting from GetMethod and return the item ID of corresponding list entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 ReadSetting(CCoreApp * pMe, uint16 wDlgID)
{
  int i, j;
  uint64 val=0;

/*lint -e737*/
/*lint -e574*/
  for (i=0; i < ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (MenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        CORE_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return 0;
      }

      if (MenuActionTbl[i].getfnPtr == NULL) {
        /* Null Get fn pointer */
        CORE_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
        return 0;
      }

      val = (*MenuActionTbl[i].getfnPtr)(pMe);

      for (j=i; (j < ARR_SIZE(MenuActionTbl)) && (MenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (MenuActionTbl[j].nActionData == val) {
          return MenuActionTbl[j].wItemID;
        }
      }
      /* No match found */
      CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return 0;
    }
  }
/*lint +e737*/
/*lint +e574*/
  /* No match found */
  CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return 0;

} /* End ReadSetting */

/*===========================================================================
FUNCTION ReadAndSetCheckBoxItem

DESCRIPTION
  Read setting from GetMethod and return the item ID of corresponding list entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ReadAndSetCheckBoxItem(CCoreApp * pMe, uint16 wDlgID)
{
  int i, j;
  uint64 val=0;

/*lint -e737*/
/*lint -e574*/
  for (i=0; i < ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (MenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        CORE_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return ;
      }

      if (MenuActionTbl[i].getfnPtr == NULL) {
        /* Null Get fn pointer */
        CORE_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
        return ;
      }

      val = (*MenuActionTbl[i].getfnPtr)(pMe);

      for (j=i; (j < ARR_SIZE(MenuActionTbl)) && (MenuActionTbl[j].wDialogID == wDlgID); j++) {
        if ((MenuActionTbl[j].nActionData & val) == MenuActionTbl[j].nActionData) {
            IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

            IMenuCtl* pICurrentMenu =
                (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) MenuActionTbl[j].wControlID);

            SetCheckBoxItem(pICurrentMenu,MenuActionTbl[j].wItemID,TRUE);
        }
      }
      return ;
    }
  }
/*lint +e737*/
/*lint +e574*/
  /* No match found */
  CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return ;

} /* End ReadSetting */

/*===========================================================================
FUNCTION GetDlgMenuCtlID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetDlgMenuCtlID(CCoreApp *pMe, IDialog* pIActiveDialog)
/*lint -esym(715,pMe)*/
{
  uint16 wDlgID;
  int i;

  if  ( pIActiveDialog == NULL ) {
    return 0;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);

/*lint -e737*/
/*lint -e574*/
  for (i = 0; i< ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      return MenuActionTbl[i].wControlID;
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetDlgMenuCtlID(): No matching menu ctl", 0, 0, 0);
  return 0; /* error */
} /* End GetDlgMenuCtlID */



/*===========================================================================
FUNCTION GetNextEditDlgID

DESCRIPTION
  The function is used when a static dlg is to be followed by a textctl dialog.
  It returns the next TextCtl dialog with the given Static control dialog ID

DEPENDENCIES
  Note: should be ONLY called from static dialog only

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetNextEditDlgID(uint16 wStaticDlgID)
{

  int i;
  int nCount;

  if ( wStaticDlgID == 0 ) {
    return 0;
  }

/*lint -e737*/
/*lint -e574*/
  nCount = ARR_SIZE(LookupEditDlgID);
  for (i = 0; i< nCount; i++) {
    if (LookupEditDlgID[i].wDialogID == wStaticDlgID) {
      return LookupEditDlgID[i].wEditDialogID;
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetNextEditDlgID(): No matching edit ctl", 0, 0, 0);
  return 0; /* error */

} /* End GetNextEditDlgID */



/*===========================================================================
FUNCTION GetNextSelectedItemDlgID

DESCRIPTION
  The function is used when a menu control selection is being made.
  It returns the next dialog ID that corresponds to the item selected

DEPENDENCIES
  Note: This function should ONLY be called from a menu dialog which
    will create another dialog

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetNextSelectedItemDlgID(CCoreApp *pMe, uint16 wItemID)
/*lint -esym(715,pMe)*/
{
  int i;

/*lint -e737*/
/*lint -e574*/
  for (i = 0; i< ARR_SIZE(MenuActionTbl); i++) {
    if ((MenuActionTbl[i].wItemID == wItemID) &&
        (MenuActionTbl[i].wActionType == LAUNCH_DIALOG)) {
       return ((uint16) MenuActionTbl[i].nActionData);
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetNextSelectedItemDlgID(): No next dlgID found", 0, 0, 0);
  return 0; /* error */
} /* End GetNextSelectedItemDlgID */



/*===========================================================================
FUNCTION GetDlgSkMenuCtlID

DESCRIPTION
  The function returns the softkey menu control for a given dialog ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetDlgSkMenuCtlID(uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -e737*/
/*lint -e574*/
  for (i = 0; i < ARR_SIZE(StaticDlgs); i++) {
    if (StaticDlgs[i].wDialogID == wDlgID) {
      return StaticDlgs[i].wSoftkeyID;
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetDlgSkMenuCtlID(): No matching soft key ctl", 0, 0, 0);
  return 0; /* error */

} /* End GetDlgSkMenuCtlID */

/*===========================================================================
FUNCTION StripFeatureMenuItem

DESCRIPTION
  update the menu to reflect the correspondings under the FEATURE flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void StripFeatureMenuItem(CCoreApp * pMe, IMenuCtl* pICurrentMenu, uint16 wCtrlID)
{
  boolean bReturnStatus;
  uint32 dwSecurityStateMask;

  switch (wCtrlID)
  {
  case IDC_MENU_SOUND:
    /* Note: remove MUTE item now since only access through SK UI */
    // Require coordination between Call screens dialog and settings
    // dialog for Mute!!!
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_MUTE);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }

#ifndef FEATURE_TTY
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_TTY);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /*!FEATURE_TYY*/
    break;

  case IDC_MENU_SETTINGS:
#ifndef FEATURE_CPHS
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_CPHS);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#ifndef FEATURE_WLAN
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu WLAN item", 0,0,0);
    }
#endif

#ifndef FEATURE_NETSETTINGS_APP
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETWORK);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu WLAN item", 0,0,0);
    }
#endif
    break;

#ifdef FEATURE_CPHS
  case IDC_MENU_CPHS:
#ifndef FEATURE_ALS
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_ALS);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
    break;
#endif /*FEATURE_CPHS */

  case IDC_MENU_BACKLIGHT:
#if !defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_INTENSITY);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
    break;

  case IDC_MENU_RESTRICT:
#ifndef FEATURE_UI_RESTRICT_TO_BOOK
    bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile, IDS_RESTRICT_PHONE, NULL);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif /*!FEATURE_UI_RESTRICT_TO_BOOK*/
#ifndef FEATURE_MMGSDI
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_ADN_SLOT1);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_ADN_SLOT2);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_FDN_SLOT1);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_FDN_SLOT2);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    /* feature_MMGSDI is defined */
    /* check slot 1 present or not */
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_PRESENT_MASK) != COREAPP_SLOT1_PRESENT_MASK){
      /* no slot 1 */
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_ADN_SLOT1);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_FDN_SLOT1);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }

#ifndef FEATURE_MMGSDI_DUAL_SLOT
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_ADN_SLOT2);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_FDN_SLOT2);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    /* there is FEATURE_MMGSDI_DUAL_SLOT */
    /* check slot 2 present or not */
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_PRESENT_MASK) != COREAPP_SLOT2_PRESENT_MASK){
      /* no slot 1 */
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_ADN_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RESTRICT_FDN_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }

#endif /* !FEATURE_MMGSDI_DUAL_SLOT */
#endif /* !FEATURE_MMGSDI */
    break;

    /* Disable PIN Change option if both pin1 and pin2 have been disabled*/
    case IDC_MENU_SECURITY:
#ifndef FEATURE_MMGSDI
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif
#ifndef FEATURE_MMGSDI_DUAL_SLOT
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_USER_SLOT_PREF);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif
#ifndef FEATURE_MMGSDI_PERSONALIZATION
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PERSO_FEATURE_IND);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PERSO_FEATURE_DATA);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
#ifndef FEATURE_UIM_RUN_TIME_ENABLE
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_RTRE_CONFIG);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
      break;

    case IDC_MENU_PINCHANGE:
      dwSecurityStateMask = DetermineSecurityMask(pMe);

      if ((dwSecurityStateMask == COREAPP_EMERGENCY_MASK) || (dwSecurityStateMask == COREAPP_NOT_READY_MASK)) {
        CORE_ERR("Emergency/Not Ready Security menu", 0, 0, 0);
        return;
      }

      dwSecurityStateMask = ~dwSecurityStateMask; /* Get the bits that need to be removed from the menu */

      if ((dwSecurityStateMask & COREAPP_PIN1_CODE_MASK) == COREAPP_PIN1_CODE_MASK) {
        /* remove pin1 in slot 1 change option when pin1 is disabled */
        if ((pMe->m_sPIN1_lock_state == AEECARD_PIN_DISABLED) ||
            (pMe->m_sPIN1_lock_state == AEECARD_PIN_UNKNOWN) ||
            (pMe->m_sPIN1_lock_state == AEECARD_PIN_NOT_VALID)) {
          bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_1);
          if (bReturnStatus == FALSE) {
            CORE_ERR ("Failed to delete menu item", 0,0,0);
          }
        }
        /* remove pin2 change option when pin2 is disabled */
        if ((pMe->m_sPIN2_lock_state == AEECARD_PIN_DISABLED) ||
            (pMe->m_sPIN2_lock_state == AEECARD_PIN_UNKNOWN) ||
            (pMe->m_sPIN2_lock_state == AEECARD_PIN_NOT_VALID)) {
          bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_2);
          if (bReturnStatus == FALSE) {
            CORE_ERR ("Failed to delete menu item", 0,0,0);
          }
        }
      }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if ((dwSecurityStateMask & COREAPP_PIN1_SLOT2_CODE_MASK) == COREAPP_PIN1_SLOT2_CODE_MASK) {
        /* remove pin1 in slot 2 change option when pin1 is disabled */
        if ((pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_DISABLED) ||
            (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_UNKNOWN) ||
            (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_NOT_VALID)) {
          bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_1_SLOT2);
          if (bReturnStatus == FALSE) {
            CORE_ERR ("Failed to delete menu item", 0,0,0);
          }
        }
        /* remove pin2 change option when pin2 is disabled */
        if ((pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_DISABLED) ||
            (pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_UNKNOWN) ||
            (pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_NOT_VALID)) {
          bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_2_SLOT2);
          if (bReturnStatus == FALSE) {
            CORE_ERR ("Failed to delete menu item", 0,0,0);
          }
        }
      }
#else
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_1_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINCHANGE_2_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }

#endif
      break;

  case IDC_MENU_PINLOCK:
      if ((pMe->m_sPIN1_lock_state == AEECARD_PIN_UNKNOWN) ||
          (pMe->m_sPIN1_lock_state == AEECARD_PIN_NOT_VALID))
      {
        /* remove PIN LOCK Code */
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_1);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }
#ifdef FEATURE_WCDMA
      if ((pMe->m_sPIN2_lock_state == AEECARD_PIN_UNKNOWN) ||
          (pMe->m_sPIN2_lock_state == AEECARD_PIN_NOT_VALID))
#endif
      {
        /* remove PIN 2 LOCK Code */
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_2);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if ((pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_UNKNOWN) ||
          (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_NOT_VALID))
      {
        /* remove PIN LOCK Code for slot 2*/
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_1_SLOT2);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }
#ifdef FEATURE_WCDMA
      if ((pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_UNKNOWN) ||
          (pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_NOT_VALID))
#endif
      {
        /* remove PIN 2 LOCK Code for slot 2*/
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_2_SLOT2);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }
#else /*FEATURE_MMGSDI_DUAL_SLOT */
      /* remove PIN LOCK Code for slot 2*/
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_1_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PINLOCK_2_SLOT2);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif
      break;

  case IDC_MENU_DISPLAY:
    {
#ifndef FEATURE_MDP
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_DISP_DISABLE_BG_IMAGE);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif // ! FEATURE_MDP

#if defined (FEATURE_USRSYSCLOCK)
      {
        //Have RTC, don't allow menu in CDMA mode
        AEECMSSInfo ss_info;
        int retval;

        retval = ICM_GetSSInfo(pMe->m_pICM, &ss_info, sizeof(ss_info));
        if (AEE_SUCCESS != retval) {
          CORE_ERR("Couldnt get serving system info %d", retval, 0, 0);
          return;
        }

        if (AEECM_IS_MODE_CDMA(ss_info.sys_mode)) {
          bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SETSECURETIME);
          if (bReturnStatus == FALSE) {
            CORE_ERR ("Failed to delete menu item", 0,0,0);
          }
        }
      }
#else
      //No RTC, no sense in allowing set time menu
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SETTIME);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      //No RTC, no sense in allowing set time menu
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SETSECURETIME);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif

#if ! defined(FEATURE_GSM) && ! defined(FEATURE_WCDMA)
      /* Language selection menu is for GCF compliance, only applicable to
       * to GSM/WCDMA.  (The functionality could be kept for CDMA, but we only
       * require for GSM/WCDMA.)
       */
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_LANGUAGE_SELECT);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }

#endif /* ! defined(FEATURE_GSM) && ! defined(FEATURE_WCDMA) */

#if !defined(FEATURE_DISPLAY_CONTENT_MAP)
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_DISPLAY_MAP);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif /* !defined(FEATURE_DISPLAY_CONTENT_MAP) */

    }
    break;
  case IDC_MENU_DTMFLENGTH:
#if !defined(FEATURE_CDMA1X)
    //burst is only defined for 1x targets
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_DTMFLENGTH_BURST);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
    break;

  case IDC_MENU_EXTRAS:
    {
      AEECMSSInfo ss_info;
      int retval;

      retval = ICM_GetSSInfo(pMe->m_pICM, &ss_info, sizeof(ss_info));
      if (AEE_SUCCESS != retval) {
        CORE_ERR("Couldnt get serving system info %d", retval, 0, 0);
        return;
      }

      if (!AEECM_IS_MODE_CDMA(ss_info.sys_mode)) {
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_PRIVACY);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu Privacy item", 0,0,0);
        }
      }

#ifdef FEATURE_USRSYSCLOCK
      if ( pMe->m_pIUsrSysClock == NULL ){
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETWORK_TIME);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_DELETE_NITZ_PLMN);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }
#else
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_NETWORK_TIME);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_DELETE_NITZ_PLMN);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
#endif /* FEATURE_USRSYSCLOCK*/

#if !defined(FEATURE_RUNTIME_DEVMAP) || !defined(FEATURE_USB_DEFAULT_MTP)
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_USBDEVICESWITCH);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu USB Device Switch item", 0,0,0);
      }
#endif

    }
    break;

  case IDC_MENU_SOUND_DEVICE:
#ifndef FEATURE_TTY
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_HEADSET);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_HCO);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_VCO);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    if (!uisnd_is_device_attached(SND_DEVICE_TTY_HEADSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_HEADSET);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
    if (!uisnd_is_device_attached(SND_DEVICE_TTY_HCO))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_HCO);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
    if (!uisnd_is_device_attached(SND_DEVICE_TTY_VCO))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_TTY_VCO);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
#endif


    if (!uisnd_is_device_attached(SND_DEVICE_STEREO_HEADSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_STEREO_HEADSET);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
    if (!uisnd_is_device_attached(SND_DEVICE_HEADSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_HEADSET);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
    if (!uisnd_is_device_attached(SND_DEVICE_HFK))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_HFK);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }

#ifndef FEATURE_USB_CARKIT
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_USB_HFK);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    if (!uisnd_is_device_attached(SND_DEVICE_USB))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_USB_HFK);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
#endif

#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_STEREO_DAC);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    if (!uisnd_is_device_attached(SND_DEVICE_SDAC))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_STEREO_DAC);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
#endif

#ifndef FEATURE_APP_BLUETOOTH
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP_SCO);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_HEADSET);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
  #ifndef FEATURE_BT_EXTPF_AV
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP_SCO);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
  #else
    if (!uisnd_is_device_attached(SND_DEVICE_BT_A2DP_HEADSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
    #ifndef FEATURE_BT_EXTPF_AV_A2DP_SCO
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP_SCO);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    #else
      if (!uisnd_is_device_attached(SND_DEVICE_BT_A2DP_SCO_HEADSET))
      {
        bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_A2DP_SCO);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Failed to delete menu item", 0,0,0);
        }
      }
    #endif /* FEATURE_BT_EXTPF_AV_SCO */
  #endif /* FEATURE_BT_EXTPF_AV */
  #ifndef FEATURE_BT_AG
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_HEADSET);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
  #else
    if (!uisnd_is_device_attached(SND_DEVICE_BT_HEADSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_BT_HEADSET);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
  #endif  /* FEATURE_BT_AG */
#endif /* FEATURE_APP_BLUETOOTH */

#ifndef FEATURE_SPEAKER_PHONE
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_SPEAKERPHONE);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#else
    if (!uisnd_is_device_attached(SND_DEVICE_SPEAKER_PHONE))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_SPEAKERPHONE);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }
#endif

    if (!uisnd_is_device_attached(SND_DEVICE_HANDSET))
    {
      bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_SOUNDDEV_HANDSET);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Failed to delete menu item", 0,0,0);
      }
    }

    break;

  case IDC_ICONV_CORE:
#if !defined(FEATURE_IMS)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_IMS_SETTINGS);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_IMS_VIDEOSHARE);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#elif !defined(FEATURE_IMS_VIDEOSHARE)

    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_IMS_VIDEOSHARE);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#if !defined(FEATURE_ALARMCLOCK_APP)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_ALARMCLOCK);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif

#if !defined(FEATURE_FMRADIO_APP)
    bReturnStatus = IMENUCTL_DeleteItem(pICurrentMenu, IDL_FMRADIO);
    if (bReturnStatus == FALSE) {
      CORE_ERR ("Failed to delete menu item", 0,0,0);
    }
#endif
    break;

  default:
    break;
  }
} /* End StripFeatureMenuItem */


/*===========================================================================
FUNCTION CreateMenuSelectedItemDlg

DESCRIPTION
  It obtains the current menu selection and create the next level dialog
  accordingly

DEPENDENCIES
  GetDlgMenuCtlID, GetNextSeletedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
static boolean CreateMenuSelectedItemDlg(CCoreApp *pMe)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  uint16 wDlgID;
  uint16 wItemID;
  IDialog* pIActiveDialog;
  int nReturnStatus;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL ) {
    CORE_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the menu control */
  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    CORE_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);

  if(pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Get the selected item */
  wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

#ifdef FEATURE_AUXSETTINGS_APP 
#ifdef FEATURE_WLAN
  if (wItemID == IDL_WLAN)
  {
    if (ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_AUXSETTINGS, AUX_StartWLAN) != SUCCESS)
      CORE_ERR("Could not start AEECLSID_AUXSETTINGS applet", 0, 0, 0);
    return TRUE;
  }
#endif
#endif

  /* Get next dialog ID */
  wDlgID = GetNextSelectedItemDlgID(pMe, wItemID);

  if(wDlgID == 0) { /* error */
    CORE_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  /* Create next dialog */
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
              wDlgID, NULL);

  return CheckAEEReturnStatus(nReturnStatus);

} /* End CreateMenuSelectedItemDlg */



/*===========================================================================
FUNCTION GetRingerFileName

DESCRIPTION
 This function returns the file name portion of the path.

DEPENDENCIES
 None

SIDE EFFECTS
  None
===========================================================================*/
static char * GetRingerFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;

   return pszName;
}

/*===========================================================================
FUNCTION GetRingerValue

DESCRIPTION
   This function gets the level of multimedia ringer on ringer menu

DEPENDENCIES
 None

SIDE EFFECTS
  None
===========================================================================*/

static int GetRingerValue(int item)
{

  switch(item)
  {
  case IDL_RINGTYPE_5:
    return 0;

  case IDL_RINGTYPE_6:
    return 1;

  case IDL_RINGTYPE_7:
    return 2;

  case IDL_RINGTYPE_8:
    return 3;

  case IDL_RINGTYPE_9:
    return 4;

  case IDL_RINGTYPE_10:
    return 5;

  case IDL_RINGTYPE_11:
    return 6;

  case IDL_RINGTYPE_12:
    return 7;

  case IDL_RINGTYPE_13:
    return 8;

  case IDL_RINGTYPE_14:
    return 9;

  case IDL_RINGTYPE_15:
    return 10;

  case IDL_RINGTYPE_16:
    return 11;

  case IDL_RINGTYPE_17:
    return 12;

  case IDL_RINGTYPE_18:
    return 13;

  case IDL_RINGTYPE_19:
    return 14;

  default:
    return 0;
  }

}
/*===========================================================================
FUNCTION GetResourceValue

DESCRIPTION
   This function gets the RESOURCE ID of MM ringer

DEPENDENCIES
 None

SIDE EFFECTS
  None
===========================================================================*/

static int GetResourceValue(int item)
{

  switch(item) {
  case 0:
    return IDL_RINGTYPE_5;

  case 1:
    return IDL_RINGTYPE_6;

  case 2:
    return IDL_RINGTYPE_7;

  case 3:
    return IDL_RINGTYPE_8;

  case 4:
    return IDL_RINGTYPE_9;

  case 5:
    return IDL_RINGTYPE_10;

  case 6:
    return IDL_RINGTYPE_11;

  case 7:
    return IDL_RINGTYPE_12;

  case 8:
    return IDL_RINGTYPE_13;

  case 9:
    return IDL_RINGTYPE_14;

  case 10:
    return IDL_RINGTYPE_15;

  case 11:
    return IDL_RINGTYPE_16;

  case 12:
    return IDL_RINGTYPE_17;

  case 13:
    return IDL_RINGTYPE_18;

  case 14:
    return IDL_RINGTYPE_19;

  default:
    return 0;
  }

}
/*===========================================================================
FUNCTION AddMultiMediaRingers

DESCRIPTION
     This function adds multimedia ringer to the specified menu.
DEPENDENCIES
 None

SIDE EFFECTS
  None
===========================================================================*/
static int AddMultiMediaRingers(IMenuCtl* pICurrentMenu, CCoreApp *pMe)
{
  IFileMgr *  pIFileMgr;
  uint16      wItemID = 0;
  AECHAR      *szBuf;
  uint16 itemtoadd=0;
  FileInfo fi;
  uint16 j=0;
  szBuf = MALLOC(MAX_FILE_NAME * sizeof(AECHAR));

  if(!szBuf) {
    CORE_ERR ("Null pointer ", 0,0,0);
    return FALSE;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    FREE(szBuf);
    return FALSE;
  }

  if( IFILEMGR_EnumInit(pIFileMgr, AEE_RINGER_DIR, FALSE) != SUCCESS)
  {
    FREE(szBuf);
    IFILEMGR_Release(pIFileMgr);
    return FALSE;
  }

  while ((wItemID < MM_RINGER_MAX_FILES)  && IFILEMGR_EnumNext(pIFileMgr, &fi))
  {
    char *   szName;

    pMe->m_pszFileArray[wItemID] = STRDUP(fi.szName);
    if (!pMe->m_pszFileArray[wItemID])
    {
      pMe->m_nNumFiles = wItemID;
      FREE(szBuf);
      IFILEMGR_Release(pIFileMgr);
    }

    // Add only the file name
    szName = GetRingerFileName(fi.szName);
    if (szName)
    {
      itemtoadd = (uint16) GetResourceValue(wItemID);
      if(itemtoadd == (uint16) 0)
      {
        FREE(szBuf);
        IFILEMGR_Release(pIFileMgr);
        return FALSE;
      }
      (void) STRTOWSTR(szName, szBuf, MAX_FILE_NAME);
        IMENUCTL_SetItemText(pICurrentMenu, (uint16)itemtoadd, gpszResourceFile,0, szBuf);
      }

    wItemID++;
  }

  for(j = wItemID; j <MM_RINGER_MAX_FILES; j++)
  {
    itemtoadd = (uint16) GetResourceValue(j);
    IMENUCTL_DeleteItem(pICurrentMenu, (uint16)itemtoadd);
  }
  FREE(szBuf);
  IFILEMGR_Release(pIFileMgr);
  return IMENUCTL_Redraw(pICurrentMenu) ;
}

/*===========================================================================
FUNCTION LookupResIDForSysPrefErrCode

DESCRIPTION
  Map between ICM PH error code with the Resource String ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 LookupSystemPrefMsgString (uint16 wDlgID)
{
  switch (wDlgID)
  {
  case IDD_RTRE_CONFIG:
    /* Invalid RTRE configuration parameter. CDMA Only */
    return IDS_RTRE_CONFIG_ERROR;

  case IDD_ALS_LINE_SELECTION:
    return IDS_ALS_LINE_SELECTION_ERROR;

  case IDD_ALS_LINE_SWITCHING:
    return IDS_ALS_LINE_SWITCHING_ERROR;

  default:
    return IDS_CHANGE_FAILED;
  }
}
/*===========================================================================
FUNCTION InitRadioButtonDlg

DESCRIPTION
  This function update the radio button menu item list based on featurization,
  update the radio button for the selected item based on NV value

DEPENDENCIES
  GetDlgMenuCtlID, StripFeatureMenuItem,
  SetMenuIcon, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitRadioButtonDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;

  if ( pIActiveDialog == NULL ) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the control ID */
  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Remove or update items from the menu based on featurization requirement */
  StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

  /* Get the current setting from static variable or NV */
  wItemID = ReadSetting(pMe, wDlgID);

  if(wItemID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    DisplayMessageDlg(pMe, IDS_READ_FAILED, 3);
    return FALSE;
  }

  /* Initialize radio button images with IDB_RADIO_UNFILLED */
  InitMenuIcons (pICurrentMenu);

  /* Update the radio button for the currently selected option */
  SetMenuIcon(pICurrentMenu, wItemID, TRUE);

  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End InitRadioButtonDlg */


/*===========================================================================
FUNCTION InitCheckBoxDlg

DESCRIPTION
  This function update the check box menu items list based on featurization,
  update the Check box item for the selected item based on NV value

DEPENDENCIES
  GetDlgMenuCtlID, StripFeatureMenuItem,
  SetMenuIcon, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitCheckBoxDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  IMenuCtl* pISkMenu= NULL;
  uint16 wCtrlID, wSkCtrlID;

  if ( pIActiveDialog == NULL ) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the control ID */
  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wSkCtrlID = GetDlgSkMenuCtlID(wDlgID);
  if(wSkCtrlID != 0)
  {
    pISkMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog, (int16) wSkCtrlID);
    if(pISkMenu != NULL)
    {
      SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISkMenu);
    }
  }

  /* Remove or update items from the menu based on featurization requirement */
  StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

 /* Initialize radio button images with IDB_CHECK_OFF */
  InitCheckBox (pICurrentMenu);

  /* Get the current setting from static variable or NV */
   ReadAndSetCheckBoxItem(pMe, wDlgID);


  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End InitCheckBoxDlg */


#ifdef FEATURE_MMGSDI_DUAL_SLOT

/*===========================================================================
FUNCTION InitSelectTechDlg

DESCRIPTION
  This function update the check box menu items list based on featurization,
  update the Check box item for the selected item based on NV value

DEPENDENCIES
  GetDlgMenuCtlID, StripFeatureMenuItem,
  SetMenuIcon, SetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitSelectTechDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  IMenuCtl* pISkMenu= NULL;
  uint16 wCtrlID, wSkCtrlID;
  int count,i;
  int nReturnStatus;


  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  /* Get the control ID */
  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, wCtrlID);
  if (pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wSkCtrlID = GetDlgSkMenuCtlID(wDlgID);
  if(wSkCtrlID != 0)
  {
    pISkMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog, wSkCtrlID);
    if(pISkMenu != NULL)
    {
      if(wDlgID == IDD_SELECT_TECH_FOR_CARD_1 && !IsCardValid(pMe,AEECARD_PIN1_SLOT2))
      {
          IMENUCTL_DeleteItem(pISkMenu, IDL_SK_SELECT_TECH_CARD_2);
      }
      else if(wDlgID == IDD_SELECT_TECH_FOR_CARD_2 && !IsCardValid(pMe, AEECARD_PIN1))
      {
          IMENUCTL_DeleteItem(pISkMenu, IDL_SK_SELECT_TECH_CARD_1);
          IMENUCTL_DeleteItem(pISkMenu, IDL_SK_SELECT_TECH_DEF_2);
      }
      SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISkMenu);
    }
  }

  /* Remove or update items from the menu based on featurization requirement */
  StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

 /* Initialize radio button images with IDB_CHECK_OFF */
  //InitCheckBox (pICurrentMenu);

  count = IMENUCTL_GetItemCount(pICurrentMenu);

  nReturnStatus = PopulateCardApplications(pMe);

  if (nReturnStatus == SUCCESS){

    boolean setItem = FALSE;
    boolean delItem = FALSE;

    /* Get user preferred slot */
    pMe->m_byGSMUserSlotPref = GetUserSlotPrefGSM(pMe);
    pMe->m_byCDMAUserSlotPref = GetUserSlotPrefCDMA(pMe);

    for(i=0;i<count;i++)
    {
      uint32 wItemID = IMENUCTL_GetItemID(pICurrentMenu, i);
      setItem = FALSE;
      delItem = FALSE;

      switch(wItemID)
      {
        case IDL_SELECT_GSM_1:
        case IDL_GSM_1:
          if(pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_GSM_MASK )
          {
            if(pMe->m_byGSMUserSlotPref == AEECARD_SLOT1)
            {
              setItem = TRUE;
            }
          }
          else
          {
            delItem = TRUE;
          }
          break;
        case IDL_SELECT_GSM_2:
        case IDL_GSM_2:
          if(pMe->m_dwSecurityMask & COREAPP_SLOT2_APP_GSM_MASK )
          {
            if(pMe->m_byGSMUserSlotPref == AEECARD_SLOT2)
            {
              setItem = TRUE;
            }
          }
          else
          {
            delItem = TRUE;
          }
          break;

        case IDL_SELECT_CDMA_1:
        case IDL_CDMA_1:
          if(pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_RUIM_MASK)
          {
            if(pMe->m_byCDMAUserSlotPref == AEECARD_SLOT1)
            {
              setItem = TRUE;
            }
          }
          else
          {
            delItem = TRUE;
          }
          break;

        case IDL_SELECT_CDMA_2:
        case IDL_CDMA_2:
          if(pMe->m_dwSecurityMask & COREAPP_SLOT2_APP_RUIM_MASK )
          {
            if(pMe->m_byCDMAUserSlotPref == AEECARD_SLOT2)
            {
              setItem = TRUE;
            }
          }
          else
          {
            delItem = TRUE;
          }
          break;

      }
      if(delItem)
      {
        IMENUCTL_DeleteItem(pICurrentMenu, wItemID);
        count--;
        i--;
      }
      else
      {
        SetCheckBoxItem(pICurrentMenu, wItemID,setItem);
      }
    }
  }

  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End InitSelectTechDlg */
#endif /*FEATURE_MMGSDI_DUAL_SLOT */


/*===========================================================================
FUNCTION UpdateSetting

DESCRIPTION
  Update menu setting static variable and NV values if the selected Item is
  NV related.
  Based on whether the change of setting took place, update the display of
  radio buttons.

DEPENDENCIES
  ReadSetting, InitMenuIcons, SetMenuIcon

SIDE EFFECTS
  None
===========================================================================*/
static boolean UpdateSetting(CCoreApp *pMe,
                          uint16 wSelectedItemID,
                          IMenuCtl* pICurrentMenu)
{
  IDialog* pIActiveDialog;

  uint16 wDlgID;
  uint16 wCtrlID;


  if ((wSelectedItemID == 0) || (pMe == NULL))
  {
    /* error checking */
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wDlgID == 0) { /*error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if(pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }


  if(WriteSetting(pMe, wDlgID, wSelectedItemID)) {

    /* Initialize radio button images with IDB_RADIO_UNFILLED */
    InitMenuIcons (pICurrentMenu);

    /* Update radio button image to reflect new setting */
    SetMenuIcon (pICurrentMenu, wSelectedItemID, TRUE);

    return TRUE;
  }
  return FALSE;
} /* End UpdateSetting */



/*===========================================================================
FUNCTION LookupSoundDlgNVEnum

DESCRIPTION
  Return NV enum type based on dialog ID for Ringer Type, Ringer Volume,
  Key beep volume, earpiece volume and message volume.  NV_MAX_I is
  returned if dialog ID other than the above are passed into this function

DEPENDENCIES
  GetKeyBeepVolumeNVEnum, GetRingerVolumeNVEnum, GetEarpieceVolumeNVEnum,
  GetMessagEVolumeNVEnum

SIDE EFFECTS
  None
===========================================================================*/
static nv_items_enum_type LookupSoundDlgNVEnum(CCoreApp * pMe, uint16 wDlgID)
{
  switch(wDlgID) {

  case IDD_RINGTYPE:
    return NV_RINGER_TYPE_I;

  case IDD_KEYBEEPVOLUME:
    return GetKeyBeepVolumeNVEnum();

  case IDD_RINGERVOLUME:
    return GetRingerVolumeNVEnum();

  case IDD_EARPIECEVOLUME:
    return GetEarpieceVolumeNVEnum();

  case IDD_MESSAGEVOLUME:
    return GetMessageVolumeNVEnum();

  default: /* error */
    CORE_ERR ("LookupSoundDlgNVEnum(): Cannot find NV Enum for DlgID", 0, 0, 0 );
    return NV_MAX_I;
  }
} /* End LookupSoundDlgNVEnum */



/*===========================================================================
FUNCTION LookupValue

DESCRIPTION
  return ringer type NV value based on item list ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int LookupValue(uint16 wDlgID, uint16 wItemID)
{
  int i, j;

/*lint -e737*/
/*lint -e574*/
  for (i=0; i < ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (MenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        CORE_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return 0;
      }

      for (j=i; (j < ARR_SIZE(MenuActionTbl)) && (MenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (MenuActionTbl[j].wItemID == wItemID) {
          return (int) (MenuActionTbl[j].nActionData);
        }
      }
    }
  }
/*lint +e737*/
/*lint +e574*/
  /* No match found */
  CORE_ERR ("No match for  dialog %d item %d", wDlgID, wItemID, 0);
  return 0;
} /* End LookupValue */


/*===========================================================================
FUNCTION UpdateSoundRadioButtonMenuTitle

DESCRIPTION
  Since the volume radio button menus share dialogs, this function is called
  to update the title accordingly

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void UpdateSoundRadioButtonMenuTitle(IMenuCtl* pICurrentMenu, uint16 wDlgID, nv_items_enum_type item)
{
  boolean bReturnStatus;

  switch (wDlgID) {

  case IDD_KEYBEEPVOLUME:
    switch (item) {
    case NV_BEEP_SPKR_LVL_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_CARKEYVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
    case NV_BEEP_LVL_SHADOW_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_PRIVACYKEYVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    case NV_BEEP_SDAC_LVL_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_BEEPSDACVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
#endif
    default:
      break;
    }
    break;

  case IDD_RINGERVOLUME:
    switch (item) {
      case NV_MM_LVL_I:
        bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                                IDS_MM_EAR_VOL, NULL);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Could not set title", 0, 0, 0);
        }
        break;
      case NV_MM_LVL_SHADOW_I:
        bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                                IDS_MM_HS_VOL, NULL);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Could not set title", 0, 0, 0);
        }
        break;
      case NV_MM_SPEAKER_LVL_I:
        bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                                IDS_MM_SPKR_VOL, NULL);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Could not set title", 0, 0, 0);
        }
        break;
      case NV_RINGER_SPKR_LVL_I:
        bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                                IDS_CARRINGERVOLUME, NULL);
        if (bReturnStatus == FALSE) {
          CORE_ERR ("Could not set title", 0, 0, 0);
        }
        break;

      default:
        break;
    }
    break;

  case IDD_EARPIECEVOLUME:
    switch (item) {
    case NV_SPEAKER_LVL_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_CARSPKRVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
    case NV_EAR_LVL_SHADOW_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_HEADSETVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    case NV_SDAC_LVL_I:
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                              IDS_SDACVOLUME, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
      break;
#endif
    default:
      break;
    }
    break;

  case IDD_MESSAGEVOLUME:
    if (item == NV_ALERTS_LVL_SHADOW_I) {
      bReturnStatus = IMENUCTL_SetTitle(pICurrentMenu, gpszResourceFile,
                               IDS_CARMESSAGEVOL, NULL);
      if (bReturnStatus == FALSE) {
        CORE_ERR ("Could not set title", 0, 0, 0);
      }
    }
    break;

  default:
    break;
  }
} /* End UpdateSoundRadioButtonMenuTitle */



/*===========================================================================
FUNCTION InitSoundRadioButtonDlg

DESCRIPTION
  initialize sound radio button, update the menu title, list item and radio
  button image

DEPENDENCIES
  GetDlgMenuCtlID, StripFeatureMenuItem, LookupSounDlgNVEnum,
  UpdateSoundRadioButtonMenuTitle, ReadSetting,
  SetMenuIcon, SetDefaultMenuLook(pICurrentMenu)

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitSoundRadioButtonDlg(CCoreApp *pMe, uint16 wDlgID, IDialog * pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;
  nv_items_enum_type item;

  if ( pIActiveDialog == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if ( pICurrentMenu == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }


  StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);

  item = LookupSoundDlgNVEnum(pMe, wDlgID);
  if( item == NV_MAX_I ) { /* error */
    CORE_ERR ("Invalid NV item", 0,0,0);
    return FALSE;
  }

  /* Title update */
  UpdateSoundRadioButtonMenuTitle(pICurrentMenu, wDlgID, item);

  /* translate the newly obtained value to Resource ID */
  wItemID = ReadSetting(pMe, wDlgID);
  if (wItemID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* update radio button image */
  SetMenuIcon(pICurrentMenu, wItemID, TRUE);
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End InitSoundRadioButtonDlg */



/*===========================================================================
FUNCTION InitSoundRingerButtonDlg

DESCRIPTION
  initialize ringer radio button, update the menu title, list item and radio
  button image

DEPENDENCIES
  GetDlgMenuCtlID, StripFeatureMenuItem, LookupSounDlgNVEnum,
  UpdateSoundRadioButtonMenuTitle, ReadSetting,
  SetMenuIcon, SetDefaultMenuLook(pICurrentMenu)

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitSoundRingerButtonDlg(CCoreApp *pMe, uint16 wDlgID, IDialog * pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wItemID;
  nv_items_enum_type item;
  uint16 mm_ringer_table[] =
  {
    IDL_RINGTYPE_5,
    IDL_RINGTYPE_6,
    IDL_RINGTYPE_7,
    IDL_RINGTYPE_8,
    IDL_RINGTYPE_9,
    IDL_RINGTYPE_10,
    IDL_RINGTYPE_11,
    IDL_RINGTYPE_12,
    IDL_RINGTYPE_13,
    IDL_RINGTYPE_14,
    IDL_RINGTYPE_15,
    IDL_RINGTYPE_16,
    IDL_RINGTYPE_17,
    IDL_RINGTYPE_18,
    IDL_RINGTYPE_19
  };

  if (pIActiveDialog == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
  if(wCtrlID == 0) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if(pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
  if(AddMultiMediaRingers(pICurrentMenu, pMe) == FALSE)
  {
    return FALSE;
  }
  item = LookupSoundDlgNVEnum(pMe, wDlgID);
  if( item == NV_MAX_I ) { /* error */
    return FALSE;
  }

  /* Title update */
  UpdateSoundRadioButtonMenuTitle(pICurrentMenu, wDlgID, item);

  /* translate the newly obtained value to Resource ID */
  wItemID = ReadSetting(pMe, wDlgID);
  if (wItemID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (pMe->m_bMMRinger)
  {
    int i=0;
    /* Get the current MM ringer */
    while (i < MM_RINGER_MAX_FILES)
    {
      if(pMe->m_pszFileArray[i])
      {
        if (STRCMP(pMe->m_pszMMRingerFile, pMe->m_pszFileArray[i]) == 0)
        {
          /* got a match */
          SetMenuIcon (pICurrentMenu, mm_ringer_table[i], TRUE);
          SetDefaultMenuLook (pMe->a.m_pIShell, pICurrentMenu);
          return TRUE;
        }
      }
      i++;
    }
    /* no match - maybe the file is missing/deleted */
    /* set the ringer back to standard */

    pMe->m_bMMRinger = FALSE;

    (void) SetRingerType (pMe, (uint64)UISND_RING_NORMAL);
    /* wItemID should still be valid */

  }
  SetMenuIcon (pICurrentMenu, wItemID, TRUE);
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;
} /* End InitSoundRingerButtonDlg */

/*===========================================================================
FUNCTION PlaySampleSound

DESCRIPTION
  based on the currently highlighted menu item, a sample tone is played.

DEPENDENCIES
  LookupValue, GetKeyBeepVolumeNVEnum,
  uisnd_sample_snd, uisnd_sample_tone

SIDE EFFECTS
  None
===========================================================================*/
static void PlaySampleSound(CCoreApp * pMe, uint16 wDlgID, uint16 w)
{
  int nSample;

  switch (wDlgID) {

  case IDD_RINGTYPE:
    nSample = LookupValue(wDlgID, w);
    if (nSample != 0) { /* error checking */
      uisnd_sample_snd(UI_ALERT_SND, uisnd_ringer_snd_map[nSample],
                      (byte) pMe->m_byRingerVolume);
    }
    break;

  case IDD_KEYBEEPVOLUME:

    nSample = LookupValue(wDlgID, w);

    if (nSample != 0) { /* error checking */
      uisnd_sample_tone( UI_BEEP_SND, SND_2ND, 300, (byte) nSample);
    }
    break;

  case IDD_RINGERVOLUME:
    nSample = LookupValue(wDlgID, w);
    if (nSample != 0) { /* error checking */
      uisnd_sample_snd( UI_ALERT_SND, uisnd_ringer_snd_map[pMe->m_RingerType],
                          (byte) nSample);
    }
    break;

  case IDD_EARPIECEVOLUME:
    nSample = LookupValue(wDlgID, w);
    if ( pMe->m_byKeyBeepVolume && (nSample != 0)) {
       /* no sound if earpiece volume is being adjusted in a call */
       if ( !ui_is_in_call()) {
           /* make the sound using that volume */
           uisnd_sample_tone( UI_BEEP_SND, SND_2ND, 300 , pMe->m_byKeyBeepVolume);
       }
    }
    break;


  case IDD_MESSAGEVOLUME:
    nSample = LookupValue(wDlgID, w);
    if ( nSample != 0 ) {  /* error checking */
      uisnd_sample_tone(UI_MSG_SND, SND_MSG_WAITING, 200, (byte) nSample);
    }
    break;

  default:
    break;
  }
} /* End PlaySampleSound */

/*===========================================================================
FUNCTION FormatSystemTime

DESCRIPTION
  Get system time

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
static int FormatSystemTime(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  JulianType systime;
  int nBufLen = 0;

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_SYSTIME,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  /* use the mobility management info to display time, else
     use the current settings
  */
  if( pMe->m_ssInfo.mm_information.time_and_time_zone_avail )
  {
    uint32 secs;
    secs = JULIANTOSECONDS(&pMe->m_ssInfo.mm_information.time_and_time_zone.time);

    if(pMe->m_ssInfo.mm_information.time_and_time_zone.time_zone)
    {
      uint32 hrs,mins;
      boolean pZone = pMe->m_ssInfo.mm_information.time_and_time_zone.time_zone > 0?TRUE:FALSE;
      uint32  timeZone  = pZone ?pMe->m_ssInfo.mm_information.time_and_time_zone.time_zone:
                                 (pMe->m_ssInfo.mm_information.time_and_time_zone.time_zone *-1);

      hrs  = timeZone/4;
      mins = (timeZone%4)*15; /*Quarter*/
      
      if(pZone)
      {
        secs += ((hrs*60*60) + mins*60);
      }
      else
        secs -= ((hrs*60*60) + mins*60);
    }
    
    GETJULIANDATE(secs, &systime);
    
    //other lint errors through here re: sign usage/rollover
    (void)FormatTimeDate(systime,&pszContent[nBufLen], (nLen - nBufLen));

  } else {
    if( GetTimeofDay(pMe, &systime))
    {
      (void) FormatTimeDate(systime, &pszContent[nBufLen], (nLen - nBufLen));
    }

    else
    {
        ISHELL_LoadResString (pMe->a.m_pIShell,
                        gpszResourceFile,
                        IDS_INVALID,
                        &pszContent[nBufLen],
                        (nLen - nBufLen) * (int) sizeof(AECHAR));
    }
  }
  nBufLen = WSTRLEN (pszContent);
  pszContent[nBufLen++] = '\n';
  return nBufLen;

} /* End of FormatSystemTime */

/*===========================================================================

FUNCTION DirNumPCSToText


DESCRIPTION
  Translate Mobile directory number digit format to ASCII text format.
  Mobile directory number ranges from 1 to 10 decimal, which is translated
  to '1' .. '9', '0' ASCII text.

  dest is a buffer pointer of at least 15 byte space.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void DirNumPCSToText(char *dest, nv_mob_dir_data_type *src)
{
  int i;
  int len;

  if ((src == NULL) || (dest == NULL))
  {
    CORE_ERR("NULL ptr %x %x", dest, src, 0);
    return;
  }

  len = src->n_digits;

  if(len > 15)
  {
    MSG_ERROR("Ignoring data beyond length 15",0,0,0);
  }

  for (i=0; i<MIN(len,15); i++) {
    dest[i] = (src->digitn[i]==10)? (char)'0' : (char)src->digitn[i]+'0';
  }
}

/*===========================================================================
FUNCTION FormatPhoneNum

DESCRIPTION
  Get phone num: if CDMA: from NV, if WCDMA or GSM, from sim/usim

DEPENDENCIES
  ui_get_nv, ui_put_nv, gsdi/ICard functions (TBD)

SIDE EFFECTS
  None
===========================================================================*/
static int FormatPhoneNum(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen = 0;

#if defined (FEATURE_CDMA1X)
  nv_item_type nvi;
  nv_mob_dir_data_type nv_dir_num; /* current phone number */
  int i, j;
  char pszTempbuf[NV_DIR_NUMB_PCS_SIZ];
  char pszBuf[NV_DIR_NUMB_PCS_SIZ];
#endif /* F_CDMA_800 || F_CDMA_1900 */

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_PHONENO,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;


  if (pMe->m_ssInfo.sys_mode == AEECM_SYS_MODE_CDMA) {
    /* 1x mode */
#if defined (FEATURE_CDMA1X)

/*lint -e737*/
/*lint -e574*/
    (void) MEMSET (pszTempbuf, 0, ARR_SIZE(pszTempbuf));
    (void) MEMSET (pszBuf, 0, ARR_SIZE(pszBuf));
/*lint +e737*/
/*lint +e574*/

    /* Get all the NAM/MIN information that we need */
    if(ui_get_nv( NV_CURR_NAM_I, &nvi ) == NV_NOTACTIVE_S) /* get current nam */
    {
      // Default it to first NAM
      nvi.curr_nam = 0;
      (void) ui_put_nv ( NV_CURR_NAM_I, &nvi );
    }

    /* If the NAM is out of range, default to the first NAM */
    if ( nvi.curr_nam >= NV_MAX_NAMS ) {
      nvi.curr_nam = 0;
      (void) ui_put_nv ( NV_CURR_NAM_I, &nvi );
    }

    nv_dir_num.nam = nvi.curr_nam;

    if (ui_get_nv ( NV_DIR_NUMBER_PCS_I, (nv_item_type *)&nv_dir_num ) == NV_DONE_S)
    {
      /* Translate it to ASCII format */
      DirNumPCSToText(pszTempbuf, &nv_dir_num);
    }
    else
    {
      nv_dir_num.n_digits =0;
      (void) ui_put_nv ( NV_DIR_NUMBER_PCS_I, (nv_item_type *)&nv_dir_num );
      MSG_ERROR("Ignoring Dir Number",0,0,0);
    }

    if (nv_dir_num.n_digits == 0) {
      ISHELL_LoadResString (pMe->a.m_pIShell,
                      gpszResourceFile,
                      IDS_INVALID,
                      &pszContent[nBufLen],
                      (nLen - nBufLen) * (int) sizeof(AECHAR));
      nBufLen = WSTRLEN (pszContent);
    }
    else {
      if (nv_dir_num.n_digits <= 10) {
        /* If 10 or less digits, format as 'xxx-xxx-xxxx' with '0' padded left */
        for (i=11, j=nv_dir_num.n_digits; i>=0; i--) {
          if (j>0) {
            pszBuf[i] = (char)pszTempbuf[--j];
          }
          else {
            pszBuf[i] = '0';
          }
          if ( i==4 || i==8 ) {
            pszBuf[--i] = '-';
          }
        }
        nv_dir_num.n_digits += 2;
      }
      else {
        (void)MEMCPY(pszBuf, pszTempbuf, nv_dir_num.n_digits);
      }

      pszBuf[nv_dir_num.n_digits] = 0;

      (void)STRTOWSTR(pszBuf, &pszContent[nBufLen],
                      (nLen-nBufLen)*(int)sizeof(AECHAR));
      nBufLen = WSTRLEN(pszContent);
    }
#endif /*defined (FEATURE_CDMA1X) */
  }
  else {
    /* GW mode */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    IAddrBook    *pIAddrBook = NULL;
    IAddrRec     *pIAddrRec = NULL;
    int           wNumFields;
    int           i;
    AEEAddrField *pField;
    int           nReturnStatus = ENOMEMORY;
    AEECardApp    cardApp;
    int           wCat = AEE_ADDR_CAT_SIM_MSISDN; /* Default to slot 1 */
    boolean       bFound = FALSE;
    boolean       bFirstNum = TRUE;

    do {

      /* In case of dual slot, we display the MSISDN numbers from the slot where
       * GSM subscription came from */
      if (!pMe->m_pICard) {
        MSG_ERROR("ICARD pointer is NULL", nReturnStatus, 0, 0);
        nReturnStatus = ENOMEMORY;
        break;
      }

      if ((nReturnStatus = ICARD_GetCardApps(pMe->m_pICard,
                                            &cardApp)) != AEE_SUCCESS) {
        MSG_ERROR("Get Card Apps failed %d", nReturnStatus, 0, 0);
        break;
      }

      /* Use MSISDN2 if that slot is operational */
      if (cardApp.operational_slot & AEECARD_GSM_SLOT2_OPERATIONAL)
        wCat = AEE_ADDR_CAT_SIM_MSISDN2; /* Change to slot2 */

      /* Create Address Book */
      if ((nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                  AEECLSID_ADDRBOOK,
                                  (void **) &pIAddrBook)) != AEE_SUCCESS) {
        MSG_ERROR("Failed to create IAddrBook %d", nReturnStatus, 0, 0);
        break;
      }

      /* Initialize enumeration */
      nReturnStatus = IADDRBOOK_EnumRecInit(pIAddrBook, wCat,
                                            AEE_ADDRFIELD_NONE, NULL, 0);
    } while (FALSE); /*lint !e717 */

    /* Add string based on return value */
    switch (nReturnStatus) {

      case AEE_SUCCESS:
        /* Add each number */
        while ((pIAddrRec = IADDRBOOK_EnumNextRec(pIAddrBook)) != NULL) {
          bFound = TRUE;

          wNumFields = IADDRREC_GetFieldCount(pIAddrRec);

          /* Get phone number from the record */
          for (i = 0; i < wNumFields; i++) {
            pField = IADDRREC_GetField(pIAddrRec, i);

            if (pField && pField->fType == AEEDB_FT_PHONE) {
              AECHAR *wszFormattedNum;

              wszFormattedNum = FormatPhoneNumberForDisplay(pField->pBuffer);
              if (wszFormattedNum)
              {
                if (!bFirstNum)
                {
                  pszContent[nBufLen++] = '/';
                }
                bFirstNum = FALSE;

                nBufLen += WSTRLCPY(&pszContent[nBufLen],
                                    wszFormattedNum, nLen - nBufLen);
                FREE(wszFormattedNum);
              }
            }
          } /* For each field */

          IADDRREC_Release(pIAddrRec);
        } /* For each number */

        if (!bFound) {
          ISHELL_LoadResString(pMe->a.m_pIShell,
                               gpszResourceFile,
                               IDS_NOINFO,
                               &pszContent[nBufLen],
                               (nLen - nBufLen) * (int) sizeof(AECHAR));
          nBufLen = WSTRLEN (pszContent);
        }
        break;

      case EITEMBUSY:
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             gpszResourceFile,
                             IDS_SIM_NOT_READY,
                             &pszContent[nBufLen],
                             (nLen - nBufLen) * (int) sizeof(AECHAR));
        nBufLen = WSTRLEN (pszContent);
        break;

      default:
        MSG_ERROR("Error in obtaining MSISDN: %d, %x", nReturnStatus,
                                                       pIAddrBook, 0);
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             gpszResourceFile,
                             IDS_INVALID,
                             &pszContent[nBufLen],
                             (nLen - nBufLen) * (int) sizeof(AECHAR));
        nBufLen = WSTRLEN (pszContent);
        break;
    }

    if (pIAddrBook)
      IADDRBOOK_Release(pIAddrBook);

#endif
  }
  pszContent[nBufLen++] = '\n';
  return nBufLen;
} /* End of FormatPhoneNum */


/*===========================================================================
FUNCTION FormatMailboxNum

DESCRIPTION
  Get phone num: if WCDMA or GSM, from sim/usim (MBDN)

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static int FormatMailboxNum(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen = 0;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  IAddrBook    *pIAddrBook = NULL;
  IAddrRec     *pIAddrRec = NULL;
  int           wNumFields;
  int           i;
  AEEAddrField *pField;
  int           nReturnStatus = ENOMEMORY;
  AEECardApp    cardApp;
  int           wCat = AEE_ADDR_CAT_SIM_MBDN; /* Default to slot 1 */
  boolean       bFound = FALSE;
  boolean       bFirstNum = TRUE;

  ISHELL_LoadResString (pMe->a.m_pIShell,
                        gpszResourceFile,
                        IDS_MAILBOXNUM,
                        pszContent,
                        nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  do
  {

    /* In case of dual slot, we display the MBDN numbers from the slot where
     * GSM subscription came from */
    if (!pMe->m_pICard)
    {
      MSG_ERROR("ICARD pointer is NULL", nReturnStatus, 0, 0);
      nReturnStatus = ENOMEMORY;
      break;
    }

    if ((nReturnStatus = ICARD_GetCardApps(pMe->m_pICard,
                                           &cardApp)) != AEE_SUCCESS)
    {
      MSG_ERROR("Get Card Apps failed %d", nReturnStatus, 0, 0);
      break;
    }

    /* Use MBDN2 if that slot is operational */
    if (cardApp.operational_slot & AEECARD_GSM_SLOT2_OPERATIONAL)
      wCat = AEE_ADDR_CAT_SIM_MBDN2; /* Change to slot2 */

    /* Create Address Book */
    if ((nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                               AEECLSID_ADDRBOOK,
                                               (void **) &pIAddrBook)) != AEE_SUCCESS)
    {
      MSG_ERROR("Failed to create IAddrBook %d", nReturnStatus, 0, 0);
      break;
    }

    /* Initialize enumeration */
    nReturnStatus = IADDRBOOK_EnumRecInit(pIAddrBook, wCat,
                                          AEE_ADDRFIELD_NONE, NULL, 0);
  } while (FALSE); /*lint !e717 */

  /* Add string based on return value */
  switch (nReturnStatus)
  {
    case AEE_SUCCESS:
      /* Add each number */
      while ((pIAddrRec = IADDRBOOK_EnumNextRec(pIAddrBook)) != NULL)
      {
        bFound = TRUE;

        wNumFields = IADDRREC_GetFieldCount(pIAddrRec);

        /* Get phone number from the record */
        for (i = 0; i < wNumFields; i++)
        {
          pField = IADDRREC_GetField(pIAddrRec, i);

          if (pField && pField->fType == AEEDB_FT_PHONE)
          {
            AECHAR *wszFormattedNum;

            wszFormattedNum = FormatPhoneNumberForDisplay(pField->pBuffer);
            if (wszFormattedNum)
            {
              if (!bFirstNum)
              {
                pszContent[nBufLen++] = '/';
              }
              bFirstNum = FALSE;

              nBufLen += WSTRLCPY(&pszContent[nBufLen],
                                  wszFormattedNum, nLen - nBufLen);
              FREE(wszFormattedNum);
            }
          }
        } /* For each field */

        IADDRREC_Release(pIAddrRec);
      } /* For each number */

      if (!bFound)
      {
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             gpszResourceFile,
                             IDS_NOINFO,
                             &pszContent[nBufLen],
                             (nLen - nBufLen) * (int) sizeof(AECHAR));
        nBufLen = WSTRLEN (pszContent);
      }
      break;

    case EITEMBUSY:
      ISHELL_LoadResString(pMe->a.m_pIShell,
                           gpszResourceFile,
                           IDS_SIM_NOT_READY,
                           &pszContent[nBufLen],
                           (nLen - nBufLen) * (int) sizeof(AECHAR));
      nBufLen = WSTRLEN (pszContent);
      break;

    default:
      MSG_ERROR("Error in obtaining MBDN: %d, %x", nReturnStatus,
                pIAddrBook, 0);
      ISHELL_LoadResString(pMe->a.m_pIShell,
                           gpszResourceFile,
                           IDS_INVALID,
                           &pszContent[nBufLen],
                           (nLen - nBufLen) * (int) sizeof(AECHAR));
      nBufLen = WSTRLEN (pszContent);
      break;
  }

  if (pIAddrBook)
    IADDRBOOK_Release(pIAddrBook);


  pszContent[nBufLen++] = '\n';
#endif /* FEATURE_GSM || FEATURE_WCDMA */

  return nBufLen;
} /* End of FormatVoiceMailNum */

/*===========================================================================
FUNCTION FormatCPHSMailboxNum

DESCRIPTION
  Get phone num: if WCDMA or GSM, from sim/usim (MBN)

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static int FormatCPHSMailboxNum(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen = 0;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  IAddrBook    *pIAddrBook = NULL;
  IAddrRec     *pIAddrRec = NULL;
  int           wNumFields;
  int           i;
  AEEAddrField *pField;
  int           nReturnStatus = ENOMEMORY;
  AEECardApp    cardApp;
  int           wCat = AEE_ADDR_CAT_SIM_MBN; /* Default to slot 1 */
  boolean       bFound = FALSE;
  boolean       bFirstNum = TRUE;

  ISHELL_LoadResString (pMe->a.m_pIShell,
                        gpszResourceFile,
                        IDS_MAILBOXNUM,
                        pszContent,
                        nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  do
  {

    /* In case of dual slot, we display the MBN numbers from the slot where
     * GSM subscription came from */
    if (!pMe->m_pICard)
    {
      MSG_ERROR("ICARD pointer is NULL", nReturnStatus, 0, 0);
      nReturnStatus = ENOMEMORY;
      break;
    }

    if ((nReturnStatus = ICARD_GetCardApps(pMe->m_pICard,
                                           &cardApp)) != AEE_SUCCESS)
    {
      MSG_ERROR("Get Card Apps failed %d", nReturnStatus, 0, 0);
      break;
    }

    /* Use MBN2 if that slot is operational */
    if (cardApp.operational_slot & AEECARD_GSM_SLOT2_OPERATIONAL)
      wCat = AEE_ADDR_CAT_SIM_MBN2; /* Change to slot2 */

    /* Create Address Book */
    if ((nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                               AEECLSID_ADDRBOOK,
                                               (void **) &pIAddrBook)) != AEE_SUCCESS)
    {
      MSG_ERROR("Failed to create IAddrBook %d", nReturnStatus, 0, 0);
      break;
    }

    /* Initialize enumeration */
    nReturnStatus = IADDRBOOK_EnumRecInit(pIAddrBook, wCat,
                                          AEE_ADDRFIELD_NONE, NULL, 0);
  } while (FALSE); /*lint !e717 */

  /* Add string based on return value */
  switch (nReturnStatus)
  {
    case AEE_SUCCESS:
      /* Add each number */
      while ((pIAddrRec = IADDRBOOK_EnumNextRec(pIAddrBook)) != NULL)
      {
        bFound = TRUE;

        wNumFields = IADDRREC_GetFieldCount(pIAddrRec);

        /* Get phone number from the record */
        for (i = 0; i < wNumFields; i++)
        {
          pField = IADDRREC_GetField(pIAddrRec, i);

          if (pField && pField->fType == AEEDB_FT_PHONE)
          {
            AECHAR *wszFormattedNum;

            wszFormattedNum = FormatPhoneNumberForDisplay(pField->pBuffer);
            if (wszFormattedNum)
            {
              if (!bFirstNum)
              {
                pszContent[nBufLen++] = '/';
              }
              bFirstNum = FALSE;

              nBufLen += WSTRLCPY(&pszContent[nBufLen],
                                  wszFormattedNum, nLen - nBufLen);
              FREE(wszFormattedNum);
            }
          }
        } /* For each field */

        IADDRREC_Release(pIAddrRec);
      } /* For each number */

      if (!bFound)
      {
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             gpszResourceFile,
                             IDS_NOINFO,
                             &pszContent[nBufLen],
                             (nLen - nBufLen) * (int) sizeof(AECHAR));
        nBufLen = WSTRLEN (pszContent);
      }
      break;

    case EITEMBUSY:
      ISHELL_LoadResString(pMe->a.m_pIShell,
                           gpszResourceFile,
                           IDS_SIM_NOT_READY,
                           &pszContent[nBufLen],
                           (nLen - nBufLen) * (int) sizeof(AECHAR));
      nBufLen = WSTRLEN (pszContent);
      break;

    default:
      MSG_ERROR("Error in obtaining MBN: %d, %x", nReturnStatus,
                pIAddrBook, 0);
      ISHELL_LoadResString(pMe->a.m_pIShell,
                           gpszResourceFile,
                           IDS_INVALID,
                           &pszContent[nBufLen],
                           (nLen - nBufLen) * (int) sizeof(AECHAR));
      nBufLen = WSTRLEN (pszContent);
      break;
  }

  if (pIAddrBook)
    IADDRBOOK_Release(pIAddrBook);

  pszContent[nBufLen++] = '\n';
#endif /* FEATURE_GSM || FEATURE_WCDMA */

  return nBufLen;
} /* End of FormatCPHSVoiceMailNum */



/*===========================================================================
FUNCTION FormatVersion

DESCRIPTION
  Get software version from mobile

DEPENDENCIES
  ver_dir variable

SIDE EFFECTS
  None
===========================================================================*/
static int FormatVersion(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen = 0;

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_SWVERSION,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  (void) STRTOWSTR(mob_sw_rev, &pszContent[nBufLen], (nLen-nBufLen)*(int) sizeof(AECHAR));
  nBufLen = WSTRLEN(pszContent);
  pszContent[nBufLen++] = '\n';
  return nBufLen;
} /* End of FormatVersion */

/*===========================================================================
FUNCTION FormatBREWVersion

DESCRIPTION
  Get software version from mobile

DEPENDENCIES
  ver_dir variable

SIDE EFFECTS
  None
===========================================================================*/
static int FormatBREWVersion(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen = 0;
  byte buf[30];

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_BREWVERSION,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

/*lint -e737*/
/*lint -e574*/
  (void) GETAEEVERSION(buf, ARR_SIZE(buf), GAV_LATIN1);
/*lint +e737*/
/*lint +e574*/
  (void) STRTOWSTR((char *) buf, &pszContent[nBufLen], (nLen-nBufLen)*(int) sizeof(AECHAR));
  nBufLen = WSTRLEN(pszContent);
  pszContent[nBufLen++] = '\n';
  return nBufLen;
} /* End of FormatVersion */


/*===========================================================================
FUNCTION FormatESN

DESCRIPTION
  Get ESN from nv

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
static int FormatESN(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  nv_item_type nvi;
  int nBufLen = 0;
  char pszWorkBuf[20];
#ifdef FEATURE_MEID_SUPPORT
  nv_stat_enum_type  nvstat = NV_NOTACTIVE_S;
#endif /* FEATURE_MEID_SUPPORT */

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_ESN,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  (void)ui_get_nv(NV_ESN_CHKSUM_I,&nvi);

  if (nvi.esn_chksum.chksum != (dword) NV_VALID_ESN_CHKSUM) {

    ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_INVALID,
                    &pszContent[nBufLen],
                    (nLen - nBufLen) * (int) sizeof(AECHAR));

    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = '\n';

  } else {

    dword manufacturer_id, serial_no;

    (void)ui_get_nv(NV_ESN_I, &nvi);

    manufacturer_id = (nvi.esn.esn & 0xFF000000) >> 24;
    serial_no       = (nvi.esn.esn & 0x0FFFFFFL);

    (void) SNPRINTF(pszWorkBuf, sizeof(pszWorkBuf), "%02X%06X\0", manufacturer_id, serial_no);

    (void) STRTOWSTR (pszWorkBuf, &pszContent[nBufLen], (nLen-nBufLen)*(int) sizeof(AECHAR));
    nBufLen = WSTRLEN(pszContent);
    pszContent[nBufLen++] = '\n';
  }
#ifdef FEATURE_MEID_SUPPORT
  /* Check if the ESN is a Pseudo ESN (i.e. MSB is set to 0x80)
     If it is, then MEID is to be read and displayed */
  if (COREAPP_PSEUDO_ESN_UBYTE == ((nvi.esn.esn & 0xFF000000) >> 24))
  {
     nvstat = ui_get_nv(NV_MEID_I,&nvi);
     if (NV_DONE_S == nvstat)
     {
       {
         dword meid_hi, meid_lo;
         meid_lo = (nvi.meid[0] & 0xFFFFFFFFL);
         meid_hi = (nvi.meid[1] & 0xFFFFFFFFL);

         SNPRINTF(pszWorkBuf, sizeof(pszWorkBuf), "MEID HI %06X\0", meid_hi);
         STRTOWSTR (pszWorkBuf, &pszContent[nBufLen], (nLen-nBufLen)*sizeof(AECHAR));
         nBufLen = WSTRLEN(pszContent);
         pszContent[nBufLen++] = '\n';


         SNPRINTF(pszWorkBuf, sizeof(pszWorkBuf), "MEID LO %08X\0", meid_lo);
         STRTOWSTR (pszWorkBuf, &pszContent[nBufLen], (nLen-nBufLen)*sizeof(AECHAR));
         nBufLen = WSTRLEN(pszContent);
         pszContent[nBufLen++] = '\n';
       }
     }
     else
     {
       CORE_ERR("MEID Read not a success..Value Returned = %d",nvstat,0,0);
     }

  } /* end if */
#endif /* FEATURE_MEID_SUPPORT */
  return nBufLen;
} /* End of FormatESN */



/*===========================================================================
FUNCTION FormatPRL

DESCRIPTION
  Get PRL

DEPENDENCIES
  ui_get_nv,sd_misc_prl_ver_get, mc_get_prl_version

SIDE EFFECTS
  None
===========================================================================*/
static int FormatPRL(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen=0;
  int j;
  char pszText[12];
  int nPos = 0;
  word prl_ver = 0;
  int nTextLen;

/*lint -e737*/
/*lint -e574*/
  (void) MEMSET (pszText,' ', ARR_SIZE(pszText));
/*lint +e737*/
/*lint +e574*/
  pszText[11] = 0;

  nTextLen = (int) STRLEN(pszText);


#ifdef FEATURE_SD20
  prl_ver = sd_misc_prl_ver_get();
#else
#if defined FEATURE_CDMA1X
  prl_ver = mc_get_prl_version();
#endif
#endif

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_PRLVERSION,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;
/*lint -e774*/
  if (prl_ver == 0) {
    ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_NOINFO,
                    &pszContent[nBufLen],
                    (nLen-nBufLen)*(int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = '\n';
    return nBufLen;
  }
  else {
    for (j = nTextLen-1; j >= 0; j--) {
      pszText[j] = (byte) (prl_ver % 10 + '0');
      prl_ver /= 10;
      if (prl_ver == 0) { /* done */
        nTextLen -= j;
        nPos = j;
        break;
      }
    }
/*lint +e774*/
    (void) STRTOWSTR(&pszText[nPos], &pszContent[nBufLen], (nLen-nBufLen)*(int)sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = '\n';
    return nBufLen;
  }

} /* End FormatPRL */

/*===========================================================================
FUNCTION FormatPLMN

DESCRIPTION
  Get PRL

DEPENDENCIES
  ui_get_nv,sd_misc_prl_ver_get, mc_get_prl_version

SIDE EFFECTS
  None
Note: For GW modes when the MM Info is received with Long name or
      short name for the PLMN name, it is also saved in the NV. When a
      PLMN ID is used, temporarily it still displays the stored PLMN name
      if it is not avaialable in the NV. This has to be changed if NV also
      saves the database of PLMN names vs IDs and display the PLMN name
      according to the ID received.
===========================================================================*/
static int FormatPLMN(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{
  int nBufLen=0;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  char *plmn_buf = NULL;
  char buf[MAX_STR_SIZE+1];
  AECHAR wBuf[MAX_STR_SIZE+1];

  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_PLMN,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));

  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

  if(pMe->m_ssInfo.srv_capability != AEECM_SRV_CAPABILITY_NOT_AVAILABLE &&
    pMe->m_ssInfo.srv_capability != AEECM_SRV_CAPABILITY_NONE)
  {
    if (SUCCESS != CoreUtils_GetPLMN(pMe, &plmn_buf, FALSE)) 
    {
      return nBufLen;
    }

    (void) STRTOWSTR(plmn_buf,wBuf,sizeof(AECHAR)* MAX_STR_SIZE);
    (void) WSTRLCAT(pszContent, wBuf, nLen );
    FREEIF(plmn_buf);

    // Forbidden?
    buf[0] = '\0';
    wBuf[0] = '\0';

    if(pMe->m_ssInfo.plmn_forbidden)
    {
      (void) SNPRINTF(buf,sizeof(buf)," Forbidden");
      (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
      (void) WSTRLCAT(pszContent, wBuf, nLen);

    }

    // RAT
    if (pMe->m_ssInfo.sys_mode == AEECM_SYS_MODE_GSM)
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n RAT: GSM");
    }
    else if(pMe->m_ssInfo.sys_mode == AEECM_SYS_MODE_WCDMA)
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n RAT: UMTS");
    }
    else
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n RAT: Unknown");
    }
    (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
    (void) WSTRLCAT(pszContent, wBuf, nLen);


    // CS,PS?
    buf[0] = '\0';
    if(pMe->m_ssInfo.srv_capability == AEECM_SRV_CAPABILITY_CS_SERVICE_ONLY)
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n Service Capability: CS");
    }
    else if(pMe->m_ssInfo.srv_capability == AEECM_SRV_CAPABILITY_PS_SERVICE_ONLY)
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n Service Capability: PS");
    }
    else if(pMe->m_ssInfo.srv_capability == AEECM_SRV_CAPABILITY_CS_PS_SERVICE)
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n Service Capability: CS,PS");
    }
    else
    {
      (void) SNPRINTF(buf,sizeof(buf),"\n Service Capability: Invalid");
    }
    (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
    (void) WSTRLCAT(pszContent, wBuf, nLen);

    buf[0] = '\0';
    switch (pMe->m_ssInfo.srv_domain)
    {
      case AEECM_SRV_DOMAIN_CS_ONLY:
        (void) SNPRINTF(buf,sizeof(buf),"\n Current Srv: CS");
        break;
      case AEECM_SRV_DOMAIN_PS_ONLY:
        (void) SNPRINTF(buf,sizeof(buf),"\n Current Srv: PS");
        break;
      case AEECM_SRV_DOMAIN_CS_PS:
        (void) SNPRINTF(buf,sizeof(buf),"\n Current Srv: CS,PS");
        break;
      default:
        (void) SNPRINTF(buf,sizeof(buf),"\n Current Srv: Invalid");
        break;
    }

    (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE );
    (void) WSTRLCAT(pszContent, wBuf, nLen);

    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = '\n';
    return nBufLen;
  }
  else
#endif /* FEATURE_WCDMA || FEATURE_GSM */
  {
    ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_PLMN,
                    pszContent,
                    nLen * (int) sizeof(AECHAR));

    nBufLen = WSTRLEN (pszContent);

    if (nBufLen >= nLen)
      return nBufLen;

    ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_NOINFO,
                    &pszContent[nBufLen],
                    (nLen-nBufLen)* (int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = '\n';
    return nBufLen;
  }

} /* End FormatPLMN */

/*===========================================================================
FUNCTION FormatIMSI

DESCRIPTION
  Get IMSI: if CDMA: from NV, if WCDMA or GSM, from IMSI file in sim/usim

DEPENDENCIES
  ui_get_nv, gsdi_get_card_mode, gsdi_sim_read

SIDE EFFECTS
  None
===========================================================================*/
static int FormatIMSI(CCoreApp *pMe, AECHAR* pszContent, int nLen)
{

  char pszText[20];
  int nBufLen = 0;

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
  // TO DO - have an ICard API for this
  gsdi_returns_T                  status;
  uim_items_enum_type          sim_file_name_imsi = UIM_NO_SUCH_ITEM;
  gsdi_card_mode_capability_T  psCardMode;
  int dummy;
  byte pbyIMSI[GSDI_IMSI_LEN];
  int i, j;
#endif /*defined(FEATURE_WCDMA) || defined(FEATURE_GSM)*/
  nv_item_type nvi;
  byte byNam;
  int nTextLen = 0;


/*lint -e737*/
/*lint -e574*/
  (void) MEMSET (pszText, ' ', ARR_SIZE(pszText));
  pszText[ARR_SIZE(pszText)-1] = 0;
/*lint +e737*/
/*lint +e574*/

  if (pMe->m_rtreControl == AEECM_RTRE_CONTROL_NV) {
    ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_IMSI,
                    pszContent,
                    nLen * (int) sizeof(AECHAR));
  }
  else {
    ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_TRUEIMSI,
                  pszContent,
                  nLen * (int) sizeof(AECHAR));
  }
  nBufLen = WSTRLEN (pszContent);

  if (nBufLen >= nLen)
    return nBufLen;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  if (pMe->m_rtreControl != AEECM_RTRE_CONTROL_NV) {
    if(gsdi_get_card_mode(&psCardMode) != GSDI_SUCCESS)
    {
      ERR("SIM either not initialized or invalid", 0,0,0);
      ISHELL_LoadResString (pMe->a.m_pIShell,
                      gpszResourceFile,
                      IDS_INVALID,
                      &pszContent[nBufLen],
                      (nLen - nBufLen) * (int) sizeof(AECHAR));

      nBufLen = WSTRLEN (pszContent);
      pszContent[nBufLen++] = '\n';
      return nBufLen;
    }

    /* Get the sim file name based on the sim mode */
    if( psCardMode == GSDI_CARD_GSM)
    {
      sim_file_name_imsi = UIM_GSM_IMSI;
    }
    else if( psCardMode == GSDI_CARD_USIM)
    {
      sim_file_name_imsi = UIM_USIM_IMSI;
    }
    else
    {
      ERR("Invalid SIM card mode %d", 0,0,0);
      ISHELL_LoadResString (pMe->a.m_pIShell,
                      gpszResourceFile,
                      IDS_INVALID,
                      &pszContent[nBufLen],
                      (nLen - nBufLen) * (int) sizeof(AECHAR));

      nBufLen = WSTRLEN (pszContent);
      pszContent[nBufLen++] = '\n';
      return nBufLen;
    }

    status = gsdi_sim_read( sim_file_name_imsi,
                            GSDI_NO_INDEX,
                            0,
                            NULL,
                            0,
                            (byte *) &pbyIMSI[0],
                            GSDI_IMSI_LEN,
                            0,
                            &dummy, 0, NULL );

    if (status != GSDI_SUCCESS )
    {
      ERR("gsdi_sim_read() failure, GDSI status %d", status,0,0);
      ISHELL_LoadResString (pMe->a.m_pIShell,
                      gpszResourceFile,
                      IDS_INVALID,
                      &pszContent[nBufLen],
                      (nLen - nBufLen) * (int) sizeof(AECHAR));

      nBufLen = WSTRLEN (pszContent);
      pszContent[nBufLen++] = '\n';
      return nBufLen;
    }

    j = 0;
    pszText[j++] = ((pbyIMSI[1] & 0xF0)>>4) + '0';
    for (i = 2; i < pbyIMSI[0]; i++) {
      pszText[j++] = (pbyIMSI[i] & 0x0F) + '0';
      pszText[j++] = ((pbyIMSI[i] & 0xF0)>>4) + '0';
    }
    if((pbyIMSI[1] & 0x0F) == 0x9) { /* even # digit */
      pszText[j++] = (pbyIMSI[i] & 0x0F) + '0';
      pszText[j++] = ((pbyIMSI[i] & 0xF0)>>4) + '0';
    }
    else { /* odd # digit */
      pszText[j++] = (pbyIMSI[i] & 0x0F) + '0';
    }

    pszText[j] = 0;

    (void) STRTOWSTR(pszText, &pszContent[nBufLen], (nLen - nBufLen)*(int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
  }
  else
#endif /* FEATURE_MMGSDI */
  {
    /* Get all the NAM/MIN information that we need */
    (void) ui_get_nv( NV_CURR_NAM_I, &nvi ); /* get current nam */

    byNam = nvi.curr_nam;

    /* Get the MCC - len 3 */
    nvi.imsi_t_mcc.nam = (byte) byNam;
    (void) ui_get_nv( NV_IMSI_T_MCC_I, &nvi );

    nTextLen = MccToAsc(nvi.imsi_t_mcc.imsi_mcc, pszText);
    pszText[nTextLen] = 0;

    (void) STRTOWSTR(pszText, &pszContent[nBufLen], (nLen - nBufLen)*(int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = ' '; /* overwrite the NULL */


    if (nBufLen >= nLen) {
      return nBufLen;
    }

    /* Get 11_12 */
    nvi.imsi_t_11_12.nam = (byte) byNam;
    (void) ui_get_nv( NV_IMSI_T_11_12_I, &nvi);

    nTextLen = MncToAsc(nvi.imsi_11_12.imsi_11_12, pszText);

    pszText[nTextLen] = 0;

    (void) STRTOWSTR(pszText, &pszContent[nBufLen], (nLen - nBufLen)* (int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);
    pszContent[nBufLen++] = ' '; /* overwrite the NULL */

    if (nBufLen >= nLen) {
      return nBufLen;
    }

    /* Get area code */
    nvi.imsi_t_s2.nam = (byte) byNam;
    (void) ui_get_nv( NV_IMSI_T_S2_I, &nvi );

    nTextLen = Min2ToAsc( nvi.min2.min2[CDMAMIN], pszText );
    pszText[nTextLen] = 0;

    (void) STRTOWSTR(pszText, &pszContent[nBufLen], (nLen - nBufLen)*(int) sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent); /* overwrite the NULL */

    if (nBufLen >= nLen) {
      return nBufLen;
    }

    /* Get phone number */
    nvi.imsi_t_s1.nam = (byte) byNam;
    (void) ui_get_nv( NV_IMSI_T_S1_I, &nvi );

    nTextLen = Min1ToAsc( nvi.min1.min1[CDMAMIN], pszText );
    pszText[nTextLen] = 0;

    (void) STRTOWSTR(pszText, &pszContent[nBufLen], (nLen - nBufLen)*(int)sizeof(AECHAR));
    nBufLen = WSTRLEN (pszContent);

  }
  pszContent[nBufLen++] = '\n';
  return nBufLen;

} /* End of FormatIMSI */


/*===========================================================================
FUNCTION CreateTextEditDlg

DESCRIPTION
  Get next dialog ID and create the new dialog

DEPENDENCIES
  Note: This function should ONLY be called when the current dialog has a
        IStatic control. ?? for now, may make it more generalize later
  GetNextEditDlgID

SIDE EFFECTS
  None
===========================================================================*/
static boolean CreateTextEditDlg(CCoreApp* pMe)
{
  IDialog* pIActiveDialog;
  uint16 wDlgID;
  uint16 wCurrentDlgID;
  int nReturnStatus;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  wCurrentDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wCurrentDlgID == 0) { /*error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get next dialog ID */
  wDlgID = GetNextEditDlgID(wCurrentDlgID);
  if(wDlgID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
              wDlgID, NULL);
  return CheckAEEReturnStatus(nReturnStatus);
} /* End CreateTextEditDlg */



/*===========================================================================
FUNCTION GetStaticDlgTitleResStringID

DESCRIPTION
  This function matches dialog id with a resource string ID used for static
  control title

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetStaticDlgTitleResStringID(CCoreApp *pMe, uint16 wDlgID)
{
  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -e737*/
/*lint -e574*/
  for (i = 0; i< ARR_SIZE(StaticDlgs); i++) {
    if (StaticDlgs[i].wDialogID == wDlgID) {
      return StaticDlgs[i].wDialogTitle;
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetStaticDlgTitleResStringID(): No matching dialog", 0, 0, 0);
  return 0; /* error */
} /* End GetStaticDlgTitleResStringID */

/*===========================================================================
FUNCTION SetStaticDlgTitle

DESCRIPTION
  Based on IDD, assign title for the static dialog

DEPENDENCIES
  Note: Still need to set the title even if it was created in the resource editor,
        because of the ISTATIC_SetText function call later on

SIDE EFFECTS
  None
===========================================================================*/
static void SetStaticDlgTitle(CCoreApp *pMe, uint16 wDlgID, AECHAR *title, int nLen)
{

  uint16 wStrID = 0;
  int nResult = 0;

  wStrID = GetStaticDlgTitleResStringID(pMe,wDlgID);

  nResult = ISHELL_LoadResString(pMe->a.m_pIShell,
                          gpszResourceFile,
                          wStrID,
                          title,
                          nLen*((int)sizeof(AECHAR)));
  if(nResult == 0) {
    CORE_ERR("Failed to set title for static dlg %d",wDlgID,0,0);
  }

} /* End SetStaticDlgTitle */

/*===========================================================================
FUNCTION SetStaticDlgContent

DESCRIPTION
  Based on IDD, assign content for the static dialog

DEPENDENCIES
  GetNVBannerSetting

SIDE EFFECTS
  None
===========================================================================*/
static void SetStaticDlgContent(CCoreApp *pMe, uint16 wDlgID, AECHAR* pszContent, int nLen)
{
  AECHAR* psValue;
  int nCurLen = 0;
  JulianType time_d;

  switch(wDlgID)
  {
    case IDD_MYINFO:
      pszContent[nCurLen++] = '\n';
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatPhoneNum(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatMailboxNum(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatCPHSMailboxNum(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatVersion(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatESN(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatPRL(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatIMSI(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatPLMN(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatSystemTime(pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      nCurLen += FormatBREWVersion (pMe, &pszContent[nCurLen], (nLen - nCurLen));
      if (nCurLen >= nLen)
        break;
      pszContent[nCurLen] = '\0';
      break;

    case IDD_BANNER_VIEW:
      psValue = GetBanner(pMe);
      if ((psValue != NULL) && (nLen >= (BANNER_BUFFER_SIZE+1))) {
        (void) MEMCPY(pszContent, psValue, (BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));
      }
      break;

    case IDD_SETTIME_VIEW:
    (void) GetTimeofDay (pMe, &time_d);
    (void) FormatTimeDate (time_d, pszContent, nLen);
      break;

    case IDD_RESTORE_SETTINGS:
      ISHELL_LoadResString (pMe->a.m_pIShell,
                gpszResourceFile,
                IDS_RESTOREALL,
                &pszContent[nCurLen],
              (nLen - nCurLen) * (int) sizeof(AECHAR));
      break;

    default: /* error */
      pszContent[0] = (AECHAR)'\0';
      CORE_ERR("SetStaticDlgContent(): no matching IDD", 0, 0, 0);
      break;
  }
} /* End SetStaticDlgContent */



/*===========================================================================
FUNCTION GetDlgStaticCtlID

DESCRIPTION
  return the control for the static dlg

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint16 GetDlgStaticCtlID(uint16 wDlgID)
{

  int i;

  if ( wDlgID == 0 ) {
    return 0;
  }

/*lint -e737*/
/*lint -e574*/
  for (i = 0; i< ARR_SIZE(StaticDlgs); i++) {
    if (StaticDlgs[i].wDialogID == wDlgID) {
      return StaticDlgs[i].wCtrlID;
    }
  }
/*lint +e737*/
/*lint +e574*/

  CORE_ERR( "GetDlgStaticCtlID(): No matching static ctl", 0, 0, 0);
  return 0; /* error */

} /* End GetDlgStaticCtlID */



/*===========================================================================
FUNCTION InitStaticDlg

DESCRIPTION
  Set the static dialog title, content and style

DEPENDENCIES
  SetStaticDlgTitle, SetStaticDlgContent, SetGeneralStaticTextStyle,
  SetStaticTextCtlSize

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitStaticDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIDialog)
{
  IStatic* pIStaticText;
  IMenuCtl* pIMenu;
  AECHAR title[TITLE_BUFFER_SIZE];
  AECHAR psContent[TEXT_BUFFER_SIZE];
  uint16 wStaticCtrlID;
  uint16 wMenuCtrlID;

  wStaticCtrlID = GetDlgStaticCtlID(wDlgID);
  wMenuCtrlID = GetDlgSkMenuCtlID(wDlgID);

  if ((wStaticCtrlID == 0) || (wMenuCtrlID == 0)) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pIDialog == NULL) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, (int16) wStaticCtrlID);
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) wMenuCtrlID);

  if((pIStaticText == NULL) || (pIMenu == NULL)) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set the title and content */
/*lint -e737*/
/*lint -e574*/
  SetStaticDlgTitle(pMe, wDlgID, title, ARR_SIZE(title));
  SetStaticDlgContent(pMe, wDlgID, psContent, ARR_SIZE(psContent));
/*lint +e737*/
/*lint +e574*/

  ISTATIC_SetText(pIStaticText, title, psContent, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  /* Set the softkey menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  /* Get the newly defined softkey size and use that to set the static size */
  (void) SetStaticTextCtlSize(pIStaticText, (IControl*)pIMenu);
  (void) SetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */

  /* Force the control to redraw */
  return ICONTROL_Redraw((IControl*)pIStaticText);

} /*End InitStaticDlg */

/*===========================================================================
FUNCTION InitBannerDlg

DESCRIPTION
  initialize banner dialog with standard textctl dialog style, NV obtained
  banner letters etc

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitBannerDlg(CCoreApp *pMe, IDialog* pIActiveDialog)
{
  ITextCtl* pIBannerEdit;
  IMenuCtl* pISoftKeyMenu; /* softkey menu */
  AEERect MenuRect;
  AEERect TextRect;
  boolean bReturnStatus;

  if (pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE; /* error */
  }
  pIBannerEdit = (ITextCtl*)IDIALOG_GetControl(pIActiveDialog,
                                                   IDC_TEXT_BANNER_EDIT);

  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                          IDC_SK_BANNER_EDIT);

  if((pIBannerEdit == NULL) || (pISoftKeyMenu == NULL)) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Update the softkey menu */
  bReturnStatus = IMENUCTL_DeleteAll(pISoftKeyMenu);
  if (bReturnStatus == FALSE) {
    CORE_ERR ("Could not delete menu item", 0, 0, 0);
  }

  bReturnStatus = IMENUCTL_AddItem(pISoftKeyMenu, gpszResourceFile, IDS_OK,
            IDL_BANNER_EDIT_OK,
            NULL, 0);
  if (bReturnStatus == FALSE) {
    CORE_ERR ("Could not add menu item", 0, 0, 0);
  }

  ITEXTCTL_SetSoftKeyMenu(pIBannerEdit, NULL);

  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  bReturnStatus = IMENUCTL_Redraw(pISoftKeyMenu);
  if (bReturnStatus == FALSE) {
    CORE_ERR ("Could not redraw menu item", 0, 0, 0);
  }
  /* Set Text control size */
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  ITEXTCTL_GetRect(pIBannerEdit, &TextRect);
  TextRect.dy = MenuRect.y - 1;

  ITEXTCTL_SetRect(pIBannerEdit, &TextRect);

  /* Set textctl properties */
  ITEXTCTL_SetProperties(pIBannerEdit, TP_FRAME);
  ITEXTCTL_SetMaxSize(pIBannerEdit, BANNER_BUFFER_SIZE);
  ITEXTCTL_SetInputMode(pIBannerEdit, AEE_TM_LETTERS);
  /* Get banner text from NV */
  ITEXTCTL_SetText(pIBannerEdit, pMe->m_pszBanner,(int) BANNER_BUFFER_SIZE);
  ITEXTCTL_SetCursorPos (pIBannerEdit, TC_CURSOREND);
  return TRUE;

} /* End InitBannerDlg */



/*===========================================================================
FUNCTION InitTimeDateDlg

DESCRIPTION
  Initialize Time/Date editing dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitTimeDateDlg(CCoreApp *pMe)
{
  AEERect rc;
  IDateCtl* pIDate;
  ITimeCtl* pITime;
  IMenuCtl* pISoftKeyMenu;
  IDialog*  pIActiveDialog;
  JulianType date;
  int msecs;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pIDate = (IDateCtl*)IDIALOG_GetControl(pIActiveDialog, IDC_DATE);
  pITime = (ITimeCtl*)IDIALOG_GetControl(pIActiveDialog , IDC_TIME);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
        IDC_SK_SETTIME_EDIT);

  if ((pIDate == NULL) || (pITime == NULL) || (pISoftKeyMenu == NULL)) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  // Set the rect for date and time
  // Hardcoded values... pretty bad !!!!
  rc.x = 2;
  rc.y = 20;
  rc.dx = (int16) pMe->m_cxScreen-5;
  rc.dy = 60;
  IDATECTL_SetRect(pIDate, &rc);
  rc.y += rc.dy;
  ITIMECTL_SetRect(pITime, &rc);

  // Setup the controls
  (void) GetTimeofDay (pMe,&date);

  IDATECTL_SetDate(pIDate, date.wYear, date.wMonth, date.wDay);
  msecs = ((date.wHour * 60*60)+(date.wMinute*60)+(date.wSecond))*1000;
  ITIMECTL_SetTime(pITime, msecs);


  ITIMECTL_EnableCommand(pITime, TRUE, TIMEDONE);
  IDATECTL_EnableCommand(pIDate, TRUE, DATEDONE);


  IDATECTL_SetActive(pIDate, TRUE);
  ITIMECTL_SetActive(pITime, FALSE);
  IMENUCTL_SetActive(pISoftKeyMenu, FALSE);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_Redraw(pISoftKeyMenu);
  ITIMECTL_Redraw(pITime);
  IDIALOG_SetFocus(pIActiveDialog, IDC_DATE);
  IDATECTL_Redraw(pIDate);
  return TRUE;

}

/*===========================================================================
FUNCTION CreateSecuritySubmenuDlg

DESCRIPTION
  Based on current security menu selection and create the next dialog

DEPENDENCIES
  GetNextSelectedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
static boolean CreateSecuritySubmenuDlg(CCoreApp *pMe, IMenuCtl* pICurrentMenu)
{
  uint16 wItemID;
  uint16 wDlgID;
  int nReturnStatus;

  if(pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);
  /* Get next dialog ID */
  wDlgID = GetNextSelectedItemDlgID(pMe, wItemID);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if (wItemID == IDL_USER_SLOT_PREF) {
    /* do not check the NV */
    DetermineDualSlotState(pMe, &wDlgID, FALSE);
    if (wDlgID == 0) {
      return TRUE;
    }
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  if(wDlgID == 0) { /* error */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
    wDlgID, NULL);

  return CheckAEEReturnStatus(nReturnStatus);

} /* CreateSecuritySubmenuDlg */


/*===========================================================================
FUNCTION InitSecurityDlg

DESCRIPTION
  initialize security menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitSecurityDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;

#ifdef FEATURE_MMGSDI
  AEECardPinStatus pin2_status;    /* PIN2 Status */
  int nReturnStatus;
#endif

  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  if(wDlgID != IDD_SECURITY) { /* error */
    CORE_ERR ("Wrong dlg id", 0,0,0);
    return FALSE;
  }
  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_SECURITY);
  if(pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_MMGSDI
  /* evaluate whether pin2 is enabled or not
    When we get to this point, pin1 and nv pMe members are already
    set appropriately, thus, we will only require pin2 pMe value */
  /* pin2 status is required because we need that to determine whether the
     Change pin option should be displayed to the user or not */
  if (pMe->m_sPIN2_lock_state == AEECARD_PIN_UNKNOWN) {
    nReturnStatus = ICARD_GetPinStatus (pMe->m_pICard, AEECARD_PIN2, &pin2_status);
    if (nReturnStatus != SUCCESS) return FALSE;

    /* update the pMe pin2 enabled value */
    pMe->m_sPIN2_lock_state = pin2_status.lock_state;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* evaluate whether pin2 in slot 2 is enabled or not
    When we get to this point, pin1 and nv pMe members are already
    set appropriately, thus, we will only require pin2 pMe value */
  /* pin2 slot 2 status is required because we need that to determine whether the
     Change pin option should be displayed to the user or not */
  if (pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_UNKNOWN) {
    nReturnStatus = ICARD_GetPinStatus (pMe->m_pICard, AEECARD_PIN2_SLOT2, &pin2_status);
    if (nReturnStatus != SUCCESS) return FALSE;

    /* update the pMe pin2 slot 2 enabled value */
    pMe->m_sPIN2_slot2_lock_state = pin2_status.lock_state;
  }
#endif
#else
  pMe->m_sPIN2_lock_state = AEECARD_PIN_DISABLED;
#endif

  StripFeatureMenuItem(pMe, pICurrentMenu, IDC_MENU_SECURITY);
  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  return TRUE;

} /* End InitSecurityDlg */



/*===========================================================================
FUNCTION StartRestoring

DESCRIPTION
  start the restoring process

DEPENDENCIES
  Various SetNVXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
static void StartRestoring(CCoreApp* pMe, IDialog* pIDialog)
{
  IStatic* pIStaticText;
  AECHAR psDefaultBanner[BANNER_BUFFER_SIZE+1];

  if(pIDialog == NULL) { /* error */
    return;
  }

  /*========= NV items restoring */

  /* silence the sound driver during initialization */
  uisnd_tone_stop();



  /* Restore Sound menu */
  (void) SetRingerVolume(pMe, (uint64)UISND_2ND_VOL);
  (void) SetKeyBeepVolume(pMe, (uint64)UISND_2ND_VOL);
  (void) SetEarpieceVolume(pMe, (uint64)UISND_2ND_VOL);
  (void) SetMessageVolume(pMe, (uint64)UISND_2ND_VOL);
  (void) SetRingerType(pMe, (uint64)UISND_RING_NORMAL);
  (void) SetSmartSound(pMe, (uint64)TRUE);
  (void) SetServiceAlert(pMe, (uint64)TRUE);
  (void) SetSilenceAll(pMe, (uint64)FALSE);
  pMe->m_bMute = FALSE;


  /* Display menu */
  (void) SetBackLightDuration(pMe, (uint64)UI_BKLT_TIME_MAX);
#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
  (void) SetBackLightIntensity(pMe, (uint64)LCD_BACKLIGHT_LVL_DEFAULT);
#endif
  (void) STRTOWSTR("QUALCOMM", psDefaultBanner, sizeof(psDefaultBanner));
  (void) SetBanner(pMe, psDefaultBanner);
  (void) SetAutoHyphen(pMe, (uint64)TRUE);

  /* Extras menu */
  (void) SetAutoAnswer(pMe, (uint64)TRUE);
  (void) SetAutoRetry(pMe, (uint64)3);
  (void) SetOneTouch(pMe, (uint64)TRUE);
  (void) SetPrivacy(pMe, (uint64)TRUE);

  /* format the "Restore Completed message */
  pIStaticText = (IStatic*)IDIALOG_GetControl(pIDialog, IDC_STATIC_RESTORESETTINGSCOMPLETED);
  if(pIStaticText == NULL) { /* error */
    return;
  }

  (void) SetGeneralStaticTextStyle(pIStaticText);

} /* End StartRestoring */


/****************************************************************************

                          Exported functions

****************************************************************************/
/*===========================================================================
FUNCTION WriteSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean WriteSetting(CCoreApp *pMe, uint16 wDlgID, uint16 wItemID)
{
  int i, j;

/*lint -e737*/
/*lint -e574*/
  for (i=0; i < ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (MenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        CORE_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }


      for (j=i; (j < ARR_SIZE(MenuActionTbl)) && (MenuActionTbl[j].wDialogID == wDlgID); j++) {
        if (MenuActionTbl[j].wItemID == wItemID) {
          if (MenuActionTbl[j].setfnPtr == NULL) {
            /* Null Get fn pointer */
            CORE_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
            return FALSE;
          }
          if ((*MenuActionTbl[j].setfnPtr)(pMe, MenuActionTbl[j].nActionData))
            return TRUE;
          return FALSE;
        }
      }
/*lint +e737*/
/*lint +e574*/

      /* No match found */
      CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
      return FALSE;
    }
  }
  /* No match found */
  CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

} /* End WriteSetting */

/*===========================================================================
FUNCTION UpdateCheckBoxSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean UpdateCheckBoxSetting(CCoreApp *pMe, IMenuCtl *pICurrentMenu)
{
  int i, j;

  IDialog* pIActiveDialog = (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  uint16 wDlgID = IDIALOG_GetID(pIActiveDialog);
  uint64 nNewSetting =0;
  boolean setValue = FALSE;
  boolean matchFound = FALSE;
  SetFnPtrType   *setfnPtr = NULL;


/*lint -e737*/
/*lint -e574*/
  for (i=0; i < ARR_SIZE(MenuActionTbl); i++) {
    if (MenuActionTbl[i].wDialogID == wDlgID) {
      /* got a match */
      if (MenuActionTbl[i].wActionType != MENU_SETTING) {
        /* Not a menu setting - here in error */
        CORE_ERR ("Invalid action for dialog %d", wDlgID, 0, 0);
        return FALSE;
      }
      for (j=i; (j < ARR_SIZE(MenuActionTbl)) && (MenuActionTbl[j].wDialogID == wDlgID); j++) {

        if (MenuActionTbl[j].setfnPtr == NULL) {
          /* Null Get fn pointer */
          CORE_ERR ("Invalid function pointer %d", wDlgID, 0, 0);
          return FALSE;
        }
        /*If Check box is set, then unset it*/
        setValue = GetCheckBoxVal(pICurrentMenu, MenuActionTbl[j].wItemID);
        if(setValue)
        {
           nNewSetting |= MenuActionTbl[j].nActionData;
        }

        setfnPtr = MenuActionTbl[j].setfnPtr;
        matchFound = TRUE;
      }
      if(matchFound)
         break;
    }
  }
/*lint +e737*/
/*lint +e574*/

  if(matchFound)
  {
    return ((setfnPtr)(pMe, nNewSetting));
  }

  /* No match found */
  CORE_ERR ("No match for  dialog %d", wDlgID, 0, 0);
  return FALSE;

}/* End UpdateCheckBoxSetting */

/*===========================================================================
FUNCTION CreateIdleMenuDlg

DESCRIPTION
  create the idle menu dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateIdleMenuDlg(CCoreApp *pMe)
{
  int nReturnStatus;

  if(pMe->m_bLPM)
  {
    return TRUE; // Do not display the Idle Menu Dlg at this point.
  }

  if(pMe->m_pSplashImage != NULL) { /* Create Idle Screen */
      (void) HideSplash(pMe);
  }

  /* Create idle dialog */
  MSG_HIGH("ISHELL_CreateDialog start", 0, 0, 0);
  //outpw (TIMETEST_PORT, ISHELL_CreateDialog_in_CreateIdleMenuDlg);
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                      IDD_CORE, NULL);
  DetermineNextCardState(pMe,FALSE);
  //outpw (TIMETEST_PORT, 0x0);
  MSG_HIGH("ISHELL_CreateDialog end", 0, 0, 0);

  MSG_HIGH("UpdateCoreText start", 0, 0, 0);
  UpdateCoreText(pMe);
  MSG_HIGH("UpdateCoreText end", 0, 0, 0);

  return  CheckAEEReturnStatus(nReturnStatus);

} /* End CreateIdleMenuDlg */


/*===========================================================================
FUNCTION CreateLPMDlg

DESCRIPTION
  create the idle menu dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateLPMDlg(CCoreApp *pMe)
{
  int nReturnStatus;
  IDialog *pIActiveDialog;
  uint32 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog != NULL) {
   wDlgID = IDIALOG_GetID(pIActiveDialog);
   if (wDlgID == IDD_LPM) {
    return FALSE;
   }
  }


       /*error checking */

  if(pMe->m_pSplashImage != NULL) { /* Create Idle Screen */
      (void) HideSplash(pMe);
  }

  if ( pMe->m_pIAnn != NULL) {
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, ANNUN_STATE_OFF); /* reserved space */
  #if !defined (FEATURE_WMS_APP)
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG,      ANNUN_STATE_OFF); /* message alert */
  #endif
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_NET,      ANNUN_STATE_OFF); /* network */
#if !defined(FEATURE_ALARMCLOCK_APP)
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ALARM,    ANNUN_STATE_OFF); /* alarm */
#endif
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SECURITY, ANNUN_STATE_OFF); /* security */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MODE,     ANNUN_STATE_OFF); /* mode indicator */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ROAM,     ANNUN_STATE_OFF); /* roam */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_DOMAIN,   ANNUN_STATE_OFF); /* domain */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_RSSI,     ANNUN_STATE_OFF); /* RSSI */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_INUSE,    ANNUN_STATE_OFF); /* in use */
    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_TIME,     ANNUN_STATE_OFF); /* time */
  }

  /* Create idle dialog */
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                IDD_LPM, NULL);

  return  CheckAEEReturnStatus(nReturnStatus);

} /* End CreateIdleMenuDlg */



/*===========================================================================
FUNCTION CreateKeyGuardOnDlg

DESCRIPTION
  Start the sequences of keyguard dialogs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateKeyGuardOnDlg(CCoreApp * pMe)
{
  int nReturnStatus;

  /* Cancel all timers */
  ISHELL_CancelTimer(pMe->a.m_pIShell, 0, pMe);

  /* go back to the idle screen first */
  (void) DisplayIdleMenuDlg(pMe);

  /* Create the "Keyguard is active, press 123 to unlock message dialog */
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                            IDD_KEYGUARD_ON, NULL);
  return CheckAEEReturnStatus(nReturnStatus);

} /* End CreateKeyGuardOnDlg */




/*==========================================================================

                        EVENT HANDLERS

=========================================================================*/

/*===========================================================================
FUNCTION IdleMenuDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IdleMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  int nReturnStatus;
  int dlg;
  boolean bReturnValue;

  if (pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* We are in the idle menu, so cancel the DisplayIdleMenuDlg timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        /*Do not display the "Select operator" Dialog immediately.*/
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SetSelOperatorDisplay, pMe);
#endif
  }

  switch (evt) {

  case EVT_DIALOG_START:
    CORE_MSG ("IdleMenuDlg: EVT_DIALOG_START", 0, 0, 0);

    pMe->m_pIdleDialog =  ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    dlg = IDIALOG_GetID(pMe->m_pIdleDialog);
    if (dlg != IDD_CORE) {
      CORE_ERR("Did not get correct dialog, got %d", dlg, 0, 0);
    }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    /*Do not display the "Select operator" Dialog immediately.*/
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) SetSelOperatorDisplay,
                    (uint32*) pMe);
#endif
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, IDC_ICONV_CORE);
    if(pICurrentMenu) {
      StripFeatureMenuItem(pMe, pICurrentMenu, IDC_ICONV_CORE);
      SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
#ifdef FEATURE_GSTK
      if(!AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM) && pMe->m_pISTK)
      {
         ISTK_IdleScreen(pMe->m_pISTK);
      }
#endif
      return TRUE;
    }
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ----> error */

  case EVT_KEY_RELEASE:
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) SetSelOperatorDisplay,
                    (uint32*) pMe);
#endif
    break;
  case EVT_KEY:
    CORE_MSG ("IdleMenuDlg: EVT_KEY", 0, 0, 0);
    switch(w) {


    case AVK_END:
      /* Switch to Dialer app if we are in a call */
      if(AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM)) {
        return CDialerApp_StartApp(pMe->a.m_pIShell);

      }
#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
#ifdef TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
      // Abort PPP Session if dormant data call is present.
      else if(ds707_extif_pkt_is_dormant())
      {
        CoreUtil_AbortPPPDataSession((void*) pMe);
        return TRUE;
      }
#endif
#endif
      return FALSE;

      /* consume these events without handling */
    case AVK_CLR:
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
      return TRUE;

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
#if defined FEATURE_APP_DIALER
      {
        // Pass digit on to the Dialer App
        char digit  = AVKType2ASCII((AVKType)w);
        return CDialerApp_StartApp_Digit(pMe->a.m_pIShell, (AECHAR)digit);
      }
#endif

      /* fall through if dialer app not defined */
    case AVK_SEND:
#ifdef FEATURE_APP_RECENTCALLS
      ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP,
                              RECENTCALLS_REDIAL_SCREEN_STR);
#endif
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("IdleMenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    switch(w) {

#ifdef FEATURE_APP_CONTACT
    case IDL_CONTACT:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CONTACT_APP);
      return TRUE;
#endif

#ifdef FEATURE_APP_DIALER
    case IDL_DIALER:
      return CDialerApp_StartApp(pMe->a.m_pIShell);
#endif

#ifdef FEATURE_APP_DATASTATS
    case IDL_DATASTATS:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DATASTATS);
      return TRUE;
#endif

#ifdef FEATURE_APP_RECENTCALLS
    case IDL_RECENTCALL:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_RECENTCALLSAPP);
      return TRUE;
#endif

#ifdef FEATURE_WMS_APP
    case IDL_WMS:
        ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_WMSAPP,"START WMS APP");
        return TRUE;
#endif

    case IDL_SETTINGS:
      bReturnValue = CreateMenuSelectedItemDlg(pMe);
      if (bReturnValue == FALSE) {
        CORE_ERR ("Could not create dialog", 0, 0, 0);
      }
      return TRUE;

    case IDL_MY_INFO:
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                            IDD_MYINFO, NULL);
      return CheckAEEReturnStatus(nReturnStatus);

    case IDL_MEDIAPLAYER:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_MEDIAPLAYER);
      return TRUE;

#ifdef FEATURE_APP_WAP
    case IDL_BROWSER:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_TMT_WAP_APP);
      return TRUE;
#endif

    case IDL_APPMANAGER:
#if MIN_BREW_VERSION(3,0)
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_APPMANAGER);
#else
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_APPMGR_BID);
#endif
      return TRUE;

#ifdef FEATURE_GPSAPP
    case IDL_GPS:
      bReturnValue = ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_GPSONE,
                                            gpsApp_StartUpStr);
      return TRUE;
#endif

#ifdef FEATURE_UI_3D_APP
    case IDL_UI3D:
      bReturnValue = ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_UI3D);
      return TRUE;
#endif

#ifdef FEATURE_APP_CATAPP
    case IDL_CATAPP:
      ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_CATAPP,
                                           "Start Cat App");
      return TRUE;
#endif

#ifdef FEATURE_APP_FLDDBG
    case IDL_DEBUG:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_FIELDDEBUGAPP);
      return TRUE;
#endif

#ifdef FEATURE_APP_QCAMERA
    case IDL_QCAMERA:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_QCAMERA);
      return TRUE;
#endif

#ifdef FEATURE_APP_QCAMCORDER
    case IDL_QCAMCORDER:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_QCAMCORDER);
      return TRUE;
#endif

#ifdef FEATURE_QVPHONE_APP
    case IDL_VIDEOPHONE:
      ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_QVIDEOPHONE,"START VT APP");
      return TRUE;
#endif /* FEATURE_QVPHONE_APP */

#ifdef FEATURE_JAVA
    case IDL_JAVA:
      bReturnValue = ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_JAVA_AMS);
      return TRUE;
#endif

#ifdef FEATURE_APP_BLUETOOTH
    case IDL_BLUETOOTH:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP);
      return TRUE;
#endif

#ifdef FEATURE_APP_MMS
    case IDL_MMS:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_TMT_MMS_APP);
      return TRUE;
#endif

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE

    case IDL_IMS_VIDEOSHARE:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_IMS_VIDEOSHARE);
      return TRUE;
#endif
    case IDL_IMS_SETTINGS:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_IMS_SETTINGS);
      return TRUE;

#endif //FEATURE_IMS

#ifdef FEATURE_ALARMCLOCK_APP
    case IDL_ALARMCLOCK:
      ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_ALARMCLOCK);
      return  TRUE;
#endif

#ifdef FEATURE_FMRADIO_APP
    case IDL_FMRADIO:
    {
       ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_FMRADIOAPP);
       return TRUE;
    }
#endif /* FEATURE_FMRADIO_APP */

    default: // other apps will be added to this later!!!!
      // For now display Feature not available
      DisplayMessageDlg(pMe, IDS_FEATURE_UNAVAILABLE, 3);
      return TRUE;
    }

  case EVT_DIALOG_END:
    CORE_MSG ("IdleMenuDlg: EVT_DIALOG_END", 0, 0, 0);
    pMe->m_pIdleDialog = NULL;
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
  //The compiler wants this, lint doesn't.
  return FALSE; //lint !e527
} /* End IdleMenuDlgEventHandler */

/*===========================================================================
FUNCTION MenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean MenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID, wDlgID;
  int nReturnStatus;
  boolean bReturnValue;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {

  case EVT_DIALOG_START:
    CORE_MSG ("MenuDlg: EVT_DIALOG_START", 0, 0, 0);
    wCtrlID = GetDlgMenuCtlID(pMe, (IDialog*)dw);
    if(wCtrlID == 0) {
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, (int16) wCtrlID);
    if(pICurrentMenu) {
      /* remove certain items based on featurization */
      StripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
      SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      return TRUE;
    }
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ---> error */

  case EVT_KEY:
    CORE_MSG ("MenuDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* Digit keys should be ignored */

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("MenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

#ifdef FEATURE_NETSETTINGS_APP
    if(w == IDL_NETWORK)
      {
        ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_NETSETTINGS);
        return TRUE;
      }
#endif

#ifdef FEATURE_AUXSETTINGS_APP
    if(w == IDL_SIOCONFIGURATION)
      {
        ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_AUXSETTINGS, 
                               AUX_StartSIOCONFIGURATIONS);
        return TRUE;
      }
#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
    if(w == IDL_USBDEVICESWITCH)
      {
        ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_AUXSETTINGS, 
                               AUX_StartUSBDEVSW);
        return TRUE;
      }
#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */
    if(w == IDL_SETSECURETIME)
      {
        ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_AUXSETTINGS, 
                               AUX_StartSECTIME);
        return TRUE;
      }
#endif

    if (w == IDL_MUTE) { /* mute menu, check for whether we are in call,
                            if not in call, display a "Feature Avaliable During Call only" */
      if (!ui_is_in_call()) { /* not in call */
        DisplayMessageDlg(pMe, IDS_FEATURE_AVAILABLE_IN_CALL, 3);
        return TRUE;
      }
      // Maybe want to redo the Mute item... which require coordination
      // between Call screens dialog and settings
      // dialog for Mute!!!
    }
    if( w == IDL_MMVOL)
    {
      pMe->m_bMMVolumeSetting = TRUE;
    }

    bReturnValue = CreateMenuSelectedItemDlg(pMe);
    if (bReturnValue == FALSE) {
      CORE_ERR ("Could not create dialog", 0, 0, 0);
    }
    return TRUE;

  case EVT_USER:
    CORE_MSG ("MenuDlg: EVT_USER", 0, 0, 0);
    switch (w) {
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                IDD_SECURITY, NULL);

      return CheckAEEReturnStatus(nReturnStatus);

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    wDlgID = IDIALOG_GetID((IDialog*)dw);
    if (wDlgID != IDD_SETTINGS)
    {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                     COREAPPTIMEOUT,
                     (PFNNOTIFY) DisplayIdleMenuDlg,
                     (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    if(pMe->m_nNWSelectionMode != APPCOMFUNC_NW_SEL_INACTIVE)
    {
      pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
      FREEIF(pMe->m_pDetailedPLMNList);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    break;
  }

  return FALSE;
} /* End MenuDlgEventHandler */

/*===========================================================================
FUNCTION RadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean RadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("RadioButnDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("RadioButnDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("RadioButnDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    wCtrlID = GetDlgMenuCtlID(pMe, pIDialog);
    if(wCtrlID == 0) {
      CORE_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }
    pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
    if(pICurrentMenu == NULL) {
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    if (UpdateSetting(pMe, w, pICurrentMenu)) {
      /* Let the update image be shown for 2 seconds before going back to
      the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                       SELECTIONVERIFICATIONTIME,
                       (PFNNOTIFY) EndCurrentDlg,
                       (uint32*) pMe);
     (void)CheckAEEReturnStatus(nReturnStatus);
    }
    else {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = LookupSystemPrefMsgString(wDlgID);
      DisplayMessageDlg(pMe, wStringID, 3);
    }
    return TRUE;

  case EVT_USER:
    CORE_MSG ("RadioButtonDlg: EVT_USER", 0, 0, 0);
    /* user slot preference setting failure */
    switch(w) {
    case EVT_USER_SLOT_PREF_FAIL:
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      return TRUE;
    case EVT_USER_SLOT_PREF_SUCCESS:
      DisplayMessageDlg(pMe, IDS_CHANGE_SUCCESS, 3);
      return TRUE;

    default:
      break;
    }

    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("RadioButnDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End RadioButtonDlgEventHandler */

/*===========================================================================
FUNCTION CheckBoxDlgEventHandler

DESCRIPTION
  Handle check box event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CheckBoxDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pIMenu;
  IDialog *pIDialog;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) GetDlgMenuCtlID(pMe, pIDialog));


  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("CheckBoxDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitCheckBoxDlg(pMe, w, (IDialog*)dw);



  case EVT_KEY:
    CORE_MSG ("CheckBoxDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("CheckBoxDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

    if(IMENUCTL_IsActive(pIMenu))// Sk Is Not Active
    {
       uint16 wItemID = IMENUCTL_GetSel(pIMenu);
       boolean setBox = !GetCheckBoxVal(pIMenu, wItemID);
       SetCheckBoxItem (pIMenu, wItemID, setBox);
       return TRUE; // Do nothing
    }

    /*Sk Is Active*/
    if (UpdateCheckBoxSetting(pMe, pIMenu)) {
      /* Let the update image be shown for 2 seconds before going back to
      the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                       SELECTIONVERIFICATIONTIME,
                       (PFNNOTIFY) EndCurrentDlg,
                       (uint32*) pMe);
     (void)CheckAEEReturnStatus(nReturnStatus);
    }
    else {
      IDialog* pIActiveDialog;
      uint16 wDlgID;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
    }
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("CheckBoxDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End CheckBoxDlgEventHandler */


/*===========================================================================
FUNCTION RestrictCheckBoxDlgEventHandler

DESCRIPTION
  Handle Restrict menu check box event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean RestrictCheckBoxDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pIMenu;
  IDialog *pIDialog;
  uint32 nNewValue;
  boolean setValue;
  uint16 wItemID;
  boolean setBox;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) GetDlgMenuCtlID(pMe, pIDialog));


  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("RestrictCheckBoxDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitCheckBoxDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("RestrictCheckBoxDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("RestrictCheckBoxDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

    /* Get the previous setting */
    setValue = GetCheckBoxVal(pIMenu, w);

    switch (w) {
    case IDL_RESTRICT_EFS:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_EFS;
      }
      else {
        nNewValue = COREUTILS_EFS;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    case IDL_RESTRICT_SPEED:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_SPEED_DIAL;
      }
      else {
        nNewValue = COREUTILS_SPEED_DIAL;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    case IDL_RESTRICT_ADN_SLOT1:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_ADN_SLOT1;
      }
      else {
        nNewValue = COREUTILS_ADN_SLOT1;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    case IDL_RESTRICT_ADN_SLOT2:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_ADN_SLOT2;
      }
      else {
        nNewValue = COREUTILS_ADN_SLOT2;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    case IDL_RESTRICT_FDN_SLOT1:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_FDN_SLOT1;
      }
      else {
        nNewValue = COREUTILS_FDN_SLOT1;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    case IDL_RESTRICT_FDN_SLOT2:
      if(setValue) { /* need to change the value to unset */
        nNewValue = ~COREUTILS_FDN_SLOT2;
      }
      else {
        nNewValue = COREUTILS_FDN_SLOT2;
      }
      (void) SetRestrict(pMe, (uint64)nNewValue);
      return TRUE;

    default:
      CORE_ERR("Invalid selection", 0, 0, 0);
      break;
    }
    return FALSE;

  case EVT_USER:
    CORE_MSG ("RestrictCheckBoxDlg: EVT_USER", 0, 0, 0);
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIDialog == NULL) {
      return FALSE;
    }
    pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_RESTRICT);
    if (pIMenu == NULL) {
      CORE_ERR("Null dialog ptr", 0, 0, 0);
      return FALSE;
    }
    /* get pin2 verification success => need to call enable/disable FDN function */
    switch(w) {
#ifdef FEATURE_MMGSDI
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
      if ((dw == AEECARD_PIN2_SLOT2) || (dw == AEECARD_PIN2)) {
        wItemID = IMENUCTL_GetSel(pIMenu);
        /* Create hour glass dialog */
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                  IDD_HOURGLASS, NULL);
        (void) CheckAEEReturnStatus(nReturnStatus);
        switch (wItemID) {
        case IDL_RESTRICT_FDN_SLOT1:
          setBox = GetCheckBoxVal(pIMenu, wItemID);
          if (setBox) {
            /* need to disable it now */
            /* Populate the m_sCallback structure */
            pMe->m_sCallback.pfnCancel = NULL;
            pMe->m_sCallback.pfnNotify = DisableFDN_cb;
            pMe->m_sCallback.pNotifyData = pMe;
            ICARD_DisableFDNSlot(pMe->m_pICard, AEECARD_SLOT1, FALSE,
                                 NULL, &sFDNActionStatus, &pMe->m_sCallback);
          }
          else {
            /* need to enable it now */
            /* Populate the m_sCallback structure */
            pMe->m_sCallback.pfnCancel = NULL;
            pMe->m_sCallback.pfnNotify = EnableFDN_cb;
            pMe->m_sCallback.pNotifyData = pMe;
            ICARD_EnableFDNSlot(pMe->m_pICard, AEECARD_SLOT1, FALSE,
                                NULL, &sFDNActionStatus, &pMe->m_sCallback);
          }
          break;

        case IDL_RESTRICT_FDN_SLOT2:
          setBox = GetCheckBoxVal(pIMenu, wItemID);
          if (setBox) {
            /* need to disable it now */
            /* Populate the m_sCallback structure */
            pMe->m_sCallback.pfnCancel = NULL;
            pMe->m_sCallback.pfnNotify = DisableFDN_cb;
            pMe->m_sCallback.pNotifyData = pMe;
            ICARD_DisableFDNSlot(pMe->m_pICard, AEECARD_SLOT2, FALSE,
                                 NULL, &sFDNActionStatus, &pMe->m_sCallback);
          }
          else {
            /* need to enable it now */
            /* Populate the m_sCallback structure */
            pMe->m_sCallback.pfnCancel = NULL;
            pMe->m_sCallback.pfnNotify = EnableFDN_cb;
            pMe->m_sCallback.pNotifyData = pMe;
            ICARD_EnableFDNSlot(pMe->m_pICard, AEECARD_SLOT2, FALSE,
                                NULL, &sFDNActionStatus, &pMe->m_sCallback);
          }
          break;
        default:
          /* remove the hour glass dialog */
          ISHELL_EndDialog(pMe->a.m_pIShell);
          break;
        }
        return TRUE;
      }
      return FALSE; /* get a pin success for PIN1 */
#endif /*FEATURE_MMGSDI */

    case EVT_RESTRICT_UPDATE_FDN_FAIL:
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      return TRUE;

    case EVT_RESTRICT_UPDATE_FDN_SUCCESS:
      return SetNVRestrict(pMe);


    case EVT_RESTRICT_UPDATE_SUCCESS: /* update the screen */
      wItemID = IMENUCTL_GetSel(pIMenu);
      setBox = !GetCheckBoxVal(pIMenu, wItemID);
      SetCheckBoxItem (pIMenu, wItemID, setBox);
      /* Let the update image be shown for 2 seconds before going back to
      the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                       SELECTIONVERIFICATIONTIME,
                       (PFNNOTIFY) EndCurrentDlg,
                       (uint32*) pMe);
     (void)CheckAEEReturnStatus(nReturnStatus);
      return CheckAEEReturnStatus(nReturnStatus);

    case EVT_RESTRICT_UPDATE_FAIL:
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      return TRUE;

    default:
      return FALSE;
    }


  case EVT_DIALOG_END:
    CORE_MSG ("CheckBoxDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End RestrictCheckBoxDlgEventHandler */


/*===========================================================================
FUNCTION ContrastDlgEventHandler

DESCRIPTION
  Handle events for contrast dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ContrastDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("ContrastDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitContrastDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("ContrastDlg: EVT_KEY %d", w, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);


    case AVK_SELECT:
      //don't handle this as autorepeat (came in with select pressed)
      if((dw & KB_AUTOREPEAT) != 0) {
        return FALSE;
      }

      // save value
      if (!SetContrast(pMe, (byte)pMe->m_byDispContrast)) {
        MSG_ERROR("Unable to set contrast",0,0,0);
      }

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

    case AVK_UP:
      UpdateContrast(pMe, CONTRAST_STEP);
      return TRUE;
    case AVK_DOWN:
      UpdateContrast(pMe, -CONTRAST_STEP);
      return TRUE;

    default:
          return TRUE; /* ignore all other keys */
    }

  case EVT_DIALOG_END:
    CORE_MSG ("ContrastDlg: EVT_DIALOG_END", 0, 0, 0);
    // Change contrast to value in NV
    RestoreStoredContrast(pMe);

    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End ContrastDlgEventHandler */


/*===========================================================================
   This function saves the file name and sets the media data locally.
   Assumption: pmd is not NULL.
===========================================================================*/
static boolean MM_Ringer_SetMediaData(CCoreApp * pme, AEEMediaData * pmd)
{
  if (!pmd->pData) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (pme->m_pszMMRingerFile == NULL)
    pme->m_pszMMRingerFile = (char *) MALLOC(sizeof(char)* AEE_MAX_FILE_NAME);

  (void) MEMCPY (pme->m_pszMMRingerFile, (char *)pmd->pData, sizeof (char) * AEE_MAX_FILE_NAME);
  (void) MEMCPY(&pme->m_sMd, pmd, sizeof(AEEMediaData));

   return TRUE;
}

/*===========================================================================
   This function opens the player window with the specified file.
   It also tries to play the file if MediaPlayer is in plugin mode.

   Note: This function allocates memory for the file name and the memory
   is released by player window's destructor.
===========================================================================*/
static void MM_Ringer_PlayFile(CCoreApp * pme, const char * pszFile)
{
   AEEMediaData   md;
   char *         pszBuf;

   pszBuf = STRDUP(pszFile);
   if (!pszBuf)
      return;

   md.clsData = MMD_FILE_NAME;
   md.pData = (void *)pszBuf;
   md.dwSize = 0;

   (void) MM_Ringer_SetMediaData(pme, &md);

}

/*===========================================================================
FUNCTION SoundRadioButtonDlgEventHandler

DESCRIPTION
  handle sound radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SoundRadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IDialog * pIDialog;
  uint16 wDlgID;
  int nReturnStatus;
  IMenuCtl* pICurrentMenu;
  uint16    wCtrlID = 0;


  CCoreApp * pMe = (CCoreApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
    case EVT_DIALOG_START:
      CORE_MSG ("SndRadioBDlg: EVT_DIALOG_START", 0, 0, 0);
      pMe->m_bDemoAlert = TRUE;
      CoreTask_SetDemoAlert(pMe->m_bDemoAlert);
      return InitSoundRadioButtonDlg(pMe, w, (IDialog*)dw);

    case EVT_KEY:
      CORE_MSG ("SndRadioBDlg: EVT_KEY", 0, 0, 0);
      switch(w) {
        case AVK_CLR:
          /* disable the timer that will bring the user to the previous screen */
          ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          return CheckAEEReturnStatus(nReturnStatus);

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
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_END:
        case AVK_INFO:  /* power down */
          /* disable the timer that will bring the user to the previous screen */
          ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
          return FALSE;

        default:
          return FALSE; /* ignore other keys */
      }

    case EVT_CTL_SEL_CHANGED: /* when moving between different sound selection */
      CORE_MSG ("SndRadioBDlg: EVT_CTL_SEL_CHANGED", 0, 0, 0);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) { /* ERROR */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      if (!pMe->m_bMMRinger && !pMe->m_bMMVolumeSetting) {
        PlaySampleSound(pMe, wDlgID, w);
      }
      else {
        if(pMe->m_pIMedia) {
          IMEDIA_Stop(pMe->m_pIMedia);
          IMEDIA_Release(pMe->m_pIMedia);
          pMe->m_pIMedia = NULL;
        }
        /* MM ringer sample */
        MM_Ringer_PlayFile(pMe,pMe->m_pszMMRingerFile);
        nReturnStatus = AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &pMe->m_sMd, &pMe->m_pIMedia);
        if (nReturnStatus == SUCCESS)
        {
#ifdef FEATURE_MULTIMEDIA
          uisnd_mm_vol ((byte) LookupValue(wDlgID, w),LookupSoundDlgNVEnum(pMe, wDlgID));
#endif
          nReturnStatus = IMEDIA_Play(pMe->m_pIMedia);
          (void)CheckAEEReturnStatus(nReturnStatus);
        }
        else
        {
          /* file is missing - restore to default */
          pMe->m_bMMRinger=FALSE;
          if (pMe->m_pszMMRingerFile) {
            FREE(pMe->m_pszMMRingerFile);
            pMe->m_pszMMRingerFile = NULL;
            (void) SetRingerType (pMe, (uint64)UISND_RING_NORMAL);
          }
          return FALSE;
        }
      }
      return TRUE; /* digital keys shall be ignored */


    case EVT_COMMAND:
      CORE_MSG ("SoundRadioButtonDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* user makes (new) selection */
      /* disable the timer that shows users their newly selected option */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if(pIDialog == NULL) {
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }

        wCtrlID = GetDlgMenuCtlID(pMe, pIDialog);
        if(wCtrlID == 0) {
          CORE_ERR ("Null pointer ", 0,0,0);
          return FALSE;
        }
        pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, wCtrlID);
        if(pICurrentMenu == NULL) {
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }
      if (UpdateSetting(pMe, w, pICurrentMenu)) {
        /* Let the update image be shown for 2 seconds before going back to
        the previous screen */
        nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                        SELECTIONVERIFICATIONTIME,
                        (PFNNOTIFY) EndCurrentDlg,
                        (uint32*) pMe);
        (void)CheckAEEReturnStatus(nReturnStatus);
      }
      else {
        DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      }
      return TRUE;

    case EVT_USER:
      CORE_MSG ("SndRadioBDlg: EVT_USER", 0, 0, 0);
      if (w == EVT_RING_COMPLETE) { /* Ring complete */
        uisnd_snd_stop();
        return TRUE;
      }
      return FALSE;

    case EVT_DIALOG_END:
      CORE_MSG ("SndRadioBDlg: EVT_DIALOG_END", 0, 0, 0);
      uisnd_snd_stop();
      if (pMe->m_pIMedia) {
        IMEDIA_Stop (pMe->m_pIMedia);
        IMEDIA_Release(pMe->m_pIMedia);
        pMe->m_pIMedia = NULL;
      }
      pMe->m_bDemoAlert = FALSE;
      CoreTask_SetDemoAlert(pMe->m_bDemoAlert);
      pMe->m_bMMVolumeSetting = FALSE;
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      COREAPPTIMEOUT,
                      (PFNNOTIFY) DisplayIdleMenuDlg,
                      (uint32*) pMe);

      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End SoundRadioButtonDlgEventHandler */



/*===========================================================================
FUNCTION SoundRadioButtonRingTypeDlgEventHandler

DESCRIPTION
  handle ringer radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SoundRadioButtonRingTypeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IDialog * pIDialog;
  uint16 wDlgID;
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
    case EVT_DIALOG_START:
      CORE_MSG ("SndRingTypeDlg: EVT_DIALOG_START", 0, 0, 0);
      pMe->m_bDemoAlert = TRUE;
      CoreTask_SetDemoAlert(pMe->m_bDemoAlert);
      return InitSoundRingerButtonDlg(pMe, w, (IDialog*)dw);

    case EVT_KEY:
      CORE_MSG ("SndRingTypeDlg: EVT_KEY", 0, 0, 0);
      switch(w) {
        case AVK_CLR:
          /* disable the timer that will bring the user to the previous screen */
          ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          return CheckAEEReturnStatus(nReturnStatus);

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
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_END:
        case AVK_INFO:  /* power down */
          /* disable the timer that will bring the user to the previous screen */
          ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
          return FALSE;

        default:
          return FALSE; /* ignore other keys */
      }

   case EVT_CTL_SEL_CHANGED: /* when moving between different sound selection */
     CORE_MSG ("SndRingTypeDlg: EVT_CTL_SEL_CHANGED", 0, 0, 0);
     pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
     if(pIDialog == NULL) { /* ERROR */
       CORE_ERR ("Null pointer", 0,0,0);
       return FALSE;
     }
     wDlgID = IDIALOG_GetID(pIDialog);
     if (wDlgID == 0) {
       CORE_ERR ("Null pointer", 0,0,0);
       return FALSE;
     }
     if(pMe->m_pIMedia) {
       IMEDIA_Stop(pMe->m_pIMedia);
       IMEDIA_Release(pMe->m_pIMedia);
       pMe->m_pIMedia = NULL;
     }
     if (w<= IDL_RINGTYPE_4 && w >= IDL_RINGTYPE_STANDARD)
     {
       PlaySampleSound(pMe, wDlgID, w);
       pMe->m_bMMRinger = FALSE;
     }
     else
     {
       MM_Ringer_PlayFile(pMe,pMe->m_pszFileArray[(int)GetRingerValue(w)]);
       nReturnStatus = AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &pMe->m_sMd, &pMe->m_pIMedia);
       pMe->m_bMMRinger = TRUE;
       if (nReturnStatus == SUCCESS)
       {
         nReturnStatus = IMEDIA_Play(pMe->m_pIMedia);
         pMe->m_bMMRinger = TRUE;
         (void)CheckAEEReturnStatus(nReturnStatus);
         pMe->m_bMMRinger = TRUE;
       }
       else
       {
         pMe->m_bMMRinger=FALSE;
         return FALSE;
       }
     }
     return TRUE; /* digital keys shall be ignored */

   case EVT_COMMAND:
     CORE_MSG ("SoundRadioButtonRingTypeDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* user makes (new) selection */
      /* disable the timer that shows users their newly selected option */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wCtrlID = GetDlgMenuCtlID(pMe, pIDialog);
      if(wCtrlID == 0) {
        CORE_ERR ("Null pointer ", 0,0,0);
        return FALSE;
      }
      pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, wCtrlID);
      if(pICurrentMenu == NULL) {
         CORE_ERR ("Null pointer", 0,0,0);
         return FALSE;
      }

      if (UpdateSetting(pMe, w, pICurrentMenu)) {
        /* Let the update image be shown for 2 seconds before going back to
        the previous screen */
        nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                        SELECTIONVERIFICATIONTIME,
                        (PFNNOTIFY) EndCurrentDlg,
                        (uint32*) pMe);
        (void)CheckAEEReturnStatus(nReturnStatus);
      }
      else {
        DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      }
      return TRUE;

    case EVT_USER:
      CORE_MSG ("SndRingTypeDlg: EVT_USER", 0, 0, 0);
      if (w == EVT_RING_COMPLETE) { /* Ring complete */
        uisnd_snd_stop();
        return TRUE;
      }
      return FALSE;

    case EVT_DIALOG_END:
      CORE_MSG ("SndRingTypeDlg: EVT_DIALOG_END", 0, 0, 0);
      uisnd_snd_stop();
      pMe->m_bDemoAlert = FALSE;
      if (pMe->m_pIMedia)
      {
        IMEDIA_Stop(pMe->m_pIMedia);
        IMEDIA_Release(pMe->m_pIMedia);
        pMe->m_pIMedia = NULL;
      }
      CoreTask_SetDemoAlert(pMe->m_bDemoAlert);
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      COREAPPTIMEOUT,
                      (PFNNOTIFY) DisplayIdleMenuDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End SoundRadioButtonRingDlgEventHandler */


/*===========================================================================
FUNCTION StaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  InitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean StaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }


  switch(evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("StaticDlg: EVT_DIALOG_START", 0, 0, 0);
      return InitStaticDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("StaticDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("StaticDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if(w == IDL_BANNER_VIEW_EDIT || w == IDL_SETTIME_VIEW_SET) {
      return CreateTextEditDlg(pMe); /* go to next text editing dialog */
    }
    else if (w == IDL_BANNER_VIEW_OK
      || w == IDL_SETTIME_VIEW_OK
      || w == IDL_MYINFO_OK
      || w == IDL_RESTORESETTINGS_CANCEL ) {
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);
      /* user is satisfied with the current setting, end the dialog */
    }
    else if (w == IDL_RESTORESETTINGS_OK) { /* start restoring setting dlg */
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
              IDD_RESTORE_SETTINGS_NOW, NULL);
      return CheckAEEReturnStatus(nReturnStatus);
    }

    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("StaticDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End StaticDlgEventHandler */

/*===========================================================================
FUNCTION BannerDlgEventHandler

DESCRIPTION
  handle banner editing dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean BannerDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IDialog* pIActiveDialog;
  ITextCtl* pIBannerEdit;
  AECHAR wBuf[BANNER_BUFFER_SIZE+1];
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);

  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("BannerDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitBannerDlg(pMe, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("BannerDlg: EVT_KEY", 0, 0, 0);
    if (w != AVK_SELECT) {
      switch(w) {
      case AVK_CLR:
        PopDlg (pMe, IDD_DISPLAY);
        return TRUE;

      case AVK_DOWN:
      case AVK_UP:
        pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if (pIActiveDialog == NULL) {
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }
        if (w == AVK_DOWN) {
          IDIALOG_SetFocus(pIActiveDialog, IDC_SK_BANNER_EDIT);
        }
        else {
          IDIALOG_SetFocus(pIActiveDialog, IDC_TEXT_BANNER_EDIT);
        }
        return TRUE;

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
      case AVK_LEFT:
      case AVK_RIGHT:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* event handled */

      default:
        return FALSE;
      }
    }


    /* fall through for AVK_SELECT */

  case EVT_COMMAND:
    CORE_MSG ("BannerDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if((w == IDL_BANNER_EDIT_OK) || (w == AVK_SELECT)) { /* OK pressed */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIActiveDialog == NULL) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      pIBannerEdit = (ITextCtl*)IDIALOG_GetControl(pIActiveDialog,
                                                   IDC_TEXT_BANNER_EDIT);
      if(pIBannerEdit == NULL) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      /* Get the Text from Text box and save to NV
      */
      ITEXTCTL_GetText(pIBannerEdit, wBuf, sizeof(wBuf));
      (void) SetBanner(pMe, wBuf);
      PopDlg (pMe, IDD_DISPLAY);
      return TRUE;
    }
    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("BannerDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End BannerDlgEventHandler */



/*===========================================================================
FUNCTION TimeDateDlgEventHandler

DESCRIPTION
  time date edit dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean TimeDateDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
/*lint -esym(715,dw)*/
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IDateCtl* pIDate;
  ITimeCtl* pITime;
  int nReturnStatus;
  IDialog*  pIActiveDialog;

  if ( pMe == NULL ) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, 0, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
            COREAPPTIMEOUT,
        (PFNNOTIFY) DisplayIdleMenuDlg,
        (uint32*) pMe);

    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    return InitTimeDateDlg(pMe);

  case EVT_KEY:
    switch(w) {
    case AVK_CLR:
      PopDlg (pMe, IDD_SETTIME_VIEW);
      return TRUE;

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* ignored others */
    }

  case EVT_COMMAND:
    CORE_MSG ("TimeDateDlgEventHandler:EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIActiveDialog == NULL) {
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    pIDate = (IDateCtl*)IDIALOG_GetControl(pIActiveDialog, IDC_DATE);
    pITime = (ITimeCtl*)IDIALOG_GetControl(pIActiveDialog , IDC_TIME);
    if ((pIDate == NULL) || (pITime == NULL)) {
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    switch(w) {
      case DATEDONE:
        /* Switch focus to time control */
        IDATECTL_SetActive(pIDate, FALSE);
        ITIMECTL_SetActive(pITime, TRUE);
        IDATECTL_Redraw(pIDate);
        IDIALOG_SetFocus(pIActiveDialog, IDC_TIME);
        ITIMECTL_Redraw(pITime);
        return TRUE;

      case TIMEDONE:
        /* Switch focus to softkey */
        IDATECTL_SetActive(pIDate, FALSE);
        ITIMECTL_SetActive(pITime, FALSE);
        ITIMECTL_Redraw(pITime);
        IDATECTL_Redraw(pIDate);
        IDIALOG_SetFocus(pIActiveDialog, IDC_SK_SETTIME_EDIT);
        return TRUE;

      case IDL_SETTIME_EDIT_OK:
        /* Save time and date */
        {
          JulianType date;
          int minutes;
          int year, month, day;

          IDATECTL_GetDate(pIDate, &year, &month, &day);
          date.wYear = (uint16) year;
          date.wMonth = (uint16) month;
          date.wDay = (uint16) day;
          date.wWeekDay = IDATECTL_GetDayOfWeek(pIDate);

          minutes = (ITIMECTL_GetTime(pITime))/(60*1000);
          if (minutes > 0) {
            date.wHour = (unsigned short) minutes/60;
            date.wMinute = (unsigned short) minutes%60;
          }
          date.wSecond = 0;
          (void) SetTimeofDay(pMe, &date);
          PopDlg (pMe, IDD_DISPLAY);
          return TRUE;
        }

      default:
        break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    return TRUE;

  default: // Other events that have not been handled by the handler
    return FALSE;
  }
}



/*===========================================================================
FUNCTION KeyGuardDlgEventHandler

DESCRIPTION
  handle keyguard radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean KeyGuardDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {

    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("KeyGuardDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("KeyGuardDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE; /* passed to app's event handler */
    }

  case EVT_COMMAND:
    CORE_MSG ("KeyGuardDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes a (new) selection, disable display timer and keyguard creation timer */
    /* disable the timer that shows users their newly selected option */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
    /* disable the timer associated with starting the keyguard function */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CreateKeyGuardOnDlg, pMe);

    (void) UpdateSetting(pMe, w, (IMenuCtl*)dw);
    if (pMe->m_byKeyguard != 0) {
      /* Let the update image be shown for 2 seconds before going back to the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      SELECTIONVERIFICATIONTIME,
                      (PFNNOTIFY) EndCurrentDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("KeyGuardDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End KeyGuardDlgEventHandler */



/*===========================================================================
FUNCTION KeyGuardOnDlgEventHandler

DESCRIPTION
  handle keyguard ON dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean KeyGuardOnDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IStatic * pIStatic;
  int nReturnStatus;

  if (pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset all timers, there will not be any kind of
     time out when the keyguard is active
  */
  if (evt == EVT_DIALOG_START) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, 0, pMe);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("KeyGuardOnDlg: EVT_DIALOG_START", 0, 0, 0);
    pIStatic = (IStatic*) IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_KEYGUARD_ON);
    (void) SetGeneralStaticTextStyle(pIStatic);
    pMe->m_nKeyguardUnlockCodePos = 0;
    return TRUE;

  case EVT_KEY:
    CORE_MSG ("KeyGuardOnDlg: EVT_KEY", 0, 0, 0);
    /* determine if the keyguard should be deactivated */
    if (w == AVK_1 || w == AVK_2 || w == AVK_3) {
      switch(pMe->m_nKeyguardUnlockCodePos) {
      case 0:
        if (w == AVK_1) {
          pMe->m_nKeyguardUnlockCodePos = 1;
        }
        else {
          pMe->m_nKeyguardUnlockCodePos = 0;
        }
        break;
      case 1:
        if (w == AVK_2) {
          pMe->m_nKeyguardUnlockCodePos = 2;
        }
        else {
          pMe->m_nKeyguardUnlockCodePos = 0;
        }
        break;
      case 2:
        pMe->m_nKeyguardUnlockCodePos = 0;
        if (w == AVK_3) {
          /* Unlock successfully
          */
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          return CheckAEEReturnStatus(nReturnStatus);
        }
        break;

      default:
        break;
      }
      return TRUE;
    }
    else { /* display keyguard unlock method message */

      switch(w) {
        /* digit keys */
        case AVK_0:
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
        /* clear key */
        case AVK_CLR:
        /* end key */
        case AVK_END:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          pMe->m_nKeyguardUnlockCodePos = 0;
          return TRUE; /* other key shall be ignored! */

        default:
          return FALSE; /* pass to core app to handle */
      }
    }

  case EVT_COMMAND:
    CORE_MSG ("KeyGuardOnDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("KeyGuardOnDlg: EVT_DIALOG_END", 0, 0, 0);
    pMe->m_nKeyguardUnlockCodePos = 0;
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End KeyGuardOnDlgEventHandler */


/*===========================================================================
FUNCTION SecurityDlgEventHandler

DESCRIPTION
  handle security menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SecurityDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  IDialog *pIActiveDialog;

  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {

  case EVT_DIALOG_START:
    CORE_MSG ("SecurityDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitSecurityDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("SecurityDlg: EVT_KEY", 0, 0, 0);
    switch(w){
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* pass to core app to handle */
    }

  case EVT_COMMAND:
    CORE_MSG ("SecurityDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    /* Create security submenu */
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIActiveDialog == NULL) { /* error */
      return FALSE;
    }

    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_SECURITY);
    if (pICurrentMenu == NULL) { /* error */
      return FALSE;
    }

    (void) CreateSecuritySubmenuDlg(pMe, pICurrentMenu);
    return TRUE;

  case EVT_USER:
    CORE_MSG ("SecurityDlg: EVT_USER", 0, 0, 0);
    /* will not received EVT_SECURITY_SUCCESS / FAIL since we already pass the
    enter pin1 screen in order to get to this security menu dialog*/
    switch(w) {
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS: /* Create restrict menu */
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                          IDD_RESTRICT, NULL);
      return CheckAEEReturnStatus(nReturnStatus);
    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:

    CORE_MSG ("SecurityDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                  COREAPPTIMEOUT,
                  (PFNNOTIFY) DisplayIdleMenuDlg,
                  (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
} /* End SecurityDlgEventHandler */



/*===========================================================================
FUNCTION RestoreSettingsNowDlgEventHandler

DESCRIPTION
  handle restore setting events

DEPENDENCIES
  StartRestoring

SIDE EFFECTS
  None
===========================================================================*/
boolean RestoreSettingsNowDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Cancel Timer */
  if(evt == EVT_DIALOG_START) {  /* Cancel DisplayIdleMenuDlg timer */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
  }

  switch(evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("RestoreSettgsDlg: EVT_DIALOG_START", 0, 0, 0);
    StartRestoring(pMe, (IDialog*)dw); /* start restoring */

    /* Reset Timer and set up timer for ending this dialog */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    2000,
                    (PFNNOTIFY) EndCurrentDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  case EVT_KEY:
    CORE_MSG ("RestoreSettgsDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    /* the following keys bring back to security menu */
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
    /* clear key */
    case AVK_CLR:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      EndCurrentDlg(pMe);
      return TRUE;

    default:
      break;
    }
    return TRUE; //Not sure.

  case EVT_COMMAND:
      return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("RestoreSettgsDlg: EVT_DIALOG_END", 0, 0, 0);
    PopDlg(pMe, IDD_SECURITY); /* go back to the security menu */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End RestoreSettingsNowDlgEventHandler */



/*===========================================================================
FUNCTION PINChangeDlgEventHandler

DESCRIPTION
  handle PIN Change dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean PINChangeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  int nReturnStatus;
  uint16 wDlgID;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Stop Timer */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  switch (evt) {

  case EVT_DIALOG_START:
    CORE_MSG ("PINChangeDlg: EVT_DIALOG_START", 0, 0, 0);
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, IDC_MENU_PINCHANGE);
    if(pICurrentMenu) {
      /* remove or update items from the menu based on featurization requirement */
      StripFeatureMenuItem(pMe, pICurrentMenu, IDC_MENU_PINCHANGE);
      SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      return TRUE;
    }
    return FALSE; /* no current menu control ----> error */

  case EVT_KEY:
    CORE_MSG ("PINChangeDlg: EVT_KEY, key=%d", w, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      return FALSE; /* pass to core app to handle */
    }

  case EVT_COMMAND:
    CORE_MSG ("PINChangeDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    switch(w) {
    case IDL_PINCHANGE_1: /* Change PIN 1 */
      wDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1, IDD_OLD_PIN_REQUEST);
      if (wDlgID != 0) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                            gpszResourceFile,  wDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      break;
    case IDL_PINCHANGE_2: /* Change PIN 2 */
      wDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2, IDD_OLD_PIN2_REQUEST);
      if (wDlgID != 0) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case IDL_PINCHANGE_1_SLOT2: /* Change PIN 1 slot2 */
      wDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1_SLOT2, IDD_OLD_PIN_REQUEST_SLOT2);
      if (wDlgID != 0) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      break;
    case IDL_PINCHANGE_2_SLOT2: /* Change PIN 2 slot 2*/
      wDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2_SLOT2, IDD_OLD_PIN2_REQUEST_SLOT2);
      if (wDlgID != 0) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      break;
#endif
    default: /* NV Change */
      wDlgID = GetNextPINProcessDlgID(pMe, 0, IDD_OLD_NV_REQUEST);
      if (wDlgID != 0) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      break;
    }
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("PINChangeDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;


  case EVT_USER:
    CORE_MSG ("PINChangeDlg: EVT_USER", 0, 0, 0);
    switch(w) {
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS: /* change PIN successfully */
      /* Display warning message */
      DisplayMessageDlg(pMe, IDS_CHANGE_SUCCESS, SELECTION_TIME_IN_SECONDS);
      return TRUE;
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
      DisplayMessageDlg(pMe, IDS_CHANGE_SUCCESS_NO_UPDATE, SELECTION_TIME_IN_SECONDS);
      return TRUE;
    default:
      break;
    }
    return FALSE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End PINChangeDlgEventHandler */


/*===========================================================================
FUNCTION EnablePinDlgEventHandler

DESCRIPTION
  handle Entering PIN from radio button screen dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean EnablePinDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CCoreApp * pMe = (CCoreApp*) pUser;
  int nReturnStatus;
  uint16 wCtrlID;
  IDialog *pIDialog;
  IMenuCtl *pIMenu;
  uint16 wListID;
  uint16 wNextDlgID;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {

  case EVT_DIALOG_START:
    CORE_MSG ("EnablePINDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitRadioButtonDlg(pMe, (uint16)w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("EnablePINDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* handled keys */

    default:
      break; /* pass to core app to handle */
    }
    return FALSE;

  case EVT_COMMAND:
    CORE_MSG ("EnablePINDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    switch(w) {
      case IDL_PINLOCK1_ON:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1, IDD_ENTERPINENABLE);

        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);

      case IDL_PINLOCK1_OFF:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1, IDD_ENTERPINDISABLE);

        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);

      case IDL_NVLOCK_ON:
        wNextDlgID = GetNextPINProcessDlgID(pMe, 0, IDD_ENTERNVCODEENABLE);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
      case IDL_NVLOCK_OFF:
        wNextDlgID = GetNextPINProcessDlgID(pMe, 0, IDD_ENTERNVCODEDISABLE);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);

      case IDL_PINLOCK2_ON:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2, IDD_ENTERPIN2ENABLE);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
      case IDL_PINLOCK2_OFF:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2, IDD_ENTERPIN2DISABLE);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case IDL_PINLOCK1_SLOT2_ON:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1_SLOT2, IDD_ENTERPINENABLE_SLOT2);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
      case IDL_PINLOCK1_SLOT2_OFF:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN1_SLOT2, IDD_ENTERPINDISABLE_SLOT2);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
      case IDL_PINLOCK2_SLOT2_ON:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2_SLOT2, IDD_ENTERPIN2ENABLE_SLOT2);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
      case IDL_PINLOCK2_SLOT2_OFF:
        wNextDlgID = GetNextPINProcessDlgID(pMe, AEECARD_PIN2_SLOT2, IDD_ENTERPIN2DISABLE_SLOT2);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  wNextDlgID,  NULL);
        return (nReturnStatus == SUCCESS);
#endif
      default:
        return FALSE;
     }

  case EVT_USER:
    CORE_MSG ("EnablePINDlg: EVT_USER", 0, 0, 0);
    switch(w) {
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:

      /* Update the radio button on success */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      wCtrlID = GetDlgMenuCtlID(pMe, pIDialog);

      pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) wCtrlID);
      /* Get the user previously selected item, such that we can update the radio button */
      wListID = IMENUCTL_GetSel(pIMenu);

      /* update radio buttons */
      InitMenuIcons (pIMenu);
      SetMenuIcon (pIMenu, wListID, TRUE);

      /* Display warning message */
      DisplayMessageDlg(pMe, IDS_CHANGE_SUCCESS, SELECTION_TIME_IN_SECONDS);

      /* If PUK was verified now send back a subscription available command
        Remember that the subscription was made unavailable when pin was
         blocked */
      if (EVT_PUK_VERIFY_SUCCESS == w) {
        CoreApp_ProcessSubscriptionStatus(pMe);
      }

      return TRUE;

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("EnablePINDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End EnablePinDlgEventHandler */



/*===========================================================================
FUNCTION SelectCardDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SelectCardDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  uint16 wItemID;
  IDialog* pIActiveDialog;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("SelectCardDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("SelectCardDlg: EVT_KEY", 0, 0, 0);
    switch(w) {

      /*Do not allow user to use back or end key*/
    case AVK_CLR:
    case AVK_END:
      return TRUE;

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_INFO:  /* power down */
    default:
      return FALSE;
    }

  case EVT_COMMAND:
    {
      boolean bCard1Ready = FALSE;
      boolean bCard2Ready = FALSE;

      CORE_MSG ("SelectCardDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* user makes (new) selection */
      /* disable the timer that shows users their newly selected option */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIActiveDialog == NULL ) {
        CORE_ERR ("Null pointer ", 0,0,0);
        return FALSE;
      }

      /* Get the control ID */
      wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
      if(wCtrlID == 0) { /* error */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, wCtrlID);

      if(pICurrentMenu == NULL) {
        CORE_ERR ("Null pointer ", 0,0,0);
        return FALSE;
      }

      wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

      bCard1Ready = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1);
      bCard2Ready = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1_SLOT2);

      switch(wItemID) {
      case IDL_SELECT_CARD1:
      case IDL_SELECT_DEFAULT:
      case IDL_MENU_SELECT_CARD1:
      case IDL_MENU_SELECT_CARD_DEF:
        if(IsCardValid(pMe, AEECARD_PIN1))
        {
          //If the Pin for card 1 was not verified then verify it
          //Otherwise go ahead and send a PIN verification success here.
          if (!bCard1Ready) {
            StartPINVerificationProcess(pMe,AEECARD_PIN1);
          }
          else {
            ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
              EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN1);
          }
          return TRUE;
        }
        else
        {
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_CARD_1_ERROR, NULL);
          return TRUE;
        }
        break;

      case IDL_SELECT_CARD2:
        if(IsCardValid(pMe, AEECARD_PIN1_SLOT2))
        {
          if (!bCard2Ready) {
            StartPINVerificationProcess(pMe,AEECARD_PIN1_SLOT2);
          }
          else {
            ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
              EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN1_SLOT2);
          }
        }
        else
        {
          //Card has become invalid -> error
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_CARD_2_ERROR, NULL);
          return TRUE;
        }
        break;
      }
      return TRUE;
   }

  case EVT_USER:
  {
    boolean gsm = FALSE;
    boolean cdma = FALSE;
    uint32 newSlotGSM = AEECARD_SLOT_NONE;
    uint32 newSlotCDMA = AEECARD_SLOT_NONE;
    boolean bNVReady = FALSE;

    CORE_MSG ("SelectCardDlg: EVT_USER", 0, 0, 0);
    pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    /* Get the control ID */
    wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
    if(wCtrlID == 0) { /* error */
      CORE_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, wCtrlID);

    if(pICurrentMenu == NULL) {
      CORE_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }

    wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

    bNVReady = CoreSecurity_IsPinVerified(pMe, 0);

    if (dw == 0) {
      /* NV verification */
      switch (w) {
      case EVT_PIN_VERIFY_SUCCESS:
      case EVT_PIN_ENABLE_SUCCESS:
      case EVT_PIN_CHANGE_SUCCESS:
      case EVT_PUK_VERIFY_SUCCESS:
           if (FALSE == pMe->m_bSlotChangePending)
           {
              /* Subscription status changed */
              ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                      EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
           }
           /* end the current dialog */
           while( pIActiveDialog != NULL) {
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
           }
           CreateIdleMenuDlg(pMe);
           return TRUE;
      }
    }
    else { /* PIN1 or PIN1 slot 2 verification */
      switch (w) {
      case EVT_PIN_VERIFY_SUCCESS:
      case EVT_PIN_ENABLE_SUCCESS:
      case EVT_PIN_CHANGE_SUCCESS:
      case EVT_PUK_VERIFY_SUCCESS:
        switch (wItemID)
        {
        case IDL_MENU_SELECT_CARD1:
        case IDL_SELECT_CARD1:
          /* End current dialog */
          ISHELL_EndDialog(pMe->a.m_pIShell);
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                    IDD_SELECT_TECH_FOR_CARD_1, NULL);
          return CheckAEEReturnStatus(nReturnStatus);
        case IDL_SELECT_CARD2:
          /* End current dialog */
          ISHELL_EndDialog(pMe->a.m_pIShell);
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                    IDD_SELECT_TECH_FOR_CARD_2, NULL);
          return CheckAEEReturnStatus(nReturnStatus);
        case IDL_MENU_SELECT_CARD_DEF:
        case IDL_SELECT_DEFAULT:

          DetermineOperSlotForDefault(pMe, &newSlotGSM, &newSlotCDMA,
                                    &gsm, &cdma);
          UpdateSlotPref(pMe, gsm, cdma, newSlotGSM, newSlotCDMA);
          pMe->m_bUserMadeFirstSelSub = TRUE;
          if (!bNVReady) {
            /* need to verify NV code */
            StartNVCodeVerificationProcess(pMe, pMe->m_sNV_lock_state);
            return TRUE;
          }
          else {
            /* no need to verify NV, can create idle menu now */
            if (FALSE == pMe->m_bSlotChangePending)
            {   /* Subscription status changed */
              ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                      EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
            }
            /* end the current dialog */
            while( pIActiveDialog != NULL) {
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            }
            CreateIdleMenuDlg(pMe);
            return TRUE;
          }
        }
        return FALSE;

      default:
        return FALSE;
      }
    }
    return FALSE;
  }

  case EVT_DIALOG_END:
    CORE_MSG ("RadioButnDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
  return FALSE;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
} /* End SelectCardDlgEventHandler */

/*===========================================================================
FUNCTION SelectTechDlgEventHandler

DESCRIPTION
  Handle check box event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SelectTechDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;
  IMenuCtl* pIMenu;
  IDialog *pIDialog;
  uint16 wItemID;
  uint16 wDlgID = 0;
  boolean gsm = FALSE;
  boolean cdma = FALSE;
  uint32 newSlotGSM = AEECARD_SLOT_NONE;
  uint32 newSlotCDMA = AEECARD_SLOT_NONE;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("SelectTechDlg: EVT_DIALOG_START", 0, 0, 0);
    /* PopulateCardApplication in case the pMe variable hasn't been initialized yet */
    PopulateCardApplications(pMe);
    return InitSelectTechDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("SelectTechDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      return TRUE;
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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
    default:
      return FALSE;
    }


  case EVT_NOTIFY:
    switch (((AEENotify *) dw)->dwMask )
    {
      case NMASK_CARD_STATUS:
        switch(pMe->m_nCardStatus) {
          case AEECARD_VALID_CARD: /* received SIM_INIT_COMPLETED_EVENT */
          case AEECARD_AVAIL_CARD:
            MSG_HIGH("Valid Card Received slot 1",0,0,0);
            ValidateCardsAndChangeState(pMe,FALSE);
            break;

          case AEECARD_CARD_REINIT:
            /* Go to SIM Reinit state since we will get  a valid card for this card */
            if (CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1))
            {
              ISHELL_EndDialog(pMe->a.m_pIShell);
              MSG_HIGH("Reinit Received slot 1",0,0,0);
              nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                    gpszResourceFile,  IDD_SIM_REINIT,  NULL);
            }
            break;
        }
        break;

      case NMASK_CARD_SLOT2_STATUS:
        switch(pMe->m_nCardStatus_slot2) {
          case AEECARD_VALID_CARD: /* received SIM_INIT_COMPLETED_EVENT */
          case AEECARD_AVAIL_CARD:
            MSG_HIGH("Valid Card Received slot 2",0,0,0);
            ValidateCardsAndChangeState(pMe,FALSE);
            break;

          case AEECARD_CARD_REINIT:
            /* We will get a valid card event for this card sicne Pin is verified */
            if (CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1_SLOT2))
            {
              ISHELL_EndDialog(pMe->a.m_pIShell);
              MSG_HIGH("Reinit Received slot 2",0,0,0);
              nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                  gpszResourceFile,  IDD_SIM_REINIT,  NULL);
            }
            break;
        }
        break;

      default:
        break;
    }

    return TRUE;

  case EVT_COMMAND:
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, GetDlgMenuCtlID(pMe, pIDialog));

    CORE_MSG ("SelectTechDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if (pIMenu == NULL) {
      MSG_ERROR("Null Menu ctl", 0, 0, 0);
      return FALSE;
    }
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);


    /* This logic below will toggle the technology being selected
    between cards if both cards are present */

    if(IMENUCTL_IsActive(pIMenu))// SoftKey Menu Control is Not Active
    {
       boolean setBox;
       wItemID = IMENUCTL_GetSel(pIMenu);
       setBox = !GetCheckBoxVal(pIMenu, wItemID);
       SetCheckBoxItem (pIMenu, wItemID, setBox);
       if(wItemID ==  IDL_SELECT_GSM_1)
       {
         SetCheckBoxItem (pIMenu, IDL_SELECT_GSM_2, FALSE);
       }
       else if(wItemID ==  IDL_SELECT_GSM_2)
       {
         SetCheckBoxItem (pIMenu, IDL_SELECT_GSM_1, FALSE);
       }
       else if(wItemID ==  IDL_SELECT_CDMA_1)
       {
         SetCheckBoxItem (pIMenu, IDL_SELECT_CDMA_2, FALSE);
       }
       else if(wItemID ==  IDL_SELECT_CDMA_2)
       {
         SetCheckBoxItem (pIMenu, IDL_SELECT_CDMA_1, FALSE);
       }
       return TRUE; // Do nothing
    }
    /*SK is active meaning the user is on the DEFAULT/(CARD1CARD2)/OK Menu*/
    else
    {
      boolean bCard1Ready = FALSE;
      boolean bCard2Ready = FALSE;

      /* Get dialog ID */
      wDlgID = IDIALOG_GetID(pIDialog);

      if (wDlgID == 0) {
        return FALSE;
      }
      /* Get the softkey menu control */
      pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, GetDlgSkMenuCtlID(wDlgID));

      if (pIMenu == NULL) {
        return FALSE;
      }

      /* Get the item selected within that control i.e DEFAULT/(CARD1CARD2)/OK*/
      wItemID = IMENUCTL_GetSel(pIMenu);

      bCard1Ready = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1);
      bCard2Ready = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1_SLOT2);


      switch (wItemID)
      {
        /* DONE and both cards selected */
      case IDL_SK_SELECT_DONE:
        /* Done and card 1 selected */
      case IDL_SK_SELECT_TECH_DONE_1:
        /* Done and card 2 selected */
      case IDL_SK_SELECT_TECH_DONE_2:

        DetermineNewGSMCDMAOperSlot(pMe, &newSlotGSM, &newSlotCDMA,
                                   &gsm, &cdma);
        UpdateSlotPref(pMe, gsm, cdma, newSlotGSM, newSlotCDMA);

        break;

      /* Card 1 selected while displaying tech for card 2 */
      case IDL_SK_SELECT_TECH_CARD_1:
        if (!bCard1Ready) {
          /* need to verify PIN1 code */
          StartPINVerificationProcess(pMe, AEECARD_PIN1);
          return TRUE;
        }
        else
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN1);
        }
        break;


      /* Card 2 selected while displaying tech for card 1 */
      case IDL_SK_SELECT_TECH_CARD_2:
        if (!bCard2Ready)
        {
          /* need to verify PIN1 code */
          StartPINVerificationProcess(pMe, AEECARD_PIN1_SLOT2);
          return TRUE;
        }
        else
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                             EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN1_SLOT2);
        }
        break;

      case IDL_SK_DEFAULT:
      case IDL_SK_SELECT_TECH_DEF_1:
      case IDL_SK_SELECT_TECH_DEF_2:
        if (!bCard1Ready)
        {
          /* Default is equivalent for selecting card1 so need to verify PIN1 code */
          StartPINVerificationProcess(pMe, AEECARD_PIN1);
          return TRUE;
        }
        else
        {
          /* Card 1 not ready now check if card 2 can be taken as default */
          DetermineOperSlotForDefault(pMe, &newSlotGSM, &newSlotCDMA,
                                    &gsm, &cdma);
          /* Update the slot pref and wait for slot success event
             Once we get EVT_USER_SLOT_PREF_SUCCESS we send the
             Subscription change */
          UpdateSlotPref(pMe, gsm, cdma, newSlotGSM, newSlotCDMA);
        }
        break;
      }
    }
    return TRUE;

  case EVT_USER:
    {
      CORE_MSG ("SelectTechDlg: EVT_USER", 0, 0, 0);
      /* NV verification */
      switch (w) {
        /* We have verified the PIN now start the Card Selection for user to make selection */
        case EVT_PIN_VERIFY_SUCCESS: /* PIN1 / PIN2 verification success */
        case EVT_PIN_ENABLE_SUCCESS:
        case EVT_PIN_CHANGE_SUCCESS:
        case EVT_PUK_VERIFY_SUCCESS:
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          DetermineNextCardState(pMe,FALSE);
          break;

        case EVT_USER_SLOT_PREF_SUCCESS:
          pMe->m_bUserMadeFirstSelSub = TRUE;

          if (FALSE == pMe->m_bSlotChangePending)
          {
            /* Subscription status changed */
            ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                      EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
          }

          /* end the current dialog */
          while( pIDialog != NULL)
          {
            (void)ISHELL_EndDialog(pMe->a.m_pIShell);
            pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          }
          CreateIdleMenuDlg(pMe);
          break;

        case EVT_USER_SLOT_PREF_FAIL:
          /* TBD haven't gotten this to happen on target, only simulated
          GSDI does not always populate nCmdStatus with FAILURE so
          check if that has been fixed in lower layers */
          CORE_ERR ("User Slot Pref Failure", 0, 0, 0);
          if (FALSE == pMe->m_bSlotChangePending)
          {
            /* Subscription status changed */
            ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                               EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
          }

          /* end the current dialog */
          while( pIDialog != NULL)
          {
            (void)ISHELL_EndDialog(pMe->a.m_pIShell);
             pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          }
          CreateIdleMenuDlg(pMe);
          return FALSE;

        case EVT_SYS_PREF_ERR:
          //TBD why do we get this do we need to throw a message
          CORE_ERR ("Unable to Set System Pref", 0, 0, 0);
          return FALSE;

        case EVT_MESSAGE_END:
          return FALSE;

        default:
          return FALSE;
      } /* switch (w) */
    } //EVT_USER
        return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("CheckBoxDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else /*FEATURE_MMGSDI_DUAL_SLOT */
  return FALSE;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
} /* End SelectTechDlgEventHandler */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION DeterminePersoDialogID

DESCRIPTION
  Determine the dialog to be displayed if 
  personalization feature is activated.

  The dialog ID is returned in wDlgID
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void  DeterminePersoDialogID(CCoreApp *pMe, uint16 *wDlgID)
{
  if ((pMe == NULL) || (wDlgID == NULL)) {
    return;
  }
  /* Assign dialog ID's based on the perso event */
  switch (pMe->m_nPersoStatus)
  {     
    case AEECARD_PERSO_NETWORK_FAIL:
      *wDlgID = IDD_ENTER_NW_PERSO_KEY;
      break;
    case AEECARD_PERSO_NETWORK_SUBSET_FAIL:
      *wDlgID = IDD_ENTER_NW_SUB_PERSO_KEY;
      break;
    case AEECARD_PERSO_SERVICE_PROVIDER_FAIL:
      *wDlgID = IDD_ENTER_SP_PERSO_KEY;
      break;
    case AEECARD_PERSO_CORPORATE_FAIL:
      *wDlgID = IDD_ENTER_CP_PERSO_KEY;
      break;
    case AEECARD_PERSO_SIM_FAIL:
      *wDlgID = IDD_ENTER_SIM_PERSO_KEY;
      break;
    case AEECARD_PERSO_NETWORK_DEACTIVATE:        
    case AEECARD_PERSO_NETWORK_SUBSET_DEACTIVATE:        
    case AEECARD_PERSO_SERVICE_PROVIDER_DEACTIVATE:      
    case AEECARD_PERSO_CORPORATE_DEACTIVATE:
    case AEECARD_PERSO_SIM_DEACTIVATE:
      *wDlgID = IDD_CORE;
       break;
    case AEECARD_PERSO_NETWORK_CONTROL_BLOCKED:
    case AEECARD_PERSO_NETWORK_SUBSET_BLOCKED:
    case AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED:
    case AEECARD_PERSO_CORPORATE_BLOCKED:
    case AEECARD_PERSO_SIM_BLOCKED:
    case AEECARD_PERSO_FAILED:
      *wDlgID = IDD_NOCARD_EMERGENCY;          
      break;
    default:
      *wDlgID = IDD_CORE;
      break;
  } // End of switch
}
#endif

/*===========================================================================
FUNCTION DetermineSingleSlotState

DESCRIPTION
  Determine DetermineSingleSlotState on card info

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int  DetermineSingleSlotState(CCoreApp *pMe, uint16 *wDlgID, boolean bAllowBypass)
{

  boolean bCard1Ready = FALSE;
  boolean bCard1Available = FALSE;

  *wDlgID = 0;

  if ((pMe == NULL) || (wDlgID == NULL)) {
    return EFAILED;
  }

  /* If we are in low power mode return
   * Dialog is already created in CreateLPMMode function
   */
  if (pMe->m_bLPM)
  {
    *wDlgID = IDD_LPM;
    return SUCCESS;
  }    
  bCard1Available = IsCardValid(pMe, AEECARD_PIN1);
  bCard1Ready     = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1);

  if (!bCard1Available) {
    if (((pMe->m_rtreConfig == AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK) &&
         (pMe->m_rtreControl == AEECM_RTRE_CONTROL_NV)) ||
         (pMe->m_rtreConfig == AEECM_RTRE_CONFIG_NV_ONLY))
    {
      *wDlgID = IDD_CORE;
    }
    else
    {
      *wDlgID = IDD_CARD_INVALID;
    }
  } //end of if (!bCard1Available)
  else
  {
    /* card1 is available */
    if(!bCard1Ready)
    {
      *wDlgID = IDD_ENTERPIN;
    }
    else
    {
      /* Set the value of wDlgID in case 
       * FEATURE_MMGSDI_PERSONALIZATION is not defined
       */
      *wDlgID = IDD_CORE;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
      if (pMe->m_nPersoStatus != AEECARD_PERSO_NO_EVENT)
      { 
        DeterminePersoDialogID(pMe,wDlgID);  
      }
#endif
    }   // end of if(!bCard1Ready) else
  } // end of if (!bCard1Available) else
  return SUCCESS;
} /* DetermineSingleSlotState */


/*===========================================================================
FUNCTION DetermineNextCardState

DESCRIPTION
  Determine the card state based on card info
  This function delegates the determination to single and dual slot specific
  functions

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int DetermineNextCardState(CCoreApp *pMe, boolean bAllowByPass) {
  uint16 wDlgID = 0;
  uint16 wCurrDlgID = 0;
  int nReturnStatus = SUCCESS;
  IDialog *pIDialog;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog != NULL) {
    wCurrDlgID = IDIALOG_GetID(pIDialog);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  nReturnStatus = DetermineDualSlotState(pMe, &wDlgID, bAllowByPass);
#else
  nReturnStatus = DetermineSingleSlotState(pMe, &wDlgID, bAllowByPass);
#endif

  CORE_ERR("Coreapp: DetermineSingleSlotState status %d, dialog %d", nReturnStatus, wDlgID, 0);

  if (nReturnStatus != SUCCESS) {
    return nReturnStatus;
  }
  if (wDlgID == 0) {
    /* either both card1 and card2 are invalid
       or card 2 pin verification */
    return SUCCESS;
  }


  /* dialog is different, use PopDlg to the wDlgID to clear all dialogs */
  /* clean up the existing dialog */
  switch (wDlgID) {
    case IDD_LPM:
      //Do nothing for now
      return SUCCESS;    

    case IDD_CORE:
      if (wCurrDlgID == wDlgID) {
        /* same dialog, done */
        return SUCCESS;
      }
      PopDlg (pMe, wDlgID);
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_SUBSCRIPTION_CHANGED, 0);
      return SUCCESS;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    /* Perso dialogs */        
    case IDD_ENTER_NW_PERSO_KEY:            
    case IDD_ENTER_NW_SUB_PERSO_KEY:      
    case IDD_ENTER_SP_PERSO_KEY:
    case IDD_ENTER_CP_PERSO_KEY:
    case IDD_ENTER_SIM_PERSO_KEY:
      /* Create new dialog */      
      if (wCurrDlgID == wDlgID) {
        /* same dialog, done */
        return SUCCESS;
      }
      PopDlg (pMe, wDlgID);
      return ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                 wDlgID,  NULL);      
#endif
    
    case IDD_NOCARD_EMERGENCY:
    case IDD_CARD_INVALID:
      if (wCurrDlgID == wDlgID) {
        /* same dialog, done */
        return SUCCESS;
      }
      PopDlg (pMe, wDlgID);
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                       wDlgID, NULL);
      return nReturnStatus;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case IDD_ENTERPIN_SLOT2:
      if (wCurrDlgID == wDlgID || IDD_HOURGLASS == wCurrDlgID) {
        /* if we are waiting for PIN verfication or this is
        the pin verification dialog then do not start the same dialog */
        return SUCCESS;
      }
      PopDlg (pMe, wDlgID);
      StartPINVerificationProcess(pMe, AEECARD_PIN1_SLOT2);
      return SUCCESS;
#endif

    case IDD_ENTERPIN:
      if (wCurrDlgID == wDlgID || IDD_HOURGLASS == wCurrDlgID) {
        /* if we are waiting for PIN verfication or this is
        the pin verification dialog then do not start the same dialog */
        return SUCCESS;
      }
      PopDlg (pMe, wDlgID);
      StartPINVerificationProcess(pMe, AEECARD_PIN1);
      return SUCCESS;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /* Added this case instead of default because there was
    overlapping of dialogs resulting in user selection overwritten */
    case IDD_SELECT_TECH_FOR_CARD_1:
    case IDD_SELECT_TECH_FOR_CARD_2:
    case IDD_SELECT_TECH_FOR_BOTH_CARDS:
      if (wCurrDlgID == wDlgID || IDD_HOURGLASS == wCurrDlgID)
      {
          return SUCCESS;
      }
      else
      {
        PopDlg (pMe, wDlgID);
      }

      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                       wDlgID, NULL);
      /* User is about to select a technology or card */
      return nReturnStatus;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      if (TRUE == pMe->m_bSlotChangePending) {
        if (wCurrDlgID == wDlgID) {
          /* same dialog, done */
          return SUCCESS;
        }
        PopDlg (pMe, wDlgID);
        /* Can only get here if already finished "reiniting"
           after a slot change process */
        /* go to idle */
        /* This needs to be SendEvent, because the processing for
           EVT_SECURITY_SUCCESS destroys all dialogs.  This ensures that the
           kill all dialogs doesn't kill the new Idle dialog as well. */
        ISHELL_SendEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP,
                       EVT_USER, EVT_SECURITY_SUCCESS, 0);
        CreateIdleMenuDlg(pMe);
        return SUCCESS;
      }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
      if (wCurrDlgID == wDlgID) {
        /* same dialog, remove the dialog in case the softkey is required to be
         * updated in the refresh reset scenario */
        ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      else
      {
        PopDlg (pMe, wDlgID);
      }

      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                       wDlgID, NULL);
      /* User is about to select a technology or card */
      return nReturnStatus;
  }

} /* DetermineNextCardState */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION DetermineDualSlotState

DESCRIPTION
  Determine the next card states based on card info

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int  DetermineDualSlotState(CCoreApp *pMe, uint16 *wDlgID, boolean bAllowBypass)
{

  static uint32 dwOldSecurityMask = 0;

  boolean bCard1Ready     = FALSE;
  boolean bCard1Available = FALSE;

  boolean bCard2Ready = FALSE;
  boolean bCard2Available = FALSE;

  *wDlgID = 0;

  if ((pMe == NULL) || (wDlgID == NULL)) {
    return EFAILED;
  }

  /*
    When we start other app from idle Menu, and then press the END Key,
    BREW closes all apps including CoreApp and then it again starts coreApp.
    We must allow moving to IDD_CORE under this condition.
  */
  if (bAllowBypass && (dwOldSecurityMask == pMe->m_dwSecurityMask))
  {
    *wDlgID = IDD_CORE;
    return SUCCESS;
  }

  /* If we are in low power mode return
   * Dialog is already created in CreateLPMMode function
   */
  if (pMe->m_bLPM)
  {
    *wDlgID = IDD_LPM;
    return SUCCESS;
  } 
  /* TODO
   * Need to add: Handling of perso feature dialogs
   */
  bCard1Available = IsCardValid(pMe, AEECARD_PIN1);
  bCard1Ready     = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1);

  bCard2Available = IsCardValid(pMe, AEECARD_PIN1_SLOT2);
  bCard2Ready     = CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1_SLOT2);

  if (!bCard1Available && !bCard2Available) {
    if (((pMe->m_rtreConfig == AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK) &&
         (pMe->m_rtreControl == AEECM_RTRE_CONTROL_NV)) ||
         (pMe->m_rtreConfig == AEECM_RTRE_CONFIG_NV_ONLY))
    {
      *wDlgID = IDD_CORE;
    }
    else
    {
      *wDlgID = IDD_CARD_INVALID;
    }
  }
  else if(bCard1Available && bCard2Available)
  {
    if(!bCard1Ready && !bCard2Ready)
    {
      *wDlgID =IDD_SELECT_CARD;
    }
    else if( !bCard1Ready && bCard2Ready)
    {
      *wDlgID = IDD_SELECT_TECH_FOR_CARD_2;
      if(pMe->m_bTestAutomationMode) {
        /* is slot 2 pin disable and slot 1 pin enable? */
        if ( (pMe->m_sPIN1_lock_state == AEECARD_PIN_ENABLED) &&
             (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_DISABLED) )
        {
          *wDlgID = IDD_ENTERPIN;
        }
      }
    }
    else if(bCard1Ready && !bCard2Ready)
    {
      *wDlgID = IDD_SELECT_TECH_FOR_CARD_1;
      if(pMe->m_bTestAutomationMode) {
      /* is slot 1 pin disable and slot 2 pin enable? */
      if ( (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_ENABLED) &&
           (pMe->m_sPIN1_lock_state == AEECARD_PIN_DISABLED) )
      {
        *wDlgID = IDD_ENTERPIN_SLOT2;
      }
    }
  }
  else  // Both are ready for display
  {
    *wDlgID = IDD_SELECT_TECH_FOR_BOTH_CARDS;
    //Check for NV Item
    if(pMe->m_bTestAutomationMode)
    {
      /* check if both pin was disabled */
      if (((pMe->m_sPIN1_lock_state == AEECARD_PIN_DISABLED) &&
           (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_DISABLED))  ||
           (((pMe->m_sPIN1_lock_state == AEECARD_PIN_ENABLED) ||
           (pMe->m_sPIN1_lock_state == AEECARD_PIN_VERIFIED)) &&
           (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_DISABLED))  ||
           ((pMe->m_sPIN1_lock_state == AEECARD_PIN_DISABLED) &&
           ((pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_ENABLED) ||
           (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_VERIFIED))))
      {
        *wDlgID = IDD_CORE;
        // Get the user slot pref accordingly
        pMe->m_byGSMUserSlotPref = GetUserSlotPrefGSM(pMe);
        pMe->m_byCDMAUserSlotPref = GetUserSlotPrefCDMA(pMe);
      }
    }
  }
  else if(bCard1Available)
  {
    if(!bCard1Ready)
    {
      *wDlgID = IDD_SELECT_CARD1;
      if (pMe->m_bTestAutomationMode) {
        *wDlgID = IDD_ENTERPIN;
      }
    }
    else
    {
      *wDlgID = IDD_SELECT_TECH_FOR_CARD_1;
       //Check the NV Item
       if(pMe->m_bTestAutomationMode)
       {
         *wDlgID = IDD_CORE;
         // Get the user slot pref accordingly
         pMe->m_byGSMUserSlotPref = GetUserSlotPrefGSM(pMe);
         pMe->m_byCDMAUserSlotPref = GetUserSlotPrefCDMA(pMe);
       }
    }
    else if(bCard2Available)
    {
      if(!bCard2Ready)
      {
        /* need to verify PIN1 code for slot 2 */
        *wDlgID = IDD_ENTERPIN_SLOT2;
      }
      else
      {
        *wDlgID = IDD_SELECT_TECH_FOR_CARD_2;
        //Check the NV Item
        if(pMe->m_bTestAutomationMode)
        {
          *wDlgID = IDD_CORE;
          // Get the user slot pref accordingly
          pMe->m_byGSMUserSlotPref = GetUserSlotPrefGSM(pMe);
          pMe->m_byCDMAUserSlotPref = GetUserSlotPrefCDMA(pMe);
      }
    }
  }

  dwOldSecurityMask = pMe->m_dwSecurityMask;

  return SUCCESS;
} /* DetermineDualSlotState */


/*===========================================================================
FUNCTION DetermineOperSlotForDefault

DESCRIPTION
  Populate the new operational slot and gsm/cdma boolean for default tech
  user selection

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DetermineOperSlotForDefault(CCoreApp *pMe, uint32 *newSlotGSM, uint32 *newSlotCDMA,
                                 boolean *gsm, boolean *cdma)
{

  if((pMe == NULL) || (newSlotGSM == NULL) || (newSlotCDMA == NULL)
     || (gsm == NULL) || (cdma == NULL))
  {
    CORE_ERR("something is Null %x %x %x", pMe, newSlotGSM, newSlotCDMA);
    return;
  }

  *newSlotGSM = AEECARD_SLOT_NONE;
  *newSlotCDMA = AEECARD_SLOT_NONE;
  *gsm = FALSE;
  *cdma = FALSE;

  /* populate the correct operational slot info first */
  if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_CDMA_OPERATIONAL_MASK) == COREAPP_SLOT1_CDMA_OPERATIONAL_MASK) {
    *newSlotCDMA = AEECARD_SLOT1;
  }
  if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_CDMA_OPERATIONAL_MASK) == COREAPP_SLOT2_CDMA_OPERATIONAL_MASK) {
    *newSlotCDMA = AEECARD_SLOT2;
  }
  if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_GSM_OPERATIONAL_MASK) == COREAPP_SLOT1_GSM_OPERATIONAL_MASK) {
    *newSlotGSM = AEECARD_SLOT1;
  }
  if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_GSM_OPERATIONAL_MASK) == COREAPP_SLOT2_GSM_OPERATIONAL_MASK) {
    *newSlotGSM = AEECARD_SLOT2;
  }

  if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_PRESENT_MASK) == COREAPP_SLOT1_PRESENT_MASK) {
    /* slot 1 present */
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_RUIM_MASK) == COREAPP_SLOT1_APP_RUIM_MASK)
    {
      /* CDMA presented in slot 1 */
      *newSlotCDMA = AEECARD_SLOT1;
      *cdma = TRUE;
    }
    else {
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_GSM_MASK) == COREAPP_SLOT1_APP_GSM_MASK) {
        /* has only gsm */
        *newSlotGSM = AEECARD_SLOT1;
        *gsm = TRUE;
      }
    }
  }
  else {
    CORE_ERR("No Slot 1, should have allow Default selection", 0, 0, 0);
    return;
  }
} /* DetermineOperSlotForDefault*/


/*===========================================================================
FUNCTION DetermineNewGSMCDMAOperSlot

DESCRIPTION
  Populate the new operational slot and gsm/cdma boolean based on user check
  box selection

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DetermineNewGSMCDMAOperSlot(CCoreApp *pMe, uint32 *newSlotGSM, uint32 *newSlotCDMA,
                                 boolean *gsm, boolean *cdma)
{
  uint8 itemCount, i;
  boolean itemSet = FALSE;
  IMenuCtl* pIMenu = NULL;
  IDialog* pIDialog = NULL;
  uint16 wItemID = 0;

  if((pMe == NULL) || (newSlotGSM == NULL) || (newSlotCDMA == NULL)
     || (gsm == NULL) || (cdma == NULL)) {
     CORE_ERR("something is Null %x %x %x", pMe, newSlotGSM, newSlotCDMA);
     return;
  }

  *gsm = FALSE;
  *cdma = FALSE;
  *newSlotGSM = AEECARD_SLOT_NONE;
  *newSlotCDMA = AEECARD_SLOT_NONE;

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL)
     return;

  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, GetDlgMenuCtlID(pMe, pIDialog));

  if(pIMenu == NULL)
     return;

  itemCount = IMENUCTL_GetItemCount(pIMenu);

  for(i=0;i<itemCount;i++)
  {
    wItemID = IMENUCTL_GetItemID(pIMenu, i);
    itemSet = GetCheckBoxVal(pIMenu, wItemID);

    switch(wItemID)
    {
      case IDL_SELECT_GSM_1:
      case IDL_GSM_1:
        if(itemSet)
        {
          *newSlotGSM = AEECARD_SLOT1;
          *gsm = TRUE;
        }
        break;
      case IDL_SELECT_GSM_2:
      case IDL_GSM_2:
        if(itemSet)
        {
          *newSlotGSM = AEECARD_SLOT2;
          *gsm = TRUE;
        }
        break;
      case IDL_SELECT_CDMA_1:
      case IDL_CDMA_1:
        if(itemSet)
        {
          *newSlotCDMA = AEECARD_SLOT1;
          *cdma = TRUE;
        }
        break;
      case IDL_SELECT_CDMA_2:
      case IDL_CDMA_2:
        if(itemSet)
        {
          *newSlotCDMA = AEECARD_SLOT2;
          *cdma = TRUE;
        }
        break;
      default:
        CORE_ERR("unexpected control flow %d", wItemID, 0, 0);
        break;
    }
  }
} /* DetermineNewGSMCDMAOperSlot */

/*===========================================================================
FUNCTION UpdateSlotPref

DESCRIPTION
  Determine if slot switch is required based on operational slot (new and old)
  also set the user preferred slot based on the selection

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int UpdateSlotPref(CCoreApp *pMe, boolean gsm, boolean cdma, uint32 newSlotGSM, uint32 newSlotCDMA)
{

  boolean slot1ReadyForTechDisplay = FALSE;
  boolean slot2ReadyForTechDisplay = FALSE;

  if(pMe == NULL) {
    return EFAILED;
  }

  pMe->m_bSlotChangePending = FALSE;

  switch (newSlotGSM) {
    case AEECARD_SLOT_NONE:
      pMe->m_byGSMUserSlotPref = newSlotGSM;
      break;
    case AEECARD_SLOT1:
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_GSM_OPERATIONAL_MASK) ==
         COREAPP_SLOT2_GSM_OPERATIONAL_MASK)
      { /* original op slot for GSM is slot 2 */
        pMe->m_bSlotChangePending = TRUE;
      }
      break;
    case AEECARD_SLOT2:
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_GSM_OPERATIONAL_MASK) ==
         COREAPP_SLOT1_GSM_OPERATIONAL_MASK)
      { /* original op slot for GSM is slot 1 */
        pMe->m_bSlotChangePending = TRUE;
      }
      break;
  }


  switch (newSlotCDMA) {
    case AEECARD_SLOT_NONE:
      pMe->m_byCDMAUserSlotPref = newSlotCDMA;
      break;
    case AEECARD_SLOT1:
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_CDMA_OPERATIONAL_MASK) ==
         COREAPP_SLOT2_CDMA_OPERATIONAL_MASK)
      { /* original op slot for CDMA is slot 2 */
        pMe->m_bSlotChangePending = TRUE;
      }
      break;
    case AEECARD_SLOT2:
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_CDMA_OPERATIONAL_MASK) ==
         COREAPP_SLOT1_CDMA_OPERATIONAL_MASK)
      { /* original op slot for CDMA is slot 1 */
        pMe->m_bSlotChangePending = TRUE;
      }
      break;
  }


  slot1ReadyForTechDisplay    = CoreSecurity_IsCardReadyForTechDisplay(pMe, AEECARD_PIN1);
  slot2ReadyForTechDisplay    = CoreSecurity_IsCardReadyForTechDisplay(pMe, AEECARD_PIN1_SLOT2);

  if ((pMe->m_bSlotChangePending & COREAPP_PENDING_SLOT_CHG1_MASK) == COREAPP_PENDING_SLOT_CHG1_MASK) {
    if (!slot1ReadyForTechDisplay) {
      /* will not get a REINIT and VALID combination for slot 1 */
      pMe->m_bSlotChangePending &= ~COREAPP_PENDING_SLOT_CHG1_MASK_VALID_MASK;
    }
  }
  if ((pMe->m_bSlotChangePending & COREAPP_PENDING_SLOT_CHG2_MASK) == COREAPP_PENDING_SLOT_CHG2_MASK) {
    if (!slot2ReadyForTechDisplay) {
      /* will not get a REINIT and VALID combination for slot 2 */
      pMe->m_bSlotChangePending &= ~COREAPP_PENDING_SLOT_CHG2_MASK_VALID_MASK;
    }
  }

  /* write to NV with new selection regardless */
  SetUserSlotPrefGSM(pMe, newSlotGSM);
  SetUserSlotPrefCDMA(pMe, newSlotCDMA);

  /* If user did not make any selection we still want to move on
  This is like getting a successful slot change*/
  if (AEECARD_SLOT_NONE == newSlotGSM && AEECARD_SLOT_NONE == newSlotCDMA) {
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
               EVT_USER, EVT_USER_SLOT_PREF_SUCCESS, 0);
  }

  /* Set the network mode preference */
  if(gsm && cdma)
  {
    SetForceModePref(pMe, AEECM_MODE_PREF_AUTOMATIC);
  }
  else if(gsm)
  {
    SetForceModePref(pMe, AEECM_MODE_PREF_GSM_ONLY);
  }
  else if(cdma)
  {
    SetForceModePref(pMe, AEECM_MODE_PREF_CDMA_HDR_ONLY);
  }

  return SUCCESS;

} /* UpdateSlotPref*/
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

#if (defined(FEATURE_WCDMA)|| defined(FEATURE_GSM))
#ifdef FEATURE_TC
/*===========================================================================
FUNCTION UpdateTCScreen

DESCRIPTION
  Update the TC screen with the corresponding text based on the newly
  avaliable PH info

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void UpdateTCScreen(CCoreApp *pMe)
{
  uint16 wStringID = IDS_INVALID_TC_TYPE;
  AECHAR pBuf[TC_BUFFER_SIZE];
  IDialog *pIDialog;
  IStatic *pIStatic;

  if (pMe == NULL)
  {
    CORE_ERR("Null pMe", 0, 0, 0);
    return;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog == NULL)
  {
    CORE_ERR("Null DialogPtr", 0, 0, 0);
    return;
  }
  pIStatic = (IStatic*)IDIALOG_GetControl(pIDialog, IDC_STATIC_TC_TEST_MODE);
  if (pIStatic == NULL)
  {
    CORE_ERR("Null Static Ptr", 0, 0, 0);
    return;
  }

  switch( pMe->m_phInfo.test_control_type )
  {
  case  AEECM_TEST_CONTROL_TYPE_LB_MODE1:    /* LOOPBACK MODE1 */
    wStringID = IDS_LOOPBACK1;
    break;

  case  AEECM_TEST_CONTROL_TYPE_LB_MODE2:    /* LOOPBACK MODE2 */
    wStringID = IDS_LOOPBACK2;
    break;

  case  AEECM_TEST_CONTROL_TYPE_LB_MODE1_WITH_DUMMY_SIGNALLING:   /* LOOPBACK MODE1 with dummy signalling */
    wStringID = IDS_LOOPBACK1_DS;
    break;

  case  AEECM_TEST_CONTROL_TYPE_LB_MODE2_WITH_DUMMY_SIGNALLING:   /* LOOPBACK MODE2 with dummy signalling */
    wStringID = IDS_LOOPBACK2_DS;
    break;

  default:
    break;
  }
  if (ISHELL_LoadResString (pMe->a.m_pIShell, gpszResourceFile, wStringID,
                            pBuf, TC_BUFFER_SIZE * sizeof(AECHAR)) != 0) {
    ISTATIC_SetText (pIStatic, NULL, pBuf, AEE_FONT_BOLD, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);
  }
  (void) SetGeneralStaticTextStyle(pIStatic);
}
#endif
#endif

/*===========================================================================
FUNCTION TCTestModeDlgEventHandler

DESCRIPTION
  handle TC Test Mode dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean TCTestModeDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  CCoreApp * pMe = (CCoreApp*) pUser;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START ) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
  }


  switch(evt) {
  case EVT_DIALOG_START:
#if (defined(FEATURE_WCDMA)|| defined(FEATURE_GSM))
#ifdef FEATURE_TC
    {
      IStatic* pIStaticText;
      CORE_MSG ("TCTestModeDlg: EVT_DIALOG_START", 0, 0, 0);
      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_TC_TEST_MODE);
      if (pIStaticText != NULL)
      {
        (void) SetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */
        /* Force the control to redraw */
        ICONTROL_Redraw((IControl*)pIStaticText);
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }

  case EVT_KEY:
    CORE_MSG ("TCTestModeDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_NOTIFY:
    if (!pMe->m_bSuspended && ((AEENotify *) dw)->cls == AEECLSID_CM_NOTIFIER)
    {
      AEECMNotifyInfo *event_info = (((AEENotify *) dw)->pData);
      if (((AEENotify *) dw)->dwMask  == NMASK_CM_PHONE )
      {
        if (event_info->event == AEECM_EVENT_PH_OPRT_MODE)
        {
          if (event_info->event_data.ph.oprt_mode != AEECM_OPRT_MODE_NET_TEST_GW)
          {
            (void) ISHELL_EndDialog(pMe->a.m_pIShell);
            return TRUE;
          }
          else
          { /* Get the subsequent NET_TEST_GW event, need to update the screen now */
            UpdateTCScreen(pMe);
            return TRUE;
          }
        }
      }
    }
    return FALSE;

  case EVT_COMMAND:
    CORE_MSG ("TCTestModeDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("TCTestModeDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;
#endif // FEATURE_TC
#endif //(defined(FEATURE_WCDMA)|| defined(FEATURE_GSM))

  default:
    return FALSE;
  }

}

/*===========================================================================
FUNCTION PLMNListDlgEventHandler

DESCRIPTION
  Handle PLMN List Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
//lint -save -esym(715, pUser) -esym(715, evt)
boolean PLMNListDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  sys_plmn_id_s_type plmn_id1;
  sys_plmn_id_s_type plmn_id2;
  AECHAR *pszBuf ;
  boolean bDisplayRAT;
  int i, j, k;
  boolean bMatch;
  boolean bReturnStatus;
  IMenuCtl *pICurrentMenu;
  IDialog *pIDialog;
  CCoreApp *pMe = (CCoreApp*) pUser;
  uint32 dwData;

  PRINT_FUNCTION_NAME();

  if(NULL == pMe)
  {
    CORE_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);

  }

  switch(evt)
  {
  case EVT_DIALOG_START:
    CORE_MSG ("PLMNListDlgEventHandler: EVT_DLG_START", 0, 0, 0);
    pIDialog = (IDialog*) dw;
    if((pIDialog == NULL) || (IDD_PLMN_LIST != w))
    {
      CORE_ERR("Not PLMNList Dlg", 0, 0, 0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_PLMN_LIST);

    if(pICurrentMenu == NULL)
    {
      CORE_ERR("pICurrentMenu NULL", 0, 0, 0);
      return FALSE;
    }

    bReturnStatus = IMENUCTL_DeleteAll(pICurrentMenu);

    if (bReturnStatus == FALSE)
    {
      CORE_ERR ("Failed to delete-all ", 0,0,0);
    }

    j = IDL_PLMN1;
    pszBuf = (AECHAR*) MALLOC(TEXT_BUFFER_SIZE*sizeof(AECHAR));

    if(pszBuf == NULL)
    {
      CORE_ERR("Malloc Failed", 0, 0, 0);
      return FALSE;
    }

    bDisplayRAT = (pMe->m_nNWSelectionMode == APPCOMFUNC_NW_SEL_MAN_PLMN_WRAT);

    for(i = 0 ; i < (int) pMe->m_pDetailedPLMNList->length ; i++ )
    {
      bMatch = FALSE;
      //Remove the duplicates if Not With RAT;
      if(!bDisplayRAT)
      {
        plmn_id1.identity[0] = pMe->m_pDetailedPLMNList->info[i].plmn.identity[0];
        plmn_id1.identity[1] = pMe->m_pDetailedPLMNList->info[i].plmn.identity[1];
        plmn_id1.identity[2] = pMe->m_pDetailedPLMNList->info[i].plmn.identity[2];

        for( k = 0; k < i; k++)
        {
          plmn_id2.identity[0] = pMe->m_pDetailedPLMNList->info[k].plmn.identity[0];
          plmn_id2.identity[1] = pMe->m_pDetailedPLMNList->info[k].plmn.identity[1];
          plmn_id2.identity[2] = pMe->m_pDetailedPLMNList->info[k].plmn.identity[2];
          //TODO: Change sys_plmn_match function with some Ixxx() function.
          if(sys_plmn_match(plmn_id1, plmn_id2))
          {
            bMatch = TRUE;
            break;
          }
        }
      }
      if(!bMatch)
      {
        CoreUtil_FormatPLMN(pszBuf, TEXT_BUFFER_SIZE, &pMe->m_pDetailedPLMNList->info[i], bDisplayRAT);
        //Data for the item is the index into Detailed PLMN List.
        IMENUCTL_AddItem(pICurrentMenu, gpszResourceFile, 0, (uint16) j, pszBuf, (unsigned long) i);
        j++;

      }
      if( j == (IDL_PLMN1 + MAX_PLMN_LIST_ITEMS))
      {
        CORE_ERR("Maximum 20 Items allowed in PLMN List", 0, 0, 0);
        break;
      }
    }
    SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
    FREE(pszBuf);
    return TRUE;

  case EVT_COMMAND:
    CORE_MSG ("PLMNListDlgEventHandler: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL)
    {
      CORE_ERR("pIDialog NULL",0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_PLMN_LIST);

    if(pICurrentMenu == NULL)
    {
      CORE_ERR("pICurrentMenu NULL", 0, 0, 0);
      return FALSE;
    }

    IMENUCTL_GetItemData(pICurrentMenu,w,&dwData);
    ISHELL_EndDialog(pMe->a.m_pIShell);

    if(dwData >= pMe->m_pDetailedPLMNList->length)
    {
       MSG_ERROR("Invalid Data",0,0,0);
       return FALSE;
    }
    if(SetNetworkManualSelection(pMe, dwData))
    {

      ISHELL_SetTimer(pMe->a.m_pIShell,
                          SELECTIONVERIFICATIONTIME,
                          (PFNNOTIFY) EndCurrentDlg,
                          (uint32*) pMe);
    }
    else
    {
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
    }
    pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    FREEIF(pMe->m_pDetailedPLMNList);

    return TRUE;

  case EVT_KEY:
    CORE_MSG ("PLMNListDlgEventHandler: EVT_KEY", 0, 0, 0);
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
    case AVK_POUND:
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    case AVK_SEND:
    case AVK_MESSAGE:
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE;

    case AVK_END:
      pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
      FREEIF(pMe->m_pDetailedPLMNList);
      return FALSE;

    case AVK_CLR:
      //Restart the IdleMenuTimer
      //End this dialog
      pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
      FREEIF(pMe->m_pDetailedPLMNList);

      return CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell));

    default:
      return FALSE;
    }

  case EVT_DIALOG_END:
    CORE_MSG ("PLMNListDlgEventHandler: EVT_END", 0, 0, 0);

    return TRUE;


  default:
    return FALSE;
  }
#else
    CORE_ERR ("No Handling PLMNListDlgEventHandler", 0, 0, 0);
    return FALSE;
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */
}
//lint -restore

/*===========================================================================
FUNCTION PLMNSearchingEventHandler

DESCRIPTION
  Handle PLMN Searching Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
//lint -save -esym(715, pUser) -esym(715, evt) -esym(715,dw)
boolean PLMNSearchingDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  CCoreApp * pMe = (CCoreApp*) pUser;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif


  switch(evt)
  {
  case EVT_DIALOG_START:
    CORE_MSG ("PLMNSearchingDlgEventHandler: EVT_DLG_START", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    return TRUE;


  case EVT_KEY:
    CORE_MSG ("PLMNSearchingDlgEventHandler: EVT_KEY", 0, 0, 0);
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
    case AVK_POUND:
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    case AVK_SEND:
    case AVK_MESSAGE:
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE;


    case AVK_CLR:
    case AVK_END:
      //Cancel the GetNetworks
      //End this dialog
      CoreUtil_CancelGetNW(pMe);
      return CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell));

    default:
      return FALSE;
    }



  case EVT_DIALOG_END:
    CORE_MSG ("PLMNSearchingDlgEventHandler: EVT_DLG_END", 0, 0, 0);
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        /*Do not display the "Select operator" Dialog immediately.*/
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) SetSelOperatorDisplay, pMe);
#endif


    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
   CORE_ERR ("No Handling PLMNSearchingEventHandler", 0, 0, 0);
   return FALSE;
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

}
//lint -restore

/*===========================================================================
FUNCTION CUGGroupIDDlgEventHandler

DESCRIPTION
  Handle CUG Group ID Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CUGGroupIDDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  ITextCtl *pITextCtl;

  PRINT_FUNCTION_NAME();

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);

  }

  switch(evt)
  {
  case EVT_DIALOG_START:
    {
      AECHAR pszGroupId[6];
      uint16 grpID ;
      char buf[6];
      CORE_MSG ("CUGGroupIDDlg: EVT_DLG_START", 0, 0, 0);
      pIDialog = (IDialog*) dw;
      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CUG_GROUP_ID);
      IMENUCTL_DeleteAll(pIMenuCtl);
      IMENUCTL_AddItem(pIMenuCtl, gpszResourceFile, IDS_OK, IDL_CUG_GROUP_ID_OK, NULL, 0);
      SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
      pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CUG_GROUP_ID);
      grpID = GetCUGGroupID(pMe) ;
      (void) SNPRINTF(buf, sizeof(buf), "%d", grpID);
      (void) STRTOWSTR(buf, pszGroupId, sizeof(pszGroupId));
      ITEXTCTL_SetText(pITextCtl, pszGroupId, -1);
      ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl) | TP_FRAME);
      ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
      ITEXTCTL_SetMaxSize(pITextCtl, 5);
    }
    return TRUE;

  case EVT_KEY:
    CORE_MSG ("CUGGroupIDDlg: EVT_KEY, key=%d", w, 0, 0);
    switch(w)
    {

    case AVK_CLR:
      return CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell));

    default:
      return FALSE;
    }


  case EVT_COMMAND:
    CORE_MSG ("CUGGroupIDDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    switch(w)
    {
    case IDL_CUG_GROUP_ID_OK:
      {
        //Store the value and end the dialog.
        char buf[6];
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CUG_GROUP_ID);
        (void) WSTRTOSTR(ITEXTCTL_GetTextPtr(pITextCtl), buf, sizeof(buf));
        ISHELL_EndDialog(pMe->a.m_pIShell);
        if(SetCUGGroupID(pMe, (uint16) ATOI(buf)) == FALSE)
        {
          DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        }
      }
      return TRUE;

    default:
      break;
    }
    break;

  case EVT_DIALOG_END:
    CORE_MSG ("CUGGroupIDDlg: EVT_DLG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }

   return FALSE;
}

/*===========================================================================
FUNCTION LPMDlgEventHandler

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean LPMDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  IStatic * pIStaticText = NULL;
  AEEChargerStatus chgStatus;
  uint32 wID = IDC_STATIC_LPM;


  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch(evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("LPMDlg: EVT_DIALOG_START", 0, 0, 0);

    //Check to See if there is any battery. Display appropriate message
    chgStatus = IBATT_GetChargerStatus(pMe->m_pIBatt);

    pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, wID);
    if (pIStaticText != NULL)
    {
      if(chgStatus == AEECHG_STATUS_NO_BATT || chgStatus == AEECHG_STATUS_NO_CHARGE_NO_BATT)
      {
        AECHAR pBuf[TEXT_BUFFER_SIZE];
        if (ISHELL_LoadResString (pMe->a.m_pIShell, gpszResourceFile, IDS_NO_BATTERY,
                            pBuf, TC_BUFFER_SIZE * sizeof(AECHAR)) != 0) {
          ISTATIC_SetText (pIStaticText, NULL, pBuf, AEE_FONT_BOLD, AEE_FONT_BOLD);
        }
      }
      (void) SetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */
      /* Force the control to redraw */
      ICONTROL_Redraw((IControl*)pIStaticText);
      return TRUE;
    }
    else
    {
      return FALSE;
    }

  case EVT_KEY:
    CORE_MSG ("LPM: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
    case AVK_END:
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    CORE_MSG ("LPMDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("LPMDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End LPMDlgEventHandler */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION DisplayPLMNListAgain

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayPLMNListAgain(CCoreApp *pMe)
{
  if(pMe->m_pCopyDetailedPLMNList == NULL)
  {
    CORE_ERR("Null Pointer", 0, 0, 0);
    return;
  }

  if( GetNetworkSelect(pMe) == AEECM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    /* Free any stale Memory before allocating new */
    FREEIF(pMe->m_pDetailedPLMNList);
    pMe->m_pDetailedPLMNList = (AEECMDetailedPLMNList*) MALLOC(sizeof(AEECMDetailedPLMNList));

    if(pMe->m_pDetailedPLMNList == NULL)
    {
      CORE_ERR("Null Pointer", 0, 0, 0);
      return;
    }

    MEMCPY(pMe->m_pDetailedPLMNList,pMe->m_pCopyDetailedPLMNList,  sizeof(AEECMDetailedPLMNList));

    if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile, IDD_PLMN_LIST, NULL)) == FALSE)
    {
      CORE_ERR("Create IDD_PLMN_LIST failed", 0, 0, 0);
    }

  }
}

/*===========================================================================
FUNCTION SearchManualPLMNList

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SearchManualPLMNList(CCoreApp *pMe)
{
   if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, 
        gpszResourceFile, IDD_SCAN_NETWORK, NULL)) == FALSE)
    {
      CORE_ERR("Create IDD_SCAN_NETWORK failed", 0, 0, 0);
    }
}
#endif
/*===========================================================================
FUNCTION ScanNetworkDlgEventHandler

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ScanNetworkDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  IStatic * pIStaticText = NULL;
#endif

  if(pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch(evt){
    case EVT_DIALOG_START:
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
      CORE_MSG ("ScanNetworkDlg: EVT_DIALOG_START", 0, 0, 0);
      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_SCAN_NETWORK);
      if(pIStaticText != NULL)
      {
        (void) SetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */
        /* Force the control to redraw */
        ICONTROL_Redraw((IControl*)pIStaticText);
        return TRUE;
      }
      else
      {
        return FALSE;
      }
#else
      return FALSE;
#endif

    case EVT_KEY:
      CORE_MSG ("ScanNetworkDlg: EVT_KEY", 0, 0, 0);
      switch(w){
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
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE;

        case AVK_CLR:
        case AVK_END:
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
          //Cancel the GetNetworks
          CoreUtil_CancelGetNW(pMe);
#endif
          //End this dialog
          return CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell));

        default:
          return FALSE;
      }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    case EVT_COMMAND:
      CORE_MSG ("ScanNetworkDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      
      if(w == IDL_SCANNETWORK_YES)
      {  
        // Show the searching dialog

        /* Free any stale Memory before allocating new */
        FREEIF(pMe->m_pDetailedPLMNList);
        FREEIF(pMe->m_pCopyDetailedPLMNList);
        pMe->m_pDetailedPLMNList = (AEECMDetailedPLMNList*)
          MALLOC(sizeof(AEECMDetailedPLMNList));

        pMe->m_pCopyDetailedPLMNList = (AEECMDetailedPLMNList*)
          MALLOC(sizeof(AEECMDetailedPLMNList));

        pMe->m_nCallback.pfnCancel = NULL;
        pMe->m_nCallback.pfnNotify = (PFNNOTIFY) CoreUtil_GetNetworksCb;
        pMe->m_nCallback.pNotifyData = (void*) pMe;

        pMe->m_nNWSelectionMode  = APPCOMFUNC_NW_SEL_MAN_PLMN;

        if(CheckAEEReturnStatus(ICM_GetNetworks(pMe->m_pICM,
          AEECM_NETWORK_LIST_AVAILABLE,
          &pMe->m_PhError,
          pMe->m_pDetailedPLMNList,
          sizeof(AEECMDetailedPLMNList),
          &pMe->m_nCallback)) == FALSE)
        {
          pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
          FREEIF(pMe->m_pDetailedPLMNList);
          FREEIF(pMe->m_pCopyDetailedPLMNList);
          ISHELL_EndDialog(pMe->a.m_pIShell);
          DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
          return TRUE;
        }
        else //Searching....
        {
          // These updates take a long time. Show the searching dialog
          ISHELL_EndDialog(pMe->a.m_pIShell);
          return CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell,
            gpszResourceFile, IDD_PLMN_SEARCHING, NULL));
        }
      }
      else //if w == IDL_SCANNETWORK_NO
      {
        ISHELL_EndDialog(pMe->a.m_pIShell);
        return TRUE;
      }
#endif //#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

    case EVT_DIALOG_END:
      CORE_MSG ("ScanNetworkDlg: EVT_DIALOG_END", 0, 0, 0);
      return TRUE;

    default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
}


/*===========================================================================
FUNCTION ALSLineSelectDlgEventHandler

DESCRIPTION
  Handle ALS Line Selection radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ALSLineSelectDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
#ifdef FEATURE_ALS
  int nReturnStatus;
  CCoreApp * pMe = (CCoreApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  switch (evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("ALSLineSelectDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    CORE_MSG ("ALSLineSelectDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return CheckAEEReturnStatus(nReturnStatus);

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
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    {
      int nReturnStatus;
      AEECardPinStatus pin_status;

      CORE_MSG ("ALSLineSelectDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* user makes (new) selection */
      /* disable the timer that shows users their newly selected option */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);
      /* Determine state of security pin/nv verification */

      nReturnStatus = ICARD_GetPinStatus(pMe->m_pICard, AEECARD_PIN2, &pin_status);

      if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
      {
        CORE_ERR("ICARD_GetPinStatus Failed", 0, 0, 0);
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER,
                    EVT_PIN_VERIFY_FAIL, 0L);
        return TRUE;
      }
      pMe->m_sPIN2_lock_state = pin_status.lock_state;

      switch(pMe->m_sPIN2_lock_state) {
      case AEECARD_PIN_DISABLED:
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER,
                    EVT_PIN_VERIFY_SUCCESS, 0L);
        return TRUE;

      case AEECARD_PIN_ENABLED:
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_PIN2_REQUEST, NULL);
        return CheckAEEReturnStatus(nReturnStatus);

      case AEECARD_PIN_VERIFIED:
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER,
                    EVT_PIN_VERIFY_SUCCESS, 0L);
        return TRUE;

      case AEECARD_PIN_BLOCKED:
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_PUK2_REQUEST, NULL);
        return CheckAEEReturnStatus(nReturnStatus);

      case AEECARD_PIN_PERM_DISABLED:
      case AEECARD_PIN_UNKNOWN:
      default:
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER,
                    EVT_PIN_VERIFY_FAIL, 0L);
        return TRUE;
      }
    }

  case EVT_USER:
    CORE_MSG ("ALSLineSelectDlg: EVT_USER", 0, 0, 0);
    /* user slot preference setting failure */
    switch(w) {
    case EVT_PIN_VERIFY_SUCCESS:
      {
        IDialog*  pIActiveDialog;
        IMenuCtl* pICurrentMenu;
        uint16    wItemID = 0;
        uint16    wCtrlID = 0;

        /* pin 2 verification success */
        pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if(pIActiveDialog == NULL) {
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }

        wCtrlID = GetDlgMenuCtlID(pMe, pIActiveDialog);
        if(wCtrlID == 0) {
          CORE_ERR ("Null pointer ", 0,0,0);
          return FALSE;
        }
        pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, wCtrlID);
        if(pICurrentMenu == NULL) {
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }

        /* Get the selected item */
        wItemID = IMENUCTL_GetSel((IMenuCtl*)pICurrentMenu);

        if (UpdateSetting(pMe, wItemID, (IMenuCtl*)pICurrentMenu)) {
          /* Let the update image be shown for 2 seconds before going back to
          the previous screen */
          nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                          SELECTIONVERIFICATIONTIME,
                          (PFNNOTIFY) EndCurrentDlg,
                          (uint32*) pMe);
          (void)CheckAEEReturnStatus(nReturnStatus);
        }
        else {
          DisplayMessageDlg(pMe, IDS_ALS_LINE_SELECTION_ERROR, 3);
        }
      }
      return TRUE;

    case EVT_PIN_VERIFY_FAIL:
      DisplayMessageDlg(pMe, IDS_ALS_LINE_SELECTION_ERROR, 3);
      return TRUE;

    default:
      break;
    }

    return FALSE;

  case EVT_DIALOG_END:
    CORE_MSG ("ALSLineSelectDlg: EVT_DIALOG_END", 0, 0, 0);
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
  CORE_MSG("ALSLineSelectionDlg Not Supported: Evt: 0x%x", evt, 0, 0);
  return FALSE;
#endif /*FEATURE_ALS */
} /* End ALSLineSelectDlgEventHandler */


/******************************************************************************/
/*******                    Update Contrast                             *******/
/******************************************************************************/
static void UpdateContrast(CCoreApp * pMe, int value)
{
  char temp[20];
  AECHAR szBuf[MAX_STR_SIZE] = {'0'};
  int contrastValue;
  IStatic * pTextCtrl;
  IDialog * pIActiveDialog;

  if (pMe == NULL) {
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return;
  }

  contrastValue = pMe->m_byDispContrast + value;

  if(contrastValue > pMe->m_byContrastMax)
  {
    contrastValue = pMe->m_byContrastMax;
  }
  if(contrastValue < pMe->m_byContrastMin)
  {
    contrastValue = pMe->m_byContrastMin;
  }

  disp_set_contrast(contrastValue);
  pMe->m_byDispContrast = contrastValue;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIActiveDialog == NULL) {
    CORE_ERR ("Null pointer GetActiveDialog", 0,0,0);
    return;
  }

  pTextCtrl = (IStatic*)IDIALOG_GetControl(pIActiveDialog, IDC_STATIC_CONTRAST);
  if (pTextCtrl == NULL) {
    CORE_ERR ("Null pointer from GetControl", 0,0,0);
    return;
  }

  ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile, IDS_CONTRAST_VALUE, szBuf, sizeof(szBuf));
  (void)SNPRINTF(temp, ARR_SIZE(temp), " %d/%d", pMe->m_byDispContrast, pMe->m_byContrastMax);
  //Add value/max to end of "Contrast Value"
  (void)STRTOWSTR(temp, &szBuf[WSTRLEN(szBuf)], sizeof(szBuf) - WSTRLEN(szBuf));

  ISTATIC_SetText(pTextCtrl, NULL, szBuf, 0, AEE_FONT_BOLD);
  ISTATIC_Redraw(pTextCtrl);
}

static boolean    InitContrastDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  AEERect  rectDisp, rect;
  IStatic * pImageCtrl;
  IStatic * pTextCtrl;

  if ( pIActiveDialog == NULL || pMe == NULL) {
    CORE_ERR ("Null pointer pIActiveDialog or pMe", 0,0,0);
    return FALSE;
  }

  pMe->m_byDispContrast = GetContrast(pMe);

  //Get screen clip rect
  IDISPLAY_GetClipRect(pMe->a.m_pIDisplay, &rectDisp);

  pTextCtrl = (IStatic*)IDIALOG_GetControl(pIActiveDialog, IDC_STATIC_CONTRAST);
  if (pTextCtrl == NULL) {
    CORE_ERR ("Null pointer from GetControl", 0,0,0);
    return FALSE;
  }

  pImageCtrl = (IStatic*)IDIALOG_GetControl(pIActiveDialog, IDC_STATIC_CONTRAST_BMP);
  if (pImageCtrl == NULL) {
    CORE_ERR ("Null pointer from GetControl", 0,0,0);
    return FALSE;
  }

  //Move text to bottom of screen, make width the screen width
  ISTATIC_GetRect(pTextCtrl, &rect);
  rect.y = rectDisp.dy - rect.dy;
  rect.dx = rectDisp.dx;
  ISTATIC_SetRect(pTextCtrl, &rect);

  //Make width of image the screen width
  ISTATIC_GetRect(pImageCtrl, &rect);
  rect.dx = rectDisp.dx;
  ISTATIC_SetRect(pImageCtrl, &rect);

  UpdateContrast(pMe, 0);

  return TRUE;
}

//lint -restore  "unusual pointer cast 740"

