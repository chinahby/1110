/*============================================================================
//           
// 文件:
//        OEMCameraVC0848.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        OEM camera 处理函数相关文件
//                 
//
==============================================================================*/
/*============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/

/*============================================================================
                         INCLUDE FILES
============================================================================*/
#include"camera_vc0848_defi.h"
#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "AEE_OEMFile.h"
#include "AEEMediaFormats.h"
#include "AEEPosDet.h"
#include "AEEStdLib.h"
#include "mmd_user_depend.h"
/*============================================================================
                         CONSTANT DEFINITIONS
============================================================================*/

#define CAMERA_QUALITY_STEP   25
#define CAMERA_PREVIEW_WIDTH  176
#define CAMERA_PREVIEW_HEIGHT 132
#define CAMERA_VEDIO_FRAME    15



 typedef enum 
{
  CAMERA_SUCCESS ,
  CAMERA_INVALID_STATE,
  CAMERA_INVALID_PARM,
  CAMERA_INVALID_FORMAT,
  CAMERA_NO_SENSOR,
  CAMERA_NO_PICTURE,
  CAMERA_NO_MEMORY,
  CAMERA_NOT_SUPPORTED,
  CAMERA_FAILED,
  CAMERA_RET_CODE_MAX
} camera_ret_code_type;

typedef enum  
{
  EDSP_DISPLAY_MODE_NORMAL = 0,
  EDSP_DISPLAY_MODE_PIP,
  EDSP_DISPLAY_MODE_PIP_WITH_OVERLAY,
  EDSP_DISPLAY_MODE_RESERVED
} edsp_display_mode_type;


typedef enum
{
  CAMERA_DEVICE_MEM,
  CAMERA_DEVICE_EFS,
  CAMERA_DEVICE_MAX
} camera_device_type;


typedef enum 
{
  CAMERA_FUNC_START,
  CAMERA_FUNC_STOP,
  CAMERA_FUNC_START_PREVIEW,
  CAMERA_FUNC_TAKE_PICTURE,
  CAMERA_FUNC_ENCODE_PICTURE,
  CAMERA_FUNC_START_RECORD,
  CAMERA_FUNC_SET_OVERLAY,
  CAMERA_FUNC_CLR_OVERLAY,
  CAMERA_FUNC_SET_POSITION,
  CAMERA_FUNC_SET_STRING,
  CAMERA_FUNC_SET_PARM,
  CAMERA_FUNC_MAX,
  /* The followings are for internal use only */
  CAMERA_FUNC_STOP_PREVIEW,
  CAMERA_FUNC_RELEASE_PICTURE,
  CAMERA_FUNC_PAUSE_RECORD,
  CAMERA_FUNC_RESUME_RECORD,
  CAMERA_FUNC_STOP_RECORD,
  CAMERA_FUNC_ENABLE_FRAME_CALLBACK,
  CAMERA_FUNC_DISABLE_FRAME_CALLBACK,
  CAMERA_FUNC_RELEASE_FRAME,
  CAMERA_FUNC_BLT,
  CAMERA_FUNC_SET_FAILED,
  CAMERA_FUNC_SET_ZOOM,
#ifdef FEATURE_VIDEO_ENCODE
  CAMERA_FUNC_VIDEO_ENGINE_CB,
  CAMERA_FUNC_VIDEO_HANDSHAKE
#endif /* FEATURE_VIDEO_ENCODE */
} camera_func_type;


typedef enum 
{
  CAMERA_ORIENTATION_LANDSCAPE,
  CAMERA_ORIENTATION_PORTRAIT
} camera_orientation_type;

typedef enum 
{
  CAMERA_RSP_CB_SUCCESS,    /* Function is accepted         */
  CAMERA_EXIT_CB_DONE,      /* Function is executed         */
  CAMERA_EXIT_CB_FAILED,    /* Execution failed or rejected */
  CAMERA_EXIT_CB_DSP_IDLE,  /* DSP is in idle state         */
  CAMERA_EXIT_CB_DSP_ABORT, /* Abort due to DSP failure     */
  CAMERA_EXIT_CB_ABORT,     /* Function aborted             */
  CAMERA_EXIT_CB_ERROR,     /* Failed due to resource       */
  CAMERA_EVT_CB_FRAME,      /* Preview or video frame ready */
  CAMERA_CB_MAX
} camera_cb_type;

typedef struct 
{
  word  x;
  word  y;
  word  width;
  word  height;
  word  scale;
  word  rotate;
  boolean  mirror;
} edsp_resize_info_type;

typedef struct 
{
  edsp_resize_info_type  frame;
  edsp_resize_info_type  display;
  uint16                fps;
  uint16                rotate;
  boolean               mirror;
  uint16                zoom;
  uint16                zoom_max;
} camera_resize_type;

typedef void (*camera_cb_f_type)
(
    camera_cb_type cb,
    const void *client_data,
    camera_func_type func,
    int32 parm4
);
typedef struct 
{
  /* Width and height of the sensor */
  uint16 sensor_width;
  uint16 sensor_height;
  /* Width and height of the display */
  uint16 display_width;
  uint16 display_height;
  /* Start X and Start Y of the display */
  uint16 display_start_x;
  uint16 display_start_y;
  /* Frames per second */
  uint16 fps;
  /* Whether the device driver can sense when sensor is rotated */
  boolean sensor_rotation_sensing;
  /* How the sensor are installed */
  uint16 default_rotation;
  camera_orientation_type default_orientation;
  boolean mirror;
} camera_info_type;

typedef struct
{
    int32 minimum_value; /* Minimum allowed value */
    int32 maximum_value; /* Maximum allowed value */
    int32 step_value;    /* step value */
    int32 default_value; /* Default value */
    int32 current_value; /* Current value */
} camera_parm_info_type;

typedef enum _camera_parm_type
{
    CAMERA_PARM_AUDIO_FMT,
    CAMERA_PARM_SPACE_LIMIT,
    CAMERA_PARM_REFLECT,
    CAMERA_PARM_PREVIEW_MODE,
    CAMERA_PARM_PREVIEW_FPS,
    CAMERA_PARM_EXPOSURE_COMPENSATION,
    CAMERA_PARM_AUTO_EXPOSURE_MODE,
    CAMERA_PARM_ISO,
    CAMERA_PARM_LUMA_ADAPTATION,
    CAMERA_PARM_STROBE_FLASH,
    CAMERA_PARM_FLASH,
    CAMERA_PARM_AF_MODE,
    CAMERA_PARM_FPS,
    CAMERA_PARM_CONTRAST,
    CAMERA_PARM_BRIGHTNESS,
    CAMERA_PARM_SHARPNESS,
    CAMERA_PARM_HUE,
    CAMERA_PARM_SATURATION,
    CAMERA_PARM_ZOOM,
    CAMERA_PARM_FRAME_TIMESTAMP,
    CAMERA_PARM_NIGHTSHOT_MODE,
    CAMERA_PARM_ANTIBANDING,
    CAMERA_PARM_SENSOR_POSITION,
    CAMERA_PARM_ENCODE_ROTATION,
    CAMERA_PARM_EXPOSURE,
    CAMERA_PARM_WB,
    CAMERA_PARM_EFFECT,
    CAMERA_PARM_RED_EYE_REDUCTION,
    CAMERA_PARM_FADING,
    CAMERA_PARM_TRANSITION,
    CAMERA_PARM_FOCUS_RECT,
    CAMERA_PARM_BESTSHOT_MODE,
    CAMERA_PARM_HISTOGRAM,
    CAMERA_PARM_MAX
}camera_parm_type;

typedef enum
{
    CAMERA_QUALITY_LOWEST = 1,
    CAMERA_QUALITY_LOW = 25,
    CAMERA_QUALITY_MEDIUM = 50,
    CAMERA_QUALITY_HIGH = 75,
    CAMERA_QUALITY_HIGHEST = 100
} camera_quality_type;

typedef struct
{
    camera_device_type     device;
    camera_quality_type    quality;
    camera_encode_type     format;
    char                   filename[FILE_LEN_UNICODE];
} camera_handle_efs_type;

typedef struct 
{
    camera_device_type     device;
    camera_quality_type    quality;
    camera_encode_type     format;
    uint8                  *buffer;
    uint32                 length;
} camera_handle_mem_type;

typedef union
{
    camera_device_type      device;
    camera_handle_mem_type  mem;
    camera_handle_efs_type  efs;
} camera_handle_type;
