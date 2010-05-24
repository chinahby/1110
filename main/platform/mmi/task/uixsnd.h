#ifndef UIXSND_H
#define UIXSND_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


         U S E R   I N T E R F A C E   S O U N D   M A N A G E R

GENERAL DESCRIPTION
  This manages the nasty details of what volume and which path to use
  at which time.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 1991 - 2006 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc60x0/apps/ui/uixsnd.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/06   jks     Added forced audio path support
11/06/02   kar     Mainlined F_MULTIPLE_RINGER_TYPES
03/29/02   kar     Initial MM version
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
// 音量定义
#define UISND_1ST_VOL 0x1F
#define UISND_2ND_VOL 0x6F
#define UISND_3RD_VOL 0xAF
#define UISND_4TH_VOL 0xFF

// 平均音量
#define UISND_AVG_VOL UISND_2ND_VOL

// The "Off/Silent" setting
#define UISND_VOL_OFF 0
// Maximum settings
#define UISND_MAX_VOL UISND_4TH_VOL

/*
** Define UISND_ESC_VOL for targets that does NOT support escalating
** ringing as well -- in uinsstrt we need to overwrite UISND_ESC_VOL
** to UISND_AVE_VOL value for such targets.
*/
#define UISND_ESC_VOL 0x8F


// UI 希望播放的声音类型定义
typedef enum 
{
    UI_PWRUP_SND,              /* Sounds for power on, ext power off/on   */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = key beep volume */
    UI_BEEP_SND,               /* Beep snd for most keys                  */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = key beep volume */
    UI_EARBEEP_SND,            /* Beep for low battery, minute beep etc.. */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ear volume      */
                               /*  in car kit volume    = ringer volume   */
    UI_ALERT_SND,              /* alert ring sound - includes signalling  */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ringer volume   */
    UI_CALL_SND,               /* Call feedback sound ringer/earpiece     */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
    UI_MSG_SND,                /* Message notifications                   */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = ringer volume   */
    UI_EXT_PWR_SND,            /* External Power sounds                   */
                               /*  in use generator     = ringer          */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
    UI_SVC_ALERT_SND,          /* Service Alert sound */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ringer          */
                               /*  volume used          = constant        */
    UI_DTMF_SND,               /* DTMF sounds during a call               */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator =  n/a            */
                               /*  volume used          = up to snd task  */
    UI_CALL_EAR_SND            /* CAll feedback sound earpiece/earpiece   */
                               /*  in use generator     = ear piece       */
                               /*  not in use generator = ear piece       */
                               /*  volume used          = constant        */
                               /* these will be generated at the earpiece */
                               /* even if the phone is not in use         */
} uisnd_snd_type;


/*==============================================================================
函数: 
    uisnd_voice_vol
       
说明: 
    Set the TTY device volume. Then also tell sound task.
       
参数: 
    vol [in]: the volume
    nv_item [in]: nv item indicating the device
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_voice_vol(byte vol, nv_items_enum_type nv_item);

#ifdef FEATURE_TTY
/*==============================================================================
函数: 
    uisnd_tty_vol
       
说明: 
    Set the TTY device volume. Then also tell sound task.
       
参数: 
    vol [in]: the volume
    tty_dev [in]: the TTY device type
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_tty_vol(byte vol, snd_device_type tty_dev);
#endif /* FEATURE_TTY */

#ifdef FEATURE_MULTIMEDIA
/*==============================================================================
函数: 
    uisnd_mm_vol
       
说明: 
    Set the Multimedia volume. Then also tell sound task.
       
参数: 
    vol [in]: the volume
    nv_item [in]: nv item indicating the device
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_mm_vol(byte vol, nv_items_enum_type nv_item);

#endif

/*==============================================================================
函数: 
    uisnd_beep_vol
       
说明: 
    Set the beep volume.
       
参数: 
    vol [in]: the volume
    nv_item [in]: nv item indicating the device
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_beep_vol(byte vol,  nv_items_enum_type nv_item);

/*==============================================================================
函数: 
    uisnd_alert_vol
       
说明: 
    Set the SMS alert volume.SMS Volume
       
参数: 
    vol [in]: the volume
    nv_item [in]: nv item indicating the device
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_alert_vol(byte vol, nv_items_enum_type nv_item);

/*==============================================================================
函数: 
    uisnd_GetSoundInfo
       
说明: 
    Set the ring volume, and tell sound task also.
       
参数: 
    vol [in]: the volume
    nv_item [in]: nv item indicating the device
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_ring_vol(byte vol, nv_items_enum_type nv_item);

/*==============================================================================
函数: 
    uisnd_set_device
       
说明: 
    Set the normal and mute input and output device.  If the global gbForceAudioPath
    flag is set, force the audio device to the global gForceAudioDevice, else set the
    audio device through auto-discovery
       
参数: 
    speaker_mute [in]: Mute, unmute output
    microphone_mute [in]: Mute, unmute microphone
    change_mute [in]: 是否改变静音控制 want change?
    callback_ptr [in]: Call back function
    client_data [in]: Pointer to Client data
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_set_device(snd_mute_control_type speaker_mute,
                      snd_mute_control_type microphone_mute,
                      boolean               change_mute,
                      snd_cb_func_ptr_type  callback_ptr,
                      const void            *client_data);

/*==============================================================================
函数: 
    uisnd_set_forced_audio_path
       
说明: 
    Get the forced input and output audio path. 
    SND_DEVICE_CURRENT indicated autoselect
       
参数: 
    none
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
uint32 uisnd_get_forced_audio_path(void);
                      
/*==============================================================================
函数: 
    uisnd_set_forced_audio_path
       
说明: 
    Set the normal input and output audio path to something specific.
    This setting remains until the requested audio path is changed.
       
参数: 
    new_device [in]: 声音设备类型
    speaker_mute [in]: Mute, unmute output
    microphone_mute [in]: Mute, unmute microphone
    callback_ptr [in]: Call back function
    client_data [in]: Pointer to Client data
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_set_forced_audio_path(snd_device_type        new_device,
                                 snd_mute_control_type  speaker_mute,
                                 snd_mute_control_type  microphone_mute,
                                 snd_cb_func_ptr_type   callback_ptr,
                                 const void             *client_data);

/*==============================================================================
函数: 
    uisnd_clear_forced_audio_path
       
说明: 
    Clear the forced input and output audio path
       
参数: 
    none
    
返回值:
    none
       
备注:
    A context switch might occurs
    
==============================================================================*/
void uisnd_clear_forced_audio_path(void);

/*==============================================================================
函数: 
    uisnd_snd
       
说明: 
    Play a sound
       
参数: 
    type [in]: 声音类型 type of sound 
    snd [in]:  音调类型  which sound
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_snd(uisnd_snd_type  type, snd_sound_id_type   snd);

/*==============================================================================
函数: 
    uisnd_sample_snd
       
说明: 
    Play a sample sound
       
参数: 
    type [in]: 声音类型 type of sound 
    snd [in]:  音调类型  which sound
    vol [in]: 音量
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_sample_snd(uisnd_snd_type  type, snd_sound_id_type snd, byte  vol);

/*==============================================================================
函数: 
    uisnd_snd_stop
       
说明: 
    Stop any sound
       
参数: 
    none
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_snd_stop(void);

/*==============================================================================
函数: 
    uisnd_multi
       
说明: 
    Play a multitone
       
参数: 
    dotx [in]: transmit over air?
    sound [in]: list of tones
    received [in]: ulti tones received from BS
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_multi(boolean  dotx, snd_compact_tone_type *sound, boolean received);

/*==============================================================================
函数: 
    uisnd_multi_stop
       
说明: 
    Stop any multi tone
       
参数: 
    none
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_multi_stop(void);

/*==============================================================================
函数: 
    uisnd_tone
       
说明: 
    Play a tone
       
参数: 
    type [in]: 声音类型 type of sound 
    tone [in]: 音调类型  which tone
    time [in]: 播放持续时间，单位为毫秒 duration in ms
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_tone(uisnd_snd_type  type,  snd_tone_type  tone, word   time);

/*==============================================================================
函数: 
    uisnd_sample_tone
       
说明: 
    Play a sample tone while adjusting volume
       
参数: 
    type [in]: 声音类型 type of sound
    tone [in]: 音调类型 which tone
    time [in]: 播放持续时间，单位为毫秒 duration in ms
    vol [in]: 播放使用的音量
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_sample_tone(uisnd_snd_type type, snd_tone_type tone, word time, byte vol);

/*==============================================================================
函数: 
    uisnd_dtmf
       
说明: 
    Play a dtmf - a special tone which may be transmitted, and is always
    played out the speaker
       
参数: 
    tone [in]: 声音类型
    time [in]: 播放持续时间，单位为毫秒
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_dtmf(snd_tone_type   tone,  word time);

#ifdef FEATURE_AUDIO_AGC
/*==============================================================================
函数: 
    uisnd_set_aagc
       
说明: 
    函数打开或关闭 Audio AGC 特征。
       
参数: 
    enable [in]: 开关标志
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_set_aagc(boolean enable);
#endif /* FEATURE_AUDIO_AGC */


/*==============================================================================
函数: 
    uisnd_tone_stop
       
说明: 
    Stop any tone
       
参数: 
    none
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_tone_stop(void);

/*==============================================================================
函数: 
    uisnd_snd_init
       
说明: 
    Initialize gbrewui -> snd buffers
       
参数: 
    none
    
返回值:
    none
       
备注:
    函数仅被调用一次
    
==============================================================================*/
void uisnd_snd_init(void);


/*==============================================================================
函数: 
    uisnd_key_sound
       
说明: 
    Do the beep associated with a key being pressed.
       
参数: 
    key [in]: 按键码
    time [in]: 持续时间
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_key_sound(word key,word time);


/*==============================================================================
函数: 
    uisnd_call_fail
       
说明: 
    play call fail sound
       
参数: 
    snd [in]: which sound
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_call_fail(snd_sound_id_type snd);

/*==============================================================================
函数: 
    uisnd_play_signal
       
说明: 
    play tone signal or ISDN alert or IS-54B alert
       
参数: 
    type [in]: tone signal/ISDN alerting/IS-54B alert
    signal [in]: indicate which signals to play
    pitch [in]: medium, high or low
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_play_signal(byte signal_type, byte signal, byte pitch);

#endif  /* #ifndef UIXSND_H */

