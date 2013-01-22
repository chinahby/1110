#ifndef SNDI_H
#define SNDI_H
/*===========================================================================

    S O U N D   S E R V I C E S   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary for the Sound driver to
  interact with the Vocoder interface and the CXD1270/1271, not including
  the specifics defining the Vocoder interface.

REFERENCES
  MSM3100 User's Manual.

Copyright (c) 1992 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndi.h#14 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/09   yp      CMI changes for SCMM 
09/07/09   vsud    Changes for resampler image imeplementation.
07/07/09    rk     Notifying SND that PAs are turned on, wakes up sound task.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
03/10/09   sj      Added slowtalk feature support
01/19/09   dp      CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08   rm      Added RVE API changes
11/27/08   vsud    fix for 156536-same signal is used across different tasks.
10/02/08   vsud    Fixed CR# 157201 for SMAF file play back issues.
08/26/08   anb     Added support for DSP based WMAPRO
06/20/08   knm     Added Support for AuxPGA Line In loopback path setting
04/11/08   skr     Fixed Thin UI Build compilation issues for 6246
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
12/19/07    hs     Added support ISDB-T dual mono output control
12/08/07    sr     Buffer_underrun_flag moved to snd_sound_status_type .
                   Fix for CR - 127448
11/14/07    hs     Added QEnsemble support
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
05/28/07    pl     Moved SLEEP MACRO to snd.c 
04/07/07    pl     Addeded support for starting qsynth AMR-NB
                   decoder.
09/05/06    ay     Added AMR-WB+ support.
06/27/06    pl     Added environment & source 3d enable/disable and reset support.
06/26/06    pl     Defined new structures for doppler, reverb effect.
03/27/06    ak     Added support for sidetone control.
03/16/06   anb     Added support for sbc re-configuration given the encoded params
                   CR 78581,87334.Modified snd_sbc_params_packet_type definition.
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device.
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
11/16/05    sp     Changes to support SBC encoding in lower priority task.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Added support for QAudioFX: 3D positional commands and query
                   interface; added snd_adec_qcp_start() function prototype.
09/16/05    sp     Fixed a compiler error.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     VOX support
08/29/05    sp     Made changes so that SBC paramters could be changed for
                   testing purposes.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/22/05    sm     Added WAV A/Ulaw support.
06/03/05    st     Added aux_type to tone_status_type.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record funcationlity.
05/18/05    ac     Modified code to support new AV sync functionality.
04/19/05    aw     Modified code to not use hdr.callback_ptr for AudioPP funcs.
04/21/05    hz     Added VirtualPacketLength support for Windows Media
                   playback.
03/25/05   ac/sg   Added support for A/V synchronization.
03/02/05    aw     Added support for Qconcert.
02/14/05    hz     Added support for WMA decode under FEATURE_WMA.
02/05/05    st     Modified code to support DLS processing.
11/19/04    sp     Added support for Arm Post Processing.
08/11/04    st     Added SND_AUDFMT_CHECK_RESET command and
                   snd_audfmt_check_reset() function prototype.
06/08/04    aw     Added support for Equalizer and Spectrum analyzer.
04/22/04    lg     Added support for Animated AAC Ringers.
03/22/04    st     Added support for PCM playback and recording; updated
                   snd_audfmt_pause_seq() and snd_audfmt_set_vol_seq()
                   and snd_audfmt_adec_chan_flush() function prototypes.
                   Added SND_MIDI_OUT_QCP_PAUSE_CHAN and
                   SND_MIDI_OUT_QCP_RESUME_CHAN commands; updated
                   snd_audfmt_adec_chan_pause() function prototype; added
                   snd_audfmt_pause_seq() and snd_audfmt_set_vol_seq()
                   function prototypes.
01/26/04    lg     Modified code to support tune, tempo & pan adjustments and
                   volume, pause & stop of each sequence in multi_sequencer
01/26/04    aw     Added support to get file specs.
01/21/04    aw     Added support for qsynth volume mapping function.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/16/03  aw/st    Modified code to support YADPCM and audfmt multi-sequences.
10/10/03    sg     Modified to support additional AAC params reqd by the DSP.
           ymc     Updated copyright.
04/18/03    aw     Added audio support for Q3D image.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
03/12/03    sm     Sleep code also featurized under FEATURE_SLEEP_TASK
02/28/03    st     Added support for EC/NS control setting.
02/19/03  aw/st    Added support for playback of ADPCM file format.
11/19/02    st     Added snd_audfmt_synth_mvol() and snd_audfmt_adec_mvol()
                   function prototypes.
11/05/02    sg     Added support for iMelody format.
08/13/02   aw/st   Modified code to support ADPCM audio decoder.
           sg/st   Added support for MP4 and AAC; added support for flush
                   command.
07/11/02    yl     Added the new field for snd_qcp_rec_packet_type.
10/01/01    st     Modified code to add support for MIDI device ADEC functions,
                   including support for multiple channels.
09/16/01    st     Added SND_MIDI_MAX_VALUE for range checking purposes.
09/03/01    st     Added support for QCP recording and support functions
                   (set/get information).
07/30/01   ymc     Utilized SLEEP_ASSERT_OKTS and SLEEP_NEGATE_OKTS macros.
04/26/01    st     Added snd_adec_mode_type definition.
04/26/01    sm     Removed speed_factor from tone_status_type.
04/04/01    st     Modified code to queue command for vocoder VP signal
                   instead of setting REX signal; added support for MIDI device
                   output interface; removed stereo_pan parameter from 
                   tone_status_type and snd_sound_status_type; added length
                   parameter to snd_cmx_af_read_cb_packet_type.
02/28/01    st     Added support for the audio format preparse done mechanism;
                   modified snd_af_adec_int_packet_type to use a void pointer
                   instead of a snd_sound_status_type pointer.
12/04/00    sm     Added audfmt_song_config, stereo_pan to snd_sound_status_type
                   and tone_status_type. Add adec_chan_vol(), adec_chan_pan().
            sm     Added audfmt vars to snd_sound_status_type, tone_status_type.
07/15/00    st     Use 8bit value for volume levels; use 32bit values for time
                   durations; added SND_CHANGE_M (general sound change mask
                   value); added support for tune and tempo functions; added
                   snd_audfmt_wait() and snd_audfmt_continue() prototypes;
                   removed snd_get_packets() and snd_cmd() prototypes.
05/18/00    st     Modified defines to support another level of sound playback;
                   added support for sound flow command packets; added support
                   for audio format sounds; added snd_get_packet() and 
                   snd_cmd() function prototypes; added sound age function type.
03/06/00    st     Modified tone_index parameter in snd_sound_status_type to
                   16 bit value; removed FEATURE_ENHANCED_RINGS from scaling.
10/04/99   jkl     Removed SND_CODEC_VOL_M. 
                   Pad mask is read from calibration data.
09/09/99  jkl/st   Modifications after code review. Added AGC/AVC commands; 
                   only include msg.h if SND_DEBUG is defined to remove 
                   compiler warnings.
08/24/99    st     Added snd_click_state_type.
08/17/99   jkl     Modified for the new API
06/30/99   RPW     Integrated AGC feature with VR/VM features.
03/22/99    st     Modified snd_status_type; modified SND_MAX_PERIOD macro to
                   remove snd_analog_mode dependency.
03/04/99    st     Incorporated changes to support ha's vocoder frequency 
                   calculation routines: modifed snd_dtmf_type, added 
                   snd_mn_counter_type structure for the ringer, moved 
                   #defines for vocoder tone values in voc_core.h.
                   Also made modifications to support tone generation from
                   specified frequencies: modified tone_status_type. 
01/13/99    st     Added support for voice prompting; added VOL_MEMORY and 
                   VOL_CONST definitions from sndihw.h so that they can be used
                   in snd_vol_ctl_type.
12/04/98    sm     Support selectable ROM keyword for backward compatibility.
11/04/98    sm     Merged ARM/MSM3000 support to mainline.
10/08/98    st     HFK-VR mainlining: Added snd_hfk_vr_tone_state_type, 
                   snd_extcom_command_type, snd_ext_com_state_type, 
                   snd_hfk_aec_set_type, and snd_hfk_phs_set_type
10/08/98    st     Added SND_VERIFY_DFILT_IS_DISABLED macro and DATA_FILTERS
                   bitmask defines for volume override.
08/28/98    st     Added SND_TONE_VOC_TIMEOUT define. CR6033.
06/22/98    st     Added SND_GEN_ADSP in snd_gen_type for ADSP tone generation 
                   on SSS.
04/23/98    st     Added BIO_SW10 defines and macro for DOTG volume override
                   verification and ctl_flags field in snd_vol_ctl_type 
                   structure, fixes CR4888.
02/13/98    ha     Added scaling field to snd_pstate_type for HFK volume 
                   scaling support.
01/21/98    ha     Added scaling field to snd_status_type and 
                   tone_status_type to support volume scaling.
                   Added additional piano notes.
                   Updated copyright.
01/16/98    ro     Added SND_CODEC_VOL_M to support external communications
                   Added snd_device_type. Added device to snd_pstate_type.
                   Added device to tone_status_type.
                   Added snd_audio_process_type.
12/08/97    ha     Added click_state field to snd_status_type and to
                   tone_status_type
11/21/97    ha     Added DS6 tone entry (2489.1Hz)
10/23/97    ro     Change codec volume mask for non-HFK+, fixes CR 4190.
08/10/97    ro     Change SND_HFK_DEFAULT value to unmute car stereo.
07/23/97    ro     Moved ringer definitions to sndring.h
                   Added new tones to snd_dtmf_index_type.
                   Added new vocoder frequency constants.
07/01/97    ro     Remove #if (TG==??) in MSM register definitions.
06/17/97    ro     Added 425Hz vocoder tone.
06/06/97    ro     Get rid of hanging comma in snd_gen_type.
05/23/97    ro     Add phs_override to tone_status_type.
04/29/97    ro     Added control values for HFK PLUS.
04/25/97    ro     Change #if SND_DEBUG to #ifdef SND_DEBUG
03/28/97    ro     Support for TGP.  Add label_index and label_cnt to
                   snd_status_type to support custom alerts.
                   Changed snd_dtmf_type dtmf_ptr to snd_tone_type tone
                   in tone_status_type.
                   Add DEF_DTMF macro for SND_DEBUG builds.
                   Defined additional ringer and vocoder frequencies.
                   Remove all T_C1, no longer supported.
03/12/97   jjn     Added (TG==T_MD) for Module in conditional compile statements
02/12/97    ro     Added support for aux generators - SND_GEN_AUX.
01/07/97    ro     Added SND_FIRST_HFK_TONE_TIME.
                   Added SND_VOLUME_M, removed SND_SPKR_VOL_M, and
                   SND_PHONE_VOL_M.
                   Added snd_vol_ctl_type, snd_pstate_type, tone_status_type
                   Added sclass field to snd_status_type.
                   Changed vol from byte to snd_vol_type in snd_status_type.
                   Added VOC_xxxx_HZ definitions for tones that may now be
                   played through the car kit in addition to the ringer.
12/12/96    ro     Added (TG==T_T) cases to all #if's that have (TG==T_G)
                   for initial support of TGP build.
12/05/96    ro     Change SND_VOC_DTMF_UNITY_GAIN for T_I2 from 0x7fff
                   to 0x2000, by removing T_I2 from #if
09/19/96   fkm     Added SND_VOC_1P5DB_HOT/COLD Definitions
05/29/96   fkm     Added Defintions for SND_CODEC_3DB
04/18/96   fkm     Added SND_CODEC_15DB Definition
03/19/96   fkm     Updates Per Code Review
02/22/96   fkm     Changed index_a/index_b to index_hi/index_lo to help
                   note that High/Low tones belong in certain sides of the
                   DTMF generators on some targets.
02/16/96   fkm     Tweeks to comments and documentation
02/06/96   fkm     Put ()s around Macros to Make Lint Happy
01/17/96   fkm     Added T_C1 Stuff
01/03/96   fkm     Lint Nits
12/21/95   fkm     Removed (TG<T_G)s and added SND_DEBUG_MSG()
12/13/95   fkm     ISS2/India Vocoder and Ringer Values
11/30/95   fkm     Added New Gemini Ringer Values
11/10/95   fkm     Removed RNG_M_START bit from Ringer Table Values
11/02/95   fkm     Added Gemini Vocoder and Codec Gain Definitions
10/11/95   fkm     Added SND_ALLOW_SLEEPING.
07/21/95   fkm     Minor Documentation Updates
07/06/95   fkm     Factory Patch is no longer Automagically Enabled for Gemini
06/19/95   fkm     Factory Patch: Force Car Kit Enable from DM "poke"
06/14/95   fkm     SND_MAX_PERIOD now 25 msec in FM mode (covers for MSM bug).
06/08/95   fkm     Added TIM2 Definitions for Gemini Ringer Support
05/16/95   fkm     Removed "Twist" from Gemini for now and some changes in
                   preparation for ISS2.
04/24/95   fkm     Changes needed for Gemini
08/15/94   jah     Set Vocoder DTMF Unity gain for ISS-1
03/13/94   jah     Changed #if T_SLEEP!=0 to #ifdef T_SLEEP
12/09/93   jah     Made sleep macros conditional on T_SLEEP
04/26/93   jah     Added changes for Beta II mobile.
04/14/93   jah     Changed aud_image[] to snd_audi[]
04/09/93   jah     Added SND_SLEEP_ALLOW/FORBID() macros for CDMA sleep.
02/09/93   jah     Added added DTMF path change to the sound status structure.
01/08/93   jah     Additions for no-qwacking switch between voice and tones.
01/05/93   jah     Changed SND_MAX_PERIOD to account for SND_VOC_GRAIN, which
                   allows Vocoder tones to continue seemlessly.
12/22/92   jah     Added Portable speaker sense/select.
12/09/92   jah     Removed REAL_CLOCK pre-Stretch-1 support.
10/16/92   jah     Added extern's for regional data/routines.  Added the tone
                   generator type for the ringer.
09/15/92   jah     Added SONY_1271_GPIO and SONY_1271_GPIO_BUF.
                   Added stub horn alert for non-mobile environments.
09/03/92   jah     Moved horn alert to proper gpio, fixed SND_NEW_TONE.
09/02/92   jah     Added horn alert macros.
09/01/92   jah     Added VOX_MODE() macro.
08/25/92   jah     Ordered tones to simplify snd_change.
08/20/92   jah     Added SND_VOC_POP_WAIT for waiting for vocoder pop.
08/14/92   jah     Added Vocoder values for keypad DTMFs
07/30/92   jah     A few BIO symbol name corrections for the stretch.
07/27/92   jah     Converted to BIO interface, added changes from code review
04/20/92   jah     Created

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc. */
#include "target.h"     /* Target specific definitions      */
#ifndef FEATURE_CMI

#include "clk.h"        /* Definitions for CLK_MS_PER_TICK  */
#endif
#include "nv.h"         /* NV Defintions                    */
#ifdef SND_DEBUG
#include "msg.h"        /* Make sure MSG_LEVEL is defined   */
#endif
#include "sdb.h"        /* Definitions of sounds id         */
#include "snd.h"        /* Definitions of filters           */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#include "sleep.h"      /* Sleep signal definitions         */
#include "task.h"       /* Sleep TCB declaration            */
#endif
#ifdef FEATURE_AUDIO_FORMAT
#include "audmain.h"    /* Audio Format definitions         */
#include "qw.h"         /* Quadword typedefs and prototypes */
#endif
#if defined(FEATURE_VOICE_PLAYBACK) || defined(FEATURE_ARM_POST_PROCESSING)
#include "voc.h"        /* Vocoder driver definitions       */
#endif /* FEATURE_VOICE_PLAYBACK */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbcenc.h" 
#endif


#if (TG!=T_REX)
#include "bio.h"        /* Bit I/O macros                   */
#endif

/* <EJECT> */
/*===========================================================================

              SOUND TASK REX SIGNAL DEFINITIONS

===========================================================================*/

/* Reserved (unused) signals.  Note the Upper Nibble is reserved for
** system wide "common signals".  (See task.h.)
*/

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#ifndef FEATURE_AVS_SBCENC_TASK 
#define SND_SBC_CODEC_TIMER_SIG 0x0010
#else
#define SND_RSVD_0010_SIG       0x0010
#endif
#endif

#ifdef FEATURE_AUDIO_FORMAT
/*To handle the case when snd's module is not available on current image*/
#define SND_VOC_MODULE_STATUS   0x2000

/* Signal is used to indicate that a requested data buffer has been received.
** This is only used in the case where synchronous (blocking) reads are 
** required.
*/
#define SND_AUDIO_FORMAT_SIG    0x0080
#else
#define SND_RSVD_0080_SIG       0x0080
#endif

#ifdef FEATURE_ANIMATED_AAC
#define SND_AAC_RETRY_SIG 0x0400
#else
#define SND_RSVD_0400_SIG       0x0400
#endif

#ifdef SND_EXTCOM
#error code not present
#else
#define SND_RSVD_0800_SIG       0x0800
#endif /* SND_EXTCOM */

/* This signal is used to create a delay for turning on an audio path
*/
#define SND_PATH_TIMER_SIG      0x0200

/* This signal is used by Searcher to tell Sound the Phone is fully awake.
*/
#define SND_PHONE_AWAKE_SIG     0x0100

/* This signal is used to keep the vocoder on for a period of time
** after a sound is played.
*/
#define SND_OVERRIDE_TIMER_SIG  0x0040

/* This signal is set when the Vocoder changes on/off state
*/
#define SND_VOC_CHNG_SIG        0x0020

/* This signal is set when it's time to stop a tone
*/
#define SND_TONE_TIMER_SIG      0x1000

/* This signal is set when the audio can be unmuted after an audio path change
*/
#define SND_UNMUTE_SIG          0x0004

/* This signal is set when an item is put on the snd_cmd_q
*/
#define SND_CMD_Q_SIG           0x0002

/* This signal is set when the diag_rpt_timer expires
*/
#define SND_RPT_TIMER_SIG       0x0001

/* <EJECT> */
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/* Masks of things which get changed when we process the queue.  Masks are
** used so that we can clear out the queue before we go about any lengthy
** hardware manipulations.  This provides timely processing of the most
** recent commands at the expense of a static sequence of processing changes.
*/
#define SND_NO_CHANGE_M 0
#define SND_AUD_IN_M    0x0001
#define SND_AUD_OUT_M   (SND_AUD_IN_M << 1)
#define SND_AUD_DTMF_M  (SND_AUD_IN_M << 2)
#define SND_VOLUME_M    (SND_AUD_IN_M << 3)
#define SND_CHANGE_M    (SND_AUD_IN_M << 4)
#define SND_RING_VOL_M  (SND_AUD_IN_M << 5)
#ifdef FEATURE_VOICE_PLAYBACK
#define SND_PLAYBACK_M  (SND_AUD_IN_M << 6)
#define SND_SND_LO_M    (SND_AUD_IN_M << 7)
#define SND_SND_MED_M   (SND_AUD_IN_M << 8)
#define SND_SND_HI_M    (SND_AUD_IN_M << 9)
#define SND_TONE_M      (SND_AUD_IN_M << 10)
#define SND_VP_SIGNAL   (SND_AUD_IN_M << 11)
#else
#define SND_SND_LO_M    (SND_AUD_IN_M << 6)
#define SND_SND_MED_M   (SND_AUD_IN_M << 7)
#define SND_SND_HI_M    (SND_AUD_IN_M << 8)
#define SND_TONE_M      (SND_AUD_IN_M << 9)
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */

/* If this is entered as the volume, then the database volume setting will
** be used.
*/
#ifdef FEATURE_VERSION_C337
#define SND_VOL_USE_DEFAULT 0x01
#else
#define SND_VOL_USE_DEFAULT 0xff
#endif

/* Mask of all-mute masks
*/
#define SND_SND_MUTE  ( (snd_mute_type)         \
                        ( SND_MUTE_SND          \
                        | SND_MUTE_SNDV         \
                        | SND_MUTE_SNDPATH ) )

#define SND_ALL_MUTE  ( (snd_mute_type)         \
                        ( SND_MUTE_ACPFM        \
                        | SND_MUTE_ACPRXTX      \
                        | SND_MUTE_SND          \
                        | SND_MUTE_SNDV         \
                        | SND_MUTE_DIAG         \
                        | SND_MUTE_SNDPATH ) )

/*-------------------------------------------------------------------------
** Sound Commands
** 
**-----------------------------------------------------------------------*/
typedef enum {          /* (parameter) Brief description                   */

  SND_SET_DEVICE,       /* (path) Set the audio path, and muting           */
  SND_SET_VOLUME,       /* (volume) Set the volume for the [device, method]*/
  SND_GET_VOLUME,       /* (volume) Get the volume for the [device, method]*/
  SND_GET_LEVELS,       /* (volume) Set the maximum volume                 */
  SND_TONE_START,       /* (tone) Play a single tone for a while           */
  SND_TONE_STOP,        /* (no parameter) Stop playing the current tone    */
  SND_SOUND_ID_START,   /* (sound) Play a predefined sound                 */
  SND_SOUND_START,      /* (sound) Play a sound from a list of tones       */
  SND_SOUND_STOP,       /* (no parameter) Stop the current sound           */

#ifdef FEATURE_VOICE_PLAYBACK
  SND_VOICE_PROMPT_PLAY,/* (*voc_pb_header_type) Play a voice prompt       */
  SND_VOICE_PROMPT_STOP,/* (no parameter) Stop all voice prompts           */
  SND_VOICE_PROMPT_SIG, /* (vp_sig) Signal from vocoder for VP playback    */
#endif

#ifdef FEATURE_VOICE_MEMO
  SND_VOICE_PAUSE_RESUME,/* (vp) pause the current playback                */
#endif

  SND_FILTERS,          /* (filters) Set VOX etc based on filter type      */

  SND_EC_MODE_CTL,      /* Set echo canceller mode                         */
  SND_NS_MODE_CTL,      /* Set noise suppression mode                      */
  SND_VOX_MODE_CTL,     /* Set VOX mode                                    */
  SND_SIDETONE_MODE_CTL, /* Set sidetone mode */

#ifdef FEATURE_AUDIO_AGC
  SND_AVC_CTL,          /* (avc_ctl) Automatic Volume Control              */
  SND_AGC_CTL,          /* (agc_ctl) Automatic Gain Control                */
#endif

  SND_SMARTSND_CTL,	    /* (smartsnd_ctl) Controls AGC/AVC/RVE         */

  SND_MUTE_CAR_STEREO,  /* (stereo) Mute Car Stereo (if Car Kit)           */

#ifdef SND_EXTCOM
#error code not present
#endif

  SND_SOUND_GET_TIME,   /* Get the playback time of a sound                */
  SND_SOUND_GET_SPEC,   /* Get the audio format type of a sound data       */

#ifdef FEATURE_AUDIO_FORMAT
  /* The following four commands are for all sounds but currently only the
  ** audio formats sounds are supported.
  */
  SND_SOUND_FLUSH,      /* Flush buffers and restart playing               */
  SND_SOUND_REWIND,     /* Rewind playback of a sound                      */
  SND_SOUND_FFORWARD,   /* Fast-forward playback of a sound                */
  SND_SOUND_PAUSE,      /* Pause playback of a sound                       */
  SND_SOUND_SEEK_PAUSE, /* Prepare to receive rewind/FF command            */
  SND_SOUND_RESUME,     /* Resume playback of a sound                      */
  SND_SOUND_TEMPO,      /* Modify playback tempo of a sound                */
  SND_SOUND_TUNE,       /* Modify playback tune of a sound                 */
  SND_SOUND_PAN,        /* Modify playback stereo pan of a sound           */

  SND_AUDFMT_SEQ_STOP,        /* Stop indicated sequence command           */
  SND_AUDFMT_SEQ_PAUSE,       /* Pause indicated sequence command          */
  SND_AUDFMT_SEQ_RESUME,      /* Resume indicated sequence command         */
  SND_AUDFMT_SEQ_FFORWARD,    /* Fast-forward indicated sequence command   */
  SND_AUDFMT_SEQ_REWIND,      /* Rewind indicated sequence command         */
  SND_AUDFMT_SEQ_SET_TEMPO,   /* Set tempo of indicated sequence command   */
  SND_AUDFMT_SEQ_SET_TUNE,    /* Set tune of indicated sequence command    */
  SND_AUDFMT_SEQ_SET_PAN,     /* Set pan of indicated sequence command     */
  SND_AUDFMT_SEQ_SET_VOL,     /* Set vol of indicated sequence command     */
#ifdef FEATURE_AUDFMT_HYBRID_MODE
  SND_MULTI_CMD_SET_POLY_MODE, /* Set poly mode command                    */
  SND_MULTI_CMD_GET_POLY_MODE, /* Get poly mode command                    */
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
  SND_AUDFMT_GET_TIME_ABORT,  /* Abort get time of audfmt sound            */
  SND_AUDFMT_READ_CALLBACK,   /* Get data callback command                 */

  SND_AUDFMT_HEADER_CALLBACK, /* Get header data callback command          */
  SND_AUDFMT_DLS_CALLBACK,    /* Get DLS callback command                  */
  SND_AUDFMT_CHECK_RESET,     /* Check reset command                       */

  SND_AUDFMT_PREPARSE_DONE,   /* Preparse finished command                 */
  SND_AUDFMT_AV_SYNC,         /* AV sync update from DSP                   */

#ifdef FEATURE_DLS
  SND_AUDFMT_DLS_LOAD,          /* Load DLS file to be used by all seq     */
  SND_AUDFMT_DLS_UNLOAD,        /* Unload all DLS resources                */
  SND_AUDFMT_DLS_READ_CALLBACK, /* DLS read callback command               */
#endif /* FEATURE_DLS */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
  SND_AUDFMT_ADEC_INT,  /* DSP requesting more data                        */
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
#ifdef FEATURE_QCP
  SND_QCP_GET_FUNC,     /* QCP get command                                 */
  SND_QCP_SET_FUNC,     /* QCP set command                                 */
  SND_QCP_RECORD,       /* Start QCP recording command                     */
  SND_QCP_REC_VOC_CB,   /* QCP record callback from vocoder driver         */
  SND_QCP_RECORD_BUF,   /* Received QCP recording buffer                   */
  SND_QCP_RECORD_STOP,  /* QCP record stop command                         */
  SND_QCP_RECORD_PAUSE, /* QCP record pause command                        */
  SND_QCP_RECORD_RESUME,/* QCP record resume command                       */
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
  SND_PCM_RECORD,       /* Start PCM recording command                     */
  SND_PCM_RECORD_BUF,   /* Received PCM recording buffer                   */
  SND_PCM_RECORD_STATUS,/* PCM recording status update                     */
  SND_PCM_RECORD_STOP,  /* PCM record stop command                         */
  SND_PCM_RECORD_PAUSE, /* PCM record pause command                        */
  SND_PCM_RECORD_RESUME,/* PCM record resume command                       */
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
  SND_MM_RECORD,        /* Start MultiMedia recording command              */
  SND_MM_RECORD_BUF,    /* Received MultiMedia recording buffer            */
  SND_MM_RECORD_STATUS, /* MultiMedia recording status update              */
  SND_MM_RECORD_STOP,   /* MulitMedia record stop command                  */
  SND_MM_RECORD_PAUSE,  /* MultiMedia record pause command                 */
  SND_MM_RECORD_RESUME, /* MultiMedia record resume command                */
  SND_MM_RECORD_ISR,    /* MultiMedia record interrupt from QDSP           */
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AUDFMT_EQUALIZER
  SND_EQUALIZER_CTL,          /* Eqaulizer control command                 */
  SND_EQUALIZER_SET_FUNC,     /* Equalizer set filter command              */
  SND_EQUALIZER_GAIN,         /* Equalizer Gain control command            */
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
  SND_SPECTRUM_ANALYZER_CTL,  /* Spectrum analyzer control command         */
  SND_SPECTRUM_ANALYZER_DATA, /* Spectrum analyzer data available          */
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
  
#ifdef FEATURE_AUDFMT_QCONCERT
  SND_QCONCERT_CTL,           /* QConcert control command                  */
  SND_QCONCERT_PARAMS,        /* QConcert configuration parameters command */
#endif /* FEATURE_AUDFMT_QCONCERT */
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_MIDI_OUT
  SND_MIDI_OUT_OPEN,    /* Enable MIDI output device                       */
  SND_MIDI_OUT_CLOSE,   /* Disable MIDI output device                      */
  SND_MIDI_OUT_MSG,     /* Issue commands to MIDI device                   */
#ifdef FEATURE_MIDI_OUT_QCP
  SND_MIDI_OUT_QCP,        /* Play QCP file to MIDI device                 */
  SND_MIDI_OUT_ADEC_START_CHAN, /* Start delayed playback of ADEC          */
  SND_MIDI_OUT_QCP_STOP,   /* Stop QCP file to MIDI device                 */
  SND_MIDI_OUT_QCP_PAUSE,  /* Pauses QCP files to MIDI device              */
  SND_MIDI_OUT_QCP_RESUME, /* Resumes QCP files to MIDI device             */
  SND_MIDI_OUT_QCP_PAUSE_CHAN,  /* Pauses QCP file to MIDI device          */
  SND_MIDI_OUT_QCP_RESUME_CHAN, /* Resumes QCP file to MIDI device         */
  SND_MIDI_OUT_QCP_PAN,    /* Modifies panning of QCP file to MIDI device  */
  SND_MIDI_OUT_QCP_VOL,    /* Modifies volume of QCP file to MIDI device   */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_QAUDIOFX
  SND_3D_POS_CMD_ENABLE,             /* 3D position enable command            */
  SND_3D_POS_CMD_RESET,
  SND_3D_POS_CMD_SET_LSNR_POS,       /* 3D position listener position command */
  SND_3D_POS_CMD_SET_LSNR_ORIENT,    /* 3D position listener orientation cmd  */
  SND_3D_POS_CMD_SET_LSNR_VEL,       /* 3D position listener velocity command */
  SND_3D_POS_CMD_SET_REVERB,         /* 3D position reverb enable command     */
  SND_3D_POS_CMD_REVERB_ENABLE,      /* 3D position reverb disable command    */
  SND_3D_POS_CMD_SET_SRC_VOL,        /* 3D position source volume command     */
  SND_3D_POS_CMD_SET_SRC_VOL_ATTEN,  /* 3D pos source volume attenuation cmd  */
  SND_3D_POS_CMD_SET_SRC_POS,        /* 3D position source position command   */
  SND_3D_POS_CMD_SET_SRC_VEL,        /* 3D position source position command   */
  SND_3D_POS_CMD_SRC_ENABLE,
  SND_3D_POS_CMD_SRC_RESET,
  SND_3D_POS_CMD_SET_REVERB_VOL,     /* 3D position source reverb volume cmd  */
  SND_3D_POS_CMD_QUERY_ENV,          /* 3D position query environment command */
  SND_3D_POS_CMD_QUERY_SRC,          /* 3D position query source command      */
#endif /* FEATURE_QAUDIOFX */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  SND_SBCCODEC_CTL,      /* Set BT SBC codec callback to receive PCM from DSP */
  SND_SBCCODEC_DATA,       /* DSP is sending PCM data to SBC codec            */
#ifndef FEATURE_AVS_SBCENC_TASK
  SND_SBCCODEC_BITRATE,    /* Set SBC bitrate                                 */
#endif
  SND_SBCCODEC_INIT,
  SND_SBCCODEC_PARAMS,
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AVS_I2SSBC
  SND_I2SSBC_ENABLE,
  SND_I2SSBC_DISABLE,
#endif /* FEATURE_AVS_I2SSBC */

#if defined (FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) 
  SND_PP_CTL,         /* Enable/Disable ARM Post processing */
  SND_PP_DSP_DATA,    /* DSP is sending data to the client */
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE */

  /* enable and disable external AuxPCM clock */
  SND_AUX_PCM_CLK_ENABLE, 
  SND_AUX_PCM_CLK_DISABLE,

#ifdef FEATURE_AUDIO_PCR_SUPPORT
  SND_AUDFMT_ADJUST_SAMPLE, /* Add/Drop a audio sample */
#endif /* FEATURE_AUDIO_PCR_SUPPORT */
  SND_OP_DUAL_MONO_MODE,    /* Dual Mono Channel Mode cmd configmration       */
  SND_OP_SBR_MODE,          /* SBR command confirmation                       */
  SND_OP_CODEC_INFO_UPDATE, /* Audio codec info feedback                      */
  SND_SET_AUX_LINE_IN,      /* Enable or Disable Line In                      */

  SND_SLOWTALK_CTL,         /* Slowtalk control command                  */
  SND_SLOWTALK_PARAMS,      /* Slowtalk configuration parameters command */
  SND_AUDFMT_PA_ON_CB,     /* Notify the SND,when PAs are on */

  SND_INVALID           /* (Leave Last) Invalid Command - Causes Error     */
} snd_op_type;

/* Header for commands
**      If the callback pointer is non-null, the function will be called,
**      with the client data as a parameter, when the command is finished.
*/
typedef struct {
  q_link_type          link;           /* Command queue link           */
  snd_op_type          cmd;            /* Sound command to perform     */
  snd_cb_func_ptr_type callback_ptr;   /* Callback function            */
  const void           *client_data;   /* Pointer to Client data       */
} snd_hdr_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* AuxPGA Line In path setting packet
*/
typedef struct {
  snd_hdr_type                hdr;           /* Command Header        */
  snd_aux_line_in_mode_type   aux_mode;      /* aux line in mode      */
} snd_aux_line_in_packet_type;

/* Audio path setting packet
*/
typedef struct {
  snd_hdr_type           hdr;           /* Command Header        */
  snd_device_type        device;        /* Device                */
  snd_mute_control_type  out_mute_s;    /* Is output path muted? */
  snd_mute_control_type  in_mute_s;     /* Is input path muted?  */
} snd_device_packet_type;

/* Volume level control packet
*/
typedef struct {
  snd_hdr_type        hdr;              /* Command Header             */
  snd_device_type     device;           /* Device                     */
  snd_method_type     method;           /* Which generator to use     */
  uint8               volume;           /* Volume level               */
} snd_vol_packet_type;

/* Get Volume level control packet
*/
typedef struct {
  snd_hdr_type             hdr;              /* Command Header             */
  snd_device_type          device;           /* Device                     */
  snd_method_type          method;           /* Which generator to use     */
  snd_vol_cb_func_ptr_type callback_ptr;     /* Callback function          */
} snd_get_vol_packet_type;
 
/* Maximum Volume level control packet
*/
typedef struct {
  snd_hdr_type             hdr;              /* Command Header             */
  snd_device_type          device;           /* Device                     */
  snd_method_type          method;           /* Which generator to use     */
  snd_lev_cb_func_ptr_type callback_ptr;     /* Callback function          */
} snd_level_packet_type;

/* Single tone packet
*/
typedef struct {
  snd_hdr_type        hdr;              /* Command Header              */
  snd_device_type     device;           /* Device                      */
  snd_method_type     method;           /* Which generator to use      */
  snd_tone_type       tone_id;          /* Single tone identification  */
  uint16              freq_hi;          /* Higher freq                 */
  uint16              freq_lo;          /* Lower freq                  */
  uint16              duration_ms;      /* Duration of tone            */
  snd_apath_type      dtmf_path;        /* DTMF tx/local/mute pathing  */
  uint8               volume;           /* Volume level                */
} snd_tone_packet_type;

/* Sound from database packet
*/
typedef struct {
  snd_hdr_type        hdr;              /* Command Header              */
  snd_device_type     device;           /* Device                      */
  snd_method_type     method;           /* Which generator to use      */
  snd_sound_id_type   sound_id;         /* Sound id                    */
  snd_priority_type   priority;         /* Priority of the sound       */
  snd_apath_type      dtmf_path;        /* DTMF tx/local/mute pathing  */
  uint8               volume;           /* Volume level                */
  uint16              repeat_cnt;       /* Repeat counter              */
} snd_sound_id_packet_type;

/* Sound from list of tones packet
*/
typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  snd_device_type      device;           /* Device                      */
  snd_method_type      method;           /* Which generator to use      */
  const snd_sound_type *sound_ptr;       /* Multi-tone                  */
  snd_priority_type    priority;         /* priority                    */
  snd_apath_type       dtmf_path;        /* DTMF tx/local/mute pathing  */
  uint8                volume;           /* Volume level                */
  uint16               repeat_cnt;       /* Repeat counter              */
#ifdef FEATURE_VOICE_PLAYBACK
  voc_pb_header_type   **vp;             /* List of pointers to voice   */
                                         /* prompts used in sound       */
#endif
} snd_sound_packet_type;

/* Stop Sound
*/
typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  snd_priority_type    priority;         /* Priority                    */
} snd_sound_stop_type;

#ifdef FEATURE_VOICE_PLAYBACK
/* Single voice prompt packet
*/
typedef struct {
  snd_hdr_type        hdr;              /* Command Header              */
  snd_device_type     device;           /* Device                      */
  voc_pb_header_type  *vp;              /* Pointer to VP to play       */
} snd_vp_packet_type;

/* Vocoder signal with command for VP playback
*/
typedef struct {
  snd_hdr_type                hdr;          /* Command Header               */
  voc_end_vpentry_return_type vp_status;    /* Status of VP playback        */
  const void                  *client_data; /* Client data from VP playback */
} snd_vp_sig_packet_type;
#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_VOICE_MEMO
typedef enum {
  SND_VP_PLAY_PAUSE,              /* Pause playing of voice memo             */
  SND_VP_PLAY_RESUME,             /* Resume playing of voice memo            */
  SND_VP_INVALID
} snd_vp_play_record_type;

typedef struct {
  snd_hdr_type              hdr;         /* Command Header             */
  snd_vp_play_record_type   play_record; /* Type of VM action to take  */
} snd_vp_action_type;
#endif

typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  snd_filters_type     filters;          /* Type of Filtering Wanted    */
} snd_filters_packet_type;

/* EC/NS mode control packet
*/
typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  snd_ec_ns_mode_type  mode;             /* EC/NS mode                  */
} snd_ec_ns_mode_packet_type;

/* VOX mode control packet
*/
typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  snd_vox_mode_type    mode;             /* VOX mode                    */
} snd_vox_mode_packet_type;

/* sidetone mode control packet
*/
typedef struct {
  snd_hdr_type              hdr;         /* Command Header             */
  snd_sidetone_mode_type    mode;        /* Sidetone mode              */
} snd_sidetone_mode_packet_type;

#ifdef FEATURE_AUDIO_AGC
typedef struct {
  snd_hdr_type         hdr;              /* Command Header              */
  boolean              on;               /* AGC/AVC on/off              */
} snd_agc_avc_packet_type;
#endif /* FEATURE_AUDIO_AGC */

typedef struct {
  snd_hdr_type              hdr;	 /* Command Header		*/
  snd_smartsnd_mode_type    smartsnd;    /* AGC/RVE/AVC on/off          */
}snd_smartsnd_packet_type;


/* Car Stereo Mute Control Packet
*/
typedef struct {
  snd_hdr_type   hdr;                   /* Command Header              */
  boolean        mute_stereo;           /* True=Mute Stereo            */
} snd_stereo_packet_type;

#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */

/* Sound get time command packet
*/
typedef struct {
  snd_hdr_type              hdr;         /* Command Header              */
  snd_sound_type            *sound_ptr;  /* Sound to get time           */
  snd_time_cb_func_ptr_type cb_func;     /* Callback function pointer   */
} snd_sound_get_time_packet_type;

/* Sound get audio specification command packet
*/
typedef struct {
  snd_hdr_type              hdr;         /* Command Header              */
  snd_sound_type            *sound_ptr;  /* Sound to get time           */
  snd_spec_cb_func_ptr_type cb_func;     /* Callback function pointer   */
} snd_sound_get_spec_packet_type;

#ifdef FEATURE_AUDIO_FORMAT
/* Sound rewind, fast-forward, pause and resume command packet
*/
typedef struct {
  snd_hdr_type              hdr;         /* Command Header                */
  snd_priority_type         priority;    /* Priority of sound to act upon */
  uint32                    time_ms;     /* Time in ms for command        */
} snd_sound_flow_cmd_packet_type;

/* Comamnd packet for sequence commands
*/
typedef struct {
  snd_hdr_type hdr;                /* Command Header                 */
  void         *sequence;          /* Sequence handle                */
  uint32       num;                /* Number associated with command */
} snd_sequence_cmd_packet_type;

/* Preparse done command packet
*/
typedef struct {
  snd_hdr_type              hdr;         /* Command Header              */
  void                      *client_data;/* Pointer to client_data      */
} snd_cmx_af_preparse_done_packet_type;
  
/* Data read callback command packet
*/
typedef struct {
  snd_hdr_type hdr;          /* Command Header              */
  uint8        **data;       /* Pointer to set in read      */
  uint8        *buf;         /* Buffer returned             */
  uint32       length;       /* Length of read request      */
  uint32       pos;          /* Position of read request    */
  void         *client_data; /* Client data for request     */
} snd_cmx_af_read_cb_packet_type;

/* Get DLS waveform callback command packet
*/
typedef struct {
  snd_hdr_type hdr;          /* Command Header                           */
  uint8        *data;        /* Buffer containing DLS waveform data      */
  uint8        *buf;         /* Buffer to be used to decode DLS waveform */
  uint32       length;       /* Length of DLS waveform requested         */
  void         *client_data; /* Client data for DLS request              */
} snd_cmx_af_dls_cb_packet_type;

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* Forward definition of voc_adec_cb_func_type */
typedef voc_adec_cb_func_type voc_adec_cb_func_struct;
/* Forward definition of voc_codec_update_cb_func_type */
typedef voc_codec_update_cb_func_type voc_codec_update_cb_func_struct;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */


#ifdef FEATURE_QCP
/* QCP get function command
*/
typedef struct {
  snd_hdr_type           hdr;         /* Command Header              */
  snd_qcp_func_enum_type func;        /* Function to perform         */
  snd_af_access_type     *qcp_handle; /* Handle of QCP file          */
} snd_qcp_get_func_packet_type;

/* QCP set function command
*/
typedef struct {
  snd_hdr_type                 hdr;          /* Command Header              */
  snd_qcp_func_enum_type       func;         /* Function to perform         */
  uint8                        *buffer;      /* Buffer with data            */
  uint32                       num_bytes;    /* Number of bytes in buffer   */
  snd_af_access_type           *handle;      /* Handle of QCP file          */
  snd_qcp_rec_cb_func_ptr_type cb_func;      /* Callback fucntion           */
  const void                   *client_data; /* Client data                 */
} snd_qcp_set_func_packet_type;

/* QCP record command packets.
*/
typedef struct {
  snd_hdr_type                 hdr;         /* Command Header              */
  snd_qcp_format_type          format;      /* Format to use for QCP rec   */
  snd_qcp_dir_type             link;        /* Path to record QCP data     */
  snd_mm_qcp_rec_para_type     rec_qcp_param;/* MM record qcp parameters   */
  snd_qcp_rec_cb_func_ptr_type cb_func;     /* Callback function           */
  const void                   *client_data;/* Client data                 */
} snd_qcp_rec_packet_type;

/* QCP record callback from vocoder command packet.
*/
typedef struct {
  snd_hdr_type        hdr;           /* Command Header              */
  voc_rec_status_type status;        /* Status of vocoder callback  */
  uint8               *tx_buffer;    /* Tx data buffer              */
  uint32              tx_num_bytes;  /* Number of bytes of Tx data  */
  uint32              tx_num_frames; /* Number of frames of Tx data */
  uint8               *rx_buffer;    /* Rx data buffer              */
  uint32              rx_num_bytes;  /* Number of bytes of Rx data  */
  uint32              rx_num_frames; /* Number of frames of Rx data */
} snd_qcp_rec_cb_packet_type;

/* QCP recording buffer command packet.
*/
typedef struct {
  snd_hdr_type  hdr;        /* Command Header              */
  uint8         *buffer;    /* Pointer to recording buffer */
  uint32        num_bytes;  /* Buffer size in bytes        */
} snd_qcp_rec_buf_packet_type;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
/* PCM record command packets.
*/
typedef struct {
  snd_hdr_type                 hdr;         /* Command Header              */
  snd_pcm_rec_dir_type         link;        /* Path to record PCM data     */
  uint8                        *buf1;       /* Buffer 1 for recorded data  */
  uint32                       num_buf1;    /* Size of buf1, in bytes      */
  uint8                        *buf2;       /* Buffer 2 for recorded data  */
  uint32                       num_buf2;    /* Size of buf2, in bytes      */
  snd_pcm_rec_cb_func_ptr_type cb_func;     /* Callback function           */
  const void                   *client_data;/* Client data                 */
} snd_pcm_rec_packet_type;

/* PCM recording buffer command packet.
*/
typedef struct {
  snd_hdr_type  hdr;          /* Command Header              */
  uint8         *buffer;      /* Pointer to recording buffer */
  uint32        num_bytes;    /* Buffer size in bytes        */
  const void    *client_data; /* Client data                 */
} snd_pcm_rec_buf_packet_type;

/* PCM recording command packet.
*/
typedef struct {
  snd_hdr_type         hdr;         /* Command Header        */
  snd_pcm_rec_dir_type link;        /* Path to apply command */
} snd_pcm_rec_cmd_packet_type;

/* PCM recording status packet.
*/
typedef struct {
  snd_hdr_type                 hdr;         /* Command Header          */
  snd_af_data_cmd_enum         status;      /* Status of PCM recording */
  snd_pcm_rec_cb_func_ptr_type cb_func;     /* Client CB func          */
  const void                   *client_data;/* Client data ptr         */
  snd_pcm_rec_data_struct_type data;        /* PCM data structure      */
} snd_pcm_rec_status_packet_type;
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/* 
 * MultiMedia record command packets.
 */
typedef struct {
  snd_hdr_type                hdr;          /* Command Header              */
  snd_mm_rec_dir_type         dir;          /* MultiMedia recording direction */
  snd_mm_rec_aud_type         rec_param;    /* Record parameters              */
  snd_mm_rec_cmd_struct_type  buf;          /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func;      /* Callback function to CMX       */
  const void                  *client_data; /* Client data pointer            */
} snd_mm_rec_packet_type;

/* 
 * MultiMedia recording buffer command packet.
 */
typedef struct {
  snd_hdr_type           hdr;               /* Command Header                 */
  uint8                  *buffer;           /* Pointer to recording buffer    */
  uint32                 num_bytes;         /* Buffer size in bytes           */
  const void             *client_data;      /* Client data                    */
  snd_mm_rec_format_type rec_format;
} snd_mm_rec_buf_packet_type;

/* 
 * MultiMeida recording command packet.
 */
typedef struct {
  snd_hdr_type           hdr;               /* Command Header                 */
  snd_mm_rec_dir_type    link;              /* Record direction               */
} snd_mm_rec_cmd_packet_type;

/* 
 * MultiMedia recording status packet.
 */
typedef struct {
  snd_hdr_type                 hdr;          /* Command Header             */
  snd_af_data_cmd_enum         status;       /* Status of MM recording     */
  snd_mm_rec_cb_func_ptr_type  cb_func;      /* Client CB func             */
  const void                   *client_data; /* Client data ptr            */
  snd_mm_rec_data_struct_type  data;         /* MM data structure          */
  snd_mm_rec_format_type       format;       /* Format to be recorded      */
} snd_mm_rec_status_packet_type;

/*
 * Structure for MultiMedia interrupt to post processing command to sound
 * driver
 */
typedef struct {
  snd_hdr_type  hdr;                          /* Command Header            */
} snd_mm_rec_isr_type;
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AUDFMT_EQUALIZER
/* Enable/disable eqaulizer control command packet
*/
typedef struct { 
  snd_hdr_type         hdr;            /* Command Header               */
  boolean              enable;         /* Enable/Disable equalizer     */
  snd_cb_func_ptr_type callback_ptr;   /* Callback function            */
  const void           *client_data;   /* Pointer to Client data       */
} snd_equalizer_ctl_packet_type;

/* Set eqaulizer control command packet
*/
typedef struct { 
  snd_hdr_type                    hdr;            /* Command Header         */
  const snd_equalizer_filter_type *filter;        /* Equalizer filter       */
  uint16                          num;            /* Number of filter band  */
  snd_cb_func_ptr_type            callback_ptr;   /* Callback function      */
  const void                      *client_data;   /* Pointer to Client data */
} snd_equalizer_set_packet_type;

/* Eqaulizer gain control command packet
*/
typedef struct {
  snd_hdr_type         hdr;          /* Command Header               */
  uint16               band;         /* Band number                  */
  int16                gain;         /* Gain of the band             */
  snd_cb_func_ptr_type callback_ptr; /* Callback function            */
  const void           *client_data; /* Pointer to Client data       */
} snd_equalizer_gain_packet_type;
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* Spectrum analyzer control command packet
*/
typedef struct {
  snd_hdr_type                  hdr;           /* Command Header         */
  uint16                        update_period; /* Data update period     */
  snd_spectrum_analyzer_cb_type cb_func;       /* Client CB for SA data  */
  snd_cb_func_ptr_type          callback_ptr;  /* Callback function      */
  const void                    *client_data;  /* Pointer to Client data */
} snd_spectrum_analyzer_ctl_packet_type;
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AUDFMT_QCONCERT
/* QConcert control command packet
*/
typedef struct {
  snd_hdr_type         hdr;           /* Command Header         */
  boolean              enable;        /* Enable/disable flag    */
  snd_cb_func_ptr_type callback_ptr;  /* Callback function      */
  const void           *client_data;  /* Pointer to Client data */
} snd_qconcert_ctl_packet_type;

/* QConcert configuration command packet
*/
typedef struct {
  snd_hdr_type hdr;                       /* Command Header           */
  snd_qconcert_params_type params;        /* Configuration parameters */
  snd_cb_func_ptr_type     callback_ptr;  /* Callback function        */
  const void               *client_data;  /* Pointer to Client data   */
} snd_qconcert_params_packet_type;
#endif /* FEATURE_AUDFMT_QCONCERT */

typedef struct {
  snd_hdr_type              hdr;            /* Command Header     */
  qword                     timestamp;      /* Timestamp in ms    */ 
  qword                     num_of_samples; /* PCM sample count       */
  qword                     num_of_bytes;   /* Frame byte count       */
} snd_av_sync_cmd_packet_type;

#endif /* FEATURE_AUDIO_FORMAT */

/* slowtalk control command packet
*/
typedef struct {
  snd_hdr_type          hdr;           /* Command Header         */
  boolean               enable;        /* Enable/disable flag    */
  snd_cb_func_ptr_type  callback_ptr;  /* Callback function      */
  const void            *client_data;  /* Pointer to Client data */
} snd_slowtalk_ctl_packet_type;

/* slowtalk configuration command packet
*/
typedef struct {
  snd_hdr_type             hdr;           /* Command Header           */
  snd_slowtalk_params_type params;        /* Configuration parameters */
  snd_cb_func_ptr_type     callback_ptr;  /* Callback function        */
  const void               *client_data;  /* Pointer to Client data   */
} snd_slowtalk_params_packet_type;

#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/* Post-proc control command packet
*/
typedef struct {
  snd_hdr_type   hdr;             /* Command Header */
  snd_pp_cb_type pp_cb_func;      /* Client callback for PP data */
}snd_pp_ctl_packet_type;

/* Post-proc data packet command
*/
typedef struct {
  snd_hdr_type    hdr;          /* Command Header */
  uint16          *buffer;      /* Pointer to the PP buffer from DSP */
  uint16          buf_size;     /* Size of the data in the buffer */
  uint16          pp_id;        /* The index of this buffer in the bufferpool */
  uint16          num_channels; /* Number of channels, 1 = Mono, 2 = Stereo   */
}snd_pp_ctl_resp_packet_type;

#endif /* FEATURE_ARM_POST_PROCESSING   || FEATURE_AVS_WB_PCM_RX_INTERFACE */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
typedef struct {
  snd_hdr_type          hdr;
  snd_sbc_bitrate_type  bitrate_hint;
  uint32                sbc_bitrate;
}snd_sbc_ctl_bitrate_packet_type;

typedef struct {
  snd_hdr_type hdr;
  snd_sbc_frame_cb_func_ptr_type sbc_cb;
}snd_sbc_ctl_packet_type;

typedef void (*snd_sbc_bitrate_ev_cb_func_type) (uint32 bitrate);

typedef struct {
  snd_hdr_type                hdr;           /* Command Header           */
  audiosbcenc_data_type       params;         /* encoded configuration parameters */
  snd_cb_func_ptr_type        callback_ptr;  /* Callback function        */
  const void                  *client_data;  /* Pointer to Client data   */
} snd_sbc_params_packet_type;
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AVS_I2SSBC
typedef struct {
  snd_hdr_type                hdr;           /* Command header                */
  snd_sample_rate_type        sample_rate;   /* Sample rate                   */
  snd_i2ssbc_cb_func_ptr_type i2ssbc_cb;     /* callback to get I2SSBC status */
}snd_i2ssbc_ctl_packet_type;
#endif /* FEATURE_AVS_I2SSBC */

/* Enumerated type for indexing into snd_array.  As an example,
** snd_array[SND_TONE] contains all information associated with the Tone
** currently being generated (if any).
*/
typedef enum {

#ifdef FEATURE_VOICE_PLAYBACK
  SND_PLAYBACK = 0,   /* Voice Prompt playback
                      */
  SND_SND_LO   = 1,   /* Reference to a series of dtmfs or tones which are
                      ** known to the sound task and which can be referred
                      ** to by a name from the command block.
                      */
  SND_SND_MED  = 2,   /* Multiple tones or dtmfs supplied in command block
                      */
  SND_SND_HI   = 3,   /* Multiple tones or dtmfs supplied in command block
                      */
  SND_TONE     = 4,   /* Single tone or dtmf
                      */
#else

  SND_SND_LO  = 0,    /* Reference to a series of dtmfs or tones which are
                      ** known to the sound task and which can be referred
                      ** to by a name from the command block.
                      */
  SND_SND_MED = 1,    /* Multiple tones or dtmfs supplied in command block
                      */
  SND_SND_HI  = 2,    /* Multiple tones or dtmfs supplied in command block
                      */
  SND_TONE    = 3,    /* Single tone or dtmf
                      */
#endif /* FEATURE_VOICE_PLAYBACK */
  SND_ARRAY_SIZE      /* Each variable in this enumeration is used to index
                      ** into the snd_array so this constant can be used to
                      ** size the snd_array.
                      */
} snd_array_idx_type;

/* Sound age function pointer type.
*/
typedef void (* snd_age_func_ptr_type)
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /* Mask of Changes found by snd_change() */
);

#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */

/* Type to describe a sound's status
*/
typedef struct {

  boolean        active;           /* Whether sound is in process -or- not   */
  boolean        hold;             /* Whether we are waiting for the voc.    */
  const snd_sound_type 
                 *sound_ptr;       /* Pointer to array of tones to play      */
  uint32         time_remain_ms;   /* Count of ms before tone is timed out   */
  word           tone_index;       /* Tone_ptr array index of current tone   */
  snd_age_func_ptr_type
                 age_func_ptr;     /* Pointer to aging function to use       */
  snd_cb_func_ptr_type
                 callback_ptr;     /* Callback function                      */
  const void     *client_data;     /* Pointer to client data                 */
  snd_device_type device;          /* Temporary device                       */
  snd_method_type method;          /* Sound class (which generator)          */
  byte           vol;              /* Tone Volume                            */
  snd_apath_type dtmf_path;        /* Temporary DTMF tx/local/mute pathing   */
  word           loop_cnt;         /* Loop count for SND_LOOP_BACK2          */
  word           label_index;      /* Array index of label tone              */
  word           label_cnt;        /* Loop count for label                   */
  word           repeat_cnt;       /* Repeat count                           */
  word           scaling;          /* Scale level to be used                 */
#ifdef FEATURE_VOICE_PLAYBACK
  boolean        vp_playing;       /* Whether a voice prompt is playing      */
#endif
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
#ifdef FEATURE_AUDIO_FORMAT
  void                 *audfmt_ctl; /* Audio format sound controller         */
  audmain_cmd_buf_type *audfmt_buf; /* Audio format command buffer           */
  audmain_song_config_type *audfmt_song_cfg; /* decoder specific config info */

  snd_sample_rate_type sample_rate; /* Sample rate of current sound          */
#endif /* FEATURE_AUDIO_FORMAT */
  boolean        buffer_underrun_flag; /* Buffer underrun flag used by 
                                        * snd_age_qcp and snd_age_adec_stream*/
  boolean wave_resampler;						
					
} snd_sound_status_type;

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* Audio decoder interrupt command packet
*/
typedef struct {
  snd_hdr_type           hdr;             /* Command Header              */
  void                   *data_ptr;       /* Data ptr                    */
} snd_af_adec_int_packet_type;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#ifdef FEATURE_MIDI_OUT
/* Enum type indicating status of MIDI output device */
typedef enum {
  SND_MIDI_OUT_DISABLE,     /* Disable MIDI device */
  SND_MIDI_OUT_ENABLE,      /* Enable MIDI device  */
  SND_MIDI_OUT_INVALID      /* Invalid status      */
} snd_midi_out_type;

/* MIDI_OUT command packet
*/
typedef struct {
  snd_hdr_type           hdr;             /* Command Header              */
  uint8                  *msg_buffer;     /* Buffer of MIDI commands     */
  uint32                 length;          /* Length, in bytes of buffer  */
} snd_midi_out_packet_type;

/* MIDI_OUT_OPEN command packet
*/
typedef struct {
  snd_hdr_type                   hdr;            /* Command Header        */
  snd_qsynth_adec_mode_enum_type adec_mode;      /* QSynth ADec command   */
} snd_midi_out_open_packet_type;

#ifdef FEATURE_MIDI_OUT_QCP
/* MIDI_OUT_QCP command packet
*/
typedef struct {
  snd_hdr_type                 hdr;            /* Command Header              */
  uint32                       channel;        /* Channel Number              */
  snd_af_access_type           *audfmt_handle; /* Handle of QCP file to play  */
  snd_midi_out_adec_param_type param;          /* Playback parameters         */
} snd_midi_out_qcp_packet_type;
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_QAUDIOFX
/* 3D positional enable packet type */
typedef struct {
  snd_hdr_type               hdr;            /* Command Header            */
  snd_3d_pos_mode_enum_type  mode;           /* 3D positional enable mode */
} snd_3d_pos_enable_packet_type;

/* 3D positional position packet type */
typedef struct {
  snd_hdr_type                 hdr;            /* Command Header        */
  uint32                       channel;        /* Object channel        */
  snd_3d_pos_position_cmd_type position;       /* XYZ position          */
} snd_3d_pos_position_packet_type;

/* 3D positional listener orientation packet type */
typedef struct {
  snd_hdr_type                    hdr;        /* Command Header         */
  snd_3d_pos_lsnr_orient_cmd_type orient;     /* Listener orientation   */
} snd_3d_pos_lsnr_orient_packet_type;

/* 3D positional velocity packet type */
typedef struct {
  snd_hdr_type                 hdr;            /* Command Header        */
  uint32                       channel;        /* Object channel        */
  snd_3d_pos_velocity_cmd_type velocity;       /* XYZ position          */
} snd_3d_pos_velocity_packet_type;

/* 3D positional reverb enable packet type */
typedef struct {
  snd_hdr_type                 hdr;
  boolean                      reverb_enable;
} snd_3d_reverb_enable_packet_type;

/* 3D positional reverb packet type */
typedef struct {
  snd_hdr_type                  hdr;            /* Command Header        */
  uint32                        channel;        /* Object channel        */
  snd_3d_reverb_enum_type   reverb_type;    /* Reverb Parameter type */
  snd_3d_reverb_cmd_type  reverb_param;   /* Reverb Parameters     */
} snd_3d_reverb_packet_type;

typedef struct {
  snd_hdr_type               hdr;            /* Command Header            */
  uint32                     channel;        /* Object channel            */
  snd_3d_pos_mode_enum_type  mode;           /* 3D positional enable mode */
} snd_3d_pos_src_enable_packet_type;

typedef struct {
  snd_hdr_type               hdr;            /* Command Header            */
  uint32                     channel;        /* Object channel            */
} snd_3d_pos_src_reset_packet_type;

/* 3D positional source volume packet type */
typedef struct {
  snd_hdr_type                   hdr;            /* Command Header        */
  uint32                         channel;        /* Object channel        */
  snd_3d_pos_src_volume_cmd_type src_vol;        /* Source volume         */
} snd_3d_pos_src_vol_packet_type;

/* 3D positional source volume attenuation packet type */
typedef struct {
  snd_hdr_type                      hdr;         /* Command Header            */
  uint32                            channel;     /* Object channel            */
  snd_3d_pos_src_vol_atten_cmd_type vol_atten;   /* Source volume attenuation */
} snd_3d_pos_src_vol_atten_packet_type;

/* 3D positional reverb source volume packet type */
typedef struct {
  snd_hdr_type                   hdr;            /* Command Header        */
  uint32                         channel;        /* Object channel        */
  snd_3d_reverb_src_volume_cmd_type reverb_vol; /* Source Reverb volume */
} snd_3d_reverb_src_vol_packet_type;

/* 3D positional query packet type */
typedef struct {
  snd_hdr_type               hdr;            /* Command Header        */
  snd_3d_pos_param_enum_type param;          /* Parameter to query    */
  uint32                     channel;        /* Object channel        */
  snd_3d_pos_get_cb_ptr_type cb_func;        /* Callback function     */
} snd_3d_pos_query_packet_type;
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_DLS
/* DLS load/unload command packet
*/
typedef struct {
  snd_hdr_type           hdr;             /* Command Header              */
  snd_af_access_type     *audfmt_handle;  /* Handle of DLS file to load  */
  uint32                 byte_offset;     /* Offset to start of DLS file */
} snd_audfmt_dls_packet_type;
#endif /* FEATURE_DLS */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
typedef struct {
  snd_hdr_type              hdr;
  snd_audfmt_poly_mode_type mode;
} snd_multi_pmode_packet_type;

typedef struct {
  snd_hdr_type              hdr;
  snd_poly_mode_cb_func_ptr_type pm_callback_ptr;
} snd_multi_get_pmode_packet_type;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

typedef struct{
  snd_hdr_type         hdr;            /* Command Header               */
  snd_cb_func_ptr_type callback_ptr;   /* Callback function            */
} snd_aux_pcm_clk_ctl_packet_type;

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/* Adjust audio sample
*/
typedef struct
{
  snd_hdr_type                   hdr;      /* Command Header              */
  snd_priority_type              priority; /* Priority of the sound       */
  boolean                        add_sample;   /* Adjust sample type Add/Drop */
} snd_af_adjust_sample_packet_type;
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
typedef struct {
  snd_hdr_type              hdr;            /* Command Header     */
  snd_cmx_af_info_type      codec_info;
  const void                *client_data;
} snd_af_codec_update_packet_type;

typedef struct {
  snd_hdr_type                          hdr;
  snd_priority_type                     priority; /* Priority of the sound  */
  snd_af_dual_mono_mode_enum_type       mode;
} snd_af_dual_mono_mode_packet_type;

typedef struct {
  snd_hdr_type                          hdr;
  snd_priority_type                     priority; /* Priority of the sound */
  snd_af_sbr_mode_enum_type             mode;
} snd_af_sbr_mode_packet_type;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Sound packets commands 
*/
typedef union {
  snd_hdr_type              hdr;       /* Plain header                      */
  snd_device_packet_type    dev;       /* Device     packet                 */
  snd_vol_packet_type       vol;       /* Volume level packet               */
  snd_get_vol_packet_type   get_vol;   /* Get Volume level packet           */
  snd_level_packet_type     level;     /* Max Volume level packet           */
  snd_tone_packet_type      tone;      /* Single tone packet                */
  snd_sound_id_packet_type  sound_id;  /* Sound from database packet        */
  snd_sound_packet_type     sound;     /* Sound from list of tones packet   */
  snd_sound_stop_type       snd_stop;  /* Stop all Sounds                   */
#ifdef FEATURE_VOICE_PLAYBACK
  snd_vp_packet_type        vp;        /* Voice Prompt packet               */
  snd_vp_sig_packet_type    vp_sig;    /* Signal from vocoder for VP        */
#endif
#ifdef FEATURE_VOICE_MEMO
  snd_vp_action_type        vp_control;/* Voice Prompt control              */
#endif
  snd_filters_packet_type   filters;   /* Type of Filtering Wanted          */
  snd_ec_ns_mode_packet_type ec_ns_mode; /* EC/NS control                   */
  snd_vox_mode_packet_type  vox_mode;  /* VOX mode                          */
  snd_sidetone_mode_packet_type  sidetone_mode; /* sidetone mode            */
#ifdef FEATURE_AUDIO_AGC
  snd_agc_avc_packet_type   avc_ctl;   /* Automatic Volume Control          */
  snd_agc_avc_packet_type   agc_ctl;   /* Automatic Gain Control            */
#endif
  snd_smartsnd_packet_type	smartsnd_ctl; /*AGC/RVE/AVC Controls			*/

  snd_stereo_packet_type    stereo;    /* Car Stereo Mute Packet            */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */
  snd_sound_get_time_packet_type  sound_get_time; /* Get playback time      */
  snd_sound_get_spec_packet_type  sound_get_spec; /* Get audio file format  */
#ifdef FEATURE_AUDIO_FORMAT
  snd_sound_flow_cmd_packet_type  sound_flow;     /* Sound functionality    */
  snd_av_sync_cmd_packet_type     av_sync;        /* AV Sync functionality  */
  snd_sequence_cmd_packet_type    seq_cmd;        /* Sequence command       */

  snd_cmx_af_read_cb_packet_type  cmx_af_read_cb; /* Data read callbacks    */
  snd_cmx_af_dls_cb_packet_type   cmx_af_dls_cb;  /* Get DLS callbacks      */
  snd_cmx_af_preparse_done_packet_type
                                  preparse;       /* Preparse done command  */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
  snd_af_adec_int_packet_type     af_adec_int;    /* Get adec data request  */
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
#ifdef FEATURE_QCP
  snd_qcp_get_func_packet_type qcp_get_func; /* QCP get function command    */
  snd_qcp_set_func_packet_type qcp_set_func; /* QCP set function command    */
  snd_qcp_rec_packet_type      qcp_rec;      /* QCP record command packet   */
  snd_qcp_rec_cb_packet_type   qcp_rec_cb;   /* QCP record vocoder callback */
  snd_qcp_rec_buf_packet_type  qcp_buffer;   /* QCP record buffer packet    */
#endif /* FEATURE_QCP */
#ifdef FEATURE_PCM_REC
  snd_pcm_rec_packet_type        pcm_rec;        /* PCM record pkt          */
  snd_pcm_rec_buf_packet_type    pcm_rec_buf;    /* PCM record buffer pkt   */
  snd_pcm_rec_cmd_packet_type    pcm_rec_cmd;    /* PCM record command pkt  */
  snd_pcm_rec_status_packet_type pcm_rec_status; /* PCM record status pkt   */
#endif /* FEATURE_PCM_REC */
#ifdef FEATURE_MM_REC
  snd_mm_rec_packet_type         mm_rec;         /* MM record pkt           */
  snd_mm_rec_buf_packet_type     mm_rec_buf;     /* MM record buffer pkt    */
  snd_mm_rec_cmd_packet_type     mm_rec_cmd;     /* MM record command pkt   */
  snd_mm_rec_status_packet_type  mm_rec_status;  /* MM record status pkt    */
  snd_mm_rec_isr_type            mm_rec_isr;    /* MM record isr pkt      */
#endif /* FEATURE_MM_REC */
#ifdef FEATURE_AUDFMT_EQUALIZER
  snd_equalizer_ctl_packet_type       eq_ctl;    /* Equalizer control cmd   */
  snd_equalizer_set_packet_type       eq_set;    /* Equalizer set command   */
  snd_equalizer_gain_packet_type      eq_gain;   /* Equalizer gain command  */
#endif /* FEATURE_AUDFMT_EQUALIZER */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
  snd_spectrum_analyzer_ctl_packet_type  sp_ctl;  /* Spectrum analyzer      */
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */
#ifdef FEATURE_AUDFMT_QCONCERT
  snd_qconcert_ctl_packet_type    qconcert_ctl;    /* QConcert control cmd  */
  snd_qconcert_params_packet_type qconcert_params; /* QConcert Config cmd   */
#endif /* FEATURE_AUDFMT_QCONCERT */
#endif /* FEATURE_AUDIO_FORMAT */
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) 
  snd_pp_ctl_packet_type pp_ctl;           /* ARM Post Processing config.  */
  snd_pp_ctl_resp_packet_type pp_resp_ctl; /* ARM PP data from DSP         */
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE  */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  snd_sbc_ctl_bitrate_packet_type sbc_bitrate_ctl;
  snd_sbc_ctl_packet_type         sbc_ctl; /* SBC Init to be called by BT */
  snd_sbc_params_packet_type     sbc_params;
#endif
#ifdef FEATURE_AVS_I2SSBC
  snd_i2ssbc_ctl_packet_type     i2ssbc_ctl;
#endif /* FEATURE_AVS_I2SSBC */
#ifdef FEATURE_MIDI_OUT
  snd_midi_out_packet_type     midi_out;      /* MIDI_OUT command           */
  snd_midi_out_open_packet_type midi_out_open;/* MIDI_OUT_OPEN command      */
#ifdef FEATURE_MIDI_OUT_QCP
  snd_midi_out_qcp_packet_type midi_out_qcp;  /* MIDI_OUT_QCP command       */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_QAUDIOFX
  snd_3d_pos_enable_packet_type        enable_3d_pos;    /* 3D pos enable cmd */
  snd_3d_pos_position_packet_type      position_3d_pos;  /* Position command  */
  snd_3d_pos_lsnr_orient_packet_type   orient_3d_pos;    /* Lsnr orient cmd   */
  snd_3d_pos_velocity_packet_type      velocity_3d_pos;  /* Position command  */
  snd_3d_reverb_enable_packet_type     rev_flag;/* Enable/Disable Reverb cmd */
  snd_3d_reverb_packet_type            reverb_3d_pos;    /* Reverb command  */
  snd_3d_pos_src_enable_packet_type    src_enable_3d;/* Source 3d enable cmd */
  snd_3d_pos_src_reset_packet_type     src_reset_3d;/* Source 3d enable cmd */
  snd_3d_pos_src_vol_packet_type       src_vol_3d_pos;   /* Source volume cmd */
  snd_3d_pos_src_vol_atten_packet_type vol_atten_3d_pos; /* Src vol atten cmd */
  snd_3d_reverb_src_vol_packet_type reverb_vol_3d_pos;/* Reverb volume cmd */
  snd_3d_pos_query_packet_type         query_3d_pos;     /* 3D pos query cmd  */
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_DLS
  snd_audfmt_dls_packet_type   audfmt_dls;    /* DLS load/unload command    */
#endif /* FEATURE_DLS */
#ifdef FEATURE_AUDFMT_HYBRID_MODE
  snd_multi_pmode_packet_type     pmode_set;
  snd_multi_get_pmode_packet_type pmode_get;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
  snd_aux_pcm_clk_ctl_packet_type aux_pcm_clk_ctl; /* auxpcm clok enable/disable cmd */
#ifdef FEATURE_AUDIO_PCR_SUPPORT
  snd_af_adjust_sample_packet_type adjust_sample; /* Add/drop one sample */
#endif /* FEATURE_AUDIO_PCR_SUPPORT */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  snd_af_codec_update_packet_type   codec_update;
  snd_af_dual_mono_mode_packet_type dual_mono_mode;
  snd_af_sbr_mode_packet_type       sbr_mode;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */
  snd_aux_line_in_packet_type       aux_linein;       /* Aux Line In cmd packet     */

  snd_slowtalk_ctl_packet_type    slowtalk_ctl;    /* slowtalk control cmd  */
  snd_slowtalk_params_packet_type slowtalk_params; /* slowtalk Config cmd   */

} snd_packets_type;

/*--------------------------------------------------------------------------
** Sound Internal definitions
**------------------------------------------------------------------------*/

/* Type of sound that is currently playing */
typedef enum {
  SND_TYPE_TONE,                  /* DTMF playing                         */
#ifdef FEATURE_VOICE_PLAYBACK
  SND_TYPE_VOICE_PROMPT,          /* Voice prompt is playing              */
#endif
#ifdef FEATURE_MIDI
  SND_TYPE_AUDFMT,                /* Audio format sound (midi) is playing */
#endif
#ifdef FEATURE_MP3
  SND_TYPE_MP3,                   /* Audio format sound (MP3) is playing  */
#endif
#ifdef FEATURE_AAC
  SND_TYPE_AAC,                   /* Audio format sound (AAC) is playing  */
#endif
#ifdef FEATURE_REAL_AUDIO
  SND_TYPE_RA,                    /* Audio format sound (Real) is playing */
#endif
#ifdef FEATURE_IMELODY
  SND_TYPE_IMY,                   /* iMelody sound is playing             */
#endif
#ifdef FEATURE_ADPCM
  SND_TYPE_ADPCM,                 /* ADPCM sound is playing               */
#endif
#ifdef FEATURE_PCM
  SND_TYPE_PCM,                   /* PCM sound is playing                 */
#endif
#ifdef FEATURE_WMA
  SND_TYPE_WMA,                   /* WMA sound is playing                 */
  SND_TYPE_WMA_PRO,               /* WMA Pro sound is playing             */
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
  SND_TYPE_AMR_WB,                /* AMR-WB sound is playing              */
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  SND_TYPE_AMR_WBPLUS,            /* AMR-WB+ sound is playing             */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_VOC_G711
  SND_TYPE_WAV_MULAW,             /* Wav, Mu-Law format                   */
  SND_TYPE_WAV_ALAW,              /* Wav, A-Law format                    */
#endif /* FEATURE_VOC_G711 */

  SND_TYPE_SILENCE                /* Silence (i.e. nothing is playing)    */
} snd_type_type;

/* Frequency pair which defines a tone/DTMF
*/
typedef struct {
  word           freq_hi;        /* High frequency value in hz */
  word           freq_lo;        /* Low frequency value in hz  */
} snd_freq_pair_type;

#ifdef FEATURE_AUDIO_FORMAT

#ifdef FEATURE_MPEG_AUDIO
typedef struct {
  uint8    mpeg_mask[4];
  uint8    mpeg_header[4];
  uint8    mpeg_mask_size;        /* size in bytes             */
} snd_audfmt_mpeg_header_config;

#ifdef FEATURE_AAC
typedef struct {
  snd_audfmt_mpeg_header_config mpeg_hdr_config;
  audmain_aac_config_type       params;
} snd_audfmt_aac_dsp_config;
#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
typedef struct {
  snd_audfmt_mpeg_header_config mpeg_hdr_config;
  audmain_mp3_config_type       params;
} snd_audfmt_mp3_dsp_config;
#endif /* FEATURE_MP3 */


#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_REAL_AUDIO
typedef struct {
  snd_ra_decoder_mode_type      decoder_mode;   /* Decoder (Gecko) mode     */
  snd_sample_rate_type          sample_rate;    /* Sampling rate            */
  snd_ra_channel_type           num_channels;   /* Number of channels       */
  snd_ra_samples_per_frame_type samples_per_frame; /* Samples per frame     */
  uint16                        num_regions;    /* Number of regions        */
  uint16                        bits_per_frame; /* Number of bits per frame */
  uint16                        coupling_start; /* Stereo coupling region   */
  uint16                        coupling_quant; /* Coupling quantization    */
  uint16                        frame_erasure;  /* Frame erasure flag       */
} snd_audfmt_ra_dsp_config;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
/* Structure used to pass the WMA decoder configuration to the DSP
*/
typedef struct {
  byte version;               /* version number 1 or 2               */ 
  uint8  bits_per_sample;      /* Valid bits per sample             */
  uint16 channels;              /* number of channels 1 or 2          */
  uint16 bytes_per_second;    /* bytes per second for playing back   */
  uint16 sample_rate;         /* Vaild sample frequency in Hz        */
  uint16 encoder_options;     /* Encoder option                      */
  uint16 virtual_pkt_len;     /* VirtulaPacketLength in ASF */
  uint16 advanced_encoder_opt1; /* Support for LBR bitstream */
  uint16 channel_mask;         /* For stero or mono streams  */
  uint16 format_tag;           /* Codec ID: 0x162 or 0x166 for WMAPRO 10 */
  uint32 advanced_encoder_opt2; /* Support for LBR bitstream */

} snd_audfmt_wma_dsp_config_type;
#endif

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
typedef struct {
  uint16 sample_rate;  /* Sampling rate in Hz  */
  uint16 num_channels; /* Decoder channel mode */
} snd_audfmt_amr_wbp_dsp_config_type;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

typedef union {
#ifdef FEATURE_MPEG_AUDIO
  snd_audfmt_mpeg_header_config mpeg_hdr_config;  /* MP3 data       */
#ifdef FEATURE_MP3
  snd_audfmt_mp3_dsp_config     mp3_config;       /* MP3 data       */
#endif
#ifdef FEATURE_AAC
  snd_audfmt_aac_dsp_config     aac_config;       /* AAC data       */
#endif
#endif

#ifdef FEATURE_REAL_AUDIO
  snd_audfmt_ra_dsp_config      ra_config;        /* RealAudio data */
#endif

#ifdef FEATURE_WMA
  snd_audfmt_wma_dsp_config_type wma_config;
#endif

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  snd_audfmt_amr_wbp_dsp_config_type amr_wbp_config; /* AMR-WB+ data */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

  byte dummy;
} snd_audfmt_dsp_config;

typedef void (*snd_av_sync_int_cb_ptr_type) (
  qword timestamp,      /* Timestamp for the callback in ms          */
  qword num_of_samples, /* Sample counter. Use only lower 48 bits    */
  qword num_of_bytes    /* Sample counter. Use only lower 48 bits    */
);

#endif /* FEATURE_AUDIO_FORMAT */

/* All the information required to play a tone to a specific generator
** on a specific audio path.
*/
typedef struct {
  snd_freq_pair_type   freq_pair;     /* The frequencies of the tone to play */
  snd_device_type      device;        /* Device                              */
  snd_method_type      method;        /* Method                              */
  uint32               time_remain_ms;/* Time remaining on this tone [mSec]  */
  byte                 vol;           /* Volume                              */
  snd_apath_type       dtmf_path;     /* Who will hear the tone tx/local/mute*/
  snd_type_type        type;          /* Type of sound (DTMF, VP, ...)       */
  snd_type_type        aux_type;      /* Type of AUX sound                   */

#ifdef FEATURE_AUDIO_FORMAT
  audmain_cmd_buf_type *audfmt_buf;   /* Audio format command buffer         */

  snd_audfmt_dsp_config audfmt_dsp_config; /* DSP configuration specific to  */
                                           /* the type of audio being played */

  snd_av_sync_int_cb_ptr_type av_sync_cb;       /* AV sync cb pointer    */
  snd_av_sync_interval_type   av_sync_mode;     /* AV sync mode          */
  uint32                      av_sync_interval; /* Interval for AV sync  */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
  voc_adec_cb_func_struct audfmt_adec_cb;  /* ADEC callback function         */
  void                    *audfmt_adec;    /* Client data for ADEC callbacks */
  /* Codec update callback function */
  voc_codec_update_cb_func_struct audfmt_codec_update_cb;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

  snd_sample_rate_type        sample_rate; /* Sample rate of audio playback   */
  snd_audfmt_start_enum_type  audfmt_path; /* Playback path (LOCAL, RX, etc)  */
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
} tone_status_type;

/* Path State Type
** Includes: physical path, whether the path is muted or not,
** and the current volume.
*/
typedef struct {
  snd_device_type         device;       /* The audio device connected   */
  snd_method_type         method;       /* The method used              */
  snd_mute_control_type   mute;         /* Path's mute state            */
  byte                    vol;          /* The current volume.          */
} snd_pstate_type;

/* Path status type */
typedef enum {
  SND_PATH_NOT_READY,            /* Path is not ready                    */
  SND_PATH_VOC_READY,            /* Path is ready, vocoder not warmed up */
  SND_PATH_READY                 /* Path is ready                        */
} snd_path_ready_type;

/* Type indicating any extra audio processing the the vocoder should perform
** for this device.
*/
typedef enum {
  SND_PROCESS_NONE = 0x00,         /* No additional audio processing       */
  SND_PROCESS_ESEC = 0x01,         /* Enable Ear Seal Echo Cancellation    */
  SND_PROCESS_NOT_SET = 0x80       /* Use as first time indicator          */
} snd_audio_process_type;

#ifdef FEATURE_VOICE_PLAYBACK
/* Maximum number of voice prompts allowed                              */
#define SND_MAX_VP  14
/* Voice prompt queue functions return type                             */
typedef enum {
  SND_PPQ_PQ_FAIL,            /* returned if function not successful    */
  SND_PPQ_PQ_SUCCESS          /* returned if function successful        */
} snd_ppq_pq_return_type;

/* Structure for queue of voice prompt pointers                         */
typedef struct {
  int number;          /* number of voice prompts in queue              */
  voc_pb_header_type *prompts[SND_MAX_VP];
                       /* queue of the voice prompt pointers            */
  const void         *client_data[SND_MAX_VP];
                       /* client data associated with voice prompt      */
} snd_prompt_ptr_q_type;

/* Structure for queue of voice prompts                                 */
typedef struct {
  int index;                      /* current index into queue           */
  int num_pending;                /* number of VPs left in queue        */
  word prompts[SND_MAX_VP];       /* queue of VPs                       */
} snd_prompts_q_type;

#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_MIDI_OUT
/* These defines indicate the standard MIDI commands that are supported
** by the MIDI output device.
*/
#define SND_MIDI_NOTE_OFF              0x80
#define SND_MIDI_NOTE_ON               0x90
#define SND_MIDI_AFTER_TOUCH           0xA0
#define SND_MIDI_CONTROL_CHANGE        0xB0
#define SND_MIDI_PROGRAM_CHANGE        0xC0
#define SND_MIDI_CHANNEL_PRESSURE      0xD0
#define SND_MIDI_PITCH_WHEEL           0xE0
#define SND_MIDI_RESET                 0xFF
#define SND_MIDI_MAX_VALUE             0x7F
#endif /* FEATURE_MIDI_OUT */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* This type is used to indicate the type of audio decoder that should be used.
*/
typedef enum {
#ifdef FEATURE_QSYNTH_COMMON
  SND_ADEC_MODE_QSYNTH,    /* QSynth audio decoder mode */
#endif
#ifdef FEATURE_QTUNES_COMMON
  SND_ADEC_MODE_QTUNES,    /* QTunes audio decoder mode */
#endif
#ifdef FEATURE_ADPCM
  SND_ADEC_MODE_ADPCM,     /* ADPCM audio decoder mode  */
#endif
#ifdef FEATURE_PCM
  SND_ADEC_MODE_PCM,       /* PCM audio decoder mode    */
#endif
#ifdef FEATURE_VOC_G711
  SND_ADEC_MODE_WAV_MULAW,
  SND_ADEC_MODE_WAV_ALAW,
#endif /* FEATURE_VOC_G711 */
  SND_ADEC_MODE_MAX
} snd_adec_mode_type;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/* Type used to indicate the source of playback of ADPCM file
*/
typedef enum {
  SND_ADEC_ADPCM_PLAY_FILE,     /* ADPCM file started with play command    */
  SND_ADEC_ADPCM_PLAY_AUDFADEC  /* ADPCM file started with audfadec module */
} snd_adec_adpcm_play_enum_type;

/* This type is used to indicate the format of audio decoder data
*/
typedef enum {
  SND_ADPCM_ADPCM_MODE,         /* ADPCM format            */
  SND_ADPCM_YADPCM_MODE,        /* Y-ADPCM format          */
  SND_ADPCM_SIGNED_PCM_MODE,    /* Signed PCM format       */
  SND_ADPCM_UNSIGNED_PCM_MODE,  /* Unsigned PCM format     */
#ifdef FEATURE_VOC_G711
  SND_ADPCM_WAV_ALAW_MODE,      /* Wav, A-Law format       */
  SND_ADPCM_WAV_MULAW_MODE      /* Wav, Mu-Law format      */
#endif /* FEATURE_VOC_G711 */
} snd_adpcm_mode_enum_type;

/* Type to describe the adpcm format
*/ 
typedef struct {
  uint16 channels;               /* Number of channels                        */
  uint32 sample_rate;            /* Samplers per second                       */
  uint32 bytes_rate;             /* Average bytes per second                  */
  uint16 frame_size;             /* Block Align, num of bytes for n-channels  */
  uint16 bits_per_sample;        /* Number of bits used for each sample       */
  snd_adpcm_mode_enum_type mode; /* Format of audio decoder data              */
} snd_adec_adpcm_format_type;
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/* This type is used to set the synth into a particular audio decoder mode
*/
typedef enum {
  SND_SET_ADEC_MODE_ADPCM,  /* Set mode to ADPCM   */
  SND_SET_ADEC_MODE_YADPCM, /* Set mode to Y-ADPCM */
  SND_SET_ADEC_MODE_QCP     /* Set mode to QCP     */
#ifdef FEATURE_PCM
  , SND_SET_ADEC_MODE_PCM   /* Set mode to PCM     */
#endif /* FEATURE_PCM */
} snd_set_adec_mode_enum_type;
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*---------------------------------------------------------------------------
  MSM2Px macros, commands, definitions, and types
---------------------------------------------------------------------------*/

/* The maximum number of milliseconds between tone updates.
** This limit is imposed by the max interval for a rex timer (16 bits).
**
** The Vocoder's tone generation granularity (units of tone-time) are
** factored in to allow for overlap of continuing tones, instead of
** terminate-restart.
*/
#define SND_VOC_GRAIN   10
#define SND_MAX_PERIOD  (0xFFFF - SND_VOC_GRAIN)

/* Bitmask position indicating whether BIO_SW10 should be checked for
** overriding volume settings during DOTG calls. For use in ctl_flags
** of snd_vol_ctl_type structure. Rest of bitfield positions are 
** reserved for future use.
*/
#define SND_CHECK_BIO_SW10_MASK     0x01
#define SND_CHECK_BIO_SW10_ENABLE   0x01
#define SND_CHECK_BIO_SW10_DISABLE  0x00

/* Bitmask position indicating whether DATA_FILTERS should be checked for
** overriding volume settings. For use in ctl_flags of snd_vol_ctl_type 
** structure. Rest of bitfield positions are reserved for future use.
*/
#define SND_VERIFY_DATA_FILTER_MASK     0x02
#define SND_VERIFY_DATA_FILTER_ENABLE   0x02
#define SND_VERIFY_DATA_FILTER_DISABLE  0x00

#ifdef SND_HFK_PLUS
#error code not present
#endif /* SND_HFK_PLUS */

#ifdef FEATURE_MIDI
/* The following type is used for setting up the volume level for the qsynth 
** image. when it is playing Webaudio or SMAF file.
*/
typedef enum {
  SND_QSYNTH_DEFAULT_VOL_LVL = 0,  /* Default volume level for MIDI/WEBAUDIO */
  SND_QSYNTH_SMAF_VOL_LVL_MA2=1,    /* Volume level for SMAF   */
  SND_QSYNTH_SMAF_VOL_LVL_MA3=1,
  SND_QSYNTH_SMAF_VOL_LVL_MA5=2  /*volume level for Ma5 Smaf*/
} snd_qsynth_vol_mode_enum_type;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_GASYNTH_INTERFACE
typedef enum {
  SND_VOC_ADEC_QCP,
  SND_VOC_ADEC_AMR
} snd_voc_adec_enum_type;
#endif /* FEATURE_GASYNTH_INTERFACE */
                                                                                        
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_vp_callback

DESCRIPTION
  This function calls the function pointed to by snd_vp_callback_func,
  if one exists.

DEPENDENCIES
  snd_vp_callback_func

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_vp_callback( const void *client_data, snd_status_type status );
#endif
/* <EJECT> */

/*===========================================================================

                      MACRO AND DEFINITION DECLARATIONS

===========================================================================*/

/* Certain Codec Gains (for voc_codec_pad() Calls).  There are 8 Steps
** of 3DB each for the Handset and 8 steps of 4.25 DB each for the Car Kit.
*/
#define SND_CODEC_0DB        (byte)(~(0x0<<5))     /* 0 DB Above Minimum   */
#define SND_CODEC_MIN_VOL    SND_CODEC_0DB         /* Minimum Codec Volume */

/* Amount of time in ms to wait for the sound task to wait for the vocoder 
** to become available to play a tone. If, at the end of this time, the 
** vocoder is not available then SND_TONE will be expired.
*/
#define SND_TONE_VOC_TIMEOUT 500

/* <EJECT> */
/*===========================================================================

MACRO SND_NEW_TONE

DESCRIPTION
  See if the noise indexed by a snd_array_idx_type is in a change mask
  SND_TONE_M..SND_PLAYBACK_M or SND_TONE_M..SND_SND_LO_M must be sequential.

PARAMETERS
  idx   Index into the change mask defined above.
  mask  Mask of current changes.

DEPENDENCIES
  idx must be in the range of masks specified above.

RETURN VALUE
  True  New tone
  False Not a new tone

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_VOICE_PLAYBACK
#define SND_NEW_TONE(idx,mask)  (((mask) & (SND_PLAYBACK_M << (idx))) != 0)
#else
#define SND_NEW_TONE(idx,mask)  (((mask) & (SND_SND_LO_M << (idx))) != 0)
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
/*===========================================================================

MACRO SND_BIO_SW10_IS_ENABLED

DESCRIPTION
  This macro will check the proper bitmask of the byte to see if 
  BIO_SW10 should be checked for volume override during a DOTG call.

PARAMETERS
  bitmask

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if BIO_SW10 should be checked.
  FALSE if BIO_SW10 should not be checked.

SIDE EFFECTS
  None

===========================================================================*/
#define SND_BIO_SW10_IS_ENABLED(v) ((v & SND_CHECK_BIO_SW10_MASK) == \
                                    SND_CHECK_BIO_SW10_ENABLE)
/* <EJECT> */
/*===========================================================================

MACRO SND_VERIFY_DFILT_IS_DISABLED

DESCRIPTION
  This macro will check the proper bitmask of the byte to see if
  DATA FILTER should override the volume settings.

PARAMETERS
  bitmask

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if DATA FILTER should not override volume settings.
  FALSE if DATA FILTER should override volume settings.

SIDE EFFECTS
  None

===========================================================================*/
#define SND_VERIFY_DFILT_IS_DISABLED(v) ((v & SND_VERIFY_DATA_FILTER_MASK) == \
                                         SND_VERIFY_DATA_FILTER_DISABLE)
/* <EJECT> */
/*===========================================================================

MACRO SND_DEBUG_MSG

DESCRIPTION
  Allows leaving certain Debug Messages in Sound that can be turned on/off
  Regionally.  This macro is a NOP unless /DSND_DEBUG is added to the
  compile line for the routines.

PARAMETERS
  fmt Format string for message (printf style).  Note, this parameter
      must be a string literal (e.g. "Tuned to %lx"), not a variable.
  p1  1st parameter for format string
  p2  2nd parameter for format string
  p3  3rd parameter for format string

DEPENDENCIES
  Turned on/off by /DSND_DEBUG at compile time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef SND_DEBUG
/* SND_DEBUG is meaningless unless at we are compiling with at least
** MSG_LVL_FATAL.  Force either no SND_DEBUG or else MSG_LVL_FATAL.
*/
#if MSG_LEVEL > MSG_LVL_FATAL
#error /DSND_DEBUG Requires at least MSG_LVL_FATAL
#endif
#define SND_DEBUG_MSG(fmt,p1,p2,p3)     MSG_FATAL(fmt,p1,p2,p3)
#else
#define SND_DEBUG_MSG(fmt,p1,p2,p3)
#endif
/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION snd_audfmt_wait

DESCRIPTION
  This function waits a maximum of SND_AUDFMT_DATA_TIMEOUT until
  SND_AUDIO_FORMAT_SIG is set.

DEPENDENCIES
  snd_audfmt_data_timer

RETURN VALUE
  FALSE if wait timed out.
  TRUE if signal was set properly.

SIDE EFFECTS
  snd_audfmt_data_timer

===========================================================================*/
extern boolean snd_audfmt_wait(void);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_continue

DESCRIPTION
  This function sets the SND_AUDIO_FORMAT_SIG signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_audfmt_continue(void);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_preparse_done

DESCRIPTION
  This function issues a command to sound to indicate that the audio format
  file has been parsed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_preparse_done ( void *client_data );

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_new_data

DESCRIPTION
  This function receives new data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_new_data (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 position
);

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION snd_audfmt_new_dls_data

DESCRIPTION
  This function receives new data for the DLS parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_new_dls_data (
  void   *data,
  uint8  *buf,
  uint32 length,
  uint32 position
);
#endif /* FEATURE_DLS */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_header_data

DESCRIPTION
  This function receives new header data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_header_data (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 position
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_dls_data

DESCRIPTION
  This function receives DLS data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_dls_data (
  void   *client_data,
  uint8  *data,
  uint8  *buf,
  uint32 length
);
/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION snd_audfmt_adec_int

DESCRIPTION
  This function requests new data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_adec_int (
  void *snd_ptr
);
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION snd_qcp_rec_new_buffer_req

DESCRIPTION
  This function receives new buffer for QCP recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void snd_qcp_rec_new_buffer_req ( uint8 *buf, uint32 num_bytes );

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_rec_new_buffer

DESCRIPTION
  This function receives new buffer for QCP recording in command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_qcp_rec_new_buffer ( uint8 *buf, uint32 num_bytes );
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION snd_pcm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_pcm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_pcm_rec_data_struct_type *data,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_buf_cb

DESCRIPTION
  This function is used as the callback function for PCM recording
  buffer requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_buf_cb (
  uint8            *buffer,
  uint32           num_bytes,
  const void       *client_data
);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION snd_mm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_mm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_mm_rec_data_struct_type *data,
  snd_mm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

/*===========================================================================

FUNCTION snd_mm_record_buf_cb

DESCRIPTION
  This function is used as the callback function for MultiMedia recording
  buffer requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_buf_cb (
  uint8            *buffer,
  uint32           num_bytes,
  const void       *client_data
);
#endif /* FEATURE_MM_REC */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION snd_adec_adpcm_stop

DESCRIPTION
  This function stops playback on the specified ADPCM channel.

DEPENDENCIES
  FEATURE_QSYNTH_ADPCM.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void snd_adec_adpcm_stop ( uint32 channel );
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION snd_adec_adpcm_start

DESCRIPTION
  This function initializes the specified ADPCM channel.

DEPENDENCIES
  FEATURE_QSYNTH_ADPCM.

RETURN VALUE
  snd_status_type
  - SND_SUCCESS: command was successful.
  - SND_FAILURE: command failed.

SIDE EFFECTS
  None.

===========================================================================*/
extern snd_status_type snd_adec_adpcm_start (
  uint32                        channel,
  snd_adec_adpcm_play_enum_type adpcm_play_type,
  snd_adec_adpcm_format_type    *format,
  snd_adec_start_mode_enum_type start_mode
);
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION snd_set_adec_song_config

DESCRIPTION
  This function sets the song config for Animate-AAC and Codec Clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
boolean snd_set_adec_song_config(
  audmain_song_config_type *config, 
  snd_sample_rate_type    samplerate
);
#endif

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION snd_get_midi_out_enabled_flag

DESCRIPTION
  This function returns the state of the MIDI OUT device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean snd_get_midi_out_enabled_flag (void);
#endif /* FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION snd_audfmt_adec_mvol

DESCRIPTION
  This function sets the ADEC master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type snd_audfmt_adec_mvol (
  uint16 vol
);
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_vol

DESCRIPTION
  Modifies the audio decoder channel volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type snd_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 volume
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_pan

DESCRIPTION
  Modifies the audio decoder channel volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type snd_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_start

DESCRIPTION
  Starts the adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type snd_audfmt_adec_chan_start (
  uint32  channel
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_pause

DESCRIPTION
  Sets the pause flag for adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type snd_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_flush

DESCRIPTION
  Sends a flush command to the adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_audfmt_adec_chan_flush (
  uint32  channel
);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION snd_set_adec_mode

DESCRIPTION
  This function sets the audio decoder mode for the QSynth image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_set_adec_mode ( snd_set_adec_mode_enum_type mode );
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE 
/*===========================================================================

FUNCTION snd_voc_adec_start

DESCRIPTION
  This function starts the audio decoder for QCP or AMR-NB playback
  
DEPENDENCIES
  NONE
  
RETURN VALUE
  A value of snd_status_type to indiciate whether DSP is properly
  configured to accept 13K audio data. 

SIDE EFFECTS
  NONE
===========================================================================*/
extern snd_status_type snd_voc_adec_start(
  uint32                        channel,
  snd_voc_adec_enum_type        voc_type,
  snd_adec_start_mode_enum_type start_mode
);

/*===========================================================================

FUNCTION snd_adec_data_consumed

DESCRIPTION
  This function detects decoder has finished consuming data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - data_is_needed flag is set
  FALSE - data_is_needed flag is not set 

SIDE EFFECTS
  NONE

===========================================================================*/
extern boolean snd_adec_data_consumed(
  uint8 channel
);
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION snd_3d_pos_enable_req

DESCRIPTION
  This function processes a 3D positional enable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_enable_req (
  snd_3d_pos_mode_enum_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/*===========================================================================

FUNCTION snd_3d_pos_reset_req

DESCRIPTION
  This function processes an environment 3D reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_reset_req (
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_pos_req

DESCRIPTION
  This function processes a 3D positional listener position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_pos_req (
  snd_3d_pos_position_cmd_type *pos,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_orient_req

DESCRIPTION
  This function processes a 3D positional listener orientation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_orient_req (
  snd_3d_pos_lsnr_orient_cmd_type *orient,
  snd_cb_func_ptr_type            cb_func,
  const void                      *client_data
);

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_lsnr_vel_req

DESCRIPTION
  This function processes a 3D positional listener velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_vel_req (
  snd_3d_pos_velocity_cmd_type *vel,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_req

DESCRIPTION
  This function processes a reverb preset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_set_req (
  snd_3d_reverb_enum_type   reverb_type,    /* Reverb Parameter type */
  snd_3d_reverb_cmd_type   *reverb_param,   /* Reverb Parameters     */
  snd_cb_func_ptr_type          cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_enable_req

DESCRIPTION
  This function processes a reverb enable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_enable_req (
  boolean                       enable,
  snd_cb_func_ptr_type          cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_enable_req

DESCRIPTION
  This function processes a source 3D positional enable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_enable_req (
  uint32                    channel,
  snd_3d_pos_mode_enum_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/*===========================================================================

FUNCTION snd_3d_pos_src_reset_req

DESCRIPTION
  This function processes a source 3D reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_reset_req (
  uint32                    channel,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/*===========================================================================

FUNCTION snd_3d_pos_src_vol_req

DESCRIPTION
  This function processes a 3D positional source volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vol_req (
  uint32                         channel,
  snd_3d_pos_src_volume_cmd_type *vol,
  snd_cb_func_ptr_type           cb_func,
  const void                     *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_vol_atten_req

DESCRIPTION
  This function processes a 3D positional source volume attenuation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vol_atten_req (
  uint32                            channel,
  snd_3d_pos_src_vol_atten_cmd_type *vol_atten,
  snd_cb_func_ptr_type              cb_func,
  const void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_pos_req

DESCRIPTION
  This function processes a 3D positional source position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_pos_req (
  uint32                       channel,
  snd_3d_pos_position_cmd_type *pos,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_src_vel_req

DESCRIPTION
  This function processes a 3D positional source velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vel_req (
  uint32                       channel,
  snd_3d_pos_velocity_cmd_type *vel,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_src_vol_req

DESCRIPTION
  This function processes a source reverb volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_src_vol_req (
  uint32                       channel,
  sint31                       volume,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_env_req

DESCRIPTION
  This function processes a 3D positional environment parameter
  query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_query_env_req (
  snd_3d_pos_param_enum_type param,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_src_req

DESCRIPTION
  This function processes a 3D positional source parameter query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_query_src_req (
  snd_3d_pos_param_enum_type param,
  uint32                     channel,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
);
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION snd_set_dls_config

DESCRIPTION
  This function sets the DLS configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
typedef struct {
  uint32 hp_inst_def;   /* HP instrument definition table */
  uint32 hp_drum_def;   /* HP drum definition table       */
  uint32 hp_wave_info;  /* HP waveform table              */
  uint32 hq_inst_def;   /* HQ instrument definition table */
  uint32 hq_drum_def;   /* HQ drum definition table       */
  uint32 hq_wave_info;  /* HQ waveform table              */
  uint32 hq_inst_num;   /* Number of HQ instruments       */
  uint32 hq_drum_num;   /* Number of HQ drums             */
} snd_dls_config_type;

extern void snd_set_dls_config (
  snd_dls_config_type *dls_config
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_qsynth_vol_mode

DESCRIPTION
  This function sets the qsynth volume mapping configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_set_qsynth_vol_mode(
  snd_qsynth_vol_mode_enum_type mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_hybrid_mode

DESCRIPTION
  This function sets the hybrid mode configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_set_hybrid_mode (
  snd_audfmt_poly_mode_type hybrid_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_pause_seq

DESCRIPTION
  This function issues a pause command to the indictaed sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean snd_audfmt_pause_seq (
  uint32  sequence,
  boolean pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_set_vol_seq

DESCRIPTION
  This function issues a set volume command to the indictaed sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean snd_audfmt_set_vol_seq (
  uint32 sequence,
  uint16 vol
);

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_check_reset

DESCRIPTION
  This function issues a check reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_audfmt_check_reset ( void );
#endif /* FEATURE_MIDI */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
/*==========================================================================
 FUNCTION snd_multi_set_poly_mode_req

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
extern void snd_multi_set_poly_mode_req(
  snd_audfmt_poly_mode_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
);

/*==========================================================================
 FUNCTION snd_multi_get_poly_mode_req

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
extern void snd_multi_get_poly_mode_req(
  snd_poly_mode_cb_func_ptr_type cb_func,
  const void                     *client_data
);
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION snd_audfmt_codec_update_int

DESCRIPTION
  This function requests new data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
extern void snd_audfmt_codec_update_int(
  uint32 dualmono,
  void *snd_ptr
);

/*===========================================================================

FUNCTION snd_audfmt_dual_mono_mode

DESCRIPTION
  This function sets dual mono mode.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
extern void snd_audfmt_dual_mono_mode_req(
  snd_priority_type                   priority,
  snd_af_dual_mono_mode_enum_type     dual_mono_mode,
  snd_cb_func_ptr_type                callback_ptr,
  const void                          *client_data
);

/*===========================================================================

FUNCTION snd_audfmt_sbr_mode_mode

DESCRIPTION
  This function sets sbr enable/disable flag of decoder.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
extern void snd_audfmt_sbr_mode_req(
  snd_priority_type              priority,
  snd_af_sbr_mode_enum_type      sbr_mode,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
);


/*===========================================================================

FUNCTION snd_af_dual_mono_mode_req

DESCRIPTION
 Dual mono mode req fuction.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
extern void snd_audfmt_dual_mono_mode_req(
  snd_priority_type                   priority,
  snd_af_dual_mono_mode_enum_type     mode,
  snd_cb_func_ptr_type                callback_ptr,
  const void                          *client_data
); 

/*===========================================================================

FUNCTION snd_audfmt_codec_update_req

DESCRIPTION
 codec udpate req function

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
extern void snd_audfmt_codec_update_req(
  snd_cmx_af_info_type                *codec_info, 
  const void                          *client_data
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */



/*==========================================================================

 FUNCTION snd_smartsnd_ctl_req

 DESCRIPTION
   Controls AGC/AVC/RVE.

 DEPENDENCIES
   None
   
 RETURN VALUE
   None

 SIDE EFFECTS
   None   
 ==========================================================================*/

extern void snd_smartsnd_ctl_req( snd_packets_type *cmd_ptr );
#endif /* SNDI_H */

