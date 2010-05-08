/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This is "user interface central," the main dispatcher.  It surrounds the
  state machine and turns all incoming events and signals into events for
  the state machine.  It also accepts input from the handset, buffers it,
  and makes appropriate beeps.

  In addition, several minor functions used by both UI.C and the state
  machine are included here.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ui_task should be created by MC before anyone calls ui_cmd.
  NV, HS, SND, and DIAG should be started before ui_task.
  All other functions are internal to UI.

  Copyright (c) 1999 through 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/core_ui.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/09   suj     Handle backlight logic in Mango UI as is done for 3GUI.
04/20/09   Gopi    Moved the functions which are not needed for Mango under "ifndef FEATURE_MANGO_UI"
04/04/09   suj	    Notify EVT_HEADSET to BREW in case of Mango UI.
12/29/08   kcp     CR 163704  M7501BXDCKAASD5716  PowerDown
11/07/08   kcp     Taskkey on 6290 does not seem to be mapped correctly.
11/04/08   kcp     LCD refresh issue
09/26/08   kcp     optimizing the LCD 2.85 current
09/22/08   kcp     Add voting for sleep when backlight turns off
05/19/08   cvs     Remove deprecated/unneeded header
10/31/07   sgo     Added support for HS_FLIP_*_K to HDk specific handle_keys
09/05/07   jks     Mainlined Brew App Coordination. Proxy App to represent App
                   from Alternate Environments.
08/27/07   jks     Mainlined support for MediaFlo TV key
08/24/07   jks     Fixed dependency compile issue.
08/21/07   jks     Mainlined changes for set TTYHeadset device status
                   in HDK build when FEATURE_UIXSND_IMPLEMENTATION is defined.
08/17/07   cvs     Add function to get backlight duration
08/15/07   cvs     fix RefreshAppContent
07/16/07   jks     Added featurization for SND_DEVICE_SDAC
04/06/07   jas     Remove code that sends WLAN module to low power mode (LPM)
04/05/07   mjv     OEMBattery is now notified immediately when external power
                   is added or removed (HDK only).
03/26/07   cvs     Decouple clock scaling + sleep voting.
03/16/07   mjv     Fix compile error when defined(FEATURE_UIONE_HDK) and
                   !defined(FEATURE_KEYPAD_MULTI_KEY)
03/14/07   jas     Reworked the display refresh based on logical display config
03/12/07   cvs     Add a temp key for HSED.  Real key to come from BREW FRB.
03/01/07   cvs     Remove broken sleep voting
02/23/07   jas     Refactoring for HDK builds
02/12/07   mjv     Featurized more legacy UI code with FEATURE_UIONE_HDK
01/24/07   cvs     Fix compile warnings
12/06/06   cvs     Fix compile warnings
11/14/06   ypmw    Fix audio mutes due to key press (despite DTMF and key tone off)
11/13/06   cvs     Mainlining HDK changes
11/09/06   sb      Added support for using KxIpc under FEATURE_IXCONTENTSVC
09/06/06   jks     Added FEATURE_NEW_SLEEP_API and removed redundant code
                   for FEATURE_SLEEP
09/22/06   cvs     Mainlining BREW 4.0 changes
09/08/06   cvs     Fix key release handling on multikey targets
09/08/06   cvs     Fix compile problem with previous checkin
09/07/06   adb     Added keybeeps and LCD support for Pandora
08/31/06   dd      Removed support for FEATURE_DISABLE_BREW_DURING_GPS.
08/25/06   jas     Fixing code to turn off LCD
07/28/06   cvs     Add USB driver signals
07/12/06   cvs     Fix compile warnings
06/14/06   jas     Adding support for IPC
05/17/06   ck/jas  Tweaking the SD card polling
03/14/06   cvs     more keypress logging
02/17/06   jas     Fix compiler warning on gps_disable_brew.
01/30/06   jas     Fixing featurization of code for new overlay
01/27/06   jas     Fixing featurization of previous LCD timer fix.
01/25/06   jas     Modified sleep voting to not sleep during BREW dispatch
01/20/06   jas     Fixed LCD timer code to work with dual-display targets.
01/19/06   jas     Added fix to push content out to display again after turning
                   display on.  Updates occurred before display was turned on.
01/05/06   cvs     Fix key handling for USB carkit
12/07/05   ak      Send a PWR Key if we do not receive a KEY RELEASE for an END key.
11/08/05   cvs     Fix featurizations
10/18/05   cvs     Added PTT key support
10/07/05   RI      Added GetDisplayStatus() to get the status of the LCD
09/22/05   cvs     Support for Stereo USB Carkit
07/27/05   AT      Remove compile warning for unused variable.
07/01/05   cvs     Fix key beeps for multi keys, and use callbacks
06/10/05   cvs     Support for Smart USB Carkit
06/01/05   trc     Add more HS_* to AVK_* key mappings and remove dependency
                   on obsolete FEATURE_EXTENDED_KEYS
05/24/05   aps     code cleanup. updated indentation and if-braces in
                   accordance with coding standards.
05/19/09   aps     Added CoreTask_GetSilenceAll() & CoreTask_SetSilenceAll()
05/10/05   cvs     Changes to powerdown to use SendEvent
05/10/05   cvs     Changes to powerdown to not call AEE_Exit from AEE_Dispatch
05/10/05   cvs     Get powerdown done in one call from TMC
05/03/05   CH      Change which removes bridle starting in BREW 3.1.4
04/27/05   hxw     Added FEATURE_DISABLE_BREW_DURING_GPS for GPS RF dev on 6800
02/25/05   sun     Added Carkit Features under FEATURE_USB_CARKIT
01/27/05   tjc     Added keybeeps for keys on Slingshot that don't necessarily
                   have a function yet
01/21/05   cvs     Coreapp no longer creates symlinks
01/20/05   trc     Added CoreTask_GetGameMode()
12/21/04   sun     Added support for new keys
12/20/04   cvs     Remove duplicate variable declaration in ThinUI
12/17/04   cvs     Add sample multimedia ringers
12/13/04   ssc     Changed include file name case to match the Linux names
                   of the files. Mismatches in the case break AMSS builds
                   on Linux servers.
12/06/04   ak      Removed Compiler Warning
11/23/04   sun     Added support for USB HFK
11/15/04   adm     Added shutter sound filed to be loaded directly onto EFS.
11/11/04   sun     The display will now turned off from UI.
11/10/04   cvs     Allow files in filesystem to override ROM files
11/02/04    AT     Changed name of #ifdef to match BTG's.
10/29/04   jas     Updates for secondary display
10/28/04    AT     Integrated QIS changes for handling the NET signal.
09/14/04    sun    Changed LPM to use HS macro instead
09/09/04    ak     Fixed Acquisition failure when charger connected
08/30/04    AT     Keep the CPU from sleeping whenever AEE_dispatch returns
                   non-zero
08/16/04   tml     Added user key press notification to catapp
08/02/04    gr     Eliminated unnecessary symlink creation. Added a crude
                   space reservation mechanism for symlinks.
07/29/04   jas     Added support for clamshell flip events
07/15/04   dle     Moved shutter sound file location.
07/08/04   sun     Stop the sound if it is started at key press
06/25/04   sun     Send SubscriptionAvailable before LPM
06/23/04   sun     Send a separate event to change the operating mode
06/15/04   sun     Romized Qcamera files
06/09/04   sun     Added the Raptor Changes
06/09/04   sun     Null pointer checks
06/01/04   sun     Fixed Backlights
05/26/04   sun     Renamed ems files
05/25/04   PA      More directories to be created on powerup.
05/24/04   sun     All directories were not being created on powerup.
05/13/04   AT      Added Changes for Thin UI.
05/13/04   sun     Added LPM Changes
05/07/04   sun     Fixed TTY
05/06/04   PMD     Added code to Romize EMS Contents
04/29/04   sun     Fixed baclight duration
04/16/04   sun     Made Backlight/kepad light modifications
04/02/04   sun     Added Raptor Changes
03/08/04   sun     Fixed Compile Errors
03/08/04   sun     UI Task Changes due to IPC
03/04/04   sun     Fixed the backlight settings
03/01/04   sun     Check Game Mode before waking from Sleep
02/25/04   ram     Integration with IBatt Interface.
02/20/04   pn      ROMized Bluetooth bar file
02/20/04   NNG     Added support for volume up/down keys.
02/17/04   sun     Mainlined disp_on
02/11/04   wx      Added support for Java external AMS app romized bar file
01/30/04   ak      InitResources() is now called before AEE_Init()
01/29/04   ram     Replace AVK_SPEAKER with EVT_SPEAKERPHONE_TOGGLE
01/28/04   ram     Add speaker-phone functionality.
01/16/04   kar     Vote not okay to sleep only if BREW is throttling
12/18/03   ram     Changed InitResources function and removed lint errors.
12/11/03   kar     Handle RUIM and SIO signals
12/05/03    lz     Changed location of mms_app_res.bar and wap_app_res.bar
                   to be brew/mod/tmt instead of a separate app directory
                   since they share on mif file.
12/01/03   ram     Romize DataStats bar file for BREW3.0
11/25/03   sun    Added CFGI items for all external devices
11/19/03   ak    BREW 3.0 directory structure change
11/19/03   ak    Jump table initialized before AEE_Init
11/17/03   NNG     Romized videophone app bar file.
11/13/03   att     is call allowed changes
11/05/03   is      Fixed GPSONE feature
11/05/03   is      Change GPSONE features
11/05/03   PMD     Added OEMWMS_MsgInfoCacheCb Registration in ui_init
11/04/03   ram     Fixed backlight isuue
10/29/03   ram     Support for romizing bar file for datastats app
10/23/03   PA      Changed ICM CLSID to AEECLSID_CM
10/21/03   sun      Fixed compile warnings
10/20/03   kar     Dyanmic clk freq changing cleanup
10/20/03   BPW     Added support for Emergency Calls
10/16/03   sun     Added support for OTASP
10/09/03   sun     Fixed Compile Error
09/26/03   tml     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "OEMFeatures.h"

#ifdef FEATURE_UIONE_HDK
#include "OEMConfig.h"
#include "OEMDeviceInfoEx.h"
#include "OEMBattery.h"
#endif // FEATURE_UIONE_HDK

#include "ui.h"
#include "ui_base.h"
#include "msg.h"
#include "err.h"

#include "hsui.h"
#ifdef FEATURE_UIXSND_IMPLEMENTATION
#include "uixsnd.h"
#endif // FEATURE_UIXSND_IMPLEMENTATION
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "ghdi_nvmem.h"
#endif

#if defined(FEATURE_UIONE_HDK) && defined(FEATURE_UIXSND_IMPLEMENTATION) && defined(FEATURE_TTY)
#ifndef FEATURE_BMP 
//fixing the compilation error in BMP as this file not present
#include "CustomConfigItems.h"
#endif
#include "CustomOEMConfigItems.h"
#include "AEEConfig.h"
#endif

#include <string.h>

#ifdef FEATURE_TOUCHSCREEN
#include "AEETOUCHSCREEN.bid"
IQI * piTouchscreen = NULL;
#endif // FEATURE_TOUCHSCREEN

#include "BREWVersion.h"
#include "OEMFeatures.h"

#include "AEE_OEM.h"
#include "AEE.h"

#if MIN_BREW_VERSION(4,0)
#include <l4/thread.h>
#include "AEEISignalQ.h"
#include "AEESignalQ.bid"

/*These should go in a brew header*/
extern void k1ExHandler_SetExceptionHandlerOf(L4_ThreadId_t thread_id);
extern IEnv *gpiRexEnv;

#endif

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#if !MIN_BREW_VERSIONEx(3,1,4)
#include "BRIDLEInit.h"
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */
#endif //FEATURE_BREW_3_0

#include "AEEShell.h"
#include "AEEStdLib.h"
#ifdef FEATURE_MANGO_UI
#include "AEEDeviceNotifier.h"
#endif 
#include "core_ui.h"
#ifndef FEATURE_UIONE_HDK
#include "CoreApp.h"
#include "CoreUtils.h"
#endif /* !FEATURE_UIONE_HDK */

#ifndef FEATURE_UIONE_HDK
#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#endif /* FEATURE_UIONE_HDK */

#ifdef FEATURE_UI_PBM
#include "uiupbm.h"
#endif

#include "fs_public.h"

#if defined(FEATURE_SLEEP) || defined(FEATURE_NEW_SLEEP_API)
#include "sleep.h"
#endif

#ifdef FEATURE_WMS_APP
#include "wms.h"
#include "OEMWMS.h"
#endif

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMEvent.h"
#endif

#ifdef FEATURE_JAVA
#include "java_config.h"
#endif

#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_APP_CATAPP
#include "catapp.h"
#include "CAT.BID"
#endif /*FEATURE_APP_CATAPP */
#endif /* !FEATURE_UIONE_HDK */

#include "task.h"
#include "dog.h"
#include "tmc.h"

#include"pmapp.h"
#ifndef FEATURE_UIONE_HDK
#include "AppComFunc.h"
#endif /* !FEATURE_UIONE_HDK */

#ifdef FEATURE_USB_CARKIT
#include "usbcc.h"
#endif

#if !MIN_BREW_VERSION(4,0)
#include "OEMDisplayDev.h"
#endif

#ifdef FEATURE_EFS_HOTPLUG
#include "fs_hotplug.h"
#endif

#ifdef FEATURE_REX_IPC
#include "ipcns.h"
#endif

/* Brew App Coordination. Proxy App to represent App from Alternate Env.
 * and AltEnvShell Extension for BREW App developers who want to deal with
 * Alternate Environment applications (start/stop/postevent) */

#if defined(FEATURE_BREWAPPCOORD)
#include "BrewAppCoordLib.h"
#endif

#ifdef FEATURE_IXCONTENTSVC
#include "ixipc.h"
#include "IxContentInit.h"
#endif


#include "AEECLSID_LOGICALDISPLAYCFG.BID"

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Ring buffer for holding keycodes.  When read index == write index, it is
** empty.  When read index is 1 ahead of write index, the buffer is full.
** Note:  this "burns" 1 datum (a byte) in order to tell full from empty.
*/
#define UI_KEY_BUF_SIZE 128      /* Max keycodes to buffer    */
#define UI_KEY_BUF_MASK 0x7f    /* Mask for modulus in ring  */

#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
#define BURST_DTMF_LEN 95
#define MAX_SYM_LINK_LENGTH 8 /* Maximum size for the symbolic length e.g. "file12" */

//lint -save -e740
#ifdef FEATURE_OTASP
static boolean ui_pwrdown_complete = FALSE;
#endif
static boolean ui_game_mode = FALSE;
static boolean ui_deepsleep = FALSE;
static boolean ui_burst = FALSE;
static boolean tone_started = FALSE;
static boolean ui_demo_alert = FALSE;
static boolean ui_powerup = FALSE;
static byte ui_backval = FALSE;
static boolean send_keys = FALSE;
static byte ui_backlight_duration = 0;
static boolean ui_silence_key = FALSE; /*silence key status */
#endif
#endif // !FEATURE_UIONE_HDK
static boolean ui_ok_to_sleep = FALSE;
static kpd_handle_type ui_kpd_handle; /* Keypad Handle */
static lcd_handle_type ui_lcd_handle; /* LCD handle */
#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
static boolean ui_core_lpm = FALSE;  /*Current LPM Status*/
static uint32 lcd_timer =0;
static boolean ui_display = TRUE;
static boolean ui_silenceall = FALSE; /* silence all status */
static uint32 uisnd_num_keys_pressed = 0; /* number of keys currently pressed */
static boolean ui_call_pres_previously = FALSE; /*this variable will be used whether the call was previously itself */
#endif
#ifdef T_QSC1100
//this varibale is being introduced to avoid issue 154047
//This varibale will be updated whenevr the backlight status is changed
//and this will be used to voteforsleep iff backlight is off in CoreAppHandleSignals
static boolean gbIsBacklightOn = FALSE;
#endif


#if (defined(FEATURE_TTY) && !defined(FEATURE_MANGO_UI))
static byte ui_tty =0;
#endif

/* Keep track of which key is held down, if any */
hs_key_type  key_down_key = HS_NONE_K;
#endif // !FEATURE_UIONE_HDK



typedef struct {
  hs_key_type        hs_key;      /* HS key              */
  AVKType            aee_vcode;   /* AEE Virtual keycode */
  uint32             dwparam;     /* dwparam to send with event */
} hs_to_aee_key_type;

/* HS to AEE vocdes translation table */
static hs_to_aee_key_type const hs_to_aee_tbl[] =
{
  {HS_0_K,            AVK_0, 0},
  {HS_1_K,            AVK_1, 0},
  {HS_2_K,            AVK_2, 0},
  {HS_3_K,            AVK_3, 0},
  {HS_4_K,            AVK_4, 0},
  {HS_5_K,            AVK_5, 0},
  {HS_6_K,            AVK_6, 0},
  {HS_7_K,            AVK_7, 0},
  {HS_8_K,            AVK_8, 0},
  {HS_9_K,            AVK_9, 0},
  {HS_PWR_K,          AVK_POWER, 0},
  {HS_END_K,          AVK_END, 0},
  {HS_SEND_K,         AVK_SEND, 0},
  {HS_CLR_K,          AVK_CLR, 0},
  {HS_UP_K,           AVK_UP, 0},
  {HS_DOWN_K,         AVK_DOWN, 0},
#ifndef FEATURE_UIONE_HDK
  {HS_SD1_K,          EVT_VOLUME_UP, 0},
  {HS_SD2_K,          EVT_VOLUME_DOWN, 0},
#endif /* FEATURE_UIONE_HDK */
  #ifdef FEATURE_NAV_KEYPAD
  {HS_LEFT_K,         AVK_LEFT, 0},
  {HS_RIGHT_K,        AVK_RIGHT, 0},
  {HS_SEL_K,          AVK_SELECT, 0},
  #endif
  {HS_STAR_K,         AVK_STAR, 0},
  {HS_POUND_K,        AVK_POUND, 0},
  {HS_MSG_K,          AVK_MESSAGE, 0},
  {HS_MENU_K,         AVK_MENU, 0},
  {HS_MUTE_K,         AVK_MUTE, 0},
#ifdef FEATURE_PMIC_HS_SEND_END_DETECT
  {HS_HSED_K,         AVK_PUNC1},
#endif /* FEATURE_PMIC_HS_SEND_END_DETECT */
#ifndef FEATURE_UIONE_HDK
  {HS_EXT_PWR_ON_K,   EVT_EXT_PWR_ON, 0},
  {HS_EXT_PWR_OFF_K,  EVT_EXT_PWR_OFF, 0},
#if defined(FEATURE_SECONDARY_DISPLAY)
  {HS_FLIP_OPEN_K,    EVT_FLIP, TRUE},
  {HS_FLIP_CLOSE_K,   EVT_FLIP, FALSE},
#endif
#ifdef FEATURE_SPEAKER_PHONE
  {HS_PTT_K,          EVT_SPEAKERPHONE_TOGGLE, 0},
#endif
  {HS_HEADSET_K,      EVT_HEADSET_ON, 0},
  {HS_STEREO_HEADSET_K, EVT_STEREO_HEADSET_ON, 0},
  {HS_HEADSET_OFF_K,  EVT_HEADSET_OFF, 0},
  {HS_FREE_K,         EVT_HFK_ON, 0},
  {HS_PHONE_K,        EVT_HFK_OFF, 0},
#endif /* FEATURE_UIONE_HDK */
#ifdef FEATURE_USB_CARKIT
#ifndef FEATURE_UIONE_HDK
  {HS_USB_HFK_ON_K,   EVT_USB_HFK_ON, 0},
  {HS_USB_HFK_OFF_K,  EVT_USB_HFK_OFF, 0},
  {HS_USB_STEREO_HFK_ON_K,   EVT_USB_STEREO_HFK_ON, 0},
  {HS_USB_HFK_GENERIC_BUTTON_K, EVT_USB_GENERIC_BUTTON_PRESS,0},
#endif /* FEATURE_UIONE_HDK */
  {HS_USB_HFK_SEND_BUTTON_K, AVK_SEND,0},
  {HS_USB_HFK_END_BUTTON_K, AVK_END,0},
#ifndef FEATURE_UIONE_HDK
  {HS_USB_HFK_VOICEDIAL_BUTTON_K, EVT_USB_VOICEDIAL_BUTTON_PRESS,0},
  {HS_USB_HFK_RETURN_TO_CRADLE_K, EVT_USB_RETURN_TO_CRADLE,0},
  {HS_USB_HFK_REMOVE_FROM_CRADLE_K, EVT_USB_REMOVE_FROM_CRADLE,0},
#endif /* FEATURE_UIONE_HDK */
#endif
#ifndef FEATURE_UIONE_HDK
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  {HS_SDAC_ON_K,      EVT_SDAC_ON, 0},
  {HS_SDAC_OFF_K,     EVT_SDAC_OFF, 0},
#endif
#endif /* FEATURE_UIONE_HDK */
  {HS_F1_K,           AVK_FUNCTION1, 0},
  {HS_F2_K,           AVK_FUNCTION2, 0},
  {HS_FA_K,           AVK_INFO, 0},
  {HS_SF1_K,          AVK_SOFT1,0},
  {HS_SF2_K,          AVK_SOFT2,0},
  {HS_INFO_K,         AVK_INFO,0},
#ifdef FEATURE_UIONE_HDK
  {HS_TASK_K,         AVK_APP_SWITCH,0},
#else
  {HS_TASK_K,         AVK_FUNCTION,0},
#endif /*FEATURE_UIONE_HDK*/
  {HS_VOL_UP_K,       AVK_VOLUME_UP,0},
  {HS_VOL_DOWN_K,     AVK_VOLUME_DOWN,0},
  {HS_PTT_K,          AVK_PTT,0},
  {HS_TV_K,           AVK_FUNCTION, 0},
  {HS_VIDEO_K,        AVK_VIDEO_PHONE,0},
};


LOCAL struct {
  byte rd_idx;                  /* read index                */
  byte wr_idx;                  /* write index               */
  byte data[ UI_KEY_BUF_SIZE ]; /* data buffer               */
#ifdef FEATURE_KEYPAD_MULTI_KEY
  byte key_parm[UI_KEY_BUF_SIZE];  /* key parm buffer           */
#endif //FEATURE_KEYPAD_MULTI_KEY
} ui_key_buffer;                /* keycode input ring buffer */

#ifdef FEATURE_NEW_SLEEP_API
extern sleep_okts_handle gNewSleepHandle;
#endif

#if MIN_BREW_VERSION(4,0)
static AEECallback KeyHandlerCb = {0}; /* callback info for the key handler */
static boolean bBREWIsInitialized = FALSE; /* Boolean to keep track of whether BREW has been initialized. */
#endif

#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/*===========================================================================
FUNCTION CoreTask_SetSilenceAll

DESCRIPTION
  Set the SILENCEALL flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetSilenceAll(boolean new_value)
{
    ui_silenceall = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetSilenceAll

DESCRIPTION
  Get the SILENCEALL flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetSilenceAll(void)
{
    return ui_silenceall;
}

/*===========================================================================
FUNCTION CoreTask_SetCoreLPM

DESCRIPTION
  Set the LPM flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetCoreLPM(boolean new_value)
{
    ui_core_lpm = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetCoreLPM

DESCRIPTION
  Get the LPM flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetCoreLPM(void)
{
    return ui_core_lpm;
}

/*===========================================================================
FUNCTION CoreTask_SetPwrDnComplete

DESCRIPTION
  Set the ui_pwrdown_complete static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetPwrDnComplete(boolean new_value)
/*lint -esym(715,new_value)*/
{
#ifdef FEATURE_OTASP
  ui_pwrdown_complete = new_value;
#endif
}

/*===========================================================================
FUNCTION CoreTask_GetPwrDnComplete

DESCRIPTION
  Get the ui_pwrdown_complete static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetPwrDnComplete(void)
{
#ifdef FEATURE_OTASP
  return ui_pwrdown_complete;
#else
  return TRUE;
#endif
}

/*===========================================================================
FUNCTION CoreTask_SetGameMode

DESCRIPTION
  Set the ui_game_mode static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetGameMode(boolean new_value)
{
  ui_game_mode = new_value ? TRUE : FALSE;
}

/*===========================================================================
FUNCTION CoreTask_GetGameMode

DESCRIPTION
  Get the ui_game_mode static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetGameMode(void)
{
  return ui_game_mode;
}

/*===========================================================================
FUNCTION CoreTask_SetDeepSleep

DESCRIPTION
  Set the ui_deepsleep static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetDeepSleep(boolean new_value)
{
  ui_deepsleep = new_value;
}

/*===========================================================================
FUNCTION CoreTask_SetBurst

DESCRIPTION
  Set the ui_burst static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetBurst(boolean new_value)
{
  ui_burst = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetBurst

DESCRIPTION
  Get the ui_burst static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetBurst(void)
{
  return ui_burst;
}

/*===========================================================================
FUNCTION CoreTask_SetDemoAlert

DESCRIPTION
  Set the ui_demo_alert static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetDemoAlert(boolean new_value)
{
  ui_demo_alert = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetDemoAlert

DESCRIPTION
  Returns the ui_demo_alert static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetDemoAlert(void)
{
  return ui_demo_alert;
}

/*===========================================================================
FUNCTION CoreTask_SetBackVal

DESCRIPTION
  Set the ui_backval static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetBackVal(byte new_value)
{
  ui_backval = new_value;
  if (!lcd_ctrl_bklit_onoff(ui_lcd_handle, ui_backval)) {
    ERR( "Couldn't set lcd backlight level", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION CoreTask_SetBackDuration

DESCRIPTION
  Set the ui_backval static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetBackDuration(byte new_value)
{
  ui_backlight_duration = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetBackDuration

DESCRIPTION
  Get the ui_backval static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte CoreTask_GetBackDuration(void)
{
  return ui_backlight_duration;
}

/* <EJECT> */
/*===========================================================================
FUNCTION SwitchScrnBacklightOnOff

DESCRIPTION
  Turn backlighting on or off

DEPENDENCIES
  Screen is on

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void SwitchScrnBacklightOnOff ( boolean onoff )
{
   if (TRUE == onoff) {  /* force to turn on lcd backlight */
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
      disp_set_backlight(ui_backval);
#else
      disp_set_backlight(onoff);
#endif
   }
   else         /* force to turn off lcd backlight */
   {
      disp_set_backlight(0);
   }
} /* SwitchScrnBacklightOnOff */


/* <EJECT> */
/*===========================================================================
FUNCTION SwitchHotplugPollingTime

DESCRIPTION
  change hotplug polling time for power savings

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
//lint -e{715}
void SwitchHotplugPollingTime ( boolean onoff )
{
#ifdef FEATURE_EFS_HOTPLUG
   if (TRUE == onoff)
   {
      efs_hotplug_set_polling_interval(5000); /* Poll every 5 seconds */
   }
   else
   {
      efs_hotplug_set_polling_interval (0);    /* Polling is disabled */
   }
#endif
} /* SwitchHotplugPollingTime */

/*===========================================================================
FUNCTION SwitchKeyPadBacklightOnOff

DESCRIPTION
  Turn backlighting on or off

DEPENDENCIES
  Screen is on

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void SwitchKeyPadBacklightOnOff ( boolean onoff )
{
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
   byte val = KEYPAD_BACKLIGHT_LVL_DEFAULT;
#endif

   if (TRUE == onoff) {  /* force to turn on lcd backlight */
#ifdef FEATURE_PMIC_LCDKBD_LED_DRIVER
      keypad_set_backlight(val);
#else
      keypad_set_backlight(onoff);
#endif
   }
   else                 /* force to turn off lcd backlight */
   {
      keypad_set_backlight(0);
   }
}
#endif //FEATURE_MANGO_UI
void CoreSetBklight(boolean onOff)
{
    gbIsBacklightOn = onOff;
}
#endif //FEATURE_UIONE_HDK

/* <EJECT> */
/*===========================================================================
FUNCTION VoteForSleep

DESCRIPTION
  This function determines to vote okay or not okay to sleep.

  Input parameter
  TRUE - okay to sleep
  FALSE - not okay to sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void VoteForSleep (boolean flag)
{
  static boolean is_sleeping = FALSE;

  if (is_sleeping == flag) {
    /* nothing to do */
    return;
  }

  is_sleeping = flag; /* update state */

  if (flag == FALSE) {
    UI_DISABLE_SLEEP(); /* not okay to sleep */
  }
  else {
    UI_ENABLE_SLEEP();       /* okay to sleep */
  }
}

#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/* <EJECT> */
/*===========================================================================
FUNCTION HandleBacklights_cb

DESCRIPTION
  The callback for backlighing.  Just turn it off.

DEPENDENCIES
  Should only be set as a callback

RETURN VALUE
  Nothing

SIDE EFFECTS
  None
===========================================================================*/
void HandleBacklights_cb ()
{
  SwitchScrnBacklightOnOff( FALSE );
  SwitchKeyPadBacklightOnOff( FALSE );
  SwitchHotplugPollingTime( FALSE );
#ifdef T_QSC1100
  gbIsBacklightOn = FALSE;
  VoteForSleep(TRUE);
#endif

}

/*===========================================================================
FUNCTION CoreTask_SetPowerUp

DESCRIPTION
  Set the ui_powerup static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetPowerUp(boolean new_value)
{
  ui_powerup = new_value;

  if(!ui_powerup) {
    IShell *pShell = AEE_GetShell();
    if(pShell) {
      ISHELL_CancelTimer(pShell, (PFNNOTIFY) HandleBacklights_cb, NULL);
    }
  }
}

#ifdef FEATURE_TTY
/*===========================================================================
FUNCTION CoreTask_SetTTY

DESCRIPTION
  Set the ui_powerup static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetTTY(boolean new_value)
{
  ui_tty = new_value;
}

/*===========================================================================
FUNCTION CoreTask_GetTTY

DESCRIPTION
  Get the ui_headset static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetTTY(void)
{
  return ui_tty;
}
#endif /* FEATURE_TTY */
/*===========================================================================
FUNCTION CoreTask_GetKeyDown

DESCRIPTION
  Get the key_down static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
hs_key_type CoreTask_GetKeyDown(void)
{
  return key_down_key;
}


// TO DO - replace this call with ICallHistory in purevoice
int uivr_get_redial_phonenumber(char* pszNumber, char* pszName)
{
  return 0;
}
/* <EJECT> */
/*===========================================================================
FUNCTION EnableUIKeys

DESCRIPTION
  Tells the UI that the Coreapp is ready to start receiving keys

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void EnableUIKeys (boolean flag)
{
  send_keys = flag;
  (void) rex_set_sigs (&ui_tcb, UI_KEY_SIG);
}
#endif /* !FEATURE_UIONE_HDK */
#endif /* !FEATURE_MANGO_UI*/


/* <EJECT> */
/*===========================================================================
FUNCTION HandleStopSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void CoreAppHandleStopSig(void)
{
#if (defined(FEATURE_UIONE_HDK) || defined(FEATURE_MANGO_UI))
#if MIN_BREW_VERSION(4,0)
    //change the value to false before the brew exits
    bBREWIsInitialized = FALSE;
#endif
    AEE_Exit();
    ui_complete_pwrdown();
#else /* !FEATURE_UIONE_HDK && !FEATURE_FEATURE_MANGO_UI*/
#ifdef FEATURE_OTASP
    /* If MC initiates the power-down we will get the signal, before */
    /* we went through our power-down procedure. In that case we do  */
    /* not want to ack right now.                                    */
    if ( ui_pwrdown_complete == TRUE ) {
      {
        IShell *shell_ptr = AEE_GetShell();
        if (shell_ptr != NULL) {
          ISHELL_SendEvent(shell_ptr, AEECLSID_CORE_APP,
                          EVT_USER,EVT_UI_EXIT,0L);
        }

        /* Terminate BREW */
        EnableUIKeys(FALSE);
#if defined(FEATURE_BREWAPPCOORD)
        /* BREW App Coordination should exit so that other side knows
         * BREW has terminated */
        (void) BAC_Exit();
#endif
#if MIN_BREW_VERSION(4,0)
        //change the value to false before the brew exits
        bBREWIsInitialized = FALSE;
#endif
        AEE_Exit();
        ui_pwrdown_complete = FALSE ;    /* clear flag */
        (void) rex_clr_sigs( rex_self(), TASK_STOP_SIG );
      }
      ui_complete_pwrdown();
    }
    else
    {
      IShell *shell_ptr = AEE_GetShell();
      if (shell_ptr != NULL) {
        ISHELL_SendEvent(shell_ptr, AEECLSID_CORE_APP,
                           EVT_USER, EVT_MC_PWR_DOWN, 0L);
      } else {
        ui_pwrdown_complete = TRUE;
      }
      CoreAppHandleStopSig();
    }
#else /* FEATURE_OTASP */
    {
      IShell* shell_ptr = AEE_GetShell();
      if (shell_ptr != NULL) {
        ISHELL_SendEvent(shell_ptr, AEECLSID_CORE_APP,
                           EVT_USER,EVT_UI_EXIT,0L);
      }
    }
    /* Terminate BREW */
    EnableUIKeys(FALSE);
#if defined(FEATURE_BREWAPPCOORD)
    /*BREW App Coordination should exit so that other side knows
    * BREW has terminated*/
    (void) BAC_Exit();
#endif
#if MIN_BREW_VERSION(4,0)
    //change the value to false before the brew exits
    bBREWIsInitialized = FALSE;
#endif
    AEE_Exit();
    ui_complete_pwrdown();
#endif /* FEATURE_OTASP */
#endif /* !FEATURE_UIONE_HDK */
} /* CoreAppHandleStopSig */



/* <EJECT> */
/*===========================================================================
FUNCTION CoreAppHandleOfflineSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void CoreAppHandleOfflineSig(void)
{
#if !defined(FEATURE_UIONE_HDK) && !defined(FEATURE_MANGO_UI)
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr != NULL) {
    ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                      EVT_USER,
                      EVT_TASK_OFFLINE,0L);
  }
#endif /* !FEATURE_UIONE_HDK */
} /* CoreAppHandleOfflineSig */


#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/*===========================================================================
FUNCTION KeyUp

DESCRIPTION
  Fake a key up in case we need to play a sound or multitone even while a
  key is pressed down.

DEPENDENCIES
  It won't cause any harm if there is no key down.

RETURN VALUE
  Nothing

SIDE EFFECTS
  Stops key tone, if any.
  Sends continuous DTMF stop command to MC if needed.

===========================================================================*/
void KeyUp( void )
{
  if (uisnd_num_keys_pressed)
    uisnd_num_keys_pressed--;

  if( key_down_key != HS_NONE_K ) { /* If there is a key down... */
    if (uisnd_num_keys_pressed == 0)
    {
      key_down_key = HS_NONE_K;     /* There is no key down now */

      if( tone_started ) {
        tone_started = FALSE;

        uisnd_tone_stop();          /* Then stop the music */
      }
    }
  }
} /* KeyUp */
#endif // !FEATURE_MANGO_UI
#endif // !FEATURE_UIONE_HDK

/*===========================================================================

FUNCTION GetKey

DESCRIPTION
  Read a keycode from the input buffer.

DEPENDENCIES
  None

RETURN VALUE
  This procedure returns the oldest unread keycode.  If no keycode
  is currently available, HS_NONE_K is returned.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_KEYPAD_MULTI_KEY
byte GetKey( keypad_key_event_type *key_event_ptr )
#else
byte GetKey( void )
#endif
{

  byte keycode;

  if (ui_key_buffer.wr_idx == ui_key_buffer.rd_idx) { /* buffer is empty */
    keycode = HS_NONE_K;
#ifdef FEATURE_KEYPAD_MULTI_KEY
    key_event_ptr->key_code = HS_NONE_K;
#endif
  } else {                      /* get code & increment ring read index */
    keycode = ui_key_buffer.data[ui_key_buffer.rd_idx];
#ifdef FEATURE_KEYPAD_MULTI_KEY
    key_event_ptr->key_code = ui_key_buffer.data[ui_key_buffer.rd_idx];
    key_event_ptr->key_parm = ui_key_buffer.key_parm[ui_key_buffer.rd_idx];
#endif
    ui_key_buffer.rd_idx = (ui_key_buffer.rd_idx+1) & UI_KEY_BUF_MASK;
  }
  return( keycode );

} /* GetKey */

#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
static void keysound_cb_func(const void *client_data, snd_status_type status)
{
  if (status == SND_PLAY_DONE) {
    tone_started = FALSE;
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION PlayKeySound

DESCRIPTION
  Plays the beep for a key.

DEPENDENCIES
  uisnd_key_sound.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with some UI global variables

===========================================================================*/
static void PlayKeySound (hs_key_type key)
{
  static qword key_tone_time;     /* time key went down */
  static word  nTime;              /* for key beep */

  switch (key) {
    case HS_RELEASE_K:
      MSG_ERROR("PlayKeySound called for HS_RELEASE_K, unsupported", 0, 0, 0);
      break;

    case HS_UP_K:
    case HS_DOWN_K:
      /* No beeps while demo alert */
      if (ui_demo_alert)
        break;
      /* fall through */
    case HS_0_K:
    case HS_1_K:
    case HS_2_K:
    case HS_3_K:
    case HS_4_K:
    case HS_5_K:
    case HS_6_K:
    case HS_7_K:
    case HS_8_K:
    case HS_9_K:
    case HS_CLR_K:
    case HS_SEND_K:
    case HS_END_K:
#ifdef FEATURE_NAV_KEYPAD
    case HS_LEFT_K:
    case HS_RIGHT_K:
#else
    case HS_PF1_K:
    case HS_PF2_K:
#endif
    case HS_POUND_K:
    case HS_STAR_K:

#ifdef T_FFA6550_SS
    /* Enable keybeeps on keys for Slingshot that don't necessarily have a
       function yet */
    case HS_FA_K:
    case HS_F1_K:
    case HS_F2_K:
#endif // T_FFA6550_SS

    case HS_SF1_K:
    case HS_SF2_K:
    case HS_INFO_K:
    case HS_TASK_K:
    case HS_PTT_K:
    case HS_VOL_UP_K:
    case HS_VOL_DOWN_K:

      /* Play keybeep */
      key_down_key = key;
      clk_read_ms( key_tone_time ); /* get time of start */
      if( ui_burst || key == HS_END_K ) {
        nTime = BURST_DTMF_LEN;      /* bursts last only 95 ms */
      } else {
        tone_started = TRUE;
        nTime = 0;           /* continuous are forever */
      }

      uisnd_num_keys_pressed++;
      uisnd_key_sound((word)key,nTime,keysound_cb_func,(void*)key);
      break;

    default:
      break;

  }
}

/*===========================================================================
FUNCTION  WakeupFromStandby

DESCRIPTION
  Send call manager command to wake up from standby/deep sleep.

DEPENDENCIES
  UI and CM interface initialized

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WakeupFromStandby( void )
{

  int nReturnStatus;
  IShell *shell_ptr = AEE_GetShell();
  ICM *pICM = NULL;

  if (shell_ptr != NULL) {
    /* Create the Call Manager object. */
    nReturnStatus = ISHELL_CreateInstance(shell_ptr,
                                AEECLSID_CM,
                                (void **) &pICM);

    if(nReturnStatus != SUCCESS) {
      return FALSE;
    }

    if (pICM == NULL) { /* error */
      return FALSE;
    }
    nReturnStatus = ICM_WakeupFromStandby(pICM);
    ICM_Release(pICM);
    pICM = NULL;
    return (nReturnStatus == SUCCESS);
  }
  else {
    return FALSE;
  }
} /* WakeupFromStandby*/

#endif // !FEATURE_MANGO_UI
#endif //!FEATURE_UIONE_HDK


#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/*===========================================================================
FUNCTION CoreTask_RefreshAppContent

DESCRIPTION
  Forces a refresh of the display by invalidating the device bitmap.

PROTOTYPE
    void CoreTask_RefreshAppContent(IShell *pIShell, AEECLSID phyDisplayClsId)

    pIShell:          A pointer to the IShell
    phyDisplayCldId:  The class id of the physical display whose content needs
                      to be refreshed.

DEPENDENCIES
  None

RETURN VALUE
  Nothing

SIDE EFFECTS
  None

===========================================================================*/
static void CoreTask_RefreshAppContent(IShell *pIShell,
                                       AEECLSID phyDisplayClsId)
{
   ILogicalDisplayCfg       *pLogicalDispCfg = NULL;
   int                       nRetVal;
   AEECLSID                  nMappedLogDisp;
   physicalDisplayStateType  sPhyDispState;
   IAnnunciator *pIAnnun = NULL;  // Annunciator pointer for refresh

   if (pIShell == NULL)
   {
      MSG_MED("NULL shell pointer.", 0, 0, 0);
      return;
   }

   /* Make sure the request is for a supported display class. */
   if ((phyDisplayClsId != AEECLSID_PHYSICALDISPLAY1)
#if defined(FEATURE_SECONDARY_DISPLAY)
       && (phyDisplayClsId != AEECLSID_PHYSICALDISPLAY2)
#endif
       )
   {
      MSG_MED("Invalid display class id, 0x%x", phyDisplayClsId, 0, 0);
      return;
   }

   /* Use a do..while (0) to simplify cleanup in error cases. */
   do
   {
      // Create a logical display config
	      nRetVal = ISHELL_CreateInstance(pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                      (void **)&pLogicalDispCfg);
      if ((nRetVal != SUCCESS) || (pLogicalDispCfg == NULL)) {
         MSG_MED("Failed creating logical disp cfg, %d, 0x%x", nRetVal,
                 pLogicalDispCfg, 0);
         break;
      }

      nMappedLogDisp =
                ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(pLogicalDispCfg,
                                                            phyDisplayClsId);

      /* If there's no mapped logical display, there is no need to turn it on. */
      if (nMappedLogDisp == 0)
      {
         break;
      }

      /* Now unlink and relink the physical display to force a refresh. */
      sPhyDispState.physicalDisplay = phyDisplayClsId;
      sPhyDispState.state = 0;
      nRetVal = ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                                        nMappedLogDisp,
                                                        1, &sPhyDispState);
      if (nRetVal != SUCCESS)
      {
         MSG_MED("Failed to unlink physical display 0x%x from logical display 0x%x.",
                 sPhyDispState.physicalDisplay, nMappedLogDisp, 0);
         break;
      }

      sPhyDispState.state = 1;
      nRetVal = ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                                     nMappedLogDisp,
                                                     1, &sPhyDispState);
      if (nRetVal != SUCCESS)
      {
         MSG_MED("Failed to relink physical display 0x%x to logical display 0x%x.",
                 sPhyDispState.physicalDisplay, nMappedLogDisp, 0);
         break;
      }

   }
   while (0);

   if (pLogicalDispCfg != NULL)
   {
      ILOGICALDISPLAYCFG_Release(pLogicalDispCfg);
   }
	            nRetVal = ISHELL_CreateInstance(pIShell,
	                                                  AEECLSID_ANNUNCIATOR,
	                                                  (void **)&pIAnnun);
	            if ((SUCCESS != nRetVal) || !pIAnnun)
	            {
	              MSG_MED("Unable to create annunciator, %d.", nRetVal, 0, 0);
	            }
	            else
	            {
	              if (IANNUNCIATOR_Redraw(pIAnnun) != SUCCESS)
	              {
	                MSG_MED("Unable to update annunciator.", 0, 0, 0);
	              }
	              IANNUNCIATOR_Release(pIAnnun);
	         }
}

/*===========================================================================
FUNCTION HandleBacklights

DESCRIPTION
  Handle backlight off timing

DEPENDENCIES
  ui_backval should be set correctly

RETURN VALUE
  Nothing

SIDE EFFECTS
  Sets up a clock callback

===========================================================================*/
void HandleBacklights (
  boolean onoff,                  /* turn on or off */
  int nTime                        /* how long on - UI_BKLT_TIME_MAX = always */
                                  /* UI_BKLT_TIME_NONE = never. */
)
{
  IShell   *pShell = AEE_GetShell();
#if defined(FEATURE_SECONDARY_DISPLAY)
  int       nDevInfoSuccess;
  boolean   bIsFlipOpen;
  int       nFlipOpenSize;
#endif

  if ( !ui_powerup ) {
    onoff = FALSE;
    nTime = UI_BKLT_TIME_NONE;
  }
  if( ui_powerup ) {
    if (pShell) {
#ifndef FEATURE_MANGO_UI
      ISHELL_PostEventEx(pShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                          EVT_USER, EVT_LCD_ON, (uint32)onoff);
#endif
    }
  }
  if( onoff ) {                   /* turn on */
#if defined(FEATURE_SECONDARY_DISPLAY)

    /* Check clamshell status before turning the display on.  If the
     * clamshell is closed, turn on the secondary display.  If not,
     * turn on the primary.
     */
    nFlipOpenSize = sizeof(bIsFlipOpen);
    nDevInfoSuccess = ISHELL_GetDeviceInfoEx(pShell, AEE_DEVICESTATE_FLIP_OPEN,
                                         (void *)&bIsFlipOpen, &nFlipOpenSize);
    if (nDevInfoSuccess != SUCCESS) {
      /* If we couldn't get the status, error and default to
       * clamshell open.
       */
      MSG_MED("Unable to get clamshell status for backlight: %d.",
              nDevInfoSuccess, 0, 0);
      bIsFlipOpen = TRUE;
    }

    if ((bIsFlipOpen == FALSE) && (! ui_display)
#ifdef T_FFA6550_SS
        /* When SS is articulated and not closed, the primary display is not
         * visible, so turn on the secondary.  If it's articulated and closed,
         * the primary display is visible, so turn it on instead.
         */
        && (hs_query_articulated_state() == FALSE)
#endif /* T_FFA6550_SS */
      ) {
      ui_display = TRUE;
      //powerup the display as we turnedoff the LCD power when the backlight was turnedoff previously
      disp_powerup2();
      disp_on2();
      CoreTask_RefreshAppContent(pShell, AEECLSID_PHYSICALDISPLAY2);
    }
    else
    {
      // Turn on the Display only if it has been switched off.
      if(!ui_display) {
        ui_display = TRUE;
        //powerup the display as we turnedoff the LCD power when the backlight was turnedoff previously
        disp_powerup();
        disp_on();
        CoreTask_RefreshAppContent(pShell, AEECLSID_PHYSICALDISPLAY1);
      }
    }
#else // FEATURE_SECONDARY_DISPLAY
    if(!ui_display) {
      ui_display = TRUE;
      //powerup the display as we turnedoff the LCD power when the backlight was turnedoff previously
      disp_powerup();
      disp_on();
      CoreTask_RefreshAppContent(pShell, AEECLSID_PHYSICALDISPLAY1);
#ifdef T_FFA6650_PANDORA
      pm_vote_vreg_switch(PM_ON_CMD,PM_VREG_MMC_ID,
			  PM_VOTE_VREG_MMC_APP__LCD);
#else
      pm_vote_vreg_switch(PM_ON_CMD,PM_VREG_BOOST_ID,
			  PM_VOTE_VREG_BOOST_APP__LCD);
#endif
    }

#endif // FEATURE_SECONDARY_DISPLAY
    CoreTask_SetLCDTimer(lcd_timer);

    if( nTime == UI_BKLT_TIME_NONE ) {
      SwitchScrnBacklightOnOff( FALSE );
      SwitchKeyPadBacklightOnOff( FALSE );
      SwitchHotplugPollingTime( FALSE );
#ifdef T_QSC1100
	  gbIsBacklightOn = FALSE;
      VoteForSleep(TRUE);
#endif
      if(pShell)
       ISHELL_CancelTimer(pShell, (PFNNOTIFY) HandleBacklights_cb, NULL);
      return;
    }

    if( nTime == UI_BKLT_TIME_MAX ) {    /* always on */
      SwitchScrnBacklightOnOff( TRUE );
      SwitchKeyPadBacklightOnOff( TRUE );
      SwitchHotplugPollingTime( TRUE );
#ifdef T_QSC1100
	  gbIsBacklightOn = TRUE;
      VoteForSleep(FALSE);
#endif

      if(pShell)
        ISHELL_CancelTimer(pShell, (PFNNOTIFY) HandleBacklights_cb, NULL);

    } else {
      int time_ms = 1000*nTime;
      SwitchScrnBacklightOnOff( TRUE );
      SwitchKeyPadBacklightOnOff( TRUE );
      SwitchHotplugPollingTime( TRUE );
#ifdef T_QSC1100
	  gbIsBacklightOn = TRUE;
      VoteForSleep(FALSE);
#endif

      if(pShell)
        ISHELL_SetTimer(pShell,time_ms, (PFNNOTIFY) HandleBacklights_cb,NULL);
    }
  } else {                        /* turn off */

    SwitchScrnBacklightOnOff( FALSE );
    SwitchKeyPadBacklightOnOff( FALSE );
    SwitchHotplugPollingTime( FALSE );
#ifdef T_QSC1100
	gbIsBacklightOn = FALSE;
    VoteForSleep(TRUE);
#endif

    if(pShell)
      ISHELL_CancelTimer(pShell, (PFNNOTIFY) HandleBacklights_cb, NULL);
  }
}
#endif // !FEATURE_MANGO_UI
#endif //!FEATURE_UIONE_HDK

/*===========================================================================
FUNCTION ConvertHsToVcode

DESCRIPTION
   This function translates HS key to AEE vcode.

DEPENDENCIES

RETURN VALUE
  boolean:   TRUE if pVcode and pDWParam could be set; FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean ConvertHsToVcode(hs_key_type key, AVKType * const pVcode,
                         uint32 * const pDWParam)
{
  int i;

  #ifdef FEATURE_NAV_KEYPAD
  if (key == HS_SEL_K)
    MSG_HIGH("Received select key", 0, 0, 0);
  #endif

  if ((NULL == pVcode) || (NULL == pDWParam)) {
    MSG_HIGH("Received NULL ptr: %x %x", pVcode, pDWParam, 0);
    return FALSE;
  }

/*lint -e737*/
/*lint -e574*/
  for (i= 0; i< ARR_SIZE(hs_to_aee_tbl); i++)
  {
    if (hs_to_aee_tbl[i].hs_key == key) {
      *pVcode = hs_to_aee_tbl[i].aee_vcode;
      *pDWParam = hs_to_aee_tbl[i].dwparam;
      MSG_HIGH("Returning key %d (%x) and param %d", *pVcode, *pVcode, *pDWParam);
      return TRUE;
    }
  }
/*lint +e737*/
/*lint +e574*/

  *pVcode = AVK_UNDEFINED;
  *pDWParam = 0;
  return TRUE;
} /* End ConvertHsToVcode */

#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_APP_CATAPP
boolean ValidUserKey(AVKType key) {
  switch (key) {
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
  case EVT_VOLUME_UP:
  case EVT_VOLUME_DOWN:
  case AVK_LEFT:
  case AVK_RIGHT:
  case AVK_SELECT:
  case AVK_STAR:
  case AVK_POUND:
    return TRUE;
  default:
    return FALSE;
  }
} /* ValidUserKey */
#endif /*FEATURE_APP_CATAPP */

/*===========================================================================
FUNCTION SendPowerKey

DESCRIPTION
  Sends a PWR key to the UI if the KEY RELEASE for an END Key is
  not received for two seconds.
DEPENDENCIES

RETURN VALUE
  Nothing

SIDE EFFECTS
  None
===========================================================================*/
void SendPowerKey ()
{
  AVKType last_vcode = AEE_INVALID_CODE;
  uint32 dwparam;
  boolean bConvertSuccess=FALSE;


  bConvertSuccess = ConvertHsToVcode( HS_PWR_K,
                                       &last_vcode, &dwparam);
  if (! bConvertSuccess) {
    // If we couldn't set the values, use default values
    last_vcode = AVK_UNDEFINED;
    dwparam = 0;
  }

  if (last_vcode > AEE_AVK_BASE)
  {
    IShell* pShell = AEE_GetShell();
    if (pShell == NULL) {
      MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
      return;
    }
    //Send Power Key
    (void) AEE_Event(EVT_KEY_PRESS, last_vcode, dwparam);
    (void) AEE_Event(EVT_KEY, last_vcode, dwparam);
  }
}
#endif // !FEATURE_UIONE_HDK

/* <EJECT> */
/*===========================================================================
FUNCTION handle_keys

DESCRIPTION
  Handle a key pressed.

DEPENDENCIES
  REX is running.  NV and DIAG and HS are running.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
#ifdef FEATURE_UIONE_HDK
void handle_keys(void)
{
#ifdef FEATURE_KEYPAD_MULTI_KEY
   keypad_key_event_type   key;
#else
  kpd_key_event_type  key;               /* hold key temporarily */
#endif /* FEATURE_KEYPAD_MULTI_KEY */
  boolean bConvertSuccess;
  /* static locals are used to remember the last pressed key for
   * handling events when the keypad doesn't support pressing multiple keys
   */
  static AVKType last_vcode = AEE_INVALID_CODE;
  static uint32 dwparam = 0;
#ifdef FEATURE_KEYPAD_MULTI_KEY
  while ( (key.key_code = GetKey(&key)) != HS_NONE_K) {
#else /* FEATURE_KEYPAD_MULTI_KEY */
  while ( ( key.key_code = (hs_key_type)GetKey()) != HS_NONE_K ) {
#endif
    MSG_HIGH("Received Key %d (%x) from HS", key.key_code, key.key_code, 0);
    if (key.key_code == HS_RELEASE_K) {
#ifdef FEATURE_KEYPAD_MULTI_KEY
      bConvertSuccess = ConvertHsToVcode( (hs_key_type) key.key_parm,
                                      &last_vcode, &dwparam);
      if (bConvertSuccess) {
         if(AVK_POWER != last_vcode)
         {
            (void) AEE_Event(EVT_KEY_RELEASE, last_vcode, dwparam);
        }
      }
      last_vcode = AEE_INVALID_CODE;
      dwparam = 0;
    }
    else
#else
      if (last_vcode > AEE_AVK_BASE)
      {
        (void) AEE_Event(EVT_KEY_RELEASE, last_vcode, dwparam);
      }

      last_vcode = AEE_INVALID_CODE;
      dwparam = 0;
    }
    else if (HS_PWR_K != key.key_code)
#endif
    {
#ifndef FEATURE_KEYPAD_MULTI_KEY
      /* If we don't support multiple keypresses, then release the last key before we wipe out
         all memory that it was pressed. */
      if(last_vcode > AEE_INVALID_CODE)
         AEE_Event(EVT_KEY_RELEASE, last_vcode, dwparam);
#endif
      bConvertSuccess = ConvertHsToVcode( (hs_key_type) key.key_code,
                                          &last_vcode, &dwparam);
      if (! bConvertSuccess) {
        // If we couldn't set the values, use default values
        last_vcode = AVK_UNDEFINED;
        dwparam = 0;
      }

      /* AVK_UNDEFINED is the same as AEE_INVALID_CODE. */
      if (last_vcode <= AEE_INVALID_CODE) {
         switch (key.key_code) {
         case HS_EXT_PWR_ON_K:
         case HS_EXT_PWR_OFF_K:
            OEMBattery_Refresh();
            break;
            case HS_STEREO_HEADSET_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_ENABLED);
#ifdef FEATURE_TTY
            (void) CoreUI_SetTTYDeviceStatus(UISND_DEV_ENABLED);
#endif /* FEATURE_TTY */
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoStereoHeadphoneOn(TRUE);
            break;
            case HS_HEADSET_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_ENABLED);
#ifdef FEATURE_TTY
            /* set tty device attached*/
            (void) CoreUI_SetTTYDeviceStatus(UISND_DEV_ENABLED);
#endif /* FEATURE_TTY */
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoHeadphoneOn(TRUE);
            break;
            case HS_HEADSET_OFF_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_STEREO_HEADSET, UISND_DEV_UNATTACHED);
            uisnd_set_device_status(SND_DEVICE_HEADSET, UISND_DEV_UNATTACHED);
#ifdef FEATURE_TTY
            uisnd_set_device_status(SND_DEVICE_TTY_HEADSET, UISND_DEV_UNATTACHED);
            uisnd_set_device_status(SND_DEVICE_TTY_HCO, UISND_DEV_UNATTACHED);
            uisnd_set_device_status(SND_DEVICE_TTY_VCO, UISND_DEV_UNATTACHED);
#endif
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoStereoHeadphoneOn(FALSE);
            (void) OEM_SetDeviceInfoHeadphoneOn(FALSE);
            break;

         case HS_FREE_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_HFK, UISND_DEV_ENABLED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoHFKOn(TRUE);
            break;
         case HS_PHONE_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_HFK, UISND_DEV_UNATTACHED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoHFKOn(FALSE);
            break;

#ifdef FEATURE_USB_CARKIT
         case HS_USB_HFK_ON_K:
         case HS_USB_STEREO_HFK_ON_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_USB, UISND_DEV_ENABLED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoUSBHFKOn(TRUE);
            break;

         case HS_USB_HFK_OFF_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_USB, UISND_DEV_UNATTACHED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoUSBHFKOn(FALSE);
            break;
#endif

#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
         case HS_SDAC_ON_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_SDAC, UISND_DEV_ENABLED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoSDACOn(TRUE);
            break;

         case HS_SDAC_OFF_K:
#ifdef FEATURE_UIXSND_IMPLEMENTATION
            uisnd_set_device_status(SND_DEVICE_SDAC, UISND_DEV_UNATTACHED);
#endif // FEATURE_UIXSND_IMPLEMENTATION
            (void) OEM_SetDeviceInfoSDACOn(FALSE);
            break;
#endif
#ifdef FEATURE_SECONDARY_DISPLAY
         case HS_FLIP_OPEN_K:
            (void) OEM_SetDeviceInfoFlipOpen(TRUE);
            break;

         case HS_FLIP_CLOSE_K:
            (void) OEM_SetDeviceInfoFlipOpen(FALSE);
            break;
#endif
         default:
            break;
        }
      }
      else {
        switch (last_vcode)
        {
#ifdef FEATURE_KEYPAD_MULTI_KEY
          case AVK_POWER:
            break;
#endif
          default:
            (void) AEE_Event(EVT_KEY_PRESS, last_vcode, dwparam);
            (void) AEE_Event(EVT_KEY, last_vcode, dwparam);
            break;
        }  //end of switch
      }  // end of else
    }
  } /* while (key = GetKey() */
}
#else /* !FEATURE_UIONE_HDK */
void handle_keys(void)
{
#ifdef FEATURE_KEYPAD_MULTI_KEY
   keypad_key_event_type   key;
#else
  kpd_key_event_type  key;               /* hold key temporarily */
#endif /* FEATURE_KEYPAD_MULTI_KEY */
  boolean bConvertSuccess;

  /* static locals are used to remember the last pressed key for
   * handling events when the keypad doesn't support pressing multiple keys
   */
  static AVKType last_vcode = AEE_INVALID_CODE,
                 last_key_pressed = AEE_INVALID_CODE;
  static uint32 dwparam = 0;


#ifdef FEATURE_KEYPAD_MULTI_KEY
  while ( (key.key_code = GetKey(&key)) != HS_NONE_K) {
    ConvertHsToVcode( (hs_key_type) key.key_parm, &last_vcode, &dwparam);
#else /* FEATURE_KEYPAD_MULTI_KEY */
  while ( ( key.key_code = (hs_key_type)GetKey()) != HS_NONE_K ) {
#endif
    MSG_HIGH("Received Key %d (%x) from HS", key.key_code, key.key_code, 0);

#ifndef FEATURE_MANGO_UI
    /* Send command to Call Manager to wake up the system from deep sleep */
    if (ui_deepsleep && !ui_game_mode) {
      if (WakeupFromStandby()) { /* ICM call */
         /* if the request is accepted by CM then set deepsleep to FALSE so
         ** UI would not send multiple requests to CM for subsequent key presses.
         */
         ui_deepsleep = FALSE;
      }
    }
#endif
    if (key.key_code == HS_RELEASE_K) {
      if (last_vcode > AEE_AVK_BASE)
      {
        if ((AVK_END == last_key_pressed ))
        {
          IShell* pShell = AEE_GetShell();
          if (pShell == NULL)
          {
            MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
            return;
          }
          ISHELL_CancelTimer (pShell, (PFNNOTIFY) SendPowerKey, NULL);

          bConvertSuccess = ConvertHsToVcode( HS_END_K,
                                      &last_vcode, &dwparam);

          //Send END Key
          (void) AEE_Event(EVT_KEY_PRESS, last_vcode, dwparam);
          (void) AEE_Event(EVT_KEY, last_vcode, dwparam);
        }
        AEE_Event(EVT_KEY_RELEASE, last_vcode, dwparam);
      }
      last_vcode = AEE_INVALID_CODE;
      last_key_pressed = AEE_INVALID_CODE;
      dwparam = 0;
#ifndef FEATURE_MANGO_UI
      HandleBacklights (TRUE, ui_backlight_duration);
      KeyUp();
#endif
    }
    else
    {
#ifndef FEATURE_KEYPAD_MULTI_KEY
      /* If we don't support multiple keypresses, then release the last key before we wipe out
         all memory that it was pressed. */
      if(last_vcode > AEE_INVALID_CODE)
         AEE_Event(EVT_KEY_RELEASE, last_vcode, dwparam);
#endif

      bConvertSuccess = ConvertHsToVcode( (hs_key_type) key.key_code,
                                          &last_vcode, &dwparam);
      if (! bConvertSuccess) {
        // If we couldn't set the values, use default values
        last_vcode = AVK_UNDEFINED;
        dwparam = 0;
      }
      if (last_vcode < AEE_INVALID_CODE) {
        /* External device key */
#ifndef FEATURE_MANGO_UI
        IShell* pShell = AEE_GetShell();
        if (pShell == NULL) {
          MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
          return;
        }
        ISHELL_PostEventEx (pShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                            EVT_USER,last_vcode,dwparam);
#else
	IShell* pShell = AEE_GetShell();
	if (pShell != NULL) {
	// Mango UI: Send a device notification (EVT_HEADSET) to the BREW layers.
	if((last_vcode == EVT_HEADSET_ON)
	   || (last_vcode == EVT_STEREO_HEADSET_ON)
	   || (last_vcode == EVT_HEADSET_OFF))
	{
	   AEEDeviceNotify deviceNotify;
	   deviceNotify.wParam =  (last_vcode != EVT_HEADSET_OFF);
	   deviceNotify.dwParam =  0;
	   (void) AEE_Event(EVT_HEADSET, deviceNotify.wParam, deviceNotify.dwParam);
	   ISHELL_Notify (pShell, AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_HEADSET , (void *)&deviceNotify);
	}				
	}
#endif
      }
      else {
        switch (last_vcode)
        {
          case AVK_END:
            {
              IShell* pShell = AEE_GetShell();
              if (pShell == NULL) {
                MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
                return;
              }
              ISHELL_SetTimer (pShell, 2000, (PFNNOTIFY)SendPowerKey, NULL);
              last_key_pressed = last_vcode;
            }
            break;

          case AVK_POWER:
            MSG_MED ("Do not send Power Key yet", 0,0,0);
            break;

          default:
            (void) AEE_Event(EVT_KEY_PRESS, last_vcode, dwparam);
            (void) AEE_Event(EVT_KEY, last_vcode, dwparam);
#ifndef FEATURE_MANGO_UI
#ifdef FEATURE_APP_CATAPP
            {
              IShell* pShell = AEE_GetShell();
              if (pShell == NULL) {
                MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
                return;
              }
              /* Post event to CatApp */
              if (ValidUserKey(last_vcode)) {
              ISHELL_PostEventEx (pShell, EVTFLG_ASYNC, AEECLSID_CATAPP,
                              EVT_CAT_USER_KEY_EVENT,last_vcode,0L);
              }
            }
#endif /*FEATURE_APP_CATAPP*/
#endif //FEATURE_MANGO_UI
            break;
        }  //end of switch
      }  // end of else
#ifndef FEATURE_MANGO_UI
      if (!ui_silence_key)
        PlayKeySound((hs_key_type)key.key_code);
#endif
    }
  } /* while (key = GetKey() */
}
#endif // !FEATURE_UIONE_HDK

/* <EJECT> */
/*===========================================================================
FUNCTION CoreAppHandleSignals

DESCRIPTION
  This function handles all signals received by UI task. This function is to
  be invoked for every signal UI recives. It calls the appropriate functions
  to handle each signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void CoreAppHandleSignals (
    rex_sigs_type sigs
)
{
  uint32 ret_val = 0;

  /* Stop task - nothing special to do */
  if ( ( sigs & TASK_STOP_SIG ) != 0 ) {
    CoreAppHandleStopSig();
  }
/* Check if BREW is initialized. */
#if MIN_BREW_VERSION(4,0)
else
{
  //update the var only if it is not a stop sig
  bBREWIsInitialized = AEE_IsInitialized( );
}
#endif

  /* Go offline signal */
  if( ( sigs & TASK_OFFLINE_SIG ) != 0 ) {
    CoreAppHandleOfflineSig();
  }

  /* Key event signal - we have keys from the handset */
  if( sigs & UI_KEY_SIG ) {
    (void) rex_clr_sigs( rex_self(), UI_KEY_SIG );



#if (!defined(FEATURE_UIONE_HDK) && !defined(FEATURE_MANGO_UI))
    if (send_keys)
#endif /* !FEATURE_UIONE_HDK */
      handle_keys();
  } /* key signal set */

#ifdef FEATURE_BREW_NET_SIG
  /* Brew signals */
  if ( sigs & AEE_NET_SIG) {
    extern void OEMNet_SignalHandler(void);
    (void) rex_clr_sigs( rex_self(), AEE_NET_SIG);
    (void) OEMNet_SignalHandler();
  }
#endif

#if !MIN_BREW_VERSION(4,0)
  /* Brew signal */
  if ( sigs & AEE_APP_SIG) {
    /* Handle AEE_APP_SIG */
    (void) rex_clr_sigs( rex_self(), AEE_APP_SIG);
    VoteForSleep(FALSE);
    ret_val = AEE_Dispatch();
    if (ret_val == 0) {
// need this change for Mango too.	    
#ifdef T_QSC1100
	   if(TRUE != gbIsBacklightOn)
#endif
	   {
          VoteForSleep(TRUE); /* okay to sleep */
       }
    }
    else {
      VoteForSleep(FALSE); /* not okay to sleep */
    }
  }
#endif

  if ( sigs & UI_RUIM_SIG) {
    (void) rex_clr_sigs( rex_self(), UI_RUIM_SIG);
    VoteForSleep(FALSE);
    ret_val =  AEE_Dispatch();
    if (ret_val == 0) {
//Need this change for Mango too.	    
#if (defined(T_QSC1100) )
       if(TRUE != gbIsBacklightOn)
#endif
	   {
          VoteForSleep(TRUE); /* okay to sleep */
       }
    }
    else {
      VoteForSleep(FALSE); /* not okay to sleep */
    }
  }

#ifdef FEATURE_BREW_SIO
  if ( sigs & AEE_SIO_SIG) {
    extern void OEMSIO_TaskHandler(void);
    (void) rex_clr_sigs( rex_self(), AEE_SIO_SIG);
    OEMSIO_TaskHandler();
  }
#endif

#ifdef FEATURE_BREW_USBDRIVER
  if ( sigs & AEE_USBDRIVER_SIG )
  {
    extern void OEMUSBDriver_TaskHandler(void);
    (void) rex_clr_sigs( rex_self(), AEE_USBDRIVER_SIG );
    (void) OEMUSBDriver_TaskHandler();
  }
#endif

#ifdef FEATURE_BREWAPPCOORD
  if ( sigs & UI_BAC_SIG )
  {
    (void) rex_clr_sigs( rex_self(), UI_BAC_SIG );
    (void) BAC_SigHandler();
  }
#endif
}

/*===========================================================================
FUNCTION CoreApp_Getsigs

DESCRIPTION
  Give the signal that we will need from this.

DEPENDENCIES
  None

RETURNS


SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type CoreApp_Getsigs( void )
{
  rex_sigs_type sigs = 0;

  /* Start with the signals we cooperate with the base ui. */
  sigs |= TASK_OFFLINE_SIG;
  sigs |= TASK_STOP_SIG;

  /* Key event signal - we have keys from the handset */
  sigs |= UI_KEY_SIG;

  /* Brew signal */
  sigs |= AEE_APP_SIG;

  /* RUIM Signal */
  sigs |= UI_RUIM_SIG;
#ifdef FEATURE_BREW_SIO
  sigs |= AEE_SIO_SIG;
#endif

#ifdef FEATURE_BREW_USBDRIVER
  sigs |= AEE_USBDRIVER_SIG;
#endif

#ifdef FEATURE_BREWAPPCOORD
  sigs |= UI_BAC_SIG;
#endif

#ifdef FEATURE_BREW_NET_SIG
  sigs |= AEE_NET_SIG;
#endif

  return sigs;
}

/*===========================================================================
FUNCTION InitResources

DESCRIPTION
  Initialize the FS for resource files

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void InitResources (void)
{
#ifndef PLATFORM_LTK
  int i;
  struct fs_stat sbuf;
  const char *brew_dirs[] =
  {
#if MIN_BREW_VERSION(4,0)
    "sys",
    "shared",
    "mod",
    "mif",
#else
    "brew",
    "brew/sys",
    "brew/shared",
    "brew/mod",
    "brew/mif",
#endif
    "user",
    "user/sound",
    "user/sound/ringer",
  };

  int nDirNum = ARR_SIZE(brew_dirs); //Number of 'other' brew directories.

  for(i=0; i < nDirNum; i++)
  {
    if(efs_stat((char*) brew_dirs[i], &sbuf) != 0) {
      if(efs_mkdir((char*)brew_dirs[i], 0555)== -1) {
        ERR_FATAL("Unable to create directory", 0, 0, 0);
      }
    }
  }

#endif /* PLATFORM_LTK */
}

/*===========================================================================

LOCAL FUNCTION ui_key_event_cb

DESCRIPTION
  Queue the keycode on the ring buffer, and signals the UI task. If there
  is no room in the buffer, an error is logged and the task is signalled
  anyway.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ui_key_event_cb
(
#ifdef FEATURE_KEYPAD_MULTI_KEY
  keypad_key_event_type key_event
#else
  kpd_key_event_type key_event
#endif
)
{

  word isave;                   /* PSW parameter for INTLOCK/FREE_SAV    */

  /* If the ring buffer is not full (full is when the write index is
  ** circularly 1 less than the read index), put the keycode into the
  ** ring buffer.
  */
  INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */

  if (((ui_key_buffer.wr_idx + 1) & UI_KEY_BUF_MASK) != ui_key_buffer.rd_idx) {
    ui_key_buffer.data[ui_key_buffer.wr_idx] = key_event.key_code;
#ifdef FEATURE_KEYPAD_MULTI_KEY
    ui_key_buffer.key_parm[ui_key_buffer.wr_idx] = key_event.key_parm;
#endif
    ui_key_buffer.wr_idx = (ui_key_buffer.wr_idx+1) & UI_KEY_BUF_MASK;
  } else {
    ERR("Input buffer overflow (size %d)", UI_KEY_BUF_SIZE, 0, 0 );
  }

  /* If BREW is initialized, we can use the CS SignalQ to handle key events */
  #if MIN_BREW_VERSION(4,0)
  {
     if (TRUE == bBREWIsInitialized) {
        CALLBACK_Init(&KeyHandlerCb, handle_keys, NULL);
        AEE_ResumeCallback(&KeyHandlerCb, 0);
     }
     else {
        /* Fallback to rex signals */
        (void) rex_set_sigs( &ui_tcb, UI_KEY_SIG );
     }
  }
  #endif

  INTFREE_SAV( isave );       /* Restore interrupts (PSW) */

  #if !MIN_BREW_VERSION(4,0)
     (void) rex_set_sigs( &ui_tcb, UI_KEY_SIG );
  #endif

#ifdef FEATURE_KEYPAD_MULTI_KEY
  MSG_LOW("Keycode = 0x%x, Keyparm = 0x%x", key_event.key_code,
	  key_event.key_parm, 0);
#endif

} /* end of ui_key_event_cb */

#ifdef FEATURE_USB_CARKIT
/*===========================================================================

LOCAL FUNCTION ui_usb_event_cb

DESCRIPTION
  Queue the usb keycode on the buffer, and signals the UI task. If there
  is no room in the buffer, an error is logged and the task is signalled
  anyway.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ui_usb_event_cb(usb_otg_cc_event_type evt)
{
  word isave;                   /* PSW parameter for INTLOCK/FREE_SAV    */
  hs_key_type keycode1 = HS_NONE_K;
  hs_key_type keycode2 = HS_NONE_K;
  hs_key_type keycode3 = HS_NONE_K;
  usb_otg_cc_service_req_type carkit_service_req;
  usb_otg_cc_status_type      carkit_status;

  static boolean last_handset_on_cradle_state = FALSE;

  carkit_status = usb_otg_cc_get_carkit_status();

  //Handle cradle events
  if (last_handset_on_cradle_state != carkit_status.hs_on_cradle)
  {
    last_handset_on_cradle_state = carkit_status.hs_on_cradle;

    if (carkit_status.hs_on_cradle)
    {
      keycode2 = HS_USB_HFK_RETURN_TO_CRADLE_K;
    }
    else
    {
      keycode2 = HS_USB_HFK_REMOVE_FROM_CRADLE_K;
    }
  }

  switch(evt)
  {
    case USB_OTG_CC_EVENT__CARKIT_AVAILABLE:
      if (usb_otg_cc_feature_supported(USBCC_OTG_CC_FEATURE__STEREO))
      {
        keycode1 = HS_USB_STEREO_HFK_ON_K;
      }
      else
      {
        keycode1 = HS_USB_HFK_ON_K;
      }
      break;
    case USB_OTG_CC_EVENT__CARKIT_UNAVAILABLE:
      keycode1 = HS_USB_HFK_OFF_K;
      break;
    case USB_OTG_CC_EVENT__CARKIT_SERVICE_REQUEST:

      carkit_service_req = usb_otg_cc_get_carkit_service_request();

      if (carkit_service_req.carkit_generic_button_pressed)
      {
        keycode1 = HS_USB_HFK_GENERIC_BUTTON_K;
      }
      else if (carkit_service_req.carkit_send_button_pressed)
      {
        keycode1 = HS_USB_HFK_SEND_BUTTON_K;
        keycode3 = HS_RELEASE_K;
      }
      else if (carkit_service_req.carkit_end_button_pressed)
      {
        keycode1 = HS_USB_HFK_END_BUTTON_K;
        keycode3 = HS_RELEASE_K;
      }
      else if (carkit_service_req.carkit_vad_button_pressed)
      {
        keycode1 = HS_USB_HFK_VOICEDIAL_BUTTON_K;
      }
      else if (carkit_service_req.carkit_ptt_button_pressed)
      {
        keycode1 = HS_PTT_K;
        keycode3 = HS_RELEASE_K;
      }
      break;

    default:
      break;
  }
  MSG_MED("Processing USB OTG keys %d %d", keycode1, keycode2, 0);

  /* If the ring buffer is not full (full is when the write index is
  ** circularly 1 less than the read index), put the keycode into the
  ** ring buffer.
  */
  INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */

  if (keycode1 != HS_NONE_K)
  {
    if (((ui_key_buffer.wr_idx + 1) & UI_KEY_BUF_MASK) != ui_key_buffer.rd_idx) {
      ui_key_buffer.data[ui_key_buffer.wr_idx] = keycode1;
      ui_key_buffer.wr_idx = (ui_key_buffer.wr_idx+1) & UI_KEY_BUF_MASK;
    } else {
      ERR("Input buffer overflow (size %d)", UI_KEY_BUF_SIZE, 0, 0 );
    }
  }

  if (keycode2 != HS_NONE_K)
  {
    if (((ui_key_buffer.wr_idx + 1) & UI_KEY_BUF_MASK) != ui_key_buffer.rd_idx) {
      ui_key_buffer.data[ui_key_buffer.wr_idx] = keycode2;
      ui_key_buffer.wr_idx = (ui_key_buffer.wr_idx+1) & UI_KEY_BUF_MASK;
    } else {
      ERR("Input buffer overflow (size %d)", UI_KEY_BUF_SIZE, 0, 0 );
    }
  }

  if (keycode3 != HS_NONE_K)
  {
    if (((ui_key_buffer.wr_idx + 1) & UI_KEY_BUF_MASK) != ui_key_buffer.rd_idx) {
      ui_key_buffer.data[ui_key_buffer.wr_idx] = keycode3;
      ui_key_buffer.wr_idx = (ui_key_buffer.wr_idx+1) & UI_KEY_BUF_MASK;
    } else {
      ERR("Input buffer overflow (size %d)", UI_KEY_BUF_SIZE, 0, 0 );
    }
  }

  INTFREE_SAV( isave );       /* Restore interrupts (PSW) */

  (void) rex_set_sigs( &ui_tcb, UI_KEY_SIG );
}
#endif
/*===========================================================================
FUNCTION CoreTask_KeyPadSubscriptionCb

DESCRIPTION
  Call back function to report the status of keypad servie subscription.

DEPENDENCIES
  None.

RETURN VALUE
  Nothing

SIDE EFFECTS
  None.

===========================================================================*/
void ui_kpd_sub_cb( kpd_handle_type kpd_handle )
{
  ui_kpd_handle = kpd_handle;
}


/* <EJECT> */
/*===========================================================================
FUNCTION ui_key_init

DESCRIPTION
  Initializes the key handler and subscribes to the keypad key events.

DEPENDENCIES
  None.

RETURN VALUE
  Nothing

SIDE EFFECTS
  None.

===========================================================================*/
void ui_key_init( void )
{
  (void) kpd_sub_srvc(ui_kpd_sub_cb);

  if (ui_kpd_handle.status == HS_SUBSCRIBED) {
    (void) kpd_reg_key_event(ui_kpd_handle, ui_key_event_cb);

#ifdef FEATURE_USB_CARKIT
    usb_otg_cc_set_evt_hdlr(ui_usb_event_cb);

    if(usb_otg_cc_is_carkit_available()) {
      ui_usb_event_cb(USB_OTG_CC_EVENT__CARKIT_AVAILABLE);
    }
#endif
  }
  else {
    ERR_FATAL("Couldn't subscribe to KEYPAD service.",0,0,0);
  }
}

/*===========================================================================

FUNCTION ui_lcd_sub_cb

DESCRIPTION
  Handle LCD display driver call back.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ui_lcd_sub_cb
(
  lcd_handle_type lcd_handle
)
{
  ui_lcd_handle = lcd_handle;

  if (ui_lcd_handle.status != HS_SUBSCRIBED) {
    ERR_FATAL("Couldn't subscribe to LCD service.",0,0,0);
  }
}

/*===========================================================================

FUNCTION ui_lcd_init

DESCRIPTION
  Sets up the screen handlers' internal variables, blanks the screen,
  and subscribes to the LCD display driver services.

DEPENDENCIES
  Called from UI_TASK

RETURN VALUE
  None

SIDE EFFECTS
  Only as above

===========================================================================*/
void ui_lcd_init ( void )
{

  /* Subscribe to LCD display driver services */
  (void) lcd_sub_srvc(ui_lcd_sub_cb);

}



/*===========================================================================

FUNCTION ui_get_lcd_handle

DESCRIPTION
  Gets the LCD_HANDLE

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  Only as above

===========================================================================*/
lcd_handle_type ui_get_lcd_handle(void)
{
        return ui_lcd_handle;
}


#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/*===========================================================================
FUNCTION SetLCDOff

DESCRIPTION
  Sets the display off

  If a call is up during that time then the display will not switched off.
  The timer is setup to check after 20 seconds.

DEPENDENCIES
  If any call

SIDE EFFECTS
  None
===========================================================================*/
void SetLCDOff(void)
{
  if(ui_display) {
    int nReturnStatus;
    IShell *shell_ptr = AEE_GetShell();
    ICM *pICM = NULL;

    if (shell_ptr != NULL) {
      /* Create the Call Manager object. */
      nReturnStatus = ISHELL_CreateInstance(shell_ptr,
                                AEECLSID_CM,
                                (void **) &pICM);

      if(nReturnStatus != SUCCESS) {
        return;
      }

      if (pICM == NULL) { /* error */
        return;
      }

if(ui_call_pres_previously == FALSE) // if the call was presntly previously also then no need to extend the timer
{
      if(AEECM_IS_ANYCALL_PRESENT(pICM)) {
        CoreTask_SetLCDTimer(lcd_timer);
        ICM_Release(pICM);
        pICM = NULL;
        ui_call_pres_previously = TRUE;
        return;
      }
      }
      if(pICM){
        ICM_Release(pICM);
        pICM = NULL;
      }
    }

    // if everything is fine until this point, then turn off the display.
    HandleBacklights(FALSE, ui_backlight_duration );
    disp_off();
    //powerdown the display to save the LCD current as we would turn it
    //on when we turnon the backlight
    disp_powerdown();
#if defined(FEATURE_SECONDARY_DISPLAY)
    disp_off2();
    //powerdown the display to save the LCD current as we would turn it
    //on when we turnon the backlight
    disp_powerdown2();
#endif
#ifdef T_FFA6650_PANDORA
    pm_vote_vreg_switch(PM_OFF_CMD,PM_VREG_MMC_ID,PM_VOTE_VREG_MMC_APP__LCD);
#else
    pm_vote_vreg_switch(PM_OFF_CMD,PM_VREG_BOOST_ID,PM_VOTE_VREG_BOOST_APP__LCD);
#endif
    ui_display = FALSE;
    ui_call_pres_previously = FALSE;  //reset the global variable for the next call
  }

}

/*===========================================================================
FUNCTION CoreTask_SetLCDTimer

DESCRIPTION
  Set the Brew timer - If the user has chosen to set the LCD off after "timer" ms
  Else cancel timer.

DEPENDENCIES
  Will set the LCD off after "timer" milliseconds

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetLCDTimer(uint32 timer)
{

  IShell* pShell = AEE_GetShell();

  if (pShell == NULL) {
   MSG_ERROR ("Cannot get Shell pointer", 0,0,0);
   return;
  }

  lcd_timer = timer;

  ISHELL_CancelTimer(pShell,
                       (PFNNOTIFY) SetLCDOff,
                       0);

  if(timer != 0) {
    ISHELL_SetTimer(pShell,
                    timer,
                    (PFNNOTIFY) SetLCDOff,
                    0);
  }
 }
#endif // !FEATURE_MANGO_UI
#endif /* !FEATURE_UIONE_HDK */

/*===========================================================================
FUNCTION CoreTask_init

DESCRIPTION
  Initialize the user interface - called immediately after UI_TASK is
  started.  Initialize screen handler, queues, timers, etc.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from CoreTask_Init

===========================================================================*/
void CoreTask_init( void )
{

#if MIN_BREW_VERSION(4,0)
  IShell *piShell;
#endif

  uisnd_snd_init();               /* packets for talking to sound task */

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( &ui_tcb, UI_RPT_TIMER_SIG );

#ifdef FEATURE_WMS_APP
  /* Register this callback to WMS.  This callback is for the purpose of
  ** sharing cache that is stored in WMS.  WMS does NOT have to be
  ** initialized for this callback to be registered.
  */
  wms_reg_msg_info_cache_cb(OEMWMS_MsgInfoCacheCb);
#endif /* FEATURE_WMS_APP */

  /* tell the handset task that we want key signals to come to us */
  ui_key_init();
  ui_lcd_init();
#if (!defined(FEATURE_UIONE_HDK) && !defined(FEATURE_MANGO_UI))
  CoreTask_SetPowerUp(TRUE);
#endif /* !FEATURE_UIONE_HDK */

#ifndef T_WINNT
#ifdef FEATURE_IXCONTENTSVC
  /* Synchronize with the IxContent task and then register with IPC. */
  IxContentIPCSync ();
  ixipc_init( IPCSVC_IXCONTENTHANDLER, UI_IPC_SIG );
#else
#ifdef FEATURE_REX_IPC
  /* Register the IPC signal */
  rex_register_receive_signal(UI_IPC_SIG);
  ipcns_node_register(rex_self());
#endif /* FEATURE_REX_IPC */
#endif /* FEATURE_IXCONTENTSVC */
#endif /* T_WINNT */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#ifndef T_WINNT
  ghdi_nvmem_register_task(UI_NV_SIG, ui_wait, NULL);
#endif /* T_WINNT */
#endif

#if !MIN_BREW_VERSIONEx(3,1,4)
#if MIN_BREW_VERSION(3,1)
 (void) bridle_Init(NULL);
#else
#if MIN_BREW_VERSION(3,0)
   bridle_InitSWITable(NULL);
#endif
#endif
#endif /* !MIN_BREW_VERSIONEx(3,1,4) */

     InitResources(); /* set up resource files */
#if MIN_BREW_VERSION(4,0)
   (void) AEE_Init(gpiRexEnv, &piShell);
   #ifdef FEATURE_TOUCHSCREEN
   	nErr = IEnv_CreateInstance(gpiRexEnv, AEECLSID_TOUCHSCREEN, (void **)&piTouchscreen);
   	if (AEE_SUCCESS != nErr) {
   		nErr = 2;
    	}
    	else {
    	  IQI_RELEASEIF(piTouchscreen);
	 	}
   #endif //FEATURE_TOUCHSCREEN
   k1ExHandler_SetExceptionHandlerOf(ui_tcb.thread_id);
#else
   (void) AEE_Init(AEE_APP_SIG);
#endif

#if defined(FEATURE_BREWAPPCOORD)
   /* Initialize BREW App Coordination */
   (void) BAC_Init();
#endif

   VoteForSleep(TRUE);

} /* end of CoreTask_Init */

#ifndef FEATURE_UIONE_HDK
#ifndef FEATURE_MANGO_UI
/*===========================================================================
FUNCTION GetDisplayStatus

DESCRIPTION
  Returns status of  flag ui_display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean GetDisplayStatus(void)
{
  return ui_display;
}

/*===========================================================================
FUNCTION SetKeySilenceStatus

DESCRIPTION
  set ui_silence_key

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetKeySilenceStatus(boolean status)
{
  ui_silence_key = status;
}
#endif // !FEATURE_MANGO_UI
#endif // !FEATURE_UIONE_HDK

#if defined(FEATURE_UIONE_HDK) && defined(FEATURE_UIXSND_IMPLEMENTATION) && defined(FEATURE_TTY)
int CoreUI_SetTTYDeviceStatus(uisnd_device_status  status)
{
   IShell *pIShell = AEE_GetShell();
   IConfig *pIConfig = NULL;
   int result;

   // Create an IConfig interface
   result = ISHELL_CreateInstance(
      pIShell,
      AEECLSID_CONFIG,
      (void **)&pIConfig
   );

   if (SUCCESS == result) {
      uint32 ttyMode  = TTY_OFF;

      if (SUCCESS == ICONFIG_GetItem(pIConfig, (int) CFGI_TTY,
                                    (void *) &ttyMode, sizeof(uint32))) {

         if (ttyMode == TTY_FULL) {
            uisnd_set_device_status(SND_DEVICE_TTY_HEADSET, status);
         }
         else if (ttyMode == TTY_HEAR) {
            uisnd_set_device_status(SND_DEVICE_TTY_HCO, status);
         }
         else if (ttyMode == TTY_TALK) {
            uisnd_set_device_status(SND_DEVICE_TTY_VCO, status);
         }
      }

      (void) ICONFIG_Release(pIConfig);
      pIConfig = NULL;
   }

   return result;
}
#endif

//lint -restore

