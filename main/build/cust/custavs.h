#ifndef CUSTAVS_H
#define CUSTAVS_H
/*===========================================================================

DESCRIPTION
  Configuration for AVS

  Copyright (c) 2011 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/build/cust/main/latest/inc/custavs.h#15 $ $DateTime: 2011/02/08 06:22:14 $ $Author: shravb $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   sb      Enabled FEATURE_LOCAL_DTMF_TX_VOICE
06/01/09   nr      AMR NB disbaled for ULC Low Tier builds.
05/05/09   knm     Defined FEATURE_ULC_WARMUP_DELAY macro for ULC
02/21/09   sj      Added FEATURE_AVS_QCAM_COMBO_VOC_INTERFACE feature for
                   fixing CR 171955.
12/22/08   knm     defined FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL to support dynamic
                   clock switching for enhanced AAC Plus audio files.
11/26/08    ry     define the feature FEATURE_AUDIO_CAMERA_CONCURRENCY only for
                   mid and high tier build.
06/08/08   vsud     enabled PCM packet logging feature
23/07/08   vsud    added audio calibration feature in the file.
22/07/08   vsud    removed incall supported in ULC./snd test code.
06/31/08   vsud    moved the incall feature to value added.
09/14/07  az/anb   Included the feature to turn on the hclk.
04/30/07   anb     Enabled FEATURE_AVS_INCALL_MIXER_ADEC_PB for InCall-MP3 support
04/26/07   kan     Enabled  QDSP_PACKET_SNIFFER for 1X vocoder packet log
02/28/07   sud     Enabled FEATURE_AVS_AUDIO_DECODER_SCO and
                   FEATURE_AVS_I2SSBC.
06/07/06    sg     Enabled Vocoder PCM interface explicitly since this is
                   required for FTM and vocoder debugging.
04/28/06   act     Initial version.
===========================================================================*/

/* -----------------------------------------------------------------------
** Audio / Vocoder / Sound
** ----------------------------------------------------------------------- */

/* enable ULC warmup delay feature to reduce the pop noise */
#define FEATURE_ULC_WARMUP_DELAY

#define FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL

/*enable the new Echo cancellation for the voc_combo..*/
#define FEATURE_AVS_NEXTGEN_EC

#define FEATURE_PCM_PKT_LOG

#define FEATURE_LOCAL_DTMF_TX_VOICE

/* Enable the vocoder PCM interfaces */
#define FEATURE_VOC_PCM_INTERFACE

/* Force all firmware image change events to be sent to the vocoder driver */
#define FEATURE_FORCE_VOCODER_EVENTS

/* Use the new Sound API */
#define  FEATURE_NEW_SOUND_API

/* Enable the Hclk */
#define  FEATURE_AVS_BUS_CLK_CTRL

/* Enable AUDIO AGC
*/
#define FEATURE_AUDIO_AGC

/* Enable Headset Support.
*/
#define FEATURE_HEADSET

/* Enable Analog HFK (SURF only)
*/
#define FEATURE_ANALOG_HFK

/* Enable TTY Support
*/
#define FEATURE_TTY
#define FEATURE_TTY_CDMA

/* Provides an interface to signal another application of the presence
 * or absense of TTY activity on the mobile link.
*/
#undef FEATURE_TTY_ACTIVITY_STATUS

/* Allows use of the internal codec's Stereo DAC found on the 6100,
 * 6300, 6250, and 6500 MSMs.
*/
#define FEATURE_INTERNAL_SDAC

/* Support for packet clients */
#define FEATURE_VOC_PACKET_INTERFACE

/**************************************************************************
[FEATURE NAME]:      QDSP_PACKET_SNIFFER
[SCOPE]:             AVS, MVS
[DESCRIPTION]:       1X vocoder packet logging
[OEM CAN ENABLE/DISABLE]: YES.
**************************************************************************/
#define QDSP_PACKET_SNIFFER

/* Enable Stereo DAC - if AUXPCM is not required */
#ifndef FEATURE_AUXPCM
  #ifndef FEATURE_INTERNAL_SDAC
     #define FEATURE_EXTERNAL_SDAC
  #endif
#endif

#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
   /* Adds calibration values and provides common code between
    * FEATURE_INTERNAL_SDAC and FEATURE_EXTERNAL_SDAC.
   */
   #define FEATURE_STEREO_DAC
#endif

#ifdef FEATURE_EXTERNAL_SDAC
  #ifndef FEATURE_I2C
     /* Enable the I2C driver.
     */
     #define FEATURE_I2C
  #endif
#endif

#ifdef FEATURE_BT
  /* I2SSBC feature
  */
  #define FEATURE_AVS_I2SSBC
  /* Enable MP3, AAC, MIDI, etc file playback on BT-SCO
  */
  #define FEATURE_AVS_AUDIO_DECODER_SCO
#endif

/* Enable Stereo ADC */
#if defined(FEATURE_AAC_REC) || defined(FEATURE_AVS_I2SSBC)
  #define FEATURE_EXTERNAL_SADC
#endif /* FEATURE_AAC_REC || FEATURE_AVS_I2SSBC */

/* The new Vocoder task support */
#define FEATURE_VOC_TASK

/* DFM SAT
 * Used for better supervisory audio tone.
 * Enable all SAT writes under vocoder control.
*/
#define FEATURE_VOC_SAT_CONTROL

/* Define to enable the QDSP task support.
*/
#define FEATURE_QDSP_TASK

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#if defined(FEATURE_CAMERA) || defined(FEATURE_VIDEO_ENCODE) || \
    defined(FEATURE_MP4_DECODER)
  /* Enable audio support for graphic images
  */
  #define FEATURE_GRAPH_AUDIO

  #if defined(FEATURE_CAMERA) || defined(FEATURE_VIDEO_ENCODE)
    /* Enable IMA-ADPCM audio support for graphic images.
    */
    #define FEATURE_GRAPH_ADPCM
  #endif /* FEATURE_CAMERA || FEATURE_VIDEO_ENCODE */
#endif /* FEATURE_CAMERA || FEATURE_VIDEO_ENCODE || FEATURE_MP4_DECODER */

/* Support for packet clients */
#ifdef FEATURE_VOC_PACKET_INTERFACE

  /* Enable Multimode Voice Services
  */
  #define FEATURE_MVS

#endif /* FEATURE_VOC_PACKET_INTERFACE */


/* Enable feature */

#define FEATURE_AVS_DYNAMIC_CALIBRATION

/*We require two other features need to be enabled*/

#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION

/* This feature is required make ACM accessible from PC over Diag*/

/*Make calibration data non constant (generally these

tables are in ROM)*/

#define AUDIO_CALIBRATION

/* Enable ATP protocol layer */

#define FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL

#endif


/*Homing Sequence feature..*/
#ifdef FEATURE_AMR_VOCODER
#define FEATURE_AMR_REC_HOMING_SEQUENCE
#endif

#ifdef FEATURE_AUDIO_CONFIGURATION_MID_TIER
#define FEATURE_AUDIO_CAMERA_CONCURRENCY
#endif /* FEATURE_AUDIO_CONFIGURATION_MID_TIER */

/*
FEATURE_AVS_QCAM_COMBO_VOC_INTERFACE feature is added specific to ULC for
following issue.
"For Qcamcorder Image vocoder interface changed from legacy to combo vocoder
interface, so DMTF was not working while Qcamcorder recording.
Eventaully this feature needs to be removed when combo vocoder interface"
for QCamcorder image will be available for all targets.
*/

#define FEATURE_AVS_QCAM_COMBO_VOC_INTERFACE

#endif /* CUSTAVS_H */

