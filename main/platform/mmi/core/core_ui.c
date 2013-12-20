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

$Header: //depot/asic/qsc1100/core_ui.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/09   suj     Changes to support HS_SWITCH interface for Headset.
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
#ifdef FEATURE_OEMOMH
#include "OEMOMH.h"
#endif
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "OEMFeatures.h"
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
#include "core_ui.h"

#include "fs_public.h"

#if defined(FEATURE_SLEEP) || defined(FEATURE_NEW_SLEEP_API)
#include "sleep.h"
#endif

#include "wms.h"
#include "OEMWMS.h"
#ifdef  FEATURE_CDSMS_CACHE_USELIST
#include "OEMWMS_CacheInfo.h"
#endif
#ifdef FEATURE_BREW_3_0
#include "AEE_OEMEvent.h"
#endif

#ifdef FEATURE_JAVA
#error code not present
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
#include "OEMClassIDs.h"



//Add By zzg 2010_12_08
#include "AEEBacklight.h"
#include "AEEAlert.h"
#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "OEMDeviceNotifier.h"

#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
extern boolean bIsResetOemNv;
#endif

static IBacklight   *gpBacklight = NULL;
#if defined(FEATURE_BACKLIGHT_KEYPAD)
static IBacklight   *gpKeyBacklight = NULL;
#endif
static IALERT       *gpAlert = NULL;
#ifdef FEATURE_ICM
static ICM          *gpICM = NULL;
#else
static ITelephone          *g_pITelephone = NULL;
#endif
static boolean      m_isBacklight = FALSE;
//Add End
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
#define UI_KEY_HOLD_TMS         1800
#define UI_KEY_REPEAT_START_TMS 1500
#define UI_KEY_REPEAT_TMS       150

#ifdef FEATURE_OTASP
static boolean ui_pwrdown_complete = FALSE;
#endif
static boolean send_keys = FALSE;
static boolean ui_ok_to_sleep = FALSE;
static kpd_handle_type ui_kpd_handle; /* Keypad Handle */
static lcd_handle_type ui_lcd_handle; /* LCD handle */

#define UI_KEY_TYPE byte
typedef enum {
    AVK_METHED_NONE,    // NONE
    AVK_METHED_REPT,    // Repeat
    AVK_METHED_HOLD,    // Hold
    AVK_METHED_MAX
}AVKMethod;

typedef struct {
  hs_key_type        hs_key;      /* HS key              */
  AVKType            aee_vcode;   /* AEE Virtual keycode */
  AVKMethod          aee_method;  /* Key Method          */
  boolean            bpressed;    /* Key Pressed         */
  uint32             dwparam;     /* dwparam to send with event */
} hs_to_aee_key_type;

static hs_to_aee_key_type hs_to_aee_tbl[] =
{
    {HS_0_K,                        AVK_0,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_1_K,                        AVK_1,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_2_K,                        AVK_2,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_3_K,                        AVK_3,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_4_K,                        AVK_4,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_5_K,                        AVK_5,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_6_K,                        AVK_6,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_7_K,                        AVK_7,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_8_K,                        AVK_8,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_9_K,                        AVK_9,                          AVK_METHED_HOLD, FALSE, 0},
    {HS_STAR_K,                     AVK_STAR,                       AVK_METHED_HOLD, FALSE, 0},
    {HS_POUND_K,                    AVK_POUND,                      AVK_METHED_HOLD, FALSE, 0},

    {HS_INFO_K,                     AVK_INFO,                       AVK_METHED_HOLD, FALSE, 0},
    {HS_PWR_K,                      AVK_POWER,                      AVK_METHED_HOLD, FALSE, 0},
    {HS_END_K,                      AVK_END,                        AVK_METHED_HOLD, FALSE, 0},
    {HS_SEND_K,                     AVK_SEND,                       AVK_METHED_HOLD, FALSE, 0},
	{HS_CLR_K,                      AVK_CLR,                        AVK_METHED_HOLD, FALSE, 0},
    {HS_SEL_K,                      AVK_SELECT,                     AVK_METHED_HOLD, FALSE, 0},

    {HS_UP_K,                       AVK_UP,                         AVK_METHED_REPT, FALSE, 0},
    {HS_DOWN_K,                     AVK_DOWN,                       AVK_METHED_REPT, FALSE, 0},
    {HS_LEFT_K,                     AVK_LEFT,                       AVK_METHED_REPT, FALSE, 0},
    {HS_RIGHT_K,                    AVK_RIGHT,                      AVK_METHED_REPT, FALSE, 0},
    {HS_VOL_DOWN_K,                 AVK_VOLUME_DOWN,                AVK_METHED_REPT, FALSE, 0},
    {HS_VOL_UP_K,                   AVK_VOLUME_UP,                  AVK_METHED_REPT, FALSE, 0},
    
    {HS_CAMERA_K,                   AVK_CAMERA,                     AVK_METHED_HOLD, FALSE, 0},
    {HS_TV_K,                       AVK_TV,                         AVK_METHED_HOLD, FALSE, 0},
    {HS_FM_K,                       AVK_FM,                         AVK_METHED_HOLD, FALSE, 0},
    {HS_MUSIC_K,                    AVK_MUSIC,                      AVK_METHED_HOLD, FALSE, 0},
    {HS_FFWD_K,                     AVK_FFWD,                      	AVK_METHED_HOLD, FALSE, 0},
    {HS_RWD_K,                      AVK_RWD,                      	AVK_METHED_HOLD, FALSE, 0},
    {HS_PAUSE_K,                    AVK_PAUSE,                      AVK_METHED_HOLD, FALSE, 0},
    
    {HS_FLIP_OPEN_K,                AVK_FLIP_OPEN,                  AVK_METHED_NONE, FALSE, 0},
    {HS_FLIP_CLOSE_K,               AVK_FLIP_CLOSE,                 AVK_METHED_NONE, FALSE, 0},
    
    // Headset
    {HS_HEADSET_K,                  AVK_HEADSET_CONNECT,            AVK_METHED_NONE, FALSE, 1},
    {HS_STEREO_HEADSET_K,           AVK_HEADSET_CONNECT,            AVK_METHED_NONE, FALSE, 1},
    {HS_HEADSET_OFF_K,              AVK_HEADSET_DISCONNECT,         AVK_METHED_NONE, FALSE, 0},
    {HS_HEADSET_SWITCH_K,           AVK_HEADSET_SWITCH,             AVK_METHED_NONE, FALSE, 0}, //Key Press
    {HS_HEADSET_SWITCH_OFF_K,       AVK_HEADSET_SWITCH_OFF,         AVK_METHED_NONE, FALSE, 0}, //Key Long Press
   
#ifdef FEATURE_ALL_KEY_PAD
    {HS_Q_K,						AVK_Q,							AVK_METHED_HOLD, FALSE, 0},
	{HS_W_K,						AVK_W,							AVK_METHED_HOLD, FALSE, 0},
	{HS_E_K,						AVK_E,							AVK_METHED_HOLD, FALSE, 0},
	{HS_R_K,						AVK_R,							AVK_METHED_HOLD, FALSE, 0},
	{HS_T_K,						AVK_T,							AVK_METHED_HOLD, FALSE, 0},
	{HS_Y_K,						AVK_Y,							AVK_METHED_HOLD, FALSE, 0},
	{HS_U_K,						AVK_U,							AVK_METHED_HOLD, FALSE, 0},
	{HS_I_K,						AVK_I,							AVK_METHED_HOLD, FALSE, 0},
	{HS_O_K,						AVK_O,							AVK_METHED_HOLD, FALSE, 0},
	{HS_P_K,						AVK_P,							AVK_METHED_HOLD, FALSE, 0},
	{HS_A_K,						AVK_A,							AVK_METHED_HOLD, FALSE, 0},
	{HS_S_K,						AVK_S,							AVK_METHED_HOLD, FALSE, 0},
	{HS_D_K,						AVK_D,							AVK_METHED_HOLD, FALSE, 0},
	{HS_F_K,						AVK_F,							AVK_METHED_HOLD, FALSE, 0},
	{HS_G_K,						AVK_G,							AVK_METHED_HOLD, FALSE, 0},
	{HS_H_K,						AVK_H,							AVK_METHED_HOLD, FALSE, 0},
	{HS_J_K,						AVK_J,							AVK_METHED_HOLD, FALSE, 0},
	{HS_K_K,						AVK_K,							AVK_METHED_HOLD, FALSE, 0},
	{HS_L_K,						AVK_L,							AVK_METHED_HOLD, FALSE, 0},
	{HS_SHIFT_K,					AVK_SHIFT,						AVK_METHED_HOLD, FALSE, 0},
	{HS_Z_K,						AVK_Z,							AVK_METHED_HOLD, FALSE, 0},
	{HS_X_K,						AVK_X,							AVK_METHED_HOLD, FALSE, 0},
	{HS_C_K,						AVK_C,							AVK_METHED_HOLD, FALSE, 0},
	{HS_V_K,						AVK_V,							AVK_METHED_HOLD, FALSE, 0},
	{HS_B_K,						AVK_B,							AVK_METHED_HOLD, FALSE, 0},
	{HS_N_K,						AVK_N,							AVK_METHED_HOLD, FALSE, 0},
	{HS_M_K,						AVK_M,							AVK_METHED_HOLD, FALSE, 0},
	{HS_DOLLAR_K,					AVK_RWD,					    AVK_METHED_HOLD, FALSE, 0},
	{HS_ENTER_K,					AVK_ENTER,						AVK_METHED_HOLD, FALSE, 0},
	{HS_CAPLK_K,					AVK_CAPLK,						AVK_METHED_HOLD, FALSE, 0},
	{HS_SPACE_K,					AVK_SPACE,						AVK_METHED_HOLD, FALSE, 0},
	{HS_SYMBOL_K,					AVK_SYMBOL,						AVK_METHED_HOLD, FALSE, 0},
	{HS_LCTRL_K,					AVK_LCTRL,						AVK_METHED_HOLD, FALSE, 0},
	#ifndef FEATURE_LANG_THAI
	#if defined(FEATURE_NUM_KEY_MID)
	{HS_F1_K,					    AVK_MESSAGE,				    AVK_METHED_HOLD, FALSE, 1},
	{HS_F2_K,					    AVK_MUSIC,						AVK_METHED_HOLD, FALSE, 1},
	#elif defined(FEATURE_VERSION_S1000T)
	{HS_F1_K,					    AVK_MESSAGE,						    AVK_METHED_HOLD, FALSE, 1},
	{HS_F2_K,					    AVK_MUSIC,						AVK_METHED_HOLD, FALSE, 1},
	#else
	{HS_F1_K,					    AVK_UP,						    AVK_METHED_HOLD, FALSE, 1},
	{HS_F2_K,					    AVK_DOWN,						AVK_METHED_HOLD, FALSE, 1},
	#endif
	#else
	{HS_F1_K,					    AVK_MESSAGE,				    AVK_METHED_HOLD, FALSE, 1},
	{HS_F2_K,					    AVK_MUSIC,						AVK_METHED_HOLD, FALSE, 1},
	#endif
	{HS_F3_K,					    AVK_DEL,						AVK_METHED_HOLD, FALSE, 0},
	{HS_F4_K,					    AVK_MUTE,						AVK_METHED_HOLD, FALSE, 1},
    {HS_F5_K,					    AVK_UP,						    AVK_METHED_HOLD, FALSE, 1},
	{HS_MUSIC_K,					AVK_MUSIC,						AVK_METHED_HOLD, FALSE, 1},
	{HS_BLUETOOTH_K,				AVK_BLUETOOTH,					AVK_METHED_HOLD, FALSE, 1},
#endif
	{HS_NONE_K,                     AVK_UNDEFINED,                  AVK_METHED_NONE, FALSE, 0},
};
AVKType    last_vcode = AEE_INVALID_CODE;
uint32	   dwParam_code = 0;		//Add By zzg 2010_09_09

LOCAL struct {
  byte rd_idx;                  /* read index                */
  byte wr_idx;                  /* write index               */
  UI_KEY_TYPE data[ UI_KEY_BUF_SIZE ]; /* data buffer        */
#ifdef FEATURE_KEYPAD_MULTI_KEY
  UI_KEY_TYPE key_parm[UI_KEY_BUF_SIZE];  /* key parm buffer */
#endif //FEATURE_KEYPAD_MULTI_KEY
} ui_key_buffer;                /* keycode input ring buffer */


#ifdef FEATURE_NEW_SLEEP_API
extern sleep_okts_handle gNewSleepHandle;
#endif

#if MIN_BREW_VERSION(4,0)
static AEECallback KeyHandlerCb = {0}; /* callback info for the key handler */
static boolean bBREWIsInitialized = FALSE; /* Boolean to keep track of whether BREW has been initialized. */
#endif

static void StopKeyRepeat(hs_to_aee_key_type *ptbl);
static void StopKeyHold(hs_to_aee_key_type *ptbl);

extern void keypad_set_backlight( byte level );
static boolean CoreTask_HandleAEEEvt(AEEEvent evt, uint16 wParam, uint32 dwParam);
static void CoreTask_CreateAEEInstance(void);
static void CoreTask_FreeAEEInstance(void);
void VoteForSleep (boolean flag);

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
  if(new_value) // 避免关机过程中发生进入睡眠的状况
  {
    VoteForSleep(FALSE);
  }
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
  
  if(ui_pwrdown_complete && !is_sleeping) {
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
  if(send_keys)
  {
    (void) rex_set_sigs (&ui_tcb, UI_KEY_SIG);
  }
}

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
    StopKeyRepeat(NULL);
    StopKeyHold(NULL);
    
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
        CoreTask_FreeAEEInstance();
        AEE_Exit();
#ifndef USES_RELEASE_VERSION
        //OEMOS_DbgDump();
#endif
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
    CoreTask_FreeAEEInstance();
    AEE_Exit();
#ifndef USES_RELEASE_VERSION
    //OEMOS_DbgDump();
#endif
    ui_complete_pwrdown();
#endif /* FEATURE_OTASP */
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
} /* CoreAppHandleOfflineSig */


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
  UI_KEY_TYPE keycode;

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

/*===========================================================================
FUNCTION GetHsToVcodeIdx

DESCRIPTION
   This function translates HS key to AEE vcode.

DEPENDENCIES

RETURN VALUE
  int:   the index of hs_to_aee_tbl

SIDE EFFECTS

===========================================================================*/
static int GetHsToVcodeIdx(hs_key_type key)
{
    int i;
  
    for (i= 0; i< ARR_SIZE(hs_to_aee_tbl); i++)
    {
        if (hs_to_aee_tbl[i].hs_key == key) {
            MSG_HIGH("Returning key %d (%x) and param %d", hs_to_aee_tbl[i].aee_vcode, hs_to_aee_tbl[i].aee_vcode, hs_to_aee_tbl[i].dwparam);
            return i;
        }
    }
    return i-1;
} /* End GetHsToVcodeIdx */

static void KeyRepeatCB(void *pUser)
{
    hs_to_aee_key_type *ptbl = (hs_to_aee_key_type *)pUser;
    AEE_Event(EVT_KEY, ptbl->aee_vcode, ptbl->dwparam|KB_AUTOREPEAT);
    if(ptbl->bpressed)
    {
        AEE_SetSysTimer(UI_KEY_REPEAT_TMS, KeyRepeatCB, ptbl);
    }
}

static void StartKeyRepeat(hs_to_aee_key_type *ptbl)
{
    if(ptbl->bpressed)
    {
        AEE_SetSysTimer(UI_KEY_REPEAT_START_TMS, KeyRepeatCB, ptbl);
    }
}

static void StopKeyRepeat(hs_to_aee_key_type *ptbl)
{
    AEE_CancelTimer(KeyRepeatCB, ptbl);
}

static void KeyHoldCB(void *pUser)
{
    hs_to_aee_key_type *ptbl = (hs_to_aee_key_type *)pUser;

//Add By zzg 2010_11_23
#ifdef FEATURE_UNLOCK_KEY_SPACE
#ifdef FEATURE_KEYGUARD
	extern boolean OEMKeyguard_HandleEvent(AEEEvent  evt, uint16    wParam,uint32 dwParam); 

	if (TRUE == (OEMKeyguard_HandleEvent(EVT_KEY_HELD, ptbl->aee_vcode,ptbl->dwparam)))
	{
		
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
		if (AVK_SPACE == ptbl->aee_vcode)
		{
		
#if defined(FEATURE_BACKLIGHT_KEYPAD)
	        if (gpKeyBacklight)
			{
	            IBACKLIGHT_Enable(gpKeyBacklight);
	        }
#endif
	        if (gpBacklight)
			{
				if(!IBACKLIGHT_IsEnabled(gpBacklight))
				{
					m_isBacklight = FALSE;
					MSG_FATAL("IBACKLIGHT_Enable........000",0,0,0);
	            	IBACKLIGHT_Enable(gpBacklight);
				}
				else
				{
					m_isBacklight = TRUE;
				}
	        }
	        
	        if (gpAlert)
			{
	            IALERT_KeyBeep(gpAlert, (AVKType)AVK_SPACE, TRUE);
	        }
		
		}
#endif
		return;
	}		
#endif
#endif
//Add End

	AEE_Event(EVT_KEY_HELD, ptbl->aee_vcode, ptbl->dwparam);
}

static void StartKeyHold(hs_to_aee_key_type *ptbl)
{
    if(ptbl->bpressed)
    {
        AEE_SetSysTimer(UI_KEY_HOLD_TMS, KeyHoldCB, ptbl);
    }
}

static void StopKeyHold(hs_to_aee_key_type *ptbl)
{	
    AEE_CancelTimer(KeyHoldCB, ptbl);
}

void pointer_keyeablebacklight(void)
{
	IBACKLIGHT_Enable(gpBacklight);
}

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

void handle_keys(void)
{
#ifdef FEATURE_KEYPAD_MULTI_KEY
    keypad_key_event_type   key;
#else
    kpd_key_event_type  key;               /* hold key temporarily */
#endif /* FEATURE_KEYPAD_MULTI_KEY */

    /* static locals are used to remember the last pressed key for
     * handling events when the keypad doesn't support pressing multiple keys
    */
    
    static int hs2vcodeidx = ARR_SIZE(hs_to_aee_tbl)-1;
    last_vcode = AEE_INVALID_CODE;
#ifdef FEATURE_KEYPAD_MULTI_KEY
    while ( (key.key_code = GetKey(&key)) != HS_NONE_K) 
	{
        MSG_HIGH("Received Key code = %#x and key parm = %#x from HS", key.key_code, key.key_parm, 0);
		
#else /* FEATURE_KEYPAD_MULTI_KEY */
    while ( ( key.key_code = (hs_key_type)GetKey()) != HS_NONE_K ) 
	{
        MSG_HIGH("Received Key %d (%x) from HS", key.key_code, key.key_code, 0);
#endif
        if (key.key_code == HS_RELEASE_K) 
		{
#ifdef FEATURE_KEYPAD_MULTI_KEY
            hs2vcodeidx = GetHsToVcodeIdx(key.key_parm);
#endif
            last_vcode = hs_to_aee_tbl[hs2vcodeidx].aee_vcode;
            dwParam_code = hs_to_aee_tbl[hs2vcodeidx].dwparam;	//Add By zzg 2010_09_09
            
            if (last_vcode != AVK_UNDEFINED && hs_to_aee_tbl[hs2vcodeidx].bpressed)
			{
                hs_to_aee_tbl[hs2vcodeidx].bpressed = FALSE;

                if (CoreTask_HandleAEEEvt(EVT_KEY_RELEASE, last_vcode, dwParam_code))
				{
                    continue;
                }
				
                if(hs_to_aee_tbl[hs2vcodeidx].aee_method == AVK_METHED_REPT)
				{
                    StopKeyRepeat(&hs_to_aee_tbl[hs2vcodeidx]);
                }
				else if(hs_to_aee_tbl[hs2vcodeidx].aee_method == AVK_METHED_HOLD)
                {                	
                    StopKeyHold(&hs_to_aee_tbl[hs2vcodeidx]);
                }

                AEE_Event(EVT_KEY_RELEASE, last_vcode, hs_to_aee_tbl[hs2vcodeidx].dwparam);
            }
        }
		else
		{
            hs2vcodeidx = GetHsToVcodeIdx(key.key_code);
            last_vcode = hs_to_aee_tbl[hs2vcodeidx].aee_vcode;
            dwParam_code = hs_to_aee_tbl[hs2vcodeidx].dwparam;	//Add By zzg 2010_09_09
			           
            if(last_vcode != AVK_UNDEFINED)
			{	
                CoreTask_HandleAEEEvt(EVT_KEY_PRESS, last_vcode, dwParam_code);

                if (CoreTask_HandleAEEEvt(EVT_KEY, last_vcode, dwParam_code))
				{
                    continue;
                }	

                hs_to_aee_tbl[hs2vcodeidx].bpressed = TRUE;
               
                (void) AEE_Event(EVT_KEY_PRESS, last_vcode, hs_to_aee_tbl[hs2vcodeidx].dwparam);
                (void) AEE_Event(EVT_KEY,       last_vcode, hs_to_aee_tbl[hs2vcodeidx].dwparam);
				
                if(hs_to_aee_tbl[hs2vcodeidx].aee_method == AVK_METHED_REPT)
				{
                    StartKeyRepeat(&hs_to_aee_tbl[hs2vcodeidx]);
                }
				else if(hs_to_aee_tbl[hs2vcodeidx].aee_method == AVK_METHED_HOLD)
				{
                    StartKeyHold(&hs_to_aee_tbl[hs2vcodeidx]);
                }
            }  // end of else
        }
    } /* while (key = GetKey() */
}

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

    if (send_keys)
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
      VoteForSleep(TRUE); /* okay to sleep */
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
      VoteForSleep(TRUE); /* okay to sleep */
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
  if ( sigs & UI_RTC_SIG )
  {
    (void) rex_clr_sigs( rex_self(), UI_RTC_SIG );
    ISHELL_SendEvent( AEE_GetShell(), AEECLSID_CORE_APP, EVT_RTC, 0, 0);
    OEMRTC_Run_Call_Back_Function(); 
    //OEMRTC_Process_Timer_List();
  }
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
  sigs |= UI_RTC_SIG;
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
    "brew/sys/download",
    "brew/shared",
    "brew/mod",
    "brew/mif",
#endif
    "user",
    "user/sound",
    "user/sound/ringer",
  };
  int nDirNum = ARR_SIZE(brew_dirs); //Number of 'other' brew directories.
#ifdef FEATURE_VERSION_S600S  //add by yangdecai
	int m_read_from_nv =1;
	OEMNV_Get(NV_RTRE_CONFIG_I,&m_read_from_nv);
  
	if(m_read_from_nv != 1)
	{
	  m_read_from_nv = 1;
	  OEMNV_Put(NV_RTRE_CONFIG_I,&m_read_from_nv);
	}
#endif	

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

  if(!send_keys)
  {
    uint16 keycode = key_event.key_code;
#ifndef FEATURE_KEYPAD_MULTI_KEY
    static uint16 prevkey = HS_RELEASE_K;
#endif
    if(keycode == HS_RELEASE_K)
    {
#ifdef FEATURE_KEYPAD_MULTI_KEY
        keycode = key_event.key_parm;
#else
        keycode = prevkey;
#endif
    }
#ifndef FEATURE_KEYPAD_MULTI_KEY
    else
    {
        prevkey = keycode;
    }
#endif
    switch(key_event.key_code){
    case HS_HEADSET_K:
    case HS_STEREO_HEADSET_K:
    case HS_HEADSET_OFF_K:
        break;
    default:
        return;
    }
  }
  
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

  /* Register this callback to WMS.  This callback is for the purpose of
  ** sharing cache that is stored in WMS.  WMS does NOT have to be
  ** initialized for this callback to be registered.
  */
  
#if (defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST))
    // Register this callback to WMS.  This callback is for the purpose of
    // sharing cache that is stored in WMS.  WMS does NOT have to be
    // initialized for this callback to be registered.
#ifdef   FEATURE_CDSMS_CACHE
    wms_reg_msg_info_cache_cb(OEMWMS_MsgInfoCacheCb);
#else
    wms_reg_msg_info_cache_cb(OEMWMS_MsgInfoCacheCbExt);
#endif    
#endif

  /* tell the handset task that we want key signals to come to us */
  ui_key_init();
  ui_lcd_init();


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
#ifdef FEATURE_OEMOMH 
   MSG_ERROR("OMH: Calling OEMOMH_Init after AEE_Init()", 0, 0, 0);
   OEMOMH_Init();
#endif //FEATURE_MSHOP_DOWNLOAD_CONFIG_FOR_CT_AND_OMH
#endif
    CoreTask_CreateAEEInstance();
#if defined(FEATURE_BREWAPPCOORD)
   /* Initialize BREW App Coordination */
   (void) BAC_Init();
#endif
   
   VoteForSleep(TRUE);
} /* end of CoreTask_Init */

//lint -restore

#ifdef USES_MMI
/*==============================================================================
函数: 
    IsRunAsFactoryTestMode
       
说明: 
    函数用于判断当前软件是否按厂测模式运行。
       
参数: 
    none
    
返回值:
    TRUE: 当前软件运行为厂测模式
    FALSE: 当前软件运行为非厂测模式
       
备注:
    使用软件的版本是否有卡版本：无论 RTRE 配置如何，使用的软件版本要不无卡、要不
    有卡，二者只能取一，无其它选择!

==============================================================================*/
boolean   IsRunAsFactoryTestMode(void)
{
#ifndef WIN32
    nv_item_type nvi;

    if (NV_DONE_S != OEMNV_Get(NV_ESN_I, &nvi)) {
        return TRUE;
    }
    return (nvi.esn.esn == 0);
#else
    return TRUE;
#endif

}


/*==============================================================================
函数: 
    IsRunAsUIMVersion
       
说明: 
    函数用于判断当前软件是否按有卡版本运行。
       
参数: 
    none
    
返回值:
    TRUE: 当前软件运行为有卡版本
    FALSE: 当前软件运行为无卡?姹?
       
备注:
    使用软件的版本是否有卡版本：无论 RTRE 配置如何，使用的软件版本要不无卡、要不
    有卡，二者只能取一，无其它选择!

==============================================================================*/
boolean   IsRunAsUIMVersion(void)
{
#if defined(FEATURE_VERSION_W208S)||defined(FEATURE_VERSION_S600S)
    return FALSE;
#else
#ifdef FEATURE_UIM
    return TRUE;
#else
    return FALSE;
#endif
#endif
}
#endif

int oemui_is_lcd_test_state(int bb)
{
    static int g_is_lcd_test = 0;
    if (bb>=0)
    {
        g_is_lcd_test = bb;
    }
    return g_is_lcd_test;	
}

/*
#include "AEEBacklight.h"
#include "AEEAlert.h"
#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "OEMDeviceNotifier.h"

#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
extern boolean bIsResetOemNv;
#endif

static IBacklight   *gpBacklight = NULL;
#if defined(FEATURE_BACKLIGHT_KEYPAD)
static IBacklight   *gpKeyBacklight = NULL;
#endif
static IALERT       *gpAlert = NULL;
#ifdef FEATURE_ICM
static ICM          *gpICM = NULL;
#else
static ITelephone          *g_pITelephone = NULL;
#endif
static boolean      m_isBacklight = FALSE;
*/



/*==============================================================================
函数: 
    brewui_isincall
       
说明: 
    函数用于判断当前话机是否处于通话状态。
       
参数: 
    none
    
返回值:
    TRUE: 话机处于通话状态
    FALSE: 话机处于非通话状态
       
备注:
    Brew 必须已启动

==============================================================================*/
boolean   brewui_isincall(void)
{    
#ifdef FEATURE_ICM
    if (NULL == gpICM)
    {
        return FALSE;
    }

    return AEECM_IS_VOICECALL_CONNECTED(gpICM);
#else
	AEETCalls po;
	
    if (NULL == g_pITelephone)
    {
        return FALSE;
    }

	
	if(SUCCESS != ITELEPHONE_GetCalls(g_pITelephone, &po,sizeof(AEETCalls)))
	{
		return FALSE;
	}
	
    return po.dwCount>0?TRUE:FALSE;
#endif
}

static void CoreTask_CreateAEEInstance(void)
{
    IShell *pShell = AEE_GetShell();
    if(!pShell){
        lcd_message("BREW startup"
                        "failed.     ");
        
        VoteForSleep(TRUE);

        // send offline command to everybody else
        (void) cm_ph_cmd_oprt_mode(NULL,NULL,CM_CLIENT_ID_ANONYMOUS,SYS_OPRT_MODE_OFFLINE);
        return;
    }
    
#ifdef FEATURE_LOAD_DEFAULT_SETTING
    OEM_ImportOeminitdata(pShell);
#endif /*FEATURE_LOAD_DEFAULT_SETTING*/

#ifdef FEATRUE_AUTO_SET_NEED_NV_VALUE
    if(TRUE == bIsResetOemNv)
	{
	
//Add By zzg 2010_10_22
#ifdef FEATURE_APP_BLUETOOTH	
		ISHELL_StartBackgroundApplet(pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");		
		//ISHELL_StartAppletArgs(pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");	
#endif
//Add End   

        OEM_RestoreFactorySetting();
    }
#endif

    // 在这里创建 oemui task 用到的 BREW 的相关接口
    (void)ISHELL_RegisterNotify(pShell, AEECLSID_DIALER, AEECLSID_SHELL, NMASK_SHELL_START_STATUS);
    (void)ISHELL_RegisterNotify(pShell, AEECLSID_WMSAPP, AEECLSID_SHELL, NMASK_SHELL_START_STATUS);
    (void) ISHELL_CreateInstance(pShell, AEECLSID_BACKLIGHT, (void**)&gpBacklight);
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    (void) ISHELL_CreateInstance(pShell, AEECLSID_BACKLIGHT_KEYPAD, (void**)&gpKeyBacklight);
#endif
    (void) ISHELL_CreateInstance(pShell, AEECLSID_ALERT, (void**)&gpAlert);
#ifdef FEATURE_ICM
    (void) ISHELL_CreateInstance(pShell, AEECLSID_CM,    (void**)&gpICM);
#else
    (void) ISHELL_CreateInstance(pShell, AEECLSID_TELEPHONE,(void**)&g_pITelephone);
#endif

    // 背光处理
    // Turn on the backlight immediately on startup so the
    // user can see the startup animation at night 
    if (gpBacklight) {
        IBACKLIGHT_Enable(gpBacklight);
    }
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    if (gpKeyBacklight) {
        IBACKLIGHT_Enable(gpKeyBacklight);
    } 
#endif
}

static void CoreTask_FreeAEEInstance(void)
{
    // 释放 Brew 相关接口
    if (gpBacklight){
        IBACKLIGHT_Release(gpBacklight);
        gpBacklight = NULL;
    }
#if defined(FEATURE_BACKLIGHT_KEYPAD)
    if (gpKeyBacklight){
        IBACKLIGHT_Release(gpKeyBacklight);
        gpKeyBacklight = NULL;
    }
#endif
    if (gpAlert){
        IALERT_Release(gpAlert);
        gpAlert = NULL;
    }
#ifdef FEATURE_ICM
    if (gpICM) {
        ICM_Release(gpICM);
        gpICM = NULL;
    }
#else
    if (g_pITelephone) {
        ITELEPHONE_Release(g_pITelephone);
        g_pITelephone = NULL;
    }
#endif
}

static boolean bSendHeadsetKeyAllowed = FALSE;
static void HeadsetKeyAllowedCB(void *pUser)
{
    bSendHeadsetKeyAllowed = TRUE;
}
static boolean CoreTask_HandleAEEEvt(AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    AEECLSID cls;
    boolean  bHandle = FALSE;
#ifndef FEATURE_ICM
	AEETCalls po;
#endif
    
#ifdef FEATURE_KEYGUARD	
    
    if (OEMKeyguard_HandleEvent(evt, wParam,dwParam))
    {	
        return TRUE;
    }	
#endif
    MSG_FATAL("evt====%0x,,wParam====%0x",evt,wParam,0);
    // AVK_HEADSET_SWITCH means headset key press
    if (wParam == AVK_HEADSET_SWITCH && bSendHeadsetKeyAllowed) {
#ifdef FEATURE_ICM
        if(AEECM_IS_VOICECALL_CONNECTED(gpICM)){
#else
		if(SUCCESS != ITELEPHONE_GetCalls(g_pITelephone, &po,sizeof(AEETCalls)))
		{
			return FALSE;
		}
        if(po.dwCount>0){
#endif
            // AVK_END instead of AVK_HEADSET_SWITCH in conversation
            wParam = AVK_END;
        }else{
            // add by pyuangui 20121204
            #if defined( FEATURE_VERSION_W317A) || defined( FEATURE_VERSION_C316)
            if((AEE_Active()!=AEECLSID_APP_MUSICPLAYER) && (AEE_Active()!=AEECLSID_APP_FMRADIO))
		    #endif
        	{        	 
        	  MSG_FATAL("AVK_HEADSET_SWITCH---",0,0,0);
              // AVK_SEND instead of AVK_HEADSET_SWITCH
              wParam = AVK_SEND;
        	}
			// add end
        }
        bHandle = TRUE;
    }
    
    cls = AEE_Active();
    switch(wParam){
#if defined (FEATURE_FLEXI_STATIC_BREW_APP)||defined (FEATURE_VERSION_FLEXI203P)
    case AVK_CAPLK:
        if(cls == 0x01007262)
        {
            return TRUE;
        }
        break;
#endif

//Add by pyuangui 20121220
//#ifdef FEATURE_VERSION_W317A
     case AVK_CLR:			
	 	#if (defined(FEATURE_DISP_128X160)&& defined(FEATURE_QQ_APP))||(defined(FEATURE_DISP_128X128)&& defined(FEATURE_QQ_APP))
		if (cls == AEECLSID_UCWEB)
		#else
		if (cls == AEECLSID_UCWEB || cls == AEECLSID_QQ)		
		#endif
		{
			wParam = AVK_SOFT2;
			bHandle = TRUE;
		}
        break;
     case AVK_SELECT:		
	 	#if (defined(FEATURE_DISP_128X160)&& defined(FEATURE_QQ_APP))||(defined(FEATURE_DISP_128X128)&& defined(FEATURE_QQ_APP))
		if (cls == AEECLSID_UCWEB)
		#else
        if (cls == AEECLSID_UCWEB || cls == AEECLSID_QQ)		
		#endif
		{
			wParam = AVK_SOFT1;
            bHandle = TRUE;
		}
        break; 
    case AVK_INFO:			
		#if (defined(FEATURE_DISP_128X160)&& defined(FEATURE_QQ_APP))||(defined(FEATURE_DISP_128X128)&& defined(FEATURE_QQ_APP))
		if (cls == AEECLSID_UCWEB)
		#else
		if (cls == AEECLSID_UCWEB || cls == AEECLSID_QQ)		
		#endif
		{
			wParam = AVK_SELECT;
			bHandle = TRUE;
		}
        break;  
//#endif
//Add End

#ifdef FEATURE_VERSION_EC99      
    case AVK_FM:	
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }
		if ((cls != AEECLSID_APP_FMRADIO) && (cls != AEECLSID_DIALER) && (!brewui_isincall()) )	//0x100611f	
		{
            ISHELL_StartApplet(AEE_GetShell(), AEECLSID_APP_FMRADIO);				
		}
        else if (cls == AEECLSID_APP_FMRADIO)
        {
           ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_FMRADIO,
                              EVT_USER,
                              AVK_INFO,
                              0);
        }
        return TRUE; 
    }
        
    case AVK_PAUSE:
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }       

        if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }

        if ((cls != AEECLSID_APP_MUSICPLAYER) && (cls != AEECLSID_DIALER) && (!brewui_isincall()) )	//0x1006126	
		{
            ISHELL_StartApplet(AEE_GetShell(), AEECLSID_APP_MUSICPLAYER);				
		}
        else 
        {
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_PAUSE,
                              0);
        }
        return TRUE;
    }
    case AVK_FFWD:  
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }

        if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }
        
        //if (cls == AEECLSID_APP_MUSICPLAYER)	//pre //maybe background
		{
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_FFWD,
                              0);	
            return TRUE;
		}
        break;
    }
    case AVK_RWD:    
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }

        if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }
        
        //if (cls == AEECLSID_APP_MUSICPLAYER)    //next
		{
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_RWD,
                              0);
            return TRUE;
		}
        break;   
    }    
#endif

#ifdef FEATURE_VERSION_K212_20D
   if(!IsMp3PlayerStatusNone())
   	{
   case AVK_PAUSE:
    {
        MSG_FATAL("***zzg core_ui AVK_PAUSE cls=%x***", cls, 0, 0);   

        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }

		if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }
		
        if ((cls != AEECLSID_APP_MUSICPLAYER) && (cls != AEECLSID_DIALER) && (!brewui_isincall()) )	//0x1006126	
		{
            ISHELL_StartApplet(AEE_GetShell(), AEECLSID_APP_MUSICPLAYER);				
		}
        else 
        {
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_PAUSE,
                              0);
        }
        return TRUE;
    }
    case AVK_FFWD:  
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }

        if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }
        
        //if (cls == AEECLSID_APP_MUSICPLAYER)	//pre //maybe background
		{
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_FFWD,
                              0);	
            return TRUE;
		}
        break;
    }
    case AVK_RWD:    
    {
        if (cls == AEECLSID_QUICKTEST)
        {
            return FALSE;
        }

        if (cls == AEECLSID_APP_FMRADIO)
        {
            break;
        }
        
        //if (cls == AEECLSID_APP_MUSICPLAYER)    //next
		{
            ISHELL_PostEvent( AEE_GetShell(),
                              AEECLSID_APP_MUSICPLAYER,
                              EVT_USER,
                              AVK_RWD,
                              0);
            return TRUE;
		}
        break; 
    }
   	}
#endif

#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP 
	case AVK_CLR:			
		if ((cls == AEECLSID_SMARTFREN_MNEWS)
			|| (cls == AEECLSID_SMARTFREN_MSHOP)
			|| (cls == AEECLSID_SMARTFREN_MKARAOKE)
			|| (cls == AEECLSID_SMARTFREN_TWITTER)
			|| (cls == AEECLSID_SMARTFREN_FACEBOOK)
			|| (cls == AEECLSID_SMARTFREN_SATU)
			|| (cls == AEECLSID_SMARTFREN_SFM)
			|| (cls == AEECLSID_SMARTFREN_TEST)
			|| (cls == AEECLSID_CORE_APP)
			|| (cls == AEECLSID_SMARTFREN_MSHOP_HITZ233C)
			|| (cls == AEECLSID_SMARTFREN_FACEBOOK_HITZ233C)
			|| (cls == AEECLSID_SMARTFREN_TWITTER_HITZ233C)
			|| (cls == AEECLSID_SMARTFREN_MOKA_HITZ233C)
			|| (cls == AEECLSID_SMARTFREN_SFM_HITZ233C))		
		{
			if (dwParam == 0)
			{
				wParam = AVK_SOFT2;
				bHandle = TRUE;
			}
		}	
		break;
#endif
    case AVK_END:
				
		//Add By zzg 2010_11_27
		/*
		if (cls == AEECLSID_BLUETOOTH_APP)
			{	
				wParam = AVK_CANCEL;
				bHandle = TRUE;
			}
		//Add End
		*/
        if (cls == AEECLSID_DIALER) 
		{
            // AVK_END 会关掉全部程序，回到待机界面，这里做转换，避免此问题
            db_items_value_type dbItemValue;
            
            db_get(DB_IN_USE, &dbItemValue);			
			
            if (dbItemValue.in_use)
			{
                wParam = AVK_ENDCALL;
                bHandle = TRUE;
            }
        }
#ifdef FEATURE_APP_MUSICPLAYER
        if(cls==AEECLSID_APP_MUSICPLAYER){
            wParam = AVK_BGPLAY;
            bHandle = TRUE;
        }
#endif
        if(cls == AEECLSID_APP_SECURITYMENU){
            wParam = AVK_WITHDRAW;
            bHandle = TRUE;
        }
		if(cls == AEECLSID_QUICKTEST)
		{
			wParam = AVK_ENDCALL;
            bHandle = TRUE;
		}
        break;
        
    case AVK_HEADSET_CONNECT:
    case AVK_HEADSET_DISCONNECT:
        if (EVT_KEY_PRESS == evt)
        {
            ISHELL_PostEventEx( AEE_GetShell(),
                                EVTFLG_ASYNC,
                                AEECLSID_CORE_APP,
                                wParam == AVK_HEADSET_CONNECT?EVT_HEADSET_CONNECT:EVT_HEADSET_DISCONNECT,
                                0,
                                0
                               );
            
            if(wParam == AVK_HEADSET_DISCONNECT)
            {
                AEE_CancelTimer(HeadsetKeyAllowedCB, NULL);
                bSendHeadsetKeyAllowed = FALSE;
            }
            else
            {
                AEE_SetTimer(UI_KEY_REPEAT_START_TMS, HeadsetKeyAllowedCB, NULL);
            }
            
#if defined(FEATURE_BACKLIGHT_KEYPAD)
            if (gpKeyBacklight) {
                IBACKLIGHT_Enable(gpKeyBacklight);
            }
#endif
            if (gpBacklight){
				MSG_FATAL("IBACKLIGHT_Enable........0010",0,0,0);
                IBACKLIGHT_Enable(gpBacklight);
            }
        }
        return TRUE;
    case AVK_FLIP_OPEN:
        if (EVT_KEY_PRESS == evt){
#if defined(FEATURE_BACKLIGHT_KEYPAD)
            if (gpKeyBacklight) {
                IBACKLIGHT_Enable(gpKeyBacklight);
            }
#endif
            if (gpBacklight){
				MSG_FATAL("IBACKLIGHT_Enable........0011",0,0,0);
                IBACKLIGHT_Enable(gpBacklight);
            }
            // 合盖打开
            AEE_Event (EVT_FLIP, TRUE, 0);
        }
        return TRUE;
        
    case AVK_FLIP_CLOSE:
        if (EVT_KEY_PRESS == evt){
            // 合盖关上
            AEE_Event (EVT_FLIP, FALSE, 0);
            if (NULL != gpBacklight)
            {
               IBACKLIGHT_TurnOff(gpBacklight);
            }
#ifdef FEATRUE_DEVICETYPE_FLIP
            if(brewui_isincall() == FALSE && cls != AEECLSID_CORE_APP && cls != AEECLSID_QUICKTEST)
            {
                ISHELL_CloseApplet(AEE_GetShell(), TRUE);
            }
            else if(brewui_isincall() && cls != AEECLSID_DIALER)
            {
                ISHELL_PostEvent( AEE_GetShell(),
                                  AEECLSID_DIALER,
                                  EVT_FLIP,
                                  0,
                                  0
                                  );
            }
#endif
        }
        return TRUE;
        
    default:
        break;
    }
    
    switch(evt){
    case EVT_KEY_PRESS:		
#if defined(FEATURE_BACKLIGHT_KEYPAD)
        if (gpKeyBacklight) {
            IBACKLIGHT_Enable(gpKeyBacklight);
        }
#endif
        if (gpBacklight){
			if(!IBACKLIGHT_IsEnabled(gpBacklight))
			{
				//ISHELL_PostEvent(AEE_GetShell(),AEE_Active(),EVT_USER_REDRAW,0,0L);
				m_isBacklight = FALSE;
				MSG_FATAL("IBACKLIGHT_Enable........0012",0,0,0);
            	IBACKLIGHT_Enable(gpBacklight);
				#ifndef FEATURE_VERSION_K212
				if (wParam == AVK_ENDCALL)
				{
					break;	//继续传给callapp to End call.
				}
				#endif
				if((cls == AEECLSID_CORE_APP))
				{
					ISHELL_PostEvent(AEE_GetShell(), 
                     AEECLSID_CORE_APP,
                     EVT_UPDATEIDLE,
                     0,0L);
				}
				
				return TRUE;

			}
			else
			{
				m_isBacklight = TRUE;
			}
        }
        
        if (gpAlert){
            	IALERT_KeyBeep(gpAlert, (AVKType) wParam, TRUE);
        }
        break;
        
    case EVT_KEY_RELEASE:
#if defined(FEATURE_BACKLIGHT_KEYPAD)
        if (gpKeyBacklight)
		{
			MSG_FATAL("IBACKLIGHT_Enable........001",0,0,0);
            IBACKLIGHT_Enable(gpKeyBacklight);
        }   
#endif

#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
		if (wParam == AVK_SPACE)
		{
			return FALSE;
		}
#endif
		
		if(!m_isBacklight)
		{
			return TRUE;
		}
        if (gpBacklight)
		{
            if(FALSE == IBACKLIGHT_IsEnabled(gpBacklight))
            {
                if(oemui_is_lcd_test_state(-1) == 1){
                    if(AVK_1 == wParam)
					{
                        oemui_is_lcd_test_state(0);
                    }
                }
				else
                {
                	MSG_FATAL("IBACKLIGHT_Enable........002",0,0,0);
					#ifdef FEATURE_VERSION_K212
					if(AVK_STAR != wParam)
					#endif
					{
                    	IBACKLIGHT_Enable(gpBacklight);
					}
                }
				return TRUE;
            }
			else
            {
            	MSG_FATAL("IBACKLIGHT_Enable........003",0,0,0);
                IBACKLIGHT_Enable(gpBacklight);
            }
        }
        if (gpAlert) {
			#ifdef FEATURE_VERSION_K212
			if((cls != AEECLSID_DIALER) &&(cls != AEECLSID_MAIN_MENU))
			#endif
			{
            	IALERT_KeyBeep(gpAlert, (AVKType) wParam, FALSE);
			}
        }
		
		
        break;
	case EVT_KEY:
		
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
		if (wParam == AVK_SPACE)
		{
			return FALSE;
		}
#endif
#ifndef FEATURE_VERSION_K212
		if (wParam == AVK_ENDCALL)
		{
			break;	//继续传给callapp to End call.
		}
#endif
#ifdef FEATURE_OEMOMH
        if (wParam == AVK_RIGHT)
		{
			//WMSUtil_SendVMNSim();
			//CallApp_StartCallTest();
		}
#endif
		if(!m_isBacklight)
		{
			return TRUE;
		}
		break;
        
    default:
        break;
    }
	
    if(bHandle == TRUE)
    {
        AEE_Event(evt, wParam, 0);
        if(wParam == AVK_ENDCALL)
        {
            // 在CallApp为顶层应用的情况下，需要通知后台应用AVK_END事件
            static NotifyKeyEvent g_KeyEvent;
            g_KeyEvent.eCode        = evt;
            g_KeyEvent.wParam       = AVK_END;
            g_KeyEvent.dwParam      = 0;
            g_KeyEvent.clsHandled   = AEECLSID_SHELL;
            AEE_Notify(AEECLSID_SHELL, (AVK_END << 16) | NMASK_SHELL_KEY, &g_KeyEvent);
        }
    }
    return bHandle;
}

void CoreTask_HandleUIMErr(void)
{
#ifdef FEATURE_ICM
    if (NULL == gpICM)
    {
        return;
    }
    MSG_FATAL("CoreTask_HandleUIMErr",0,0,0);
    ICM_EndAllCalls(gpICM);
#endif
}

