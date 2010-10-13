
/*===========================================================================

FILE: qcamera.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for qcamera
    source files.

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/inc/qcamera.h#6 $ $DateTime: 2008/12/18 21:55:13 $ $Author: kgelli $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 12/17/08  KC    Added Raw preview capture feature
 11/25/08  RK    Added a New Feature ACC(Audio Camera Concurrency) support if
                 FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND is defined.
 11/25/08  KC     Added a new variable bDirectMode to keep track of the LCD direct mode status.
 08/11/08  KC    Added a new enum QCamMultishotMode for Runtime TE feature for Multishot.
                 This is used to decide on what mode is currently supported.
 11/09/07  NC     Added support to FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE.
 08/30/07  KD     Moved definition of Qcamera_BuildSaveDlg() to qcamera_i.h
 08/20/07  SK     UI Changes for Panoramic Stitch feature
 07/30/07  KD     Updated the QCam struct for the multishot enhancement feature
 05/22/07  SK     JPEG Decoder Resizing feature
 05/24/07  NC     Removed extra variables.
 05/10/07  KV     Defined QCAM_1_BUTTON_SNAPSHOT for 1-Button SnapShot Option.
 04/19/07  SK     UI Support for WVGA Display
 04/03/07  KV     Defined QCAM_SELF_TIMER_TOOLONG for 25s UI option for snapshot timer.
 03/23/07  KV     Added member variables in _QCam struct for dynamic font size.
 03/15/07  NC     Defined QCAM_MAX_FOCUSRECTS for focus rects feature.
 03/07/07  ZL     Added FEATURE_CAMERA_MULTI_SENSOR support.
 02/28/07  NC     Enabled direct mode support if FEATURE_CAMERA_LCD_DIRECT_MODE is defined.
 02/07/07  NC     Redesigned preview histogram feature.
 01/18/07  sk     Defined QCAM_MAX_MULTISHOTS as 8 for 7k
 01/11/07  ZL     Added support for one button AF & snapshot.
 01/03/07  SK     Redesigning the Multiple Focus Rectangles
 11/22/06  ZL     Switched to use ISTATIC at memory usage screen.
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/17/06  NC     Fixed wrong size issue with Max size option.
 11/03/06  NC     Added new variables for multiple displays feature.
 09/07/06  NC     Added new variable for new automation events.
 09/05/06  NC     Set condition FEATURE_CAMERA_SUPPORT_ICON_ARRAY for icon overlay.
 08/25/06  NC     Added UI support to enable LCD size display.
 08/07/06  NC     Added FPS High end setting support.
 07/28/06  NC     Deleted QcamMenus_BuildBracketMenu and added new variables.
 07/27/06  NC     Made QcamMenus_BuildBracketMenu public for dialog to call it.
 07/13/06  NC     Added new member variables to support multishot and bestshot.
 06/08/06  NC     Added a new function Qcamera_GetListFPS and variables for getting fps list.
 04/06/06  NC     Added a new function Qcamera_DrawBusyImage() and new property items.
 03/29/06  NC     Added a new exit type for album launch.
 02/28/06  NC     Added counter for automation test and update the option flag.
 02/08/06  NC     Added flags for dynamic support items.
 01/18/06  JR     Added support for small screen sizes as on SC1x target.
 10/12/05  SM     automation support for prompt to save/delete
 07/22/05  NC     Added a new function Qcamera_GetPreviewRect to match preview image location.
 06/07/05  NC     Added new definitions for Preview FPS options.
 04/15/05  NC     Added a new helper function Qcamera_DiagStatusReport().
 04/04/05  NC     Added a new variable to eliminate param in Qcamera_PromptForDelAll.
 03/24/05  NC     Added new variables and a new function for message display handler.
 03/15/05  NC     Update the param for Qcamera_SetIconArray()
 03/14/05  NC     Added feature to Brightness option.
 03/11/05  JN     Added nightshot mode.
 03/09/05  NC     Removed FEATURE_CAMERA_CONTRAST and updated data types of some variable.
 03/03/05  NC     Changed return type of Qcamera_ComposeStamp()
 03/01/05  NC     Defined new variables.
 01/24/05  NC     Document main logic go through.
 01/17/05  NC     Code clean up. Updated function names.
 01/14/05  NC     Use IOverlay for MDP feature.
 01/04/05  NC     Merge QCam.h code
 12/07/04  NC     Deleted old DIAG feature.
 11/16/04  NC     Added new parameter to Qcamera_PromptForDelAll for more options.
 07/7/04   NC     Initial design for qcamera re-architecture
============================================================================*/
#ifndef QCAMERA_H
#define QCAMERA_H

#ifdef AEE_SIMULATOR
#include "QcamSim.h"
#else
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA

#include "AEEModGen.h"    // Module interface definitions
#include "AEEAppGen.h"    // Applet interface definitions
#include "AEEShell.h"     // Shell interface definitions
#include "AEEFile.h"      // File interface definitions
#include "AEESound.h"     // Sound Interface definitions
#include "AEEMenu.h"      // Menu Services
#include "AEEStdLib.h"    // AEE stdlib services

#include "AEEImage.h"
#include "AEEImageCtl.h"
#include "AEEMedia.h"     // AEE Multimedia Services
#include "AEEMediaUtil.h"
#include "AEECamera.h"
#include "AEEPosDet.h"
#ifdef FEATURE_OVERLAY
#include "IOverlay.h"
#endif // FEATURE_OVERLAY
#include "ICameraExt.h"

#include "QcamCommon.h"
#include "QcamDisp.h"
#include "qcamera.bid"
#include "qcamera.brh"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifndef AEE_SIMULATOR
#include "ipl.h"
#endif

#ifdef FEATURE_QVPHONE
#include "AEEVideoPhone.h"
#endif

/*===========================================================================
Feature condition - Some FEATUREs are under condition of FEATURE_MDP
They are defined in different cust files and condition could not be set there.
Workaround in .c files to make sure no compiling issue happens in the case.
===========================================================================*/
#ifndef FEATURE_MDP
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
#undef FEATURE_CAMERA_LCD_DIRECT_MODE
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
#endif //FEATURE_MDP
#ifndef FEATURE_MMOVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
#undef FEATURE_CAMERA_MULTIPLE_DISPLAY
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_CAMERA_HISTOGRAM
#undef FEATURE_CAMERA_HISTOGRAM
#endif //FEATURE_CAMERA_HISTOGRAM
#ifdef FEATURE_CAMERA_PANORAMIC
#undef FEATURE_CAMERA_PANORAMIC
#endif //FEATURE_CAMERA_PANORAMIC
#endif //FEATURE_MMOVERLAY

// auto antibanding option to be added in Brew 3.1.5.x
#ifndef CAM_ANTIBANDING_AUTO
#define CAM_ANTIBANDING_AUTO       (CAM_ANTIBANDING_BASE+3)   // Auto detect antibanding
#endif


#define QCAMERA_APP_VERSION "2.0"

#ifndef QCAMERA_VGA_RES_FILE
#define QCAMERA_VGA_RES_FILE "qcamera_vga.bar"
#endif //QCAMERA_VGA_RES_FILE

// Make sure this value is in sync with the camera settings (camif.h)
#define QCAM_CAMERA_SIZE QCAM_SIZE_UXGA

#define QCAM_CONTRAST_STEPS   10
#define QCAM_SHARPNESS_STEPS  10
#define QCAM_HUE_STEPS        10
#define QCAM_SATURATION_STEPS 10
#define IDS_TPMENU_START      IDS_TAKE_PIC
#define IDS_SETMENU_START     IDS_SIZE

#ifdef FEATURE_CAMERA7500
#define QCAM_MAX_MULTISHOTS     8
#define QCAM_MAX_FOCUSRECTS     3
#else
#define QCAM_MAX_MULTISHOTS     16
#endif // FEATURE_CAMERA7500

#define QCAM_MAX_DELAY          1000
// Property bits
#define QCAM_LANDSCAPE          0x00000000
#define QCAM_PROMPT_SAVE        0x00000001
#define QCAM_PORTRAIT           0x00000002
#define QCAM_DIR_ASCENDING      0x00000004
#define QCAM_PHOTO_ASCENDING    0x00000008
#define QCAM_LUMA_ADAPTATION    0x00000010
#define QCAM_LCD_DISP           0x00000020
#define QCAM_MULTI_DISP         0x00000040
#define QCAM_WVGASIZE_DISP      0x00000080
#define QCAM_1_BUTTON_SNAPSHOT  0x00000100
#define QCAM_SLIDESHOW_RESIZE   0x00000200
#define QCAM_PANORAMIC_EFFECT   0x00000400

// JPEG quality setting
#define JPEG_QUALITY_LOW 50
#define JPEG_QUALITY_MED 75
#define JPEG_QUALITY_HIGH 90

//self timer set time
#define QCAM_SELF_TIMER_SHORT   2
#define QCAM_SELF_TIMER_MED     5
#define QCAM_SELF_TIMER_LONG    10
#define QCAM_SELF_TIMER_TOOLONG	25

//slide show speed
#define QCAM_SLIDESHOW_FAST     1000
#define QCAM_SLIDESHOW_MED      2000
#define QCAM_SLIDESHOW_SLOW     4000

//max take picture number
#define QCAM_MAX_PICS_10  10
#define QCAM_MAX_PICS_20  20
#define QCAM_MAX_PICS_50  50
#define QCAM_MAX_PICS_NO_LIMIT  0

//preview FPS option list - real value * 1000
//These definitions will be removed once camera service layer
//support GetPFSList.
#define QCAM_FPS_30000  30000
#define QCAM_FPS_15000  15000
#define QCAM_FPS_7500   7500
#define QCAM_FPS_3750   3750

#define MAX_ICONS 6

// ISO type
#define QCAM_ISO_0    0
#define QCAM_ISO_1    1
#define QCAM_ISO_100  100
#define QCAM_ISO_200  200
#define QCAM_ISO_400  400
#define QCAM_ISO_800  800

//Multishot options
#define QCAM_MULTISHOT_OFF    0
#define QCAM_MULTISHOT_ON     1
#define QCAM_MULTISHOT_AE     2

// Quality type
typedef enum
{
  QCAM_QUALITY_LOW = 0,
  QCAM_QUALITY_MED,
  QCAM_QUALITY_HIGH,
  QCAM_QUALITY_PERCENT,
  QCAM_QUALITY_FILESIZE,
  QCAM_QUALITY_BOTH
}QCamQualityType;

// Encoding type
typedef enum
{
  QCAM_ENCODING_JPEG,
  QCAM_ENCODING_PNG,
  QCAM_ENCODING_RAW,
  QCAM_ENCODING_MEM,
  QCAM_ENCODING_YCBCR,
  QCAM_ENCODING_RAW_PREVIEW
}QCamEncodingType;

// JPEG Spec option
typedef enum
{
  QCAM_JPEG_QUALITY,
  QCAM_JPEG_FILESIZE,
  QCAM_JPEG_BOTH
}QCamJPEGSpec;

typedef enum
{
  QCAM_ICONS_NO,
  QCAM_ICONS_PREVIEW_ONLY,
  QCAM_ICONS_PREVIEW_ENCODE
}QCamIconState;

//Auto Exposure Algorithm type
typedef enum
{
  QCAM_AEC_FRAME_AVERAGE,
  QCAM_AEC_CENTER_WEIGHTED,
  QCAM_AEC_SPOT_METERING
}QCamAEmode;

typedef enum
{
  QCAM_GPS_DISABLED,
  QCAM_GPS_ENABLED,
  QCAM_GPS_DATA_VALID,
  QCAM_GPS_ACQUIRING,
  QCAM_GPS_FAILURE,
  QCAM_GPS_TIMEOUT,
  QCAM_GPS_INFO_UNAVAILABLE,
  QCAM_GPS_STATUS_COUNT
}QCamGPSState;

typedef enum
{
  QCAM_SOFT_MENU_NONE,
  QCAM_SOFT_MENU_TAKE_PHOTO,
  QCAM_SOFT_MENU_SETTINGS,
  QCAM_SOFT_MENU_MULTISHOT
}QCamSoftMenuType;

//Active menu id
typedef enum
{
  QCAM_MENU_NONE,
  QCAM_MENU_SELFTIMER,
  QCAM_MENU_SLIDESHOW_SPEED,
  QCAM_MENU_ENCODING,
  QCAM_MENU_GPS,
  QCAM_MENU_SAVE,
  QCAM_MENU_WHITEBALANCE,
  QCAM_MENU_EFFECTS,
  QCAM_MENU_SOUND,
  QCAM_MENU_PROPERTIES,
  QCAM_MENU_SIZE,
  QCAM_MENU_DELETEALL,
  QCAM_MENU_MAX_PICS,        // Max # of pictures that can be taken
  QCAM_MENU_FRAME_OVERLAY,
  QCAM_MENU_REFLECT,
  QCAM_MENU_MEM_USAGE,
  QCAM_MENU_FLASH,
  QCAM_MENU_STROBEFLASH,
  QCAM_MENU_REDEYE,
  QCAM_MENU_QUALITY,
  QCAM_MENU_AUTOEXPOSURE,
  QCAM_MENU_ANTIBANDING,
  QCAM_MENU_NIGHTSHOT_MODE,
  QCAM_MENU_PREVIEWFPS_LOW,
  QCAM_MENU_PREVIEWFPS_HIGH,
  QCAM_MENU_JPEGSPEC,
  QCAM_MENU_ISO,
  QCAM_MENU_AFMODE,
  QCAM_MENU_AFMODEMETERING,
  QCAM_MENU_MULTISHOT,
  QCAM_MENU_BESTSHOT,
  QCAM_MENU_AFRECT_MODE
}QCamMenuType;

typedef enum
{
  QCAM_STATE_NULL,
  QCAM_STATE_START_PREVIEW,
  QCAM_STATE_PREVIEW,
  QCAM_STATE_SNAPSHOT,
  QCAM_STATE_RECORD,
  QCAM_STATE_SAVE,
  QCAM_STATE_MENU,
  QCAM_STATE_QCAMCORDER
}QCamState;

#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif

typedef struct _QCam QCam;      //all variable needed in this application

// QCamera applet structure
struct _QCam
{
  AEEApplet        a;
  AEEDeviceInfo    di;
  AEERect          m_rcPreView; //preview area, in center if smaller than m_rcDisp
  AEERect          m_rcDisp;    //full display area
  ICamera         *m_pICam;
  IMenuCtl        *m_pIMenu;
  IMenuCtl        *m_pISoftMenu;
  IFileMgr        *m_pIFileMgr;
  IMedia          *m_pMedia;
  IImage          *m_pFrameImg;
  IPosDet         *m_pIPosDet;
  IDIB            *m_pFrameDIB;
  IDIB            *m_pFrame;
  IStatic         *m_pIStatic;
  byte            *m_pData;
#ifdef HEAP_CHECK
  IHeap           *m_pIHeap;
#endif //HEAP_CHECK
  CameraSensorInfo m_sensorInfo;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  uint16           m_nSensorId;
#endif //FEATURE_CAMERA_MULTI_SENSOR

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

  QCamState        m_eState;
  Qalbum_LaunchDS *m_pLaunchDS;
  QCamMenuType     m_activeMenu;
  QCamSoftMenuType m_activeSoftMenu;
  QCam_FileList   *m_pSoundList;

  QCamEncodingType m_encodingType;
  QCamQualityType  m_qualityType;
  QCamSaveType     m_saveLoc;
  QCamAEmode       m_autoExposure;
  QCamIconState    m_eIconState;
  QCamJPEGSpec     m_nJPEGSpec;
  //
  char             m_sCurrentFileName[FILE_NAME_SIZE];
  char             m_sFrameFileName[FILE_NAME_SIZE];
  char             m_sShutterSoundFile[FILE_NAME_SIZE];
  const char      *m_pResFile;
  int16            m_nTitleHeight;
  uint16           m_softMenuHeight;
  uint16           m_nSlideshowSpeed;
  uint16           m_nSelfTimerDelay;
  uint16           m_nFlash;
  uint16           m_nStrobeFlash;
  uint16           m_nQuality;
  uint16           m_nSize;
  uint16           m_nMaxSize;
  uint16           m_nMaxNumPics;   // Maximum # of pics that can be taken
  uint32           m_nProperties;
  uint8            m_nWhitebalance;
  uint8            m_nEffect;
  uint16           m_nAntibanding;
  boolean          m_bRedeyeReduce;
  uint16           m_nReflectPreview;
  boolean          m_bNightshotMode; // TRUE/FALSE - enable/disable nightshot mode
  uint32           m_nLowFPS;
  uint32           m_nHighFPS;
  uint32           m_nMaxFPS;
  uint32          *m_pListFPS;
  uint16           m_nSelHighFPS;
  uint16           m_nSelLowFPS;
  uint32           m_nSelMultishot;
  uint32           m_nMulPhotos;
  uint32           m_nPhotoInterval;
  uint32           m_nShutIdx;
  uint32           m_nBestshot;

  int32            m_nISO;
  int32            m_nMaxISO;
  uint32           m_nAFMode;
  uint16           m_nDelOpt;
  //
  uint16           m_wCurrentImage;
  uint16           m_wLastImageDay;
  uint16           m_wLastImageMonth;
  uint16           m_wLastImageYear;
  byte             m_cDateTime;
  byte             m_cPosition;
//
  uint16           m_nSelSound;
  uint16           m_nTotalSound;
  uint16           m_nLeftSec;
  uint16           m_wTakePicMenuSel;
  uint32           m_nFileSize;
  uint16           m_wLastKey;
  boolean          m_bKeyDown;
  boolean          m_bUpdatePreviewFrame;
  boolean          m_bSaveAppPic;
  boolean          m_bFocusStarted;
  boolean          m_bOneButtonSnapshot;
  boolean          m_bNeedToSend; // Set if send is pressed on the save menu
  //                                 // because SendPicture has to be called later'
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND  
  boolean          m_bSnapCaptureDone;    // Snapshot Capture done
  boolean          m_bSnapShutterSndDone; // Shutter Sound Playback done
  boolean          m_bNeedToEncodePicture;// Snapshot Capture done & Shutter Sound still playing  
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
  boolean          m_bRedEyeRemovalSupported;
  boolean          m_bNightShotSupported;
  boolean          m_bLASupported;
  boolean          m_bMultiShotSupported;
  boolean          m_bBracketingSupported;
  boolean          m_bBSMSupported;
  boolean          m_bDisplayLCDSize;
  boolean          m_b2ndDisplay;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  //
  int32            m_nBright;
  int32            m_nBrightnessMin;
  int32            m_nBrightnessMax;
  int32            m_nBrightnessStep;
  //
  int32            m_nExposure;
  int32            m_nExposureMin;
  int32            m_nExposureMax;
  int32            m_nExposureStep;
  //
  int32            m_nFocus;
  int32            m_nFocusMin;
  int32            m_nFocusMax;
  int32            m_nFocusStep;
  //
  int32            m_nContrast;
  int32            m_nContrastMin;
  int32            m_nContrastMax;
  int32            m_nContrastStep;
  //
  int32            m_nSharpness;
  int32            m_nSharpnessMin;
  int32            m_nSharpnessMax;
  int32            m_nSharpnessStep;
  //
#ifdef FEATURE_CAMERA7500
  int32            m_nHue;
  int32            m_nHueMin;
  int32            m_nHueMax;
  int32            m_nHueStep;
  //
  int32            m_nSaturation;
  int32            m_nSaturationMin;
  int32            m_nSaturationMax;
  int32            m_nSaturationStep;
  //
  uint32           m_nAFModeMetering;
  int32            m_nRectcount;
  AEERect         *m_pRect;
#endif //FEATURE_CAMERA7500
  //
  int32            m_nZoom;
  int32            m_nZoomMin;
  int32            m_nZoomMax;
  int32            m_nZoomStep;
  //
  AEEGPSInfo       m_GPSInfo;
  QCamGPSState     m_eGPSState;
  uint8            m_nGPSAccuracy;
  AEECallback      GPSInfoCB;
  //
  // For capping the # of pics that can be taken
  uint32           m_nMaxFileSize;
  //
  boolean          m_bCanTakePic;
  uint32           m_transparentValue;
  boolean          m_bShowMenu;
  boolean          m_bGPSmenu;
  int              m_nRetry;
  boolean          m_bMsgTimerOn;
  uint16           m_nMsgId;
  boolean          m_bGridOn;
  boolean          m_bSmallScreen;      // TRUE on SC1x and other smaller displays
  boolean          m_bHasAutoAntibanding;  // TRUE if sensor supports Auto option
  boolean          m_bFTL;        // TRUE if FTL memory is available
  int32              m_nPanoramic;

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
  IMMOverlayMgr   *m_pIMMOverlayMgr;
  IMMOverlay      *m_pIMMOverlayFull;
  IMMOverlay      *m_pIMMOverlayMenu;
  IMMOverlay      *m_pIMMOverlayInfo;
  IMMOverlay      *m_pIMMOverlayTitle;
#ifdef FEATURE_CAMERA7500
  IMMOverlay      *m_pIMMOverlayRect[QCAM_MAX_FOCUSRECTS];
#endif //FEATURE_CAMERA7500
#else
  IOverlay        *m_pIOverlay;
  OVERLAY_ID       m_mdpMenu_ovId;
  OVERLAY_ID       m_mdpTitle_ovId;
  OVERLAY_ID       m_mdpInfo_ovId;
#endif // FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  AEECLSID         m_coreUIClsId;
  IMMOverlay      *m_pOvFrame;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_CAMERA_HISTOGRAM
  IMMOverlay      *m_pOvHist;
  uint8            m_nPrvwHistogram;
  IBitmap         *m_pBmpHist;
  AEERect          m_rcHist;
#endif //FEATURE_CAMERA_HISTOGRAM
#ifdef FEATURE_CAMERA_PANORAMIC
  IMMOverlay      *m_pOvPanoramic;
#endif //FEATURE_CAMERA_PANORAMIC
#ifdef FEATURE_CAMERA_MOBICAT
  boolean          m_bMobiCaTon;
#endif //FEATURE_CAMERA_MOBICAT
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  boolean          m_bIconLoaded;
  ipl_icon_type   *iconList[MAX_ICONS+1];
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
#ifdef FEATURE_QVPHONE
  IVideoPhone	  *m_pIVP;
#endif // FEATURE_QVPHONE
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  boolean m_nDirectMode;
#endif
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
};

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
/*-------------------------------------------------------------------
Menu handler functions
-------------------------------------------------------------------*/
void Qcamera_OnTakePicture(QCam * pMe);
void Qcamera_OnOrientation(QCam * pMe, uint16 key);
void Qcamera_OnZoom(QCam * pMe, uint16 key);
#ifdef FEATURE_CAMERA_BRIGHTNESS
void Qcamera_OnBrightness(QCam * pMe, uint16 key);
#else
void Qcamera_OnExposure(QCam * pMe, uint16 key);
#endif
void Qcamera_OnFocus(QCam * pMe, uint16 key);
void Qcamera_OnContrast(QCam * pMe, uint16 key);
#if defined (FEATURE_CAMERA_V2) || defined (FEATURE_CAMERA7500)
void Qcamera_OnSharpness(QCam * pMe, uint16 key);
#endif //FEATURE_CAMERA_V2 || FEATURE_CAMERA7500
void Qcamera_OnAlbum(QCam * pMe, uint16 key);
void Qcamera_OnSlideshow(QCam * pMe, uint16 key);
void Qcamera_OnFrame(QCam * pMe, uint16 key);
#ifndef FEATURE_CAMERA_V2
void Qcamera_OnGPS(QCam * pMe, uint16 key);
#endif //FEATURE_CAMERA_V2
void Qcamera_OnSettings(QCam * pMe, uint16 key);
#ifdef FEATURE_APP_QCAMCORDER
void Qcamera_OnQCamcorder(QCam * pMe, uint16 key);
#endif
#ifdef FEATURE_CAMERA_MOBICAT
void Qcamera_OnMCT(QCam * pMe, uint16 key);
#endif
#ifndef FEATURE_CAMERA_V2
void Qcamera_OnSimulateDisplay(QCam *pMe, uint16 key);
#endif //FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA7500
void Qcamera_OnHue(QCam * pMe, uint16 key);
void Qcamera_OnSaturation(QCam * pMe, uint16 key);
void Qcamera_OnEffect(QCam * pMe, uint16 key);
void Qcamera_OnHistogram(QCam * pMe, uint16 key);
#endif //FEATURE_CAMERA7500
/*-------------------------------------------------------------------
//Take picture menu mapping:
-------------------------------------------------------------------*/
// execute menu on sel function definition
typedef void (*PFNMENUSEL) (QCam *pMe, uint16 key);

// event handler sanity check definition
typedef struct _CamMenuKeyHandler
{
  uint16 wMenuID;
  PFNMENUSEL MenuOnKeyFunc;
} CamMenuKeyHandler;

// edit menu On select key handler function table
static const CamMenuKeyHandler CamMenuKeyHandlers[] = {
  {IDS_TAKE_PIC,      Qcamera_OnOrientation},
  {IDS_ZOOM,          Qcamera_OnZoom},
#ifdef FEATURE_CAMERA_BRIGHTNESS
  {IDS_BRIGHTNESS,    Qcamera_OnBrightness},
#else
  {IDS_EV,            Qcamera_OnExposure},
#endif //FEATURE_CAMERA_BRIGHTNESS
  {IDS_FOCUS,         Qcamera_OnFocus},
  {IDS_CONTRAST,      Qcamera_OnContrast},
#if defined (FEATURE_CAMERA_V2) || defined (FEATURE_CAMERA7500)
  {IDS_SHARPNESS,     Qcamera_OnSharpness},
#endif //FEATURE_CAMERA_V2 || FEATURE_CAMERA7500
#ifdef FEATURE_CAMERA7500
  {IDS_HUE,           Qcamera_OnHue},
  {IDS_SATURATION,    Qcamera_OnSaturation},
  {IDS_EFFECTS,       Qcamera_OnEffect},
  {IDS_HISTOGRAM,  Qcamera_OnHistogram},
#endif //FEATURE_CAMERA7500
  {IDS_ALBUM,         Qcamera_OnAlbum},
  {IDS_SLIDESHOW,     Qcamera_OnSlideshow},
  {IDS_FRAME,         Qcamera_OnFrame},
#ifndef FEATURE_CAMERA_V2
  {IDS_GPS_SETUP,     Qcamera_OnGPS},
#endif
  {IDS_SETTINGS,      Qcamera_OnSettings},
#ifdef FEATURE_APP_QCAMCORDER
  {IDS_QCAMCORDER,    Qcamera_OnQCamcorder},
#endif
#ifdef FEATURE_CAMERA_MOBICAT
  {IDS_MCT,           Qcamera_OnMCT},
#endif
#ifndef FEATURE_CAMERA_V2
  {IDS_SIMULATE_2NDLCD, Qcamera_OnSimulateDisplay}
#endif
};
/*------------------------------------------------------
Helper function
----------------------------------------------------------*/
void Qcamera_PlayShutterSound(QCam* pMe);
void Qcamera_UpdateFrame(QCam *pMe);
void Qcamera_SavePhoto(QCam *pMe);
void Qcamera_TurnOnCameraPreview(QCam* pMe);
void Qcamera_GetDirLen(QCam *pMe, uint32 *rootDirLen, uint32 *dataDirLen);
boolean Qcamera_GetFileName(QCam *pMe);
AEERect Qcamera_GetPreviewRect(QCam *pMe);
uint16 Qcamera_GetMaxSizeId(const CameraSensorInfo *pSensorInfo);
uint32 *Qcamera_GetListFPS(QCam *pMe);
AEESize *Qcamera_GetSize(QCam *pMe);
#ifndef FEATURE_CAMERA_V2
int Qcamera_SetDisplaySize(QCam *pMe, AEESize *pSize);
#endif //#ifndef FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
int Qcamera_SetDirectMode(QCam *pMe, boolean bDirectMode);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
void Qcamera_MemUsage(QCam* pMe);
void Qcamera_DisplayTakePicMenuName(QCam* pMe, int item);
boolean Qcamera_PromptForDelAll(QCam * pMe);
boolean Qcamera_IsWideSize(QCam * pMe);
void Qcamera_DisplayTimedImg(QCam* pMe, PFNNOTIFY ph, uint16 imgID);
void Qcamera_DisplayInfo(QCam *pMe);
void Qcamera_DrawBusyImage(QCam *pMe, int x, int y);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
/*************************************************************************/
//ipl and camera related functions, need to update with the OEM APIs.
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
boolean Qcamera_LoadIconArray(QCam * pMe, const IDIB *pDIB, int index);
boolean Qcamera_SetIconArray(QCam * pMe);
void Qcamera_ClearIcon(QCam * pMe);
IDIB *Qcamera_ComposeStamp(QCam * pMe, IDIB *pInDIB, AEESize size);
#endif //#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

#endif //#ifdef FEATURE_APP_QCAMERA

#endif // QCAMERA_H

/*===========================================================================
                      MAIN QCAMERA LOGIC GO THROUGH

  1. Qcamera start up and initialize
  2. Enter preview mode
  3. Take picture(s)
  4. Save a picture
  5. View a photo (in slide show or album mode)
  6. Build and execute a setting menu
  7. Add a frame overlay to preview image
  8. Save Multiple Pictures
  **************************************************************************
1. Qcamera start up and initialize
  - On EVT_APP_START in Qcamera_HandleEvent()
    Qcamera_Init() -> start ICamera
      Create instances for all BREW interfaces
      Qcamera_GetConfigRecord()
      Initialize menu controls
      Qcamera_CreateCamera()
      Initialize MDP display if feature defined

    ISHELL_HandleEvent(EVT_COMMAND, IDS_TAKE_PIC) -> start preview mode.

  **************************************************************************
2. Enter preview mode
  - On EVT_COMMAND in Qcamera_HandleEvent()
    //(wParam == IDS_TAKE_PIC)
    IDISPLAY_ClearScreen()  //clear old screen
    IDISPLAY_Update()       //update before mpd deregister.
    ISHELL_SetTimer(100, Qcamera_CameraPreview) //need timer to avoid gragh task conflict

  - In Qcamera_CameraPreview()
    //if (bAddFrame)
    //Load a frame and add it to overlay is a long processing.
    //Need to wait for callback before calling ICAMERA_Preview
    - Qcamera_LoadFrameOverlay()
    else
    - QCam_AddRemoveIconsStamps();
    - ICAMERA_Preview(); -> camera will call back after viewfinder ready

    ICAMERA_GetParm(CAM_PARM_ZOOM) // update current zoom
    Qcamera_MdpRegisterPreview()   // register mdp for preview if mdp defined.
    QcamMenus_BuildSoftMenu(QCAM_SOFT_MENU_TAKE_PHOTO) //build the Take Picture menu

    //Whenever viewfinder gets a new frame, it sends callback here
    //to update the preview image until preview mode is stopped.
  - On CAM_STATUS_FRAME in Qcamera_CamEventNotify()
    Qcamera_UpdateFrame()
      ICAMERA_GetFrame()
      IDISPLAY_BitBlt() -> camera will call mdp to display and update if mdp defined.
      //if (#ifndef FEATURE_MDP)
      - IDISPLAY_Update()

  **************************************************************************
3. Take picture(s)
  - On EVT_KEY in Qcamera_HandleEvent()
    //(pMe->m_activeSoftMenu == QCAM_SOFT_MENU_TAKE_PHOTO)
    // Select key on several menu icons
    Qcamera_TakePicKeyHandler(AVK_SELECT)
      Qcamera_OnTakePicture()
        Qcamera_StopIfAFIndicator()
        QCam_CheckProperties()
        Qcamera_TakePicture()
          Qcamera_IsSpaceAvailable() -> stop give message if not enough space
          Qcamera_StopCamera()
          //If timer is not 0, start the timer.
          //The timer will count down to 0
          //and start Qcamera_PreTakePicture() in Qcamera_selfTimerCB()
          Qcamera_DisplaySelfTimerBar()
          Qcamera_StartFocus()
            //If auto focus is supported
            -Qcamera_UpdateAFIndicator()
            -Qcamera_PreTakePicture()
            -ICAMERA_SetParm()
            -Qcamera_ResetAFIndicator()
          Qcamera_PreTakePicture()
            //If a shutter sound is selected, play the sound
            //Start take picture process in Qcamera_MediaNotify() when sound stops
            Qcamera_PlayShutterSound()
            //Before take picture, have to stop preview.
            //When viewfinder is stop, it will send the callback to continue.
            pMe->m_eState = QCAM_STATE_SNAPSHOT
              Qcamera_StopPreview()

  - On CAM_STATUS_DONE in Qcamera_CamEventNotify()
    // In QCAM_STATE_SNAPSHOT state
    Qcamera_DoSnapshot()
      Qcamera_ToggleMenuDisplay() //hide the overlay menu if using mdp display
      //if (pMe->m_nMaxFileSize > 0)
      - ICAMERA_SetMaxFileSize()    //set file size if using file size control
      //else
      - ICAMERA_SetQuality()  //set quality if using quality control
      ICAMERA_SetVideoEncode() //set encoding type
      ICAMERA_SetParm(CAM_PARM_GPSINFO) //set GPS info
      pMe->m_eState = QCAM_STATE_SAVE
      ICAMERA_DeferEncode()
      //if ( pMe->m_nMulPhotos > 1 )
      -ICAMERA_RecordSnapshotEx()//call camera to take multiple pictures
      //else
      -ICAMERA_RecordSnapshot()  //call camera to take a picture. it will callback



  //For multishot, this is called back once for each frame
  - On CAM_STATUS_FRAME in Qcamera_CamEventNotify() when state is QCAM_STATE_SAVE
    //If this is the first frame
    Qcamera_UpdateFrame()

  - On CAM_STATUS_DONE in Qcamera_CamEventNotify() when state is QCAM_STATE_SAVE
    if (pMe->m_nMulPhotos > 1) 		//multishot mode
    -Qcamera_BuildMultishotMenu()
       Qcamera_InitMultishotMenu()	//Initalize menu
       Qcamera_GetRotationType()    //Determine how to rotate the icons in menu
       Qcamera_GetMultishotIcon()   //Call for each frame to obtain an icon
       pMe->m_activeSoftMenu = QCAM_SOFT_MENU_MULTISHOT

  **************************************************************************
4. Save a picture
  - On CAM_STATUS_DONE in Qcamera_CamEventNotify()
    // In QCAM_STATE_SAVE state
    QCamDisp_MdpRegisterFullScreen() //register full screen display if mdp defined
    //if not prompt to save
    - Qcamera_UpdateFrame()   //Display the picture taken by camera.
    - Qcamera_DoAutoSave()
        Qcamera_GetFileName() //Auto generate a file name for the new photo
        Qcamera_DisplaySavedImageName() //Display the short file name on screen
        Qcamera_SavePhoto()   //Start save photo processing
    //else
    - ISHELL_CreateDialog(IDD_SAVE_PIC)

    //If prompt to save is selected, logic turns to here
  - On EVT_DIALOG_INIT in Qcamera_HandleEvent()
    Qcamera_MapEventHandlers(IDD_SAVE_PIC)
      IDIALOG_SetEventHandler(QCamDlgSave_EventHandler)

    - On EVT_DIALOG_START in QCamDlgSave_EventHandler <QcamDlgSave.c>
      Create static field for date directory of long file name display.
      Create text field for short file name display and edit.
      Create menu buttons for user option (Save, Cancel, Send)

    //Update the frame after creating the dialog
    //Otherwise, the take picture frame is hide under the dialog.
    - Qcamera_UpdateFrame()

    //If Save button is pressed, BREW IMenuCtl will handle the key event
    //and send a EVT_COMMEND back. Logic turns to here
  - On EVT_COMMAND in QCamDlgSave_EventHandler()
    QCam_GetValidFileName() //check if user updated file is valid
    ISHELL_EndDialog()      //end the save dialog
    Qcamera_SavePhoto()     //Start save photo processing

  - In Qcamera_SavePhoto()
    ICAMERA_SetMediaData()    //Pass the destination file name or buffer to camera
    ICAMERA_EncodeSnapshot()  //Start encoding, camera will call back after encoding

  - On CAM_CMD_ENCODESNAPSHOT in Qcamera_CamEventNotify()
    Write the buffer to a efs file if it is MEM mode encoding.
    Qcamera_DisplayPhotoSavedImage()
      QCamDisp_DrawIcon() //Draw the saved image message if prompt to save
      Qcamera_DisplaySavedImageName()
      ISHELL_SetTimer(timeout, Qcamera_TurnOnCameraPreview) -> go back to preview mode

  **************************************************************************
5. View a photo (in slide show or album mode)
  - On AVK_SELECT of Qcamera_TakePicKeyHandler()
    CamMenuKeyHandlers[index].MenuOnKeyFunc() -> map to an Onkey function with sel key
    //if Album key is selected
    Qcamera_OnAlbum()
    //if Slideshow key is selected
    Qcamera_OnSlideshow()
      Qcamera_PreLaunchViewer() //VIEWER_MODE_ALBUM or VIEWER_MODE_SLIDESHOW

  - In Qcamera_PreLaunchViewer()
    Fill pMe->m_pLaunchDS with selected data
    Qcamera_StopCamera() -> Stop the preview mode, camera will call back when done

  - On CAM_STATUS_DONE in Qcamera_CamEventNotify()
    //if (pMe->m_pLaunchDS)
    ISHELL_StartAppletArgs() -> Start Qalbum application with launch data.
    //Qcamera is suspended when Qalbum is launched. All user interface events
    //is passed to Qalbum by BREW. Look at Qalbum logic go through in qalbum_i.h.
    //When Qalbum is closed, Qcamera is resumed by BREW. It returns to preview mode
    //on EVT_APP_RESUME of Qcamera_HandleEvent().

  **************************************************************************
6. Build and execute a setting menu
  - On AVK_SELECT of Qcamera_TakePicKeyHandler()
    CamMenuKeyHandlers[index].MenuOnKeyFunc() -> map to an Onkey function with sel key
    Qcamera_OnSettings()
      Qcamera_StopCamera() -> Stop the preview mode, camera will call back when done
      pMe->m_eState = QCAM_STATE_MENU

  - On CAM_STATUS_DONE in Qcamera_CamEventNotify()
    // In QCAM_STATE_MENU state
    QCamDisp_MdpRegisterFullScreen() //register full screen display if mdp defined
    QcamMenus_BuildSoftMenu(QCAM_SOFT_MENU_SETTINGS)

  - In QcamMenus_BuildSoftMenu() <QcamMenus.c>
    QcamMenus_BuildSettingsMenu()
      QCam_InitMenuData()
      QCam_AddMenuItem() -> for loop to add all items
      QcamMenus_DoSettingsMenu() -> To build a list menu of current selected item

    //When user hits a left/right key in the setting menu, the event will be
    //mapped to setting menu key handler by Qcamera_HandleEvent().
  - On AVK_LEFT or AVK_RIGHT in Qcamera_SettingsKeyHandler()
    QcamMenus_DoSettingsMenu()
    IMENUCTL_HandleEvent()

  - In QcamMenus_DoSettingsMenu()
    CSettingMenuHandlers[index].MenuBuildFunc() -> map to a build menu function
    IMENUCTL_Redraw()
    IDISPLAY_UpdateEx()

    //If user selects an up/down key in the list menu, IMENUCTL_HandleEvent()
    //will handle the item hightlight and send a comman event back
  - On EVT_COMMAND in Qcamera_HandleEvent()
    //(pMe->m_activeMenu != QCAM_MENU_NONE)
    QcamMenus_ExecuteActiveMenu()
      CExecMenuHandlers[i].MenuExecFunc() -> map to an execute menu function
    //In the execute menu function, it update the radio icon or the check mark
    //with current focus item. Some may call ICAMERA_SetParm to pass the new
    //parameter to the camera.

  **************************************************************************
7. Add a frame overlay to preview image
  - On AVK_SELECT of Qcamera_TakePicKeyHandler()
    CamMenuKeyHandlers[index].MenuOnKeyFunc() -> map to an Onkey function with sel key
    Qcamera_OnFrame()
      Qcamera_GetMatchSizeFrameDir()
    //Display timed message if no matched frame size directory.
      Qcamera_PreLaunchViewer(VIEWER_MODE_FRAME)
    //See logic 5 for launching Qalbum application.
    //User will select a new frame (or no frame) in Qalbum if frame files are existing.
    //Qalbum will be closed when a frame is selected and return to Qcamera.

  - On EVT_APP_RESUME in Qcamera_HandleEvent()
    //if (pMe->m_pLaunchDS->mode == VIEWER_MODE_FRAME)
    STRCPY(pMe->m_sFrameFileName, pMe->m_pLaunchDS->sFrameFileName)
    ISHELL_HandleEvent(EVT_COMMAND, IDS_TAKE_PIC) -> start preview mode.
    //See logic 3 for entering the preview mode.
    //When the add frame flag is set, Qcamera_LoadFrameOverlay() is called

  - In Qcamera_LoadFrameOverlay()
    Load the image from selected frame file
    IIMAGE_Notify(Qcamera_FrameImageNotify) -> Handle the image in callback

  - In Qcamera_FrameImageNotify()
    Get the IDIB pointer from the image
    Get the transparency color from the IDIB
    Qcamera_IsSizeMatch() //Check if frame size matches the image size
    Convert the image data from RGB888 to RGB565 color format
    Qcamera_AddRemoveIconsStamps() -> Combine with the stamps if needed and add overlay
      ICAMERA_ClearOverlay() //clear old frame overlay
      - Qcamera_ClearStamp() //if no icons selected
          Qcamera_ClearIcon() -> clear previor loaded icons
      - Qcamera_AddStampPreview() //if icons are selected for preview only
          Qcamera_SetIconArray() -> add the icons overlay on foreground
      - Qcamera_AddStampEncode()  //if icons are selected for add stamp
          Qcamera_ComposeStamp()  //compose the icons with the frame
      Qcamera_AddOverlay()
        ICAMERA_AddOverlay() -> Call camera to add the frame overlay to preview.
    ICAMERA_Preview() -> start preview.
  **************************************************************************
8. Save Multiple Pictures
  On EVT_KEY in Qcamera_HandleEvent()
    //If key is AVK_SELECT  (Save the selected frame)
    -Qcamera_MultishotKeyHandler(AVK_SELECT)
       //set pMe->m_nShutIdx to index of the selected menu item
       //if (pMe->m_bMultishotSaved[pMe->m_nShutIdx] = FALSE)  image has not been saved
       pMe->m_bMultishotSaved[pMe->m_nShutIdx]  = TRUE
       Qcamera_DoAutoSave
         Qcamera_GetFileName
         Qcamera_DisplaySavedImageName
         Qcamera_SavePhoto
    //else (Save all frames that haven't been saved yet)
    -Qcamera_MultishotKeyHandler(AVK_STAR)
    -pMe->m_bMultishotSaveNext = TRUE
    -Qcamera_MultishotSaveNextFrame()			//Save the next frame that hasn't been saved
       Qcamera_DoAutoSave()
         Qcamera_GetFileName
         Qcamera_DisplaySavedImageName
         Qcamera_SavePhoto
       pMe->m_nShutIdx = last saved frame index

       

  //This is called back after the first frame has been saved
  On CAM_CMD_ENCODESNAPSHOT in Qcamera_CamEventNotify()
  //if (pMe->m_bMultishotSaveNext  && pMe->m_nShutIdx < pMe->m_nMulPhotos)
  -Qcamera_MultishotSaveNextFrame(pMe)	
    //follow the same logic as above for this call
  -pMe->m_bMultishotSaveNext = FALSE if this was the last frame  
  ===========================================================================*/

