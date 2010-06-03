/*=============================================================================
  FILE: MPDefines.h

DESCRIPTION
  This file contains common #define definitions used in multiple header files
  for MediaPlayer.

  SERVICES: 
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPDefines.h#45 $

===========================================================================*/
 
#ifndef MPDEFINES_H
#define MPDEFINES_H

#include "OEMFeatures.h"
#include "AEEMedia.h"
#include "AEEComdef.h"
#include "AEEMediaMPEG4.h"


#ifdef MP_ALLOCATE_HERE
#define EXT
#else
#define EXT extern
#endif

#if (defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_MEDIA_VIDEO))
#define MP_FEATURE_VIDEO  // enable MP video feature
#endif //(defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_MEDIA_VIDEO))

#ifdef FEATURE_ICONTENT
//#define MP_FEATURE_ICONTENT
#endif //FEATURE_ICONTENT

#ifdef FEATURE_ICONTENTHANDLER
#define MP_FEATURE_ICONTENTHANDLER
#endif //FEATURE_ICONTENTHANDLER

#ifdef FEATURE_MBMS_DEMO
#define MP_FEATURE_MBMS
#endif

// these are temporary defines for recording. They should be removed when setting
// "Sample Rate" and "Channel Mode" features are supported by Brew.
// note, when brew supporting above, add back IDB_SAMPLE_RATE and IDB_CHANNEL in CRecorderSettingWin_New
typedef enum {  
  CMX_AF_AAC_CHANNEL_UNKNOWN = 0,
  CMX_AF_AAC_CHANNEL_MONO,       /* Single channel (mono) data                */
  CMX_AF_AAC_CHANNEL_DUAL,       /* Stereo data                               */
  CMX_AF_AAC_CHANNEL_TRIPLE,     /* 3 channels: 1+2 (UNSUPPORTED)             */
  CMX_AF_AAC_CHANNEL_QUAD,       /* 4 channels: 1+2+1 (UNSUPPORTED)           */
  CMX_AF_AAC_CHANNEL_QUINTUPLE,  /* 5 channels: 1+2+2 (UNSUPPORTED)           */
  CMX_AF_AAC_CHANNEL_SEXTUPLE,   /* 5+1 channels: 1+2+2+1 (UNSUPPORTED)       */
  CMX_AF_AAC_CHANNEL_OCTUPLE     /* 7+1 channels: 1+2+2+2+1 (UNSUPPORTED)     */
} AUDIO_CHANNEL_TYPE;

typedef enum {
  CMX_AF_SAMPLE_RATE_8000     = 1,                                /* 8k      */
  CMX_AF_MP3_SAMPLE_RATE_8000 = CMX_AF_SAMPLE_RATE_8000,
#if defined(FEATURE_STEREO_DAC)
  CMX_AF_SAMPLE_RATE_11025,                                       /* 11.025k */
  CMX_AF_MP3_SAMPLE_RATE_11025 = CMX_AF_SAMPLE_RATE_11025,
  CMX_AF_SAMPLE_RATE_12000,                                       /* 12k     */
  CMX_AF_MP3_SAMPLE_RATE_12000 = CMX_AF_SAMPLE_RATE_12000,

  CMX_AF_SAMPLE_RATE_16000,                                       /* 16k     */
  CMX_AF_MP3_SAMPLE_RATE_16000 = CMX_AF_SAMPLE_RATE_16000,
  CMX_AF_SAMPLE_RATE_22050,                                       /* 22.050k */
  CMX_AF_MP3_SAMPLE_RATE_22050 = CMX_AF_SAMPLE_RATE_22050,
  CMX_AF_SAMPLE_RATE_24000,                                       /* 24k     */
  CMX_AF_MP3_SAMPLE_RATE_24000 = CMX_AF_SAMPLE_RATE_24000,
 
  CMX_AF_SAMPLE_RATE_32000,                                       /* 32k     */
  CMX_AF_MP3_SAMPLE_RATE_32000 = CMX_AF_SAMPLE_RATE_32000,
  CMX_AF_SAMPLE_RATE_44100,                                       /* 44.1k   */
  CMX_AF_MP3_SAMPLE_RATE_44100 = CMX_AF_SAMPLE_RATE_44100,
  CMX_AF_SAMPLE_RATE_48000,                                       /* 48k     */
  CMX_AF_MP3_SAMPLE_RATE_48000 = CMX_AF_SAMPLE_RATE_48000,

#ifdef FEATURE_AAC_REC
  CMX_AF_SAMPLE_RATE_64000,                                       /* 64k     */
  CMX_AF_SAMPLE_RATE_88200,                                       /* 88.2k   */
  CMX_AF_SAMPLE_RATE_96000,                                       /* 96k     */
#endif /* FEATURE_AAC_REC        */

#endif /* FEATURE_STEREO_DAC */

  CMX_AF_SAMPLE_RATE_UNKNOWN,                                /* Unknown rate */
  CMX_AF_MP3_SAMPLE_RATE_UNKNOWN = CMX_AF_SAMPLE_RATE_UNKNOWN
} AUDIO_SAMPLE_RATE;

#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO)) 
#define MP_FEATURE_FAST_PLAYBACK

  // temporary define here for QTV replay speed, these should be removed once it is in AEEMediaMPEG4.h
  enum ePlaybackSpeed
  {
    QTV_PLAYBACK_SPEED_NORMAL  /* Normal speed (default) */
    ,QTV_PLAYBACK_SPEED_1P3X   /* 1.3x speed */
  };

#endif //#if defined((FAST_PLAYBACK_VIDEO_ONLY) || defined(FAST_PLAYBACK_AUDIO)) 

#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB)
#define MP_FEATURE_PLAY_LIVE
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB)


#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
// temporary define here for QTV replay speed, these should be removed once it is in AEEMediaMPEG4.h
enum eDualMonoOutput
{
  DUAL_MONO_MAIN_TO_L_AND_R
  ,DUAL_MONO_SUB_TO_L_AND_R
  ,DUAL_MONO_MAIN_TO_L_SUB_TO_R
  ,DUAL_MONO_SUB_TO_L_MAIN_TO_R
};
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION


/* The following define is for new data structures, header files or external */
/* variables that are used in MediaPlayer, but are not available in older    */
/* builds.  C0mment out this define to build MediaPlayer on older builds.    */
#define MP_USE_NEWER_BUILD_ITEMS

// feature defines
#if (defined(FEATURE_AUDIO_EQUALIZER)||defined(FEATURE_AUDIO_QCONCERT)||defined(FEATURE_AUDIO_SPECTRUM_ANALYZER))
#define FEATURE_MP_EQ

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif //FEATURE_AUDIO_QENSEMBLE

#endif

// Setting Win UI
#define FEATURE_MP_EQ_UI

#define FEATURE_KEY_HOLD_SCROLL
#define FEATURE_MP_RECORDER_SETTINGS
#define FEATURE_MP_RESOURCE_MERGE
#define FEATURE_MP_KEY_MAPPING
#define FEATURE_MP_HELP_SCREEN
#define FEATURE_MP_SEEK_TIME_DLG


#define MP_EVENT (MM_CMD_USER_BASE + 100)
#define MP_EVENT_ERROR     (MP_EVENT + 1)
#define MP_EVENT_REDRAW_CTRL (MP_EVENT + 2)

/*
 * Based on Menu style sheet:
 */
#define MENU8_FT                 AEE_FT_NONE
#define MENU8_SELECT_FT          AEE_FT_RAISED
#define MENU8_RO                 AEE_RO_TRANSPARENT
#define MENU8_SELECT_RO          AEE_RO_TRANSPARENT
#define MENU8_COLOR_MASK        (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT)
#define MENU8_BACKGROUND        MAKE_RGB(255,255,204)
#define MENU8_SELECT_BACKGROUND MAKE_RGB(153, 204, 204)
#define MENU8_SELECT_TEXT       RGB_BLACK

#define TB8_BACKGROUND           MAKE_RGB(192,192,192)
#define TB8_SELECT_BACKGROUND    MAKE_RGB(192, 192, 192)

// Always set the max path length to 512, regardless of BREW Versions in use.
// We use IFILE_EnumNextEx, which does not have any length limits.
// This will enable MediaPlayer to read long file path of media files imported
// from external sources.
#define MP_MAX_FILE_NAME        512

#define MP_DETERMINE_VOL_LEVELS

// Includes event reporting for EVENT_MEDIA_PLAYER_KEYPRESS
#define MP_FEATURE_KEYPRESS_EVENT 

#define MP_GETTOTALTIME_RETRY_MAX 100

// This define should not be defined here, but from the build command line if it is necessary to pick up the
// tip of MediaPlayer but not the newest Video-OEM layer that defines AEEMediaUtil_CreatePseudoMediaEx
// If this is define we will use the old AEEMediaUtil_CreatePseudoMedia
//#define MP_USE_CREATEPSEUDOMEDIA

// May need to comment out MP_FEATURE_MPEG4_TEXT define in order to compile with older builds.
#define MP_FEATURE_MPEG4_TEXT 

#ifdef FEATURE_MMOVERLAY
// Error return value from IMMOverlay_UpdateAsync().
// Not defined in AEEMMOverlay.h, so define it here.
#define MP_ERR_IMMOVERLAY    0xFFFFFFFF
#endif // FEATURE_MMOVERLAY

#if (defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_DIVX_DRM))
#define MP_FEATURE_QTV_DIVX_DRM
#endif //#if (defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_DIVX_DRM))

// May need to comment out MP_FEATURE_ICONTENT_PROG_DL in older builds
#define MP_FEATURE_ICONTENT_PROG_DL

// In older builds the AEETrackList structure does not contain the language field
// enable this define to use code that handles the old structure format.
// #define MP_USE_OLD_AEETRACKLIST

// There is a condition that in the LTK #include OEMefs.h does not work if it
// is not protected by #ifdef FEATURE_APP_MEDIA_VIDEO.  However, we need to
// include the same header if flag FEATURE_APP_MEDIA_VIDEO is not defined.
// This flag should only be defined in the LTK builds
//#define MP_LTK_OEMEFS_CONDITIONAL

#endif //MPDEFINES_H
