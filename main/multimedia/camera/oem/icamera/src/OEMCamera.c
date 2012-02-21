/*===========================================================================

FILE: OEMCamera.c

SERVICES: ICamera interface implementation

DESCRIPTION
   This file implements BREW ICamera. ICamera controls the device camera
   and allows recording of snapshot or movie.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright ?1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/oem/icamera/rel/3.1.5/latest/src/OEMCamera.c#13 $ $DateTime: 2008/11/30 00:35:48 $ $Author: shilpam $

 when      who    what, where, why
 --------  -----  --------------------------------------------------------------
11/30/08  sm     Added support for preview rotation
 11/25/08  RK    Added a New Feature ACC(Audio Camera Concurrency) support if 
                 FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND is defined.
 11/25/08  KC     Modified the bRange value returned for CAM_PARM_SIZE_LIST as the lists returned are discrete ones.
                  Invalidating the frame bitmap in OEMCamera_Framesblt in the frame call back mode.
 11/13/08  sanal  Merged state machine changes in OEM Camera from mainline, this 
                  prevents unexpected behaviour with fast start/stop recording
 09/10/08  shiven Updating GetParm support for CAM_PARM_BITRATE
 08/18/08   KC     Added support for new parameters, CAM_PARM_PIXEL_COUNT,CAM_PARM_VIDEO_ENCODE_LIST,CAM_PARM_AUDIO_ENCODE_LIST
                  CAM_PARM_DISPLAY_SIZE_LIST; modified handling CAM_PARM_SIZE_LIST ; made setting CAM_PRAM_DISPLAY_SIZE synchronous
 08/11/08  KC     Modified setting SET_SIZE and DISPLAY_SIZE to synchoronous mechanism.
                  Controlling pme->m_noMoreEncode flag at runtime.
 07/29/08  ps     When post card recording is not supported return UNSUPPORTED error.(merged change from SC1X)
 07/18/08  RK     Modified the critical area in OEMCamera_CameraLayerCB() fixed the CR 151517.
 05/02/08  KC     Fixing a issue that might cause a stack corrution issue.
 04/10/08  KC     OR'ed FEATURE_CAMERA_BURST_MODE with FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
 03/25/08  anil   fixed CAM_PARM_SIZE/DISPLAY_SIZE param code in ICamera_SetParm
 03/06/08   KC     Featurised yCbCr instances with FEATURE_CAMERA_ENCODE_YCBCR for 6K targets
 02/18/08  anil   Verified user entered bitrate is valid or not in OEMCamera_SetParm
 02/11/08  sanal   Adding Idiag support for frames dropped in a recorded clip
 12/06/07  Yogi   Expect another callback to OEMCamera_CameraLayerCB when 
                  VIDEO_LIMIT_REACHED_CHUNK_TABLE /~_SAMPLE_TABLE/~_MOVIE_DURATION
                  errors are returned by the video engine.
 12/05/07  RK     Added fix to OEMCamera_SetPreviewFPS and OEMCamera_SetParmthe when pass the negative parm value. 
 11/14/07  NC     Returned error code to Get/SetParam on invalid ISO value.
 11/12/07  NC     Changed nCurrent to nMin in OEMCamera_ConvertGetFPS for min value.
 11/06/07  NC     Added case CAM_PARM_IS_SUPPORT to item support checking.
 10/01/07  hap    Added changes needed to support YCbCr as a type of encoding.
 09/14/07  KD     Removed deprecated function calls
 09/03/07  Yogi   Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
 08/30/07  KD     Stopping the camera in OEMCamera_Delete before entering
                  critical section.
 08/28/07  YZ     Remove the critical area changes in OEMCamera_Delete() and OEMCamera_CameraLayerCB()
 08/15/07  KD     Updated the OEMCamera_CameraLayerCB logic to fix an issue with
                  multishot and to allow image encoding to be canceled
 07/29/07  yogi   Added support for audio only recording. 
 07/29/07  YZ     Added the critical area in OEMCamera_Delete() and changed it in OEMCamera_CameraLayerCB()
 06/29/07  kadhir Added max and dynamic bitrate support for camcorder recording
 05/31/07  jdas   SC1x changes for camcorder
 05/09/07  jdas   Put code under feature to fix compilation issue
 04/27/07  jdas   Added fix to start preview when space error is reported to UI.
 04/18/07  ZL     Fixed bug in OEMCamera_DoFocus().
 03/07/07  ZL     Updated FEATURE_CAMERA_MULTI_SENSOR support.
 03/06/07  NC     Fixed bug in OEMCamera_CameraLayerCB.
 02/27/07  NC     Updated LCD direct mode feature to fix bugs.
 01/31/07  dhaval Return movie size list according to target support.
 01/16/07  dhaval Return success on audio format settings for recording.
 01/08/07   gm    Remove Compile error for 6k
 12/22/06  pavan  Disabled QVGA recording on 6260 for Camera Midlet
 12/16/06    gm    Fix for Frame CallBack,Add new parameter support for
                  Direct Mode
 12/05/06  NC     Added MDP feature condition to Direct Access mode.
 11/14/06  gm     Added support for Direct Access mode and Frame Call back mode
 11/08/06  dhaval Pass the space_limit value through camera_set_parm
 11/2/06   jdas   Fixed AAC settings problem
 10/30/06  dhaval Added CAM_PARM_SPACE_LIMIT case to set the space limit
                  threshold value in camcorder.
 10/05/06  RR     Added OEM support for Preview Histogram.
 09/15/06  NC     Fixed memory leaks.
 09/14/06  nrw    Fixed abort logic by removing rspCopy.bInUse = FALSE
 09/08/06  RR     Set condition !FEATURE_CAMERA7500 for  m_bAudioPostCardRecord
 09/05/06  NV     Added support for Get/Set Hue, Saturation
 08/29/06  kadhir Removed unwanted Cross Compilation Dependency
                  When qcamcorder Feature Disabled
 08/25/06   NC    Set condition FEATURE_CAMERA7500 for bracketing mode.
 08/14/06   NC    Used FEATURE_CAMERA_HJR for CAMERA_ISO_DEBLUR condition.
 08/04/06   NC    Fixed compiler warnings.
 07/28/06   NC    Added CAMERA_ISO_DEBLUR mapping. HJR is mutual exclusive with ISO settings.
                  CAMERA_PARM_HAND_JITTER_REDUCTION is removed and CAM_PARM_DEBLUR is deprecated.
 07/21/06   wyh   Remove global variables that control camcorder special efx
 07/12/06   NC    Replaced CAM_STATUS_PICTURE with CAM_STATUS_FRAME for multishot record callback.
 07/12/06   NC    Added function OEMCamera_GetSupportStatus and
                  fixed potential stack override issue in GetParm.
 07/10/06   NC    Fixed a bug in get parm for CAM_PARM_IS_SUPPORT and CAM_PARM_IS_MOVIE.
 07/06/06   NC    Added supports for multishot and bestshot mode.
 06/29/06   NC    Set flag to ignor callback in case of release camera.
 06/16/06   NC    Return CAM_PENDING on success DoFocus.
 06/14/06   NC    Added FEATURE_CAMERA_V790 for new feature condition per service define.
 06/09/06   nrw   Added exposure metering support for FEATURE_CAMERA_V2
 06/09/06   NC    Added support for CAM_PARM_FPS_LIST
 06/01/06   dw    Fixed lint warning for video recording with AAC audio
 05/31/06   dw    Changes from BREW 3.1.4 for video recording error recovery
 05/17/06   NC    Implemented GetParm for CAM_PARM_RED_EYE_REDUCTION and CAM_PARM_IS_SUPPORT
 04/06/06   NC    Change CAMERA_PARM_DEBLUR to CAMERA_PARM_HAND_JITTER_REDUCTION
                  per camera update.
 03/02/06   NC    Merge updates from 3.1.4.
 03/01/06   NC    Updates for 3.1.5 release.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_CAMERA)

#include "OEMCameraDMSS.h"
#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "AEE_OEMFile.h"
#include "AEEMediaFormats.h"
#include "AEEPosDet.h"
#include "AEEStdLib.h"

#ifdef FEATURE_DSP
#include "cam_module.h"
#endif
#ifdef FEATURE_ACM
#include "AEE_OEM.h"
#endif

#ifdef FEATURE_BMP_ACM
#include "AEEIACM.h"
#include "AEEACM.bid"
#include "AEEIEnv.h"
#include "AEE_OEM.h"
#include "AEEISignalCBFactory.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEEIResourceStatus.h"
#include "AEEResourceStatus.bid"
#include "AEEIParameters1.h"
#include "AEEPriorityMgr.bid"
#include "msg.h"
#endif // FEATURE_BMP_ACM
#ifdef CUST_EDITION
#include "custcamsensor.h"
#endif
#ifdef FEATURE_CAMERA_NOFULLSCREEN
#include "disp.h"
#endif
#include "Camsensor.h"
#define NUM_OF_SENSORS_ON_PHONE  2
extern camsensor_static_params_type camsensor_static_params[NUM_OF_SENSORS_ON_PHONE];
#ifndef RELEASEIF
#define RELEASEIF(pi)  do { if (pi) { IBASE_Release((IBase*)(pi)); (pi)=0; }} while(0)
#endif

/*===========================================================================
Feature condition - FEATURE_CAMERA_LCD_DIRECT_MODE is under condition of FEATURE_MDP
They are defined in different cust files and condition could not be set there.
Workaround in .c files to make sure no compiling issue happens in the case.
===========================================================================*/
#ifndef FEATURE_MDP
#ifdef  FEATURE_CAMERA_LCD_DIRECT_MODE
#undef FEATURE_CAMERA_LCD_DIRECT_MODE
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
#endif //FEATURE_MDP

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/
#define OEMCamera_ISREADY(pme)        ((pme)->m_nMode == CAM_MODE_READY)
#define OEMCamera_ISPREVIEW(pme)      (((pme)->m_nMode == CAM_MODE_PREVIEW) \
                                       || ((pme)->m_nNextMode == CAM_MODE_PREVIEW))
#define OEMCamera_ISSNAPSHOT(pme)     (((pme)->m_nMode == CAM_MODE_SNAPSHOT) \
                                       || ((pme)->m_nNextMode == CAM_MODE_SNAPSHOT))
#define OEMCamera_ISMOVIE(pme)        (((pme)->m_nMode == CAM_MODE_MOVIE)    \
                                       || ((pme)->m_nNextMode == CAM_MODE_MOVIE))
#define OEMCamera_STATECHANGING(pme)  ((pme)->m_bStateChanging)
#define OEMCamera_ISPAUSED(pme)       ((pme)->m_bPaused)

#define OEMCamera_CAMERAREADY(pme)    (OEMCamera_ISREADY(pme) && !OEMCamera_STATECHANGING(pme))
#define OEMCamera_STARTOK(pme)        OEMCamera_CAMERAREADY(pme)
#define OEMCamera_STOPOK(pme)         (!OEMCamera_ISREADY(pme))

#define OEMCamera_ISMOVINGMODE(pme)   (OEMCamera_ISPREVIEW(pme) || OEMCamera_ISMOVIE(pme))
#define OEMCamera_PAUSEOK(pme)        (OEMCamera_ISMOVINGMODE(pme) && !OEMCamera_ISPAUSED(pme) && !OEMCamera_STATECHANGING(pme))
#define OEMCamera_RESUMEOK(pme)       (OEMCamera_ISMOVINGMODE(pme) && OEMCamera_ISPAUSED(pme) && !OEMCamera_STATECHANGING(pme))
#define OEMCamera_ENCODESNAPSHOTOK(pme) (1)

#define OEMCAMERA_ENTER_CRITICAL_SECTION()          OEMCriticalSection_Enter(&gOEMCameraCriticalSection); {
#define OEMCAMERA_LEAVE_CRITICAL_SECTION()        ;} OEMCriticalSection_Leave(&gOEMCameraCriticalSection);

/*===========================================================================

                      CameraDevice DECLARATIONS

===========================================================================*/
typedef struct CameraDevice   CameraDevice;
#ifdef FEATURE_DSP
uint16 g_fbuffer[A8_MAIN_LCD_WIDTH*A8_MAIN_LCD_HEIGHT] = {0};
#endif

struct CameraDevice
{
   int   nRefs;
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
   AEECLSID OwnerCls;
#endif
};

#ifdef FEATURE_CAMERA_MULTI_SENSOR
static int  CameraDevice_Open(uint16 sensor_id);
static int  CameraDevice_Close(uint16 sensor_id);

static CameraDevice     gCameraDevice[MAX_SUPPORTED_SENSORS] = {0};
static camera_info_type gCameraInfo[MAX_SUPPORTED_SENSORS] = {0};
static uint16 camera_current_sensor = 0xFFFF;
static uint16 camera_num_sensors = 0;
#else
static int  CameraDevice_Open(void);
static int  CameraDevice_Close(void);

static CameraDevice     gCameraDevice = { 0 };
static camera_info_type gCameraInfo;
#endif
#ifdef  FEATURE_CAMERA_LCD_DIRECT_MODE
static boolean bDirectMode;
#endif
#ifdef FEATURE_CAMERA_NOFULLSCREEN
static boolean bDirectToLCD;
#endif

/*===========================================================================

                      ICamera DECLARATIONS

===========================================================================*/
static boolean       OEMCamera_FilePath(OEMCamera * pme, void * pData);
static void          OEMCamera_InitCameraRsp(OEMCamera * pme, int16 nCmd);
static CameraRsp *   OEMCamera_AllocStartCameraRsp(OEMCamera * pme, int16 nSubCmd);
static void          OEMCamera_FreeStartCameraRsp(OEMCamera * pme);
static CameraRsp *   OEMCamera_GetStartCameraRsp(OEMCamera * pme);
static void          OEMCamera_FreeCameraRsp(CameraRsp * pRsp);
static int           OEMCamera_Preview(OEMCamera * pme, CameraRsp * pRsp);
static int           OEMCamera_RecordMovie(OEMCamera * pme, CameraRsp * pRsp);
static int           OEMCamera_RecordSnapshot(OEMCamera * pme, CameraRsp * pRsp);
static boolean       OEMCamera_SaveFrameInfo(OEMCamera * pme, camera_frame_type * pf);
static int           OEMCamera_AddOverlay(OEMCamera * pme, IDIB * pb);
static void          OEMCamera_SetReadyMode(OEMCamera * pme);
static int           OEMCamera_GetSupportStatus(OEMCamera * pme, int16 itemID);
#ifdef FEATURE_VIDEO_ENCODE
static int16         OEMCamera_AudioStreamType(uint32 clsAudio, uint32 dwSubAudio);
#endif //FEATURE_VIDEO_ENCODE
#ifndef FEATURE_CAMERA_V2
static int           OEMCamera_DoFocus(OEMCamera * pme, int32 mode, int32 step);
#ifndef FEATURE_CAMERA7500
static int           OEMCamera_GetFocusRect(int32 * pRc, int32 * pInfo);
static int           OEMCamera_SetFocusRect(AEERect rc);
#endif //ifndef FEATURE_CAMERA7500
#endif //FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA_V7
static void          OEMCamera_ConvertGetEV(OEMCamera * pme,
                                            AEEParmInfo * pi,
                                            AEEFixedPointParm * p1,
                                            AEEFixedPointParmInfo * pInfo);
static int32         OEMCamera_ConvertSetEV(OEMCamera * pme, AEEFixedPointParm * pParm);
static void          OEMCamera_ConvertGetFPS(AEEParmInfo *pi,
                                             AEEFixedPointMinMax * pMinMax);
static int           OEMCamera_SetPreviewFPS(AEEFixedPointMinMax * pMinMax,
                                             CameraRsp * pRsp);
#endif //#ifdef FEATURE_CAMERA_V7

#ifdef FEATURE_CAMERA_V770
static int           OEMCamera_ConverISO(int32 * pVal, boolean bSet);
#endif //#ifdef FEATURE_CAMERA_V770

#define OEMCamera_FreeCmdCameraRsp        OEMCamera_FreeCameraRsp
#define OEMCamera_UngetCameraRsp(pRsp)    {(pRsp)->bInUse = FALSE;}

// CameraLayer related...

#if defined(FEATURE_VIDEO_ENCODE)
static int  OEMCamera_SetMovieDestination(OEMCamera * pme, video_handle_type * ph);
#endif // defined(FEATURE_VIDEO_ENCODE)

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
// ACM Limitations - raghub - 02/23/07
// The current design of OEMCamera restricts the number of ICAMERA instances
// in the system to 1. Also, there is no arbitration based on priority for
// Camera instances i.e. if one application has successfully created an ICAMERA
// instance, even if a higher priority application tries to create another
// instance later, it shall fail.
// The notion of priority
// is only applicable to non-Camera DSP using apps and the Camera. So, if the
// Camera is held by a lower priority app and a higher priority app wants to
// create an MP4 IMEDIA object, the ICAMERA instance is force released.

    static void pcbCameraACM(void *p);
    static int OEMCamera_ACMTransaction(OEMCamera *pme,int nReasonCode);
#endif // FEATURE_ACM || FEATURE_BMP_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

static OEMCriticalSection gOEMCameraCriticalSection;

#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
void ICamera_SetSnapshotProperties(OEMCamera * pme, camera_encode_properties_type * p);
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

//This will be removed if defined in the camera.h
#ifndef CAMERA_FPS_DENOMINATOR
#define CAMERA_FPS_DENOMINATOR 1000
#endif //CAMERA_FPS_DENOMINATOR

// This is a temporary solution for UI to get a list of video resolution
// Will remove the following array once we get the sizeList support from lower
// camera services layer
static AEESize gCameraMovieSizeList[] = {
#ifndef FEATURE_VIDEO_ENCODE_MULTIMEDIA_TIER /* This feature is enabled for only 6260*/
  { 240,  320 },  // QCAM_SIZE_QVGA
#endif
  { 144,  176 },  // QCAM_SIZE_QCIF
  { 96,  128 },  // QCAM_SIZE_SQCIF
  { 80, 96 }, // QCAM_SIZE_MOVIEMAIL
  { 0, 0 }    // NULL terminator
};

static AEESize gCameraDisplaySizeList[] = {
#if defined(FEATURE_DISP_160X128)
  { 160, 128 },
#elif defined(FEATURE_DISP_220X176)
  { 220, 176 },  
#elif defined(FEATURE_DISP_128X160)
  { 128, 160 },
#elif defined(FEATURE_DISP_176X220)
  { 176, 220 },
#elif defined(FEATURE_DISP_240X320)
  { 240, 320 }, 
#elif defined(FEATURE_DISP_320X240)
  { 320, 240 },   
#elif defined(FEATURE_DISP_128X128)
  { 128, 128 },
  { 128, 96  },
#else
  { 240, 320 },  // QCAM_SIZE_QVGA 
  { 144, 176 },  // QCAM_SIZE_QCIF 
  { 96,  128 },  // QCAM_SIZE_SQCIF 
  { 80,  96  }, // QCAM_SIZE_MOVIEMAIL 
#endif
  { 0, 0 }    // NULL terminator 
}; 

static AEESize gCameraSnapshotSizeList [] = { 
#if defined(USE_CAMSENSOR_SIV121A)
#if defined(FEATURE_DISP_160X128)
  { 160, 128 },
#elif defined(FEATURE_DISP_220X176)
  { 220, 176 },  
#elif defined(FEATURE_DISP_128X160)
  { 128, 160 },  
#elif defined(FEATURE_DISP_176X220)
  { 176, 220 }, 
#elif defined(FEATURE_DISP_240X320)
  { 240, 320 },   
#elif defined(FEATURE_DISP_320X240)
  { 320, 240 },  
#elif defined(FEATURE_DISP_128X128)
  { 128, 128 },
  { 160, 120 },
#endif
#if defined(FEATURE_DISP_128X160)
  { 176 , 220},
  { 240 , 320},
#else
  { 220 , 176},
  { 320 , 240},
#endif
  { 640 , 480},
#elif defined(USE_CAMSENSOR_SIV120)
#if defined(FEATURE_DISP_160X128)
  { 160, 128 },
#elif defined(FEATURE_DISP_220X176)
  { 220, 176 },  
#elif defined(FEATURE_DISP_128X160)
  { 128, 160 },
#elif defined(FEATURE_DISP_176X220)
  { 176, 220 }, 
#elif defined(FEATURE_DISP_240X320)
  { 240, 320 },  
#elif defined(FEATURE_DISP_320X240)
  { 320, 240 },  
#elif defined(FEATURE_DISP_128X128)
  { 128, 128 },
  { 160, 120 },
#endif
  { 220 , 176},
  { 320 , 240},
#else
  {1200, 1600},
  { 1024 , 1280},
  {  960 ,1280},
  { 768 , 1024},
  {  600 , 800},
  { 480 , 640},
  {  288 , 352},
  {  240 , 320},
  {  144 , 176},
  { 120 , 160 },
  {  48 , 80},
#endif
  { 0, 0 } // NULL terminator 
};
  
static AEECLSID gCameraEncodeVideoList[] = { 
  AEECLSID_JPEG, 
  AEECLSID_PNG,
  AEECLSID_MEDIAMPEG4
}; 
  
static AEECLSID gCameraEncodeAudioList [] = { 
   AEECLSID_MEDIAQCP 
#ifdef FEATURE_AAC_REC 
 ,
 AEECLSID_MEDIAAAC 
#endif /* FEATURE_AAC_REC */ 
}; 

/*===============================================================================

                     CameraDevice Function Definitions

===============================================================================*/
#ifdef FEATURE_CAMERA_MULTI_SENSOR
static int CameraDevice_Open(uint16 sensor_id)
{
  if (gCameraDevice[sensor_id].nRefs > 0)
  {
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    // With ACM, multiple instances created by different apps are not allowed
    if (AEE_GetAppContextCls(AEE_GetAppContext()) != gCameraDevice[sensor_id].OwnerCls)
      return EFAILED;
#endif     
    ++(gCameraDevice[sensor_id].nRefs);
  }
  else
  {
    if (camera_select_sensor(sensor_id))
      return EFAILED;

    // Initialize the phrase device
    if (camera_start(NULL, NULL))
      return EFAILED;

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    gCameraDevice[sensor_id].OwnerCls = AEE_GetAppContextCls(AEE_GetAppContext());
#endif     
    gCameraDevice[sensor_id].nRefs = 1;
    camera_get_info(&gCameraInfo[sensor_id]);
  }

  return SUCCESS;
}

static int CameraDevice_Close(uint16 sensor_id)
{
  if (gCameraDevice[sensor_id].nRefs <= 0)
    return EFAILED;

  if (--(gCameraDevice[sensor_id].nRefs) == 0)
  {
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    gCameraDevice[sensor_id].OwnerCls = 0;
#endif     
    camera_stop(NULL, NULL);
  }

  return SUCCESS;
}
#else
static int CameraDevice_Open(void)
{
  if (gCameraDevice.nRefs > 0)
  {
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    // With ACM, multiple instances created by different apps are not allowed
    if (AEE_GetAppContextCls(AEE_GetAppContext()) != gCameraDevice.OwnerCls)
      return EFAILED;
#endif     
    ++gCameraDevice.nRefs;
  }
  else
  {
    // Initialize the phrase device
    if (camera_start(NULL, NULL))
      return EFAILED;

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    gCameraDevice.OwnerCls = AEE_GetAppContextCls(AEE_GetAppContext());
#endif     
    gCameraDevice.nRefs = 1;
    camera_get_info(&gCameraInfo);
  }

  return SUCCESS;
}

static int CameraDevice_Close(void)
{
  if (gCameraDevice.nRefs <= 0)
    return EFAILED;

  if (--gCameraDevice.nRefs == 0)
  {
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    gCameraDevice.OwnerCls = 0;
#endif     
    camera_stop(NULL, NULL);
  }

  return SUCCESS;
}
#endif //FEATURE_CAMERA_MULTI_SENSOR

/*===============================================================================

                     ICamera Function Definitions

===============================================================================*/
/*==================================================================

==================================================================*/
void OEMCamera_Init(void)
{
	#ifdef FEATURE_DSP
	AIT701_system_init();
	#endif
  // Empty
}

/****************/
#if defined (FEATURE_BMP_ACM) 
#error code not present
#endif // FEATURE_BMP_ACM

#if defined (FEATURE_ACM)

static void pcbCameraACM(void *p)
{
   ACM_Callback_data_type CBdata=*((ACM_Callback_data_type*)p);
   int nCamStopResult = 0;
   int retVal = 0;
   ACM_resource_state_type state= CBdata.state;
   OEMCamera *pme =  (OEMCamera *)CBdata.pdata;
   MSG_FATAL("pcbCameraACM............................",0,0,0);
   //callback to release the resource
   if(pme->m_enResStateCamera == RES_STATE_ACQUIRED)
   {
      DBGPRINTF_LOW("callback to release the resource");
      if(state != ACM_RESOURCE_ACQUIRED)
      {
         DBGPRINTF_LOW("Asking us to force release ... now wait for callback for stop to occur");
         pme->m_boCameraStoppedDueToACMRelease = TRUE;
         // Asking us to force release
         nCamStopResult = OEMCamera_Stop((OEMINSTANCE)pme);
         // now wait for callback for stop to occur

         if(nCamStopResult == EBADSTATE)
         { /// camera is in ready state, not yet started preview
            camera_stop(NULL, NULL); // force shutdown of CAMERA
            MSG_HIGH("Camera is not in a good state to stop , but we are force shutting down due to ACM request",0,0,0);
            // ASH: Camera may be taking picture , the service layer doesn't allow aborting picture_taking
            // So we are force shutting down camera.
            {
               CameraRsp *pRsp;
               pRsp = OEMCamera_AllocCmdCameraRsp( pme, CAM_CMD_ENCODESNAPSHOT, 0 );
               OEMCamera_CameraLayerCB( CAMERA_EXIT_CB_FAILED, (void*)pRsp->hObject,
                                        CAMERA_FUNC_TAKE_PICTURE, 0 );
               OEMCamera_FreeCameraRsp(pRsp);
            }
#if defined (FEATURE_ODM_UI)
            IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
#else // FEATURE_ODM_UI
            OEMACM_PriorityMgr_Free (pme);
#endif // FEATURE_ODM_UI
            pme->m_enResStateCamera = RES_STATE_NORES;
         }
         else if(nCamStopResult == CAMERA_SUCCESS)
         {
             //  wait in the callback to stop
         }
         else
         {
#if defined (FEATURE_ODM_UI)
            IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
#else // FEATURE_ODM_UI
            OEMACM_PriorityMgr_Free (pme);
#endif // FEATURE_ODM_UI
            pme->m_enResStateCamera = RES_STATE_NORES;
            MSG_FATAL("Unable to stop the camera during request by ACM to stop", 0,0,0);
         }
      }
   }
   //callback when resource is ready
   if(state == ACM_RESOURCE_ACQUIRED)
   {
     MSG_FATAL("callback when resource is ready in case of delayed acquire",0,0,0) ;
     pme->m_enResStateCamera = RES_STATE_ACQUIRED;
     pme->m_boCameraStoppedDueToACMRelease = FALSE;

     // We are here because of an ACM transaction. So, we will be executing
     // a delayed start. Make sure that the structure is valid.
     if (pme->m_cameraCallStartStruct.isValid)
     {
       pme->m_cameraCallStartStruct.isValid = FALSE;
       if (! pme->m_cameraCallStartStruct.encodingSnapshot)
       {
       	 MSG_FATAL("callback when resource is ready in case of delayed acquire start",0,0,0) ;
         retVal = OEMCamera_Start( (OEMINSTANCE)pme,
                                  pme->m_cameraCallStartStruct.mode,
                                  pme->m_cameraCallStartStruct.dwParam);
      }
      else
      {
         pme->m_cameraCallStartStruct.encodingSnapshot = FALSE;
         retVal = OEMCamera_EncodeSnapshot((OEMINSTANCE)pme);
       }
     }
   }
}


static int OEMCamera_ACMTransaction(OEMCamera *pme,int nReasonCode)
{
#if defined (FEATURE_ODM_UI)
   uint32   u32appres_status = 0;
#endif // FEATURE_ODM_UI
   int      nRet = SUCCESS;

   ACM_resource_state_type stAcmResourceStateType = ACM_RESOURCE_UNKNOWN;
   if(!pme)
   {
      return EBADPARM;
   }

   // See if the ACM is acuired else return Acquiring
   if( (pme->m_enResStateCamera == RES_STATE_ACQUIRED) ||
       (pme->m_enResStateCamera == RES_STATE_ACQUIRING) )
   {
      // if not acquired for the requested reason code release the reesource
      // so that we do reacquire for the new reason code
      // else we just return out.
      if(pme->m_nResAquired_For_Reason != nReasonCode)
      {
#if defined (FEATURE_ODM_UI)
            IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
#else // FEATURE_ODM_UI
            OEMACM_PriorityMgr_Free (pme);
#endif // FEATURE_ODM_UI
         pme->m_nResAquired_For_Reason = nReasonCode;
         DBGPRINTF_LOW("Released the camera resource as the reason is different now" );
      }
      else
      {
         //resourse is already acquired for the same reason
         DBGPRINTF_LOW("resource already acquir(ed)/(ing) for the same reason" ) ;
         return  nRet;
      }
   }

   pme->m_enResStateCamera = RES_STATE_NORES;

   // Acquiring the resource permission from ACM - doing it only if Resource not acquired or in acquiring state
#ifdef FEATURE_ODM_UI
   nRet = IAPPRESCTL_Acquire(pme->m_pAppResCtl,nReasonCode,&u32appres_status);
   if(nRet != SUCCESS)
   {
      //CAM_FreeACMResCTL(pme->m_pAppResCtl);
      IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
      return nRet;
    }

   if(u32appres_status != RESCTL_STATUS_ACQUIRED)
   {
      //Permission to use Camera resource not granted by ACM
      nRet = AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      DBGPRINTF_MED("Permission to use Camera resource not granted by ACM");
      IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
      return nRet;
   }
   // Permission granted it could be a delay acquire also
   // Assign the callback pointer to be called when ready
   pme->m_stCallBackDataType.pdata = (void*) pme;
   pme->m_stCallBackDataType.state = ACM_RESOURCE_UNKNOWN;

   nRet = OEMACM_StateMgr_OnStatusChangeNotify(pme->m_pAppResCtl,pcbCameraACM, &pme->m_stCallBackDataType);
   if(nRet != SUCCESS)
   {
      nRet = AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      //CAM_FreeACMResCTL(pme->m_pAppResCtl);
      IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
      return nRet;
   }

   nRet = OEMACM_StateMgr_GetResourceState(pme->m_pAppResCtl,&stAcmResourceStateType);
   if(nRet != SUCCESS)
   {
      nRet = AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      //CAM_FreeACMResCTL(pme->m_pAppResCtl);
      IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
      return nRet;
   }
   if(stAcmResourceStateType != ACM_RESOURCE_ACQUIRED)
   {
      nRet = SUCCESS;
      DBGPRINTF_LOW("Camera resource is delayed acquired.. so we are in acquiring state - wait for callback from ACM");
      pme->m_enResStateCamera = RES_STATE_ACQUIRING;
      return nRet;
   }
#else // FEATURE_ODM_UI
   nRet = OEMACM_PriorityMgr_Acquire (pme, nReasonCode, &stAcmResourceStateType);
   if(nRet != SUCCESS)
   {
      return nRet;
   }

   if(stAcmResourceStateType == ACM_RESOURCE_BUSY )
   {
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
   else
   {
      // ACM_RESOURCE_ACQUIRED or ACM_RESOURCE_ACQUIRING
      // Permission granted it could be a delay acquire also
      // Assign the callback pointer to be called when ready
      pme->m_stCallBackDataType.pdata = (void*) pme;
      pme->m_stCallBackDataType.state = ACM_RESOURCE_UNKNOWN;

      if(OEMACM_StateMgr_OnStatusChangeNotify(pme,
                                              pcbCameraACM,
                                              &pme->m_stCallBackDataType) != SUCCESS)
      {
         OEMACM_PriorityMgr_Free (pme);
         return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      }

      if(stAcmResourceStateType == ACM_RESOURCE_ACQUIRING)
      {
         nRet = SUCCESS;
         DBGPRINTF_LOW("Camera resource is delayed acquired.. so we are in acquiring state - wait for callback from ACM");
         pme->m_enResStateCamera = RES_STATE_ACQUIRING;
         return nRet;
      }
   }
#endif // FEATURE_ODM_UI
   pme->m_enResStateCamera = RES_STATE_ACQUIRED;
   pme->m_nResAquired_For_Reason = nReasonCode;
   pme->m_boCameraStoppedDueToACMRelease= FALSE;
   DBGPRINTF_LOW("Camera Resource is acquired without delay - no one else is using camera");
   return nRet;
}
#endif // FEATURE_ACM
/*==================================================================

==================================================================*/
int OEMCamera_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph)
{
#ifdef FEATURE_DSP
	OEMCamera *pme = NULL;
	int        nRet = SUCCESS;
    IShell*  papplet = NULL;



	if (!ph)
		return EBADPARM;

	// Alloc memory for the object
	pme = (OEMCamera *)MALLOC(sizeof(OEMCamera));
	if (!pme)
	{
		nRet = ENOMEMORY;
		goto Done;
	}

	pme->m_nMode = CAM_MODE_READY;

	// By default, file size is disabled by setting param to zero
	pme->m_nFileSize = OEMCAMERA_FILE_SIZE_DISABLED;
	// Assume we will be using Snapshot mode
	
    pme->m_pFileMgr = NULL;
    
    papplet = (IShell*)AEE_GetShell();

    nRet = ISHELL_CreateInstance(papplet, AEECLSID_FILEMGR, (void **)&pme->m_pFileMgr);

    
	if (nRet != SUCCESS || pme->m_pFileMgr == NULL) 
	{
		MSG_FATAL("AEECLSID_FILEMGR Create Failed!",0,0,0);
		goto Done;
	}
					    

	*ph = (OEMINSTANCE)pme;
		return nRet;

Done:
	if (ph)
		*ph = NULL;
	FREE(pme);
	return nRet;

#else
  OEMCamera *pme = NULL;
  int        nRet = SUCCESS;
#ifdef FEATURE_CAMERA_MULTI_SENSOR
  camera_info_type *sensorInfo = NULL;
#endif // FEATURE_CAMERA_MULTI_SENSOR

  if (!ph)
    return EBADPARM;

  // Alloc memory for the object
  pme = (OEMCamera *)MALLOC(sizeof(OEMCamera));

  if (!pme)
  {
    nRet = ENOMEMORY;
    goto Done;
  }

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  camera_num_sensors = camera_get_sensors(&sensorInfo);
  MSG_FATAL("camera_num_sensors=%d",camera_num_sensors,0,0);
  if (camera_num_sensors == 0)
  {
    //nRet = EFAILED;
    //goto Done;
  }
  MEMCPY(gCameraInfo, sensorInfo, sizeof(gCameraInfo));

  switch (cls)
  {
    case AEECLSID_CAMERA1: // the primary sensor
      pme->m_sensorId = 0;
      if ((camera_num_sensors == 1) &&
          (gCameraInfo[pme->m_sensorId].sensor_available == FALSE))
      {
        // The primary sensor failed somehow, select the second sensor.
        pme->m_sensorId = 1;
      }
      break;

    case AEECLSID_CAMERA2:
      pme->m_sensorId = 1;
      if ((camera_num_sensors == 1) &&
          (gCameraInfo[pme->m_sensorId].sensor_available == FALSE))
      {
        // The second sensor failed somehow, select the primary sensor.
        pme->m_sensorId = 0;
      }
      break;

    case AEECLSID_CAMERA3:
    case AEECLSID_CAMERA4:
      nRet = ECLASSNOTSUPPORT;
      goto Done;
  } // end of switch

  // Open the camera device, if necessary
  if (CameraDevice_Open(pme->m_sensorId))
  {
    nRet = ENOTALLOWED;
    goto Done;
  }
  camera_current_sensor = pme->m_sensorId;
#else // FEATURE_CAMERA_MULTI_SENSOR
  // Open the camera device, if necessary
  if (CameraDevice_Open())
  {
    nRet = ENOTALLOWED;
    goto Done;
  }
#endif // FEATURE_CAMERA_MULTI_SENSOR

  pme->m_bRelease = FALSE;
  pme->m_nMode = CAM_MODE_READY;
  pme->m_nNextMode = CAM_MODE_READY;
#ifdef FEATURE_JPEG_ENCODER
  pme->m_clsVideo = AEECLSID_JPEG;
#else
  pme->m_clsVideo = AEECLSID_PNG;
#endif
  pme->m_pFrame = (FrameBitmap *)pFrame;

  /* Set Frame size default to 0. _set_dimension() is not called
   * until a non-zero value is set. */
  pme->m_sizeFrame.cx = 0;
  pme->m_sizeFrame.cy = 0;
  /* Set display size default to 160*120. A zero display size may
   * cause divide by zero exception. */
  pme->m_sizeDisplay.cx = 160;
  pme->m_sizeDisplay.cy = 120;
  // By default, file size is disabled by setting param to zero
  pme->m_nFileSize = OEMCAMERA_FILE_SIZE_DISABLED;
#ifdef FEATURE_CAMERA_MULTI_SENSOR
  pme->m_nFPS = (byte)gCameraInfo[pme->m_sensorId].fps;
#else
  pme->m_nFPS = (byte)gCameraInfo.fps;
#endif // FEATURE_CAMERA_MULTI_SENSOR
  // Assume we will be using Snapshot mode
  pme->m_nQuality = OEMCAMERA_QUALITY_HIGH;
  pme->m_nPreviewType = CAM_PREVIEW_SNAPSHOT;

#ifdef FEATURE_CAMERA_V7
  {
    camera_parm_info_type pi;
    if (camera_get_parm(CAMERA_PARM_EXPOSURE_COMPENSATION, &pi) == SUCCESS)
    {
      //This value is not passed up to UI. It is saved
      //in OEM and used to reconstruct the parameter set to camera.
      //Get the value in start up in case UI calls SetParm
      //before GetParm.
      pme->m_nEVFactor = (uint16)(pi.step_value & 0xFFFF);
    }
  }
#endif //ifdef FEATURE_CAMERA_V7

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)  
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  /* Default to Frame CallBack whenever you enter the Camera App */
  bDirectMode = FALSE; 
#endif
#ifdef FEATURE_CAMERA_NOFULLSCREEN
  bDirectToLCD = FALSE;
#endif
  OEMCamera_InitCameraRsp(pme, CAM_CMD_START);
  OEMCamera_InitCameraRsp(pme, CAM_CMD_SETPARM);
  OEMCamera_InitCameraRsp(pme, CAM_CMD_ENCODESNAPSHOT);

  #if defined(FEATURE_BREWAPI_SPECIFIC)
  // PNG
  OEMCamera_InitCameraRsp(pme, CAM_CMD_OVERLAY_IMAGE);
  #endif //defined(FEATURE_BREWAPI_SPECIFIC)

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
#if defined (FEATURE_ODM_UI)
  pShellCam = AEE_GetShell();
  pme->m_boACMCreated = FALSE;
  pme->m_pAppResCtl = NULL;
  nRet = ISHELL_CreateInstance(pShellCam,AEECLSID_APPRESCTL, (void**)&pme->m_pAppResCtl);
  if(nRet != SUCCESS)
  {
    return nRet;
  }
  if(!pme->m_pAppResCtl)
  {
    return ENOMEMORY;
  }
  pme->m_boACMCreated = TRUE;
#endif // FEATURE_ODM_UI
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif
  pme->m_nResAquired_For_Reason = ACM_REASON_CODES_MIN;
  MEMSET((void *)&(pme->m_cameraCallStartStruct), 0,
         sizeof(CameraCallStartStruct));
#endif // FEATURE_ACM || FEATURE_BMP_ACM
 
  *ph = (OEMINSTANCE)pme;
  return nRet;

Done:
  if (ph)
    *ph = NULL;
  FREE(pme);
  return nRet;
  #endif
}

/*==================================================================

==================================================================*/
void OEMCamera_Delete(OEMINSTANCE h)
{
#ifdef FEATURE_DSP
	OEMCamera * pme = (OEMCamera *)h;	   
	 if (!pme )
	   return;

	 RELEASEIF(pme->m_pFileMgr);
	 FREE(pme);

#else
  int         i;
  OEMCamera * pme = (OEMCamera *)h;

  if (!pme )
    return;

  //Turn on the flag so callback from camera will be ignored.
  pme->m_bRelease = TRUE;
  //if it is taking picture or encoding, abort it first.
  if (OEMCamera_ISSNAPSHOT(pme))
    (void)camera_release_picture();

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  CameraDevice_Close(pme->m_sensorId);
#else
  CameraDevice_Close();
#endif

  OEMCAMERA_ENTER_CRITICAL_SECTION()

  /* Unregister the handler */
  AEEObjectMgr_Unregister(pme->m_hObject);

  // Delete all the Rsps...
  OEMCamera_FreeStartCameraRsp(pme);
  for (i = 0; i < OEMCAMERA_RSP_COMMAND_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_rspCommand[i]);

  for (i = 0; i < OEMCAMERA_RSP_ENCODESNAPSHOT_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_rspEncodeSnapshot[i]);

#if defined(FEATURE_BREWAPI_SPECIFIC)
  for (i = 0; i < OEMCAMERA_RSP_COMMAND_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_pngCommand[i]);
#endif // defined(FEATURE_BREWAPI_SPECIFIC)

#ifdef FEATURE_VIDEO_MULTI_UUID
  for (i=0; i<pme->m_uuidInfo.nUuid; i++)
  {
    FREEIF(pme->m_uuidInfo.pAtom[i]);
    FREEIF(pme->m_uuidInfo.pData[i]);
  }
#else /* FEATURE_VIDEO_MULTI_UUID */
  FREEIF(pme->m_pUuid_cpgd);
  FREEIF(pme->m_pCpgd);
#endif /* FEATURE_VIDEO_MULTI_UUID */

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
  if(pme->m_boACMCreated && pme->m_pAppResCtl)
  {
    IAPPRESCTL_Free(pme->m_pAppResCtl,NULL);
    pme->m_enResStateCamera = RES_STATE_NORES;
    IAPPRESCTL_Release(pme->m_pAppResCtl);
    pme->m_pAppResCtl = NULL;
    pme->m_enResStateCamera = RES_STATE_UNKNOWN;
    pme->m_boACMCreated = FALSE;
  }
#else // FEATURE_ODM_UI
  OEMACM_PriorityMgr_Free (pme);

  pme->m_enResStateCamera = RES_STATE_UNKNOWN;
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
  FREEIF(pme->m_pszMIME);
  FREEIF(pme->m_pszFile);
  OEMCAMERA_LEAVE_CRITICAL_SECTION()
  FREE(pme);
  #endif
}

/*==================================================================

==================================================================*/
int OEMCamera_QueryInterface(OEMINSTANCE h, AEECLSID idReq, void ** ppo)
{
   // OEMs can extend ICAMERA_QueryInterface() to return
   // related objects like IResourceCtl associated with ICamera object.
   #ifdef FEATURE_DSP
   return ECLASSNOTSUPPORT;
   #else
#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
   OEMCamera *pme = (OEMCamera *)h;

   if (idReq == AEECLSID_APPRESCTL)
   {
      if(pme->m_boACMCreated && pme->m_pAppResCtl)
      {
         IBASE_AddRef ((IBase*)pme->m_pAppResCtl);
         *ppo = (IBase*)pme->m_pAppResCtl;
         return AEE_SUCCESS;
      }
      else
      {
         *ppo = NULL;
      }
   }
#endif // FEATURE_ODM_UI
#else //FEATURE_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif //FEATURE_BMP_ACM
#endif //FEATURE_ACM
   return ECLASSNOTSUPPORT;
   #endif
}

/*==================================================================

==================================================================*/
int OEMCamera_RegisterNotify(OEMINSTANCE h, PFNCAMERANOTIFY pfnNotify, void * pUser)
{
   OEMCamera * pme = (OEMCamera *)h;

   pme->m_pfnNotify = pfnNotify;
   pme->m_pUser = pUser;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int OEMCamera_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2)
{
	
#ifdef FEATURE_DSP
	int               nRet = SUCCESS;
	OEMCamera *       pme = (OEMCamera *)h;

	switch (nParmID)
	{
		case CAM_PARM_QUALITY:
		// Quality is treated differenty for snapshot VS movie modes
		if (pme->m_nPreviewType == CAM_PREVIEW_SNAPSHOT)
		{
			// Valid range for snapshot quality param is 1 to 100
			// equal the quality percent (for snapshot camera only).
			if ((p1 >= OEMCAMERA_QUALITY_MIN_VALUE) && (p1 <= OEMCAMERA_QUALITY_MAX_VALUE))
				pme->m_nQuality = (int16)p1;
			else
				nRet = EBADPARM;
		}
		else if (pme->m_nPreviewType == CAM_PREVIEW_MOVIE)
		{
			if ((p1 >= OEMCAMCORDER_QUALITY_MIN_VALUE) && (p1 <= OEMCAMCORDER_QUALITY_MAX_VALUE))
			{
				pme->m_nQuality = (int16)p1;
			}
			else
			{
				nRet = EBADPARM;
			}
		}
		break;
        case CAM_PARM_MEDIA_DATA:
             {
                AEEMediaData* pmd = (AEEMediaData*)p1;
                pme->m_pszFile = pmd->pData;
             }            
            break;

		case CAM_PARM_FLASH_CTL:
			if (p1)
			{
			}
			else
			nRet = EBADPARM;
			break;

		case CAM_PARM_NIGHTSHOT_MODE:
		    break;

		case CAM_PARM_EFFECT:
		
			if (p1)
			{
			}
			else
			{
				nRet = EBADPARM;
			}

			break;

		case CAM_PARM_FLASH:
			break;

		case CAM_PARM_FILE_SIZE:
			// File size will take precidence over Quality if it is non zero
			// Units are Kilobytes for this param
			if ((p1 >= OEMCAMERA_FILE_SIZE_MIN_VALUE) && (p1 <= OEMCAMERA_FILE_SIZE_MAX_VALUE))
			{
				//pme->m_nFileSize = (int32)p1;
				nRet = EUNSUPPORTED;
			}
			else
			{
				nRet = EBADPARM;
			}
			break;
		case CAM_PARM_WB:
			if(p1)
			{
				eParm = CAMERA_PARM_WB;
			}
			else
			{
				nRet = EBADPARM;
			}
			break;

		case CAM_PARM_IS_SUPPORT:  // Fall through...
		case CAM_PARM_IS_MOVIE: // This cannot be set by user
		case CAM_PARM_PIXEL_COUNT:
		case CAM_PARM_VIDEO_ENCODE_LIST: // This cannot be set by user
		case CAM_PARM_AUDIO_ENCODE_LIST: // This cannot be set by user
		case CAM_PARM_SIZE_LIST: // This cannot be set by user
		case CAM_PARM_FPS_LIST: // This cannot be set by user
		default:
			nRet = EUNSUPPORTED;
		break;
	}

	return nRet;

#else
  int               nRet = SUCCESS;
  camera_parm_type  eParm = CAMERA_PARM_MAX;
  CameraData        *pCData = NULL;
  OEMCamera *       pme = (OEMCamera *)h;
  MSG_FATAL("camera_set_parm........nParmID=%d",nParmID,0,0);
  switch (nParmID)
  {
    case CAM_PARM_MEDIA_DATA:
      if (p1)
      {
        MEMCPY(&pme->m_md, (void *)p1, sizeof(AEEMediaData));
        if (pme->m_md.clsData == MMD_FILE_NAME)
        {
          if (!OEMCamera_FilePath(pme, pme->m_md.pData))
            return EBADPARM;
        }
        else if (pme->m_md.clsData == MMD_BUFFER)
        {
          if (!pme->m_md.pData || !pme->m_md.dwSize)
            return EBADPARM;
        }
        else
          return EUNSUPPORTED;
      }
      if (p2)
      {
        char *   psz = (char *)p2;

        FREEIF(pme->m_pszMIME);
        pme->m_pszMIME = STRDUP(psz);
      }
      break;

      case CAM_PARM_VIDEO_ENCODE:
        switch(p1)
        {
          case CAM_ENCODE_SHORT_HEADER:
            if (p2)
            {
              pCData = (CameraData *) p2;
              MEMCPY(&(pme->m_wShortHeader), pCData->pData, pCData->dwBytes);
            }
            break;

          case CAM_ENCODE_UUID:
#ifdef FEATURE_VIDEO_MULTI_UUID
          {
            int i;
            for (i=0; i<pme->m_uuidInfo.nUuid; i++)
            {
              FREEIF(pme->m_uuidInfo.pAtom[i]);
              FREEIF(pme->m_uuidInfo.pData[i]);
            }
            pme->m_uuidInfo.nUuid = 1;
            pCData = (CameraData *) p2;
            pme->m_uuidInfo.pAtom[0] = MALLOC(VIDEO_UUID_ATOM_SIZE);
            MEMCPY(pme->m_uuidInfo.pAtom[0], pCData->pData, VIDEO_UUID_ATOM_SIZE);
          }
#else /* FEATURE_VIDEO_MULTI_UUID */
          FREEIF(pme->m_pUuid_cpgd);
          if(p2){
            pCData = (CameraData *) p2;
            pme->m_pUuid_cpgd = MALLOC(pCData->dwBytes);
            MEMCPY(pme->m_pUuid_cpgd, pCData->pData, pCData->dwBytes);
          }
#endif /* FEATURE_VIDEO_MULTI_UUID */
          break;

         case CAM_ENCODE_UUID_DATA:  // single set of uuid scenario
#ifdef FEATURE_VIDEO_MULTI_UUID
           pCData = (CameraData *) p2;
           pme->m_uuidInfo.pData[0] = MALLOC(pCData->dwBytes);
           if (pme->m_uuidInfo.pData[0] != NULL)
           {
             MEMCPY(pme->m_uuidInfo.pData[0], pCData->pData, pCData->dwBytes);
           }
           pme->m_uuidInfo.dataSize[0] = (uint32)pCData->dwBytes;
#else /* FEATURE_VIDEO_MULTI_UUID */
           FREEIF(pme->m_pCpgd);
           if(p2){
             pCData = (CameraData *) p2;
             pme->m_pCpgd = MALLOC(pCData->dwBytes);
             MEMCPY(pme->m_pCpgd, pCData->pData, pCData->dwBytes);
             pme->m_nszpCpgd = (uint32)  pCData->dwBytes;
           }
           else
           {
             pme->m_nszpCpgd =0;
           }
#endif /* FEATURE_VIDEO_MULTI_UUID */
           break;

#ifdef FEATURE_VIDEO_MULTI_UUID
         case CAM_ENCODE_UUID_LIST_ATOM_INFO:
           if (p2 == NULL)
           {
             int i;
             for (i=0; i<pme->m_uuidInfo.nUuid; i++)
             {
               FREEIF(pme->m_uuidInfo.pAtom[i]);
               FREEIF(pme->m_uuidInfo.pData[i]);
             }
             pme->m_uuidInfo.nUuid = 0;
           }
           else
           {
             CameraEncodeUUIDAtomInfo *pCInfo = (CameraEncodeUUIDAtomInfo *) p2;
             pme->m_uuidInfo.nUuid++;
             pme->m_uuidInfo.pAtom[pme->m_uuidInfo.nUuid-1] = MALLOC(VIDEO_UUID_ATOM_SIZE);
             if (pme->m_uuidInfo.pAtom[pme->m_uuidInfo.nUuid-1] != NULL)
             {
               MEMCPY(pme->m_uuidInfo.pAtom[pme->m_uuidInfo.nUuid-1], pCInfo->pAtom, VIDEO_UUID_ATOM_SIZE);
             }
             pme->m_uuidInfo.pData[pme->m_uuidInfo.nUuid-1] = MALLOC(pCInfo->dwDataSize);
             if (pme->m_uuidInfo.pData[pme->m_uuidInfo.nUuid-1] != NULL)
             {
               MEMCPY(pme->m_uuidInfo.pData[pme->m_uuidInfo.nUuid-1], pCInfo->pData, pCInfo->dwDataSize);
             }
             pme->m_uuidInfo.dataSize[pme->m_uuidInfo.nUuid-1] = pCInfo->dwDataSize;
           }
           break;
#endif /* FEATURE_VIDEO_MULTI_UUID */

         default:
          pme->m_clsVideo = (AEECLSID)p1;
          pme->m_dwSubVideo = (uint32)p2;
          break;
         }
         break;
#ifdef FEATURE_VIDEO_ENCODE
      case CAM_PARM_AUDIO_ENCODE:
         pme->m_clsAudio = (AEECLSID)p1;
         pme->m_dwSubAudio = (uint32)p2;
         eParm = CAMERA_PARM_AUDIO_FMT;
         p1 = (int32)OEMCamera_AudioStreamType(pme->m_clsAudio, pme->m_dwSubAudio);
         if (-1 != p1)
            eParm = CAMERA_PARM_AUDIO_FMT;

#ifdef FEATURE_CAMCORDER_AAC_REC
         if(p1 == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC && p2 != NULL)
         {
           CameraAACEncodingInfo info = {0};
           info.uMode       = ((uint32)p2 >> 24) & 0xFF;
           info.dwFrequency = ((uint32)p2 >> 16) & 0xFF;
           info.uQuality    = ((uint32)p2 >> 8)  & 0xFF;

           /* Perform range checking for uints, only max checks needed */
           if(info.uMode       <= OEMCAMCORDER_AAC_MODE_MAX_VALUE &&
              info.dwFrequency <= OEMCAMCORDER_AAC_FREQ_MAX_VALUE &&
              info.uQuality    <= OEMCAMCORDER_AAC_QUALITY_MAX_VALUE)
           {
             pme->m_dwSubAACAudio.uMode       = info.uMode;
             pme->m_dwSubAACAudio.dwFrequency = info.dwFrequency;
             pme->m_dwSubAACAudio.uQuality    = info.uQuality;
           }
           else
           {
             nRet = EBADPARM;
           }
         }
         else
#endif /* FEATURE_CAMCORDER_AAC_REC */
         if(p1 != VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
         {
           /* return success as audio format is set for all formats except AAC */
           return nRet;
         }
         break;

#if !defined(FEATURE_CAMERA_V2)
       case CAM_PARM_SPACE_LIMIT:
         if( p1 >= CAM_SPACELIMIT_DEFAULT &&
             p1 <= CAM_SPACELIMIT_HIGH
           )
           pme->m_nSpaceLimitThreshold = (uint8)p1;
         //if p1 is high then MAX and low then MIN then set default
         else
           pme->m_nSpaceLimitThreshold = (uint8)CAM_SPACELIMIT_DEFAULT;
         eParm = CAMERA_PARM_SPACE_LIMIT;
         break;
#endif //FEATURE_CAMERA_V2
#endif /* FEATURE_VIDEO_ENCODE */

      case CAM_PARM_REFLECT:
        eParm = CAMERA_PARM_REFLECT;
          break;
      case CAM_PARM_PREVIEW_TYPE:
        eParm = CAMERA_PARM_PREVIEW_MODE;
         if ((p1 == CAM_PREVIEW_SNAPSHOT) ||
             (p1 == CAM_PREVIEW_MOVIE))
            pme->m_nPreviewType = (int16)p1;
         else
            nRet = EBADPARM;
        break;

      case CAM_PARM_SIZE:
         if (p1)
         {
            AEESize *pSize = (AEESize *) p1;
            if (pSize->cx <= 0 || pSize->cy <= 0) 
            { 
               nRet = EBADPARM; 
               break; 
            }
            MEMCPY(&pme->m_sizeFrame, (AEESize *)p1, sizeof(AEESize));
#if defined FEATURE_CAMERA_V2 || defined FEATURE_CAMERA_V7
            nRet = (int)camera_set_dimensions((uint16)pme->m_sizeFrame.cx,
                                                 (uint16)pme->m_sizeFrame.cy,
                                                 (uint16)pme->m_sizeDisplay.cx,
                                                 (uint16)pme->m_sizeDisplay.cy,
                                                 NULL, NULL);
#else
            nRet = (int)camera_set_dimensions((uint16)pme->m_sizeFrame.cx, (uint16)pme->m_sizeFrame.cy, (uint16)pme->m_sizeDisplay.cx, NULL, NULL);
#endif //defined FEATURE_CAMERA_V2 || defined FEATURE_CAMERA_V7       
            nRet = OEMCamera_AEEError(nRet);          
         }
         else
         {
            nRet =EBADPARM; 
         }
         break;

      case CAM_PARM_QUALITY:
         // File size will take precedence over Quality if it is non zero
         // so clear file size here.
         pme->m_nFileSize = OEMCAMERA_FILE_SIZE_DISABLED;
         // Quality is treated differenty for snapshot VS movie modes
         if (pme->m_nPreviewType == CAM_PREVIEW_SNAPSHOT)
         {
            // Valid range for snapshot quality param is 1 to 100
            // equal the quality percent (for snapshot camera only).
            if ((p1 >= OEMCAMERA_QUALITY_MIN_VALUE) &&
                (p1 <= OEMCAMERA_QUALITY_MAX_VALUE))
               pme->m_nQuality = (int16)p1;
            else
               nRet = EBADPARM;
         }
         else if (pme->m_nPreviewType == CAM_PREVIEW_MOVIE)
         {

            if ((p1 >= OEMCAMCORDER_QUALITY_MIN_VALUE) &&
                (p1 <= OEMCAMCORDER_QUALITY_MAX_VALUE))
               pme->m_nQuality = (int16)p1;
            else
               nRet = EBADPARM;
         }
         break;

#if defined(FEATURE_CAMERA_V7)
      case CAM_PARM_BITRATE:
         /* User specifies bit rate for camcorder encoding*/
         if(p1 < OEMCAMCORDER_BITRATE_MIN_VALUE || p1 > OEMCAMCORDER_BITRATE_MAX_VALUE)
           nRet = EBADPARM;
         else
           pme->m_nBitrate = p1;
         break;

      case CAM_PARM_PREVIEW_FPS:
        if (p1)
        {
          eParm = CAMERA_PARM_PREVIEW_FPS;
        }
        break;

      case CAM_PARM_EXPOSURE_COMP:
        if (p1)
        {
          p1 = OEMCamera_ConvertSetEV(pme, (AEEFixedPointParm *)p1);
          eParm = CAMERA_PARM_EXPOSURE_COMPENSATION;
        }
        break;
#endif // defined(FEATURE_CAMERA_V7)

#if defined(FEATURE_CAMERA_V7) || defined(FEATURE_CAMERA_V2)
      case CAM_PARM_EXPOSURE_METERING:
        eParm = CAMERA_PARM_AUTO_EXPOSURE_MODE;
        break;
#endif // defined(FEATURE_CAMERA_V7) || defined(FEATURE_CAMERA_V2)

#if defined(FEATURE_CAMERA_V770)
      case CAM_PARM_ISO:
        nRet = OEMCamera_ConverISO(&p1, TRUE);
        if (nRet == SUCCESS)
          eParm = CAMERA_PARM_ISO;
        break;

      case CAM_PARM_LUMA_ADAPTATION:
        eParm = CAMERA_PARM_LUMA_ADAPTATION;
        break;

      case CAM_PARM_FLASH_CTL:
        if (p1)
        {
          if ((p1 & CAM_FLASHTYPE_STROBE) == CAM_FLASHTYPE_STROBE)
            eParm = CAMERA_PARM_STROBE_FLASH;
          else
            eParm = CAMERA_PARM_FLASH;
          p1 = p2 ? p2 : (p1 & ~CAM_FLASHTYPE_MASK);
        }
        else
            nRet = EBADPARM;
        break;

      case CAM_PARM_AF_INFO:
        eParm = CAMERA_PARM_AF_MODE;
        break;

#endif // defined(FEATURE_CAMERA_V770)

      case CAM_PARM_FPS:
//         if (gCameraInfo.fps != (uint16)pme->m_nFPS)
//            nRet = EBADPARM;
         if (p1)
         {
            pme->m_nFPS = (byte)p1;
            eParm = CAMERA_PARM_FPS;
         }
         else
            nRet = EBADPARM;
         break;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
      case CAM_PARM_USER_ATOM:
         eParm = CAMERA_PARM_USER_ATOM;
         break;
#endif
      case CAM_PARM_CONTRAST:
         eParm = CAMERA_PARM_CONTRAST;
         break;

      case CAM_PARM_BRIGHTNESS:
         eParm = CAMERA_PARM_BRIGHTNESS;
         break;

      case CAM_PARM_SHARPNESS:
         eParm = CAMERA_PARM_SHARPNESS;
         break;

#ifdef FEATURE_CAMERA7500
      case CAM_PARM_HUE:
         eParm = CAMERA_PARM_HUE;
         break;

      case CAM_PARM_SATURATION:
         eParm = CAMERA_PARM_SATURATION;
         break;
#endif //FEATURE_CAMERA7500

      case CAM_PARM_ZOOM:
         eParm = CAMERA_PARM_ZOOM;
         break;

#ifdef FEATURE_CAMERA_V770
      case CAM_PARM_FRAME_TIMESTAMP:
         eParm = CAMERA_PARM_FRAME_TIMESTAMP;
         break;
#endif // FEATURE_CAMERA_V770

      case CAM_PARM_NIGHTSHOT_MODE:
         eParm = CAMERA_PARM_NIGHTSHOT_MODE;
         break;

      case CAM_PARM_ANTIBANDING:
         eParm = CAMERA_PARM_ANTIBANDING;
         break;

      case CAM_PARM_ROTATE_PREVIEW:
         eParm = CAMERA_PARM_PREVIEW_ROTATION;
         if (p1 == 0)
            p1 = CAMERA_SP_NORMAL;
         else if (p1 == 180)
            p1 = CAMERA_SP_REVERSE;
         else if (p1 == 90)
            p1 = CAMERA_SP_CLK_TANGENTIAL;
         else if (p1 == 270)
            p1 = CAMERA_SP_ACLK_TANGENTIAL;
         else
            nRet = EBADPARM;
         break;

      case CAM_PARM_ROTATE_ENCODE:
         eParm = CAMERA_PARM_ENCODE_ROTATION;
         break;

      case CAM_PARM_OVERLAY:
         nRet = OEMCamera_AddOverlay(pme, (IDIB *)p1);
         break;

      case CAM_PARM_DISPLAY_SIZE:
         if (p1)
         {
            AEESize *pSize = (AEESize *) p1;
            if (pSize->cx <= 0 || pSize->cy <= 0) 
            { 
               nRet = EBADPARM; 
               break; 
            }
            MEMCPY(&pme->m_sizeDisplay, (void *)p1, sizeof(AEESize));
            if (pme->m_sizeFrame.cx != 0 && pme->m_sizeFrame.cy != 0)
            {
#if defined FEATURE_CAMERA_V2 || defined FEATURE_CAMERA_V7
               nRet = (int)camera_set_dimensions((uint16)pme->m_sizeFrame.cx,
                                                 (uint16)pme->m_sizeFrame.cy,
                                                 (uint16)pme->m_sizeDisplay.cx,
                                                 (uint16)pme->m_sizeDisplay.cy,
                                                 NULL, NULL);

#else
               nRet = (int)camera_set_dimensions((uint16)pme->m_sizeFrame.cx, (uint16)pme->m_sizeFrame.cy, (uint16)pme->m_sizeDisplay.cx, NULL, NULL);
#endif //defined FEATURE_CAMERA_V2 || defined FEATURE_CAMERA_V7
            }
            nRet = OEMCamera_AEEError(nRet);            
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      case CAM_PARM_DEFER_ENCODE:
         if (OEMCamera_CAMERAREADY(pme))
            pme->m_bDeferEncode = (boolean)p1;
         else
            nRet = EBADSTATE;
         break;

      case CAM_PARM_THUMBNAIL_INFO:
         if(p1)
         {
            CameraThumbnailInfo *cti = (CameraThumbnailInfo*)p1;
            AEESize   tsize = cti->size;
            uint32    quality = cti->nQuality;

            if((tsize.cx > 0)  || (tsize.cy > 0) || (quality > 0))
            {
              nRet = camera_set_thumbnail_properties(tsize.cx,tsize.cy,quality);
              nRet = OEMCamera_AEEError(nRet);
            }
            else
              nRet = EBADPARM;
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      case CAM_PARM_GPSINFO:
         if (p1)
         {
            AEEGPSInfo *         pgi = (AEEGPSInfo *)p1;
            camera_position_type pt;

            pt.timestamp = pgi->dwTimeStamp;
            pt.altitude = pgi->wAltitude;
            pt.latitude = pgi->dwLat;
            pt.longitude = pgi->dwLon;

            nRet = camera_set_position(&pt, NULL, NULL);
            nRet = OEMCamera_AEEError(nRet);
         }
         else
            nRet = EBADPARM;
         break;

      case CAM_PARM_EXIF_IFDTAG:
         if (p1)
         {
            CameraExifTagInfo *     pti = (CameraExifTagInfo *)p1;
            camera_exif_tag_type    tt;

            tt.tag_id = pti->wTagID;
            tt.type = (tiff_data_type)pti->wTagType;
            tt.value = pti->pTagData;
            tt.count = (uint16)pti->dwBytes;
            tt.ifd = (camera_ifd_type)(pti->wIFDID == CAM_EXIF_IFD_DEFAULT ? DEFAULT_IFD : pti->wIFDID);
            nRet = camera_set_exif_tag(&tt, NULL, NULL);
            nRet = OEMCamera_AEEError(nRet);
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      case CAM_PARM_EXPOSURE:
         if (p1)
            eParm = CAMERA_PARM_EXPOSURE;
         else
            nRet = EBADPARM;
         break;

      case CAM_PARM_WHITE_BALANCE:
         if (p1)
            eParm = CAMERA_PARM_WB;
         else
            nRet = EBADPARM;
         break;

      case CAM_PARM_EFFECT:
         if (p1)
            eParm = CAMERA_PARM_EFFECT;
         else
            nRet = EBADPARM;
         break;

      

      case CAM_PARM_RED_EYE_REDUCTION:
         eParm = CAMERA_PARM_RED_EYE_REDUCTION;
         break;
#ifdef FEATURE_CAMERA_NOFULLSCREEN
      case CAM_PARM_LCD_DIRECT_ACCESS:
      {
        if(p2 != 0)
        {
          AEERect *pRect = (AEERect *) p2;
          pme->m_cxOffset       = pRect->x;
          pme->m_cyOffset       = pRect->y;
          pme->m_sizeDisplay.cx = pRect->dx;
          pme->m_sizeDisplay.cy = pRect->dy;
          bDirectToLCD= (boolean)p1;
          if(bDirectToLCD)
          {
            disp_lock_screen(pRect->y, pRect->dy, pRect->x, pRect->dx);
          }
          else
          {
            disp_lock_screen(0, 0, 0, 0);
          }
        }
        else //p1 > 0 and p2 == 0
        {
          nRet = CAMERA_INVALID_PARM;
        }
      }
      //No callback required for this SetParm.
      nRet = OEMCamera_AEEError(nRet);
      break;
#endif
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
      //Suggest using CAM_PARM_LCD_DIRECT_ACCESS_EX to replace this
      case CAM_PARM_LCD_DIRECT_ACCESS:
      {
        if(p1 == 0 )
        {
          //Turn off direct mode
          nRet = camera_set_lcd_direct_mode(0, 0, 0, 0, 0);
          if (nRet == CAMERA_SUCCESS)
            bDirectMode = FALSE;
        }
        else if(p2 != 0)
        {
          AEESize *pSize = (AEESize *) p2;
          pme->m_sizeDisplay.cx = pSize->cx;
          pme->m_sizeDisplay.cy = pSize->cy;
          /* If Direct Mode is enabled then default the
           * Display to be Primary LCD , MDP Layer 0 [ BGL ] -> 0x10
           * Do this since we only get AEESize in p2 -> x = 0, y = 0
           * camera_lcd_direct_mode(mode,x,y,dx,dy) */
          nRet = camera_set_lcd_direct_mode(0x10, 0, 0,
                                            pme->m_sizeDisplay.cx,
                                            pme->m_sizeDisplay.cy);
          if (nRet == CAMERA_SUCCESS)
            bDirectMode = TRUE;
        }
        else //p1 > 0 and p2 == 0
        {
          nRet = CAMERA_INVALID_PARM;
        }
      }
      //No callback required for this SetParm.
      nRet = OEMCamera_AEEError(nRet);
      break;

      case CAM_PARM_LCD_DIRECT_ACCESS_EX:
      {
        int32 mode = 0x0 ;
        CameraDirectMode *pCamDirectInfo = (CameraDirectMode *)p2;
        bDirectMode = (boolean)p1;
        if(pCamDirectInfo)
        {
          //Turn on the direct mode with some required info
          mode = (int32)pCamDirectInfo->clsDisplay + 1;
          if (mode > 0x2)
          {
            //We only support primary (0x10) and secondary (0x20), Invalid mode
            mode = 0;
          }

          mode <<= 4;    /*Upper Nibble is the Display destination */

          if (pCamDirectInfo->nLayer == -1)
          {
            mode |= 0; //Background layer: MDP_LAYER_0
          }
          else if(pCamDirectInfo->nLayer == 0)
          {
            mode |= 1; //First layer: MDP_LAYER_1
          }
          else if(pCamDirectInfo->nLayer == 1)
          {
            mode |= 2; //Foreground layer: MDP_LAYER_2
          }
          else
          {
            //Invalid mode
            mode = 0;
          }
          MSG_FATAL("CAM_PARM_LCD_DIRECT_ACCESS_EX %x",mode,0,0);
          if(mode > 0)
          {
            nRet = camera_set_lcd_direct_mode(mode,
                                              pCamDirectInfo->xDst,
                                              pCamDirectInfo->yDst,
                                              pCamDirectInfo->cxDst,
                                              pCamDirectInfo->cyDst);
            if (nRet == CAMERA_SUCCESS){}
          }
          else
          {
            bDirectMode = FALSE;
            nRet = CAMERA_INVALID_PARM;
          }
        }
        else if(p1 == 0 )
        {
          //Turn off direct mode
          nRet = camera_set_lcd_direct_mode(0, 0, 0, 0, 0);
          if (nRet == CAMERA_SUCCESS)
            bDirectMode = FALSE;
        }
        else //p1 > 0 && p2 == 0
        {
           nRet = CAMERA_INVALID_PARM;
        }
      }
      //No callback required for this SetParm.
      nRet = OEMCamera_AEEError(nRet);
      break; /* case CAM_PARM_LCD_DIRECT_ACCESS_EX: */
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

      case CAM_PARM_FILE_SIZE:
         // File size will take precidence over Quality if it is non zero
         // Units are Kilobytes for this param
         if ((p1 >= OEMCAMERA_FILE_SIZE_MIN_VALUE) &&
             (p1 <= OEMCAMERA_FILE_SIZE_MAX_VALUE))
          pme->m_nFileSize = (int32)p1;
         else
            nRet = EBADPARM;
         break;
#ifdef FEATURE_VIDEO_ENCODE_FADING
     case CAM_PARM_FADE:
        eParm = CAMERA_PARM_FADING;
        switch(p1)
        {
         case CAM_FADE_IN:
           p1 = CAMERA_PARM_FADE_IN;
          break;
          case CAM_FADE_OUT:
           p1 = CAMERA_PARM_FADE_OUT;
          break;
         case CAM_FADE_IN_OUT:
           p1 = CAMERA_PARM_FADE_IN_OUT;
          break;
         case CAM_FADE_OFF:
         default:
           p1 = CAMERA_PARM_FADE_OFF;
          break;
        }
       break;
#endif //FEATURE_VIDEO_ENCODE_FADING
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
     case CAM_PARM_TRANSITION:
       eParm = CAMERA_PARM_TRANSITION;
       switch(p1)
       {
       case CAM_TRANSITION_ALPHA:
         p1 = CAMERA_PARM_TRANSITION_ALPHA;
         break;
       case CAM_TRANSITION_CURTAIN:
         p1 = CAMERA_PARM_TRANSITION_CURTAIN;
         break;
       case CAM_TRANSITION_OFF:
       default:
         p1 = CAMERA_PARM_TRANSITION_OFF;
         break;
       }
       break;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */
      case CAM_PARM_FOCUS:
#ifndef FEATURE_CAMERA_V2
        nRet = OEMCamera_DoFocus(pme, p1, p2);
#endif //FEATURE_CAMERA_V2
        break;

      case CAM_PARM_FOCUS_RECT:
#ifndef FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA7500
        eParm = CAMERA_PARM_FOCUS_RECT;
#else
        if (p1)
        {
          AEERect *pRc = (AEERect *)p1;
          nRet = OEMCamera_SetFocusRect(*pRc);
        }
#endif //FEATURE_CAMERA7500
#endif //#ifndef FEATURE_CAMERA_V2
        break;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif //FEATURE_CAMERA_BESTSHOT_MODE

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)      
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

#ifdef FEATURE_CAMERA_HISTOGRAM
      case CAM_PARM_HISTOGRAM:
        eParm = CAMERA_PARM_HISTOGRAM;
        break;
#endif //FEATURE_CAMERA_HISTOGRAM
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      case CAM_PARM_SHUTTER_SOUND:
        eParm = CAMERA_PARM_SHUTTER_SOUND;
        break;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND

	case CAM_PARM_WB:
		if(p1)
		{
			eParm = CAMERA_PARM_WB;
		}
		else
		{
			nRet = EBADPARM;
		}
		break;
	#ifdef FEATURE_LCD_TOUCH_ENABLE
	case CAM_PARM_FLASH:
		MSG_FATAL("OEMCamera_ContrlFlash....start.....",0,0,0);
		gpio_tlmm_config(GPIO_OUTPUT_54);
		if(p1)
		{
			MSG_FATAL("ONNNNNNNNNNNNNNN",0,0,0);
			gpio_out(GPIO_OUTPUT_54,(GPIO_ValueType)GPIO_HIGH_VALUE);
		}
		else
		{
			MSG_FATAL("OFFFFFFFFFFFFFFF",0,0,0);
			gpio_out(GPIO_OUTPUT_54,(GPIO_ValueType)GPIO_LOW_VALUE);
		}
		MSG_FATAL("OEMCamera_ContrlFlash....end.....",0,0,0);
		break;
	#else
	case CAM_PARM_FLASH:
         eParm = CAMERA_PARM_FLASH;
         break;
	#endif
      case CAM_PARM_IS_SUPPORT:  // Fall through...
      case CAM_PARM_IS_MOVIE: // This cannot be set by user
      case CAM_PARM_PIXEL_COUNT:
      case CAM_PARM_VIDEO_ENCODE_LIST: // This cannot be set by user
      case CAM_PARM_AUDIO_ENCODE_LIST: // This cannot be set by user
      case CAM_PARM_SIZE_LIST: // This cannot be set by user
      case CAM_PARM_FPS_LIST: // This cannot be set by user
      default:
         nRet = EUNSUPPORTED;
         break;
   }
   MSG_FATAL("nRet=================%d",nRet,0,0);
   if (SUCCESS == nRet && eParm != CAMERA_PARM_MAX)
   {
      int         nCamRet;
      CameraRsp * pRsp;
      MSG_FATAL("OEMCamera_AllocCmdCameraRsp start",0,0,0);
      pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_SETPARM, nParmID);
      if (!pRsp)
         nRet = EITEMBUSY;
      else
      {
#if defined(FEATURE_CAMERA_V7)
         if (eParm == CAMERA_PARM_PREVIEW_FPS)
            nCamRet = OEMCamera_SetPreviewFPS((AEEFixedPointMinMax *)p1, pRsp);
#ifdef FEATURE_CAMERA_V770
         else if (eParm == CAMERA_PARM_AF_MODE
#ifdef FEATURE_VIDEO_ENCODE
          || (eParm == CAMERA_PARM_AUDIO_FMT && p1 == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
#endif
          )
            nCamRet = (int)camera_set_parm_2(eParm, p1, p2,
                                             OEMCamera_CameraLayerCB,
                                             (void *)pRsp->hObject);
#endif //FEATURE_CAMERA_V770
         else
#endif // FEATURE_CAMERA_V7
			MSG_FATAL("camera_set_parm..............",0,0,0);
            nCamRet = (int)camera_set_parm(eParm, p1, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
         if (!nCamRet)
            nRet = CAM_PENDING;
         else
         {
            OEMCamera_FreeCameraRsp(pRsp);
            nRet = OEMCamera_AEEError(nCamRet);
         }
      }
   }

   return nRet;
   #endif
}

/*==================================================================

==================================================================*/
int OEMCamera_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2)
{
#ifdef FEATURE_DSP
	OEMCamera *       pme = (OEMCamera *)h;
   int               nRet = SUCCESS;
   
   return nRet;

#else
   OEMCamera *       pme = (OEMCamera *)h;
   int               nRet = SUCCESS;
   camera_parm_type  eParm = CAMERA_PARM_MAX;

   switch (nParmID)
   {
      case CAM_PARM_MEDIA_DATA:
         if (pP1)
            MEMCPY(pP1, &pme->m_md, sizeof(AEEMediaData));
         if (pP2)
            *pP2 = (int32)&pme->m_pszMIME;
         break;

      case CAM_PARM_VIDEO_ENCODE:
         if (pP1)
            *pP1 = (int32)pme->m_clsVideo;
         if (pP2)
            *pP2 = (int32)pme->m_dwSubVideo;
         break;
#ifdef FEATURE_VIDEO_ENCODE
      case CAM_PARM_AUDIO_ENCODE:
         if (pP1)
            *pP1 = (int32)pme->m_clsAudio;
         if (pP2)
         {
           if(pP1 && *pP1 == AEECLSID_MEDIAQCP)
            *pP2 = (int32)pme->m_dwSubAudio;
#ifdef FEATURE_CAMCORDER_AAC_REC
           else if(pP1 && *pP1 == AEECLSID_MEDIAAAC)
             MEMCPY(pP2, &pme->m_dwSubAACAudio, sizeof(CameraAACEncodingInfo));
#endif /* FEATURE_CAMCORDER_AAC_REC */
           else
             *pP2 = -1;
         }

         break;
#if !defined(FEATURE_CAMERA_V2)
      case CAM_PARM_SPACE_LIMIT:
         if(pP1)
            *pP1 = (int8)pme->m_nSpaceLimitThreshold;
         break;
#endif // FEATURE_CAMERA_V2
#endif /* FEATURE_VIDEO_ENCODE */
      case CAM_PARM_REFLECT:
         eParm = CAMERA_PARM_REFLECT;
         break;

      case CAM_PARM_PIXEL_COUNT: 
        if (pP1) 
        {
          camera_info_type cInfo = {0};
          camera_get_info(&cInfo);
          pme->sensorSize.cx = cInfo.sensor_width; 
          pme->sensorSize.cy = cInfo.sensor_height; 
          MEMCPY(pP1, &pme->sensorSize, sizeof(AEESize)); 
        }
        break; 

      case CAM_PARM_SIZE:
         if (pP1)
            MEMCPY(pP1, &pme->m_sizeFrame, sizeof(AEESize));
         break;

      case CAM_PARM_QUALITY:
         if (pP1)
            *pP1 = (int32)pme->m_nQuality;
         if (pP2)
         {
            AEEParmInfo pi;

            pi.nCurrent = pme->m_nQuality;


            if (pme->m_nPreviewType == CAM_PREVIEW_SNAPSHOT)
            {
               pi.nDefault = OEMCAMERA_QUALITY_HIGH;
               pi.nMin = OEMCAMERA_QUALITY_MIN_VALUE;
               pi.nMax = OEMCAMERA_QUALITY_MAX_VALUE;
               pi.nStep = (int32)1;
            }
            else if (pme->m_nPreviewType == CAM_PREVIEW_MOVIE)
            {
               pi.nDefault = OEMCAMCORDER_QUALITY_MAX_VALUE;
               pi.nMin = OEMCAMCORDER_QUALITY_MIN_VALUE;
               pi.nMax = OEMCAMCORDER_QUALITY_MAX_VALUE;
               pi.nStep = (int32)1;
            }


            MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
         }
         break;

#if defined(FEATURE_CAMERA_V7)
      // Get user defined bit rate
      case CAM_PARM_BITRATE:
         if (pP1)
         {
           *pP1 = (int32)pme->m_nBitrate;
         }
         if (pP2)
         {
             AEEParmInfo pi;
             pi.nCurrent = pme->m_nBitrate;
             pi.nDefault = OEMCAMCORDER_BITRATE_DEFAULT_VALUE;
             pi.nMax     = OEMCAMCORDER_BITRATE_MAX_VALUE;
             pi.nMin     = OEMCAMCORDER_BITRATE_MIN_VALUE;
             pi.nStep    = OEMCAMCORDER_BITRATE_MIN_VALUE;
             MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
         }
         break;

      case CAM_PARM_PREVIEW_FPS:
        eParm = CAMERA_PARM_PREVIEW_FPS;
        break;

      case CAM_PARM_EXPOSURE_COMP:
        eParm = CAMERA_PARM_EXPOSURE_COMPENSATION;
        break;
#endif // defined(FEATURE_CAMERA_V7)

#if defined(FEATURE_CAMERA_V7) || defined(FEATURE_CAMERA_V2)
      case CAM_PARM_EXPOSURE_METERING:
        eParm = CAMERA_PARM_AUTO_EXPOSURE_MODE;
        break;

#endif // defined(FEATURE_CAMERA_V7) || defined(FEATURE_CAMERA_V2)

#if defined(FEATURE_CAMERA_V770)
      case CAM_PARM_ISO:
        eParm = CAMERA_PARM_ISO;
        break;

      case CAM_PARM_LUMA_ADAPTATION:
        eParm = CAMERA_PARM_LUMA_ADAPTATION;
        break;

      case CAM_PARM_FLASH_CTL:
        if (pP1)
        {
          if ((*pP1 & CAM_FLASHTYPE_STROBE) == CAM_FLASHTYPE_STROBE)
            eParm = CAMERA_PARM_STROBE_FLASH;
          else
            eParm = CAMERA_PARM_FLASH;
        }
       else
            nRet = EBADPARM;
        break;

      case CAM_PARM_AF_INFO:
        {
          int32 val1;
          int32 val2;

          nRet = (int)camera_get_parm_2(CAMERA_PARM_AF_MODE, &val1, &val2);
          nRet = OEMCamera_AEEError(nRet);

          if(nRet == SUCCESS)
          {
            if (pP1)
              *pP1 = (uint32)val1;
            if (pP2)
              *pP2 = (uint32)val2;
          }
        }
        break;

#endif // defined(FEATURE_CAMERA_V770)

     case CAM_PARM_FPS:
         eParm = CAMERA_PARM_FPS;
         break;

      case CAM_PARM_CONTRAST:
         eParm = CAMERA_PARM_CONTRAST;
         break;

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
	  case CAM_PARM_CURRENT_SENSOR:
	  	   eParm = CAMERA_PARM_CURRENT_SENSOR;
         break;
#endif
      case CAM_PARM_BRIGHTNESS:
           eParm = CAMERA_PARM_BRIGHTNESS;
         break;

      case CAM_PARM_SHARPNESS:
         eParm = CAMERA_PARM_SHARPNESS;
         break;

#ifdef FEATURE_CAMERA7500
        case CAM_PARM_HUE:
         eParm = CAMERA_PARM_HUE;
         break;

      case CAM_PARM_SATURATION:
         eParm = CAMERA_PARM_SATURATION;
         break;
#endif //FEATURE_CAMERA7500

      case CAM_PARM_ZOOM:
         eParm = CAMERA_PARM_ZOOM;
         break;

      case CAM_PARM_NIGHTSHOT_MODE:
         eParm = CAMERA_PARM_NIGHTSHOT_MODE;
         break;

      case CAM_PARM_ANTIBANDING:
         eParm = CAMERA_PARM_ANTIBANDING;
         break;

      case CAM_PARM_ROTATE_PREVIEW:
         eParm = CAMERA_PARM_PREVIEW_ROTATION;
         break;

      case CAM_PARM_ROTATE_ENCODE:
         eParm = CAMERA_PARM_ENCODE_ROTATION;
         break;

      case CAM_PARM_OVERLAY:
         nRet = EUNSUPPORTED;
         break;

      case CAM_PARM_DISPLAY_SIZE:
         if (pP1)
            MEMCPY((void *)pP1, &pme->m_sizeDisplay, sizeof(AEESize));
         break;

      case CAM_PARM_DEFER_ENCODE:
         if (pP1)
            *(boolean *)pP1 = pme->m_bDeferEncode;
         break;

      case CAM_PARM_MODE:
         if (pP1)
            *(int16 *)pP1 = pme->m_nMode;
         if (pP2)
            *(boolean *)pP2 = pme->m_bPaused;
         break;

      case CAM_PARM_EXPOSURE:
         eParm = CAMERA_PARM_EXPOSURE;
         break;

      case CAM_PARM_WHITE_BALANCE:
         eParm = CAMERA_PARM_WB;
         break;

      case CAM_PARM_EFFECT:
         eParm = CAMERA_PARM_EFFECT;
         break;

      case CAM_PARM_RED_EYE_REDUCTION:
         eParm = CAMERA_PARM_RED_EYE_REDUCTION;
         break;
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      case CAM_PARM_SHUTTER_SOUND:
        nRet = EUNSUPPORTED;
        break;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND

      case CAM_PARM_THUMBNAIL_INFO:
         if(pP1)
         {
            CameraThumbnailInfo cti={0};
            uint32 height;
            uint32 width;
            uint32 quality;

            nRet = camera_get_thumbnail_properties(&width, &height, &quality);
            nRet = OEMCamera_AEEError(nRet);

            if(nRet == SUCCESS)
            {
              cti.nStructSize = sizeof(CameraThumbnailInfo);
              cti.size.cx = width;
              cti.size.cy = height;
              cti.nQuality = (int16)quality;
              MEMCPY(pP1,&cti,sizeof(CameraThumbnailInfo));
            }
         }
         break;

      case CAM_PARM_SENSOR_INFO:
        if(pP1)
        {
          CameraSensorInfo csi = {0};
          camera_info_type cInfo = {0};

          camera_get_info(&cInfo);

          csi.dwSize = sizeof(CameraSensorInfo);
          STRTOWSTR(cInfo.sensor_model, csi.szName, sizeof(csi.szName));
          csi.size.cx = cInfo.sensor_width;
          csi.size.cy = cInfo.sensor_height;
          csi.dwFPSPreview = cInfo.fps;
          csi.bLandscape = (cInfo.default_orientation == CAMERA_ORIENTATION_LANDSCAPE);
          MEMCPY(pP1,&csi,sizeof(CameraSensorInfo));
        }
        break;
      case CAM_PARM_SENSOR_MODEL:
      {
	  	  
          if(FALSE
		  	#if defined( USE_CAMSENSOR_SIV121A) 
		  	||camsensor_static_params[0].sensor_model == CAMSENSOR_SIV121A_ID
		  	#endif
		  	#if defined(USE_CAMSENSOR_DB8B63A)
			||camsensor_static_params[0].sensor_model == CAMSENSOR_DB8V63A_ID
			#endif
		  	)
          {
                *pP1 = 30;
          }
          else if(camsensor_static_params[0].sensor_model == CAMSENSOR_SIC110A_ID)
          {
               *pP1 = 10;
          }
          else
          {
          	   *pP1 = 10;
          }
          break;
      }
        
      case CAM_PARM_FOCUS_RECT:
#ifndef FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA7500
          eParm = CAMERA_PARM_FOCUS_RECT;
#else
          nRet = OEMCamera_GetFocusRect(pP1, pP2);
#endif //FEATURE_CAMERA7500
#endif //#ifndef FEATURE_CAMERA_V2
        break;

      case CAM_PARM_FILE_SIZE:
         // Units are Kilobytes for this param
         if (pP1)
            *pP1 = (int32)pme->m_nFileSize;
         if (pP2)
         {
            AEEParmInfo pi;

            pi.nCurrent = pme->m_nFileSize;
            pi.nDefault = OEMCAMERA_FILE_SIZE_MIN_VALUE;
            pi.nMin = OEMCAMERA_FILE_SIZE_MIN_VALUE;
            pi.nMax = OEMCAMERA_FILE_SIZE_MAX_VALUE;
            pi.nStep = (int32)1;

            MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
         }
         break;

      case CAM_PARM_FOCUS:
         eParm = CAMERA_PARM_FOCUS_STEP;
         break;

      case CAM_PARM_PREVIEW_TYPE:
         eParm = CAMERA_PARM_PREVIEW_MODE;
         break;

      case CAM_PARM_SIZE_LIST:
        if ( pP1 && (*pP1 == CAM_MODE_MOVIE))
        {
          *pP1 = (int32)gCameraMovieSizeList;
        }
        else if (pP1 && (*pP1 == CAM_MODE_SNAPSHOT) )
        {
          *pP1 = (int32)gCameraSnapshotSizeList;
        }
        else
        {
          nRet = EUNSUPPORTED;
        }
	if (pP2)
          *pP2 = FALSE; // The size list is a discrete one
        break;

#ifdef FEATURE_CAMERA_V770
     case CAM_PARM_FRAME_TIMESTAMP:
         eParm = CAMERA_PARM_FRAME_TIMESTAMP;
        break;
#endif

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)  
#error code not present
#endif
	
      case CAM_PARM_IS_MOVIE:
        if (pP1)
        {
#ifdef FEATURE_VIDEO_ENCODE
          *(boolean *)pP1 = 1; // TRUE
#else
          *(boolean *)pP1 = 0; // FALSE
#endif //FEATURE_VIDEO_ENCODE
        }
        else
          nRet = EBADPARM;
        break;

      case CAM_PARM_IS_SUPPORT:
        if (pP1 && pP2)
        {
          nRet = OEMCamera_GetSupportStatus(pme, (int16)(int32)pP1);
          if (nRet == SUCCESS)
          {
            *(boolean *)pP2 = 1; //TRUE;
          }
          else
          {
            *(boolean *)pP2 = 0; //FALSE;
          }
        }
        else
          nRet = EBADPARM;
        break;

#ifdef FEATURE_CAMERA_V790
      case CAM_PARM_FPS_LIST:
        if (pP1)
        {
          int32 nSize = 0;
          uint32 *pList = NULL;

          nRet = (int)camera_get_parm_2(CAMERA_PARM_FPS_LIST, pP1, &nSize);
          nRet = OEMCamera_AEEError(nRet);

          if(nRet == SUCCESS)
          {
            if (nSize > 0)
            {
              //make sure it is a NULL terminate fps list
              pList = (uint32 *)(*pP1);
              pList += nSize;
              if (*pList != NULL)
                nRet = EFAILED;
            }
            else
              nRet = EFAILED;
          }
          if (pP2)
            *pP2 = FALSE;
        }
        break;
#endif //FEATURE_CAMERA_V790

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif //FEATURE_CAMERA_BESTSHOT_MODE

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
       case CAM_PARM_LCD_DIRECT_ACCESS:
       {
         if(!pP1 || !pP2)
         {
           nRet = EBADPARM;
         }
         else
         {
           AEESize *size = (AEESize *)pP2;
           *(boolean *)pP1 = bDirectMode;
           size->cx = pme->m_sizeDisplay.cx;
           size->cy = pme->m_sizeDisplay.cy;
         }
       }
       break;

       case CAM_PARM_LCD_DIRECT_ACCESS_EX:
         *(boolean *)pP1 = bDirectMode;
         break;
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
#ifdef FEATURE_VIDEO_ENCODE
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif//FEATURE_VIDEO_ENCODE
  case CAM_PARM_DISPLAY_SIZE_LIST: 
    if ( pP1 && (*pP1 == CAM_MODE_MOVIE || *pP1 == CAM_MODE_PREVIEW) ) 
    { 
      *pP1 = (int32)gCameraDisplaySizeList; 
    } 
    else 
    { 
      nRet = EUNSUPPORTED; 
    } 
    if (pP2) 
      *pP2 = FALSE;  // The display size list is a discrete one
    break; 
 
  case CAM_PARM_VIDEO_ENCODE_LIST:   
     if (pP1) 
        *pP1 = (int32)gCameraEncodeVideoList; 
     break; 
      
  case CAM_PARM_AUDIO_ENCODE_LIST:  
     if (pP1) 
        *pP1 = (int32)gCameraEncodeAudioList; 
     break; 
      default:
         nRet = EUNSUPPORTED;
         break;
   }

   if ((pP1 || pP2) && eParm != CAMERA_PARM_MAX && SUCCESS == nRet)
   {
      int         nCamRet;
      AEEParmInfo pi;

      MEMSET(&pi, (int)0, sizeof(AEEParmInfo));
      nCamRet = (int)camera_get_parm(eParm, (camera_parm_info_type *)&pi);
      if (!nCamRet)
      {
         // Handle some exceptions...this can be done in a function if the list gets big.
         // Convert sensor position to rotate-angle...
         switch (nParmID)
         {
         case CAM_PARM_ROTATE_PREVIEW:
            if (pP1)
               *pP1 = pi.nCurrent ? 180 : 0;
            if (pP2)
            {
               pi.nMin = 0;
               pi.nMax = 180;
               pi.nStep = 180;
               MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
            }
            break;
         case CAM_PARM_RED_EYE_REDUCTION:
            if (pP1)
               *(boolean *)pP1 = (boolean)pi.nCurrent;
            break;
#ifdef FEATURE_CAMERA_V7
         case CAM_PARM_EXPOSURE_COMP:
            OEMCamera_ConvertGetEV(pme, &pi,
                                   (AEEFixedPointParm *)pP1,
                                   (AEEFixedPointParmInfo *)pP2);
            break;
         case CAM_PARM_PREVIEW_FPS:
            OEMCamera_ConvertGetFPS(&pi, (AEEFixedPointMinMax *)pP1);
            break;
#endif //FEATURE_CAMERA_V7
#ifdef FEATURE_CAMERA_V770
         case CAM_PARM_ISO:
            //Only the current value and Max. value are useful for ISO.
            nRet = OEMCamera_ConverISO(&pi.nCurrent, FALSE);
            if (nRet == SUCCESS)
            {
               if (pP1)
                  *pP1 = pi.nCurrent;

               nRet = OEMCamera_ConverISO(&pi.nMax, FALSE);
               if (pP2 && (nRet == SUCCESS))
               {
                  MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
               }
             }
             break;
          case CAM_PARM_FRAME_TIMESTAMP:
          case CAM_PARM_LUMA_ADAPTATION:
             if (pP1)
               *(boolean *)pP1 = (boolean)pi.nCurrent;
             break;
#endif //FEATURE_CAMERA_V770

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
		  case CAM_PARM_CURRENT_SENSOR:
			 if (pP1)
             *(cam_sensor_model_pair_type *)pP1 = (cam_sensor_model_pair_type)pi.nCurrent;
		  	 break;
#endif	
          default:
             if (pP1)
                *pP1 = pi.nCurrent;
             if (pP2)
                MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
         }
      }
      else
         nRet = OEMCamera_AEEError(nCamRet);
   }

   return nRet;
   #endif
}

/*==================================================================

==================================================================*/
int OEMCamera_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam)
{
   #ifdef FEATURE_DSP
   	OEMCamera * pme = (OEMCamera *)h;
	int         nRet = EFAILED;
    MSG_FATAL("OEMCamera_Start.................................",0,0,0);
	if (nMode == CAM_MODE_PREVIEW)
	{	    
		nRet = OEMCamera_Preview(pme,NULL);
	}
	else if (nMode == CAM_MODE_SNAPSHOT)
   	{
		nRet = OEMCamera_RecordSnapshot(pme,NULL);
	}
    else if (nMode == CAM_MODE_UPDATE)
    {
        nRet = OEMCamera_Update(pme, dwParam);
    }
	else
    {
    	nRet = EBADPARM;
	}

	return nRet;
   #else
   
   OEMCamera * pme = (OEMCamera *)h;
   int         nRet = EFAILED;
   CameraRsp * pRsp;
   
   if (!OEMCamera_STARTOK(pme))
      return EBADSTATE;

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   // check to see if an ACM operation is already in progress.
   // We cannot have more than one outstanding ACM transaction. So, return
   // failure.
   if (pme->m_cameraCallStartStruct.isValid)
   {
     return EBADSTATE;
   }
   
   // Starting a block for restricting scope.
   {
     int nRequestReasonCode;

     switch (nMode)
     {
       case CAM_MODE_PREVIEW:
       case CAM_MODE_SNAPSHOT:
       {
#ifdef FEATURE_CAMERA_MULTI_SENSOR
         if(pme->m_sensorCLSID == AEECLSID_CAMERA1)
         {
           nRequestReasonCode =  ACM_VIEW_FIND_CAMERA1;
         }
         else
         {
           nRequestReasonCode =  ACM_VIEW_FIND_CAMERA2;
         }
#else //FEATURE_CAMERA_MULTI_SENSOR
         nRequestReasonCode =  ACM_VIEW_FIND_CAMERA;
#endif // FEATURE_CAMERA_MULTI_SENSOR
         break;
       }
       case CAM_MODE_MOVIE:
       {
#ifdef FEATURE_CAMERA_MULTI_SENSOR
         if(pme->m_sensorCLSID == AEECLSID_CAMERA1)
         {
           nRequestReasonCode =  ACM_CAMCORD_CAMERA1;
         }
         else
         {
           nRequestReasonCode =  ACM_CAMCORD_CAMERA2;
         }
#else //FEATURE_CAMERA_MULTI_SENSOR
         nRequestReasonCode =  ACM_CAMCORD_CAMERA;
#endif // FEATURE_CAMERA_MULTI_SENSOR
         break;
       }

       default:
         DBGPRINTF("Error: Unknown mode specified to OEMCamera_Start");
         return EBADPARM;
     } // end switch

     // The following should never happen because this should go along
     // with the isValid flag on the m_CameraCallStartStruct.
     if(pme->m_enResStateCamera == RES_STATE_ACQUIRING)
     {
       DBGPRINTF("ERROR: Another thread is already acquiring the resource on "
                 "the same object. State Corrupted");
      return EBADSTATE;
     }

     nRet = OEMCamera_ACMTransaction(pme,nRequestReasonCode);

     if (nRet != SUCCESS)
     {
       // We could not acquire the resource for whatever reason
       return EFAILED;
     }

     // Even if we get a Success above, check the resource state.
     if( (pme->m_enResStateCamera == RES_STATE_UNKNOWN) ||
         (pme->m_enResStateCamera == RES_STATE_NORES) )
     {
       return EFAILED;
     }

     if(pme->m_enResStateCamera == RES_STATE_ACQUIRING)
     {
       pme->m_cameraCallStartStruct.mode = nMode;
       pme->m_cameraCallStartStruct.dwParam = dwParam;
       pme->m_cameraCallStartStruct.isValid = TRUE;
       return CAM_PENDING;
     }
   }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   pRsp = OEMCamera_AllocStartCameraRsp(pme, nMode);
   if (!pRsp)
      return EITEMBUSY;

   if (nMode == CAM_MODE_PREVIEW)
      nRet = OEMCamera_Preview(pme, pRsp);
   else if (nMode == CAM_MODE_SNAPSHOT)
   {
#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
      nRet = OEMCamera_RecordSnapshot(pme, pRsp);
   }
   else if (nMode == CAM_MODE_MOVIE)
   {
     if(dwParam ==CAM_MOVIE_POSTCARD)
     {
#ifdef FEATURE_POSTCARD_RECORDING
        pme->m_bAudioPostCardRecord = TRUE;
#else
        return EUNSUPPORTED;
#endif /* FEATURE_POSTCARD_RECORDING */
     }
     else
     {
       pme->m_bAudioPostCardRecord = FALSE;
     }

#ifdef FEATURE_VIDEO_ENCODE_MULTIMEDIA_TIER /* This feature is enabled for only 6260*/
/*This is to disable QVGA recording on 6260 for Camera Midlet*/
     if(pme->m_sizeFrame.cx == 240 && pme->m_sizeFrame.cy == 320 ||
         pme->m_sizeFrame.cx == 320 && pme->m_sizeFrame.cy == 240)
       nRet = EBADPARM;
     else
#endif
     nRet = OEMCamera_RecordMovie(pme, pRsp);
   }
   else
      nRet = EBADPARM;

   if (nRet)
   {
      OEMCamera_FreeCameraRsp(pRsp);
#ifdef FEATURE_ACM
      // We have failed doing something else, like starting a preview,
      // mallocing a memory or recording a snapshot. So, release our
      // claim on the resource.
      OEMACM_PriorityMgr_Free (pme) ;
      pme->m_enResStateCamera = RES_STATE_NORES ;
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
   }
   else
   {
#ifdef FEATURE_VIDEO_ENCODE
     if(nMode != CAM_MODE_MOVIE)
#endif /* FEATURE_VIDEO_ENCODE */
     {
      pme->m_bStateChanging = TRUE;
     }
   }

   return nRet;
   #endif
}

#ifdef FEATURE_DSP
int OEMCamera_Update(OEMINSTANCE h, uint32 dwParam)
{
	//OEMCamera * pme = (OEMCamera *)h;
    uint16* pbmp = (uint16*)dwParam;
    MEMCPY(g_fbuffer, pbmp, sizeof(g_fbuffer));
    AIT701_cam_update_osd(g_fbuffer,0,0,A8_MAIN_LCD_WIDTH,A8_MAIN_LCD_HEIGHT);
    return SUCCESS;
}

#endif

/*==================================================================

==================================================================*/
int OEMCamera_Stop(OEMINSTANCE h)
{
#ifdef FEATURE_DSP
//OEMCamera * pme = (OEMCamera *)h;
	
	AIT701_cam_exit_preview();
	return SUCCESS;

#else
   OEMCamera * pme = (OEMCamera *)h;
   int         nCamRet = -1;

   if ((!OEMCamera_STOPOK(pme)) &&
         !OEMCamera_STATECHANGING(pme))
      return EBADSTATE;

   if (OEMCamera_ISPREVIEW(pme))
   {
      nCamRet = (int)camera_stop_preview();
   }
   else if (OEMCamera_ISSNAPSHOT(pme))
   {
      nCamRet = camera_release_picture();
   }
#if defined(FEATURE_VIDEO_ENCODE)
   else if (OEMCamera_ISMOVIE(pme))
      nCamRet = (int)camera_stop_record();
#endif // defined(FEATURE_VIDEO_ENCODE)

#ifdef FEATURE_ACM
#if defined (SIRIUS_PLATFORM)
#error code not present
#else // SIRIUS_PLATFORM
   OEMACM_PriorityMgr_Free(pme);
#endif // SIRIUS_PLATFORM
   pme->m_enResStateCamera = RES_STATE_NORES;
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

   if (CAMERA_SUCCESS != nCamRet)
   {
      //Callback is registered on start privew.
      //If camera returns error code here, need to free the data.
      OEMCamera_FreeStartCameraRsp(pme);
   }
   return OEMCamera_AEEError(nCamRet);
   #endif
}

/*==================================================================

==================================================================*/



/*==================================================================

==================================================================*/
int OEMCamera_Pause(OEMINSTANCE h, boolean bPause)
{
   OEMCamera * pme = (OEMCamera *)h;
   int         nCamRet;

   if ( ( bPause && !OEMCamera_PAUSEOK(pme) ) ||
        (!bPause && !OEMCamera_RESUMEOK(pme)) )
      return EBADSTATE;

   pme->m_bPaused = bPause;

   if (bPause)
   {
      if (OEMCamera_ISPREVIEW(pme))
         nCamRet = camera_disable_frame_callback();
#if defined(FEATURE_VIDEO_ENCODE)
      else if (OEMCamera_ISMOVIE(pme))
         nCamRet = camera_pause_record();
#endif // defined(FEATURE_VIDEO_ENCODE)
      else
         nCamRet = CAMERA_RET_CODE_MAX;
   }
   else
   {
      if (OEMCamera_ISPREVIEW(pme))
         nCamRet = camera_enable_frame_callback();
#if defined(FEATURE_VIDEO_ENCODE)
      else if (OEMCamera_ISMOVIE(pme))
         nCamRet = camera_resume_record();
#endif // defined(FEATURE_VIDEO_ENCODE)
      else
         nCamRet = CAMERA_RET_CODE_MAX;
   }

   return OEMCamera_AEEError(nCamRet);
}

/*==================================================================

==================================================================*/
int OEMCamera_EncodeSnapshot(OEMINSTANCE hInstance)
{
#ifdef FEATURE_DSP
	OEMCamera *          pme = (OEMCamera *)hInstance;
   int                  nRet;  
 
	IFile    *pFileDst = NULL;
    DBGPRINTF("EncodeSnapshot-----%s",pme->m_pszFile);
    pFileDst = IFILEMGR_OpenFile(pme->m_pFileMgr,pme->m_pszFile,_OFM_CREATE);
    
	if ( pFileDst )
	{
	    DBGPRINTF("EncodeSnapshot-000---%0x---%s",pFileDst,pme->m_pszFile);
		IFILE_Write(pFileDst,g_fbuffer, pme->m_nFileSize);
	}
	RELEASEIF(pFileDst); 

   return (nRet);

#else
   OEMCamera *          pme = (OEMCamera *)hInstance;
   int                  nRet;
   camera_handle_type   h;
   CameraRsp *          pRsp = NULL;

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
   {
     int nRequestReasonCode;
#ifdef FEATURE_CAMERA_MULTI_SENSOR
       if(pme->m_sensorCLSID == AEECLSID_CAMERA1)
       {
           nRequestReasonCode =  ACM_VIEW_CAPTURE_CAMERA1 ;
       }
       else
       {
           nRequestReasonCode =  ACM_VIEW_CAPTURE_CAMERA2 ;
       }
#else // FEATURE_CAMERA_MULTI_SENSOR
       nRequestReasonCode =  ACM_VIEW_CAPTURE_CAMERA;
#endif // FEATURE_CAMERA_MULTI_SENSOR

       if(OEMCamera_ACMTransaction(pme,nRequestReasonCode) != SUCCESS)
       {
           // we are giving resource not found, but it could
           // be an acm error
           return EFAILED ;
       }


       if ( (pme->m_enResStateCamera == RES_STATE_UNKNOWN) ||
            (pme->m_enResStateCamera == RES_STATE_NORES) )
       {
         return EFAILED;
       }

       if(pme->m_enResStateCamera == RES_STATE_ACQUIRING)
       {
         // Make a note that this is an encode snapshot, so do not
         // look at mode in the ACM callback.
         pme->m_cameraCallStartStruct.encodingSnapshot = TRUE;
         pme->m_cameraCallStartStruct.dwParam = NULL;
         pme->m_cameraCallStartStruct.isValid = TRUE;
         return CAM_PENDING;
       }
   }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   if (!pme->m_pFrame || !pme->m_pFrame->m_dib.pBmp)
   {
     nRet = EFAILED;
     goto FAILURE;
   }

   pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_ENCODESNAPSHOT, 0);
   if (!pRsp)
   {
     nRet = EITEMBUSY;
     goto FAILURE;
   }

   nRet = OEMCamera_SetSnapshotDestination(pme, &h);

   if (SUCCESS != nRet)
   {
     goto FAILURE;
   }

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
   MSG_FATAL("camera_encode_picture",0,0,0);
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
   nRet = camera_encode_picture(&pme->m_rawFrame, &h, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
#else /* FEATURE_CAMERA_ENCODE_PROPERTIES */
   // If File Size Param was given a positive, non zero value,
   // then call API to encode (JPEG only) upto the specified
   // size which is in K bytes.
   // Only applicable to JPEG mode.
   if ((pme->m_nFileSize > 0) && ((pme->m_clsVideo) == AEECLSID_JPEG))
   {
     nRet = camera_encode_picture_size_limit(pme->m_nFileSize, &pme->m_rawFrame, &h, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
   }
   else
   {
     nRet = camera_encode_picture(&pme->m_rawFrame, &h, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
   }
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

   if (CAMERA_SUCCESS != nRet)
     OEMCamera_FreeCameraRsp(pRsp);

   return OEMCamera_AEEError(nRet);

 FAILURE:
   if (pRsp)
   {
     OEMCamera_FreeCameraRsp(pRsp);
   }
#ifdef FEATURE_ACM
   OEMACM_PriorityMgr_Free (pme);
   pme->m_enResStateCamera = RES_STATE_NORES ;
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
   return (nRet);
   #endif
}

/*==================================================================

==================================================================*/
void OEMCamera_ReleaseFrame(OEMINSTANCE h)
{
   // Release the frame buffer after the frame is delivered to the user.
   camera_release_frame();
}

/*===============================================================================

                     LOCAL FUNCTIONS

===============================================================================*/
static boolean OEMCamera_FilePath(OEMCamera * pme, void * pData)
{
   const char *cpszFile = (const char *)pData;
   char *      pszFS = 0;
   int         nLen;
   int         bRet = FALSE;

   if (!cpszFile)
      goto Done;

   FREEIF(pme->m_pszFile);

   // Get resolved fs:/ path length
   if (SUCCESS != AEE_ResolvePath(cpszFile, NULL, &nLen))
      goto Done;

   pszFS = MALLOC(nLen);
   if (!pszFS)
      goto Done;

   // Resolve path to fs:/
   if (SUCCESS != AEE_ResolvePath(cpszFile, pszFS, &nLen))
      goto Done;

   // Check for access
   if (SUCCESS != AEE_CheckPathAccess(pszFS, AEEFP_CREATE,0))
      goto Done;

   /* measure native path */
   if (SUCCESS != OEMFS_GetNativePath(pszFS,0, &nLen))
      goto Done;

   pme->m_pszFile = MALLOC(nLen);
   if (!pme->m_pszFile)
      goto Done;

   /* get native path */
   if (SUCCESS != OEMFS_GetNativePath(pszFS,pme->m_pszFile,&nLen))
      goto Done;

   bRet = TRUE;

Done:
   FREEIF(pszFS);
   return bRet;
}

/*==================================================================

==================================================================*/
static void OEMCamera_InitCameraRsp(OEMCamera * pme, int16 nCmd)
{
   CameraRsp * pList;
   int         nLen;
   int         i;

   switch (nCmd)
   {
      case CAM_CMD_START:
         pList = pme->m_rspStart;
         nLen = OEMCAMERA_RSP_START_MAX;
         break;

      case CAM_CMD_SETPARM:
         pList = pme->m_rspCommand;
         nLen = OEMCAMERA_RSP_COMMAND_MAX;
         break;

      case CAM_CMD_ENCODESNAPSHOT:
         pList = pme->m_rspEncodeSnapshot;
         nLen = OEMCAMERA_RSP_ENCODESNAPSHOT_MAX;
         break;

#if defined(FEATURE_BREWAPI_SPECIFIC)
      case CAM_CMD_OVERLAY_IMAGE:
         pList = pme->m_pngCommand;
         nLen = OEMCAMERA_RSP_COMMAND_MAX;
         break;
#endif // defined(FEATURE_BREWAPI_SPECIFIC)

      default:
         pList = NULL;
         nLen = 0;
   }

   for (i = 0; i < nLen; i++)
   {
      pList[i].camNotify.nCmd = nCmd;
      pList[i].camNotify.pCam = NULL;
      pList[i].bInUse = FALSE;
      pList[i].pOwner = pme;
   }
}

/*==================================================================
   For CAM_CMD_START, alloc all cbs
==================================================================*/
static CameraRsp * OEMCamera_AllocStartCameraRsp(OEMCamera * pme, int16 nSubCmd)
{
   CameraRsp * pcb = NULL;

   OEMCAMERA_ENTER_CRITICAL_SECTION()

   CameraRsp * pList = pme->m_rspStart;
   int         nLen = OEMCAMERA_RSP_START_MAX;
   int         i;

   if (!pList[0].hObject)
   {
      // Just register the first rsp and use it in all Start() related API...
      if (AEEObjectMgr_Register(&pList[0], &pList[0].hObject))
         goto Done;

      for (i = 0; i < nLen; i++)
      {
         pList[i].bInUse = FALSE;
         pList[i].camNotify.nSubCmd = nSubCmd;
      }
   }

   pcb = &pList[0];

Done:
   OEMCAMERA_LEAVE_CRITICAL_SECTION()

   return pcb;
}

/*==================================================================
   For CAM_CMD_START, free all cbs
==================================================================*/
static void OEMCamera_FreeStartCameraRsp(OEMCamera * pme)
{
   CameraRsp * pList = pme->m_rspStart;
   int         nLen = OEMCAMERA_RSP_START_MAX;
   int         i;

   for (i = 0; i < nLen; i++)
      OEMCamera_FreeCameraRsp(&pList[i]);
}

/*==================================================================

==================================================================*/
static CameraRsp * OEMCamera_GetStartCameraRsp(OEMCamera * pme)
{
   CameraRsp * pcb;

   OEMCAMERA_ENTER_CRITICAL_SECTION()

   CameraRsp * pList = pme->m_rspStart;
   int         nLen = OEMCAMERA_RSP_START_MAX;
   int         i;

   for (i = 0; i < nLen && pList[i].bInUse; i++)
     ;

   if (i < nLen)
   {
      pList[i].bInUse = TRUE;
      pcb = &pList[i];
   }
   else
      pcb = NULL;

   OEMCAMERA_LEAVE_CRITICAL_SECTION()

   return pcb;
}

/*==================================================================
   Alloc and Get
==================================================================*/
CameraRsp * OEMCamera_AllocCmdCameraRsp(OEMCamera * pme, int16 nCmd, int16 nSubCmd)
{
   CameraRsp * pcb = NULL;

   OEMCAMERA_ENTER_CRITICAL_SECTION()

   CameraRsp * pList = pme->m_rspCommand;
   int         nLen;
   int         i;

   switch (nCmd)
   {
      case CAM_CMD_SETPARM:
         nLen = OEMCAMERA_RSP_COMMAND_MAX;
         pList = pme->m_rspCommand;
         break;

      case CAM_CMD_ENCODESNAPSHOT:
         nLen = OEMCAMERA_RSP_ENCODESNAPSHOT_MAX;
         pList = pme->m_rspEncodeSnapshot;
         break;

#if defined(FEATURE_BREWAPI_SPECIFIC)
      case CAM_CMD_OVERLAY_IMAGE:
         nLen = OEMCAMERA_RSP_COMMAND_MAX;
         pList = pme->m_pngCommand;
         break;
#endif // defined(FEATURE_BREWAPI_SPECIFIC)

      default:
         goto Done;
   }

   for (i = 0; i < nLen; i++)
   {
      if (!pList[i].bInUse) {
         break;
      }
   }

   if (i < nLen)
   {
      if (AEEObjectMgr_Register(&pList[i], &pList[i].hObject))
         goto Done;

      pList[i].bInUse = TRUE;
      pList[i].camNotify.nSubCmd = nSubCmd;
      pcb = &pList[i];
   }

Done:
   OEMCAMERA_LEAVE_CRITICAL_SECTION()

   return pcb;
}

/*==================================================================

==================================================================*/
static void OEMCamera_FreeCameraRsp(CameraRsp * pRsp)
{
   if (!pRsp)
     return;

   OEMCAMERA_ENTER_CRITICAL_SECTION()
//   CALLBACK_Cancel((AEECallback *)pRsp);
   if (pRsp->hObject)
   {
     AEEObjectMgr_Unregister(pRsp->hObject);
     pRsp->hObject = 0;
   }
   pRsp->bInUse = FALSE;
   OEMCAMERA_LEAVE_CRITICAL_SECTION()
}

/*==================================================================

==================================================================*/
static int OEMCamera_Preview(OEMCamera * pme, CameraRsp * pRsp)
{
#ifdef FEATURE_DSP
    int ncheck;
	ext_camera_para_struct ext_cam_para = {0};
    AIT701_cam_preview(&ext_cam_para);	
    ncheck=AIT701_cam_check_frame(0x01);
    #if 0
    if(ncheck==0)
    {
    MSG_FATAL("OEMCamera_Preview-----ncheck=%d",ncheck,0,0);
	
    }
    else
    {
        MSG_FATAL("OEMCamera_Preview-----ncheck=%d",ncheck,0,0);

    }  
    #endif
    return SUCCESS;

#else
   int      nRet;
   nRet = (int)camera_start_preview(OEMCamera_CameraLayerCB,
                                    (void *)pRsp->hObject);
   if(nRet == CAMERA_SUCCESS)
   {
      pme->m_nNextMode = CAM_MODE_PREVIEW;
   }  
   return OEMCamera_AEEError(nRet);
   #endif
}

#ifndef FEATURE_CAMERA_V2
/*==================================================================

==================================================================*/
static int OEMCamera_DoFocus(OEMCamera * pme, int32 mode, int32 step)
{
  CameraRsp * pRsp;
  int         nRet = CAMERA_INVALID_PARM;

  pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_SETPARM, CAM_PARM_FOCUS);
  if (!pRsp)
    nRet = EITEMBUSY;
  else
  {
   if (mode == CAM_FOCUS_AUTO)
   {
     nRet = (int)camera_start_focus(CAMERA_AUTO_FOCUS, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
   }
   else if (mode == CAM_FOCUS_MANUAL)
   {
     (void)camera_set_parm(CAMERA_PARM_FOCUS_STEP, step, NULL, NULL);
     nRet = (int)camera_start_focus(CAMERA_MANUAL_FOCUS, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
   }

   if (nRet)
   {
     OEMCamera_FreeCameraRsp(pRsp);
     nRet = OEMCamera_AEEError(nRet);
   }
   else
     nRet = CAM_PENDING;
  }

  return nRet;
}

/*==================================================================

==================================================================*/
#ifndef FEATURE_CAMERA7500
static int OEMCamera_GetFocusRect(int32 *pRc, int32 *pInfo)
{
  int         nRet = EUNSUPPORTED;

#ifdef FEATURE_CAMERA_V7
  camera_focus_window_type p;
  CameraFocusRectInfo      frInfo;

  if (!pRc)
  {
    return EBADPARM;
  }

  ZEROAT(&p);

  nRet = (int)camera_get_focus_window(&p);
  if (nRet == CAMERA_SUCCESS)
  {

    SETAEERECT((AEERect *)pRc, p.x, p.y, p.dx, p.dy);

    //More info
    frInfo.dwStructSize = (uint32)sizeof(CameraFocusRectInfo);
    frInfo.dxStep = (int32)p.dx_step_size;
    frInfo.dyStep = (int32)p.dy_step_size;

    //rectFocusRegion defined the min and max values
    //for the focus rect to move
    frInfo.rectFocusRegion.x = p.min_x;
    frInfo.rectFocusRegion.dx = p.max_x;
    frInfo.rectFocusRegion.y = p.min_y;
    frInfo.rectFocusRegion.dy = p.max_y;

    if (pInfo)
      MEMCPY(pInfo, &frInfo, sizeof(CameraFocusRectInfo));
  }

  //Convert camera return value to OEMCamera return value.
  nRet = OEMCamera_AEEError(nRet);

#endif /* FEATURE_CAMERA_V7 */
  return nRet;
}
#endif //ifndef FEATURE_CAMERA7500
#ifdef FEATURE_CAMERA7500
int OEMCamera_GetMultipleFocusRect(int32* pRc, AEERect **pInfo)
{
  int         nRet = EUNSUPPORTED;
  int         numWindows = 0;

  camera_focus_window_rectangles_type p;
  static AEERect multiple_focus_windows[3];

  if (!pInfo)
  {
    return EBADPARM;
  }

  ZEROAT(&p);

  nRet = (int)camera_get_focus_window_rectangles(&p);
  if (nRet == CAMERA_SUCCESS)
  {
    *pRc = p.focus_window_count;

    if (p.focus_window_count)
    {
      while (numWindows < p.focus_window_count)
      {
        multiple_focus_windows[numWindows].x  = p.windows_list[numWindows].x_upper_left;
        multiple_focus_windows[numWindows].y  = p.windows_list[numWindows].y_upper_left;
        multiple_focus_windows[numWindows].dx = p.windows_list[numWindows].width;
        multiple_focus_windows[numWindows].dy = p.windows_list[numWindows].height;
        numWindows++;
      }
      *pInfo = multiple_focus_windows;
    }
    else
    {
      *pInfo = NULL;
    }
  }

  //Convert camera return value to OEMCamera return value.
  nRet = OEMCamera_AEEError(nRet);

  return nRet;
}
#endif

/*==================================================================

==================================================================*/
#ifndef FEATURE_CAMERA7500
static int OEMCamera_SetFocusRect(AEERect rc)
{
  int         nRet = EUNSUPPORTED;

#ifdef FEATURE_CAMERA_V7
  camera_focus_window_type  p;

  ZEROAT(&p);
  nRet = (int)camera_get_focus_window(&p);
  if (CAMERA_SUCCESS == nRet)
  {
     //New rect
     p.x =            rc.x;
     p.dx =           rc.dx;
     p.y =            rc.y;
     p.dy =           rc.dy;

     nRet = camera_set_focus_window(&p);
  }

  //Convert camera return value to OEMCamera return value.
  nRet = OEMCamera_AEEError(nRet);
#endif /* FEATURE_CAMERA_V7 */
  return nRet;
}
#endif //ifndef FEATURE_CAMERA7500
#endif /* nFEATURE_CAMERA_V2 */
/*==================================================================

==================================================================*/
static int OEMCamera_RecordSnapshot(OEMCamera * pme, CameraRsp * pRsp)
{
#ifdef FEATURE_DSP
	pme->m_nFileSize = AIT701_Test_capture(g_fbuffer);
	return SUCCESS;

#else
   int                  nRet;
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
   camera_encode_properties_type   p;
#endif // FEATURE_CAMERA_ENCODE_PROPERTIES
   MSG_FATAL("OEMCamera_RecordSnapshot",0,0,0);
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
   ICamera_SetSnapshotProperties(pme, &p);
   camera_set_encode_properties(&p);
#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
   nRet = (int)camera_take_picture(OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
#else /* FEATURE_CAMERA_ENCODE_PROPERTIES */
#ifdef FEATURE_CAMERA_V7
   //This update supports the new encoder. It is only applied to 6550 now.
   switch (pme->m_clsVideo)
   {
     case CAM_ENCODE_RAW:
       nRet = (int)camera_take_picture(OEMCamera_CameraLayerCB, (void *)pRsp->hObject, CAMERA_RAW_SNAPSHOT);
       break;
     case AEECLSID_JPEG:
     case AEECLSID_PNG:
#if defined(FEATURE_CAMERA7500) || defined (FEATURE_CAMERA_ENCODE_YCBCR)
     case CAM_ENCODE_YCBCR:
#endif //FEATURE_CAMERA7500
     default:
       nRet = (int)camera_take_picture(OEMCamera_CameraLayerCB, (void *)pRsp->hObject, CAMERA_SNAPSHOT);
       break;
   }
#else
   nRet = (int)camera_take_picture(OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
#endif //FEATURE_CAMERA_V7
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */
   if(nRet == CAMERA_SUCCESS)
   {
      pme->m_nNextMode = CAM_MODE_SNAPSHOT;
   }
   return OEMCamera_AEEError(nRet);
   #endif
}

/*==================================================================

==================================================================*/
static int OEMCamera_RecordMovie(OEMCamera * pme, CameraRsp * pRsp)
{
#if !defined(FEATURE_VIDEO_ENCODE)
   return EUNSUPPORTED;
#else
   int                           nRet;
   video_handle_type             h;
   video_fmt_stream_audio_type   eAudioType = VIDEO_FMT_STREAM_AUDIO_INVALID;
   int32                         wFileFormat = CAM_FILE_BRAND_3G2;
   video_fmt_stream_video_type   nVideoFormat = VIDEO_FMT_STREAM_VIDEO_MPEG4;
   boolean nSh = FALSE;
   video_profile_type video_profile = VIDEO_ENC_LOW_RATE;

   nRet = OEMCamera_SetMovieDestination(pme, &h);
   if (SUCCESS != nRet)
      return nRet;

   nRet = (int)OEMCamera_AudioStreamType(pme->m_clsAudio, pme->m_dwSubAudio);
   if (nRet != -1)
     eAudioType = (video_fmt_stream_audio_type)nRet;

   if ((eAudioType == VIDEO_FMT_STREAM_AUDIO_INVALID) || (-1 == nRet))
      return EBADPARM;

   // Note:
   // (1) MP4 format is hard-coded. When more video container formats
   //     are supported, the container type must be detected using the MIME
   //     string specified via ICAMERA_SetMediaData().
   // (2) MP4 stream type is hard-coded. When more video container formats
   //     are supported, then the container type must be derived using the
   //     pme->m_clsVideo and pme->m_dwSubVideo.
   // (3) pme->m_nQuality is cast to video_profile_type as camera_quality_type matches
   //     video_profile_type.



   if(pme->m_pszMIME)
   { if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_MPG4) ==0)
      {wFileFormat = CAM_FILE_BRAND_MP4;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_AMC) ==0)
      {wFileFormat = CAM_FILE_BRAND_AMC;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_3GP) ==0)
      {wFileFormat = CAM_FILE_BRAND_3GP;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_3G2) ==0)
      {wFileFormat = CAM_FILE_BRAND_3G2;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_3G2_FRAG) ==0)
      {wFileFormat = CAM_FILE_BRAND_3G2_FRAG;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_K3G) ==0)
      {wFileFormat = CAM_FILE_BRAND_K3G;
      }
   else if(STRCMP(pme->m_pszMIME, CAM_FILE_FORMAT_SKM) ==0)
      {wFileFormat = CAM_FILE_BRAND_SKM;
      }
   }

 /* Temporary solution to distinguish between video formats */
   if (pme->m_clsVideo == AEECLSID_MEDIAMPEG4)
   {
     if(pme->m_dwSubVideo == 1 || pme->m_dwSubVideo == 2)
       nVideoFormat = VIDEO_FMT_STREAM_VIDEO_H263;
#ifdef FEATURE_H264_ENCODER
     else if(pme->m_dwSubVideo == 3)
       nVideoFormat = VIDEO_FMT_STREAM_VIDEO_H264;
#endif /* FEATURE_H264_ENCODER */
     else if(pme->m_dwSubVideo == 4)
       nVideoFormat = VIDEO_FMT_STREAM_VIDEO_NONE;
     else
       nVideoFormat = VIDEO_FMT_STREAM_VIDEO_MPEG4;
   }


   /* Ensure ShortHeader settings and video format are valid */
   if((nVideoFormat != VIDEO_FMT_STREAM_VIDEO_H263) &&
      (pme->m_wShortHeader != CAM_SHORT_HEADER_DISABLED))
   {
//     MSG_ERROR("Invalid combintaion: video format: %d  SH: %d",
//               nVideoFormat,pme->m_wShortHeader,0);
   }

  /* Set the nSh flag only - Note: Short Header != H263 video format */
   switch(wFileFormat)
   {
      case CAM_FILE_BRAND_MP4://MPEG4
      case CAM_FILE_BRAND_AMC://AMC
      case CAM_FILE_BRAND_K3G: //K3G
         switch(pme->m_wShortHeader)
         {
            case CAM_SHORT_HEADER_DISABLED:
               nSh = FALSE;
               break;
            case CAM_SHORT_HEADER_ENABLED:
               nSh = TRUE;
               break;
         }
       break;
    case CAM_FILE_BRAND_3GP://3GP
      switch(pme->m_wShortHeader)
      {
         /* Short Header not valid for 3GP */
         case CAM_SHORT_HEADER_DISABLED:
         case CAM_SHORT_HEADER_ENABLED:
           nSh = FALSE;
           break;
      }
      break;
    case CAM_FILE_BRAND_3G2_FRAG:
    case CAM_FILE_BRAND_3G2://3G2
      switch(pme->m_wShortHeader)
      {
         case CAM_SHORT_HEADER_DISABLED:
         case CAM_SHORT_HEADER_ENABLED:
           nSh = FALSE;
           break;
         case CAM_SHORT_HEADER_TEST:
           nSh = TRUE;
           break;
       }
        break;
   }
   /* cast the quality to correct video profile */
   switch(pme->m_nQuality)
   {
      case OEMCAMCORDER_QUALITY_LOW:
        video_profile = VIDEO_ENC_LOW_RATE;
        break;
      case OEMCAMCORDER_QUALITY_MED:
        video_profile = VIDEO_ENC_MED_RATE;
        break;
      case OEMCAMCORDER_QUALITY_HIGH:
        video_profile = VIDEO_ENC_HIGH_RATE;
        break;
#ifdef FEATURE_CAMERA_V770
     case OEMCAMCORDER_QUALITY_USER:
        video_profile = VIDEO_ENC_USER_DEFINED;
        break;
#endif
     case OEMCAMCORDER_QUALITY_MAX:
        video_profile = VIDEO_ENC_MAX_RATE;
        break;
     case OEMCAMCORDER_QUALITY_DYNAMIC:
        video_profile = VIDEO_ENC_DYNAMIC_RATE;
        break;

      default:
        video_profile = VIDEO_ENC_LOW_RATE;
        break;

   }

   nRet = (int)camera_start_record(VIDEO_FMT_MP4,
                                   nVideoFormat,
                                   (video_fmt_brand_type)wFileFormat,
                                   nSh,
                                   eAudioType,
#if defined(FEATURE_CAMERA_V770) && !defined(FEATURE_CAMERA7500)
                                   pme->m_bAudioPostCardRecord,
#endif //defined(FEATURE_CAMERA_V770) && !defined(FEATURE_CAMERA7500)
#ifdef FEATURE_VIDEO_MULTI_UUID
                                   &(pme->m_uuidInfo),
#else /* FEATURE_VIDEO_MULTI_UUID */
                                   pme->m_pCpgd,
                                   pme->m_nszpCpgd,
                                   pme->m_pUuid_cpgd,
#endif /* FEATURE_VIDEO_MULTI_UUID */
                                   video_profile,
#if defined(FEATURE_CAMERA_V7)
                                   (uint32)pme->m_nBitrate,
#endif
                                   &h,
                                   OEMCamera_CameraLayerCB,
                                   (void *)pRsp->hObject
                                  );
   if(nRet == CAMERA_SUCCESS)
   {
      pme->m_nNextMode = CAM_MODE_MOVIE;
   }
   return OEMCamera_AEEError(nRet);
#endif // defined(FEATURE_VIDEO_ENCODE)
}

#ifdef FEATURE_CAMERA_V7
/*==================================================================

==================================================================*/
static void OEMCamera_ConvertGetEV(OEMCamera * pme,
                                   AEEParmInfo *pi,
                                   AEEFixedPointParm * p1,
                                   AEEFixedPointParmInfo * pInfo)
{
  int16 num;

  if (!pi)
  {
    return;
  }

  //camera API returns all values with numerator in upper 16 bits
  //and denominator in lower 16 bits for EV. Denominators are all
  //the same. Save it in pme->m_nEVFactor for SetParm since BREW
  //interface has no way to pass it up to the UI
  pme->m_nEVFactor = (uint16)(pi->nStep & 0xFFFF);
  if (pme->m_nEVFactor == 0)
    pme->m_nEVFactor = 1;

  //Pass the scaled value(numerator) only. In this way,
  //UI step up/down won't affect the precision.
  //Multiple all value by 1000 to comply with the definitions
  //of AEEFixedPointParmInfo
  num = (int16)((pi->nCurrent >> 16) & 0xFFFF);
  if(p1)
  {
    ZEROAT(p1);
    p1->nCurrent = (int32)(num * 1000);
  }

  if (pInfo)
  {
    ZEROAT(pInfo);
    pInfo->nCurrent = (int32)(num * 1000);

    num = (int16)((pi->nMin >> 16) & 0xFFFF);
    pInfo->nMin = (int32)(num * 1000);

    num = (int16)((pi->nMax >> 16) & 0xFFFF);
    pInfo->nMax = (int32)(num * 1000);

    num = (int16)((pi->nStep >> 16) & 0xFFFF);
    pInfo->nStep = (int32)(num * 1000);

    num = (int16)((pi->nDefault >> 16) & 0xFFFF);
    pInfo->nDefault = (int32)(num * 1000);
  }
}

/*==================================================================

==================================================================*/
static int32 OEMCamera_ConvertSetEV(OEMCamera * pme, AEEFixedPointParm * pParm)
{
  int32 nOut = 0;

  //get the denominator back
  if (pme->m_nEVFactor == 0)
    pme->m_nEVFactor = 1;

  //Convert the value back to the format of EV in camera for camera API.
  //(Upper 16 bits is Numerator. Lower 16 bits is Denominator.)
  nOut = (int32)pme->m_nEVFactor;
  nOut |= ((pParm->nCurrent / 1000) << 16);
  return nOut;
}

/*==================================================================

==================================================================*/
static void OEMCamera_ConvertGetFPS(AEEParmInfo *pi,
                                    AEEFixedPointMinMax * pMinMax)
{
  int16 num;

  if (!pi || !pMinMax)
  {
    return;
  }

  ZEROAT(pMinMax);

  //pi values from camera_get_parm has the format of dwFPS:
  //Lower 16 bits is Numerator. Upper 16 bits is Denominator.
  //For preview FPS, the denominator is fix and defined as
  //CAMERA_FPS_DENOMINATOR
  num = (int16)(pi->nMin & 0xFFFF);
  pMinMax->nMin = (int32)((num * 1000) / CAMERA_FPS_DENOMINATOR);

  num = (int16)(pi->nMax & 0xFFFF);
  pMinMax->nMax = (int32)((num * 1000) / CAMERA_FPS_DENOMINATOR);
}

/*==================================================================

==================================================================*/
static int OEMCamera_SetPreviewFPS(AEEFixedPointMinMax * pMinMax,
                                   CameraRsp * pRsp)
{
  int32 nMax, nMin;

  if (!pMinMax)
    return CAMERA_INVALID_PARM;
  if (pMinMax->nMin <= 0 || pMinMax->nMax <= 0)
    return CAMERA_INVALID_PARM;
  //The denominator of preview FPS is fix and defined as
  //CAMERA_FPS_DENOMINATOR in camera.h.
  //Preview FPS only care about the min. and max. option.
  //Convert the values back to dwFPS format for camrea API.
  nMin = (int32)(CAMERA_FPS_DENOMINATOR << 16);
  nMin |= (int32)((pMinMax->nMin * CAMERA_FPS_DENOMINATOR) / 1000);

  nMax = (int32)(CAMERA_FPS_DENOMINATOR << 16);
  nMax |= (int32)((pMinMax->nMax * CAMERA_FPS_DENOMINATOR) / 1000);

  return (int)camera_set_parm_2(CAMERA_PARM_PREVIEW_FPS,
                                nMax,
                                nMin,
                                OEMCamera_CameraLayerCB,
                                (void *)pRsp->hObject);
}

#endif //FEATURE_CAMERA_V7
#ifdef FEATURE_CAMERA_V770
/*==================================================================

==================================================================*/
static int OEMCamera_ConverISO(int32 * pVal, boolean bSet)
{
  int nRet = SUCCESS;
  if (bSet)
  {
    switch (*pVal)
    {
    case 0:
      *pVal = CAMERA_ISO_AUTO;
      break;
#ifdef FEATURE_CAMERA_HJR
    case 1:
      *pVal = CAMERA_ISO_DEBLUR;
      break;
#endif //FEATURE_CAMERA_HJR
    case 100:
      *pVal = CAMERA_ISO_100;
      break;
    case 200:
      *pVal = CAMERA_ISO_200;
      break;
    case 400:
      *pVal = CAMERA_ISO_400;
      break;
    case 800:
      *pVal = CAMERA_ISO_800;
      break;
//    case 1600:
//      *pVal = CAMERA_ISO_1600;
//      break;
    default:
      nRet = EBADPARM;
      break;
    }
  }
  else
  {
    switch (*pVal)
    {
    case CAMERA_ISO_AUTO:
      *pVal = 0;
      break;
#ifdef FEATURE_CAMERA_HJR
    case CAMERA_ISO_DEBLUR:
      *pVal = 1;
      break;
#endif //FEATURE_CAMERA_HJR
    case CAMERA_ISO_100:
      *pVal = 100;
      break;
    case CAMERA_ISO_200:
      *pVal = 200;
      break;
    case CAMERA_ISO_400:
      *pVal = 400;
      break;
    case CAMERA_ISO_800:
      *pVal = 800;
      break;
//    case CAMERA_ISO_1600:
//      *pVal = 1600;
//      break;
    default:
      nRet = EUNSUPPORTED;
      break;
    }
  }

  return nRet;
}

#endif //FEATURE_CAMERA_V770

/*==================================================================

==================================================================*/
static int OEMCamera_GetSupportStatus(OEMCamera *pme, int16 itemID)
{
  int nRet = EUNSUPPORTED;

  switch (itemID)
  {
  case CAM_PARM_MEDIA_DATA:
  case CAM_PARM_VIDEO_ENCODE:
  case CAM_PARM_AUDIO_ENCODE:
  case CAM_PARM_SIZE:
  case CAM_PARM_DISPLAY_SIZE:
  case CAM_PARM_DEFER_ENCODE:
  case CAM_PARM_MODE:
  case CAM_PARM_OVERLAY:
  case CAM_PARM_GPSINFO:
  case CAM_PARM_EXIF_IFDTAG:
  case CAM_PARM_PREVIEW_TYPE:
  case CAM_PARM_THUMBNAIL_INFO:
  case CAM_PARM_SENSOR_INFO:
  case CAM_PARM_QUALITY:
  case CAM_PARM_FPS:
  case CAM_PARM_CONTRAST:
  case CAM_PARM_SHARPNESS:
#ifdef FEATURE_CAMERA7500
  case CAM_PARM_HUE:
  case CAM_PARM_SATURATION:
  case CAM_PARM_AUTO_FOCUS_REGION_LIST:
#endif //FEATURE_CAMERA7500
  case CAM_PARM_ZOOM:
  case CAM_PARM_ROTATE_PREVIEW:
  case CAM_PARM_ROTATE_ENCODE:
  case CAM_PARM_EXPOSURE:
  case CAM_PARM_WHITE_BALANCE:
  case CAM_PARM_EFFECT:
  case CAM_PARM_FLASH:
  case CAM_PARM_REFLECT:
  case CAM_PARM_FILE_SIZE:
  case CAM_PARM_ANTIBANDING:
  case CAM_PARM_NIGHTSHOT_MODE:
  case CAM_PARM_IS_SUPPORT:
  case CAM_PARM_PIXEL_COUNT:
  case CAM_PARM_VIDEO_ENCODE_LIST: 
  case CAM_PARM_AUDIO_ENCODE_LIST:
  case CAM_PARM_DISPLAY_SIZE_LIST:
  case CAM_PARM_SIZE_LIST:
  case CAM_PARM_IS_MOVIE:
  case  CAM_PARM_SPACE_LIMIT:
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  case CAM_PARM_USER_ATOM:
#endif
#ifdef FEATURE_VIDEO_ENCODE
  case CAM_PARM_FRAMES_DROPPED:
#endif
#ifdef FEATURE_CAMERA_PANORAMIC
  case CAM_PARM_PANORAMIC:
  case CAM_PARM_STITCH:
#endif
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
  case CAM_PARM_TRANSITION:
#endif
  //Feature defined items
#ifdef FEATURE_CAMERA_BRIGHTNESS
  case CAM_PARM_BRIGHTNESS:
#endif //FEATURE_CAMERA_BRIGHTNESS
#ifdef FEATURE_STROBE_FLASH
  case CAM_PARM_RED_EYE_REDUCTION:
#endif // FEATURE_STROBE_FLASH
#ifndef FEATURE_CAMERA_V2
  case CAM_PARM_FOCUS:
  case CAM_PARM_FOCUS_RECT:
#endif //FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA_V7
  case CAM_PARM_BITRATE:
  case CAM_PARM_EXPOSURE_COMP:
  case CAM_PARM_PREVIEW_FPS:
#endif //FEATURE_CAMERA_V7
#ifdef FEATURE_VIDEO_ENCODE_FADING
  case CAM_PARM_FADE:
#endif //FEATURE_VIDEO_ENCODE_FADING
#if defined(FEATURE_CAMERA_V7) || defined(FEATURE_CAMERA_V2)
  case CAM_PARM_EXPOSURE_METERING:
#endif //FEATURE_CAMERA_V7 || FEATURE_CAMERA_V2
#ifdef FEATURE_CAMERA_V770
  case CAM_PARM_ISO:
  case CAM_PARM_AF_INFO:
#endif //FEATURE_CAMERA_V770
#ifdef FEATURE_CAMERA_V790
  case CAM_PARM_FPS_LIST:
#endif //FEATURE_CAMERA_V790
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif //FEATURE_CAMERA_BESTSHOT_MODE
#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  case CAM_PARM_LCD_DIRECT_ACCESS:
  case CAM_PARM_LCD_DIRECT_ACCESS_EX:
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
  case CAM_PARM_SHUTTER_SOUND:
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    nRet = SUCCESS;
    break;
  //Conditional support item.
#ifdef FEATURE_CAMERA_V770
  case CAM_PARM_LUMA_ADAPTATION:
  case CAM_PARM_FLASH_CTL:
  case CAM_PARM_FRAME_TIMESTAMP:
  case CAM_PARM_SHUTTER_SPEED:
  case CAM_PARM_APERTURE:
  case CAM_PARM_SHUTTER_CALLBACK:
  case CAM_PARM_DIGITAL_ZOOM_ENABLE:
    {
      int32 nVal = 0;
      nRet = OEMCamera_GetParm((OEMINSTANCE *)pme, itemID, &nVal, NULL);
    }
    break;
#endif //FEATURE_CAMERA_V770
  default:
    break;
  }
  return nRet;
}

/*==================================================================
   Map camera error code to AEE error code
==================================================================*/
int OEMCamera_AEEError(int nRet)
{
   if (nRet < 0)
      return EFAILED;

   switch ((camera_ret_code_type)nRet)
   {
      case CAMERA_SUCCESS:       nRet = SUCCESS;         break;
      case CAMERA_INVALID_STATE: nRet = EBADSTATE;       break;
      case CAMERA_INVALID_PARM:  nRet = EBADPARM;        break;
      case CAMERA_INVALID_FORMAT:nRet = EBADPARM;        break;
      case CAMERA_NO_SENSOR:     nRet = EFAILED;         break;
      case CAMERA_NO_MEMORY:     nRet = ENOMEMORY;       break;
      case CAMERA_NOT_SUPPORTED: nRet = EUNSUPPORTED;    break;
      case CAMERA_FAILED:        nRet = EFAILED;         break;
      default:                   nRet = EFAILED;         break;
   }

   return nRet;
}

/*==================================================================

==================================================================*/
int OEMCamera_SetSnapshotDestination(OEMCamera * pme, camera_handle_type * ph)
{
   AEEMediaData *       pmd = &pme->m_md;
   int                  nRet = SUCCESS;
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
   camera_encode_type   eFormat;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */

   if (!pmd || !pmd->pData)
      return EFAILED;

#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
   switch (pme->m_clsVideo)
   {
      case AEECLSID_JPEG:
         eFormat = CAMERA_JPEG;
         break;

      case AEECLSID_PNG:
         eFormat = CAMERA_PNG;
         break;

      case CAM_ENCODE_RAW:
         eFormat = CAMERA_RAW;
         break;

#if defined(FEATURE_CAMERA7500) || defined (FEATURE_CAMERA_ENCODE_YCBCR)
      case CAM_ENCODE_YCBCR:
         eFormat = CAMERA_YCBCR_ENCODE;
         break;
#endif

      default:
         eFormat  = CAMERA_JPEG;
         break;
   }
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->device = ph->efs.device = CAMERA_DEVICE_EFS;
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
      ph->efs.quality = pme->m_nQuality;
      ph->efs.format = eFormat;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
      if (pmd->pData)
         STRLCPY(ph->efs.filename, pme->m_pszFile, sizeof(ph->efs.filename));
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      ph->device = ph->mem.device = CAMERA_DEVICE_MEM;
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
      ph->mem.quality = pme->m_nQuality;
      ph->mem.format = eFormat;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
      if (pmd->pData)
      {
         ph->mem.buffer = (byte *)pmd->pData;
         ph->mem.length = pmd->dwSize;
      }
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData >= MMD_ISOURCE)
   {
      // Perform streaming....
      nRet = EUNSUPPORTED;
   }

   return nRet;
}

#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
void ICamera_SetSnapshotProperties(OEMCamera * pme, camera_encode_properties_type * p)
{
   switch (pme->m_clsVideo)
   {
      case AEECLSID_JPEG:
         p->format = CAMERA_JPEG;
         break;

      case AEECLSID_PNG:
         p->format = CAMERA_PNG;
         break;

      case CAM_ENCODE_RAW:
         p->format = CAMERA_RAW;
         break;

#if defined (FEATURE_CAMERA7500) || defined (FEATURE_CAMERA_ENCODE_YCBCR)
      case CAM_ENCODE_YCBCR:
         p->format = CAMERA_YCBCR_ENCODE;
         break;
#endif
      default:
         p->format  = CAMERA_JPEG;
         break;
   }
   p->quality = pme->m_nQuality;
   p->file_size = pme->m_nFileSize;
}
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

/*==================================================================

==================================================================*/
#if defined(FEATURE_VIDEO_ENCODE)
static int OEMCamera_SetMovieDestination(OEMCamera * pme, video_handle_type * ph)
{
   AEEMediaData * pmd = &pme->m_md;
   int            nRet = SUCCESS;

   if (!pmd || !pmd->pData)
      return EFAILED;

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->method = ph->efs.method = VIDEO_METHOD_EFS;
      if (pmd->pData)
         STRLCPY((char *)ph->efs.filename, pme->m_pszFile, sizeof(ph->efs.filename));
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      ph->method = ph->efs.method = VIDEO_METHOD_MEM;
      if (pmd->pData)
      {
         ph->mem.buf = (uint8 *)pmd->pData;
         ph->mem.len = pmd->dwSize;
      }
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData >= MMD_ISOURCE)
   {
      // Perform streaming....
      nRet = EUNSUPPORTED;
   }

   return nRet;
}
#endif // defined(FEATURE_VIDEO_ENCODE)

/*==================================================================
   CameraLayer callback called in CameraLayer context
==================================================================*/
void OEMCamera_CameraLayerCB(camera_cb_type cb, const void *client_data, camera_func_type func, int32 parm4)
{
   PFNCAMERANOTIFY   pfnAEE = NULL;
   void *          pUserAEE = NULL;
   boolean     bEncodeFailed = FALSE;
   CameraRsp   rspCopy;
#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)  
#error code not present
#endif   
   OEMCAMERA_ENTER_CRITICAL_SECTION()

   CameraRsp * pRsp = (CameraRsp *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   OEMCamera * pme;
   int         nStatus = 0;
   uint32      dwData = SUCCESS;
  
    if (!pRsp || !pRsp->pOwner)
      goto CBExit;

   pme = pRsp->pOwner;
   //If callback comes after ICamera_Release, ignore it.
   if (pme->m_bRelease)
     goto CBExit;
   
   MEMSET(&rspCopy, (int)0, sizeof(rspCopy));
   MSG_FATAL("OEMCamera_CameraLayerCB %d %d %d",cb,func,parm4);
   // Common status handling...
   switch (cb)
   {
      case CAMERA_EXIT_CB_DSP_IDLE:    // Fall through...
      case CAMERA_EXIT_CB_DSP_ABORT:   // Fall through...
      case CAMERA_EXIT_CB_ABORT:       // Fall through...
      case CAMERA_EXIT_CB_ERROR:
         nStatus = CAM_STATUS_ABORT;
         switch(parm4)
         {
            case CAMERA_ERROR_NO_MEMORY:
            case CAMERA_ERROR_EFS_SPACE_EXHAUSTED:
               nStatus = CAM_STATUS_SPACE_ERROR;
               break;

            case CAMERA_ERROR_EFS_FILE_OPEN:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_ALREADY_OPEN;
               break;

            case CAMERA_ERROR_EFS_FILE_NOT_OPEN:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_OPEN;
               break;

            case CAMERA_ERROR_EFS_FILE_ALREADY_EXISTS:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_ALREADY_EXIST;
               break;

            case CAMERA_ERROR_EFS_NONEXISTENT_DIR:
            case CAMERA_ERROR_EFS_NONEXISTENT_FILE:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_NOT_EXIST;
               break;

            case CAMERA_ERROR_EFS_BAD_FILE_NAME:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_BAD_NAME;
               break;

            case CAMERA_ERROR_EFS_BAD_FILE_HANDLE:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_BAD_HANDLE;
               break;

            case CAMERA_ERROR_EFS_OTHER_ERROR:
            case CAMERA_ERROR_EFS_OPEN_TABLE_FULL:
               nStatus = CAM_STATUS_IO_ERROR;
               dwData = CAM_ERR_FS;
               break;
            default:
               nStatus = CAM_STATUS_ABORT;
               break;
         }

         if ((pRsp->camNotify.nCmd == CAM_CMD_START) ||
             (pRsp->camNotify.nCmd == CAM_CMD_ENCODESNAPSHOT))
         {
            MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
            OEMCamera_SetReadyMode(pme);
         }
         goto Resume;

      case CAMERA_EVT_CB_FRAME:
         //MSG_FATAL("CAMERA_EVT_CB_FRAME %d",bDirectMode,0,0);
         if (func == CAMERA_FUNC_START_PREVIEW
#if defined(FEATURE_VIDEO_ENCODE)
             || func == CAMERA_FUNC_START_RECORD
#endif /* FEATURE_VIDEO_ENCODE */
            )
         {
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
           if(bDirectMode)
           {
             DBGPRINTF("bDirectMode");
             /* if in Direct Access mode then blt out now */
             camera_blt_direct((camera_frame_type *)parm4);
           }
           else
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
#ifdef FEATURE_CAMERA_NOFULLSCREEN
           if(bDirectToLCD)
           {
             DBGPRINTF("camera_blt_ext %d %d %d %d",pme->m_cxOffset, pme->m_cyOffset, pme->m_sizeDisplay.cx, pme->m_sizeDisplay.cy);
             camera_blt_ext((camera_frame_type *)parm4, 0, 0, 0, pme->m_cxOffset, pme->m_cyOffset, pme->m_sizeDisplay.cx, pme->m_sizeDisplay.cy, CAMERA_TOLCD);
           }
           else
#endif
           {
             OEMCamera_SaveFrameInfo(pme, (camera_frame_type *)parm4);
             nStatus = CAM_STATUS_FRAME;
           }
         }
         goto Resume;
   }

   switch (func)
   {
      case CAMERA_FUNC_START: // Fall through...
      case CAMERA_FUNC_STOP:
         break;

      case CAMERA_FUNC_START_PREVIEW:
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               MSG_FATAL("CAM_MODE_PREVIEW",0,0,0);
               pme->m_nMode = CAM_MODE_PREVIEW;
               pme->m_nNextMode = CAM_MODE_READY;
               nStatus = CAM_STATUS_START;
               pme->m_bStateChanging = FALSE;
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_EPREVIEWFAIL;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

            case CAMERA_EXIT_CB_DONE:
               nStatus = CAM_STATUS_DONE;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;
         }
         break;
      }

      case CAMERA_FUNC_STOP_PREVIEW:
         break;

#ifndef FEATURE_CAMERA_V2
      case CAMERA_FUNC_START_FOCUS:
         switch (cb)
         {

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               break;

            case CAMERA_EXIT_CB_DONE:
               nStatus = CAM_STATUS_DONE;
               break;
         }
         break;
#endif //nFEATURE_CAMERA_V2

      case CAMERA_FUNC_TAKE_PICTURE:
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               MSG_FATAL("CAM_MODE_SNAPSHOT",0,0,0);
               pme->m_nMode = CAM_MODE_SNAPSHOT;
               nStatus = CAM_STATUS_START;
               pme->m_bStateChanging = FALSE;
               pme->m_nNextMode = CAM_MODE_READY;
               break;

            case CAMERA_EXIT_CB_DONE:
               nStatus = CAM_STATUS_DONE;
               dwData = pme->m_bDeferEncode;

               // Save the frame info.
               OEMCamera_SaveFrameInfo(pme, (camera_frame_type *)parm4);

               // If encoding is not deferred, then encode the snapshot right away.
               if (!pme->m_bDeferEncode)
               {
                  if (SUCCESS != OEMCamera_EncodeSnapshot(pme))
                     bEncodeFailed = TRUE;
               }
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_ESNAPSHOTFAIL;
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
               if(pme->m_boCameraStoppedDueToACMRelease == TRUE)
               {
                  pme->m_boCameraStoppedDueToACMRelease = FALSE;
               }
#endif // FEATURE_ACM || FEATURE_BMP_ACM
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;
         }
         break;
      }

#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

      case CAMERA_FUNC_ENCODE_PICTURE:
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               MSG_FATAL("CAM_MODE_SNAPSHOT",0,0,0);
               pme->m_nMode = CAM_MODE_SNAPSHOT;
               pme->m_bStateChanging = FALSE;
               pme->m_nNextMode = CAM_MODE_READY;
               break;

            case CAMERA_EXIT_CB_DONE:
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               nStatus = CAM_STATUS_DONE;
               dwData = parm4;
#if defined(FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)  
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
               break;

            case CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED:
               nStatus = CAM_STATUS_FILE_SIZE_EXCEEDED;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

            case CAMERA_EXIT_CB_FAILED:
            default:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_ESNAPSHOTFAIL;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

         }
         break;
      }

#if defined(FEATURE_VIDEO_ENCODE)
      case CAMERA_FUNC_START_RECORD:
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               MSG_FATAL("CAM_MODE_MOVIE",0,0,0);
               pme->m_nMode = CAM_MODE_MOVIE;
               nStatus = CAM_STATUS_START;
               pme->m_bStateChanging = FALSE;
               pme->m_nNextMode = CAM_MODE_READY;
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_EMOVIEFAIL;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

            case CAMERA_EXIT_CB_DONE:
               nStatus = CAM_STATUS_DONE;
               MSG_FATAL("OEMCamera_SetReadyMode",0,0,0);
               OEMCamera_SetReadyMode(pme);
               break;

            case CAMERA_STATUS_CB:

#ifdef DETAILED_SPACE_OUT_MSG     //This feature should not be enabled in pre-6550

               if ((parm4 == VIDEO_LIMIT_NEAR) || (parm4 == VIDEO_LIMIT_IMMINENT))
               {
                  nStatus = CAM_STATUS_SPACE_WARNING;
               }
               else if (  (parm4 == VIDEO_LIMIT_NEAR_SAMPLE_TABLE) || (parm4 == VIDEO_LIMIT_IMMINENT_SAMPLE_TABLE))
               {
                  nStatus = CAM_STATUS_SPACE_WARNING;
                  dwData = CAM_ERR_SPACE_MSG_SAMPLE_TABLE;
               }
               else if (  (parm4 == VIDEO_LIMIT_NEAR_CHUNK_TABLE) || (parm4 == VIDEO_LIMIT_IMMINENT_CHUNK_TABLE))
               {
                  nStatus = CAM_STATUS_SPACE_WARNING;
                  dwData = CAM_ERR_SPACE_MSG_CHUNK_TABLE;
               }
               else if (  (parm4 == VIDEO_LIMIT_NEAR_STORAGE) || (parm4 == VIDEO_LIMIT_IMMINENT_STORAGE))
               {
                  nStatus = CAM_STATUS_SPACE_WARNING;
                  dwData = CAM_ERR_SPACE_MSG_STORAGE;
               }
               else if (  (parm4 == VIDEO_LIMIT_NEAR_MOVIE_DURATION) || (parm4 == VIDEO_LIMIT_IMMINENT_MOVIE_DURATION))
               {
                  nStatus = CAM_STATUS_SPACE_WARNING;
                  dwData = CAM_ERR_SPACE_MSG_MOVIE_DURATION;
               }
               else if (parm4 == VIDEO_LIMIT_REACHED)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
               }
               else if (parm4 == VIDEO_LIMIT_REACHED_CHUNK_TABLE)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
                  dwData = CAM_ERR_SPACE_MSG_CHUNK_TABLE;
               }
               else if (parm4 == VIDEO_LIMIT_REACHED_SAMPLE_TABLE)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
                  dwData = CAM_ERR_SPACE_MSG_SAMPLE_TABLE;
               }
               else if (parm4 == VIDEO_LIMIT_REACHED_MOVIE_DURATION)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
                  dwData = CAM_ERR_SPACE_MSG_MOVIE_DURATION;
               }
               else if (parm4 == VIDEO_LIMIT_REACHED_STORAGE)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
                  dwData = CAM_ERR_SPACE_MSG_STORAGE;
               }
               else if (parm4 == VIDEO_ERROR || parm4 == VIDEO_FAILURE)
               {
                  nStatus = CAM_STATUS_SPACE_ERROR;
                  dwData = CAM_ERR_SPACE_MSG_VIDEO;
               }
#else

               if (parm4 == VIDEO_LIMIT_NEAR ||
                   parm4 == VIDEO_LIMIT_IMMINENT)
                  nStatus = CAM_STATUS_SPACE_WARNING;
               else if (parm4 == VIDEO_LIMIT_REACHED ||
                        parm4 == VIDEO_ERROR         ||
                        parm4 == VIDEO_FAILURE)
                  nStatus = CAM_STATUS_SPACE_ERROR;

#endif //DETAILED_SPACE_OUT_MSG
               break;
         }
         break;
      }
#endif //#ifdef FEATURE_VIDEO_ENCODE

      case CAMERA_FUNC_DISABLE_FRAME_CALLBACK: // Fall through...
#if defined(FEATURE_VIDEO_ENCODE)
      case CAMERA_FUNC_PAUSE_RECORD:
#endif //FEATURE_VIDEO_ENCODE
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               nStatus = CAM_STATUS_PAUSE;
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_EPAUSEFAIL;
               break;
         }
         break;
      }

      case CAMERA_FUNC_ENABLE_FRAME_CALLBACK: // Fall through...
#if defined(FEATURE_VIDEO_ENCODE)
      case CAMERA_FUNC_RESUME_RECORD:
#endif //FEATURE_VIDEO_ENCODE
      {
         switch (cb)
         {
            case CAMERA_RSP_CB_SUCCESS:
               nStatus = CAM_STATUS_RESUME;
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_ERESUMEFAIL;
               break;
         }
         break;
      }

#if defined(FEATURE_VIDEO_ENCODE)
      case CAMERA_FUNC_STOP_RECORD:
         break;
#endif //FEATURE_VIDEO_ENCODE

      case CAMERA_FUNC_SET_PARM:
      {
         switch (cb)
         {
            case CAMERA_EXIT_CB_DONE:
               nStatus = CAM_STATUS_DONE;
               break;

            case CAMERA_EXIT_CB_FAILED:
               nStatus = CAM_STATUS_FAIL;
               dwData = CAM_ESETPARMFAIL;
               break;
         }
         break;
      }
   }

Resume:
   if (nStatus)
   {
      if (pRsp->camNotify.nCmd == CAM_CMD_START)
      {
         pRsp = OEMCamera_GetStartCameraRsp(pme);
         if (!pRsp)
            goto CBExit;
      }

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
      if( nStatus == CAM_STATUS_DONE)
      {
         // changing the status of CAM_STATUS_DONE to CAM_STATUS_ABORT , this happens because
         // ACM has taken away the camera resource, We call OEMCamera_Stop resulting in CAM_STATUS_DONE
         // This way application will get ABORT
         if(pme->m_boCameraStoppedDueToACMRelease == TRUE)
         {
            nStatus =  CAM_STATUS_ABORT;
            pme->m_boCameraStoppedDueToACMRelease = FALSE;
         }
      }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

      pRsp->camNotify.nStatus = nStatus;
      pRsp->camNotify.pData = (void *)dwData;

      MEMCPY(&rspCopy, pRsp, sizeof(CameraRsp));

      if (pRsp->camNotify.nCmd == CAM_CMD_START)
      {
         if (nStatus == CAM_STATUS_DONE || nStatus == CAM_STATUS_ABORT ||
             nStatus == CAM_STATUS_FAIL || nStatus == CAM_STATUS_SPACE_ERROR)
         {
            //Need to free callback data on done or any error cases.
#if defined(FEATURE_VIDEO_ENCODE)
            if(
#ifdef DETAILED_SPACE_OUT_MSG
               parm4 != VIDEO_LIMIT_REACHED_STORAGE &&
               parm4 != VIDEO_LIMIT_REACHED_CHUNK_TABLE &&
               parm4 != VIDEO_LIMIT_REACHED_SAMPLE_TABLE &&
               parm4 != VIDEO_LIMIT_REACHED_MOVIE_DURATION &&
#endif /* DETAILED_SPACE_OUT_MSG */
               parm4 != VIDEO_LIMIT_REACHED &&
               parm4 != VIDEO_FAILURE &&
               parm4 != VIDEO_ERROR
              )
#endif /* FEATURE_VIDEO_ENCODE */
            {
               OEMCamera_FreeStartCameraRsp(pme);
            }
         }
         else
         {
            //Only reset the bInUse flag, more callback are coming.
            OEMCamera_UngetCameraRsp(pRsp);
         }
      }
      else if (pRsp->camNotify.nCmd == CAM_CMD_SETPARM ||
               pRsp->camNotify.nCmd == CAM_CMD_ENCODESNAPSHOT)
      {
         if(nStatus == CAM_STATUS_DONE || nStatus == CAM_STATUS_FAIL ||
            nStatus == CAM_STATUS_IO_ERROR || nStatus == CAM_STATUS_ABORT ||
            nStatus == CAM_STATUS_SPACE_ERROR ||
            nStatus == CAM_STATUS_FILE_SIZE_EXCEEDED)
         {
           //Need to free callback data on done or any error cases.
            OEMCamera_FreeCmdCameraRsp(pRsp);
         }
         else
         {
            //Only reset the bInUse flag, more callback are coming.
            OEMCamera_UngetCameraRsp(pRsp);
         }
      }
      else
      {
         //Should not run into here
         OEMCamera_UngetCameraRsp(pRsp);
      }
   }

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
   //No callback to UI if the direct mode is On.
   if (bDirectMode && (cb == CAMERA_EVT_CB_FRAME))
     goto CBExit;
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

  if (rspCopy.bInUse && pme->m_pfnNotify)
  {
      pfnAEE = pme->m_pfnNotify;
      pUserAEE = pme->m_pUser;
  }

CBExit:
   OEMCAMERA_LEAVE_CRITICAL_SECTION()

  if (pfnAEE)
  {
    pfnAEE(pUserAEE, (void *)&rspCopy.camNotify);

    if (bEncodeFailed)
    {
      //If no defer to encode after take picture and it failed with error code,
      //no more callback to here.
      //We just send a callback for success takeing snapshot above, so we need to
      //create a abort callback for the failure of this encoding and send here.
      rspCopy.camNotify.nCmd = CAM_CMD_ENCODESNAPSHOT;
      rspCopy.camNotify.nSubCmd = 0;
      rspCopy.camNotify.nStatus = CAM_STATUS_ABORT;
      rspCopy.camNotify.pData = NULL;
      rspCopy.camNotify.dwSize = 0;
      pfnAEE(pUserAEE, (void *)&rspCopy.camNotify);
    }
  }
}

/*==================================================================

==================================================================*/
static boolean OEMCamera_SaveFrameInfo(OEMCamera * pme, camera_frame_type * pf)
{
   if (!pf || !pf->buffer)
   {
      pme->m_pFrame->m_dib.cx = pme->m_pFrame->m_dib.cy = 0;
      pme->m_pFrame->m_dib.pBmp = NULL;
      return FALSE;
   }

   // Save the frame.
   MEMCPY(&pme->m_rawFrame, pf, sizeof(camera_frame_type));

   // Save it in the bitmap. Check write access.
   MEMMOVE(&pme->m_pFrame->m_rawFrame, pf, sizeof(camera_frame_type));

   // Set frame DIB info -- this is not complete info.
   if ((pf->rotation == 0) || (pf->rotation == 180))
   {
     pme->m_pFrame->m_dib.cx = pf->dx;
     pme->m_pFrame->m_dib.cy = pf->dy;
   }
   else
   {
     pme->m_pFrame->m_dib.cx = pf->dy;
     pme->m_pFrame->m_dib.cy = pf->dx;
   }
    pme->m_pFrame->m_dib.pBmp = pf->buffer;

   return TRUE;
}

/*==================================================================

==================================================================*/
static int OEMCamera_AddOverlay(OEMCamera * pme, IDIB * pDIB)
{
   int   nRet;

   if (pDIB)
      nRet = camera_set_overlay(pDIB->cx, pDIB->cy, pDIB->ncTransparent, pDIB->pBmp, NULL, NULL);
   else
      nRet = camera_clr_overlay(NULL, NULL);

   return OEMCamera_AEEError(nRet);
}

/*==================================================================

==================================================================*/
static void OEMCamera_SetReadyMode(OEMCamera * pme)
{
   MSG_FATAL("CAM_MODE_READY",0,0,0);
   pme->m_nMode = CAM_MODE_READY;
   pme->m_nNextMode = CAM_MODE_READY;
   pme->m_bPaused = FALSE;
   pme->m_bStateChanging = FALSE;
}

#ifdef FEATURE_VIDEO_ENCODE
/*==================================================================

==================================================================*/
static int16 OEMCamera_AudioStreamType(uint32 clsAudio, uint32 dwSubAudio)
{
   int   nFmt;

   if (clsAudio == 0)
      nFmt = VIDEO_FMT_STREAM_AUDIO_NONE;
   else if (clsAudio == AEECLSID_MEDIAQCP)
   {
      switch (dwSubAudio)
      {
         case MM_QCP_FORMAT_FIXED_FULL_13K:
         case MM_QCP_FORMAT_VAR_FULL_13K:
            nFmt = VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL;
            break;

         case MM_QCP_FORMAT_VAR_HALF_13K:
         case MM_QCP_FORMAT_FIXED_HALF_13K:
            nFmt = VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF;
            break;

         case MM_QCP_FORMAT_EVRC:
         case MM_QCP_FORMAT_FIXED_FULL_EVRC:
            nFmt = VIDEO_FMT_STREAM_AUDIO_EVRC;
            break;

         case MM_QCP_FORMAT_FIXED_4_75_AMR:
         case MM_QCP_FORMAT_FIXED_5_15_AMR:
         case MM_QCP_FORMAT_FIXED_5_9_AMR :
         case MM_QCP_FORMAT_FIXED_6_7_AMR :
         case MM_QCP_FORMAT_FIXED_7_4_AMR :
         case MM_QCP_FORMAT_FIXED_7_95_AMR:
         case MM_QCP_FORMAT_FIXED_10_2_AMR:
         case MM_QCP_FORMAT_FIXED_12_2_AMR:
            nFmt = VIDEO_FMT_STREAM_AUDIO_AMR;
            break;

         default:
            nFmt = VIDEO_FMT_STREAM_AUDIO_INVALID;
            break;
      } // switch
   }
#ifdef FEATURE_CAMCORDER_AAC_REC
   else if(clsAudio == AEECLSID_MEDIAAAC)
   {
     nFmt = VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC;
   }
#endif /* FEATURE_CAMCORDER_AAC_REC */
   else
      nFmt = VIDEO_FMT_STREAM_AUDIO_INVALID;

   return nFmt;
}
#endif /* FEATURE_VIDEO_ENCODE */

/*==================================================================

==================================================================*/
int OEMCamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                       void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop)
{
   camera_frame_type *  pRawFrame = (camera_frame_type *)pSrc;
   IBitmap *            pDstBmp = (IBitmap *)pDst;
   IDIB *               pDstDIB;
   int      nRet;
   camera_format_type   nColorScheme;

#if defined(AEE_SIMULATOR)
   AEERect  rc;

   if (SUCCESS != IBITMAP_QueryInterface(pDstBmp, AEECLSID_DIB, (void **)&pDstDIB))
      return EFAILED;

   switch (pDstDIB->nColorScheme)
   {
       case IDIB_COLORSCHEME_666:
         nColorScheme = CAMERA_RGB666;
         break;
       case IDIB_COLORSCHEME_444:
         nColorScheme = CAMERA_RGB444;
         break;
       case IDIB_COLORSCHEME_888:
         nColorScheme = CAMERA_RGB888;
         break;
       default:
         nColorScheme = CAMERA_RGB565;
         break;
   }

   nRet = camera_blt_ext(pRawFrame, (uint16)xSrc, (uint16)ySrc, pDst, (uint16)xDst, (uint16)yDst, (uint16)cxDst, (uint16)cyDst, nColorScheme);
   
   rc.x = xDst;
   rc.y = yDst;
   rc.dx = cxDst;
   rc.dy = cyDst;
   IBITMAP_Invalidate(pDst, &rc);

   IBITMAP_Release((IBitmap *)pDstDIB);

   return OEMCamera_AEEError(nRet);
#else
   //
   // QCT-specific code:
   //
#if !defined(OEM_PIXEL_SIZE)
#define OEM_PIXEL_SIZE   2
#endif
   uint16   nPixelSize;
#ifndef FEATURE_MDP
   AEERect  rc;
#endif /* FEATURE_MDP */
   if (SUCCESS != IBITMAP_QueryInterface(pDstBmp, AEECLSID_DIB, (void **)&pDstDIB))
      return EFAILED;

   switch (pDstDIB->nColorScheme)
   {
       case IDIB_COLORSCHEME_666:
         nColorScheme = CAMERA_RGB666;
         nPixelSize = 4;
         break;
       case IDIB_COLORSCHEME_444:
         nColorScheme = CAMERA_RGB444;
         nPixelSize = 2;
         break;
       default:
         nColorScheme = CAMERA_RGB565;
         nPixelSize = 2;
         break;
   }

   nRet = camera_blt_ext(pRawFrame, (uint16)(pDstDIB->nPitch / nPixelSize), pDstDIB->cy, (void *)pDstDIB->pBmp, (uint16)xDst, (uint16)yDst, (uint16)cxDst, (uint16)cyDst, nColorScheme);
   
   //nRet = camera_blt(pRawFrame, (uint16)(pDstDIB->nPitch / OEM_PIXEL_SIZE), pDstDIB->cy, (void *)pDstDIB->pBmp, (uint16)xDst, (uint16)cxDst, (uint16)cyDst);
   //nRet = camera_blt_ext(pRawFrame, (uint16)(pDstDIB->nPitch / OEM_PIXEL_SIZE), pDstDIB->cy, (void *)pDstDIB->pBmp, (uint16)xDst, (uint16)yDst, (uint16)cxDst, (uint16)cyDst, nColorScheme);
/* If in FrameCallBack Mode display Invalidate the Display buffer*/
#ifndef FEATURE_MDP
   rc.x = xDst;
   rc.y = yDst;
   rc.dx = cxDst;
   rc.dy = cyDst;

   IBITMAP_Invalidate(pDst, &rc);
#elif defined FEATURE_CAMERA_LCD_DIRECT_MODE
   if (!bDirectMode)
   {
     AEERect  rc;

     rc.x = xDst;
     rc.y = yDst;
     rc.dx = cxDst;
     rc.dy = cyDst;
     IBITMAP_Invalidate(pDst, &rc);
   }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
   IBITMAP_Release((IBitmap *)pDstDIB);

   return OEMCamera_AEEError(nRet);
#endif // defined(AEE_SIMULATOR)
}

#endif // defined(FEATURE_BREW_CAMERA)
