#ifndef VOCCODEC_H
#define VOCCODEC_H
/*===========================================================================

           V O C O D E R   C A L I B R A T I O N   D A T A B A S E
                     C O D E C   I D   H E A D E R

DESCRIPTION
  This header file contains the definition of the codec IDs associated with
  the vocoder calibration database.

Copyright (c) 2005-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/voccodec.h#7 $
  $DateTime: 2009/05/22 09:25:30 $
  $Author: nrchella $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/09   nr	   Added WB calibration tables and new SURF devices (CR#177834,#173500)
05/13/09   rm      EVRC-WB and IIR filter table related updates
11/18/08   knm     Added Macro for HS-USB along with USB car kit
10/22/08   dp      FM over BT AG changes
08/28/07   ymc     Added Crystal Speech support.
           ymc     Updated Copyright.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
03/15/06   ymc     Added FEATURE_INTERNAL_SADC support.
           ymc     Updated Copyright.
03/10/06    sm     Fixed a featurization issue with SDAC
01/30/06    sp     Added support for I2S SBC interface.
12/02/05   act     Re-featurizing stereo-related codecs and sound devices.
09/18/05    sp     Added support for DSP based SBC encoder.
08/26/05    sm     Added voc_codec_mode_type.
08/10/05    sm     Featurized stereo USB support.
07/26/05    sm     Added stereo USB support.
06/03/05    sg     Forced voc_codec_type to use 32bit words.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for external SADC for MultiMedia recording.
04/21/05   act     Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Created.

===========================================================================*/

#include "customer.h"           /* Customer feature specific              */
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.          */

typedef enum {
  VOC_CODEC_DEFAULT,

  VOC_CODEC_ON_CHIP_0 = VOC_CODEC_DEFAULT, /* On Chip Codec Channel 1 */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  VOC_CODEC_ON_CHIP_0_DUAL_MIC,     /* On Chip dual mic stereo channels */
#endif
  VOC_CODEC_ON_CHIP_1,            /* On Chip Codec Channel 2          */
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  VOC_CODEC_STEREO_HEADSET,       /* On Chip Codec Channels 1 and 2   */
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
  VOC_CODEC_ON_CHIP_AUX,          /* On Chip Codec Aux. Channel       */
  VOC_CODEC_ON_BOARD,             /* Off Chip, On Board Codec         */
#ifdef FEATURE_SPEAKER_PHONE
  VOC_CODEC_SPEAKER,              /* Speaker Phone                    */
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_BT
  VOC_CODEC_BT_INTERCOM,          /* BT Intercom                      */
#ifdef FEATURE_BT_AG
  VOC_CODEC_BT_AG,                /* BT path                          */
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  VOC_CODEC_BT_A2DP,
  VOC_CODEC_BT_A2DP_SCO,
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
  VOC_CODEC_OFF_BOARD,            /* Aux. Codec                       */
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  VOC_CODEC_SDAC,                 /* Stereo DAC                       */
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
  VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET,    /* Stereo ADC - in MONO mode  */
  VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET,  /* Stereo ADC - in MONO mode  */
  VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET,  /* Stereo ADC - in STEREO mode*/
  VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET,/* Stereo ADC - in STEREO mode*/
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */
#ifdef FEATURE_TTY
  VOC_CODEC_TTY_ON_CHIP_1,        /* TTY On Chip Codec Channel 2      */
  VOC_CODEC_TTY_OFF_BOARD,        /* TTY Aux. Codec                   */
  VOC_CODEC_TTY_VCO,              /* Voice Carry-Over TTY             */
  VOC_CODEC_TTY_HCO,              /* Hearing Carry-Over TTY           */
#endif /* FEATURE_TTY */
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  VOC_CODEC_USB,                  /* USB off-board codec              */
#ifdef FEATURE_INTERNAL_SDAC
  VOC_CODEC_STEREO_USB,           /* Stereo USB off-board codec       */
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
  /* FM over BT AG */
#ifdef FEATURE_BT_AG
  VOC_CODEC_FM_OVER_BT_AG,
#endif
  VOC_CODEC_ON_CHIP_0_SURF,			/* On Chip Codec Channel 1 for SURF */
  VOC_CODEC_ON_CHIP_1_SURF,			/* On Chip Codec Channel 2 for SURF */
#ifdef FEATURE_SPEAKER_PHONE
  VOC_CODEC_SPEAKER_SURF,           /* Speaker Phone for SURF */
#endif /* FEATURE_SPEAKER_PHONE */
  VOC_CODEC_MAX,
  VOC_CODEC_NONE,                 /* Place Holder                     */

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_CODEC_32BIT_DUMMY = 0x7FFFFFFF
} voc_codec_type;

typedef enum {
  VOC_CODEC_MODE_CDMA_VOICE,
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  VOC_CODEC_MODE_CDMA_VOC_ADEC_MIXER,
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
  VOC_CODEC_MODE_MULTIMEDIA,
#endif /* defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) */
#ifdef FEATURE_MM_REC
  VOC_CODEC_MODE_MM_REC,
#endif /* FEATURE_MM_REC */
#ifdef FEATURE_AVS_I2SSBC
  VOC_CODEC_MODE_I2SSBC,
#endif /* FEATURE_AVS_I2SSBC */
  VOC_CODEC_MODE_CDMA_WB_VOICE,
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  VOC_CODEC_MODE_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type */
  VOC_CODEC_MODE_32BIT_DUMMY = 0x7FFFFFFF
} voc_codec_mode_type;
#endif /* VOCCODEC_H */

