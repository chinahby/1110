#ifndef COREUTILS_H
#define COREUTILS_H
/*===========================================================================

FILE: CoreUtils.h

DESCRIPTION
   Header file for CoreUtils.c

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2002 - 2008 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreUtils.h#116 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
09/28/07   clm     Add PLMN name update CFGI and get functions.
03/20/07   jas/ak  Adding support for persistent preference values.
02/23/07   jas     Refactoring for HDK builds
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
11/14/06   ypmw    Fix audio mutes due to key press (despite DTMF and key tone off) 
10/19/06   jas     Changing backlight defines to match new semantics
09/08/06   jas     Removing menus that were related to the NetSettings app
08/29/06   jas     Restoring changes that were erroneously reverted in prior
                   sidetone control checkin.
08/23/06   jks     Added support for WLAN Adhoc networks
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added menu to set IncallMixing state
08/08/06   joe     NetSettings related changes.
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
05/04/06   jas     Porting display mapping code from 7500
03/13/06   jks     Added support for WLAN scan mode selection
02/13/06   jas     Added support for GCF language selection
12/05/05   jks     Added WLANScanModeChangeTimerExpired declaration
11/21/05   jas     Support for disabling background images
10/21/05   RI      Added the NITZ menu functions.
10/20/05   sun     Removed support for BT-SAP
10/12/05   RI      Added GetDisplayStatus() declaration.
09/28/05   RI      Changed GetTimeOfDay() declaration.
09/22/05   cvs     Support for Stereo USB Carkit
09/05/05   RI      Enable user time using MM info
08/31/05   sun     Added support for BT-SAP
08/18/05   jas     Adding routine to convert hex wide strings to binary buffer
08/08/05   aps     Fixed NV volume level lookup for sound devices
07/20/05   AT      Added CFGI items for display_duration and back_light_intensity
                   NV item is the intensity except for 0, if we read a 0, then
                   we use the default value.  The value 0 is treated as special
                   since on some targets (6550), 0 makes the display unreadable
                   and we only want to use this if the user has actually requested
                   it.
06/30/05   jas     Removing unused SetWLANEnc and SetWLANAuth functions
                   Moving IsSubscriptionAvail from CoreApp.c
06/28/05   cvs     Add menu to select sound device
06/10/05   cvs     Support for Smart USB Carkit
06/07/05   sun     Added support for CCBS
05/27/05   jas     Added Get/Set functions for WLAN network selection mode
05/19/05   aps     Prototype CoreTask_GetSilenceAll()& CoreTask_SetSilenceAll(). 
05/10/05   cvs     Get powerdown done in one call from TMC
04/19/05   jas     Conversion of ICM WLAN channels to IWIFI values
04/13/05   jas     Reworking WLAN security menus
03/15/05   cvs     Support for updating time on screen less frequently
03/14/05   cvs     Modifying Get/Set routines to maniuplate 64-bit values
03/10/05   jas     Removed local WLAN SSID string size
03/07/05   jas     Converting get/set routines to use uint64
02/22/05   jas     Additional mods for WLAN settings menus
01/21/05   cvs     Rearchitect uisnd_set_device()
01/20/04   trc     Added CoreTask_GetGameMode()
12/09/04   cvs     Add function to avoid current system
12/06/04   BW      Added LAN menu and related submenu
11/12/04   ank,cvs Dualslot changes from 6300
09/28/04   JAS     Updated feature defs for secondary display content
09/22/04   cvs     Add Contrast menu, lint fixes
09/02/04   tml     Added CPHS support and added revision history comment
07/29/04   JAS     Added support for dual-display targets
06/24/04   sun     Send Separate Event to Change Mode
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
05/25/04   SUN     Fixed Compile Warning
05/13/04   AT      Moved restriction settings to ui_base.h
05/13/04   sun     Added LPM Changes
05/07/04   sun     Fixed TTY, Separated Game Mode from LPM
04/29/04   sun     Fixed Backlight duration
04/16/04   sun     Made Backlight/kepad light modifications
04/14/04   sun     Added new menu item to set VR
03/01/04   sun     Added function to set Game Mode
02/25/04   ram     Integration with IBatt Interface.
01/28/04   ram     Add speaker-phone functionality.
12/22/03   tml     ICARD API Changes
01/07/04   ram     Add Multimedia Volume setting
12/09/03   ram     Removed lint errors.
12/02/03   ram     Changes for exiting emergency mode.
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/23/03   sun     Renamed functions
11/20/03   ram     Added HDR_ABORT_PPP support and fixed annunciators.
11/13/03   NNG     Fixed compiler error.
11/11/03   ram     Added Game-Mode.
11/03/03   tml     Fixed compilation error
11/03/03   ram     Added PLMN selection menu.
09/30/03   tml     Added function prototypes
09/26/03   tml     Added function prototypes
09/21/03   tml     ICM support
08/29/03   ram     Removed function CheckAEEReturnStatus (Added macro to 
                   AppComFunc.h)
08/06/03   tml     Dual slot support
08/05/03   kar     Set time changes
05/08/03   kar     System menu changes
05/05/03   tml     Added NV Lock Code support
04/14/03   kar     Code cleanup
04/01/03   kar     Redefined core macros. Code cleanup
03/18/03   kar     Removed unused prototypes
02/21/03   kar     Changed function prototype
02/10/03   kar     Added support for numedit dialog
02/03/03   kar     Fixed compiler warnings
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
01/30/03   kar     Removed PM1000 code
01/28/03   kar     Changes for AppComFunc API
01/07/03   kar     Changes for AppComFunc API
12/31/02   kar     Defined size for time string
10/08/02   tml     code review changes
09/20/02   aaa     Support for time display at my info screen
09/16/02   kar     Support for time display
08/21/02  js/tml   Support for ICARD Integration
08/13/02   kar     Core utils header
===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AEE.h"
#include "AEEMenu.h"

#include "CoreApp.h"

#include "nv.h"
#include "hs.h"


#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER) || defined(PLATFORM_LTK)
/* temporary until completion of BREW api */
#include "disp.h"
#include "keypad.h"

#define LCD_BACKLIGHT_LVL_DEFAULT LCD_BACKLIGHT_LVL_2
#define KEYPAD_BACKLIGHT_LVL_DEFAULT KEYPAD_BACKLIGHT_LVL_2
#define COREAPP_LVL_0_NV_VAL (0xA0 | LCD_BACKLIGHT_LVL_0)
#endif

/*=============================================================================

        MACRO DECLARATION

=============================================================================== */

/*===========================================================================
MACRO CORE_FATAL

DESCRIPTION
  Call ERR_FATAL when fatal error needs to be informed

DEPENDENCIES
  ERR_FATAL

SIDE EFFECTS
  None
===========================================================================*/
#include "err.h"
#include "msg.h"
#include "oemnvint.h"
#define CORE_FATAL(xx_exp)\
  if(!(xx_exp)){ \
    ERR_FATAL( "Fatal Assertion " #xx_exp " failed", 0, 0, 0 ); \
  }
#define CORE_ERR(format, code1, code2, code3) MSG_ERROR(format, code1, code2, code3)
#define CORE_MSG(format, code1, code2, code3) MSG_MED(format, code1, code2, code3)

#define SELECTIONVERIFICATIONTIME 2000
#define SELECTION_TIME_IN_SECONDS (SELECTIONVERIFICATIONTIME / 1000)

#define BANNER_BUFFER_SIZE  10      /* Size for Banner string */
#define TIME_BUFFER_SIZE    10      /* Size for Time String */
#define TIMEDATE_BUFFER_SIZE    25  /* Size for TimeDate string */
#define DATE_BUFFER_SIZE    11      /* Size for Date string - mm/dd/yyyy */
#define NUMBER_BUFFER_SIZE  32      /* Size for Dial string */
#define WORD_BUFFER_SIZE    10      /* Size of input buffer for 32-bit-value */

#define COREUTILS_EFS         0x0001
#define COREUTILS_SPEED_DIAL  0x0002
#define COREUTILS_ADN_SLOT1   0x0004
#define COREUTILS_ADN_SLOT2   0x0008
#define COREUTILS_FDN_SLOT1   0x0010
#define COREUTILS_FDN_SLOT2   0x0020
#define COREUTILS_NO_RESTRICTION 0x0000
#define COREUTILS_NO_EFS            0xFFFE
#define COREUTILS_NO_SPEED_DIAL     0xFFFD
#define COREUTILS_NO_ADN_SLOT1      0xFFFB
#define COREUTILS_NO_ADN_SLOT2      0xFFF7
#define COREUTILS_NO_FDN_SLOT1      0xFFEF
#define COREUTILS_NO_FDN_SLOT2      0xFFDF

#ifdef FEATURE_MMGSDI_DUAL_SLOT
#define COREUTILS_PENDING_SLOT_BEFORE_FORCE_MODE  0
#define COREUTILS_FORCE_MODE_SUCCESS              1
#define COREUTILS_FORCE_MODE_FAIL                 2
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#define UI_BKLT_TIME_NONE 0             /* backlight always off */
#define UI_BKLT_TIME_MAX  255           /* backlight always on */
#define UI_BKLT_DEFAULT 10        /* backlight default timer */


#ifdef FEATURE_TTY
typedef enum {
    UI_TTY_FULL=OEMNV_TTY_FULL,
    UI_TTY_TALK=OEMNV_TTY_TALK,
    UI_TTY_HEAR=OEMNV_TTY_HEAR,
    UI_TTY_OFF=OEMNV_TTY_OFF
} ui_tty_option_type;
#endif
/* No of rings reqd. for auto-answer */
#define UI_AUTO_ANSWER_RINGS 2
#define CDMAMIN 1

/* Technology #define for subscription checking */
#define COREAPP_GSM                           0x01
#define COREAPP_UMTS                          0x02
#define COREAPP_CDMA                          0x03
#define COREAPP_WLAN                          0x04

#if defined(FEATURE_DISPLAY_CONTENT_MAP)
/* Logical display types */
#define COREAPP_LOGICAL_DISP_PRI              0x01
#define COREAPP_LOGICAL_DISP_SEC              0x02
#define COREAPP_LOGICAL_DISP_NONE             0x03
#endif /* defined(FEATURE_DISPLAY_CONTENT_MAP) */


/*=============================================================================

              FUNCTION DECLARATION

=============================================================================== */

/*===========================================================================
FUNCTION GetRestrict

DESCRIPTION
  Get Restrict value from NV, update the byRestrict static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRestrict(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetNVRestrict

DESCRIPTION
  write the pMe restrict member to NV

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNVRestrict(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetRestrict

DESCRIPTION
  Set static variable byRestrict, and write to NV

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRestrict(CCoreApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION GetMute

DESCRIPTION
  Get the bMute static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetMute(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetMute

DESCRIPTION
  Set the bMute static variable, call the uisnd_set_mute which will do the
  actual muting or unmuting

DEPENDENCIES
  uisnd_set_mute

SIDE EFFECTS
  None
===========================================================================*/
boolean SetMute(CCoreApp * pMe, uint64 nNewVal);


#ifdef FEATURE_TTY
/*===========================================================================
FUNCTION GetTTY

DESCRIPTION
  Obtain TTY information from NV, set the byTTY variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetTTY(CCoreApp * pMe);



/*===========================================================================
FUNCTION SETTTY

DESCRIPTION
  Set the byTTY static variable

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetTTY(CCoreApp * pMe, uint64 nNewVal);
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
uint64 GetSoundDevice(CCoreApp * pMe);


/*===========================================================================
FUNCTION SetSoundDevice

DESCRIPTION
  Set Sound Device in uixsnd.  -1 indicates audoselect.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSoundDevice(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetAutoHyphen

DESCRIPTION
  Obtain Auto Hyphen information from NV, set the bAutoHyphen variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoHyphen(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetAutoHyphen

DESCRIPTION
  Write the new Auto Hyphen information to NV, update the bAutoHyphen variable

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoHyphen(CCoreApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION GetVR

DESCRIPTION
  Obtain VR information from ICONFIG, set the bVR variable

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetVR(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetVR

DESCRIPTION
  Write the new VR information to ICONFIG, update the bVR variable

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
boolean SetVR(CCoreApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION FormatDate

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
void FormatDate (JulianType julian, AECHAR *pszString, int nLen);

/*===========================================================================
FUNCTION FormatTime

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
void FormatTime (JulianType julian, AECHAR *pszString, int nLen);

/*===========================================================================
 FUNCTION FormatTimeDate

 DESCRIPTION
 Formats the time and the date

 DEPENDENCIES
  FormatTime, FormatDate

SIDE EFFECTS
  None
===========================================================================*/
int FormatTimeDate(JulianType time, AECHAR *pszDest,  int nLen);

/*===========================================================================
FUNCTION UpdateBacklight

DESCRIPTION
  update backlight screen accordingly

DEPENDENCIES
  ui_backlight

SIDE EFFECTS
  None
===========================================================================*/
void UpdateBacklight(CCoreApp *pMe);


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
uint64 GetBackLightDuration(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetBackLightDuration

DESCRIPTION
  Update byBacklight variable, and write to NV.

DEPENDENCIES
  ui_put_nv, UpdateBacklight

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBackLightDuration(CCoreApp * pMe, uint64 nNewVal);

#if defined (FEATURE_PMIC_LCDKBD_LED_DRIVER)
/*===========================================================================
FUNCTION GetBackLightIntensity

DESCRIPTION
  Get byBklightIntensity

DEPENDENCIES
  uiscrn_set_lcd_backlight_level

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetBackLightIntensity(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetBackLightIntensity

DESCRIPTION
  Set new value to byBklightIntensity, update lcd backlight intensity level

DEPENDENCIES
  uiscrn_set_lcd_backlight_level

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBackLightIntensity(CCoreApp * pMe, uint64 nNewVal);
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
uint64 GetServiceAlert(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetServiceAlert

DESCRIPTION
  A NV write to service alert parameter and update the bServiceAlert variable

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetServiceAlert(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION InitializeVolumes

DESCRIPTION
  initialize the various device volumes.

DEPENDENCIES
  the corresponding Get/Set methods for Sound related features

SIDE EFFECTS
  None
===========================================================================*/
void InitializeVolumes (CCoreApp *pMe);

#ifdef FEATURE_MULTIMEDIA
/*===========================================================================
FUNCTION GetMMVolumeNVEnum

DESCRIPTION
  Based on the current bAnalogHfk and bOnhook values, determine
  the corresponding NV items for MM ringer volume

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetMMVolumeNVEnum(void);

/*===========================================================================
FUNCTION GetMMVolume

DESCRIPTION
  Get the MM ringer volume from NV store, update the static variable and update
  the sound device's ringer volume setting

DEPENDENCIES
  GetMMVolumeNVEnum, ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint32 GetMMVolume(CCoreApp * pMe);

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
boolean SetMMRingerVolume(CCoreApp * pMe, uint32 nNewVal);
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
nv_items_enum_type GetRingerVolumeNVEnum(void);

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
uint64 GetRingerVolume(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetRingerVolume

DESCRIPTION
  Set the static variable to the new value, update NV item and sound driver
  ringer volume setting

DEPENDENCIES
  GetRingerVolumeNVEnum, ui_put_nv, uisnd_ring_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRingerVolume(CCoreApp * pMe, uint64 nNewVal);


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
nv_items_enum_type GetKeyBeepVolumeNVEnum(void);

/*===========================================================================
FUNCTION GetKeyBeepVolume

DESCRIPTION
  Get the beep volume from NV and update the sound driver

DEPENDENCIES
  GetKeyBeepVolumeNVEnum, ui_get_nv, uisnd_beep_vol

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetKeyBeepVolume(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetKeyBeepVolume

DESCRIPTION
  Set the key beep information in NV, update the sound driver

DEPENDENCIES
  GetKeyBeepVolumeNVEnum, ui_put_nv, uisnd_beep_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetKeyBeepVolume(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetEarpieceVolumeNVEnum

DESCRIPTION
  Return the NV item type for earpiece based on several device variables

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetEarpieceVolumeNVEnum(void);

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
uint64 GetEarpieceVolume(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetEarpieceVolume

DESCRIPTION
  Set earpiece information in NV, update sound driver

DEPENDENCIES
  GetEarpieceVolumeNVEnum, ui_put_nv, uisnd_voice_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetEarpieceVolume(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetMessageVolumeNVEnum

DESCRIPTION
  Return message alert NV item depending on various device parameters

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type GetMessageVolumeNVEnum(void);

/*===========================================================================
FUNCTION GetMessageVolume

DESCRIPTION
  Get Message alert volume from NV, update byMessageVolume and sound driver ???

DEPENDENCIES
  GetMessageVolumeNVEnum, ui_get_nv, uisnd_alert_vol

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetMessageVolume(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetMessageVolume

DESCRIPTION
  Write message alert volume to NV, update static variable and sound driver

DEPENDENCIES
  GetMessageVolumeNVEnum, ui_put_nv, uisnd_alert_vol

SIDE EFFECTS
  None
===========================================================================*/
boolean SetMessageVolume(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetSilenceAll

DESCRIPTION
  Get the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetSilenceAll(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetSilenceAll

DESCRIPTION
  set the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSilenceAll(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetAutoRetry

DESCRIPTION
  Get auto retry duration setting from NV

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoRetry(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetAutoRetry

DESCRIPTION
  Write auto retry information: interval + on/off, to NV

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoRetry(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetIncallMixing

DESCRIPTION
  Get the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetIncallMixing(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetIncallMixing

DESCRIPTION
  set the static variable

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
boolean SetIncallMixing(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetAutoAnswer

DESCRIPTION
  Get auto answer (on/off) setting from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetAutoAnswer(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetAutoAnswer

DESCRIPTION
  Write auto answer information (on/off + # of rings) to NV, update bAutoAnswer

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetAutoAnswer(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetBeepLength

DESCRIPTION
  Get beep length information from NV

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetBeepLength(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetBeepLength

DESCRIPTION
  Assign new value to beep length static variable and write to NV

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBeepLength(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetRingerType

DESCRIPTION
  Get ringer type from NV, update current static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetRingerType(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetRingerType

DESCRIPTION
  Assign new value to ringer type and write to NV

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRingerType(CCoreApp * pMe, uint64 newVal);

/*===========================================================================
FUNCTION GetOneTouch

DESCRIPTION
  Get one touch information from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetOneTouch(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetOneTouch

DESCRIPTION
  Set one touch setting to NV, update one touch static variable

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetOneTouch(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetKeyguard

DESCRIPTION
  Get keyguard value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetKeyguard (CCoreApp *pMe);

/*===========================================================================
FUNCTION SetKeyguard

DESCRIPTION
  assign new keyguard value, start key guard initiation timer as required

DEPENDENCIES
  CreateKeyGuardOnDlg

SIDE EFFECTS
  None
===========================================================================*/
boolean SetKeyguard(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetSmartSound

DESCRIPTION
  Get smart sound information from NV, update static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetSmartSound(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetSmartSound

DESCRIPTION
  Set smart sound variable, write to NV, update sound driver

DEPENDENCIES
  ui_put_nv, uisnd_set_aagc

SIDE EFFECTS
  None
===========================================================================*/
boolean SetSmartSound(CCoreApp * pMe, uint64 nNewVal);


/*===========================================================================
FUNCTION GetBanner

DESCRIPTION
  Get NV banner information and update banner static variable

DEPENDENCIES
  ui_get_nv, STRTOWSTR

SIDE EFFECTS
  None
===========================================================================*/
AECHAR * GetBanner(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetBanner

DESCRIPTION
  Write the new banner information to NV, and update static variable

DEPENDENCIES
  ui_put_nv, WSTRTOSTR, memcpy

SIDE EFFECTS
  None
===========================================================================*/
boolean SetBanner(CCoreApp * pMe, AECHAR * psNewVal);

/*===========================================================================
FUNCTION WATOI

DESCRIPTION
  Takes a AECHAR string and turns the result into an integer.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WATOI(const AECHAR *str);

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
boolean GetTimeofDay (CCoreApp *pMe, JulianType *julian_date);
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
boolean SetTimeofDay (CCoreApp *pMe, JulianType *julian);

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
void DisplayTimeWithRefresh (CCoreApp *pMe);

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
boolean DisplayTimeUpdateScreen (CCoreApp *pMe);

/*===========================================================================
FUNCTION GetPrivacy

DESCRIPTION
  Return privacy option from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPrivacy(CCoreApp * pMe);


/*===========================================================================
FUNCTION SetPrivacy

DESCRIPTION
  update static variable, write to NV and the corresponding cm info

DEPENDENCIES
  ui_cm_privacy_pref, ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPrivacy(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetPINLock1

DESCRIPTION
  based on Security state, set and return the PINLock1 status

DEPENDENCIES
  CheckSecurityState

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock1(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetPINLock1

DESCRIPTION
  Set PIN Lock using ICard functions

DEPENDENCIES
  ICARD_EnablePIN, ICARD_DisablePIN

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock1(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetPINLock2

DESCRIPTION
  Return the pMe->m_bPIN2_enabled variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock2(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetPINLock2

DESCRIPTION
  Call ICard function to enable or disable PIN2 based on the input param

DEPENDENCIES
  ICARD_EnablePin, ICARD_DisablePin

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock2(CCoreApp * pMe, uint64 nNewVal);


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
uint64 GetPINLock1Slot2(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetPINLock1Slot2

DESCRIPTION
  Set PIN Lock for slot 2 using ICard functions

DEPENDENCIES
  ICARD_EnablePIN, ICARD_DisablePIN

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock1Slot2(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetPINLock2Slot2

DESCRIPTION
  Return the pMe->m_sPIN2_slot2_lock_state variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetPINLock2Slot2(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetPINLock2Slot2

DESCRIPTION
  Call ICard function to enable or disable PIN2 for slot 2 based on the 
  input param

DEPENDENCIES
  ICARD_EnablePin, ICARD_DisablePin

SIDE EFFECTS
  None
===========================================================================*/
boolean SetPINLock2Slot2(CCoreApp * pMe, uint64 nNewVal );
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION GetNVLock

DESCRIPTION
  Return the pMe->m_bNV_enabled variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNVLock(CCoreApp * pMe);



/*===========================================================================
FUNCTION SetNVLock

DESCRIPTION
  Set the NV lock code

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNVLock(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION DisplayIdleMenuDlg

DESCRIPTION
  Bring the program to the Idle menu screen display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean DisplayIdleMenuDlg(CCoreApp *pMe);

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
void DisplayMessageDlg(CCoreApp *pMe, uint16 wRes_IDD, int nDisplaySeconds);

/*===========================================================================
FUNCTION DisplayMessageDlgEx

DESCRIPTION
  Create a message dialog and display the text passed in
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayMessageDlgEx(CCoreApp *pMe, AECHAR *pszText, int nDisplaySeconds);

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
void PopDlg(CCoreApp* pMe, uint16 wPopDlgID);


/*===========================================================================
FUNCTION SetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetGeneralStaticTextStyle(IStatic *pIStaticText);


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
boolean SetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl);

/*===========================================================================
FUNCTION EndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void EndMessageDlg(CCoreApp *pMe);

/*===========================================================================
FUNCTION EndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void EndCurrentDlg(CCoreApp *pMe);

/*===========================================================================
FUNCTION MessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean MessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION HourGlassDlgEventHandler

DESCRIPTION
  handle hour glass message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean HourGlassDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION Min2ToAsc

DESCRIPTION
  Convert a MIN2 type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int Min2ToAsc( word wAreaCode, char *pszText );


/*===========================================================================
FUNCTION Min1ToAsc

DESCRIPTION
  Convert a MIN1 type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int Min1ToAsc( dword dwPhoneNo, char *pszText );


/*===========================================================================
FUNCTION MccToAsc

DESCRIPTION
  Convert a MCC type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int MccToAsc( word wMcc, char *pszTxt);


/*===========================================================================
FUNCTION MncToAsc

DESCRIPTION
  Convert a MNC type number to ASCII

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int MncToAsc( word wMnc, char *pszTxt);


/*===========================================================================
FUNCTION ExitEmergencyMode

DESCRIPTION

  Exit from the Emergency mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ExitEmergencyMode(CCoreApp *pMe);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION SetNetworkAutoSelection

DESCRIPTION

  Change network select setting.

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
                                AEECMServDomPref srv_domain_pref);

/*===========================================================================
FUNCTION SetNetworkManualSelection

DESCRIPTION

  Change network select setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNetworkManualSelection(CCoreApp *pMe, uint32 nNewSetting);

/*===========================================================================
FUNCTION CoreUtil_GetNetworksCb

DESCRIPTION

  List of avialable PLMNs is sent by ICM with this callback.
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void  CoreUtil_GetNetworksCb(CCoreApp *pMe);
/*===========================================================================
FUNCTION GetNetworkSelect

DESCRIPTION
  Get Network -> Select menu setting from SK UI.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNetworkSelect(CCoreApp *pMe);
#endif /* #if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */

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
uint64 GetRTREConfig(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetRTREConfig

DESCRIPTION
  Set RTRE Config.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetRTREConfig(CCoreApp *pMe, uint64 nNewSetting) ;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GetUserSlotPrefGSM

DESCRIPTION
  Return the user preferred slot for GSM from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetUserSlotPrefGSM(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetUserSlotPrefGSM

DESCRIPTION
  Call gsdi function set the User Slot Preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetUserSlotPrefGSM(CCoreApp * pMe, uint64 nNewVal );

/*===========================================================================
FUNCTION GetUserSlotPrefCDMA

DESCRIPTION
  Return the user preferred slot for CDMA from NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetUserSlotPrefCDMA(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetUserSlotPrefCDMA

DESCRIPTION
  Call gsdi function set the User Slot Preference

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetUserSlotPrefCDMA(CCoreApp * pMe, uint64 nNewVal );

/*===========================================================================
FUNCTION GetCards

DESCRIPTION
  Gets the Card Status for both cards

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCards(CCoreApp * pMe);

#endif /*FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION IsCardValid

DESCRIPTION
  check if the card is ok

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IsCardValid(CCoreApp *pMe, uint8 byPinID);

 /*=============================================================================
FUNCTION: PopulateCardApplications

DESCRIPTION:
  Populate the card application accordingly

SIDE EFFECTS:
  None.

=============================================================================*/
int PopulateCardApplications (CCoreApp *pMe);

/*===========================================================================
FUNCTION LookupResIDForSysPrefErrCode

DESCRIPTION
  Map between ICM PH error code with the Resource String ID

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
uint16 LookupResIDForSysPrefErrCode (uint32 dwErrCode);

/* <EJECT> */
/*===========================================================================
FUNCTION UI_BACKLIGHT

DESCRIPTION
  Handle backlight off timing

DEPENDENCIES
  ui.backval should be set correctly

RETURN VALUE
  Nothing

SIDE EFFECTS
  Sets up a clock callback

===========================================================================*/
void ui_backlight (
  boolean onoff,                  /* turn on or off */
  int time                        /* how long on - UI_BKLT_TIME_MAX = always */
                                  /* UI_BKLT_TIME_NONE = never. */
);

/*===========================================================================
FUNCTION SetUSBHFKAttached

DESCRIPTION
  Used to set whether or not a USB HFK is attached
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void SetUSBHFKAttached(boolean new_value);

/*===========================================================================
FUNCTION GetUSBHFKAttached

DESCRIPTION
  Used to get whether or not a USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean GetUSBHFKAttached(void);

/*===========================================================================
FUNCTION SetStereoUSBHFKAttached

DESCRIPTION
  Used to set whether or not a Stereo USB HFK is attached
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void SetStereoUSBHFKAttached(boolean new_value);

/*===========================================================================
FUNCTION GetStereoUSBHFKAttached

DESCRIPTION
  Used to get whether or not a USB HFK is attached

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean GetStereoUSBHFKAttached(void);

/*===========================================================================
FUNCTION GetSpeakerPhone

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifndef FEATURE_UIONE_HDK
boolean GetSpeakerPhone(void);
#else
static __inline boolean GetSpeakerPhone(void)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */

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
extern void EnableUIKeys (boolean flag);

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
  int time                        /* how long on - UI_BKLT_TIME_MAX = always */
                                  /* UI_BKLT_TIME_NONE = never. */
);

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
void CoreUtil_FormatPLMN(AECHAR* pszBuf, int nBufSize, AEECMDetailedPLMNInfo *pDetailedPLMNInfo, boolean bDisplayRAT);
/*===========================================================================
FUNCTION CoreUtil_CancelGetNW

DESCRIPTION
  Cancel the call to ICM_GetNetworks and free allocated memory.

DEPENDENCIES
  None

  SIDE EFFECTS
  None
===========================================================================*/
void CoreUtil_CancelGetNW(CCoreApp *pMe);
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/


/*===========================================================================
FUNCTION GetGameMode

DESCRIPTION
    Return the current game mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetGameMode(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetGameMode

DESCRIPTION
    Return the Game mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetGameMode(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetOprtMode

DESCRIPTION
    Return the current phone operating mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 GetOprtMode(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetOprtMode

DESCRIPTION
    Return the current phone operating mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetOprtMode(CCoreApp *pMe, uint32 nNewVal);
/*===========================================================================
FUNCTION GetCUGEnable

DESCRIPTION
    Return  if CUG is enabled or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGEnable(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetCUGEnable

DESCRIPTION
    Set if CUG is enabled or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGEnable(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetCUGPrefGroup

DESCRIPTION
    Return if the CUG Pref Group is supressed or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGPrefGroup(CCoreApp *pMe);


/*===========================================================================
FUNCTION SetCUGPrefGroup

DESCRIPTION
    Set CUG Pref group Supress/ No supress.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGPrefGroup(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetCUGOutAccess

DESCRIPTION
    Return  if CUG Out Access is supressed or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetCUGOutAccess(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetCUGOutAccess

DESCRIPTION
    Set CUG Out Access Supress/ No Supress

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGOutAccess(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetCUGGroupID

DESCRIPTION
    Return  CUG Group ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 GetCUGGroupID(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetCUGGroupID

DESCRIPTION
    Set CUG Group ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetCUGGroupID(CCoreApp *pMe, uint16 nNewVal);

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
void CoreUtil_AbortPPPDataSession(void *pUser);
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
void UpdateCoreText(CCoreApp *pMe);
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
void ResizeCoreDlg(CCoreApp *pMe);



/*===========================================================================
FUNCTION SetLPMMode

DESCRIPTION
    Set the phone operating mode.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetLPMMode(CCoreApp *pMe, uint32 nNewVal);

/*===========================================================================
FUNCTION GetLCDValue

DESCRIPTION
    Return the current LCD

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetLCDValue(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetLCDValue

DESCRIPTION
    Set the LCD

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetLCDValue(CCoreApp *pMe,  uint64 nNewVal);

/*===========================================================================
FUNCTION SetSelOperatorDisplay

DESCRIPTION
    The "Select Operator" Dialog can now be displayed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetSelOperatorDisplay(CCoreApp *pMe);

#if defined(FEATURE_SECONDARY_DISPLAY)
/*===========================================================================
FUNCTION ToggleDisplays

DESCRIPTION
  Toggles between primary and secondary LCDs.  Based on the flip
  position, it turns one display on and the other off.

PROTOTYPE
  void ToggleDisplays(CCoreApp *pMe, boolean bIsFlipOpen)

PARAMETERS
  CCoreApp *pMe:      Pointer to the Core App structure
  boolean IsFlipOpen: Indicates whether the flip is open or closed

DEPENDENCIES
  None

RETURN VALUE
  boolean:   TRUE if the displays could be set correctly; FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ToggleDisplays(CCoreApp *pMe, boolean bIsFlipOpen);
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
uint64 GetALSLineSelected(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetALSLineSelected

DESCRIPTION

  Change ALS Line selection setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetALSLineSelected(CCoreApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION GetALSLineSwitch

DESCRIPTION
  Initialize ALS Line Switching menu

DEPENDENCIES
  Various Get and Set method for the variables in CoreUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetALSLineSwitch(CCoreApp *pMe);

/*===========================================================================
FUNCTION SetALSLineSwitch

DESCRIPTION

  Change ALS Line switching setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetALSLineSwitch(CCoreApp *pMe, uint64 nNewSetting);

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
uint16 GetContrast(CCoreApp * pMe);


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
boolean SetContrast(CCoreApp * pMe, byte nNewVal);


/*===========================================================================
FUNCTION RestoreStoredContrast

DESCRIPTION
  This function changes the screen contrast to the value in NVRam

DEPENDENCIES
  None

SIDE EFFECTS
  Screen contrast changes
===========================================================================*/
void RestoreStoredContrast(CCoreApp * pMe);

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
boolean IsHexWString(AECHAR *pszwHexString);

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
int ConvertWStrToBinBuf(const AECHAR *pszwIn, uint8 *pBuf, uint32 nBufLen);

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
uint64 GetCCBSSetting(CCoreApp * pMe);

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
boolean SetCCBSSetting(CCoreApp * pMe, uint64 nNewVal);

/*=============================================================================
FUNCTION: IsSubscriptionAvail

DESCRIPTION:
  Check if the passed in technology subscription information can be retrieved
  from the card

SIDE EFFECTS:
  None.

=============================================================================*/
boolean IsSubscriptionAvail(CCoreApp *pMe, uint8 wTechnology);

#ifdef FEATURE_USRSYSCLOCK
/*===========================================================================
FUNCTION GetNetworkTime

DESCRIPTION
  Get the NV setting for network time enable/disable.

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetNetworkTime(CCoreApp * pMe);

/*===========================================================================
FUNCTION SetNetworkTime

DESCRIPTION
  Write network time (on/off) to NV, update bNetworkTime

DEPENDENCIES
  ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetNetworkTime(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetDeleteNITZPLMN

DESCRIPTION
  Dummy Get Function to delete the NITZ PLMN info.

DEPENDENCIES
  ICONFIG

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDeleteNITZPLMN(CCoreApp * pMe);
/*===========================================================================
FUNCTION SetDeleteNITZPLMN

DESCRIPTION
  Delete the NITZ PLMN info in NV  

DEPENDENCIES
  (void) ui_put_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDeleteNITZPLMN(CCoreApp * pMe, uint64 nNewVal);
#endif // FEATURE_USRSYSCLOCK

/*===========================================================================
FUNCTION GetDisplayStatus

DESCRIPTION
  Returns status of  flag ui_display 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean GetDisplayStatus(void);

/*===========================================================================
FUNCTION SetDisableBgImage

DESCRIPTION
  Used to set whether or not background images should be disabled

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SetDisableBgImage(CCoreApp * pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetDisableBgImage

DESCRIPTION
  Used to get whether or not background images should be disabled

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 GetDisableBgImage(CCoreApp *pMe);

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
boolean SetLanguageSelection(CCoreApp *pMe, uint64 nNewVal);

/*===========================================================================
FUNCTION GetLanguageSelection

DESCRIPTION
  Used to get the current language selection.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
uint64 GetLanguageSelection(CCoreApp *pMe);
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#if defined(FEATURE_DISPLAY_CONTENT_MAP)
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
uint64 GetDisplay1Mapping(CCoreApp * pMe);

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
boolean SetDisplay1Mapping(CCoreApp * pMe, uint64 nNewVal);

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
uint64 GetDisplay2Mapping(CCoreApp * pMe);

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
boolean SetDisplay2Mapping(CCoreApp * pMe, uint64 nNewVal);

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
uint64 GetDisplay3Mapping(CCoreApp * pMe);

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
boolean SetDisplay3Mapping(CCoreApp * pMe, uint64 nNewVal);

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
uint64 GetDisplay4Mapping(CCoreApp * pMe);

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
boolean SetDisplay4Mapping(CCoreApp * pMe, uint64 nNewVal);

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
boolean ShowManualPLMNList(CCoreApp *pMe, uint16 w);

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
uint64 GetSidetone(CCoreApp *pMe);

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
boolean SetSidetone(CCoreApp *pMe, uint64 newVal);

/*===========================================================================
FUNCTION SetKeySilenceStatus

DESCRIPTION
  set ui_silence_key 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SetKeySilenceStatus(boolean status);

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
/*===========================================================================
FUNCTION CoreUtils_GetPLMN

DESCRIPTION
  Gets PLMN name string.

DEPENDENCIES
  ui_netwk_get_network_info

SIDE EFFECTS
  None
===========================================================================*/
int CoreUtils_GetPLMN(CCoreApp *pMe, char **ppszPLMN, boolean bLookInSIM);

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
int CoreUtils_UpdatePLMNCFGI(CCoreApp *pMe);
#endif // FEATURE_USRSYSCLOCK

#endif //defined(FEATURE_GSM) || defined (FEATURE_WCDMA)

#endif // COREUTILS_H

