#ifndef CUSTOEMSBLAUDIO_H
#define CUSTOEMSBLAUDIO_H
/*===========================================================================

DESCRIPTION
  Configuration for OEMSBL Audio 

  Copyright (c) 2002-2006 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/custoemsblaudio.h#1 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/08    sj     Added to AVS mainline code
08/11/06    sp     Initial version. All audio features that are defined in
                   any of the custfiles or build file and not required for
                   OEMSBL Audio must be undefined here. This cust file should
                   be the last cust file to be included in the custxxxx.h file.
===========================================================================*/

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE

  #ifdef MSG_FATAL
    #undef MSG_FATAL
  #endif

  #define MSG_FATAL(x,a,b,c) OEMSBL_ERR_FATAL()

  #define MSG_LVL_LOW ((0x00000001) |(0x00000002))

  #ifdef ERR_FATAL
    #undef ERR_FATAL
  #endif

  #define ERR_FATAL(x,a,b,c) OEMSBL_ERR_FATAL()

  #ifdef MSG_SPRINTF_FMT_VAR_3
    #undef MSG_SPRINTF_FMT_VAR_3
  #endif

  #define MSG_SPRINTF_FMT_VAR_3(x,a,b,c,d,e) 



  /* CMX related */

  /*********************************************/
  /* New undefs added by Subhash */
  #undef FEATURE_AUDIO_PCR_SUPPORT
  #undef PNG_USES_LIBSTD 
  #undef GRAPH_USES_LIBSTD 
  #undef FEATURE_AUDIO_ISDBT_DUAL_MONO
  #undef FEATURE_CMX_SCREEN_DIMENSIONS 
  #undef FEATURE_AUDIO_QENSEMBLE 
  #undef FEATURE_AUDFMT_QCONCERT_PLUS
  #undef FEATURE_AVS_CODEC_CLK_SRC_MGMT
  #undef FEATUE_AUDFMT_EVB
  #undef FEATURE_AAC_FORMAT_LOAS
  #undef FEATURE_AMR_WB_3GP_RAW_FRAMES
  #undef FEATURE_AVS_BSAC
  #undef FEATURE_AMR_WB_AUDIO_DEC
//  #undef FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE
  #undef FEATURE_AMR_WBPLUS_AUDIO_DEC 
  #undef FEATURE_AVS_INCALL_ADEC_PB 
//  #undef FEATURE_AAC_PLUS 
  #undef FEATURE_MIDI_KEYBOARD
//  #undef FEATURE_STEREO_DAC 
  #undef FEATURE_NEW_SYNTH
  /*********************************************/

  #undef FEATURE_IMELODY
  #undef FEATURE_MULTISEQUENCER
  #undef FEATURE_GASYNTH_INTERFACE
  #undef FEATURE_QAUDIOFX
  #undef FEATURE_WEBAUDIO
  #undef FEATURE_SMAF_DSP_WAVEFORM_UNPACKING
  #undef FEATURE_STEREO_YADPCM
  #undef FEATURE_AUDIO_POSTCARD
  #undef FEATURE_ENABLE_SVG_SVGDOM
  #undef FEATURE_XMF
  #undef FEATURE_DLS
  #undef FEATURE_AUDFMT_SMAF
  #undef FEATURE_SMAF
  #undef FEATURE_ANIMATED_AAC
  #undef FEATURE_AUDFMT_EQUALIZER
  #undef FEATURE_AUDFMT_QCONCERT
  #undef FEATURE_AUDFMT_SPECTRUM_ANALYZER
  #undef FEATURE_AUDFMT_IIR_FILTER
  #undef FEATURE_MULTIPLE_ADEC_CHANNELS
  #undef FEATURE_MIDI
  #undef FEATURE_QCP
  #undef FEATURE_QSYNTH
  #undef FEATURE_QSYNTH_ADRC
  #undef FEATURE_QSYNTH_ADPCM
  #undef FEATURE_ADPCM
  #undef FEATURE_MIDI_OUT
  #undef FEATURE_PCM_REC
  #undef FEATURE_PCM
  #undef FEATURE_AUDFMT_MULTISEQUENCER
  #undef FEATURE_PNG_ENCODER
  #undef FEATURE_JPEG_ENCODER
  #undef FEATURE_PNG_DECODER
  #undef FEATURE_JPEG_DECODER
  #undef FEATURE_SAF
  #undef FEATURE_CMX
  #undef FEATURE_CMX_IMAGE
  #undef FEATURE_MIDI
  #undef FEATURE_MIDI_OUT
  #undef FEATURE_MIDI_OUT_QCP
  #undef FEATURE_PCM
  #undef FEATURE_QCP
  #undef FEATURE_QSYNTH_PCM
  #undef FEATURE_QSYNTH_COMMON
  #undef FEATURE_WAVE_PB
  #undef FEATURE_ADEC
  #undef FEATURE_MIDI
  #undef FEATURE_VOICE_PLAYBACK
  #undef FEATURE_VOICE_RECORD
  #undef FEATURE_AUDIO_FORMAT
  #undef FEATURE_PNG_DECODER
  #undef FEATURE_MLZ_DECODER
  #undef FEATURE_GRAPH_TASK
  #undef FEATURE_MULTIPLE_RINGER_TYPES
  #undef FEATURE_SND_RING_SET_1
  #undef FEATURE_SND_RING_SET_2
  #undef FEATURE_CMX
  #undef FEATURE_VS_TASK
  #undef FEATURE_AUDFMT_HYBRID_MODE
  #undef FEATURE_MULTIMEDIA
  #undef FEATURE_EXTERNAL_SADC

  /* AVS related */

  /*********************************************/
  /* New undefs added by Subhash */

  #undef FEATURE_VOC_PCM_INTERFACE

  /* Enable snd test block and PCM interface test */
 #undef  SND_TEST_CODE
 #undef  FEATURE_VOC_PCM_INTERFACE_TEST

  /* Enable PCM Logging*/
  #undef FEATURE_PCM_PKT_LOG
  #undef FEATURE_IIR_FILTER  
  #undef FEATURE_IIR_FILTER_5S
  #undef FEATURE_AVS_CRYSTAL_SPEECH
  #undef FEATURE_AVS_VOC_DTMF_MIXING
  #undef FEATURE_MULTIMEDIA_CODEC_GAIN
  #undef FEATURE_VOC_DTMF_DETECT
  #undef FEATURE_HEADSET_SWITCH
  #undef FEATURE_AVS_IN_CALL_AAC
  #undef FEATURE_AVS_IN_CALL_WMA
  #undef FEATURE_AVS_INCALL_MIXER_CMX
  #undef FEATURE_AVS_CODEC_CLK_SRC_MGMT
  #undef FEATURE_VOC_AMR_WB 
  #undef FEATURE_VOC_PACKET_INTERFACE
  #undef QDSP_PACKET_SNIFFER 
  #undef FEATURE_GSM_WCDMA_AMR_PKT_LOG
  #undef FEATURE_AMR_REC_HOMING_SEQUENCE
  #undef FEATURE_AVS_DYNAMIC_CALIBRATION
  #undef FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL
  /*********************************************/
  
  #undef FEATURE_AVS_NEXTGEN_EC
  #undef FEATURE_VOC_ADPCM
  #undef FEATURE_NEW_SOUND_API
  #undef FEATURE_AUDIO_AGC
  #undef FEATURE_ANALOG_HFK
  #undef FEATURE_TTY_UMTS
  #undef FEATURE_VOC_PCM_INTERFACE
  #undef FEATURE_TTY
  #undef FEATURE_TTY_CDMA
  #undef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  #undef FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
  #undef FEATURE_QDSP_MODULE_RESOURCE_MGMT
  #undef FEATURE_QDSP_SET_IMAGE_HW_RESOURCES
  #undef FEATURE_AMR_VOCODER
  #undef FEATURE_MP4_AMR
  #undef FEATURE_AUDFMT_AMR
  #undef FEATURE_GSM
  #undef FEATURE_GSM_EFR
  #undef MSMAUD_VOC_AMR
  #undef MSMAUD_VOC_AMR_COMMON
  #undef MSMAUD_VOC_FR
  #undef MSMAUD_VOC_EFR
  #undef MSMAUD_VOC_HR
  #undef FEATURE_MVS
  #undef FEATURE_NO_TX_COMFORT_NOISE
  #undef FEATURE_GRAPH_AUDIO
  #undef FEATURE_GRAPH_ADPCM
  #undef FEATURE_AVS_INCALL_MIXER_ADEC_PB

  /* BT related */
  #undef FEATURE_BT
  #undef FEATURE_SBC_DSP_CODEC
  #undef FEATURE_AVS_A2DP_AVSYNC
  #undef FEATURE_BT_AG
  #undef FEATURE_AVS_BT_SCO_REWORK
  #undef FEATURE_AVS_I2SSBC
  #undef FEATURE_AVS_AUDIO_DECODER_SCO
  #undef FEATURE_ARM_POST_PROCESSING

  /* Audio decoders */
  #undef FEATURE_MP3
  #undef FEATURE_REAL_AUDIO
  #undef FEATURE_WMA
  #undef FEATURE_AAC
  #undef FEATURE_ENHANCED_AAC_PLUS
  #undef FEATURE_QTUNES
  #undef FEATURE_QTUNES_COMMON
  #undef FEATURE_MPEG_AUDIO
  #undef FEATURE_QTUNES_AAC
  #undef FEATURE_MPEG_AUDIO
  #undef FEATURE_ADEC

  /* Audio Encoders */
  #undef FEATURE_AAC_REC
  #undef FEATURE_MM_REC

#endif

#endif /* CUSTOEMSBLAUDIO_H */
