#ifndef CUSTCMX_H
#define CUSTCMX_H
/*===========================================================================

      C M X   S U B S Y S T E M   B U I L D   C O N F I G U R A T I O N
                        H E A D E R   F I L E

  This file controls the presence or absence of features built into AMSS
  audio code. 
  
  Note that some audio-centric feature macros may not be controlled via
  this header file. There are four possibilities why an audio-centric
  feature may not be controlled here:

    1. The feature may be a subset of a non-audio feature, such as video
       systems, and/or only available under a non-audio license agreement.

    2. The feature is specific to the internal workings of the audio code
       and is not meant as a controllable feature.

    3. The feature is not available for the purchased chipset, but due to
       the use of shared code among chipset products, the feature name may
       appear in code.

    4. The code protected by the feature macro is in development and
       incomplete or non-functional.

  ** IMPORTANT ** IT IS STRONGLY ADVISED THAT DEVELOPERS NOT ATTEMPT TO
  ENABLE OR DISABLE ANY FEATURES WITHOUT CONSULTING YOUR QUALCOMM SUPPORT
  CONTACT. DOING SO MAY ENABLE INCOMPLETE OR NON-FUNCTIONAL CODE, MAY CREATE
  LINK RESOLUTION ERRORS WITH LIBRARIES, OR MAY VIOLATE THE TERMS OF YOUR
  LICENSE AGREEMENT.

  Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/build/cust/main/latest/inc/custcmx.h#11 $ $DateTime: 2009/04/14 01:10:01 $ $Author: skray $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/09   skr     enabled FEATURE_QTUNES_ADRC
02/09/09    bk     Added Feature PNG_USES_LIBSTD under FEATURE_RNG_DECODER
                   to remove Banned API.
01/19/09    bk     Added support for the feature FEATURE_STD_MIDI.
10/17/08    ry     Reorganised features required for mid and high tier.
10/13/08    ry     Added a check on FEATURE_AUDIO_CONFIGURATION_MID_TIER macro
                   to undef some features for Mid tier.
08/14/08   vsud    Enabled Qconcert+ feature.
06/31/08   vsud    Removed the Mp3 and EVB recording from Low Tier build.
11/05/07   anb     Enabled FEATURE_AUDFMT_EVB
10/17/07   anb     Enabled the FEATURE_QAUDIOFX.
09/14/07   anb     Modified to enable FEATURE_AUDFMT_QCONCERT if
                   FEATURE_QCONCERT is enabled.
06/21/07   anb     Modified to enable FEATURE_AUDIO_QSYNTH_48K_SUPPORT if
                   synth is enabled.
04/30/07   anb     Enabled FEATURE_AMR_IF1
04/20/07   anb     Enabled FEATURE_SAF,FEATURE_AAC_FORMAT_LOAS
                   (Fixes 115054,113847) 
02/28/07   sud     Enabled FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE.
                   Removed FEATURE_QAUDIOFX and FEATURE_SAF.
06/12/06    sg     Defined FEATURE_XMF and FEATURE_DLS for VA builds,
                   undefined FEATURE_CMX for MINIMAL config. Build file
                   related cleanup: Moved some feature definitions to this file.
06/09/06    sg     Define MULTITMEDIA only if MIDI or some true MM format like
                   AAC is enabled.
05/24/06    sg     Moved FEATURE_PCM to standard configuration as a temporary
                   workaround that will allow us to enable G711 and disable
                   PCM playback.
                   Enable FEATURE_QSYNTH_PCM only if FEATURE_QSYNTH is enabled.
04/28/06   act     Initial revision.

===========================================================================*/


/*-------------------------------------------------------------------------*\
                         C O N V E N T I O N S

  As shipped, the feature macro definitions contained in this file are
  categorized into groups representing the common build
  configurations QUALCOMM releases with each chipset AMSS release. This was
  done to simplify the number of feature macros being defined by the
  compiler command-line and to provide a "one-stop-shop" to control the
  audio build configuration.

  FEATURE_AUDIO_CONFIGURATION_VALUE_ADD represents the "value-add" licensing
  featureset.

  FEATURE_AUDIO_CONFIGURATION_STANDARD represents the standard AMSS release
  featureset.

  FEATURE_AUDIO_CONFIGURATION_MINIMAL represents the minimal speech-only
  AMSS featureset.

\*-------------------------------------------------------------------------*/




/*-------------------------------------------------------------------------*\
                             V A L U E - A D D

  These features are part of the value-add configuration.
\*-------------------------------------------------------------------------*/
#ifdef FEATURE_AUDIO_CONFIGURATION_VALUE_ADD
  #ifndef FEATURE_AUDIO_CONFIGURATION_STANDARD
    #define FEATURE_AUDIO_CONFIGURATION_STANDARD
  #endif /* !FEATURE_AUDIO_CONFIGURATION_STANDARD */
  #ifndef FEATURE_AUDIO_CONFIGURATION_MID_TIER
    #define FEATURE_AUDIO_CONFIGURATION_MID_TIER
  #endif

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_WEBAUDIO
  [SCOPE]:             ADSP, AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables the superset of functionality within CMX,
                       AUDFMT, and AVS that allows playback of PMD (CMX)
                       files.
  **************************************************************************/
  #define FEATURE_WEBAUDIO

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_ANIMATED_AAC
  [SCOPE]:             
  [DESCRIPTION]:       
  **************************************************************************/
  #define FEATURE_ANIMATED_AAC

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MULTIPLE_ADEC_CHANNELS
  [SCOPE]:             AUDFMT, AVS, CMX
  [DESCRIPTION]:       Enables multi-channel audio decoder support for some
                       multimedia playback DSP firmware.
  **************************************************************************/
  #define FEATURE_MULTIPLE_ADEC_CHANNELS

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDIO_POSTCARD
  [SCOPE]:             BREW, CMX
  [DESCRIPTION]:       Enables software to combine a JPEG image file and
                       a QCP audio file into an Audio Postcard
                       (PMD/CMX format) file.
  **************************************************************************/
  #define FEATURE_AUDIO_POSTCARD

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_SMAF
  [SCOPE]:             AUDFMT, AVS, CMX, BREW
  [DESCRIPTION]:       Enables SMAF file playback.
  **************************************************************************/
  #define FEATURE_SMAF

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MULTISEQUENCER
  [SCOPE]:             BREW
  [DESCRIPTION]:       Enables multiple MIDI sequence playback support for some
                       multimedia playback DSP firmware.
  **************************************************************************/
  #define FEATURE_MULTISEQUENCER

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_XMF
  [SCOPE]:             
  [DESCRIPTION]:       Enables the XMF file playback
  **************************************************************************/
  #define FEATURE_XMF

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_DLS
  [SCOPE]:             
  [DESCRIPTION]:       Enables the MIDI DLS support
  **************************************************************************/
  #define FEATURE_DLS

#endif /* FEATURE_AUDIO_CONFIGURATION_VALUE_ADD */


/*-------------------------------------------------------------------------*\
  These features are required for Mid tier as well as high tier.

\*-------------------------------------------------------------------------*/
#ifdef FEATURE_AUDIO_CONFIGURATION_MID_TIER
  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AAC
  [SCOPE]:             
  [DESCRIPTION]:                     
  **************************************************************************/
  #define FEATURE_AAC

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDFMT_EQUALIZER
  [SCOPE]:             AVS
  [DESCRIPTION]:       Enables the audio equalizer post-processing block for
                       multimedia file playback.
  **************************************************************************/
  #define FEATURE_AUDFMT_EQUALIZER

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDFMT_SPECTRUM_ANALYZER
  [SCOPE]:             AVS
  [DESCRIPTION]:       Enables the spectrum analyzer post-processing block
                       for multimedia file playback.
  **************************************************************************/
  #define FEATURE_AUDFMT_SPECTRUM_ANALYZER

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDFMT_IIR_FILTER
  [SCOPE]:             AVS
  [DESCRIPTION]:       Enables the IIR filters post-processing block
                       for multimedia file playback.
  **************************************************************************/
  #define FEATURE_AUDFMT_IIR_FILTER

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QCONCERT
  [SCOPE]:             
  [DESCRIPTION]:       Enables the game-audio interface.
  **************************************************************************/
  #define FEATURE_QCONCERT
  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QAUDIOFX
  [SCOPE]:             
  [DESCRIPTION]:       Enables the game-audio interface.
  **************************************************************************/
  //#define FEATURE_QAUDIOFX / for ULC bringup 

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_IMELODY
  [SCOPE]:             
  [DESCRIPTION]:       
  **************************************************************************/
  #define FEATURE_IMELODY
#endif

/*-------------------------------------------------------------------------*\
                              S T A N D A R D

  These features are part of the standard configuration.
\*-------------------------------------------------------------------------*/
#ifdef FEATURE_AUDIO_CONFIGURATION_STANDARD
  #ifndef FEATURE_AUDIO_CONFIGURATION_MINIMAL
    #define FEATURE_AUDIO_CONFIGURATION_MINIMAL
  #endif /* !FEATURE_AUDIO_CONFIGURATION_MINIMAL */


  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MIDI
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables basic MIDI file playback.
  **************************************************************************/
  #define FEATURE_MIDI

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QCP
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables QCP file playback.
  **************************************************************************/
  #define FEATURE_QCP

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QSYNTH
  [SCOPE]:             ADSP, AUDFMT, AVS, CMX
  [DESCRIPTION]:       Enables the driver code that controls the DSP-based
                       synthesizer for MIDI.
  **************************************************************************/
  #define FEATURE_QSYNTH

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QSYNTH_ADRC
  [SCOPE]:             AVS
  [DESCRIPTION]:       Enables dynamic range control post-processing block on 
                       the MIDI synthesizer.
  **************************************************************************/
  #define FEATURE_QSYNTH_ADRC

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QTUNES_ADRC
  [SCOPE]:             AVS
  [DESCRIPTION]:       Enables dynamic range control post-processing block on 
                       QTUNES playback.
  **************************************************************************/
  #define FEATURE_QTUNES_ADRC

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QSYNTH_ADPCM
  [SCOPE]:             AUDFMT, AVS, CMX
  [DESCRIPTION]:       Enables ADPCM playback driver code on the MIDI
                       synthesizer.
  **************************************************************************/
  #define FEATURE_QSYNTH_ADPCM

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_ADPCM
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables ADPCM WAV file playback.
  **************************************************************************/
  #define FEATURE_ADPCM

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MIDI_OUT
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables CMX API functions and lower layer functionality
                       that allows applications to send MIDI commands directly
                       to the MIDI synthesizer.
  **************************************************************************/
  #define FEATURE_MIDI_OUT

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_PCM_REC
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables PCM recording functionality.
  **************************************************************************/
  #define FEATURE_PCM_REC

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDFMT_EVB
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables .EVB file playback.
  **************************************************************************/
  #define FEATURE_AUDFMT_EVB

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MP3
  [SCOPE]:             ADSP, AUDFMT, AVS, BREW, CMX, QTV
  [DESCRIPTION]:       Enables MP3 file playback.
  **************************************************************************/
  #define FEATURE_MP3

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_PCM
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables WAVE file playback.
  **************************************************************************/
  #define FEATURE_PCM

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables file playback with Fractional Sampling Frequency
  **************************************************************************/
  #define FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE
  /**************************************************************************
  [FEATURE NAME]:      GRAPH_USES_LIBSTD
  [SCOPE]:             GRAPH
  [DESCRIPTION]:       This uses different API in graph.c, avoid compilation
                       warnings.
  **************************************************************************/
  #define GRAPH_USES_LIBSTD 

  #ifdef FEATURE_AUDIO_CONFIGURATION_LO_TIER
   /**************************************************************************
  [FEATURE NAME]:      FEATURE_AUDFMT_EVB
  [SCOPE]:             AUDFMT, AVS, BREW, CMX
  [DESCRIPTION]:       Enables .EVB file playback.
  **************************************************************************/
  #undef FEATURE_AUDFMT_EVB

  /**************************************************************************
  [FEATURE NAME]:      FEATURE_MP3
  [SCOPE]:             ADSP, AUDFMT, AVS, BREW, CMX, QTV
  [DESCRIPTION]:       Enables MP3 file playback.
  **************************************************************************/
  #undef FEATURE_MP3

  #endif

#ifdef FEATURE_STD_MIDI

#ifdef FEATURE_AUDIO_CONFIGURATION_STANDARD



#undef FEATURE_AUDIO_QSYNTH_48K_SUPPORT

#undef FEATURE_MIDI_OUT

#ifdef FEATURE_AUDFMT_HYBRID_MODE
    #undef FEATURE_AUDFMT_HYBRID_MODE
#endif

#define FEATURE_AUDFMT_IIR_FILTER  
#define FEATURE_GASYNTH_INTERFACE
#ifndef FEATURE_MULTIPLE_ADEC_CHANNELS
   #define FEATURE_MULTIPLE_ADEC_CHANNELS
#endif /*FEATURE_MULTIPLE_ADEC_CHANNELS*/

#endif /*FEATURE_AUDIO_CONFIGURATION_STANDARD*/

#ifdef FEATURE_AUDIO_CONFIGURATION_MID_TIER



  #undef FEATURE_AUDFMT_EQUALIZER

  #undef FEATURE_AUDFMT_SPECTRUM_ANALYZER

  #undef FEATURE_QCONCERT

  #undef FEATURE_IMELODY

#endif /*FEATURE_AUDIO_CONFIGURATION_MID_TIER*/

#ifdef FEATURE_AUDIO_CONFIGURATION_VALUE_ADD


  #undef FEATURE_WEBAUDIO

  #undef FEATURE_ANIMATED_AAC

  #undef FEATURE_AUDIO_POSTCARD

  #undef FEATURE_SMAF

  #undef FEATURE_MULTISEQUENCER

  #undef FEATURE_XMF

  #undef FEATURE_DLS

#endif  /*FEATURE_AUDIO_CONFIGURATION_MID_TIER*/

#endif/*FEATURE_STD_MIDI*/

#endif /* FEATURE_AUDIO_CONFIGURATION_STANDARD */




/*-------------------------------------------------------------------------*\
                               M I N I M A L

  These features are part of the minimal configuration.
\*-------------------------------------------------------------------------*/
#ifdef FEATURE_AUDIO_CONFIGURATION_MINIMAL

#else /* FEATURE_AUDIO_CONFIGURATION_MINIMAL */
#error You must define one of: FEATURE_AUDIO_CONFIGURATION_MINIMAL, \
  FEATURE_AUDIO_CONFIGURATION_STANDARD, or                         \
  FEATURE_AUDIO_CONFIGURATION_VALUE_ADD.
#endif /* FEATURE_AUDIO_CONFIGURATION_MINIMAL */







/*--------------- DO NOT CHANGE CODE BELOW THIS LINE ----------------------*/
/****************************************************************************
*****************************************************************************
*****************************************************************************
*****************************************************************************
*****************************************************************************
*****************************************************************************
****************************************************************************/

/*---------------------------------------------------------------------------
                          Feature Dependencies
---------------------------------------------------------------------------*/

#ifdef FEATURE_AUDFMT_EVB
#ifndef FEATURE_QCP
#define FEATURE_QCP
#endif
#endif


#ifdef FEATURE_MULTISEQUENCER
  #ifndef FEATURE_AUDFMT_MULTISEQUENCER
    #define FEATURE_AUDFMT_MULTISEQUENCER
  #endif
#endif

#ifdef FEATURE_ANIMATED_AAC
  #ifndef FEATURE_WEBAUDIO
    #error "FEATURE_ANIMATED_AAC requires licensed FEATURE_WEBAUDIO"
  #endif
  #ifndef FEATURE_AAC
    #error "FEATURE_ANIMATED_AAC requires licensed FEATURE_AAC"
  #endif
#endif

#ifdef FEATURE_AUDIO_POSTCARD
  #ifndef FEATURE_WEBAUDIO
    #error "FEATURE_AUDIO_POSTCARD requires licensed FEATURE_WEBAUDIO"
  #endif
  #ifndef FEATURE_PNG_ENCODER
    #define FEATURE_PNG_ENCODER /* POSSIBLE */ 
  #endif
  
  //#ifndef FEATURE_JPEG_ENCODER
  // #define FEATURE_JPEG_ENCODER /* POSSIBLE */ 
  //#endif
  
#endif

#ifdef FEATURE_WEBAUDIO
  #ifndef FEATURE_PNG_DECODER
    #define FEATURE_PNG_DECODER
  #endif
  
  //#ifndef FEATURE_JPEG_DECODER
  //  #define FEATURE_JPEG_DECODER
  //#endif
  
  #ifndef FEATURE_SAF
    #define FEATURE_SAF
  #endif
  #ifndef FEATURE_MIDI
    #define FEATURE_MIDI
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
  #ifndef FEATURE_PCM
    #define FEATURE_PCM
  #endif
  #ifndef FEATURE_QCP
    #define FEATURE_QCP
  #endif
#endif

#ifdef FEATURE_ADPCM
  #ifndef FEATURE_PCM
    #define FEATURE_PCM
  #endif
#endif

#ifdef FEATURE_PNG_DECODER
  #ifndef PNG_USES_LIBSTD
     #define PNG_USES_LIBSTD
  #endif /*PNG_USES_LIBSTD*/
  #endif /* FEATURE_PNG_DECODER*/

#ifndef FEATURE_STD_MIDI
#if defined(FEATURE_PCM) && defined(FEATURE_QSYNTH)
  #ifndef FEATURE_QSYNTH_PCM
    #define FEATURE_QSYNTH_PCM
  #endif
#endif
#endif /*FEATURE_STD_MIDI*/

#ifdef FEATURE_AAC_PLUS
  #ifndef FEATURE_AAC
    #error "FEATURE_AAC_PLUS requires licensed FEATURE_AAC"
  #endif
#endif

#ifdef FEATURE_PCM_REC
  #ifndef FEATURE_VOC_PCM_INTERFACE
    #define FEATURE_VOC_PCM_INTERFACE
  #endif
#endif

#ifndef FEATURE_STD_MIDI
#ifdef FEATURE_QSYNTH
  #ifndef FEATURE_AUDIO_QSYNTH_48K_SUPPORT
    #define FEATURE_AUDIO_QSYNTH_48K_SUPPORT
  #endif
#endif
#endif /*FEATURE_STD_MIDI*/

#ifdef FEATURE_QSYNTH_PCM
  #ifndef FEATURE_GASYNTH_INTERFACE
    #define FEATURE_GASYNTH_INTERFACE
  #endif
  #ifndef FEATURE_QSYNTH_ADPCM
    #define FEATURE_QSYNTH_ADPCM
  #endif
#endif

#if defined(FEATURE_ADPCM) || defined(FEATURE_PCM)
  #ifndef FEATURE_WAVE_PB
    #define FEATURE_WAVE_PB
  #endif
#endif

#ifdef FEATURE_QSYNTH_ADRC
  #ifndef FEATURE_QSYNTH_COMMON
    #define FEATURE_QSYNTH_COMMON
  #endif
#endif

#ifdef FEATURE_QSYNTH_ADPCM
  #ifndef FEATURE_QSYNTH_COMMON
    #define FEATURE_QSYNTH_COMMON
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
#endif

#ifdef FEATURE_QAUDIOFX
  #ifndef FEATURE_MIDI_OUT
    #define FEATURE_MIDI_OUT
  #endif
#endif

#ifdef FEATURE_MIDI_OUT
  #ifndef FEATURE_MIDI_OUT_QCP
    #define FEATURE_MIDI_OUT_QCP
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
  #ifndef FEATURE_MIDI
    #define FEATURE_MIDI
  #endif
#endif

#ifdef FEATURE_MIDI
  #ifndef FEATURE_QSYNTH_COMMON
    #define FEATURE_QSYNTH_COMMON
  #endif
#endif

#ifdef FEATURE_MP3
  #ifndef FEATURE_QTUNES
    #define FEATURE_QTUNES
  #endif
  #ifndef FEATURE_QTUNES_COMMON
    #define FEATURE_QTUNES_COMMON
  #endif
  #ifndef FEATURE_MPEG_AUDIO
    #define FEATURE_MPEG_AUDIO
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
#endif

#ifdef FEATURE_AAC
  #ifndef FEATURE_QTUNES_AAC
    #define FEATURE_QTUNES_AAC
  #endif
  #ifndef FEATURE_QTUNES_COMMON
    #define FEATURE_QTUNES_COMMON
  #endif
  #ifndef FEATURE_MPEG_AUDIO
    #define FEATURE_MPEG_AUDIO
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
  #ifndef FEATURE_AAC_FORMAT_LOAS
   #define FEATURE_AAC_FORMAT_LOAS
  #endif
#endif

#ifdef FEATURE_REAL_AUDIO
  #ifndef FEATURE_QTUNES_COMMON
    #define FEATURE_QTUNES_COMMON
  #endif
  #ifndef FEATURE_ADEC
    #define FEATURE_ADEC
  #endif
#endif

#ifdef FEATURE_QCP
  #ifndef FEATURE_VOICE_PLAYBACK
    #define FEATURE_VOICE_PLAYBACK
  #endif
  #ifndef FEATURE_VOICE_RECORD
    #define FEATURE_VOICE_RECORD
  #endif
#endif

#if defined(FEATURE_MIDI)         || defined(FEATURE_QCP)           || \
    defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_IMELODY)       || \
    defined(FEATURE_WAVE_PB)      || defined(FEATURE_QTUNES_COMMON) || \
    defined(FEATURE_PCM_REC)
  #ifndef FEATURE_AUDIO_FORMAT
    #define FEATURE_AUDIO_FORMAT
  #endif
  #ifndef FEATURE_CMX
    #define FEATURE_CMX
  #endif
#endif

#ifdef FEATURE_SAF
  #ifndef FEATURE_PNG_DECODER
    #define FEATURE_PNG_DECODER
  #endif
  #ifndef FEATURE_MLZ_DECODER
    #define FEATURE_MLZ_DECODER
  #endif
#endif

#if defined(FEATURE_PNG_DECODER)  || defined(FEATURE_SAF) || \
    defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
  #ifndef FEATURE_CMX_IMAGE
    #define FEATURE_CMX_IMAGE
  #endif
#endif

#ifdef FEATURE_CMX_IMAGE
  #ifndef FEATURE_CMX
    #define FEATURE_CMX
  #endif
  #ifndef FEATURE_GRAPH_TASK
    #define FEATURE_GRAPH_TASK
  #endif
#endif

#ifdef FEATURE_MULTIMEDIA
  #ifdef FEATURE_MIDI
    #ifndef FEATURE_MULTIPLE_RINGER_TYPES
      #define FEATURE_MULTIPLE_RINGER_TYPES
    #endif
    #ifndef FEATURE_MULTIPLE_RINGER_TYPES
      #define FEATURE_MULTIPLE_RINGER_TYPES
    #endif
    #ifndef FEATURE_SND_RING_SET_1
      #define FEATURE_SND_RING_SET_1
    #endif
    #ifndef FEATURE_SND_RING_SET_2
      #define FEATURE_SND_RING_SET_2
    #endif
  #endif
  #ifndef FEATURE_CMX
    #define FEATURE_CMX
  #endif
#endif

#ifdef FEATURE_CMX
  #ifndef FEATURE_VS_TASK
    #define FEATURE_VS_TASK
  #endif
#endif

#ifdef FEATURE_SMAF
  #ifndef FEATURE_AUDFMT_SMAF
    #define FEATURE_AUDFMT_SMAF
  #endif
  #ifndef FEATURE_STEREO_YADPCM
    #define FEATURE_STEREO_YADPCM
  #endif
  #ifndef FEATURE_SMAF_DSP_WAVEFORM_UNPACKING
    #define FEATURE_SMAF_DSP_WAVEFORM_UNPACKING
  #endif
#endif

#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_AUDFMT_MULTISEQUENCER)
  #ifndef FEATURE_AUDFMT_HYBRID_MODE
    #define FEATURE_AUDFMT_HYBRID_MODE
  #endif
#endif

#if defined(FEATURE_ADEC) || defined(FEATURE_MIDI)
  #ifndef FEATURE_MULTIMEDIA
    /* This a UI feature that only needs to enabled for formats that use
     * SND_METHOD_MIDI
     */
	  #define FEATURE_MULTIMEDIA
  #endif
#endif
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
  #define FEATURE_AMR_IF1
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */


#ifdef FEATURE_QCONCERT
  #define FEATURE_AUDFMT_QCONCERT_PLUS
#endif

#ifdef FEATURE_AUDFMT_QCONCERT_PLUS
    #ifndef FEATURE_AUDFMT_QCONCERT
        #define FEATURE_AUDFMT_QCONCERT
    #endif
#endif 

#if defined(FEATURE_QCONCERT) || defined(FEATURE_AUDFMT_EQUALIZER) || defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  #define ENABLE_ISNDCTL_EXT
#endif

#endif /* CUSTCMX_H */
