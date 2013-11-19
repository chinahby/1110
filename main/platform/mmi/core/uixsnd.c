/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


         U S E R   I N T E R F A C E   S O U N D   M A N A G E R

GENERAL DESCRIPTION
  This manages the nasty details of what volume and which path to use
  at which time.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1991-2007 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/uixsnd.c#81 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/09   Gopi    Changes for Mango UI Integration
03/12/09   kcp     reverting the change done for 
                   "to make both brew and thinui build from same code baseline"
01/28/09   kcp     In order to make both brew and thinui build from same code baseline,
                   we need to add a conditional check on uixsnd.c
06/16/08   cvs     Default sidetone to off
02/06/08   ss      Modified vibrate to use snd_tone_start.
11/16/07   pn      Removed BTApp dependencies.
07/24/07   jks     Featurized support for SND_DEVICE_BT_A2DP_SCO_HEADSET
07/16/07   jks     Added featurization for SND_DEVICE_SDAC
06/18/07   cvs     Fix divide by 0.
06/13/07   jks     Add set_volume_scaled, for backward compatibility
04/20/07   ypmw    Fix the auto-select to handset instead of speaker phone.
                   The bug is from the result of error merge.
02/23/07   jas     Refactoring for HDK builds
02/01/07   ypmw    Move the sound device attach variable from IConfig to
                   uisnd_device_info_table
01/25/07   jks     Fixed RCVT compile warning
01/24/07   jks     Mapped UI 4 levels of voulme to sndcal 7 levels of volume
01/15/07   pn      Enhanced debug messages
12/15/06   jks     Fix for external power event sound interrupting Java game sound
11/06/06   jas,cvs Featurizing snd calls for 7k targets
11/06/06   jas     Reverting prior change (doesn't work)
10/13/06   cvs     Fix compile warnings
10/02/06   jas,cvs Featurizing snd calls for 7k targets
09/23/06   cvs     Fix compile warnings
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added functionality to get & set sound incall-mixing state
07/12/06   cvs     Fix compile warnings
06/15/06   cvs     uisnd_force_device_mute() shouldn't try to force
                   the sound device to SND_DEVICE_CURRENT.
06/15/06   jks     Fix for power sound interrupting ringer sound
06/02/06   cvs     add function to restore mute only
05/12/06   cvs     remove unused header files
04/28/06   cvs     Fix ThinUI compile errors
04/13/06   cvs     Move call control to its own file
03/27/06   ng      Featurized VT app
02/17/06   jks     Removed duplicate code
09/28/05   cvs     Fix ThinUI compile errors
09/22/05   cvs     Support for Stereo USB Carkit
08/08/05   aps     Fixed NV volume level lookup for sound devices
                   Fixed Lint Warnings
08/02/05   AT      Add callbacks to snd_set_device and snd_set_volume.
07/01/05   cvs     Fix key beeps for multi keys, and use callbacks
06/28/05   cvs     Add menu to select sound device
06/10/05   cvs     Support for Smart USB Carkit
06/09/05   cvs     Fix up/down beeps while in calls
06/09/05   cvs     Fix SilenceAll featurization
05/24/05   aps     code cleanup. updated indentation and if-braces in
                   accordance with coding standards.
05/20/05   aps     added MSG_HIGH sound suppression messgs when silenceall
05/19/05   aps     Added support for silenceAll feature
05/06/05   cvs     Better message logging at device/mute change
04/15/05   cvs     Add new type of Bluetooth headset
03/31/05   cvs     More APIs so OEMSound can go through uisnd
04/01/05   cvs     Add notifications of device/mute setting changes
03/31/05   cvs     Add APIs so OEMSound can go through uisnd, preventing device conflicts
03/07/05   cvs     properly set return code in uisnd_set_device_specific()
03/01/05   sun     Fixed default device types
02/23/05   cvs     ThinUI compile fixes
02/16/05   cvs     Fix BT headset featurization
01/24/05   rsl     Removed obsolete include file ds.h
01/21/05   cvs     Rearchitect uisnd_set_device()
12/20/04   cvs     Handle sound devices on apps processor in multiproc systems
11/23/04   sun     Added support for USB HFK
10/08/04   AT      Fixed code review comments.
10/07/04   AT      Updated uisnd_multi to make sure a SND_STOP is
                   at the end of the uisnd_multi function.
08/03/04   PA      Added support for thin UI builds, set sound volume
07/27/04   sun     Do not play two key beeps
02/20/04   pn      Replaced BTApp_AGEnabled() with BTDeviceSelected().
02/10/04   ram     Added reference count for set_device function.
01/28/04   ram     Add speaker-phone functionality.
12/16/03   BPW     Removed unused functions
12/09/03   ram     Removed lint errors.
11/23/03   sun     Renamed Functions
11/14/03   BPW     Added event for Dialer on ringer end
11/13/03   NNG     Added proper event for VT.
11/13/03   BPW     Removed complete event to be sent to Dialer
11/05/03   tml     Added bluetooth audio path support
11/05/03   NNG     Added support for VT.
11/03/03   kar     Fixed End key play tone
10/09/03   sun     Fixed Compile Error
10/02/03   BPW     Added sound end events to be sent to Dialer App
09/26/03   tml     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>

#ifndef TARGET_H
#include "target.h"
#endif

#ifndef COMDEF_H
#include "comdef.h"
#endif

#ifndef CUSTOMER_H
#include "customer.h"
#endif

#include "queue.h"

#include "msg.h"
#include "err.h"

#include "assert.h"
#include "hs.h"

#ifdef FEATURE_MULTIMEDIA
#include "cmx.h"
#endif

#include "cm.h"      //Should be changed to ICM
#include "tmc.h"
#include "uixsnd.h"

#ifdef FEATURE_UI_CORE
#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#ifndef USES_MMI
#include "CoreApp.BID"
#include "core_ui.h"
#include "CoreUserEvents.h"
#endif
#include "AppComFunc.h"
#ifndef USES_MMI
#ifdef FEATURE_APP_DIALER
#include "DialerApp.h"
#endif

#ifdef FEATURE_QVPHONE_APP
#include "QVIDEOPHONE.BID"
#endif /* FEATURE_QVPHONE_APP */
#endif
#endif /* FEATURE_UI_CORE */

#include "ui_base.h"
#include "uiutils.h"

#include "AEEConfig.h"
//#include "OEMTSGBridge.h"
//#include "uiutils.h"
#include "OEMNV.h"


//lint -save -e785
//lint -save -e528
/* <EJECT> */

//Handle sound devices in apps processor.
//If we're a modem processor, don't do anything here
#if !defined(FEATURE_UI_DUALPROC_MDM)
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static snd_device_type out_device;              /* normal out device */
static boolean out_device_autoselect = TRUE;    /* is the current device there by autoselection? */
static int num_speaker = 0;                     /* number of speaker users (0=muted) */
static int num_microphone = 0;                  /* number of microphone users (0=muted) */
#ifdef CUST_EDITION
static boolean force_mute = TRUE;               /* Whether we are in a state excluding the reference counters */
#else
static boolean force_mute = FALSE;              /* Whether we are in a state excluding the reference counters */
#endif
static snd_mute_control_type forced_spkr_mute_state = SND_MUTE_MUTED;
static snd_mute_control_type forced_mic_mute_state  = SND_MUTE_MUTED;
static boolean vibrator_on = FALSE;              /* True if the vibrator is on */
static rex_crit_sect_type uisnd_crit_sect;
static snd_sidetone_mode_type sidetone_setting = SND_SIDETONE_FORCE_DISABLED; // Default to ON every reset
static boolean enable_sound_mixing = FALSE;     /* status of music mixing while in voice call */
#ifdef CUST_EDITION
static clk_cb_type uisnd_vibrator_clk;    /* Timer to vibrating the phone */
#endif

typedef struct {
  snd_device_type          device;
  snd_method_type          method;
  uint8                    volume;
  uint8                    num_vol_levels_desired; //number of levels that the caller
                                                   // thinks there are.  0=no mapping
  snd_cb_func_ptr_type     callback_ptr;
  void                    *client_data;
} uisnd_set_vol_info_s_type;

typedef struct uisnd_notify_node_s {
  UISND_NOTIFY_FUNC         notify_func;
  void                     *user_data;
  struct uisnd_notify_node_s *next;
} uisnd_notify_node_s_type;

static uisnd_notify_node_s_type *uisnd_notify_list = NULL;

/* Mapping array from uisnd_ringer_type to snd_sound_id_type for the       */
/* ui ringer types uisnd_ringer_type. Make sure that the SND enum in the   */
/* array matches the expected ui ringer type */
snd_sound_id_type uisnd_ringer_snd_map[UISND_LAST_RING+1] = {
  SND_NORMAL_ALERT,  /* Normal alert                          */
  SND_RING_1,        /* User selectable ring 1                */
  SND_RING_2,        /* User selectable ring 2                */
  SND_RING_3,        /* User selectable ring 3                */
  SND_RING_4         /* User selectable ring 4                */
#ifdef FEATURE_ENHANCED_RINGS
  ,
  SND_RING_5,        /* User selectable ring 5                */
  SND_RING_6,        /* User selectable ring 6                */
  SND_RING_7,        /* User selectable ring 7                */
  SND_RING_8,        /* User selectable ring 8                */
  SND_RING_9         /* User selectable ring 9                */
#endif
};

typedef struct {
  snd_device_type device;
  boolean (*pAttachedFn)(void);
  uisnd_device_status device_status;
  boolean useAttachedFn;
  nv_items_enum_type nv_keybeep_level;
  nv_items_enum_type nv_earpiece_level;
  nv_items_enum_type nv_ringer_level;
  nv_items_enum_type nv_mm_level;
  nv_items_enum_type nv_message_level;
} uisnd_priority_type;

static uisnd_priority_type uisnd_device_info_table[] = {
#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_FM_OEM
  {SND_DEVICE_HEADSET_FM, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
  {SND_DEVICE_SPEAKER_FM, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
#endif
#ifdef FEATURE_TTY
  {SND_DEVICE_TTY_HEADSET, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_SHADOW_I},
  {SND_DEVICE_TTY_VCO, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_SHADOW_I},
  {SND_DEVICE_TTY_HCO, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_SHADOW_I},
#endif /* FEATURE_TTY */
  {SND_DEVICE_STEREO_HEADSET, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
  {SND_DEVICE_HEADSET, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
  {SND_DEVICE_HFK, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_SPKR_LVL_I, NV_SPEAKER_LVL_I, NV_RINGER_SPKR_LVL_I, NV_MM_SPEAKER_LVL_I, NV_ALERTS_LVL_I},
#ifdef FEATURE_USB_CARKIT
  {SND_DEVICE_STEREO_USB, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_SPKR_LVL_I, NV_SPEAKER_LVL_I, NV_RINGER_SPKR_LVL_I, NV_MM_SPEAKER_LVL_I, NV_ALERTS_LVL_I},
  {SND_DEVICE_USB, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_SPKR_LVL_I, NV_SPEAKER_LVL_I, NV_RINGER_SPKR_LVL_I, NV_MM_SPEAKER_LVL_I, NV_ALERTS_LVL_I},
#endif /* FEATURE_USB_CARKIT */
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  {SND_DEVICE_SDAC, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_SDAC_LVL_I, NV_SDAC_LVL_I, NV_RINGER_LVL_I, NV_MM_SDAC_LVL_I, NV_ALERTS_LVL_I},
#endif
#ifdef FEATURE_BT_AG
  {SND_DEVICE_BT_HEADSET, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I,  NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
#endif /* FEATURE_BT_AG */
#ifdef FEATURE_BT_EXTPF_AV
  {SND_DEVICE_BT_A2DP_HEADSET, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I,  NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
#endif /* FEATURE_BT_EXTPF_AV */
#ifdef FEATURE_SPEAKER_PHONE
  // GetSpeakerPhone says that the speakerphone was activated by
  // pressing the button.  That bumps it up in priority.
  // It is always attached (see later entry) for the purposes of
  // uisnd_is_device_attached()
  {SND_DEVICE_SPEAKER_PHONE, NULL, UISND_DEV_DISABLED, FALSE,
   NV_BEEP_LVL_I,  NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_I},
#endif
#endif //!FEATURE_UI_CORE_REMOVED
  {SND_DEVICE_HANDSET, NULL, UISND_DEV_ENABLED, FALSE,
   NV_BEEP_LVL_I,  NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_I},
  //Devices below the first NULL are only for the purposes of
  //uisnd_is_device_attached()
  {SND_DEVICE_CURRENT, NULL, UISND_DEV_ENABLED, FALSE,
   NV_BEEP_LVL_I,  NV_EAR_LVL_I, NV_RINGER_LVL_I, NV_MM_LVL_I, NV_ALERTS_LVL_I},
#ifndef FEATURE_UI_CORE_REMOVED
#ifdef FEATURE_ANALOG_HFK
  {SND_DEVICE_AHFK, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_SPKR_LVL_I, NV_SPEAKER_LVL_I, NV_RINGER_SPKR_LVL_I, NV_MM_SPEAKER_LVL_I, NV_ALERTS_LVL_I},
#endif
#ifdef FEATURE_BT_AG
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
  (SND_DEVICE_BT_AG_LOCAL_AUDIO, NULL, UISND_DEV_UNATTACHED, FALSE,
   NV_BEEP_LVL_SHADOW_I, NV_EAR_LVL_SHADOW_I, NV_RINGER_LVL_I, NV_MM_LVL_SHADOW_I, NV_ALERTS_LVL_SHADOW_I},
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#endif //!FEATURE_UI_CORE_REMOVED
};

static void uisnd_notify(uisnd_notify_data_s_type *notify_data);


#ifndef FEATURE_UI_CORE_REMOVED
/* <EJECT> */
/*===========================================================================
FUNCTION UISND_KEY_SOUND

DESCRIPTION
  Do the beep associated with a key being pressed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void uisnd_key_sound(
  word                  key,          /* which key */
  word                  wtime,        /* duration in ms */
  snd_cb_func_ptr_type  callback_ptr, /* Call back function */
  const void            *client_data  /* Pointer to Client data */
)
{
  MSG_MED("Starting Key Sounds %d %d", key, wtime, 0);
  /* Choose the right beep for the right key */
  switch( key ) {

    case HS_POUND_K:
    case HS_STAR_K:
    case HS_0_K:        /* All these can be transmitted */
    case HS_1_K:        /* over the air, or not, so they */
    case HS_2_K:        /* take special processing */
    case HS_3_K:
    case HS_4_K:
    case HS_5_K:
    case HS_6_K:
    case HS_7_K:
    case HS_8_K:
    case HS_9_K:
      {
        snd_tone_type tone;

        if( key == (word)HS_POUND_K ) {
          tone = SND_POUND; /* and sound */
        }  else {
          if( key == (word)HS_STAR_K ) {
            tone = SND_STAR;
          }  else {
      /* Assumes tones are in order */
            tone = (snd_tone_type)(key-(word)HS_0_K+(word)SND_0) ;
          }
        }

        /* Play it locally - dialer will Tx OTA */
        uisnd_tone_callback( UI_BEEP_SND, tone, wtime, callback_ptr, client_data );
      }
      break;
    case HS_END_K:
    case HS_PWR_K:
    case HS_SEND_K:     /* Control keys which are never transmitted */
      uisnd_tone_callback( UI_BEEP_SND, SND_CTRL, wtime, callback_ptr, client_data );
      break;

#ifdef FEATURE_NAV_KEYPAD
    case HS_UP_K:
    case HS_DOWN_K:
    case HS_CLR_K:
    case HS_LEFT_K:
    case HS_RIGHT_K:
    case HS_SEL_K:
      uisnd_tone_callback( UI_BEEP_SND, SND_2ND, wtime, callback_ptr, client_data );
      break ;
#endif

    default:            /* Anything else - never transmitted */
      uisnd_tone_callback( UI_BEEP_SND, SND_2ND, wtime, callback_ptr, client_data );
  }   /* switch key */
}
#endif /* !FEATURE_UI_CORE_REMOVED */

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_set_volumes_for_sound_type

DESCRIPTION
  maps a sound type to various sound devices and sets their volume levels.

DEPENDENCIES
  static table uisnd_device_info_table[]

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void uisnd_set_volumes_for_sound_type ( uisnd_sound_volume_type sound_type,
            nv_items_enum_type nv_item,
            snd_method_type method_type,
            byte vol)
{
  unsigned int i;
  switch (sound_type) {
  case UISND_KEY_BEEP_SND:
    for(i = 0; i< ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_device_info_table[i].nv_keybeep_level == nv_item) {
        uisnd_set_volume_scaled(uisnd_device_info_table[i].device,
                                method_type,
                                vol, NULL, NULL);
      }
    }
    break;
  case UISND_EAR_SND:
    for(i = 0; i< ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_device_info_table[i].nv_earpiece_level == nv_item) {
        uisnd_set_volume_scaled(uisnd_device_info_table[i].device,
                                method_type,
                                vol, NULL, NULL);
      }
    }
    break;
  case UISND_RINGER_SND:
    for(i = 0; i< ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_device_info_table[i].nv_ringer_level == nv_item) {
        uisnd_set_volume_scaled(uisnd_device_info_table[i].device,
                                method_type,
                                vol, NULL, NULL);
      }
    }
    break;
  case UISND_MM_SND:
    for(i = 0; i< ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_device_info_table[i].nv_mm_level == nv_item) {
        uisnd_set_volume_scaled(uisnd_device_info_table[i].device,
                                method_type,
                                vol, NULL, NULL);
      }
    }
    break;
  case UISND_MSG_SND:
    for(i = 0; i< ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_device_info_table[i].nv_message_level == nv_item) {
        uisnd_set_volume_scaled(uisnd_device_info_table[i].device,
                                method_type,
                                vol, NULL, NULL);
      }
    }
    break;
  default:
    MSG_ERROR("Error: Unknown sound type %d",sound_type,0,0);
    break;
  }
}

             /* <EJECT> */
/*===========================================================================
FUNCTION uisnd_map_device_to_nv_item

DESCRIPTION
  maps a sound device to an NV item using uisnd_device_info_table[]

DEPENDENCIES
  static table uisnd_device_info_table[]

RETURNS
  nv_items_enum_type: corresponding to sound device and sound type

SIDE EFFECTS
  None
===========================================================================*/
nv_items_enum_type uisnd_map_device_to_nv_item ( snd_device_type out_dev,
             uisnd_sound_volume_type sound_type)
{
  unsigned int i;
  int index = -1;
  nv_items_enum_type ret_val;
  for (i=0; (i< ARR_SIZE(uisnd_device_info_table) && index == -1); i++) {
    if (uisnd_device_info_table[i].device == out_dev) /* device found */
      index = i;
  }
  switch(sound_type) {
  case UISND_MM_SND:
    ret_val = (index == -1)?(NV_MM_LVL_I):(uisnd_device_info_table[index].nv_mm_level);
    break;
  case UISND_KEY_BEEP_SND:
    ret_val = (index == -1)?(NV_BEEP_LVL_I):(uisnd_device_info_table[index].nv_keybeep_level);
    break;
  case UISND_EAR_SND:
    ret_val = (index == -1)?(NV_EAR_LVL_I):(uisnd_device_info_table[index].nv_earpiece_level);
    break;
  case UISND_RINGER_SND:
    ret_val = (index == -1)?(NV_RINGER_LVL_I):(uisnd_device_info_table[index].nv_ringer_level);
    break;
  case UISND_MSG_SND:
    ret_val = (index == -1)?(NV_ALERTS_LVL_I):(uisnd_device_info_table[index].nv_message_level);
    break;
  default:
    MSG_ERROR("Error: Unknown sound type %d",sound_type,0,0);
    ret_val = NV_MAX_I;
    break;
  }
  return ret_val;
}

/* <EJECT> */
/*===========================================================================
FUNCTION CHOOSE_DEVICE

DESCRIPTION
  Choose the correct output device

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
static snd_device_type choose_device( uisnd_snd_type type ) {
  snd_device_type rval;

  switch( type ) {
    case UI_BEEP_SND:
    case UI_ALERT_SND:
    case UI_EARBEEP_SND:
    case UI_MSG_SND:
    case UI_CALL_SND:
    case UI_CALL_EAR_SND:
    case UI_DTMF_SND:
    case UI_SVC_ALERT_SND:
    case UI_EXT_PWR_SND:
    case UI_PWRUP_SND:
      rval = SND_DEVICE_CURRENT;  /* use current device */
      break;
    default:
      ERR( "Bad sound type in choose_device type = %d", type, 0, 0 );
      rval = SND_DEVICE_HANDSET;
      break ;
  }

  return( rval );
}
/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_is_to_be_silenced

DESCRIPTION
  Determine whether this sound method is to be silenced with
  silenceall set

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
static boolean uisnd_is_to_be_silenced( snd_method_type type )
{
#ifndef USES_MMI
#ifndef FEATURE_UI_CORE_REMOVED
#ifndef FEATURE_MANGO_UI
  if(CoreTask_GetSilenceAll())
    return ((type == SND_METHOD_VOICE) ? FALSE : TRUE);
  else
#endif
#endif
#endif
    return FALSE;
}
/* <EJECT> */
/*===========================================================================
FUNCTION CHOOSE_METHOD

DESCRIPTION
  Choose the correct sound method

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
static snd_method_type choose_method( uisnd_snd_type type )
{

  snd_method_type method;                      /* return value */

  switch( type )
  {

    case UI_BEEP_SND:             /* key beep */
        /* We will use the keybeep method for the specified device.       */
        method = SND_METHOD_KEY_BEEP ;
        break ;

    case UI_PWRUP_SND:            /* power up beep */
    case UI_EXT_PWR_SND:
      /* We will use the message method whether we are in a call or not.  */
      method = SND_METHOD_MESSAGE ;
      break;

    case UI_ALERT_SND:
      /* We will use the ring method whether we are in a call or not.     */
      method = SND_METHOD_RING ;
      break;

    case UI_MSG_SND:
    case UI_EARBEEP_SND:
    case UI_CALL_SND:
      if (( !ui_is_in_call() ) || (ui_curr_call_is_sms())) { /* SMS6 */
        /* We will use the ring method when we are not in a call          */
        if (type == UI_MSG_SND) {
          method = SND_METHOD_RING;
        }
        else {
          method = SND_METHOD_KEY_BEEP ;
        }
      }
      else
      {
        /* We will use the voice method ( earpiece or speaker ) for the   */
        /* specified device.                                              */
        method = SND_METHOD_VOICE ;
      }
      break;

    case UI_SVC_ALERT_SND:
      if (ui_is_in_call()) {
        method = SND_METHOD_VOICE ;
      }
      else {
        method = SND_METHOD_MESSAGE ;
      }
      break;

    case UI_CALL_EAR_SND:
      /* If we are not in a data call :                                   */
      /* We will use the voice method ( earpiece or speaker ) for the     */
      /* specified device.                                                */
      method = SND_METHOD_VOICE ;
      break;

    case UI_DTMF_SND:
      /* This type is used only by uisnd_dtmf which is only called during   */
      /* a call. SND_METHOD_VOICE will be used                              */
      method = SND_METHOD_VOICE ;
      break ;

    default:
      ERR( "Bad sound type %d: Using SND_METHOD_KEY_BEEP", type, 0, 0 );
      method = SND_METHOD_KEY_BEEP;
      break;

  }
  return( method );
}


/*===========================================================================
FUNCTION UISND_BEEP_VOL

DESCRIPTION
  Set the beep volume.

DEPENDENCIES
  None.
===========================================================================*/
void uisnd_beep_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
)
{
  /* update our settings */
  switch ( nv_item ) {
  case NV_BEEP_LVL_I :
  case NV_BEEP_SPKR_LVL_I :
  case NV_BEEP_LVL_SHADOW_I:
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  case NV_BEEP_SDAC_LVL_I:
#endif
    uisnd_set_volumes_for_sound_type(UISND_KEY_BEEP_SND, nv_item, SND_METHOD_KEY_BEEP,
                                 vol);
    break;
  default :
    break ;
  }
}
/*===========================================================================
FUNCTION UISND_ALERT_VOL

DESCRIPTION
  Set the SMS alert volume.
  SMS Volume
DEPENDENCIES
  None.
===========================================================================*/
void uisnd_alert_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
)
{
  /* update our settings */
  switch ( nv_item ) {
  case NV_ALERTS_LVL_I:
  case NV_ALERTS_LVL_SHADOW_I:
    uisnd_set_volumes_for_sound_type( UISND_MSG_SND, nv_item, SND_METHOD_MESSAGE,
                                  vol);
    break ;
  default :
    break ;
  }
}
/*===========================================================================
FUNCTION UISND_VOICE_VOL

DESCRIPTION
  Set the voice volume. Then also tell sound task.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_voice_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
)
{

  /* update our settings */
  switch ( nv_item ) {
  case NV_EAR_LVL_I :
  case NV_SPEAKER_LVL_I :
  case NV_EAR_LVL_SHADOW_I:
#if defined(FEATURE_STEREO_DAC) || defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  case NV_SDAC_LVL_I:
#endif
    uisnd_set_volumes_for_sound_type(UISND_EAR_SND, nv_item, SND_METHOD_VOICE,
                                 vol);
    break;
  default :
    break ;
  }
}


#ifdef FEATURE_TTY
/*===========================================================================
FUNCTION UISND_TTY_VOL

DESCRIPTION
  Set the TTY device volume. Then also tell sound task.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tty_vol(
  byte vol,                      /* the volume           */
  snd_device_type tty_dev        /* the TTY device type  */
)
{
  /* update our settings */
  switch ( tty_dev )
  {
    case SND_DEVICE_TTY_HEADSET:
      uisnd_set_volume_scaled(SND_DEVICE_TTY_HEADSET, SND_METHOD_VOICE, vol, NULL, NULL);
      break;
    case SND_DEVICE_TTY_HCO:
      uisnd_set_volume_scaled(SND_DEVICE_TTY_HCO, SND_METHOD_VOICE, vol, NULL, NULL);
      break;
    case SND_DEVICE_TTY_VCO:
      uisnd_set_volume_scaled(SND_DEVICE_TTY_VCO, SND_METHOD_VOICE, vol, NULL, NULL);
      break;
    default :
      break ;
  }
}
#endif /* FEATURE_TTY */

#ifdef FEATURE_MULTIMEDIA
/*===========================================================================
FUNCTION UISND_MM_VOL

DESCRIPTION
  Set the Multimedia volume. Then also tell sound task.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_mm_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
)
{

  /* update our settings */
  switch ( nv_item ) {
  case NV_MM_LVL_I:
  case NV_MM_LVL_SHADOW_I:
  case NV_MM_SPEAKER_LVL_I:
  case NV_MM_SDAC_LVL_I:
    uisnd_set_volumes_for_sound_type(UISND_MM_SND, nv_item, SND_METHOD_MIDI,
                                 vol);
    break;
  default :
    break ;
  }
}

#endif /* FEATURE_MULTIMEDIA */



/* <EJECT> */
/*===========================================================================
FUNCTION UISND_RING_VOL

DESCRIPTION
  Set the ring volume, and tell sound task also.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_ring_vol(
  byte vol,                        /* the volume                    */
  nv_items_enum_type nv_item       /* nv item indicating the device */
)
{
  /* update our settings */
  switch ( nv_item ) {
  case NV_RINGER_LVL_I:
  case NV_RINGER_SPKR_LVL_I:
    uisnd_set_volumes_for_sound_type(UISND_RINGER_SND, nv_item, SND_METHOD_RING,
                                 vol);
    break ;
  default :
    break ;
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION UISND_FORCE_DEVICE_AUTOSELECT

DESCRIPTION
  Force uisnd_set_device_auto() back to autoselect mode

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_force_device_autoselect(void)
{
  out_device_autoselect = TRUE;
}


/*===========================================================================

FUNCTION uisnd_set_device_cb

DESCRIPTION
  This function is the default callback when we call snd_set_device.
  If we would give that function a NULL callback, this function is
  called to at least log a problem if we received an error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_set_device_cb
             ( const void      *client_data, /* pointer to Client data   */
               snd_status_type status )      /* Status returned by Sound */
{
  static uint32 uisnd_set_device_errors = 0;
  if (status != SND_SUCCESS)
  {
    uisnd_set_device_errors++;
    ERR("snd_set_device returned error %d - status %x for client data %x",
        uisnd_set_device_errors, status, client_data);
  }
}

/*===========================================================================

FUNCTION uisnd_set_device

DESCRIPTION
  Set device and muting for the voice path, or control muting for the
  current device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_set_device
(
  snd_device_type       device,
    /* The New device Type the caller wants */
  snd_mute_control_type ear_mute,
    /* Mute, unmute output                  */
  snd_mute_control_type mic_mute,
    /* Mute, unmute microphone              */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  if (callback_ptr == NULL)
  {
    callback_ptr = uisnd_set_device_cb;
    client_data = (void *) ((((uint32) device & 0xFFFF) << 16) |
                            (((uint32) ear_mute & 0xFF) << 8) |
                            (((uint32) mic_mute & 0xFF) << 0));
  }

  snd_set_device( device, ear_mute, mic_mute, callback_ptr, client_data);
}

/*===========================================================================
FUNCTION UISND_RESTORE_MUTE

DESCRIPTION
  Restore mute settings

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_restore_mute(
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
)
{
  snd_mute_control_type speaker_mute_s    = SND_MUTE_MUTED;
  snd_mute_control_type microphone_mute_s = SND_MUTE_MUTED;
  uisnd_notify_data_s_type notify_data;

  /* force mute is for compatibility with BREW semantics */
  if (force_mute) {
    speaker_mute_s = forced_spkr_mute_state;
    microphone_mute_s = forced_mic_mute_state;
  } else {
    if(num_speaker)
      speaker_mute_s = SND_MUTE_UNMUTED;

    if(num_microphone)
      microphone_mute_s = SND_MUTE_UNMUTED;
  }

  /* Finally call SND with the right device and mute status */
  //Call sound so that callbacks happen as expected
  MSG_MED("uisnd_restore_mute dev: %d spkr_mute: %d mic_mute: %d",
          out_device, speaker_mute_s, microphone_mute_s );
  MSG_MED("num_spkr: %d num_mic: %d force_mute: %d",
          num_speaker, num_microphone, force_mute );
  uisnd_set_device( out_device,speaker_mute_s,
                  microphone_mute_s, callback_ptr, client_data);

  //send notification
  notify_data.out_device = out_device;
  notify_data.speaker_mute = speaker_mute_s;
  notify_data.microphone_mute = microphone_mute_s;

  uisnd_notify(&notify_data);
}


/*===========================================================================
FUNCTION UISND_SET_DEVICE_AUTO

DESCRIPTION
  Set the normal input and output device.
  If a specific device has been set through UISND_SET_DEVICE_SPECIFIC,
  that device is used, unless it has been removed.  Otherwise, the highest
  priority attached device is used.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_set_device_auto(
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
)
{
  snd_mute_control_type speaker_mute_s    = SND_MUTE_MUTED;
  snd_mute_control_type microphone_mute_s = SND_MUTE_MUTED;
  uisnd_notify_data_s_type notify_data;
  unsigned int i;

  /* force mute is for compatibility with BREW semantics */
  if (force_mute) {
    speaker_mute_s = forced_spkr_mute_state;
    microphone_mute_s = forced_mic_mute_state;
  } else {
    if(num_speaker)
      speaker_mute_s = SND_MUTE_UNMUTED;

    if(num_microphone)
      microphone_mute_s = SND_MUTE_UNMUTED;
  }

  if (!out_device_autoselect) {
    //specific device selected.  Lets make sure it's still here.
    if (!uisnd_is_device_attached(out_device)) {
      out_device_autoselect = TRUE;
    } else {
      out_device_autoselect = FALSE;
    }
  }

  if (out_device_autoselect) {
    for (i=0; i<ARR_SIZE(uisnd_device_info_table); i++) {
      if (uisnd_is_device_enabled(uisnd_device_info_table[i].device)) {
          //Device is attached and enabled
          out_device = uisnd_device_info_table[i].device;
          break;
      }
    }

    /* Finally call SND with the right device and mute status */
    uisnd_set_device( out_device,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  } else {
    /* Finally call SND with the right device and mute status */
    //Call sound so that callbacks happen as expected
    uisnd_set_device( SND_DEVICE_CURRENT,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  }

  MSG_MED("uisnd_set_device_auto dev: %d spkr_mute: %d mic_mute: %d",
          (out_device_autoselect ? out_device : SND_DEVICE_CURRENT),
          speaker_mute_s, microphone_mute_s );
  MSG_MED("num_spkr: %d num_mic: %d force_mute: %d",
          num_speaker, num_microphone, force_mute );

  //send notification
  notify_data.out_device = out_device;
  notify_data.speaker_mute = speaker_mute_s;
  notify_data.microphone_mute = microphone_mute_s;

  uisnd_notify(&notify_data);
}
/*===========================================================================
FUNCTION UISND_SET_DEVICE_SPECIFIC

DESCRIPTION
  Set the normal input and output device, to something specific.
  This setting remains until the requested device is removed.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_set_device_specific(
  snd_device_type       new_device,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
)
{
  snd_mute_control_type speaker_mute_s    = SND_MUTE_MUTED;
  snd_mute_control_type microphone_mute_s = SND_MUTE_MUTED;
  uisnd_notify_data_s_type notify_data;
  boolean found = FALSE;

  if (force_mute) {
    speaker_mute_s = forced_spkr_mute_state;
    microphone_mute_s = forced_mic_mute_state;
  } else {
    if(num_speaker)
      speaker_mute_s = SND_MUTE_UNMUTED;

    if(num_microphone)
      microphone_mute_s = SND_MUTE_UNMUTED;
  }


  //try to find the new device
  if (uisnd_is_device_attached(new_device)) {
    out_device = new_device;
    out_device_autoselect = FALSE;
    found = TRUE;
  }

  MSG_MED("uisnd_set_device_specific dev: %d spkr_mute: %d mic_mute: %d",
          (found ? out_device : SND_DEVICE_CURRENT),
          speaker_mute_s, microphone_mute_s );
  MSG_MED("num_spkr: %d num_mic: %d force_mute: %d",
          num_speaker, num_microphone, force_mute );

  //if we found the device, call with new device, else call w/current
  if (found) {
    /* Finally call SND with the right device and mute status */
    uisnd_set_device( out_device,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  } else {
    /* Finally call SND with the right device and mute status */
    uisnd_set_device( SND_DEVICE_CURRENT,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  }

  //send notification
  notify_data.out_device = out_device;
  notify_data.speaker_mute = speaker_mute_s;
  notify_data.microphone_mute = microphone_mute_s;

  uisnd_notify(&notify_data);

  return found;
}
/*===========================================================================
FUNCTION UISND_SET_MUTE

DESCRIPTION
  Set the mute settings.  The unmute from this function is reference counted.
  This function can also be used to "unforce" the forced mute settings.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_set_mute(
  uisnd_mute_control_type speaker_mute,
  uisnd_mute_control_type microphone_mute,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
)
{
  snd_mute_control_type speaker_mute_s    = SND_MUTE_MUTED;
  snd_mute_control_type microphone_mute_s = SND_MUTE_MUTED;
  uisnd_notify_data_s_type notify_data;

  force_mute = FALSE;

  switch (speaker_mute) {
  case UISND_MUTE_UNMUTED:
    num_speaker++;
    break;

  case UISND_MUTE_MUTED:
    if(num_speaker)
      num_speaker--;
    break;

  default:
    break;
  }

  switch (microphone_mute) {
  case UISND_MUTE_UNMUTED:
    num_microphone++;
    break;

  case UISND_MUTE_MUTED:
    if(num_microphone)
      num_microphone--;
    break;

  default:
    break;
  }


  if(num_speaker)
    speaker_mute_s = SND_MUTE_UNMUTED;

  if(num_microphone)
    microphone_mute_s = SND_MUTE_UNMUTED;

  MSG_MED("uisnd_set_mute dev: %d spkr_mute: %d mic_mute: %d",
          SND_DEVICE_CURRENT, speaker_mute_s, microphone_mute_s );
  MSG_MED("num_spkr: %d num_mic: %d force_mute: %d",
          num_speaker, num_microphone, force_mute );

  /* Finally call SND with the right device and mute status */
  uisnd_set_device( SND_DEVICE_CURRENT,speaker_mute_s,
                  microphone_mute_s, callback_ptr, client_data);

  //send notification
  notify_data.out_device = out_device;
  notify_data.speaker_mute = speaker_mute_s;
  notify_data.microphone_mute = microphone_mute_s;

  uisnd_notify(&notify_data);
}

/*===========================================================================
FUNCTION UISND_FORCE_DEVICE_MUTE

DESCRIPTION
  Force the device and mute settings.  The unmute from this function
  is NOT reference counted.  To go back to the reference counted mute, you
  need to call uisnd_set_mute().  Sound device handling is the same as
  uisnd_set_device-specific()

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_force_device_mute(
  snd_device_type new_device,
  uisnd_mute_control_type speaker_mute,
  uisnd_mute_control_type microphone_mute,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data

)
{
  snd_mute_control_type speaker_mute_s    = forced_spkr_mute_state;
  snd_mute_control_type microphone_mute_s = forced_mic_mute_state;
  uisnd_notify_data_s_type notify_data;
  boolean found = FALSE;


  /* Handle device */

  if (new_device != SND_DEVICE_CURRENT)
  {
    //try to find the new device
    if (uisnd_is_device_attached(new_device)) {
      out_device = new_device;
#ifndef CUST_EDITION
      out_device_autoselect = FALSE;
#endif
      found = TRUE;
    }
  }

  /* now handle mute */

  /* we need to know if we were in a forced state, so we can properly handle
     the case of not changing when forcing for the first time */
  if (force_mute) {
    /* we were forced before, so go for it */
    /* don't handle no change, because var is already valid */
    switch (speaker_mute) {
    case UISND_MUTE_MUTED:
      speaker_mute_s = SND_MUTE_MUTED;
      break;

    case UISND_MUTE_UNMUTED:
      speaker_mute_s = SND_MUTE_UNMUTED;
      break;

    default:
      break;
    }
    switch (microphone_mute) {
    case UISND_MUTE_MUTED:
      microphone_mute_s = SND_MUTE_MUTED;
      break;

    case UISND_MUTE_UNMUTED:
      microphone_mute_s = SND_MUTE_UNMUTED;
      break;

    default:
      break;
    }
  } else {
    /* we weren't forced before, so properly handle the no change cases */
    switch (speaker_mute) {
    case UISND_MUTE_MUTED:
      speaker_mute_s = SND_MUTE_MUTED;
      break;

    case UISND_MUTE_UNMUTED:
      speaker_mute_s = SND_MUTE_UNMUTED;
      break;

    case UISND_MUTE_NO_CHANGE:
      if(num_speaker)
        speaker_mute_s = SND_MUTE_UNMUTED;
      else
        speaker_mute_s = SND_MUTE_MUTED;
      break;

    default:
      break;
    }
    switch (microphone_mute)
    {
    case UISND_MUTE_MUTED:
      microphone_mute_s = SND_MUTE_MUTED;
      break;

    case UISND_MUTE_UNMUTED:
      microphone_mute_s = SND_MUTE_UNMUTED;
      break;

    case UISND_MUTE_NO_CHANGE:
      if(num_microphone)
        microphone_mute_s = SND_MUTE_UNMUTED;
      else
        microphone_mute_s = SND_MUTE_MUTED;
      break;

    default:
      break;
    }

  }

  forced_spkr_mute_state = speaker_mute_s;
  forced_mic_mute_state = microphone_mute_s;
  force_mute = TRUE;

  MSG_MED("uisnd_force_device_mute dev: %d spkr_mute: %d mic_mute: %d",
          (found ? out_device : SND_DEVICE_CURRENT),
          speaker_mute_s, microphone_mute_s );
  MSG_MED("num_spkr: %d num_mic: %d force_mute: %d",
          num_speaker, num_microphone, force_mute );

  //if we found the device, call with new device, else call w/current
  if (found) {
    /* Finally call SND with the right device and mute status */
    uisnd_set_device( out_device,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  } else {
    /* Finally call SND with the right device and mute status */
    uisnd_set_device( SND_DEVICE_CURRENT,speaker_mute_s,
                    microphone_mute_s, callback_ptr, client_data);
  }

  //send notification
  notify_data.out_device = out_device;
  notify_data.speaker_mute = speaker_mute_s;
  notify_data.microphone_mute = microphone_mute_s;

  uisnd_notify(&notify_data);

  return found;
}

/* <EJECT> */

/*===========================================================================
FUNCTION UISND_IS_DEVICE_ATTACHED

DESCRIPTION
  See if the given device is attached.  Returns TRUE for yes, FALSE for no.

RETURN VALUE
  TRUE if device is attached.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_is_device_attached(
  snd_device_type       device
)
{
  unsigned int i = 0;

  //try to find the device
  for (i=0; i<ARR_SIZE(uisnd_device_info_table); i++) {
    if (uisnd_device_info_table[i].device == device) {
      //we found the device.  See if it's attached.

      if (uisnd_device_info_table[i].useAttachedFn)
      {
        if ((uisnd_device_info_table[i].pAttachedFn != NULL
             && uisnd_device_info_table[i].pAttachedFn()) ||
            (uisnd_device_info_table[i].pAttachedFn == NULL))
          return TRUE; //always attached if pAttachedFn = NULL
      }
      else
      {
        if (uisnd_device_info_table[i].device_status != UISND_DEV_UNATTACHED)
          return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION UISND_IS_DEVICE_ENABLED

DESCRIPTION
  See if the given device is enbled.  Returns TRUE for yes, FALSE for no.

RETURN VALUE
  TRUE if device is enbled.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_is_device_enabled(
  snd_device_type       device
)
{
  unsigned int i = 0;

  //try to find the device
  for (i=0; i<ARR_SIZE(uisnd_device_info_table); i++) {
    if (uisnd_device_info_table[i].device == device) {
      //we found the device.  See if it's enabled.
      if (uisnd_device_info_table[i].useAttachedFn)
      {
        if ((uisnd_device_info_table[i].pAttachedFn != NULL
             && uisnd_device_info_table[i].pAttachedFn()) ||
            (uisnd_device_info_table[i].pAttachedFn == NULL))
          return TRUE; //always attached if pAttachedFn = NULL
      }
      else
      {
        if (uisnd_device_info_table[i].device_status == UISND_DEV_ENABLED)
          return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION UISND_SET_DEVICE_STATUS

DESCRIPTION
  Set a sound device status.

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void uisnd_set_device_status(
  snd_device_type       device,
  uisnd_device_status new_value
)
{
  unsigned int i = 0;

  //try to find the device
  for (i=0; i<ARR_SIZE(uisnd_device_info_table); i++) {
    if (uisnd_device_info_table[i].device == device) {
      //we found the device.  Set it's status.
      uisnd_device_info_table[i].device_status = new_value;
      return;
    }
  }
}

/*===========================================================================
FUNCTION UISND_GET_DEVICE

DESCRIPTION
  Get the current output device, and mute settings

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_get_device(uisnd_notify_data_s_type *info)
{
  if (info == NULL)
    return;

  info->out_device = out_device;

  if (force_mute) {
    info->speaker_mute = forced_spkr_mute_state;
    info->microphone_mute = forced_mic_mute_state;
  } else {
    if(num_speaker)
      info->speaker_mute = SND_MUTE_UNMUTED;
    else
      info->speaker_mute = SND_MUTE_MUTED;

    if(num_microphone)
      info->microphone_mute = SND_MUTE_UNMUTED;
    else
      info->microphone_mute = SND_MUTE_MUTED;
  }
}

/* <EJECT> */
/*==========================================================================
FUNCTION UISND_CB_FUNCTION3

DESCRIPTION
  This function performs the call back called from sound server indicating
  status and results from the requested command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A context switch might occurs
===========================================================================*/

void uisnd_cb_function3( void * client_data, snd_status_type status )
/*lint -esym(715,client_data)*/
{
  if (status == SND_REPEAT) {
#ifndef USES_MMI
#ifndef FEATURE_UIONE_HDK
#ifdef FEATURE_UI_CORE
    IShell *shell_ptr = AEE_GetShell();
    if (shell_ptr != NULL) {
      ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER,
                       EVT_RING_COMPLETE, 0);
    }
#endif /* FEATURE_UI_CORE */
#endif /* FEATURE_UIONE_HDK */
#endif
  }
                /* Cannot use rex_self. This routine runs under Sound task */
}

/* <EJECT> */

/*===========================================================================
FUNCTION uisnd_SND

DESCRIPTION
  Play a sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_snd(
  uisnd_snd_type      type,         /* type of sound */
  snd_sound_id_type   snd           /* which sound */
)
{
#ifdef FEATURE_AUDIO_FORMAT
  snd_method_type    method;
  snd_priority_type  priority;

  method = choose_method(type);
  uisnd_set_volume(choose_device(type),method,100,NULL,NULL);
  if(!uisnd_is_to_be_silenced(method))
  {
    MSG_MED("Starting Sounds %d %d", type, snd, 0);
    if ( type == UI_EXT_PWR_SND )
    {
       /* Had to set this to SND_PRIO_HIGH.  If set to LOW, it */
       /* will interrupt and stop audio file playback.  If set */
       /* to MED, it will interrupt and stop ringer playback.  */
       /* That leaves only HIGH, which may interfere with      */
       /* sustained DTMF.  This occurs when the external power */
       /* is plugged in or unplugged.                          */
       priority = SND_PRIO_HIGH;
    }
    else
    {
       priority = SND_PRIO_MED;
    }

    snd_sound_id_start( choose_device(type), method,
                        snd, priority, SND_APATH_LOCAL,
                        NULL, NULL);
  }
  else
  {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, snd, 0);
  }
#else /* FEATURE_AUDIO_FORMAT */
  MSG_HIGH("Sound Suppressed due to no FEATURE_AUDIO_FORMAT", 0, 0, 0);
#endif /* FEATURE_AUDIO_FORMAT */
}

#ifdef FEATURE_QVPHONE_APP
/*==========================================================================

FUNCTION uisnd_qvphone_cb

DESCRIPTION
  This function performs the call back called from sound server indicating
  status and results from the requested command.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void uisnd_qvphone_cb (void *client_data, snd_status_type status)
{
   if (status == SND_REPEAT) {
     IShell *shell_ptr = AEE_GetShell();
     if (shell_ptr != NULL) {
       ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_QVIDEOPHONE,
                          EVT_USER, UISND_VT_RING_COMPLETE, 0);
     }
   }
}

/*===========================================================================
FUNCTION uisnd_ring

DESCRIPTION
  Play a sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_ring(
  uisnd_snd_type      type,         /* type of sound */
  snd_sound_id_type   snd           /* which sound */
)
{
  snd_method_type method;
  method = choose_method(type);
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d %d", type, snd, 0);
 #ifdef FEATURE_AUDIO_FORMAT
    if ( type == UI_ALERT_SND) {
      snd_sound_id_start( choose_device(type), method,
                          snd, SND_PRIO_MED, SND_APATH_LOCAL,
                          (snd_cb_func_ptr_type)uisnd_qvphone_cb, NULL);
    } else {
      snd_sound_id_start( choose_device(type), method,
                          snd, SND_PRIO_MED, SND_APATH_LOCAL,
                          NULL, NULL);
    }
#endif
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, snd, 0);
  }
}
#endif /* FEATURE_QVPHONE_APP */

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_sample_snd

DESCRIPTION
  Play a sample sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_sample_snd(
  uisnd_snd_type    type,         /* type of sound */
  snd_sound_id_type snd,          /* which sound */
  byte              vol
)
{
  snd_method_type method;
  method = choose_method(type);
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d %d", type, snd, 0);
  if( type == UI_ALERT_SND ) {
      snd_sample_sound_id_start(choose_device(type), method, snd, vol, 1,
                        (snd_cb_func_ptr_type)uisnd_cb_function3, NULL);
  } else {
      snd_sample_sound_id_start(choose_device(type), method, snd, vol, 1,
                        NULL, NULL);
  }
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, snd, 0);
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_SND_STOP

DESCRIPTION
  Stop any sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up.
===========================================================================*/
void uisnd_snd_stop( void )
{
#ifdef FEATURE_AUDIO_FORMAT
  snd_priority_type pri = SND_PRIO_MED;
#ifndef USES_MMI
#ifdef FEATURE_UI_CORE
#ifndef FEATURE_MANGO_UI
  if (CoreTask_GetDemoAlert())
    pri = SND_PRIO_LOW;
#endif
#endif /* FEATURE_UI_CORE */
#endif
  MSG_MED ("snd_stop_sound", 0, 0, 0);
  snd_sound_stop(pri, NULL, NULL);

#else /* FEATURE_AUDIO_FORMAT */
  MSG_HIGH("Sound Suppressed due to no FEATURE_AUDIO_FORMAT", 0, 0, 0);
#endif /* FEATURE_AUDIO_FORMAT */
}

/* <EJECT> */
/*==========================================================================

FUNCTION UISND_CB_FUNCTION2

DESCRIPTION
  This function performs the call back called from sound server indicating
  status and results from the requested command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A context switch might occurs

===========================================================================*/
void uisnd_cb_function2( void *client_data, snd_status_type status )
/*lint -esym(715,client_data)*/
{
  if (status == SND_REPEAT) {
#ifndef USES_MMI
#ifdef FEATURE_APP_DIALER
    IShell *shell_ptr = AEE_GetShell();
    if (shell_ptr != NULL) {
      // TO DO - put right event here
      ISHELL_PostEventEx(shell_ptr, EVTFLG_ASYNC, AEECLSID_DIALER,
                        EVT_USER,
                        EVT_DIALER_MULTI_COMPLETE, 0);
    }
#endif
#endif
  }
                /* Cannot use rex_self. This routine runs under Sound task */
}

/* <EJECT> */

/*===========================================================================
FUNCTION uisnd_MULTI

DESCRIPTION
  Play a multitone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_multi(
  boolean         dotx,           /* transmit over air? */
  snd_compact_tone_type        *sound,         /* list of tones */
  boolean         received        /* Multi tones received from BS */
)
{
#ifdef FEATURE_AUDIO_FORMAT
  static snd_compact_sound_type multi_sound;
  static snd_compact_tone_type sound_buf[66];

  byte i;
  snd_apath_type dtmf_path = SND_APATH_LOCAL;
  snd_device_type device;
  snd_method_type method;

  for (i = 0; i < (ARR_SIZE(sound_buf) - 1); i++) {
    sound_buf[i] = sound[i];
    if (sound_buf[i].tone == SND_STOP)
      break;
  }

  if (i == (ARR_SIZE(sound_buf) - 1)) {
    ERR("Incoming buffer at maximum size %d", sound[i].tone, 0, 0 );
  }

  /* Fill in the rest of the buffer with SND_STOPs. */
  /* This routine always ensures at least one at the end. */
  for (; i < ARR_SIZE(sound_buf); i++) {
    sound_buf[i].tone = SND_STOP;
    sound_buf[i].param.duration_ms = 0;
  }
  multi_sound.type       = SND_COMPACT_SOUND;
  multi_sound.tone_array = sound_buf;

  if(dotx) {
    dtmf_path = SND_APATH_BOTH;
  }
  if (received) {
    device     = choose_device( UI_CALL_SND );
    method     = choose_method( UI_CALL_SND );
  } else {
    device     = choose_device( UI_BEEP_SND );
    method     = choose_method( UI_BEEP_SND );
  }

  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds ",0,0,0);
    snd_sound_start(device, method, (snd_sound_type *) &multi_sound,
        SND_PRIO_HIGH, dtmf_path,
                  (snd_cb_func_ptr_type)uisnd_cb_function2, NULL);
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set ",0,0,0);
  }
#else /* FEATURE_AUDIO_FORMAT */
  MSG_HIGH("Sound Suppressed due to no FEATURE_AUDIO_FORMAT", 0, 0, 0);
#endif /* FEATURE_AUDIO_FORMAT */
}


/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_MULTI_STOP

DESCRIPTION
  Stop any multi tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_multi_stop( void )
{
  MSG_MED("Stopping a multi-sound", 0, 0, 0);
#ifdef FEATURE_AUDIO_FORMAT
  snd_sound_stop(SND_PRIO_HIGH, NULL, NULL);
#endif /* FEATURE_AUDIO_FORMAT */
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_TONE

DESCRIPTION
  Play a tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tone(
  uisnd_snd_type  type,           /* type of sound */
  snd_tone_type   tone,           /* which tone */
  word            wtime           /* duration in ms */
)
{
  snd_device_type device;
  snd_method_type method;

  MSG_MED("Starting a tone %d %d %d", type, tone, wtime);
  device  = choose_device( type );
  method  = choose_method( type );
  uisnd_set_volume(device,method,100,NULL,NULL);
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d %d", type, tone, 0);
    uisnd_play_tone_generic( device, method, tone, wtime,
           SND_APATH_LOCAL, NULL, NULL);
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, tone, 0);
  }
}

/*===========================================================================
FUNCTION uisnd_TONE_callback

DESCRIPTION
  Play a tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tone_callback(
  uisnd_snd_type        type,         /* type of sound */
  snd_tone_type         tone,         /* which tone */
  word                  wtime,        /* duration in ms */
  snd_cb_func_ptr_type  callback_ptr, /* Call back function */
  const void            *client_data  /* Pointer to Client data */
)
{
  snd_device_type device;
  snd_method_type method;

  MSG_MED("Starting a tone %d %d %d", type, tone, wtime);
  device  = choose_device( type );
  method  = choose_method( type );
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d %d", type, tone, 0);
    uisnd_play_tone_generic( device, method, tone, wtime,
           SND_APATH_LOCAL, callback_ptr, client_data);
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, tone, 0);
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_sample_tone

DESCRIPTION
  Play a sample tone while adjusting volume

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_sample_tone(
  uisnd_snd_type  type,           /* type of sound */
  snd_tone_type   tone,           /* which tone */
  word            wtime,           /* duration in ms */
  byte            vol
)
{
  snd_device_type device;
  snd_method_type method;

  device  = choose_device( type );
  method  = choose_method( type );
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d %d", type, tone, 0);
    snd_sample_tone_start(device, method, tone, wtime, vol, NULL, NULL);
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d %d", type, tone, 0);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_DTMF

DESCRIPTION
  Play a dtmf - a special tone which may be transmitted, and is always
  played out the speaker

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_dtmf(
  snd_tone_type   tone,           /* which dtmf */
  word            wtime            /* duration in ms */
)
{

  snd_device_type device;
  snd_method_type method;
  snd_apath_type  dtmf_path = SND_APATH_LOCAL;


  MSG_MED("Starting a DTMF %d %d", tone, wtime, 0);
  device  = choose_device( UI_DTMF_SND );
  method  = choose_method( UI_DTMF_SND );
  if(!uisnd_is_to_be_silenced(method)) {
    MSG_MED("Starting Sounds %d", tone,0, 0);
    uisnd_play_tone_generic( device, method, tone, wtime, dtmf_path, NULL, NULL);
  } else {
    MSG_HIGH("Sound Suppressed due to silenceall set %d", tone, 0, 0);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_tone_stop

DESCRIPTION
  Stop any tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tone_stop( void )
{
  MSG_MED("Stopping a tone", 0, 0, 0);
  snd_tone_stop(NULL, NULL);
}

/*===========================================================================
FUNCTION uisnd_tone_stop_generic

DESCRIPTION
  Stop any tone

DEPENDENCIES
  uisnd_snd_init has been called to set things up
===========================================================================*/
void uisnd_tone_stop_generic
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  MSG_MED("Stopping a tone", 0, 0, 0);
  snd_tone_stop(callback_ptr, client_data);
}

#ifdef FEATURE_UI_CORE_REMOVED
/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_init_beep_vol

DESCRIPTION
  Initialize beep volume based on NV setting

DEPENDENCIES
===========================================================================*/
void uisnd_init_beep_vol(nv_items_enum_type item)
{
  nv_item_type nvi;
  byte vol = UISND_2ND_VOL;

  if (ui_get_nv(item, &nvi) == NV_DONE_S) {
    switch (item) {
      case NV_BEEP_LVL_I:
        vol = nvi.beep_level;
        break;

      case NV_BEEP_SPKR_LVL_I:
        vol = nvi.beep_spkr_lvl;
        break;

      case NV_BEEP_LVL_SHADOW_I:
        vol = nvi.beep_lvl_shadow;
        break;

      default:
        break;
    }
  }

  uisnd_beep_vol(vol, item);
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_init_voice_vol

DESCRIPTION
  Initialize voice volume based on NV setting

DEPENDENCIES
===========================================================================*/
void uisnd_init_voice_vol(nv_items_enum_type item)
{
  nv_item_type nvi;
  byte vol = UISND_2ND_VOL;

  if (ui_get_nv(item, &nvi) == NV_DONE_S) {
    switch (item) {
      case NV_SPEAKER_LVL_I:
        vol = nvi.speaker_level;
        break;

      case NV_EAR_LVL_I:
        vol = nvi.ear_level;
        break;

      case NV_EAR_LVL_SHADOW_I:
        vol = nvi.ear_lvl_shadow;
        break;

      default:
        break;
    }
  }

  uisnd_voice_vol(vol, item);
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_init_ring_vol

DESCRIPTION
  Initialize ringer volume based on NV setting

DEPENDENCIES
===========================================================================*/
void uisnd_init_ring_vol(nv_items_enum_type item)
{
  nv_item_type nvi;
  byte vol = UISND_2ND_VOL;

  if (ui_get_nv(item, &nvi) == NV_DONE_S) {
    switch (item) {
      case NV_RINGER_LVL_I:
        vol = nvi.ringer_level;
        break;

      case NV_RINGER_SPKR_LVL_I:
        vol = nvi.ringer_spkr_lvl;
        break;

      default:
        break;
    }
  }

  uisnd_ring_vol(vol, item);
}

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_init_alert_vol

DESCRIPTION
  Initialize message alert volume based on NV setting

DEPENDENCIES
===========================================================================*/
void uisnd_init_alert_vol(nv_items_enum_type item)
{
  nv_item_type nvi;
  byte vol = UISND_2ND_VOL;

  if (ui_get_nv(item, &nvi) == NV_DONE_S) {
    switch (item) {
      case NV_ALERTS_LVL_I:
        vol = nvi.alerts_lvl;
        break;

      case NV_ALERTS_LVL_SHADOW_I:
        vol = nvi.alerts_lvl_shadow;
        break;

      default:
        break;
    }
  }

  uisnd_alert_vol(vol, item);
}

#ifdef FEATURE_MULTIMEDIA
/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_init_alert_vol

DESCRIPTION
  Initialize multimedia volume based on NV setting

DEPENDENCIES
===========================================================================*/
void uisnd_init_mm_vol(nv_items_enum_type item)
{
  nv_item_type nvi;
  byte vol = UISND_2ND_VOL;

  if (ui_get_nv(item, &nvi) == NV_DONE_S) {
    switch (item) {
      case NV_MM_LVL_I:
        vol = nvi.mm_lvl;
        break;

      case NV_MM_LVL_SHADOW_I:
        vol = nvi.mm_lvl_shadow;
        break;

      case NV_MM_SPEAKER_LVL_I:
        vol = nvi.mm_speaker_lvl;
        break;

      default:
        break;
    }
  }

  uisnd_mm_vol(vol, item);
}
#endif /* FEATURE_MULTIMEDIA */

#endif /* FEATURE_UI_CORE_REMOVED */

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_SND_INIT

DESCRIPTION
  Initialize ui -> snd buffers

DEPENDENCIES
  Called once, by UI.C
===========================================================================*/
void uisnd_snd_init( void )
{
  rex_init_crit_sect(&uisnd_crit_sect);

  out_device = SND_DEVICE_HANDSET; /* through phone */

#ifdef FEATURE_UI_CORE_REMOVED
  uisnd_set_mute(UISND_MUTE_MUTED, UISND_MUTE_MUTED, NULL, NULL);
  uisnd_set_device_auto(NULL,NULL);


  /* beep volume */
  uisnd_init_beep_vol(NV_BEEP_LVL_I);
  uisnd_init_beep_vol(NV_BEEP_SPKR_LVL_I);
  uisnd_init_beep_vol(NV_BEEP_LVL_SHADOW_I);

  /* voice volume */
  uisnd_init_voice_vol(NV_SPEAKER_LVL_I);
  uisnd_init_voice_vol(NV_EAR_LVL_I);
  uisnd_init_voice_vol(NV_EAR_LVL_SHADOW_I);

  /* Ringer volume */
  uisnd_init_ring_vol(NV_RINGER_LVL_I);
  uisnd_init_ring_vol(NV_RINGER_SPKR_LVL_I);

  /* Message alert volume */
  uisnd_init_alert_vol(NV_ALERTS_LVL_I );
  uisnd_init_alert_vol(NV_ALERTS_LVL_SHADOW_I);

#ifdef FEATURE_MULTIMEDIA
  /* Multimedia volume */
  uisnd_init_mm_vol(NV_MM_LVL_I);
  uisnd_init_mm_vol(NV_MM_LVL_SHADOW_I);
  uisnd_init_mm_vol(NV_MM_SPEAKER_LVL_I);
#endif

#endif /* FEATURE_UI_CORE_REMOVED */
#ifdef CUST_EDITION
  // Define the vibrator timer
  clk_def( &uisnd_vibrator_clk );
#endif
}


#ifdef FEATURE_AUDIO_AGC
/* <EJECT> */
/*===========================================================================
FUNCTION UISND_SET_AAGC

DESCRIPTION
  Enable or disable the Audio AGC feature.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_set_aagc(
  boolean enable
)
{
  snd_agc_ctl( enable, NULL, NULL);
  snd_avc_ctl( enable, NULL, NULL);

}
#endif /* FEATURE_AUDIO_AGC */


/*===========================================================================

FUNCTION uisnd_get_levels

DESCRIPTION
  Get volume level for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_get_levels
(
  snd_device_type          device,
    /* The device chosen for this command   */
  snd_method_type          method,
    /* method                               */
  snd_lev_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
)
{
  snd_get_levels(  device,
                   method,
                   callback_ptr,
                   client_data);
}

/*===========================================================================

FUNCTION map_uisnd_volume_to_snd_volume

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  Volume level mapped for 4 level audio or 7 level audio.

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
uint8 map_uisnd_volume_to_snd_volume
(
  uint8 input_volume,
  uint8 input_volume_levels,
  uint8 output_volume_levels
)
{
  if (input_volume_levels == 0) {
    return input_volume;
  } else {
    return (output_volume_levels * input_volume) / input_volume_levels;
  }
}

/*===========================================================================

FUNCTION uisnd_set_volume_cb

DESCRIPTION
  This function is the default callback to uisnd_set_volume.  If that function
  was given a NULL callback, this function is called to at least log a
  problem if we received an error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_set_volume_cb
             ( const void      *client_data, /* pointer to Client data   */
               snd_status_type status )      /* Status returned by Sound */
{
  static uint32 uisnd_set_volume_errors = 0;
  if (status != SND_SUCCESS)
  {
    uisnd_set_volume_errors++;
    ERR("snd_set_volume returned error %d - status %x for client data %x",
        uisnd_set_volume_errors, status, client_data);
  }
}

/*===========================================================================

FUNCTION uisnd_get_levels_set_volume_cb

DESCRIPTION
  Call back function for snd_get_levels when the next action is to set
  the volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sound device info table entry for number_of_volume_levels will be updated if
  sound services sends in status == SUCCESS.

===========================================================================*/
void uisnd_get_levels_set_volume_cb
(
  const void       *client_data, /* pointer to Client data   */
  snd_status_type  status,       /* Status returned by Sound */
  uint16           levels        /* Maximum Volume Level     */
)
{
  uisnd_set_vol_info_s_type *vol_info_ptr;
  uint8 adjusted_volume;
  snd_cb_func_ptr_type vol_callback_ptr;
  void *vol_client_data;

  vol_info_ptr = (uisnd_set_vol_info_s_type *)client_data;

  if ( vol_info_ptr != NULL)
  {
    //scale volume if needed
    adjusted_volume = map_uisnd_volume_to_snd_volume(vol_info_ptr->volume,
                                                     vol_info_ptr->num_vol_levels_desired,
                                                     levels);

    vol_callback_ptr = vol_info_ptr->callback_ptr;
    vol_client_data = vol_info_ptr->client_data;

    if (vol_callback_ptr == NULL)
    {
      vol_callback_ptr = uisnd_set_volume_cb;
      vol_client_data = (void *) ((((uint32) vol_info_ptr->device & 0xFFFF) << 16) |
                                  (((uint32) vol_info_ptr->method & 0xFF) << 8) |
                                  ( (uint32) adjusted_volume & 0xFF));
    }

    snd_set_volume(vol_info_ptr->device,
                   vol_info_ptr->method,
                   adjusted_volume,
                   vol_callback_ptr,
                   vol_client_data);


    mem_free(&tmc_heap, vol_info_ptr);
  }
}


/*===========================================================================

FUNCTION uisnd_set_volume

DESCRIPTION
  Set volume for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_set_volume
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  uint8                 volume,
    /* Volume                               */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  uisnd_set_vol_info_s_type *vol_info_ptr;

  vol_info_ptr = mem_malloc(&tmc_heap, sizeof(uisnd_set_vol_info_s_type));

  vol_info_ptr->device = device;
  vol_info_ptr->method = method;
  vol_info_ptr->volume = volume;
  vol_info_ptr->num_vol_levels_desired = 0;
  vol_info_ptr->callback_ptr = callback_ptr;
  vol_info_ptr->client_data = (void*)client_data;

  snd_get_levels(  device,
                   method,
                   uisnd_get_levels_set_volume_cb,
                   vol_info_ptr);
}


/*===========================================================================

FUNCTION uisnd_set_volume_scaled

DESCRIPTION
  Set volume for a device x method, pass through to snd,
  scales the requested sound volume assuming 4 volume levels
  intended

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_set_volume_scaled
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  uint8                 volume,
    /* Volume                               */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  uisnd_set_vol_info_s_type *vol_info_ptr;

  vol_info_ptr = mem_malloc(&tmc_heap, sizeof(uisnd_set_vol_info_s_type));

  vol_info_ptr->device = device;
  vol_info_ptr->method = method;
  vol_info_ptr->volume = volume;
  vol_info_ptr->num_vol_levels_desired = UISND_MAX_VOL;
  vol_info_ptr->callback_ptr = callback_ptr;
  vol_info_ptr->client_data = (void*)client_data;

  snd_get_levels(  device,
                   method,
                   uisnd_get_levels_set_volume_cb,
                   vol_info_ptr);
}


/*===========================================================================

FUNCTION uisnd_get_volume

DESCRIPTION
  Get volume for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_get_volume
(
  snd_device_type          device,
    /* The New device Type the caller wants */
  snd_method_type          method,
    /* method                               */
  snd_vol_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
)
{
   snd_get_volume(device,
                  method,
                  callback_ptr,
                  client_data);

}

/*===========================================================================

FUNCTION uisnd_play_tone_generic

DESCRIPTION
  Start playing a tone using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_play_tone_generic
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  snd_tone_type         tone_id,
    /* Tone id                              */
  uint16                duration,
    /* Duration of the tone in ms           */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
   snd_tone_start( device,
                   method,
                   tone_id,
                   duration,
                   dtmf_path,
                   callback_ptr,
                   client_data);
}

/*===========================================================================

FUNCTION uisnd_freq_tone_start

DESCRIPTION
  Start playing a tone specified by a pair of frequencies
  using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void uisnd_freq_tone_start
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  uint16                f1_hz,
    /* The higher freq of the DTMF pair     */
  uint16                f2_hz,
    /* The lower freq of the DTMF pair      */
  uint16                duration,
    /* Duration of the tone in ms           */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_freq_tone_start(device,
                      method,
                      f1_hz,
                      f2_hz,
                      duration,
                      dtmf_path,
                      callback_ptr,
                      client_data);
}

#ifdef CUST_EDITION
static void uisnd_vibrate_cmd(boolean vibrate)
{
    hs_packets_type* hs_cmd_ptr = NULL;      /* command to handset */

    /* If no available command buffers return */
    if(( hs_cmd_ptr = (hs_packets_type *) q_get( &hs_cmd_free_q )) == NULL )
    {
        return;
    }

    hs_cmd_ptr->vib_moto_ctrl.hdr.cmd        = HS_SET_VIB_MOTO_ONOFF;
    hs_cmd_ptr->vib_moto_ctrl.hdr.task_ptr   = NULL;
    hs_cmd_ptr->vib_moto_ctrl.hdr.done_q_ptr = &hs_cmd_free_q;
    hs_cmd_ptr->vib_moto_ctrl.onoff         = vibrate;
    hs_cmd( hs_cmd_ptr );
}

static void uisnd_vibrator_cb( int4 interval )
{
   uisnd_vibrate_cmd(FALSE);
   clk_dereg( &uisnd_vibrator_clk );
}

/*==================================================================
Function: uisnd_vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.

Parameter(s):
   uint16 u16Duration: Duration of vibration in milliseconds

Return Value:
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   uisnd_stop_vibrate

==================================================================*/
void uisnd_vibrate(uint16 wDuration,
       snd_cb_func_ptr_type callback_ptr,
       const void *client_data)
{
	  nv_item_type	SimChoice;
 /* #ifndef FEATURE_VERSION_C306   //add by yangdecai 20110312
  uisnd_vibrate_cmd(TRUE);
  clk_reg( &uisnd_vibrator_clk,
            uisnd_vibrator_cb,
            wDuration,
            0,
            FALSE );
           
            
  #else  */
#if defined(FEATURE_VERSION_C306)|| defined(FEATURE_VERSION_V3CM301) || defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_M74)|| defined(FEATURE_LOWER_MEM)
#if defined( FEATURE_VERSION_C316)|| defined(FEATURE_VERSION_M74)||defined(FEATURE_VERSION_K212_HUALU) // xxzhen 2 in 1 motor
  uisnd_vibrate_cmd(TRUE);
	 clk_reg( &uisnd_vibrator_clk,
	              uisnd_vibrator_cb,
	              wDuration,
	              0,
	              FALSE );
#else

#ifdef FEATURE_ALERT_3AND1
#ifdef FEATURE_VERSION_C260_IC19
    snd_freq_tone_start(SND_DEVICE_CURRENT,
				   SND_METHOD_RING,
				   180,/*188*/
				   180,/*188*/
				   (uint16)(1000),
				   (snd_apath_type)(SND_APATH_LOCAL),
				   callback_ptr,
				   client_data
				  );
#else
    snd_freq_tone_start(SND_DEVICE_CURRENT,
				   SND_METHOD_RING,
				   175,
				   175,
				   (uint16)(1000),
				   (snd_apath_type)(SND_APATH_LOCAL),
				   callback_ptr,
				   client_data
				  );
#endif
#else
    uisnd_vibrate_cmd(TRUE);
    clk_reg( &uisnd_vibrator_clk,
              uisnd_vibrator_cb,
              wDuration,
              0,
              FALSE ); 
#endif

#endif

#else
  uisnd_vibrate_cmd(TRUE);
    clk_reg( &uisnd_vibrator_clk,
              uisnd_vibrator_cb,
              wDuration,
              0,
              FALSE );

  					 
#endif
  vibrator_on = TRUE;
}

/*==================================================================
Function: uisnd_stop_vibrate

Description:
   It stops the current vibration.

Parameter(s):
   None.

Return Value:
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   uisnd_vibrate

==================================================================*/
void uisnd_stop_vibrate(snd_cb_func_ptr_type callback_ptr, const void *client_data)
{
  uisnd_vibrate_cmd(FALSE);
  clk_dereg( &uisnd_vibrator_clk );
  vibrator_on = FALSE;
}

#else
/*==================================================================
Function: uisnd_vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.

Parameter(s):
   uint16 u16Duration: Duration of vibration in milliseconds

Return Value:
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   uisnd_stop_vibrate

==================================================================*/
void uisnd_vibrate(uint16 wDuration,
       snd_cb_func_ptr_type callback_ptr,
       const void *client_data)
{
#ifdef FEATURE_AUDIO_FORMAT
  snd_tone_start( SND_DEVICE_CURRENT,
                  SND_METHOD_AUX,
                  (snd_tone_type)(SND_1),
                  (uint16)(1000),
                  (snd_apath_type)(SND_APATH_LOCAL),
                  callback_ptr,
                  client_data
                );
  MSG_FATAL("uisnd_vibrate................................",0,0,0);
  
#else /* FEATURE_AUDIO_FORMAT */
  MSG_HIGH("Sound Suppressed due to no FEATURE_AUDIO_FORMAT", 0, 0, 0);
#endif /* FEATURE_AUDIO_FORMAT */

  vibrator_on = TRUE;
}

/*==================================================================
Function: uisnd_stop_vibrate

Description:
   It stops the current vibration.

Parameter(s):
   None.

Return Value:
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   uisnd_vibrate

==================================================================*/
void uisnd_stop_vibrate(snd_cb_func_ptr_type callback_ptr, const void *client_data)
{
#ifdef FEATURE_AUDIO_FORMAT
  if (vibrator_on) {
    snd_tone_stop( callback_ptr, client_data );
  }
#endif /* FEATURE_AUDIO_FORMAT */
  vibrator_on = FALSE;
}
#endif
/*==================================================================
Function: uisnd_is_vibrator_on

Description:
   Can be used to check if vibrator is ON.

Return Value:
   TRUE - Vibrator is ON.
   FALSE - Vibrator is OFF

==================================================================*/
boolean uisnd_is_vibrator_on(void)
{
  return vibrator_on;
}

/*===========================================================================
FUNCTION UISND_NOTIFY_REGISTER

DESCRIPTION
  This function is used to register a callback function that is used to notify
  UISND device and mute setting changes. The user data given as input will be
  passed on when the notification callback is called.

RETURN VALUE
  TRUE if successful

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  uisnd_notify_unregister
===========================================================================*/
boolean uisnd_notify_register(UISND_NOTIFY_FUNC func, void *user_data)
{
  uisnd_notify_node_s_type *node;
  uisnd_notify_node_s_type *curr;

  if (func == NULL) {
    return FALSE;
  }

  /* Allocate notification node */
  node = mem_malloc(&tmc_heap, sizeof(uisnd_notify_node_s_type));
  if (node == NULL) {
    MSG_ERROR("Couldn't alloc mem for notify node", 0, 0, 0);
    return FALSE;
  }

  rex_enter_crit_sect(&uisnd_crit_sect);

  /* Return error if func/user_data pair is already registered */
  curr = uisnd_notify_list;
  while (curr)
  {
    if ((curr->notify_func == func) && (curr->user_data == user_data)) {
      MSG_HIGH("Callback %p:%x already registered", func, user_data, 0);
      mem_free(&tmc_heap,node);
      rex_leave_crit_sect(&uisnd_crit_sect);
      return FALSE;
    }
    curr = curr->next;
  }

  /* Add node to the head of notification list */
  node->next = uisnd_notify_list;
  node->notify_func = func;
  node->user_data = user_data;

  uisnd_notify_list = node;

  rex_leave_crit_sect(&uisnd_crit_sect);

  return TRUE;
}

/*===========================================================================
FUNCTION UISND_NOTIFY_UNREGISTER

DESCRIPTION
  This function is used to cancel the notification function registered with UISND.

  The function pointer and user data provided as input should match the
  registered function pointer and user data.

RETURN VALUE
  TRUE if successful

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  uisnd_notify_register
===========================================================================*/
boolean uisnd_notify_unregister(UISND_NOTIFY_FUNC func, void *user_data)
{
  uisnd_notify_node_s_type *curr;
  uisnd_notify_node_s_type *prev;
  boolean ret = FALSE;

  if (func == NULL) {
    return FALSE;
  }

  rex_enter_crit_sect(&uisnd_crit_sect);

  if (!uisnd_notify_list) {
    MSG_ERROR("Notification list is empty", 0, 0, 0);
    rex_leave_crit_sect(&uisnd_crit_sect);
    return FALSE;
  }

  prev = NULL;
  curr = uisnd_notify_list;

  while (curr)
  {
    if ((curr->notify_func == func) && (curr->user_data == user_data)) {
      /* Found node, delete it from the list */
      if (prev)
        prev->next = curr->next;
      else
        uisnd_notify_list = curr->next;

      mem_free(&tmc_heap,curr);
      ret = TRUE;
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  rex_leave_crit_sect(&uisnd_crit_sect);

  if (ret != TRUE) {
    MSG_ERROR("Could not find notification function %p:%x",
                 func, user_data, 0);
  }

  return ret;
}

/*===========================================================================
FUNCTION UISND_NOTIFY

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static void uisnd_notify(uisnd_notify_data_s_type *notify_data)
{
  uisnd_notify_node_s_type *curr = uisnd_notify_list;

  while (curr)
  {
    if (curr->notify_func) {
      (*curr->notify_func)(curr->user_data, notify_data);
    }
    else
    {
      MSG_ERROR("Invalid entry %p found in notify list", curr, 0, 0);
      break;
    }

    curr = curr->next;
  }
}

/*===========================================================================
FUNCTION uisnd_get_incall_mixing

DESCRIPTION
  Get the sound mixing flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean uisnd_get_incall_mixing(void)
{
  return enable_sound_mixing;
}

/*===========================================================================
FUNCTION uisnd_set_incall_mixing

DESCRIPTION
  Set the sound mixing flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void uisnd_set_incall_mixing(boolean new_value)
{
  enable_sound_mixing = new_value;
}


/*===========================================================================
FUNCTION UISND_SET_SIDETONE

DESCRIPTION sets sidetone value

RETURN VALUE
  None

DEPENDENCIES
  Sidetone functions from Snd.c

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void uisnd_set_sidetone(snd_sidetone_mode_type setting)
{
#ifdef FEATURE_AUDIO_FORMAT
  snd_sidetone_ctl(setting, NULL, NULL);
  sidetone_setting = setting;
#endif /*FEATURE_AUDIO_FORMAT*/
}

/*===========================================================================
FUNCTION UISND_GET_SIDETONE

DESCRIPTION sets sidetone value

RETURN VALUE
  None

DEPENDENCIES
  Sidetone functions from Snd.c

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
snd_sidetone_mode_type uisnd_get_sidetone(void)
{
  return sidetone_setting;
}

#endif //!FEATURE_UI_DUALPROC_MDM
//lint -restore
