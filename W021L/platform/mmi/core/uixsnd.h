#ifndef UIXSND_H
#define UIXSND_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


         U S E R   I N T E R F A C E   S O U N D   M A N A G E R

GENERAL DESCRIPTION
  This manages the nasty details of what volume and which path to use
  at which time.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 1991 - 2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/uixsnd.h#26 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/08   cvs     Fix compile warning
06/13/07   jks     Add set_volume_scaled, for backward compatibility
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
01/24/07   jks     Mapped UI 4 levels of voulme to sndcal 7 levels of volume
08/17/06   joe     Added support for sidetone control
08/11/06   sk      Added functionality to get & set sound incall-mixing state
06/02/06   cvs     add function to restore mute only
03/27/06   ng      Featurized VT app
02/17/06   jks     Removed duplicate code
08/08/05   aps     Fixed NV volume level lookup for sound devices
08/02/05   AT      Add callbacks to snd_set_device and snd_set_volume.
07/01/05   cvs     Fix key beeps for multi keys, and use callbacks
06/28/05   cvs     Add menu to select sound device
03/31/05   cvs     More APIs so OEMSound can go through uisnd
04/01/05   cvs     Add notifications of device/mute setting changes
03/31/05   cvs     Add APIs so OEMSound can go through uisnd, preventing device conflicts
01/31/05   cvs     Add define for snd API change
01/21/05   cvs     Rearchitect uisnd_set_device()
08/03/04   PA      Featurized function not used in thin UI
12/16/03   BPW     Removed unused functions
11/13/03   NNG     Added proper event for VT.
11/05/03   NNG     Added support for VT.
09/26/03   tml     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "snd.h"
#include "nv.h"

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* version tracking by other subsystems */
#define UI_UISND_NO_DIRECT_STATUS_CALLS  /* uisnd no longer has direct calls to any 
                                            external subsystems for dev. status. */

/* Used to make a standard error beep from the UI state machine */
#define UISND_ERR_TONE() uisnd_tone( UI_BEEP_SND, SND_ERR, 300 )

/* make a standard clearall tone */
#define UISND_CLRALL() uisnd_tone( UI_BEEP_SND, SND_CTRL, 200 );

/* make a standard msgall tone */
#define UISND_MSGALL() uisnd_tone( UI_BEEP_SND, SND_CTRL, 200 );

#define UISND_FOUR_LEVEL_AUDIO 4
#define UISND_EAR_LVLS 4
#define UISND_SPKR_LVLS 4
#define UISND_DAC_LVLS 4
#define UISND_BT_LEVELS 15

#define UISND_SND_1ST_VOL 1
#define UISND_SND_2ND_VOL 2
#define UISND_SND_3RD_VOL 3
#define UISND_SND_4TH_VOL 4
#define UISND_SND_5TH_VOL 5
#define UISND_SND_6TH_VOL 6
#define UISND_SND_7TH_VOL 7

#define UISND_1ST_VOL 1
#define UISND_2ND_VOL 2
#define UISND_3RD_VOL 3
#define UISND_4TH_VOL 4


#ifdef FEATURE_BT_AG
#define UISND_AG_1ST_VOL 1
#define UISND_AG_2ND_VOL 2
#define UISND_AG_3RD_VOL 3
#define UISND_AG_4TH_VOL 4
#define UISND_AG_5TH_VOL 5
#define UISND_AG_6TH_VOL 6
#define UISND_AG_7TH_VOL 7
#define UISND_AG_8TH_VOL 8
#define UISND_AG_9TH_VOL 9
#define UISND_AG_10TH_VOL 10
#define UISND_AG_11TH_VOL 11
#define UISND_AG_12TH_VOL 12
#define UISND_AG_13TH_VOL 13
#define UISND_AG_14TH_VOL 14
#define UISND_AG_15TH_VOL 15
#endif

/* Average default volume if there is none defined */
#define UISND_AVG_VOL UISND_2ND_VOL

/* The "Off/Silent" setting */
#define UISND_VOL_OFF 0
/* Maximum settings */
#define UISND_MAX_VOL UISND_4TH_VOL

#ifdef FEATURE_UI_SEVEN_LEVEL_SND
#define UISND_MAX_EAR_VOL UISND_7TH_VOL
#else
#define UISND_MAX_EAR_VOL UISND_4TH_VOL
#endif

#ifdef FEATURE_UI_ESCALATING_RING
#define UISND_RING_LVLS 6
#define UISND_RING_SPKR_LVLS 6
#else

#define UISND_RING_LVLS 5
#define UISND_RING_SPKR_LVLS 5

#endif /* FEATURE_UI_ESCALATING_RING */
#define UISND_BEEP_LVLS 5
#define UISND_BEEP_SPKR_LVLS 5
#define UISND_MSG_LVLS 5
#define UISND_MSG_SPKR_LVLS 5

/*
** Define UISND_ESC_VOL for targets that does NOT support escalating
** ringing as well -- in uinsstrt we need to overwrite UISND_ESC_VOL
** to UISND_AVE_VOL value for such targets.
*/
#define UISND_ESC_VOL 0x8F

/* whether outdevice gets voice or not */
#define UISND_NOVOICE FALSE
#define UISND_VOICE TRUE

#define ALERT_SOUND_LENGTH 2000   /* Length (ms) of Alert sound (Ring) */

#ifdef FEATURE_QVPHONE_APP
#define UISND_VT_RING_COMPLETE 0xFF
#endif /* FEATURE_QVPHONE_APP */

/* for setting mute.  USE_DEFAULT uses the value based on the reference counter. */
/* Any physical path may be muted or unmuted.
*/
typedef enum {
  UISND_MUTE_MUTED = SND_MUTE_MUTED,     /* Audio path is muted.     */
  UISND_MUTE_UNMUTED = SND_MUTE_UNMUTED, /* Audio path is not muted.         */
  UISND_MUTE_NO_CHANGE = SND_MUTE_MAX    /* No change is desired to this path */
} uisnd_mute_control_type;

typedef struct {
  snd_device_type       out_device;
  snd_mute_control_type speaker_mute;
  snd_mute_control_type microphone_mute;
} uisnd_notify_data_s_type;

typedef void (*UISND_NOTIFY_FUNC)(void *user_data,  uisnd_notify_data_s_type *);

/* Which type of sound we want to make */
typedef enum {                    /* the volumes */
  UI_PWRUP_SND,                /* Sounds for power on, ext power off/on   */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = key beep volume */
  UI_BEEP_SND,                 /* Beep snd for most keys                  */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = key beep volume */
  UI_EARBEEP_SND,              /* Beep for low battery, minute beep etc.. */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ear volume      */
                               /*  in car kit volume    = ringer volume   */
  UI_ALERT_SND,                /* alert ring sound - includes signalling  */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ringer volume   */
  UI_CALL_SND,                 /* Call feedback sound ringer/earpiece     */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
  UI_MSG_SND,                  /* Message notifications                   */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ringer volume   */
  UI_EXT_PWR_SND,              /* External Power sounds                   */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
  UI_SVC_ALERT_SND,            /* Service Alert sound */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
  UI_DTMF_SND,                 /* DTMF sounds during a call               */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator =  n/a            */
                               /*  volume used          = up to snd task  */
  UI_CALL_EAR_SND              /* CAll feedback sound earpiece/earpiece   */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ear piece       */
                               /*  volume used          = constant        */
                               /* these will be generated at the earpiece */
                               /* even if the phone is not in use         */
} uisnd_snd_type;


/* Ringer type for the standard alert */
/* It is translated into ringer_type in sound module using the */
/* mapping array uisnd_ringer_snd_map. Make sure both          */
/* uisnd_ringer_type and uisnd_ringer_snd_map are in sync      */
typedef enum {
  UISND_RING_NORMAL = 0,          /* Normal alert                          */
  UISND_RING_1 = 1,               /* User selectable ring 1                */
  UISND_RING_2 = 2,               /* User selectable ring 2                */
  UISND_RING_3 = 3,               /* User selectable ring 3                */
  UISND_RING_4 = 4,               /* User selectable ring 4                */
#ifdef FEATURE_ENHANCED_RINGS
  UISND_RING_5 = 5,               /* User selectable ring 5                */
  UISND_RING_6 = 6,               /* User selectable ring 6                */
  UISND_RING_7 = 7,               /* User selectable ring 7                */
  UISND_RING_8 = 8,               /* User selectable ring 8                */
  UISND_RING_9 = 9,               /* User selectable ring 9                */
#endif
  UISND_LAST_RING                 /* Should be last item in table          */
} uisnd_ringer_type;

/* Type of sound */
typedef enum {
  UISND_KEY_BEEP_SND,
  UISND_EAR_SND,
  UISND_RINGER_SND,
  UISND_MM_SND,
  UISND_MSG_SND
} uisnd_sound_volume_type;

/* Type of sound device status */
/* An external sound device can be physically  unattached or enabled. If it is enabled, 
   it is attached to the handset and turned on. (eg, headset) 
   An internal sound device is always attached but can be turned on or off.(eg, spearker phone)
   If a speaker phone is disabled, it means that it is turned off by the settings.  
   If it is enabled, it means is turned on.
*/
typedef enum
{
   UISND_DEV_UNATTACHED, //snd device not attached    
   UISND_DEV_DISABLED,   //snd attached and turned off                    
   UISND_DEV_ENABLED     //snd attached and turned on           
} uisnd_device_status;

/* Mapping array from uisnd_ringer_type to snd_predef_type for the         */
/* above 10 ui ringer type uisnd_ringer_type. */
extern snd_sound_id_type uisnd_ringer_snd_map[(int)UISND_LAST_RING+1];


/* <EJECT> */
/*===========================================================================
FUNCTION UISND_VOICE_VOL

DESCRIPTION
  Set the voice volume.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
extern void uisnd_voice_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
);

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
);
#endif /* FEATURE_TTY */

#ifdef FEATURE_MULTIMEDIA
/*===========================================================================
FUNCTION UISND_MM_VOL

DESCRIPTION
  Set the Multimedia volume.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
extern void uisnd_mm_vol(
  byte vol,                         /* the volume                  */
  nv_items_enum_type nv_item                      /* nv item indicating the path */
);
#endif


/*===========================================================================
FUNCTION UISND_BEEP_VOL

DESCRIPTION
  Set the beep volume.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
extern void uisnd_beep_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
);

/* <EJECT> */
/*===========================================================================
FUNCTION UISND_alert_VOL

DESCRIPTION
  Set the SMS volume.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
extern void uisnd_alert_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
);


/* <EJECT> */
/*===========================================================================
FUNCTION UISND_RING_VOL

DESCRIPTION
  Set the ring volume

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
extern void uisnd_ring_vol(
  byte vol,                         /* the volume                    */
  nv_items_enum_type nv_item        /* nv item indicating the device */
);

/* <EJECT> */
#define UI_SND_DEV_MGMT             /* A clue to interface users that it changed */
/*===========================================================================
FUNCTION UISND_FORCE_DEVICE_AUTOSELECT

DESCRIPTION
  Force uisnd_set_device_auto() back to autoselect mode

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_force_device_autoselect(void);

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
);

#define UI_SND_RESTORE_MUTE
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
);

/*===========================================================================
FUNCTION UISND_SET_DEVICE_SPECIFIC

DESCRIPTION
  Set the normal input and output device, to something specific.
  This setting remains until the requested device is removed.

RETURN VALUE
  TRUE if device was changed as requested.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_set_device_specific(
  snd_device_type new_device,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
);
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

);
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

);

/* <EJECT> */
/*===========================================================================
FUNCTION UISND_GET_DEVICE

DESCRIPTION
  Get the current output device, and mute settings

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_get_device(uisnd_notify_data_s_type *info);

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
  snd_device_type device
);

#define UI_UISND_TRACKS_DEVICES

/*===========================================================================
FUNCTION UISND_IS_DEVICE_ENABLED

DESCRIPTION
  See if the given device is enabled.  Returns TRUE for yes, FALSE for no.

RETURN VALUE
  TRUE if device is enabled.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
boolean uisnd_is_device_enabled(
  snd_device_type device
);

/*===========================================================================
FUNCTION UISND_SET_DEVICE_STATUS

DESCRIPTION
  Set a device to status.  

RETURN VALUE

DEPENDENCIES
  
===========================================================================*/
void uisnd_set_device_status(
  snd_device_type       device,
  uisnd_device_status new_value
);
/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_SND

DESCRIPTION
  Play a sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_snd(
  uisnd_snd_type    type,         /* type of sound */
  snd_sound_id_type snd           /* which sound */
);

#ifdef FEATURE_QVPHONE_APP
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
);
#endif /* FEATURE_QVPHONE_APP */

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
);

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_SND_STOP

DESCRIPTION
  Stop any sound

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_snd_stop( void );


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
  snd_compact_tone_type *sound,   /* list of tones */
  boolean         received        /* Multi tones received from BS */
);

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_MULTI_STOP

DESCRIPTION
  Stop any multi tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_multi_stop( void );


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
  word            time            /* duration in ms */
);

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
);

/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_sample_tone

DESCRIPTION
  Play a sample tone while adjusting volumes.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_sample_tone(
  uisnd_snd_type  type,           /* type of sound */
  snd_tone_type   tone,           /* which tone */
  word            time,           /* duration in ms */
  byte            vol             /* volume to be played in */
);


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
  word            time            /* duration in ms */
);

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
);
#endif /* FEATURE_AUDIO_AGC */


/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_tone_stop

DESCRIPTION
  Stop any tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tone_stop( void );

/*===========================================================================
FUNCTION uisnd_tone_stop_generic

DESCRIPTION
  Stop any tone

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_tone_stop_generic
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
);


/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_SND_INIT

DESCRIPTION
  Initialize ui -> snd buffers

DEPENDENCIES
  Called once, by UI.C to init buffers
===========================================================================*/
void uisnd_snd_init( void );

/* <EJECT> */

#ifdef FEATURE_UI_ALT_CALL_FAIL
/* <EJECT> */
/*===========================================================================
FUNCTION uisnd_alt_call_fail

DESCRIPTION
  Generate a custom pre-defined call fail sound.  Three rapid INFO key
  beeps, small delay, three rapid INFO key beeps.

DEPENDENCIES
  uisnd_SND_INIT has been called to set things up
===========================================================================*/
void uisnd_alt_call_fail (void);
#endif /* FEATURE_UI_ALT_CALL_FAIL */

#ifdef FEATURE_UI_ESCALATING_RING
/* <EJECT> */
/*===========================================================================
FUNCTION UISND_SET_ESCALATING_RING_LVL

DESCRIPTION

  Set the correct earpiece and speaker ringer volume levels for escalating
  ringing. The volume level depends on whether we are alerting at the moment
  and if so on the number of rings already played.

DENDENCIES
===========================================================================*/
void uisnd_set_escalating_ring_lvl( void );
#endif /* ifdef FEATURE_UI_ESCALATING_RING */


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
);

#endif


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
void uisnd_set_volumes_for_sound_type ( uisnd_sound_volume_type snd_type,
            nv_items_enum_type nv_item,
            snd_method_type meth_type,
            byte vol);
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
nv_items_enum_type uisnd_map_device_to_nv_item ( snd_device_type out_device,
             uisnd_sound_volume_type snd_type);

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
);

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
);

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
extern void uisnd_set_volume
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
);


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
extern void uisnd_set_volume_scaled
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
);


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
extern void uisnd_play_tone_generic
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
);

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
);

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
extern void uisnd_get_volume
(
  snd_device_type          device,
    /* The New device Type the caller wants */
  snd_method_type          method,
    /* method                               */
  snd_vol_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
);

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
void uisnd_vibrate(uint16 wDuration, snd_cb_func_ptr_type callback_ptr, const void *client_data);

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
void uisnd_stop_vibrate(snd_cb_func_ptr_type callback_ptr, const void *client_data);

/*==================================================================
Function: uisnd_is_vibrator_on

Description:
   Can be used to check if vibrator is ON.

Return Value:
   TRUE - Vibrator is ON.
   FALSE - Vibrator is OFF

==================================================================*/
boolean uisnd_is_vibrator_on(void);

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
boolean uisnd_notify_register(UISND_NOTIFY_FUNC func, void *user_data);

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
boolean uisnd_notify_unregister(UISND_NOTIFY_FUNC func, void *user_data);

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
void uisnd_set_sidetone(snd_sidetone_mode_type setting);

/*===========================================================================
FUNCTION UISND_GET_SIDETONE

DESCRIPTION gets sidetone value

RETURN VALUE
  None

DEPENDENCIES
  Sidetone functions from Snd.c

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
snd_sidetone_mode_type uisnd_get_sidetone(void);

/*===========================================================================
FUNCTION uisnd_get_incall_mixing

DESCRIPTION
  Get the sound mixing flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean uisnd_get_incall_mixing(void);

/*===========================================================================
FUNCTION uisnd_set_incall_mixing

DESCRIPTION
  Set the sound mixing flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void uisnd_set_incall_mixing(boolean new_value);

#endif  /* #ifndef UIXSND_H */
