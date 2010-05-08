/*===========================================================================

FILE: CoreUtils.c

DESCRIPTION
   This file contains utility functions for Core app.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2002 - 2008 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreUtils.c#234 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
03/05/08   cvs     Fix TTY volumes based on scaling
12/21/07   cvs     Fix issues relating to slower CM init, and new ICard behavior
10/25/07   cvs     Lint fixes
10/10/07   clm     Remove added use of deprecated SPRINTF function and fix 
                   compiler warning.
09/28/07   clm     Add PLMN name update CFGI and get functions.
08/03/07   cvs     Featurize secondary display code
08/01/07   cvs     Fix issue where time display can go to 1 second updates
07/16/07   jks     Added featurization for SND_DEVICE_SDAC
06/13/07   cvs     Handle LIMITED_REGIONAL like LIMITED service
03/20/07   jas/ak  Adding support for persistent preference values.
03/13/07   ypmw    Add 5 sec option for LCD display and backlight
03/07/07   jas     Reimplement background images with overlays.
                   Support VGA background images.
02/28/07   cvs     Have restrictions menu fully track FDN
02/07/07   cvs     Change SetTimeofDay to use IUSRSYSCLOCK
02/05/07   ypmw    Fix the compiler errors for the changes of sound dev 
                   attach variables
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/13/06   sj      Do not send subscription available to CM unless we
                   have received the VALID card event. Fixes CR 107527.
12/06/06   RT      Included CoreService.h to fix compiler warnings
11/27/06   ypmw    Fix the compiler error
11/14/06   ypmw    Fix wlan compiler warnings
11/14/06   ypmw    Fix audio mutes due to key press (despite DTMF and key tone off)
09/19/06   jas     Adding band value to AEECMWLANPref structure.
09/08/06   jas     Removing menus that were related to the NetSettings app
09/06/06   ak      Added NULL Pointer checks for Network Selection Callback
09/01/06   ak      Cleanup whitespaces from earlier checkins
09/01/06   jas     Removing code that was replicated in NetSettings
08/31/06   jas     Eliminating NV_VALUE_ERROR_BOOLEAN
08/29/06   jas     Restoring changes that were erroneously reverted in prior
                   sidetone control checkin.
08/23/06   jks     Added support for WLAN Adhoc networks
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added menu to set IncallMixing state
08/08/06   joe     NetSettings related changes.
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
07/12/06   jks     Fixed compile warnings
06/15/06   cvs     Fix Klocwork errors
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
05/04/06   jas     Porting display mapping code from 7500
04/13/06   cvs     Move call control to its own file
03/13/06   jks     Added support for WLAN scan mode selection
03/14/06   jks     Removed WLAN prfile_id
03/07/06   cvs     Fix memory overwrites with small buffers in FormatTime()
                   and FormatDate()
02/17/06   cvs     Adding functionality to make game mode persistent
02/13/06   jas     Added support for GCF language selection
01/26/06   jks     Null ptr check
01/24/06   jas     Modified WATOI to explicitly convert empty strings to 0
12/20/05   ak      Fixed Compilation issues
12/05/05   jks     Added WLANScanModeChangeTimerExpired function, added SSID
                   filtering to CoreUtil_GetWLANCb, removed GetIPVersion and
                   SetIPVersion functions
11/21/05   jas     Support for disabling background images
11/07/05   ak      Do not allow phone operations for blocked pin case
10/21/05   RI      Updated the NITZ menu
10/20/05   sun     Removed support for BT-SAP
10/17/05   RI      Updated the CoreUtil_CancelGetNW() not to call the cancel
                   function since it is not set in the get networks and
                   terminate get networks commands.
10/07/05   RI      Updated the DisplayTimeWithRefresh to be called
                   only if LCD is on.
10/03/05   RI      Updated the GetTimeOfDay() function.
09/30/05   cvs     Fix EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED when idle dialog
                   is not the current dialog.
09/22/05   cvs     Support for Stereo USB Carkit
09/17/05   RI      Fixed GetTimeOfDay() if ISysClock is defined.
                     - added NULL checks
09/15/05   cvs     Fix subscription in SIM reinit.
09/05/05   RI      Enable user time using MM info
08/31/05   sun     Added support for BT-SAP
08/18/05   jas     Create conversion from hex text to binary buffer
08/08/05   aps     Fixed NV volume level lookup for sound devices
07/27/05   AT      Fixing compile errors
07/27/05   cvs     Code Review cleanups
07/25/05   sun     Added support for Idle Screen notification
07/20/05   AT      Added CFGI items for display_duration and back_light_intensity
                   NV item is the intensity except for 0, if we read a 0, then
                   we use the default value.  The value 0 is treated as special
                   since on some targets (6550), 0 makes the display unreadable
                   and we only want to use this if the user has actually requested
                   it.
07/12/05   AT      Fix issues related to the time displayed.
07/07/05   cvs     Remove global pointer to IConfig
06/30/05   jas     Call ICM_SetSubscriptionStatus IWIFI_Commit, when needed
                   Move IsSubscriptionAvail from CoreApp.c
06/29/05   cvs     Restore continuous beep len at powerup
06/28/05   cvs     Add menu to select sound device
06/23/05   jas     Switching WLAN TLS security to TLS Certificate security
06/10/05   cvs     Support for Smart USB Carkit
06/08/05   cvs     TTY devices can be detected as stereo headsets, too.
06/07/05   sun     Added support for CCBS
06/02/05   cvs     Fix swapped TTY HCO and VCO
05/27/05   jas     Add Get/Set methods for WLAN network selection mode
05/24/05   aps     code cleanup. updated indentation and if-braces in
                   accordance with coding standards.
05/19/05   aps     updated GetSilenceAll() and SetSilenceAll()
04/21/05   jas     Adding support for WPA WLAN security
04/19/05   jas     Add conversion between ICM WLAN channel and IWIFI channel
04/13/05   jas     Reworking WLAN security menus
04/07/05   RI      Support for TTY.
03/16/05   jas     Fixing logic error in GetDhcp
03/15/05   cvs     Support for updating time on screen less frequently
                   Remove updating time with MM info, as that is broken.
03/15/05   JAS     Fixing logic error causing IPs to not be saved in SaveLan
03/14/05   cvs     Modifying Get/Set routines to maniuplate 64-bit values
03/07/05   cvs     Add missing error message
03/07/05   JAS     Modifying Get/Set routines to maniuplate 64-bit values
03/07/05   JAS     Updating value for sizing SSID string
02/22/05   JAS     Updating WLAN menus
02/15/05   cvs     Change prototype to match coremenu.c
02/03/05   tjc/jas Support for full-screen mode
01/21/05   cvs     Rearchitect uisnd_set_device()
12/13/04   jas     Correcting date on secondary display
12/09/04   cvs     Add function to avoid current system
12/06/04   BW      Added LAN menu and related submenu
11/23/04   sun     Added support for USB HFK
11/18/04   sun     Use the CFGI item to configure Beep Length
11/12/04   ank,cvs Dualslot changes from 6300
11/12/04   jas     Split time/date on 2ary display into 2 lines for
                   formatting.
11/11/04   sun     Set the LCD timers instead of just a on/off value
10/27/04   cvs     Added more conservative limits for contrast
10/06/04   sun     Fixed Manual PLMN Search screens
10/01/04   sun     Allow termination of PLMN search
09/28/04   jas     Expanded error messages for clarity
09/22/04   cvs     Add Contrast menu, lint fixes
09/02/04   tml     Added CPHS support and added revision history comment
08/09/04   PA      Check for correct return value in SetPrivacy.
08/02/04   PA      Display PLMN List again only for network selection error.
07/27/04   sun     Do not write time to PMIC  - Temporary Fix
07/08/04   sun     Fixed the Key beep tone
07/02/04   sun     Invalidate Timer after setting it once
06/28/04   jas     Added support for dual-display targets
06/09/04   sun     Adjusted Bluetooth Volumes as well
06/07/04   tml     Added personalizaton support
06/07/04 sun/tml   Linking error fix
06/07/04   tml     Fixed linking error
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
06/01/04   sun     Fixed Backlights, null pointer checks
05/25/04   PA      Fix LPM transition in FTM mode.
05/13/04   AT      Added Thin UI changes.
05/13/04   sun     Added LPM Changes and Display Time after Setting it
05/07/04   sun     Fixed TTY, Separated Game Mode from LPM
04/29/04   sun     Fixed Backlight duration time, made plmn info modifications
04/16/04   sun     Do not update Backlight Intensity while change duration
04/14/04   sun     Added new menu item to set VR
03/19/04   sun     Added changes for DTMF
03/117/04  tml     Fixed RTRE configuration failure
03/17/04   wli     Fixed run time RTRE configuration failure.
03/04/04   sun     Fixed the backlight settings
03/01/04   sun     Set Game Mode
02/25/04   ram     Integration with IBatt Interface.
02/24/04   ram     Center the Idle menu text.
01/29/04   tml     Added FEATURE_NEW_ICARD feature
01/28/04   ram     Add speaker-phone functionality.
01/20/04   sun     Set the DisplayTime timer to 1 second
12/22/03   tml     ICARD API changes
01/07/04   ram     Add Multimedia Volume setting
01/07/04   sun     Added null pointer in check in DisplayIdleMenuDlg
12/09/03   ram     Removed lint errors.
12/02/03   ram     Changes for exiting emergency mode.
12/02/03   ram     Fix compile erros.
11/31/03   BPW     Do not handle auto repeat key events in BREW 3.0
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/25/03   sun     Added CFGI items for all external devices
11/20/03   ram     Added HDR_ABORT_PPP support.
11/11/03   ram     Added Game-Mode.
11/07/03   tml     User Pref Slot changes
11/06/03   AT/tml  Added call to ui_set_restrictions.
11/03/03   kar     Fixed End key issue and compilation error
11/03/03   ram     Added PLMN selection menu.
10/23/03   PA      Integrated with latest ICM
10/09/03   tml     Voice privacy fix
10/09/03   sun     Fixed Compile Error
10/06/03   tml     Voice privacy fix
09/30/03   tml     Added ICM function calls for checking mode and band pref
                   validity
09/30/03   tml     Reflected changes in ICM API
09/24/03   kar     Fixed compile errors
09/21/03   tml     ICM support
08/29/03   ram     Removed function CheckAEEReturnStatus (Added macro to
                   AppComFunc.h)
08/26/03   tml     Fixed end key issue
08/25/03   kar     END key now works properly for non MMGSDI targets
08/06/03   tml     Dual slot support
08/05/03   kar     Set time changes
07/16/03   tml     enable/disable pin1/pin2 cleanup
07/15/03   tml     Security clean up
07/14/03   kar     Fixed DisplayIdleMenuDlg
06/18/03   kar     Fixed compile error
06/17/03   kar     Clear SKUI dlg properly while popping to idle display
06/11/03   ck      RTRE default config is set to NV Only when the feature
                   FEATURE_RTRE_DEFAULT_IS_NV is ON.
05/16/03   wli     Added featurization for GSM 1X access in RTRE
05/08/03   kar     System menu changes
05/03/03   tml     Added Get and Set methods for NV Lock code
05/02/03   tml     Added GSM_ACCESS support for RTRE
04/14/03   kar     Code cleanup
04/01/03   kar     Code cleanup
03/18/03   kar     Increased duration for time display
03/13/03   tml     Removed ASSERT
03/07/03   kar     Fixed crash when setting keyguard to "Guard Now"
02/21/03   kar     Changed function prototype
02/14/03   tml     Compilatio fix for GW builds
02/14/03   kar     Fixed compile error on GW builds
02/13/03   NNG     Added check to return from display idle menu when SK UI dialog.
02/04/03   tml     compilation fix for RTRE build
01/31/03   tml     Change pm function call to clk RTC function call
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
01/30/03   kar     Removed PM1000 code
01/28/03   kar     Changes for AppComFunc API
01/07/03   kar     Changes for AppComFunc API
12/31/02   kar     Changes for  variable type change for banner string
                   DisplayTime now uses the member element of pMe to store time
12/04/02   ic      In SetAutoAnswer() added setting ui.isanswer to
                   TRUE/FALSE to reflect the change in SK UI
11/20/02   tml     DisplayIdleMenuDlg fix
11/19/02   kar     Tweaked SetGeneralMenuStyle
10/28/02   tml     RTRE Security Changes
10/08/02   tml     Code review changes
10/02/02   kar     Code cleanup
09/20/02  kar/tml  Added check in DisplayIdleMenuDlg so that it will not try to
                   end the "idle" menu dialog. Cleaned up time functions
09/20/02   aaa     Support for time display at my info screen
09/19/02   tml     Removed Corefeatures.h
09/19/02   kar     Featurization fixes
09/18/02   kar     Support for MM ringer volume
09/17/02   kar     Range check ringer type
09/17/02   kar     Code cleanup
09/16/02   kar     Support for time display
09/09/02    js     Add support for pin verification message display
08/27/02   aaa     Fixed backlight timing issue
08/22/02   kar     Renamed variables to comply with guidelines
08/21/02  js/tml   Support for ICARD Integration
08/13/02   kar     Core utils module
===========================================================================*/
//lint -save -e611  Suppress "suspicious cast" errors
//lint -save -e766  Suppress "CoreApp_qvga_res.h" not used

/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "OEMFeatures.h"
#include "comdef.h"
#include "AEEStdLib.h"
#include "CoreApp.h"
#include "CoreMenu.h"
#include "CoreService.h"
#include "CoreUtils.h"
#include "CoreApp_res.h"
#include "CoreApp_qvga_res.h"
#include "CoreSecurity.h"
#include "AppComFunc.h"
#include "core_ui.h"
#include "ui.h"
#include "ui_callctrl.h"
#include "ui_base.h"
#include "uiutils.h"
#include "AEEFile.h"
#include "AEECM.h"
#if defined(FEATURE_SECONDARY_DISPLAY)
#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#endif

#ifdef FEATURE_PMIC_RTC
#include "clk.h"
#include "clkrtc.h"
#endif
#include "disp.h"

#ifdef FEATURE_USRSYSCLOCK
#include "AEEUsrSysClock.h"
#endif

#define NV_VALUE_ERROR_BYTE -1
#define NV_VALUE_ERROR_AECHAR NULL

#define MAX_STR_SIZE 50

#define COREUTIL_VOICE_AS_DATA_TIME 600
//lint -save -e740
static const char DigitToAscTable[10] =
    { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

static AEECallback CoreUtil_StatusCb;
static AEECMPhError status;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
static AEECardGenericCmdStatus sUserPrefSlotActionStatus;
#endif

#define CONTRAST_UNDEFINED 0
#if defined(T_MSM6550)
  //For MDDI displays, the driver defaults are correct.
  //TBD need to check this on an EBI-2 display, once it's working.
  #define CONTRAST_MIN  CONTRAST_UNDEFINED
  #define CONTRAST_MAX  CONTRAST_UNDEFINED
#elif defined(T_MSM6500)
  #define CONTRAST_MIN  65
  #define CONTRAST_MAX  95
#elif defined(T_MSM6250)
  #define CONTRAST_MIN  70
  #define CONTRAST_MAX  110
#else
  #define CONTRAST_MIN  CONTRAST_UNDEFINED
  #define CONTRAST_MAX  CONTRAST_UNDEFINED
#endif

#ifdef FEATURE_MMGSDI_DUAL_SLOT
int  DetermineDualSlotState(CCoreApp *pMe, uint16 *wDlgID, boolean bAllowBypass);
#endif

/*===========================================================================
FUNCTION CoreUtil_SystemPrefErrCb

DESCRIPTION
  Callback function passed to ICM for System Preference Setting

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_SystemPrefErrCb(void *pNotifyData)
{
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }
  ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_SYS_PREF_ERR, status);

  return;
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
uint16 LookupResIDForSysPrefErrCode (uint32 dwErrCode)
{
  switch (dwErrCode)
  {
  case AEECM_PH_ERROR_MODE_PREF_S:
    /* Invalid mode preference selection relatively to what we
    ** currently have in the PRL - e.g. ANALOG_ONLY when there are no
    ** analog entries in the PRL */
    return IDS_MODE_PREF_ERROR;

  case AEECM_PH_ERROR_SYS_PREF_S:
    /* Invalid system preference selection for this target
    ** - e.g. A_ONLY for a none 800 MHz target, etc. - obsolete in SD20*/
    return IDS_NETWORK_SEL_ERROR;

  case AEECM_PH_ERROR_SYS_SEL_PREF_S:
    /* Invalid system selection preference for this target/PRL
    ** - e.g. band-class 1 for a PCS only target */
    return IDS_CHANGE_FAILED;

  case AEECM_PH_ERROR_HYBR_PREF_S:
    /* FEATURE_HDR - Invalid command or hybrid preference selection */
    return IDS_HYBRID_PREF_ERROR;

  case AEECM_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S:
    /* Current NAM is not RUIM NAM. CDMA Only */
    return IDS_NAM_ERROR;

  case AEECM_PH_ERROR_NONE:
    return 0; /* change successfully */

  case AEECM_PH_ERROR_LINE_SWITCHING:
    return IDS_ALS_LINE_SWITCHING_ERROR;

  case AEECM_PH_ERROR_SELECTED_LINE:
    return IDS_ALS_LINE_SELECTION_ERROR;

  case AEECM_PH_ERROR_ALS_NOT_SUPPORTED:
  case AEECM_PH_ERROR_ALS_SIM_WRITE_FAILED:
    return IDS_ALS_OPERATION_ERROR;

  case AEECM_PH_ERROR_OTHER:
  default:
    return IDS_CHANGE_FAILED;

  }
}

/*===============================================================================

                    FUNCTION DEFINITIONS

=============================================================================== */

/*===========================================================================
FUNCTION RangeCheckVolume

DESCRIPTION
  Check to see if the volume is within range

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean RangeCheckVolume (byte vol)
{
  if ((vol == UISND_VOL_OFF) || (vol == UISND_1ST_VOL) ||
      (vol == UISND_2ND_VOL) || (vol == UISND_3RD_VOL) ||
      (vol == UISND_4TH_VOL))
      return TRUE;

  return FALSE;
} /* End RangeCheckVolume */



/*===========================================================================
FUNCTION RangeCheckBacklightDuration

DESCRIPTION
  check backlight value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean RangeCheckBacklightDuration(byte byVal)
{
  switch (byVal)
  {
  case UI_BKLT_TIME_MAX:
  case 5:
  case 10:
  case 30:
  case UI_BKLT_TIME_NONE:
    return TRUE;
  default:
    return FALSE;
  }
} /* End RangeCheckBacklightDuration */



/*===========================================================================
FUNCTION RangeCheckAutoRetry

DESCRIPTION
  check auto retry value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean RangeCheckAutoRetry(byte byVal)
{
  switch (byVal)
  {
  case 0:
  case 1:
  case 3:
  case 6:
    return TRUE;
  default:
    return FALSE;
  }
} /* End RangeCheckAutoRetry */


/*===========================================================================
FUNCTION RangeCheckLCDValue

DESCRIPTION
  check auto retry value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean RangeCheckLCDValue(uint32 val)
{
  switch (val)
  {
    case 0:
    case COREAPP_LCD_OFF_TIME_5_SEC:
    case COREAPP_LCD_OFF_TIME_10_SEC:
    case COREAPP_LCD_OFF_TIME_30_SEC:
    case COREAPP_LCD_OFF_TIME_2_MIN:
    case COREAPP_LCD_OFF_TIME_10_MIN:
      return TRUE;
    default:
      CORE_ERR("Invalid LCD Value encountered %d", val, 0, 0);
      return FALSE;
  }
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION CoreUtil_CancelGetNW

DESCRIPTION
  Cancel the call to ICM_GetNetworks and free allocated memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_CancelGetNW(CCoreApp *pMe)
{
  //Reached here because user pressed End key while "Searching...."
  PRINT_FUNCTION_NAME();

  if(pMe->m_nNWSelectionMode != APPCOMFUNC_NW_SEL_INACTIVE) {
    pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    /* Note: The cancel function is set to NULL when the
             Get Networks command is sent to ICM or a
             Terminate Get Networks is done.
      This must not be executed unless a cancel function is set
    */
    if( pMe->m_nCallback.pfnCancel )
    {
      pMe->m_nCallback.pfnCancel(&pMe->m_nCallback);
    }
    FREEIF(pMe->m_pDetailedPLMNList);
    FREEIF(pMe->m_pCopyDetailedPLMNList);
  }

}
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

/*===========================================================================
FUNCTION GetRestrict

DESCRIPTION
  Get Restrict value from NV, update the wRestrict static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRestrict(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  AEESimCapabilities sSimCap;
  AEESimCapabilities sSimCap2;
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  /* initialize variables */
  pMe->m_wRestrict = COREUTILS_NO_RESTRICTION;
  sSimCap.fdn_enabled = 0;
  sSimCap2.fdn_enabled = 0;

  /* check SIM FDN */
  nReturnStatus = ICARD_GetSimCapabilitySlot(pMe->m_pICard, AEECARD_SLOT1, &sSimCap);
  if (nReturnStatus == SUCCESS) {
    if (sSimCap.fdn_enabled) {
      pMe->m_wRestrict |= COREUTILS_FDN_SLOT1;
    } else {
      pMe->m_wRestrict &= ~COREUTILS_FDN_SLOT1;
    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* check SIM FDN */
  nReturnStatus = ICARD_GetSimCapabilitySlot(pMe->m_pICard, AEECARD_SLOT2, &sSimCap2);
  if (nReturnStatus == SUCCESS) {
    if (sSimCap2.fdn_enabled) {
      pMe->m_wRestrict |= COREUTILS_FDN_SLOT2;
    } else {
      pMe->m_wRestrict &= ~COREUTILS_FDN_SLOT2;
    }
  }
#endif

  result = ui_get_nv(NV_CALL_RSTRC_I, &nvi);

  if(result == NV_NOTACTIVE_S) {  /* write to NV */
    nvi.call_rstrc = pMe->m_wRestrict;
    (void) ui_put_nv(NV_CALL_RSTRC_I, &nvi);
    ui_set_restrictions(pMe->m_wRestrict);
    return (uint64)pMe->m_wRestrict;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_wRestrict |=  (byte)nvi.call_rstrc;
    if (!sSimCap.fdn_enabled) {
      /* remove the FDN slot1 enabled bitmask */
      pMe->m_wRestrict &= COREUTILS_NO_FDN_SLOT1;
    }
    if (!sSimCap2.fdn_enabled) {
      /* remove the FDN slot2 enabled bitmask */
      pMe->m_wRestrict &= COREUTILS_NO_FDN_SLOT2;
    }

    nvi.call_rstrc = pMe->m_wRestrict;
    (void) ui_put_nv(NV_CALL_RSTRC_I, &nvi);
    ui_set_restrictions(pMe->m_wRestrict);
    return (uint64)pMe->m_wRestrict;
  }
  return (uint64) (NV_VALUE_ERROR_BYTE);
} /* End GetRestrict */



/*===========================================================================
FUNCTION SetNVRestrict

DESCRIPTION
  write the pMe restrict member to NV

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNVRestrict(CCoreApp *pMe)
{
  nv_item_type nvi;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nvi.call_rstrc = pMe->m_wRestrict;
  (void) ui_put_nv(NV_CALL_RSTRC_I, &nvi);
  ui_set_restrictions(pMe->m_wRestrict);
  ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_RESTRICT_UPDATE_SUCCESS, 0);

  return TRUE;
} /* SetNVRestrict */

/*===========================================================================
FUNCTION SetRestrict

DESCRIPTION
  Set static variable wRestrict, and write to NV

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRestrict(CCoreApp * pMe, uint64 nNewVal)
{
  uint16 wNewVal = (uint16) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  switch (wNewVal)
  {
    case COREUTILS_EFS:
    case COREUTILS_SPEED_DIAL:
#ifdef FEATURE_MMGSDI
    case COREUTILS_ADN_SLOT1:
#endif
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case COREUTILS_ADN_SLOT2:
#endif
      pMe->m_wRestrict |= (byte) wNewVal;
      (void) SetNVRestrict(pMe);
      break;
    case COREUTILS_NO_EFS:
    case COREUTILS_NO_SPEED_DIAL:
#ifdef FEATURE_MMGSDI
    case COREUTILS_NO_ADN_SLOT1:
#endif
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case COREUTILS_NO_ADN_SLOT2:
#endif
    case COREUTILS_NO_RESTRICTION:
      pMe->m_wRestrict &= wNewVal;
      (void) SetNVRestrict(pMe);
      break;
#ifdef FEATURE_MMGSDI
    case COREUTILS_FDN_SLOT1:
    case COREUTILS_NO_FDN_SLOT1:
      if ((pMe->m_sPIN2_lock_state == AEECARD_PIN_DISABLED) ||
          (pMe->m_sPIN2_lock_state == AEECARD_PIN_NOT_VALID)) {
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN2);
      }
      else {
        ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_PIN2_REQUEST, NULL);
      }
      break;
#endif
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case COREUTILS_FDN_SLOT2:
    case COREUTILS_NO_FDN_SLOT2:
      if ((pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_DISABLED) ||
          (pMe->m_sPIN2_slot2_lock_state == AEECARD_PIN_NOT_VALID)) {
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
            EVT_USER, EVT_PIN_VERIFY_SUCCESS, AEECARD_PIN2_SLOT2);
      }
      else {
        ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_PIN2_REQUEST_SLOT2, NULL);
      }
      break;
#endif

    default:
      return FALSE;
  }

  return TRUE;
} /* Set Restrict */

/*===========================================================================
FUNCTION GetMute

DESCRIPTION
  Get the bMute static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetMute(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }
  return (uint64)(pMe->m_bMute);
}

/*===========================================================================
FUNCTION SetMute

DESCRIPTION
  Set the bMute static variable, call the uisnd_set_mute which will do the
  actual muting or unmuting

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean SetMute(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Function valid only in call */
  if (ui_is_in_call()) {
    pMe->m_bMute = bNewVal;



    if(pMe->m_bMute) { /* mute the speaker! */
      uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_MUTED, NULL, NULL);
    }
    else { /* unmute the speaker! */
      uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
    }
    return TRUE;
  }
  return FALSE;
} /* End SetMute */


#ifdef FEATURE_TTY
/*===========================================================================
FUNCTION GetTTY

DESCRIPTION
  Obtain TTY information from NV, set the byTTY variable

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetTTY(CCoreApp * pMe)
{
  OEMTTY tty;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_TTY ,
      &tty, sizeof(OEMTTY))) != TRUE) {
      (void) SetTTY(pMe, (uint64)OEMNV_TTY_OFF);
    return (uint64)pMe->m_byTTY;
  }

  pMe->m_byTTY = tty;

  return (uint64)pMe->m_byTTY;
} /* End GetTTY */




/*===========================================================================
FUNCTION SetTTY

DESCRIPTION
  Set the byTTY static variable

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
boolean SetTTY(CCoreApp * pMe, uint64 nNewVal)
{
  OEMTTY byNewVal = (OEMTTY) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  pMe->m_byTTY = byNewVal;
  CoreTask_SetTTY(pMe->m_byTTY);

#ifdef FEATURE_TTY
  /* set the volume levels for TTY */

  if(pMe->m_byTTY == OEMNV_TTY_FULL){
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_HEADSET );
  } else if (pMe->m_byTTY == OEMNV_TTY_HEAR){
    uisnd_tty_vol( GetEarpieceVolume(pMe), SND_DEVICE_TTY_HCO );
  }else if (pMe->m_byTTY == OEMNV_TTY_TALK){
    uisnd_tty_vol( UISND_MAX_VOL, SND_DEVICE_TTY_VCO );
  }
#endif /* FEATURE_TTY */

  /* make sure sound device is set appropriately */
  uisnd_set_device_auto(NULL, NULL);
  InitializeVolumes(pMe);

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_TTY ,
      &byNewVal, sizeof(OEMTTY))) != TRUE) {
     return FALSE;
  }
  return TRUE;
} /* End SetTTY */
#endif /*FEATURE_TTY*/

/*===========================================================================
FUNCTION GetSoundDevice

DESCRIPTION
  Get Sound Device from uixsnd.  Errors return SND_DEVICE_CURRENT

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetSoundDevice(CCoreApp * pMe)
{
  uisnd_notify_data_s_type snd_info;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)SND_DEVICE_CURRENT;
  }

  uisnd_get_device(&snd_info);

  return (uint64)snd_info.out_device;
} /* End GetSoundDevice */

/*===========================================================================
FUNCTION SetSoundDevice

DESCRIPTION
  Set Sound Device in uixsnd.  SND_DEVICE_MAX  indicates audoselect.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSoundDevice(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bRetVal;
  snd_device_type snd_device = (snd_device_type)nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (nNewVal == SND_DEVICE_MAX)
  {
    uisnd_force_device_autoselect();
    uisnd_set_device_auto(NULL, NULL);
    InitializeVolumes(pMe);
    return TRUE;
  }
  else
  {
    bRetVal = uisnd_set_device_specific(snd_device, NULL, NULL);
    InitializeVolumes(pMe);
    return bRetVal;
  }
} /* End SetSoundDevice */

/*===========================================================================
FUNCTION GetAutoHyphen

DESCRIPTION
  Obtain Auto Hyphen information from NV, set the bAutoHyphen variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoHyphen(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }
  result = ui_get_nv(NV_AUTO_HYPHEN_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) SetAutoHyphen(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bAutoHyphen;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_bAutoHyphen = nvi.auto_hyphen;
    return (uint64)pMe->m_bAutoHyphen;
  }

  return (uint64)FALSE;
} /* End GetAutoHyphen */



/*===========================================================================
FUNCTION SetAutoHyphen

DESCRIPTION
  Write the new Auto Hyphen information to NV, update the bAutoHyphen variable

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoHyphen(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer setting autohyphen.", 0,0,0);
    return FALSE;
  }
  pMe->m_bAutoHyphen = bNewVal;
  nvi.auto_hyphen = pMe->m_bAutoHyphen;
  (void) ui_put_nv(NV_AUTO_HYPHEN_I, &nvi);
  return TRUE;
} /* End SetAutoHyphen */



/*===========================================================================
FUNCTION UpdateBacklight

DESCRIPTION
  update backlight screen accordingly

DEPENDENCIES
  ui_backlight

SIDE EFFECTS
  None
===========================================================================*/
void UpdateBacklight(CCoreApp *pMe)
{
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer updating backlight.", 0,0,0);
    return;
  }
  //Always turn on the LCD and then set the backlight
  HandleBacklights( TRUE, pMe->m_byBacklight );

  CoreTask_SetBackDuration(pMe->m_byBacklight);
} /* End UpdateBacklight */



/*===========================================================================
FUNCTION GetBackLightDuration

DESCRIPTION
  Get the backlight duration from NV, assign new value to byBacklight, update
  backlight screen accordingly

DEPENDENCIES
  UpdateBacklight,ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetBackLightDuration(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer getting backlight duration.", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  result = ui_get_nv(NV_BACK_LIGHT_I, &nvi);

  if(result == NV_NOTACTIVE_S) {  /* write to NV */
    (void) SetBackLightDuration(pMe, (uint64)UI_BKLT_TIME_MAX);
    return (uint64)pMe->m_byBacklight;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_byBacklight = nvi.back_light;
    if (!RangeCheckBacklightDuration(pMe->m_byBacklight)) {
      pMe->m_byBacklight = UI_BKLT_TIME_MAX;
      nvi.back_light = pMe->m_byBacklight;
      (void) ui_put_nv(NV_BACK_LIGHT_I, &nvi);
    }
    UpdateBacklight(pMe);
    return (uint64)pMe->m_byBacklight;
  }

  return (uint64) (NV_VALUE_ERROR_BYTE);
} /* End GetBacklightDuration */



/*===========================================================================
FUNCTION SetBackLightDuration

DESCRIPTION
  Update byBacklight variable, and write to NV.

DEPENDENCIES
  (void) ui_put_nv, UpdateBacklight

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBackLightDuration(CCoreApp * pMe, uint64 nNewVal)
{
  // when setting the ui.backlight1
  nv_item_type nvi;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer setting backlight duration.", 0,0,0);
    return FALSE;
  }

  pMe->m_byBacklight = byNewVal;
  if (!RangeCheckBacklightDuration(pMe->m_byBacklight)) {
    pMe->m_byBacklight = UI_BKLT_TIME_MAX;
  }

  UpdateBacklight(pMe);
  nvi.back_light = pMe->m_byBacklight;
  (void) ui_put_nv(NV_BACK_LIGHT_I, &nvi);
  return TRUE;
} /* End SetBacklightDuration */



#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
/*===========================================================================
FUNCTION RangeCheckBacklightIntensity

DESCRIPTION
  Ensures the intensity is one of the allowed values.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean RangeCheckBacklightIntensity(uint64 intensity)
{
  switch (intensity)
  {
    case LCD_BACKLIGHT_LVL_0:
    case LCD_BACKLIGHT_LVL_1:
    case LCD_BACKLIGHT_LVL_2:
    case LCD_BACKLIGHT_LVL_3:
    case LCD_BACKLIGHT_LVL_4:
      return TRUE;
    default:
      CORE_ERR("Invalid backlight setting %d", (int) intensity, 0, 0);
      return FALSE;
  }
}

/*===========================================================================
FUNCTION GetBackLightIntensity

DESCRIPTION
  Get byBklightIntensity

DEPENDENCIES
  uiscrn_set_lcd_backlight_level

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetBackLightIntensity(CCoreApp * pMe)
{
  uint8 back_light_intensity;

  if (pMe == NULL || pMe->m_pIConfig == NULL ) { /* error checking */
    CORE_ERR ("Null pointer %x getting backlight duration.", pMe,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  if(!CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig,
        CFGI_BACK_LIGHT_INTENSITY, &back_light_intensity, sizeof(uint8))))
  {

    CORE_ERR ("Could not get Item", 0,0,0);
    (void) SetBackLightIntensity(pMe, (uint64)LCD_BACKLIGHT_LVL_DEFAULT);
    return (uint64)pMe->m_byBklightIntensity;
  }

  /* Make 0 mean an unset value. COREAPP_LVL_0_NV_VAL is used
     when we actually want a 0. */
  if (back_light_intensity == 0)
    back_light_intensity = LCD_BACKLIGHT_LVL_DEFAULT;
  else if (back_light_intensity == COREAPP_LVL_0_NV_VAL)
     back_light_intensity = LCD_BACKLIGHT_LVL_0;

  pMe->m_byBklightIntensity = back_light_intensity;

  if (!RangeCheckBacklightIntensity(pMe->m_byBklightIntensity)) {
    CORE_ERR ("Received value %d out of bounds", back_light_intensity,0,0);
    /* pMe->m_byBklightIntensity is set by SetBackLightIntensity */
    (void) SetBackLightIntensity(pMe, (uint64)LCD_BACKLIGHT_LVL_DEFAULT);
  }

  return (uint64)pMe->m_byBklightIntensity;
} /* End GetBackLightIntensity */


/*===========================================================================
FUNCTION SetBackLightIntensity

DESCRIPTION
  Set new value to byBklightIntensity, update lcd backlight intensity level

DEPENDENCIES
  uiscrn_set_lcd_backlight_level

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBackLightIntensity(CCoreApp * pMe, uint64 nNewVal)
{
  uint8 NewVal = (uint32) nNewVal;

  if (pMe == NULL || pMe->m_pIConfig == NULL) { /* error checking */
    CORE_ERR ("Null pointer setting backlight intensity %x.", pMe,0,0);
    return FALSE;
  }

  if (!RangeCheckBacklightIntensity(nNewVal))
  {
    NewVal = LCD_BACKLIGHT_LVL_DEFAULT;
  }

  pMe->m_byBklightIntensity = NewVal;

  if (NewVal == LCD_BACKLIGHT_LVL_0)
     NewVal = COREAPP_LVL_0_NV_VAL;
  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_BACK_LIGHT_INTENSITY,
                                          &NewVal, sizeof(uint8))) != TRUE)
  {
    CORE_ERR("Could not Set the Item",0,0,0);
    return FALSE;
  }

  CoreTask_SetBackVal(pMe->m_byBklightIntensity);

  return TRUE;
} /* End Setbacklightintensity */

#endif /* defined (FEATURE_PMIC_LCDKBD_LED_DRIVER) */



/*===========================================================================
FUNCTION GetServiceAlert

DESCRIPTION
  Perform an NV get for service alert, update the static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetServiceAlert(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer getting service alert.", 0,0,0);
    return (uint64)(FALSE);
  }
  result = ui_get_nv(NV_SVC_AREA_ALERT_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) SetServiceAlert(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bServiceAlert;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_bServiceAlert = nvi.svc_area_alert;
    return (uint64)pMe->m_bServiceAlert;
  }

  return (uint64)FALSE;
} /* End GetServiceAlert */



/*===========================================================================
FUNCTION SetServiceAlert

DESCRIPTION
  A NV write to service alert parameter and update the bServiceAlert variable

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetServiceAlert(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer setting service alert.", 0,0,0);
    return FALSE;
  }
  pMe->m_bServiceAlert = bNewVal;
  nvi.svc_area_alert = pMe->m_bServiceAlert;
  (void) ui_put_nv(NV_SVC_AREA_ALERT_I, &nvi);
  return TRUE;
} /* End SetServiceAlert */

/*===========================================================================
FUNCTION InitializeVolumes

DESCRIPTION
  initialize the various device volumes.

DEPENDENCIES
  the corresponding Get/Set methods for Sound related features

SIDE EFFECTS
  None
===========================================================================*/
void InitializeVolumes (CCoreApp *pMe)
{
  /* Volume Initialization */
  
  //ui_silence_key decides if PlayKeySound will run or not
  if (GetKeyBeepVolume(pMe) == UISND_VOL_OFF)
    SetKeySilenceStatus(TRUE);
  else
    SetKeySilenceStatus(FALSE);
 
  (void)GetEarpieceVolume(pMe);
  (void)GetRingerVolume(pMe);  // check for UISND_ESC_VOL in the get function?
#ifdef FEATURE_MULTIMEDIA
  (void)GetMMVolume(pMe);
#endif
  (void)GetMessageVolume(pMe);
}

#ifdef FEATURE_MULTIMEDIA
/*===========================================================================
FUNCTION GetMMVolumeNVEnum

DESCRIPTION
  Based on the current bAnalogHfk and bOnhook values, determine
  the corresponding NV items for MM volume

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetMMVolumeNVEnum()
{
  uisnd_notify_data_s_type info;

  uisnd_get_device(&info);
  return uisnd_map_device_to_nv_item(info.out_device, UISND_MM_SND);
} /* End GetMMVolumeNVEnum */


/*===========================================================================
FUNCTION GetMMVolume

DESCRIPTION
  Get the MM volume from NV store, update the static variable and update
  the sound device's ringer volume setting

DEPENDENCIES
  GetMMVolumeNVEnum, ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint32 GetMMVolume(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  nv_items_enum_type item;
  byte MMRingerVol = UISND_2ND_VOL;
  boolean bRange;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return ((byte) NV_VALUE_ERROR_BYTE);
  }

  item = GetMMVolumeNVEnum();

  if( (item != NV_MM_LVL_I) &&
    (item!= NV_MM_SPEAKER_LVL_I) &&
    (item!= NV_MM_LVL_SHADOW_I)) {
    CORE_ERR ("Unhandled MM Ringer NV item %d", item, 0, 0);
    return (uint32) (NV_VALUE_ERROR_BYTE);
  }

  result = ui_get_nv(item, &nvi);

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    if(item == NV_MM_LVL_I) {
      MMRingerVol = nvi.mm_lvl;
    }
    else if(item == NV_MM_SPEAKER_LVL_I) {
      MMRingerVol = nvi.mm_speaker_lvl;
    }
    else if(item == NV_MM_LVL_SHADOW_I) {
      MMRingerVol = nvi.mm_lvl_shadow;
    }
  }

  bRange = RangeCheckVolume(MMRingerVol);
  if ( bRange == FALSE) {
    /* volume out of range */
    CORE_ERR ("Ringer Volume out of range - GetMMVolume", 0, 0, 0);
  }
  if ( (bRange == FALSE) || (result == NV_NOTACTIVE_S)) {
    MMRingerVol = UISND_2ND_VOL;
    if(item == NV_MM_LVL_I) {
      nvi.mm_lvl = MMRingerVol;
    }
    else if(item == NV_MM_SPEAKER_LVL_I) {
      nvi.mm_speaker_lvl = MMRingerVol;
    }
    else if(item == NV_MM_LVL_SHADOW_I) {
      nvi.mm_lvl_shadow = MMRingerVol;
    }
    (void) ui_put_nv (item, &nvi);
  }

  uisnd_mm_vol( MMRingerVol, item);
#ifdef FEATURE_BT
  uisnd_mm_vol(MMRingerVol, NV_MM_LVL_SHADOW_I);
#endif
  return (uint32) MMRingerVol;
} /* End GetMMVolume */

/*===========================================================================
FUNCTION SetMMRingerVolume

DESCRIPTION
  Set the static variable to the new value, update NV item and sound driver
  MM ringer volume setting

DEPENDENCIES
  GetRingerVolumeNVEnum, (void) ui_put_nv, uisnd_ring_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetMMRingerVolume(CCoreApp * pMe, uint32 nNewVal)
{
  nv_item_type nvi;
  nv_items_enum_type item;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) {
    /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (RangeCheckVolume(byNewVal) == FALSE) {
    /* Volume out of Range */
    CORE_ERR ("Ringer Volume out of range - SetMMRingerVolume", 0, 0, 0);
    byNewVal = UISND_2ND_VOL;
  }
  item = GetMMVolumeNVEnum();

  switch (item)
  {
    case NV_MM_LVL_I:
      nvi.mm_lvl = byNewVal;
      break;
    case NV_MM_SPEAKER_LVL_I:
      nvi.mm_speaker_lvl = byNewVal;
      break;
    case NV_MM_LVL_SHADOW_I:
      nvi.mm_lvl_shadow = byNewVal;
      break;
    default:
      CORE_ERR ("Unhandled MM Ringer vol item %d", item, 0, 0);
      return FALSE;
  }

  (void) ui_put_nv(item, &nvi);

  uisnd_mm_vol( byNewVal, item);
#ifdef FEATURE_BT
  uisnd_mm_vol(byNewVal, NV_MM_LVL_SHADOW_I);
#endif

  return TRUE;
} /* End SetMMRingerVolume */

#endif //FEATURE_MULTIMEDIA

/*===========================================================================
FUNCTION GetRingerVolumeNVEnum

DESCRIPTION
  Based on the current bAnalogHfk and bOnhook values, determine
  the corresponding NV items for ringer volume

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetRingerVolumeNVEnum()
{
  uisnd_notify_data_s_type info;

  uisnd_get_device(&info);
  return uisnd_map_device_to_nv_item(info.out_device, UISND_RINGER_SND);
} /* End GetRingerVolumeNVEnum */


/*===========================================================================
FUNCTION GetRingerVolume

DESCRIPTION
  Get the ringer volume from NV store, update the static variable and update
  the sound device's ringer volume setting

DEPENDENCIES
  GetRingerVolumeNVEnum, ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRingerVolume(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  nv_items_enum_type item;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return ((uint64) NV_VALUE_ERROR_BYTE);
  }

#ifdef FEATURE_MULTIMEDIA
  if((pMe->m_bMMRinger)|| (pMe->m_bMMVolumeSetting)) {
    return (uint64)GetMMVolume(pMe);
  }
#endif
  item = GetRingerVolumeNVEnum();

  result = ui_get_nv(item, &nvi);

  if (result == NV_NOTACTIVE_S) {
    pMe->m_byRingerVolume = UISND_2ND_VOL;
    nvi.ringer_level = UISND_2ND_VOL;
    (void) ui_put_nv (item, &nvi);
    uisnd_ring_vol( pMe->m_byRingerVolume, item);
    return (uint64)pMe->m_byRingerVolume;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */

    switch (item)
    {
      case NV_RINGER_LVL_I:
        pMe->m_byRingerVolume = nvi.ringer_level;
        break;
      case NV_RINGER_SPKR_LVL_I:
        pMe->m_byRingerVolume = nvi.ringer_spkr_lvl;
        break;
      default:
        CORE_ERR ("Unhandled Ringer NV item %d", item, 0, 0);
        pMe->m_byRingerVolume = UISND_VOL_OFF;
        break;
    }

    if (RangeCheckVolume(pMe->m_byRingerVolume) == FALSE) {
      /* volume out of range */
      CORE_ERR ("Ringer Volume out of range - GetRingerVolume", 0, 0, 0);
      pMe->m_byRingerVolume = UISND_2ND_VOL;
      nvi.ringer_level = UISND_2ND_VOL;
      (void) ui_put_nv (item, &nvi);
    }
    uisnd_ring_vol( pMe->m_byRingerVolume, item);
#ifdef FEATURE_BT
    uisnd_ring_vol(pMe->m_byRingerVolume, NV_RINGER_LVL_I);
#endif
    return (uint64)pMe->m_byRingerVolume;
  }

  return (uint64) (NV_VALUE_ERROR_BYTE);
} /* End GetRingerVolume */



/*===========================================================================
FUNCTION SetRingerVolume

DESCRIPTION
  Set the static variable to the new value, update NV item and sound driver
  ringer volume setting

DEPENDENCIES
  GetRingerVolumeNVEnum, (void) ui_put_nv, uisnd_ring_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRingerVolume(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  nv_items_enum_type item;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_MULTIMEDIA
  if((pMe->m_bMMRinger)|| (pMe->m_bMMVolumeSetting)) {
    return SetMMRingerVolume(pMe, (uint32)nNewVal);
  }
#endif
  if (RangeCheckVolume(byNewVal) == FALSE) {
    /* Volume out of Range */
    CORE_ERR ("Ringer Volume out of range - SetRingerVolume", 0, 0, 0);
    byNewVal = UISND_2ND_VOL;
  }
  item = GetRingerVolumeNVEnum();

  switch (item)
  {
    case NV_RINGER_LVL_I:
      nvi.ringer_level = byNewVal;
      break;
    case NV_RINGER_SPKR_LVL_I:
      nvi.ringer_spkr_lvl = byNewVal;
      break;
    default:
      CORE_ERR ("Unhandled Ringer vol item %d", item, 0, 0);
      return FALSE;
  }

  (void) ui_put_nv(item, &nvi);

  uisnd_ring_vol( byNewVal, item);
#ifdef FEATURE_BT
  uisnd_ring_vol(byNewVal, NV_RINGER_LVL_I);
#endif
  pMe->m_byRingerVolume = byNewVal;
  return TRUE;
} /* End SetRingerVolume */



/*===========================================================================
FUNCTION GetKeyBeepVolumeNVEnum

DESCRIPTION
  Based on bAnalogHfk, bSdac, bHeadset and bOnhook, determine the corresponding
  NV item for beep volume

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetKeyBeepVolumeNVEnum()
{
  uisnd_notify_data_s_type info;

  uisnd_get_device(&info);
  return uisnd_map_device_to_nv_item(info.out_device, UISND_KEY_BEEP_SND);
} /*End GETKEYBEEPVOLUMENVENUM*/



/*===========================================================================
FUNCTION GetKeyBeepVolume

DESCRIPTION
  Get the beep volume from NV and update the sound driver

DEPENDENCIES
  GetKeyBeepVolumeNVEnum, ui_get_nv, uisnd_beep_vol

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetKeyBeepVolume(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  nv_items_enum_type item;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }
  item = GetKeyBeepVolumeNVEnum();

  result = ui_get_nv(item, &nvi);

  if(result == NV_NOTACTIVE_S) {
    pMe->m_byKeyBeepVolume = UISND_2ND_VOL;
    nvi.beep_level = UISND_2ND_VOL;
    (void) ui_put_nv (item, &nvi);
    uisnd_beep_vol( pMe->m_byKeyBeepVolume, item);
    return (uint64)pMe->m_byKeyBeepVolume;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */

    if(item == NV_BEEP_LVL_I) {
      pMe->m_byKeyBeepVolume = nvi.beep_level;
    }
    else if (item == NV_BEEP_SPKR_LVL_I) {
      pMe->m_byKeyBeepVolume = nvi.beep_spkr_lvl;
    }
    else if (item == NV_BEEP_LVL_SHADOW_I) {
      pMe->m_byKeyBeepVolume = nvi.beep_lvl_shadow;
    }
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    else if (item == NV_BEEP_SDAC_LVL_I) {
      pMe->m_byKeyBeepVolume = nvi.beep_sdac_lvl;
    }
#endif

    if (RangeCheckVolume(pMe->m_byKeyBeepVolume) == FALSE) {
      /* volume out of range */
      CORE_ERR ("Keybeep Volume out of range - GetKeyBeepVolume", 0, 0, 0);
      pMe->m_byKeyBeepVolume = UISND_2ND_VOL;
      nvi.beep_level = UISND_2ND_VOL;
      (void) ui_put_nv (item, &nvi);
    }

    uisnd_beep_vol( pMe->m_byKeyBeepVolume, item);
#ifdef FEATURE_BT
    uisnd_beep_vol(pMe->m_byKeyBeepVolume, NV_BEEP_LVL_SHADOW_I);
#endif
    return (uint64)pMe->m_byKeyBeepVolume;
  }

  return (uint64) (NV_VALUE_ERROR_BYTE);
} /*End GetKeyBeepVolume*/



/*===========================================================================
FUNCTION SetKeyBeepVolume

DESCRIPTION
  Set the key beep information in NV, update the sound driver

DEPENDENCIES
  GetKeyBeepVolumeNVEnum, (void) ui_put_nv, uisnd_beep_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetKeyBeepVolume(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  nv_items_enum_type item;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  item = GetKeyBeepVolumeNVEnum();

  if (RangeCheckVolume(byNewVal) == FALSE) {
    /* Volume out of Range */
    CORE_ERR ("Keybeep Volume out of range - SetKeyBeepVolume", 0, 0, 0);
    byNewVal = UISND_2ND_VOL;
  }

  switch (item)
  {
    case NV_BEEP_LVL_I:
      nvi.beep_level = byNewVal;
      break;
    case NV_BEEP_SPKR_LVL_I:
      nvi.beep_spkr_lvl = byNewVal;
      break;
    case NV_BEEP_LVL_SHADOW_I:
      nvi.beep_lvl_shadow = byNewVal;
      break;
  #if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    case NV_BEEP_SDAC_LVL_I:
      nvi.beep_sdac_lvl = byNewVal;
      break;
  #endif
    default:
      CORE_ERR("Invalid Item",0, 0, 0);
      return FALSE;
  }

  (void) ui_put_nv(item, &nvi);
  uisnd_beep_vol( byNewVal, item);
#ifdef FEATURE_BT
  uisnd_beep_vol(byNewVal, NV_BEEP_LVL_SHADOW_I);
#endif
  pMe->m_byKeyBeepVolume = byNewVal;
  
  //ui_silence_key decides if PlayKeySound will run or not
  if (GetKeyBeepVolume(pMe) == UISND_VOL_OFF)
    SetKeySilenceStatus(TRUE);
  else
    SetKeySilenceStatus(FALSE);

  return TRUE;
} /*End SetKeyBeepVolume */



/*===========================================================================
FUNCTION GetEarpieceVolumeNVEnum

DESCRIPTION
  Return the NV item type for earpiece based on several device variables

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetEarpieceVolumeNVEnum()
{
  uisnd_notify_data_s_type info;

  uisnd_get_device(&info);
  return uisnd_map_device_to_nv_item(info.out_device, UISND_EAR_SND);
} /*End GetEarpieceVolumeNVEnum*/



/*===========================================================================
FUNCTION GetEarpieceVolume

DESCRIPTION
  Get earpiece volume information from NV, update static variable and sound
  driver

DEPENDENCIES
  GetEarpieceVolumeNVEnum, ui_get_nv, uisnd_voice_vol

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetEarpieceVolume(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  nv_items_enum_type item;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  item = GetEarpieceVolumeNVEnum();

  result = ui_get_nv(item, &nvi);

  if (result == NV_NOTACTIVE_S) {
    pMe->m_byEarpieceVolume = UISND_2ND_VOL;
    nvi.ear_level = UISND_2ND_VOL;
    (void) ui_put_nv (item, &nvi);
    uisnd_voice_vol( pMe->m_byEarpieceVolume, item);
    return (uint64)pMe->m_byEarpieceVolume;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */

    if(item == NV_SPEAKER_LVL_I) {
      pMe->m_byEarpieceVolume = nvi.speaker_level;
    }
    else if (item == NV_EAR_LVL_I) {
      pMe->m_byEarpieceVolume = nvi.ear_level;
    }
    else if (item == NV_EAR_LVL_SHADOW_I) {
      pMe->m_byEarpieceVolume = nvi.ear_lvl_shadow;
    }
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    else if (item == NV_SDAC_LVL_I) {
      pMe->m_byEarpieceVolume = nvi.sdac_lvl;
    }
#endif

    if (!RangeCheckVolume(pMe->m_byEarpieceVolume) ||
        (pMe->m_byEarpieceVolume == UISND_VOL_OFF)) {
      /* volume out of range */
      CORE_ERR ("Earpiece Volume out of range - GetEarpieceVolume", 0, 0, 0);
      pMe->m_byEarpieceVolume = UISND_2ND_VOL;
      nvi.ear_level = UISND_2ND_VOL;
      (void) ui_put_nv (item, &nvi);
    }
    uisnd_voice_vol( pMe->m_byEarpieceVolume, item);
#ifdef FEATURE_BT
    uisnd_voice_vol(pMe->m_byEarpieceVolume, NV_EAR_LVL_SHADOW_I);
#endif
    return (uint64)pMe->m_byEarpieceVolume;
  }

  return (uint64)UISND_4TH_VOL;
} /*End GetEarpieceVolume*/



/*===========================================================================
FUNCTION SetEarpieceVolume

DESCRIPTION
  Set earpiece information in NV, update sound driver

DEPENDENCIES
  GetEarpieceVolumeNVEnum, (void) ui_put_nv, uisnd_voice_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetEarpieceVolume(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  nv_items_enum_type item;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  item = GetEarpieceVolumeNVEnum();

  if (!RangeCheckVolume(byNewVal) || byNewVal == 0) {
    /* Volume out of Range */
    CORE_ERR ("Earpiece Volume out of range - SetEarpieceVolume", 0, 0, 0);
    byNewVal = UISND_2ND_VOL;
  }

  switch (item)
  {
    case NV_SPEAKER_LVL_I:
      nvi.speaker_level = byNewVal;
      break;
    case NV_EAR_LVL_I:
      nvi.ear_level = byNewVal;
      break;
    case NV_EAR_LVL_SHADOW_I:
      nvi.ear_lvl_shadow = byNewVal;
      break;
  #if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
    case NV_SDAC_LVL_I:
      nvi.sdac_lvl = byNewVal;
      break;
  #endif
    default:
      CORE_ERR("Invalid Item",0, 0, 0);
      return FALSE;
  }

  (void) ui_put_nv(item, &nvi);

  uisnd_voice_vol(byNewVal, item);
#ifdef FEATURE_BT
  uisnd_voice_vol(byNewVal, NV_EAR_LVL_SHADOW_I);
#endif
#ifdef FEATURE_TTY
  uisnd_tty_vol(byNewVal, SND_DEVICE_TTY_HCO);
#endif
  pMe->m_byEarpieceVolume = byNewVal;
  return TRUE;
} /*End SetEarpieceVolume*/



/*===========================================================================
FUNCTION GetMessageVolumeNVEnum

DESCRIPTION
  Return message alert NV item depending on various device parameters

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetMessageVolumeNVEnum()
{
  uisnd_notify_data_s_type info;

  uisnd_get_device(&info);
  return uisnd_map_device_to_nv_item(info.out_device, UISND_MSG_SND);
} /*End GetMessageVolumeNVEnum*/



/*===========================================================================
FUNCTION GetMessageVolume

DESCRIPTION
  Get Message alert volume from NV, update byMessageVolume and sound driver

DEPENDENCIES
  GetMessageVolumeNVEnum, ui_get_nv, uisnd_alert_vol

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetMessageVolume(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  nv_items_enum_type item;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  item = GetMessageVolumeNVEnum();

  result = ui_get_nv(item, &nvi);

  if (result == NV_NOTACTIVE_S) {
    pMe->m_byMessageVolume = UISND_2ND_VOL;
    nvi.alerts_lvl = UISND_2ND_VOL;
    (void) ui_put_nv (item, &nvi);
    uisnd_alert_vol( pMe->m_byMessageVolume, item);
    return (uint64)pMe->m_byMessageVolume;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */

    if(item == NV_ALERTS_LVL_I) {
      pMe->m_byMessageVolume = nvi.alerts_lvl;
    }
    else {
      pMe->m_byMessageVolume = nvi.alerts_lvl_shadow;
    }

    if (!RangeCheckVolume(pMe->m_byMessageVolume)) {
      /* volume out of range */
      CORE_ERR ("Message Volume out of range - GetMessageVolume", 0, 0, 0);
      pMe->m_byMessageVolume = UISND_2ND_VOL;
      nvi.alerts_lvl = UISND_2ND_VOL;
      (void) ui_put_nv (item, &nvi);
    }
    uisnd_alert_vol( pMe->m_byMessageVolume, item);
#ifdef FEATURE_BT
    uisnd_alert_vol( pMe->m_byMessageVolume, NV_ALERTS_LVL_SHADOW_I );
#endif
    return (uint64)pMe->m_byMessageVolume;
  }

  return (uint64) (NV_VALUE_ERROR_BYTE);
} /*End GetMessageVolume*/



/*===========================================================================
FUNCTION SetMessageVolume

DESCRIPTION
  Write message alert volume to NV, update static variable and sound driver

DEPENDENCIES
  GetMessageVolumeNVEnum, ui_put_nv, uisnd_alert_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetMessageVolume(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  nv_items_enum_type item;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (!RangeCheckVolume(byNewVal)) {
    /* Volume out of Range */
    CORE_ERR ("Message Volume out of range - SetMessageVolume", 0, 0, 0);
    byNewVal = UISND_2ND_VOL;
  }

  item = GetMessageVolumeNVEnum();
  if(item == NV_ALERTS_LVL_I) {
    nvi.alerts_lvl = byNewVal;
  }
  else
  {
    nvi.alerts_lvl_shadow = byNewVal;
  }
  (void) ui_put_nv(item, &nvi);

  uisnd_alert_vol( byNewVal, item);
#ifdef FEATURE_BT
  uisnd_alert_vol( byNewVal, NV_ALERTS_LVL_SHADOW_I );
#endif
  pMe->m_byMessageVolume = byNewVal;
  return TRUE;
} /*End SetMessageVolume*/

/*===========================================================================
FUNCTION GetSilenceAll

DESCRIPTION
  Get the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetSilenceAll(CCoreApp * pMe)
{
  boolean bIsSilenced;
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }
  /* looking up the corresponding NV variable */
  ICONFIG_GetItem(pMe->m_pIConfig, CFGI_SILENCEALL, &bIsSilenced,
        sizeof(boolean));
  CoreTask_SetSilenceAll(bIsSilenced);
  return (uint64)bIsSilenced;
}


/*===========================================================================
FUNCTION SetSilenceAll

DESCRIPTION
  set the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSilenceAll(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  /* set NV variable */
  ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SILENCEALL, &bNewVal,
        sizeof(boolean));
  CoreTask_SetSilenceAll(bNewVal);
  return TRUE;
} /*End SetSilenceAll*/



/*===========================================================================
FUNCTION GetAutoRetry

DESCRIPTION
  Get auto retry duration setting from NV

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoRetry(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }
  result = ui_get_nv(NV_AUTO_REDIAL_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) SetAutoRetry(pMe, (uint64)3);
    return (uint64)pMe->m_byAutoRetry;
  }
  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_byAutoRetry = nvi.auto_redial.rings;
    if (!RangeCheckAutoRetry(pMe->m_byAutoRetry)) {
      pMe->m_byAutoRetry = 3;
      nvi.auto_redial.rings = pMe->m_byAutoRetry;
      nvi.auto_redial.enable = TRUE;
      (void) ui_put_nv(NV_AUTO_REDIAL_I, &nvi);
    }
    return (uint64)pMe->m_byAutoRetry;
  }

  return (uint64) (NV_VALUE_ERROR_BYTE);
} /*End GetAutoRetry*/



/*===========================================================================
FUNCTION SetAutoRetry

DESCRIPTION
  Write auto retry information: interval + on/off, to NV

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoRetry(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_byAutoRetry = byNewVal;
  if (!RangeCheckAutoRetry(pMe->m_byAutoRetry)) {
     pMe->m_byAutoRetry = 3;
  }
  nvi.auto_redial.rings = pMe->m_byAutoRetry;
  nvi.auto_redial.enable = (pMe->m_byAutoRetry != 0);
  (void) ui_put_nv(NV_AUTO_REDIAL_I, &nvi);
  return TRUE;
} /*End SetAutoRetry*/


/*===========================================================================
FUNCTION GetIncallMixing

DESCRIPTION
  Get the static variable value

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetIncallMixing(CCoreApp * pMe)
{
  boolean bMixingEnabled;

  bMixingEnabled = uisnd_get_incall_mixing();
  return (uint64)bMixingEnabled;
}

/*===========================================================================
FUNCTION SetIncallMixing

DESCRIPTION
  Set the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetIncallMixing(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bNewVal = (boolean) nNewVal;

  uisnd_set_incall_mixing(bNewVal);
  return TRUE;
}

/*===========================================================================
FUNCTION GetAutoAnswer

DESCRIPTION
  Get auto answer (on/off) setting from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoAnswer(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }

  result = ui_get_nv(NV_AUTO_ANSWER_I, &nvi);

  if(result == NV_NOTACTIVE_S) {
    (void) SetAutoAnswer(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bAutoAnswer;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    if((nvi.auto_answer.rings != UI_AUTO_ANSWER_RINGS) && nvi.auto_answer.enable) {
    /* This should only happen during initialization, and should happen at most once */
      nvi.auto_answer.rings = UI_AUTO_ANSWER_RINGS;
      (void) ui_put_nv(NV_AUTO_ANSWER_I, &nvi);
    }
    pMe->m_bAutoAnswer = nvi.auto_answer.enable;
    return (uint64)pMe->m_bAutoAnswer;
  }

  return (uint64)FALSE;
} /*End GetAutoAnswer*/



/*===========================================================================
FUNCTION SetAutoAnswer

DESCRIPTION
  Write auto answer information (on/off + # of rings) to NV, update bAutoAnswer

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoAnswer(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_bAutoAnswer = bNewVal;
  nvi.auto_answer.enable = pMe->m_bAutoAnswer;
  if (pMe->m_bAutoAnswer) {
    nvi.auto_answer.rings = UI_AUTO_ANSWER_RINGS;
  }
  else
  {
    nvi.auto_answer.rings = 0;
  }

  (void) ui_put_nv(NV_AUTO_ANSWER_I, &nvi);

  return TRUE;

} /*End SetAutoAnswer */



/*===========================================================================
FUNCTION GetVR

DESCRIPTION
  Get VR (on/off) setting from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetVR(CCoreApp * pMe)
{
  boolean vrEnable;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_VR ,
      &vrEnable, sizeof(boolean))) != TRUE) {
    (void) SetVR(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bVR;
  }

  pMe->m_bVR = vrEnable;
  return (uint64)pMe->m_bVR;

} /*End GetVR*/



/*===========================================================================
FUNCTION SetVR

DESCRIPTION
  Write VR (on/off) to NV, update bVR

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetVR(CCoreApp * pMe, uint64 nNewVal)
{
  boolean origVRVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  origVRVal = pMe->m_bVR;
  pMe->m_bVR = (boolean)nNewVal;

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_VR ,
      &pMe->m_bVR, sizeof(boolean))) != TRUE) {
    CORE_ERR("Could not Set the Item",0,0,0);
    pMe->m_bVR = origVRVal;
    return FALSE;
  }

  return TRUE;

} /*End SetVR */




/*===========================================================================
FUNCTION GetBeepLength

DESCRIPTION
  Get beep length information from NV

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetBeepLength(CCoreApp * pMe)
{
  uint32 beepLength;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return (uint64)0;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_BEEP_LENGTH ,
      &beepLength, sizeof(beepLength))) != TRUE) {
      CORE_ERR("Unable to get Beep Length from ICONFIG", 0, 0, 0);
      return (uint64)0; // Could not get the value
  }

  /* range check for beeplength, default to OEMNV_DTMFCONT */
  if ((beepLength == OEMNV_DTMFOFF) ||
      (beepLength == OEMNV_DTMFBURST) ||
      (beepLength == OEMNV_DTMFCONT))
    pMe->m_byBeepLength = (uint8)beepLength;
  else
    pMe->m_byBeepLength = (uint8)OEMNV_DTMFCONT;

  CoreTask_SetBurst(pMe->m_byBeepLength == OEMNV_DTMFBURST);

  return (uint64)beepLength;

} /*End GetBeepLength*/



/*===========================================================================
FUNCTION SetBeepLength

DESCRIPTION
  Assign new value to beep length static variable and write to NV

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBeepLength(CCoreApp * pMe, uint64 nNewVal)
{
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_BEEP_LENGTH ,
      &nNewVal, sizeof(uint32))) != TRUE) {
     return FALSE;
  }
  /* range check for beeplength, default to OEMNV_DTMFCONT */
  if ((nNewVal == OEMNV_DTMFOFF) ||
      (nNewVal == OEMNV_DTMFBURST) ||
      (nNewVal == OEMNV_DTMFCONT))
    pMe->m_byBeepLength = (uint8)nNewVal;
  else
    pMe->m_byBeepLength = (uint8)OEMNV_DTMFCONT;

  CoreTask_SetBurst(pMe->m_byBeepLength == OEMNV_DTMFBURST);
  return TRUE;

} /*End SetBeepLength */



/*===========================================================================
FUNCTION GetRingerType

DESCRIPTION
  Get ringer type from NV, update current static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRingerType(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)0;
  }
  result = ui_get_nv(NV_RINGER_TYPE_I, &nvi);

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    /* range checking */
    if (nvi.ringer_type >= UISND_LAST_RING) {
      nvi.ringer_type = UISND_RING_NORMAL;
      (void) ui_put_nv (NV_RINGER_TYPE_I, &nvi);
    }

    pMe->m_RingerType = (uisnd_ringer_type)nvi.ringer_type;

    result = ui_get_nv (NV_MM_RINGER_FILE_I, &nvi);

    if (result == NV_DONE_S) {
      if (pMe->m_pszMMRingerFile) {
        FREE((void *)(pMe->m_pszMMRingerFile));
        pMe->m_pszMMRingerFile = NULL;
      }

#ifdef FEATURE_MULTIMEDIA
      if (MEMCMP((void *) nvi.mm_ringer_file, (void *)"Std", 3) != 0) {
        pMe->m_pszMMRingerFile = (char *)MALLOC(AEE_MAX_FILE_NAME*sizeof(char));
        (void) MEMCPY (pMe->m_pszMMRingerFile,
             (void *) nvi.mm_ringer_file, sizeof (nvi.mm_ringer_file));
        pMe->m_bMMRinger = TRUE;
      }
      else
#endif /* FEATURE_MULTIMEDIA */
      {
        pMe->m_bMMRinger = FALSE;
      }

    }
    return (uint64)pMe->m_RingerType;
  }

  return (uint64)0;
} /*End GetRingerType*/



/*===========================================================================
FUNCTION SetRingerType

DESCRIPTION
  Assign new value to ringer type and write to NV

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRingerType(CCoreApp * pMe, uint64 nNewVal)
{
  uisnd_ringer_type newVal = (uisnd_ringer_type) nNewVal;
  nv_item_type nvi;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* range checking */
  if (newVal >= UISND_LAST_RING)
    newVal = UISND_RING_NORMAL;

  pMe->m_RingerType = newVal;
  nvi.ringer_type = pMe->m_RingerType;
  (void) ui_put_nv(NV_RINGER_TYPE_I, &nvi);

  /* Multimedia Ringer */
#ifdef FEATURE_MULTIMEDIA
  if (pMe->m_bMMRinger) {
    (void) MEMCPY ((void *) nvi.mm_ringer_file,
         (void *)(pMe->m_pszMMRingerFile),
         sizeof(nvi.mm_ringer_file));
    (void) ui_put_nv (NV_MM_RINGER_FILE_I, &nvi);
  }
  else
  {
    (void) MEMCPY ((void *) nvi.mm_ringer_file, "Std\0", 4);
    (void) ui_put_nv (NV_MM_RINGER_FILE_I, &nvi);
    if (pMe->m_pszMMRingerFile) {
      FREE(pMe->m_pszMMRingerFile);
      pMe->m_pszMMRingerFile = NULL;
    }
  }
#endif /* FEATURE_MULTIMEDIA */

  /* Update volume since Ringer Type changes */
  (void) GetRingerVolume (pMe);
  return TRUE;

} /*End SetRingerType*/



/*===========================================================================
FUNCTION GetOneTouch

DESCRIPTION
  Get one touch information from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetOneTouch(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }
  result = ui_get_nv(NV_ONETOUCH_DIAL_I, &nvi);

  if(result == NV_NOTACTIVE_S) {
    (void) SetOneTouch(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bOneTouch;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_bOneTouch = nvi.onetouch_dial;
    return (uint64)pMe->m_bOneTouch;
  }

  return (uint64)FALSE;
} /*End GetOneTouch*/



/*===========================================================================
FUNCTION SetOneTouch

DESCRIPTION
  Set one touch setting to NV, update one touch static variable

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetOneTouch(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_bOneTouch = bNewVal;
  nvi.onetouch_dial = pMe->m_bOneTouch;
  (void) ui_put_nv(NV_ONETOUCH_DIAL_I, &nvi);
  return TRUE;
} /*End SetOneTouch*/

/*===========================================================================
FUNCTION GetKeyguard

DESCRIPTION
  Get keyguard value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetKeyguard (CCoreApp *pMe)
{
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)0;
  }
  return (uint64)(pMe->m_byKeyguard);
}

/*===========================================================================
FUNCTION SetKeyguard

DESCRIPTION
  assign new keyguard value, start key guard initiation timer as required

DEPENDENCIES
  CreateKeyGuardOnDlg

SIDE EFFECTS
  None
===========================================================================*/
boolean SetKeyguard(CCoreApp *pMe, uint64 nNewVal)
{
  int32 nTimeToStart;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_byKeyguard = byNewVal;
  nTimeToStart = pMe->m_byKeyguard * 1000;

  /* Start timer for keyguard selection other than the
       "Guard now" option
    */
  if(nTimeToStart != 0) {
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    nTimeToStart,
                    (PFNNOTIFY) CreateKeyGuardOnDlg,
                    (uint32*) pMe);
  }
  else
  { /* start the key guard dialogs immediately */
    ISHELL_SetTimer(pMe->a.m_pIShell,
                    100,
                    (PFNNOTIFY) CreateKeyGuardOnDlg,
                    (uint32*) pMe);
  }
  return TRUE;
} /* End SetKeyguard */


/*===========================================================================
FUNCTION GetSmartSound

DESCRIPTION
  Get smart sound information from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetSmartSound(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)(FALSE);
  }
  result = ui_get_nv(NV_AUTO_VOLUME_ENABLED_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) SetSmartSound(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bSmartSound;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_bSmartSound = nvi.auto_volume_enabled;
    uisnd_set_aagc(pMe->m_bSmartSound);
    return (uint64)pMe->m_bSmartSound;
  }

  return (uint64)FALSE;
} /*End GetSmartSound */



/*===========================================================================
FUNCTION SetSmartSound

DESCRIPTION
  Set smart sound variable, write to NV, update sound driver

DEPENDENCIES
  (void) ui_put_nv, uisnd_set_aagc

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSmartSound(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_bSmartSound = bNewVal;
  nvi.auto_volume_enabled = pMe->m_bSmartSound;
  (void) ui_put_nv(NV_AUTO_VOLUME_ENABLED_I, &nvi);
  uisnd_set_aagc(pMe->m_bSmartSound);
  return TRUE;
} /*End SetSmartSound*/



/*===========================================================================
FUNCTION GetBanner

DESCRIPTION
  Get NV banner information and update banner static variable

DEPENDENCIES
  ui_get_nv, STRTOWSTR

SIDE EFFECTS
  None
===========================================================================*/
AECHAR * GetBanner(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;
  byte pbyBannerBuf[BANNER_BUFFER_SIZE+1];

  if ((pMe == NULL) || (pMe->m_pIAnn == NULL)) {
    CORE_ERR ("Null pointer", 0,0,0);
    return NULL;
  }

  result = ui_get_nv(NV_BANNER_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) STRTOWSTR("QUALCOMM", pMe->m_pszBanner,(BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));
    (void) WSTRTOSTR(pMe->m_pszBanner, (char *) pbyBannerBuf,(BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));
    (void) MEMCPY((void *) nvi.banner.letters, pbyBannerBuf, sizeof(pbyBannerBuf));
    (void) ui_put_nv(NV_BANNER_I, &nvi);

    if (pMe->m_phInfo.oprt_mode != AEECM_OPRT_MODE_OFFLINE) {
      IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, pMe->m_pszBanner);
    }
    return pMe->m_pszBanner;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    (void) STRTOWSTR((char *) nvi.banner.letters,pMe->m_pszBanner,
           (BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));

    if (pMe->m_phInfo.oprt_mode != AEECM_OPRT_MODE_OFFLINE) {
      IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, pMe->m_pszBanner);
    }
    return pMe->m_pszBanner;
  }

  return NV_VALUE_ERROR_AECHAR;
} /*End GetBanner*/



/*===========================================================================
FUNCTION SetBanner

DESCRIPTION
  Write the new banner information to NV, and update static variable

DEPENDENCIES
  (void) ui_put_nv, WSTRTOSTR, memcpy

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBanner(CCoreApp * pMe,  AECHAR * psNewVal)
{
  nv_item_type nvi;
  byte pbyBannerBuf[BANNER_BUFFER_SIZE+1];

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  (void) WSTRLCPY (pMe->m_pszBanner,psNewVal,(BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));

  (void) WSTRTOSTR(psNewVal,(char *) pbyBannerBuf,(BANNER_BUFFER_SIZE+1)*sizeof(AECHAR));

  (void) MEMCPY((void *) nvi.banner.letters, pbyBannerBuf,sizeof(pbyBannerBuf));
  (void) ui_put_nv(NV_BANNER_I, &nvi);

  if (pMe->m_phInfo.oprt_mode != AEECM_OPRT_MODE_OFFLINE) {
    IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, pMe->m_pszBanner);
  }
  return TRUE;
} /*End SetBanner*/

/*===========================================================================
FUNCTION WATOI

DESCRIPTION
  Takes a AECHAR string and turns the result into an integer.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WATOI(const AECHAR *str)
{
  int ret_val = 0;
  int sign = 1;

  if (!str) return 0;

  /* Explicitly turn an empty string into zero */
  if (WSTRLEN(str) == 0) {
    return 0;
  }

  /* Get past any leading spaces. */
  while (*str == (AECHAR) ' ') { str++; }

  /* Check if its a negative number. */
  if (*str == (AECHAR) '-') {
    sign=-1;    /* Set the sign to negative. */
    str++;

//lint -save -e722 "Suspicious use of ;"
    while (*str == (AECHAR) ' ') { str++; }   /* Allow "- 5", for instance */
 //lint +e722 "Suspicious use of ;"
  }

  /* Loop until we find a non-digit (including NULL termination) */
  while ((*str >= (AECHAR) '0') && (*str <= (AECHAR) '9'))
  {
    ret_val = (ret_val * 10) + (*str - (AECHAR) '0');
    str++;
  }

  return (sign * ret_val);
} /*End WATOI */

/*===========================================================================
FUNCTION FormatTime

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
void FormatTime (JulianType julian, AECHAR *pszString, int nLen)
{
  char c = 'a';
  char* pszTemp = (char *) MALLOC(sizeof(char)*(uint32) (nLen+1));

  if(!pszTemp)
    return;
  if (!pszString) return;

  if (julian.wHour >= 12) {
    c = 'p';
    if (julian.wHour > 12) {
      julian.wHour -= 12;
    }
  }
  if (julian.wHour == 0 ) {
    julian.wHour = 12;
  }

  (void) SNPRINTF(pszTemp, sizeof(char)*(nLen+1),
                  "%2u:%02u%cm",julian.wHour,julian.wMinute, c);
  (void) STRTOWSTR (pszTemp, pszString, nLen * (int)sizeof(AECHAR));
  FREE(pszTemp);


}
/*===========================================================================
FUNCTION FormatDate

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
void FormatDate (JulianType julian, AECHAR *pszString, int nLen)
{
  char days[][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  char months[][4] = {"   ", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  int yr;
  char* pszTemp = (char *) MALLOC(sizeof(char)*(uint32)(nLen+1));

  if(!pszTemp)
    return;
  if (!pszString) return;

  if ((julian.wWeekDay > 6) || (julian.wMonth == 0) || (julian.wMonth > 12))
    return;

  yr = julian.wYear %100;
  (void) SNPRINTF(pszTemp, sizeof(char)*(nLen+1),
                  " %3s, %3s %2u \'%02u",
                  days[julian.wWeekDay],months[julian.wMonth],
                  julian.wDay, yr  );
  (void) STRTOWSTR (pszTemp, pszString, nLen * sizeof(AECHAR));
  FREE(pszTemp);

}

/*===========================================================================
FUNCTION FormatDateNumeric

DESCRIPTION
  Formats the Julian date, in numeric format, into the given string.
  The format is MM/DD/YYYY.

PARAMETERS
  CCoreApp *pMe:             Pointer to the CoreApp structure
  const JulianType *julian:  Pointer to the julian date
  AECHAR *pszString:         A buffer where the date will be stored
  int nLen:                  Length of the pszString buffer in AECHARs

DEPENDENCIES
  pszString must be valid

RETURN VALUE
  Number of bytes written into the given buffer.  Zero if failed,
  nonzero otherwise.

SIDE EFFECTS
  None
===========================================================================*/
#define FORMAT_BUFFER_SIZE 20
int FormatDateNumeric(CCoreApp *pMe, const JulianType *julian,
                      AECHAR *pszString, int nLen)
{
  AECHAR pszFormat[FORMAT_BUFFER_SIZE];
  int nByteCount;

  if ((!pszString) || (!julian) || (nLen < DATE_BUFFER_SIZE)) {
    return 0;
  }

  if ((julian->wWeekDay > 6) || (julian->wMonth == 0) || (julian->wMonth > 12)) {
    return 0;
  }

  nByteCount = ISHELL_LoadResString(pMe->a.m_pIShell, gpszResourceFile,
                                    IDS_DATE_FORMAT, pszFormat,
                                    (FORMAT_BUFFER_SIZE * sizeof(AECHAR)));
  if (0 == nByteCount) {
    // If we failed to load the string
    return 0;
  }

  WSPRINTF(pszString, (int)((uint32)nLen * sizeof(AECHAR)), pszFormat,
           (julian->wMonth), julian->wDay, julian->wYear);
  return WSTRLEN(pszString);

}



/*===========================================================================
 FUNCTION FormatTimeDate

 DESCRIPTION
  Formats the time and the date

 DEPENDENCIES
  FormatTime, FormatDate

SIDE EFFECTS
  None
===========================================================================*/
int FormatTimeDate(JulianType j_time, AECHAR *pszDest,  int nLen)
{
  int nCurLen = 0;

  FormatTime(j_time, pszDest, nLen);//format the time
  nCurLen = WSTRLEN (pszDest);

  FormatDate(j_time, &pszDest[nCurLen], (nLen-nCurLen));//format the date
  nCurLen = WSTRLEN (pszDest);
  return nCurLen;

}

/*===========================================================================
FUNCTION SetTimeofDay

DESCRIPTION
  Sets the system time

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean SetTimeofDay (CCoreApp *pMe, JulianType *julian)
/*lint -esym(715,julian) -esym(715,pMe) */
{
#ifdef FEATURE_USRSYSCLOCK
  uint32     UTCTimeInSeconds = 0;
  int32      utcOffset = 0;
  uint32     localTime = 0;
  int        iResult;
  AEETimeZoneType TimeZoneId;

  if( ( pMe == NULL ) || (pMe->m_pIUsrSysClock == NULL) )
  {
    CORE_ERR("Null pointers", 0, 0, 0);
    return FALSE;
  }

  iResult = IUSRSYSCLOCK_GetTimeZone(pMe->m_pIUsrSysClock, &TimeZoneId);
  if (iResult != SUCCESS) {
    CORE_ERR("FAILED to get timezone for IUsrSysClock, reason %d",iResult,0,0);
    return FALSE;
  }

  localTime = JULIANTOSECONDS(julian);

  iResult = IUSRSYSCLOCK_GetTimeOffset(pMe->m_pIUsrSysClock, localTime, TimeZoneId, &utcOffset, NULL);
  if ( iResult != SUCCESS) {
    CORE_ERR("FAILED to get timeoffset for IUsrSysClock, reason %d",iResult,0,0);
    return FALSE;
  }

  UTCTimeInSeconds = localTime - utcOffset;

  iResult = IUSRSYSCLOCK_SetTimeS(pMe->m_pIUsrSysClock, UTCTimeInSeconds);
  if ( iResult != SUCCESS) {
    CORE_ERR("FAILED to set time for IUsrSysClock, reason %d",iResult,0,0);
    return FALSE;
  }

  /* Start timer to display time */
  ISHELL_SetTimer(pMe->a.m_pIShell,
                      1000,
                      (PFNNOTIFY) DisplayTimeWithRefresh,
                      pMe);

  return TRUE;
#else  // FEATURE_USRSYSCLOCK
  return FALSE;
#endif // FEATURE_USRSYSCLOCK

}
/*===========================================================================
FUNCTION GetTimeofDay

DESCRIPTION
  Returns the current time

DEPENDENCIES
  none

RETURN VALUE
  TRUE - if time is valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean GetTimeofDay (CCoreApp *pMe, JulianType *julian_date)
{
#ifdef FEATURE_USRSYSCLOCK
  uint32     UTCTimeInSeconds = 0;
  int32      utcOffset = 0;
  uint32     localTime = 0;
  int        iResult;
  AEETimeZoneType TimeZoneId;

  if( ( pMe == NULL ) || (pMe->m_pIUsrSysClock == NULL) )
  {
    CORE_ERR("Null pointers", 0, 0, 0);
    return FALSE;
  }
  if(IUSRSYSCLOCK_IsTimeSet(pMe->m_pIUsrSysClock)){
    // user time has been set.

    // get the UTC time
    iResult = IUSRSYSCLOCK_GetTimeS(pMe->m_pIUsrSysClock, &UTCTimeInSeconds);
    if (iResult == EBADSTATE) {
      CORE_MSG("Unreliable time for IUsrSysClock",0,0,0);
    }
    else
    {
      if (iResult != SUCCESS) {
        CORE_ERR("FAILED to get time for IUsrSysClock",0,0,0);
        return FALSE;
      }
    }
    iResult = IUSRSYSCLOCK_GetTimeZone(pMe->m_pIUsrSysClock, &TimeZoneId);
    if (iResult != SUCCESS) {
      CORE_ERR("FAILED to get timezone for IUsrSysClock, reason %d",iResult,0,0);
      return FALSE;
    }
    iResult = IUSRSYSCLOCK_GetTimeOffset(pMe->m_pIUsrSysClock, UTCTimeInSeconds, TimeZoneId, &utcOffset, NULL);
    if ( iResult != SUCCESS) {
      CORE_ERR("FAILED to get timeoffset for IUsrSysClock, reason %d",iResult,0,0);
      return FALSE;
    }

    // Convert the local time zone to offset in seconds
    // local time = UTC offset +  local time zone offset

    localTime = (uint32) ((int32)UTCTimeInSeconds + utcOffset);
    GETJULIANDATE(localTime, julian_date);

    return TRUE;
  } else {
    GETJULIANDATE(0, julian_date);
    /* Process time only if valid */
    if (julian_date->wYear != 1980) {
      return TRUE;
    }
  }
#else
  /*lint -esym(715, pMe) */
  GETJULIANDATE(0, julian_date);
  /* Process time only if valid */
  if (julian_date->wYear != 1980) {
    return TRUE;
  }
#endif // FEATURE_USRSYSCLOCK
  return FALSE;

}


/*===========================================================================
FUNCTION DisplayTimeWithRefresh

DESCRIPTION
  Displays the current time on the primary display.  If there is a
  secondary display, it also prints the time and date there.  This
  function also sets a timer, so that it will be periodically invoked.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void DisplayTimeWithRefresh (CCoreApp *pMe)
{
  int time_to_redraw;
  JulianType julian_date;

  if( !GetDisplayStatus()){
    // if the LCD is off, do not update the time
    /*lint -esym(715, time_to_redraw)*/
    ISHELL_CancelTimer (pMe->a.m_pIShell, (PFNNOTIFY) DisplayTimeWithRefresh, pMe);
    return;
  }

  (void) DisplayTimeUpdateScreen(pMe);

  /* Set to update when the minute timer rolls over.  */
  /* A service change will bring us back sooner. */
  if (GetTimeofDay(pMe, &julian_date) != FALSE) {
    time_to_redraw = 1000*(60-julian_date.wSecond);
  } else {
    time_to_redraw = 60000;
  }

  /* Set the timer.  If it was already set in SetTimeofDay, this will
   * just restart the timer.
   */
  ISHELL_SetTimer(pMe->a.m_pIShell,
                  time_to_redraw,
                  (PFNNOTIFY) DisplayTimeWithRefresh,
                  pMe);
  return;
}

/*===========================================================================
FUNCTION DisplayTimeUpdateScreen

DESCRIPTION
  Displays the current time on the primary display.  If there is a
  secondary display, it also prints the time and date there.

RETURN VALUE
  TRUE if the displayed time changed

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean DisplayTimeUpdateScreen (CCoreApp *pMe)
{
  JulianType julian_date;
#ifdef FEATURE_SECONDARY_DISPLAY
  int        nCurLen;
#endif
  boolean    bRetVal;
#ifdef FEATURE_USRSYSCLOCK
  IConfig    *pIConfig = NULL;
  boolean    update_user_time = FALSE;

  uint32     UTCTimeInSeconds = 0;
  int32      utcOffset = 0;
  uint32     localTime = 0;
  int        iResult;
  AEETimeZoneType TimeZoneId;
#endif // FEATURE_USRSYSCLOCK

  /* use the mobility management info to display time, else
     use the current settings
  */

  (void) STRTOWSTR ("\0", pMe->m_pszTime, (TIME_BUFFER_SIZE+1)*sizeof(AECHAR));

#ifdef FEATURE_USRSYSCLOCK
  if (!pMe->a.m_pIShell) {
     return FALSE;
  }
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG, (void **)&pIConfig)
      != SUCCESS)
  {
    return FALSE;
  }
  if (pIConfig == NULL) return FALSE;

  if (ICONFIG_GetItem(pIConfig, CFGI_AUTO_TIME_ENABLE, (void *)&update_user_time,
                          sizeof(update_user_time)) != SUCCESS) {
    update_user_time = FALSE;
  }

  (void )ICONFIG_Release(pIConfig);
  pIConfig = NULL;

  if (update_user_time == TRUE)
  {

    // get the UTC time
    iResult = IUSRSYSCLOCK_GetTimeS(pMe->m_pIUsrSysClock, &UTCTimeInSeconds);
    if (iResult != SUCCESS) {
      CORE_ERR("FAILED to get time for ISysClock, reason %d",iResult,0,0);
      return FALSE;
    }

    iResult = IUSRSYSCLOCK_GetTimeZone(pMe->m_pIUsrSysClock, &TimeZoneId);
    if ( iResult!= SUCCESS) {
      CORE_ERR("FAILED to get timezone for ISysClock, reason %d",iResult,0,0);
      return FALSE;
    }

    iResult = IUSRSYSCLOCK_GetTimeOffset(pMe->m_pIUsrSysClock, UTCTimeInSeconds, TimeZoneId, &utcOffset, NULL);
    if ( iResult != SUCCESS) {
      CORE_ERR("FAILED to get timezone for ISysClock, reason %d",iResult,0,0);
      return FALSE;
    }

    // Convert the local time zone to offset in seconds
    // local time = UTC offset +  local time zone offset

    localTime = (uint32) ((int32)UTCTimeInSeconds + utcOffset);

    // convert seconds to Julian date.
    GETJULIANDATE(localTime, &julian_date);

    FormatTime(julian_date, pMe->m_pszTime, TIME_BUFFER_SIZE);
    // Invalidate Timeafter Setting it once.

    // pMe->m_ssInfo.mm_information.time_and_time_zone_avail = FALSE;
    // Time not correct 100% of the time - TBD
  } else {

    if (GetTimeofDay(pMe,&julian_date) != FALSE) {
      /* Process time only if valid */
      FormatTime(julian_date, pMe->m_pszTime, TIME_BUFFER_SIZE);
    }
  }
#else
  if (GetTimeofDay(pMe, &julian_date) != FALSE)
  {
    /* Process time only if valid */
    FormatTime(julian_date, pMe->m_pszTime, TIME_BUFFER_SIZE);
  }
#endif // FEATURE_USRSYSCLOCK

  if (pMe->m_pIAnn) {
    IANNUNCIATOR_SetFieldEx (pMe->m_pIAnn, ANNUN_FIELD_TIME, pMe->m_pszTime);
  }

#ifdef FEATURE_SECONDARY_DISPLAY
  // Now, update the secondary display, if present
  if ((NULL != pMe->m_pIDisp2) && (NULL != pMe->m_pTimeDate) &&
      (NULL != pMe->m_pszTimeDate)) {
    FormatTime(julian_date, pMe->m_pszTimeDate, TIMEDATE_BUFFER_SIZE);
    nCurLen = WSTRLEN(pMe->m_pszTimeDate);

    // Add a newline and then increment the string length accordingly
    pMe->m_pszTimeDate[nCurLen] = (AECHAR)'\n';
    pMe->m_pszTimeDate[nCurLen+ 1] = (AECHAR)'\0';
    nCurLen++;

    // Add the date to the end
    nCurLen = FormatDateNumeric(pMe, &julian_date,
                                (pMe->m_pszTimeDate + nCurLen),
                                (TIMEDATE_BUFFER_SIZE - nCurLen));

    /* If we could not construct the date string, log an error and just
     * show the time.
     */
    if (0 == nCurLen) {
      MSG_ERROR("Unable to format the date string.", 0, 0, 0);
    }

    ISTATIC_SetText(pMe->m_pTimeDate, NULL, pMe->m_pszTimeDate,
                    AEE_FONT_LARGE, AEE_FONT_LARGE);
    ISTATIC_Redraw(pMe->m_pTimeDate);

  }
#endif

  if (0 == WSTRNCMP(pMe->m_pszTime, pMe->m_pszLastTime, TIME_BUFFER_SIZE+1)) {
    bRetVal = FALSE;
  }
  else
  {
    bRetVal = TRUE;
  }

  (void)MEMCPY(pMe->m_pszLastTime, pMe->m_pszTime, (TIME_BUFFER_SIZE+1)*sizeof(AECHAR));

  return bRetVal;
}


/*===========================================================================
FUNCTION GetPrivacy

DESCRIPTION
  Return privacy option from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPrivacy(CCoreApp * pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64) (NV_VALUE_ERROR_BYTE);
  }

  result = ui_get_nv(NV_VOICE_PRIV_I, &nvi);

  if (result == NV_NOTACTIVE_S) {
    (void) SetPrivacy(pMe, (uint64)TRUE);
    return (uint64)pMe->m_byPrivacy;
  }

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    pMe->m_byPrivacy = nvi.voice_priv;
    return (uint64)pMe->m_byPrivacy;
  }

  return ((uint64)NV_VALUE_ERROR_BYTE);
} /*End GetPrivacy */



/*===========================================================================
FUNCTION SetPrivacy

DESCRIPTION
  update static variable, write to NV and the corresponding cm info

DEPENDENCIES
  ui_cm_privacy_pref, (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPrivacy(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  int nReturnStatus;
  byte byNewVal = (byte) nNewVal;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_byPrivacy = byNewVal;
  nvi.voice_priv = pMe->m_byPrivacy;
  (void) ui_put_nv(NV_VOICE_PRIV_I, &nvi);
  if (nNewVal == TRUE) {
    nReturnStatus =  ICM_SetPrivacyPref(pMe->m_pICM,
                         AEECM_PRIVACY_PREF_ENHANCED); /* enhance */
  }
  else
  {
    nReturnStatus =  ICM_SetPrivacyPref(pMe->m_pICM,
                         AEECM_PRIVACY_PREF_STANDARD); /* enhance */
  }
  return (nReturnStatus == AEE_SUCCESS);

} /*End SetPrivacy */


/*===========================================================================
FUNCTION GetPINLock1

DESCRIPTION
  based on Security state, set and return the PINLock1 status

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock1(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return (uint64)FALSE;
  }
  switch(pMe->m_sPIN1_lock_state)
  {
    case AEECARD_PIN_DISABLED:
    case AEECARD_PIN_NOT_VALID:
    case AEECARD_PIN_UNKNOWN:
      return (uint64)FALSE;
    default:
      return (uint64)TRUE;
  }
} /* End GetPINLock1 */


/*===========================================================================
FUNCTION SetPINLock1

DESCRIPTION
  Set PIN Lock using ICARD functions

DEPENDENCIES
  ICARD_EnablePIN, ICARD_DisablePIN

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock1(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  if (bNewVal) {
    (void) CoreSecurity_EnablePIN(pMe, AEECARD_PIN1);
  } else {
    (void) CoreSecurity_DisablePIN(pMe, AEECARD_PIN1);
  }
  return TRUE;
}

/*===========================================================================
FUNCTION GetNVLock

DESCRIPTION
  Return the pMe->m_sNV_lock_state variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNVLock(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return (uint64)FALSE;
  }
  switch(pMe->m_sNV_lock_state)
  {
    case AEECARD_PIN_DISABLED:
    case AEECARD_PIN_NOT_VALID:
    case AEECARD_PIN_UNKNOWN:
      return (uint64)FALSE;
    default:
      return (uint64)TRUE;
  }
}



/*===========================================================================
FUNCTION SetNVLock

DESCRIPTION
  Set the NV lock code

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNVLock(CCoreApp * pMe, uint64 nNewVal)
{
  nv_item_type nvi;
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }
  if (VerifyNVLockCode(pMe->m_sPIN.code)) {
    nvi.auto_lock = (boolean)nNewVal;
    (void) ui_put_nv(NV_AUTO_LOCK_I, &nvi);
    if (nvi.auto_lock) {
      pMe->m_sNV_lock_state = AEECARD_PIN_ENABLED;
    }
    else {
      pMe->m_sNV_lock_state = AEECARD_PIN_DISABLED;
    }
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                           EVT_USER, EVT_PIN_ENABLE_SUCCESS, 0L); /* 0 to indicate NV */
    /* success */
  }
  else {
    /* fail */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                     EVT_USER, EVT_PIN_ENABLE_FAIL, 0L); /* 0 to indicate NV */
  }
  return TRUE;

}


/*===========================================================================
FUNCTION GetPINLock2

DESCRIPTION
  Return the pMe->m_bPIN2_enabled variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock2(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return (uint64)FALSE;
  }
  switch(pMe->m_sPIN2_lock_state)
  {
    case AEECARD_PIN_DISABLED:
    case AEECARD_PIN_NOT_VALID:
    case AEECARD_PIN_UNKNOWN:
      return (uint64)FALSE;
    default:
      return (uint64)TRUE;
  }
}



/*===========================================================================
FUNCTION SetPINLock2

DESCRIPTION
  Call ICARD function to enable or disable PIN2 based on the input param

DEPENDENCIES
  ICARD_EnablePin, ICARD_DisablePin

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock2(CCoreApp * pMe, uint64 nNewVal )
{
  boolean bNewVal = (boolean) nNewVal;
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  if (bNewVal) {
    (void) CoreSecurity_EnablePIN(pMe, AEECARD_PIN2);
  } else {
    (void) CoreSecurity_DisablePIN(pMe, AEECARD_PIN2);
  }
  return TRUE;
}

#ifdef FEATURE_MMGSDI_DUAL_SLOT

/*===========================================================================
FUNCTION GetPINLock1Slot2

DESCRIPTION
  Return the pMe->m_sPIN1_slot2_lock_state variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock1Slot2(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return (uint64)FALSE;
  }
  switch(pMe->m_sPIN1_slot2_lock_state)
  {
    case AEECARD_PIN_DISABLED:
    case AEECARD_PIN_NOT_VALID:
    case AEECARD_PIN_UNKNOWN:
      return (uint64)FALSE;
    default:
      return (uint64)TRUE;
  }
} /* End GetPINLock1Slot2 */


/*===========================================================================
FUNCTION SetPINLock1Slot2

DESCRIPTION
  Set PIN Lock for slot 2 using ICARD functions

DEPENDENCIES
  ICARD_EnablePIN, ICARD_DisablePIN

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock1Slot2(CCoreApp * pMe, uint64 nNewVal)
{
  boolean bNewVal = (boolean) nNewVal;

  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  if (bNewVal) {
    (void) CoreSecurity_EnablePIN(pMe, AEECARD_PIN1_SLOT2);
  } else {
    (void) CoreSecurity_DisablePIN(pMe, AEECARD_PIN1_SLOT2);
  }
  return TRUE;
}



/*===========================================================================
FUNCTION GetPINLock2Slot2

DESCRIPTION
  Return the pMe->m_sPIN2_slot2_lock_state variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock2Slot2(CCoreApp * pMe)
{
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return (uint64)FALSE;
  }
  switch(pMe->m_sPIN2_slot2_lock_state)
  {
    case AEECARD_PIN_DISABLED:
    case AEECARD_PIN_NOT_VALID:
    case AEECARD_PIN_UNKNOWN:
      return (uint64)FALSE;
    default:
      return (uint64)TRUE;
  }
}



/*===========================================================================
FUNCTION SetPINLock2Slot2

DESCRIPTION
  Call ICARD function to enable or disable PIN2 for slot 2 based on the
  input param

DEPENDENCIES
  ICARD_EnablePin, ICARD_DisablePin

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock2Slot2(CCoreApp * pMe, uint64 nNewVal )
{
  boolean bNewVal = (boolean) nNewVal;
  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  if (bNewVal) {
    (void) CoreSecurity_EnablePIN(pMe, AEECARD_PIN2_SLOT2);
  } else {
    (void) CoreSecurity_DisablePIN(pMe, AEECARD_PIN2_SLOT2);
  }
  return TRUE;
}
#endif /*FEATURE_MMGSDI_DUAL_SLOT */


/*===========================================================================
FUNCTION DisplayIdleMenuDlg

DESCRIPTION
  Bring the program to the Idle menu screen display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean DisplayIdleMenuDlg(CCoreApp *pMe)
{
  IDialog *pIActiveDialog;
  uint16 wDlgID;
  IMenuCtl* pControl;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL) { /* error */
    return FALSE;
  }
  wDlgID = IDIALOG_GetID(pIActiveDialog);

  if(wDlgID == 0) {
    return FALSE;
  }

  if((wDlgID == IDD_NOCARD_EMERGENCY) || (wDlgID == IDD_CARD_INVALID)
    || (wDlgID == IDD_CORE)) {
    /* Set menu selection to first item */
    if (wDlgID == IDD_CORE) {
      pControl = (IMenuCtl *) IDIALOG_GetControl (pIActiveDialog, IDC_ICONV_CORE);
      if (pControl != NULL) {
        IMENUCTL_SetSel(pControl, (uint16) -1);
      }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
      ISHELL_SetTimer(pMe->a.m_pIShell,
                      COREAPPTIMEOUT,
                      (PFNNOTIFY) SetSelOperatorDisplay,
                      (uint32*) pMe);
#endif
    }
    return TRUE;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* User has not made first selection, power up scenario */
  if (!pMe->m_bUserMadeFirstSelSub) {
    return TRUE;
  }
  else {
    /* non power up scenario */
    if ((wDlgID == IDD_SELECT_TECH_FOR_BOTH_CARDS ) ||
        (wDlgID ==  IDD_SELECT_TECH_FOR_CARD_1 ) ||
        (wDlgID == IDD_SELECT_TECH_FOR_CARD_2 ))
    {
      return TRUE;
    }
  }
#else
  if ((pMe->m_dwSecurityMask & COREAPP_NOT_READY_MASK) <
       DetermineSecurityMask(pMe)) {
    /* not finish with security initialization yet */
    return TRUE;
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  /* cancel this timer if it is still running */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  PopDlg (pMe, IDD_CORE);

  /* Set menu selection to first item */
  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL) { /* error */
    return FALSE;
  }

  pControl = (IMenuCtl *) IDIALOG_GetControl (pIActiveDialog, IDC_ICONV_CORE);
  if (pControl != NULL) {
    IMENUCTL_SetSel(pControl, (uint16) -1);
  }

#ifdef FEATURE_GSTK
  if(!AEECM_IS_ANYCALL_PRESENT(pMe->m_pICM) && pMe->m_pISTK)
  {
    ISTK_IdleScreen(pMe->m_pISTK);
  }
#endif
  return TRUE;
} /*End DisplayIdleMenuDlg*/


/*===========================================================================
FUNCTION DisplayMessageDlg

DESCRIPTION
  Create a message dialog based in resource id of string
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayMessageDlg(CCoreApp *pMe, uint16 nStringID, int nDisplaySeconds)
{
  int nReturnStatus;

  pMe->m_MessageID = nStringID;
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_GENERAL_MESSAGE, NULL);

  if(CheckAEEReturnStatus(nReturnStatus)) {
    /* Reset idle Timer, since we are successful in creating the message display */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

    if (nDisplaySeconds > 0) {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      (int) nDisplaySeconds*1000,
                      (PFNNOTIFY) EndMessageDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
  }


} /*End DisplayMessageDlg*/

/*===========================================================================
FUNCTION DisplayMessageDlgEx

DESCRIPTION
  Create a message dialog and display the text passed in.
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayMessageDlgEx(CCoreApp *pMe, AECHAR *pszText, int nDisplaySeconds)
{
  int nReturnStatus;

  if (pszText == NULL) {
    CORE_ERR ("Null pointer", 0, 0, 0);
    return;
  }

  if (pMe->m_pszMsgString != NULL) {
    FREE(pMe->m_pszMsgString);
  }

  pMe->m_pszMsgString = WSTRDUP (pszText);

  if (pMe->m_pszMsgString == NULL) {
    CORE_ERR ("Null pointer", 0, 0, 0);
    return;
  }

  pMe->m_MessageID = 0;

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                  IDD_GENERAL_MESSAGE, NULL);

  if(CheckAEEReturnStatus(nReturnStatus)) {
    /* Reset idle Timer, since we are successful in creating the message display */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

    if (nDisplaySeconds > 0) {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      (int) nDisplaySeconds*1000,
                      (PFNNOTIFY) EndMessageDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
  }


} /*End DisplayMessageDlgEx*/


/*===========================================================================
FUNCTION PopDlg

DESCRIPTION
  Pop dialog stack until the specified dialog.
  If not found, go back till the core dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void PopDlg(CCoreApp* pMe, uint16 wPopDlgID)
{
  int nReturnStatus;
  IDialog* pIDialog;
  uint16 wDlgID;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  if (wPopDlgID == 0) {
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog == NULL) { /* error */
    return;
  }
  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) { /* error */
    return;
  }
  for (; (wDlgID != IDD_CORE) && (wDlgID != wPopDlgID);) {
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    (void)CheckAEEReturnStatus(nReturnStatus);

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);  /* Get the next active dlg */
    if(pIDialog == NULL) { /* error */
      return;
    }
    wDlgID = IDIALOG_GetID(pIDialog);
    if(wDlgID == 0) {
      return;
    }
  }
} /*End PopDlg */



/*===========================================================================
FUNCTION SetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetGeneralStaticTextStyle(IStatic *pIStaticText)
{
  ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
  ISTATIC_SetActive(pIStaticText, TRUE);
  return TRUE;
} /*End SetGeneralStaticTextStyle */



/*===========================================================================
FUNCTION SetStaticTextCtlSize

DESCRIPTION
  Set the rect for static control when there are one other control presented
  in the dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl)
{
  AEERect CtrlRect;
  AEERect StaticRect;

  if ( (pICurrentStatic == NULL) || (pIOtherCtrl == NULL)) { /* error checking */
    return FALSE;
  }

  /* Set static control size */
  ICONTROL_GetRect(pIOtherCtrl, &CtrlRect);
  ISTATIC_GetRect(pICurrentStatic, &StaticRect);

  StaticRect.dy = CtrlRect.y - 1;
  ISTATIC_SetRect(pICurrentStatic, &StaticRect);
  return TRUE;
} /*End SetStaticTextCtlSize*/




/*===========================================================================
FUNCTION IsMessageDlg

DESCRIPTION
  Is the dialog ID one of those in the LookupDisplayMessageCtlID?

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean IsMessageDlg(uint16 wDlgID)
{
  if (wDlgID == IDD_GENERAL_MESSAGE)
    return TRUE;
  return FALSE;
} /* End IsMessageDlg */



/*===========================================================================
FUNCTION EndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Send an EVT_MESSAGE_END event
===========================================================================*/
void EndMessageDlg(CCoreApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0) { /* error */
      return;
    }
    if (IsMessageDlg(wDlgID)) {
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER, EVT_MESSAGE_END, wDlgID);
    }
    else {
      CORE_ERR("EndMessageDlg(): End non Message Dlg",
              0, 0, 0);
    }
  }
  /* error if reach this point */
} /*End EndMessageDlg*/



/*===========================================================================
FUNCTION EndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void EndCurrentDlg(CCoreApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0) { /* error */
      return;
    }

    if (wDlgID == IDD_CORE) { /* do not end the core idle dialog */
      return;
    }

    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }
} /*End EndCurrentDlg */


/*===========================================================================
FUNCTION MessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean MessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IStatic * pIStatic;
  IDialog * pIDialog;
  uint16 wDlgID;
  int nReturnStatus;

  CCoreApp * pMe = (CCoreApp*) pUser;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0)) {
    return FALSE;
  }
#endif

  switch (evt) {
  case EVT_DIALOG_START:
    pIStatic = (IStatic*) IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_MESSAGE);
    if(pIStatic != NULL) {
      if (pMe->m_MessageID != 0) {
        AECHAR *pBuf=NULL;
        (void) SetGeneralStaticTextStyle(pIStatic);
        pBuf = MALLOC (MAX_STR_SIZE*sizeof(AECHAR));
        if (pBuf == NULL) {
          CORE_ERR ("No memory", 0,0,0);
          return FALSE;
        }
        if (ISHELL_LoadResString (pMe->a.m_pIShell, gpszResourceFile, pMe->m_MessageID,
                                  pBuf, MAX_STR_SIZE * sizeof(AECHAR)) != 0) {
          ISTATIC_SetText (pIStatic, NULL, pBuf, AEE_FONT_BOLD, AEE_FONT_BOLD);
          IDIALOG_Redraw ((IDialog*)dw);
        }
        FREE (pBuf);
      }
      else {
        if (pMe->m_pszMsgString != NULL) {
          ISTATIC_SetText (pIStatic, NULL, pMe->m_pszMsgString, AEE_FONT_BOLD,
                            AEE_FONT_BOLD);
          IDIALOG_Redraw ((IDialog*)dw);
        }
      }
    }
    else {
      CORE_ERR("FAILED to get pIStatic for control id",0,0,0);
    }
    return TRUE;

  case EVT_KEY:

    if (w == AVK_END) { /* go all the way back to idle screen */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIDialog == NULL) {
        return FALSE; /* error */
      }
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        return FALSE;
      }

      /* ending the message NOW -> cancel end message timer */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndMessageDlg, pMe);
      EndMessageDlg(pMe);

      if ((pMe->m_MessageID == IDS_PIN1_INCORRECT) ||
          (pMe->m_MessageID == IDS_EMERGENCY_ONLY) ||
          (pMe->m_MessageID == IDS_PIN1_BLOCKED) ||
          (pMe->m_MessageID == IDS_NEW_PIN1_MISMATCH)) {
        return TRUE;
      }
      else {  /* if not displaying security related screens,
         end key brings user back to the idle menu */
        return FALSE;
      }
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
    /* clear key */
    case AVK_CLR:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      /* these key events will make the dialog goes away and also cancel the timer
      if this action is taken before the timer is being triggered */
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndMessageDlg, pMe);
      EndMessageDlg(pMe);
      return TRUE;

    default:
      return FALSE; /* let core app handles these events */
    }

  case EVT_COMMAND:
    return TRUE;

  case EVT_DIALOG_END:
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndMessageDlg, pMe);
    /* Clear all variables and free memory if any */
    pMe->m_MessageID = 0;
    if (pMe->m_pszMsgString) {
      FREE (pMe->m_pszMsgString);
      pMe->m_pszMsgString = NULL;
    }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    if(pMe->m_bNetSelErr) {
       pMe->m_bNetSelErr = FALSE;
       DisplayPLMNListAgain(pMe);
       return TRUE;
    }
#endif /*FEATURE_WCDMA || FEATURE_GSM */
    if (pMe->m_bSecurityTimer) {
      /* reset the timer for display idle screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      COREAPPTIMEOUT,
                      (PFNNOTIFY) DisplayIdleMenuDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /*End MessageDlgEventHandler*/


/*===========================================================================
FUNCTION HourGlassDlgEventHandler

DESCRIPTION
  handle hour glass message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean HourGlassDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  IStatic * pIStatic;
  int nReturnStatus;

  CCoreApp * pMe = (CCoreApp*) pUser;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0)) {
    return FALSE;
  }
#endif

  switch (evt)
  {
  case EVT_DIALOG_START:

    pIStatic = (IStatic*) IDIALOG_GetControl((IDialog*)dw, IDC_HRGLS_MESSAGE);
    (void) SetGeneralStaticTextStyle(pIStatic);
    return TRUE;

  case EVT_KEY:

    switch(w) {
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
      /* these key events will make the dialog goes away */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    default:
      return FALSE; /* let core app handles these events */
    }

  case EVT_COMMAND:
    return TRUE;

  case EVT_USER:

    switch(w)
    {
    /* pass the event back to the previous dialog */
    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
    case EVT_PIN_VERIFY_FAIL:
    case EVT_PIN_ENABLE_FAIL:
    case EVT_PIN_CHANGE_FAIL:
    case EVT_PUK_VERIFY_FAIL:
    case EVT_RESTRICT_UPDATE_FDN_FAIL:
    case EVT_RESTRICT_UPDATE_FDN_SUCCESS:
#ifdef FEATURE_MMGSDI_PERSONALIZATION
    case EVT_PERSO_DEACT_SUCCESS: /* Deactivate success */
    case EVT_PERSO_ACT_SUCCESS: /* Activate success */
    case EVT_PERSO_DEACT_FAIL: /* Deactivate failed */
    case EVT_PERSO_ACT_FAIL: /* Activate failed */
    case EVT_PERSO_SET_DATA_SUCCESS:
    case EVT_PERSO_SET_DATA_FAIL:
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
      /* Notify the CORE_Menu event handler to start the Core Menu */
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER, w, dw);
      /* End this message dialog */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    default:
      break;
    }

    return FALSE;

  case EVT_DIALOG_END:
    if (pMe->m_bSecurityTimer) {
      /* reset the timer for display idle screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      COREAPPTIMEOUT,
                      (PFNNOTIFY) DisplayIdleMenuDlg,
                      (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return TRUE;
  }
} /*End HourGlassDlgEventHandler*/

/*===========================================================================
FUNCTION Min2ToAsc

DESCRIPTION
  Convert a MIN2 type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int Min2ToAsc(
  word wAreaCode,                 /* min2 to convert from */
  char *pszText                       /* room for 3 digits */
)
{
  int nLen = 0;

  pszText[nLen++] = DigitToAscTable[ wAreaCode/100 ];
  wAreaCode %= 100;
  pszText[nLen++] = DigitToAscTable[ wAreaCode/10 ];
  pszText[nLen++] = DigitToAscTable[ wAreaCode%10 ];
  return nLen;
} /* End Min2ToAsc */



/*===========================================================================
FUNCTION Min1ToAsc

DESCRIPTION
  Convert a MIN1 type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int Min1ToAsc(
  dword dwPhoneNo,                 /* min1 to convert from */
  char *pszText                       /* room for 8 digits */
)
{
  word wTemp;
  int nLen = 0;

  if( dwPhoneNo == 0 ) {
    for( wTemp=0; wTemp<7; wTemp++ ) {
      pszText[nLen++] = '0';
    }
  }
  else {
    wTemp = (word) (dwPhoneNo>>14 );
    pszText[nLen++] = DigitToAscTable[ wTemp/100 ];
    wTemp %= 100;
    pszText[nLen++] = DigitToAscTable[ wTemp/10 ];
    pszText[nLen++] = DigitToAscTable[ wTemp%10 ];
    dwPhoneNo &= 0x3FFFL;                /* get bottom 14 bits */
    /* next digit is top 4 bits */
    wTemp = (word) (( dwPhoneNo >> 10 ) & 0xF );
    pszText[nLen++] = (char) ( ( ( wTemp == 10 ) ? 0 : wTemp ) + '0' );
    wTemp = (word) ( dwPhoneNo & 0x3FF ); /* get bottom 10 bits */
    pszText[nLen++] = DigitToAscTable[ wTemp/100 ];
    wTemp %= 100;
    pszText[nLen++] = DigitToAscTable[ wTemp/10 ];
    pszText[nLen++] = DigitToAscTable[ wTemp%10 ];
  }
  return nLen;
} /* End Min1ToAsc */


/*===========================================================================
FUNCTION MccToAsc

DESCRIPTION
  Convert a MCC type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int MccToAsc(
  word wMcc,                       /* mcc to convert from */
  char *pszTxt                       /* room for 3 digits */
)
{
  int nLen = 0;

  pszTxt[nLen++] = DigitToAscTable[ wMcc/100 ];
  wMcc %= 100;
  pszTxt[nLen++] = DigitToAscTable[ wMcc/10 ];
  pszTxt[nLen++] = DigitToAscTable[ wMcc%10 ];
  return nLen;
} /* End MccToAsc */



/*===========================================================================
FUNCTION MncToAsc

DESCRIPTION
  Convert a MNC type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int MncToAsc(
  word wMnc,                       /* mnc to convert from */
  char *pszTxt                       /* room for 2 digits */
)
{
  int nLen = 0;

  pszTxt[nLen++] = DigitToAscTable[ wMnc/10 ];
  pszTxt[nLen++] = DigitToAscTable[ wMnc%10 ];
  return nLen;
} /*End MncToAsc */


/*===========================================================================
FUNCTION ExitEmergencyMode

DESCRIPTION

  Exit from the Emergency mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ExitEmergencyMode(CCoreApp *pMe)
{
  int nReturnStatus = SUCCESS;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }


  CoreUtil_StatusCb.pfnCancel = NULL;
  CoreUtil_StatusCb.pfnNotify = CoreUtil_SystemPrefErrCb;
  CoreUtil_StatusCb.pNotifyData = pMe;

    /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                      AEECM_MODE_PREF_PERSISTENT,AEECM_PREF_TERM_PERMANENT, 0,
                      AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, pMe->m_phInfo.band_pref,
                      AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                      AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                      NULL, &status, &CoreUtil_StatusCb
                      );
    if (nReturnStatus == SUCCESS) {
      return TRUE;
    }


  return FALSE;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION CoreUtil_GetNetworksCb

DESCRIPTION

  List of avialable PLMNs is sent by ICM with this callback.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_GetNetworksCb(CCoreApp *pMe)
{
  int nReturnStatus;
  IDialog *pIDialog;
  CORE_MSG("Get NWCb", 0, 0, 0);



  //Create the menu dialog if non-empty PLMN list returned by N/W.
  //Display the list and handle selection.

  //First clear the previous "Searching" Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if((pIDialog == NULL) || (IDD_PLMN_SEARCHING != IDIALOG_GetID(pIDialog))) {
    CORE_ERR("GetNetwork cb in non-IDD_PLMN_SEARCHING", 0, 0, 0);
  }
  else
  {
    ISHELL_EndDialog(pMe->a.m_pIShell);
  }

  if(pMe->m_PhError != AEECM_PH_ERROR_NONE) {
    CORE_ERR("Get Networks Cancelled due to error code %d returned by CM",pMe->m_PhError,0,0);
    pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    FREEIF(pMe->m_pDetailedPLMNList);
    FREEIF(pMe->m_pCopyDetailedPLMNList);
    //Now display the error message.
    DisplayMessageDlg(pMe, IDS_PLMN_SEARCH_CANCELED, 3);
    return;
  }

  if((NULL == pMe->m_pCopyDetailedPLMNList) || (NULL == pMe->m_pDetailedPLMNList) || (0 == pMe->m_pDetailedPLMNList->length)) {
    CORE_ERR("Empty PLMN List", 0, 0, 0);
    pMe->m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    FREEIF(pMe->m_pDetailedPLMNList);
    FREEIF(pMe->m_pCopyDetailedPLMNList);
    //Now display the error message.
    DisplayMessageDlg(pMe, IDS_PLMN_LIST_EMPTY, 3);
    return;
  }

  MEMCPY(pMe->m_pCopyDetailedPLMNList, pMe->m_pDetailedPLMNList,
    sizeof(AEECMDetailedPLMNList));

  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                  gpszResourceFile, IDD_PLMN_LIST, NULL);
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE) {
    CORE_ERR("Create IDD_PLMN_LIST failed", 0, 0, 0);
  }

}


/*===========================================================================
FUNCTION SetNetworkAutoSelection

DESCRIPTION

  Change network select setting.
  Save it and return TRUE if request to CM has been sent,
  return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNetworkAutoSelection(CCoreApp *pMe,
                                AEECMModePref mode_pref,
                                AEECMPrefTerm term_pref,
                                AEECMAcqOrderPref acq_order,
                                AEECMBandPref band_pref,
                                AEECMRoamPref roam_pref,
                                AEECMHybrPref hyb_pref,
                                AEECMServDomPref srv_domain_pref)
{

  int nReturnStatus;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  CoreUtil_StatusCb.pfnCancel = NULL;
  CoreUtil_StatusCb.pfnNotify = CoreUtil_SystemPrefErrCb;
  CoreUtil_StatusCb.pNotifyData = pMe;

  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    mode_pref, term_pref,
                    0, acq_order,
                    band_pref, roam_pref,
                    hyb_pref, srv_domain_pref,
                    AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC, NULL, /* plmn id */
                    &status, &CoreUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION SetNetworkManualSelection

DESCRIPTION

  Change network select setting.
  Save it and return TRUE if request to CM has been sent,
  return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNetworkManualSelection(CCoreApp *pMe, uint32 nNewSetting)
{

  int nReturnStatus;
  AEECMModePref mode_pref = AEECM_MODE_PREF_NO_CHANGE;
  AEECMPrefTerm term_pref = AEECM_PREF_TERM_PERMANENT;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  CoreUtil_StatusCb.pfnCancel = NULL;
  CoreUtil_StatusCb.pfnNotify = CoreUtil_SystemPrefErrCb;
  CoreUtil_StatusCb.pNotifyData = pMe;


  if(pMe->m_nNWSelectionMode == APPCOMFUNC_NW_SEL_MAN_PLMN_WRAT)
  {
    if(pMe->m_pDetailedPLMNList->info[nNewSetting].rat == AEECM_RAT_GSM_RADIO_ACCESS) {
      mode_pref = AEECM_MODE_PREF_GSM_ONLY;
      term_pref = AEECM_PREF_TERM_NET_SEL_MANUAL;
    }
    else if(pMe->m_pDetailedPLMNList->info[nNewSetting].rat == AEECM_RAT_UMTS_RADIO_ACCESS) {
      mode_pref = AEECM_MODE_PREF_WCDMA_ONLY;
      term_pref = AEECM_PREF_TERM_NET_SEL_MANUAL;
    }

  }
  MSG_HIGH("PLMN ID Selected - %d%d%d",
           (int)pMe->m_pDetailedPLMNList->info[nNewSetting].plmn.identity[0],
           (int)pMe->m_pDetailedPLMNList->info[nNewSetting].plmn.identity[1],
           (int)pMe->m_pDetailedPLMNList->info[nNewSetting].plmn.identity[2]);
  /* Send CM command */
  nReturnStatus = ICM_SetSystemPreference(pMe->m_pICM,
                    mode_pref, term_pref,
                    0, AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                    AEECM_BAND_PREF_NO_CHANGE, AEECM_ROAM_PREF_NO_CHANGE,
                    AEECM_HYBR_PREF_NO_CHANGE, AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                    AEECM_NETWORK_SEL_MODE_PREF_MANUAL,
                    &pMe->m_pDetailedPLMNList->info[nNewSetting].plmn, /* plmn id */
                    &status, &CoreUtil_StatusCb /* error cb */
                    );

  if (nReturnStatus == SUCCESS) {
    pMe->m_bNetSel = TRUE;
    return TRUE;
  }
  else {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION GetNetworkSelect

DESCRIPTION
  Get Network -> Select menu setting.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNetworkSelect(CCoreApp *pMe)
{
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return 0;
  }
/*lint -e571*/
  return (uint64) (pMe->m_phInfo.network_sel_mode_pref);
/*lint +e571*/
}

#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
/*===========================================================================
FUNCTION GetRTREConfig

DESCRIPTION
  Get RTRE Config.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRTREConfig(CCoreApp *pMe)
{
  nv_item_type nvi;
  nv_stat_enum_type result;

  result = ui_get_nv (NV_RTRE_CONFIG_I, &nvi);

  if(result != NV_DONE_S) {  /* write to NV */
#ifdef FEATURE_RTRE_DEFAULT_IS_NV
    nvi.rtre_config = NV_RTRE_CONFIG_NV_ONLY;
#else
    nvi.rtre_config = NV_RTRE_CONFIG_RUIM_ONLY;
#endif
    (void) ui_put_nv (NV_RTRE_CONFIG_I, &nvi);
  }
  return (uint64)(nvi.rtre_config);
}

/*===========================================================================
FUNCTION SetRTREConfig

DESCRIPTION
  Set RTRE Config.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRTREConfig(CCoreApp *pMe, uint64 nNewSetting)
{
  int retVal;

  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }

  switch (nNewSetting)
  {
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
    case NV_RTRE_CONFIG_SIM_ACCESS:
      pMe->m_bySubscriptionSent &= ~COREAPP_GW_SUBS_AVAIL_MASK;
      pMe->m_bySubscriptionSent &= ~COREAPP_CDMA_SUBS_AVAIL_MASK;
      retVal = ICM_SetRTREConfig(pMe->m_pICM, AEECM_RTRE_CONFIG_SIM_ACCESS);
      break;
#endif
    case NV_RTRE_CONFIG_RUIM_ONLY:
      pMe->m_bySubscriptionSent &= ~COREAPP_GW_SUBS_AVAIL_MASK;
      pMe->m_bySubscriptionSent &= ~COREAPP_CDMA_SUBS_AVAIL_MASK;
      retVal = ICM_SetRTREConfig(pMe->m_pICM, AEECM_RTRE_CONFIG_RUIM_ONLY);
      break;
    case NV_RTRE_CONFIG_NV_ONLY:
      pMe->m_bySubscriptionSent &= ~COREAPP_GW_SUBS_AVAIL_MASK;
      pMe->m_bySubscriptionSent &= ~COREAPP_CDMA_SUBS_AVAIL_MASK;
      retVal = ICM_SetRTREConfig(pMe->m_pICM, AEECM_RTRE_CONFIG_NV_ONLY);
      break;
    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
    default:
      pMe->m_bySubscriptionSent &= ~COREAPP_GW_SUBS_AVAIL_MASK;
      pMe->m_bySubscriptionSent &= ~COREAPP_CDMA_SUBS_AVAIL_MASK;
      retVal = ICM_SetRTREConfig(pMe->m_pICM, AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK);
      break;
  }
  return (retVal == AEE_SUCCESS) ? TRUE : FALSE;
}
#endif /* F_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION UserSlotPref_cb

DESCRIPTION

  UserSlotPref_cb is called in response to ICARD_ChangeUserSlotPref
  and will check the status the action.

  Fail:    post EVT_USER_SLOT_PREF_FAIL event to coreapp
  Success: ignore

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  UserSlotPref_cb(void *pNotifyData)
{
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  pMe->m_bSlotChangePending = FALSE;
  /* did user pref slot setting complete successfully ? */
  if (sUserPrefSlotActionStatus.nCmdStatus != SUCCESS) {
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_USER_SLOT_PREF_FAIL, 0);
  }
  else {
    /* success */
    if (sUserPrefSlotActionStatus.dwData == (uint32)FALSE) {
      /* there is no pending refresh to be performed */
      ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_USER_SLOT_PREF_SUCCESS, 0);
    }
  }
  return;
}

/*===========================================================================
FUNCTION GetUserSlotPrefGSM

DESCRIPTION
  Return the user preferred slot for GSM from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetUserSlotPrefGSM(CCoreApp * pMe)
{
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)(NV_VALUE_ERROR_BYTE);
  }

  nReturnStatus = ICARD_GetUserSlotPref(pMe->m_pICard,
                                        AEECARD_GSM_APP, &(pMe->m_byGSMUserSlotPref));

  if (nReturnStatus != SUCCESS)
    return ((uint64)NV_VALUE_ERROR_BYTE);

  return (uint64)pMe->m_byGSMUserSlotPref;
}

/*===========================================================================
FUNCTION SetUserSlotPrefGSM

DESCRIPTION
  Call gsdi function set the User Slot Preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetUserSlotPrefGSM(CCoreApp * pMe, uint64 nNewVal )
{
  int nReturnStatus;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }
  pMe->m_byGSMUserSlotPref = byNewVal;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = UserSlotPref_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  nReturnStatus = ICARD_ChangeUserSlotPref(pMe->m_pICard,
                       (uint8)byNewVal, AEECARD_GSM_APP,
                       &sUserPrefSlotActionStatus,
                       &pMe->m_sCallback);
  return (nReturnStatus == SUCCESS);
}

/*===========================================================================
FUNCTION GetUserSlotPrefCDMA

DESCRIPTION
  Return the user preferred slot for CDMA from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetUserSlotPrefCDMA(CCoreApp * pMe)
{
  int nReturnStatus;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return ((uint64)NV_VALUE_ERROR_BYTE);
  }

  nReturnStatus = ICARD_GetUserSlotPref(pMe->m_pICard,
                                       AEECARD_CDMA_APP,
                                       &(pMe->m_byCDMAUserSlotPref));

  if (nReturnStatus != SUCCESS)
    return ((uint64)NV_VALUE_ERROR_BYTE);

  return (uint64)pMe->m_byCDMAUserSlotPref;

}

/*===========================================================================
FUNCTION SetUserSlotPrefCDMA

DESCRIPTION
  Call gsdi function set the User Slot Preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetUserSlotPrefCDMA(CCoreApp * pMe, uint64 nNewVal )
{
  int nReturnStatus;
  byte byNewVal = (byte) nNewVal;

  if (pMe == NULL) {
    CORE_ERR("pMe is Null", 0, 0, 0);
    return FALSE;
  }
  pMe->m_byCDMAUserSlotPref = byNewVal;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = UserSlotPref_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  nReturnStatus = ICARD_ChangeUserSlotPref(pMe->m_pICard,
                                          (uint8)byNewVal,
                                          AEECARD_CDMA_APP,
                                           &sUserPrefSlotActionStatus,
                                          &pMe->m_sCallback);

  return (nReturnStatus == SUCCESS);
}


/*===========================================================================
FUNCTION GetCards

DESCRIPTION
  Check how many cards are inserted

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCards(CCoreApp * pMe)
{
  boolean card1= FALSE , card2 = FALSE;

  if (pMe == NULL) {
    CORE_ERR("Null pMe", 0, 0, 0);
    return COREAPP_NO_CARD;
  }

  if ((pMe->m_nCardStatus != AEECARD_NO_CARD) ||
      (pMe->m_nCardStatus != AEECARD_INVALID_CARD) ) {
      card1= TRUE;
  }

  if ((pMe->m_nCardStatus_slot2 != AEECARD_NO_CARD) ||
        (pMe->m_nCardStatus_slot2 != AEECARD_INVALID_CARD)) {
      card2 = TRUE;
  }

  if(card1)
    return COREAPP_CARD1;
  if(card2)
    return COREAPP_CARD2;

  return COREAPP_NO_CARD;

}
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */
/*===========================================================================
FUNCTION IsCardValid

DESCRIPTION
  check if the card is ok

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IsCardValid(CCoreApp *pMe, uint8 byPinID)
{
  uint8 lock_state;
  uint8 card_status;

  if (pMe == NULL) {
    CORE_ERR("Null pMe", 0, 0, 0);
    return FALSE;
  }

  switch(byPinID)
  {
    case AEECARD_PIN1:
      lock_state = pMe->m_sPIN1_lock_state;
      card_status =pMe->m_nCardStatus;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      lock_state = pMe->m_sPIN1_slot2_lock_state;
      card_status =pMe->m_nCardStatus_slot2;
      break;
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */
    default:
      //bad Pin
      return FALSE;
  }

  if(card_status == AEECARD_VALID_CARD ||
     card_status == AEECARD_AVAIL_CARD ||
     card_status == AEECARD_NOT_INIT ||
     card_status == AEECARD_CARD_REINIT)
  {
    if(lock_state != AEECARD_PIN_UNKNOWN &&
       lock_state != AEECARD_PIN_PERM_DISABLED &&
       lock_state != AEECARD_PIN_NOT_VALID)
    {
      return TRUE;
    }
  }
  return FALSE;
}



#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION CoreUtil_FormatPLMN

DESCRIPTION

 Format a string corresponding to PLMN  to be displayed in PLMN List.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_FormatPLMN(AECHAR* pszBuf,
                         int nBufSize,
                         AEECMDetailedPLMNInfo *pDetailedPLMNInfo,
                         boolean bDisplayRAT)
{

  uinetwk_network_info_s_type nwInfo;
  sys_plmn_id_s_type plmn;
  char *buf ;
  AECHAR *wBuf ;

  PRINT_FUNCTION_NAME();

  if((pszBuf == NULL) || (nBufSize <= 0) || (pDetailedPLMNInfo == NULL)) {
    CORE_ERR("Bad Parameters", 0, 0, 0);
    return;
  }

  pszBuf[0] = '\0';

  buf = (char*) MALLOC(MAX_STR_SIZE*sizeof(char));
  wBuf = (AECHAR*) MALLOC(MAX_STR_SIZE*sizeof(AECHAR));

  if((buf == NULL) || (wBuf == NULL)) {
    CORE_ERR("Malloc failed", 0, 0, 0);
    FREEIF(buf);
    FREEIF(wBuf);
    return;
  }

  // PLMN Id
  plmn.identity[0] = pDetailedPLMNInfo->plmn.identity[0];
  plmn.identity[1] = pDetailedPLMNInfo->plmn.identity[1];
  plmn.identity[2] = pDetailedPLMNInfo->plmn.identity[2];
  //TODO: Change this to ICM plmn_id type.
  if(ui_netwk_get_network_info(plmn, &nwInfo)) {
    (void) SNPRINTF(buf,MAX_STR_SIZE, "PLMN:%s,",nwInfo.short_name_ptr);
  }
  else
  {
    (void) SNPRINTF(buf, MAX_STR_SIZE, "PLMN:%d  %d,", nwInfo.mcc, nwInfo.mnc);
  }

  (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
  (void) WSTRLCAT(pszBuf, wBuf, nBufSize);

  // Forbidden?

  if(pDetailedPLMNInfo->plmn_forbidden) {
    (void) SNPRINTF(buf,MAX_STR_SIZE,"Forbidden,");
    (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
    (void) WSTRLCAT(pszBuf, wBuf, nBufSize);
  }

  buf[0] = '\0';
  // CS,PS?
  if(pDetailedPLMNInfo->plmn_service_capability == AEECM_SRV_CAPABILITY_CS_SERVICE_ONLY) {
    (void) SNPRINTF(buf,MAX_STR_SIZE,"Domain Pref: CS,");
  }
  else if(pDetailedPLMNInfo->plmn_service_capability == AEECM_SRV_CAPABILITY_PS_SERVICE_ONLY) {
    (void) SNPRINTF(buf,MAX_STR_SIZE,"Domain Pref: PS");
  }
  else if(pDetailedPLMNInfo->plmn_service_capability == AEECM_SRV_CAPABILITY_CS_PS_SERVICE) {
   (void) SNPRINTF(buf,MAX_STR_SIZE,"Domain Pref: CS,PS,");
  }

  (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
  (void) WSTRLCAT(pszBuf, wBuf, nBufSize);

  if(bDisplayRAT) {
    if(pDetailedPLMNInfo->rat == AEECM_RAT_GSM_RADIO_ACCESS) {
      (void) SNPRINTF(buf,MAX_STR_SIZE,"RAT: GSM");
    }
    else if(pDetailedPLMNInfo->rat == AEECM_RAT_UMTS_RADIO_ACCESS) {
      (void) SNPRINTF(buf,MAX_STR_SIZE,"RAT: UMTS");
    }
    else
    {
      (void) SNPRINTF(buf,MAX_STR_SIZE,"RAT: Invalid");
    }

    (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* MAX_STR_SIZE);
    (void) WSTRLCAT(pszBuf, wBuf, nBufSize);
  }
  FREE(buf);
  FREE(wBuf);
}
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/


/*===========================================================================
FUNCTION GetGameMode

DESCRIPTION
    Return the current phone operating mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetGameMode(CCoreApp *pMe)
{

  boolean game_mode;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_GAME_MODE,
      &game_mode, sizeof(game_mode))) != TRUE) {
    (void) SetGameMode(pMe, 0);
    return (uint64)0;
  }

  return (uint64)game_mode;
}

/*===========================================================================
FUNCTION SetGameMode

DESCRIPTION
    Set the phone operating mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetGameMode(CCoreApp *pMe, uint64 nNewVal)
{
  uint32 optMode;
  boolean game_mode = (boolean) nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  (game_mode) ? (optMode=AEECM_OPRT_MODE_LPM) : (optMode=AEECM_OPRT_MODE_ONLINE);

  if(SetOprtMode(pMe, optMode)) {
    CoreTask_SetGameMode(game_mode);

    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_GAME_MODE,
        &game_mode, sizeof(game_mode))) != TRUE) {
       return FALSE;
    }
    return TRUE;
  }

  CORE_ERR("Unable to set Game Mode %d", nNewVal, 0, 0);
  return FALSE;
}

/*===========================================================================
FUNCTION SetLPMMode

DESCRIPTION
    Set the phone operating mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetLPMMode(CCoreApp *pMe, uint32 nNewVal)
{
  if (nNewVal != FALSE) {
    if (SetOprtMode(pMe, AEECM_OPRT_MODE_LPM)) {
      pMe->m_bLPM = nNewVal;
      return TRUE;
    }
  } else {
    //This sets the operating mode to either LPM or ONLINE
    //based on game mode flag
    if (SetGameMode(pMe, GetGameMode(pMe))) {
      pMe->m_bLPM = nNewVal;
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION GetOprtMode

DESCRIPTION
    Return the current phone operating mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 GetOprtMode(CCoreApp *pMe)
{
  /*lint -e571*/
  return (uint32)(pMe->m_phInfo.oprt_mode);
  /*lint +e571*/
}

/*===========================================================================
FUNCTION SetOprtMode

DESCRIPTION
    Set the phone operating mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetOprtMode(CCoreApp *pMe, uint32 nNewVal)
{
  if(pMe->m_pICM != NULL) {
    if(ICM_SetOperatingMode(pMe->m_pICM, (AEECMOprtMode) nNewVal)== SUCCESS) {
      pMe->m_phInfo.oprt_mode = (AEECMOprtMode) nNewVal;
      return TRUE;
    }
  }
  CORE_ERR("Unable to set OprtMode %d", nNewVal, 0, 0);
  return FALSE;
}
/*===========================================================================
FUNCTION GetCUGEnable

DESCRIPTION
    Return  if CUG is enabled or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGEnable(CCoreApp *pMe)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint64) FALSE;
    }

    return (uint64) cugInfo.present;

  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return (uint64) FALSE;
  }
}

/*===========================================================================
FUNCTION SetCUGEnable

DESCRIPTION
    Set if CUG is enabled or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGEnable(CCoreApp *pMe, uint64 nNewVal)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    cugInfo.present = (boolean) nNewVal;

    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to set cugInfo using IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    return TRUE;
  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return FALSE;
  }
}
/*===========================================================================
FUNCTION GetCUGPrefGroup

DESCRIPTION
    Return if the CUG Pref Group is supressed or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGPrefGroup(CCoreApp *pMe)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint64) FALSE;
    }
    if( cugInfo.present ) {
     return (uint64) cugInfo.cm_suppress_pref_cug;
    }
    else
    {
      CORE_ERR("CugInfo not present", 0, 0, 0);
      return (uint64) FALSE;
    }

  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return (uint64) FALSE;
  }
}

/*===========================================================================
FUNCTION SetCUGPrefGroup

DESCRIPTION
    Set CUG Pref group Supress/ No supress.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGPrefGroup(CCoreApp *pMe, uint64 nNewVal)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    cugInfo.cm_suppress_pref_cug = (boolean) nNewVal;
    cugInfo.present = TRUE;
    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to set cugInfo using IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    return TRUE;
  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return FALSE;
  }
}
/*===========================================================================
FUNCTION GetCUGOutAccess

DESCRIPTION
    Return  if CUG Out Access is supressed or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGOutAccess(CCoreApp *pMe)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint64) FALSE;
    }

    if(cugInfo.present) {
      return (uint64) cugInfo.cm_suppress_outgoing_access;
    }
    else
    {
      CORE_ERR("CugInfo not present", 0, 0, 0);
      return (uint64) FALSE;
    }
  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return (uint64) FALSE;
  }
}

/*===========================================================================
FUNCTION SetCUGOutAccess

DESCRIPTION
    Set CUG Out Access Supress/ No Supress

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGOutAccess(CCoreApp *pMe, uint64 nNewVal)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    cugInfo.cm_suppress_outgoing_access = (boolean) nNewVal;
    cugInfo.present = TRUE;
    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to set cugInfo using IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    return TRUE;
  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return FALSE;
  }
}
/*===========================================================================
FUNCTION GetCUGGroupID

DESCRIPTION
    Return  CUG Group ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 GetCUGGroupID(CCoreApp *pMe)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint16)0 ;
    }
    if(cugInfo.present && cugInfo.cm_cug_index.present) {
      return (uint16) ((cugInfo.cm_cug_index.msb << 8) | (cugInfo.cm_cug_index.lsb));
    }
    else
    {
      CORE_ERR("cugInfo missing", 0, 0, 0);
      return 0;
    }

  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return (uint32) FALSE;
  }
}

/*===========================================================================
FUNCTION SetCUGGroupID

DESCRIPTION
    Set CUG Group ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGGroupID(CCoreApp *pMe, uint16 nNewVal)
{
  AEECMFwdCugInfo cugInfo;
  PRINT_FUNCTION_NAME();
  if(pMe->m_pIConfig != NULL) {
    if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to get cugInfo from IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }
    cugInfo.present = TRUE;
    cugInfo.cm_cug_index.present = TRUE;
    cugInfo.cm_cug_index.lsb = nNewVal & 0xFF;
    cugInfo.cm_cug_index.msb = (nNewVal & 0xFF00) >> 8;

    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CUG ,
      &cugInfo, sizeof(AEECMFwdCugInfo))) != TRUE) {
      CORE_ERR("Unable to set cugInfo using IConfig", 0, 0, 0);
      return (uint32) FALSE;
    }

    return TRUE;
  }
  else
  {
    CORE_ERR("IConfig NULL", 0, 0, 0);
    return FALSE;
  }
}

#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
#ifdef TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
/*===========================================================================
FUNCTION CoreUtil_AbortPPPDataSession

DESCRIPTION
    Abort PPP Data session

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_AbortPPPDataSession(void *pUser)
{
  dss_net_policy_info_type policy_info;
  sint15 dss_errno;
  dss_iface_id_type iface_id;

  dss_init_net_policy_info(&policy_info);                     /* set the default values for the network policy */
  policy_info. policy_flag =  DSS_IFACE_POLICY_UP_SPEC ;      /* Up preferred policy */
  policy_info.iface.kind = DSS_IFACE_NAME;
  policy_info.iface.info.name =   DSS_IFACE_CDMA_SN;           /* 1x interface */
  policy_info.is_routeable = TRUE;
  iface_id = dss_get_iface_id_by_policy( policy_info, &dss_errno);
  MSG_MED( "End Key Pressed...UI sends GO NULL command to Data...", 0, 0, 0 );
  dss_iface_ioctl( iface_id, DSS_IFACE_IOCTL_GO_NULL,  NULL, &dss_errno);
}
#endif //TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
#endif //FEATURE_HDR_SESSION_TIED_TO_PPP


/*===========================================================================
FUNCTION UpdateCoreText

DESCRIPTION
Change the core menu screen text.

DEPENDENCIES
  m_pszIdleMenuText set to the text to be displayed.

SIDE EFFECTS
  None
===========================================================================*/
void UpdateCoreText(CCoreApp *pMe)
{
  IStatic *pIStatic;
  uint32 dwProperties;

  if (pMe->m_pIdleDialog)
  {
    pIStatic = (IStatic*) IDIALOG_GetControl(pMe->m_pIdleDialog, IDC_STATIC_CORE_MESSAGE);
    if(pIStatic != NULL) {
      dwProperties = ISTATIC_GetProperties(pIStatic);
      ISTATIC_SetProperties(pIStatic, dwProperties | ST_CENTERTEXT);
      if(WSTRLEN(pMe->m_pszIdleMenuText) != 0) {
        ISTATIC_SetText(pIStatic, NULL, pMe->m_pszIdleMenuText,
            AEE_FONT_NORMAL, AEE_FONT_BOLD);
      }
      else
      {
        ISTATIC_SetText(pIStatic, NULL, NULL, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      }
      ISTATIC_Redraw(pIStatic);
    }

    ResizeCoreDlg(pMe);
  }
} /* UpdateCoreText */


/*===========================================================================
FUNCTION ResizeCoreDlg

DESCRIPTION
Resize the static ctl and the icon view ctl of the core menu.
Called after updating the static ctl text.

DEPENDENCIES
  pMe->m_pszIdleMenuText set to the text in static ctl.

SIDE EFFECTS
  None
===========================================================================*/
void ResizeCoreDlg(CCoreApp *pMe)
{
  IDialog *pIDialog;
  IStatic *pIStatic;
  IMenuCtl *pIMenuCtl;
  AEERect rect;
  int y = 0;
  int h = pMe->m_cyScreen;
  int w = pMe->m_cxScreen;
  int minIconSize = 60 ; //Do't allow static ctl to occupy the full-screen.

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog != NULL) {
    if(pMe->m_pIdleDialog != NULL) {
      pIStatic = (IStatic*) IDIALOG_GetControl(pMe->m_pIdleDialog, IDC_STATIC_CORE_MESSAGE);
      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pMe->m_pIdleDialog, IDC_ICONV_CORE);

      if(pMe->m_pszIdleMenuText != NULL) {
        y = h;
      }
      if(pIStatic != NULL) {
        ISTATIC_SetProperties(pIStatic, ISTATIC_GetProperties(pIStatic)| ST_NOSCROLL);
        SETAEERECT(&rect, 0, 0, w, y);
        ISTATIC_SizeToFit(pIStatic, &rect);
        SETAEERECT(&rect, 0, 0, w, MIN(rect.dy, (pMe->m_cyScreen - minIconSize)));
        ICONTROL_SetRect((IControl*) pIStatic, &rect);
        SETAEERECT(&rect, 0, rect.dy, w, h - rect.dy);
        ICONTROL_SetRect((IControl*) pIMenuCtl, &rect);
        ISTATIC_SetActive(pIStatic, FALSE);
        IDIALOG_SetFocus(pMe->m_pIdleDialog, IDC_ICONV_CORE);
        if( IDD_CORE == IDIALOG_GetID(pIDialog)) {
          ISTATIC_Redraw(pIStatic);
          IMENUCTL_Redraw(pIMenuCtl);
          IDIALOG_Redraw(pMe->m_pIdleDialog);
        }
      }
    }
  }
} /* ReizeCoreDlg */

/*===========================================================================
FUNCTION SetUSBHFKAttached

DESCRIPTION
  Used to set whether or not a USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetUSBHFKAttached(boolean new_value)
{
  IConfig *pIConfig = NULL;
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL) {
    MSG_ERROR("Shell ptr null", 0, 0, 0);
    return;
  }

  ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);
  if (pIConfig) {
    ICONFIG_SetItem(pIConfig, CFGI_USB_HFK_ATTACHED, &new_value, 1);
    ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION GetUSBHFKAttached

DESCRIPTION
  Used to get whether or not a USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean GetUSBHFKAttached(void)
{
  boolean hfk = FALSE;

  IConfig *pIConfig = NULL;
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL) {
    MSG_ERROR("Shell ptr null", 0, 0, 0);
    return FALSE;
  }

  ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);
  if (pIConfig) {
    ICONFIG_GetItem(pIConfig, CFGI_USB_HFK_ATTACHED, &hfk, 1);
    ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }

  return  hfk;
}

/*===========================================================================
FUNCTION SetStereoUSBHFKAttached

DESCRIPTION
  Used to set whether or not a Stereo USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetStereoUSBHFKAttached(boolean new_value)
{
  IConfig *pIConfig = NULL;
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL) {
    MSG_ERROR("Shell ptr null", 0, 0, 0);
    return;
  }

  ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);
  if (pIConfig) {
    ICONFIG_SetItem(pIConfig, CFGI_STEREO_USB_HFK_ATTACHED, &new_value, 1);
    ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION GetStereoUSBHFKAttached

DESCRIPTION
  Used to get whether or not a USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean GetStereoUSBHFKAttached(void)
{
  boolean hfk = FALSE;

  IConfig *pIConfig = NULL;
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL) {
    MSG_ERROR("Shell ptr null", 0, 0, 0);
    return FALSE;
  }

  ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);
  if (pIConfig) {
    ICONFIG_GetItem(pIConfig, CFGI_STEREO_USB_HFK_ATTACHED, &hfk, 1);
    ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }

  return  hfk;
}

/*===========================================================================
FUNCTION GetSpeakerPhone

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
//Notes: This funiction will be removed when VideoPhone.c is using 
//uisnd_is_device_attached
         
boolean GetSpeakerPhone(void)
{
#ifdef FEATURE_SPEAKER_PHONE
  return  uisnd_is_device_enabled(SND_DEVICE_SPEAKER_PHONE);
#else
  return FALSE;
#endif
}

/*===========================================================================
FUNCTION GetLCDValue

DESCRIPTION
    Return the current LCD timer

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetLCDValue(CCoreApp *pMe)
{
  uint32 display_duration = 0;

  PRINT_FUNCTION_NAME();

  if(pMe == NULL || pMe->m_pIConfig == NULL ){
    CORE_ERR("Null pointer passed to GetLCDValue %x", pMe, 0, 0);
    return (uint64) 0;
  }
  if(! CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_LCD_TIMER,
                                            &display_duration, sizeof(uint32))))
  {
    CORE_ERR("Could not get CFGI_LCD_TIMER", 0, 0, 0);
  }

  if (!RangeCheckLCDValue(display_duration))
  {
    CORE_ERR("Display duration out of range %d", display_duration, 0, 0);
    display_duration = 0;        /* Set the default as Always On. */
    (void) SetLCDValue(pMe, (uint64)display_duration);
  }

  pMe->m_nLcdTimer = display_duration;
  return (uint64)(pMe->m_nLcdTimer);
}

/*===========================================================================
FUNCTION SetLCDValue

DESCRIPTION
    Set the LCD timer

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetLCDValue(CCoreApp *pMe, uint64 nNewVal)
{
  boolean lcdOff;
  uint32 display_duration;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if (!RangeCheckLCDValue(nNewVal))
  {
    nNewVal = 0;        /* Set the default as Always On. */
  }

  pMe->m_nLcdTimer =(uint32)nNewVal;
  CoreTask_SetLCDTimer(pMe->m_nLcdTimer );     /* Set the timer. */

  display_duration = pMe->m_nLcdTimer;
  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_LCD_TIMER,
                          &display_duration, sizeof(uint32))) != TRUE)
  {
    CORE_ERR("Could not set CFGI_LCD_TIMER.", 0, 0, 0);
  }

  lcdOff = (pMe->m_nLcdTimer != 0)?TRUE:FALSE;

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_LCD ,
      &lcdOff, sizeof(boolean))) != TRUE) {
     return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION SetSelOperatorDisplay

DESCRIPTION
    The "Select Operator" Dialog can now be displayed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetSelOperatorDisplay(CCoreApp *pMe)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   uint32 currentSel;
   IDialog* pIDialog;

   if(pMe == NULL || pMe->a.m_pIShell == NULL) {
     CORE_ERR("Null Pointer in SetSelOperatorDisplay",0,0,0);
     return;
   }

   pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

   currentSel = GetNetworkSelect(pMe);

   if(currentSel == AEECM_NETWORK_SEL_MODE_PREF_MANUAL) {
     if((pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_LIMITED ||
         pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_LIMITED_REGIONAL ||
         pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_NO_SRV)
         && pIDialog && (IDIALOG_GetID(pIDialog) == IDD_CORE) ) {
       SearchManualPLMNList(pMe);
     }

   }
#endif

}

#if defined(FEATURE_SECONDARY_DISPLAY)
/*===========================================================================
FUNCTION ToggleDisplays

DESCRIPTION
  Toggles between primary and secondary LCDs.  Based on the flip
  position, it turns one display on and the other off.

PROTOTYPE
  void ToggleDisplays(CCoreApp *pMe, boolean bIsFlipOpen)

PARAMETERS
  CCoreApp *pMe:        Pointer to the Core App structure
  boolean bIsFlipOpen:  Indicates whether the flip is open or closed

DEPENDENCIES
  None

RETURN VALUE
  boolean:   TRUE if the displays could be set correctly; FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ToggleDisplays(CCoreApp *pMe, boolean bIsFlipOpen)
{

  AEERect        DirtyRegion;
  AEEBitmapInfo  DirtyBMPInfo;
  int            nReturnCode;
  boolean        bRetSuccess = TRUE;
  ILogicalDisplayCfg *pLogicalDisplayCfg;

  if ((NULL == pMe) || (NULL == pMe->m_pIDisp2) ||
      (NULL == pMe->m_pIBitmap) || (NULL == pMe->m_pIBitmap2)) {
    return FALSE;
  }

  /* If the flip just closed, turn off the primary and turn on the secondary.
   * If it opened, turn off the secondary and turn on the primary.
   * Invalidate the entire bitmap for the display being turned on, so that
   * we are assured a full screen refresh.
   */
  if (! bIsFlipOpen) {
    /* Flip is closed */
    nReturnCode = IBITMAP_GetInfo(pMe->m_pIBitmap2, &DirtyBMPInfo,
                                  sizeof(AEEBitmapInfo));
    if (SUCCESS != nReturnCode) {

      MSG_ERROR("Unable to get bmp2 info for toggle.", 0, 0, 0);
      bRetSuccess = FALSE;
    }

    // Attempt to toggle the displays, even if we couldn't get the bitmap info
    /*Create an instance of LogicalDisplayCfg */
    if(SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                        (void **)&pLogicalDisplayCfg)) {
       physicalDisplayStateType physState;
       AEECLSID                 logDisplay;

       logDisplay = ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(pLogicalDisplayCfg,
                                                               AEECLSID_PHYSICALDISPLAY1);
       if (logDisplay != 0) {
         /* If the physical display is mapped to a logical display, unmap it. */
         physState.physicalDisplay = AEECLSID_PHYSICALDISPLAY1;
         physState.state = 0;

         //Unlink the primary display from all logical displays
         (void) ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDisplayCfg,
                                                        logDisplay, 1, &physState);
       }

       //ReLink the secondary display
       physState.physicalDisplay = AEECLSID_PHYSICALDISPLAY2;
       physState.state = 1;
       (void) ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDisplayCfg,
                                              AEECLSID_DISPLAY2, 1, &physState);

       (void) ILOGICALDISPLAYCFG_Release(pLogicalDisplayCfg);
    }

    if (bRetSuccess) {

      // Redraw the Annunciator
      if (IANNUNCIATOR_Redraw(pMe->m_pIAnn) != SUCCESS) {
        MSG_ERROR("Unable to update annunciator on toggle to secondary.",
                  0, 0, 0);
      }

      /* The entire bitmap is invalidated to ensure that the entire
       * contents of the bitmap will be refreshed on the screen that
       * was just turned on.
       */
      DirtyRegion.x = 0;
      DirtyRegion.y = 0;
      DirtyRegion.dx = (int16)DirtyBMPInfo.cx;
      DirtyRegion.dy = (int16)DirtyBMPInfo.cy;
      IBITMAP_Invalidate(pMe->m_pIBitmap2, &DirtyRegion);
      IDISPLAY_Update(pMe->m_pIDisp2);
    }
  }
  else
  {
    /* Flip is open */
    nReturnCode = IBITMAP_GetInfo(pMe->m_pIBitmap, &DirtyBMPInfo,
                                  sizeof(AEEBitmapInfo));
    if (SUCCESS != nReturnCode) {
      MSG_ERROR("Unable to get bmp info for toggle.", 0, 0, 0);
      bRetSuccess = FALSE;
    }

    // Attempt to toggle the displays, even if we couldn't get the bitmap info
    /*Create an instance of LogicalDisplayCfg */
    if(SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                        (void **)&pLogicalDisplayCfg))
    {
       physicalDisplayStateType physState;
       AEECLSID                 logDisplay;

       logDisplay = ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(pLogicalDisplayCfg,
                                                               AEECLSID_PHYSICALDISPLAY2);
       if (logDisplay != 0) {
         /* If the physical display is mapped to a logical display, unmap it. */
         physState.physicalDisplay = AEECLSID_PHYSICALDISPLAY2;
         physState.state = 0;

         //Unlink the primary display from all logical displays
         (void) ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDisplayCfg,
                                                        logDisplay, 1, &physState);
       }

       //ReLink the primary display
       physState.physicalDisplay = AEECLSID_PHYSICALDISPLAY1;
       physState.state = 1;
       (void) ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDisplayCfg,
                                              AEECLSID_DISPLAY1, 1, &physState);

       (void) ILOGICALDISPLAYCFG_Release(pLogicalDisplayCfg);
    }

    if (bRetSuccess) {

      // Redraw the Annunciator
      if (IANNUNCIATOR_Redraw(pMe->m_pIAnn) != SUCCESS) {
        MSG_ERROR("Unable to update annunciator on toggle to primary.",
                  0, 0, 0);
      }

      /* The entire bitmap is invalidated to ensure that the entire
       * contents of the bitmap will be refreshed on the screen that
       * was just turned on.
       */
      DirtyRegion.x = 0;
      DirtyRegion.y = 0;
      DirtyRegion.dx = (int16)DirtyBMPInfo.cx;
      DirtyRegion.dy = (int16)DirtyBMPInfo.cy;
      IBITMAP_Invalidate(pMe->m_pIBitmap, &DirtyRegion);

      IDISPLAY_Update(pMe->a.m_pIDisplay);
    }
  }

  UpdateBacklight(pMe);

  return bRetSuccess;

}
#endif // defined(FEATURE_SECONDARY_DISPLAY)


/*===========================================================================
FUNCTION GetALSLineSelected

DESCRIPTION
  Initialize ALS Line Selection menu

DEPENDENCIES
  Various Get and Set method for the variables in CoreUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetALSLineSelected(CCoreApp *pMe)
{
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return 0;
  }

//lint -save -e571 suspicious cast (casting enum to uint64)
  return (uint64)(pMe->m_phInfo.m_alsLine);
//lint +e571
}


/*===========================================================================
FUNCTION SetALSLineSelected

DESCRIPTION

  Change ALS Line selection setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetALSLineSelected(CCoreApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  /* Check if valid before sending it to CM */

  CoreUtil_StatusCb.pfnCancel = NULL;
  CoreUtil_StatusCb.pfnNotify = CoreUtil_SystemPrefErrCb;
  CoreUtil_StatusCb.pNotifyData = pMe;
  /* Send CM command */
  nReturnStatus = ICM_SelectLine(pMe->m_pICM,
                    (AEECMAlsLine) nNewSetting,
                    &status, &CoreUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }

}

/*===========================================================================
FUNCTION GetALSLineSwitch

DESCRIPTION
  Initialize ALS Line Switching menu

DEPENDENCIES
  Various Get and Set method for the variables in CoreUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetALSLineSwitch(CCoreApp *pMe)
{
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return 0;
  }
//lint -save -e571 suspicious cast (casting enum to uint64)
  return (uint64)(pMe->m_phInfo.m_alsLineSwitch);
// lint +e571 suspicious cast (casting enum to uint64)
}


/*===========================================================================
FUNCTION SetALSLineSwitch

DESCRIPTION

  Change ALS Line switching setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetALSLineSwitch(CCoreApp *pMe, uint64 nNewSetting)
{
  int nReturnStatus;
  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  /* Check if valid before sending it to CM */
  CoreUtil_StatusCb.pfnCancel = NULL;
  CoreUtil_StatusCb.pfnNotify = CoreUtil_SystemPrefErrCb;
  CoreUtil_StatusCb.pNotifyData = pMe;
  /* Send CM command */
  nReturnStatus = ICM_SetLineSwitching(pMe->m_pICM,
                    (AEECMAlsLineSwitching) nNewSetting,
                    &status, &CoreUtil_StatusCb /* error cb */
                    );
  if (nReturnStatus == SUCCESS) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION GetContrast

DESCRIPTION
  Obtain Contrast information from NV

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None

RETURN VALUE
  Contrast value stored in NV, or 0xffff if failed
===========================================================================*/
uint16 GetContrast(CCoreApp * pMe)
{
  byte contrast;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return 0xffff;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CONTRAST_LVL ,
      &contrast, sizeof(contrast))) != TRUE) {
      CORE_ERR("Unable to get LCD Value from IConfig", 0, 0, 0);
      return 0xffff;
  }

  return contrast;
} /* End GetContrast */

/*===========================================================================
FUNCTION SetContrast

DESCRIPTION
  Set the contrast in NV

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetContrast(CCoreApp * pMe, byte nNewVal)
{
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CONTRAST_LVL ,
      &nNewVal, sizeof(nNewVal))) != TRUE) {
     return FALSE;
  }
  return TRUE;
} /* End SetContrast */


/*===========================================================================
FUNCTION RestoreStoredContrast

DESCRIPTION
  This function changes the screen contrast to the value in NVRam

DEPENDENCIES
  None

SIDE EFFECTS
  Screen contrast changes
===========================================================================*/
void RestoreStoredContrast(CCoreApp * pMe)
{

  disp_info_type di;

  if (pMe == NULL) {
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return;
  }

  di = disp_get_info();
  pMe->m_byContrastMin = CONTRAST_MIN;
  pMe->m_byContrastMax = CONTRAST_MAX;
  pMe->m_byContrastDefault = di.contrast_default;

  //This is the case if no one filled in min/max contrast for this platform.
  //Use driver defaults
  if ((pMe->m_byContrastMin == CONTRAST_UNDEFINED) &&
      (pMe->m_byContrastMax == CONTRAST_UNDEFINED)) {
    pMe->m_byContrastMin = di.contrast_min;
    pMe->m_byContrastMax = di.contrast_max;
  }
  //read contrast from NV
  pMe->m_byDispContrast = GetContrast(pMe);

  //the only reason for contrast to be out of range here is
  //if it's corrupted in NV.  So, use the default.
  if(pMe->m_byDispContrast > pMe->m_byContrastMax) {
    pMe->m_byDispContrast = pMe->m_byContrastDefault;
  }

  if(pMe->m_byDispContrast < pMe->m_byContrastMin) {
    pMe->m_byDispContrast = pMe->m_byContrastDefault;
  }

  if (!SetContrast(pMe, pMe->m_byDispContrast)) {
    MSG_ERROR("Unable to set contrast",0,0,0);
  }

  disp_set_contrast(pMe->m_byDispContrast);
}

/*===========================================================================
FUNCTION IsHexWString

DESCRIPTION
  This function determines whether or not a given string contains only
  characters that represent hexadecimal numbers.  This routine assumes
  that the input is a correctly-formed, NULL-terminated string.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IsHexWString(AECHAR *pszwHexString)
{
  if (! pszwHexString) {
    CORE_ERR("Null string", 0, 0, 0);
    return FALSE;
  }

  while (*pszwHexString)
  {
    if (((*pszwHexString >= '0') && (*pszwHexString <= '9')) ||
        ((*pszwHexString >= 'A') && (*pszwHexString <= 'F')) ||
        ((*pszwHexString >= 'a') && (*pszwHexString <= 'f')))
    {
      pszwHexString++;
      continue;
    }
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION ConvertWStrToBinBuf

DESCRIPTION
  Converts an ASCII wide string of hexadecimal characters to a buffer of
  binary data.  Note that, since each character translates to 4 bits, the
  input character buffer must be of even length for the translation to work.
  e.g. Converts the wide string "1A2B" into two octets with values 0x1A 0x2B.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  Number of bytes populated in pBuf if successful.  (If the buffer provided
    is larger than necessary, only the returned number of bytes will be valid.)
  -1 otherwise.

COMMENTS
  This routine expects hexadecimal characters only, but it does perform
  all necessary error checking.  Upper- and lower-case hex values are
  both accepted.
===========================================================================*/
int ConvertWStrToBinBuf(const AECHAR *pszwIn, uint8 *pBuf, uint32 nBufLen)
{
  int       nBytes = 0;
  int       nStrLen;
  int       i;
  AECHAR    cToConvert;
  uint8     nValue;

  if ((! pszwIn) || (! pBuf) || (nBufLen == 0))
  {
    CORE_MSG("Invalid input, %x %x %d", pszwIn, pBuf, nBufLen);
    return -1;
  }

  nStrLen = (int)WSTRLEN(pszwIn);
  if ((nStrLen & 0x00000001) != 0)
  {
    CORE_MSG("Input string has odd length, %d", nStrLen, 0, 0);
    return -1;
  }
  else if ((uint32)nStrLen >> 1 > nBufLen)
  {
    /* Buffer must be at least half the string length */
    CORE_MSG("Buffer too short for string %d %d", nBufLen, nStrLen, 0);
    return -1;
  }

  while (nStrLen != 0)
  {
    /* We already know that the output buffer is large enough, so we
     * just run until the input string is processed.  Consume two input
     * characters at at time, converting them into a single binary byte.
     */

    nValue = 0;
    for (i = 0; i < 2; i++)
    {
      /* Convert the character to binary value */
      cToConvert = *pszwIn++;
      if ((cToConvert >= (AECHAR)'0') && (cToConvert <= (AECHAR)'9'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'0');
      }
      else if ((cToConvert >= (AECHAR)'A') && (cToConvert <= (AECHAR)'F'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'A' + 0xA);
      }
      else if ((cToConvert >= (AECHAR)'a') && (cToConvert <= (AECHAR)'f'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'a' + 0xA);
      }
      else
      {
        CORE_MSG("Input string has invalid character, %d", (uint16)cToConvert,
                 0, 0);
        return -1;
      }

      /* For the first of the two bytes, shift it over to the upper nibble,
       * to make room for the other nibble.
       */
      if (i == 0)
      {
        nValue = (uint8)(nValue << 4);
      }
    }

    /* Copy the converted value into the appropriate byte */
    *pBuf = nValue;
    pBuf++;
    nBytes++;
    nStrLen = (int)WSTRLEN(pszwIn);
  }
  return nBytes;
}

/*===========================================================================
FUNCTION GetCCBSSetting

DESCRIPTION
  Obtain CCBS information from NV

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None

RETURN VALUE
  CCBS value stored in NV
===========================================================================*/
uint64 GetCCBSSetting(CCoreApp * pMe)
{
  OEMCCBS ccbs;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return OEM_CCBS_NONE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return OEM_CCBS_NONE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_CCBS ,
      &ccbs, sizeof(OEMCCBS))) != TRUE)
  {
      return OEM_CCBS_NONE;
  }

  return ccbs;
} /* End GetCCBSSetting */

/*===========================================================================
FUNCTION SetCCBSSetting

DESCRIPTION
  Set the ccbs in NV

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetCCBSSetting(CCoreApp * pMe, uint64 nNewVal)
{
  OEMCCBS ccbs = (OEMCCBS)nNewVal;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pMe pointer", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig, CFGI_CCBS ,
      &ccbs, sizeof(OEMCCBS))) != TRUE)
  {
     return FALSE;
  }
  return TRUE;
} /* End SetCCBSSetting */

/*=============================================================================
FUNCTION: IsSubscriptionAvail

DESCRIPTION:
  Check if the passed in technology subscription information can be retrieved
  from the card

SIDE EFFECTS:
  None.

=============================================================================*/
boolean IsSubscriptionAvail(CCoreApp *pMe, uint8 wTechnology) {
  uint32 dwSlot1OperMask = 0;
  uint32 dwSlot2OperMask = 0;
  uint32 dwSlot1TechMask = 0;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint32 dwSlot2TechMask = 0;
#endif
  boolean bSubAvail = TRUE;

  switch (wTechnology) {
    case COREAPP_GSM:
      dwSlot1OperMask = COREAPP_SLOT1_GSM_OPERATIONAL_MASK;
      dwSlot2OperMask = COREAPP_SLOT2_GSM_OPERATIONAL_MASK;
      dwSlot1TechMask = COREAPP_SLOT1_APP_GSM_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      dwSlot2TechMask = COREAPP_SLOT2_APP_GSM_MASK;
#endif
      break;
    case COREAPP_UMTS:
      dwSlot1OperMask = COREAPP_SLOT1_UMTS_OPERATIONAL_MASK;
      dwSlot2OperMask = COREAPP_SLOT2_UMTS_OPERATIONAL_MASK;
      dwSlot1TechMask = COREAPP_SLOT1_APP_USIM_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      dwSlot2TechMask = COREAPP_SLOT2_APP_USIM_MASK;
#endif
      break;
    case COREAPP_CDMA:
      dwSlot1OperMask = COREAPP_SLOT1_CDMA_OPERATIONAL_MASK;
      dwSlot2OperMask = COREAPP_SLOT2_CDMA_OPERATIONAL_MASK;
      dwSlot1TechMask = COREAPP_SLOT1_APP_RUIM_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      dwSlot2TechMask = COREAPP_SLOT2_APP_RUIM_MASK;
#endif
      break;

    default:
      CORE_ERR("Unknown technology, %d", wTechnology, 0, 0);
      return FALSE;
  }
  if ((pMe->m_dwSecurityMask & dwSlot1OperMask) == dwSlot1OperMask) {
    /* the technology is in slot 1 */
    /* is the pin perm disabled? or blocked */
    if ((pMe->m_sPIN1_lock_state == AEECARD_PIN_PERM_DISABLED) ||
        (pMe->m_sPIN1_lock_state == AEECARD_PIN_BLOCKED))
    {
      bSubAvail = FALSE;
    }
    else {
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_PRESENT_MASK) != COREAPP_SLOT1_PRESENT_MASK) {
        bSubAvail = FALSE;
      }
      else {
        if ((pMe->m_dwSecurityMask & dwSlot1TechMask) != dwSlot1TechMask) {
          bSubAvail = FALSE;
        }
        /* check if the card status is valid*/
        if ((pMe->m_nCardStatus == AEECARD_NO_CARD) ||
            (pMe->m_nCardStatus == AEECARD_INVALID_CARD) ||
            (pMe->m_nCardStatus == AEECARD_NOT_READY)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            || (pMe->m_nCardStatus == AEECARD_AVAIL_CARD)
#endif
#if defined(FEATURE_GSTK) || defined(FEATURE_MMGSDI_DUAL_SLOT)
            || (pMe->m_nCardStatus == AEECARD_CARD_REINIT)
            || (pMe->m_nCardStatus == AEECARD_NOT_READY)
#endif
            )
        {
          bSubAvail = FALSE;
        }
        /* check if the PIN status is valid */
        if ( !CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1)) {
          bSubAvail = FALSE;
        }

        if (bSubAvail)
        {
          bSubAvail = FALSE;
          /* check if card is valid */
          if ( pMe->m_nCardStatus == AEECARD_VALID_CARD )
          {
            bSubAvail = TRUE;
          }
        }
      }
    } /* pin is not permanently disabled */
  }
  else if ((pMe->m_dwSecurityMask & dwSlot2OperMask) == dwSlot2OperMask) {
    /* the technology is in slot 2 */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /* is the pin perm disabled? */
    if ((pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_PERM_DISABLED) ||
        (pMe->m_sPIN1_slot2_lock_state == AEECARD_PIN_BLOCKED))
    {
      bSubAvail = FALSE;
    }
    else {
      if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_PRESENT_MASK) != COREAPP_SLOT2_PRESENT_MASK) {
        bSubAvail = FALSE;
      }
      else {
        if ((pMe->m_dwSecurityMask & dwSlot2TechMask) != dwSlot2TechMask) {
          bSubAvail = FALSE;
        }
        /* check if the card status is valid */
        if ((pMe->m_nCardStatus_slot2 == AEECARD_NO_CARD) ||
          (pMe->m_nCardStatus_slot2 == AEECARD_INVALID_CARD) ||
          (pMe->m_nCardStatus_slot2 == AEECARD_NOT_READY) ||
          (pMe->m_nCardStatus_slot2 == AEECARD_AVAIL_CARD)
#if defined(FEATURE_GSTK) || defined(FEATURE_MMGSDI_DUAL_SLOT)
          || (pMe->m_nCardStatus_slot2 == AEECARD_CARD_REINIT)
          || (pMe->m_nCardStatus_slot2 == AEECARD_NOT_READY)
#endif
          )
        {
          bSubAvail = FALSE;
        }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        /* Check if pin status is valid */
        if ( !CoreSecurity_IsPinVerified(pMe, AEECARD_PIN1_SLOT2)) {
          bSubAvail = FALSE;
        }

        if (bSubAvail)
        {
          bSubAvail = FALSE;
          /* check if card is valid */
          if ( pMe->m_nCardStatus_slot2 == AEECARD_VALID_CARD )
          {
            bSubAvail = TRUE;
          }
        }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      }
    }
#else /* FEATURE_MMGSDI_DUAL_SLOT */
    bSubAvail = FALSE;
#endif
  }
  else {
    /* techonology not available */
    bSubAvail = FALSE;
  }

  return bSubAvail;
}


#ifdef FEATURE_USRSYSCLOCK
/*===========================================================================
FUNCTION GETNETWORKTIME

DESCRIPTION
  Get the NV setting for network time enable/disable.

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNetworkTime(CCoreApp * pMe)
{
  boolean EnableNwTime;
  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_AUTO_TIME_ENABLE ,
      &EnableNwTime, sizeof(EnableNwTime))) != TRUE)
  {
   (void) SetNetworkTime(pMe, (uint64)TRUE);
    return (uint64)pMe->m_bNetworkTime;
  }

  pMe->m_bNetworkTime = EnableNwTime;

  return (uint64)pMe->m_bNetworkTime;
} /*End GetNetworkTime*/


/*===========================================================================
FUNCTION SetNetworkTime

DESCRIPTION
  Write network time (on/off) to NV, update bNetworkTime

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNetworkTime(CCoreApp * pMe, uint64 nNewVal)
{
  boolean EnableNWTime = pMe->m_bNetworkTime;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  pMe->m_bNetworkTime = (boolean)nNewVal;

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig,  CFGI_AUTO_TIME_ENABLE ,
      &pMe->m_bNetworkTime, sizeof(boolean))) != TRUE)
  {
    CORE_ERR("Could not Set the Item",0,0,0);
    pMe->m_bNetworkTime = EnableNWTime;
    return FALSE;
  }

  return TRUE;

} /*End SetNetworkTime */

/*===========================================================================
FUNCTION GetDeleteNITZPLMN

DESCRIPTION
  Dummy Get Function to delete the NITZ PLMN info.

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDeleteNITZPLMN(CCoreApp * pMe)
{

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return (uint64)FALSE;
  }

  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return (uint64)FALSE;
  }

  return (uint64)TRUE;
} /*End GetDeleteNITZPLMN*/


/*===========================================================================
FUNCTION SetDeleteNITZPLMN

DESCRIPTION
  Delete the NITZ PLMN info in NV

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDeleteNITZPLMN(CCoreApp * pMe, uint64 nNewVal)
{
  AECHAR plmn_name[NV_MAX_NW_NAME_CHARS + 1];

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  if(pMe->m_pIConfig == NULL) {
    CORE_ERR("Unable to get Config ptr", 0, 0, 0);
    return FALSE;
  }

  if(( (boolean)nNewVal) == TRUE) {

    (void )MEMSET(plmn_name, ' ', sizeof(plmn_name));

    if(CheckAEEReturnStatus(ICONFIG_SetItem(pMe->m_pIConfig,  CFGI_NITZ_NW_INFO ,
       plmn_name, sizeof(plmn_name))) != TRUE)
    {
      CORE_ERR("Could not Set the Item",0,0,0);
      return FALSE;
    }
  }
  return TRUE;

} /*End SetNetworkTime */

#endif //FEATURE_USRSYSCLOCK

//lint -save -e818  Suppress "pMe could be const"
/*===========================================================================
FUNCTION SetDisableBgImage

DESCRIPTION
  Used to set whether or not background images should be disabled

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDisableBgImage(CCoreApp * pMe, uint64 nNewVal)
{
  IConfig *pIConfig = NULL;
  boolean  bNewValue = (boolean)nNewVal;
  int      nRetVal;

  if (!pMe)
  {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  // Save the new value
  nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void **)&pIConfig);
  if ((nRetVal == SUCCESS) && pIConfig) {
    (void)ICONFIG_SetItem(pIConfig, CFGI_DISABLE_BG_IMAGE, &bNewValue,
                          sizeof(bNewValue));
    (void)ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig, %d 0x%x", nRetVal, pIConfig, 0);
  }

#if defined(FEATURE_ENHANCED_OVERLAY) || defined(FEATURE_MMOVERLAY)
  // Turn the background image on or off
  if (bNewValue == TRUE)
  {
    CoreApp_ToggleBgImage(pMe, FALSE);
  }
  else
  {
    CoreApp_ToggleBgImage(pMe, TRUE);
  }
#endif /* FEATURE_ENHANCED_OVERLAY || FEATURE_MMOVERLAY */

  return TRUE;

}

/*===========================================================================
FUNCTION GetDisableBgImage

DESCRIPTION
  Used to get whether or not background images should be disabled

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDisableBgImage(CCoreApp *pMe)
{
  boolean  bDisableBgImage = FALSE;
  IConfig *pIConfig = NULL;
  int      nRetVal;

  if (! pMe)
  {
    return (uint64)FALSE;
  }

  nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void **)&pIConfig);
  if ((nRetVal == SUCCESS) && pIConfig) {
    (void)ICONFIG_GetItem(pIConfig, CFGI_DISABLE_BG_IMAGE, &bDisableBgImage,
                          sizeof(bDisableBgImage));
    (void)ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }

  return (uint64)bDisableBgImage;
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
/*===========================================================================
FUNCTION SetLanguageSelection

DESCRIPTION
  Used to set the current language selection.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetLanguageSelection(CCoreApp *pMe, uint64 nNewVal)
{
  byte     nOldValue;
  byte     nNewValue = (uint8)nNewVal;
  boolean  bValueChanged = TRUE;          /* Assume the value did change. */
  IConfig *pIConfig = NULL;
  int      nRetVal;

  if (!pMe) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  // Save the new value
  nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void **)&pIConfig);
  if ((nRetVal == SUCCESS) && pIConfig) {

    /* First, we need to get the current setting, so that we know if the
     * value has actually changed.  We need to notify ISTK exactly when
     * the actual setting has changed.
     */
    nRetVal = ICONFIG_GetItem(pIConfig, CFGI_LANGUAGE_SELECTION, &nOldValue,
                              sizeof(nOldValue));
    if (nRetVal != SUCCESS) {
      /* We obtain the current value.  We'll log an error and just
       * keep the assumption that the value actually changed (i.e.
       * don't change the value of bValueChanged).
       */
      CORE_MSG("Failed to get lang selection, %d", nRetVal, 0, 0);
    }
    else if (nOldValue == nNewValue) {
      bValueChanged = FALSE;
    }

    if (bValueChanged == TRUE) {
      nRetVal = ICONFIG_SetItem(pIConfig, CFGI_LANGUAGE_SELECTION, &nNewValue,
                                sizeof(nNewValue));
      if (nRetVal != SUCCESS) {
        CORE_MSG("Failed to set lang selection, %d", nRetVal, 0, 0);
      }
#ifdef FEATURE_GSTK
      else {
        AEESTKLangSelectionResponse  sLangSel;
        uint8 nLangSel[COREAPP_ISTK_LENG_SEL_LEN];

        /* Language selection strings for ISTK come from ISO 639. */
        switch (nNewValue) {
          case COREAPP_LANG_SEL_GER:
            nLangSel[0] = (uint8)'d';
            nLangSel[1] = (uint8)'e';
            break;

          case COREAPP_LANG_SEL_ENG:
          default:
            nLangSel[0] = (uint8)'e';
            nLangSel[1] = (uint8)'n';
            break;
        }

        sLangSel.len = COREAPP_ISTK_LENG_SEL_LEN;
        sLangSel.sLangCode = nLangSel;
        nRetVal = ISTK_LangSelection(pMe->m_pISTK, &sLangSel);
        if (nRetVal != SUCCESS) {
          CORE_MSG("Failed ISTK notification, %d", nRetVal, 0, 0);
        }
      }
#endif /* FEATURE_GSTK */
    }

    (void)ICONFIG_Release(pIConfig);
  } else {
    CORE_MSG("Cant create IConfig, %d 0x%x", nRetVal, pIConfig, 0);
  }

  return TRUE;
}

/*===========================================================================
FUNCTION GetLanguageSelection

DESCRIPTION
  Used to get the current language selection.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetLanguageSelection(CCoreApp *pMe)
{
  uint8    nLangSelection = COREAPP_LANG_SEL_ENG;
  IConfig *pIConfig = NULL;
  int      nRetVal;

  if (! pMe) {
    return (uint64)FALSE;
  }

  nRetVal = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                                  (void **)&pIConfig);
  if ((nRetVal == SUCCESS) && pIConfig) {
    (void)ICONFIG_GetItem(pIConfig, CFGI_LANGUAGE_SELECTION, &nLangSelection,
                          sizeof(nLangSelection));
    (void)ICONFIG_Release(pIConfig);

    switch (nLangSelection) {
      case COREAPP_LANG_SEL_GER:
      case COREAPP_LANG_SEL_ENG:
        break;

      default:
        nLangSelection = COREAPP_LANG_SEL_ENG;
        break;
    }
  } else {
    CORE_MSG("Cant create IConfig", 0, 0, 0);
  }

  return (uint64)nLangSelection;
}
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
//lint -restore

#if defined(FEATURE_DISPLAY_CONTENT_MAP)
/*===========================================================================
FUNCTION GetDisplayNMapping

DESCRIPTION
  Returns the logical display to which a given physical display is mapped.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  COREAPP_LOGICAL_DISP_PRI  - The requested physical display is mapped to the
                              primary logical display.
  COREAPP_LOGICAL_DISP_SEC  - The requested physical display is mapped to the
                              secondary logical display.
  COREAPP_LOGICAL_DISP_NONE - The requested physical is not mapped to any
                              logical display, or it is mapped to an
                              unrecognized logical display.
===========================================================================*/
static uint64 GetDisplayNMapping(CCoreApp * pMe, AEECLSID phys_display)
{
  ILogicalDisplayCfg *pLogicalDispCfg;
  AEECLSID display_class;
  uint64 ret = COREAPP_LOGICAL_DISP_NONE;

  if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                         (void **) &pLogicalDispCfg)) {
    CORE_ERR("Could not create ILogicalDisplayConfig %x %x", pMe, phys_display, 0);
    return ret;
  }

  /* look for display in primary list */
  display_class = ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(pLogicalDispCfg, phys_display);

  switch(display_class)
    {
     case AEECLSID_DISPLAY1:
      ret = COREAPP_LOGICAL_DISP_PRI;
      break;

     case AEECLSID_DISPLAY2:
        ret = COREAPP_LOGICAL_DISP_SEC;
        break;

     default:
        ret = COREAPP_LOGICAL_DISP_NONE;
        break;
      }

  ILOGICALDISPLAYCFG_Release(pLogicalDispCfg);

  return ret;
}

/*===========================================================================
FUNCTION SetDisplayNMapping

DESCRIPTION
  Maps a physical display to a logical display.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  TRUE  - The mapping was successfully established.
  FALSE - The mapping was not established.
===========================================================================*/
static boolean SetDisplayNMapping(CCoreApp * pMe, uint64 nNewVal, AEECLSID phys_display)
{
  ILogicalDisplayCfg *pLogicalDispCfg;
  physicalDisplayStateType dispState;
  boolean                  bRetVal = FALSE;

  if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                         (void **) &pLogicalDispCfg)) {
    CORE_ERR("Could not create ILogicalDisplayConfig %x %d %x", pMe, nNewVal, phys_display);
    return bRetVal;
  }

  switch(nNewVal)
  {
  case COREAPP_LOGICAL_DISP_NONE:
    dispState.physicalDisplay = phys_display;
    dispState.state = 0;
    ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                            AEECLSID_DISPLAY1,
                                            1,
                                            &dispState);
    ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                            AEECLSID_DISPLAY2,
                                            1,
                                            &dispState);
    bRetVal = TRUE;
    break;
  case COREAPP_LOGICAL_DISP_PRI:
    dispState.physicalDisplay = phys_display;
    dispState.state = 1;
    ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                            AEECLSID_DISPLAY1,
                                            1,
                                            &dispState);
    bRetVal = TRUE;
    break;

  case COREAPP_LOGICAL_DISP_SEC:
    dispState.physicalDisplay = phys_display;
    dispState.state = 1;
    ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                            AEECLSID_DISPLAY2,
                                            1,
                                            &dispState);
    bRetVal = TRUE;
    break;

  default:
    CORE_ERR("Setting display %x mapping to unknown value %d", phys_display, nNewVal, 0);
    break;

  }

  /* TBD Enforce a rule that at least the primary display content is present
   * on at least on physical display.
   */

  ILOGICALDISPLAYCFG_Release(pLogicalDispCfg);

  return bRetVal;
}


/*===========================================================================
FUNCTION GetDisplay1Mapping

DESCRIPTION
  Return which logical display physical display 1 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  Display mapping
===========================================================================*/
uint64 GetDisplay1Mapping(CCoreApp * pMe)
{
  return GetDisplayNMapping(pMe, AEECLSID_PHYSICALDISPLAY1);
}

/*===========================================================================
FUNCTION SetDisplay1Mapping

DESCRIPTION
  Set which logical display physical display 1 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetDisplay1Mapping(CCoreApp * pMe, uint64 nNewVal)
{
  return SetDisplayNMapping(pMe, nNewVal, AEECLSID_PHYSICALDISPLAY1);
}

/*===========================================================================
FUNCTION GetDisplay2Mapping

DESCRIPTION
  Return which logical display physical display 2 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  Display mapping
===========================================================================*/
uint64 GetDisplay2Mapping(CCoreApp * pMe)
{
  return GetDisplayNMapping(pMe, AEECLSID_PHYSICALDISPLAY2);
}

/*===========================================================================
FUNCTION SetDisplay2Mapping

DESCRIPTION
  Set which logical display physical display 2 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetDisplay2Mapping(CCoreApp * pMe, uint64 nNewVal)
{
  return SetDisplayNMapping(pMe, nNewVal, AEECLSID_PHYSICALDISPLAY2);
}

/*===========================================================================
FUNCTION GetDisplay3Mapping

DESCRIPTION
  Return which logical display physical display 3 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  Display mapping
===========================================================================*/
uint64 GetDisplay3Mapping(CCoreApp * pMe)
{
  return GetDisplayNMapping(pMe, AEECLSID_PHYSICALDISPLAY3);
}

/*===========================================================================
FUNCTION SetDisplay3Mapping

DESCRIPTION
  Set which logical display physical display 3 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetDisplay3Mapping(CCoreApp * pMe, uint64 nNewVal)
{
  return SetDisplayNMapping(pMe, nNewVal, AEECLSID_PHYSICALDISPLAY3);
}

/*===========================================================================
FUNCTION GetDisplay4Mapping

DESCRIPTION
  Return which logical display physical display 4 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  Display mapping
===========================================================================*/
uint64 GetDisplay4Mapping(CCoreApp * pMe)
{
  return GetDisplayNMapping(pMe, AEECLSID_PHYSICALDISPLAY4);
}

/*===========================================================================
FUNCTION SetDisplay4Mapping

DESCRIPTION
  Set which logical display physical display 4 is mapped to

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  TRUE if successful

===========================================================================*/
boolean SetDisplay4Mapping(CCoreApp * pMe, uint64 nNewVal)
{
  return SetDisplayNMapping(pMe, nNewVal, AEECLSID_PHYSICALDISPLAY4);
}
#endif /* defined(FEATURE_DISPLAY_CONTENT_MAP) */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION ShowManualPLMNList

DESCRIPTION
  Launch and handle the manual network select dialogs.

DEPENDENCIES
  InitMenuIcon, SetMenuIcon,ICM_GetNetworks,SetNetworkAutoSelection.

SIDE EFFECTS
  For Manual and Manual w/rat, callback is registered.
===========================================================================*/
boolean ShowManualPLMNList(CCoreApp *pMe, uint16 w)
{
  int nReturnStatus;

  PRINT_FUNCTION_NAME();

  pMe->m_nNWSelectionMode  = APPCOMFUNC_NW_SEL_INACTIVE;

  if ((w == APPCOMFUNC_NW_SEL_MAN_PLMN) ||
      (w == APPCOMFUNC_NW_SEL_MAN_PLMN_WRAT))
  {
    /* Free any previously allocated memory before allocating new one*/
    FREEIF(pMe->m_pDetailedPLMNList);
    FREEIF(pMe->m_pCopyDetailedPLMNList);
    pMe->m_pDetailedPLMNList = (AEECMDetailedPLMNList*)
      MALLOC(sizeof(AEECMDetailedPLMNList));

    pMe->m_pCopyDetailedPLMNList = (AEECMDetailedPLMNList*)
      MALLOC(sizeof(AEECMDetailedPLMNList));

    pMe->m_nCallback.pfnCancel = NULL;
    pMe->m_nCallback.pfnNotify = (PFNNOTIFY) CoreUtil_GetNetworksCb;
    pMe->m_nCallback.pNotifyData = (void*) pMe;

    pMe->m_nNWSelectionMode  = w;


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
      DisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
      return TRUE;
    }
    // These updates take a long time. Show the searching dialog
    else //Searching....
    {
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile, IDD_PLMN_SEARCHING, NULL);
      return CheckAEEReturnStatus(nReturnStatus);
    }
  }

  return FALSE;
}
//lint -restore
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

/*===========================================================================
FUNCTION GetSidetone

DESCRIPTION
  Gets the current state of the sidetone

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  snd_sidetone_mode_type enumeration that is current sidetone state.

===========================================================================*/
uint64 GetSidetone(CCoreApp *pMe)
{
  return (uint64) uisnd_get_sidetone();
}

/*===========================================================================
FUNCTION SetSidetone()

DESCRIPTION
  Sets the sidetone.

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE
  Always TRUE. The underlying sidetone functions do not return error codes.

===========================================================================*/
boolean SetSidetone(CCoreApp *pMe, uint64 newVal)
{
  uisnd_set_sidetone((snd_sidetone_mode_type)newVal);
  return TRUE;
}

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
/*===========================================================================
FUNCTION CoreUtils_GetPLMN

DESCRIPTION
  Gets PLMN name string.

DEPENDENCIES
  ui_netwk_get_network_info

SIDE EFFECTS
  ppszPLMN needs to be freed by the calling function.
===========================================================================*/
int CoreUtils_GetPLMN(CCoreApp *pMe, char **ppszPLMN, boolean bLookInSIM)
{
  AEECMPLMNID  plmn;
  uinetwk_network_info_s_type nwInfo = {0};
  char *buf;
  void *pResult;
  boolean nameAvailable = FALSE;
  boolean bGotUINetworkName = FALSE;
  int len;

  #ifdef FEATURE_CPHS_EONS
  char *encodedBuf=NULL ,*decodedBuf=NULL;
  uint32 decodedBufLen =0;
  uint8 slot= AEECARD_SLOT_NONE,recNum=0;
  AEECardApp pCardApp;
  AEECardServiceIndType svcInd;
  #endif
  #ifdef FEATURE_USRSYSCLOCK
  AECHAR wBuf[MAX_STR_SIZE+1];
  uint32 mcc = 0;
  uint32 mnc = 0;
  #endif

  if ((NULL == pMe) || (NULL == ppszPLMN) || (NULL == pMe->m_pICard))
    return EFAILED;

  //Init Value
  *ppszPLMN = NULL;

  buf = (char*) MALLOC(MAX_STR_SIZE*sizeof(char));
  if(buf == NULL)
  {
    return EFAILED;
  }

  //Get the current mcc and mnc values
  plmn.identity[0] = pMe->m_ssInfo.sys_id.id.plmn.identity[0];
  plmn.identity[1] = pMe->m_ssInfo.sys_id.id.plmn.identity[1];
  plmn.identity[2] = pMe->m_ssInfo.sys_id.id.plmn.identity[2];
  bGotUINetworkName = ui_netwk_get_network_info(*((sys_plmn_id_s_type*)&plmn), &nwInfo);

#ifdef FEATURE_CPHS_EONS
  if (!nameAvailable && bLookInSIM) {
    //Use the name from the SIM
    encodedBuf = (char*) MALLOC(COREAPP_MAX_NETWORK_LONGNAME_LEN*sizeof(char));
    if(encodedBuf == NULL)
    {
      FREEIF(buf);
      return EFAILED;
    }
    /* Look for the current serving system
    if current system is GSM read service#17 SST if UMTS read service #19 UST.
    set spn_service_available accordingly
    */

    if (ICARD_GetCardApps(pMe->m_pICard, &pCardApp) != SUCCESS)
    {
      CORE_ERR("EONS:Could not get Card Apps",0,0,0);
    }


    if (pCardApp.slot1_present || pCardApp.slot2_present)
    {
      if (ICARD_GetUserSlotPref(pMe->m_pICard, AEECARD_GSM_APP, &slot)!= SUCCESS)
      {
        CORE_ERR("EONS:Could not get GSM Card App",0,0,0);
      }

      if (AEECARD_SLOT_NONE == slot)
      {
        if (ICARD_GetUserSlotPref(pMe->m_pICard, AEECARD_USIM_APP, &slot)!= SUCCESS)
        {
          CORE_ERR("EONS:Could not get USIM Card App",0,0,0);
        }
      }
    } else
    {
      CORE_ERR("EONS:Both the Slots are empty",0,0,0);
    }

    if (SUCCESS == ICARD_GetServiceAvailableInd(pMe->m_pICard, slot,
                                                AEECARD_SPN,
                                                &svcInd))
    {
      if (TRUE == svcInd.bGSMSvcOn || TRUE == svcInd.bUMTSSvcOn)
      {
        nameAvailable = CoreApp_DisplaySPN(pMe, encodedBuf);
      } else
      {
        /* SPN Not available so check the OPL and PNN */
        nameAvailable = CoreApp_DisplayONS(pMe,encodedBuf,slot,&recNum);
      }
    }

    if (!nameAvailable)
    {
      /* SPN Not available so check the OPL and PNN */
      nameAvailable = CoreApp_DisplayONS(pMe,encodedBuf,slot,&recNum);
    }

    if (nameAvailable)
    {
      if (AEECM_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET
          == pMe->m_PNNInfo.m_pnnArray[recNum].codingScheme)
      {

        MSG_MED("EONS: Send Buffer For Decoding",0,0,0);

        //Call the Decoding Fucntions here with the encodedBuf
        decodedBufLen = ((pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen*8) - (pMe->m_PNNInfo.m_pnnArray[recNum].spareBitsLongName))/7;
        decodedBufLen = MIN(decodedBufLen,COREAPP_MAX_NETWORK_LONGNAME_LEN);
        decodedBuf = (char*) MALLOC((decodedBufLen+1)*sizeof(char)); // One for NULL char
        if (decodedBuf == NULL)
        {
          FREEIF(buf);
          FREEIF(encodedBuf);
          nameAvailable = FALSE;
        }
        /* Decode the packed name */
        AppCom_ConvertPacked7BitToGSMDefault((uint8*)encodedBuf,
                                             decodedBufLen,
                                             decodedBufLen,
                                             0,
                                             (uint8*)decodedBuf);
        /*  Convert the unpacked name into ASCII */
        MEMCPY(encodedBuf,decodedBuf,decodedBufLen);
        AppCom_ConvertGSMDefaultToASCII((uint8*)encodedBuf,
                                        decodedBufLen,
                                        (uint8*)decodedBuf);
        //TBD Do we have to care for LF/CR characters ?
        if (pMe->m_PNNInfo.m_pnnArray[recNum].bIsAddCI)
        {
          MSG_MED("EONS: Country Initial Must Be Added",0,0,0);
          buf[0] = '-'; // get CI from MCC TBD
          MEMCPY(&buf[1],decodedBuf,decodedBufLen);
        } else
        {
          MEMCPY(buf,decodedBuf,decodedBufLen);
        }
      } else
      {
        MSG_MED("EONS: Unknown Decoding Type",0,0,0);
        nameAvailable = FALSE;
      }
    } else
    {
      /* SPN, OPL and PNN not available Get Name from ONS CPHS File*/
      nameAvailable = CoreApp_DisplayFromCPHSInfo(pMe,encodedBuf);
      if (nameAvailable)
      {
        MSG_MED("EONS: CPHS ONS Name is Available...Now Start Decoding",0,0,0);

        //Call the Decoding Fucntions here with encodedBuf
        decodedBuf = (char*)MALLOC((pMe->m_CPHSOpNameInfo.m_NameLen+1)*sizeof(char));
        if (decodedBuf == NULL)
        {
          FREEIF(buf);
          FREEIF(encodedBuf);
          nameAvailable = FALSE;
        }

        AppCom_ConvertGSMDefaultToASCII((uint8*)encodedBuf,
                                        pMe->m_CPHSOpNameInfo.m_NameLen,
                                        (uint8*)decodedBuf);
        decodedBuf[pMe->m_CPHSOpNameInfo.m_NameLen] = '\0';
        MEMCPY(buf,decodedBuf,pMe->m_CPHSOpNameInfo.m_NameLen);
      }
    }
  }

#endif /* #ifdef FEATURE_CPHS_EONS */

  if (!nameAvailable)
  {
    //Use the name from the ssInfo
    if (pMe->m_ssInfo.mm_information.full_name_avail){

      len = STRLEN((char*)pMe->m_ssInfo.mm_information.full_name.name);
      pResult = MEMCPY( buf, pMe->m_ssInfo.mm_information.full_name.name,
                  MIN( len, MAX_STR_SIZE));
      if (!pResult) {
        CORE_ERR("Null Ptr from MEMCPY",0,0,0);
        FREEIF(buf);
        return EFAILED;
      }
      nameAvailable= TRUE;

    } else if (pMe->m_ssInfo.mm_information.short_name_avail) {

      len = STRLEN((char*)pMe->m_ssInfo.mm_information.short_name.name);
      pResult = MEMCPY( buf, pMe->m_ssInfo.mm_information.short_name.name,
                  MIN( len, MAX_STR_SIZE));
      if (!pResult) {
        CORE_ERR("Null Ptr from MEMCPY",0,0,0);
        FREEIF(buf);
        return EFAILED;
      }

      nameAvailable = TRUE;

    } 
 #ifdef FEATURE_USRSYSCLOCK
    // Check to see if we have the name previously saved in CFGI_NITZ_NW_INFO
    else if ((CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO,
              wBuf, sizeof(wBuf))) == TRUE) &&
             (CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO_MCC,
              &mcc, sizeof(mcc))) == TRUE) &&
             (CheckAEEReturnStatus(ICONFIG_GetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO_MNC,
              &mnc, sizeof(mnc))) == TRUE)) 
    {
      if ((wBuf[0] != 0x0000) && (nwInfo.mcc == mcc) && (nwInfo.mnc == mnc))
      {
        (void)WSTRTOSTR(wBuf, buf, MAX_STR_SIZE);
        nameAvailable = TRUE;
      }
    }
 #endif //FEATURE_USRSYSCLOCK
  }

  if (!nameAvailable){
    /* get the name from the predefined list */
    if (bGotUINetworkName)
    {
      (void) SNPRINTF(buf, MAX_STR_SIZE*sizeof(char), "%s", nwInfo.short_name_ptr);
    }
    else
    {
      (void) SNPRINTF(buf, MAX_STR_SIZE*sizeof(char), "%d %d", nwInfo.mcc, nwInfo.mnc);
    }
  }

  *ppszPLMN = buf;

#ifdef FEATURE_CPHS_EONS
  FREEIF(encodedBuf);
  FREEIF(decodedBuf);
#endif /* #ifdef FEATURE_CPHS_EONS */
  return SUCCESS;
}

#ifdef FEATURE_USRSYSCLOCK
/*===========================================================================
FUNCTION CoreUtils_UpdatePLMNCFGI

DESCRIPTION
  Update CFGI_NITZ_NW_INFO, CFGI_NITZ_NW_INFO_MCC, CFGI_NITZ_NW_INFO_MNC

DEPENDENCIES
  
SIDE EFFECTS
  None

Note: 
===========================================================================*/
int CoreUtils_UpdatePLMNCFGI(CCoreApp *pMe)
{
  char *buf = NULL;
  sys_plmn_id_s_type plmn;
  AECHAR wBuf[MAX_STR_SIZE+1] = {0};
  uint32 mcc = 0;
  uint32 mnc = 0;
  boolean bNameAvailable = FALSE;
  boolean plmn_is_undefined;
  boolean mnc_includes_pcs_digit;


  if ((NULL == pMe) || (NULL == pMe->m_pIConfig))
  {
    return EFAILED;
  }

  if ((!pMe->m_ssInfo.mm_information.plmn_avail) || 
      ((!pMe->m_ssInfo.mm_information.full_name_avail) && 
       (!pMe->m_ssInfo.mm_information.short_name_avail)))
  {
    //No data available for update
    return SUCCESS;
  }

  if(pMe->m_ssInfo.srv_capability != AEECM_SRV_CAPABILITY_NOT_AVAILABLE &&
    pMe->m_ssInfo.srv_capability != AEECM_SRV_CAPABILITY_NONE)
  {
    // Get mcc and mnc information
    plmn.identity[0] = pMe->m_ssInfo.mm_information.plmn.identity[0];
    plmn.identity[1] = pMe->m_ssInfo.mm_information.plmn.identity[1];
    plmn.identity[2] = pMe->m_ssInfo.mm_information.plmn.identity[2];

    sys_plmn_get_mcc_mnc( plmn, &plmn_is_undefined, &mnc_includes_pcs_digit,
                         &mcc, &mnc );

    buf = (char*) MALLOC(MAX_STR_SIZE*sizeof(char));
    if(buf == NULL)
    {
      return ENOMEMORY;
    }

    if (pMe->m_ssInfo.mm_information.full_name_avail) 
    {
      int len = STRLEN((char*)pMe->m_ssInfo.mm_information.full_name.name);

      MEMCPY( buf, pMe->m_ssInfo.mm_information.full_name.name,
            MIN( len, MAX_STR_SIZE));
      (void)STRTOWSTR(buf,wBuf,sizeof(AECHAR)* MAX_STR_SIZE);
      bNameAvailable = TRUE;
    } 
    else if (pMe->m_ssInfo.mm_information.short_name_avail) 
    {
      int len = STRLEN((char*)pMe->m_ssInfo.mm_information.short_name.name);

      MEMCPY( buf, pMe->m_ssInfo.mm_information.short_name.name,
            MIN( len, MAX_STR_SIZE));

      (void)STRTOWSTR(buf,wBuf,sizeof(AECHAR)* MAX_STR_SIZE);
      bNameAvailable = TRUE;
    }
  }

  if (bNameAvailable)
  {
    ICONFIG_SetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO, (void *)wBuf, sizeof(wBuf));
    ICONFIG_SetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO_MCC, (void *)&mcc, sizeof(mcc));
    ICONFIG_SetItem(pMe->m_pIConfig, CFGI_NITZ_NW_INFO_MNC, (void *)&mnc, sizeof(mnc));
  }

  FREEIF(buf);
  return SUCCESS;
} /* End FormatPLMN */
#endif // FEATURE_USRSYSCLOCK

#endif //defined(FEATURE_GSM) || defined (FEATURE_WCDMA)

//lint -restore  Restore #766 "Coreapp_qvga_res.h" not used
//lint -restore  Restore #611 "suspicious cast" errors
