/*===========================================================================

FILE: Qcamcorder.c
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camcorder/apps/6k/main/latest/inc/qcamcorder.h#12 $ $DateTime: 2009/04/28 03:30:51 $ $Author: psisodia $

 when         who     what, where, why
 --------  -----  ----------------------------------------------------------
04/28/09   ps     Featurized frequent pause resume calculation for lcd direct mode.
03/04/09   ps     Added nPreviousFrameTimeMs and  bPausePressed to synchronize pause and resume. 
02/20/09   kamit   Dynamic frame overlay support is added. IImage pointer is added to Qcamcorder structure.
01/20/09   girip  Moved filename diplay to the line next to recordtime display by editing the
                  macro defenition QCAMCORDER_NAME_Y. Increased Name width.
11/11/08   ps     Migrated from ICM to ITelephone APIs to check voice call during camcorder
11/03/08   sanal  Workaround for handling slow arrival AEE Callbacks
10/23/08   ps      Removed compiler Superfluous warnings 
09/09/08   ps      Featurization of Timestamp special effects support.
10/07/08   sanal  Added Transcoding error and unsupported states.
07/08/08   shiven Adding support for loci atom
06/30/08   yogi   Removed usage of QCAM_PICTURE_WIDTH/HEIGHT
03/17/08   anil   Added support set udta atom
02/11/08   sanal  Adding Idiag support for frames dropped in a recorded clip
01/29/08   Gopi    Added support for transcoder with movieedit interface
12/27/07   Yogi   Fixed the special effects menu and Recording Menu when
                  Postcard recording is enabled.
12/14/07   Yogi   Modified code to use MMOverlay.
12/07/07   jdas   Added Dynamic frame overlay UI support
11/26/07   Yogi   Fixed the overlapping menu names problem, minor code cleanup
                  in Qcamcorder_DisplayPreviewMenuName Function.
11/21/07   Yogi   Added Post card recording support 
11/20/07   gopi   Modified File Merge Api as brew team requirements.
11/20/07   dhaval Removed OEMCamera_Priv.h from camcorder header file
11/19/07   dhaval  Removed compilation warnings
10/16/07   dhaval Do postedit operation in play callback
09/20/07   jdas   Dynamic Overlay support in camcorder
09/18/07   Gopi   Added support for File merge
09/17/07   dhaval Added save_in_progress variable
09/03/07   Yogi   Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
07/24/07   yogi   Added an additional enumeration for video format none. 
06/29/07   kadhir Added max and dynamic bitrate support for camcorder recording
06/19/07   kadhir Added FTL and USB as storage media
05/08/07   gopi   Convert Elapsed recording time units as milliseconds from seconds.
03/28/07   gopi   Fixed SKM brand recording problems with AMR audio format.
03/21/07   Kadhir Added UI support for video stabilization
                  feature (post processing mode).
03/01/07   dhaval Define QCAMCORDER_MAX_EXTENSION_SIZE macro.
11/28/06   Pavan  Support added for Movie Editing Interface
11/09/06   dhaval Added partial transcoding support
10/30/06   dhaval Added UI support for space limit threshold selection
10/23/06   jdas   Disabled Qcamcorder app while in VT call
10/03/06   Nag    Error message added in case target bit rate is out of range
09/04/06   dhaval Added support to k3g,mp4 and 3g2 as target brand in transcoder.
07/21/06   wyh    Removed global variables for controlling ccd special efx
                  FEATURE_VIDEO_ENCODE_FADING moved to custcamcorder.h
05/09/06   Nag    Added support for start and end time for Transcode
03/27/06   dw     Changes for raw mode advanced configurations
03/08/06   kusuma Added support for smooth zoom.
01/05/06   cl     Added QCAMCORDER_AUDIODUB_ABORT QcamcorderState.
01/03/06   Gopi   Added support for launching camcorder app with out sensor.
12/22/05   gm     Modified alignment of Event Payloads
12/22/05   gm     Created data structure for Event Payloads
11/30/05   dw     Added display of clip's telop text stream and GPS properties
                  Reduced playback progress bar height for MDP optimizations
11/21/05   Gopi   Added support for Auto/Manual Focus.
11/14/05   Gopi   Added support for storing class id of Audio.
09/26/05   dw     Added support for displaying file name during recording
09/13/05   dw     Code enhancements for Raw Mode feature
08/24/05   cl     Fix for background not updated when user goto album.
08/11/05   alin   Audiodubbing fixes: Enable video-only playback;  Map camcorder
                  to BREW audio format;  Fix memory leaks,  IMovieFile bug.
08/02/05   cl     Added an option in album mode (play-all) where all recorded
                  clips can be played
08/02/05   drl    Fix for empty album not displaying preview.
07/13/05   cl     Lint-free qcamcorder.c
07/12/05   cl/dw  Added boolean for focusing on specific clips in album
           dw     Added new states for handling post editing errors
07/08/05   alin   Restart + Pause QTV playback prior to video editing
06/23/05   cl     Linted qcamcorder.c
06/21/05   dw     Renabled AAC advanced menus.
06/15/05   dw     Added menu support for advanced AAC settings, but function
                  of new menus currently disabled
06/08/05   dw     Added support for camsensor antibanding
                  Added camera callback error states for user notification
06/07/05   alin   Rename some macros to be camcorder specific
05/20/05   dw     Fixes for disabling qcamcorder while in call.
                  Added hooks for full screen playback display.
05/17/05   dw     Added menu icons for AAC audio and special fade effects
05/10/05   dw     Enabled post editing to work between FLASH and MMC
05/05/05   ali    Error from VideoEdit will delete temporary files.
04/29/05   dw     Added H263 sample option to video format menu
04/27/05   dw     Added H264 option within new Video Format menu
04/22/05   dw     Added raw mode support
04/04/05   dw     Enabled continuous recording with fading special effects
                  Removed featurization for MDP.
03/30/05   dw     Added IDIAG code support for 6550 Qcamcorder
                  Fixed invalid menu settings for short header
03/16/05   dw     Removed redundant menu type enum
03/16/05   drl    Added support for rapid Start/Stop.
03/01/05   dw     Added menu option for time stamp special effects
03/01/05   ali    Changes for transcoder brand- added 3gp
02/22/05   dw     Added globals for fade in/out video encoding effects
02/22/05   wh/dw  Fading/bitrate control
02/10/05   wyh    Allow users to specify bitrate in transcoder.
01/26/05   dw     Added variable m_bRecord_to_Preview for getting zoom parameters
                  only after camera callback when transitioning from Record to Preview modes.
                  Changed feature name of bitrate menu to FEATURE_VIDEO_ENCODE_BITRATE.
01/20/05   dw     Added and featurized new user specified bit rate menu
01/19/05   wh     Changed the feature name for overlay to avoid evoking pre-
                  6550 codes
01/18/05   dw     Added routines for featurized frame overlay UI support
01/08/04   dw     Fixes for full display of telop text during text entry
12/13/04   ssc    #include file name case corrections for builds in
                  case-sensitive Linux build environment.
09/16/04   ali    Added header info
============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "comdef.h"

#if defined(FEATURE_APP_QCAMCORDER)
#ifndef QCAMCORDER_H
#define QCAMCORDER_H

#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"            // File interface definitions
#include "AEESound.h"            // Sound Interface definitions
#include "AEEMenu.h"        // Menu Services
#include "AEEStdLib.h"   // AEE stdlib services
#include "AEEMediaFormats.h"

#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif /* FEATURE_MMOVERLAY */

#ifdef FEATURE_VIDEO_ENCODE_RAW
#include "AEESource.h"          // For Getline routines
#endif /* FEATURE_VIDEO_ENCODE_RAW */

#include "AEEImage.h"
#include "AEEImageCtl.h"
#include "AEEMedia.h"         // AEE Multimedia Services
#include "AEECamera.h"
#include "AEEPosDet.h"
#include "OEMFeatures.h"
#include "QcamCommon.h"
#include "qcamcorder_res.h"
#include "mdp_drv_api.h"        // For MDP display

#ifdef FEATURE_MP4_TRANSCODER
#include "AEEMovieFile.h"
#endif
#ifdef FEATURE_BREW_IMOVIEEDIT
#include "AEEMovieEdit.h"
#endif /* FEATURE_BREW_IMOVIEEDIT */
#ifdef FEATURE_ICM
#include "AEECM.h"
#endif
#ifdef FEATURE_QVPHONE
#include "AEEVideoPhone.h"
#endif

#include "AEEMediaMPEG4.h"

#include "AEETelephone.h"
#include "AEEIModel.h"


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

// Make sure this value is in sync with the camera settings (camif.h)
#define QCAMCORDER_CAMERA_SIZE CAM_SIZE_VGA

// Size of buffer used to hold text strings
#define TEXT_STRING_SIZE (40)
#define PROMPT_SIZE (256)

#define QCAMCORDER_MP4_MOVIE_EXTENSION ".mp4"
#define QCAMCORDER_3G_MOVIE_EXTENSION ".3g2"
#define QCAMCORDER_AMC_MOVIE_EXTENSION ".amc"
#define QCAMCORDER_3GP_MOVIE_EXTENSION ".3gp"
#define QCAMCORDER_K3G_MOVIE_EXTENSION ".k3g"
#define QCAMCORDER_SKM_MOVIE_EXTENSION ".skm"

/* MAX MOVIE EXTENSION SIZE*/
#define QCAMCORDER_MAX_EXTENSION_SIZE   10

#ifdef FEATURE_VIDEO_ENCODE_RAW
#define QCAMCORDER_YUV_MOVIE_EXTENSION ".yuv"
#endif /* FEATURE_VIDEO_ENCODE_RAW */

#ifdef FEATURE_MP4_TRANSCODER
#define MOVIEMAIL_WIDTH 96
#define MOVIEMAIL_HEIGHT 80
#endif

#define QCAMCORDER_EXTENSION_LENGTH 4
#ifdef FEATURE_BREW_3_0
#define QCAMCORDER_ROOT_DIR    "fs:/~/qcdr"
#else
#define QCAMCORDER_ROOT_DIR    "qcdr"
#endif

#ifdef FEATURE_BREW_3_0
#define AEEFS_MMC_DIR  "fs:/card0/"
#else
#define AEEFS_MMC_DIR  "mmc1/"
#endif /* FEATURE_BREW_3_0 */

#ifdef FEATURE_EFS_FTL
#ifdef FEATURE_BREW_3_0
#define AEEFS_MOD_FTL_DIR "fs:/shared/ftl/"
#else
#define AEEFS_MOD_FTL_DIR "ftl1/"
#endif
#endif

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#ifdef FEATURE_BREW_3_0
#define AEEFS_USB_DIR "fs:/~0x0103809c/"
#define AEEFS_MOD_USB_DIR "fs:/mod/26108/"
#else
#define AEEFS_USB_DIR "usb1/"
#define AEEFS_MOD_USB_DIR "usb1/"
#endif
#endif



#define QCAMCORDER_SOUNDS_SDIR "snd"
#define QCAMCORDER_FRAMES_SDIR "frm"

#define QCAMCORDER_SOUNDS_DIR QCAMCORDER_ROOT_DIR DIRECTORY_STR QCAMCORDER_SOUNDS_SDIR
#define QCAMCORDER_FRAMES_DIR QCAMCORDER_ROOT_DIR DIRECTORY_STR QCAMCORDER_FRAMES_SDIR
#ifdef FEATURE_BREW_3_0

#ifdef FEATURE_COMMON_MM_DIR
#define QCAMCORDER_TMP_AUDIO_FILE "fs:/shared/media/tmpaud.qcp"
#define QCAMCORDER_TMP_VIDEO_FILE "fs:/shared/media/tmpvid.mp4"
#else
#define QCAMCORDER_TMP_AUDIO_FILE "fs:/~/media/tmpaud.qcp"
#define QCAMCORDER_TMP_VIDEO_FILE "fs:/~/media/tmpvid.mp4"
#endif /*FEATURE BREW 3.0 */
#define QCAMCORDER_TMP_VIDEO_MMC_FILE "fs:/card0/media/tmpaud.qcp"
#ifdef FEATURE_EFS_FTL
#define QCAMCORDER_TMP_VIDEO_FTL_FILE AEEFS_FTL_DIR "media/tmpvid.mp4"
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAMCORDER_TMP_VIDEO_USB_FILE AEEFS_USB_DIR "media/tmpvid.mp4"
#endif
#else

#define QCAMCORDER_TMP_AUDIO_FILE QCAMCORDER_ROOT_DIR DIRECTORY_STR "tmpaud.qcp"
#define QCAMCORDER_TMP_VIDEO_FILE QCAMCORDER_ROOT_DIR DIRECTORY_STR "tmpvid.mp4"
#define QCAMCORDER_TMP_VIDEO_MMC_FILE "mmc1/tmpvid.mp4"
#ifdef FEATURE_EFS_FTL
#define QCAMCORDER_TMP_VIDEO_FTL_FILE "ftl1/tmpvid.mp4"
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAMCORDER_TMP_VIDEO_USB_FILE "usb1/tmpvid.mp4"
#endif
#endif

#define QCAMCORDER_ROOT_DIR_LEN   (STRLEN(QCAMCORDER_ROOT_DIR   DIRECTORY_STR))
//#define QCAMCORDER_MOVIES_DIR_LEN (STRLEN(QCAMCORDER_MOVIES_DIR DIRECTORY_STR))
#define QCAMCORDER_SOUNDS_DIR_LEN (STRLEN(QCAMCORDER_SOUNDS_DIR DIRECTORY_STR))
#define QCAMCORDER_FRAMES_DIR_LEN (STRLEN(QCAMCORDER_SOUNDS_DIR DIRECTORY_STR))
// This valus is FS_FILENAME_MAX_LENGTH_P - QCAMCORDER_PHOTOS_DATE_DIR_LEN - 6
// where 6 == (STRLEN("brew/") + 1) (+ 1 to allow for the NULL terminator)
// FS_FILENAME_MAX_LENGTH_P is defined in fs_parms.h
// When FS_FILENAME_MAX_LENGTH_P is changed in fs_parms.h
// or when EFS2 is adopted, this value should be updated
#ifndef FS_FILENAME_MAX_LENGTH_P
#define FS_FILENAME_MAX_LENGTH_P 40
#endif
#define QCAMCORDER_MAX_MOVIE_FILE_NAME_LENGTH (FS_FILENAME_MAX_LENGTH_P - QCAMCORDER_MOVIES_DIR_LEN - 6)

/* NOTE: The QCamcorder quality setting is passed along to ICAMERA_SetQuality,
** which in turn passes it to ICAMERA_SetParm (CAM_PARM_QUALITY).  The
** underlying OEM layer passes it AS-IS to the video engine, which interprets
** it using the video_profile_type enum type.  So these settings need to be
** kept in sync with that enum!
*/
#define QCAMCORDER_QUALITY_DYNAMIC 5
#define QCAMCORDER_QUALITY_MAX     4
#define QCAMCORDER_QUALITY_USER 3
#define QCAMCORDER_QUALITY_HIGH 2
#define QCAMCORDER_QUALITY_MED  1
#define QCAMCORDER_QUALITY_LOW  0

// Max # of clips
#define QCAMCORDER_NUMCLIPS_LOW 3
#define QCAMCORDER_NUMCLIPS_MED 5
#define QCAMCORDER_NUMCLIPS_HIGH 10
#define QCAMCORDER_NUMCLIPS_NONE 0

// Max length of clips in seconds
#define QCAMCORDER_CLIPLENGTH_LOW 15
#define QCAMCORDER_CLIPLENGTH_MED 20
#define QCAMCORDER_CLIPLENGTH_HIGH 30
#define QCAMCORDER_CLIPLENGTH_NONE 0

#define QCAMCORDER_CONFIG_FILE  QCAMCORDER_ROOT_DIR DIRECTORY_STR "qcdr.cfg"
#define QCAMCORDER_SOUND_FILE QCAMCORDER_SOUNDS_DIR DIRECTORY_STR "record.wav"

#define QCAMCORDER_BRIGHTNESS_STEPS 10


#define QCAMCORDER_BRIGHTNESS_FACTOR 8
#define QCAMCORDER_HUE_FACTOR        8
#define QCAMCORDER_SATURATION_FACTOR 8

#define QCAMCORDER_SOFT_MENU_COLOR MAKE_RGB(255, 255, 204) // menu background color
#define QCAMCORDER_SOFT_MENU_SEL_COLOR MAKE_RGB(153, 204, 204) // selected menu background color
#define QCAMCORDER_SOFT_MENU_HEIGHT 19 // Height in pixels of the soft key menu

#define QCAMCORDER_PROGRESS_BAR_HEIGHT 13 // Height in pixels of the playback progress bar
#define QCAMCORDER_TELOP_TEXT_SPACE 8     // Extra height needed for full telop entry display
#define USER_TELOP_MAX_LENGTH  25
#define QCAMCORDER_BORDER_COLOR MAKE_RGB(255, 255, 255) // Border color

#define QCAMCORDER_MENU_HEIGHT (17)

#define QCAMCORDER_QCIF_POS_X  (0)
#define QCAMCORDER_QCIF_POS_Y  (QCAMCORDER_MENU_HEIGHT)
#define QCAMCORDER_QCIF_WIDTH  (176)
#define QCAMCORDER_QCIF_HEIGHT (144)

#define QCAMCORDER_SQCIF_POS_X  (24)
#define QCAMCORDER_SQCIF_POS_Y  ((QCAMCORDER_QCIF_HEIGHT - QCAMCORDER_SQCIF_HEIGHT) / 2 + QCAMCORDER_MENU_HEIGHT)
#define QCAMCORDER_SQCIF_WIDTH  (128)
#define QCAMCORDER_SQCIF_HEIGHT (96)

#define QCAMCORDER_PICTURE_X_POS  (0)
#define QCAMCORDER_PICTURE_Y_POS  (QCAMCORDER_MENU_HEIGHT)

#define QCAMCORDER_RECORD_BLINK_DELAY 500

#define QCAMCORDER_STATE_ICON_X (5  + QCAMCORDER_BORDER_POS_X)
#define QCAMCORDER_STATE_ICON_Y (4  + QCAMCORDER_BORDER_POS_Y)
#define QCAMCORDER_STATE_TEXT_X (24 + QCAMCORDER_BORDER_POS_X)
#define QCAMCORDER_STATE_TEXT_Y (7  + QCAMCORDER_BORDER_POS_Y)

#define QCAMCORDER_TIME_X      126
#define QCAMCORDER_TIME_Y      1
#define QCAMCORDER_TIME_WIDTH  49
#define QCAMCORDER_TIME_HEIGHT 12
#define QCAMCORDER_TIME_COLOR  MAKE_RGB(0, 128, 128)

#define QCAMCORDER_NAME_X      180
#define QCAMCORDER_NAME_Y      (1 + QCAMCORDER_TIME_HEIGHT) /* To start displaying in line next to RecordTime */
#define QCAMCORDER_NAME_WIDTH  70            /* Width increased from 60 to 70 as in 6290, name doesnt fit in to present size */
#define QCAMCORDER_NAME_HEIGHT 12
#define QCAMCORDER_NAME_COLOR  MAKE_RGB(128, 0, 0)

#define QCAMCORDER_OVERLAY_NAME_X  2
#define QCAMCORDER_OVERLAY_NAME_HEIGHT 12

#define VOCODER_PACKET_SIZE_13K_FULL         35
#define VOCODER_PACKET_SIZE_13K_HALF         17
#define VOCODER_PACKET_SIZE_EVRC             23
#define VOCODER_PACKET_SIZE_AMR              32  /* 12.2 kbps mode */


/* These values indicate the x co-ordinate at which the 
 * image/string have to be drawn to the display, when the preview menu name 
 * is displayed */
#define QCAMCORDER_PREVIEW_MENU_DISPLAY_IMAGE_X    2
#define QCAMCORDER_PREVIEW_MENU_DISPLAY_STRING_X   16

/* This is the size of an AMR file header, as defined by RFC 3267. */
#define AMR_FILE_HEADER_SIZE                 6

/* This is the size of an actual QCP file header, as generated by CMX. */
#define CMX_QCP_HEADER_SIZE                  194

/* Maximum # of audio samples to be transferred at a time during audio dubbing */
#define AUDIODUB_SAMPLES 4

#define AUDIODUB_BUF_SIZE CMX_QCP_HEADER_SIZE + (35 * AUDIODUB_SAMPLES)

/*maximum bit rate*/
#define MAX_BITRATE 512000

#define  QCC_EVENT_BASE                           EVT_USER    // Base used by ICamera
#define  QCC_EXECUTE_CONFIRM_MENU_EVENT           (QCC_EVENT_BASE + 100)


extern uint32  efx_special_fade;
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
extern boolean time_stamp_on;
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/

typedef enum
{  QCAMCORDER_SIZE_CIF = 0,
   QCAMCORDER_SIZE_QCIF,
   QCAMCORDER_SIZE_SQCIF,
   QCAMCORDER_SIZE_QVGA,
   QCAMCORDER_SIZE_MM,
   QCAMCORDER_SIZE_DISPLAY
}QcamcorderRecordSizeType;

typedef enum
{ QCAMCORDER_TIMER_OFF=0,
   QCAMCORDER_TIMER_LOW,
   QCAMCORDER_TIMER_MED,
   QCAMCORDER_TIMER_HI
}QcamcorderTimerType;

typedef enum
{ QCAMCORDER_EFFECTS_OFF=0,
   QCAMCORDER_EFFECTS_MONO,
   QCAMCORDER_EFFECTS_SEPIA,
   QCAMCORDER_EFFECTS_NEGATIVE
}QcamcorderEffectsType;

typedef enum
{
  QCAMCORDER_ANTIBANDING_OFF = 0,
  QCAMCORDER_ANTIBANDING_50HZ,
  QCAMCORDER_ANTIBANDING_60HZ
}QcamcorderAntibandingType;


#ifdef FEATURE_VIDEO_ENCODE_FADING
typedef enum
{
    QCAMCORDER_FADE_OFF,
    QCAMCORDER_FADE_IN,
    QCAMCORDER_FADE_OUT,
    QCAMCORDER_FADE_IN_FADE_OUT

}FadeInOut;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
typedef enum
{
  QCAMCORDER_SPECIAL_FADE_NONE = 0,
  QCAMCORDER_SPECIAL_FADE_ALPHA,
  QCAMCORDER_SPECIAL_FADE_CURTAIN,
  QCAMCORDER_SPECIAL_FADE_MAX

}SpecialFadeType;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */


typedef enum
{
   QCAMCORDER_GPS_DISABLED,
   QCAMCORDER_GPS_ENABLED,
   QCAMCORDER_GPS_DATA_VALID,
   QCAMCORDER_GPS_ACQUIRING,
   QCAMCORDER_GPS_FAILURE,
   QCAMCORDER_GPS_TIMEOUT,
   QCAMCORDER_GPS_INFO_UNAVAILABLE,
   QCAMCORDER_GPS_STATUS_COUNT
}QcamcorderGPSState;

typedef enum
{
   QCAMCORDER_FILE_MP4 = 0,
   QCAMCORDER_FILE_AMC,
   QCAMCORDER_FILE_3GP,
   QCAMCORDER_FILE_3G2,
   QCAMCORDER_FILE_K3G,
   QCAMCORDER_FILE_3G2_FRAG,
   QCAMCORDER_FILE_SKM
}FileFormatType;

typedef enum
{
   QCAMCORDER_FILE_XCODER_AMC=0,
   QCAMCORDER_FILE_XCODER_3GP,
   QCAMCORDER_FILE_XCODER_MP4,
   QCAMCORDER_FILE_XCODER_3G2,
   QCAMCORDER_FILE_XCODER_K3G
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
   ,QCAMCORDER_FILE_XCODER_3G2_FRAG
#endif //FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
}TranscodeFileFormatType;

typedef enum
{
   QCAMCORDER_SH_DISABLE = 0,
   QCAMCORDER_SH_ENABLE,
   QCAMCORDER_SH_TEST

}ShortHeaderType;

#ifdef FEATURE_DVS
typedef enum
{
  QCAMCORDER_DVSMARGIN_LOW = 0,
  QCAMCORDER_DVSMARGIN_MED,
  QCAMCORDER_DVSMARGIN_HIGH
}DVSMarginType;
#endif

typedef enum
{
  QCAMCORDER_VIDEO_FORMAT_MPEG4 = 0,
  QCAMCORDER_VIDEO_FORMAT_H263,
  QCAMCORDER_VIDEO_FORMAT_H263_SAMPLE,
#ifdef FEATURE_H264_ENCODER
  QCAMCORDER_VIDEO_FORMAT_H264,
#endif /* FEATURE_H264_ENCODER */
  QCAMCORDER_VIDEO_FORMAT_NONE = 4  /* This is hard coded since the values in OEMCamera.c are hardcoded.*/
}VideoFormatType;

typedef enum
{
  QCAMCORDER_AUDIO_FORMAT_NONE=MM_QCP_FORMAT_UNKNOWN,
  QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_13K=MM_QCP_FORMAT_FIXED_FULL_13K,
  QCAMCORDER_AUDIO_FORMAT_FIXED_HALF_13K=MM_QCP_FORMAT_FIXED_HALF_13K,
  QCAMCORDER_AUDIO_FORMAT_FIXED_FULL_EVRC=MM_QCP_FORMAT_FIXED_FULL_EVRC,
  QCAMCORDER_AUDIO_FORMAT_FIXED_12_2_AMR=MM_QCP_FORMAT_FIXED_12_2_AMR
#ifdef FEATURE_CAMCORDER_AAC_REC
  ,QCAMCORDER_AUDIO_FORMAT_VAR_AAC
#endif /* FEATURE_CAMCORDER_AAC_REC */
}AudioFormatType;

typedef enum
{
   QCAMCORDER_MENU_NONE,
   QCAMCORDER_MENU_PREVIEW,
   QCAMCORDER_MENU_SETTINGS,
   QCAMCORDER_MENU_EDIT
}QcamcorderSoftMenuType;

typedef enum
{
   QCAMCORDER_RECORD_START,
   QCAMCORDER_RECORD_PAUSE,
   QCAMCORDER_RECORD_STOP,
   QCAMCORDER_RECORD_RESUME,
   QCAMCORDER_RECORD_SPACE_WARNING,
   QCAMCORDER_RECORD_ERROR
}QcamcorderRecordType;

typedef enum
{
   QCAMCORDER_SAVE_FLASH,
   QCAMCORDER_SAVE_MMC
#ifdef FEATURE_EFS_FTL
   ,QCAMCORDER_SAVE_FTL
#endif
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
   ,QCAMCORDER_SAVE_USB
#endif
}QcamcorderSaveType;

typedef enum
{ QCAMCORDER_WHITEBALANCE_AUTO,
  QCAMCORDER_WHITEBALANCE_FLUORESCENT,
  QCAMCORDER_WHITEBALANCE_INCANDESCENT,
  QCAMCORDER_WHITEBALANCE_DAYLIGHT
}QcamcorderWhiteBalanceType;

typedef enum
{
    QCAMCORDER_ROTATE_OFF=0,
    QCAMCORDER_ROTATE_90,
    QCAMCORDER_ROTATE_180,
    QCAMCORDER_ROTATE_270

}RotateDegree;

#ifdef FEATURE_VIDEO_ENCODE_RAW
typedef enum
{
  QCAMCORDER_VIDEO_STREAM_NONE = 0, /* no video                       */
  QCAMCORDER_VIDEO_STREAM_MPEG4,    /* ISO/IEC 14496-2                */
  QCAMCORDER_VIDEO_STREAM_H263,     /* H.263                          */
  QCAMCORDER_VIDEO_STREAM_H264      /* H.264                          */
}QcamcorderRawEncoderVideoType;

typedef enum
{
  QCAMCORDER_RAW_CONFIG_FAIL = 0,  /* No config                       */
  QCAMCORDER_RAW_MENU_CONFIG,      /* Config by menu settings         */
  QCAMCORDER_RAW_FILE_CONFIG       /* Config by info file contents    */
}QcamcorderRawEncoderConfigState;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

typedef enum
{
  QCAMCORDER_DISPLAY_FULL_SCREEN,
  QCAMCORDER_DISPLAY_MENU_SELECT
}QcamcorderDisplaySizeType;

typedef enum
{
  QCAMCORDER_PLAYBACK_FULL_SCREEN,
  QCAMCORDER_PLAYBACK_REGULAR
}QcamcorderPlayBackSizeType;


#ifdef FEATURE_CAMCORDER_AAC_REC
typedef enum
{
  QCAMCORDER_AAC_CONFIG_MODE,
  QCAMCORDER_AAC_CONFIG_FREQ,
  QCAMCORDER_AAC_CONFIG_BITRATE
}QcamcorderAACConfigType;

typedef enum
{
  QCAMCORDER_AAC_MONO2MONO = 0,
  QCAMCORDER_AAC_STEREO2MONO,
  QCAMCORDER_AAC_STEREO2STEREO
}QcamcorderAACModeType;

typedef enum
{
  QCAMCORDER_AAC_FREQ_8 = 0,
  QCAMCORDER_AAC_FREQ_11,
  QCAMCORDER_AAC_FREQ_16,
  QCAMCORDER_AAC_FREQ_22,
  QCAMCORDER_AAC_FREQ_24
}QcamcorderAACFreqType;

typedef enum
{
  QCAMCORDER_AAC_BITRATE_LOW = 0,
  QCAMCORDER_AAC_BITRATE_MED,
  QCAMCORDER_AAC_BITRATE_HIGH,
}QcamcorderAACBitrateType;
#endif /* FEATURE_CAMCORDER_AAC_REC */

typedef enum
{
   QCAMCORDER_MENU_NOMENU,
   QCAMCORDER_MENU_SOUND,
   QCAMCORDER_MENU_NUMCLIPS,
   QCAMCORDER_MENU_CLIPLENGTH,
   QCAMCORDER_MENU_SIZE,
   QCAMCORDER_MENU_DELETEALL,
   QCAMCORDER_MENU_FILELIST,
   QCAMCORDER_MENU_QUALITY,
   QCAMCORDER_MENU_AUDIO_FORMAT,
   QCAMCORDER_MENU_FRAMERATE,
   QCAMCORDER_MENU_GPS,
   QCAMCORDER_MENU_SAVE_LOC,
   QCAMCORDER_MENU_DELETE,
   QCAMCORDER_MENU_FILE_FORMAT,
   QCAMCORDER_MENU_SHORT_HEADER,
   QCAMCORDER_MENU_VIDEO_FORMAT,
   QCAMCORDER_MENU_TELOP,
   QCAMCORDER_MENU_CONT,
   QCAMCORDER_MENU_WHITEBALANCE,
   QCAMCORDER_MENU_MEM,
   QCAMCORDER_MENU_TIMER,
   QCAMCORDER_MENU_EFFECTS,
   QCAMCORDER_MENU_ANTIBANDING,
   QCAMCORDER_MENU_ROTATE,
   QCAMCORDER_MENU_SPECIAL_EFFECTS
#ifdef FEATURE_MP4_TRANSCODER
   ,QCAMCORDER_MENU_TRANSCODER_VIDEO_SIZE,
   QCAMCORDER_MENU_TRANSCODER_AUDIO_FORMAT,
   QCAMCORDER_MENU_TRANSCODER_BRAND,
   QCAMCORDER_MENU_TRANSCODER_VIDEO_FORMAT
#endif
#ifdef FEATURE_DVS
   ,QCAMCORDER_MENU_DVSMARGIN
#endif
#ifdef FEATURE_VIDEO_ENCODE_RAW
   ,QCAMCORDER_MENU_RAWFILELIST
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#ifdef FEATURE_CAMCORDER_AAC_REC
  ,QCAMCORDER_MENU_AAC_CONFIG,
   QCAMCORDER_MENU_AAC_MODE,
   QCAMCORDER_MENU_AAC_FREQ,
   QCAMCORDER_MENU_AAC_BITRATE
#endif /* FEATURE_CAMCORDER_AAC_REC */
   ,QCAMCORDER_MENU_POST_EDIT
   ,QCAMCORDER_MENU_SPACE_THRESHOLD
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif


}QcamcorderMenuType;

typedef enum
{
  QCAMCORDER_NULL,
  QCAMCORDER_START_PREVIEW,
  QCAMCORDER_PREVIEW,
  QCAMCORDER_START_RECORDING,
  QCAMCORDER_RECORDING,
  QCAMCORDER_PAUSED,
  QCAMCORDER_RESUME_RECORDING,
  QCAMCORDER_EDIT,
  QCAMCORDER_PLAYING,
  QCAMCORDER_SPACE_WARNING,
  QCAMCORDER_PROPERTIES,
  QCAMCORDER_TELOP_POSTEDIT,
  QCAMCORDER_GPS_POSTEDIT,
  QCAMCORDER_PREVIEW_TO_SETTINGS,
  QCAMCORDER_PREVIEW_TO_GPS,
  QCAMCORDER_PREVIEW_TO_ALBUM,
#ifdef FEATURE_VIDEO_ENCODE_RAW
  QCAMCORDER_PREVIEW_TO_RAW_ALBUM,
#endif /* FEATURE_VIDEO_ENCODE_RAW */
  QCAMCORDER_CAMLAYER_ERROR
#ifdef FEATURE_MP4_TRANSCODER
  ,QCAMCORDER_TRANSCODE
  ,QCAMCORDER_TRANSCODE_DONE
  ,QCAMCORDER_TRANSCODE_UNSUPPORTED
  ,QCAMCORDER_TRANSCODE_ERROR
#endif
#ifdef FEATURE_QCAMCORDER_VIDEO_EDITING
  , QCAMCORDER_VIDEO_EDIT_SELECTION
  , QCAMCORDER_VIDEO_EDIT_PENDING
  , QCAMCORDER_VIDEO_EDIT_STARTED
  , QCAMCORDER_VIDEO_EDIT_DONE
  , QCAMCORDER_VIDEO_EDIT_DONE_ERROR
  , QCAMCORDER_VIDEO_EDIT_UNSUPPORTED

#endif
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
  ,QCAMCORDER_CUT_MERGE
  ,QCAMCORDER_CUT_MERGE_DONE
#endif
  ,QCAMCORDER_POSTEDIT
#ifdef FEATURE_VIDEO_AUDIO_DUBBING
  ,QCAMCORDER_AUDIODUB
  ,QCAMCORDER_AUDIODUB_ABORT
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */
#ifdef FEATURE_VIDEO_ENCODE_FADING
  ,QCAMCORDER_FADING_OUT
#endif /* FEATURE_VIDEO_ENCODE_FADING */
#ifdef FEATURE_VIDEO_ENCODE_RAW
  ,QCAMCORDER_START_RAW_ENCODE
  ,QCAMCORDER_RAW_ENCODING
#endif /* FEATURE_VIDEO_ENCODE_RAW */
,QCAMCORDER_TRANSPORTION
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif
}QcamcorderState;

typedef struct
{
   uint32 dwFreeSpace; // in KB
   uint32 dwTotalFileSystemSize; // in KB
   uint32 dwRemainingVideoDuration; // in seconds
   uint32 dwUsedMemory; // in percentage
} Qcamcorder_MemUsageData;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif// FEATURE_APP_TEST_AUTOMATION

#define QCAMCORDER_FRAME_RATE_1 0x0002000F
#define QCAMCORDER_FRAME_RATE_2 0x00020014
#define QCAMCORDER_FRAME_RATE_3 0x0002001E

typedef struct QProgCtl   QProgCtl;
// Progress control: includes progress bar and the title
struct QProgCtl
{
   IShell *          m_pIShell;
   IDisplay *        m_pIDisplay;
   IStatic *         m_pTitle;
   AEERect           m_rectMain;
   AEERect           m_rectBar;
};


#ifdef FEATURE_VIDEO_ENCODE_RAW
/* DO NOT CHANGE the order of the members in this
** structure. Modifications will cause raw encoder
** to parse incorrect init parameter values */
typedef struct
{
  /* Regular raw encoding config parameters */
  uint32  frame_width;
  uint32  frame_height;
  uint32  file_brand;
  uint32  videoformat;
  uint32  time_increment;
  uint32  time_resolution;
  uint32  profile;
  uint32  user_bitrate;
  uint32  rotation;
  uint32  save_location;
  uint32  start_frame;
  uint32  end_frame;

  /* Advanced raw encoding firmware config parametners */
  uint32  adv_cfg_enable;        /* enable flag for advanced lib config */
  uint32  four_mv_threshold;     /* 4mv threshold */
  uint32  intra_16x16_bias;      /* intra 16x16 bias */
  uint32  intra_4x4_bias;        /* intra 4x4 bias */
  uint32  inter_1mv_bias;        /* inter one mv bias */
  uint32  inter_4mv_bias;        /* inter four mv bias */
  uint32  mb_search_type;        /* type of search: 1-MB, 2-Block, 3-MB and Block */
  uint32  mb_min_blk_anchor;     /* use MB min as block anchor */
  uint32  mb_anchor_blk_anchor;  /* use MB anchor as block anchor */
  uint32  fractional_pel_enable; /* Fractional pel search enable */
  uint32  qp_I_VOP;              /* Quantizer Scale I-VOPs */
  uint32  qp_P_VOP;              /* Quantizer Scale P-VOPs */
  uint32  quant_precision;       /* Quantizer Precision */
  uint32  intra_period_ms;       /* Intra period in milliseconds */
  uint32  rc_mode;               /* rate control: 0-off, 1-on, 2-on_var_fps */
  uint32  input_format;          /* input format: 0-YCbCr linepacked, 1-YCrCb interlaced */

}RawEncoderConfigType;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

typedef union{
  const char *utf8;
  const short *utf16;
} CameraUTFStringType;

typedef struct{
  uint16              language;
  boolean             isUTF16;
  CameraUTFStringType string;
  uint32              length;
} CameraLanguageCodedString;
  
typedef struct{
  boolean 					 loci_valid;
  CameraLanguageCodedString  name;
  uint8                      role;
  uint32                     longitude;
  uint32                     latitude;
  uint32                     altitude;
  CameraLanguageCodedString  astr_body;
  CameraLanguageCodedString  add_notes;
} CameraLocationType;

typedef struct{
  boolean                     atom_is_valid;
  boolean 					  titl_auth_dscp_cprt_valid;	
  CameraLanguageCodedString   title;
  CameraLanguageCodedString   author;
  CameraLanguageCodedString   description;
  CameraLanguageCodedString   copyright;
  CameraLocationType          loci;
}CameraUdatAtomType;

typedef struct _Qcamcorder Qcamcorder;        //all variable needed in this application

// Qcamcorder applet structure
struct _Qcamcorder
{
   AEEApplet                  a;
   AEEDeviceInfo              di;

   ICamera                    *m_pICam;
   IMenuCtl                   *m_pIPreview;
   IMenuCtl                   *m_pISettings;
   IMenuCtl                   *m_pIMenu;
   IMenuCtl                   *m_pIEditMenu;
   IMenuCtl                   *m_pISoftMenu;
   IDisplay                   *m_timeDisplay;
   IDisplay                   *m_nameDisplay;
   IMedia                     *pMedia;       // For playing audio/video
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
   IMedia                     *pRecordMedia; // For recording audio

   QcamcorderMenuType         m_activeMenu;
   QcamcorderSoftMenuType     m_activeSoftMenu;

   char                       sCurrentFileName[FILE_NAME_SIZE];
   char                       sRecordFileName[FILE_NAME_SIZE];
   IFileMgr                   *pIFileMgr;
   char                       sStartSoundFile[FILE_NAME_SIZE];

#ifdef FEATURE_VIDEO_ENCODE_OVERLAY_UI
   boolean                    bUpdatePreviewFrame;
   boolean                    bEnableFrameOverlay;
   char                       sFrameOverlay[FILE_NAME_SIZE];
   char                       sOverlayMenuFile[FILE_NAME_SIZE];
   IDisplay                   *m_overlayDisplay;
   IDIB                       *pFrame;
   IImage 					  *pIImage; /* Added for use in Dynamic frame overlay to pass as 
                                                               argument to QCam_GetPNGImageData() in 
                                                               Qcamcorder_Callback_LoadFrame() */
#endif

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

#ifdef FEATURE_VIDEO_ENCODE_FADING
   FadeInOut                  mFadeInOut;
#endif

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
   SpecialFadeType            mSpecialFade;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
   boolean                    bEnableTimeStamp;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/
   IFile                      *pIFile;
   QcamcorderState            eState;
   int32                      nBright;
   int32                      nBrightnessMin;
   int32                      nBrightnessMax;
   int32                      nBrightnessStep;
   uint8                      nAutoOrManFocus;
   uint8                      FocusInProgress;
   int32                      nFocus;
   int32                      nFocusMin;
   int32                      nFocusMax;
   int32                      nFocusStep;
   int32                      nZoom;
   int32                      nZoomMin;
   int32                      nZoomMax;
   int32                      nZoomStep;

   AEEGPSInfo                 GPSInfo;
   QcamcorderGPSState         eGPSState;
   uint8                      nGPSAccuracy;
   AEECallback                GPSInfoCB;
   IPosDet                    *pIPosDet;

   QcamcorderSaveType saveLoc;
   uint16                     m_maxNumClips;
   uint16                     m_maxClipLength;  // in seconds
   boolean                    bStopRecording;     // For continuous record mode
   uint32                     nRecordStartTime;
   uint32                     nRecordStartTimeMs;
   #ifndef FEATURE_CAMERA_LCD_DIRECT_MODE
   uint32                     nPreviousFrameTimeMs; //used in case of pause to get more accurate time
   boolean                    bPausePressed;
   #endif
   uint32                     nRecordTotalTime;
   uint32                     nRecordTotalTimeMs;
   uint16                     wCurrentMovie;       // # to use for next filename when recording
   uint16                     wQuality;

#ifdef FEATURE_VIDEO_ENCODE_BITRATE
   int32                      wBitRate;     // User specified bit rate
                                        // Valid if Quality setting is USER
#endif

   uint32                     wAudioFormatClsId;
   uint16                     wAudioFormat;
   uint16                     wAudioFormatUsed; // Type of audio recorded
   uint16                     wPreviewMenuSel;
   QcamcorderRecordSizeType   eSize;
   uint32                     m_FrameRate;
   FileFormatType             wFileFormat;
   ShortHeaderType            wShortHeader;
   VideoFormatType            wVideoFormat;
   uint16                     m_nwhiteBalance;
   QProgCtl                   m_ProgCtl;
   int32                      m_bTelop;
   uint32                     msg;

#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   uint32                     m_dFirstMarker;
   uint32                     m_dSecondMarker;
#endif

   uint32                     m_dwTotalTime;    // Total playback time
   uint32                     m_dwPlayPos;      // Current playback pos
   uint16                     m_wVolume;        // Volume
   boolean                    m_bEditHit;
   uint32                     m_dStartPos;
   uint32                     nVideoTimeScale;

#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   uint32                     nVopTimeIncRes;
#endif

   uint32                     nAudioTimeScale;
   QcamcorderTimerType        m_nTimer;
   boolean                    bDisplaybar;

#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   int32                      time_stamp1;
   int32                      time_stamp2;
   int32                      original_timestamp1;
   int32                      last_time;
   int32                      delta;
#endif

   char                       sVideoEditFileName[FILE_NAME_SIZE];
   char                       sRawEncoderFileName[FILE_NAME_SIZE];
   char*                      pFileFormat;
   int32                      nTimer;
   int32                      nLeftSec;
   QcamcorderEffectsType      m_nEffect;
   QcamcorderAntibandingType  m_nAntibanding;
   RotateDegree               m_bRotate;
   boolean                    bPostEditDone;
   CameraData                 *uuid_data;
   CameraData                 *uuid;
   CameraUdatAtomType         *udat_atom;
   AEERect                    EditRect;
   boolean                    m_bCont;
#ifdef FEATURE_MP4_TRANSCODER
   byte                       *pVideoHeaderContents;
   byte                       *pAudioHeaderContents;
   boolean                    bTranscodeHeader;
   uint32                     videoHeaderSize;
   uint32                     audioHeaderSize;
   IMovieFile                 *pOutFile;
   IMovieFile                 *pInFile;
   uint16                     wTranscodeAudioFormat;
   QcamcorderRecordSizeType   eTranscoderSize;
   char                       sTranscodeFileName[FILE_NAME_SIZE];
   char                       sTranscodeOutFile[FILE_NAME_SIZE];
   AEEMovieFileInfo           *mfi;
   TranscodeFileFormatType    eTranscoderBrand;
   boolean                    bTranscode_portion;
   VideoFormatType            eTranscoderVideoCodec;
   ShortHeaderType            wTranscoderShortHeader;
   uint32                     nStartFrameNo;
   uint32                     nEndFrameNo;
#endif
#ifdef FEATURE_DVS
   boolean                    bDVS;
   DVSMarginType              eDVSMargin;
#endif
#ifdef FEATURE_VIDEO_ENCODE_RAW
   IMovieFile                 *pRawStartFile;
   IMovieFile                 *pRawStopFile;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

#ifdef FEATURE_BREW_IMOVIEEDIT
   IMovieEdit                 * m_pInEdit;
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
   IMovieEdit                 * m_pOutEdit;
   uint32                     m_movieeditStatus;
   AEECallback                m_movieeditCB;
   AEEMovieEditInfo           m_mei;
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
   uint32                     m_nEditPortion;
   uint32                     m_nPostEditTelopSize;
   uint8                      * m_pPostEditTelopData;
   AEEMovieEditCutInfo        * m_cutInfo;
   AEEMovieEditAudioDubInfo   * m_auddubInfo;
   AEEMovieEditExtension      * m_extension;
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif //#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
   AEEMovieEditTranscoderStatus m_transcoderstatus;
   AEETranscoderTargetParams   targetparams;
   boolean                     bTranscoderStoping;   
#endif //#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
#endif /* FEATURE_BREW_IMOVIEEDIT */

#ifdef FEATURE_QCAMCORDER_TELOP
   boolean                    b_telopEntered;
#endif

#ifdef FEATURE_MMOVERLAY
   IMMOverlayMgr     *m_pIMMOverlayMgr;
   IMMOverlay        *m_pIMMOverlay1;
   IMMOverlay        *m_pIMMOverlay2;
   IMMOverlay        *m_pIMMOverlay_HideVideoFrm;
#else
   uint32                     m_mdpMenu_ovId;
   uint32                     m_mdpTitle_ovId;
#endif /* FEATURE_MMOVERLAY */

   boolean                    m_bShowMenu;

   boolean                    m_bRecord_to_Preview;
   boolean                    m_bPreview_to_Record;
   boolean                    m_bWait_to_Finish;
   QcamcorderDisplaySizeType  m_displaySize;
   QcamcorderPlayBackSizeType m_playbackSize;

#ifdef FEATURE_CAMCORDER_AAC_REC
   QcamcorderAACConfigType    m_AACConfig;
   uint32                     m_AACInfo;
#endif /* FEATURE_CAMCORDER_AAC_REC */

   AEEMediaMPEG4SpecEx        ExtendedInfo;
   char                       TelopTextInfo[USER_TELOP_MAX_LENGTH];

  /* diag automation specific data for the qcamcorder */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  //used in place of *m_pICM 
  ITelephone                   *pIPhone;
  ModelListener                 phoneListener; 
  boolean                      bVoiceCallRejected;

#ifdef FEATURE_QVPHONE
   IVideoPhone                *m_pIVP;
#endif /* FEATURE_QVPHONE */

   /* Focus on topmost clip in album mode */
   boolean                    b_focusFirstItem;

   /* Play all album files */
   boolean                    m_bPlayAll;

#ifdef FEATURE_VIDEONOSENSOR
   boolean                    m_bNoSensor;
#endif

   boolean                    m_bFocusStarted;

   boolean                    m_bKeySelect;//For smooth zoom
   uint16                     m_bLastKey; //For smooth zoom

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
   AEEMediaDataEx   * m_pmdExList;   //Extended Media data list
   boolean          AudioDubbing;
#endif

   uint8                      wSpaceLimit;  // space limit threshold <Low,Med,High or default>
   boolean                    bSaveInPrg;
#ifdef FEATURE_VIDEO_ENCODE_FILE_MERGE
#error code not present
#endif

#ifdef FEATURE_MMOVERLAY 
   /* This is a temporary hack to fix the problem described below:
    * When the error message is displayed after adding MMOVERLAY it 
    * gets cleared immediately due to updation of the background layer 
    * with the preview frames. */
   boolean                    b_displayingSpaceErrorMsg;
#endif /* FEATURE_MMOVERLAY */

};

typedef struct
{
 FileFormatType record_file_format;
 VideoFormatType record_video_format;
 uint16 record_quality;
 uint32 record_frame_rate;
 AudioFormatType record_audio_format;
} _record_event_payload;

typedef struct
  {
   VideoFormatType record_video_format;
   FileFormatType  record_file_format;
   AudioFormatType record_audio_format;
   TranscodeFileFormatType xcode_file_format;
   uint16 xcode_audio_format;
  } _xcoder_event_payload;


char* QCamcorder_GetAuddubfileName(Qcamcorder* pMe);
char* Qcamcorder_GetVideoEditFileName(Qcamcorder* pMe);
char* Qcamcorder_GetTranscodeFileName(Qcamcorder* pMe);

#endif /* QCAMCORDER_H */
#endif  /* if FEATURE_APP_QCAMCORDER */
