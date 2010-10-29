#ifndef AEECAMERAEX_H
#define AEECAMERAEX_H

/*============================================================================
FILE:   AEECameraEx.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file defines ICamera interface that controls the device camera.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright (c) 2010 WaterWorld
All Rights Reserved.

Anthor:			Wangliang
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

typedef struct ICamera  ICamera;

// AEECameraNotify structure is returned via AEECallback
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

   int         (*EncodeSnapshot)(ICamera * po);
};

#define ICAMERA_AddRef(p)                       AEEGETPVTBL(p, ICamera)->AddRef(p)
#define ICAMERA_Release(p)                      AEEGETPVTBL(p, ICamera)->Release(p)
#define ICAMERA_QueryInterface(p, i, p2)        AEEGETPVTBL(p, ICamera)->QueryInterface(p, i, p2)

#define ICAMERA_RegisterNotify(p, pfn, pu)      AEEGETPVTBL(p, ICamera)->RegisterNotify(p, pfn, pu)


#define ICAMERA_SetParm(p, c, p1, p2)           AEEGETPVTBL(p, ICamera)->SetParm(p, c, p1, p2)
#define ICAMERA_GetParm(p, c, pp1, pp2)         AEEGETPVTBL(p, ICamera)->GetParm(p, c, pp1, pp2)

// Camera preview/record control APIs:
#define ICAMERA_Preview(p)                      AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_PREVIEW,  0)
#define ICAMERA_RecordSnapshot(p)               AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_SNAPSHOT, 0)
#define ICAMERA_RecordMovie(p)                  AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_MOVIE,    CAM_MOVIE_NORMAL)
#define ICAMERA_RecordMoviePostcard(p)          AEEGETPVTBL(p, ICamera)->Start(p, CAM_MODE_MOVIE,    CAM_MOVIE_POSTCARD)
#define ICAMERA_Stop(p)                         AEEGETPVTBL(p, ICamera)->Stop(p)
#define ICAMERA_Pause(p)                        AEEGETPVTBL(p, ICamera)->Pause(p, TRUE)
#define ICAMERA_Resume(p)                       AEEGETPVTBL(p, ICamera)->Pause(p, FALSE)
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
#define ICAMERA_SetMaxFileSize(p, s)            ICAMERA_SetParm(p, CAM_PARM_FILE_SIZE,    (int32)(s),       0)
#define ICAMERA_RotatePreview(p, v)             ICAMERA_SetParm(p, CAM_PARM_ROTATE_PREVIEW,(int32)(v),      0)
#define ICAMERA_RotateEncode(p, v)              ICAMERA_SetParm(p, CAM_PARM_ROTATE_ENCODE,(int32)(v),       0)
#define ICAMERA_AddOverlay(p, pb)               ICAMERA_SetParm(p, CAM_PARM_OVERLAY,      (int32)(pb),      0)
#define ICAMERA_ClearOverlay(p)                 ICAMERA_SetParm(p, CAM_PARM_OVERLAY,      0,                0)

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

#endif // AEECAMERA_H

