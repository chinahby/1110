#ifndef ICAMERAEXT_H
#define ICAMERAEXT_H

/*============================================================================
FILE:   ICameraExt.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file defines extra ICamera interfaces that is not in current AEECamera.h.
   The items should be removed if they are picked up by the AEECamera.h.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright  2006 - 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/oem/icamera/rel/3.1.5/latest/inc/ICameraExt.h#4 $ $DateTime: 2009/04/22 03:16:32 $ $Author: kgelli $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/25/08  RK    Added a New Feature ACC(Audio Camera Concurrency) support if 
                 FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND is defined.
 08/11/08   KC     Added CAM_PARM_MULTISHOT_MODE for multishot mode
 02/15/08  sanal  Added  CAM_PARM_FRAMES_DROPPED
 12/07/07  dhaval Added ICAMERA_SetResolution support
 10/16/07  NC     Set condition per AEECamera.h BREW 3.1.5 SP01 update.
 10/17/07  kgelli Added CAM_PARM_PANORAMIC for panoramic state
 08/07/07  SK     Added CAM_PARM_STITCH for Panoramic stitch feature  
 07/19/07  KV     Updated the document for Param ID CAM_PARM_THUMBNAIL_INFO
 03/16/07  NC     Correct document for some parm IDS.
 02/27/07  NC     Added documentation for Direct mode Parm ID and struct.
 02/05/07  NC     Added CAM_STATUS_UPDATE for preview histogram ready callback.
 01/12/07  rsu    Added CAM_PARM_DIS for DIS Settings
 01/04/07  sk     Made correction for p2 of CAM_PARM_AF_INFO for Redesining.
 01/03/07  sk     Changed definitions for p2 of CAM_PARM_AF_INFO for Redesining.
 12/14/06  gm     Added CAM_PARM_LCD_DIRECT_ACCESS_EX For Direct Mode Display 
 10/30/06  dhaval Added CAM_PARM_SPACE_LIMIT case to set the space limit 
                  threshold value in camcorder.
 10/04/06  NC     Updated Preview Histogram definitions.
 09/05/06  NV     Added UI support for Hue and Saturation
 08/15/06  NC     Remove CAM_BESTSHOT_TEXT.
 07/28/06  NC     Defined special ISO settings
 07/21/06  wyn    Camcorder special effects control. 
 07/14/06  NC     Added Night option for BSM.
 07/10/06  NC     Updated some definitions. 
 06/07/06  NC     Initial design of the file
===============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEECamera.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//Port extra definitions for 3.1.4.
//IMPOTANT NOTE: Remove CAM_PARM_FRAME_TIMESTAMP definition if it is not defined 
//in AEECamera.h or here.
#ifndef CAM_PARM_FRAME_TIMESTAMP // This is BREW 3.1.4 with no new 3.1.5 stuff
//
// Status codes
//
#define CAM_ERR_SPACE_MSG_SAMPLE_TABLE     (CAM_ERR_BASE + 10)  // Sample table is reaching limit
#define CAM_ERR_SPACE_MSG_CHUNK_TABLE      (CAM_ERR_BASE + 11)  // Chunk table is reaching limit
#define CAM_ERR_SPACE_MSG_STORAGE          (CAM_ERR_BASE + 12)  // Storage (i.e. Flash, MMC etc) is reaching limit
#define CAM_ERR_SPACE_MSG_MOVIE_DURATION   (CAM_ERR_BASE + 13)  // Movie duration is reaching limit
#define CAM_ERR_SPACE_MSG_VIDEO            (CAM_ERR_BASE + 14)  // Video is reaching limit
//
// Camera Control Parms
//
#define CAM_PARM_QCOM_RESERVED_BEGIN (CAM_PARM_CTL_BASE + 20)     // Reserved parameter ID's begin
#define CAM_PARM_FADE               CAM_PARM_QCOM_RESERVED_BEGIN //[Set] Set the fading for movie recording mode
#define CAM_PARM_QCOM_RESERVED_END  (CAM_PARM_QCOM_RESERVED_BEGIN + 100) // Reserved parameter ID's end

#define CAM_PARM_FRAME_TIMESTAMP   (CAM_PARM_BASE + 23)     // [Set/Get] Enable encoding of time stamp in movie frames
//
#define CAM_PARM_DEBLUR             (CAM_PARM_CTL_BASE + 126)// [Set/Get] Hand jitter reduction
#define CAM_PARM_LUMA_ADAPTATION    (CAM_PARM_CTL_BASE + 127)// [Set/Get] Luma Adaptation
#define CAM_PARM_FLASH_CTL          (CAM_PARM_CTL_BASE + 128)// [Set/Get] Flash control
#define CAM_PARM_AF_INFO            (CAM_PARM_CTL_BASE + 129) // [Set/Get] Set the auto focus info

//Fading

#define CAM_FADE_BASE                   (0x0)     // Fading Base
#define CAM_FADE_USER_BASE              (0x1000)  // Fading Base used for extension

#define CAM_FADE_OFF                    (CAM_FADE_BASE) // Fade off
#define CAM_FADE_IN                     (CAM_FADE_BASE + 1) // Fade In On
#define CAM_FADE_OUT                    (CAM_FADE_BASE + 2) // Fade Out On
#define CAM_FADE_IN_OUT                 (CAM_FADE_BASE + 3) // Fade In Out On

//
// Camera encoding. In addition to these values, AEECLSID of media formats can also
// be used -- for example, AEECLSID_JPEG or AEECLSID_MEDIAMPEG4.
//
#define CAM_ENCODE_UUID_LIST_ATOM_INFO    (CAM_ENCODE_BASE + 6) // UUID atom data
//
//
// CAM_MODE_MOVIE specific
//
#define CAM_MOVIE_BASE              0
#define CAM_MOVIE_NORMAL            (CAM_MOVIE_BASE)     // Normal movie
#define CAM_MOVIE_POSTCARD          (CAM_MOVIE_BASE +1)  // Postcard movie
//
// Effect (See documentation of CAM_PARM_EFFECT)
//
#define CAM_EFFECT_POSTERIZE           (CAM_EFFECT_BASE + 8)   // Posterize
#define CAM_EFFECT_WHITEBOARD          (CAM_EFFECT_BASE + 9)   // Whiteboard
#define CAM_EFFECT_BLACKBOARD          (CAM_EFFECT_BASE + 10)  // Blackboard
#define CAM_EFFECT_AQUA                (CAM_EFFECT_BASE + 11)  // Aqua
//
// Flash control (See documentation of CAM_PARM_FLASH_CTL)
//
#define CAM_FLASHTYPE_MASK            0xFF000000   // Flash type mask
#define CAM_FLASHTYPE_DEFAULT         0x00000000   // Default flash
#define CAM_FLASHTYPE_LED             0x01000000   // LED flash
#define CAM_FLASHTYPE_STROBE          0x02000000   // Strobe flash
//
// Auto Focus Settings (p1 of CAM_PARM_AF_INFO)
//
#define CAM_AF_MODE_NORMAL                0  // Normal mode
#define CAM_AF_MODE_MACRO                 1  // Enable Macro mode
#define CAM_AF_MODE_FULL_RANGE            2  // Enable full range auto focus
//
// Auto Focus metering (p2 of CAM_PARM_AF_INFO)
//
#define CAM_AF_METER_AUTO              1
#define CAM_AF_METER_CENTER_SPOT       2 // Focus based on center spot metered
#define CAM_AF_METER_CENTER_WEIGHTED   3 // Focus based on center weighted metered
#define CAM_AF_METER_AVERAGE           4 // Focus based on average metered
//
// UUID Atom Info
//
typedef struct CameraEncodeUUIDAtomInfo
{
   byte *   pAtom;         // atom type
   uint32   dwAtomSize;    // Number of bytes in pAtom
   byte *   pData;         // data
   uint32   dwDataSize;    // Number of bytes in pData
} CameraEncodeUUIDAtomInfo;

// AAC Encoding Advanced Info
typedef struct CameraAACEncodingInfo
{
   uint16    uMode;        // Input / Output AAC mode
   uint32    dwFrequency;  // Sampling Frequency
   uint16    uQuality;     // Audio Quality
   uint32    dwReserved;   // Reserved for future use
} CameraAACEncodingInfo;

// Camera preview/record control APIs:
#define ICAMERA_RecordMoviePostcard(p)          AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_MOVIE,    CAM_MOVIE_POSTCARD)

// inline support functions
static __inline int ICAMERA_SetFlashCtl(ICamera *me, int32 nFlashType, int32 nFlashLevel, int32 nCustomLevel)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_FLASH_CTL, (int32)(nFlashType | nFlashLevel), nCustomLevel);
}

static __inline int ICAMERA_GetFlashCtl(ICamera *me, int32 nFlashType, int32 *pnFlashLevel, AEEParmInfo *pi)
{
   int32 nFlash = nFlashType | ((pnFlashLevel && CAM_FLASH_CUSTOM == *pnFlashLevel) ? *pnFlashLevel : 0);
   int   nErr = AEEGETPVTBL(me,ICamera)->GetParm(me, CAM_PARM_FLASH_CTL, &nFlash, (int32 *)pi);

   if (SUCCESS == nErr && pnFlashLevel)
      *pnFlashLevel = (nFlash & ~CAM_FLASHTYPE_MASK);

   return nErr;
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
CameraAACEncodingInfo

Description:
This structure specifies the information required to set the advanced
configurations for AAC encoding through video encoding

Definition:
  typedef struct CameraAACEncodingInfo
  {
    uint16    uMode;        // Input / Output AAC mode
    uint32    dwFrequency;  // Sampling Frequency
    uint16    uQuality;     // Audio Quality
    uint32    dwReserved;   // Reserved for future use
  } CameraAACEncodingInfo;

Members:
      uMode:       AAC encoding Input/Output modes
                   Range: enumerations [0,1,2] where 
                   0 -  Mono   / Mono
                   1 -  Stereo / Mono
                   2 -  Stereo / Stereo

      dwFrequency: AAC audio sampling frequency (bps)
                   Range: [8000, 11025, 16000, 22050, 32000, 44000]

      uQuality:    AAC encoding quality
                   Range: enumerations [0,1,2]  where
                   0 - Low Quality
                   1 - Medium Quality
                   2 - High Quality 

      dwReserved:  Reserved bits for future AAC mods

Comments:
   None.

See Also:
   ICAMERA_SetAudioEncode()
=============================================================================
CAM_PARM_

Description:

CAM_PARM_XXX is the format for Camera Control Parameters.
=============================================================================
CAM_PARM_FADE
Set:  Sets the fading for movie recording mode
      p1 = CAM_FADE_XXX
      if   CAM_FADE_IN:   Fade In at start of movie recording
      CAM_FADE_OUT: Fade Out at end of movie recording
      CAM_FADE_IN_OUT: Fade In at start of movie recording, Fade Out
      at end of movie recording
      CAM_FADE_OFF : No fading in Movie Recording

CAM_PARM_FRAME_TIMESTAMP
Set:  Enables encoding of timestamp (or counter) into each video frame during movie encoding.
      This should be called before starting the movie recording.      
      p1 = boolean, TRUE/FALSE=>Enable/Disable
Get:  Gets the current value.
      p1 = Pointer to boolean value

CAM_PARM_EFFECT
      CAM_EFFECT_POSTERIZE: Posterize
      CAM_EFFECT_WHITEBOARD:Whiteboard
      CAM_EFFECT_BLACKBOARD:Blackboard
      CAM_EFFECT_AQUA:      Aqua

CAM_PARM_ISO
Info: This is an exposure control parameter. This parameter controls the sensor's level of
      sensitivity to light. As the sensitivity is increased, less light is needed and the sensor output 
      is amplified (along with noise). Typical ISO values are 0(Auto), 25, 50, 100, 200, 400, 800, 1600, 3200.
      A value of 100 represents normal sensitivity.

CAM_PARM_DEBLUR
Info: Control the hand jitter blurring by maximum up to n shots for de-blurring processing.
The more shots, the longer processing time.
Additional Info: In photography, without fixation equipment such as a tripod, photographer's
slight hand vibration, the so-called hand jitter, is common and accumulates over time, which
blurs the captured image. The hand jitter blurring is severe when longer exposure is needed
due to less incoming light, close-up shot and zooming amplifies hand jitter, and mobile
cameras are likely to jitter more due to smaller aperture and one-hand handling.
On the other side, shorter exposure means unnoticeable jitter but darker image.
The hand jitter reduction solution is as follows, multiple shorter exposure images are taken
rapidly and sequentially, the motion vectors between neighboring frames are calculated, and all
frames are registered according to the calculated motion vectors, and finally the merged
images are processed through adaptive sharpening filter to enhance edge and further reduce noise.

Set: Enables and sets the number of shots to be processed for deblur.
     p1 = uint32. 0: disabled,
                  > 0: Number of shots to process
Get: Gets the current value.
     p1 = uint32 *. Pointer to current value
     p2 = AEEParmInfo *

CAM_PARM_LUMA_ADAPTATION
Info: Enable/Disable the luma adaptation in the high dynamic range in preview. When the parameter
is set to enabled, luma adaptation creates a non-linear tone reproduction curve which also
automatically varies according to the image's luminance distribution. The variable adjustment keeps
the contrast of the image while showing both shadow and bright details in the original image.
Set:  Enable/Disable luma adaptation.
      p1 = boolean. TRUE => Enabled.
Get:  Gets the current value
      p1 = boolean *

CAM_PARM_FLASH_CTL
Info: Allows flash control. User specifies the flash type: LED or strobe. Multiple
      flash types can be controlled at the same time.
      In addition, for each flash type, following flash level options are supported:
      CAM_FLASH_AUTO:   Auto
      CAM_FLASH_OFF:    Off
      CAM_FLASH_LOW:    Low intensity
      CAM_FLASH_MEDIUM: Medium intensity
      CAM_FLASH_HIGH:   High intensity
      CAM_FLASH_CUSTOM: Custom value (p2) provided by user
Set:  Sets the flash type and level. This operation is done in any of the camera modes.
      p1 = (CAM_FLASHTYPE_XXX | CAM_FLASH_XXX)
      p2 = uint32 value for CAM_FLASH_CUSTOM
Get:  Gets the current value.
      Input: p1 = CAM_FLASHTYPE_XXX Or (CAM_FLASHTYPE_XXX| CAM_FLASH_CUSTOM)
      Output:p1 = Pointer to current value
             p2 = AEEParmInfo *, if p1 (as input) contains CAM_FLASH_CUSTOM

CAM_PARM_AF_INFO
Set:  Sets the camera auto focus parameters
      p1 = CAM_AF_MODE_XXX. Following AF modes are defined
              CAM_AF_MODE_NORMAL:Normal mode
              CAM_AF_MODE_MACRO: Enable macro mode
              CAM_AF_MODE_FULL_RANGE: Enable full range auto focus
      p2 = CAM_AF_METER_XXX. Following AF metering values are defined
              CAM_AF_METER_CENTER_SPOT:    Focus based on center spot metered
              CAM_AF_METER_CENTER_WEIGHTED:Focus based on center weighted metered
              CAM_AF_METER_AVERAGE:        Focus based on average metered
Get:  Gets the auto focus parameters
      p1 = Pointer to CAM_AF_MODE_XXX
      p2 = Pointer to CAM_AF_METER_XXX

=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

ICAMERA_RecordMoviePostcard()

Description:
    This function starts the camera operation in movie mode and records a movie postcard. The recorded
frames to be sent to the caller while encoding those frames. Movie postcard has only one frame (or
same frame repeated) with a background audio.

Prototype:
    int ICAMERA_RecordMoviePostcard(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    EBADPARM: Bad parm is passed
    ENOMEMORY: Not enough memory
    EBADSTATE: RecordMovie() cannot be done in current state

Comments:
    You need to set the media data before calling this function. Also, you may want
to specify active encoding, picture format and quality that should be used for encoding
the movie.
    All the events that originate due to this API and due to the following APIs
will be reported via the user-specified callback:
~
    ICAMERA_Stop()~
    ICAMERA_Pause()~
    ICAMERA_Resume()~
    CAM_STATUS_START callback happens once the recording begins.~
    CAM_STATUS_FRAME callbacks happen continuously unless you pause.~
    CAM_STATUS_DONE callback occurs when recording is stopped.~
    CAM_STATUS_ABORT callback occurs when recording is aborted. In the callback,
AEECameraNotify,~
===pre>
    nCmd = CAM_CMD_START ~
    nSubCmd = CAM_MODE_MOVIE.
===/pre>

Version:
   Introduced BREW Client 3.1.5

See Also:
    ICAMERA_Start()
    ICAMERA_Stop()
    ICAMERA_Pause()
    ICAMERA_Resume()
    ICAMERA_GetFrame()

=============================================================================

ICAMERA_SetFlashCtl()

Description:
    This function enables device flash control. User can set flash type and level.

Prototype:
   int ICAMERA_SetFlashCtl(ICamera *pICamera, int32 nFlashType, int32 nFlashLevel, int32 nCustomLevel)

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nFlashType:  Specifies flash type. CAM_FLASHTYPE_XXX
    nFlashLevel: Flash level: CAM_FLASH_XXX
    nCustomLevel: If nFlashLevel is CAM_FLASH_CUSTOM, this specifies the level

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure. Current focus type may not be CAM_FOCUS_AUTO

Comments:
   To get flash control, use ICAMERA_GetFlashCtl() API.
    See Camera Control Parameters for CAM_PARM_FLASH_CTL details.

Version:
   Introduced BREW Client 3.1.5

See Also:
    ICAMERA_GetFlashCtl()

=============================================================================

ICAMERA_GetFlashCtl()

Description:
    This function enables device flash control. User can set flash type and level.

Prototype:
   int ICAMERA_GetFlashCtl(ICamera *me, int32 nFlashType, int32 *pnFlashLevel, AEEParmInfo *pi)

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nFlashType:  Specifies flash type. CAM_FLASHTYPE_XXX
    pnFlashLevel [out]: Pointer to current flash level: CAM_FLASH_XXX
    pi [out]: If nFlashLevel is CAM_FLASH_CUSTOM, this specifies the level range info

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure. Current focus type may not be CAM_FOCUS_AUTO

Comments:
   To set flash control, use ICAMERA_SetFlashCtl() API.
    See Camera Control Parameters for CAM_PARM_FLASH_CTL details.

Version:
   Introduced BREW Client 3.1.5

See Also:
    ICAMERA_SetFlashCtl()

============================================================================= */
#endif //ifndef CAM_PARM_FRAME_TIMESTAMP

//Port extra definitions for 3.1.4 and 3.1.5
#define CAM_STATUS_UPDATE               (CAM_STATUS_BASE + 12)  // [Preview] Ready to update histogram.

#define CAM_PARM_ENCODESNAPSHOT_EX      (CAM_PARM_QCOM_RESERVED_BEGIN + 1)
#define CAM_PARM_FRAME_EX               (CAM_PARM_QCOM_RESERVED_BEGIN + 2)
#define CAM_PARM_TRANSITION             (CAM_PARM_QCOM_RESERVED_BEGIN+3)
#ifndef CAM_PARM_BESTSHOT
#define CAM_PARM_BESTSHOT               (CAM_PARM_CTL_BASE + 130)// [Set/Get] Best shot mode
#endif //CAM_PARM_BESTSHOT
#define CAM_PARM_BRACKETING             (CAM_PARM_CTL_BASE + 131)// [Set/Get] Bracketing mode
#define CAM_PARM_DIGITAL_ZOOM_ENABLE    (CAM_PARM_CTL_BASE + 132)// [Set/Get] Enable/Disable digital zoom
#define CAM_PARM_AUTO_FOCUS_REGION_LIST (CAM_PARM_CTL_BASE + 133)// [Get] Auto focus regions
#define CAM_PARM_SHUTTER_CALLBACK       (CAM_PARM_CTL_BASE + 134)// [Get] Shutter open/close state and time stamp
#define CAM_PARM_HISTOGRAM              (CAM_PARM_CTL_BASE + 135)// [Set] Preview histogram
//Hue, Saturation setting
#define CAM_PARM_HUE                    (CAM_PARM_CTL_BASE + 136)// [Set/Get] Hue
#define CAM_PARM_SATURATION             (CAM_PARM_CTL_BASE + 137)// [Set/Get] Saturation
#define CAM_PARM_SPACE_LIMIT            (CAM_PARM_CTL_BASE + 138)// [Set/Get] Set the space limit for video recording
// Direct Mode Enable
#define CAM_PARM_LCD_DIRECT_ACCESS_EX   (CAM_PARM_CTL_BASE + 139)// [Set/Get] Enable/Disable Direct Mode Display
// DIS for Camcorder
#define CAM_PARM_DIS                    (CAM_PARM_CTL_BASE + 140)// [Set] DIS
// stitch action in panoramic mode
#define CAM_PARM_STITCH                 (CAM_PARM_CTL_BASE + 141)// [Set] Stitch specified snapshots 
// panoramic mode setting
#define CAM_PARM_PANORAMIC              (CAM_PARM_CTL_BASE + 142)// [Set] Enable/Disable Panoramic mode

#define CAM_PARM_FRAMES_DROPPED         (CAM_PARM_CTL_BASE + 143)// [Get] Passes clip stats(now only frames dropped) to the UI.

#define CAM_PARM_USER_ATOM              (CAM_PARM_CTL_BASE + 144)// [Set] Set user atom parameters

#define CAM_PARM_MULTISHOT_MODE         (CAM_PARM_CTL_BASE + 145)// [Set] Set user atom parameters
#define CAM_PARM_SHUTTER_SOUND          (CAM_PARM_CTL_BASE + 146)// [Set] Set Shutter Sound for ACC

#ifndef CAM_BESTSHOT_BASE
//CAM_PARM_BESTSHOT settings
#define CAM_BESTSHOT_BASE           0
#define CAM_BESTSHOT_OFF            (CAM_BESTSHOT_BASE)      // Turn off bestshot mode
#define CAM_BESTSHOT_LANDSCAPE      (CAM_BESTSHOT_BASE + 1)  // Landscape/scenery
#define CAM_BESTSHOT_SNOW           (CAM_BESTSHOT_BASE + 2)  // Snow
#define CAM_BESTSHOT_BEACH          (CAM_BESTSHOT_BASE + 3)  // Beach
#define CAM_BESTSHOT_SUNSET         (CAM_BESTSHOT_BASE + 4)  // Sunset
#define CAM_BESTSHOT_NIGHT          (CAM_BESTSHOT_BASE + 5)  // Night
#define CAM_BESTSHOT_PORTRAIT       (CAM_BESTSHOT_BASE + 6)  // Portrait
#define CAM_BESTSHOT_BACKLIGHT      (CAM_BESTSHOT_BASE + 7)  // Backlight
#define CAM_BESTSHOT_SPORTS         (CAM_BESTSHOT_BASE + 8)  // Sports
#define CAM_BESTSHOT_ANTISHAKE      (CAM_BESTSHOT_BASE + 9)  // Antishake
#define CAM_BESTSHOT_FLOWERS        (CAM_BESTSHOT_BASE + 10) // Flowers
#define CAM_BESTSHOT_CANDLE_LIGHT   (CAM_BESTSHOT_BASE + 11) // Candle light
#endif //CAM_BESTSHOT_BASE
#ifdef CAM_BESTSHOT_BASE 
#define CAM_BESTSHOT_PARTY   (CAM_BESTSHOT_BASE + 12) // Party mode
#endif
//
//CAM_PARM_BRACKETING settings
//
#define CAM_BRACKETING_BASE         0
#define CAM_BRACKETING_OFF          (CAM_BRACKETING_BASE)            // Turn off bracketing mode
#define CAM_BRACKETING_AE           (CAM_BRACKETING_BASE + 1)        // Bracketing for auto exposure
#define CAM_BRACKETING_AF           (CAM_BRACKETING_BASE + 2)        // Bracketing for auto focus
#define CAM_BRACKETING_AWB          (CAM_BRACKETING_BASE + 3)        // Bracketing for auto whitebalance
//
//Histogram reqire mode
#define CAM_HISTOGRAM_OFF           0 // Turn off preview histogram 
#define CAM_HISTOGRAM_LUMA          1 // Display Luma histogram on preview
#define CAM_HISTOGRAM_RGB           2 // Display RGB histogram on preview
#define CAM_HISTOGRAM_BOTH          3 // Display both Luma and RGB histograms on preview

// CAM_PARM_TRANSITION settings
#define CAM_TRANSITION_CURTAIN      (CAM_FADE_BASE + 4) // horizon curtains
#define CAM_TRANSITION_ALPHA        (CAM_FADE_BASE + 5) // alpha transition
#define CAM_TRANSITION_OFF          (CAM_FADE_BASE + 6) // transition off

#ifndef CAM_ISO_AUTO
//Special ISO settings
#define CAM_ISO_AUTO                0
#define CAM_ISO_AUTO_DEBLUR         1 
#endif //CAM_ISO_AUTO

/* CAM_PARM_SPACE_LIMIT settings */
#define CAM_SPACELIMIT_BASE         0
#define CAM_SPACELIMIT_DEFAULT      (CAM_SPACELIMIT_BASE)
#define CAM_SPACELIMIT_LOW          (CAM_SPACELIMIT_BASE + 1)
#define CAM_SPACELIMIT_MED          (CAM_SPACELIMIT_BASE + 2)
#define CAM_SPACELIMIT_HIGH         (CAM_SPACELIMIT_BASE + 3)

#undef  CAM_AF_METER_CENTER_SPOT
#undef  CAM_AF_METER_CENTER_WEIGHTED
#undef  CAM_AF_METER_AVERAGE

#define CAM_AF_METER_AUTO              1
#define CAM_AF_METER_CENTER_SPOT       2 // Focus based on center spot metered
#define CAM_AF_METER_CENTER_WEIGHTED   3 // Focus based on center weighted metered
#define CAM_AF_METER_AVERAGE           4 // Focus based on average metered

// p2 of Direct Mode parameter
typedef struct CameraDirectMode 
{
   AEECLSID clsDisplay; // Destination display
   int      nLayer; // -1: Background, +1: Foreground
   int32   xDst;    // Destination x. Maps to x_offset in camera_set_lcd_direct_mode()
   int32   yDst;    // Destination y. Maps to y_offset in camera_set_lcd_direct_mode()
   int32   cxDst;  // Destination width. Maps to width in camera_set_lcd_direct_mode()
   int32   cyDst;  // Destination height. Maps to height in camera_set_lcd_direct_mode()
} CameraDirectMode;

// parameter for ICAMERA_RecordSnapshotEx 
typedef struct CameraSnapshotMulti
{
  int      nCount;     // Number of snapshots
  uint32   dwInterval; // Suggested interval between snapshots. Zero means burst mode.
} CameraSnapshotMulti;

typedef struct CameraShutterStatus
{
  boolean  bOpen;
  uint64   ullTimeStampMS;
} CameraShutterStatus;

//
// psm points to CameraSnapshotMulti. If it is NULL, it defaults to ICAMERA_RecordSnapshot()
// Callback sequence:
// When app calls ICAMERA_RecordSnapshotEx(),
//    - A callback is received for each frame. CAM_STATUS_FRAME, AEECameraNotify::*pData contains an int (the frame index).
//    - {CAM_STATUS_DONE, CAM_CMD_START, CAM_MODE_SNAPSHOT, *pData = int (Total # of snapshots)
//    - {CAM_STATUS_DONE, CAM_CMD_ENCODESNAPSHOT, *pData = int (index of encoded snapshot)
//
static __inline int ICAMERA_RecordSnapshotEx(ICamera *me, CameraSnapshotMulti *psm)
{
   return AEEGETPVTBL(me,ICamera)->Start(me, CAM_MODE_SNAPSHOT, (uint32)psm);
}

static __inline int ICAMERA_GetFrameEx(ICamera *me, int nIndex, IBitmap **ppFrame)
{
   return AEEGETPVTBL(me,ICamera)->GetParm(me, CAM_PARM_FRAME_EX, (int32 *)nIndex, (int32 *)ppFrame);
}

static __inline int ICAMERA_EncodeSnapshotEx(ICamera *me, int nIndex)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_ENCODESNAPSHOT_EX, (int32)nIndex, 0);
}

static __inline int ICAMERA_DoStitch(ICamera *me, int num, int overlap)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_STITCH, (int32)num, (int32)overlap);
}


static __inline int ICAMERA_SetHue(ICamera *me, int nIndex)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_HUE, (int32)nIndex, 0);
}

static __inline int ICAMERA_SetSaturation(ICamera *me, int nIndex)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_SATURATION, (int32)nIndex, 0);
}

#define ICAMERA_SetSpaceLimit(p, q)             ICAMERA_SetParm(p, CAM_PARM_SPACE_LIMIT,   (int32)(q),      0)

#define ICAMERA_SetDIS(p, q)                    ICAMERA_SetParm(p, CAM_PARM_DIS,      (int32)(q),       0)

static __inline void ICAMERA_SetResolution(ICamera *me, int nWidth, int nHeight)
{
   AEESize eSize;
   eSize.cx = nWidth;
   eSize.cy = nHeight;
   AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_SIZE, (int32) (&eSize),0);
   AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_DISPLAY_SIZE, (int32) (&eSize),0);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
CameraSnapshotMulti

Description:
This structure specifies the information required to call ICAMERA_RecordSnapshotEx

Definition:
  typedef struct CameraSnapshotMulti
  {
    int      nCount;     // Number of snapshots
    uint32   dwInterval; // Suggested interval between snapshots. 
  } CameraSnapshotMulti;

Members:
      nCount:      Number of snapshots in one time.
                   It is limited by the VFE used in the target.
                   For 6K targets, it is limited to 16.
                   For 7K targets, it is limited to 8.

      dwInterval:  Interval between snapshot frames in ms.
                   Zero means burst mode.
                   Suggest not exceed 1000.

Comments:
   None.

See Also:
   None.

=======================================================================
CameraDirectMode

Description:
This structure specifies the information required to enable LCD direct mode

Definition:
  typedef struct CameraDirectMode
  {
    AEECLSID clsDisplay;
    int     nLayer;
    int32   xDst;
    int32   yDst;
    int32   cxDst;
    int32   cyDst;
  } CameraDirectMode;

Members:
  clsDisplay: Destination display id.
              0: primary LCD.
              1: secondary LCD

  nLayer:     MDP display layer.
              -1: MDP Layer 0, lowest layer for background.
              0:  MDP Layer 1, middle layer.
              1:  MDP layer 2, higher layer for foreground

  xDst:       Destination x. Maps to x_offset in camera_set_lcd_direct_mode()
  yDst:       Destination y. Maps to y_offset in camera_set_lcd_direct_mode()
  cxDst:      Destination width. Maps to width in camera_set_lcd_direct_mode()
  cyDst:      Destination height. Maps to height in camera_set_lcd_direct_mode()

Comments:
  LCD direct mode feature is implemented by MDP display. If target does not support
  MDP feature, this feature should be turned off as well.

See Also:
   None.
=====================================================================
CAM_PARM_

Description:

CAM_PARM_XXX is the format for Camera Control Parameters.
=============================================================================
CAM_PARM_BESTSHOT
Set:  Enables selected best shot effect in taking pictures.
      This should be called before taking the picture.      
      p1 = int32, best shot index defined in CAM_BESTSHOT_xxx
Get:  Gets the current value.
      p1 = int32, current best shot index

CAM_PARM_BRACKETING
Set:  Specifies bracketing mode while taking multishot pictures.
      p1 = int32, bracketing mode defined in CAM_BRACKETING_xxx
Get:  Gets the current value.
      p1 = int32, current bracketing mode

CAM_PARM_DIGITAL_ZOOM_ENABLE
Set:  Enable/Disable the digital zoom.
      p1 = boolean, 0 - Disabled, 1 - Enabled
      Default is enabled.
Get:  Gets the current control state.
      p1 = boolean, current digital zoom state

CAM_PARM_AUTO_FOCUS_REGION_LIST
Get:  Gets the auto focus regions.
      p1 = int32, number of focus rectangles
      p2 = AEERect *, pointer to focus rectangle array

CAM_PARM_SHUTTER_CALLBACK
Get:  Gets the shutter close/open state.
      p1 = boolean, TRUE - opened, FALSE - closed
      p2 = int32, time stamp for the shutter close/open in ms

CAM_PARM_HISTOGRAM
Set:  Sets the preview histogram display on/off
  [in]p1 = int32, histogram mode defined in CAM_HISTOGRAM_xxx
  [in]p2 = IBitmap *, pointer to histogram bitmap interface.
           The pBmp buffer is allocated and freed by UI. 
           It should be in RGB 565 color format. 

CAM_PARM_TRANSITION
Set:  Set movie recording transition (effective between pause/resume's)
      p1 = CAM_TRANSITION_XXX
      if   CAM_TRANSITION_ALPHA:    alpha blend transition
           CAM_TRANSITION_CURTAIN:  sideways curtain transition
           CAM_TRANSITION_OFF:      turn off all transitions

CAM_PARM_HUE
Set:  Sets the hue of the picture to be recorded.
      This operation is done in any of the camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_SATURATION
Set:  Sets the saturation of the picture to be recorded.
      This operation is done in any of the camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *
	  
CAM_PARM_SPACE_LIMIT
Set:  Sets the user specified space limit / duration while recording
      a video clip	  
      p1 = int32 value

CAM_PARM_LCD_DIRECT_ACCESS_EX
Set:  Sets the LCD direct mode On/Off
      p1 = boolean. TRUE/FALSE => Enable/Disable LCD direct-access mode.
      p2 = CameraDirectMode *, if p1 is FALSE, p2 could be NULL.
Get:  Gets the current mode status.
      p1 = Pointer to current mode

CAM_PARM_DIS
Set:  Sets the dis settings as per user selection (off-medium-high) in preview
      and toggles dis on-off in recording
      p1 = int32 value

CAM_PARM_THUMBNAIL_INFO
Set:  Sets thumbnail size, quality and decides whether to encode Thumbnail or not
      p1 = CameraThumbnailInfo *, Sets size, quality.
      p2 = bCtl, boolean TRUE/FALSE => This decides to enode Thumbnail or not.
Get:  Gets the Current value.
      p1 = Pointer to current value.
      p2 = TRUE (Encode Thumbnail) , FALSE(Don't Encode Thumbnail).       

CAM_PARM_STITCH
Set:  Enables/Disables stitch action in panoramic mode
      p1 = Number or snapshot frames to stitch. 
           0 means no stitch(camera could clean up snapshot buffer before returning
           to preview)  
      p2 = Fator of transpaent snapshot overlay on next preview. 
           It is denominate of the %. 4 means 1/4 of overlap on full image. 
           It could be 0 if p1 is 0. Otherwise, it must larger than 1.

CAM_PARM_PANORAMIC
Set:  Enables/Disables panoramic mode
      p1 = Notifies if the camera is in normal mode or panoramic mode
           1 means Panoramic, 0 means normal mode

=============================================================================
  CAM_STATUS_
=============================================================================
Definition:
  CAM_STATUS_UPDATE                [Preview] Ready to update histogram.

Members:
   None

Comments:
   None

See Also:
   None

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 
===H2>
Set Preview Histogram Call Flow:
===/H2>
mode:
<Y, RGB, YRGB>   ---ICAMERA_SetParm(mode, IBitmap)-->
                                                  ---camera_set_parm_2(mode, buffer struct)->
                                                  <-------DONE-ready for update histogram----
                 <-----CAM_CMD_SETPARM/CAM_STATUS_UPDATE---
                              :
                              :
                              :
mode <OFF>        ---ICAMERA_SetParm(mode, NULL)-->
                                                  ---camera_set_parm_2(mode, null)->
                                                  <-------DONE-ready for clear buffer----
                 <-----CAM_CMD_SETPARM/CAM_STATUS_DONE---


==============================================================================================*/

#endif // AEECAMERAEXT_H
