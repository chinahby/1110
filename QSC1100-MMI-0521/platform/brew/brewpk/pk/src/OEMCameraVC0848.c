/*============================================================================
//           
// 文件:
//        OEMCameraVC0848.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        OEM camera 处理函数相关文件
//                 
==============================================================================*/
/*============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/

/*============================================================================

                   本文件包含的外部文件

==============================================================================*/
#include "OEMFeatures.h"
#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "AEE_OEMFile.h"
#include "AEEMediaFormats.h"
#include "AEEPosDet.h"
#include "AEEStdLib.h"
#include "VIM_COMMON.h"
#include "Disp_options.h"
#include "OEMCameraVC0848.h"
#include "AEE_OEM.h"
#include "err.h"
#include "OEMCamera.h"
#include "Mmd_ply_driver.h"
#include "..\..\apps\media\media_player.h"
#include "Bio.h"
#include "rex.h"
#include "clk.h"
#include "video_common.h"
#include "bio_mb6100.h"  //for snapshot 20081203
#include "db.h"
#include "oemcamera.h"
#include "vc0848.h"
#include "OEMFSPath_priv.h"
/*===========================================================================

                      CameraDevice DECLARATIONS

===========================================================================*/
#if 0
typedef enum _VIM_FILE_TYPE{
	VIM_FILE_TYPE_JPG,
	VIM_FILE_TYPE_AMR,
	VIM_FILE_TYPE_3GP,
	VIM_FILE_TYPE_ERROR = 0XFF
}VIM_FILE_TYPE;
#endif 

struct _camera_encode_properties_type
{
    camera_encode_type format; 
    int16              quality;
    int32              file_size  ;
};
typedef struct  _camera_encode_properties_type  camera_encode_properties_type;
camera_encode_properties_type  p; //set the type of encode properties 

AEECLSID AEECLSID_APPRESCTL;

typedef struct CameraDevice   CameraDevice;
struct CameraDevice
{
    int   nRefs;
};

camera_resize_type      camera_resize;

static CameraDevice     gCameraDevice = { 0 };
static camera_info_type gCameraInfo;

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/
#define OEMCamera_ISREADY(pme)        ((pme)->m_nMode == CAM_MODE_READY)
#define OEMCamera_ISPREVIEW(pme)      ((pme)->m_nMode == CAM_MODE_PREVIEW)
#define OEMCamera_ISSNAPSHOT(pme)     ((pme)->m_nMode == CAM_MODE_SNAPSHOT)
#define OEMCamera_ISMOVIE(pme)        ((pme)->m_nMode == CAM_MODE_MOVIE)
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
#define OEMCAMERA_LEAVE_CRITICAL_SECTION()          ;} OEMCriticalSection_Leave(&gOEMCameraCriticalSection);
#define OEMCamera_UngetCameraRsp(pRsp)              {(pRsp)->bInUse = FALSE;}

#define OEMCamera_FreeCmdCameraRsp                  OEMCamera_FreeCameraRsp

#define MIN_FILE_NAME_LEN   64
#define CAMERA_PREVIEW_HEIGHT_OFFSET   28
/*===========================================================================

                      ICamera DECLARATIONS

===========================================================================*/

static CameraRsp *     OEMCamera_AllocStartCameraRsp(OEMCamera * pme, int16 nSubCmd);
static int             OEMCamera_RecordSnapshot(OEMCamera * pme, CameraRsp * pRsp);
static void            OEMCamera_FreeCameraRsp(CameraRsp * pRsp);
static void            OEMCamera_FreeStartCameraRsp(OEMCamera * pme);
static void            OEMCamera_SetPreviewMode(OEMCamera *pme);
static CameraRsp *     OEMCamera_GetStartCameraRsp(OEMCamera * pme);
static void            OEMCamera_SetReadyMode(OEMCamera * pme);
static boolean         OEMCamera_SaveFrameInfo(OEMCamera * pme, camera_frame_type * pf);
static void            OEMCamera_FreeStartCameraRsp(OEMCamera * pme);
static int             OEMCamera_Preview(OEMCamera * pme, CameraRsp * pRsp);
static int             OEMCamera_RecordMovie(OEMCamera * pme, CameraRsp * pRsp);
static boolean         OEMCamera_FilePath(OEMCamera * pme, void * pData);
static int             OEMCamera_AddOverlay(OEMCamera * pme, IDIB * pDIB);
CameraRsp *            OEMCamera_AllocCmdCameraRsp(OEMCamera * pme, int16 nCmd, int16 nSubCmd);
camera_ret_code_type   OEMCamera_InSetParm(  camera_parm_type id,  int32 parm,  camera_cb_f_type callback,  void *client_data);
camera_ret_code_type   OEMCamera_InGetParm(camera_parm_type id, camera_parm_info_type *parm);
static void            OEMCamera_InitCameraRsp(OEMCamera * pme, int16 nCmd);
static int             OEMCamera_SetSnapshotDestination(OEMCamera * pme, camera_handle_type * ph);
static int             OEMCamera_SetMovieDestination(OEMCamera * pme, video_handle_type * ph);
static void            OEMCameraSetNextMode(int16 nMode,db_items_value_type db_item);
static void            OEMCamera_Preview_Init(OEMCamera * pme, int nPreviewType, int nMode);


static OEMCriticalSection gOEMCameraCriticalSection;

/*==================================================================

==================================================================*/
void OEMCamera_Init(void)
{

    // Empty
}

/*===========================================================================
FUNCTION      CAMERA_START

DESCRIPTION
              Set the LCD display window where application can render on.
              Other parts of LCD may be owned by others.

              Can only be executed in INIT state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type CameraDevice_start(camera_cb_f_type callback,void *client_data)
{
    camera_ret_code_type ret_code = CAMERA_SUCCESS;
    vc_union_type  vc_data;

    vc_data.dev_run.curr_dev = VC_DEV_CAMERA;
    VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data);
    { 
        camera_resize.frame.x = 0;
        camera_resize.frame.y = 0;
        camera_resize.frame.rotate = 0;
        camera_resize.frame.mirror = FALSE;
 
        camera_resize.display.x = CAMERA_PREVIEW_WIDTH;
        camera_resize.display.y = CAMERA_PREVIEW_HEIGHT;
        camera_resize.display.rotate = 0;
        camera_resize.display.mirror = FALSE;
  
        camera_resize.fps = CAMERA_VEDIO_FRAME;
 
        ret_code = CAMERA_SUCCESS;
    }
#if 0    
    else
        ret_code = CAMERA_FAILED;
#endif

    if((ret_code == CAMERA_SUCCESS) && (callback != NULL))
    {
        (callback)(CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_START, NULL);
    }
  
    return ret_code;
}

/*===========================================================================
FUNCTION      CAMERA_STOP

DESCRIPTION
              The camera function is terminated and the sensor enters power
              saving mode.

              Can be executed in all but INIT state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type CameraDevice_stop(camera_cb_f_type callback,void *client_data)
{
    camera_ret_code_type ret_code = CAMERA_SUCCESS;

    vc_union_type  vc_data;
    vc_data.dev_run.curr_dev = VC_DEV_CAMERA;

    VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data); 
    {      
        ret_code = CAMERA_SUCCESS;
    }
#if 0    
    else
        ret_code = CAMERA_FAILED;
#endif  
    if((ret_code == CAMERA_SUCCESS) && (callback != NULL))
    {
        (callback)(CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_STOP, NULL);
    }
    
    return ret_code;
}


/*===========================================================================
FUNCTION      CAMERA_GET_INFO

DESCRIPTION
              Retrieve info

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
void Camera_get_info(camera_info_type *info)
{
    info->sensor_width = camera_resize.frame.width;
    info->sensor_height = camera_resize.frame.height;
  
    info->display_width = camera_resize.display.width;
    info->display_height = camera_resize.display.height;
  
    info->display_start_x = camera_resize.display.x;
    info->display_start_y = camera_resize.display.y;
  
    info->fps = camera_resize.fps;
  
    info->sensor_rotation_sensing = FALSE;
    info->default_rotation = camera_resize.frame.rotate;
    info->default_orientation = CAMERA_ORIENTATION_LANDSCAPE;
  
    info->mirror = camera_resize.frame.mirror;
}

static int CameraDevice_Open(void)
{
    if (gCameraDevice.nRefs > 0)
    {
#ifdef FEATURE_ACM    
        return EFAILED;
#else
        ++gCameraDevice.nRefs;
#endif     
    }
    else
    {
        // Initialize the phrase device
        if(CameraDevice_start(NULL, NULL) != CAMERA_SUCCESS)
        {
            return EFAILED;
        }
        gCameraDevice.nRefs = 1;
        Camera_get_info(&gCameraInfo);
    }

    return SUCCESS;
}

static int CameraDevice_Close(void)
{
    if (gCameraDevice.nRefs <= 0)
        return EFAILED;

    if (--gCameraDevice.nRefs == 0)
    {
        CameraDevice_stop(NULL, NULL);
    }
  
    return SUCCESS;
}

 
/*==================================================================
description:
             select camera device and set the parameter, open the camera device if necessary.  
parameters:
             cls:  the name of the camera sensor
             pframe: frame info
             wsize: null
             ph:
return:   
             EBADPARM:
             ENOMEMORY:
             nRet:
             
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph)
{
    OEMCamera *pme = NULL;
    int        nRet = SUCCESS;
    db_items_value_type db_item;
   if(SUCCESS != oem_media_scheduler())
   {
        return EITEMBUSY;
   }
        
    if (!ph)
        return EBADPARM;

    // Alloc memory for the object
    pme = (OEMCamera *)MALLOC(sizeof(OEMCamera));
    MSG_FATAL("MALLOC for camera instance", 0, 0, 0);
  
    if (!pme)
    {
        nRet = ENOMEMORY;
        goto Done;
    }
  
  
    // Open the camera device, if necessary
    if (CameraDevice_Open())
    {
        nRet = ENOTALLOWED;
        goto Done;
    }
    MSG_FATAL("here camera device open ", 0, 0, 0);
    
    pme->m_bRelease = FALSE;
    pme->m_nMode = CAM_MODE_READY;
    pme->m_clsVideo = AEECLSID_JPEG;
  
    pme->m_pFrame = (FrameBitmap *)pFrame;
  
    /* Set Frame size default to 0. _set_dimension() is not called
     * until a non-zero value is set. */
    pme->m_sizeFrame.cx = CAMERA_MAX_WIDTH;
    pme->m_sizeFrame.cy = CAMERA_MAX_HEIGHT;
    /* Set display size default to 160*120. A zero display size may
     * cause divide by zero exception. */
    pme->m_sizeDisplay.cx = CAMERA_PREVIEW_WIDTH;
    pme->m_sizeDisplay.cy = CAMERA_PREVIEW_HEIGHT;
    pme->m_cxOffset = 0;
    pme->m_cyOffset = CAMERA_PREVIEW_HEIGHT_OFFSET;
    // By default, file size is disabled by setting param to zero
    pme->m_nFileSize = OEMCAMERA_FILE_SIZE_DISABLED;


    // Assume we will be using Snapshot mode
    pme->m_nQuality = OEMCAMERA_QUALITY_HIGH;
    pme->m_nPreviewType = CAM_PREVIEW_SNAPSHOT;
    pme->m_nEffect =  MMD_MEDIA_SPECIAL_NORMAL; 
    pme->m_bPreviewWithFrame = VC_PREV_NOFRAME;
    
#ifdef FEATURE_CAMERA_ZOOM
    gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep=0;
#endif

#ifdef FEATURE_CAMERA_BURST_MODE
    // pme->m_nBracketingMode = CAMERA_BRACKETING_OFF; 
    pme->m_multishotInfo.nCount = 1;
    pme->m_multishotInfo.dwInterval = 0;
    pme->m_nShotIndex = -1;
    pme->m_nCurrIndex = -1;
#endif //FEATURE_CAMERA_BURST_MODE

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
    /* Default to Frame CallBack whenever you enter the Camera App */
    pme->m_bDirectMode = FALSE;
#endif

    OEMCamera_InitCameraRsp(pme, CAM_CMD_START);
    OEMCamera_InitCameraRsp(pme, CAM_CMD_SETPARM);
    OEMCamera_InitCameraRsp(pme, CAM_CMD_ENCODESNAPSHOT);
  
    #if defined(FEATURE_BREWAPI_SPECIFIC)
    // PNG
    OEMCamera_InitCameraRsp(pme, CAM_CMD_OVERLAY_IMAGE);
    #endif //defined(FEATURE_BREWAPI_SPECIFIC)
  
    db_item.nCamMode = CAM_MODE_READY;
    db_put(DB_CAMERA_MODE, &db_item);
    pme->m_bPreviewInit = FALSE;
    *ph = (OEMINSTANCE)pme;
    return nRet;

Done:
    if (ph)
        *ph = NULL;
    FREE(pme);
    return nRet;
}


/*==================================================================

==================================================================*/
int OEMCamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                       void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop)
{
    return CAMERA_NOT_SUPPORTED;
}

/*==================================================================
description:
              Release the frame buffer after the frame is delivered to the user.

parameters:
             h:  demonte an opaque handle to a particular instance of data.              
return:        
                      
note:
             none
             remarms:
state:
             valid
==================================================================*/
void OEMCamera_ReleaseFrame(OEMINSTANCE h)
{    
}

/*==================================================================
description:
             delete the camera device ,delete all the Rsps 
parameters:
           h: camera info
return:   
                         
note:
             none
             remarms:
state:
             valid
==================================================================*/
void OEMCamera_Delete(OEMINSTANCE h)
{
    int         i;
    db_items_value_type db_item;
    OEMCamera * pme = (OEMCamera *)h;

    if (!pme )
    return;

    //Turn on the flag so callback from camera will be ignored.
    pme->m_bRelease = TRUE;
    //if it is taking picture or encoding, abort it first.
    if (OEMCamera_ISSNAPSHOT(pme))
    (void)camera_release_picture();

    /* Unregister the handler */
    AEEObjectMgr_Unregister(pme->m_hObject);

    // Delete all the Rsps...
    MSG_ERROR("OEMCamera_Delete call OEMCamera_FreeStartCameraRsp",0, 0, 0);
    OEMCamera_FreeStartCameraRsp(pme);
    for (i = 0; i < OEMCAMERA_RSP_COMMAND_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_rspCommand[i]);

    for (i = 0; i < OEMCAMERA_RSP_ENCODESNAPSHOT_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_rspEncodeSnapshot[i]);

#if defined(FEATURE_BREWAPI_SPECIFIC)
    for (i = 0; i < OEMCAMERA_RSP_COMMAND_MAX; i++)
    OEMCamera_FreeCameraRsp(&pme->m_pngCommand[i]);
#endif // defined(FEATURE_BREWAPI_SPECIFIC)

#ifdef FEATURE_CAMERA_MULTI_SENSOR
    CameraDevice_Close(pme->m_sensorId);
#else
    CameraDevice_Close();
#endif

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

    FREEIF(pme->m_pszMIME);
    FREEIF(pme->m_pszFile);

    db_item.nCamMode = CAM_MODE_READY;
    db_put(DB_CAMERA_MODE, &db_item);
    FREE(pme);
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
      case CAMERA_SUCCESS:        nRet = SUCCESS;         break;
      case CAMERA_INVALID_STATE:  nRet = EBADSTATE;       break;
      case CAMERA_INVALID_PARM:   nRet = EBADPARM;        break;
      case CAMERA_INVALID_FORMAT: nRet = EBADPARM;        break;
      case CAMERA_NO_SENSOR:      nRet = EFAILED;         break;
      case CAMERA_NO_MEMORY:      nRet = ENOMEMORY;       break;
      case CAMERA_NOT_SUPPORTED:  nRet = EUNSUPPORTED;    break;
      case CAMERA_FAILED:         nRet = EFAILED;         break;
      default:                    nRet = EFAILED;         break;
   }

   return nRet;
}

/*==================================================================
   CameraLayer callback called in CameraLayer context
==================================================================*/
void OEMCamera_CameraLayerCB(camera_cb_type cb, const void *client_data, camera_func_type func, int32 parm4)
{
    CameraRsp * pRsp = (CameraRsp *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
    OEMCamera * pme;
    CameraRsp   rspCopy;
    
    if (!pRsp || !pRsp->pOwner)
        return;

    pme = pRsp->pOwner;

    MEMSET(&rspCopy, (int)0, sizeof(rspCopy));

    OEMCAMERA_ENTER_CRITICAL_SECTION()

    int         nStatus = 0;
    uint32      dwData = SUCCESS;

    // Common status handling...
    switch (cb)
    {
        case CAMERA_EXIT_CB_DSP_IDLE:    // Fall through...
        case CAMERA_EXIT_CB_DSP_ABORT:   // Fall through...
        case CAMERA_EXIT_CB_ABORT:       // Fall through...
        case CAMERA_EXIT_CB_ERROR:       
            nStatus = CAM_STATUS_ABORT; 
            if (pRsp->camNotify.nCmd == CAM_CMD_START)
                OEMCamera_SetReadyMode(pme);
            goto Resume;
            break;
   
        case CAMERA_EVT_CB_FRAME:
            if (func == CAMERA_FUNC_START_PREVIEW || func == CAMERA_FUNC_START_RECORD)
            {
                OEMCamera_SaveFrameInfo(pme, (camera_frame_type *)parm4);
                nStatus = CAM_STATUS_FRAME;
            }
            goto Resume;
            break;
    }

    switch (func)
    {
        case CAMERA_FUNC_START: // Fall through...
        case CAMERA_FUNC_STOP:
        break;

        case CAMERA_FUNC_START_PREVIEW:
        {
            ERR("CAMERA_FUNC_START_PREVIEW",0,0,0); 
            switch (cb)
            {
                case CAMERA_RSP_CB_SUCCESS:   
                    pme->m_nMode = CAM_MODE_PREVIEW;
                    pRsp->camNotify.nCmd = CAM_CMD_START;
                    pRsp->camNotify.nSubCmd = CAM_MODE_PREVIEW;
                    nStatus = CAM_STATUS_START;
                    ERR("the state changing false!",0,0,0); 
                    pme->m_bStateChanging = FALSE;
                    MSG_FATAL("pme->m_nMode=%d,nStatus=%d,m_bStateChanging=%d", pme->m_nMode, nStatus, pme->m_bStateChanging);
                    break;

                case CAMERA_EXIT_CB_FAILED:    
                    nStatus = CAM_STATUS_FAIL; 
                    dwData = CAM_EPREVIEWFAIL; 
                    OEMCamera_SetReadyMode(pme);
                    break;

                case CAMERA_EXIT_CB_DONE:     
                    nStatus = CAM_STATUS_DONE;
                    OEMCamera_SetReadyMode(pme);
                    break;
            }
            break;
        }

        case CAMERA_FUNC_STOP_PREVIEW:
        break;

        case CAMERA_FUNC_TAKE_PICTURE:
        {
            ERR("CAMERA_FUNC_TAKE_PICTURE",0,0,0); 
            switch (cb)
            {
                case CAMERA_RSP_CB_SUCCESS:   
                    pme->m_nMode = CAM_MODE_SNAPSHOT;
                    nStatus = CAM_STATUS_START; 
                    ERR("the state changing false!",0,0,0); 
                    pme->m_bStateChanging = FALSE;
                    MSG_FATAL("pme->m_nMode=%d,nStatus=%d,m_bStateChanging=%d", pme->m_nMode, nStatus, pme->m_bStateChanging);
                    break;

                case CAMERA_EXIT_CB_DONE:
                    MSG_FATAL("set ready mode", 0, 0, 0);
                    nStatus = CAM_STATUS_DONE;
                    OEMCamera_SetReadyMode(pme);
                    break;

                case CAMERA_EXIT_CB_FAILED:   
                    nStatus = CAM_STATUS_FAIL; 
                    dwData = CAM_ESNAPSHOTFAIL; 
                    OEMCamera_SetReadyMode(pme);
                    break;
            }
            break;
        }
       
        case CAMERA_FUNC_ENCODE_PICTURE:
        {
            ERR("CAMERA_FUNC_ENCODE_PICTURE",0,0,0); 
            switch (cb)
            {
                case CAMERA_EXIT_CB_DONE:     
                    nStatus = CAM_STATUS_DONE;
                    dwData = parm4;
                    break;

                case CAMERA_EXIT_CB_FAILED:  
                    nStatus = CAM_STATUS_FAIL; 
                    dwData = CAM_ESNAPSHOTFAIL; 
                    OEMCamera_SetPreviewMode(pme);
                    break;

                case CAMERA_EXIT_CB_ERROR:
                    nStatus = CAM_STATUS_FAIL;
                    dwData = CAM_ESAVEFAIL;
                    OEMCamera_SetPreviewMode(pme);
                    break;
            }
            break;
        }

        case CAMERA_FUNC_START_RECORD:
        {
            ERR("CAMERA_FUNC_START_RECORD",0,0,0); 
            switch (cb)
            {
                case CAMERA_RSP_CB_SUCCESS:   
                    pme->m_nMode = CAM_MODE_MOVIE;
                    ERR("the state changing false!",0,0,0); 
                    nStatus = CAM_STATUS_START; 
                    pme->m_bStateChanging = FALSE; 
                    MSG_FATAL("pme->m_nMode=%d,nStatus=%d,m_bStateChanging=%d", pme->m_nMode, nStatus, pme->m_bStateChanging);
                    break;

                case CAMERA_EXIT_CB_FAILED:    
                    nStatus = CAM_STATUS_FAIL; 
                    dwData = CAM_EMOVIEFAIL; 
                    OEMCamera_SetReadyMode(pme);
                    break;

                case CAMERA_EXIT_CB_DONE:     
                    nStatus = CAM_STATUS_DONE;
                    OEMCamera_SetReadyMode(pme);
                    break;
            }
            break;
        }

        case CAMERA_FUNC_DISABLE_FRAME_CALLBACK: // Fall through...
        case CAMERA_FUNC_PAUSE_RECORD:
        {
            ERR("CAMERA_FUNC_PAUSE_RECORD",0,0,0); 
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
        case CAMERA_FUNC_RESUME_RECORD:
        {
            ERR("CAMERA_FUNC_RESUME_RECORD",0,0,0);
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

        case CAMERA_FUNC_STOP_RECORD:
            break;

        case CAMERA_FUNC_SET_PARM:
        {
        ERR("CAMERA_FUNC_SET_PARM, cb:%d",cb,0,0);
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
    ERR("nStatus = %d",nStatus,0,0);
   
Resume:
    if (nStatus)
    {
        if (pRsp->camNotify.nCmd == CAM_CMD_START)
        {
            pRsp = OEMCamera_GetStartCameraRsp(pme);
            if (!pRsp)
            goto CBExit;
        }

        pRsp->camNotify.nStatus = nStatus;
        pRsp->camNotify.pData = (void *)dwData;
  
        MEMCPY(&rspCopy, pRsp, sizeof(CameraRsp));

        if (pRsp->camNotify.nCmd == CAM_CMD_START)
        {
            if (nStatus == CAM_STATUS_DONE || nStatus == CAM_STATUS_ABORT)
                OEMCamera_FreeStartCameraRsp(pme);
            else
                OEMCamera_UngetCameraRsp(pRsp);
        }
        else if (pRsp->camNotify.nCmd == CAM_CMD_SETPARM)
            OEMCamera_FreeCmdCameraRsp(pRsp);
        else
            OEMCamera_UngetCameraRsp(pRsp);
    }

CBExit:
    OEMCAMERA_LEAVE_CRITICAL_SECTION()

    if (rspCopy.bInUse && pme->m_pfnNotify) 
    {
        // BRIDLE_ASSERT_SAFE_PFN(pme->m_pfnNotify);
        pme->m_pfnNotify(pme->m_pUser, (void *)&rspCopy.camNotify);
    }    

}
/*==================================================================
description:
             query the interface that just if the interface be supported
parameters:
              h:camera info
              idReq:
              ppo:
return:   
             AEE_SUCCESS:
            ECLASSNOTSUPPORT:             
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_QueryInterface(OEMINSTANCE h, AEECLSID idReq, void ** ppo)
{
    // OEMs can extend ICAMERA_QueryInterface() to return
    // related objects like IResourceCtl associated with ICamera object.
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
    #endif //FEATURE_ACM
    return ECLASSNOTSUPPORT;
}

/*==================================================================
description:
             User registered callback info.
parameters:
              h:camera info
              pfnNotify:
              pUser:
return:   
             SUCCESS:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_RegisterNotify(OEMINSTANCE h, PFNCAMERANOTIFY pfnNotify, void * pUser)
{
    OEMCamera * pme = (OEMCamera *)h;

    pme->m_pfnNotify = pfnNotify;
    pme->m_pUser = pUser;
 
    return SUCCESS;
}

/*==================================================================
description:
             select and start the camera in preview ,snapshot and movie states.
parameters:
              h:camera info
              nMode:camera mode
              dwParam:
              
return:   
             EBADPARM:
             EUNSUPPORTED:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam)
{
   OEMCamera * pme = (OEMCamera *)h;
   int         nRet = EFAILED;
   CameraRsp * pRsp;
   db_items_value_type db_item;

   static vc_union_type  vc_data;
   
   MSG_FATAL("OEMCamera_Start with nMode:%d, m_nMode", nMode, pme->m_nMode, 0);
   /* Lauence: the following judge contidion will occur some incorrect state in camera, careful pls, just remove the code for camera debug */
#if 0   
    if (!((OEMCamera_STARTOK(pme)) ||
        ((OEMCamera_ISPREVIEW(pme)) &&
        ((nMode == CAM_MODE_SNAPSHOT) || (nMode == CAM_MODE_MOVIE)))))
    {
        return EBADSTATE;
    }
#else
   //if (!OEMCamera_STARTOK(pme))
     // return EBADSTATE;
#endif // if 0    
   pRsp = OEMCamera_AllocStartCameraRsp(pme, nMode);
   if (!pRsp)
      return EITEMBUSY;

   // get sensor status whether it exists or not  
    if(pme->m_bPreviewInit == FALSE)
    {
        VC_DeviceControl(VC_ITM_SENSOR_STAT_I, VC_FUNC_GET_PARM, &vc_data);

        if(vc_data.sensor != VC_SENSOR_FIND_OK)
        {
           MSG_ERROR("vc_data.sensor != VC_SENSOR_FIND_OK, return EFAILED", 0, 0,0 );
           return EFAILED; // if no sensor find, return EFAILED
        }
    }
   
   switch(nMode)
   {
   case CAM_MODE_PREVIEW:
        if(OEMCamera_ISPREVIEW(pme)) 
            return EBADSTATE;
        OEMCameraSetNextMode(nMode,db_item);
        nRet = OEMCamera_Preview(pme, pRsp);
        break;
       
   case CAM_MODE_SNAPSHOT:
       if(OEMCamera_ISSNAPSHOT(pme)) 
           return EBADSTATE;
        
#ifndef FEATURE_CAMERA_BURST_MODE
        if (dwParam)
        {
            CameraSnapshotMulti *info = (CameraSnapshotMulti *)dwParam;
            pme->m_multishotInfo.nCount = info->nCount;
            pme->m_multishotInfo.dwInterval = info->dwInterval;
        }
        else
        {
            pme->m_multishotInfo.nCount = 1;
            pme->m_multishotInfo.dwInterval = 0;
        }
#endif //FEATURE_CAMERA_BURST_MODE
        OEMCameraSetNextMode(nMode,db_item);
        nRet = OEMCamera_RecordSnapshot(pme, pRsp);
        break;

   case CAM_MODE_MOVIE:
       if(OEMCamera_ISMOVIE(pme)) 
           return EBADSTATE;
       
        if(dwParam ==CAM_MOVIE_POSTCARD)
            pme->m_bAudioPostCardRecord = TRUE;
        else
            pme->m_bAudioPostCardRecord = FALSE;
        OEMCameraSetNextMode(nMode,db_item);
        nRet = OEMCamera_RecordMovie(pme, pRsp);
        break;

   default:
        nRet = EBADPARM;
        break;
   }
   
   if (nRet)
   {
      MSG_FATAL("Start, nRet:%d call OEMCamera_FreeCameraRsp", nRet, 0, 0);
      OEMCamera_FreeCameraRsp(pRsp);
      return CAMERA_FAILED;
   }
   else
   {
      pme->m_bStateChanging = TRUE;
      return CAMERA_SUCCESS;
   }
   //else
   //{
      //db_item.nCamMode = nMode;
      //db_put(DB_CAMERA_MODE, &db_item);
      //pme->m_bStateChanging = TRUE;
   //}
   //return nRet;
}


static void OEMCamera_Preview_Init(OEMCamera * pme, int nPreviewType, int nMode)
{
   static vc_union_type vc_data;
   
   if(nMode != CAM_MODE_PREVIEW) 
     return;
   
   pme->m_bPreviewInit = TRUE;
   
   switch(pme->m_nPreviewType)
   {
   case CAM_PREVIEW_SNAPSHOT:
     vc_data.prev_parm.prev_info.display_width  = pme->m_sizeDisplay.cx;
     vc_data.prev_parm.prev_info.display_height = pme->m_sizeDisplay.cy;
     vc_data.prev_parm.prev_info.display_xpos   = pme->m_cxOffset;
     vc_data.prev_parm.prev_info.display_ypos   = pme->m_cyOffset;
     vc_data.prev_parm.prev_info.display_effect = pme->m_nEffect;
     vc_data.prev_parm.frame.cx                 = pme->m_sizeFrame.cx;
     vc_data.prev_parm.frame.cy                 = pme->m_sizeFrame.cy;
     vc_data.prev_parm.quality                  = pme->m_nQuality;
     vc_data.prev_parm.prev_type                = VC_PREV_SNAPSHOT;
     vc_data.prev_parm.prev_frametype           = pme->m_bPreviewWithFrame;
     VC_DeviceControl(VC_ITM_PREVIEW_I, VC_FUNC_CAMERA_ON, &vc_data); // == VC_STAT_DONE)
     break;

   case CAM_PREVIEW_MOVIE:
     vc_data.prev_parm.prev_info.display_width  = CAMERA_PREVIEW_WIDTH;
     vc_data.prev_parm.prev_info.display_height = CAMERA_PREVIEW_HEIGHT;
     vc_data.prev_parm.prev_info.display_xpos   = 0;
     vc_data.prev_parm.prev_info.display_ypos   = CAMERA_PREVIEW_HEIGHT_OFFSET;
     vc_data.prev_parm.prev_info.display_effect = pme->m_nEffect;
     vc_data.prev_parm.frame.cx                 = pme->m_sizeFrame.cx;
     vc_data.prev_parm.frame.cy                 = pme->m_sizeFrame.cy;
     vc_data.prev_parm.quality                  = 0;
     vc_data.prev_parm.prev_type                = VC_PREV_MOVIE;
     vc_data.prev_parm.prev_frametype           = 0;
     VC_DeviceControl(VC_ITM_PREVIEW_I, VC_FUNC_CAMERA_ON, &vc_data);
     break;

   default:
     //ret_cb = CAMERA_EXIT_CB_FAILED;
     break;
   }
#if 0
   switch(nMode)
   {
   case CAM_MODE_PREVIEW:
       VC_DeviceControl(VC_ITM_PREVIEW_I, VC_FUNC_CAMERA_ON, &vc_data);
       break;
       
   case CAM_MODE_SNAPSHOT:
       //VC_DeviceControl(VC_ITM_REC_SNAPSHOT_I, VC_FUNC_CAMERA_ON, &vc_data);
       break;
       
   case CAM_MODE_MOVIE:
       //VC_DeviceControl(VC_ITM_REC_VIDEO_I, VC_FUNC_CAMERA_ON, &vc_data);
       break;
       
   default:
       break;
   }
#endif   
}
/*==================================================================
description:
              start preview
parameters:
             pme:  camera info
             pRsp:command codes
return:        
           boolean           
note:
             none
             remarms:
state:
             valid
==================================================================*/
static int OEMCamera_Preview(OEMCamera * pme, CameraRsp * pRsp)
{
    camera_ret_code_type  ret_code = CAMERA_SUCCESS;
    camera_cb_type        ret_cb;
    //vc_union_type         vc_data;
    
    switch(pme->m_nPreviewType)
    {
    case CAM_PREVIEW_SNAPSHOT:
        ret_cb = CAMERA_RSP_CB_SUCCESS;
        OEMCamera_Preview_Init(pme, CAM_PREVIEW_SNAPSHOT, CAM_MODE_PREVIEW);
        break;

    case CAM_PREVIEW_MOVIE:
        ret_cb = CAMERA_RSP_CB_SUCCESS;
        OEMCamera_Preview_Init(pme, CAM_PREVIEW_MOVIE, CAM_MODE_PREVIEW);
        break;

    default:
        ret_cb = CAMERA_EXIT_CB_FAILED;
        break;
    }
    
    if((ret_code == CAMERA_SUCCESS) && (OEMCamera_CameraLayerCB != NULL))
    {
      MSG_FATAL("OEMCamera_Preview call OEMCamera_CameraLayerCB with FUNC_START_PREVIEW", 0, 0, 0);  
      (OEMCamera_CameraLayerCB)(ret_cb, (void *)pRsp->hObject, CAMERA_FUNC_START_PREVIEW, NULL);
    }
    
    return ret_code;
}


/*==================================================================
description:
              pause the camera function. Pause the frame callback in preview and camera state. 
              And pause encode in camera state.
parameters:
              h          :  camera info
              bPause :         
return:   
             EBADSTATE:
             EUNSUPPORTED:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_Pause(OEMINSTANCE h, boolean bPause)
{
    // VC0848 not support PAUSE and RESUME func in Preview and Recording procedure
    return CAMERA_NOT_SUPPORTED;
}

/*==================================================================
description:
              stop the current action and make the camera be ready
parameters:
              h:camera info
                         
return:   
             EBADPARM:
             EUNSUPPORTED:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_Stop(OEMINSTANCE h)
{
    int   nCamRet = -1;
    db_items_value_type db_item;
    vc_union_type  vc_data;
    
    OEMCamera * pme = (OEMCamera *)h;
    CameraRsp * pRsp = OEMCamera_GetStartCameraRsp(pme);  
    //20081203
    if(!pme)
    {
        return EBADPARM;
    }
    
    if (!pRsp)
        pRsp = &pme->m_rspStart[0];
 
    pRsp->camNotify.nStatus = CAM_STATUS_DONE;   

    vc_data.preview = VC_OP_OFF;
    
    if (OEMCamera_ISPREVIEW(pme))
    {
        VC_DeviceControl(VC_ITM_PREVIEW_I, VC_FUNC_CONTROL, &vc_data);
        nCamRet = CAMERA_SUCCESS;
    }
    else if (OEMCamera_ISSNAPSHOT(pme))
    {
        VC_DeviceControl(VC_ITM_REC_SNAPSHOT_I, VC_FUNC_CONTROL, &vc_data);
        nCamRet = CAMERA_SUCCESS;
    }
    else if (OEMCamera_ISMOVIE(pme))
    {
        VC_DeviceControl(VC_ITM_REC_VIDEO_I, VC_FUNC_CONTROL, &vc_data);
        nCamRet = CAMERA_SUCCESS;
    }
    OEMCamera_SetReadyMode(pme);
    
    db_item.nCamMode = pme->m_nMode;
    db_put(DB_CAMERA_MODE, &db_item);
    
    return OEMCamera_AEEError(nCamRet);
}

/*==================================================================
description:
               encodes the latest raw snapshot frame

parameters:
             hInstance:  camera info               
return:   
             EFAILED:
             CAM_PENDING:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/

int OEMCamera_EncodeSnapshot(OEMINSTANCE hInstance)
{
    OEMCamera *          pme = (OEMCamera *)hInstance;
    int                  nRet;
    camera_handle_type   h;
    CameraRsp *          pRsp;
    
    if (!OEMCamera_ENCODESNAPSHOTOK(pme))
    {
        return EBADSTATE;
    }
 
    if (!pme->m_pFrame || !pme->m_pFrame->m_dib.pBmp)
    {
        return EFAILED;
    }
    
    pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_ENCODESNAPSHOT, 0);
    //if (!pRsp)
    //   nRet = EITEMBUSY;
    
    
    nRet = OEMCamera_SetSnapshotDestination(pme, &h);
    if (SUCCESS != nRet)
    {
        OEMCamera_FreeCmdCameraRsp(pRsp);
        //camera_release_picture(FALSE);
        VIM_HAPI_CaptureJpeg_Release();
        
        return nRet;
    }
 
    //nRet = camera_encode_picture(&pme->m_rawFrame, &h, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
 
    return OEMCamera_AEEError(nRet);
}

int OEMCamera_FrameBitmapNew(IBitmap ** ppFrame, uint16 * pwSize)
{
    return SUCCESS;
}
/*==================================================================

            USER MODE CODE SECTION

==================================================================*/
int OEMCamera_GetConfigInfo(CameraConfigInfo * pInfo)
{
    return SUCCESS;
}

boolean OEMCamera_GetFrameTrigger(IBitmap * pFrame)
{
    return SUCCESS;
}

/*==================================================================
description:
             set the camera control parms 
parameters:
              h:camera info
              nParmID:camera control Parms
              p1:
              p2:
return:   
             EBADPARM:
             EUNSUPPORTED:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/

int OEMCamera_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2)
{
    int               nRet = SUCCESS;
    camera_parm_type  eParm = CAMERA_PARM_MAX;
    CameraData        *pCData = NULL;
    OEMCamera *       pme = (OEMCamera *)h;
    static vc_union_type  vc_data;
    db_items_value_type  value_ptr;
    
    ERR("nParmID=%d, p1=%d",nParmID,p1,0);

    switch (nParmID)
    {
        case CAM_PARM_MEDIA_DATA:
        if (p1)
        {
            MSG_FATAL("come in to set the media data p1...", 0, 0, 0);
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
#else /* FEATURE_VIDEO_MULTI_UUID */
            FREEIF(pme->m_pUuid_cpgd);
            if(p2)
            {
                pCData = (CameraData *) p2;
                pme->m_pUuid_cpgd = MALLOC(pCData->dwBytes);
                MEMCPY(pme->m_pUuid_cpgd, pCData->pData, pCData->dwBytes);
            }
#endif /* FEATURE_VIDEO_MULTI_UUID */
            break;

        case CAM_ENCODE_UUID_DATA:  // single set of uuid scenario
#ifdef FEATURE_VIDEO_MULTI_UUID
#else /* FEATURE_VIDEO_MULTI_UUID */
            FREEIF(pme->m_pCpgd);
            if(p2)
            {
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

#ifdef FEATURE_AAC_REC  
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
#endif /* FEATURE_AAC_REC */
            if(p1 != VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
            {
                /* return success as audio format is set for all formats except AAC */
                return nRet;
            }
            break;

#endif /* FEATURE_VIDEO_ENCODE */

        case CAM_PARM_REFLECT: 
            eParm = CAMERA_PARM_REFLECT;
            break;
        case CAM_PARM_PREVIEW_TYPE: 
        MSG_FATAL("come in to set the perview type...", 0, 0, 0);
            //eParm = CAMERA_PARM_PREVIEW_MODE;
            if ((p1 == CAM_PREVIEW_SNAPSHOT) ||
                (p1 == CAM_PREVIEW_MOVIE))
                pme->m_nPreviewType = (int16)p1;
            else
                nRet = EBADPARM;
                break;
        case CAM_PARM_PREVIEWWITHFRAME:
         MSG_FATAL("set the parameter for preview with frame or not ", 0, 0, 0);
         pme->m_bPreviewWithFrame = (boolean)p1;
         break;                         //modify 
        case CAM_PARM_SIZE:   
        MSG_FATAL("come in to set frame size...",0, 0, 0);
            if (p1)
            {
                AEESize * tmpSize = (AEESize *)p1;
                MSG_FATAL("set frame size, cx:%d, cy:%d",tmpSize->cx, tmpSize->cy, 0);
                if(tmpSize->cx <= 0 || tmpSize->cx > CAMERA_MAX_WIDTH ||
                   tmpSize->cy <= 0 || tmpSize->cy > CAMERA_MAX_HEIGHT)
                {
                   nRet = EBADPARM;
                   break;
                }
                MEMCPY(&pme->m_sizeFrame, (AEESize *)p1, sizeof(AEESize));
            }

            break;
        case CAM_PARM_DISPLAY_OFFSET:
            if(pme->m_cxOffset != p1)
                {
                    pme->m_cxOffset = p1;
                }
            if(pme->m_cyOffset != p2)
                {
                    pme->m_cyOffset = p2;
                }
            break;

        case CAM_PARM_QUALITY:
        MSG_FATAL("come in to set the Quality ", 0, 0, 0);
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

        case CAM_PARM_BITRATE:      
        /* User specifies bit rate for camcorder encoding*/
            pme->m_nBitrate = p1;
            break;

        /* case CAM_PARM_PREVIEW_FPS:   
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
           break;*/


#if defined(FEATURE_CAMERA_V770)  
        case CAM_PARM_ISO:
            if (OEMCamera_ConverISO(&p1, TRUE) == SUCCESS)
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
            break;

        case CAM_PARM_AF_INFO: 
            eParm = CAMERA_PARM_AF_MODE;
            break;

#endif // defined(FEATURE_CAMERA_V770)

        case CAM_PARM_FPS:           
            MSG_FATAL("come in to set the fps...",0, 0, 0);
            pme->m_nFPS = (byte)p1;
            value_ptr.nEnvironMode = pme->m_nFPS;
            // eParm = CAMERA_PARM_FPS;
            //MSG_FATAL("pme->m_nFPS =%d",pme->m_nFPS,0,0);
            vc_data.parm.p1 = (int32)p1;
            VC_DeviceControl(VC_ITM_SET_ENVIRONMENT_I, VC_FUNC_SET_PARM, &vc_data);
            break;

        case CAM_PARM_BANDING:
            pme->m_nSensorMode = (uint8)p1;
            vc_data.parm.p1 = (int32)p1;
            VC_DeviceControl(VC_ITM_SET_BANDING_I, VC_FUNC_SET_PARM, &vc_data);
            break;

        case CAM_PARM_EFFECT:
            //eParm = CAMERA_PARM_EFFECT;
            //MSG_FATAL("set the parameter of effect", 0, 0, 0);
            pme->m_nEffect  = (int16)p1;
            vc_data.parm.p1 = (int32)p1;
            VC_DeviceControl(VC_ITM_SET_EFFECT_I, VC_FUNC_SET_PARM, &vc_data);
            break;
            
        case CAM_PARM_MULTISHOT:
            MSG_FATAL("set the parameter of mutishot", 0, 0, 0);
            pme->m_multishotInfo.nCount=(int)p1;
            MSG_FATAL("set_nCount=%d", pme->m_multishotInfo.nCount, 0, 0);
            break;
        case CAM_PARM_CONTRAST:
            eParm = CAMERA_PARM_CONTRAST;
            break;

        case CAM_PARM_BRIGHTNESS:
            //eParm = CAMERA_PARM_BRIGHTNESS;
            pme->m_nBrightnessval = (uint8)p1;
            MSG_FATAL("set the parameter of brightness", 0, 0, 0);
            vc_data.parm.p1= (int32)p1;
            VC_DeviceControl(VC_ITM_SET_BRIGHTNESS_I, VC_FUNC_SET_PARM, &vc_data);          
            break;

        case CAM_PARM_SHARPNESS:
            eParm = CAMERA_PARM_SHARPNESS;
            break;
        case CAM_PARM_ZOOM:
            
            pme->m_nZoomstep = (int16)p1; 
            MSG_FATAL("in setparm to set the zoom mode", 0, 0, 0);
            nRet=(int)VIM_HAPI_SetPreviewZoom_1(VIM_HAPI_SET_STEP,pme->m_nZoomstep,pme->m_sizeFrame.cx);
            
            if(nRet)
            {
                MSG_FATAL("can not zoom", 0, 0, 0); 
                return nRet;                                     
            }
            break;
            //}

        case CAM_PARM_ROTATE_ENCODE:
            eParm = CAMERA_PARM_ENCODE_ROTATION;
            break;

        case CAM_PARM_OVERLAY:
            nRet = OEMCamera_AddOverlay(pme, (IDIB *)p1);
            break;

        case CAM_PARM_DISPLAY_SIZE:
        ERR("Camera_disp",0,0,0);
        if (p1)
        {
            ERR("if use",0,0,0);
            MEMCPY(&pme->m_sizeDisplay, (void *)p1, sizeof(AEESize));
       
            DBGPRINTF("x=%d,y=%d",pme->m_sizeDisplay.cx,pme->m_sizeDisplay.cy);
            nRet = SUCCESS;
        }
        break;

        case CAM_PARM_DEFER_ENCODE:
            if (OEMCamera_CAMERAREADY(pme))
                pme->m_bDeferEncode = (boolean)p1;
            else
                nRet = EBADSTATE;
            break;

        case CAM_PARM_FILE_SIZE:
            // File size will take precidence over Quality if it is non zero
            // Units are Kilobytes for this param
            if ((p1 >= OEMCAMERA_FILE_SIZE_MIN_VALUE) &&
            (p1 <= OEMCAMERA_FILE_SIZE_MAX_VALUE))
                pme->m_nFileSize = (int32)p1;
            else
                nRet = EBADPARM;
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

    if (SUCCESS == nRet && eParm != CAMERA_PARM_MAX)
    {
        int nCamRet;
        CameraRsp * pRsp;

        pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_SETPARM, nParmID);
        if (!pRsp)
            nRet = EITEMBUSY;
        else
        {

            nCamRet = (int)OEMCamera_InSetParm(eParm, p1, OEMCamera_CameraLayerCB, (void *)pRsp->hObject);
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
 
}

/*==================================================================
description:
             get the camera control parms 
parameters:
              h:camera info
              nParmID:camera control Parms
              *pP1:
              *pP2:
return:   
             EBADPARM:
             EUNSUPPORTED:
                       
note:
             none
             remarms:
state:
             valid
==================================================================*/
int OEMCamera_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2)
{
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

        case CAM_PARM_AUDIO_ENCODE:
            if (pP1)
                *pP1 = (int32)pme->m_clsAudio;
            if (pP2)
                *pP2 = (int32)pme->m_dwSubAudio;
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

        case CAM_PARM_FPS: 
            *pP1 = (int32)(pme->m_nFPS);
            MSG_FATAL("fps=%d",*pP1,0,0);
            break;
   
   
        case CAM_PARM_CONTRAST:
            eParm = CAMERA_PARM_CONTRAST;
            break;
   
        case CAM_PARM_BRIGHTNESS:
            *pP1 = (int32)(pme->m_nBrightnessval);
            //eParm = CAMERA_PARM_BRIGHTNESS;
            break;
        case CAM_PARM_BANDING:
            *pP1 = (int32)(pme->m_nSensorMode);
   
        case CAM_PARM_SHARPNESS:
            eParm = CAMERA_PARM_SHARPNESS;
            break;
   
        case CAM_PARM_ZOOM:
            eParm = CAMERA_PARM_ZOOM;
            break;
        case CAM_PARM_MULTISHOT:
            *pP1=(int32)(pme->m_multishotInfo.nCount);
            MSG_FATAL("get_nCount=%d",*pP1,0,0);
            break;
        case CAM_PARM_ROTATE_PREVIEW:
            eParm = CAMERA_PARM_SENSOR_POSITION;
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
               *pP1 = (int32)pme->m_bDeferEncode;
                break;

        case CAM_PARM_MODE:
            if (pP1)
               *pP1 = (int32)pme->m_nMode;
            if (pP2)
               *pP2 = (int32)pme->m_bPaused;
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

        case CAM_PARM_PREVIEW_TYPE:
            if (pP1)
                *pP1 = (int32)pme->m_nPreviewType;
                break;

        case CAM_PARM_IS_SUPPORT:  // Fall through...
        case CAM_PARM_IS_MOVIE:
        case CAM_PARM_PIXEL_COUNT:
        case CAM_PARM_VIDEO_ENCODE_LIST:
        case CAM_PARM_AUDIO_ENCODE_LIST:
        case CAM_PARM_SIZE_LIST:
        case CAM_PARM_FPS_LIST:
        default:
            nRet = EUNSUPPORTED;
            break;
    }

    if (pP1 && eParm != CAMERA_PARM_MAX && SUCCESS == nRet)
    {
        int         nCamRet;
        AEEParmInfo pi;

        MEMSET(&pi, (int)0, sizeof(AEEParmInfo));
        nCamRet = (int)OEMCamera_InGetParm(eParm, (camera_parm_info_type *)&pi);
        ERR("test fsp",0,0,0);
        if (!nCamRet)
        {
            // Handle some exceptions...this can be done in a function if the list gets big.
            // Convert sensor position to rotate-angle...
            if (nParmID == CAM_PARM_ROTATE_PREVIEW)
            {
                if (pP1)
                    *pP1 = pi.nCurrent ? 180 : 0;
                if (pP2)
                {
                    pi.nMin = 0;
                    pi.nMax = 180;
                    pi.nStep = 180;
                    MEMCPY(pP2, &pi, sizeof(AEEParmInfo));
                }
            }
            else
            {
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
}


/*==================================================================

==================================================================*/

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
  
        default:
            p->format  = CAMERA_JPEG;
            break;
    }
    p->quality = pme->m_nQuality;
    p->file_size = pme->m_nFileSize;
}

/*==================================================================

==================================================================*/
static int OEMCamera_RecordSnapshot(OEMCamera * pme, CameraRsp * pRsp)
{
    camera_cb_type          ret_cb;
    vc_union_type           vc_data;
    int  nRet;   // return the value of result when capture pic 2009.04.18
    
    MSG_FATAL("multishot_count =%d", pme->m_multishotInfo.nCount, 0, 0);
    if(pme->m_multishotInfo.nCount >= 1)
    {
        char * pFile = NULL;
        vc_data.parm.p1 = pme->m_multishotInfo.nCount;

        // init record snapshot parm first
        OEMCamera_Preview_Init(pme, CAM_PREVIEW_SNAPSHOT, CAM_MODE_SNAPSHOT);
        
        pFile = SPLITPATH((const char *)pme->m_pszFile, OEMFS_CARD0_DIR);
        vc_data.parm.file_scr = VC_IN_CORE_FILE;
        if(!pFile)
        {
            pFile = SPLITPATH((const char *)pme->m_pszFile, OEMFS_BREW_DIR);
            vc_data.parm.file_scr = VC_IN_BASEBAND_RAM;
        }
        STRCPY((char *)&vc_data.parm.szStr, pFile);
        MSG_FATAL("nncount=%d", pme->m_multishotInfo.nCount, 0, 0);        
        //for(num = 0; num < pme->m_multishotInfo.nCount; num++)
        {
           ret_cb = CAMERA_RSP_CB_SUCCESS;
           (OEMCamera_CameraLayerCB)(ret_cb, (void *)pRsp->hObject, CAMERA_FUNC_TAKE_PICTURE, NULL); 
           nRet = VC_DeviceControl(VC_ITM_REC_SNAPSHOT_I, VC_FUNC_CAMERA_ON, &vc_data);
           return nRet;
        }
    }
    else
    {
        MSG_FATAL("error multishot_count ", 0, 0, 0);
        return EBADPARM;
    }
    
      return CAMERA_SUCCESS;
   
}



/*===============================================================================

                     LOCAL FUNCTIONS

===============================================================================*/
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
description:
          .........................

parameters:
             pme:  camera info
             nSubCmd:command codes
return:        
           boolean           
note:
             none
             remarms:
state:
             valid
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

==================================================================*/
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
            STRNCPY((char *)ph->efs.filename, pme->m_pszFile, sizeof(ph->efs.filename));
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
#ifdef FEATURE_AAC_REC
    else if(clsAudio == AEECLSID_MEDIAAAC)
    {
        nFmt = VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC;
    }
#endif /* FEATURE_AAC_REC */
    else
        nFmt = VIDEO_FMT_STREAM_AUDIO_INVALID; 
        return nFmt;
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
   For CAM_CMD_START, free all cbs
==================================================================*/
static void OEMCamera_FreeStartCameraRsp(OEMCamera * pme)
{
    CameraRsp * pList = pme->m_rspStart;
    int   nLen = OEMCAMERA_RSP_START_MAX;
    int   i;
 
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
 
    for (i = 0; i < nLen && pList[i].bInUse; i++);
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

==================================================================*/
static int OEMCamera_RecordMovie(OEMCamera * pme, CameraRsp * pRsp)
{
    int nRet;
    camera_cb_type        ret_cb = CAMERA_EXIT_CB_DONE;

    vc_union_type  vc_data;
    char * pFile = NULL;
    
    MSG_FATAL("here in the OEMCamera_RecordMovie();", 0, 0, 0);
    ret_cb = CAMERA_RSP_CB_SUCCESS;
    
    // Init record movie parm first
    OEMCamera_Preview_Init(pme, CAM_PREVIEW_MOVIE, CAM_MODE_MOVIE);
    
    pFile = SPLITPATH((const char *)pme->m_pszFile, OEMFS_CARD0_DIR);
    vc_data.parm.file_scr = VC_IN_CORE_FILE;
    if(!pFile)
    {
        pFile = SPLITPATH((const char *)pme->m_pszFile, OEMFS_BREW_DIR);
        vc_data.parm.file_scr = VC_IN_BASEBAND_RAM;
    }
    STRCPY((char *)&vc_data.parm.szStr, pFile);

    (OEMCamera_CameraLayerCB)(ret_cb, (void *)pRsp->hObject, CAMERA_FUNC_START_RECORD, NULL); 
    
    nRet=VC_DeviceControl(VC_ITM_REC_VIDEO_I, VC_FUNC_CAMERA_ON, &vc_data);
    if (nRet)
    {
      return CAMERA_FAILED;
    }
    else
    {
      return CAMERA_SUCCESS;
    }
}


/*==================================================================

==================================================================*/
static void   OEMCamera_SetPreviewMode(OEMCamera *pme)
{
    pme->m_nMode = CAM_MODE_PREVIEW;
    pme->m_bPaused = FALSE;
    pme->m_bStateChanging = FALSE;
}
/*==================================================================

==================================================================*/
static void OEMCamera_SetReadyMode(OEMCamera * pme)
{
    pme->m_nMode = CAM_MODE_READY;
    pme->m_bPaused = FALSE;
    pme->m_bStateChanging = FALSE;
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
    MEMMOVE(&pme->m_pFrame, pf, sizeof(camera_frame_type));//modify 
 
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
discription:
     this function add or clear the frame of the picture .
==================================================================*/
static int OEMCamera_AddOverlay(OEMCamera * pme, IDIB * pDIB)
{
    int   nRet;

    if (pDIB)
        //nRet = camera_set_overlay(pDIB->cx, pDIB->cy, pDIB->ncTransparent, pDIB->pBmp, NULL, NULL);
        nRet = MMD_LCD_SetOverlay ( VM_ENABLE, pDIB->nDepth, pDIB->nColorScheme, pDIB->ncTransparent);
    else
        nRet = MMD_LCD_SetOverlay ( VM_DISABLE, NULL,NULL,NULL);

    return OEMCamera_AEEError(nRet);
}

/*===========================================================================

FUNCTION      OEMCamera_InSetParm

DESCRIPTION
              Set various control on the camera sensor.

DEPENDENCIES
None

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type OEMCamera_InSetParm(  camera_parm_type id,  int32 parm,  camera_cb_f_type callback,  void *client_data)
{
    camera_ret_code_type  ret_code = CAMERA_SUCCESS;
    camera_cb_type        ret_cb = CAMERA_EXIT_CB_DONE;
    int                   nRet;
    switch(id)
    {
        case CAMERA_PARM_SENSOR_POSITION:
        {
            nRet=(int)VIM_HAPI_SetLCDMirror(VIM_HAPI_MIRROR_180,VIM_HAPI_A_LAYER);
            if(nRet)
            {
                ret_cb = CAMERA_EXIT_CB_FAILED;
                break;
            }
            nRet=(int)VIM_HAPI_SetLCDMirror(VIM_HAPI_ROTATE_180,VIM_HAPI_A_LAYER);
            if(nRet)
            {
                ret_cb = CAMERA_EXIT_CB_FAILED;
                break;
            }
        }
        break;     

        case CAMERA_PARM_BRIGHTNESS:
            ret_cb = CAMERA_EXIT_CB_FAILED;
            break;

        case CAMERA_PARM_CONTRAST:
        case CAMERA_PARM_SHARPNESS:
        case CAMERA_PARM_ENCODE_ROTATION:
        case CAMERA_PARM_EXPOSURE:
        case CAMERA_PARM_WB:
        case CAMERA_PARM_EFFECT:
        default:
            MSG_ERROR("Not supported parm, %d", id,0,0);
            ret_cb = CAMERA_EXIT_CB_FAILED;
            ret_code = CAMERA_NOT_SUPPORTED;
        break;
        }

        if((ret_code == CAMERA_SUCCESS) && (callback != NULL))
        {
            (callback)(ret_cb, client_data, CAMERA_FUNC_SET_PARM, NULL);
        }

        return ret_code;
}

/*===========================================================================

FUNCTION      OEMCamera_InGetParm

DESCRIPTION
              Retrieve control values.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type OEMCamera_InGetParm(camera_parm_type id,camera_parm_info_type *parm)
{
    camera_ret_code_type ret_code = CAMERA_SUCCESS;

    switch(id)
    {
        case CAMERA_PARM_SENSOR_POSITION:// Gemsea031023 changed 
            parm->minimum_value = 0;
            parm->maximum_value = 180;
            parm->step_value = 180;
            parm->default_value = 0;
            parm->current_value = camera_resize.frame.rotate;
            break;

         /* case CAMERA_PARM_SENSOR_MIRROR:// Gemsea031023 changed 
            parm->minimum_value = 0;
            parm->maximum_value = 1;
            parm->step_value = 1;
            parm->default_value = 0;
            parm->current_value = camera_resize.frame.mirror;
            break;*/

        case CAMERA_PARM_BRIGHTNESS:
            parm->minimum_value = 0x00;
            parm->maximum_value = 0xFF;
            parm->step_value = 0x10;
            parm->default_value = 0x60;
            parm->current_value = 0x60;
            break;

        /*case CAMERA_PARM_ZOOM:
            parm->minimum_value = 1;
            parm->maximum_value = camera_resize.zoom_max;
            parm->step_value = 1;
            parm->default_value = 1;
            parm->current_value = camera_resize.zoom;
            break;*///20081128

        case CAMERA_PARM_CONTRAST:
        case CAMERA_PARM_ENCODE_ROTATION:
        //case CAMERA_PARM_ENCODE_MIRROR:
        case CAMERA_PARM_SHARPNESS:
        case CAMERA_PARM_EXPOSURE:
        case CAMERA_PARM_WB:
        case CAMERA_PARM_EFFECT:
        default:
            MSG_ERROR("Not supported parm, %d", id,0,0);
            ret_code = CAMERA_NOT_SUPPORTED;
            break;
        }

        return ret_code;
}
/*===============================================================================              

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
        if (!pList[i].bInUse) 
        {
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
static int OEMCamera_SetSnapshotDestination(OEMCamera * pme, camera_handle_type * ph)
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

/*#ifdef FEATURE_CAMERA7500
        case CAM_ENCODE_YCBCR:
            eFormat = CAMERA_YCBCR_ENCODE;
            break;
#endif*/

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
#endif  /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
        if (pmd->pData)
            STRNCPY(ph->efs.filename, pme->m_pszFile, sizeof(ph->efs.filename));
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

/*This func is to set the camera mode that the camera will be into the db*/
static void OEMCameraSetNextMode(int16 nMode,db_items_value_type db_item)
{
   db_item.nCamMode = nMode;
   db_put(DB_CAMERA_MODE, &db_item);
   
}

