/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         U S E R   I N T E R F A C E   S O U N D   M A N A G E R

GENERAL DESCRIPTION
  This manages the nasty details of what volume and which path to use
  at which time.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1991-2006 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc60x0/apps/gbrewui/uixsnd.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/06   jks     Added forced audio path support
04/13/06   gmy     Fix compile fail assoc'd w/intro of CATAPP/GSTK to SC1x 
09/24/03   BPW     Added event sent to Dialer App when ring is complete
07/24/03    sg     Added support for stereo headset.
07/03/03   AAA     Fixed a Stereo problem, where we check to see if there is
                   a voice call active, then don't switch to Stereo
06/23/03   AAA     Fixed the problem where no audio was heard from stereo jack
                   while streaming
03/14/03   RI      Added busy alert sound type in uisnd_call_fail().
02/13/02    lz     Removed code that generated key beeps when in data call.
01/20/03   kar     Fixed compile warnings
01/17/03   pn      In uisnd_set_device() used gbrewui.bt_is_voice_path in place of
                   gbrewui.bt_hs_enabled.
11/06/02   kar     Mainlined F_MULTIPLE_RINGER_TYPES
10/10/02   jb      Merged in run-time switchable multimode code
10/02/02   kar     Code cleanup
09/02/02   kar     Added support for Pip Tone Signal and IS-54B Pip-Pip-Pip-Pip
                   alerting.
08/07/02   kar     Fixed featurization typo
07/24/02   kar     Fixed uisnd_snd_stop to stop sample sounds properly
07/23/02   kar     Posted RING_SIG to CoreApp
07/04/02   lz      Featurized some volume settings under FEATURE_UI_CORE.
06/04/02   YR      End key does not necessarily puts back to idle state during a call.
03/29/02   kar     Initial MM version
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
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cai.h"
#include "db.h"
#endif

#include "uixsnd.h"
#include "oemui.h"

#include "assert.h"

#ifdef FEATURE_PHONE_VR
#include "uisvr.h"
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO
#include "uisvm.h"
#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_UI_CORE
#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#endif /* FEATURE_UI_CORE */

#define UI_HORN_ALERT()

#ifdef FEATURE_UI_CORE
#include "AEESound.h"
#include "AEEStdLib.h"
#endif

#ifdef FEATURE_UI_CORE
static AEESoundInfo  gsi;
#endif

static boolean  gbForceAudioPath = FALSE;
static snd_device_type  gForcedAudioDevice;
#include "OEMSVC.h"

extern AEESoundDevice OEMSound_GetAEESndDevice(snd_device_type  e);

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static snd_device_type out_device; 

extern brewui_global_data_type  gbrewui;

/*==============================================================================
函数: 
    uisnd_key_sound
       
说明: 
    Do the beep associated with a key being pressed.
       
参数: 
    key [in]: 虚拟键码
    time [in]: 持续时间
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_key_sound(AVKType key,word time)
{
    // Choose the right beep for the right key
    switch (key) 
    {
        // All these can be transmitted over the air, or not, so they
        // take special processing
        case AVK_POUND:
        case AVK_STAR:
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
            {
                snd_tone_type tone;
                
                if (key == AVK_POUND) 
                {
                    tone = SND_POUND; /* and sound */
                }  
                else 
                {
                    if (key == AVK_STAR) 
                    {
                        tone = SND_STAR;
                    }  
                    else 
                    {
                        // Assumes tones are in order
                        tone = (snd_tone_type)(key-AVK_0+(word)SND_0);
                    }
                }

                uisnd_tone(UI_BEEP_SND, tone, time);
            }
            break;
            
        // Control keys which are never transmitted
        case AVK_END:
        case AVK_POWER:
        case AVK_SEND:     
            if (key != (word)AVK_END || gbrewui.powerup) 
            { /* don't beep on powerup */
                uisnd_tone(UI_BEEP_SND, SND_CTRL, time);
            }
            break;


#ifdef FEATURE_NAV_KEYPAD
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
            uisnd_tone(UI_BEEP_SND, SND_2ND, time);
            break ;
#endif

        default:
            break ;
    }
}

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
void uisnd_call_fail(snd_sound_id_type snd)
{
    switch (snd) 
    {
        case SND_REORDER:
        case SND_ABRV_REORDER:
        case SND_INTERCEPT:
        case SND_ABRV_INTERCEPT:
        case SND_FADE_TONE:
        case SND_BUSY_ALERT:
        case SND_SPECIAL_INFO:
            uisnd_snd(UI_CALL_SND, snd);
            break;
        
        default:
            break;
    }
}

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*==============================================================================
函数: 
    play_tone_signal
       
说明: 
    play tone signal
       
参数: 
    signal [in]: indicate which signals to play
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static void play_tone_signal(byte signal)
{
    switch (signal) 
    {
        case CAI_TONE_DIAL:
            uisnd_snd(UI_CALL_EAR_SND, SND_DIAL_TONE);
            break;
            
        case CAI_TONE_RING:
            uisnd_snd(UI_CALL_EAR_SND, SND_RING_BACK);
            break;
            
        case CAI_TONE_BUSY:
            uisnd_snd(UI_CALL_EAR_SND, SND_BUSY_ALERT);
            break;
            
        case CAI_TONE_ANSWER:
            uisnd_snd(UI_CALL_EAR_SND, SND_ANSWER);
            break;
            
        case CAI_TONE_INTERCEPT:
            uisnd_call_fail(SND_INTERCEPT);
            break;
            
        case CAI_TONE_ABBR_INTERCEPT:
            uisnd_call_fail(SND_ABRV_INTERCEPT);
            break;
            
        case CAI_TONE_REORDER:
            uisnd_call_fail(SND_REORDER);
            break;
            
        case CAI_TONE_ABBR_REORDER:
            uisnd_call_fail(SND_ABRV_REORDER);
            break;

        case CAI_TONE_CONFIRM:
            uisnd_snd(UI_CALL_SND, SND_CONFIRMATION);
            break;
        
        case CAI_TONE_CALLWAIT:
            uisnd_snd(UI_CALL_SND, SND_CALL_WAITING);
            break;
            
        case CAI_TONE_PIP:
            uisnd_snd(UI_CALL_SND, SND_PIP_TONE);
            break;

        case CAI_TONE_OFF:
            uisnd_multi_stop();
            uisnd_snd_stop();
            break;
            
        default:
            uisnd_snd_stop();
            break;
    }
}

/*==============================================================================
函数: 
    play_snd_or_vibrator
       
说明: 
    playing vibrator or play sound in handset/module
       
参数: 
    type [in]: type of sound
    snd [in]: which sound
    
返回值:
    none
       
备注:
    depends on gbrewui.vibrator or gbrewui.onhook
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static void play_snd_or_vibrator(uisnd_snd_type type, snd_sound_id_type  snd)
{
    boolean vibrator_is_on;
    
    vibrator_is_on = FALSE;
    
    
    if (vibrator_is_on && !gbrewui.onhook) 
    {
        ;
    } 
    else 
    {
        uisnd_snd(type, snd);
    }
}

/*==============================================================================
函数: 
    play_isdn_alert
       
说明: 
    play IDSN alert
       
参数: 
    signal [in]: indicate which signals to play
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static void play_isdn_alert(byte signal)
{
    switch (signal) 
    {
        case CAI_ISDN_NORMAL:
            play_snd_or_vibrator(UI_ALERT_SND, SND_NORMAL_ALERT);
            UI_HORN_ALERT();
            break;
            
        case CAI_ISDN_INTERGROUP:
            play_snd_or_vibrator(UI_ALERT_SND,SND_INTR_GROUP_ALERT);
            UI_HORN_ALERT();
            break;
            
        case CAI_ISDN_SPECIAL:
            play_snd_or_vibrator(UI_ALERT_SND, SND_SPCL_ALERT);
            UI_HORN_ALERT();
            break;
            
        case CAI_ISDN_PING:
            // note!! in old UI, this is different from all other cases
            // design by purpose or  a bug ???
            uisnd_snd(UI_ALERT_SND, SND_PING_RING);
            break;
            
        case CAI_ISDN_OFF:
            uisnd_snd_stop();
            break;

        default:
            // follow old UI, treat it as CAI_ISDN_NORMAL
            play_snd_or_vibrator(UI_ALERT_SND, SND_NORMAL_ALERT);
            UI_HORN_ALERT();
            break;
    }
}

/*==============================================================================
函数: 
    play_is54b_alert
       
说明: 
    play IS54B alert
       
参数: 
    signal [in]: indicate which signals to play
    pitch [in]: medium, high or low
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static void play_is54b_alert(byte pitch,  byte signal)
{
#define IS54_SIGNAL_NUM        13 // there are 13 IS54 signals (values 0-12)
#define PITCH_LEVEL_NUM        3  // medium, high and low

    // sounds for IS54 signaling
    static const snd_sound_id_type is54[ IS54_SIGNAL_NUM ][ PITCH_LEVEL_NUM ] = 
    {
        { (snd_sound_id_type)0, (snd_sound_id_type)0, (snd_sound_id_type)0 },
        { SND_IS54B_LONG_M_ALERT, SND_IS54B_LONG_H_ALERT, SND_IS54B_LONG_L_ALERT },
        { SND_IS54B_SS_M_ALERT, SND_IS54B_SS_H_ALERT, SND_IS54B_SS_L_ALERT },
        { SND_IS54B_SSL_M_ALERT, SND_IS54B_SSL_H_ALERT, SND_IS54B_SSL_L_ALERT },
        { SND_IS54B_SS2_M_ALERT, SND_IS54B_SS2_H_ALERT, SND_IS54B_SS2_L_ALERT },
        { SND_IS54B_SLS_M_ALERT, SND_IS54B_SLS_H_ALERT, SND_IS54B_SLS_L_ALERT },
        { SND_IS54B_SSSS_M_ALERT, SND_IS54B_SSSS_H_ALERT, SND_IS54B_SSSS_L_ALERT },
        { SND_IS54B_PBX_LONG_M_ALERT, SND_IS54B_PBX_LONG_H_ALERT,
                                      SND_IS54B_PBX_LONG_L_ALERT },
        { SND_IS54B_PBX_SS_M_ALERT, SND_IS54B_PBX_SS_H_ALERT,
                                    SND_IS54B_PBX_SS_L_ALERT },
        { SND_IS54B_PBX_SSL_M_ALERT, SND_IS54B_PBX_SSL_H_ALERT,
                                     SND_IS54B_PBX_SSL_L_ALERT },
        { SND_IS54B_PBX_SLS_M_ALERT, SND_IS54B_PBX_SLS_H_ALERT,
                                     SND_IS54B_PBX_SLS_L_ALERT },
        { SND_IS54B_PBX_SSSS_M_ALERT, SND_IS54B_PBX_SSSS_H_ALERT,
                                     SND_IS54B_PBX_SSSS_L_ALERT },
        { SND_IS53A_PPPP_M_ALERT, SND_IS53A_PPPP_H_ALERT,
                                     SND_IS53A_PPPP_L_ALERT }
    };

    const int IS54_DEFAULT_SIGNAL   = 1;
    
    boolean vibrator_is_on;
    boolean multiple_ringer_enabled;
    
    vibrator_is_on = FALSE;
    
    
    multiple_ringer_enabled = TRUE;

    if (signal >= IS54_SIGNAL_NUM) 
    {
        // set to default for invalid signals
        signal = IS54_DEFAULT_SIGNAL;
    }
    if (pitch >= PITCH_LEVEL_NUM) 
    {
        // default pitch is medium
        pitch = CAI_PITCH_MED;
    }

    if (signal > 0) 
    {
        // verify array access won't be out of bound
        ASSERT(signal < ARR_SIZE(is54) );
        ASSERT(pitch < ARR_SIZE(is54[0]) );

        if (vibrator_is_on && !gbrewui.onhook) 
        {
            ;
        } 
        else 
        {
            uisnd_snd(UI_ALERT_SND, is54[signal][pitch]);
        }
    } 
    else 
    {
        uisnd_snd_stop();
    }
    
    UI_HORN_ALERT();
}

#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */


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
void uisnd_play_signal(byte signal_type, byte signal, byte pitch)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    switch (signal_type) 
    {
        case CAI_SIG_TONE:
            play_tone_signal(signal);
            break;
            
        case CAI_SIG_ISDN:
            play_isdn_alert(signal);
            break;
            
        case CAI_SIG_IS54B:
            play_is54b_alert(pitch, signal);
            break;
            
        default:
            MSG_HIGH("Unknown SIGNAL type: %d", signal_type, 0, 0);
            break;
    }
#endif
}


/*==============================================================================
函数: 
    choose_device
       
说明: 
    Choose the correct output device
       
参数: 
    type [in]: 声音类型
    
返回值:
    snd_device_type
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static snd_device_type choose_device(uisnd_snd_type type) 
{
    snd_device_type rval;
    
    switch (type) 
    {
        case UI_BEEP_SND:
        case UI_ALERT_SND:
        case UI_EARBEEP_SND:
        case UI_MSG_SND:
        case UI_CALL_SND:
        case UI_CALL_EAR_SND:
        case UI_DTMF_SND:
        case UI_SVC_ALERT_SND:
            rval = SND_DEVICE_CURRENT;  /* use current device */
            break;
            
        case UI_PWRUP_SND:
        case UI_EXT_PWR_SND:
            rval = SND_DEVICE_HANDSET;
            break;
            
        default:
            ERR( "Bad sound type in choose_device type = %d", type, 0, 0 );
            rval = SND_DEVICE_HFK;
            break ;
    }
    
    return(rval);
}

/*==============================================================================
函数: 
    choose_method
       
说明: 
    Choose the correct sound method
       
参数: 
    type [in]: 声音类型
    
返回值:
    snd_method_type
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static snd_method_type choose_method(uisnd_snd_type type)
{
    snd_method_type method;
    boolean headsetPresent = FALSE;
    
    (void)OEM_SVCGetConfig(CFGI_HEADSET_PRESENT, &headsetPresent, sizeof(headsetPresent));
    
    switch (type) 
    {
        case UI_BEEP_SND: 
            method = SND_METHOD_KEY_BEEP ;
            break ;

        case UI_PWRUP_SND: 
        case UI_EXT_PWR_SND:
            method = SND_METHOD_MESSAGE ;
            break;

        case UI_ALERT_SND:
            method = SND_METHOD_RING ;
            break;

        case UI_MSG_SND:
        case UI_EARBEEP_SND:
        case UI_CALL_SND:
            if (!(brewui_isincall() || headsetPresent))
            {
                if (type == UI_MSG_SND) 
                {
                    method = SND_METHOD_RING;
                }
                else 
                {
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
            if (brewui_isincall() || headsetPresent) 
            {
                method = SND_METHOD_VOICE ;
            }
            else 
            {
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
    
    return(method);
}


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
void uisnd_beep_vol(byte vol,  nv_items_enum_type nv_item)
{
    // 更新设置
    switch (nv_item)
    {
        case NV_BEEP_LVL_I :
            snd_set_volume(SND_DEVICE_HANDSET, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
            break;
            
        case NV_BEEP_SPKR_LVL_I :
            snd_set_volume(SND_DEVICE_HFK, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
#ifdef FEATURE_ANALOG_HFK
            snd_set_volume(SND_DEVICE_AHFK, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
#endif
#ifdef FEATURE_USB_ISOC_AUDIO
            snd_set_volume(SND_DEVICE_USB, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
#endif
            break;
            
        case NV_BEEP_LVL_SHADOW_I:
            snd_set_volume(SND_DEVICE_HEADSET, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
#ifdef FEATURE_TTY
            snd_set_volume(SND_DEVICE_TTY_HEADSET, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_HCO, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_VCO, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
#endif
            break;
            
#ifdef FEATURE_STEREO_DAC
        case NV_BEEP_SDAC_LVL_I:
            snd_set_volume(SND_DEVICE_SDAC, SND_METHOD_KEY_BEEP, vol, NULL, NULL);
            break;
#endif

        default:
            break;
    }
}

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
void uisnd_alert_vol(byte vol, nv_items_enum_type nv_item)
{
    // 更新设置
    switch (nv_item)
    {
        case NV_ALERTS_LVL_I:
            snd_set_volume(SND_DEVICE_HANDSET, SND_METHOD_MESSAGE, vol, NULL, NULL);
#ifdef FEATURE_STEREO_DAC
            snd_set_volume(SND_DEVICE_SDAC, SND_METHOD_MESSAGE, vol, NULL, NULL);
#endif
            break;
            
        case NV_ALERTS_LVL_SHADOW_I:
            snd_set_volume(SND_DEVICE_HEADSET, SND_METHOD_MESSAGE, vol, NULL, NULL);

#ifdef FEATURE_TTY
            snd_set_volume(SND_DEVICE_TTY_HEADSET, SND_METHOD_MESSAGE, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_HCO, SND_METHOD_MESSAGE, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_VCO, SND_METHOD_MESSAGE, vol, NULL, NULL);
#endif
            break;
            
        default :
            break ;
    }
}

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
void uisnd_voice_vol(byte vol, nv_items_enum_type nv_item)
{
    // 更新设置
    switch ( nv_item )
    {
        case NV_EAR_LVL_I:
            snd_set_volume(SND_DEVICE_HANDSET, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
            
        case NV_SPEAKER_LVL_I :
            snd_set_volume(SND_DEVICE_HFK, SND_METHOD_VOICE, vol, NULL, NULL);
#ifdef FEATURE_ANALOG_HFK
            snd_set_volume(SND_DEVICE_AHFK, SND_METHOD_VOICE, vol, NULL, NULL);
#endif
#ifdef FEATURE_USB_ISOC_AUDIO
            snd_set_volume(SND_DEVICE_USB, SND_METHOD_VOICE, vol, NULL, NULL);
#endif
            break;
            
        case NV_EAR_LVL_SHADOW_I:
            snd_set_volume(SND_DEVICE_HEADSET, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
            
#ifdef FEATURE_STEREO_DAC
        case NV_SDAC_LVL_I:
            snd_set_volume(SND_DEVICE_SDAC, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
#endif
        default :
            break ;
    }
}


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
void uisnd_tty_vol(byte vol, snd_device_type tty_dev)
{
    // 更新设置
    switch (tty_dev)
    {
        case SND_DEVICE_TTY_HEADSET:
            snd_set_volume(SND_DEVICE_TTY_HEADSET, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
            
        case SND_DEVICE_TTY_HCO:
            snd_set_volume(SND_DEVICE_TTY_HCO, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
            
        case SND_DEVICE_TTY_VCO:
            snd_set_volume(SND_DEVICE_TTY_VCO, SND_METHOD_VOICE, vol, NULL, NULL);
            break;
            
        default :
            break ;
    }
}
#endif

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
void uisnd_mm_vol(byte vol, nv_items_enum_type nv_item)
{
    // 更新设置
    switch (nv_item)
    {
        case NV_MM_LVL_I:
            snd_set_volume(SND_DEVICE_HANDSET, SND_METHOD_MIDI, vol, NULL, NULL);
            break;
            
        case NV_MM_LVL_SHADOW_I:
            snd_set_volume(SND_DEVICE_HEADSET, SND_METHOD_MIDI, vol, NULL, NULL);
            break;
            
        case NV_MM_SPEAKER_LVL_I:
            snd_set_volume(SND_DEVICE_HFK, SND_METHOD_MIDI, vol, NULL, NULL);
#ifdef FEATURE_ANALOG_HFK
            snd_set_volume(SND_DEVICE_AHFK, SND_METHOD_MIDI, vol, NULL, NULL);
#endif
#ifdef FEATURE_USB_ISOC_AUDIO
            snd_set_volume(SND_DEVICE_USB, SND_METHOD_MIDI, vol, NULL, NULL);
#endif
            break;
            
#ifdef FEATURE_STEREO_DAC
        case NV_MM_SDAC_LVL_I:
            snd_set_volume(SND_DEVICE_SDAC, SND_METHOD_MIDI, vol, NULL, NULL);
            break;
#endif
        default :
            break ;
    }
}

#endif /* FEATURE_MULTIMEDIA */


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
void uisnd_ring_vol(byte vol, nv_items_enum_type nv_item)
{
    // 更新设置
    switch (nv_item)
    {
        case NV_RINGER_LVL_I:
            snd_set_volume(SND_DEVICE_HANDSET, SND_METHOD_RING, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_HEADSET, SND_METHOD_RING, vol, NULL, NULL);
#ifdef FEATURE_STEREO_DAC
            snd_set_volume(SND_DEVICE_SDAC, SND_METHOD_RING, vol, NULL, NULL);
#endif
#ifdef FEATURE_TTY
            snd_set_volume(SND_DEVICE_TTY_HEADSET, SND_METHOD_RING, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_HCO, SND_METHOD_RING, vol, NULL, NULL);
            snd_set_volume(SND_DEVICE_TTY_VCO, SND_METHOD_RING, vol, NULL, NULL);
#endif
            break ;

        case NV_RINGER_SPKR_LVL_I:
            snd_set_volume(SND_DEVICE_HFK, SND_METHOD_RING, vol, NULL, NULL);
#ifdef FEATURE_ANALOG_HFK
            snd_set_volume(SND_DEVICE_AHFK, SND_METHOD_RING, vol, NULL, NULL);
#endif
#ifdef FEATURE_USB_ISOC_AUDIO
            snd_set_volume(SND_DEVICE_USB, SND_METHOD_RING, vol, NULL, NULL);
#endif
            break ;
            
        default :
            break ;
    }
}

/*==============================================================================
函数: 
    uisnd_GetSoundInfo
       
说明: 
    Get current sound info.
    psi->eMthod and psi->eAPath are of no consequence.
       
参数: 
    psi [in/out]: AEESoundInfo 类型缓冲区指针
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_GetSoundInfo(AEESoundInfo * psi)
{
    AEESoundMethod eMethod;
    
    gsi.eDevice = OEMSound_GetAEESndDevice(out_device);
    if (NULL != psi)
    {
        eMethod = psi->eMethod;
        MEMCPY(psi, &gsi, sizeof(AEESoundInfo));
        psi->eMethod = eMethod;
    }
}


/*==============================================================================
函数: 
    uisnd_SetSoundInfo
       
说明: 
    Set current sound info.
    psi->eMthod and psi->eAPath are of no consequence.
       
参数: 
    psi [in]: AEESoundInfo 类型缓冲区指针
    
返回值:
    none
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
void uisnd_SetSoundInfo(AEESoundInfo * psi)
{
    if (NULL == psi)
    {
        return;
    }
        
    MEMCPY(&gsi, psi, sizeof(AEESoundInfo));
}

/*==============================================================================
函数: 
    check_not_in_memo_vr
       
说明: 
    Check if phone is in voice memo or VR state
       
参数: 
    none
    
返回值:
    boolean
       
备注:
    调用本函数前，函数 uisnd_snd_init 已经被调用
    
==============================================================================*/
static boolean  check_not_in_memo_vr(void)
{
#ifdef FEATURE_VOICE_MEMO
    if (ui_is_in_vm())
    {
        FALSE; 
    }
#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_PHONE_VR
    if (ui_is_in_vr())
    {
        FALSE; 
    }
    if (uivr_incoming_call_in_prog())
    {
        FALSE; 
    }
    if (uivr_in_standby())
    {
        FALSE; 
    }
#endif
    
    return TRUE;
}

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
                      const void            *client_data)
{
    snd_device_type device = out_device;
    
    if (change_mute)
    {
        gsi.eEarMuteCtl = speaker_mute;
        gsi.eMicMuteCtl = microphone_mute;
    }

    if (gbForceAudioPath)
    {
        switch (gForcedAudioDevice )
        {
            case SND_DEVICE_STEREO_HEADSET:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_STEREO_HEADSET;
                break;
                
            case SND_DEVICE_HEADSET:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_HEADSET;
                break;
        
#ifdef FEATURE_TTY
            case SND_DEVICE_TTY_HEADSET:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_TTY_HEADSET;
                break;
                
            case SND_DEVICE_TTY_VCO:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_TTY_VCO;
                break;
                
            case SND_DEVICE_TTY_HCO:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_TTY_HCO;
                break;
#endif /* FEATURE_TTY */

            case SND_DEVICE_HFK:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_HFK;
                break;
#ifdef FEATURE_SPEAKER_PHONE
            case SND_DEVICE_SPEAKER_PHONE:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_SPEAKER_PHONE;
                break;
#endif
            case SND_DEVICE_HANDSET:
                gbrewui.spkr    = FALSE;
                out_device = SND_DEVICE_HANDSET;
                break;

#ifdef FEATURE_USB_ISOC_AUDIO
            case SND_DEVICE_USB:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_USB;
                break;
#endif /* FEATURE_USB_ISOC_AUDIO */

#ifdef FEATURE_ANALOG_HFK
            case SND_DEVICE_AHFK:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_AHFK;
                break;
#endif /* FEATURE_ANALOG_HFK */

#ifdef FEATURE_STEREO_DAC
            case SND_DEVICE_SDAC:
                gbrewui.spkr    = TRUE;
                out_device = SND_DEVICE_SDAC;
                break;
#endif /* FEATURE_STEREO_DAC */

            default :
                out_device = gForcedAudioDevice;
                break ;
        }
    }
    else // !gbForceAudioPath
    {
        boolean headsetPresent = FALSE;
        
        (void)OEM_SVCGetConfig(CFGI_HEADSET_PRESENT, &headsetPresent, sizeof(headsetPresent));
        if (headsetPresent)
        {
            gbrewui.spkr    = TRUE;
            
            if (gbrewui.headset_is_mono == FALSE)
            {
                out_device = SND_DEVICE_STEREO_HEADSET;
            }
            else
            {
                out_device = SND_DEVICE_HEADSET;
            }

#ifdef FEATURE_TTY
            if (gbrewui.headset_is_mono == TRUE)
            {
                if (gbrewui.tty_option == UI_TTY_FULL && check_not_in_memo_vr())
                {
                    out_device = SND_DEVICE_TTY_HEADSET;
                }
                else if (gbrewui.tty_option == UI_TTY_TALK && check_not_in_memo_vr())
                {
                    out_device = SND_DEVICE_TTY_VCO;
                }
                else if (gbrewui.tty_option == UI_TTY_HEAR && check_not_in_memo_vr())
                {
                    out_device = SND_DEVICE_TTY_HCO;
                }
            }
#endif /* FEATURE_TTY */
        }
        else if (gbrewui.onhook)
        {
            gbrewui.spkr    = TRUE;
#ifdef FEATURE_SPEAKER_PHONE
            out_device = SND_DEVICE_SPEAKER_PHONE;
#else
            out_device = SND_DEVICE_HFK;
#endif
        }
        else /* set device to handset */
        {
            gbrewui.spkr    = FALSE;
            out_device = SND_DEVICE_HANDSET;
        }

#ifdef FEATURE_USB_ISOC_AUDIO
        if (gbrewui.usb_hfk) 
        {
            gbrewui.spkr    = TRUE;
            out_device = SND_DEVICE_USB;
        }
#endif /* FEATURE_USB_ISOC_AUDIO */

#ifdef FEATURE_ANALOG_HFK
        if (gbrewui.analog_hfk)
        {
            gbrewui.spkr    = TRUE;
            out_device = SND_DEVICE_AHFK;
        }
#endif
#ifndef CUST_EDITION
#ifdef FEATURE_STEREO_DAC
        if (gbrewui.sdac)
        {
            /* Set path to SDAC only if not in alert or call */
            /* And not in VR or VM state */
            if (!ui_is_in_alert() 
#ifdef FEATURE_VOICE_MEMO
                && !ui_is_in_vm()
#endif /* FEATURE_VOICE_MEMO */
#ifdef FEATURE_PHONE_VR
                && !ui_is_in_vr()
#endif /* FEATURE_PHONE_VR */
                )
#else 
        if (1)
            {
                if(ui_is_in_call())
                {
                #ifndef CUST_EDITION
                    if(!ISVOICE(gbrewui.evt_param.call_event.call_info.call_type))
                #endif                        
                    {
                        gbrewui.spkr    = TRUE;
                        out_device = SND_DEVICE_SDAC;
                    }
                }
                else
                {
                    gbrewui.spkr    = TRUE;
                    out_device = SND_DEVICE_SDAC;
                }
            }
        }
#endif
#endif/*CUST_EDTION*/

    }
    
    /* Finally call SND with the right device and mute status */
    if (out_device >= SND_DEVICE_MAX && out_device != SND_DEVICE_CURRENT)
    {
        snd_device_type tepdevice = out_device;
        out_device = device;
        device = tepdevice;
    }
    else
    {
        device = out_device;
    }
    snd_set_device(device,gsi.eEarMuteCtl,
              gsi.eMicMuteCtl, callback_ptr, client_data);
    //DBGPRINTF("snd_set_device %d %d %d",device,gsi.eEarMuteCtl,gsi.eMicMuteCtl);
}

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
uint32 uisnd_get_forced_audio_path(void)
{
    if (gbForceAudioPath)
    {
        return(gForcedAudioDevice);
    }
    
    return(SND_DEVICE_CURRENT);
}

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
                                 const void             *client_data)
{
    gbForceAudioPath = TRUE;
    
    gForcedAudioDevice = new_device;
    
    uisnd_set_device(speaker_mute, microphone_mute, TRUE, callback_ptr, client_data);
}

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
void uisnd_clear_forced_audio_path(void)
{
    gbForceAudioPath = FALSE;
    if(brewui_isincall() == FALSE)
    {
        uisnd_set_device(SND_MUTE_MUTED, SND_MUTE_MUTED, TRUE, NULL, NULL);
    }
}

/*==============================================================================
函数: 
    uisnd_cb_function3
       
说明: 
    This function performs the call back called from sound server indicating
    status and results from the requested command.
       
参数: 
    client_data [in]: 指向客户数据缓冲区的指针
    status [in]: 声音任务状态
    
返回值:
    none
       
备注:
    A context switch might occurs
    
==============================================================================*/
void uisnd_cb_function1( void * client_data, snd_status_type status )
{
    if (status == SND_REPEAT)
    {
        (void)rex_set_sigs( &ui_tcb, UI_RING_SIG);
    }
}

/*==============================================================================
函数: 
    uisnd_cb_function3
       
说明: 
    This function performs the call back called from sound server indicating
    status and results from the requested command.
       
参数: 
    client_data [in]: 指向客户数据缓冲区的指针
    status [in]: 声音任务状态
    
返回值:
    none
       
备注:
    A context switch might occurs
    
==============================================================================*/
void uisnd_cb_function3( void * client_data, snd_status_type status )
{
    if (status == SND_REPEAT) 
    {
#ifdef FEATURE_THINUI
#if defined (FEATURE_APP_DIALER) && defined (FEATURE_ICM) && !defined(T_QSC60X0)
        IShell *pIShell = AEE_GetShell();
        ISHELL_PostEventEx(pIShell, 
                           EVTFLG_ASYNC, AEECLSID_DIALER,
                           EVT_USER,
                           EVT_DIALER_RING_COMPLETE, 0);
#endif // defined (FEATURE_APP_DIALER) && defined (FEATURE_ICM)
#endif /*FEATURE_THINUI*/
    }
}

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
void uisnd_snd(uisnd_snd_type  type, snd_sound_id_type   snd)
{
    if (type == UI_ALERT_SND)
    {
        snd_sound_id_start(choose_device(type), 
                           choose_method(type),
                           snd, 
                           SND_PRIO_MED, 
                           SND_APATH_LOCAL,
                           (snd_cb_func_ptr_type)uisnd_cb_function1,
                           NULL);
    } 
    else 
    {
        snd_sound_id_start(choose_device(type), 
                           choose_method(type),
                           snd, SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL, 
                           NULL);
    }
}

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
void uisnd_sample_snd(uisnd_snd_type  type, snd_sound_id_type snd, byte  vol)
{
    if( type == UI_ALERT_SND ) 
    {
        snd_sample_sound_id_start(choose_device(type), 
                                  choose_method(type), 
                                  snd, vol, 1,
                                  (snd_cb_func_ptr_type)uisnd_cb_function3, 
                                  NULL);
    } 
    else 
    {
        snd_sample_sound_id_start(choose_device(type), 
                                  choose_method(type), 
                                  snd, vol, 1,
                                  NULL, NULL);
    }
}


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
void uisnd_snd_stop(void)
{
    snd_priority_type pri = SND_PRIO_MED;

#ifdef FEATURE_MULTIMEDIA
    if (memcmp(gbrewui.mm_ringer_file,"Std",3) != 0 ) 
    {
        MSG_MED ("cmx_audfmt_stop", 0, 0, 0);
        cmx_audfmt_stop( NULL,NULL);
    }
#endif

    MSG_MED ("snd_stop_sound", 0, 0, 0);
    snd_sound_stop(pri, NULL, NULL);
}

/*==============================================================================
函数: 
    uisnd_cb_function2
       
说明: 
    This function performs the call back called from sound server indicating
    status and results from the requested command.
       
参数: 
    client_data [in]: 指向客户数据缓冲区的指针
    status [in]: 声音任务状态
    
返回值:
    none
       
备注:
    A context switch might occurs
    
==============================================================================*/
void uisnd_cb_function2(void *client_data, snd_status_type status)
{
    if (status == SND_PLAY_DONE)
    {
        // Cannot use rex_self. This routine runs under Sound task
        (void)rex_set_sigs(&ui_tcb, UI_MULTI_STOP_SIG);
    }
}

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
void uisnd_multi(boolean  dotx, snd_compact_tone_type *sound, boolean received)
{
    static snd_compact_sound_type multi_sound;
    static snd_compact_tone_type sound_buf[66];
    
    byte i;
    snd_apath_type dtmf_path = SND_APATH_LOCAL;
    snd_device_type device;
    snd_method_type method;
    
    for (i = 0; i < 66; i++)
    {
        sound_buf[i] = sound[i];
    }
    multi_sound.type       = SND_COMPACT_SOUND;
    multi_sound.tone_array = sound_buf;

    if (dotx)
    {
        dtmf_path = SND_APATH_BOTH;
    }
    if (received) 
    {
        device = choose_device( UI_CALL_SND );
        method = choose_method( UI_CALL_SND );
    } 
    else 
    {
        device = choose_device( UI_BEEP_SND );
        method = choose_method( UI_BEEP_SND );
    }

    (void) rex_clr_sigs(rex_self(), UI_MULTI_STOP_SIG);
    
    snd_sound_start(device, 
                    method, 
                    (snd_sound_type *) &multi_sound,
                    SND_PRIO_HIGH, 
                    dtmf_path,
                    (snd_cb_func_ptr_type)uisnd_cb_function2, NULL);
}


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
void uisnd_multi_stop(void)
{
    snd_sound_stop(SND_PRIO_HIGH, NULL, NULL);
}

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
void uisnd_tone(uisnd_snd_type  type,  snd_tone_type  tone, word   time)
{
    snd_device_type device;
    snd_method_type method;
    
    device  = choose_device(type);
    method  = choose_method(type);
    snd_tone_start(device, method, tone, time, SND_APATH_LOCAL, NULL, NULL);
}


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
void uisnd_sample_tone(uisnd_snd_type type, snd_tone_type tone, word time, byte vol)
{
    snd_device_type device;
    snd_method_type method;
    
    device  = choose_device( type );
    method  = choose_method( type );
    snd_sample_tone_start(device, method, tone, time, vol, NULL, NULL);
}

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
void uisnd_dtmf(snd_tone_type   tone,  word time)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    static db_items_value_type dbi;
#endif
    snd_device_type device;
    snd_method_type method;
    snd_apath_type  dtmf_path = SND_APATH_LOCAL;

    device  = choose_device(UI_DTMF_SND);
    method  = choose_method(UI_DTMF_SND);

    if (brewui_isincall()) 
    {// determine the path of the dtmfs
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
        db_get(DB_DIGITAL, &dbi);
        
        if (dbi.digital) 
        {
            dtmf_path = SND_APATH_LOCAL;
        } 
        else 
        {
            dtmf_path = SND_APATH_BOTH;
        }
#endif
    } 
    else 
    {// not in call
        dtmf_path = SND_APATH_LOCAL;
    }
    
    snd_tone_start(device, method, tone, time, dtmf_path, NULL, NULL);
}

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
void uisnd_tone_stop(void)
{
    snd_tone_stop(NULL, NULL);
}


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
void uisnd_snd_init(void)
{
#ifdef FEATURE_TTY
    gbrewui.tty_option = UI_TTY_OFF;
#endif
    out_device = SND_DEVICE_HANDSET;
    gsi.eDevice = OEMSound_GetAEESndDevice(out_device);
    gsi.eEarMuteCtl = (AEESoundMuteCtl)SND_MUTE_MUTED;
    gsi.eMicMuteCtl = (AEESoundMuteCtl)SND_MUTE_MUTED;
    gsi.eAPath = AEE_SOUND_APATH_LOCAL;
    gsi.eMethod = AEE_SOUND_METHOD_BEEP;
    
    gbrewui.headset_is_mono = TRUE;
#ifdef FEATURE_STEREO_DAC
    gbrewui.sdac = FALSE;     /* not using SDAC */
#endif
}


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
void uisnd_set_aagc(boolean enable)
{
    snd_agc_ctl(enable, NULL, NULL);
    snd_avc_ctl(enable, NULL, NULL);
}
#endif /* FEATURE_AUDIO_AGC */

