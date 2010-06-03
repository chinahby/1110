/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Sound Hardware Routines (2nd/3rd Generation)")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S O U N D   D A T A B A S E    R O U T I N E S

GENERAL DESCRIPTION
  These routines provide the routines for the sound database interface.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  sdb_get_sound
    Returns pointer to sequence of tones.
  sdb_free_sound
    Frees buffer of tones.

Copyright(c) 1999 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sdb.c#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/06    st     Modified code to handle SND_CMX_ADEC_READ_VAR case in
                   sdb_audfmt_data_cb(), CR89055.
02/17/06    sm     Added CEPT dial tone sound.
02/03/06    sm     Added CEPT ringing sound.
01/06/06    sm     Added CEPT sounds: SND_RADIO_PATH_ACK, 
                   SND_RADIO_PATH_NOT_AVAIL, SND_CEPT_CALL_WAITING
11/12/05    sm     Added GSM sounds:  SND_SUBSCRIBER_BUSY, SND_CONGESTION, 
                   SND_ERROR_INFO, SND_NUMBER_UNOBTAINABLE, SND_AUTH_FAILURE.
                   Fixed special_info_cadence_buf[] was not terminated.
08/02/05    st     Modified code to handle SND_CMX_AF_READ_VAR and
                   SND_CMX_AF_FREE_VAR_BUF status callbacks; modified code to
                   use malloc/free APIs.
01/25/05    st     Modified code to allocate an extra three samples for
                   waveform looping overlap requirement.
03/22/04    st     Fixed featurization.
01/26/04    sg     Modified to support DLS sounds.
03/10/03    st     Made length parameter in snd_audfmt_data_type 32-bit;
                   added example of how to include audio format file in SDB.
11/05/02    sg     Added snd_audfmt_data_type so that the data length can be
                   stored for audfmt sounds.
09/21/02    st     Modified is53a_pppp_l_alert_buf to repeat indefinitely.
04/04/01    st     Modified sdb_audfmt_data_cb() to support new interface.
02/28/01    st     Modified sdb_audfmt_data_cb() to support SND_CMX_AF_PREPARSE
                   command; removed WLL sounds; removed unnecessary
                   featurization; lint updates.
12/04/00    st     Added special_info_cadence_sound.
06/13/00    st     Modified code to remove msg_file compiler warning.
04/13/00    st     Added sdb_audfmt_data_cb() fucntion to support audio
                   format sounds in the sound database.
01/27/00    st     Added snd_hfk_call_orig_sound and FEATURE_PHONE_VR_HFK
                   to VR-HFK sounds.
09/09/99    st     Modified after code review.
08/17/99    st     Created file.

===========================================================================*/

#include "sdb.h"
#include <stdlib.h>             /* Dynamic memory allocation routines      */
#ifdef FEATURE_AUDIO_FORMAT
#include "msg.h"
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_AUDIO_FORMAT
/* Definition for holding AUDFMT sounds */
typedef struct
{
  uint32 length;
  void*  data;
} snd_audfmt_data_type;
#endif /* FEATURE_AUDIO_FORMAT */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Pre-defined sounds
**
** These sounds are lists of tones and looping constructs.  Most of the
** ringing sequences are pairs of tones modulated at some rate (e.g.
** 10 or 20 Hz), separated by periods of silence.
*/

/* IS-95 standard alert, on for 2s, then 4s of silence, repeating.
** See IS-95 section 7.7.5.5
*/
const snd_compact_tone_type ring_buf[] = {    /* Phone Ringing (Alert) */
  { SND_RING_A,            25 },
  { SND_RING_B,            25 },
  { SND_LOOP_BACK2,        39 },                 /* 2 seconds on  */
  { SND_CALLBACK_SILENCE,4000 },                 /* 4 seconds off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type ring_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) ring_buf
};

const snd_compact_tone_type pwrup_buf[] = {   /* Wake-up/Power-up sound */
  { SND_SILENCE,          500 },
  { SND_PWRUP,            500 },
  { SND_STOP,               0 }
};

const snd_compact_sound_type pwrup_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) pwrup_buf
};

/* Sounds for EIA/TIA Wideband Spread Spectrum Standard section 7.7.5.5
*/

/* Table 7.7.5.5-3, Tone Signals -  -  -  -  -  -  -  -  -  -  -  -  -  -
** (mostly call progress tones)
*/

const snd_compact_tone_type dial_tone_buf[] = {   /* Dial tone          */
  { SND_DIAL_TONE_TONE,  1000 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type dial_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) dial_tone_buf
};

const snd_compact_tone_type call_buf[] = {    /* Ringback, audible ring */
  { SND_RBACK,           2000 },                 /* 2 seconds on  */
  { SND_SILENCE,         4000 },                 /* 4 seconds off */
  { SND_RPT,                0 }
};

const snd_compact_sound_type call_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) call_buf
};

const snd_compact_tone_type int_buf[] = {     /* Intercept              */
  { SND_INTERCEPT_A,      250 },
  { SND_INTERCEPT_B,      250 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type int_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) int_buf
};

const snd_compact_tone_type abrv_int_buf[] = {/* Intercept              */
  { SND_INTERCEPT_A,      250 },
  { SND_INTERCEPT_B,      250 },
  { SND_LOOP_BACK2,         7 },                 /* 4 Seconds              */
  { SND_STOP,               0 }
};

const snd_compact_sound_type abrv_int_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) abrv_int_buf
};

const snd_compact_tone_type reord_buf[] = {   /* Reorder                */
  { SND_REORDER_TONE,     250 },                 /*   (Network congestion) */
  { SND_SILENCE,          250 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type reord_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) reord_buf
};

const snd_compact_tone_type abrv_reord_buf[] = { /* Abbreviated Reorder */
  { SND_REORDER_TONE,     250 },                 /*   (Network congestion) */
  { SND_SILENCE,          250 },
  { SND_LOOP_BACK2,         7 },                 /* 4 seconds              */
  { SND_STOP,               0 }
};

const snd_compact_sound_type abrv_reord_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) abrv_reord_buf
};

const snd_compact_tone_type busy_buf[] = {    /* Abbrev. Alert (busy)   */
  { SND_BUSY,             500 },
  { SND_SILENCE,          500 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type busy_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) busy_buf
};

const snd_compact_tone_type confirm_tone_buf[] = { /* Confirmation Tone  */
  { SND_DIAL_TONE_TONE,   100 },
  { SND_SILENCE,          100 },
  { SND_LOOP_BACK2,         2 },                  /* 3 notes */
  { SND_STOP,               0 }
};

const snd_compact_sound_type confirm_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) confirm_tone_buf
};

/* Answer tone (European call progress signal, use unknown) */
const snd_compact_tone_type answer_buf[] = {   /* Answer tone            */
  { SND_ANSWER_TONE,      500 },
  { SND_STOP,               0 }
};

const snd_compact_sound_type answer_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) answer_buf
};

const snd_compact_tone_type call_waiting_buf[] = { /* Call Waiting       */
  { SND_CALL_WT_TONE,     200 },
  { SND_STOP,               0 }
};

const snd_compact_sound_type call_waiting_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) call_waiting_buf
};

const snd_compact_tone_type off_hook_buf[] = { /* Off-Hook Warning       */
  { SND_OFF_HOOK_TONE,    100 },
  { SND_SILENCE,          100 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type off_hook_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) off_hook_buf
};

/* Table 7.7.5.5-4, ISDN Alerting -  -  -  -  -  -  -  -  -  -  -  -  -  -
*/

const snd_compact_tone_type normal_alert_buf[] = { /* Normal Alerting    */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        39 },                  /* 2 seconds on  */
  { SND_CALLBACK_SILENCE,4000 },                  /* 4 seconds off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type normal_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) normal_alert_buf
};

/* Intergroup Alerting    */
const snd_compact_tone_type intergroup_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /* 800 ms on  */
  { SND_CALLBACK_SILENCE, 400 }, /* 400 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type intergroup_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) intergroup_alert_buf
};

/* Special/Priority Alerting */
const snd_compact_tone_type special_alert_buf[] = { 
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /* 800 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /* 800 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        31 }, /* 1600 ms on  */
  { SND_CALLBACK_SILENCE,4000 }, /* 4000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type special_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) special_alert_buf
};

const snd_compact_tone_type ping_ring_buf[] = {   /* "Ping ring"          */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 },                 /* 500 ms on */
  { SND_STOP,               0 }
};

const snd_compact_sound_type ping_ring_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) ping_ring_buf
};

/* Table 7.7.5.5-5, IS-54B Alerting (High/Med/Low Tone) -  -  -  -  -  -  -  -
**
** One entry per pitch (High/Medium/Low) per pattern type (11 types)
**   (12 Types with IS-53A Addition)
**
*/

const snd_compact_tone_type is54b_long_h_alert_buf[] = {  /* Long      */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        39 },                         /* 2 seconds on  */
  { SND_CALLBACK_SILENCE,4000 },                         /* 4 seconds off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_long_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_long_h_alert_buf
};

const snd_compact_tone_type is54b_ss_h_alert_buf[] = {/* Short-short   */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        15 },                         /* 800 ms on     */
  { SND_CALLBACK_SILENCE, 400 },                         /* 400 ms off    */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss_h_alert_buf
};

const snd_compact_tone_type is54b_ssl_h_alert_buf[] = {/* Short-short-long */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 },                         /*  400 ms on       */
  { SND_SILENCE,          200 },                         /*  200 ms off      */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 },                         /*  400 ms on       */
  { SND_SILENCE,          200 },                         /*  200 ms off      */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        15 },                         /*  800 ms on       */
  { SND_CALLBACK_SILENCE,4000 },                         /* 4000 ms off      */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssl_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssl_h_alert_buf
};

const snd_compact_tone_type is54b_ss2_h_alert_buf[] = {/* Short-short-2    */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,         1000 }, /* 1000 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss2_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss2_h_alert_buf
};

const snd_compact_tone_type is54b_sls_h_alert_buf[] = {/* Short-long-short */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_sls_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_sls_h_alert_buf
};

/* Short-short-short-short */
const snd_compact_tone_type is54b_ssss_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,2500 }, /* 2500 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssss_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssss_h_alert_buf
};

const snd_compact_tone_type is54b_pbx_long_h_alert_buf[] = { /* PBX Long  */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_long_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_long_h_alert_buf
};

/* PBX Short-short */
const snd_compact_tone_type is54b_pbx_ss_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ss_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ss_h_alert_buf
};

/* PBX Short-short-long */
const snd_compact_tone_type is54b_pbx_ssl_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssl_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssl_h_alert_buf
};

/* PBX Short-long-short */
const snd_compact_tone_type is54b_pbx_sls_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_sls_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_sls_h_alert_buf
};

/* PBX Short-short-short-short  */
const snd_compact_tone_type is54b_pbx_ssss_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE, 800 }, /* 800 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssss_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssss_h_alert_buf
};

/* Pip-Pip-Pip-Pip */
const snd_compact_tone_type is53a_pppp_h_alert_buf[] = {
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_HIGH_PITCH_A,      25 },
  { SND_HIGH_PITCH_B,      25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_CALLBACK_SILENCE, 100 }, /* 100 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is53a_pppp_h_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is53a_pppp_h_alert_buf
};

const snd_compact_tone_type is54b_long_m_alert_buf[] = {  /* Long    */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        39 }, /* 2 seconds on  */
  { SND_CALLBACK_SILENCE,4000 }, /* 4 seconds off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_long_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_long_m_alert_buf
};

const snd_compact_tone_type is54b_ss_m_alert_buf[] = {    /* Short-short  */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /* 800 ms on  */
  { SND_CALLBACK_SILENCE, 400 }, /* 400 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss_m_alert_buf
};

const snd_compact_tone_type is54b_ssl_m_alert_buf[] = {/* Short-short-long */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_CALLBACK_SILENCE,4000 }, /* 4000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssl_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssl_m_alert_buf
};

const snd_compact_tone_type is54b_ss2_m_alert_buf[] = {/* Short-short-2   */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,         1000 }, /* 1000 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss2_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss2_m_alert_buf
};

const snd_compact_tone_type is54b_sls_m_alert_buf[] = {/* Short-long-short */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_sls_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_sls_m_alert_buf
};

/* Short-short-short-short */
const snd_compact_tone_type is54b_ssss_m_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,2500 }, /* 2500 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssss_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssss_m_alert_buf
};

const snd_compact_tone_type is54b_pbx_long_m_alert_buf[] = { /* PBX Long  */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_long_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_long_m_alert_buf
};

/* PBX Short-short */
const snd_compact_tone_type is54b_pbx_ss_m_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ss_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ss_m_alert_buf
};

/* PBX Short-short-long */
const snd_compact_tone_type is54b_pbx_ssl_m_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssl_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssl_m_alert_buf
};

/* PBX Short-long-short */
const snd_compact_tone_type is54b_pbx_sls_m_alert_buf[] = { 
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_sls_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_sls_m_alert_buf
};

/* PBX Short-short-short-short */
const snd_compact_tone_type is54b_pbx_ssss_m_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE, 800 }, /* 800 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssss_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssss_m_alert_buf
};

/* Pip-Pip-Pip-Pip  */
const snd_compact_tone_type is53a_pppp_m_alert_buf[] = {
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_CALLBACK_SILENCE, 100 }, /* 100 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is53a_pppp_m_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is53a_pppp_m_alert_buf
};

const snd_compact_tone_type is54b_long_l_alert_buf[] = {  /* Long          */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        39 }, /* 2 seconds on  */
  { SND_CALLBACK_SILENCE,4000 }, /* 4 seconds off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_long_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_long_l_alert_buf
};

const snd_compact_tone_type is54b_ss_l_alert_buf[] = {    /* Short-short   */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /* 800 ms on  */
  { SND_CALLBACK_SILENCE, 400 }, /* 400 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss_l_alert_buf
};

const snd_compact_tone_type is54b_ssl_l_alert_buf[] = {/* Short-short-long */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_CALLBACK_SILENCE,4000 }, /* 4000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssl_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssl_l_alert_buf
};

const snd_compact_tone_type is54b_ss2_l_alert_buf[] = {/* Short-short-2    */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,         1000 }, /* 1000 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ss2_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ss2_l_alert_buf
};

const snd_compact_tone_type is54b_sls_l_alert_buf[] = {/* Short-long-short */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,3000 }, /* 3000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_sls_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_sls_l_alert_buf
};

/* Short-short-short-short */
const snd_compact_tone_type is54b_ssss_l_alert_buf[] = { 
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_SILENCE,          500 }, /*  500 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /*  500 ms on  */
  { SND_CALLBACK_SILENCE,2500 }, /* 2500 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_ssss_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_ssss_l_alert_buf
};

const snd_compact_tone_type is54b_pbx_long_l_alert_buf[] = { /* PBX Long */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        19 }, /* 1000 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_long_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_long_l_alert_buf
};

/* PBX Short-short */
const snd_compact_tone_type is54b_pbx_ss_l_alert_buf[] = {
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE,2000 }, /* 2000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ss_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ss_l_alert_buf
};

/* PBX Short-short-long */
const snd_compact_tone_type is54b_pbx_ssl_l_alert_buf[] = { 
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssl_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssl_l_alert_buf
};

/* PBX Short-long-short */
const snd_compact_tone_type is54b_pbx_sls_l_alert_buf[] = {
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,        15 }, /*  800 ms on  */
  { SND_SILENCE,          200 }, /*  200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /*  400 ms on  */
  { SND_CALLBACK_SILENCE,1000 }, /* 1000 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_sls_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_sls_l_alert_buf
};

/* PBX Short-short-short-short */
const snd_compact_tone_type is54b_pbx_ssss_l_alert_buf[] = {
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_SILENCE,          200 }, /* 200 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         7 }, /* 400 ms on  */
  { SND_CALLBACK_SILENCE, 800 }, /* 800 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is54b_pbx_ssss_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is54b_pbx_ssss_l_alert_buf
};

/* Pip-Pip-Pip-Pip  */
const snd_compact_tone_type is53a_pppp_l_alert_buf[] = { 
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_LOW_PITCH_A,       25 },
  { SND_LOW_PITCH_B,       25 },
  { SND_LOOP_BACK2,         1 }, /* 100 ms on  */
  { SND_CALLBACK_SILENCE, 100 }, /* 100 ms off */
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type is53a_pppp_l_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) is53a_pppp_l_alert_buf
};

/* Additional Alerts -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
*/

const snd_compact_tone_type fade_tone_buf[] = {   /* Fade tone             */
  { SND_SILENCE,          150 }, /* 150ms delay on fade start for voc reset */
  { SND_SILENCE,           25 },
  { SND_LOW_PITCH_A,      120 },
  { SND_LOOP_BACK2,         3 }, /* 4 notes */
  { SND_STOP,               0 }
};

const snd_compact_sound_type fade_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) fade_tone_buf
};

const snd_compact_tone_type svc_change_buf[] = {  /* Service change tone   */
  { SND_SILENCE,           25 },
  { SND_MED_PITCH_A,      120 },
  { SND_LOOP_BACK2,         1 }, /* 2 notes */
  { SND_STOP,               0 }
};

const snd_compact_sound_type svc_change_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) svc_change_buf
};

/* changed durations from 250 to 175 and number from 5 to 4 */
const snd_compact_tone_type message_waiting_buf[] = { /* Msg Waiting Alert    */
  { SND_MSG_WAITING,      175 },
  { SND_SILENCE,          175 },
  { SND_LOOP_BACK2,         3 },                     /* Four Beeps           */
  { SND_STOP,               0 }
};

const snd_compact_sound_type message_waiting_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) message_waiting_buf
};

const snd_compact_tone_type abrv_alert_buf[] = {      /* Abreviate Alert  */
  { SND_MED_PITCH_A,       25 },
  { SND_MED_PITCH_B,       25 },
  { SND_LOOP_BACK2,         9 }, /* 500 ms on */
  { SND_STOP,               0 }
};

const snd_compact_sound_type abrv_alert_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) abrv_alert_buf
};

const snd_compact_tone_type pip_tone_buf[] = {       /* Pip-Pip-Pip-Pip  */
  { SND_PIP_TONE_TONE,    100 }, /* 100 ms on  */
  { SND_SILENCE,          100 }, /* 100 ms off */
  { SND_LOOP_BACK2,         3 }, /* Four Beeps */
  { SND_STOP,               0 }
};

const snd_compact_sound_type pip_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) pip_tone_buf
};

/* Any new sound tables will be defined here */

/* Sound for call-failed or reorder for some configurations.
*/
const snd_compact_tone_type alternate_reorder_buf[] = {
  { SND_INTERCEPT_A,       90 },
  { SND_SILENCE,           90 },
  { SND_LOOP_BACK2,         2 },
  { SND_SILENCE,          710 },
  { SND_INTERCEPT_A,       90 },
  { SND_SILENCE,           90 },
  { SND_LOOP_BACK2,         2 },
  { SND_STOP,               0 }
};

const snd_compact_sound_type alternate_reorder_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) alternate_reorder_buf
};

/* DTACO roam tone        */
const snd_compact_tone_type dtaco_roam_tone_buf[] = {    
  { SND_DIAL_TONE_TONE,    1100 },
  { SND_DT_DTACO_TONE,       60 },
  { SND_DIAL_TONE_TONE,     900 },
  { SND_RPT_NOCALLBACK,       0 }
};

const snd_compact_sound_type dtaco_roam_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) dtaco_roam_tone_buf
};

/* Sound tables defining user-selectable rings
*/
const snd_compact_tone_type user_ring_1_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_RING_A,            20 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            20 },
  { SND_SILENCE,          340 },
  { SND_BACK_TO_LABEL,      3 },
  { SND_CALLBACK_SILENCE,2400 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_1_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_1_buf
};

const snd_compact_tone_type user_ring_2_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_RING_A,            20 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            20 },
  { SND_SILENCE,           20 },
  { SND_BACK_TO_LABEL,      3 },
  { SND_SILENCE,          645 },
  { SND_LABEL,              0 },
  { SND_RING_A,            20 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            20 },
  { SND_SILENCE,           20 },
  { SND_BACK_TO_LABEL,      3 },
  { SND_SILENCE,          645 },
  { SND_LABEL,              0 },
  { SND_RING_A,            20 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            20 },
  { SND_SILENCE,           20 },
  { SND_BACK_TO_LABEL,      3 },
  { SND_SILENCE,          645 },
  { SND_LABEL,              0 },
  { SND_RING_A,            20 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            20 },
  { SND_SILENCE,           20 },
  { SND_BACK_TO_LABEL,      3 },
  { SND_CALLBACK_SILENCE,3750 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_2_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_2_buf
};

const snd_compact_tone_type user_ring_3_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_RING_A,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_C,            30 },
  { SND_SILENCE,           20 },
  { SND_BACK_TO_LABEL,      5 },
  { SND_CALLBACK_SILENCE,2000 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_3_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_3_buf
};

const snd_compact_tone_type user_ring_4_buf[] = { 
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_RING_A,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_C,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_C,            30 },
  { SND_SILENCE,          240 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_SILENCE,         1120 },
  { SND_LABEL,              0 },
  { SND_RING_A,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_C,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_B,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_C,            30 },
  { SND_SILENCE,          240 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_CALLBACK_SILENCE,2760 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_4_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_4_buf
};

/* Sound tables defining user-selectable rings that use volume scaling.
*/
const snd_compact_tone_type user_ring_5_buf[] = { 
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
 { SND_SILENCE,            0 },
 { SND_LABEL,              0 },
 { SND_VOL_SCALE,          0 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          1 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          0 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          1 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          1 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          2 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          1 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          2 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          3 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          3 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          3 },
 { SND_RING_E6,           60 },
 { SND_VOL_SCALE,          3 },
 { SND_RING_A7,           60 },
 { SND_VOL_SCALE,          0 },
 { SND_BACK_TO_LABEL,      1 },
 { SND_CALLBACK_SILENCE,2760 },
 { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_5_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_5_buf
};

const snd_compact_tone_type user_ring_6_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,          100 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,          100 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,          100 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,          100 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,          100 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_D,           100 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,          100 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_CALLBACK_SILENCE,2760 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_6_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_6_buf
};

const snd_compact_tone_type user_ring_7_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_C5,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_E5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_G5,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_C6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_C6,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_G5,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_E5,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_C5,           60 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_CALLBACK_SILENCE,2760 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_7_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_7_buf
};

const snd_compact_tone_type user_ring_8_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_C4,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_CS4,          60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_D4,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_DS4,          60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_E4,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_F4,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_G4,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_GS4,          60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_C5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_CS5,          60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_D5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_DS5,          60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_E5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_F5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_G5,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_GS5,          60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_C6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_D6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_DS6,          60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_F6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_G6,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_GS6,          60 },
  { SND_CALLBACK_SILENCE,2760 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_8_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_8_buf
};

const snd_compact_tone_type user_ring_9_buf[] = {
// The first tone is a zero duration silence to compensate for a bug in SND
// that does not see control tones when they are first.
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_E6,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          0 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          1 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          2 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,           60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_D,            60 },
  { SND_VOL_SCALE,          3 },
  { SND_RING_A7,           60 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_CALLBACK_SILENCE,2760 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type user_ring_9_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) user_ring_9_buf
};

const snd_compact_tone_type roam_ring_1_buf[]= {
  { SND_SILENCE,            0 },
  { SND_LABEL,              0 },
  { SND_RING_A,            30 },
  { SND_RING_B,            30 },
  { SND_SILENCE,           20 },
  { SND_RING_A,            30 },
  { SND_RING_C,            30 },
  { SND_SILENCE,           20 },
  { SND_LOOP_BACK2,         2 },
  { SND_BACK_TO_LABEL,      1 },
  { SND_CALLBACK_SILENCE,2000 },
  { SND_RPT_NOCALLBACK,     0 }
};

const snd_compact_sound_type roam_ring_1_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) roam_ring_1_buf
};

/* These are Service Transition Alerts.
*/
/* Entering service alert          */
const snd_compact_tone_type snd_svc_acq_buf[] = {  
  { SND_RING_E4,          80 },
  { SND_SILENCE,          40 },
  { SND_RING_E4,          80 },        
  { SND_SILENCE,          40 },
  { SND_RING_G4,          80 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type snd_svc_acq_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_svc_acq_buf
};

/* Leaving service alert           */
const snd_compact_tone_type snd_svc_lost_buf[] = {     
  { SND_RING_E4,          80 },
  { SND_SILENCE,          40 },
  { SND_RING_E4,          80 },
  { SND_SILENCE,          40 },
  { SND_RING_C4,          80 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type snd_svc_lost_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_svc_lost_buf
};

/* Change to a more preferred service */
const snd_compact_tone_type snd_svc_chng_more_pref_buf[] = {
  { SND_RING_E4,          80 },        
  { SND_SILENCE,          40 }, 
  { SND_RING_G4,          80 },
  { SND_STOP,              0 } 
};

const snd_compact_sound_type snd_svc_chng_more_pref_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_svc_chng_more_pref_buf
};

/* Change to a less preferred service */
const snd_compact_tone_type snd_svc_chng_less_pref_buf[] = {
  { SND_RING_E4,          80 },
  { SND_SILENCE,          40 }, 
  { SND_RING_C4,          80 },
  { SND_STOP,              0 } 
};

const snd_compact_sound_type snd_svc_chng_less_pref_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_svc_chng_less_pref_buf
};

/* These sounds define detection or loss of External Power
*/
/* External power on sound            */
const snd_compact_tone_type snd_ext_pwr_on_buf[] = { 
  { SND_RING_D4,       50 },
  { SND_SILENCE,       50 },
  { SND_RING_FS4,      50 },
  { SND_STOP,           0 }
};

const snd_compact_sound_type snd_ext_pwr_on_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_ext_pwr_on_buf
};

/* External power off sound          */
const snd_compact_tone_type snd_ext_pwr_off_buf[] = { 
  { SND_RING_FS4,         50 },
  { SND_SILENCE,          50 },
  { SND_RING_D4,          50 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type snd_ext_pwr_off_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_ext_pwr_off_buf
};

/* Call answer sound when in HFK     */
const snd_compact_tone_type snd_vrhfk_call_received_buf[] = {
/* The sound silence at the start of this sound is here because it is possible
** that the first tone will be cut off while the vocoder is being acquired for
** the call. Only a temporary fix.
*/
  { SND_SILENCE,         250 },
  { SND_RING_B4,         125 },
  { SND_RING_D4,         125 },
  { SND_RING_FS4,        125 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type snd_vrhfk_call_received_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_vrhfk_call_received_buf
};

/* Call origination sound when in HFK     */
const snd_compact_tone_type snd_hfk_call_orig_buf[] = {
/* The sound silence at the start of this sound is here because it is possible
** that the first tone will be cut off while the vocoder is being acquired for
** the call. Only a temporary fix.
*/
  { SND_SILENCE,         250 },
  { SND_RING_AS4,        125 },
  { SND_RING_D4,         125 },
  { SND_RING_F4,         125 },
  { SND_RING_D4,         125 },
  { SND_RING_AS4,        125 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type snd_hfk_call_orig_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) snd_hfk_call_orig_buf
};


const snd_compact_tone_type special_info_cadence_buf[] = {    /*  */
  { SND_RING_AS5,        330 },
  { SND_RING_F5,         330 },
  { SND_RING_A6,         330 },
  { SND_SILENCE,        1000 },
  { SND_STOP,              0 }
};

const snd_compact_sound_type special_info_cadence_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) special_info_cadence_buf
};


const snd_compact_tone_type subscriber_busy_buf[] = {
  { SND_DT_TONE_BRAZIL,   500 }, /* 425Hz tone for 500 ms */
  { SND_SILENCE,          500 }, /* 500 ms silence        */
  { SND_RPT,              0 }
};

const snd_compact_sound_type subscriber_busy_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) subscriber_busy_buf
};


const snd_compact_tone_type congestion_buf[] = {
  { SND_DT_TONE_BRAZIL,   200 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,          200 }, /* 200 ms silence        */
  { SND_RPT,              0 }
};

const snd_compact_sound_type congestion_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) congestion_buf
};


const snd_flexible_tone_type error_information_buf[] = {
  { SND_FREQ,     330, 950, 950 }, /* 950Hz tone for 330 ms          */
  { SND_SILENCE, 1000,   0,   0 }, /* 1000 ms silence                */
  { SND_RPT,        2,   0,   0 }  /* Repeat everything 2 more times */
};

const snd_flexible_sound_type error_information_sound = {
  SND_FLEXIBLE_SOUND,
  (snd_flexible_tone_type *) error_information_buf
};


const snd_flexible_tone_type number_unobtainable_buf[] = {
  { SND_FREQ,     330, 1400, 1400 }, /* 1400Hz tone for 330 ms         */
  { SND_SILENCE, 1000,    0,    0 }, /* 1000 ms silence                */
  { SND_RPT,        2,    0,    0 }  /* Repeat everything 2 more times */
};

const snd_flexible_sound_type number_unobtainable_sound = {
  SND_FLEXIBLE_SOUND,
  (snd_flexible_tone_type *) number_unobtainable_buf
};


const snd_flexible_tone_type auth_failure_buf[] = {
  { SND_FREQ,     330, 1800, 1800 }, /* 1800Hz tone for 330 ms         */
  { SND_SILENCE, 1000,    0,    0 }, /* 1000 ms silence                */
  { SND_RPT,        2,    0,    0 }  /* Repeat everything 2 more times */
};

const snd_flexible_sound_type auth_failure_sound = {
  SND_FLEXIBLE_SOUND,
  (snd_flexible_tone_type *) auth_failure_buf
};


const snd_compact_tone_type radio_path_ack_buf[] = {
  { SND_DT_TONE_BRAZIL,   200 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,          500 }, /* 500 ms silence        */
  { SND_STOP,               0 }
};

const snd_compact_sound_type radio_path_ack_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) radio_path_ack_buf
};


const snd_compact_tone_type radio_path_not_avail_buf[] = {
  { SND_DT_TONE_BRAZIL,   200 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,          200 }, /* 200 ms silence        */
  { SND_RPT,                3 }, /* Repeat everything 3 more times */
  { SND_STOP,               0 }
};

const snd_compact_sound_type radio_path_not_avail_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) radio_path_not_avail_buf
};


const snd_compact_tone_type cept_call_waiting_buf[] = {
  { SND_DT_TONE_BRAZIL,   200 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,          600 }, /* 600 ms silence        */
  { SND_DT_TONE_BRAZIL,   200 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,         3000 }, /* 3000 ms silence        */
  { SND_RPT,                1 }, /* Repeat everything 1 more time */
  { SND_STOP,               0 }
};

const snd_compact_sound_type cept_call_waiting_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) cept_call_waiting_buf
};


const snd_compact_tone_type cept_ring_buf[] = {
  { SND_DT_TONE_BRAZIL,  1000 }, /* 425Hz tone for 200 ms */
  { SND_SILENCE,         4000 }, /* 600 ms silence        */
  { SND_RPT,                0 }  /* Repeat forever        */
};

const snd_compact_sound_type cept_ring_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) cept_ring_buf
};


const snd_compact_tone_type cept_dial_tone_buf[] = {   /* CEPT Dial tone    */
  { SND_DT_TONE_BRAZIL,  1000 },
  { SND_RPT,                0 }
};

const snd_compact_sound_type cept_dial_tone_sound = {
  SND_COMPACT_SOUND,
  (snd_compact_tone_type *) cept_dial_tone_buf
};


#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================
FUNCTION sdb_audfmt_data_cb

DESCRIPTION
  This routine is used as the data callback function for audio format sounds
  in the sound database.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void sdb_audfmt_data_cb (
  snd_af_data_cmd_enum command,
  void                 *audfmt_file,
  uint32               num,
  void                 *data
) {
  snd_af_read_cb_type    *read_cb_info_ptr;
  snd_af_preparse_type   *preparse_ptr;
  snd_audfmt_data_type   *audfmt_data = (snd_audfmt_data_type*)audfmt_file;
  snd_af_get_dls_cb_type *dls_cb_info_ptr;
  uint8                  *decode_buf = NULL;

  switch (command) {
    case SND_CMX_AF_READ:
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
    case SND_CMX_ADEC_READ:
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
      MSG_MED("Read Callback", 0, 0, 0);
      read_cb_info_ptr = (snd_af_read_cb_type *) data;
      if(read_cb_info_ptr->cb_func != NULL) {
        read_cb_info_ptr->cb_func(read_cb_info_ptr->client_data,
                                  &(((uint8 *)audfmt_data->data)[num]), 
                                  audfmt_data->length, num);
      }
      break;

    case SND_CMX_AF_READ_VAR:
#if defined(FEATURE_WEBAUDIO) || defined (FEATURE_MIDI_OUT_QCP)
    case SND_CMX_ADEC_READ_VAR:
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
      read_cb_info_ptr = (snd_af_read_cb_type *) data;
      if((data != NULL) && (read_cb_info_ptr->cb_func != NULL)) {
        if(read_cb_info_ptr->buffer_size > 0) {
          decode_buf = malloc(read_cb_info_ptr->buffer_size);
          if(decode_buf != NULL) {
            memcpy(&decode_buf[read_cb_info_ptr->offset],
                   &(((uint8 *)audfmt_data->data)[num]),
                   read_cb_info_ptr->buffer_size - read_cb_info_ptr->offset);

            read_cb_info_ptr->cb_func(read_cb_info_ptr->client_data,
                decode_buf,
                read_cb_info_ptr->buffer_size - read_cb_info_ptr->offset, num);
            break;
          }
        }
        read_cb_info_ptr->cb_func(read_cb_info_ptr->client_data, NULL, 0, num);
      }
      break;

#ifdef FEATURE_WEBAUDIO
    case SND_CMX_AF_FREE_DLS:
#endif /* FEATURE_WEBAUDIO */
    case SND_CMX_AF_FREE_VAR_BUF:
      if((data != NULL) && (((uint8 *) data < (uint8 *) audfmt_data->data) ||
                            ((uint8 *) data > ((uint8 *) audfmt_data->data +
                                               audfmt_data->length)))) {
        free(data);
      }
      break;

    case SND_CMX_AF_GET_DLS:
      MSG_MED("GET DLS Callback", 0, 0, 0);
      dls_cb_info_ptr = (snd_af_get_dls_cb_type *) data;
      if((data != NULL) && (dls_cb_info_ptr->cb_func != NULL)) {
#ifdef FEATURE_WEBAUDIO
        if((dls_cb_info_ptr->length > 0) &&
           ((dls_cb_info_ptr->position + dls_cb_info_ptr->length) <=
            audfmt_data->length)) {
          decode_buf = malloc((dls_cb_info_ptr->length + 3) << 2);
          if(decode_buf != NULL) {
            dls_cb_info_ptr->cb_func(dls_cb_info_ptr->client_data,
                    &(((uint8 *)audfmt_data->data)[dls_cb_info_ptr->position]),
                    decode_buf, dls_cb_info_ptr->length);
            break;
          }
        }
#endif /* FEATURE_WEBAUDIO */
        dls_cb_info_ptr->cb_func(dls_cb_info_ptr->client_data, NULL, NULL, 0);
      }
      break;

    case SND_CMX_AF_SET_TRACKS:
      MSG_HIGH("Set Tracks Callback", 0, 0, 0);
      *(uint32 *) data = audfmt_data->length;
      break;

    case SND_CMX_AF_PREPARSE:
      preparse_ptr = (snd_af_preparse_type *) data;
      if(preparse_ptr->preparse_func != NULL) {
        preparse_ptr->preparse_func(preparse_ptr->client_data);
      }
      break;

    case SND_CMX_AF_GET_FILE_LEN:
      *(uint32 *)data = audfmt_data->length;
      break;

    default:
      MSG_HIGH("Unhandled Callback: %d", command, 0, 0);
      break;
  }
}

/***************************************************************************
This is an example of how to add an audio format sound to the sound database.

*** This buffer contains the data of the audio format file (MIDI/WebAudio/QCP/
*** MP3/...).
const uint8 audfmt_data[] = {
#include "audfmt_data.txt"
};

*** This structure contains a pointer to the data and the size of the buffer
*** in number of bytes.
const snd_audfmt_data_type audfmt_buf = { sizeof(audfmt_data),
                                          (void *) audfmt_data
                                        };

*** This structure is the one that should be added to sdb_sound_ptrs[]
*** below.
const snd_audfmt_sound_type audfmt_sound = {
  SND_AUDIO_FORMAT_SOUND,
  { (void *) &audfmt_buf, sdb_audfmt_data_cb },
  0, SND_AUDFMT_START_NORMAL
};
****************************************************************************/

#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
/* Array of pointers to sound buffers
**
** This array is indexed by sdb_sounds.
**
*/

snd_sound_type const * const sdb_sound_ptrs[] = {
  (snd_sound_type *) &ring_sound,                   /* Phone Ringing (Alert)                     */
  (snd_sound_type *) &pwrup_sound,                  /* Wake-up/Power-up sound                    */

  (snd_sound_type *) &dial_tone_sound,              /* Dial tone                                 */
  (snd_sound_type *) &dtaco_roam_tone_sound,        /* DTACO roam tone                           */
  (snd_sound_type *) &call_sound,                   /* Ringback                                  */
  (snd_sound_type *) &int_sound,                    /* Intercept                                 */
  (snd_sound_type *) &reord_sound,                  /* Reorder                                   */
  (snd_sound_type *) &busy_sound,                   /* Abbreviated Alert (Busy)                  */
  (snd_sound_type *) &confirm_tone_sound,           /* Confirmation Tone                         */
  (snd_sound_type *) &call_waiting_sound,           /* Call Waiting                              */
  (snd_sound_type *) &answer_sound,                 /* Answer Tone                               */
  (snd_sound_type *) &off_hook_sound,               /* Off-Hook Warning                          */

  (snd_sound_type *) &normal_alert_sound,           /* Normal Alerting                           */
  (snd_sound_type *) &intergroup_alert_sound,       /* Intergroup Alerting                       */
  (snd_sound_type *) &special_alert_sound,          /* Special/Priority Alerting                 */
  (snd_sound_type *) &ping_ring_sound,              /* "Ping ring"                               */

  (snd_sound_type *) &is54b_long_h_alert_sound,     /* IS-54B High Long                          */
  (snd_sound_type *) &is54b_ss_h_alert_sound,       /* IS-54B High Short-short                   */
  (snd_sound_type *) &is54b_ssl_h_alert_sound,      /* IS-54B High Short-short-long              */
  (snd_sound_type *) &is54b_ss2_h_alert_sound,      /* IS-54B High Short-short-2                 */
  (snd_sound_type *) &is54b_sls_h_alert_sound,      /* IS-54B High Short-long-short              */
  (snd_sound_type *) &is54b_ssss_h_alert_sound,     /* IS-54B High Short-short-short-short       */
  (snd_sound_type *) &is54b_pbx_long_h_alert_sound, /* IS-54B High PBX Long                      */
  (snd_sound_type *) &is54b_pbx_ss_h_alert_sound,   /* IS-54B High PBX Short-short               */
  (snd_sound_type *) &is54b_pbx_ssl_h_alert_sound,  /* IS-54B High PBX Short-short-long          */
  (snd_sound_type *) &is54b_pbx_sls_h_alert_sound,  /* IS-54B High PBX Short-long-short          */
  (snd_sound_type *) &is54b_pbx_ssss_h_alert_sound, /* IS-54B High PBX Short-short-short-short   */
  (snd_sound_type *) &is53a_pppp_h_alert_sound,     /* IS-53A High Pip-Pip-Pip-Pip               */

  (snd_sound_type *) &is54b_long_m_alert_sound,     /* IS-54B Medium Long                        */
  (snd_sound_type *) &is54b_ss_m_alert_sound,       /* IS-54B Medium Short-short                 */
  (snd_sound_type *) &is54b_ssl_m_alert_sound,      /* IS-54B Medium Short-short-long            */
  (snd_sound_type *) &is54b_ss2_m_alert_sound,      /* IS-54B Medium Short-short-2               */
  (snd_sound_type *) &is54b_sls_m_alert_sound,      /* IS-54B Medium Short-long-short            */
  (snd_sound_type *) &is54b_ssss_m_alert_sound,     /* IS-54B Medium Short-short-short-short     */
  (snd_sound_type *) &is54b_pbx_long_m_alert_sound, /* IS-54B Medium PBX Long                    */
  (snd_sound_type *) &is54b_pbx_ss_m_alert_sound,   /* IS-54B Medium PBX Short-short             */
  (snd_sound_type *) &is54b_pbx_ssl_m_alert_sound,  /* IS-54B Medium PBX Short-short-long        */
  (snd_sound_type *) &is54b_pbx_sls_m_alert_sound,  /* IS-54B Medium PBX Short-long-short        */
  (snd_sound_type *) &is54b_pbx_ssss_m_alert_sound, /* IS-54B Medium PBX Short-short-short-short */
  (snd_sound_type *) &is53a_pppp_m_alert_sound,     /* IS-53A Medium Pip-Pip-Pip-Pip             */

  (snd_sound_type *) &is54b_long_l_alert_sound,     /* IS-54B Low Long                           */
  (snd_sound_type *) &is54b_ss_l_alert_sound,       /* IS-54B Low Short-short                    */
  (snd_sound_type *) &is54b_ssl_l_alert_sound,      /* IS-54B Low Short-short-long               */
  (snd_sound_type *) &is54b_ss2_l_alert_sound,      /* IS-54B Low Short-short-2                  */
  (snd_sound_type *) &is54b_sls_l_alert_sound,      /* IS-54B Low Short-long-short               */
  (snd_sound_type *) &is54b_ssss_l_alert_sound,     /* IS-54B Low Short-short-short-short        */
  (snd_sound_type *) &is54b_pbx_long_l_alert_sound, /* IS-54B Low PBX Long                       */
  (snd_sound_type *) &is54b_pbx_ss_l_alert_sound,   /* IS-54B Low PBX Short-short                */
  (snd_sound_type *) &is54b_pbx_ssl_l_alert_sound,  /* IS-54B Low PBX Short-short-long           */
  (snd_sound_type *) &is54b_pbx_sls_l_alert_sound,  /* IS-54B Low PBX Short-long-short           */
  (snd_sound_type *) &is54b_pbx_ssss_l_alert_sound, /* IS-54B Low PBX Short-short-short-short    */
  (snd_sound_type *) &is53a_pppp_l_alert_sound,     /* IS-53A Low Pip-Pip-Pip-Pip                */

  (snd_sound_type *) &fade_tone_sound,              /* Fade tone                                 */
  (snd_sound_type *) &svc_change_sound,             /* Service change tone                       */
  (snd_sound_type *) &svc_change_sound,             /* Pad for obsolete - Horn alert             */
  (snd_sound_type *) &abrv_reord_sound,             /* Abbreviated REORDER (4 Seconds)           */
  (snd_sound_type *) &abrv_int_sound,               /* Abbreviated INTERCEPT (4 Seconds)         */
  (snd_sound_type *) &alternate_reorder_sound,      /* Alternate Reorder sound (1.79 seconds)    */
  (snd_sound_type *) &message_waiting_sound,        /* Message Waiting Alert                     */
  (snd_sound_type *) &abrv_alert_sound,             /* Abbreviated Alert                         */
  (snd_sound_type *) &pip_tone_sound,               /* Pip-Pip-Pip-Pip In Call (Vocoder) Version */

  (snd_sound_type *) &roam_ring_1_sound,             /* Roaming Ring                             */

  (snd_sound_type *) &snd_svc_acq_sound,             /* Service acquired sound                   */
  (snd_sound_type *) &snd_svc_lost_sound,            /* Service lost sound                       */
  (snd_sound_type *) &snd_svc_chng_more_pref_sound,  /* Change to a more preferred service sound */
  (snd_sound_type *) &snd_svc_chng_less_pref_sound,  /* Change to a less preferred service sound */
  (snd_sound_type *) &snd_ext_pwr_on_sound,          /* External power on sound                  */
  (snd_sound_type *) &snd_ext_pwr_off_sound,         /* External power off sound                 */

  (snd_sound_type *) &user_ring_1_sound,            /* User selectable ring 1                    */
  (snd_sound_type *) &user_ring_2_sound,            /* User selectable ring 2                    */
  (snd_sound_type *) &user_ring_3_sound,            /* User selectable ring 3                    */
  (snd_sound_type *) &user_ring_4_sound,            /* User selectable ring 4                    */
  (snd_sound_type *) &user_ring_5_sound,            /* User selectable ring 5                    */
  (snd_sound_type *) &user_ring_6_sound,            /* User selectable ring 6                    */
  (snd_sound_type *) &user_ring_7_sound,            /* User selectable ring 7                    */
  (snd_sound_type *) &user_ring_8_sound,            /* User selectable ring 8                    */
  (snd_sound_type *) &user_ring_9_sound,            /* User selectable ring 9                    */

  (snd_sound_type *) &snd_vrhfk_call_received_sound,/* Call answer sound for when in HFK         */
  (snd_sound_type *) &snd_hfk_call_orig_sound,      /* Call origination sound for when in HFK    */
  (snd_sound_type *) &special_info_cadence_sound,   /* Special info sound                        */
  (snd_sound_type *) &subscriber_busy_sound,        /* Subscriber busy sound                     */
  (snd_sound_type *) &congestion_sound,             /* Congestion sound                          */
  (snd_sound_type *) &error_information_sound,      /* Error information sound                   */
  (snd_sound_type *) &number_unobtainable_sound,    /* Number unobtainable sound                 */
  (snd_sound_type *) &auth_failure_sound,           /* Authentication failure sound              */
  (snd_sound_type *) &radio_path_ack_sound,         /* Radio path acknowlegement sound           */
  (snd_sound_type *) &radio_path_not_avail_sound,   /* Radio path not available sound            */
  (snd_sound_type *) &cept_call_waiting_sound,      /* CEPT call waiting sound                   */
  (snd_sound_type *) &cept_ring_sound,              /* CEPT ring sound                           */
  (snd_sound_type *) &cept_dial_tone_sound,         /* CEPT dial tone                            */     

  NULL                                              /* MUST BE LAST - See snd_init() for Details */
}; /* end of sound table */
/* <EJECT> */
/*===========================================================================
FUNCTION sdb_get_sound

DESCRIPTION
  This routine returns a buffer with the requested sound.

DEPENDENCIES
  sdb_sound_ptrs

RETURN VALUE
  sdb_status_type
  snd_sound_type

SIDE EFFECTS
  None.
===========================================================================*/
sdb_status_type sdb_get_sound (
  snd_sound_id_type    index,         /* Index of sound to retrieve */
  const snd_sound_type **sound        /* Buffer with sound returned */
)
{
  if(index < SND_LAST_SND) {
    *sound = sdb_sound_ptrs[index];
    return( SDB_SUCCESS );
  } else {
    return( SDB_FAIL );
  }
}
/* <EJECT> */
/*===========================================================================
FUNCTION sdb_free_sound

DESCRIPTION
  This routine frees the memory that was allocated by sdb_get_sound().
  A ROM table implementation is currently employed, therefore there is nothing
  to do in this function.

DEPENDENCIES
  None.

RETURN VALUE
  sdb_status_type

SIDE EFFECTS
  None.
===========================================================================*/
sdb_status_type sdb_free_sound (
  const snd_sound_type *buf         /* Buffer to free */
  /*lint -esym(715,buf) Tell Lint we know buf parameter is not used */
)
{
  return( SDB_SUCCESS );
}
