/*===========================================================================

FILE: qcamera_i.h

GENERAL DESCRIPTION
    This is the private header file only used by qcamera.c

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/qcamera_i.h#5 $ $DateTime: 2009/04/15 00:19:29 $ $Author: saigopal $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/15/09  sai    Added support for new feature, USB MSD as save location
 12/18/08  KC     Added support for Raw preview capture.
 11/09/07  NC     Added support to FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE.
 10/04/07  SK     Fixed compilation issue when FEATURE_CAMERA_BURST_MODE is not defined
 09/05/07  NC     Fixed some compiling issues with feature condition change.
 08/30/07  KD     Added definition for Qcamera_BuildSaveDlg()
 08/17/07  KV     Added Qcamera_DiagSelectMultishotPicture()
 08/07/07  SK     UI Changes for Panoramic Stitch feature
 07/30/07  KD     Added function headers for the enhanced multishot UI
 05/28/07  KV     Added Qcamera_CheckListFps static function prototype.
 04/19/07  SK     UI Support for WVGA Display
 04/05/07  NC     Added feature condition for QCamera_GetOvObj to avoid compiler warning
 03/16/07  NC     Added Qcamera_ClearAFRects for focus windows clean up.
 02/28/07  NC     Added Qcamera_SetPreviewReady for direct mode update.
 02/07/07  NC     Redesigned preview histogram feature.
 02/06/07  NC     Optimized draw focus rect feature.
 01/11/07  SK     Updated QCam_PictureSize
 11/29/06  NC     Fixed IMMOverlay full screen register issue.
 11/22/06  ZL     Switched to use ISTATIC at memory usage screen.
 11/20/06  NC     Added shared function Qcamera_StopPreview.
 11/15/06  SG     IDIAG Support for QMobicaT
 09/07/06  NC     Added automation support for Multishot feature and IsSupport.
 09/05/06  NC     Set condition FEATURE_CAMERA_SUPPORT_ICON_ARRAY for icon overlay.
 09/01/06  NC     Filter out file/function for target compatible.
 06/14/06  JR     Added support for FTL memory.
 06/08/06  NC     Added a new function Qcamera_InitVals to simpligy init code.
 04/06/06  NC     Remove Qcamera_DrawBusyImage() to qcamera.h for public use.
 01/19/06  JN     Added smooth zoom support.
 10/14/05  NC     Added 3MP, 4MP and 5MP QCam_PictureSize.
 09/16/05  NC     Removed Qcamera_GetMaxSizeId to qcamera.h for using in other files.
 09/15/05  NC     Lint errors clean up.
 09/06/05  NC     Changed two function return types to boolean.
 07/29/05  NC     Added Qcamera_DiagShutterSound()
 05/24/05  NC     Removed Qcamera_UpdateEVPerStep() and Qcamera_ConvertToDisplayEV().
 05/12/05  NC     Updated QCam_PictureSize and added param to Qcamera_DeleteAllFilesResponse().
                  Removed Qcamera_ValueInDataList().
 05/09/09  NC     Added Qcamera_StopIfAFIndicator().
 04/29/05  NC     Removed Qcamera_UpdateGPSIcon().
 04/07/05  NC     Added new shared functions for MemUsage display.
 03/24/05  NC     Added new functions and removed unused function.
 03/03/05  NC     Updated QCam_PictureSize and param for Qcamera_AddOverlay()
 03/01/05  NC     Updated parameters for Qcamera_ValueInDataList()
 02/18/05  NC     Defined all time out values for timers.
 01/17/05  NC     Code clean up. Updated function names.
 01/05/05  NC     Added a new file for qcamera static prototypes.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "qcamera.h"

#ifdef FEATURE_APP_QCAMERA
#include "QcamMenus.h"
#include "QcamDlgSave.h"
#include "QcamDlgSize.h"
#ifndef FEATURE_CAMERA_V2
#include "QcamDlgMultiShot.h"
#include "IIPL.h"
#include "IPL.BID"
#endif //FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif


#define RGB_GREEN           (MAKE_RGB(0, 0xff, 0))
#define RGB_RED             (MAKE_RGB(0xff, 0, 0))

#define DEFAULT_LATITUDE    6897300
#define DEFAULT_LONGITUDE   -21810381
#define DEFAULT_ALTITUDE    645

#define TIMEOUT_67    67    //67 ms smooth zoom interval, simulates 15 fps
#define TIMEOUT_100   100   //100 ms UI work aroud before return to preview
#define TIMEOUT_300   300   //300 ms addition delay for UI work around
#define TIMEOUT_500   500   //500 ms focus fail indicator
#define TIMEOUT_1000  1000  //1 s selftimer count down
#define TIMEOUT_1200  1200  //1.2 s message display for user eye catching
#define TIMEOUT_2000  2000  //2 seconds focus done indicator


#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif

// [Image Size][Image Quality (low, med, hi)]
// Picture size in kilobytes
// Index number represents encoding type and quality info
// From left to right:
// JPEG Quality Low
// JPEG Quality Mid
// JPEG Quality High
// PNG - 1/2 H x W
// RAW image always get the max resolution the sensor can support
// Calculate its size separatly
// For exif header add extra 65k for image sizes above or eual to VGA and 35k for images
// bellow VGA Resolutions
int QCam_PictureSize[QCAM_SIZE_COUNT][4] =
{
  {511, 984,1850,2585},   // QCAM_SIZE_MAX -dummy, same as last one
  {511, 984,1850,2585},   // QCAM_SIZE_QSXGA
  {427, 811,1514,2112},   // QCAM_SIZE_QXGAP
  {344, 638,1179,1638},   // QCAM_SIZE_QXGA
  {235, 415, 745,1025},   // QCAM_SIZE_UXGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {143, 195, 315,720},    // QCAM_SIZE_SXGA
  {138, 187, 295,679},    // QCAM_SIZE_4VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {112, 143, 212,458},    // QCAM_SIZE_XGA
  {104, 123, 165,305},    // QCAM_SIZE_SVGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 90, 102, 130,218},    // QCAM_SIZE_VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 43,  47,  65, 80},    // QCAM_SIZE_CIF
  { 40,  43,  56, 73},    // QCAM_SIZE_QVGA
  { 39,  40,  42, 48},    // QCAM_SIZE_QCIF
  { 38,  39,  43, 45},    // QCAM_SIZE_QQVGA
  { 38,  38,  40, 45}     // QCAM_SIZE_STAMP
};

/*===============================================================================
                     MACROS
=============================================================================== */
#define QCAM_HAS_SHUTTER_SOUND(x)  (STRCMP(x, QCAM_SHUTTER_FILE))
#define IS_IMGID(x) ((x > 5000) && (x < 6000))
/*===============================================================================
LOCAL FUNCTION DECLARATIONS
=============================================================================== */
static boolean Qcamera_CreateCamera(QCam *pMe);
static void Qcamera_ReleaseCamera(QCam *pMe);
static boolean Qcamera_InitData(QCam *pMe);
static boolean Qcamera_Init(QCam *pMe);
static void Qcamera_FreeAppData(QCam *pMe);
static void Qcamera_GetConfigRecord(QCam *pMe);
static void Qcamera_SetConfigRecord(QCam *pMe);

/*****************  Event Handlers for Camera Application *********************/
static boolean Qcamera_HandleEvent(QCam *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Qcamera_SettingsKeyHandler(QCam *pMe, AEEEvent eCode, uint16 key);
static boolean Qcamera_TakePicKeyHandler(QCam *pMe, AEEEvent eCode, uint16 key);
static boolean Qcamera_MapEventHandlers(QCam *pMe, IDialog *pIDialog, uint16 wDlgResID);
static boolean Qcamera_GPSHandler(QCam *pMe, AEEEvent eCode, uint16 key);

/*****************  Notifiers for Camera Application ****************/
static void Qcamera_FrameImageNotify(void *pUser, IImage * pImage, AEEImageInfo * pi, int nErr);
static void Qcamera_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
static void Qcamera_CamEventNotify(QCam *pMe, AEECameraNotify *pcn);

//Self timer and shutter sound functions
static void Qcamera_DisplaySelfTimerBar(QCam *pMe);
static void Qcamera_selfTimerCB(QCam *pMe);
static void Qcamera_StopSound(QCam *pMe);

//GPS functions
static uint16 Qcamera_GetGPSid(QCam *pMe);
static void Qcamera_GetGPSInfo(QCam *pMe);
static void Qcamera_GetGPSInfoCB(void *pUser);

//Frame and stamp functions
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static uint16 Qcamera_GetIconID(uint32 id);
static void Qcamera_LoadIcons(QCam *pMe);
static void Qcamera_ClearStamp(QCam *pMe);
static void Qcamera_AddStampPreview(QCam *pMe);
static void Qcamera_AddStampEncode(QCam *pMe);
static void Qcamera_AddRemoveIconsStamps(QCam *pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static void Qcamera_AddOverlay(QCam *pMe, const IDIB *pDIB);
static boolean Qcamera_LoadFrameOverlay(QCam *pMe);

//Display functions
static void Qcamera_DisplayTakePicErr(QCam *pMe, uint16 msgID);
static void Qcamera_DisplayTimedMsg(QCam *pMe, PFNNOTIFY ph, uint16 msgID);
static void Qcamera_DisplaySavedImageName(QCam *pMe);
static void Qcamera_DisplayPhotoSavedImage(QCam *pMe);
static void Qcamera_GetTitleText(QCam *pMe, uint16 itemID, AECHAR *txtBuf, int size);
static void Qcamera_DisplayTitle(QCam *pMe, uint16 titleID, uint16 img1ID,
                                 const AECHAR *txt1Str, uint16 img2ID, uint16 txt2ID);
static void Qcamera_UpdateAFIndicator(QCam *pMe, uint16 iconID);
static void Qcamera_ResetAFIndicator(QCam *pMe);
static void Qcamera_StopIfAFIndicator(QCam *pMe);

static void Qcamera_MemUsageBlock(QCam *pMe, boolean bMMC, boolean bFTL 
  #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  , boolean bUSB 
  #endif
);

#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE


/**************** MDP related functions for Camera application *******************/
#ifdef FEATURE_OVERLAY
static void Qcamera_AddGrid(QCam *pMe);
static void Qcamera_MdpRegisterPreview(QCam *pMe);
static void Qcamera_ToggleMenuDisplay(QCam *pMe);
#endif // FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
static void Qcamera_RegisterOvScreen(QCam *pMe, 
                                     boolean bFullScreen, 
                                     boolean bTransparent);
#ifdef FEATURE_CAMERA7500
static void Qcamera_UpdateAFRects(QCam *pMe);
static void Qcamera_ClearAFRects(QCam *pMe);
#endif // FEATURE_CAMERA7500
#endif //FEATURE_MMOVERLAY
#if defined FEATURE_CAMERA_HISTOGRAM || defined FEATURE_CAMERA_PANORAMIC
static boolean QCamera_GetOvObj(QCam *pMe, 
                                IBitmap *pBitmap, 
                                AEERect rc, 
                                IMMOverlay **ppMMOverlay);
#endif //FEATURE_CAMERA_HISTOGRAM || FEATURE_CAMERA_PANORAMIC

//Preview Histogram functions
#ifdef FEATURE_CAMERA_HISTOGRAM
static int Qcamera_GetHistogramInfo(QCam *pMe);
static int QCamera_SetHistogram(QCam *pMe);
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
//Panoramic mode functions
#ifdef FEATURE_CAMERA_PANORAMIC
static void Qcamera_UpdateNumberIconbar (QCam *pMe);
static IBitmap *Qcamera_GetPartialFrame(QCam *pMe, IIpl *pIIpl);
static void Qcamera_GetOverlayRc(QCam *pMe, AEERect *pRc);
static boolean Qcamera_UpdatePanoramicFrame (QCam *pMe);
static void Qcamera_UpdateStitchFrame(QCam *pMe);
#endif //FEATURE_CAMERA_PANORAMIC
static void Qcamera_BuildSaveDlg(QCam *pMe);
/*------------------------------------------------------
Helper function
----------------------------------------------------------*/
static boolean Qcamera_IsValueInBound(uint32 val, uint32 min, uint32 max);
static boolean Qcamera_IsSpaceAvailable(QCam *pMe);
static boolean Qcamera_ValidateDir(QCam *pMe);
static void Qcamera_InitVals(QCam *pMe, int16 nId, int32 nStep, int32 *pVal,
                             int32 *pMin, int32 *pMax, int32 *pStep);

static int32 Qcamera_UpdatePerStep(QCam *pMe, int32 nCur, int32 step,
                                   int32 max, int32 min, uint16 key);
static boolean Qcamera_PreLaunchViewer(QCam *pMe, QalbumLaunchMode mode);
static boolean Qcamera_StopCamera(QCam *pMe);
static void Qcamera_StopPreview(QCam *pMe);
static void Qcamera_PreTakePicture(QCam *pMe);
static void Qcamera_TakePicture(QCam *pMe);
static void Qcamera_DoSnapshot(QCam *pMe);
static void Qcamera_DoAutoSave(QCam *pMe);
static void Qcamera_DoSmoothZoom(QCam *pMe);

static void Qcamera_CameraPreview(QCam *pMe);
static boolean Qcamera_DeleteAll(QCam *pMe, uint16 id);
static void Qcamera_DeleteAllFilesResponse(QCam *pMe, uint16 selId);

static int Qcamera_GetRawSize(QCam *pMe);
static int Qcamera_GetRemainPicNum(QCam *pMe, boolean bMMC, boolean bFTL 
  #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  , boolean bUSB 
  #endif
  );
static void Qcamera_GetDefImageSize(QCam *pMe);
static int Qcamera_GetQualityIndex(QCam *pMe);
static const char *Qcamera_GetDir(QCamEncodingType enc, QCamSaveType saveLoc);
static const char *Qcamera_GetExtension(QCamEncodingType enc);
static const char *Qcamera_GetMatchSizeFrameDir(QCam *pMe);
static void Qcamera_SetPreviewReady(QCam *pMe);
static void Qcamera_CheckListFps(QCam *pMe);
/*------------------------------------------------------
//Test automation functions
----------------------------------------------------------*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#endif //#ifdef FEATURE_APP_QCAMERA
