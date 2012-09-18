#ifndef AEECAMERA_H
#define AEECAMERA_H

/*============================================================================
FILE:   AEECamera.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file defines ICamera interface that controls the device camera.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright (c) 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEMedia.h"
#include "AEEBitmap.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//
// Status codes
//
// Status code is returned via registered callback function to
// indicate event status and to return data to client.
// AEECameraNotify::nStatus sent via callback function contains
// the following status codes.
//
#define CAM_STATUS_BASE                1                       // Base used by ICamera
#define CAM_STATUS_USER_BASE           0x8000                  // Base for extension

#define CAM_STATUS_START               (CAM_STATUS_BASE)       // [Preview/Record] Operation started successfully
#define CAM_STATUS_DONE                (CAM_STATUS_BASE + 1)   // [Preview/Record/SetParm/GetParm/EncodeSnapshot] Operation completed successfully
                                                               // For RecordSnapShot, pData = TRUE/FALSE => Defered encode enabled/disabled
                                                               // For EncodeSnapshot, pData = Number of bytes encoded
#define CAM_STATUS_FAIL                (CAM_STATUS_BASE + 2)   // [Preview/Record/SetParm/GetParm/EncodeSnapshot] Operation failed, pData = CAM_EXXX error code.
#define CAM_STATUS_ABORT               (CAM_STATUS_BASE + 3)   // [Any] Current operation aborted: Camera entered ready state
#define CAM_STATUS_FRAME               (CAM_STATUS_BASE + 4)   // [Any] Frame captured by camera.
#define CAM_STATUS_PAUSE               (CAM_STATUS_BASE + 5)   // [Preview/Record] Record movie paused
#define CAM_STATUS_RESUME              (CAM_STATUS_BASE + 6)   // [Preview/Record] Record movie resumed
                                                               //                  AEECameraNotify::pData is IBitmap pointer representing the snapshot
#define CAM_STATUS_DATA_IO_DELAY       (CAM_STATUS_BASE + 7)   // [Preview/Record] Operation being delayed by data i/o access
#define CAM_STATUS_SPACE_WARNING       (CAM_STATUS_BASE + 8)   // [Record] Memory available to store recording running low, pData contains CAM_ERR_SPACE_MSG_XXX
#define CAM_STATUS_SPACE_ERROR         (CAM_STATUS_BASE + 9)   // [Record] Memory unavailable to store recording, pData contains CAM_ERR_SPACE_MSG_XXX
#define CAM_STATUS_FILE_SIZE_EXCEEDED  (CAM_STATUS_BASE + 10)  // [EncodeSnapshot/RecordMovie] Operation completed but output encoded image exceeds specified file size
#define CAM_STATUS_IO_ERROR            (CAM_STATUS_BASE + 11)  // [EncodeSnapshot/RecordMovie] I/O Error, pData contains CAM_ERR_XXX
#ifdef FEATURE_DSP
#define CAM_STATUS_UPDATE               (CAM_STATUS_BASE + 12)  // [Preview] Ready to update histogram.
#endif
#define CAM_ERR_BASE                   1        // Base used by ICamera
#define CAM_ERR_USER_BASE              0x1000   // Base for derived class

#define CAM_ERR_ALREADY_OPEN           (CAM_ERR_BASE + 1)   // File/Stream already open
#define CAM_ERR_OPEN                   (CAM_ERR_BASE + 2)   // File/Stream failed to open or not open
#define CAM_ERR_ALREADY_EXIST          (CAM_ERR_BASE + 3)   // File/Stream already exists
#define CAM_ERR_NOT_EXIST              (CAM_ERR_BASE + 4)   // File/Stream does not exist
#define CAM_ERR_BAD_NAME               (CAM_ERR_BASE + 5)   // File/Stream bad name
#define CAM_ERR_BAD_HANDLE             (CAM_ERR_BASE + 6)   // File/Stream bad handle
#define CAM_ERR_READ                   (CAM_ERR_BASE + 7)   // File/Stream read error
#define CAM_ERR_WRITE                  (CAM_ERR_BASE + 8)   // File/Stream write error
#define CAM_ERR_FS                     (CAM_ERR_BASE + 9)   // General file system error
#define CAM_ERR_SPACE_MSG_SAMPLE_TABLE     (CAM_ERR_BASE + 10)  // Sample table is reaching limit
#define CAM_ERR_SPACE_MSG_CHUNK_TABLE      (CAM_ERR_BASE + 11)  // Chunk table is reaching limit
#define CAM_ERR_SPACE_MSG_STORAGE          (CAM_ERR_BASE + 12)  // Storage (i.e. Flash, MMC etc) is reaching limit
#define CAM_ERR_SPACE_MSG_MOVIE_DURATION   (CAM_ERR_BASE + 13)  // Movie duration is reaching limit
#define CAM_ERR_SPACE_MSG_VIDEO            (CAM_ERR_BASE + 14)  // Video is reaching limit

//
// Command codes
//
// Command code is returned via registered callback function to
// indicate event type and to return data to client.
// AEECameraNotify::nStatus in the GetStatus() function called from callback function
// contains the following command codes.
//
#define CAM_CMD_BASE                1        // Base used by ICamera
#define CAM_CMD_USER_BASE           0x1000   // Base for derived class

#define CAM_CMD_SETPARM             (CAM_CMD_BASE)     // SetParm(), nSubCmd = nParmID
#define CAM_CMD_GETPARM             (CAM_CMD_BASE + 1) // GetParm(), nSubCmd = nParmID
#define CAM_CMD_START               (CAM_CMD_BASE + 2) // Start(), nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_SNAPSHOT/CAM_MODE_MOVIE
#define CAM_CMD_ENCODESNAPSHOT      (CAM_CMD_BASE + 3) // EncodeSnapshot()
#ifdef FEATURE_DSP
#define CAM_MODE_UPDATE             (CAM_MODE_BASE + 4)   //update screen after ui paint
#endif
//
// Camera Control Parms
//
// Set/Get ICamera parms using ICAMERA_SetParm()/ICAMERA_GetParm().
// Setting/Getting camera parms may result in async callback events. If
// this happens, then function will return CAM_PENDING status.
//
// IMPORTANT NOTE:
//    For documentation of these parameters, please refer to
//    "Camera Control Parameters (CAM_PARM_XXX)" section under
//    documentation (scroll down a few pages).
//
#ifdef FEATURE_DSP
#define TRANS_COLOR                    0x001f
#endif
#define CAM_PARM_BASE               1                       // Base used by ICamera
#define CAM_PARM_USER_BASE          0x8000                  // Base for extension

#define CAM_PARM_MEDIA_DATA         (CAM_PARM_BASE)         // [Set/Get] Destination media data
#define CAM_PARM_VIDEO_ENCODE       (CAM_PARM_BASE + 1)     // [Set/Get] Video/image encoding type
#define CAM_PARM_AUDIO_ENCODE       (CAM_PARM_BASE + 2)     // [Set/Get] Audio encoding type
#define CAM_PARM_SIZE               (CAM_PARM_BASE + 3)     // [Set/Get] Size of image to be encoded
#define CAM_PARM_DISPLAY_SIZE       (CAM_PARM_BASE + 4)     // [Set/Get] Size of image to be displayed
#define CAM_PARM_DEFER_ENCODE       (CAM_PARM_BASE + 5)     // [Set/Get] Defer encoding

#define CAM_PARM_MODE               (CAM_PARM_BASE + 6)     // [Get] Camera mode
#define CAM_PARM_IS_SUPPORT         (CAM_PARM_BASE + 7)     // [Get] Checks if specific parm is supported
#define CAM_PARM_IS_MOVIE           (CAM_PARM_BASE + 8)     // [Get] Checks if movie recording is supported
#define CAM_PARM_PIXEL_COUNT        (CAM_PARM_BASE + 9)     // [Get] Pixel count
#define CAM_PARM_VIDEO_ENCODE_LIST  (CAM_PARM_BASE + 10)    // [Get] List of video encoding types supported
#define CAM_PARM_AUDIO_ENCODE_LIST  (CAM_PARM_BASE + 11)    // [Get] List of audio encoding types supported
#define CAM_PARM_SIZE_LIST          (CAM_PARM_BASE + 12)    // [Get] List of image sizes supported
#define CAM_PARM_DISPLAY_SIZE_LIST  (CAM_PARM_BASE + 13)    // [Get] List of display sizes supported
#define CAM_PARM_FPS_LIST           (CAM_PARM_BASE + 14)    // [Get] List of FPS supported

#define CAM_PARM_OVERLAY            (CAM_PARM_BASE + 15)    // [Set] Overlay image to be encoded or clear overlays
#define CAM_PARM_GPSINFO            (CAM_PARM_BASE + 16)    // [Set] GPS info to be encoded
#define CAM_PARM_EXIF_IFDTAG        (CAM_PARM_BASE + 17)    // [Set] Set Exchangeable Image File Format (EXIF 2.2+) tags

#define CAM_PARM_LCD_DIRECT_ACCESS  (CAM_PARM_BASE + 18)    // [Set/Get] Camera services layer renders frames directly on LCD
#define CAM_PARM_PREVIEW_TYPE       (CAM_PARM_BASE + 19)    // [Set/Get] Hint if preview is for snapshot or movie
#define CAM_PARM_THUMBNAIL_INFO     (CAM_PARM_BASE + 20)    // [Set/Get] Thumbnail info
#define CAM_PARM_SENSOR_INFO        (CAM_PARM_BASE + 21)    // [Set/Get] Sensor info
#define CAM_PARM_PREVIEW_FPS        (CAM_PARM_BASE + 22)    // [Set/Get] FPS control in preview mode
#define CAM_PARM_FRAME_TIMESTAMP   (CAM_PARM_BASE + 23)     // [Set/Get] Enable encoding of time stamp in movie frames
#define  CAM_PARM_DISPLAY_OFFSET    (CAM_PARM_BASE + 24)
#define  CAM_PARM_SENSOR_MODEL     (CAM_PARM_BASE + 25)   //add by xuhui

#define CAM_PARM_CTL_BASE           (CAM_PARM_BASE + 0x100)
#define CAM_PARM_QUALITY            (CAM_PARM_CTL_BASE)     // [Set/Get] Quality
#define CAM_PARM_FPS                (CAM_PARM_CTL_BASE + 1) // [Set/Get] Frames per second
#define CAM_PARM_CONTRAST           (CAM_PARM_CTL_BASE + 2) // [Set/Get] Contrast
#define CAM_PARM_BRIGHTNESS         (CAM_PARM_CTL_BASE + 3) // [Set/Get] Brightness
#define CAM_PARM_SHARPNESS          (CAM_PARM_CTL_BASE + 4) // [Set/Get] Sharpness
#define CAM_PARM_ZOOM               (CAM_PARM_CTL_BASE + 5) // [Set/Get] Digital Zoom
#define CAM_PARM_ROTATE_PREVIEW     (CAM_PARM_CTL_BASE + 6) // [Set/Get] Rotate: affects preview only
#define CAM_PARM_ROTATE_ENCODE      (CAM_PARM_CTL_BASE + 7) // [Set/Get] Rotate while encoding: affects snapshot and movie modes. Preview is not rotated.
#define CAM_PARM_EXPOSURE           (CAM_PARM_CTL_BASE + 8) // [Set/Get] Exposure
#define CAM_PARM_WHITE_BALANCE      (CAM_PARM_CTL_BASE + 9) // [Set/Get] White balance
#define CAM_PARM_EFFECT             (CAM_PARM_CTL_BASE + 10)// [Set/Get] Effect
#define CAM_PARM_FLASH              (CAM_PARM_CTL_BASE + 11)// [Set/Get] Flash control. Deprecated.
#define CAM_PARM_RED_EYE_REDUCTION  (CAM_PARM_CTL_BASE + 12)// [Set/Get] Red-Eye reduction control
#define CAM_PARM_REFLECT            (CAM_PARM_CTL_BASE + 13)// [Set/Get] Reflection
#define CAM_PARM_FILE_SIZE          (CAM_PARM_CTL_BASE + 14)// [Set/Get] JPEG file size encoding target
#define CAM_PARM_NIGHTSHOT_MODE     (CAM_PARM_CTL_BASE + 15)// [Set/Get] Nightshot mode enable/disable
#define CAM_PARM_ANTIBANDING        (CAM_PARM_CTL_BASE + 16)// [Set/Get] Enable fluorecent light anti-banding
#define CAM_PARM_FOCUS              (CAM_PARM_CTL_BASE + 17)// [Set/Get] Set the focus
#define CAM_PARM_FOCUS_RECT         (CAM_PARM_CTL_BASE + 18)// [Set/Get] Set the focus rectangle
#define CAM_PARM_BITRATE            (CAM_PARM_CTL_BASE + 19)// [Set/Get] Set the encoding bit rate for video
#define CAM_PARM_QCOM_RESERVED_BEGIN (CAM_PARM_CTL_BASE + 20)     // Reserved parameter ID's begin
#define CAM_PARM_MULTISHOT          (CAM_PARM_CTL_BASE + 21)// [Set]  Multishot 
#define CAM_PARM_BANDING            (CAM_PARM_CTL_BASE + 22)// [Set] Camera Banding
#define CAM_PARM_PREVIEWWITHFRAME   (CAM_PARM_CTL_BASE + 23)// [Set] Camera Preview with frame or not
#define CAM_PARM_WB                 (CAM_PARM_CTL_BASE + 24) // [Set/Get] Frames per second

#define CAM_PARM_FADE               CAM_PARM_QCOM_RESERVED_BEGIN //[Set] Set the fading for movie recording mode
#define CAM_PARM_QCOM_RESERVED_END  (CAM_PARM_QCOM_RESERVED_BEGIN + 100) // Reserved parameter ID's end
#define CAM_PARM_EXPOSURE_METERING  (CAM_PARM_CTL_BASE + 121)// [Set/Get] Set exposure metering
#define CAM_PARM_EXPOSURE_COMP      (CAM_PARM_CTL_BASE + 122)// [Set/Get] Set exposure compensation
#define CAM_PARM_ISO                (CAM_PARM_CTL_BASE + 123)// [Set/Get] Set sensor sensitivity
#define CAM_PARM_APERTURE           (CAM_PARM_CTL_BASE + 124)// [Set/Get] Set aperture
#define CAM_PARM_SHUTTER_SPEED      (CAM_PARM_CTL_BASE + 125)// [Set/Get] Set shutter speed
#define CAM_PARM_DEBLUR             (CAM_PARM_CTL_BASE + 126)// [Set/Get] Hand jitter reduction
#define CAM_PARM_LUMA_ADAPTATION    (CAM_PARM_CTL_BASE + 127)// [Set/Get] Luma Adaptation
#define CAM_PARM_FLASH_CTL          (CAM_PARM_CTL_BASE + 128)// [Set/Get] Flash control
#define CAM_PARM_AF_INFO            (CAM_PARM_CTL_BASE + 129) // [Set/Get] Set the auto focus info
#define CAM_PARM_BESTSHOT           (CAM_PARM_CTL_BASE + 130)// [Set/Get] Best shot mode
//#define CAM_PARM_FLASH              (CAM_PARM_CTL_BASE + 131) //
#define CAM_PARM_CURRENT_SENSOR		(CAM_PARM_QCOM_RESERVED_BEGIN + 1)// [Set/Get] Current sensor
/*
** 保持和camsensor_sensor_model_pair_type一致
*/
typedef enum
{
	CAM_PARM_ID_PAIR_START=0,
	CAM_PARM_ID_PAIR_SIV121A_SID130B = CAM_PARM_ID_PAIR_START,
	CAM_PARM_ID_PAIR_SID130B_SIV121A,
	CAM_PARM_ID_PAIR_SIV121D_SID130B,
	CAM_PARM_ID_PAIR_SID130B_SIV121D,
	CAM_PARM_ID_PAIR_SP0838_SP0A18,
	CAM_PARM_ID_PAIR_SP0A18_SP0838,
	CAM_PARM_ID_PAIR_SIV120A_SID130B,
	CAM_PARM_ID_PAIR_SID130B_SIV120A,
	CAM_PARM_ID_PAIR_SIV121A_SIV121A,
	CAM_PARM_ID_PAIR_GC0329_GC0329,
	CAM_PARM_ID_PAIR_END
}cam_sensor_model_pair_type;
//
// Reference values for CAM_PARM_SIZE:
//
// VGA              640 x 480
// QVGA             320 x 240
// QQVGA            160 x 120
// CIF              352 x 288
// QCIF             176 x 144
//

//
// Camera encoding. In addition to these values, AEECLSID of media formats can also
// be used -- for example, AEECLSID_JPEG or AEECLSID_MEDIAMPEG4.
//
#define CAM_ENCODE_BASE                   1                     // Encode base used by ICamera
#define CAM_ENCODE_RAW                    (CAM_ENCODE_BASE)     // Raw format -- catch all
#define CAM_ENCODE_YCBCR                  (CAM_ENCODE_BASE + 1) // YCbCr format
#define CAM_ENCODE_RGB565                 (CAM_ENCODE_BASE + 2) // BREW IDIB RGB565 color-scheme
#define CAM_ENCODE_UUID                   (CAM_ENCODE_BASE + 3) // UUID
#define CAM_ENCODE_UUID_DATA              (CAM_ENCODE_BASE + 4) // UUID data
#define CAM_ENCODE_SHORT_HEADER           (CAM_ENCODE_BASE + 5) // Short header
#define CAM_ENCODE_UUID_LIST_ATOM_INFO    (CAM_ENCODE_BASE + 6) // UUID atom data
//
// Camera States
//
#define CAM_MODE_BASE               1                     // Base used by ICamera
#define CAM_MODE_USER_BASE          0x1000                // Base used for extension

#define CAM_MODE_READY              (CAM_MODE_BASE)       // Ready state. Camera is ready for operation.
#define CAM_MODE_PREVIEW            (CAM_MODE_BASE + 1)   // Preview state or start subcmd. Camera starts sending preview frames
#define CAM_MODE_SNAPSHOT           (CAM_MODE_BASE + 2)   // Record mode: snapshot or start subcmd
#define CAM_MODE_MOVIE              (CAM_MODE_BASE + 3)   // Record mode: movie or start subcmd

//
// CAM_MODE_MOVIE specific
//
#define CAM_MOVIE_BASE              0
#define CAM_MOVIE_NORMAL            (CAM_MOVIE_BASE)     // Normal movie
#define CAM_MOVIE_POSTCARD          (CAM_MOVIE_BASE +1)  // Postcard movie

//Fading

#define CAM_FADE_BASE                   (0x0)     // Fading Base
#define CAM_FADE_USER_BASE              (0x1000)  // Fading Base used for extension

#define CAM_FADE_OFF                    (CAM_FADE_BASE) // Fade off
#define CAM_FADE_IN                     (CAM_FADE_BASE + 1) // Fade In On
#define CAM_FADE_OUT                    (CAM_FADE_BASE + 2) // Fade Out On
#define CAM_FADE_IN_OUT                 (CAM_FADE_BASE + 3) // Fade In Out On


//
// Exposure (See documentation of CAM_PARM_EXPOSURE)
//
#define CAM_EXPOSURE_BASE              1                       // Base used by ICamera
#define CAM_EXPOSURE_USER_BASE         0x1000                  // Base used for extension

#define CAM_EXPOSURE_AUTO              (CAM_EXPOSURE_BASE)     // Auto
#define CAM_EXPOSURE_DAY               (CAM_EXPOSURE_BASE + 1) // For subjects under day light
#define CAM_EXPOSURE_NIGHT             (CAM_EXPOSURE_BASE + 2) // For subjects in dark environments
#define CAM_EXPOSURE_LANDSCAPE         (CAM_EXPOSURE_BASE + 3) // For distant subjects
#define CAM_EXPOSURE_STRONG_LIGHT      (CAM_EXPOSURE_BASE + 4) // For subjects in strong or reflected light
#define CAM_EXPOSURE_SPOTLIGHT         (CAM_EXPOSURE_BASE + 5) // For subjects in spotlight
#define CAM_EXPOSURE_PORTRAIT          (CAM_EXPOSURE_BASE + 6) // For subjects behind an obstacle
#define CAM_EXPOSURE_MOVING            (CAM_EXPOSURE_BASE + 7) // For moving subjects

//
// Effect (See documentation of CAM_PARM_EFFECT)
//
#define CAM_EFFECT_BASE                1                       // Base used by ICamera
#define CAM_EFFECT_USER_BASE           0x1000                  // Base used for extension

#define CAM_EFFECT_OFF                 (CAM_EFFECT_BASE)       // No special effect
#define CAM_EFFECT_MONO                (CAM_EFFECT_BASE + 1)   // Black and white
#define CAM_EFFECT_NEGATIVE            (CAM_EFFECT_BASE + 2)   // Color and brightness reversed
#define CAM_EFFECT_SOLARIZE            (CAM_EFFECT_BASE + 3)   // Light intensity emphasized
#define CAM_EFFECT_PASTEL              (CAM_EFFECT_BASE + 4)   // Contrast emphasized
#define CAM_EFFECT_MOSAIC              (CAM_EFFECT_BASE + 5)   // Mosaic
#define CAM_EFFECT_RESIZE              (CAM_EFFECT_BASE + 6)   // Stretch along x or y. Aspect ratio not preserved.
#define CAM_EFFECT_SEPIA               (CAM_EFFECT_BASE + 7)   // Sepia effect
#define CAM_EFFECT_POSTERIZE           (CAM_EFFECT_BASE + 8)   // Posterize
#define CAM_EFFECT_WHITEBOARD          (CAM_EFFECT_BASE + 9)   // Whiteboard
#define CAM_EFFECT_BLACKBOARD          (CAM_EFFECT_BASE + 10)  // Blackboard
#define CAM_EFFECT_AQUA                (CAM_EFFECT_BASE + 11)  // Aqua

//
// White Balance (See documentation of CAM_PARM_WHITE_BALANCE)
//
#define CAM_WB_BASE                    1                 // Base used by ICamera
#define CAM_WB_USER_BASE               0x1000            // Base used for extension

#define CAM_WB_AUTO                    (CAM_WB_BASE)     // Auto
#define CAM_WB_CUSTOM                  (CAM_WB_BASE + 1) // Custom value provided by user
#define CAM_WB_INCANDESCENT            (CAM_WB_BASE + 2) // For subjects under incandescent lighting
#define CAM_WB_TWILIGHT                (CAM_WB_BASE + 6) // For subjects under low light or dark conditions
#define CAM_WB_FLUORESCENT             (CAM_WB_BASE + 3) // For subjects under fluorescent lighting
#define CAM_WB_DAYLIGHT                (CAM_WB_BASE + 4) // For subjects under sunlight, strong or varying light conditions
#define CAM_WB_CLOUDY_DAYLIGHT         (CAM_WB_BASE + 5) // For subjects under cloudy daylight conditions
#define CAM_WB_SHADE                   (CAM_WB_BASE + 7) // For subjects under shade

//
// Flash control (See documentation of CAM_PARM_FLASH_CTL)
//
#define CAM_FLASHTYPE_MASK            0xFF000000   // Flash type mask
#define CAM_FLASHTYPE_DEFAULT         0x00000000   // Default flash
#define CAM_FLASHTYPE_LED             0x01000000   // LED flash
#define CAM_FLASHTYPE_STROBE          0x02000000   // Strobe flash

//
// Flash level (See documentation of CAM_PARM_FLASH_CTL)
//
#define CAM_FLASH_BASE                 1                 // Base used by ICamera
#define CAM_FLASH_USER_BASE            0x1000            // Base used for extension

#define CAM_FLASH_AUTO                 (CAM_FLASH_BASE)     // Auto
#define CAM_FLASH_OFF                  (CAM_FLASH_BASE + 1) // Off
#define CAM_FLASH_LOW                  (CAM_FLASH_BASE + 2) // Low intensity
#define CAM_FLASH_MEDIUM               (CAM_FLASH_BASE + 3) // Medium intensity
#define CAM_FLASH_HIGH                 (CAM_FLASH_BASE + 4) // High intensity
#define CAM_FLASH_CUSTOM               (CAM_FLASH_BASE + 5) // Custom value provided by user

//Preview control
#define CAM_PREVIEW_BASE                    0                 // Base used by ICamera
#define CAM_PREVIEW_USER_BASE               0x1000            // Base used for extension

#define CAM_PREVIEW_SNAPSHOT              (CAM_PREVIEW_BASE)     // Snapshot
#define CAM_PREVIEW_MOVIE                 (CAM_PREVIEW_BASE + 1) // Movie

//Antibanding control
#define CAM_ANTIBANDING_BASE           0                 // Base used by ICamera
#define CAM_ANTIBANDING_USER_BASE      0x1000            // Base used for extension
#define CAM_ANTIBANDING_OFF           (CAM_ANTIBANDING_BASE)     // No antibanding
#define CAM_ANTIBANDING_60HZ          (CAM_ANTIBANDING_BASE+1)   // 60 Hertz antibanding
#define CAM_ANTIBANDING_50HZ          (CAM_ANTIBANDING_BASE+2)   // 50 Hertz antibanding

// Focus types
#define CAM_FOCUS_BASE                 0                 // Base used by ICamera
#define CAM_FOCUS_USER_BASE            0x1000            // Base used for extension

#define CAM_FOCUS_AUTO                 (CAM_FOCUS_BASE)     // Auto focus
#define CAM_FOCUS_MANUAL               (CAM_FOCUS_BASE + 1) // Manual focus. Specify value

//
// Auto Focus Settings (p1 of CAM_PARM_AF_INFO)
//
#define CAM_AF_MODE_NORMAL                0  // Normal mode
#define CAM_AF_MODE_MACRO                 1  // Enable Macro mode
#define CAM_AF_MODE_FULL_RANGE            2  // Enable full range auto focus

//
// Auto Focus metering (p2 of CAM_PARM_AF_INFO)
//
#define CAM_AF_METER_CENTER_SPOT       0 // Focus based on center spot metered
#define CAM_AF_METER_CENTER_WEIGHTED   1 // Focus based on center weighted metered
#define CAM_AF_METER_AVERAGE           2 // Focus based on average metered

// Exposure Metering
#define CAM_EXPOSURE_METERING_BASE              0                 // Base used by ICamera
#define CAM_EXPOSURE_METERING_USER_BASE         0x1000            // Base used for extension
#define CAM_EXPOSURE_METERING_MATRIX            (CAM_EXPOSURE_METERING_BASE)     // Average exposure over the frame
#define CAM_EXPOSURE_METERING_CENTER_WEIGHTED   (CAM_EXPOSURE_METERING_BASE + 1) // Center weighted exposure
#define CAM_EXPOSURE_METERING_SPOT              (CAM_EXPOSURE_METERING_BASE + 2) // Spot metering

// Special ISO settings
#define CAM_ISO_AUTO                0
#define CAM_ISO_AUTO_DEBLUR         1 

// Reflection types
#define CAM_REFLECT_BASE              0                  // Base used by ICamera
#define CAM_REFLECT_USER_BASE         0x1000             // Base used for extension
#define CAM_REFLECT_DISABLE           (CAM_REFLECT_BASE)    // Reflection is disabled
#define CAM_REFLECT_MIRROR            (CAM_REFLECT_BASE + 1)// Mirror reflection
#define CAM_REFLECT_WATER             (CAM_REFLECT_BASE + 2)// Water reflection
#define CAM_REFLECT_MAX               (CAM_REFLECT_BASE + 8)// Max pre-defined reflection
//
//============== Begin: Exchangeable Image File Format (EXIF 2.2+) Related ===========
//

// IFD
#define CAM_EXIF_IFD_DEFAULT              0xffff   // BREW uses the most appropriate IFD
#define CAM_EXIF_IFD_ZERO                 0        // 0th IFD
#define CAM_EXIF_IFD_FIRST                1        // 1st IFD
#define CAM_EXIF_IFD_EXIF                 2        // EXIF IFD
#define CAM_EXIF_IFD_GPSINFO              3        // GPSInfo IFD
#define CAM_EXIF_IFD_INTEROP              4        // Interoperability IFD

// Tag Data Types
#define CAM_EXIF_TAGTYPE_BYTE             1        // 8-bit unsigned integer
#define CAM_EXIF_TAGTYPE_ASCII            2        // 8-bit byte containing one 7-bit ASCII code. The final byte is terminated with NULL
#define CAM_EXIF_TAGTYPE_SHORT            3        // 16-bit unsigned integer
#define CAM_EXIF_TAGTYPE_LONG             4        // 32-bit unsigned integer
#define CAM_EXIF_TAGTYPE_RATIONAL         5        // Two LONGs. The first LONG is the numerator and the second LONG expresses the denominator
#define CAM_EXIF_TAGTYPE_UNDEFINED        7        // 8-bit byte that can take any value depending on the field definition
#define CAM_EXIF_TAGTYPE_SLONG            9        // 32-bit signed integer (2's complement notation)
#define CAM_EXIF_TAGTYPE_SRATIONAL        10       // Two SLONGs. The first SLONG is the numerator and the second SLONG is the denominator

//
// Tags
//
// 0th IFD TagIDs
#define CAM_EXIF_TAG_ImageDescription        0x010e   // [Type:String] Image title

// EXIF IFD TagIDs
#define CAM_EXIF_TAG_UserComment             0x9286   // [Type:Undefined] User comments

// GPSInfo TagIDs
#define CAM_EXIF_TAG_GPSAreaInformation      0x001c   // [Type:String] Name of GPS area

//
//============== End: Exchangeable Image File Format (EXIF 2.2+) Related ===========
//

//File Format types
#define CAM_FILE_BRAND_MP4             0
#define CAM_FILE_BRAND_AMC             1
#define CAM_FILE_BRAND_3GP             2
#define CAM_FILE_BRAND_3G2             3
#define CAM_FILE_BRAND_K3G             4
#define CAM_FILE_BRAND_3G2_FRAG        5
#define CAM_FILE_BRAND_SKM             6

#define CAM_SHORT_HEADER_DISABLED      0
#define CAM_SHORT_HEADER_ENABLED       1
#define CAM_SHORT_HEADER_TEST          2

#define CAM_FILE_FORMAT_MPG4           "mp4"
#define CAM_FILE_FORMAT_3GP            "3gp"
#define CAM_FILE_FORMAT_3G2            "3g2"
#define CAM_FILE_FORMAT_K3G            "k3g"
#define CAM_FILE_FORMAT_AMC            "amc"
#define CAM_FILE_FORMAT_3G2_FRAG       "3g2f"
#define CAM_FILE_FORMAT_SKM            "skm"

//
// AAC encoding info
//
// I/O
#define CAM_AAC_IO_MONO_MONO           0
#define CAM_AAC_IO_STEREO_MONO         1
#define CAM_AAC_IO_STEREO_STEREO       2

// Quality
#define CAM_AAC_QUALITY_LOW            0
#define CAM_AAC_QUALITY_MEDIUM         1
#define CAM_AAC_QUALITY_HIGH           2

//
//CAM_PARM_BESTSHOT settings
//
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

// TagInfo specification structure
//
typedef struct CameraExifTagInfo
{
   uint16   wTagID;     // [Mandatory] TagID
   uint16   wTagType;   // [Mandatory] Tag data type
   void *   pTagData;   // [Mandatory] Tag data
   uint32   dwBytes;    // [Mandatory] Number of bytes in pTagData

   uint16   wIFDID;     // [Optional] IFD ID. Use Default for most cases
   uint16   wReserved;  // Reserved
} CameraExifTagInfo;

//
//============== End: EXIF Related ===================================
//

//
// Specifies finite equally spaced discrete values
//
typedef struct AEEParmInfo
{
   int32    nMin;
   int32    nMax;
   int32    nStep;
   int32    nDefault;
   int32    nCurrent;
} AEEParmInfo;

//
// Real numbers are represented as fixed point integers.
// Precision is 3 decimal digits, i.e., all values are multiplied by 1000.
//
#define AEEFixedPointParmInfo    AEEParmInfo
#define AEEFixedPointMinMax      AEEFixedPointParmInfo   // Only nMin and nMax fields are valid. Ignore the rest.
#define AEEFixedPointParm        AEEFixedPointParmInfo   // Only nCurrent represents the value of parm. Ignore the rest.

typedef struct CameraThumbnailInfo
{
   int16    nStructSize;
   AEESize  size;          // Size of thumbnail
   int16    nQuality;      // Quality %. Typically 50%: Low, 75%: Medium, 100%: High
} CameraThumbnailInfo;

typedef struct CameraSensorInfo
{
   uint32      dwSize;        // Size of this struct
   AECHAR      szName[32];    // Name of the sensor
   AEESize     size;          // Max Width and Height of sensor
   uint32      dwFPSPreview;  // Max FPS of sensor in preview mode
   uint32      dwFPSMovie;    // Max FPS of sensor in movie mode
   flg         bLandscape:1;  // Sensor orientation
} CameraSensorInfo;

typedef struct CameraData
{
   void *   pData;      // [Mandatory] data
   uint32   dwBytes;    // [Mandatory] Number of bytes in pData
} CameraData;

// Focus Rect Info
typedef struct CameraFocusRectInfo
{
   uint32   dwStructSize;  // Size of this struct

   int32    dxStep;        // Focus step in x direction
   int32    dyStep;        // Focus step in y direction

   AEERect  rectFocusRegion;// Region within which focus window can move
} CameraFocusRectInfo;

// UUID Atom Info
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

typedef struct ICamera  ICamera;

// AEECameraNotify structure is returned via AEECallback
typedef struct AEECameraNotify
{
   ICamera *      pCam;
   int16          nCmd;
   int16          nSubCmd;
   int16          nStatus;
   int16          nReserved;
   void *         pData;
   uint32         dwSize;
} AEECameraNotify;

typedef void (*PFNCAMERANOTIFY)(void * pUser, AEECameraNotify * pNotify);

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//********************************************************************************************************************************
//
// ICamera Interface
//
//********************************************************************************************************************************
AEEINTERFACE(ICamera)
{
   INHERIT_IQueryInterface(ICamera);

   int         (*RegisterNotify)(ICamera * po, PFNCAMERANOTIFY pfnNotify, void * pUser);

   int         (*SetParm)(ICamera * po, int16 nParmID, int32 p1, int32 p2);
   int         (*GetParm)(ICamera * po, int16 nParmID, int32 * pP1, int32 * pP2);

   int         (*Start)(ICamera * po, int16 nMode, uint32 dwParam);
   int         (*Stop)(ICamera * po);
   int         (*Pause)(ICamera * po, boolean bPause);

   int         (*GetFrame)(ICamera * po, IBitmap ** ppFrame);
   int         (*EncodeSnapshot)(ICamera * po);
};

#define ICAMERA_AddRef(p)                       AEEGETPVTBL(p, ICamera)->AddRef(p)
#define ICAMERA_Release(p)                      AEEGETPVTBL(p, ICamera)->Release(p)
#define ICAMERA_QueryInterface(p, i, p2)        AEEGETPVTBL(p, ICamera)->QueryInterface(p, i, p2)

#define ICAMERA_RegisterNotify(p, pfn, pu)      AEEGETPVTBL(p, ICamera)->RegisterNotify(p, pfn, pu)

#define ICAMERA_SetParm(p, c, p1, p2)           AEEGETPVTBL(p, ICamera)->SetParm(p, c, p1, p2)
#define ICAMERA_GetParm(p, c, pp1, pp2)         AEEGETPVTBL(p, ICamera)->GetParm(p, c, pp1, pp2)

// Camera preview/record control APIs:
#define ICAMERA_Start(p, m, dw)                 AEEGETPVTBL(p, ICamera)->Start(p, m, dw)
#define ICAMERA_Preview(p)                      AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_PREVIEW,  0)
#define ICAMERA_RecordSnapshot(p)               AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_SNAPSHOT, 0)
#define ICAMERA_RecordMovie(p)                  AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_MOVIE,    CAM_MOVIE_NORMAL)
#define ICAMERA_RecordMoviePostcard(p)          AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_MOVIE,    CAM_MOVIE_POSTCARD)
#ifdef FEATURE_DSP
#define ICAMERA_UpdateScreen(p, dw)             AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_UPDATE, dw)
#endif
#define ICAMERA_Stop(p)                         AEEGETPVTBL(p, ICamera)->Stop(p)
#define ICAMERA_Pause(p)                        AEEGETPVTBL(p, ICamera)->Pause(p, TRUE)
#define ICAMERA_Resume(p)                       AEEGETPVTBL(p, ICamera)->Pause(p, FALSE)
#define ICAMERA_GetFrame(p, ppf)                AEEGETPVTBL(p, ICamera)->GetFrame(p, ppf)
#define ICAMERA_EncodeSnapshot(p)               AEEGETPVTBL(p, ICamera)->EncodeSnapshot(p)
#define ICAMERA_DeferEncode(p, b)               ICAMERA_SetParm(p, CAM_PARM_DEFER_ENCODE, (int32)(b), 0)

//
// Commonly used APIs to set/get ICamera parm
//
// Camera record info APIs:
#define ICAMERA_SetMediaData(p, pmd, ppsz)      ICAMERA_SetParm(p, CAM_PARM_MEDIA_DATA,   (int32)(pmd),     (int32)(ppsz))
#define ICAMERA_SetVideoEncode(p, cls, dw)      ICAMERA_SetParm(p, CAM_PARM_VIDEO_ENCODE, (int32)(cls),     (int32)(dw))
#define ICAMERA_SetAudioEncode(p, cls, dw)      ICAMERA_SetParm(p, CAM_PARM_AUDIO_ENCODE, (int32)(cls),     (int32)(dw))
#define ICAMERA_SetSize(p, ps)                  ICAMERA_SetParm(p, CAM_PARM_SIZE,         (int32)(ps),      0)
#define ICAMERA_SetQuality(p, q)                ICAMERA_SetParm(p, CAM_PARM_QUALITY,      (int32)(q),       0)
#define ICAMERA_SetBitRate(p, q)                ICAMERA_SetParm(p, CAM_PARM_BITRATE,      (int32)(q),       0)
#define ICAMERA_SetFramesPerSecond(p, v)        ICAMERA_SetParm(p, CAM_PARM_FPS,          (int32)(v),       0)
#define ICAMERA_SetContrast(p, v)               ICAMERA_SetParm(p, CAM_PARM_CONTRAST,     (int32)(v),       0)
#define ICAMERA_SetBrightness(p, v)             ICAMERA_SetParm(p, CAM_PARM_BRIGHTNESS,   (int32)(v),       0)
#define ICAMERA_SetSharpness(p, v)              ICAMERA_SetParm(p, CAM_PARM_SHARPNESS,    (int32)(v),       0)
#define ICAMERA_SetZoom(p, v)                   ICAMERA_SetParm(p, CAM_PARM_ZOOM,         (int32)(v),       0)
#define ICAMERA_SetEffect(p,v)                  ICAMERA_SetParm(p, CAM_PARM_EFFECT,       (int32)(v),       0)
#define ICAMERA_SetWb(p,v)                      ICAMERA_SetParm(p, CAM_PARM_WB,           (int32)(v),       0)
#define ICAMERA_SetMaxFileSize(p, s)            ICAMERA_SetParm(p, CAM_PARM_FILE_SIZE,    (int32)(s),       0)
#define ICAMERA_RotatePreview(p, v)             ICAMERA_SetParm(p, CAM_PARM_ROTATE_PREVIEW,(int32)(v),      0)
#define ICAMERA_RotateEncode(p, v)              ICAMERA_SetParm(p, CAM_PARM_ROTATE_ENCODE,(int32)(v),       0)
#define ICAMERA_AddOverlay(p, pb)               ICAMERA_SetParm(p, CAM_PARM_OVERLAY,      (int32)(pb),      0)
#define ICAMERA_ClearOverlay(p)                 ICAMERA_SetParm(p, CAM_PARM_OVERLAY,      0,                0)
#define ICAMERA_ControlFlash(p,b)               ICAMERA_SetParm(p, CAM_PARM_FLASH,        (int32)b,         0)


// Camera display API:
#define ICAMERA_SetDisplaySize(p, pr)           ICAMERA_SetParm(p, CAM_PARM_DISPLAY_SIZE, (int32)(pr),      0)

#define ICAMERA_GetMode(p, pm, pb)              ICAMERA_GetParm(p, CAM_PARM_MODE,         (int32 *)(pm),    (int32 *)(pb))

// Camera capabilities APIs:
#define ICAMERA_IsSupport(p, parm, pb)          ICAMERA_GetParm(p, CAM_PARM_IS_SUPPORT,   (int32 *)(parm),                 (int32 *)(pb))
#define ICAMERA_IsContrast(p, pb)               ICAMERA_GetParm(p, CAM_PARM_IS_SUPPORT,   (int32 *)(CAM_PARM_CONTRAST),    (int32 *)(pb))
#define ICAMERA_IsBrightness(p, pb)             ICAMERA_GetParm(p, CAM_PARM_IS_SUPPORT,   (int32 *)(CAM_PARM_BRIGHTNESS),  (int32 *)(pb))
#define ICAMERA_IsSharpness(p, pb)              ICAMERA_GetParm(p, CAM_PARM_IS_SUPPORT,   (int32 *)(CAM_PARM_SHARPNESS),   (int32 *)(pb))
#define ICAMERA_IsZoom(p, pb)                   ICAMERA_GetParm(p, CAM_PARM_IS_SUPPORT,   (int32 *)(CAM_PARM_ZOOM),        (int32 *)(pb))
#define ICAMERA_IsMovie(p, pb)                  ICAMERA_GetParm(p, CAM_PARM_IS_MOVIE,     (int32 *)(pb),                   NULL)
#define ICAMERA_GetSizeList(p, ppl, pb)         ICAMERA_GetParm(p, CAM_PARM_SIZE_LIST,    (int32 *)(ppl),                  (int32 *)(pb))
#define ICAMERA_GetDisplaySizeList(p, ppl, pb)  ICAMERA_GetParm(p, CAM_PARM_DISPLAY_SIZE_LIST,(int32 *)(ppl),              (int32 *)(pb))

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
#define ICAMERA_GetCurrentSensor(p, ppl, pb)    ICAMERA_GetParm(p, CAM_PARM_CURRENT_SENSOR,(int32 *)(ppl),                 (int32 *)(pb))
#endif

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
AEECameraNotify

Description:
This structure contains information of an event generated by ICamera
object. It is sent via the registered callback function.

Definition:
   typedef struct AEECameraNotify
   {
      ICamera *      pCam;
      int16          nCmd;
      int16          nSubCmd;
      int16          nStatus;
      int16          nReserved;
      void *         pData;
      uint32         dwSize;
   } AEECameraNotify;

Members:
   pCam:    ICamera object originating this callback
   nCmd:    Command code. CAM_CMD_XXX
   nSubCmd: Sub command code (see comments)
   nStatus: Status code. CAM_STATUS_XXX
   nReserved: Reserved field
   pData:   Context-based data
   dwSize:  Context-based data size

Comments:

   If nCmd = CAM_CMD_SETPARM/CAM_CMD_GETPARM, then nSubCmd will be nParmID (CAM_PARM_XXX).

   If nCmd = CAM_CMD_START, then nSubCmd will be CAM_MODE_PREVIEW/CAM_MODE_SNAPSHOT/CAM_MODE_MOVIE.

See Also:
   ICAMERA_RegisterNotify,
   ICAMERA_SetParm(),
   ICAMERA_GetParm(),
   ICAMERA_Start()

=============================================================================

CameraExifTagInfo

Description:
This structure specifies Exchangeable Image File Format (EXIF 2.2+) tag info
to be encoded in the image.

Definition:
   typedef struct CameraExifTagInfo
   {
      uint16   wTagID;     // [Mandatory] TagID
      uint16   wTagType;   // [Mandatory] Tag data type
      void *   pTagData;   // [Mandatory] Tag data
      uint32   dwBytes;    // [Mandatory] Number of bytes in pTagData

      uint16   wIFDID;     // [Optional] IFD ID. Use Default for most cases
      uint16   wReserved;  // Reserved
   } CameraExifTagInfo;

Members:
   wTagID:   [Mandatory] TagID
   wTagType: [Mandatory] Tag data type
   pTagData: [Mandatory] Tag data
   dwBytes:  [Mandatory] Number of bytes in pTagData
   wIFDID:   [Optional] IFD ID. Use Default for most cases
   wReserved:Reserved

Comments:
   None.

See Also:
   ICAMERA_SetParm()

=============================================================================

CameraFocusRectInfo

Description:
This structure specifies the information required to set the camera focus rectangle.

Definition:
   typedef struct CameraFocusRectInfo
   {
      uint32   dwStructSize;

      int32    dxStep;
      int32    dyStep;

      AEERect  rectFocusRegion;
   } CameraFocusRectInfo;

Members:
      dwStructSize:  Size of this struct
      dxStep:        Focus step in x direction
      dyStep:        Focus step in y direction
      rectFocusRegion: Region within which focus window can move

Comments:
   None.

See Also:
   ICAMERA_SetParm()

=============================================================================

AEEParmInfo

Description:
This structure specifies finite equally spaced discrete values.

Definition:
   typedef struct AEEParmInfo
   {
      int32    nMin;
      int32    nMax;
      int32    nStep;
      int32    nDefault;
      int32    nCurrent;
   } AEEParmInfo;

Members:
   nMin: Minimum value
   nMax: Maximum value
   nStep: Increment/Decrement steps
   nDefault: Default value
   nCurrent: Current value

Comments:
   None.

See Also:
   None

=============================================================================

CameraThumbnailInfo

Description:
This structure specifies the properties of the thumbnail image saved as part
of the encoded picture.

Definition:
   typedef struct CameraThumbnailInfo
   {
      int16    nStructSize;
      AEESize  size;
      int16    nQuality;
   } CameraThumbnailInfo;

Members:
   nStructSize:   Size of this struct
   size:          Size of thumbnail
   nQuality:      Quality %. Typically 50%: Low, 75%: Medium, 100%: High

Comments:
   None.

See Also:
   None

=============================================================================

CameraSensorInfo

Description:
This structure specifies the properties of the camera sensor.

Definition:
   typedef struct CameraSensorInfo
   {
      uint32      dwSize;
      AECHAR      szName[32];
      AEESize     size;
      uint32      dwFPSPreview;
      uint32      dwFPSMovie;
      flg         bLandscape:1;
   } CameraSensorInfo;

Members:
   dwSize:        Size of this struct
   szName:        Name of the sensor
   size:          Max Width and Height of sensor
   dwFPSPreview:  Max FPS of sensor in preview mode
   dwFPSMovie:    Max FPS of sensor in movie mode
   bLandscape:    Sensor orientation

Comments:
   None.

See Also:
   None

=============================================================================
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
                   CAM_AAC_IO_MONO_MONO     : Mono/Mono
                   CAM_AAC_IO_STEREO_MONO   : Stereo/Mono
                   CAM_AAC_IO_STEREO_STEREO : Stereo/Stereo

      dwFrequency: AAC audio sampling frequency (bps)
                   Range: [8000, 11025, 16000, 22050, 32000, 44000]

      uQuality:    AAC encoding quality
                   CAM_AAC_QUALITY_LOW   : Low
                   CAM_AAC_QUALITY_MEDIUM: Medium
                   CAM_AAC_QUALITY_HIGH  : High

      dwReserved:  Reserved

Comments:
   None.

See Also:
   ICAMERA_SetAudioEncode()
=============================================================================
CAM_PARM_

Description:

CAM_PARM_XXX is the format for Camera Control Parameters.
=============================================================================
Camera Control Parameters

Description:
These parameters(CAM_PARM_XXX) allow setting/getting of camera parameters. They are used in
ICAMERA_SetParm() and ICAMERA_GetParm() APIs.
===pre>
CAM_PARM_MEDIA_DATA
Set:  Sets the media data before encoding. This is done in Ready mode before calling
      ICAMERA_RecordSnapshot() or ICAMERA_RecordMovie()
      p1 = AEEMediaData *
      p2 = MIME string (NULL terminated)
Get:  Gets the current media data.
      p1 = AEEMediaData *
      p2 = Pointer to MIME string (const char **)
      Note: String p2 should be copied and should not be freed.

CAM_PARM_VIDEO_ENCODE
CAM_PARM_AUDIO_ENCODE
Set:  Sets the active video/audio encode type for encoding. This is done in Ready mode before calling
      ICAMERA_RecordSnapshot() or ICAMERA_RecordMovie()
      p1 = uint32, CAM_ENCODE_XXX or AEECLSID of the media format
      p2 = Extra info regarding the encoding like sub formats.
Get:  Gets the current active encode type.
      p1 = Pointer to uint32, active encode type: CAM_ENCODE_XXX or AEECLSID of the media format
      p2 = Extra info regarding the encoding like sub formats.
      If *p1 == AEECLSID_MEDIAQCP, p2 returns the enumeration for the QCELP audio type
      If *p1 == AEECLSID_MEDIAAAC, p2 must be an address with allocated memory of size 
                    CameraAACEncodingInfo so that AAC audio encoding parameters can be populated.

CAM_PARM_SIZE
Set:  Sets the size of the picture to be recorded. This is done in Ready mode before calling
      ICAMERA_RecordSnapshot() or ICAMERA_RecordMovie().
      p1 = AEESize *
Get:  Gets the current picture size.
      p1 = AEESize *

CAM_PARM_DISPLAY_SIZE
Set:  Sets the frame display size for preview and movie modes.
      p1 = AEESize *
Get:  Gets the current frame display size.
      p1 = AEESize *

CAM_PARM_DEFER_ENCODE
Set:  This parm enables/disables deferring of the frame (snapshot) encoding done by ICAMERA_RecordSnapshot() API.
      p1 = boolean. TRUE => Defer enabled.
Get:  Gets the current value
      p1 = boolean *

CAM_PARM_MODE
Get:  Gets the camera mode.
      p1 = Pointer to CAM_MODE_XXX
      p2 = Pointer to boolean: TRUE/FALSE: Paused/Resumed.

CAM_PARM_IS_SUPPORT
Get:  Checks if specified parm is supported.
      p1[in] = ParmID
      p2[out] = Pointer to boolean: TRUE/FALSE => Supported/Unsupported.

CAM_PARM_IS_MOVIE
Get:  Checks if camera records movie.
      p1[out] = Pointer to boolean: TRUE/FALSE => Supported/Unsupported.

CAM_PARM_PIXEL_COUNT
Get:  Returns camera pixel count.
      p1[out] = AEESize *

CAM_PARM_VIDEO_ENCODE_LIST
CAM_PARM_AUDIO_ENCODE_LIST
Get:  Returns list of supported video/audio encoding formats.
      Output: p1 = Pointer to NULL-terminated list of AEECLSID (AEECLSID **)
      Note: The list should be copied and should not be freed.

CAM_PARM_SIZE_LIST
Get:  Returns list of discrete sizes supported or continuos range
      (e.g. any size between 10x10 to 100x150) for specified mode.
      Input: p1 = CAM_MODE_SNAPSHOT/CAM_MODE_MOVIE (*ppList must contain this value)
      Output: p1 = Pointer to NULL-size terminated list of AEESize (ppList of type AEESize **)
                   If NULL, indicates that any value is supported.
              p2 = Pointer to boolean, bRange, when TRUE indicates the passed list
                   is a NULL-terminated paired list (i.e. multiple of 2) of ranges
      Note: The list should be copied and should not be freed.

CAM_PARM_DISPLAY_SIZE_LIST
Get:  Returns list of discrete display sizes (typically for preview or movie mode) supported
      or continuos range (e.g. any size between 10x10 to 100x150) for specified mode.
      Input: p1 = CAM_MODE_PREVIEW/CAM_MODE_MOVIE (*ppList must contain this value)
      Output: p1 = Pointer to NULL-size terminated list of AEESize (ppList of type AEESize **)
                   If NULL, indicates that any value is supported.
              p2 = Pointer to boolean, bRange, when TRUE indicates the passed list
                   is a NULL-terminated paired list (i.e. multiple of 2) of ranges
      Note: The list should be copied and should not be freed.

CAM_PARM_FPS_LIST
Get:  Returns list of supported discrete frames per second(FPS) or continuos range
      (e.g. any size between 5 to 30) for specified mode.
      Input: p1 = CAM_MODE_PREVIEW/CAM_MODE_MOVIE (*ppList must contain this value)
      Output: p1 = Pointer to NULL-terminated list of uint32 dwFPS (ppList of type uint32 **).
                   See dwFPS format in CAM_PARM_FPS documentation.
                   If NULL, indicates that any value is supported.
              p2 = Pointer to boolean, bRange, when TRUE indicates the passed list
                   is a NULL-terminated paired list (i.e. multiple of 2) of ranges
      Note: The list should be copied and should not be freed.

CAM_PARM_OVERLAY
Set:  Sets the overlay image that will be part of the recorded picture. This operation is done
      any camera mode.
      p1 = IBitmap *
      Note: You can add overlays on top of another image by calling this function repeatedly with
            different images. To clear ALL overlays, call this function with p1 = 0, p2 = NULL.
Get:  Gets the current overlay info.
      p1 = IBitmap *

CAM_PARM_GPSINFO
Set: Sets AEEGPSInfo to be encoded in the image. This has to be set for each recording.
     p1 = AEEGPSInfo *

CAM_PARM_EXIF_IFDTAG
Set: Set Exchangeable Image File Format (EXIF 2.2+) tags to be encoded in the image.
     This has to be set for each recording.
     p1 = CameraExifTagInfo *

CAM_PARM_LCD_DIRECT_ACCESS
Info: This parameter specifies if camera services layer is in a mode that renders frames directly
      on the device LCD, bypassing IBitmap/IDisplay mechanism and device bitmap. This allows
      super-fast blts and enables higher FPS for preview/movie modes. If the parameter is not implemented,
      then assume that this mode is not supported.

Set:  Puts the camera in LCD direct-access mode.
      p1 = boolean. TRUE/FALSE => Enable/Disable LCD direct-access mode.
      p2 = AEESize *, where cx and cy denote the start frame position
Get:  Lets app know if camera is in LCD direct-access mode.
      p1 = Pointer to the boolean value.
      p2 = AEESize *, where cx and cy denote the start frame position

CAM_PARM_PREVIEW_TYPE
Set:  Indicates if preview is being done for snapshot or movie. This parameter helps optimize device
      camera resources/performance and should be set before starting the preview.
      This is an optional parameter.
      p1 = CAM_PREVIEW_XXX
Get:  Gets the current value.
      p1 = Pointer to current value

CAM_PARM_THUMBNAIL_INFO
Info : The properties of the thumbnail image saved as part of the encoded picture.
       The properties are thumbnail height, width and quality.
Set:  Sets the thumbnail properties.
      Value must be > 0.
      p1 = CameraThumbnailInfo *
Get:  Gets the current properties in use for the thumbnail image.
      p1 = CameraThumbnailInfo *

CAM_PARM_SENSOR_INFO
Info: Gives information about the camera sensor.
Set:  Set is not allowed.
Get:  Gets the sensor information.
      p1 = CameraSensorInfo *

CAM_PARM_PREVIEW_FPS
Info: Allows user to enable auto FPS control by setting min and max FPS for previewing.
Set:  Set min and max FPS. If min is specified, then auto FPS control is enabled and
      preview frames are generated in the specified range, min to max. If min FPS is zero, then
      FPS is fixed at max.
      p1 = AEEFixedPointMinMax *
Get:  Gets the current value and supported FPS info
      p1 = AEEFixedPointMinMax *

CAM_PARM_FRAME_TIMESTAMP
Set:  Enables encoding of timestamp (or counter) into each video frame during movie encoding.
      This should be called before starting the movie recording.      
      p1 = boolean, TRUE/FALSE=>Enable/Disable
Get:  Gets the current value.
      p1 = Pointer to boolean value

CAM_PARM_QUALITY
Set:  Sets the quality of the picture to be recorded. This is done in Ready mode before calling
      ICAMERA_RecordSnapshot() or ICAMERA_RecordMovie().
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_FPS
Set:  Sets the frames per second of the camera preview and movie modes. This is done in Ready
      mode before calling ICAMERA_Preview() or ICAMERA_RecordMovie().
      p1 = uint32 dwFPS value.
      dwFPS format: Lower 16 bits is Numerator.
                    Upper 16 bits is Denominator. Zero denominator is treated as 1.

Get:  Gets the current FPS.
      p1 = Pointer to FPS value

CAM_PARM_CONTRAST
Set:  Sets the contrast of the picture to be recorded. This operation is done in any of the
      camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_BRIGHTNESS
Set:  Sets the brightness of the picture to be recorded. This operation is done in any of the
      camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_SHARPNESS
Set:  Sets the sharpness of the picture to be recorded. Sharpness, typically, specifies the
      number of adjacent pixels to be used, by camera sensor, to compose the each pixel.
      This operation is done in any of the camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_ZOOM
Set:  Sets the zoom level of the picture to be recorded. This operation is done in any of the
      camera modes.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_ROTATE_PREVIEW
Set:  Sets the rotation angle of the picture to be previewed. This operation is done only in preview modes.
      It affects preview mode only.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_ROTATE_ENCODE
Set:  Sets the rotation angle of the picture to be recorded and encoded. This operation is
      done in snapshot or movie mode. It affects snapshot/movie modes only.
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_EXPOSURE
Info: Exposure is the amount of light on the subject. For example when recording a backlighted subject
      or a subject in snow, increase the exposure, and when recording a subject with extremely bright
      illumination such as spotlight, decrease the exposure.
      Following exposure modes are defined to automatically adjust the exposure based on the
      scene:
      CAM_EXPOSURE_AUTO:         Auto setting
      CAM_EXPOSURE_DAY:          For subjects under day light
      CAM_EXPOSURE_NIGHT:        For subjects in dark environments
      CAM_EXPOSURE_LANDSCAPE:    For distant subjects
      CAM_EXPOSURE_STRONG_LIGHT: For subjects in strong or reflected light
      CAM_EXPOSURE_SPOTLIGHT:    For subjects in spotlight
      CAM_EXPOSURE_PORTRAIT:     For subjects behind an obstacle
      CAM_EXPOSURE_MOVING:       For moving subjects
Set:  Sets the exposure. This operation is done in any of the camera modes.
      p1 = CAM_EXPOSURE_XXX
Get:  Gets the current value.
      p1 = Pointer to current value

CAM_PARM_WHITE_BALANCE
Info: White Balance adjustment is adjusting the perception of light by the camera. For example,
      the image looks blue under sunlight, and looks red under mercury lamps. Human eyes can resolve
      these problems but camera cannot resolve without making adjustments.
      Following white balance modes are supported.
      CAM_WB_AUTO:         Auto
      CAM_WB_CUSTOM:       Custom value provided by user
      CAM_WB_INCANDESCENT: For subjects under incandescent lighting
      CAM_WB_TWILIGHT:     For subjects under low light or dark conditions
      CAM_WB_FLUORESCENT:  For subjects under fluorescent lighting
      CAM_WB_DAYLIGHT:     For subjects under sunlight, strong or varying light conditions or under sodium/mercury lamps
      CAM_WB_CLOUDY_DAYLIGHT: For subjects under cloudy daylight conditions
      CAM_WB_SHADE:        For subjects under shade
Set:  Sets the white balance. This operation is done in any of the camera modes.
      p1 = CAM_WB_XXX
Get:  Gets the current value.
      Input: p1 = CAM_WB_CUSTOM or any
      Output:p1 = Pointer to current value
             p2 = AEEParmInfo *, if p1 (as input) is CAM_WB_CUSTOM

CAM_PARM_EFFECT
Info: Effect parameter allows you process the image to obtain special effects.
      Following effect types are defined:
      CAM_EFFECT_OFF:      No special effect
      CAM_EFFECT_MONO:     Black and white
      CAM_EFFECT_NEGATIVE: Color and brightness reversed
      CAM_EFFECT_SOLARIZE: Light intensity emphasized
      CAM_EFFECT_PASTEL:   Contrast emphasized
      CAM_EFFECT_MOSAIC:   Mosaic
      CAM_EFFECT_RESIZE:   Stretch along x or y. Aspect ratio not preserved.
      CAM_EFFECT_SEPIA:    Sepia effect
      CAM_EFFECT_POSTERIZE: Posterize effect by reducing the number of colors used in the image
      CAM_EFFECT_WHITEBOARD:Whiteboard effect with a predominantly "white" background
      CAM_EFFECT_BLACKBOARD:Blackboard effect with a predominantly "black" background
      CAM_EFFECT_AQUA:      Aqua. "Underwater" effect with ocean blue tint
Set:  Sets the camera effect mode.
      This operation is done in any of the camera modes.
      p1 = CAM_EFFECT_XXX
      p2 = Destination AEESize *, if p1 is CAM_EFFECT_RESIZE
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEESize *, if p1 is CAM_EFFECT_RESIZE

CAM_PARM_FLASH
Info: THIS PARAMETER IS DEPRECATED. Please Use CAM_PARM_FLASH_CTL.
      Allows flash control.
      Following flash control options are supported:
      CAM_FLASH_AUTO:   Auto
      CAM_FLASH_OFF:    Off
      CAM_FLASH_LOW:    Low intensity
      CAM_FLASH_MEDIUM: Medium intensity
      CAM_FLASH_HIGH:   High intensity
      CAM_FLASH_CUSTOM: Custom value provided by user
Set:  Sets the flash control mode. This operation is done in any of the camera modes.
      p1 = CAM_FLASH_XXX
      p2 = uint32 value for CAM_FLASH_CUSTOM
Get:  Gets the current value.
      Input: p1 = CAM_FLASH_CUSTOM or any
      Output:p1 = Pointer to current value
             p2 = AEEParmInfo *, if p1 (as input) is CAM_FLASH_CUSTOM

CAM_PARM_RED_EYE_REDUCTION
Info: Enables/Disables red-eye reduction capability.
Set:  Sets red-eye enable/disable parameter
      p1 = boolean: TRUE/FALSE => Enable/Disable
Get:  Gets the current value.
      Input: p1 = boolean *

CAM_PARM_REFLECT
Info: Allows user to set image reflection type.
Set:  Sets reflection type.
      p1 = Reflection type. Please see the following explanation:
           (1) A value of Zero means reflection is disabled.
           (2) If p1 is less than CAM_REFLECT_MAX, then it is a pre-defined reflection type as defined in CAM_PARM_REFLECT_XXX.
               This is typically the most commonly supported implementation.
           (3) Optionally, this parameter allows the flexibility of specifying custom reflection as follows:
               If p1 is equal to or greater than CAM_REFLECT_MAX, then it specifies the mirror position in angle(degrees) with reference
               X-axis, anti-clockwise as follows: Imagine a circle around the camera frame and a mirror with its
               center touching tangentially to the circle. As you move this mirror along the circle, maintaining its tangential position,
               various reflections are possible. Following table gives few mirror positions and corresponding reflection types
               0:   Reflection disabled
               1  to (CAM_REFLECT_MAX - 1): Pre-defined reflection types. See CAM_REFLECT_XXX
               90:  Reflection on right of frame (mirror reflection)
               180: Reflection on top of frame
               270: Refection on left of frame (mirror reflection)
               360: Reflection on bottom of frame (water reflection)
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *, if #3 (see above) is supported

CAM_PARM_FILE_SIZE
Set:  Sets the max target file size of the encoded file.
      This typically applies to JPEG format.
      p1 = int32 value in units kilobytes. If zero, CAM_PARM_QUALITY will be used for encoding.
Get:  Gets the current value and can return AEEParmInfo if non-null
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_NIGHTSHOT_MODE
Set:  Enables/Disables the nightshot mode of the camera.
      p1 = TRUE/FALSE
      if TRUE : Enables the nightshot mode
         FALSE : Disables the nightshot mode.
Get:  Gets the current value of the nightshot mode.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_ANTIBANDING
Info: Allows fluorescent light antibanding control. Following antibanding options are supported:
      CAM_ANTIBANDING_OFF     =     No antibanding
      CAM_ANTIBANDING_60HZ    =     60 Hertz antibanding
      CAM_ANTIBANDING_50HZ    =     50 Hertz antibanding
Set:  Sets the antibanding mode. This operation is done in any of the camera modes.
      p1 = CAM_ANTIBANDING_XXX
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_FOCUS
Set:  Sets the camera focus to a specified mode
      p1 = CAM_FOCUS_XXX
      p2 = int32 focus info.
           For CAM_FOCUS_MANUAL, contains the focus value.
Get:  Gets the current value and can return AEEParmInfo if non-null
      p1 = Pointer to CAM_FOCUS_XXX
      p2 = AEEParmInfo *

CAM_PARM_FOCUS_RECT
Set:  Sets the camera focus rectangle within which the sensor will focus.
      p1 = AEERect *
Get:  Gets the camera focus rectangle and associated focus rect information.
      p1 = Current AEERect *
      p2= CameraFocusRectInfo *

CAM_PARM_BITRATE
Set:  Sets the encoding bit rate of the video clip to be recorded. This is done in Ready mode
      before calling ICAMERA_RecordMovie().
      p1 = int32 value
Get:  Gets the current value.
      p1 = Pointer to current value
      p2 = AEEParmInfo *

CAM_PARM_FADE
Set:  Sets the fading for movie recording mode
      p1 = CAM_FADE_XXX
      if   CAM_FADE_IN:   Fade In at start of movie recording
      CAM_FADE_OUT: Fade Out at end of movie recording
      CAM_FADE_IN_OUT: Fade In at start of movie recording, Fade Out
      at end of movie recording
      CAM_FADE_OFF : No fading in Movie Recording

CAM_PARM_EXPOSURE_METERING
Info: This is an exposure control parameter. The metering mode enables camera to calculate
      the optimal EV (exposure value) for that mode by measuring the amount of light on the scene.
      NOTE: After this parameter is set, CAM_EXPOSURE_AUTO will use the new value.
      Following metering modes are supported:
      CAM_EXPOSURE_METERING_CENTER_WEIGHTED: Center weighted exposure
      CAM_EXPOSURE_METERING_SPOT           : Spot metering
      CAM_EXPOSURE_METERING_MATRIX         : Average exposure over the frame
Set:  Sets the specified metering mode.
      p1 = CAM_EXPOSURE_METERING_XXX.
Get:  Gets the current value
      p1 = Pointer to current value

CAM_PARM_EXPOSURE_COMP
Info: This is an exposure control parameter. This parameter refines and compensates the metering mode to
      correctly expose the image. Typical values range from -2 EV to +2 EV or -5 EV to +5EV.
      NOTE: After this parameter is set, CAM_EXPOSURE_AUTO will use the new value.
Set:  Set EV compensation
      p1 = AEEFixedPointParm *
Get:  Gets the current value and supported exposure compensation
      p1 = AEEFixedPointParm *
      p2 = AEEFixedPointParmInfo *

CAM_PARM_ISO
Info: This is an exposure control parameter. This parameter controls the sensor's level of
      sensitivity to light. As the sensitivity is increased, less light is needed and the sensor output 
      is amplified (along with noise). Typical ISO values are 0(Auto), 25, 50, 100, 200, 400, 800, 1600, 3200.
      A value of 100 represents normal sensitivity.
      NOTE: After this parameter is set, CAM_EXPOSURE_AUTO will use the new value.
Set:  Set ISO
      p1 = uint32
      Two special ISO values are defined:
      CAM_ISO_AUTO         : Auto ISO selection
      CAM_ISO_AUTO_DEBLUR  : Auto ISO selection with deblur (hand-jitter reduction)

Get:  Gets the current value and supported ISO info
      p1 = uint32 *
      p2 = AEEParmInfo *

CAM_PARM_APERTURE
Info: This is an exposure control parameter. Reserved for future use.

CAM_PARM_SHUTTER_SPEED
Info: This is an exposure control parameter. Reserved for future use.

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

CAM_PARM_BESTSHOT
Set:  Enables selected best shot mode effect in taking pictures.
      This should be called before taking the picture.      
      p1 = int32 value for best shot mode CAM_BESTSHOT_XXX
      Following best shot modes are defined:
      CAM_BESTSHOT_OFF            : Turn off bestshot mode
      CAM_BESTSHOT_LANDSCAPE      : Landscape/scenery
      CAM_BESTSHOT_SNOW           : Snow
      CAM_BESTSHOT_BEACH          : Beach
      CAM_BESTSHOT_SUNSET         : Sunset
      CAM_BESTSHOT_NIGHT          : Night
      CAM_BESTSHOT_PORTRAIT       : Portrait
      CAM_BESTSHOT_BACKLIGHT      : Backlight
      CAM_BESTSHOT_SPORTS         : Sports
      CAM_BESTSHOT_ANTISHAKE      : Antishake
      CAM_BESTSHOT_FLOWERS        : Flowers
      CAM_BESTSHOT_CANDLE_LIGHT   : Candle light

Get:  Gets the current value.
      p1 = int32, current best shot mode

===/pre>


See Also:
   None.
=============================================================================

PFNCAMERANOTIFY

Description:
This is the type specification for callback function that user must
register with the ICamera object. ICamera object sends all the events
and data to user via the registered callback function.

Definition:
   typedef void (*PFNCAMERANOTIFY)(void * pUser, AEECameraNotify * pNotify);

Members:
   pUser: User specified data pointer~
   pNotify: Callback event-specific information

Comments:
   None.

See Also:
   AEECameraNotify.

=============================================================================

 CAM_CMD_

Description:

 CAM_CMD_XXX is the format for Camera Command codes.


=============================================================================

    Camera Command codes

Description:
    Command code is returned via registered callback function to indicate event
type and to return data to client. AEECameraNotify::nStatus in the GetStatus() function
called from callback function contains the following command codes.

Definition:
    CAM_CMD_BASE:  Base used by IMedia
    CAM_CMD_USER_BASE:  Base for derived class
    CAM_CMD_SETPARM:  SetParm():  nSubCmd = nParmID
    CAM_CMD_GETPARM:  GetParm():  nSubCmd = nParmID
    CAM_CMD_START:  Start():  nSubCmd = CAM_MODE_PREVIEW, CAM_MODE_SNAPSHOT, CAM_MODE_MOVIE
    CAM_CMD_ENCODESNAPSHOT:  EncodeSnapshot()

Members:
    None

Comments:
    None

See Also:
    None

=============================================================================

 CAM_STATUS_

Description:

 CAM_STATUS_XXX is the format for Camera Status codes


=============================================================================

    Camera Status codes

Description:
    Status code is returned via registered callback function to indicate event status
and to return data to client. AEECameraNotify::nStatus sent via callback function
contains the following status codes.

Definition:
   CAM_STATUS_BASE:  Base used by ICamera
   CAM_STATUS_USER_BASE:  Base for extension
   CAM_STATUS_START:  [Preview, Record] Operation started successfully
   CAM_STATUS_DONE:  [Preview, Record, SetParm, GetParm, EncodeSnapshot] Operation completed successfully
                      For RecordSnapShot, pData = TRUE/FALSE => Defered encode enabled/disabled
   CAM_STATUS_FAIL:  [Preview, Record, SetParm, GetParm, EncodeSnapshot] Operation failed, pData = CAM_EXXX error code.
   CAM_STATUS_ABORT:  [Any] Current operation aborted: Camera entered ready state
   CAM_STATUS_FRAME:  [Any] Frame captured by camera.
   CAM_STATUS_PAUSE:  [Preview, Record] Record movie paused
   CAM_STATUS_RESUME:  [Preview, Record] Record movie resumed.
   CAM_STATUS_DATA_IO_DELAY:  [Preview, Record] Operation being delayed by data i/o access
   CAM_STATUS_SPACE_WARNING:  [Record] Memory available to store recording running low

Members:
   None

Comments:
   None

See Also:
   None
=================================================================================
AEEFixedPointParmInfo

Description:

This structure specifies finite equally spaced discrete values but the values represent real numbers. Real numbers are represented as fixed point integers. Precision is 3 decimal digits, i.e. all values are multiplied by 1000. 

Definition:

#define AEEFixedPointParmInfo    AEEParmInfo

Comments: 
none

See Also:
AEEParmInfo

=================================================================================
AEEFixedPointMinMax      

Description:

This structure specifies finite equally spaced discrete values but the values represent real numbers. Real numbers are represented as fixed point integers. Precision is 3 decimal digits, i.e. all values are multiplied by 1000. 

Definition:

#define AEEFixedPointMinMax      AEEFixedPointParmInfo  


Comments: 
In AEEFixedPointMinMax only nMin and nMax fields are valid. Ignore the rest.

See Also:
AEEParmInfo

=================================================================================
AEEFixedPointParm        

Description:

This structure specifies finite equally spaced discrete values but the values represent real numbers. Real numbers are represented as fixed point integers. Precision is 3 decimal digits, i.e. all values are multiplied by 1000. 

Definition:

#define AEEFixedPointParm        AEEFixedPointParmInfo   


Comments: 
In AEEFixedPointParm only nCurrent represents the value of parm. Ignore the rest. 

See Also:
AEEParmInfo

=============================================================================
CameraData

Description:
This struct specifies any extra information about video encoding. It is used in the function ICAMERA_SetVideoEncode

Definition:

typedef struct CameraData
{
   void *   pData;      
   uint32   dwBytes;    
} CameraData;

Members:
pData : Pointer to data buffer
dwbytes : Number of bytes in pData

Comments: None

See Also:
ICAMERA_SetVideoEncode

=================================================================
CameraEncodeUUIDAtomInfo

Description:
This struct specifies any extra information about video encoding. It is used in the function ICAMERA_SetVideoEncode

Definition:

typedef struct CameraEncodeUUIDAtomInfo
{
   byte *   pAtom;         
   uint32   dwAtomSize;    
   byte *   pData;         // data
   uint32   dwDataSize;    // Number of bytes in pData
} CameraEncodeUUIDAtomInfo;

Members:
pAtom: poiinter to buffer containing atom type
dwAtomSize : Number of bytes in pAtom
pData : Pointer to data buffer
dwDataSize : Number of bytes in pData

Comments:

 None

See Also:
ICAMERA_SetVideoEncode
=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

ICamera Interface

Description:
   ICamera interface provides a generic way to BREW applications to
   control a device camera and to record snapshots and movies in various
   formats like JPEG, MPEG4, and so forth. ICamera is a privileged class.
   You need to specify the class ID in the MIF.

===H2>
   Event Notification:
===/H2>

   ICamera asynchronously notifies all the events to client app via the
   callback function. The app must register a callback notification function
   using ICAMERA_RegisterNotify().
===H2>
   Display:
===/H2>

   ICamera dispatches the captured frames via user-registered callback
   function in preview, snapshot, and movie modes. The frame is deliverd via CAM_STATUS_FRAME
   callback. In the callback, the user needs to call ICAMERA_GetFrame() to get the frame
   represented by IBitmap.

   It is app's responsibility to display these frames onto the screen or other destination.
   ICamera DOES NOT perform any display operations.
===H2>
   Preview Mode:
===/H2>

   Before you start the camera in preview mode, you need to perform the following operations:
   (1) ICAMERA_SetDisplaySize() to set the frame display size
   (2) [Optional] ICAMERA_SetFramesPerSecond() to set the FPS of the camera

   ICAMERA_Preview() starts the camera in preview mode. CAM_STATUS_START callback will be sent to the app.
   Preview frames are delivered via CAM_STATUS_FRAME callback. Use ICAMERA_GetFrame() to retrieve the frame.
   ICAMERA_Pause() stops the frame callbacks. ICAMERA_Resume resumes the frame callbacks.
   ICAMERA_Stop() stops the preview operation and puts the camera in ready mode. CAM_STATUS_DONE callback
   will be sent to the app.
===H2>
   Snapshot Mode:
===/H2>

   Before you do snapshot operation, you need to perform the following operations:
   (1) ICAMERA_SetMediaData()
   (2) ICAMERA_SetSize()
   (3) [Optional] ICAMERA_SetVideoEncode()
   (4) [Optional] ICAMERA_SetQuality() or ICAMERA_SetMaxFileSize()
   (5) [Optional] ICAMERA_SetFramesPerSecond()

   ICAMERA_RecordSnapshot() starts the snapshot recording operation.
   CAM_STATUS_START callback will be sent to app followed by CAM_STATUS_DONE when recording
   is complete.
   When the encoding is completed, {CAM_CMD_ENCODESNAPSHOT, CAM_STATUS_DONE} callback will be sent.
   When MaxFileSize and Quality are both set, whichever is set last will take effect.

   ICamera can be configured to defer the snapshot encoding as follows. This is known as DeferEncode
   feature. Note that, by default, DeferEncode is disabled.
   (1) ICAMERA_SetMediaData()
   (2) ICAMERA_DeferEncode(TRUE): Indicates that encoding must be defered

   ICAMERA_RecordSnapshot() starts the snapshot recording operation.
   CAM_STATUS_START callback will be sent to app followed by CAM_STATUS_DONE when recording
   is complete. Now, only raw frame is recorded and it is not yet encoded.
   App can access the raw frame using ICAMERA_GetFrame() in the callback.
   ICAMERA_EncodeSnapshot() encodes the frame and sends {CAM_CMD_ENCODESNAPSHOT, CAM_STATUS_DONE} callback
   when encoding is done. Do ICAMERA_SetMediaData() before calling ICAMERA_EncodeSnapshot.
===H2>
   Movie Mode:
===/H2>

   Before you do start recording movie, you need to perform the following operations:
   (1) ICAMERA_SetMediaData()
   (2) ICAMERA_SetSize()
   (3) [Optional] ICAMERA_SetVideoEncode() and ICAMERA_SetAudioEncode()
   (4) [Optional] ICAMERA_SetQuality()
   (5) [Optional] ICAMERA_SetBitRate()
   (6) [Optional] ICAMERA_SetFramesPerSecond()

   Recorded frames are delivered via CAM_STATUS_FRAME callback. Use ICAMERA_GetFrame() to retrieve the frame.
   ICAMERA_Pause() pauses recording as well as stops the frame callbacks.
   ICAMERA_Resume resumes the recording and the frame callbacks.
   ICAMERA_Stop() stops the record operation and puts the camera in ready mode. CAM_STATUS_DONE callback
   will be sent to app.
===H2>
   App Suspend/Resume:
===/H2>

   When the app gets EVT_APP_SUSPEND, it is recommended that the app stop the camera and release the ICamera interface.
   When the app gets EVT_APP_RESUME, it can create ICamera interface and resume its operation.
===H2>
   Sample Code:
===/H2>

===H2>
   Multiple Camera (sensor) support:
===/H2>
   Each camera sensor is identified by ICamera CLSID. AEECamera.bid defines a list of CLSIDs with
   each CLSID identfying a sensor. AEECLSID_CAMERA is the same as AEECLSID_CAMERA1, which represents the
   default (primary) sensor. If the device has two cameras, then the second sensor is identified
   using AEECLSID_CAMERA2 and so on.

   Some device implementations may not support multiple sensors at the same time (simultaneously). This means you
   cannot create ICamera instances corresponding to each of the sensors at the same time. Typically,
   ISHELL_CreateInstance() would return ENOTALLOWED if multiple sensors are not supported simultaneously.

===pre>
   The following code snippet starts the camera in preview mode and displays frames to the device
   screen.

      static int CApp_StartCameraInPreviewMode(CApp * pme)
      {
         int   nErr;

         // Create ICamera instance.
         nErr = ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_CAMERA, (void **)&pme->m_pICamera);
         if (nErr)
            return nErr;

         // Register callback notification function.
         nErr = ICAMERA_RegisterNotify(pme->m_pICamera, CApp_CameraNotify, pme);
         if (nErr)
            return nErr;

         ICAMERA_SetDisplaySize(pme->m_pICamera, &pme->m_sizePreview);
         ICAMERA_SetSize( m_pICamera, &pme->m_sizeFrame);
         nErr = ICAMERA_Preview(pme->m_pICamera);
         if (nErr)
            return nErr;

         return SUCCESS;
      }

      static void CApp_CameraNotify(void * pUser, AEECameraNotify * pn)
      {
         CApp *   pme = (CApp *)pUser;

         if (!pme || !pn)
            return;

         switch (pn->nStatus)
         {
            case CAM_STATUS_START:
               // Preview has begun...
               break;

            case CAM_STATUS_FRAME:
            {
               IBitmap *      pFrame;
               AEEBitmapInfo  bi;

               //
               // IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
               //
               if (SUCCESS != ICAMERA_GetFrame(pme->m_pICamera, &pFrame))
                  return;

               // Get the bitmap info...this can be saved in app global structure.
               IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

               // Display the frame at (0, 0) location of the screen
               IDISPLAY_BitBlt(pme, 0, 0, bi.cx, bi.cy, pFrame, 0, 0, AEE_RO_COPY);
               IBITMAP_Release(pFrame);
               break;
            }

            case CAM_STATUS_DONE:
               // ICAMERA_Stop() was called and preview operation stopped.
               break;

            case CAM_STATUS_ABORT:
               // Preview got aborted.
               break;
         }
      }
===/pre>
===H2>
Camera State Machine:
===/H2>
===pre>
--------------------
                -------------------------READY-----------------------
               |    ^                     |  ^                  |   ^
      Preview()|    |     RecordSnapshot()|  |     RecordMovie()|   |
               |    |                     |  |                  |   |Stop()/
               |    |Stop()/              |  |DONE/ABORT        |   |DONE/ABORT
               V    |DONE/ABORT           V  |                  V   |
              PREVIEW                   SNAPSHOT               MOVIE
               |    ^                                           |   ^
               |    |                                           |   |
                ----                                             ---
               Pause()                                          Pause()
               Resume()                                         Resume()

===/pre>
===H2>
Camera Call Flow:
===/H2>
===pre>
[ICamera State]  BREW App                      BREW ICamera           Example OEM Camera Layer
---------------------------------------------------------------------------------------
(1) Preview:
-----------
                     ------ICAMERA_SetDisplaySize()--------->

[READY]              ------ICAMERA_Preview()--------->
                                                      -------camera_start_preview()->

[PREVIEW]                                             <-------SUCCESS----------------
                     <-----CAM_CMD_START/CAM_STATUS_START-----------

                                                      <-------FRAME-------------------
                     <-----CAM_CMD_START/CAM_STATUS_FRAME---
                     ------ICAMERA_GetFrame()---------> [returns PreviewFrame IBitmap]

                                                      <-------FRAME-------------------
                     <-----CAM_CMD_START/CAM_STATUS_FRAME---
                     ------ICAMERA_GetFrame()---------> [returns PreviewFrame IBitmap]
                              :
                              :
                              :
                     ------ICAMERA_Stop()------------>
                                                      -------camera_stop_preview()-->

[READY]                                               <-------DONE-------------------
                     <-----CAM_CMD_START/CAM_STATUS_DONE------------

(2) Record SnapShot (Immediate Encoding):
----------------------------------------
                     ------ICAMERA_SetMediaData()------->
                     ------ICAMERA_SetVideoEncode()---->
                     ------ICAMERA_SetSize()------->

                     ------[Optional]ICAMERA_SetQuality()------>

[READY]              ------ICAMERA_RecordSnapshot()-->
                                                      -------camera_take_picture()-->
                                                      <-------SUCCESS----------------
[SNAPSHOT]           <-----CAM_STATUS_START-----------
                                                      <-------DONE(SnapshotFrame)----
[READY]              <-----CAM_CMD_START/CAM_STATUS_DONE------------
                                                      -------camera_encode(SnapshotFrame)-->
                                                      <-------SUCCESS-------------

                                                      <-------DONE----------------
[READY]              <-----CAM_CMD_ENCODESNAPSHOT/CAM_STATUS_DONE------------

(2) Record SnapShot (Deferred Encoding):
---------------------------------------
                     ------ICAMERA_SetMediaData()------->
                     ------ICAMERA_SetVideoEncode()---->
                     ------ICAMERA_SetSize()------->
                     ------ICAMERA_SetQuality()------>

[READY]              ------ICAMERA_RecordSnapshot()-->
                                                      -------camera_take_picture()-->
                                                      <-------SUCCESS----------------
[SNAPSHOT]           <-----CAM_STATUS_START-----------
                                                      <-------DONE(SnapshotFrame)--
[READY]              <-----CAM_CMD_START/CAM_STATUS_DONE------------
                     ------ICAMERA_GetFrame()---------> [returns SnapshotFrame IBitmap]

                     ------ICAMERA_EncodeSnapshot(SnapshotFrame)-->
                                                      -------camera_encode(SnapshotFrame)-->
                                                      <-------SUCCESS-------------

                                                      <-------DONE----------------
[READY]              <-----CAM_CMD_ENCODESNAPSHOT/CAM_STATUS_DONE------------

(3) Record Movie:
----------------
                     ------ICAMERA_SetMediaData()------->
                     ------ICAMERA_SetVideoEncode()---->
                     ------ICAMERA_SetAudioEncode()---->
                     ------ICAMERA_SetSize()------->

                     ------[Optional]ICAMERA_SetQuality()------>
                     ------[Optional]ICAMERA_SetBitRate()------>
                     ------[Optional]ICAMERA_SetFramesPerSecond()------>

[READY]              ------ICAMERA_RecordMovie()----->
                                                      -------camera_start_record()->

[MOVIE]                                               <-------SUCCESS---------------
                     <-----CAM_CMD_START/CAM_STATUS_START-----------

                                                      <-------FRAME-------------------
                     <-----CAM_CMD_START/CAM_STATUS_FRAME---
                     ------ICAMERA_GetFrame()---------> [returns MovieFrame IBitmap]

                                                      <-------FRAME-------------------
                     <-----CAM_CMD_START/CAM_STATUS_FRAME---
                     ------ICAMERA_GetFrame()---------> [returns MovieFrame IBitmap]
                              :
                              :
                              :
                     ------ICAMERA_Pause()----------->
                                                      -------camera_pause_record()-->

                                                      <-------SUCCESS----------------
                     <-----CAM_CMD_START/CAM_STATUS_PAUSE-----------
                              :
                              :
                     ------ICAMERA_Resume()---------->
                                                      -----camera_resume_record()--->

                                                      <-------SUCCESS----------------
                     <-----CAM_CMD_START/CAM_STATUS_RESUME----------
                              :
                              :
                     ------ICAMERA_Stop()------------>
                                                      -------camera_stop_record()--->

[READY]                                               <-------DONE-------------------
                     <-----CAM_CMD_START/CAM_STATUS_DONE------------

(4) Setting Parameters (Synchronous and Asynchronous operations):
----------------------------------------------------------------
                     ------ICAMERA_SetMediaData()------->
                     ------ICAMERA_SetVideoEncode()---->
                     ------ICAMERA_SetSize()------->
                     ------ICAMERA_SetQuality()------>
                     ------[Optional]ICAMERA_SetBitRate()------>

[ANY]                ---ICAMERA_SetParm(Brightness)-->
                                                      ---camera_set_parm(brightness)->

                                                      <-------DONE--------------------
                     <-----CAM_CMD_SETPARM(CAM_PARM_BRIGHTNESS)/CAM_STATUS_DONE---
===/pre>

The following header file is required:

AEECamera.h
=============================================================================

ICAMERA_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Release()

=============================================================================

ICAMERA_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_AddRef()

=============================================================================

ICAMERA_QueryInterface()

Description:
    This function can be used to
     -    Get a pointer to an interface or data based on the input class ID
     -    Query an extended version of the ICamera-derived class
     -    Support version compatibility

Prototype:
    int ICAMERA_QueryInterface
    (
    ICamera * pICamera,
    AEECLSID clsReq,
    void ** ppo
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    clsReq:  [in]:  A globally unique id to identify the entity (interface or
data) that we are trying to query.
    ppo:  [out]:  Pointer to the interface or data that we want to retrieve. If
the value passed back is NULL, the interface or data that we query are not available.

Return Value:
    SUCCESS: on success,
    Otherwise: an error code.

Comments:
    If ppo is back a NULL pointer, the interface or data that we query is not available.

Side Effects:
    If an interface is retrieved, then this function increments its reference count.

    If a data structure is retrieved, then a pointer to the internal structure is
given and user should not free it.

Version:
   Introduced BREW Client 2.1

See Also:
    None

=============================================================================

ICAMERA_RegisterNotify()

Description:
    This function registers a callback notification function with ICamera object.
ICamera reports asynchronous events using this callback.

Prototype:
    int ICAMERA_RegisterNotify
    (
    ICamera * pICamera,
    PFNCAMERANOTIFY pfnNotify,
    void * pUser
    )

Parameters:
    pICamera:  Pointer to the ICamera Interface object
    pfnNotify:  User callback function pointer
    pUser:  User data to be used when calling pfnNotify()

Return Value:
    SUCCESS: Successful.
    EBADSTATE: Error - ICamera is not in Ready state.

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    None

=============================================================================

ICAMERA_SetParm()

Description:
    This function sets the camera control parameters.

Prototype:
    int ICAMERA_SetParm
    (
    ICamera * pICamera,
    int16 nParmID,
    int32 p1,
    int32 p2
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nParmID:  CAM_PARM_XXX. See Camera Control Parameters
    p1:  Depends on the nParmID parameter
    p2:  Depends on the nParmID parameter

Return Value:
    SUCCESS: Successful. Operation is completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    See Camera Control Parameters for parameter details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    AEEParmInfo

=============================================================================

ICAMERA_GetParm()

Description:
    This function gets the camera control parameters.

Prototype:
    int ICAMERA_GetParm
    (
    ICamera * pICamera,
    int16 nParmID,
    int32 * pP1,
    int32 * pP2
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nParmID:  CAM_PARM_XXX
    pP1:  Depends on the nParmID parameter
    pP2:  Depends on the nParmID parameter

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    See Camera Control Parameters for parameter details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_SetParm()
    AEEParmInfo
=============================================================================

ICAMERA_Start()

Description:
    This function starts camera operation in preview, snapshot, or movie mode.

Prototype:
    int ICAMERA_Start
    (
    ICamera * pICamera,
    int16 nMode,
    uint32 dwParam
    );

Parameters:
    pICamera:  Pointer to ICamera Interface
    nMode:  CAM_MODE_XXX: CAM_MODE_PREVIEW / CAM_MODE_SNAPSHOT / CAM_MODE_MOVIE
    dwParam:  Reserved

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    EBADPARM: Bad parm is passed
    ENOMEMORY: Not enough memory
    EBADSTATE: Start cannot be done in current state

Comments:
    All the events that originate due to this API and due to the following API will
be reported via the user-specified callback.
~
    ICAMERA_Preview()~
    ICAMERA_RecordSnapshot()~
    ICAMERA_RecordMovie()~
    ICAMERA_Stop()~
    ICAMERA_Pause()~
    ICAMERA_Resume()~
    In the callback, AEECameraNotify,
*
===pre>
    nCmd = CAM_CMD_START
    nSubCmd = nMode.
===/pre>


Version:
   Introduced BREW Client 2.1

See Also:
    AEECameraNotify
    ICAMERA_Stop()
    ICAMERA_Pause()
    ICAMERA_Resume()
    ICAMERA_GetFrame()
    ICAMERA_Preview()
    ICAMERA_RecordMovie()
    ICAMERA_RecordSnapshot()

=============================================================================

ICAMERA_Preview()

Description:
    This function starts the camera operation in preview mode, which causes ICamera
to start sending frames to the client.

Prototype:
    int ICAMERA_Preview(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    EBADPARM: Bad parm is passed
    ENOMEMORY: Not enough memory
    EBADSTATE: Preview cannot be done in current state

Comments:
    You need to set the frame display size before calling this function. All the
events that originate due to this API and due to the following APIs will be reported
via the user-specified callback:
~
    ICAMERA_Stop()~
    ICAMERA_Pause()~
    ICAMERA_Resume()~
    CAM_STATUS_START callback happens once the preview begins.~
    CAM_STATUS_FRAME callbacks happen continuously unless you pause.~
    CAM_STATUS_DONE callback occurs when preview is stopped.~
    CAM_STATUS_ABORT callback occurs when preview is aborted.~
*
    In the callback, AEECameraNotify,~
===pre>
    nCmd = CAM_CMD_START
    nSubCmd = CAM_MODE_PREVIEW.
===/pre>

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Stop()
    ICAMERA_Pause()
    ICAMERA_Resume()
    ICAMERA_GetFrame()
    ICAMERA_SetDisplaySize()
    ICAMERA_SetSize()

=============================================================================
ICAMERA_RecordSnapshot()

Description:
    This function starts the camera operation in snapshot mode which causes the
camera to take a snapshot. If Defer Encoding is not enabled (default), this function
causes the snapshot to be encoded.

Prototype:
    int ICAMERA_RecordSnapshot(ICamera *pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    EBADPARM: Bad parm is passed
    ENOMEMORY: Not enough memory
    EBADSTATE: RecordSnapshot() cannot be done in current state

Comments:
    You need to set the media data before calling this function. Also, you may want
to specify active encoding, picture format and quality that should be used for encoding
the snapshot. In the callback, AEECameraNotify,
===pre>
    nCmd = CAM_CMD_START
    nSubCmd = CAM_MODE_SNAPSHOT.
===/pre>

    This function results in {CAM_CMD_START, CAM_STATUS_START} callback followed
by {CAM_CMD_START, CAM_STATUS_DONE} callback after the snapshot is taken. This is
followed by {CAM_CMD_ENCODESNAPSHOT, CAM_STATUS_DONE} callback after the snapshot
is encoded.

    You can defer encoding by calling ICAMERA_DeferEncoding(pICamera, TRUE); This
causes ICAMERA_RecordSnapshot() not to encode the snapshot. You can get the raw snapshot
frame using ICAMERA_GetFrame() and call ICAMERA_EncodeSnapshot() to encode the snapshot.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_EncodeSnapshot()
    ICAMERA_DeferEncode()
    ICAMERA_GetFrame()
    ICAMERA_SetDisplaySize()
    ICAMERA_SetSize()
    ICAMERA_SetMediaData()
    ICAMERA_SetQuality()
    ICAMERA_SetVideoEncode()
=============================================================================

ICAMERA_RecordMovie()

Description:
    This function starts the camera operation in movie mode, which causes recorded
frames to be sent to the caller while encoding those frames.

Prototype:
    int ICAMERA_RecordMovie(ICamera * pICamera);

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
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Stop()
    ICAMERA_Pause()
    ICAMERA_Resume()
    ICAMERA_GetFrame()
    ICAMERA_SetDisplaySize()
    ICAMERA_SetSize()
    ICAMERA_SetMediaData()
    ICAMERA_SetQuality()
    ICAMERA_SetAudioEncode()
    ICAMERA_SetVideoEncode()

=============================================================================

ICAMERA_RecordMoviePostcard()

Description:
    This function starts the camera operation in movie mode and records a movie postcard. 
    Movie postcard has only one frame (or same frame repeated) with background audio.
    The recorded frame(s) are sent to the caller during encoding of the frame(s).

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
    ICAMERA_SetDisplaySize()
    ICAMERA_SetSize()
    ICAMERA_SetMediaData()
    ICAMERA_SetQuality()
    ICAMERA_SetAudioEncode()
    ICAMERA_SetVideoEncode()

=============================================================================

ICAMERA_Stop()

Description:
    This function stops the current camera operation and puts it in Ready state.

Prototype:
    int ICAMERA_Stop(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    ENOMEMORY: Not enough memory
    EBADSTATE: Stop cannot be done in current state

Comments:
    This function results in CAM_STATUS_DONE status callback.
    In the callback, AEECameraNotify,
===pre>
     nCmd = CAM_CMD_START
     nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_MOVIE.
===/pre>

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Preview()
    ICAMERA_RecordMovie()
    ICAMERA_RecordSnapshot()

=============================================================================

ICAMERA_Pause()

Description:
    This function pauses the camera operation. In preview and record modes, the
frame callbacks are paused. In record mode, the encoding is also paused.

Prototype:
    int ICAMERA_Pause(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    ENOMEMORY: Not enough memory
    EBADSTATE: Pause cannot be done in current state

Comments:
    This API does not apply to Snapshot mode.

    This function results in CAM_STATUS_PAUSE status callback.
    In the callback, AEECameraNotify,
===pre>
     nCmd = CAM_CMD_START
     nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_MOVIE.
===/pre>

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Preview()
    ICAMERA_RecordMovie()
    ICAMERA_Resume()

=============================================================================

ICAMERA_Resume()

Description:
    This function resumes the camera operation. In preview and record modes, the
frame callbacks are resumed. In record mode, the encoding is also resumed.

Prototype:
    int ICAMERA_Resume(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Command accepted
    EFAILED: General failure
    ENOMEMORY: Not enough memory
    EBADSTATE: Resume cannot be done in current state

Comments:
    This API does not apply to Snapshot mode. This function results in CAM_STATUS_RESUME
status callback. In the callback, AEECameraNotify,
===pre>
     nCmd = CAM_CMD_START
     nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_MOVIE.
===/pre>

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Pause()
    ICAMERA_Preview()
    ICAMERA_RecordMovie()
    ICAMERA_RecordSnapshot()

=============================================================================

ICAMERA_GetFrame()

Description:
    This function returns the current frame captured by the camera.

Prototype:
    int ICAMERA_GetFrame(ICamera * pICamera, IBitmap ** ppFrame);

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    ppFrame:  [out]:  Frame IBitmap returned .

Return Value:
    SUCCESS: if successful~
    Error code, if failure.

Comments:
    This function is typically called in response to CAM_STATUS_FRAME callback.
It is caller's responsibility to release the IBitmap object after calling this function.

    The caller can QueryInterface, on the returned IBitmap, for IDIB which, if supported,
allows access to frame data.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_Preview()
    ICAMERA_RecordMovie()

=============================================================================

ICAMERA_EncodeSnapshot()

Description:
    This function encodes the recorded snapshot/frame.

Prototype:
    int ICAMERA_EncodeSnapshot
    (
    ICamera * pICamera,
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: if successful.~
    Error code if failure.

Comments:
    This API is typically called in response to CAM_STATUS_DONE when ICAMERA_RecordSnapshot().
You can use ICAMERA_GetFrame() to get the latest raw snapshot frame.

    This function results in {CAM_CMD_ENCODESNAPSHOT, CAM_STATUS_DONE} callback
after the snapshot is encoded.

    This API can be called in any mode. It may abort the current active operation
like preview, movie, encode snapshot, and so forth. It may also be used to encode any frame,
if supported.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_RecordSnapshot()
    ICAMERA_GetFrame()

=============================================================================

ICAMERA_DeferEncode()

Description:
    This function defers the encoding of the snapshot done by ICAMERA_RecordSnapshot()
API. You need to explicitly call ICAMERA_EncodeSnapshot() to encode the snapshot.

Prototype:
    int ICAMERA_DeferEncode
    (
    ICamera * pICamera,
    boolean bDefer
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    bDefer:  TRUE implies the encoding will be done by user.

Return Value:
    SUCCESS: if successful.~
    Error code if failure.

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()
    ICAMERA_RecordSnapshot()
    ICAMERA_EncodeSnapshot()

=============================================================================
ICAMERA_SetMediaData()

Description:
    This function sets the media data where the recorded and encoded data will be
saved.

Prototype:
    int ICAMERA_SetMediaData
    (
    ICamera * pICamera,
    AEEMediaData * pmd,
    const char * cpszMIME
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    pmd:  Pointer to media data
    cpszMIME:  Optional MIME type of the media
               If this is not-null, it is presumed to be the
               file format of the new movie file that will be recorded.
               It should be set to:
               CAM_FILE_FORMAT_MPG4  for files with .mp4 extension
               CAM_FILE_FORMAT_AMC   for files with .amc extension
               CAM_FILE_BRAND_3GP    for files with .3gp extension
               CAM_FILE_BRAND_3G2    for files with .3g2 extension


Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetVideoEncode()

Description:
    This function sets the active video/image encoding type used to encode the recorded
snapshot/movie.

Prototype:
    int ICAMERA_SetVideoEncode
    (
    ICamera * pICamera,
    AEECLSID cls,
    uint32 dwExtra
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    cls:  Encoding class ID: CAM_ENCODE_RAW, AEECLSID_JPEG,
                             AEECLSID_MEDIAMPEG4, CAM_ENCODE_UUID,
                             CAM_ENCODE_UUID_DATA, CAM_ENCODE_SHORT_HEADER,
                             etc.
    dwExtra:  Extra info regarding the encoding like sub formats.
              If cls is CAM_ENCODE_UUID, dwExtra must point to structure
              of type CameraData. The data in this structure specifies
              the type of atom that will follow when a subsequent call
              to ICAMERA_SetVideoEncode is made with cls=CAM_ENCODE_UUID_DATA.
              For example, to set the telop text atom in the moviefile,
              CameraData->pData would point to something like follows:
              {"tsml"-A88C-11d4-8197-009027087703}

              If cls is CAM_ENCODE_UUID_DATA, dwExtra must point to
              structure of type CameraData. This is the actual data that
              will be written as a uuid atom in the moviefile.

              If cls is CAM_ENCODE_UUID_LIST_ATOM_INFO, dwExtra points to
              structure of type CameraEncodeUUIDAtomInfo. This is a structure
              that specifies the atom and the actual data that will be written
              as a uuid atom in the moviefile..
              For example, to set the telop text atom in the moviefile,
              CameraEncodeUUIDAtomInfo->pAtom would point to something like
              follows: {"tsml"-A88C-11d4-8197-009027087703}
              If dwExtra is NULL, uuid atoms that were previously set in uuid
              list will be cleared.

              If cls is CAM_ENCODE_SHORT_HEADER, dwExtra must point to
              structure of type CameraData. CameraData->pData should
              point to following:

              0  if new recorded movie file will have regular MP4 header

              1  if new recorded movie file will have H.263 header


Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current setting value via ICAMERA_GetParm(),
    see Camera Control Parameters for CAM_PARM_VIDEO_ENCODE details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetAudioEncode()

Description:
    This function sets the active audio encoding type used to encode along with
the recorded snapshot/movie.

Prototype:
    int ICAMERA_SetAudioEncode
    (
    ICamera * pICamera,
    AEECLSID cls,
    uint32 dwExtra
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    cls:  Encoding class ID. E.g. AEECLSID_MEDIAQCP, etc.
    dwExtra:  Extra info regarding the encoding like sub formats.
~
      Example:For AEECLSID_MEDIAQCP, sub-format can be specified as MM_QCP_FORMAT_FIXED_FULL_EVRC.
*

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_AUDIO_ENCODE details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetSize()

Description:
    This function sets the camera to record a snapshot or movie in specified size.

Prototype:
    int ICAMERA_SetSize
    (
    ICamera * pICamera,
    AEESize * pSize
    )

Parameters:
    pICamera [in]:  Pointer to ICamera Interface
    pSize:  Size of the picture

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_SIZE details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    CAM_PARM_SIZE

=============================================================================

ICAMERA_SetQuality()

Description:
    This function sets the camera to capture specified picture quality.

Prototype:
    int ICAMERA_SetQuality
    (
    ICamera * pICamera,
    int16 nQuality
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nQuality:  Picture quality

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_QUALITY details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    ICAMERA_SetMaxFileSize()

=============================================================================

ICAMERA_SetBitRate()

Description:
    This function sets the user specified encoding bit rate to record a video clip.

Prototype:
    int ICAMERA_SetBitRate
    (
    ICamera * pICamera,
    int16 nBitRate
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nBitRate:  User specified encoding bit rate

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_BITRATE details.

Version:
    Introduced BREW Client 3.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetFramesPerSecond()

Description:
    This function sets the frames per second setting of camera in preview or movie
mode.

Prototype:
    int ICAMERA_SetFramesPerSecond
    (
    ICamera * pICamera,
    uint32 dwFPS
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    dwFPS:  Frames per second. See dwFPS format in CAM_PARM_FPS documentation.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_FPS details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetContrast()

Description:
    This function sets the contrast of the camera.

Prototype:
    int ICAMERA_SetContrast
    (
    ICamera * pICamera,
    int32 nValue
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Contrast value.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_CONTRAST details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    AEEParmInfo

=============================================================================

ICAMERA_SetBrightness()

Description:
    This function sets the brightness of the camera.

Prototype:
    int ICAMERA_SetBrightness
    (
    ICamera * pICamera,
    int32 nValue
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Brightness value.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_BRIGHTNESS details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    AEEParmInfo

=============================================================================

ICAMERA_SetSharpness()

Description:
    This function sets the sharpness of the camera.

Prototype:
    int ICAMERA_SetSharpness
    (
    ICamera * pICamera,
    int32 nValue
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Sharpness value.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_SHARPNESS details.

    ICAMERA_GetParm() with CAM_PARM_SHARPNESS, returns AEEParmInfo that specifies
the sharpness info.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetZoom()

Description:
    This function sets the zoom of the camera.

Prototype:
    int ICAMERA_SetZoom
    (
    ICamera * pICamera,
    int32 nValue
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Zoom value

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_ZOOM details.

    ICAMERA_GetParm() with CAM_PARM_ZOOM, returns AEEParmInfo that specifies the
zoom info.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_RotatePreview()

Description:
    This function rotates the preview frame. Only preview mode is affected.

Prototype:
    int ICAMERA_RotatePreview(ICamera * pICamera, int32 nValue);

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Rotation angle

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_ROTATE_PREVIEW details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    ICAMERA_RotateEncode()

=============================================================================

ICAMERA_RotateEncode()

Description:
    This function rotates the recorded and encoded frame. Only snapshot and movie
modes are affected.

Prototype:
    int ICAMERA_RotateEncode(ICamera * pICamera, int32 nValue);

Parameters:
    pICamera:  Pointer to ICamera Interface.
    nValue:  Rotation angle

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_ROTATE_ENCODE details.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    ICAMERA_RotatePreview()

=============================================================================

ICAMERA_AddOverlay()

Description:
    This function sets the overlay image that will be part of the recorded picture.
This operation is done any camera mode.

Prototype:
    int ICAMERA_AddOverlay
    (
    ICamera * pICamera,
    IBitmap * pb
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    pb:  Pointer to IBitmap representing the overlay

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    You can add overlays on top of another image by calling this function repeatedly
with different images. To clear ALL overlays, call ICAMERA_ClearOverlay().

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_SetParm()
    ICAMERA_ClearOverlay()

=============================================================================

ICAMERA_ClearOverlay()

Description:
    This function clears all the overlaid images.

Prototype:
    int ICAMERA_ClearOverlay(ICamera * pICamera);

Parameters:
    pICamera:  Pointer to ICamera Interface.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    ICAMERA_ClearOverlay()

=============================================================================

ICAMERA_SetDisplaySize()

Description:
    This function sets the frame display size where the captured data is displayed.

Prototype:
    int ICAMERA_SetDisplaySize
    (
    ICamera * pICamera,
    AEESize * pSize
    )

Parameters:
    pICamera:  Pointer to ICamera Interface.
    pSize:  Frame display size within the main display/off-srceen buffer area

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_SetMaxFileSize()

Description:
   This function sets the camera max target file size for encoding (typically
   JPEG encoding).

Prototype:
   int ICAMERA_SetMaxFileSize
   (
     ICamera * pICamera,
     int32 nFileSize
   );

Parameters:
   pICamera [in]: Pointer to ICamera Interface.
   nFileSize: Target output file size for image encoding

Return Value:
   SUCCESS:    Successful. Operation completed.
   CAM_PENDING: Result returned via the registered callback
   EBADPARM:   Bad parm
   ENOMEMORY: Not enough memory
   EBADSTATE:  Cannot set parm in the current state
   EUNSUPPORTED: Parm not supported by the class
   EFAILED: General Failure

Comments:
    To get the current value via ICAMERA_GetParm() information,
    see Camera Control Parameters for CAM_PARM_FILE_SIZE details.
Side Effects:
   None.

See Also:
   ICAMERA_GetParm()
   ICAMERA_SetQuality()
=============================================================================

ICAMERA_GetMode()

Description:
    This function returns the current camera mode.

Prototype:
    int ICAMERA_GetMode
    (
    ICamera * pICamera,
    int16 * pnMode,
    boolean * pbPaused
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pnMode:  [out]:  Pointer to mode. CAM_MODE_XXX
    pbPaused:  [out]:  TRUE/FALSE
    Paused/Resumed

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_Start()

=============================================================================

ICAMERA_IsSupport()

Description:
    This function checks if specified parameter is supported by ICamera Interface.
This function is useful to check the camera capabilities like setting of brightness,
zoom, etc.

Prototype:
    int ICAMERA_IsSupport
    (
    ICamera * pICamera,
    int16 nParmID,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    nParmID:  [in]:  CAM_PARM_XXX parameter ID.See Camera Control Parameters
    pbSupport:  [out]:  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_IsContrast()

Description:
    This function checks if camera has contrast setting capability.

Prototype:
    int ICAMERA_IsContrast
    (
    ICamera * pICamera,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pbSupport:  [out]:  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_IsSupport()

=============================================================================

ICAMERA_IsBrightness()

Description:
    This function checks if camera has brightness setting capability.

Prototype:
    int ICAMERA_IsBrightness
    (
    ICamera * pICamera,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pbSupport:  [out]:  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_IsSupport()

=============================================================================

ICAMERA_IsSharpness()

Description:
    This function checks if camera has sharpness setting capability.

Prototype:
    int ICAMERA_IsSharpness
    (
    ICamera * pICamera,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pbSupport:  [out]:  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_IsSupport()

=============================================================================

ICAMERA_IsZoom()

Description:
    This function checks if camera has zoom capability.

Prototype:
    int ICAMERA_IsZoom
    (
    ICamera * pICamera,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pbSupport:  [out]:  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_IsSupport()

=============================================================================

ICAMERA_IsMovie()

Description:
    This function checks if camera has movie recording capability.

Prototype:
    int ICAMERA_IsMovie
    (
    ICamera * pICamera,
    boolean * pbSupport
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    pbSupport:  [out]:  Pointer to boolean. : TRUE/FALSE => Supported/Unsupported.

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()

=============================================================================

ICAMERA_GetSizeList()

Description:
    This function retrieves the list of discrete sizes supported for specified mode
or continuos range (for example, any size between 10x10 to 100x150).

Prototype:
    int ICAMERA_GetSizeList
    (
    ICamera * pICamera,
    AEESize ** ppList,
    boolean * pbRange
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    ppList:  [in]:  ppList contains CAM_MODE_SNAPSHOT/CAM_MODE_MOVIE
    ppList:  [out]:  Pointer to NULL- terminated size list
    pbRange:  [out]:  Pointer to boolean when TRUE indicates the passed list is
a NULL-terminated paired list (i.e. multiple of 2) of ranges

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    The list should be copied and should not be freed.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    AEESize

=============================================================================

ICAMERA_GetDisplaySizeList()

Description:
    This function retrieves the list of discrete display sizes supported for specified
mode or continuous range (for example, any size between 10x10 to 100x150).

Prototype:
    int ICAMERA_GetDisplaySizeList
    (
    ICamera * pICamera,
    AEESize ** ppList,
    boolean * pbRange
    )

Parameters:
    pICamera:  [in]:  Pointer to ICamera Interface.
    ppList:[in]:  *ppList contains CAM_MODE_PREVIEW/CAM_MODE_MOVIE
    ppList:[out]:  Pointer to NULL- terminated size list
    pbRange:  [out]:  Pointer to boolean when TRUE indicates the passed list is
a NULL-terminated paired list (that is, a multiple of 2) of ranges

Return Value:
    SUCCESS: Successful. Operation completed.
    CAM_PENDING: Value returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    The list should be copied and should not be freed.

Version:
   Introduced BREW Client 2.1

See Also:
    ICAMERA_GetParm()
    AEESize

=============================================================================

ICAMERA_SetFlashCtl()

Description:
    This function allows app to control device flash. User can set flash type and level.

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
    This function allows app to get flash type and level.

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

#endif // AEECAMERA_H
