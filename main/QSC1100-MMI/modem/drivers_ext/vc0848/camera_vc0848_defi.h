/*=============================================================================
//           
// 文件:
//        camera_vc0848_defi.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        OEM camera 处理函数相关文件
//                 
//
===============================================================================*/
/*=============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

===============================================================================*/


/*=============================================================================
                            INCLUDE FILES
===============================================================================*/
#include "OEMCamera.h"
#include "OEMObjectMgr.h"

#if defined (FEATURE_BREWAPI_SPECIFIC)
#include "OEMCamera_Priv.h"
#endif
#include "ICameraExt.h"

#include "mmd_ply_driver.h"

//-------------------------------------------------------------------
//      Macros
//-------------------------------------------------------------------
#define FEATURE_CAMERA_ENCODE_PROPERTIES  //define this feature for the encode properties
#define FEATURE_CAMERA_BURST_MODE         //define this feature for multi shot
#define FEATURE_CAMERA_ZOOM

#define CAMERA_MAX_WIDTH   1600
#define CAMERA_MAX_HEIGHT  1200
#define OEMCAMERA_RSP_START_MAX           3  // CAM_CMD_START can have 3 pending callbacks
#define OEMCAMERA_RSP_COMMAND_MAX         5  // CAM_CMD_SETPARM can have 2 pending callbacks (Two sets allowed)
#define OEMCAMERA_RSP_ENCODESNAPSHOT_MAX  1  // CAM_CMD_ENCODESNAPSHOT can have 1 pending callback (One EncodeSnapshot allowed)

#if defined (FEATURE_BREWAPI_SPECIFIC)
#define CAM_CMD_OVERLAY_IMAGE       (CAM_CMD_USER_BASE + 1)
#endif

// The quality param is a percent from 1 to 100
#define OEMCAMERA_QUALITY_MIN_VALUE 1
#define OEMCAMERA_QUALITY_MAX_VALUE 100
// Mapping from old Low, Med, and High enums is: 50, 75 and 90
// percent quality levels respectively
#define OEMCAMERA_QUALITY_LOW       50
#define OEMCAMERA_QUALITY_MED       75
#define OEMCAMERA_QUALITY_HIGH      90

// Camcorder uses the same parm but a different mapping
#define OEMCAMCORDER_QUALITY_MIN_VALUE 0
#define OEMCAMCORDER_QUALITY_LOW       (OEMCAMCORDER_QUALITY_MIN_VALUE)
#define OEMCAMCORDER_QUALITY_MED       (OEMCAMCORDER_QUALITY_MIN_VALUE + 1)
#define OEMCAMCORDER_QUALITY_HIGH      (OEMCAMCORDER_QUALITY_MIN_VALUE + 2)
#define OEMCAMCORDER_QUALITY_USER      (OEMCAMCORDER_QUALITY_MIN_VALUE + 3)
#define OEMCAMCORDER_QUALITY_MAX_VALUE (OEMCAMCORDER_QUALITY_USER)

// Camcorder uses these constants for AAC recording range checking
// These constants map to enums in upper and lower layers
#ifdef FEATURE_AAC_REC
#define OEMCAMCORDER_AAC_MODE_MAX_VALUE 2
#define OEMCAMCORDER_AAC_FREQ_MAX_VALUE 4
#define OEMCAMCORDER_AAC_QUALITY_MAX_VALUE 2
#endif /* FEATURE_AAC_REC */

// The File Size  param is in Kilobytes and could be 1 to 4000
// which translated means 1k bytes to 4 MB.  4 MB allows for 4 Megapixel
// camera operation!  We allow 0 as the minimum because this
// DISABLES the file size control option (ignores the number and
// just encodes to m_nQuality level)
#define OEMCAMERA_FILE_SIZE_DISABLED  0
#define OEMCAMERA_FILE_SIZE_MIN_VALUE 0
#define OEMCAMERA_FILE_SIZE_MAX_VALUE 4000


//-------------------------------------------------------------------
//     Type Declarations
//-------------------------------------------------------------------
typedef struct OEMCamera         OEMCamera;
typedef struct CameraRsp         CameraRsp;

/*==================================================================

  Frame IBitmap

==================================================================*/
typedef enum _camera_encode_type
{
    CAMERA_JPEG, 
    CAMERA_PNG,
    CAMERA_RAW  
} camera_encode_type;

typedef struct 
{
    // Format of the frame 
    camera_encode_type format;
    // Width and height of the picture 
    uint16 dx;
    uint16 dy;
    //it indicates the degree of clockwise rotation that should be
    //applied to obtain the exact view of the captured image. 
    uint16 rotation;
    void * buffer;
}  camera_frame_type;

typedef struct FrameBitmap
{
   IDIB              m_dib;

   int32             m_nRefs;
    camera_frame_type m_rawFrame;
} FrameBitmap;

struct CameraRsp
{
   AEEObjectHandle   hObject;
   AEECameraNotify   camNotify;
   OEMCamera *       pOwner;
   flg               bInUse:1;
};

#if defined (FEATURE_ACM)
typedef enum _tagResourceState
{
    RES_STATE_UNKNOWN,
    RES_STATE_NORES,
    RES_STATE_ACQUIRED,
    RES_STATE_ACQUIRING
}RESOURCESTATE;

typedef struct _tagCameraCallStartStruct
{
  int16 mode;
  uint32 dwParam;
  boolean isValid;
 // This is used instead of mode when ICAMERA_EncodeSnapshot is called.
  boolean encodingSnapshot;
} CameraCallStartStruct;

#endif // FEATURE_ACM

typedef enum  _CHANGE_MODE
{
    CHANGE_NORMAL=0,
    CHANGE_ADD,
    CHANGE_DEC,
    CHANGE_STEP,
    CHANGE_MODE_ERROR = 0x7fffffff
}CHANGE_MODE;
struct OEMCamera
{
    // Pre-allocated callback rsp info.
    CameraRsp         m_rspStart[OEMCAMERA_RSP_START_MAX];
    CameraRsp         m_rspCommand[OEMCAMERA_RSP_COMMAND_MAX];
    CameraRsp         m_rspEncodeSnapshot[OEMCAMERA_RSP_ENCODESNAPSHOT_MAX];

#if defined(FEATURE_BREWAPI_SPECIFIC)
    CameraRsp         m_pngCommand[OEMCAMERA_RSP_COMMAND_MAX];
    AEECameraOverlayImage m_image;
    uint32            m_nOIBufSize;
    uint32            m_nOIBytesRecv;
#endif //defined(FEATURE_BREWAPI_SPECIFIC)

    // User registered callback info.
    PFNCAMERANOTIFY   m_pfnNotify;
    void *            m_pUser;


    // Camera mode.
    RMEDIA_FILE       m_recMediaFile;
    MMD_PLYFILE       m_pfileHandle;

    int16             m_nMode;
    flg               m_bStateChanging:1;
    flg               m_bPaused:1;
    flg               m_bDeferEncode:1;
    int16             m_nPreviewType;
    boolean           m_bPreviewWithFrame;

    // Media data.
    AEEMediaData      m_md;
    char *            m_pszMIME;
    char *            m_pszFile;

    AEECLSID          m_clsVideo;
    uint32            m_dwSubVideo;
    AEECLSID          m_clsAudio;
    uint32            m_dwSubAudio;

    // Frame info.
    AEESize           m_sizeFrame;
    int16             m_nQuality;
    FrameBitmap *     m_pFrame;
//#if defined(FEATURE_CAMERA_V7)
    int32             m_nBitrate;
    uint16            m_nEVFactor;
//#endif // defined(FEATURE_CAMERA_V7)
    byte              m_nFPS;
    camera_frame_type m_rawFrame;
    int32             m_nFileSize;

// Display info: screen rectangle and offset within the frame.
    AEESize           m_sizeDisplay;
    int16             m_cxOffset;
    int16             m_cyOffset;
// the parameter of effect
    int16             m_nEffect;
#ifdef FEATURE_CAMERA_ZOOM
    CHANGE_MODE       m_zoomMode;
    int16             m_nZoomstep;
    uint8*            m_pGetzoomstep; //add 20081217
#endif
    uint8             m_nBrightnessval;
    uint8             m_nSensorMode; 
//file specific info

    int32             m_wShortHeader;

#ifdef FEATURE_VIDEO_MULTI_UUID
    video_uuid_info_type m_uuidInfo;
#else /* FEATURE_VIDEO_MULTI_UUID */
    void *            m_pCpgd;
    uint32            m_nszpCpgd;
    uint8*            m_pUuid_cpgd;
#endif /* FEATURE_VIDEO_MULTI_UUID */

    flg               m_bAudioPostCardRecord:1;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
    //Sensor Info
    uint16            m_sensorId;
#endif

#ifdef FEATURE_AAC_REC
    CameraAACEncodingInfo m_dwSubAACAudio;
#endif /* FEATURE_AAC_REC */

    AEEObjectHandle   m_hObject;
    boolean           m_bRelease;
    /* ACM Resource*/

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
    IAppResCtl*       m_pAppResCtl;
    boolean           m_boACMCreated;
#endif // FEATURE_ODM_UI
    ACM_Callback_data_type  m_stCallBackDataType;
    RESOURCESTATE     m_enResStateCamera;
    CameraCallStartStruct m_cameraCallStartStruct;
    int               m_nResAquired_For_Reason;
    boolean           m_boCameraStoppedDueToACMRelease;
#ifdef FEATURE_CAMERA_MULTI_SENSOR
    AEECLSID          m_sensorCLSID;
#endif // FEATURE_CAMERA_MULTI_SENSOR
#endif // FEATURE_ACM

#ifdef FEATURE_CAMERA_BURST_MODE  
    int32             m_nBracketingMode;
    CameraSnapshotMulti  m_multishotInfo;
    int               m_nShotIndex;
    int               m_nCurrIndex;
#endif //FEATURE_CAMERA_BURST_MODE

    uint8             m_nSpaceLimitThreshold;
#ifdef  FEATURE_CAMERA_LCD_DIRECT_MODE
    boolean           m_bDirectMode;
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
    boolean           m_bPreviewInit;
};



